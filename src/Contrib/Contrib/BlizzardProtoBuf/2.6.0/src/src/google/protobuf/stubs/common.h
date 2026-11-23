// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/stubs/common.h
// Subset: the fixed-width typedefs and limits, implicit_cast/down_cast, scoped_ptr/array,
// GOOGLE_COMPILE_ASSERT, the version check, the logging declarations (LogMessage, LogFinisher,
// GOOGLE_LOG/CHECK macros), Closure/FunctionClosure0/1, Mutex and its locks, shutdown and
// Blizzard's startup list.

#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../../../../../../../blz/include/blz/string.h"

namespace google {
namespace protobuf {

typedef unsigned int uint;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

#define GOOGLE_LONGLONG(x) x##LL
#define GOOGLE_ULONGLONG(x) x##ULL

static const int32 kint32max = 0x7FFFFFFF;
static const int32 kint32min = -kint32max - 1;
static const int64 kint64max = GOOGLE_LONGLONG(0x7FFFFFFFFFFFFFFF);
static const int64 kint64min = -kint64max - 1;
static const uint32 kuint32max = 0xFFFFFFFFu;
static const uint64 kuint64max = GOOGLE_ULONGLONG(0xFFFFFFFFFFFFFFFF);

#undef GOOGLE_DISALLOW_EVIL_CONSTRUCTORS
#define GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(TypeName)    \
  TypeName(const TypeName&);                           \
  void operator=(const TypeName&)

// Not a DLL build.
#define LIBPROTOBUF_EXPORT

namespace internal {

template<typename To, typename From>
inline To implicit_cast(From const &f) {
  return f;
}

// NDEBUG build: no RTTI check.
template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f) {                   // so we only accept pointers
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false) {
    implicit_cast<From*, To>(0);
  }

  return static_cast<To>(f);
}

}  // namespace internal

using internal::implicit_cast;
using internal::down_cast;

namespace internal {

template <bool>
struct CompileAssert {
};

}  // namespace internal

#undef GOOGLE_COMPILE_ASSERT
#define GOOGLE_COMPILE_ASSERT(expr, msg)   typedef ::google::protobuf::internal::CompileAssert<(bool(expr))>           msg[bool(expr) ? 1 : -1]

namespace internal {

template <class C>
class scoped_ptr {
 public:

  typedef C element_type;

  explicit scoped_ptr(C* p = NULL) : ptr_(p) { }

  ~scoped_ptr() {
    enum { type_must_be_complete = sizeof(C) };
    delete ptr_;
  }

  void reset(C* p = NULL) {
    if (p != ptr_) {
      enum { type_must_be_complete = sizeof(C) };
      delete ptr_;
      ptr_ = p;
    }
  }

  C& operator*() const {
    assert(ptr_ != NULL);
    return *ptr_;
  }
  C* operator->() const  {
    assert(ptr_ != NULL);
    return ptr_;
  }
  C* get() const { return ptr_; }

  bool operator==(C* p) const { return ptr_ == p; }
  bool operator!=(C* p) const { return ptr_ != p; }

  void swap(scoped_ptr& p2) {
    C* tmp = ptr_;
    ptr_ = p2.ptr_;
    p2.ptr_ = tmp;
  }

  C* release() {
    C* retVal = ptr_;
    ptr_ = NULL;
    return retVal;
  }

 private:
  C* ptr_;

  template <class C2> bool operator==(scoped_ptr<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_ptr<C2> const& p2) const;

  scoped_ptr(const scoped_ptr&);
  void operator=(const scoped_ptr&);
};

template <class C>
class scoped_array {
 public:

  typedef C element_type;

  explicit scoped_array(C* p = NULL) : array_(p) { }

  ~scoped_array() {
    enum { type_must_be_complete = sizeof(C) };
    delete[] array_;
  }

  void reset(C* p = NULL) {
    if (p != array_) {
      enum { type_must_be_complete = sizeof(C) };
      delete[] array_;
      array_ = p;
    }
  }

