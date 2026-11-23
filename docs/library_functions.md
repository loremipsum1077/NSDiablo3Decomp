# Excluded functions (label L)

`data/2.6.2/functions.csv` uses one more Quality label besides O, M, W and U:

| Label | Meaning | Counted in progress |
|---|---|---|
| O | matching | yes |
| M / W | written, not matching | yes |
| U | unwritten | yes |
| L | excluded from the project: library, SDK runtime, or skipped | no |

L is for code that is not part of the game and cannot be reproduced here: either it cannot
be decompiled from evidence (no DWARF, no available source, no owner file), or it is
toolchain runtime that this build cannot define without changing how the program links. It is left out of every total (function and
byte percentages, the status page, the equivalence tiers) and is never sent to the checker.
The label comes from the shared nx-decomp-tools convention (`tools/common`, `tools/check`).

## What is labelled L

All 2,180 functions (815,076 bytes) that `function_files.csv` maps to no source file. They
are one contiguous block starting at `0x7100994f30`: FMOD Studio low-level API 1.10.05 and
the code built into it (its FLAC and Vorbis decoders, the NNAudio output plugin, the DSP
units). FMOD is closed source and these objects carry no debug info.

Nine Nintendo SDK startup functions (424 bytes) at the very start of the image, from
`rocrt.cpp`, `rocrt_LinkerSymbolGetter.cpp` and `MainRuntimeObject.cpp`: `_init`, `_fini`,
`__aeabi_atexit`, `__cxa_atexit`, `__cxa_finalize`, `__nnDetailNintendoSdkRuntimeObjectFile`,
`__nnDetailNintendoSdkRuntimeObjectFileRefer`, `nndetailRoGetRoDataStart` and
`nndetailRoGetRoDataEnd`. They have DWARF and an owner, but they are the SDK's runtime
object, not game code, and defining them in our build alters the link: `_init`/`_fini` set
`DT_INIT`/`DT_FINI`, and the `atexit` trio rebinds 114 existing callers under `-Bsymbolic`
(notes in `build/try/draft/rocrt*.txt`).

## Categories and reasons

The checker knows only the letter L, so every exclusion uses it. The reason is recorded in
`config/excluded_functions.csv` (Address, Name, Category, Reason):

| Category | Meaning | Listed in the file |
|---|---|---|
| library | closed-source third-party code with no debug info (FMOD) | no: any L row not in the file |
| sdk-runtime | Nintendo SDK startup code this build cannot define | yes, one row each |
| skipped | game or vendored code that cannot be reproduced or identified | yes, one row each, with the specific reason |

`skipped` is for single functions where the evidence needed no longer exists or the checker
cannot identify the function, and where writing it would mean inventing code. It is not for
functions that are merely hard: those stay U, or are written and marked M. Currently two:

- The two static `qsort_helper` functions of LibTomCrypt (`der_encode_set.c`,
  `der_encode_setof.c`, 644 bytes): vendored and compiled, but with no DWARF and a shared
  name the checker cannot tell them apart. They could move to M if the inventory names are
  ever made resolvable.

The `Blizzard::Lock::RefCounted` destructors (D2 and D0, 24 bytes) were on this list until
2026-11-21. They are emitted only with the class's vtable, which the original unit needed for
Jobs code the linker removed. That code is recorded well enough to write: the DWARF gives the
Job layout and `Job::~Job` (Job.cpp:35) with its size, inline tree and line rows. With
`Jobs.h` and that one destructor written, both RefCounted destructors match and `~Job`
compiles to its recorded 0x144 bytes.

`tools/status_report.py` reports the count and bytes per category and fails if the file lists
a function that is not labelled L. Adding a function to a category means setting its label to
L and adding its row, with a reason, in the same commit.

## What is not labelled L

- LibTomCrypt and LibTomMath also have no DWARF, but their upstream source is identified and
  vendored (`src/Contrib/Contrib/DemonWare/external/README.md`). They are faithful source
  that does not byte-match, so they stay M and count.
- Lua and every other unit with DWARF.

## Changing the set

A function leaves L by getting an owner in `function_files.csv` (through DWARF or
`config/function_file_supplement.csv`) and being set back to U; the landing flow then treats
it like any other function. The FLAC and Vorbis decoders inside FMOD derive from open-source
libraries and could be candidates if their exact versions are ever identified.
`tools/gen_functions_csv.py` keeps existing labels when it regenerates the inventory.
