// Original file: X/Common/AnimPlayer.cpp
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "ActorCommonData.h"
#include "AnimPlayer.h"
#include "TagMaps/AnimTagsSwig.h"
#include "TagMaps/AnimTags.h"
#include "Definitions/AnimationDefinition.h"
#include "Definitions/ActorDefinition.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "Game.h"
#include "GameCommonData.h"
#include "Player.h"
#include "GlobalSNO.h"
#include "Networking/GameMessage.h"
#include "../Client/CGame.h"
#include "../Server/SGame.h"

struct AnimPlayer {  // size 0xdc
    AnimSpec tAnimSpecArray[6];  // 0x00
    int32 nNumQueuedAnimations;  // 0xc0
    uint32 dwAnimationStartTime;  // 0xc4
    uint32 dwAnimationEndTime;  // 0xc8
    uint32 dwIdleAnimationTag;  // 0xcc
    uint32 dwAnimationTag;  // 0xd0
    BOOL bUseAnimationTranslation;  // 0xd4
    AnimReason eAnimReason;  // 0xd8
};
static_assert(sizeof(AnimPlayer) == 0xdc, "AnimPlayer");

struct AnimPlayerGlobals {  // size 0x30
    XChainedFixedBlockAllocator<AnimPlayer> poolAnimPlayer;  // 0x00
};
static_assert(sizeof(AnimPlayerGlobals) == 0x30, "AnimPlayerGlobals");

// Also declared in X/Common/AnimCommon.h; defined at AnimCommon.cpp:187.
void AnimationGetAvgVelocity(const AnimationDefinition * pdefAnim, const int32 nPermutationIndex, const float flTranslationScale, const BOOL bMultipleBySpeedScale, Vector3D & wvVel);

WorldDistance AnimGetWalkingSpeed(const AnimationDefinition * pdefAnim, float flSpeedFactor);
void AnimPlayerPlayIdle(ActorCommonData & tACD);
BOOL AnimPlayerGetAnimationTiming(ActorCommonData & tACD, const uint32 dwAnimationTag, const uint32 dwLength, const float flSpeedScalar, RNG & rng, AnimationTiming & tAnimationTiming);
ACDID LocalPlayerACDGet();  // X/Client/CPlayer.h
SNO CPlayerInputGetCurrentAnimPower(Player * ptPlayer);  // X/Client/CPlayerInput.cpp
// X/Client/UserInterface/Interface/UIConversations.cpp
namespace UIConversation {
BOOL ACDIsInConversation(const ACDID idACD, BOOL bConvDialogOnly, BOOL bUnusableOnly);
}  // namespace UIConversation
void AnimPlayerGetCurrentAnim(const ActorCommonData & tACD, SNO & snoAnim, int32 & nAnimPermutation);
#include "../Server/PlayerList.h"
#include "Movement/ActorMovement.h"
#include "Attributes/FastAttribute.h"
#include "../Client/RActors/RActors.h"
#include "../Client/RActors/RActorAnimation.h"
#include "../Client/Physics/Ragdoll.h"
#include "AnimCommon.h"
#include "TagMaps/PowerTags.h"
#include "WorldPlace.h"
#include "Movement/ActorSteer.h"
#include "Powers/PowersCommon.h"
#include "../Server/Actors/Actors.h"
#include "../Server/Actors/ActorPowers.h"
#include "../Server/Powers/Powers.h"
#include "../Server/Movement/SActorMovement.h"
#include "../Client/Movement/CActorMovement.h"
#include "../Client/Gfx/GfxVB.h"
void AnimPlayerIdleAnimationChanged(ActorCommonData & tACD, const uint32 dwAnimTag);
BOOL ReplayIsPlaying(); void RActorAnimationSetPaused(RActor & tRActor, BOOL fPaused);  // X/Client/RActors/RActors.cpp
void RActorPlayRemainingKeyframedRMessages(RActor * ptRActor); RagdollType RActorGetRagdollType(const RActor * ptRActor);  // X/Client/RActors/RActors.cpp
void AnimationChannelSetPercentComplete(AnimChannel & tChannel, float flFraction);  // X/Client/Animation.cpp
BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor); float RActorAnimationGetPercentComplete(const RActor & tRActor); void RActorAnimationSetPercentComplete(RActor & tRActor, float flPercentComplete); BOOL RActorGetAnimLayerForTag(RActor * ptRActor, uint32 dwAnimationTag, AnimLayer & eAnimLayer);  // RActorAnimation.cpp:2087
uint32 RActorGetAnimTreeLayerHash(AnimLayer eAnimLayer); BOOL RActorGetAnimTreeLayerImmediatelyActive(AnimLayer eAnimLayer); BOOL RActorGetAnimTreeManualRampOut(RActor * ptRActor, uint32 dwAnimationTag); BOOL RActorAnimTreeLayerIsActive(const RActor & tRActor, AnimLayer eAnimLayer);  // RActorAnimation.cpp:2645
void RActorAnimTreeLayerRampOutBlend(RActor & tRActor, AnimLayer eAnimLayer); void RActorAnimTreeLayerRampOutBlendForManual(RActor & tRActor, AnimLayer eAnimLayer); void RActorAnimTreeLayerRampOutBlendForLooping(RActor & tRActor, AnimLayer eAnimLayer);
void AnimationChannelPlay(RActor * ptRActor, const AnimChannelPlayParams & tChannelPlayParams, const BOOL bDisableBlending);  // X/Client/Animation.cpp
void AnimTreeLayerPlay(RActor * ptRActor, uint32 dwLayerHash, const AnimChannelPlayParams & tChannelPlayParams, BOOL bImmediatelyActive, BOOL bManualRampOut); void AnimTreeLayerSetPoseFromLayer(RActor * ptRActor, uint32 dwLayerHash, uint32 dwFromLayerHash);  // X/Client/AnimTree.cpp; parameter names not checked
void AnimTagGetAdditionalTag(const uint32 dwAnimTag, uint32 & uAdditionalTag);  // X/Common/TagMaps/AnimTags.cpp; parameter names not checked
void VisualInventoryUpdateSheathed(const ACDID idACD);  // X/Client/CVisualInventory.cpp:832
void SNOFileGetFilePath(const SNOName & snoname, char * szFilePath, int32 nFilePathMaxSize); float PowerGetAnimationSpeedForPower(const ACDID idACD, AutoPowerDefinitionR pdefPower, const int32 nComboLevel); BOOL AnimIsDeathAnim(uint32 uAnimTag);  // X/Common/Powers/PowersCommon.cpp:8234; X/Common/TagMaps/AnimTags.cpp:660
BOOL IsTriggerEventTargetingActor(ActorID idActor); Angle ACDGetYaw(ACDID idACD);


static const uint32 TICKS_BLENDING_DISABLED = 2;





static const TypeDescriptorField PlayAnimationMessageSpec_Fields[6] = {
    TypeDescriptorField("", DT_INT, offsetof(PlayAnimationMessageSpec, dwDuration), NULL, 1), TypeDescriptorField("", DT_SNO, offsetof(PlayAnimationMessageSpec, snoAnim), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayAnimationMessageSpec, dwAnimationTag), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(PlayAnimationMessageSpec, nPermutationIndex), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PlayAnimationMessageSpec, flAnimSpeedScale), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(PlayAnimationMessageSpec), NULL, 0),
};

ComplexTypeDescriptor TypeDescriptor_PlayAnimationMessageSpec("PlayAnimationMessageSpec", 0, PlayAnimationMessageSpec_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAY_ANIMATION_MESSAGE_SPEC = &TypeDescriptor_PlayAnimationMessageSpec;

static const TypeDescriptorField PlayAnimationMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayAnimationMessage, tHeader), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(PlayAnimationMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayAnimationMessage, eAnimReason), NULL, NULL, (const void *)(ANIM_REASON_COUNT - 1)), TypeDescriptorField("", DT_FLOAT, offsetof(PlayAnimationMessage, flUnitAnimStartTime), NULL, 1),
    FixedArrayTypeDescriptorField("tAnim", DT_PLAY_ANIMATION_MESSAGE_SPEC, offsetof(PlayAnimationMessage, tAnim), NULL, 8, 3, -4), TypeDescriptorField(NULL, DT_NULL, sizeof(PlayAnimationMessage), NULL, 0),
};

