// X/Client/RActors/RActorAnimation.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/p2-ractor/anim/frag.cpp by build/try/p2-ractor/gen.py.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "RActorAnimation.h"
#include "RActors.h"
#include "../Animation.h"
#include "../CGame.h"
#include "../LookAt.h"
#include "../BonePulse.h"
#include "../Physics/Ragdoll.h"
#include "../Physics/PartialRagdoll.h"
#include "../Physics/BreakableRagdoll.h"
#include "../../Common/GameCommonData.h"
#include "../../Sigma/Random.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Player.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Target.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../CWorld.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/TagMaps/AnimTagsSwig.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/AnimationDefinition.h"
#include "../../Common/Definitions/AnimTreeDefinition.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/Memory/SigmaMemory.h"

#include "../../Common/AnimCommon.h"







// Defined in X/Client/Animation.cpp (no shared declaration yet).
void AnimationSetChannelPlayParams(AnimChannelPlayParams & tChannelPlayParams, const AnimationDefinition * pdefAnim, RNG & rng);
SNO AnimTreeLayerGetSNO(const AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash);
void AnimTreeLayerPlay(RActor * ptRActor, uint32 dwLayerHash, const AnimChannelPlayParams & tChannelPlayParams, BOOL bImmediatelyActive, BOOL bManualRampOut);
void AnimationInitializeForGame();
void AnimationBoneCacheInit(AnimBoneCache & tCache, int nBoneCount, int nNumBoneCacheEntries, const BoneStructure * ptBoneStructure);
void AnimationChannelInit(AnimChannel & tChannel, const int32 nBoneCount, const BoneStructure * ptBoneStructure, const float flTranslationScale, SNO snoAppearance);
void AnimationChainInit(RActorID idRActor);  // X/Client/RActors/AnimationChain.cpp
void AnimationChannelDispose(AnimChannel & tChannel);
void AnimationBoneCacheDispose(AnimBoneCache & tCache);
void AnimationChainDispose(RActorID idRActor);  // X/Client/RActors/AnimationChain.cpp
void AnimationDisposeForGame();
void AnimationChannelSetPercentComplete(AnimChannel & tChannel, float flFraction);
void AnimationChannelSetSpeedScale(AnimChannel & tChannel, const float flScale);
void AnimationChannelScaleToVelocity(AnimChannel & tChannel, const WorldDistance wdMag);
void AnimationChannelUpdate(AnimChannel & tChannel, AnimBoneCache & tCache, float flDeltaTime);
void AnimTreeUpdate(RActor * ptRActor, const AnimTreeDefinition & tTreeDefinition, const AnimTreeControl & tControl, AnimTree & tTree, float flDeltaTime);
void AnimTreeInit(AnimTree & tTree, int32 nBoneCount, const BoneStructure * ptBoneStructure, const float flTranslationScale, SNO snoAppearance, SNO snoAnimSet, SNO snoAnimTreeDefinition, uint32 dwAnimTagDefault, WeaponClass eWeaponClassDefault);
void AnimTreeDispose(AnimTree & tTree);
BOOL AnimTreeDefinitionHasLayer(const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash);
void AnimTreeLayerScaleToVelocity(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash, const WorldDistance wdMag);
BOOL AnimTreeLayerHasManualBlendRampOut(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash);
void AnimTreeLayerRampOutBlend(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash);
PlaybackMode AnimTreeLayerGetPlaybackMode(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash);
BOOL AnimTreeLayerIsActive(AnimTree & tTree, const AnimTreeDefinition & tTreeDefinition, uint32 dwLayerHash);
int32 HardpointIndexFromName(const SNO snoAppearance, HardpointHash szName);  // X/Client/Definitions/CAttachmentDefinition.cpp:19
int32 HardpointIndexFromName(const AppearanceDefinition * pdefAppearance, HardpointHash szName);  // X/Client/Definitions/CAttachmentDefinition.cpp:30
uint32 StringHashI(LPCSTR pszName);
uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim);  // X/Common/Powers/PowersCommon.cpp:8013  // X/Sigma/TypeDescriptor/SymbolicValue.cpp:63

void RActorAnimationEnableAnimTree(RActor & tRActor, BOOL Enable, SNO snoAnimTree);
void RActorUpdateBones(RActor * ptRActor, float flDeltaTime, const ActorDefinition * pdefActor, BOOL fForceUpdate);
BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor);
void RActorAnimTreeUpdate(RActor & tRActor, float flDeltaTime);
uint32 RActorGetAnimTreeLayerHash(AnimLayer eAnimLayer);
BOOL RActorGetAnimTreeLayerImmediatelyActive(AnimLayer eAnimLayer);
BOOL RActorGetAnimTreeManualRampOut(RActor * ptRActor, uint32 dwAnimationTag);
void RActorAnimTreeLayerRampOutBlend(RActor & tRActor, AnimLayer eAnimLayer);
void RActorAnimTreeLayerRampOutBlendForManual(RActor & tRActor, AnimLayer eAnimLayer);
void RActorAnimTreeLayerRampOutBlendForLooping(RActor & tRActor, AnimLayer eAnimLayer);

class EditModeAnimCache {  // size 0x1
public:
    void Initialize();
    void Dispose();
    BOOL UseCache(RActor *);
    ActorAnim * LookupAnim(SNO);
    void AddAnim(SNO, ActorAnim *);
    BOOL RemoveAnim(const ActorAnim *);
};

struct RActorAnimGlobals {  // size 0x180
    XChainedFixedBlockAllocator<ActorAnim> poolActorAnim;  // 0x00
    EditModeAnimCache tAnimCache;  // 0x30
    XGrowableMap<unsigned int, unsigned int, AnimLayer, AnimLayer> tPlayerTagToLayer;  // 0x38
    XGrowableMap<unsigned int, unsigned int, AnimLayer, AnimLayer> tNPCTagToLayer;  // 0xb8
    uint32 dwAnimLayerHash[17];  // 0x138
};
static_assert(sizeof(RActorAnimGlobals) == 0x180, "RActorAnimGlobals");

void sInitPlayerTagLayerMap(RActorAnimGlobals * ptRActorAnimGlobals);
void sInitNPCTagLayerMap(RActorAnimGlobals * ptRActorAnimGlobals);


// Original Animation.cpp and AnimCommon.cpp declarations, signatures verified in DWARF.
BOOL AnimationChannelIsAnimating(const AnimChannel &, const AnimBoneCache &);
BOOL AnimationTreeIsAnimating(AnimTree &, const AnimTreeControl &, const AnimTreeDefinition &);
BOOL AnimationChannelNeedsInterpolation(const AnimChannel &);
BOOL AnimationTreeNeedsInterpolation(AnimTree &, const AnimTreeControl &, const AnimTreeDefinition &);
void AnimationChannelInterpolateBones(AnimChannel &, AnimBoneCache &, int32, PRSTransform *&);
void AnimTreeBlendBones(const AnimTreeDefinition &, const AnimTreeControl &, AnimBoneStack &, AnimTree &);
BOOL AnimTreeLayerIsAnimating(AnimTree &, const AnimTreeDefinition &, uint32);
SNO AnimationChannelGetSNO(const AnimChannel &);
void AnimationChannelPlay(RActor *, const AnimChannelPlayParams &, BOOL);
void AnimTreeLayerSetPoseFromLayer(RActor *, uint32, uint32);
BOOL RActorGetAnimLayerForTag(RActor *, uint32, AnimLayer &);
BOOL RActorAnimTreeLayerIsActive(const RActor &, AnimLayer);
void AnimTagGetAdditionalTag(uint32, uint32 &);

// Original SNOFiles.cpp / Animation.cpp signatures from indexed DWARF.
void SNOFileGetFilePath(const SNOName &, char *, int32);
void ProcessBones(RActor *, ProcessedBone *, AnimationController **, const PRSTransform *, const BoneStructure *, int32, float);
void ProcessBoneMatrices(ProcessedBone *, ProcessedBoneSkin *, Vector3D &, Vector3D &, const BoneStructure *, int32);

// Animation.cpp / ACDWorld.cpp indexed declarations.
float AnimTreeGetLayerPercentComplete(const AnimTree &, const AnimTreeDefinition &, uint32);
BOOL ACDIsInTown(const ActorCommonData &);

WorldPlace RActorGetWorldPlace(const RActor *);











































































































































































































