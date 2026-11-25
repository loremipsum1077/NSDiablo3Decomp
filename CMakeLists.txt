cmake_minimum_required(VERSION 3.13)
project(d3 CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(D3_LUA_COMPILER "Use the pinned experimental LLVM backend for Lua only" OFF)
# Late-GlobalOpt plugin (D3_USE_LATEPIPE): needed by clang 5.0.1 (2.6.2) only; LLVM 6.0+ runs the step itself.
# tools/build.py sets it from tools/config.toml's clang_version.
option(D3_USE_LATEPIPE "Load the clang 5.0.1 late-GlobalOpt plugin (toolchain/latepipe)" ON)
# Weak-provider link (toolchain/weak-providers.json): the COMDAT providers observed in the
# 2.6.2 / clang 5.0.1 build. Other versions link directly with lld (first copy wins).
option(D3_WEAK_PROVIDERS "Link through toolchain/weak_provider_link.py (2.6.2 only)" ON)
# Mixed-compiler builds (2.7.7). Lua, BlizzardProtoBuf, DemonWare and OggVorbis are
# byte-identical between 2.6.2 and 2.7.7: Blizzard linked them prebuilt, so they keep the
# 2.6.2 recipe (clang 5.0.1, LatePipe, the Lua backend). tools/build.py builds them first
# with D3_LIBRARY_SET=legacy-only into build/<version>-legacy, then the main build uses
# exclude-legacy and links those objects from D3_LEGACY_OBJECT_DIR.
set(D3_LIBRARY_SET "all" CACHE STRING "all | legacy-only | exclude-legacy")
set_property(CACHE D3_LIBRARY_SET PROPERTY STRINGS all legacy-only exclude-legacy)
set(D3_LEGACY_OBJECT_DIR "" CACHE PATH "legacy-only build directory (exclude-legacy only)")
set(D3_LEGACY_REGEX "/src/Contrib/Contrib/(DemonWare|BlizzardProtoBuf|lua|OggVorbis)/")
if(NOT D3_LIBRARY_SET MATCHES "^(all|legacy-only|exclude-legacy)$")
    message(FATAL_ERROR "D3_LIBRARY_SET must be all, legacy-only or exclude-legacy")
endif()
option(D3_LUA_OWNER_RECIPE "Use the reviewed experimental recipe for lstate/lgc only" OFF)
set(D3_LUA_OWNER_PACKET "" CACHE FILEPATH "Explicit fixed owner recipe packet")
set(D3_LUA_OWNER_LLC "" CACHE FILEPATH "Explicit pinned diagnostic owner backend")
if(D3_LUA_COMPILER AND D3_LUA_OWNER_RECIPE)
    message(FATAL_ERROR "Select either D3_LUA_COMPILER or D3_LUA_OWNER_RECIPE, not both")
endif()
if(D3_LUA_OWNER_RECIPE)
    find_program(D3_LUA_OWNER_PYTHON python3 REQUIRED)
    foreach(input D3_LUA_OWNER_PACKET D3_LUA_OWNER_LLC)
        if(NOT IS_ABSOLUTE "${${input}}" OR NOT EXISTS "${${input}}" OR IS_DIRECTORY "${${input}}")
            message(FATAL_ERROR "${input} must name an existing absolute file for the experimental owner recipe")
        endif()
    endforeach()
    execute_process(
        COMMAND ${D3_LUA_OWNER_PYTHON} ${PROJECT_SOURCE_DIR}/toolchain/lua/owner_compile.py
            --dependencies ${D3_LUA_OWNER_PACKET}
        RESULT_VARIABLE D3_LUA_OWNER_STATUS OUTPUT_VARIABLE D3_LUA_OWNER_HEADER_LIST
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_VARIABLE D3_LUA_OWNER_ERROR)
    if(NOT D3_LUA_OWNER_STATUS EQUAL 0 OR D3_LUA_OWNER_HEADER_LIST STREQUAL "")
        message(FATAL_ERROR "Owner recipe packet rejected: ${D3_LUA_OWNER_ERROR}")
    endif()
    string(REPLACE "\n" ";" D3_LUA_OWNER_HEADER_LIST "${D3_LUA_OWNER_HEADER_LIST}")
    get_filename_component(D3_LUA_OWNER_FRONTEND "${CMAKE_C_COMPILER}" REALPATH)
    set(D3_LUA_OWNER_DEPENDS "${PROJECT_SOURCE_DIR}/toolchain/lua/owner_compile.py"
        "${D3_LUA_OWNER_PACKET}" "${D3_LUA_OWNER_LLC}" "${CMAKE_C_COMPILER}" "${D3_LUA_OWNER_FRONTEND}")
    foreach(header ${D3_LUA_OWNER_HEADER_LIST})
        list(APPEND D3_LUA_OWNER_DEPENDS "${PROJECT_SOURCE_DIR}/${header}")
    endforeach()
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/toolchain/lua/owner_compile.py" "${D3_LUA_OWNER_PACKET}")
endif()
if(D3_LUA_COMPILER)
    find_program(D3_LUA_PYTHON python3 REQUIRED)
    execute_process(COMMAND ${D3_LUA_PYTHON} ${PROJECT_SOURCE_DIR}/toolchain/backend.py
        RESULT_VARIABLE D3_LUA_STATUS OUTPUT_VARIABLE D3_LUA_TOOLCHAIN
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_VARIABLE D3_LUA_ERROR)
    if(NOT D3_LUA_STATUS EQUAL 0)
        message(FATAL_ERROR "Lua backend not ready: ${D3_LUA_ERROR}. Run python3 tools/backends.py --check")
    endif()
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Codegen flags proven by a compiler flag sweep (docs/compile_flags.md): with
# these, upstream zlib 1.2.3 compiles to 34/34 byte-identical functions, and
# dropping or changing any one of them loses matches. -g is on the list
# because it changes clang 5's codegen, not for the debug info.
add_compile_options(-O3 -mcpu=cortex-a57 -fno-strict-aliasing -mno-implicit-float
                    -ffunction-sections -g)
# char is signed on the original target (the aarch64-linux default is
# unsigned). PCRE's match() only matches with this.
add_compile_options(-fsigned-char)
# From the binary itself: no typeinfo symbols, no personality routine or
# LSDA, and an .eh_frame entry for every function including the C ones.
add_compile_options(-funwind-tables)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -stdlib=libc++")
add_compile_options(-Wall -Wextra)
# Proven by the same sweep; the link below is `-shared` and needs it too.
add_compile_options(-fPIC)
# The build image's glibc headers define atoi, atof, bsearch, tolower, isalnum
# and friends as inline functions when optimising. The original was built
# against the SDK's libc, which has none of those, so it calls them.
add_compile_options(-D__NO_INLINE__)

# One compiler for everything: the DWARF producer strings in the .nss say
# every compile unit was built with Nintendo's clang fork based on LLVM 5.0.x
# (5.0.1 for the game and most middleware, a 5.0.0 snapshot for Lua and
# BlizzardProtoBuf). data/2.6.2/function_files.csv has it per function.
# The game's protoc output (X/Common/Protobuf/*.pb.cc) is game code too.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/config/local_game_data.json")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/tools/game_data_manifest.py"
    RESULT_VARIABLE game_data_status ERROR_VARIABLE game_data_error)
