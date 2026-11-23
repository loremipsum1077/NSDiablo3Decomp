#!/usr/bin/env python3
"""
Set up the Diablo III 2.6.2 (Switch) decomp tooling from your own dump.

  tools/setup.py [build/exefs/main]

1. fetches nx2elf (prebuilt, from open-ead/nx-decomp-tools-binaries) into
   toolchain/nx-decomp-tools-binaries (gitignored)
2. converts the ExeFS `main` NSO -> data/main.elf (+ main.uncompressed.nso),
   verifying the NSO and ELF SHA-256
3. generates data/functions.csv from the unstripped .nss in the RomFS
4. applies tools/patches to the checker and builds tools/check, tools/listsym, tools/decompme (needs cargo/rust)
"""
import argparse
import hashlib
import json
import platform
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

# Keep the read-only input check from creating a local bytecode cache.
sys.dont_write_bytecode = True
from verify_original_inputs import require_inputs

ROOT = Path(__file__).resolve().parent.parent
# Compatibility mirror for contributor_check AST consumers; manifest loading checks equality.
NSO_SHA256 = "9f0cb00de2ae3c1bd05757d100e834d5480bb56d51aa664103058a7c7006c115"
ELF_SHA256 = "9c246371f89c55c2f3bbacc69d0be2c1c60f6a070d66284ce5869171f88be90d"
BINARIES = ROOT / "toolchain" / "nx-decomp-tools-binaries"
LOCK = ROOT / "tools" / "setup_lock.json"


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def load_lock(path: Path = LOCK) -> dict:
    lock = json.loads(path.read_text())
    if lock.get("schema_version") != 1:
        raise RuntimeError("unsupported setup lock schema")
    return lock


def verify_binaries(path: Path, lock: dict) -> None:
    """Fail closed without resetting an existing user's checkout."""
    if path.is_symlink() or not (path / ".git").exists():
        raise RuntimeError(f"{path} is not a binaries git checkout; move it aside and rerun setup")
    revision = subprocess.check_output(
        ["git", "-C", str(path), "rev-parse", "HEAD"], text=True).strip()
    if revision != lock["revision"]:
        raise RuntimeError(f"binaries revision {revision} does not match pinned {lock['revision']}; "
                           f"move {path} aside and rerun setup")
    for relative, expected in lock["sha256"].items():
        binary = path / relative
        if binary.is_symlink() or not binary.is_file() or sha256(binary) != expected:
            raise RuntimeError(f"binaries SHA-256 mismatch: {relative}; "
                               f"move {path} aside and rerun setup")
        if binary.stat().st_mode & 0o111 == 0:
            raise RuntimeError(f"binary is not executable: {binary}")


def fetch_binaries():
    lock = load_lock()
    if BINARIES.exists() or BINARIES.is_symlink():
        verify_binaries(BINARIES, lock)
        return
    BINARIES.parent.mkdir(parents=True, exist_ok=True)
    # Only publish a complete verified checkout. Interrupted downloads cannot
    # leave a directory that a subsequent setup incorrectly treats as installed.
    with tempfile.TemporaryDirectory(prefix=".nx-binaries-", dir=BINARIES.parent) as temp:
        checkout = Path(temp) / "checkout"
        subprocess.check_call(["git", "init", "--quiet", str(checkout)])
        subprocess.check_call(["git", "-C", str(checkout), "fetch", "--depth", "1",
                               lock["repository"], lock["revision"]])
        subprocess.check_call(["git", "-C", str(checkout), "checkout", "--quiet", "--detach", "FETCH_HEAD"])
        verify_binaries(checkout, lock)
        checkout.rename(BINARIES)


def pinned_nx2elf() -> Path:
    verify_binaries(BINARIES, load_lock())
    folder = {"Linux": "linux", "Darwin": "macos"}.get(platform.system())
    if folder is None:
        raise RuntimeError(f"no pinned nx2elf for {platform.system()}")
    return BINARIES / folder / "nx2elf"