ComplexTypeDescriptor TypeDescriptor_PlayAnimationMessage("PlayAnimationMessage", 0, PlayAnimationMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAY_ANIMATION_MESSAGE = &TypeDescriptor_PlayAnimationMessage;

static const TypeDescriptorField SetIdleAnimationMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SetIdleAnimationMessage, tHeader), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(SetIdleAnimationMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SetIdleAnimationMessage, dwAnimTag), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(SetIdleAnimationMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SetIdleAnimationMessage("SetIdleAnimationMessage", 0, SetIdleAnimationMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SET_IDLE_ANIMATION_MESSAGE = &TypeDescriptor_SetIdleAnimationMessage;









void AnimPlayerInitializeForGame() {

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    AnimPlayerGlobals *& ptAnimPlayerGlobals = GameGetActiveGameCommonData().ptAnimPlayerGlobals;
    ptAnimPlayerGlobals = (AnimPlayerGlobals *)SigmaMemoryNew(sizeof(AnimPlayerGlobals), 4, ptMemoryInterface, TRUE);
    ptAnimPlayerGlobals->poolAnimPlayer.Initialize();
    ptAnimPlayerGlobals->poolAnimPlayer.SetMemoryInterface(ptMemoryInterface);
    ptAnimPlayerGlobals->poolAnimPlayer.SetBlockCount_Tracked(256);
}





void AnimPlayerDisposeForGame() {

    AnimPlayerGlobals *& ptAnimPlayerGlobals = GameGetActiveGameCommonData().ptAnimPlayerGlobals;
    ptAnimPlayerGlobals->poolAnimPlayer.Dispose();
    SigmaMemoryFree(ptAnimPlayerGlobals, AppGetCurrentMemoryInterface()); ptAnimPlayerGlobals = NULL;
}






















static uint32 sAnimPlayerGetTime() {
    uint32 dwNow;
    if (GameActiveGameCommonDataIsClient()) {
        dwNow = CGameGetClientTicks();
    } else {
        dwNow = GameGetServerTime();
    }
    return dwNow;
}













BOOL AnimPlayerACDIsInConversationAndNotInCombat(const ActorCommonData & tACD) {
    if (!GameActiveGameCommonDataIsClient())
        return FALSE;
    if (tACD.id != LocalPlayerACDGet())
        return FALSE;
    if (!tACD.IsAlive())
        return FALSE;
    if (!tACD.IsInCombat() && UIConversation::ACDIsInConversation(tACD.id, FALSE, FALSE)) {
        SNO snoCurrentAnimPower = CPlayerInputGetCurrentAnimPower(PlayerGetByACD(tACD.id));
        return snoCurrentAnimPower == SNO_NONE ||
               snoCurrentAnimPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK);
    }
    return FALSE;
}



























static void sAnimPlayerPlayExactClient(
    ActorCommonData & tACD,
    uint32 dwAnimationTag,
    SNO snoAnim,
    int32 nPermutationIndex,
    float flAnimSpeedScale,
    float flAnimUnitStartTime)
{
    RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);
    RActor * ptRActor = RActorTryToGet(idRActor);

    if (ptRActor == NULL ||
        ptRActor->ptActorAnim == NULL ||
        !tACD.CanAnimate())
    {
        return;
    }

    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
    BOOL fSheathWeapons = (pdefAnim->dwFlags & (1 << ANIMATION_SHEATH_WEAPONS)) != 0;
    BOOL bHideFistWeapons = (pdefAnim->dwFlags >> ANIMATION_HIDE_FIST_WEAPONS_BIT) & 1;

    fSheathWeapons = fSheathWeapons || tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_WEAPONS_SHEATHED)) || AnimPlayerACDIsInConversationAndNotInCombat(tACD);

    AnimChannelPlayParams tChannelPlayParams;
    AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, nPermutationIndex, flAnimSpeedScale);

    uint32 dwNow = sAnimPlayerGetTime();

    BOOL fDisableBlending = ptRActor->dwCreationTick >= dwNow - TICKS_BLENDING_DISABLED;

    if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {
        AnimationChannelPlay(ptRActor, tChannelPlayParams, fDisableBlending);
        if (flAnimUnitStartTime > EPSILON)
        {
            RActorAnimationSetPercentComplete(*ptRActor, flAnimUnitStartTime);
        }
    }
    else
    {
        if (dwAnimationTag != TAG_ACTOR_ANIMATION_INVALID)
        {
            AnimLayer eAnimLayer = ANIM_LAYER_NONE;
            if (!RActorGetAnimLayerForTag(ptRActor, dwAnimationTag, eAnimLayer))
            {
                char szPath[256];
                SNOName snoname;
                snoname.snoGroup = ANIMATION_SNO;
                snoname.snoHandle = tChannelPlayParams.snoAnim;
                SNOFileGetFilePath(snoname, szPath, sizeof(szPath));
            }

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
            if (eAnimLayer != ANIM_LAYER_NONE)
            {
                uint32 dwAnimLayerHash = RActorGetAnimTreeLayerHash(eAnimLayer);
                BOOL bImmediatelyActive = RActorGetAnimTreeLayerImmediatelyActive(eAnimLayer);
                BOOL bManualRampOut = RActorGetAnimTreeManualRampOut(ptRActor, dwAnimationTag);

                if (!fDisableBlending && eAnimLayer == ANIM_LAYER_NPC &&
                    !RActorAnimTreeLayerIsActive(*ptRActor, ANIM_LAYER_CACHE))
                {
                    uint32 dwCacheLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_CACHE);
                    AnimTreeLayerSetPoseFromLayer(ptRActor, dwCacheLayerHash, dwAnimLayerHash);
                }

                AnimTreeLayerPlay(ptRActor, dwAnimLayerHash, tChannelPlayParams, bImmediatelyActive, bManualRampOut);

                if (eAnimLayer == ANIM_LAYER_NPC && fDisableBlending)
                {
                    uint32 dwCacheLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_CACHE);
                    AnimTreeLayerSetPoseFromLayer(ptRActor, dwCacheLayerHash, dwAnimLayerHash);
                }
            }

            uint32 dwAdditionalTag;
            AnimTagGetAdditionalTag(dwAnimationTag, dwAdditionalTag);
            if (dwAdditionalTag != TAG_ACTOR_ANIMATION_INVALID)
            {
                SNO snoAdditionalAnim = tACD.GetSpecificAnim(dwAdditionalTag);
                if (snoAdditionalAnim != SNO_NONE)
                {
                    int32 nAdditionalPermutationIndex = 0;
                    sAnimPlayerPlayExactClient(tACD, dwAdditionalTag, snoAdditionalAnim, nAdditionalPermutationIndex, flAnimSpeedScale, flAnimUnitStartTime);
                }
            }
        }
        else
        {
            char szPath[256];
            SNOName snoname;
            snoname.snoGroup = ANIMATION_SNO;
            snoname.snoHandle = tChannelPlayParams.snoAnim;
            SNOFileGetFilePath(snoname, szPath, sizeof(szPath));
        }
    }

    BOOL bUpdateInventory = FALSE;
    BOOL fWeaponsAlreadySheathed = (ptRActor->dwFlags & (1 << RACTOR_WEAPONS_SHEATHED_BIT)) != 0;
    if (fSheathWeapons != fWeaponsAlreadySheathed)
    {
        if (fSheathWeapons)
            ptRActor->dwFlags |= (1 << RACTOR_WEAPONS_SHEATHED_BIT);
        else
            ptRActor->dwFlags &= ~(1 << RACTOR_WEAPONS_SHEATHED_BIT);
        bUpdateInventory = TRUE;
    }
    BOOL bFistWeaponsAlreadyHidden = (ptRActor->dwFlags & (1 << RACTOR_HIDE_FIST_WEAPONS_BIT)) != 0;
    if (bFistWeaponsAlreadyHidden != bHideFistWeapons)
    {
        if (bHideFistWeapons)
            ptRActor->dwFlags |= (1 << RACTOR_HIDE_FIST_WEAPONS_BIT);
        else
            ptRActor->dwFlags &= ~(1 << RACTOR_HIDE_FIST_WEAPONS_BIT);
        bUpdateInventory = TRUE;
    }
    if (bUpdateInventory)
    {
        VisualInventoryUpdateSheathed(tACD.id);
    }
}













































