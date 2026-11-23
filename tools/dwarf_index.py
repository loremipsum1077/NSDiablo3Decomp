#!/usr/bin/env python3
"""Read all of the .nss DWARF once and cache what the other tools need in
build/dwarf/index.pkl: every type (deduplicated across compile units), every
function definition with its parameter names, every global variable, and how
often each inline function was inlined.

  python3 tools/dwarf_index.py          # build the cache (about a minute)

Used as a library by tools/gen_headers.py and tools/inline_rank.py.
"""
import argparse
import hashlib
import multiprocessing
import os
import pickle
import posixpath
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
NSS = ROOT / "build/romfs/DiabloIIINX64r.nss"
CACHE = ROOT / "build/dwarf/index.pkl"

PREFIXES = (
    ("D:/BuildServer/2/work/code/branches/2_6_2/", ""),
    ("D:/home/teamcity2/work/sdk/", "NintendoSDK/src/"),
    ("C:/Nintendo/DiabloIII/NintendoSDK/", "NintendoSDK/"),
    ("C:/Projects/D3/Main/Trunk/", ""),
    ("C:/dev/d3/main/trunk/", ""),
    ("C:/dev/d3/trunk/", ""),
    ("C:/Project/", ""),
    ("c:/package_cache/", "NintendoSDK/package_cache/"),
)
KEEP = {
    "name", "type", "decl_file", "decl_line", "byte_size", "data_member_location", "accessibility",
    "virtuality", "vtable_elem_location", "declaration", "external", "artificial", "linkage_name",
    "const_value", "abstract_origin", "specification", "low_pc", "inline", "call_line", "bit_size",
    "explicit", "enum_class", "count", "upper_bound",
}
TYPE_TAGS = {"structure_type": "struct", "class_type": "class", "union_type": "union",
             "enumeration_type": "enum", "typedef": "typedef"}
SCOPE_TAGS = ("namespace", "structure_type", "class_type", "union_type")
WRAPPERS = ("const_type", "volatile_type", "array_type", "restrict_type")
INDIRECT = ("pointer_type", "reference_type", "rvalue_reference_type", "ptr_to_member_type")

_path_cache = {}


def norm_path(raw):
    """Build-machine path -> path relative to the original source root."""
    p = _path_cache.get(raw)
    if p is None:
        p = posixpath.normpath(re.sub(r"/+", "/", raw.replace("\\", "/")))
        for prefix, repl in PREFIXES:
            if p.lower().startswith(prefix.lower()):
                p = repl + p[len(prefix):]
                break
        _path_cache[raw] = p
    return p


def is_game_file(path):
    return bool(path) and not path.startswith(("NintendoSDK/", "/", "..")) and ":" not in path


def is_generated_file(path):
    """Files gen_headers.py writes: the game tree plus the SDK headers it uses."""
    return is_game_file(path) or path.startswith("NintendoSDK/Include/")


def strip_targs(name):
    """blz::vector<int, blz::allocator<int> >::size -> blz::vector::size"""
    out, depth = [], 0
    i = 0
    while i < len(name):
        c = name[i]
        if name.startswith("operator", i) and depth == 0:
            m = re.match(r"operator\s*(<<=|<<|<=|<|>>=|>>|>=|>|->\*|->|\(\)|\[\])?", name[i:])
            out.append(m.group(0))
            i += len(m.group(0))
            continue
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif depth == 0:
            out.append(c)
        i += 1
    return "".join(out)


# ---------------------------------------------------------------- parsing