void sInitPlayerTagLayerMap(RActorAnimGlobals * ptRActorAnimGlobals)
{

    XGrowableMap<unsigned int, unsigned int, AnimLayer, AnimLayer> & tTagToLayer = ptRActorAnimGlobals->tPlayerTagToLayer;
    tTagToLayer.Initialize(16, FALSE, NULL);


    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RUN, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RUNSTART, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RUNEND, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TOWNRUN, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TOWNRUNSTART, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TOWNRUNEND, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_WALK, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_WALKSTART, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_WALKEND, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TOWNWALK, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TOWNWALKSTART, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TOWNWALKEND, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPRINT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPRINTSTART, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPRINTEND, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TURN, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TURN_LEFT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TURN_RIGHT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_WALK_01, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_WALK_01_START, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_WALK_01_END, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_WALK_02, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_WALK_02_START, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_WALK_02_END, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_RUN_OUTRO_LEFTFOOT_START, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_RUN_OUTRO_LEFTFOOT_MID, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_RUN_OUTRO_RIGHTFOOT_START, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_RUN_OUTRO_RIGHTFOOT_MID, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_EMOTE_IDLE_SHUFFLE_LEFT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_EMOTE_IDLE_SHUFFLE_RIGHT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_WALK_OUTRO_LEFTFOOT_START, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_WALK_OUTRO_RIGHTFOOT_START, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_SPRINT_LEFTFOOT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_SPRINT_RIGHTFOOT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_RUN_LEFTFOOT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_RUN_RIGHTFOOT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_WALK_LEFTFOOT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_WALK_RIGHTFOOT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFE, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFELEFT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFERIGHT, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFE_START, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFELEFT_START, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFERIGHT_START, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFE_END, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFELEFT_END, ANIM_LAYER_NONE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRAFERIGHT_END, ANIM_LAYER_NONE);


    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NEUTRAL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_01, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_ALERT_TO_UNALERT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STUNNED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_01, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK1, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK1_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK1_END, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK2, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK2_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK2_END, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK3, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK3_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK3_END, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK4, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK4_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK4_END, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK5, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK5_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TALK5_END, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_ROOT_STRUGGLE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_ROOT_GRAB_IDLE, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETTING_SUMMONED_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETTING_SUMMONED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETTING_SUMMONED_END, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETTING_SUMMONED_BY_MONSTER_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETTING_SUMMONED_BY_MONSTER, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETTING_SUMMONED_BY_MONSTER_END, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_CONVERSATION_NEUTRAL, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_FIRE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_LIGHTNING, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_ARCANE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_POISON, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_DISINTEGRATE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_PULVERIZE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_DECAPITATION, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_PLAGUED, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_DISMEMBER, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_ACID, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_SACRIFICE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_COLD, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_LAVA, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_HOLY, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_SPIRIT, ANIM_LAYER_DEATH);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_FIRE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_LIGHTNING, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_ARCANE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_POISON, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_DISINTEGRATE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_PULVERIZE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_DECAPITATION, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_PLAGUED, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_DISMEMBER, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_ACID, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_SACRIFICE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_COLD, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_LAVA, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_HOLY, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_SPIRIT, ANIM_LAYER_DEATH);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_02, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_02, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_03, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_03, ANIM_LAYER_DEATH);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_01, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_NO, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TAUNT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_WAVE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_YES, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TOWN_NO, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TOWN_TAUNT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TOWN_WAVE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TOWN_YES, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_STRUGGLE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GRAB_IDLE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_SPECIAL, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_SPECIAL, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_HIDDEN, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EAT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EAT_START, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EAT_STOP, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_WAIT_UNDERGROUND, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GESTURE_1, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GESTURE_2, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GESTURE_3, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GESTURE_4, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_LAUGH, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_CHEER, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_SHRUG_OR_QUESTION, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_COWER, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_EXCLAMATION_OR_SHOUT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_LAUGH, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_POINT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_SAD_OR_CRY, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TALK, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_IDLE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_USE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_GREET_OR_BOW_OR_SALUTE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_USE_LOOP, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_RESURRECT_ALLY, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RESURRECTION, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_SHRUG_OR_QUESTION_2, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_WARP_IN_MAGICAL, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_IDLE_2, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_COWER_2, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_IDLE_LONG, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_IDLE_LONG_2, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_IDLE_UNARMED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TALK_UNARMED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TALK_LONG, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_KNEEL_UNARMED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_STAND_UNARMED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_KNEEL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_STAND_UP, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_SHUSH, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_LOOK_1, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_LOOK_RIGHT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_LOOK_LEFT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_SCRATCH, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_NECK_ROLL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_READ, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_TALK_WOUNDED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_IDLE_WOUNDED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_KNEEL_TALK_01, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_KNEEL_TO_STAND, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_STAND_TO_KNEEL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_KNEEL_EAT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_KNEEL_USE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RECALL_CHANNEL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_SELECTION_SCREEN_IDLE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_STOOL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_DRINK_TABLE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_REST_TABLE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_GROUND, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_GROUND_LEANBACK, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_GROUND_LEANBACK_INJURED, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_GROUND_LEANBACK_INJURED_TALK, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_GROUND_LEANBACK_DEAD, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_GROUND_LEANBACK_TALK, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_SIT_GROUND_READING, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_STAND_LEANBACK, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_STAND_LEANLEFT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_STAND_LEANRIGHT, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_STAND_DRINKING, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_AMBIENT_STAND_READING, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_IDENTIFY_CHANNEL, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_CREATION_EMOTE, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_CREATION_IDLE, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_9, ANIM_LAYER_IDLE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SCRIPTED_EVENT_10, ANIM_LAYER_IDLE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_EMOTE_DANCE, ANIM_LAYER_IDLE);


    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETHIT, ANIM_LAYER_GETHIT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETHIT_CRITICAL, ANIM_LAYER_GETHIT);


    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_01, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_01_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_01_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_02, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_02_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_02_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_03, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_03_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_03_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_04, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_04_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_04_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_05_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_05, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_05_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_06_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_06, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_NORMAL_ATTACK_06_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RANGED_ATTACK_01, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RANGED_ATTACK_02, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RANGED_ATTACK_03, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_RANGED_ATTACK_04, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_01, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_02, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_03, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_04, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_04_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_CONTACT, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_PARRY, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEFLECT, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_HORRIFY, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_KNOCKDOWN_CYCLE, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_KNOCKDOWN_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_KNOCKDOWN_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_THROW, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_KNOCK_BACK, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_KNOCK_BACK_LAND, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SUPER_KNOCK_BACK, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SUPER_KNOCK_BACK_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SUPER_KNOCK_BACK_END, ANIM_LAYER_ACTION);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_OFFHAND_ATTACK_01, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_OFFHAND_ATTACK_01_START, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_OFFHAND_ATTACK_01_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_05, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_05_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_06, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_06_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_07, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_08, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_09, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_10, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_10_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_11, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_11_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_12, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_12_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_13, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_14, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_15, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_16, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_17, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_17_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_18, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_19, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_20, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_20_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_21, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_21_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_22, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_22_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_23, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_23_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_24, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_24_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_25, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_25_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_26, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_26_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_27, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_27_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_28, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_28_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_29, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_29_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_30, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_30_ALT, ANIM_LAYER_ACTION_ALT);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_31, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_32, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_33, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_34, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_35, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_36, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_37, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_38, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_39, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_40, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_41, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_42, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_43, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_44, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_45, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_46, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_47, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_48, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_49, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPECIAL_ATTACK_50, ANIM_LAYER_ACTION);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_MELEE_ATTACK_LEFT, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_MELEE_ATTACK_RIGHT, ANIM_LAYER_ACTION);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_BOTHHAND_ATTACK_01, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_BOTHHAND_ATTACK_01_END, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_BOTHHAND_ATTACK_01_START, ANIM_LAYER_ACTION);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_EVADE, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_EVADE_BACK, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_TOWN_EVADE, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_TOWN_EVADE_BACK, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_EVADE_RIGHT, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_EVADE_LEFT, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_TOWN_EVADE_RIGHT, ANIM_LAYER_SPECIAL_MOVE);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_TOWN_EVADE_LEFT, ANIM_LAYER_SPECIAL_MOVE);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_TEMP_CASTING, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SUMMON_GHOUL, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_FROM_GROUND, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_FROM_GROUND_2, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_JUMP_DOWN_LAND, ANIM_LAYER_ACTION);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_1, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_10, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_2, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_3, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_4, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_5, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_6, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_7, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_8, ANIM_LAYER_ACTION);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_SPAWN_9, ANIM_LAYER_ACTION);
}

void sInitNPCTagLayerMap(RActorAnimGlobals * ptRActorAnimGlobals)
{

    XGrowableMap<unsigned int, unsigned int, AnimLayer, AnimLayer> & tTagToLayer = ptRActorAnimGlobals->tNPCTagToLayer;
    tTagToLayer.Initialize(16, FALSE, NULL);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_GETHIT, ANIM_LAYER_GETHIT);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_01, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_SPECIAL, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_FIRE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_LIGHTNING, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_ARCANE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_POISON, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_DISINTEGRATE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_PULVERIZE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_DECAPITATION, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_PLAGUED, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_DISMEMBER, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_ACID, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_SACRIFICE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_COLD, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_HOLY, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_02, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEATH_03, ANIM_LAYER_DEATH);

    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_01, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_FIRE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_LIGHTNING, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_ARCANE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_POISON, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_DISINTEGRATE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_PULVERIZE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_DECAPITATION, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_PLAGUED, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_DISMEMBER, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_ACID, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_SACRIFICE, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_COLD, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_LAVA, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_HOLY, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_SPIRIT, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_02, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_03, ANIM_LAYER_DEATH);
    tTagToLayer.SetAt_Tracked(TAG_ACTOR_ANIMATION_DEAD_SPECIAL, ANIM_LAYER_DEATH);
}






void RActorsAnimInitializeForGame()
{

    RActorAnimGlobals *& ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;
    ptRActorAnimGlobals = (RActorAnimGlobals *)SigmaMemoryNew(sizeof(RActorAnimGlobals), 4, NULL, TRUE);

    ptRActorAnimGlobals->poolActorAnim.Initialize();
    ptRActorAnimGlobals->poolActorAnim.SetBlockCount_Tracked(64);















    sInitPlayerTagLayerMap(ptRActorAnimGlobals);
    sInitNPCTagLayerMap(ptRActorAnimGlobals);


    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_ACTION] = Blizzard::Util::Hash32("Action");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_ACTION_ALT] = Blizzard::Util::Hash32("ActionAlt");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_IDLE] = Blizzard::Util::Hash32("Idle");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_WALK] = Blizzard::Util::Hash32("Walk");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_RUN] = Blizzard::Util::Hash32("Run");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_SPRINT] = Blizzard::Util::Hash32("Sprint");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_SHUFFLE_LEFT] = Blizzard::Util::Hash32("ShuffleLeft");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_SHUFFLE_RIGHT] = Blizzard::Util::Hash32("ShuffleRight");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_WALK_SLOW] = Blizzard::Util::Hash32("WalkSlow");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_OUTRO] = Blizzard::Util::Hash32("Outro");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_FULL_TURN] = Blizzard::Util::Hash32("FullTurn");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_DEATH] = Blizzard::Util::Hash32("Death");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_SPECIAL_MOVE] = Blizzard::Util::Hash32("SpecialMove");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_FLINCH] = Blizzard::Util::Hash32("Flinch");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_NPC] = Blizzard::Util::Hash32("NPC");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_GETHIT] = Blizzard::Util::Hash32("GetHit");
    ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_CACHE] = Blizzard::Util::Hash32("Cache");

    AnimationInitializeForGame();
}




































































void RActorsAnimDisposeForGame()
{

    AnimationDisposeForGame();
    RActorAnimGlobals *& ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;

    ptRActorAnimGlobals->tPlayerTagToLayer.Dispose();
    ptRActorAnimGlobals->tNPCTagToLayer.Dispose();










    ptRActorAnimGlobals->poolActorAnim.Dispose();
    SigmaMemoryFree(ptRActorAnimGlobals, NULL);
    ptRActorAnimGlobals = NULL;
}