static void sAnimPlayerPlayExactServer(
    ActorCommonData & tACD,
    BOOL bTranslate,
    uint32 dwDuration,
    Vector3D wvVelocity)
{
    if (!bTranslate)
        return;

    WorldDistance wdSpeed = wvVelocity.Magnitude();
    if (wdSpeed > EPSILON)
    {
        Angle aYaw = ACDGetYaw(tACD.id);
        Vector3D wvVelocityWorldSpace(wvVelocity);
        Vector3D::Rotate(cgwvZAxis, aYaw, wvVelocityWorldSpace);

        WorldPlace tPlace = tACD.GetPlace();
        WorldPlace placeDest;
        WorldPlaceOffset(tPlace, wvVelocityWorldSpace * (float)dwDuration, placeDest);

        ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
        PowerID idPower = ActorPowersGetPrimary(idActor);
        Power * ptPower = PowerTryToGet(idPower);
        float flSpeed = Math::Max(0.0001f, wvVelocity.Magnitude());

        Actor * ptActor = ActorGet(idActor);
        if (ptActor->ptSActorMovement)
        {
            int32 nMaxTicksToWalk = -1;
            uint32 uMoveFlags = (1 << MOVEBIT_NO_PATHFINDING) | (1 << MOVEBIT_NO_STEERING) | (1 << MOVEBIT_NO_SNAP_TO_GROUND) | (1 << MOVEBIT_FROM_POWER) | (1 << MOVEBIT_DONT_AUTOPLAY_IDLE);
            SNO snoPower = SNO_NONE;
            if (ptPower)
            {
                snoPower = ptPower->snoPower;
                if (PowerChecksVerticalMovement(snoPower))
                {
                    uMoveFlags |= (1 << MOVEBIT_CHECK_VERTICAL);
                }
                if (PowerSnapsToGround(snoPower))
                {
                    uMoveFlags &= ~(1 << MOVEBIT_NO_SNAP_TO_GROUND);
                }
                if (PowerGetChargeSteeringType(snoPower) != 0)
                {
                    uMoveFlags |= (1 << MOVEBIT_USE_CHARGE_STEERING);
                    uMoveFlags &= ~(1 << MOVEBIT_NO_STEERING);
                    if (PowerGetChargeSteeringType(snoPower) == 1)
                    {
                        uMoveFlags |= (1 << MOVEBIT_DYNAMIC_PATHING);
                        uMoveFlags &= ~(1 << MOVEBIT_NO_PATHFINDING);
                    }
                    nMaxTicksToWalk = dwDuration;
                    ActorSteer & tSteer = ptActor->ptSActorMovement->GetSteer();
                    tSteer.Reset180Steering(Vector2D(0.0f, 0.0f));
                }
            }
            if (ACDIsPlayer(tACD.id) ||
                IsTriggerEventTargetingActor(idActor))
            {
                uMoveFlags |= (1 << MOVEBIT_ACTIVATE_TRIGGERS);
            }
            ptActor->ptSActorMovement->SnapPlaceToGround(placeDest);
            ptActor->ptSActorMovement->MoveTo(placeDest, flSpeed, 0.0f, TAG_ACTOR_ANIMATION_INVALID, uMoveFlags, 0.0f, ACDID_INVALID, snoPower, 0, nMaxTicksToWalk, 100.0f, NULL);
        }
    }
}
























static void sAnimPlayerStartCurrentAnim(
    ActorCommonData & tACD,
    float flAnimUnitStartTime)
{


    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    AnimSpec & tCurrentAnim = tAnimPlayer.tAnimSpecArray[0];

    uint32 dwNow = sAnimPlayerGetTime();

    uint32 dwDuration;
    Vector3D wvVelocity;



    tAnimPlayer.dwAnimationStartTime = dwNow;

    if (tCurrentAnim.dwDuration == ANIM_PLAY_FOREVER)
    {
        tAnimPlayer.dwAnimationEndTime = ANIM_PLAY_FOREVER;
        dwDuration = 0;
        wvVelocity = cgwvNull;
    }
    else
    {
        dwDuration = tCurrentAnim.dwDuration;
        tAnimPlayer.dwAnimationEndTime = dwNow + (uint32)((1.0f - flAnimUnitStartTime) * dwDuration);

        wvVelocity = tCurrentAnim.wvVelocity;
    }

    if (GameActiveGameCommonDataIsClient())
    {

        sAnimPlayerPlayExactClient(
            tACD,
            tCurrentAnim.dwAnimationTag,
            tCurrentAnim.snoAnim,
            tCurrentAnim.nPermutationIndex,
            tCurrentAnim.flAnimSpeedScale,
            flAnimUnitStartTime);
    }
    else
    {
        sAnimPlayerPlayExactServer(
            tACD,
            tAnimPlayer.bUseAnimationTranslation,
            dwDuration,
            wvVelocity);
    }
}













static void sAnimPlayerAnimationCompleted(ActorCommonData & tACD)
{
    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;



    if (tAnimPlayer.nNumQueuedAnimations == 1)
    {

        if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_COULD_HAVE_RAGDOLLED)))
        {

            if (GameActiveGameCommonDataIsClient())
            {

                RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);
                RActor * ptRActor = RActorTryToGet(idRActor);
                if (ptRActor)
                {


                    RagdollType ragdollType = RActorGetRagdollType(ptRActor);


                    if (ragdollType != 0 && ragdollType != 3)
                    {
                        tAnimPlayer.dwAnimationEndTime = ANIM_PLAY_FOREVER;
                        return;
                    }
                }
            }
            else
            {
                tAnimPlayer.dwAnimationEndTime = ANIM_PLAY_FOREVER;
                return;
            }
        }
        AnimPlayerPlayIdle(tACD);
        return;
    }




    tAnimPlayer.nNumQueuedAnimations--;

    SigmaMemoryMove(&tAnimPlayer.tAnimSpecArray[0], &tAnimPlayer.tAnimSpecArray[1], sizeof(AnimSpec) * tAnimPlayer.nNumQueuedAnimations);
    sAnimPlayerStartCurrentAnim(tACD, 0.0f);
}



















static void sAnimPlayerSetToIdle(ActorCommonData & tACD, const uint32 dwIdleAnimationTag, AnimPlayer & tAnimPlayer) {
    SigmaMemoryClear(&tAnimPlayer, sizeof(AnimPlayer));
    tAnimPlayer.dwIdleAnimationTag = dwIdleAnimationTag;
    tAnimPlayer.nNumQueuedAnimations = 1;
    tAnimPlayer.dwAnimationEndTime = ANIM_PLAY_FOREVER;
    tAnimPlayer.dwAnimationTag = dwIdleAnimationTag;
    tAnimPlayer.bUseAnimationTranslation = FALSE;
    tAnimPlayer.eAnimReason = ANIM_REASON_IDLE;
    tAnimPlayer.tAnimSpecArray[0].dwDuration = ANIM_PLAY_FOREVER;
    tAnimPlayer.tAnimSpecArray[0].dwAnimationTag = dwIdleAnimationTag;
    tAnimPlayer.tAnimSpecArray[0].snoAnim = tACD.GetSpecificAnim(dwIdleAnimationTag);
    tAnimPlayer.tAnimSpecArray[0].nPermutationIndex = 0;
    tAnimPlayer.tAnimSpecArray[0].flAnimSpeedScale = 1.0f;
    tAnimPlayer.tAnimSpecArray[0].wvVelocity = cgwvNull;
}











void AnimPlayerInit(ActorCommonData & tACD) {
    if (!tACD.IsInWorld())
        return;
    SNO snoAnim = tACD.GetSpecificAnim(TAG_ACTOR_ANIMATION_NEUTRAL);
    if (snoAnim == SNO_NONE)
        return;
    AnimPlayerGlobals * ptAnimPlayerGlobals = GameGetActiveGameCommonData().ptAnimPlayerGlobals;
    tACD.ptAnimPlayer = ptAnimPlayerGlobals->poolAnimPlayer.Alloc_Tracked();
    sAnimPlayerSetToIdle(tACD, TAG_ACTOR_ANIMATION_NEUTRAL, *tACD.ptAnimPlayer);
}

