def read_units(worker, workers):
    """Yield {offset: die} for every worker-th compile unit of the dump."""
    exe = shutil.which("llvm-dwarfdump") or "/opt/homebrew/opt/llvm/bin/llvm-dwarfdump"
    proc = subprocess.Popen([exe, "--debug-info", str(NSS)], stdout=subprocess.PIPE,
                            stderr=subprocess.DEVNULL, text=True, errors="replace", bufsize=1 << 20)
    dies, stack, cur, mine, unit = {}, [], None, False, -1
    for line in proc.stdout:
        if line[0] == "0":
            colon = line.find(":")
            rest = line[colon + 2:]
            body = rest.lstrip(" ")
            if not body.startswith("DW_TAG_"):
                cur = None
                continue
            depth = (len(rest) - len(body)) // 2
            if depth == 0:
                if mine and dies:
                    yield dies
                unit += 1
                mine = unit % workers == worker
                dies, stack = {}, []
            if not mine:
                continue
            cur = {"o": int(line[:colon], 16), "t": body[7:].strip(), "k": [], "p": None}
            dies[cur["o"]] = cur
            del stack[depth:]
            if stack:
                cur["p"] = stack[-1]
                stack[-1]["k"].append(cur)
            stack.append(cur)
        elif mine and cur is not None:
            s = line.lstrip()
            if s.startswith("DW_AT_"):
                tab = s.find("\t")
                key = s[6:tab]
                if key in KEEP and key not in cur:
                    cur[key] = s[tab + 2:s.rfind(")")]
    if mine and dies:
        yield dies
    if proc.wait():
        raise subprocess.CalledProcessError(proc.returncode, proc.args)


def ref(dies, die, key):
    v = die.get(key)
    return dies.get(int(v.split(" ", 1)[0], 16)) if v else None


def type_str(die):
    v = die.get("type")
    if not v:
        return "void"
    return v[v.find('"') + 1:v.rfind('"')]


def name_of(die):
    return die.get("name", "").strip('"')


def chain(dies, die):
    """die, then its abstract origin / specification targets."""
    seen = 0
    while die is not None and seen < 4:
        yield die
        die = ref(dies, die, "abstract_origin") or ref(dies, die, "specification")
        seen += 1


def attr(dies, die, key):
    for d in chain(dies, die):
        if key in d:
            return d[key]
    return None


def scope_of(dies, die):
    """Names of the namespaces and classes enclosing a DIE's declaration."""
    decl = list(chain(dies, die))[-1]
    names = []
    p = decl["p"]
    while p is not None:
        if p["t"] in SCOPE_TAGS:
            names.append(name_of(p) or "(anonymous)")
        p = p["p"]
    return tuple(reversed(names))


def file_line(dies, die):
    f = attr(dies, die, "decl_file")
    line = attr(dies, die, "decl_line")
    return (norm_path(f.strip('"')) if f else "", int(line) if line else 0)


def resolve_type(dies, die):
    """Follow a DW_AT_type to the named type it depends on.
    Returns (target die or None, True if only reached through a pointer/reference)."""
    indirect = False
    t = ref(dies, die, "type")
    for _ in range(12):
        if t is None:
            return None, indirect
        if t["t"] in INDIRECT:
            indirect = True
        elif t["t"] not in WRAPPERS:
            return t, indirect
        t = ref(dies, t, "type")
    return None, indirect


def qual(dies, die):
    return "::".join(scope_of(dies, die) + (name_of(die),))


def add_dep(dies, owner, die, by_value_ok=True):
    target, indirect = resolve_type(dies, die)
    if target is None or target["t"] not in TYPE_TAGS or not name_of(target):
        return None
    kind = TYPE_TAGS[target["t"]]
    by_value = by_value_ok and not indirect or kind in ("typedef", "enum")
    owner["deps"].add((qual(dies, target), kind, by_value))
    return target


def anon_target(dies, die):
    target, indirect = resolve_type(dies, die)
    if target is not None and target["t"] in TYPE_TAGS and target["t"] != "typedef" and not name_of(target):
        return target
    return None


def params_of(dies, die, with_names):
    out, const, static = [], False, True
    for k in die["k"]:
        if k["t"] == "formal_parameter":
            if "artificial" in attr_keys(dies, k):
                static = False
                const = type_str_of(dies, k).startswith("const ")
                continue
            out.append((type_str_of(dies, k), (attr(dies, k, "name") or "").strip('"') if with_names else ""))
        elif k["t"] == "unspecified_parameters":
            out.append(("...", ""))
    return out, const, static


def attr_keys(dies, die):
    keys = set()
    for d in chain(dies, die):
        keys.update(d)
    return keys


def type_str_of(dies, die):
    for d in chain(dies, die):
        if "type" in d:
            return type_str(d)
    return "void"


def is_template(die):
    return any(k["t"].startswith("template_") or k["t"] == "GNU_template_parameter_pack" for k in die["k"])