void RActorAnimInitialize(
    RActor * ptRActor)
{
    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;




    if (!(tStructure.dwFlags & (1 << STRUCTURE_CANNOT_ANIMATE_BIT)) &&
        ActorGetAnim(ptRActor->snoActor, WEAPONCLASS_NONE, TAG_ACTOR_ANIMATION_NEUTRAL) != SNO_NONE)
    {
        RActorAnimGlobals * ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;










        ptRActor->ptActorAnim = ptRActorAnimGlobals->poolActorAnim.Alloc_Tracked();
        ActorAnim & tActorAnim = *ptRActor->ptActorAnim;

        int32 nNumBoneCacheEntries = pdefActor->snoAnimTree != SNO_NONE ? 1 : 3;






        AnimationBoneCacheInit(tActorAnim.tAnimBoneCache, tStructure.nBoneCount, nNumBoneCacheEntries, tStructure.ptBoneStructure);










        AnimationChannelInit(tActorAnim.tAnimChannel, tStructure.nBoneCount, tStructure.ptBoneStructure, ptRActor->tLocation.tWorldLocation.flScale, ptRActor->snoAppearance);
        AnimationChainInit(ptRActor->id);






        tActorAnim.ptProcessedBoneList = (ProcessedBone *)SigmaMemoryNew(tStructure.nBoneCount * (sizeof(ProcessedBone) + sizeof(ProcessedBoneSkin) + sizeof(AnimationController *)), 4, NULL, FALSE);
        tActorAnim.ptProcessedBoneSkinList = (ProcessedBoneSkin *)(tActorAnim.ptProcessedBoneList + tStructure.nBoneCount);
        tActorAnim.ptBoneAnimControllerList = (AnimationController **)(tActorAnim.ptProcessedBoneSkinList + tStructure.nBoneCount);

        SigmaMemoryClear(tActorAnim.ptBoneAnimControllerList, tStructure.nBoneCount * sizeof(AnimationController *));

        for (int32 i = 0; i < tStructure.nBoneCount; i++)
        {
            BoneStructure & tBoneStructure = tStructure.ptBoneStructure[i];
            ProcessedBone & tProcessedBone = ptRActor->ptActorAnim->ptProcessedBoneList[i];
            tProcessedBone.transformDelta = cgtPRSTransformIdentity;
            tProcessedBone.modelTransform = tBoneStructure.transform;
        }



        tActorAnim.ptLookAt = NULL;
        tActorAnim.ptRagdoll = NULL;
        float flTimeOffset = RandFloat_Internal(CGameGlobalsGet().rngLocal) * 0.25f + 0.25f;
        tActorAnim.flLastUpdateBonesTime = CGameGetClientTime() - flTimeOffset;

        GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
        BOOL bPhysicsOkay = eGameMode == GAMEMODE_PLAYING || eGameMode == GAMEMODE_HOLODECK || eGameMode == GAMEMODE_MENU;
        if (bPhysicsOkay && pdefActor->snoPhysics != SNO_NONE)
        {
            if (tStructure.dwFlags & (1 << STRUCTURE_HAS_PARTIAL_RAGDOLL_BIT))
            {
                tActorAnim.ptRagdoll = new PartialRagdoll(ptRActor->id);
            }
            else if (tStructure.dwFlags & (1 << STRUCTURE_HAS_BREAKABLE_RAGDOLL_BIT))
            {
                tActorAnim.ptRagdoll = new BreakableRagdoll(ptRActor->id);
            }
        }

        tActorAnim.ptBonePulses = NULL;
        tActorAnim.nBonePulseCount = tStructure.nBonePulseCount;
        if (tActorAnim.nBonePulseCount > 0)
        {
            tActorAnim.ptBonePulses = (BonePulse *)SigmaMemoryNew(tActorAnim.nBonePulseCount * sizeof(BonePulse), 4, NULL, FALSE);
            for (int32 i = 0; i < tActorAnim.nBonePulseCount; i++)
            {
                new (&tActorAnim.ptBonePulses[i]) BonePulse(ptRActor->id, &tStructure.ptBonePulses[i]);
            }
        }






        tActorAnim.ptAnimTree = NULL;
        tActorAnim.snoAnimTree = SNO_NONE;
        tActorAnim.ptAnimTreeControl = NULL;


        if (pdefActor->snoAnimTree != SNO_NONE)
        {
            RActorAnimationEnableAnimTree(*ptRActor, TRUE, pdefActor->snoAnimTree);
        }
    }
}





void RActorAnimDispose(RActor * ptRActor)
{

    RActorAnimGlobals * ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;

    BOOL fRemove = ptRActor->ptActorAnim != NULL;





    if (fRemove)
    {
        ActorAnim & tActorAnim = *ptRActor->ptActorAnim;




        AnimationChannelDispose(tActorAnim.tAnimChannel);
        AnimationBoneCacheDispose(tActorAnim.tAnimBoneCache);
        AnimationChainDispose(ptRActor->id);

        ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);

        ::delete tActorAnim.ptLookAt; tActorAnim.ptLookAt = NULL;

        ::delete tActorAnim.ptRagdoll; tActorAnim.ptRagdoll = NULL;

        if (tActorAnim.ptBonePulses)
        {
            for (int32 i = 0; i < tActorAnim.nBonePulseCount; i++)
            {
                tActorAnim.ptBonePulses[i].~BonePulse();
            }

            SigmaMemoryFree(tActorAnim.ptBonePulses, NULL); tActorAnim.ptBonePulses = NULL;
        }

        if (tActorAnim.ptAnimTree)
        {
            AnimTreeDispose(*tActorAnim.ptAnimTree);
            delete tActorAnim.ptAnimTree; tActorAnim.ptAnimTree = NULL;
        }

        if (tActorAnim.ptAnimTreeControl)
        {
            delete tActorAnim.ptAnimTreeControl;
            tActorAnim.ptAnimTreeControl = NULL;
        }





        SigmaMemoryFree(tActorAnim.ptProcessedBoneList, NULL);
        tActorAnim.ptProcessedBoneList = NULL;
        tActorAnim.ptProcessedBoneSkinList = NULL;
        tActorAnim.ptBoneAnimControllerList = NULL;

        ptRActorAnimGlobals->poolActorAnim.Free(ptRActor->ptActorAnim);
    }
}





BOOL RActorShouldInterpolateBones(
    RActor * ptRActor,
    ActorAnim & tActorAnim,
    const AnimTreeDefinition * pdefAnimTree,
    BOOL fIsAnimating,
    BOOL fForceUpdate)
{

    const CGame & tClientData = CGameGet();
    BOOL bRagdollNeedsInterp = tActorAnim.ptRagdoll && tActorAnim.ptRagdoll->NeedsInitialVelocity();

    if (fIsAnimating && (fForceUpdate || !tActorAnim.tAnimBoneCache.bCacheIsValid || bRagdollNeedsInterp))
        return TRUE;



    BOOL fShouldUpdateThisFrame = tClientData.bLastClientUpdate;
    if (fIsAnimating && fShouldUpdateThisFrame)
        return TRUE;




    BOOL fNeedsInterpolation;
    if (tActorAnim.ptAnimTree)
    {
        fNeedsInterpolation = AnimationTreeNeedsInterpolation(*tActorAnim.ptAnimTree, *tActorAnim.ptAnimTreeControl, *pdefAnimTree);
    }
    else
    {
        fNeedsInterpolation = AnimationChannelNeedsInterpolation(tActorAnim.tAnimChannel);
    }
    return fNeedsInterpolation;
}





