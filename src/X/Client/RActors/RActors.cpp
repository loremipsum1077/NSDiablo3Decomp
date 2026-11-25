// X/Client/RActors/RActors.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Attachments.h"
#include "../Gfx/GfxInternal.h"
// Stand-in for ../Battlenet/Subsystems/ScreenManager.h (not in src/ yet): its ldn statics.
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../Particle.h"
#include "../ParticleInternal.h"
#include "RActors.h"
#include "../SubObjectGfx.h"
#include "../UserInterface/Controls/UITextControl.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../Definitions/ShaderMapDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Act.h"
#include "../Physics/Ragdoll.h"
#include "RActorAnimation.h"
#include "RActorAttachments.h"
#include "../CPlayer.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/Player.h"
#include "../Definitions/CAttachmentDefinition.h"
#include "../../Common/Definitions/AttachmentDefinition.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Marker/Marker.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/World.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/WorldPlace.h"
#include "../../Common/Game.h"
#include "../../Common/Gizmo.h"
#include "../Gfx/GfxMaterial.h"
#include "../../Common/GameBalance.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Definitions/EncounterDefinition.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../SNOPreload.h"
#include "../Cloth.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../Physics/AttachedRagdoll.h"
#include "../../Common/VisualInventory.h"
// Includes and declarations needed by RActorsACDAttributeChanged / sInitializeBuffEffectsForPower (DWARF lines 3786, 7894).
#include "../../Common/Definitions/PowerDefinition.h"
#include "../UserInterface/Interface/UIGame.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../CPlayerInput.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../UserInterface/Interface/UIMinimap.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../UserInterface/Interface/UIToolTipManager.h"
#include "../UserInterface/Interface/UITutorials.h"
#include "../Prefetch.h"
#include "../Movement/CActorMovement.h"
Attachment * AttachmentTryToGet(const AttachmentID idAttachment);  // X/Client/Attachments.cpp:106
void sAddRope(RActorID idRActor, SNO snoRope);  // X/Client/RActors/RActors.cpp:7365
void RActorAttachmentDelete(RActorID idRActor, const AttachmentID idAttachment);  // X/Client/RActors/RActorAttachments.cpp:791
static void sInitializeBuffEffectsForPower(const PowerDefinition * pdefPower, const SNO snoPower, const uint32 dwEffectGroupTag, const Attrib eRuneAttribNone, const Attrib eRuneAttribA, const Attrib eRuneAttribB, const Attrib eRuneAttribC, const Attrib eRuneAttribD, const Attrib eRuneAttribE);  // X/Client/RActors/RActors.cpp:3786
Player * PlayerGetByACD(const ACDID idACD);  // X/Common/Player.cpp:1467


void AnimPlayerIdleAnimationChanged(ActorCommonData & tACD, const uint32 dwAnimTag);  // X/Common/AnimPlayer.cpp:1506
void CQuestsUpdateAllActors();  // X/Client/Quests/CQuests.cpp:283
AttachmentID RActorAttachExistingRActor(const RActorID idRActorMain, const RActorID idRActorAttachment, HardpointHash szHardpointName);  // X/Client/RActors/RActorAttachments.cpp:2348
float RActorAnimationGetSpeedScale(const RActor & tRActor);  // X/Client/RActors/RActorAnimation.cpp:1554
void RActorAnimationSetSpeedScale(RActor & tRActor, const float flScale);  // X/Client/RActors/RActorAnimation.cpp:1568
uint32 AnimPlayerGetAnimEndTime(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp:2018
void RActorAnimationSetPaused(RActor & tRActor, BOOL bPaused);  // X/Client/RActors/RActorAnimation.cpp:1583
void CPlayerInputClearCachedPower(Player * ptPlayer);  // X/Client/CPlayerInput.cpp:7208

CursorMgr & PlayerGetCursorMgr();  // X/Client/CPlayerInput.cpp:6603
void CPlayerChangedTeams(RActorID idRActor);  // X/Client/CPlayer.cpp:3241
BOOL QuestIsACDObjective(const ActorCommonData & tACD);  // X/Common/Quests/Quests.cpp:2094
void CQuestsFlagACD(ActorCommonData & tACD);  // X/Client/Quests/CQuests.cpp:229

void RActorAnimationLookAt(RActorID idSource, RActorID idTarget);  // X/Client/RActors/RActorAnimation.cpp:1764
void AnimationChannelSetPercentComplete(AnimChannel & tChannel, float flFraction);  // X/Client/Animation.cpp:667
BOOL PowerBuffIsDisplayed(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);  // X/Common/Powers/PowersCommon.cpp:1415
namespace UIBuffs { void HandleBuffEndTickAttributeChange(const FastAttribValue tValue, const Attrib eAttribCount, const SNO snoPower, const int32 nBuffIndex, const ActorCommonData & tACD); }  // X/Client/UserInterface/Interface/UIBuffs.cpp:2165
BOOL PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);  // X/Common/Powers/PowersCommon.cpp:1719
BOOL CPlayerInputCancelMode(Player * ptPlayer, BOOL bStopDriverMode);  // X/Client/CPlayerInput.cpp:834
void CPlayerInputOtherListenerReceivedAction(Player * ptPlayer, UIAction eAction, uint32 nDelay);  // X/Client/CPlayerInput.cpp:7007
namespace UIBuffs { void HandleBuffCountAttributeChange(const FastAttribValue tValue, const SNO snoPower, const int32 nBuffIndex, const ActorCommonData & tACD); }  // X/Client/UserInterface/Interface/UIBuffs.cpp:2233

void CHirelingSetAppearance(RActor * ptRActor, HirelingClass eClass, int32 nLevel);  // X/Common/Hireling.cpp:861

void AnimPlayerWeaponSetChanged(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp:1804
namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); }  // X/Common/Tutorials.cpp:181
void ACDUpdateAppearance(ACDID idACD);  // X/Client/CActorCommonData.cpp:81
namespace UIJewelUpgrade { void JewelUpgradeMaxChanged(); }  // X/Client/UserInterface/Console/UIConsoleJewelUpgrade.cpp:668
void RActorUpdateClientTransform(RActor * ptRActor, const ActorDefinition * pdefActor, const ActorCommonData * ptACD);  // X/Client/RActors/RActors.cpp:6025 (missing in src; 3652 bytes in the original)
void AnimPlayerUpdateAnimEndTime(ActorCommonData & tACD, uint32 dwNewEndTime);  // X/Common/AnimPlayer.cpp:2034
extern UITutorialManager gtTutorialMgr;  // X/Client/CPlayer.cpp:225


namespace UIShop
{
namespace Console
{
void Refresh(const Player * ptPlayer);  // X/Client/UserInterface/Console/UIShopConsole.cpp:2185
}
}
namespace UISkills
{
namespace Console
{
void Refresh(const Player * ptPlayer);  // X/Client/UserInterface/Console/UISkillsConsole.cpp:3461
}
}
namespace UIFollower
{
namespace Console
{
void Refresh();  // X/Client/UserInterface/Console/UIConsoleFollower.cpp:1882
}
}
namespace UISkills
{
namespace Console
{
void InitializeForPlayer(const int32 nPlayerIndex);  // X/Client/UserInterface/Console/UISkillsConsole.cpp:3138
}
}
namespace UISkills
{
namespace Console
{
void OnPlayerLeveled(const Player * ptPlayer);  // X/Client/UserInterface/Console/UISkillsConsole.cpp:3499
}
}
namespace UIFollower
{
namespace Console
{
void OnFollowerLeveled(const ActorCommonData & tACDHireling, const int32 nLevel);  // X/Client/UserInterface/Console/UIConsoleFollower.cpp:2101
}
}
#include "../CWorld.h"
BOOL CScenesIntersectLineSeg(StructureIntersectData & tStructIntersect, CSceneDataArrayIterateMode eIterateMode);  // X/Client/CSceneCollision.cpp:291
void LocationSetBounds(SNO snoActor, WorldLocation & tLocation, const ActorDefinition * pdefActor);  // X/Common/ActorCommonData.cpp:4326
void RActorComputeBoneBoundingSphere(RActor * ptRActor);  // X/Client/RActors/RActorCollision.cpp:261
void RActorUpdateSphereTreeNode(RActor * ptRActor, const Sphere & wsOld);  // X/Client/RActors/RActorCollision.cpp:280
void RActorFlagForDeletion(RActor * ptRActor);  // forward declaration: called at 6174, defined at 11473
void SmoothMovementUnitVector(const Angle aCurrentSpeed, const Vector3D & wvCurrentPosition, const Angle aMaxSpeed, const Angle aMaxAcceleration, const Angle aMaxDeceleration, const Vector3D & wvFinalPosition, Vector3D & wvNewPosition, Angle & aNewSpeed);  // X/Client/Camera/Movement.cpp:149
#include "../Emitter.h"
void EmitterGetRandomPoint(RNG & rng, const Vector3D & wpPointOffset, Emitter & tEmitter, Vector3D & wp, Vector3D & wvNormal, int32 & nBoneIndex);  // X/Client/Emitter.cpp:100










HardpointHash ACTOR_DEFAULT_EFFECT_QUESTINFO_HARDPOINT(AURA_HARDPOINT_NAMESTR);
HardpointHash ACTOR_DEFAULT_EFFECT_HARDPOINT(DEFAULT_HARDPOINT_NAMESTR);

enum {
    MAX_SUBOBJECT_IN_POOLED_HOLDER = 26,
    MAX_MONSTERS_CORPSES_BEFORE_FADING = 10,
    CRITTERS_DENSITY_DIVIDER = 1,
};







static const float FLOAT_INFINITE_LIFETIME = -1.0f;
static const float MONSTERS_CORPSES_LIFE_TIME_BEFORE_FADING = -1.0f;
static const float MONSTERS_CORPSES_LIFE_TIME_BEFORE_FADING_MULTIPLAYER = -1.0f;
static const uint32 INFINITE_OVERLAY_FADE_TIME = 0xffffffff;
static const float RACTOR_SCALE_EPSILON = 0.01f;
static const float FLINCH_TIMEOUT = 1.0f;
int32 LookIndexFromHash(const AppearanceDefinition * pdefAppearance, uint32 nHash);  // Definitions/CAttachmentDefinition.cpp
void RActorSettleCloth(RActor * ptRActor);
void RActorCollisionCapsulesUpdate(RActor * ptRActor);  // RActorCollision.cpp
void PlayerSetACD(Player & tPlayer, const ACDID idACD);  // X/Common/Player.cpp:1438
void PlayersSetMainPlayerRActor(Player * ptPlayer, RActorID idRActor);  // X/Client/CPlayerInput.cpp:6776
void CWorldEnter(SWorldID idSWorld, GameMode eMode);  // X/Client/CWorld.cpp:90
void CWorldChange(SWorldID idSWorld);  // X/Client/CWorld.cpp:1470
struct RigidAnim {  // size 0x1c
    Vector3D wvAxis;  // 0x00
    Angle a;  // 0x0c
    Angle aBob;  // 0x10
    Angle aVel;  // 0x14
    WorldDistance wdDist;  // 0x18
};
struct RActorSubObjectData {  // size 0x750
    SubObjectState tSubObjectStateArray[MAX_SUBOBJECT_IN_POOLED_HOLDER];  // 0x00
};
enum AppearanceAttributeEffectType {
    AAEFFECT_TYPE_GLOBAL_SNO = 0,
    AAEFFECT_TYPE_TAG = 1,
    AAEFFECT_TYPE_POWER_TAG = 2,
};
typedef XGrowableMap<FastAttribKey, FastAttribKey, AppearanceAttributeEffectType, AppearanceAttributeEffectType> AppearanceAttributeEffectTypeMap;
typedef XGrowableMap<FastAttribKey, FastAttribKey, int, int> AppearanceAttributeGlobalSNOMap;
typedef XGrowableMap<FastAttribKey, FastAttribKey, unsigned int, unsigned int> AppearanceAttributeEffectTagMap;
typedef XGrowableMap<FastAttribKey, FastAttribKey, unsigned int, unsigned int> AppearanceAttributePowerTagMap;
typedef XGrowableMap<int, int, int, int> AppearanceAttributePowerInitializedMap;
struct QueuedFlippyInfo {  // size 0x30
    Vector3D wpDest;  // 0x00
    SNO snoParticle;  // 0x0c
    PRTransform transform;  // 0x10
    SNO snoActorOverride;  // 0x2c
};
struct QueuedRActorInfo {  // size 0x14
    SNO snoAmbientOcclusionOverrideTex;  // 0x00
    uint32 dwFlags;  // 0x04
    uint32 dwServerCreationTick;  // 0x08
    float flLastTimeChecked;  // 0x0c
    BOOL bIsFlippy;  // 0x10
};
struct RActorGlobals {  // size 0x3f8
    uint32 dwRActorNumberCount;  // 0x00
    XChainedFixedBlockAllocator<RActorSubObjectData> poolRActorSubObjectData;  // 0x08
    XChainedFixedBlockAllocator<RigidAnim> poolRigidAnimations;  // 0x38
    CollisionCapsulePool poolCollisionCapsules;  // 0x68
    RActorIDListPooled * plistMonsterCorpses;  // 0x98
    RActorIDListPooled listMainRActors;  // 0xa0
    ParticleSystemID idLowHealthParticle;  // 0xc8
    ParticleSystemID idHunterNearbyParticle;  // 0xcc
    SoundID idLowHealthSound;  // 0xd0
    float flLastDeathTime;  // 0xd4
    uint32 dwCurrentClientCritterIndex;  // 0xd8
    blz::vector<QueuedFlippyInfo, blz::allocator<QueuedFlippyInfo> > queuedFlippies;  // 0xe0
    XGrowableMap<ACDNetworkName, ACDNetworkName, QueuedRActorInfo, QueuedRActorInfo &> queuedRActors;  // 0xf8
    AppearanceAttributeEffectTypeMap tAppearanceAttributeEffectTypeMap;  // 0x178
    AppearanceAttributeGlobalSNOMap tAppearanceAttributeGlobalSNOMap;  // 0x1f8
    AppearanceAttributeEffectTagMap tAppearanceAttributeEffectTagMap;  // 0x278
    AppearanceAttributePowerTagMap tAppearanceAttributePowerTagMap;  // 0x2f8
    AppearanceAttributePowerInitializedMap tAppearanceAttributePowerInitializedMap;  // 0x378
};
static_assert(sizeof(RActorGlobals) == 0x3f8, "RActorGlobals");
SNO RActorGetLevelArea(const RActorID idRActor);
RActorIDListPooled & RActorGetMainList();
const RActorID RActorGetTopmostOwner(const RActorID idRActor, BOOL bForEffects, BOOL fForIsInUI);
BOOL RActorIsHostile(const RActor * ptRActor);
BOOL RActorIsNecroCorpse(const RActor * ptRActor);
void RActorSetScale(RActor * ptRActor, const float flScale, BOOL bSetAsBaseScale, const ActorDefinition * pdefActor);
void RActorPreThreadSafeUpdate(RActor * ptRActor, ActorCommonData * ptACD, float flDeltaTime, const ActorDefinition * ptDefActor);
void RActorThreadSafeUpdate(RActor * ptRActor, Vector3D & wpActor, ActorCommonData * ptACD, float flDeltaTime, const ActorDefinition * ptDefActor);
void RActorPostThreadSafeUpdate(RActor * ptRActor, Vector3D & wpActor, ActorCommonData * ptACD, float flDeltaTime, const ActorDefinition * ptDefActor);
BOOL RActorHardpointCreateCentralFacing(Hardpoint & tNewHardpoint, RActorID idRActor, const Vector3D & wpLookatPointWorld, const BOOL bMustCreate);
static void sRActorRemoveEffectParticle(RActor & tRActor, FastAttribKey tKey);
static void sUpdateAllSubObjectLook(RActor * ptRActor, const AppearanceDefinition * pdefAppearance);
static void sUnloadAllSubObjectLook(RActor * ptRActor, const AppearanceDefinition * pdefAppearance);
static void sRActorSubObjectStatesInitialize(RActor * ptRActor);
static void sRActorSubObjectStatesDispose(RActor * ptRActor);
static void sRActorCancelLook(RActor & tRActor, int32 nLookToCancel);
void RActorApplyOverlay(const RActorID idRActor, const OverlayType eOverlayType);
BOOL ParticleSystemUpdateParamsForRActor(RActor * ptRActor, Vector3D & wpActorResult, float flDeltaTime);  // Particle.cpp:6810
void RActorUpdateBones(RActor * ptRActor, float flDeltaTime, const ActorDefinition * pdefActor, BOOL fForceUpdate);  // RActorAnimation.cpp
static RActorID sRActorCreate(const ACDID idACDExisting, const SNO snoActor, const SNO snoAmbientOcclusionOverrideTex, uint32 tName, const float flScale, const float flLife, const PRTransform & transform, const uint32 dwRActorAddFlags, const RenderContext eContext, SSceneID idSScene, const uint32 dwSActorCreationTick, const ActorDefinition * pdefActor, BOOL fWillHaveACD);
static void sRActorRemoveCosmeticBuff(RActor * ptRActor, CosmeticItemType eType);
RagdollType RActorGetRagdollType(const RActor * ptRActor);
SubObjectAppearance & AppearanceGetSubObjectAppearance(const AppearanceDefinition * ptDefinition, int32 nSubObjectLook, const int32 nSubObject, const GeoSetType eGeoSetType);  // AppearanceDefinition.h (AppearanceDefinition.cpp:4244)
void SubObjectStateInitFade(SubObjectState & tSubObjectState);  // SubObjectGfx.h (SubObjectGfx.cpp:3220)
static void sPreloadAllSubObjectLook(RActor * ptRActor, const AppearanceDefinition * pdefAppearance);
static void sRActorSetACD(RActor * ptRActor, const ACDID idACD, const ActorDefinition * pdefActor);
inline BOOL RActorIsEnvRActor(RActor & tRActor);
static RActorID sRActorCreateFromACD(ACDID idACD);
static void sRActorStartCorpseFadeTimer(RActor * ptRActor, const ActorCommonData * ptACD);
static void sRActorSetLifeForFade(RActor * ptRActor, const BOOL bRecurseForAttachments);
BOOL ACDIsHireling(const ActorCommonData & tACD);  // Hireling.h
void CQuestsUnFlagACD(ActorCommonData & tACD);  // CQuests.h
void CPlayersRemoveNecroCorpse(ActorCommonData * ptACD);  // CPlayer.h
void RActorACDEffectsRemove(const ACDID idACD);  // RActorACDEffects.h
void UIMinimap_ExitKnown(const ACDID idACD);  // UserInterface/UIMinimap.h
void RActorAnimationOnACDDelete(RActor & tRActor);  // RActorAnimation.h
void CACDInventoryDestroy(ActorCommonData & tACD);  // CACDInventory.h
Attachment * AttachmentFind(XPooledDataIDList & tAttachmentList, const uint32 dwID);  // Attachments.cpp
void AttachmentDelete(Attachment & tAttachment);  // Attachments.cpp
SNO RActorGetInheritedTagMapValueSNO(const RActor * ptRActor, const uint32 dwRActorTag, const uint32 dwMarkerTag);
GBID RActorGetInheritedTagMapValueGBID(const RActor * ptRActor, const uint32 dwRActorTag, const uint32 dwMarkerTag);
void ACDDelete(const ACDID idACD, BOOL bWorldDeleted);  // ActorCommonData.h (ActorCommonData.cpp:996)
namespace UIInventory { namespace Console {
    void OnInventoryChanged(const ActorLocation & tOldLocation, const ActorLocation & tNewLocation, BOOL bArg0, BOOL bArg1);
    void OnStashChanged(const ActorLocation & tOldLocation, const ActorLocation & tNewLocation, BOOL bArg0);
    void OnShopChanged(const ActorLocation & tOldLocation, const ActorLocation & tNewLocation, BOOL bArg0, BOOL bArg1);
} }
namespace UIFollower { namespace Console { void OnInventoryChanged(const ActorLocation & tOldLocation, const ActorLocation & tNewLocation, BOOL bArg0); } }
namespace UISkills { namespace Console { void OnInventoryChanged(const ActorLocation & tOldLocation, const ActorLocation & tNewLocation, BOOL bArg0); } }
namespace UIParty { namespace Console { void OnInventoryChanged(const ActorLocation & tOldLocation, const ActorLocation & tNewLocation, BOOL bArg0); } }
void ACDReapplyVisualInventory(const ACDID idACD);  // VisualInventory.h
void ParticleSystemLostRActor(RActor * ptRActor);  // Particle.h
void RActorCollisionDispose(RActor * ptRActor);  // RActorCollision.h
void RActorCollisionCapsulesDispose(RActor * ptRActor);  // RActorCollision.h
void RActorAnimDispose(RActor * ptRActor);  // RActorAnimation.h
void RActorPhysicsDispose(RActor * ptRActor);  // RActorPhysics.h
void CActorMovementDispose(RActor * ptRActor);  // Movement/CActorMovement.h
void BehaviorDispose(RActor * ptRActor);  // Behavior/Behavior.h
void RActorDetachExistingRActor(const RActorID idRActor, const ACDID idACDParent);  // RActorAttachments.h
BOOL ActorIsEnvActor(const SNO snoActor);  // ActorCommonData.h
namespace UIConversation { void CancelLineForRActor(const RActorID idRActor); }  // UserInterface/Interface/UIConversations.h
TexAnimState * MaterialAllocTexAnimState(const UberMaterial & tUberMaterial, int32 & nTexAnimStateCount);  // TexAnimation.h (TexAnimation.cpp:1071)
void MaterialFreeTexAnimState(TexAnimState *& ptTexAnimState);  // TexAnimation.h
void TexAnimInitialize(const UberMaterial & tUberMaterial, TexAnimState * ptTexAnimState, RActor * ptRActor);  // TexAnimation.h (TexAnimation.cpp:195)
OcclusionHighlighting * SubObjectGfxOcclusionHighlightingAcquire();  // SubObjectGfx.h (SubObjectGfx.cpp:3745)
void SubObjectGfxOcclusionHighlightingDispose(OcclusionHighlighting * ptManager);  // SubObjectGfx.h (SubObjectGfx.cpp:3778)
Attachment * AttachmentGet(const AttachmentID idAttachment);  // Attachments.h (Attachments.cpp:114)
int32 WorldGetDungeonFinderLevel(const SNO snoWorld);  // World.h (World.cpp:828)
SNO WorldGetDungeonFinderLevelArea(const SNO snoWorld);  // World.h (World.cpp:904)
ACDID LocalPlayerACDGet();  // CPlayer.h (CPlayer.cpp:1686)
void RActorHandleOnHighlight(RActor & tRActor, const BOOL bEnableHighlight);  // RActorAttachments.h (RActorAttachments.cpp:194)
void EffectGroupCreateUntracked(const SNO snoEffectGroup, const RActorID idRActorSource, const uint32 dwAttachmentID, const BOOL bIgnoreOwnerAlpha, const BOOL bRemote);  // EffectGroup.h (EffectGroup.cpp:1299)
ShaderTex OverlayTypeToShaderTex(const OverlayType eOverlayType);  // Gfx/GfxMaterial.h (GfxMaterial.cpp:552)
void ParticlesUpdateFixedUpdateFlag();  // Particle.h (Particle.cpp:7136)
void RopesUpdateFixedUpdateFlag();  // Rope.h (Rope.cpp:1998)
void ParentObjectRegister(int nDepth, ParentType eType, DataID idObject, void * pObject, ACDID idACD);  // ParentObjects.h (ParentObjects.cpp:723)
void RActorUpdateClientTransform(RActor * ptRActor, const ActorDefinition * pdefActor, const ActorCommonData * ptACD);
void RActorInternalSetPosition(RActor * ptRActor, const Vector3D & wpPos, BOOL bAddWalkingEffects, SSceneID idSScene);
BOOL RActorIsHidden(const RActor * ptRActor);
int32 ActGetWaypointByLevelArea(const SNO snoLevelArea, BOOL bCheckActivationState);  // Act.h (Act.cpp:156)
// DWARF RActors.cpp:332 (lines 332-335 hold older declarations); linker-dropped concrete DIE, so not inline.
void RActorTilt::Clear()
{
    m_bUsingTilt = FALSE;
    m_bTiltChanging = FALSE;
    m_wvTargetTilt = cgwvZAxis;
    m_aCurrentSpeed = 0.0f;
}
void RActorTilt::Set(const TiltType nTiltType, const BOOL bSnap, const Vector3D & wvNormal, const Angle aYaw)
{



    m_bUsingTilt = TRUE;
    m_bTiltChanging = TRUE;

    switch (nTiltType)
    {
    case TILT_TYPE_SPIDER:
        m_wvTargetTilt = wvNormal;
        break;

    case TILT_TYPE_QUADRUPED:
    {
        Vector3D wvForward(Math::Cos(aYaw),
                           Math::Sin(aYaw),
                           0.0f);

        Vector3D wvRight = Vector3D::CrossProduct(wvForward, cgwvZAxis);
        Vector3D wvSlope = Vector3D::CrossProduct(wvRight, wvNormal);
        m_wvTargetTilt = Vector3D::CrossProduct(wvSlope, wvRight);
        m_wvTargetTilt.Normalize();
        break;
    }


    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    if (bSnap)
    {
        m_wvCurrentTilt = m_wvTargetTilt;
    }
}


BOOL RActorTilt::Update()
{



	if (!m_bTiltChanging)
	{
		return FALSE;
	}


	const Angle aMaxSpeed = 0.05235988f;
	const Angle aMaxAcceleration = 0.05235988f;
	const Angle aMaxDeceleration = 0.05235988f;

	SmoothMovementUnitVector(
		m_aCurrentSpeed,
		m_wvCurrentTilt,
		aMaxSpeed,
		aMaxAcceleration,
		aMaxDeceleration,
		m_wvTargetTilt,
		m_wvCurrentTilt,
		m_aCurrentSpeed);

	float flDot = Vector3D::DotProduct(m_wvCurrentTilt, m_wvTargetTilt);

	if (flDot > 0.999999f)
	{
		m_bTiltChanging = FALSE;
	}

	return TRUE;  // no row of its own: both 410 paths reach the caller's TRUE branch (5238)
}





void RActorTilt::Apply(
    PRSTransform & transform) const
{
	if (m_bUsingTilt)
	{
		Quaternion q;
		q.FromArc(cgwvZAxis, m_wvCurrentTilt);
		transform.q = q * transform.q;
	}
}




void HitJitter::Clear()
{

    m_flMag = 0.0f;
}




inline void HitJitter::Set(const float flMag)
{
    m_flMag = flMag;
}
// Declarations for sRActorCreate's callees (no src header declares them here).
void RActorsCollisionResize(int32 nMax);  // RActorCollision.cpp
WorldPlace ClientPointToPlace(const Vector3D & wp);  // CWorld.cpp
void RActorCollisionInitialize(RActor * ptRActor);  // RActorCollision.cpp
void CActorMovementInitialize(RActor * ptRActor, const WorldPlace & tPlace);  // Movement/CActorMovement.cpp
void RActorHandleRMsg(const DataID idOwner, RMessage & msg);  // RActorAttachments.cpp:556
XListMemoryPool<DataID> * MemoryPoolsGetDATAIDListPool();  // Sigma/MemoryPools.cpp
void RActorAnimInitialize(RActor * ptRActor);  // RActorAnimation.cpp
void RActorCollisionCapsulesAddInitial(RActor * ptRActor);  // RActorCollision.cpp
void BehaviorInit(RActor * ptRActor);  // Behavior/Behavior.cpp
void RActorPhysicsInitialize(RActorID idRActor);  // RActorPhysics.cpp











void HitJitter::Apply(
    PRSTransform & transform) const
{
	if (m_flMag > EPSILON)
	{
		Vector3D wvAxis = transform.q.Rotate(cgwvXAxis);
		transform.wp -= wvAxis * m_flMag;
	}
}


// DWARF RActors.cpp:483: linker-dropped concrete DIE (8 bytes), so not inline.
void RActorSink::Clear()
{
    m_flUnitSink = 0.0f;
    m_flUnitSinkSpeed = 0.0f;
}

static void sUpdateRActorRigidAnimation(RActor * ptRActor);  // RActors.cpp:3646
static void sRActorOverlayReset(RActor & tRActor);  // RActors.cpp:11250
void RActorResetDeathFadeTimes(RActor * ptRActor);  // RActors.cpp:11222
void RActorSink::Start(int32 nTicksToSink)
{

    if (nTicksToSink <= 0)
        m_flUnitSinkSpeed = 1.0f;


    else

        m_flUnitSinkSpeed = 1.0f / (float) nTicksToSink;
}
void CPlayerEnterKnown(int32 nPlayerIndex, ACDID idACD, RActorID idRActor);  // CPlayer.cpp
void CHirelingUpdateAppearance(const ActorCommonData & tACDHireling, RActor * ptRActor, int32 eClass);  // Hireling.cpp:832
void VisualInventoryUpdateSheathed(ACDID idACD);  // CVisualInventory.cpp:832
void RActorGfxPerformLOSCheckInit(RActor & tRActorTarget);  // RActorGfx.cpp:2439
namespace UIConversation { void ReattachConversationLinesForSpeaker(ActorCommonData & tACD); }  // UIConversations.cpp:1420
void RActorPlayAnimation(RActor * ptRActor, uint32 dwAnimTag, BOOL bLoop, BOOL bForce);  // RActorAnimation.cpp
BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor);  // RActorAnimation.cpp:1962
void RActorAnimTreeUpdate(RActor & tRActor, float flDeltaTime);  // RActorAnimation.cpp
void RActorHandleOnCreation(RActor & tRActor, const BOOL fFlippy);  // RActorAttachments.cpp:177
static void sRActorSetLightFilterFlags(RActor * ptRActor);  // RActors.cpp:744
static BOOL sGetSceneVertColor(RActor * ptRActor, RGBAColorValue & rgbaval, BOOL fAssignCurrentAlso);  // RActors.cpp:10976
#include "../Scene.h"  // included again at the original position below (header-guarded); sRActorCreate needs CScene
#include "../../Common/HirelingSwig.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/GameConstants.h"
void RActorsCollisionInitializeForGame(const int32 nMaxRActors);  // RActorCollision.cpp:112
void RActorsAnimInitializeForGame();  // RActorAnimation.cpp:755
void RActorGfxInitializeForGame();  // RActorGfx.cpp:155
static void sRActorInitializeActorEffects();  // RActors.cpp:3824
#include "../Sound/Sound.h"
void RActorSink::Apply(
    RActor & tRActor,
    PRSTransform & transform) const
{
    if (m_flUnitSink > EPSILON)
    {
        const WorldLocation & loc = tRActor.tLocation.tWorldLocation;
        float flHeight = loc.wcylBounds.wdHeight;
        flHeight *= -1.2f;
        flHeight *= m_flUnitSink;
        transform.wp.z += flHeight;
    }
}




void RActorsInitializeForGame()

{
	CGame & tCGame = CGameGet();


	tCGame.ptRActorGlobals = (RActorGlobals *)SigmaMemoryNew(sizeof(RActorGlobals), 4, NULL, TRUE);
	tCGame.ptRActorGlobals->poolRActorSubObjectData.Initialize();
	tCGame.ptRActorGlobals->poolRActorSubObjectData.SetBlockCount_Tracked(MAX_RACTORS);  // 1024: MAX_RACTORS (RActors.h:110) inferred, value only

    tCGame.ptRActorGlobals->poolRActorSubObjectData.SetGrow(XALLOC_GROW_LINEAR, 128);
    tCGame.ptRActorGlobals->poolRActorSubObjectData.SetShrinkable(TRUE);

	tCGame.ptRActorGlobals->poolRigidAnimations.Initialize();
	tCGame.ptRActorGlobals->poolRigidAnimations.SetBlockCount_Tracked(MAX_RACTORS);
    tCGame.ptRActorGlobals->poolCollisionCapsules.Initialize();
    tCGame.ptRActorGlobals->poolCollisionCapsules.SetBlockCount_Tracked(MAX_RACTORS);

	tCGame.ptRActorGlobals->plistMonsterCorpses = new RActorIDListPooled(MemoryPoolsGetDATAIDListPool());
    tCGame.ptRActorGlobals->listMainRActors.SetPool(MemoryPoolsGetDATAIDListPool());

	tCGame.ptRActorGlobals->idLowHealthParticle = PARTICLESYSTEMID_INVALID;
    tCGame.ptRActorGlobals->idLowHealthSound = SOUNDID_INVALID;
    tCGame.ptRActorGlobals->idHunterNearbyParticle = PARTICLESYSTEMID_INVALID;

    tCGame.ptRActorGlobals->flLastDeathTime = 0.0f;
    tCGame.ptRActorGlobals->dwCurrentClientCritterIndex = 0;


	tCGame.ptRActors = TChainedDataArrayCreateTracked<RActor, RActorID>("RActors", AppGetCurrentMemoryInterface(), MAX_RACTORS);

	RActorsCollisionInitializeForGame(tCGame.ptRActors->dwMaximumCount);
	RActorsAnimInitializeForGame();
	RActorGfxInitializeForGame();

	XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.Initialize(256, FALSE, ptMemoryInterface);
    tCGame.ptRActorGlobals->tAppearanceAttributeGlobalSNOMap.Initialize(16, FALSE, ptMemoryInterface);
    tCGame.ptRActorGlobals->tAppearanceAttributeEffectTagMap.Initialize(16, FALSE, ptMemoryInterface);
    tCGame.ptRActorGlobals->tAppearanceAttributeEffectTagMap.SetHashTableSize_Tracked(1024);
    tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.Initialize(16, FALSE, ptMemoryInterface);
    tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.SetHashTableSize_Tracked(1024);
    tCGame.ptRActorGlobals->tAppearanceAttributePowerInitializedMap.Initialize(16, FALSE, ptMemoryInterface);
    tCGame.ptRActorGlobals->tAppearanceAttributePowerInitializedMap.SetHashTableSize_Tracked(1024);
    sRActorInitializeActorEffects();

    new (&tCGame.ptRActorGlobals->queuedFlippies) blz::vector<QueuedFlippyInfo>;
    tCGame.ptRActorGlobals->queuedRActors.Initialize(16, FALSE, NULL);


}


// Includes and declarations needed by RActorPreThreadSafeUpdate and its helpers (DWARF 3686-5246).
#include "../../Common/NPC.h"
#include "../UserInterface/Interface/UIConversations.h"
#include "../Console/GamerProfile.h"
#include "../Console/Online.h"
void PowerAttributeToRuneTypeAndBuffIndex(Attrib eAttrib, RuneType & eType, int32 & nBuffIndex);  // X/Common/Powers/PowersCommon.cpp:6543
namespace UINPC
{
namespace Console
{
BOOL CachedInteractMenuIsActive();  // X/Client/UserInterface/Console/UIConsoleNPC.cpp:727
}
}
BOOL ShouldShowPrompt(const ActorCommonData * ptACDGizmo, const ActorCommonData * ptACDPlayer);  // X/Client/RActors/RActors.cpp:11481
void sAddParticle(RActor * ptRActor, SNO snoParticle, FastAttribKey tKey, HardpointHash & szHardpoint);  // X/Client/RActors/RActors.cpp:4011
BOOL PlayersAreHostileToEachOther(const Player * ptPlayer0, const Player * ptPlayer1);  // X/Common/Player.cpp:5188
BOOL GameCommonPlayerIsPartyGuide(const Player & tPlayer);  // X/Common/GameCommonData.cpp:589
BOOL CPlayerIsEngagedWithTreasureGoblin(const Player * ptPlayer);  // X/Client/CPlayer.cpp:1477
BOOL CPlayerIsEngagedWithRare(const Player * ptPlayer);  // X/Client/CPlayer.cpp:1442
namespace UISeasonStash
{
namespace Console
{
BOOL HasSeasonStashItem(Player * ptPlayer);  // X/Client/UserInterface/Console/UIConsoleSeasonStash.cpp:1249
}
}
void RActorsIntersectSphere(const Sphere & tSphere, const RActorID idIgnore, RActorIDListPooled & listActors);  // X/Client/RActors/RActorCollision.cpp:1041
BOOL RActorAnimationLookAtEnabled(RActor & tRActor);  // X/Client/RActors/RActorAnimation.cpp:1751
RActorID RActorAnimationGetLookAtTarget(const RActor & tRActor);  // X/Client/RActors/RActorAnimation.cpp:1790
void RActorUpdatePropertiesFromParent(RActor * ptRActor, const ActorDefinition * pdefActor, const ActorCommonData * ptACD);  // X/Client/RActors/RActors.cpp:4887
void GameDebugSetCurrentUpdatingActor(ACDID idACD);  // X/Common/GameDebug.cpp:368
static void sRActorInitBannerData(RActor * ptRActor, const ActorCommonData & tACD);  // X/Client/RActors/RActors.cpp:1763
void RActorPhysicsSlowTime(const Sphere & ws, float flSlowExplosionFactor);  // X/Client/RActors/RActorPhysics.cpp:314
void RActorPhysicsUpdate(RActor * ptRActor, const ActorDefinition * pdefActor, const ActorCommonData * ptACD);  // X/Client/RActors/RActorPhysics.cpp:158
void FastAttribGroupApplyDirty(AttribGroupID idAttribGroup);  // X/Common/Attributes/FastAttribute.cpp:3250
BOOL RActorCanHaveChatBubble(const RActor * ptRActor);  // forward declaration: called at 4159, defined at 12266
WorldPlace RActorGetWorldPlace(const RActor * ptRActor);  // forward declaration: called at 4832, defined at 6737
RActorID RActorFindByANN(const ACDNetworkName idANN);  // forward declaration: called at 4800, defined at 6546





static CollisionCapsulePool * sGetCollisionCapsulePool()
{

    return &(CGameGet().ptRActorGlobals->poolCollisionCapsules);
}




void RActorsDeleteAll()
{
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return;




    TDataArrayIterator<RActor> tIt;
    while (RActor * ptRActor = tCGame.ptRActors->Iterate(tIt))
        RActorDelete(ptRActor->id);


    tCGame.ptRActors->DeleteAll();
}











































































































inline int32 RActorLookIndexFromLookLink(const ActorDefinition * pdefActor, uint8 nLookLinkIndex)
{



    int32 nNewLook = LookIndexFromHash(pdefActor->snoAppearance, pdefActor->arActorLooks[nLookLinkIndex].tLookLink.dwLookHash);



    return (nNewLook != -1) ? nNewLook : 0;
}









BOOL sOcclusionHighlightable(const RActor * ptOwnerRActor)
{


    if (GameIsStartUp())
        return FALSE;




    ActorCommonData * ptACD = ACDTryToGet(ptOwnerRActor->idACD);
    if (!ptACD)
        return FALSE;




    if (ACDIsHireling(*ptACD))
        return TRUE;




    if (ptACD->IsItem())
        return TRUE;




    if (ptACD->IsMonster())
        return FALSE;




    if (ptACD->eActorType == ACTORTYPE_PLAYER)
        return PlayerIsLocal(ptOwnerRActor->id);
    GizmoType eGizmoType = ptACD->GetGizmoType();
    switch (eGizmoType)
    {
    case ACTORGIZMO_UNDEFINED:
    case ACTORGIZMO_DOOR:
    case ACTORGIZMO_CHEST:
    case ACTORGIZMO_WAYPOINT:
    case ACTORGIZMO_SIGN:
    case ACTORGIZMO_PORTAL_DESTINATION:
    case ACTORGIZMO_BREAKABLECHEST:
    case ACTORGIZMO_SPAWNER:
    case ACTORGIZMO_DESTROYABLE_OBJECT:
    case ACTORGIZMO_BREAKABLEDOOR:
    case ACTORGIZMO_GATE:
    case ACTORGIZMO_DESTROY_ON_PROXIMITY:
    case ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT:
    case ACTORGIZMO_BANNER:
    case ACTORGIZMO_MAILBOX:
        return FALSE;
    }
    return TRUE;
}


void RActorUpdateOcclusionManager(RActor * ptOwnerRActor)
{
    if (!ptOwnerRActor)
        return;




    SubObjectGfxOcclusionHighlightingDispose(ptOwnerRActor->ptOcclusionHighlighting);
    ptOwnerRActor->ptOcclusionHighlighting = NULL;

    if (sOcclusionHighlightable(ptOwnerRActor))
    {



        ptOwnerRActor->ptOcclusionHighlighting = SubObjectGfxOcclusionHighlightingAcquire();
    }
}




// NON_MATCHING: 4356 vs 4336 (x03: nFadeGroupSubObjectIndex = -1 store added at 1003, now merged as in the original). Early return at 934: original returns via w22 and tail-merges the UnlockR at 932 into the final one; `return idRActor;` reproduces that path but perturbs regalloc (4360); long-lived -1 in w24 rematerialised in ours; rest REGALLOC/scheduling.
// rest CLOSE/REGALLOC per chunkdiff. Inferred: goto at 1080 (both loops exit), fLocalLock idiom, line 1174 spelling (macro?).
static RActorID sRActorCreate(
    const ACDID idACDExisting,
    const SNO snoActor,
    const SNO snoAmbientOcclusionOverrideTex,
    uint32 tName,
    const float flScale,
    const float flLife,
    const PRTransform & transform,
    const uint32 dwRActorAddFlags,
    const RenderContext eContext,
    SSceneID idSScene,
    const uint32 dwSActorCreationTick,
    const ActorDefinition * pdefActor,
    BOOL fWillHaveACD)
{

    CGame & tCGame = CGameGet();
    GameCommonData & tGCD = CGameGetGameCommonData();

    BOOL fLocalLock = (pdefActor == NULL);
    if (fLocalLock)
    {
        pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(snoActor, FALSE);
    }





    BOOL fIsClientCritter = (pdefActor->eType == ACTORTYPE_CRITTER);
    BOOL fSkipActor;

    if (idACDExisting == ACDID_INVALID && fIsClientCritter)
    {
        tCGame.ptRActorGlobals->dwCurrentClientCritterIndex++;
    }





    uint32 dwOldMax = tCGame.ptRActors->dwMaximumCount;

    RActorID idRActor = tCGame.ptRActors->ElementNew();










    if (idRActor == RACTORID_INVALID)
    {
        if (fLocalLock)
        {
            gptGroupDefActor->UnlockR((void *&)pdefActor);
        }
        return RACTORID_INVALID;
    }



    if (tCGame.ptRActors->dwMaximumCount > dwOldMax)
    {
        RActorsCollisionResize(tCGame.ptRActors->dwMaximumCount);
    }

    RActor * ptRActor = RActorGet(idRActor);


    ptRActor->dwLastFrameSubmitted = CGameAsyncGetDrawFrameCount() - 1;


    ptRActor->dwCreationTick = CGameGetClientTicks();
    ptRActor->flDeathTime = 0.0f;
    ptRActor->dwSActorCreationTick = dwSActorCreationTick;
    ptRActor->flTimeLastHit = 0.0f;
    ptRActor->flTimeLastFlinch = 0.0f;

    ptRActor->snoActor = snoActor;
    ptRActor->snoMonster = (pdefActor->eType == ACTORTYPE_MONSTER) ? pdefActor->snoMonster : SNO_NONE;

    ptRActor->eContext = eContext;
    ptRActor->eHairStyle = HAIR_NONE;

    ptRActor->tParentData.Init();

    ptRActor->idACD = ACDID_INVALID;


    ptRActor->ptOcclusionHighlighting = NULL;
    ptRActor->bShowLocatorBeam = FALSE;

    sRActorSetACD(ptRActor, idACDExisting, pdefActor);


    if ((idACDExisting == ACDID_INVALID) && !fWillHaveACD)
    {
        SNOPreload::AddClientActorBySNO(snoActor);
    }

    BOOL fIsNPC = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_NPC_STARTS_AS_NPC);
    if ((ptRActor->snoMonster != SNO_NONE) && !fIsNPC)
    {
        ptRActor->dwFlagsEx |= (1 << RACTOREX_SUMMON_PRELOADED_BIT);
        ScopedMonsterDefinitionLockR pdefMonster(ptRActor->snoMonster);
        for (int32 i = 0; i < MAX_SUMMON_ACTORS; i++)
        {
            SNO _snoActor = pdefMonster->snoSummonActor[i];
            if (_snoActor != SNO_NONE)
            {
                SNOPreload::AddClientActorBySNO(_snoActor);
            }
        }
    }




    ptRActor->tName = tName;

    ptRActor->wpLastUpdate = transform.wp;
    ptRActor->wvVelocity = cgwvNull;
    ptRActor->dwRActorLastSeen = GameGetServerTime();
    ptRActor->idSScene = SSCENEID_INVALID;
    ptRActor->tMarkerOwner.Initialize();
    ptRActor->nFadeGroupSubObjectIndex = -1;  // inferred: original stores -1 at +0x164 (merged into the 1010 store); Particle.cpp:1516 sets it right after tMarkerOwner.Initialize()
    ptRActor->flShear = 0.0f;
    ptRActor->flStretch = 1.0f;
    ptRActor->flBaseScale = 1.0f;
    ptRActor->nItemEffectLevel = -1;
    ptRActor->flFadeAlpha = 1.0f;
    ptRActor->flTriggerFadeAlpha = 1.0f;
    ptRActor->flTriggerFadePerTick = 0.0f;
    ptRActor->tCollisionCapsuleList.SetPool(sGetCollisionCapsulePool());
    ptRActor->flRand = RandFloat_Internal(CGameGlobalsGet().rngLocal);
    ptRActor->tMinimapInfo.Initialize();

    WorldPlace tPlace = ClientPointToPlace(transform.wp);

    ActorLocationInit(ptRActor->tLocation, tPlace, transform.q, flScale);

    ptRActor->wpWalkingPos = transform.wp;
    ptRActor->ptEffectAttachments = NULL;
    ptRActor->ptLookOverrideMap = new LookOverrideMap(16, NULL);

    ptRActor->snoPhysMesh = pdefActor->snoPhysMesh;

    BOOL bIsEnvRActor = RActorIsEnvRActor(*ptRActor);
    if (!bIsEnvRActor ||
        tGCD.GameModeGet() != GAMEMODE_PLAYING)

        RActorCollisionInitialize(ptRActor);
    else
    {

        WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
        loc.ptSphereNode = NULL;
    }

    ptRActor->idMarker = MARKERID_INVALID;

    ptRActor->snoAppearance = pdefActor->snoAppearance;
    ptRActor->snoPhysics = pdefActor->snoPhysics;
    ptRActor->snoIrradianceTex = SNO_NONE;
    ptRActor->snoIrradianceTexHighlight = SNO_NONE;
    ptRActor->snoAmbientOcclusionOverrideTex = snoAmbientOcclusionOverrideTex;

    ScopedAppearanceDefinitionLockR pdefAppearance(pdefActor->snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;

    ptRActor->dwFlags = 0;
    ActorCommonData * ptACDExisting = ACDTryToGet(idACDExisting);
    if (ptACDExisting && ptACDExisting->IsHidden())
    {
        ptRActor->dwFlags |= (1 << RACTOR_HIDE_BIT);
    }

    if (pdefActor->dwFlags & (1 << ACTORDEF_LOAD_ALL_LOOK_TEXTURES_BIT))
    {
        ptRActor->dwFlags |= (1 << RACTOR_SUBOBJECT_TEXTURE_LOAD_ALL);
    }

    if (pdefActor->dwFlags & (1 << ACTORDEF_FADE_WHEN_NEAR_BIT))
    {
        ptRActor->dwFlagsEx |= (1 << RACTOREX_FADE_WHEN_NEAR_BIT);
    }

    sRActorSetLightFilterFlags(ptRActor);


    if (!(dwRActorAddFlags & (1 << RACTOR_ADD_NO_MESH_LIGHT)))
    {
        const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
        const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];
        for (int32 i = 0; i < tGeoSet.nSubObjectCount; i++)
        {
            for (int32 nSubObjectLook = 0; nSubObjectLook < pdefAppearance->nNumLooks; nSubObjectLook++)
            {
                SubObjectAppearance & tSOA = AppearanceGetSubObjectAppearance(pdefAppearance, nSubObjectLook, i, eGeoSetType);
                if (tSOA.dwFlags & (1 << SUBOBJECTAPPEARANCE_MESH_LIT_BIT))
                {
                    ptRActor->dwFlags |= (1 << RACTOR_MESH_LIT_BIT);
                    goto sMeshLitScanDone;
                }
            }
        }
    }

sMeshLitScanDone:
    ptRActor->rgbavalSceneTarget = crgbavalOne;
    sGetSceneVertColor(ptRActor, ptRActor->rgbavalSceneTarget, FALSE);
    ptRActor->rgbavalSceneCurrent = ptRActor->rgbavalSceneTarget;







    ptRActor->aYaw = transform.q.ToYaw();

    if (!(dwRActorAddFlags & (1 << RACTOR_ADD_NO_WALK)))
    {
        GameMode eGameMode = tGCD.GameModeGet();
        if (eGameMode == GAMEMODE_PLAYING ||
            eGameMode == GAMEMODE_HOLODECK ||
            eGameMode == GAMEMODE_MENU)
        {
            CActorMovementInitialize(ptRActor, tPlace);
        }
    }

    if (dwRActorAddFlags & (1 << RACTOR_ADD_FORCE_REMOTE_FX))
    {
        ptRActor->dwFlagsEx |= (1 << RACTOREX_FORCE_REMOTE_FX);
    }

    ptRActor->dwFlags |= (1 << RACTOR_SHADOWS_ENABLED_BIT);

    ptRActor->dwFlags |= (1 << RACTOR_NEED_EFFECT_UPDATE_BIT);

    if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_RIGIDANIM_ANGVEL))
    {
        ptRActor->ptRigidAnim = tCGame.ptRActorGlobals->poolRigidAnimations.Alloc_Tracked();

        RigidAnim & tRigid = *ptRActor->ptRigidAnim;
        SigmaMemoryClear(&tRigid, sizeof(RigidAnim));
        tRigid.wvAxis = tStructure.wvRigidAnimAxis;

        if (!tRigid.wvAxis.IsNormalized())
        {

            tRigid.wvAxis = cgwvZAxis;
        }

        tRigid.aBob = RandAngle(CGameGlobalsGet().rngLocal);
        tRigid.aVel = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_RIGIDANIM_ANGVEL);


        if (tRigid.aVel != 0.0f)
        {
            tRigid.a = RandAngle(CGameGlobalsGet().rngLocal);
        }
        else
        {
            tRigid.a = 0.0f;
        }

        if (TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_RIGIDANIM_RANDOMLY_FLIP_ROTATION))
        {
            if (RandFloat_Internal(CGameGlobalsGet().rngLocal) > 0.5f)
            {
                tRigid.aVel = Math::NormalizeAngle(-tRigid.aVel);
            }
        }


        sUpdateRActorRigidAnimation(ptRActor);
    }
    ptRActor->wvMinBounds = cgwvNull;
    ptRActor->wvMaxBounds = cgwvNull;
    ptRActor->bBoundingBoxCreated = FALSE;

    ptRActor->tTilt.Clear();
    ptRActor->tHitJitter.Clear();
    ptRActor->tSink.Clear();

    ptRActor->dwFlags |= (1 << RACTOR_SELECTABLE_BIT);

    BOOL bRenderOnlyInEditMode = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_RENDER_ONLY_IN_EDIT_MODE);
    if (bRenderOnlyInEditMode &&
        tGCD.GameModeGet() == GAMEMODE_PLAYING)
    {
        ptRActor->dwFlags |= (1 << RACTOR_NEVER_DRAW_BIT);

        BOOL bHideChildrenToo = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_HIDE_CHILDREN_TOO);
        if (!bHideChildrenToo) ptRActor->dwFlags |= (1 << RACTOR_DONT_PASS_ALPHA_TO_CHILDREN_BIT); else ptRActor->dwFlags &= ~(1 << RACTOR_DONT_PASS_ALPHA_TO_CHILDREN_BIT);
    }

    for (int32 nGeoSetType = 0; nGeoSetType < NUM_GEOSETS; nGeoSetType++)
    {
        const GeoSet & tGeoSet = tStructure.tGeoSets[nGeoSetType];
        RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[nGeoSetType];
        if (tGeoSet.nSubObjectCount == 0)
        {

            tGeoSetData.pSubobjectData = NULL; tGeoSetData.ptSubObjectState = NULL;
        }
        else
        {
            if (tGeoSet.nSubObjectCount > MAX_SUBOBJECT_IN_POOLED_HOLDER)
            {
                tGeoSetData.ptSubObjectState = (SubObjectState *)SigmaMemoryNew(tGeoSet.nSubObjectCount * sizeof(SubObjectState), 4, NULL, FALSE);
                tGeoSetData.pSubobjectData = NULL;
            }
            else
            {
                tGeoSetData.pSubobjectData = tCGame.ptRActorGlobals->poolRActorSubObjectData.Alloc_Tracked();
                tGeoSetData.ptSubObjectState = tGeoSetData.pSubobjectData->tSubObjectStateArray;
            }

            SigmaMemoryClear(tGeoSetData.ptSubObjectState, tGeoSet.nSubObjectCount * sizeof(SubObjectState));
        }
    }

    uint8 nLookLinkIndex = ActorChooseLookLink(pdefActor, CGameGlobalsGet().rngLocal);
    ptRActor->nLookNative = RActorLookIndexFromLookLink(pdefActor, nLookLinkIndex);
    ptRActor->nLookActive = ptRActor->nLookNative;

    RActorSetScale(ptRActor, flScale, TRUE, NULL);

    ptRActor->flAlpha = 1.0f;

    ptRActor->tRMsgList.Initialize(ptRActor->id, RActorHandleRMsg);
    ptRActor->tAttachmentList.SetPool(MemoryPoolsGetDATAIDListPool());

    RActorAnimInitialize(ptRActor);

    if (ptRActor->ptActorAnim)
    {
        RActorPlayAnimation(ptRActor, TAG_ACTOR_ANIMATION_NEUTRAL, TRUE, FALSE);
        if (RActorAnimationIsAnimTreeEnabled(*ptRActor))
        {
            RActorAnimTreeUpdate(*ptRActor, OO_TICKS_PER_SECOND);
        }
        RActorUpdateBones(ptRActor, OO_TICKS_PER_SECOND, pdefActor, TRUE);
    }

    RActorCollisionCapsulesAddInitial(ptRActor);



    sRActorSubObjectStatesInitialize(ptRActor);

    ptRActor->ptParticleParams = NULL;
    ptRActor->rgbaColor = crgbaWhite;


    ptRActor->ptBehavior = NULL;
    BehaviorInit(ptRActor);


    ptRActor->idTalkSound = -1;
    ptRActor->nTrackedEffectGroupID = 1000;
    RActorUpdateClientTransform(ptRActor, pdefActor, NULL);



    if (!ptRActor->ptActorAnim)
    {
        RActorPhysicsInitialize(idRActor);
    }



    Player * ptPlayer = PlayerGetByACD(idACDExisting);
    if (ptPlayer)
    {
        CPlayerEnterKnown(ptPlayer->nPlayerIndex, idACDExisting, idRActor);


        if (ptACDExisting && !ptACDExisting->IsAlive())
        {
            ptRActor->dwFlags |= (1 << RACTOR_HIDE_CLIENT_SIDE);
        }

        ptRActor->dwFlagsEx |= (1 << RACTOREX_FORCE_IDLE_ANIMATION_REFRESH_BIT);
    }

    if (ptACDExisting)
    {
        HirelingClass eClass = ACDGetHirelingClass(*ptACDExisting);
        if (eClass != HC_NONE)
        {
            CHirelingUpdateAppearance(*ptACDExisting, ptRActor, eClass);
        }


        UIConversation::ReattachConversationLinesForSpeaker(*ptACDExisting);
    }


    {
        ptRActor->ePickStyle = PICKSTYLE_CYLINDER;
        ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
        if (ptACD)
        {
            GizmoType nGizmoType = ptACD->GetGizmoType();
            if (nGizmoType == ACTORGIZMO_ITEM_INTERNAL)
            {
                ptRActor->ePickStyle = PICKSTYLE_AABB;
            }
        }
        if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_PICK_STYLE))
        {
            ptRActor->ePickStyle = (PickStyle)TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_PICK_STYLE);
        }
    }

    ptRActor->flForceMultiplier = 1.0f;

    ptRActor->flLife = flLife;

    ptRActor->flFloatingNumberHealAccumulator = 0.0f;
    ptRActor->uNextFloatingNumberHealTime = 0;

    RActorResetDeathFadeTimes(ptRActor);

    sRActorOverlayReset(*ptRActor);

    ptRActor->tObjTexAnimParam.flValue = 0.0f;
    ptRActor->dwDebugFlags = 0;
    ptRActor->flLookAtTargetIntensity = 0.0f;
    ptRActor->uLookAtTargetExpireTime = 0;



    ptRActor->posMonsterCorpseList = NULL;
    ptRActor->ptBannerRenderData = NULL;


    ptRActor->fLastLOSCheckResult = TRUE;

    if (bIsEnvRActor)
    {
        CScene * ptCScene = CSceneGetFromSSceneID(idSScene);

        if (ptCScene)
        {
            ptCScene->listSceneRActors.AddTail_Tracked(ptRActor->id);
            ptCScene->dwFlags &= ~(1 << CSCENE_LAST_VISIBLE_BIT);
        }
    }
    else
    {
        tCGame.ptRActorGlobals->listMainRActors.AddTail_Tracked(ptRActor->id);
    }

    RActorHandleOnCreation(*ptRActor, !!(dwRActorAddFlags & (1 << RACTOR_ADD_IS_FLIPPY)));



    ptRActor = RActorGet(idRActor);

    if (ptACDExisting)
    {

        BOOL bUpdateVisualInventory = FALSE;
        if (ptACDExisting->AttributesGetInt(ATTRIBUTE_WEAPONS_HIDDEN))
        {
            ptRActor->dwFlags |= (1 << RACTOR_WEAPONS_HIDDEN_BIT);
            bUpdateVisualInventory = TRUE;
        }

        if (ptACDExisting->AttributesGetInt(ATTRIBUTE_MAIN_HAND_WEAPON_HIDDEN))
        {
            ptRActor->dwFlagsEx |= (1 << RACTOREX_MAIN_HAND_WEAPON_HIDDEN_BIT);
            bUpdateVisualInventory = TRUE;
        }

        if (ptACDExisting->AttributesGetInt(ATTRIBUTE_OFF_HAND_WEAPON_HIDDEN))
        {
            ptRActor->dwFlagsEx |= (1 << RACTOREX_OFF_HAND_WEAPON_HIDDEN_BIT);
        }

        if (bUpdateVisualInventory)
        {
            VisualInventoryUpdateSheathed(ptRActor->idACD);
        }
    }

    RActorGfxPerformLOSCheckInit(*ptRActor);

    if (TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SLOW_TIME_RADIUS) > EPSILON)
    {
        ptRActor->dwFlagsEx |= (1 << RACTOREX_SLOW_TIME_RADIUS_BIT);
    }

    if (fLocalLock)
    {
        gptGroupDefActor->UnlockR((void *&)pdefActor);
    }

    return idRActor;
}





























static BOOL sIsTopmostParentInUI(
    RActor * ptRActor)
{
    RActorID idParent = RActorGetTopmostOwner(ptRActor->id, FALSE, TRUE);
    RActor * ptRActorParent = RActorTryToGet(idParent);
    if (ptRActorParent)
        ptRActor = ptRActorParent;


    return ptRActor->dwFlagsEx & (1 << RACTOREX_FIXED_UPDATE_BIT);
}

























































































ActorTag RActorGetItemEffectTag(const ItemEffectType eItemEffectType)
{

    ActorTag dwEffectTag = 0;

    switch (eItemEffectType)
    {
    case ITEM_EFFECT_TYPE_LIGHTNING:            dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_LIGHTNING_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_COLD:                 dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_ICE_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_FIRE:                 dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_FIRE_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_POISON:               dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_POISON_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_ARCANE:               dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_ARCANE_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_WITCHDOCTOR_DAMAGE:   dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_WITCHDOCTOR_DAMAGE_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_LIFESTEAL:            dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_LIFESTEAL_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_MANASTEAL:            dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_MANASTEAL_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_MAGIC_FIND:           dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_MAGIC_FIND_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_GOLD_FIND:            dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_GOLD_FIND_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_ATTACK_SPEED_BONUS:   dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_ATTACK_SPEED_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_CAST_SPEED_BONUS:     dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_CAST_SPEED_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_HOLY:                 dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_HOLY_EFFECTGROUP; break;
    case ITEM_EFFECT_TYPE_WIZARD_DAMAGE:        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_WIZARD_DAMAGE_EFFECTGROUP; break;
    }
    return dwEffectTag;
}




































void RActorAddLookOverride(RActor * ptRActor, uint32 nLookHash)
{


    if (!ptRActor)
        return;


    int32 nLookOverrideCount;
    // bump the reference count for this look
    if (ptRActor->ptLookOverrideMap->Lookup(nLookHash, nLookOverrideCount))
    {
        ptRActor->ptLookOverrideMap->SetAt_Tracked(nLookHash, nLookOverrideCount + 1);
    }
    else
    {
        ptRActor->ptLookOverrideMap->SetAt_Tracked(nLookHash, 1);
    }
}




void RActorRemoveLookOverride(RActor * ptRActor, uint32 nLookHash)
{


    if (!ptRActor)
        return;



    int32 nLookOverrideCount = 0;
    if (ptRActor->ptLookOverrideMap->Lookup(nLookHash, nLookOverrideCount))
    {
        nLookOverrideCount--;
        if (nLookOverrideCount <= 0)
        {
            ptRActor->ptLookOverrideMap->RemoveAt(nLookHash);
        }
        else
        {
            ptRActor->ptLookOverrideMap->SetAt_Tracked(nLookHash, nLookOverrideCount);
        }
    }
}





int32 RActorGetLookOverrideLookIndex(RActor * ptRActor)
{

    if (!ptRActor)
        return -1;



    if (ptRActor->ptLookOverrideMap->IsEmpty())
        return -1;


    int32 nLowestIndex = 0x7fffffff;

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    POS pos = ptRActor->ptLookOverrideMap->GetStartAssoc();
    while (pos)
    {
        uint32 nLookHash; int32 nValue;

        ptRActor->ptLookOverrideMap->GetNextAssoc(pos, nLookHash, nValue);

        int32 nLookIndex = LookIndexFromHash(pdefAppearance, nLookHash);

        if (nLookIndex < nLowestIndex)
            nLowestIndex = nLookIndex;
    }




    return nLowestIndex;
}




void RActorAddWeaponElementalEffects(const RActor * ptRActor)
{

    if (ptRActor->nItemEffectLevel != -1)
    {
        ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

        ActorTag dwTag = RActorGetItemEffectTag(ptRActor->eItemEffectType); if (dwTag)
        {
            SNO snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, dwTag);

            if (snoEffectGroup != SNO_NONE)

                EffectGroupCreateUntracked(snoEffectGroup, ptRActor->id, 0, FALSE, FALSE);
        }
    }
}






































































































































































































































































































































































































RActorID RActorCreate(const SNO snoActor, const float flScale, const float flLife, const PRTransform & transform, const uint32 dwRActorAddFlags, const RenderContext eContext, SSceneID idSScene)







{
    CGame & tCGame = CGameGet();

    CRefString sNameToUse;
    tCGame.ptRActorGlobals->dwRActorNumberCount++;
    sNameToUse.Format("%s-%d", SNOToString(ACTOR_SNO, snoActor, FALSE).str(), tCGame.ptRActorGlobals->dwRActorNumberCount);

    uint32 tNameToUse = MarkerStringHash(sNameToUse.str());

    float flScaleToUse = flScale;
    if (!(dwRActorAddFlags & (1 << RACTOR_ADD_OVERRIDE_SCALE)))
    {




        ScopedActorDefinitionLockR pdefActor(snoActor);
        float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
        float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);
        float flScaleFromActor = RandRangeFloat_Internal(CGameGlobalsGet().rngLocal, flMinScale, flDeltaScale);

        flScaleToUse *= flScaleFromActor;
    }



    flScaleToUse = (flScaleToUse > MIN_PRS_SCALE) ? flScaleToUse : MIN_PRS_SCALE;

    return sRActorCreate(ACDID_INVALID, snoActor, SNO_NONE, tNameToUse, flScaleToUse, flLife, transform, dwRActorAddFlags, eContext, idSScene, 0, NULL, FALSE);
}














void RActorDelete(RActorID idRActor)
{





    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;




    SubObjectGfxOcclusionHighlightingDispose(ptRActor->ptOcclusionHighlighting);
    ptRActor->ptOcclusionHighlighting = NULL;


    CGame & tCGame = CGameGet();










    if (ptRActor->ptParticleParams)
    {
        ParticleSystemLostRActor(ptRActor);
    }


    BehaviorDispose(ptRActor);

    RActorCollisionDispose(ptRActor);

    if (ptRActor->idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(ptRActor->idACD);
        if (tACD.HasAttachedACD())
        {
            ACDID idACDAttachment = ACDFromANN((ACDNetworkName)tACD.GetAttachedACD());

            if (idACDAttachment != ACDID_INVALID)
            {
                RActorDetachExistingRActor((RActorID)ACDGetOwner(idACDAttachment),
                                           ptRActor->idACD);
            }
        }
    }

    AttachmentListHandleParentDeletion(ptRActor->tAttachmentList);

    UIConversation::CancelLineForRActor(ptRActor->id);

    RActorAnimDispose(ptRActor);
    RActorPhysicsDispose(ptRActor);

    RActorCollisionCapsulesDispose(ptRActor);
    ptRActor->tRMsgList.Dispose();

    sRActorSubObjectStatesDispose(ptRActor);




    for (int32 nGeoSetType = 0; nGeoSetType < NUM_GEOSETS; nGeoSetType++)
    {
        RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[nGeoSetType];
        if (!tGeoSetData.pSubobjectData)
            { SigmaMemoryFree(tGeoSetData.ptSubObjectState, NULL); tGeoSetData.ptSubObjectState = NULL; }
        else
            tCGame.ptRActorGlobals->poolRActorSubObjectData.Free(tGeoSetData.pSubobjectData);
    }


    if (ptRActor->ptRigidAnim)
    {
        tCGame.ptRActorGlobals->poolRigidAnimations.Free(ptRActor->ptRigidAnim);
    }

    CActorMovementDispose(ptRActor);

    if (ptRActor->idACD == ACDID_INVALID)
    {
        SNOPreload::RemoveClientActorBySNO(ptRActor->snoActor);
    }

    if (ptRActor->dwFlagsEx & (1 << RACTOREX_SUMMON_PRELOADED_BIT))
    {
        ScopedMonsterDefinitionLockR pdefMonster(ptRActor->snoMonster);


        for (int32 i = 0; i < MAX_SUMMON_ACTORS; i++)
            if (pdefMonster->snoSummonActor[i] != SNO_NONE)
            {
                SNOPreload::RemoveClientActorBySNO(pdefMonster->snoSummonActor[i]);
            }
    }


    sRActorSetACD(ptRActor, ACDID_INVALID, NULL);

    if (ptRActor->posMonsterCorpseList)
    {
        tCGame.ptRActorGlobals->plistMonsterCorpses->RemoveAt(ptRActor->posMonsterCorpseList);
    }

    if (!RActorIsEnvRActor(*ptRActor))
    {
        POS pos = tCGame.ptRActorGlobals->listMainRActors.Find(idRActor);
        if (pos)
        {
            tCGame.ptRActorGlobals->listMainRActors.RemoveAt(pos);
        }
    }

    delete ptRActor->ptBannerRenderData; ptRActor->ptBannerRenderData = NULL;
    delete ptRActor->ptEffectAttachments; ptRActor->ptEffectAttachments = NULL;
    delete ptRActor->ptLookOverrideMap; ptRActor->ptLookOverrideMap = NULL;





    tCGame.ptRActors->ElementDelete(ptRActor);
}



void RActorAttachSpawnedActor(RActor * ptRActor)
{
    if (ptRActor->snoActor == SNO_NONE)
        return;



    if (ptRActor->idMarker == MARKERID_INVALID)
        return;
    {
        float flSummonedActorScale; SNO snoSummonedActor; Attachment * ptExisting;
        ScopedActorDefinitionLockR ptActorDef(ptRActor->snoActor);
        if (TagMapGetInt(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_TYPE) == ACTORGIZMO_SPAWNER)
        {


            SNO snoActor = RActorGetInheritedTagMapValueSNO(ptRActor, TAG_ACTOR_SPAWN_ACTOR_SNO, TAG_MARKER_SPAWN_ACTOR_SNO);
            GBID gbidItem = RActorGetInheritedTagMapValueGBID(ptRActor, TAG_ACTOR_SPAWN_ITEM, TAG_MARKER_SPAWN_ITEM);
            SNO snoEncounter = RActorGetInheritedTagMapValueSNO(ptRActor, TAG_ACTOR_SPAWN_ENCOUNTER, TAG_MARKER_SPAWN_ENCOUNTER);

            snoSummonedActor = snoActor;
            if (snoSummonedActor == SNO_NONE)
            {

                if (gbidItem != GBID_NONE)
                {

                    snoSummonedActor = GameBalanceGetActorSNO(GBHandle(GB_ITEMS, gbidItem));
                }
                else if (snoEncounter != SNO_NONE)
                {
                    const EncounterDefinition * ptEncounter = (const EncounterDefinition *)gptGroupDefEncounter->LockRFast(snoEncounter);
                    snoSummonedActor = ptEncounter->snoSymbol;
                }
            }






























































            ptExisting = AttachmentFind(ptRActor->tAttachmentList, INTERNAL_ATTACHMENTID_EDIT_MODE_SPAWNER_ATTACHMENT);
            if (ptExisting)
            {

                if (ptExisting->snoname.snoHandle == snoSummonedActor)
                    return;

                AttachmentDelete(*ptExisting);
            }

            if (snoSummonedActor != SNO_NONE)
            {
                flSummonedActorScale = 1.0f;

                TriggerEvent tTriggerEvent;
                TriggerEventInit(tTriggerEvent);
                tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
                tTriggerEvent.dwID = INTERNAL_ATTACHMENTID_EDIT_MODE_SPAWNER_ATTACHMENT;
                tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_ACTOR;
                tTriggerEvent.snoname.Set(ACTOR_SNO, snoSummonedActor);
                tTriggerEvent.tHardpointLinks[0].tHash = DEFAULT_HARDPOINT_NAME;
                tTriggerEvent.flScale = flSummonedActorScale;

                AttachmentID idAttach = RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
            }
        }
    }
}








































static void sPreloadAllSubObjectLook(
    RActor * ptRActor,
    const AppearanceDefinition * pdefAppearance)
{



    if (ptRActor->dwFlags & (1 << RACTOR_SUBOBJECT_LOOK_UPDATE_LOCKED))
        return;



    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];

    SNONameArray tTextures;
    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];

        BOOL fRendered = tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_RENDER_BIT);
        BOOL fShouldLoadTexture = (ptRActor->dwFlags & (1 << RACTOR_SUBOBJECT_TEXTURE_LOAD_ALL)) || fRendered;
        tSubObjectState.nSubObjectLoadedLook = tSubObjectState.nSubObjectLook;

        if (!fShouldLoadTexture)
            continue;



        const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLoadedLook];

        if (fRendered)
        {
            if (tSOA.snoCloth != SNO_NONE && tSubObjectState.idCloth == CLOTHID_INVALID)
            {
                tSubObjectState.idCloth = ClothManager::Create(ptRActor, nSubObjectIndex, tSOA.snoCloth);
            }

            if (!tSubObjectState.arTexAnimStates)
            {
                tSubObjectState.arTexAnimStates = MaterialAllocTexAnimState(tSOA.tCompositeUberMaterial, tSubObjectState.nTexAnimStateCount);
                TexAnimInitialize(tSOA.tCompositeUberMaterial, tSubObjectState.arTexAnimStates, ptRActor);
            }
        }



        if (!(tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT)))
        {
            int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
            MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
            for (int32 i = 0; i < nCount; ++i)
            {
                SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
                if (snoTex != SNO_NONE)
                {
                    tTextures.push_back(SNOName(TEXTURE_SNO, snoTex));
                }
            }

            tSubObjectState.dwFlags |= 1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT;
        }
    }



    if (tTextures.size())
    {
        SNOPreload::AddClientTextures(tTextures);
    }

}





















































































































































static void sUnloadAllSubObjectLook(
    RActor * ptRActor,
    const AppearanceDefinition * pdefAppearance)
{



    if (ptRActor->dwFlags & (1 << RACTOR_SUBOBJECT_LOOK_UPDATE_LOCKED))
        return;



    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];

    SNONameArray tTextures;
    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];

        if (tSubObjectState.idCloth != CLOTHID_INVALID)
        {
            ClothManager::Destroy(tSubObjectState.idCloth);
            tSubObjectState.idCloth = CLOTHID_INVALID;
        }

        if (tSubObjectState.arTexAnimStates)
        {
            MaterialFreeTexAnimState(tSubObjectState.arTexAnimStates);
        }

        if (tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT))
        {
            const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLoadedLook];
            int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
            MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
            for (int32 i = 0; i < nCount; ++i)
            {
                SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
                if (snoTex != SNO_NONE)
                {
                    tTextures.push_back(SNOName(TEXTURE_SNO, snoTex));
                }
            }

            tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT);
        }
    }


    if (tTextures.size())
    {
        SNOPreload::RemoveClientTextures(tTextures);
    }

}




void LockAllSubObjectLookUpdate(RActor * ptRActor)
{

    ptRActor->dwFlags |= (1 << RACTOR_SUBOBJECT_LOOK_UPDATE_LOCKED);
}




void UnlockAllSubObjectLookUpdate(RActor * ptRActor, const AppearanceDefinition * pdefAppearance)
{


    ptRActor->dwFlags &= ~(1 << RACTOR_SUBOBJECT_LOOK_UPDATE_LOCKED);
    sUpdateAllSubObjectLook(ptRActor, pdefAppearance);
}






















































































































































































































































































































































































void RActorChangeLookIndex(RActor & tRActor, const int32 nNewLook, const BOOL bOverridenLook)



{
    if (!bOverridenLook)
    {
        tRActor.nLookNative = nNewLook;


        if (RActorGetLookOverrideLookIndex(&tRActor) >= 0)
            return;
    }



    if (tRActor.nLookActive != nNewLook)
    {



        sRActorSubObjectStatesDispose(&tRActor);

        tRActor.nLookActive = nNewLook;

        sRActorSubObjectStatesInitialize(&tRActor);
    }
}



static void sRActorCancelLook(RActor & tRActor, int32 nLookToCancel)


{






    ActorCommonData * ptACD = ACDTryToGet(tRActor.idACD);
    if (ptACD)
    {
        ScopedActorDefinitionLockR pdefActor(tRActor.snoActor);
        int32 nOrigLook = RActorLookIndexFromLookLink(pdefActor, ptACD->nLookLinkIndex);

        if (tRActor.nLookNative == nLookToCancel)
        {
            tRActor.nLookNative = nOrigLook;
        }

        if (RActorGetLookOverrideLookIndex(&tRActor) < 0)
        {



            if (tRActor.nLookNative != tRActor.nLookActive)
            {



                sRActorSubObjectStatesDispose(&tRActor);

                tRActor.nLookActive = tRActor.nLookNative;

                sRActorSubObjectStatesInitialize(&tRActor);
            }
        }
    }
}


void RActorChangeLook(const RActorID idRActor, LPCSTR szLookName)


{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {



        int32 nNewLook = LookIndexFromName(ptRActor->snoAppearance, szLookName);
        if (nNewLook != -1)
        {

            RActorChangeLookIndex(*ptRActor, nNewLook, FALSE);


            RActorSettleCloth(ptRActor);
        }
    }
}


void RActorChangeLook(const RActorID idRActor, int32 nHashKey)


{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {



        int32 nNewLook = LookIndexFromHash(ptRActor->snoAppearance, nHashKey);
        if (nNewLook != -1)
        {

            RActorChangeLookIndex(*ptRActor, nNewLook, FALSE);


            RActorSettleCloth(ptRActor);
        }
    }
}
// Linker-dropped in the original (DWARF concrete DIE _Z16RActorCancelLook8RActorIDPKc at low_pc 0, 0xb0 bytes,
// lines 3214-3231 with full line table); its call keeps sRActorCancelLook out of line.
void RActorCancelLook(
    const RActorID idRActor,
    LPCSTR szLookName)
{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;



    int32 nNewLook = LookIndexFromName(ptRActor->snoAppearance, szLookName);
    if (nNewLook == -1)
        return;



    sRActorCancelLook(*ptRActor, nNewLook);
}




void RActorCancelLook(const RActorID idRActor, int32 nHashKey)


{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;



    int32 nNewLook = LookIndexFromHash(ptRActor->snoAppearance, nHashKey);
    if (nNewLook == -1)
        return;



    sRActorCancelLook(*ptRActor, nNewLook);
}



void RActorRevertToNativeLook(RActor & tRActor)

{
    if (tRActor.nLookNative >= 0)
    {




        if (tRActor.nLookActive != tRActor.nLookNative)
        {



            sRActorSubObjectStatesDispose(&tRActor);

            tRActor.nLookActive = tRActor.nLookNative;

            sRActorSubObjectStatesInitialize(&tRActor);
        }
    }
}



BOOL RActorHardpointCreateCentralFacing(
    Hardpoint & tNewHardpoint,
    RActorID idRActor,
    const Vector3D & wpLookatPointWorld,
    const BOOL bMustCreate)

{
	const RActor * ptRActor = RActorGet(idRActor);
	if (!ptRActor)
		return FALSE;  // no row: line-0 tail at +0x58c (mov w19, wzr; b epilogue, skips the unlock)




	Vector3D wpRActorCenterWorld;  // DWARF local (decl line 3296), no location and no code: unused






    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
	Vector3D wpCylinderBase = pdefActor->wcylBounds.wpBase;
	WorldDistance wdCylinderHeight = pdefActor->wcylBounds.wdHeight;
    WorldDistance wdCylinderRadius = pdefActor->wcylBounds.wdRadius;

	WorldDistance wdCenterAreaSq;
	WorldDistance wdHalfHeight = wdCylinderHeight * 0.5f;
	if (wdCylinderRadius >= wdHalfHeight)  // fcmp radius, half; fcsel lt -> radius^2 (wdHalfHeight <= wdCylinderRadius gives fcmp half, radius)
	{
		wdCenterAreaSq = wdHalfHeight * wdHalfHeight;
	}
	else
	{
		wdCenterAreaSq = wdCylinderRadius * wdCylinderRadius;
	}


	Vector3D wpActorCenterObject = Vector3D(0.f, 0.f, wpCylinderBase.z + wdHalfHeight);  // x,y are 0 (DistanceSquared at 3376 squares the neutral-pose x,y unchanged); spelling unknown: '+' is at column 69 in the original


	PRSTransform transformRActorInv = RActorGetInvClientTransform(ptRActor);
	Vector3D wpLookatPointObject = transformRActorInv * wpLookatPointWorld;


	Vector3D wpPointOnActorObject;
	Vector3D wpPointOnActorNeutralPose;
	BOOL bPointFound = FALSE;  // no row: the flag is folded into control flow (found -> +0x594, not found -> +0x584)
	int32 nBoneIndex = -1;
	{
		Vector3D wpPointOnActorWorld;
		CGameGlobals &tCGameGlobals = CGameGlobalsGet();
		Vector3D wvNormal;
		Emitter tEmitter;
		tEmitter.eType = EMIT_ACTOR_SUBOBJECT_SHELL;
		tEmitter.id = idRActor;
		tEmitter.dwSubObjectIndex = (uint32)-1;  // no row: merged into 3337's stp w22, w23 (w23 = -1); idiom from Particle.cpp:2559

		int32 nNumTries = 10;
		while (nNumTries-- > 0)  // 3341:22 is the '>'; 10 iterations (w26 = 10..1)
		{


			EmitterGetRandomPoint(tCGameGlobals.rngLocal, cgwpNull, tEmitter, wpPointOnActorWorld, wvNormal, nBoneIndex);







			wpPointOnActorObject = transformRActorInv * wpPointOnActorWorld;
			// no row: asm +0x398 copies the object-space point (v0-v2) to v8-v10 before the bone test
			wpPointOnActorNeutralPose = wpPointOnActorObject;
			if (nBoneIndex != -1)
			{


				ProcessedBone &tBone = ptRActor->ptActorAnim->ptProcessedBoneList[nBoneIndex];
				PRSTransform transformToNeutralPose = tBone.transformDelta.Inverse();
				wpPointOnActorNeutralPose = transformToNeutralPose * wpPointOnActorObject;
			}











			WorldDistance wdDistanceFromCenterSq =
				Vector3D::DistanceSquared(wpActorCenterObject, wpPointOnActorNeutralPose);


			if (wdDistanceFromCenterSq <= wdCenterAreaSq ||
				((bMustCreate == TRUE) && (nNumTries == 1)))  // cmp w26, #2 with w26 = nNumTries + 1 after the postfix decrement
			{
				// no row: bPointFound only feeds the shared FALSE tail at +0x584
				bPointFound = TRUE;
				break;
			}
		}
	}


	// no row: the shared tail at +0x584 (mov w19, wzr; b unlock) is this return
	if (!bPointFound)
	{
		return FALSE;
	}


	tNewHardpoint.nBoneIndex = nBoneIndex;



	Vector3D wvForward = wpLookatPointObject - wpPointOnActorObject;  // asm +0x59c subtracts s0-s2 (object-space point; the bone path never writes them), Set at 3415 uses v8-v10
	wvForward.Normalize();
	Vector3D wvUp = cgwvZAxis;
	Vector3D wvRight = Vector3D::CrossProduct(wvForward, wvUp);
	wvRight.Normalize();
	wvUp = Vector3D::CrossProduct(wvRight, wvForward);
	wvUp.Normalize();



	Quaternion qOrientation;
	qOrientation.FromChangeOfBasis(cgwvXAxis, cgwvYAxis, cgwvZAxis, wvForward, -wvRight, wvUp);


	tNewHardpoint.transform.Set(qOrientation, wpPointOnActorNeutralPose);

	return TRUE;  // no row: orr w19, wzr, #1 at +0x76c (return value constant 1 on this path)
}




static void sRActorSubObjectStatesInitialize(RActor * ptRActor)
{

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    for (int32 nGeoSetType = 0; nGeoSetType < NUM_GEOSETS; nGeoSetType++)
    {
        const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[nGeoSetType];
        RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[nGeoSetType];

        float flLookSwitchTime = CGameGetClientTime();
        for (int32 i = 0; i < tGeoSet.nSubObjectCount; i++)
        {
            SubObjectAppearance & tAppearance = AppearanceGetSubObjectAppearance(pdefAppearance, ptRActor->nLookActive, i, (GeoSetType)nGeoSetType);
            SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[i];

            tSubObjectState.dwFlags = 0;
            BOOL fCanBeRendered = (tAppearance.dwFlags & (1 << 0)) != 0;  // SUBOBJECTAPPEARANCE_RENDER_BIT
            tSubObjectState.dwFlags |= (fCanBeRendered << 0);  // SUBOBJECTSTATE_CAN_BE_RENDERED_BIT

            tSubObjectState.nSubObjectLook = ptRActor->nLookActive;
            tSubObjectState.flLookSwitchTime = flLookSwitchTime;

            SubObjectStateInitFade(tSubObjectState);


            if (tAppearance.tCompositeUberMaterial.snoShaderMap != SNO_NONE)
            {
                ScopedShaderMapDefinitionLockR pdefShaderMap(tAppearance.tCompositeUberMaterial.snoShaderMap);
                tSubObjectState.snoDefaultShader = ShaderMapGetDefaultShader(tAppearance.tCompositeUberMaterial.snoShaderMap, TRUE, pdefShaderMap);
                tSubObjectState.snoFadeShader = ShaderMapGetFadeShader(tAppearance.tCompositeUberMaterial.snoShaderMap, pdefShaderMap);
            }


            tSubObjectState.idCloth = CLOTHID_INVALID;
        }

        for (int32 i = 0; i < tGeoSet.nSubObjectCount; i++)
        {
            SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[i];


            BOOL fRender = (tSubObjectState.dwFlags & (1 << 0)) != 0;  // SUBOBJECTSTATE_CAN_BE_RENDERED_BIT




            if (fRender) tSubObjectState.dwFlags |= (1 << 1); else tSubObjectState.dwFlags &= ~(1 << 1);  // SUBOBJECTSTATE_RENDER_BIT
        }
    }

    sPreloadAllSubObjectLook(ptRActor, pdefAppearance);
}




static void sRActorSubObjectStatesDispose(RActor * ptRActor)
{














    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    sUnloadAllSubObjectLook(ptRActor, pdefAppearance);
}
// LINKER-REMOVED: DWARF DIE 0x01ea4383 (RActors.cpp:3504, external, low_pc 0, size 0x118) records
// tCGame 3507, tIt 3514, ptRActor 3517, pdefAppearance 3519 (ScopedDefinitionLockR<AppearanceDefinition>), inlined
// CGameGet, TChainedDataArray<RActor>::Iterate (3515) and the lock ctor/dtor (3519/3522). Rows: 3509 col 17/9 (null test
// as in RActorsDeleteAll), 3520 and 3521 three instructions each at col 9. Inferred: the two calls at 3520/3521 (their DIEs follow this one).
void RActorsResetForAppearanceChange()

{
    CGame & tCGame = CGameGet();

    if (!tCGame.ptRActors)
        return;



    TDataArrayIterator<RActor> tIt;
    while (tCGame.ptRActors->Iterate(tIt))
    {
        RActor * ptRActor = tIt.ptElement;

        ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
        sUnloadAllSubObjectLook(ptRActor, pdefAppearance);
        sPreloadAllSubObjectLook(ptRActor, pdefAppearance);
    }
}




























































































void RActorSetOrientation(RActor * ptRActor, const Quaternion & qOrientation, const ActorDefinition * pdefActor, const ActorCommonData * ptACD)
{





    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
    loc.transform.q = qOrientation;



    RActorUpdateClientTransform(ptRActor, pdefActor, ptACD);
}
















static void sUpdateRActorRigidAnimation(
    RActor * ptRActor)
{
    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    const Angle aBobVel = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_RIGIDANIM_BOB_ANGVEL);
    const WorldDistance wdBobMag = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_RIGIDANIM_BOB_MAG);
    const WorldDistance wdBobOffset = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_RIGIDANIM_BOB_OFFSET);


    RigidAnim & tRigid = *ptRActor->ptRigidAnim;

    tRigid.aBob = Math::NormalizeAngle(aBobVel + tRigid.aBob);
    tRigid.wdDist = wdBobOffset + wdBobMag * Math::Sin(tRigid.aBob);
    tRigid.a = Math::NormalizeAngle(tRigid.a + tRigid.aVel);

    RActorUpdateClientTransform(ptRActor, pdefActor, NULL);
}



static void sCopyHardpointName(const SNO snoAppearance, HardpointHash szName, HardpointLink & tLink)
{



    ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;

    for (int32 nIndex = 0; nIndex < tStructure.nHardpointCount; nIndex++)
    {
        if (tStructure.ptHardpoints[nIndex].tHash == szName)
        {
            tLink.tHash = tStructure.ptHardpoints[nIndex].tHash;
            break;
        }
    }
}



static SNO sGetNPCConversationIconSNO(
    const ConversationIcon eConvIcon)
{
	SNO sno = SNO_NONE;
	switch (eConvIcon)
	{
	case 1:  // ENUMERATOR: ConversationIcon has no enumerators in the CU DWARF; values from the switch table at 0x7100e430d0
		sno = GlobalSNOGet(GLOBALSNO_PARTICLE_QUESTINFO_EXCLAMATION);
		break;

	case 2:  // ENUMERATOR
		sno = GlobalSNOGet(GLOBALSNO_PARTICLE_QUESTINFO_QUESTION);
		break;

	case 3:  // ENUMERATOR
		sno = GlobalSNOGet(GLOBALSNO_PARTICLE_QUESTINFO_ASTERISK);
		break;

	case 4:  // ENUMERATOR
		sno = GlobalSNOGet(GLOBALSNO_PARTICLE_QUESTINFO_GOSSIP);
		break;

	case 5:  // ENUMERATOR
		sno = GlobalSNOGet(GLOBALSNO_PARTICLE_QUESTINFO_EXCLAMATION_BONUS);
		break;

	default:
		break;
	}

	return sno;
}

struct ActorAppearanceAttributeParticles
{
    Attrib eTurnOnAttribute;
    int32 nTurnOnParameter;
    GlobalSNO eGlobalSNO;
    uint32 dwEffectTag;
};

static const ActorAppearanceAttributeParticles cgarAppearanceAttribParticles[] =
{
#include "../../Common/generated-game-data/actor_particles.inc"
#line 3780
};  // values from the original .rodata at 0x7100e42d58; 0xFFFFFFFF is DWORD_NONE (Sigma.h:331 in DWARF, missing in src)



static const int32 NUM_APPEAR_ATTRIB_PARTICLES = 51;  // DWARF RActors.cpp:3784 const_value 51

static void sInitializeBuffEffectsForPower(
    const PowerDefinition * pdefPower,
    const SNO snoPower,
    const uint32 dwEffectGroupTag,
    const Attrib eRuneAttribNone,
    const Attrib eRuneAttribA,
    const Attrib eRuneAttribB,
    const Attrib eRuneAttribC,
    const Attrib eRuneAttribD,
    const Attrib eRuneAttribE)
{
    CGame & tCGame = CGameGet();
    SNO snoBuffEffectGroup = PowerGeneralTagGetSNO(pdefPower, dwEffectGroupTag);

    if (snoBuffEffectGroup != SNO_NONE)
    {
        FastAttribKey tKey = MakeAttribKey(eRuneAttribNone, snoPower);
        FastAttribKey tKeyA = MakeAttribKey(eRuneAttribA, snoPower);
        FastAttribKey tKeyB = MakeAttribKey(eRuneAttribB, snoPower);
        FastAttribKey tKeyC = MakeAttribKey(eRuneAttribC, snoPower);
        FastAttribKey tKeyD = MakeAttribKey(eRuneAttribD, snoPower);
        FastAttribKey tKeyE = MakeAttribKey(eRuneAttribE, snoPower);

        tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.SetAt_Tracked(tKey, dwEffectGroupTag);
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKey, AAEFFECT_TYPE_POWER_TAG);
        tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.SetAt_Tracked(tKeyA, dwEffectGroupTag);
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKeyA, AAEFFECT_TYPE_POWER_TAG);
        tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.SetAt_Tracked(tKeyB, dwEffectGroupTag);
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKeyB, AAEFFECT_TYPE_POWER_TAG);
        tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.SetAt_Tracked(tKeyC, dwEffectGroupTag);
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKeyC, AAEFFECT_TYPE_POWER_TAG);
        tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.SetAt_Tracked(tKeyD, dwEffectGroupTag);
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKeyD, AAEFFECT_TYPE_POWER_TAG);
        tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.SetAt_Tracked(tKeyE, dwEffectGroupTag);
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKeyE, AAEFFECT_TYPE_POWER_TAG);
    }
}

static void sRActorInitializeActorEffects()
{

    CGame & tCGame = CGameGet();

    Stopwatch tStopwatch;

    for (int32 nEntryIndex = 0; nEntryIndex < NUM_APPEAR_ATTRIB_PARTICLES; nEntryIndex++)
    {
        const ActorAppearanceAttributeParticles & tEntry = cgarAppearanceAttribParticles[nEntryIndex];
        FastAttribKey tKey = MakeAttribKey(tEntry.eTurnOnAttribute, tEntry.nTurnOnParameter);

        if (tEntry.eGlobalSNO != GLOBALSNO_INVALID)
        {
            tCGame.ptRActorGlobals->tAppearanceAttributeGlobalSNOMap.SetAt_Tracked(tKey, tEntry.eGlobalSNO);
            tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKey, AAEFFECT_TYPE_GLOBAL_SNO);
        }
        else if (tEntry.dwEffectTag != 0xFFFFFFFF)  // DWORD_NONE (Sigma.h:331 in DWARF, missing in src)
        {
            tCGame.ptRActorGlobals->tAppearanceAttributeEffectTagMap.SetAt_Tracked(tKey, tEntry.dwEffectTag);
            tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.SetAt_Tracked(tKey, AAEFFECT_TYPE_TAG);
        }
    }






}
static void sUpdateActorEffectsForAttributes(
    RActor * ptRActor,
    ActorCommonData & tACD,
    const ActorDefinition * ptActorDef)
{
    CGame & tCGame = CGameGet();
    POS pos = tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.GetStartAssoc();
    while (pos)
    {
        FastAttribKey tKey;
        AppearanceAttributeEffectType eType;
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.GetNextAssoc(pos, tKey, eType);

        AttachmentID idAttachmentExisting;

        BOOL bFoundExisting = FALSE;
        if (ptRActor->ptEffectAttachments)
        {
            bFoundExisting = ptRActor->ptEffectAttachments->Lookup(tKey, idAttachmentExisting);
        }

        int32 nAttribValue = tACD.AttributesGetInt(tKey);

        ActorType eActorType = tACD.GetActorType();

        // IsAlive is called before the nAttribValue test (3880:20 before the 3880:14 compare), so the || group comes first
        if ((tACD.IsAlive() ||
                KeyGetAttrib(tKey) == ATTRIBUTE_DISPLAYS_TEAM_EFFECT ||
                eActorType == ACTORTYPE_SERVER_PROP ||
                eActorType == ACTORTYPE_CUSTOM_BRAIN) && nAttribValue > 0)
        {

            if (!bFoundExisting)
            {
                SNO snoEffect = SNO_NONE;
                BOOL bUseRuneType = FALSE;
                RuneType eRuneType = RUNE_TYPE_INVALID;

                if (eType == AAEFFECT_TYPE_GLOBAL_SNO)
                {
                    GlobalSNO eGlobalSNO;
                    if (tCGame.ptRActorGlobals->tAppearanceAttributeGlobalSNOMap.Lookup(tKey, eGlobalSNO))
                    {
                        snoEffect = GlobalSNOGet(eGlobalSNO);
                    }
                }
                else if (eType == AAEFFECT_TYPE_TAG)
                {
                    uint32 dwEffectTag;
                    if (tCGame.ptRActorGlobals->tAppearanceAttributeEffectTagMap.Lookup(tKey, dwEffectTag))
                    {
                        snoEffect = TagMapGetSNO(ptActorDef->hTagMap, dwEffectTag);
                    }
                }
                else if (eType == AAEFFECT_TYPE_POWER_TAG)
                {
                    uint32 dwEffectTag;
                    if (tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.Lookup(tKey, dwEffectTag))
                    {
                        SNO snoPower = (SNO)KeyGetParam(tKey);
                        snoEffect = PowerGeneralTagGetSNO(snoPower, dwEffectTag);
                    }
                    int32 nBuffIndex = 0;
                    PowerAttributeToRuneTypeAndBuffIndex(KeyGetAttrib(tKey), eRuneType, nBuffIndex);
                    bUseRuneType = TRUE;
                }

                TriggerEvent tAttachParticleSystem;
                TriggerEventInit(tAttachParticleSystem);
                tAttachParticleSystem.eType = TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP;
                tAttachParticleSystem.snoname.Set(EFFECTGROUP_SNO, snoEffect);
                tAttachParticleSystem.dwID = INTERNAL_ATTACHMENTID_ATTRIB_PARTICLES + tKey.nValue;
                if (bUseRuneType)
                {
                    tAttachParticleSystem.eRuneType = eRuneType;
                    tAttachParticleSystem.bUseRuneType = TRUE;
                }

                AttachmentID idAttachment = RActorSendRMessage(*ptRActor, tAttachParticleSystem, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
                if (ptRActor->ptEffectAttachments == NULL)
                {
                    ptRActor->ptEffectAttachments = new AppearanceAttributeAttachmentIDMap(16, NULL);
                }

                ptRActor->ptEffectAttachments->SetAt_Tracked(tKey, idAttachment);
            }
        }
        else
        {
            if (bFoundExisting)
            {
                sRActorRemoveEffectParticle(*ptRActor, tKey);
            }
        }
    }
}



void RActorResetActorEffectsForAttributes(RActor * ptRActor)

{
    CGame & tCGame = CGameGet();

    POS pos = tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.GetStartAssoc();
    while (pos)
    {
        FastAttribKey tKey;
        AppearanceAttributeEffectType eType;
        tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.GetNextAssoc(pos, tKey, eType);



        AttachmentID idAttachmentExisting;
        BOOL bFoundExisting = ptRActor->ptEffectAttachments && ptRActor->ptEffectAttachments->Lookup(tKey, idAttachmentExisting);


        if (bFoundExisting)
        {

            sRActorRemoveEffectParticle(*ptRActor, tKey);
        }
    }

    ptRActor->dwFlags |= (1 << RACTOR_NEED_EFFECT_UPDATE_BIT);
}



void RActorRemoveEffectParticle(RActor & tRActor, FastAttribKey tKey)
{



    sRActorRemoveEffectParticle(tRActor, tKey);
}


static void sRActorRemoveEffectParticle(RActor & tRActor, FastAttribKey tKey)
{
    AttachmentID idAttachment;

    if (tRActor.ptEffectAttachments && tRActor.ptEffectAttachments->Lookup(tKey, idAttachment))
    {
        TriggerEvent tOutroParticleSystem;

        TriggerEventInit(tOutroParticleSystem);
        tOutroParticleSystem.eType = TRIGGEREDEVENTTYPE_OUTRO_OBJECT;
        tOutroParticleSystem.dwID = tKey.nValue + 1101;
        RActorSendRMessage(tRActor, tOutroParticleSystem, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
        tRActor.ptEffectAttachments->RemoveAt(tKey);
    }

}



void sAddParticle(RActor * ptRActor, SNO snoParticle, FastAttribKey tKey, HardpointHash & szHardpoint)




{
    ScopedActorDefinitionLockR ptActorDef(ptRActor->snoActor);

    TriggerEvent tTriggerEvent;
    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_PARTICLESYSTEM;
    tTriggerEvent.snoname.Set(PARTICLE_SNO, snoParticle);
    tTriggerEvent.dwID = tKey.nValue + INTERNAL_ATTACHMENTID_ATTRIB_PARTICLES;
    sCopyHardpointName(ptActorDef->snoAppearance, szHardpoint, tTriggerEvent.tHardpointLinks[0]);

    AttachmentID idAttachment = RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
    if (!ptRActor->ptEffectAttachments)
    {
        ptRActor->ptEffectAttachments = new AppearanceAttributeAttachmentIDMap(16, NULL);
    }

    ptRActor->ptEffectAttachments->SetAt_Tracked(tKey, idAttachment);
}

void sAddParticle(
    RActor * ptRActor,
    SNO snoParticle,
    FastAttribKey tKey)
{
    sAddParticle(ptRActor, snoParticle, tKey, ACTOR_DEFAULT_EFFECT_QUESTINFO_HARDPOINT);
}




void sAddEffectGroup(RActor * ptRActor, SNO snoEffectGroup, FastAttribKey tKey)
{


    TriggerEvent tAttachParticleSystem;

    TriggerEventInit(tAttachParticleSystem);
    tAttachParticleSystem.eType = TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP;
    tAttachParticleSystem.snoname.Set(EFFECTGROUP_SNO, snoEffectGroup);
    tAttachParticleSystem.dwID = tKey.nValue + INTERNAL_ATTACHMENTID_ATTRIB_PARTICLES;

    AttachmentID idAttachment = RActorSendRMessage(*ptRActor, tAttachParticleSystem, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
    if (!ptRActor->ptEffectAttachments)
    {
        ptRActor->ptEffectAttachments = new AppearanceAttributeAttachmentIDMap(16, NULL);
    }

    ptRActor->ptEffectAttachments->SetAt_Tracked(tKey, idAttachment);
}




static void sUpdateActorEffectsForIcons(
    RActor * ptRActor,
    ActorCommonData & tACD)
{
    const CGame & tClientData = CGameGet();
    if (!tClientData.bLastClientUpdate)
    {
        return;
    }

    Player* ptLocalPlayer = LocalPlayerGet();
    if (!ptLocalPlayer)
    {
        return;
    }

    FastAttribKey tTalkBubbleKey = MakeAttribKey(ATTRIBUTE_IN_CONVERSATION, 0);
    FastAttribKey tConvIconKey = MakeAttribKey(ATTRIBUTE_CONVERSATION_ICON, ptLocalPlayer->nPlayerIndex);
    ConversationIcon eConvIcon = (ConversationIcon)tACD.AttributesGetInt(ATTRIBUTE_CONVERSATION_ICON_OVERRIDE);
    if (eConvIcon == -1)  // ENUMERATOR: ConversationIcon 'none' value is not in the CU DWARF (cmn w25, #1)
    {
        eConvIcon = (ConversationIcon)tACD.AttributesGetInt(tConvIconKey);
    }

    AttachmentID idAttachmentExisting;


    if (!UINPC::Console::CachedInteractMenuIsActive())
    {
        for (int32 i = 0; i < MAX_LOCAL_PLAYERS; i++)
        {
            if (UIGame::GetRActorAlternateHighlight(i) == ptRActor->id)
            {
                Player* ptPlayer = PlayerGetByControllerIndex(i);
                ACDID idACDPlayer = ptPlayer ? ptPlayer->idPlayerACD : ACDID_INVALID;
                const ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);

                if (ptACDPlayer && ShouldShowPrompt(&tACD, ptACDPlayer))
                {
                    // no row: the ShouldShowPrompt cbnz goes straight to the 4156 remove, so the icon value set here fails the 4118 test
                    eConvIcon = 0;  // ENUMERATOR: a ConversationIcon value <= 0
                    break;
                }
            }
        }
    }


    if (eConvIcon > 0)
    {
        sRActorRemoveEffectParticle(*ptRActor, tTalkBubbleKey);
        SNO snoParticle = sGetNPCConversationIconSNO(eConvIcon);
        BOOL bFoundExisting = FALSE;
        if (ptRActor->ptEffectAttachments)
        {
            bFoundExisting = ptRActor->ptEffectAttachments->Lookup(tConvIconKey, idAttachmentExisting);
        }

        BOOL bAddNew = TRUE;
        if (bFoundExisting)
        {
            Attachment* ptAttachment = AttachmentTryToGet(idAttachmentExisting);
            if (ptAttachment)
            {
                if (ptAttachment->snoname.snoHandle == snoParticle)
                    bAddNew = FALSE;
                else
                    sRActorRemoveEffectParticle(*ptRActor, tConvIconKey);
            }
        }







        if (bAddNew)
        {
            sAddParticle(ptRActor, snoParticle, tConvIconKey);
        }
    }
    else
    {


        sRActorRemoveEffectParticle(*ptRActor, tConvIconKey);
    }

    if (UIConversation::RActorHasTalkBubble(ptRActor->id) && RActorCanHaveChatBubble(ptRActor))
    {
        if (!ptRActor->ptEffectAttachments || !ptRActor->ptEffectAttachments->Lookup(tTalkBubbleKey, idAttachmentExisting))
        {
            sAddParticle(ptRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_TALKING), tTalkBubbleKey);
        }
    }
    else
    {

        sRActorRemoveEffectParticle(*ptRActor, tTalkBubbleKey);
    }
}




static void sUpdateActorBannerRing(
    RActor * ptRActor,
    ActorCommonData & tACD)
{
    FastAttribKey tBannerRingKey = MakeAttribKey(ATTRIBUTE_BANNER_USABLE, 0);

    int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACD.snoActor);
    Player* ptPlayer = PlayerGetByPlayerIndex(nPlayerDest);
    Player* ptLocalPlayer = LocalPlayerGetPrimary();

    // the !ptPlayer test is a line-0 cbz after 4184 (x25 = ptPlayer)
    if (!ptPlayer ||
        PlayerIsLocal(ptPlayer) ||
        !tACD.AttributesGetInt(ATTRIBUTE_BANNER_USABLE) ||
        PlayersAreHostileToEachOther(ptPlayer, ptLocalPlayer))
    {
        if (ptRActor->dwFlags & (1 << RACTOR_BANNER_RING_ACTIVE))
        {
            sRActorRemoveEffectParticle(*ptRActor, tBannerRingKey);
            ptRActor->dwFlags &= ~(1 << RACTOR_BANNER_RING_ACTIVE);
        }
        return;
    }




    BOOL bPartyGuide = GameCommonPlayerIsPartyGuide(*ptPlayer);
    if ((ptRActor->dwFlags & (1 << RACTOR_BANNER_RING_ACTIVE)) &&
        (BOOL)((ptRActor->dwFlags >> RACTOR_BANNER_PLAYER_IS_PARTY_GUIDE) & 1) == bPartyGuide)
    {
        return;
    }


    sRActorRemoveEffectParticle(*ptRActor, tBannerRingKey);


    SNO snoBannerRingEffectGroup;
    if (bPartyGuide)
    {
        snoBannerRingEffectGroup = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_BANNER_RING_PARTY_GUIDE);
    }
    else
    {
        snoBannerRingEffectGroup = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_BANNER_RING_OTHER_PLAYER);
    }


    sAddEffectGroup(ptRActor, snoBannerRingEffectGroup, tBannerRingKey);
    ptRActor->dwFlags |= (1 << RACTOR_BANNER_RING_ACTIVE);
    if (bPartyGuide) ptRActor->dwFlags |= (1 << RACTOR_BANNER_PLAYER_IS_PARTY_GUIDE); else ptRActor->dwFlags &= ~(1 << RACTOR_BANNER_PLAYER_IS_PARTY_GUIDE);
}




static void sUpdateActorBannerIcons(
    RActor * ptRActor,
    ActorCommonData & tACD)
{
    FastAttribKey tBannerIconKey = MakeAttribKey(ATTRIBUTE_BANNER_PLAYER_IN_COMBAT, 0);

    int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACD.snoActor);
    Player* ptPlayer = PlayerGetByPlayerIndex(nPlayerDest);

    if (!ptPlayer)
    {
        // tail-merged remove+return: 4245 shares the original's code with the other icon helpers
        sRActorRemoveEffectParticle(*ptRActor, tBannerIconKey);
        return;
    }


    SNO snoBannerIconParticle = SNO_NONE;
    if (CPlayerIsEngagedWithTreasureGoblin(ptPlayer))
    {
        snoBannerIconParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_BANNER_ICON_ENGAGED_WITH_GOBLIN);
    }
    else if (CPlayerIsEngagedWithRare(ptPlayer))
    {
        snoBannerIconParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_BANNER_ICON_ENGAGED_WITH_RARE);
    }
    else if (tACD.AttributesGetInt(ATTRIBUTE_BANNER_PLAYER_IN_COMBAT))
    {
        snoBannerIconParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_BANNER_ICON_IN_COMBAT);
    }


    AttachmentID idAttachmentExistingIcon;
    if (ptRActor->ptEffectAttachments && ptRActor->ptEffectAttachments->Lookup(tBannerIconKey, idAttachmentExistingIcon))
    {
        Attachment* ptAttachment = AttachmentTryToGet(idAttachmentExistingIcon);
        if (ptAttachment)
        {
            if (ptAttachment->snoname.snoHandle != snoBannerIconParticle)
            {
                sRActorRemoveEffectParticle(*ptRActor, tBannerIconKey);
            }
            else
            {
                return;
            }
        }
    }



    if (snoBannerIconParticle != SNO_NONE)
    {
        sAddParticle(ptRActor, snoBannerIconParticle, tBannerIconKey);
    }
}




static void sUpdateSharedStash(
    RActor * ptRActor,
    ActorCommonData & tACD)
{



    Player* ptPlayer = LocalPlayerGet();

    if(!ptPlayer)
        return;



    int32 nStashIndex = CPlayerGetStashLookIndex(*ptPlayer);

    if(ptRActor->nLookActive != nStashIndex)
    {
        RActorChangeLookIndex(*ptRActor, nStashIndex, FALSE);
    }
}




static void sUpdateHQHotColdIcon(
    RActor * ptRActor,
    FastAttribKey tHotColdIconKey,
    int32 nNewValue)
{

    SNO snoHotColdIconParticle;
    switch (nNewValue)
    {
    case 0:
        snoHotColdIconParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_HQ_HOT_COLD_COLD);
        break;

    case 1:
        snoHotColdIconParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_HQ_HOT_COLD_HOT);
        break;

    case 2:
        snoHotColdIconParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_HQ_HOT_COLD_SUPERHOT);
        break;

    default:
        sRActorRemoveEffectParticle(*ptRActor, tHotColdIconKey);
        return;
    }


    AttachmentID idAttachmentExistingIcon;
    if (ptRActor->ptEffectAttachments && ptRActor->ptEffectAttachments->Lookup(tHotColdIconKey, idAttachmentExistingIcon))
    {
        Attachment * ptAttachment = AttachmentTryToGet(idAttachmentExistingIcon);
        if (ptAttachment)
        {
            if (ptAttachment->snoname.snoHandle == snoHotColdIconParticle)
                return;
            sRActorRemoveEffectParticle(*ptRActor, tHotColdIconKey);
        }
    }








    if (snoHotColdIconParticle != SNO_NONE)
    {
        sAddParticle(ptRActor, snoHotColdIconParticle, tHotColdIconKey, ACTOR_DEFAULT_EFFECT_HARDPOINT);
    }
}




static void sUpdateActorMailIcons(
    RActor * ptRActor,
    ActorCommonData & tACD)
{
    if(CGameLoadingScreenIsEnabled())
    {
        return;
    }

    Player* ptPrimaryPlayer = PlayerGetByControllerIndex(Console::GamerProfile::GetPrimaryProfileUserIndex());

    if(!ptPrimaryPlayer)
    {
        return;
    }

    FastAttribKey tMailFlagKey = MakeAttribKey(ATTRIBUTE_MAIL_FLAG_ICON, ptPrimaryPlayer->nPlayerIndex);

    AttachmentID idAttachmentExisting = ATTACHMENTID_INVALID;
    BOOL bFoundExisting = ptRActor->ptEffectAttachments && ptRActor->ptEffectAttachments->Lookup(tMailFlagKey, idAttachmentExisting);

    BOOL bHaveMail = FALSE;
    BOOL bHaveSeasonalRolloverMail = FALSE;

    for(Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if(PlayerIsLocal(ptPlayer) && Console::GamerProfile::IsActive(ptPlayer->nControllerIndex))
        {
            bHaveMail = (bHaveMail || (!GameIsSeasonal() && Console::Online::MailHasMail(ptPlayer->nControllerIndex, Console::Online::MAIL_TYPE_MESSAGE)));
            bHaveSeasonalRolloverMail = (bHaveSeasonalRolloverMail || (!GameIsSeasonal() && UISeasonStash::Console::HasSeasonStashItem(ptPlayer)));
        }
    }

    GlobalSNO snoNeeded = GLOBALSNO_INVALID;

    if(bHaveMail)
    {



        snoNeeded = bHaveSeasonalRolloverMail ? GLOBALSNO_PARTICLE_MAILBOX_MAIL_AND_SEASONAL : GLOBALSNO_PARTICLE_MAILBOX_MAIL;
    }
    else
    {


        if(bHaveSeasonalRolloverMail)
        {
            snoNeeded = GLOBALSNO_PARTICLE_MAILBOX_SEASONAL;
        }
    }



    if(!bFoundExisting && snoNeeded != GLOBALSNO_INVALID)
    {
        SNO snoParticle = GlobalSNOGet(snoNeeded);
        sAddParticle(ptRActor, snoParticle, tMailFlagKey);
    }
    else if(bFoundExisting && snoNeeded != GLOBALSNO_INVALID)
    {
        SNO snoParticle = GlobalSNOGet(snoNeeded);
        Attachment* ptAttachment = AttachmentTryToGet(idAttachmentExisting);
        if(ptAttachment && ptAttachment->snoname.snoHandle != snoParticle)
        {
            sRActorRemoveEffectParticle(*ptRActor, tMailFlagKey);
            sAddParticle(ptRActor, snoParticle, tMailFlagKey);
        }
    }
    else if(bFoundExisting && snoNeeded == GLOBALSNO_INVALID)
    {
        sRActorRemoveEffectParticle(*ptRActor, tMailFlagKey);
    }
}

static void sUpdateWeeklyChallengeIcons(
    RActor * ptRActor,
    ActorCommonData & tACD)
{
    if (CGameLoadingScreenIsEnabled())
    {
        return;
    }

    Player* ptPrimaryPlayer = PlayerGetByControllerIndex(Console::GamerProfile::GetPrimaryProfileUserIndex());

    if (!ptPrimaryPlayer)
    {
        return;
    }

    FastAttribKey tWeeklyChallengeRewardFlagKey = MakeAttribKey(ATTRIBUTE_WEEKLY_CHALLENGE_REWARD_ICON, ptPrimaryPlayer->nPlayerIndex);

    AttachmentID idAttachmentExisting;
    BOOL bFoundExisting = ptRActor->ptEffectAttachments && ptRActor->ptEffectAttachments->Lookup(tWeeklyChallengeRewardFlagKey, idAttachmentExisting);

    BOOL bHaveChallengeReward = FALSE;

    for (Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer && !bHaveChallengeReward; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer) && Console::GamerProfile::IsActive(ptPlayer->nControllerIndex))
        {
            D3::Account::SavedDefinition& tAccountDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);

            uint64 uNow = Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp());

            for (int32 n = 0; n < tAccountDefinition.console_data().challenge_rift_reward_size(); n++)
            {
                D3::Account::ConsoleChallengeRiftReward tReward = tAccountDefinition.console_data().challenge_rift_reward(n);

                if (GameIsSeasonal())
                {
                    if (tReward.season_earned() != OnlineService::GetSeasonNum())
                    {
                        continue;
                    }
                }

                bHaveChallengeReward = TRUE;
                break;
            }
        }
    }

    GlobalSNO snoNeeded = GLOBALSNO_INVALID;

    if (bHaveChallengeReward)
    {
        snoNeeded = GLOBALSNO_PARTICLE_CHALLENGE_RIFT_REWARD_ICON;
    }


    if (!bFoundExisting && snoNeeded != GLOBALSNO_INVALID)
    {
        SNO snoParticle = GlobalSNOGet(snoNeeded);
        sAddParticle(ptRActor, snoParticle, tWeeklyChallengeRewardFlagKey);
    }
    else if (bFoundExisting && snoNeeded == GLOBALSNO_INVALID)
    {
        sRActorRemoveEffectParticle(*ptRActor, tWeeklyChallengeRewardFlagKey);
    }
}






static void sActorRemoveIcons(RActor * ptRActor)
{

    FastAttribKey tKeyInConversation = MakeAttribKey(ATTRIBUTE_IN_CONVERSATION, 0);
    FastAttribKey tKeyConversationIcon = MakeAttribKey(ATTRIBUTE_CONVERSATION_ICON, LocalPlayerGet()->nPlayerIndex);
    FastAttribKey tKeyBannerUsable = MakeAttribKey(ATTRIBUTE_BANNER_USABLE, 0);
    FastAttribKey tKeyBannerInCombat = MakeAttribKey(ATTRIBUTE_BANNER_PLAYER_IN_COMBAT, 0);

    sRActorRemoveEffectParticle(*ptRActor, tKeyConversationIcon);
    sRActorRemoveEffectParticle(*ptRActor, tKeyInConversation);
    sRActorRemoveEffectParticle(*ptRActor, tKeyBannerUsable);
    sRActorRemoveEffectParticle(*ptRActor, tKeyBannerInCombat);
}
















































































static void sUpdateActorTint(RActor * ptRActor, ActorCommonData & tACD)
{


    if (tACD.IsItem() || !tACD.IsAlive())
        return;



    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, 27)))
        RActorApplyOverlay(ptRActor->id, 7);



    else if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FROZEN)))

        RActorApplyOverlay(ptRActor->id, 12);



    else if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_CHILLED)))

        RActorApplyOverlay(ptRActor->id, 11);



    else if (tACD.IsPoisoned())

        RActorApplyOverlay(ptRActor->id, 13);





}







static void sUpdateActorScale(
    RActor * ptRActor,
    ActorCommonData & tACD)
{


    const float RACTOR_SCALE_PERCENT_PER_TICK = 0.1f;

    float flScaleBonus = tACD.AttributesGetFloat(ATTRIBUTE_SCALE_BONUS);
    float flScaleTarget = ptRActor->flBaseScale + flScaleBonus;
    float flScaleCurrent = ptRActor->tLocation.tWorldLocation.flScale;
    float flScaleDiff = flScaleTarget - flScaleCurrent;
    if (Math::Abs(flScaleDiff) > RACTOR_SCALE_EPSILON)
    {
        if (tACD.AttributesGetInt(ATTRIBUTE_SCALE_BONUS_IS_IMMEDIATE))
        {
            flScaleCurrent = flScaleTarget;  // no row: the cbnz at 4670 lands on the code that already holds flScaleTarget
        }
        else
        {
            flScaleCurrent += flScaleDiff * RACTOR_SCALE_PERCENT_PER_TICK;
            if (Math::Abs(flScaleDiff) <= RACTOR_SCALE_EPSILON)
            {
                // no row: the b.ls at 4677 lands on the code that already holds flScaleTarget
                flScaleCurrent = flScaleTarget;
            }
        }
        RActorSetScale(ptRActor, flScaleCurrent, FALSE, NULL);
    }
}




static void sUpdateActorAppearanceTS(RActor * ptRActor, ActorCommonData * ptACD)
{


    if (ptRActor->dwFlags & (1 << RACTOR_MESH_LIT_BIT))
    {

        RGBAColorValue & cur = ptRActor->rgbavalSceneCurrent; const RGBAColorValue & tgt = ptRActor->rgbavalSceneTarget;  // RGBAColorValue +,-,* not in src/ yet
        RGBAColorValue d; d.r = tgt.r - cur.r; d.g = tgt.g - cur.g; d.b = tgt.b - cur.b; d.a = tgt.a - cur.a; d.r *= 0.1f; d.g *= 0.1f; d.b *= 0.1f; d.a *= 0.1f;
        cur.r = cur.r + d.r; cur.g = cur.g + d.g; cur.b = cur.b + d.b; cur.a = cur.a + d.a;
    }
    if (ptACD)
    {
        sUpdateActorTint(ptRActor, *ptACD);
    }
}



static void sSmoothDOT(
    ActorCommonData & tACD,
    Attrib eAttribDPS,
    Attrib eAttribCur,
    Attrib eAttribMax,
    Attrib eAttribCap,
    BOOL bNegate)
{
    float flDOTDps = tACD.AttributesGetFloat(eAttribDPS);
    if (Math::Abs(flDOTDps) > EPSILON)
    {
        if (bNegate)
        {
            flDOTDps = -flDOTDps;
        }
        float flDamageThisTick = flDOTDps * 0.016666668f;  // literal from the pool (1/60): the spelling of the constant is an inference

        FastAttribKey tKeyCurr;
        FastAttribKey tKeyMax;
        if (KeyGetParamType(eAttribDPS) == APT_RESOURCE)
        {
            // the resource type comes from the ACD for resource-parameterised DOTs (MakeAttribKey rows at 4731/4737)
            tKeyCurr = MakeAttribKey(eAttribCur, tACD.GetPrimaryResourceType());
            tKeyMax = MakeAttribKey(eAttribMax, tACD.GetPrimaryResourceType());
        }
        else
        {
            tKeyCurr = MakeAttribKey(eAttribCur, -1);  // RESOURCE_TYPE_INVALID: not in the owner CU's DWARF, literal (w23 = -1)
            tKeyMax = MakeAttribKey(eAttribMax, -1);  // RESOURCE_TYPE_INVALID
        }

        float flCurr = tACD.AttributesGetFloat(tKeyCurr);

        flCurr -= flDamageThisTick;
        flCurr = Math::Max(flCurr, 0.01f);
        if (eAttribMax != ATTRIBUTE_NONE)
        {
            float flMax = tACD.AttributesGetFloat(tKeyMax);
            flCurr = Math::Min(flCurr, flMax);

            if (eAttribCap != ATTRIBUTE_NONE)
            {
                float flCap = tACD.AttributesGetFloat(eAttribCap) * flMax;
                flCurr = Math::Max(flCurr, flCap);
            }
        }

        FastAttribSetValueFloat(tACD.idFastAttrib, tKeyCurr, flCurr);
    }
}



static void sRActorSmoothDOTUpdate(
    ActorCommonData & tACD)
{





    if (tACD.IsAlive())
    {
        if (tACD.AttributesGetInt(ATTRIBUTE_DAMAGE_SHIELD) == 0 &&
            !(tACD.AttributesGetInt(ATTRIBUTE_INVULNERABLE) || tACD.AttributesGetInt(ATTRIBUTE_NO_DAMAGE) ||
                tACD.AttributesGetInt(ATTRIBUTE_GOD)))
        {

            sSmoothDOT(tACD, ATTRIBUTE_DOT_DPS, ATTRIBUTE_HITPOINTS_CUR, ATTRIBUTE_HITPOINTS_MAX_TOTAL, ATTRIBUTE_DAMAGE_CAP_PERCENT, FALSE);
            sSmoothDOT(tACD, ATTRIBUTE_RESOURCE_REGEN_TOTAL, ATTRIBUTE_RESOURCE_CUR, ATTRIBUTE_RESOURCE_MAX_TOTAL, ATTRIBUTE_NONE, TRUE);
            sSmoothDOT(tACD, ATTRIBUTE_ROOT_AUTO_DECAY_PER_SECOND, ATTRIBUTE_ROOT_UNIT_VALUE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, FALSE);
        }

        sSmoothDOT(tACD, ATTRIBUTE_SUFFOCATION_PER_SECOND, ATTRIBUTE_SUFFOCATION_UNIT_VALUE, ATTRIBUTE_NONE, ATTRIBUTE_NONE, TRUE);
    }
}

const uint32 scgLookAtTimeMask = 15;


static void sRActorTrySetLookAtTarget(
    RActor & tRActor,
    ActorCommonData & tACD,
    RActorID idRActorTarget,
    float flIntensity)
{
    uint32 uNow = CGameGetClientTicks();



    ACDNetworkName idANNTarget = (ACDNetworkName)tACD.AttributesGetInt(ATTRIBUTE_LOOK_TARGET_SERVER_ANN);
    RActorID idRActorServerTarget = RActorFindByANN(idANNTarget);
    if (idRActorServerTarget == RACTORID_INVALID)
    {
        RActorID idCurrTarget = RActorAnimationGetLookAtTarget(tRActor);
        if (idCurrTarget == idRActorTarget ||
            uNow > tRActor.uLookAtTargetExpireTime ||
            flIntensity > tRActor.flLookAtTargetIntensity)
        {
            RActorAnimationLookAt(tRActor.id, idRActorTarget);

            tRActor.flLookAtTargetIntensity = flIntensity;
            tRActor.uLookAtTargetExpireTime = uNow + scgLookAtTimeMask + 1;
        }
    }
}



static void sRActorBroadcastLookAt(
    RActor * ptRActor,
    ActorCommonData & tACD)
{
    uint32 uNow = CGameGetClientTicks();

    if ((uNow & scgLookAtTimeMask) == 0)
    {
        float flMaxIntensity = tACD.AttributesGetFloat(ATTRIBUTE_LOOK_TARGET_BROADCAST_INTENSITY);
        float flRadius = tACD.AttributesGetFloat(ATTRIBUTE_LOOK_TARGET_BROADCAST_RADIUS);
        if (flMaxIntensity > EPSILON && flRadius > EPSILON)
        {
            float flOORadius = 1.0f / flRadius;
            WorldPlace tPlace = tACD.GetPlace();
            WorldPlace placeRActor = RActorGetWorldPlace(ptRActor);

            Sphere tSphere;
            tSphere.Set(tPlace.wp, flRadius);

            RActorIDListPooled listRActors(MemoryPoolsGetDATAIDListPool());
            RActorsIntersectSphere(tSphere, ptRActor->id, listRActors);

            POS pos = listRActors.GetHeadPosition();
            while (pos)
            {
                RActorID idRActorCurr = (RActorID)listRActors.GetNext(pos);
                RActor * ptRActorCurr = RActorTryToGet(idRActorCurr);
                if (ptRActorCurr && RActorAnimationLookAtEnabled(*ptRActorCurr))
                {
                    WorldPlace placeCurr = RActorGetWorldPlace(ptRActorCurr);
                    Vector3D vDiff = placeCurr.wp - placeRActor.wp;
                    float flUnitIntensity = vDiff.Magnitude() * flOORadius;
                    flUnitIntensity = Math::Min(flUnitIntensity, 1.0f);
                    flUnitIntensity = 1.0f - flUnitIntensity;
                    float flIntensity = flMaxIntensity * flUnitIntensity;

                    sRActorTrySetLookAtTarget(*ptRActorCurr, tACD, ptRActor->id, flIntensity);
                }
            }
        }
    }


    ACDNetworkName idANNTarget = (ACDNetworkName)tACD.AttributesGetInt(ATTRIBUTE_LOOK_TARGET_SERVER_ANN);
    RActorID idRActorServerTarget = RActorFindByANN(idANNTarget);
    RActorID idRActorCurrentTarget = RActorAnimationGetLookAtTarget(*ptRActor);
    if (idRActorCurrentTarget != RACTORID_INVALID)
    {
        if (idRActorServerTarget == RACTORID_INVALID)
        {
            if (RActorAnimationLookAtEnabled(*ptRActor) &&
                uNow > ptRActor->uLookAtTargetExpireTime)
            {
                RActorAnimationLookAt(ptRActor->id, RACTORID_INVALID);
            }
        }
    }
    else
    {
        if (idRActorServerTarget != RACTORID_INVALID)
        {
            RActorAnimationLookAt(ptRActor->id, idRActorServerTarget);
        }
    }
}





































































































void SoundFadeOut(const SoundID idSound);  // X/Client/Sound/Sound.cpp:1511


void RActorsUpdateFullscreenParticleSystems()

{
	ACDID idACDLocalPlayer = LocalPlayerACDGet();
	ActorCommonData * ptACD = ACDTryToGet(idACDLocalPlayer);

	if (!ptACD)
		return;


	float flValue = ptACD->GetHitPointsCurrent();
	float flHPMaxTotal = ptACD->AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL);
	RActorGlobals& tRActorGlobals = *CGameGet().ptRActorGlobals;
	ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(tRActorGlobals.idLowHealthParticle);
	SoundInstance * ptSound = SoundTryToGet(tRActorGlobals.idLowHealthSound);

	float flPct = flValue / flHPMaxTotal;
	if (flValue > EPSILON &&
        flPct < 0.3f &&
        !CGameInScriptedSequence() &&
        PlayerGetLocalPlayerCount() == 1)
	{





            if (!ptParticleSystem)
            {
                ParticleSystemAddParams tParams(GlobalSNOGet(GLOBALSNO_PARTICLE_FULLSCREEN_LOW_HEALTH), cgtPRTransformIdentity);
                tRActorGlobals.idLowHealthParticle = ParticleSystemAdd(tParams);
            }
            if (!ptSound)
            {
                RActor* ptRActor = RActorTryToGet(LocalPlayerRActorGet());
                if (ptRActor)
                {
                    TriggerEvent tTriggerEvent;
                    TriggerEventInit(tTriggerEvent);
                    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
                    tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_SOUND;
                    tTriggerEvent.snoname.Set(SOUND_SNO, GlobalSNOGet(GLOBALSNO_SOUND_LOW_HEALTH));
                    tTriggerEvent.dwID = 0;

                    AttachmentID idAttachment = RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
                    Attachment* ptAttachment = AttachmentTryToGet(idAttachment);
                    if (ptAttachment)
                    {
                        tRActorGlobals.idLowHealthSound = ptAttachment->idAttachedObject;
                    }
                }
            }

	}
	else
	{
		if (ptParticleSystem)
		{
			ptParticleSystem->PlayOutro();
		}
		tRActorGlobals.idLowHealthParticle = PARTICLESYSTEMID_INVALID;

        if (ptSound)
        {
            SoundFadeOut(ptSound->id);
        }
        tRActorGlobals.idLowHealthSound = SOUNDID_INVALID;
	}


    BOOL bNearHunter = FALSE;
    if (GameCommonDataExists() && GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx >= 0)
    {
        WorldPlace placeLocal = ptACD->GetPlace();
        for (const Player* ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            if (PlayerIsLocal(ptPlayer))
            {
                continue;
            }

            if (ptPlayer->nPlayerIndex == GameGetActiveGameCommonData().tGameSyncedData.nHunterPlayerIdx)
            {
                ActorCommonData* ptACDHunter = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDHunter)
                {
                    WorldPlace placeHunter = ptACDHunter->GetPlace();
                    if (placeHunter.idSWorld == placeLocal.idSWorld)
                    {
                        Vector2D vDist = (placeLocal.wp - placeHunter.wp).As2D();
                        static const float HUNTER_DIST_FOR_FULL_SCREEN_EFFECT = 100.0f;
                        static const float HUNTER_DIST_FOR_FULL_SCREEN_EFFECT_SQR = HUNTER_DIST_FOR_FULL_SCREEN_EFFECT * HUNTER_DIST_FOR_FULL_SCREEN_EFFECT;
                        if (vDist.MagnitudeSquared() <= HUNTER_DIST_FOR_FULL_SCREEN_EFFECT_SQR)
                        {
                            bNearHunter = TRUE;
                            break;
                        }
                    }
                }
            }
        }
    }


    ParticleSystem* ptParticleSystemHunter = ParticleSystemTryToGet(tRActorGlobals.idHunterNearbyParticle);
    if (bNearHunter)
    {
        if (!ptParticleSystemHunter)
        {
            const RActor* ptRActor = RActorTryToGet(LocalPlayerRActorGet());
            if (ptRActor)
            {
                ParticleSystemAddParams tParams(GlobalSNOGet(GLOBALSNO_PARTICLE_FULLSCREEN_HUNTER_NEARBY), PRTransform(cgqIdentity, ptRActor->tLocation.tWorldLocation.transform.wp));
                tRActorGlobals.idHunterNearbyParticle = ParticleSystemAdd(tParams);
            }
        }
    }
    else
    {
        if (ptParticleSystemHunter)
        {
            ptParticleSystemHunter->PlayOutro();
        }
        tRActorGlobals.idHunterNearbyParticle = PARTICLESYSTEMID_INVALID;
    }

}



// NON_MATCHING: 6424 vs 6356 bytes; calls, constants and control flow match. Inlined sUpdateActorEffectsForIcons keeps the last ShouldShowPrompt test and the eConvIcon > 0 test as one merged branch (original jumps straight to the 4156 remove), sUpdateActorEffectsForAttributes tail-duplicates one more TriggerEvent block, and sRActorBroadcastLookAt spills uNow at a different point.
void RActorPreThreadSafeUpdate(
    RActor * ptRActor,
    ActorCommonData * ptACD,
    float flDeltaTime,
    const ActorDefinition * ptDefActor)
{







    if (ptRActor->tParentData.m_eParentType != PARENT_NONE)
	{
		RActorUpdatePropertiesFromParent(ptRActor, ptDefActor, ptACD);
	}

	if (ptACD)
	{
        GameDebugSetCurrentUpdatingActor(ptRActor->idACD);



        if (ptRActor->dwFlags & (1 << RACTOR_NEED_EFFECT_UPDATE_BIT))
        {
            ptRActor->dwFlags &= ~(1 << RACTOR_NEED_EFFECT_UPDATE_BIT);
            sUpdateActorEffectsForAttributes(ptRActor, *ptACD, ptDefActor);
        }

        if (!ptACD->IsItem())
        {
            sUpdateActorEffectsForIcons(ptRActor, *ptACD);
            sUpdateActorScale(ptRActor, *ptACD);
        }

        switch (ptACD->GetGizmoType())
        {
        case ACTORGIZMO_BANNER:
            sUpdateActorBannerRing(ptRActor, *ptACD);
            sUpdateActorBannerIcons(ptRActor, *ptACD);




                sRActorInitBannerData(ptRActor, *ptACD);
            break;
        case ACTORGIZMO_SHARED_STASH:
            sUpdateSharedStash(ptRActor, *ptACD);
            break;


        case ACTORGIZMO_MAILBOX:
            sUpdateActorMailIcons(ptRActor, *ptACD);
            break;

        case ACTORGIZMO_WEEKLYCHALLENGE_REWARD:
            sUpdateWeeklyChallengeIcons(ptRActor, *ptACD);
            break;
        }


        sRActorBroadcastLookAt(ptRActor, *ptACD);
        sRActorSmoothDOTUpdate(*ptACD);
	}

	const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
	ptRActor->wvVelocity = (loc.transform.wp - ptRActor->wpLastUpdate) * (1.0f / flDeltaTime);
	ptRActor->wpLastUpdate = loc.transform.wp;


	if (ptRActor->dwFlagsEx & (1 << RACTOREX_SLOW_TIME_RADIUS_BIT))
	{
        float flSlowTimeRadius = TagMapGetFloat(ptDefActor->hTagMap, TAG_ACTOR_SLOW_TIME_RADIUS);
        if (flSlowTimeRadius > EPSILON)
        {
            float flSlowTimeFactor = TagMapGetFloat(ptDefActor->hTagMap, TAG_ACTOR_SLOW_TIME_FACTOR);
            Sphere ws;
            ws.wpCenter = loc.transform.wp;
            ws.wdRadius = flSlowTimeRadius;
            RActorPhysicsSlowTime(ws, flSlowTimeFactor);
        }
	}

    if (ptRActor->ptRigidBody)
    {
        RActorPhysicsUpdate(ptRActor, ptDefActor, ptACD);
    }


    else if(ptRActor->ptCActorMovement && ptRActor->tParentData.m_eParentType == PARENT_NONE)
    {





                if (!ptACD)
                {
                    ptRActor->ptCActorMovement->Update();
                }
                else
                {
                    ptRActor->ptCActorMovement->Interpolate();
                }
    }







	if (ptRActor->ptRigidAnim)
	{
		sUpdateRActorRigidAnimation(ptRActor);
	}

	if (ptRActor->tTilt.Update())
	{
		RActorUpdateClientTransform(ptRActor, ptDefActor, ptACD);
	}

	if (ptACD)
	{

        FastAttribGroupApplyDirty(ptACD->AttributesGetAttributeGroup());
	}
}




void RActorThreadSafeUpdate(RActor * ptRActor, Vector3D & wpActor, ActorCommonData * ptACD, float flDeltaTime, const ActorDefinition * ptDefActor)





{

    BOOL fLocalLock = (ptDefActor == NULL);

    if (fLocalLock)
        ptDefActor = (const ActorDefinition *)gptGroupDefActor->LockR(ptRActor->snoActor, FALSE);






    if (ptRActor->ptParticleParams && (ptRActor->ptParticleParams->eType < 4 || ptRActor->ptParticleParams->eType > 8))
    {
        BOOL bDeleted = ParticleSystemUpdateParamsForRActor(ptRActor, wpActor, flDeltaTime);





        if (bDeleted)

            ptRActor->dwFlags |= (1 << RACTOR_DELETE_THIS_TICK_BIT);
    }





    sUpdateActorAppearanceTS(ptRActor, ptACD);

    if (ptRActor->ptActorAnim)

        RActorUpdateBones(ptRActor, flDeltaTime, ptDefActor, FALSE);


    if (fLocalLock)

        gptGroupDefActor->UnlockR((void *&)ptDefActor);
}











































































































































































































































void RActorUpdate(RActor * ptRActor, float flDeltaTime)
{







    ScopedActorDefinitionLockR ptDefActor(ptRActor->snoActor);

    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);

    Vector3D wpActor;
    RActorPreThreadSafeUpdate(ptRActor, ptACD, flDeltaTime, ptDefActor);
    RActorThreadSafeUpdate(ptRActor, wpActor, ptACD, flDeltaTime, ptDefActor);
    RActorPostThreadSafeUpdate(ptRActor, wpActor, ptACD, flDeltaTime, ptDefActor);


}








































int32 RActorRegisterParentObject(RActor * ptRActor)
{
    BOOL fRActorFixedUpdate = (ptRActor->dwFlagsEx & (1 << RACTOREX_FIXED_UPDATE_BIT)) && (GameIsPaused() || GameIsSoftPaused());
    float flCurrentTime = fRActorFixedUpdate ? CGameGetRealTime() + CGameGetRealDeltaTime() : CGameGetClientTime() + CGameGetClientDeltaTime();
    if (ptRActor->flLastRegisterTime == flCurrentTime)

        return ptRActor->nSimTreeDepth;

    ptRActor->flLastRegisterTime = flCurrentTime;

    int32 nParentDepth;
    if (ptRActor->tParentData.m_eParentType != PARENT_NONE)

        nParentDepth = ptRActor->tParentData.RegisterParentObject();

    else if (ptRActor->ptParticleParams)
    {

        ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(ptRActor->ptParticleParams->idParticleSystem);
        if (ptParticleSystem)

            nParentDepth = ptParticleSystem->RegisterParentObject();
        else
            nParentDepth = -1;
    }
    else
        nParentDepth = -1;
    ptRActor->nSimTreeDepth = nParentDepth + 1;

















    ParentObjectRegister(ptRActor->nSimTreeDepth, PARENT_RACTOR, ptRActor->id, ptRActor, ptRActor->idACD);



    return ptRActor->nSimTreeDepth;
}


inline void RActorRegisterCorpse(RActor * ptRActor, BOOL fIsMonster)
{
    CGame & tCGame = CGameGet();


    if (!ptRActor->posMonsterCorpseList)
    {
        ptRActor->posMonsterCorpseList = tCGame.ptRActorGlobals->plistMonsterCorpses->AddTail_Tracked(ptRActor->id);
    }

    if (fIsMonster)
    {
        float flLifeTime = (PlayersGetCount() > 1) ? MONSTERS_CORPSES_LIFE_TIME_BEFORE_FADING_MULTIPLAYER : MONSTERS_CORPSES_LIFE_TIME_BEFORE_FADING;
        ptRActor->flLife = Math::Min(ptRActor->flLife, flLifeTime);
    }
}











void RActorsEnforceMonsterCorpseLimit()
{

    CGame & tCGame = CGameGet();


    while (tCGame.ptRActorGlobals->plistMonsterCorpses->Count() > MAX_MONSTERS_CORPSES_BEFORE_FADING)
    {
        RActorID idRActor = (RActorID)tCGame.ptRActorGlobals->plistMonsterCorpses->GetHead();
        tCGame.ptRActorGlobals->plistMonsterCorpses->RemoveHead();


        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            ptRActor->posMonsterCorpseList = NULL;
            ptRActor->flLife = 0.0f;
        }
    }
}

































































































































































































































































































RGBAColorValue RActorGetMeshLighting(const RActorID idRActor)
{
    RGBAColorValue color;
    color.Set(1.0f, 1.0f, 1.0f, 1.0f);

    const RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor && (ptRActor->dwFlags & (1 << RACTOR_MESH_LIT_BIT)))
    {


        color = ptRActor->rgbavalSceneCurrent;
    }

    return color;
}








static BOOL sSnapToLiquidSurface(
    const SWorldID idSWorld,
    Vector3D & wp)
{
    StructureIntersectData tStructIntersect(idSWorld);
	Vector3D wpTop(wp.x, wp.y, wp.z + 10.0f);
    tStructIntersect.wline.Set(wpTop, wp);
    tStructIntersect.dwIntersectFlags = (1 << INTERSECTFLAGS_IGNORE_NON_LIQUID_SURFACES_BIT);
    BOOL fIntersect = CScenesIntersectLineSeg(tStructIntersect, SkipHolodeckScenes);
    if (!fIntersect)
    {
        // inference: early-return form; line 6018 (store at col 7) sits at the function's own indentation, so it is not inside this if
        return FALSE;
    }
	wp.z = tStructIntersect.wpIntersect.z;
    // inference: returns TRUE here (no row; the caller at 6123 only tests the result)
    return TRUE;
}



void RActorUpdateClientTransform(
    RActor * ptRActor,
    const ActorDefinition * pdefActor,
    const ActorCommonData * ptACD)
{



    BOOL fLocalLock = (pdefActor == NULL);
    if (fLocalLock)
    {
        pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(ptRActor->snoActor, FALSE);
    }




    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

    PRSTransform transform(loc.transform.q, loc.transform.wp, loc.flScale);


    // inference: the last operand is most likely the inline ActorMovement::IsProjectile() (declared at DWARF ActorMovement.h:458, no body in src); its ldrb/tbnz carry 6048:9, so no inline DIE survived
    if (((sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_8_WAY_ROTATION_BIT)) || CWorldIsRetro()) &&
        !ptRActor->tParentData.ParentIsValid() &&
        ptACD && ptACD->IsAlive() &&
        ptRActor->ptCActorMovement != NULL && !(ptRActor->ptCActorMovement->m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE)))
    {
        Vector3D vFront = loc.transform.q.Rotate(cgwvXAxis);
        vFront.z = 0.0f;
        if (vFront.MagnitudeSquared() > 1e-12f)
        {
            const int32 QUANTIZED_ROTATION_NUM_QUADRANTS = 8;
            const float QUANTIZED_ROTATION_ANGLES_PER_QUADRANT = FULL_CIRCLE / QUANTIZED_ROTATION_NUM_QUADRANTS;

            Angle aYawOrig = Math::ATan(vFront.y, vFront.x);
            aYawOrig = Math::NormalizeAngle(aYawOrig);
            float flUnitYaw = aYawOrig / FULL_CIRCLE;
            int32 nQuadrant = Math::FloatRoundToInt(flUnitYaw * QUANTIZED_ROTATION_NUM_QUADRANTS);
            // inference: wrap of the top quadrant; no row of its own, its cmp #7 / fcsel sit on 6069:46
            if (nQuadrant >= QUANTIZED_ROTATION_NUM_QUADRANTS)
            {
                nQuadrant = 0;
            }
            Angle aYawNew = (float)nQuadrant * QUANTIZED_ROTATION_ANGLES_PER_QUADRANT;
            Quaternion qRotNegateOld;
            Quaternion qRotNew;
            qRotNegateOld.FromYaw(-aYawOrig);
            qRotNew.FromYaw(aYawNew);
            transform.q = transform.q * qRotNegateOld;
            transform.q = transform.q * qRotNew;
        }
    }

	if (ptRActor->dwFlags & (1 << RACTOR_OFFSET_CENTER_BIT))
	{

        Vector3D wpCenterScaledRotated = ptRActor->transformClient.Rotate(pdefActor->wsBounds.wpCenter);
        transform.wp -= wpCenterScaledRotated;
	}



	if (ptRActor->dwFlags & (1 << RACTOR_USE_SHEATH_HP_BIT))
	{
		int32 nHardpointIndexToUse = HardpointIndexFromName(ptRActor->snoAppearance, "HP_sheath");
		if (nHardpointIndexToUse != -1)
		{
			ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
			const Structure & tStructure = pdefAppearance->tStructure;

			transform *= tStructure.ptHardpoints[nHardpointIndexToUse].transform.Inverse();
		}
	}

	if (ptRActor->ptRigidAnim)
	{
		RigidAnim & tRigid = *ptRActor->ptRigidAnim;

		Vector3D wvAxisWorldSpace = transform.Rotate(tRigid.wvAxis);

		Quaternion q;
		q.SetRotation(wvAxisWorldSpace, tRigid.a);
		transform.q = q * transform.q;

		transform.wp += (tRigid.wvAxis * tRigid.wdDist);
	}



    if (ptACD == NULL)
    {
        ptACD = ACDTryToGet(ptRActor->idACD);
    }

	if (ptACD &&
		ptACD->IsItem())
	{
		if (sSnapToLiquidSurface(loc.idSWorld, transform.wp))
		{

			loc.transform.wp = transform.wp;
		}
	}

	if (ptRActor->ptCActorMovement)
	{
        TiltType nTiltType = (TiltType) TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_TILT_TYPE);

        ptRActor->ptCActorMovement->SetTilt(nTiltType, FALSE);
	}

    ptRActor->tTilt.Apply(transform);
    ptRActor->tHitJitter.Apply(transform);
    ptRActor->tSink.Apply(*ptRActor, transform);

    ptRActor->transformClient = transform;


	Matrix4x4 mStretch = cgm4x4Identity;


	mStretch._23 = ptRActor->flShear * ptRActor->flStretch;
	mStretch._33 = ptRActor->flStretch;

    Matrix4x4 mWorld;
    transform.ToMatrix(mWorld);
    ptRActor->mWorld = mStretch * mWorld;


    Sphere wsOld = loc.wsBounds;
	LocationSetBounds(ptRActor->snoActor, loc, pdefActor);




    if (ptRActor->ptActorAnim &&
        ptRActor->ptActorAnim->ptRagdoll &&
		ptRActor->ptActorAnim->ptRagdoll->ShouldUpdateBounds())
	{
		RActorComputeBoneBoundingSphere(ptRActor);
	}
    // 100000.0f is MAX_WORLDDISTANCE (DWARF MathGeneral.h:33 const_value 100000.0f; the '||' columns 58 fit its 17-character name); src MathGeneral.h:13 has 3.402823466e+38f, so the literal is written
    if (Math::Abs(wsOld.wpCenter.x) >= 100000.0f ||
        Math::Abs(wsOld.wpCenter.y) >= 100000.0f ||
        Math::Abs(wsOld.wpCenter.z) >= 100000.0f ||
        Math::Abs(wsOld.wdRadius) >= 100000.0f)
    {

        RActorFlagForDeletion(ptRActor);
    }
    else
    {


        RActorUpdateSphereTreeNode(ptRActor, wsOld);
    }

    if (fLocalLock)
    {
        gptGroupDefActor->UnlockR((void *&)pdefActor);
    }
}






























































































































void RActorInternalSetCurrentFacing(RActor * ptRActor, Angle aYaw)
{

    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
    ptRActor->aYaw = aYaw;

    loc.transform.q.SetRotation(cgwvZAxis, aYaw);

}



void RActorSnapToPosition(RActor * ptRActor, const Vector3D & wpPos, SSceneID idSScene, const ActorDefinition * pdefActor, const ActorCommonData * ptACD)
{






    RagdollType eRagdollType = RActorGetRagdollType(ptRActor);
    if (eRagdollType != INVALID_RAGDOLL_TYPE && eRagdollType != PARTIAL_RAGDOLL_TYPE)
        return;




    RActorInternalSetPosition(ptRActor, wpPos, idSScene, idSScene);

    RActorUpdateClientTransform(ptRActor, pdefActor, ptACD);
}





















void RActorSetScale(RActor * ptRActor, const float flScale, BOOL bSetAsBaseScale, const ActorDefinition * pdefActor)
{




    float flScaleToUse = Math::Pin(flScale, 1e-06f, 1000.0f);






    BOOL fLocalLock = (pdefActor == NULL);
    if (fLocalLock)
    {

        pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(ptRActor->snoActor, FALSE);

    }






    ptRActor->wdSimpleShadowRadius = flScaleToUse * TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SIMPLE_SHADOW_RADIUS);

    ActorLocationSetScale(ptRActor->tLocation, flScaleToUse);

    RActorUpdateClientTransform(ptRActor, pdefActor, NULL);

    if (bSetAsBaseScale)
        ptRActor->flBaseScale = flScaleToUse;
















    if (fLocalLock)
    {
        gptGroupDefActor->UnlockR((void *&)pdefActor);

    }
}



























































































void RActorGetCenterPoint(const RActor * ptRActor, Vector3D & wpCenter)
{





    const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
    wpCenter = loc.transform.wp + cgwvZAxis * (loc.wcylBounds.wdHeight * 0.5f);
}


void RActorGetTopPoint(const RActor * ptRActor, Vector3D & wpTop)
{





    const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
    wpTop = loc.transform.wp + cgwvZAxis * loc.wcylBounds.wdHeight;
}











RActorID RActorFindByANN(const ACDNetworkName idANN)

{

    ACDID idACD = ACDFromANN(idANN);
    if (idACD == ACDID_INVALID)
        return RACTORID_INVALID;



    return (RActorID)ACDGetOwner(idACD);
}


void RActorLocalPlayerActorSet(const ACDID idACDPlayer, Player & tPlayer)


{

    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    PlayerSetACD(tPlayer, idACDPlayer);
    tPlayer.idPlayerRActor = RACTORID_INVALID;
    tPlayer.nCurrentLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    tPlayer.nCurrentAltLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL));
    tPlayer.nCurrentHighestHeroSoloRiftLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL));

    SWorldID idSWorld = tACDPlayer.GetSWorldID();
    SWorldID idSWorldCurrent = CGameGetCurrentSWorldID();

    if (idSWorldCurrent != idSWorld)
    {
        if (idSWorldCurrent != SWORLDID_INVALID)
        {
            CWorldChange(idSWorld);
        }
        else
        {
            CWorldEnter(idSWorld, GAMEMODE_PLAYING);
        }
    }
    else
    {

        RActorID idRActor = (RActorID)ACDGetOwner(idACDPlayer);
        PlayersSetMainPlayerRActor(&tPlayer, idRActor);
    }
}




void RActorSetAimTarget(const RActorID idRActor, const ACDID idACDTarget, const TargetType eTargetType, const WorldPlace & tPlace)
{










    RActor * ptRActor = RActorGet(idRActor);
    if (!ptRActor)
        return;



    Target tTarget;
    tTarget.eType = eTargetType;
    tTarget.idACDTarget = idACDTarget;
    tTarget.tPlace = tPlace;

    if (eTargetType == TT_ACTOR)
    {
        if (idACDTarget != ACDID_INVALID)
            ptRActor->tAimTarget.InitializeFromACD(idACDTarget, FALSE, FALSE);
        else
            ptRActor->tAimTarget.Clear();
    }
    else
    {



        ptRActor->tAimTarget = tTarget;
    }
}




static RActorID sRActorCreateFromACD(ACDID idACD)
{

    ActorCommonData & tACD = ACDGet(idACD);



    RActorID idRActor = (RActorID)tACD.idOwner;
    if (!RActorTryToGet(idRActor))
    {


        XGrowableMap<ACDNetworkName, ACDNetworkName, QueuedRActorInfo, QueuedRActorInfo &> & queuedRActors = CGameGet().ptRActorGlobals->queuedRActors;
        if (queuedRActors.LookupPtr(tACD.ann))
        {
            queuedRActors.RemoveAt(tACD.ann);
        }



        idRActor = sRActorCreate(idACD,

                                 tACD.snoActor,
                                 SNO_NONE,
                                 tACD.tName,
                                 tACD.tLocation.tWorldLocation.flScale, FLOAT_INFINITE_LIFETIME, tACD.tLocation.tWorldLocation.transform,
                                 0, RENDERCONTEXT_DEFAULT, SSCENEID_INVALID, 0, NULL, FALSE);





        if (tACD.ptInventory)
        {
            ACDReapplyVisualInventory(idACD);
        }
    }

    return idRActor;
}



RActorID RActorCreateForACD(const ACDID idACDOwner, const SNO snoActor)
{


    ActorCommonData & tACD = ACDGet(idACDOwner);
    tACD.snoActor = snoActor;

    RActorID idRActor = sRActorCreateFromACD(idACDOwner);


    if (PlayerIsLocal(idACDOwner))
    {
        PlayersSetMainPlayerRActor(PlayerGetByACD(idACDOwner), idRActor);
    }

    return idRActor;
}




void RActorsCreateForWorld()
{

    SWorldID idSWorld = CGameGetCurrentSWorldID();
    ACDIterator tIterator;


    while (TRUE)
    {
        ActorCommonData * ptACD = ActorCommonDataIterate(tIterator);
        if (!ptACD)
            break;



        if (!ptACD->IsInWorld())
            continue;



        SWorldID idSWorldForACD = ptACD->GetSWorldID();

        if (idSWorldForACD != idSWorld)
            continue;



        sRActorCreateFromACD(ptACD->id);
    }
}



WorldPlace RActorGetWorldPlace(const RActor * ptRActor)
{


    if (ptRActor->idACD != ACDID_INVALID)
    {

        return ACDGet(ptRActor->idACD).GetPlace();
    }

    const WorldLocation & tLocation = ptRActor->tLocation.tWorldLocation;

    WorldPlace tPlace;
    tPlace.idSWorld = tLocation.idSWorld;
    tPlace.wp = tLocation.transform.wp;

    return tPlace;
}











































































































































































































































void RActorACDExitKnown(ActorCommonData & tACD)
{


    tACD.nRefCount--;

    if (tACD.nRefCount > 0)
        return;




    XGrowableMap<ACDNetworkName, ACDNetworkName, QueuedRActorInfo, QueuedRActorInfo &> & queuedRActors = CGameGet().ptRActorGlobals->queuedRActors;
    if (queuedRActors.LookupPtr(tACD.ann))
    {
        queuedRActors.RemoveAt(tACD.ann);
    }

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_QUEST_MONSTER)) || (tACD.dwFlags & (1 << ACD_QUEST_OBJECTIVE)))
    {
        CQuestsUnFlagACD(tACD);
    }

    VisualInventory * ptVisualInventory = VisualInventoryGet(tACD.id);
    if (ptVisualInventory)
    {



        ptVisualInventory->InitializeLast();
    }

    if (tACD.IsNecroCorpse())
    {
        CPlayersRemoveNecroCorpse(&tACD);
    }


    RActorACDEffectsRemove(tACD.id);

    tACD.RemoveAllClientEffects();


    UIMinimap_ExitKnown(tACD.id);




    RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);
    if (idRActor != RACTORID_INVALID)
    {









        if (PlayerIsLocal(idRActor))
        {
            PlayersSetMainPlayerRActor(PlayerGetByRActor(idRActor), RACTORID_INVALID);
        }


        if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DELETED_ON_SERVER)) == 1)
        {

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor)
            {

                sRActorStartCorpseFadeTimer(ptRActor, &tACD);


                UIConversation::CancelLineForRActor(ptRActor->id);
                sActorRemoveIcons(ptRActor);

                SNOPreload::AddClientActorBySNO(ptRActor->snoActor);

                RActorAnimationOnACDDelete(*ptRActor);

                sRActorSetACD(ptRActor, ACDID_INVALID, NULL);
            }
        }
        else
        {
            RActorDelete(idRActor);
        }
    }


    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer)
    {
        PlayerSetACD(*ptPlayer, ACDID_INVALID);
        ptPlayer->idPlayerRActor = RACTORID_INVALID;
        SNOPreload::UpdateEquipped();
        SNOPreload::UpdateSkills();
    }

    const ActorLocation tLocation = tACD.GetLocation();

    BOOL bWasInInventory = (tLocation.eActorLocation == AL_INVENTORY);
    if (bWasInInventory)
    {
        CACDInventoryDestroy(tACD);
    }

    ACDDelete(tACD.id, FALSE);

    if (bWasInInventory)
    {
        SNOPreload::UpdateEquipped();

        UIInventory::Console::OnInventoryChanged(tLocation, tLocation, FALSE, TRUE);
        UIFollower::Console::OnInventoryChanged(tLocation, tLocation, TRUE);
        UISkills::Console::OnInventoryChanged(tLocation, tLocation, TRUE);
        UIInventory::Console::OnStashChanged(tLocation, tLocation, TRUE);
        UIInventory::Console::OnShopChanged(tLocation, tLocation, FALSE, TRUE);
        UIParty::Console::OnInventoryChanged(tLocation, tLocation, TRUE);
    }

}

























































































































































































































static void sActorAttributeGizmoChangeState(
    RActorID idRActor,
    const int32,
    const int32)
{
    RActor * ptRActor = RActorGet(idRActor);
    ACDID idACD = ptRActor->idACD;

    ActorCommonData & tACD = ACDGet(idACD);
    int32 nGizmoState = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);

    switch (nGizmoState)
    {
    case 0:
        RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_GIZMO_CHANGE_STATE_TO_0, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
        break;
    case 1:
        RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_GIZMO_CHANGE_STATE_TO_1, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
        break;
    case 2:
        RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_GIZMO_CHANGE_STATE_TO_2, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
        break;
    case 3:
        RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_GIZMO_CHANGE_STATE_TO_3, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
        break;
    case 4:
        RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_GIZMO_CHANGE_STATE_TO_4, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
        break;
    }
}



























































































static void sActorAttributeDeleteRopeOverlay(
    RActorID idRActor)
{
    RActor * ptRActor = RActorGet(idRActor);
    RActorAttachmentDelete(idRActor, ptRActor->idRopeAttachment);
    ptRActor->idRopeAttachment = ATTACHMENTID_INVALID;
}



static void sActorAttributeModifyRopeOverlay(
    RActorID idRActor)
{
    RActor * ptRActor = RActorGet(idRActor);
    ActorCommonData & tACD = ACDGet(ptRActor->idACD);
    SNO snoRope = tACD.AttributesGetInt(ATTRIBUTE_ROPE_OVERLAY);
    if (snoRope == SNO_NONE)
        return;



    if (!RActorTryToGet(idRActor))  // inference: line 7474 has only RActorTryToGet's inlined rows; its NULL result returns
        return;



    if (snoRope == 0)  // literal 0: no enumerator; cbz at 7479
    {
        sActorAttributeDeleteRopeOverlay(idRActor);
    }
    else
    {
        sAddRope(idRActor, snoRope);
    }
}


static void sActorAttributePowerEffectChange(
    SNO snoPower)
{
    if (snoPower == SNO_NONE)
        return;



    CGame & tCGame = CGameGet();
    BOOL bInitialized;
    if (!tCGame.ptRActorGlobals->tAppearanceAttributePowerInitializedMap.Lookup(snoPower, bInitialized) ||
        !bInitialized)
    {
        ScopedPowerDefinitionLockR pdefPower(snoPower);
        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_0_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_1_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_2_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_3_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_4_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_5_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_6_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_7_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_8_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_9_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_10_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_11_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_12_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_13_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_14_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_15_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_16_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_17_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_18_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_19_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_20_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_21_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_22_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_23_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_24_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_25_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_26_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_27_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_28_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_29_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_30_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_E);

        sInitializeBuffEffectsForPower(pdefPower, snoPower, TAG_POWER_BUFF_31_EFFECT_GROUP,
            ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_NONE,
            ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_A,
            ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_B,
            ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_C,
            ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_D,
            ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_E);



        tCGame.ptRActorGlobals->tAppearanceAttributePowerInitializedMap.SetAt_Tracked(snoPower, TRUE);
    }
}




static void sApplyFreeCastEffects(
    SNO snoPower,
    Player * ptPlayer)
{
    for (int32 i = 0; i < NUM_SKILL_BUTTONS; i++)
    {
        const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, i);
        if (tButtonData.m_snoPower == snoPower)
        {
            UIGame::ApplyFreeCastEffects(ptPlayer, i);
        }
    }
}




static void sRActorAddCosmeticBuff(RActor * ptRActor, CosmeticItemType eType, SNO snoEffect)
{



    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CURRENT_COSMETIC_EFFECT, eType);

    TriggerEvent tAttachParticleSystem;
    TriggerEventInit(tAttachParticleSystem);
    tAttachParticleSystem.eType = TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP;
    tAttachParticleSystem.snoname.Set(EFFECTGROUP_SNO, snoEffect);
    tAttachParticleSystem.dwID = tKey.nValue + 1101;

    AttachmentID idAttachment = RActorSendRMessage(*ptRActor, tAttachParticleSystem, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
    if (!ptRActor->ptEffectAttachments)
    {
        ptRActor->ptEffectAttachments = new AppearanceAttributeAttachmentIDMap(16, NULL);
    }

    ptRActor->ptEffectAttachments->SetAt_Tracked(tKey, idAttachment);
}




static void sRActorRemoveCosmeticBuff(RActor * ptRActor, CosmeticItemType eType)
{



    FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CURRENT_COSMETIC_EFFECT, eType);
    AttachmentID idAttachment;

    if (ptRActor->ptEffectAttachments && ptRActor->ptEffectAttachments->Lookup(tKey, idAttachment))
    {
        TriggerEvent tAttachParticleSystem;
        TriggerEventInit(tAttachParticleSystem);
        tAttachParticleSystem.eType = TRIGGEREDEVENTTYPE_OUTRO_OBJECT;
        tAttachParticleSystem.dwID = tKey.nValue + 1101;
        RActorSendRMessage(*ptRActor, tAttachParticleSystem, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
        ptRActor->ptEffectAttachments->RemoveAt(tKey);
    }

}


void RActorAddCosmeticBuff(RActor * ptRActor, const CosmeticItemType eType, const GBID gbidItem, const BOOL fSuppressTransitionEffect)
{



    if (gbidItem == GBID_NONE || gbidItem == INVALID_GBID)
        return;





    ScopedItemDefinitionLockR ptItemDef(gbidItem);


    if (!ptItemDef)
        return;

    if (!fSuppressTransitionEffect && ptItemDef->snoStartEffect != SNO_NONE)

        EffectGroupCreateUntracked(ptItemDef->snoStartEffect, ptRActor->id, 0, FALSE, FALSE);


    if (ptItemDef->snoMainEffect != SNO_NONE)

        sRActorAddCosmeticBuff(ptRActor, eType, ptItemDef->snoMainEffect);


}


void RActorRemoveCosmeticBuff(RActor * ptRActor, const CosmeticItemType eType, const GBID gbidItem, const BOOL fSuppressTransitionEffect)
{



    if (gbidItem == GBID_NONE || gbidItem == INVALID_GBID)
    {


        sRActorRemoveCosmeticBuff(ptRActor, eType);
        return;
    }

    ScopedItemDefinitionLockR ptItemDef(gbidItem);


    if (!ptItemDef)
        return;

    if (!fSuppressTransitionEffect && ptItemDef->snoEndEffect != SNO_NONE)


        EffectGroupCreateUntracked(ptItemDef->snoEndEffect, ptRActor->id, 0, FALSE, FALSE);

    if (ptItemDef->snoMainEffect != SNO_NONE)


        sRActorRemoveCosmeticBuff(ptRActor, eType);
}

// NON_MATCHING: 9060 vs 9056 bytes; calls, constants, control flow and jump tables match. HIDDEN/ALPHA_ATTACHMENTS cases (7965/7972) reload ptRActor/dwFlags per case in the original, ours tail-merges them; XP prefetch (8391/8413) converts the old value before the < 0.1 test. Tried ternary, && order and nested ifs.
void RActorsACDAttributeChanged(
    ActorCommonData & tACD,
    FastAttribKey tKey,
    FastAttribValue tOldValue,
    FastAttribValue tValue)
{
    Attrib eAttrib = KeyGetAttrib(tKey);


    if (eAttrib == ATTRIBUTE_DURABILITY_CUR)
    {
        UIShop::Console::Refresh(PlayerGetByACD(tACD.id));
    }


    RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);

    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)  // inference: no row of its own; the NULL result of RActorTryToGet exits
        return;



    if (eAttrib >= ATTRIBUTE_POWER_BUFF_VISUAL_EFFECT_START && eAttrib <= ATTRIBUTE_POWER_BUFF_VISUAL_EFFECT_END)
    {

        SNO snoPower = KeyGetParam(tKey);
        sActorAttributePowerEffectChange(snoPower);
        ptRActor->dwFlags |= (1 << RACTOR_NEED_EFFECT_UPDATE_BIT);
        return;
    }

    ACDID idACDLocalPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(idACDLocalPlayer);

    AppearanceAttributeEffectType value;
    CGame & tCGame = CGameGet();
    if (tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.Lookup(tKey, value))
    {


        ptRActor->dwFlags |= (1 << RACTOR_NEED_EFFECT_UPDATE_BIT);
    }

    switch (eAttrib)
    {
    case ATTRIBUTE_POWER_COOLDOWN:
    case ATTRIBUTE_POWER_COOLDOWN_START:
    case ATTRIBUTE_RUNE_A:
    case ATTRIBUTE_RUNE_B:
    case ATTRIBUTE_RUNE_C:
    case ATTRIBUTE_RUNE_D:
    case ATTRIBUTE_RUNE_E:


                UISkills::Console::Refresh(PlayerGetByACD(tACD.id));
        break;


    case ATTRIBUTE_LAST_ACD_KILLED_TIME:

        if (PlayerIsLocal(tACD.id) && !ptRActor->ptCActorMovement->IsMoving())
        {
            PlayerGetByACD(tACD.id)->tClientInfo.nKillsSinceMoving++;
        }
        break;


    case ATTRIBUTE_HIDDEN:


        if (tValue.nValue) ptRActor->dwFlags |= (1 << RACTOR_HIDE_BIT); else ptRActor->dwFlags &= ~(1 << RACTOR_HIDE_BIT);
        break;



    case ATTRIBUTE_ALPHA_ATTACHMENTS:

        if (tValue.nValue) ptRActor->dwFlags |= (1 << RACTOR_HIDE_ALPHA_ATTACHMENTS_BIT); else ptRActor->dwFlags &= ~(1 << RACTOR_HIDE_ALPHA_ATTACHMENTS_BIT);
        break;



    case ATTRIBUTE_CANT_START_DISPLAYED_POWERS:

        if (PlayerIsLocal(tACD.id) && tValue.nValue == 0)
        {
            CQuestsUpdateAllActors();
        }
        break;


    case ATTRIBUTE_SUMMONED_BY_ACDID:
        {
            ACDID idACDSummonedBy = ACDFromANN((ACDNetworkName)tValue.nValue);

            if (tACD.IsMirrorImage())
            {
                if (PlayerIsLocal(idACDSummonedBy))
                {
                    ptRActor->dwFlags |= (1 << RACTOR_IS_MIRROR_IMAGE_BIT);
                }

                tACD.tCollSettings.uDynamicMyFlags = 2;  // literal: no enumerator found for the collision flag value

                ptRActor->flTimeUntilFade = 0;
            }
        }
        break;

    case ATTRIBUTE_STEALTHED:

        if (tValue.nValue)
        {
            if (!ptACDLocalPlayer ||
      (!ACDIsFriendly(tACD, *ptACDLocalPlayer, FALSE) &&
                 !GameTypeIsPvP() &&
                 !ACDIsPlayer(tACD.id) &&

                 !ptACDLocalPlayer->IsDueling()))
            {
                ptRActor->dwFlags |= (1 << RACTOR_COMPLETELY_STEALTHED_BIT);
            }
        }
        else
        {
            ptRActor->dwFlags &= ~((1 << RACTOR_STEALTHED_BIT) | (1 << RACTOR_COMPLETELY_STEALTHED_BIT));
        }
        break;











    case ATTRIBUTE_GHOSTED:

        if (GameTypeIsPvP())
        {
            if (tValue.nValue)
            {
                if (!ptACDLocalPlayer ||
                    !ACDIsFriendly(tACD, *ptACDLocalPlayer, FALSE))
                {
                    ptRActor->dwFlags |= (1 << RACTOR_COMPLETELY_STEALTHED_BIT);
                }
            }
            else
            {





                if (ptACDLocalPlayer &&
                    (ACDIsFriendly(tACD, *ptACDLocalPlayer, FALSE) || !tACD.AttributesGetInt(ATTRIBUTE_STEALTHED)))
                {
                    ptRActor->dwFlags &= ~((1 << RACTOR_STEALTHED_BIT) | (1 << RACTOR_COMPLETELY_STEALTHED_BIT));
                }
            }
        }
        break;



    case ATTRIBUTE_CHEST_OPEN:

        if (tACD.IsGizmo())
        {
            GizmoType eType = tACD.GetGizmoType();
            if (eType == ACTORGIZMO_LORE_CHEST)
            {
                Player * ptPlayer = LocalPlayerGet();
                FastAttribKey tPlayerKey = MakeAttribKey(ATTRIBUTE_CHEST_OPEN, ptPlayer->idGameConnection);
                if (tPlayerKey == tKey && FastAttribValueWentAboveZero(tKey, tOldValue, tValue))
                {
                    if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_OPEN))
                    {
                        AnimPlayerIdleAnimationChanged(tACD, TAG_ACTOR_ANIMATION_OPEN);
                    }
                }
            }
        }
        break;























    case ATTRIBUTE_OBSERVER:

        if (tValue.nValue)
        {
            ptRActor->dwFlags |= PlayerIsLocal(tACD.id) ? (1 << RACTOR_STEALTHED_BIT) : (1 << RACTOR_COMPLETELY_STEALTHED_BIT);








            tACD.tCollSettings.uStaticOneRequiredFlags = 7;  // literal: no enumerator found
            tACD.tCollSettings.uDynamicMyFlags = 0;
            tACD.tCollSettings.uDynamicFlagsNoOverlap = 0;
        }
        else
        {
            ptRActor->dwFlags &= ~(1 << RACTOR_STEALTHED_BIT);
            ptRActor->dwFlags &= ~(1 << RACTOR_COMPLETELY_STEALTHED_BIT);

            ScopedActorDefinitionLockR ptDefActor(tACD.snoActor);
            tACD.tCollSettings.uStaticOneRequiredFlags = (uint16)ptDefActor->tCollData.tCollFlags.uStaticOneRequiredFlags;
            tACD.tCollSettings.uDynamicMyFlags = ptDefActor->tCollData.tCollFlags.uDynamicMyFlags;
            tACD.tCollSettings.uDynamicFlagsNoOverlap = ptDefActor->tCollData.tCollFlags.uDynamicFlagsNoOverlap;
        }
        break;


    case ATTRIBUTE_SLOWTIME_DEBUFF:
        {

            float flOldValue = AttribValueToFloat(tOldValue);
            float flValue = AttribValueToFloat(tValue);
            if (flValue > flOldValue)
            {

                float flCurrentSpeedScale = RActorAnimationGetSpeedScale(*ptRActor);
                RActorAnimationSetSpeedScale(*ptRActor, flCurrentSpeedScale * flValue);
                uint32 dwCurrentAnimEndTime = AnimPlayerGetAnimEndTime(tACD);
                if (dwCurrentAnimEndTime != ANIM_PLAY_FOREVER)
                {
                    uint32 dwTimeLeft = dwCurrentAnimEndTime - CGameGetClientTicks();
                    AnimPlayerUpdateAnimEndTime(tACD, CGameGetClientTicks() + Math::Max<uint32>(1, (uint32)(dwTimeLeft * (flCurrentSpeedScale / flValue))));
                }
            }
            else
            {



                uint32 dwCurrentAnimStartTime = AnimPlayerGetAnimEndTime(tACD);





                if (dwCurrentAnimStartTime < CGameGetClientTicks())
                {
                    float flCurrentSpeedScale = RActorAnimationGetSpeedScale(*ptRActor);
                    RActorAnimationSetSpeedScale(*ptRActor, flCurrentSpeedScale / flOldValue);
                    uint32 dwCurrentAnimEndTime = AnimPlayerGetAnimEndTime(tACD);
                    if (dwCurrentAnimEndTime != ANIM_PLAY_FOREVER)
                    {
                        uint32 dwTimeLeft = dwCurrentAnimEndTime - CGameGetClientTicks();
                        AnimPlayerUpdateAnimEndTime(tACD, CGameGetClientTicks() + Math::Max<uint32>(1, (uint32)(dwTimeLeft * (flCurrentSpeedScale / flOldValue))));
                    }
                }
            }
        }
        break;


    case ATTRIBUTE_ATTACHED_TO_ANN:
        {
            ACDID idACDOwner = ACDFromANN((ACDNetworkName)AttribValueToInt(tValue));

            if (idACDOwner != ACDID_INVALID)
            {
                RActorID idRActorOwner = (RActorID)ACDGetOwner(idACDOwner);
                if (idRActorOwner)  // inference: cbz on the owner id at 8187 col 21
                {
                    RActor * ptRActorOwner = RActorTryToGet(idRActorOwner);


                    HardpointHash pszHardpointName = ATTACH_HARDPOINT_NAME;
                    int32 nHardpointIndex = HardpointIndexFromName(ptRActorOwner->snoAppearance, ATTACH_HARDPOINT_NAME);
                    if (nHardpointIndex == -1)  // literal: HardpointIndexFromName's not-found value
                    {
                        pszHardpointName = DEFAULT_HARDPOINT_NAME;
                    }

                    RActorAttachExistingRActor(idRActorOwner, idRActor, pszHardpointName);
                }
            }
            else
            {

                if (!tACD.AttributesGetInt(ATTRIBUTE_ATTACHMENT_HANDLED_BY_CLIENT))
                {
                    ACDID idACDOwnerScoped = ACDFromANN((ACDNetworkName)AttribValueToInt(tOldValue));
                    RActorDetachExistingRActor(idRActor, idACDOwnerScoped);

                    WorldLocation & loc = tACD.GetWorldLocation();
                    RActorInternalSetPosition(ptRActor, loc.transform.wp, FALSE, SSCENEID_INVALID);  // inference: argument 3 leaves no instruction here or in the original; FALSE assumed
                    RActorSetScale(ptRActor, loc.flScale, TRUE, NULL);
                    RActorSetOrientation(ptRActor, loc.transform.q, NULL, NULL);
                }
            }
        }
        break;

    case ATTRIBUTE_GIZMO_STATE:

        sActorAttributeGizmoChangeState(idRActor, tOldValue.nValue, tValue.nValue);  // inference: unnamed int params, values unknown
        break;



    case ATTRIBUTE_FROZEN:
        {
            int32 nValue = AttribValueToInt(tValue);
            if (nValue >= 1)
            {

                if (AnimPlayerGetAnimReason(tACD) != ANIM_REASON_DEATH)
                {
                    RActorAnimationSetPaused(*ptRActor, TRUE);
                }

                if (PlayerIsLocal(tACD.id))
                {
                    ActorMovement * ptMove = tACD.GetActorMovement();
                    if (ptMove)
                    {
                        ptMove->StopNow();
                    }
                }
            }
            else
            {

                RActorAnimationSetPaused(*ptRActor, FALSE);
            }

            if (PlayerIsLocal(tACD.id))
                CPlayerInputClearCachedPower(PlayerGetByACD(tACD.id));
        }
        break;








    case ATTRIBUTE_STUNNED:
    case ATTRIBUTE_FORCE_GRIPPED:
        {

            int32 nOldValue = AttribValueToInt(tOldValue);
            int32 nValue = AttribValueToInt(tValue);
            if (nOldValue > 0 && nValue == 0)
            {
                if (AnimPlayerGetAnimReason(tACD) == ANIM_REASON_IDLE)
                {
                    AnimPlayerPlayIdle(tACD);
                }
            }

            if (PlayerIsLocal(tACD.id))
                CPlayerInputClearCachedPower(PlayerGetByACD(tACD.id));
        }
        break;


    case ATTRIBUTE_BLIND:
    case ATTRIBUTE_FEARED:
        if (PlayerIsLocal(tACD.id))
            CPlayerInputClearCachedPower(PlayerGetByACD(tACD.id));
        break;









    case ATTRIBUTE_ROPE_OVERLAY:

        sActorAttributeModifyRopeOverlay(idRActor);
        break;


    case ATTRIBUTE_TEAM:

        if (PlayerIsLocal(idRActor))
        {
            CPlayerChangedTeams(idRActor);
        }

        if (QuestIsACDObjective(tACD))
        {
            CQuestsFlagACD(tACD);
        }

        tACD.tCollSettings.eTeam = (uint8)tACD.AttributesGetInt(ATTRIBUTE_TEAM);
        break;

    case ATTRIBUTE_SKILL_TOTAL:

        if (PlayerIsLocal(idRActor))
        {
            int32 nOldValue = AttribValueToInt(tOldValue);
            int32 nValue = AttribValueToInt(tValue);
            if (nOldValue > 0 && nValue == 0)
            {

                SNO snoPower = KeyGetParam(tKey);
                if (snoPower != SNO_NONE)
                {
                    PlayerGetCursorMgr().HandleSkillRemoval(snoPower);
                }
            }
        }
        break;







    case ATTRIBUTE_SKILL:
        {
            HirelingClass eClass = HirelingGetClass(tACD);
            if (eClass)  // inference: cbz; HIRELING_NONE is 0
            {
                UIFollower::Console::Refresh();
            }
        }
        break;








    case ATTRIBUTE_SKILLKIT:

        if (PlayerIsLocal(idRActor))
        {
            Player * ptPlayer = PlayerGetByACD(tACD.id);
            UISkills::Console::InitializeForPlayer(ptPlayer->nPlayerIndex);
        }
        break;


    case ATTRIBUTE_LOOK_TARGET_SERVER_ANN:
        {
            ACDNetworkName idANNTarget = (ACDNetworkName)AttribValueToInt(tValue);
            RActorID idRActorTarget = RACTORID_INVALID;
            if (idANNTarget != ANN_INVALID)
            {
                idRActorTarget = RActorFindByANN(idANNTarget);
            }
            RActorAnimationLookAt(idRActor, idRActorTarget);
        }
        break;



    case ATTRIBUTE_EXPERIENCE_NEXT_LO:
        {
            int32 nLevel = tACD.AttributesGetInt(ATTRIBUTE_LEVEL);
            if (nLevel >= HERO_LEVEL_MIN && nLevel < HERO_LEVEL_MAX)
            {
                ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
                const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
                uint64 nXPToLevel = tExpTable.ptExperienceLevels[nLevel - 1].nDeltaXP;
                double fOOXPToLevel = 1.0 / nXPToLevel;
                double fOldValue = AttribValueToInt(tOldValue) * fOOXPToLevel;
                double fValue = AttribValueToInt(tValue) * fOOXPToLevel;
                if (fValue < 0.1 && fOldValue > 0.1)
                {

                    SNOName snoName(EFFECTGROUP_SNO, "g_levelUp");
                    Prefetch::AddToQueue(snoName, SNOFilesAsyncPriority_High);
                }
            }
        }
        break;


    case ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO:
        {
            int32 nLevel = tACD.AttributesGetInt(ATTRIBUTE_ALT_LEVEL);
            if (nLevel >= HERO_ALT_LEVEL_MIN && nLevel <= HERO_ALT_LEVEL_MAX)
            {
                ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT));
                const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;
                uint64 nXPToLevel = tExpTableAlt.ptExperienceLevelsAlt[Math::Min(nLevel, (int32)HERO_ALT_LEVEL_REPEAT_INDEX)].nDeltaXP;
                double fOOXPToLevel = 1.0 / nXPToLevel;
                double fOldValue = AttribValueToInt(tOldValue) * fOOXPToLevel;
                double fValue = AttribValueToInt(tValue) * fOOXPToLevel;
                if (fValue < 0.1 && fOldValue > 0.1)
                {

                    SNOName snoName(EFFECTGROUP_SNO, "g_levelUp");
                    Prefetch::AddToQueue(snoName, SNOFilesAsyncPriority_High);
                }
            }
        }
        break;


    case ATTRIBUTE_QUEST_MONSTER:

        if (tValue.nValue)
        {
            CQuestsFlagACD(tACD);
        }
        else
        {
            CQuestsUnFlagACD(tACD);
        }
        break;



    case ATTRIBUTE_CLIENT_EFFECT:
        {
            Player* ptLocalPlayer = LocalPlayerGet();
            if (!ptLocalPlayer)
                break;



            FastAttribKey tLocalKey = MakeAttribKey(ATTRIBUTE_CLIENT_EFFECT, ptLocalPlayer->nPlayerIndex);
            if (!(tKey == tLocalKey))
                break;



            SNO snoEffect = tValue.nValue ? tValue.nValue : tOldValue.nValue;
            if (tValue.nValue == 0)
            {
                tACD.RemoveClientEffect(snoEffect);
            }
            else
            {
                tACD.AddClientEffect(snoEffect, 0);  // literal: ParentType 0, no enumerator in src
            }


        }
        break;
    case ATTRIBUTE_DOOR_LOCKED:
    case ATTRIBUTE_GIZMO_DISABLED_BY_SCRIPT:

        if (tValue.nValue == 0 && (tACD.AttributesGetInt(ATTRIBUTE_QUEST_MONSTER) || QuestIsACDObjective(tACD)))
        {
            CQuestsFlagACD(tACD);
        }
        else if (tValue.nValue)
        {
            CQuestsUnFlagACD(tACD);
        }
        break;
    case ATTRIBUTE_GIZMO_OPERATED:
        CQuestsUnFlagACD(tACD);  // inference: jump-table target +0x1368 is the CQuestsUnFlagACD tail; no line row
        break;
    case ATTRIBUTE_GATE_POSITION:
        {
            float flValue = AttribValueToFloat(tValue);
            if (ptRActor->ptActorAnim)
            {
                AnimationChannelSetPercentComplete(ptRActor->ptActorAnim->tAnimChannel, flValue);
                ptRActor->ptActorAnim->tAnimChannel.SetDirectlyControlled(TRUE);
            }
        }
        break;


    case ATTRIBUTE_USING_BOSSBAR:
        {
            BOOL bUsingBossbar = AttribValueToInt(tValue);
            if (bUsingBossbar)
            {
                UIGame::SetBossHPBar(tACD.id);
            }
            else
            {
                ACDID idACDBossBar = UIGame::GetBossHPBarACD();
                if (idACDBossBar == tACD.id)
                {
                    UIGame::SetBossHPBar(ACDID_INVALID);
                }
            }
        }
        break;









    case ATTRIBUTE_WEAPONS_SHEATHED:
        {
            BOOL fSheathWeapons = AttribValueToInt(tValue);
            if (fSheathWeapons)
            {
                BOOL fWeaponsAlreadySheathed = ptRActor->dwFlags & (1 << RACTOR_WEAPONS_SHEATHED_BIT);
                if (!fWeaponsAlreadySheathed)
                {
                    ptRActor->dwFlags |= (1 << RACTOR_WEAPONS_SHEATHED_BIT);
                }
            }
        }
        break;


    case ATTRIBUTE_WEAPONS_HIDDEN:
        {
            BOOL fSheathWeapons = AttribValueToInt(tValue);
            if (fSheathWeapons)
            {
                BOOL fWeaponsAlreadyHidden = ptRActor->dwFlags & (1 << RACTOR_WEAPONS_HIDDEN_BIT);
                if (!fWeaponsAlreadyHidden)
                {
                    ptRActor->dwFlags |= (1 << RACTOR_WEAPONS_HIDDEN_BIT);
                    VisualInventoryUpdateSheathed(ptRActor->idACD);
                }
            }
            else
            {
                BOOL fWeaponsAlreadyHidden = ptRActor->dwFlags & (1 << RACTOR_WEAPONS_HIDDEN_BIT);
                if (fWeaponsAlreadyHidden)
                {
                    ptRActor->dwFlags &= ~(1 << RACTOR_WEAPONS_HIDDEN_BIT);
                    VisualInventoryUpdateSheathed(ptRActor->idACD);
                }
            }
        }
        break;



    case ATTRIBUTE_MAIN_HAND_WEAPON_HIDDEN:
        {
            BOOL fSheathWeapon = AttribValueToInt(tValue);
            if (fSheathWeapon)
            {
                BOOL fWeaponAlreadyHidden = ptRActor->dwFlagsEx & (1 << RACTOREX_MAIN_HAND_WEAPON_HIDDEN_BIT);
                if (!fWeaponAlreadyHidden)
                {
                    ptRActor->dwFlagsEx |= (1 << RACTOREX_MAIN_HAND_WEAPON_HIDDEN_BIT);
                    VisualInventoryUpdateSheathed(ptRActor->idACD);
                }
            }
            else
            {
                BOOL fWeaponAlreadyHidden = ptRActor->dwFlagsEx & (1 << RACTOREX_MAIN_HAND_WEAPON_HIDDEN_BIT);
                if (fWeaponAlreadyHidden)
                {
                    ptRActor->dwFlagsEx &= ~(1 << RACTOREX_MAIN_HAND_WEAPON_HIDDEN_BIT);
                    VisualInventoryUpdateSheathed(ptRActor->idACD);
                }
            }
        }
        break;



    case ATTRIBUTE_OFF_HAND_WEAPON_HIDDEN:
        {
            BOOL fSheathWeapon = AttribValueToInt(tValue);
            if (fSheathWeapon)
            {
                BOOL fWeaponAlreadyHidden = ptRActor->dwFlagsEx & (1 << RACTOREX_OFF_HAND_WEAPON_HIDDEN_BIT);
                if (!fWeaponAlreadyHidden)
                {
                    ptRActor->dwFlagsEx |= (1 << RACTOREX_OFF_HAND_WEAPON_HIDDEN_BIT);
                    VisualInventoryUpdateSheathed(ptRActor->idACD);
                }
            }
            else
            {
                BOOL fWeaponAlreadyHidden = ptRActor->dwFlagsEx & (1 << RACTOREX_OFF_HAND_WEAPON_HIDDEN_BIT);
                if (fWeaponAlreadyHidden)
                {
                    ptRActor->dwFlagsEx &= ~(1 << RACTOREX_OFF_HAND_WEAPON_HIDDEN_BIT);
                    VisualInventoryUpdateSheathed(ptRActor->idACD);
                }
            }
        }
        break;


































    case ATTRIBUTE_SKILL_OVERRIDE_ACTIVE:

        if (PlayerIsLocal(tACD.id) && ptACDLocalPlayer)
        {


            Player * ptPlayer = PlayerGetByACD(tACD.id);
            CPlayerInputCancelMode(ptPlayer, FALSE);

            CursorMgr & tCursorMgr = PlayerGetCursorMgr();
            BOOL bIsOverridden = AttribValueToInt(tValue);
            tCursorMgr.SetHotBarOverridden(bIsOverridden);

            CPlayerInputOtherListenerReceivedAction(ptPlayer, ACTION_UI_SKILL_1, 30);  // literal delay
            CPlayerInputOtherListenerReceivedAction(ptPlayer, ACTION_UI_SKILL_2, 30);
            CPlayerInputOtherListenerReceivedAction(ptPlayer, ACTION_UI_SKILL_3, 30);
            CPlayerInputOtherListenerReceivedAction(ptPlayer, ACTION_UI_SKILL_4, 30);
            CPlayerInputOtherListenerReceivedAction(ptPlayer, ACTION_UI_RIGHT_MOUSE_BUTTON, 30);
            CPlayerInputOtherListenerReceivedAction(ptPlayer, ACTION_UI_LEFT_MOUSE_BUTTON, 30);
        }
        break;



    case ATTRIBUTE_BUFF_ICON_END_TICK_0:
    case ATTRIBUTE_BUFF_ICON_END_TICK_1:
    case ATTRIBUTE_BUFF_ICON_END_TICK_2:
    case ATTRIBUTE_BUFF_ICON_END_TICK_3:
    case ATTRIBUTE_BUFF_ICON_END_TICK_4:
    case ATTRIBUTE_BUFF_ICON_END_TICK_5:
    case ATTRIBUTE_BUFF_ICON_END_TICK_6:
    case ATTRIBUTE_BUFF_ICON_END_TICK_7:
    case ATTRIBUTE_BUFF_ICON_END_TICK_8:
    case ATTRIBUTE_BUFF_ICON_END_TICK_9:
    case ATTRIBUTE_BUFF_ICON_END_TICK_10:
    case ATTRIBUTE_BUFF_ICON_END_TICK_11:
    case ATTRIBUTE_BUFF_ICON_END_TICK_12:
    case ATTRIBUTE_BUFF_ICON_END_TICK_13:
    case ATTRIBUTE_BUFF_ICON_END_TICK_14:
    case ATTRIBUTE_BUFF_ICON_END_TICK_15:
    case ATTRIBUTE_BUFF_ICON_END_TICK_16:
    case ATTRIBUTE_BUFF_ICON_END_TICK_17:
    case ATTRIBUTE_BUFF_ICON_END_TICK_18:
    case ATTRIBUTE_BUFF_ICON_END_TICK_19:
    case ATTRIBUTE_BUFF_ICON_END_TICK_20:
    case ATTRIBUTE_BUFF_ICON_END_TICK_21:
    case ATTRIBUTE_BUFF_ICON_END_TICK_22:
    case ATTRIBUTE_BUFF_ICON_END_TICK_23:
    case ATTRIBUTE_BUFF_ICON_END_TICK_24:
    case ATTRIBUTE_BUFF_ICON_END_TICK_25:
    case ATTRIBUTE_BUFF_ICON_END_TICK_26:
    case ATTRIBUTE_BUFF_ICON_END_TICK_27:
    case ATTRIBUTE_BUFF_ICON_END_TICK_28:
    case ATTRIBUTE_BUFF_ICON_END_TICK_29:
    case ATTRIBUTE_BUFF_ICON_END_TICK_30:
    case ATTRIBUTE_BUFF_ICON_END_TICK_31:



        if (ACDIsPlayer(tACD.id) || ACDIsHireling(tACD) || tACD.IsPet())
        {
            int32 nBuffIndex;
            Attrib eAttribCount;

            switch (eAttrib)
            {
            case ATTRIBUTE_BUFF_ICON_END_TICK_0:
                nBuffIndex = 0;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_0;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_1:
                nBuffIndex = 1;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_1;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_2:
                nBuffIndex = 2;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_2;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_3:
                nBuffIndex = 3;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_3;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_4:
                nBuffIndex = 4;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_4;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_5:
                nBuffIndex = 5;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_5;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_6:
                nBuffIndex = 6;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_6;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_7:
                nBuffIndex = 7;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_7;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_8:
                nBuffIndex = 8;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_8;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_9:
                nBuffIndex = 9;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_9;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_10:
                nBuffIndex = 10;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_10;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_11:
                nBuffIndex = 11;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_11;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_12:
                nBuffIndex = 12;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_12;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_13:
                nBuffIndex = 13;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_13;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_14:
                nBuffIndex = 14;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_14;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_15:
                nBuffIndex = 15;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_15;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_16:
                nBuffIndex = 16;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_16;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_17:
                nBuffIndex = 17;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_17;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_18:
                nBuffIndex = 18;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_18;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_19:
                nBuffIndex = 19;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_19;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_20:
                nBuffIndex = 20;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_20;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_21:
                nBuffIndex = 21;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_21;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_22:
                nBuffIndex = 22;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_22;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_23:
                nBuffIndex = 23;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_23;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_24:
                nBuffIndex = 24;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_24;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_25:
                nBuffIndex = 25;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_25;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_26:
                nBuffIndex = 26;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_26;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_27:
                nBuffIndex = 27;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_27;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_28:
                nBuffIndex = 28;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_28;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_29:
                nBuffIndex = 29;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_29;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_30:
                nBuffIndex = 30;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_30;
                break;
            case ATTRIBUTE_BUFF_ICON_END_TICK_31:
                nBuffIndex = 31;
                eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_31;
                break;
            }

            SNO snoPower = KeyGetParam(tKey);
            if (PowerBuffIsDisplayed(snoPower, nBuffIndex) ||
                PowerBuffShowsActiveOnSkillButton(snoPower, nBuffIndex) ||
                PowerBuffShowsDurationOnSkillButton(snoPower, nBuffIndex))
            {
                UIBuffs::HandleBuffEndTickAttributeChange(tValue, eAttribCount, snoPower, nBuffIndex, tACD);
            }
        }
        break;

    case ATTRIBUTE_BUFF_ICON_COUNT_0:
    case ATTRIBUTE_BUFF_ICON_COUNT_1:
    case ATTRIBUTE_BUFF_ICON_COUNT_2:
    case ATTRIBUTE_BUFF_ICON_COUNT_3:
    case ATTRIBUTE_BUFF_ICON_COUNT_4:
    case ATTRIBUTE_BUFF_ICON_COUNT_5:
    case ATTRIBUTE_BUFF_ICON_COUNT_6:
    case ATTRIBUTE_BUFF_ICON_COUNT_7:
    case ATTRIBUTE_BUFF_ICON_COUNT_8:
    case ATTRIBUTE_BUFF_ICON_COUNT_9:
    case ATTRIBUTE_BUFF_ICON_COUNT_10:
    case ATTRIBUTE_BUFF_ICON_COUNT_11:
    case ATTRIBUTE_BUFF_ICON_COUNT_12:
    case ATTRIBUTE_BUFF_ICON_COUNT_13:
    case ATTRIBUTE_BUFF_ICON_COUNT_14:
    case ATTRIBUTE_BUFF_ICON_COUNT_15:
    case ATTRIBUTE_BUFF_ICON_COUNT_16:
    case ATTRIBUTE_BUFF_ICON_COUNT_17:
    case ATTRIBUTE_BUFF_ICON_COUNT_18:
    case ATTRIBUTE_BUFF_ICON_COUNT_19:
    case ATTRIBUTE_BUFF_ICON_COUNT_20:
    case ATTRIBUTE_BUFF_ICON_COUNT_21:
    case ATTRIBUTE_BUFF_ICON_COUNT_22:
    case ATTRIBUTE_BUFF_ICON_COUNT_23:
    case ATTRIBUTE_BUFF_ICON_COUNT_24:
    case ATTRIBUTE_BUFF_ICON_COUNT_25:
    case ATTRIBUTE_BUFF_ICON_COUNT_26:
    case ATTRIBUTE_BUFF_ICON_COUNT_27:
    case ATTRIBUTE_BUFF_ICON_COUNT_28:
    case ATTRIBUTE_BUFF_ICON_COUNT_29:
    case ATTRIBUTE_BUFF_ICON_COUNT_30:
    case ATTRIBUTE_BUFF_ICON_COUNT_31:




        if (ACDIsPlayer(tACD.id) || ACDIsHireling(tACD) || tACD.IsPet())
        {
            int32 nBuffIndex = 0;
            switch (eAttrib)
            {
            case ATTRIBUTE_BUFF_ICON_COUNT_1:
                nBuffIndex = 1;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_2:
                nBuffIndex = 2;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_3:
                nBuffIndex = 3;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_4:
                nBuffIndex = 4;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_5:
                nBuffIndex = 5;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_6:
                nBuffIndex = 6;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_7:
                nBuffIndex = 7;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_8:
                nBuffIndex = 8;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_9:
                nBuffIndex = 9;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_10:
                nBuffIndex = 10;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_11:
                nBuffIndex = 11;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_12:
                nBuffIndex = 12;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_13:
                nBuffIndex = 13;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_14:
                nBuffIndex = 14;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_15:
                nBuffIndex = 15;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_16:
                nBuffIndex = 16;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_17:
                nBuffIndex = 17;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_18:
                nBuffIndex = 18;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_19:
                nBuffIndex = 19;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_21:
                nBuffIndex = 21;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_22:
                nBuffIndex = 22;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_23:
                nBuffIndex = 23;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_24:
                nBuffIndex = 24;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_25:
                nBuffIndex = 25;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_26:
                nBuffIndex = 26;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_27:
                nBuffIndex = 27;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_28:
                nBuffIndex = 28;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_29:
                nBuffIndex = 29;
                break;
            case ATTRIBUTE_BUFF_ICON_COUNT_31:
                nBuffIndex = 31;
                break;
            }





            SNO snoPower = KeyGetParam(tKey);
            if (PowerBuffIsDisplayed(snoPower, nBuffIndex) ||
                PowerBuffShowsActiveOnSkillButton(snoPower, nBuffIndex) ||
                PowerBuffMergesTooltip(snoPower, nBuffIndex) ||
                PowerBuffShowsDurationOnSkillButton(snoPower, nBuffIndex))
            {
                UIBuffs::HandleBuffCountAttributeChange(tValue, snoPower, nBuffIndex, tACD);
            }

            if (UIMinimap::CanShowBuff(snoPower, nBuffIndex))
            {
                UIBuffs::HandleBuffCountAttributeChange(tValue, snoPower, nBuffIndex, tACD);
                UIMinimap::HandleBuffCountAttributeChange(tValue, snoPower, nBuffIndex, tACD);
            }
        }
        break;




    case ATTRIBUTE_FREE_CAST:

        if (PlayerIsLocal(tACD.id))
        {
            SNO snoPower = KeyGetParam(tKey);
            int32 nOldValue = AttribValueToInt(tOldValue);
            int32 nValue = AttribValueToInt(tValue);
            if (nOldValue <= 0 && nValue > 0)
            {
                sApplyFreeCastEffects(snoPower, PlayerGetByACD(tACD.id));
            }
        }
        break;



    case ATTRIBUTE_OPERATABLE_STORY_GIZMO:
        {
            BOOL fShowEffect = AttribValueToInt(tValue);
            if (fShowEffect)
            {
                CQuestsFlagACD(tACD);
            }
            else
            {
                CQuestsUnFlagACD(tACD);
            }
        }
        break;


    case ATTRIBUTE_WEAPON_EFFECT_OVERRIDE:
        {
            VisualInventory * ptVisualInventory = VisualInventoryGet(tACD.id);
            if (ptVisualInventory)
            {
                VisualItem & tVisualItemLeftHand = ptVisualInventory->tEquipmentLast.tVisualItem[VIS_LEFT_HAND];
                if (tValue.nValue > ITEM_EFFECT_TYPE_NONE && tValue.nValue < NUM_ITEM_EFFECT_TYPES)
                {
                    tVisualItemLeftHand.eItemEffectType = tValue.nValue;
                }
                else
                {
                    tVisualItemLeftHand.eItemEffectType = tOldValue.nValue;
                }
            }
            ACDUpdateAppearance(tACD.id);
        }
        break;


    case ATTRIBUTE_RACTOR_FADE_GROUP:
        {
            int32 nFadeGroup = AttribValueToInt(tValue);
            ptRActor->nFadeGroupSubObjectIndex = 0;
            if (nFadeGroup)
            {
                ptRActor->nFadeGroupSubObjectIndex = SceneGetASubObjectIndexFromFadeGroup(ptRActor->idSScene, nFadeGroup);
            }
        }
        break;











    case ATTRIBUTE_LEVEL:
        {
            Player * ptPlayer = PlayerGetByACD(tACD.id);
            if (ptPlayer && PlayerIsLocal(ptPlayer))
            {
                UISkills::Console::OnPlayerLeveled(ptPlayer);
            }
        }

        {
            HirelingClass eClass = HirelingGetClass(tACD);
            if (eClass)  // inference: cbz; HIRELING_NONE is 0
            {
                int32 nLevel = AttribValueToInt(tValue);
                CHirelingSetAppearance(ptRActor, eClass, nLevel);

                UIFollower::Console::OnFollowerLeveled(tACD, nLevel);
            }
        }
        break;



    case ATTRIBUTE_PLAYER_WEAPONCLASS_ANIM_OVERRIDE:

        if (ACDIsPlayer(tACD.id))
            AnimPlayerWeaponSetChanged(tACD);
        break;

    case ATTRIBUTE_CURRENT_WEAPONCLASS:
        AnimPlayerWeaponSetChanged(tACD);  // inference: jump-table target +0x1b54 is the AnimPlayerWeaponSetChanged tail; no line row
        break;


    case ATTRIBUTE_HIRELING_CLASS:
        {
            HirelingClass eClass = (HirelingClass)AttribValueToInt(tValue);
            CHirelingUpdateAppearance(tACD, ptRActor, eClass);
        }
        break;











    case ATTRIBUTE_LOOPING_ANIMATION_SUPPRESS_ITEM_TOOLTIPS:

        if (PlayerIsLocal(tACD.id))
        {
            gtTooltipMgr.HideTips();
        }
        break;




    case ATTRIBUTE_HITPOINTS_CUR:

        tACD.flHitPointsCurrent = tValue.flValue;
        tACD.fIsAlive = (tValue.flValue > 0.0f);
        break;




    case ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL:

        if (tValue.nValue >= 0 && PlayerIsLocal(tACD.id))
        {
            gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_TIERED_LOOT_RUN_INVENTORY_LOCKED), NULL, TRUE);
        }
        break;




    case ATTRIBUTE_JEWEL_UPGRADES_MAX:

        UIJewelUpgrade::JewelUpgradeMaxChanged();
        break;



    case ATTRIBUTE_ALPHA_FOR_OTHER_PLAYERS:
        {
            ACDNetworkName idANNOwningPlayer = (ACDNetworkName)tValue.nValue;
            if (idANNOwningPlayer != ANN_INVALID)
            {
                ACDID idACDOwningPlayer = ACDFromANN(idANNOwningPlayer);
                if (idACDOwningPlayer == ACDID_INVALID || !PlayerIsLocal(idACDOwningPlayer))
                {
                    ptRActor->dwFlagsEx |= (1 << RACTOREX_USE_REMOTE_PET_ALPHA);
                }
            }
        }
        break;


    case ATTRIBUTE_HQ_HOT_COLD_STATE:

        sUpdateHQHotColdIcon(ptRActor, tKey, tValue.nValue);
        break;



    case ATTRIBUTE_SET_ITEM_COUNT:
        {
            int32 nCount = tValue.nValue;

            if (nCount >= 2)
            {
                nCount += tACD.AttributesGetInt(ATTRIBUTE_SET_ITEM_DISCOUNT);


                if (nCount >= 4)
                {
                    GameplayPrefs & tGameplayPrefs = Prefs::GetMutableGameplayPrefs();
                    if (!tGameplayPrefs.GetSkillElectiveMode() && !Tutorial::PlayerHasSeen(*PlayerGetByACD(tACD.id), GlobalSNOGet(GLOBALSNO_TUTORIAL_ELECTIVE_MODE_P6)))
                    {
                        tGameplayPrefs.SetUseElectiveMode(TRUE);

                        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_ELECTIVE_MODE_P6), NULL, TRUE);
                    }
                }
            }
        }
        break;
    }





}





























































































































































































































































static void sRActorSetLifeForFade(RActor * ptRActor, const BOOL bRecurseForAttachments)
{


    if (!ptRActor)
        return;
    ptRActor->flLife = ptRActor->flTimeUntilFade;

    if (bRecurseForAttachments)
    {
        POS pos = ptRActor->tAttachmentList.GetHeadPosition();
        while (pos)
        {
            AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
            Attachment & tAttachment = *AttachmentGet(idAttachment);

            if (tAttachment.snoname.snoGroup == ACTOR_SNO)
            {
                if (RActorTryToGet((RActorID)tAttachment.idAttachedObject))
                {


                    sRActorSetLifeForFade(RActorTryToGet((RActorID)tAttachment.idAttachedObject), bRecurseForAttachments);
                }
            }
        }
    }

}




static void sRActorStartCorpseFadeTimer(RActor * ptRActor, const ActorCommonData * ptACD)
{


    if (!ptACD)
    {
        ptACD = ACDTryToGet(ptRActor->idACD);
        if (!ptACD)
            return;
    }




    if (ACDIsPlayer(ptACD->id))
    {
        sRActorSetLifeForFade(ptRActor, TRUE);
    }
    else if (!ACDIsHireling(*ptACD))
    {



        int32 nScriptedFadeTime = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_SCRIPTED_FADE_TIME));
        if (nScriptedFadeTime > 0)
        {
            ptRActor->flTimeUntilFade = 0.0f;
            ptRActor->flFadeTime = nScriptedFadeTime * (1.0f / 60.0f);
        }

        ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

        BOOL fUsesCorpseLimits = (ptRActor->flTimeUntilFade > 0.0f) && ptACD->IsMonster() &&



                                 TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_RESPECTS_MONSTER_CORPSE_LIMIT);





        BOOL fRemovesCorpseOnDead = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_REMOVES_CORPSE_ON_DEAD);

        if (fUsesCorpseLimits)
        {
            RActorRegisterCorpse(ptRActor, ptACD->IsMonster());
        }
        else if (fRemovesCorpseOnDead)
        {
            BOOL bAlsoFadeAttachments = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_ALSO_FADE_ATTACHMENTS_ON_DEAD);
            sRActorSetLifeForFade(ptRActor, bAlsoFadeAttachments);
        }
    }
    else if (ptRActor->flTimeUntilFade == 0.0f)
    {
        ptRActor->dwFlags |= (1 << RACTOR_HIDE_BIT);
    }
}




void RActorPlayHitFlash(RActor * ptRActor, const RGBAColor & rgbaHitFlashAdd, float flDurationAdd, const RGBAColor & rgbaHitFlashMul, float flDurationMul, const BOOL fJitter)
{





    ptRActor->flHitFlashAddDuration = flDurationAdd;
    ptRActor->flHitFlashAddTimeRemaining = flDurationAdd;

    { RGBAColorValue tVal; tVal.Set(rgbaHitFlashAdd); ptRActor->rgbavalHitFlashAdd = tVal; }  // tVal: stand-in for the RGBAColorValue(const RGBAColor &) ctor (DWARF RGBAColor.h:125) our header lacks

    ptRActor->flHitFlashMulDuration = flDurationMul;
    ptRActor->flHitFlashMulTimeRemaining = flDurationMul;
    { RGBAColorValue tVal; tVal.Set(rgbaHitFlashMul); ptRActor->rgbavalHitFlashMul = tVal; }

    if (fJitter && !PlayerIsLocal(ptRActor->id) &&
        !ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NEVER_PLAY_GETHIT_JITTER, ptRActor->snoActor))
    {

        ptRActor->tHitJitter.Set(0.5f);
    }


    POS pos = ptRActor->tAttachmentList.GetHeadPosition();
    while (pos)
    {
        AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
        Attachment & tAttachment = *AttachmentGet(idAttachment);

        if (tAttachment.snoname.snoGroup == ACTOR_SNO)
        {
            RActorID idRActorAttached = (RActorID)tAttachment.idAttachedObject;
            RActor * ptRActorAttached = RActorTryToGet(idRActorAttached);


            if (ptRActorAttached)
            {
                RActorPlayHitFlash(ptRActorAttached, rgbaHitFlashAdd, flDurationAdd, rgbaHitFlashMul, flDurationMul, fJitter);
            }
        }
    }
}
















































































































void RActorPlayHitEffectOverrideFromMessage(RActor * ptRActor, const ACDID idACDAttacker, const SNO snoEffectGroup)



{

    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    ACDID idACDLocalPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(idACDLocalPlayer);

    BOOL bAttackerIsOtherFriendlyPlayer = ptACDAttacker && ptACDLocalPlayer && ACDIsPlayer(idACDAttacker) && !PlayerIsLocal(idACDAttacker) && ACDIsFriendly(*ptACDAttacker, *ptACDLocalPlayer, FALSE);






    if (!bAttackerIsOtherFriendlyPlayer)
    {
        if (snoEffectGroup != SNO_NONE)

            EffectGroupCreateUntracked(snoEffectGroup, ptRActor->id, 0, FALSE, FALSE);
    }

}






































































































































































































































































































































































































































































































































































































































































































































































void RActorProjectileStick(RActor * ptRActor, const SNO snoActorToStick, const Vector3D & wpSourceLoc)
{



    const int32 MAX_LIKE_PROJECTILE_STICK_EVENTS = 2;









    int32 nCount = 0;
    const XPooledDataIDList & listAttachments = ptRActor->tAttachmentList;
    POS posAttachment = listAttachments.GetHeadPosition();
    while (posAttachment)
    {

        AttachmentID idAttachment = listAttachments.GetNext(posAttachment);
        const Attachment & tAttachment = *AttachmentGet(idAttachment);


        if (tAttachment.snoname.snoGroup == ACTOR_SNO &&
            tAttachment.snoname.snoHandle == snoActorToStick)
        {
            nCount++;
        }
    }












    if (nCount > MAX_LIKE_PROJECTILE_STICK_EVENTS)
        return;















    Hardpoint tDynamicHardpoint;
    if (!RActorHardpointCreateCentralFacing(tDynamicHardpoint, ptRActor->id, wpSourceLoc, FALSE))
        return;











    {
        float flLifetime = -1.0f;
        TriggerEvent tTriggerEvent;
        TriggerEventInit(tTriggerEvent);
        tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
        tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_ACTOR;
        tTriggerEvent.fIgnoreOwnerScale = TRUE;
        tTriggerEvent.snoname.Set(ACTOR_SNO, snoActorToStick);

        RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, &tDynamicHardpoint, 1, flLifetime, FALSE, 0.0f, 0);
    }


}







































































































void RActorSetLastSeenTime(Frustum & tFrustum)
{
    uint32 dwNow = GameGetServerTime();


    RActorIDListPooled & listRActors = RActorGetMainList();
    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor && Intersect::FrustumAndSphere(tFrustum, ptRActor->tLocation.tWorldLocation.wsBounds))
        {

            ptRActor->dwRActorLastSeen = dwNow;
        }
    }
}









void RActorUnlinkFromMarker(RActor * ptRActor)
{


    ptRActor->tMarkerOwner.Initialize();
    ptRActor->idSScene = SSCENEID_INVALID;
}














































































































































































void RActorOnSceneSwap(SWorldID idSWorld, SSceneID idSSceneOld, SSceneID idSSceneNew)
{







    CGame & tCGame = CGameGet();
    if (tCGame.ptRActors)
    {
        TDataArrayIterator<RActor> tIt;
        while (RActor * ptRActor = tCGame.ptRActors->Iterate(tIt))
        {
            if (ptRActor->idSScene != idSSceneOld)
            {

                RActorUpdateClientTransform(ptRActor, NULL, NULL);
            }
        }
    }
}






static void sRActorSetACD(RActor * ptRActor, const ACDID idACD, const ActorDefinition * pdefActor)
{






    if (ptRActor->idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(ptRActor->idACD);
        tACD.SetOwner(DATAID_INVALID);
    }

    ptRActor->idACD = idACD;


    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(idACD);
        tACD.SetOwner(ptRActor->id);
    }


    RActorUpdateOcclusionManager(ptRActor);
}


























// Declared here: defined in X/Client/Scene.cpp:4578 and :4677; no src header declares them.
BOOL CScenesGetPositionColor(SWorldID idSWorld, const Vector3D & wp, RGBAColor & rgbaColor);
BOOL CSceneGetPositionColor(SSceneID idSScene, const Vector3D & wp, RGBAColor & rgbaColor);


// The original has no return statement (no row). Falling off the end makes clang mark this static noreturn and delete
// sRActorCreate's tail; with this invented return the unused result is dropped (callers ignore it) and the bytes match.
static BOOL sGetSceneVertColor(
    RActor * ptRActor,
    RGBAColorValue & rgbaval,
    BOOL fAssignCurrentAlso)
{

    const BOOL bMeshLit = ptRActor->dwFlags & (1 << RACTOR_MESH_LIT_BIT);










    const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;


    if (bMeshLit)
    {
        RGBAColor rgbaColor;
        if (loc.idSScene != SSCENEID_INVALID)
        {
            if (CSceneGetPositionColor(loc.idSScene, loc.transform.wp, rgbaColor))
            {

                if (rgbaColor.r + rgbaColor.g + rgbaColor.b == 0)
                {
                    CScenesGetPositionColor(loc.idSWorld, loc.transform.wp, rgbaColor);
                }
                rgbaval.Set(rgbaColor);
            }
        }
        else
        {





            if (CScenesGetPositionColor(loc.idSWorld, loc.transform.wp, rgbaColor))
            {
                rgbaval.Set(rgbaColor);
            }
        }
    }
    return FALSE;
}




































































































































































































void RActorResetDeathFadeTimes(RActor * ptRActor)
{


    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_FADE_CORPSE_ON_DEAD))
    {
        ptRActor->flTimeUntilFade = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_FADE_CORPSE_TIME_UNTIL_FADE);
        ptRActor->flFadeTime = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_FADE_CORPSE_FADE_TIME);
    }
    else
    {
        ptRActor->flTimeUntilFade = 0.0f;
        ptRActor->flFadeTime = 0.0f;
    }














}
// DWARF RActors.cpp:11250 (line 11251 holds RActorResetDeathFadeTimes' closing brace).
static void sRActorOverlayReset(
    RActor & tRActor)
{
    tRActor.eOverlayType = 0;
    tRActor.dwOverlayFadeTimeCurrent = 0;
    tRActor.dwOverlayFadeTimeOriginal = 0;
}

static void sRActorApplyOverlay(RActor * ptRActor, const OverlayType eOverlayType, const uint32 dwFadeTime)
{



    ptRActor->dwOverlayFadeTimeOriginal = dwFadeTime;
    ptRActor->dwOverlayFadeTimeCurrent = dwFadeTime;
    ptRActor->eOverlayType = eOverlayType;
}



void RActorApplyOverlay(const RActorID idRActor, const OverlayType eOverlayType)
{


    ShaderTex eShaderTexToUse = OverlayTypeToShaderTex(eOverlayType);
    if (eShaderTexToUse == SHADERTEX_NONE)
        return;



    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;




    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    uint32 dwFadeTime;
    switch (eOverlayType)
    {


    case OVERLAYTYPE_CRITICAL_HIT_BLOOD:
        dwFadeTime = pdefAppearance->dwFadeOutTimeCriticalHitBlood;
        break;
    case OVERLAYTYPE_STONED:
        dwFadeTime = pdefAppearance->dwFadeOutTimeStoned;
        break;
    case OVERLAYTYPE_CHILLED:
        dwFadeTime = pdefAppearance->dwFadeOutTimeChilled;
        break;
    case OVERLAYTYPE_FROZEN:
        dwFadeTime = pdefAppearance->dwFadeOutTimeFrozen;
        break;
    case OVERLAYTYPE_POISONED:
        dwFadeTime = pdefAppearance->dwFadeOutTimePoisoned;
        break;
    default:
        dwFadeTime = INFINITE_OVERLAY_FADE_TIME;
        break;
    }



    sRActorApplyOverlay(ptRActor, eOverlayType, dwFadeTime);

    POS pos = ptRActor->tAttachmentList.GetHeadPosition();
    while (pos)
    {
        AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);


        Attachment & tAttachment = *AttachmentGet(idAttachment);
        if (tAttachment.snoname.snoGroup == ACTOR_SNO)
        {
            RActorID idRActorAttached = (RActorID)tAttachment.idAttachedObject;
            RActor * ptRActorAttached = RActorTryToGet(idRActorAttached);
            if (ptRActorAttached)
            {

                sRActorApplyOverlay(ptRActorAttached, eOverlayType, dwFadeTime);
            }
        }
    }
}
void RActorDetach(const RActorID idRActor)
{

    RActor * ptRActor = RActorGet(idRActor);
    ptRActor->tParentData.Init();

    if (ptRActor->ptActorAnim && ptRActor->ptActorAnim->ptRagdoll &&
        ptRActor->ptActorAnim->ptRagdoll->GetType() == ATTACHED_RAGDOLL_TYPE)
    {

        sRActorStartCorpseFadeTimer(ptRActor, NULL);
        AttachedRagdoll * ptAttachedRagdoll = (AttachedRagdoll *)ptRActor->ptActorAnim->ptRagdoll;
        ptAttachedRagdoll->Detach();
    }




}




RagdollType RActorGetRagdollType(const RActor * ptRActor)
{


    if (ptRActor->ptActorAnim && ptRActor->ptActorAnim->ptRagdoll)
    {




        return ptRActor->ptActorAnim->ptRagdoll->GetType();
    }
    return INVALID_RAGDOLL_TYPE;
}

































































































void RActorFlagForDeletion(RActor * ptRActor)
{


    ptRActor->dwFlags |= (1 << RACTOR_DELETE_THIS_TICK_BIT);
}

































































void RActorHighlight(RActor & tRActor, const BOOL fHighlight)
{
    if (AppDrawFlagIsSet(APP_DISABLE_RACTOR_HIGHLIGHT_BIT))
        return;

    if (((tRActor.dwFlags >> RACTOR_HIGHLIGHT_BIT) & 1) == fHighlight)
        return;

    if (fHighlight)
        tRActor.dwFlags |= (1 << RACTOR_HIGHLIGHT_BIT);
    else
        tRActor.dwFlags &= ~(1 << RACTOR_HIGHLIGHT_BIT);

    RActorHandleOnHighlight(tRActor, fHighlight);

    tRActor.snoIrradianceTexHighlight = SNO_NONE;
    if (fHighlight && !RActorIsNecroCorpse(&tRActor))
    {
        BOOL fIsHostile = RActorIsHostile(&tRActor);
        tRActor.snoIrradianceTexHighlight = GlobalSNOGet(fIsHostile ? GLOBALSNO_SELECTION_HOSTILE_IRRADIANCE_TEXTURE : GLOBALSNO_SELECTION_IRRADIANCE_TEXTURE);
    }






    ActorCommonData * ptACDActor = ACDTryToGet(tRActor.idACD);
    if (!fHighlight && ptACDActor)
    {
        SNO snoEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_INTERACT_ICON);
        ptACDActor->RemoveClientEffect(snoEffect);
    }
}







SSceneID RActorGetSSceneID(const RActorID idRActor)
{

    SSceneID idSScene = SSCENEID_INVALID;

    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {

        idSScene = ptRActor->idSScene;
    }

    return idSScene;
}

SNO RActorGetLevelArea(const RActorID idRActor)
{
    SNO snoLevelArea = SNO_NONE;

    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor && ptRActor->tLocation.eActorLocation == AL_WORLD)
    {
        WorldPlace tPlace;
        NavMeshHandle tNavMeshHandle;
        tPlace.idSWorld = ptRActor->tLocation.tWorldLocation.idSWorld;
        tPlace.wp = ptRActor->wpWalkingPos;
        ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
        if (!NavMeshHandleIsNone(tNavMeshHandle))
            snoLevelArea = NavMeshHandleGetLevelArea(tNavMeshHandle);

        SNO snoWorld = WorldCommonGetWorldSNO(tPlace.idSWorld);
        if (WorldIsDungeonFinderPlaceHolder(snoWorld) && WorldGetDungeonFinderLevel(snoWorld) != -1)
        {
            snoLevelArea = WorldGetDungeonFinderLevelArea(snoWorld);
        }
    }

    return snoLevelArea;
}












int32 RActorGetCurrentWaypoint(const RActorID idRActor)
{

    SNO snoLevelArea = RActorGetLevelArea(idRActor);

    return ActGetWaypointByLevelArea(snoLevelArea, FALSE);
}














































































SNO RActorGetInheritedTagMapValueSNO(const RActor * ptRActor, const uint32 dwRActorTag, const uint32 dwMarkerTag)
{



    if (ptRActor->idMarker != MARKERID_INVALID)
    {
        GameCommonData & tGCD = CGameGetGameCommonData();
        const MarkerInstance tMarkerInstance = MarkerInstanceGet(tGCD, ptRActor->idMarker);


        ScopedMarkerSetDefinitionLockR ptMarkerSetDef(tMarkerInstance.snoMarkerSet);
        const Marker & tMarker = ptMarkerSetDef->tMarkerSet[tMarkerInstance.nMarkerIndex];

        if (TagMapTagExists(tMarker.hTagMap, dwMarkerTag))

            return TagMapGetSNO(tMarker.hTagMap, dwMarkerTag);

    }

    ScopedActorDefinitionLockR pdefRActor(ptRActor->snoActor);
    return TagMapGetSNO(pdefRActor->hTagMap, dwRActorTag);
}

GBID RActorGetInheritedTagMapValueGBID(const RActor * ptRActor, const uint32 dwRActorTag, const uint32 dwMarkerTag)
{


    GameCommonData & tGCD = CGameGetGameCommonData();
    const MarkerInstance tMarkerInstance = MarkerInstanceGet(tGCD, ptRActor->idMarker);


    ScopedMarkerSetDefinitionLockR ptMarkerSetDef(tMarkerInstance.snoMarkerSet);
    const Marker & tMarker = ptMarkerSetDef->tMarkerSet[tMarkerInstance.nMarkerIndex];

    if (TagMapTagExists(tMarker.hTagMap, dwMarkerTag))

        return TagMapGetGBID(tMarker.hTagMap, dwMarkerTag);


    ScopedActorDefinitionLockR pdefRActor(ptRActor->snoActor);
    return TagMapGetGBID(pdefRActor->hTagMap, dwRActorTag);
}




inline BOOL RActorIsEnvRActor(RActor & tRActor)
{

    GameCommonData & tGCD = CGameGetGameCommonData();
    GameMode eGameMode = tGCD.GameModeGet();
    if (eGameMode == GAMEMODE_HOLODECK)
    {
        return FALSE;
    }

    return ActorIsEnvActor(tRActor.snoActor);
}












RActorIDListPooled & RActorGetMainList()
{



    CGame & tCGame = CGameGet();
    return tCGame.ptRActorGlobals->listMainRActors;
}















BOOL RActorIsHostile(const RActor * ptRActor)
{
    RActorID idRActorTopmost = RActorGetTopmostOwner(ptRActor->id, TRUE, FALSE);
    if (idRActorTopmost != RACTORID_INVALID)
    {
        const RActor * ptRActorTopMost = RActorTryToGet(idRActorTopmost);
        ptRActor = ptRActorTopMost;
    }
    if (!ptRActor)
        return FALSE;

    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (!ptACD)
        return FALSE;



    if (ptACD->IsGizmo() && ptACD->IsTargetable() && GizmoIsWreckable(ptACD->GetGizmoType()))
        return TRUE;




    if (ptACD->IsNecroCorpse() || ptACD->IsDueling())
        return TRUE;














    {
        ActorCommonData * ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet());
        if (ptACDLocalPlayer)
        {

            return ACDIsHostile(*ptACDLocalPlayer, *ptACD, FALSE);
        }
    }

    return FALSE;
}




BOOL RActorIsNecroCorpse(const RActor * ptRActor)
{
    RActorID idRActorTopmost = RActorGetTopmostOwner(ptRActor->id, TRUE, FALSE);
    if (idRActorTopmost != RACTORID_INVALID)
    {
        const RActor * ptRActorTopMost = RActorTryToGet(idRActorTopmost);
        ptRActor = ptRActorTopMost;
    }
    if (!ptRActor)
        return FALSE;



    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD)
        return ptACD->IsNecroCorpse();

    return FALSE;
}










void RActorSetFadeTimes(RActor * ptRActor, const float flTimeUntilFade, const float flFadeTime, const BOOL bRecurseForAttachments)
{
    if (!ptRActor)
        return;

    ptRActor->flTimeUntilFade = flTimeUntilFade;
    ptRActor->flFadeTime = flFadeTime;

    if (bRecurseForAttachments)
    {
        POS pos = ptRActor->tAttachmentList.GetHeadPosition();
        while (pos)
        {
            Attachment * ptAttachment = AttachmentGet(ptRActor->tAttachmentList.GetNext(pos));
            if (ptAttachment->snoname.snoGroup == ACTOR_SNO)
            {
                RActor * ptRActorAttached = RActorTryToGet((RActorID)ptAttachment->idAttachedObject);
                if (ptRActorAttached)
                {
                    ptRActorAttached->flLife = ptRActor->flLife;
                    RActorSetFadeTimes(ptRActorAttached, flTimeUntilFade, flFadeTime, bRecurseForAttachments);
                }
            }
        }
    }
}





















































































































































































































BOOL RActorIsHidden(const RActor * ptRActor)
{


    return (ptRActor->dwFlags & ((1 << RACTOR_HIDE_BIT) | (1 << RACTOR_NEVER_DRAW_BIT) | (1 << RACTOR_HIDE_CLIENT_SIDE))) ||
           (ptRActor->dwFlags & (1 << RACTOR_COMPLETELY_STEALTHED_BIT));
}





















































































BOOL RActorIsVisibleToLocalPlayer(const RActor * ptRActor)
{
    if (!ptRActor)
        return TRUE;







    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet());
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACDLocalPlayer && ptACD && !ACDIsFriendly(*ptACD, *ptACDLocalPlayer, FALSE))
    {









        if (ptACD->IsStealthed() || RActorIsHidden(ptRActor) || (ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT)))
            return FALSE;
    }

    return TRUE;
}







BOOL RActorCanHaveChatBubble(const RActor * ptRActor)
{


    if (RActorIsHidden(ptRActor))
        return FALSE;




    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    return !TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_NEVER_USES_CHAT_BUBBLES);





}


void RActorSettleCloth(RActor * ptRActor)
{


    RActorCollisionCapsulesUpdate(ptRActor);

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    for (int32 nGeoSetType = 0; nGeoSetType < NUM_GEOSETS; nGeoSetType++)
    {
        const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[nGeoSetType];
        RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[nGeoSetType];

        for (int32 i = 0; i < tGeoSet.nSubObjectCount; i++)
        {
            ClothID idCloth = tGeoSetData.ptSubObjectState[i].idCloth;
            if (idCloth != CLOTHID_INVALID)
            {
                Cloth * ptCloth = ClothManager::TryToGet(idCloth);
                if (ptCloth)
                {
                    ptCloth->Settle();
                }
            }
        }
    }
}




void RActorsUpdateFixedUpdateFlag()
{


    CGame & tCGame = CGameGet();
    TDataArrayIterator<RActor> tIt;
    while (RActor * ptRActor = tCGame.ptRActors->Iterate(tIt))
    {
        if (sIsTopmostParentInUI(ptRActor) == FALSE)
            ptRActor->dwFlagsEx &= ~(1 << RACTOREX_FIXED_UPDATE_BIT);
        else
            ptRActor->dwFlagsEx |= (1 << RACTOREX_FIXED_UPDATE_BIT);
    }
    ParticlesUpdateFixedUpdateFlag();
    RopesUpdateFixedUpdateFlag();
}







void RActorSetIrradianceMap(RActor * ptRActor, const SNO snoIrradianceTex)
{


    ptRActor->snoIrradianceTex = snoIrradianceTex;
    if (snoIrradianceTex == SNO_NONE)
        return;



    POS pos = ptRActor->tAttachmentList.GetHeadPosition();
    while (pos)
    {
        Attachment * ptAttachment = AttachmentGet(ptRActor->tAttachmentList.GetNext(pos));


        if (ptAttachment->snoname.snoGroup == ACTOR_SNO)
        {

            RActor * ptRActorAttached = RActorTryToGet((RActorID)ptAttachment->idAttachedObject);
            if (ptRActorAttached)
            {


                ptRActorAttached->snoIrradianceTex = snoIrradianceTex;
            }
        }
    }
}

#include "../CGame.h"

BOOL PlayersAreHostileToEachOther(const Player * ptPlayer0, const Player * ptPlayer1);  // Player.cpp:5188; DWARF records no header declaration.

#line 5844
float RActorGetAlpha(
    const RActor * ptRActorMain)
{
    float flAlpha = 1.0f;
#line 5850
    if (RActorIsHidden(ptRActorMain))
        return 0.0f;
#line 5856
    ActorCommonData * ptACD = ACDTryToGet(ptRActorMain->idACD);
    if (ptACD && ptACD->IsGizmo())
    {
        GizmoType eGizmoType = ptACD->GetGizmoType();
        if (eGizmoType == ACTORGIZMO_BANNER)
        {
#line 5865
            if (!(sgtAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_ALL_BANNER_LOCS_BIT)))
            {
                int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, ptACD->snoActor);
                Player * ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);
                Player * ptLocalPlayer = LocalPlayerGetPrimary();
#line 5872
                if (PlayersAreHostileToEachOther(ptPlayerDest, ptLocalPlayer) || !ptPlayerDest)
                    return 0.0f;
#line 5878
                if (ptPlayerDest->tClientInfo.ann == ANN_INVALID)
                    return 0.0f;
            }
        }
    }
#line 5891
    if (CGameInScriptedSequence() && ptACD)
    {
#line 5895
        if (ptACD->IsItem() && ptACD->GetLocation().eActorLocation == AL_WORLD)
            return 0.0f;
    }
#line 5902
    flAlpha = ptRActorMain->flAlpha;
#line 5912
    if (ptRActorMain->dwFlags & (1 << RACTOR_STEALTHED_BIT))
    {
        flAlpha *= 0.1f;
    }
    if (ptRActorMain->dwFlags & (1 << RACTOR_DONT_GET_ALPHA_FROM_PARENT_BIT))
        return flAlpha;
#line 5956
    const float NON_LOCAL_PET_ALPHA_MULTIPLIER = 0.25f;
    if (ptRActorMain->dwFlagsEx & (1 << RACTOREX_USE_REMOTE_PET_ALPHA))
    {
        flAlpha *= NON_LOCAL_PET_ALPHA_MULTIPLIER;
    }
    else if (ptACD)
    {
        ACDNetworkName idANNOwningPlayer = ACDNetworkName(ptACD->AttributesGetInt(ATTRIBUTE_ALPHA_FOR_OTHER_PLAYERS));
        if (idANNOwningPlayer != ANN_INVALID)
        {
            ACDID idACDOwningPlayer = ACDFromANN(idANNOwningPlayer);
            if (idACDOwningPlayer == ACDID_INVALID || !PlayerIsLocal(idACDOwningPlayer))
                flAlpha *= NON_LOCAL_PET_ALPHA_MULTIPLIER;
        }
    }
#line 5975
    flAlpha *= ptRActorMain->tParentData.GetAlpha();
    return flAlpha;
}

#include "../../Common/Items.h"
#include "../UserInterface/Interface/UIConversations.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Target.h"

BOOL PowerIsCloseEnough(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIgnoreRange);  // PowersCommon.cpp:5061; DWARF records no header declaration.

#line 11481
BOOL ShouldShowPrompt(
    const ActorCommonData * ptACDGizmo,
    const ActorCommonData * ptACDPlayer)
{
    if (AppDrawFlagIsSet(APP_DRAW_NO_CONSOLE_ICONS_BIT))
        return FALSE;
#line 11490
    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    BOOL fOperatableGizmo = ptACDGizmo->IsOperatableGizmo(ACDID_INVALID, FALSE);
    if (!fOperatableGizmo && !ptACDGizmo->IsOperatableNPC(ptPlayer, TRUE))
        return FALSE;
    if (fOperatableGizmo)
    {
#line 11499
        GizmoType eType = ptACDGizmo->GetGizmoType();

        if (ptACDGizmo->IsItem() && ItemGetUseType(*ptACDGizmo) == ITEM_USE_HEALTHGLYPH)
            return FALSE;
#line 11506
        return eType == ACTORGIZMO_DOOR || eType == ACTORGIZMO_CHEST ||
            eType == ACTORGIZMO_PORTAL || eType == ACTORGIZMO_WAYPOINT ||
            eType == ACTORGIZMO_ITEM_INTERNAL || eType == ACTORGIZMO_HEALING_WELL ||
            eType == ACTORGIZMO_POWERUP || eType == ACTORGIZMO_TOWN_PORTAL ||
            eType == ACTORGIZMO_HEARTH_PORTAL || eType == ACTORGIZMO_HEADSTONE ||
            eType == ACTORGIZMO_SHARED_STASH || eType == ACTORGIZMO_SECRET_PORTAL ||
            eType == ACTORGIZMO_SWITCH || eType == ACTORGIZMO_GATE ||
            eType == ACTORGIZMO_ACT_TRANSITION_OBJECT || eType == ACTORGIZMO_BANNER ||
            eType == ACTORGIZMO_LORE_CHEST || eType == ACTORGIZMO_BOSS_PORTAL ||
            eType == ACTORGIZMO_PLACED_LOOT || eType == ACTORGIZMO_DUNGEON_PORTAL ||
            eType == ACTORGIZMO_IDENTIFY_ALL || eType == ACTORGIZMO_RETURN_PORTAL ||
            eType == ACTORGIZMO_MAILBOX || eType == ACTORGIZMO_LOOTRUN_SWITCH ||
            eType == ACTORGIZMO_POOL_OF_REFLECTION || eType == ACTORGIZMO_MULTICLICK ||
            eType == ACTORGIZMO_WEEKLYCHALLENGE_SWITCH || eType == ACTORGIZMO_OBJECTIVE_FINDER ||
            eType == ACTORGIZMO_WEEKLYCHALLENGE_INSPECT_ARMORRACK || eType == ACTORGIZMO_WEEKLYCHALLENGE_REWARD;
    }
    else
    {
        Target tTarget;
        tTarget.InitializeFromACD(ptACDGizmo->id, FALSE, FALSE);

        if (!PowerIsCloseEnough(ptACDPlayer->id, AutoPowerDefinitionR(GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO)), &tTarget, FALSE))
            return FALSE;
#line 11533
        return !UIConversation::ACDIsInConversation(ptACDGizmo->id, FALSE, TRUE);
    }
}

#include "../../Common/NPC.h"

#line 10610
BOOL RActorGetTooltip(
    const RActor * ptRActor,
    CRefString & usText,
    Player *& ptBannerPlayer)
{
    ptBannerPlayer = NULL;
#line 10618
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (!ptACD)
        return FALSE;
#line 10624
    if (ptACD->IsNPC())
    {
#line 10627
        if (RActorIsHostile(ptRActor))
            return FALSE;
#line 10631
        return NPCGetUIName(*ptACD, usText);
    }
#line 10634
    else if (ptACD->IsGizmo())
    {
        GizmoType eGizmoType = ptACD->GetGizmoType();
        switch (eGizmoType)
        {
        case ACTORGIZMO_ITEM_INTERNAL:
            usText = ItemGetDisplayName(*ptACD, TRUE, FALSE, FALSE, TRUE);
            return TRUE;
        case ACTORGIZMO_BANNER:
        {
#line 10650
            int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, ptACD->snoActor);
            if (sgtAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_ALL_BANNER_LOCS_BIT))
            {
                ptBannerPlayer = LocalPlayerGet();
            }
            else
            {
                ptBannerPlayer = PlayerGetByPlayerIndex(nPlayerDest);
                Player * ptLocalPlayer = LocalPlayerGetPrimary();

                if (PlayersAreHostileToEachOther(ptBannerPlayer, ptLocalPlayer))
                {
                    ptBannerPlayer = NULL;
                    return FALSE;
                }
            }
            return TRUE;
        }
        default:
#line 10670
            return GizmoGetUIName(*ptACD, usText);
        }
    }
#line 10675
    return FALSE;
}

BOOL ACDIsPet(const ACDID idACD);  // Player.cpp:1666; DWARF records no header declaration.

#line 5790
const RActorID RActorGetTopmostOwner(
    const RActorID idRActor,
    BOOL bForEffects,
    BOOL fForIsInUI)
{
    const RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return RACTORID_INVALID;
#line 5802
    while (ParentIsRActor(ptRActor->tParentData.m_eParentType) &&
           ptRActor->tParentData.m_idParent != RACTORID_INVALID)
    {
#line 5807
        if (bForEffects && (ACDIsPlayer(ptRActor->idACD) || ACDIsPet(ptRActor->idACD)))
            break;
#line 5813
        ptRActor = RActorTryToGet(RActorID(ptRActor->tParentData.m_idParent));
        if (!ptRActor)
            return RACTORID_INVALID;
    }
#line 5824
    if (fForIsInUI && ptRActor->ptParticleParams)
    {
        ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(ptRActor->ptParticleParams->idParticleSystem);
        if (ptParticleSystem && ParentIsRActor(ptParticleSystem->tParentData.m_eParentType))
        {
            return RActorGetTopmostOwner(RActorID(ptParticleSystem->tParentData.m_idParent), FALSE, FALSE);
        }
    }
#line 5837
    return ptRActor->id;
}

#include "../Prefetch.h"
#include "../Definitions/ParticleDefinition.h"

#line 10338
static BOOL sIsFlippyReady(
    const QueuedFlippyInfo & tInfo)
{
    if (!Prefetch::AddToQueue(SNOName(PARTICLE_SNO, tInfo.snoParticle), SNOFilesAsyncPriority_High))
        return FALSE;
#line 10346
    if (tInfo.snoActorOverride == SNO_NONE)
    {
        ScopedParticleDefinitionLockR pdefParticle(tInfo.snoParticle);
        if (!SNOPreload::PrefetchClientActor(pdefParticle->snoActor, NULL, SNOFilesAsyncPriority_High))
            return FALSE;
    }
    else
    {
#line 10356
        if (!SNOPreload::PrefetchClientActor(tInfo.snoActorOverride, NULL, SNOFilesAsyncPriority_High))
            return FALSE;
    }
    return TRUE;
}

#line 10445
void RActorFlippy(
    RActorID idRActor,
    const Vector3D & wpDest,
    const SNO snoFlippyParticle,
    const SNO snoActorLoot)
{
    Vector3D wpSource;

    const RActor * ptRActor = RActorTryToGet(idRActor);

    if (ptRActor && ptRActor->tLocation.eActorLocation == AL_WORLD)
    {
        wpSource = ptRActor->tLocation.tWorldLocation.transform.wp;
    }
    else
    {
        wpSource = wpDest;
    }

    QueuedFlippyInfo tInfo;
    tInfo.wpDest = wpDest;
    tInfo.transform = PRTransform(cgqIdentity, wpSource);
    tInfo.snoParticle = snoFlippyParticle;
    tInfo.snoActorOverride = snoActorLoot;
    if (!sIsFlippyReady(tInfo))
    {
        CGame & tCGame = CGameGet();
        tCGame.ptRActorGlobals->queuedFlippies.push_back(tInfo);
        return;
    }

    ParticleSystemAddParams tParams(snoFlippyParticle, PRTransform(cgqIdentity, wpSource));
    tParams.fIsFlippy = TRUE;
    tParams.snoActorOverride = snoActorLoot;
    tParams.fHasInitialAttractor = TRUE;
    tParams.wpAttractorDest = wpDest;

    ParticleSystemAdd(tParams);
}
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Lights.h"
#include "../Definitions/LightDefinition.h"

#include "../../Common/ActorInventory.h"
#include <float.h>

void LocationSetBounds(SNO snoActor, WorldLocation & tLocation, const ActorDefinition * pdefActor);  // ActorCommonData.cpp:4326
SNO ActorInventory_GetItemLandingSound(const ActorCommonData & tACDItem);  // DWARF: ActorInventory.cpp:12327; no header declaration recorded.


static void sRActorInitBannerData(RActor * ptRActor, const ActorCommonData & tACD);
static void sRActorAddACDEffects(RActor * ptRActor, ActorCommonData & tACD);

#line 744
static void sRActorSetLightFilterFlags(
    RActor * ptRActor)
{
    ptRActor->dwLightFilterFlags = 0;
#line 749
    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    LightingTypes eLightAs = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LIGHTING_TYPE);
    if (eLightAs == 0)
        ptRActor->dwLightFilterFlags = 1 << LIGHTIGNORE_SCENES_AND_SCENE_ACTORS_BIT;
    else
        ptRActor->dwLightFilterFlags = 1 << LIGHTIGNORE_ACTORS_BIT;
}

// EKF_* (DWARF: X/Common/Networking/GameMessage.h:930; src header lacks the enum)
enum {
    EKF_FORCE_INVENTORY_BIT = 0,
    EKF_DOESNT_WALK_BIT = 1,
    EKF_IS_HIDDEN_BIT = 2,
    EKF_SET_GROUND_TIME = 3,
    EKF_NO_COLLISION = 4,
    EKF_NEW_INVENTORY_ITEM_BIT = 5,
    EKF_FORCE_REMOTE_FX = 6,
    EKF_COUNT = 7,
};

#line 1793
static BOOL sRActorACDCreateFromQueuedEvent(
    const ACDNetworkName & ann,
    const SNO snoAmbientOcclusionOverrideTex,
    const uint32 dwFlags,
    const uint32 dwServerCreationTick)
{
    ACDID idACD = ACDFromANN(ann);
    if (idACD == ACDID_INVALID)
        return FALSE;
#line 1805
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return FALSE;
#line 1811
    SWorldID idSWorld = CGameGetCurrentSWorldID();

    if (idSWorld == SWORLDID_INVALID ||
        ptACD->tLocation.eActorLocation != AL_WORLD ||
        idSWorld != ptACD->tLocation.tWorldLocation.idSWorld)
        return FALSE;
#line 1820
    ScopedActorDefinitionLockR pdefActor(ptACD->snoActor);
    LocationSetBounds(ptACD->snoActor, ptACD->tLocation.tWorldLocation, pdefActor);
#line 1824
    float flScaleToUse;
    if (ptACD->tLocation.tWorldLocation.flScale <= EPSILON)
    {
        float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
        float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);
        flScaleToUse = RandRangeFloat_Internal(CGameGlobalsGet().rngLocal, flMinScale, flDeltaScale);
    }
    else
    {
        flScaleToUse = ptACD->tLocation.tWorldLocation.flScale;
    }
#line 1836
    ActorLocationSetScale(ptACD->tLocation, flScaleToUse);
#line 1838
    uint32 dwRActorAddFlags = 0;
    if (dwFlags & (1 << EKF_DOESNT_WALK_BIT))
        dwRActorAddFlags |= (1 << RACTOR_ADD_NO_WALK);
#line 1845
    RActorID idRActor = sRActorCreate(
        ACDID_INVALID,
        ptACD->snoActor,
        snoAmbientOcclusionOverrideTex,
        ptACD->tName,
        ptACD->tLocation.tWorldLocation.flScale,
        FLOAT_INFINITE_LIFETIME,
        ptACD->tLocation.tWorldLocation.transform,
        dwRActorAddFlags,
        RENDERCONTEXT_DEFAULT,
        SSCENEID_INVALID,
        dwServerCreationTick,
        pdefActor,
        TRUE);

#line 1862
    ptACD->SetOwner(idRActor);
#line 1864
    RActor * ptRActor = RActorGet(idRActor);
    sRActorSetACD(ptRActor, idACD, pdefActor);

    int32 nNewLook = RActorLookIndexFromLookLink(pdefActor, ptACD->nLookLinkIndex);
    RActorChangeLookIndex(*ptRActor, nNewLook, FALSE);
    sRActorInitBannerData(ptRActor, *ptACD);
    sRActorAddACDEffects(ptRActor, *ptACD);
#line 1873
    sRActorSetLightFilterFlags(ptRActor);
    ptRActor->dwFlags &= ~(1 << RACTOR_SHADOWS_ENABLED_BIT);
    return TRUE;
}

#line 9222
static SoundID sRActorPlaySound(
    const RActor * ptRActor,
    const SNO snoSound)
{
    if (snoSound == SNO_NONE)
        return SOUNDID_INVALID;
#line 9231
    PlaySoundOptions tOptions;
    tOptions.eContext = ptRActor->eContext;
    tOptions.pwp = &ptRActor->tLocation.tWorldLocation.transform.wp;
    tOptions.fOccluded = (ptRActor->dwVisibilityFlags >> RACTOR_NOT_IN_LOS_BIT) & 1;
    return SoundManager::PlaySound(snoSound, tOptions);
}

#line 9298
static void sRActorPlayLandingSound(
    const RActor * ptRActor)
{
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (!ptACD)
        return;
    // DWARF: snoLandingSound (9303) is at function level, so the null test is an early return
#line 9303
    SNO snoLandingSound = ActorInventory_GetItemLandingSound(*ptACD);
#line 9309
    sRActorPlaySound(ptRActor, snoLandingSound);
}


#line 10367
void RActorUpdateQueued()

{
    CGame & tCGame = CGameGet();
    blz::vector<QueuedFlippyInfo> & queuedFlippies = tCGame.ptRActorGlobals->queuedFlippies;
    uint32 nCount = queuedFlippies.size();
    for (uint32 i = 0; i < nCount; ++i)
    {
        QueuedFlippyInfo & tInfo = queuedFlippies[i];
        if (sIsFlippyReady(tInfo))
        {
            ParticleSystemAddParams tParams(tInfo.snoParticle, tInfo.transform);
            tParams.fIsFlippy = TRUE;
            tParams.snoActorOverride = tInfo.snoActorOverride;
            tParams.fHasInitialAttractor = TRUE;
            tParams.wpAttractorDest = tInfo.wpDest;

            ParticleSystemAdd(tParams);
            blzext::vector_erase_reorder(queuedFlippies, i);
            --i;
            --nCount;
        }
    }

    XGrowableMap<ACDNetworkName, ACDNetworkName, QueuedRActorInfo, QueuedRActorInfo &> & queuedRActors = tCGame.ptRActorGlobals->queuedRActors;
    ACDNetworkName annOldest = ANN_INVALID;
    float flOldestLastTimeChecked = FLT_MAX;
    POS pos = queuedRActors.GetStartAssoc();
    while (pos)
    {
        ACDNetworkName ann;
        QueuedRActorInfo tInfo;
        queuedRActors.GetNextAssoc(pos, ann, tInfo);

        if (annOldest == ANN_INVALID)
        {
            annOldest = ann;
            flOldestLastTimeChecked = tInfo.flLastTimeChecked;
        }
        else if (tInfo.flLastTimeChecked < flOldestLastTimeChecked)
        {
            annOldest = ann;
            flOldestLastTimeChecked = tInfo.flLastTimeChecked;
        }
    }
#line 10412
    if (annOldest != ANN_INVALID)
    {
        QueuedRActorInfo * ptInfo = queuedRActors.LookupPtr(annOldest);
        // Inferred: the original leaves the function when the lookup fails (cbz on the assoc pointer to the epilogue); no own line row.
        if (!ptInfo)
            return;
#line 10417
        if (!SNOPreload::IsClientActorRequested(annOldest) || !SNOPreload::IsClientActorPending(annOldest))
        {
            sRActorACDCreateFromQueuedEvent(annOldest, ptInfo->snoAmbientOcclusionOverrideTex, ptInfo->dwFlags, ptInfo->dwServerCreationTick);
            queuedRActors.RemoveAt(annOldest);

            if (ptInfo->bIsFlippy)
            {
                RActorID idRActor = RActorFindByANN(annOldest);
                if (idRActor != RACTORID_INVALID)
                {
                    RActor * ptRActor = RActorTryToGet(idRActor);
                    if (ptRActor)
                    {
                        sRActorPlayLandingSound(ptRActor);
                    }
                }
            }
        }
        else
        {
            ptInfo->flLastTimeChecked = CGameGetClientTime();
        }
    }
}

#line 1736
static void sRActorInitBannerDataForPlayerIndex(
    RActor * ptRActor,
    const int32 nPlayerIndex)
{
    if (!ptRActor->ptBannerRenderData)
    {
        ptRActor->ptBannerRenderData = new BannerRenderData;
    }
#line 1746
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
#line 1753
    if (ptPlayer)
    {
        *ptRActor->ptBannerRenderData = ptPlayer->tBannerRenderData;
        CPlayerUpdateBannerAttachments(*ptRActor, *ptRActor->ptBannerRenderData);
    }
}

#line 1763
static void sRActorInitBannerData(
    RActor * ptRActor,
    const ActorCommonData & tACD)
{
    if (tACD.GetGizmoType() == ACTORGIZMO_BANNER)
    {
        int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACD.snoActor);
        if (sgtAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_ALL_BANNER_LOCS_BIT))
        {
            nPlayerDest = LocalPlayerGet()->nPlayerIndex;
        }
        sRActorInitBannerDataForPlayerIndex(ptRActor, nPlayerDest);
    }
    else
    {
#line 1779
        for (int32 i = 0; i < MAX_PLAYERS; ++i)
        {
#line 1782
            if (tACD.snoActor == GlobalSNOGet(GLOBALSNO_ACTOR_EMOTE_BANNER_1 + i))
            {
                sRActorInitBannerDataForPlayerIndex(ptRActor, i);
                break;
            }
        }
    }
}

ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD);  // X/Common/Items.cpp:2208
BOOL ItemIsGlyph(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
CrafterType NPCGetCrafterType(const ActorCommonData & tACD);  // X/Common/NPC.cpp
namespace NephalemRift { BOOL IsRiftKey(GBID gbidItem); }  // X/Common/NephalemRiftCommon.h

#line 1483
static SNO sRActorGetRarityEffect(
    ActorCommonData & tACD)
{
    MonsterRarity eMonsterRarity = tACD.eMonsterRarity;

    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);

    SNO snoEffectGroup = SNO_NONE;

    switch (eMonsterRarity)
    {
    case MR_CHAMPION:
    case MR_RARE:
    case MR_RARE_MINION:
    case MR_UNIQUE:
    case MR_BOSS:
        snoEffectGroup = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_EFFECT_RARITY_MONSTER_EFFECTGROUP);
        break;
    default:
        break;
    }

    return snoEffectGroup;
}

static SNO sGetTwinkleEffect(const ActorCommonData & tACD);

#line 1682
static void sRActorAddACDEffects(
    RActor * ptRActor,
    ActorCommonData & tACD)
{
    SNO snoEffectGroup = sRActorGetRarityEffect(tACD);
    if (snoEffectGroup != SNO_NONE)
    {
        EffectGroupCreateUntracked(snoEffectGroup, ptRActor->id, INTERNAL_ATTACHMENTID_RARITY_EFFECT, FALSE, FALSE);
    }


    if (tACD.IsItem())
    {
        SNO snoEffectGroupScoped = sGetTwinkleEffect(tACD);
        if (snoEffectGroupScoped != SNO_NONE)
        {
            EffectGroupCreateUntracked(snoEffectGroupScoped, ptRActor->id, 0, FALSE, FALSE);
        }
    }


    if (tACD.IsNPC())
    {
        CrafterType eCrafterType = NPCGetCrafterType(tACD);
        // -1: CRAFTER_TYPE_INVALID (X/Common/Crafting/ItemCraftingSwig.h:23; not in src headers)
        if (eCrafterType != -1 || NPCCanTrade(tACD))
        {
            EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_VENDOR), ptRActor->id, 0, FALSE, FALSE);
        }
    }
}

#line 1716
static void sRActorDeleteACDEffectsOnDeath(
    RActor * ptRActor)
{
		ActorCommonData & tACD = ACDGet( ptRActor->idACD );
		SNO snoEffectGroup = sRActorGetRarityEffect(tACD);
	// tTriggerEvent (1724) has no lexical block DIE: function scope, so the test is an early return
	if (snoEffectGroup == SNO_NONE)
		return;
	TriggerEvent tTriggerEvent;
		TriggerEventInit(tTriggerEvent);
		tTriggerEvent.eType = TRIGGEREDEVENTTYPE_OUTRO_OBJECT;
		tTriggerEvent.dwID = INTERNAL_ATTACHMENTID_RARITY_EFFECT;

		RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);

}

#line 4538
static SNO sGetTwinkleEffect(
    const ActorCommonData & tACD)
{
    if (ItemIsGlyph(tACD))
    {
        return SNO_NONE;
    }

    if (ItemIsGold(tACD))
    {
        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_GOLD);
    }

    if (ItemIsOfType(tACD.hGB, GlobalGBIDGet(GLOBALGBID_ITEMTYPE_QUEST)))
    {
        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_QUEST);
    }

    if (tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CURRENCY_PLATINUM) ||
        tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CURRENCY_PLATINUM_UNCAPPED))
    {
        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_PLATINUM);
    }

    if (ItemHasLabel(tACD.hGB.gbid, ITL_COLLECTIBLE))
    {
        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_COLLECTIBLE);
    }

    ItemQualityLevel eIQL = ItemGetDisplayedQualityLevel(tACD);
    if ((eIQL >= IQL_MAGIC_AFFIX_START && eIQL <= IQL_MAGIC_AFFIX_END) ||
        eIQL == IQL_SPECIAL)
    {
        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_MAGIC);
    }
    else if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
    {
        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_RARE);
    }
    else if (eIQL == IQL_DISPLAY_AS_SET_ITEM)
    {
        if (ItemHasLabel(tACD.hGB.gbid, ITL_CRAFTING_PLAN_LEGENDARY))
        {
            return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_RARE);
        }

        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_SET);
    }
    else if (eIQL == IQL_LEGENDARY)
    {

        BOOL bIsLootRunKey = NephalemRift::IsRiftKey(tACD.hGB.gbid);

        if (ItemHasLabel(tACD.hGB.gbid, ITL_CRAFTING) ||
            ItemHasLabel(tACD.hGB.gbid, ITL_ORNAMENT) ||
            ItemHasLabel(tACD.hGB.gbid, ITL_CRAFTING_PLAN_LEGENDARY) ||
            bIsLootRunKey)
        {
            return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_RARE);
        }
        else if (ItemGetUseType(tACD) != ITEM_USE_PLAYER_GIFT)
        {
            return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_LEGENDARY);
        }
    }
    else
    {
        return GlobalSNOGet(GLOBALSNO_EFFECTGROUP_TWINKLE_NORMAL);
    }

    return SNO_NONE;
}

#include "../../Common/SNO/SymbolicValue.h"

#line 2949
static BOOL sIsComponentLook(
    const uint32 nLook,
    const uint32 nNumComponentLooks,
    const uint32 * pnComponentLooks)
{
    for (uint32 i = 0; i < nNumComponentLooks; ++i)
    {
        if (pnComponentLooks[i] == nLook)
            return TRUE;
    }
    return FALSE;
}

#line 2968
static void sResolveComponentLooks(
    const AppearanceDefinition * pdefAppearance,
    int32 & nResolvedComponentLookCount,
    uint32 * nComponentLooks,
    int32 nComponentLookCapacity)
{
#line 2975
    const int MAX_APPEARANCE_LOOKS = 128;
    uint32 nAppearanceLooks[MAX_APPEARANCE_LOOKS];
    int32 nAppearanceLookCount = pdefAppearance->nNumLooks;
    for (int32 i = 0; i < nAppearanceLookCount; ++i)
    {
#line 2982
        nAppearanceLooks[i] = pdefAppearance->ptAppearanceLooks[i].dwLookHash;
    }
#line 2989
    uint32 nNumComponentLooks = 0;
    while (gComponentLooks[nNumComponentLooks].sName)
    {
#line 2995
        uint32 nComponentLookHash = gComponentLooks[nNumComponentLooks].nValue;
#line 2999
        BOOL fFound = FALSE;
        for (int32 i = 0; i < nAppearanceLookCount; ++i)
        {
            if (nComponentLookHash == nAppearanceLooks[i])
            {
                nComponentLooks[nNumComponentLooks] = i;
                fFound = TRUE;
                break;
            }
        }
        if (!fFound)
            nComponentLooks[nNumComponentLooks] = (uint32)-1;
        ++nNumComponentLooks;
    }
    nResolvedComponentLookCount = nNumComponentLooks;
}

#line 3022
static void sRActorSetHair(
    RActor * ptRActor,
    const HairStyle eHairStyle)
{
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
    RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];
#line 3039
    const int MAX_COMPONENT_LOOKS = 128;
    uint32 nComponentLooks[MAX_COMPONENT_LOOKS];
    int32 nNumComponentLooks;
    sResolveComponentLooks(pdefAppearance, nNumComponentLooks, nComponentLooks, MAX_COMPONENT_LOOKS);
#line 3045
    for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
    {
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[i];

        const SubObject & tSubObject = tGeoSet.ptSubObjects[i];

        if (!tSubObject.tNameInfo.tDetails.fIsHair)
            continue;
#line 3057
        if (tSubObject.tNameInfo.tDetails.dwHairStyle == eHairStyle &&
#line 3061
            sIsComponentLook(tSubObjectState.nSubObjectLook, nNumComponentLooks, nComponentLooks))
        {
            tSubObjectState.dwFlags |= 1 << SUBOBJECTSTATE_RENDER_BIT;
        }
        else
        {
            tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_RENDER_BIT);
        }
    }
#line 3072
    sUpdateAllSubObjectLook(ptRActor, pdefAppearance);
}

#line 3078
void RActorSetHairStyle(
    RActor * ptRActor,
    const HairStyle eHairStyle)
{
    if (ptRActor->eHairStyle != eHairStyle)
    {
#line 3087
        sRActorSetHair(ptRActor, eHairStyle);

        ptRActor->eHairStyle = eHairStyle;
    }
}

#include "../../Common/Definitions/MonsterDefinition.h"

#line 11376
static RActorID sFindClosestMatchingRagdoll(
    const ACDID idACDTarget,
    const SNO snoMonster)
{
    RActorID idRActorTarget = (RActorID)ACDGetOwner(idACDTarget);
    RActor * ptRActorTarget = RActorTryToGet(idRActorTarget);
    if (!ptRActorTarget)
        return RACTORID_INVALID;
#line 11387
    ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
    SNO snoActor = pdefMonster->snoActor;

    Vector3D & wpTarget = ptRActorTarget->tLocation.tWorldLocation.transform.wp;

    float flBestDistSqr = FLT_MAX;
    RActorID idBestRActor = RACTORID_INVALID;
    CGame & tCGame = CGameGet();
    TDataArrayIterator<RActor> tIt;
    while (tCGame.ptRActors->Iterate(tIt))
    {
        RActor * ptRActor = tIt.ptElement;

        if (ptRActor->snoActor != snoActor ||
            RActorGetRagdollType(ptRActor) != FULL_RAGDOLL_TYPE)
        {
            continue;
        }
#line 11406
        Vector3D & wpCmp = ptRActor->tLocation.tWorldLocation.transform.wp;
        float flDistSqr = Vector3D::DistanceSquared(wpTarget, wpCmp);
        if (flDistSqr < flBestDistSqr)
        {
            flBestDistSqr = flDistSqr;
            idBestRActor = ptRActor->id;
        }
    }
    return idBestRActor;
}

#line 11425
void RActorRemoveNearestRagdoll(
    const ACDID idACDTarget,
    const SNO snoMonster)
{
    RActorID idRActor = sFindClosestMatchingRagdoll(idACDTarget, snoMonster);
    if (idRActor != RACTORID_INVALID)
    {
        EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_FALLEN_RESURRECTION), idRActor, 0, FALSE, FALSE);
#line 11435
        RActor * ptRActor = RActorGet(idRActor);
        ptRActor->flLife = 0.0f;
    }
}

#include "../../Sigma/MemoryPools.h"

#line 10717
static void RActorDeactivate(
    RActor * ptRActor)
{
#line 10725
    if (!(ptRActor->dwFlags & (1 << RACTOR_DELETE_ON_DEACTIVATE_BIT)))
    {
        SceneCommonData * ptSCD = SceneCommonDataTryToGet(ptRActor->idSScene);
        if (ptSCD)
        {
            InactiveMarkerCreateFromMarkerSet(
                *ptSCD,
                ptRActor->tMarkerOwner,
                ptRActor->tLocation.tWorldLocation.transform,
                ptRActor->tLocation.tWorldLocation.wsBounds);
        }
    }
#line 10739
    ptRActor->tMarkerOwner.Initialize();

    RActorDelete(ptRActor->id);
}

#line 10747
void RActorCheckSeenRecently()

{
#line 10752
    uint32 dwNow = GameGetServerTime();

    RActorIDListPooled listDeactivated(MemoryPoolsGetDATAIDListPool());
    RActorIDListPooled & listRActors = RActorGetMainList();
    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor)
            continue;
#line 10766
        if (ptRActor->tMarkerOwner.snoMarkerSet == SNO_NONE)
            continue;
#line 10772
        if (ptRActor->idSScene == SSCENEID_INVALID)
            continue;
#line 10778
        if (ptRActor->dwRActorLastSeen >= dwNow)
            continue;
#line 10783
        BOOL bKeepWithScene = MarkerKeepActivatedWithScene(ptRActor->snoActor);

        if (bKeepWithScene)
            continue;
#line 10791
        listDeactivated.AddTail_Tracked(ptRActor->id);
    }

    pos = listDeactivated.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listDeactivated.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            RActorDeactivate(ptRActor);
        }
    }
}

#include "../Scene.h"

BOOL RActorIsCritterWithRActorID(const RActorID idRActor);  // Behavior/Critter.cpp:754; no header declaration in DWARF.

#line 10809
void RActorDeactivateInScene(
    SSceneID idSScene)
{
    CGame & tCGame = CGameGet();

    if (!tCGame.ptRActors)
        return;
#line 10819
    RActorIDListPooled listDeactivated(MemoryPoolsGetDATAIDListPool());
    RActorIDListPooled & listRActors = RActorGetMainList();
    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (!ptRActor)
            continue;
#line 10831
        if (ptRActor->tMarkerOwner.snoMarkerSet == SNO_NONE)
        {
#line 10836
            BOOL bIsCritter = RActorIsCritterWithRActorID(idRActor);

            if (bIsCritter)
            {
                SSceneID idSSceneWalking = ptRActor->idSScene;
                if (idSSceneWalking == idSScene)
                {
                    RActorDelete(ptRActor->id);
                }
            }
            continue;
        }
#line 10851
        if (ptRActor->idSScene != idSScene)
            continue;
#line 10856
        listDeactivated.AddTail_Tracked(idRActor);
    }

    CScene * ptCScene = CSceneGetFromSSceneID(idSScene);
    if (ptCScene)
    {
        pos = ptCScene->listSceneRActors.GetHeadPosition();
        while (pos)
        {
            RActorID idRActor = (RActorID)ptCScene->listSceneRActors.GetNext(pos);
            RActor * ptRActor = RActorGet(idRActor);
            listDeactivated.AddTail_Tracked(ptRActor->id);
        }
        ptCScene->listSceneRActors.RemoveAll();
    }
#line 10873
    pos = listDeactivated.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listDeactivated.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            RActorDeactivate(ptRActor);
        }
    }
}

#include "../../Common/AnimPlayer.h"
#include "../Movement/CActorMovement.h"

#line 5557
void RActorsUpdateACD()

{

    RActorIDListPooled listRActorsToUpdate;
    listRActorsToUpdate.SetPool(MemoryPoolsGetDATAIDListPool());
    listRActorsToUpdate.Copy(RActorGetMainList());

    POS pos = listRActorsToUpdate.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActorsToUpdate.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor && ptRActor->idACD != ACDID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
            if (ptACD)
            {
                if (ptRActor->ptActorAnim)
                {
                    AnimPlayerUpdate(*ptACD);
                }

                if (ptRActor->ptCActorMovement)
                {
                    ptRActor->ptCActorMovement->Update();
                }
            }
        }
    }
}

#include "../Physics/PartialRagdoll.h"

#line 1387
static BOOL sGetParentScale(
    ParentData & tParentData,
    float & flParentScale)
{
    if (ParentIsRActor(tParentData.m_eParentType))
    {
        const RActor * ptRActorParent = RActorTryToGet(RActorID(tParentData.m_idParent));
        if (!ptRActorParent)
            return FALSE;
#line 1399
        const WorldLocation & loc = ptRActorParent->tLocation.tWorldLocation;
        flParentScale = loc.flScale * tParentData.m_flAttachmentScaleFactor;
        return TRUE;
    }
    return FALSE;
}

#line 1428
void RActorSetParent(
    const RActorID idRActor,
    const ParentData & tParentData,
    const BOOL fIgnoreOwnerAlpha,
    const BOOL fIgnoreOwnerScale)
{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;
#line 1440
    if (fIgnoreOwnerAlpha) ptRActor->dwFlags |= 1 << RACTOR_DONT_GET_ALPHA_FROM_PARENT_BIT; else ptRActor->dwFlags &= ~(1 << RACTOR_DONT_GET_ALPHA_FROM_PARENT_BIT);
    if (fIgnoreOwnerScale) ptRActor->dwFlags |= 1 << RACTOR_IGNORE_SCALE_FROM_PARENT_BIT; else ptRActor->dwFlags &= ~(1 << RACTOR_IGNORE_SCALE_FROM_PARENT_BIT);

    ptRActor->tParentData = tParentData;

    BOOL fWasFixedUpdate = ptRActor->dwFlagsEx & (1 << RACTOREX_FIXED_UPDATE_BIT);
    BOOL fFixedUpdate = sIsTopmostParentInUI(ptRActor);
    if (fFixedUpdate) ptRActor->dwFlagsEx |= 1 << RACTOREX_FIXED_UPDATE_BIT; else ptRActor->dwFlagsEx &= ~(1 << RACTOREX_FIXED_UPDATE_BIT);

    if (fFixedUpdate != fWasFixedUpdate)
    {
#line 1452
        RActorsUpdateFixedUpdateFlag();
    }
#line 1456
    if (!(ptRActor->dwFlags & (1 << RACTOR_IGNORE_SCALE_FROM_PARENT_BIT)))
    {
        float flParentScale;
        BOOL fValid = sGetParentScale(ptRActor->tParentData, flParentScale);
        if (fValid)
        {
            ActorLocationSetScale(ptRActor->tLocation, ptRActor->flBaseScale * flParentScale);
            RActorUpdateClientTransform(ptRActor, NULL, NULL);


            ActorAnim * ptActorAnim = ptRActor->ptActorAnim;
            if (flParentScale != 1.0f && ptActorAnim)
            {
                Ragdoll * ptRagdoll = ptActorAnim->ptRagdoll;
                if (ptRagdoll && ptRagdoll->GetType() == PARTIAL_RAGDOLL_TYPE)
                {
                    ::delete ptRagdoll;
                    ptActorAnim->ptRagdoll = new PartialRagdoll(ptRActor->id);
                }
            }
        }
    }
}

static BOOL sGetSceneVertColor(RActor * ptRActor, RGBAColorValue & rgbaval, BOOL fAssignCurrentAlso);

// 6259: ACDIsPlayer(ptACD->id): the original passes the id register ElementTryToGet compared (no reload of ptRActor->idACD).
#line 6192
void RActorInternalSetPosition(
    RActor * ptRActor,
    const Vector3D & wpPos,
    BOOL bAddWalkingEffects,
    SSceneID idSScene)
{


    WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

    if (Math::Abs(wpPos.x) >= 100000.0f ||
        Math::Abs(wpPos.y) >= 100000.0f ||
        Math::Abs(wpPos.z) >= 100000.0f)
    {
#line 6207
        RActorFlagForDeletion(ptRActor);
        return;
    }

    Vector3D wpOldWalkingPos = ptRActor->wpWalkingPos;

    ptRActor->wpWalkingPos = wpPos;

    Vector3D wpOffsetPos = ptRActor->wpWalkingPos;
    if (ptRActor->ptActorAnim)
    {
        Vector3D wvOffset;
        AnimationChannelGetNonlinearOffset(ptRActor->ptActorAnim->tAnimChannel, ptRActor->ptActorAnim->tAnimBoneCache, wvOffset);
        wvOffset = loc.transform.q.Rotate(wvOffset);
        wpOffsetPos += wvOffset;
    }
#line 6229
    ptRActor->tLocation.eActorLocation = AL_WORLD;
    loc.transform.wp = wpOffsetPos;
    if (idSScene == SSCENEID_INVALID)
    {
        WorldPlace tPlace;
        tPlace.idSWorld = loc.idSWorld;
        tPlace.wp = wpPos;
        idSScene = SceneCommonGetSceneIDFromWorldPlace(tPlace, FALSE);
    }
    loc.idSScene = idSScene;
#line 6245
    ptRActor->idSScene = idSScene;


    const CGame & tClientData = CGameGet();
    if (tClientData.bLastClientUpdate)
    {
#line 6254
        sGetSceneVertColor(ptRActor, ptRActor->rgbavalSceneTarget, FALSE);
    }
#line 6258
    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD && ACDIsPlayer(ptACD->id))
    {
        if (AnimPlayerGetAnimReason(*ptACD) == ANIM_REASON_IDLE)
        {

            SNO snoOldLevelArea = SNO_NONE;
            SNO snoNewLevelArea = SNO_NONE;

            {
                WorldPlace tPlace;
                tPlace.idSWorld = loc.idSWorld;
                tPlace.wp = wpOldWalkingPos;

                NavMeshHandle tNavMeshHandle;
                ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
                if (!NavMeshHandleIsNone(tNavMeshHandle))
                {
                    snoOldLevelArea = NavMeshHandleGetLevelArea(tNavMeshHandle);
                }

                tPlace.wp = ptRActor->wpWalkingPos;

                ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
                if (!NavMeshHandleIsNone(tNavMeshHandle))
                {
                    snoNewLevelArea = NavMeshHandleGetLevelArea(tNavMeshHandle);
                }
            }
#line 6288
            if (snoOldLevelArea != snoNewLevelArea || (ptRActor->dwFlagsEx & (1 << RACTOREX_FORCE_IDLE_ANIMATION_REFRESH_BIT)))
            {
#line 6292
                AnimPlayerPlayIdle(*ptACD);
                ptRActor->dwFlagsEx &= ~(1 << RACTOREX_FORCE_IDLE_ANIMATION_REFRESH_BIT);
            }
        }
    }
#line 6309
}

#line 11801
void RActorProcessMessages(
    RActor & tRActor,
    float flDeltaTime)
{
    if (tRActor.tRMsgList.Count())
    {
        RActorID idCachedID = tRActor.id;
        tRActor.tRMsgList.Update(flDeltaTime);
    }
}

#line 5716
void RActorsRegisterParentObjects(
    BOOL fFixedUpdateOnly,
    float flDeltaTime)
{
    RActorsEnforceMonsterCorpseLimit();

    RActorIDListPooled & listRActors = RActorGetMainList();
    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        POS lastPos = pos;
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor)
        {
            BOOL fRActorFixedUpdate = ptRActor->dwFlagsEx & (1 << RACTOREX_FIXED_UPDATE_BIT);
            if (!fFixedUpdateOnly || fRActorFixedUpdate)
            {
                RActorRegisterParentObject(ptRActor);
            }
        }
        else
        {
#line 5739
            listRActors.RemoveAt(lastPos);
        }
    }
#line 5744
    if (!fFixedUpdateOnly)
    {
        CGame & tCGame = CGameGet();
        TDataArrayIterator<CScene> tIt;
        while (CSceneDataArrayIterate(tIt, tCGame, SkipHolodeckScenes))
        {
            CScene * ptCScene = tIt.ptElement;
            if (SceneIsVisible(ptCScene->id))
            {
                pos = ptCScene->listSceneRActors.GetHeadPosition();
                while (pos)
                {
                    RActorID idRActor = (RActorID)ptCScene->listSceneRActors.GetNext(pos);
                    RActor * ptRActor = RActorGet(idRActor);
                    if (ptRActor->ptParticleParams || ptRActor->ptActorAnim || ptRActor->ptRigidAnim ||
                        ptRActor->nFadeGroupSubObjectIndex != -1 || ptRActor->tParentData.m_eParentType != PARENT_NONE)
                    {
                        RActorRegisterParentObject(ptRActor);
                    }
                    else
                    {
#line 5769
                        RActorProcessMessages(*ptRActor, flDeltaTime);
                    }
                }
            }
            else
            {
                pos = ptCScene->listSceneRActors.GetHeadPosition();
                while (pos)
                {
                    RActorID idRActor = (RActorID)ptCScene->listSceneRActors.GetNext(pos);
                    RActor * ptRActor = RActorGet(idRActor);
                    RActorProcessMessages(*ptRActor, flDeltaTime);
                }
            }
        }
    }
}

#include "../Rope.h"

#line 7365
void sAddRope(
    RActorID idRActor,
    SNO snoRope)
{
    RActor * ptRActor = RActorGet(idRActor);

    TriggerEvent tTriggerEvent;
    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_ROPE;
    tTriggerEvent.snoname.Set(ROPE_SNO, snoRope);

    AttachmentID idRopeAttachment = RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, FLOAT_INFINITE_LIFETIME, FALSE, 0.0f, PARENT_RACTOR);

    if (idRopeAttachment == ATTACHMENTID_INVALID)
        return;
#line 7383
    Attachment & tAttachment = *AttachmentGet(idRopeAttachment);
    RopeID idRope = tAttachment.idAttachedObject;


    RopeAttachPoint tSourceAttachPoint;
    tSourceAttachPoint.nRopeAttachType = ROPE_ATTACH_PARENT_CENTER;
    tSourceAttachPoint.tParentData.SetHardpointIndex(PARENT_RACTOR, idRActor, -1);

    {
#line 7393
        ScopedActorDefinitionLockR ptDefActor(ptRActor->snoActor);
        if (TagMapTagExists(ptDefActor->hTagMap, TAG_ACTOR_HARDPOINT_INDEX_FOR_ROPE))
        {
            int32 nHardPoint = TagMapGetInt(ptDefActor->hTagMap, TAG_ACTOR_HARDPOINT_INDEX_FOR_ROPE);
            tSourceAttachPoint.nRopeAttachType = ROPE_ATTACH_PARENT;
            tSourceAttachPoint.tParentData.SetHardpointIndex(PARENT_RACTOR, idRActor, nHardPoint);
        }
    }
#line 7403
    RopeSetAttachPoint(idRope, FALSE, tSourceAttachPoint);


    switch (ptRActor->tAimTarget.eType)
    {
    case TT_NONE:
        break;
    case TT_ACTOR:
    {
#line 7413
        RopeAttachPoint tAttachPoint;
        RActorID idRActorTarget = (RActorID)ACDGetOwner(ptRActor->tAimTarget.idACDTarget);
        tAttachPoint.nRopeAttachType = ROPE_ATTACH_PARENT_CENTER;
        tAttachPoint.tParentData.SetHardpointIndex(PARENT_RACTOR, idRActorTarget, -1);


        if (idRActorTarget != RACTORID_INVALID)
        {
            ScopedActorDefinitionLockR ptDefActor(RActorGet(idRActorTarget)->snoActor);
            if (TagMapTagExists(ptDefActor->hTagMap, TAG_ACTOR_HARDPOINT_INDEX_FOR_ROPE))
            {
                int32 nHardPoint = TagMapGetInt(ptDefActor->hTagMap, TAG_ACTOR_HARDPOINT_INDEX_FOR_ROPE);
                tSourceAttachPoint.tParentData.SetHardpointIndex(PARENT_RACTOR, idRActorTarget, nHardPoint);
                tSourceAttachPoint.nRopeAttachType = ROPE_ATTACH_PARENT;
            }
        }
#line 7430
        RopeSetAttachPoint(idRope, TRUE, tAttachPoint);
        break;
    }
    case TT_POINT:
    {
#line 7436
        WorldPlace tPlace;
        ptRActor->tAimTarget.GetPlace(tPlace);

        RopeAttachPoint tAttachPoint;
        tAttachPoint.nRopeAttachType = ROPE_ATTACH_WORLDPOINT;
        tAttachPoint.wpAttach = tPlace.wp;
        RopeSetAttachPoint(idRope, TRUE, tAttachPoint);
        break;
    }
    default:
#line 7500
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

#include "../Definitions/CAttachmentDefinition.h"

#line 4887
void RActorUpdatePropertiesFromParent(
    RActor * ptRActor,
    const ActorDefinition * pdefActor,
    const ActorCommonData * ptACD)
{
    if (!ptRActor->tParentData.ParentIsValid())
        return;
#line 4897
    PRTransform transform;
    ptRActor->tParentData.GetTransform(transform);

    if (ptRActor->tParentData.m_eParentType == PARENT_RACTOR)
    {
        const RActor * ptRActorParent = RActorTryToGet(RActorID(ptRActor->tParentData.m_idParent));
        if (ptRActorParent)
        {
            if (ptRActor->dwFlags & (1 << RACTOR_MESH_LIT_BIT))
            {
#line 4909
                ptRActor->rgbavalSceneTarget = ptRActorParent->rgbavalSceneTarget;
            }


            if (ptRActor->idACD != ACDID_INVALID && ACDGet(ptRActor->idACD).IsAttached())
            {
                int32 nHardpointIndexPelvis = HardpointIndexFromName(ptRActor->snoAppearance, PELVIS_HARDPOINT_NAME);
                int32 nHardpointIndexDefault = HardpointIndexFromName(ptRActor->snoAppearance, DEFAULT_HARDPOINT_NAME);

                if (nHardpointIndexPelvis != -1 && nHardpointIndexDefault != -1)
                {
                    PRTransform transformPelvisHP;
                    PRTransform transformDefaultHP;
                    RActorGetHardpointTransformByIndex(*ptRActor, nHardpointIndexPelvis, transformPelvisHP);
                    RActorGetHardpointTransformByIndex(*ptRActor, nHardpointIndexDefault, transformDefaultHP);
                    transform.wp = transform.wp + (transformDefaultHP.wp - transformPelvisHP.wp);
                }
            }
        }
    }
    else if (ptRActor->tParentData.m_eParentType == PARENT_RACTOR_TOP)
    {
        const RActor * ptRActorParent = RActorTryToGet(RActorID(ptRActor->tParentData.m_idParent));
        if (ptRActorParent)
        {
            transform = ptRActorParent->transformClient;
            RActorGetTopPoint(ptRActorParent, transform.wp);
        }
    }

    if (!(ptRActor->dwFlags & (1 << RACTOR_IGNORE_SCALE_FROM_PARENT_BIT)))
    {
        float flParentScale;
        BOOL fValid = sGetParentScale(ptRActor->tParentData, flParentScale);
        if (fValid)
        {
            ActorLocationSetScale(ptRActor->tLocation, ptRActor->flBaseScale * flParentScale);
        }
    }

    WorldLocation & locItem = ptRActor->tLocation.tWorldLocation;
    locItem.transform = transform;

    RActorUpdateClientTransform(ptRActor, pdefActor, ptACD);
}

#line 9415
static void sRActorPlayTaggedEffectGroup(
    RActor * ptRActor,
    const uint32 dwEffectTag)
{
    ScopedActorDefinitionLockR pDefActor(ptRActor->snoActor);
    SNO snoEffectGroup = TagMapGetSNO(pDefActor->hTagMap, dwEffectTag);

    if (snoEffectGroup != SNO_NONE)
    {
        EffectGroupCreateUntracked(snoEffectGroup, ptRActor->id, 0, FALSE, FALSE);
    }
}

#line 9611
void RActorPlayHitEffectFromMessage(
    const RActorID idRActor,
    const ACDID idACDAttacker,
    const DamageType eDamageType,
    const BOOL fCrit)
{
    RActor * ptRActor = RActorGet(idRActor);

    BOOL bAttackerIsOtherFriendlyPlayer = FALSE;
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    ACDID idACDLocalPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(idACDLocalPlayer);
    if (ptACDAttacker && ptACDLocalPlayer)
    {
        bAttackerIsOtherFriendlyPlayer = ACDIsPlayer(idACDAttacker) && !PlayerIsLocal(idACDAttacker) && ACDIsFriendly(*ptACDAttacker, *ptACDLocalPlayer, FALSE);
    }
#line 9631
    if (!bAttackerIsOtherFriendlyPlayer)
    {
        switch (eDamageType)
        {
        case DAMAGE_TYPE_PHYSICAL:
#line 9637
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_GET_HIT_NORMAL_EFFECTGROUP);
            break;
        case DAMAGE_TYPE_FIRE:
#line 9642
            if (fCrit)
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_FIRE_CRIT_PARTICLE);
            }
            else
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_FIRE_PARTICLE);
            }
            break;
        case DAMAGE_TYPE_LIGHTNING:
#line 9653
            if (fCrit)
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_LIGHTNING_CRIT_PARTICLE);
            }
            else
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_LIGHTNING_PARTICLE);
            }
            break;
        case DAMAGE_TYPE_POISON:
#line 9664
            if (fCrit)
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_POISONED_CRIT_PARTICLE);
            }
            else
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_POISONED_PARTICLE);
            }
            break;
        case DAMAGE_TYPE_COLD:
#line 9675
            if (fCrit)
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CHILLED_CRIT_PARTICLE);
            }
            else
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CHILLED_PARTICLE);
            }
            break;
        case DAMAGE_TYPE_ARCANE:
#line 9686
            if (fCrit)
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_ARCANE_CRIT_PARTICLE);
            }
            else
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_ARCANE_PARTICLE);
            }
            break;
        case DAMAGE_TYPE_HOLY:
#line 9696
            if (fCrit)
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_HOLY_CRIT_PARTICLE);
            }
            else
            {
                sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_HOLY_PARTICLE);
            }
            break;
        }
    }
#line 9708
    if (bAttackerIsOtherFriendlyPlayer)
    {
        EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_HITFLASH_LOFI), ptRActor->id, 0, FALSE, FALSE);
    }
    else
    {
        EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_HITFLASH_DEFAULT), ptRActor->id, 0, FALSE, FALSE);
    }
}

#include "RActorAnimation.h"
#include "RActorCollision.h"

void RActorGfxDisposeForGame();  // RActorGfx.cpp:174; no header declaration in DWARF.

#line 595
void RActorsDisposeForGame()

{
    RActorsAnimDisposeForGame();
    RActorGfxDisposeForGame();
    RActorsCollisionDisposeForGame();

    CGame & tCGame = CGameGet();

    delete tCGame.ptRActorGlobals->plistMonsterCorpses; tCGame.ptRActorGlobals->plistMonsterCorpses = NULL;

    if (tCGame.ptRActors)
    {
        TChainedDataArrayDispose(tCGame.ptRActors);
    }

    tCGame.ptRActorGlobals->queuedFlippies.~vector();
    tCGame.ptRActorGlobals->queuedRActors.Dispose();

    tCGame.ptRActorGlobals->poolRActorSubObjectData.Dispose();
    tCGame.ptRActorGlobals->poolRigidAnimations.Dispose();
    tCGame.ptRActorGlobals->poolCollisionCapsules.Dispose();
    tCGame.ptRActorGlobals->listMainRActors.RemoveAll();

    tCGame.ptRActorGlobals->tAppearanceAttributeEffectTagMap.Dispose();
    tCGame.ptRActorGlobals->tAppearanceAttributeEffectTypeMap.Dispose();
    tCGame.ptRActorGlobals->tAppearanceAttributeGlobalSNOMap.Dispose();
    tCGame.ptRActorGlobals->tAppearanceAttributePowerTagMap.Dispose();
    tCGame.ptRActorGlobals->tAppearanceAttributePowerInitializedMap.Dispose();
    XFREE(tCGame.ptRActorGlobals, NULL);
#line 631
}

#include "../../Common/ComponentStyle.h"
#line 2725
static void sFindComponentForSlotStyle(
    const RActorID idRActor,
    const InventorySlotType eSlot,
    const ComponentStyle eCompStyle,
    int32 * pnCompIndices,
    int32 & nCompIndexCount)
{
    LPCSTR pszSlot = InventorySlotToComponentString(eSlot);
    if (!pszSlot)
        return;




    const RActor * ptRActor = RActorGet(idRActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
    const RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];









    for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
    {
        const SubObject & tSubObject = tGeoSet.ptSubObjects[i];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[i];
        if ((tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_CAN_BE_RENDERED_BIT)) &&
            tSubObject.tNameInfo.tDetails.fIsSlot)
        if (tSubObject.tNameInfo.tDetails.dwSlot == eSlot &&
            tSubObject.tNameInfo.tDetails.fIsComponent &&
            tSubObject.tNameInfo.tDetails.dwComponent == eCompStyle)
        {
            pnCompIndices[nCompIndexCount] = i;
            ++nCompIndexCount;
        }
    }





}




void RActorComponentsSetSlot(
    RActor * ptRActor,
    const InventorySlotType eSlot,
    const ComponentStyle eCompStyle,
    const int32 nSubObjectLook,
    const DyeType eDyeType,
    const BOOL bDisplayErrors)
{
    LPCSTR pszSlot = InventorySlotToComponentString(eSlot);
    if (!pszSlot)
        return;




    ComponentStyle eCompStyleToUse = eCompStyle;
    int32 nSubObjectLookToUse = nSubObjectLook;

    if (eDyeType == DYE_TYPE_VANISHING)
    {

        AppearanceGetNakedLook(ptRActor->snoAppearance, eCompStyleToUse, nSubObjectLookToUse);
    }

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
    RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];



    if (eSlot == SLOT_PLAYER_SHOULDERS && eCompStyleToUse == COMPONENTSTYLE_NONE)
    {


        for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
        {
            const SubObject & tSubObject = tGeoSet.ptSubObjects[i];
            SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[i];
            if ((tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_CAN_BE_RENDERED_BIT)) &&
                tSubObject.tNameInfo.tDetails.fIsSlot &&
                tSubObject.tNameInfo.tDetails.dwSlot == eSlot)
                tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_RENDER_BIT);
        }
        sUpdateAllSubObjectLook(ptRActor, pdefAppearance);
        return;
    }













    int32 arCompIndices[8];
    int32 nCompIndexCount = 0;
    sFindComponentForSlotStyle(ptRActor->id, eSlot, eCompStyleToUse, arCompIndices, nCompIndexCount);
    if (!nCompIndexCount)
    {
        switch (eCompStyleToUse)
        {
        case COMPONENTSTYLE_LIGHT_B:
        case COMPONENTSTYLE_LIGHT_C:
            eCompStyleToUse = COMPONENTSTYLE_LIGHT_A;
            break;
        case COMPONENTSTYLE_MEDIUM_B:
        case COMPONENTSTYLE_MEDIUM_C:
            eCompStyleToUse = COMPONENTSTYLE_MEDIUM_A;
            break;
        case COMPONENTSTYLE_HEAVY_B:
        case COMPONENTSTYLE_HEAVY_C:
            eCompStyleToUse = COMPONENTSTYLE_HEAVY_A;
            break;
        default: eCompStyleToUse = COMPONENTSTYLE_NONE;
        }
        if (eCompStyleToUse != COMPONENTSTYLE_NONE)
            sFindComponentForSlotStyle(ptRActor->id, eSlot, eCompStyleToUse, arCompIndices, nCompIndexCount);
        if (!nCompIndexCount)
        {
            eCompStyleToUse = COMPONENTSTYLE_NONE;
            sFindComponentForSlotStyle(ptRActor->id, eSlot, eCompStyleToUse, arCompIndices, nCompIndexCount);
        }
    }























    for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
    {
        const SubObject & tSubObject = tGeoSet.ptSubObjects[i];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[i];
        if ((tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_CAN_BE_RENDERED_BIT)) &&
            tSubObject.tNameInfo.tDetails.fIsSlot &&
            tSubObject.tNameInfo.tDetails.dwSlot == eSlot)
        {







            BOOL fComponentMatches = tSubObject.tNameInfo.tDetails.fIsComponent &&
                tSubObject.tNameInfo.tDetails.dwComponent == eCompStyleToUse;
            if (fComponentMatches)
                tSubObjectState.dwFlags |= (1 << SUBOBJECTSTATE_RENDER_BIT);
            else
                tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_RENDER_BIT);




            for (int32 nComp = 0; nComp < nCompIndexCount; ++nComp)
            {
                if (i == arCompIndices[nComp])
                {
                    if (tSubObjectState.nSubObjectLook != nSubObjectLookToUse)
                    {
                        tSubObjectState.nSubObjectLook = nSubObjectLookToUse;




                        SubObjectAppearance & tAppearance = AppearanceGetSubObjectAppearance(pdefAppearance, nSubObjectLookToUse, i, eGeoSetType);
                        if (tAppearance.tCompositeUberMaterial.snoShaderMap != SNO_NONE)
                        {
                            ScopedShaderMapDefinitionLockR pdefShaderMap(tAppearance.tCompositeUberMaterial.snoShaderMap);
                            tSubObjectState.snoDefaultShader = ShaderMapGetDefaultShader(tAppearance.tCompositeUberMaterial.snoShaderMap, TRUE, pdefShaderMap);
                            tSubObjectState.snoFadeShader = ShaderMapGetFadeShader(tAppearance.tCompositeUberMaterial.snoShaderMap, pdefShaderMap);
                        }
                    }
                    tSubObjectState.eDyeType = eDyeType;
                }
            }
        }
    }
    sUpdateAllSubObjectLook(ptRActor, pdefAppearance);




}

#include "../TexAnimation.h"

#line 2497
static void sUpdateAllSubObjectLook(
    RActor * ptRActor,
    const AppearanceDefinition * pdefAppearance)
{

    if (ptRActor->dwFlags & (1 << RACTOR_SUBOBJECT_LOOK_UPDATE_LOCKED))
        return;





    const Structure & tStructure = pdefAppearance->tStructure;
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet & tGeoSet = tStructure.tGeoSets[eGeoSetType];

    SNONameArray tTexturesToAdd;
    SNONameArray tTexturesToRemove;
    for (int32 nSubObjectIndex = 0; nSubObjectIndex < tGeoSet.nSubObjectCount; ++nSubObjectIndex)
    {
        SubObject & tSubObject = tGeoSet.ptSubObjects[nSubObjectIndex];
        RActorGeoSetData & tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];
        SubObjectState & tSubObjectState = tGeoSetData.ptSubObjectState[nSubObjectIndex];

        BOOL fRendered = tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_RENDER_BIT);
        BOOL fShouldLoadTexture = (ptRActor->dwFlags & (1 << RACTOR_SUBOBJECT_TEXTURE_LOAD_ALL)) || fRendered;

        BOOL fShouldUnload = tSubObjectState.nSubObjectLoadedLook != tSubObjectState.nSubObjectLook;
        if (fShouldUnload)
        {
            if (tSubObjectState.idCloth != CLOTHID_INVALID)
            {
                ClothManager::Destroy(tSubObjectState.idCloth);
                tSubObjectState.idCloth = CLOTHID_INVALID;
            }
            if (tSubObjectState.arTexAnimStates)
                MaterialFreeTexAnimState(tSubObjectState.arTexAnimStates);





            if (tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT))
            {
                const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLoadedLook];
                int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
                MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
                for (int32 i = 0; i < nCount; ++i)
                {
                    SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
                    if (snoTex != SNO_NONE)
                    {
                        tTexturesToRemove.push_back(SNOName(TEXTURE_SNO, snoTex));
                    }
                }
                tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT);
            }
        }

        tSubObjectState.nSubObjectLoadedLook = tSubObjectState.nSubObjectLook;


        const SubObjectAppearance & tSOA = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
        if (fRendered)
        {
            if (tSOA.snoCloth != SNO_NONE && tSubObjectState.idCloth == CLOTHID_INVALID)
            {
                tSubObjectState.idCloth = ClothManager::Create(ptRActor, nSubObjectIndex, tSOA.snoCloth);
            }
            if (!tSubObjectState.arTexAnimStates)
            {
                tSubObjectState.arTexAnimStates = MaterialAllocTexAnimState(tSOA.tCompositeUberMaterial, tSubObjectState.nTexAnimStateCount);
                TexAnimInitialize(tSOA.tCompositeUberMaterial, tSubObjectState.arTexAnimStates, ptRActor);
            }
        }
        else
        {
            if (tSubObjectState.idCloth != CLOTHID_INVALID)
            {
                ClothManager::Destroy(tSubObjectState.idCloth);
                tSubObjectState.idCloth = CLOTHID_INVALID;
            }
            if (tSubObjectState.arTexAnimStates)
                MaterialFreeTexAnimState(tSubObjectState.arTexAnimStates);
        }






        if (fShouldLoadTexture)
        {
            if (!(tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT)))
            {
                int32 nCount = tSOA.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
                MaterialTextureEntry * ptMatTexList = tSOA.tCompositeUberMaterial.ptMatTexList;
                for (int32 i = 0; i < nCount; ++i)
                {
                    SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
                    if (snoTex != SNO_NONE)
                    {
                        tTexturesToAdd.push_back(SNOName(TEXTURE_SNO, snoTex));
                    }
                }
                tSubObjectState.dwFlags |= 1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT;
            }
        }
        else if (tSubObjectState.dwFlags & (1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT))
        {



            const SubObjectAppearance & tSOAScoped = tSubObject.ptPostprocessed->ptMaterial->ptSOAs[tSubObjectState.nSubObjectLook];
            int32 nCount = tSOAScoped.tCompositeUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
            MaterialTextureEntry * ptMatTexList = tSOAScoped.tCompositeUberMaterial.ptMatTexList;
            for (int32 i = 0; i < nCount; ++i)
            {
                SNO snoTex = ptMatTexList[i].tMatTex.snoTex;
                if (snoTex != SNO_NONE)
                {
                    tTexturesToRemove.push_back(SNOName(TEXTURE_SNO, snoTex));
                }
            }
            tSubObjectState.dwFlags &= ~(1 << SUBOBJECTSTATE_TEXURES_LOADED_BIT);
        }
    }




    if (tTexturesToAdd.size())
        SNOPreload::AddClientTextures(tTexturesToAdd);


    if (tTexturesToRemove.size())
        SNOPreload::RemoveClientTextures(tTexturesToRemove);



}

// Signature from CActorInventory.cpp:416; no header declaration is recorded in DWARF.
BOOL CACDInventorySetLocation(const ACDID idACDPickee, ActorCommonData & tACDItem, const InventoryLocation & tNewInvLocation, const BOOL bPlayNewItemNotification);

#line 7118
BOOL RActorSetACDLocationFromMessage(
    ActorCommonData & tACD,
    const ActorLocation & tNewLocation,
    BOOL bFirstTime,
    const BOOL fNewItem,
    const BOOL fCreateActor)
{
    BOOL bChangedWorlds = FALSE;

    if (tNewLocation.eActorLocation == AL_WORLD)
    {
        WorldLocation & loc = tACD.GetWorldLocation();
        WorldPlace tLocationPlace;
        tLocationPlace.wp = tNewLocation.tWorldLocation.transform.wp;
        tLocationPlace.idSWorld = tNewLocation.tWorldLocation.idSWorld;

        if (loc.idSWorld != tLocationPlace.idSWorld)
        {
            tACD.ChangeWorld(tLocationPlace, tNewLocation.tWorldLocation.transform.q);
            bChangedWorlds = TRUE;
        }
        else
        {
            loc.transform = tNewLocation.tWorldLocation.transform;

            NavMeshHandle hNavMeshCurr;
            ScenesCommonNavMeshNodeFromPlace(tLocationPlace, &hNavMeshCurr, FALSE, FALSE);
            loc.idSScene = hNavMeshCurr.idSScene;

            loc.flScale = tNewLocation.tWorldLocation.flScale;
        }
    }



    RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);
    if (idRActor != RACTORID_INVALID)
    {
        RActor * ptRActor = RActorGet(idRActor);


        switch (tNewLocation.eActorLocation)
        {
        case AL_WORLD:
            if (!bChangedWorlds)
            {


                WorldLocation & loc = tACD.GetWorldLocation();
                loc.transform = tNewLocation.tWorldLocation.transform;
                loc.flScale = tNewLocation.tWorldLocation.flScale;
                RActorInternalSetPosition(ptRActor, loc.transform.wp, bFirstTime, SSCENEID_INVALID);
                RActorSetScale(ptRActor, loc.flScale, TRUE, NULL);
                RActorSetOrientation(ptRActor, loc.transform.q, NULL, NULL);
            }
            else if (!PlayerIsLocal(idRActor))
            {
                RActorDelete(idRActor);
            }
            break;







        case AL_NONE:
            RActorDelete(idRActor);
            break;
        case AL_INVENTORY:
        {


            RActorDelete(idRActor);


            ACDID idOwnerACD = tNewLocation.tInvLocation.idACDContainer;

            CACDInventorySetLocation(idOwnerACD, tACD, tNewLocation.tInvLocation, TRUE);
            break;
        }
        }
    }
    else
    {


        switch (tNewLocation.eActorLocation)
        {
        case AL_WORLD:
        {








            if (fCreateActor)
            {

                XGrowableMap<ACDNetworkName, ACDNetworkName, QueuedRActorInfo, QueuedRActorInfo &> & queuedRActors = CGameGet().ptRActorGlobals->queuedRActors;
                if (queuedRActors.LookupPtr(tACD.ann))
                    break;



                SWorldID idSWorld = CGameGetCurrentSWorldID();

                if (tNewLocation.tWorldLocation.idSWorld == idSWorld)
                {

                    sRActorCreateFromACD(tACD.id);
                }
            }
            break;
        }







        case AL_NONE:
        {
            if (tACD.tLocation.eActorLocation != AL_NONE)
            {





                ActorCommonData & tACDContainer = ACDGet(tACD.tLocation.tInvLocation.idACDContainer);
                ACDInventoryDrop(tACDContainer, tACD, NULL);
                ACDDelete(tACD.id, FALSE);
            }
            return TRUE;
        }
        case AL_INVENTORY:
        {
            ACDID idACDOwner = tNewLocation.tInvLocation.idACDContainer;

            ActorLocationType eActorLocation = tACD.GetLocationType();



            const ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);


            if (!ptACDOwner)
            {
                ACDDelete(tACD.id, FALSE);
                return TRUE;
            }












            BOOL fOwnerItem = ptACDOwner->IsItem();

            if (eActorLocation == AL_INVENTORY && !fOwnerItem && !bFirstTime)
            {



                InventoryLocation tInvLocDest;
                ACDInventoryLocationSet(tInvLocDest, idACDOwner, tNewLocation.tInvLocation.eSlot, tNewLocation.tInvLocation.pt.x, tNewLocation.tInvLocation.pt.y);
                ACDInventoryMove(tACD, tInvLocDest);

                tACD.ChangeLocation(AL_INVENTORY);
                InventoryLocation & tInvLocation = tACD.GetInventoryLocation();
                tInvLocation.eSlot = tInvLocDest.eSlot;
                tInvLocation.pt = tInvLocDest.pt;
                tInvLocation.idACDContainer = tInvLocDest.idACDContainer;
            }










            else if (eActorLocation == AL_NONE || eActorLocation == AL_WORLD || fOwnerItem || bFirstTime)
            {



                CACDInventorySetLocation(idACDOwner, tACD, tNewLocation.tInvLocation, fNewItem);
            }
            break;
        }
        }
    }
    return FALSE;

}

#include "../Behavior/Behavior.h"
#include "../../Common/Definitions/AnimTreeDefinition.h"
// CPP-only APIs recovered from the defining DWARF subprograms.
BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor); // RActorAnimation.cpp:1962
void AnimationChannelGetAvgVelocityPerTicks(const AnimChannel & tChannel, const AnimBoneCache & tCache, const float flTranslationScale, Vector3D & wvVel); // Animation.cpp:727
void AnimationChannelUpdate(AnimChannel & tChannel, AnimBoneCache & tCache, float flDeltaTime); // Animation.cpp:882
void AnimationChainUpdate(RActor * ptRActor, float flDeltaTime); // AnimationChain.cpp:145
void AnimTreeUpdate(RActor * ptRActor, const AnimTreeDefinition & tTreeDefinition, const AnimTreeControl & tControl, AnimTree & tTree, float flDeltaTime); // Animation.cpp:2686
void RActorHandleKeyframedRMessages(RActor * ptRActor); // RActorAttachments.cpp:533
float SceneRActorGetFade(RActor * ptRActor); // Scene.cpp:3941
float SceneSubObjectGetFade(RActor * ptRActor, SSceneID idSScene, int32 nSubObjectIndex); // Scene.cpp:3880
void ParticleSystemDisturbFoliage(const Vector3D & wp, const Vector3D & wvVelocity); // Particle.cpp:6687
void RActorPhysicsDisturbRigidBodies(const AABB & aabb, const Vector3D & wvVelocity); // RActorPhysics.cpp:416
#line 454 "X/Client/RActors/RActors.cpp"
BOOL HitJitter::Update()
{

    const float flApproachSpeed = 0.2f;

    if (Math::Abs(m_flMag) < 0.001f)
    {
        m_flMag = 0.0f;
        return FALSE;
    }
    Math::Lerp(m_flMag, 0.0f, flApproachSpeed, m_flMag);
    return TRUE;

}
#line 507 "X/Client/RActors/RActors.cpp"
BOOL RActorSink::Update()
{

    if (m_flUnitSinkSpeed <= EPSILON)
        return FALSE;



    m_flUnitSink += m_flUnitSinkSpeed;
    m_flUnitSink = Math::Pin(m_flUnitSink, 0.0f, 1.0f);

    return TRUE;
}
#line 4958 "X/Client/RActors/RActors.cpp"
static void sRActorGenerateDisturbances(
    RActor * ptRActor,
    const WorldLocation & loc)
{
    if ((sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT)) &&
        PlayerIsLocal(ptRActor->id))
        return;




    Vector3D wvVelocity = ptRActor->wvVelocity;


    ParticleSystemDisturbFoliage(ptRActor->transformClient.wp, wvVelocity);


    float r = loc.wcylBounds.wdRadius;
    float h = loc.wcylBounds.wdHeight * 0.5f;
    AABB aabb;
    aabb.wp = loc.wcylBounds.wpBase;
    aabb.wp.z += h;
    aabb.wvExt.Set(r, r, h);
    RActorPhysicsDisturbRigidBodies(aabb, wvVelocity);
}
#line 5302 "X/Client/RActors/RActors.cpp"
void RActorPostThreadSafeUpdate(
    RActor * ptRActor,
    Vector3D & wpActor,
    ActorCommonData * ptACD,
    float flDeltaTime,
    const ActorDefinition * ptDefActor)
{
    BOOL fLocalLock = (ptDefActor == NULL);
    if (fLocalLock)

        ptDefActor = (const ActorDefinition *)gptGroupDefActor->LockR(ptRActor->snoActor, FALSE);






    RagdollType ragdollType = INVALID_RAGDOLL_TYPE;

    if (ptRActor->ptParticleParams)
    {
        if (ptRActor->ptParticleParams->eType < PSTYPE_FOLIAGE_ACTOR || ptRActor->ptParticleParams->eType > PSTYPE_CLUTTER_DYNAMIC)
        {




            RActorSnapToPosition(ptRActor, wpActor, SSCENEID_INVALID, ptDefActor, ptACD);
        }
    }

    if (ptRActor->ptActorAnim)
    {
        ragdollType = RActorGetRagdollType(ptRActor);



        if ((ragdollType == INVALID_RAGDOLL_TYPE || ragdollType == PARTIAL_RAGDOLL_TYPE) &&
            ptRActor->ptCActorMovement && !ptRActor->ptCActorMovement->IsMoving())
        {
            Vector3D wvVelocityPerTicks;
            AnimationChannelGetAvgVelocityPerTicks(ptRActor->ptActorAnim->tAnimChannel, ptRActor->ptActorAnim->tAnimBoneCache, 1.0f, wvVelocityPerTicks);
            // bAddWalkingEffects is optimized out; FALSE is inferred for this position refresh.
            if (wvVelocityPerTicks.MagnitudeSquared() > EPSILON)
            {
                RActorInternalSetPosition(ptRActor, ptRActor->wpWalkingPos, FALSE, SSCENEID_INVALID);
            }
        }

        RActorUpdateClientTransform(ptRActor, ptDefActor, ptACD);
    }

    if (ptRActor->tHitJitter.Update())
    {
        RActorUpdateClientTransform(ptRActor, ptDefActor, ptACD);
    }

    if (ptRActor->tSink.Update())
    {
        RActorUpdateClientTransform(ptRActor, ptDefActor, ptACD);
    }

    if (ptRActor->tCollisionCapsuleList.Count())

        RActorCollisionCapsulesUpdate(ptRActor);


    RActorProcessMessages(*ptRActor, flDeltaTime);


    if (ptRActor->ptActorAnim)
    {
        ActorAnim & tActorAnim = *ptRActor->ptActorAnim;


        if (RActorAnimationIsAnimTreeEnabled(*ptRActor) || !tActorAnim.tAnimChannel.bPaused)

            RActorHandleKeyframedRMessages(ptRActor);


        if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
        {
            AnimationChannelUpdate(tActorAnim.tAnimChannel, tActorAnim.tAnimBoneCache, flDeltaTime);
        }
        else
        {
            ScopedAnimTreeDefinitionLockR pdefAnimTree(tActorAnim.snoAnimTree);
            AnimTreeUpdate(ptRActor, *pdefAnimTree, *tActorAnim.ptAnimTreeControl, *tActorAnim.ptAnimTree, flDeltaTime);
        }

        if (ragdollType == INVALID_RAGDOLL_TYPE || ragdollType == PARTIAL_RAGDOLL_TYPE)

            AnimationChainUpdate(ptRActor, flDeltaTime);

    }


    if (CGameGetGameCommonData().GameModeGet() != GAMEMODE_HOLODECK)
    {
        BehaviorStatus eStatus = BehaviorUpdate(ptRActor);
        if (eStatus == BEHAVIOR_STATUS_DELETE)
        {
            RActorDelete(ptRActor->id);
            if (fLocalLock)
                gptGroupDefActor->UnlockR((void *&)ptDefActor);
            return;
        }



    }


    if (ptRActor->nFadeGroupSubObjectIndex != -1)
    {


        if (ptRActor->nFadeGroupSubObjectIndex == 0)

            ptRActor->flAlpha = SceneRActorGetFade(ptRActor);

        else

            ptRActor->flAlpha = SceneSubObjectGetFade(ptRActor, ptRActor->idSScene, ptRActor->nFadeGroupSubObjectIndex);

    }


    if (ptRActor->flLife >= 0.0f)
    {
        if (ptRActor->flLife == 0.0f)
        {
            BOOL fDelete = TRUE;


            if (ptRActor->flFadeTime != 0.0f)
            {
                float flCurrentFadeTime = ptRActor->flFadeTime * ptRActor->flFadeAlpha;
                flCurrentFadeTime = Math::Max(flCurrentFadeTime - flDeltaTime, 0.0f);
                ptRActor->flFadeAlpha = flCurrentFadeTime / ptRActor->flFadeTime;
                if (ptRActor->flFadeAlpha > 0.0f ||
                    ptRActor->idACD != ACDID_INVALID)
                    fDelete = FALSE;
            }

            if (fDelete)
            {
                RActorDelete(ptRActor->id);
                if (fLocalLock)
                    gptGroupDefActor->UnlockR((void *&)ptDefActor);
                return;
            }





        }

        else

            ptRActor->flLife = Math::Max(ptRActor->flLife - flDeltaTime, 0.0f);

    }


    if (ptRActor->flTriggerFadePerTick > 0.0f)
    {
        ptRActor->flTriggerFadeAlpha += ptRActor->flTriggerFadePerTick;
        if (ptRActor->flTriggerFadeAlpha >= 1.0f)
        {
            if (ptRActor->dwFlags & (1 << RACTOR_FADE_AFFECTS_SHADOWS_BIT))

                ptRActor->dwFlags |= (1 << RACTOR_SHADOWS_ENABLED_BIT);


            ptRActor->flTriggerFadeAlpha = 1.0f;
            ptRActor->flTriggerFadePerTick = 0.0f;
        }
    }
    else if (ptRActor->flTriggerFadePerTick < 0.0f)
    {
        ptRActor->flTriggerFadeAlpha += ptRActor->flTriggerFadePerTick;
        if (ptRActor->flTriggerFadeAlpha <= 0.0f)
        {
            ptRActor->flTriggerFadeAlpha = 0.0f;
            ptRActor->flTriggerFadePerTick = 0.0f;
        }

        if (ptRActor->dwFlags & (1 << RACTOR_FADE_AFFECTS_SHADOWS_BIT))

            ptRActor->dwFlags &= ~(1 << RACTOR_SHADOWS_ENABLED_BIT);
    }


    if (ptRActor->dwOverlayFadeTimeCurrent != INFINITE_OVERLAY_FADE_TIME)

        ptRActor->dwOverlayFadeTimeCurrent = Math::Max((int32)ptRActor->dwOverlayFadeTimeCurrent - 1, 0);



    if (ptACD &&
        ptRActor->tLocation.eActorLocation == AL_WORLD &&
        (ptACD->IsMonster() || ACDIsPlayer(ptRActor->idACD)))
    {
        const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
        sRActorGenerateDisturbances(ptRActor, loc);
    }

    if (ptRActor->dwFlags & (1 << RACTOR_DELETE_THIS_TICK_BIT))
    {

        RActorDelete(ptRActor->id);
        if (fLocalLock)
            gptGroupDefActor->UnlockR((void *&)ptDefActor);
        return;
    }



    if (fLocalLock)

        gptGroupDefActor->UnlockR((void *&)ptDefActor);


}

#include "../../Common/Networking/GameMessage.h"
#include "../../Common/HirelingSwig.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"

ACDID ACDCreate();  // X/Common/ActorCommonData.cpp:518
void RActorACDEffectsAdd(const ACDID idACD);  // RActorACDEffects.h
BOOL QuestIsACDObjective(const ActorCommonData & tACD);  // X/Common/Quests.cpp:2094
void CQuestsFlagACD(ActorCommonData & tACD);  // CQuests.h
void AnimPlayerIdleAnimationChanged(ActorCommonData & tACD, const uint32 dwAnimTag);  // X/Common/AnimPlayer.cpp
void CPlayersAddNecroCorpse(ActorCommonData * ptACD);  // CPlayer.h


#line 1883
static ACDID sRActorACDCreateFromEvent(
    const ACDNetworkName & ann,
    const GBHandle & hGB,
    const SNOName & tSNOName,
    const int32 nRarity,
    const uint8 nLookLinkIndex,
    const SNO snoActor,
    const SNO snoAmbientOcclusionOverrideTex,
    const BOOL bFlippyActor,
    const ActorLocation & tLocation,
    const uint32 dwFlags,
    const uint32 dwServerCreationTick,
    const BOOL fCreateActor,
    const uint32 * paLookOverrides)
{
    CGame & tCGame = CGameGet();

    CRefString sNameToUse;
    tCGame.ptRActorGlobals->dwRActorNumberCount++;
    sNameToUse.Format("%s-%d", SNOToString(ACTOR_SNO, snoActor, FALSE).str(), tCGame.ptRActorGlobals->dwRActorNumberCount);

    uint32 tNameToUse = MarkerStringHash(sNameToUse.str());

    if (snoActor == SNO_NONE)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    float flScaleToUse = 1.0f;
    const ActorDefinition * pdefActor = NULL;
    if (fCreateActor)
    {
        pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(snoActor, FALSE);

        if (tLocation.tWorldLocation.flScale <= EPSILON)
        {
            float flMinScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_MIN);
            float flDeltaScale = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SCALE_DELTA);
            flScaleToUse = RandRangeFloat_Internal(CGameGlobalsGet().rngLocal, flMinScale, flDeltaScale);
        }
        else
        {
            flScaleToUse = tLocation.tWorldLocation.flScale;
        }
    }

    ActorLocation tLocationToUse = tLocation;
    ActorLocationSetScale(tLocationToUse, flScaleToUse);

    uint32 dwRActorAddFlags = 0;
    if (dwFlags & (1 << EKF_DOESNT_WALK_BIT))
    {
        dwRActorAddFlags |= (1 << RACTOR_ADD_NO_WALK);
    }
    if (dwFlags & (1 << EKF_FORCE_REMOTE_FX))
    {
        dwRActorAddFlags |= (1 << RACTOR_ADD_FORCE_REMOTE_FX);
    }



    SWorldID idSWorld = CGameGetCurrentSWorldID();
    RActorID idRActor = RACTORID_INVALID;

    if (idSWorld != SWORLDID_INVALID &&
        tLocationToUse.eActorLocation == AL_WORLD &&
        idSWorld == tLocation.tWorldLocation.idSWorld && fCreateActor)
    {


        idRActor = sRActorCreate(
            ACDID_INVALID,
            snoActor,
            snoAmbientOcclusionOverrideTex,
            tNameToUse,
            tLocationToUse.tWorldLocation.flScale,
            FLOAT_INFINITE_LIFETIME,
            tLocationToUse.tWorldLocation.transform,
            dwRActorAddFlags,
            RENDERCONTEXT_DEFAULT,
            SSCENEID_INVALID,
            dwServerCreationTick,
            pdefActor,
            TRUE);
    }


    BOOL fIsComplex = FALSE;

    for (int32 i = HC_FIRST; i < NUM_HIRELING_CLASSES; ++i)
    {
        if (HirelingGetGBID((HirelingClass)i) == hGB.gbid)
        {
            fIsComplex = TRUE;
            break;
        }
    }

    if (!fIsComplex)
    {
        fIsComplex = PlayerClassFromGBID(hGB.gbid) != PLAYER_CLASS_NONE;
    }

    AttribGroupID idFastAttributeGroup = FastAttribLightCreate(fIsComplex ? FAT_COMPLEX_ACTOR : FAT_ACTOR);

    ACDID idACD = ACDCreate();
    ActorCommonData & tACD = ACDGet(idACD);
    tACD.Initialize(
        ann,
        idRActor,
        snoActor,
        hGB,
        tSNOName,
        tNameToUse,
        tLocationToUse,
        idFastAttributeGroup,
        (dwFlags & (1 << EKF_FORCE_INVENTORY_BIT)) != 0,
        (dwFlags & (1 << EKF_NO_COLLISION)) == 0);

    if (tACD.IsItem())
    {


        tACD.eMonsterRarity = MR_NONE;
        FastAttribSetValueInt(tACD.AttributesGetAttributeGroup(), FastAttribKey(ATTRIBUTE_ITEM_QUALITY_LEVEL), nRarity);
    }
    else
    {
        tACD.eMonsterRarity = nRarity;
    }
    tACD.nLookLinkIndex = nLookLinkIndex;

    if (!(dwFlags & (1 << EKF_SET_GROUND_TIME)))
    {
        tACD.SetHitGroundTime(0);
    }

    if (idRActor != RACTORID_INVALID)
    {
        RActor * ptRActor = RActorGet(idRActor);
        sRActorSetACD(ptRActor, idACD, pdefActor);

        int32 nNewLook = RActorLookIndexFromLookLink(pdefActor, nLookLinkIndex);
        RActorChangeLookIndex(*ptRActor, nNewLook, FALSE);

        if (paLookOverrides)
        {
            // 6: elements of EnterKnownLookOverrides::aLookOverrides (X/Common/Networking/GameMessage.h)
            for (int x = 0; x < 6; ++x)
            {
                if (paLookOverrides[x] != 0)
                {
                    RActorAddLookOverride(ptRActor, paLookOverrides[x]);
                }
            }

            int32 nLookLinkIndexScoped = RActorGetLookOverrideLookIndex(ptRActor);
            if (nLookLinkIndexScoped > 0)
            {
                RActorChangeLookIndex(*ptRActor, nLookLinkIndexScoped, TRUE);
            }
        }

        sRActorInitBannerData(ptRActor, tACD);

        sRActorAddACDEffects(ptRActor, tACD);
        sRActorSetLightFilterFlags(ptRActor);

        if (bFlippyActor)
        {
            ptRActor->dwFlags &= ~(1 << RACTOR_SHADOWS_ENABLED_BIT);
        }
    }


    ACDReapplyVisualInventory(idACD);

    if (pdefActor)
    {
        gptGroupDefActor->UnlockR((void *&)pdefActor);
    }

    return idACD;
}

#line 6756
void RActorACDEnterKnown(
    const ACDEnterKnownMessage & tACDEnterMsg)
{
    SNO snoActor = tACDEnterMsg.snoActor;


    BOOL bFlippyActor = FALSE;
    BOOL bItem = tACDEnterMsg.hGB.eType == GB_ITEMS && tACDEnterMsg.eLocationType == AL_WORLD;
    if (bItem)
    {
        ScopedActorDefinitionLockR pdefActor(snoActor);
        SNO snoFlippyActor = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_FLIPPY_SPECIAL_ACTOR);
        if (snoFlippyActor != SNO_NONE)
        {
            snoActor = snoFlippyActor;
            bFlippyActor = TRUE;
        }
    }

    ActorLocation tNewLocation;
    SigmaMemoryClear(&tNewLocation, sizeof(tNewLocation));
    tNewLocation.eActorLocation = tACDEnterMsg.eLocationType;

    BOOL fNewItem = FALSE;
    if (tNewLocation.eActorLocation == AL_WORLD)
    {


        tNewLocation.tWorldLocation.flScale = tACDEnterMsg.tWorldLocationData.Get().flScale;
        tNewLocation.tWorldLocation.idSWorld = tACDEnterMsg.tWorldLocationData.Get().idSWorld;
        tNewLocation.tWorldLocation.transform = tACDEnterMsg.tWorldLocationData.Get().transform;


        WorldPlace tLocationPlace;
        tLocationPlace.wp = tNewLocation.tWorldLocation.transform.wp;
        tLocationPlace.idSWorld = tNewLocation.tWorldLocation.idSWorld;
        tNewLocation.tWorldLocation.idSScene = SceneCommonGetSceneIDFromWorldPlace(tLocationPlace, FALSE);
    }
    else if (tNewLocation.eActorLocation == AL_INVENTORY)
    {



        tNewLocation.tInvLocation.eSlot = tACDEnterMsg.tInvLocationData.Get().eSlot;
        tNewLocation.tInvLocation.pt = tACDEnterMsg.tInvLocationData.Get().pt;
        tNewLocation.tInvLocation.idACDContainer = ACDFromANN(tACDEnterMsg.tInvLocationData.Get().annContainer);

        fNewItem = (tACDEnterMsg.dwFlags & (1 << EKF_NEW_INVENTORY_ITEM_BIT)) != 0;
    }















    SNO snoAmbientOcclusionOverrideTex =
        tACDEnterMsg.snoAmbientOcclusionOverrideTex.Exists() ?
        tACDEnterMsg.snoAmbientOcclusionOverrideTex.Get() : SNO_NONE;


    BOOL fCreateActor = TRUE;

    BOOL bLegendaryItem = bItem && tACDEnterMsg.nRarity >= IQL_LEGENDARY;
    if (!bItem || CGameLoadingScreenIsEnabled() || bLegendaryItem)
    {

        if (CGameLoadingScreenIsEnabled())
        {
            SNOPreload::WaitForClientActor(tACDEnterMsg.ann);
        }
    }
    else
    {

        XGrowableMap<ACDNetworkName, ACDNetworkName, QueuedRActorInfo, QueuedRActorInfo &> & queuedRActors = CGameGet().ptRActorGlobals->queuedRActors;
        fCreateActor = !SNOPreload::IsClientActorRequested(tACDEnterMsg.ann) || SNOPreload::IsClientActorLoaded(tACDEnterMsg.ann);
        if (!fCreateActor)
        {


            // 2: SNOFilesAsyncPriority (no enumerator recorded in DWARF)
            SNOPreload::PrefetchClientActor(snoActor, NULL, 2);

            QueuedRActorInfo tInfo;
            tInfo.dwFlags = tACDEnterMsg.dwFlags;
            tInfo.dwServerCreationTick = tACDEnterMsg.dwTimeActorCreated.Exists() ? tACDEnterMsg.dwTimeActorCreated.Get() : 0;
            tInfo.snoAmbientOcclusionOverrideTex = snoAmbientOcclusionOverrideTex;
            tInfo.flLastTimeChecked = CGameGetClientTime();
            tInfo.bIsFlippy = bFlippyActor;
            queuedRActors.SetAt_Tracked(tACDEnterMsg.ann, tInfo);
        }
        else
        {

            if (queuedRActors.LookupPtr(tACDEnterMsg.ann))
            {
                queuedRActors.RemoveAt(tACDEnterMsg.ann);
            }
        }
    }



    if (fCreateActor)
    {
        LocationSetBounds(snoActor, tNewLocation.tWorldLocation, NULL);
    }



    ACDID idACD = sRActorACDCreateFromEvent(
        tACDEnterMsg.ann,
        tACDEnterMsg.hGB,
        tACDEnterMsg.tSNOName,
        tACDEnterMsg.nRarity,
        tACDEnterMsg.nLookLinkIndex,
        snoActor,
        snoAmbientOcclusionOverrideTex,
        bFlippyActor,
        tNewLocation,
        tACDEnterMsg.dwFlags,
        tACDEnterMsg.dwTimeActorCreated.Exists() ? tACDEnterMsg.dwTimeActorCreated.Get() : 0,
        fCreateActor,
        tACDEnterMsg.tEnterKnownLookOverrides.Exists() ? tACDEnterMsg.tEnterKnownLookOverrides.Get().aLookOverrides : NULL);


    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
#line 6885
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }




    ptACD->nRefCount = 1;


    if (tACDEnterMsg.snoMarkerSet.Exists() && tACDEnterMsg.nMarkerIndex.Exists())
    {
        RActor * ptRActor = RActorTryToGet((RActorID)ptACD->idOwner);
        if (ptRActor)
        {


















            ptRActor->tMinimapInfo.snoMarkerSet = tACDEnterMsg.snoMarkerSet.Get();
            ptRActor->tMinimapInfo.nMarkerIndex = static_cast<int16>(tACDEnterMsg.nMarkerIndex.Get());
        }
    }




    BOOL fDeletedACD = RActorSetACDLocationFromMessage(*ptACD, tNewLocation, TRUE, fNewItem, fCreateActor);

    if (tNewLocation.eActorLocation == AL_INVENTORY)
    {
        UIInventory::Console::OnInventoryChanged(tNewLocation, tNewLocation, FALSE, TRUE);
        UIFollower::Console::OnInventoryChanged(tNewLocation, tNewLocation, TRUE);
        UISkills::Console::OnInventoryChanged(tNewLocation, tNewLocation, TRUE);
        UIInventory::Console::OnStashChanged(tNewLocation, tNewLocation, TRUE);
        UIInventory::Console::OnShopChanged(tNewLocation, tNewLocation, FALSE, TRUE);
        UIParty::Console::OnInventoryChanged(tNewLocation, tNewLocation, TRUE);
    }



    if (!fDeletedACD)
    {

        RActorACDEffectsAdd(idACD);

        ACDReapplyVisualInventory(idACD);

        if (ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_QUEST_MONSTER)) || QuestIsACDObjective(*ptACD))
        {
            CQuestsFlagACD(*ptACD);
        }


        const Player * ptPlayer = LocalPlayerGetPrimary();

        CrafterType eType = NPCGetCrafterType(*ptACD);
        // -1: CRAFTER_TYPE_INVALID (X/Common/Crafting/ItemCraftingSwig.h:23; not in src headers)
        if (eType != -1 &&

            (ActorGetType(ptACD->tSNOName) == ACTORTYPE_SERVER_PROP ||
             ActorDefinitionGetTagMapValueInt(TAG_ACTOR_UPGRADE_WITH_CRAFTER, snoActor)))
        {


            int32 nCrafterLevel = GetCraftLevel(eType, *ptPlayer);
            uint32 nRank = CrafterGetRank(eType, nCrafterLevel);
            AnimPlayerIdleAnimationChanged(*ptACD, CrafterGetIdleAnim(nRank));
        }




        if (ptACD->GetGizmoType() == ACTORGIZMO_LORE_CHEST)
        {
            FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CHEST_OPEN, ptPlayer->idGameConnection);
            if (ptACD->AttributesGetInt(tKey))
            {

                AnimPlayerIdleAnimationChanged(*ptACD, TAG_ACTOR_ANIMATION_OPEN);
            }
        }



        FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CLIENT_EFFECT, ptPlayer->nPlayerIndex);
        SNO snoEffect = ptACD->AttributesGetInt(tKey);
        if (snoEffect != SNO_NONE)
        {

            ptACD->AddClientEffect(snoEffect, FALSE);
        }


        if (ptACD->IsNecroCorpse())
        {

            CPlayersAddNecroCorpse(ptACD);
        }

    }
}

#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Sigma/LanguageRules.h"
#include "../../Common/UserInterface/UITextControl.h"
namespace ChromaEffects { void TriggerItemDropEffect(const RGBAColor & rgbaColor); }  // X/Client/Chroma/ChromaEffects.cpp:311
BOOL RActorAnimTreeLayerPlay(RActor * ptRActor, AnimLayer eAnimLayer, const uint32 dwTag, BOOL fOnlyPlayIfDifferent);  // X/Client/RActors/RActorAnimation.cpp:2921
void RActorAttachmentDetach(const RActorID idRActor, const AttachmentID idAttachment);  // X/Client/RActors/RActorAttachments.cpp:775
namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }  // X/Client/UserInterface/Interface/UIGame.cpp:6870
namespace ChromaEffects { void TriggerLevelUpEffect(); }  // X/Client/Chroma/ChromaEffects.cpp:290
namespace UIGame { void ShowLevelUpButton(ACDID idACDPlayer); }  // X/Client/UserInterface/Interface/UIGame.cpp:2245
void CComplexEffectRemove(const ACDID idACD, const SComplexEffectID idSComplexEffect);  // X/Client/RActors/CComplexEffect.cpp:321
void RActorDeleteDynamicAttachments(RActor & tRActor);  // X/Client/RActors/RActorAttachments.cpp:827
namespace UIGameSettings
{
namespace Console
{
BOOL CanLowerGameDifficulty();  // X/Client/UserInterface/Console/UIConsoleGameSettings.cpp:926
}
}
namespace UIHUD
{
namespace Console
{
void UpdatePlayerIndicator(const Player * ptPlayer, BOOL bReapply);  // X/Client/UserInterface/Console/UIConsoleHUD.cpp:1601
}
}
void UI_Print(LPCSTR szText, const BOOL fEchoToConsole);  // X/Client/UserInterface/UI.cpp:3860
void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eItemEffectType, int32 & nEffectLevel, const BOOL bWantOverrides, const BOOL bForTooltip);  // X/Common/VisualInventory.cpp:877

#line 9241
static void sRActorAttachLoopingSound(
    RActor * ptRActor,
    const uint32 dwID,
    const SNO snoSound)
{
		TriggerEvent tTriggerEvent;


		TriggerEventInit(tTriggerEvent);
		tTriggerEvent.eType = TRIGGEREDEVENTTYPE_OUTRO_OBJECT;
		tTriggerEvent.dwID = dwID;
		RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);

	if (snoSound != SNO_NONE)
	{
		TriggerEventInit(tTriggerEvent);
		tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
		tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_SOUND;
		tTriggerEvent.snoname.Set(SOUND_SNO, snoSound);
		tTriggerEvent.dwID = dwID;
		RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
	}
}

#line 9268
static void sRActorPlayHitSound(
    const RActor * ptRActor,
    HitSoundType nHitSoundType)
{
	if (nHitSoundType == -1)  // no HITSOUND_ enumerator is -1 (PowersCommon.h:44-56): literal from the cmn #1 at +0x840
		return;




    SNO snoHitSound;
    if (nHitSoundType == HITSOUND_IMMUNE)  // cmp #9 at +0x848 has line 0
    {
        snoHitSound = GlobalSNOGet(GLOBALSNO_SOUND_COMBAT_IMMUNE_WOOSH);
    }
    else
    {
        ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
        snoHitSound = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_FIRST_HIT_BY + nHitSoundType - 1);
    }

    if (snoHitSound != SNO_NONE)
    {
        sRActorPlaySound(ptRActor, snoHitSound);
    }
}

#line 9315
static void sRActorPlayHaloSound(
    const RActor * ptRActor)
{
    SNO snoHaloSound = SNO_NONE;
   ActorCommonData & tACD = ACDGet(ptRActor->idACD);

    if (tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CURRENCY_BLOODSHARDS) ||
        tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CURRENCY_PLATINUM) ||
        tACD.hGB.gbid == GlobalGBIDGet(GLOBALGBID_CURRENCY_PLATINUM_UNCAPPED))
    {
        return;
    }


    ItemQualityLevel eIQL = ItemGetDisplayedQualityLevel(tACD);
    if (tACD.IsItem() && ItemHasLabel(tACD.hGB.gbid, ITL_COLLECTIBLE))
    {
        snoHaloSound = GlobalSNOGet(GLOBALSNO_SOUND_COLLECTIBLE_LANDING);
    }
    else if ((eIQL >= IQL_MAGIC_AFFIX_START && eIQL <= IQL_MAGIC_AFFIX_END) || eIQL == IQL_SPECIAL)
    {
        snoHaloSound = GlobalSNOGet(GLOBALSNO_SOUND_MAGIC_LANDING);
    }

    else if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
    {
        snoHaloSound = GlobalSNOGet(GLOBALSNO_SOUND_RARE_LANDING);
    }
    else if (eIQL == IQL_DISPLAY_AS_SET_ITEM)
    {
        if (ItemHasLabel(tACD.hGB.gbid, ITL_CRAFTING_PLAN_LEGENDARY))
        {
            snoHaloSound = GlobalSNOGet(GLOBALSNO_SOUND_RARE_LANDING);
        }
        else
        {

			ChromaEffects::TriggerItemDropEffect(ItemGetDisplayNameColor(IQL_DISPLAY_AS_SET_ITEM));
            snoHaloSound = GlobalSNOGet(GLOBALSNO_SOUND_SET_LANDING);
        }
    }
    else if (eIQL == IQL_LEGENDARY)
    {
        if (ItemHasLabel(tACD.hGB.gbid, ITL_CRAFTING) ||
            ItemHasLabel(tACD.hGB.gbid, ITL_ORNAMENT) ||
            ItemHasLabel(tACD.hGB.gbid, ITL_TIERED_RIFT_KEY) ||
            ItemHasLabel(tACD.hGB.gbid, ITL_CRAFTING_PLAN_LEGENDARY))
        {
            snoHaloSound = GlobalSNOGet(GLOBALSNO_SOUND_RARE_LANDING);
        }
        else
        {

			ChromaEffects::TriggerItemDropEffect(ItemGetDisplayNameColor(IQL_LEGENDARY));
            snoHaloSound = GlobalSNOGet(GLOBALSNO_SOUND_LEGENDARY_LANDING);
        }
    }






    sRActorPlaySound(ptRActor, snoHaloSound);
}

#line 9382
static void sRActorPlayFlinchAnimation(
    RActor * ptRActor)
{
    if (RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {
        RActorAnimTreeLayerPlay(ptRActor, ANIM_LAYER_FLINCH, TAG_ACTOR_ANIMATION_GETHIT, FALSE);
    }
}

#line 9394
AttachmentID RActorAttachParticleSystem(
    RActorID idRActor,
    const SNO snoParticle,
    const BOOL fAttachToChest)
{
	RActor * ptRActor = RActorGet(idRActor);
	TriggerEvent tTriggerEvent;
	TriggerEventInit(tTriggerEvent);
	tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
	tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_PARTICLESYSTEM;
	tTriggerEvent.snoname.Set(PARTICLE_SNO, snoParticle);

    if (fAttachToChest)
        tTriggerEvent.tHardpointLinks[0].tHash = CHEST_HARDPOINT_NAME;

	return RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
}

#line 9431
static SoundID sRActorPlayTaggedSound(
    RActor * ptRActor,
    const uint32 dwTag)
{
	ScopedActorDefinitionLockR pDefActor(ptRActor->snoActor);
	SNO snoSound = TagMapGetSNO(pDefActor->hTagMap, dwTag);

    return sRActorPlaySound(ptRActor, snoSound);
}

#line 9444
static void sRActorHandleDeathEffects(
    RActor * ptRActor)
{
    RActorSendMsgKey(ptRActor->id, KeyedMessage(MSGKEY_RESERVED_DEAD, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);

    if (ptRActor->ptCActorMovement)
    {
        ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
        TiltType nTiltType = (TiltType) TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_TILT_TYPE_ON_DEATH);

        ptRActor->ptCActorMovement->SetTilt(nTiltType, FALSE);
    }

    sRActorDeleteACDEffectsOnDeath(ptRActor);
    RActorResetActorEffectsForAttributes(ptRActor);

	CWorld *ptCWorld = CWorldGetFromSWorldID( CGameGetCurrentSWorldID() );
    SNO snoIrradianceToUse = (ptCWorld->snoIrradianceTexDead != SNO_NONE) ? ptCWorld->snoIrradianceTexDead : GlobalSNOGet(GLOBALSNO_RACTOR_DEAD_IRRADIANCE_TEXTURE);
    RActorSetIrradianceMap(ptRActor, snoIrradianceToUse);

	UIConversation::CancelLineForRActor(ptRActor->id);

    ptRActor->flDeathTime = CGameGetClientTime();
}

#line 9750
// NON_MATCHING: 5800/5800 bytes, 29/1450 instructions differ. The compiler places the item-effect call after the halo-sound quality tests at +0x1498. The original places it before them.
void RActorPlayEffectFromMessage(
    const RActorID idRActor,
    const EffectType eEffectType,
    const int32 nEffectParameter,
    const BOOL bRemoteEffect)
{
    RActor * ptRActor = RActorTryToGet(idRActor);

    // range check at +0x98 (cmp #0x4d; b.hi) sits at the first || ; the NULL test is a line-0 cbz
	if (eEffectType <= EFFECT_INVALID || eEffectType >= EFFECT_NUM_EFFECTS || !ptRActor)
		return;





    Player * ptPlayer;

    ptPlayer = PlayerGetByACD(ptRActor->idACD);


	switch (eEffectType)
	{




        case EFFECT_HIT:
        {
            float flCurrTime = CGameGetClientTime();
    if (flCurrTime != ptRActor->flTimeLastHit)
            {
					ptRActor->flTimeLastHit = flCurrTime;
					RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_HIT, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);

                    if (ptRActor->flTimeLastFlinch + FLINCH_TIMEOUT < flCurrTime)
                    {
                        ptRActor->flTimeLastFlinch = flCurrTime;
                        sRActorPlayFlinchAnimation(ptRActor);
                    }
            }
        }
            break;

        case EFFECT_HIT_LOCAL:
            RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_HIT_LOCAL, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
            break;

        case EFFECT_BLOCKED_LOCAL:
			sRActorPlaySound(ptRActor, GlobalSNOGet(GLOBALSNO_SOUND_BLOCKED_LOCAL));
            break;

        case EFFECT_DODGED_LOCAL:
			sRActorPlaySound(ptRActor, GlobalSNOGet(GLOBALSNO_SOUND_DODGED_LOCAL));
            break;

        case EFFECT_LOCAL_WIZARD_POWERED_IMPACT_MANA_FLASH:
			UIGame::AddHUDUntrackedEffectGroup(GlobalSNOGet(GLOBALSNO_EFFECTGROUP_POWERED_IMPACT_MANA_FLASH));
            break;

        case EFFECT_DEATH_FIRE:
            RActorApplyOverlay(idRActor, OVERLAYTYPE_FIRE_DEATH);
            sRActorHandleDeathEffects(ptRActor);
            break;

        case EFFECT_DEATH_COLD:
            RActorApplyOverlay(idRActor, OVERLAYTYPE_COLD_DEATH);
            sRActorHandleDeathEffects(ptRActor);
            break;

        case EFFECT_DEATH_LIGHTNING:
            RActorApplyOverlay(idRActor, OVERLAYTYPE_LIGHTNING_DEATH);
            sRActorHandleDeathEffects(ptRActor);
            break;

        case EFFECT_DEATH_ARCANE:
            RActorApplyOverlay(idRActor, OVERLAYTYPE_ARCANE_DEATH);
            sRActorHandleDeathEffects(ptRActor);
            break;

        case EFFECT_DEATH_POISON:
            RActorApplyOverlay(idRActor, OVERLAYTYPE_POISON_DEATH);
            sRActorHandleDeathEffects(ptRActor);
            break;

        case EFFECT_DEATH_PLAGUED:
            RActorApplyOverlay(idRActor, OVERLAYTYPE_PLAGUE_DEATH);
            sRActorHandleDeathEffects(ptRActor);
            break;
        // case labels without rows of their own: the jump table sends 1, 27, 49, 52, 55, 58 to the shared sRActorHandleDeathEffects call
        case EFFECT_DEATH:
        case EFFECT_DEATH_DISINTEGRATE:
        case EFFECT_DEATH_PULVERIZE:
        case EFFECT_DEATH_HOLY:
        case EFFECT_DEATH_LAVA:
        case EFFECT_DEATH_SPIRIT:
            sRActorHandleDeathEffects(ptRActor);
            break;



        case EFFECT_DEAD:
			sRActorStartCorpseFadeTimer(ptRActor, NULL);
            break;


        case EFFECT_RECREATE_DEAD:
    RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_RECREATE_DEAD, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);

                if (ACDIsPlayer(ptRActor->idACD))
                {


                    ptRActor->dwFlags |= (1 << RACTOR_HIDE_CLIENT_SIDE);
                }

                if (ptRActor->ptCActorMovement)
                {
                    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
                    TiltType nTiltType = (TiltType) TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_TILT_TYPE_ON_DEATH);

                    ptRActor->ptCActorMovement->SetTilt(nTiltType, FALSE);
                }
    sRActorDeleteACDEffectsOnDeath(ptRActor);
            break;


        case EFFECT_PROJECTILE_EFFECT:
        {
            ActorCommonData *ptACD = ACDTryToGet(ptRActor->idACD);
      if (ptACD && nEffectParameter != SNO_NONE)
            {
                SNO snoEffectGroup = nEffectParameter;


                    EffectGroupCreateUntracked(snoEffectGroup, ptRActor->id, 0, FALSE, bRemoteEffect);
            }
        }
            break;


        case EFFECT_CHANGE_DAMAGE_STATE:
            if (nEffectParameter > 0)
            {
          RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_DAMAGE_STATE_1 + nEffectParameter - 1, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
            }
            break;

        case EFFECT_RECREATE_DAMAGE_STATE:
            if (nEffectParameter > 0)
            {
          RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_RECREATE_DAMAGE_STATE_1 + nEffectParameter - 1, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
            }
            break;

        case EFFECT_CRITICAL_HIT:
        {
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRITICAL_HIT_EFFECTGROUP);


           ACDID idACDAttacker = ACDFromANN((ACDNetworkName)nEffectParameter);
    if (ACDIsPlayer(idACDAttacker))
            {
                RActorID idRActorAttacker = (RActorID)ACDGetOwner(idACDAttacker);
                    RActorApplyOverlay(idRActorAttacker, OVERLAYTYPE_CRITICAL_HIT_BLOOD);
            }
        }
            break;

        case EFFECT_TRIGGER_ACTOR_ATTACHMENT:
            RActorSendMsgKey(idRActor, KeyedMessage(nEffectParameter, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);
            break;

        case EFFECT_PLAY_HITSOUND:
            sRActorPlayHitSound(ptRActor, nEffectParameter);
            break;

        case EFFECT_PLAY_LANDING_SOUND:
            sRActorPlayLandingSound(ptRActor);
            break;

        case EFFECT_PLAY_HALO_SOUND:
            sRActorPlayHaloSound(ptRActor);
            break;

        case EFFECT_PLAY_PICKUP_SOUND:
			sRActorPlaySound(ptRActor, GlobalSNOGet(GLOBALSNO_SOUND_PICKUP));
            break;



        case EFFECT_PLAY_POTION_AUTOPICKUP_SOUND:
            sRActorPlaySound(ptRActor, GlobalSNOGet(GLOBALSNO_SOUND_POTION_AUTOPICKUP));
            break;

        case EFFECT_BLOOD_SHARD_ON_PICKUP:
            EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_PICKUP_BLOODSHARD), idRActor, 0, FALSE, bRemoteEffect);
            break;

        case EFFECT_PICKUP_GOLD:
            EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_PICKUP_GOLD), idRActor, 0, FALSE, bRemoteEffect);
            break;

        case EFFECT_CANTPICKUP:
            UIGame::DisplayError(GameErrorData(GAMEERROR_PICKUP_FAILED), FALSE, ptPlayer);
            break;
        case EFFECT_CANTPICKUP_CURRENCY:
            UIGame::DisplayError(GameErrorData(GAMEERROR_PICKUP_NO_SUITABLE_SLOT), FALSE, ptPlayer);
            break;
        case EFFECT_CANTPICKUP_NO_SUITABLE_SLOT:
            UIGame::DisplayError(GameErrorData(GAMEERROR_PICKUP_NO_SUITABLE_SLOT), FALSE, ptPlayer);
         gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_INVENTORY_FULL), NULL, TRUE);
            break;
        case EFFECT_CANTPICKUP_SINGULAR:
            UIGame::DisplayError(GameErrorData(GAMEERROR_PICKUP_ONLY_ONE_ALLOWED), FALSE, ptPlayer);
            break;

        case EFFECT_CANTPICKUP_SOULBOUND:
            UIGame::DisplayError(GameErrorData(GAMEERROR_PICKUP_BELONGS_TO_OTHER), FALSE, ptPlayer);
            break;

        // no rows: these three jump-table entries go straight to the function exit (+0xfe8)
        case EFFECT_OPEN_CHEST:
        case EFFECT_KNOCK_BACK:
        case EFFECT_PLAYER_DETECTED:
            break;

        case EFFECT_NECRO_CORPSE_SPAWN:
        {   SNO snoEffectGroup = nEffectParameter;
            EffectGroupCreateUntracked(snoEffectGroup, idRActor, 0, FALSE, FALSE);
            break;
        }

        case EFFECT_PLAYER_RESURRECT:
        {
            RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_PLAYER_RESURRECT, 0.0f, Shape::DEFAULT_MATERIAL), MSGPASS_TO_ACTORCHAIN);

            ptRActor->dwFlags &= ~(1 << RACTOR_HIDE_CLIENT_SIDE);
            ptRActor->dwFlags &= ~(1 << RACTOR_HIDE_BIT);
            ptRActor->dwFlags |= (1 << RACTOR_SHADOWS_ENABLED_BIT);

   ptRActor->flAlpha = ptRActor->flFadeAlpha = 1.0f;
   ptRActor->snoIrradianceTex = SNO_NONE;

            RActorResetDeathFadeTimes(ptRActor);
            ptRActor->flLife = FLOAT_INFINITE_LIFETIME;

            sRActorOverlayReset(*ptRActor);
			RActorDeleteDynamicAttachments(*ptRActor);
            ptRActor->tTilt.Clear();
            ptRActor->tHitJitter.Clear();
            ptRActor->tSink.Clear();

            {
                CGame & tCGame = CGameGet();
                float flTime = CGameGetRealTime();
                const float TIME_BEFORE_PROMPTING_TO_LOWER_DIFFICULTY = 90.0f;
                if (tCGame.ptRActorGlobals->flLastDeathTime != 0.0f &&
                    flTime - tCGame.ptRActorGlobals->flLastDeathTime <= TIME_BEFORE_PROMPTING_TO_LOWER_DIFFICULTY)
                {






                    if (UIGameSettings::Console::CanLowerGameDifficulty())
                    {
                     gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_LOWER_DIFFICULTY), ptPlayer, TRUE);
                    }
                }

                tCGame.ptRActorGlobals->flLastDeathTime = flTime;
            }

            POS pos = ptRActor->tAttachmentList.GetHeadPosition();
            while (pos)
            {
                               AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
               Attachment * ptAttachment = AttachmentTryToGet(idAttachment);

                if (ptAttachment && ptAttachment->snoname.snoGroup == ACTOR_SNO)
                {
                    RActorID idRActorAttached = (RActorID) ptAttachment->idAttachedObject;
                    RActor * ptRActorAttached = RActorTryToGet(idRActorAttached);
                    if (ptRActorAttached)
                    {
                        sRActorOverlayReset(*ptRActorAttached);
                    }
                }
            }

			RActorUpdateClientTransform(ptRActor, NULL, NULL);


   ptRActor->tRMsgList.DeleteAll();


            ActorCommonData *ptACD = ACDTryToGet(ptRActor->idACD);
            if (ptACD)
            {
                HirelingClass eClass = ACDGetHirelingClass(*ptACD);
                if (eClass != HC_NONE)
                {
                    CHirelingUpdateAppearance(*ptACD, ptRActor, eClass);
                }
                else
                {

                    UIHUD::Console::UpdatePlayerIndicator(PlayerGetByACD(ptRActor->idACD), TRUE);

                    ScopedActorDefinitionLockR ptDefActor(ptRActor->snoActor);
                    uint8 nLookLinkIndex = ActorChooseLookLink(ptDefActor, CGameGlobalsGet().rngLocal);
                    uint32 nLookIndex = RActorLookIndexFromLookLink(ptDefActor, nLookLinkIndex);
                    RActorChangeLookIndex(*ptRActor, nLookIndex, FALSE);
                    ACDReapplyVisualInventory(ptRActor->idACD);
                }
            }
        }
            break;


        case EFFECT_LEVELUP:
			ChromaEffects::TriggerLevelUpEffect();
			UIGame::ShowLevelUpButton(ptRActor->idACD);
            break;



        case EFFECT_ALT_LEVELUP:
   ChromaEffects::TriggerLevelUpEffect();
            UIGame::ShowAltLevelUpButton(ptRActor->idACD);
            break;



        case EFFECT_CRAFTER_LEARN_RECIPE:
            EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_CRAFTER_LEARN_RECIPE), idRActor, 0, FALSE, bRemoteEffect);
            break;




        case EFFECT_HEAL:
   RActorAttachParticleSystem(idRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_HEAL), TRUE);
            break;



        case EFFECT_HEAL_MANA:
            RActorAttachParticleSystem(idRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_HEAL_MANA), TRUE);
            break;



        case EFFECT_REJUVE:
			sRActorPlaySound(ptRActor, GlobalSNOGet(GLOBALSNO_SOUND_HEAL));
   RActorAttachParticleSystem(idRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_REJUVE), TRUE);
            break;



        case EFFECT_SHRINE:
			sRActorPlaySound(ptRActor, GlobalSNOGet(GLOBALSNO_SOUND_SHRINE));
   RActorAttachParticleSystem(idRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_SHRINE), TRUE);
            break;



        case EFFECT_IDENTIFY:
			sRActorPlaySound(ptRActor, GlobalSNOGet(GLOBALSNO_SOUND_SHRINE));
   RActorAttachParticleSystem(idRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_IDENTIFY), FALSE);
            break;


        case EFFECT_CREATE_PARTICLE:
        case EFFECT_ELECTRIC_CHARGE:
            RActorAttachParticleSystem(idRActor, nEffectParameter, TRUE);
            break;



        case EFFECT_PLAY_SOUND:
            sRActorPlaySound(ptRActor, nEffectParameter);
            break;









        case EFFECT_SHIELD_BLOCK:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_SHIELD_BLOCK_EFFECTGROUP);
            break;



        case EFFECT_CRIT_DEATH_PHYSICAL:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRIT_DEATH_PHYSICAL);
            break;



        case EFFECT_CRIT_DEATH_FIRE:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRIT_DEATH_FIRE);
            break;



        case EFFECT_CRIT_DEATH_POISON:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRIT_DEATH_POISON);
            break;



        case EFFECT_CRIT_DEATH_ARCANE:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRIT_DEATH_ARCANE);
            break;



        case EFFECT_CRIT_DEATH_HOLY:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRIT_DEATH_HOLY);
            break;



        case EFFECT_CRIT_DEATH_LIGHTNING:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRIT_DEATH_LIGHTNING);
            break;



        case EFFECT_CRIT_DEATH_COLD:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_CRIT_DEATH_COLD);
            break;



        case EFFECT_DEATH_RARE_MONSTER:
            sRActorPlayTaggedEffectGroup(ptRActor, TAG_ACTOR_EFFECT_DEATH_RARE_MONSTER);
            break;


        case EFFECT_WARP_IN_PORTAL:
        {
   AttachmentID idAttachment = RActorAttachParticleSystem(idRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_WARP_IN_PORTAL), TRUE);
            RActorAttachmentDetach(idRActor, idAttachment);
            break;
        }

        case EFFECT_WARP_IN_WAYPOINT:
        {
   AttachmentID idAttachment = RActorAttachParticleSystem(idRActor, GlobalSNOGet(GLOBALSNO_PARTICLE_WARP_IN_WAYPOINT), TRUE); RActorAttachmentDetach(idRActor, idAttachment);
            break;
        }



        case EFFECT_WARP_IN_COMPANION:
            EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_WARP_IN_COMPANION), idRActor, 0, FALSE, bRemoteEffect);
            break;



        case EFFECT_COSMETIC_PET_WARP_OUT:
            EffectGroupCreateUntracked(GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_COSMETIC_PET_WARP_OUT), idRActor, 0, FALSE, bRemoteEffect);
            break;





        case EFFECT_REMOVE_COMPLEX_EFFECT:
        {
            SComplexEffectID idSComplexEffect = nEffectParameter;
			CComplexEffectRemove( ptRActor->idACD, idSComplexEffect );
            break;
        }

        case EFFECT_PLAY_EFFECT_GROUP:
        {
            SNO snoEffectGroup = nEffectParameter;
			EffectGroupCreateUntracked(snoEffectGroup, idRActor, 0, FALSE, bRemoteEffect);
            break;
        }


        case EFFECT_NPC_OPERATE_LOCAL:
                SoundDelete(ptRActor->idTalkSound);
                ptRActor->idTalkSound = sRActorPlayTaggedSound(ptRActor, TAG_ACTOR_NPC_OPERATE_SOUND);
            break;


        case EFFECT_ITEM_ELEMENTAL_EFFECT:
        {
   ActorCommonData *ptACD = ACDTryToGet(ptRActor->idACD);
            if (!ptACD)
            {

    ptRActor->eItemEffectType = ITEM_EFFECT_TYPE_NONE; ptRActor->nItemEffectLevel = -1;
            }
            else
            {
             ACDGetItemEffectTypeAndLevel(*ptACD, ptRActor->eItemEffectType, ptRActor->nItemEffectLevel, TRUE, FALSE);
            }
            RActorAddWeaponElementalEffects(ptRActor);
            break;
        }


        case EFFECT_INTERRUPTED:
            if (PlayerIsLocal(ptRActor->idACD))
            {
                CPlayerInputCancelMode(PlayerGetByACD(ptRActor->idACD), FALSE);
            }
            break;


        case EFFECT_GATE_OPENING:
        {
   ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
			SNO snoSound = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_GATE_OPENING_SOUND);
            sRActorAttachLoopingSound(ptRActor, INTERNAL_ATTACHMENTID_GATE_SOUND, snoSound);
            break;
        }

        case EFFECT_GATE_CLOSING:
        {
   ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
			SNO snoSound = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_GATE_CLOSING_SOUND);
            sRActorAttachLoopingSound(ptRActor, INTERNAL_ATTACHMENTID_GATE_SOUND, snoSound);
            break;
        }

        case EFFECT_GATE_OPENED:
            sRActorPlayTaggedSound(ptRActor, TAG_ACTOR_GIZMO_GATE_OPENED_SOUND);
            break;

        case EFFECT_GATE_CLOSED:
            sRActorPlayTaggedSound(ptRActor, TAG_ACTOR_GIZMO_GATE_CLOSED_SOUND);
            break;

        case EFFECT_SINK:
            ptRActor->tSink.Start(nEffectParameter);
            break;

        case EFFECT_PROJECTILE_BOUNCE:
            if (ptRActor->ptCActorMovement)
            {
                AppGlobals & tAppGlobals = sgtAppGlobals;
                if (tAppGlobals.ptCGameActive)
                {
                    ptRActor->ptCActorMovement->Bounced();
                }
            }
            break;


        case EFFECT_SAVEPOINT_ACTIVATED:

           if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            {



            CRefString usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "GameSavedMsg");
            CRefString sAfterLanguageRules = LanguageRules::ApplyLanguageRules(usMessage.str());

		       Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);
		       UI_Print(sAfterLanguageRules, FALSE);

            UIGame::ShowSavePointButton();
        }
            break;

        default:
			DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            break;
	}
}
