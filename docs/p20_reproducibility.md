# P20: clean setup, build and checker reproducibility

P20 remains open. A successful clean compilation is useful evidence, but does not by
itself reproduce setup, generated inventories or checker acceptance. P10 middleware
provenance and generation work remains open. P19 completed an audit; its remaining
trust and rollout jobs are not waived by reproducibility work.

## Inputs and isolation

Use a new checkout at an explicit Git revision, initialize its recorded submodule
commits, and supply only the private inputs below. Do not copy an existing `build/`
tree, compiler objects, checker executable, DWARF index or Python environment.

| Input | Purpose | Expected identity |
| --- | --- | --- |
| `build/exefs/main` | Original NSO conversion | SHA-256 `9f0cb00de2ae3c1bd05757d100e834d5480bb56d51aa664103058a7c7006c115` |
| `build/romfs/DiabloIIINX64r.nss` | Symbols, ownership and DWARF generation | Record SHA-256 before generation; the `r` dump matches main, while `ra` is a different program |
| `tools/common` | Viking and Python support | Gitlink revision, plus ordered repository patches |
| `toolchain/nx-decomp-tools-binaries` | `nx2elf` | Record commit and executable SHA-256 |

Setup verifies the converted ELF against SHA-256
`9c246371f89c55c2f3bbacc69d0be2c1c60f6a070d66284ce5869171f88be90d`.
A pre-existing valid ELF permits setup to skip conversion, so supplying that ELF is
not evidence that NSO conversion ran. Keep private game bytes outside Git; retain
hashes, commands, exit statuses and summaries instead.

Prerequisites are Git, Python with venv/pip, LLVM symbol/DWARF tools, Rust/Cargo and
Docker with Compose. Record host architecture and actual tool versions. The game
compiler runs in Docker; LLVM tools used to parse the NSS run on the host and can
have a different version. The effective linker is the `ld.lld` resolved by CMake,
not necessarily a binary from the LLVM compiler archive.

## Isolated runner

`tools/reproduce.py` defaults to preflight. Add `--run` to snapshot the selected
committed revision, import raw inputs, execute setup, compare inventories, build
and run regression. Commit implementation changes first: working changes are
excluded from the snapshot. A report and stage logs survive failure; the new
checkout is retained for inspection.

The host has an existing Python environment and an offline Cargo dependency cache.
The following invocation selects them explicitly. It exercises fresh project and
checker builds but does not claim those dependency environments were created
from scratch. The cache location is host-specific and must be replaced on another
machine. The output report establishes which stages actually completed.

```sh
export PATH="$HOME/.rustup/toolchains/stable-aarch64-apple-darwin/bin:$PATH"
export CARGO_HOME=/private/tmp/p19-checker-8o4jsoj2/cargo
export CARGO_NET_OFFLINE=true
python3 tools/reproduce.py \
  --revision HEAD \
  --nso build/exefs/main \
  --nss build/romfs/DiabloIIINX64r.nss \
  --binaries toolchain/nx-decomp-tools-binaries \
  --python "$PWD/tools/common/.venv/bin/python" \
  --use-existing-python \
  --output build/p20/clean-run.json \
  --run
```

Omit `--use-existing-python` and select a base Python interpreter to exercise
creation of the setup venv. That mode requires the Python packages to be
available from a configured index or local distribution cache. Offline Cargo
requires its own prepopulated dependency cache. Neither mode replaces the live
checker or changes live quality labels; candidate validation remains isolated.

## Generation and validation commands

Run these commands in the new checkout, after placing the two private inputs:

```sh
git submodule update --init --recursive
python3 tools/setup.py build/exefs/main
python3 tools/build.py --reconfigure
python3 tools/checker_regression.py --checker tools/check --output build/p20/regression.json
```

Setup executes these generators in order:

```sh
python3 tools/gen_functions_csv.py
python3 tools/gen_function_files.py
python3 tools/gen_functions_csv.py
python3 tools/inline_rank.py --top 0
python3 tools/gen_headers.py
```

The first symbol pass creates functions; ownership generation then permits the
second pass to qualify duplicate function names. Setup also converts the NSO,
fetches converter binaries, prepares Python dependencies, applies the checker
patch series and builds the checker. `--skip-check-build` is a useful diagnostic
option but cannot satisfy clean checker-build acceptance.

`functions.csv` preserves existing quality labels by address. Its existing `O`
labels are input claims, not new verification. Compare generated identities,
addresses, sizes and ownership separately from quality labels, and account for
all drift against the committed inventories. Record hashes of `functions.csv`,
`function_files.csv`, `duplicate_names.csv`, `data_symbols.csv` and
`inline_helpers.csv`. Reference headers under `build/dwarf/headers` are generated
analysis aids; adopted files in `src/` remain hand maintained.

The regression helper runs in a copied snapshot and checks every non-`U` row. It
preserves the caller's inventory. A nonzero result with a durable report is a
recorded validation failure, not a successful rollout. Its generic report says
“Existing built ELF; no fresh source build”; a surrounding clean-run manifest
must establish the actual build lineage and must not relabel the regression
result as a pass.

## Durable evidence contract

A reviewable run must retain the following outside disposable build directories:

