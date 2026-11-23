"""Install the maintained Viking patch series without hiding apply failures."""
import hashlib
import shutil
import subprocess
import tempfile
from pathlib import Path

# This vendored commit already contains these exact patches. Patch 0004 changes
# 0001's context, so testing each patch in reverse cannot detect that fact.
INCORPORATED_COMMIT = "73b51f9a57a00443252dba41a1a5909e4d9f0495"
INCORPORATED_PATCHES = {
    "0001-viking-compare-across-trap-words.patch": "f82bb6307d282e836e89864b15ee92b7c8742df9475543c06a585b29024c9be7",
    "0002-viking-name-at-file-link-map.patch": "b361350f8f2b399df284e14840e8fdfe7f59be4e37f726a9a86b87332849f031",
    "0003-viking-batch-checker.patch": "66c2ee970e525b21f38dba960ec98e371ce59c99038000816cb13cfe1293c094",
    "0004-viking-flow-sensitive-adrp-pairing.patch": "07e0973adac79e79fc1e957e438c50c07de47d187f91e8d7369502d05d0ba832",
}


def applied_prefix(common: Path, patches: list[Path]) -> int:
    """Recognize overlapping installed patches by undoing them in a scratch tree.

    Reverse-checking each patch alone fails when a later patch edits its context.
    Never undo anything in the active vendor checkout, or treat an apply failure
    as evidence that a patch is installed.
    """
    files = subprocess.check_output(
        ["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"],
        cwd=common,
    ).decode().split('\0')
    for count in range(len(patches), 0, -1):
        with tempfile.TemporaryDirectory(prefix="d3-patch-state-") as directory:
            scratch = Path(directory)
            for name in set(files) - {''}:
                source, target = common / name, scratch / name
                if source.is_symlink() or source.is_file():
                    target.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copy2(source, target, follow_symlinks=False)
            subprocess.run(["git", "init", "--quiet"], cwd=scratch, check=True)
            for patch in reversed(patches[:count]):
                if subprocess.run(["git", "apply", "--reverse", str(patch)],
                                  cwd=scratch, capture_output=True).returncode:
                    break
            else:
                return count
    return 0


def apply_local_patches(common: Path, patch_dir: Path) -> None:
    """Skip recorded incorporated patches, otherwise apply or verify in reverse.

    Unknown revisions receive no provenance exemption. Modified historical patch
    files fail explicitly: changing them requires updating the provenance record.
    Git errors and unappliable new patches propagate to the caller.
    """
    common, patch_dir = common.resolve(), patch_dir.resolve()
    patches = sorted(patch_dir.glob("*.patch"))
    for patch in patches:
        expected = INCORPORATED_PATCHES.get(patch.name)
        if expected and hashlib.sha256(patch.read_bytes()).hexdigest() != expected:
            raise RuntimeError(f"incorporated patch content changed: {patch.name}")
    incorporated = subprocess.run(
        ["git", "merge-base", "--is-ancestor", INCORPORATED_COMMIT, "HEAD"],
        cwd=common, capture_output=True,
    ).returncode == 0
    remaining = []
    for patch in patches:
        if incorporated and patch.name in INCORPORATED_PATCHES:
            print(f">>> patch incorporated in {INCORPORATED_COMMIT[:7]}: {patch.name}")
            continue
        remaining.append(patch)
    # Overlapping historical patches require the pinned ancestry; do not infer
    # their incorporation from a source snapshot with unknown provenance.
    installed = applied_prefix(common, remaining) if incorporated else 0
    for index, patch in enumerate(remaining):
        if index < installed or (not incorporated and subprocess.run(
            ["git", "apply", "--check", "--reverse", str(patch)],
            cwd=common, capture_output=True,
        ).returncode == 0):
            print(f">>> patch already applied: {patch.name}")
            continue
        print(f">>> applying {patch.name} to the submodule")
        subprocess.check_call(["git", "apply", str(patch)], cwd=common)
