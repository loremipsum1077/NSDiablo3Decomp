// Subset of the NintendoSDK 5.4 local-wireless (ldn) API used by DemonWare's ad hoc router.
// All six names and parameter lists appear in the NSS dynamic imports. Return types
// follow the SDK API; the import names do not encode them.

#pragma once

#include "../nn_Result.h"
#include "ldn_Types.h"

namespace nn {
namespace ldn {
void Finalize();
State GetState();
Result CloseAccessPoint();
Result DestroyNetwork();
Result CloseStation();
Result Disconnect();
}  // namespace ldn
}  // namespace nn