void RActorUpdateBones(
    RActor * ptRActor,
    float flDeltaTime,
    const ActorDefinition * pdefActor,
    BOOL fForceUpdate)
{
    if (!ptRActor || !ptRActor->ptActorAnim)
        return;



    ActorAnim & tActorAnim = *ptRActor->ptActorAnim;

    if (tActorAnim.ptRagdoll && !tActorAnim.ptRagdoll->ShouldAnimate())
        return;





    BOOL bIsAnimating;
    BOOL bAnimTreeEnabled = tActorAnim.ptAnimTree != NULL;
    const AnimTreeDefinition * pdefAnimTree = bAnimTreeEnabled ? static_cast<const AnimTreeDefinition *>(gptGroupDefAnimTree->LockR(tActorAnim.snoAnimTree, FALSE)) : NULL;
    if (!bAnimTreeEnabled)
    {

        bIsAnimating = AnimationChannelIsAnimating(tActorAnim.tAnimChannel, tActorAnim.tAnimBoneCache);
    }
    else
    {
        bIsAnimating = AnimationTreeIsAnimating(*tActorAnim.ptAnimTree, *tActorAnim.ptAnimTreeControl, *pdefAnimTree);
    }

    if (!bIsAnimating && !fForceUpdate && !tActorAnim.ptRagdoll)
    {
        if (pdefAnimTree)
        {

            gptGroupDefAnimTree->UnlockR((void *&)pdefAnimTree);

        }
        return;
    }
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;

    if (!tStructure.nBoneCount)
    {
        if (pdefAnimTree)
        {

            gptGroupDefAnimTree->UnlockR((void *&)pdefAnimTree);

        }
        return;
    }



    PRSTransform ptFinalInterpBoneList[255];
    BOOL fUpdateBones = FALSE;
    BOOL fInterpolateBones = RActorShouldInterpolateBones(ptRActor, tActorAnim, pdefAnimTree, bIsAnimating, fForceUpdate);


    if (!bAnimTreeEnabled)
    {
        PRSTransform * ptInterpBoneList = NULL;
        if (fInterpolateBones)
        {
            AnimationChannelInterpolateBones(tActorAnim.tAnimChannel, tActorAnim.tAnimBoneCache, tStructure.nBoneCount, ptInterpBoneList);
            fUpdateBones = TRUE;
        }
        else
        {
            ptInterpBoneList = tActorAnim.tAnimBoneCache.tBoneCache[tActorAnim.tAnimBoneCache.nCacheIndex].ptBone;
        }

        SigmaMemoryCopy(ptFinalInterpBoneList, ptInterpBoneList, tStructure.nBoneCount * sizeof(PRSTransform));
    }
    else
    {
        PRSTransform * ptInterpBoneList;
        if (fInterpolateBones)
        {
            ScopedAnimTreeDefinitionLockR pdefAnimTreeScoped(tActorAnim.snoAnimTree);

            PRSTransform tTransform[512];
            float flWeight[512];
            AnimBoneStack tBoneStack;
            tBoneStack.dwSize = 512;
            tBoneStack.dwTop = 0;
            tBoneStack.ptTransform = tTransform;
            tBoneStack.pflWeight = flWeight;

            AnimTreeBlendBones(*pdefAnimTreeScoped, *tActorAnim.ptAnimTreeControl, tBoneStack, *tActorAnim.ptAnimTree);
            tBoneStack.dwTop -= tActorAnim.ptAnimTree->nBoneCount;


            ptInterpBoneList = tActorAnim.tAnimBoneCache.tBoneCache[tActorAnim.tAnimBoneCache.nCacheIndex].ptBone;
            SigmaMemoryCopy(ptInterpBoneList, tBoneStack.ptTransform + tBoneStack.dwTop, tStructure.nBoneCount * sizeof(PRSTransform));
            tActorAnim.tAnimBoneCache.bCacheIsValid = TRUE;
            fUpdateBones = TRUE;
        }
        else
        {
            ptInterpBoneList = tActorAnim.tAnimBoneCache.tBoneCache[tActorAnim.tAnimBoneCache.nCacheIndex].ptBone;
        }

        SigmaMemoryCopy(ptFinalInterpBoneList, ptInterpBoneList, tStructure.nBoneCount * sizeof(PRSTransform));
    }


    if (tActorAnim.ptRagdoll)
    {

        BOOL bDeathAnimChannelAnimating = bIsAnimating;
        if (bAnimTreeEnabled)
        {

            uint32 dwDeathLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_DEATH);
            ScopedAnimTreeDefinitionLockR pdefAnimTreeScoped(tActorAnim.snoAnimTree);
            bDeathAnimChannelAnimating = AnimTreeLayerIsAnimating(*tActorAnim.ptAnimTree, *pdefAnimTreeScoped, dwDeathLayerHash);
        }
        tActorAnim.ptRagdoll->Update(bDeathAnimChannelAnimating, flDeltaTime, pdefActor);

        Ragdoll * ptRagdoll = tActorAnim.ptRagdoll;
        if (ptRagdoll->NeedsInitialVelocity() == TRUE)
        {

            AnimChannel * ptAnimChannel = &tActorAnim.tAnimChannel;
            if (bAnimTreeEnabled)
            {

                uint32 dwDeathLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_DEATH);
                ScopedAnimTreeDefinitionLockR pdefAnimTreeScoped(tActorAnim.snoAnimTree);
                for (uint32 dwChannel = 0; dwChannel < pdefAnimTreeScoped->dwNumLeafs; ++dwChannel)
                {
                    if (pdefAnimTreeScoped->ptLeaf[dwChannel].tLayerName.dwHash == dwDeathLayerHash)
                    {
                        ptAnimChannel = &tActorAnim.ptAnimTree->ptChannels[dwChannel];


                        PRSTransform * ptInterpBoneList = ptFinalInterpBoneList;
                        AnimationChannelInterpolateBones(*ptAnimChannel, tActorAnim.tAnimBoneCache, tStructure.nBoneCount, ptInterpBoneList);
                        break;
                    }
                }
            }






            const float flDeltaT = ptAnimChannel->flScaleDeltaT * (1.0f / 60.0f);
            float flSaveT = ptAnimChannel->flT;
            if (flSaveT >= flDeltaT)
            {
                const SNO snoAnim = ptAnimChannel->tAnimPermID.snoAnim;
                const int32 nPermutationIndex = ptAnimChannel->tAnimPermID.nPermutationIndex;

                const AnimationDefinition * pdefAnim = (const AnimationDefinition *)gptGroupDefAnimation->LockR(snoAnim, FALSE);

                float flVelocityFactor = pdefAnim->ptPermutations[nPermutationIndex].flRagdollVelocityFactor;
                float flMomentumFactor = pdefAnim->ptPermutations[nPermutationIndex].flRagdollMomentumFactor;
                gptGroupDefAnimation->UnlockR((void *&)pdefAnim);

                ptAnimChannel->flT = Math::Max(0.0f, flSaveT - flDeltaT);
                PRSTransform ptTransformBuffer[255];
                PRSTransform * ptRelativeTransforms1 = ptTransformBuffer;

                SigmaMemoryCopy(ptTransformBuffer, ptFinalInterpBoneList, tStructure.nBoneCount * sizeof(PRSTransform));
                AnimationChannelInterpolateBones(*ptAnimChannel, tActorAnim.tAnimBoneCache, tStructure.nBoneCount, ptRelativeTransforms1);
                ptAnimChannel->flT = flSaveT;
                ptRagdoll->InitializeVelocities(ptRelativeTransforms1, ptFinalInterpBoneList, flVelocityFactor, flMomentumFactor);
            }
        }
        fUpdateBones = TRUE;
    }

    if (fUpdateBones)
    {
        ProcessBones(ptRActor, tActorAnim.ptProcessedBoneList, tActorAnim.ptBoneAnimControllerList, ptFinalInterpBoneList, tStructure.ptBoneStructure, tStructure.nBoneCount, flDeltaTime);

        ProcessBoneMatrices(tActorAnim.ptProcessedBoneList, tActorAnim.ptProcessedBoneSkinList, tActorAnim.wpMin, tActorAnim.wpMax, tStructure.ptBoneStructure, tStructure.nBoneCount);


        if (tActorAnim.ptRagdoll)
        {
            tActorAnim.ptRagdoll->PostCompute();
        }
    }




















    if (pdefAnimTree)
    {
        gptGroupDefAnimTree->UnlockR((void *&)pdefAnimTree);
    }
}




BOOL RActorPlayAnimation(
    RActor * ptRActor,
    const uint32 dwTag,
    BOOL bDisableBlending,
    BOOL fOnlyPlayIfDifferent)
{
    if (!ptRActor->ptActorAnim)
        return FALSE;



    AnimChannelPlayParams tChannelPlayParams;

    SNO snoAnim;
    if (ptRActor->idACD == ACDID_INVALID)
    {
        snoAnim = ActorGetAnim(ptRActor->snoActor, WEAPONCLASS_NONE, dwTag);
    }
    else
    {
        snoAnim = ACDGet(ptRActor->idACD).GetSpecificAnim(dwTag);
    }












    if (fOnlyPlayIfDifferent)
    {
        if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
        {
            if (AnimationChannelGetSNO(ptRActor->ptActorAnim->tAnimChannel) == snoAnim)
                return FALSE;
        }
        else
        {


            AnimLayer eAnimLayer;
            if (RActorGetAnimLayerForTag(ptRActor, dwTag, eAnimLayer))
            {
                uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
                ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);
                if (AnimTreeLayerGetSNO(*ptRActor->ptActorAnim->ptAnimTree, *pdefAnimTree, dwAnimLayerHash) == snoAnim)
                    return FALSE;
            }
        }
    }



    RNG & rng = CGameGlobalsGet().rngLocal;
    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
    AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);

    if (RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {




        AnimLayer eAnimLayer;
        if (!RActorGetAnimLayerForTag(ptRActor, dwTag, eAnimLayer))
        {
            char szPath[256];
            SNOName snoname;
            snoname.snoGroup = 6;
            snoname.snoHandle = tChannelPlayParams.snoAnim;
            SNOFileGetFilePath(snoname, szPath, sizeof(szPath));
        }
        else if (eAnimLayer != ANIM_LAYER_NONE)
        {


            switch (eAnimLayer)
            {
            case ANIM_LAYER_IDLE:
            case ANIM_LAYER_DEATH:
                RActorAnimTreeLayerRampOutBlendForManual(*ptRActor, ANIM_LAYER_ACTION);
                RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_ACTION);
                RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_ACTION_ALT);
                RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_SPECIAL_MOVE);
                break;
            case ANIM_LAYER_SPECIAL_MOVE:
                RActorAnimTreeLayerRampOutBlend(*ptRActor, ANIM_LAYER_ACTION);
                RActorAnimTreeLayerRampOutBlend(*ptRActor, ANIM_LAYER_ACTION_ALT);
                break;
            }

            uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
            BOOL bImmediatelyActive = RActorGetAnimTreeLayerImmediatelyActive(eAnimLayer);
            BOOL bManualRampOut = RActorGetAnimTreeManualRampOut(ptRActor, dwTag);
            if (!bDisableBlending && eAnimLayer == ANIM_LAYER_NPC && !RActorAnimTreeLayerIsActive(*ptRActor, ANIM_LAYER_CACHE))
            {


                uint32 dwCacheLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_CACHE);
                AnimTreeLayerSetPoseFromLayer(ptRActor, dwCacheLayerHash, dwAnimLayerHash);
            }
            AnimTreeLayerPlay(ptRActor, dwAnimLayerHash, tChannelPlayParams, bImmediatelyActive, bManualRampOut);
            if (bDisableBlending && eAnimLayer == ANIM_LAYER_NPC)
            {





                uint32 dwCacheLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_CACHE);
                AnimTreeLayerSetPoseFromLayer(ptRActor, dwCacheLayerHash, dwAnimLayerHash);
            }
        }










        uint32 dwAdditionalTag;
        AnimTagGetAdditionalTag(dwTag, dwAdditionalTag);
        if (dwAdditionalTag != 0xffffffff)
            RActorPlayAnimation(ptRActor, dwAdditionalTag, bDisableBlending, fOnlyPlayIfDifferent);
    }
    else
    {
        AnimationChannelPlay(ptRActor, tChannelPlayParams, bDisableBlending);
    }
    return snoAnim != SNO_NONE;
}





float RActorAnimationGetPercentComplete(const RActor & tRActor)
{
    ActorAnim * ptActorAnim = tRActor.ptActorAnim;
    if (ptActorAnim)
    {
        float flT = ptActorAnim->tAnimChannel.flT;
        float flTMax = ptActorAnim->tAnimChannel.flTMax;
        return flT / flTMax;
    }
    return 0.0f;
}







void RActorAnimationSetPercentComplete(RActor & tRActor, float flPercentComplete)
{
    if (tRActor.ptActorAnim)
    {
        AnimationChannelSetPercentComplete(tRActor.ptActorAnim->tAnimChannel, flPercentComplete);
    }
}










