#pragma once

// blz::mutex and friends: thin wrappers over a pthread_mutex_t (the Nintendo
// SDK's musl, nnmusl_alltypes.h, which is not in src/; only its size is kept
// here: 0x20 bytes, 8-aligned). The four _base classes hold the pthread
// object at 0x00; the user-visible classes add nothing. The member functions
// of mutex and recursive_mutex are defined in src/linux/mutex_linux.cpp; the
// timed variants, try_lock and native_handle exist in the DWARF but not in the
// binary (the linker dropped them), so they are declared only.
//
// The original member type is the global pthread_mutex_t; here it is
// blz::pthread_mutex_t so that a translation unit that also pulls in the
// host's own <pthread.h> (whose pthread_mutex_t is a different size) still
// compiles. Nothing mangled mentions it.

#include "config.h"

namespace blz {

struct pthread_mutex_t {  // nnmusl_alltypes.h, layout only
    unsigned long __opaque[4];
};

class mutex_base {
protected:
    pthread_mutex_t m_mutex;
    typedef pthread_mutex_t* native_handle_type;
};
static_assert(sizeof(mutex_base) == 0x20, "mutex_base");

class recursive_mutex_base {
protected:
    pthread_mutex_t m_mutex;
    typedef pthread_mutex_t* native_handle_type;
};
static_assert(sizeof(recursive_mutex_base) == 0x20, "recursive_mutex_base");

class timed_mutex_base {
protected:
    pthread_mutex_t m_mutex;
    typedef pthread_mutex_t* native_handle_type;
};
static_assert(sizeof(timed_mutex_base) == 0x20, "timed_mutex_base");

class recursive_timed_mutex_base {
protected:
    pthread_mutex_t m_mutex;
    typedef pthread_mutex_t* native_handle_type;
};
static_assert(sizeof(recursive_timed_mutex_base) == 0x20, "recursive_timed_mutex_base");

class mutex : public mutex_base {
    mutex(const mutex&);
    mutex& operator=(const mutex&);

public:
    mutex();
    ~mutex();
    void lock();
    bool try_lock();
    void unlock();
    native_handle_type native_handle();
};
static_assert(sizeof(mutex) == 0x20, "mutex");

class recursive_mutex : public recursive_mutex_base {
    recursive_mutex(const recursive_mutex&);
    recursive_mutex& operator=(const recursive_mutex&);

public:
    recursive_mutex();
    ~recursive_mutex();
    void lock();
    bool try_lock();
    void unlock();
    native_handle_type native_handle();
};
static_assert(sizeof(recursive_mutex) == 0x20, "recursive_mutex");

class timed_mutex : public timed_mutex_base {
    timed_mutex(const timed_mutex&);
    timed_mutex& operator=(const timed_mutex&);

public:
    timed_mutex();
    ~timed_mutex();
    void lock();
    bool try_lock();
    void unlock();
    native_handle_type native_handle();

private:
    bool _try_lock_until(uint64_t, unsigned int);
};
static_assert(sizeof(timed_mutex) == 0x20, "timed_mutex");

class recursive_timed_mutex : public recursive_timed_mutex_base {
    recursive_timed_mutex(const recursive_timed_mutex&);
    recursive_timed_mutex& operator=(const recursive_timed_mutex&);

public:
    recursive_timed_mutex();
    ~recursive_timed_mutex();
    void lock();
    bool try_lock();
    void unlock();
    native_handle_type native_handle();

private:
    bool _try_lock_until(uint64_t, unsigned int);
};
static_assert(sizeof(recursive_timed_mutex) == 0x20, "recursive_timed_mutex");

struct adopt_lock_t {};

// line 348
template <typename Mutex>
class lock_guard {
    lock_guard(const lock_guard&);
    lock_guard& operator=(const lock_guard&);

public:
    typedef Mutex mutex_type;

    // line 357
    explicit lock_guard(mutex_type& m) : m_mutex(m) {
        m_mutex.lock();
    }

    lock_guard(mutex_type& m, adopt_lock_t) : m_mutex(m) {}

    // line 369
    ~lock_guard() {
        m_mutex.unlock();
    }

private:
    mutex_type& m_mutex;
};

}  // namespace blz
