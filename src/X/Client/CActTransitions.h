// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/CActTransitions.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

struct CActTransitionGlobals {  // size 0x8
    uint32 dwActTransitionTime;  // 0x00
    BOOL bRecreateGameWithParty;  // 0x04
};
static_assert(sizeof(CActTransitionGlobals) == 0x8, "CActTransitionGlobals");

// Defined in X/Client/CActTransitions.cpp, in source order.
void CActTransitionsInitializeForGame();
void CActTransitionsDisposeForGame();
void CActTransitionStart(int32 nPlayerIndex, uint32 dwActTransitionTime, BOOL bRecreateGameWithParty);
void CActTransitionEnded();
BOOL CActTransitionInProgress();