  C& operator[](ptrdiff_t i) const {
    assert(i >= 0);
    assert(array_ != NULL);
    return array_[i];
  }

  C* get() const {
    return array_;
  }

  bool operator==(C* p) const { return array_ == p; }
  bool operator!=(C* p) const { return array_ != p; }

  void swap(scoped_array& p2) {
    C* tmp = array_;
    array_ = p2.array_;
    p2.array_ = tmp;
  }

  C* release() {
    C* retVal = array_;
    array_ = NULL;
    return retVal;
  }

 private:
  C* array_;

  template <class C2> bool operator==(scoped_array<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_array<C2> const& p2) const;

  scoped_array(const scoped_array&);
  void operator=(const scoped_array&);
};

}  // namespace internal

using internal::scoped_ptr;
using internal::scoped_array;

#define GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN()
#define GOOGLE_SAFE_CONCURRENT_WRITES_END()

#define GOOGLE_PROTOBUF_VERSION 2006000
#define GOOGLE_PROTOBUF_MIN_LIBRARY_VERSION 2006000

namespace internal {
static const int kMinHeaderVersionForLibrary = 2006000;
void VerifyVersion(int headerVersion, int minLibraryVersion, const char* filename);
blz::string VersionString(int version);
}  // namespace internal

#define GOOGLE_PROTOBUF_VERIFY_VERSION                                    \
  ::google::protobuf::internal::VerifyVersion(                            \
    GOOGLE_PROTOBUF_VERSION, GOOGLE_PROTOBUF_MIN_LIBRARY_VERSION,         \
    __FILE__)

enum LogLevel {
    LOGLEVEL_INFO = 0,
    LOGLEVEL_WARNING = 1,
    LOGLEVEL_ERROR = 2,
    LOGLEVEL_FATAL = 3,
    LOGLEVEL_DFATAL = 2,
};

namespace internal {
class LogFinisher;

class LogMessage {  // size 0x40
public:
    LogMessage(LogLevel level, const char* filename, int line);
    ~LogMessage();
    LogMessage& operator<<(const blz::string& value);
    LogMessage& operator<<(const char* value);
    LogMessage& operator<<(char value);
    LogMessage& operator<<(int value);
    LogMessage& operator<<(uint value);
    LogMessage& operator<<(long value);
    LogMessage& operator<<(unsigned long value);
    LogMessage& operator<<(double value);
private:
    friend class LogFinisher;
    void Finish();
    LogLevel level_;
    const char* filename_;
    int line_;
    blz::string message_;
};

class LogFinisher {
public:
    void operator=(LogMessage& other);
};
}  // namespace internal

typedef void LogHandler(LogLevel level, const char* filename, int line,
                        const blz::string& message);
LogHandler* SetLogHandler(LogHandler* new_func);

class LogSilencer {
 public:
  LogSilencer();
  ~LogSilencer();
};
}  // namespace protobuf
}  // namespace google

#define GOOGLE_ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#define GOOGLE_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))

