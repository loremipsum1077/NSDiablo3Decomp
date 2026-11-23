// Original file: X/Server/Powers/Power_Barbarian.cpp
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../AI/ActorAI.h"
#include "../Actors/Actors.h"
#include "../SGame.h"
#include "PowerAPI.h"
#include "PowerHelper.h"
#include "Powers.h"
#include <float.h>

static void sBarbarian_CheckForAndApplyMaxFuryBuff(ActorCommonData & tACD);

//@ 42
static void sAddFury(ActorCommonData & tACD, float flFuryInc, BOOL bIgnoreBonuses)





{
    float flFuryMultiplier = 1.0f;
    flFuryInc *= flFuryMultiplier;
    if (Math::Abs(flFuryInc) > 0.000001f)
    {
        tACD.AddResource(RESOURCE_TYPE_FURY, flFuryInc, !bIgnoreBonuses);
    }
}

//@ 59
static void sBarbarian_CheckForAndApplyMaxFuryBuff(ActorCommonData & tACD)

{
    float flFury = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, RESOURCE_TYPE_FURY);
    float flFuryMax = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, RESOURCE_TYPE_FURY);


    SNO snoPowerFury = GlobalSNOGet(GLOBALSNO_POWERS_BARBARIAN_FURY);
    if (flFuryMax - flFury < 0.000001f)
    {
        BuffID idFuryBuff = PowerAPI_BuffFindOnACD(snoPowerFury, 0, tACD, ACDID_INVALID, FALSE);

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idFuryBuff);


        float flBonus = PowerGetFormulaValue(&tACD, AutoPowerDefinitionR(snoPowerFury), 0x51199, -1, -1, 0);

        PowerAPIBuffStartData tBuffStartData(ptBuff->idPower);
        tBuffStartData.nBuffIndex = 1;
        tBuffStartData.eStackType = (BuffStackType)4;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.dwDurationMin = 300;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, flBonus);
        PowerAPI_BuffAddToACD(tBuffStartData, tACD);
    }
}

//@ 88
static void sBarbarian_FuryGainCallback(PowerAPIBuffInstance * ptBuff, float flResourceGained)



{
    ActorCommonData & tACD = ACDGet(ptBuff->idACDAffected);
    sBarbarian_CheckForAndApplyMaxFuryBuff(tACD);
}

//@ 99
static void sBarbarian_AddFuryFromDamageReceived(PowerAPIBuffInstance *, void * pData, PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDTarget)






{




    if (flHitpointDamage < 0.000001f)
    {
        return;
    }

    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
    {
        return;
    }


    SNO snoPowerFury = GlobalSNOGet(GLOBALSNO_POWERS_BARBARIAN_FURY);
    float flPercentOfHealth = Math::Min(1.0f, flHitpointDamage / ptACDTarget->GetHitPointsMaxTotal());
    float flFuryToAdd = flPercentOfHealth * 100.0f * PowerGetFormulaValue(ptACDTarget, AutoPowerDefinitionR(snoPowerFury), 0x51140, -1, -1, 0);
    sAddFury(*ptACDTarget, flFuryToAdd, TRUE);
}

//@ 130
static void sBarbarian_FuryUpdate(PowerAPIBuffInstance * ptBuff, void *)


{
    PowerID idPower = ptBuff->idPower;
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        float flIntervalSecs = PowerGetFormulaValue(idPower, 0x51500);
        ActorID idActor = (ActorID)ptACD->idOwner;
        ActorAI & tAI = ActorGetAI(idActor);

        SNO snoPowerFury = GlobalSNOGet(GLOBALSNO_POWERS_BARBARIAN_FURY);

        uint32 dwFuryDegenerationDelay = (uint32)(PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(snoPowerFury), 0x51195, -1, -1, 0) * 60.0f);

        if (ActorFlagIsSet(idActor, ACTOR_GOD_MODE_BIT))
        {
            float flFuryCurr = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, RESOURCE_TYPE_FURY);
            float flFuryMax = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, RESOURCE_TYPE_FURY);
            if (flFuryCurr < flFuryMax)
            {
                sAddFury(*ptACD, flFuryMax, TRUE);
            }
        }
        else if (tAI.dwLastAttackTime + dwFuryDegenerationDelay < GameGetServerTime() &&
                 tAI.dwLastDamageTime + dwFuryDegenerationDelay < GameGetServerTime())
        {

            float flDecayPerSec = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(snoPowerFury), 0x51191, -1, -1, 0);

            float flReducedDecayFraction = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(snoPowerFury), 0x5119a, -1, -1, 0);

            if (flReducedDecayFraction > 0.0f)
            {
                float flFuryCurr = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, RESOURCE_TYPE_FURY);
                float flFuryMax = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, RESOURCE_TYPE_FURY);

                if (flFuryCurr < flReducedDecayFraction * flFuryMax)
                {
                    flDecayPerSec = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(snoPowerFury), 0x5199b, -1, -1, 0);
                }
            }


            if (!ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_DEGENERATION_STOPPED)))
            {
                float flDecay = flIntervalSecs * flDecayPerSec;
                float flFuryMin = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_DEGENERATION_STOP_POINT, RESOURCE_TYPE_FURY);
                if (flFuryMin > FLT_EPSILON)
                {
                    float flCur = ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, RESOURCE_TYPE_FURY);
                    if (flCur <= flFuryMin)
                    {
                        return;
                    }


                    flDecay = Math::Min(flDecay, flCur - flFuryMin);
                }

                if (ptACD->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, RESOURCE_TYPE_FURY) >
                    0.000001f)
                {
                    ptACD->HealFromResourceSpent(flDecay, RESOURCE_TYPE_FURY);
                }
                sAddFury(*ptACD, -flDecay, TRUE);
            }
        }
        else
        {
            float flFuryInc = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(PowerGetSNO(idPower)), 0x51192, -1, -1, 0);
            flFuryInc *= flIntervalSecs;
            sAddFury(*ptACD, flFuryInc, TRUE);
        }


        sBarbarian_CheckForAndApplyMaxFuryBuff(*ptACD);
    }
}

//@ 212
void Barbarian_Fury(PowerID idPower, void *)


{

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.pPeriodicSpellEffect = sBarbarian_FuryUpdate;

    tBuffStartData.pOnGetHitEffect = sBarbarian_AddFuryFromDamageReceived;
    tBuffStartData.pOnResourceGainEffect = sBarbarian_FuryGainCallback;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.bRemoveOnDeath = FALSE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = (BuffStackType)0;
    tBuffStartData.dwRefreshRate = Math::FloatToInt(PowerGetFormulaValue(idPower, 0x51500) * 60.0f);

    PowerAPI_BuffAddToACD(tBuffStartData, *PowerGetOwner(idPower));
}