def build_type(dies, die, index):
    """Record a struct/class/union/enum/typedef DIE (and what it nests)."""
    kind = TYPE_TAGS[die["t"]]
    if "declaration" in die and kind != "typedef":
        return None
    path, line = file_line(dies, die)
    scope = scope_of(dies, die)
    name = name_of(die)
    key = ("::".join(scope + (name or f"(anonymous@{line})",)), path, line)
    parent = die["p"]
    rec = {
        "kind": kind, "name": name, "scope": scope, "file": path, "line": line,
        "size": int(die["byte_size"], 16) if "byte_size" in die else None,
        "nested": parent is not None and parent["t"] in TYPE_TAGS,
        "template": is_template(die), "deps": set(),
        "bases": [], "members": [], "methods": {}, "inner": [], "enumerators": [],
        "target": None, "anon": None, "enum_class": "enum_class" in die,
        "underlying": type_str(die) if kind == "enum" and "type" in die else None,
        "tparams": [(k["t"], name_of(k), type_str(k) if "type" in k else "", k.get("const_value", ""))
                    for k in die["k"] if "template" in k["t"]],
    }
    if kind == "typedef":
        rec["target"] = type_str(die)
        anon = anon_target(dies, die)
        if anon is not None:
            rec["anon"] = build_type(dies, anon, index)
        else:
            add_dep(dies, rec, die)
    for k in die["k"]:
        t = k["t"]
        if t == "enumerator":
            rec["enumerators"].append((name_of(k), k.get("const_value", "")))
        elif t == "inheritance":
            add_dep(dies, rec, k)
            rec["bases"].append((type_str(k), k.get("accessibility", ""), "virtuality" in k))
        elif t == "member":
            anon = anon_target(dies, k)
            if anon is None:
                add_dep(dies, rec, k)
            loc = k.get("data_member_location")
            rec["members"].append({
                "name": name_of(k), "type": type_str(k), "line": int(k.get("decl_line", 0)),
                "offset": int(loc, 16) if loc and loc.startswith("0x") else None,
                "access": k.get("accessibility", ""), "bits": k.get("bit_size"),
                "static": "external" in k or "declaration" in k, "value": k.get("const_value"),
                "anon": build_type(dies, anon, index) if anon is not None else None,
            })
        elif t == "subprogram":
            if "artificial" in k:
                continue
            for p in k["k"]:
                if p["t"] == "formal_parameter":
                    add_dep(dies, rec, p, by_value_ok=False)
            add_dep(dies, rec, k, by_value_ok=False)
            params, const, static = params_of(dies, k, False)
            slot = re.search(r"0x[0-9a-f]+", k.get("vtable_elem_location", ""))
            mkey = (name_of(k), k.get("linkage_name", ""), int(k.get("decl_line", 0)))
            merge_record(rec["methods"], mkey, {
                "name": name_of(k), "linkage": k.get("linkage_name", "").strip('"'),
                "line": int(k.get("decl_line", 0)), "ret": type_str(k), "params": params,
                "const": const, "static": static, "access": k.get("accessibility", ""),
                "virtual": k.get("virtuality", ""), "slot": int(slot.group(0), 16) if slot else None,
                "explicit": "explicit" in k, "template": is_template(k),
            })
        elif t in TYPE_TAGS:
            inner = build_type(dies, k, index)
            if inner is not None:
                rec["inner"].append(inner)
    merge_type(index["types"], key, rec)
    return key


def func_key(dies, die):
    path, line = file_line(dies, die)
    return (path, line, strip_targs(name_of(list(chain(dies, die))[-1])))


def build_func(dies, die, index):
    """Record a function definition (out of line, or the abstract copy of an inline one)."""
    low = die.get("low_pc")
    addr = int(low, 16) if low else 0
    if not addr and "inline" not in die:
        return
    path, line = file_line(dies, die)
    name = (attr(dies, die, "name") or "").strip('"')
    if not name:
        return
    key = func_key(dies, die)
    origin = ref(dies, die, "abstract_origin") or die
    params, const, static = params_of(dies, origin, True)
    scope = scope_of(dies, die)
    rec = {
        "name": name, "scope": scope, "file": path, "line": line,
        "ret": type_str_of(dies, die), "params": params, "const": const,
        "linkage": (attr(dies, die, "linkage_name") or "").strip('"'),
        "member": bool(scope) and static is False, "static_member": False,
        "external": "external" in attr_keys(dies, die),
        "inline": "inline" in attr_keys(dies, die), "addrs": {addr} if addr else set(),
        "locals": [],
    }
    decl = list(chain(dies, die))[-1]
    rec["in_class"] = decl["p"] is not None and decl["p"]["t"] in TYPE_TAGS
    rec["static_member"] = rec["in_class"] and static
    merge_func(index["funcs"], key, rec)
    if addr:
        count_inlined(dies, die, addr, index)


