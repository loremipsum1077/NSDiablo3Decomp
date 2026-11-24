# NSD3Decomp

A C/C++ decompilation of **Diablo III 2.6.2 for Nintendo Switch**. This target
ships matching NSS debug files with DWARF types, symbols and ownership information,
which made evidence-based recovery of every function body possible.

## Project status

The decompilation is **complete as far as it will go**: every function in the game
is either matching byte for byte, written with a documented reason it does not match,
or accounted for as code the compiler emits elsewhere. Some game data is not shipped
here and is rebuilt locally from your own game files (see
[Generate private build inputs](#generate-private-build-inputs)).

<!-- progress:start -->
![Functions](https://img.shields.io/badge/functions-98.1%25-brightgreen) ![Code bytes](https://img.shields.io/badge/code_bytes-92.6%25-brightgreen) ![Missing code](https://img.shields.io/badge/missing_game_code-0-brightgreen) ![Status](https://img.shields.io/badge/status-decompilation_complete-blue)

<p align="center"><img src="docs/progress.svg" alt="Decompilation progress infographic" width="880"></p>

| | Functions | Original code |
|---|---:|---:|
| **Matching byte for byte** | 33,729 (98.1%) | 10,794,444 B (92.6%) |
| Written, not byte-identical | 631 | 854,432 B |
| Not written as its own symbol | 8 | 2,536 B |
| **Total counted** | 34,368 | 11,651,412 B |

Of the 639 functions that do not match, **613** have no known behavioural difference, **26** possibly differ, and **0** are missing code the game needs. Each one is listed with its reason in [remaining_functions.md](docs/remaining_functions.md). 2,191 excluded library functions (FMOD, SDK runtime) are not counted; see [library_functions.md](docs/library_functions.md).

<sub>Regenerate with `python3 tools/progress_graphic.py` after landing functions.</sub>
<!-- progress:end -->

The remaining non-matching functions are accepted as they are. Most differ only
because of compiler version, register allocation or instruction ordering and layout,
and each one's cause is recorded. Matching them further is welcome but not required. See the
[exact-decompilation operating plan](docs/exact_decompilation.md) for the compiler,
proof and build-resource background. Native desktop recompilation was out of scope;
the [native subsystem harness](desktop/README.md) is preserved for anyone who wants
to take it further.

The focus is this base target first. Decompilation of later patches is a future
ambition, not a supported target today. The repository supplies no game originals,
assets, keys or playable game; contributors supply their own legally obtained
inputs. Open source-origin questions and third-party notices are listed in the [provenance inventory](docs/source_provenance.md).

Recovered implementation lives in [`src/`](src/) (game code in [`src/X/`](src/X/),
middleware in [`src/Contrib/`](src/Contrib/)); declarations primarily live in
headers alongside source under `src/`. [`include/`](include/) contains compatibility
headers. Original owner paths and function identities are indexed
in [`data/2.6.2/`](data/2.6.2/). See the [documentation index](docs/index.md)
for tooling and technical references.

A function is **O** when its compiled code equals the original: the checker
accepts it on the full build, or every instruction is equal and the checker's
only objection is a reference to something not yet defined. `python3 tools/land.py`
applies that rule ([landing](docs/decomp_workflow.md#landing)). Progress counts
are O functions and their original code bytes; they do not establish a
byte-identical rebuilt executable or a runnable game.

### License

The project-authored tools listed in [REUSE.toml](REUSE.toml) are available under the
[MIT License](LICENSES/MIT.txt). Reusers may modify, redistribute and sell them
with the copyright and permission notice. Third-party components carry their own
terms. See the [source provenance inventory](docs/source_provenance.md) for
upstream notices and the status of recovered game and SDK source.

### Generate private build inputs

From the repository root, point the bootstrap at your own extracted Switch 2.6.2
directory containing `exefs/main` and `romfs/DiabloIIINX64r.nss`.

```sh
python3 tools/bootstrap_game_data.py /path/to/game --build-protoc
```

The script verifies the game version and recovers the 24 game-specific Protobuf
schemas, controller maps, SWIG registrations and NVN graphics entry points locally.
It also recovers achievement events, font colors, hotbar anchors, Mirror Image attack names,
the Switch issuer mapping, and the sound, particle and saved-attribute tables.
It also writes the Lua table evidence used by the wrapper audit under `build/try/lua_wrapper_evidence/`.
It writes ignored files under `src/X/Common/`, where the existing build expects
them. It requires Python 3.10+ and an AArch64-capable `llvm-objdump`. On the first
run, `--build-protoc` downloads the checksum-pinned public Protobuf 2.6.0 release
and builds its compiler under `build/` using a host C++ compiler and Make.
For offline generation, pass `--protoc /path/to/protoc-2.6.0` or
`--protoc-archive /path/to/protobuf-2.6.0.tar.bz2`. The script checks generated
bindings against the hashes of the matching project sources before installing them.
When migrating an existing private checkout, `--bindings-from /path/to/old/src/X/Common/Protobuf`
reuses the 48 previously verified files and still regenerates every table from the game.
For a full build, place the verified inputs under `build/exefs/` and `build/romfs/` and run
`python3 setup.py`. With those inputs in place the wizard can run the bootstrap for you.

Want to help?

1. Fork/clone, then run **`python3 setup.py`** from the repository root.
   The guided terminal wizard checks prerequisites and explains fixes before asking consent for downloads or builds.
2. Use `python3 setup.py --check` for read-only unattended diagnostics, or
   `python3 setup.py --plan` to preview stages. See the [startup guide](docs/contributor_startup.md).
3. Coordinate one owner and inspect its authentic evidence.
4. Recover locally and run `python3 tools/land.py --no-commit <file>`.
5. Submit a fork PR under [CONTRIBUTING.md](CONTRIBUTING.md).

Discord coordinates claims across clones; local queue locks do not.

[Completion plan](docs/project_plan.md) · [Technical reference](docs/technical_reference.md)
· [Local reports and PR checks](docs/contributor_ci.md)
