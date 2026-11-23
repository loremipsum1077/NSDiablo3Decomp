// Adopted from the DWARF reference; hand-maintained.
// Original file: X/Client/Console/Utilities.h
// Subset: the upsell entry point (defined in UtilitiesNX64.cpp:166).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"

namespace Console {
namespace Utilities {
typedef OnlineService::Callback<void ()> OnUpsellCallback;

void ShowNX64Upsell(int32 nControllerIndex, OnUpsellCallback successCB, OnUpsellCallback cancelCB);
// UtilitiesNX64.cpp:100 and :217. idTarget/idPlayer are OnlineUserId (uint64) in the original.
void ShowProfile(int32 nUserIndex, uint64 idTarget, const char * pUserNickname, int32 nSizeOfNickname);
BOOL SendGameInvite(int32 nControllerIndex, uint64 idPlayer);

}  // namespace Utilities
}  // namespace Console