void AnimPlayerDispose(ActorCommonData & tACD) {
    if (tACD.ptAnimPlayer == NULL)
        return;
    AnimPlayerGlobals * ptAnimPlayerGlobals = GameGetActiveGameCommonData().ptAnimPlayerGlobals;
    ptAnimPlayerGlobals->poolAnimPlayer.Free(tACD.ptAnimPlayer);
    tACD.ptAnimPlayer = NULL;
}









BOOL AnimPlayerGetAnimationTiming(ActorCommonData & tACD, const uint32 dwAnimationTag, const uint32 dwLength, const float flSpeedScalar, AnimationTiming & tAnimationTiming) {
    RNG * prng;
    if (GameActiveGameCommonDataIsClient()) {
        prng = &CGameGlobalsGet().rngLocal;
    } else {
        prng = &SGameGetRNG();
    }
    return AnimPlayerGetAnimationTiming(tACD, dwAnimationTag, dwLength, flSpeedScalar, *prng, tAnimationTiming);
}
























void AnimPlayerGetScaledAnimationTimingForMovingPower(
    SNO snoPower,
    ActorCommonData & tACD,
    const uint32 dwAnimationTag,
    const uint32 dwLength,
    const WorldPlace wpDestination,
    const float flSpeedScalar,
    RNG & rng,
    AnimationTiming & tAnimationTiming)
{
    AnimPlayerGetAnimationTiming(tACD, dwAnimationTag, dwLength, flSpeedScalar, rng, tAnimationTiming);

    if (wpDestination.wp != cgwpNull)
    {
        Vector3D wvVelocity(tAnimationTiming.tAnimSpec[0].wvVelocity);
        WorldPlace wpStart = tACD.GetPlace();
        if (wvVelocity.Magnitude() > EPSILON)
        {
            WorldDistance wdNormal = wvVelocity.Magnitude() * tAnimationTiming.tAnimSpec[0].dwDuration;

            WorldDistance wdDesired = Vector2D::Distance(tACD.GetPlace().wp.As2D(), wpDestination.wp.As2D());


            WorldDistance wdTraveled = Math::Abs(tAnimationTiming.tAnimSpec[1].wvVelocity.z) * tAnimationTiming.tAnimSpec[1].dwDuration +
                                       Math::Abs(tAnimationTiming.tAnimSpec[2].wvVelocity.z) * tAnimationTiming.tAnimSpec[2].dwDuration;

            wdDesired -= wdTraveled;

            float flAdjustedSpeedScalar = flSpeedScalar;
            uint32 dwAdjustedLength = dwLength;



            float flMinScale = 0.75f;
            if (PowerGeneralTagExists(snoPower, TAG_POWER_ANIMATION_MIN_SCALING))
            {
                flMinScale = PowerGeneralTagGetFloat(snoPower, TAG_POWER_ANIMATION_MIN_SCALING);
            }

            float flMaxScale = 1.25f;
            if (PowerGeneralTagExists(snoPower, TAG_POWER_ANIMATION_MAX_SCALING))
            {
                flMaxScale = PowerGeneralTagGetFloat(snoPower, TAG_POWER_ANIMATION_MAX_SCALING);
            }

            flAdjustedSpeedScalar = Math::Pin(wdNormal / wdDesired * flSpeedScalar, flMinScale * flSpeedScalar, flMaxScale * flSpeedScalar);

            AnimPlayerGetAnimationTiming(tACD, dwAnimationTag, dwAdjustedLength, flAdjustedSpeedScalar, rng, tAnimationTiming);

            WorldDistance wdNewSpeed = wdDesired / tAnimationTiming.tAnimSpec[0].dwDuration;
            tAnimationTiming.tAnimSpec[0].wvVelocity = wvVelocity * (wdNewSpeed / wvVelocity.Magnitude());
        }
    }
}