void RActorAnimationScaleToVelocity(RActor & tRActor, const WorldDistance wdMag)
{
    if (tRActor.ptActorAnim)
    {
        AnimationChannelScaleToVelocity(tRActor.ptActorAnim->tAnimChannel, wdMag);
    }
}


























float RActorAnimationGetSpeedScale(const RActor & tRActor)
{
    if (tRActor.ptActorAnim)
    {
        return tRActor.ptActorAnim->tAnimChannel.flAnimSpeedScale;
    }
    return 0.0f;
}






void RActorAnimationSetSpeedScale(RActor & tRActor, const float flScale)
{
    if (tRActor.ptActorAnim)
    {
        AnimationChannelSetSpeedScale(tRActor.ptActorAnim->tAnimChannel, flScale);
    }
}








void RActorAnimationSetPaused(RActor & tRActor, BOOL bPaused)
{
    if (!tRActor.ptActorAnim)
        return;





    if (!RActorAnimationIsAnimTreeEnabled(tRActor))
    {
        ActorAnim & tActorAnim = *tRActor.ptActorAnim;
        if (bPaused)
        {
            ScopedActorDefinitionLockR pdefActor(tRActor.snoActor);
            AnimationChannelUpdate(tActorAnim.tAnimChannel, tActorAnim.tAnimBoneCache, 1.0f / 60.0f);
            RActorUpdateBones(&tRActor, 1.0f / 60.0f, pdefActor, TRUE);
        }
        tRActor.ptActorAnim->tAnimChannel.bPaused = bPaused;
    }
    else
    {
        if (bPaused)
        {

            ScopedActorDefinitionLockR pdefActor(tRActor.snoActor);
            RActorAnimTreeUpdate(tRActor, 1.0f / 60.0f);
            RActorUpdateBones(&tRActor, 1.0f / 60.0f, pdefActor, TRUE);
        }
        tRActor.ptActorAnim->ptAnimTree->bPaused = bPaused;
    }
}








































































































































BOOL RActorAnimationLookAtEnabled(RActor & tRActor)
{

    ScopedAppearanceDefinitionLockR pdefAppearance(tRActor.snoAppearance);
    const LookAtData * ptData = &pdefAppearance->tStructure.tLookAtData;
    BOOL bEnabled = ptData->bEnabled;
    return bEnabled;
}





void RActorAnimationLookAt(RActorID idSource, RActorID idTarget)
{


    RActor * ptSource = RActorTryToGet(idSource);
    if (!ptSource || !ptSource->ptActorAnim)
        return;



    if (!RActorAnimationLookAtEnabled(*ptSource))
        return;

    ActorAnim * ptAnim = ptSource->ptActorAnim;
    if (!ptAnim->ptLookAt)
    {
        ptAnim->ptLookAt = new LookAt(idSource);
    }



    ptAnim->ptLookAt->SetTarget(idTarget);
}



RActorID RActorAnimationGetLookAtTarget(const RActor & tRActor)
{
    if (!tRActor.ptActorAnim)
        return RACTORID_INVALID;

    ActorAnim * ptAnim = tRActor.ptActorAnim;
    if (ptAnim->ptLookAt)
        return ptAnim->ptLookAt->GetTarget();

    return RACTORID_INVALID;
}

































uint32 RActorAnimationGetWalkAnimTag(const RActor & tRActor, SNO snoPower, float flSpeedLimit)
{
    uint32 dwAnimTag;

    ActorCommonData & tACD = ACDGet(tRActor.idACD);
    if (PowerShouldUseClientWalking(tACD, AutoPowerDefinitionR(snoPower)))
    {
        ScopedPowerDefinitionLockR pdefPower(snoPower);
        dwAnimTag = PowerDetermineAnimation(AutoPowerDefinitionR(pdefPower), tRActor.idACD, cgTargetNone, FALSE);
    }
    else
    {


        BOOL bRun = tACD.ShouldRun();
        if (!CWorldIsRetro() && bRun && tACD.GetWalkingSpeed(TAG_ACTOR_ANIMATION_WALK) < flSpeedLimit)
        {
            if (ACDIsInTown(tACD) && AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_TOWNRUN)) dwAnimTag = TAG_ACTOR_ANIMATION_TOWNRUN; else dwAnimTag = TAG_ACTOR_ANIMATION_RUN;
        }
        else
        {







            dwAnimTag = ACDIsInTown(tACD) ? TAG_ACTOR_ANIMATION_TOWNWALK : TAG_ACTOR_ANIMATION_WALK;
        }
    }








    return dwAnimTag;
}




BOOL RActorAnimationHasReachedInterruptFrame(const RActor & tRActor)
{

    const AnimationDefinition * pdefAnim = ScopedAnimationDefinitionLockR(tRActor.ptActorAnim->tAnimChannel.tAnimPermID.snoAnim);
    const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[tRActor.ptActorAnim->tAnimChannel.tAnimPermID.nPermutationIndex];
    if (tAnimPerm.flEarliestInterruptKeyFrame >= 0.0f && tRActor.ptActorAnim->tAnimChannel.GetCurrentFrame() > tAnimPerm.flEarliestInterruptKeyFrame)
        return TRUE;
    return FALSE;
}









void RActorAnimationEnableAnimTree(RActor & tRActor, BOOL Enable, SNO snoAnimTree)
{



    ActorAnim * ptActorAnim = tRActor.ptActorAnim;
    if (!ptActorAnim)
        return;



    if (ptActorAnim->ptAnimTree)
    {
        AnimTreeDispose(*ptActorAnim->ptAnimTree);
        delete ptActorAnim->ptAnimTree; ptActorAnim->ptAnimTree = NULL;

        if (ptActorAnim->ptAnimTreeControl)
        {
            delete ptActorAnim->ptAnimTreeControl;
            ptActorAnim->ptAnimTreeControl = NULL;
        }
    }
    if (Enable)
    {
        ptActorAnim->snoAnimTree = snoAnimTree;

        if (snoAnimTree != SNO_NONE)
        {
            ScopedAppearanceDefinitionLockR pdefAppearance(tRActor.snoAppearance);
            const Structure & tStructure = pdefAppearance->tStructure;

            SNO snoAnimSet = ActorGetAnimSetSNO(tRActor.snoActor);

            ptActorAnim->ptAnimTree = new AnimTree();
            ptActorAnim->ptAnimTreeControl = new AnimTreeControl();
            SigmaMemoryClear(ptActorAnim->ptAnimTreeControl->tParam, sizeof(ptActorAnim->ptAnimTreeControl->tParam));
            ptActorAnim->ptAnimTreeControl->tParam[ACP_IS_IDLE].nValue = TRUE;
            ptActorAnim->ptAnimTreeControl->tParam[ACP_IS_ALIVE].nValue = TRUE;
            ptActorAnim->ptAnimTreeControl->tParam[ACP_FORCE_WALK].nValue = FALSE;
            ptActorAnim->ptAnimTreeControl->eRunOutro = 0;
            ptActorAnim->ptAnimTreeControl->eWalkOutro = 2;

            AnimTreeInit(
                *ptActorAnim->ptAnimTree,
                tStructure.nBoneCount,
                tStructure.ptBoneStructure,
                tRActor.tLocation.tWorldLocation.flScale,
                tRActor.snoAppearance,
                snoAnimSet,
                ptActorAnim->snoAnimTree,
                TAG_ACTOR_ANIMATION_NEUTRAL,
                WEAPONCLASS_NONE);

            RActorAnimGlobals *& ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;
            uint32 dwNPCLayerHash = ptRActorAnimGlobals->dwAnimLayerHash[ANIM_LAYER_NPC];

            ScopedAnimTreeDefinitionLockR pdefAnimTree(ptActorAnim->snoAnimTree);
            ptActorAnim->bHasNPCLayer = AnimTreeDefinitionHasLayer(*pdefAnimTree, dwNPCLayerHash);
        }
    }
}




BOOL RActorAnimationIsAnimTreeEnabled(
    const RActor & tRActor)
{
    ActorAnim * ptActorAnim = tRActor.ptActorAnim;
    if (ptActorAnim)
        return ptActorAnim->ptAnimTree != NULL;
    return FALSE;



}




void RActorAnimTreeUpdate(RActor & tRActor, float flDeltaTime)
{
    ActorAnim * ptActorAnim = tRActor.ptActorAnim;
    if (!ptActorAnim || !ptActorAnim->ptAnimTree)
        return;





    ScopedAnimTreeDefinitionLockR pdefAnimTree(ptActorAnim->snoAnimTree);
    AnimTreeUpdate(&tRActor, *pdefAnimTree, *ptActorAnim->ptAnimTreeControl, *ptActorAnim->ptAnimTree, flDeltaTime);
}




uint32 RActorGetAnimTreeLayerHash(AnimLayer eAnimLayer)
{

    RActorAnimGlobals *& ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;
    return ptRActorAnimGlobals->dwAnimLayerHash[eAnimLayer];
}




BOOL RActorGetAnimTreeLayerImmediatelyActive(AnimLayer eAnimLayer)
{
    BOOL bImmediatelyActive = eAnimLayer == ANIM_LAYER_ACTION || eAnimLayer == ANIM_LAYER_ACTION_ALT || eAnimLayer == ANIM_LAYER_DEATH || eAnimLayer == ANIM_LAYER_SPECIAL_MOVE || eAnimLayer == ANIM_LAYER_NPC || eAnimLayer == ANIM_LAYER_FULL_TURN;
    return bImmediatelyActive;
}












BOOL RActorGetAnimTreeManualRampOut(RActor * ptRActor, uint32 dwAnimationTag)
{

    if (dwAnimationTag == 0xFFFFFFFF)
        return FALSE;




    if (!ptRActor->ptActorAnim)
        return FALSE;



    SNO snoAnimTree = ptRActor->ptActorAnim->snoAnimTree;
    if (snoAnimTree == SNO_NONE)
        return FALSE;



    RActorAnimGlobals *& ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;
    if (!ptRActorAnimGlobals)
        return FALSE;



    BOOL bIsNPC = ptRActor->ptActorAnim->bHasNPCLayer;
    if (!bIsNPC)
    {






        switch (dwAnimationTag)
        {
        case TAG_ACTOR_ANIMATION_NORMAL_ATTACK_02:
        case TAG_ACTOR_ANIMATION_NORMAL_ATTACK_02_START:
        case TAG_ACTOR_ANIMATION_NORMAL_ATTACK_02_END:
        case TAG_ACTOR_ANIMATION_NORMAL_ATTACK_04:
        case TAG_ACTOR_ANIMATION_NORMAL_ATTACK_04_START:
        case TAG_ACTOR_ANIMATION_NORMAL_ATTACK_04_END:
        {
            ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
            if (ptACD)
            {
                PlayerClass eClass = PlayerGetPlayerClass(ptACD->id);
                if (eClass == PLAYER_CLASS_BARBARIAN)
                    return TRUE;
            }
            break;
        }
        default:
            break;
        }
    }




    return FALSE;
}



