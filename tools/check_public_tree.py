"""Check indexed blobs for private game inputs before copying or publishing a tree."""
import os
from pathlib import PurePosixPath
import re
import subprocess
import sys


def private_path(path):
    path = PurePosixPath(path.lower())
    return (path.parts[0] in {"game", "build", ".onboarding"} or
            bool({"exefs", "romfs", "generated-game-data"}.intersection(path.parts[:-1])) or
            path.suffix in {".nss", ".nso", ".nca", ".nsp", ".nsz", ".xci", ".xcz"} or
            path.name in {"prod.keys", "title.keys"} or
            (path.parent.as_posix() == "src/x/common/protobuf" and
             path.name.endswith((".pb.cc", ".pb.h"))))


def private_header(data):
    return (data.startswith((b"NSO0", b"PFS0", b"HFS0", b"\x7fELF")) or
            data[0x10:0x14] == b"NRO0" or data[0x100:0x104] == b"HEAD" or
            data[0x200:0x204] in {b"NCA2", b"NCA3"} or
            re.search(rb"(?im)^\s*(?:master_key_[0-9a-f]{2}|header_key|titlekek_[0-9a-f]{2})"
                      rb"\s*=\s*[0-9a-f]{32,64}\s*$", data) is not None)


def main():
    entries = subprocess.check_output(["git", "ls-files", "--stage", "-z"]).split(b"\0")
    problems = []
    with subprocess.Popen(["git", "cat-file", "--batch"], stdin=subprocess.PIPE, stdout=subprocess.PIPE) as git:
        for entry in filter(None, entries):
            meta, raw_path = entry.split(b"\t", 1)
            mode, oid, stage = meta.split()
            path = os.fsdecode(raw_path)
            if stage != b"0" or private_path(path):
                problems.append(path)
                continue
            if mode == b"160000":
                continue
            git.stdin.write(oid + b"\n")
            git.stdin.flush()
            _, kind, size = git.stdout.readline().split()
            if kind != b"blob":
                raise ValueError("expected a Git blob")
            size = int(size)
            prefix = git.stdout.read(min(size, 4096))
            if private_header(prefix):
                problems.append(path)
            remaining = size - len(prefix)
            while remaining:
                chunk = git.stdout.read(min(remaining, 1024 * 1024))
                if not chunk:
                    raise EOFError("incomplete Git blob")
                remaining -= len(chunk)
            if git.stdout.read(1) != b"\n":
                raise ValueError("incomplete Git batch response")
        git.stdin.close()
    if git.returncode:
        return git.returncode
    for path in problems:
        print(f"Review private input or unresolved index entry: {path!r}", file=sys.stderr)
    return int(bool(problems))


if __name__ == "__main__":
    sys.exit(main())
