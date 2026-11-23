# NSD3Decomp

An experimental C/C++ decompilation of **Diablo III 2.6.2 for Nintendo Switch**.
This target has matching NSS debug files with DWARF types, symbols and ownership
information, which makes evidence-based recovery possible. Function bodies still
need reconstruction and strict validation.

The owner-approved priority is **full exact decompilation first**, with native
desktop recompilation deferred. Reviewed nonmatching source is useful staging,
but does not satisfy completion. See the [exact-decompilation operating plan](docs/exact_decompilation.md)
for compiler, proof and build-resource prerequisites. The existing
[native subsystem harness](desktop/README.md) is preserved for later work.

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
