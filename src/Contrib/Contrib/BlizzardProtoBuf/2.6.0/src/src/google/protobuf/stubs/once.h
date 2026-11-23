// Subset of protobuf 2.6.0 stubs/once.h.
#pragma once

#include "atomicops.h"
#include "common.h"

namespace google {
namespace protobuf {

enum {
    ONCE_STATE_UNINITIALIZED = 0,
    ONCE_STATE_EXECUTING_CLOSURE = 1,
    ONCE_STATE_DONE = 2,
};

typedef google::protobuf::internal::AtomicWord ProtobufOnceType;

void GoogleOnceInitImpl(ProtobufOnceType* once, Closure* closure);

inline void GoogleOnceInit(ProtobufOnceType* once, void (*init_func)()) {
    if (internal::Acquire_Load(once) != ONCE_STATE_DONE) {
        internal::FunctionClosure0 func(init_func, false);
        GoogleOnceInitImpl(once, &func);
    }
}

template <typename Arg>
inline void GoogleOnceInit(ProtobufOnceType* once, void (*init_func)(Arg*),
    Arg* arg) {
    if (internal::Acquire_Load(once) != ONCE_STATE_DONE) {
        internal::FunctionClosure1<Arg*> func(init_func, false, arg);
        GoogleOnceInitImpl(once, &func);
    }
}

#define GOOGLE_PROTOBUF_ONCE_INIT ::google::protobuf::ONCE_STATE_UNINITIALIZED

class GoogleOnceDynamic {
 public:
  GoogleOnceDynamic() : state_(GOOGLE_PROTOBUF_ONCE_INIT) { }

  // If this->Init() has not been called before by any thread,
  // execute (*func_with_arg)(arg) then return.
  // Otherwise, wait until that prior invocation has finished
  // executing its function, then return.
  template<typename T>
  void Init(void (*func_with_arg)(T*), T* arg) {
    GoogleOnceInit<T>(&this->state_,
                      func_with_arg,
                      arg);
  }
 private:
  ProtobufOnceType state_;
};

#define GOOGLE_PROTOBUF_DECLARE_ONCE(NAME) \
    ::google::protobuf::ProtobufOnceType NAME = GOOGLE_PROTOBUF_ONCE_INIT

}  // namespace protobuf
}  // namespace google

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(google::protobuf::GoogleOnceDynamic) == 0x8, "google::protobuf::GoogleOnceDynamic");
