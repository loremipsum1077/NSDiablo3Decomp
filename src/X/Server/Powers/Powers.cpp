#include "Powers.h"

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/RefString.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/GameConstants.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Player.h"
#include "../LuaScript/LuaScript.h"
#include "../Timers.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/Definitions/SkillKitDefinition.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/TagMaps/TagNumbers.h"
#include "../Actors/Actors.h"
#include "../Actors/ActorPowers.h"
#include "PowerHelper.h"
#include "PowerScript.h"
#include "PowerTrigger.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "PowerAPI.h"

BOOL PowerContactTagGetBool(PowerID idPower, uint32 dwTag);
int32 PowerContactTagGetInt(PowerID idPower, uint32 dwTag);
float PowerContactTagGetFloat(PowerID idPower, uint32 dwTag);
SNO PowerContactTagGetSNO(PowerID idPower, uint32 dwTag);
BOOL PowerContactTagExists(PowerID idPower, uint32 dwTag);
void SpecificPowerSendMessage(PowerID idPower, PowerMessage eMessage, void * pParam);
float PowerGetAttackRadius(PowerID idPower, const BOOL bUseBonusRange);
void PowerUpdateFacing(const PowerID idPower, void *, BOOL bIsStartOfPower);
uint32 PowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIsAI, BOOL bFreeCast);
BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, const WorldPlace & placeStart, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax);
void PowerUpdateDeletion(Power * ptPower);
void PowerStartDataInit(PowerStartData & tPowerStartData);
BOOL PowerAlternatesAnims(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerShouldUseClientWalking(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp
float PowerGetAttackSpeedAtLevel(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, int32 nSkillLevel, int32 nComboLevel);  // X/Common/Powers/PowersCommon.cpp
void GizmoPower(Power * ptPower, PowerMessage eMessage, void * pParam);  // X/Server/Powers/Power_Gizmo.cpp
void PotionPower(Power * ptPower, PowerMessage eMessage, void * pParam);  // X/Server/Powers/Power_Potion.h
void PowerUseItem(Power * ptPower, PowerMessage eMessage, void * pParam);  // X/Server/Powers/Power_ItemUse.h
PowerID PowerStartEx(const PowerStartData & tPowerStartData, AttribGroupID idFastAttribSource, BOOL bForceStart, BOOL bIgnoreRange);
ACDID PowerGetTargetACD(const Power * ptPower);
int32 PowerGetLevel(const ActorCommonData * ptACD, SNO snoPower);  // X/Common/Powers/PowersCommon.cpp

#include "../../Common/Achievements/AchievementDefines.h"
#include "../../Common/GlobalSNO.h"
#include "../Achievements/AchievementModifierEvaluationLogic.h"
namespace Achievement {
typedef int32 EventID;












}  // namespace Achievement
void PowerGetSpecialDeathTags(AutoPowerDefinitionR pdefPower, RuneType eRune, int32 & nTagDeathType, int32 & nTagChance);  // X/Common/Powers/PowersCommon.cpp:436
void SPlayerUpdateRechargeTimer(ACDID idACDPlayer, SNO snoPower, float flTimeReduction);  // X/Server/SPlayer.cpp
namespace CombatLog { typedef int32 LogEventType; void LogEvent(LogEventType eType, ACDID idACD1, ACDID idACD2, SNO snoData, float flParam1, float flParam2, uint32 dwParam1); }  // X/Common/CombatLog.cpp:610
void ActorPowersRemovePrimary(ActorID idActor, PowerID idPower);  // X/Server/Actors/ActorPowers.cpp
uint32 SPlayerGetPowerChargeTime(Player * ptPlayer, SNO snoPower);  // X/Server/SPlayer.cpp
static const int32 POWER_FACING_UPDATE_RATE = 15;
static const int32 POWER_TIMER_POOL_COUNT = 128;
static const int32 AFFECTED_ACD_POOL_COUNT = 128;
static const int32 SCRIPTINSTANCEID_POOL_COUNT = 128;
static const int32 ATTACKPAYLOAD_POOL_COUNT = 128;
static const int32 CHOOSETARGET_POOL_COUNT = 128;
static const int32 CURSOR_POOL_COUNT = 128;
static const int32 BUFFSTARTDATA_POOL_COUNT = 128;
static const int32 DAMAGE_POOL_COUNT = 16;
#include "../../Common/GizmoSwig.h"
float PowerGetFormulaValueAtLevel(AttribGroupID idFastAttrib, const ActorCommonData * ptACD, SNO snoPower, uint8 * pbFormula, int32 nFormulaSize, int32 nLevel);  /* X/Common/Powers/PowersCommon.cpp */
static const int32 MAX_SCRIPT_IDS = 1024;
static const int32 INT_VALUES_POOL_COUNT = 256;
static const int32 FLOAT_VALUES_POOL_COUNT = 256;
#include "../../Common/ThreadGlobals.h"
static const int32 AREA_DAMAGE_ACCUMULATOR_POOL_COUNT = 256;
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower);  /* X/Common/Powers/PowersCommon.cpp */ void PowerAbortClientWalk(ACDID idACDPlayer, SNO snoPower);  /* Powers.cpp:5156 */ SNO PlayerBrainGetQueuedPower(ACDID idACD);  /* X/Server/AI/PlayerBrain.cpp */ void PlayerBrainClearQueuedPower(ACDID idACD);  /* X/Server/AI/PlayerBrain.cpp */
#include "../Movement/SActorMovement.h"
void ScriptDefinitionDisposeScript(SNO snoScript);  /* X/Server/Definitions/ScriptDefinition.cpp:906 */ void LuaScriptFreeStateWithScriptID(ScriptID idScript);  /* X/Server/LuaScript/LuaScript.cpp */ BOOL LuaScriptScriptIsExecuting(ScriptInstanceID idScript); void LuaScriptInstanceAttachmentDeleted(ScriptInstanceID idScriptInstance);  /* X/Server/LuaScript/LuaScript.cpp:2699, :5425 */ BOOL PowerRefundsResourcesWithoutDamage(AutoPowerDefinitionR pDefPower, const Player * ptPlayer);  /* X/Common/Powers/PowersCommon.cpp */
static const int32 POWER_EMOTE_COOLDOWN_TICKS = 0;
void PowerHelper_RefundCosts(PowerID idPower); void PowerDelete(PowerID idPower);  /* X/Server/Powers/PowerHelper.cpp; Powers.cpp:4056 */ void ActorPowersRemove(ActorID idActor, PowerID idPower); void BrainStanceDoDeathReplacement(ActorID idActor, ActorID idKillerActor);  /* X/Server/Actors/ActorPowers.cpp; X/Server/AI/BrainStance.cpp */ void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);  /* X/Server/AI/BrainComponents.h */ void ActorSetPowerState(ActorID idActor, LPCSTR szState); void BrainUpCallOnPowerStop(ActorID idActor, PowerID idPower, BOOL bAllowAIUpdate);  /* X/Server/Actors/Actors.cpp; X/Server/AI/BrainStance.h */
ACDID ACDInventoryGetActiveWeapon(const ActorCommonData & tACD); BOOL ItemIsBroken(const ActorCommonData & tACD);  /* X/Common/ActorInventory.cpp:5398; X/Common/Items.cpp */ AttribGroupID FastAttribLightCreate(FastAttribType eType);  /* X/Common/Attributes/FastAttribute.cpp:4571 */
#include "../../Sigma/MemoryPools.h"
void ActorPowersInitializeForGame(); void ActorPowersDisposeForGame();  /* X/Server/Actors/ActorPowers.cpp */ void PowerAPI_BuffDeleteAll(); void PowersDeleteAll();  /* X/Server/Powers/PowerAPI.cpp; Powers.cpp:920 */
Angle ACDGetYaw(ACDID idACD); /* X/Common/ActorCommonData.cpp:2997 */ void ACDsCylinderIntersectSphere(const WorldPlace & placeCenter, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // ActorCollision.cpp:349
static const uint32 gdwScriptedPowersUpdateRate = 120;
BOOL PowerAPI_TargetIsWithinArc(WorldPlace placeSource, Vector3D wvVectorToUse, Angle aMaxAngle, ACDID idACDTarget); /* PowerAPI.cpp */ WorldDistance PowerGetDistanceViaWorldPlace(const WorldPlace & placeStart, const WorldPlace & placeTarget, WorldDistance wdStartHeight, const WorldDistance wdStartRadius, WorldDistance wdTargetHeight, const WorldDistance wdTargetRadius, const WorldDistance wdLongReach);  /* X/Common/Powers/PowersCommon.cpp:8290 */ BOOL PowerNeverUpdatesFacingDuring(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, ACDID idACDTarget); BOOL PowerNeverUpdatesFacingStarting(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, ACDID idACDTarget); void PowerFaceTarget(ActorCommonData * ptACD, const Target & tTarget, const uint32 dwAnimationTag, BOOL bSnapToFacing);  /* X/Common/Powers/PowersCommon.cpp:4827, :4857, :5454 */ uint32 MarkerStringHash(LPCSTR pszName); /* Marker.cpp:204 */ void ActorTriggerAttributeUpdate(ActorID idActor); /* X/Server/Actors/Actors.cpp */ BOOL PowersPowerScriptIsBeingUsed(SNO snoScript); /* Powers.cpp:977 */ void SPlayerAssignToEmptyActiveSkillSlot(Player & tPlayer, const SNO snoPower); /* SPlayer.cpp:6483 */ BOOL SHirelingTryChooseSkill(ActorCommonData & tACDHireling, SNO snoPower); /* SHireling.cpp:994 */ int32 PowerGetAutoPurchaseLevel(AutoPowerDefinitionR pdefPower); /* PowersCommon.cpp:1324 */ BOOL PowerComputeDestination(const ACDCollSettings & tCollSettingsDuring, const ACDCollSettings & tCollSettingsDest, const ActorMovement & tMovement, const WorldPlace & placeTarget, const WorldPlace & placeStart, WorldPlace & placeResult, uint32 uRequirePathNoOverlapFlags, float flSearchRadiusMax, PathPoint2DList * ptListPathPoints); /* Powers.cpp:5557 */ void PlayerSetHotbarBtnAssignment(Player * ptPlayer, const SkillButton eButton, const HotbarButtonData & tButtonData); void PlayerSetPotionBtnAssignment(Player * ptPlayer, const HotbarButtonData & tButtonData); /* X/Common/Player.cpp:1778, :1761 */ void SPlayerAssignToActiveSkillSlot(Player & tPlayer, const SNO snoPower, const RuneType eRune, const int32 nSlot, BOOL bApplyCooldown); void SPlayerAssignToTraitSlot(Player & tPlayer, const SNO snoPower, const int32 nSlot, BOOL bApplyCooldown); void SPlayerAssignToLegendaryPowerSlot(Player & tPlayer, GBID gbidPower, int32 nSlot); /* SPlayer.cpp:6118, :6304 */ void HeroEnsureHeroHasAllSkillAttributes(ActorCommonData & tACD, const BOOL fNewHero); /* X/Server/SHero.cpp:2317 */ void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD); /* X/Common/ActorInventory.cpp:9033 */ void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace); /* ActorCommonData.cpp:4668 */ void AnimPlayerGetCurrentAnim(const ActorCommonData & tACD, SNO & snoAnim, int32 & nAnimPermutation); /* X/Common/AnimPlayer.cpp */ uint32 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability); uint32 NavMeshGetDynamicFlagsForPassability(PassabilityType ePassability); /* NavigationMesh.cpp:3180, :3187 */ BOOL NavCellHandleFromWorldPlace(const WorldPlace & tPlace, NavCellHandle & hNavCell); BOOL WorldCollSweptPoint(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Vector2D & wpStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult); /* WorldCommonCollision.cpp:62 */ BOOL WorldCollSearchForLocation(const WorldPlace & placeStart, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, float flLocationRadius, float flSearchRadius, float flSearchRadiusMax, ACDID idACDIgnore, Vector3D & wpEnd, NavCellHandle & hNavCellEnd, Team eTeam, uint16 uObjectNoPassthroughFlags, ACDID idOwner); BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult); /* WorldCommonCollision.cpp:241, :147 */ void FastAttribLightCopy(AttribGroupID idAttribGroupSource, AttribGroupID idAttribGroupDest); /* X/Common/Attributes/FastAttribute.cpp:4615 */ void GameErrorFromPowerFailureReasons(GameErrorData & tData, const uint32 dwFailureReason, const ACDID idACD, AutoPowerDefinitionR pdefPower, const BOOL fItemPower); /* X/Common/Powers/PowersCommon.cpp:5708 */ void SendPlayerCancelWarp(const ACDID idACDPlayer); /* X/Server/Network/ServerMessageSend.cpp:1403 */ PowerID PowerCreate(const ACDID idACD, SNO snoPower, const Target & tTarget); /* Powers.cpp:3930 */ void SGameStatsRecordPowerUse(ActorCommonData & tACD, SNO snoPower, PowerID, ACDID idACDTarget); /* X/Server/SGameStats.cpp:744 */ extern const SymbolicEntry gPowerFailureReasons[]; /* X/Common/Powers/PowersCommon.cpp */ void PowerAPI_BuffMergeAttribsForPowers(ActorCommonData & tACD, SNO snoPower, XPooledDataIDList & listFastAttribs); void PowerAPI_BuffUnmergeAttribsForPowers(ActorCommonData & tACD, XPooledDataIDList & listFastAttribs); /* PowerAPI.cpp:12244, :12301 */ Attrib PowerAttributeFromRuneType(RuneType eRuneType); /* X/Common/Powers/PowersCommon.cpp:8554 */ ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD); /* X/Common/ActorInventory.cpp:5373 */ BOOL ItemIsOneHandedWeapon(const ActorCommonData & tACDItem); /* X/Common/Items.cpp:3422 */ BOOL PowerShouldPlayAnimForPower(ACDID idACD, const Target & tTarget, SNO snoNewPower, SNO snoCurrentPower); /* X/Common/Powers/PowersCommon.cpp:8259 */ void AnimPlayerPlayIdle(ActorCommonData & tACD); /* X/Common/AnimPlayer.cpp:1376 */ void ActorPowersSetPrimary(ActorID idActor, PowerID idPower); void ActorPowersAdd(ActorID idActor, PowerID idPower); /* X/Server/Actors/ActorPowers.cpp:201, :315 */ void PowerSetContactFrame(PowerID idPower, int32 nFrame); void PowerTargetCriteriaInit(PowerID idPower); /* Powers.cpp:3866, :3879 */ uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim); /* X/Common/Powers/PowersCommon.cpp:8013 */ void ACDPlayEffect(const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter, const ACDID idACDOwner); /* X/Server/Actors/ActorEffect.cpp:104 */
static const uint32 DURING_BUFF_MAX_DURATION = 420;
#include "../../Common/Nav/NavCellPath.h"
static XVarScope s_scope("Powers", NULL);
static XVarInt32 s_varMaxAreaEffectTargetCount(s_scope, "MaxAreaEffectTargetCount", 64);
static XVarInt32 s_varMaxProjectileImpactCount(s_scope, "MaxProjectileImpactCount", 64);
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Definitions/AnimationDefinition.h"
#include "../../Common/WorldCommonCollision.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "NintendoSDK/Compilers/NX/nx/aarch64/include/c++/v1/math.h"
#include "../SGameConstants.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/Hyperlink.h"
#include "../LuaScript/ScriptEvent.h"
#include "../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../Common/Actors/ActorEffectCommon.h"















































































































































































































































































































































































































































































































































template <>
uint32 XHashKey<AreaDamageAccumulatorKey>(AreaDamageAccumulatorKey tKey)
{
    uint32 uHashKey = Blizzard::Util::Hash32(tKey.idACDAttacker);
    uHashKey = Blizzard::Util::Hash32More(&tKey.idACDTarget, sizeof(tKey.idACDTarget), uHashKey);
    return uHashKey;
}













void PowersInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    PowerGlobals *& ptPowerGlobals = tSGame.ptPowerGlobals;


    ptPowerGlobals = (PowerGlobals *)SigmaMemoryNew(sizeof(PowerGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    ptPowerGlobals->pArrayPowers = TChainedDataArrayCreateTracked<Power, int>("Powers", tSGame.ptMemoryInterface, INITIAL_POWERS_COUNT);
    ptPowerGlobals->tDamageAttributePool.Initialize();
    ptPowerGlobals->tDamageAttributePool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tPowerTimerPool.Initialize();
    ptPowerGlobals->tPowerTimerPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tPowerTimerPool.SetBlockCount_Tracked(POWER_TIMER_POOL_COUNT);
    ptPowerGlobals->tAffectedACDPool.Initialize();
    ptPowerGlobals->tAffectedACDPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tAffectedACDPool.SetBlockCount_Tracked(AFFECTED_ACD_POOL_COUNT);
    ptPowerGlobals->tScriptInstanceIDPool.Initialize();
    ptPowerGlobals->tScriptInstanceIDPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tScriptInstanceIDPool.SetBlockCount_Tracked(SCRIPTINSTANCEID_POOL_COUNT);
    ptPowerGlobals->tAttackPayloadPool.Initialize();
    ptPowerGlobals->tAttackPayloadPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tAttackPayloadPool.SetBlockCount_Tracked(ATTACKPAYLOAD_POOL_COUNT);
    ptPowerGlobals->tChooseTargetsPool.Initialize();
    ptPowerGlobals->tChooseTargetsPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tChooseTargetsPool.SetBlockCount_Tracked(CHOOSETARGET_POOL_COUNT);
    ptPowerGlobals->tCursorPool.Initialize();
    ptPowerGlobals->tCursorPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tCursorPool.SetBlockCount_Tracked(CURSOR_POOL_COUNT);
    ptPowerGlobals->tBuffStartDataPool.Initialize();
    ptPowerGlobals->tBuffStartDataPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tBuffStartDataPool.SetBlockCount_Tracked(BUFFSTARTDATA_POOL_COUNT);
    ptPowerGlobals->tPowerAPIDamagePool.Initialize();
    ptPowerGlobals->tPowerAPIDamagePool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tPowerAPIDamagePool.SetBlockCount_Tracked(DAMAGE_POOL_COUNT);
    ptPowerGlobals->tDamagePool.Initialize();
    ptPowerGlobals->tDamagePool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tDamageAttributePool.SetBlockCount_Tracked(DAMAGE_POOL_COUNT);
    ptPowerGlobals->tBuffInstancePool.Initialize();
    ptPowerGlobals->tBuffInstancePool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->pArrayBuffs = TChainedDataArrayCreateTracked<PowerAPIBuffInstance, int>("Buffs", tSGame.ptMemoryInterface, INITIAL_BUFFS_COUNT);
    ptPowerGlobals->listBuffsFlaggedForDeletion.Initialize(MemoryPoolsGetDATAIDListPool());
    ptPowerGlobals->tACDSortedTargetPool.Initialize();
    ptPowerGlobals->tACDSortedTargetPool.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptPowerGlobals->tScriptIDPool.Initialize();
    ptPowerGlobals->tScriptIDPool.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptPowerGlobals->tScriptIDPool.SetBlockCount_Tracked(MAX_SCRIPT_IDS);
    ptPowerGlobals->tScriptIDs.Initialize(&ptPowerGlobals->tScriptIDPool);
    ptPowerGlobals->tMapFloatValuesPool.Initialize();
    ptPowerGlobals->tMapFloatValuesPool.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptPowerGlobals->tMapFloatValuesPool.SetBlockCount_Tracked(FLOAT_VALUES_POOL_COUNT);
    ptPowerGlobals->tMapIntValuesPool.Initialize();
    ptPowerGlobals->tMapIntValuesPool.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptPowerGlobals->tMapIntValuesPool.SetBlockCount_Tracked(INT_VALUES_POOL_COUNT);

    ptPowerGlobals->tMapAreaDamageAccumulatorsPool.Initialize();
    ptPowerGlobals->tMapAreaDamageAccumulatorsPool.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptPowerGlobals->tMapAreaDamageAccumulatorsPool.SetBlockCount_Tracked(AREA_DAMAGE_ACCUMULATOR_POOL_COUNT);
    ptPowerGlobals->tMapAreaDamageAccumulators.Initialize(&ptPowerGlobals->tMapAreaDamageAccumulatorsPool);

    ptPowerGlobals->nMaxAreaEffectTargetCount = 64;
    ptPowerGlobals->nMaxProjectileImpactCount = 64;




    UpdateFunctionInitialize(UPDATE_SCRIPTED_POWERS, -1, ptPowerGlobals->tScriptedPowerUpdateData);
    ServerQueueUpdateFunction(gdwScriptedPowersUpdateRate, ptPowerGlobals->tScriptedPowerUpdateData);















    ActorPowersInitializeForGame();
}




void PowersDisposeForGame()
{
    PowerAPI_BuffDeleteAll();
    PowersDeleteAll();

    ActorPowersDisposeForGame();

    PowerGlobals *& ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

    POS pos = ptPowerGlobals->tScriptIDs.GetStartAssoc();
    while (pos)
    {
        ScriptID idScript;
        SNO snoScript;
        ptPowerGlobals->tScriptIDs.GetNextAssoc(pos, snoScript, idScript);
        LuaScriptFreeStateWithScriptID(idScript);
    }

    ptPowerGlobals->tScriptIDs.Dispose();
    ptPowerGlobals->tScriptIDPool.Dispose();

    TChainedDataArrayDispose(ptPowerGlobals->pArrayPowers);
    ptPowerGlobals->tDamageAttributePool.Dispose();
    ptPowerGlobals->tAffectedACDPool.Dispose();
    ptPowerGlobals->tPowerTimerPool.Dispose();
    ptPowerGlobals->tBuffInstancePool.Dispose();
    ptPowerGlobals->listBuffsFlaggedForDeletion.Dispose();
    TChainedDataArrayDispose(ptPowerGlobals->pArrayBuffs);
    ptPowerGlobals->tACDSortedTargetPool.Dispose();
    ptPowerGlobals->tScriptInstanceIDPool.Dispose();
    ptPowerGlobals->tAttackPayloadPool.Dispose();
    ptPowerGlobals->tCursorPool.Dispose();
    ptPowerGlobals->tBuffStartDataPool.Dispose();
    ptPowerGlobals->tChooseTargetsPool.Dispose();
    ptPowerGlobals->tDamagePool.Dispose();
    ptPowerGlobals->tPowerAPIDamagePool.Dispose();
    ptPowerGlobals->tMapIntValuesPool.Dispose();
    ptPowerGlobals->tMapFloatValuesPool.Dispose();
    ptPowerGlobals->tMapAreaDamageAccumulatorsPool.Dispose();





    {
        Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tPowerDefPurgeMutex);
        sgtAppGlobals.ptPowerDefsPurged->EmptyArray();
    }
    ServerRemoveUpdateFunction(ptPowerGlobals->tScriptedPowerUpdateData);









    SigmaMemoryFree(ptPowerGlobals, SGameManager::GameGet().ptMemoryInterface); ptPowerGlobals = NULL;
}



void PowersUpdateScriptedPowers()
{


    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    ServerUpdateWasCalled(ptPowerGlobals->tScriptedPowerUpdateData);





    {
        Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tPowerDefPurgeMutex);

        int32 nCount = sgtAppGlobals.ptPowerDefsPurged->GetArraySize();
        for (int32 i = 0; i < nCount; i++)
        {
            SNO snoScript = (*sgtAppGlobals.ptPowerDefsPurged)[i];
            BOOL bPowerScriptBeingUsed = PowersPowerScriptIsBeingUsed(snoScript);
            if (!bPowerScriptBeingUsed)
            {
                ScriptID idScript;
                if (ptPowerGlobals->tScriptIDs.Lookup(snoScript, idScript))
                {
                    LuaScriptFreeStateWithScriptID(idScript);
                    ptPowerGlobals->tScriptIDs.RemoveAt(snoScript);
                }

                ScriptDefinitionDisposeScript(snoScript);
            }
        }



        sgtAppGlobals.ptPowerDefsPurged->EmptyArray();
    }


    ServerQueueUpdateFunction(gdwScriptedPowersUpdateRate, ptPowerGlobals->tScriptedPowerUpdateData);
}





static void sPowerDeleteAllTimers(Power * ptPower)
{
    if (TimerExists(ptPower->idTimerContact))
    {
        TimerUnregister(ptPower->idTimerContact);
        ptPower->idTimerContact = TIMERID_INVALID;
    }
    if (TimerExists(ptPower->idTimerInterrupt))
    {
        TimerUnregister(ptPower->idTimerInterrupt);
        ptPower->idTimerInterrupt = TIMERID_INVALID;
    }
    if (TimerExists(ptPower->idTimerActorDone))
    {
        TimerUnregister(ptPower->idTimerActorDone);
        ptPower->idTimerActorDone = TIMERID_INVALID;
    }

    while (!ptPower->listTimers.IsEmpty())
    {
        const PowerTimer & tPowerTimer = ptPower->listTimers.GetHead();
        TimerID idTimer = tPowerTimer.idTimer;
        ptPower->listTimers.RemoveHead();
        TimerUnregister(idTimer);
    }

    PowerCheckForDone(ptPower->id);
}






















void PowersDeleteAll()
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    TDataArrayIterator<Power> tIt;
    while (Power * ptPower = ptPowerGlobals->pArrayPowers->Iterate(tIt))
    {
        sPowerDeleteAllTimers(ptPower);
    }

    ptPowerGlobals->pArrayPowers->DeleteAll();
}






void PowersForceStopForWorld(SWorldID idSWorld)
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    TDataArrayIterator<Power> tIt;
    while (Power * ptPower = ptPowerGlobals->pArrayPowers->Iterate(tIt))
    {
        if (ptPower->wpStartLocation.idSWorld == idSWorld)
        {
            if (!PowerGeneralTagGetBool(AutoPowerDefinitionR(ptPower->snoPower), TAG_POWER_PERSISTS_ON_WORLD_DELETE))
                PowerForceStop(ptPower->id, TRUE);
        }
    }
}



























BOOL PowersPowerScriptIsBeingUsed(SNO snoScript)
{

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    TDataArrayIterator<Power> tIt;
    while (Power * ptPower = ptPowerGlobals->pArrayPowers->Iterate(tIt))
    {

        if (ptPower->snoScript == snoScript)
        {
            return TRUE;
        }
    }
    return FALSE;
}







BOOL PowerUpdateTarget(PowerID idPower, const UpdateTargetData * ptUpdateTargetData)
{
    SpecificPowerSendMessage(idPower, POWER_UPDATE_TARGET, (void *)ptUpdateTargetData);
    return TRUE;
}







void PowerGetACDPoint(PowerID idPower, const ACDID idACD, NavPlace & tNavPlace, SNO snoAnim, int32 nAnimPerm, Vector3D * pvOffset)
{






    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return;
    }


    ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);

    Vector3D wvProjectileOffset(0.0f, 0.0f, 0.0f);

    ContactFrameType eContactFrameType = PowerGetContactFrameType(AutoPowerDefinitionR(ptPower->snoPower), ptPower->dwCurrentContactFrame);
    switch (eContactFrameType)
    {
    case 2:
        snoAnim = ptPower->snoContactAnimation[1];
        nAnimPerm = ptPower->nContactAnimationPermutation[1];
        break;
    case 0:
        snoAnim = ptPower->snoContactAnimation[0];
        nAnimPerm = ptPower->nContactAnimationPermutation[0];
        break;
    case 1:
        snoAnim = ptPower->snoContactAnimation[2];
        nAnimPerm = ptPower->nContactAnimationPermutation[2];
        break;
    }



    if (snoAnim == SNO_NONE)
    {
        AnimPlayerGetCurrentAnim(*ptACD, snoAnim, nAnimPerm);
    }

    if (snoAnim != SNO_NONE && nAnimPerm >= 0)
    {
        const AnimationDefinition * ptAnimDef = (const AnimationDefinition *)gptGroupDefAnimation->LockR(snoAnim, FALSE);

        wvProjectileOffset = ptAnimDef->ptPermutations[nAnimPerm].wpContactOffset[ptPower->dwCurrentContactFrame];
        wvProjectileOffset *= ptACD->tLocation.tWorldLocation.flScale;

        gptGroupDefAnimation->UnlockR((void *&)ptAnimDef);
    }
    else
    {

        ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
        wvProjectileOffset = pdefActor->wpLocationPowerSrc;
    }

    if (Math::Abs(wvProjectileOffset.x) > EPSILON ||
        Math::Abs(wvProjectileOffset.y) > EPSILON)
    {
        Quaternion qOrientation = ptACD->tLocation.tWorldLocation.transform.q;
        float flZOffset = wvProjectileOffset.z;
        wvProjectileOffset.z = 0.0f;
        wvProjectileOffset = qOrientation.Rotate(wvProjectileOffset);

        WorldCollParams tCollParams;
        tCollParams.uOneRequiredStaticFlags = NavMeshGetStaticFlagsForPassability(2);
        tCollParams.uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(2);
        tCollParams.eTeam = 1;
        NavCellHandle hNavCellCenter;
        NavCellHandleFromWorldPlace(tNavPlace.tPlace, hNavCellCenter);
        Vector3D vStart(tNavPlace.tPlace.wp);
        Vector3D vDest = vStart + wvProjectileOffset;

        WorldCollResult tResult;
        WorldCollSweptPoint(tNavPlace.tPlace.idSWorld, hNavCellCenter, vStart.As2D(), vDest.As2D(), tCollParams, 0, 0, idACD, tResult);










        tNavPlace.tPlace.wp = tResult.wpEnd;
        tNavPlace.tPlace.wp.z = vStart.z + flZOffset;
        ScenesCommonNavMeshNodeFromPlace(tNavPlace.tPlace, &tNavPlace.tNavMeshHandle, FALSE, FALSE);

        wvProjectileOffset.z = flZOffset;
    }
    else
    {
        WorldPlaceOffset(tNavPlace.tPlace, wvProjectileOffset, tNavPlace.tPlace);
    }

    if (pvOffset)
    {
        *pvOffset = wvProjectileOffset;
    }
}




ACDID PowerGetTargetACD(const PowerID idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    return PowerGetTargetACD(ptPower);
}




ACDID PowerGetTargetACD(const Power * ptPower)
{
    if (ptPower && ptPower->tTarget.IsTargetingACD())
    {
        if (ACDTryToGet(ptPower->tTarget.idACDTarget))
            return ptPower->tTarget.idACDTarget;
    }
    return ACDID_INVALID;
}











BOOL PowerGetTarget(const PowerID idPower, Target & tTarget)
{
    Power * ptPower = PowerGet(idPower);
    tTarget = ptPower->tTarget;
    return TRUE;
}









AttribGroupID PowerGetAttributeGroup(const PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    return ptPower->idFastAttrib;
}






BOOL PowerTargetAllies(PowerID idPower)
{
    return PowerContactTagGetBool(idPower, TAG_POWER_TARGET_ALLIES);
}





BOOL PowerTargetEnemies(PowerID idPower)
{
    return PowerContactTagGetBool(idPower, TAG_POWER_TARGET_ENEMIES);
}





BOOL PowerTargetNeutral(PowerID idPower)
{
    return PowerContactTagGetBool(idPower, TAG_POWER_TARGET_NEUTRAL);
}





BOOL PowerTargetNecroCorpsesOnly(PowerID idPower)
{
    return PowerContactTagGetBool(idPower, TAG_POWER_TARGET_NECRO_CORPSES_ONLY);
}




BOOL PowerIsPhysical(PowerID idPower)
{
    return PowerContactTagGetBool(idPower, TAG_POWER_IS_PHYSICAL);
}





BOOL PowerTargetCorpse(PowerID idPower)
{
    return PowerContactTagGetBool(idPower, TAG_POWER_TARGET_CORPSE);
}




BOOL PowerHasEscapeRange(const Power & tPower)
{

    float flEscapeRange = PowerGetFormulaValue(tPower.id, TAG_POWER_ESCAPE_ATTACK_RADIUS);
    if (flEscapeRange > 1e-6f)
    {
        if (PowerGetTemplate(AutoPowerDefinitionR(tPower.snoPower), tPower.idACD) == ANIMATION_POWER)
            return TRUE;
    }
    return FALSE;
}











BOOL sPowerCanStart(ActorID idActor, const PowerDefinition * pdefPower, const PowerStartData & tPowerStartData)
{
    ActorCommonData * ptACD = ACDTryToGet(ActorGet(idActor)->idACD);
    const Target * ptTarget = &tPowerStartData.tTarget;
    uint32 dwFailureMask = ~((1 << PS_CANCELABLE_SKILLS_ACTIVE) | (1 << PS_NOT_CLOSE_ENOUGH_BIT) | (1 << PS_NEED_GENERAL_COOLDOWN_BIT));
    if (PowerIsPassive(pdefPower))
        dwFailureMask &= ~((1 << PS_NOT_IN_TOWN) | (1 << PS_IMMOBILIZED) | (1 << PS_FEARED) | (1 << PS_SILENCED));
    if (PowerCanUseWhenFeared(ptACD, pdefPower))
        dwFailureMask &= ~(1 << PS_FEARED);
    if (PowerPlayerCanUseWhenDead(pdefPower))
        dwFailureMask &= ~(1 << PS_CASTER_IS_DEAD);
    if (!PowerIsPrimary(pdefPower, ptACD->id))
        dwFailureMask &= ~(1 << PS_UNCANCELABLE_SKILLS_ACTIVE);
    if (tPowerStartData.bFreeCast)
        dwFailureMask &= ~((1 << PS_NEED_COOLDOWN_BIT) | (1 << PS_NEED_RESOURCE_BIT));
    if (tPowerStartData.bDoesntAnimate)
        dwFailureMask &= ~(1 << PS_UNCANCELABLE_SKILLS_ACTIVE);
    if (PowerIsGizmo(pdefPower))
        dwFailureMask |= (1 << PS_NOT_CLOSE_ENOUGH_BIT);
    return (PowerFailureReasons(*ptACD, pdefPower, ptTarget, tPowerStartData.bFreeCast, FALSE) & dwFailureMask) == 0;
}












