#define GOOGLE_LOG(LEVEL)                                                 \
  ::google::protobuf::internal::LogFinisher() =                           \
    ::google::protobuf::internal::LogMessage(                             \
      ::google::protobuf::LOGLEVEL_##LEVEL, __FILE__, __LINE__)
#define GOOGLE_LOG_IF(LEVEL, CONDITION) \
  !(CONDITION) ? (void)0 : GOOGLE_LOG(LEVEL)
#define GOOGLE_CHECK(EXPRESSION) \
  GOOGLE_LOG_IF(FATAL, !(EXPRESSION)) << "CHECK failed: " #EXPRESSION ": "
#define GOOGLE_CHECK_EQ(A, B) GOOGLE_CHECK((A) == (B))
#define GOOGLE_CHECK_NE(A, B) GOOGLE_CHECK((A) != (B))
#define GOOGLE_CHECK_LT(A, B) GOOGLE_CHECK((A) <  (B))
#define GOOGLE_CHECK_LE(A, B) GOOGLE_CHECK((A) <= (B))
#define GOOGLE_CHECK_GT(A, B) GOOGLE_CHECK((A) >  (B))
#define GOOGLE_CHECK_GE(A, B) GOOGLE_CHECK((A) >= (B))

namespace google {
namespace protobuf {
namespace internal {
template<typename T>
T* CheckNotNull(const char* /* file */, int /* line */,
                const char* name, T* val) {
  if (val == NULL) {
    GOOGLE_LOG(FATAL) << name;
  }
  return val;
}
}  // namespace internal
}  // namespace protobuf
}  // namespace google
#define GOOGLE_CHECK_NOTNULL(A) \
  internal::CheckNotNull(__FILE__, __LINE__, "'" #A "' must not be NULL", (A))

// NDEBUG build (as the original was): DCHECKs compile to dead code.
#define GOOGLE_DCHECK(EXPRESSION) while(false) GOOGLE_CHECK(EXPRESSION)
#define GOOGLE_DCHECK_EQ(A, B) GOOGLE_DCHECK((A) == (B))
#define GOOGLE_DCHECK_LE(A, B) GOOGLE_DCHECK((A) <= (B))
#define GOOGLE_DCHECK_LT(A, B) GOOGLE_DCHECK((A) <  (B))
#define GOOGLE_DCHECK_GE(A, B) GOOGLE_DCHECK((A) >= (B))
#define GOOGLE_DCHECK_GT(A, B) GOOGLE_DCHECK((A) >  (B))

namespace google {
namespace protobuf {

class Closure {
public:
    Closure() {}
    virtual ~Closure();

    virtual void Run() = 0;

private:
    Closure(const Closure&);
    void operator=(const Closure&);
};

namespace internal {

class FunctionClosure0 : public Closure {
public:
    typedef void (*FunctionType)();

    FunctionClosure0(FunctionType function, bool self_deleting)  // line 874
        : function_(function), self_deleting_(self_deleting) {}
    ~FunctionClosure0();

    void Run() {  // line 878
        bool needs_delete = self_deleting_;  // read in case callback deletes
        function_();
        if (needs_delete) delete this;
    }

private:
    FunctionType function_;
    bool self_deleting_;
};

template <typename Arg1>
class FunctionClosure1 : public Closure {
 public:
  typedef void (*FunctionType)(Arg1 arg1);

  FunctionClosure1(FunctionType function, bool self_deleting,
                   Arg1 arg1)
    : function_(function), self_deleting_(self_deleting),
      arg1_(arg1) {}
  ~FunctionClosure1() {}

  void Run() {
    bool needs_delete = self_deleting_;  // read in case callback deletes
    function_(arg1_);
    if (needs_delete) delete this;
  }

 private:
  FunctionType function_;
  bool self_deleting_;
  Arg1 arg1_;
};

class Mutex {  // size 0x8
 public:
  // Create a Mutex that is not held by anybody.
  Mutex();

  // Destructor
  ~Mutex();

  // Block if necessary until this Mutex is free, then acquire it exclusively.
  void Lock();

  // Release this Mutex.  Caller must hold it exclusively.
  void Unlock();

  // Crash if this Mutex is not held exclusively by this thread.
  // May fail to crash when it should; will never crash when it should not.
  void AssertHeld();

 private:
  struct Internal;
  Internal* mInternal;

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(Mutex);
};

// MutexLock(mu) acquires mu when constructed and releases it when destroyed.
class MutexLock {
 public:
  explicit MutexLock(Mutex *mu) : mu_(mu) { this->mu_->Lock(); }
  ~MutexLock() { this->mu_->Unlock(); }
 private:
  Mutex *const mu_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MutexLock);
};

typedef MutexLock ReaderMutexLock;
typedef MutexLock WriterMutexLock;

// MutexLockMaybe is like MutexLock, but is a no-op when mu is NULL.
class MutexLockMaybe {
 public:
  explicit MutexLockMaybe(Mutex *mu) :
    mu_(mu) { if (this->mu_ != NULL) { this->mu_->Lock(); } }
  ~MutexLockMaybe() { if (this->mu_ != NULL) { this->mu_->Unlock(); } }
 private:
  Mutex *const mu_;
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MutexLockMaybe);
};

}  // namespace internal

using internal::Mutex;
using internal::MutexLock;
using internal::ReaderMutexLock;
using internal::WriterMutexLock;
using internal::MutexLockMaybe;

namespace internal {

void OnShutdown(void (*func)());

// Blizzard: generated files queue their AddDesc on a list that the engine runs during its
// own protobuf startup, instead of calling it from a static initialiser.
typedef void (*OnStartupFunc)();

struct ProtobufStartupStruct {  // size 0x10
    ProtobufStartupStruct() : m_func(NULL), m_next(NULL) {}  // line 1186
    OnStartupFunc m_func;  // 0x00
    ProtobufStartupStruct* m_next;  // 0x08
};

void OnStartup(ProtobufStartupStruct* node, OnStartupFunc func);
bool IsStartupProtobufLibraryStarted();
bool IsStartupProtobufLibraryComplete();

// Set by descriptor.cc's static initialiser; the startup code calls it first.
extern OnStartupFunc g_descriptorStartup;

// line 1195
bool IsStructurallyValidUTF8(const char* buf, int len);

}  // namespace internal

void StartupProtobufLibrary();
void ShutdownProtobufLibrary();

// Blizzard: upstream has `using namespace std;` here; strings are blz's.
using blz::string;
}  // namespace protobuf
}  // namespace google