BOOL AnimPlayerGetAnimationTiming(
    ActorCommonData & tACD,
    const uint32 dwAnimationTag,
    const uint32 dwLength,
    const float flSpeedScalar,
    RNG & rng,
    AnimationTiming & tAnimationTiming)
{
    tAnimationTiming.dwAnimationTag = dwAnimationTag;

    if (dwAnimationTag == TAG_ACTOR_ANIMATION_INVALID)
    {
        for (int i = 0; i < 3; i++)
        {

            tAnimationTiming.tAnimSpec[i].dwAnimationTag = TAG_ACTOR_ANIMATION_INVALID;
            tAnimationTiming.tAnimSpec[i].dwDuration = 0; tAnimationTiming.tAnimSpec[i].snoAnim = SNO_NONE;
            tAnimationTiming.tAnimSpec[i].wvVelocity = cgwvNull;
            tAnimationTiming.tAnimSpec[i].nPermutationIndex = 0;
            tAnimationTiming.tAnimSpec[i].flAnimSpeedScale = 1.0f;
        }

        tAnimationTiming.tAnimSpec[0].dwDuration = 60;
        tAnimationTiming.dwCycleTime = 0;
        tAnimationTiming.dwInterruptTime = 0;
        for (int i = 0; i < 4; i++)
        {
            tAnimationTiming.dwExecuteTime[i] = tAnimationTiming.dwStartAnimExecuteTime[i] = tAnimationTiming.dwEndAnimExecuteTime[i] = 0;
        }
        return FALSE;
    }









    SNO snoAnimSet[3];
    uint32 dwAnimSetTag[3];
    tACD.GetSpecificAnimSet(dwAnimationTag, snoAnimSet, dwAnimSetTag);

    for (int i = 0; i < 3; i++)
    {
        AnimSpec & tAnimSpec = tAnimationTiming.tAnimSpec[i];
        tAnimSpec.snoAnim = snoAnimSet[i];
        tAnimSpec.dwAnimationTag = dwAnimSetTag[i];

        if (tAnimSpec.snoAnim != SNO_NONE)
        {
            AnimChannelPlayParams tChannelPlayParams;

            ScopedAnimationDefinitionLockR pdefAnim(tAnimSpec.snoAnim);

            AnimationSetChannelPlayParams(tChannelPlayParams, pdefAnim, rng);

            tChannelPlayParams.flAnimSpeedScale *= flSpeedScalar;

            tAnimSpec.nPermutationIndex = tChannelPlayParams.nPermutationIndex;
            tAnimSpec.flAnimSpeedScale = tChannelPlayParams.flAnimSpeedScale;

            tAnimSpec.dwDuration = AnimGetDurationTimeFromPlayParams(tChannelPlayParams, pdefAnim);

            if (GameActiveGameCommonDataIsServer())
            {
                if (!tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_COULD_HAVE_RAGDOLLED)) &&
                    AnimCanRagdollFromPlayParams(tChannelPlayParams, pdefAnim))
                {
                    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_COULD_HAVE_RAGDOLLED), 1);
                }
            }

            AnimationGetAvgVelocity(
                pdefAnim,
                tChannelPlayParams.nPermutationIndex,
                flSpeedScalar,
                TRUE,
                tAnimSpec.wvVelocity);

            if (i == 0)
            {
                tAnimationTiming.dwInterruptTime = AnimGetInterruptTimeFromPlayParams(tChannelPlayParams, pdefAnim);

                for (int f = 0; f < 4; f++)
                {
                    tAnimationTiming.dwExecuteTime[f] = AnimGetContactTimeFromPlayParams(tChannelPlayParams, f, pdefAnim);
                }

                tAnimationTiming.dwCycleTime = tAnimSpec.dwDuration;
            }

            if (i == 2)
            {
                for (int f = 0; f < 4; f++)
                {
                    tAnimationTiming.dwEndAnimExecuteTime[f] = AnimGetContactTimeFromPlayParams(tChannelPlayParams, f, pdefAnim);
                }
            }

            if (i == 1)
            {
                for (int f = 0; f < 4; f++)
                {
                    tAnimationTiming.dwStartAnimExecuteTime[f] = AnimGetContactTimeFromPlayParams(tChannelPlayParams, f, pdefAnim);
                }
            }
        }
        else
        {
            tAnimSpec.dwDuration = 0;
            tAnimSpec.wvVelocity = cgwvNull;
            tAnimSpec.nPermutationIndex = 0; tAnimSpec.flAnimSpeedScale = 1.0f;
        }
    }


    AnimSpec & tAnimSpecContinue = tAnimationTiming.tAnimSpec[0];
    AnimSpec & tAnimSpecStart = tAnimationTiming.tAnimSpec[1];
    AnimSpec & tAnimSpecEnd = tAnimationTiming.tAnimSpec[2];

    tAnimationTiming.flUnitAnimStartTime = 0.0f;

    if (tAnimSpecContinue.snoAnim == SNO_NONE)
    {
        if (GameActiveGameCommonDataIsClient())
        {

            if (!AnimIsDeathAnim(dwAnimationTag))
            {  // the call survives; what it guarded (presumably a trace) is not in this build
            }
        }



        tAnimSpecContinue.snoAnim = tACD.GetSpecificAnim(TAG_ACTOR_ANIMATION_NEUTRAL);
        tAnimSpecContinue.dwAnimationTag = TAG_ACTOR_ANIMATION_NEUTRAL;
        tAnimSpecContinue.dwDuration = tAnimSpecContinue.snoAnim != SNO_NONE ? 60 : 0;
        tAnimSpecContinue.nPermutationIndex = 0;







        tAnimSpecContinue.flAnimSpeedScale = 1.0f;
        tAnimationTiming.dwCycleTime = tAnimSpecContinue.dwDuration;
        tAnimationTiming.dwInterruptTime = 0;
        for (int f = 0; f < 4; f++)
        {
            tAnimationTiming.dwExecuteTime[f] = tAnimationTiming.dwStartAnimExecuteTime[f] = tAnimationTiming.dwEndAnimExecuteTime[f] = 0;
        }
        return FALSE;
    }






    if (dwLength == ANIM_PLAY_FOREVER)
    {


        tAnimSpecContinue.dwDuration = ANIM_PLAY_FOREVER;
        tAnimSpecContinue.wvVelocity = cgwvNull;
    }
    else if (dwLength != ANIM_USE_ANIM_LENGTH)
    {
        int32 nCalcuatedDuration = dwLength - tAnimSpecStart.dwDuration - tAnimSpecEnd.dwDuration;
        tAnimSpecContinue.dwDuration = Math::Max(nCalcuatedDuration, 0);
        tAnimationTiming.dwCycleTime = tAnimSpecContinue.dwDuration;
    }



    for (int f = 0; f < 4; f++)
    {

        if (tAnimationTiming.dwExecuteTime[f] == 0xffffffff)
        {
            tAnimationTiming.dwExecuteTime[f] = tAnimationTiming.dwCycleTime;
        }
        tAnimationTiming.dwExecuteTime[f] = Math::Min(tAnimationTiming.dwExecuteTime[f], tAnimationTiming.dwCycleTime);


        if (tAnimationTiming.dwStartAnimExecuteTime[f] == 0xffffffff)
        {
            tAnimationTiming.dwStartAnimExecuteTime[f] = tAnimSpecStart.dwDuration;
        }
        tAnimationTiming.dwStartAnimExecuteTime[f] = Math::Min(tAnimationTiming.dwStartAnimExecuteTime[f], tAnimSpecStart.dwDuration);


        if (tAnimationTiming.dwEndAnimExecuteTime[f] == 0xffffffff)
        {
            tAnimationTiming.dwEndAnimExecuteTime[f] = tAnimSpecEnd.dwDuration;
        }
        tAnimationTiming.dwEndAnimExecuteTime[f] = Math::Min(tAnimationTiming.dwEndAnimExecuteTime[f], tAnimSpecEnd.dwDuration);
    }


    if (tAnimationTiming.dwInterruptTime == 0xffffffff)
    {
        tAnimationTiming.dwInterruptTime = tAnimationTiming.dwCycleTime;
    }

    uint32 dwLastContact = 0;

    for (int i = 0; i < 4; i++)
    {
        dwLastContact = tAnimationTiming.dwExecuteTime[i] > dwLastContact ? tAnimationTiming.dwExecuteTime[i] : dwLastContact;
    }




    tAnimationTiming.dwInterruptTime = Math::Pin(tAnimationTiming.dwInterruptTime, dwLastContact, tAnimationTiming.dwCycleTime);


    return TRUE;
}


static void sAnimPlayerSendAnimationsToPlayers(ActorCommonData & tACD, AnimPlayer & tAnimPlayer, const PlayerList & listDestPlayers, const AnimReason eAnimReason) {




    PlayAnimationMessage tMessage;





    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.eAnimReason = eAnimReason;





    tMessage.nUsedAnimArrayElements = tAnimPlayer.nNumQueuedAnimations;

    for (int32 i = 0; i < tAnimPlayer.nNumQueuedAnimations; i++) {

        AnimSpec & tTimingSpec = tAnimPlayer.tAnimSpecArray[i];
        PlayAnimationMessageSpec & tAnimMessage = tMessage.tAnim[i];



        tAnimMessage.dwAnimationTag = tTimingSpec.dwAnimationTag;
        tAnimMessage.dwDuration = tTimingSpec.dwDuration;
        tAnimMessage.snoAnim = tTimingSpec.snoAnim;
        tAnimMessage.nPermutationIndex = tTimingSpec.nPermutationIndex;
        tAnimMessage.flAnimSpeedScale = tTimingSpec.flAnimSpeedScale;
    }



    tMessage.flUnitAnimStartTime = AnimPlayerGetUnitTime(tACD);

    MessageSendToClient(listDestPlayers, SMSG_PLAY_ANIMATION, &tMessage, sizeof(tMessage));
}



static void sAnimPlayerSendAnimationsToClient(ActorCommonData & tACD, const BOOL fAnimWasPreplayed, const AnimReason eAnimReason)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, tACD.id, FALSE);

    if (fAnimWasPreplayed &&
        !ReplayIsPlaying())
    {

        Player * ptPlayer = PlayerGetByACD(tACD.id);

        POS pos = listDestPlayers.Find(ptPlayer);
        if (pos)
        {
            listDestPlayers.RemoveAt(pos);
        }
    }

    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    sAnimPlayerSendAnimationsToPlayers(tACD, tAnimPlayer, listDestPlayers, eAnimReason);
}













static int32 sAnimPlayerQueueAnim(
    ActorCommonData & tACD,
    int32 nOffset,
    int32 & nAnimIndex,
    AnimationTiming & tAnimationTiming)
{
    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    AnimSpec & tTimingSpec = tAnimationTiming.tAnimSpec[nOffset];

    if (tTimingSpec.dwDuration == 0)
    {
        return 0;
    }



    AnimSpec & tPlayerSpec = tAnimPlayer.tAnimSpecArray[nAnimIndex];

    tPlayerSpec = tTimingSpec;

    nAnimIndex++;

    return tTimingSpec.dwDuration;
}