void PowerDetachFromActor(PowerID idPower, BOOL bFromForceStop)
{


    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;



    ActorID idPowerActor = ActorID(ACDGetOwner(ptPower->idACD));
    const Actor * ptActor = ActorTryToGet(idPowerActor);
    if (!ptActor)
        return;





    BOOL bWeArePrimaryPower = ActorPowersGetPrimary(idPowerActor) == idPower;




    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);

    if (PowerIsComboPower(AutoPowerDefinitionR(ptPower->snoPower)) || PowerRefreshesComboLevel(AutoPowerDefinitionR(ptPower->snoPower)))
    {
        if (bWeArePrimaryPower && ptACD)
        {
            ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_COMBO_TIME_LAST_MOVE), GameGetServerTime());
        }
    }

    if (!PowerIsInterruptable(idPower))
    {
        ptPower->dwFlags |= (1 << PF_CANCEL);
        return;
    }

    ActorPowersRemovePrimary(idPowerActor, idPower);

    if (ptACD)
    {
        PowerClearDuringBuffs(ptPower, ptACD);
    }

    if (bWeArePrimaryPower)
    {
        ActorSetPowerState(idPowerActor, "-");
        SpecificPowerSendMessage(idPower, POWER_ACTOR_DONE, (void *)-1);



        BOOL bAllowAIUpdate = !bFromForceStop;
        BrainUpCallOnPowerStop(idPowerActor, idPower, bAllowAIUpdate);
    }
}



void PowerClearDuringBuffs(const Power * ptPower, const ActorCommonData * ptACD)
{





    const PowerID idPower = ptPower->id;


    if (PowerIsUninterruptableDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_UNINTERRUPTIBLE_DURING_BUFF);
        BuffID idUninterruptibleBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idUninterruptibleBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idUninterruptibleBuff, FALSE, TRUE, 0, TRUE);
        }
    }

    if (PowerIsInvulnerableDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_INVULNERABLE_DURING_BUFF);
        BuffID idInvulnerableBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idInvulnerableBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idInvulnerableBuff, FALSE, TRUE, 0, TRUE);
        }
    }

    if (PowerIsInvisibleDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_INVISIBLE_DURING_BUFF);
        BuffID idInvisBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idInvisBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idInvisBuff, FALSE, TRUE, 0, TRUE);
        }
    }


    if (PowerIsUntargetableDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_UNTARGETABLE_DURING_BUFF);
        BuffID idUntargetableBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idUntargetableBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idUntargetableBuff, FALSE, TRUE, 0, TRUE);
        }
    }

    if (PowerIsImmuneToStunDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_STUN_DURING_BUFF);
        BuffID idImmuneToStunDuringBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idImmuneToStunDuringBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idImmuneToStunDuringBuff, FALSE, TRUE, 0, TRUE);
        }
    }

    if (PowerIsImmuneToFearDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_FEAR_DURING_BUFF);
        BuffID idImmuneToFearDuringBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idImmuneToFearDuringBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idImmuneToFearDuringBuff, FALSE, TRUE, 0, TRUE);
        }
    }

    if (PowerIsImmuneToSnareDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_SNARE_DURING_BUFF);
        BuffID idImmuneToSnareDuringBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idImmuneToSnareDuringBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idImmuneToSnareDuringBuff, FALSE, TRUE, 0, TRUE);
        }
    }

    if (PowerIsImmuneToRootDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_ROOT_DURING_BUFF);
        BuffID idImmuneToRootDuringBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idImmuneToRootDuringBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idImmuneToRootDuringBuff, FALSE, TRUE, 0, TRUE);
        }
    }

    if (PowerCannotDieDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
    {
        SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_CANNOT_DIE_DURING_BUFF);
        BuffID idCannotDieDuringBuff = PowerAPI_BuffFindOnACDByPowerID(snoBuff, ptACD, idPower);
        if (idCannotDieDuringBuff != -1)
        {
            PowerAPI_BuffFlagForDeletion(idCannotDieDuringBuff, FALSE, TRUE, 0, TRUE);
        }
    }
}

void PowerForceStop(PowerID idPower, BOOL bIgnoreContactFrame)
{
    Power * ptPower;
    ActorCommonData * ptACD;
    ptPower = PowerTryToGet(idPower);






    if (!ptPower || (!bIgnoreContactFrame && ptPower->dwLastContactTick))
        return;



    if (ptPower->dwFlags & (1 << PF_STOPPING))
        return;




    ptPower->dwFlags |= (1 << PF_STOPPING);
    ptPower->dwFlags &= ~(1 << PF_MANUALLY_FINISH);
    ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);

    SpecificPowerSendMessage(idPower, POWER_INTERRUPT, NULL);

    ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD)
    {

        PowerClearDuringBuffs(ptPower, ptACD);
    }


    POS posBuffs = ptPower->listBuffInstances.GetHeadPosition();
    while (posBuffs)
    {
        BuffID idBuff = ptPower->listBuffInstances.GetNext(posBuffs);
        PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
    }

    POS pos = ptPower->listScriptInstanceIDs.GetHeadPosition();

    while (pos)
    {
        ScriptInstanceID idScript = ptPower->listScriptInstanceIDs.GetNext(pos);

        if (LuaScriptScriptIsExecuting(idScript))
        {
            LuaScriptInstanceAttachmentDeleted(idScript);
        }
        else
        {
            LuaScriptCloseInstance(idScript);
        }
    }

    ptPower->listScriptInstanceIDs.RemoveAll();

    sPowerDeleteAllTimers(ptPower);

    PowerDetachFromActor(idPower, TRUE);
}




void PowerInterrupt(PowerID idPower, BOOL bIgnoreContactFrame)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;
    if (!bIgnoreContactFrame && ptPower->dwLastContactTick)
        return;
    if (ptPower->dwFlags & (1 << PF_INTERRUPTED))
        return;
    ptPower->dwFlags |= (1 << PF_INTERRUPTED);
    SpecificPowerSendMessage(idPower, POWER_INTERRUPT, NULL);
    if (TimerExists(ptPower->idTimerInterrupt))
    {
        TimerUnregister(ptPower->idTimerInterrupt);
        ptPower->idTimerInterrupt = TIMERID_INVALID;
    }
    ptPower->dwFlags &= ~(1 << PF_MANUALLY_FINISH);
    ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD)
    {
        PowerClearDuringBuffs(ptPower, ptACD);
        if (PowerShouldUseClientWalking(*ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
            PowerAbortClientWalk(ptACD->id, ptPower->snoPower);
        if (!ACDIsPlayer(ptACD->id) && PowerIsCoolDownDone(*ptACD, ptPower->snoPower))
            PowerHelper_AddCooldown(idPower);
    }
    PowerDetachFromActor(idPower, FALSE);
}































BOOL sPowerHasValidSource(const ActorID idActor)
{


    const Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor)
        return FALSE;

    return TRUE;
}






uint32 PowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIsAI, BOOL bFreeCast)
{




    uint32 dwRetval = 0;
    ActorID idActor = (ActorID)tACD.idOwner;












    if (!sPowerHasValidSource(idActor))
    {
        // The failure-reason bit names are not in the DWARF.
        return 0x400;
    }

    PowerID idPower = ActorPowersGetPrimary(idActor);
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower && PowerIsPrimary(AutoPowerDefinitionR(pdefPower), tACD.id))
        dwRetval = 1;




    if (!PowerCanTarget(AutoPowerDefinitionR(pdefPower), tACD.id, ptTarget))
    {
        dwRetval |= 0x200;
    }


    if (!bIsAI)
    {



        if (!PowerActorHasResourceMinCostToStart(tACD, AutoPowerDefinitionR(pdefPower)))
        {
            dwRetval |= 0x80;
        }
    }


    dwRetval |= PowerACDCanStartPowerFailureReasons(tACD, AutoPowerDefinitionR(pdefPower), ptTarget, bIsAI, bFreeCast);


    return dwRetval;
}


BOOL PowerCanUseNow(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, uint32 * pdwFailureReasons)
{
    uint32 dwFailureReasons;
    if (pdwFailureReasons)
        dwFailureReasons = *pdwFailureReasons;
    else
        dwFailureReasons = PowerFailureReasons(tACD, AutoPowerDefinitionR(pdefPower), ptTarget, FALSE, FALSE);

    // The failure-reason bit names are not in the DWARF; these five bits do not block use.
    const uint32 dwIgnoredReasons = 0x0008020D;

    return (dwFailureReasons & ~dwIgnoredReasons) == 0;
}





















void PowerSetCoolDown(ActorCommonData & tACD, const uint32 dwCooldown)
{
    tACD.AttributesSetInt(ATTRIBUTE_GENERAL_COOLDOWN, dwCooldown);
}






void PowerTriggerCooldown(ActorCommonData * ptACD, SNO snoPower, float flOverridenCooldown)
{
    if (ptACD && snoPower != SNO_NONE)
    {
        uint32 dwCooldownTicks = PowerGetCastingCooldown(AutoPowerDefinitionR(snoPower), *ptACD);
        if (flOverridenCooldown > 1e-6f)
            dwCooldownTicks = Math::FloatToInt(flOverridenCooldown * TICKS_PER_SECOND);
        PowerHelper_AddCooldown(*ptACD, snoPower, dwCooldownTicks, FALSE);
    }
}










// NON_MATCHING: 464 vs 472 bytes; same calls, constants and branches (code on DWARF lines). Register allocation only: the original spills snoPowersCooldown with -nReductionInTicks (stp w22, w8) and copies the 0x165 key through w22 around the set path; ours rematerialises the key. Tried the old continue-style form and this block form.
void PowerReduceCooldown(
    ActorCommonData * ptACD,
    SNO snoPower,
    int32 nReductionInTicks)
{
    if (!ptACD || snoPower == SNO_NONE || nReductionInTicks <= 0)
        return;













    SNO snoPowersCooldown = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);

        if (ptBuff && !ptBuff->bDeleted)
        {
            uint32 dwCooldownEndTick = FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower));
            uint32 dwCooldownStartTick = FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, snoPower));
            // (uint32)-1 is DWORD_NONE (Sigma.h:331 in the DWARF); src Sigma.h does not define it.
            if (dwCooldownEndTick != (uint32)-1 && ptBuff->snoPower == snoPowersCooldown)
            {

                uint32 dwNow = GameGetServerTime();
                uint32 dwNewCooldownEndTick = dwCooldownEndTick - nReductionInTicks;
                if (dwNewCooldownEndTick < dwNow)
                {
                    PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, FALSE, TRUE);
                }
                else
                {
                    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower), dwNewCooldownEndTick);
                    if (dwCooldownStartTick != (uint32)-1 && dwCooldownStartTick >= nReductionInTicks)
                    {
                        FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, snoPower), dwCooldownStartTick - nReductionInTicks);
                    }

                    PowerAPI_BuffModifyDuration(idBuff, -nReductionInTicks, FALSE, TRUE);
                }
            }
        }
    }

    SPlayerUpdateRechargeTimer(ptACD->id, snoPower, (float)nReductionInTicks / TICKS_PER_SECOND);
}




void PowerReduceCooldownByPercentage(
    ActorCommonData * ptACD,
    SNO snoPower,
    float flReductionPct)
{
    if (!ptACD || snoPower == SNO_NONE || flReductionPct <= 0.000001f)
        return;












    // Clamp and test share line 1827 in the line table (fcsel); written as one line.
    if (flReductionPct - 1.0f > 0.000001f) flReductionPct = 1.0f;




    uint32 dwRechargeReductionInTicks = 0;

    SNO snoPowersCooldown = GlobalSNOGet(GLOBALSNO_POWERS_COOLDOWN);
    POS pos = ptACD->listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = ptACD->listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);

        if (ptBuff && !ptBuff->bDeleted && ptBuff->snoPower == snoPowersCooldown)
        {
            uint32 dwCooldownEndTick = FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower));
            uint32 dwCooldownStartTick = FastAttribGetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, snoPower));
            // (uint32)-1 is DWORD_NONE (Sigma.h:331 in the DWARF); src Sigma.h does not define it.
            if (dwCooldownEndTick != (uint32)-1)
            {
                uint32 dwReductionInTicks = Math::FloatToInt(flReductionPct * (dwCooldownEndTick - dwCooldownStartTick));
                if (dwReductionInTicks > dwRechargeReductionInTicks) dwRechargeReductionInTicks = dwReductionInTicks;  // one row, csel (no Math::Max inlined in the DWARF)




                uint32 dwNow = GameGetServerTime();
                uint32 dwNewCooldownEndTick = dwCooldownEndTick - dwReductionInTicks;
                if (dwNewCooldownEndTick < dwNow)
                {
                    PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, FALSE, TRUE);
                }
                else
                {
                    FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower), dwNewCooldownEndTick);
                    if (dwCooldownStartTick != (uint32)-1 && dwCooldownStartTick >= dwReductionInTicks)
                    {
                        FastAttribSetValueInt(ptBuff->idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, snoPower), dwCooldownStartTick - dwReductionInTicks);
                    }

                    PowerAPI_BuffModifyDuration(idBuff, -dwReductionInTicks, FALSE, TRUE);
                }
            }
        }
    }

    int32 nMaxCharges = PowerActorGetMaxCharges(*ptACD, snoPower);
    if (nMaxCharges)
    {
        Player * ptPlayer = PlayerGetByACD(ptACD->id);
        if (ptPlayer)
        {
            dwRechargeReductionInTicks = Math::FloatToInt(flReductionPct * SPlayerGetPowerChargeTime(ptPlayer, snoPower));
        }

        if (dwRechargeReductionInTicks)
        {
            SPlayerUpdateRechargeTimer(ptACD->id, snoPower, (float)dwRechargeReductionInTicks / TICKS_PER_SECOND);
        }
    }


}




BOOL PowerIsAssigned(SNO snoPower, Player * ptPlayer)
{
    if (!ptPlayer)
        return FALSE;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;

    for (int32 i = 0; i < 6; i++)
    {
        if (ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == snoPower)
            return TRUE;
    }

    for (int32 i = 0; i < 4; i++)
    {
        if (ptPlayer->tHeroStateData.tSavedData.snoTraits[i] == snoPower)
            return TRUE;
    }

    SNO snoBasicAttack = ptACDPlayer->AttributeGetBasicAttack();
    if (snoBasicAttack != SNO_NONE && snoBasicAttack == snoPower)
        return TRUE;

    return FALSE;
}



















float PowerGetResourceCost(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
    return PowerGetFormulaValue(NULL, AutoPowerDefinitionR(pdefPower), TAG_POWER_COST_RESOURCE, ptPower->nSkillLevel, ptPower->idFastAttrib, 0);
}





float PowerGetContinualResourceCost(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
    return PowerGetFormulaValue(NULL, AutoPowerDefinitionR(pdefPower), TAG_POWER_COST_CONTINUAL_RESOURCE, ptPower->nSkillLevel, ptPower->idFastAttrib, 0);
}




float PowerGetRange(PowerID idPower, const BOOL bUseBonusRange)
{
    return PowerGetAttackRadius(idPower, bUseBonusRange);
}






float PowerGetAttackRadius(PowerID idPower, const BOOL bUseBonusRange)
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    return PowerGetAttackRadiusAtLevel(ptACD, AutoPowerDefinitionR(ptPower->snoPower), ptPower->nSkillLevel, bUseBonusRange);
}


























float PowerGetAttackSpeed(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptPower && ptACD)
    {
        return PowerGetAttackSpeedAtLevel(*ptACD, AutoPowerDefinitionR(ptPower->snoPower), ptPower->nSkillLevel, -1);
    }
    return 1.0f;
}





static void sAutoPurchaseStartingActiveSkills(ActorCommonData & tACD, const SkillKitDefinition * pdefSkillKit)
{






    Player * ptPlayer = PlayerGetByACD(tACD.id);

    int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    for (int32 i = 0; i < nCount; i++)
    {
        const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        SNO snoPower = tEntry.snoPower;
        if (snoPower == SNO_NONE)
        {
            continue;
        }

        if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower)) > 0)
        {
            continue;
        }

        int32 nAutoPurchaseLevel = PowerGetAutoPurchaseLevel(AutoPowerDefinitionR(snoPower));
        if (nAutoPurchaseLevel > 0)
        {
            if (ptPlayer)
            {
                tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);
                if (PowerIsDisplayed(AutoPowerDefinitionR(snoPower)))
                {
                    SPlayerAssignToEmptyActiveSkillSlot(*ptPlayer, snoPower);
                }
            }
            else
            {
                SHirelingTryChooseSkill(tACD, snoPower);
            }
        }
    }
}



static void sAutoPurchaseStartingTraits(ActorCommonData & tACD, const SkillKitDefinition * pdefSkillKit)
{




    int32 nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
    for (int32 i = 0; i < nCount; i++)
    {
        const TraitEntry & tEntry = pdefSkillKit->arTraitEntries[i];
        SNO snoPower = tEntry.snoPower;
        if (snoPower == SNO_NONE)
            continue;
        int32 nAutoPurchaseLevel = PowerGetAutoPurchaseLevel(AutoPowerDefinitionR(snoPower));
        if (nAutoPurchaseLevel > 0)
        {
            tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);
        }
    }
}




void PowerActorAutoPurchaseStartingSkillsAndTraits(ActorCommonData & tACD)
{
    SNO snoSkillKit = tACD.GetSkillKit();
    if (snoSkillKit == SNO_NONE)
        return;




    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    sAutoPurchaseStartingActiveSkills(tACD, pdefSkillKit);
    sAutoPurchaseStartingTraits(tACD, pdefSkillKit);
}



void PowerActorAutoPurchaseAlwaysKnownTraits(ActorCommonData & tACD)
{
    SNO snoSkillKit = tACD.GetSkillKit();
    if (snoSkillKit == SNO_NONE)
        return;







    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);

    int32 nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
    for (int32 i = 0; i < nCount; i++)
    {
        SNO snoPower = pdefSkillKit->arTraitEntries[i].snoPower;
        if (snoPower == SNO_NONE)
            continue;

        if (PowerGeneralTagGetBool(AutoPowerDefinitionR(snoPower), TAG_POWER_ALWAYS_KNOWN))
        {
            tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);
        }
    }
}


