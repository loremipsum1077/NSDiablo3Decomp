#!/usr/bin/env python3
"""Generate a reference copy of every original source file from the DWARF,
under build/dwarf/headers/, mirroring the original tree.

Each generated file holds, in original line order:
  - the types declared in that file: structs, classes, unions, enums and
    typedefs with member offsets, base classes, access, virtual slots and
    method declarations (with parameter names where a definition has them)
  - #includes and forward declarations worked out from the types it uses
  - a list of the functions defined in the file with their full signatures,
    address (or, for inline functions, how many functions they were inlined
    into) and of the variables defined there

Function bodies are not in the DWARF. Template classes appear only as their
instantiations; one instantiation per template is emitted inside `#if 0`.

  python3 tools/gen_headers.py                       # regenerate everything
  python3 tools/gen_headers.py --adopt X/Sigma/Math/Sphere.h
                                                     # copy into src/ to work on

Files in src/ are hand-maintained once adopted; --adopt never overwrites.
"""
import argparse
import posixpath
import re
import shutil
import sys
from collections import defaultdict
from pathlib import Path

import dwarf_index
from dwarf_index import is_generated_file, strip_targs

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / "build/dwarf/headers"
ACCESS = {"DW_ACCESS_public": "public", "DW_ACCESS_protected": "protected", "DW_ACCESS_private": "private"}
NO_RET = re.compile(r"^operator (?!new|delete)[A-Za-z_]")


def declare(type_str, name):
    """C declarator: put the name inside function-pointer and array types."""
    if not name:
        return type_str
    m = re.search(r"\((?:[\w:<>, ]*::)?[*&]+(?=[\[\)])", type_str)
    if m:
        return type_str[:m.end()] + name + type_str[m.end():]
    m = re.search(r"((?:\[\d*\])+)$", type_str)
    if m:
        return f"{type_str[:m.start()].rstrip()} {name}{m.group(1)}"
    return f"{type_str} {name}"


def external_include(path):
    """#include line for a header outside the game tree (libc, libc++, SDK)."""
    low = path.lower()
    if "/include/bits/" in low:
        return "#include <stddef.h>\n#include <stdint.h>"
    for marker, comment in (("/include/c++/v1/", ""), ("/aarch64/include/", ""),
                            ("nintendosdk/include/", "  // Nintendo SDK"), ("/noarch/include/", "  // protobuf")):
        at = low.find(marker)
        if at >= 0:
            tail = path[at + len(marker):]
            if not comment and not tail.startswith("__"):
                return f"#include <{tail}>"
            return f"// #include <{tail}>{comment}"
    return f"// external: {path}"


