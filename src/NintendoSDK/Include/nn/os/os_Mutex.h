// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/os/os_Mutex.h

#pragma once

#include "os_MutexTypes.h"

namespace nn { namespace os { struct MutexType; } }

namespace nn {
namespace os {
class Mutex {  // size 0x20
    Mutex(const nn::os::Mutex &);
    nn::os::Mutex & operator=(const nn::os::Mutex &);
    Mutex(nn::os::Mutex &&);
    nn::os::Mutex & operator=(nn::os::Mutex &&);
public:
    explicit Mutex(bool);
    explicit Mutex(bool, int);
    ~Mutex();
    void Lock();
    bool TryLock();
    void Unlock();
    bool IsLockedByCurrentThread() const;
    void lock();
    bool try_lock();
    void unlock();
    operator nn::os::MutexType &();
    operator const nn::os::MutexType &() const;
    nn::os::MutexType * GetBase();
private:
    nn::os::MutexType m_Mutex;  // 0x00
};
static_assert(sizeof(Mutex) == 0x20, "Mutex");

}  // namespace os
}  // namespace nn