uint32 AnimPlayerPlayAnimationsWithTiming(
    ActorCommonData & tACD,
    AnimationTiming * ptAnimationTiming,
    int32 nNumAnims,
    BOOL bUseAnimationTranslation,
    AnimReason eAnimReason,
    BOOL fAnimWasPreplayed,
    BOOL bDontSendToClient)
{
    if (tACD.ptAnimPlayer == NULL)
    {
        return 0;
    }

    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;



    if (eAnimReason == ANIM_REASON_IDLE &&
        tAnimPlayer.eAnimReason == ANIM_REASON_IDLE)
    {
        AnimSpec & tCurrentAnim = tAnimPlayer.tAnimSpecArray[0];
        AnimSpec & tNewAnim = ptAnimationTiming->tAnimSpec[0];
        if (tCurrentAnim.snoAnim == tNewAnim.snoAnim ||
            tCurrentAnim.dwDuration != ANIM_PLAY_FOREVER)
        {
            bDontSendToClient = TRUE;
        }
    }



    else if (eAnimReason == ANIM_REASON_IDLE &&
             tAnimPlayer.eAnimReason == ANIM_REASON_POWER &&
             ACDIsPlayer(tACD.id))
    {

        AnimSpec & tCurrentAnim = tAnimPlayer.tAnimSpecArray[0];
        if (tCurrentAnim.dwDuration != ANIM_PLAY_FOREVER)
        {
            bDontSendToClient = TRUE;
        }
    }






    tAnimPlayer.bUseAnimationTranslation = bUseAnimationTranslation;
    tAnimPlayer.eAnimReason = eAnimReason;

    int32 nAnimIndex = 0;
    uint32 dwPlayLength = 0;

    for (int32 i = 0; i < nNumAnims; i++)
    {
        dwPlayLength += sAnimPlayerQueueAnim(tACD, 1, nAnimIndex, ptAnimationTiming[i]);
        dwPlayLength += sAnimPlayerQueueAnim(tACD, 0, nAnimIndex, ptAnimationTiming[i]);
        dwPlayLength += sAnimPlayerQueueAnim(tACD, 2, nAnimIndex, ptAnimationTiming[i]);
    }

    if (nAnimIndex <= 0)
    {
        return 0;
    }

    tAnimPlayer.nNumQueuedAnimations = nAnimIndex;

    sAnimPlayerStartCurrentAnim(tACD, ptAnimationTiming->flUnitAnimStartTime);
    tAnimPlayer.dwAnimationTag = ptAnimationTiming->dwAnimationTag;

    if (GameActiveGameCommonDataIsServer() && !bDontSendToClient)
    {
        if (eAnimReason == ANIM_REASON_IDLE)
        {
            AnimPlayerSendIdle(tACD);
        }
        else
        {
            sAnimPlayerSendAnimationsToClient(tACD, fAnimWasPreplayed, eAnimReason);
        }
    }

    return dwPlayLength;
}



















uint32 AnimPlayerPlayAnimation(
    ActorCommonData & tACD,
    uint32 dwAnimationTag,
    uint32 dwLength,
    BOOL bUseAnimationTranslation,
    AnimReason eAnimReason,
    float flUnitAnimStartTime,
    const BOOL bDontSendToClient,
    float flSpeedScalar)
{
    if (tACD.ptAnimPlayer == NULL)
    {
        return dwLength == ANIM_USE_ANIM_LENGTH ? 0 : dwLength;
    }

    AnimationTiming tAnimationTiming;
    AnimPlayerGetAnimationTiming(tACD, dwAnimationTag, dwLength, flSpeedScalar, tAnimationTiming);
    tAnimationTiming.flUnitAnimStartTime = flUnitAnimStartTime;

    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    if (dwLength == ANIM_PLAY_FOREVER)
    {
        AnimSpec & tCurrentAnim = tAnimPlayer.tAnimSpecArray[0];
        AnimSpec & tNewAnim = tAnimationTiming.tAnimSpec[0];
        if (tCurrentAnim.snoAnim == tNewAnim.snoAnim &&
            tCurrentAnim.dwDuration == ANIM_PLAY_FOREVER &&
            tAnimPlayer.eAnimReason == eAnimReason)
        {
            return ANIM_PLAY_FOREVER;
        }
    }

    float flPercentComplete = 0.0f;
    BOOL bRetainPercentComplete = FALSE;
    if (tAnimPlayer.eAnimReason == ANIM_REASON_WALKING && eAnimReason == ANIM_REASON_WALKING)
    {
        bRetainPercentComplete = TRUE;
        RActor * ptRActor = RActorGet((RActorID)ACDGetOwner(tACD.id));
        if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
        {
            flPercentComplete = RActorAnimationGetPercentComplete(*ptRActor);
        }
    }

    uint32 dwPlayLength = AnimPlayerPlayAnimationsWithTiming(tACD, &tAnimationTiming, 1, bUseAnimationTranslation, eAnimReason, FALSE, bDontSendToClient);
    if (bRetainPercentComplete)
    {
        RActor * ptRActor = RActorGet((RActorID)ACDGetOwner(tACD.id));
        if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
        {
            RActorAnimationSetPercentComplete(*ptRActor, flPercentComplete);
        }
    }
    return dwPlayLength;
}





























static void sAnimPlayerSendSetIdleAnimation(ActorCommonData & tACD, const AnimPlayer & tAnimPlayer, const PlayerList & listDestPlayers) {

    SetIdleAnimationMessage tMessage;



    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.dwAnimTag = tAnimPlayer.dwIdleAnimationTag;

    MessageSendToClient(listDestPlayers, SMSG_ACD_SET_IDLE_ANIM, &tMessage, sizeof(tMessage));
}




uint32 AnimPlayerGetIdleAnimation(ActorCommonData & tACD) {
    if (tACD.ptAnimPlayer) {
        AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
        return tAnimPlayer.dwIdleAnimationTag;
    }
    return 0;
}









void AnimPlayerSetIdleAnimation(ActorCommonData & tACD, uint32 dwIdleAnimationTag)
{
    if (tACD.ptAnimPlayer == NULL)
        return;

    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    if (tAnimPlayer.dwIdleAnimationTag == dwIdleAnimationTag)
        return;

    AnimPlayerIdleAnimationChanged(tACD, dwIdleAnimationTag);

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, tACD.id, FALSE);

    sAnimPlayerSendSetIdleAnimation(tACD, tAnimPlayer, listDestPlayers);
    ActorMovement * ptMovement = tACD.GetActorMovement();
    if (ptMovement)
    {
        ptMovement->ResetCache();
    }
}
















void AnimPlayerPlayIdle(ActorCommonData & tACD)
{
    if (tACD.ptAnimPlayer == NULL)
        return;

    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    uint32 dwAnimTag = tAnimPlayer.dwIdleAnimationTag;

    if (GameActiveGameCommonDataIsClient())
    {
        if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCE_GRIPPED)) && tACD.IsAlive() && !tACD.IsAttached())
        {
            if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_KNOCK_BACK))
            {
                dwAnimTag = TAG_ACTOR_ANIMATION_KNOCK_BACK;
            }
        }
        else if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_STUNNED)) && tACD.IsAlive() && !tACD.IsAttached())
        {
            if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_STUNNED))
            {
                dwAnimTag = TAG_ACTOR_ANIMATION_STUNNED;
            }
        }
        else if (dwAnimTag == TAG_ACTOR_ANIMATION_NEUTRAL)
        {
            if (ACDIsPlayer(tACD.id) && ACDIsInTown(tACD) && AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL))
            {
                dwAnimTag = TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL;
            }
            if (AnimPlayerACDIsInConversationAndNotInCombat(tACD) && AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_EMOTE_IDLE))
            {
                dwAnimTag = TAG_ACTOR_ANIMATION_EMOTE_IDLE;
            }
        }
    }
    AnimPlayerPlayAnimation(tACD, dwAnimTag, ANIM_PLAY_FOREVER, FALSE, ANIM_REASON_IDLE, 0.0f, FALSE, 1.0f);
}















void AnimPlayerUpdate(ActorCommonData & tACD)
{
    if (tACD.ptAnimPlayer == NULL)
        return;

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FROZEN)))
    {
        if (tACD.ptAnimPlayer->eAnimReason != ANIM_REASON_DEATH)
            return;

        if (GameActiveGameCommonDataIsClient())
        {
            RActor * ptRActor = RActorTryToGet((RActorID)tACD.idOwner);
            if (ptRActor)
            {
                RActorAnimationSetPaused(*ptRActor, FALSE);
            }
        }
        FastAttribSetValueFloat(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_FROZEN), 0.0f);
    }

    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    if (tAnimPlayer.dwAnimationEndTime == ANIM_PLAY_FOREVER)
        return;

    uint32 dwNow = sAnimPlayerGetTime();

    while (TRUE)
    {
        int32 nDifference = dwNow - tAnimPlayer.dwAnimationEndTime;
        if (nDifference < 0)
            break;

        if (GameActiveGameCommonDataIsClient() &&
            tACD.idOwner != RACTORID_INVALID)
        {
            RActor * ptRActor = RActorGet((RActorID)tACD.idOwner);
            RActorPlayRemainingKeyframedRMessages(ptRActor);
        }

        sAnimPlayerAnimationCompleted(tACD);

        if (tAnimPlayer.dwAnimationEndTime == ANIM_PLAY_FOREVER)
            return;

        tAnimPlayer.dwAnimationEndTime -= nDifference;
    }
}





























