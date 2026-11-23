// X/Server/Powers/Power_Monk.cpp

#include <float.h>

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "PowerAPI.h"
#include "Powers.h"









































inline void sAddSpirit(float flSpiritGain, ACDID idACDMonk, BOOL bApplyOnHitBonuses)
{



    ActorCommonData * ptACD = ACDTryToGet(idACDMonk);
    if (ptACD == NULL)
    {
        return;
    }

    ptACD->AddResource(RESOURCE_TYPE_SPIRIT, flSpiritGain, bApplyOnHitBonuses);
}



static void sMonk_AddSpiritOnHit(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    PowerAPIDamage & tPowerDamage,
    float flHitpointDamage,
    float * flDamageDone,
    ACDID idACDAttacker,
    ACDID idACDTarget,
    ACDID idACDFromPet)
{





    PowerID idActivePower = tPowerDamage.idPower;
    Power * ptCurrentPower = PowerTryToGet(idActivePower);
    Power * ptSpiritPower = PowerTryToGet(ptBuff->idPower);
    if (ptCurrentPower == NULL || ptSpiritPower == NULL)
    {
        return;
    }

    ACDID idACDMonk = (ACDID)(intptr)pData;
    if (idACDMonk != idACDAttacker || idACDAttacker == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    if (ptACDAttacker == NULL)
    {
        return;
    }

    if (ptCurrentPower->dwFlags & (1 << PF_ONE_TIME_EVENT_USED))
    {
        return;
    }

    ptCurrentPower->dwFlags |= (1 << PF_ONE_TIME_EVENT_USED);

    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
    {
        return;
    }

    // Pool constant in the original: FLT_EPSILON = 1.1920929e-07f = _ZL15s_intTypeLookup+0xac (0x34000000).
    if (!ptACDTarget->IsMonster() && !ACDIsPlayer(idACDTarget) && !ptACDTarget->IsPet())
    {
        return;
    }

    float flSpiritGain;
    if (PowerIsBasicAttack(ptCurrentPower->snoPower))
    {
        ScopedGlobalDefinitionLockR pdefGlobal;

        flSpiritGain = pdefGlobal->ptServerData->flMonkSpiritGainedFromBasicAttack;
    }
    else
    {
        flSpiritGain = PowerGetFormulaValue(idActivePower, TAG_POWER_SPIRIT_GAIN);
    }
    if (flSpiritGain > FLT_EPSILON)
    {
        sAddSpirit(flSpiritGain, idACDMonk, TRUE);
    }
}



void Monk_Spirit(
    const PowerID idPower,
    void *)
{
    if (PowerTryToGet(idPower) == NULL)
    {
        return;
    }

    ActorCommonData * ptACDMonk = PowerGetOwner(idPower);
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.pOnHitEffect = sMonk_AddSpiritOnHit;
    tBuffStartData.pOnHitEffectData = (void *)(intptr)ptACDMonk->id;
    tBuffStartData.bInfinite = true;
    tBuffStartData.bDoNotCopy = true;
    tBuffStartData.bUndispellable = true;
    tBuffStartData.bRemoveOnPassiveApplication = false;
    tBuffStartData.bRemoveOnRespec = false;
    tBuffStartData.bRemoveOnDeath = false;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.bPositiveEffect = true;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDMonk);
}
