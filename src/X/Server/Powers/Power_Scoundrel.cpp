// Original file: X/Server/Powers/Power_Scoundrel.cpp
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Items.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Random.h"
#include "../AI/BrainComponents.h"
#include "../Actors/ActorEffect.h"
#include "../SGame.h"
#include "PowerAPI.h"
#include "PowerHelper.h"
#include "Powers.h"

// Declared in X/Server/Loot/Loot.h and X/Server/Ai/HirelingBrain.h, which are not in src/ yet.
void LootDropExplicitGold(const ActorCommonData & tACDLootee, const ACDID idACDAssigned, const int32 nGoldAmount);  // Loot.cpp:3656
void HirelingBrainClearAttack(ActorID idHireling);  // HirelingBrain.cpp:1770








void Scoundrel_RapidFire(PowerID idPower, void *)

{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
        return;





    ACDID idACDPrimaryItem = ACDInventoryGetPrimaryWeapon(ptACD->id);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDPrimaryItem);





    if (!ptACDItem || !ItemHasLabel(ptACDItem->hGB.gbid, ITL_BOWCLASS))
        return;




    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    float flBonus = PowerGetFormulaValue(idPower, TAG_POWER_RAPIDFIRE_ATTACK_SPEED_BONUS_PERCENT);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT, flBonus);
    PowerAPI_BuffAddToACD(tBuffStartData, *PowerGetOwner(idPower));
}


void Scoundrel_Multishot(PowerID idPower, void *)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    float flBonus = PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_COUNT);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_PROJECTILE_AMOUNT_BONUS_PERCENT, flBonus);
    PowerAPI_BuffAddToACD(tBuffStartData, *PowerGetOwner(idPower));
}




static void sAnatomyBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);

    float flCritBonus = PowerGetFormulaValue(idPower, TAG_POWER_ANATOMY_CRIT_BONUS_PERCENT);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED, flCritBonus);

    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}

void Scoundrel_Anatomy(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sAnatomyBuff, AURA_TYPE_SELF_AND_MASTER, FALSE);
}




static void sScavengeBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);

    float flGemFindBonus = PowerGetFormulaValue(idPower, TAG_POWER_GEM_FIND_BONUS);
    SNO snoMonsterDropGem = GlobalSNOGet(GLOBALSNO_TREASURECLASS_GEM);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_TREASURE_FIND, snoMonsterDropGem), flGemFindBonus);




    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}

void Scoundrel_Scavenge(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sScavengeBuff, AURA_TYPE_MASTER_ONLY, FALSE);
}




static void sBlackMarketBuff(PowerID idPower, ACDID idACDTarget)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    PowerAPI_AuraSetupBuffStartData(tBuffStartData, idPower);

    float flSalvageFindBonus = PowerGetFormulaValue(idPower, TAG_POWER_SALVAGE_FIND_BONUS);
    SNO snoMonsterDropCraftingReagent = GlobalSNOGet(GLOBALSNO_TREASURECLASS_CRAFTINGREAGENT);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_TREASURE_FIND, snoMonsterDropCraftingReagent), flSalvageFindBonus);


    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
}

void Scoundrel_BlackMarket(PowerID idPower, void *)

{
    PowerAPI_AuraStart(idPower, sBlackMarketBuff, AURA_TYPE_MASTER_ONLY, FALSE);
}




static void sScoundrel_RainOfGoldOnHit(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tPowerDamage, float flHitpointDamage, float * flDamageDone, ACDID idACDAttacker, ACDID idACDTarget, ACDID idACDFromPet)

{












    float flChanceToDrop = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_PROC_CHANCE);
    float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
    if (flRoll < flChanceToDrop)
    {

        float flDamageToGoldPercent = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_RAINOFGOLD_DAMAGE_TO_GOLD_PERCENT);
        int32 nGoldToDrop = Math::FloatToInt(flDamageToGoldPercent * flHitpointDamage);
        ACDID idACDGoldOwner = idACDAttacker;
        ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);

        if (ptACDAttacker &&
            ptACDAttacker->IsPet())
        {
            idACDGoldOwner = ACDGetPetOwner(ptACDAttacker->id);
        }


        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        LootDropExplicitGold(tACDTarget, idACDGoldOwner, nGoldToDrop);


        SNO snoEffectGroup = PowerGetEffect(ptBuff->idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
        if (snoEffectGroup != SNO_NONE)
        {
            ACDPlayEffect(idACDAttacker, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup, ACDID_INVALID);
        }
    }
}
void Scoundrel_RainOfGold(PowerID idPower, void *)

{
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.pOnHitEffect = sScoundrel_RainOfGoldOnHit;
    tBuffStartData.bInfinite = TRUE;
    PowerAPI_BuffAddToACD(tBuffStartData, *PowerGetOwner(idPower));
}




void Scoundrel_DirtyFighting(PowerID idPower, void *)

{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;




    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
        return;





    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
        return;




    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);



    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}
void Scoundrel_Bandage(PowerID idPower, void *)

{
}






void Scoundrel_Vanish(PowerID idPower, void *)

{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    ActorID idHireling = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;


    if (ptACD->IsHidden())
        return;




    PowerAPIExplosion tExplosion;
    WorldPlace placeSource = ptACD->GetPlace();
    PowerHelper_ExplosionInit(idPower, placeSource, tExplosion);
    PowerAPI_DoExplosion(tExplosion);

    HirelingBrainClearAttack(idHireling);
    BrainAPIDoIdle(idHireling);

    PowerAPIBuffStartData tBuffStartData(idPower);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_AI_IN_SPECIAL_STATE, TRUE);
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_VANISH_DURATION) * 60.0f);
    tBuffStartData.snoEndingEffect = PowerGetEffect(idPower, TAG_POWER_ENDING_EFFECT_GROUP);


    float flHitPointsToHeal = PowerGetFormulaValue(idPower, TAG_POWER_HITPOINTS_TO_HEAL);
    float flRegenPerSecond = flHitPointsToHeal * 60.0f / tBuffStartData.dwDurationMin;
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS, flRegenPerSecond);

    SNO snoPower = PowerGetSNO(idPower);
    Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(snoPower, -1, tBuffStartData.nBuffIndex);
    if (eAttrib != -1)
    {

        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(eAttrib, snoPower), 1);
    }


    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}