BOOL RActorGetAnimLayerForTag(RActor * ptRActor, uint32 dwAnimationTag, AnimLayer & eAnimLayer)
{
    eAnimLayer = ANIM_LAYER_NONE;
    if (dwAnimationTag == 0xFFFFFFFF)
        return FALSE;

    if (!ptRActor->ptActorAnim)
        return FALSE;

    SNO snoAnimTree = ptRActor->ptActorAnim->snoAnimTree;
    if (snoAnimTree == SNO_NONE)
        return FALSE;

    RActorAnimGlobals *& ptRActorAnimGlobals = CGameGet().ptRActorAnimGlobals;
    if (!ptRActorAnimGlobals)
        return FALSE;

    BOOL bIsNPC = ptRActor->ptActorAnim->bHasNPCLayer;
    if (bIsNPC)
    {
        if (!ptRActorAnimGlobals->tNPCTagToLayer.Lookup(dwAnimationTag, eAnimLayer))
            eAnimLayer = ANIM_LAYER_NPC;
        return TRUE;
    }
    return ptRActorAnimGlobals->tPlayerTagToLayer.Lookup(dwAnimationTag, eAnimLayer);
}


















































































void sRActorAnimationChangeAnimTreeLocomotionSet(
    RActor & tRActor,
    ActorCommonData & tACD,
    AnimTreeControl & tTreeControl,
    BOOL bInTown,
    BOOL bArchonForm,
    BOOL bForceWalk)
{
    const float SLOW_WALK_SCALE = 0.05f;
    const float SPRINT_SCALE = 2.0f;
    const float FORCE_WALK_SCALE = 2.0f;
    float flTranslationScale = tRActor.tLocation.tWorldLocation.flScale;
    float flMultiplier = bForceWalk ? FORCE_WALK_SCALE : 1.0f;


    SNO snoSprintAnim = tACD.GetSpecificAnim(bForceWalk ? TAG_ACTOR_ANIMATION_WALK : TAG_ACTOR_ANIMATION_RUN);
    if (bInTown && !bArchonForm)
        snoSprintAnim = tACD.GetSpecificAnim(bForceWalk ? TAG_ACTOR_ANIMATION_TOWNWALK : TAG_ACTOR_ANIMATION_TOWNRUN);


    uint32 dwSprintLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_SPRINT);
    if (snoSprintAnim != SNO_NONE)
    {
        RNG & rng = CGameGlobalsGet().rngLocal;
        AnimChannelPlayParams tChannelPlayParams;
        ScopedAnimationDefinitionLockR pdefAnim(snoSprintAnim);
        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
        tChannelPlayParams.flAnimSpeedScale *= flMultiplier;
        AnimTreeLayerPlay(&tRActor, dwSprintLayerHash, tChannelPlayParams, FALSE, FALSE);

        Vector3D wvVel;
        AnimationGetAvgVelocity(pdefAnim, 0, flTranslationScale, TRUE, wvVel);
        tTreeControl.tParam[ACP_SPRINT_SPEED].flValue = wvVel.x * SPRINT_SCALE * flMultiplier * 60.0f;
    }
    else
        tTreeControl.tParam[ACP_SPRINT_SPEED].flValue = 42.72f;




    SNO snoRunAnim = tACD.GetSpecificAnim(bForceWalk ? TAG_ACTOR_ANIMATION_WALK : TAG_ACTOR_ANIMATION_RUN);
    if (bInTown && !bArchonForm)
        snoRunAnim = tACD.GetSpecificAnim(bForceWalk ? TAG_ACTOR_ANIMATION_TOWNWALK : TAG_ACTOR_ANIMATION_TOWNRUN);


    uint32 dwRunLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_RUN);
    if (snoRunAnim != SNO_NONE)
    {
        RNG & rng = CGameGlobalsGet().rngLocal;
        AnimChannelPlayParams tChannelPlayParams;

        ScopedAnimationDefinitionLockR pdefAnim(snoRunAnim);
        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
        tChannelPlayParams.flAnimSpeedScale *= flMultiplier;
        AnimTreeLayerPlay(&tRActor, dwRunLayerHash, tChannelPlayParams, FALSE, FALSE);

        Vector3D wvVel;
        AnimationGetAvgVelocity(pdefAnim, 0, flTranslationScale, TRUE, wvVel);
        tTreeControl.tParam[ACP_RUN_SPEED].flValue = wvVel.x * flMultiplier * 60.0f;
    }
    else
        tTreeControl.tParam[ACP_RUN_SPEED].flValue = 21.36f;




    SNO snoWalkAnim = tACD.GetSpecificAnim(TAG_ACTOR_ANIMATION_WALK);
    if (bInTown && !bArchonForm)
        snoWalkAnim = tACD.GetSpecificAnim(TAG_ACTOR_ANIMATION_TOWNWALK);


    uint32 dwWalkLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_WALK);
    if (snoWalkAnim != SNO_NONE)
    {
        RNG & rng = CGameGlobalsGet().rngLocal;
        AnimChannelPlayParams tChannelPlayParams;

        ScopedAnimationDefinitionLockR pdefAnim(snoWalkAnim);
        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);

        AnimTreeLayerPlay(&tRActor, dwWalkLayerHash, tChannelPlayParams, FALSE, FALSE);
        Vector3D wvVel;
        AnimationGetAvgVelocity(pdefAnim, 0, flTranslationScale, TRUE, wvVel);
        tTreeControl.tParam[ACP_WALK_SPEED].flValue = wvVel.x * 60.0f;
    }
    else
        tTreeControl.tParam[ACP_WALK_SPEED].flValue = 12.0f;




    SNO snoWalkSlowAnim = tACD.GetSpecificAnim(TAG_ACTOR_ANIMATION_WALK);
    if (bInTown && !bArchonForm)
        snoWalkSlowAnim = tACD.GetSpecificAnim(TAG_ACTOR_ANIMATION_TOWNWALK);


    uint32 dwWalkSlowLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_WALK_SLOW);
    if (snoWalkSlowAnim != SNO_NONE)
    {
        RNG & rng = CGameGlobalsGet().rngLocal;
        AnimChannelPlayParams tChannelPlayParams;

        ScopedAnimationDefinitionLockR pdefAnim(snoWalkSlowAnim);
        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);

        AnimTreeLayerPlay(&tRActor, dwWalkSlowLayerHash, tChannelPlayParams, FALSE, FALSE);
        Vector3D wvVel;
        AnimationGetAvgVelocity(pdefAnim, 0, flTranslationScale, TRUE, wvVel);
        tTreeControl.tParam[ACP_WALK_SLOW_SPEED].flValue = wvVel.x * SLOW_WALK_SCALE * 60.0f;
    }
    else
        tTreeControl.tParam[ACP_WALK_SLOW_SPEED].flValue = 0.6f;



    SNO snoShuffleLeftAnim = tACD.GetSpecificAnim(TAG_ACTOR_EMOTE_IDLE_SHUFFLE_LEFT);
    uint32 dwShuffleLeftHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_SHUFFLE_LEFT);
    if (snoShuffleLeftAnim != SNO_NONE)
    {
        RNG & rng = CGameGlobalsGet().rngLocal;
        AnimChannelPlayParams tChannelPlayParams;

        ScopedAnimationDefinitionLockR pdefAnim(snoShuffleLeftAnim);
        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
        AnimTreeLayerPlay(&tRActor, dwShuffleLeftHash, tChannelPlayParams, FALSE, FALSE);
    }

    SNO snoShuffleRightAnim = tACD.GetSpecificAnim(TAG_ACTOR_EMOTE_IDLE_SHUFFLE_RIGHT);
    uint32 dwShuffleRightHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_SHUFFLE_RIGHT);
    if (snoShuffleRightAnim != SNO_NONE)
    {
        RNG & rng = CGameGlobalsGet().rngLocal;
        AnimChannelPlayParams tChannelPlayParams;

        ScopedAnimationDefinitionLockR pdefAnim(snoShuffleRightAnim);
        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
        AnimTreeLayerPlay(&tRActor, dwShuffleRightHash, tChannelPlayParams, FALSE, FALSE);
    }

}


