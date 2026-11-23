#include "../../include/blz/semaphore.h"

// The libc calls (Nintendo's musl); declared minimally, not in src/.
extern "C" {
int sem_init(blz::sem_t*, int, unsigned int);
int sem_destroy(blz::sem_t*);
int sem_post(blz::sem_t*);
}

namespace blz {

// line 18
semaphore::semaphore(unsigned int value) {
    sem_init(&m_sem, 0, value);
}

// line 26
semaphore::~semaphore() {
    sem_destroy(&m_sem);
}

// line 78
void semaphore::post() {
    sem_post(&m_sem);
}

}  // namespace blz