// Layout checks generated from the DWARF by tools/layout_asserts.py.
// Not checked: scoped_ptr (template); scoped_array (template); FunctionClosure1 (template)
static_assert(sizeof(google::protobuf::internal::LogMessage) == 0x40, "google::protobuf::internal::LogMessage");
static_assert(sizeof(google::protobuf::internal::LogFinisher) == 0x1, "google::protobuf::internal::LogFinisher");
static_assert(sizeof(google::protobuf::LogSilencer) == 0x1, "google::protobuf::LogSilencer");
static_assert(sizeof(google::protobuf::Closure) == 0x8, "google::protobuf::Closure");
static_assert(sizeof(google::protobuf::internal::FunctionClosure0) == 0x18, "google::protobuf::internal::FunctionClosure0");
static_assert(sizeof(google::protobuf::internal::Mutex) == 0x8, "google::protobuf::internal::Mutex");
static_assert(sizeof(google::protobuf::internal::MutexLock) == 0x8, "google::protobuf::internal::MutexLock");
static_assert(sizeof(google::protobuf::internal::MutexLockMaybe) == 0x8, "google::protobuf::internal::MutexLockMaybe");
static_assert(sizeof(google::protobuf::internal::ProtobufStartupStruct) == 0x10, "google::protobuf::internal::ProtobufStartupStruct");
static_assert(offsetof(google::protobuf::internal::ProtobufStartupStruct, m_func) == 0x0, "google::protobuf::internal::ProtobufStartupStruct");
static_assert(offsetof(google::protobuf::internal::ProtobufStartupStruct, m_next) == 0x8, "google::protobuf::internal::ProtobufStartupStruct");

// Added at the end so the lines above keep their numbers.
#define GOOGLE_DCHECK_NE(A, B) GOOGLE_DCHECK((A) != (B))

// Added at the end so the lines above keep their numbers.
// emulates google3/util/endian/endian.h (defined in common.cc)
namespace google {
namespace protobuf {
LIBPROTOBUF_EXPORT uint32 ghtonl(uint32 x);
}  // namespace protobuf
}  // namespace google
