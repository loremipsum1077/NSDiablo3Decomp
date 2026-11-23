# Late GlobalOpt (the LatePipe plugin)

Every optimised C and C++ unit is compiled with one extra step in clang 5.0.1's `-O3`
pipeline: GlobalOpt, GlobalDCE and a fresh GlobalsAA after the inliner, where LLVM 6.0 runs
them. The step is a 30-line clang plugin, `toolchain/latepipe/LatePipe.cpp`, loaded with
`-fplugin=build/latepipe/<hash>/LatePipe.so` from `CMakeLists.txt`. Lua and LibTomCrypt/LibTomMath (built `-O0`) are
excluded, and `D3_STOCK_PIPELINE_SOURCES` can keep individual files on the stock pipeline (it
is empty; see below).

| | Stock clang 5.0.1 `-O3` | LLVM 6.0 `-O3` | This build |
|---|---|---|---|
| GlobalOpt before the inliner | yes | yes | yes |
| GlobalOpt + GlobalDCE + GlobalsAA after "Deduce function attributes in RPO" | no | yes | yes (plugin) |
| Everything else | 5.0.1 | 6.0 | 5.0.1 |

## Finding

The original objects look as if GlobalOpt also ran after inlining. Symptoms, all in code
whose source was otherwise right:

- file statics split into the original's names (`s_tHeroActorE.0.2` ... `.14.2` in
  SNOPreload.cpp; window statics in UIBossElements.cpp; `stCurrentDestination` in
  UIConsoleMainMenu.cpp; static-local key arrays in SPlayer.cpp and
  AchievementModifierEvaluationLogic.cpp; `hBullet` in Font.cpp);
- statics that become constants once their last store or load is inlined away
  (NavZoneIntersectPrim.cpp `sgvCrnDeltaA/B`, Wind.cpp, Player.cpp `PlayersGetCount`);
- a `__cxa_atexit` registered for an empty implicit destructor that the original does not
  register (UIConsoleNetworkOptionsNX64.cpp, UIConsoleEULA.cpp): the late GlobalOpt removes
  the empty destructor call.

Stock clang 5.0.1 runs GlobalOpt once, before the inliner, so none of these can be
reproduced by any source change. LLVM 6.0.0 added the second GlobalOpt + GlobalDCE to
`PassManagerBuilder::populateModulePassManager` (with a GlobalsAA rebuild after it).

## Evidence

- `opt -O3 -debug-pass=Structure`: LLVM 5.0.0 and 5.0.1 run GlobalOpt only before the
  inliner; 6.0.0 and 7.0.0 also run GlobalOpt + GlobalDCE after it.
- The game's DWARF producer string is "Rynda 1.4.9 based on LLVM 5.0.1". A vendor backport
  of the LLVM 6 change is the likely explanation.
- With the plugin, SNOPreload.cpp gets exactly the original's split-global names; stock gets
  none.
- No link-time optimisation: 0 cross-compile-unit references among 345,325
  inlined-subroutine DIEs. The extra step therefore runs per translation unit.
- Controls: the plugin loaded with no passes, and a two-step bitcode route, give
  byte-identical objects (61/61 files each); `opt -globalopt` on the bitcode equals
  GlobalOpt-at-the-end on 26/26 files. Only the LLVM 6 position (`EP_VectorizerStart`,
  after RPO function attributes; only Float2Int runs between the two points on 5.0.1) also
  matches Player.cpp's `PlayersGetCount`; GlobalOpt at `EP_OptimizerLast` does not.

The first measurement, notes and scripts are in `build/try/pipeline/` (local, not in git).

## Measurement

Two full builds of the same commit (`d6cc77f1`, the dev head on 2026-11-21) in separate
worktrees, one with the stock flags and one with this change (plugin flags exactly as
`CMakeLists.txt` produces them; the two `compile_commands.json` differ only by the
`-fplugin=` argument on 1,327 of 1,525 units).

try_file's masked comparison, every object of every source, before and after:

| Sources | Objects identical | Still match | Stop matching | Newly match |
|---:|---:|---:|---:|---:|
| 1,524 | 1,479 | 36,125 | 0 | 41 |

The 41: SNOPreload.cpp 16, UIBossElements.cpp 9, UIConsoleMainMenu.cpp 3 (including its static
initializer), Font.cpp 2 (including its static initializer), SPlayer.cpp 2 (including its static
initializer), AchievementModifierEvaluationLogic.cpp 2, and one each in jpge.cpp, UIConsoleEULA.cpp
(static initializer), UIConsoleNetworkOptionsNX64.cpp (static initializer), UIConsoleTouch.cpp,
Wind.cpp, NavZoneIntersectPrim.cpp and Player.cpp. RefString.cpp, SigmaTiming.cpp and
ControllerDebugView.cpp keep all their matches with the source fixes below.

