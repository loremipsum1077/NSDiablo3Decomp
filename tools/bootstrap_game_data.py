#!/usr/bin/env python3
"""Generate ignored 2.6.2 C++ bindings and includes from a local extracted game."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
from tempfile import TemporaryDirectory

from game_button_maps import generate_buttons
from game_lua_tables import LuaTableImage, generate_lua_tables
from game_proto import schema
from game_protoc import build_protoc
from game_sno_tables import generate
from validation.elf import Elf
from verify_original_inputs import InputError, require_inputs

ROOT = Path(__file__).resolve().parents[1]
BINDINGS = ROOT / "src/X/Common/Protobuf"
INCLUDES = ROOT / "src/X/Common/generated-game-data"
PROTO_INCLUDE = "../../../Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/"


def digest(data):
    return hashlib.sha256(data).hexdigest()


SOURCE_DIR_2_6_2 = "D:/BuildServer/2/work/code/branches/2_6_2/X/Common/Protobuf/"


def adapt(path, text, source_dir=SOURCE_DIR_2_6_2, layout_checks=True):
    """Apply the checked Hydra ABI, original diagnostic lines and project layout checks.

    source_dir is the build-server directory the #line directive names; layout_checks
    appends layout/<Name>.inc (2.6.2 DWARF sizes) to the header."""
    text = re.sub(r'#include <(google/protobuf/[^>]+)>', rf'#include "{PROTO_INCLUDE}\1"', text)
    text = re.sub(r'(?:::)?std::(pair|string)\b', r'::blz::\1', text)
    if path.suffix == ".h":
        text = text.replace("#include <string>\n\n", "").replace("#include <string>\n", "")
        text = text.replace(f'#include "{PROTO_INCLUDE}google/protobuf/extension_set.h"\n', '')
        enum_marker = r'^template <> struct is_proto_enum<[^\n]*\n'
        text = re.sub(enum_marker, lambda _: (
            "// protoc also emits an is_proto_enum<> specialisation before each GetEnumDescriptor<>; they are\n"
            "// left out here with template_util.h (see generated_enum_reflection.h). Nothing compiled uses them.\n"
        ), text, count=1, flags=re.M)
        text = re.sub(enum_marker, "", text, flags=re.M)
        # Retain two editorial differences in the previously reviewed sources.
        if path.name == "Hero.pb.h":
            marker = "// @@protoc_insertion_point(field_set_allocated:D3.Hero.Digest.original_hero_id)\n}"
            text = text.replace(marker, marker + "\n")
        if not layout_checks:
            return text
        checks = (BINDINGS / "layout" / (path.name.split(".")[0] + ".inc")).read_text()
        end = text.rindex("#endif")
        return text[:end] + "\n" + checks + "\n" + text[end:]

    lines = text.splitlines(keepends=True)
    lines[2] = f'#line 4 "{source_dir}{path.name}"\n'
    name = path.name.split(".")[0]
    check = next(i for i, line in enumerate(lines, 1) if "GOOGLE_CHECK(file != NULL)" in line) + 5
    if name == "Console":
        check = 65  # Line cited by the reviewed source comment.
    if name == "PartyMessage":
        extra = ["// Hydra protoc's five extra lines; the static initializer shows these game headers.",
                 '#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"',
                 '#include "../SNO/SNOName.h"', '#include "../WorldPlace.h"', '#include "../../Client/Gfx/GfxVB.h"']
    else:
        extra = ["// The Hydra protoc that generated the original emits five more lines in this include block",
                 "// than stock protoc 2.6.0; what they held is not recoverable. These five comment lines stand",
                 "// in for them so every GOOGLE_CHECK below keeps the __LINE__ the binary has (the",
                 f"// `file != NULL` check in protobuf_AssignDesc_{name}_2eproto is at line {check}, for one).", "//"]
    lines[8:8] = [line + "\n" for line in extra]
    text = "".join(lines)
    internal = "::google::protobuf::internal::"
    ident = name + "_2eproto"
    startup = "StaticDescriptorInitializer_Struct_" + ident
    text = text.replace("  GOOGLE_PROTOBUF_VERIFY_VERSION;", (
        f"\n  GOOGLE_CHECK({internal}IsStartupProtobufLibraryStarted());\n"
        f"  GOOGLE_CHECK(!{internal}IsStartupProtobufLibraryComplete());\n\n"
        "  GOOGLE_PROTOBUF_VERIFY_VERSION;"
    ))
    text = text.replace("// Force AddDescriptors() to be called at static initialization time.",
                        f"{internal}ProtobufStartupStruct {startup};")
    text = text.replace(f"    protobuf_AddDesc_{ident}();",
                        f"    {internal}OnStartup(&{startup},\n"
                        f"                                            &protobuf_AddDesc_{ident});")
    instance = f"static_descriptor_initializer_{ident}_;"
    text = text.replace("} " + instance, f"}};\nStaticDescriptorInitializer_{ident} " + instance)

    # Hydra validates on input and omits the stock serialization diagnostic.
    def utf8_check(match):
        indent, args, phase = match.groups()
        if phase == "SERIALIZE":
            return ""
        return f"{indent}DO_({internal}IsStructurallyValidUTF8(\n{args}));\n"

    return re.sub(r"^( *)::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField\(\n"
                  r"((?:[^\n]+\n)?[^\n]+),\n *::google::protobuf::internal::WireFormat::(PARSE|SERIALIZE),\n"
                  r' *"[^"]+"\);\n', utf8_check, text, flags=re.M)


def binding_paths(manifest):
    for name, row in manifest.items():
        for ext in ("cc", "h"):
            yield BINDINGS / f"{name}.pb.{ext}", row[ext + "_sha256"]


def nvn_entry_points(image):
    # The loader's 498 NUL-terminated procedure names are consecutive in .rodata.
    names = image.read_vaddr(0xd6bfe7, 15002).split(b"\0")
    if len(names) != 499 or names[-1] or any(
        not re.fullmatch(rb"nvn[A-Z][A-Za-z0-9]*", name) for name in names[:-1]
    ):
        raise ValueError("unexpected NVN procedure-name table")
    return "".join(f"NVN_ENTRY({name[3:].decode('ascii')})\n" for name in names[:-1])


def gameplay_tables(image, relocations):
    def string_at_pointer(slot):
        address = relocations[slot]
        raw = image.read_vaddr(address, 1024)
        return json.dumps(raw[:raw.index(0)].decode("ascii"))

    events = []
    for slot in range(0x103b4e0, 0x103b4e0 + 135 * 24, 24):
        category, event, operand = struct.unpack("<ii8xi4x", image.read_vaddr(slot, 24))
        label = string_at_pointer(slot + 8)
        events.append(f"    {{ {category}, {event}, {label}, {operand} }},\n")

    palette = image.read_vaddr(0xe538a0, 40 * 4)
    return {
        "achievement_events.inc": "".join(events),
        "font_colors.inc": "".join(
            "    RGBAColor(" + ", ".join(f"0x{value:02x}" for value in rgba) + "),\n"
            for rgba in struct.iter_unpack("4B", palette)
        ),
        "hotbar_anchors.inc": "".join(
            f"    case {2 + i}: return {string_at_pointer(0x104ad30 + 8 * i)};\n" for i in range(4)
        ),
        "mirror_attacks.inc": "".join(
            f"        {string_at_pointer(0x1097e08 + 8 * i)},\n" for i in range(3)
        ),
    }


def bootstrap(args):
    game = args.game.resolve()
    require_inputs(game, nso=args.nso)
    nss = game / "romfs/DiabloIIINX64r.nss"
    image = Elf(nss)
    manifest = json.loads((ROOT / "config/local_game_data.json").read_text())
    descriptors = manifest["descriptors"]
    raw_schemas = {}
    for name, row in descriptors.items():
        raw = image.read_vaddr(int(row["address"], 16), row["length"])
        if digest(raw) != row["sha256"]:
            raise ValueError(f"descriptor mismatch: {name}")
        raw_schemas[name] = schema(raw)

    tables = generate(ROOT, image, nss, args.objdump)
    tables["button_maps.inc"] = generate_buttons(image, nss, args.objdump)
    lua_image = LuaTableImage(ROOT, image)
    tables["nvn_entry_points.inc"] = nvn_entry_points(image)
    tables.update(gameplay_tables(image, lua_image.relocations))
    tables["lua_tables.inc"], evidence = generate_lua_tables(lua_image)
    if digest(evidence.encode()) != manifest["lua_evidence_sha256"]:
        raise ValueError("SWIG evidence differs from its reviewed hash")
    sounds = struct.unpack("<185I", image.read_vaddr(0xe4495c, 185 * 4))
    tables["error_sounds.inc"] = "\n".join(
        "    " + ", ".join(map(str, sounds[i:i + 10])) + "," for i in range(0, len(sounds), 10)
    ) + "\n"

    issuer_data = image.read_vaddr(0xe3ca60, 406)
    if not issuer_data.endswith(b"\0"):
        raise ValueError("unterminated Switch issuer mapping")
    tables["issuer_mapping.inc"] = json.dumps(issuer_data[:-1].decode("ascii")) + "\n"

    for name, address, count, layout in (
        ("account_attribs", 0xe53ed4, 21, "<ii"),
        ("account_wide_attribs", 0xe53f7c, 10, "<ii"),
        ("partition_attribs", 0xe53fcc, 11, "<ii"),
        ("hero_attribs", 0xe54c10, 27, "<ii"),
        ("actor_particles", 0xe42d58, 51, "<iiiI"),
    ):
        raw = image.read_vaddr(address, count * struct.calcsize(layout))
        tables[name + ".inc"] = "".join(
            "    { " + ", ".join(map(str, row)) + " },\n" for row in struct.iter_unpack(layout, raw)
        )

    if tables.keys() != manifest["includes"].keys():
        raise ValueError("generated include inventory differs from manifest")
    for name, expected in manifest["includes"].items():
        if digest(tables[name].encode()) != expected:
            raise ValueError(f"table reconstruction mismatch: {name} (check llvm-objdump version)")

    build = ROOT / "build"
    build.mkdir(exist_ok=True)
    with TemporaryDirectory(dir=build, prefix="game-data-") as tmp:
        stage = Path(tmp)
        missing = [(p, sha) for p, sha in binding_paths(descriptors)
                   if not p.is_file() or digest(p.read_bytes()) != sha]
        outputs = {INCLUDES / name: text.encode() for name, text in tables.items()}
        outputs[build / "try/lua_wrapper_evidence/tables.evidence.json"] = evidence.encode()
        if missing and not args.bindings_from:
            compiler = args.protoc or "protoc"
            if args.build_protoc or args.protoc_archive:
                compiler = build_protoc(build, args.protoc_archive)
            elif not args.protoc and (build / "game-tools/protoc-2.6.0").exists():
                compiler = str(build / "game-tools/protoc-2.6.0")
            version = subprocess.check_output([compiler, "--version"], text=True).strip()
            if version != "libprotoc 2.6.0":
                raise ValueError(f"expected libprotoc 2.6.0, found {version}. Pass --build-protoc or --protoc /path/to/protoc-2.6.0")
            for name, proto in raw_schemas.items():
                (stage / (name + ".proto")).write_text(proto)
            subprocess.run([compiler, "-I.", "--cpp_out=.", *[n + ".proto" for n in raw_schemas]],
                           cwd=stage, check=True)
        if missing:
            for path, expected in binding_paths(descriptors):
                if args.bindings_from:
                    content = (args.bindings_from / path.name).read_bytes()
                else:
                    content = adapt(path, (stage / path.name).read_text()).encode()
                if digest(content) != expected:
                    raise ValueError(f"{path.name} differs from its reviewed source; no files installed")
                outputs[path] = content
        for path, content in outputs.items():
            if path.is_file() and path.read_bytes() == content:
                continue
            path.parent.mkdir(parents=True, exist_ok=True)
            scratch = stage / (path.name + ".ready")
            scratch.write_bytes(content)
            scratch.replace(path)
    print(f"Ready: {len(descriptors)} verified binding pairs and {len(tables)} local data includes.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("game", type=Path, help="extracted directory containing exefs/ and romfs/")
    parser.add_argument("--nso", type=Path, help="alternative location of the compressed NSO for setup")
    parser.add_argument("--objdump", default="llvm-objdump")
    compiler = parser.add_mutually_exclusive_group()
    compiler.add_argument("--protoc", help="path to the open-source 2.6.0 compiler")
    compiler.add_argument("--build-protoc", action="store_true", help="download and build the pinned public compiler when needed")
    compiler.add_argument("--bindings-from", type=Path,
                          help="migrate previously generated private bindings with matching SHA-256 hashes")
    compiler.add_argument("--protoc-archive", type=Path, help="build from a local protobuf-2.6.0.tar.bz2 release archive")
    args = parser.parse_args()
    # Subprocesses run in a private staging directory. Resolve relative tool paths up front.
    for option in ("protoc", "objdump"):
        tool = getattr(args, option)
        if tool and "/" in tool:
            setattr(args, option, str(Path(tool).resolve()))
    try:
        bootstrap(args)
    except (InputError, OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"Game-data bootstrap failed: {error}\n")


if __name__ == "__main__":
    main()
