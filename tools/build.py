#!/usr/bin/env python3
"""Build the project ELF, optionally using the pinned Lua-only backend."""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path
import resource_admission

VERSION = "2.6.2"
ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = ROOT / "build" / VERSION
LUA_IMAGE = "d3-lua-build:5.0.1"
sys.path.insert(0, str(ROOT / "toolchain"))
from backends import check, prepare_lua
import backend
sys.path.insert(0, str(ROOT / "toolchain/latepipe"))
import latepipe


def compose(*args):
    subprocess.run(["docker", "compose", "run", "--rm", "build", *args], check=True, cwd=ROOT)


def lua_container(*args):
    subprocess.run(["docker", "run", "--rm", "--pull", "never", "--network", "none", "-v", f"{ROOT}:/work",
                    "-v", f"{backend.resolve(ROOT).absolute()}:/opt/d3-lua-cache:ro",
                    "-e", "D3_LUA_TOOLCHAIN=/opt/d3-lua-cache",
                    "-e", "D3_DOCKER_PLATFORM=" + os.environ.get("D3_DOCKER_PLATFORM", "linux/arm64"),
                    "-w", "/work", LUA_IMAGE, *args], check=True, cwd=ROOT)


def main():
    ap = argparse.ArgumentParser("build.py", description="Build the Diablo III 2.6.2 decomp")
    ap.add_argument("--clean", action="store_true", help="clean before building")
    ap.add_argument("--verbose", action="store_true")
    ap.add_argument("--reconfigure", action="store_true", help="delete and regenerate build/2.6.2 first")
    ap.add_argument("--lua-compiler", action="store_true", default=None,
                    help="compile Lua with the pinned Lua backend (default when it is prepared)")
    ap.add_argument("--no-lua-compiler", dest="lua_compiler", action="store_false",
                    help="compile Lua with the stock compiler")
    ap.add_argument("--lua-archive", type=Path, help="offline LLVM 5.0.1 source archive inside repository")
    ap.add_argument("--jobs", type=int, default=2, help="parallel project build jobs (1–8; default: 2)")
    ap.add_argument("--lua-jobs", type=int, default=2, help="parallel Lua compiler bootstrap jobs (1–8; default: 2)")
    ap.add_argument("--onboarding-output", action="store_true",
                    help="use separate resumable normal/Lua onboarding build directories")
    args = ap.parse_args()
    if args.lua_compiler is None:
        # Lua was built with a different code generator; use it whenever it is there.
        args.lua_compiler = bool(args.lua_archive) or not check(ROOT, lua=True)
    relative_build = ("build/onboarding-" + ("lua" if args.lua_compiler else "normal")) if args.onboarding_output else f"build/{VERSION}"
    build_dir = ROOT / relative_build
    if build_dir.is_symlink():
        ap.error("build directory must not be a symlink")
    if args.lua_archive and not args.lua_compiler:
        ap.error("--lua-archive requires --lua-compiler")
    if not 1 <= args.jobs <= 8 or not 1 <= args.lua_jobs <= 8:
        ap.error("--jobs and --lua-jobs must be between 1 and 8")
    if shutil.which("docker") is None:
        sys.exit("docker not found: install Docker Desktop / docker engine")
    try:
        with resource_admission.lease():
            resource_admission.check(str(build_dir.absolute()))
            # Builds exclude each other through the lease above. Single-object
            # compiles (tools/try_file.py) may run at the same time.
            if args.lua_compiler and args.lua_archive:
                prepare_lua(ROOT, args.lua_archive, args.lua_jobs)
            errors = check(ROOT, lua=args.lua_compiler)
            if errors:
                raise RuntimeError("\n".join(errors))
            # The late-GlobalOpt plugin every optimised unit loads (docs/late_globalopt.md):
            # built in d3-lua-build:5.0.1 when missing, refused when it differs from its pin.
            latepipe.build(ROOT)
            resource_admission.check(str(build_dir.absolute()), growth_bytes=resource_admission.ATTEMPT_GROWTH)
            runner = lua_container if args.lua_compiler else compose
            if args.reconfigure and build_dir.is_dir():
                shutil.rmtree(build_dir)
            # Always configure the explicit selection, including OFF, so switching
            # modes cannot silently reuse objects or a previous CMake cache option.
            runner("cmake", "-G", "Ninja", "-DCMAKE_TOOLCHAIN_FILE=toolchain/ToolchainNX64.cmake",
                   "-DD3_LUA_COMPILER=" + ("ON" if args.lua_compiler else "OFF"), "-B", relative_build)
            build = ["cmake", "--build", relative_build]
            if args.clean:
                build.append("--clean-first")
            if args.verbose:
                build.append("--verbose")
            build += ["--", "-j" + str(args.jobs)]
            runner(*build)
    except (OSError, RuntimeError, ValueError, KeyError, subprocess.CalledProcessError) as error:
        sys.exit(str(error))


if __name__ == "__main__":
    main()
