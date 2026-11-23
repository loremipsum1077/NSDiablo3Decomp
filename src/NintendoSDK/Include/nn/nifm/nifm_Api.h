// The nn::nifm entry points main.elf imports (llvm-nm --dynamic --undefined-only).
// Declarations only -- the SDK is not vendored and its functions carry no DWARF.

#pragma once

#include "../nn_Result.h"
#include "../socket/netinet/in.h"

namespace nn {
namespace nifm {

void Initialize();
Result GetCurrentPrimaryIpAddress(in_addr* pOutInAddr);

}  // namespace nifm
}  // namespace nn

// Used by X/Client/Console/Online.cpp.
namespace nn {
namespace nifm {

bool IsNetworkRequestOnHold();
void CancelNetworkRequest();
void SubmitNetworkRequestAndWait();
void SubmitNetworkRequest();
bool IsNetworkAvailable();

}  // namespace nifm
}  // namespace nn
