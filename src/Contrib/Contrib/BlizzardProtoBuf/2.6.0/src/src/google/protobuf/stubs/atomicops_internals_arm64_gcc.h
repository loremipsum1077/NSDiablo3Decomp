// Subset of protobuf 2.6.0 stubs/atomicops_internals_arm64_gcc.h: only the Atomic64 helpers
// that the DWARF shows being used (line numbers in comments are the originals').
#pragma once

namespace google {
namespace protobuf {
namespace internal {

inline void MemoryBarrier() {  // line 40
  __asm__ __volatile__ ("dmb ish" ::: "memory");
}

inline Atomic64 NoBarrier_CompareAndSwap(volatile Atomic64* ptr,  // line 188
                                         Atomic64 old_value,
                                         Atomic64 new_value) {
  Atomic64 prev;
  int32_t temp;

  __asm__ __volatile__ (
    "0:                                    \n\t"
    "ldxr %[prev], %[ptr]                  \n\t"
    "cmp %[prev], %[old_value]             \n\t"
    "bne 1f                                \n\t"
    "stxr %w[temp], %[new_value], %[ptr]   \n\t"
    "cbnz %w[temp], 0b                     \n\t"
    "1:                                    \n\t"
    : [prev]"=&r" (prev),
      [temp]"=&r" (temp),
      [ptr]"+Q" (*ptr)
    : [old_value]"IJr" (old_value),
      [new_value]"r" (new_value)
    : "cc", "memory"
  );

  return prev;
}

inline Atomic64 Acquire_CompareAndSwap(volatile Atomic64* ptr,  // line 263
                                       Atomic64 old_value,
                                       Atomic64 new_value) {
  Atomic64 prev = NoBarrier_CompareAndSwap(ptr, old_value, new_value);
  MemoryBarrier();

  return prev;
}

inline void Release_Store(volatile Atomic64* ptr, Atomic64 value) {  // line 290
  __atomic_store_n(ptr, value, __ATOMIC_RELEASE);  // stlr
}

inline Atomic64 Acquire_Load(volatile const Atomic64* ptr) {  // line 303
  Atomic64 value;

  __asm__ __volatile__ (  // NOLINT
    "ldar %x[value], %[ptr]  \n\t"
    : [value]"=r" (value)
    : [ptr]"Q" (*ptr)
    : "memory"
  );  // NOLINT

  return value;
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
