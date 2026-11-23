# Source provenance and third-party notices

## Earlier decompilation work

The maintainer recalls using the game's DWARF data with Ghidra in the predecessor
repository and reports never having had a Nintendo SDK. The maintainer believes
the animation and networking files came from that work, but does not recall how
each file was written or how the Blizzard notices were added. All four notices
are present in the initial import (`6e5e627d`). No record shows how they were
added, so they have been removed; each first line now points to this document and
the line count is unchanged. Local Ghidra exports under
`build/ghidra/decomp/X/Client/` provide function-level evidence of compiled behavior.

## Game and SDK source under review

| Files | Evidence available | Open question |
| --- | --- | --- |
| `src/X/Client/Animation2D.cpp`, `AnimationCommon.cpp` | The closing comments identify functions, original addresses, DWARF declarations, field offsets and constants used to check the implementations. The 2010 Blizzard notices were removed. | Confirm the pre-import origin of the source text. |
| `src/X/Client/Network/CGameMessage.cpp`, `CNetwork.cpp` | The 2.6.2 DWARF identifies three MessageSendToServer overloads at offsets 0x1e9f70–0x1e9fc0 and ten CNetwork functions at 0x1f6720–0x1f6c30. The local exports under `build/dwarf/headers/X/Client/Network/` record signatures and line numbers. The 2012 Blizzard notices were removed. | Locate the pre-import reconstruction records for these bodies. |
| `src/BlizzardCore/` (40 files) | `System_Lock.cpp` identifies semaphore functions from the NX64 unity build. `Package_Util.cpp` traces hash and job implementations to DWARF files and records linker-removed functions. `System_String.cpp` documents inferred control flow and a size mismatch. | Confirm the source-text origin of the remaining headers and implementation bodies. |
| `src/NintendoSDK/` | Bootstrap recovers all 498 NVN loader names from the local executable. `Include/nvn/nvn_Cpp.h` documents DWARF generation and hand maintenance. The compiler headers `bits/alltypes.h` and `nnmusl_alltypes.h` identify specific DWARF compilation units. | Establish the inputs used for the other headers and compiler compatibility files. |
| `src/Contrib/Contrib/{Domino,blz,DemonWare,fmodstudiosrc11005}/` | Individual files carry reconstruction notes. The FMOD C header describes its public entry points and DWARF-derived types. DemonWare's external crypto libraries have separate notices. | Identify the basis for distribution of each component, including any vendor-specific additions. |

## Local source comparison

After `python3 tools/gen_headers.py`, run `python3 tools/review_source_origins.py`
to list the tracked path, local reference path and text comparison for each file.
The comparison accepts the project’s adoption comment and normalizes line endings.
A reference-path match identifies debug evidence for an owner. Only a text match
reproduces the tracked declarations. Other implementations still need source records.

| Component | Tracked files reviewed | Matching reference text |
| --- | ---: | ---: |
| NintendoSDK | 68 | 20 |
| BlizzardCore | 40 | 1 |
| blz | 78 | 0 |
| Domino | 101 | 13 |
| FMOD | 4 | 0 |
| DemonWare, excluding the separately pinned external libraries | 580 | 22 |

### The 22 paths without a same-name reference

| Files | Evidence available in the checkout |
| --- | --- |
| Seven BlizzardCore unity files under `Source/System` (Hardware, Lock, Memory, OS, Thread, Time and Using) | Their source comments name the implementation files included by each original compilation unit. Use those constituent paths for line and signature comparisons. |
| DemonWare `bdSingleton.cpp`, `bdSort.h`, `bdFileInfo.h` | The first defines the registry pointer. The second documents a handwritten declaration for the sort inline file. The third contains the visibility enum used by storage declarations. |
| Domino `Include/dmApi.h` | The project chose the header name and reconstructed the entry points from mangled symbols and the corresponding body and fixture implementations. |
| FMOD `fmod.h` and Nintendo `ldn_Api.h` | Both identify a vendor API as a source. All six LDN names and empty parameter lists are independently visible with `llvm-nm --dynamic --undefined-only build/romfs/DiabloIIINX64r.nss | c++filt`. The return types and the exact vendor references still need confirmation. |
| Five Nintendo compiler headers under `Compilers/NX/nx/aarch64/include` | The two alltypes headers cite specific compilation units. The two math headers and ctype header identify particular wrappers, predicates or original lines. |
| Nintendo account API and async context, NIFM API and socket API | The files identify imports, calling code and, for the opaque async context, its observed member size. |

## Notices in the tree

| Component | Notice or source record |
| --- | --- |
| curl | [COPYING](../src/Externals/curl/COPYING), transcribed from the notice retained in `lib/warnless.c`. Individual files also carry notices from other authors. |
| Lua | [lua.h](../src/Contrib/Contrib/lua/src/lua.h) contains the copyright and permission notice. |
| zlib | The [upstream README](../src/Contrib/Contrib/Zlib/README) includes its license. |
| PCRE | Source files such as [pcre_exec.c](../src/Contrib/Contrib/PCRE/pcre_exec.c) retain the copyright and redistribution terms. |
| Ogg, Vorbis and Theora | Each component has a `COPYING` file under [OggVorbis](../src/Contrib/Contrib/OggVorbis). |
| LibTomCrypt and LibTomMath | The [source record](../src/Contrib/Contrib/DemonWare/external/README.md) identifies upstream archives, hashes and license files. |
| JPEG compressor | [jpge.cpp](../src/Contrib/Contrib/jpeg-compressor_v104_r1/jpge.cpp) identifies the author and public-domain dedication. |
| Protocol Buffers | Runtime sources such as [message.cc](../src/Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/message.cc) retain Google's redistribution terms. The tree also contains Hydra-specific changes. The two UTF-8 lookup tables match the pinned upstream archive byte for byte and are sourced from that archive. See [COPYING.txt](../src/Contrib/Contrib/BlizzardProtoBuf/2.6.0/COPYING.txt). Bootstrap generates the game bindings locally. |

The [MIT License](../LICENSES/MIT.txt) covers the project-authored tooling identified in [REUSE.toml](../REUSE.toml). The submodule and upstream patches have separate origins.