def count_inlined(dies, die, addr, index):
    for k in die["k"]:
        if k["t"] == "inlined_subroutine":
            origin = ref(dies, k, "abstract_origin")
            if origin is not None:
                stat = index["inlined"].setdefault(func_key(dies, origin), [set(), 0])
                stat[0].add(addr)
                stat[1] += 1
            count_inlined(dies, k, addr, index)
        elif k["t"] == "lexical_block":
            count_inlined(dies, k, addr, index)


def walk(dies, die, index):
    for k in die["k"]:
        t = k["t"]
        if t == "namespace":
            walk(dies, k, index)
        elif t in TYPE_TAGS:
            build_type(dies, k, index)
            if t != "typedef":
                for m in k["k"]:
                    if m["t"] == "subprogram" and "inline" in m:
                        build_func(dies, m, index)
        elif t == "subprogram":
            build_func(dies, k, index)
        elif t == "variable" and "specification" not in k:
            path, line = file_line(dies, k)
            name = name_of(k)
            if name and path:
                merge_record(index["vars"], (path, line, name), {
                    "name": name, "scope": scope_of(dies, k), "type": type_str(k), "file": path,
                    "line": line, "external": "external" in k,
                    "linkage": k.get("linkage_name", "").strip('"')})


def new_index():
    return {"types": {}, "funcs": {}, "vars": {}, "inlined": {}, "units": []}


def stable_key(value):
    """A total ordering of index values, independent of dict/set insertion order."""
    if isinstance(value, dict):
        return ("dict", tuple(sorted((stable_key(k), stable_key(v))
                                     for k, v in value.items())))
    if isinstance(value, (set, frozenset)):
        return ("set", tuple(sorted(map(stable_key, value))))
    if isinstance(value, (tuple, list)):
        return (type(value).__name__, tuple(map(stable_key, value)))
    return (type(value).__name__, value)


def representative_key(rec, excluded=()):
    return stable_key({k: v for k, v in rec.items() if k not in excluded})


def merge_record(records, key, rec):
    old = records.get(key)
    if old is None or stable_key(rec) < stable_key(old):
        records[key] = rec


def merge_type(records, key, rec):
    rec = dict(rec, inner=sorted(set(rec["inner"]), key=stable_key))
    old = records.get(key)
    if old is None:
        records[key] = rec
        return
    # Keep one coherent layout. Union fields must never affect the winner: doing
    # so would make the result dependent on how compile units were partitioned.
    def rank(value):
        # C-style typedefs can share the named concrete type's key (for
        # example typedef enum UnOpr {...} UnOpr). Retain the definition,
        # otherwise generation emits a self alias and loses its constants.
        return (value["kind"] == "typedef", -len(value["members"]),
                representative_key(value, ("methods", "deps", "inner")))
    winner = min((old, rec), key=rank)
    result = dict(winner)
    result["deps"] = old["deps"] | rec["deps"]
    result["inner"] = sorted(set(old["inner"]) | set(rec["inner"]), key=stable_key)
    result["methods"] = dict(old["methods"])
    for mkey, method in rec["methods"].items():
        merge_record(result["methods"], mkey, method)
    records[key] = result


def merge_func(records, key, rec):
    old = records.get(key)
    if old is None:
        records[key] = rec
        return
    def rank(value):
        # Prefer descriptions that recovered parameter names; resolve every tie
        # by content, including template instantiations collapsed by func_key.
        return (-sum(bool(name) for _, name in value["params"]),
                representative_key(value, ("addrs", "inline")))
    result = dict(min((old, rec), key=rank))
    result["addrs"] = old["addrs"] | rec["addrs"]
    result["inline"] = old["inline"] or rec["inline"]
    records[key] = result