// NON_MATCHING: 1704 both; scheduling only: the fmul for flWalkOutroVelocityThreshold (line 2432) is placed before the line-2436 compare instead of after the IS_ALIVE store. Fixed so far: ptACD if-blocks (2448/2483/2547), !bArchonForm && bIsAlive order (2436/2538), (x < thr) || bOutroPlayed (2510/2523), AIM_BLEND before AIM_YAW (2400).
void RActorAnimationUpdateAnimTreeState(
    RActor & tRActor,
    float flSpeed,
    Angle aYawSpeed,
    BOOL bTurn180Trigger,
    BOOL bTurn180Active,
    float flTurn180AnimScale,
    BOOL bAimEnabled,
    const Vector3D & wpAimTarget)
{

    const float flMinTurnSpeed = 120.0f;
    if (!tRActor.ptActorAnim)
        return;




    AnimTree * ptAnimTree = tRActor.ptActorAnim->ptAnimTree;
    AnimTreeControl * ptAnimTreeControl = tRActor.ptActorAnim->ptAnimTreeControl;
    if (!ptAnimTree || !ptAnimTreeControl)
        return;




    BOOL bArchonForm = FALSE;
    BOOL bIsAlive = ptAnimTreeControl->tParam[ACP_IS_ALIVE].nValue;
    ActorCommonData * ptACD = ACDTryToGet(tRActor.idACD);
    if (ptACD)
    {
        bIsAlive = ptACD->IsAlive();
        PlayerClass eClass = PlayerGetPlayerClass(ptACD->id);
        bArchonForm = eClass == 2 && ptACD->GetWeaponClassForAnim(NULL) == 14;
    }




    ScopedAnimTreeDefinitionLockR pdefAnimTree(tRActor.ptActorAnim->snoAnimTree);


    float flPrevForwardSpeed = ptAnimTreeControl->tParam[ACP_FORWARD_SPEED].flValue;
    float flForwardSpeed = flSpeed * 60.0f;
    float flDeltaForwardSpeed = flForwardSpeed - flPrevForwardSpeed;
    ptAnimTreeControl->tParam[ACP_FORWARD_SPEED].flValue = flPrevForwardSpeed + flDeltaForwardSpeed * 0.5f;

    float flPrevYawSpeed = ptAnimTreeControl->tParam[ACP_TURN_SPEED].flValue;
    float flYawSpeed = bArchonForm || bTurn180Active ? 0.0f : Math::AngleToDeg(aYawSpeed) * 60.0f;
    float flDeltaTurnSpeed = flYawSpeed - flPrevYawSpeed;
    ptAnimTreeControl->tParam[ACP_TURN_SPEED].flValue = flPrevYawSpeed + flDeltaTurnSpeed * 0.5f;
    ptAnimTreeControl->tParam[ACP_ABS_TURN_SPEED].flValue = Math::Abs(ptAnimTreeControl->tParam[ACP_TURN_SPEED].flValue);
    ptAnimTreeControl->tParam[ACP_IS_IDLE].nValue = flSpeed < 0.000001f;
    ptAnimTreeControl->tParam[ACP_IS_TURNING].nValue = Math::Abs(flYawSpeed) > flMinTurnSpeed;


    WorldPlace tActorPlace = RActorGetWorldPlace(&tRActor);
    Vector3D vecTargetDir = wpAimTarget - tActorPlace.wp;

    Angle aTargetYaw = Math::ATan(vecTargetDir.y, vecTargetDir.x);
    Angle aAim = Math::SignedAngularDifference(tRActor.aYaw, aTargetYaw);
    float flAim = Math::AngleToDeg(aAim);
    ptAnimTreeControl->tParam[ACP_AIM_BLEND].flValue = bAimEnabled ? 1.0f : 0.0f;
    ptAnimTreeControl->tParam[ACP_AIM_YAW].flValue = flAim;

    // DWARF records bInTown; FALSE initialization outside the non-monster branch is inferred and dead.
    BOOL bInTown = FALSE;
    if (ptACD && !ptACD->IsMonster())
    {
        WeaponClass ePrevWeaponClass = ptAnimTreeControl->tParam[ACP_WEAPON_CLASS].nValue;
        WeaponClass eWeaponClass = ptACD->GetWeaponClassForAnim(NULL);
        BOOL bPrevInTown = ptAnimTreeControl->tParam[ACP_IN_TOWN].nValue;
        bInTown = ACDIsInTown(*ptACD);

        BOOL bPrevForceWalk = ptAnimTreeControl->tParam[ACP_FORCE_WALK].nValue;
        BOOL bForceWalk = CWorldIsRetro();
        if (ePrevWeaponClass != eWeaponClass ||
            bPrevInTown != bInTown ||
            bPrevForceWalk != bForceWalk)
        {
            sRActorAnimationChangeAnimTreeLocomotionSet(tRActor, *ptACD, *ptAnimTreeControl, bInTown, bArchonForm, bForceWalk);
        }


        ptAnimTreeControl->tParam[ACP_WEAPON_CLASS].nValue = eWeaponClass;
        ptAnimTreeControl->tParam[ACP_IN_TOWN].nValue = bInTown;
        ptAnimTreeControl->tParam[ACP_FORCE_WALK].nValue = bForceWalk;
    }


    ptAnimTreeControl->tParam[ACP_IS_ALIVE].nValue = bIsAlive;


    float flRunOutroVelocityThreshold = (ptAnimTreeControl->tParam[ACP_WALK_SPEED].flValue + ptAnimTreeControl->tParam[ACP_RUN_SPEED].flValue) * 0.5f;
    float flWalkOutroVelocityThreshold = ptAnimTreeControl->tParam[ACP_WALK_SPEED].flValue * 0.25f;
    const float OUTRO_TIMER_LENGTH_SEC = 0.2f;
    BOOL bOutroPlayed = FALSE;

    if (!bArchonForm && bIsAlive)
    {
        if ((ptAnimTreeControl->tParam[ACP_FORWARD_SPEED].flValue < flRunOutroVelocityThreshold) &&
            (ptAnimTreeControl->flRunOutroTimer < 0.0f))
        {
            uint32 dwOutroLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_OUTRO);

            if (AnimTreeGetLayerPercentComplete(*ptAnimTree, *pdefAnimTree, dwOutroLayerHash) < 0.000001f)
            {
                SNO snoAnimOutroLeftFoot = SNO_NONE;
                SNO snoAnimOutroRightFoot = SNO_NONE;

                if (ptACD)
                {
                    snoAnimOutroLeftFoot = ptACD->GetSpecificAnim(TAG_ACTOR_RUN_OUTRO_LEFTFOOT);
                    snoAnimOutroRightFoot = ptACD->GetSpecificAnim(TAG_ACTOR_RUN_OUTRO_RIGHTFOOT);
                }

                bool bLeftOutro = ptAnimTreeControl->eRunOutro == 0;

                RNG & rng = CGameGlobalsGet().rngLocal;
                AnimChannelPlayParams tChannelPlayParams;
                SNO snoAnim = bLeftOutro ? snoAnimOutroLeftFoot : snoAnimOutroRightFoot;

                if (snoAnim != SNO_NONE)
                {
                    if (ptAnimTreeControl->eRunOutro != 4)
                    {
                        ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
                        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
                        AnimTreeLayerPlay(&tRActor, dwOutroLayerHash, tChannelPlayParams, TRUE, FALSE);
                    }
                    bOutroPlayed = TRUE;
                }
            }
        }

        else if ((ptAnimTreeControl->tParam[ACP_FORWARD_SPEED].flValue < flWalkOutroVelocityThreshold) &&
                 (ptAnimTreeControl->flWalkOutroTimer < 0.0f))
        {
            uint32 dwOutroLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_OUTRO);

            if (AnimTreeGetLayerPercentComplete(*ptAnimTree, *pdefAnimTree, dwOutroLayerHash) < 0.000001f)
            {
                SNO snoAnimOutroLeftFoot = SNO_NONE;
                SNO snoAnimOutroRightFoot = SNO_NONE;

                if (ptACD)
                {
                    snoAnimOutroLeftFoot = ptACD->GetSpecificAnim(TAG_ACTOR_WALK_OUTRO_LEFTFOOT);
                    snoAnimOutroRightFoot = ptACD->GetSpecificAnim(TAG_ACTOR_WALK_OUTRO_RIGHTFOOT);
                }

                bool bLeftOutro = ptAnimTreeControl->eWalkOutro == 2;

                RNG & rng = CGameGlobalsGet().rngLocal;
                AnimChannelPlayParams tChannelPlayParams;
                SNO snoAnim = bLeftOutro ? snoAnimOutroLeftFoot : snoAnimOutroRightFoot;

                if (snoAnim != SNO_NONE)
                {
                    if (ptAnimTreeControl->eWalkOutro != 5)
                    {
                        ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
                        AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
                        AnimTreeLayerPlay(&tRActor, dwOutroLayerHash, tChannelPlayParams, TRUE, FALSE);

                    }
                    bOutroPlayed = TRUE;
                }
            }
        }
    }

    if ((ptAnimTreeControl->tParam[ACP_FORWARD_SPEED].flValue < flRunOutroVelocityThreshold) || bOutroPlayed)
    {
        ptAnimTreeControl->flRunOutroTimer = OUTRO_TIMER_LENGTH_SEC;
    }
    else
    {


        ptAnimTreeControl->flRunOutroTimer -= 1.0f / 60.0f;
    }



    if ((ptAnimTreeControl->tParam[ACP_FORWARD_SPEED].flValue < flWalkOutroVelocityThreshold) || bOutroPlayed)
    {
        ptAnimTreeControl->flWalkOutroTimer = OUTRO_TIMER_LENGTH_SEC;
    }
    else
    {


        ptAnimTreeControl->flWalkOutroTimer -= 1.0f / 60.0f;
    }



    // DWARF records bTurn180Played; initialization and successful-play assignment are inferred dead stores.
    BOOL bTurn180Played = FALSE;
    if (!bArchonForm && bIsAlive && bTurn180Trigger)
    {
        uint32 dwFullTurnLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_FULL_TURN);

        if (AnimTreeGetLayerPercentComplete(*ptAnimTree, *pdefAnimTree, dwFullTurnLayerHash) < 0.000001f)
        {
            SNO snoAnimLeft180 = SNO_NONE;
            SNO snoAnimRight180 = SNO_NONE;

            if (ptACD)
            {
                snoAnimLeft180 = ptACD->GetSpecificAnim(TAG_ACTOR_EMOTE_IDLE_SHUFFLE_LEFT_180);
                snoAnimRight180 = ptACD->GetSpecificAnim(TAG_ACTOR_EMOTE_IDLE_SHUFFLE_RIGHT_180);
            }

            RNG & rng = CGameGlobalsGet().rngLocal;
            AnimChannelPlayParams tChannelPlayParams;
            BOOL bLeftTurn = flYawSpeed > 0.0f;
            SNO snoAnim = bLeftTurn ? snoAnimLeft180 : snoAnimRight180;

            if (snoAnim != SNO_NONE)
            {
                ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
                AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);
                tChannelPlayParams.flAnimSpeedScale = flTurn180AnimScale;

                AnimTreeLayerPlay(&tRActor, dwFullTurnLayerHash, tChannelPlayParams, TRUE, FALSE);
                bTurn180Played = TRUE;
            }
        }
    }


}




void RActorAnimationOnACDDelete(RActor & tRActor)
{
    AnimTree * ptAnimTree = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTree : NULL;
    AnimTreeControl * ptAnimTreeControl = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTreeControl : NULL;
    if (ptAnimTree && ptAnimTreeControl)
    {
        BOOL bIsAlive = ptAnimTreeControl->tParam[ACP_IS_ALIVE].nValue;









        ActorCommonData * ptACD = ACDTryToGet(tRActor.idACD);
        if (ptACD)
        {
            bIsAlive = ptACD->IsAlive();
        }
        ptAnimTreeControl->tParam[ACP_IS_ALIVE].nValue = bIsAlive;
    }
}