class Generator:
    def __init__(self, index):
        # Consumers must not inherit worker completion order, including when
        # choosing among duplicate names or case variants of Windows paths.
        self.types = dict(sorted(index["types"].items()))
        self.funcs = dict(sorted(index["funcs"].items()))
        self.inlined = index["inlined"]
        self.vars = dict(sorted(index["vars"].items()))
        self.by_linkage = {f["linkage"]: f for f in self.funcs.values() if f["linkage"]}
        self.by_name = {}
        for key, rec in self.types.items():
            old = self.by_name.get(key[0])
            if old is None or (not self.types[old]["members"] and rec["members"]):
                self.by_name[key[0]] = key
        self.anon_used = set()
        for rec in self.types.values():
            if rec["anon"]:
                self.anon_used.add(rec["anon"])
            for m in rec["members"]:
                if m["anon"]:
                    self.anon_used.add(m["anon"])

    # ------------------------------------------------------------ types

    def body(self, key, pad):
        rec = self.types[key]
        kind = rec["kind"]
        if kind == "enum":
            return [f"{pad}    {n} = {v}," for n, v in rec["enumerators"]]
        items = []
        for m in rec["members"]:
            items.append((m["line"], 0, "member", m))
        for _, m in sorted(rec["methods"].items()):
            items.append((m["line"], 1, "method", m))
        for inner in rec["inner"]:
            if inner not in self.anon_used and inner in self.types:
                items.append((self.types[inner]["line"], 0, "type", inner))
        items.sort(key=lambda i: (i[0], i[1]))
        lines = []
        access = "private" if kind == "class" else "public"
        default = access
        cls = strip_targs(rec["name"])
        for _, _, what, item in items:
            want = default if what == "type" else ACCESS.get(item["access"], default)
            if what != "type" and want != access:
                access = want
                lines.append(f"{pad}{access}:")
            if what == "type":
                lines += self.emit(item, pad + "    ")
            elif what == "member":
                lines += self.member(item, pad + "    ")
            else:
                lines.append(self.method(item, cls, pad + "    "))
        return lines

    def member(self, m, pad):
        if m["name"].startswith("_vptr$"):
            return [f"{pad}// vtable pointer"]
        note = f"  // 0x{m['offset']:02x}" if m["offset"] is not None else ""
        if m["anon"] and m["anon"] in self.types:
            lines = self.emit(m["anon"], pad, declarator=m["name"], type_str=m["type"])
            lines[-1] += note
            return lines
        text = declare(m["type"], m["name"])
        if m["bits"]:
            text += f" : {int(m['bits'], 16)}"
        if m["static"]:
            text = "static " + text + (f" = {m['value']}" if m["value"] is not None else "")
        return [f"{pad}{text};{note}"]

    def method(self, m, cls, pad):
        params = m["params"]
        definition = self.by_linkage.get(m["linkage"])
        if definition and len(definition["params"]) == len(params):
            params = definition["params"]
        args = ", ".join(declare(t, n) if t != "..." else "..." for t, n in params)
        name = m["name"]
        special = strip_targs(name) in (cls, "~" + cls) or NO_RET.match(name)
        text = "" if special else m["ret"] + " "
        text = f"{text}{name}({args})"
        if m["const"]:
            text += " const"
        if "pure" in m["virtual"]:
            text += " = 0"
        if m["virtual"]:
            text = "virtual " + text
        elif m["static"]:
            text = "static " + text
        if m["explicit"]:
            text = "explicit " + text
        note = f"  // vtable slot {m['slot']}" if m["slot"] is not None else ""
        if m["template"]:
            return f"{pad}// template instantiation: {text};"
        return f"{pad}{text};{note}"

    def emit(self, key, pad="", declarator=None, type_str=None):
        """Lines declaring one type. declarator: emit as `struct {...} name;`."""
        rec = self.types[key]
        kind, name = rec["kind"], rec["name"]
        if kind == "typedef":
            if rec["anon"] and rec["anon"] in self.types:
                lines = self.emit(rec["anon"], pad, declarator=name, type_str=rec["target"])
                lines[0] = lines[0].replace(pad, pad + "typedef ", 1)
                return lines
            return [f"{pad}typedef {declare(rec['target'], name)};"]
        head = kind
        if kind == "enum" and rec["enum_class"]:
            head = "enum class"
        if name and declarator is None:
            head += " " + name
        if kind == "enum" and rec["enum_class"] and rec["underlying"]:
            head += f" : {rec['underlying']}"
        if rec["bases"]:
            head += " : " + ", ".join(
                f"{ACCESS.get(a, 'private' if kind == 'class' else 'public')} {'virtual ' if v else ''}{t}"
                for t, a, v in rec["bases"])
        note = []
        if rec["size"] is not None and kind != "enum":
            note.append(f"size 0x{rec['size']:x}")
        if kind == "enum" and rec["underlying"] and not rec["enum_class"]:
            note.append(f"underlying type {rec['underlying']}")
        lines = [f"{pad}{head} {{" + (f"  // {', '.join(note)}" if note else "")]
        lines += self.body(key, pad)
        tail = ""
        if declarator is not None:
            dims = re.search(r"((?:\[\d*\])+)$", type_str or "")
            tail = (" " + declarator if declarator else "") + (dims.group(1) if dims else "")
        lines.append(f"{pad}}}{tail};")
        return lines

    def primary(self, rec):
        """Forward declaration of a class template, from one instantiation's parameters."""
        params = []
        for tag, name, type_s, _ in rec["tparams"]:
            if tag == "template_type_parameter":
                params.append(f"typename {name}".strip())
            elif tag == "template_value_parameter":
                params.append(f"{type_s} {name}".strip())
            elif tag == "GNU_template_parameter_pack":
                params.append(f"typename... {name}".strip())
            else:
                return None
        return f"template <{', '.join(params)}> {rec['kind']} {strip_targs(rec['name'])};  // defined by hand"

    # ------------------------------------------------------------ files

    def all_deps(self, key, seen):
        rec = self.types[key]
        deps = set(rec["deps"])
        for inner in rec["inner"] + [rec["anon"]] + [m["anon"] for m in rec["members"]]:
            if inner and inner in self.types and inner not in seen:
                seen.add(inner)
                deps |= self.all_deps(inner, seen)
        return deps

    def includes(self, path, keys):
        includes, external, forwards = set(), set(), set()
        local = {self.types[k]["name"] and k[0] for k in keys}
        for key in keys:
            for qualname, kind, by_value in self.all_deps(key, {key}):
                target = self.by_name.get(qualname)
                if target is None:
                    if kind in ("struct", "class") and "<" not in qualname:
                        forwards.add((qualname, kind))
                    continue
                rec = self.types[target]
                if rec["file"] == path:
                    if not by_value and not rec["nested"] and not rec["template"] and qualname not in local:
                        forwards.add((qualname, kind))
                    continue
                if by_value or rec["nested"] or rec["template"] or kind not in ("struct", "class"):
                    (includes if is_generated_file(rec["file"]) else external).add(rec["file"])
                else:
                    forwards.add((qualname, rec["kind"]))
        forwards = {(q, k) for q, k in forwards
                    if q not in self.by_name or self.types[self.by_name[q]]["file"] not in includes}
        lines = [f'#include "{posixpath.relpath(f, posixpath.dirname(path))}"' for f in sorted(includes)]
        lines += sorted({external_include(f) for f in external if f})
        if lines:
            lines.append("")
        for qualname, kind in sorted(forwards):
            *scope, name = qualname.split("::")
            opened = "".join(f"namespace {s} {{ " for s in scope)
            lines.append(f"{opened}{kind} {name};{' }' * len(scope)}")
        if forwards:
            lines.append("")
        return lines

    def signature(self, f):
        scope = "::".join(f["scope"])
        args = ", ".join(declare(t, n) if t != "..." else "..." for t, n in f["params"])
        cls = strip_targs(f["scope"][-1]) if f["scope"] else ""
        special = f["in_class"] and (strip_targs(f["name"]) in (cls, "~" + cls) or NO_RET.match(f["name"]))
        text = ("" if special else f["ret"] + " ") + (scope + "::" if scope else "") + f"{f['name']}({args})"
        if f["const"]:
            text += " const"
        if not f["external"] and not f["in_class"]:
            text = "static " + text
        return text

    def render(self, path, keys, funcs, variables):
        lines = ["// Generated from the DWARF by tools/gen_headers.py -- reference only.",
                 f"// Original file: {path}", ""]
        if not path.endswith((".cpp", ".c", ".cc", ".inl")):
            lines += ["#pragma once", ""]
        lines += self.includes(path, keys)

        scope = ()
        templates = defaultdict(list)
        for key in sorted(keys, key=lambda k: (self.types[k]["line"], k[0])):
            rec = self.types[key]
            if rec["template"]:
                group = templates[(rec["line"], strip_targs(key[0]))]
                group.append(key)
                if len(group) > 1 or rec["nested"]:
                    continue
            if rec["scope"] != scope:
                lines += [f"}}  // namespace {s}" for s in reversed(scope)]
                lines += [f"namespace {s} {{" for s in rec["scope"]]
                scope = rec["scope"]
            if rec["template"]:
                primary = self.primary(rec)
                if primary:
                    lines += [primary, ""]
                continue
            lines += self.emit(key)
            if rec["name"] and rec["size"] is not None and rec["kind"] != "enum":
                lines.append(f'static_assert(sizeof({rec["name"]}) == 0x{rec["size"]:x}, "{rec["name"]}");')
            lines.append("")
        lines += [f"}}  // namespace {s}" for s in reversed(scope)]

        if templates:
            lines += ["", "// Templates. The DWARF only has instantiations; one of each is shown.", "#if 0"]
            for (line, name), insts in sorted(templates.items()):
                first = self.types[insts[0]]
                params = ", ".join(f"{n} = {v or t}" for _, n, t, v in first["tparams"])
                lines.append(f"// {name} (line {line}): {len(insts)} instantiation(s); shown with {params}")
                lines += self.emit(insts[0])
                lines.append("")
            lines.append("#endif")

        if funcs:
            lines += ["", "// Functions defined in this file (bodies are not in the DWARF):"]
            for f in sorted(funcs, key=lambda f: (f["line"], f["name"])):
                key = (f["file"], f["line"], strip_targs(f["name"]))
                where = []
                if f["addrs"]:
                    addrs = sorted(f["addrs"])
                    where.append(f"0x{0x7100000000 | addrs[0]:x}" + (f" (+{len(addrs) - 1} more)" if addrs[1:] else ""))
                if key in self.inlined:
                    where.append(f"inlined into {len(self.inlined[key][0])} functions")
                elif f["inline"] and not f["addrs"]:
                    where.append("inline")
                lines.append(f"//   line {f['line']:5}  {self.signature(f)}  [{', '.join(where)}]")
        if variables:
            lines += ["", "// Variables defined in this file:"]
            for v in sorted(variables, key=lambda v: v["line"]):
                scope_s = "::".join(v["scope"])
                lines.append(f"//   line {v['line']:5}  {'' if v['external'] else 'static '}"
                             f"{declare(v['type'], (scope_s + '::' if scope_s else '') + v['name'])}")
        return "\n".join(lines) + "\n"

    def run(self, out):
        types, funcs, variables = defaultdict(list), defaultdict(list), defaultdict(list)
        # The originals were built on Windows: X/Server/AI and X/Server/Ai are one directory.
        canonical = {}

        def canon(path):
            return canonical.setdefault(path.lower(), path)

        for key, rec in self.types.items():
            if not rec["nested"] and key not in self.anon_used and is_generated_file(rec["file"]):
                types[canon(rec["file"])].append(key)
        for f in self.funcs.values():
            if is_generated_file(f["file"]):
                funcs[canon(f["file"])].append(f)
        for v in self.vars.values():
            if is_generated_file(v["file"]):
                variables[canon(v["file"])].append(v)
        paths = sorted(set(types) | set(funcs) | set(variables))
        shutil.rmtree(out, ignore_errors=True)
        for path in paths:
            target = out / path
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(self.render(path, types[path], funcs[path], variables[path]))
        return paths


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--adopt", nargs="+", metavar="PATH",
                    help="copy these generated files (paths as in the original tree) into src/")
    ap.add_argument("--out", type=Path, default=OUT)
    args = ap.parse_args()
    sys.setrecursionlimit(10000)

    if not args.adopt or not args.out.is_dir():
        paths = Generator(dwarf_index.load()).run(args.out)
        print(f">>> wrote {len(paths)} files to {args.out}")
    for path in args.adopt or []:
        src, dst = args.out / path, ROOT / "src" / path
        if not src.is_file():
            sys.exit(f"{path}: not generated (no such original file in the DWARF)")
        if dst.exists():
            print(f"skip {path}: src/{path} already exists")
            continue
        dst.parent.mkdir(parents=True, exist_ok=True)
        text = src.read_text().replace(
            "// Generated from the DWARF by tools/gen_headers.py -- reference only.\n",
            "// Adopted from tools/gen_headers.py output; hand-maintained from here on.\n")
        dst.write_text(text)
        print(f"adopted src/{path}")


if __name__ == "__main__":
    main()
