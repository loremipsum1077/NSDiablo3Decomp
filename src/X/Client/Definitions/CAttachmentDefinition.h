// X/Client/Definitions/CAttachmentDefinition.h -- provisional owning header.
//
// The 2.6.2 DWARF attributes these lookups over an appearance's hardpoint and look
// tables to X/Client/Definitions/CAttachmentDefinition.cpp
// (build/dwarf/headers/X/Client/Definitions/CAttachmentDefinition.cpp). Only the
// externally visible entry points are declared; the AppearanceDefinition* overloads
// that CAttachmentDefinition.cpp declares locally are not repeated here.

#pragma once

#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/AttachmentDefinition.h"

struct AppearanceDefinition;

// X/Client/Definitions/CAttachmentDefinition.cpp
int32 HardpointIndexFromName(const SNO snoAppearance, HardpointHash szName);   // line 19   [0x71001ab0d0]
int32 LookIndexFromName(const SNO snoAppearance, LPCSTR szLookName);            // line 95   [0x71001ab2e0]
int32 LookIndexFromHash(const SNO snoAppearance, uint32 nHash);                 // line 106  [0x71001ab390]
