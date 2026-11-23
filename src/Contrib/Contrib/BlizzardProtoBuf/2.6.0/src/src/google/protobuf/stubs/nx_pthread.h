// Target-libc facts this vendored copy of protobuf depends on.  Not upstream
// protobuf.
//
// main.elf was built against the NX libc's pthread shim, whose
// pthread_mutex_t is 32 bytes; we compile against the build container's
// aarch64-linux-gnu glibc, whose pthread_mutex_t is 48.  The size is directly
// observable in the binary: both `new Mutex::Internal` sites ask operator new
// for 0x20 bytes -- Mutex::Mutex at 0xb64c68 and InitLogSilencerCount at
// 0xb64800 -- and both functions match exactly once Internal is 32 bytes.
// Everything else about the mutex is unchanged: Mutex::Lock still calls
// pthread_mutex_lock (0xb64cec) and reports "pthread_mutex_lock: " on failure.

#ifndef GOOGLE_PROTOBUF_STUBS_NX_PTHREAD_H__
#define GOOGLE_PROTOBUF_STUBS_NX_PTHREAD_H__

#include <pthread.h>

// Storage with the target's size and alignment.  The pthread entry points
// only ever see the address, so giving them this object's storage is enough.
struct NXPthreadMutex {
  long long opaque[4];
  pthread_mutex_t* get() { return reinterpret_cast<pthread_mutex_t*>(opaque); }
};
static_assert(sizeof(NXPthreadMutex) == 32, "NX pthread_mutex_t size");

#endif  // GOOGLE_PROTOBUF_STUBS_NX_PTHREAD_H__