Team PowerGetTeam(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    return ptPower->eTeam;
}





PayloadType PowerGetPayload(SNO snoPower, uint32 nContactFrame)
{
    PayloadType ePayload = PowerContactTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_PAYLOAD_TYPE, nContactFrame);
    if (ePayload == -1)
        ePayload = 1;
    return ePayload;
}











PayloadType PowerGetPayload(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    return PowerGetPayload(ptPower->snoPower, ptPower->dwCurrentContactFrame);
}
























float PowerGetPayloadParam0(PowerID idPower)
{
    return PowerGetFormulaValue(idPower, TAG_POWER_PAYLOAD_PARAM_0);
}




float PowerGetPayloadParam1(PowerID idPower)
{
    return PowerGetFormulaValue(idPower, TAG_POWER_PAYLOAD_PARAM_1);
}





BOOL PowerIsValid(SNO snoPower)
{

    return SNOFileExists(POWER_SNO, snoPower);
}



SNO PowerGetSNO(PowerID idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
        return ptPower->snoPower;
    return SNO_NONE;
}



ActorCommonData * PowerGetOwner(PowerID idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
        return ACDTryToGet(ptPower->idACD);
    return NULL;
}


ActorCommonData * PowerGetOwner(const Power * ptPower)
{
    ActorCommonData * pRet = NULL;
    if (ptPower)
        pRet = ACDTryToGet(ptPower->idACD);
    return pRet;
}









ACDID PowerGetAlternateSource(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    if (ACDTryToGet(ptPower->idACDAlternateSource))
        return ptPower->idACDAlternateSource;
    return ACDID_INVALID;
}










void PowerSetAlternateSource(PowerID idPower, ACDID idACDSource)
{


    Power * ptPower = PowerGet(idPower);

    if (ACDTryToGet(idACDSource))
    {
        ptPower->idACDAlternateSource = idACDSource;
    }
}





BOOL PowerIsInterruptable(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    return ptPower->dwFlags & (1 << PF_INTERRUPTABLE);
}






void SpecificPowerSendMessage(PowerID idPower, PowerMessage eMessage, void * pParam)
{
    Power * ptPower = PowerGet(idPower);

    const PowerDefinition * pPowerDef = (const PowerDefinition *)gptGroupDefPower->LockR(ptPower->snoPower, FALSE);
    SNO snoPowerScript = pPowerDef->snoScript;
    gptGroupDefPower->UnlockR((void *&)pPowerDef);

    switch (ptPower->ePowerTemplate)
    {
    case WALK_POWER:
    case ANIMATION_POWER:
        if (snoPowerScript != SNO_NONE)
            ScriptedAnimationPowerMessageHandler(ptPower, eMessage, pParam);
        else
            AnimationPower(ptPower, eMessage, pParam);
        break;
    case GIZMO_POWER:
        GizmoPower(ptPower, eMessage, pParam);
        break;
    case HEARTBEAT_POWER:
    case ONSLAY_POWER:
    case ONGETHIT_POWER:
    case ONCRITICAL_POWER:
    case ONCREATE_POWER:
    case PROC_POWER:
    case SECONDARY_ANIMATION_POWER:
        if (snoPowerScript != SNO_NONE)
            ScriptedNonAnimationPowerMessageHandler(ptPower, eMessage, pParam);
        else
            NonAnimationPowerMessageHandler(ptPower, eMessage, pParam);
        break;
    case POTION_POWER:
        PotionPower(ptPower, eMessage, pParam);
        break;
    case USE_ITEM_POWER:
        PowerUseItem(ptPower, eMessage, pParam);
        break;
    case SPECIALWALK_POWER:
    case ATTACKMOVE_POWER:
        if (snoPowerScript != SNO_NONE)
            ScriptedSpecialWalkPowerMessageHandler(ptPower, eMessage, pParam);
        else
            SpecialWalkPowerMessageHandler(ptPower, eMessage, pParam);
        break;
    case LOOPING_ANIMATION_POWER:
        if (snoPowerScript != SNO_NONE)
            ScriptedLoopingAnimationPowerMessageHandler(ptPower, eMessage, pParam);
        else
            LoopingAnimationPower(ptPower, eMessage, pParam);
        break;
    }
}




























SNO PowerGetEffect(PowerID idPower, uint32 dwTag)
{
    SNO snoEffectGroup = SNO_NONE;
    if (PowerContactTagExists(idPower, dwTag))
        snoEffectGroup = PowerContactTagGetSNO(idPower, dwTag);
    return snoEffectGroup;
}











SNO PowerGetGeneralEffect(SNO snoPower, uint32 dwTag)
{
    SNO snoEffectGroup = SNO_NONE;
    if (PowerGeneralTagExists(AutoPowerDefinitionR(snoPower), dwTag))
        snoEffectGroup = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), dwTag);
    return snoEffectGroup;
}














BOOL PowerUseAlternateAnim(PowerID idPower)
{
    BOOL bUseAlternateAnim = FALSE;
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
        if (ptACD && PowerAlternatesAnims(*ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
        {
            // The preplayed-anim flag names are not in the DWARF; bit 1 selects the alternate animation.
            bUseAlternateAnim = (ptPower->tPreplayedAnimData.dwPrePlayedAnimFlags >> 1) & 1;
        }
    }
    return bUseAlternateAnim;
}




SNO PowerDetermineCastEffect(PowerID idPower, ActorCommonData * ptACD)
{

    BOOL bUseAlternateEffect = PowerUseAlternateAnim(idPower);
    BOOL bFemale = FALSE;
    uint32 dwTag = bUseAlternateEffect ? TAG_POWER_CASTING_EFFECT_GROUP_2 : TAG_POWER_CASTING_EFFECT_GROUP;




    if (ptACD && ACDIsPlayer(ptACD->id))
    {
        Player * ptPlayer = ACDGetControllingPlayer(ptACD->id);

        bFemale = (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_FEMALE)) ? TRUE : FALSE;


        if (bFemale)
        {

            dwTag = bUseAlternateEffect ? TAG_POWER_CASTING_EFFECT_GROUP_FEMALE_2 : TAG_POWER_CASTING_EFFECT_GROUP_FEMALE;
        }
    }




    Power * ptPower = PowerGet(idPower);
    SNO snoPower = ptPower->snoPower;

    if (PowerIsComboPower(AutoPowerDefinitionR(snoPower)) && ptACD)
    {
        int32 nComboLevel = ptPower->nComboLevel;
        switch (nComboLevel)
        {
        case 0:
            dwTag = bFemale ? TAG_POWER_COMBO_CASTING_EFFECT_GROUP_FEMALE_0 : TAG_POWER_COMBO_CASTING_EFFECT_GROUP_0;
            break;
        case 1:
            dwTag = bFemale ? TAG_POWER_COMBO_CASTING_EFFECT_GROUP_FEMALE_1 : TAG_POWER_COMBO_CASTING_EFFECT_GROUP_1;
            break;
        case 2:
            dwTag = bFemale ? TAG_POWER_COMBO_CASTING_EFFECT_GROUP_FEMALE_2 : TAG_POWER_COMBO_CASTING_EFFECT_GROUP_2;
            break;
        }
        return PowerGetGeneralEffect(snoPower, dwTag);
    }

    return PowerGetEffect(idPower, dwTag);
}





SNO PowerDetermineContactFrameEffect(PowerID idPower, ActorCommonData * ptACD)
{

    BOOL bUseAlternateEffect = PowerUseAlternateAnim(idPower);
    BOOL bFemale = FALSE;
    uint32 dwTag = bUseAlternateEffect ? TAG_POWER_CONTACT_FRAME_EFFECT_GROUP_2 : TAG_POWER_CONTACT_FRAME_EFFECT_GROUP;




    if (ptACD && ACDIsPlayer(ptACD->id))
    {
        Player * ptPlayer = ACDGetControllingPlayer(ptACD->id);

        bFemale = (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_FEMALE)) ? TRUE : FALSE;


        if (bFemale)
        {

            dwTag = bUseAlternateEffect ? TAG_POWER_CONTACT_FRAME_EFFECT_GROUP_FEMALE_2 : TAG_POWER_CONTACT_FRAME_EFFECT_GROUP_FEMALE;
        }
    }




    Power * ptPower = PowerGet(idPower);
    SNO snoPower = ptPower->snoPower;

    if (PowerIsComboPower(AutoPowerDefinitionR(snoPower)) && ptACD)
    {
        int32 nComboLevel = ptPower->nComboLevel;
        switch (nComboLevel)
        {
        case 0:
            dwTag = bFemale ? TAG_POWER_COMBO_CONTACT_FRAME_EFFECT_GROUP_FEMALE_0 : TAG_POWER_COMBO_CONTACT_FRAME_EFFECT_GROUP_0;
            break;
        case 1:
            dwTag = bFemale ? TAG_POWER_COMBO_CONTACT_FRAME_EFFECT_GROUP_FEMALE_1 : TAG_POWER_COMBO_CONTACT_FRAME_EFFECT_GROUP_1;
            break;
        case 2:
            dwTag = bFemale ? TAG_POWER_COMBO_CONTACT_FRAME_EFFECT_GROUP_FEMALE_2 : TAG_POWER_COMBO_CONTACT_FRAME_EFFECT_GROUP_2;
            break;
        }
        return PowerGetGeneralEffect(snoPower, dwTag);
    }

    return PowerGetEffect(idPower, dwTag);
}





AttribGroupID PowerEquip(
    PowerID idPower)
{
    AttribGroupID idFastAttrib = FastAttribGroupCreate(0, -1);

    SpecificPowerSendMessage(idPower, POWER_POPULATE_ATTRIBUTES, (void *)(intptr_t)idFastAttrib);

    ActorCommonData * ptACD = PowerGetOwner(idPower);

    ptACD->AttributesMergeAttributeGroup(idFastAttrib);

    return idFastAttrib;
}




void PowerUnequip(
    PowerID idPower,
    AttribGroupID idFastAttrib)
{
    if (idFastAttrib == -1)
    {
        return;
    }

    ActorCommonData * ptACD = PowerGetOwner(idPower);

    ptACD->AttributesUnmergeAttributeGroup(idFastAttrib);

    FastAttribGroupDelete(idFastAttrib);
}



static void sBreakAllImmobilization(
    ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);
    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_IMMOBOLIZED), 0);
    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_POWER_IMMOBILIZED), 0);
    PowerAPI_FreeFromRoot(idACD);
}



void PowerSnapshotMeleeRange(Power * ptPower, const ACDID idACDPlayer, float flLongReach, const Target & tTarget)
{




    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);



    float flRange = PowerGetRange(&tACDPlayer, AutoPowerDefinitionR(ptPower->snoPower), FALSE);
    flRange += flLongReach;
    flRange += tACDPlayer.GetRadius();

    WorldPlace placePlayer = tACDPlayer.GetPlace();

    float flEscapeAngleInDegrees = PowerGetFormulaValue(ptPower->id, TAG_POWER_ESCAPE_ATTACK_ANGLE);
    Vector3D wvVectorToUse;
    WorldPlace placeDest;

    BOOL bValidPlace = FALSE;
    if (!tTarget.IsNone() && tTarget.IsInWorld())
    {
        tTarget.GetPlace(placeDest);
        bValidPlace = placeDest.idSWorld == placePlayer.idSWorld;
    }





    if (flEscapeAngleInDegrees > EPSILON && bValidPlace)
    {
        WorldPlaceGetVectorBetween(placePlayer, placeDest, wvVectorToUse);
        if (wvVectorToUse.MagnitudeSquared() < EPSILON)
        {
            bValidPlace = FALSE;
        }
    }

    if (!bValidPlace)
    {

        placeDest = tACDPlayer.GetPlace();
        Angle aYaw = ACDGetYaw(tACDPlayer.id);
        Vector3D vFront(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
        placeDest.wp += vFront;
    }

    WorldPlaceGetVectorBetween(placePlayer, placeDest, wvVectorToUse);
    wvVectorToUse.z = 0.0f;

    ACDsCylinderIntersectSphere(placePlayer, flRange, idACDPlayer, ptPower->listInitialMeleeRangeACDs, 7);

    if (flEscapeAngleInDegrees > EPSILON)
    {
        POS pos = ptPower->listInitialMeleeRangeACDs.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            ACDID idACDTarget = ACDID(ptPower->listInitialMeleeRangeACDs.GetNext(pos));

            if (!PowerAPI_TargetIsWithinArc(placePlayer, wvVectorToUse, flEscapeAngleInDegrees * (PI / 180.0f) * 0.5f, idACDTarget))
            {
                ptPower->listInitialMeleeRangeACDs.RemoveAt(posLast);
            }
        }
    }

    SGameGlobalsGet();  // result unused in the shipped build















    ACDID idACDTarget = tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget && ptACDTarget->IsInWorld())
    {







        ActorMovement * ptMove = ptACDTarget->GetActorMovement();
        if (ptMove)
        {
            WorldPlace placeTarget;
            placeTarget.idSWorld = placePlayer.idSWorld;
            ptMove->GetCurrentInterpolatedPos(placeTarget.wp);
            float wdDistance = PowerGetDistanceViaWorldPlace(placePlayer, placeTarget,
                tACDPlayer.GetHeight(), tACDPlayer.GetRadius(),
                ptACDTarget->GetHeight(), ptACDTarget->GetRadius(), flLongReach);



            if (wdDistance <= flRange)
            {
                ptPower->listInitialMeleeRangeACDs.AddHead_Tracked(idACDTarget);
            }
        }
    }
}



















static void sCopyAttributes(
    PowerID idPower,
    AttribGroupID idFastAttribSource)
{
    Power * ptPower = PowerGet(idPower);


    FastAttribLightCopy(idFastAttribSource, ptPower->idFastAttrib);
}




PowerID PowerStart(ACDID idACD, SNO snoPower, const Target & tTarget)
{
    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);

    tPowerStartData.idACD = idACD;
    tPowerStartData.idACDAlternateSource = idACD;
    tPowerStartData.snoPower = snoPower;
    tPowerStartData.tTarget = tTarget;

    PowerID idPower = PowerStartEx(tPowerStartData, -1, FALSE, FALSE);

    return idPower;
}





CRefString sGetPowerFailureString(uint32 dwFailureReason)
{
    CRefString strFail;

    for (int32 i = 0; i < 32; i++)
    {
        if (dwFailureReason & (1 << i))
        {
            strFail += "[";
            strFail += SymbolToName(i, gPowerFailureReasons);
            strFail += "]";
        }
    }

    return strFail;
}



void PowerEvaluatePowerStartedAchievementEvent(const PowerStartData & tPowerStartData)
{
    Achievement::EvaluationData tEvaluationData;
    tEvaluationData.SetPlayer(PlayerGetByACD(tPowerStartData.idACD));
    if (tEvaluationData.GetPlayer())
    {
        tEvaluationData.SetPower(&tPowerStartData);
        tEvaluationData.SetACD(ACDTryToGet(tPowerStartData.idACD));
        tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_PLAYER_POWER_STARTED, tPowerStartData.snoPower, 1);
    }
}
















void sPowerDuringBuffExpiration(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, void * pData)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
    Power * ptPower = PowerTryToGet(ptBuff->idPower);
    if (ptPower)
    {
        if (!(ptPower->dwFlags & (1 << PF_HAS_SENT_DURING_BUFF_WARNING)))
            ptPower->dwFlags |= (1 << PF_HAS_SENT_DURING_BUFF_WARNING);
    }
}
























