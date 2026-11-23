"""Compare tracked SDK and middleware files to locally generated DWARF reference text."""
import csv
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
COMPONENTS = ("NintendoSDK", "BlizzardCore", "Contrib/Contrib/blz", "Contrib/Contrib/Domino",
              "Contrib/Contrib/fmodstudiosrc11005", "Contrib/Contrib/DemonWare")


def main():
    reference = ROOT / "build/dwarf/headers"
    if not reference.is_dir():
        sys.exit("Generate the local reference files with python3 tools/gen_headers.py first.")
    references = {p.relative_to(reference).as_posix().lower(): p
                  for p in reference.rglob("*") if p.is_file()}
    tracked = subprocess.check_output(["git", "ls-files", "-z", "--", *["src/" + c for c in COMPONENTS]],
                                      cwd=ROOT).decode().split("\0")
    writer = csv.writer(sys.stdout)
    writer.writerow(("tracked_path", "reference_path", "comparison"))
    for path in filter(None, tracked):
        if "/DemonWare/external/" in path:
            continue  # Pinned upstream libraries have a separate source record.
        source = (ROOT / path).read_text().splitlines()
        candidates = [path.removeprefix("src/")]
        for line in source[:5]:
            if line.startswith("// Original file: "):
                candidates.insert(0, line.removeprefix("// Original file: "))
        ref = next((references[k.lower()] for k in candidates if k.lower() in references), None)
        status = "no reference file"
        if ref:
            lines = ref.read_text().splitlines()
            if (source and lines and source[0].startswith("// Adopted from tools/gen_headers.py") and
                    lines[0] == "// Generated from the DWARF by tools/gen_headers.py -- reference only."):
                lines[0] = source[0]
            status = "matches reference text" if lines == source else "text differs"
        writer.writerow((path, ref.relative_to(reference).as_posix() if ref else "", status))


if __name__ == "__main__":
    main()