void AnimPlayerIdleAnimationChanged(ActorCommonData & tACD, const uint32 dwAnimTag) {
    if (tACD.ptAnimPlayer == NULL)
        return;
    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    if (tAnimPlayer.dwIdleAnimationTag == dwAnimTag)
        return;
    tAnimPlayer.dwIdleAnimationTag = dwAnimTag;
    if (tAnimPlayer.eAnimReason == ANIM_REASON_IDLE)
        AnimPlayerPlayIdle(tACD);
}



















void AnimPlayerUpdateGatePosition(ActorCommonData & tACD)
{
    float flValue = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_GATE_POSITION));
    if (flValue > 0.0f)
    {
        RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);
        RActor * ptRActor = RActorGet(idRActor);
        if (ptRActor->ptActorAnim)
        {
            AnimationChannelSetPercentComplete(ptRActor->ptActorAnim->tAnimChannel, flValue);
            ptRActor->ptActorAnim->tAnimChannel.SetDirectlyControlled(TRUE);
        }
    }
}






void AnimPlayerSendAnimations(ActorCommonData & tACD, const PlayerList & listDestPlayers) {


    if (tACD.IsInInventory())
        return;



    if (tACD.ptAnimPlayer == NULL)
        return;

    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;




    if (tAnimPlayer.dwIdleAnimationTag != TAG_ACTOR_ANIMATION_NEUTRAL) {

        sAnimPlayerSendSetIdleAnimation(tACD, tAnimPlayer, listDestPlayers);
    }

    if (tAnimPlayer.eAnimReason != ANIM_REASON_IDLE) {






        sAnimPlayerSendAnimationsToPlayers(tACD, tAnimPlayer, listDestPlayers, tAnimPlayer.eAnimReason);
    }
}




void AnimPlayerSendIdleAnimation(ActorCommonData & tACD, const PlayerList & listDestPlayers) {



    if (tACD.ptAnimPlayer == NULL)
        return;

    AnimPlayer tAnimPlayer;



    sAnimPlayerSetToIdle(tACD, tACD.ptAnimPlayer->dwIdleAnimationTag, tAnimPlayer);

    sAnimPlayerSendSetIdleAnimation(tACD, tAnimPlayer, listDestPlayers);
    sAnimPlayerSendAnimationsToPlayers(tACD, tAnimPlayer, listDestPlayers, tAnimPlayer.eAnimReason);
}




void AnimPlayerSendAnimationToPlayer(
    ActorCommonData & tACD,
    uint32 dwAnimationTag,
    uint32 dwLength,
    const AnimReason eAnimReason,
    const Player & tPlayer)
{








    PlayAnimationMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.eAnimReason = eAnimReason;
    tMessage.flUnitAnimStartTime = 0.0f; tMessage.nUsedAnimArrayElements = 0;


    AnimationTiming tAnimationTiming;
    AnimPlayerGetAnimationTiming(tACD, dwAnimationTag, dwLength, 1.0f, tAnimationTiming);

    const int eOrderedOffsets[3] = { 1, 0, 2 };

    for (int i = 0; i < 3; i++)
    {
        AnimSpec & tAnimSpec = tAnimationTiming.tAnimSpec[eOrderedOffsets[i]];
        if (tAnimSpec.snoAnim != SNO_NONE)
        {
            PlayAnimationMessageSpec & tAnimMessage = tMessage.tAnim[tMessage.nUsedAnimArrayElements];
            tMessage.nUsedAnimArrayElements++;

            tAnimMessage.dwDuration = tAnimSpec.dwDuration;
            tAnimMessage.snoAnim = tAnimSpec.snoAnim;
            tAnimMessage.dwAnimationTag = tAnimSpec.dwAnimationTag; tAnimMessage.nPermutationIndex = tAnimSpec.nPermutationIndex;

            tAnimMessage.flAnimSpeedScale = tAnimSpec.flAnimSpeedScale;
        }
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);

    MessageSendToClient(listDestPlayers, SMSG_PLAY_ANIMATION, &tMessage, sizeof(tMessage));
}






void AnimPlayerPlayAnimationFromMessage(
    ActorCommonData & tACD,
    const PlayAnimationMessage & tAnimMsg)
{
    RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor == NULL)
    {
        return;
    }


    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;

    BOOL bMoving = FALSE;
    if (ptRActor->ptCActorMovement)
    {
        bMoving = ptRActor->ptCActorMovement->IsMoving();
    }


    if (tAnimMsg.eAnimReason != ANIM_REASON_INTERRUPTABLE_ANIMATION && tAnimMsg.eAnimReason != ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH) { if (bMoving && tAnimMsg.eAnimReason == ANIM_REASON_FLOAT_CONVERSATION) return; }
    else if (bMoving)
        return;





    if (ptRActor->ptCActorMovement && !(ptRActor->ptCActorMovement->m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE)))
    {

        ptRActor->ptCActorMovement->StopNow();
    }

    tAnimPlayer.bUseAnimationTranslation = FALSE;


    int32 nCount = tAnimMsg.nUsedAnimArrayElements;


    for (int32 i = 0; i < nCount; i++)
    {
        AnimSpec & tPlayerSpec = tAnimPlayer.tAnimSpecArray[i];
        const PlayAnimationMessageSpec & tAnimMessage = tAnimMsg.tAnim[i];



        tPlayerSpec.dwDuration = tAnimMessage.dwDuration;
        tPlayerSpec.snoAnim = tAnimMessage.snoAnim;
        tPlayerSpec.dwAnimationTag = tAnimMessage.dwAnimationTag;
        tPlayerSpec.nPermutationIndex = tAnimMessage.nPermutationIndex;
        tPlayerSpec.flAnimSpeedScale = tAnimMessage.flAnimSpeedScale;
        tPlayerSpec.wvVelocity = cgwvNull;
    }

    tAnimPlayer.nNumQueuedAnimations = nCount;









    sAnimPlayerStartCurrentAnim(tACD, tAnimMsg.flUnitAnimStartTime);

    tAnimPlayer.eAnimReason = (AnimReason)tAnimMsg.eAnimReason;
    tAnimPlayer.dwAnimationTag = TAG_ACTOR_ANIMATION_INVALID;
}





AnimReason AnimPlayerGetAnimReason(const ActorCommonData & tACD) {
    if (tACD.ptAnimPlayer) {
        const AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
        return tAnimPlayer.eAnimReason;
    }
    return ANIM_REASON_NOT_ANIMATING;
}








BOOL AnimPlayerShouldPlayAllMessages(ActorCommonData & tACD) {
    BOOL bPlayAllMessages = FALSE;
    SNO snoAnim;
    int nPermutation;
    AnimPlayerGetCurrentAnim(tACD, snoAnim, nPermutation);
    if (snoAnim != SNO_NONE) {
        ScopedAnimationDefinitionLockR ptAnimDef(snoAnim);
        bPlayAllMessages = (ptAnimDef->dwFlags & (1 << ANIMATION_PLAY_ALL_MESSAGES_BIT)) != 0;
    }
    return bPlayAllMessages;
}










BOOL AnimPlayerShouldTurnToFace(const ActorCommonData & tACD) {
    SNO snoAnim;
    BOOL bDontTurn = FALSE;
    int nPermutation;
    if (tACD.snoActor != SNO_NONE) {
        ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
        if (pdefActor->dwFlags & (1 << ACTORDEF_NEVER_TURNS_BIT))
            return FALSE;
    }
    AnimPlayerGetCurrentAnim(tACD, snoAnim, nPermutation);
    if (snoAnim != SNO_NONE) {
        ScopedAnimationDefinitionLockR ptAnimDef(snoAnim);
        bDontTurn = (ptAnimDef->dwFlags & (1 << ANIMATION_DOESNT_TURN_TO_FACE_BIT)) != 0;
    }
    return !bDontTurn;
}













