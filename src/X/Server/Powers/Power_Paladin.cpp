// Original file: X/Server/Powers/Power_Paladin.cpp
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../AI/BrainComponents.h"
#include "../Actors/ActorEffect.h"
#include "PowerAPI.h"
#include "Powers.h"























static void sLoyaltyBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);
    tBuffStartData.nAuraPriority = 2;

    float flHitPointsGranted = PowerGetFormulaValue(idPower, TAG_POWER_HITPOINTS_TO_HEAL);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS, flHitPointsGranted);


    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}


void Templar_Loyalty(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sLoyaltyBuff, AURA_TYPE_SELF_AND_MASTER, FALSE);
}




static void sInspireBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);
    tBuffStartData.nAuraPriority = 3;

    float flManaGranted = PowerGetFormulaValue(idPower, TAG_POWER_MANA_GAINED_PER_SECOND);
    float flArcanumGranted = PowerGetFormulaValue(idPower, TAG_POWER_ARCANUM_GAINED_PER_SECOND);
    float flPowerGranted = PowerGetFormulaValue(idPower, TAG_POWER_POWER_GAINED_PER_SECOND);

    float flFuryBonus = PowerGetFormulaValue(idPower, TAG_POWER_FURY_GENERATION_BONUS_PERCENT);
    float flSpiritBonus = PowerGetFormulaValue(idPower, TAG_POWER_SPIRIT_GENERATION_BONUS_PERCENT);
    float flFaithBonus = PowerGetFormulaValue(idPower, TAG_POWER_FAITH_GENERATION_BONUS_PERCENT);
    float flEssenceBonus = PowerGetFormulaValue(idPower, TAG_POWER_ESSENCE_GENERATION_BONUS_PERCENT);


    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, RESOURCE_TYPE_MANA), flManaGranted);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, RESOURCE_TYPE_ARCANUM), flArcanumGranted);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, RESOURCE_TYPE_HATRED), flPowerGranted);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_REGEN_PER_SECOND, RESOURCE_TYPE_FAITH), flFaithBonus);


    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_FURY), flFuryBonus);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_SPIRIT), flSpiritBonus);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, RESOURCE_TYPE_ESSENCE), flEssenceBonus);

    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}




void Templar_Inspire(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sInspireBuff, AURA_TYPE_SELF_AND_MASTER, FALSE);
}




void Templar_Protection(PowerID idPower, void *)

{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;

        float flDamageAborbPercent = PowerGetFormulaValue(idPower, TAG_POWER_TEMPLAR_PROTECTION_DAMAGE_ABSORB_PERCENT);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_ABSORB_PERCENT, flDamageAborbPercent);

        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}







static void sGuardianBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);
    tBuffStartData.nAuraPriority = 2;

    float flResistanceLightning = PowerGetFormulaValue(idPower, TAG_POWER_LIGHTNING_RESISTANCE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_LIGHTNING), flResistanceLightning);

    float flResistanceCold = PowerGetFormulaValue(idPower, TAG_POWER_COLD_RESISTANCE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_COLD), flResistanceCold);

    float flResistanceFire = PowerGetFormulaValue(idPower, TAG_POWER_FIRE_RESISTANCE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESISTANCE, DAMAGE_TYPE_FIRE), flResistanceFire);

    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}




void Templar_Guardian(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sGuardianBuff, AURA_TYPE_SELF_AND_MASTER, FALSE);
}




static void sKnightBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);
    tBuffStartData.nAuraPriority = 4;

    float flMovementSpeedBonus = PowerGetFormulaValue(idPower, TAG_POWER_MOVEMENT_SPEED_PERCENT_INCREASE_MIN);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR, flMovementSpeedBonus);

    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}




void Templar_Knight(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sKnightBuff, AURA_TYPE_SELF_AND_MASTER, FALSE);
}




static void sTributeBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);

    float flGoldFindBonus = PowerGetFormulaValue(idPower, TAG_POWER_GOLD_FIND_BONUS);
    if (flGoldFindBonus > EPSILON)
    {
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_GOLD_FIND, flGoldFindBonus);
    }

    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}




void Templar_Tribute(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sTributeBuff, AURA_TYPE_MASTER_ONLY, FALSE);
}




static void sEmpathyHitCallback(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, float * flDamageDone, PowerAPIDamage & tPowerDamage, void * pData)

{
    // NOTE: tPowerDamage and pData are unused







    if (!flDamageDone)
        return;

    PowerID idPower = ptBuff->idPower;
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDFollower = PowerGetOwner(ptPower);
    if (!ptACDFollower)
        return;
    if (!ptACDFollower->IsAlive())
        return;
    if (ptACDFollower->id == idACDTarget)
        return;

    float flTotalDamage = 0.0f;



    float flPercentTransfered = PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_REDUCTION_PERCENT);




    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType)
    {
        if (flDamageDone[nDamageType] > EPSILON)
        {
            float flDamageTransferred = flPercentTransfered * flDamageDone[nDamageType];
            flTotalDamage += flDamageTransferred;
            flDamageDone[nDamageType] -= flDamageTransferred;
        }
    }



    SNO snoEffectGroup = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TEMPLAR_EMPATHY);
    ACDPlayEffectGroupACDToACD(snoEffectGroup, idACDTarget, ptACDFollower->id);



    float flCurrentHP = ptACDFollower->GetHitPointsCurrent();
    ptACDFollower->SetHitPoints(flCurrentHP - flTotalDamage, TRUE, FALSE);

    if (ptACDFollower->GetHitPointsCurrent() <= 0.0f)
    {
        BrainAPIDoDeath((ActorID)ptACDFollower->idOwner, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    }
}
static void sEmpathyBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);
    tBuffStartData.pDamageShieldEffect = sEmpathyHitCallback;

    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}




void Templar_Empathy(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sEmpathyBuff, AURA_TYPE_MASTER_ONLY, TRUE);
}