PowerID PowerStartEx(
    const PowerStartData & tPowerStartData,
    AttribGroupID idFastAttribSource,
    BOOL bForceStart,
    BOOL bIgnoreRange)
{
    Stopwatch tStopwatch;
    float flTime;

    ACDID idACD = tPowerStartData.idACD;
    SNO snoPower = tPowerStartData.snoPower;
    const Target & tStartDataTarget = tPowerStartData.tTarget;
    ActorID idActor = ActorID(ACDGetOwner(idACD));



    if (!PowerIsValid(snoPower))
    {

        ActorSetPowerState(idActor, "Idle (Invalid Skill)");
        return -1;
    }

    if (!ActorTryToGet(idActor))
    {

        ActorSetPowerState(idActor, "Idle (Invalid Actor)");
        return -1;
    }

    ActorCommonData & tACD = ACDGet(idACD);

    ScopedPowerDefinitionLockR pdefPower(snoPower);


    if (!bForceStart && !sPowerCanStart(idActor, pdefPower, tPowerStartData))
    {
        uint32 dwFailureReason = PowerFailureReasons(tACD, pdefPower, &tStartDataTarget, FALSE, FALSE);
        if (dwFailureReason && bIgnoreRange)
        {
            dwFailureReason &= ~(1 << PS_NOT_CLOSE_ENOUGH_BIT);
        }
        if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_SENDS_FAILURE_REASON_TO_CLIENT))
        {
            GameErrorData tErrorData;
            GameErrorFromPowerFailureReasons(tErrorData, dwFailureReason, tACD.id, pdefPower, TRUE);
            if (tErrorData.eError != GAMEERROR_NONE)
            {
                PlayerDisplayErrorMessage(tACD.id, tErrorData.eError);
            }
        }
        else if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO))
        {
            CRefString sPowerFailureFlags = sGetPowerFailureString(dwFailureReason);



            ActorCommonData * ptACDGizmo = ACDTryToGet(tPowerStartData.tTarget.idACDTarget);
            if (ptACDGizmo)
            {
                if (ptACDGizmo->GetActorType() == ACTORTYPE_GIZMO)
                {
                    GizmoType eGizmoType = ptACDGizmo->GetGizmoType();
                    if (eGizmoType == ACTORGIZMO_HEARTH_PORTAL || eGizmoType == ACTORGIZMO_PORTAL)
                    {
                        SendPlayerCancelWarp(tACD.id);
                    }
                }
            }
        }
        else
        {
            CRefString sPowerFailureFlags = sGetPowerFailureString(dwFailureReason);

        }

        ActorSetPowerState(idActor, "Idle (Failed Power)");
        return -1;
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (PowerBreaksImmobilize(&tACD, pdefPower))
    {
        sBreakAllImmobilization(tACD.id);
    }

    if (PowerBreaksStun(&tACD, pdefPower))
    {
        PowerAPI_FreeFromStun(tACD.id);
    }

    if (PowerBreaksSnare(&tACD, pdefPower))
    {
        PowerAPI_FreeFromSnare(tACD.id);
    }

    if (PowerBreaksRoot(&tACD, pdefPower))
    {
        PowerAPI_FreeFromRoot(tACD.id);
    }

    if (PowerBreaksFear(&tACD, pdefPower))
    {
        PowerAPI_FreeFromFear(tACD.id);
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    PowerID idPower = PowerCreate(tACD.id, snoPower, tStartDataTarget);
    Power * ptPower = PowerGet(idPower);
    ptPower->listBuffInstances.SetPool(MemoryPoolsGetDATAIDListPool());


    const Target & tTarget = ptPower->tTarget;




    if (!tTarget.GetPlace(ptPower->placeStartTarget))
    {
        ptPower->placeStartTarget = cgplaceNull;
    }

    if (bIgnoreRange)
    {
        ptPower->dwFlags |= (1 << PF_IGNORE_RANGE);
    }

    ptPower->snoCasterActor = tACD.GetActorSNO();

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_SHOW_POWERS))
    {
        if (tACD.GetLocationType() == AL_WORLD)
        {
            WorldPlace placeSource = tACD.GetPlace();
            CRefString strText;
            strText.Format("%s", SNOToString(POWER_SNO, snoPower, 0).str());
            if (tTarget.eType == TT_ACTOR)
            {
                ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.GetACD());
                if (ptACDTarget)
                {
                    CRefString strTarget;
                    strTarget.Format("(%s)", MarkerNameGetString(ptACDTarget->tName));
                    strText += strTarget;
                }
            }

            (void)CRefString(strText);  // A debug text call taking strText by value (and presumably placeSource) was compiled out here: only the argument copy survives, and the DWARF records no callee (no inlined_subroutine at this line, no declaration). Probably an empty stub from X/Common/DebugDraw.h, whose DebugDrawID enum this CU retains; the name is unknown, so none is invented.
        }
    }


    if (PowerIsPrimary(pdefPower, idACD) && PowerIsOffensive(pdefPower))
    {
        if (!(PowerTargetsContactPlaneOnly(pdefPower) ||
              PowerTargetPathableOnly(pdefPower) ||
              PowerTargetNavMeshOnly(pdefPower) ||
              PowerTargetGroundOnly(pdefPower)))
        {
            CRefString sError;
            sError.Format("PowerStartEx: Actor(%s) is starting a PowerSNO (%s) which does not have one of the four valid targeting types set (Contact Plane Offset, Pathable, Nav Mesh, or Ground Plane). This will result in undefined power script behavior and must be fixed.\n",
                          HyperlinkCreateFromSNO(SNOName(ACTOR_SNO, tACD.snoActor), NULL).str(),
                          HyperlinkCreateFromSNO(SNOName(POWER_SNO, snoPower), NULL).str());

        }
    }


    ptPower->nSkillLevel = tPowerStartData.nSkillLevel;
    ptPower->uAICoolDownKey = 0;
    ptPower->uAICoolDownTicks = 0;

    ptPower->bFreeCast = tPowerStartData.bFreeCast;
    ptPower->bDoesntAnimate = tPowerStartData.bDoesntAnimate;
    ptPower->bAllowHitProcs = tPowerStartData.bAllowHitProcs;

    ptPower->dwParam = tPowerStartData.dwParam;

    SGameStatsRecordPowerUse(tACD, snoPower, idPower, tTarget.IsInvalid() ? ACDID_INVALID : tTarget.GetACD());

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    XPooledDataIDList listMergedBuffFastAttribs(MemoryPoolsGetDATAIDListPool());
    if (ACDIsPlayer(ptPower->idACD))
    {
        PowerAPI_BuffMergeAttribsForPowers(tACD, snoPower, listMergedBuffFastAttribs);
    }

    if (idFastAttribSource == -1)
    {
        AttribGroupID idFastAttrib = PowerEquip(idPower);
        sCopyAttributes(idPower, ACDGet(ptPower->idACD).idFastAttrib);
        PowerUnequip(idPower, idFastAttrib);
    }
    else
    {
        sCopyAttributes(idPower, idFastAttribSource);
    }

    if (tPowerStartData.eRuneTypeOverride != RUNE_TYPE_INVALID)
    {
        if (tPowerStartData.eRuneTypeOverride < RUNE_TYPE_COUNT || tPowerStartData.eRuneTypeOverride == RUNE_TYPE_NONE)
        {




            RuneType eRuneTypeExisting = PowerGetRuneType(ptPower);
            if (eRuneTypeExisting != RUNE_TYPE_INVALID)
            {
                FastAttribSetValueInt(ptPower->idFastAttrib, MakeAttribKey(PowerAttributeFromRuneType(eRuneTypeExisting), ptPower->snoPower), 0);
            }

            if (tPowerStartData.eRuneTypeOverride != RUNE_TYPE_NONE)
            {
                FastAttribSetValueInt(ptPower->idFastAttrib, MakeAttribKey(PowerAttributeFromRuneType(tPowerStartData.eRuneTypeOverride), ptPower->snoPower), 1);
            }
        }
    }

    if (ACDIsPlayer(ptPower->idACD))
    {
        PowerAPI_BuffUnmergeAttribsForPowers(tACD, listMergedBuffFastAttribs);
    }

    if (PowerUsesMainHandOnly(pdefPower, tPowerStartData.nComboLevel) && ACDIsDualWielding(tACD, TRUE))
    {
        FastAttribSetValueInt(ptPower->idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 0);

        ACDID idACDPrimaryWeapon = ACDInventoryGetPrimaryWeapon(tACD.id);
        ACDID idACDSecondaryWeapon = ACDInventoryGetSecondaryWeaponOrShield(tACD.id);

        ActorCommonData * ptACDPrimaryWeapon = ACDTryToGet(idACDPrimaryWeapon);
        ActorCommonData * ptACDSecondaryWeapon = ACDTryToGet(idACDSecondaryWeapon);

        if (!ptACDPrimaryWeapon && ptACDSecondaryWeapon && ItemIsOneHandedWeapon(*ptACDSecondaryWeapon))
        {
            FastAttribSetValueInt(ptPower->idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 1);
        }
    }


    if (FastAttribGetValueInt(ptPower->idFastAttrib, FastAttribKey(ATTRIBUTE_LEVEL)) <= 0)
    {
        FastAttribSetValueInt(ptPower->idFastAttrib, FastAttribKey(ATTRIBUTE_LEVEL), ActorGetCorrectLevel(SNONAME_NONE, -1));
    }


    ptPower->idACDAlternateSource = tPowerStartData.idACDAlternateSource;

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    SpecificPowerSendMessage(idPower, POWER_CREATE, NULL);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }



    if (PowerIsPrimary(pdefPower, tACD.id) && !tPowerStartData.bDoesntAnimate)
    {
        PowerID idCurrentPower = ActorPowersGetPrimary(idActor);
        Power * ptPowerCurrent = PowerTryToGet(idCurrentPower);
        if (ptPowerCurrent)
        {
            PowerTemplate eCurrentTemplate = PowerGetTemplate(AutoPowerDefinitionR(ptPowerCurrent->snoPower), ptPowerCurrent->idACD);
            switch (eCurrentTemplate)
            {
            case SPECIALWALK_POWER:
            case ATTACKMOVE_POWER:
                PowerInterrupt(idCurrentPower, FALSE);
                PowerDetachFromActor(idCurrentPower, FALSE);
                break;

            case GIZMO_POWER:
            case LOOPING_ANIMATION_POWER:
                PowerForceStop(idCurrentPower, TRUE);
                break;

            default:
                PowerDetachFromActor(idCurrentPower, FALSE);
                break;
            }

            if (PowerShouldPlayAnimForPower(
                    tACD.id,
                    tTarget,
                    ptPower->snoPower,
                    PowerGetSNO(idCurrentPower)))
            {
                AnimPlayerPlayIdle(tACD);
            }
        }

        ActorPowersSetPrimary(idActor, idPower);






        if (PowerHasEscapeRange(*ptPower) && ACDIsPlayer(tACD.id))
        {


            PowerSnapshotMeleeRange(ptPower, tACD.id, 1.0f, tTarget);
        }
    }
    else
    {
        ptPower->dwFlags |= (1 << PF_INTERRUPTABLE);
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    ActorPowersAdd(idActor, idPower);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    PowerTemplate eTemplate = PowerGetTemplate(pdefPower, idACD);
    if (eTemplate != SECONDARY_ANIMATION_POWER)
    {

        if (PowerIsUninterruptableDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_UNINTERRUPTIBLE_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_UNINTERRUPTIBLE), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerIsInvulnerableDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_INVULNERABLE_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_INVULNERABLE), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerIsInvisibleDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_INVISIBLE_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_HIDDEN), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerIsUntargetableDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_UNTARGETABLE_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_UNTARGETABLE), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerIsImmuneToStunDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_STUN_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_STUN_IMMUNE), 1);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_BLIND_IMMUNE), 1);
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_FREEZE_IMMUNE), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerIsImmuneToSnareDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_SNARE_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_SLOWDOWN_IMMUNE), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerIsImmuneToRootDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_ROOT_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_ROOT_IMMUNE), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerIsImmuneToFearDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_IMMUNE_TO_FEAR_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_FEAR_IMMUNE), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }

        if (PowerCannotDieDuring(&tACD, pdefPower))
        {
            SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_CANNOT_DIE_DURING_BUFF);
            PowerAPIBuffStartData tBuffStartData(idPower);
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.bPositiveEffect = true;
            tBuffStartData.eStackType = 1;
            if (ACDIsPlayer(tACD.id))
            {
                tBuffStartData.dwDurationMin = DURING_BUFF_MAX_DURATION;
                tBuffStartData.pExpirationEffect = sPowerDuringBuffExpiration;
                tBuffStartData.pExpirationEffectData = (void *)(intptr_t)snoPower;
            }
            else
            {
                tBuffStartData.bInfinite = true;
            }
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_CANNOT_DIE_DURING), 1);
            PowerAPI_BuffAddToACD(tBuffStartData, tACD);
        }
    }


    if (PowerIsEmote(pdefPower))
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_EMOTE_COOLDOWN), GameGetServerTime() + POWER_EMOTE_COOLDOWN_TICKS);
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    ptPower->tPreplayedAnimData = tPowerStartData.tPreplayedAnimData;

    if (tPowerStartData.dwTargetMessageFlags & 1)  // bit 0; no enumerator for it in src
    {
        ptPower->dwFlags |= (1 << PF_CONSOLE_MOVE_TOWARDS_TARGET);
    }

    if (tPowerStartData.dwAnimTagToUse != -1)
    {

        ptPower->dwAnimationTag = tPowerStartData.dwAnimTagToUse;
    }
    else
    {
        ptPower->dwAnimationTag = PowerDetermineAnimation(pdefPower,
                                                          tACD.id,
                                                          tTarget,
                                                          (tPowerStartData.tPreplayedAnimData.dwPrePlayedAnimFlags & 2) != 0);
    }



    PowerUpdateFacing(idPower, NULL, TRUE);
    ptPower->qStartOrientation = tACD.GetOrientation();


    int32 nComboLevel = tPowerStartData.nComboLevel;
    if (PowerIsComboPower(pdefPower))
    {
        ptPower->nComboLevel = nComboLevel;

        PowerSetContactFrame(idPower, nComboLevel);
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_COMBO_TIME_LAST_MOVE), GameGetServerTime());
    }
    else if (nComboLevel && PowerIsPrimary(pdefPower, tACD.id))
    {
        if (PowerRefreshesComboLevel(pdefPower))
        {
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_COMBO_TIME_LAST_MOVE), GameGetServerTime());
        }

        else if (PowerIsOffensive(pdefPower))
        {

            tACD.SetComboLevel(0);
        }
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }



    PowerEvaluatePowerStartedAchievementEvent(tPowerStartData);

    SpecificPowerSendMessage(idPower, POWER_START, tPowerStartData.ptExtraPowerStartData);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    if (!(ptPower->dwFlags & (1 << PF_FAILED)))
    {
        SNO snoCastEffect = PowerDetermineCastEffect(idPower, &tACD);
        if (snoCastEffect != SNO_NONE)
        {
            ACDPlayEffect(tACD.id, EFFECT_PLAY_EFFECT_GROUP, snoCastEffect, tACD.id);
        }
    }

    if (PowerIsOffensive(pdefPower))
    {
        ACDID idACDTarget = tTarget.GetACD();
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && ACDIsHostile(tACD, *ptACDTarget, TRUE))
        {
            tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_LAST_ACD_ATTACKED), idACDTarget);
            ptACDTarget->AttributesSetInt(FastAttribKey(ATTRIBUTE_LAST_ACD_ATTACKED_BY), idACD);
        }

        PowerAPI_BuffApplyOnAttackEffects(tACD.id, idACDTarget, idPower, snoPower);
    }

    PowerCheckForDone(idPower);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    ScriptEventOnActorUsedPower(tACD.id, snoPower, tTarget.GetACD());

    return idPower;
}





void PowerUpdateFacing(const PowerID idPower, void * pData)
{
    PowerUpdateFacing(idPower, pData, FALSE);
}







void PowerUpdateFacing(const PowerID idPower, void *, BOOL bIsStartOfPower)
{



    Power * ptPower = PowerGet(idPower);
    ActorID idPowerActor = ActorID(ACDGetOwner(ptPower->idACD));

    const Actor * ptActor = ActorTryToGet(idPowerActor);
    if (!ptActor)
        return;



    const PowerID idPrimaryPower = ActorPowersGetPrimary(idPowerActor);
    if (idPrimaryPower != idPower)
        return;




    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    if (ptACD && PowerIsChannelled(*ptACD, AutoPowerDefinitionR(ptPower->snoPower)) && PowerClientControlsFacing(AutoPowerDefinitionR(ptPower->snoPower)))
    {
        return;
    }



    if (ptPower->dwLastContactTick && PowerContactFreezesFacing(AutoPowerDefinitionR(ptPower->snoPower)))
    {
        return;
    }


    BOOL bSnapToFacing = FALSE;
    if (!bIsStartOfPower)
    {
        if (PowerNeverUpdatesFacingDuring(AutoPowerDefinitionR(ptPower->snoPower), ptPower->idACD, ptPower->tTarget.idACDTarget))
        {
            return;
        }
    }
    else
    {
        if (PowerNeverUpdatesFacingStarting(AutoPowerDefinitionR(ptPower->snoPower), ptPower->idACD, ptPower->tTarget.idACDTarget))
        {
            return;
        }
        if (PowerSnapsToFacing(AutoPowerDefinitionR(ptPower->snoPower)))
        {
            bSnapToFacing = TRUE;
        }
    }



    if (!ptACD ||
        !ptACD->IsGizmo())
    {




        PowerFaceTarget(ptACD, ptPower->tTarget, ptPower->dwAnimationTag, bSnapToFacing);

        PowerTriggerAddTimerTrigger(idPower, POWER_FACING_UPDATE_RATE, PowerUpdateFacing, NULL);
    }
}



static BOOL sPowerIsDone(const Power * ptPower)
{


    if (ptPower->dwFlags & (1 << PF_MANUALLY_FINISH))
        return FALSE;


    if (ptPower->idTimerContact != -1)
        return FALSE;


    if (ptPower->idTimerActorDone != -1)
        return FALSE;


    if (ptPower->listTimers.Count() != 0)
        return FALSE;


    if (ptPower->tListSummonedActors.Count() != 0)
        return FALSE;


    if (ptPower->listScriptInstanceIDs.Count() > 0)
    {
        POS pos = ptPower->listScriptInstanceIDs.GetHeadPosition();
        while (pos)
        {
            ScriptInstanceID idScriptInstance = ptPower->listScriptInstanceIDs.GetNext(pos);
            if (LuaScriptTryToGetLuaStateWithScriptInstanceID(idScriptInstance))
                return FALSE;
        }
    }


    if (ptPower->listBuffInstances.Count() > 0)
        return FALSE;

    return TRUE;
}












void PowerCheckForDone(const PowerID idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;

    BOOL bDone = sPowerIsDone(ptPower);

    if (bDone)
        ServerQueueUpdateFunction(0, ptPower->tPowerUpdateData);
}