def prepare_executable(nso: Path):
    from common import setup_common as setup
    elf = setup.get_target_elf_path()
    if elf.is_file() and sha256(elf) == ELF_SHA256:
        print(">>> main.elf is already set up")
        return
    if not nso.is_file():
        setup.fail(f"{nso} is not a file")
    if sha256(nso) != NSO_SHA256:
        setup.fail(f"unexpected NSO hash {sha256(nso)} (expected {NSO_SHA256}); this repo targets 2.6.2")
    elf.parent.mkdir(parents=True, exist_ok=True)
    subprocess.check_call([str(pinned_nx2elf()), str(nso), "--export-elf", str(elf),
                           "--export-uncompressed", str(setup.get_uncompressed_target_path())])
    if sha256(elf) != ELF_SHA256:
        setup.fail(f"unexpected ELF hash {sha256(elf)} (expected {ELF_SHA256})")
    print(">>> main.elf set up and verified")


def link_system_objdump():
    """The prebuilt macOS llvm-objdump needs a libLLVM.dylib the binaries repo
    does not ship, which breaks tools/check's diff view. util/tools.py looks in
    toolchain/bin first, so point it at the system one."""
    objdump = shutil.which("llvm-objdump") or "/opt/homebrew/opt/llvm/bin/llvm-objdump"
    if not Path(objdump).is_file():
        return
    link = ROOT / "toolchain" / "bin" / "llvm-objdump"
    link.parent.mkdir(parents=True, exist_ok=True)
    link.unlink(missing_ok=True)
    link.symlink_to(objdump)


def apply_local_patches():
    from checker_patches import apply_local_patches as apply
    apply(ROOT / "tools" / "common", ROOT / "tools" / "patches")


def install_checker():
    """Build the patched checker with its committed dependency resolution.

    An explicit private target path prevents CARGO_TARGET_DIR and user Cargo
    configuration from selecting another checkout's compiled artifacts.
    """
    target = ROOT / "build" / "checker-target"
    try:
        subprocess.check_call(["cargo", "build", "--locked", "--manifest-path",
                               str(ROOT / "tools/common/viking/Cargo.toml"),
                               "--release", "--target-dir", str(target)])
    except FileNotFoundError as error:
        raise RuntimeError("cargo is unavailable; install Rust and Cargo before running setup") from error
    binaries = [(target / "release" / name, ROOT / "tools" / name)
                for name in ("check", "listsym", "decompme")]
    for source, _ in binaries:
        if not source.is_file():
            raise RuntimeError(f"cargo did not produce expected checker tool: {source}")
    for source, destination in binaries:
        shutil.copy2(source, destination)


def main(argv=None):
    ap = argparse.ArgumentParser("setup.py")
    ap.add_argument("nso", type=Path, nargs="?", default=ROOT / "build/exefs/main")
    ap.add_argument("--skip-check-build", action="store_true", help="don't build tools/check (skips cargo)")
    ap.add_argument("--preserve-inventories", action="store_true",
                    help="use reviewed tracked inventories; do not regenerate inventories or headers")
    args = ap.parse_args(argv)

    # Reject originals before venv setup, downloads, conversion, generation or builds.
    require_inputs(ROOT / "build", root=ROOT, nso=args.nso)

    subprocess.check_call([sys.executable, str(ROOT / "tools/bootstrap_game_data.py"),
                           str(ROOT / "build"), "--nso", str(args.nso.resolve())])

    from common.setup_venv import enter_venv
    enter_venv()
    fetch_binaries()
    link_system_objdump()
    prepare_executable(args.nso)
    if not args.preserve_inventories:
        subprocess.check_call([sys.executable, str(ROOT / "tools/gen_functions_csv.py")])
        subprocess.check_call([sys.executable, str(ROOT / "tools/gen_function_files.py")])
        # again: qualifying duplicate names needs function_files.csv (quality is kept)
        subprocess.check_call([sys.executable, str(ROOT / "tools/gen_functions_csv.py")])
        subprocess.check_call([sys.executable, str(ROOT / "tools/inline_rank.py"), "--top", "0"])
        subprocess.check_call([sys.executable, str(ROOT / "tools/gen_headers.py")])
    if not args.skip_check_build:
        apply_local_patches()
        install_checker()
    print(">>> done")


if __name__ == "__main__":
    try:
        main()
    except (RuntimeError, subprocess.CalledProcessError) as error:
        sys.exit(f"setup: {error}")
