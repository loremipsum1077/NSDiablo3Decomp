"""Enumerate private build inputs whose SHA-256 identities are pinned by a revision."""
import json
from pathlib import Path
import re

MANIFEST = "config/local_game_data.json"


def generated_files(manifest):
    paths = {}
    for name, row in manifest["descriptors"].items():
        if not re.fullmatch(r"[A-Za-z][A-Za-z0-9_]*", name):
            raise ValueError("invalid descriptor name")
        for ext in ("cc", "h"):
            paths[f"src/X/Common/Protobuf/{name}.pb.{ext}"] = row[ext + "_sha256"]
    for name, expected in manifest["includes"].items():
        if not re.fullmatch(r"[a-z][a-z_]*\.inc", name):
            raise ValueError("invalid generated include name")
        paths[f"src/X/Common/generated-game-data/{name}"] = expected
    if not all(isinstance(sha, str) and re.fullmatch(r"[0-9a-f]{64}", sha) for sha in paths.values()):
        raise ValueError("invalid generated file hash")
    return paths


if __name__ == "__main__":
    root = Path(__file__).resolve().parents[1]
    manifest = json.loads((root / MANIFEST).read_text())
    missing = [name for name in generated_files(manifest) if not (root / name).is_file()]
    if missing:
        raise SystemExit(f"Missing {len(missing)} local game inputs, including {missing[0]}. "
                         "Run python3 tools/bootstrap_game_data.py /path/to/game --build-protoc")