void PowerUpdateDeletion(Power * ptPower)
{


    BOOL bDone = sPowerIsDone(ptPower);
    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);

    if (bDone)
    {
        ActorID idPowerActor = ActorID(ACDGetOwner(ptPower->idACD));
        ActorPowersRemove(idPowerActor, ptPower->id);

        if (ptACD)
        {
            PowerClearDuringBuffs(ptPower, ptACD);

            if (PowerCannotDieDuring(ptACD, AutoPowerDefinitionR(ptPower->snoPower)))
            {


                if (ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_QUEUE_DEATH)) > 0 && !ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_CANNOT_DIE_DURING)))
                {

                    ActorID idLastDamageActor = ActorID(ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_DAMAGE_MAINACTOR)));
                    if (!ActorTryToGet(idLastDamageActor))
                    {
                        idLastDamageActor = ACTORID_INVALID;
                    }

                    SNO snoDeathReplacementPower = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_DEATH_REPLACEMENT_POWER_SNO));
                    if (snoDeathReplacementPower != SNO_NONE)
                    {
                        BrainStanceDoDeathReplacement(idPowerActor, idLastDamageActor);
                    }
                    else
                    {
                        BrainAPIDoDeath(idPowerActor, idLastDamageActor, FALSE, FALSE, FALSE, NULL, NULL);
                    }
                }
            }



            if (ACDIsPlayer(ptACD->id))
            {
                Player * ptPlayer = PlayerGetByACD(ptACD->id);
                if (ptPlayer)
                {
                    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
                    if (PowerRefundsResourcesWithoutDamage(AutoPowerDefinitionR(pdefPower), ptPlayer))
                    {
                        if (!(ptPower->dwFlags & (1 << PF_CONSOLE_HAS_DAMAGED_OR_BUFFED)))
                        {
                            PowerHelper_RefundCosts(ptPower->id);
                        }
                    }
                }
            }
        }


        PowerDelete(ptPower->id);
    }
}




void PowerUpdate(PowerID idPower)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        ServerUpdateWasCalled(ptPower->tPowerUpdateData);
        PowerUpdateDeletion(ptPower);
    }
}










BOOL PowerAtDestination(const PowerID idPower, const ACDID idACDProjectile, const ACDID idACDIntersected, const SpellEffect eSpellEffect)
{
    ImpactData tImpact;
    tImpact.idACDProjectile = idACDProjectile;
    tImpact.idACDIntersected = idACDIntersected;
    tImpact.eSpellEffect = eSpellEffect;
    if (ACDIsProjectile(idACDProjectile))
        SpecificPowerSendMessage(idPower, POWER_IMPACT, &tImpact);
    else
        SpecificPowerSendMessage(idPower, POWER_ARRIVED, &tImpact);
    return tImpact.idACDIntersected != ACDID_INVALID;
}





















Power * PowerGet(PowerID idPower)
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

    return ptPowerGlobals->pArrayPowers->ElementGet(idPower);
}





Power * PowerTryToGet(PowerID idPower)
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    if (!ptPowerGlobals)
        return NULL;


    return ptPowerGlobals->pArrayPowers->ElementTryToGet(idPower);
}





static SNO sPowerDetermineProjectileSNO(PowerID idPower)
{
    SNO snoProjectile = SNO_NONE;

    if (PowerContactTagGetBool(idPower, TAG_POWER_USES_WEAPON_PROJECTILE))
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner && ACDIsPlayer(ptACDOwner->id))
        {
            ACDID idACDActiveWeapon = ACDInventoryGetActiveWeapon(*ptACDOwner);
            ActorCommonData * ptACDActiveWeapon = ACDTryToGet(idACDActiveWeapon);
            if (ptACDActiveWeapon && !ItemIsBroken(*ptACDActiveWeapon))
            {
                SNO snoWeaponActor = ACDGet(idACDActiveWeapon).snoActor;
                ScopedActorDefinitionLockR pdefActor(snoWeaponActor);
                snoProjectile = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_PROJECTILE_SNO);
            }
        }
    }

    if (snoProjectile == SNO_NONE)
    {
        snoProjectile = PowerContactTagGetSNO(idPower, TAG_POWER_PROJECTILE_SNO);
    }
    return snoProjectile;
}




SNO PowerGetExplosionSNO(const PowerID idPower)
{
    SNO snoExplosion = PowerContactTagGetSNO(idPower, TAG_POWER_EXPLOSION_SNO);
    if (snoExplosion == SNO_NONE)
    {
        ActorCommonData * ptACDOwner = PowerGetOwner(idPower);
        if (ptACDOwner)
        {
            SNO snoWeapon = ACDInventoryGetPrimaryWeaponSNO(ptACDOwner->id);
            ScopedActorDefinitionLockR pdefActor(snoWeapon);
            snoExplosion = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EXPLOSION_SNO);
        }
    }
    return snoExplosion;
}








void PowerSetContactFrame(
    PowerID idPower,
    int32 nFrame)
{
    Power * ptPower = PowerGet(idPower);
    ptPower->dwCurrentContactFrame = nFrame;
    PowerTargetCriteriaInit(idPower);
}





void PowerTargetCriteriaInit(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    TargetCriteria & tTargetCriteria = ptPower->tTargetCriteria;
    SigmaMemoryClear(&tTargetCriteria, sizeof(tTargetCriteria));

    PayloadType ePayload = PowerGetPayload(idPower);

    // PayloadType's enumerator names are not in the DWARF.
    if (ePayload == 3)
    {
        tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_IGNORES_PETS);
    }

    if (PowerTargetAllies(idPower))
    {
        tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_ALLIES);
    }

    if (PowerTargetEnemies(idPower))
    {
        tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_ENEMIES);
    }

    if (PowerContactTagGetBool(idPower, TAG_POWER_TARGET_IGNORE_WRECKABLES))
    {
        tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_IGNORES_WRECKABLES);
    }

    if (PowerTargetNecroCorpsesOnly(idPower))
    {
        tTargetCriteria.dwTeamRelationshipFlags |= (1 << TCTR_AFFECTS_NECRO_CORPSES_ONLY);
    }

    if (PowerTargetCorpse(idPower))
        tTargetCriteria.eLifeStatus = TCLS_DEAD_ONLY;
    else
        tTargetCriteria.eLifeStatus = TCLS_ALIVE_ONLY;
}






static const uint32 POWER_COOKIE_VALUE = 0xec00eae;





PowerID PowerCreate(const ACDID idACD, SNO snoPower, const Target & tTarget)
{



    PowerTemplate ePowerTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), idACD);

    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    PowerID idPower = ptPowerGlobals->pArrayPowers->ElementNew();

    if (idPower == -1)
    {


        TDataArrayIterator<Power> tIterator;
        while (ptPowerGlobals->pArrayPowers->Iterate(tIterator))
        {
        }



        DisplayInternalError(NULL, "", 3950, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    ActorCommonData & tACD = ACDGet(idACD);
    Power * ptPower = PowerGet(idPower);
    ptPower->nSkillLevel = -1;
    ptPower->ePowerTemplate = ePowerTemplate;
    ptPower->idACD = idACD;
    ptPower->idACDAlternateSource = idACD;
    ptPower->snoPower = snoPower;
    ptPower->bFreeCast = FALSE;
    ptPower->bDoesntAnimate = FALSE;
    ptPower->bAllowHitProcs = FALSE;
    ptPower->nCurrentSpellEffect = 0;
    ptPower->wpStartLocation = tACD.GetPlace();
    ptPower->qStartOrientation = tACD.GetOrientation();
    ptPower->tTarget = tTarget;
    ptPower->dwFlags &= ~(1 << PF_INTERRUPTABLE);
    ptPower->dwFlags &= ~(1 << PF_CANCEL);
    ptPower->dwFlags &= ~(1 << PF_INITAL_COSTS_PAID);
    ptPower->dwFlags &= ~(1 << PF_STAMINA_COST_PAID);
    ptPower->dwFlags &= ~(1 << PF_FAILED);
    ptPower->dwFlags &= ~(1 << PF_MANUALLY_FINISH);
    ptPower->dwFlags &= ~(1 << PF_GLOBE_DROPPED);
    ptPower->tListSummonedActors.SetPool(MemoryPoolsGetDATAIDListPool());

    ptPower->idTimerContact = TIMERID_INVALID;
    ptPower->idTimerInterrupt = TIMERID_INVALID;
    ptPower->idTimerActorDone = TIMERID_INVALID;

    ptPower->listTimers.SetPool(&ptPowerGlobals->tPowerTimerPool);
    ptPower->listAffectedACDs.SetPool(&ptPowerGlobals->tAffectedACDPool);
    ptPower->listScriptInstanceIDs.SetPool(&ptPowerGlobals->tScriptInstanceIDPool);
    ptPower->listPowerAPIDamage.SetPool(&ptPowerGlobals->tPowerAPIDamagePool);
    ptPower->listPayloads.SetPool(&ptPowerGlobals->tAttackPayloadPool);
    ptPower->listCursors.SetPool(&ptPowerGlobals->tCursorPool);
    ptPower->listBuffs.SetPool(&ptPowerGlobals->tBuffStartDataPool);
    ptPower->listChooseTargets.SetPool(&ptPowerGlobals->tChooseTargetsPool);
    ptPower->listDamage.SetPool(&ptPowerGlobals->tDamagePool);
    ptPower->mapFloatValues.SetPool(&ptPowerGlobals->tMapFloatValuesPool);
    ptPower->mapIntValues.SetPool(&ptPowerGlobals->tMapIntValuesPool);

    ptPower->idFastAttrib = FastAttribLightCreate(FastAttribType(1));
    ptPower->eHitsound = 0;


    ptPower->eTeam = tACD.GetTeam();
    ptPower->snoProjectile = sPowerDetermineProjectileSNO(idPower);

    ptPower->listACDsTargetted.SetPool(MemoryPoolsGetDATAIDListPool());
    ptPower->listACDsTracked.SetPool(MemoryPoolsGetDATAIDListPool());
    ptPower->listInitialMeleeRangeACDs.SetPool(MemoryPoolsGetDATAIDListPool());
    ptPower->listActiveCallbackBuffs.SetPool(MemoryPoolsGetDATAIDListPool());

    ptPower->dwStartTick = GameGetServerTime();
    ptPower->dwFirstContactTick = 0;
    ptPower->dwLastContactTick = 0;

    ptPower->dwCurrentContactFrame = 0;

    ptPower->snoContactAnimation[0] = SNO_NONE;
    ptPower->snoContactAnimation[1] = SNO_NONE;
    ptPower->snoContactAnimation[2] = SNO_NONE;
    ptPower->nContactAnimationPermutation[0] = 0;
    ptPower->nContactAnimationPermutation[1] = 0;
    ptPower->nContactAnimationPermutation[2] = 0;

    ptPower->dwFlags &= ~(1 << PF_WAS_REFLECTED);
    ptPower->idACDReflectedBy = ACDID_INVALID;
    ptPower->snoPowerLastReflectedBy = SNO_NONE;
    ptPower->idACDLastReflectedBy = ACDID_INVALID;

    ptPower->bDidFirstHitResourceGains = FALSE;

    ptPower->flExecutionTime = 0.0f;
    ptPower->ptCurrentStopwatch = NULL;

    ptPower->snoScript = SNO_NONE;
    ptPower->idScript = -1;
    ptPower->ptLuaState = NULL;
    ptPower->nScriptCalls = ptPower->nLockGlobalsCount = ptPower->nExpensiveScriptCalls = ptPower->nPayloadCount = 0;




    UpdateFunctionInitialize(UPDATE_POWER, idPower, ptPower->tPowerUpdateData);

    PowerTargetCriteriaInit(idPower);


    ptPower->dwParam = (uint32)-1;
    ptPower->flParam = 0.0f;
    ptPower->flParam2 = 0.0f;
    ptPower->flParam3 = 0.0f;

    SigmaMemoryClear(ptPower->pPowerTemplateData, sizeof(ptPower->pPowerTemplateData));
    ptPower->dwCookieValue = POWER_COOKIE_VALUE;

    ptPower->dwLastLifeOnHitTick = 0;

    return idPower;
}




void PowerDelete(PowerID idPower)
{
    Power * ptPower;
    PowerGlobals * ptPowerGlobals;

    ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;

























    PowerReuseScriptState(AutoPowerDefinitionR(ptPower->snoPower));







    ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

    if (ptPower->idScript != -1)
    {



        lua_State * ptLuaState = LuaScriptTryGetLuaState(ptPower->idScript);
        if (ptLuaState)
        {
            if (!PowerReuseScriptState(AutoPowerDefinitionR(ptPower->snoPower)))
            {
                LuaScriptFreeStateWithScriptID(ptPower->idScript);
            }
        }




        ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);

        if (pdefPower->snoScript != ptPower->snoScript)
        {
            BOOL bFoundOther = FALSE;
            TDataArrayIterator<Power> tIt;


            while (Power * ptPowerInstance = ptPowerGlobals->pArrayPowers->Iterate(tIt))
            {

                if (ptPowerInstance->snoScript == ptPower->snoScript && ptPowerInstance->id != ptPower->id)
                {
                    bFoundOther = TRUE;
                }
            }
            if (!bFoundOther)
            {

                if (PowerReuseScriptState(AutoPowerDefinitionR(ptPower->snoPower)))
                {
                    LuaScriptFreeStateWithScriptID(ptPower->idScript);
                    ScriptID idScript;
                    if (ptPowerGlobals->tScriptIDs.Lookup(ptPower->snoScript, idScript))
                    {
                        ptPowerGlobals->tScriptIDs.RemoveAt(ptPower->snoScript);
                    }
                }

                ScriptDefinitionDisposeScript(ptPower->snoScript);
            }
        }

        ptPower->snoScript = -1;






    }

    if (ptPower->idFastAttrib != -1)
    {
        FastAttribGroupDelete(ptPower->idFastAttrib);
    }

    ptPowerGlobals->pArrayPowers->ElementDelete(ptPower, TRUE);
}




BOOL PowerEffectTagExists(PowerID idPower, uint32 dwTag)
{
    return PowerContactTagExists(idPower, dwTag);
}






int32 PowerEffectTagGetInt(PowerID idPower, uint32 dwTag)
{
    return PowerContactTagGetInt(idPower, dwTag);
}






SNO PowerEffectTagGetSNO(PowerID idPower, uint32 dwTag)
{
    return PowerContactTagGetSNO(idPower, dwTag);
}















BOOL PowerContactTagGetBool(PowerID idPower, uint32 dwTag)
{
    Power * ptPower = PowerGet(idPower);
    return PowerContactTagGetBool(AutoPowerDefinitionR(ptPower->snoPower), dwTag, ptPower->dwCurrentContactFrame);
}





int32 PowerContactTagGetInt(PowerID idPower, uint32 dwTag)
{
    Power * ptPower = PowerGet(idPower);
    return PowerContactTagGetInt(AutoPowerDefinitionR(ptPower->snoPower), dwTag, ptPower->dwCurrentContactFrame);
}





float PowerContactTagGetFloat(PowerID idPower, uint32 dwTag)
{
    Power * ptPower = PowerGet(idPower);
    return PowerContactTagGetFloat(AutoPowerDefinitionR(ptPower->snoPower), dwTag, ptPower->dwCurrentContactFrame);
}





SNO PowerContactTagGetSNO(PowerID idPower, uint32 dwTag)
{
    Power * ptPower = PowerGet(idPower);
    return PowerContactTagGetSNO(AutoPowerDefinitionR(ptPower->snoPower), dwTag, ptPower->dwCurrentContactFrame);
}















BOOL PowerContactTagExists(PowerID idPower, uint32 dwTag)
{
    Power * ptPower = PowerGet(idPower);
    return PowerContactTagExists(AutoPowerDefinitionR(ptPower->snoPower), dwTag, ptPower->dwCurrentContactFrame);
}
















BOOL PowerNeedsUpcallFromWalking(PowerID idPower)
{
    PowerTemplate nTemplate = PowerGet(idPower)->ePowerTemplate;
    switch (nTemplate)
    {
    case SPECIALWALK_POWER:
    case ATTACKMOVE_POWER:
        return TRUE;
    }
    return FALSE;
}





SpellEffect PowerGetSpellEffect(PowerID idPower, int32 nIndex)
{
    uint32 dwTag;
    switch (nIndex)
    {
    case 0:
        dwTag = TAG_POWER_SPELL_FUNC_0;
        break;
    case 1:
        dwTag = TAG_POWER_SPELL_FUNC_1;
        break;
    default:
        dwTag = (uint32)-1;
        break;
    }

    SpellEffect eSpellEffect = 0;
    if (dwTag != (uint32)-1)
        eSpellEffect = PowerContactTagGetInt(idPower, dwTag);
    return eSpellEffect;
}









SpellEffect PowerGetNextSpellEffect(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    return PowerGetSpellEffect(idPower, ptPower->nCurrentSpellEffect + 1);
}





SpellEffect PowerGetBeginEffect(PowerID idPower)
{
    SNO snoPower = PowerGetSNO(idPower);
    return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_SPELL_FUNC_BEGIN);
}





SpellEffect PowerGetEndEffect(PowerID idPower)
{
    SNO snoPower = PowerGetSNO(idPower);
    if (PowerIsComboPower(AutoPowerDefinitionR(snoPower)))
    {
        switch (PowerGet(idPower)->nComboLevel)
        {
        case 0:
            return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_COMBO_SPELL_FUNC_END_1);
        case 1:
            return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_COMBO_SPELL_FUNC_END_2);
        case 2:
            return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_COMBO_SPELL_FUNC_END_3);
        }
    }
    return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_SPELL_FUNC_END);
}









SpellEffect PowerGetInterruptedEffect(PowerID idPower)
{
    SNO snoPower = PowerGetSNO(idPower);
    if (PowerIsComboPower(AutoPowerDefinitionR(snoPower)))
    {
        switch (PowerGet(idPower)->nComboLevel)
        {
        case 0:
            return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_COMBO_SPELL_FUNC_INTERRUPTED_1);
        case 1:
            return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_COMBO_SPELL_FUNC_INTERRUPTED_2);
        case 2:
            return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_COMBO_SPELL_FUNC_INTERRUPTED_3);
        }
    }
    return PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_SPELL_FUNC_INTERRUPTED);
}










