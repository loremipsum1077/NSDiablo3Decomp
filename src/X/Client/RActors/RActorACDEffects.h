// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/RActors/RActorACDEffects.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Sigma/Math/Vector.h"

typedef int32 RopeID;

typedef int32 RopePosition;

typedef int32 SNO;  // X/Common/SNO/SNOFiles.h

// X/Client/RActors/RActorACDEffects.cpp
RopeID RActorACDRopeEffect(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom,
                           const ACDID idACDTo, const RopePosition ePositionTo, const BOOL bIgnoreOwnerAlpha);
RopeID RActorACDRopeEffect(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom,
                           const Vector3D & wpDest, const BOOL bIgnoreOwnerAlpha);
void RActorACDEffectsAdd(const ACDID idACD);
void RActorACDEffectsRemove(const ACDID idACD);
