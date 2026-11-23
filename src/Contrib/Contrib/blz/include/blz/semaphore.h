#pragma once

// blz::semaphore over a POSIX sem_t (nnmusl's, 0x28 bytes; the SDK header is
// not in src/, so only the size is kept). Defined in src/linux/semaphore_linux.cpp:
// the constructor, destructor and post(); wait, try_wait, post(n) and
// native_handle exist in the DWARF but the linker dropped them, so they are
// declared only.

// Like pthread_mutex_t in mutex.h, the member type is blz::sem_t here so a
// translation unit that also includes the host's <semaphore.h> still compiles.

#include "config.h"

namespace blz {

struct sem_t {  // nnmusl semaphore.h, layout only
    unsigned long __opaque[5];
};

class semaphore_base {
protected:
    sem_t m_sem;
    typedef sem_t* native_handle_type;
};
static_assert(sizeof(semaphore_base) == 0x28, "semaphore_base");

class semaphore : public semaphore_base {
    semaphore(const semaphore&);
    semaphore& operator=(const semaphore&);

public:
    semaphore(unsigned int);
    ~semaphore();
    void wait();
    bool try_wait();
    void post();
    void post(unsigned int);
    native_handle_type native_handle();

private:
    bool _try_wait_until(uint64_t, uint32_t);
};
static_assert(sizeof(semaphore) == 0x28, "semaphore");

}  // namespace blz
