// Not adopted from the DWARF: the image only has this class as a 0x20-byte member of bdAuthSwitch
// and four imported member functions. Layout is opaque; member names are not known.
// Original file: NintendoSDK/Include/nn/account/account_AsyncContext.h

#pragma once

#include "../nn_Result.h"
#include <stdint.h>

namespace nn {
namespace account {
class AsyncContext {  // size 0x20
public:
    AsyncContext();
    ~AsyncContext();
    nn::Result HasDone(bool * pOut);
    nn::Result GetResult();
private:
    uint64_t _storage[4];  // 0x00
};
static_assert(sizeof(AsyncContext) == 0x20, "AsyncContext");

}  // namespace account
}  // namespace nn
