// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/friends/friends_AsyncContext.h

#pragma once

#include "../os/os_Mutex.h"

namespace nn { class Result; }
namespace nn { namespace friends { namespace detail { class AsyncContextInternal; } } }
namespace nn { namespace os { class SystemEvent; } }

namespace nn {
namespace friends {
class AsyncContext {  // size 0x28
    AsyncContext(const nn::friends::AsyncContext &);
    nn::friends::AsyncContext & operator=(const nn::friends::AsyncContext &);
    AsyncContext(nn::friends::AsyncContext &&);
    nn::friends::AsyncContext & operator=(nn::friends::AsyncContext &&);
public:
    AsyncContext();
    ~AsyncContext();
    nn::Result GetSystemEvent(nn::os::SystemEvent *);
    nn::Result Cancel();
    nn::Result HasDone(bool *);
    nn::Result GetResult() const;
    void SetInternal(nn::friends::detail::AsyncContextInternal *);
private:
    nn::os::Mutex m_Mutex;  // 0x00
    nn::friends::detail::AsyncContextInternal * m_pInternal;  // 0x20
};
static_assert(sizeof(AsyncContext) == 0x28, "AsyncContext");

}  // namespace friends
}  // namespace nn
