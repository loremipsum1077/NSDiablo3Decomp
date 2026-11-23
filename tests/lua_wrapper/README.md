# Lua interface evidence checks

Run these from the repository root after compiling the wrapper with `tools/try_file.py`:

```sh
python3 tests/lua_wrapper/extract_tables.py
cmp src/X/Common/generated-game-data/lua_tables.inc build/try/lua_wrapper_evidence/lua_tables.inc
python3 tests/lua_wrapper/audit_tables.py build/try/X_Server_LuaScript_LuaScriptInterface_wrap.cpp/obj.o
python3 tests/lua_wrapper/audit_calls.py build/try/X_Server_LuaScript_LuaScriptInterface_wrap.cpp/obj.o
```

The extractor uses the repository's original `data/2.6.2/main.elf`, symbol inventories and relative relocations. It emits reviewable C++ table initializers and a structured evidence manifest under `build/try/lua_wrapper_evidence/`. Bootstrap generates the same evidence manifest from the NSS. Both generators check the committed hashes. The local manifest records original addresses, sizes, SHA256 hashes of original bytes and pointer targets; it contains no copied binary or decompiler output.

The table audit compares compiled non-relocated bytes and resolved pointer targets (function/global names, interior offsets and string contents), rather than masking them. It covers 42 tables and 5,661 pointer targets, including the complete command and constant registrations. `swig_variables` has no surviving original storage symbol; its four getter entries are evidenced by the original `Hydra_Init` calls and data operands.

The call audit checks named direct-call and tail-call relocations at the corresponding original instruction offsets for byte-matching functions. Different-size or nonmatching functions are excluded. External targets absent from `functions.csv` remain explicitly unresolved; in this wrapper the four `strcmp` calls target `strcmp@plt` at original address `0x00c1d6a0`. These checks supplement the required full build/check and do not claim to replace it.

Runtime control flow was reconstructed from the current binary and DWARF. SWIG 1.3.29's `SWIG/Lib/{swiginit.swg,swigrun.swg,lua/luarun.swg}` in the upstream `swig/swig` repository corroborates its initialization and conversion patterns. This identifies a matching runtime family, not a proven exact generator release. The wrapper's argument diagnostics and validation typemaps are game-specific.

Run `python3 tools/gen_headers.py` followed by `python3 tools/recover_lua_bindings.py` to regenerate the binding evidence and wrapper batches under `build/try/lua_bindings_regenerated/`. The extractor reads wrapper locals and line numbers from the local NSS and native declarations from the generated DWARF headers. `config/lua_binding_recipes.json` records the reviewed argument conversions, overload defaults and five reconstructed arms. The small fixture under `tests/fixtures/` exercises the generator without game inputs.
