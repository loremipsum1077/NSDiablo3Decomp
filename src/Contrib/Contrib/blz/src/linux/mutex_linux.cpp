#include "../../include/blz/mutex.h"

// The libc calls (Nintendo's musl); declared minimally, not in src/.
namespace blz {
struct pthread_mutexattr_t {
    unsigned int __attr[2];
};
}  // namespace blz

extern "C" {
int pthread_mutex_init(blz::pthread_mutex_t*, const blz::pthread_mutexattr_t*);
int pthread_mutex_destroy(blz::pthread_mutex_t*);
int pthread_mutex_lock(blz::pthread_mutex_t*);
int pthread_mutex_unlock(blz::pthread_mutex_t*);
int pthread_mutexattr_init(blz::pthread_mutexattr_t*);
int pthread_mutexattr_settype(blz::pthread_mutexattr_t*, int);
int pthread_mutexattr_destroy(blz::pthread_mutexattr_t*);
}

// musl's PTHREAD_MUTEX_RECURSIVE
#define PTHREAD_MUTEX_RECURSIVE 1

namespace blz {

// line 21
mutex::mutex() {
    pthread_mutex_init(&m_mutex, nullptr);
}

// line 32
mutex::~mutex() {
    pthread_mutex_destroy(&m_mutex);
}

// line 40
void mutex::lock() {
    pthread_mutex_lock(&m_mutex);
}

// line 67
void mutex::unlock() {
    pthread_mutex_unlock(&m_mutex);
}

// line 92
recursive_mutex::recursive_mutex() {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

// line 109
recursive_mutex::~recursive_mutex() {
    pthread_mutex_destroy(&m_mutex);
}

// line 117
void recursive_mutex::lock() {
    pthread_mutex_lock(&m_mutex);
}

// line 148
void recursive_mutex::unlock() {
    pthread_mutex_unlock(&m_mutex);
}

}  // namespace blz