void AnimPlayerWeaponSetChanged(ActorCommonData & tACD)
{
    if (tACD.ptAnimPlayer == NULL)
    {
        return;
    }


    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;

    if (tAnimPlayer.dwAnimationTag == TAG_ACTOR_ANIMATION_INVALID ||
        tAnimPlayer.nNumQueuedAnimations != 1)
    {
        return;
    }



    if (tAnimPlayer.eAnimReason != ANIM_REASON_IDLE && tAnimPlayer.eAnimReason != ANIM_REASON_WALKING)
    {
        return;
    }



    float flSpeedScalar = 1.0f;
    if (tAnimPlayer.eAnimReason == ANIM_REASON_WALKING)
    {
        flSpeedScalar = PowerGetAnimationSpeedForPower(tACD.id, GlobalSNOGet(GLOBALSNO_POWERS_WALK), -1);
    }

    AnimSpec & tCurrentAnim = tAnimPlayer.tAnimSpecArray[0];

    uint32 dwAnimationTag = tAnimPlayer.dwAnimationTag;

    AnimationTiming tAnimationTiming;
    AnimPlayerGetAnimationTiming(tACD, dwAnimationTag, tCurrentAnim.dwDuration, flSpeedScalar, tAnimationTiming);

    AnimSpec & tNewAnim = tAnimationTiming.tAnimSpec[0];
    if (tCurrentAnim.snoAnim == tNewAnim.snoAnim)
    {
        return;
    }


    AnimPlayerPlayAnimation(tACD, dwAnimationTag, ANIM_PLAY_FOREVER, tAnimPlayer.bUseAnimationTranslation, tAnimPlayer.eAnimReason, 0.0f, FALSE, 1.0f);
}





uint32 AnimPlayerGetCurrentAnimTag(ActorCommonData & tACD) {
    if (tACD.ptAnimPlayer) {
        AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
        return tAnimPlayer.dwAnimationTag;
    }
    return TAG_ACTOR_ANIMATION_NEUTRAL;
}









void AnimPlayerSendIdle(ActorCommonData & tACD)
{
    ActorMovement * ptMove = tACD.GetActorMovement();
    if (ptMove && !(ptMove->m_uMoveFlags & (1 << MOVEBIT_IS_PROJECTILE)) && ptMove->IsMoving())
        return;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetThatKnowACD(listDestPlayers, tACD.id, FALSE);

    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);
    MessageSendToClient(listDestPlayers, 122, &tMessage, sizeof(tMessage));  // SMSG_PLAY_IDLE_ANIMATION
}













BOOL AnimPlayerAnimExists(ActorCommonData & tACD, const uint32 dwAnimTag) {
    return tACD.GetSpecificAnim(dwAnimTag) != SNO_NONE;
}







uint32 AnimPlayerGetWalkCoolDownTag(ActorCommonData & tACD, const uint32 dwAnimTag) {
    if (dwAnimTag == TAG_ACTOR_ANIMATION_INVALID)
        return TAG_ACTOR_ANIMATION_INVALID;
    switch (dwAnimTag) {
    case TAG_ACTOR_ANIMATION_RUN:
        if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_RUN_COOL_DOWN))
            return TAG_ACTOR_ANIMATION_RUN_COOL_DOWN;
        break;
    case TAG_ACTOR_ANIMATION_TOWNRUN:
        if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_TOWN_RUN_COOL_DOWN))
            return TAG_ACTOR_ANIMATION_TOWN_RUN_COOL_DOWN;
        break;
    case TAG_ACTOR_ANIMATION_WALK:
        if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_WALK_COOL_DOWN))
            return TAG_ACTOR_ANIMATION_WALK_COOL_DOWN;
        break;
    case TAG_ACTOR_ANIMATION_TOWNWALK:
        if (AnimPlayerAnimExists(tACD, TAG_ACTOR_ANIMATION_TOWN_WALK_COOL_DOWN))
            return TAG_ACTOR_ANIMATION_TOWN_WALK_COOL_DOWN;
        break;
    default: {
        uint32 dwStartTag;
        uint32 dwEndTag;
        AnimTagGetStartAndEndTags(dwAnimTag, dwStartTag, dwEndTag);
        if (AnimPlayerAnimExists(tACD, dwEndTag))
            return dwEndTag;
        break;
    }
    }
    return TAG_ACTOR_ANIMATION_INVALID;
}

























void AnimPlayerPlayNextAnimation(ActorCommonData & tACD)
{
    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    if (tAnimPlayer.nNumQueuedAnimations == 0)
    {
        return;
    }

    sAnimPlayerAnimationCompleted(tACD);
    sAnimPlayerSendAnimationsToClient(tACD, FALSE, ANIM_REASON_IDLE);
}





void AnimPlayerGetCurrentAnim(const ActorCommonData & tACD, SNO & snoAnim, int32 & nAnimPermutation) {
    snoAnim = SNO_NONE;
    nAnimPermutation = -1;
    if (tACD.ptAnimPlayer) {
        AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
        if (tAnimPlayer.nNumQueuedAnimations > 0) {
            snoAnim = tAnimPlayer.tAnimSpecArray[0].snoAnim;
            nAnimPermutation = tAnimPlayer.tAnimSpecArray[0].nPermutationIndex;
        }
    }
}



























uint32 AnimPlayerGetAnimEndTime(ActorCommonData & tACD) {
    if (tACD.ptAnimPlayer) {
        AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
        return tAnimPlayer.dwAnimationEndTime;
    }
    return 0xffffffff;
}









void AnimPlayerUpdateAnimEndTime(ActorCommonData & tACD, uint32 dwNewEndTime) {
    if (tACD.ptAnimPlayer) {
        AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
        tAnimPlayer.dwAnimationEndTime = dwNewEndTime;
    }
}







float AnimPlayerGetUnitTime(ActorCommonData & tACD) {
    float flUnitAnimStartTime = 0.0f;
    AnimPlayer & tAnimPlayer = *tACD.ptAnimPlayer;
    uint32 uDuration = tAnimPlayer.tAnimSpecArray[0].dwDuration;
    if (tAnimPlayer.dwAnimationEndTime != ANIM_PLAY_FOREVER && uDuration != 0) {
        float flTimeLeft = (float)(tAnimPlayer.dwAnimationEndTime - sAnimPlayerGetTime());
        flUnitAnimStartTime = 1.0f - flTimeLeft / (float)uDuration;
        flUnitAnimStartTime = Math::Pin(flUnitAnimStartTime, 0.0f, 1.0f);
    }
    return flUnitAnimStartTime;
}











void AnimPlayerPlaySecondaryAnim(
    ActorCommonData & tACD,
    uint32 uAnimTag,
    PreplayedAnimData & tPreplayedAnimData,
    SNO snoPower)
{
    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
    SNO snoCurrentPower = ActorPowersGetPrimaryPower(idActor);
    if (uAnimTag != TAG_ACTOR_ANIMATION_INVALID && snoCurrentPower == SNO_NONE)
    {


        AnimationTiming tAnimationTiming;
        RandSetSeed_Internal(SGameGlobalsGet().rngSyncedAnim, tPreplayedAnimData.dwSyncedSeed);
        RNG * prng = &SGameGlobalsGet().rngSyncedAnim;
        float flSpeedScalar = PowerGetAnimationSpeedForPower(tACD.id, snoPower, -1);
        AnimPlayerGetAnimationTiming(tACD, uAnimTag, ANIM_USE_ANIM_LENGTH, flSpeedScalar, *prng, tAnimationTiming);
        AnimPlayerPlayAnimationsWithTiming(tACD, &tAnimationTiming, 1, FALSE, ANIM_REASON_INTERRUPTABLE_ANIMATION, TRUE, FALSE);
    }
}












WorldDistance AnimSNOGetWalkingSpeed(SNO snoAnim, float flSpeedFactor)
{
    ScopedAnimationDefinitionLockR pdefAnim(snoAnim);
    return AnimGetWalkingSpeed(pdefAnim, flSpeedFactor);
}






WorldDistance AnimGetWalkingSpeed(const AnimationDefinition * pdefAnim, float flSpeedFactor)
{
    Vector3D wvVel;
    AnimationGetAvgVelocity(pdefAnim, 0, flSpeedFactor, TRUE, wvVel);

    WorldDistance wdSpeed = wvVel.x;

    if (wdSpeed < EPSILON)
    {
        wdSpeed = flSpeedFactor * 0.3f;
    }
    return wdSpeed;
}
