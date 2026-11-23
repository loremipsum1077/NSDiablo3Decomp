#pragma once

#include "nvn_Cpp.h"

namespace nvn {

#define NVN_ENTRY(name) name##Func pfncpp_nvn##name;
#include "../../../X/Common/generated-game-data/nvn_entry_points.inc"
#undef NVN_ENTRY

#line 530 "NintendoSDK/Include/NVN/nvn_CppFuncPtrImpl.h"
void nvnLoadCPPProcs(nvn::Device * device, nvn::DeviceGetProcAddressFunc getProc)
{
#define NVN_ENTRY(name) pfncpp_nvn##name = (name##Func)getProc(device, "nvn" #name);
#include "../../../X/Common/generated-game-data/nvn_entry_points.inc"
#undef NVN_ENTRY
}

}  // namespace nvn
