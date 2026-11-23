// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Nav/NavCellHandle.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../../Sigma/SigmaTypes.h"

typedef int32 SSceneID;

struct NavCellHandle {  // size 0x8
    SSceneID idSScene;  // 0x00
    uint32 uCellIndex;  // 0x04
    BOOL operator==(const NavCellHandle & rhs) const {  // line 22
        return uCellIndex == rhs.uCellIndex && idSScene == rhs.idSScene;  // line 24
    }
    BOOL operator!=(const NavCellHandle & rhs) const {  // line 27
        return !(*this == rhs);
    }
};
static_assert(sizeof(NavCellHandle) == 0x8, "NavCellHandle");


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  BOOL NavCellHandle::operator==(const NavCellHandle & rhs) const  [inlined into 5 functions]
//   line    27  BOOL NavCellHandle::operator!=(const NavCellHandle & rhs) const  [inlined into 1 functions]
