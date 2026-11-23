# Technical reference

Commands here run from the repository root. Begin with the numbered [contributor startup](contributor_startup.md); this reference describes lower-level tools and their side effects.

# Why 2.6.2 ?

This is the version on the physical copy of the game which shipped with DWARF files which allows decomp to be done easier

# How to dump the game to help?

Prerequisites

- A built hactool. Source: https://github.com/SciresM/hactool. On macOS or Linux, run make.
- Your own Switch keys in prod.keys. Put the file at ~/.switch/prod.keys, where hactool looks by default. You can also pass -k /path/to/prod.keys explicitly. The file needs to be dumped from your own console.
- About 30 GB of free space. The raw NCAs take about 14 GB and the unpacked files about 13 GB.

1. Extract the XCI partitions

mkdir -p xci_out
./hactool -t xci --disablekeywarns --outdir=xci_out game.xci

This produces update/, logo/, normal/ and secure/. The game is in secure/, and its largest .nca is the main program NCA.

2. Find the main NCA

ls -S xci_out/secure/*.nca | head -1

Use that path as <MAIN_NCA> below. For this dump it was 99a8e2c711520b5c5378be8d5009e43c.nca. Other dumps will have a different hash.

3. Unpack ExeFS and RomFS

mkdir -p unpacked/exefs unpacked/romfs
./hactool -t nca --disablekeywarns \
  --exefsdir=unpacked/exefs \
  --romfsdir=unpacked/romfs \
  <MAIN_NCA>

- unpacked/exefs/ holds main, main.npdm, rtld and sdk, which are the NSO executables.
- unpacked/romfs/ holds the game data (the CPKs/ folder) and the NSO/NSS files.

4. Locate the DWARF debug files

The DWARF files are the .nss files in the RomFS root. They came out with step 3, so there is nothing separate to extract.

ls -l unpacked/romfs/*.nss
file unpacked/romfs/*.nss      # should say "ELF 64-bit ... ARM aarch64 ... with debug_info, not stripped"

Optional: inspect the debug info

llvm-dwarfdump --debug-info unpacked/romfs/<name>.nss | less
readelf -S unpacked/romfs/<name>.nss | grep debug

Notes

- -t xci and -t nca set the input type explicitly. The tool can usually detect it, but this avoids ambiguity.
- --disablekeywarns only silences key-loading warnings and doesn't change the output.
- The default action is extract. You don't need -x.
- Add -y to verify hashes and signatures if you want to check the dump is intact.

Put the exefs and romfs files in a folder named "build" in the root of this repo. It should be gitignored already.

So the layout is `build/exefs/{main,main.npdm,rtld,sdk}` and `build/romfs/*.nss`.

# Getting main.elf (setup)

hactool only decompresses an NSO; it cannot produce an ELF. The conversion is done by
[nx2elf](https://github.com/shuffle2/nx2elf), which `tools/setup.py` fetches for you
(prebuilt, from open-ead/nx-decomp-tools-binaries, into the gitignored `toolchain/`).

Prerequisites: python3, git, a Rust toolchain (`cargo`, to build the checker), and `llvm`
(`brew install llvm` on macOS, `apt install llvm` on Linux) for `llvm-nm`.

```
git submodule update --init --recursive
tools/setup.py                      # defaults to build/exefs/main
# or: tools/setup.py /path/to/main
```

This does four things:

1. Converts the ExeFS `main` NSO to `data/2.6.2/main.elf` (and `data/2.6.2/main.uncompressed.nso`).
   The NSO is checked against SHA-256 `9f0cb00d...c006c115` and the ELF against
   `9c246371...f88be90d`, so a wrong-version dump fails loudly. Note this repo targets 2.6.2.
2. Generates `data/2.6.2/functions.csv` (every function: address, size, mangled name) from
   `build/romfs/DiabloIIINX64r.nss`.
3. Builds `tools/check`, `tools/listsym` and `tools/decompme` from the
   [nx-decomp-tools](https://github.com/open-ead/nx-decomp-tools) submodule.
4. Applies `tools/patches/*.patch` to the checker (trap-word comparison, `name@file`
   lookup through the link map, batch mode, flow-sensitive ADRP pairing) and builds it.
5. A mangled name that several files define (file-statics, anonymous-namespace functions) is
   written `name@File.cpp` in `functions.csv`, the shortest file-path suffix that tells the
   copies apart; `tools/check` resolves it through the link map. The list is also in
   `data/2.6.2/duplicate_names.csv`. Copies with no DWARF file stay `FUN_<vaddr>`.

Manual equivalent of step 1 (what setup.py runs):

```
nx2elf build/exefs/main --export-elf data/2.6.2/main.elf --export-uncompressed data/2.6.2/main.uncompressed.nso
```

`data/2.6.2/main.elf` is stripped. The DWARF lives only in the unstripped ELFs
`build/romfs/DiabloIIINX64r.nss` (matches `exefs/main`, BuildID `eafc2cef...`) and
`DiabloIIINX64ra.nss` (matches `DiabloIIINX64ra.nso`, BuildID `7247a8c6...`). Their
segments sit at the same vaddrs as `main.elf`, so addresses carry over directly.

```
llvm-dwarfdump --debug-info build/romfs/DiabloIIINX64r.nss | less
llvm-nm -S -n --defined-only build/romfs/DiabloIIINX64r.nss
```

# Build

For an isolated setup/build/check reproduction from committed sources, use
`python3 tools/reproduce.py --help`. It records input and tool hashes, compares
regenerated inventories, and retains stage logs without changing the live inventory.
The default is preflight; `--run` executes in a new private checkout. See
[the reproducibility guide](p20_reproducibility.md) for prerequisites and evidence
boundaries. Setup pins converter binaries in `tools/setup_lock.json` and rejects
an existing checkout whose revision or binary hashes differ.

The compiler is clang 5.0.1, cross-compiling to aarch64. The DWARF in the `.nss` records the
compiler of every compile unit: all of them are Nintendo's clang fork based on LLVM 5.0.x
(5.0.1 for the game and most middleware, a 5.0.0 snapshot for Lua and BlizzardProtoBuf). It is
a Linux prebuilt, so the build runs in Docker. Requires Docker.

```
python3 tools/build.py            # first run builds the image (downloads LLVM), then configures + builds
python3 tools/build.py --clean    # full rebuild
```

For the pinned patched backend scoped to recovered Lua sources, run
`python3 tools/build.py --lua-compiler`. This bootstraps the compiler from
verified source and the checked-in patch, then performs the normal project
link. Other sources retain their normal compiler commands. See
[Lua build instructions](../toolchain/lua/README.md) for offline inputs, provenance,
and switching back to the upstream backend.

Output is `build/2.6.2/d3`, a shared ELF linked by `ld.lld`. Sources live under `src/`,
mirroring the original tree recorded in the DWARF (`src/X/Client/CGame.cpp` for
`X/Client/CGame.cpp`). The
compile flags in `CMakeLists.txt` are proven: with them, the zlib 1.2.3 linked into the game
compiles to 34/34 byte-identical functions. See [docs/compile_flags.md](compile_flags.md)
for the evidence.

Every optimised unit also loads a compiler plugin, `-fplugin=build/latepipe/<hash>/LatePipe.so`,
that adds LLVM 6.0's post-inliner GlobalOpt + GlobalDCE to clang 5.0.1's `-O3` pipeline, as the
original compiler behaves. `tools/build.py` builds it on demand in `d3-lua-build:5.0.1` and its
SHA-256 is pinned in `toolchain/latepipe/latepipe.lock.json`. Lua, LibTomCrypt/LibTomMath and
the few files in `D3_STOCK_PIPELINE_SOURCES` are compiled without it. See
[docs/late_globalopt.md](late_globalopt.md).

The link rule also writes `d3.map.provenance`, binding the ELF and link map with
SHA256 hashes immediately after a successful link. The maintained checker patches
require this record for qualified `name@file` identities. For a stale record, remove
`build/2.6.2/d3.map.provenance`; a missing record forces a fresh link on
`python3 tools/build.py`. Never create provenance for an old
ELF/map pair. This detects mixed build artifacts and does not replace source-owner
review.

`toolchain/link_anchor.cpp` is linked in only so `d3` always has a `.rela.dyn` section, which
`tools/check` requires. It adds no functions.

The data and linkage ledger separately inventories
globals, vtables, imports, initialization and TLS. Run `python3 tools/data_coverage.py --help`
to reproduce it; stored-byte equality does not prove relocated targets or source ownership.
`python3 tools/validate_data.py --help` runs the relocation-aware validator
with verified ELF/map provenance, preserving unresolved objects and checking pointer identities.

# What the debug info gives us

The `.nss` DWARF describes every type, every function signature and every inlined call. Three
tools turn it into something to work from (`tools/setup.py` runs the first two; they share an
index cached at `build/dwarf/index.pkl`, built in about half a minute).

- `python3 tools/gen_headers.py` writes a reference copy of every original source file to
  `build/dwarf/headers/`, mirroring the original tree. Each file has the types declared there
  (member offsets, base classes, access, virtual slots, method declarations with parameter
  names), the includes and forward declarations they need, a `static_assert` on each type's
  size, and a list of the functions and variables defined in the file. The Nintendo SDK
  headers the game uses are generated too.

  To work on a file, copy its header into `src/` with
  `python3 tools/gen_headers.py --adopt X/Sigma/Math/Sphere.h`. From then on it is
  hand-maintained; `--adopt` never overwrites.

  Reference generation uses canonical representatives for grouped DWARF records.
  `python3 tools/verify_dwarf_reproducibility.py --workers 2 4 --output build/dwarf-proof`
  checks independent uncached indexes, all reference headers and both helper CSVs.

  Two limits. Function bodies are not in the DWARF. Class templates exist only as
  instantiations, so a template is emitted as a forward declaration plus one example
  instantiation inside `#if 0`, and has to be written by hand. Until then, headers that hold
  such a template by value do not compile on their own.

- `python3 tools/inline_rank.py` writes `data/2.6.2/inline_helpers.csv`: every inline function,
  ranked by how many functions in the binary it was inlined into
  (`Callers,Sites,File,Line,Function`). A function usually only matches once the helpers inlined
  into it are written the way the original was, so the top of this list is the work that unlocks
  the most. `--prefix X/Sigma/` restricts the printed list to one area.

- `python3 tools/helper_ledger.py` writes `data/2.6.2/helper_ledger.csv`: the same helpers with
  how many of their callers are strictly O-quality, caller addresses, source-presence and
  body heuristics, and the smallest remaining caller file. Template instantiations are
  grouped: an O caller supplies evidence for its own context, not every specialization.
  It prints high fan-in families lacking exact evidence, source-defined families and
  possible missing-body queues; these are research leads, not established blockers.
  `python3 tools/helper_audit.py --out /tmp/helper_audit.json` exports a read-only audit
  against committed HEAD, with ranked named callers, strict totals and input hashes.
  The [P06 prerequisite map](p06_prerequisites.md) records reviewed consumer needs
  and bounded follow-up jobs.

- `python3 tools/dwarf_fn.py <mangled name>` prints one function's prototype, locals and tree of
  inlined calls.

# Decompiler output (optional)

`tools/ghidra_export.py` writes Ghidra's decompiled C for every function to
`build/ghidra/decomp/`, one file per original source file (for example
`build/ghidra/decomp/X/Client/CGame.cpp`). Each function is headed by its address, size and
mangled name from `functions.csv`, and carries the DWARF names for parameters, locals and types.

```
GHIDRA_INSTALL_DIR=/path/to/ghidra python3 tools/ghidra_export.py
```

The first run imports `build/romfs/DiabloIIINX64r.nss` into a Ghidra project at
`build/ghidra/d3_decomp_262` with its DWARF applied and the image base at `0x7100000000`, so
Ghidra addresses equal those in `functions.csv` (about 10 minutes). The export itself takes
about as long again. The project can also be opened in the Ghidra app, but not while the
script is running. Functions Ghidra never defined are listed in `_missing.csv`.

# Validating the decomp

Decompiled functions are validated by compiling them and comparing the bytes against
`data/2.6.2/main.elf`, using the maintained patched nx-decomp-tools checker (same as botw/other Switch decomps).

- `data/2.6.2/functions.csv` is the list of functions (`Address,Quality,Size,Name`).
  `Quality` is `U` (not decompiled), `O` (recorded matching-quality claim for one function), `W` (written but not matching yet;
  `tools/check <name>` sets this), `m`/`M` (accepted non-match with a `// NON_MATCHING:`
  comment), or `L` (library code excluded from the project and from every total; see
  [library_functions.md](library_functions.md)).
- `data/2.6.2/data_symbols.csv` holds the known global data symbols (`Address,Name`).
- Both are generated from the `.nss` by `tools/gen_functions_csv.py` (run by `tools/setup.py`).
- `data/2.6.2/function_files.csv` maps every function to the original source file it was
  compiled from (`Address,File,Compiler`), generated from the DWARF by
  `tools/gen_function_files.py`. Put a function in the file listed there. Functions with an
  empty `File` have no accepted ownership evidence. When a compile-unit range omits a
  function, the generator can recover its emitting unit from an unambiguous concrete
  subprogram range covering the exact function extent. Declaration paths do not override
  emitting-unit ownership.

These are per-function comparison tools, not proof of a fully linked byte-identical
rebuilt executable. Existing O labels do not establish fresh verification.
Object-only matches (including `try_file.py`, which masks relocation operands) are
provisional. Acceptance also requires authenticated source, relocation/data/unwind
checks and fresh normal/Lua link evidence bound to the candidate; see
[contributor validation](../CONTRIBUTING.md#validation-and-review). Progress summaries
count inventory labels and original function sizes, not overall effort or runtime
completion. The vendored `tools/common` reporting tools are not a separate acceptance
gate and their summary wording must be interpreted with these limits.

Low-level workflow for a function (after setup in the primary clone; follow the
[contributor workflow](../CONTRIBUTING.md) for a claimed worktree):

1. Write it under `src/`, in the file `function_files.csv` lists for it.
   `python3 tools/dwarf_fn.py <mangled name>` prints what the DWARF records about it: return
   type, parameter and local names, the source file and line, and every call that was inlined
   into it with the helper's own file and line. Use the original names, and write the same
   inline helpers: the function often only matches when they are structured the same way.
2. `python3 tools/build.py`
3. `tools/check <mangled name>` prints `OK` and sets `Quality` to `O` on an exact match, or
   an asm diff if not.
4. `tools/check` with no argument checks every non-`U` function in the current build.
   This alone does not cover newly recovered U identities or prove fresh both-mode
   baseline/candidate preservation; retain the complete acceptance evidence before merging.
5. `tools/listsym` lists symbols in `build/2.6.2/d3`.

`src/X/Client/CGame.cpp` and `src/X/Client/Chroma/ChromaEffects.cpp` each have one tiny
function that already matches, as an example.