BOOL RActorAnimTreeLayerHasReachedInterruptFrame(const RActor & tRActor, AnimLayer eAnimLayer)
{
    AnimTree * ptAnimTree = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTree : NULL;
    if (!ptAnimTree)
        return TRUE;











    uint32 dwLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    ScopedAnimTreeDefinitionLockR pdefAnimTree(tRActor.ptActorAnim->snoAnimTree);
    for (uint32 dwChannel = 0; dwChannel < pdefAnimTree->dwNumLeafs; dwChannel++)
    {
        if (pdefAnimTree->ptLeaf[dwChannel].tLayerName.dwHash == dwLayerHash)
        {
            AnimChannel * ptChannel = &ptAnimTree->ptChannels[dwChannel];
            const AnimationDefinition * pdefAnim = ScopedAnimationDefinitionLockR(ptChannel->tAnimPermID.snoAnim);
            const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[ptChannel->tAnimPermID.nPermutationIndex];
            if (tAnimPerm.flEarliestInterruptKeyFrame >= 0.0f && ptChannel->GetCurrentFrame() > tAnimPerm.flEarliestInterruptKeyFrame)
                return TRUE;
        }
    }
    return FALSE;
}









BOOL RActorAnimTreeLayerIsActive(const RActor & tRActor, AnimLayer eAnimLayer)
{
    AnimTree * ptAnimTree = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTree : NULL;
    if (!ptAnimTree)
        return FALSE;

    uint32 dwLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    ScopedAnimTreeDefinitionLockR pdefAnimTree(tRActor.ptActorAnim->snoAnimTree);
    return AnimTreeLayerIsActive(*ptAnimTree, *pdefAnimTree, dwLayerHash);
}















void RActorAnimTreeSetOutroAnim(const RActor & tRActor, OutroAnimationType eOutroAnim)
{
    AnimTreeControl * ptAnimTreeControl = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTreeControl : NULL;
    if (!ptAnimTreeControl)
        return;

    switch (eOutroAnim)
    {
    case 0:
    case 1:
    case 4:
        ptAnimTreeControl->eRunOutro = eOutroAnim;
        break;
    case 2:
    case 3:
    case 5:
        ptAnimTreeControl->eWalkOutro = eOutroAnim;
        break;
    }
}








































































void RActorAnimTreeLayerScaleToVelocity(RActor & tRActor, AnimLayer eAnimLayer, const WorldDistance wdMag)
{
    AnimTree * ptAnimTree = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTree : NULL;
    if (!ptAnimTree)
        return;

    uint32 dwLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    ScopedAnimTreeDefinitionLockR pdefAnimTree(tRActor.ptActorAnim->snoAnimTree);
    AnimTreeLayerScaleToVelocity(*ptAnimTree, *pdefAnimTree, dwLayerHash, wdMag);
}















void RActorAnimTreeLayerRampOutBlend(RActor & tRActor, AnimLayer eAnimLayer)
{
    AnimTree * ptAnimTree = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTree : NULL;
    if (!ptAnimTree)
        return;

    uint32 dwLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    ScopedAnimTreeDefinitionLockR pdefAnimTree(tRActor.ptActorAnim->snoAnimTree);
    AnimTreeLayerRampOutBlend(*ptAnimTree, *pdefAnimTree, dwLayerHash);
}














void RActorAnimTreeLayerRampOutBlendForManual(RActor & tRActor, AnimLayer eAnimLayer)
{
    AnimTree * ptAnimTree = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTree : NULL;
    if (!ptAnimTree)
        return;










    uint32 dwLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    ScopedAnimTreeDefinitionLockR pdefAnimTree(tRActor.ptActorAnim->snoAnimTree);
    if (AnimTreeLayerHasManualBlendRampOut(*ptAnimTree, *pdefAnimTree, dwLayerHash))
    {
        AnimTreeLayerRampOutBlend(*ptAnimTree, *pdefAnimTree, dwLayerHash);
    }
}





void RActorAnimTreeLayerRampOutBlendForLooping(RActor & tRActor, AnimLayer eAnimLayer)
{
    AnimTree * ptAnimTree = tRActor.ptActorAnim ? tRActor.ptActorAnim->ptAnimTree : NULL;
    if (!ptAnimTree)
        return;

    uint32 dwLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    ScopedAnimTreeDefinitionLockR pdefAnimTree(tRActor.ptActorAnim->snoAnimTree);
    if (AnimTreeLayerGetPlaybackMode(*ptAnimTree, *pdefAnimTree, dwLayerHash) == PLAYBACKMODE_LOOP)
    {
        AnimTreeLayerRampOutBlend(*ptAnimTree, *pdefAnimTree, dwLayerHash);
    }
}














int32 RActorAnimTreeLayerGetHardpointLinkIndex(
    const RActor & tRActor,
    AnimLayer eAnimLayer,
    const AnimTreeDefinition * pdefAnimTree,
    const AppearanceDefinition * pdefAppearance)
{
    if (!tRActor.ptActorAnim)
        return -1;



    AnimTree * ptAnimTree = tRActor.ptActorAnim->ptAnimTree;
    if (!ptAnimTree)
        return -1;



    uint32 dwLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    BOOL fLocalLock = FALSE;
    if (!pdefAnimTree)
    {
        pdefAnimTree = (const AnimTreeDefinition *)gptGroupDefAnimTree->LockRFast(tRActor.ptActorAnim->snoAnimTree);  // all columns 24: a lock macro in the original, expansion inferred
        fLocalLock = TRUE;
    }
    for (uint32 dwLeafIndex = 0; dwLeafIndex < pdefAnimTree->dwNumLeafs; dwLeafIndex++)
    {
        if (pdefAnimTree->ptLeaf[dwLeafIndex].tLayerName.dwHash == dwLayerHash)
        {
            const AnimPermID & idPerm = ptAnimTree->ptChannels[dwLeafIndex].tAnimPermID;
            const AnimationDefinition * pdefAnimation = (const AnimationDefinition *)gptGroupDefAnimation->LockR(idPerm.snoAnim, FALSE);


            const AnimPermutation * ptPermutation = &pdefAnimation->ptPermutations[idPerm.nPermutationIndex];
            int32 nHardpointIndex = pdefAppearance ? HardpointIndexFromName(pdefAppearance, ptPermutation->tHardpointLink.tHash) : HardpointIndexFromName(tRActor.snoAppearance, ptPermutation->tHardpointLink.tHash);
            gptGroupDefAnimation->UnlockR((void *&)pdefAnimation);
            if (fLocalLock)
                DefinitionUnlockR<AnimTreeDefinition>(pdefAnimTree);
            return nHardpointIndex;
        }
    }







    if (fLocalLock)
        DefinitionUnlockR<AnimTreeDefinition>(pdefAnimTree);
    return -1;

}



// The two tests on 2932 are in this order: written layer-first, the ptActorAnim test on 2927 folds into the inlined helper (block layout differs).
BOOL RActorAnimTreeLayerPlay(
    RActor * ptRActor,
    AnimLayer eAnimLayer,
    const uint32 dwTag,
    BOOL fOnlyPlayIfDifferent)
{
    if (!ptRActor->ptActorAnim)
        return FALSE;



    if (!RActorAnimationIsAnimTreeEnabled(*ptRActor) || eAnimLayer == ANIM_LAYER_NONE)
        return FALSE;








    AnimChannelPlayParams tChannelPlayParams;

    SNO snoAnim;
    if (ptRActor->idACD == ACDID_INVALID)
    {
        snoAnim = ActorGetAnim(ptRActor->snoActor, WEAPONCLASS_NONE, dwTag);
    }
    else
    {
        snoAnim = ACDGet(ptRActor->idACD).GetSpecificAnim(dwTag);
    }












    if (fOnlyPlayIfDifferent)
    {
        uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
        ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);
        if (AnimTreeLayerGetSNO(*ptRActor->ptActorAnim->ptAnimTree, *pdefAnimTree, dwAnimLayerHash) == snoAnim)
            return FALSE;
    }



    RNG & rng = CGameGlobalsGet().rngLocal;

    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
    AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);



    if (eAnimLayer == ANIM_LAYER_IDLE || eAnimLayer == ANIM_LAYER_DEATH)
    {
        RActorAnimTreeLayerRampOutBlendForManual(*ptRActor, ANIM_LAYER_ACTION);
        RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_ACTION);
        RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_ACTION_ALT);
        RActorAnimTreeLayerRampOutBlendForLooping(*ptRActor, ANIM_LAYER_SPECIAL_MOVE);
    }
    else if (eAnimLayer == ANIM_LAYER_SPECIAL_MOVE)
    {
        RActorAnimTreeLayerRampOutBlend(*ptRActor, ANIM_LAYER_ACTION);
        RActorAnimTreeLayerRampOutBlend(*ptRActor, ANIM_LAYER_ACTION_ALT);
    }

    uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
    BOOL bImmediatelyActive = RActorGetAnimTreeLayerImmediatelyActive(eAnimLayer);
    BOOL bManualRampOut = RActorGetAnimTreeManualRampOut(ptRActor, dwTag);
    AnimTreeLayerPlay(ptRActor, dwAnimLayerHash, tChannelPlayParams, bImmediatelyActive, bManualRampOut);

    return snoAnim != SNO_NONE;
}




int32 RActorAnimationLookupBone(const RActor & tRActor, LPCSTR szBoneName)
{


    if (!tRActor.ptActorAnim)
        return -1;

    const AnimChannel & tAnimChannel = tRActor.ptActorAnim->tAnimChannel;


    if (tAnimChannel.tAnimPermID.snoAnim == SNO_NONE)
        return -1;



    ScopedAnimationDefinitionLockR pdefAnim(tAnimChannel.tAnimPermID.snoAnim);

    int32 nPerm = tAnimChannel.tAnimPermID.nPermutationIndex;

    if (nPerm < pdefAnim->nPermutationCount)
    {



        const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[nPerm];



        uint64 dwBoneHash = StringHashI(szBoneName);

        for (int32 i = 0; i != tAnimPerm.nBoneCount; i++)
        {

            if (dwBoneHash == tAnimPerm.ptBoneNames[i].dwHash)
                return i;
        }
    }
    return -1;
}

