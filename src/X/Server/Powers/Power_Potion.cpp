// Original file: X/Server/Powers/Power_Potion.cpp
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Target.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/MemoryPools.h"
#include "../AI/AIStates.h"
#include "../Crafting/SItemCrafting.h"
#include "PowerAPI.h"
#include "PowerHelper.h"
#include "PowerTrigger.h"
#include "Power_Potion.h"
#include "Powers.h"
#include "SpellEffect.h"

uint8 * PowerGetSkillData(PowerID idPower, uint32 dwSizeDesired);  // Powers.cpp:5087
void PowerHelper_CostsInit(PowerID idPower, PowerAPICosts & tCosts);  // PowerHelper.cpp
BOOL PowerAPI_DoCosts(const PowerAPICosts & tCostInfo, float flResourceCostModifier);  // PowerAPI.cpp:7116
float PowerGetPayloadParam0(PowerID idPower);  // Powers.cpp:2191
BOOL SACDInventoryDeleteItemAfterUse(ActorCommonData & tACDItem);  // ActorInventory.cpp:7193

static void sPotionPower_Delayed(const PowerID idPower, void *);

//@ 33
static PotionPowerData * sPotionPower_GetData(const PowerID idPower)
{

    return (PotionPowerData *)PowerGetSkillData(idPower, sizeof(PotionPowerData));
}

//@ 42
static void sPotionPower_PopulateAttributes(const PowerID idPower, AttribGroupID idFastAttrib)


{
    PowerHelper_PopulateCommonAttributes(idPower, idFastAttrib);
}

//@ 52
void sPotionPower_Start(const PowerID idPower, const PotionPowerData * ptPotionPowerData)


{
    if (ptPotionPowerData == NULL)
    {
        return;
    }





    PotionPowerData * ptData = sPotionPower_GetData(idPower);

    *ptData = *ptPotionPowerData;

    PowerAPIAnimation tAnimation;
    PowerHelper_AnimationInit(idPower, tAnimation);
    PowerAPI_DoAnimation(tAnimation);
}

//@ 76
static void sPotionPower_ChooseTargetsInitialize(const PowerID idPower, const NavPlace & tNavPlace, PowerAPIChooseTargets & tTargetingInfo)



{
    WorldDistance wdRadius = PowerGetPayloadParam0(idPower);
    PowerHelper_ChooseTargetsInit(idPower, tTargetingInfo);
    PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, wdRadius, tTargetingInfo.tGeometry);
}

//@ 86
static void sPotionPower_DoDamage(const PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);




    NavPlace tNavPlace; BOOL bSuccess = ptPower->tTarget.GetNavPlace(tNavPlace);

    if (!bSuccess)
    {
        return;
    }



    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);


    PowerAPIChooseTargets tTargetingInfo;
    sPotionPower_ChooseTargetsInitialize(idPower, tNavPlace, tTargetingInfo);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tTargetingInfo, listTargets);


    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}

//@ 119
static void sPotionPower_Contact(const PowerID idPower)
{

    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (ptPower->tTarget.IsInvalidForSource(ptACDOwner))
    {
        return;
    }

    ACDID idACDItem = sPotionPower_GetData(idPower)->idACDItem;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
    {
        return;
    }


//@ 137
    SACDInventoryDeleteItemAfterUse(*ptACDItem);


    if (ptPower->tTarget.eType == TT_ACTOR)
    {
        WorldPlace tPlace;
        ptPower->tTarget.GetPlace(tPlace);
        ptPower->tTarget.InitializeFromPlace(tPlace);
    }

    PowerAPICosts tCosts;
    PowerHelper_CostsInit(idPower, tCosts);
    PowerAPI_DoCosts(tCosts, 1.0f);


    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptPower->idACD, tNavPlaceSource, SNO_NONE, -1, NULL);
    PowerAPICreateProjectile tProjectile;
    PowerHelper_ProjectileInit(idPower, ptPower->idACD, tNavPlaceSource, ptPower->tTarget, (SpellEffect)0, tProjectile);

    PowerAPI_DoProjectile(tProjectile);

    AIStateActivateCooldown(*ptPower);
}

//@ 165
static void sPotionPower_Delayed(const PowerID idPower, void *)


{
    sPotionPower_DoDamage(idPower);
}

//@ 175
static void sPotionPower_Impact(const PowerID idPower, const ImpactData * pImpactData)




{
    Power * ptPower = PowerGet(idPower);

    NavPlace tNavPlace;
    SpellEffect_GetProjectileImpactData(
        pImpactData->idACDProjectile,
        pImpactData->idACDIntersected,
        ptPower->tTarget);


    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, tNavPlace.tPlace, tExplosion);

    PowerAPI_DoExplosion(tExplosion);


    sPotionPower_DoDamage(idPower);


    int32 nDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, 0x50ce0) * 60.0f);
    int32 nLoops = Math::FloatToInt(PowerGetFormulaValue(idPower, 0x50cf0));
    for (int32 i = 0; i < nLoops; i++)
    {
        PowerTriggerAddTimerTrigger(idPower, nDelay * (i + 1), sPotionPower_Delayed, NULL);
    }


    ACDID idACDProjectile = pImpactData->idACDProjectile;
    PowerAPIDestroyProjectile tProjectile;
    PowerHelper_StopProjectileInit(idPower, idACDProjectile, pImpactData->idACDIntersected, tProjectile);
    PowerAPI_DestroyProjectile(tProjectile);
}

//@ 216
static void sPotionPower_ActorDone(const PowerID idPower)
{

    Power * ptPower = PowerGet(idPower);


    VisualInventoryResetSlot(ptPower->idACD, SLOT_PLAYER_LEFT_HAND);
}

//@ 228
void PotionPower(Power * ptPower, PowerMessage eMessage, void * pParam)



{
    switch (eMessage)
    {
    case POWER_POPULATE_ATTRIBUTES:
        sPotionPower_PopulateAttributes(ptPower->id, (AttribGroupID)(size_t)pParam);
        break;
    case POWER_UI_DAMAGE_NUMBERS:
        PowerHelper_AttackPayloadInit_Standard(ptPower->id, *(PowerAPIAttackPayload *)pParam);
        break;
    case POWER_START:
        sPotionPower_Start(ptPower->id, (const PotionPowerData *)pParam);
        break;
    case POWER_CONTACT:
        sPotionPower_Contact(ptPower->id);
        break;
    case POWER_IMPACT:
        sPotionPower_Impact(ptPower->id, (const ImpactData *)pParam);
        break;
    case POWER_ACTOR_DONE:
        sPotionPower_ActorDone(ptPower->id);
        break;
    }
}