- Repository revision, dirty-tree state, gitlinks, ordered patch hashes and the
  full compiled checker source manifest. Tie the executed checker hash to that
  source, Cargo lockfile, compiler version and successful fresh build command.
- Private input hashes, converter identity and conversion output hash; generated
  inventory hashes and a structured comparison to committed data.
- Docker image ID/digest and platform, compiler/linker paths, versions and hashes,
  CMake/Ninja versions, dependency versions and exact configure/build commands.
- A fresh build log and hashes for `d3`, `d3.map` and link-time
  `d3.map.provenance`; retain the provenance contents so their binding is auditable.
- Checker fixture/Rust test summaries and the full isolated inventory regression,
  including every rejected existing `O` row. Preserve failed stage, command,
  exit status and log if any stage stops early.

Hashing a checker and hashing patches in the same report does not prove the
checker was compiled from those patches. Likewise, a source revision plus an
old object cache does not prove a fresh build. Record both construction and
execution, and distinguish a cached container image from an image rebuilt from
its Dockerfile.

## Remaining reproducibility boundaries

The initial audit found these boundaries; run evidence must state which were
resolved by the executed tooling and which remain:

- The container uses the `ubuntu:20.04` tag and apt packages without a complete
  immutable package snapshot. A compiler release number alone is not an archive
  checksum or a complete toolchain lock. Matching output across container
  architectures requires measured evidence.
- Python requirements pin direct versions, but do not provide a complete
  hash-locked distribution set. Cargo lockfile and Rust version must both be
  captured; host Rust availability must not be inferred from an old checker.
- NSS parsing depends on host LLVM output. Record its version and input hash.
  The DWARF pickle cache now checks parser hash, schema and NSS filesystem
  identity; a clean claim still requires an empty build tree and full input hashes.
- A build of checked-in middleware does not establish P10 upstream provenance or
  reproducible generation of all recovered code. No fixture-only result becomes
  landed game coverage through this exercise.
- The stricter checker has known unresolved existing `O` rejections.
  Those jobs remain deferred under their recorded evidence
  triggers; do not weaken checks or deploy a candidate merely to make P20 green.

P20 can close only when a clean checkout reproduces the required inventories and
validation with these prerequisites documented, discrepancies reconciled, and
required proof retained durably. A clean-build-only run or a blocked setup is
explicitly partial evidence.

## Initial validation on this host

The initial read-only audit on 2026-10-19 found Docker 27.4.0, Compose
v2.31.0-desktop.2, Python 3.14.7 and Homebrew LLVM 22.1.8. Docker daemon access
required sandbox escalation. Rust was absent from `PATH`, but native toolchain
executables were present under
`~/.rustup/toolchains/stable-aarch64-apple-darwin/bin`.

A separate fresh baseline checkout completed all 571 game build steps using the
existing `d3-build:5.0.1` image and container networking disabled. This establishes
clean compilation of that source snapshot with that image. It does not establish
image construction, NSO conversion, generation or a fresh checker build. Any
supplementary checker run using copied tools/reference inputs must retain that
limitation. The clean runner's report is the authoritative record for subsequent
end-to-end attempts and their stage outcomes.

## Validation result

P20-F01 validation covered the delivered tooling and
its real execution against source revision `1327d77`. The bounded workflow job is
complete; P20 remains partial. No game source, quality label or live checker was changed.
Strict inventory totals remain **13,662 O functions / 3,188,172 bytes**.

Clean setup converted the supplied NSO, rebuilt its DWARF index and headers, applied
all nine checker patches, and compiled all three checker tools with Cargo `--locked`
in an empty target directory. Four inventories reproduce byte for byte. The fifth,
`inline_helpers.csv`, retains 22,298 rows and identical caller/site counts, paths and
lines, but changes 140 descriptions. The index merges worker results in completion order and retains
the first grouped function description; P20-F02 covers canonical representative
selection and reviewed regeneration. The runner correctly reports failure at the
inventory gate and skips its subsequent build/check stages.

A separately recorded fresh build in that same isolated checkout completes all 571
steps. Its newly built checker checks 14,644 non-U rows: 12,185 pass and 2,459 mismatch.
Exactly 1,477 existing O labels are rejected, with the **same identities** as the
previous P19-F04 evidence—zero newly rejected or resolved identities. This reproduces
the known strict-checker result; it does not establish that those O labels are valid.
All 149 tooling tests pass with that fresh checker. An initial run against the old
installed checker failed 101 assertions; it was left untouched.

This run reused the explicitly recorded
Python environment, Cargo dependency cache and compiler image. P20-F03 covers
independent dependency provisioning and complete locks. Deferred P19 investigations
retain their existing revisit conditions.

## P20-F02 completed: canonical DWARF generation

P20-F02 closes the helper-description drift
reported above. Independent uncached builds with two and four workers and distinct
Python hash seeds reproduce all 2,983 reference headers and both 22,298-row helper
CSVs byte for byte. Canonicalization preserves inline evidence, function addresses,
object sizes and ordered non-static members, while restoring omitted nested
declarations. Generated inventories are reconciled.

The new regression suite has 23 passing tests. Strict O coverage remains 13,662
functions / 3,188,172 bytes. This closes P20-F02 only: complete dependency provisioning
(P20-F03), P10 provenance and deferred P19 remediation still prevent P20 closure.