SNO PowerGetChildPowerBySNO(SNO snoPower)
{
    return PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_CHILD_POWER);
}





SNO PowerGetChildPower(PowerID idPower)
{
    return PowerGetChildPowerBySNO(PowerGetSNO(idPower));
}




SNO PowerGetTargetingPassabilitySNOBySNO(SNO snoPower)
{
    return PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_TARGETING_PASSABILITY_SNO);
}














void InitializeDamage(DamageAttribute & tDamageAttribute, FastAttribKey tKey, float flValue, int32 nValue, int32 nDuration)
{
    tDamageAttribute.tKey = tKey;
    tDamageAttribute.flValue = flValue;
    tDamageAttribute.nValue = nValue;
    tDamageAttribute.nDuration = nDuration;
}



































BOOL PowerCheckToHit(const PowerAPIAttackPayload & tPayload, const ActorCommonData & tACDTarget)
{
    ActorCommonData * ptACDAttacker = PowerGetOwner(tPayload.idPower);
    if (ptACDAttacker)
    {
        Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptACDAttacker->id));
        if (ptActor)
        {
            if (ActorFlagIsSet(ptActor->id, ACTOR_EASYKILL_BIT))
                return TRUE;
            if (ptACDAttacker->AttributesGetInt(FastAttribKey(ATTRIBUTE_ALWAYS_HITS)) > 0)
                return TRUE;
        }
    }
    if (tPayload.bAlwaysHits)
        return TRUE;
    GizmoType eGizmoType = tACDTarget.GetGizmoType();
    if (eGizmoType == ACTORGIZMO_BREAKABLECHEST)
        return TRUE;
    if (eGizmoType == ACTORGIZMO_BREAKABLEDOOR)
        return TRUE;
    PowerID idPower = tPayload.idPower;
    float flHitChance = 0.0f;
    Power * ptPower = PowerTryToGet(idPower);
    if (ptPower)
    {
        float flPowerHitChance = PowerContactTagGetFloat(idPower, TAG_POWER_HIT_CHANCE);
        if (flPowerHitChance < 0.000001f)
            flPowerHitChance = 1.0f;
        flHitChance = flPowerHitChance * FastAttribGetValueFloat(ptPower->idFastAttrib, FastAttribKey(ATTRIBUTE_HIT_PERCENT));
    }
    float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
    Power * ptPowerLog = PowerTryToGet(tPayload.idPower);
    CombatLog::LogEvent(2, ptPowerLog ? ptPowerLog->idACD : ACDID_INVALID, tACDTarget.id, ptPowerLog ? ptPowerLog->snoPower : SNO_NONE, flRoll, flHitChance, 0);
    return flRoll < flHitChance;
}




















BOOL PowerCheckToDodge(const PowerAPIAttackPayload & tPayload, const ActorCommonData & tACDTarget, ActorCommonData * ptACDAttacker)
{
    if (tPayload.bAlwaysHits)
        return FALSE;
    int32 nLevel = tACDTarget.AttributesGetInt(FastAttribKey(ATTRIBUTE_EFFECTIVE_LEVEL));
    float flDodge = PowerComputeDodgeChance(tACDTarget, nLevel, tPayload.eDeliveryMechanism, FALSE);
    if (!(flDodge > 0.000001f))
        return FALSE;
    SNO snoPower = PowerGetSNO(tPayload.idPower);
    float flDodgeRoll = RandFloat_Internal(SGameGlobalsGet().rng);
    CombatLog::LogEvent(1, tACDTarget.id, ptACDAttacker ? ptACDAttacker->id : ACDID_INVALID, snoPower, flDodge, flDodgeRoll, 0);
    if (flDodgeRoll < flDodge)
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(PlayerGetByACD(tACDTarget.id));
        if (tEvaluationData.GetPlayer())
        {
            tEvaluationData.SetPower(snoPower);
            tEvaluationData.SetACD(ptACDAttacker);
            tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_PLAYER_DODGED_ATTACK, snoPower, 1);
        }
        return TRUE;
    }
    return FALSE;
}









































void PowerAddAffectedACDs(PowerID idPower, XPooledDataIDList & listTargets, const ACDID idACDSource)
{
    if (PowerNoAffectedACD(AutoPowerDefinitionR(PowerGetSNO(idPower))) == TRUE)
        return;

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idTarget = (ACDID)listTargets.GetNext(pos);
        PowerAddAffectedACD(idPower, idTarget, idACDSource);
    }
}








void PowerAddAffectedACD(PowerID idPower, const ACDID idACDTarget, const ACDID idACDSource)
{
    AffectedACD tAffectedACD;
    Power * ptPower = PowerGet(idPower);

    if (PowerNoAffectedACD(AutoPowerDefinitionR(ptPower->snoPower)) == TRUE)
        return;

    tAffectedACD.idACDTarget = idACDTarget;

    if (PowerTracksAffectedACDInstanceWide(AutoPowerDefinitionR(ptPower->snoPower)))
        tAffectedACD.idACDSource = ACDID_INVALID;
    else
        tAffectedACD.idACDSource = idACDSource;

    if (!ptPower->listAffectedACDs.Find(tAffectedACD))
        ptPower->listAffectedACDs.AddTail_Tracked(tAffectedACD);
}



























































BOOL PowerHasAffectedACD(PowerID idPower, const PowerDefinition * pdefPower, const ACDID idACDTarget, const ACDID idACDSource)
{
    AffectedACD tAffectedACD;
    AffectedACD tAffectedACDGlobal;
    const Power * ptPower = PowerGet(idPower);

    if (PowerNoAffectedACD(AutoPowerDefinitionR(pdefPower)) == TRUE)
        return FALSE;

    tAffectedACD.idACDSource = idACDSource;
    tAffectedACD.idACDTarget = idACDTarget;
    if (ptPower->listAffectedACDs.Find(tAffectedACD))
        return TRUE;

    tAffectedACDGlobal.idACDSource = ACDID_INVALID;
    tAffectedACDGlobal.idACDTarget = idACDTarget;
    if (ptPower->listAffectedACDs.Find(tAffectedACDGlobal))
        return TRUE;

    return FALSE;
}















void PowerRunChild(PowerID idPower, void * pData)
{


    Power * ptPower = PowerGet(idPower);

    ActorCommonData * ptACDOwner = PowerGetOwner(ptPower);
    if (!ptACDOwner)
    {
        return;
    }

    SNO snoPower = PowerGetChildPower(idPower);



    uint32 dwReasons = PowerFailureReasons(*ptACDOwner, AutoPowerDefinitionR(snoPower), &cgTargetNone, FALSE, FALSE);

    uint32 dwMask = ~((1 << PS_CANCELABLE_SKILLS_ACTIVE) | (1 << PS_UNCANCELABLE_SKILLS_ACTIVE) | (1 << PS_WRONG_WEAPON));



    if ((dwReasons &
         dwMask) == 0)
    {
        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = tPowerStartData.idACDAlternateSource = ptACDOwner->id;

        tPowerStartData.snoPower = snoPower;

        if (!ptPower->tTarget.IsInvalidForSource(ptACDOwner))
        {
            tPowerStartData.tTarget = ptPower->tTarget;
        }
        else
        {
            tPowerStartData.tTarget = cgTargetNone;
        }
        tPowerStartData.ptExtraPowerStartData = pData;

        PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
    }
}




int32 PowerGetLevel(const PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    if (!ptPower)
        return 0;

    if (ptPower->nSkillLevel != -1)
        return ptPower->nSkillLevel;

    ActorCommonData * ptACD = ACDTryToGet(ptPower->idACD);
    return PowerGetLevel(ptACD, ptPower->snoPower);
}










void PowerResetSkills(ActorCommonData & tACD, BOOL bNewHero)
{


    XGrowableList<FastAttribKey, const FastAttribKey &> listSkills;

    AttribGroupID idFastAttrib = tACD.AttributesGetAttributeGroup();
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        FastAttribValue tValue = tIt.tEntryBuffer.tValue;

        if (KeyGetAttrib(tKey) != ATTRIBUTE_SKILL)
        {
            continue;
        }

        int32 nSkillLevel = AttribValueToInt(tValue);
        if (nSkillLevel <= 0)
        {
            continue;
        }


        listSkills.AddTail_Tracked(tKey);
    }

    POS pos = listSkills.GetHeadPosition();
    while (pos)
    {
        FastAttribKey tKey = listSkills.GetNext(pos);

        FastAttribSetValueInt(idFastAttrib, tKey, 0);
    }

    int32 nCurrentLevel = FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_LEVEL));


    if (ACDIsPlayer(tACD.id))
    {



        Player * ptPlayer = PlayerGetByACD(tACD.id);
        if (ptPlayer)
        {
            HotbarButtonData tEmptyButtonData;
            tEmptyButtonData.Empty();

            for (int32 i = 0; i < 7; i++)
            {
                PlayerSetHotbarBtnAssignment(ptPlayer, SkillButton(i), tEmptyButtonData);
            }
            PlayerSetPotionBtnAssignment(ptPlayer, tEmptyButtonData);

            for (int32 i = 0; i < 6; i++)
            {
                SPlayerAssignToActiveSkillSlot(*ptPlayer, SNO_NONE, -1, i, FALSE);
            }
            for (int32 i = 0; i < 4; i++)
            {
                SPlayerAssignToTraitSlot(*ptPlayer, SNO_NONE, i, FALSE);
            }
            for (int32 i = 0; i < 3; i++)
            {
                SPlayerAssignToLegendaryPowerSlot(*ptPlayer, -1, i);
            }
        }


        HeroEnsureHeroHasAllSkillAttributes(tACD, bNewHero);

        ACDInventoryEquipmentOrSkillChanged(tACD);
    }
}




inline BOOL PowerHasTargettedACD(PowerID idPower, ACDID idACD)
{
    Power * ptPower = PowerGet(idPower);
    return ptPower->listACDsTargetted.Find(idACD) != NULL;
}














void PowerAddTargettedACD(PowerID idPower, ACDID idACD)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
    {
        if (!ptPower->listACDsTargetted.Find(idACD))
            ptPower->listACDsTargetted.AddTail_Tracked(idACD);
    }
}
































































































































void PowerSetParam(PowerID idPower, uint32 dwParam)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
        ptPower->dwParam = dwParam;
}









uint32 PowerGetParam(PowerID idPower)
{
    Power * ptPower = PowerGet(idPower);
    if (ptPower)
        return ptPower->dwParam;
    return (uint32)-1;
}








void PowerStartDataInit(PowerStartData & tPowerStartData)
{
    tPowerStartData.idACD = ACDID_INVALID;
    tPowerStartData.idACDAlternateSource = ACDID_INVALID;
    tPowerStartData.snoPower = SNO_NONE;
    tPowerStartData.tTarget.Clear();
    tPowerStartData.ptExtraPowerStartData = NULL;
    tPowerStartData.nSkillLevel = -1;
    SigmaMemoryClear(&tPowerStartData.tPreplayedAnimData, sizeof(tPowerStartData.tPreplayedAnimData));
    tPowerStartData.nComboLevel = 0;
    tPowerStartData.dwAnimTagToUse = (uint32)-1;
    tPowerStartData.bFreeCast = FALSE;
    tPowerStartData.bDoesntAnimate = FALSE;
    tPowerStartData.bAllowHitProcs = FALSE;
    tPowerStartData.eRuneTypeOverride = -1;
    tPowerStartData.dwParam = (uint32)-1;
}




uint8 * PowerGetSkillData(PowerID idPower, uint32 dwSizeDesired)
{
    Power * ptPower = PowerGet(idPower);
    return ptPower->pPowerTemplateData;
}








void PowerAbortChannelled(ACDID idACDPlayer, SNO snoPower)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
        return;
    ActorID idActor = (ActorID)ACDGetOwner(idACDPlayer);
    if (idActor == ACTORID_INVALID)
        return;
    if (!PowerIsChannelled(*ptACDPlayer, AutoPowerDefinitionR(snoPower)))
        return;
    PowerID idPowerPrimary = ActorPowersGetPrimary(idActor);
    Power * ptPrimaryPower = PowerTryToGet(idPowerPrimary);
    if (ptPrimaryPower && ptPrimaryPower->snoPower == snoPower)
    {
        SpecificPowerSendMessage(idPowerPrimary, 10, (void *)-1);
        PowerInterrupt(idPowerPrimary, TRUE);
    }
}








void PowerAbortLooping(ACDID idACDPlayer, SNO snoPower)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
        return;
    ActorID idActor = (ActorID)ACDGetOwner(idACDPlayer);
    if (idActor == ACTORID_INVALID)
        return;
    PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), idACDPlayer);
    if (eTemplate == LOOPING_ANIMATION_POWER || eTemplate == GIZMO_POWER)
    {
        PowerID idPowerPrimary = ActorPowersGetPrimary(idActor);
        Power * ptPrimaryPower = PowerTryToGet(idPowerPrimary);
        if (ptPrimaryPower && ptPrimaryPower->snoPower == snoPower)
        {
            PowerInterrupt(idPowerPrimary, TRUE);
            ActorPowersRemovePrimary(idActor, idPowerPrimary);
        }
    }
}










void PowerAbortClientWalk(ACDID idACDPlayer, SNO snoPower)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (!ptACDPlayer)
        return;
    ActorID idActor = (ActorID)ACDGetOwner(idACDPlayer);
    if (idActor == ACTORID_INVALID)
        return;
    PowerID idPowerPrimary = ActorPowersGetPrimary(idActor);
    Power * ptPrimaryPower = PowerTryToGet(idPowerPrimary);
    if (ptPrimaryPower && ptPrimaryPower->snoPower == snoPower)
    {
        if (!(ptPrimaryPower->dwFlags & (1 << PF_USING_CLIENT_AUTO_WALK)))
            return;
        PowerAtDestination(idPowerPrimary, ACDID_INVALID, ACDID_INVALID, 0);
        ptPrimaryPower->dwFlags |= (1 << PF_INTERRUPTABLE);
        PowerDetachFromActor(idPowerPrimary, FALSE);
        Actor * ptActor = ActorGet(idActor);
        if (ptActor->ptSActorMovement)
            ptActor->ptSActorMovement->StopAndSyncPosition(FALSE, FALSE);
        return;
    }
    SNO snoQueuedPower = PlayerBrainGetQueuedPower(idACDPlayer);
    if (snoQueuedPower != SNO_NONE && snoQueuedPower == snoPower)
        PlayerBrainClearQueuedPower(idACDPlayer);
}




























float PowerGetFormulaValue(PowerID idPower, uint32 dwPowerTag)
{
    float flValue = 0.0f;
    Power * ptPower = PowerTryToGet(idPower);
    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptPower)
    {
        flValue = PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(ptPower->snoPower), dwPowerTag, ptPower->nSkillLevel, ptPower->idFastAttrib, ptPower->dwCurrentContactFrame);
    }
    return flValue;
}







float PowerGetMonsterFormulaValue(Power * ptPower, uint32 dwMonsterTag)
{
    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);
    SNO snoMonster = ActorGetMonsterSNO(ptPower->snoCasterActor);
    ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
    const CTagMap * pTagMap = pdefMonster->hTagMap;
    float flValue = 0.0f;
    switch (TagGetVariantType(pTagMap, dwMonsterTag))
    {
    case TAGVARIANTTYPE_FORMULA:
    {
        uint32 dwSize;
        uint8 * pbFormula = TagMapGetBBEFormula(pTagMap, dwMonsterTag, dwSize);
        if (dwSize)
        {
            AttribGroupID idFastAttrib = ptPower->idFastAttrib;
            int & rnPowerFormulaStackCount = ThreadGlobals::Get().m_nPowerFormulaStackCount;
            if (rnPowerFormulaStackCount >= POWER_FORMULA_STACK_OVERFLOW_LIMIT)
            {
                const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwMonsterTag);
                (void)ptInfo;
            }
            else
            {
                ++rnPowerFormulaStackCount;
                flValue = PowerGetFormulaValueAtLevel(idFastAttrib, ACDTryToGet(ptPower->idACD), pdefPower->tHeader.snoHandle, pbFormula, dwSize, -1);
                --rnPowerFormulaStackCount;
            }
        }
        break;
    }
    case TAGVARIANTTYPE_INT:
        flValue = (float)TagMapGetInt(pTagMap, dwMonsterTag);
        break;
    case TAGVARIANTTYPE_FLOAT:
        flValue = TagMapGetFloat(pTagMap, dwMonsterTag);
        break;
    default:
    {
        const AxeTagInfo * pTagInfo = TagMapGetTagInfo(pTagMap, dwMonsterTag);
        (void)pTagInfo;
        break;
    }
    }
    return flValue;
}

































CRefString PowerGetMonsterSNONameFormulaValue(Power * ptPower, uint32 dwMonsterTag)
{


    ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);

    SNO snoMonster = ActorGetMonsterSNO(ptPower->snoCasterActor);
    ScopedMonsterDefinitionLockR pdefMonster(snoMonster);


    const CTagMap * pTagMap = pdefMonster->hTagMap;

    if (TagGetVariantType(pTagMap, dwMonsterTag) == TAGVARIANTTYPE_SNO)
    {
        SNO snoValue = TagMapGetSNO(pTagMap, dwMonsterTag);
        const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwMonsterTag);
        SNOGroup snoGroup = AxeTagTypeToSNOGroup(ptInfo->eType);
        return SNOToString(snoGroup, snoValue, TRUE);
    }
    else
    {
        const AxeTagInfo * pTagInfo = TagMapGetTagInfo(pTagMap, dwMonsterTag);
        (void)pTagInfo;
    }

    return CRefString("");
}












