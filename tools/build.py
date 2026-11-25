#!/usr/bin/env python3
"""Build the project ELF, optionally using the pinned Lua-only backend."""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path
import resource_admission

import project_version

VERSION = project_version.VERSION
ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = ROOT / "build" / VERSION
LUA_IMAGE = "d3-lua-build:5.0.1"
# clang 5.0.1 (2.6.2) builds through docker compose and loads the LatePipe plugin;
# any other compiler (2.7.7: 7.1.0) runs its own image directly and needs neither.
STOCK_501 = project_version.CLANG == "5.0.1"
DOWNLOADS = ROOT / "build" / "toolchains" / "downloads"
sys.path.insert(0, str(ROOT / "toolchain"))
from backends import check, prepare_lua
import backend
sys.path.insert(0, str(ROOT / "toolchain/latepipe"))
import latepipe


def compose(*args):
    subprocess.run(["docker", "compose", "run", "--rm", "build", *args], check=True, cwd=ROOT)


def image_container(*args):
    subprocess.run(["docker", "run", "--rm", "--pull", "never", "--network", "none", "-v", f"{ROOT}:/work",
                    "-w", "/work", project_version.IMAGE, *args], check=True, cwd=ROOT)


LEGACY_RELATIVE = project_version.LEGACY_BUILD.relative_to(ROOT).as_posix()


def build_legacy(args):
    """Mixed build, phase 1: the prebuilt libraries with the 2.6.2 recipe.

    clang 5.0.1 + LatePipe + the Lua backend, in d3-lua-build:5.0.1, objects only
    (no link) into build/<version>-legacy. The main phase links them.
    """
    errors = check(ROOT, lua=True)
    if errors:
        raise RuntimeError("legacy libraries need the clang 5.0.1 toolchain and Lua backend:\n"
                           + "\n".join(errors))
    latepipe.build(ROOT)
    legacy_dir = project_version.LEGACY_BUILD
    if args.reconfigure and legacy_dir.is_dir():
        shutil.rmtree(legacy_dir)
    lua_container("cmake", "-G", "Ninja", "-DCMAKE_TOOLCHAIN_FILE=toolchain/ToolchainNX64.cmake",
                  "-DD3_LUA_COMPILER=ON", "-DD3_USE_LATEPIPE=ON", "-DD3_WEAK_PROVIDERS=OFF",
                  "-DD3_LIBRARY_SET=legacy-only", "-B", LEGACY_RELATIVE)
    build = ["cmake", "--build", LEGACY_RELATIVE]
    if args.clean:
        build.append("--clean-first")
    if args.verbose:
        build.append("--verbose")
    lua_container(*build, "--", "-j" + str(args.jobs))


def setup_image():
    """Build d3-build:7.1.0 from the archive in build/toolchains/downloads (SHA-256 pinned)."""
    if project_version.CLANG != "7.1.0":
        sys.exit(f"--setup-image only knows clang 7.1.0, not {project_version.CLANG}")
    archive = DOWNLOADS / "clang+llvm-7.1.0-aarch64-linux-gnu.tar.xz"
    if not archive.is_file():
        sys.exit(f"{archive} not found: download it from "
                 "https://github.com/llvm/llvm-project/releases/tag/llvmorg-7.1.0")
    subprocess.run(["docker", "build", "--platform", "linux/arm64", "-t", project_version.IMAGE,
                    "-f", str(ROOT / "toolchain" / "Dockerfile.clang7"), str(DOWNLOADS)], check=True)


def lua_container(*args):
    subprocess.run(["docker", "run", "--rm", "--pull", "never", "--network", "none", "-v", f"{ROOT}:/work",
                    "-v", f"{backend.resolve(ROOT).absolute()}:/opt/d3-lua-cache:ro",
                    "-e", "D3_LUA_TOOLCHAIN=/opt/d3-lua-cache",
                    "-e", "D3_DOCKER_PLATFORM=" + os.environ.get("D3_DOCKER_PLATFORM", "linux/arm64"),
                    "-w", "/work", LUA_IMAGE, *args], check=True, cwd=ROOT)


def main():
    ap = argparse.ArgumentParser("build.py", description=f"Build the Diablo III {VERSION} decomp "
                                 f"with clang {project_version.CLANG}")
    ap.add_argument("--clean", action="store_true", help="clean before building")
    ap.add_argument("--verbose", action="store_true")
    ap.add_argument("--reconfigure", action="store_true", help=f"delete and regenerate build/{VERSION} first")
    ap.add_argument("--setup-image", action="store_true", help="build the clang 7.1.0 Docker image and exit")
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
    if args.setup_image:
        setup_image()
        return
    if not STOCK_501:
        if args.lua_compiler is False or args.lua_archive:
            ap.error("in a mixed build Lua always uses the prepared Lua backend (legacy phase)")
        if args.onboarding_output:
            ap.error("--onboarding-output is for the 2.6.2 / clang 5.0.1 build")
        args.lua_compiler = False   # the main (7.1.0) phase compiles no Lua
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
            if STOCK_501:
                errors = check(ROOT, lua=args.lua_compiler)
                if errors:
                    raise RuntimeError("\n".join(errors))
                # The late-GlobalOpt plugin every optimised unit loads (docs/late_globalopt.md):
                # built in d3-lua-build:5.0.1 when missing, refused when it differs from its pin.
                latepipe.build(ROOT)
            else:
                if subprocess.run(["docker", "image", "inspect", project_version.IMAGE],
                                  capture_output=True).returncode:
                    raise RuntimeError(f"{project_version.IMAGE} not found: run python3 tools/build.py --setup-image")
                build_legacy(args)
            resource_admission.check(str(build_dir.absolute()), growth_bytes=resource_admission.ATTEMPT_GROWTH)
            runner = lua_container if args.lua_compiler else compose if STOCK_501 else image_container
            if args.reconfigure and build_dir.is_dir():
                shutil.rmtree(build_dir)
            # Always configure the explicit selection, including OFF, so switching
            # modes cannot silently reuse objects or a previous CMake cache option.
            mixed = [] if STOCK_501 else ["-DD3_LIBRARY_SET=exclude-legacy",
                                          "-DD3_LEGACY_OBJECT_DIR=/work/" + LEGACY_RELATIVE]
            runner("cmake", "-G", "Ninja", "-DCMAKE_TOOLCHAIN_FILE=toolchain/ToolchainNX64.cmake",
                   "-DD3_LUA_COMPILER=" + ("ON" if args.lua_compiler else "OFF"),
                   "-DD3_USE_LATEPIPE=" + ("ON" if project_version.LATEPIPE else "OFF"),
                   "-DD3_WEAK_PROVIDERS=" + ("ON" if STOCK_501 else "OFF"), *mixed, "-B", relative_build)
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
