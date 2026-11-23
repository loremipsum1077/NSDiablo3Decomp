# Compile flags for 2.6.2

The DWARF in the `.nss` names the compiler of every compile unit but not its command line, so
the flags were found by experiment with a compiler-sweep tool. The game links in open-source
libraries built by the same compiler in the same build tree. Their source is known exactly, so
known source permits controlled flag comparisons; headers, ABI and toolchain differences
still need independent checking.

## Result

```
-O3 -mcpu=cortex-a57 -fno-strict-aliasing -mno-implicit-float -ffunction-sections -fPIC -g
-fsigned-char -funwind-tables -fno-exceptions -fno-rtti
```

These base flags reproduce the calibrated subsets below with stock clang 5.0.1.
They are not a complete specification for every original owner. Lua uses scoped
unsigned-char/O0 settings and an optional pinned experimental backend patch; see
`toolchain/lua/README.md`. The original Nintendo compiler/SDK is not distributed.
Optimised units also load the late-GlobalOpt plugin, which changes the pass pipeline rather
than a flag; see [late_globalopt.md](late_globalopt.md).

## Evidence

**zlib 1.2.3** (`Contrib/Contrib/Zlib`, 34 functions): 34/34 byte-identical. Every codegen flag
is needed. Starting from the full set:

| Change | Exact matches |
|---|---|
| full set | 34/34 |
| without `-ffunction-sections` | 33/34 |
| `-fPIE` instead of `-fPIC` | 33/34 |
| without `-fPIC` | 27/34 |
| `-fno-vectorize -fno-slp-vectorize` instead of `-mno-implicit-float` | 29/34 |
| without `-mno-implicit-float` | 22/34 |
| with `-fomit-frame-pointer` | 18/34 |
| `-O2` instead of `-O3` | 16/34 |
| without `-g` | 14/34 |
| no `-mcpu` | 7/34 |
| `-mcpu=cortex-a53` | 6/34 |

`-g` matters because debug info changes clang 5's code generation, not because the debug
sections are compared.

**PCRE 8.35** (`Contrib/Contrib/PCRE`, 30 functions): 30/30. Neighbouring releases score far
lower (8.34 and 8.36: 21/30). Three functions needed more than the zlib flags:

- `match` needs `-fsigned-char`: the original sign-extends loads from `static const char`
  tables. `char` is signed on the original target, unlike the aarch64-linux default. zlib is
  34/34 with or without it.
- `compile_branch` needs `HAVE_STDINT_H` in `config.h` (an integer overflow check instead of a
  floating-point one).
- `pcre_compile2` needs `isdigit` to be a plain range check, as in the SDK's libc, not glibc's
  table lookup. `config.h` defines it that way.

The other `config.h` options are the generic defaults with `SUPPORT_UTF` and `SUPPORT_UCP`;
`src/Contrib/Contrib/PCRE/config.h` records the evidence for each.

**Game code** (`X/Sigma/Math`, 9 hand-written functions): 9/9 match with the same flags, once
the source follows the inline helpers the DWARF records (`tools/dwarf_fn.py`).

**C++ and unwind flags**, read from the binary:

- `-fno-rtti`: 1,123 vtables and no typeinfo symbols.
- `-fno-exceptions`: one CIE with augmentation `zR`, no personality routine, no LSDA.
- `-funwind-tables`: `.eh_frame` has an entry for all 36,559 functions, including C ones.
- `-mno-implicit-float` in game code: functions under `X/` contain almost no vector-register
  instructions other than scalar register moves.

## Known exceptions

- Lua and BlizzardProtoBuf were built in another tree with an LLVM 5.0.0 snapshot, and curl and
  the SDK objects by Nintendo. Their flags are not calibrated. curl uses vector registers for
  copies, so it was not built with `-mno-implicit-float`.
- BlizzardProtoBuf's own `.cc` files get `-funsigned-char -DNDEBUG` (CMakeLists.txt).
  `hash<const char*>` in descriptor.cc zero-extends each byte, and `scoped_ptr::operator->`
  has no assert. With both flags, stubs/substitute.cc goes from 2/3 to 3/3, descriptor.cc gains
  26 functions, and the other protobuf files keep every match. The game's `.pb.cc` files are
  game code and keep the global flags.
- DemonWare's `.cpp` files get `-funsigned-char` (CMakeLists.txt). The DWARF records each
  compile unit's `char` base type: all 260 DemonWare units say `DW_ATE_unsigned_char`, as do
  BlizzardProtoBuf, Lua, curl and OggVorbis, while the game's own units and zlib say
  `DW_ATE_signed_char`. Most DemonWare code does not care, but the bdJSON parsers switch on
  `char` and only match with it (33/35 against 26/35). The flag changed nothing for the 851
  DemonWare functions that already matched. Lua's C owners also receive an appended
  `-funsigned-char` override; all 28 saved Lua CUs independently confirm unsigned plain
  `char`.
  curl will need the same flag when someone starts it.
- OggVorbis C owners now receive scoped `-funsigned-char`, `-std=gnu89`,
  `--target=aarch64-linux` and `-ffast-math`. The framing/decode/psy pilot verifies
  all 39 original functions in both build modes; removing fast math from psy
  drops its result from 14/14 to 4/14. LLVM 5.0.1 and unsigned plain char are
  recorded in the original CUs; GNU89 is the compatible upstream build dialect.
- `Contrib/Contrib/Domino` uses NEON heavily and may have its own settings.

`data/2.6.2/function_files.csv` gives the source file and compiler for each function.

## Reproducing

Core setup/build commands reproduce the configured flags without a sweep or model
provider. `tools/try_file.py` supplies provisional object comparison using core
`tools/validation/{elf,scoring}.py`; full acceptance still requires all strict gates.
