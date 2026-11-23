# DemonWare/external: LibTomCrypt and LibTomMath

Pristine upstream source, vendored so the 259 library functions in the binary have
faithful source. It is **not byte-matching** and is not expected to be with the project's
compiler. Evidence and method: `build/try/libtom/REPORT.md`, `build/try/o0/REPORT.md`.

| Library | Release | Upstream archive | SHA-256 |
|---|---|---|---|
| LibTomCrypt | 1.10 | https://github.com/libtom/libtomcrypt/archive/refs/tags/1.10.tar.gz | `bdd7f3bffb8c3e746f985e2d808ee6cd3adc7f1787523398d4167343e409f289` |
| LibTomMath | 0.38 | https://github.com/libtom/libtommath/archive/refs/tags/0.38.tar.gz | `0e97c4f60c536f096dce7e6ac281793a574efbc6f7d1294e76438ec6a355e61d` |

Both are public domain (see each `LICENSE`). No file here is edited.

## What is known and what is not

- The original objects carry no DWARF. The release is identified from source differences
  between releases (1.09/1.10/1.11 for crypt, 0.38/0.39/0.40 for math); 1.09 and 1.10 differ
  in one function only, so 1.10 is probable rather than proven.
- The original code is unoptimised and was produced by a compiler no stock clang release
  reproduces. Only a few trivial functions are byte-identical. Every other function is
  labelled M (written, not matching).
- Only the upstream files that define a function present in the binary are vendored, plus
  two fragments those files include (`aes_tab.c`, `sha224.c`). Functions in those files that
  the binary does not contain (the original linker dropped them) are still emitted by our
  build.
- Two static `qsort_helper` functions (`der_encode_set.c`, `der_encode_setof.c`) are listed
  in functions.csv as `FUN_00c059d8` and `FUN_00c05f30`; the checker cannot find them by
  that name, so they stay U although their source is here.

## Layout

- `libtomcrypt/src/...`: upstream paths. The DWARF of DemonWare code records
  `DemonWare/external/libtomcrypt/src/headers/tomcrypt_cipher.h`, which fixes this location.
- `libtomcrypt/src/headers/`: the project's reconstructed headers used by DemonWare C++
  code. They are left as they were.
- `libtomcrypt/upstream-headers/`: the pristine upstream headers, used only to compile the
  vendored `.c` files. Kept apart so the DemonWare code that already matches is unaffected.
  Merging the two header sets is future work and needs an includer check.
- `libtommath/`: upstream flat layout. This directory name is an inference (sibling of
  libtomcrypt); nothing in the binary records it.

## Build configuration (CMakeLists.txt)

`-O0 -mllvm -fast-isel=false`; features restricted to what the binary's descriptor tables
and symbols show (AES, 3DES, MD5, SHA-1, SHA-2, Tiger, Yarrow; LibTomMath descriptor with
28-bit digits); `ARGTYPE=3`, `LTC_NO_TEST`. Allocation and sorting go through DemonWare's
hooks (`libTomCryptMalloc`, `libTomCryptRealloc`, `libTomCryptCalloc`, `libTomCryptFree`,
`libTomCryptQsort`), which the DWARF of bdMemory.cpp and bdCryptoUtils.cpp names.
Function ownership comes from `config/function_file_supplement.csv`, which
`tools/gen_function_files.py` applies to functions that have no DWARF owner.
