"""The game version and compiler this checkout targets, from tools/config.toml.

  VERSION      game version, e.g. "2.7.7" (data/<VERSION>/, build/<VERSION>/)
  CLANG        compiler version, e.g. "7.1.0"
  IMAGE        Docker build image, "d3-build:<CLANG>"
  LATEPIPE     whether units load the late-GlobalOpt plugin. Only clang 5.0.1
               needs it: LLVM 6.0+ runs that step itself.
  MIXED        True when prebuilt libraries (LEGACY_DIRS) keep clang 5.0.1.
"""
from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CONFIG = ROOT / 'tools' / 'config.toml'


def _read(key: str, default: str) -> str:
    text = CONFIG.read_text() if CONFIG.is_file() else ''
    match = re.search(rf'^\s*{key}\s*=\s*"([^"]+)"', text, re.M)
    return match.group(1) if match else default


VERSION = _read('default_version', '2.6.2')
CLANG = _read('clang_version', '5.0.1')
IMAGE = f'd3-build:{CLANG}'
LATEPIPE = CLANG == '5.0.1'
DATA = ROOT / 'data' / VERSION
BUILD = ROOT / 'build' / VERSION

# Libraries Blizzard linked prebuilt (byte-identical between 2.6.2 and 2.7.7): with
# any compiler other than 5.0.1 they are still built with the 2.6.2 recipe, in
# LEGACY_BUILD, and linked into BUILD. Mirrors D3_LEGACY_REGEX in CMakeLists.txt.
LEGACY_DIRS = ('src/Contrib/Contrib/DemonWare/', 'src/Contrib/Contrib/BlizzardProtoBuf/',
               'src/Contrib/Contrib/lua/', 'src/Contrib/Contrib/OggVorbis/')
MIXED = CLANG != '5.0.1'
LEGACY_BUILD = ROOT / 'build' / f'{VERSION}-legacy'


def is_legacy(relative: str) -> bool:
    """True for a repository-relative source path built with the 2.6.2 recipe."""
    return MIXED and any(relative.startswith(d) for d in LEGACY_DIRS)
