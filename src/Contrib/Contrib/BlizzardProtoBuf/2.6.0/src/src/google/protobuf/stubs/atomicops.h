// Minimal subset of protobuf 2.6.0 stubs/atomicops.h (only the typedefs this build uses).
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace google {
namespace protobuf {
namespace internal {
typedef intptr_t Atomic64;

typedef intptr_t AtomicWord;

}  // namespace internal
}  // namespace protobuf
}  // namespace google

#include "atomicops_internals_arm64_gcc.h"