float PowerGetProcScalar(Power & tPower)
{
    if (tPower.bDoesntAnimate && !tPower.bAllowHitProcs)
        return 0.0f;
    RuneType eRuneType = PowerGetRuneType(&tPower);
    uint32 dwPowerTag;
    if (PowerIsComboPower(AutoPowerDefinitionR(tPower.snoPower)))
    {
        int32 nComboLevel = tPower.nComboLevel;
        switch (nComboLevel)
        {
        case 0:
            switch (eRuneType)
            {
            case RUNE_TYPE_A: dwPowerTag = TAG_POWER_RUNEA_COMBO1_PROC_SCALAR; break;
            case RUNE_TYPE_B: dwPowerTag = TAG_POWER_RUNEB_COMBO1_PROC_SCALAR; break;
            case RUNE_TYPE_C: dwPowerTag = TAG_POWER_RUNEC_COMBO1_PROC_SCALAR; break;
            case RUNE_TYPE_D: dwPowerTag = TAG_POWER_RUNED_COMBO1_PROC_SCALAR; break;
            case RUNE_TYPE_E: dwPowerTag = TAG_POWER_RUNEE_COMBO1_PROC_SCALAR; break;
            default: dwPowerTag = TAG_POWER_NORUNE_COMBO1_PROC_SCALAR; break;
            }
            break;
        case 1:
            switch (eRuneType)
            {
            case RUNE_TYPE_A: dwPowerTag = TAG_POWER_RUNEA_COMBO2_PROC_SCALAR; break;
            case RUNE_TYPE_B: dwPowerTag = TAG_POWER_RUNEB_COMBO2_PROC_SCALAR; break;
            case RUNE_TYPE_C: dwPowerTag = TAG_POWER_RUNEC_COMBO2_PROC_SCALAR; break;
            case RUNE_TYPE_D: dwPowerTag = TAG_POWER_RUNED_COMBO2_PROC_SCALAR; break;
            case RUNE_TYPE_E: dwPowerTag = TAG_POWER_RUNEE_COMBO2_PROC_SCALAR; break;
            default: dwPowerTag = TAG_POWER_NORUNE_COMBO2_PROC_SCALAR; break;
            }
            break;
        case 2:
            switch (eRuneType)
            {
            case RUNE_TYPE_A: dwPowerTag = TAG_POWER_RUNEA_COMBO3_PROC_SCALAR; break;
            case RUNE_TYPE_B: dwPowerTag = TAG_POWER_RUNEB_COMBO3_PROC_SCALAR; break;
            case RUNE_TYPE_C: dwPowerTag = TAG_POWER_RUNEC_COMBO3_PROC_SCALAR; break;
            case RUNE_TYPE_D: dwPowerTag = TAG_POWER_RUNED_COMBO3_PROC_SCALAR; break;
            case RUNE_TYPE_E: dwPowerTag = TAG_POWER_RUNEE_COMBO3_PROC_SCALAR; break;
            default: dwPowerTag = TAG_POWER_NORUNE_COMBO3_PROC_SCALAR; break;
            }
            break;
        default:
            return 0.0f;
        }
    }
    else
    {
        switch (eRuneType)
        {
        case RUNE_TYPE_A: dwPowerTag = TAG_POWER_RUNEA_PROC_SCALAR; break;
        case RUNE_TYPE_B: dwPowerTag = TAG_POWER_RUNEB_PROC_SCALAR; break;
        case RUNE_TYPE_C: dwPowerTag = TAG_POWER_RUNEC_PROC_SCALAR; break;
        case RUNE_TYPE_D: dwPowerTag = TAG_POWER_RUNED_PROC_SCALAR; break;
        case RUNE_TYPE_E: dwPowerTag = TAG_POWER_RUNEE_PROC_SCALAR; break;
        default: dwPowerTag = TAG_POWER_NORUNE_PROC_SCALAR; break;
        }
    }
    return PowerGetFormulaValue(tPower.id, dwPowerTag);
}









































































BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax)
{
    WorldPlace tStart;
    tMovement.GetPlace(tStart);
    return PowerComputeDestination(snoPower, tMovement, placeTarget, tStart, placeResult, bActivateTriggers, flSearchRadiusMax);
}









BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeTarget, const WorldPlace & placeStart, WorldPlace & placeResult, BOOL bActivateTriggers, float flSearchRadiusMax)
{







    ACDCollSettings tCollSettingsDuring;
    ACDCollSettings tCollSettingsDest;
    tMovement.GetCollSettings(snoPower, 0, tCollSettingsDuring);
    tMovement.GetCollSettings(snoPower, 1, tCollSettingsDest);

    uint32 uPathRequiredNoOverlapFlags = 0;
    if (snoPower != SNO_NONE)
    {
        ScopedPowerDefinitionLockR pdefPower(snoPower);
        if (pdefPower)
        {
            if (pdefPower->tPower.uFlags & (1 << POWERBIT_REQUIRES_WALKING_PATH_TO_TARGET))
            {
                uPathRequiredNoOverlapFlags = tCollSettingsDuring.uDynamicFlagsNoOverlap;
            }
        }
    }

    PathPoint2DList listPathPoints;
    listPathPoints.SetPool(NavCellPathGetPointPool());
    BOOL bPathFound = PowerComputeDestination(tCollSettingsDuring, tCollSettingsDest, tMovement, placeTarget, placeStart, placeResult, uPathRequiredNoOverlapFlags, flSearchRadiusMax, &listPathPoints);

    if (bActivateTriggers && uPathRequiredNoOverlapFlags)
    {
        if (bPathFound)
        {

            POS pos = listPathPoints.GetHeadPosition();
            Vector2D vLastPoint = placeStart.wp.As2D();
            while (pos)
            {
                Vector2D & vCurrPoint = listPathPoints.GetNext(pos);
                tMovement.ActivateTriggers(vLastPoint, vCurrPoint);
                vLastPoint = vCurrPoint;
            }
        }
        else
        {

            tMovement.ActivateTriggers(placeStart.wp.As2D(), placeResult.wp.As2D());
        }
    }


    return bPathFound;
}





BOOL PowerComputeDestination(const ACDCollSettings & tCollSettingsDuring,
                             const ACDCollSettings & tCollSettingsDest,
                             const ActorMovement & tMovement,
                             const WorldPlace & placeTarget,
                             const WorldPlace & placeStart,
                             WorldPlace & placeResult,
                             uint32 uRequirePathNoOverlapFlags,
                             float flSearchRadiusMax,
                             PathPoint2DList * ptListPathPoints)

{
    placeResult = placeTarget;

    float flRadius = tMovement.GetRadius(FALSE);

    Circle circleStart;
    circleStart.Set(placeStart.wp.x, placeStart.wp.y, flRadius);

    Vector3D vDelta = placeResult.wp - placeStart.wp;
    if (vDelta.MagnitudeSquared() > 250000.0f)  // literal from the image (0x7100e3c2a8); the DWARF names no constant
    {
        placeResult = placeStart;
        return FALSE;
    }



    BOOL bPathOk = TRUE;
    if (uRequirePathNoOverlapFlags)
    {
        ACDCollParams tACDCollParams;
        tACDCollParams.uObjectNoOverlapFlags = uRequirePathNoOverlapFlags;
        tACDCollParams.uSpecialFlags |= 1;

        ActorCommonData * ptACD = ACDTryToGet(tMovement.GetACDID());
        if (ptACD)
        {
            tACDCollParams.eTeam = ptACD->GetTeam();
        }
        else
        {
            tACDCollParams.eTeam = TEAM_INVALID;
        }

        PathPoint2DList listPathPoints;
        if (!ptListPathPoints)
        {

            listPathPoints.SetPool(NavCellPathGetPointPool());
            ptListPathPoints = &listPathPoints;
        }

        PathResult ePathResult = tMovement.FindPath(placeTarget, *ptListPathPoints, 1, 0, &tACDCollParams);
        bPathOk = (ePathResult == PATH_RESULT_NOT_NECESSARY || ePathResult == PATH_RESULT_FOUND);





        if (ePathResult == PATH_RESULT_NOT_NECESSARY && ptListPathPoints->IsEmpty())
        {



            ptListPathPoints->AddTail_Tracked(placeStart.wp.As2D());
            ptListPathPoints->AddTail_Tracked(placeTarget.wp.As2D());
        }
    }

    ACDID idACD = tMovement.GetACDID();

    WorldCollParams tWorldCollParamsDuring(tCollSettingsDuring);
    WorldCollParams tWorldCollParamsDest(tCollSettingsDest);

    Vector3D vFoundLoc(placeResult.wp);
    NavCellHandle hNavCellTarget;

    NavCellHandle hNavCellStart;
    NavCellHandleFromWorldPlace(placeStart, hNavCellStart);

    BOOL bFoundDest = FALSE;
    if (bPathOk)
    {
        bFoundDest = WorldCollSearchForLocation(placeResult,
                                                tCollSettingsDest.uStaticOneRequiredFlags,

                                                tCollSettingsDest.uDynamicFlagsNoOverlap,
                                                flRadius,
                                                0.0f,
                                                flSearchRadiusMax,
                                                idACD,
                                                vFoundLoc,
                                                hNavCellTarget,
                                                (Team)tCollSettingsDest.eTeam, 0, ACDID_INVALID);

        WorldCollResult tResultDuring;
        WorldCollSweptCircle(placeResult.idSWorld, hNavCellStart, circleStart, vFoundLoc.As2D(), tWorldCollParamsDuring, 3, 1, idACD, tResultDuring);








        bPathOk = !tResultDuring.bCollision;
    }

    if (bFoundDest && bPathOk)
    {

        placeResult.wp = vFoundLoc;
    }
    else
    {


        WorldCollResult tResultDest;
        WorldCollSweptCircle(placeResult.idSWorld, hNavCellStart, circleStart, vFoundLoc.As2D(), tWorldCollParamsDest, 3, 1, idACD, tResultDest);









        placeResult.wp = tResultDest.wpEnd;
    }

    tMovement.SnapPlaceToGround(placeResult);



    if (NXSDK::IsNaNFloat(placeResult.wp.x) ||
        NXSDK::IsNaNFloat(placeResult.wp.y) ||
        NXSDK::IsNaNFloat(placeResult.wp.z))
    {













        placeResult = placeStart;
        return FALSE;
    }


    return bPathOk;
}


ACDID PowerStartOnProxy(ACDID idACDSource, SNO snoProxyActor, SNO snoPower, WorldPlace tPlace, Quaternion qOrientation, Target tTarget, uint32 dwDelay, BOOL bCopyAttributesFromSource)
{










    ActorCommonData & tACDSource = ACDGet(idACDSource);
    int32 nLevel = tACDSource.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    Team eTeam = tACDSource.GetNaturalTeam();
    if (eTeam == -1)
    {
        eTeam = 0;
    }



    CRefString sActorName("Power_Proxy_");
    sActorName += SNOToString(ACTOR_SNO, snoProxyActor, FALSE);
    ActorAddStruct tActorAdd;

    tActorAdd.tPlace = tPlace;
    tActorAdd.qOrientation = qOrientation;
    tActorAdd.eTeam = eTeam;
    tActorAdd.snoActor = snoProxyActor;
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = snoProxyActor;
    tActorAdd.tMarkersName = MarkerStringHash(sActorName);
    tActorAdd.nLevel = nLevel;
    ScenesCommonNavMeshNodeFromPlace(tPlace, &tActorAdd.tNavMeshHandle, FALSE, FALSE);

    ActorID idActor = ActorAdd(tActorAdd);

    if (idActor == ACTORID_INVALID)
    {
        return ACDID_INVALID;
    }


    Actor * ptActor = ActorTryToGet(idActor);


    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE), 1);
    ActorTriggerAttributeUpdate(ptActor->id);

    SNO snoProxyDelayedPower = GlobalSNOGet(GLOBALSNO_POWERS_PROXY_DELAYED_POWER);
    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = tACD.id;
    tPowerStartData.snoPower = snoProxyDelayedPower;
    tPowerStartData.tTarget = tTarget;
    tPowerStartData.ptExtraPowerStartData = (void *)(uintptr_t)dwDelay;

    PowerID idPower;
    AttribGroupID idFastAttrib = -1;
    if (bCopyAttributesFromSource)
    {
        idFastAttrib = FastAttribGroupCreate(0, -1);
        tACDSource.AttributesMergeAttributeGroup(idFastAttrib);
        idPower = PowerStartEx(tPowerStartData, tACDSource.idFastAttrib, FALSE, FALSE);
    }
    else
    {
        idPower = PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
    }

    if (idPower != -1)
    {
        PowerSetParam(idPower, snoPower);
        PowerSetAlternateSource(idPower, idACDSource);
    }

    if (bCopyAttributesFromSource)
    {
        tACDSource.AttributesUnmergeAttributeGroup(idFastAttrib);
        FastAttribGroupDelete(idFastAttrib);
    }

    return tACD.id;
}




void PowerSetStoredInt(Power * ptPower, LPCSTR szName, int32 nValue)
{
    uint32 dwHash = XHashKey<LPCSTR>(szName);
    ptPower->mapIntValues.SetAt_Tracked(dwHash, nValue);
}






int32 PowerGetStoredInt(Power * ptPower, LPCSTR szName)
{
    int32 nValue = 0;
    uint32 dwHash = XHashKey<LPCSTR>(szName);
    ptPower->mapIntValues.Lookup(dwHash, nValue);
    return nValue;
}








void PowerSetStoredFloat(Power * ptPower, LPCSTR szName, float flValue)
{
    uint32 dwHash = XHashKey<LPCSTR>(szName);
    ptPower->mapFloatValues.SetAt_Tracked(dwHash, flValue);
}






float PowerGetStoredFloat(Power * ptPower, LPCSTR szName)
{
    float flValue = 0.0f;
    uint32 dwHash = XHashKey<LPCSTR>(szName);
    ptPower->mapFloatValues.Lookup(dwHash, flValue);
    return flValue;
}








RuneType PowerGetRuneType(const Power * ptPower)
{
    if (FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_A, ptPower->snoPower)) > 0)
        return 0;
    if (FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_B, ptPower->snoPower)) > 0)
        return 1;
    if (FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_C, ptPower->snoPower)) > 0)
        return 2;
    if (FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_D, ptPower->snoPower)) > 0)
        return 3;
    if (FastAttribGetValueInt(ptPower->idFastAttrib, MakeAttribKey(ATTRIBUTE_RUNE_E, ptPower->snoPower)) > 0)
        return 4;
    return -1;
}














DamageType PowerGetDamageType(const Power * ptPower)
{
    RuneType eRune = PowerGetRuneType(ptPower);
    return PowerGetDamageType(AutoPowerDefinitionR(ptPower->snoPower), eRune);
}




BrainAction PowerGetBrainAction(SNO snoPower)
{
    BrainAction eAction = -1;
    if (snoPower != SNO_NONE)
        eAction = PowerGeneralTagGetInt(AutoPowerDefinitionR(snoPower), TAG_POWER_BRAIN_ACTION_TYPE);
    return eAction;
}









BOOL PowerCheckForSpecialSkillDeath(const SNO snoPower, const RuneType eRune)
{
    if (snoPower == SNO_NONE)
        return FALSE;
    float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);
    int32 nTagDeathType;
    int32 nTagChance;
    PowerGetSpecialDeathTags(AutoPowerDefinitionR(snoPower), eRune, nTagDeathType, nTagChance);
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    float flChance = PowerGeneralTagGetFloat(AutoPowerDefinitionR(snoPower), nTagChance);
    return flRandom < flChance;
}





















float PowerGetClientRunSpeed(const Power & tPower)
{
    float flSpeed = 0.0f;
    if (tPower.snoPower != SNO_NONE)
    {
        PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(tPower.snoPower), tPower.idACD);
        if (eTemplate == ATTACKMOVE_POWER || eTemplate == SPECIALWALK_POWER)
        {
            ActorCommonData * ptACD = ACDTryToGet(tPower.idACD);
            if (ptACD && PowerShouldUseClientWalking(*ptACD, AutoPowerDefinitionR(tPower.snoPower)))
            {
                flSpeed = ptACD->GetWalkingSpeed(TAG_ACTOR_ANIMATION_RUN);
                flSpeed *= PowerGetFormulaValue(tPower.id, TAG_POWER_WALKING_SPEED_MULTIPLIER);
            }
        }
    }
    return flSpeed;
}










void PowerUpdateServerVariables()
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    if (ptPowerGlobals)
    {
        ptPowerGlobals->nMaxAreaEffectTargetCount = s_varMaxAreaEffectTargetCount.Get();
        ptPowerGlobals->nMaxProjectileImpactCount = s_varMaxProjectileImpactCount.Get();
    }
}




int32 PowerGetMaxAreaEffectTargetCount()
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    if (ptPowerGlobals)
        return ptPowerGlobals->nMaxAreaEffectTargetCount;
    return 64;
}







int32 PowerGetMaxProjectileImpactCount()
{
    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    if (ptPowerGlobals)
        return ptPowerGlobals->nMaxProjectileImpactCount;
    return 64;
}
