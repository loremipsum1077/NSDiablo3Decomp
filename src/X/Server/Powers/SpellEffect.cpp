// X/Server/Powers/SpellEffect.cpp
#include "../../Common/Actors/ActorEffectSwig.h"
#include "SpellEffect.h"
#include "../../Client/Console/NX64NFP.h"
#include <float.h>
#include "../../Common/GameConstants.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Monsters.h"
#include "../../Common/TagMaps/MonsterTags.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Powers/SpellEffectCommon.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Target.h"
#include "../../Common/WorldCommonCollision.h"
#include "../../Sigma/MemoryPools.h"
#include "../AI/AIStates.h"
#include "../AI/ActorAI.h"
#include "../AI/BrainComponents.h"
#include "../AI/BrainStance.h"
#include "../Actors/ActorEffect.h"
#include "../Actors/ActorPowers.h"
#include "../Actors/Actors.h"
#include "../LuaScript/ScriptEvent.h"
#include "../Movement/SActorMovement.h"
#include "../PlayerList.h"
#include "../SPlayer.h"
#include "PowerAPI.h"
#include "PowerHelper.h"
#include "PowerTrigger.h"
#include "Powers.h"
#include "PowersSwig.h"
#include "../SActTransitions.h"
int32 MarkerGetACDsLinkedByActor(const ActorID idActor, XPooledDataIDList & listACDs, LPCSTR pszGroupName);  // X/Server/Marker/SMarker.cpp:1231
// X/Server/Powers/Power_ItemUse.h
void PowerStartUseItem(ACDID idACDPlayer, ACDID idACDItemToUse, const Target * ptTarget);  // Power_ItemUse.cpp:2086
// X/Server/Network/STrickle.h
void STrickleTriggerUpdate();  // STrickle.cpp:129
// X/Server/Powers/Power_ItemUse.h
typedef int32 ItemUseResult;
ItemUseResult ItemUse(const PowerID idPower, const ACDID idACDUser, const ACDID idACDItemToUse, const Target * ptTarget);  // Power_ItemUse.cpp:1836
// X/Server/SGizmo.h
void SGizmoOperated(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator);  // SGizmo.cpp:2489
void SGizmoOperatePortal(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator);  // SGizmo.cpp:1828
void SGizmoOperateSwitch(ActorCommonData & tACD, const BOOL bSet);  // SGizmo.cpp:696
BOOL SGizmoConsumeItemToOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator);  // SGizmo.cpp:2528
// X/Server/SItems.h
BOOL SItemPlayerIdentify(ActorCommonData & tACDItem, ActorCommonData * ptACDPlayer);  // SItems.cpp:44
// X/Server/Network/ClientValidation.h
BOOL CValidateItemIsInPlayerInventory(const Player & tPlayer, const ACDID idACDItem, BOOL bExcludeHirelingSlots);  // ClientValidation.cpp:873
// X/Common/Items.h
BOOL ItemIsUnidentified(const ActorCommonData & tACDItem);  // Items.cpp:3679
// X/Common/ActorInventory.h
typedef void (*InventorySearchCallback)(ACDID, InventorySlotType, void *);
void ACDInventoryIterateItems(const ActorCommonData & tACD, InventorySlotType eSlot, uint32 dwACDIIFlags, InventorySearchCallback pfnCallback, void * pCallbackUserData);  // ActorInventory.cpp:2646
extern const InventorySlotType cgtDefaultPlayerInventorySlots[15];
ACDID ACDInventoryGetActiveWeapon(const ActorCommonData & tACD);  // ActorInventory.cpp:5398
// X/Server/Powers/Powers.h
void PowerStartDataInit(PowerStartData & tPowerStartData);  // Powers.cpp:5066
PowerID PowerStartEx(const PowerStartData & tPowerStartData, AttribGroupID idFastAttribSource, BOOL bForceStart, BOOL bIgnoreRange);
void PowerApplyGenericAttributeBuffAttributes(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, AttribGroupID idFastAttrib);  // PowersCommon.cpp:8452
TimerID ActorDeleteSoon(ActorID idActor, int32 nDuration);  // Actors.cpp:4470
// X/Server/ConversationMgr.h
namespace ConversationMgr {
BOOL StartConversation(const int32 nPlayerTalkingIndex, const SNO snoConversation, const ACDID idACDTalkingTo, const ACDID idACDAlt1, const ACDID idACDAlt2, const ACDID idACDAlt3, const ACDID idACDAlt4, const int32 nPriority, const BOOL bForceInterrupt, const BOOL bPlayerInitiated);  // ConversationMgr.cpp:2032
}
class PathPoint2DList;
BOOL PowerComputeDestination(const ACDCollSettings & tCollSettingsDuring, const ACDCollSettings & tCollSettingsDest, const ActorMovement & tMovement, const WorldPlace & placeTarget, const WorldPlace & placeStart, WorldPlace & placeResult, uint32 uRequirePathNoOverlapFlags, float flSearchRadiusMax, PathPoint2DList * ptListPathPoints);  // Powers.cpp:5557
void PowerAPI_DoBurrowIn(ActorCommonData * ptACDTarget, BOOL bInvisibleDuring, WeaponClass eBurrowWeaponClass, int32 nAppearanceParameter, BOOL bAlphaAttachments);  // PowerAPI.cpp:13403
void PowerAPI_FreeFromRoot(const ACDID idACD);  // PowerAPI.cpp:12777
void PowerAPI_FreeFromSnare(const ACDID idACD);  // PowerAPI.cpp:12817
void PowerAPI_FreeFromStun(const ACDID idACD);  // PowerAPI.cpp:12851
void PowerAPI_FreeFromFear(const ACDID idACD);  // PowerAPI.cpp:12930
SNO PowerContactTagGetSNO(PowerID idPower, uint32 dwTag);  // Powers.cpp:4238
SNO PowerEffectTagGetSNO(PowerID idPower, uint32 dwTag);  // Powers.cpp
float PowerGetPayloadParam0(PowerID idPower);  // Powers.cpp:2191
int32 PowerContactTagGetInt(PowerID idPower, uint32 dwTag);  // Powers.cpp
// X/Common/ActorCommonData.h
void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace);  // ActorCommonData.cpp:4668
// X/Common/Actors/ActorCollision.h
void ACDsCylinderIntersectSphere(const WorldPlace & placeCenter, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // ActorCollision.cpp:349
void ACDsIntersectCapsule(SWorldID idSWorld, const Capsule & capsule, XPooledDataIDList & listACDs, BOOL bForPower);  // ActorCollision.cpp:518
// X/Common/ActorCommonData.h
Angle ACDGetYaw(ACDID idACD);  // ActorCommonData.cpp:2997
// X/Server/Powers/PowerHelper.h
BOOL PowerHelperCheckLineOfSiteOnTarget(PowerID idPower, PassabilityType ePassabilityType);  // PowerHelper.cpp
// X/Server/AI/BrainStance.h
void BrainStanceSetIdleAnimation(ActorID idActor, uint32 dwIdleAnimationTag);  // BrainStance.cpp:2589
// X/Server/AI/AIStates.h
void AIStateSetActive(ActorID idActor, BOOL bActive);  // AIStates.cpp:6045
void BrainStanceDoKnockBack(PowerID idPower, ActorID idActor, Angle aYaw, WorldDistance wdKnockbackMagnitude, float flHeight, float flGravity, uint32 dwAnimTag, BOOL bKnockbackIgnoresWeight, SNO snoKnockbackPower, BOOL bSuperKnockback, BOOL bContinuousKnockbackPower);  // BrainStance.cpp:3044
ACDID PowerAPI_GetRootSource(const ACDID idACD);  // PowerAPI.cpp:12972
void sRoot_KillRooter(PowerAPIBuffInstance * ptBuff, BuffRemovalReason eReason, void * pData);
// X/Common/Networking/GameMessage.h (kept local, as in SPlayer.cpp and PowerAPI.cpp)
#include "../../Common/Actors/ComplexEffect.h"
enum { FLOATING_NUMBER_TYPE_ROOT = 12 };
#include "../Achievements/AchievementModifierEvaluationLogic.h"
#include "../../Common/Achievements/AchievementDefines.h"
// ExperienceCommon.cpp:78; no recovered header declaration.
BOOL ExperiencePlayersCloseEnough(const ActorCommonData & tACDVictim, ACDID idACDOtherPlayer);
// NX64NFP.cpp:34; no recovered header declaration.
namespace Console { namespace NFP { extern uint32 s_nNFPScanningSeconds; } }
// X/Server/AI/SanctuaryBrains.h
void SkeletonShieldBearerLowerShield(ACDID idACD);  // SanctuaryBrains.cpp:607
// X/Server/Actors/SComplexEffect.h
typedef int32 SComplexEffectID;
int32 SComplexEffectAdd(ActorCommonData & tACD, const SNO snoEffectGroup, const ACDID idACDDestParam, int32 eType, uint32 dwParam1, uint32 dwParam2, BOOL bIgnoreOwnerAlpha);  // SComplexEffect.cpp:175
void SComplexEffectRemove(const ACDID idACD, const int32 idSComplexEffect);  // SComplexEffect.cpp:247
void PowerAbortChannelled(ACDID idACDPlayer, SNO snoPower);  // Powers.cpp:5100
// X/Common/Nav/NavigationMesh.h
BOOL NavMeshIsPointOnHandle(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp);  // NavigationMesh.cpp:676
void NavMeshRollDataInitialize(NavMeshRollData & tRollData);  // NavigationMesh.cpp:1937
BOOL NavMeshRollMarble(NavMeshRollData & tRollData);  // NavigationMesh.cpp:2000
BOOL NavMeshCallBackPassabilityTypeCanEnter(void * pCallbackData, const NavMeshHandle & tNavMeshHandle);  // NavigationMesh.cpp:3383
// X/Server/AI/BrainComponents.h
void BrainAPIDoPlayAnimation(const ActorID idActor, const uint32 dwAnimTag, const uint32 dwDuration, const BOOL bAllowPlayingAnimInterrupt, const BOOL bInterruptableAnim, float flUnitStartTime, const BOOL bDontSendToClient, const float flSpeedScalar);  // BrainComponents.cpp:491
// X/Server/Powers/PowerAPI.h
int32 PowerAPI_GetSummonCountPer(ActorCommonData & tACD, SNO snoPower);  // PowerAPI.cpp
float PowerAPI_GetSummonLifetime(ActorCommonData & tACD, SNO snoPower);  // PowerAPI.cpp
SNO PowerGetExplosionSNO(PowerID idPower);  // Powers.cpp
void SpellEffect_Projectile(PowerID idPower, void * pData);  // SpellEffect.cpp:1567
void sSpellEffect_SpawnBannerBase(PowerID idPower, void *);  // SpellEffect.cpp:14994
// X/Common/ActorCommonData.h, X/Server/SPlayer.h, X/Common/VisualInventory.h
void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace);  // ActorCommonData.cpp:4668
void VisualInventoryCopy(ACDID idACDSource, ACDID idACDDest);  // VisualInventory.cpp:1181
void BrainUpCallOnPowerStop(ActorID idActor, PowerID idPower, BOOL bAllowAIUpdate);  // BrainStance.cpp:2191
// X/Common/Powers/PowersCommon.h, X/Common/Definitions/LevelAreaDefinition.h
BOOL PowerRollToTargetNavPlace(ACDID idACD, const Target & tTarget, const PassabilityType ePassabilityType, const BOOL fIgnoreDestinationHeight, NavPlace & tNavPlace);  // PowersCommon.cpp:5139
BOOL LevelAreaIsTown(SNO snoLevelArea);  // LevelAreaDefinition.cpp:463
// X/Server/Ai/SiegebreakerDemonBrain.h, X/Common/WorldCommonCollision.h
Vector3D SiegebreakerDemonBrainGetLastChargePosition(ActorID idActor);  // SiegebreakerDemonBrain.cpp:702
void SiegebreakerDemonBrainSetLastChargePosition(ActorID idActor, Vector3D wvPosition);  // SiegebreakerDemonBrain.cpp:692
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);  // WorldCommonCollision.cpp:147
// X/Server/Powers/SpellEffect.cpp-local data type (DWARF)
struct PandemoniumData
{
    GlobalSNO eGlobalSnoSpot;
    GlobalSNO eGlobalSnoPortal;
    LPCSTR szGameFlag;
};
enum { ACTOR_SPHERE_TREE_FLAGS_ALL = 7 };  // X/Common/Actors/ActorCollision.h
enum { GB_INVALID = -1 };  // X/Common/GameBalance.h
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // ActorCollision.cpp:317
ActorID GizmoAddBySNO(int32 nLevel, ActorAddStruct & tActorAdd);  // Actors.cpp:5803
BOOL SACDInventoryDeleteItemAfterUse(ActorCommonData & tACDItem);  // ActorInventory.cpp:7193
// X/Server/Actors/Projectile.h, PowerAPI.h
BOOL ProjectileReflect(ACDID idACDProjectile, ACDID idACDReflector, BOOL bReflectYawOnly, Angle aYawOffset, Team eTeam, float flSpeedMult);  // Projectile.cpp:1127
BOOL PowerAPI_BuffApplyReflectEffects(ACDID idACDTarget, ImpactData * ptImpact);  // PowerAPI.cpp:11600
// X/Server/Powers/Powers.h, PowerAPI.h, PowerHelper.h, PowerAttributes.h, Projectile.h
void PowerAddAffectedACDs(PowerID idPower, XPooledDataIDList & listTargets, const ACDID idACDSource);  // Powers.cpp:4597
BOOL PowerAPI_DoChooseExplosion(const ChooseTargetGeometry & tGeometry, const XPooledDataIDList & listTargets, WorldPlace & placeExplosion);  // PowerAPI.cpp:6511
BOOL ProjectileIsLobbed(ACDID idACDProjectile);  // Projectile.cpp:1308
void PowerHelper_ExplosionProjectileInit(PowerID idPower, const WorldPlace & placeSource, XPooledDataIDList & listTargets, const ImpactData * pImpactData, PowerAPIExplosion & tExplosion);  // PowerHelper.cpp:1273
SNO PowerGetProjectileStickSNO(PowerID idPower);  // Powers.cpp:671
float PowerAttributesGetFloat(PowerID idPower, FastAttribKey tKey);  // PowerAttributes.cpp:17
// X/Server/Powers/PowerAPI.h (kept local, as in PowerHelper.cpp)
struct PowerAPIChooseOpenGround {  // size 0x20
    NavPlace tNavPlace;  // 0x00
    float flSkipDistance;  // 0x18
    BOOL bUseActorPassability;  // 0x1c
};
void PowerHelper_ChooseOpenGroundInit(PowerID, const NavPlace & tNavPlace, PowerAPIChooseOpenGround & tOpenGroundInfo, float flSkipDistance);  // PowerHelper.cpp:833
BOOL PowerAPI_DoChooseOpenGround(ActorCommonData & tACD, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & placeOpenSpot, NavMeshHandle & tNavMeshHandleResult);  // PowerAPI.cpp:6235
BOOL PowerPlaceToNavMeshHandle(const WorldPlace & tPlace, NavMeshHandle & tNavMeshHandle);  // PowersCommon.cpp:5266
// X/Common/Powers/PowersCommon.h
void PowerGetTargetPlace(AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldPlace & tPlace);  // PowersCommon.cpp:2949
// X/Server/Powers/PowerHelper.h
SNO PowerHelper_GetAnimForPowerOwner(const PowerID idPower);  // PowerHelper.cpp:1669
BOOL PowerContactTagGetBool(PowerID idPower, uint32 dwTag);  // Powers.cpp
// X/Common/WorldCommonCollision.h
BOOL WorldCollSweptPoint(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);  // WorldCommonCollision.cpp:62
// X/Common/Nav/NavigationMesh.h
uint32 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability);  // NavigationMesh.cpp:3180
uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability);  // NavigationMesh.cpp:3187
BOOL NavMeshPlaceCanEnterForSpawning(const WorldPlace & placeSpawn, const PassabilityType ePassabilityType);  // NavigationMesh.cpp:3320
SNO sDeathPortalChoosePortal(const PowerID idPower);  // SpellEffect.cpp:16646
BOOL SpellEffect_DeathPortalCreate(const SNO snoActor, const WorldPlace tPlace, Power * ptPower);  // SpellEffect.cpp:16681
// X/Server/AI/BrainComponents.h
void BrainAPIDoScriptedDeath(ActorCommonData & tACD, int32 eDeathType, int32 nParam1, int32 nParam2, ActorID idActorKiller);  // BrainComponents.cpp
// X/Server/Powers/PowersSwig.h
float PowerGetMonsterFormulaValue(Power * ptPower, uint32 dwTag);  // PowersSwig.cpp
void sWallMonsterSpitMonster(PowerID idPower, void * pData);  // SpellEffect.cpp:13560
// X/Server/Actors/Projectile.h
void ProjectileSeekActor(ACDID idACDProjectile, ACDID idACDTarget, float flMaxAngleToRotate, BOOL bYawOnly);  // Projectile.cpp
// X/Server/Powers/Powers.h
BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeSource, const WorldPlace & placeRolloutSource, WorldPlace & placeDest, int32 nFlags, float flDistance);  // Powers.cpp
// X/Common/AnimPlayer.h
void AnimPlayerSendAnimationToPlayer(ActorCommonData & tACD, uint32 dwAnimationTag, uint32 dwLength, AnimReason eAnimReason, const Player & tPlayer);  // AnimPlayer.cpp
// X/Server/Monsters/ZombieCrawler.h
BOOL ZombieCrawlerGetSpawnNavPlace(ACDID idACD, NavPlace & tNavPlace);
// X/Server/AI/BrainStance.h
BOOL BrainStanceDoPower(ActorID idActor, const Target & tTarget, SNO snoPower, EBrainPowerResult eResult, void * pData, const PreplayedAnimData * ptPreplayedAnimData, int32 nComboLevel, uint32 dwFlags);  // BrainStance.cpp
// X/Server/SScene.h
ACDID SSceneFindFirstGizmo(SSceneID idSScene, GizmoType eGizmoType);  // SScene.cpp
// X/Server/Gizmos.h
void GizmoSendHearthPortalToPlayer(Player & tPlayer, ACDID idACDPortal);  // Gizmos.cpp
// X/Server/AI/BrainComponents.h
void BrainAPIResurrectByType(ACDID idACD, int32 eResurrectType);  // BrainComponents.cpp
// X/Server/NPCHelper.h
void NPCHelperDetach(ActorCommonData & tACDPlayer, ActorCommonData & tACDHelper);  // NPCHelper.cpp
// X/Common/Player.h
int32 PlayerGetNumPetsOfType(ACDID idACDPlayer, int32 ePetType);  // Player.cpp
ACDID PlayerGetFirstPet(ACDID idACDPlayer, int32 ePetType, int32 & nPos);  // Player.cpp
// X/Server/SPlayer.h
int32 SPlayerAddPet(ACDID idACDPlayer, PetType eType, ACDID idACDPet, BOOL fSendNow, int32 nMaxCount, PetComparisonValueFunc pCompareFunc);  // SPlayer.cpp
// X/Server/Experience.h
void ExperienceDropLootWithinExpRange(ActorCommonData & tACDLootee, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist);  // Experience.cpp:1149

struct SpellEffectRunData {  // size 0x20
    SpellEffect eSpellEffect;  // 0x00
    PowerTriggerCallback pfnCallback;  // 0x08
    GlobalSNO tGlobalSNOs[4];  // 0x10
};
#include "../../Sigma/DataStructures/XVar.h"
static XVarScope s_scopeHacks("Hacks", NULL);  // line 117
XVarBool s_varHacksIdentifyLogging(s_scopeHacks, "IdentifyLogging", false);  // line 118
// X/Server/Actors/Projectile.h, X/Server/LuaScript/LuaScript.h, X/Server/SWorld.h
const ActorCommonData * ProjectileGetOwnerACD(ACDID idACDProjectile);  // Projectile.cpp:1080
void ProjectileSetSpeedMultiplier(ACDID idACDProjectile, float flSpeedMultiplier, int32 nDuration, BOOL bAdjustDetonationTimes, BOOL bSmoothSlow, BOOL bForce);  // Projectile.cpp:958
BOOL IsTriggerEventTargetingActor(ActorID idActor);  // LuaScript.cpp:642
SWorldID SWorldFindBySNO(const SNO & snoWorld);  // SWorld.cpp:1406
// X/Server/AI/AIStates.h
WorldPlace AIStateGetRandomLOSPlace(ACDID idACD, float flDistMin, float flDistMax);  // AIStates.cpp:1519
ACDID sSpawnMorluCasterMeteorProxy(PowerID idPower, WorldPlace tPlace, SNO snoActor, uint32 nDuration);  // SpellEffect.cpp:16304

// X/Server/AI/BrainComponents.h, X/Server/Network/ServerMessageSend.h, X/Common/Tutorial.h
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce);  // BrainComponents.cpp:7464
void ServerMessageSendForceInterfaceClose(const ACDID idACDPlayer);  // ServerMessageSend.cpp:1450
namespace Tutorial { BOOL SendTutorialToClient(SNO snoTutorial, Player * ptPlayer, const char * szUIAnchorName, const ACDID idACDTarget); }

// X/Server/Actors/Actors.h, X/Server/Powers/Powers.h
void ActorGetMonsterHPData(SNO snoMonster, int32 nLevel, MonsterRarity eMonsterRarity, float & flHP, float & flHPDelta, const BOOL bAvenger, const BOOL bIsTieredLootRunMonster, const BOOL bIsSetDungeon, const BOOL bUnknown9);  // Actors.cpp:2157
BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax);  // Powers.cpp:5481
#include "../../Common/Definitions/MonsterDefinition.h"
static void sSpellEffect_ResurrectCorpse(PowerID idPower, void *)  // line 123
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    if (ptPower->tTarget.IsInvalidForSource(ptACD))
    {
        return;
    }
    ACDID idACDCorpse = ptPower->tTarget.idACDTarget;
    if (idACDCorpse == ACDID_INVALID)
    {
        return;
    }
    ActorCommonData & tACDCorpse = ACDGet(idACDCorpse);
    ActorID idCorpse = (ActorID)ACDGetOwner(idACDCorpse);
    BrainAPIDoResurrect(idCorpse, FALSE);
    BrainAPIDoPlayAnimation(idCorpse, TAG_ACTOR_ANIMATION_GETTING_SUMMONED, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
    tACDCorpse.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDCorpse.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    if (ACDIsPlayer(idACDCorpse))
    {
        ServerMessageSendForceInterfaceClose(idACDCorpse);
        SPlayerWarped(tACDCorpse, 0, NULL, ACDID_INVALID, 0.0f, TRUE);
    }
}


static void sSpellEffect_ItemResurrectCorpse(PowerID idPower, void * pData)  // line 170
{
    ACDID idACDItem = (ACDID)(intptr)pData;
    ActorCommonData * ptACD = ACDTryToGet(idACDItem);
    if (ptACD && ptACD->IsItem())
    {
        Power * ptPower = PowerGet(idPower);
        ACDID idACDTarget = ptPower->tTarget.GetACD();
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
        {
            ACDNetworkName idANNTargetRedirect = (ACDNetworkName)ptACDTarget->AttributesGetInt(ATTRIBUTE_HEADSTONE_PLAYER_ANN);
            WorldPlace placeResurrect = ptACDTarget->GetPlace();
            Quaternion qResurrect = ptACDTarget->GetOrientation();
            ACDID idACDTargetRedirect = ACDFromANN(idANNTargetRedirect);
            if (idACDTargetRedirect != ACDID_INVALID)
            {
                ptPower->tTarget.InitializeFromACD(idACDTargetRedirect, FALSE, FALSE);
            }
            ACDID idACDResurrected = ptPower->tTarget.GetACD();
            sSpellEffect_ResurrectCorpse(idPower, NULL);
            SACDInventoryDeleteItemAfterUse(*ptACD);
            if (idACDTargetRedirect != ACDID_INVALID)
            {
                ActorID idActorResurrected = (ActorID)ACDGetOwner(idACDResurrected);
                ActorSnapToPosition(idActorResurrected, placeResurrect, TRUE, TRUE);
                ActorSetOrientation(idActorResurrected, qResurrect, TRUE);
            }
        }
    }
}

// X/Common/SceneCommon.h
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive);  // SceneCommon.cpp




// X/Server/AI/BrainStance.h, X/Server/AI/BrainComponents.h
BOOL BrainStanceIsWalking(ActorID idActor);  // BrainStance.cpp:1035
ACDID BrainAPIGetActorAttackTarget(ACDID idACDAttacker);  // BrainComponents.cpp:3034

// X/Common/Marker/Marker.h
uint32 MarkerStringHash(LPCSTR pszName);  // Marker.cpp:204
#include "../../Common/HirelingSwig.h"
#include "../../Common/Marker/Marker.h"
#include "../Crafting/SItemCrafting.h"
enum { GB_HIRELINGS = 4 };  // X/Common/Definitions/GameBalanceDefinition.h:57 (DWARF header dump)
ActorBrain ActorGetBrainType(const ActorID idActor);  // X/Server/AI/ActorAI.cpp:1355
void HirelingBrainCreateHireling(ActorID idHireling, ActorID idLeader);  // X/Server/AI/HirelingBrain.cpp
void SHirelingClearSkills(ActorCommonData & tACDHireling);  // X/Server/SHireling.cpp:586
PowerID PowerAPI_StartSpawnAnimPower(const ACDID idACD, uint32 dwAnimTag);  // X/Server/Powers/PowerAPI.cpp:12673
void ExperienceUpdatePerLevelHitpointFactorVitality(ActorCommonData & tACD);  // X/Server/Experience.cpp:1293
void ExperienceSetLevel(const ACDID idACD, int32 nLevels, const BOOL fUpdateClient);  // X/Server/Experience.h
void SACDInventoryLoad(ACDID idACDContainer, SNO snoInventory);  // X/Common/ActorInventory.cpp:9594
SNO HirelingGetSNO(HirelingClass eClass, HirelingType eType);  // X/Common/Hireling.cpp:252
#line 217
ACDID SpellEffect_SummonHirelingGuts(
    ACDID idACDOwner,
    HirelingClass eClass,
    const PowerAPISummonActor & tSummonInfo,
    MonsterRarity tRarity)
{
	ActorCommonData & tACDOwner = ACDGet(idACDOwner);
#line 228
	CRefString sActorName(SNOToString(ACTOR_SNO, tSummonInfo.snoActor));
#line 231
	GBHandle hGB(GB_HIRELINGS, HirelingGetGBID(eClass));
#line 233
	ActorAddStruct tActorAdd;
	tActorAdd.dwActorAddFlags = (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
	tActorAdd.tPlace = tSummonInfo.placeSummonLocation;
	tActorAdd.qOrientation = tSummonInfo.qOrientation;
	tActorAdd.eTeam = tACDOwner.GetTeam();
	tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
	tActorAdd.tSNOName.snoHandle = tSummonInfo.snoActor;
	tActorAdd.snoActor = tSummonInfo.snoActor;
	tActorAdd.tMarkersName = MarkerStringHash(sActorName);
	tActorAdd.eMonsterRarity = tRarity;
	tActorAdd.nLevel = tSummonInfo.nActorLevel;
	tActorAdd.hGB = hGB;
	tActorAdd.bIsSummon = TRUE;
	ScenesCommonNavMeshNodeFromPlace(tSummonInfo.placeSummonLocation, &tActorAdd.tNavMeshHandle, FALSE, FALSE);
#line 248
	ActorID idHireling = GBAdd(tActorAdd);
#line 250
	if (idHireling == ACTORID_INVALID)
	{
#line 253
		tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK);
#line 255
		idHireling = GBAdd(tActorAdd);
	}
#line 258
	if (idHireling == ACTORID_INVALID)
	{
		return ACDID_INVALID;
	}
#line 265
	Actor * ptHireling = ActorGet(idHireling);
	if (!ptHireling)
	{
#line 269
		ActorDelete(idHireling);
		return ACDID_INVALID;
	}
#line 273
	ACDID idACDHireling = ptHireling->idACD;
	ActorCommonData & tACDHireling = ACDGet(idACDHireling);
#line 280
	Player * ptPlayer = PlayerGetByACD(idACDOwner);
	if (ptPlayer)
	{
		SPlayerTrickleAdd(*ptPlayer, idACDHireling);
	}
#line 287
	ScopedActorDefinitionLockR pdefActor(tACDHireling.tSNOName.snoHandle);
	ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
	tACDHireling.SetSkillKit(pdefMonster->snoSkillKit, FALSE);
#line 292
	if (ActorGetBrainType(idHireling) == ACTORBRAIN_HIRELING)
	{
#line 295
		HirelingBrainCreateHireling(idHireling, (ActorID)ACDGetOwner(idACDOwner));
	}
#line 301
	SHirelingClearSkills(tACDHireling);
#line 304
	PowerAPI_StartSpawnAnimPower(idACDHireling, TAG_ACTOR_ANIMATION_GETTING_SUMMONED);
#line 309
	const GameBalanceDefinition * pdefHirelings;
	const HirelingEntry * pHireling = (const HirelingEntry *)GBLockR(hGB, pdefHirelings, TRUE);
#line 313
	tACDHireling.AttributesSetFloat(ATTRIBUTE_STRENGTH, pHireling->flAttributesStrength);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_DEXTERITY, pHireling->flAttributesDexterity);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_INTELLIGENCE, pHireling->flAttributesIntelligence);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_VITALITY, pHireling->flAttributesVitality);
	tACDHireling.AttributesSetInt(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE, pHireling->ePrimaryDamageAttribute);
#line 319
	ExperienceUpdatePerLevelHitpointFactorVitality(tACDHireling);
#line 322
	tACDHireling.AttributesSetInt(ATTRIBUTE_LEVEL_CAP, tACDOwner.AttributesGetInt(ATTRIBUTE_LEVEL_CAP));
#line 325
	tACDHireling.AttributesSetFloat(ATTRIBUTE_ATTACKS_PER_SECOND, 1.0f);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_CASTING_SPEED, 1.0f);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BASE, pHireling->flAttributesCritPercentBase);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED, 0.0f);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_BONUS_UNCAPPED, 0.0f);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_CRIT_PERCENT_CAP, pHireling->flAttributesCritPercentCap);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_CRIT_DAMAGE_CAP, pHireling->flAttributesCritDamageCap);
#line 333
	tACDHireling.AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS, 0.0f);
#line 335
	ScopedGlobalDefinitionLockR pdefGlobal;
#line 337
	float flAttackSpeedCap = pdefGlobal->ptServerData->flAttackSpeedItemPercentCap / 100.0f;
	tACDHireling.AttributesSetFloat(ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT_CAP, flAttackSpeedCap);
#line 340
	float flCooldownMinTime = pdefGlobal->ptServerData->flCooldownMinimumTime;
	tACDHireling.AttributesSetFloat(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME, flCooldownMinTime);
#line 343
	float flMovementScalarCap = Math::Max(1.0f, pdefGlobal->ptServerData->flMovementScalarCap);
	tACDHireling.AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR_CAP, flMovementScalarCap);
#line 347
	tACDHireling.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, GlobalSNOGet(GLOBALSNO_POWERS_PUNCH)), TRUE);
	ACDInventoryEquipmentOrSkillChanged(tACDHireling);
#line 350
	GBUnlockR(pdefHirelings);
#line 352
	return idACDHireling;
}
#line 357
static void sUpdateHirelingStats(
    ACDID idACDHireling,
    ActorCommonData & tACDOwner)
{
	ActorCommonData & tACDHireling = ACDGet(idACDHireling);
#line 364
	int32 nActorLevel = Math::Max(tACDOwner.AttributesGetInt(ATTRIBUTE_LEVEL), 1);
	ExperienceSetLevel(idACDHireling, nActorLevel, FALSE);
#line 368
	ScopedGlobalDefinitionLockR pdefGlobal;
#line 371
	tACDHireling.AttributesSetFloat(ATTRIBUTE_CORE_ATTRIBUTES_FROM_ITEM_BONUS_MULTIPLIER, pdefGlobal->ptServerData->flHirelingCoreAttributeBonusMultiplier);
	tACDHireling.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
}
#line 377
ACDID SpellEffect_SummonHireling(
    ACDID idACDOwner,
    PowerAPISummonActor & tSummonInfo,
    HirelingType eType)
{
	ActorCommonData & tACDOwner = ACDGet(idACDOwner);
#line 385
	HirelingClass eClass = HirelingGetTypeToCreate(idACDOwner);
#line 387
	HirelingSetTypeToCreate(idACDOwner, HC_NONE);
	CRefString sHirelingType;
	SNO snoActor = HirelingGetSNO(eClass, eType);
#line 392
	SNOName tSNOName;
#line 395
	tSummonInfo.snoActor = snoActor;
	tSummonInfo.nActorLevel = 1;
#line 398
	ACDID idACDHireling = SpellEffect_SummonHirelingGuts(idACDOwner, eClass, tSummonInfo, 5);
	if (idACDHireling != ACDID_INVALID)
	{
#line 402
		if (ACDIsPlayer(idACDOwner))
		{
			SPlayerAddPet(idACDOwner, (eType == HT_NORMAL) ? PETTYPE_HIRELING : PETTYPE_HIRELING_PROXY, idACDHireling, FALSE, -1, NULL);
		}
#line 408
		sUpdateHirelingStats(idACDHireling, tACDOwner);
#line 418
		GBHandle hGB(GB_HIRELINGS, HirelingGetGBID(eClass));
		const GameBalanceDefinition * pdefHirelings;
		const HirelingEntry * pHireling = (const HirelingEntry *)GBLockR(hGB, pdefHirelings, TRUE);
#line 422
		SACDInventoryLoad(idACDHireling, pHireling->snoInventory);
#line 424
		GBUnlockR(pdefHirelings);
	}
#line 427
	return idACDHireling;
}
#line 433
void sBasicMelee(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData & tACD = ACDGet(ptPower->idACD);
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_MELEE;
    tPayload.eAttackDirection = ATTACKDIRECTION_STRAIGHT;
    // 
    if (PowerAffectedByDualWield(AutoPowerDefinitionR(ptPower->snoPower)) && ACDIsPlayer(ptPower->idACD) &&
        ACDIsDualWielding(tACD, TRUE))
    {
        // 
        DualWieldAttackType eAttackType = tACD.AttributesGetInt(ATTRIBUTE_DUALWIELD_HAND);
        // 
        if (eAttackType == DUALWIELD_ATTACK_OFFHAND)
        {
            // 
            tPayload.eAttackDirection = ATTACKDIRECTION_LEFT;
        }
        else
        {
            // 
            tPayload.eAttackDirection = ATTACKDIRECTION_RIGHT;
        }
    }
    // 
    // 
    int32 nTargetsHit = PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    // 
    // 
    // 
    PowerSetParam(idPower, nTargetsHit);
    // 
}

void SpellEffect_BasicMelee(PowerID idPower, void * pData)
{
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        // 
        return;
    }
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        // 
        return;
    }
    // 
    // 
    sBasicMelee(idPower, pData);
}

void SpellEffect_BasicMeleeBothAttack(PowerID idPower, void * pData)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        // 
        return;
    }
    // 
    ActorCommonData & tACD = ACDGet(ptPower->idACD);
    if (PowerAffectedByDualWield(AutoPowerDefinitionR(ptPower->snoPower)) && ACDIsPlayer(ptPower->idACD) &&
        ACDIsDualWielding(tACD, TRUE))
    {
        // 
        // 
        // 
        tACD.AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, DUALWIELD_ATTACK_MAIN);
        SpellEffect_BasicMelee(idPower, pData);
        tACD.AttributesSetInt(ATTRIBUTE_DUALWIELD_HAND, DUALWIELD_ATTACK_OFFHAND);
        SpellEffect_BasicMelee(idPower, pData);
    }
}





static void sSpellEffect_SuicideExplosionDeath(const PowerID idPower, void *)
{
    // 
    // 
    Power * ptPower = PowerGet(idPower);
    // 
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    // 
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);

    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);

    WorldPlace placeSource = ACDGet(ptPower->idACD).GetPlace();
    // 
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, placeSource, tExplosion);
    PowerAPI_DoExplosion(tExplosion);

    // 
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    ActorID idActor = ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID;
    BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
}



static void sSpellEffect_SuicideExplosion(const PowerID idPower, void * pData)
{
    // 
    sSpellEffect_SuicideExplosionDeath(idPower, pData);
    // 
    // 
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    ActorID idActor = ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID;
    ActorFlagForDeletion(idActor);
}





static void sSpellEffect_SuicideExplosionWithLoot(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    // 
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);

    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);

    ActorCommonData & tACD = ACDGet(ptPower->idACD);
    WorldPlace placeSource = tACD.GetPlace();
    // 
    // 
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, placeSource, tExplosion);
    PowerAPI_DoExplosion(tExplosion);

    // 
    ACDID idACDTarget = ACDID_INVALID;
    if (ptPower->tTarget.eType == TT_ACTOR)
    {
        idACDTarget = ptPower->tTarget.idACDTarget;
    }
    // 
    // 
    ExperienceDropLootWithinExpRange(tACD, idACDTarget, (uint32)-1, FALSE, 0.0f);

    // 
    {
        // 
        ActorID idActor = (ActorID)tACD.idOwner;
        BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        ActorFlagForDeletion(idActor);
    }
}



static void sSpellEffect_Suicide(const PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ActorID idActor = (ActorID)ptACD->idOwner;
        BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    }
}






static inline void sDamageTarget(const PowerID idPower, void * pData)
{





    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundTargetFromPayload(idPower, tChooseTargets);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}






static void sSpellEffect_DamageTarget(const PowerID idPower, void * pData)
{



    sDamageTarget(idPower, pData);
}



static void sSpellEffect_DamageAtContactPoint(const PowerID idPower, void *)
{
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            WorldPlace placeContact;
            placeContact.idSWorld = ptACDOwner->GetSWorldID();
            ptACDOwner->GetContactOffset(placeContact.wp, ptPower->dwCurrentContactFrame);

            Target tTarget;
            tTarget.InitializeFromPlace(placeContact);

            PowerAPIChooseTargets tChooseTargets;
            PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, tTarget, tChooseTargets);

            XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
            PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

            // 
            PowerAPIAttackPayload tPayload;
            PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);

            // 
            PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
        }
    }
}




static void sSpellEffect_DamageCloud(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);

    WorldPlace placeSource;
    ptPower->tTarget.GetPlace(placeSource);

    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitFromPayload(idPower, placeSource, ptPower->tTarget, tChooseTargets);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}








static void sCloudInternal(const PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower->tTarget.eType == TT_ACTOR)
    {
        WorldPlace tPlace;
        ptPower->tTarget.GetPlace(tPlace);
        ptPower->tTarget.InitializeFromPlace(tPlace);
    }
    // 
    WorldPlace placeTarget;
    ptPower->tTarget.GetPlace(placeTarget);
    // 
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, placeTarget, tExplosion);
    PowerAPI_DoExplosion(tExplosion);
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    // 
    sSpellEffect_DamageCloud(idPower, NULL);
    // 
    SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (snoEffect != SNO_NONE && ptACDOwner)
    {
        // 
        ACDPlayEffect(ptACDOwner->id, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
    }
    // 
    // 
    // 
    int32 nDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_POISON_CLOUD_INTERVAL_DURATION) * 60.0f);
    int32 nLoops = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_POISON_CLOUD_NUM_INTERVALS));
    // 
    for (int32 i = 0; i < nLoops; i++)
    {
        // 
        PowerTriggerAddTimerTrigger(idPower, nDelay * (i + 1), sSpellEffect_DamageCloud, NULL);
    }
}




static void sSpellEffect_Cloud(const PowerID idPower, void *)
{
    // 
    // 
    // 
    BOOL bLOS = PowerHelperCheckLineOfSiteOnTarget(idPower, PASSABILITY_PROJECTILE);
    if (!bLOS)
    {
        // 
        // 
        // 
        return;
    }
    sCloudInternal(idPower);
}




static void sSpellEffect_Suicide_Cloud(const PowerID idPower, void *)
{
    BOOL bLOS = PowerHelperCheckLineOfSiteOnTarget(idPower, PASSABILITY_PROJECTILE);
    if (!bLOS)
    {
        // 
        // 
        // 
        return;
    }
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        // 
        // 
        // 
        return;
    }
    ptPower->tTarget.InitializeFromACD(ptACDOwner->id, FALSE, FALSE);
    // 
    // 
    sCloudInternal(idPower);
    // 
    // 
    BrainAPIDoDeath((ActorID)ptACDOwner->idOwner, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
}



static void sSpellEffect_SpiderWebImmobolize(const PowerID idPower, void *)
{
    // 
    // 
    Power * ptPower = PowerGet(idPower);

    WorldPlace placeSource;
    ptPower->tTarget.GetPlace(placeSource);

    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitFromPayload(idPower, placeSource, ptPower->tTarget, tChooseTargets);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        POS posOld = pos;
        ACDID idACDCandidate = (ACDID)listTargets.GetNext(pos);
        if (!ACDIsPlayer(idACDCandidate))
        {
            listTargets.RemoveAt(posOld);
        }
    }

    // 
    if (!listTargets.IsEmpty())
    {
        // 
        // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);

        // 
        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    }
}



static void sSpellEffect_SpiderWebSlow(PowerID idPower, void * pData)
{



    ImpactData * pImpactData = (ImpactData *)pData;
    Target tImpactTarget;
    WorldPlace tTargetPlace;
    SpellEffect_GetProjectileImpactData(pImpactData->idACDProjectile, pImpactData->idACDIntersected, tImpactTarget);
    tImpactTarget.GetPlace(tTargetPlace);


    PowerAPIDestroyProjectile tProjectile;
    PowerHelper_StopProjectileInit(idPower, pImpactData->idACDProjectile, pImpactData->idACDIntersected, tProjectile);
    PowerAPI_DestroyProjectile(tProjectile);


    CRefString sActorName("SpiderWebSlow");
    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);


    tActorAdd.tPlace = tTargetPlace;
    tActorAdd.qOrientation.FromYaw(RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, FULL_CIRCLE));
    tActorAdd.eTeam = TEAM_MONSTER_0;
    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

    tActorAdd.tMarkersName = MarkerStringHash(sActorName);
    tActorAdd.nLevel = 1;
    tActorAdd.snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_SPIDER_WEB);

    ActorID idActorWeb = ActorAdd(tActorAdd);
    if (idActorWeb == ACTORID_INVALID)
    {
        return;
    }


    ACDID idACDWeb = ACDFindByActor(idActorWeb);



    uint32 dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DURATION_MIN) * 60.0f);
    uint32 dwDurationDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DURATION_DELTA) * 60.0f);
    ActorDeleteSoon(idActorWeb, dwDurationMin + dwDurationDelta);


    PowerID idPowerWeb = PowerStart(idACDWeb, GlobalSNOGet(GLOBALSNO_POWERS_SPIDER_WEB_SLOW), cgTargetNone);
}






static void sSpellEffect_Heal(PowerID idPower, void *)
{
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    Power * ptPower = PowerGet(idPower);
    if (ptPower->tTarget.IsInvalidForSource(PowerGetOwner(idPower)))
    {
        return;
    }
    ACDID idACDAlly = ptPower->tTarget.idACDTarget;
    if (idACDAlly == ACDID_INVALID)
    {
        return;
    }
    ActorCommonData * ptACDAlly = ACDTryToGet(idACDAlly);
    if (!ptACDAlly || !ptACDAlly->IsAlive())
    {
        return;
    }
    WorldPlace placeSource;
    ptPower->tTarget.GetPlace(placeSource);
    ACDPlayEffect(idACDAlly, EFFECT_CREATE_PARTICLE, PowerEffectTagGetSNO(idPower, TAG_POWER_TARGET_IMPACT_SNO), ACDID_INVALID);
    ACDPlayEffect(idACDAlly, EFFECT_PLAY_EFFECT_GROUP, PowerEffectTagGetSNO(idPower, TAG_POWER_TARGET_IMPACT_EFFECT_GROUP), ACDID_INVALID);
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    float flHitPointsToHeal = PowerGetFormulaValue(idPower, TAG_POWER_HITPOINTS_TO_HEAL);
    ptACDAlly->Heal(flHitPointsToHeal, TRUE);
}



static void sSpellEffect_HealOther(PowerID idPower, void *)
{
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDSource = PowerGetOwner(ptPower);
    if (!ptACDSource)
    {
        return;
    }
    if (ptPower->tTarget.IsInvalidForSource(ptACDSource))
    {
        return;
    }
    ACDID idACDTarget = ptPower->tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
    {
        return;
    }
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget || !ptACDTarget->IsAlive())
    {
        return;
    }
    SNO snoEffectGroup = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
    if (snoEffectGroup != SNO_NONE)
    {
        ACDPlayEffect(idACDTarget, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup, ACDID_INVALID);
    }
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    float flHitPointsToHeal = PowerGetFormulaValue(idPower, TAG_POWER_HITPOINTS_TO_HEAL);
    ptACDTarget->Heal(flHitPointsToHeal, TRUE);
}



static void sDestructableObjectAOE_Damage(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        if (ptPower->tTarget.eType == TT_ACTOR)
        {
            ACDID idACDTarget = ptPower->tTarget.GetACD();
            if (idACDTarget != ACDID_INVALID && !ACDTryToGet(idACDTarget))
            {
                return;
            }
        }
        else if (ptPower->tTarget.eType == TT_NONE)
        {
            // 
            // 
            // 
            // 
            ptPower->tTarget.InitializeFromPlace(ptPower->wpStartLocation);
        }

        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        PowerAPIChooseTargets tTargets;
        // 
        PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, ptPower->tTarget, tTargets);
        PowerAPI_DoChooseTargets(tTargets, listTargets);

        // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    }
}


static void sSpellEffect_DestructableObjectAOE(const PowerID idPower, void *)
{
    // 
    // 
    float flDelay = PowerGetFormulaValue(idPower, TAG_POWER_DESTRUCTABLE_OBJECT_DAMAGE_DELAY);
    uint32 dwDelay = Math::FloatRoundToInt(flDelay * 60.0f);
    // 
    PowerTriggerAddTimerTrigger(idPower, dwDelay, sDestructableObjectAOE_Damage, NULL);
}




static void sSpellEffect_Nova(const PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);

    // 
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD)
    {
        // 
        WorldPlace placeSource = ptACD->GetPlace();
        // 
        // 
        // 
        PowerAPIExplosion tExplosion;
        PowerHelper_ExplosionInit(idPower, placeSource, tExplosion);
        PowerAPI_DoExplosion(tExplosion);

        // 
        // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        // 
        // 
        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    }
}


static void sSpellEffect_Nova_Delayed(const PowerID idPower, void * pData)
{
    // 
    // 
    float flDelay = PowerGetFormulaValue(idPower, TAG_POWER_NOVA_DELAY);
    PowerTriggerAddTimerTrigger(idPower, Math::FloatToInt(flDelay * 60.0f), sSpellEffect_Nova, pData);
}






BOOL SpellEffect_ProjectileWeaponPower_GetTarget(const PowerID idPower, Target & tTargetToShootAt, NavPlace & tNavPlaceSource, Angle aYawOffset)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return FALSE;
    }
    // 
    ActorID idPowerActor = (ActorID)ptACD->idOwner;
    const Actor * ptActor = ActorTryToGet(idPowerActor);
    if (!ptActor)
    {
        return FALSE;
    }
    // 
    if (ptPower->tTarget.IsInvalidForSource(ptACD))
    {
        return FALSE;
    }
    // 
    // 
    WorldPlace placeTarget;
    PowerGetTargetPlace(AutoPowerDefinitionR(ptPower->snoPower), ptPower->tTarget, placeTarget);
    // 
    if (!WorldPlaceInSameWorld(tNavPlaceSource.tPlace, placeTarget))
    {
        return FALSE;
    }
    // 
    // 
    Vector3D wvAim;
    WorldPlaceGetVectorBetween(tNavPlaceSource.tPlace, placeTarget, wvAim);
    // 
    WorldDistance wdAimMagSquared = wvAim.MagnitudeSquared();
    if (wdAimMagSquared > 250.0f * 250.0f)
    {
        wvAim.Normalize();
        wvAim *= 250.0f;
    }
    // 
    wdAimMagSquared = wvAim.MagnitudeSquared();
    if (!PowerContactTagGetBool(idPower, TAG_POWER_IS_LOBBED) &&
        (wdAimMagSquared < 1.0f || ptPower->tTarget.idACDTarget == ACDID_INVALID))
    {
        // 
        // 
        // 
        WorldDistance wdAimMag = wvAim.Magnitude();
        // 
        wvAim = Vector3D(Math::Cos(ptActor->aYaw), Math::Sin(ptActor->aYaw), 0.0f) * wdAimMag;
    }
    // 
    // 
    // 
    // 
    if (aYawOffset != 0.0f)
    {
        Vector3D::Rotate(cgwvZAxis, aYawOffset, wvAim);
    }
    // 
    // 
    WorldPlace placeTargetToShootAt;
    WorldPlaceOffset(tNavPlaceSource.tPlace, wvAim, placeTargetToShootAt);
    // 
    tTargetToShootAt.InitializeFromPlace(placeTargetToShootAt);
    return TRUE;
}



void SpellEffect_Projectile(const PowerID idPower, void *)
{
    // 
    // 
    // 
    // 
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }
    // 
    Power * ptPower = PowerGet(idPower);
    Target tTargetToShootAt;
    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, -1, -1, NULL);
    // 
    float flMinDist = PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_MIN_TARGET_DISTANCE);
    // 
    float flFallbackDist = (flMinDist > 1e-6f) ? flMinDist : 200.0f;
    // 
    BOOL bSuccess = FALSE;
    // 
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    // 
    // 
    // 
    // 
    if (ptACDOwner && ACDIsPlayer(ptACDOwner->id))
    {
        if (ptPower->tTarget.IsTargetingACD())
        {
            SNO snoAnim = PowerHelper_GetAnimForPowerOwner(idPower);
            Vector3D vContactOffset;
            ptACDOwner->GetContactOffsetForAnim(snoAnim, vContactOffset, 0, 0, FALSE);
            // 
            ACDID idACDTarget = ptPower->tTarget.GetACD();
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (ptACDTarget)
            {
                // 
                tTargetToShootAt.InitializeFromPlace(ptACDTarget->GetPlace());
                tTargetToShootAt.tPlace.wp.z += vContactOffset.z;
                bSuccess = TRUE;
            }
            // 
            else if (ptPower->tTarget.eType == TT_ACTOR_WITH_FALLBACK_POINT)
            {
                WorldPlace wpFallback;
                if (ptPower->tTarget.GetPlace(wpFallback))
                {
                    tTargetToShootAt.InitializeFromPlace(wpFallback);
                }
                bSuccess = TRUE;
            }
        }
        // 
        // 
        // 
        // 
        // 
        else
        {
            // 
            tTargetToShootAt.InitializeFromPlace(ptPower->tTarget.tPlace);
            bSuccess = TRUE;
        }
    }
    // 
    else
    {
        bSuccess = SpellEffect_ProjectileWeaponPower_GetTarget(idPower, tTargetToShootAt, tNavPlaceSource, 0.0f);
        if (flMinDist > 1e-6f && bSuccess)
        {
            // 
            // 
            // 
            WorldPlace placeTarget;
            tTargetToShootAt.GetPlace(placeTarget);
            if (tNavPlaceSource.tPlace.idSWorld != placeTarget.idSWorld ||
                Vector3D::Distance2DSquared(tNavPlaceSource.tPlace.wp, placeTarget.wp) < flMinDist * flMinDist)
            {
                // 
                // 
                // 
                // 
                // 
                flFallbackDist = flMinDist;
                bSuccess = FALSE;
            }
        }
        // 
        if (ptPower->tTarget.eType == TT_ACTOR)
        {
            BOOL bLobbed = PowerContactTagGetBool(idPower, TAG_POWER_IS_LOBBED);
            ACDID idACDTarget = ptPower->tTarget.GetACD();
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (!ptACDTarget || (!ptACDTarget->IsAlive() && !bLobbed))
            {
                bSuccess = FALSE;
            }
            // 
        }
    }
    // 
    if (ptACDOwner && !bSuccess)
    {
        WorldPlace wpTarget = ptACDOwner->GetPlace();
        Angle aYaw = ACDGetYaw(ptACDOwner->id);
        Vector3D vOffset(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
        Vector3D vSourceOffset = tNavPlaceSource.tPlace.wp - wpTarget.wp;
        vSourceOffset.z = 0.0f;
        vOffset *= flFallbackDist + vSourceOffset.Magnitude();
        wpTarget.wp += vOffset;
        tTargetToShootAt.InitializeFromPlace(wpTarget);
    }
    SpellEffect eSpellEffectToRun = PowerGetNextSpellEffect(idPower);
    PowerAPICreateProjectile tProjectile;
    PowerHelper_ProjectileInit(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, tTargetToShootAt, eSpellEffectToRun, tProjectile);
    PowerAPI_DoProjectile(tProjectile);
    if (ptACDOwner)
    {
        int32 nBonusShots = Math::FloatToInt(ptACDOwner->AttributesGetFloat(ATTRIBUTE_PROJECTILE_AMOUNT_BONUS_PERCENT)) +
                            Math::FloatRoundToInt(PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_COUNT)) - 1;
        float flAngle = Math::DegToAngle(PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_SPREAD_ANGLE));
        for (int32 i = 0; i < nBonusShots; i++)
        {
            Angle aYawOffset = flAngle * 0.125f * (i / 2 + 1);
            if (i & 1)
            {
                aYawOffset = Math::NormalizeAngle(-aYawOffset);
            }
            if (SpellEffect_ProjectileWeaponPower_GetTarget(idPower, tTargetToShootAt, tNavPlaceSource, aYawOffset))
            {
                PowerHelper_ProjectileInit(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, tTargetToShootAt, eSpellEffectToRun, tProjectile);
                PowerAPI_DoProjectile(tProjectile);
            }
        }
    }
}


void SpellEffect_GetProjectileImpactData(ACDID idACDProjectile, ACDID idACDIntersected, Target & tImpactTarget)
{
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    ActorCommonData * ptACDIntersected = ACDTryToGet(idACDIntersected);
    if (ptACDIntersected)
    {
        // 
        tImpactTarget.InitializeFromACD(idACDIntersected, FALSE, FALSE);
    }
    else
    {
        tImpactTarget.InitializeFromACD(idACDProjectile, FALSE, FALSE);
    }
}


BOOL SpellEffect_CalculatePierce(PowerID idPower, ACDID idACDProjectile, ACDID idACDIntersected)
{
    // 
    // 
    // 
    if (idACDIntersected == ACDID_INVALID)
    {
        return FALSE;
    }
    // 
    BOOL bRollPierceChance = TRUE;
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    // 
    // 
    float flPierceChance = PowerAttributesGetFloat(idPower, ATTRIBUTE_PIERCE_CHANCE);
    if (flPierceChance < 1e-6f)
    {
        // 
        flPierceChance = PowerGetFormulaValue(idPower, TAG_POWER_PIERCE_CHANCE);
        if (flPierceChance < 1e-6f)
        {
            // 
            if (ptACDProjectile)
            {
                flPierceChance = ptACDProjectile->AttributesGetFloat(ATTRIBUTE_PIERCE_CHANCE);
            }
            if (flPierceChance < 1e-6f)
            {
                bRollPierceChance = FALSE;
            }
        }
    }
    // 
    if (bRollPierceChance)
    {
        // 
        // 
        // 
        // 
        // 
        // 
        float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
        if (flRoll <= flPierceChance)
        {
            return TRUE;
        }
    }
    if (ptACDProjectile)
    {
        int32 nPierceCharges = ptACDProjectile->AttributesGetInt(ATTRIBUTE_PIERCE_CHARGE);
        if (nPierceCharges > 0)
        {
            ptACDProjectile->AttributesSetInt(ATTRIBUTE_PIERCE_CHARGE, nPierceCharges - 1); return TRUE;
        }
    }
    return FALSE;
}



static void sImpactDamage(const PowerID idPower, void * pData)
{
    ImpactData * pImpactData = (ImpactData *)pData;
    Target tImpactTarget;
    SpellEffect_GetProjectileImpactData(pImpactData->idACDProjectile, pImpactData->idACDIntersected, tImpactTarget);
    Power * ptPower = PowerGet(idPower);
    if (ProjectileIsReflected(pImpactData->idACDProjectile))
    {
        ptPower->dwFlags |= (1 << PF_WAS_REFLECTED);
        ptPower->idACDReflectedBy = ProjectileGetReflectedByACD(pImpactData->idACDProjectile);
    }
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tChooseTargets;
    ActorCommonData * ptACDProjectile = ACDTryToGet(pImpactData->idACDProjectile);
    if (!ptACDProjectile)
    {
        return;
    }
    WorldPlace placeExplosion; WorldPlace placeProj = ptACDProjectile->GetPlace();
    PowerHelper_ChooseTargetsInitFromPayload(idPower, placeProj, tImpactTarget, tChooseTargets);
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    PowerAddAffectedACDs(idPower, listTargets, pImpactData->idACDProjectile);
    // 
    // 
    // 
    // 
    if (listTargets.Count() == 0)
    {
        // 
        pImpactData->idACDIntersected = ACDID_INVALID;
    }
    // 
    // 
    // 
    BOOL bExplosionFound = PowerAPI_DoChooseExplosion(tChooseTargets.tGeometry, listTargets, placeExplosion);
    // 
    // 
    // 
    if (pImpactData->idACDIntersected == ACDID_INVALID)
    {
        // 
        // 
        placeExplosion = ACDGet(pImpactData->idACDProjectile).GetPlace();
        // 
        // 
        if (ProjectileIsLobbed(pImpactData->idACDProjectile))
        {
            // 
            placeExplosion.wp.z = Math::Max(placeExplosion.wp.z, placeProj.wp.z);
        }
    }
    else
    {
        // 
        // 
        // 
        // 
        // 
        // 
        // 
        placeProj = ACDGet(pImpactData->idACDProjectile).GetPlace();
        Vector3D vDelta = placeExplosion.wp - placeProj.wp;
        vDelta *= 0.5f;
        placeExplosion.wp.x = placeProj.wp.x + vDelta.x;
        placeExplosion.wp.y = placeProj.wp.y + vDelta.y;
        placeExplosion.wp.z = placeProj.wp.z;
    }
    // 
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionProjectileInit(idPower, placeExplosion, listTargets, pImpactData, tExplosion);
    // 
    BOOL fDoPierce = SpellEffect_CalculatePierce(idPower, pImpactData->idACDProjectile, pImpactData->idACDIntersected);
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    // 
    if (!fDoPierce)
    {
        // 
        tPayload.snoProjectileToStick = PowerGetProjectileStickSNO(idPower);
    }
    // 
    // 
    tPayload.eDeliveryMechanism = PDM_RANGED;
    // 
    // 
    int32 nTargetsHit = PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    // 
    // 
    if (nTargetsHit > 0 ||
        pImpactData->idACDIntersected == ACDID_INVALID)
    {
        // 
        PowerAPI_DoExplosion(tExplosion);
    }
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    BOOL fDestroyProjectile = !fDoPierce;
    if (fDestroyProjectile)
    {
        // 
        PowerAPIDestroyProjectile tProjectile;
        PowerHelper_StopProjectileInit(idPower, pImpactData->idACDProjectile, pImpactData->idACDIntersected, tProjectile);
        PowerAPI_DestroyProjectile(tProjectile);
    }
    else
    {
        // 
        // 
        // 
        // 
        pImpactData->bStop = FALSE;
    }
    // 
    // 
    // 
    ptPower->dwFlags &= ~(1 << PF_WAS_REFLECTED);
}



BOOL SpellEffect_ProjectileTryReflect(const PowerID idPower, ImpactData * ptImpact)
{
    // 
    // 
    ACDID idACDTarget = ptImpact->idACDIntersected;
    if (idACDTarget != ACDID_INVALID)
    {
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    // 
    // 
        if (PowerAPI_BuffApplyReflectEffects(idACDTarget, ptImpact))
        {
            return TRUE;
        }
    // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    // 
        float flHighestChance = 0.0f;
        for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; nDamageType++)
        {
            float flChance = tACDTarget.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_PROJECTILE_REFLECT_CHANCE, nDamageType));
            if (flChance > 1e-6f &&
                tPayload.arCombatDamage[nDamageType].flMin + tPayload.arCombatDamage[nDamageType].flDelta != 0.0f &&
    // 
                flChance > flHighestChance)
            {
                flHighestChance = flChance;
            }
        }
    // 
        BOOL fReflect = TRUE;
        if (flHighestChance < 1e-6f)
        {
            fReflect = FALSE;
        }
        else if (flHighestChance >= 0.999999f)
        {
            fReflect = TRUE;
        }
        else
        {
            float flRand = RandFloat_Internal(SGameGlobalsGet().rng);
            if (flRand <= flHighestChance)
            {
                fReflect = TRUE;
            }
            else
            {
                fReflect = FALSE;
            }
        }
        if (fReflect)
        {
            if (ProjectileReflect(ptImpact->idACDProjectile, idACDTarget, FALSE, 0.0f, TEAM_INVALID, 1.0f))
            {
                ptImpact->bStop = FALSE;
                return TRUE;
            }
        }
    }
    return FALSE;
}




void SpellEffect_ImpactDamage(const PowerID idPower, void * pData)
{
    // 
    ImpactData * ptImpact = (ImpactData *)pData;
    // 
    // 
    // 
    if (SpellEffect_ProjectileTryReflect(idPower, ptImpact))
    {
        // 
        return;
    }
    // 
    sImpactDamage(idPower, pData);
}



static void sSpellEffect_SummonZombie(PowerID idPower, void *)
{
    SNO snoMonster = GlobalSNOGet(GLOBALSNO_MONSTER_ZOMBIE_GHOULSPAWN);
    PowerAPISummonActor tSummonInfo;
    BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonster, ACDID_INVALID, tSummonInfo, TRUE);
    if (bSuccess)
    {
        PowerAPI_SummonActor(tSummonInfo);
        PowerAPIExplosion tExplosion;
        PowerHelper_ExplosionInit(idPower, tSummonInfo.placeSummonLocation, tExplosion);
        PowerAPI_DoExplosion(tExplosion);
        PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    }
}












static int32 sSummonSkeleton(PowerID idPower, int32 nSkeletonsToSummon, BOOL bSearchForSpot, BOOL bDontAddIfLocSearchFails)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return 0;
    }
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return 0;
    }
    if (ptACD->tSNOName.snoGroup != ACTOR_SNO)
    {
        return 0;
    }
    for (int32 i = 0; i < nSkeletonsToSummon; i++)
    {
        BOOL bSuccess;
        PowerAPISummonActor tSummonInfo;
        SNO snoMonsterActor = MonsterGetRandomSummonActor(ptACD->tSNOName, SGameGlobalsGet().rng);
        bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, bSearchForSpot);
        if (!bSuccess)
        {
            return i;
        }
        if (!bSearchForSpot)
        {
            tSummonInfo.qOrientation = ptACD->GetOrientation();
            ptPower->tTarget.GetPlace(tSummonInfo.placeSummonLocation);
            tSummonInfo.dwAddFlags |= (1 << ACTORADD_DONT_PERTURB);
        }
        if (bDontAddIfLocSearchFails)
        {
            tSummonInfo.dwAddFlags |= (1 << ACTORADD_DONT_ADD_IF_LOCATION_SEARCH_FAILS);
        }
        tSummonInfo.dwAddFlags |= (1 << ACTORADD_ADD_AT_SPAWNER_LOCATION_IF_SEARCH_FAILS);
        // 
        ACDID idACDSkeleton = PowerAPI_SummonActor(tSummonInfo);
        if (idACDSkeleton == ACDID_INVALID)
        {
            return i;
        }
        // 
        // 
        ActorCommonData & tACDSkeleton = ACDGet(idACDSkeleton);
        // 
        // 
        tACDSkeleton.InheritGroupsFromACD(*ptACD);
        // 
        // 
        SNO snoRope = PowerContactTagGetSNO(idPower, TAG_POWER_ROPE_SNO);
        // 
        if (snoRope != SNO_NONE)
        {
            // 
            ACDPlayEffectRopeACDToACD(snoRope, ptACD->id, 4, idACDSkeleton, 0, FALSE);
        }
        // 
        // 
        // 
        // 
        // 
        // 
        // 
        SNO snoEffectGroup = PowerContactTagGetSNO(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
        if (snoEffectGroup != SNO_NONE)
        {
            // 
            ACDPlayEffectGroupACDToACD(snoEffectGroup, ptACD->id, idACDSkeleton);
        }
        // 
        // 
        if (PowerGetExplosionSNO(idPower) != SNO_NONE)
        {
            PowerAPIExplosion tExplosion;
            PowerHelper_ExplosionInit(idPower, tACDSkeleton.GetPlace(), tExplosion);
            PowerAPI_DoExplosion(tExplosion);
        }
        // 
        // 
        int32 nSummonDuration = Math::FloatToInt(PowerAPI_GetSummonLifetime(tACDSkeleton, ptPower->snoPower) * 60.0f);
        if (nSummonDuration > 0)
        {
            // 
            tACDSkeleton.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, GameGetServerTime() + nSummonDuration);
        }
        // 
        // 
    }
    // 
    int nStoredCount = ptACD->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
    // 
    ptACD->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nStoredCount + nSkeletonsToSummon);
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    return nSkeletonsToSummon;
}



static void sSpellEffect_SummonSkeleton(PowerID idPower, void *)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = ptPower ? PowerGetOwner(ptPower) : NULL;
    // 
    if (ptPower && ptACDOwner)
    {
        int32 nSkeletonsToSummon = PowerAPI_GetSummonCountPer(*ptACDOwner, ptPower->snoPower);
        sSummonSkeleton(idPower, nSkeletonsToSummon, TRUE, FALSE);
    }
}




static void sSpellEffect_VomitZombie(PowerID idPower, void *)
{
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        // 
        ActorCommonData * ptACDZombie = PowerGetOwner(ptPower);
        if (ptACDZombie)
        {
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            int32 nZombiesToSummon = PowerAPI_GetSummonCountPer(*ptACDZombie, ptPower->snoPower);
            Angle aYaw = ACDGetYaw(ptACDZombie->id);
            WorldPlace placeZombie = ptACDZombie->GetPlace();
            WorldPlace placeTarget;
            placeTarget.idSWorld = placeZombie.idSWorld;
            placeTarget.wp.x = Math::Cos(aYaw);
            placeTarget.wp.y = Math::Sin(aYaw);
            placeTarget.wp.z = 0.0f;
            placeTarget.wp *= 7.0f;
            placeTarget.wp += placeZombie.wp;
            // 
            // 
            NavCellHandle hNavCellCenter;
            NavCellHandleFromWorldPlace(placeZombie, hNavCellCenter);
            // 
            WorldCollParams tCollParams;
            tCollParams.uOneRequiredStaticFlags = NavMeshGetStaticFlagsForPassability(PASSABILITY_PLAYER_AI_OFF);
            tCollParams.uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(PASSABILITY_PLAYER_AI_OFF);
            tCollParams.eTeam = TEAM_NEUTRAL;
            // 
            // 
            WorldCollResult tResult;
            WorldCollSweptPoint(placeZombie.idSWorld,
                                hNavCellCenter,
                                (const Vector2D &)placeZombie.wp,
                                (const Vector2D &)placeTarget.wp,
                                tCollParams,
                                0,
                                0,
                                ptACDZombie->id,
                                tResult);
            // 
            // 
            // 
            // 
            // 
            // 
            placeTarget.wp = tResult.wpEnd;
            BOOL bWantPerturb = FALSE;
            if (!NavMeshPlaceCanEnterForSpawning(placeTarget, PASSABILITY_PLAYER_AI_OFF))
            {
                // 
                // 
                placeTarget.wp.x = placeZombie.wp.x;
                placeTarget.wp.y = placeZombie.wp.y;
                bWantPerturb = TRUE;
            }
            // 
            // 
            ptPower->tTarget.InitializeFromPlace(placeTarget);
            // 
            // 
            if (nZombiesToSummon > 1)
            {
                WorldPlace placeCenter;
                float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(ptACDZombie->id, placeCenter);
                int32 nNumSummoned = ACDCountNearbySummonedUnits(*ptACDZombie, flTargetScanRadius);
                // 
                int32 nMaxSummonedSkeletons = PowerAPI_GetSummonMaxConcurrent(*ptACDZombie, ptPower->snoPower);
                nZombiesToSummon = Math::Min(nZombiesToSummon, nMaxSummonedSkeletons - nNumSummoned);
            }
            int32 nNumSummoned = sSummonSkeleton(idPower, nZombiesToSummon, bWantPerturb, TRUE);
            int32 nNumDidntSummon = nZombiesToSummon - nNumSummoned;
            if (nNumDidntSummon)
            {
                ptPower->tTarget.InitializeFromPlace(ptACDZombie->GetPlace());
                sSummonSkeleton(idPower, nNumDidntSummon, TRUE, TRUE);
            }
        }
    }
}
static void sSpellEffect_ZKBallSummonSkeleton(PowerID idPower, void *)
{


    Power * ptPower = PowerGet(idPower);


    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }


    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, ptPower->tTarget.idACDTarget, ACDID_INVALID);

    int32 nSkeletonsToSummon = PowerAPI_GetSummonCountPer(*ptACD, ptPower->snoPower);

    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
    for (int32 i = 0; i < nSkeletonsToSummon; i++)
    {
        PowerAPISummonActor tSummonInfo;
        BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, TRUE);
        if (!bSuccess)
        {
            return;
        }

        ACDID idACDSkeleton = PowerAPI_SummonActor(tSummonInfo);
        if (idACDSkeleton == ACDID_INVALID)
        {
            return;
        }

        ActorCommonData & tACDSkeleton = ACDGet(idACDSkeleton);
        tACDSkeleton.InheritGroupsFromACD(*ptACD);


        SNO snoRope = PowerContactTagGetSNO(idPower, TAG_POWER_ROPE_SNO);

        if (snoRope != SNO_NONE)
        {
            ACDPlayEffectRopeACDToACD(snoRope,

                ptPower->tTarget.idACDTarget, 0, idACDSkeleton, 0, FALSE);
        }




        SNO snoChainRope = PowerContactTagGetSNO(idPower, TAG_POWER_ROPE_CHAIN_SNO);
        if (snoChainRope != SNO_NONE)
        {
            ACDPlayEffectRopeACDToACD(snoChainRope,

                ptACD->id, 0,

                ptPower->tTarget.idACDTarget, 0, FALSE);
        }


        SNO snoEffectGroup = PowerContactTagGetSNO(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
        if (snoEffectGroup != SNO_NONE)
        {
            ACDPlayEffectGroupACDToACD(snoEffectGroup, ptACD->id, idACDSkeleton);
        }

        if (PowerGetExplosionSNO(idPower) != SNO_NONE)
        {
            PowerAPIExplosion tExplosion;
            PowerHelper_ExplosionInit(idPower, tACDSkeleton.GetPlace(), tExplosion);
            PowerAPI_DoExplosion(tExplosion);
        }

        int32 nSummonDuration = Math::FloatToInt(PowerAPI_GetSummonLifetime(*ptACD, ptPower->snoPower) * 60.0f);
        if (nSummonDuration > 0)
        {
            tACDSkeleton.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, GameGetServerTime() + nSummonDuration);
        }
    }
}





static void sSpellEffect_SummonPaintingSkeleton(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    int32 nSkeletonsToSummon = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SKELETON_SUMMON_COUNT));
    int32 nSummoned = sSummonSkeleton(idPower, nSkeletonsToSummon, TRUE, FALSE);
    int32 nRemaining = nSkeletonsToSummon - nSummoned;
    if (nRemaining > 0)
    {
        // 
        ActorCommonData * ptACDPainting = PowerGetOwner(ptPower);
        if (ptACDPainting)
        {
            // 
            ActorMovement * ptMove = ptACDPainting->GetActorMovement();
            Angle aYaw = ACDGetYaw(ptACDPainting->id);
            WorldPlace placePainting = ptACDPainting->GetPlace();
            WorldPlace placeTarget;
            placeTarget.idSWorld = placePainting.idSWorld;
            placeTarget.wp.x = Math::Cos(aYaw);
            placeTarget.wp.y = Math::Sin(aYaw);
            placeTarget.wp.z = 0.0f;
            // 
            placeTarget.wp *= 5.0f;
            placeTarget.wp += placePainting.wp;
            // 
            ptMove->SnapPlaceToGround(placeTarget);
            ptPower->tTarget.InitializeFromPlace(placeTarget);
            // 
            sSummonSkeleton(idPower, nRemaining, TRUE, FALSE);
            // 
            // 
            int32 nStoredCount = ptACDPainting->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
            // 
            ptACDPainting->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, Math::Min(nStoredCount, 1));
        }
    }
}



void sSkeltonKing_WhirlwindCleanup(const PowerID idPower, void *)
{
    PowerForceStop(idPower, TRUE);
}






void sSkeletonKing_WhirlwindBuffRemoved(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    if (ptBuff)
    {
        Power * ptPower = PowerTryToGet(ptBuff->idPower);
        ActorCommonData * ptACDOwner = PowerGetOwner(ptBuff->idPower);
        ActorID idActor = ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID;
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor && ptActor->ptSActorMovement)
        {
            ptActor->ptSActorMovement->StopNow();
        }
        AnimPlayerPlayNextAnimation(*ptACDOwner);
        AnimationTiming tAnimationTiming;
        AnimPlayerGetAnimationTiming(*ptACDOwner, ptPower->dwAnimationTag, ANIM_USE_ANIM_LENGTH, 1.0f, tAnimationTiming);
        uint32 dwCooldownAnimDuration = tAnimationTiming.tAnimSpec[ANIMOFFSET_END].dwDuration;
        PowerTriggerAddTimerTrigger(ptPower->id, dwCooldownAnimDuration, sSkeltonKing_WhirlwindCleanup, NULL);
    }
}


// X/Common/Networking/GameMessage.h (DWARF names; kept local, as FloatingNumberMessage above)












void sSpellEffect_SkeletonKing_WhirlwindEnd(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, PowerGetOwner(ptPower), ACDID_INVALID);
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}





static void sSkeletonKing_Whirlwind_SteeringUpdate(PowerID idPower, void * pData)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);



    if (!ptACDOwner || !ptACDOwner->IsAlive())
    {
        return;
    }


    BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, ptACDOwner, ACDID_INVALID);
    if (idBuff == BUFFID_INVALID)
    {
        return;
    }

    ActorID idActor = (ActorID)ptACDOwner->idOwner;
    Actor * ptActor = ActorGet(idActor);



    PowerID idPrimaryPower = ActorPowersGetPrimary(idActor);
    if (idPrimaryPower != idPower)
    {
        sSpellEffect_SkeletonKing_WhirlwindEnd(idPower, NULL);
        return;
    }


    Target tTarget;
    PowerGetTarget(idPower, tTarget);
    if (tTarget.IsInvalid())
    {
        sSpellEffect_SkeletonKing_WhirlwindEnd(idPower, NULL);
        return;
    }

    if (ptActor && ptActor->ptSActorMovement)
    {
        BOOL bFirstContactFrame = (BOOL)(intptr)pData;

        WorldPlace myPlace = ptACDOwner->GetPlace();
        WorldPlace placeTarget;
        tTarget.GetPlace(placeTarget);



        if (myPlace.idSWorld != placeTarget.idSWorld)
        {
            if (bFirstContactFrame)
            {
                sSpellEffect_SkeletonKing_WhirlwindEnd(idPower, NULL);
            }
            return;
        }

        Vector3D meToTarget = placeTarget.wp - myPlace.wp;
        float flDistToTarget = meToTarget.Normalize();

        Vector3D myFacing = ptACDOwner->GetForward();
        float flDotProduct = Vector3D::DotProduct(meToTarget, myFacing);


        if (tTarget.eType == TT_ACTOR)
        {
            if ((flDistToTarget < 3.0f || flDotProduct < -0.5f) && !bFirstContactFrame)
            {
                return;
            }
        }
        else if (tTarget.eType == TT_POINT)
        {










            if (flDistToTarget < 0.5f || flDotProduct < 0.0f)
            {
                if (bFirstContactFrame)
                {


                    sSpellEffect_SkeletonKing_WhirlwindEnd(idPower, NULL);
                }
                return;
            }
        }



        float flDist = Math::Max(flDistToTarget, 30.0f);
        placeTarget.wp = myPlace.wp + meToTarget * flDist;

        uint32 uMoveFlags = (1 << MOVEBIT_AUTO_UPDATE_YAW) | (1 << MOVEBIT_FROM_POWER) | (1 << MOVEBIT_DONT_AUTOPLAY_IDLE) | (1 << MOVEBIT_ACTIVATE_TRIGGERS);

        float wdSpeed = ptPower->flParam;
        ptActor->ptSActorMovement->MoveTo(placeTarget, wdSpeed, 0.0f, (ActorAnimationTag)-1, uMoveFlags, 0.0f, ACDID_INVALID, ptPower->snoPower, 0, -1, 0.0f, NULL);



        if (bFirstContactFrame)
        {
            AnimationTiming tAnimationTiming;
            AnimPlayerGetAnimationTiming(*ptACDOwner, ptPower->dwAnimationTag, ANIM_USE_ANIM_LENGTH, 1.0f, tAnimationTiming);
            int32 nTickDelay = tAnimationTiming.tAnimSpec[ANIMOFFSET_CONTINUE].dwDuration + 1;
            PowerTriggerAddTimerTrigger(idPower, nTickDelay, sSkeletonKing_Whirlwind_SteeringUpdate, (void *)TRUE);
        }
    }
}



void sSpellEffect_sSkeletonKing_WhirlwindAttack(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }


    BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, ptACD, ACDID_INVALID);
    if (idBuff == BUFFID_INVALID)
    {
        return;
    }


    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());

    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);


    PowerAPI_DoChooseTargets(tTargets, listTargets);
    {
        POS pos = listTargets.GetTailPosition();
        while (pos)
        {
            POS oldPos = pos;
            const ACDID idACDNewTarget = (ACDID)listTargets.GetPrev(pos);
            if (idACDNewTarget == ptACD->id)
            {
                listTargets.RemoveAt(oldPos);
            }
        }


        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);

        tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
        tPayload.wdKnockbackMagnitude = PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_KNOCKBACK_MAGNITUDE);
        tPayload.eDeliveryMechanism = PDM_MELEE;

        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    }

    ActorID idActor = (ActorID)ptACD->idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor && ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->StopAndSyncPosition(TRUE, FALSE);
    }



    AnimationTiming tAnimationTiming;
    AnimPlayerGetAnimationTiming(*ptACD, ptPower->dwAnimationTag, ANIM_USE_ANIM_LENGTH, 1.0f, tAnimationTiming);
    int32 nTickDelay = tAnimationTiming.tAnimSpec[ANIMOFFSET_CONTINUE].dwDuration + 1;
    PowerTriggerAddTimerTrigger(idPower, nTickDelay, sSpellEffect_sSkeletonKing_WhirlwindAttack, NULL);


    sSkeletonKing_Whirlwind_SteeringUpdate(idPower, NULL);
}



void sSpellEffect_SkeletonKing_WhirlwindBegin(PowerID idPower, void * pData)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    ActorID idActor = (ActorID)ptACD->idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        // 
        return;
    }
    // 
    // 
    // 
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    // 
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.pRemovalEffect = sSkeletonKing_WhirlwindBuffRemoved;
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_DURATION_MIN) * 60.0f);
    tBuffStartData.dwDurationDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_DURATION_DELTA) * 60.0f);
    // 
    // 
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_STUN_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ROOT_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CHARM_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_BLIND_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_SLOWDOWN_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FREEZE_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    // 
    WorldDistance wdSpeed = PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_MOVE_SPEED);
    ptPower->flParam = wdSpeed;
}




static void sSpellEffect_SkeletonKing_WhirlwindMove(PowerID idPower, void *)
{




    sSkeletonKing_Whirlwind_SteeringUpdate(idPower, (void *)TRUE);
}




static void sSpellEffect_SummonFallen_OnSpawn(PowerID idPower, void *)
{
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
    int32 nNumToSpawn = (int32)PowerGetFormulaValue(idPower, TAG_POWER_FALLEN_MAX_SUMMON_COUNT);
    // 
    for (int32 i = 0; i < nNumToSpawn; i++)
    {
        // 
        PowerAPISummonActor tSummonInfo;
        BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, TRUE);
        // 
        if (!bSuccess)
        {
            // 
            // 
            return;
        }
        ACDID idACDSummoned = PowerAPI_SummonActor(tSummonInfo);
        // 
        if (idACDSummoned == ACDID_INVALID)
        {
            // 
            // 
            return;
        }
        // 
        ActorCommonData & tACDSummoned = ACDGet(idACDSummoned);
        tACDSummoned.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDSummoned.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
        tACDSummoned.InheritGroupsFromACD(*ptACD);
    }
}




static void sSpellEffect_SummonFallen(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);

    PowerAPISummonActor tSummonInfo;
    BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, TRUE);
    if (!bSuccess)
    {
        return;
    }


    ACDID idACDSummoned = PowerAPI_SummonActor(tSummonInfo);

    if (idACDSummoned == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDSummoned = ACDGet(idACDSummoned);
    tACDSummoned.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDSummoned.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    tACDSummoned.InheritGroupsFromACD(*ptACD);

    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, 0);

        RemoveRagdollMessage tMsg;
        tMsg.annTarget = ANNFromACD(ptACD->id);
        tMsg.snoMonster = snoMonsterActor;
        MessageSendToClient(listDestPlayers, SMSG_REMOVE_NEAREST_RAGDOLL, &tMsg, sizeof(tMsg));
    }

    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
}



static void sSpellEffect_SummonZombieCrawler(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
    if (snoMonsterActor == SNO_NONE)
    {
        return;
    }
    PowerAPISummonActor tSummonInfo;
    BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, FALSE);
    // 
    if (!bSuccess)
    {
        // 
        // 
        // 
        return;
    }
    tSummonInfo.nActorLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
    // 
    // 
    tSummonInfo.qOrientation = ptACD->GetOrientation();
    tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    // 
    NavPlace tSpawnNavPlace;
    if (ZombieCrawlerGetSpawnNavPlace(ptACD->id, tSpawnNavPlace))
    {
        // 
        // 
        // 
        // 
        tSummonInfo.placeSummonLocation = tSpawnNavPlace.tPlace;
        // 
        ACDID idACDZombie = PowerAPI_SummonActor(tSummonInfo);
        // 
        if (idACDZombie != ACDID_INVALID)
        {
            // 
            ActorCommonData & tACDZombie = ACDGet(idACDZombie);
            // 
            tACDZombie.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
            tACDZombie.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
            tACDZombie.InheritGroupsFromACD(*ptACD);
        }
        // 
        // 
        PowerAPIExplosion tExplosion;
        PowerHelper_ExplosionInit(idPower, tSummonInfo.placeSummonLocation, tExplosion);
        PowerAPI_DoExplosion(tExplosion);
        // 
        // 
        PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    }
}






void sSpellEffect_UniqueMonsterGenericSummon_BuffRemoved(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    ActorCommonData * ptACDOwner = ptPower ? PowerGetOwner(ptPower) : NULL;
    if (!ptACDOwner)
    {
        return;
    }
    XPooledDataIDList listSummons(MemoryPoolsGetDATAIDListPool());
    ACDGetNearbySummonedUnits(*ptACDOwner, 60.0f, listSummons);
    // 
    POS pos = listSummons.GetHeadPosition();
    while (pos)
    {
        ACDID idACDSummon = (ACDID)listSummons.GetNext(pos);
        // 
        // 
        // 
        // 
        // 
        // 
        ActorCommonData * ptACDSummon = ACDTryToGet(idACDSummon);
        if (ptACDSummon)
        {
            BrainAPIDoScriptedDeath(*ptACDSummon, 0, 0, 0, ACTORID_INVALID);
        }
    }
}

void sSpellEffect_UniqueMonsterGenericSummon_DelayedSpawnOnRemoval(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptPower || !ptACD)
    {
        return;
    }
    // 
    // 
    int32 nMonsterSummonTagsIndex = (int32)PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_MONSTER_GENERIC_SUMMON_TAGS_INDEX);
    int32 nTagOffset = nMonsterSummonTagsIndex * (TAG_MONSTER_GENERIC_SUMMON2_SPAWN_ANIM - TAG_MONSTER_GENERIC_SUMMON_SPAWN_ANIM);
    // 
    // 
    SNO snoSpawnEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_UNIQUE_MONSTER_GENERIC_SUMMON_SPAWN);
    if (snoSpawnEffect != SNO_NONE)
    {
        // 
        ACDPlayEffect(ptACD->id, EFFECT_PLAY_EFFECT_GROUP, snoSpawnEffect, ACDID_INVALID);
    }
    // 
    // 
    // 
    int32 nSpawnAnimTag = (int32)PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_SPAWN_ANIM + nTagOffset);
    if (nSpawnAnimTag != -1)
    {
        // 
        AnimPlayerPlayAnimation(*ptACD, nSpawnAnimTag, (uint32)-1, TRUE, ANIM_REASON_SPAWN, 0.0f, FALSE, 1.0f);
    }
}
CRefString PowerGetMonsterSNONameFormulaValue(Power * ptPower, uint32 dwMonsterTag); // No header declaration recovered; DWARF records this overload only in Powers.cpp:5307.
static void sSpellEffect_UniqueMonsterGenericSummon(
    PowerID idPower,
    void *)
{

    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = ptPower ? PowerGetOwner(ptPower) : NULL;
    if (!ptACDOwner)
    {
        return;
    }


    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }


    int32 nMonsterSummonTagsIndex = (int32)PowerGetFormulaValue(idPower, TAG_POWER_MONSTER_GENERIC_SUMMON_TAGS_INDEX);
    int32 nTagOffset = nMonsterSummonTagsIndex * (TAG_MONSTER_GENERIC_SUMMON2_MONSTER_TYPE - TAG_MONSTER_GENERIC_SUMMON_MONSTER_TYPE);


    CRefString szMonsterType = PowerGetMonsterSNONameFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_MONSTER_TYPE + nTagOffset);
    SNO snoMonsterType = SNOFileLookupGUID(ACTOR_SNO, szMonsterType.str());

    CRefString szRopeEffect = PowerGetMonsterSNONameFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_ROPE_EFFECT + nTagOffset);
    SNO snoRopeEffect = SNOFileLookupGUID(ROPE_SNO, szRopeEffect.str());


    SNO snoAttractorEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_UNIQUE_MONSTER_GENERIC_SUMMON_ATTRACTOR);


    SNO snoSpawnEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_UNIQUE_MONSTER_GENERIC_SUMMON_SPAWN);

    float flSummonDuration = PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_DURATION + nTagOffset);
    int32 nSummonDuration = Math::FloatToInt(flSummonDuration * TICKS_PER_SECOND);

    int32 nSpawnAnimTag = (int32)PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_SPAWN_ANIM + nTagOffset);
    float flSpawnDelay = 0.4f;


    int32 nSummonCount = (int32)PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_MIN_COUNT + nTagOffset);
    int32 nSummonDelta = (int32)PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_DELTA_COUNT + nTagOffset);
    nSummonCount = RandRange_Internal(SGameGlobalsGet().rng, nSummonCount, nSummonDelta);


    int nTotalSummonCount = ptACDOwner->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
    int32 nMaxTotalSummonCount = (int32)PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_MAX_TOTAL_SUMMONS + nTagOffset);
    if (nMaxTotalSummonCount > 0 && nTotalSummonCount + nSummonCount > nMaxTotalSummonCount)
    {
        nSummonCount = nMaxTotalSummonCount - nTotalSummonCount;
    }


    int32 nMaxConcurrentSummonCount = (int32)PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_MAX_CONCURRENT_SUMMONS + nTagOffset);
    if (nMaxConcurrentSummonCount > 0)
    {
        WorldPlace wpOwnerPlace = ptACDOwner->GetPlace();
        float flAwarenessRadius = BrainAPIActorGetAwarenessScanRadius(ptACDOwner->id, wpOwnerPlace);
        int32 nExistingMinions = ACDCountNearbySummonedUnits(*ptACDOwner, flAwarenessRadius);
        if (nExistingMinions + nSummonCount > nMaxConcurrentSummonCount)
        {
            nSummonCount = nMaxConcurrentSummonCount - nExistingMinions;
        }
    }


    PowerAPIBuffStartData tBuffDelayedSpawn(idPower);
    tBuffDelayedSpawn.idACDSource = ptACDOwner->id;
    tBuffDelayedSpawn.bInfinite = false;
    tBuffDelayedSpawn.dwDurationMin = Math::FloatToInt(flSpawnDelay * TICKS_PER_SECOND);
    tBuffDelayedSpawn.dwDurationDelta = 0;
    tBuffDelayedSpawn.bPositiveEffect = true;
    tBuffDelayedSpawn.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffDelayedSpawn.nBuffIndex = 1;
    tBuffDelayedSpawn.pRemovalEffect = sSpellEffect_UniqueMonsterGenericSummon_DelayedSpawnOnRemoval;
    FastAttribSetValueInt(tBuffDelayedSpawn.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueInt(tBuffDelayedSpawn.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffDelayedSpawn.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
    FastAttribSetValueInt(tBuffDelayedSpawn.idFastAttrib, ATTRIBUTE_DISABLED, TRUE);


    int nSummonedMinions = 0;
    for (; nSummonedMinions < nSummonCount; nSummonedMinions++)
    {

        PowerAPISummonActor tSummonInfo;
        if (!PowerHelper_SummonActorInit(idPower, snoMonsterType, ACDID_INVALID, tSummonInfo, TRUE))
        {
            break;
        }

        if (nSpawnAnimTag != -1)
        {
            tSummonInfo.dwAnimTag = nSpawnAnimTag;
        }

        tSummonInfo.dwAddFlags |= (1 << ACTORADD_DONT_ADD_IF_LOCATION_SEARCH_FAILS);
        tSummonInfo.dwAddFlags |= (1 << ACTORADD_ADD_AT_SPAWNER_LOCATION_IF_SEARCH_FAILS);


        ACDID idACDMinion = PowerAPI_SummonActor(tSummonInfo);
        if (idACDMinion == ACDID_INVALID)
        {
            break;
        }


        ActorCommonData & tACDMinion = ACDGet(idACDMinion);
        tACDMinion.InheritGroupsFromACD(*ptACDOwner);


        tACDMinion.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDMinion.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);


        if (snoRopeEffect != SNO_NONE)
        {
            ACDPlayEffectRopeACDToACD(snoRopeEffect,

                ptACDOwner->id, ROPE_POSITION_RIGHT_HAND, idACDMinion, ROPE_POSITION_DEFAULT, FALSE);
        }





        if (snoAttractorEffect != SNO_NONE)
        {
            SComplexEffectAdd(*ptACDOwner, snoAttractorEffect, idACDMinion, COMPLEXEFFECT_TYPE_NORMAL, 0, 0, TRUE);
        }




        PowerAPI_BuffAddToACD(tBuffDelayedSpawn, tACDMinion);








        if (nSummonDuration > 0)
        {
            tACDMinion.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, GameGetServerTime() + nSummonDuration);
        }
    }


    nTotalSummonCount += nSummonedMinions;
    ptACDOwner->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nTotalSummonCount);


    BOOL bKillSummonsOnDeath = (int32)PowerGetMonsterFormulaValue(ptPower, TAG_MONSTER_GENERIC_SUMMON_KILL_SUMMONS_ON_DEATH + nTagOffset);
    if (bKillSummonsOnDeath && PowerAPI_BuffFindOnACD(ptPower->snoPower, 0, *ptACDOwner, ACDID_INVALID, FALSE) == BUFFID_INVALID)
    {
        PowerAPIBuffStartData tBuff(idPower);
        tBuff.idACDSource = ptACDOwner->id;
        tBuff.bInfinite = true;
        tBuff.bPositiveEffect = true;
        tBuff.eStackType = BUFF_STACKTYPE_NEVER;
        tBuff.nBuffIndex = 0;
        tBuff.pRemovalEffect = sSpellEffect_UniqueMonsterGenericSummon_BuffRemoved;
        PowerAPI_BuffAddToACD(tBuff, *ptACDOwner);
    }
}




static void sSpellEffect_SummonTriuneDemon(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);

    const int32 nDemonsToSummon = 1;

    int32 nCount = 0;
    for (int32 i = 0; i < nDemonsToSummon; i++)
    {
        PowerAPISummonActor tSummonInfo;
        BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, TRUE);
        if (!bSuccess)
        {
            return;
        }

        WorldPlace wpSummonActorTarget(tSummonInfo.placeSummonLocation);
        PowerComputeDestination(ptPower->snoPower, *ptACD->GetActorMovement(), wpSummonActorTarget, tSummonInfo.placeSummonLocation, FALSE, 10.0f);

        ACDID idACDDemon = PowerAPI_SummonActor(tSummonInfo);

        if (idACDDemon == ACDID_INVALID)
        {
            return;
        }




        ActorCommonData & tACDDemon = ACDGet(idACDDemon);
        tACDDemon.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDDemon.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
        tACDDemon.InheritGroupsFromACD(*ptACD);

        int32 nSummonDuration = Math::FloatToInt(PowerGetFormulaValue(tSummonInfo.idPower, TAG_POWER_SUMMONED_ACTOR_LIFE_DURATION) * 60.0f);
        tACDDemon.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, GameGetServerTime() + nSummonDuration);


        SNO snoRope = PowerEffectTagGetSNO(idPower, TAG_POWER_ROPE_SNO);
        if (snoRope != SNO_NONE)
        {
            ACDPlayEffectRopeACDToACD(snoRope, ptACD->id, 4, tACDDemon.id, 0, FALSE);
        }




        nCount++;
    }

    int nStoredCount = ptACD->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
    nStoredCount += nCount;
    ptACD->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nStoredCount);

    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
}



static void sSpellEffect_PerformStandardDamage(PowerID idPower, void *)
{

    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);

    PowerHelper_DoFirstTimeCosts(idPower, TRUE);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_RANGED;
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}











static void sSpellEffect_MonsterTeleport(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);



    if (ptPower->tTarget.eType == TT_POINT)
    {

        ActorCommonData * ptACD = PowerGetOwner(ptPower);
        if (ptACD)
        {
            ActorID idActorOwner = (ActorID)ptACD->idOwner;


            ActorSnapToPosition(idActorOwner, ptPower->tTarget.tPlace, TRUE, TRUE);
        }
    }
    else
    {



        DisplayInternalError(NULL, "", 2850, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




static void sSpellEffect_Knockback(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    const ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }
    // 
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
    // 
    float flRange = PowerGetPayloadParam0(idPower);
    const ActorCommonData * ptACDTarget = ACDTryToGet(ptPower->tTarget.idACDTarget);
    if (!ptACDTarget)
    {
        // 
        return;
    }
    // 
    // 
    float flRangeSq = flRange * flRange;
    float flDistSq = Vector3D::Distance2DSquared(ptACDTarget->GetPlace().wp, ptACDOwner->GetPlace().wp);
    if (flRangeSq < FLT_EPSILON || flRangeSq > flDistSq)
    {
        // 
        // 
        // 
        PowerAPI_DoPayload(tPayload, ptPower->tTarget.idACDTarget, ACDID_INVALID);
    }
}



static void sSpellEffect_KnockbackStart(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IN_KNOCKBACK, 1);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}





static void sSpellEffect_KnockbackStop(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, *ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }
}














static void sSpellEffect_Stun(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, ptPower->tTarget.idACDTarget, ACDID_INVALID);
}












static void sSpellEffect_Charged_Bolt_Spawn_Arc(const PowerID idPower, Angle aAngCenter, Angle aAngDelta, int32 nNumBolts)
{
    // 
    // 
    // 
    // 
    Power * ptPower = PowerGet(idPower);
    // 
    // 
    // 
    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptPower->idACD, tNavPlaceSource, -1, -1, NULL);
    // 
    const Angle aAngBetweenMissiles = 2.1f / nNumBolts;
    const Angle aAngVariance = (aAngBetweenMissiles + aAngBetweenMissiles + aAngBetweenMissiles) / 4.0f;
    // 
    // 
    Angle aAngleOffset = (nNumBolts & 1) ? 0.0f :
        // 
        aAngBetweenMissiles * 0.5f;
    // 
    // 
    for (int32 i = 0; i < nNumBolts; i++)
    {
        Angle aYaw = aAngleOffset;
        if (i & 1)
        {
            aYaw = -aAngleOffset;
        }
        else if (i != 0)
        {
            // 
            aAngleOffset += aAngBetweenMissiles;
        }
        // 
        aYaw += aAngCenter;
        aYaw = Math::NormalizeAngle(aYaw + RandRangeFloat_Internal(SGameGlobalsGet().rng, -aAngVariance * 0.5f, aAngVariance));
        // 
        // 
        // 
        Vector3D vTargetOffset;
        vTargetOffset.x = Math::Cos(aYaw);
        vTargetOffset.y = Math::Sin(aYaw);
        vTargetOffset.z = 0.0f;
        // 
        Target tTarget;
        tTarget.eType = TT_POINT;
        WorldPlaceOffset(tNavPlaceSource.tPlace, vTargetOffset, tTarget.tPlace);
        // 
        // 
        SpellEffect eSpellEffectToRun = PowerGetNextSpellEffect(idPower);
        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInit(idPower, ACDID_INVALID, tNavPlaceSource, tTarget, eSpellEffectToRun, tProjectile);
        tProjectile.tProjectileData.tAnimationData.wdSpeed = PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_SPEED);
        tProjectile.tProjectileData.eDPath = DPATH_CHARGED_BOLT;
        PowerAPI_DoProjectile(tProjectile);
    }
}




static void sSpellEffect_Charged_Bolt(const PowerID idPower, void *)
{
    // 
    // 
    int32 nNumMissiles = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_CHARGED_BOLT_NUM_BOLTS));
    // 
    // 
    int32 nMissilesRight = nNumMissiles >> 1;
    int32 nMissilesLeft = nMissilesRight;
    // 
    // 
    if (nNumMissiles & 1)
    {
        // 
        if (Rand_Internal(SGameGlobalsGet().rng, 1) == 0)
        {
            nMissilesRight++;
        }
        else
        {
            nMissilesLeft++;
        }
    }
    // 
    // 
    // 
    sSpellEffect_Charged_Bolt_Spawn_Arc(idPower, 5.49778748f, 0.866025f, nMissilesLeft);
    // 
    // 
    // 
    // 
    // 
    sSpellEffect_Charged_Bolt_Spawn_Arc(idPower, 8.63938046f, 0.866025f, nMissilesRight);
}





static void sSpellEffect_ImpactDamageRunChild(PowerID idPower, void * pData)
{
    // 
    // 
    ImpactData * pImpactData = (ImpactData *)pData;
    // 
    // 
    // 
    // 
    // 
    ACDID idACDIntersected = pImpactData->idACDIntersected;
    // 
    // 
    SpellEffect_ImpactDamage(idPower, pData);
    // 
    // 
    Power * ptPower = PowerGet(idPower);
    // 
    // 
    if (ProjectileIsReflected(pImpactData->idACDProjectile))
    {
        ptPower->dwFlags |= (1 << PF_WAS_REFLECTED);
        ptPower->idACDReflectedBy = ProjectileGetReflectedByACD(pImpactData->idACDProjectile);
    }
    // 
    // 
    ACDID idACDIntersectedNew = pImpactData->idACDIntersected;
    // 
    if (idACDIntersectedNew != idACDIntersected)
    {
        pImpactData->idACDIntersected = idACDIntersected;
    }
    // 
    // 
    PowerRunChild(idPower, pData);
    // 
    if (pImpactData->idACDIntersected != idACDIntersectedNew)
    {
        pImpactData->idACDIntersected = idACDIntersectedNew;
    }
    if (ProjectileIsReflected(pImpactData->idACDProjectile))
    {
        ptPower->dwFlags &= ~(1 << PF_WAS_REFLECTED);
    }
}



static void sSpellEffect_RunChild(PowerID idPower, void * pData)
{
    PowerRunChild(idPower, pData);
}





static void sSpellEffect_TownPortal(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD && ACDIsPlayer(ptACD->id) == TRUE)
    {
        ACDID idACDTPScroll = ACDID_INVALID;
        int32 nCount = ActorInventoryCountItemsOfTypeOnPlayer(ptACD->id, GlobalGBIDGet(GLOBALGBID_ITEM_TOWNPORTALSTONE), &idACDTPScroll);
        if (idACDTPScroll != ACDID_INVALID)
        {
            PowerStartUseItem(ptACD->id, idACDTPScroll, NULL);
        }
    }
}


















static void sSpellEffect_SwipeDoDamage(PowerID idPower, void * pData)
{
    ACDID idACDTarget = (ACDID)(intptr)pData;
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (idACDTarget == ACDID_INVALID || !ptACDOwner)
    {
        return;
    }
    // 
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        // 
        return;
    }
    // 
    // 
    // 
    if (!PowerCanTarget(idPower, idACDTarget, ACDID_INVALID))
    {
        // 
        return;
    }
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
    tPayload.wdKnockbackMagnitude = 16.0f;
    // 
    // 
    PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
}







static void sSpellEffect_Swipe(PowerID idPower, void * ptData, BOOL bIsRightSideSwipe)
{
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, ptACD->GetPlace(), tExplosion);
    PowerAPI_DoExplosion(tExplosion);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    Power * ptPower = PowerGet(idPower);
    NavPlace tNavPlace;
    if (!ptPower->tTarget.GetNavPlace(tNavPlace))
    {
        return;
    }
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    Angle aAttackAngle = Math::DegToAngle((float)tTargets.tGeometry.tPie.nDegrees);
    Angle aRotate = aAttackAngle * 0.5f;
    if (bIsRightSideSwipe)
    {
        aRotate = Math::NormalizeAngle(-aRotate);
    }
    // 
    // 
    // 
    // 
    Vector3D wvAttackCenter;
    Vector3D wvFacing;
    WorldPlace placeDest;
    ptPower->tTarget.GetPlace(placeDest);
    WorldPlaceGetVectorBetween(tTargets.tGeometry.tPie.placeCenter, placeDest, wvAttackCenter);
    WorldPlaceGetVectorBetween(tTargets.tGeometry.tPie.placeCenter, placeDest, wvFacing);
    Vector3D::Rotate(cgwvZAxis, aRotate, wvAttackCenter);
    tTargets.tGeometry.tPie.wvPieVector = wvAttackCenter;
    PowerAPI_DoChooseTargets(tTargets, listTargets);
    // 
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        // 
        POS posLast = pos;
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        // 
        if (idACDTarget == ptACD->id)
        {
            listTargets.RemoveAt(posLast);
        }
    }
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        // 
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        WorldPlace wpTarget = ACDGet(idACDTarget).GetPlace();
        // 
        // 
        // 
        Vector3D wvToTarget;
        WorldPlaceGetVectorBetween(tTargets.tGeometry.tPie.placeCenter, wpTarget, wvToTarget);
        // 
        Angle aDifference = Vector3D::AngleBetween(wvFacing, wvToTarget);
        // 
        // 
        // 
        if (bIsRightSideSwipe)
        {
            // 
            aDifference = Math::NormalizeAngle(-aDifference);
        }
        // 
        // 
        // 
        if (aDifference > aAttackAngle)
        {
            // 
            // 
            // 
            // 
            if (aDifference > PI)
            {
                aDifference = 0.0f;
            }
            else
            {
                aDifference = aAttackAngle;
            }
        }
        // 
        // 
        // 
        float flDelay = aDifference / aAttackAngle;
        int32 nDelay = Math::FloatToInt(flDelay * 15.0f + 0.5f);
        // 
        // 
        // 
        // 
        // 
        // 
        if (nDelay <= 1)
        {
            sSpellEffect_SwipeDoDamage(idPower, (void *)(intptr)idACDTarget);
        }
        else
        {
            // 
            // 
            PowerTriggerAddTimerTrigger(idPower, nDelay, sSpellEffect_SwipeDoDamage, (void *)(intptr)idACDTarget);
        }
    }
    // 
    // 
}



static void sSpellEffect_SwipeLeft(PowerID idPower, void * ptData)
{
    // 
    sSpellEffect_Swipe(idPower, ptData, FALSE);
}





static void sSpellEffect_SwipeRight(PowerID idPower, void * ptData)
{
    // 
    sSpellEffect_Swipe(idPower, ptData, TRUE);
}





static void sSpellEffect_SiegebreakerDemon_Grab(PowerID idPower, void * ptData)
{
    // 
    // 
    // 
    // 
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }
    // 
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    ACDID idACDTargetSwungAt = PowerGetTargetACD(idPower);
    // 
    if (idACDTargetSwungAt != ACDID_INVALID)
    {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTargetSwungAt);
    // 
    // 
    // 
        if (ptACDTarget && !ptACDOwner->HasAttachedACD() && ptACDTarget->IsTargetable() && !ptACDTarget->AttributesGetInt(ATTRIBUTE_INVULNERABLE))
        {
            SNO snoCurrentPower = ActorPowersGetPrimaryPower((ActorID)ptACDTarget->idOwner);
            if (snoCurrentPower != SNO_NONE && PowerIsChannelled(*ptACDTarget, AutoPowerDefinitionR(snoCurrentPower)))
            {
                ActorInterruptPrimaryPower(idACDTargetSwungAt);
            }
    // 
            ActorCommonData & tACDTargetSwungAt = ACDGet(idACDTargetSwungAt);
            ptACDOwner->AttachACD(tACDTargetSwungAt);
        }
    }
}






static void sSpellEffect_SiegebreakerDemon_GrabToBite(PowerID idPower, void *)
{
    // 
    // 
    // 
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    // 
    ACDID idACDTargetSwungAt = PowerGetTargetACD(idPower);
    // 
    if (ptACDOwner && idACDTargetSwungAt != ACDID_INVALID)
    {
    // 
    // 
    // 
        SNO snoActor = GlobalSNOGet(GLOBALSNO_MONSTER_WIZARD_MIRROR_IMAGE);
        ActorCommonData & tACDTarget = ACDGet(idACDTargetSwungAt);
        NavPlace tCurrentNavPlace;
        ACDGetNavMeshHandleWithPlace(tACDTarget, tCurrentNavPlace.tNavMeshHandle, tCurrentNavPlace.tPlace);
    // 
        PowerAPISummonActor tSummonInfo;
        SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
        tSummonInfo.idPower = idPower;
        tSummonInfo.snoActor = snoActor;
        tSummonInfo.nActorLevel = tACDTarget.AttributesGetInt(ATTRIBUTE_LEVEL);
        tSummonInfo.idACDCorpse = ACDID_INVALID;
        tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    // 
    // 
        tSummonInfo.placeSummonLocation = tCurrentNavPlace.tPlace;
        tSummonInfo.qOrientation = tACDTarget.GetOrientation();
        tSummonInfo.eTeamOverride = TEAM_INVALID;
    // 
        ACDID idACDClone = PowerAPI_SummonActor(tSummonInfo);
    // 
    // 
        WeaponClass eWeaponClass = tACDTarget.GetWeaponClassForAnim(NULL);
        ActorCommonData & tACDClone = ACDGet(idACDClone);
        tACDClone.ChangeActor(tACDTarget.GetActorSNO());
        tACDClone.AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, eWeaponClass);
        tACDClone.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, 1);
        tACDClone.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, 1);
    // 
        ActorLocationSetScale(tACDClone.tLocation, tACDTarget.GetScale());
    // 
        SPlayerAddPet(idACDTargetSwungAt, PETTYPE_WIZARDMIRRORIMAGE, idACDClone, FALSE, -1, NULL);
    // 
        VisualInventoryCopy(idACDTargetSwungAt, idACDClone);
    // 
    // 
        if (!ptACDOwner->HasAttachedACD())
        {
    // 
            ptACDOwner->AttachACD(tACDClone);
        }
    // 
    // 
        tACDClone.AttributesSetInt(ATTRIBUTE_ATTACHMENT_HANDLED_BY_CLIENT, 1);
        tACDClone.AttributesSetInt(ATTRIBUTE_DELETED_ON_SERVER, 1);
    // 
    // 
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bRemoveOnDeath = FALSE;
        tBuffStartData.bPositiveEffect = FALSE;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, 1);
        tBuffStartData.bInfinite = TRUE;
        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
        BrainUpCallOnPowerStop((ActorID)ACDGetOwner(idACDTargetSwungAt), idPower, TRUE);
    }
}




static void sSpellEffect_SiegebreakerDemon_Bite(PowerID idPower, void * ptData)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    }
}






































static void sSpellEffect_SiegebreakerDemon_Pound(PowerID idPower, void * ptData)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    // 
    if (!ptACD->HasAttachedACD())
    {
        // 
        return;
    }
    // 
    ACDID idACDAttachment = ptACD->GetAttachedACD();
    ActorCommonData & tACDAttachment = ACDGet(idACDAttachment);
    // 
    // 
    ptACD->DetachACD(tACDAttachment);
    WorldPlace tPlace = ptACD->GetPlace();
    tACDAttachment.SetFacing(tPlace, FALSE);
    // 
    // 
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, tPlace, tExplosion);
    PowerAPI_DoExplosion(tExplosion);
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
    tPayload.wdKnockbackMagnitude = 16.0f;
    PowerAPI_DoPayload(tPayload, tACDAttachment.id, ACDID_INVALID);
}



static void sSpellEffect_SiegebreakerDemon_Stomp(PowerID idPower, void * ptData)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, ptACD->GetPlace(), tExplosion);
    PowerAPI_DoExplosion(tExplosion);

    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        // 
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        // 
        if (idACDTarget != ptACD->id)
        {
            // 
            // 
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            // 
            if (ptACDTarget && ptACDTarget->IsPet())
            {
                // 
                BrainAPIDoDeath((ActorID)ACDGetOwner(ptACDTarget->id), (ActorID)ptACD->idOwner, TRUE, TRUE, TRUE, NULL, NULL);
            }
            else
            {
                // 
                // 
                PowerAPIAttackPayload tPayload;
                PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
                tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
                tPayload.wdKnockbackMagnitude = 16.0f;
                PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
            }
        }
    }
    // 
    // 
    // 
}



static void sSpellEffect_SiegebreakerDemon_LookAround(PowerID idPower, void * ptData)
{
}









static void sSpellEffect_SiegebreakerDemon_ChargeCleanup(PowerID idPower, void *)
{
    PowerInterrupt(idPower, FALSE);
}





static BOOL sSiegebreakerIsBlockedWalkingTowardsPoint(ACDID idACD, const WorldPlace & placeDest)
{
    // 
    // 
    ActorCommonData & tACD = ACDGet(idACD);
    ActorMovement * ptMove = tACD.GetActorMovement();
    // 
    float flRadius;
    WorldPlace placeCurr;
    // 
    ptMove->GetPlaceAndRadius(placeCurr, flRadius, FALSE);
    // 
    Circle circleStart;
    circleStart.Set((const Vector2D &)placeCurr.wp, flRadius);
    // 
    WorldCollParams tCollParams(tACD.GetCollSettings());
    WorldCollResult tResult;
    WorldCollSweptCircle(placeCurr.idSWorld,
                         ptMove->m_hNavCell,
                         circleStart,
                         (const Vector2D &)placeDest.wp,
                         tCollParams,
                         0,
                         0,
                         idACD,
                         tResult);
    // 
    return tResult.bCollision;
}
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
static const int32 SIEGEBREAKER_CHARGE_UPDATE_RATE = 10;
static void sSpellEffect_SiegebreakerDemon_ChargeImpact(PowerID idPower, void * ptData)
{
    // 
    // 
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }
    // 
    // 
    ActorID idActor = (ActorID)ptACD->idOwner;
    // 
    if (ActorPowersGetPrimary(idActor) != idPower)
    {
        return;
    }
    // 
    Power * ptPower = PowerGet(idPower);
    // 
    Vector3D wvCurrentPosition = ptACD->GetWorldPlaceCenter().wp;
    Vector3D wvLastPosition = SiegebreakerDemonBrainGetLastChargePosition(idActor);
    WorldDistance wdDistSquared = Vector3D::DistanceSquared(wvCurrentPosition, wvLastPosition);
    if (wdDistSquared < 0.0001f)
    {
        Angle aYaw;
        Vector3D wvOffset = cgwvXAxis * 0.1f;
        aYaw = ptACD->GetOrientation().ToYaw();
        Vector3D::Rotate(cgwvZAxis, aYaw, wvOffset);
        WorldPlace wpForward;
        WorldPlaceOffset(ptACD->GetPlace(), wvOffset, wpForward);
        if (sSiegebreakerIsBlockedWalkingTowardsPoint(ptACD->id, wpForward))
        {
    // 
            AnimPlayerPlayNextAnimation(*ptACD);
    // 
            AnimationTiming tAnimationTiming;
            AnimPlayerGetAnimationTiming(*ptACD, ptPower->dwAnimationTag, (uint32)-1, 1.0f, tAnimationTiming);
    // 
            uint32 dwCooldownAnimDuration = tAnimationTiming.tAnimSpec[ANIMOFFSET_END].dwDuration;
    // 
    // 
            PowerTriggerAddTimerTrigger(idPower, dwCooldownAnimDuration, sSpellEffect_SiegebreakerDemon_ChargeCleanup, ptData);
        }
        else
        {
    // 
            PowerTriggerAddTimerTrigger(idPower, SIEGEBREAKER_CHARGE_UPDATE_RATE, sSpellEffect_SiegebreakerDemon_ChargeImpact, ptData);
        }
        return;
    }
    SiegebreakerDemonBrainSetLastChargePosition(idActor, wvCurrentPosition);
    // 
    // 
    PowerAPIExplosion tExplosion; PowerHelper_ExplosionInit(idPower, ptACD->GetPlace(), tExplosion);
    PowerAPI_DoExplosion(tExplosion);
    // 
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    // 
    NavPlace tNavPlace;
    if (ptPower->tTarget.GetNavPlace(tNavPlace))
    {
    // 
    // 
    // 
        PowerAPIChooseTargets tTargets;
        PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
        PowerAPI_DoChooseTargets(tTargets, listTargets);
    // 
        POS pos = listTargets.GetHeadPosition();
        while (pos)
        {
    // 
            ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
    // 
    // 
            if (idACDTarget == ptACD->id)
            {
                continue;
            }
    // 
            PowerAPIAttackPayload tPayload; PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
            tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
            tPayload.wdKnockbackMagnitude = 20.0f;
            PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
        }
    // 
    // 
    // 
    // 
        PowerTriggerAddTimerTrigger(idPower, SIEGEBREAKER_CHARGE_UPDATE_RATE, sSpellEffect_SiegebreakerDemon_ChargeImpact, ptData);
    }
}




static void sSpellEffect_SiegebreakerDemon_Charge(PowerID idPower, void * ptData)
{
    // 
    // 
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    // 
    sSpellEffect_SiegebreakerDemon_ChargeImpact(idPower, ptData);
}














static void sSpellEffect_SiegebreakerDemon_MiniCharge(PowerID idPower, void * ptData)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, ptACD->GetPlace(), tExplosion);
    PowerAPI_DoExplosion(tExplosion);

    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    // 
    Power * ptPower = PowerGet(idPower);
    NavPlace tNavPlace;
    // 
    if (ptPower->tTarget.GetNavPlace(tNavPlace))
    {
        // 
        // 
        // 
        // 
        PowerAPIChooseTargets tTargets;
        PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
        // 
        // 
        // 
        // 
        Vector3D wvOffset = cgwvXAxis;
        Angle aYaw = ptACD->GetOrientation().ToYaw();
        Vector3D::Rotate(cgwvZAxis, aYaw, wvOffset);
        const int32 nOffsetDistance = 5;
        WorldPlaceOffset(tTargets.tGeometry.tPie.placeCenter, wvOffset * nOffsetDistance, tTargets.tGeometry.tPie.placeCenter);
        tTargets.tGeometry.tPie.wvPieVector = wvOffset;
        // 
        PowerAPI_DoChooseTargets(tTargets, listTargets);

        // 
        // 
        POS pos = listTargets.GetHeadPosition();
        while (pos)
        {
            // 
            POS posLast = pos;
            ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
            // 
            if (idACDTarget == ptACD->id)
            {
                listTargets.RemoveAt(posLast);
            }
        }

        // 
        // 
        // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
        tPayload.wdKnockbackMagnitude = 9.0f;
        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    }
}



static void sSpellEffect_PoisonBreath(PowerID idPower, void * ptData)
{



    Power * ptPower = PowerGet(idPower);


    WorldPlace placeTarget;
    ptPower->tTarget.GetPlace(placeTarget);


    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, placeTarget, tExplosion);


    PowerGetTargetPlace(AutoPowerDefinitionR(ptPower->snoPower), ptPower->tTarget, placeTarget);
    WorldPlace placeSource = ACDGet(ptPower->idACD).GetPlace();
    WorldPlace placeExplosion;

    Vector3D wvSourceToTarget;
    WorldPlaceGetVectorBetween(placeSource, placeTarget, wvSourceToTarget);
    wvSourceToTarget.z = 0.0f;
    wvSourceToTarget.Normalize();

    tExplosion.qOrientation.FromArc(cgwvXAxis, wvSourceToTarget);


    WorldPlaceOffset(placeSource, wvSourceToTarget * 6.0f, placeExplosion);

    ptPower->tTarget.InitializeFromPlace(placeExplosion);


    PowerAPI_DoExplosion(tExplosion);


    sSpellEffect_DamageCloud(idPower, NULL);


    int32 nDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_POISON_CLOUD_INTERVAL_DURATION) * 60.0f);
    int32 nLoops = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_POISON_CLOUD_NUM_INTERVALS));

    for (int32 i = 0; i < nLoops; i++)
    {
        PowerTriggerAddTimerTrigger(idPower, nDelay * (i + 1), sSpellEffect_DamageCloud, NULL);
    }
}



static void sSpellEffect_SetDamageStateOne(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ACDID idACDTarget = ptPower->tTarget.GetACD();
        if (idACDTarget != ACDID_INVALID)
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (ptACDTarget)
            {
                int32 nDamageState = ptACDTarget->AttributesGetInt(ATTRIBUTE_DAMAGE_STATE_CURRENT);
                if (nDamageState != 1)
                {
                    ptACDTarget->ChangeDamageState(1);
                }
            }
        }
    }
}






static void sSpellEffect_Nothing(PowerID, void *)
{
}



















ACDID SpellEffect_SpawnActor(PowerID idPower, SNO snoActor, NavPlace tNavPlace, Quaternion & qOrientation, uint32 dwAddFlags, BOOL bLoot)
{
    // 
    // 
    PowerAPISummonActor tSummonInfo;
    SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
    tSummonInfo.idPower = idPower;
    tSummonInfo.snoActor = snoActor;
    tSummonInfo.nActorLevel = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SUMMONED_ACTOR_LEVEL));
    tSummonInfo.idACDCorpse = ACDID_INVALID;
    tSummonInfo.dwAddFlags = dwAddFlags;
    tSummonInfo.placeSummonLocation = tNavPlace.tPlace;
    tSummonInfo.qOrientation = qOrientation;
    tSummonInfo.eTeamOverride = TEAM_INVALID;
    // 
    uint32 dwAnimTag = PowerContactTagGetInt(idPower, TAG_POWER_SUMMONED_ANIMATION);
    if (dwAnimTag != (uint32)-1)
    {
        // 
        tSummonInfo.dwAnimTag = dwAnimTag;
    }
    // 
    // 
    ACDID idACDMonster = PowerAPI_SummonActor(tSummonInfo);
    if (!bLoot && idACDMonster != ACDID_INVALID)
    {
        // 
        // 
        ActorCommonData & tACDMonster = ACDGet(idACDMonster);
        tACDMonster.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDMonster.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
        // 
        tACDMonster.InheritGroupsFromACD(*PowerGetOwner(idPower));
    }
    // 
    return idACDMonster;
}













































































static void sSpellEffect_Curse_Proc(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ACDID idACDTarget = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        return;
    }
    // 
    // 
    float flNewAmplifyPercent = PowerGetFormulaValue(idPower, TAG_POWER_CURSE_DAMAGE_AMPLIFY_PERCENT);
    int32 nDuration = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_CURSE_DURATION) * 60.0f);
    int32 nOldTimer = ptACDTarget->AttributesGetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, ATT_CURSE));
    if (nOldTimer < nDuration)
    {
        // 
        ptACDTarget->AttributesRefreshTimerGroup(ATT_CURSE, nDuration);
    }
    // 
    // 
    AttribGroupID idTimerGroup = ActorTimerEffectsGetGroup((ActorID)ACDGetOwner(idACDTarget), ATT_CURSE);
    if (idTimerGroup != ATTRIBGROUPID_INVALID)
    {
        // 
        // 
        float flAmplifyPercent = (nOldTimer > 0) ? ptACDTarget->AttributesGetFloat(ATTRIBUTE_AMPLIFYDAMAGE_PERCENT) : 0.0f;
        // 
        // 
        flAmplifyPercent = Math::Max(flAmplifyPercent, flNewAmplifyPercent);
        FastAttribSetValueFloat(idTimerGroup, ATTRIBUTE_AMPLIFYDAMAGE_PERCENT, flAmplifyPercent);
    }
}




























































static void sSpawnSporePod(PowerID idPower, void * pData)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptPower || !ptACD || !ptACD->IsAlive())
    {
        return;
    }
    SNO snoMonster = GlobalSNOGet(GLOBALSNO_MONSTER_SPORE_POD);
    float flOffset = (float)(intptr)pData;
    WorldPlace placeSource = ptACD->GetPlace();
    Vector3D vForward = ptACD->GetForward();
    WorldPlace placeTarget(placeSource);
    placeTarget.wp += vForward * flOffset;
    ptPower->tTarget.InitializeFromPlace(placeTarget);
    // 
    // 
    PowerAPISummonActor tSummonInfo;
    BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonster, ACDID_INVALID, tSummonInfo, TRUE);
    if (!bSuccess)
    {
        // 
        // 
        // 
        return;
    }
    // 
    ACDID idACDSpore = PowerAPI_SummonActor(tSummonInfo);
    if (idACDSpore != ACDID_INVALID)
    {
        // 
        int nSummonDuration = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SUMMONED_ACTOR_LIFE_DURATION) * 60.0f);
        if (nSummonDuration > 0)
        {
            // 
            // 
            ActorCommonData & tACDSpore = ACDGet(idACDSpore);
            tACDSpore.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, GameGetServerTime() + nSummonDuration);
            // 
            // 
            ActorFlagSet((ActorID)tACDSpore.idOwner, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
        }
    }
}




static void sSpellEffect_Summon_Spore_Pods(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }
    ActorID idActor = (ActorID)ptACDOwner->idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }
    // 
    WorldPlace placeSource = ptACDOwner->GetPlace();
    WorldPlace placeTarget;
    ptPower->tTarget.GetPlace(placeTarget);
    // 
    if (placeSource.idSWorld != placeTarget.idSWorld)
    {
        // 
        // 
        // 
        return;
    }
    // 
    // 
    int32 nToSpawn = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SKELETON_SUMMON_COUNT));
    // 
    WorldDistance wdDist = Vector3D::Distance(placeSource.wp, placeTarget.wp);
    float flOffsetPerSpawn = Math::Pin(wdDist / (float)(nToSpawn + 1), 4.0f, 10.0f);
    const int32 nDelay = 30;
    float flBaseOffset = ptACDOwner->GetRadius();
    // 
    // 
    for (int32 i = 0; i < nToSpawn; i++)
    {
        // 
        float flOffset = flBaseOffset + flOffsetPerSpawn * (float)(i + 1);
        PowerTriggerAddTimerTrigger(idPower, nDelay * i, sSpawnSporePod, (void *)(intptr)flOffset);
    }
}




static ACDID sSpellEffect_Transform_Monster_Base(PowerID idPower, BOOL bUseSummonActorTeam)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return ACDID_INVALID;
    }
    NavPlace tNavPlace;
    ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
    Quaternion qOrientation = ptACD->GetOrientation();
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, ptACD->GetPlace(), tExplosion);
    PowerAPI_DoExplosion(tExplosion);
    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    SNO snoTransformMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
    // 
    ACDID idACDMonster = SpellEffect_SpawnActor(idPower, snoTransformMonsterActor, tNavPlace, qOrientation, dwAddFlags, TRUE);
    if (idACDMonster != ACDID_INVALID)
    {
        // 
        // 
        ActorCommonData & tACDMonster = ACDGet(idACDMonster);
        // 
        if (ptACD->dwFlags & (1 << ACD_KNOWN_WITH_SCENE))
        {
            // 
            tACDMonster.dwFlags |= (1 << ACD_KNOWN_WITH_SCENE);
        }
        // 
        // 
        ActorID idOldActor = (ActorID)ptACD->idOwner;
        BrainAPIDoDeath(idOldActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        ActorFlagForDeletion(idOldActor);
        // 
        if (bUseSummonActorTeam)
        {
            // 
            ScopedActorDefinitionLockR pdefMonsterActor(snoTransformMonsterActor);
            if (pdefMonsterActor)
            {
                // 
                ActorCommonData & tACDMonsterScoped = ACDGet(idACDMonster);
                Team eMonsterTeam = TagMapGetInt(pdefMonsterActor->hTagMap, TAG_ACTOR_DEFAULT_TEAM);
                tACDMonsterScoped.SetTeam(eMonsterTeam);
            }
        }
    }
    // 
    return idACDMonster;
}

static void sSpellEffect_Transform_Monster(PowerID idPower, void *)
{
    // 
    // 
    sSpellEffect_Transform_Monster_Base(idPower, FALSE);
}



static void sSpellEffect_Transform_Monster_Inherit_Group(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    uint32 uGroupPrimary = ptACD->uGroupPrimary;
    uint32 uGroupSecondary = ptACD->uGroupSecondary;
    ACDID idNewMonster = sSpellEffect_Transform_Monster_Base(idPower, TRUE);
    if (idNewMonster != ACDID_INVALID)
    {
        ActorCommonData * ptACDNewMonster = ACDTryToGet(idNewMonster);
        if (ptACDNewMonster)
        {
            // 
            ptACDNewMonster->SetPrimaryGroup(uGroupPrimary, TRUE, ACDID_INVALID);
            ptACDNewMonster->SetSecondaryGroup(uGroupSecondary, TRUE, ACDID_INVALID);
        }
    }
}



static void sSpellEffect_KillPack_Proc(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    ActorID idActor = (ActorID)ptACD->idOwner;
    ACDPack * ptACDPack = ACDPackTryToGet(ptACD->idACDPack);
    if (!ptACDPack)
    {
        return;
    }
    const ACDIDListPooled * plistACDPack = ptACDPack->GetMemberList();
    // 
    POS pos = plistACDPack->GetHeadPosition();
    while (pos)
    {
        // 
        ACDID idACDPackMember = (ACDID)plistACDPack->GetNext(pos);
        if (idACDPackMember != ptACD->id)
        {
            // 
            ActorCommonData * ptACDPackMember = ACDTryToGet(idACDPackMember);
            if (ptACDPackMember->IsAlive())
            {
                // 
                ActorID idActorPackMember = (ActorID)ACDGetOwner(idACDPackMember);
                if (idActorPackMember != ACTORID_INVALID)
                {
                    // 
                    BrainAPIDoDeath(idActorPackMember, idActor, FALSE, FALSE, FALSE, NULL, NULL);
                }
            }
        }
    }
}



void sRoot_RooterBuffRemoved(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    ACDID idACDRootee = ptBuff->idACDSource;
    ActorCommonData * ptACDRootee = ACDTryToGet(idACDRootee);
    if (ptACDRootee && ptACDRootee->IsAlive())
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(ptBuff->snoPower, ptACDRootee, ptBuff->idACDAffected);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }
}







void SpellEffectRun(PowerID idPower, SpellEffect eSpellEffect, void * pData);  // SpellEffect.cpp:19358

void sRoot_KillRooter(
    PowerAPIBuffInstance * ptBuff,
    BuffRemovalReason eReason,
    void * pData)
{


    ACDID idACDRooter = ptBuff->idACDSource;
    ActorCommonData * ptACDRooter = ACDTryToGet(idACDRooter);
    if (ptACDRooter)
    {
        ScopedPowerDefinitionLockR pdefPower(ptBuff->snoPower);
        SpellEffect eSpellEffect = (SpellEffect)PowerContactTagGetInt(pdefPower, TAG_POWER_ROOT_END_FUNC, 0);
        if (eSpellEffect != -1)
        {
            SpellEffectRun(ptBuff->idPower, eSpellEffect, (void *)(intptr)ptBuff->idACDAffected);
        }

        if (ptACDRooter->IsAlive())
        {
            ActorID idActorRooter = (ActorID)ACDGetOwner(idACDRooter);
            if (!ActorFlagIsSet(idActorRooter, ACTOR_DEACTIVATING))
            {
                BrainAPIDoDeath(idActorRooter, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
            }
        }
    }
}




static void sRoot_ImmobilizeTarget(PowerID idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDRooter = PowerGetOwner(idPower);
    if (!ptACDRooter)
    {
        return;
    }
    if (ptPower->tTarget.IsTargetingACD() &&
        !ptPower->tTarget.IsInvalid())
    {
        return;
    }
    ACDID idACDTarget = ptPower->tTarget.idACDTarget;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    if (!tACDTarget.IsAlive())
    {
        return;
    }
    if (!tACDTarget.CanBeImmobilized())
    {
        return;
    }
    float flRootResistance = tACDTarget.AttributesGetFloat(ATTRIBUTE_RESISTANCE_ROOT_TOTAL);
    if (RandFloat_Internal(SGameGlobalsGet().rng) < flRootResistance)
    {
        return;
    }
    WorldPlace wpStart = tACDTarget.GetPlace();
    WorldPlace wpEnd = ptACDRooter->GetPlace();
    if (wpStart.idSWorld != wpEnd.idSWorld)
    {
        return;
    }
    ActorID idActor = (ActorID)tACDTarget.idOwner;
    PowerID idCurrPower = ActorPowersGetPrimary(idActor);
    if (idCurrPower != POWERID_INVALID)
    {
        Power * ptPowerScoped = PowerTryToGet(idCurrPower);
        if (ptPowerScoped && PowerIsTranslate(AutoPowerDefinitionR(ptPowerScoped->snoPower)))
        {
            PowerInterrupt(idCurrPower, FALSE);
        }
    }
    ActorMovement * ptMove = tACDTarget.GetActorMovement();
    if (ptMove)
    {
        ptMove->StopNow();
    }
    Vector3D vKnockbackDir = wpEnd.wp - wpStart.wp;
    vKnockbackDir.z = 0.0f;
    float flMag = 0.0f;
    if (vKnockbackDir.MagnitudeSquared() > 1e-12f)
    {
        flMag = vKnockbackDir.Normalize();
    }
    // 
    // 
    uint32 dwGetHitAnimTag = AnimPlayerAnimExists(tACDTarget, TAG_ACTOR_ANIMATION_KNOCK_BACK) ? TAG_ACTOR_ANIMATION_KNOCK_BACK : TAG_ACTOR_ANIMATION_GETHIT;
    // 
    // 
    // 
    // 
    Angle aYaw = Vector3D::AngleBetween(cgwvXAxis, vKnockbackDir) + PI;
    BrainStanceDoKnockBack(idPower, idActor, aYaw, flMag, 1.0f, -0.01f, dwGetHitAnimTag, TRUE, SNO_NONE, FALSE, FALSE);
    // 
    int32 nDuration = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_ROOT_DURATION) * 60.0f);
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    if (nDuration > 0)
    {
        // 
        tBuffStartData.dwDurationMin = nDuration;
    }
    // 
    tBuffStartData.pRemovalEffect = sRoot_KillRooter;
    // 
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IS_ROOTED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ROOT_TOTAL_TICKS, nDuration);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
    // 
    if (ACDIsPlayer(ptACDRooter->id))
    {
        // 
        Player * ptPlayer = PlayerGetByACD(ptACDRooter->id);
        if (ptPlayer)
        {
            // 
            // 
            // 
            FloatingNumberMessage tMessage;
            tMessage.annTarget = ANNFromACD(tACDTarget.id);
            tMessage.flDamage = 0.0f;
            tMessage.eType = FLOATING_NUMBER_TYPE_ROOT;
            // 
            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
        }
    }
    // 
    // 
    // 
    if (tACDTarget.IsAlive())
    {
        // 
        // 
        // 
        // 
        // 
        ACDID idACDRooterResult = PowerAPI_GetRootSource(tACDTarget.id);
        if (idACDRooterResult != ptACDRooter->id)
        {
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        PowerAPIBuffStartData tBuffStartDataRooter(idPower);
        tBuffStartDataRooter.idACDSource = tACDTarget.id;
        tBuffStartDataRooter.pRemovalEffect = sRoot_RooterBuffRemoved;
        PowerAPI_BuffAddToACD(tBuffStartDataRooter, *ptACDRooter);
        // 
        float flAutoDecayPerSecond = 0.0f;
        // 
        if (nDuration > 0)
        {
            flAutoDecayPerSecond = (1.0f / (float)nDuration) * 60.0f;
        }
        // 
        ptACDRooter->AttributesSetFloat(ATTRIBUTE_ROOT_AUTO_DECAY_PER_SECOND, flAutoDecayPerSecond);
        // 
        ptACDRooter->AttributesSetFloat(ATTRIBUTE_ROOT_UNIT_VALUE, 1.0f);
        ptACDRooter->AttributesSetInt(ATTRIBUTE_ROOT_TARGET_ACD, tACDTarget.id);
        ptACDRooter->SetTargetable(TRUE);
    }
}



static void sSpellEffect_SpawnRootAtACD(PowerID idPower, ACDID idACDTarget, SNO snoRootMonsterActor)
{
    if (snoRootMonsterActor == SNO_NONE)
    {
        return;
    }
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    NavPlace tNavPlace;
    ACDGetNavMeshHandleWithPlace(tACDTarget, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
    Quaternion qOrientation = tACDTarget.GetOrientation();
    PowerAPISummonActor tSummonInfo;
    SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
    tSummonInfo.idPower = idPower;
    tSummonInfo.snoActor = snoRootMonsterActor;
    tSummonInfo.nActorLevel = ptACDOwner->AttributesGetInt(ATTRIBUTE_LEVEL);
    tSummonInfo.idACDCorpse = ACDID_INVALID;
    tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    tSummonInfo.placeSummonLocation = tNavPlace.tPlace;
    tSummonInfo.qOrientation = qOrientation;
    tSummonInfo.eTeamOverride = TEAM_INVALID;
    ACDID idACDRoot = PowerAPI_SummonActor(tSummonInfo);
    if (idACDRoot != ACDID_INVALID)
    {
        // 
        // 
        ActorCommonData & tACDRoot = ACDGet(idACDRoot);
        tACDRoot.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDRoot.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
        tACDRoot.SetTargetable(FALSE);
        tACDRoot.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
        tACDRoot.AttributesSetInt(ATTRIBUTE_STUN_IMMUNE, TRUE);
        tACDRoot.AttributesSetInt(ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);
        tACDRoot.AttributesSetInt(ATTRIBUTE_GETHIT_IMMUNE, TRUE);
        // 
        SNO snoRootPower = PowerGetSNO(idPower);
        // 
        // 
        if (PowerContactTagGetBool(AutoPowerDefinitionR(snoRootPower), TAG_POWER_ROOT_IS_UNTARGETABLE, 0))
        {
            // 
            tACDRoot.SetTargetable(FALSE);
        }
        // 
        // 
        Target tTarget;
        tTarget.eType = TT_ACTOR;
        tTarget.idACDTarget = idACDTarget;
        // 
        PowerID idPowerGrab = POWERID_INVALID;
        SNO snoGrabPower = PowerContactTagGetSNO(idPower, TAG_POWER_ROOT_GRAB_POWER);
        // 
        if (snoGrabPower != SNO_NONE)
        {
            // 
            idPowerGrab = PowerStart(idACDRoot, snoGrabPower, tTarget);
        }
        // 
        if (idPowerGrab == POWERID_INVALID)
        {
            // 
            ActorID idActorRoot = (ActorID)ACDGetOwner(idACDRoot);
            BrainAPIDoDeath(idActorRoot, idActorRoot, TRUE, TRUE, TRUE, NULL, NULL);
        }
        else
        {
            // 
            // 
            float flRootDuration = PowerGetFormulaValue(idPower, TAG_POWER_ROOT_DURATION);
            flRootDuration = Math::Max(0.0f, flRootDuration);
            int32 nRootTicks = Math::FloatRoundToInt(flRootDuration * 60.0f);
            uint32 dwExpireTime = GameGetServerTime() + nRootTicks;
            tACDRoot.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, dwExpireTime);
        }
    }
}




static void sSpellEffect_Root_Proc(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower && ptPower->tTarget.eType == TT_ACTOR)
    {
        // 
        // 
        // 
        // 
        SNO snoRootMonsterActor = PowerEffectTagGetSNO(idPower, TAG_POWER_SPAWNED_ACTOR);
        sSpellEffect_SpawnRootAtACD(idPower, ptPower->tTarget.idACDTarget, snoRootMonsterActor);
    }
}




static void sSpellEffect_Entangle_Ray(PowerID idPower, void *)
{
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    // 
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        SNO snoRootMonsterActor = GlobalSNOGet(GLOBALSNO_MONSTER_ENTANGLING_ROOTS);
        sSpellEffect_SpawnRootAtACD(idPower, idACDTarget, snoRootMonsterActor);
    }
}



static ACDID sRootGrab_TryToGetTarget(PowerID idPower)
{
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    tTargets.dwFlags |= (1 << CTF_CLOSEST_ONLY_BIT);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    ACDID idACDTarget = ACDID_INVALID;
    if (listTargets.Count() > 0)
    {
        ACDID idACDPotential = (ACDID)listTargets.GetHead();
        if (!ACDGet(idACDPotential).IsRooted())
        {
            idACDTarget = idACDPotential;
        }
    }

    // 
    // 
    return idACDTarget;
}



static void sSpellEffect_Root_Grab(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDRooter = PowerGetOwner(ptPower);
    if (!ptACDRooter || !ptACDRooter->IsAlive())
    {
        // 
        return;
    }
    // 
    // 
    ACDID idACDTarget = sRootGrab_TryToGetTarget(idPower);
    if (idACDTarget != ACDID_INVALID)
    {
        // 
        // 
        ptPower->tTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
        // 
        sRoot_ImmobilizeTarget(idPower);
    }
}



static void sSpellEffect_Root_Try_Grab(PowerID idPower, void *)
{
    // 
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACDRooter = PowerGetOwner(ptPower);
    // 
    if (!ptACDRooter || !ptACDRooter->IsAlive())
    {
        // 
        return;
    }
    // 
    // 
    ACDID idACDTarget = sRootGrab_TryToGetTarget(idPower);
    if (idACDTarget != ACDID_INVALID)
    {
        // 
        // 
        ptPower->tTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
        // 
        sRoot_ImmobilizeTarget(idPower);
        // 
        // 
        ActorID idActorRooter = (ActorID)ptACDRooter->idOwner;
        AIStateSetActive(idActorRooter, FALSE);
        BrainStanceSetIdleAnimation(idActorRooter, TAG_ACTOR_ANIMATION_ROOT_GRAB_IDLE);
        PowerForceStop(idPower, FALSE);
    }
}



static void sSpellEffect_ZombieKiller_Grab(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ACDID idACDZombie = ptPower->tTarget.GetACD();
        if (idACDZombie != ACDID_INVALID)
        {
            ActorCommonData * ptACDZombie = ACDTryToGet(idACDZombie);
            if (ptACDZombie && ptACDZombie->IsAlive())
            {
                sRoot_ImmobilizeTarget(idPower);
            }
        }
    }
}



static void sSpellEffect_ZombieKiller_Squish(PowerID, void * pData)
{


    ACDID idACDTarget = (ACDID)(intptr)pData;
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);

    if (ptACDTarget && ptACDTarget->IsAlive() &&
        !ACDIsPlayer(ptACDTarget->id))
    {
        PowerAPIDamage tDamage;
        PowerHelper_DamageInit(tDamage, POWERID_INVALID, idACDTarget);
        tDamage.snoAltDamageSourcePower = GlobalSNOGet(GLOBALSNO_POWERS_ZOMBIE_KILLER_GRAB);

        ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);

        if (!ActorFlagIsSet(idActorTarget, ACTOR_DEACTIVATING))
        {
            BrainAPIDoDeath(idActorTarget, ACTORID_INVALID, TRUE, TRUE, TRUE, &tDamage, NULL);
        }
    }
}



static void sSpellEffect_Damage_Aura(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    // 
    if (tPayload.arCombatDamage[DAMAGE_TYPE_FIRE].flMin > EPSILON)
    {
        // 
        ptACD->AttributesSetInt(ATTRIBUTE_FIRE_AURA, 1);
    }
    if (tPayload.arCombatDamage[DAMAGE_TYPE_LIGHTNING].flMin > EPSILON)
    {
        // 
        ptACD->AttributesSetInt(ATTRIBUTE_LIGHTNING_AURA, 1);
    }
    if (tPayload.arCombatDamage[DAMAGE_TYPE_COLD].flMin > EPSILON)
    {
        // 
        ptACD->AttributesSetInt(ATTRIBUTE_COLD_AURA, 1);
    }
    if (tPayload.arCombatDamage[DAMAGE_TYPE_POISON].flMin > EPSILON)
    {
        // 
        ptACD->AttributesSetInt(ATTRIBUTE_POISON_AURA, 1);
    }
    // 
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);

    // 
    // 
    int32 nDuration = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_AURA_DURATION) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, nDuration, sSpellEffect_Damage_Aura, NULL);
}


static void sTeleportPlayEffect(PowerID idPower, BOOL bUseEndingEffect)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    // 
    if (!ptACD)
    {
        // 
        return;
    }
    // 
    // 
    // 
    uint32 dwEffectTag = bUseEndingEffect ? TAG_POWER_ENDING_EFFECT_GROUP : TAG_POWER_GENERIC_EFFECT_GROUP_0;
    // 
    // 
    SNO snoTeleportEffect = PowerGetEffect(idPower, dwEffectTag);
    if (snoTeleportEffect != SNO_NONE)
    {
        // 
        ACDPlayEffect(ptACD->id, EFFECT_PLAY_EFFECT_GROUP, snoTeleportEffect, ACDID_INVALID);
    }
}



static void sChampion_Finish(ACDID idACD)
{
    // 
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    BrainConditionSetCondition(idActor, BRAIN_CONDITION_READY);
    BrainAPIDoIdle(idActor);
}



static void sSpellEffect_Champion_Teleport_In(PowerID idPower, void *)
{
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        // 
        // 
        // 
        return;
    }
    // 
    sTeleportPlayEffect(idPower, TRUE);
    ptACD->Hide(FALSE);
    ptACD->SetTargetable(TRUE);
    // 
    sChampion_Finish(ptACD->id);
}


static const int32 CHAMPION_TELEPORT_NUM_POINTS_TO_CHECK = 3;
static const Angle CHAMPION_TELEPORT_ANGLE_INC = 2.09439516f;
static const float CHAMPION_TELEPORT_DISTANCE_MIN = 10.0f;
static const float CHAMPION_TELEPORT_DISTANCE_DELTA = 20.0f;
static void sSpellEffect_Champion_Teleport_Move(PowerID idPower, void *)
{
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    ActorID idActor = (ActorID)ptACD->idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }
    WorldPlace tStartPlace = ptACD->GetPlace();
    Angle aCurrentYaw = RandAngle(SGameGlobalsGet().rng);
    float flWinningDistSqr = 0.0f; float flDistance = RandRangeFloat_Internal(SGameGlobalsGet().rng, CHAMPION_TELEPORT_DISTANCE_MIN, CHAMPION_TELEPORT_DISTANCE_DELTA);
    WorldPlace wpFinalResult;
    wpFinalResult = tStartPlace;
    // 
    for (int32 i = 0; i < CHAMPION_TELEPORT_NUM_POINTS_TO_CHECK; i++)
    {
        aCurrentYaw = Math::NormalizeAngle(aCurrentYaw + CHAMPION_TELEPORT_ANGLE_INC);
    // 
    // 
        Vector3D vTargetOffset;
        vTargetOffset.x = flDistance * Math::Sin(aCurrentYaw);
        vTargetOffset.y = flDistance * Math::Cos(aCurrentYaw);
        vTargetOffset.z = 0.0f;
    // 
        Target tTarget;
        tTarget.eType = TT_POINT;
        WorldPlaceOffset(tStartPlace, vTargetOffset, tTarget.tPlace);
    // 
        NavPlace tNavPlace;
        PowerRollToTargetNavPlace(ptACD->id, tTarget, PASSABILITY_MONSTER, TRUE, tNavPlace);
        if (!NavMeshHandleIsNone(tNavPlace.tNavMeshHandle))
        {
            NavMeshHandle tNavMeshHandleResult;
            WorldPlace wpTempResult;
            if (FindSpawnLocation(ptActor->snoActor, tNavPlace.tNavMeshHandle, PASSABILITY_MONSTER, 0, wpTempResult, tNavMeshHandleResult) &&
    // 
                wpTempResult.idSWorld == tStartPlace.idSWorld)
            {
                SNO snoLevelArea = NavMeshHandleGetLevelArea(tNavMeshHandleResult);
                if (snoLevelArea == SNO_NONE || !LevelAreaIsTown(snoLevelArea))
                {
                    float flDistSqr = Vector3D::Distance2DSquared(wpTempResult.wp, tStartPlace.wp);
                    if (flDistSqr > flWinningDistSqr)
                    {
                        flWinningDistSqr = flDistSqr;
                        wpFinalResult = wpTempResult;
                    }
                }
            }
        }
    }
    // 
    // 
    ActorSnapToPosition(idActor, wpFinalResult, TRUE, TRUE);
    // 
    // 
    // 
    // 
    PowerTriggerAddTimerTrigger(idPower, 1, sSpellEffect_Champion_Teleport_In, NULL);
}


static const int32 CHAMPION_TELEPORT_TICKS = 48;
    // 
    // 
static void sSpellEffect_Champion_Teleport(PowerID idPower, void *)
{
    // 
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    sTeleportPlayEffect(idPower, FALSE);
    ptACD->Hide(TRUE);
    ptACD->SetTargetable(FALSE);
    // 
    // 
    // 
    // 
    PowerTriggerAddTimerTrigger(idPower, CHAMPION_TELEPORT_TICKS, sSpellEffect_Champion_Teleport_Move, NULL);
}






































































































































static void sSpellEffect_Champion_Clone(PowerID idPower, void *);






































ActorID GBAdd(ActorAddStruct & tActorAdd);  // X/Server/Actors/Actors.cpp:6041
void MonsterAffixesApply(ActorCommonData & tACD, MonsterRarity eMonsterRarity, const GBHandle & hGB);  // X/Server/RandomMonster.cpp:945
enum { GB_MONSTER_AFFIXES = 18 };  // X/Common/GameBalance.h (kept local, as GB_INVALID above)


void sSpellEffect_MonsterAffix_Doppelganger_OnDeath(
    PowerAPIBuffInstance * ptBuff,
    void * pData,
    ACDID idACDTarget,
    ACDID idACDKiller)
{


    ActorCommonData * ptACD = PowerGetOwner(ptBuff->idPower);
    ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
    if (ptACD->id != idACDTarget)
    {
        return;
    }

    ActorID idActor = (ActorID)ptACD->idOwner;


    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(ptBuff->idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTargetScoped = (ACDID)listTargets.GetNext(pos);


        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTargetScoped);
        if (!ptACDTarget)
            continue;

        if (ptACDTarget->AttributesGetInt(ATTRIBUTE_SUMMONED_BY_ACDID) == ptACD->id && ptACDTarget->eMonsterRarity == ptACD->eMonsterRarity)
        {
            ActorID idTarget = (ActorID)ptACDTarget->idOwner;
            if (ptACDKiller)
            {
                ActorID idKiller = (ActorID)ptACDKiller->idOwner;
                BrainAPIDoDeath(idTarget, idKiller, FALSE, FALSE, FALSE, NULL, NULL);
            }
            else
            {
                BrainAPIDoDeath(idTarget, idActor, FALSE, FALSE, FALSE, NULL, NULL);
            }
        }
    }
}




static void sSpellEffect_MonsterAffix_Doppelganger(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);





    ActorID idActor = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }



    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.pDeathEffect = sSpellEffect_MonsterAffix_Doppelganger_OnDeath;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);


    if (!ptACD->AttributesGetInt(ATTRIBUTE_POWER_SAVED_ATTRIBUTE))
    {
        ptACD->GenerateSummonerID();

        NavPlace tNavPlace;
        Quaternion qOrientation;
        ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
        qOrientation = ptACD->GetOrientation();

        ActorAddStruct tActorAdd;
        tActorAdd.snoActor = ptACD->snoActor;
        tActorAdd.dwActorAddFlags = 0;
        tActorAdd.tPlace = tNavPlace.tPlace;
        tActorAdd.qOrientation = qOrientation;
        tActorAdd.eTeam = ptACD->GetNaturalTeam();
        tActorAdd.tSNOName = ptACD->tSNOName;
        tActorAdd.tMarkersName = MarkerStringHash(SNOToString(ACTOR_SNO, ptACD->tSNOName.snoHandle, FALSE));
        tActorAdd.nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
        tActorAdd.tNavMeshHandle = tNavPlace.tNavMeshHandle;
        tActorAdd.flScale = ptACD->GetScale();
        tActorAdd.bIsSummon = TRUE;

        ActorID idActorClone = GBAdd(tActorAdd);
        if (idActorClone == ACTORID_INVALID)
        {
            return;
        }


        Actor * ptActorClone = ActorTryToGet(idActorClone);
        if (!ptActorClone)
        {
            return;
        }


        ACDID idACDClone = ptActorClone->idACD;
        ActorCommonData * ptACDClone = ACDTryToGet(idACDClone);
        if (!ptACDClone)
        {
            return;
        }


        uint32 SummonerID = ptACD->AttributesGetInt(ATTRIBUTE_SUMMONER_ID);

        ptACDClone->AttributesSetInt(ATTRIBUTE_SUMMONED_BY_ACDID, SummonerID);


        SigmaMemoryCopy(ptACDClone->pRareNameGBIDs, ptACD->pRareNameGBIDs, sizeof(ptACD->pRareNameGBIDs));
        ptACDClone->eMonsterRarity = ptACD->eMonsterRarity;


        int32 nCloneAttributeIdx = 0;
        GBID gbidDoppelgangerAffix = StringToGBID("Doppelganger");
        for (int32 i = 0; i < 8; i++)
        {
            if (ptACD->pMonsterAffixGBIDs[i] != GBID_NONE && ptACD->pMonsterAffixGBIDs[i] != gbidDoppelgangerAffix)
            {

                GBHandle hGB;
                hGB.eType = GB_MONSTER_AFFIXES;
                hGB.gbid = ptACD->pMonsterAffixGBIDs[i];

                if (GBIDExists(hGB))
                {






                    MonsterAffixesApply(*ptACDClone, ptACD->eMonsterRarity, hGB);

                    ptACDClone->pMonsterAffixGBIDs[nCloneAttributeIdx] = ptACD->pMonsterAffixGBIDs[i];
                    nCloneAttributeIdx++;
                }
            }
        }



        GBHandle hGB;
        hGB.eType = GB_MONSTER_AFFIXES;
        hGB.gbid = StringToGBID("Doppelganger Clone");
        MonsterAffixesApply(*ptACDClone, ptACD->eMonsterRarity, hGB);
        ptACDClone->pMonsterAffixGBIDs[nCloneAttributeIdx] = hGB.gbid;


        ptACD->AttributesSetInt(ATTRIBUTE_POWER_SAVED_ATTRIBUTE, TRUE);
    }
}



static void sSpellEffect_Doppelganger_Clone(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    ScopedActorDefinitionLockR pdefActor(ptACD->tSNOName.snoHandle);
    ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);

    int32 nLevel = ActorGetCorrectLevel(ptACD->tSNOName, ptACD->AttributesGetInt(ATTRIBUTE_LEVEL));

    float flHPMin;
    float flHPDelta;
    BOOL bAvenger =

        ptACD->IsAvenger();

    ActorGetMonsterHPData(pdefActor->snoMonster, nLevel, ptACD->eMonsterRarity, flHPMin, flHPDelta, bAvenger, FALSE, FALSE, FALSE);

    float flHitPoints = RandRangeFloat_Internal(SGameGlobalsGet().rng, flHPMin, flHPDelta);





    float flHPCurrent = ptACD->GetHitpointPercent();

    float flHealthMultiplier = 1.0f + ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_BONUS);
    flHealthMultiplier = Math::Max(flHealthMultiplier, 0.0f);
    flHitPoints *= flHealthMultiplier;
    ptACD->AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX, flHitPoints);


    ptACD->AttributesSetFloat(ATTRIBUTE_HITPOINTS_CUR, flHPCurrent * flHitPoints);



    float flDamagePercent = ptACD->AttributesGetFloat(ATTRIBUTE_CHAMPION_CLONE_DAMAGE_BONUS_PERCENT);
    ptACD->AttributesSetFloat(ATTRIBUTE_DAMAGE_WEAPON_PERCENT_BONUS, flDamagePercent);
    ptACD->AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, flDamagePercent);


    SNO snoNormalLoot = pdefMonster->snoTreasureClass;
    ptACD->AttributesSetInt(ATTRIBUTE_TREASURE_CLASS, snoNormalLoot);
}



static void sSpellEffect_TriuneVessel_Overpower(PowerID idPower, void *)
{
    ActorCommonData & tACD = *PowerGetOwner(idPower);

    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, tACD.GetPlace(), tExplosion);
    PowerAPI_DoExplosion(tExplosion);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}






static void sSpellEffect_Scavenger_Leap(PowerID idPower, void * pData)
{
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }










    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}

static void sSpellEffect_Lacuni_Leap_Begin(PowerID idPower, void *)
{



    Power * ptPower = PowerGet(idPower);
    ACDID idACDTarget = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        return;
    }

    ActorMovement * ptMoveTarget = ptACDTarget->GetActorMovement();
    if (!ptMoveTarget)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }

    WorldPlace placeCurr = ptACDOwner->GetPlace();
    WorldPlace placeTarget;
    float flRadius;
    ptMoveTarget->GetPlaceAndRadius(placeTarget, flRadius, FALSE);
    if (placeCurr.idSWorld != placeTarget.idSWorld)
    {
        return;
    }

    Vector3D vDir = placeCurr.wp - placeTarget.wp;
    if (vDir.MagnitudeSquared() < EPSILON * EPSILON)
    {
        return;
    }

    float flDist = vDir.Normalize();
    float flDesiredDistFromTarget = PowerGetFormulaValue(idPower, TAG_POWER_LACUNI_LEAP_TARGET_OFFSET);
    flDist = Math::Min(flDist, flDesiredDistFromTarget);

    NavCellHandle tNavCellHandleStart;
    if (!NavCellHandleFromWorldPlace(placeTarget, tNavCellHandleStart))
    {
        return;
    }

    WorldCollParams tCollParams(ptACDOwner->GetCollSettings());
    WorldCollResult tResult;
    Vector3D vEnd = vDir * flDist;
    vEnd += placeTarget.wp;
    Circle circleStart;
    circleStart.Set((const Vector2D &)placeTarget.wp, flRadius);
    WorldCollSweptCircle(placeTarget.idSWorld, tNavCellHandleStart, circleStart, (const Vector2D &)vEnd, tCollParams, 0, 0, idACDTarget, tResult);

    placeTarget.wp += vDir * (flDist * tResult.flT);
    ptPower->tTarget.InitializeFromPlace(placeTarget);
}



void sBurrowIn(PowerID idPower, WeaponClass eBurrowWeaponClass, int32 nAppearanceParameter, BOOL bAlphaAttachments)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptPower || !ptACD)
    {
        return;
    }

    if (PowerGeneralTagExists(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_BURROW_WEAPON_CLASS_OVERRIDE))
    {
        WeaponClass eWeaponClassOverride = PowerGeneralTagGetInt(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_BURROW_WEAPON_CLASS_OVERRIDE);
        if (eWeaponClassOverride != -1)
        {
            eBurrowWeaponClass = eWeaponClassOverride;
        }
    }

    BOOL bInvisibleDuring = PowerIsInvisibleDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)) || PowerGetFormulaValue(idPower, TAG_POWER_BURROW_HIDES_ACTOR) != 0.0f;
    PowerAPI_DoBurrowIn(ptACD, bInvisibleDuring, eBurrowWeaponClass, nAppearanceParameter, bAlphaAttachments);
}









static void sSpellEffect_BurrowIn(PowerID idPower, void *)
{
    sBurrowIn(idPower, 1, -1, TRUE);
}





static void sSpellEffect_BurrowOut(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPI_DoBurrowOut(ptACD, TRUE);
    }
}





static void sSpellEffect_Scavenger_Burrow_In_Start(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        sBurrowIn(idPower, -1, -1, TRUE);
    }
}







static void sSpellEffect_Scavenger_Burrow_In(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.nAppearanceParameter = 14;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ALPHA_ATTACHMENTS, TRUE);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}








static void sSpellEffect_Scavenger_Burrow_Out(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_SCAVENGER_BURROW_IN), ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
        PowerAPI_DoBurrowOut(ptACD, FALSE);
        AIStateSetState((ActorID)ptACD->idOwner, 2, FALSE);
    }
}











static void sSpellEffect_Sandshark_Burrow_In(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR, 1.0f);
        sBurrowIn(idPower, 1, -1, TRUE);
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.nAppearanceParameter = 14;
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
        AIStateSetState((ActorID)ptACD->idOwner, 4, FALSE);
    }
}









static void sSpellEffect_Sandshark_Burrow_Out(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetFloat(ATTRIBUTE_MOVEMENT_SCALAR, 0.8f);
        PowerAPI_DoBurrowOut(ptACD, FALSE);
        BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_SANDSHARK_BURROW_IN), *ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
        AIStateSetState((ActorID)ptACD->idOwner, 2, FALSE);
    }
}









static void sSpellEffect_Sandshark_UnderSand_Attack_Begin(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
}










void sSandsharkPlayChildPower(const PowerID idPower, void *)
{
    PowerForceStop(idPower, TRUE);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    ActorID idActor = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;
    BrainAPIDoIdle(idActor);
    PowerRunChild(idPower, NULL);
}







void sPlayPreBurstEffect(const PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        SNO snoPreBurstEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
        if (snoPreBurstEffect != SNO_NONE)
        {
            ACDPlayEffect(ptACD->id, EFFECT_PLAY_EFFECT_GROUP, snoPreBurstEffect, ACDID_INVALID);
        }
    }
}





static void sSpellEffect_Sandshark_UnderSand_Attack_Teleport(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }

    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (!ptMove)
    {
        return;
    }

    ActorID idActor = (ActorID)ptACD->idOwner;

    BOOL bSuccess = FALSE;

    if (!ptPower->tTarget.IsNone())
    {
        WorldPlace placeTarget;
        ptPower->tTarget.GetPlace(placeTarget);

        PowerAPIChooseOpenGround tOpenGroundInfo;
        NavPlace tNavPlace;
        PowerPlaceToNavMeshHandle(placeTarget, tNavPlace.tNavMeshHandle);
        tNavPlace.tPlace = placeTarget;
        PowerHelper_ChooseOpenGroundInit(idPower, tNavPlace, tOpenGroundInfo, 0.0f);
        NavMeshHandle hNavMeshCurr;
        bSuccess = PowerAPI_DoChooseOpenGround(*ptACD, tOpenGroundInfo, placeTarget, hNavMeshCurr);

        if (bSuccess)
        {
            ptPower->tTarget.InitializeFromPlace(placeTarget);
            ptMove->SnapToNewPlace(placeTarget, TRUE, FALSE, TRUE, 0);
        }
    }


    if (!bSuccess)
    {
        PowerForceStop(idPower, TRUE);
        return;
    }



    PowerTriggerAddTimerTrigger(idPower, 1, sPlayPreBurstEffect, NULL);
    int32 nDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SANDSHARK_DELAY_BEFORE_SAND_ATTACK) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, nDelay, sSandsharkPlayChildPower, NULL);

    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI)
    {
        uint32 dwTime = GameGetServerTime()
            + nDelay;
        ptAI->dwDelayBrainUpdateTime = dwTime; ptAI->dwDelayBrainDeadUpdateTime = dwTime;

        ptAI->dwDelayBrainActionTime = dwTime;
    }
}


static void sSpellEffect_Sandshark_UnderSand_Attack_Burst(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        PowerAPIChooseTargets tTargets;
        PowerHelper_ChooseTargetsInitAroundTargetFromPayload(idPower, tTargets);
        PowerAPI_DoChooseTargets(tTargets, listTargets);

        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    }
}








static void sSpellEffect_ZombieEatStart(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 2);
    }
}





static void sSpellEffect_ZombieEatStop(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 0);
    }
}





static void sSpellEffect_EnterSpecialState(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE, TRUE);
        ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 1);
    }
}





static void sSpellEffect_ExitSpecialState(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 0);
        ptACD->AttributesSetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE, FALSE);
    }
}





static void sSpellEffect_EmoteConversation(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        SNO snoConversation = PowerGeneralTagGetSNO(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_EMOTE_CONVERSATION_SNO);
        if (snoConversation != SNO_NONE)
        {
            ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
            if (ptACDOwner)
            {
                Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
                int32 nPlayerTalkingIndex = ptPlayer ? ptPlayer->nPlayerIndex : -1;
                ConversationMgr::StartConversation(nPlayerTalkingIndex, snoConversation, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, ACDID_INVALID, 0x7fffffff, FALSE, FALSE);
            }
        }
    }
}









static void sRemoveSuffocateBuffs(ActorCommonData & tACD)
{
    POS pos = tACD.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = (BuffID)tACD.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted)
        {
            float flSuffocationPerSecond = FastAttribGetValueFloat(ptBuff->idFastAttrib, ATTRIBUTE_SUFFOCATION_PER_SECOND);
            if (Math::Abs(flSuffocationPerSecond) > EPSILON)
            {
                // 
                PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
    }
}



static void sSuffocate_DamagingPulse(PowerAPIBuffInstance * ptBuff, void * pData)
{
    PowerID idPower = ptBuff->idPower;
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    float flSuffocationPerSecond = ptACD->AttributesGetFloat(ATTRIBUTE_SUFFOCATION_PER_SECOND);
    if (flSuffocationPerSecond > EPSILON)
    {
        float flUnitSuffocation = ptACD->AttributesGetFloat(ATTRIBUTE_SUFFOCATION_UNIT_VALUE);
        if (flUnitSuffocation < 1.0f)
        {
            uint32 uTickInterval = (uint32)(uintptr)pData;
            float flSuffocationInc = (flSuffocationPerSecond * uTickInterval) / 60.0f;
            flUnitSuffocation = Math::Min(flUnitSuffocation + flSuffocationInc, 1.0f);
            ptACD->AttributesSetFloat(ATTRIBUTE_SUFFOCATION_UNIT_VALUE, flUnitSuffocation);
        }
        // 
        // 
        if (flUnitSuffocation >= 1.0f)
        {
            // 
            // 
            // 
            PowerAPIAttackPayload tPayload;
            PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
            tPayload.eDeliveryMechanism = PDM_THORNS;
            PowerAPI_DoPayload(tPayload, ptACD->id, ACDID_INVALID);
        }
    }
}



static void sSuffocate_RestoringPulse(PowerAPIBuffInstance * ptBuff, void * pData)
{
    PowerID idPower = ptBuff->idPower;
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    float flSuffocationPerSecond = ptACD->AttributesGetFloat(ATTRIBUTE_SUFFOCATION_PER_SECOND);
    float flUnitSuffocation = ptACD->AttributesGetFloat(ATTRIBUTE_SUFFOCATION_UNIT_VALUE);
    if (flUnitSuffocation > 0.0f)
    {
        uint32 uTickInterval = (uint32)(uintptr)pData;
        float flSuffocationInc = (flSuffocationPerSecond * uTickInterval) / 60.0f;
        flUnitSuffocation = Math::Max(flUnitSuffocation + flSuffocationInc, 0.0f);
        // 
        ptACD->AttributesSetFloat(ATTRIBUTE_SUFFOCATION_UNIT_VALUE, flUnitSuffocation);
    }
    // 
    // 
    if (flUnitSuffocation <= 0.0f)
    {
        // 
        ptACD->AttributesSetFloat(ATTRIBUTE_SUFFOCATION_UNIT_VALUE, 0.0f);
        // 
        // 
        PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}



static void sSpellEffect_SuffocateAreaEnter(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD && ACDIsPlayer(ptACD->id))
    {
        sRemoveSuffocateBuffs(*ptACD);
        // 
        uint32 uTickInterval = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DEBUFF_REFRESH_INTERVAL_IN_SECONDS) * 60.0f);
        if (uTickInterval)
        {
            // 
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.bInfinite = TRUE;
            tBuffStartData.eStackType = 0;
            tBuffStartData.nMaxStackCount = 1;
            tBuffStartData.pPeriodicSpellEffect = sSuffocate_DamagingPulse;
            tBuffStartData.pPeriodicSpellEffectData = (void *)(uintptr)uTickInterval;
            tBuffStartData.dwRefreshRate = uTickInterval;
            // 
            float flSuffocationPerSecond = PowerGetFormulaValue(idPower, TAG_POWER_SUFFOCATION_PER_SECOND);
            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_SUFFOCATION_PER_SECOND, flSuffocationPerSecond);
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            float flSuffocationUnitValue = Math::Max(ptACD->AttributesGetFloat(ATTRIBUTE_SUFFOCATION_UNIT_VALUE), 0.01f);
            // 
            ptACD->AttributesSetFloat(ATTRIBUTE_SUFFOCATION_UNIT_VALUE, flSuffocationUnitValue);
        }
    }
}



static void sSpellEffect_SuffocateAreaExit(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD && ACDIsPlayer(ptACD->id))
    {
        sRemoveSuffocateBuffs(*ptACD);
        // 
        uint32 uTickInterval = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DEBUFF_REFRESH_INTERVAL_IN_SECONDS) * 60.0f);
        if (uTickInterval)
        {
            // 
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.bInfinite = TRUE;
            tBuffStartData.eStackType = 0;
            tBuffStartData.nMaxStackCount = 1;
            tBuffStartData.pPeriodicSpellEffect = sSuffocate_RestoringPulse;
            tBuffStartData.pPeriodicSpellEffectData = (void *)(uintptr)uTickInterval;
            tBuffStartData.dwRefreshRate = uTickInterval;
            // 
            float flSuffocationPerSecond = PowerGetFormulaValue(idPower, TAG_POWER_SUFFOCATION_PER_SECOND);
            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_SUFFOCATION_PER_SECOND, flSuffocationPerSecond);
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
        }
    }
}



static void sFirebreath_DamagingPulse(PowerAPIBuffInstance * ptBuff, void *)
{
    PowerID idPower = ptBuff->idPower;
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    sSpellEffect_PerformStandardDamage(idPower, NULL);
}





static void sSpellEffect_Firebreath_Start(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    Power * ptPower = PowerTryToGet(idPower);
    if (ptACD && ptPower)
    {
        uint32 uTickInterval = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DEBUFF_REFRESH_INTERVAL_IN_SECONDS) * 60.0f);
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
        tBuffStartData.nMaxStackCount = 1;
        tBuffStartData.pPeriodicSpellEffect = sFirebreath_DamagingPulse;
        tBuffStartData.pPeriodicSpellEffectData = (void *)(uintptr)uTickInterval;
        tBuffStartData.dwRefreshRate = uTickInterval;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}

void TriuneBerserkerBrainMissed(ActorID idActor);  // X/Server/Ai/TriuneBrains.cpp:548




static void sSpellEffect_Firebreath_Stop(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = PowerGetOwner(idPower);
        if (ptACD)
        {
            BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, *ptACD, ACDID_INVALID);
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }
}





static void sShieldEffect(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, float * flDamageDone, PowerAPIDamage & tPowerDamage, void * pData)
{
    if (!flDamageDone)
    {
        return;
    }
    float flAmountLeft = (float)(intptr)pData;
    float flAmountAbsorbed = 0.0f;
    for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++)
    {
        if (flDamageDone[iDamageType] > EPSILON)
        {
            if (flDamageDone[iDamageType] < flAmountLeft)
            {
                flAmountLeft -= flDamageDone[iDamageType];
                flAmountAbsorbed += flDamageDone[iDamageType];
                flDamageDone[iDamageType] = 0.0f;
            }
            else
            {
                flAmountAbsorbed += flAmountLeft;
                flDamageDone[iDamageType] -= flAmountLeft;
                flAmountLeft = 0.0f;
                break;
            }
        }
    }
    // 
    // 
    // 
    // 
    // 
    tPowerDamage.bDamageWasBlocked = TRUE;
    tPowerDamage.flDamageBlocked = flAmountAbsorbed;
    // 
    // 
    SNO snoSound = GlobalSNOGet(GLOBALSNO_SOUND_FIRESHIELD_HIT);
    if (snoSound != SNO_NONE)
    {
        // 
        ACDPlayEffect(idACDTarget, EFFECT_PLAY_SOUND, snoSound, ACDID_INVALID);
    }
    // 
    // 
    if (flAmountLeft - flAmountAbsorbed > EPSILON)
    {
        // 
        // 
        ptBuff->pDamageShieldEffectData = (void *)(intptr)(flAmountLeft - flAmountAbsorbed);
    }
    else
    {
        // 
        // 
        // 
        PowerAPI_BuffFlagForDeletion(ptBuff->id, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}

static void sSpellEffect_Shield(PowerID idPower, void * pData)
{
    // 
    // 
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        // 
        return;
    }
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.pDamageShieldEffect = sShieldEffect;
    tBuffStartData.nAppearanceParameter = 2;
    // 
    tBuffStartData.pDamageShieldEffectData = (void *)(intptr)Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SHIELD_DAMAGE_ABSORB_AMOUNT));
    PowerAPI_BuffAddToACD(tBuffStartData, *PowerGetOwner(idPower));
}




static void sBreakableShieldEffect(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, float * flDamageDone, PowerAPIDamage & tPowerDamage, void * pData)
{
    if (!flDamageDone)
    {
        return;
    }
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    if (tACDTarget.AttributesGetInt(ATTRIBUTE_STUNNED) ||
        tACDTarget.AttributesGetInt(ATTRIBUTE_FROZEN) ||
        tACDTarget.AttributesGetInt(ATTRIBUTE_FEARED) ||
        tACDTarget.AttributesGetInt(ATTRIBUTE_FORCE_GRIPPED))
    {
        return;
    }
    if (tPowerDamage.eDeliveryMechanism == 4)
    {
        return;
    }
    float flAmountLeft = tACDTarget.AttributesGetFloat(ATTRIBUTE_BREAKABLE_SHIELD_HP);
    float flAmountAbsorbed = 0.0f;
    // 
    // 
    for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++)
    {
        // 
        // 
        if (flDamageDone[iDamageType] > EPSILON)
        {
            // 
            if (flDamageDone[iDamageType] < flAmountLeft)
            {
                // 
                flAmountLeft -= flDamageDone[iDamageType];
                flAmountAbsorbed += flDamageDone[iDamageType];
                flDamageDone[iDamageType] = 0.0f;
            }
            else
            {
                // 
                // 
                flAmountAbsorbed += flAmountLeft;
                flAmountLeft = 0.0f;
                flDamageDone[iDamageType] = 0.0f;
                break;
            }
        }
    }
    // 
    // 
    // 
    // 
    // 
    // 
    tPowerDamage.bDamageWasBlocked = TRUE;
    tPowerDamage.flDamageBlocked = flAmountAbsorbed;
    ACDPlayEffect(idACDTarget, EFFECT_SHIELD_BLOCK, 0, ACDID_INVALID);
    // 
    // 
    if (flAmountLeft - flAmountAbsorbed > EPSILON && !tPowerDamage.bCriticalHit)
    {
        // 
        // 
        tACDTarget.AttributesSetFloat(ATTRIBUTE_BREAKABLE_SHIELD_HP, flAmountLeft - flAmountAbsorbed);
    }
    else
    {
        // 
        // 
        // 
        tPowerDamage.bCausesTransform = TRUE;
        PowerAPI_BuffFlagForDeletion(ptBuff->id, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        tACDTarget.AttributesSetFloat(ATTRIBUTE_BREAKABLE_SHIELD_HP, 0.0f);
        // 
        // 
        tACDTarget.ChangeDamageState(1);
        // 
        BrainAPITriggerUpdate((ActorID)ACDGetOwner(idACDTarget), TRUE);
    }
}

static void sSpellEffect_Breakable_Shield(PowerID idPower, void * pData)
{
    // 
    // 
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        // 
        return;
    }
    // 
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.pDamageShieldEffect = sBreakableShieldEffect;
    tBuffStartData.bInfinite = TRUE;
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    // 
    // 
    BrainAPIDoPlayAnimation((ActorID)ptACD->idOwner, TAG_ACTOR_ANIMATION_NEUTRAL, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
}

static void sSpellEffect_TriuneBerserkerPowerHit(PowerID idPower, void *)
{



    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }

    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        WorldPlace placeContact;
        placeContact.idSWorld = ptACDOwner->GetSWorldID();
        ptACDOwner->GetContactOffset(placeContact.wp, ptPower->dwCurrentContactFrame);

        Target tTarget;
        tTarget.InitializeFromPlace(placeContact);

        PowerAPIChooseTargets tChooseTargets;
        PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, tTarget, tChooseTargets);

        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        PowerAPI_DoChooseTargets(tChooseTargets, listTargets);


        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);


        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);


        if (listTargets.Count() == 0)
        {
            ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);
            TriuneBerserkerBrainMissed((ActorID)ACDGetOwner(ptPower->idACD));
        }
        else
        {
            BrainAPIDoPlayAnimation((ActorID)ACDGetOwner(ptPower->idACD), TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_03, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
        }
    }
}




static void sSpellEffect_ShieldSkeleton_BasicMelee(PowerID idPower, void * pData)
{
    // 
    // 
    // 
    SpellEffect_BasicMelee(idPower, pData);
    // 
    // 
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    // 
    SkeletonShieldBearerLowerShield(ptACD->id);
    // 
    BrainAPITriggerUpdate(ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID, TRUE);
}




static void sSpellEffect_Dodge(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower)
        {

            PowerAPIBuffStartData tBuffStartData(idPower);
            int32 nDuration = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DODGE_DURATION) * 60.0f);
            tBuffStartData.dwDurationMin = nDuration;

            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS, 1.0f);

            PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);

            float flAngleOffset = Math::DegToAngle(PowerGetFormulaValue(idPower, TAG_POWER_DODGE_TRAVEL_ANGLE_OFFSET));
            WorldPlace placeStart = ptACD->GetPlace();
            Angle aYaw = ACDGetYaw(ptACD->id);
            aYaw += flAngleOffset;

            float flDist = PowerGetFormulaValue(idPower, TAG_POWER_DODGE_TRAVEL_DISTANCE_MIN) +
                PowerGetFormulaValue(idPower, TAG_POWER_DODGE_TRAVEL_DISTANCE_DELTA);

            WorldPlace placeDest;
            placeDest.idSWorld = placeStart.idSWorld;
            placeDest.wp = Vector3D(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
            placeDest.wp *= flDist;
            placeDest.wp += placeStart.wp;

            float flSpeed = PowerGetFormulaValue(idPower, TAG_POWER_DODGE_TRAVEL_SPEED);

            ActorID idActor = (ActorID)ptACD->idOwner;
            Actor * ptActor = ActorTryToGet(idActor);
            if (ptActor && ptActor->ptSActorMovement)
            {
                uint32 uMoveFlags = (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING);

                ActorAnimationTag eAnimTag = PowerGeneralTagGetInt(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_ANIMATION_TAG);
                ptActor->ptSActorMovement->MoveTo(placeDest, flSpeed, 0.0f, eAnimTag, uMoveFlags, 0.0f, ACDID_INVALID, SNO_NONE, 0, -1, 0.0f, NULL);
            }
        }
    }
}



static ACDID sCorpulent_SpawnMonster(PowerID idPower, SNO snoActor, int32 nLevel, NavPlace tNavPlace, Angle angYawFacing)
{
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    Quaternion qOrientation;
    qOrientation.FromYaw(angYawFacing);
    // 
    PowerAPISummonActor tSummonInfo;
    SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
    tSummonInfo.idPower = idPower;
    tSummonInfo.snoActor = snoActor;
    // 
    tSummonInfo.nActorLevel = nLevel;
    tSummonInfo.idACDCorpse = ACDID_INVALID;
    tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    // 
    // 
    tSummonInfo.placeSummonLocation = tNavPlace.tPlace;
    tSummonInfo.qOrientation = qOrientation;
    tSummonInfo.eTeamOverride = TEAM_INVALID;
    // 
    return PowerAPI_SummonActor(tSummonInfo);
}





static void sSpellEffect_CorpulentExplodeSanityDelete(const PowerID idPower, void *)
{
    ActorCommonData * ptACDCorpulent = PowerGetOwner(idPower);
    if (ptACDCorpulent)
    {
        ptACDCorpulent->AttributesSetInt(ATTRIBUTE_INVULNERABLE, FALSE);
        ptACDCorpulent->AttributesSetInt(ATTRIBUTE_FREEZE_IMMUNE, FALSE);
        ptACDCorpulent->AttributesSetInt(ATTRIBUTE_UNTARGETABLE, FALSE);
        ptACDCorpulent->AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED, 0.0f);
    }
}









static void sSpellEffect_CorpulentExplodeBegin(PowerID idPower, void *)
{
    ActorCommonData * ptACDCorpulent = PowerGetOwner(idPower);
    if (ptACDCorpulent)
    {
        ptACDCorpulent->AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
        ptACDCorpulent->AttributesSetInt(ATTRIBUTE_FREEZE_IMMUNE, TRUE);
        ptACDCorpulent->AttributesSetInt(ATTRIBUTE_UNTARGETABLE, TRUE);
        ptACDCorpulent->AttributesSetFloat(ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED, 1.0f);
        ActorFlagSet((ActorID)ptACDCorpulent->idOwner, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    }
}








static void sCorpulentSpawnMonsters(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        int32 nCountLeft = ptACD->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
        if (nCountLeft > 0)
        {
            NavPlace tNavPlace;
            ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
            int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
            SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
            if (snoMonsterActor != SNO_NONE)
            {
                Angle angYawFacing = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, FULL_CIRCLE);
                ACDID idACDSpawnedMonster = sCorpulent_SpawnMonster(idPower, snoMonsterActor, nLevel, tNavPlace, angYawFacing);
                if (idACDSpawnedMonster != ACDID_INVALID)
                {
                    // 
                    // 
                    ActorID idSpawnedActor = (ActorID)ACDGetOwner(idACDSpawnedMonster);
                    AIStateSetState(idSpawnedActor, 4, FALSE);
                    // 
                    // 
                    // 
                }
            }
            // 
            // 
            // 
            // 
            ptACD->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nCountLeft - 1);
            // 
            PowerTriggerAddTimerTrigger(idPower, 4, sCorpulentSpawnMonsters, NULL);
        }
        else
        {
            // 
            // 
            ActorID idActor = (ActorID)ptACD->idOwner;
            ActorID idLastDamageActor = (ActorID)ptACD->AttributesGetInt(ATTRIBUTE_LAST_DAMAGE_MAINACTOR);
            if (!ActorTryToGet(idLastDamageActor))
            {
                idLastDamageActor = ACTORID_INVALID;
            }
            // 
            // 
            BrainAPIDoDeath(idActor, idLastDamageActor, FALSE, FALSE, FALSE, NULL, NULL);
            ActorFlagForDeletion(idActor);
        }
    }
}



static void sSpellEffect_CorpulentExplode(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    ActorID idActor = (ActorID)ACDGetOwner(ptPower->idACD);
    if (idActor != ACTORID_INVALID)
    {
        // 
        // 
        // 
        PowerAPIChooseTargets tChooseTargets;
        PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
        // 
        // 
        // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        // 
        // 
        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
        // 
        WorldPlace placeSource = ACDGet(ptPower->idACD).GetPlace();
        // 
        // 
        // 
        PowerAPIExplosion tExplosion;
        PowerHelper_ExplosionInit(idPower, placeSource, tExplosion);
        PowerAPI_DoExplosion(tExplosion);
        // 
        ActorCommonData & tACD = ACDGet(ptPower->idACD);
        tACD.Hide(TRUE);
        // 
        int32 nNumToSpawn = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_CORPULENT_CRYPT_KID_SPAWN_COUNT));
        tACD.AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nNumToSpawn);
        PowerTriggerAddTimerTrigger(idPower, 0, sCorpulentSpawnMonsters, NULL);
    }
}


static void sSpellEffect_CryptChildLeapOutStart(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        Target tTarget;
        PowerGetTarget(idPower, tTarget);
        NavPlace tNavPlace;
        tTarget.GetNavPlace(tNavPlace);
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_CRYPT_CHILD_LEAP_OUT_BUFF);
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.snoPower = snoBuff;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED, 1.0f);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}









static void sSpellEffect_CryptChildLeapOut(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_CRYPT_CHILD_LEAP_OUT_BUFF);
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoBuff, *ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }
}





static void sSpellEffect_Summoned(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        PowerClearDuringBuffs(ptPower, ptACD);
        ptACD->AttributesSetInt(ATTRIBUTE_AI_USED_SPAWN_ANIM, TRUE);
    }
}













static BOOL sCheckSoulSiphonLOS(PowerID idPower, ACDID idACDSource, ACDID idACDTarget)
{
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    NavPlace tNavPlaceSource;
    ACDGetNavMeshHandleWithPlace(ACDGet(idACDSource), tNavPlaceSource.tNavMeshHandle, tNavPlaceSource.tPlace);
    if (!NavMeshIsPointOnHandle(tNavPlaceSource.tNavMeshHandle, tNavPlaceSource.tPlace.wp))
    {
        return FALSE;
    }
    WorldPlace tTargetLoc = tACDTarget.GetPlace();
    if (tNavPlaceSource.tPlace.idSWorld != tTargetLoc.idSWorld)
    {
        // 
        return FALSE;
    }
    // 
    // 
    float flMaxDist = PowerGetFormulaValue(idPower, TAG_POWER_GHOST_SOULSIPHON_MAX_CHANNELLING_DISTANCE);
    float flMaxDistSqr = flMaxDist * flMaxDist;
    float flDistSqr = Vector3D::DistanceSquared(tNavPlaceSource.tPlace.wp, tTargetLoc.wp);
    if (flDistSqr > flMaxDistSqr)
    {
        // 
        return FALSE;
    }
    // 
    // 
    // 
    // 
    // 
    NavMeshRollData tRollData;
    NavMeshRollDataInitialize(tRollData);
    tRollData.pCallbackFunction = NavMeshCallBackPassabilityTypeCanEnter;
    tRollData.pCallbackData = (void *)PASSABILITY_PROJECTILE;
    tRollData.tNavMeshHandleStartCenter = tNavPlaceSource.tNavMeshHandle;
    tRollData.wpStart = tNavPlaceSource.tPlace.wp;
    tRollData.wpDest = tTargetLoc.wp;
    tRollData.dwMarbleRollFlags = 1;
    return NavMeshRollMarble(tRollData);
}








static void sSpellEffect_Ghost_SoulSiphon_Start(const PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ACDID idACDTarget = ptPower->tTarget.GetACD();
        if (idACDTarget != ACDID_INVALID)
        {
            ptPower->listACDsTracked.AddTail_Tracked(idACDTarget);
        }
        ptPower->tTarget.Clear();
    }
}







static void sSpellEffect_Ghost_SoulSiphon_Cleanup(const PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        ACDID idACDTarget = ptPower->tTarget.GetACD();
        if (idACDTarget != ACDID_INVALID)
        {
            BuffID idBuffTarget = PowerAPI_BuffFindOnACD(ptPower->snoPower, ACDTryToGet(idACDTarget), ptACD->id);
            PowerAPI_BuffFlagForDeletion(idBuffTarget, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
        // 
        // 
        BuffID idBuffSource = PowerAPI_BuffFindOnACD(ptPower->snoPower, ptACD, ptACD->id);
        PowerAPI_BuffFlagForDeletion(idBuffSource, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        // 
        PowerAbortChannelled(ptACD->id, ptPower->snoPower);
        // 
        // 
        int32 idSComplexEffect = (int32)ptPower->dwParam;
        if (idSComplexEffect != -1)
        {
            // 
            SComplexEffectRemove(ptACD->id, idSComplexEffect);
            ptPower->dwParam = (uint32)-1;
        }
        // 
        // 
        BrainAPIDoIdle((ActorID)ptACD->idOwner);
    }
}




static void sSoulSiphon_LifeDrain(PowerAPIBuffInstance * ptBuff, void *, PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID)
{
    // 
    // 
    // 
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    // 
    if (!ptACDAttacker || !ptACDAttacker->IsAlive())
    {
        // 
        // 
        return;
    }
    // 
    float flScalar = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_0);
    // 
    if (tPowerDamage.idPower == ptBuff->idPower)
    {
        // 
        ptACDAttacker->Heal(flScalar * flHitpointDamage, TRUE);
    }
}



static void sSoulSiphon_DOTStartCallback(PowerID idPower, ACDID idACDTarget, void *, const PowerAPIDamage &, PowerAPIBuffStartData & tBuffStartData)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
    if (!ptACD)
    {
        return;
    }
    BOOL bApplySlow = TRUE;
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = (BuffID)ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted && ptBuff->snoPower == tBuffStartData.snoPower)
        {
            // 
            float flMovementScalar = FastAttribGetValueFloat(ptBuff->idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR);
            if (flMovementScalar < -EPSILON)
            {
                // 
                // 
                bApplySlow = FALSE;
                break;
            }
        }
    }
    // 
    // 
    if (bApplySlow)
    {
        // 
        float flSlowMult = PowerGetFormulaValue(idPower, TAG_POWER_GHOST_SOULSIPHON_SLOW_MULTIPLIER);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flSlowMult);
    }
    // 
    // 
    tBuffStartData.pOnGetHitEffect = sSoulSiphon_LifeDrain;
}



static void sSpellEffect_Ghost_SoulSiphon_Pulse(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    BOOL bAbort = TRUE;
    float flMaxTime = PowerGetFormulaValue(idPower, TAG_POWER_GHOST_SOULSIPHON_MAX_CHANNELLING_TIME);
    int32 nMaxTicks = Math::FloatToInt(flMaxTime * 60.0f);
    int32 nElapsedTicks = GameGetServerTime() - ptPower->dwStartTick;
    ACDID idACDTarget = (ptPower->listACDsTracked.Count() > 0) ? (ACDID)ptPower->listACDsTracked.GetHead() : ptPower->tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    if (nElapsedTicks < nMaxTicks && ptACDTarget && ptACDTarget->IsAlive() && sCheckSoulSiphonLOS(idPower, ptACD->id, idACDTarget))
    {
        // 
        // 
        // 
        // 
        // 
        SNO snoEffect = PowerEffectTagGetSNO(idPower, TAG_POWER_SOURCE_DEST_EFFECT_GROUP);
        if (snoEffect != SNO_NONE)
        {
            // 
            // 
            // 
            // 
            // 
            if (ptPower->dwParam == (uint32)-1)
            {
                // 
                ptPower->dwParam = SComplexEffectAdd(*ptACD, snoEffect, idACDTarget, 0, 0, 0, FALSE);
            }
            // 
            // 
            float flDuration = 0.5f;
            float flDPS = PowerGetFormulaValue(idPower, TAG_POWER_GHOST_SOULSIPHON_DAMAGE_PER_SECOND);
            // 
            // 
            // 
            // 
            PowerAPIAttackPayload tPayload;
            PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
            tPayload.arCombatDamage[DAMAGE_TYPE_ARCANE].flMin = flDPS * flDuration;
            tPayload.flDamageDurationInSecs[DAMAGE_TYPE_ARCANE] = flDuration;
            tPayload.pDOTBuffStartCallback = sSoulSiphon_DOTStartCallback;
            PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
            // 
            // 
            PowerAPIBuffStartData tBuffStartDataSource(idPower);
            tBuffStartDataSource.dwDurationMin = 30;
            tBuffStartDataSource.eStackType = 2;
            tBuffStartDataSource.nMaxStackCount = 0x10000;
            // 
            FastAttribSetValueInt(tBuffStartDataSource.idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, TRUE);
            PowerAPI_BuffAddToACD(tBuffStartDataSource, *ptACD);
            // 
            PowerTriggerAddTimerTrigger(idPower, 20, sSpellEffect_Ghost_SoulSiphon_Pulse, NULL);
            bAbort = FALSE;
        }
    }
    // 
    // 
    // 
    if (bAbort)
    {
        // 
        sSpellEffect_Ghost_SoulSiphon_Cleanup(idPower, NULL);
    }
}


static void sSpellEffect_Ghost_SoulSiphon(PowerID idPower, void *)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        // 
        return;
    }
    // 
    // 
    // 
    ptPower->dwParam = (uint32)-1;
    sSpellEffect_Ghost_SoulSiphon_Pulse(idPower, NULL);
}

static void sSpellEffect_Ghost_WalkThroughWalls_Callback(PowerAPIBuffInstance * ptBuff, void * pData)
{


    PowerID idPower = ptBuff->idPower;
    Power * ptPower = PowerGet(idPower);
    if (ptPower == NULL)
    {
        return;
    }

    ACDID idACD = ptPower->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL || !ptACD->IsAlive())
    {
        return;
    }



    BOOL bOffMesh = FALSE;


    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(*ptACD, tNavMeshHandle);

    if (!NavMeshHandleIsNone(tNavMeshHandle))
    {
        bOffMesh = !NavMeshHandleGetStaticFlag(tNavMeshHandle, 0) &&
                   !NavMeshHandleGetStaticFlag(tNavMeshHandle, 1);
    }


    BOOL bWasOffMesh = (BOOL)(intptr)pData;
    if (bOffMesh == bWasOffMesh)
        return;
    ptBuff->pPeriodicSpellEffectData = (void *)(intptr)bOffMesh;

    ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE), bOffMesh);
    ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_UNTARGETABLE), bOffMesh);
    ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_HIDDEN), bOffMesh);



    if (bOffMesh)
    {
        PowerAPI_FreeFromRoot(idACD);
        PowerAPI_FreeFromSnare(idACD);
        PowerAPI_FreeFromStun(idACD);
        PowerAPI_FreeFromFear(idACD);
    }

    SNO snoEffectGroup = PowerGeneralTagGetSNO(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_GHOST_WALLWALK_EFFECT_SNO);
    if (snoEffectGroup != SNO_NONE)
    {
        ACDPlayEffect(ptACD->id, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup, ACDID_INVALID);
    }
    else
    {
        ACDPlayEffect(ptACD->id, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECTGROUP_GHOST_WALLWALK), ACDID_INVALID);
    }
}
static const uint32 GHOST_WALKTHROUGHWALLS_REFRESH_RATE = 6;

static void sSpellEffect_Ghost_WalkThroughWalls(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower == NULL)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner == NULL)
    {
        return;
    }





    ptACDOwner->tCollSettings.uStaticOneRequiredFlags = 0x840;
    ptACDOwner->tCollSettings.uDynamicFlagsNoOverlap = 0x108;
    ptACDOwner->ModifiedMark(ACDMODIFIED_COLL_FLAGS);


    PowerAPIBuffStartData tBuffStartDataTarget(idPower);
    tBuffStartDataTarget.bInfinite = TRUE;
    tBuffStartDataTarget.bPositiveEffect = TRUE;
    tBuffStartDataTarget.eStackType = (BuffStackType)0;
    tBuffStartDataTarget.pPeriodicSpellEffect = sSpellEffect_Ghost_WalkThroughWalls_Callback; tBuffStartDataTarget.pPeriodicSpellEffectData = NULL;

    tBuffStartDataTarget.dwRefreshRate = GHOST_WALKTHROUGHWALLS_REFRESH_RATE;

    PowerAPI_BuffAddToACD(tBuffStartDataTarget, *ptACDOwner);
}





void sSpellEffect_BogBlight_PreBurstGo(const PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD && ptACD->IsAlive())
    {
        PowerRunChild(idPower, NULL);
    }
}








static void sSpellEffect_BogBlight_BurstOut(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPI_DoBurrowOut(ptACD, FALSE);
        ActorID idActor = (ActorID)ptACD->idOwner;
        AIStateSetState(idActor, 2, FALSE);
        PowerRunChild(idPower, NULL);
    }
}






static void sSpellEffect_BogBlight_Burrow_In(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    sBurrowIn(idPower, -1, -1, TRUE);
    AnimPlayerSetIdleAnimation(*ptACD, TAG_ACTOR_ANIMATION_SPAWN_2);
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ALPHA_ATTACHMENTS, TRUE);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}

















void sSpellEffect_BogBlight_PreBurst(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    ActorID idActor = ptACD ? (ActorID)ptACD->idOwner : ACTORID_INVALID;



    if (ptPower->tTarget.IsNone())
    {
        return;
    }



    ptACD->Hide(TRUE);

    WorldPlace tTargetPlace;

    ptPower->tTarget.GetPlace(tTargetPlace);
    ActorSnapToPosition(idActor, tTargetPlace, TRUE, TRUE);
    float flMinDist = PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_BURST_OUT_DISTANCE_MIN);
    float flMaxDist = PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_BURST_OUT_DISTANCE_MAX);
    WorldPlace tNewPlace = AIStateGetRandomLOSPlace(ptACD->id, flMinDist, flMaxDist);
    ActorSnapToPosition(idActor, tNewPlace, TRUE, TRUE);


    Vector3D vToTarget =
        tTargetPlace.wp - tNewPlace.wp;
    Angle aYaw = Math::ATan(vToTarget.y, vToTarget.x);
    Quaternion qOrientation;
    qOrientation.FromYaw(aYaw);
    ActorMovement * ptMove = ptACD->GetActorMovement();

    ptMove->SetNewFacing(aYaw, FALSE, TRUE);
    ActorSetOrientation(idActor, qOrientation, TRUE);

    float flDelay = PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_BURST_OUT_DELAY);
    int32 nTickDelay = Math::FloatToInt(flDelay * 60.0f);


    ACDID idACDProxy = sSpawnMorluCasterMeteorProxy(idPower, ptACD->GetPlace(), GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY), 180);
    SNO snoPreBurstEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
    if (snoPreBurstEffect != SNO_NONE)
    {
        ACDPlayEffect(idACDProxy, EFFECT_PLAY_EFFECT_GROUP, snoPreBurstEffect, ACDID_INVALID);
    }

    PowerTriggerAddTimerTrigger(idPower, nTickDelay, sSpellEffect_BogBlight_PreBurstGo, NULL);
}



static void sSpellEffect_BogBlight_Burrow_Out(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        AnimPlayerSetIdleAnimation(*ptACD, 0x11000);
        BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_BOGBLIGHT_BURROW_IN), ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
        PowerAPI_DoBurrowOut(ptACD, FALSE);
    }
}













static void sSpellEffect_Rockworm_HideIdle(PowerID idPower, void *)
{
    sBurrowIn(idPower, 1, -1, TRUE);
}





static void sSpellEffect_Rockworm_BurstOut(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPI_DoBurrowOut(ptACD, FALSE);
        ActorID idActor = (ActorID)ptACD->idOwner;
        AIStateSetState(idActor, 2, FALSE);
    }
}






void sSpellEffect_Rockworm_PreBurstGo(const PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD && ptACD->IsAlive())
    {
        PowerRunChild(idPower, NULL);
    }
}











void sSpellEffect_Rockworm_PreBurst(PowerID idPower, void *)
{





    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    ActorID idActor = (ActorID)ptACD->idOwner;


    if (ptPower->tTarget.IsNone())
    {
        return;
    }

    ptACD->Hide(FALSE);

    WorldPlace tTargetPlace;

    ptPower->tTarget.GetPlace(tTargetPlace);
    float flMinDist = PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_BURST_OUT_DISTANCE_MIN);
    float flMaxDist = PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_BURST_OUT_DISTANCE_MAX);

    Vector3D wvTargetOffset(RandRangeFloat_Internal(SGameGlobalsGet().rng, EPSILON, 1.0f),
        RandRangeFloat_Internal(SGameGlobalsGet().rng, EPSILON, 1.0f),
        0.0f);

    wvTargetOffset.Normalize();
    wvTargetOffset *= RandRangeFloat_Internal(SGameGlobalsGet().rng, flMinDist, flMaxDist);

    tTargetPlace.wp += wvTargetOffset;

    WorldPlace tNewPlace;
    PowerComputeDestination(ptPower->snoPower, *ptACD->GetActorMovement(), tTargetPlace, tNewPlace, FALSE, 10.0f);

    ActorSnapToPosition(idActor, tNewPlace, TRUE, TRUE);


    Vector3D vToTarget =
        tTargetPlace.wp - tNewPlace.wp;
    Angle aYaw = Math::ATan(vToTarget.y, vToTarget.x);
    Quaternion qOrientation;
    qOrientation.FromYaw(aYaw);
    ActorMovement * ptMove = ptACD->GetActorMovement();

    ptMove->SetNewFacing(aYaw, FALSE, TRUE);
    ActorSetOrientation(idActor, qOrientation, TRUE);

    float flDelay = PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_BURST_OUT_DELAY);
    int32 nTickDelay = Math::FloatToInt(flDelay * 60.0f);



    PowerTriggerAddTimerTrigger(idPower, 1, sPlayPreBurstEffect, NULL);

    PowerTriggerAddTimerTrigger(idPower, nTickDelay, sSpellEffect_Rockworm_PreBurstGo, NULL);
}



static void sSpellEffect_Rockworm_Retreat(PowerID idPower, void *)
{
    sBurrowIn(idPower, 1, -1, TRUE);
}


BOOL NavMeshGetHeightAndNormal(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp, const BOOL fPinToSquareBounds, WorldDistance & flHeight, Vector3D & wvNormal);  // X/Common/Nav/NavigationMesh.cpp:783


static void sSpellEffect_Rockworm_SpitWeb(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    PowerAPISummonActor tSummonInfo;
    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
    if (snoMonsterActor == SNO_NONE)
    {
        return;
    }

    BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, FALSE);
    if (!bSuccess)
    {
        return;
    }


    WorldPlace placeACD = ptACD->GetPlace();
    NavCellHandle hNavCellCenter;
    NavCellHandleFromWorldPlace(placeACD, hNavCellCenter);

    PassabilityType ePassabilityType = PASSABILITY_MONSTER;
    WorldCollParams tCollParams;
    tCollParams.uOneRequiredStaticFlags = NavMeshGetStaticFlagsForPassability(ePassabilityType);
    tCollParams.uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(ePassabilityType);
    tCollParams.eTeam = TEAM_NEUTRAL;

    Angle aYaw = ACDGetYaw(ptACD->id);
    Vector3D vTargetPos(
        Math::Cos(aYaw),
        Math::Sin(aYaw),
        0.0f);
    vTargetPos *= PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_WEB_SPIT_DISTANCE);
    vTargetPos += placeACD.wp;

    Circle wcStart;
    wcStart.Set((const Vector2D &)placeACD.wp, 5.0f);
    WorldCollResult tResult;
    WorldCollSweptCircle(placeACD.idSWorld,
                         hNavCellCenter,
                         wcStart,
                         (const Vector2D &)vTargetPos,
                         tCollParams,
                         3,
                         1,

                         ptACD->id,
                         tResult);

    tSummonInfo.placeSummonLocation.idSWorld = placeACD.idSWorld;
    tSummonInfo.placeSummonLocation.wp = tResult.wpEnd;

    NavMeshHandle tNavMeshHandle;
    Vector3D vNormal;
    ScenesCommonNavMeshNodeFromPlace(tSummonInfo.placeSummonLocation, &tNavMeshHandle, FALSE, FALSE);
    NavMeshGetHeightAndNormal(tNavMeshHandle, tSummonInfo.placeSummonLocation.wp, FALSE, tSummonInfo.placeSummonLocation.wp.z, vNormal);

    tSummonInfo.dwAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK);
    tSummonInfo.dwAddFlags |= (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);

    ACDID idACDWeb = PowerAPI_SummonActor(tSummonInfo);
    if (idACDWeb == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDWeb = ACDGet(idACDWeb);
    ActorID idActorWeb = (ActorID)ACDGetOwner(idACDWeb);


    tACDWeb.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDWeb.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    tACDWeb.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
    ActorFlagSet(idActorWeb, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    tACDWeb.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);

    uint32 dwDuration = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_ROCKWORM_WEB_DURATION) * 60.0f);
    ActorDeleteSoon(idActorWeb, dwDuration);


    SNO snoRope = PowerEffectTagGetSNO(idPower, TAG_POWER_ROPE_SNO);
    if (snoRope != SNO_NONE)
    {
        ACDPlayEffectRopeACDToACD(snoRope,

            ptACD->id, 2, idACDWeb, 0, FALSE);
    }




    PowerID idPowerWeb = PowerStart(idACDWeb, GlobalSNOGet(GLOBALSNO_POWERS_ROCKWORM_WEB), cgTargetNone);
}

#include "../SWorld.h"
#include "../SScene.h"
#include "../../Common/Definitions/ActDefinition.h"
#include "../../Common/GameSwig.h"
#include "../SGame.h"

static void sSpellEffect_WebSlowPulse(PowerID idPower, void *)
{
    // 
    // 
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }
    // 
    // 
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        // 
        // 
        PowerAPIBuffStartData tBuffStartDataTarget(idPower);
        tBuffStartDataTarget.dwDurationMin = 30;
        tBuffStartDataTarget.eStackType = BUFF_STACKTYPE_NEVER;
        // 
        // 
        float flSlowMult = PowerGetFormulaValue(idPower, TAG_POWER_SLOW_MULTIPLIER);
        FastAttribSetValueFloat(tBuffStartDataTarget.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flSlowMult);
        PowerAPI_BuffAddToACD(tBuffStartDataTarget, ACDGet(idACDTarget));
    }
    // 
    PowerTriggerAddTimerTrigger(idPower, 20, sSpellEffect_WebSlowPulse, NULL);
}
static void sSpellEffect_RootStruggle(PowerID idPower, void *)
{


    if (idPower == POWERID_INVALID)
    {
        return;
    }

    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        float flUnitModify = PowerGetFormulaValue(idPower, TAG_POWER_ROOT_TIMER_MODIFICATION_PER_STRUGGLE);
        flUnitModify = Math::Pin(flUnitModify, 0.0f, 1.0f);

        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos && ptACD->listBuffs.Count() > 0 && ptACD->IsAlive())
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (!ptBuff || ptBuff->bDeleted)
                continue;
            if (FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_IS_ROOTED))
            {
                ACDID idACDRooter = ptBuff->idACDSource;
                ActorCommonData * ptACDRooter = ACDTryToGet(idACDRooter);
                if (ptACDRooter)
                {
                    float flUnit;


                    int nTotalTicks = FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_ROOT_TOTAL_TICKS);
                    if (nTotalTicks > 0)
                    {

                        uint32 uNow = GameGetServerTime();
                        int32 nTimeLeft = ptBuff->dwDuration - uNow + ptBuff->dwStartTick;
                        flUnit = (float)nTimeLeft / (float)nTotalTicks;
                    }
                    else
                    {

                        flUnit = ptACDRooter->AttributesGetFloat(ATTRIBUTE_ROOT_UNIT_VALUE);
                    }

                    flUnit = Math::Pin(flUnit - flUnitModify, 0.0f, 1.0f);
                    ptACDRooter->AttributesSetFloat(ATTRIBUTE_ROOT_UNIT_VALUE, flUnit);

                    if (flUnit <= 1e-6f)
                    {
                        PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
                    }


                    else if (nTotalTicks > 0)
                    {
                        int32 nDesiredTicks = Math::FloatRoundToInt(flUnit * nTotalTicks);
                        uint32 uNow = GameGetServerTime();
                        int32 nElapsed = uNow - ptBuff->dwStartTick;
                        int32 nDurationDelta = nDesiredTicks + nElapsed - ptBuff->dwDuration;
                        PowerAPI_BuffModifyDuration(idBuff, nDurationDelta, FALSE, TRUE);
                    }


                    ACDPlayHitEffect(idACDRooter, ptACD->id, 0, FALSE);
                }
            }
        }
    }
}









// X/Server/Actors/Actors.h, X/Server/SGizmo.h, X/Server/SWorld.h, X/Common/SceneCommon.h, X/Server/SPlayer.h, X/Common/World.h, X/Common/Act.h, X/Server/SGame.h, X/Server/TownPortal.h
void ActorChangeWorld(const ActorID idActor, const WorldPlace & tPlace, const Quaternion & q);  // Actors.cpp:6093
void GizmoSetHearthPortal(Player & tPlayer, const WorldPlace & placeReturn, const BOOL bSendToClient);  // SGizmo.cpp:3484
SSceneID SWorldFindFirstSceneInWorld(SWorldID idSWorld, SNO snoScene);  // SWorld.cpp:2142
void SceneCommonDataActivate(const SSceneID idSScene);  // SceneCommon.cpp:729
BOOL SPlayerIsInSetDungeon(const ActorCommonData & tACDPlayer);  // SPlayer.cpp:14244
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // World.cpp:605
SNO ActGetCurrentHubWorldSNO();  // Act.cpp:372
BOOL SPlayerInScriptedSequence(const Player & tPlayer);  // SPlayer.cpp:10849
void SGameGetOpenWorldHubWorldForPlayer(const Player & tPlayer, ResolvedPortalDestination & tDest);  // SGame.cpp:6756
void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest);  // Act.cpp:260
void SPlayerSetLastWaypointLevelArea(Player * ptPlayer, const SNO snoLevelArea);  // SPlayer.cpp:12427
void TownPortalsDeleteForPlayer(Player & tPlayer);  // TownPortal.cpp:450
















static void sSpellEffect_RecallEnterBeginPortalWalk(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        // 
        // 
        // 
        // 
        return;
    }
    PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
    // 
    // 
    WorldPlace wpTarget = ptACDOwner->GetPlace();
    wpTarget.wp.x += PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
    wpTarget.wp.y += PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
    // 
    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_EXIT_STONE_OF_RECALL);
    // 
    Target tTarget;
    tTarget.InitializeFromPlace(wpTarget);
    // 
    ActorID idActorUser = (ActorID)ACDGetOwner(ptACDOwner->id);
    BrainStanceDoPower(idActorUser, tTarget, snoPower, BRAIN_POWER_START, NULL, NULL, 0, 0);
}




static void sSpellEffect_RecallEnterShowPortal(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }
    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
        return;
    }
    // 
    ACDID idACDPortal = ACDID_INVALID;
    // 
    WorldPlace wpPlayer = ptACDOwner->GetPlace();
    if (wpPlayer.idSWorld != SWORLDID_INVALID)
    {
        // 
        NavMeshHandle tNavMeshHandle;
        if (ScenesCommonNavMeshNodeFromPlace(wpPlayer, &tNavMeshHandle, TRUE, FALSE))
        {
            // 
            idACDPortal = SSceneFindFirstGizmo(tNavMeshHandle.idSScene, ACTORGIZMO_HEARTH_PORTAL);
        }
    }
    // 
    // 
    // 
    // 
    GizmoSendHearthPortalToPlayer(*ptPlayer, idACDPortal);
    // 
    int32 uTicksUntilSpecialWalk = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, uTicksUntilSpecialWalk, sSpellEffect_RecallEnterBeginPortalWalk, NULL);
}






static void sSpellEffect_RecallEnterOnEnd(PowerID idPower, void * pData)
{




    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
        return;
    }


    if (SPlayerInScriptedSequence(*ptPlayer))
    {
        PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
        return;
    }

    SNO snoHub;

    if (GameIsOpenWorld())
    {
        ResolvedPortalDestination tDest;
        SGameGetOpenWorldHubWorldForPlayer(*ptPlayer, tDest);
        snoHub = tDest.snoWorld;
    }
    else if (GameIsWeeklyChallenge())
    {
        snoHub = SGameGetChallengeRiftHubWorld();
    }
    else
    {
        snoHub = ActGetCurrentHubWorldSNO();
    }

    if (snoHub == SNO_NONE)
    {
        PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
        return;
    }





    SWorldID idSWorldHub = WorldCommonFindBySNO(snoHub, TRUE);

    if (idSWorldHub == SWORLDID_INVALID)
    {
        idSWorldHub = SWorldCreate(snoHub, TRUE, SPlayerIsLocal(*ptPlayer), NULL, ACDID_INVALID, ACDID_INVALID, 0, 0, 0, 0);


        if (idSWorldHub == SWORLDID_INVALID)
        {
            const WorldLocation & loc = ptACDOwner->GetWorldLocation();
            const SScene * tSScene = SSceneGet(loc.idSScene);
            const SWorld * tSWorld = SWorldGet(loc.idSWorld);







            PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
            return;
        }
    }

    SWorld * ptSWorld = SWorldGet(idSWorldHub);

    SSceneID idSSceneDest = SWorldFindFirstSceneInWorld(ptSWorld->id, ptSWorld->tTownPortalLocation.snoScene);

    if (ptSWorld->tTownPortalLocation.snoScene == SNO_NONE)
    {
        const WorldLocation & loc = ptACDOwner->GetWorldLocation();
        const SScene * tSScene = SSceneGet(loc.idSScene);
        const SWorld * tSWorld = SWorldGet(loc.idSWorld);







        PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
        return;
    }


    if (idSSceneDest == SSCENEID_INVALID)
    {
        const WorldLocation & loc = ptACDOwner->GetWorldLocation();
        const SScene * tSScene = SSceneGet(loc.idSScene);
        const SWorld * tSWorld = SWorldGet(loc.idSWorld);

        ResolvedPortalDestination tDest;
        ActGetCurrentStartLocation(tDest);









        PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
        return;
    }

    SceneCommonDataActivate(idSSceneDest);

    WorldPlace placeOld;

    BOOL bIsSetDungeon = SPlayerIsInSetDungeon(*ptACDOwner);
    if (!bIsSetDungeon)
    {
        placeOld = ptACDOwner->GetPlace();
    }
    else
    {
        SWorld * ptSetDungeon = SWorldGet(ptACDOwner->GetPlace().idSWorld);
        placeOld = ptSetDungeon->tReturnPortalLocation.placeReturn;
    }

    Quaternion qOrientation = ptACDOwner->GetOrientation();
    GizmoSetHearthPortal(*ptPlayer, placeOld, FALSE);


    WorldPlace placePlayer = ptACDOwner->GetPlace();

    ActorID idActorUser = (ActorID)ACDGetOwner(ptACDOwner->id);
    ActorChangeWorld(idActorUser, ptSWorld->tTownPortalLocation.wpPortal, qOrientation);

    SPlayerWarped(*ptACDOwner, WARP_HEARTH_PORTAL, &placePlayer, ACDID_INVALID, 0.0f, TRUE);

    ScriptEventOnUserEvent("StoneOfRecallUsed", ptACDOwner->id, 0);

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.nBuffIndex = 0;

    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS, 1.0f);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);

    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);

    int32 uTicksUntilPortalShow = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, uTicksUntilPortalShow, sSpellEffect_RecallEnterShowPortal, NULL);


    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
    {
        if (PlayersAreLocalToEachOther(*ptPlayer, *ptOtherPlayer) && ptPlayer->nPlayerIndex != ptOtherPlayer->nPlayerIndex)
        {
            ActorCommonData * ptOtherACD = ACDTryToGet(ptOtherPlayer->idPlayerACD);
            if (ptOtherACD)
            {


                ActorID idActorUserScoped = (ActorID)ACDGetOwner(ptOtherACD->id);

                PowerID idPowerPrimary = ActorPowersGetPrimary(idActorUserScoped);
                Power * ptPowerPrimary = PowerTryToGet(idPowerPrimary);
                if (!ptPowerPrimary || ptPowerPrimary->snoPower != GlobalSNOGet(GLOBALSNO_POWERS_ENTER_STONE_OF_RECALL))
                {



                    GizmoSetHearthPortal(*ptOtherPlayer, placeOld, FALSE);

                    qOrientation = ptOtherACD->GetOrientation();


                    ActorChangeWorld(idActorUserScoped, ptSWorld->tTownPortalLocation.wpPortal, qOrientation);

                    SPlayerWarped(*ptOtherACD, WARP_HEARTH_PORTAL, &placePlayer, ACDID_INVALID, 0.0f, TRUE);

                    ScriptEventOnUserEvent("StoneOfRecallUsed", ptOtherACD->id, 0);
                }
            }
        }
    }

}





static void sSpellEffect_RecallEnterOnStart(PowerID idPower, void * pData)
{




    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }

    if (SPlayerInScriptedSequence(*ptPlayer))
    {
        return;
    }


    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.nBuffIndex = 0;

    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS, 1.0f);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, TRUE);

    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);


    SNO snoProxyActor = GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY);

    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = snoProxyActor;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
    tActorAdd.eFastAttribType = FAT_SIMPLE;
    tActorAdd.eTeam = PowerGetTeam(idPower);
    tActorAdd.tPlace = ptACDOwner->GetPlace();
    tActorAdd.qOrientation = ptACDOwner->GetOrientation();

    ScopedActorDefinitionLockR pdefActor(snoProxyActor);
    float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
    float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);

    tActorAdd.flScale = RandRangeFloat_Internal(SGameGlobalsGet().rng, flMinScale, flDeltaScale);
    tActorAdd.tSNOName = SNOName(ACTOR_SNO, snoProxyActor);


    ActorID idActorProxy = ActorAdd(tActorAdd);
    if (idActorProxy == ACTORID_INVALID)
    {
        return;
    }

    ACDID idACDProxy = ACDFindByActor(idActorProxy);
    if (idACDProxy == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDProxy = ACDGet(idACDProxy);
    tACDProxy.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
    ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_IS_POWER_PROXY, TRUE);


    uint32 dwDuration = 120;
    ActorDeleteSoon(idActorProxy, dwDuration);

    SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
    if (snoEffect != SNO_NONE)
    {
        ACDPlayEffect(idACDProxy, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
    }

    int32 uTicksUntilWorldSwitch = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, uTicksUntilWorldSwitch, sSpellEffect_RecallEnterOnEnd, NULL);


    PlayerIndexMessage tMessage;
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, 0, -1);
    MessageSendToClient(listDestPlayers, SMSG_TOWN_PORTAL_USED, &tMessage, sizeof(tMessage));
}


static void sSpellEffect_RecallStart(PowerID idPower, void * pData)
{
    Power * ptPower = PowerGet(idPower);
    ACDID idACDItem = (ACDID)(intptr)pData;
    ptPower->dwParam = idACDItem;
    PowerHelper_DoFirstTimeCosts(ptPower->id, TRUE);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        ScriptEventOnPlayerTownPortal(ptACDOwner->id);
    }
    STrickleTriggerUpdate();
}









static void sSpellEffect_RecallInterrupted(PowerID, void *)
{
    STrickleTriggerUpdate();
}







static void sSpellEffect_RecallExitOnStart(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
        tBuffStartData.dwDurationMin = (int32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}





static void sSpellEffect_Recall(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACDUser = PowerGetOwner(idPower);
    if (!ptACDUser)
    {
        // 
        return;
    }
    Player * ptOriginalPlayer = PlayerGetByACD(ptACDUser->id);
    if (!ptOriginalPlayer)
    {
        // 
        // 
        // 
        // 
        return;
    }
    if (!ptACDUser->IsAlive())
    {
        // 
        // 
        // 
        return;
    }
    AIStateActivateCooldown(*ptPower);
    // 
    // 
    // 
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayersAreLocalToEachOther(*ptOriginalPlayer, *ptPlayer))
        {
            // 
            // 
            // 
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            // 
            if (!ptACDPlayer->IsAlive())
            {
                BrainAPIResurrectByType(ptPlayer->idPlayerACD, 4);
            }
            // 
            PowerStartData tPowerStartData;
            PowerStartDataInit(tPowerStartData);
            tPowerStartData.idACD = ptACDPlayer->id;
            tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ENTER_STONE_OF_RECALL);
            PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
        }
    }
}

const int32 RECALL_WAIT_FOR_LOADING_TICK_INTERVAL = 5;


static void sSpellEffect_RecallCheckLoaded(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        // 
        // 
        return;
    }
    if (ptACDOwner->IsLoading())
    {
        // 
        PowerTriggerAddTimerTrigger(idPower, RECALL_WAIT_FOR_LOADING_TICK_INTERVAL, sSpellEffect_RecallCheckLoaded, NULL);
        return;
    }
    // 
    // 
    // 
    PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
    // 
    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = ptACDOwner->id;
    tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_EXIT_RECALL_PORTAL);
    PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
}



static void sSpellEffect_RecallPortalEnterOnEnd(PowerID idPower, void *)
{


    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }


    if (WorldTryGetCommonData(ptPlayer->tHearthPortalInfo.place.idSWorld))
    {
        WorldPlace placePlayerBeforeWarp = ptACDOwner->GetPlace();
        ActorID idActorPlayer = (ActorID)ACDGetOwner(ptACDOwner->id);


        WorldPlace placePlayer = ptACDOwner->GetPlace();


        NavMeshHandle tNavMeshHandle;
        ScenesCommonNavMeshNodeFromPlace(ptPlayer->tHearthPortalInfo.place, &tNavMeshHandle, TRUE, FALSE);
        ActorChangeWorld(idActorPlayer, ptPlayer->tHearthPortalInfo.place, ptACDOwner->GetOrientation());

        SPlayerWarped(*ptACDOwner, WARP_HEARTH_PORTAL, &placePlayer, ACDID_INVALID, 0.0f, TRUE);
        SPlayerSetLastWaypointLevelArea(ptPlayer, ptPlayer->tHearthPortalInfo.snoLastWaypointLevelArea);
        TownPortalsDeleteForPlayer(*ptPlayer);


        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.nBuffIndex = 0;

        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS, 1.0f);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);

        PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);

        PowerTriggerAddTimerTrigger(idPower, RECALL_WAIT_FOR_LOADING_TICK_INTERVAL, sSpellEffect_RecallCheckLoaded, NULL);
    }
    else
    {
        TownPortalsDeleteForPlayer(*ptPlayer);
        PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, 0, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
    }
}




static void sSpellEffect_RecallPortalEnterOnStart(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
    {
        return;
    }



    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }


    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.nBuffIndex = 0;

    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS, 1.0f);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);

    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);


    SNO snoProxyActor = GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY);

    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = snoProxyActor;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
    tActorAdd.eFastAttribType = FAT_SIMPLE;
    tActorAdd.eTeam = PowerGetTeam(idPower);
    tActorAdd.tPlace = ptACDOwner->GetPlace();
    tActorAdd.qOrientation = ptACDOwner->GetOrientation();

    ScopedActorDefinitionLockR pdefActor(snoProxyActor);
    float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
    float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);

    tActorAdd.flScale = RandRangeFloat_Internal(SGameGlobalsGet().rng, flMinScale, flDeltaScale);
    tActorAdd.tSNOName = SNOName(ACTOR_SNO, snoProxyActor);


    ActorID idActorProxy = ActorAdd(tActorAdd);
    if (idActorProxy == ACTORID_INVALID)
    {
        return;
    }

    ACDID idACDProxy = ACDFindByActor(idActorProxy);
    if (idACDProxy == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDProxy = ACDGet(idACDProxy);
    tACDProxy.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
    ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_IS_POWER_PROXY, TRUE);


    uint32 dwDuration = 120;
    ActorDeleteSoon(idActorProxy, dwDuration);

    SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
    if (snoEffect != SNO_NONE)
    {
        ACDPlayEffect(idACDProxy, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
    }

    uint32 uTicksUntilWorldSwitch = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, uTicksUntilWorldSwitch, sSpellEffect_RecallPortalEnterOnEnd, NULL);
}



static void sSpellEffect_RecallExitCheckLoaded(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            if (ptACDOwner->IsLoading())
            {
                PowerTriggerAddTimerTrigger(idPower, 5, sSpellEffect_RecallExitCheckLoaded, NULL);
                return;
            }
            SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
            if (snoEffect != SNO_NONE)
            {
                ACDPlayEffect(ptACDOwner->id, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
            }
        }
    }
}












static void sSpellEffect_RecalPortalExitPlayArrivalEffect(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            PowerTriggerAddTimerTrigger(idPower, 5, sSpellEffect_RecallExitCheckLoaded, NULL);
        }
    }
}









static void sSpellEffect_RecallPortalExitOnStart(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
            if (ptPlayer)
            {
                PowerAPIBuffStartData tBuffStartData(idPower);
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, TRUE);
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
                uint32 dwDuration = (int32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
                tBuffStartData.dwDurationMin = dwDuration;
                PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
                PowerTriggerAddTimerTrigger(idPower, dwDuration, sSpellEffect_RecalPortalExitPlayArrivalEffect, NULL);
            }
        }
    }
}













static void sSpellEffect_SwitchingSkill(PowerID idPower, void *)
{
}








static void sSpellEffect_DungeonStone(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        Target tTarget;
        ActorCommonData * ptACDGizmo = ACDTryToGet(PowerGetTargetACD(ptPower));
        if (ptACDGizmo && (ptACDGizmo->GetGizmoType() == ACTORGIZMO_DUNGEON_PORTAL || ptACDGizmo->GetGizmoType() == ACTORGIZMO_RETURN_PORTAL))
        {
            ActorCommonData * ptACDOperator = PowerGetOwner(ptPower);
            if (ptACDOperator)
            {
                SGizmoOperatePortal(*ptACDGizmo, ptACDOperator);
            }
        }
    }
}










static void sSpellEffect_LootRunPortal(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        Target tTarget;
        ActorCommonData * ptACDGizmo = ACDTryToGet(PowerGetTargetACD(ptPower));
        if (ptACDGizmo)
        {
            ActorCommonData * ptACDOperator = PowerGetOwner(ptPower);
            if (SGizmoConsumeItemToOperate(*ptACDGizmo, ptACDOperator))
            {
                SGizmoOperateSwitch(*ptACDGizmo, TRUE);
                SGizmoOperated(*ptACDGizmo, ptACDOperator);
            }
        }
    }
}







static void sSpellEffect_IdentifyItem(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ACDID idACDItem = PowerGetTargetACD(ptPower);
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem)
        {
            ActorCommonData * ptACDPlayer = PowerGetOwner(ptPower);
            if (ptACDPlayer)
            {
                Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
                if (ptPlayer && CValidateItemIsInPlayerInventory(*ptPlayer, idACDItem, FALSE) && ItemIsUnidentified(*ptACDItem))
                {
                    SItemPlayerIdentify(*ptACDItem, ptACDPlayer);
                }
            }
        }
    }
}






















static void sSpellEffect_IdentifySingleItem(ACDID idACDSquare, InventorySlotType, void * pUserData)
{
    ActorCommonData & tACDItem = ACDGet(idACDSquare);
    if (ItemIsUnidentified(tACDItem))
    {
        ActorCommonData * ptACDOwner = (ActorCommonData *)pUserData;
        SItemPlayerIdentify(tACDItem, ptACDOwner);
    }
}






static void sSpellEffect_IdentifyAll(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        ActorCommonData * ptACD = ptACDOwner;
        if (ptACD)
        {
            for (int32 nIndex = 0; cgtDefaultPlayerInventorySlots[nIndex] != -1; nIndex++)
            {
                ACDInventoryIterateItems(*ptACD, cgtDefaultPlayerInventorySlots[nIndex], 0, sSpellEffect_IdentifySingleItem, ptACD);
            }
            PowerSetStoredInt(ptPower, "bSuccess", TRUE);
        }
    }
}















static void sSpellEffect_IdentifyAll_Start(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    PowerSetStoredInt(ptPower, "idRopeLeft", -1);
    PowerSetStoredInt(ptPower, "idRopeRight", -1);

    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }

    ACDID idACDTarget = PowerGetTargetACD(ptPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget || ptACDTarget->GetGizmoType() != ACTORGIZMO_IDENTIFY_ALL)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }


    SNO snoRope = GlobalSNOGet(GLOBALSNO_ROPE_IDENTIFY_ALL);

    SComplexEffectID idRopeLeft = SComplexEffectAdd(*ptACDTarget, snoRope, ptACDOwner->id, 2, 1, 3, FALSE);
    PowerSetStoredInt(ptPower, "idRopeLeft", idRopeLeft);

    SComplexEffectID idRopeRight = SComplexEffectAdd(*ptACDTarget, snoRope, ptACDOwner->id, 2, 1, 4, FALSE);
    PowerSetStoredInt(ptPower, "idRopeRight", idRopeRight);

    Tutorial::SendTutorialToClient(GlobalSNOGet(GLOBALSNO_TUTORIAL_IDENTIFY_ALL), ptPlayer, NULL, ACDID_INVALID);
}




static void sSpellEffect_IdentifyAll_Interrupt(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        // 
        return;
    }
    SComplexEffectID idRopeLeft = PowerGetStoredInt(ptPower, "idRopeLeft");
    if (idRopeLeft != -1)
    {
        // 
        SComplexEffectRemove(ptACDOwner->id, idRopeLeft);
    }
    // 
    SComplexEffectID idRopeRight = PowerGetStoredInt(ptPower, "idRopeRight");
    if (idRopeRight != -1)
    {
        // 
        SComplexEffectRemove(ptACDOwner->id, idRopeRight);
    }
    // 
    // 
    // 
    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    ACDID idACDTarget = PowerGetTargetACD(ptPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptPlayer && ptACDTarget && ptACDTarget->GetGizmoType() == ACTORGIZMO_IDENTIFY_ALL && !PowerGetStoredInt(ptPower, "bSuccess"))
    {
        // 
        AnimPlayerSendAnimationToPlayer(*ptACDTarget, TAG_ACTOR_ANIMATION_CLOSING, (uint32)-1, ANIM_REASON_GIZMO, *ptPlayer);
    }
}




static void sSpellEffect_TeleportPlayer_DelayedArrivalEffect(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            if (!ptACDOwner->IsLoading() && PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_WORLD_CREATING_BUFF), ptACDOwner, ACDID_INVALID) == BUFFID_INVALID)
            {
                SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_1);
                if (snoEffect != SNO_NONE)
                {
                    ACDPlayEffect(ptACDOwner->id, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
                }
                PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDOwner, ptPower->snoPower, -1, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);
                return;
            }
            PowerTriggerAddTimerTrigger(idPower, 5, sSpellEffect_TeleportPlayer_DelayedArrivalEffect, NULL);
        }
    }
}












static void sSpellEffect_TeleportPlayer_OutroEffect(Power * ptPower, ActorCommonData * ptACDOwner, PowerTriggerCallback pTeleportFunction)
{
    // 
    SNO snoProxyActor = GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY);
    // 
    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = snoProxyActor;
    tActorAdd.tSNOName = SNOName(ACTOR_SNO, snoProxyActor);
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
    tActorAdd.eFastAttribType = FAT_SIMPLE;
    tActorAdd.eTeam = PowerGetTeam(ptPower->id);
    tActorAdd.tPlace = ptACDOwner->GetPlace();
    tActorAdd.qOrientation = ptACDOwner->GetOrientation();
    tActorAdd.flScale = 1.0f;
    // 
    ActorID idActorProxy = ActorAdd(tActorAdd);
    if (idActorProxy == ACTORID_INVALID)
    {
        pTeleportFunction(ptPower->id, NULL);
        return;
    }
    // 
    ACDID idACDProxy = ACDFindByActor(idActorProxy);
    if (idACDProxy == ACDID_INVALID)
    {
        pTeleportFunction(ptPower->id, NULL);
        return;
    }
    // 
    ActorCommonData & tACDProxy = ACDGet(idACDProxy);
    tACDProxy.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_IS_POWER_PROXY, TRUE);
    ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    // 
    // 
    ActorDeleteSoon(idActorProxy, 120);
    // 
    // 
    SNO snoEffect = PowerGetEffect(ptPower->id, TAG_POWER_GENERIC_EFFECT_GROUP_0);
    if (snoEffect != SNO_NONE)
    {
        // 
        ACDPlayEffect(idACDProxy, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
    }
    // 
    // 
    // 
    PowerAPIBuffStartData tBuffStartData(ptPower->id);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.dwDurationMin = 900;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.nBuffIndex = 0;
    // 
    // 
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ALPHA_ATTACHMENTS, TRUE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS, 1.0f);
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    // 
    // 
    int32 uTicksUntilWorldSwitch = Math::FloatToInt(PowerGetFormulaValue(ptPower->id, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
    PowerTriggerAddTimerTrigger(ptPower->id, uTicksUntilWorldSwitch, pTeleportFunction, NULL);
}




static void sSpellEffect_TeleportToPlayer_Internal(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        if (ptACDOwner)
        {
            Player * ptPlayerDest = PlayerGetByACD((ACDID)ptPower->dwParam);
            if (ptPlayerDest)
            {
                Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
                if (ptPlayer)
                {
                    SPlayerWarpToOtherPlayer(ptPlayer, ptPlayerDest);
                    PowerTriggerAddTimerTrigger(idPower, 5, sSpellEffect_TeleportPlayer_DelayedArrivalEffect, NULL);
                }
            }
        }
    }
}


// X/Common/Networking/GameMessage.h (DWARF names; kept local, as FloatingNumberMessage above)











static void sSpellEffect_TeleportToPlayer(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }
    // 
    // 
    Player * ptPlayerDest = PlayerGetByACD((ACDID)ptPower->dwParam);
    if (!ptPlayerDest)
    {
        return;
    }
    // 
    // 
    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }
    // 
    // 
    // 
    sSpellEffect_TeleportPlayer_OutroEffect(ptPower, ptACDOwner, sSpellEffect_TeleportToPlayer_Internal);
}

static void sSpellEffect_TeleportToPlayer_Cast(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }

    Player * ptPlayerDest = PlayerGetByACD((ACDID)ptPower->dwParam);
    if (!ptPlayerDest)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }


    PortedToPlayerMessage tMessage;
    tMessage.nPlayerIndexPorted = ptPlayer->nPlayerIndex;
    tMessage.nPlayerIndexTarget = ptPlayerDest->nPlayerIndex;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, 0, -1);
    MessageSendToClient(listDestPlayers, SMSG_PORTED_TO_PLAYER, &tMessage, sizeof(tMessage));



    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = tPowerStartData.idACDAlternateSource = ptACDOwner->id;
    tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_TELEPORT_TO_PLAYER);
    tPowerStartData.dwParam = ptPower->dwParam;

    PowerID idNewPower = PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
    Power * ptNewPower = PowerTryToGet(idNewPower);
    if (ptNewPower)
    {
        ptNewPower->dwParam = ptPower->dwParam;


        PowerHelper_DoFirstTimeCosts(idNewPower, TRUE);
    }
}




static void sSpellEffect_TeleportToWaypoint_Internal(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        if (ptACDOwner)
        {
            Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
            if (ptPlayer)
            {
                uint32 uWaypointIdx = ptPower->dwParam;
                SPlayerWarpToWaypoint(ptPlayer, uWaypointIdx);
                PowerTriggerAddTimerTrigger(idPower, 5, sSpellEffect_TeleportPlayer_DelayedArrivalEffect, NULL);
            }
        }
    }
}












static void sSpellEffect_TeleportToWaypoint(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }
    // 
    // 
    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }
    // 
    // 
    // 
    sSpellEffect_TeleportPlayer_OutroEffect(ptPower, ptACDOwner, sSpellEffect_TeleportToWaypoint_Internal);
}

static void sSpellEffect_TeleportToWaypoint_Cast(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }
    Player * ptPlayer = PlayerGetByACD(ptACDOwner->id);
    if (!ptPlayer)
    {
        return;
    }
    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = ptACDOwner->id;
    tPowerStartData.idACDAlternateSource = tPowerStartData.idACD;
    tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_TELEPORT_TO_WAYPOINT);
    tPowerStartData.dwParam = ptPower->dwParam;
    PowerID idNewPower = PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
    Power * ptNewPower = PowerTryToGet(idNewPower);
    if (ptNewPower)
    {
        PowerHelper_DoFirstTimeCosts(idNewPower, TRUE);
    }
}











static float sCompareCompanionLifeTime(ACDID idACD)
{

    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    Actor * ptActor = ActorGet(idActor);

    return (float)ptActor->dwTimeCreated;
}

static void sSpellEffect_SummonVanityPet(PowerID idPower, void * pData)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }


    SNO snoCompanion = (SNO)(intptr)pData;
    if (snoCompanion == SNO_NONE)
    {
        return;
    }



    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    if (!ptACDPlayer)
    {
        return;
    }

    if (!ACDIsPlayer(ptACDPlayer->id))
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);

    int32 nPos;
    ACDID idACDExistingPet = PlayerGetFirstPet(ptACDPlayer->id, PETTYPE_COMPANION, nPos);
    if (idACDExistingPet != ACDID_INVALID)
    {
        ActorCommonData * ptACDExistingPet = ACDTryToGet(idACDExistingPet);
        if (ptACDExistingPet && ptACDExistingPet->GetActorSNO() == snoCompanion)
        {

            PlayerRemovePet(*ptPlayer, idACDExistingPet, FALSE);
            PowerAPI_BuffFlagAllForDeletion(*ptACDExistingPet);
            BrainAPIDoDeath((ActorID)ACDGetOwner(idACDExistingPet), (ActorID)ACDGetOwner(ptACDPlayer->id), TRUE, TRUE, TRUE, NULL, NULL);
            return;
        }



        PlayerRemovePet(*ptPlayer, idACDExistingPet, FALSE);
        PowerAPI_BuffFlagAllForDeletion(*ptACDExistingPet);
        BrainAPIDoDeath((ActorID)ACDGetOwner(idACDExistingPet), (ActorID)ACDGetOwner(ptACDPlayer->id), TRUE, TRUE, TRUE, NULL, NULL);
    }



    PowerHelper_DoFirstTimeCosts(idPower, TRUE);

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_DEACTIVATE);
    tActorAdd.eTeam = ptACDPlayer->GetNaturalTeam();
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = snoCompanion;
    tActorAdd.nLevel = 1;
    tActorAdd.snoActor = snoCompanion;
    tActorAdd.bIsSummon = TRUE;

    NavMeshHandle tNavMeshHandleResult;
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(*ptACDPlayer, tNavMeshHandle);
    BOOL bFindResult = FindSpawnLocation(snoCompanion,
        tNavMeshHandle,
        PASSABILITY_MONSTER,
        0,
        tActorAdd.tPlace,
        tNavMeshHandleResult);


    if (!bFindResult)
    {
        return;
    }

    ScenesCommonNavMeshNodeFromPlace(tActorAdd.tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

    ActorID idActor = GBAdd(tActorAdd);

    if (idActor == ACTORID_INVALID)
    {
        return;
    }

    ACDID idACDSummoned = ACDFindByActor(idActor);

    if (idACDSummoned != ACDID_INVALID)
    {
        SPlayerAddPet(*ptPlayer, PETTYPE_COMPANION, idACDSummoned, TRUE, 1, sCompareCompanionLifeTime, FALSE);
    }
}



static void sSpellEffect_Warp(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptPower && ptACDOwner)
    {
        ACDWarpToTarget(*ptACDOwner, ptPower->tTarget, 0, 0.0f, TRUE);
    }
}






static void sSpellEffect_WarnOthers(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptPower && ptACDOwner)
    {
        ACDID idACDTarget = ptPower->tTarget.GetACD();
        if (idACDTarget != ACDID_INVALID)
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (ptACDTarget)
            {
                BrainAPIWarnNearbyAlliesAboutTarget((ActorID)ptACDOwner->idOwner, (ActorID)ptACDTarget->idOwner);
            }
        }
    }
}



static void sSpellEffect_SummoningMachineActivate(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDCaster = PowerGetOwner(idPower);
    if (!ptACDCaster)
    {
        return;
    }

    ACDID idACDNode = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDNode = ACDTryToGet(idACDNode);
    if (!ptACDNode)
    {
        return;
    }

    ActorID idActorNode = (ActorID)ACDGetOwner(idACDNode);
    XPooledDataIDList listLinkedACDs(MemoryPoolsGetDATAIDListPool());
    MarkerGetACDsLinkedByActor(idActorNode, listLinkedACDs, NULL);
    if (listLinkedACDs.Count() != 1)
    {
        return;
    }


    ACDID idACDCenter = (ACDID)listLinkedACDs.GetHead();
    ActorCommonData * ptACDCenter = ACDTryToGet(idACDCenter);
    if (!ptACDCenter || !ptACDCenter->IsAlive())
    {
        return;
    }


    PowerAPIBuffStartData tCasterBuffStartData(idPower);
    tCasterBuffStartData.dwDurationMin = 60;
    tCasterBuffStartData.bPositiveEffect = TRUE;
    tCasterBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tCasterBuffStartData.nBuffIndex = 0;
    FastAttribSetValueInt(tCasterBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    FastAttribSetValueInt(tCasterBuffStartData.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, TRUE);
    PowerAPI_BuffAddToACD(tCasterBuffStartData, *ptACDCaster);


    PowerAPIBuffStartData tNodeBuffStartData(idPower);
    tNodeBuffStartData.dwDurationMin = 60;
    tNodeBuffStartData.bPositiveEffect = TRUE;
    tNodeBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tNodeBuffStartData.nBuffIndex = 1;

    float flIsInvulnerable = PowerGetFormulaValue(idPower, TAG_POWER_SUMMONING_MACHINE_NODE_IS_INVULNERABLE);
    if (flIsInvulnerable > 1e-6f)
    {
        FastAttribSetValueInt(tNodeBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    }

    FastAttribSetValueInt(tNodeBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    PowerAPI_BuffAddToACD(tNodeBuffStartData, *ptACDNode);


    PowerAPIBuffStartData tCenterBuffStartData(idPower);
    tCenterBuffStartData.dwDurationMin = 60;
    tCenterBuffStartData.bPositiveEffect = TRUE;
    tCenterBuffStartData.eStackType = BUFF_STACKTYPE_ONE_PER_SOURCE;
    tCenterBuffStartData.idACDSource = ptACDNode->id;
    tCenterBuffStartData.nBuffIndex = 2;
    tCenterBuffStartData.nMaxStackCount = 1;

    FastAttribSetValueInt(tCenterBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_SUMMONING_MACHINE_SPAWN_COUNT, ptACDCaster->snoActor), 1);
    FastAttribSetValueInt(tCenterBuffStartData.idFastAttrib, ATTRIBUTE_SUMMONING_MACHINE_SPAWN_TEAM, ptACDCaster->GetNaturalTeam());
    FastAttribSetValueInt(tCenterBuffStartData.idFastAttrib, ATTRIBUTE_SUMMONING_MACHINE_NUM_CASTERS, 1);
    FastAttribSetValueInt(tCenterBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    PowerAPI_BuffAddToACD(tCenterBuffStartData, *ptACDCenter);
}



static int32 sSummoningMachineSpawnActor(
    PowerID idPower,
    SNO snoActorToSpawn,
    int32 nNumToSpawn,
    Team eSpawnTeam)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return 0;
    }

    ActorCommonData * ptACDMachine = PowerGetOwner(idPower);
    if (!ptACDMachine)
    {
        return 0;
    }

    ptPower->tTarget.InitializeFromACD(ptACDMachine->id, 0, 0);

    for (int32 i = 0; i < nNumToSpawn; i++)
    {
        PowerAPISummonActor tSummonInfo;
        BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoActorToSpawn, ACDID_INVALID, tSummonInfo, 1);
        if (!bSuccess)
        {
            return i;
        }

        tSummonInfo.eTeamOverride = eSpawnTeam;

        ACDID idACDSummoned = PowerAPI_SummonActor(tSummonInfo);
        if (idACDSummoned == ACDID_INVALID)
        {
            return i;
        }

        SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
        if (snoEffect != SNO_NONE)
        {
            ACDPlayEffect(idACDSummoned, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
        }

        ActorCommonData & tACDSummoned = ACDGet(idACDSummoned);
        AnimPlayerPlayAnimation(tACDSummoned, TAG_ACTOR_ANIMATION_TEMP_CASTING, (uint32)-1, TRUE, ANIM_REASON_SPAWN, 0.0f, FALSE, 1.0f);
    }
    return nNumToSpawn;
}




static void sSpellEffect_SummoningMachineSummon(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDMachine = PowerGetOwner(idPower);
    if (!ptACDMachine)
    {
        return;
    }

    XPooledDataIDList listSNOs(MemoryPoolsGetDATAIDListPool());
    int32 nTotalSpawnCount = 0;
    AttribGroupID idFastAttrib = ptACDMachine->AttributesGetAttributeGroup();
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        FastAttribValue tValue = tIt.tEntryBuffer.tValue;
        if (KeyGetAttrib(tKey) != ATTRIBUTE_SUMMONING_MACHINE_SPAWN_COUNT)
        {
            continue;
        }

        SNO snoActorToSpawn = KeyGetParam(tKey);
        listSNOs.AddTail_Tracked(snoActorToSpawn);
        int32 nNumToSpawn = tValue.nValue;
        nTotalSpawnCount += nNumToSpawn;
    }

    SNO snoActorToSpawn = SNO_NONE;
    uint32 uRandomValue = Rand_Internal(SGameGlobalsGet().rng, listSNOs.Count() - 1);
    uint32 uListCount = 0;
    POS pos = listSNOs.GetHeadPosition();
    while (pos)
    {
        snoActorToSpawn = listSNOs.GetNext(pos);
        if (uListCount++ <= uRandomValue)
        {
            break;
        }
    }



    if (snoActorToSpawn != SNO_NONE)
    {
        Team eSpawnTeam = ptACDMachine->AttributesGetInt(ATTRIBUTE_SUMMONING_MACHINE_SPAWN_TEAM);
        int32 nSummonedCount = sSummoningMachineSpawnActor(idPower, snoActorToSpawn, nTotalSpawnCount, eSpawnTeam);

        int32 nTotalCount = ptACDMachine->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
        nTotalCount += nSummonedCount;
        ptACDMachine->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nTotalCount);
    }
}

void ZombieBrainDisableZombieCrawlerSpawnOnDeath(const ActorID idActor);  // SanctuaryBrains.cpp:480


static void sSpellEffect_Bonereaver_Consume(PowerID idPower, void *)
{


    Target tTarget;
    PowerGetTarget(idPower, tTarget);
    ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDTarget && ptACDOwner &&
        ptACDTarget->IsAlive() &&
        ptACDTarget->IsMonster())
    {
        ActorID idActor = (ActorID)ptACDTarget->idOwner;
        ZombieBrainDisableZombieCrawlerSpawnOnDeath(idActor);
        PowerAPIDamage tDamage;
        PowerHelper_DamageInit(tDamage, idPower, tTarget.idACDTarget);
        BrainAPIDoDeath(idActor, (ActorID)ptACDOwner->idOwner, TRUE, TRUE, TRUE, &tDamage, NULL);

        float flHealthMax = ptACDOwner->GetHitPointsMaxTotal();
        float flHealAmountPercentage = PowerGetFormulaValue(idPower, TAG_POWER_BONEREAVER_HEAL_PERCENTAGE);
        float flHealTime = PowerGetFormulaValue(idPower, TAG_POWER_BONEREAVER_HEAL_DURATION);
        if (flHealTime <= 1e-6f)
        {
            return;
        }

        uint32 uHealTicks = Math::FloatRoundToInt(flHealTime * 60.0f);
        float flDPS = flHealthMax * flHealAmountPercentage / flHealTime;
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.dwDurationMin = uHealTicks;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND, flDPS);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DOT_DPS, -flDPS);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    }
}





static void sSpellEffect_Bonereaver_Summon(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);

    const int nDemonsToSummon = 3;

    for (int i = 0; i < nDemonsToSummon; i++)
    {
        PowerAPISummonActor tSummonInfo;
        Power * ptPower = PowerGet(idPower);
        ptPower->tTarget.InitializeFromACD(ptPower->idACD, FALSE, FALSE);
        BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, TRUE);
        if (!bSuccess)
        {
            return;
        }


        ACDID idACDDemon = PowerAPI_SummonActor(tSummonInfo);

        if (idACDDemon == ACDID_INVALID)
        {
            return;
        }


        ActorCommonData & tACDDemon = ACDGet(idACDDemon);
        tACDDemon.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDDemon.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);

        int nSummonDuration = Math::FloatToInt(PowerGetFormulaValue(tSummonInfo.idPower, TAG_POWER_SUMMONED_ACTOR_LIFE_DURATION) * 60.0f);
        tACDDemon.AttributesSetInt(ATTRIBUTE_SUMMON_EXPIRATION_TICK, GameGetServerTime() + nSummonDuration);
    }

    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
}




static void sSpellEffect_Beast_Charge_Begin(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        // 
        // 
        // 
        return;
    }
    ACDID idACDTarget = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        // 
        // 
        // 
        return;
    }
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN) * 60.0f);
    tBuffStartData.eStackType = 0;
    tBuffStartData.nBuffIndex = 7;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDTarget);
    // 
    PowerAPIBuffStartData tEffectBuffStartData(idPower);
    tEffectBuffStartData.dwDurationMin = 0;
    tEffectBuffStartData.bInfinite = TRUE;
    tEffectBuffStartData.eStackType = 0;
    tEffectBuffStartData.nBuffIndex = 0;
    tEffectBuffStartData.snoEndingEffect = PowerGetEffect(idPower, TAG_POWER_ENDING_EFFECT_GROUP);
    FastAttribSetValueInt(tEffectBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    PowerAPI_BuffAddToACD(tEffectBuffStartData, *ptACDOwner);
}




static void sSpellEffect_Beast_Charge(PowerID idPower, void *)
{
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        // 
        // 
        WorldPlace placeContact;
        placeContact.idSWorld = ptACDOwner->GetSWorldID();
        ptACDOwner->GetContactOffset(placeContact.wp, ptPower->dwCurrentContactFrame);

        Target tTarget;
        tTarget.InitializeFromPlace(placeContact);

        PowerAPIChooseTargets tChooseTargets;
        PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, tTarget, tChooseTargets);

        // 
        PowerHelper_DoFirstTimeCosts(idPower, TRUE);

        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

        // 
        // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
        tPayload.eDeliveryMechanism = PDM_MELEE;
        tPayload.wdKnockbackMagnitude = 8.0f;
        // 
        // 
        PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);

        BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, ptACDOwner, ACDID_INVALID);
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}


static void sSpellEffect_HelperAttach(PowerID idPower, void *)
{
    Target tTarget;
    BOOL bGotPower = PowerGetTarget(idPower, tTarget);
    if (!bGotPower)
    {
        return;
    }
    ACDID idACDTarget = tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget ||
        !ptACDTarget->IsHelper())
    {
        return;
    }
    if (ptACDTarget->IsPet())
    {
        // 
        return;
    }
    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    // 
    // 
    // 
    // 
    int32 nTotalPetsOfType = PlayerGetNumPetsOfType(ptACDPlayer->id, PETTYPE_HELPER);
    if (nTotalPetsOfType > 3)
    {
        return;
    }
    {
        BOOL bEscortSet = AIStateSetStateData((ActorID)ACDGetOwner(idACDTarget), 1);
        if (bEscortSet)
        {
            // 
            // 
            // 
            // 
            // 
            // 
            // 
            SPlayerAddPet(ptACDPlayer->id, PETTYPE_HELPER, idACDTarget, TRUE, 4, NULL);
            ptACDTarget->AttributesSetInt(ATTRIBUTE_GUARD_OBJECT_ACDID, ACDID_INVALID);
            ScriptEventOnHelperAttached(idACDTarget, ptACDPlayer->id);
        }
    }
}



static void sSpellEffect_HelperGuardObject(PowerID idPower, void *)
{
    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    int32 nNumHelpers = PlayerGetNumPetsOfType(ptACDPlayer->id, PETTYPE_HELPER);
    if (nNumHelpers < 1)
    {
        return;
    }
    Target tTarget;
    BOOL bGotPower = PowerGetTarget(idPower, tTarget);
    if (!bGotPower)
    {
        // 
        // 
        // 
        return;
    }
    ACDID idACDObject = tTarget.GetACD();
    // 
    int32 nPos;
    ACDID idACDHelper = PlayerGetFirstPet(ptACDPlayer->id, PETTYPE_HELPER, nPos);
    ActorCommonData & tACDHelper = ACDGet(idACDHelper);
    if (!tACDHelper.IsAlive())
    {
        // 
        return;
    }
    NPCHelperDetach(*ptACDPlayer, tACDHelper);
    {
        BOOL bSuccess = AIStateSetStateData((ActorID)ACDGetOwner(idACDHelper), 3);
        // 
        // 
        // 
        // 
        // 
        // 
    }
    tACDHelper.AttributesSetInt(ATTRIBUTE_GUARD_OBJECT_ACDID, idACDObject);
    ScriptEventOnHelperGuardObject(idACDHelper, idACDObject);
}



static void sSpellEffect_SetMode_EscortFollow(PowerID idPower, void *)
{
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        BOOL bSuccess = AIStateSetStateData((ActorID)ptACDOwner->idOwner, 1);
    }
}







// X/Common/Powers/PowersCommon.h, X/Server/AI/BrainStance.h
void PowerFaceTarget(ActorCommonData * ptACD, const Target & tTarget, const uint32 dwAnimationTag, BOOL bSnapToFacing);  // PowersCommon.cpp:5454
void BrainStanceDoIdle(ActorID idActor);  // BrainStance.cpp:2416


static void sSpellEffect_NPC_LookAt(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        Target tTarget;
        if (PowerGetTarget(idPower, tTarget))
        {
            ACDID idACDTarget = tTarget.GetACD();
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (ptACDTarget)
            {



                if (ACDIsPlayer(idACDTarget))
                {
                    Player * ptPlayer = PlayerGetByACD(idACDTarget);
                    if (!ptACD->IsKnownByPlayer(*ptPlayer))
                    {
                        return;
                    }
                }

                ACDNetworkName idANNTarget = ANNFromACD(idACDTarget);

                PowerAPIBuffStartData tBuffStartData(idPower);
                tBuffStartData.dwDurationMin = 90;
                tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
                FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_LOOK_TARGET_SERVER_ANN, idANNTarget);
                PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);

                Angle aYaw = ACDGetYaw(ptACD->id);
                Vector3D vFacing(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
                WorldPlace placeUs = ptACD->GetPlace();
                WorldPlace placeTarget = ptACDTarget->GetPlace();
                Vector3D vDiff = placeTarget.wp - placeUs.wp;
                vDiff.z = 0.0f;
                if (vDiff.MagnitudeSquared() > EPSILON * EPSILON)
                {
                    vDiff.Normalize();
                }
                else
                {
                    vDiff = vFacing;
                }
                float flCosAng = Vector3D::DotProduct(vFacing, vDiff);
                if (flCosAng < 0.8660254f)
                {
                    PowerFaceTarget(ptACD, tTarget, (uint32)-1, FALSE);
                }
            }
        }

        ActorID idActor = (ActorID)ptACD->idOwner;
        BrainStanceDoIdle(idActor);
    }
}



static void sGoatmanLightningShield_GetHit(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDTarget)
{
    // 
    // 
    // 
    // 
    // 
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDAttacker || !ptACDAttacker->IsAlive() || !ptACDTarget)
    {
        // 
        return;
    }
    // 
    // 
    if (tPowerDamage.eDeliveryMechanism == 4 || tPowerDamage.eDeliveryMechanism == 5)
    {
        // 
        // 
        return;
    }
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(ptBuff->idPower, tPayload);
    // 
    tPayload.eDeliveryMechanism = 5;
    PowerAPI_DoPayload(tPayload, idACDAttacker, ACDID_INVALID);
    // 
    SNO snoRope = PowerEffectTagGetSNO(ptBuff->idPower, TAG_POWER_ROPE_SNO);
    ACDPlayEffectRopeACDToACD(snoRope, idACDAttacker, 1, idACDTarget, 1, FALSE);
    // 
    // 
    // 
    // 
    // 
    // 
    // 
    PowerAPI_BuffFlagForDeletion(ptBuff->id, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
}



static void sSpellEffect_GoatmanLightningShield(PowerID idPower, void *)
{
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        // 
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN) * 60.0f);
        tBuffStartData.nAppearanceParameter = 0x25;
        tBuffStartData.pOnGetHitEffect = sGoatmanLightningShield_GetHit;
        tBuffStartData.eStackType = 0;
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}



static void sGoatmanColdShield_DeliverDamage(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, void *)
{
    ActorCommonData * ptACDAffected = ACDTryToGet(idACDTarget);
    if (ptACDAffected)
    {
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(ptBuff->idPower, tPayload);
        tPayload.eDeliveryMechanism = PDM_THORNS;
        PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
    }
}







static void sGoatmanColdShield_GetHit(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDTarget)
{
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDAttacker && ptACDAttacker->IsAlive() && ptACDTarget)
    {
        if (tPowerDamage.eDeliveryMechanism == PDM_MELEE)
        {
            PowerAPIBuffStartData tBuffStartData(ptBuff->idPower);
            tBuffStartData.dwDurationMin = 15;
            tBuffStartData.pExpirationEffect = sGoatmanColdShield_DeliverDamage;
            tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACDAttacker);
            SNO snoEffectGroup = PowerContactTagGetSNO(ptBuff->idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
            if (snoEffectGroup != SNO_NONE)
            {
                ACDPlayEffect(idACDAttacker, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup, ACDID_INVALID);
            }
        }
        PowerAPI_BuffFlagForDeletion(ptBuff, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}























static void sGoatmanColdShieldAbsorb(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, float * flDamageDone, PowerAPIDamage & tPowerDamage, void * pData)
{
    if (!flDamageDone)
    {
        return;
    }
    PowerID idPower = ptBuff->idPower;
    if (!PowerTryToGet(idPower))
    {
        return;
    }
    float flAbsorbPercent = (float)(intptr)pData * 0.01f;
    for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++)
    {
        flDamageDone[iDamageType] *= flAbsorbPercent;
    }
}














static void sSpellEffect_GoatmanColdShield(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        Power * ptPower = PowerTryToGet(idPower);
        if (ptPower)
        {
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.bInfinite = TRUE;
            tBuffStartData.pOnGetHitEffect = sGoatmanColdShield_GetHit;
            tBuffStartData.pDamageShieldEffect = sGoatmanColdShieldAbsorb;
            tBuffStartData.pDamageShieldEffectData = (void *)(intptr)Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SHIELD_DAMAGE_ABSORB_PERCENT) * 100.0f);
            tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ENRAGED, TRUE);
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
        }
    }
}












void sSpellEffect_GoatmanIceball(const PowerID idPower, void *)
{
    // 
    // 
    // 
    // 
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        // 
        return;
    }
    Power * ptPower = PowerGet(idPower);
    // 
    Target tTargetToShootAt;
    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, -1, -1, NULL);
    // 
    ActorCommonData * ptACDCaster = PowerGetOwner(idPower);
    if (ptACDCaster)
    {
        SNO snoBuffShield = GlobalSNOGet(GLOBALSNO_POWERS_GOATMAN_COLD_SHIELD_BUFF);
        if (snoBuffShield != SNO_NONE)
        {
            BuffID idBuffShield = PowerAPI_BuffFindOnACD(snoBuffShield, *ptACDCaster, ACDID_INVALID);
            if (idBuffShield != BUFFID_INVALID)
            {
                PowerAPI_BuffFlagForDeletion(idBuffShield, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
        // 
        // 
        // 
        // 
        // 
        ActorMovement * ptMove = ptACDCaster->GetActorMovement();
        if (ptMove)
        {
            ptMove->SnapPlaceToGround(tNavPlaceSource.tPlace);
        }
    }
    // 
    // 
    BOOL bSuccess = SpellEffect_ProjectileWeaponPower_GetTarget(idPower, tTargetToShootAt, tNavPlaceSource, 0.0f);
    if (!bSuccess)
    {
        // 
        return;
    }
    // 
    // 
    SpellEffect eSpellEffectToRun = PowerGetNextSpellEffect(idPower);
    PowerAPICreateProjectile tProjectile;
    PowerHelper_ProjectileInit(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, tTargetToShootAt, eSpellEffectToRun, tProjectile);
    tProjectile.tProjectileData.eDPath = DPATH_SIN_WAVE;
    tProjectile.tProjectileData.tDurationData.dwTime = 150;
    tProjectile.tProjectileData.tDPathSinData.flSinIncPerTick = 0.08f;
    tProjectile.tProjectileData.tDPathSinData.flLateralMaxDistance = 10.0f;
    tProjectile.tProjectileData.tDPathSinData.flOOLateralDistanceToScale = 1.0f / 30.0f;
    tProjectile.tProjectileData.tDPathSinData.flSpeed = 0.4f;
    ACDID idACDProjectile = PowerAPI_DoProjectile(tProjectile);
}

void sSpellEffect_GoatmanDrumHit(const PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }


    ACDID idACDDrum = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDDrum = ACDTryToGet(idACDDrum);
    if (!ptACDDrum)
    {
        return;
    }

    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundTargetFromPayload(idPower, tChooseTargets);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    PowerAPIBuffStartData tDrumBuffStartData(idPower);
    tDrumBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN) * 60.0f);
    tDrumBuffStartData.bPositiveEffect = TRUE;
    tDrumBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tDrumBuffStartData.nBuffIndex = 0;
    FastAttribSetValueFloat(tDrumBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_PERCENT_ALL));
    FastAttribSetValueFloat(tDrumBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR, PowerGetFormulaValue(idPower, TAG_POWER_MOVEMENT_SPEED_PERCENT_INCREASE_MIN));
    FastAttribSetValueFloat(tDrumBuffStartData.idFastAttrib, ATTRIBUTE_SCALE_BONUS, PowerGetFormulaValue(idPower, TAG_POWER_SCALE_BONUS));
    FastAttribSetValueInt(tDrumBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);

    ScopedActorDefinitionLockR pdefActor(ptACDDrum->snoActor);
    SNO snoActor0 = TagMapGetSNO(pdefActor->hTagMap, 393232 /* 0x60010 */);
    SNO snoActor1 = TagMapGetSNO(pdefActor->hTagMap, 393248 /* 0x60020 */);
    SNO snoActor2 = TagMapGetSNO(pdefActor->hTagMap, 393264 /* 0x60030 */);
    SNO snoActor3 = TagMapGetSNO(pdefActor->hTagMap, 393280 /* 0x60040 */);


    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        SNO snoActorTarget = tACDTarget.tSNOName.snoHandle;
        if (snoActorTarget == snoActor0 || snoActorTarget == snoActor1 || snoActorTarget == snoActor2 || snoActorTarget == snoActor3)
        {



            ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);
            AIStateSetState(idActorTarget, 2, FALSE);
            PowerAPI_BuffAddToACD(tDrumBuffStartData, tACDTarget);
        }
    }

    SNO snoEffectGroup = PowerContactTagGetSNO(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
    if (snoEffectGroup != SNO_NONE)
    {
        ACDPlayEffect(ptACDDrum->id, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup, ACDID_INVALID);
    }
}



void sSpellEffect_GoatmanEmpower(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }

    ACDID idACDFriend = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDFriend = ACDTryToGet(idACDFriend);
    if (!ptACDFriend)
    {
        return;
    }


    SNO snoRope = PowerContactTagGetSNO(idPower, TAG_POWER_ROPE_SNO);
    if (snoRope != SNO_NONE)
    {
        ACDPlayEffectRopeACDToACD(snoRope,
            ptACDOwner->id,
            3, idACDFriend, 1, FALSE);
    }




    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN) * 60.0f);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.nBuffIndex = 0;
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_PERCENT_ALL));
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_ABSORB_PERCENT_ALL, PowerGetFormulaValue(idPower, TAG_POWER_SHIELD_DAMAGE_ABSORB_PERCENT));
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ENRAGED, TRUE);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDFriend);
}



void sSpellEffect_GoatmanEnrage(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = PowerGetOwner(ptPower);
        if (ptACD)
        {
            float flDurationMin = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN);
            float flDurationDelta = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_DELTA);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.dwDurationMin = Math::FloatToInt((flDurationMin + flDurationDelta) * 60.0f);
            tBuffStartData.bPositiveEffect = TRUE;
            tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
            tBuffStartData.nBuffIndex = 0;
            tBuffStartData.snoEndingEffect = PowerGetEffect(idPower, TAG_POWER_ENDING_EFFECT_GROUP);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ENRAGED, TRUE);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
        }
    }
}






void sSpellEffect_GoatmanExplode(const PowerID idPower, void *)
{
    sSpellEffect_PerformStandardDamage(idPower, NULL);

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        ActorID idActor = (ActorID)ACDGetOwner(ptACDOwner->id);
        BrainAPIDoDeath(idActor, ACTORID_INVALID, FALSE, FALSE, FALSE, NULL, NULL);
        ActorFlagForDeletion(idActor);
    }
}





static void sDeathNovaPulse(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        float flRadiusMax = PowerGetFormulaValue(idPower, TAG_POWER_DEATH_NOVA_RADIUS_MAX);
        float flRadiusFtPerSec = PowerGetFormulaValue(idPower, TAG_POWER_DEATH_NOVA_RADIUS_FT_PER_SEC);
        ptPower->flParam += flRadiusFtPerSec * (1.0f / 60.0f);
        if (ptPower->flParam >= flRadiusMax)
        {
            ptPower->flParam = flRadiusMax;
        }
        else
        {
            PowerTriggerAddTimerTrigger(idPower, 1, sDeathNovaPulse, NULL);
        }

        WorldPlace placeCenter;
        if (ptPower->tTarget.IsNone())
        {
            placeCenter = ptACDOwner->GetPlace();
        }
        else
        {
            ptPower->tTarget.GetPlace(placeCenter);
        }
        ActorID idActorOwner = (ActorID)ptACDOwner->idOwner;
        // 
        XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
        ACDsCylinderIntersectSphere(placeCenter, ptPower->flParam, ptACDOwner->id, listTargets, 7);
        // 
        POS pos = listTargets.GetHeadPosition();
        while (pos)
        {
            // 
            ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            // 
            // 
            // 
            if (ptACDTarget && (ptACDTarget->IsWreckableObject() || PowerCanTarget(idPower, idACDTarget, ACDID_INVALID)))
            {
                // 
                ActorID idActorTarget = (ActorID)ptACDTarget->idOwner;
                Actor * ptActorTarget = ActorGet(idActorTarget);
                // 
                // 
                if (ptActorTarget->ptAI)
                {
                    BrainAPIDoDeath(idActorTarget, idActorOwner, TRUE, TRUE, TRUE, NULL, NULL);
                }
            }
        }
    }
}



static void sSpellEffect_DeathNova(PowerID idPower, void *)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        // 
        ptPower->flParam = 0.0f;
        PowerTriggerAddTimerTrigger(idPower, 0, sDeathNovaPulse, NULL);
    }
}



static void sSpellEffect_HearthTimeUp(PowerID idPower, void * pParam)
{
    ACDID idACDItemToUse = (ACDID)(intptr)pParam;
    PowerForceStop(idPower, TRUE);
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        SNO snoChildPower = PowerGetChildPower(idPower);
        PowerID idChildPower = PowerStart(ptACDOwner->id, snoChildPower, cgTargetNone);
        Power * ptChildPower = PowerTryToGet(idChildPower);
        if (ptChildPower)
        {
            ptChildPower->dwParam = idACDItemToUse;
        }
    }
}











static void sSpellEffect_Hearth(PowerID idPower, void * pParam)
{
    float flTime = PowerGetFormulaValue(idPower, TAG_POWER_HEARTH_TIME);
    int32 uTicks = Math::FloatRoundToInt(flTime * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, uTicks, sSpellEffect_HearthTimeUp, pParam);
}






static void sSpellEffect_HearthFinish(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ACDID idACDItemToUse = (ACDID)ptPower->dwParam;
        ItemUse(idPower, ptPower->idACD, idACDItemToUse, &ptPower->tTarget);
    }
}










static void sSpellEffect_GenericAttributeBuff(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bRemoveOnDeath = FALSE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.nBuffIndex = 0;
    PowerApplyGenericAttributeBuffAttributes(*ptACD, AutoPowerDefinitionR(PowerGetSNO(idPower)), tBuffStartData.idFastAttrib);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}






static void sProxyDelayedPower_DoCleanup(PowerID idPower, void * pData)
{
    PowerID idTriggeredPower = (PowerID)(intptr)pData;
    Power * ptPower = PowerTryToGet(idTriggeredPower);
    if (ptPower && ptPower->listTimers.Count() > 0)
    {
        PowerCheckForDone(idTriggeredPower);
        PowerTriggerAddTimerTrigger(idPower, 1, sProxyDelayedPower_DoCleanup, (void *)(intptr)idTriggeredPower);
        return;
    }
    ActorCommonData * ptACDProxy = PowerGetOwner(idPower);
    if (ptACDProxy)
    {
        ActorID idActor = (ActorID)ptACDProxy->idOwner;
        ActorFlagForDeletion(idActor);
    }
}









static void sProxyDelayedPower_DoPower(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    SNO snoPower = PowerGetParam(idPower);
    ActorCommonData * ptACDProxy = PowerGetOwner(ptPower);
    if (ptACDProxy)
    {
        Target tTarget;
        PowerGetTarget(idPower, tTarget);
        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = ptACDProxy->id;
        tPowerStartData.snoPower = snoPower;
        tPowerStartData.tTarget = tTarget;
        tPowerStartData.idACDAlternateSource = PowerGetAlternateSource(idPower);
        PowerID idTriggeredPower = PowerStartEx(tPowerStartData, ptPower->idFastAttrib, FALSE, FALSE);
        PowerTriggerAddTimerTrigger(idPower, 1, sProxyDelayedPower_DoCleanup, (void *)(intptr)idTriggeredPower);
    }
}















static void sSpellEffect_ProxyDelayedPower(PowerID idPower, void * pParam)
{
    uint32 dwDelay = (uint32)(uintptr)pParam;
    PowerTriggerAddTimerTrigger(idPower, dwDelay, sProxyDelayedPower_DoPower, NULL);
}






static void sSpellEffect_BouncingBalls(PowerID idPower, void *)
{





    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }

    Power * ptPower = PowerGet(idPower);
    Target tTargetToShootAt;
    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, -1, -1, NULL);

    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }

    WorldPlace placeSource = ptACD->GetPlace();
    WorldPlace placeTarget(placeSource);
    Angle aYaw = ACDGetYaw(ptACD->id);
    Vector3D vDir(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);

    SpellEffect eSpellEffectToRun = PowerGetNextSpellEffect(idPower);
    int32 nShots = Math::FloatRoundToInt(PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_COUNT));
    for (int32 i = 0; i < nShots; i++)
    {
        float flDist = 17.0f;
        placeTarget.wp = vDir * flDist;
        placeTarget.wp.x += RandRangeFloat_Internal(SGameGlobalsGet().rng, -8.0f, 8.0f);
        placeTarget.wp.y += RandRangeFloat_Internal(SGameGlobalsGet().rng, -8.0f, 8.0f);
        placeTarget.wp += placeSource.wp;
        tTargetToShootAt.InitializeFromPlace(placeTarget);

        PowerAPICreateProjectile tProjectile;
        PowerHelper_ProjectileInit(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, tTargetToShootAt, eSpellEffectToRun, tProjectile);
        tProjectile.tProjectileData.tAnimationData.wdSpeed = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.2f, 0.3f);
        tProjectile.tProjectileData.tAimingData.flHeightAboveSource = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.5f, 5.5f);
        tProjectile.tProjectileData.tDurationData.dwTime = Math::FloatRoundToInt(PowerGetFormulaValue(idPower, TAG_POWER_BOUNCING_BALL_LIFETIME) * 60.0f);
        PowerAPI_DoProjectile(tProjectile);
    }
}



static void sSpellEffect_BurningHouseFireballImpact(PowerID idPower, void * pData)
{
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ACDID idACDHouse = ptPower->tTarget.GetACD();
    if (ACDIsPlayer(idACDHouse) || idACDHouse == ACDID_INVALID)
    {
        return;
    }
    // 
    // 
    ActorCommonData * ptACDHouse = ACDTryToGet(idACDHouse);
    if (!ptACDHouse)
    {
        return;
    }
    // 
    float flHPCurr = ptACDHouse->GetHitPointsCurrent();
    float flHPMax = ptACDHouse->GetHitPointsMaxTotal();
    if (flHPCurr > 1e-6f && flHPMax > 1e-6f)
    {
    // 
        float flHPHalf = flHPMax * 0.5f;
        if (flHPCurr > flHPHalf)
        {
            ptACDHouse->SetHitPoints(flHPHalf, TRUE, FALSE);
        }
    // 
        SNO snoChildPower = PowerGetChildPower(idPower);
        if (snoChildPower != SNO_NONE)
        {
            BuffID idBuff = PowerAPI_BuffFindOnACD(snoChildPower, *ptACDHouse, ACDID_INVALID);
            if (idBuff == BUFFID_INVALID)
            {
                float flDPS = PowerGetFormulaValue(idPower, TAG_POWER_BURN_HOUSE_DAMAGE_PER_SECOND);
    // 
                PowerAPIBuffStartData tBuffStartData(idPower);
                tBuffStartData.snoPower = snoChildPower;
                tBuffStartData.bPositiveEffect = FALSE;
                tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
                tBuffStartData.eDamageType = DAMAGE_TYPE_FIRE;
                tBuffStartData.flDamagePerSecond = flDPS;
                tBuffStartData.bInfinite = TRUE;
                tBuffStartData.idACDSource = ptPower->idACD;
                tBuffStartData.snoDOTDamageSourcePower = PowerGetSNO(idPower);
                PowerAPI_BuffAddToACD(tBuffStartData, *ptACDHouse);
    // 
                ScriptEventOnUserEventSentToActor("BuildingFireStarted", ptPower->idACD, idACDHouse, 0);
            }
        }
    }
    // 
    sImpactDamage(idPower, pData);
}



static void sSpellEffect_BurningHouseWaterballImpact(PowerID idPower, void * pData)
{
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ACDID idACDHouse = ptPower->tTarget.GetACD();
    if (ACDIsPlayer(idACDHouse) || idACDHouse == ACDID_INVALID)
    {
        return;
    }
    // 
    // 
    ActorCommonData * ptACDHouse = ACDTryToGet(idACDHouse);
    if (!ptACDHouse)
    {
        return;
    }
    // 
    float flHPCurr = ptACDHouse->GetHitPointsCurrent();
    float flHPMax = ptACDHouse->GetHitPointsMaxTotal();
    if (flHPCurr > 1e-6f && flHPMax > 1e-6f)
    {
        float flHealAmount = PowerGetFormulaValue(idPower, TAG_POWER_BURN_HOUSE_WATER_HEAL_AMOUNT);
        flHPCurr += flHealAmount;
    // 
        if (flHPCurr >= flHPMax)
        {
    // 
    // 
            SNO snoChildPower = PowerGetChildPower(idPower);
            if (snoChildPower != SNO_NONE)
            {
                BuffID idBuff = PowerAPI_BuffFindOnACD(snoChildPower, *ptACDHouse, ACDID_INVALID);
                if (idBuff != BUFFID_INVALID)
                {
                    PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
                }
            }
    // 
            ScriptEventOnUserEventSentToActor("BuildingFireStopped", ptPower->idACD, idACDHouse, 0);
            flHPCurr = flHPMax;
        }
        ptACDHouse->SetHitPoints(flHPCurr, TRUE, FALSE);
    }
    // 
    sImpactDamage(idPower, pData);
}










static const PandemoniumData ctPandemoniumGlobalSpots[] =
{
    { GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_SPOT_0, GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_0, "UberPortalVanilla_0" },
    { GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_SPOT_1, GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_1, "UberPortalVanilla_1" },
    { GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_SPOT_2, GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_2, "UberPortalVanilla_2" },
};
static const int32 scgPandemoniumNumGlobalSpots = sizeof(ctPandemoniumGlobalSpots) / sizeof(ctPandemoniumGlobalSpots[0]);
    // 
static const PandemoniumData ctUberGlobalSpots[] =
{
    { GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_SPOT_0, GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_0, "UberPortalX1_0" },
    { GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_SPOT_1, GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_1, "UberPortalX1_1" },
    { GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_SPOT_2, GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_2, "UberPortalX1_2" },
    { GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_SPOT_3, GLOBALSNO_ACTOR_PANDEMONIUM_PORTAL_3, "UberPortalX1_3" },
};
static const int32 scgUberNumGlobalSpots = sizeof(ctUberGlobalSpots) / sizeof(ctUberGlobalSpots[0]);
    // 
static void sSetUberPortalGameFlag(int32 nPortalIdx, BOOL bVanilla)
{
    // 
    // 
    if (nPortalIdx >= 0)
    {
        if (nPortalIdx < scgPandemoniumNumGlobalSpots)
        {
            GameFlagSet(ctPandemoniumGlobalSpots[nPortalIdx].szGameFlag, bVanilla);
        }
        if (nPortalIdx < scgUberNumGlobalSpots)
        {
            GameFlagSet(ctUberGlobalSpots[nPortalIdx].szGameFlag, !bVanilla);
        }
    }
}
namespace BossEncounterManager { BOOL IsEncounterCompleted(SNO snoEncounter); } // No header declaration recovered; DWARF records only BossEncounter.cpp:966.
static void sSpellEffect_PandemoniumPortal(
    PowerID idPower,
    void * pData)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            ACDID idACDItem = (ACDID)(intptr_t)pData;
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem)
            {
                WorldPlace placeOwner = ptACDOwner->GetPlace();
                XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
                ACDsIntersectSphere(placeOwner, 50.0f, ptACDOwner->id, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);

                ACDID arACDSpots[scgPandemoniumNumGlobalSpots];
                SNO arSNOSpots[scgPandemoniumNumGlobalSpots];
                SNO arSNOPortalActors[scgPandemoniumNumGlobalSpots];
                for (int32 i = 0; i < scgPandemoniumNumGlobalSpots; i++)
                {
                    arACDSpots[i] = ACDID_INVALID;
                    arSNOSpots[i] = GlobalSNOGet(ctPandemoniumGlobalSpots[i].eGlobalSnoSpot);
                    arSNOPortalActors[i] = GlobalSNOGet(ctPandemoniumGlobalSpots[i].eGlobalSnoPortal);
                }

                int32 nSpotsFound = 0;
                POS pos = listACDs.GetHeadPosition();
                while (pos)
                {
                    ACDID idACD = (ACDID)listACDs.GetNext(pos);
                    ActorCommonData * ptACD = ACDTryToGet(idACD);
                    if (ptACD)
                    {
                        for (int32 i = 0; i < scgPandemoniumNumGlobalSpots; i++)
                        {
                            if (arSNOSpots[i] == ptACD->snoActor)
                            {
                                if (arACDSpots[i] == ACDID_INVALID)
                                {
                                    arACDSpots[i] = ptACD->id;
                                    nSpotsFound++;
                                }
                            }





                            if (arSNOPortalActors[i] == ptACD->snoActor)
                            {

                                ScopedActorDefinitionLockR pdefActor(arSNOPortalActors[i]);
                                SNO snoBossEncounter = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO);

                                if (snoBossEncounter != SNO_NONE)
                                {
                                    if (!BossEncounterManager::IsEncounterCompleted(snoBossEncounter))
                                    {
                                        PlayerDisplayErrorMessage(ptACDOwner->id, GAMEERROR_ITEM_NOT_READY);
                                        return;
                                    }
                                }
                            }
                        }
                    }
                }

                if (nSpotsFound == scgPandemoniumNumGlobalSpots)
                {
                    uint32 & uFlags = SGameGlobalsGet().uPandemoniumFlags;

                    uint32 uCount = 0;
                    for (int32 i = 0; i < scgPandemoniumNumGlobalSpots; i++)
                    {
                        if (!(uFlags & (1 << i)))
                        {
                            uCount++;
                        }
                    }

                    if (uCount > 0)
                    {
                        uint32 uRandomValue = Rand_Internal(SGameGlobalsGet().rng, uCount - 1);
                        for (int32 i = 0; i < scgPandemoniumNumGlobalSpots; i++)
                        {
                            if (!(uFlags & (1 << i)) && uRandomValue <= i && arACDSpots[i] != ACDID_INVALID)
                            {


                                ActorCommonData * ptACDSpot = ACDTryToGet(arACDSpots[i]);
                                if (ptACDSpot)
                                {
                                    ActorAddStruct tActorAdd;
                                    tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);



                                    tActorAdd.tPlace = ptACDSpot->GetPlace();
                                    tActorAdd.qOrientation.FromYaw(0.0f);
                                    tActorAdd.eTeam = TEAM_SCENERY;
                                    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

                                    tActorAdd.tMarkersName = ptACDSpot->tName;
                                    tActorAdd.snoActor = GlobalSNOGet(ctPandemoniumGlobalSpots[i].eGlobalSnoPortal);
                                    if (tActorAdd.snoActor != SNO_NONE)
                                    {
                                        ActorID idActorPortal = GizmoAddBySNO(0, tActorAdd);
                                        Actor * ptActorPortal = ActorTryToGet(idActorPortal);
                                        if (ptActorPortal)
                                        {
                                            uFlags |= (1 << i);

                                            sSetUberPortalGameFlag(i, TRUE);
                                            SACDInventoryDeleteItemAfterUse(*ptACDItem);


                                            SActTransitionSetPreventActTransition(TRUE);
                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }


            }

            PlayerDisplayErrorMessage(ptACDOwner->id, GAMEERROR_POWER_UNUSABLE_IN_AREA);
        }
    }
}



static void sSpellEffect_UberBossPortal(PowerID idPower, ACDID idACDItem, int32 nPortalIndex)
{
    // 
    // 
    // 
    if (nPortalIndex < 0 || nPortalIndex >= scgUberNumGlobalSpots)
    {
        return;
    }
    // 
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem)
            {
                WorldPlace placeOwner = ptACDOwner->GetPlace();
                XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
                ACDsIntersectSphere(placeOwner, 50.0f, ptACDOwner->id, listACDs, ACTOR_SPHERE_TREE_FLAGS_ALL);
    // 
                SNO snoSpot = GlobalSNOGet(ctUberGlobalSpots[nPortalIndex].eGlobalSnoSpot);
                if (snoSpot == SNO_NONE)
                {
                    return;
                }
    // 
    // 
    // 
                SNO snoPortalActor = GlobalSNOGet(ctUberGlobalSpots[nPortalIndex].eGlobalSnoPortal);
                if (snoPortalActor == SNO_NONE)
                {
                    return;
                }
    // 
    // 
    // 
                uint32 & uFlags = SGameGlobalsGet().uPandemoniumFlags;
                if (uFlags & (1 << nPortalIndex))
                {
    // 
                    PlayerDisplayErrorMessage(ptACDOwner->id, GAMEERROR_ITEM_NOT_READY);
                    return;
                }
    // 
                ActorCommonData * ptACDSpot = NULL;
                POS pos = listACDs.GetHeadPosition();
                while (pos)
                {
                    ACDID idACD = (ACDID)listACDs.GetNext(pos);
                    ActorCommonData * ptACD = ACDTryToGet(idACD);
                    if (ptACD && ptACD->snoActor == snoSpot)
                    {
                        ptACDSpot = ptACD;
                        break;
                    }
                }
    // 
                if (ptACDSpot)
                {
                    ActorAddStruct tActorAdd;
                    tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_DOESNT_DEACTIVATE);
    // 
    // 
    // 
                    tActorAdd.tPlace = ptACDSpot->GetPlace();
                    tActorAdd.qOrientation.FromYaw(0.0f);
                    tActorAdd.eTeam = TEAM_SCENERY;
                    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;
    // 
                    tActorAdd.tMarkersName = ptACDSpot->tName;
                    tActorAdd.snoActor = snoPortalActor;
    // 
    // 
                    ActorID idActorPortal = GizmoAddBySNO(0, tActorAdd);
                    Actor * ptActorPortal = ActorTryToGet(idActorPortal);
                    if (ptActorPortal)
                    {
                        uFlags |= (1 << nPortalIndex);
    // 
                        sSetUberPortalGameFlag(nPortalIndex, FALSE);
    // 
                        SACDInventoryDeleteItemAfterUse(*ptACDItem);
    // 
    // 
                        SActTransitionSetPreventActTransition(TRUE);
    // 
                        return;
                    }
                }
    // 
            }
    // 
            PlayerDisplayErrorMessage(ptACDOwner->id, GAMEERROR_POWER_UNUSABLE_IN_AREA);
        }
    }
}



static void sSpellEffect_UberBossSpecificPortal0(PowerID idPower, void * pData)
{
    // 
    // 
    sSpellEffect_UberBossPortal(idPower, (ACDID)(intptr_t)pData, 0);
}



static void sSpellEffect_UberBossSpecificPortal1(PowerID idPower, void * pData)
{
    // 
    // 
    sSpellEffect_UberBossPortal(idPower, (ACDID)(intptr_t)pData, 1);
}



static void sSpellEffect_UberBossSpecificPortal2(PowerID idPower, void * pData)
{
    // 
    // 
    sSpellEffect_UberBossPortal(idPower, (ACDID)(intptr_t)pData, 2);
}



static void sSpellEffect_UberBossSpecificPortal3(PowerID idPower, void * pData)
{
    // 
    // 
    sSpellEffect_UberBossPortal(idPower, (ACDID)(intptr_t)pData, 3);
}



static void sSpellEffect_WarpInMagicalStart(PowerID idPower, void *)
{
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);

        tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN) * 60.0f);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
    }
}





static void sSpellEffect_WarpInMagicalContact(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDOwner = ACDTryToGet(ptPower->idACD);
        if (ptACDOwner)
        {
            BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, ptACDOwner, ACDID_INVALID);
            if (idBuff != BUFFID_INVALID)
            {
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
    }
}





static void sPoolPulse(PowerAPIBuffInstance * ptBuff, void * pData)
{
    ACDID idACDPool = (ACDID)(intptr)pData;
    PowerID idPower = ptBuff->idPower;
    ActorCommonData * ptACDPool = ACDTryToGet(idACDPool);
    if (!ptACDPool)
    {
        // 
        return;
    }
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        // 
        // 
        // 
        return;
    }
    uint32 uOldContactFrame = ptPower->dwCurrentContactFrame;
    ptPower->dwCurrentContactFrame = ptACDPool->AttributesGetInt(ATTRIBUTE_STORED_CONTACT_FRAME);
    // 
    Target tTarget;
    tTarget.InitializeFromACD(idACDPool, FALSE, FALSE);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, tTarget, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.bTriggersHitProcs = FALSE;
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    // 
    ptPower->dwCurrentContactFrame = uOldContactFrame;
}
BOOL PowerHelper_IsValidOwner(const PowerID idPower);  // X/Server/Powers/PowerHelper.cpp:1594


static void sSpawnPool(
    Power & tPower,
    ImpactData & tImpactData)
{
    SNO snoPool = PowerContactTagGetSNO(tPower.id, TAG_POWER_POOL_ACTOR);
    if (snoPool == SNO_NONE)
    {
        return;
    }

    ActorCommonData * ptACDProjectile = ACDTryToGet(tImpactData.idACDProjectile);
    if (!ptACDProjectile)
    {
        return;
    }

    float flDuration = PowerGetFormulaValue(tPower.id, TAG_POWER_POOL_DURATION);
    if (flDuration <= EPSILON)
    {
        return;
    }
    uint32 dwDuration = Math::FloatToInt(flDuration * 60.0f);

    float flPulseInterval = Math::Max(0.0f, PowerGetFormulaValue(tPower.id, TAG_POWER_POOL_PULSE_INTERVAL));
    uint32 dwPulseInterval = Math::FloatToInt(flPulseInterval * 60.0f);
    dwPulseInterval = Math::Max<uint32>(1, dwPulseInterval);

    WorldPlace placeProj = ptACDProjectile->GetPlace();

    CRefString sActorName = SNOToString(POWER_SNO, tPower.snoPower, FALSE);
    sActorName += "_Pool";

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);


    tActorAdd.tPlace = placeProj;
    tActorAdd.qOrientation.FromYaw(RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, FULL_CIRCLE));
    tActorAdd.eTeam = TEAM_SCENERY;
    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

    tActorAdd.tMarkersName = MarkerStringHash(sActorName);
    tActorAdd.nLevel = 1;
    tActorAdd.snoActor = snoPool;

    ActorID idActorPool = ActorAdd(tActorAdd);
    if (idActorPool != ACTORID_INVALID)
    {
        ACDID idACDPool = ACDFindByActor(idActorPool);
        ActorCommonData & tACDPool = ACDGet(idACDPool);


        tPower.listAffectedACDs.RemoveAll();

        PowerAPIBuffStartData tBuffStartData(tPower.id);
        tBuffStartData.dwDurationMin = dwDuration;
        tBuffStartData.pPeriodicSpellEffect = sPoolPulse;
        tBuffStartData.pPeriodicSpellEffectData = (void *)(intptr)idACDPool;
        tBuffStartData.dwRefreshRate = dwPulseInterval;
        PowerAPI_BuffAddToACD(tBuffStartData, tACDPool);
        tACDPool.AttributesSetInt(ATTRIBUTE_STORED_CONTACT_FRAME, tPower.dwCurrentContactFrame);
        ActorDeleteSoon(idActorPool, dwDuration);
    }
}



static void sSpellEffect_SuperProjectileImpact(PowerID idPower, void * pData)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
    if (!pdefPower)
    {
        return;
    }

    ImpactData * ptImpact = (ImpactData *)pData;
    if (!ptImpact)
    {
        return;
    }


    if (!PowerHelper_IsValidOwner(idPower))
    {
        ActorID idProjectile = (ActorID)ACDGetOwner(ptImpact->idACDProjectile);
        if (idProjectile != ACTORID_INVALID)
        {
            ActorFlagForDeletion(idProjectile);
        }
        return;
    }



    if (SpellEffect_ProjectileTryReflect(idPower, ptImpact))
    {
        return;
    }


    for (int32 nContactFrame = 0; nContactFrame < 4; nContactFrame++)  // MAX_NUM_CONTACT_FRAMES (PowersStruct.h:24, not defined in src)
    {
        if (PowerIsContactFrameEnabled(pdefPower, nContactFrame))
        {
            ptPower->dwCurrentContactFrame = nContactFrame;


            sImpactDamage(idPower, pData);


            sSpawnPool(*ptPower, *ptImpact);
        }
    }
}




static void sSpellEffect_SuperProjectile(PowerID idPower, void *)
{



    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }

    Power * ptPower = PowerGet(idPower);
    Target tTargetToShootAt;
    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, -1, -1, NULL);

    float flMinDist = PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_MIN_TARGET_DISTANCE);
    float flFallbackDist = (flMinDist > 1e-6f) ? flMinDist : 200.0f;




    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }

    float flJitterAngle = PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_JITTER);
    Angle aJitterOffset = 0.0f;
    if (flJitterAngle > 1e-6f)
    {
        float flRandJitter = RandRangeFloat_Internal(SGameGlobalsGet().rng, flJitterAngle * -0.5f, flJitterAngle);
        aJitterOffset = Math::DegToAngle(flRandJitter);
    }

    BOOL bSuccess = SpellEffect_ProjectileWeaponPower_GetTarget(idPower, tTargetToShootAt, tNavPlaceSource, aJitterOffset);
    if (flMinDist > 1e-6f && bSuccess)
    {


        WorldPlace placeTarget;
        tTargetToShootAt.GetPlace(placeTarget);
        if (tNavPlaceSource.tPlace.idSWorld != placeTarget.idSWorld ||
            Vector3D::Distance2DSquared(tNavPlaceSource.tPlace.wp, placeTarget.wp) < flMinDist * flMinDist)
        {
            flFallbackDist = flMinDist;
            bSuccess = FALSE;
        }
    }




    if (ptPower->tTarget.eType == TT_ACTOR)
    {
        BOOL bLobbed = PowerContactTagGetBool(idPower, TAG_POWER_IS_LOBBED);
        ACDID idACDTarget = ptPower->tTarget.GetACD();
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (!ptACDTarget || (!ptACDTarget->IsAlive() && !bLobbed))
        {
            bSuccess = FALSE;
        }
    }

    if (!bSuccess)
    {
        WorldPlace wpTarget = ptACDOwner->GetPlace();
        Angle aYaw = ACDGetYaw(ptACDOwner->id);
        Vector3D vOffset(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
        Vector3D vSourceOffset = tNavPlaceSource.tPlace.wp - wpTarget.wp;
        vSourceOffset.z = 0.0f;
        vOffset *= flFallbackDist + vSourceOffset.Magnitude();
        wpTarget.wp += vOffset;
        tTargetToShootAt.InitializeFromPlace(wpTarget);
    }

    SpellEffect eSpellEffectToRun = PowerGetNextSpellEffect(idPower);
    PowerAPICreateProjectile tProjectile;
    PowerHelper_ProjectileInit(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, tTargetToShootAt, eSpellEffectToRun, tProjectile);
    PowerAPI_DoProjectile(tProjectile);



    {
        int32 nBonusShots = Math::FloatToInt(ptACDOwner->AttributesGetFloat(ATTRIBUTE_PROJECTILE_AMOUNT_BONUS_PERCENT)) +
                            Math::FloatRoundToInt(PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_COUNT)) - 1;

        float flAngle = Math::DegToAngle(PowerGetFormulaValue(idPower, TAG_POWER_PROJECTILE_SPREAD_ANGLE));
        for (int32 i = 0; i < nBonusShots; i++)
        {

            Angle aYawOffset = flAngle * 0.5f * (i / 2 + 1);
            if (flJitterAngle > 1e-6f)
            {
                float flRandJitter = RandRangeFloat_Internal(SGameGlobalsGet().rng, flJitterAngle * -0.5f, flJitterAngle);
                aYawOffset += Math::DegToAngle(flRandJitter);
            }


            if (i & 1)
            {
                aYawOffset = Math::NormalizeAngle(-aYawOffset);
            }
            if (SpellEffect_ProjectileWeaponPower_GetTarget(idPower, tTargetToShootAt, tNavPlaceSource, aYawOffset))
            {
                PowerHelper_ProjectileInit(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, tTargetToShootAt, eSpellEffectToRun, tProjectile);
                PowerAPI_DoProjectile(tProjectile);
            }
        }
    }
}









static void sSpellEffect_SnakemanCasterElectricBurstChannelTimer(PowerAPIBuffInstance * ptBuff, void *);
static BOOL sSpellEffect_ElectricBurstPulse(PowerID idPower);

static void sElectricBurst(PowerID idPower, int32 nAppearanceParameter)
{
    // 
    // 
    ActorCommonData * ptAcd = PowerGetOwner(idPower);
    if (ptAcd)
    {
        Power * ptPower = PowerGet(idPower);
        ptPower->dwFlags &= ~(1 << PF_INTERRUPTABLE);
        ptPower->dwFlags |= (1 << PF_MANUALLY_FINISH);
        // 
        AnimPlayerPlayAnimation(*ptAcd, TAG_ACTOR_ANIMATION_NORMAL_ATTACK_02, 0xfffffffe, TRUE, ANIM_REASON_POWER, 0.0f, FALSE, 1.0f);
        // 
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.nBuffIndex = 0;
        tBuffStartData.nAppearanceParameter = nAppearanceParameter;
        // 
        uint32 dwDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_BLIZZARD_TIME_BETWEEN_UPDATES) * 60.0f);
        tBuffStartData.pPeriodicSpellEffect = sSpellEffect_SnakemanCasterElectricBurstChannelTimer;
        tBuffStartData.dwRefreshRate = dwDelay;
        // 
        BuffID idBuff = PowerAPI_BuffAddToACD(tBuffStartData, *ptAcd);
        // 
        PowerSetParam(idPower, idBuff);
    }
}

static void sSpellEffect_SnakemanCasterElectricBurst(PowerID idPower, void *)
{
    // 
    // 
    sElectricBurst(idPower, 59);
}

static void sEelDoBurst(PowerAPIBuffInstance * pBuff, ACDID, void *)
{
    // 
    // 
    // 
    sSpellEffect_ElectricBurstPulse(pBuff->idPower);
    PowerForceStop(pBuff->idPower, TRUE);
}

static void sSpellEffect_ElectricEelElectricBurst(PowerID idPower, void *)
{
    // 
    ActorCommonData * ptAcd = PowerGetOwner(idPower);
    if (ptAcd)
    {
        // 
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.nAppearanceParameter = 60;
        tBuffStartData.dwDurationMin = 60;
        tBuffStartData.pExpirationEffect = sEelDoBurst;
        PowerAPI_BuffAddToACD(tBuffStartData, *ptAcd);
    }
}

static void sSpellEffect_SnakemanCasterElectricBurstChannelTimer(PowerAPIBuffInstance * ptBuff, void *)
{
    if (ptBuff)
    {
        Power * ptPower = PowerTryToGet(ptBuff->idPower);
        if (ptPower)
        {
            // 
            int32 nDurationMin = Math::FloatToInt(PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_DURATION_MIN) * 60.0f);
            int32 nElapsedTicks = GameGetServerTime() - ptPower->dwStartTick;
            BOOL bHitPlayer = sSpellEffect_ElectricBurstPulse(ptBuff->idPower);
            if (!bHitPlayer || nElapsedTicks > nDurationMin)
            {
                // 
                AnimPlayerPlayAnimation(*PowerGetOwner(ptBuff->idPower), TAG_ACTOR_ANIMATION_NORMAL_ATTACK_01_END, 0xffffffff, TRUE, ANIM_REASON_POWER, 0.0f, FALSE, 1.0f);
                PowerInterrupt(ptBuff->idPower, TRUE);
            }
        }
    }
}


static BOOL sSpellEffect_ElectricBurstPulse(PowerID idPower)
{
    BOOL bHitPlayer = FALSE;
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptAcd = PowerGetOwner(ptPower);
    if (!ptAcd)
    {
        return FALSE;
    }
    PowerAPIBuffStartData tImmunityBuff(idPower);
    uint32 dwRefreshRate = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_BLIZZARD_TIME_BETWEEN_UPDATES) * 60.0f);
    tImmunityBuff.dwDurationMin = dwRefreshRate - 1;
    tImmunityBuff.nBuffIndex = 3;
    WorldPlace tPlace = ptAcd->GetPlace();
    Target tTarget;
    tTarget.InitializeFromPlace(tPlace);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitFromPayload(idPower, tPlace, tTarget, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        ActorCommonData * ptACDTarget = ACDTryToGet((ACDID)listTargets.GetNext(pos));
        if (ptACDTarget && PowerAPI_BuffFindOnACD(ptPower->snoPower, ptACDTarget, ACDID_INVALID) == BUFFID_INVALID)
        {
            // 
            // 
            // 
            // 
            if (ACDIsPlayer(ptACDTarget->id))
            {
                bHitPlayer = TRUE;
            }
            // 
            // 
            // 
            PowerAPI_DoPayload(tPayload, ptACDTarget->id, ACDID_INVALID);
            PowerAPI_BuffAddToACD(tImmunityBuff, *ptACDTarget);
        }
    }
    // 
    // 
    // 
    return bHitPlayer;
}


static void sSpellEffect_SnakemanCasterElectricBurstInterrupted(PowerID idPower, void * pData)
{
    BuffID idBuff = PowerGetParam(idPower);
    if (idBuff == BUFFID_INVALID && pData)
    {
        idBuff = (BuffID)(intptr)pData;
    }
    PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
}






static void sSpellEffect_SnakemanCasterElectricBurstStart(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ptPower->tTarget.Clear();
    }
}





static void sSpellEffect_StealthIn(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 1);
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_STEALTH_BUFF);
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.snoPower = snoBuff;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_STEALTHED, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE_BY_PETS, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST, TRUE);
        tBuffStartData.nAppearanceParameter = 57;
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}







static void sSpellEffect_StealthOut(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 0);
        SNO snoStartBuff = GlobalSNOGet(GLOBALSNO_POWERS_STEALTH_BUFF);
        BuffID idStartBuff = PowerAPI_BuffFindOnACD(snoStartBuff, ptACD, ACDID_INVALID);
        if (idStartBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idStartBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }
}





static void sSpellEffect_SnakemanMeleeStealth(PowerID idPower, void *)
{
    sSpellEffect_StealthIn(idPower, NULL);
}



static void sSpellEffect_SnakemanMeleeUnstealth(PowerID idPower, void *)
{
    sSpellEffect_StealthOut(idPower, NULL);
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        SNO snoEffectGroup = PowerContactTagGetSNO(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
        if (snoEffectGroup != SNO_NONE)
        {
            ACDPlayEffect(ptACDOwner->id, EFFECT_PLAY_EFFECT_GROUP, snoEffectGroup, ACDID_INVALID);
        }
    }
}


void PowerGetNavPlaceFromACD(const ActorCommonData * ptACD, NavPlace & tNavPlace);  // X/Common/Powers/PowersCommon.cpp:5129


static void sSpellEffect_FallenChampionLeaderShout(PowerID idPower, void *)
{



    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());


    float flRadius = PowerGetFormulaValue(idPower, TAG_POWER_FALLEN_CHAMPION_LEADER_SHOUT_EFFECT_RADIUS);


    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    NavPlace tNavPlace;
    PowerGetNavPlaceFromACD(ptACDOwner, tNavPlace);


    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, flRadius, tChooseTargets.tGeometry);


    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    PowerAPIBuffStartData tWarningShoutBuffStartData(idPower);
    tWarningShoutBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_FALLEN_CHAMPION_LEADER_SHOUT_DURATION) * 60.0f);
    tWarningShoutBuffStartData.nAppearanceParameter = 49;
    tWarningShoutBuffStartData.bPositiveEffect = TRUE;
    tWarningShoutBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    FastAttribSetValueFloat(tWarningShoutBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_PERCENT_ALL));
    FastAttribSetValueFloat(tWarningShoutBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR, PowerGetFormulaValue(idPower, TAG_POWER_MOVEMENT_SPEED_PERCENT_INCREASE_MIN));
    FastAttribSetValueInt(tWarningShoutBuffStartData.idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, TRUE);


    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);

        if (tACDTarget.tSNOName.snoHandle == GlobalSNOGet(GLOBALSNO_MONSTER_FALLEN_GRUNT))
        {
            ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);
            AIStateSetState(idActorTarget, 2, FALSE);
            PowerAPI_BuffAddToACD(tWarningShoutBuffStartData, tACDTarget);
            if (ActorPowersGetPrimaryPower(idActorTarget) == SNO_NONE)
            {
                BrainStanceDoPower(idActorTarget, cgTargetNone, GlobalSNOGet(GLOBALSNO_POWERS_FALLEN_GRUNT_SHOUT), BRAIN_POWER_START, NULL, NULL, 0, 0);
            }
        }
    }
}

static void sFallenHoundHitBuffOnHit(PowerAPIBuffInstance * ptBuff, void *, PowerAPIDamage &, float, float *, ACDID, ACDID, ACDID)
{
    PowerAPI_BuffFlagForDeletion(ptBuff->id, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
}









static void sSpellEffect_FallenHoundHitBuff(const PowerID idPower, void *)
{
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bRemoveOnDeath = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.pOnHitEffect = sFallenHoundHitBuffOnHit;
    float flDamagePercent = PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_PERCENT_ALL);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, flDamagePercent);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
}





static void sSpellEffect_FallenHound_Leap(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    const ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);

    if (!ptACDOwner)
    {
        return;
    }

    ACDID idACDTarget = PowerGetTargetACD(ptPower);
    const ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        return;
    }


    WorldPlace tDest = ptACDTarget->GetPlace();
    WorldPlace tStart = ptACDOwner->GetPlace();
    if (tDest.idSWorld != tStart.idSWorld)
    {
        return;
    }

    Vector3D vToTarget = tDest.wp - tStart.wp;
    vToTarget.Normalize();
    float flMin = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    float flDelta = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
    float flRandDist = RandRangeFloat_Internal(SGameGlobalsGet().rng, flMin, flDelta);

    vToTarget *= flRandDist;

    tDest.wp += vToTarget;
    const ActorMovement * ptMove = ptACDOwner->GetActorMovement();
    PowerComputeDestination(
        ptPower->snoPower, *ptMove, tDest, tDest, FALSE, 20.0f);






    ptPower->tTarget.InitializeFromPlace(tDest);
}


SNO SpellEffect_GetWeaponPower(ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);
    ACDID idACDWeapon = ACDInventoryGetActiveWeapon(tACD);
    ActorCommonData * ptACDWeapon = ACDTryToGet(idACDWeapon);
    if (ptACDWeapon)
    {
        SNO snoWeaponPower = ptACDWeapon->AttributesGetPower(0);
        if (snoWeaponPower != SNO_NONE)
        {
            return snoWeaponPower;
        }
    }
    return GlobalSNOGet(GLOBALSNO_POWERS_PUNCH);
}






void sDuneDervish_WhirlwindUpdate(PowerAPIBuffInstance * ptBuff, void *)
{


    PowerID idPower = ptBuff->idPower;
    Power * ptPower = PowerGet(idPower);

    if (!ptPower)
    {
        return;
    }

    ACDID idACD = ptPower->idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }


    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());

    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);



    SNO snoWeaponPower = SpellEffect_GetWeaponPower(ptACD->id);


    if (PowerGetPayload(snoWeaponPower, 0) == 5)  // PayloadType 5; enumerator names are not in the DWARF
    {
        tTargets.tGeometry.tSphere.wdRadius = PowerGetRange(ptACD, AutoPowerDefinitionR(snoWeaponPower), TRUE);
    }

    float flRangeMultiplier = PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWHIND_RANGE_MULTIPLIER);
    float flRangeBonus = PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWHIND_RANGE_BONUS);
    tTargets.tGeometry.tSphere.wdRadius *= flRangeMultiplier;
    tTargets.tGeometry.tSphere.wdRadius += flRangeBonus;

    PowerAPI_DoChooseTargets(tTargets, listTargets);


    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);

    tPayload.eDeliveryMechanism = PDM_MELEE;

    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}




static BOOL sDuneDervish_ReflectEffect(PowerAPIBuffInstance * ptBuff, ImpactData * ptImpact, void *)



{

    ACDID idACDTarget = ptImpact->idACDProjectile;

    ActorCommonData * ptACDCenter = PowerGetOwner(ptBuff->idPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!(ptACDCenter || ptACDTarget))
    {
        return FALSE;
    }


    Team eTeamPower = PowerGetTeam(ptBuff->idPower);
    BOOL bTeamsAreAllied = FALSE;


    const ActorCommonData * ptACDProjectileOwner = ProjectileGetOwnerACD(idACDTarget);
    if (ptACDProjectileOwner)
    {
        Team eTeamTarget = ptACDProjectileOwner->GetTeam();
        bTeamsAreAllied = TeamsAreFriendly(eTeamTarget, eTeamPower);
    }



    if (!ProjectileIsReflected(idACDTarget) && bTeamsAreAllied)
    {
        return FALSE;
    }



    WorldPlace wpOwner = ptACDCenter->GetPlace();
    WorldPlace wpProjectile = ptACDTarget->GetPlace();
    Vector3D wvToTarget;
    WorldPlaceGetVectorBetween(wpProjectile, wpOwner, wvToTarget);

    ActorMovement * ptMove = ptACDTarget->GetActorMovement();

    Vector3D wvCurrentVelocity = ptMove->GetVelocity();


    Angle aDifference = Vector3D::AngleBetween(wvToTarget, wvCurrentVelocity);
    if (aDifference > QUARTER_CIRCLE && aDifference < QUARTER_CIRCLE * 3.0f)
    {
        return FALSE;
    }

    Vector3D wvFacing(cgwvXAxis);
    WorldPlace placeDest;
    Angle aFacing = ptACDCenter->GetOrientation().ToYaw();
    Vector3D::Rotate(cgwvZAxis, aFacing, wvFacing);


    Angle aReflectionOffset = QUARTER_CIRCLE - RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, HALF_CIRCLE);

    BOOL bSuccess = ProjectileReflect(idACDTarget, ptACDCenter->id, TRUE, aReflectionOffset, eTeamPower, 1.0f);
    PowerID idPowerProj = ActorPowersGetOwnerPower((ActorID)ACDGetOwner(idACDTarget));
    Power * ptPowerProj = PowerTryToGet(idPowerProj);
    if (ptACDProjectileOwner && ptPowerProj)
    {
        float flHealthPercentCap = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_REFLECT_DAMAGE_HEALTH_PERCENT_CAP);
        if (flHealthPercentCap > EPSILON)
        {
            FastAttribSetValueInt(ptPowerProj->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_HAS_DAMAGE_CAP, TRUE);
            if (ptACDProjectileOwner->IsPet())
            {
                SNO snoSummon = FastAttribGetValueInt(ptACDProjectileOwner->idFastAttrib, ATTRIBUTE_SUMMONED_BY_SNO);
                ActorCommonData * ptACDSummonner = ACDTryToGet((ACDID)FastAttribGetValueInt(ptACDProjectileOwner->idFastAttrib, ATTRIBUTE_SUMMONED_BY_ACDID));
                if (snoSummon != -2 && ptACDSummonner)
                {
                    FastAttribSetValueFloat(ptPowerProj->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_CAP_REMAINING, flHealthPercentCap * ptACDSummonner->GetHitPointsMaxTotal());
                }
            }
            else
            {
                FastAttribSetValueFloat(ptPowerProj->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_CAP_REMAINING, flHealthPercentCap * ptACDProjectileOwner->GetHitPointsMaxTotal());
            }
        }
    }




    return bSuccess;
}

static BOOL sDuneDervish_ShouldReflectTest(PowerAPIBuffInstance * ptBuff, ImpactData * ptImpact, void *)



{

    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        return FALSE;
    }

    ActorCommonData * ptACDProjectile = ACDTryToGet(ptImpact->idACDProjectile);
    if (!ptACDProjectile)
    {
        return FALSE;
    }


    ScopedActorDefinitionLockR pdefActor(ptACDProjectile->snoActor);
    BOOL bCanBeReflected = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_CAN_BE_REFLECTED);
    if (!bCanBeReflected)
    {
        return FALSE;
    }

    uint32 dwNow = GameGetServerTime();
    uint32 dwElapsedBuffTime = dwNow - ptBuff->dwStartTick;
    uint32 dwReflectBufferTicks = Math::FloatToInt(PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_NO_REFLECT_BUFFER_TIME) * 60.0f);

    if (dwElapsedBuffTime < dwReflectBufferTicks)
    {
        return FALSE;
    }
    if (ptBuff->dwDuration - dwElapsedBuffTime < dwReflectBufferTicks)
    {
        return FALSE;
    }





    return TRUE;
}

static const uint32 DUNE_DERVISH_STEERING_UPDATE_TICKS = 20;
static void sDuneDervish_SteeringUpdate(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);

    if (!ptACDOwner)
    {
        return;
    }

    ActorID idActor = (ActorID)ptACDOwner->idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    ACDID idACDTarget = PowerGetTargetACD(ptPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);

    if (ptActor && ptActor->ptSActorMovement && ptACDTarget)
    {
        WorldPlace placeTarget = ptACDTarget->GetPlace();
        uint32 uMoveFlags = (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING) | (1 << MOVEBIT_FROM_POWER) | (1 << MOVEBIT_DONT_AUTOPLAY_IDLE) | (1 << MOVEBIT_ACTIVATE_TRIGGERS);


        if (IsTriggerEventTargetingActor(idActor))
        {
            uMoveFlags |= (1 << MOVEBIT_ACTIVATE_TRIGGERS);
        }

        WorldDistance wdSpeed = PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_MOVE_SPEED);
        ptActor->ptSActorMovement->MoveTo(placeTarget, wdSpeed, 0.0f, (ActorAnimationTag)-1, uMoveFlags, 0.0f, idACDTarget, ptPower->snoPower, 0, -1, 0.0f, NULL);
    }

    PowerTriggerAddTimerTrigger(idPower, DUNE_DERVISH_STEERING_UPDATE_TICKS, sDuneDervish_SteeringUpdate, NULL);
}




void sDuneDervish_WhirlwindBuffExpired(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    if (ptBuff)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptBuff->idPower);
        ActorID idActor = ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID;
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor && ptActor->ptSActorMovement)
        {
            ptActor->ptSActorMovement->StopNow();
        }
        PowerForceStop(ptBuff->idPower, TRUE);
    }
}












void sSpellEffect_DuneDervish_WhirlwindBegin(PowerID idPower, void *)
{



    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }

    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }



    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.nBuffIndex = 0;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;


    FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    tBuffStartData.pShouldReflectTest = sDuneDervish_ShouldReflectTest;
    tBuffStartData.pReflectEffect = sDuneDervish_ReflectEffect;
    tBuffStartData.pPeriodicSpellEffect = sDuneDervish_WhirlwindUpdate;
    tBuffStartData.pRemovalEffect = sDuneDervish_WhirlwindBuffExpired;
    tBuffStartData.dwRefreshRate = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_PULSE_RATE) * 60.0f);
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_DURATION_MIN) * 60.0f);
    tBuffStartData.dwDurationDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_WHIRLWIND_DURATION_DELTA) * 60.0f);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, TRUE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_ABSORB_PERCENT, PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_REDUCTION_PERCENT));
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);

    PowerTriggerAddTimerTrigger(idPower, DUNE_DERVISH_STEERING_UPDATE_TICKS, sDuneDervish_SteeringUpdate, NULL);
}




void sSpellEffect_DuneDervish_WhirlwindEnd(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, ACDGet(ptPower->idACD), ACDID_INVALID);
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}


BOOL PowerActorCanResurrect(ActorID idActor);  // X/Server/ActorIsAffected.cpp:366
enum { MF_FALLEN = 1 };  // X/Common/ActorCommonDataSwig.h:31 (kept local, as MF_TREASURE_GOBLIN in SGame.cpp)


void sSpellEffect_FallenShaman_Ressurect(PowerID idPower, void *)
{



    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);

    if (!ptACDOwner)
    {
        return;
    }

    NavPlace tNavPlace;
    PowerGetNavPlaceFromACD(ptACDOwner, tNavPlace);


    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());

    PowerAPIChooseTargets tTargets;
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, 40.0f, tChooseTargets.tGeometry);


    PowerAPI_DoChooseTargets(tTargets, listTargets);

    ACDID idACDPrimaryTarget = PowerGetTargetACD(idPower);
    listTargets.AddHead_Tracked(idACDPrimaryTarget);



    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDResurrected = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDResurrected = ACDTryToGet(idACDResurrected);
        if (!ptACDResurrected)
        {
            continue;
        }





        if (ptACDOwner->GetPlace().idSWorld != ptACDResurrected->GetPlace().idSWorld)
        {
            continue;
        }

        ActorID idCorpse = (ActorID)ptACDResurrected->idOwner;

        if (!PowerActorCanResurrect(idCorpse))
        {
            continue;
        }


        ScopedActorDefinitionLockR pdefActor(ptACDResurrected->tSNOName.snoHandle);
        ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
        bool bIsFallen = pdefMonster->eMonsterFamily == MF_FALLEN;
        if (!bIsFallen)
        {
            continue;
        }


        PowerHelper_DoFirstTimeCosts(idPower, TRUE);


        BrainAPIDoResurrect(idCorpse, FALSE);
        BrainAPIDoPlayAnimation(idCorpse, TAG_ACTOR_ANIMATION_RESURRECTION, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);


        ptACDResurrected->AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        ptACDResurrected->AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
        ptACDResurrected->AttributesSetInt(ATTRIBUTE_RESURRECTED, TRUE);
        break;
    }
}



static BOOL sSpellEffect_SandMonster_SandWall_ReflectTest(PowerAPIBuffInstance * ptBuff, ImpactData * ptImpact, void *)



{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        return FALSE;
    }

    ACDID idACDProjectile = ptImpact->idACDProjectile;

    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (!ptACDProjectile)
    {
        return FALSE;
    }

    ScopedActorDefinitionLockR pdefActor(ptACDProjectile->snoActor);
    BOOL bCanBeReflected = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_CAN_BE_REFLECTED);
    if (!bCanBeReflected)
    {
        return FALSE;
    }

    return TRUE;
}


static BOOL sSpellEffect_SandMonster_SandWall_Reflect(PowerAPIBuffInstance * ptBuff, ImpactData * ptImpact, void *)



{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        return FALSE;
    }


    ACDID idACDTarget = ptImpact->idACDProjectile;

    ActorCommonData * ptACDCenter = PowerGetOwner(ptBuff->idPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDCenter == NULL || ptACDTarget == NULL)
    {
        return FALSE;
    }

    ScopedActorDefinitionLockR pdefActor(ptACDTarget->snoActor);
    BOOL bCanBeReflected = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_CAN_BE_REFLECTED);
    if (!bCanBeReflected)
    {
        return FALSE;
    }


    ProjectileSetSpeedMultiplier(idACDTarget, 2.0f, -1, TRUE, TRUE, FALSE);


    Team eTeamPower = PowerGetTeam(ptBuff->idPower);
    BOOL bTeamsAreAllied = FALSE;


    const ActorCommonData * ptACDProjectileOwner = ProjectileGetOwnerACD(idACDTarget);
    if (ptACDProjectileOwner)
    {
        Team eTeamTarget = ptACDProjectileOwner->GetTeam();
        bTeamsAreAllied = TeamsAreFriendly(eTeamTarget, eTeamPower);
    }



    if (!ProjectileIsReflected(idACDTarget) && bTeamsAreAllied)
    {
        return FALSE;
    }



    WorldPlace wpOwner = ptACDCenter->GetPlace();
    WorldPlace wpProjectile = ptACDTarget->GetPlace();
    Vector3D wvToTarget;
    WorldPlaceGetVectorBetween(wpProjectile, wpOwner, wvToTarget);

    ActorMovement * ptMove = ptACDTarget->GetActorMovement();

    Vector3D wvCurrentVelocity = ptMove->GetVelocity();


    Angle aDifference = Vector3D::AngleBetween(wvToTarget, wvCurrentVelocity);
    if (aDifference > QUARTER_CIRCLE && aDifference < QUARTER_CIRCLE * 3.0f)
    {
        return FALSE;
    }

    Vector3D wvFacing(cgwvXAxis);
    WorldPlace placeDest;
    Angle aFacing = ptACDCenter->GetOrientation().ToYaw();
    Vector3D::Rotate(cgwvZAxis, aFacing, wvFacing);

    float flDeflectionJitterAngle = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SAND_WALL_DEFLECTION_JITTER);
    Angle DeflectionAngle = Math::DegToAngle(flDeflectionJitterAngle);

    DeflectionAngle = Math::Max(FLT_EPSILON, DeflectionAngle);





    Angle aReflectionOffset = DeflectionAngle * 0.5f - RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, DeflectionAngle);

    ProjectileReflect(idACDTarget, ptACDCenter->id, TRUE, aReflectionOffset, eTeamPower, 1.0f);

    PowerID idPowerProj = ActorPowersGetOwnerPower((ActorID)ACDGetOwner(idACDTarget));
    Power * ptPowerProj = PowerTryToGet(idPowerProj);
    if (ptACDProjectileOwner && ptPowerProj)
    {
        float flHealthPercentCap = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_REFLECT_DAMAGE_HEALTH_PERCENT_CAP);
        if (flHealthPercentCap > EPSILON)
        {
            FastAttribSetValueInt(ptPowerProj->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_HAS_DAMAGE_CAP, TRUE);
            FastAttribSetValueFloat(ptPowerProj->idFastAttrib, ATTRIBUTE_PROJECTILE_REFLECT_DAMAGE_CAP_REMAINING, flHealthPercentCap * ptACDProjectileOwner->GetHitPointsMaxTotal());
        }
    }




    return TRUE;
}


void sSpellEffect_SandMonster_SandWall(PowerID idPower, void *)
{


    uint32 dwBuffDurationMin = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SAND_WALL_DURATION) * 60.0f);
    uint32 dwBuffDurationDelta = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SAND_WALL_DURATION_DELTA) * 60.0f);

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.bRemoveOnDeath = TRUE;
    tBuffStartData.dwDurationMin = dwBuffDurationMin; tBuffStartData.dwDurationDelta = dwBuffDurationDelta;

    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.pReflectEffect = sSpellEffect_SandMonster_SandWall_Reflect;
    tBuffStartData.pShouldReflectTest = sSpellEffect_SandMonster_SandWall_ReflectTest;
    tBuffStartData.nAppearanceParameter = 61;

    for (uint32 i = DAMAGE_TYPE_FIRE; i < NUM_DAMAGE_TYPES; i++)




    {
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, i), 100.0f);
    }

    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDOwner);
}


void sSpellEffect_SoaringAscend(PowerID idPower, void *)
{
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        sBurrowIn(idPower, 1, -1, TRUE);
        ptACDOwner->tCollSettings.uStaticOneRequiredFlags = 7;
        ptACDOwner->ModifiedMark(ACDMODIFIED_COLL_FLAGS);
    }
}









void sSpellEffect_SoaringDescend(PowerID idPower, void *)
{
    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (ptACDOwner)
    {
        PowerAPI_DoBurrowOut(ptACDOwner, FALSE);
        ptACDOwner->tCollSettings.uStaticOneRequiredFlags = 1;
        ptACDOwner->ModifiedMark(ACDMODIFIED_COLL_FLAGS);
    }
}

// X/Server/Powers/PowerAPI.h, X/Server/AI/ActorAI.h
void PowerAPI_ForceTarget(ACDID idACDReceiver, ACDID idACDForcedTarget);  // PowerAPI.cpp:7683
void ActorAISetTauntTarget(ActorID idActorTauntee, ACDID idACDTaunter);  // ActorAI.cpp:2107
void PowerAPI_BuffDealDOTDamage(PowerAPIBuffInstance & tBuff, uint32 uInterval);  // PowerAPI.cpp:12342




static void sSpellEffect_TreasureGoblin_PortalPulse(PowerAPIBuffInstance * ptBuff, void * pData)
{


    PowerID idPower = ptBuff->idPower;
    ACDID idACDPortal = (ACDID)(intptr)pData;
    ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);

    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ACDID idACDGoblin = ptPower->idACD;
    ActorCommonData * ptACDGoblin = ACDTryToGet(idACDGoblin);

    if (!ptACDPortal)
    {
        return;
    }


    if (!ptACDGoblin)
    {

        BrainAPIDoDeath((ActorID)ACDGetOwner(idACDPortal), ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        return;
    }

    WorldPlace tGoblinPlace = ptACDGoblin->GetPlace();
    WorldPlace tPortalPlace = ptACDPortal->GetPlace();
    if (tPortalPlace.idSWorld != tGoblinPlace.idSWorld)
    {
        return;
    }

    float flDistToPortalSq = Vector3D::Distance2DSquared(tGoblinPlace.wp, tPortalPlace.wp);
    float flDesiredDist = PowerGetFormulaValue(idPower, 0xa7400);  // tag 684032; src's only enumerator for this value (TAG_POWER_OVERRIDE_HIT_EFFECTS) does not fit this use
    if (flDistToPortalSq < flDesiredDist * flDesiredDist)
    {

        Target tTarget;
        tTarget.InitializeFromACD(idACDPortal, 0, 0);
        SNO snoUsePortalPower = GlobalSNOGet(GLOBALSNO_POWERS_GOBLIN_USE_PORTAL);
        PowerStart(idACDGoblin, snoUsePortalPower, tTarget);
    }
}


void sSpellEffect_GoblinUsePortal(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    Target tPortal;
    PowerGetTarget(idPower, tPortal);
    BrainAPIDoDeath((ActorID)ACDGetOwner(tPortal.idACDTarget), ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    ActorDeleteSoon((ActorID)ptACD->idOwner, 1);
}






void sSpellEffect_GoblinThrowPortal(PowerID idPower, void * pData)
{


    Power * ptPower = PowerGet(idPower);
    WorldPlace tRandomTargetPlace;
    ActorCommonData * ptACD = PowerGetOwner(ptPower);

    if (!ptACD)
    {
        return;
    }

    tRandomTargetPlace = ptACD->GetPlace();
    float flRandomDistance = PowerGetFormulaValue(idPower, 0xa7401);  // tag 684033; src's enumerator for this value (TAG_POWER_CRITICAL_HIT_EFFECT) does not fit this use
    RNG & rng = SGameGlobalsGet().rng;
    flRandomDistance += RandFloat_Internal(rng) * PowerGetFormulaValue(idPower, 0xa7402);  // tag 684034 (TAG_POWER_HIT_EFFECT in src), as above

    tRandomTargetPlace.wp += ptACD->GetForward() * flRandomDistance;

    WorldPlace tModifiedPlace;

    const ActorMovement * ptACM = ptACD->GetActorMovement();

    if (!ptACM)
    {
        return;
    }

    ACDCollSettings tSettingsDuring, tSettingsDest;
    memset(&tSettingsDuring, 0, sizeof(tSettingsDuring));
    memset(&tSettingsDest, 0, sizeof(tSettingsDest));
    ptACM->GetCollSettings(ptPower->snoPower, POWER_PASSABILITY_DURING, tSettingsDuring);
    ptACM->GetCollSettings(tSettingsDest);
    WorldPlace placeActor;
    ptACM->GetPlace(placeActor);
    PowerComputeDestination(tSettingsDuring, tSettingsDest, *ptACM, tRandomTargetPlace, placeActor, tModifiedPlace, 0, 50.0f, NULL);

    ptPower->tTarget.InitializeFromPlace(tModifiedPlace);

    SpellEffect_Projectile(idPower, NULL);

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);



    tActorAdd.tPlace = ptACD->GetPlace();
    tActorAdd.qOrientation.FromYaw(RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, FULL_CIRCLE));
    tActorAdd.eTeam = TEAM_SCENERY;
    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

    tActorAdd.tMarkersName = MarkerStringHash("TargetProxy");
    tActorAdd.nLevel = 1;
    tActorAdd.snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_TREASURE_GOBLIN_PORTAL_PROXY);

    ActorID idActor = ActorAdd(tActorAdd);
    PowerSetParam(idPower, idActor);
    ACDID idACDTargetProxy = ACDFindByActor(idActor);

    PowerAPI_ForceTarget(ptACD->id, idACDTargetProxy);
    ptACD->AttributesRefreshTimerGroup(ATT_TAUNT, 0xffff);
    ActorAISetTauntTarget((ActorID)ptACD->idOwner, idACDTargetProxy);
}



void sSpellEffect_GoblinSpawnPortal(PowerID idPower, void * pData)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDGoblin = PowerGetOwner(ptPower);
    if (!ptACDGoblin)
    {
        return;
    }

    ImpactData * pImpactData = (ImpactData *)pData;
    NavPlace tNavPlace;
    tNavPlace.tPlace.wp = pImpactData->vPosition;
    tNavPlace.tPlace.idSWorld = ptACDGoblin->GetPlace().idSWorld;
    PowerPlaceToNavMeshHandle(tNavPlace.tPlace, tNavPlace.tNavMeshHandle);
    Quaternion qParentRot = ptACDGoblin->GetOrientation();
    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);

    ACDID idACDPortal = SpellEffect_SpawnActor(idPower, GlobalSNOGet(GLOBALSNO_ACTOR_TREASURE_GOBLIN_PORTAL), tNavPlace, qParentRot, dwAddFlags, FALSE);

    ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);
    if (!ptACDPortal)
    {
        return;
    }



    ptACDPortal->SetInvulnerable(TRUE);


    ActorID idProxy = (ActorID)PowerGetParam(idPower);
    ActorDeleteSoon(idProxy, 1);
    PowerAPI_ForceTarget(ptACDGoblin->id, idACDPortal);
    ActorAISetTauntTarget((ActorID)ptACDGoblin->idOwner, idACDPortal);


    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.pPeriodicSpellEffect = sSpellEffect_TreasureGoblin_PortalPulse;
    tBuffStartData.pPeriodicSpellEffectData = (void *)(intptr)idACDPortal;
    tBuffStartData.dwRefreshRate = 12;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bRemoveOnDeath = TRUE;
    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDPortal));
}


static void sSpellEffect_CreepMob_Plunge_Cleanup(PowerID idPower, void *)
{
    PowerForceStop(idPower, TRUE);
}




static void sSpellEffect_CreepMob_End_Plunge(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(ptPower->snoPower, ptACD, ptACD->id);
        PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        ACDID idACDProjectile = (ACDID)ptPower->dwParam;
        ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
        if (ptACDProjectile)
        {
            ActorFlagForDeletion((ActorID)ptACDProjectile->idOwner);
        }
    }
}










static inline void sSpellEffect_CreepMob_Finish_Plunge(Power * ptPower, ActorCommonData * ptACD)
{
    AnimPlayerPlayNextAnimation(*ptACD);
    AnimationTiming tAnimationTiming;
    AnimPlayerGetAnimationTiming(*ptACD, ptPower->dwAnimationTag, ANIM_USE_ANIM_LENGTH, 1.0f, tAnimationTiming);
    uint32 dwCooldownAnimDuration = tAnimationTiming.tAnimSpec[ANIMOFFSET_END].dwDuration;
    PowerTriggerAddTimerTrigger(ptPower->id, dwCooldownAnimDuration, sSpellEffect_CreepMob_Plunge_Cleanup, NULL);
}








static void sSpellEffect_CreepMob_Kill_Creeper(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void * ptData)
{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        ACDID idACDCreeper = (ACDID)(intptr)ptData;
        ActorID idActorCreeper = (ActorID)ACDGetOwner(idACDCreeper);
        if (idActorCreeper != ACTORID_INVALID)
        {
            BrainAPIDoDeath(idActorCreeper, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        }
        sSpellEffect_CreepMob_Finish_Plunge(ptPower, ptACD);
    }
}









static void sSpellEffect_CreepMob_Plunge_Pulse(PowerAPIBuffInstance * ptBuff, void * ptData)
{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    ACDID idACDCreeper = (ACDID)(intptr)ptData;
    if (ptACD)
    {
        ActorCommonData * ptACDCreeper = ACDTryToGet(idACDCreeper);
        if (ptACDCreeper)
        {
            WorldPlace placeCurr = ptACDCreeper->GetPlace();
            Target tGroundTarget;
            tGroundTarget.InitializeFromPlace(placeCurr);
            SNO snoCreeperAttackPower = GlobalSNOGet(GLOBALSNO_POWERS_CREEPMOB_CREEPER_ATTACK);
            PowerStart(ptACD->id, snoCreeperAttackPower, tGroundTarget);
        }
    }
}









static void sSpellEffect_CreepMob_Arm_Plunge(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    // 
    // 
    Target tTarget;
    PowerGetTarget(idPower, tTarget);
    // 
    if (tTarget.IsTargetingACD())
    {
    // 
    // 
        ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.GetACD());
        if (!ptACDTarget)
        {
            return;
        }
        ActorID idActor = (ActorID)ptACD->idOwner;
        ActorID idTarget = (ActorID)ptACDTarget->idOwner;
        BrainAPIUpdateTargets(idActor, TRUE);
        if (!BrainAPIActorHasTarget(idActor, idTarget))
        {
    // 
    // 
    // 
    // 
            sSpellEffect_CreepMob_Finish_Plunge(ptPower, ptACD);
            return;
        }
    }
    // 
    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, -1, -1, NULL);
    // 
    // 
    float flRandomDistance = PowerGetFormulaValue(idPower, TAG_POWER_PORTAL_MIN_RANGE);
    RNG & rng = SGameGlobalsGet().rng;
    flRandomDistance += RandFloat_Internal(rng) * PowerGetFormulaValue(idPower, TAG_POWER_PORTAL_DELTA_RANGE);
    Angle flRandomYaw = RandRangeFloat_Internal(rng, 0.0f, FULL_CIRCLE);
    Vector3D vDirection(Math::Cos(flRandomYaw), Math::Sin(flRandomYaw), 0.0f);
    // 
    WorldPlace tTargetPlace;
    tTarget.GetPlace(tTargetPlace);
    // 
    WorldPlace tRandomTargetPlace;
    tRandomTargetPlace = tTargetPlace;
    tRandomTargetPlace.wp += vDirection * flRandomDistance;
    // 
    const ActorMovement * ptACM = ptACD->GetActorMovement();
    if (ptACM)
    {
        PowerComputeDestination(ptPower->snoPower, *ptACM, tRandomTargetPlace, tTargetPlace, tRandomTargetPlace, 0, 10.0f);
    }
    tTarget.InitializeFromPlace(tRandomTargetPlace);
    // 
    // 
    // 
    SpellEffect eSpellEffectToRun = PowerGetNextSpellEffect(idPower);
    PowerAPICreateProjectile tProjectile;
    PowerHelper_ProjectileInit(idPower, ptPower->idACDAlternateSource, tNavPlaceSource, tTarget, eSpellEffectToRun, tProjectile);
    tProjectile.tProjectileData.tDurationData.dwTime = (uint32)(Vector3D::Distance2D(tProjectile.tProjectileData.tAnimationData.placeStart.wp, tRandomTargetPlace.wp) / tProjectile.tProjectileData.tAnimationData.wdSpeed);
    tProjectile.tProjectileData.dwFlags |= (1 << PROJECTILE_HUGS_GROUND_BIT);
    ACDID idACDProjectile = PowerAPI_DoProjectile(tProjectile);
    ptPower->dwParam = idACDProjectile;
}



static void sSpellEffect_CreepMob_Spawn_Creeper(PowerID idPower, void * pData)
{
    ImpactData * pImpactData = (ImpactData *)pData;
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD || !ptACD->IsAlive())
    {
        return;
    }
    // 
    ActorID idActor = (ActorID)ptACD->idOwner;
    PowerID idPrimaryPower = ActorPowersGetPrimary(idActor);
    if (idPrimaryPower != idPower)
    {
        return;
    }
    // 
    // 
    // 
    // 
    // 
    Target tImpactTarget;
    // 
    SpellEffect_GetProjectileImpactData(pImpactData->idACDProjectile, pImpactData->idACDIntersected, tImpactTarget);
    WorldPlace tTargetPlace;
    tImpactTarget.GetPlace(tTargetPlace);
    NavPlace tNavPlace;
    tNavPlace.tPlace.wp = tTargetPlace.wp;
    tNavPlace.tPlace.idSWorld = ptACD->GetPlace().idSWorld;
    PowerPlaceToNavMeshHandle(tNavPlace.tPlace, tNavPlace.tNavMeshHandle);
    Quaternion qParentRot = ptACD->GetOrientation();
    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    // 
    PowerAPIDestroyProjectile tProjectile;
    PowerHelper_StopProjectileInit(idPower, pImpactData->idACDProjectile, pImpactData->idACDIntersected, tProjectile);
    PowerAPI_DestroyProjectile(tProjectile);
    ptPower->dwParam = ACDID_INVALID;
    // 
    // 
    // 
    ACDID idACDCreeper = SpellEffect_SpawnActor(idPower, GlobalSNOGet(GLOBALSNO_ACTOR_CREEPMOB_ARM), tNavPlace, qParentRot, dwAddFlags, FALSE);
    // 
    ActorCommonData * ptACDCreeper = ACDTryToGet(idACDCreeper);
    if (ptACDCreeper)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.pRemovalEffect = sSpellEffect_CreepMob_Kill_Creeper;
        tBuffStartData.pRemovalEffectData = (void *)(intptr)idACDCreeper;
        tBuffStartData.pPeriodicSpellEffect = sSpellEffect_CreepMob_Plunge_Pulse;
        tBuffStartData.pPeriodicSpellEffectData = (void *)(intptr)idACDCreeper;
        tBuffStartData.dwRefreshRate = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_POISON_CLOUD_INTERVAL_DURATION) * 60.0f);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
    else
    {
        PowerAbortChannelled(ptACD->id, ptPower->snoPower);
    }
}






static void sSpellEffect_Brickhouse_ArmShields_OnTakeDamage(PowerAPIBuffInstance * ptBuff, ACDID, float * flDamageDone, PowerAPIDamage &, void *)
{
    if (flDamageDone)
    {
        float flToAbsorb = 1.0f - PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SHIELD_DAMAGE_ABSORB_PERCENT);
        for (int32 iDamageType = 0; iDamageType < NUM_DAMAGE_TYPES; iDamageType++)
        {
            if (flDamageDone[iDamageType] > 1e-06f)
            {
                flDamageDone[iDamageType] *= flToAbsorb;
            }
        }
    }
}










static void sSpellEffect_Brickhouse_Outro_To_Enrage(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, void *)
{
    PowerForceStop(ptBuff->idPower, TRUE);
    Target tSelfTarget;
    tSelfTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
    SNO snoBrickhouseEnragePower = GlobalSNOGet(GLOBALSNO_POWERS_BRICKHOUSE_ENRAGE);
    PowerStart(idACDTarget, snoBrickhouseEnragePower, tSelfTarget);
}








static void sSpellEffect_Brickhouse_ArmShields(PowerID idPower, void *)
{
    float flDurationMin = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN);
    float flDurationDelta = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_DELTA);
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.dwDurationMin = Math::FloatToInt((flDurationMin + flDurationDelta) * 60.0f);
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.pDamageShieldEffect = sSpellEffect_Brickhouse_ArmShields_OnTakeDamage;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.pExpirationEffect = sSpellEffect_Brickhouse_Outro_To_Enrage;
    PowerAPI_BuffAddToACD(tBuffStartData, *PowerGetOwner(idPower));
}





static void sSpellEffect_Brickhouse_Enrage_Cleanup(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    // 
    // 
    // 
    // 
    AIStateSetState(ACDTryToGet(ptBuff->idACDAffected) ? (ActorID)ACDTryToGet(ptBuff->idACDAffected)->idOwner : ACTORID_INVALID, 2, FALSE);
}


static void sSpellEffect_Brickhouse_Enrage(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    // 
    float flDurationMin = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN);
    float flDurationDelta = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_DELTA);
    float flDamagePercentAll = PowerGetFormulaValue(idPower, TAG_POWER_DAMAGE_PERCENT_ALL);
    float flMovementSpeedBase = PowerGetFormulaValue(idPower, TAG_POWER_MOVEMENT_SPEED_PERCENT_INCREASE_MIN);
    float flAttackSpeedBase = PowerGetFormulaValue(idPower, TAG_POWER_ATTACK_SPEED_PERCENT_INCREASE_MIN);
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.dwDurationMin = Math::FloatToInt((flDurationMin + flDurationDelta) * 60.0f);
    tBuffStartData.eStackType = 0;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.pRemovalEffect = sSpellEffect_Brickhouse_Enrage_Cleanup;
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, flDamagePercentAll);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_UNCAPPED_BONUS, flMovementSpeedBase);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT, flAttackSpeedBase);
    tBuffStartData.snoEndingEffect = PowerGetEffect(idPower, TAG_POWER_ENDING_EFFECT_GROUP);
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, *PowerGetOwner(idPower));
    // 
    // 
    AIStateSetState((ActorID)ptACD->idOwner, 4, FALSE);
}


static void sEvaluateBrickhouseDestroyedDoorAchievementEvent(
    const ActorCommonData & tBrickhouse)
{





    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        if (ExperiencePlayersCloseEnough(tBrickhouse, ptPlayer->idPlayerACD))
        {



            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);
            tEvaluationData.SetACD(&tBrickhouse);

            tEvaluationData.EvaluateEvent(
                ptPlayer,
                Achievement::ACH_EVENT_BRICKHOUSE_DESTROYED_DOOR,
                tBrickhouse.snoActor,
                1, (uint32)sub_71007b5e70());
        }
    }
}


static void sBrickHouse_Destruction_Update(
    PowerAPIBuffInstance * ptBuff,
    void *)
{
    ActorCommonData * ptACDOwner = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptACDOwner)
        return;




    ACDID idACDTarget = BrainAPIChoosePrimaryTarget(*ptACDOwner, FALSE);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
        return;



    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
        return;



    WorldPlace wpPrevious = ptPower->wpStartLocation;
    WorldPlace wpCurrent = ptACDOwner->GetPlace();

    float flDistChangeSq = Vector3D::Distance2DSquared(wpCurrent.wp, wpPrevious.wp);
    if (flDistChangeSq < 4.0f || wpCurrent.idSWorld != wpPrevious.idSWorld)
        return;



    ptPower->wpStartLocation = wpCurrent;


    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = ptBuff->idPower;
    tChooseTargets.ePassabilityType = PASSABILITY_MONSTER;
    tChooseTargets.dwFlags = 0;

    Target tTarget;
    WorldPlace tTargetOffset = wpCurrent;
    tTargetOffset.wp += ptACDOwner->GetForward() * 5.0f;
    tTarget.InitializeFromPlace(tTargetOffset);

    PowerHelper_ChooseTargetsInitGeometryPie(
        ptACDOwner->GetPlace(), tTarget, 8.0f, 30.0f, 170, tChooseTargets.tGeometry);






    tChooseTargets.tGeometry.tPie.wvPieVector = ptACDOwner->GetForward();


    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(ptBuff->idPower, tPayload);
    tPayload.flBaseDamageScalar = 2.0f;
    tPayload.arCombatDamage[DAMAGE_TYPE_PHYSICAL].flMin = 100000.0f;
    tPayload.eDeliveryMechanism = PDM_RANGED;

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listTargets.GetNext(pos);

        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (!ptACD || !ptACD->IsWreckableObject() || !ptACD->IsAlive())
            continue;



        ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);


        {

            float fDistSq = Vector3D::Distance2DSquared(ptACDOwner->GetPlace().wp, ptACD->GetPlace().wp);
            if (fDistSq > 49.0f)
                continue;



            PowerAPI_DoPayload(tPayload, idACD, ACDID_INVALID);
            PowerAPIExplosion tExplosion;
            tExplosion.dwDuration = 60;
            tExplosion.placeSource = wpCurrent;
            tExplosion.qOrientation = ptACDOwner->GetOrientation();
            tExplosion.snoExplosionActor = GlobalSNOGet(GLOBALSNO_POWERS_BRICKHOUSE_EXPLOSION);
            PowerAPI_DoExplosion(tExplosion);

            sEvaluateBrickhouseDestroyedDoorAchievementEvent(*ptACDOwner);
        }
    }
}



static void sSpellEffect_Brickhouse_Destruction_Setup(
    PowerID idPower,
    void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
        return;








    ptPower->tTargetCriteria.dwTeamRelationshipFlags = (1 << TCTR_AFFECTS_ENEMIES) | (1 << TCTR_IGNORES_PETS) | (1 << TCTR_IGNORES_PLAYERS) | (1 << TCTR_IGNORES_CASTER);

    PowerAPIBuffStartData tBuff(idPower);
    tBuff.pPeriodicSpellEffect = sBrickHouse_Destruction_Update;
    tBuff.dwDurationMin = tBuff.dwDurationDelta = 0;

    tBuff.bInfinite = true;
    tBuff.nBuffIndex = 0;
    tBuff.bUndispellable = true;
    tBuff.dwRefreshRate = 6;

    PowerAPI_BuffAddToACD(tBuff, *ptACD);
}


void sSpawnExpelledMonster(PowerID idPower, void * pData)
{
    int32 nCountLeft = (int32)(intptr)pData - 1;
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (nCountLeft < 0 || !ptACD)
    {
        return;
    }
    {
        NavPlace tNavPlace;
        ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
        int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
        SNO snoMonsterActor = PowerContactTagGetSNO(idPower, TAG_POWER_EXPEL_MONSTER_SNO);
        if (snoMonsterActor != SNO_NONE)
        {
            Angle angYawFacing = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, FULL_CIRCLE);
            ACDID idACDSpawnedMonster = sCorpulent_SpawnMonster(idPower, snoMonsterActor, nLevel, tNavPlace, angYawFacing);
            if (idACDSpawnedMonster != ACDID_INVALID)
            {
                // 
                ActorID idSpawnedActor = (ActorID)ACDGetOwner(idACDSpawnedMonster);
                AIStateSetState(idSpawnedActor, 4, FALSE);
                // 
                PowerTriggerAddTimerTrigger(idPower, 0, sSpawnExpelledMonster, NULL);
            }
        }
        // 
        // 
        // 
        // 
        ptACD->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nCountLeft);
        float flIntervalInSecs = PowerGetFormulaValue(idPower, TAG_POWER_EXPEL_MONSTER_SPAWN_INTERVAL);
        int32 nIntervalInTicks = Math::FloatRoundToInt(flIntervalInSecs * 60.0f);
        // 
        PowerTriggerAddTimerTrigger(idPower, nIntervalInTicks, sSpawnExpelledMonster, (void *)(intptr)nCountLeft);
    }
}



void sSpellEffect_ExpelMonster(PowerID idPower, void *)
{
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        // 
        int32 nNumToSpawn = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_EXPEL_MONSTER_COUNT));
        PowerTriggerAddTimerTrigger(idPower, 0, sSpawnExpelledMonster, (void *)(intptr)nNumToSpawn);
    }
}


void sSpellEffect_MovementSlow(PowerID idPower, void *)
{
    // 
    // 
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        // 
        // 
        PowerAPIBuffStartData tBuffStartDataTarget(idPower);
        float flDurationMin = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN);
        float flDurationDelta = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_DELTA);
        tBuffStartDataTarget.dwDurationMin = Math::FloatToInt((flDurationMin + flDurationDelta) * 60.0f);
        tBuffStartDataTarget.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartDataTarget.bPositiveEffect = FALSE;
        // 
        // 
        float flSlowMult = PowerGetFormulaValue(idPower, TAG_POWER_SLOW_MULTIPLIER);
        FastAttribSetValueFloat(tBuffStartDataTarget.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flSlowMult);
        PowerAPI_BuffAddToACD(tBuffStartDataTarget, ACDGet(idACDTarget));
    }
}

const uint32 TARPIT_PERIODIC_TICK_RATE = 10;

void sSpellEffect_TarpitSlowPeriodicEFfect(PowerAPIBuffInstance * ptBuff, void *)
{
    if (!ptBuff)
    {
        return;
    }
    ActorCommonData * ptACDAffected = ACDTryToGet(ptBuff->idACDAffected);
    ActorID idActorAffected = ptACDAffected ? (ActorID)ptACDAffected->idOwner : ACTORID_INVALID;
    Actor * ptActorAffected = ActorTryToGet(idActorAffected);
    if (ptActorAffected && (ptActorAffected->uCurrNavMeshFlags & 0x100))
    {
        PowerAPIBuffStartData tBuffStartDataTarget(ptBuff->idPower);
        tBuffStartDataTarget.bInfinite = FALSE;
        tBuffStartDataTarget.dwDurationMin = 20;
        tBuffStartDataTarget.dwDurationDelta = 0;
        tBuffStartDataTarget.eStackType = BUFF_STACKTYPE_REFRESH;
        tBuffStartDataTarget.nBuffIndex = 0;
        tBuffStartDataTarget.bPositiveEffect = FALSE;
        tBuffStartDataTarget.pPeriodicSpellEffect = sSpellEffect_TarpitSlowPeriodicEFfect;
        tBuffStartDataTarget.dwRefreshRate = TARPIT_PERIODIC_TICK_RATE;
        float flSlowMult = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SLOW_MULTIPLIER);
        FastAttribSetValueFloat(tBuffStartDataTarget.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flSlowMult);
        PowerAPI_BuffAddToACD(tBuffStartDataTarget, *ptACDAffected);
    }
}








void sSpellEffect_TarpitSlowOn(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD && ACDIsPlayer(ptACD->id))
    {
        PowerAPIBuffStartData tBuffStartDataTarget(idPower);
        tBuffStartDataTarget.bInfinite = TRUE;
        tBuffStartDataTarget.nBuffIndex = 1;
        tBuffStartDataTarget.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartDataTarget.bHitsOnFirstTick = TRUE;
        tBuffStartDataTarget.bPositiveEffect = TRUE;
        tBuffStartDataTarget.pPeriodicSpellEffect = sSpellEffect_TarpitSlowPeriodicEFfect;
        tBuffStartDataTarget.dwRefreshRate = TARPIT_PERIODIC_TICK_RATE;
        PowerAPI_BuffAddToACD(tBuffStartDataTarget, *ptACD);
    }
}




void sSpellEffect_TarpitSlowOff(PowerID idPower, void *)
{
    SNO snoPowerTarpitOn = GlobalSNOGet(GLOBALSNO_POWERS_TARPIT_SLOW_ON);
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD && ACDIsPlayer(ptACD->id))
    {
        POS pos = ptACD->listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = ptACD->listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff->snoPower == snoPowerTarpitOn)
            {
                PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
    }
}





void sSpellEffect_InvulnerableAnimBegin(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.nMaxStackCount = 1;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNINTERRUPTIBLE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANNOT_DIE_DURING, TRUE);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS, 1.0f);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}




void sSpellEffect_InvulnerableAnimEnd(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        SNO snoPower = PowerGetSNO(idPower);
        BuffID idStartBuff = PowerAPI_BuffFindOnACD(snoPower, *ptACD, ACDID_INVALID);
        if (idStartBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idStartBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }
}
// X/Common/WorldCommonCollision.h
BOOL WorldCollSearchForLocation(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flLocationRadius, float flSearchRadius, float flSearchRadiusMax, ACDID idACDIgnore, Vector3D & wpEnd, NavCellHandle & hNavCellEnd, Team eTeam, uint16 uObjectNoPassthroughFlags, ACDID idOwner);  // WorldCommonCollision.cpp:241


void sSpellEffect_EelSpawnBegin(PowerID idPower, void *)
{



    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (ptMove)
    {
        WorldPlace placeCurr = ptACD->GetPlace();
        const float OFFSET_ANGLE_INC = EIGHTH_CIRCLE;
        const float SEARCH_RADIUS = 5.0f;
        const float TOTAL_SEARCH_ANGLE = FULL_CIRCLE;
        float flStartAngle = RandFloat_Internal(SGameGlobalsGet().rng) * FULL_CIRCLE;
        ScopedActorDefinitionLockR ptDefActor(ptACD->snoActor);
        for (float flAngle = 0.0f; flAngle <= TOTAL_SEARCH_ANGLE; flAngle += OFFSET_ANGLE_INC)
        {
            float flCheckAngle = Math::NormalizeAngle(flStartAngle + flAngle);
            WorldPlace placeCheck(placeCurr);
            placeCheck.wp.x += Math::Cos(flCheckAngle) * SEARCH_RADIUS;
            placeCheck.wp.y += Math::Sin(flCheckAngle) * SEARCH_RADIUS;

            float flRadius;
            if (ptDefActor->tCollData.eCollisionShape == COLLISION_SHAPE_CYLINDER)
            {
                flRadius = ActorGetCollisionCylinderRadius(ptDefActor);
            }
            else
            {
                flRadius = ptDefActor->wcylBounds.wdRadius;
            }

            Vector3D vFoundLoc(placeCheck.wp);
            NavCellHandle hNavCellTarget;
            BOOL bFoundDest = WorldCollSearchForLocation(placeCheck,

                ptDefActor->tCollData.tCollFlags.uStaticOneRequiredFlags, 0x60, flRadius, 0.0f,



                Math::Max(flRadius + 1.0f, SEARCH_RADIUS), ACDID_INVALID, vFoundLoc, hNavCellTarget, TEAM_NEUTRAL, 0, ACDID_INVALID);




            if (bFoundDest)
            {
                placeCurr.wp = vFoundLoc;
                ptACD->SetFacing(placeCurr, TRUE);
                break;
            }
        }

        ptMove->SnapToNewPlace(placeCurr, TRUE, FALSE, TRUE, 0);
    }

    sSpellEffect_InvulnerableAnimBegin(idPower, NULL);
}



static void sLunaticPoolSpawnLunatic(
    PowerID idPower,
    void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;



    ACDID idACDLunatic = (ACDID)ptPower->dwParam;
    ActorCommonData * ptACDLunatic = ACDTryToGet(idACDLunatic);
    if (ptACDLunatic)
    {
        ActorID idActorLunatic = (ActorID)ACDGetOwner(idACDLunatic);
        ActorFlagSet(idActorLunatic, ACTOR_DOESNT_DEACTIVATE_BIT, FALSE);

        SNO snoPowerLeap = PowerGetChildPower(idPower);
        if (snoPowerLeap != SNO_NONE)
        {
            ActorMovement * ptMove = ptACDLunatic->GetActorMovement();
            if (ptMove)
            {
                WorldPlace placeLunatic;
                float flRadius;
                ptMove->GetPlaceAndRadius(placeLunatic, flRadius, FALSE);

                BOOL bFoundDest = FALSE;
                ACDID idACDClosestEnemy = BrainAPIActorGetClosestTarget(idActorLunatic, SNO_NONE, FALSE, NULL);
                ActorCommonData * ptACDEnemy = ACDTryToGet(idACDClosestEnemy);
                WorldPlace placeTry;
                if (ptACDEnemy)
                {
                    placeTry = ptACDEnemy->GetPlace();
                    bFoundDest = TRUE;
                }
                else
                {
                    ACDID idACDTarget = (ACDID)ptPower->listACDsTargetted.GetHead();
                    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
                    if (ptACDTarget)
                    {
                        ActorID idActorTarget = (ActorID)ACDGetOwner(idACDTarget);
                        XPooledDataIDList listLinkedACDs(MemoryPoolsGetDATAIDListPool());
                        MarkerGetACDsLinkedByActor(idActorTarget, listLinkedACDs, NULL);
                        if (listLinkedACDs.Count() > 0)
                        {
                            ACDID idACDLink = (ACDID)listLinkedACDs.GetHead();
                            ActorCommonData * ptACDLink = ACDTryToGet(idACDLink);
                            if (ptACDLink)
                            {
                                placeTry = ptACDLink->GetPlace();
                                bFoundDest = TRUE;
                            }
                        }
                    }
                }

                if (bFoundDest)
                {
                    ACDCollSettings tCollSettingsDuring;
                    ACDCollSettings tCollSettingsDest;
                    ptMove->GetCollSettings(snoPowerLeap, POWER_PASSABILITY_DURING, tCollSettingsDuring);
                    ptMove->GetCollSettings(snoPowerLeap, POWER_PASSABILITY_DESTINATION, tCollSettingsDest);

                    Vector3D vFoundLoc(placeTry.wp);
                    NavCellHandle hNavCellTarget;
                    bFoundDest = WorldCollSearchForLocation(placeTry,
                        tCollSettingsDest.uStaticOneRequiredFlags,

                        tCollSettingsDest.uDynamicFlagsNoOverlap,
                        ptACDLunatic->GetRadius(), 0.0f, 20.0f, idACDLunatic,
                        vFoundLoc, hNavCellTarget, TEAM_NEUTRAL, 0, ACDID_INVALID);





                    if (bFoundDest)
                    {

                        WorldCollParams tWorldCollParamsDest(tCollSettingsDest);
                        Circle circleStart;
                        circleStart.Set(vFoundLoc.x, vFoundLoc.y, flRadius);
                        WorldCollResult tResultSweepBack;
                        WorldCollSweptCircle(placeTry.idSWorld, hNavCellTarget, circleStart,
                            (const Vector2D &)placeLunatic.wp, tWorldCollParamsDest, 3, 1, idACDLunatic, tResultSweepBack);








                        ptACDLunatic->SetEnabled(TRUE);
                        Target tTargetLeap;
                        WorldPlace placeResult;
                        placeResult.idSWorld = placeTry.idSWorld;
                        placeResult.wp = tResultSweepBack.wpEnd;

                        ptMove->SnapPlaceToGround(placeResult);

                        tTargetLeap.InitializeFromPlace(placeResult);
                        ActorID idActorLunaticScoped = (ActorID)ACDGetOwner(idACDLunatic);
                        BrainStanceDoPower(idActorLunaticScoped, tTargetLeap, snoPowerLeap, BRAIN_POWER_START, NULL, NULL, 0, 0);
                    }
                }


                if (!bFoundDest)
                {
                    ActorFlagForDeletion(idActorLunatic);
                }
            }
        }

        ptPower->dwParam = ACDID_INVALID;
    }
}



static void sSpellEffect_LunaticDiveInStart(const PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    ptPower->dwParam = ACDID_INVALID;
}





static void sSpellEffect_LunaticDiveInInterrupted(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ACDID idACDLunatic = (ACDID)ptPower->dwParam;
        ActorCommonData * ptACDLunatic = ACDTryToGet(idACDLunatic);
        if (ptACDLunatic)
        {
            ActorFlagForDeletion((ActorID)ACDGetOwner(idACDLunatic));
        }
    }
}






static void sSpellEffect_LunaticDiveInLand(
    const PowerID idPower,
    void *)
{
    Power * ptPower = PowerGet(idPower);

    ACDID idACDTarget = (ACDID)ptPower->listACDsTargetted.GetHead();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget)
    {
        SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
        if (snoEffect != SNO_NONE)
        {
            ACDPlayEffect(idACDTarget, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
        }
    }

    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        SNO snoActor = PowerContactTagGetSNO(idPower, TAG_POWER_EXPEL_MONSTER_SNO);
        if (snoActor != SNO_NONE && ptACDTarget)
        {

            WorldPlace placeSpawn = ptACDTarget->GetPlace();
            Angle aYaw = ACDGetYaw(idACDTarget);
            Quaternion qOrientation;
            qOrientation.FromYaw(aYaw);

            PowerAPISummonActor tSummonInfo;
            SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
            tSummonInfo.idPower = idPower;
            tSummonInfo.snoActor = snoActor;
            tSummonInfo.nActorLevel = -1;
            tSummonInfo.idACDCorpse = ACDID_INVALID;
            tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_REPORT_SPAWN_ERROR) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);



            tSummonInfo.placeSummonLocation = placeSpawn;
            tSummonInfo.qOrientation = qOrientation;
            tSummonInfo.eTeamOverride = TEAM_INVALID;

            ACDID idACDLunatic = PowerAPI_SummonActor(tSummonInfo);
            ActorCommonData * ptACDLunatic = ACDTryToGet(idACDLunatic);
            if (ptACDLunatic)
            {
                ptACDLunatic->SetEnabled(FALSE);

                ActorID idActorLunatic = (ActorID)ACDGetOwner(idACDLunatic);
                ActorFlagSet(idActorLunatic, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
                PowerTriggerAddTimerTrigger(idPower, 180, sLunaticPoolSpawnLunatic, NULL);
            }

            ptPower->dwParam = idACDLunatic;
        }

        ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
        ActorFlagForDeletion(idActor);
    }
}


static void sDrainResource(
    PowerID idPower,
    ActorCommonData & tACD,
    int32 nPowerTag,
    ResourceType eResource)
{
    if (eResource == RESOURCE_TYPE_INVALID)
        return;
    float flCurrValue = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eResource);
    float flMaxValue = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResource);
    if (flCurrValue > 1e-6f)
    {
        float flDrainPercent = PowerGetFormulaValue(idPower, nPowerTag);
        float flDrainAmount = flMaxValue * flDrainPercent;
        flCurrValue = flCurrValue - flDrainAmount;
        flCurrValue = Math::Pin(flCurrValue, 0.0f, flMaxValue);
        tACD.ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flCurrValue, eResource);
    }
}


void ActorPowersForceStopPrimary(ActorID idActor, BOOL bIgnoreContactFrame);  // ActorPowers.cpp:375
static void sMastaBlastaRiderCombineSwap(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDRider = PowerGetOwner(idPower);
    if (ptACDRider && ptACDRider->IsAlive())
    {
        ACDID idACDSteed = (ACDID)ptPower->listACDsTargetted.GetHead();
        ActorCommonData * ptACDSteed = ACDTryToGet(idACDSteed);
        if (ptACDSteed && ptACDSteed->IsAlive())
        {
            PowerAPISummonActor tSummonInfo;
            SNO snoMonsterActor = MonsterGetSummonActor(ptACDRider->tSNOName, 0);
            BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, 0);
            if (!bSuccess)
            {
                return;
            }

            tSummonInfo.nActorLevel = ptACDSteed->AttributesGetInt(ATTRIBUTE_LEVEL);
            tSummonInfo.qOrientation = ptACDSteed->GetOrientation();
            tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
            tSummonInfo.placeSummonLocation = ptACDSteed->GetPlace();

            ACDID idACDCombined = PowerAPI_SummonActor(tSummonInfo);
            ActorCommonData * ptACDCombined = ACDTryToGet(idACDCombined);
            if (ptACDCombined)
            {
                float flHPPercent = ptACDSteed->GetHitpointPercent();
                float flHPMax = ptACDCombined->GetHitPointsMaxTotal();
                ptACDCombined->SetHitPoints(flHPPercent * flHPMax, TRUE, FALSE);
                BrainAPIDoPlayAnimation((ActorID)ACDGetOwner(idACDCombined), TAG_ACTOR_ANIMATION_EMOTE_TAUNT, (uint32)-1, FALSE, FALSE, 0.5f, FALSE, 1.0f);
                ActorFlagForDeletion((ActorID)ACDGetOwner(idACDSteed));
                ActorFlagForDeletion((ActorID)ACDGetOwner(ptACDRider->id));
            }
        }
    }
}


void sSpellEffect_MastaBlastaRiderCombine(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACDRider = PowerGetOwner(idPower);
        if (ptACDRider)
        {
            ActorMovement * ptMove = ptACDRider->GetActorMovement();
            if (ptMove && ptPower->listACDsTargetted.Count() > 0)
            {
                ACDID idACDSteed = (ACDID)ptPower->listACDsTargetted.GetHead();
                ActorCommonData * ptACDSteed = ACDTryToGet(idACDSteed);
                if (ptACDSteed && ptACDSteed->IsAlive())
                {
                    SNO snoPowerSteedCombine = GlobalSNOGet(GLOBALSNO_POWERS_MASTABLASTA_STEED_COMBINE);
                    if (snoPowerSteedCombine != SNO_NONE)
                    {
                        ActorID idActorSteed = (ActorID)ACDGetOwner(idACDSteed);
                        ActorPowersForceStopPrimary(idActorSteed, TRUE);
                        Target tTargetRider;
                        tTargetRider.InitializeFromACD(ptACDRider->id, 0, 0);
                        PowerStart(idACDSteed, snoPowerSteedCombine, tTargetRider);





                        float flHeight = PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_TRAJECTORY_HEIGHT);
                        float flGravity = PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_TRAJECTORY_GRAVITY);
                        uint32 uMoveFlags = 0x16;
                        Vector3D vInitialVel;
                        uint32 uTicks = 60;

                        float flTicks = (float)uTicks;
                        float flOOTicks = 1.0f / flTicks;
                        WorldPlace placeRider = ptACDRider->GetPlace();
                        WorldPlace placeSteed = ptACDSteed->GetPlace();


                        Angle aYaw = ACDGetYaw(ptACDSteed->id);
                        Vector3D vSteedDir(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
                        vSteedDir *= 5.0f;
                        placeSteed.wp += vSteedDir;

                        float flHeightDiff = placeSteed.wp.z - placeRider.wp.z;
                        flHeightDiff = flHeight + flHeightDiff;

                        float flVel = flHeightDiff - flGravity * flTicks * flTicks;
                        flVel *= flOOTicks;

                        vInitialVel = (placeSteed.wp - placeRider.wp) * flOOTicks;
                        vInitialVel.z = flVel;

                        ptACDRider->SetFacing(placeSteed, TRUE);
                        ptMove->MoveWithArc(ptPower->dwAnimationTag, (ActorAnimationTag)-1, uMoveFlags, flGravity, vInitialVel, ptPower->snoPower, 0.0f, NULL);


                        PowerTriggerAddTimerTrigger(idPower, 42, sMastaBlastaRiderCombineSwap, NULL);
                    }
                }
            }
        }
    }
}


void sSpellEffect_MastaBlastaSteedDrain(PowerID idPower, void *)
{


    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);


    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_MELEE;
    tPayload.eAttackDirection = ATTACKDIRECTION_STRAIGHT;


    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && ACDIsPlayer(idACDTarget))
        {


            PlayerClass eClass = PlayerGetPlayerClass(idACDTarget);
            int32 nTagResource0 = -1;
            ResourceType eResource0 = RESOURCE_TYPE_INVALID;
            int32 nTagResource1 = -1;
            ResourceType eResource1 = RESOURCE_TYPE_INVALID;
            switch (eClass)
            {
            case PLAYER_CLASS_DEMONHUNTER:
                nTagResource0 = TAG_POWER_MASTABLASTA_DRAIN_HATRED_PERCENT;
                eResource0 = RESOURCE_TYPE_HATRED;
                nTagResource1 = TAG_POWER_MASTABLASTA_DRAIN_DISCIPLINE_PERCENT;
                eResource1 = RESOURCE_TYPE_DISCIPLINE;
                break;
            case PLAYER_CLASS_BARBARIAN:
                nTagResource0 = TAG_POWER_MASTABLASTA_DRAIN_FURY_PERCENT;
                eResource0 = RESOURCE_TYPE_FURY;
                break;
            case PLAYER_CLASS_WIZARD:
                nTagResource0 = TAG_POWER_MASTABLASTA_DRAIN_ARCANUM_PERCENT;
                eResource0 = RESOURCE_TYPE_ARCANUM;
                break;
            case PLAYER_CLASS_WITCHDOCTOR:
                nTagResource0 = TAG_POWER_MASTABLASTA_DRAIN_MANA_PERCENT;
                eResource0 = RESOURCE_TYPE_MANA;
                break;
            case PLAYER_CLASS_MONK:
                nTagResource0 = TAG_POWER_MASTABLASTA_DRAIN_SPIRIT_PERCENT;
                eResource0 = RESOURCE_TYPE_SPIRIT;
                break;
            case PLAYER_CLASS_CRUSADER:
                nTagResource0 = TAG_POWER_MASTABLASTA_DRAIN_FURY_PERCENT;
                eResource0 = RESOURCE_TYPE_FAITH;
                break;
            case PLAYER_CLASS_NECROMANCER:
                nTagResource0 = TAG_POWER_MASTABLASTA_DRAIN_FURY_PERCENT;
                eResource0 = RESOURCE_TYPE_ESSENCE;
                break;
            }



            sDrainResource(idPower, *ptACDTarget, nTagResource0, eResource0);
            sDrainResource(idPower, *ptACDTarget, nTagResource1, eResource1);
        }
    }
}


void sSpellEffect_MastaBlastaDismountRider(PowerID idPower, void *)
{


    ActorCommonData * ptACDCombined = PowerGetOwner(idPower);
    if (ptACDCombined)
    {
        PowerAPISummonActor tSummonInfo;
        SNO snoMonsterActor = MonsterGetSummonActor(ptACDCombined->tSNOName, 0);
        BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, 0);
        if (!bSuccess)
        {
            return;
        }

        tSummonInfo.nActorLevel = ptACDCombined->AttributesGetInt(ATTRIBUTE_LEVEL);
        tSummonInfo.qOrientation = ptACDCombined->GetOrientation();
        tSummonInfo.dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
        tSummonInfo.placeSummonLocation = ptACDCombined->GetPlace();

        ACDID idACDRider = PowerAPI_SummonActor(tSummonInfo);
        ActorCommonData * ptACDRider = ACDTryToGet(idACDRider);
        if (ptACDRider)
        {


            ActorID idActorRider = (ActorID)ACDGetOwner(idACDRider);
            AIStateSetState(idActorRider, 4, FALSE);
        }
    }
}



void sSpellEffect_DemonTrooperLeapOutStart(PowerID idPower, void *)
{


    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD)
    {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
        {
            WorldPlace placeCurr;
            float flRadius;
            ptMove->GetPlaceAndRadius(placeCurr, flRadius, FALSE);
            Angle aYaw = ACDGetYaw(ptACD->id);
            WorldPlace placeCheck(placeCurr);
            Vector3D vDir(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
            float flLeapDistMin = PowerGetFormulaValue(idPower, TAG_POWER_LEAP_DIST_MIN);
            float flLeapDistMax = PowerGetFormulaValue(idPower, TAG_POWER_LEAP_DIST_MAX);
            float flLeapDistDelta = flLeapDistMax - flLeapDistMin;
            float flDist = RandRangeFloat_Internal(SGameGlobalsGet().rng, flLeapDistMin, flLeapDistDelta);
            vDir *= flDist;
            placeCheck.wp += vDir;

            Vector3D vFoundLoc(placeCheck.wp);
            NavCellHandle hNavCellTarget;
            BOOL bFoundDest = WorldCollSearchForLocation(placeCheck,
                1, 0x60,


                flRadius, 0.0f,

                Math::Max(flRadius + 1.0f, 15.0f), ACDID_INVALID, vFoundLoc, hNavCellTarget, TEAM_NEUTRAL, 0, ACDID_INVALID);




            if (bFoundDest)
            {
                placeCheck.wp = vFoundLoc;
                ptACD->SetFacing(placeCheck, TRUE);
                ptPower->tTarget.InitializeFromPlace(placeCheck);

                float flHeight = PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_TRAJECTORY_HEIGHT);
                float flGravity = PowerGetFormulaValue(idPower, TAG_POWER_SPECIAL_WALK_TRAJECTORY_GRAVITY);
                uint32 uMoveFlags = (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING) | (1 << MOVEBIT_AUTO_UPDATE_YAW);
                ptMove->MoveWithArc(ptPower->tTarget.tPlace, ptPower->dwAnimationTag, (ActorAnimationTag)-1, uMoveFlags, flGravity, flHeight, ptPower->snoPower, 0.0f);
            }

            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.bInfinite = TRUE;
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IN_KNOCKBACK, TRUE);
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
        }
    }
}


const int32 WALL_MONSTER_SPIT_INTERVAL_TICKS = 9;
void sWallMonsterSpitMonster(PowerID idPower, void * pData)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
        return;
    int32 nNumToSpawn = (int32)(intptr)pData;
    {
        NavPlace tNavPlace;
        PowerGetACDPoint(idPower, ptPower->idACD, tNavPlace, -1, -1, NULL);

        int32 nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
        Angle aYawBase = ACDGetYaw(ptACD->id);
        SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
        if (snoMonsterActor != SNO_NONE)
        {
            Angle angYawFacing = aYawBase +
                RandRangeFloat_Internal(SGameGlobalsGet().rng, -EIGHTH_CIRCLE, QUARTER_CIRCLE);
            ACDID idACDSpawnedMonster = sCorpulent_SpawnMonster(idPower, snoMonsterActor, nLevel, tNavPlace, angYawFacing);
            if (idACDSpawnedMonster != ACDID_INVALID)
            {


                ActorID idSpawnedActor = (ActorID)ACDGetOwner(idACDSpawnedMonster);
                AIStateSetState(idSpawnedActor, 4, FALSE);


                ActorCommonData & tACDSpawnedMonster = ACDGet(idACDSpawnedMonster);
                tACDSpawnedMonster.InheritGroupsFromACD(*ptACD);
            }


            if (nNumToSpawn > 1)
            {
                PowerTriggerAddTimerTrigger(idPower, WALL_MONSTER_SPIT_INTERVAL_TICKS, sWallMonsterSpitMonster, (void *)(intptr)(nNumToSpawn - 1));
            }
        }
    }
}


void sSpellEffect_WallMonsterSummon(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        // 
        // 
        return;
    }
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        // 
        NavPlace tNavPlace;
        PowerGetACDPoint(idPower, ptPower->idACD, tNavPlace, -1, -1, NULL);
        // 
        SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
        if (snoMonsterActor != SNO_NONE)
        {
            // 
            // 
            WorldPlace placeCenter;
            float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(ptACD->id, placeCenter);
            int32 nMaxConcurrent = PowerAPI_GetSummonMaxConcurrent(*ptACD, ptPower->snoPower);
            int32 nExistingUnits = ACDCountNearbySummonedUnits(*ptACD, flTargetScanRadius);
            int32 nNumDesired = nMaxConcurrent - nExistingUnits;
            if (nNumDesired > 0)
            {
                // 
                int32 nCurrSummonCount = ptACD->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
                int32 nMaxSummonCount = PowerAPI_GetSummonMaxTotal(*ptACD, ptPower->snoPower);
                int32 nNumLeft = nMaxSummonCount - nCurrSummonCount;
                int32 nNumToSummonAtOnce = PowerAPI_GetSummonCountPer(*ptACD, ptPower->snoPower);
                // 
                // 
                // 
                nNumDesired = Math::Min(Math::Min(nNumDesired, nNumLeft), nNumToSummonAtOnce);
                ptACD->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nCurrSummonCount + nNumDesired);
                // 
                sWallMonsterSpitMonster(idPower, (void *)(intptr)nNumDesired);
            }
        }
    }
}


static void sBloodStarTrackPlayer(PowerAPIBuffInstance * ptBuff, void *)
{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ActorCommonData * ptACDTarget = ACDTryToGet(PowerGetTargetACD(ptPower));
    if (!ptACDTarget)
    {
        // 
        // 
        PowerAPIDestroyProjectile tProjectile;
        PowerHelper_StopProjectileInit(ptBuff->idPower, ptBuff->idACDAffected, ACDID_INVALID, tProjectile);
        PowerAPI_DestroyProjectile(tProjectile);
        return;
    }
    // 
    // 
    // 
    // 
    int32 nUpdateRate = (int32)PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_3);
    ProjectileSeekActor(ptBuff->idACDAffected, ptACDTarget->id, (float)nUpdateRate, FALSE);
}


void sSuccubusOnStunEnd(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, void * pData)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
    // 
    float fAdditionalStunTime = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_3);
    if (ptACD && fAdditionalStunTime > FLT_EPSILON)
    {
        // 
        PowerAPIBuffStartData tExtendedStun(ptBuff->idPower);
        tExtendedStun.dwDurationMin = (uint32)(fAdditionalStunTime * 60.0f);
        tExtendedStun.bStun = TRUE;
        tExtendedStun.bPositiveEffect = FALSE;
        tExtendedStun.nBuffIndex = 0;
        // 
        PowerAPI_BuffAddToACD(tExtendedStun, *ptACD);
    }
    // 
    // 
    SComplexEffectID idRope = (SComplexEffectID)(intptr)pData;
    // 
    SComplexEffectRemove(idACDTarget, idRope);
}


void sSpellEffect_Succubus_StunNPC(PowerID idPower, void *)
{
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    SNO snoRope = GlobalSNOGet(GLOBALSNO_SUCCUBUS_ROPE);
    // 
    float fDistance = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    int32 nMaxTargets = (int32)PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
    float fStunTime = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
    float fDamageScalar = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4);
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.flBaseDamageScalar = fDamageScalar;
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitGeometrySphere(ptACD->GetPlace(), fDistance, tChooseTargets.tGeometry);
    tChooseTargets.idPower = idPower;
    tChooseTargets.ePassabilityType = PASSABILITY_PROJECTILE;
    tChooseTargets.dwFlags = 0;
    //
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    PowerAPIBuffStartData tBuff(idPower);
    tBuff.dwDurationMin = (uint32)(fStunTime * 60.0f);
    tBuff.bPositiveEffect = FALSE;
    tBuff.bStun = TRUE;
    tBuff.nBuffIndex = 0;
    tBuff.pExpirationEffect = sSuccubusOnStunEnd;
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos && nMaxTargets)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (!ptACDTarget || !ptACDTarget->IsNPC() || ptACDTarget->IsPet())
        {
            continue;
        }
        // 
        // 
        // 
        SComplexEffectID idRope = SComplexEffectAdd(*ptACD, snoRope, idACDTarget, 2, 1, 1, FALSE);
        tBuff.pExpirationEffectData = (void *)(intptr)idRope;
        PowerAPI_BuffAddToACD(tBuff, *ptACDTarget);
        PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
        // 
        nMaxTargets--;
        // 
    }
}


void sSpellEffect_Succubus_BloodStar(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    ActorCommonData * ptACDTarget = ACDTryToGet(PowerGetTargetACD(ptPower));
    if (!ptACDTarget)
    {
        return;
    }
    // 
    // 
    // 
    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptACD->id, tNavPlaceSource, -1, -1, NULL);
    // 
    Target tTarget;
    PowerGetTarget(idPower, tTarget);
    // 
    Vector3D vContactOffset;
    ptACD->GetContactOffset(vContactOffset, ptPower->dwCurrentContactFrame);
    // 
    tTarget.InitializeFromPlace(ptACDTarget->GetWorldPlaceCenter());
    // 
    PowerAPICreateProjectile tProjectileInfo;
    PowerHelper_ProjectileInit(idPower, ptACD->id, tNavPlaceSource, tTarget, SPELL_SUCCUBUS_BLOODSTAR_IMPACT, tProjectileInfo);
    // 
    ACDID idProjectile = PowerAPI_DoProjectile(tProjectileInfo);
    // 
    // 
    // 
    // 
    if (idProjectile != ACDID_INVALID)
    {
        // 
        PowerAPIBuffStartData tData(idPower);
        tData.bInfinite = TRUE;
        tData.pPeriodicSpellEffect = sBloodStarTrackPlayer;
        tData.dwRefreshRate = 6;
        // 
        PowerAPI_BuffAddToACD(tData, ACDGet(idProjectile));
    }
}

void sBloodStarOnCostEffect(PowerAPIBuffInstance * ptBuff, void *, ACDID idACD, PowerID idPowerFrom, SNO, float flResourceCost, ResourceType, float, BOOL, float)
{
    // 
    // 
    // 
    // 
    // 
    // 
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD || flResourceCost < FLT_EPSILON)
    {
        // 
        // 
        return;
    }
    // 
    // 
    float flHealthLost = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_5);
    // 
    float flReductionCoef = PowerGetFormulaValue(idPowerFrom, TAG_POWER_COST_RESOURCE_REDUCTION_COEFFICIENT);
    if (flReductionCoef > 1e-6f)
    {
        // 
        flHealthLost *= flReductionCoef;
    }
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_None(ptBuff->idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_THORNS;
    tPayload.arCombatDamage[0].flMin = flHealthLost;
    tPayload.bNeverCauseGetHit = TRUE;
    tPayload.bAlwaysHits = TRUE;
    // 
    // 
    PowerAPI_DoPayload(tPayload, idACD, ACDID_INVALID);
}


void sSpellEffect_Succubus_BloodStar_Impact(PowerID idPower, void * pData)
{
    ImpactData * pImpactData = (ImpactData *)pData;
    if (!pImpactData)
    {
        return;
    }
    ActorCommonData * ptACDHit = ACDTryToGet(pImpactData->idACDIntersected);
    if (ptACDHit && !ptACDHit->IsStealthed())
    {
        // 
        // 
        // 
        // 
        // 
        // 
        PowerAPIExplosion tExplosion;
        PowerHelper_ExplosionInit(idPower, ptACDHit->GetPlace(), tExplosion);
        PowerAPI_DoExplosion(tExplosion);
        // 
        // 
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        tPayload.eDeliveryMechanism = PDM_RANGED;
        BOOL bHit = PowerAPI_DoPayload(tPayload, ptACDHit->id, ACDID_INVALID);
        // 
        // 
        // 
        // 
        if (bHit)
        {
            PowerAPIBuffStartData tBuffStartData(idPower);
            // 
            // 
            // 
            tBuffStartData.dwDurationMin = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
            // 
            // 
            tBuffStartData.dwDurationDelta = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1) * 60.0f);
            // 
            tBuffStartData.bPositiveEffect = FALSE;
            tBuffStartData.eAppearanceAttribute = ATTRIBUTE_BUFF_VISUAL_EFFECT;
            tBuffStartData.nAppearanceParameter = 63;
            tBuffStartData.nBuffIndex = 0;
            // 
            float flCostData = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
            // 
            Power * ptPower = PowerTryToGet(idPower);
            PowerSetStoredFloat(ptPower, "CostPenalty", flCostData);
            // 
            // 
            BOOL bUseOnCostEffect = FALSE;
            if (ACDIsPlayer(ptACDHit->id))
            {
                PlayerClass eClass = PlayerGetPlayerClass(ptACDHit->id);
                switch (eClass)
                {
                    case PLAYER_CLASS_DEMONHUNTER:
                    case PLAYER_CLASS_WIZARD:
                    case PLAYER_CLASS_WITCHDOCTOR:
                        bUseOnCostEffect = TRUE;
                        break;
                }
            }
            // 
            if (bUseOnCostEffect)
            {
                tBuffStartData.pOnCostEffect = sBloodStarOnCostEffect;
            }
            else
            {
                // 
                float flPrecentPenalty = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4);
                FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_ARMOR_BONUS_PERCENT, -flPrecentPenalty);
            }
            // 
            // 
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACDHit);
        }
    }
    PowerAPIDestroyProjectile tProjectile; PowerHelper_StopProjectileInit(idPower, pImpactData->idACDProjectile, pImpactData->idACDIntersected, tProjectile); PowerAPI_DestroyProjectile(tProjectile);
}

void sSpellEffect_Succubus_Fly(PowerID idPower, void *)
{
    // 
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_ENEMIES);
    ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_WRECKABLES);
    // 
    // 
    ACDID idACDInitialTarget = PowerGetTargetACD(ptPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDInitialTarget);
    if (!ptACDTarget)
    {
        return;
    }
    // 
    // 
    // 
    WorldPlace tPlaceOpen = ptACDTarget->GetPlace();
    // 
    // 
    PowerAPIChooseOpenGround tOpenGroundInfo;
    NavPlace tNavPlace;
    PowerPlaceToNavMeshHandle(tPlaceOpen, tNavPlace.tNavMeshHandle);
    tNavPlace.tPlace = tPlaceOpen;
    PowerHelper_ChooseOpenGroundInit(idPower, tNavPlace, tOpenGroundInfo, 0.0f);
    NavMeshHandle hNavMeshCurr;
    BOOL bSuccess = PowerAPI_DoChooseOpenGround(*ptACD, tOpenGroundInfo, tPlaceOpen, hNavMeshCurr);
    // 
    // 
    if (!bSuccess)
    {
        tPlaceOpen = ptACDTarget->GetPlace();
    }
    // 
    // 
    // 
    // 
    WorldPlace tPlaceFinal;
    const ActorMovement * ptACM = ptACD->GetActorMovement();
    if (!ptACM)
    {
        return;
    }
    // 
    // 
    PowerComputeDestination(ptPower->snoPower, *ptACM, tPlaceOpen, ptACD->GetPlace(), tPlaceFinal, 0, 10.0f);
    // 
    float flDistSq = Vector3D::Distance2DSquared(tPlaceOpen.wp, tPlaceFinal.wp);
    // 
    if (flDistSq >= 100.0f)
    {
        // 
        PowerHelper_DoFirstTimeCosts(idPower, TRUE);
        PowerInterrupt(idPower, FALSE);
    }
    else
    {
        // 
        // 
        // 
        ptPower->tTarget.InitializeFromPlace(tPlaceFinal);
    }
}

void sSpellEffect_Succubus_Leap(PowerID idPower, void *)  // original line 13997
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_ENEMIES);
    ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_WRECKABLES);
    ACDID idACDInitialTarget = PowerGetTargetACD(ptPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDInitialTarget);
    if (!ptACDTarget)
    {
        return;
    }
    float flDistance = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    PowerCursor tDestCursor;
    {
        WorldPlace wpCursor = ptPower->wpStartLocation;
        WorldPlace wpTarget = ptACDTarget->GetPlace();
        if (wpCursor.idSWorld != wpTarget.idSWorld)
        {
            return;
        }
        Vector3D vDelta = wpCursor.wp - wpTarget.wp;
        Vector2D vFromTarget(vDelta.x, vDelta.y);
        if (vFromTarget.MagnitudeSquared() < 1e-6f)
        {
            return;
        }
        Vector2D vFacing = ptACD->GetForward2D();
        float flDotProduct = Vector2D::DotProduct(vFromTarget, vFacing);
        if (flDotProduct < FLT_EPSILON)
        {
            vFromTarget = vFacing * -1.0f;
        }
        else
        {
            ActorSnapFaceActor(ptACD->id, idACDInitialTarget);
        }
        Angle aNewYaw = vFromTarget.ToYaw();
        tDestCursor.qOrientation.SetRotation(cgwvZAxis, aNewYaw);
        ptPower->listCursors.AddTail_Tracked(tDestCursor);
        Vector3D wvFacing = cgwvXAxis;
        Vector3D::Rotate(cgwvZAxis, aNewYaw, wvFacing);
        wvFacing *= flDistance;
        tDestCursor.tPlace.wp = wvFacing + wpTarget.wp;
        tDestCursor.tPlace.idSWorld = wpTarget.idSWorld;
    }
    ActorMovement * ptACM = ptACD->GetActorMovement();
    if (!ptACM)
    {
        return;
    }
    WorldPlace placeResult = tDestCursor.tPlace;
    PowerComputeDestination(ptPower->snoPower, *ptACM, tDestCursor.tPlace, placeResult, FALSE, 10.0f);
    ptPower->tTarget.InitializeFromPlace(placeResult);
}











static void sCorruptAngel_DamageShield(
    PowerAPIBuffInstance * ptBuff,
    ACDID idACDTarget,
    float * flDamageDone,
    PowerAPIDamage &,
    void *)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
    if (ptACD == NULL)
    {
        return;
    }

    DamageType ePrimaryDamageType = GetPredominantDamageType(flDamageDone, FALSE);
    DamageType eCurrentImmunity = (DamageType)ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_EFFECT_LEVEL));

    ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_RESOURCE_EFFECT_LEVEL), ePrimaryDamageType);

    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (ptPower == NULL)
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();
    uint32 dwLastChange = ptPower->dwParam;
    uint32 dwChangeTime = (uint32)(PowerGetFormulaValue(ptACD, ptBuff->snoPower, TAG_POWER_SCRIPT_FORMULA_1, -1, -1, 0) * 60.0f);

    if (eCurrentImmunity == ePrimaryDamageType)
    {
        float flDamageScalar = PowerGetFormulaValue(ptACD, ptBuff->snoPower, TAG_POWER_SCRIPT_FORMULA_0, -1, -1, 0);
        flDamageDone[ePrimaryDamageType] *= flDamageScalar;
    }

    if (dwNow >= dwLastChange + dwChangeTime)
    {
        if (eCurrentImmunity != ePrimaryDamageType)
        {
            SComplexEffectRemove(ptACD->id, ptBuff->nBuffData);

            ptBuff->nBuffData = SComplexEffectAdd(
                *ptACD,
                GlobalSNOGet(GLOBALSNO_EFFECTGROUP_CORRUPT_ANGEL_UNHOLY_SHIELD),
                ACDID_INVALID, 0, 0, 0, FALSE);
        }

        ptPower->dwParam = dwNow;
    }
}


static void sCorruptAngel_UpdateShield(PowerAPIBuffInstance * ptBuff, void *)
{


    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD == NULL)
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();
    uint32 dwLastChange = ptPower->dwParam;

    uint32 dwExpireTime = (uint32)(PowerGetFormulaValue(ptACD, ptBuff->snoPower, TAG_POWER_SCRIPT_FORMULA_2, -1, -1, 0) * 60.0f);

    if (dwNow > dwLastChange + dwExpireTime)
    {


        SComplexEffectRemove(ptACD->id, ptBuff->nBuffData);
        ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_RESOURCE_EFFECT_LEVEL), -1);
    }
}


void sSpellEffect_CorruptAngel_UnholyShield(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD == NULL)
    {
        return;
    }


    PowerAPIBuffStartData tBuffData(idPower);
    tBuffData.bInfinite = TRUE;
    tBuffData.bPositiveEffect = TRUE;
    tBuffData.pDamageShieldEffect = sCorruptAngel_DamageShield;
    tBuffData.pPeriodicSpellEffect = sCorruptAngel_UpdateShield;

    ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_RESOURCE_EFFECT_LEVEL), -1);

    PowerAPI_BuffAddToACD(tBuffData, *ptACD);
}

void sSpellEffect_CorruptAngel_MeleeStrike(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    RNG & rng = SGameGlobalsGet().rng;
    DamageType eType = RandRange_Internal(rng, 0, NUM_DAMAGE_TYPES - 1);
    DamageType eCurrentImmunity = ptACD->AttributesGetInt(ATTRIBUTE_RESOURCE_EFFECT_LEVEL);
    // 
    float flAlignedDamageScalar = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_None(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_MELEE;
    tPayload.eAttackDirection = ATTACKDIRECTION_STRAIGHT;
    tPayload.flBaseDamageScalar = (eCurrentImmunity == eType) ? flAlignedDamageScalar : 1.0f;
    // 
    DamageRangeFloat & tDmg = tPayload.arCombatDamage[eType];
    tDmg.flMin = tPayload.flBaseDamageScalar * ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_MIN_TOTAL, DAMAGE_TYPE_PHYSICAL));
    tDmg.flDelta = tPayload.flBaseDamageScalar * ptACD->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA_TOTAL, DAMAGE_TYPE_PHYSICAL));
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}


static void sCorruptAngelDashOnDamageEffect(
    PowerAPIBuffInstance * ptBuff,
    void *)
{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }

    float flTargetRadius = PowerGetFormulaValue(ptPower->id, TAG_POWER_SCRIPT_FORMULA_1);
    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = ptPower->id;
    tChooseTargets.ePassabilityType = PASSABILITY_PROJECTILE;
    tChooseTargets.dwFlags = 0;

    PowerHelper_ChooseTargetsInitGeometrySphere(ptACD->GetPlace(), flTargetRadius, tChooseTargets.tGeometry);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(ptBuff->idPower, tPayload);

    float flDamageScalar = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_2);
    float flKnockbackHeight = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_3);
    float flKnockbackMagnitude = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_4);
    float flKnockbackGravity = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_5);

    tPayload.eDeliveryMechanism = PDM_MELEE;
    tPayload.eAttackDirection = ATTACKDIRECTION_STRAIGHT;
    tPayload.eKnockBack = KNOCKBACK_DIRECTION;
    tPayload.wdKnockbackMagnitude = flKnockbackMagnitude;

    tPayload.flKnockbackHeightMin = flKnockbackHeight;
    tPayload.flKnockbackHeightDelta = 0.0f;
    tPayload.flKnockbackGravityMin = flKnockbackGravity;
    tPayload.flKnockbackGravityDelta = 0.0f; tPayload.bKnockbackIgnoresWeight = TRUE;
    tPayload.snoKnockbackPower = SNO_NONE;
    tPayload.bAlwaysHits = TRUE;
    tPayload.flBaseDamageScalar = flDamageScalar;


    Vector3D vAngelForward = ptACD->GetForward();

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);

        if (!ptACDTarget || ptPower->listACDsTracked.Find(idACDTarget))
        {
            continue;
        }

        Vector3D vToTarget = ptACDTarget->GetPlace().wp - ptACD->GetPlace().wp;
        vToTarget.Normalize();

        float flCross = Vector3D::CrossProduct(vAngelForward, vToTarget).z;
        Vector3D vDirection = vAngelForward;

        if (flCross > 0.0f)
        {
            Vector3D::Rotate(cgwvZAxis, 66.0f * PI / 180.0f, vDirection);
        }
        else
        {
            Vector3D::Rotate(cgwvZAxis, -66.0f * PI / 180.0f, vDirection);
        }

        tPayload.wvKnockbackDirection = vDirection;

        PowerAPI_DoPayload(tPayload, idACDTarget, ACDID_INVALID);
        ptPower->listACDsTracked.AddTail_Tracked(idACDTarget);
    }
}


void sCreateDashFrenzyBuff(
    PowerID idPower,
    void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }

    float flDamageInterval = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * TICKS_PER_SECOND;
    PowerAPIBuffStartData tData(idPower);
    tData.bInfinite = TRUE;
    tData.bPositiveEffect = TRUE;
    tData.pPeriodicSpellEffect = sCorruptAngelDashOnDamageEffect;
    tData.dwRefreshRate = (uint32)flDamageInterval;
    tData.nBuffIndex = 0;
    FastAttribSetValueInt(tData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);
    tData.snoEndingEffect = PowerGetEffect(idPower, TAG_POWER_ENDING_EFFECT_GROUP);


    BuffID idBuff = PowerAPI_BuffAddToACD(tData, *ptACD);
    PowerSetStoredInt(ptPower, "idBuff", idBuff);

    int32 nIndex = ptPower->listCursors.Count();
    PowerCursor tCursor;
    tCursor.tPlace = ptPower->wpStartLocation;
    tCursor.qOrientation = ptPower->qStartOrientation;
    ptPower->listCursors.AddTail_Tracked(tCursor);

    PowerSetStoredInt(ptPower, "cursorSource", nIndex);
}




static void sCorruptAngelDashTrailPeriodicEffect(PowerAPIBuffInstance * ptBuff, void *)
{


    Power * ptPower = PowerTryToGet(ptBuff->idPower);


    if (ptPower == NULL)
    {
        return;
    }


    WorldPlace wpDest;
    ptPower->tTarget.GetPlace(wpDest);


    if (ActorPowersGetPrimary((ActorID)ACDGetOwner(ptPower->idACD)) == ptBuff->idPower)
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
        if (ptACD)
        {
            wpDest = ptACD->GetPlace();
        }
    }

    float flRadius = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_9);
    float flBonusHeight = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_10);

    PowerAPIChooseTargets tChooseTargets;
    tChooseTargets.idPower = ptBuff->idPower;
    tChooseTargets.dwFlags = 0;
    tChooseTargets.ePassabilityType = 2;
    PowerHelper_ChooseTargetsInitGeometryCapsule(ptPower->wpStartLocation, wpDest, flRadius, flBonusHeight, tChooseTargets.tGeometry);
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);

    float flDPS = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_11);
    uint32 dwDuration = (uint32)(PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_12) * 60.0f);
    PowerAPIBuffStartData tBuffStartData(ptBuff->idPower);
    tBuffStartData.dwDurationMin = dwDuration;
    tBuffStartData.bPositiveEffect = FALSE;
    tBuffStartData.eStackType = 0;
    tBuffStartData.eDamageType = 1;
    tBuffStartData.flDamagePerSecond = flDPS;
    tBuffStartData.idACDSource = ptPower->idACD;
    tBuffStartData.nBuffIndex = 2;

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {

        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        PowerAPI_BuffAddToACD(tBuffStartData, tACDTarget);
    }
}


void sCreateDashTrailBuff(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD == NULL)
    {
        return;
    }

    uint32 dwDuration = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_7) * 60.0f);
    float flDamageInterval = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_8) * 60.0f;

    PowerAPIBuffStartData tData(idPower);
    tData.bInfinite = FALSE;
    tData.bPositiveEffect = TRUE;
    tData.dwDurationMin = dwDuration;
    tData.eStackType = 1; tData.nMaxStackCount = 10;

    tData.pPeriodicSpellEffect = sCorruptAngelDashTrailPeriodicEffect;
    tData.dwRefreshRate = (uint32)flDamageInterval;
    tData.nBuffIndex = 1;
    FastAttribSetValueInt(tData.idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_NONE, ptPower->snoPower), TRUE);

    PowerAPI_BuffAddToACD(tData, *ptACD);
}

void sSpellEffect_CorruptAngel_PiercingDash_OnStart(
    PowerID idPower,
    void *)
{
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);

    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }

    PowerTriggerAddTimerTrigger(idPower, 6, sCreateDashFrenzyBuff, NULL);




    uint32 dwGroundEffectDuration = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_12) * TICKS_PER_SECOND);
    if (dwGroundEffectDuration)
    {
        uint32 dwGroundEffectDelay = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_13) * TICKS_PER_SECOND);

        PowerTriggerAddTimerTrigger(idPower, dwGroundEffectDelay, sCreateDashTrailBuff, NULL);
    }



}


void sSpellEffect_CorruptAngel_PiercingDash_OnCreate(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);

    if (!ptACD)
    {
        return;
    }

    ptPower->tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_ENEMIES);
    ptPower->tTargetCriteria.dwTeamRelationshipFlags &= ~(1 << TCTR_IGNORES_WRECKABLES);

    ACDID idACDInitialTarget = PowerGetTargetACD(ptPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDInitialTarget);
    if (!ptACDTarget)
    {
        return;
    }


    WorldPlace placeTarget = ptACDTarget->GetPlace();

    Vector3D vToTarget = placeTarget.wp - ptACD->GetPlace().wp;
    vToTarget.Normalize();

    float flDistancePastTarget = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_6);

    vToTarget *= flDistancePastTarget;

    placeTarget.wp += vToTarget;
    const ActorMovement & tACM = *ptACD->GetActorMovement();
    PowerComputeDestination(ptPower->snoPower, tACM, placeTarget, placeTarget, FALSE, 10.0f);

    ptPower->tTarget.InitializeFromPlace(placeTarget);
}



void sSpellEffect_CorruptAngel_PiercingDash_OnEnd(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        BuffID idBuff = PowerGetStoredInt(ptPower, "idBuff");
        PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}




































































































































void sMaghdaShieldEnd(PowerAPIBuffInstance * ptBuff, ACDID, void *)
{
    if (ptBuff)
    {
        Power * ptPower = PowerTryToGet(ptBuff->idPower);
        ActorCommonData * ptACD = PowerGetOwner(ptPower);
        if (ptACD)
        {
            AIStateActivateCooldown((ActorID)ptACD->idOwner, ptPower->uAICoolDownKey, ptPower->uAICoolDownTicks, ptACD);
        }
    }
}










void sSpellEffect_Maghda_Shield(PowerID idPower, void *)
{
    PowerAPIBuffStartData tBuffData(idPower);
    tBuffData.bPositiveEffect = TRUE;
    tBuffData.dwDurationMin = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
    tBuffData.dwDurationDelta = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1) * 60.0f);
    tBuffData.eAppearanceAttribute = ATTRIBUTE_BUFF_VISUAL_EFFECT;
    tBuffData.nAppearanceParameter = 0x42;
    tBuffData.nBuffIndex = 1;
    tBuffData.dwRefreshRate = 30;
    tBuffData.pExpirationEffect = sMaghdaShieldEnd;
    // 
    // 
    // 
    // 
    // 
    // 
    tBuffData.listAIStatesToMerge.AddTail_Tracked(GlobalSNOGet(GLOBALSNO_AISTATE_MAGHDA_SHIELD));
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_AI_IN_SPECIAL_STATE, TRUE);
    // 
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_UNINTERRUPTIBLE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_CANNOT_DIE_DURING, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_STUN_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_ROOT_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_FREEZE_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_SLOWDOWN_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_CHARM_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffData.idFastAttrib, ATTRIBUTE_BLIND_IMMUNE, TRUE);
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        // 
        PowerAPI_BuffAddToACD(tBuffData, *ptACD);
    }
    // 
}


static void sBodyGuardTeleportArrived(const PowerID idPower, void *)
{
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    ptACD->AttributesSetInt(ATTRIBUTE_HIDDEN, FALSE);
    sTeleportPlayEffect(idPower, TRUE);
}



static void sBodyGuard_Teleport(
    BOOL bPreserveTargetDistance,
    PowerID idPower,
    uint32 dwTagToUse,
    const WorldPlace & tDest)
{
    static const float sflTolerance = 3.0f;
    const Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }

    WorldPlace placeCurr = ptACD->GetPlace();
    float flDesiredTargetDistance;
    // The only caller passes TRUE, so the other branch leaves no code; its body is a guess.
    if (!bPreserveTargetDistance)
    {
        flDesiredTargetDistance = PowerGetFormulaValue(idPower, dwTagToUse);
    }
    else
    {
        flDesiredTargetDistance = Vector3D::Distance(tDest.wp, placeCurr.wp);
    }

    uint32 nTries = 0;
    RNG & rng = SGameGlobalsGet().rng;
    WorldPlace placeResult;
    BOOL bPass = FALSE;
    SActorMovement * ptMove = (SActorMovement *)ptACD->GetActorMovement();

    do
    {
        Vector3D vFacing(cgwvXAxis);
        Vector3D::Rotate(cgwvZAxis, RandAngle(rng), vFacing);
        WorldPlace placeDest;
        placeDest.wp = tDest.wp + vFacing * flDesiredTargetDistance;
        placeDest.idSWorld = tDest.idSWorld;

        PowerComputeDestination(ptPower->snoPower, *ptMove, placeDest, tDest, placeResult, 0, 10.0f);

        {

            float flDistanceSq = Vector3D::Distance2DSquared(placeResult.wp, placeDest.wp) - flDesiredTargetDistance;

            if (flDistanceSq > FLT_EPSILON)
            {
                flDistanceSq = Math::Sqrt(flDistanceSq);
            }




            bPass = flDistanceSq < sflTolerance;
        }
    } while (!bPass && nTries++ < 5);


    if (bPass)
    {
        sTeleportPlayEffect(idPower, FALSE);
        ptMove->ActivateTriggers(placeCurr.wp.As2D(), placeResult.wp.As2D());
        ActorSnapToPosition((ActorID)ptACD->idOwner, placeResult, TRUE, TRUE);
        ptACD->AttributesSetInt(ATTRIBUTE_HIDDEN, TRUE);
        PowerTriggerAddTimerTrigger(idPower, 1, sBodyGuardTeleportArrived, NULL);
    }
}

static void sSetTeleportCoolDown(
    PowerAPIBuffInstance * ptBuff,
    uint32 dwTagToUse)
{
    float flDelayInSeconds = PowerGetFormulaValue(ptBuff->idPower, dwTagToUse);
    uint32 dwDelay = (uint32)(flDelayInSeconds * 60.0f);
    ptBuff->nBuffData = GameGetServerTime() + dwDelay;
}


static void sEvaluateTeleport(
    PowerAPIBuffInstance * ptBuff,
    void *)
{
    if (!ptBuff || GameGetServerTime() < (uint32)ptBuff->nBuffData)
    {
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptACD)
    {
        return;
    }

    ActorID idActor = (ActorID)ptACD->idOwner;
    if (!BrainStanceIsWalking(idActor))
    {
        return;
    }

    ACDID idACDTarget = BrainAPIGetActorAttackTarget(ptACD->id);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        return;
    }


    if (ptACDTarget->GetPlace().idSWorld != ptACD->GetPlace().idSWorld)
    {
        return;
    }

    float flDistanceSq = Vector3D::Distance2DSquared(ptACDTarget->GetPlace().wp, ptACD->GetPlace().wp);
    float flDistMinSq = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_0);
    float flDistMaxSq = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_1);
    flDistMaxSq *= flDistMaxSq;
    flDistMinSq += flDistMinSq;

    RNG & rng = SGameGlobalsGet().rng;
    if (flDistMinSq <= flDistanceSq && flDistMaxSq >= flDistanceSq)
    {

        float flTeleportChance = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_2);
        float flChance = RandFloat_Internal(rng);

        if (flChance > flTeleportChance)
        {
            sSetTeleportCoolDown(ptBuff, TAG_POWER_SCRIPT_FORMULA_3);
            return;
        }

        // dwTagToUse is unused when bPreserveTargetDistance is TRUE; the original argument is not recoverable.
        sBodyGuard_Teleport(TRUE, ptBuff->idPower, 0, ptACDTarget->GetPlace());
        sSetTeleportCoolDown(ptBuff, TAG_POWER_SCRIPT_FORMULA_4);
        ActorSnapFaceActor(ptACD->id, ptACDTarget->id);
    }

}

void sSpellEffect_AzmodanBodyguard_TeleportAffix(PowerID idPower, void *)
{


    PowerAPIBuffStartData tBuffData(idPower);
    tBuffData.bInfinite = TRUE;
    tBuffData.dwRefreshRate = 30;
    tBuffData.pPeriodicSpellEffect = sEvaluateTeleport;
    tBuffData.bPositiveEffect = TRUE;
    tBuffData.dwStartTick = 0;
    tBuffData.eStackType = BUFF_STACKTYPE_ONE_PER_SOURCE;
    tBuffData.nMaxStackCount = 1;
    tBuffData.nBuffData = 0;
    tBuffData.snoEndingEffect = PowerGetEffect(idPower, TAG_POWER_ENDING_EFFECT_GROUP);

    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPI_BuffAddToACD(tBuffData, *ptACD);
    }
}


void sApplyFrenzyStack(PowerAPIBuffInstance * ptBuff, void *, PowerAPIDamage &, float, float *, ACDID, ACDID, ACDID)
{
    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (ptACD)
    {
        float flFrenzyDurationSeconds = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_0)
            * 60.0f;
        float flFrenzyStacks = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_1);
        PowerAPIBuffStartData tFrenzyStack(ptBuff->idPower);
        tFrenzyStack.bPositiveEffect = TRUE;
        tFrenzyStack.dwDurationMin = (uint32)flFrenzyDurationSeconds;
        tFrenzyStack.eStackType = BUFF_STACKTYPE_REFRESH;
        tFrenzyStack.nMaxStackCount = (uint32)flFrenzyStacks;
        tFrenzyStack.eAppearanceAttribute = ATTRIBUTE_BUFF_VISUAL_EFFECT;
        tFrenzyStack.nAppearanceParameter = 65;
        tFrenzyStack.nBuffIndex = 1;
        float flAPSBonusPerStack = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_2);
        float flCooldownReduxPercent = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_3);
        FastAttribSetValueFloat(tFrenzyStack.idFastAttrib, ATTRIBUTE_ATTACKS_PER_SECOND_BONUS, flAPSBonusPerStack);
        FastAttribSetValueFloat(tFrenzyStack.idFastAttrib, ATTRIBUTE_AI_COOLDOWN_REDUCTION_PERCENT, flCooldownReduxPercent);
        PowerAPI_BuffAddToACD(tFrenzyStack, *ptACD);
    }
}

















void sSpellEffect_Frenzy(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPIBuffStartData tData(idPower);
        tData.bInfinite = TRUE;
        tData.eStackType = BUFF_STACKTYPE_NEVER;
        tData.pOnHitEffect = sApplyFrenzyStack;
        tData.bPositiveEffect = TRUE;
        PowerAPI_BuffAddToACD(tData, *ptACD);
    }
}






void sSpellEffect_PickupNearby(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->PickupNearby();
    }
}


// X/Common/Networking/GameMessage.h (kept local)

  // size 0x18
void ACDCollDisposeSphereNode(ActorCommonData & tACD);  // ActorCommonCollision.cpp:172
void ACDSetYaw(ACDID idACD, const Angle aYaw);  // ActorCommonData.cpp:3027
void sSpellEffect_SpawnBanner(PowerID idPower, void * ptData);  // SpellEffect.cpp:15102

void sSpellEffect_DespawnBanner(PowerID idPower, void * ptData)
{




    ACDID idACDBanner = (ACDID)(intptr)ptData;
    ActorCommonData * ptACDBanner = ACDTryToGet(idACDBanner);
    if (ptACDBanner)
    {
        ActorID idActorBanner = (ActorID)ACDGetOwner(idACDBanner);
        Actor * ptActorBanner = ActorTryToGet(idActorBanner);
        if (ptActorBanner)
        {
            float flFadeTimeSeconds = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
            int nFadeTimeTicks = (int)(flFadeTimeSeconds * 60.0f);

            ptACDBanner->AttributesSetInt(ATTRIBUTE_DELETED_ON_SERVER, TRUE);

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

            DeathFadeTimeMessage tDeathMessage;
            tDeathMessage.ann = ANNFromACD(idACDBanner);
            tDeathMessage.nTimeUntilFade = 0;
            tDeathMessage.nFadeTime = nFadeTimeTicks;
            tDeathMessage.bRecurseForAttachments = TRUE;
            MessageSendToClient(listDestPlayers, SMSG_SET_DEATH_FADE_TIMES, &tDeathMessage, sizeof(tDeathMessage));

            ACDCollDisposeSphereNode(*ptACDBanner);
            listDestPlayers.RemoveAll();
            PlayerListGetThatKnowACD(listDestPlayers, idACDBanner, FALSE);
            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(idACDBanner);
            MessageSendToClient(listDestPlayers, SMSG_REMOVE_FROM_SPHERE_LIST, &tMessage, sizeof(tMessage));

            ActorDeleteSoon(idActorBanner, nFadeTimeTicks);
        }
    }
}


void sSpellEffect_SpawnBannerBase(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    if (!ptACDPlayer)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
    {
        return;
    }


    if (ptPlayer->nPlayerIndex >= 8)
    {
        return;
    }

    WorldPlace placeFinal;
    WorldPlace placeDesired;
    WorldPlace placeSource = ptACDPlayer->GetPlace();
    ptPower->tTarget.GetPlace(placeDesired);

    if (!SWorldTryToGet(placeDesired.idSWorld))
    {
        return;
    }


    if (PowerIsPrimary(AutoPowerDefinitionR(ptPower->snoPower), ptACDPlayer->id))
    {
        PowerHelper_AddCooldown(idPower);

        ACDID idACDOldBanner = (ACDID)ptACDPlayer->AttributesGetInt(ATTRIBUTE_BANNER_ACDID);
        ActorCommonData * ptACDOldBanner = ACDTryToGet(idACDOldBanner);
        if (ptACDOldBanner)
        {

            ActorID idActorOldBanner = (ActorID)ACDGetOwner(idACDOldBanner);
            ActorFlagForDeletion(idActorOldBanner);
        }

        ActorMovement * ptACM = ptACDPlayer->GetActorMovement();
        PowerComputeDestination(ptPower->snoPower, *ptACM, placeDesired, placeSource, placeFinal, 0, 10.0f);
    }
    else
    {

        placeFinal = placeDesired;
    }

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = 0;
    tActorAdd.tPlace = placeFinal;
    tActorAdd.qOrientation = cgqIdentity;
    tActorAdd.eTeam = TEAM_SCENERY;
    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

    tActorAdd.tMarkersName = MarkerStringHash(CRefString().Format("Arrival Banner Player %d", ptPlayer->nPlayerIndex));
    tActorAdd.nLevel = 1;
    tActorAdd.snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_BANNER_ARRIVAL_PROXY);
    tActorAdd.tSNOName = SNOName(ACTOR_SNO, tActorAdd.snoActor);

    ActorID idActorBannerArrival = ActorAdd(tActorAdd);
    if (idActorBannerArrival == ACTORID_INVALID)
    {
        return;
    }


    int32 uTicksUntilWorldSwitch = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, uTicksUntilWorldSwitch, sSpellEffect_SpawnBanner, (void *)(intptr)idActorBannerArrival);
}

void sSpellEffect_BeginBannerSpawn(PowerID idPower, void *)
{
    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return;
    }
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        // 
        return;
    }
    // 
    // 
    // 
    int32 uTicksUntilBaseSpawn = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3) * 60.0f);
    PowerTriggerAddTimerTrigger(idPower, uTicksUntilBaseSpawn, sSpellEffect_SpawnBannerBase, NULL);
    // 
}


const float BANNER_YAW = 45.0f;

void sSpellEffect_SpawnBanner(PowerID idPower, void * ptData)
{


    ActorID idActorArrival = (ActorID)(intptr)ptData;

    Actor * ptActorArrival = ActorTryToGet(idActorArrival);
    if (!ptActorArrival)
    {
        return;
    }

    ACDID idACDArrival = ACDFindByActor(idActorArrival);
    ActorCommonData * ptACDArrival = ACDTryToGet(idACDArrival);


    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    if (!ptACDPlayer)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
    {
        return;
    }


    if (ptPlayer->nPlayerIndex >= 8)
    {
        return;
    }

    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    WorldPlace placeFinal;
    WorldPlace placeDesired;
    if (ptACDArrival)
    {

        placeFinal = ptACDArrival->GetPlace();
        ActorFlagForDeletion(idActorArrival);
    }
    else
    {
        WorldPlace placeSource = ptACDPlayer->GetPlace();
        ptPower->tTarget.GetPlace(placeDesired);

        ActorMovement * ptACM = ptACDPlayer->GetActorMovement();
        PowerComputeDestination(ptPower->snoPower, *ptACM, placeDesired, placeSource, placeFinal, 0, 10.0f);
    }

    GlobalSNO eGlobalSNO = (GlobalSNO)(GLOBALSNO_ACTOR_EMOTE_BANNER_1 + ptPlayer->nPlayerIndex);

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = 0;
    tActorAdd.tPlace = placeFinal;
    tActorAdd.qOrientation = cgqIdentity;
    tActorAdd.eTeam = TEAM_SCENERY;
    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

    tActorAdd.tMarkersName = MarkerStringHash(CRefString().Format("Banner Player %d", ptPlayer->nPlayerIndex));
    tActorAdd.nLevel = 1;
    tActorAdd.snoActor = GlobalSNOGet(eGlobalSNO);
    tActorAdd.tSNOName = SNOName(ACTOR_SNO, tActorAdd.snoActor);

    ActorID idActorBanner = ActorAdd(tActorAdd);
    if (idActorBanner == ACTORID_INVALID)
    {
        return;
    }


    ACDID idACDBanner = ACDFindByActor(idActorBanner);
    ACDSetYaw(idACDBanner, BANNER_YAW * (PI / 180.0f));


    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
    PowerTemplate eTemplate = PowerGetTemplate(pdefPower, ACDID_INVALID);
    if (eTemplate == ANIMATION_POWER)
    {
        Minimap::ActivateMarker(idACDBanner);
    }

    ptACDPlayer->AttributesSetInt(ATTRIBUTE_BANNER_ACDID, idACDBanner);


    ActorFlagSet(idActorBanner, ACTOR_DYING, TRUE);

    float flDelayUntilFade = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);

    PowerTriggerAddTimerTrigger(idPower,
        (int)(flDelayUntilFade * 60.0f), sSpellEffect_DespawnBanner, (void *)(intptr)idACDBanner);
}




void sSpellEffect_TerrorDemon_MeleeStrike(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_MELEE;
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    // 
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DURATION_MIN) * 60.0f);
    tBuffStartData.dwDurationDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DURATION_DELTA) * 60.0f);
    float flReducedHealingPercent = PowerGetFormulaValue(idPower, TAG_POWER_REDUCED_HEALING_RECEIVED_PERCENT);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_REDUCED_HEALING_RECEIVED_PERCENT, flReducedHealingPercent);
    // 
    SNO snoPower = PowerGetSNO(idPower);
    Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(snoPower, RUNE_TYPE_INVALID, tBuffStartData.nBuffIndex);
    if (eAttrib != ATTRIBUTE_NONE)
    {
        // 
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(eAttrib, snoPower), TRUE);
    }
    // 
    // 
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        // 
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDTarget));
    }
}


void sSpellEffect_TerrorDemon_ShadowPhase(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPI_FreeFromRoot(ptACD->id);
        PowerAPI_FreeFromSnare(ptACD->id);
        PowerAPI_FreeFromStun(ptACD->id);
        PowerAPI_FreeFromFear(ptACD->id);
        PowerAPI_BuffRemoveAllNegativeBuffs(*ptACD);
        sBurrowIn(idPower, -1, -1, TRUE);
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_STUN_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ROOT_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CHARM_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_BLIND_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_SLOWDOWN_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FREEZE_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, TRUE);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}











void sSpellEffect_TerrorDemon_ShadowPhase_End(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPI_DoBurrowOut(ptACD, FALSE);
        BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_TERROR_DEMON_SHADOW_PHASE), ptACD, ACDID_INVALID);
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }
}

#include "../../Common/Console/Avenger.h"
// X/Common/Nav/NavigationMesh.h, X/Server/Actors/Actors.h
BOOL NavMeshSearchForLocation(const WorldPlace & placeStart, const PassabilitySettings & tPassability, const float flMinSearchRadius, const float flMaxSearchRadius, const uint32 dwFindLocationFlags, NavMeshHandle & tNavMeshHandleLanding, WorldPlace & placeLanding, BOOL bSearchOverImpassable, Team eTeam, float flRadiusOverride);  // NavigationMesh.cpp:3421
ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd);  // Actors.cpp:5915




void sSpellEffect_TerrorDemonAvenger_BurrowIn(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }


    sBurrowIn(idPower, -1, -1, TRUE);


    NavPlace tNavPlace;
    tNavPlace.tPlace.wp = ptACD->GetPlace().wp;
    tNavPlace.tPlace.idSWorld = ptACD->GetPlace().idSWorld;
    PowerPlaceToNavMeshHandle(tNavPlace.tPlace, tNavPlace.tNavMeshHandle);
    SNO snoPortal = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_AVENGER_PORTAL);
    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    tActorAdd.tPlace = tNavPlace.tPlace;
    tActorAdd.eTeam = ptACD->GetTeam();
    tActorAdd.tSNOName.Set(ACTOR_SNO, snoPortal);

    tActorAdd.snoActor = snoPortal;
    ScenesCommonNavMeshNodeFromPlace(tNavPlace.tPlace, &tNavPlace.tNavMeshHandle, FALSE, FALSE);

    ActorID idPortal = ActorAdd(tActorAdd);
    Actor * ptActorPortal = ActorTryToGet(idPortal);
    if (ptActorPortal)
    {
        ActorCommonData * ptACDPortal = ACDTryToGet(ptActorPortal->idACD);
        if (ptACDPortal)
        {



            ptACDPortal->SetInvulnerable(TRUE);
            ActorDeleteSoon((ActorID)ACDGetOwner(ptActorPortal->idACD), 160);
        }
    }
}

void sSpellEffect_TerrorDemonAvenger_SpawnClones(PowerID idPower, void *)
{




    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD || !ptACD->IsAvenger())
    {
        return;
    }


    ptACD->SetInvulnerable(FALSE);
    ptACD->SetTargetable(TRUE);

    SNO snoShadowCloneSpawnEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_SHADOWCLONE_SPAWN);

    AvengerData & tAvengerData = *ptACD->ptAvengerData;
    for (uint32 nIndex = 0; nIndex < tAvengerData.arVictims.size(); nIndex++)
    {
        ActorAddStruct tActorAdd;
        WorldPlace tPlaceStart = ptACD->GetPlace();
        WorldPlace tPlaceResult;
        NavMeshHandle tGoodNavMesh;
        PassabilitySettings tPassability;
        PassabilitySettingsInit(tPassability);
        tPassability.ePassabilityType = PASSABILITY_MONSTER;


        BOOL bFoundGoodLoc = NavMeshSearchForLocation(tPlaceStart, tPassability, 5.0f + nIndex * 3.0f, 30.0f, 0, tGoodNavMesh, tPlaceResult, FALSE, TEAM_MONSTER_0, 0.0f);
        if (!bFoundGoodLoc)
        {
            continue;
        }

        GlobalSNO eGlobalSNOClone = Avenger::Console::GetGlobalSNOActorForShadowCloneVictim(tAvengerData.arVictims[nIndex]);
        if (eGlobalSNOClone == GLOBALSNO_INVALID)
        {
            continue;
        }

        SNO snoClone = GlobalSNOGet(eGlobalSNOClone);
        Quaternion qOrientation = ptACD->GetOrientation();
        tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
        tActorAdd.tSNOName.snoHandle = snoClone;
        tActorAdd.snoActor = snoClone;
        tActorAdd.dwActorAddFlags = (1 << ACTORADD_DONT_PERTURB);
        tActorAdd.tPlace = tPlaceResult;
        tActorAdd.qOrientation = qOrientation;
        tActorAdd.eTeam = TEAM_MONSTER_0;
        tActorAdd.bAvenger = TRUE;

        GBHandleList listGroupAffixes(GBGetHandlePool());
        EncounterAddStruct tEncounterAdd;
        EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

        ActorID idActor = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
        if (idActor != ACTORID_INVALID)
        {
            ACDID idACD = ActorGet(idActor)->idACD;

            ActorCommonData * ptACDClone = ACDTryToGet(idACD);
            if (ptACDClone)
            {
                ptACDClone->AttributesSetInt(ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, TRUE);
                tAvengerData.arVictimIDs.push_back(idActor);
                ACDPlayEffect(idACD, EFFECT_PLAY_EFFECT_GROUP, snoShadowCloneSpawnEffect, ACDID_INVALID);
            }
        }
    }
}




static void sSpellEffect_CannibalBarbarian_IntroFear(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_RANGED;
    tPayload.bAlwaysHits = TRUE;
    // 
    tPayload.rangeFearLength.flMin = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4);
    tPayload.rangeFearLength.flDelta = 0.0f;
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    float flRadius = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    // 
    // 
    ptPower->tTargetCriteria.dwTeamRelationshipFlags = 4;
    // 
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(ptACD->GetPlace(), flRadius, tChooseTargets.tGeometry);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}

void sSpellEffect_CannibalBarbarian_Intro(PowerID idPower, void *)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_RANGED;
    tPayload.bAlwaysHits = TRUE;
    // 
    // 
    tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
    float flKnockbackHeight = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
    float flKnockbackMagnitude = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
    float flKnockbackGravity = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3);
    tPayload.wdKnockbackMagnitude = flKnockbackMagnitude;
    tPayload.flKnockbackHeightMin = flKnockbackHeight;
    tPayload.flKnockbackHeightDelta = 0.0f;
    tPayload.flKnockbackGravityMin = flKnockbackGravity;
    tPayload.flKnockbackGravityDelta = 0.0f;
    tPayload.bKnockbackIgnoresWeight = TRUE;
    tPayload.snoKnockbackPower = SNO_NONE;
    // 
    tPayload.flBaseDamageScalar = 0.0f;
    // 
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    float flRadius = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    // 
    // 
    ptPower->tTargetCriteria.dwTeamRelationshipFlags = 4;
    // 
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(ptACD->GetPlace(), flRadius, tChooseTargets.tGeometry);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
    // 
    uint32 nFearDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_5));
    // 
    // 
    PowerTriggerAddTimerTrigger(idPower, nFearDelay, sSpellEffect_CannibalBarbarian_IntroFear, NULL);
}


void sSpellEffect_CoreEliteDemon_DropPod_Setup(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        sBurrowIn(idPower, -1, -1, TRUE);
    }
}







ACDID sAddProxyHelper(SNO snoProxyActor, float flDuration, const WorldPlace & tPlace, const Quaternion & qOrientation, PowerID idPower)
{
    // 
    // 
    // 
    // 
    // 
    // 
    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = snoProxyActor;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
    tActorAdd.eFastAttribType = FAT_SIMPLE;
    tActorAdd.eTeam = PowerGetTeam(idPower);
    tActorAdd.tPlace = tPlace;
    tActorAdd.qOrientation = qOrientation;
    tActorAdd.flScale = 1.0f;
    tActorAdd.tSNOName = SNOName(ACTOR_SNO, snoProxyActor);
    tActorAdd.dwActorAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK);
    tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_PERTURB);
    tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    // 
    // 
    ActorID idActorProxy = ActorAdd(tActorAdd);
    if (idActorProxy == ACTORID_INVALID)
    {
        return ACDID_INVALID;
    }
    // 
    // 
    ACDID idACDProxy = ACDFindByActor(idActorProxy);
    if (idACDProxy == ACDID_INVALID)
    {
        return ACDID_INVALID;
    }
    // 
    // 
    ActorCommonData & tACDProxy = ACDGet(idACDProxy);
    tACDProxy.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
    ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);
    tACDProxy.AttributesSetInt(ATTRIBUTE_IS_POWER_PROXY, TRUE);
    // 
    uint32 dwDuration = Math::FloatToInt(flDuration * 60.0f);
    // 
    ActorDeleteSoon(idActorProxy, dwDuration);
    // 
    return idACDProxy;
}
void sSpellEffect_CoreEliteDemon_StartDrop(PowerID idPower, void * ptData)
{

    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    ACDID idACDProxy = (ACDID)(intptr)ptData;
    ActorCommonData * ptACDProxy = ACDTryToGet(idACDProxy);
    if (!ptACDProxy)
    {
        PowerAPI_DoBurrowOut(ptACD, FALSE);
        return;
    }

    Target tTarget;
    tTarget.InitializeFromPlace(ptACDProxy->GetPlace());

    PowerAPI_DoBurrowOut(ptACD, TRUE);

    Power * ptPower = PowerTryToGet(PowerStart(ptACD->id, GlobalSNOGet(GLOBALSNO_POWERS_DEMON_DROPPOD), tTarget));

    ptACD->AttributesSetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE, FALSE);

    PowerAPIBuffStartData tBuff(idPower);
    tBuff.dwDurationMin = 7200;
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_STUN_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_ROOT_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_CHARM_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_BLIND_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_SLOWDOWN_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_FREEZE_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuff.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);

    if (ptPower)
    {
        ptPower->dwParam = PowerAPI_BuffAddToACD(tBuff, *ptACD);
    }
}



void sSpellEffect_CoreEliteDemon_DropPod_Drop_Begin(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }

    ACDID idACDTarget = PowerGetTargetACD(ptPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        return;
    }

    WorldPlace tPlaceOpen = ptACDTarget->GetPlace();

    PowerAPIChooseOpenGround tOpenGroundInfo;
    NavPlace tNavPlace;
    PowerPlaceToNavMeshHandle(tPlaceOpen, tNavPlace.tNavMeshHandle);
    tNavPlace.tPlace = tPlaceOpen;
    PowerHelper_ChooseOpenGroundInit(idPower, tNavPlace, tOpenGroundInfo, 0.0f);
    NavMeshHandle hNavMeshCurr;
    BOOL bSuccess = PowerAPI_DoChooseOpenGround(*ptACDOwner, tOpenGroundInfo, tPlaceOpen, hNavMeshCurr);

    if (!bSuccess)
    {
        tPlaceOpen = ptACDTarget->GetPlace();
    }

    ActorSnapToPosition((ActorID)ptACD->idOwner, tPlaceOpen, TRUE, TRUE);

    float flWarningLife = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);

    ACDID idProxy = sAddProxyHelper(GlobalSNOGet(GLOBALSNO_ACTOR_DROP_POD_PROXY_WARNING),
        flWarningLife + 0.5f,
        tPlaceOpen,
        ptACD->GetOrientation(),
        idPower);

    PowerTriggerAddTimerTrigger(idPower,
        (uint32)(flWarningLife * 60.0f),
        sSpellEffect_CoreEliteDemon_StartDrop,
        (void *)(intptr)idProxy);

    ptACD->AttributesSetInt(ATTRIBUTE_AI_IN_SPECIAL_STATE, TRUE);
}


void sSpellEffect_CoreEliteDemon_DropPod_Drop_Impact(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    ptACD->AttributesSetInt(ATTRIBUTE_ALPHA_ATTACHMENTS, 0);
    // 
    uint32 dwNow = GameGetServerTime();
    PowerAPI_BuffSetEndTick(ptPower->dwParam, dwNow + 10);
    // 
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_MELEE;
    tPayload.bAlwaysHits = TRUE;
    // 
    // 
    tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
    float flDamageScalar = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
    float flKnockbackHeight = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
    float flKnockbackMagnitude = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3);
    float flKnockbackGravity = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4);
    tPayload.wdKnockbackMagnitude = flKnockbackMagnitude;
    tPayload.flKnockbackHeightMin = flKnockbackHeight;
    tPayload.flKnockbackHeightDelta = 0.0f;
    tPayload.flKnockbackGravityMin = flKnockbackGravity;
    tPayload.flKnockbackGravityDelta = 0.0f;
    tPayload.bKnockbackIgnoresWeight = TRUE;
    tPayload.snoKnockbackPower = SNO_NONE;
    tPayload.flBaseDamageScalar = flDamageScalar;
    // 
    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
    // 
    float fRadius = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_5);
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(ptACD->GetPlace(), fRadius, tChooseTargets.tGeometry);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}



void sSpellEffect_HoodedNightmare_Lightning_Effect(PowerID idPower, void * pData)
{

    ImpactData * pImpactData = (ImpactData *)pData;

    pImpactData->bStop = FALSE;
    if (pImpactData->idACDIntersected == ACDID_INVALID)
    {
        PowerAPIDestroyProjectile tProjectile;
        PowerHelper_StopProjectileInit(idPower, pImpactData->idACDProjectile, ACDID_INVALID, tProjectile);
        PowerAPI_DestroyProjectile(tProjectile);
        return;
    }

    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD == NULL)
    {
        return;
    }

    AffectedACD tTarget;
    tTarget.idACDSource = ptACD->id;
    tTarget.idACDTarget = pImpactData->idACDIntersected;
    if (ptPower->listAffectedACDs.Find(tTarget))
    {
        return;
    }

    ACDPlayEffect(pImpactData->idACDIntersected, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_HOODEDNIGHTMARE_LIGHNING_IMPACT), ACDID_INVALID);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);

    PowerAPI_DoPayload(tPayload, pImpactData->idACDIntersected, ACDID_INVALID);
}



void sSpellEffect_HoodedNightmare_Lightning(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD == NULL)
    {
        return;
    }

    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptACD->id, tNavPlaceSource, -1, -1, NULL);

    float fDistance = 20.0f;
    WorldPlace tPlace;
    ptPower->tTarget.GetPlace(tPlace);

    Vector3D vStart(ptACD->GetPlace().wp);
    Vector3D vToTarget = tPlace.wp - vStart;
    Vector3D vFinal = vStart + vToTarget * fDistance;
    vFinal.z = tNavPlaceSource.tPlace.wp.z;
    tPlace.wp = vFinal;

    ptPower->tTarget.InitializeFromPlace(tPlace);

    PowerAPICreateProjectile tProjectile;
    PowerHelper_ProjectileInit(idPower, ptACD->id, tNavPlaceSource, ptPower->tTarget, SPELL_HOODEDNIGHTMARE_LIGHTNING_EFFECT, tProjectile);

    tProjectile.tProjectileData.tDurationData.dwTime = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);

    ACDID idACDProjectile = PowerAPI_DoProjectile(tProjectile);

    ActorCommonData * ptACDProjectile = ACDTryToGet(idACDProjectile);
    if (ptACDProjectile)
    {



        ptACDProjectile->AttributesSetFloat(FastAttribKey(ATTRIBUTE_PIERCE_CHANCE), 1.0f);
    }
}

void sSpellEffect_HoodedNightmare_Curses(PowerID idPower, void *)
{


    ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
    if (!ptACDOwner)
    {
        return;
    }

    ACDID idACDTarget = PowerGetTargetACD(idPower);
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
    {
        return;
    }

    SNO snoPower = PowerGetSNO(idPower);

    BuffID idExisting = PowerAPI_BuffFindOnACD(snoPower, ptACDTarget, ACDID_INVALID);
    if (idExisting != BUFFID_INVALID)
    {
        PowerAPI_BuffFlagForDeletion(idExisting, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }

    PowerAPIBuffStartData tBuff(idPower);
    tBuff.bPositiveEffect = FALSE;
    tBuff.nBuffIndex = RandRange_Internal(SGameGlobalsGet().rng, 0, 3);
    tBuff.dwDurationMin = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
    tBuff.eStackType = BUFF_STACKTYPE_ONE_PER_SOURCE;

    Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(snoPower, RUNE_TYPE_INVALID, tBuff.nBuffIndex);
    if (eAttrib != -1)
    {
        FastAttribSetValueInt(tBuff.idFastAttrib, MakeAttribKey(eAttrib, snoPower), TRUE);
    }

    switch (tBuff.nBuffIndex)
    {
    case 0:
        {
            float flResistPercent = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
            for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++)
            {
                FastAttribSetValueFloat(tBuff.idFastAttrib, MakeAttribKey(ATTRIBUTE_RESISTANCE_PERCENT, i), flResistPercent);
            }
        }
        break;
    case 1:
        {


            float flResistPercent = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
            FastAttribSetValueFloat(tBuff.idFastAttrib, ATTRIBUTE_ARMOR_BONUS_PERCENT, flResistPercent);
        }
        break;
    case 2:
        {


            float flReduction = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3);
            FastAttribSetValueFloat(tBuff.idFastAttrib, ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT, flReduction);
        }
        break;
    case 3:


        tBuff.flSlowAmount = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4);
        tBuff.bSlow = TRUE;
        tBuff.bFailsIfSlowImmune = TRUE;
        break;
    }


    ACDPlayEffectGroupACDToACD(GlobalSNOGet(GLOBALSNO_HOODEDNIGHTMARE_CURSES), ptACDOwner->id, ptACDTarget->id);
    PowerAPI_BuffAddToACD(tBuff, *ptACDTarget);
}


static void sBoneArmorDamageShield(
    PowerAPIBuffInstance * ptBuff,
    ACDID,
    float *,
    PowerAPIDamage & tPowerDamage,
    void *)
{
    float flRemaining = (float)ptBuff->nBuffData;
    POS pos = tPowerDamage.listDamage.GetHeadPosition();

    float flAmountAbsorbed = 0.0f;

    while (pos)
    {
        DamageAttribute & tAttrib = tPowerDamage.listDamage.GetNext(pos);

        if (tAttrib.nDuration > 0)
        {
            continue;
        }

        if (!(tAttrib.flValue < flRemaining))
        {
            flAmountAbsorbed += flRemaining;
            tAttrib.flValue -= flRemaining;
            flRemaining = 0.0f;
            break;
        }
        else
        {
            flAmountAbsorbed += tAttrib.flValue;
            flRemaining -= tAttrib.flValue;
            tAttrib.flValue = 0.0f;
        }
    }


    if (flRemaining < FLT_EPSILON)
    {
        PowerAPI_BuffFlagForDeletion(ptBuff, TRUE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    }

    ptBuff->nBuffData = (uint32)flRemaining;

    if (flRemaining > 0.0f && tPowerDamage.listDamage.Count())
    {
        Player * ptPlayerAttacker = ACDGetControllingPlayer(tPowerDamage.idACDSource);
        FloatingNumberMessage tMessage;
        tMessage.annTarget = ANNFromACD(tPowerDamage.idACDTarget);
        tMessage.flDamage = flAmountAbsorbed;
        tMessage.eType = 11;  // FloatingNumberType 11; enumerator not in src

        Player * ptPlayer = ACDGetControllingPlayer(tPowerDamage.idACDTarget);
        PlayerList listDestPlayers(PlayerListGetElementPool());
        if (ptPlayer)
        {
            listDestPlayers.AddTail_Tracked(ptPlayer);
        }
        if (ptPlayerAttacker)
        {
            listDestPlayers.AddTail_Tracked(ptPlayerAttacker);
        }
        if (listDestPlayers.Count() > 0)
        {
            MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
        }
    }
}


void sSpellEffect_HoodedNightmare_BoneArmor(PowerID idPower, void *)
{


    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD == NULL)
    {
        return;
    }

    float flDuration = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    float flMaxABS = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);

    PowerAPIBuffStartData tBuff(idPower);
    tBuff.dwDurationMin = (uint32)(flDuration * 60.0f);
    tBuff.pDamageShieldEffect = sBoneArmorDamageShield;
    tBuff.nBuffData = (uint32)flMaxABS;
    tBuff.nBuffIndex = 0;

    tBuff.eAppearanceAttribute = ATTRIBUTE_BUFF_VISUAL_EFFECT; tBuff.nAppearanceParameter = 67;

    PowerAPI_BuffAddToACD(tBuff, *ptACD);
}


void sSpellEffect_HoodedNightmare_Gateway(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        // 
        return;
    }
    float flDuration = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    Target tTarget;
    // 
    PowerGetTarget(idPower, tTarget);
    WorldPlace tPlace;
    tTarget.GetPlace(tPlace);
    // 
    ACDID idACDProxy = sAddProxyHelper(
        GlobalSNOGet(GLOBALSNO_ACTOR_HOODEDNIGHTMARE_GATEWAY),
        flDuration,
        tPlace,
        ptACD->GetOrientation(),
        idPower);
    // 
    if (idACDProxy != ACDID_INVALID)
    {
        ActorID idActorProxy = (ActorID)ACDGetOwner(idACDProxy);
        ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
        ACDGet(idACDProxy).AttributesSetInt(ATTRIBUTE_SUMMONED_BY_ACDID, ptACD->GenerateSummonerID());
    }
    // 
    // 
    PowerAPIBuffStartData tData(idPower);
    tData.dwDurationMin = (uint32)(flDuration * 60.0f);
    // 
    uint32 nCurrentSummons = ptACD->AttributesGetInt(ATTRIBUTE_SUMMON_COUNT);
    ptACD->AttributesSetInt(ATTRIBUTE_SUMMON_COUNT, nCurrentSummons + 1);
}


void sSpellEffect_StampTarget_IntoGround(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ACDID idACD = ptPower->tTarget.GetACD();
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD)
        {
            ptPower->tTarget.InitializeFromPlace(ptACD->GetPlace());
        }
    }
}





void sSpellEffect_RollToTarget(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = PowerGetOwner(ptPower);
        if (ptACD)
        {
            ActorMovement * ptMove = ptACD->GetActorMovement();
            if (ptMove)
            {
                ACDCollSettings tCollSettingsDuring;
                ACDCollSettings tCollSettingsDest;
                ptMove->GetCollSettings(ptPower->snoPower, POWER_PASSABILITY_DURING, tCollSettingsDuring);
                ptMove->GetCollSettings(ptPower->snoPower, POWER_PASSABILITY_DESTINATION, tCollSettingsDest);
                WorldPlace placeTarget;
                if (ptPower->tTarget.GetPlace(placeTarget))
                {
                    WorldPlace placeActor;
                    ptMove->GetPlace(placeActor);
                    PowerComputeDestination(tCollSettingsDuring, tCollSettingsDest, *ptMove, placeTarget, placeActor, placeTarget, 0, 10.0f, NULL);
                    ptPower->tTarget.InitializeFromPlace(placeTarget);
                }
            }
        }
    }
}


















static void sSpellEffect_MorluMelee_PhoenixShieldEffect(PowerAPIBuffInstance * ptBuff, ACDID, float * flDamageDone, PowerAPIDamage &, void *);
    // 
void sSpellEffect_MorluMelee_PhoenixOnStart(PowerID idPower, void *)
{
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.snoPower = ptPower->snoPower;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
    tBuffStartData.nMaxStackCount = 1;
    tBuffStartData.dwDurationMin = 0;
    tBuffStartData.nBuffIndex = 0;
    tBuffStartData.bPositiveEffect = TRUE;
    // 
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, 1);
    tBuffStartData.pDamageShieldEffect = sSpellEffect_MorluMelee_PhoenixShieldEffect;
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    // 
}

static void sSpellEffect_MorluMelee_Phoenix_PhoenixExpiration(PowerAPIBuffInstance * ptBuff, ACDID, void *)
{
    // 
    // 
    // 
    if (!ptBuff)
    {
        return;
    }
    // 
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptACD)
    {
        return;
    }
    // 
    float flPhoenixStateHPPercent = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_1);
    float flHPMax = ptACD->GetHitPointsMaxTotal();
    ptACD->SetHitPoints(flPhoenixStateHPPercent * flHPMax, TRUE, FALSE);
    // 
    AnimPlayerPlayAnimation(*ptACD, TAG_ACTOR_ANIMATION_TRANSFORM_02, 0xfffffffe, FALSE, ANIM_REASON_POWER, 0.0f, FALSE, 1.0f);
    AnimPlayerSetIdleAnimation(*ptACD, TAG_ACTOR_ANIMATION_NEUTRAL);
    // 
    PowerAPIBuffStartData tBuffStartData(ptPower->id);
    tBuffStartData.snoPower = ptPower->snoPower;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
    tBuffStartData.nMaxStackCount = 1;
    tBuffStartData.dwDurationMin = 0;
    tBuffStartData.nBuffIndex = 0;
    tBuffStartData.bPositiveEffect = TRUE;
    // 
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, 1);
    tBuffStartData.pDamageShieldEffect = sSpellEffect_MorluMelee_PhoenixShieldEffect;
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    // 
    ActorID idActor = (ActorID)ptACD->idOwner;
    ActorFlagSet(idActor, ACTOR_DOESNT_DEACTIVATE_BIT, FALSE);
}

static void sSpellEffect_MorluMelee_PhoenixShieldEffect(PowerAPIBuffInstance * ptBuff, ACDID, float * flDamageDone, PowerAPIDamage &, void *)
{
    // 
    // 
    // 
    // 
    // 
    if (!ptBuff)
    {
        return;
    }
    // 
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    // 
    float flTotalDamage = 0.0f;
    for (int x = 0; x < NUM_DAMAGE_TYPES; x++) { flTotalDamage += flDamageDone[x]; }
    // 
    // 
    float flCurrentHP = ptACD->GetHitPointsCurrent();
    // 
    // 
    if (flCurrentHP - flTotalDamage < 1e-6f)
    {
    // 
    // 
        for (int x = 0; x < NUM_DAMAGE_TYPES; x++) { flDamageDone[x] = 0.0f; }
    // 
    // 
    // 
        float flPhoenixStateHPPercent = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_0);
        float flHPMax = ptACD->GetHitPointsMaxTotal();
        ptACD->SetHitPoints(flPhoenixStateHPPercent * flHPMax, TRUE, FALSE);
    // 
    // 
        PowerAPI_BuffRemoveAllNegativeBuffs(*ptACD);
    // 
        ActorID idActor = (ActorID)ptACD->idOwner;
        PowerID idCurrPower = ActorPowersGetPrimary(idActor);
        if (idCurrPower != POWERID_INVALID)
        {
            Power * ptPowerScoped = PowerTryToGet(idCurrPower);
            if (ptPowerScoped)
            {
                PowerInterrupt(idCurrPower, FALSE);
            }
        }
    // 
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
        {
            ptMove->StopNow();
        }
    // 
        ActorFlagSet(idActor, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
    // 
        BrainAPIDoIdle(idActor);
    // 
    // 
        PowerAPI_BuffFlagForDeletion(ptBuff->id, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    // 
        PowerAPIBuffStartData tBuffStartData(ptPower->id);
        tBuffStartData.snoPower = ptPower->snoPower;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
        tBuffStartData.nMaxStackCount = 1;
        tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_2) * 60.0f);
        tBuffStartData.bRemoveOnDeath = FALSE;
        tBuffStartData.nBuffIndex = 1;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_STUN_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_ROOT_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CHARM_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_BLIND_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_SLOWDOWN_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FREEZE_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_GETHIT_IMMUNE, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HAS_SPECIAL_DEATH_ANIMTAG, 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_BURROWED, 1);
        tBuffStartData.pExpirationEffect = sSpellEffect_MorluMelee_Phoenix_PhoenixExpiration;
        tBuffStartData.listAIStatesToMerge.AddTail_Tracked(GlobalSNOGet(GLOBALSNO_AISTATE_DONOTHING_PRIORITY));
    // 
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    // 
        AnimPlayerPlayAnimation(*ptACD, TAG_ACTOR_ANIMATION_DEATH_01, 0xfffffffe, FALSE, ANIM_REASON_POWER, 0.0f, FALSE, 1.0f);
        AnimPlayerSetIdleAnimation(*ptACD, TAG_ACTOR_ANIMATION_DEAD_01);
    }
}


ACDID sSpawnMorluCasterMeteorProxy(PowerID idPower, WorldPlace tPlace, SNO snoActor, uint32 nDuration)
{
    // 
    // 
    // 
    PowerAPISummonActor tSummonInfo;
    BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoActor, ACDID_INVALID, tSummonInfo, FALSE);
    if (!bSuccess)
    {
        return ACDID_INVALID;
    }

    // 
    tSummonInfo.placeSummonLocation = tPlace;
    tSummonInfo.dwAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    // 
    // 
    // 
    ACDID idACDProxy = PowerAPI_SummonActor(tSummonInfo);
    if (idACDProxy != ACDID_INVALID)
    {
        ActorID idActorProxy = (ActorID)ACDGetOwner(idACDProxy);
        ActorCommonData & tACDProxy = ACDGet(idACDProxy);
        tACDProxy.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
        ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);
        tACDProxy.AttributesSetInt(ATTRIBUTE_IS_POWER_PROXY, TRUE);
        // 
        ActorDeleteSoon(idActorProxy, nDuration);
    }
    return idACDProxy;
}




void sSpellEffect_MorluCaster_Meteor_Periodic_Damage(PowerID idPower, void *)
{
    Power * ptPower = PowerGet(idPower);
    PowerAPIChooseTargets tTargets;
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, ptPower->tTarget, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_None(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_DOT;
    tPayload.arCombatDamage[DAMAGE_TYPE_FIRE].flMin = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4);
    tPayload.arCombatDamage[DAMAGE_TYPE_FIRE].flDelta = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_5);
    tPayload.flDamageDurationInSecs[DAMAGE_TYPE_FIRE] = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
    tPayload.bNeverCauseGetHit = TRUE;
    tPayload.bAlwaysHits = TRUE;
    tPayload.bTriggersHitProcs = FALSE;
    // 
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}


void sSpellEffect_MorluCaster_Meteor_Impact(PowerID idPower, void *)
{
    // 
    // 
    // 
    Power * ptPower = PowerGet(idPower);
    // 
    PowerAPIChooseTargets tTargets;
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, ptPower->tTarget, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);

    uint32 nImpactTicks = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1));
    uint32 nImpactInterval = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2) * 60.0f);
    uint32 nAfterburnDuration = nImpactTicks * nImpactInterval;

    WorldPlace tPlace;
    ptPower->tTarget.GetPlace(tPlace);
    // 
    sSpawnMorluCasterMeteorProxy(idPower, tPlace,
        GlobalSNOGet(GLOBALSNO_ACTOR_MORLU_CASTER_METEOR_IMPACT),
        nAfterburnDuration);
    // 
    sSpawnMorluCasterMeteorProxy(idPower, tPlace,
        GlobalSNOGet(GLOBALSNO_ACTOR_MORLU_CASTER_METEOR_AFTERBURN),
        nAfterburnDuration);

    // 
    for (uint32 i = 1; i <= nImpactTicks; i++)
    {
        PowerTriggerAddTimerTrigger(idPower, i * nImpactInterval, sSpellEffect_MorluCaster_Meteor_Periodic_Damage, NULL);
    }
}

void sSpellEffect_MorluCaster_Meteor(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    uint32 nImpactDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
    // 
    // 
    // 
    WorldPlace tPlace;
    ptPower->tTarget.GetPlace(tPlace);
    // 
    ACDID idACDProxy = sSpawnMorluCasterMeteorProxy(idPower,
        tPlace,
        GlobalSNOGet(GLOBALSNO_ACTOR_MORLU_CASTER_METEOR_PENDING),
        nImpactDelay);
    // 
    // 
    if (idACDProxy != ACDID_INVALID)
    {
        // 
        ActorCommonData & tACDProxy = ACDGet(idACDProxy);
        // 
        // 
        // 
        ptPower->tTarget.InitializeFromPlace(tACDProxy.GetPlace());
        // 
        // 
        PowerTriggerAddTimerTrigger(idPower, nImpactDelay, sSpellEffect_MorluCaster_Meteor_Impact, NULL);
    }
}

void sSpellEffect_MorluCaster_BreathOfFire(PowerID idPower, void *)
{
    // 
    // 
    // 
    // 
    // 
    PowerAPIChooseTargets tTargets;
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eKnockBack = KNOCKBACK_FROM_ATTACKER;
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}


void sSpellEffect_MorluCaster_Shift(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        if (ptPower->tTarget.eType == TT_POINT)
        {
            ActorCommonData * ptACD = PowerGetOwner(ptPower);
            if (ptACD)
            {
                ActorID idActorOwner = (ActorID)ptACD->idOwner;
                ActorSnapToPosition(idActorOwner, ptPower->tTarget.tPlace, TRUE, TRUE);
            }
        }
        else
        {








            DisplayInternalError(NULL, "", 16479, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
}

static void sWizard_LightningSpeedOnGetHit(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tPowerDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDTarget)
{
    PowerID idPower = ptBuff->idPower;
    if (PowerTryToGet(idPower))
    {
        ActorCommonData & tACD = ACDGet(idACDTarget);
        if (tPowerDamage.eDeliveryMechanism == PDM_MELEE)
        {
            BuffID idBuff = PowerAPI_BuffFindOnACD(PowerGetChildPower(idPower), tACD, ACDID_INVALID);
            if (idBuff == BUFFID_INVALID)
            {
                SNO snoEffect = PowerContactTagGetSNO(idPower, TAG_POWER_GENERIC_EFFECT_GROUP_0);
                if (snoEffect != SNO_NONE)
                {
                    ACDPlayEffect(idACDTarget, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
                }
            }
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = PowerGetChildPower(idPower);
            tBuffStartData.bPositiveEffect = TRUE;
            tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
            tBuffStartData.nMaxStackCount = Math::FloatRoundToInt(PowerGetFormulaValue(idPower, TAG_POWER_LIGHTNING_SPEED_STACK_COUNT));
            tBuffStartData.dwDurationMin = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_LIGHTNING_SPEED_DURATION) * 60.0f);
            tBuffStartData.nAppearanceParameter = 42;
            float flMovementScalar = PowerGetFormulaValue(idPower, TAG_POWER_LIGHTNING_SPEED_MOVEMENT_SCALAR_BONUS);
            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR, flMovementScalar);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }
    }
}
















void Wizard_LightningSpeed(const PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.bInfinite = TRUE;
        tBuffStartData.bPositiveEffect = TRUE;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.pOnGetHitEffect = sWizard_LightningSpeedOnGetHit;
        PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    }
}




void sSpellEffect_Console_Teleport(const PowerID idPower, void *)
{
    const float MAX_SAFE_TELEPORT_DISTANCE_SQUARED = 160000.0f;
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    ActorID idActorOwner = (ActorID)ptACD->idOwner;
    Actor * ptActor = ActorTryToGet(idActorOwner);
    if (!ptActor)
    {
        return;
    }
    Player * ptPlayer = PlayerGetByACD(ptACD->id);
    if (!ptPlayer)
    {
        return;
    }
    Power * ptPower = PowerTryToGet(idPower);
    WorldPlace placeTarget;
    ptPower->tTarget.GetPlace(placeTarget);
    WorldPlace playerPlace = ptACD->GetPlace();
    if (playerPlace.idSWorld != placeTarget.idSWorld)
    {
        return;
    }
    float flDistSq = WorldPlaceDistanceSquared(placeTarget, playerPlace);
    if (flDistSq > MAX_SAFE_TELEPORT_DISTANCE_SQUARED)
    {
        return;
    }
    SActorMovement * ptMove = ptActor->ptSActorMovement;
    ptMove->SnapToNewPlace(placeTarget, TRUE, FALSE, TRUE, 0);
    ptACD->PickupNearby();
}





















extern "C" BOOL sub_71007d50c0(SNO snoWorld);  // unnamed 2.7.7 function (0x71007d50c0)
extern "C" BOOL sub_71007d5160();  // unnamed 2.7.7 function (0x71007d5160)
// NON_MATCHING: 404 vs 412 bytes; the original compares snoWorld and the two realm SNOs from
// registers although their addresses go to SWorldFindBySNO, ours reloads them.
BOOL sDeathPortalDestinationCanCreateWorld(SNO snoPortal, BOOL * bWorldExists)
{
    if (bWorldExists)
    {
        *bWorldExists = FALSE;
    }
    if (snoPortal != SNO_NONE)
    {
        ScopedActorDefinitionLockR pdefActorPortal(snoPortal);
        SNO snoWorld = TagMapGetSNO(pdefActorPortal->hTagMap, TAG_ACTOR_GIZMO_PORTAL_SPECIFIED_WORLD);
        if (snoWorld != SNO_NONE)
        {
            if (!sub_71007d50c0(snoWorld))
            {
                SNO snoGreedRealm = GlobalSNOGet(GLOBALSNO_WORLD_GREED_REALM);
                SNO snoAncientRealm = GlobalSNOGet(GLOBALSNO_UNK_686);
                SWorldID idSWorldGreedRealm = SWorldFindBySNO(snoGreedRealm);
                SWorldID idSWorldAncientRealm = SWorldFindBySNO(snoAncientRealm);
                if ((snoWorld == snoGreedRealm && idSWorldAncientRealm != SWORLDID_INVALID) ||
                    (snoWorld == snoAncientRealm && idSWorldGreedRealm != SWORLDID_INVALID))
                {
                    if (bWorldExists) { *bWorldExists = TRUE; }
                    return FALSE;
                }
            }
            else if (sub_71007d5160())
            {
                if (bWorldExists) { *bWorldExists = TRUE; }
                return FALSE;
            }
            SWorldID idSWorld = SWorldFindBySNO(snoWorld);
            if (idSWorld != SWORLDID_INVALID)
            {
                if (bWorldExists) { *bWorldExists = TRUE; }
            }
            else
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}








static const int32 scgDeathPortalNumPortals = 4;

// NON_MATCHING: 676 vs 672 bytes; the weighted pick keeps a found flag (cset/csel) and the
// weight across the definition unlock in the original; register/stack assignment differs.
SNO sDeathPortalChoosePortal(const PowerID idPower)
{

    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        int32 nPortalCount = 0;

        for (int32 i = 0; i < scgDeathPortalNumPortals; i++)
        {
            BOOL bWorldExists = FALSE;
            SNO snoPortal = PowerContactTagGetSNO(idPower, TAG_POWER_DEATH_PORTAL_ACTOR_0 + i * 16);
            if (sDeathPortalDestinationCanCreateWorld(snoPortal, &bWorldExists))
            {
                nPortalCount++;
            }
        }

        if (nPortalCount > 0)
        {
            if (ptPower->snoPower == SNOFileLookupGUID(POWER_SNO, "p1_TreasureGoblin_OnDeathGreedPortal"))
            {
                // Name invented: the 2.7.7 table at 0x7100eb2420 has no symbol.
                const uint32 arTags[scgDeathPortalNumPortals] = {
                    TAG_POWER_DEATH_PORTAL_ACTOR_0,
                    TAG_POWER_DEATH_PORTAL_ACTOR_0 + 16,
                    TAG_POWER_DEATH_PORTAL_ACTOR_0 + 32,
                    TAG_POWER_DEATH_PORTAL_ACTOR_0 + 48,
                };
                float arWeights[scgDeathPortalNumPortals];
                float flTotalWeight = 0.0f;
                for (int32 i = 0; i < nPortalCount; i++)
                {
                    SNO snoPortal = PowerContactTagGetSNO(idPower, arTags[i]);
                    ScopedActorDefinitionLockR pdefActorPortal(snoPortal);
                    arWeights[i] = TagMapGetFloat(pdefActorPortal->hTagMap, TAG_ACTOR_POWER_INSTEAD_OF_DEATH_CHANCE);
                    flTotalWeight += arWeights[i];
                }
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng) * (flTotalWeight - EPSILON);
                int32 nChosen = 0;
                for (int32 i = 0; i < nPortalCount; i++)
                {
                    if (flRoll < arWeights[i])
                    {
                        nChosen = i;
                        break;
                    }
                    flRoll -= arWeights[i];
                }
                SNO snoPortal = PowerContactTagGetSNO(idPower, arTags[nChosen]);
                if (sDeathPortalDestinationCanCreateWorld(snoPortal, NULL))
                {
                    return snoPortal;
                }
            }
            else
            {
                for (int32 i = 0; i < scgDeathPortalNumPortals; i++)
                {
                    SNO snoPortal = PowerContactTagGetSNO(idPower, TAG_POWER_DEATH_PORTAL_ACTOR_0 + i * 16);
                    if (sDeathPortalDestinationCanCreateWorld(snoPortal, NULL))
                    {
                        return snoPortal;
                    }
                }
            }
        }
    }

    return SNO_NONE;
}
// No header declaration recovered; DWARF records only SWorld.cpp:4017.
void SWorldLimitToPlayersInGame(SWorld & tSWorld);
BOOL SpellEffect_DeathPortalCreate(
    const SNO snoActor,
    const WorldPlace tPlace,
    Power * ptPower)
{
    if (snoActor == SNO_NONE || !sDeathPortalDestinationCanCreateWorld(snoActor, NULL))
    {
        return FALSE;
    }

    BOOL bSuccess = FALSE;

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
    tActorAdd.tPlace = tPlace;
    tActorAdd.eTeam = TEAM_SCENERY;
    tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

    tActorAdd.tMarkersName = MarkerStringHash("DeathPortal");
    tActorAdd.snoActor = snoActor;

    {
        ActorID idActorPortal = GizmoAddBySNO(0, tActorAdd);
        Actor * ptActorPortal = ActorTryToGet(idActorPortal);
        if (ptActorPortal)
        {
            ActorCommonData & tACDPortal = ACDGet(ptActorPortal->idACD);
            SWorld * ptSWorld = SWorldTryToGet(tACDPortal.GetSWorldID());
            if (ptSWorld)
            {

                Vector3D vDirection = 5.0f * (cgwvXAxis + cgwvYAxis);

                WorldPlace placeReturnDest = tACDPortal.GetPlace();
                placeReturnDest.wp += vDirection;
                Target tTargetDest;
                tTargetDest.InitializeFromPlace(placeReturnDest);

                WorldPlace placeFinalDest;

                NavPlace tNavPlace;
                if (!PowerRollToTargetNavPlace(
                    tACDPortal.id, tTargetDest, PASSABILITY_PLAYER, FALSE, tNavPlace))
                {




                    placeFinalDest = tACDPortal.GetPlace();
                }
                else
                {
                    placeFinalDest = tNavPlace.tPlace;
                }

                ActorAddStruct tActorAddStartLocation;
                tActorAddStartLocation.snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_START_LOCATION);
                tActorAddStartLocation.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DONT_PERTURB);
                tActorAddStartLocation.tPlace = placeFinalDest;
                tActorAddStartLocation.eTeam = TEAM_SCENERY;
                tActorAddStartLocation.hGB.eType = GB_INVALID; tActorAddStartLocation.hGB.gbid = GBID_NONE;



                CRefString sReturnActorName("DeathPortalReturnLocation");
                SNO snoWorld = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_PORTAL_SPECIFIED_WORLD, tActorAdd.snoActor);
                sReturnActorName += SNOToString(WORLD_SNO, snoWorld, FALSE);

                tActorAddStartLocation.tMarkersName = MarkerStringHash(sReturnActorName.str());

                ActorID idActorStartLocation = GizmoAddBySNO(0, tActorAddStartLocation);
                Actor * ptActorStartLocation = ActorTryToGet(idActorStartLocation);

                if (ptActorStartLocation)
                {
                    ActorCommonData & tACDStartLocation = ACDGet(ptActorStartLocation->idACD);
                    EntranceLocation tReturnLocation;
                    tReturnLocation.idSScene = tACDStartLocation.GetWorldLocation().idSScene;
                    tReturnLocation.snoMarker = SNO_NONE;
                    tReturnLocation.tName = MarkerStringHash(sReturnActorName.str());

                    if (tReturnLocation.idSScene != SSCENEID_INVALID)
                    {













                        uint32 uReturnLocationGUID = SGameGlobalsGet().uNextDynamicEntranceGUID++;
                        tACDStartLocation.AttributesSetInt(ATTRIBUTE_DYNAMIC_ENTRANCE_GUID, uReturnLocationGUID);
                        ptSWorld->SetEntranceLocation(uReturnLocationGUID, tReturnLocation);

                        tACDPortal.AttributesSetInt(ATTRIBUTE_LINKED_DYNAMIC_ENTRANCE_GUID, uReturnLocationGUID);


                        if (snoWorld != SNO_NONE)
                        {
                            SWorldID idSWorld = SWorldCreate(snoWorld, FALSE, FALSE, NULL, ACDID_INVALID, tACDPortal.id, 0, WARP_UNKNOWN, FALSE, (1 << FLAG_SWORLD_FROM_DEATH_PORTAL));
                            SWorld * ptSWorldScoped = SWorldTryToGet(idSWorld);
                            if (ptSWorldScoped)
                            {


                                if (!ptPower || PowerContactTagGetBool(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_DEATH_PORTAL_LIMIT_TO_PLAYERS_IN_GAME, ptPower->dwCurrentContactFrame))
                                {
                                    SWorldLimitToPlayersInGame(*ptSWorldScoped);
                                }

                                if (ptPower)
                                {
                                    SNO snoWorldMonsterPower = PowerContactTagGetSNO(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_DEATH_PORTAL_WORLD_MONSTER_POWER, ptPower->dwCurrentContactFrame);
                                    if (snoWorldMonsterPower != SNO_NONE)
                                    {
                                        ptSWorldScoped->snoWorldMonsterPower = snoWorldMonsterPower;
                                    }

                                    SNO snoWorldPlayerPower = PowerContactTagGetSNO(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_DEATH_PORTAL_WORLD_PLAYER_POWER, ptPower->dwCurrentContactFrame);
                                    if (snoWorldPlayerPower != SNO_NONE)
                                    {
                                        ptSWorldScoped->snoWorldPlayerPower = snoWorldPlayerPower;
                                    }

                                    StringLabelHandle hLabelDescription = PowerContactTagGetInt(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_DEATH_PORTAL_WORLD_QUEST_DESCRIPTION, ptPower->dwCurrentContactFrame);
                                    if (hLabelDescription != -1)
                                    {
                                        ptSWorldScoped->hWorldQuestDescriptionLabel = hLabelDescription;
                                    }
                                    // Original also reads the description tag for the title.
                                    StringLabelHandle hLabelTitle = PowerContactTagGetInt(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_DEATH_PORTAL_WORLD_QUEST_DESCRIPTION, ptPower->dwCurrentContactFrame);
                                    if (hLabelTitle != -1)
                                    {
                                        ptSWorldScoped->hWorldQuestTitleLabel = hLabelTitle;
                                    }
                                }


                                bSuccess = TRUE;
                            }
                        }
                    }
                }








            }
        }
    }
    return bSuccess;

}


static void sSpellEffect_DeathPortalProc(const PowerID idPower, void *)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    // 
    SNO snoActor = sDeathPortalChoosePortal(idPower);
    // 
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    WorldPlace tPlace = ptACD ? ptACD->GetPlace() : ptPower->wpStartLocation;
    // 
    SpellEffect_DeathPortalCreate(snoActor, tPlace, ptPower);
}


static BOOL sSpellEffect_TryDeathPortalProcDelay(const PowerID idPower)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD || !ptACD->IsInWorld())
    {
        return FALSE;
    }
    if (ACDIsInTieredLootRun(*ptACD))
    {
        return FALSE;
    }
    // 
    // 
    SWorldID idSWorld = ptACD->GetWorldLocation().idSWorld;
    if (WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(idSWorld)))
    {
        return FALSE;
    }
    // 
    // 
    int32 nDelay = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_DEATH_PORTAL_DELAY) * 60.0f);
    if (nDelay <= 0)
    {
        sSpellEffect_DeathPortalProc(idPower, NULL);
    }
    else
    {
        // 
        PowerTriggerAddTimerTrigger(idPower, nDelay, sSpellEffect_DeathPortalProc, NULL);
    }
    // 
    return TRUE;
}


static void sSpellEffect_DeathPortalProcDelay_Generic(const PowerID idPower, void *)
{
    float flPortalChance = PowerGetFormulaValue(idPower, TAG_POWER_DEATH_PORTAL_CHANCE);
    if (flPortalChance < 0.999999f && RandFloat_Internal(SGameGlobalsGet().rng) > flPortalChance)
    {
        // 
        return;
    }
    // 
    // 
    sSpellEffect_TryDeathPortalProcDelay(idPower);
}


static void sSpellEffect_DeathPortalProcDelay_GreedRealm(const PowerID idPower, void *)
{
    if (!GameIsAdventureMode())
    {
        return;
    }
    if (GameGetMonsterLevel() < 70)
    {
        return;
    }
    // 
    BOOL bVisitedGreedRealm = TRUE;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        // 
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            // 
            // 
            // 
            int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
            if (nPlayerLevel >= 70 && !(ptPlayer->tAccount.uPartitionFlags & (1 << PARTITION_FLAG_GREED_REALM_VISITED)))
            {
                // 
                bVisitedGreedRealm = FALSE;
                break;
            }
        }
    }
    // 
    // 
    // 
    float flPortalChance = PowerGetFormulaValue(idPower, TAG_POWER_DEATH_PORTAL_CHANCE); if (!bVisitedGreedRealm) { flPortalChance = PowerGetFormulaValue(idPower, TAG_POWER_DEATH_PORTAL_CHANCE_SPECIAL); }
    // 
    // 
    // 
    // 
    // 
    // 
    if (flPortalChance < 0.999999f && RandFloat_Internal(SGameGlobalsGet().rng) > flPortalChance)
    {
        // 
        return;
    }
    // 
    if (sSpellEffect_TryDeathPortalProcDelay(idPower))
    {
        // 
        // 
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            // 
            ptPlayer->tAccount.uPartitionFlags |= (1 << PARTITION_FLAG_GREED_REALM_VISITED);
            // 
            // 
            ptPlayer->uConsoleSyncFlags |= (1 << PCSF_PARTITION_FLAGS);
        }
    }
}



static void sSpellEffect_PortalMonster_Burrow_In(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    if (ptACD->AttributesGetInt(ATTRIBUTE_POWER_SAVED_ATTRIBUTE))
    {
        PowerID idLifetimePower = PowerStart(ptACD->id, GlobalSNOGet(GLOBALSNO_POWERS_PORTALMONSTER_LIFETIME_BUFF), cgTargetNone);
    }
    else
    {
        sBurrowIn(idPower, 1, 68, FALSE);
    }
}








static void sSpellEffect_PortalMonster_Burrow_Out(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_PORTALMONSTER_BURROW_IN), ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
        PowerAPI_DoBurrowOut(ptACD, FALSE);
        PowerID idLifetimePower = PowerStart(ptACD->id, GlobalSNOGet(GLOBALSNO_POWERS_PORTALMONSTER_LIFETIME_BUFF), cgTargetNone);
        ptACD->AttributesSetInt(ATTRIBUTE_POWER_SAVED_ATTRIBUTE, TRUE);
    }
}


















static void sSpellEffect_PortalMonster_Death_Orientation(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        // 
        // 
        // 
        // 
        return;
    }
    Quaternion qRot;
    float flAngle = 0.78539819f;
    qRot.FromYaw(flAngle);

    Vector3D vTargetOffset = cgwvXAxis;
    vTargetOffset = qRot.Rotate(vTargetOffset);
    // 
    WorldPlace wpTarget = ptACD->GetPlace();
    wpTarget.wp += vTargetOffset;
    // 
    ptACD->SetFacing(wpTarget, TRUE);
}



static void sSpellEffect_ArmorScavenger_Burrow_In(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        // 
        return;
    }
    ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 2);
    // 
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = TRUE;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMUNE_TO_KNOCKBACK, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_STUN_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_BLIND_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FEAR_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CHARM_IMMUNE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_FREEZE_IMMUNE, TRUE);
    // 
    // 
    float flMoveSpeedReductionPct = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    if (flMoveSpeedReductionPct >= 0.999999f)
    {
        // 
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_IMMOBOLIZED, TRUE);
    }
    else
    {
        // 
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT, flMoveSpeedReductionPct);
    }
    // 
    // 
    tBuffStartData.nAppearanceParameter = 0x47;
    // 
    // 
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DODGE_CHANCE_BONUS, 1.0f);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_BURROWED, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_AI_IN_SPECIAL_STATE, TRUE);
    ptACD->EnableCollision(FALSE);
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}



static void sSpellEffect_ArmorScavenger_Burrow_Out(PowerID idPower, void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD)
    {
        ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 0);
        ptACD->EnableCollision(TRUE);
        BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_ARMORSCAVENGER_BURROW_IN), ptACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {
            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
        PowerID idArmorPower = PowerStart(ptACD->id, GlobalSNOGet(GLOBALSNO_POWERS_ARMORSCAVENGER_ARMOR_BUFF), cgTargetNone);
    }
}













void sSpellEffect_ArmorScavenger_Pre_Burrow_Complete(PowerAPIBuffInstance * ptBuff, BuffRemovalReason eRemovalReason, void *)
{
    if (eRemovalReason == BUFF_REMOVAL_REASON_EXPIRED)
    {
        Power * ptPower = PowerTryToGet(ptBuff->idPower);
        ActorCommonData * ptACD;
        if (ptPower && (ptACD = PowerGetOwner(ptBuff->idPower)) && ptACD->IsAlive())
        {
            PowerInterrupt(ptBuff->idPower, TRUE);
            PowerStart(ptACD->id, GlobalSNOGet(GLOBALSNO_POWERS_ARMORSCAVENGER_BURROW_IN), cgTargetNone);
            AIStateSetState((ActorID)ptACD->idOwner, 4, FALSE);
        }
    }
}





















static void sSpellEffect_ArmorScavenger_Pre_Burrow_Start(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }
    ptPower->dwFlags |= (1 << PF_MANUALLY_FINISH);
    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (ptMove)
    {
        ptMove->StopNow();
    }
    AnimPlayerPlayAnimation(*ptACD, TAG_ACTOR_ANIMATION_SPAWN_4, 0xfffffffe, TRUE, ANIM_REASON_POWER, 0.0f, FALSE, 1.0f);
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.pRemovalEffect = sSpellEffect_ArmorScavenger_Pre_Burrow_Complete;
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1) * 60.0f);
    tBuffStartData.dwDurationDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2) * 60.0f);
    tBuffStartData.nAppearanceParameter = 70;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}














static void sSpellEffect_ArmorScavenger_Pre_Burrow_Interrupted(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = PowerGetOwner(idPower);
        if (ptACD)
        {
            BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_ARMORSCAVENGER_PRE_BURROW), ptACD, ACDID_INVALID);
            if (idBuff != BUFFID_INVALID)
            {
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
    }
}









static void sSpellEffect_RockFodder_ChargeTeleportOwner(Power * ptPower, ActorCommonData * ptACDOwner, const WorldPlace & tPlaceDesired, Angle aFacing, const WorldPlace * tPlaceRolloutSource)
{
    // 
    // 
    // 
    BuffID idBuff = PowerAPI_BuffFindOnACD(GlobalSNOGet(GLOBALSNO_POWERS_ROCKFODDER_CHARGE), ptACDOwner, ACDID_INVALID);
    if (idBuff == BUFFID_INVALID)
    {
        // 
        // 
        return;
    }
    // 
    // 
    PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
    // 
    // 
    ActorID idActorOwner = ptACDOwner ? (ActorID)ptACDOwner->idOwner : ACTORID_INVALID;
    Actor * ptActorOwner = ActorTryToGet(idActorOwner);
    if (!ptActorOwner)
    {
        // 
        return;
    }
    SActorMovement * ptMoveOwner = ptActorOwner->ptSActorMovement;
    // 
    // 
    WorldPlace tPlaceWarp = tPlaceDesired;
    if (tPlaceRolloutSource)
    {
        PowerComputeDestination(ptPower->snoPower, *ptMoveOwner, tPlaceDesired, *tPlaceRolloutSource, tPlaceWarp, 0, 10.0f);
    }
    // 
    // 
    ptMoveOwner->SnapToNewPlace(tPlaceWarp, TRUE, FALSE, TRUE, 0);
    ptMoveOwner->SetNewFacing(aFacing, TRUE, TRUE);
    ptACDOwner->PickupNearby();
    // 
    // 
    PowerInterrupt(ptPower->id, TRUE);
}


static void sSpellEffect_RockFodder_ChargeUpdate(PowerAPIBuffInstance * ptBuff, void *)
{


    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACDProjectile = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptACDProjectile)
    {
        return;
    }


    ActorCommonData * ptACDTarget = ACDTryToGet(PowerGetTargetACD(ptPower));
    if (ptACDTarget && ptACDTarget->IsAlive())
    {
        WorldPlace tPlaceTarget = ptACDTarget->GetPlace();
        WorldPlace tPlaceProjectile = ptACDProjectile->GetPlace();
        if (tPlaceTarget.idSWorld == tPlaceProjectile.idSWorld)
        {
            ProjectileSeekActor(ptBuff->idACDAffected, ptACDTarget->id, 360.0f, TRUE);


            WorldDistance wdToTarget = WorldPlaceDistance(tPlaceTarget, tPlaceProjectile);

            float flProjectileSpeed = PowerAttributesGetFloat(ptBuff->idPower, ATTRIBUTE_PROJECTILE_SPEED);
            float flTimeToTarget = wdToTarget / flProjectileSpeed;

            ProjectileSetDetonationTime(ptBuff->idACDAffected, (uint32)flTimeToTarget + 3);
        }
    }
}


static void sSpellEffect_RockFodder_ChargeRemoval(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (!ptPower)
    {
        // 
        // 
        return;
    }
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    ActorCommonData * ptACDProjectile = ACDTryToGet(ptBuff->idACDAffected);
    if (ptACDOwner && ptACDProjectile)
    {
        // 
        // 
        // 
        // 
        // 
        sSpellEffect_RockFodder_ChargeTeleportOwner(ptPower, ptACDOwner, ptACDProjectile->GetPlace(), ptACDProjectile->GetOrientation().ToYaw(), NULL);
    }
}



void sSpellEffect_RockFodder_Charge(PowerID idPower, void *)
{



    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }


    NavPlace tNavPlaceSource;
    PowerGetACDPoint(idPower, ptACDOwner->id, tNavPlaceSource, -1, -1, NULL);

    Target tTarget;
    PowerGetTarget(idPower, tTarget);


    PowerAPICreateProjectile tProjectileInfo;
    PowerHelper_ProjectileInit(idPower, ptACDOwner->id, tNavPlaceSource, tTarget, SPELL_ROCKFODDER_CHARGE_IMPACT, tProjectileInfo);
    tProjectileInfo.tProjectileData.dwFlags |= (1 << PROJECTILE_HUGS_GROUND_BIT);
    tProjectileInfo.tProjectileData.dwFlags |= (1 << PROJECTILE_DONT_PERFORM_MARBLE_ROLL_TO_CASTER_BIT);

    ACDID idProjectile = PowerAPI_DoProjectile(tProjectileInfo);
    if (idProjectile == ACDID_INVALID)
    {
        return;
    }
    ActorCommonData & tACDProjectile = ACDGet(idProjectile);


    WorldPlace tPlaceTarget;
    tTarget.GetPlace(tPlaceTarget);
    WorldPlace tPlaceProjectile = tACDProjectile.GetPlace();
    WorldDistance wdToTarget = WorldPlaceDistance(tPlaceTarget, tPlaceProjectile);

    float flProjectileSpeed = PowerAttributesGetFloat(idPower, ATTRIBUTE_PROJECTILE_SPEED);
    float flTimeToTarget = wdToTarget / flProjectileSpeed;

    ProjectileSetDetonationTime(idProjectile, (uint32)flTimeToTarget + 3);


    PowerAPIBuffStartData tProjectileBuff(idPower);
    tProjectileBuff.bInfinite = TRUE;
    tProjectileBuff.pPeriodicSpellEffect = sSpellEffect_RockFodder_ChargeUpdate;
    tProjectileBuff.pRemovalEffect = sSpellEffect_RockFodder_ChargeRemoval;
    tProjectileBuff.dwRefreshRate = 6;
    PowerAPI_BuffAddToACD(tProjectileBuff, ACDGet(idProjectile));


    PowerAPIBuffStartData tOwnerBuff(idPower);
    tOwnerBuff.bInfinite = TRUE;
    FastAttribSetValueInt(tOwnerBuff.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tOwnerBuff.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tOwnerBuff.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);
    PowerAPI_BuffAddToACD(tOwnerBuff, *ptACDOwner);


    ptPower->dwFlags &= ~(1 << PF_INTERRUPTABLE);
    ptPower->dwFlags |= (1 << PF_MANUALLY_FINISH);
}



void sSpellEffect_RockFodder_ChargeImpact(PowerID idPower, void * pData)
{


    ImpactData * pImpactData = (ImpactData *)pData;


    ActorCommonData * ptACDHit = ACDTryToGet(pImpactData->idACDIntersected);
    if (ptACDHit && !ptACDHit->IsStealthed())
    {
        PowerAPIAttackPayload tPayload;
        PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
        PowerAPI_DoPayload(tPayload, ptACDHit->id, ACDID_INVALID);


        Power * ptPower = PowerTryToGet(idPower);
        ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
        ActorCommonData * ptACDProjectile = ACDTryToGet(pImpactData->idACDProjectile);

        if (ptPower && ptACDOwner && ptACDProjectile)
        {
            WorldPlace tPlaceProjectile = ptACDProjectile->GetPlace();
            sSpellEffect_RockFodder_ChargeTeleportOwner(ptPower, ptACDOwner, ptACDHit->GetPlace(), ptACDProjectile->GetOrientation().ToYaw(), &tPlaceProjectile);
        }
    }


    PowerAPIDestroyProjectile tProjectile;
    PowerHelper_StopProjectileInit(idPower, pImpactData->idACDProjectile, pImpactData->idACDIntersected, tProjectile);
    PowerAPI_DestroyProjectile(tProjectile);
}



void sSpellEffect_NightScreamer_CheapAttack(PowerID idPower, void *)
{



    Target tTarget;
    PowerGetTarget(idPower, tTarget);
    ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.GetACD());
    if (!ptACDTarget)
    {
        return;
    }


    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    tPayload.eDeliveryMechanism = PDM_MELEE;
    tPayload.eAttackDirection = ATTACKDIRECTION_STRAIGHT;
    tPayload.bIgnoreDamageBonusAndReduction = TRUE;



    float flDamageTotal = 0.0f;
    for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++)
    {
        flDamageTotal += tPayload.arCombatDamage[i].flMin;
        tPayload.arCombatDamage[i].flDelta = 0.0f;
    }

    float flHalfRemainingHealth = ptACDTarget->GetHitPointsCurrent() * 0.5f;

    for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++)
    {
        tPayload.arCombatDamage[i].flMin = flHalfRemainingHealth * (tPayload.arCombatDamage[i].flMin / flDamageTotal);
    }


    PowerAPI_DoPayload(tPayload, tTarget.GetACD(), ACDID_INVALID);


    PowerAPIBuffStartData tStunBuff(idPower);
    tStunBuff.dwDurationMin = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0) * 60.0f);
    tStunBuff.dwDurationDelta = (uint32)(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1) * 60.0f);
    FastAttribSetValueInt(tStunBuff.idFastAttrib, ATTRIBUTE_STUNNED, TRUE);
    PowerAPI_BuffAddToACD(tStunBuff, *ptACDTarget);
}



void sSpellEffect_NightScreamer_AllyBuff(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    SNO snoEffect = PowerGetEffect(idPower, TAG_POWER_CASTING_EFFECT_GROUP);
    if (snoEffect != SNO_NONE)
    {
        ACDPlayEffect(ptACD->id, EFFECT_PLAY_EFFECT_GROUP, snoEffect, ACDID_INVALID);
    }
    // 
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.nBuffIndex = 0;
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.eStackType = BUFF_STACKTYPE_REFRESH;
    tBuffStartData.nAppearanceParameter = 69;
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1) * 60.0f);
    tBuffStartData.dwDurationDelta = 0;
    // 
    // 
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_SCALAR_UNCAPPED_BONUS, PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2));
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_ATTACKS_PER_SECOND_BONUS, PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3));
    // 
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundSourceFromPayload(idPower, tChooseTargets);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && idACDTarget != ptACD->id)
        {
            // 
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACDTarget);
        }
    }
    // 
}









const int32 scnWraithSwerveAnimTagsCount = 2;
static const uint32 sctWraithSwerveAnimTags[scnWraithSwerveAnimTagsCount] = { TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_49, TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_50 };
void sSpellEffect_Wraith_Swerve_Advance(PowerID idPower, void *)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    Target tTarget;
    PowerGetTarget(idPower, tTarget);
    ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.GetACD());
    if (!ptACDTarget)
    {
        return;
    }
    // 
    // 
    int32 nSwerveProgress = PowerGetStoredInt(ptPower, "nSwerveProgress");
    if (nSwerveProgress >= scnWraithSwerveAnimTagsCount)
    {
        return;
    }
    // 
    // 
    uint32 dwSwerveAnimTag = sctWraithSwerveAnimTags[nSwerveProgress];
    PowerSetStoredInt(ptPower, "nSwerveProgress", nSwerveProgress + 1);
    // 
    // 
    if (AnimPlayerAnimExists(*ptACD, dwSwerveAnimTag))
    {
        // 
        // 
        // 
        AnimationTiming tAnimationTiming;
        AnimPlayerGetAnimationTiming(*ptACD, dwSwerveAnimTag, ANIM_USE_ANIM_LENGTH, 1.0f, tAnimationTiming);
        uint32 dwDuration = tAnimationTiming.tAnimSpec[ANIMOFFSET_CONTINUE].dwDuration;
        // 
        // 
        // 
        PreplayedAnimData tPreplayedAnimData;
        tPreplayedAnimData.dwServerTimeAnimStarted = GameGetServerTime();
        tPreplayedAnimData.dwSyncedSeed = 0;
        tPreplayedAnimData.dwPrePlayedAnimFlags = 0;
        // 
        // 
        PowerTriggerPlayTriggerAnimation(idPower, dwSwerveAnimTag, dwDuration, 1.0f, TRUE, tPreplayedAnimData, ANIM_REASON_POWER, CONTACT_FRAME_NORMAL, TRUE);
    }
}


void sSpellEffect_Wraith_Swerve_Projectile(PowerID idPower, void * pData)
{
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    // 
    // 
    int32 nSwerveProgress = PowerGetStoredInt(ptPower, "nSwerveProgress");
    if (nSwerveProgress >= scnWraithSwerveAnimTagsCount)
    {
        return;
    }
    // 
    // 
    // 
    SpellEffect_Projectile(idPower, pData);
}



void sSpellEffect_Wraith_Swerve_ProjectileImpact(PowerID idPower, void * pData)
{
    ImpactData * pImpactData = (ImpactData *)pData;
    ActorCommonData * ptACDHit = ACDTryToGet(pImpactData->idACDIntersected);
    if (!ptACDHit || ptACDHit->IsStealthed())
    {
        PowerAPIDestroyProjectile tProjectile;
        PowerHelper_StopProjectileInit(idPower, pImpactData->idACDProjectile, pImpactData->idACDIntersected, tProjectile);
        PowerAPI_DestroyProjectile(tProjectile);
        return;
    }
    // 
    // 
    PowerAPIExplosion tExplosion;
    PowerHelper_ExplosionInit(idPower, ptACDHit->GetPlace(), tExplosion);
    PowerAPI_DoExplosion(tExplosion);
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, ptACDHit->id, ACDID_INVALID);
}



const int32 SPAWN_CAGE_ELEMENTS = 5;
const float SPAWN_CAGE_ANGLE_INC = FULL_CIRCLE / SPAWN_CAGE_ELEMENTS;
const float SPAWN_CAGE_SPAWN_OFFSET = 1.0f;
const int32 SPAWN_CAGE_LIFE_DURATION_TICKS = 240;
void sSpellEffect_Rockworm_Spawn_Cage(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }

    ACDID idACDTarget = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget || !ptACDTarget->CanBeImmobilized())
    {
        return;
    }

    Actor * ptActorTarget = ActorTryToGet((ActorID)ACDGetOwner(ptACDTarget->id));
    if (!ptActorTarget)
    {
        return;
    }

    float flRadius = ptActorTarget->ptSActorMovement->GetRadius(TRUE);

    WorldPlace placeTarget = ptACDTarget->GetPlace();
    SNO snoMonsterActor = MonsterGetSummonActor(ptACD->tSNOName, 0);
    float flCombinedRadius = flRadius + SPAWN_CAGE_SPAWN_OFFSET;
    float flAngle = 0.0f;
    for (int32 i = 0; i < SPAWN_CAGE_ELEMENTS; i++)
    {
        PowerAPISummonActor tSummonInfo;
        BOOL bSuccess = PowerHelper_SummonActorInit(idPower, snoMonsterActor, ACDID_INVALID, tSummonInfo, 0);
        if (bSuccess)
        {
            Vector3D vDir(Math::Cos(flAngle), Math::Sin(flAngle), 0.0f);
            vDir *= flCombinedRadius;
            WorldPlace placeSpawn(placeTarget);
            placeSpawn.wp += vDir;
            Target tTargetSpawn;
            tTargetSpawn.InitializeFromPlace(placeSpawn);

            NavPlace tNavPlace;
            PowerRollToTargetNavPlace(
                ptACDTarget->id, tTargetSpawn, PASSABILITY_MONSTER, FALSE, tNavPlace);





            tSummonInfo.nActorLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
            tSummonInfo.placeSummonLocation = tNavPlace.tPlace;
            tSummonInfo.dwAddFlags |= (1 << ACTORADD_DONT_PERTURB);
            tSummonInfo.dwAddFlags |= (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);

            ACDID idACDSummoned = PowerAPI_SummonActor(tSummonInfo);
            if (idACDSummoned != ACDID_INVALID)
            {
                ActorID idActorSummoned = (ActorID)ACDGetOwner(idACDSummoned);
                ActorDeleteSoon(idActorSummoned, SPAWN_CAGE_LIFE_DURATION_TICKS);
            }
        }

        flAngle += SPAWN_CAGE_ANGLE_INC;
    }

    ActorSnapToPosition((ActorID)ACDGetOwner(ptACDTarget->id), placeTarget, TRUE, TRUE);
}



void sSpellEffect_Rockworm_BurrowAndTeleport(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    sBurrowIn(idPower, 1, -1, TRUE);

    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
    Actor * ptActor = ActorTryToGet(idActor);
    ACDID idACDTarget = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptActor || !ptACDTarget || !ptActor->ptSActorMovement)
    {
        return;
    }

    WorldPlace placeCurr = ptACD->GetPlace();
    WorldPlace placeTarget = ptACDTarget->GetPlace();
    WorldPlace placeResult;

    PowerComputeDestination(ptPower->snoPower, *ptActor->ptSActorMovement, placeTarget, placeCurr, placeResult, FALSE, 10.0f);






    Vector3D vToTarget = placeTarget.wp - placeCurr.wp;
    if (vToTarget.MagnitudeSquared() > EPSILON * EPSILON)
    {
        vToTarget.Normalize();
        const float DESIRED_ROCKWORM_PREDICTED_TARGET_DISTANCE = 28.0f;
        WorldPlace placeForwardTarget(placeResult);
        placeForwardTarget.wp += vToTarget * DESIRED_ROCKWORM_PREDICTED_TARGET_DISTANCE;

        PowerComputeDestination(ptPower->snoPower, *ptActor->ptSActorMovement, placeForwardTarget, placeTarget, placeResult, FALSE, 10.0f);
    }






    ActorSnapToPosition(idActor, placeResult, TRUE, TRUE);
}



void sSpellEffect_LeaperAngel_PreventNearbyLeaper(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    // 
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    // 
    TargetCriteria tStoredTargetCriteria = ptPower->tTargetCriteria;
    // 
    ptPower->tTargetCriteria.dwTeamRelationshipFlags = (1 << TCTR_AFFECTS_ALLIES) | (1 << TCTR_IGNORES_WRECKABLES);
    ptPower->tTargetCriteria.eLifeStatus = TCLS_ALIVE_ONLY;
    // 
    // 
    // 
    PowerAPIChooseTargets tChooseTargets;
    float flRadius = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    PowerHelper_ChooseTargetsInitGeometrySphere(ptACD->GetPlace(), flRadius, tChooseTargets.tGeometry);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.nBuffIndex = 0;
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1) * 60.0f);
    tBuffStartData.dwDurationDelta = 0;
    // 
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        ACDID idACD = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDScoped = ACDTryToGet(idACD);
        if (ptACDScoped)
        {
            // 
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACDScoped);
        }
    }
    // 
    // 
    ptPower->tTargetCriteria = tStoredTargetCriteria;
}



void sSpellEffect_LeaperAngel_FireBladesBuff_OnRemoval(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void *)
{
    if (ptBuff)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(ptBuff->idPower);
        if (ptACDOwner)
        {
            ACDPlayEffect(ptACDOwner->id, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_LEAPERANGEL_FIREBLADES_END), ACDID_INVALID);
            ptACDOwner->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 0);
        }
    }
}








void sSpellEffect_LeaperAngel_FireBladesBuff_OnDoDamage(PowerAPIBuffInstance *, PowerAPIDamage &, float * flDamageDone, ACDID, ACDID)
{
    float flTotalDamageDone = 0.0f;
    for (int32 i = 0; i < NUM_DAMAGE_TYPES; i++)
    {
        flTotalDamageDone += flDamageDone[i];
        flDamageDone[i] = 0.0f;
    }
    flDamageDone[DAMAGE_TYPE_FIRE] = flTotalDamageDone;
}







void sSpellEffect_LeaperAngel_FireBladesBuff(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (!ptACD)
    {
        return;
    }
    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.nBuffIndex = 1;
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2) * 60.0f);
    tBuffStartData.dwDurationDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3) * 60.0f);
    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4));
    tBuffStartData.pOnDoDamageEffect = sSpellEffect_LeaperAngel_FireBladesBuff_OnDoDamage;
    tBuffStartData.pRemovalEffect = sSpellEffect_LeaperAngel_FireBladesBuff_OnRemoval;
    tBuffStartData.nAppearanceParameter = 72;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
    ptACD->AttributesSetInt(ATTRIBUTE_CURRENT_WEAPONCLASS, 1);
}


ACDID ActivateMarkerName(LPCSTR szMarkerName);  // X/Common/InactiveMarker.cpp:1956






static void sSpellEffect_Teleport_Near_Target(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }

    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (!ptMove)
    {
        return;
    }

    if (!ptPower->tTarget.IsNone())
    {
        WorldPlace placeCurrent, placeTarget;
        ptMove->GetPlace(placeCurrent);
        ptPower->tTarget.GetPlace(placeTarget);

        float flOffsetMin = PowerGetFormulaValue(idPower, TAG_POWER_TELEPORT_NEAR_TARGET_DIST_MIN);
        float flOffsetDelta = PowerGetFormulaValue(idPower, TAG_POWER_TELEPORT_NEAR_TARGET_DIST_DELTA);

        float flRandOffset = RandRangeFloat_Internal(SGameGlobalsGet().rng, flOffsetMin, flOffsetDelta);
        float flRandYaw = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, FULL_CIRCLE);

        Quaternion qYaw;
        qYaw.FromYaw(flRandYaw);

        Vector3D vecOffset(flRandOffset, 0.0f, 0.0f);
        vecOffset = qYaw.Rotate(vecOffset);

        WorldPlace placeDest(placeTarget);
        placeDest.wp += vecOffset;
        PowerComputeDestination(ptPower->snoPower, *ptMove, placeDest, placeTarget, placeDest, FALSE, 10.0f);

        ptMove->SnapToNewPlace(placeDest, TRUE, FALSE, TRUE, 0);
        ptMove->SetNewFacing((placeTarget.wp - placeDest.wp).As2D().ToYaw(), TRUE, TRUE);
    }
    else
    {
        PowerForceStop(idPower, TRUE);
    }
}




static void sSpellEffect_Teleport_To_Target(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }
    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (!ptMove)
    {
        return;
    }
    if (!ptPower->tTarget.IsNone())
    {
        // 
        WorldPlace placeTarget;
        ptPower->tTarget.GetPlace(placeTarget);
        // 
        WorldPlace placeDest = placeTarget;
        if (!PowerComputeDestination(ptPower->snoPower, *ptMove, placeTarget, placeTarget, placeDest, 0, 10.0f))
        {
            // 
            PowerForceStop(idPower, TRUE);
        }
        else
        {
            // 
            // 
            ptMove->SnapToNewPlace(placeDest, TRUE, FALSE, TRUE, 0);
        }
    }
    else
    {
        // 
        PowerForceStop(idPower, TRUE);
    }
}



static void sSpellEffect_Adria_Teleport_AOE(PowerID idPower, void *)
{
    // 
    // 
    // 
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower || ptPower->idACD == ACDID_INVALID)
    {
        return;
    }
    // 
    // 
    // 
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }
    // 
    // 
    // 
    Target tTarget;
    tTarget.InitializeFromACD(ptACD->id, TRUE, FALSE);
    // 
    PowerAPIChooseTargets tChooseTargets;
    PowerHelper_ChooseTargetsInitAroundPlaceFromPayload(idPower, tTarget, tChooseTargets);
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPI_DoChooseTargets(tChooseTargets, listTargets);
    // 
    // 
    // 
    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    // 
    // 
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}

static void sSpellEffect_Adria_Teleport_To_Cauldron_Activate(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }

    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (!ptMove)
    {
        return;
    }

    ACDID idACDMarkerLocation = ACDID_INVALID;
    uint32 tNameToFind = MarkerStringHash("x1_Adria_Cauldron_Activate_Loc");

    ActorIterator tIt;
    while (ActorDataArrayIterate(tIt))
    {
        const Actor * ptActor = tIt.ptElement;

        if (ptActor->tName == tNameToFind)
        {
            idACDMarkerLocation = ptActor->idACD;
            break;
        }
    }

    if (idACDMarkerLocation == ACDID_INVALID)
    {
        idACDMarkerLocation = ActivateMarkerName("x1_Adria_Cauldron_Activate_Loc");
    }

    ActorCommonData * ptACDMarkerLocation = ACDTryToGet(idACDMarkerLocation);
    if (!ptACDMarkerLocation)
    {
        return;
    }

    WorldPlace wpCurrent = ptACD->GetPlace();
    WorldPlace wpMarker = ptACDMarkerLocation->GetPlace();
    if (wpCurrent.idSWorld != wpMarker.idSWorld)
    {
        return;
    }

    ptMove->SnapToNewPlace(wpMarker, TRUE, FALSE, TRUE, 0);
    ptMove->SetNewFacing(ptACDMarkerLocation->GetOrientation().ToYaw(), TRUE, TRUE);
}



static void sSpellEffect_Generic_Legendary_Potion_Start(PowerID idPower, void * idACDItem)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }

    ActorCommonData * ptACDItem = ACDTryToGet((ACDID)(intptr)idACDItem);
    if (!ptACDItem)
    {
        return;
    }
    float flBuffDuration = ptACDItem->AttributesGetFloat(ATTRIBUTE_POTION_BONUS_BUFF_DURATION);
    uint32 dwTicksBuffDuration = Math::FloatToInt(flBuffDuration * 60.0f);

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.dwDurationMin = dwTicksBuffDuration;


    float flBonusArmorPercent = ptACDItem->AttributesGetFloat(ATTRIBUTE_POTION_BONUS_ARMOR_PERCENT);
    float flBonusResistAll = ptACDItem->AttributesGetFloat(ATTRIBUTE_POTION_BONUS_RESIST_ALL);
    float flBonusHealing = ptACDItem->AttributesGetFloat(ATTRIBUTE_POTION_BONUS_HITPOINTS_GRANTED);
    float flBonusLifeOnHit = ptACDItem->AttributesGetFloat(ATTRIBUTE_POTION_BONUS_LIFE_ON_HIT);
    float flBonusLifeOnKill = ptACDItem->AttributesGetFloat(ATTRIBUTE_POTION_BONUS_LIFE_ON_KILL);
    if (flBonusArmorPercent > EPSILON)
    {
        tBuffStartData.nBuffIndex = 0;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_ARMOR_BONUS_PERCENT, flBonusArmorPercent);
    }
    else if (flBonusResistAll > EPSILON)
    {
        tBuffStartData.nBuffIndex = 1;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_RESIST_ALL, flBonusResistAll);
    }
    else if (flBonusHealing > EPSILON)
    {
        tBuffStartData.nBuffIndex = 2;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND, flBonusHealing / flBuffDuration);
    }
    else if (flBonusLifeOnHit != 0.0f)
    {
        tBuffStartData.nBuffIndex = 3;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_ON_HIT, flBonusLifeOnHit);
    }
    else if (flBonusLifeOnKill != 0.0f)
    {
        tBuffStartData.nBuffIndex = 4;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_ON_KILL, flBonusLifeOnKill);
    }

    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}



static void sBreakWalls_PeriodicEffect(PowerAPIBuffInstance * ptBuff, void *)
{
    ActorCommonData * ptACDOwner = ACDTryToGet(ptBuff->idACDAffected);
    if (!ptACDOwner)
    {
        return;
    }
    // 
    SNO snoWallActor = GlobalSNOGet(GLOBALSNO_MONSTER_WALLER_MODEL);
    // 
    // 
    WorldPlace placeACD = ptACDOwner->GetPlace();
    Angle aYaw = ACDGetYaw(ptACDOwner->id);
    Vector3D vFacing(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
    // 
    Capsule wCapsule;
    wCapsule.wp1 = placeACD.wp;
    wCapsule.wp2 = placeACD.wp + vFacing * 6.0f;
    wCapsule.flRadius = 4.0f;
    // 
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectCapsule(placeACD.idSWorld, wCapsule, listTargets, FALSE);
    // 
    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        // 
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        // 
        if (tACDTarget.snoActor == snoWallActor)
        {
            // 
            ActorID idActor = (ActorID)tACDTarget.idOwner;
            BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
        }
    }
}

static void sSpellEffect_Legendary_Potion_Break_Walls(PowerID idPower, void * idACDItem)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }
    ActorCommonData * ptACDItem = ACDTryToGet((ACDID)(intptr)idACDItem);
    if (!ptACDItem)
    {
        return;
    }
    // 
    // 
    // 
    float flBuffDuration = ptACDItem->AttributesGetFloat(ATTRIBUTE_POTION_BONUS_BUFF_DURATION);
    uint32 dwTicksBuffDuration = Math::FloatToInt(flBuffDuration * 60.0f);
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    // 
    // 
    // 
    // 
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.bHitsOnFirstTick = TRUE;
    tBuffStartData.dwDurationMin = dwTicksBuffDuration;
    tBuffStartData.dwRefreshRate = 15;
    tBuffStartData.pPeriodicSpellEffect = sBreakWalls_PeriodicEffect;
    tBuffStartData.bPositiveEffect = TRUE;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_DESTROYS_WALLER_WALLS, TRUE);
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}

static void sSpellEffect_Generic_Break_Walls(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }
    ActorCommonData * ptACDTarget = ACDTryToGet(ptPower->tTarget.idACDTarget);
    if (!ptACDTarget)
    {
        return;
    }
    // 
    // 
    // 
    float flBuffDuration = PowerGetFormulaValue(idPower, TAG_POWER_BUFF_DURATION_MIN);
    uint32 dwTicksBuffDuration = Math::FloatToInt(flBuffDuration * 60.0f);
    // 
    PowerAPIBuffStartData tBuffStartData(idPower);
    // 
    // 
    // 
    // 
    tBuffStartData.bInfinite = FALSE;
    tBuffStartData.bHitsOnFirstTick = TRUE;
    tBuffStartData.dwDurationMin = dwTicksBuffDuration;
    tBuffStartData.dwRefreshRate = 6;
    tBuffStartData.pPeriodicSpellEffect = sBreakWalls_PeriodicEffect;
    tBuffStartData.bPositiveEffect = TRUE;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_MOVEMENT_DESTROYS_WALLER_WALLS, TRUE);
    // 
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDTarget);
}



static void sSpellEffect_Legendary_Detonate_DOT_Buffs(PowerID idPower, void *)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        return;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (!ptACD)
    {
        return;
    }

    if (ptPower->tTarget.IsInvalidForSource(ptACD))
    {
        return;
    }

    ACDID idACDTarget = ptPower->tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    uint32 dwNow = GameGetServerTime();




    POS pos = tACDTarget.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACDTarget.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);


        if (ptBuff && !ptBuff->bDeleted && ptBuff->idACDSource == ptACD->id && ptBuff->flDamagePerSecond > EPSILON)
        {

            float flSecondsToConsume = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
            int32 nTicksLeft = (int32)(flSecondsToConsume * 60.0f);
            if (nTicksLeft <= 0)
            {
                nTicksLeft = 1800;
            }

            if (ptBuff->dwDuration != 0)
            {

                uint32 dwTimeElapsed = dwNow - ptBuff->dwStartTick;
                if (dwTimeElapsed >= ptBuff->dwDuration)
                {
                    continue;
                }

                nTicksLeft = Math::Min<int>(nTicksLeft, ptBuff->dwDuration - dwTimeElapsed);
            }





            ptBuff->uLastDOTTick -= nTicksLeft;
            PowerAPI_BuffDealDOTDamage(*ptBuff, 0);



            if (ptBuff->dwDuration != 0 && !ptBuff->bDeleted)
            {
                PowerAPI_BuffModifyDuration(idBuff, -nTicksLeft, FALSE, TRUE);
            }
        }
    }
}

// NX64NFP.cpp:35,36,38; no recovered header declarations.
namespace Console { namespace NFP { extern uint32 s_nNFPDefaultCooldownMinutes; extern uint32 s_nNFPGoblinCooldownMinutes; extern uint32 s_nNFPDefaultDemonSpawnDelay; } }
static void sSpellEffect_AmiiboPortalScan_Begin(
    PowerID idPower,
    void * pData)
{


    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    if (!ptACDPlayer)
        return;



    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
        return;



    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);

    ACDLookAtMessage tMessage;
    tMessage.annSrc = ANNFromACD(ptACDPlayer->id);
    tMessage.annTarget = ANNFromACD((ACDID)(intptr_t)pData);
    MessageSendToClient(listDestPlayers, SMSG_AMIIBO_SCAN_BEGIN, &tMessage, sizeof(tMessage));
}







static void sSpellEffect_AmiiboPortalScan_End(
    PowerAPIBuffInstance * ptBuff,
    BuffRemovalReason eBuffRemovalReason,
    void * pData)
{



    if (!ptBuff)
        return;



    ActorCommonData * ptACDPlayer = PowerGetOwner(ptBuff->idPower);
    if (!ptACDPlayer)
        return;



    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
        return;



    ActorID idPortal = (ActorID)(intptr_t)pData;
    Actor * ptPortal = ActorTryToGet(idPortal);

    if (ptPortal && ptPortal->ptAI && !ActorFlagIsSet(idPortal, ACTOR_DEACTIVATING))
    {

        BrainAPIDoDeath(idPortal, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    }






    if (eBuffRemovalReason != BUFF_REMOVAL_REASON_REPLACED)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);

        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(ptACDPlayer->id);
        MessageSendToClient(listDestPlayers, SMSG_AMIIBO_SCAN_END, &tMessage, sizeof(tMessage));
    }
}








static void sSpellEffect_AmiiboPortal_Begin(
    PowerID idPower,
    void * pData)
{

    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);

    if (!ptACDPlayer || !ptACDPlayer->GetHasNFPDevice())
        return;




    NavPlace tNavPlace;
    tNavPlace.tPlace.wp = ptACDPlayer->tLocation.tWorldLocation.transform.wp;
    tNavPlace.tPlace.idSWorld = ptACDPlayer->GetPlace().idSWorld;
    PowerPlaceToNavMeshHandle(tNavPlace.tPlace, tNavPlace.tNavMeshHandle);
    Quaternion qRot(cgqIdentity);
    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    ACDID idACDPortal = SpellEffect_SpawnActor(idPower, SNOFileLookupGUID(ACTOR_SNO, "Amiibo_Scan_Portal"), tNavPlace, qRot, dwAddFlags, FALSE);

    ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);
    if (ptACDPortal)
    {
        const Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
        if (ptPlayer)
        {

            ptACDPortal->SetAssignedHeroID(ptPlayer);
        }






        PowerAPIBuffStartData tScanningBuff(idPower);
        tScanningBuff.pRemovalEffect = sSpellEffect_AmiiboPortalScan_End;
        tScanningBuff.pRemovalEffectData = (void *)(intptr_t)ptACDPortal->idOwner;
        tScanningBuff.dwDurationMin = Console::NFP::s_nNFPScanningSeconds * TICKS_PER_SECOND;
        tScanningBuff.dwDurationDelta = 0;
        tScanningBuff.bRemoveOnDeath = true;
        tScanningBuff.bPositiveEffect = true;
        BuffID idScanningBuff = PowerAPI_BuffAddToACD(tScanningBuff, *ptACDPlayer);


        sSpellEffect_AmiiboPortalScan_Begin(idPower, (void *)(intptr_t)idACDPortal);
    }
}











static BOOL sIsAmiiboPowerActive(const Console::NFP::NFPScanResult eScanResult)
{
    BOOL bAmiiboPowerPortalExists = FALSE;
    SNO snoPortalToCheckFor = SNO_NONE;
    if (eScanResult == Console::NFP::NFPScanResult_TreasureGoblinFigure)
    {
        snoPortalToCheckFor = SNOFileLookupGUID(ACTOR_SNO, "p1_Greed_Portal");
    }
    else if (eScanResult == Console::NFP::NFPScanResult_GenericFigure)
    {
        snoPortalToCheckFor = SNOFileLookupGUID(ACTOR_SNO, "Spawner_Amiibo_Demons");
    }

    if (snoPortalToCheckFor != SNO_NONE)
    {
        ActorIterator tIt;
        while (ActorDataArrayIterate(tIt))
        {
            Actor * ptActor = tIt.ptElement;
            if (ptActor->snoActor == snoPortalToCheckFor)
                bAmiiboPowerPortalExists = TRUE;
        }
    }

    return bAmiiboPowerPortalExists;
}



static void sResetClientToServerAmiiboScanTime(Player * ptPlayer, const Console::NFP::NFPScanResult eScanResult)
{
    if (!ptPlayer)
        return;




    PlayerDWordDataMessage tMessage;
    tMessage.dwData = eScanResult;
    switch (eScanResult)
    { case Console::NFP::NFPScanResult_GenericFigure:
        tMessage.nPlayerIndex = Blizzard::Time::ToUnixTime(ptPlayer->tNFPDefaultScanTime);
        break;


    case Console::NFP::NFPScanResult_TreasureGoblinFigure:
        tMessage.nPlayerIndex = Blizzard::Time::ToUnixTime(ptPlayer->tNFPGoblinScanTime);
        break;

    default:
        return;
    }



    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_SYNC_AMIIBO_SCAN_TIME, &tMessage, sizeof(tMessage));
}

static void sSpellEffect_AmiiboPortalDefault_Spawn(
    const PowerID idPower,
    void *)
{
    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    if (!ptACDPlayer || !ptACDPlayer->IsInWorld())
        return;




    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;




    NavPlace tNavPlace;
    tNavPlace.tPlace = ptPower->placeStartTarget;
    PowerPlaceToNavMeshHandle(tNavPlace.tPlace, tNavPlace.tNavMeshHandle);


    SNOArray listActors;
    blz::vector<int> listAllowedMonsterRarities;
    listAllowedMonsterRarities.push_back(MR_CHAMPION);
    listAllowedMonsterRarities.push_back(MR_RARE);
    ActorFindMonsterForPlayersLevelArea(*ptACDPlayer, listActors, listAllowedMonsterRarities);

    SNO snoActor = SNO_NONE;
    int32 nCount = listActors.GetArraySize();
    if (nCount)
    {
        int32 nIndex = RandRange_Internal(SGameGlobalsGet().rng, 0, nCount - 1);
        snoActor = listActors[nIndex];
    }

    if (snoActor == SNO_NONE)
    {

        snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_SUMMONING_SCROLL_FALLBACK);
    }

    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = snoActor;
    tActorAdd.tSNOName.Set(ACTOR_SNO, snoActor);
    tActorAdd.eMonsterRarity = MR_RARE;
    tActorAdd.eTeam = TEAM_MONSTER_0;
    tActorAdd.tMarkersName = MarkerStringHash("SummonMonster");
    tActorAdd.bIsSummon = TRUE;

    tActorAdd.idMarkersSScene = tNavPlace.tNavMeshHandle.idSScene;
    tActorAdd.tPlace = tNavPlace.tPlace;
    tActorAdd.tNavMeshHandle = tNavPlace.tNavMeshHandle;

    ActorID idActorMonster;
    if (SSceneTryToGet(tActorAdd.tNavMeshHandle.idSScene))
    {
        idActorMonster = GBAdd(tActorAdd);
    }





}




static void sSpellEffect_AmiiboPortalDefault_Begin(
    PowerID idPower,
    void * pData)
{



    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    if (!ptACDPlayer || !ptACDPlayer->IsInWorld())
        return;




    PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDPlayer, SNOFileLookupGUID(POWER_SNO, "AmiiboPortal"), -1, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);

    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
        return;



    if (sIsAmiiboPowerActive(Console::NFP::NFPScanResult_GenericFigure))
    {

        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_AMIIBO_POWER_ALREADY_ACTIVE);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_GenericFigure);
        return;
    }




    Blizzard::Time::Timestamp tTimeSinceLastScan = Blizzard::Time::GetTimestamp() - ptPlayer->tNFPDefaultScanTime;
    if (tTimeSinceLastScan < Console::NFP::s_nNFPDefaultCooldownMinutes * 60000000000ULL)
    {

        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_POWER_IN_COOLDOWN);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_GenericFigure);
        return;
    }






    if (ACDIsInTieredLootRun(*ptACDPlayer))
    {
        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_GenericFigure);
        return;
    }




    if (SWorldIsDungeonFinder(ptACDPlayer->GetWorldLocation().idSWorld))
    {
        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_POWER_UNUSABLE_IN_AREA);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_GenericFigure);
        return;
    }



    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_GenericFigure);
        return;
    }



    ptPlayer->tNFPDefaultScanTime = Blizzard::Time::GetTimestamp();
    ptPlayer->uConsoleSyncFlags |= 1 << PCSF_AMIIBO_SCAN_TIMES;


    SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_AcctFlags, PLAYER_SAVE_MODE_END_OF_FRAME);



    NavPlace tNavPlace;
    tNavPlace.tPlace = ptPower->placeStartTarget;
    PowerPlaceToNavMeshHandle(tNavPlace.tPlace, tNavPlace.tNavMeshHandle);
    Quaternion qRot(cgqIdentity);
    uint32 dwAddFlags = (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);
    ACDID idACDSpawner = SpellEffect_SpawnActor(idPower, SNOFileLookupGUID(ACTOR_SNO, "Spawner_Amiibo_Demons"), tNavPlace, qRot, dwAddFlags, FALSE);


    PowerTriggerAddTimerTrigger(idPower, Console::NFP::s_nNFPDefaultDemonSpawnDelay, sSpellEffect_AmiiboPortalDefault_Spawn, NULL);





}



static void sSpellEffect_AmiiboPortalTreasureGoblin_Begin(
    PowerID idPower,
    void * pData)
{



    ActorCommonData * ptACDPlayer = PowerGetOwner(idPower);
    if (!ptACDPlayer || !ptACDPlayer->IsInWorld())
        return;




    PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDPlayer, SNOFileLookupGUID(POWER_SNO, "AmiiboPortal"), -1, BUFF_REMOVAL_REASON_DEFAULT, ACDID_INVALID);

    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
        return;



    if (sIsAmiiboPowerActive(Console::NFP::NFPScanResult_TreasureGoblinFigure))
    {

        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_AMIIBO_POWER_ALREADY_ACTIVE);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_TreasureGoblinFigure);
        return;
    }



    Blizzard::Time::Timestamp tTimeSinceLastScan = Blizzard::Time::GetTimestamp() - ptPlayer->tNFPGoblinScanTime;
    if (tTimeSinceLastScan < Console::NFP::s_nNFPGoblinCooldownMinutes * 60000000000ULL)
    {

        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_POWER_IN_COOLDOWN);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_TreasureGoblinFigure);
        return;
    }






    if (ACDIsInTieredLootRun(*ptACDPlayer))
    {
        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_TreasureGoblinFigure);
        return;
    }




    if (SWorldIsDungeonFinder(ptACDPlayer->GetWorldLocation().idSWorld))
    {
        PlayerDisplayErrorMessage(ptACDPlayer->id, GAMEERROR_POWER_UNUSABLE_IN_AREA);
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_TreasureGoblinFigure);
        return;
    }



    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
    {
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_TreasureGoblinFigure);
        return;
    }


    BOOL bWorldExists = FALSE;
    SNO snoPortal = PowerContactTagGetSNO(idPower, TAG_POWER_DEATH_PORTAL_ACTOR_0);
    if (!sDeathPortalDestinationCanCreateWorld(snoPortal, &bWorldExists))
    {
        sResetClientToServerAmiiboScanTime(ptPlayer, Console::NFP::NFPScanResult_TreasureGoblinFigure);
        return;
    }



    ptPlayer->tNFPGoblinScanTime = Blizzard::Time::GetTimestamp();
    ptPlayer->uConsoleSyncFlags |= 1 << PCSF_AMIIBO_SCAN_TIMES;


    SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_AcctFlags, PLAYER_SAVE_MODE_END_OF_FRAME);

    WorldPlace tPlace = ptPower->placeStartTarget;
    SpellEffect_DeathPortalCreate(snoPortal, tPlace, ptPower);


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ptPlayer->tAccount.uPartitionFlags |= 1 << PARTITION_FLAG_GREED_REALM_VISITED;


        ptPlayer->uConsoleSyncFlags |= 1 << PCSF_PARTITION_FLAGS;
    }






}










void PowerAPI_MissileImpact(const PowerID idPower, void * pData);  // DWARF X/Server/Powers/PowerAPI.cpp:7550
void PowerAPI_MissileImpactOnce(const PowerID idPower, void * pData);  // DWARF X/Server/Powers/PowerAPI.cpp:7586
void Barbarian_Fury(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Barbarian.cpp:212
void Scoundrel_Multishot(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:68
void Scoundrel_RapidFire(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:36
void Scoundrel_Anatomy(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:93
void Scoundrel_RainOfGold(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:193
void Scoundrel_Bandage(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:240
void Scoundrel_Vanish(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:250
void Scoundrel_DirtyFighting(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:205
void Scoundrel_Scavenge(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:118
void Scoundrel_BlackMarket(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Scoundrel.cpp:141
void Monk_Spirit(const PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Monk.cpp:146
void Templar_Loyalty(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Paladin.cpp:54
void Templar_Inspire(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Paladin.cpp:96
void Templar_Protection(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Paladin.cpp:105
void Templar_Guardian(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Paladin.cpp:152
void Templar_Knight(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Paladin.cpp:177
void Templar_Tribute(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Paladin.cpp:204
void Templar_Empathy(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Paladin.cpp:284
void Enchantress_Disorient(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Enchantress.cpp:31
void Enchantress_Charm(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Enchantress.cpp:67
void Shrine_Buff(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:178
void Shrine_Desecrated_Blessed(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:350
void Shrine_Desecrated_Enlightened(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:360
void Shrine_Desecrated_Fortune(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:370
void Shrine_Desecrated_Frenzied(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:380
void PowerSummonMonsterForShrineOrScrollOfCalling(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_ItemUse.cpp:1132
void IGR_Buff(const PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:389
void Community_Event_Buff(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:422
void Anniversary_Buff(const PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:528
void GizmoPortalOperate(PowerID idPower, void *);  // DWARF X/Server/Powers/Power_Gizmo.cpp:537
void Sand_Tornado_OnSpawn(const PowerID idPower, void *);  // DWARF X/Server/AI/SandTornadoBrain.cpp:498

SpellEffectRunData arSpellEffectFunctions[] = {
    { SPELL_BASIC_MELEE, SpellEffect_BasicMelee, { GLOBALSNO_INVALID } },
    { SPELL_KNOCKBACK, sSpellEffect_Knockback, { GLOBALSNO_INVALID } },
    { SPELL_KNOCKBACK_START, sSpellEffect_KnockbackStart, { GLOBALSNO_INVALID } },
    { SPELL_KNOCKBACK_STOP, sSpellEffect_KnockbackStop, { GLOBALSNO_INVALID } },
    { SPELL_IMPACT_DAMAGE, SpellEffect_ImpactDamage, { GLOBALSNO_INVALID } },
    { SPELL_IMPACT_DAMAGE_RUN_CHILD, sSpellEffect_ImpactDamageRunChild, { GLOBALSNO_INVALID } },
    { SPELL_RUN_CHILD, sSpellEffect_RunChild, { GLOBALSNO_INVALID } },
    { SPELL_STANDARD_DAMAGE, sSpellEffect_PerformStandardDamage, { GLOBALSNO_INVALID } },
    { SPELL_PROJECTILE, SpellEffect_Projectile, { GLOBALSNO_INVALID } },
    { SPELL_MELEE_BOTH_ATTACK, SpellEffect_BasicMeleeBothAttack, { GLOBALSNO_INVALID } },
    { SPELL_DAMAGE_AT_CONTACT_POINT, sSpellEffect_DamageAtContactPoint, { GLOBALSNO_INVALID } },
    { SPELL_NOTHING, sSpellEffect_Nothing, { GLOBALSNO_INVALID } },
    { SPELL_TOWNPORTAL, sSpellEffect_TownPortal, { GLOBALSNO_INVALID } },
    { SPELL_SUMMONED, sSpellEffect_Summoned, { GLOBALSNO_INVALID } },
    { SPELL_ROOT_STRUGGLE, sSpellEffect_RootStruggle, { GLOBALSNO_INVALID } },
    { SPELL_RECALL_START, sSpellEffect_RecallStart, { GLOBALSNO_INVALID } },
    { SPELL_RECALL_INTERRUPTED, sSpellEffect_RecallInterrupted, { GLOBALSNO_INVALID } },
    { SPELL_RECALL, sSpellEffect_Recall, { GLOBALSNO_INVALID } },
    { SPELL_RECALL_ENTER_ONSTART, sSpellEffect_RecallEnterOnStart, { GLOBALSNO_ACTOR_GENERIC_PROXY, GLOBALSNO_INVALID } },
    { SPELL_RECALL_EXIT_ONSTART, sSpellEffect_RecallExitOnStart, { GLOBALSNO_INVALID } },
    { SPELL_RECALL_PORTAL_ENTER_ONSTART, sSpellEffect_RecallPortalEnterOnStart, { GLOBALSNO_ACTOR_GENERIC_PROXY, GLOBALSNO_INVALID } },
    { SPELL_RECALL_PORTAL_EXIT_ONSTART, sSpellEffect_RecallPortalExitOnStart, { GLOBALSNO_INVALID } },
    { SPELL_SWITCHING_SKILL, sSpellEffect_SwitchingSkill, { GLOBALSNO_INVALID } },
    { SPELL_DUNGEON_STONE, sSpellEffect_DungeonStone, { GLOBALSNO_INVALID } },
    { SPELL_LOOTRUN_PORTAL, sSpellEffect_LootRunPortal, { GLOBALSNO_INVALID } },
    { SPELL_IDENTIFY_ITEM, sSpellEffect_IdentifyItem, { GLOBALSNO_INVALID } },
    { SPELL_IDENTIFY_ALL, sSpellEffect_IdentifyAll },
    { SPELL_IDENTIFY_ALL_START, sSpellEffect_IdentifyAll_Start },
    { SPELL_IDENTIFY_ALL_INTERRUPT, sSpellEffect_IdentifyAll_Interrupt },
    { SPELL_TELEPORT_TO_PLAYER_CAST, sSpellEffect_TeleportToPlayer_Cast },
    { SPELL_TELEPORT_TO_WAYPOINT_CAST, sSpellEffect_TeleportToWaypoint_Cast },
    { SPELL_TELEPORT_TO_PLAYER, sSpellEffect_TeleportToPlayer },
    { SPELL_TELEPORT_TO_WAYPOINT, sSpellEffect_TeleportToWaypoint },
    { SPELL_SUMMON_VANITY_PET, sSpellEffect_SummonVanityPet },
    { SPELL_HEAL_OTHER, sSpellEffect_HealOther, { GLOBALSNO_INVALID } },
    { SPELL_RESURRECT_CORPSE, sSpellEffect_ResurrectCorpse, { GLOBALSNO_INVALID } },
    { SPELL_ITEM_RESURRECT_CORPSE, sSpellEffect_ItemResurrectCorpse, { GLOBALSNO_INVALID } },
    { SPELL_CLOUD, sSpellEffect_Cloud, { GLOBALSNO_INVALID } },
    { SPELL_SUICIDE_CLOUD, sSpellEffect_Suicide_Cloud, { GLOBALSNO_INVALID } },
    { SPELL_NOVA, sSpellEffect_Nova, { GLOBALSNO_INVALID } },
    { SPELL_NOVA_DELAYED, sSpellEffect_Nova_Delayed, { GLOBALSNO_INVALID } },
    { SPELL_DAMAGE_TARGET, sSpellEffect_DamageTarget, { GLOBALSNO_INVALID } },
    {
        SPELL_MISSILE_IMPACT, PowerAPI_MissileImpact, { GLOBALSNO_INVALID }
    },
    {
        SPELL_MISSILE_IMPACT_ONCE, PowerAPI_MissileImpactOnce, { GLOBALSNO_INVALID }
    },
    { SPELL_SUICIDE_EXPLOSION, sSpellEffect_SuicideExplosion, { GLOBALSNO_INVALID } },
    { SPELL_SUICIDE_EXPLOSION_DEATH, sSpellEffect_SuicideExplosionDeath, { GLOBALSNO_INVALID } },
    { SPELL_SUICIDE_EXPLOSION_WITH_LOOT, sSpellEffect_SuicideExplosionWithLoot, { GLOBALSNO_INVALID } },
    { SPELL_SUICIDE, sSpellEffect_Suicide, { GLOBALSNO_INVALID } },
    { SPELL_DESTRUCTABLE_OBJECT_AOE, sSpellEffect_DestructableObjectAOE, { GLOBALSNO_INVALID } },
    { SPELL_POISON_BREATH, sSpellEffect_PoisonBreath, { GLOBALSNO_INVALID } },
    { SPELL_SET_DAMAGE_STATE_ONE, sSpellEffect_SetDamageStateOne, { GLOBALSNO_INVALID } },
    { SPELL_SPIDER_WEB_IMMOBOLIZE, sSpellEffect_SpiderWebImmobolize, { GLOBALSNO_INVALID } },
    { SPELL_SPIDER_WEB_SLOW, sSpellEffect_SpiderWebSlow, { GLOBALSNO_ACTOR_SPIDER_WEB, GLOBALSNO_INVALID } },
    { SPELL_ZOMBIE_EAT_START, sSpellEffect_ZombieEatStart, { GLOBALSNO_INVALID } },
    { SPELL_ZOMBIE_EAT_STOP, sSpellEffect_ZombieEatStop, { GLOBALSNO_INVALID } },
    { SPELL_HEAL, sSpellEffect_Heal, { GLOBALSNO_INVALID } },
    { SPELL_STUN, sSpellEffect_Stun, { GLOBALSNO_INVALID } },
    { SPELL_CHARGED_BOLT, sSpellEffect_Charged_Bolt, { GLOBALSNO_INVALID } },
    { SPELL_MONSTER_TELEPORT, sSpellEffect_MonsterTeleport, { GLOBALSNO_INVALID } },
    { SPELL_SHIELD, sSpellEffect_Shield, { GLOBALSNO_INVALID } },
    { SPELL_BREAKABLE_SHIELD, sSpellEffect_Breakable_Shield, { GLOBALSNO_INVALID } },
    { SPELL_SHIELD_SKELETON_BASIC_MELEE, sSpellEffect_ShieldSkeleton_BasicMelee, { GLOBALSNO_INVALID } },
    { SPELL_DODGE, sSpellEffect_Dodge, { GLOBALSNO_INVALID } },
    { SPELL_TRANSFORM_MONSTER, sSpellEffect_Transform_Monster, { GLOBALSNO_INVALID } },
    { SPELL_TRANSFORM_MONSTER_INHERIT_GROUP, sSpellEffect_Transform_Monster_Inherit_Group, { GLOBALSNO_INVALID } },
    { SPELL_BURROW_IN, sSpellEffect_BurrowIn, { GLOBALSNO_INVALID } },
    { SPELL_BURROW_OUT, sSpellEffect_BurrowOut, { GLOBALSNO_INVALID } },
    { SPELL_STEALTH_IN, sSpellEffect_StealthIn, { GLOBALSNO_INVALID } },
    { SPELL_STEALTH_OUT, sSpellEffect_StealthOut, { GLOBALSNO_INVALID } },
    { SPELL_ENTER_SPECIAL_STATE, sSpellEffect_EnterSpecialState, { GLOBALSNO_INVALID } },
    { SPELL_EXIT_SPECIAL_STATE, sSpellEffect_ExitSpecialState, { GLOBALSNO_INVALID } },
    { SPELL_EMOTE_CONVERSATION, sSpellEffect_EmoteConversation, { GLOBALSNO_INVALID } },
    { SPELL_SUFFOCATE_AREA_ENTER, sSpellEffect_SuffocateAreaEnter, { GLOBALSNO_INVALID } },
    { SPELL_SUFFOCATE_AREA_EXIT, sSpellEffect_SuffocateAreaExit, { GLOBALSNO_INVALID } },
    { SPELL_FIREBREATH_START, sSpellEffect_Firebreath_Start, { GLOBALSNO_INVALID } },
    { SPELL_FIREBREATH_STOP, sSpellEffect_Firebreath_Stop, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_ZOMBIE, sSpellEffect_SummonZombie, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_SKELETON, sSpellEffect_SummonSkeleton, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_PAINTING_SKELETON, sSpellEffect_SummonPaintingSkeleton, { GLOBALSNO_INVALID } },
    { SPELL_VOMIT_ZOMBIE, sSpellEffect_VomitZombie, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_FALLEN, sSpellEffect_SummonFallen, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_FALLEN_ON_SPAWN, sSpellEffect_SummonFallen_OnSpawn, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_ZOMBIE_CRAWLER, sSpellEffect_SummonZombieCrawler, { GLOBALSNO_INVALID } }, { SPELL_UNIQUE_MONSTER_GENERIC_SUMMON, sSpellEffect_UniqueMonsterGenericSummon, { GLOBALSNO_INVALID } },
    { SPELL_SWIPE_LEFT, sSpellEffect_SwipeLeft, { GLOBALSNO_INVALID } },
    { SPELL_SWIPE_RIGHT, sSpellEffect_SwipeRight, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_STOMP, sSpellEffect_SiegebreakerDemon_Stomp, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_LOOK_AROUND, sSpellEffect_SiegebreakerDemon_LookAround, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_CHARGE, sSpellEffect_SiegebreakerDemon_Charge, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_MINICHARGE, sSpellEffect_SiegebreakerDemon_MiniCharge, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_GRAB, sSpellEffect_SiegebreakerDemon_Grab, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_GRAB_TO_BITE, sSpellEffect_SiegebreakerDemon_GrabToBite, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_BITE, sSpellEffect_SiegebreakerDemon_Bite, { GLOBALSNO_INVALID } },
    { SPELL_SIEGEBREAKER_DEMON_POUND, sSpellEffect_SiegebreakerDemon_Pound, { GLOBALSNO_INVALID } },
    { SPELL_TRIUNE_VESSEL_OVERPOWER, sSpellEffect_TriuneVessel_Overpower, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_TRIUNE_DEMON, sSpellEffect_SummonTriuneDemon, { GLOBALSNO_INVALID } },
    { SPELL_TRIUNE_BERSERKER_POWER_HIT, sSpellEffect_TriuneBerserkerPowerHit, { GLOBALSNO_INVALID } },
    { SPELL_CHAMPION_TELEPORT, sSpellEffect_Champion_Teleport, { GLOBALSNO_INVALID } }, { SPELL_CHAMPION_CLONE, sSpellEffect_Champion_Clone, { GLOBALSNO_INVALID } },
    { SPELL_DOPPELGANGER_AFFIX, sSpellEffect_MonsterAffix_Doppelganger, { GLOBALSNO_INVALID } },
    { SPELL_DOPPELGANGER_AFFIX_CLONE, sSpellEffect_Doppelganger_Clone, { GLOBALSNO_INVALID } },
    { SPELL_CURSE_PROC, sSpellEffect_Curse_Proc, { GLOBALSNO_INVALID } },
    { SPELL_KILLPACK_PROC, sSpellEffect_KillPack_Proc, { GLOBALSNO_INVALID } },
    { SPELL_ROOT_PROC, sSpellEffect_Root_Proc, { GLOBALSNO_INVALID } },
    { SPELL_DAMAGE_AURA, sSpellEffect_Damage_Aura, { GLOBALSNO_INVALID } },
    { SPELL_ENTANGLE_RAY, sSpellEffect_Entangle_Ray, { GLOBALSNO_INVALID } },
    { SPELL_ROOT_GRAB, sSpellEffect_Root_Grab, { GLOBALSNO_INVALID } },
    { SPELL_ROOT_TRY_GRAB, sSpellEffect_Root_Try_Grab, { GLOBALSNO_INVALID } },
    { SPELL_SUMMON_SPORES, sSpellEffect_Summon_Spore_Pods, { GLOBALSNO_INVALID } },
    { SPELL_ZOMBIE_KILLER_GRAB, sSpellEffect_ZombieKiller_Grab, { GLOBALSNO_INVALID } },
    { SPELL_ZOMBIE_KILLER_SQUISH, sSpellEffect_ZombieKiller_Squish, { GLOBALSNO_INVALID } },
    { SPELL_SCAVENGER_LEAP, sSpellEffect_Scavenger_Leap, { GLOBALSNO_INVALID } },
    { SPELL_SCAVENGER_BURROW_IN_START, sSpellEffect_Scavenger_Burrow_In_Start, { GLOBALSNO_INVALID } },
    { SPELL_SCAVENGER_BURROW_IN, sSpellEffect_Scavenger_Burrow_In, { GLOBALSNO_INVALID } },
    { SPELL_SCAVENGER_BURROW_OUT, sSpellEffect_Scavenger_Burrow_Out, { GLOBALSNO_INVALID } },
    { SPELL_LACUNI_LEAP_BEGIN, sSpellEffect_Lacuni_Leap_Begin, { GLOBALSNO_INVALID } },
    { SPELL_SANDSHARK_BURROW_IN, sSpellEffect_Sandshark_Burrow_In, { GLOBALSNO_INVALID } },
    { SPELL_SANDSHARK_BURROW_OUT, sSpellEffect_Sandshark_Burrow_Out, { GLOBALSNO_INVALID } },
    { SPELL_SANDSHARK_UNDER_SAND_ATTACK_TELEPORT, sSpellEffect_Sandshark_UnderSand_Attack_Teleport, { GLOBALSNO_INVALID } },
    { SPELL_SANDSHARK_UNDER_SAND_ATTACK_BEGIN, sSpellEffect_Sandshark_UnderSand_Attack_Begin, { GLOBALSNO_INVALID } },
    { SPELL_SANDSHARK_UNDER_SAND_ATTACK_BURST, sSpellEffect_Sandshark_UnderSand_Attack_Burst, { GLOBALSNO_INVALID } },
    { SPELL_CORPULENT_EXPLODE, sSpellEffect_CorpulentExplode, { GLOBALSNO_INVALID } },
    { SPELL_CORPULENT_EXPLODE_BEGIN, sSpellEffect_CorpulentExplodeBegin, { GLOBALSNO_INVALID } },
    { SPELL_CORPULENT_EXPLODE_SANITY_DELETE, sSpellEffect_CorpulentExplodeSanityDelete, { GLOBALSNO_INVALID } },
    { SPELL_CRYPTCHILD_LEAPOUT_START, sSpellEffect_CryptChildLeapOutStart, { GLOBALSNO_INVALID } },
    { SPELL_CRYPTCHILD_LEAPOUT, sSpellEffect_CryptChildLeapOut, { GLOBALSNO_INVALID } },
    { SPELL_GOATMAN_LIGHTNING_SHIELD, sSpellEffect_GoatmanLightningShield, { GLOBALSNO_INVALID } },
    { SPELL_GOATMAN_COLD_SHIELD, sSpellEffect_GoatmanColdShield, { GLOBALSNO_INVALID } },
    { SPELL_GOATMAN_ICEBALL, sSpellEffect_GoatmanIceball, { GLOBALSNO_INVALID } },
    { SPELL_GOATMAN_DRUM_HIT, sSpellEffect_GoatmanDrumHit, { GLOBALSNO_INVALID } },
    { SPELL_GOATMAN_EMPOWER, sSpellEffect_GoatmanEmpower, { GLOBALSNO_INVALID } },
    { SPELL_GOATMUTANT_ENRAGE, sSpellEffect_GoatmanEnrage, { GLOBALSNO_INVALID } },
    { SPELL_GOATMUTANT_EXPLODE, sSpellEffect_GoatmanExplode, { GLOBALSNO_INVALID } },
    { SPELL_BONEREAVER_CONSUME, sSpellEffect_Bonereaver_Consume, { GLOBALSNO_INVALID } },
    { SPELL_BONEREAVER_SUMMON, sSpellEffect_Bonereaver_Summon, { GLOBALSNO_INVALID } },
    { SPELL_GHOST_SOUL_SIPHON, sSpellEffect_Ghost_SoulSiphon, { GLOBALSNO_INVALID } },
    { SPELL_GHOST_SOUL_SIPHON_START, sSpellEffect_Ghost_SoulSiphon_Start, { GLOBALSNO_INVALID } },
    { SPELL_GHOST_SOUL_SIPHON_CLEANUP, sSpellEffect_Ghost_SoulSiphon_Cleanup, { GLOBALSNO_INVALID } },
    { SPELL_GHOST_WALK_THROUGH_WALLS, sSpellEffect_Ghost_WalkThroughWalls, { GLOBALSNO_INVALID } },
    { SPELL_ROCKWORM_HIDE_IDLE, sSpellEffect_Rockworm_HideIdle, { GLOBALSNO_INVALID } },
    { SPELL_ROCKWORM_PRE_BURST, sSpellEffect_Rockworm_PreBurst, { GLOBALSNO_INVALID } },
    { SPELL_ROCKWORM_BURST_OUT, sSpellEffect_Rockworm_BurstOut, { GLOBALSNO_INVALID } },
    { SPELL_ROCKWORM_RETREAT, sSpellEffect_Rockworm_Retreat, { GLOBALSNO_INVALID } },
    { SPELL_ROCKWORM_SPIT_WEB, sSpellEffect_Rockworm_SpitWeb, { GLOBALSNO_INVALID } },
    { SPELL_WEB_SLOW_PULSE, sSpellEffect_WebSlowPulse, { GLOBALSNO_INVALID } },
    { SPELL_BOGBLIGHT_PRE_BURST, sSpellEffect_BogBlight_PreBurst, { GLOBALSNO_INVALID } },
    { SPELL_BOGBLIGHT_BURST_OUT, sSpellEffect_BogBlight_BurstOut, { GLOBALSNO_INVALID } },
    { SPELL_BOGBLIGHT_BURROW_OUT, sSpellEffect_BogBlight_Burrow_Out, { GLOBALSNO_INVALID } },
    { SPELL_BOGBLIGHT_BURROW_IN, sSpellEffect_BogBlight_Burrow_In, { GLOBALSNO_INVALID } },
    { SPELL_BEAST_CHARGE_BEGIN, sSpellEffect_Beast_Charge_Begin, { GLOBALSNO_INVALID } },
    { SPELL_BEAST_CHARGE, sSpellEffect_Beast_Charge, { GLOBALSNO_INVALID } },
    {
        SPELL_BARBARIAN_FURY, Barbarian_Fury, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_MULTISHOT, Scoundrel_Multishot, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_RAPIDFIRE, Scoundrel_RapidFire, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_ANATOMY, Scoundrel_Anatomy, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_RAINOFGOLD, Scoundrel_RainOfGold, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_BANDAGE, Scoundrel_Bandage, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_VANISH, Scoundrel_Vanish, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_DIRTYFIGHTING, Scoundrel_DirtyFighting, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_SCAVENGE, Scoundrel_Scavenge, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SCOUNDREL_BLACKMARKET, Scoundrel_BlackMarket, { GLOBALSNO_INVALID }
    },
    { SPELL_HELPER_ATTACH, sSpellEffect_HelperAttach, { GLOBALSNO_INVALID } },
    { SPELL_HELPER_GUARD_OBJECT, sSpellEffect_HelperGuardObject, { GLOBALSNO_INVALID } },
    { SPELL_SETMODE_ESCORTFOLLOW, sSpellEffect_SetMode_EscortFollow, { GLOBALSNO_INVALID } },
    { SPELL_NPC_LOOKAT, sSpellEffect_NPC_LookAt, { GLOBALSNO_INVALID } },
    {
        SPELL_MONK_SPIRIT, Monk_Spirit, { GLOBALSNO_INVALID }
    },
    { SPELL_WIZARD_LIGHTNING_SPEED, Wizard_LightningSpeed, { GLOBALSNO_INVALID } },
    {
        SPELL_TEMPLAR_LOYALTY, Templar_Loyalty, { GLOBALSNO_INVALID }
    },
    {
        SPELL_TEMPLAR_INSPIRE, Templar_Inspire, { GLOBALSNO_INVALID }
    },
    {
        SPELL_TEMPLAR_PROTECTION, Templar_Protection, { GLOBALSNO_INVALID }
    },
    {
        SPELL_TEMPLAR_GUARDIAN, Templar_Guardian, { GLOBALSNO_INVALID }
    },
    {
        SPELL_TEMPLAR_KNIGHT, Templar_Knight, { GLOBALSNO_INVALID }
    },
    {
        SPELL_TEMPLAR_TRIBUTE, Templar_Tribute, { GLOBALSNO_INVALID }
    },
    {
        SPELL_TEMPLAR_EMPATHY, Templar_Empathy, { GLOBALSNO_INVALID }
    },
    {
        SPELL_ENCHANTRESS_DISORIENT, Enchantress_Disorient, { GLOBALSNO_INVALID }
    },
    {
        SPELL_ENCHANTRESS_CHARM, Enchantress_Charm, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SHRINE_BUFF, Shrine_Buff, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SHRINE_DESECRATED_BLESSED, Shrine_Desecrated_Blessed, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SHRINE_DESECRATED_ENLIGHTENED, Shrine_Desecrated_Enlightened, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SHRINE_DESECRATED_FORTUNE, Shrine_Desecrated_Fortune, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SHRINE_DESECRATED_FRENZIED, Shrine_Desecrated_Frenzied, { GLOBALSNO_INVALID }
    },
    {
        SPELL_SHRINE_CALL_MONSTER, PowerSummonMonsterForShrineOrScrollOfCalling, { GLOBALSNO_INVALID }
    },
    {
        SPELL_IGR_BUFF, IGR_Buff, { GLOBALSNO_INVALID }
    },
    {
        SPELL_COMMUNITY_EVENT_BUFF, Community_Event_Buff, { GLOBALSNO_INVALID }
    },
    {
        SPELL_ANNIVERSARY_BUFF, Anniversary_Buff, { GLOBALSNO_INVALID }
    },
    {
        SPELL_GIZMO_PORTAL_OPERATE, GizmoPortalOperate, { GLOBALSNO_INVALID }
    },
    { SPELL_WARP_IN_MAGICAL_START, sSpellEffect_WarpInMagicalStart, { GLOBALSNO_INVALID } },
    { SPELL_WARP_IN_MAGICAL_CONTACT, sSpellEffect_WarpInMagicalContact, { GLOBALSNO_INVALID } },
    { SPELL_HEARTH, sSpellEffect_Hearth, { GLOBALSNO_INVALID } },
    { SPELL_HEARTH_FINISH, sSpellEffect_HearthFinish, { GLOBALSNO_INVALID } },
    { SPELL_BURNING_HOUSE_FIREBALL_IMPACT, sSpellEffect_BurningHouseFireballImpact, { GLOBALSNO_INVALID } },
    { SPELL_BURNING_HOUSE_WATERBALL_IMPACT, sSpellEffect_BurningHouseWaterballImpact, { GLOBALSNO_INVALID } }, { SPELL_PANDEMONIUM_PORTAL, sSpellEffect_PandemoniumPortal, { GLOBALSNO_INVALID } },
    { SPELL_UBER_BOSS_SPECIFIC_PORTAL_0, sSpellEffect_UberBossSpecificPortal0, { GLOBALSNO_INVALID } },
    { SPELL_UBER_BOSS_SPECIFIC_PORTAL_1, sSpellEffect_UberBossSpecificPortal1, { GLOBALSNO_INVALID } },
    { SPELL_UBER_BOSS_SPECIFIC_PORTAL_2, sSpellEffect_UberBossSpecificPortal2, { GLOBALSNO_INVALID } },
    { SPELL_UBER_BOSS_SPECIFIC_PORTAL_3, sSpellEffect_UberBossSpecificPortal3, { GLOBALSNO_INVALID } }, { SPELL_AMIIBO_PORTAL_BEGIN, sSpellEffect_AmiiboPortal_Begin, { GLOBALSNO_INVALID } }, { SPELL_AMIIBO_PORTAL_DEFAULT_BEGIN, sSpellEffect_AmiiboPortalDefault_Begin, { GLOBALSNO_INVALID } }, { SPELL_AMIIBO_PORTAL_TREASURE_GOBLIN_BEGIN, sSpellEffect_AmiiboPortalTreasureGoblin_Begin, { GLOBALSNO_INVALID } },
    { SPELL_DEATH_NOVA, sSpellEffect_DeathNova, { GLOBALSNO_INVALID } },
    { SPELL_GENERIC_ATTRIBUTE_BUFF, sSpellEffect_GenericAttributeBuff, { GLOBALSNO_INVALID } },
    { SPELL_PROXY_DELAYED_POWER, sSpellEffect_ProxyDelayedPower, { GLOBALSNO_INVALID } },
    { SPELL_BOUNCING_BALLS, sSpellEffect_BouncingBalls, { GLOBALSNO_INVALID } },
    { SPELL_SUPER_PROJECTILE_IMPACT, sSpellEffect_SuperProjectileImpact, { GLOBALSNO_INVALID } },
    { SPELL_SUPER_PROJECTILE, sSpellEffect_SuperProjectile, { GLOBALSNO_INVALID } },
    { SPELL_SNAKEMAN_CASTER_ELECTRIC_BURST, sSpellEffect_SnakemanCasterElectricBurst, { GLOBALSNO_INVALID } },
    { SPELL_SNAKEMAN_CASTER_BURST_INTERRUPT, sSpellEffect_SnakemanCasterElectricBurstInterrupted, { GLOBALSNO_INVALID } },
    { SPELL_SNAKEMAN_CASTER_BURST_START, sSpellEffect_SnakemanCasterElectricBurstStart, { GLOBALSNO_INVALID } },
    { SPELL_SNAKEMAN_MELEE_STEALTH, sSpellEffect_SnakemanMeleeStealth, { GLOBALSNO_INVALID } },
    { SPELL_SNAKEMAN_MELEE_UNSTEALTH, sSpellEffect_SnakemanMeleeUnstealth, { GLOBALSNO_INVALID } },
    { SPELL_ELECTRICEEL_ELECTRIC_BURST, sSpellEffect_ElectricEelElectricBurst, { GLOBALSNO_INVALID } },
    { SPELL_FALLEN_CHAMPION_LEADER_SHOUT, sSpellEffect_FallenChampionLeaderShout, { GLOBALSNO_INVALID } },
    { SPELL_FALLEN_HOUND_HIT_BUFF, sSpellEffect_FallenHoundHitBuff, { GLOBALSNO_INVALID } },
    { SPELL_FALLEN_HOUND_LEAP, sSpellEffect_FallenHound_Leap, { GLOBALSNO_INVALID } },
    { SPELL_FALLEN_SHAMAN_RESSURECT, sSpellEffect_FallenShaman_Ressurect, { GLOBALSNO_INVALID } },
    { SPELL_DUNE_DERVISH_WHIRLWIND_BEGIN, sSpellEffect_DuneDervish_WhirlwindBegin, { GLOBALSNO_INVALID } },
    { SPELL_DUNE_DERVISH_WHIRLWIND_END, sSpellEffect_DuneDervish_WhirlwindEnd, { GLOBALSNO_INVALID } },
    {
        SPELL_SAND_TORNADO_AOE, Sand_Tornado_OnSpawn, { GLOBALSNO_INVALID }
    },
    { SPELL_ZK_BALL_SUMMON_SKELETON, sSpellEffect_ZKBallSummonSkeleton, { GLOBALSNO_INVALID } },
    { SPELL_SANDMONSTER_SANDWALL, sSpellEffect_SandMonster_SandWall, { GLOBALSNO_INVALID } },
    { SPELL_SOARING_DESCEND, sSpellEffect_SoaringDescend, { GLOBALSNO_INVALID } },
    { SPELL_SOARING_ASCEND, sSpellEffect_SoaringAscend, { GLOBALSNO_INVALID } },
    { SPELL_GOBLIN_USE_PORTAL, sSpellEffect_GoblinUsePortal, { GLOBALSNO_INVALID } },
    { SPELL_GOBLIN_THROW_PORTAL, sSpellEffect_GoblinThrowPortal, { GLOBALSNO_ACTOR_TREASURE_GOBLIN_PORTAL_PROXY, GLOBALSNO_INVALID } },
    { SPELL_GOBLIN_SPAWN_PORTAL, sSpellEffect_GoblinSpawnPortal, { GLOBALSNO_ACTOR_TREASURE_GOBLIN_PORTAL, GLOBALSNO_INVALID } },
    { SPELL_CREEPMOB_ARM_PLUNGE, sSpellEffect_CreepMob_Arm_Plunge, { GLOBALSNO_INVALID } },
    { SPELL_CREEPMOB_SPAWN_CREEPER, sSpellEffect_CreepMob_Spawn_Creeper, { GLOBALSNO_ACTOR_CREEPMOB_ARM, GLOBALSNO_INVALID } },
    { SPELL_CREEPMOB_END_PLUNGE, sSpellEffect_CreepMob_End_Plunge, { GLOBALSNO_INVALID } },
    { SPELL_BRICKHOUSE_ARMSHIELDS, sSpellEffect_Brickhouse_ArmShields, { GLOBALSNO_INVALID } },
    { SPELL_BRICKHOUSE_ENRAGE, sSpellEffect_Brickhouse_Enrage, { GLOBALSNO_INVALID } }, { SPELL_BRICKHOUSE_DESTRUCTION_SETUP, sSpellEffect_Brickhouse_Destruction_Setup, { GLOBALSNO_INVALID } },
    { SPELL_EXPEL_MONSTER, sSpellEffect_ExpelMonster, { GLOBALSNO_INVALID } },
    { SPELL_AREA_MOVEMENT_SLOW, sSpellEffect_MovementSlow, { GLOBALSNO_INVALID } },
    { SPELL_TARPIT_SLOW_ON, sSpellEffect_TarpitSlowOn, { GLOBALSNO_INVALID } },
    { SPELL_TARPIT_SLOW_OFF, sSpellEffect_TarpitSlowOff, { GLOBALSNO_INVALID } },
    { SPELL_INVULNERABLE_ANIM_BEGIN, sSpellEffect_InvulnerableAnimBegin, { GLOBALSNO_INVALID } },
    { SPELL_INVULNERABLE_ANIM_END, sSpellEffect_InvulnerableAnimEnd, { GLOBALSNO_INVALID } },
    { SPELL_EEL_SPAWN_BEGIN, sSpellEffect_EelSpawnBegin, { GLOBALSNO_INVALID } },
    { SPELL_SKELETON_KING_WHIRLWIND_BEGIN, sSpellEffect_SkeletonKing_WhirlwindBegin, { GLOBALSNO_INVALID } },
    { SPELL_SKELETON_KING_WHIRLWIND_MOVE, sSpellEffect_SkeletonKing_WhirlwindMove, { GLOBALSNO_INVALID } },
    { SPELL_SKELETON_KING_WHIRLWIND_ATTACK, sSpellEffect_sSkeletonKing_WhirlwindAttack, { GLOBALSNO_INVALID } },
    { SPELL_SKELETON_KING_WHIRLWIND_END, sSpellEffect_SkeletonKing_WhirlwindEnd, { GLOBALSNO_INVALID } },
    { SPELL_DEATH_PORTAL_PROC, sSpellEffect_DeathPortalProcDelay_Generic, { GLOBALSNO_INVALID } },
    { SPELL_DEATH_PORTAL_PROC_GREED_REALM, sSpellEffect_DeathPortalProcDelay_GreedRealm, { GLOBALSNO_INVALID } },
    { SPELL_WARP, sSpellEffect_Warp, { GLOBALSNO_INVALID } },
    { SPELL_WARN_OTHERS, sSpellEffect_WarnOthers, { GLOBALSNO_INVALID } },
    { SPELL_SUMMONING_MACHINE_ACTIVATE, sSpellEffect_SummoningMachineActivate, { GLOBALSNO_INVALID } },
    { SPELL_SUMMONING_MACHINE_SUMMON, sSpellEffect_SummoningMachineSummon, { GLOBALSNO_INVALID } },
    { SPELL_LUNATIC_DIVE_IN_START, sSpellEffect_LunaticDiveInStart, { GLOBALSNO_INVALID } },
    { SPELL_LUNATIC_DIVE_IN_INTERRUPTED, sSpellEffect_LunaticDiveInInterrupted, { GLOBALSNO_INVALID } }, { SPELL_LUNATIC_DIVE_IN_LAND, sSpellEffect_LunaticDiveInLand, { GLOBALSNO_INVALID } },
    { SPELL_MASTABLASTA_RIDER_COMBINE, sSpellEffect_MastaBlastaRiderCombine, { GLOBALSNO_INVALID } },
    { SPELL_MASTABLASTA_STEED_DRAIN, sSpellEffect_MastaBlastaSteedDrain, { GLOBALSNO_INVALID } },
    { SPELL_MASTABLASTA_DISMOUNT_RIDER, sSpellEffect_MastaBlastaDismountRider, { GLOBALSNO_INVALID } },
    { SPELL_DEMON_TROOPER_LEAP_OUT_START, sSpellEffect_DemonTrooperLeapOutStart, { GLOBALSNO_INVALID } },
    { SPELL_WALLMONSTER_SUMMON, sSpellEffect_WallMonsterSummon, { GLOBALSNO_INVALID } },
    { SPELL_SUCCUBUS_BLOODSTAR, sSpellEffect_Succubus_BloodStar, { GLOBALSNO_INVALID } },
    { SPELL_SUCCUBUS_LEAP, sSpellEffect_Succubus_Leap, { GLOBALSNO_INVALID } },
    { SPELL_SUCCUBUS_FLY, sSpellEffect_Succubus_Fly, { GLOBALSNO_INVALID } },
    { SPELL_SUCCUBUS_BLOODSTAR_IMPACT, sSpellEffect_Succubus_BloodStar_Impact, { GLOBALSNO_INVALID } },
    { SPELL_SUCCUBUS_STUN_NPC, sSpellEffect_Succubus_StunNPC, { GLOBALSNO_INVALID } },
    { SPELL_CORRUPT_ANGEL_UNHOLY_SHIELD, sSpellEffect_CorruptAngel_UnholyShield, { GLOBALSNO_INVALID } },
    { SPELL_CORRUPT_ANGEL_MELEE_STRIKE, sSpellEffect_CorruptAngel_MeleeStrike, { GLOBALSNO_INVALID } },
    { SPELL_CORRUPTANGEL_PIERCINGDASH_CREATE, sSpellEffect_CorruptAngel_PiercingDash_OnCreate, { GLOBALSNO_INVALID } },
    {
        SPELL_CORRUPTANGEL_PIERCINGDASH_START, sSpellEffect_CorruptAngel_PiercingDash_OnStart, { GLOBALSNO_INVALID }
    },
    { SPELL_CORRUPTANGEL_PIERCINGDASH_END, sSpellEffect_CorruptAngel_PiercingDash_OnEnd, { GLOBALSNO_INVALID } },
    { SPELL_FRENZY, sSpellEffect_Frenzy, { GLOBALSNO_INVALID } },
    { SPELL_PICKUP_NEARBY, sSpellEffect_PickupNearby, { GLOBALSNO_INVALID } },
    { SPELL_SPAWN_BANNER, sSpellEffect_BeginBannerSpawn, { GLOBALSNO_ACTOR_BANNER_ARRIVAL_PROXY, GLOBALSNO_INVALID } },
    { SPELL_TERROR_DEMON_MELEE_STRIKE, sSpellEffect_TerrorDemon_MeleeStrike, { GLOBALSNO_INVALID } },
    { SPELL_TERROR_DEMON_SHADOW_PHASE, sSpellEffect_TerrorDemon_ShadowPhase, { GLOBALSNO_INVALID } },
    { SPELL_TERROR_DEMON_SHADOW_PHASE_END, sSpellEffect_TerrorDemon_ShadowPhase_End, { GLOBALSNO_INVALID } },
    { SPELL_TERROR_DEMON_AVENGER_BURROW_IN, sSpellEffect_TerrorDemonAvenger_BurrowIn, { GLOBALSNO_ACTOR_CONSOLE_AVENGER_PORTAL, GLOBALSNO_INVALID } },
    { SPELL_TERROR_DEMON_AVENGER_SPAWN_CLONES, sSpellEffect_TerrorDemonAvenger_SpawnClones, { GLOBALSNO_INVALID } },
    { SPELL_CANNIBAL_BARBARIANS_INTRO, sSpellEffect_CannibalBarbarian_Intro, { GLOBALSNO_INVALID } },
    { SPELL_CORE_ELITE_DEMON_DROP_POD_SETUP, sSpellEffect_CoreEliteDemon_DropPod_Setup, { GLOBALSNO_INVALID } },
    { SPELL_CORE_ELITE_DEMON_DROP_POD_BEGIN, sSpellEffect_CoreEliteDemon_DropPod_Drop_Begin, { GLOBALSNO_ACTOR_DROP_POD_PROXY_WARNING, GLOBALSNO_INVALID } },
    { SPELL_CORE_ELITE_DEMON_DROP_POD_IMPACT, sSpellEffect_CoreEliteDemon_DropPod_Drop_Impact, { GLOBALSNO_INVALID } },
    { SPELL_HOODEDNIGHTMARE_LIGHTNING, sSpellEffect_HoodedNightmare_Lightning, { GLOBALSNO_INVALID } },
    { SPELL_HOODEDNIGHTMARE_LIGHTNING_EFFECT, sSpellEffect_HoodedNightmare_Lightning_Effect, { GLOBALSNO_INVALID } },
    { SPELL_HOODEDNIGHTMARE_CURSES, sSpellEffect_HoodedNightmare_Curses, { GLOBALSNO_INVALID } },
    { SPELL_HOODEDNIGHTMARE_BONEARMOR, sSpellEffect_HoodedNightmare_BoneArmor, { GLOBALSNO_INVALID } },
    { SPELL_HOODEDNIGHTMARE_GATEWAY, sSpellEffect_HoodedNightmare_Gateway, { GLOBALSNO_ACTOR_HOODEDNIGHTMARE_GATEWAY, GLOBALSNO_INVALID } },
    { SPELL_MAGHDA_SHIELD, sSpellEffect_Maghda_Shield, { GLOBALSNO_INVALID } },
    { SPELL_AZMODAN_BODYGUARD_TELEPORT_AFFIX, sSpellEffect_AzmodanBodyguard_TeleportAffix, { GLOBALSNO_INVALID } },
    { SPELL_STAMP_TARGET_INTO_GROUND, sSpellEffect_StampTarget_IntoGround, { GLOBALSNO_INVALID } },
    { SPELL_ROLL_TO_TARGET, sSpellEffect_RollToTarget, { GLOBALSNO_INVALID } },
    { SPELL_MORLU_CASTER_METEOR, sSpellEffect_MorluCaster_Meteor, { GLOBALSNO_ACTOR_MORLU_CASTER_METEOR_IMPACT, GLOBALSNO_ACTOR_MORLU_CASTER_METEOR_AFTERBURN, GLOBALSNO_ACTOR_MORLU_CASTER_METEOR_PENDING, GLOBALSNO_INVALID } },
    { SPELL_MORLU_CASTER_BREATH_OF_FIRE, sSpellEffect_MorluCaster_BreathOfFire, { GLOBALSNO_INVALID } },
    { SPELL_MORLU_CASTER_SHIFT, sSpellEffect_MorluCaster_Shift, { GLOBALSNO_INVALID } },
    { SPELL_MORLU_MELEE_PHOENIX, sSpellEffect_MorluMelee_PhoenixOnStart, { GLOBALSNO_INVALID } },
    { SPELL_CONSOLE_TELEPORT, sSpellEffect_Console_Teleport, { GLOBALSNO_INVALID } },
    { SPELL_PORTALMONSTER_BURROW_IN, sSpellEffect_PortalMonster_Burrow_In, { GLOBALSNO_INVALID } },
    { SPELL_PORTALMONSTER_BURROW_OUT, sSpellEffect_PortalMonster_Burrow_Out, { GLOBALSNO_INVALID } },
    { SPELL_PORTALMONSTER_DEATH_ORIENTATION, sSpellEffect_PortalMonster_Death_Orientation, { GLOBALSNO_INVALID } },
    { SPELL_ARMORSCAVENGER_BURROW_IN, sSpellEffect_ArmorScavenger_Burrow_In, { GLOBALSNO_INVALID } },
    { SPELL_ARMORSCAVENGER_BURROW_OUT, sSpellEffect_ArmorScavenger_Burrow_Out, { GLOBALSNO_INVALID } },
    { SPELL_ARMORSCAVENGER_PRE_BURROW_START, sSpellEffect_ArmorScavenger_Pre_Burrow_Start, { GLOBALSNO_INVALID } },
    { SPELL_ARMORSCAVENGER_PRE_BURROW_INTERRUPTED, sSpellEffect_ArmorScavenger_Pre_Burrow_Interrupted, { GLOBALSNO_INVALID } },
    { SPELL_ROCKFODDER_CHARGE, sSpellEffect_RockFodder_Charge, { GLOBALSNO_INVALID } },
    { SPELL_ROCKFODDER_CHARGE_IMPACT, sSpellEffect_RockFodder_ChargeImpact, { GLOBALSNO_INVALID } },
    { SPELL_NIGHTSCREAMER_CHEAPATTACK, sSpellEffect_NightScreamer_CheapAttack, { GLOBALSNO_INVALID } },
    { SPELL_NIGHTSCREAMER_ALLYBUFF, sSpellEffect_NightScreamer_AllyBuff, { GLOBALSNO_INVALID } },
    { SPELL_WRAITH_SWERVE_ADVANCE, sSpellEffect_Wraith_Swerve_Advance, { GLOBALSNO_INVALID } },
    { SPELL_WRAITH_SWERVE_PROJECTILE, sSpellEffect_Wraith_Swerve_Projectile, { GLOBALSNO_INVALID } },
    { SPELL_WRAITH_SWERVE_PROJECTILEIMPACT, sSpellEffect_Wraith_Swerve_ProjectileImpact, { GLOBALSNO_INVALID } },
    { SPELL_ROCKWORM_SPAWN_CAGE, sSpellEffect_Rockworm_Spawn_Cage, { GLOBALSNO_INVALID } },
    { SPELL_ROCKWORM_BURROW_AND_TELEPORT, sSpellEffect_Rockworm_BurrowAndTeleport, { GLOBALSNO_INVALID } },
    { SPELL_LEAPERANGEL_PREVENT_NEARBY_LEAPER, sSpellEffect_LeaperAngel_PreventNearbyLeaper, { GLOBALSNO_INVALID } },
    { SPELL_LEAPERANGEL_FIRE_BLADES_BUFF, sSpellEffect_LeaperAngel_FireBladesBuff, { GLOBALSNO_INVALID } },
    { SPELL_TELEPORT_NEAR_TARGET, sSpellEffect_Teleport_Near_Target, { GLOBALSNO_INVALID } },
    { SPELL_TELEPORT_TO_TARGET, sSpellEffect_Teleport_To_Target, { GLOBALSNO_INVALID } },
    { SPELL_ADRIA_TELEPORT_AOE, sSpellEffect_Adria_Teleport_AOE, { GLOBALSNO_INVALID } },
    { SPELL_ADRIA_TELEPORT_TO_CAULDRON_ACTIVATE, sSpellEffect_Adria_Teleport_To_Cauldron_Activate, { GLOBALSNO_INVALID } },
    { SPELL_GENERIC_LEGENDARY_POTION_START, sSpellEffect_Generic_Legendary_Potion_Start, { GLOBALSNO_INVALID } },
    { SPELL_LEGENDARY_POTION_BREAK_WALLS, sSpellEffect_Legendary_Potion_Break_Walls, { GLOBALSNO_INVALID } },
    { SPELL_GENERIC_BREAK_WALLS, sSpellEffect_Generic_Break_Walls, { GLOBALSNO_INVALID } },
    { SPELL_LEGENDARY_DETONATE_DOT_BUFFS, sSpellEffect_Legendary_Detonate_DOT_Buffs, { GLOBALSNO_INVALID } },
};




































#include "../../Sigma/DataStructures/XMap.h"
SpellEffect PowerGetSpellEffect(PowerID idPower, int32 nIndex);  // Powers.cpp
SpellEffect PowerGetBeginEffect(PowerID idPower);  // Powers.cpp
SpellEffect PowerGetEndEffect(PowerID idPower);  // Powers.cpp

static const int32 nNumSpellEffectFunctions = sizeof(arSpellEffectFunctions) / sizeof(arSpellEffectFunctions[0]);

typedef XMap<int32, int32, PowerTriggerCallback, PowerTriggerCallback, 512> SpellEffectMap;
static SpellEffectMap gmapSpellEffects;




void SpellEffectRun(PowerID idPower, SpellEffect eSpellEffect, void * pData)
{



    PowerTriggerCallback pfnSpellEffectFunction;
    if (gmapSpellEffects.Lookup(eSpellEffect, pfnSpellEffectFunction))
    {


        pfnSpellEffectFunction(idPower, pData);
    }
    else
    {

        Power * ptPower = PowerGet(idPower);  // fed a trace that is not in this build
    }
}




void SpellEffectRunNextEffect(PowerID idPower, void * pData)
{


    Power * ptPower = PowerGet(idPower);

    SpellEffect eSpellEffectToRun = PowerGetSpellEffect(idPower, ptPower->nCurrentSpellEffect);
    if (eSpellEffectToRun != SPELL_INVALID && eSpellEffectToRun != SPELL_NONE)
    {

        SpellEffectRun(idPower, eSpellEffectToRun, pData);
    }
}




void SpellEffectRunBeginEffect(PowerID idPower, void * pData)
{


    SpellEffect eSpellEffectToRun = PowerGetBeginEffect(idPower);
    if (eSpellEffectToRun != SPELL_INVALID && eSpellEffectToRun != SPELL_NONE)
    {
        SpellEffectRun(idPower, eSpellEffectToRun, pData);
    }
    Power * ptPower = PowerGet(idPower);
    PowerHelper_HitSoundInit(ptPower);
}






void SpellEffectRunEndEffect(PowerID idPower, void * pData)
{

    SpellEffect eSpellEffectToRun = PowerGetEndEffect(idPower);
    if (eSpellEffectToRun != SPELL_INVALID && eSpellEffectToRun != SPELL_NONE)
    {

        SpellEffectRun(idPower, eSpellEffectToRun, pData);
    }
}





void SpellEffectRunInterruptedEffect(PowerID idPower, void * pData)
{

    SpellEffect eSpellEffectToRun = PowerGetInterruptedEffect(idPower);
    if (eSpellEffectToRun != SPELL_INVALID && eSpellEffectToRun != SPELL_NONE)
    {

        SpellEffectRun(idPower, eSpellEffectToRun, pData);
    }
}

void SpellEffectIncrement(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    ptPower->nCurrentSpellEffect++;
}






void SpellEffect_InitializeForApp()
{





    for (int32 nIndex = 0; nIndex < nNumSpellEffectFunctions; nIndex++)
    {
        SpellEffectRunData & tData = arSpellEffectFunctions[nIndex];
        gmapSpellEffects.SetAt_Tracked(tData.eSpellEffect, tData.pfnCallback);
    }
}

// DWARF records only X/Server/RandomMonster.cpp:1260; no header declaration was found.
void MonsterDoOnCreatePowers(ACDID idACD);
#line 5040 "X/Server/Powers/SpellEffect.cpp"
static void sChampionSpawnClone(
    PowerID idPower,
    NavPlace tNavPlace,
    Quaternion & qOrientation)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }

    CRefString sActorName("Cloned_");
    sActorName += SNOToString(ACTOR_SNO, ptACD->tSNOName.snoHandle, FALSE);

    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = 0;
    tActorAdd.tPlace = tNavPlace.tPlace;
    tActorAdd.qOrientation = qOrientation;
    tActorAdd.eTeam = ptACD->GetNaturalTeam();
    tActorAdd.tSNOName = ptACD->tSNOName;
    tActorAdd.tMarkersName = MarkerStringHash(sActorName);
    tActorAdd.nLevel = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL);
    tActorAdd.tNavMeshHandle = tNavPlace.tNavMeshHandle;
    tActorAdd.flScale = ptACD->GetScale();
    tActorAdd.bIsSummon = TRUE;

    ActorID idActorClone = GBAdd(tActorAdd);
    if (idActorClone == ACTORID_INVALID)
    {
        return;
    }


    Actor * ptActorClone = ActorTryToGet(idActorClone);
    if (!ptActorClone)
    {
        return;
    }


    ACDID idACDClone = ptActorClone->idACD;
    ActorCommonData * ptACDClone = ACDTryToGet(idACDClone);
    if (!ptACDClone)
    {
        return;
    }


    uint32 SummonerID = ptACD->AttributesGetInt(ATTRIBUTE_SUMMONER_ID);

    ptACDClone->AttributesSetInt(ATTRIBUTE_SUMMONED_BY_ACDID, SummonerID);


    SigmaMemoryCopy(ptACDClone->pRareNameGBIDs, ptACD->pRareNameGBIDs, sizeof(ptACD->pRareNameGBIDs));
    ptACDClone->eMonsterRarity = MR_NORMAL;


    int32 nCloneAttributeIdx = 0;
    GBID gbidDoppelgangerAffix = StringToGBID("Doppelganger");
    GBID gbidPuppetmasterAffix = StringToGBID("Puppetmaster");
    for (int32 i = 0; i < 8; i++)
    {
        if (ptACD->pMonsterAffixGBIDs[i] != GBID_NONE && ptACD->pMonsterAffixGBIDs[i] != gbidDoppelgangerAffix)
        {

            GBHandle hGB;
            hGB.eType = GB_MONSTER_AFFIXES;
            hGB.gbid = ptACD->pMonsterAffixGBIDs[i];

            if (!GBIDExists(hGB))
            {
                continue;
            }




            MonsterRarity eMonsterRarity = ptACD->eMonsterRarity;
            if (hGB.gbid == gbidPuppetmasterAffix)
            {
                eMonsterRarity = MR_RARE_MINION;
            }



            MonsterAffixesApply(*ptACDClone, eMonsterRarity, hGB);

            ptACDClone->pMonsterAffixGBIDs[nCloneAttributeIdx] = ptACD->pMonsterAffixGBIDs[i];
            nCloneAttributeIdx++;
        }
    }
    for (; nCloneAttributeIdx < 8; nCloneAttributeIdx++)
    {
        ptACDClone->pMonsterAffixGBIDs[nCloneAttributeIdx] = GBID_NONE;
    }


    float flHealthMultiplier = 1.0f + ptACD->AttributesGetFloat(ATTRIBUTE_CHAMPION_CLONE_HITPOINT_BONUS_PERCENT);
    flHealthMultiplier = Math::Max(flHealthMultiplier, 0.0f);
    float flHealth = ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX);
    flHealth *= flHealthMultiplier;
    ptACDClone->AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX, flHealth);
    ptACDClone->AttributesSetFloat(ATTRIBUTE_HITPOINTS_CUR, ptACDClone->GetHitPointsMaxTotal());


    float flDamagePercent = ptACD->AttributesGetFloat(ATTRIBUTE_CHAMPION_CLONE_DAMAGE_BONUS_PERCENT);
    ptACDClone->AttributesSetFloat(ATTRIBUTE_DAMAGE_WEAPON_PERCENT_BONUS, flDamagePercent);
    ptACDClone->AttributesSetFloat(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS, flDamagePercent);
    ptACDClone->AttributesSetFloat(ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT, ptACD->AttributesGetFloat(ATTRIBUTE_DAMAGE_DONE_REDUCTION_PERCENT));


    ptACDClone->AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
    ptACDClone->AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);


    ACDPack * ptACDPack = ACDPackTryToGet(ptACD->idACDPack);
    if (ptACDPack)
    {
        ptACDPack->AddMember(*ptACDClone);
    }


    ACDPlayEffect(idACDClone, EFFECT_PLAY_EFFECT_GROUP, GlobalSNOGet(GLOBALSNO_EFFECTGROUP_CLONED), ACDID_INVALID);

    MonsterDoOnCreatePowers(idACDClone);
}




static const float CHAMPION_CLONE_AWARENESS_RADIUS = 150.0f;
static void sSpellEffect_Champion_Clone(
    PowerID idPower,
    void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
    {
        return;
    }





    ActorID idActor = (ActorID)ptACD->idOwner;
    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
    {
        return;
    }

    ptACD->GenerateSummonerID();

    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    int32 nMaxSummons = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_AI_MAX_CLONE_COUNT);

    int32 nExistingUnits = ACDCountNearbySummonedUnits(*ptACD, CHAMPION_CLONE_AWARENESS_RADIUS);
    if (nExistingUnits >= nMaxSummons)
    {
        sChampion_Finish(ptACD->id);
        return;
    }

    NavPlace tNavPlace;
    Quaternion qOrientation;
    ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
    qOrientation = ptACD->GetOrientation();

    sChampionSpawnClone(idPower, tNavPlace, qOrientation);
    sChampion_Finish(ptACD->id);
}
