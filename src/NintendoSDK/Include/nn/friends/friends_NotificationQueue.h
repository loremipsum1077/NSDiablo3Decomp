// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/friends/friends_NotificationQueue.h

#pragma once

#include "../account/account_Types.h"
#include "../os/os_Mutex.h"
#include "../os/os_SystemEvent.h"

namespace nn { class Result; }
namespace nn { namespace friends { struct NotificationInfo; } }
namespace nn { namespace friends { namespace detail { namespace ipc { class INotificationService; } } } }

namespace nn {
namespace friends {
class NotificationQueue {  // size 0x68
    NotificationQueue(const nn::friends::NotificationQueue &);
    nn::friends::NotificationQueue & operator=(const nn::friends::NotificationQueue &);
    NotificationQueue(nn::friends::NotificationQueue &&);
    nn::friends::NotificationQueue & operator=(nn::friends::NotificationQueue &&);
public:
    NotificationQueue();
    ~NotificationQueue();
    nn::Result Initialize(const nn::account::Uid &);
    nn::Result Initialize();
    void Finalize();
    const nn::account::Uid & GetUid() const;
    nn::os::SystemEvent * GetSystemEvent();
    nn::Result Clear();
    nn::Result Pop(nn::friends::NotificationInfo *);
private:
    nn::os::Mutex m_Mutex;  // 0x00
    nn::account::Uid m_Uid;  // 0x20
    nn::friends::detail::ipc::INotificationService * m_Handle;  // 0x30
    nn::os::SystemEvent m_Event;  // 0x38
};
static_assert(sizeof(NotificationQueue) == 0x68, "NotificationQueue");

}  // namespace friends
}  // namespace nn