if(NOT game_data_status EQUAL 0)
    message(FATAL_ERROR "${game_data_error}")
endif()
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/X/*.cpp ${PROJECT_SOURCE_DIR}/src/X/*.cc)
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*.c)
set(MIDDLEWARE_SOURCES ${ALL_SOURCES})
list(FILTER MIDDLEWARE_SOURCES EXCLUDE REGEX "/src/X/")
if(D3_LIBRARY_SET STREQUAL "legacy-only")
    list(FILTER MIDDLEWARE_SOURCES INCLUDE REGEX "${D3_LEGACY_REGEX}")
    set(GAME_SOURCES "")
elseif(D3_LIBRARY_SET STREQUAL "exclude-legacy")
    list(FILTER MIDDLEWARE_SOURCES EXCLUDE REGEX "${D3_LEGACY_REGEX}")
    if(NOT IS_DIRECTORY "${D3_LEGACY_OBJECT_DIR}")
        message(FATAL_ERROR "D3_LIBRARY_SET=exclude-legacy needs D3_LEGACY_OBJECT_DIR (a legacy-only build)")
    endif()
    file(GLOB_RECURSE D3_LEGACY_OBJECTS
        "${D3_LEGACY_OBJECT_DIR}/CMakeFiles/d3_middleware_obj.dir/src/Contrib/Contrib/*.obj")
    list(FILTER D3_LEGACY_OBJECTS INCLUDE REGEX "${D3_LEGACY_REGEX}")
    if(NOT D3_LEGACY_OBJECTS)
        message(FATAL_ERROR "no legacy objects under ${D3_LEGACY_OBJECT_DIR}: build it first")
    endif()
    list(LENGTH D3_LEGACY_OBJECTS D3_LEGACY_COUNT)
    message(STATUS "Linking ${D3_LEGACY_COUNT} legacy (clang 5.0.1) objects from ${D3_LEGACY_OBJECT_DIR}")
endif()
# Upstream LibTomCrypt fragments that aes.c / sha256.c #include; not compile units.
list(FILTER MIDDLEWARE_SOURCES EXCLUDE REGEX "/external/libtomcrypt/src/(ciphers/aes/aes_tab|hashes/sha2/sha224)\\.c$")

# Guarantees a .rela.dyn section for tools/check (see the file).
add_library(d3_anchor_obj OBJECT ${PROJECT_SOURCE_DIR}/toolchain/link_anchor.cpp)
set(ALL_OBJECTS $<TARGET_OBJECTS:d3_anchor_obj>)
if(MIDDLEWARE_SOURCES)
    add_library(d3_middleware_obj OBJECT ${MIDDLEWARE_SOURCES})
    target_include_directories(d3_middleware_obj PRIVATE include src)
    # PCRE is configured through its config.h (src/Contrib/Contrib/PCRE/config.h).
    file(GLOB PCRE_SOURCES ${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/PCRE/*.c)
    set_source_files_properties(${PCRE_SOURCES} PROPERTIES
        COMPILE_DEFINITIONS HAVE_CONFIG_H
        INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/PCRE)
    # BlizzardProtoBuf was built in its own tree: char is unsigned there (hash<const char*>
    # zero-extends each byte) and asserts are compiled out (scoped_ptr::operator-> has none).
    # The later -funsigned-char overrides the global -fsigned-char.
    file(GLOB_RECURSE PROTOBUF_SOURCES ${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/BlizzardProtoBuf/*.cc)
    set_source_files_properties(${PROTOBUF_SOURCES} PROPERTIES
        COMPILE_OPTIONS -funsigned-char
        COMPILE_DEFINITIONS NDEBUG)
    # DemonWare was built in its own tree too: every one of its 260 compile units records
    # `char` as DW_ATE_unsigned_char in the DWARF (the game's own units say signed_char).
    # Most of its code does not care, but char switches and compare chains load with ldrb.
    file(GLOB_RECURSE DEMONWARE_SOURCES ${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/DemonWare/*.cpp)
    set_source_files_properties(${DEMONWARE_SOURCES} PROPERTIES
        COMPILE_OPTIONS -funsigned-char)
    # curl: all of its compile units record `char` as DW_ATE_unsigned_char
    # (curl_strequal loads with ldrb, not ldrsb).
    file(GLOB_RECURSE CURL_SOURCES ${PROJECT_SOURCE_DIR}/src/Externals/curl/*.c)
    set_source_files_properties(${CURL_SOURCES} PROPERTIES
        COMPILE_OPTIONS -funsigned-char)
    # Vendored codec C owners retain their original unsigned-char ABI. Keep
    # GNU89, fast math and the non-GNU target local to the codec tree; see the
    # original-CU and comparison evidence indexed by docs/compile_flags.md.
    set(OGGVORBIS_ROOT ${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/OggVorbis)
    file(GLOB_RECURSE OGGVORBIS_SOURCES CONFIGURE_DEPENDS ${OGGVORBIS_ROOT}/*.c)
    set_source_files_properties(${OGGVORBIS_SOURCES} PROPERTIES
        COMPILE_OPTIONS "--target=aarch64-linux;-std=gnu89;-funsigned-char;-ffast-math"
        INCLUDE_DIRECTORIES "${OGGVORBIS_ROOT}/libogg/include;${OGGVORBIS_ROOT}/libvorbis/include;${OGGVORBIS_ROOT}/libvorbis/lib;${OGGVORBIS_ROOT}/libtheora/include")
    # LibTomCrypt 1.10 / LibTomMath 0.38, pristine upstream source (see
    # src/Contrib/Contrib/DemonWare/external/README.md). The original objects have no DWARF
    # and were built unoptimised by a compiler stock clang does not reproduce, so these are
    # faithful source that does not byte-match. Feature macros follow the descriptor tables
    # and symbols present in the binary; the allocator hooks are DemonWare's (bdMemory.cpp).
    set(LIBTOM_ROOT ${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/DemonWare/external)
    file(GLOB_RECURSE LIBTOM_SOURCES CONFIGURE_DEPENDS ${LIBTOM_ROOT}/libtomcrypt/src/*.c ${LIBTOM_ROOT}/libtommath/*.c)
    list(FILTER LIBTOM_SOURCES EXCLUDE REGEX "/(aes_tab|sha224)\\.c$")
    set_source_files_properties(${LIBTOM_SOURCES} PROPERTIES
        COMPILE_OPTIONS "-O0;-mllvm;-fast-isel=false"
        COMPILE_DEFINITIONS "NDEBUG;LTC_SOURCE;ARGTYPE=3;LTC_NO_TEST;LTM_DESC;LTC_NO_CIPHERS;DES;RIJNDAEL;LTC_NO_HASHES;SHA512;SHA384;SHA256;SHA224;SHA1;MD5;TIGER;LTC_NO_PRNGS;YARROW;FORTUNA;YARROW_AES=3;FORTUNA_POOLS=32;FORTUNA_WD=10;XMALLOC=libTomCryptMalloc;XREALLOC=libTomCryptRealloc;XCALLOC=libTomCryptCalloc;XFREE=libTomCryptFree;XQSORT=libTomCryptQsort"
        INCLUDE_DIRECTORIES "${LIBTOM_ROOT}/libtomcrypt/upstream-headers;${LIBTOM_ROOT}/libtommath")
    # ltm.c's original DWARF/assembly records unoptimized code (SRC-4be2d8042a96).
    set_source_files_properties(src/Contrib/Contrib/lua/src/ltm.c PROPERTIES COMPILE_OPTIONS "-O0;-mllvm;-fast-isel=false")
    # lzio.c original assembly is unoptimized (SRC-b0d1517a03a2).
    set_source_files_properties(src/Contrib/Contrib/lua/src/lzio.c PROPERTIES COMPILE_OPTIONS "-O0;-mllvm;-fast-isel=false")
    # lmem.c also records unoptimized code (SRC-6fa67182d24a).
    set_source_files_properties(src/Contrib/Contrib/lua/src/lmem.c PROPERTIES COMPILE_OPTIONS "-O0;-mllvm;-fast-isel=false")
    # Recovered Lua runtime owners also use unoptimized original code. See
    # docs/lua_runtime_recovery.md for remaining Nintendo compiler differences.
    set_source_files_properties(
        src/Contrib/Contrib/lua/src/lapi.c
        src/Contrib/Contrib/lua/src/ltable.c
        src/Contrib/Contrib/lua/src/lstring.c
        src/Contrib/Contrib/lua/src/lfunc.c
        src/Contrib/Contrib/lua/src/lobject.c
        PROPERTIES COMPILE_OPTIONS "-O0;-mllvm;-fast-isel=false")
    if(D3_LUA_OWNER_RECIPE)
        set(D3_LUA_OWNER_SOURCES)
        foreach(owner lstate lgc)
            set(owner_source "${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/lua/src/${owner}.c")
            if(EXISTS "${owner_source}")
                list(APPEND D3_LUA_OWNER_SOURCES "${owner_source}")
                set_property(SOURCE "${owner_source}" APPEND PROPERTY COMPILE_OPTIONS -O0 -mllvm -fast-isel=false)
                set_property(SOURCE "${owner_source}" APPEND PROPERTY OBJECT_DEPENDS "${D3_LUA_OWNER_DEPENDS}")
            endif()
        endforeach()
    endif()
    # linit.c records unoptimized code (SRC-805bf196050a).
    set_source_files_properties(src/Contrib/Contrib/lua/src/linit.c PROPERTIES COMPILE_OPTIONS "-O0;-mllvm;-fast-isel=false")
    # Owners recovered from Lua 5.1.1 text (including documented local deviations); their original code is
    # unoptimized like the owners above.
    set_source_files_properties(
        src/Contrib/Contrib/lua/src/ldump.c
        src/Contrib/Contrib/lua/src/lundump.c
        src/Contrib/Contrib/lua/src/lmathlib.c
        src/Contrib/Contrib/lua/src/loslib.c
        src/Contrib/Contrib/lua/src/ldblib.c
        src/Contrib/Contrib/lua/src/ltablib.c
        src/Contrib/Contrib/lua/src/lauxlib.c
        src/Contrib/Contrib/lua/src/lcode.c
        src/Contrib/Contrib/lua/src/lopcodes.c
        src/Contrib/Contrib/lua/src/ldo.c
        src/Contrib/Contrib/lua/src/lstrlib.c
        PROPERTIES COMPILE_OPTIONS "-O0;-mllvm;-fast-isel=false")
    # All 28 original Lua CUs encode plain char as unsigned. Append after the
    # owner options above so neither their selector settings nor this override
    # are replaced. See docs/compiler_research/focused/unsigned_char.md.
    file(GLOB LUA_SOURCES ${PROJECT_SOURCE_DIR}/src/Contrib/Contrib/lua/src/*.c)
    set_property(SOURCE ${LUA_SOURCES} APPEND PROPERTY COMPILE_OPTIONS -funsigned-char)
    # Every Lua unit was built unoptimised. The per-file lists above predate this
    # rule; newly written Lua files get it here.
    foreach(lua_source ${LUA_SOURCES})
        get_source_file_property(lua_options ${lua_source} COMPILE_OPTIONS)
        if(NOT "${lua_options}" MATCHES "-O0")
            set_property(SOURCE ${lua_source} APPEND PROPERTY COMPILE_OPTIONS -O0 -mllvm -fast-isel=false)
        endif()
    endforeach()
    if(D3_LUA_COMPILER)
        # Preserve object/map owner paths. The launcher passes non-Lua commands
        # through unchanged and replaces only the backend for these Lua CUs.
        set_property(TARGET d3_middleware_obj PROPERTY C_COMPILER_LAUNCHER
            "${CMAKE_COMMAND};-E;env;D3_LUA_TOOLCHAIN=${D3_LUA_TOOLCHAIN};${D3_LUA_PYTHON};${PROJECT_SOURCE_DIR}/toolchain/lua/compile.py")
        set_property(SOURCE ${LUA_SOURCES} APPEND PROPERTY OBJECT_DEPENDS
            "${PROJECT_SOURCE_DIR}/toolchain/backend.py;${PROJECT_SOURCE_DIR}/toolchain/lua/compile.py;${PROJECT_SOURCE_DIR}/toolchain/lua/bootstrap.py;${PROJECT_SOURCE_DIR}/toolchain/lua/Dockerfile;${PROJECT_SOURCE_DIR}/toolchain/lua/config.json;${PROJECT_SOURCE_DIR}/toolchain/llvm.lock.json;${PROJECT_SOURCE_DIR}/toolchain/llvm_archive.py;${PROJECT_SOURCE_DIR}/toolchain/lua/llvm-5.0.1.patch;${D3_LUA_TOOLCHAIN}/receipt.json;${D3_LUA_TOOLCHAIN}/out/bin/llc")
    elseif(D3_LUA_OWNER_RECIPE)
        set_property(TARGET d3_middleware_obj PROPERTY C_COMPILER_LAUNCHER
            "${CMAKE_COMMAND};-E;env;D3_LUA_OWNER_PACKET=${D3_LUA_OWNER_PACKET};D3_LUA_OWNER_LLC=${D3_LUA_OWNER_LLC};${D3_LUA_OWNER_PYTHON};${PROJECT_SOURCE_DIR}/toolchain/lua/owner_compile.py")
    endif()
    list(APPEND ALL_OBJECTS $<TARGET_OBJECTS:d3_middleware_obj>)
endif()
if(GAME_SOURCES)
    add_library(d3_game_obj OBJECT ${GAME_SOURCES})
    target_include_directories(d3_game_obj PRIVATE include src)
    # Matching-hack convention shared with the other nx-decomp projects:
    # wrap per-function source tricks in #ifdef MATCHING_HACK_NX_CLANG.
    target_compile_definitions(d3_game_obj PRIVATE NON_MATCHING MATCHING_HACK_NX_CLANG NDEBUG)
    list(APPEND ALL_OBJECTS $<TARGET_OBJECTS:d3_game_obj>)
endif()
if(D3_LEGACY_OBJECTS)
    list(APPEND ALL_OBJECTS ${D3_LEGACY_OBJECTS})
endif()

# Late GlobalOpt (docs/late_globalopt.md). The original compiler behaves as if GlobalOpt and
# GlobalDCE also ran after the inliner, where LLVM 6.0 runs them; stock clang 5.0.1 runs them
# only before. The LatePipe plugin (toolchain/latepipe/) adds that step, at LLVM 6.0's position,
# to every C and C++ unit except the exclusions below. Its binary is pinned by SHA-256 and the
# flag names the pinned file, so a different plugin is a different command line.
# clang 7.1.0 (2.7.7) already runs the step, so D3_USE_LATEPIPE is OFF there.
if(D3_USE_LATEPIPE)
find_program(D3_LATEPIPE_PYTHON python3 REQUIRED)
execute_process(COMMAND ${D3_LATEPIPE_PYTHON} ${PROJECT_SOURCE_DIR}/toolchain/latepipe/latepipe.py --path
    RESULT_VARIABLE D3_LATEPIPE_STATUS OUTPUT_VARIABLE D3_LATEPIPE
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_VARIABLE D3_LATEPIPE_ERROR)
if(NOT D3_LATEPIPE_STATUS EQUAL 0)
    message(FATAL_ERROR "${D3_LATEPIPE_ERROR} (tools/build.py builds the plugin)")
endif()
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/toolchain/latepipe/latepipe.lock.json"
    "${PROJECT_SOURCE_DIR}/toolchain/latepipe/LatePipe.cpp"
    "${PROJECT_SOURCE_DIR}/toolchain/latepipe/latepipe.py")
set(D3_LATEPIPE_SOURCES ${ALL_SOURCES})
# Excluded: the Lua interpreter. Every Lua unit is built -O0 with its own flags (and, in Lua
# mode, its own backend); the measurement did not cover it. Kept exactly as configured above.
list(FILTER D3_LATEPIPE_SOURCES EXCLUDE REGEX "/src/Contrib/Contrib/lua/")
# Excluded: LibTomCrypt / LibTomMath (built -O0 with their own flags; not measured), and the
# two fragments that are not compile units.
list(FILTER D3_LATEPIPE_SOURCES EXCLUDE REGEX "/src/Contrib/Contrib/DemonWare/external/")
list(FILTER D3_LATEPIPE_SOURCES EXCLUDE REGEX "/external/libtomcrypt/src/(ciphers/aes/aes_tab|hashes/sha2/sha224)\\.c$")
# Sources kept on the stock pipeline: functions in them match today and stop matching with
# the late step because our source lacks code the original has. Fixing the source and removing
# the entry is the follow-up work (docs/late_globalopt.md, "Stock-pipeline list").
set(D3_STOCK_PIPELINE_SOURCES
    # (empty: RefString.cpp, SigmaTiming.cpp and ControllerDebugView.cpp were here until their
    # linker-removed functions were written from the DWARF)
)
foreach(stock_source ${D3_STOCK_PIPELINE_SOURCES})
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${stock_source}")
        message(FATAL_ERROR "D3_STOCK_PIPELINE_SOURCES names a missing file: ${stock_source}")
    endif()
    list(REMOVE_ITEM D3_LATEPIPE_SOURCES "${PROJECT_SOURCE_DIR}/${stock_source}")
endforeach()
# Appended last so the per-source COMPILE_OPTIONS set above (which replace) cannot drop it.
set_property(SOURCE ${D3_LATEPIPE_SOURCES} APPEND PROPERTY COMPILE_OPTIONS "-fplugin=${D3_LATEPIPE}")
endif()

# Link as a shared ELF (no entry point needed, undefined imports allowed) so
# tools/check gets a real ELF with a symbol table, at build/<version>/d3.
# -Bsymbolic binds calls to our own definitions (main.elf is statically linked;
# check compares call targets and must not see PLT stubs). -Map is the link
# map tools/check can use to resolve name@file entries.
if(D3_LIBRARY_SET STREQUAL "legacy-only")
    message(STATUS "legacy-only: building objects for the main build to link; no d3 link here")
elseif(ALL_OBJECTS)
    find_program(LLD_PATH ld.lld REQUIRED)
    find_program(D3_LINK_PYTHON python3 REQUIRED)
    if(D3_WEAK_PROVIDERS)
        set(D3_LINK_PREFIX ${D3_LINK_PYTHON} ${PROJECT_SOURCE_DIR}/toolchain/weak_provider_link.py
            --manifest ${PROJECT_SOURCE_DIR}/toolchain/weak-providers.json
            --build-root ${CMAKE_BINARY_DIR} --evidence ${CMAKE_BINARY_DIR}/d3.order.json --)
        set(D3_LINK_EVIDENCE ${CMAKE_COMMAND} -E true)
    else()
        set(D3_LINK_PREFIX "")
        # No provider plan: record an empty one so the output set is the same.
        set(D3_LINK_EVIDENCE ${D3_LINK_PYTHON} -c
            "open(__import__('sys').argv[1], 'w').write('{}')" ${CMAKE_BINARY_DIR}/d3.order.json)
    endif()
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/d3 ${CMAKE_BINARY_DIR}/d3.map ${CMAKE_BINARY_DIR}/d3.map.provenance ${CMAKE_BINARY_DIR}/d3.order.json
        COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/d3.map.provenance ${CMAKE_BINARY_DIR}/d3.order.json
        COMMAND ${D3_LINK_PREFIX}
                ${LLD_PATH} -shared -Bsymbolic --no-demangle -Map=${CMAKE_BINARY_DIR}/d3.map ${ALL_OBJECTS} -o ${CMAKE_BINARY_DIR}/d3
        COMMAND ${D3_LINK_EVIDENCE}
        COMMAND ${CMAKE_COMMAND} -DELF=${CMAKE_BINARY_DIR}/d3 -P ${PROJECT_SOURCE_DIR}/toolchain/RecordLinkProvenance.cmake
        DEPENDS ${ALL_OBJECTS} ${PROJECT_SOURCE_DIR}/toolchain/RecordLinkProvenance.cmake
                ${PROJECT_SOURCE_DIR}/toolchain/weak_provider_link.py ${PROJECT_SOURCE_DIR}/toolchain/weak-providers.json
        COMMAND_EXPAND_LISTS
        VERBATIM
    )
    add_custom_target(d3_elf ALL DEPENDS ${CMAKE_BINARY_DIR}/d3 ${CMAKE_BINARY_DIR}/d3.map ${CMAKE_BINARY_DIR}/d3.map.provenance ${CMAKE_BINARY_DIR}/d3.order.json)
else()
    message(STATUS "src/ has no source files yet -- nothing to build")
endif()
