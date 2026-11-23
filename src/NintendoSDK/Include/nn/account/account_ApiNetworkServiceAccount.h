// Import prototypes written from their call sites in bdAuthSwitch.cpp and the dynamic symbol names.
// Original files: NintendoSDK/Include/nn/account/account_Api*.h, nn/err/err_Api.h

#pragma once

#include "account_AsyncContext.h"
#include "account_Types.h"
#include "../nn_Result.h"
#include <stddef.h>

namespace nn {
namespace account {
nn::Result EnsureNetworkServiceAccountAvailable(const UserHandle & handle);
nn::Result IsNetworkServiceAccountAvailable(bool * pOut, const UserHandle & handle);
nn::Result GetNetworkServiceAccountId(NetworkServiceAccountId * pOutId, const UserHandle & handle);
nn::Result EnsureNetworkServiceAccountIdTokenCacheAsync(AsyncContext * pOutContext, const UserHandle & handle);
nn::Result LoadNetworkServiceAccountIdTokenCache(size_t * pOutActualSize, char * buffer, size_t bufferSize, const UserHandle & handle);
}  // namespace account
namespace err {
void ShowError(nn::Result result);
}  // namespace err
}  // namespace nn
