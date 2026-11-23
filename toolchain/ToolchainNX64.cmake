# CMake toolchain file for the pinned compiler -- clang 5.0.1,
# --target=aarch64-linux-gnu. The flags are a
# starting point carried over from the sister 2.7.7 project (see CMakeLists.txt).  The linux-gnu triple is the one every calibration
# score was measured with (30/38 exact); a bare aarch64-none-elf triple
# finds no libc/libc++ headers and was never validated against the binary.
#
# The LLVM prebuilt is linux/arm64 only, so this is intended to run
# inside the project's build container (see Dockerfile / docker-compose.yml
# at the repo root), not directly on a macOS host.

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

find_program(CLANG_C_COMPILER clang REQUIRED)
find_program(CLANG_CXX_COMPILER clang++ REQUIRED)
set(CMAKE_C_COMPILER ${CLANG_C_COMPILER})
set(CMAKE_CXX_COMPILER ${CLANG_CXX_COMPILER})

set(CMAKE_C_COMPILER_TARGET aarch64-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET aarch64-linux-gnu)

# We can't link a full executable yet -- there's no libc/libc++/SDK stub
# libraries recovered for this target. CMake's compiler-detection try-compile
# step needs to build a static library instead of an executable, or it fails
# before we even get to building object files.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