Full-link gate (`tools/check --batch` on all 34,366 non-L functions of each build, labelled
the way `tools/land.py --sync` labels them):

| | Checker ok | O after `--sync` | Proof gaps | O that would break |
|---|---:|---:|---:|---:|
| stock | 31,056 | 33,534 | 2,508 | 0 |
| plugin | 31,092 | 33,570 | 2,512 | 0 |

`--sync` would turn 36 functions from M to O and none the other way. The other 5 of the 41
newly match the masked comparison but end in a proof gap at the gate (the static initializers
of Font.cpp, UIConsoleMainMenu.cpp and SPlayer.cpp: an original GOT target without a known
data identity; `UIBossEncounter::Enter_Close`: an unproven ADRP pair;
`EvaluateMonsterModifiers`: an unproven jump table, as before). Like any proof gap they become
O when their file is next landed, since try_file finds every instruction equal. No new split
symbol, deleted global or data placement change produced a failing verdict.

## What is and is not verified

Verified: the objects the plugin produces, function by function, against the original
(try_file's masked comparison over the whole tree, and the full-link `tools/check` gate); that loading the plugin with no passes changes nothing; that the plugin binary is
reproducible from the pinned recipe.

Not verified:

- The pass position is emulated, not proven. There are no sources of the vendor compiler;
  an unknown vendor pass could still exist, and the backport could differ from LLVM 6.0 in
  details the game's code never exercises.
- The Lua interpreter and LibTomCrypt/LibTomMath are not compiled with the plugin. They are
  built `-O0`, where the step does not run at all (it is part of the `-O2`/`-O3` module
  pipeline; the first measurement found the 168 LibTom objects byte-identical with it).
- The plugin binary is pinned for `linux/arm64` only. An `amd64` gate box needs its own pin,
  checked by building the tree once with it and comparing every object.

## The plugin

`toolchain/latepipe/LatePipe.cpp` registers the three passes at
`PassManagerBuilder::EP_VectorizerStart`. `toolchain/latepipe/latepipe.lock.json` pins:

- the SHA-256 of `LatePipe.cpp`,
- the build image (`d3-lua-build:5.0.1`: `d3-build:5.0.1` plus g++, with the same
  `/opt/llvm` 5.0.1 headers and clang binary) and its exact `g++ --version` line,
- the g++ flags (`-shared -fPIC -O2 -std=c++11 -fno-rtti ... -Wl,-z,nodelete`),
- the SHA-256 of the resulting `LatePipe.so` per Docker platform.

The binary is installed at `build/latepipe/<first 16 hex of its SHA-256>/LatePipe.so`
(ignored), so the compile flags name the exact binary: a different plugin is a different
command line, which rebuilds every object and invalidates every flag-keyed cache
(`build/status/equiv.py`, the pilot's `fast_compile.py`).

- `tools/build.py` builds it when it is missing (one `docker run` of `d3-lua-build:5.0.1`,
  no network) and refuses a file whose bytes differ from the pin.
- CMake's configure step runs `python3 toolchain/latepipe/latepipe.py --path`, which fails
  when the plugin is missing, differs from its pin, or `LatePipe.cpp` differs from the
  pinned source hash. The lock, the source and the script are configure dependencies.
- `tools/try_file.py` copies the flag from `build/2.6.2/compile_commands.json` like every
  other flag and stops with a clear message when the plugin file is absent.
  `build/status/equiv.py` and `build/large3` take their flags from try_file and mount the
  checkout at `/work`, so they pick it up unchanged.
- `tools/claim.py` copies `build/latepipe/` into each new claim worktree: the worktree is
  mounted at `/work`, where a symlink to the main checkout would not resolve.

To rebuild by hand: `python3 toolchain/latepipe/latepipe.py --build`. It never overwrites
an existing file; to replace a corrupt one, move it aside first.

Why g++ and `-z nodelete`: `d3-build:5.0.1` has no host C++ compiler or linker, and the
plugin must share the release clang's libstdc++ ABI (`RegisterStandardPasses` holds a
`std::function`). `llvm_shutdown` dlcloses plugins before it destroys the global extension
list that points into them; without `-z nodelete` clang crashes at exit.

Changing the plugin: edit `LatePipe.cpp`, build it with the lock's recipe, rebuild the whole
tree, compare every object and every checker verdict with the previous plugin, then update
`source_sha256` and `plugin_sha256` in the lock in the same commit.

## Stock-pipeline list

`D3_STOCK_PIPELINE_SOURCES` in `CMakeLists.txt` lists sources compiled without the plugin.
A file belongs there only when functions in it match today and stop matching with the late
step, because our source lacks code the original has. Fixing the source and removing the
entry is follow-up work; nothing else may be added without the same evidence.

The list is empty. It started with the three files whose functions stopped matching in the
first measurement; each kept a read or write alive only until the early GlobalOpt with code
that is not in the DWARF, or lacked code the DWARF records. Each was fixed by writing the
linker-removed functions the DWARF records (`DW_AT_low_pc 0`) and moved to the plugin:

| File | Was | Fix | Size check |
|---|---|---|---|
| `X/Sigma/RefString.cpp` | `sRefStringAssert` (not in the DWARF) read `s_bInitialized` | `RefStringIsInitialized` (line 292) returns it | 12 bytes, DWARF 12 |
| `X/Sigma/SigmaTiming.cpp` | `sSigmaTimingTestClock` (not in the DWARF) | `SigmaTimingSetMethod` (291) and `SigmaGetClockFrequency` (298) | 12 and 100 bytes, DWARF 12 and 100 |
| `X/Client/Debug/ControllerDebugView.cpp` | four setters declared, not written | `ToggleControllerDebugView` (58), `SetControllerDebugView` (64), `TouchPadDebugSetReferencePoint` (223), `TouchPadDebugRemoveReferencePoint` (231) | 28, 16, 28, 16 bytes, DWARF the same |

The removed functions leave no instructions, so their bodies are reconstructed: the setters
and `RefStringIsInitialized` from their names and the fields they name;
`SigmaGetClockFrequency` (an `int64` with a frame pointer) on the pattern of
`SigmaGetGameClockTicks` (its `TickCount` value 1000 and error line are not evidenced; neither
changes its size or any matched function). A `SetControllerDebugView` that also set
`bDebugView` compiled to 20 bytes and was rejected. The matched functions depend only on which
fields are stored and read, not on the reconstructed details.

To put a file on the list: add it with a one-line reason, in the same commit as the
measurement that shows which of its matching functions the late step loses.

## Workarounds that may now be unnecessary

These files carry comments that describe working around, or failing because of, the missing
post-inline GlobalOpt. They were left unchanged. With the plugin, a workaround may now be
unnecessary and unfaithful to the original, and a NON_MATCHING note may be stale; each
should be re-read and, where the workaround is not in the DWARF, removed once the file still
matches without it.

| File | Comment |
|---|---|
| `X/Client/Camera/CameraManager.cpp` | NON_MATCHING: needs post-inline GlobalOpt SRA of `lightPosOffset` |
| `X/Client/Debug/TargetingDebugView.cpp` | linker-removed function written so GlobalOpt does not fold `tDebugGlobals` |
| `X/Client/Definitions/PreferencesDefinition.cpp` | GlobalOpt folds `s_bBackupValid` to FALSE |
| `X/Client/Font/Font.cpp` | NON_MATCHING: `hBullet` static placement |
| `X/Client/UserInterface/Console/UIConsoleMainMenu.cpp` | NON_MATCHING: `stCurrentDestination` not split |
| `X/Client/UserInterface/Console/UIConsoleNetworkOptionsNX64.cpp` | NON_MATCHING: `__cxa_atexit` of an empty implicit destructor |
| `X/Client/UserInterface/Interface/UIBlizzconEnd.cpp` | global shrunk to a byte by GlobalOpt |
| `X/Client/UserInterface/Interface/UIBossElements.cpp` | NON_MATCHING x9: window static not split |
| `X/Client/Weather.cpp` | initializer shaped so no `_GLOBAL__sub_I` is emitted |
| `X/Client/Wind.cpp` | GlobalOpt only runs before inlining |
| `X/Common/Nav/NavZoneIntersectPrim.cpp` | NON_MATCHING x3: `sgvCrnDeltaA/B` not folded |
| `X/Common/UserInterface/Font.cpp` | global initialised so GlobalOpt keeps align 4 |
| `X/Server/Achievements/AchievementModifierEvaluationLogic.cpp` | NON_MATCHING: static-local array not split |
| `X/Server/SPlayer.cpp` | NON_MATCHING: static key arrays not split |
| `X/Sigma/DebugChannelInterface/XDebugChannelMemory.cpp` | read of `s_bMutexActive` kept so GlobalOpt does not delete it |

Found with `grep -rln GlobalOpt src include`. Several NON_MATCHING notes describe functions
the plugin now matches (see the measurement); their comments are stale.