def canonicalize(index):
    """Order maps and compile units without reordering members or parameters.

    Sets remain sets for existing consumers. Pickle bytes are an implementation
    detail; generated artifacts and the index's semantic content are canonical.
    """
    def ordered(value):
        if isinstance(value, dict):
            return {k: ordered(value[k]) for k in sorted(value, key=stable_key)}
        if isinstance(value, list):
            return [ordered(v) for v in value]
        if isinstance(value, tuple):
            return tuple(ordered(v) for v in value)
        return value
    result = ordered(index)
    result["units"] = sorted(result["units"])
    return result


def merge(into, part):
    for key, rec in part["types"].items():
        merge_type(into["types"], key, rec)
    for key, rec in part["funcs"].items():
        merge_func(into["funcs"], key, rec)
    for key, rec in part["vars"].items():
        merge_record(into["vars"], key, rec)
    for key, (callers, sites) in part["inlined"].items():
        stat = into["inlined"].setdefault(key, [set(), 0])
        stat[0] |= callers
        stat[1] += sites
    into["units"] += part["units"]


def work(args):
    worker, workers = args
    sys.setrecursionlimit(10000)
    index = new_index()
    for dies in read_units(worker, workers):
        cu = next(iter(dies.values()))
        index["units"].append(norm_path(posixpath.join(cu.get("comp_dir", "").strip('"'),
                                                       name_of(cu))))
        walk(dies, cu, index)
    return index


CACHE_VERSION = 2


def cache_identity():
    """Invalidate legacy caches and changes to the parser or source file.

    The NSS stat identity avoids hashing the multi-gigabyte input on each tool
    invocation. Reproducibility evidence must record its full hash separately.
    """
    stat = NSS.stat()
    return {"version": CACHE_VERSION,
            "parser_sha256": hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            "nss": (str(NSS.resolve()), stat.st_size, stat.st_mtime_ns,
                    stat.st_ctime_ns, stat.st_ino)}


def build(workers=None, cache=None):
    if not NSS.is_file():
        sys.exit(f"{NSS} not found")
    workers = max(2, min(8, os.cpu_count() or 2)) if workers is None else workers
    if not isinstance(workers, int) or workers < 1:
        raise ValueError("workers must be a positive integer")
    cache = CACHE if cache is None else Path(cache)
    identity = cache_identity()
    index = new_index()
    if workers == 1:
        merge(index, work((0, 1)))
    else:
        with multiprocessing.Pool(workers) as pool:
            for part in pool.imap_unordered(work, [(i, workers) for i in range(workers)]):
                merge(index, part)
    index = canonicalize(index)
    if identity != cache_identity():
        raise RuntimeError("DWARF input or parser changed while indexing")
    cache.parent.mkdir(parents=True, exist_ok=True)
    # Keep the existing top-level index shape for read-only cache consumers.
    cached = dict(index, _cache_identity=identity)
    name = None
    try:
        with tempfile.NamedTemporaryFile(dir=cache.parent, prefix=cache.name + ".",
                                         suffix=".tmp", delete=False) as f:
            name = f.name
            pickle.dump(cached, f, protocol=pickle.HIGHEST_PROTOCOL)
        os.replace(name, cache)
    finally:
        if name is not None and os.path.exists(name):
            os.unlink(name)
    return index


def load():
    if CACHE.is_file() and NSS.is_file():
        try:
            with CACHE.open("rb") as f:
                cached = pickle.load(f)
            if cached.pop("_cache_identity", None) == cache_identity():
                return cached
        except (OSError, EOFError, pickle.UnpicklingError):
            pass
    print(">>> indexing the DWARF (about a minute)", file=sys.stderr)
    return build()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--workers", type=int, help="DWARF workers (default: 2 to 8 CPUs)")
    parser.add_argument("--cache", type=Path, default=CACHE, help="output pickle path")
    args = parser.parse_args()
    if args.workers is not None and args.workers < 1:
        parser.error("--workers must be positive")
    idx = build(workers=args.workers, cache=args.cache)
    print(f">>> {len(idx['types'])} types, {len(idx['funcs'])} function definitions, "
          f"{len(idx['vars'])} variables, {len(idx['inlined'])} inlined functions -> {args.cache}")
