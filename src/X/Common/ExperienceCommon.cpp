// Reconstructed from the original ExperienceCommon.cpp DWARF and AArch64 code.
#include "ActorCommonData.h"
#include "Attributes/FastAttribConstants.h"
#include "Attributes/FastAttributeStructs.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/GlobalDefinition.h"
#include "Game.h"
#include "GlobalSNO.h"
#include "Player.h"
#include "WorldPlace.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"

// Original declaration: ActorCommonData.cpp, _Z15ACDGetLevelAreaRK15ActorCommonData.
SNO ACDGetLevelArea(const ActorCommonData & tACD);

uint64 ExperienceGetCurrentLevelXP(const ActorCommonData & tACD, const BOOL bUseAlt)
{
    int32 nIndex;
    uint64 nExpNext;
    if (bUseAlt) {
        nIndex = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL));
        if (nIndex < HERO_ALT_LEVEL_MIN || nIndex >= HERO_ALT_LEVEL_MAX)
            return 0;
        nExpNext = tACD.GetAltExperienceNext();
    } else {
        const int32 nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
        if (nLevel < HERO_LEVEL_MIN || nLevel >= HERO_LEVEL_MAX)
            return 0;
        nIndex = nLevel - HERO_LEVEL_MIN;
        nExpNext = tACD.GetExperienceNext();
    }

    uint64 nXPToLevel;
    if (bUseAlt) {
        const SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT);
        ScopedGameBalanceDefinitionLockR ptExpTableDef(snoXPTable);
        const ExperienceTableAlt & tExpTableAlt = ptExpTableDef->tExperienceTableAlt;
        nXPToLevel = tExpTableAlt.ptExperienceLevelsAlt[nIndex].nDeltaXP;
    } else {
        const SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
        ScopedGameBalanceDefinitionLockR ptExpTableDef(snoXPTable);
        const ExperienceTable & tExpTable = ptExpTableDef->tExperienceTable;
        nXPToLevel = tExpTable.ptExperienceLevels[nIndex].nDeltaXP;
    }
    const uint64 nTotal = nXPToLevel - Math::Min(nExpNext, nXPToLevel);
    return nTotal;
}

BOOL ExperiencePlayersCloseEnough(const ActorCommonData & tACDVictim, ACDID idACDOtherPlayer)
{
    if (tACDVictim.id == idACDOtherPlayer)
        return TRUE;

    const ActorCommonData & tACDPlayer = ACDGet(idACDOtherPlayer);
    const WorldPlace tPlaceVictim = tACDVictim.GetPlace();
    const WorldPlace tPlaceOther = tACDPlayer.GetPlace();
    if (tPlaceOther.idSWorld != tPlaceVictim.idSWorld)
        return FALSE;

    const WorldDistance wdSeparationSquared = Vector3D::Distance2DSquared(tPlaceVictim.wp, tPlaceOther.wp);
    const float flMaxSeparation = GameGetMaxExpDist();
    if (wdSeparationSquared < flMaxSeparation * flMaxSeparation)
        return TRUE;

    const SNO snoVictimLevelArea = ACDGetLevelArea(tACDVictim);
    if (snoVictimLevelArea != SNO_NONE && snoVictimLevelArea == ACDGetLevelArea(tACDPlayer))
        return TRUE;
    return FALSE;
}

uint64 ExperienceGetRestXPToGrant(const ActorCommonData & tACDPlayer, const uint64 nXPGranted)
{
    if (nXPGranted == 0)
        return 0;

    const float flRestXPBonusPct = tACDPlayer.AttributesGetFloat(FastAttribKey(ATTRIBUTE_REST_EXPERIENCE_BONUS_PCT));
    const uint64 nRestXPToGrant = (uint64)(nXPGranted * flRestXPBonusPct);
    const uint64 nCurrentRestXP = tACDPlayer.GetRestExperience();
    return Math::Min(nRestXPToGrant, nCurrentRestXP);
}

float ExperienceGetBonusFromGear(const ActorCommonData & tACDPlayer)
{
    ScopedGlobalDefinitionLockR pdefGlobal;
    const int32 nPlayerLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    const SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
    ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
    const float flGearXPScalar = tExpTable.ptExperienceLevels[nPlayerLevel - 1].flGearXPScalar;
    float flExpMultiplierForPlayer = 1.0f + flGearXPScalar * tACDPlayer.AttributesGetFloat(FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS_PCT));

    const ACDID idACDHireling = PlayerGetHireling(tACDPlayer.id);
    const ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling && ptACDHireling->IsAlive()) {
        const float flHirelingBonus = ptACDHireling->AttributesGetFloat(FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS_PCT));
        flExpMultiplierForPlayer += flGearXPScalar * (flHirelingBonus * pdefGlobal->flHirelingDiminishedBonusRatio);
    }
    return flExpMultiplierForPlayer;
}

float ExperienceGetHirelingBonus(const ActorCommonData & tACDPlayer)
{
    ScopedGlobalDefinitionLockR pdefGlobal;
    const ACDID idACDHireling = PlayerGetHireling(tACDPlayer.id);
    const ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    float flHirelingExpBonus = 0.0f;
    if (ptACDHireling && ptACDHireling->IsAlive())
        flHirelingExpBonus = ptACDHireling->AttributesGetFloat(FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS)) * pdefGlobal->flHirelingDiminishedBonusRatio;
    return flHirelingExpBonus;
}
