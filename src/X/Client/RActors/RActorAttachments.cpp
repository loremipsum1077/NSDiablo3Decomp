// X/Client/RActors/RActorAttachments.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/p2-ractor/att/frag.cpp by build/try/p2-ractor/gen.py.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "RActorAttachments.h"
#include "RActors.h"
#include "RActorAnimation.h"
#include "../Attachments.h"
#include "../AttachmentParent.h"
#include "../CGame.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Definitions/AttachmentDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/Player.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/MonsterTags.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/GlobalSNO.h"
#include "../Sound/Sound.h"
#include "../RMessage.h"
#include "../Physics/Ragdoll.h"
#include "../Physics/PartialRagdoll.h"

enum {
    RESIDUAL_FOOTSTEP_TIME = 120,
};

// Defined in X/Client/Attachments.cpp (no shared declaration yet).
Attachment * AttachmentTryToGet(const AttachmentID idAttachment);  // Attachments.cpp:106
void AttachmentDetach(Attachment & tAttachment);  // Attachments.cpp:979
void AttachmentDelete(Attachment & tAttachment);  // Attachments.cpp:1058

BOOL PlayerIsLocal(const ACDID idACD);  // X/Client/CPlayer.cpp:5216
void RActorSetParent(const RActorID idRActor, const ParentData & tParentData, const BOOL fIgnoreOwnerAlpha, const BOOL fIgnoreOwnerScale);  // RActors.cpp:1428

// Defined in X/Client/RActors/RActorAnimation.cpp.
BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor);

AttachmentID RActorAddAttachment(RActor & tRActor, const AttachmentType eAttachmentType, const SNO sno, const uint32 dwAttachmentID, HardpointHash szHardpointName, BOOL bIgnoreOwnerScale);
BOOL RActorIsRemote(const RActor & tRActor);
void RActorClearActorAttachment(RActor & tRActor, const uint32 dwAttachmentID);
Attachment & AttachmentGet(const AttachmentID idAttachment);  // Attachments.cpp
void AttachmentOutro(Attachment & tAttachment);  // Attachments.cpp
BOOL MsgTriggeredEventShouldTrigger(const MsgTriggeredEvent & tMsgTriggerEvent, const KeyedMessage & tMsg);  // Attachments.cpp
RagdollType RActorGetRagdollType(const RActor * ptRActor);  // RActors.cpp
static void sHandleSnapWeaponAttachment(RActor * ptRActor, HardpointHash szWeaponHardpoint);
static void sHandleMsgKey(RActor * ptRActor, const KeyedMessage & tMsg);
static void sRActorPlayKeyframedRMessages(RActor * ptRActor, AnimChannel & tChannel, float flTCurrent, BOOL (*pfnEventTypeFilter)(TriggeredEventType));
void sRActorPlayLayeredKeyframedRMessages(RActor * ptRActor, BOOL bPlayRemaining);
static AttachmentID sHandleMsg(RActorID * idRActors, RMessage & msg, BOOL bUsesDynamicHardpoints);
#include "../../Common/Definitions/AnimationDefinition.h"
#include "../../Common/Definitions/AnimTreeDefinition.h"
#include "../CPlayer.h"
// X/Client/EffectGroup.cpp:52, original external hardpoint hash.
extern const HardpointHash HARDPOINT_CLOSEST_DAMAGE;
// X/Client/RActors/RActorAnimation.cpp:2865.
int32 RActorAnimTreeLayerGetHardpointLinkIndex(const RActor & tRActor, AnimLayer eAnimLayer, const AnimTreeDefinition * pdefAnimTree, const AppearanceDefinition * pdefAppearance);
// X/Client/RActors/RActorAttachments.cpp:2177.
int32 RActorGetClosestDamageHardpointIndex(const RActorID idRActor, WorldPlace wpPosition, BOOL bGetFarthest, const AppearanceDefinition * pdefAppearance);

#include "../Definitions/CAttachmentDefinition.h"
// X/Client/Definitions/CAttachmentDefinition.cpp:30.
int32 HardpointIndexFromName(const AppearanceDefinition * pdefAppearance, HardpointHash szName);



#include "../Trails.h"
#include "../../Common/SNO/SymbolicValue.h"
#include "../Rope.h"
#include "../Particle.h"
#include "../Definitions/SurfaceDefinition.h"
#include "../Prefetch.h"
#include "../../Common/SceneCommon.h"
#include "../Definitions/WeatherDefinition.h"
#include "../../Common/VisualInventory.h"
#include "../EffectGroup.h"
BOOL ShouldDisableForLowFXMode(const RMessage & msg);  // X/Client/Attachments.cpp:123
void AttachmentSpawn(const RMessage & msg, const ParentData & tParentData, const PRTransform & transform, const float flScale, const BOOL fOverrideScale);  // X/Client/Attachments.cpp:485
TrailID TrailAdd(const SNO snoTrail, const RActorID idRActor, const HardpointLink * ptHardpointLinks, const BOOL fIgnoreOwnerAlpha, const RenderContext eContext);  // X/Client/Trails.cpp:437
AttachmentID AttachmentAdd(const RMessage & msg, const DataID idAttachedObject);  // X/Client/Attachments.cpp:812
BOOL CSceneGetSurface(SSceneID idSScene, const Vector3D & wp, SNO & snoSurface);  // X/Client/Scene.cpp:4696
BOOL CScenesGetSurface(SWorldID idSWorld, const Vector3D & wp, SNO & snoSurface);  // X/Client/Scene.cpp:4608
BOOL RActorIsVisibleToLocalPlayer(const RActor * ptRActor);  // X/Client/RActors/RActors.cpp:12229
void CinematographyHardpointView(const RActorID idTargetRActor, const Hardpoint & tHardpoint);  // X/Client/Camera/Cinematography.cpp:424
BOOL RActorPlayAnimation(RActor * ptRActor, const uint32 dwTag, BOOL bDisableBlending, BOOL fOnlyPlayIfDifferent);  // X/Client/RActors/RActorAnimation.cpp:1342
void RActorChangeLook(const RActorID idRActor, int32 nHashKey);  // X/Client/RActors/RActors.cpp:3191
void RActorCancelLook(const RActorID idRActor, int32 nHashKey);  // X/Client/RActors/RActors.cpp:3236
void RActorAnimTreeSetOutroAnim(const RActor & tRActor, OutroAnimationType eOutroAnim);  // X/Client/RActors/RActorAnimation.cpp:2670
void VisualInventoryApply(RActor * ptRActor, VisualInventory & tVisualInventory, const VisualInventoryType eVisualInventoryType, const PlayerClass ePlayerClass, const BOOL fFemale);  // X/Client/CVisualInventory.cpp:784
void RActorAttachmentResolveHardpointLinks(RActorID * idRActors, RMessage & msg);  // X/Client/RActors/RActorAttachments.cpp:1083
void RActorPhysicsSetAnimationOverride(RActor * ptRActor, BOOL bEnable);  // X/Client/RActors/RActorPhysics.cpp:505
namespace SoundManager { void PlayMusic(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle); }  // X/Client/Sound/Sound.cpp:2712
void RActorPhysicsEnableRagdoll(RActor * ptRActor);  // X/Client/RActors/RActorPhysics.cpp:180
void RActorPlayHitFlash(RActor * ptRActor, const RGBAColor & rgbaHitFlashAdd, float flDurationAdd, const RGBAColor & rgbaHitFlashMul, float flDurationMul, const BOOL fJitter);  // X/Client/RActors/RActors.cpp:9567
extern const SymbolicEntry gLeftPhysicsHardpointNames[6];  // X/Common/SNO/SymbolicValue.cpp:261
extern const SymbolicEntry gPhysicsHardpointNames[6];  // X/Common/SNO/SymbolicValue.cpp:250
extern const SymbolicEntry gRightPhysicsHardpointNames[6];  // X/Common/SNO/SymbolicValue.cpp:272
static AttachmentID sAttachmentCreate(const RActorID * idRActors, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1222, defined at 1589
void RActorGetHardpointTransform(const RActor & tRActor, const Hardpoint & tHardpoint, PRTransform & transform);  // RActorAttachments.cpp forward: called at 1559, defined at 2167
static void sFootstepCreate(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1260, defined at 1674
static void sPlayTriggeredAnim(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1280, defined at 1936
static void sModifyTransparency(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1284, defined at 1950
static void sChangeLook(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1296, defined at 2013
static void sAttachmentSpawn(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1226, defined at 1567
static void sAttachCamera(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1264, defined at 1827
static AttachmentID sTrailCreate(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1252, defined at 1633
static AttachmentID sRopeCreate(const RActorID * idRActors, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1230, defined at 1424
static void sModifyShadow(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1288, defined at 1969
static void sFrostBreathCreate(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1276, defined at 1903
static void sAttachSpawnedActor(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1272, defined at 1847
static void sCancelLook(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1300, defined at 2031
static void sSetOutroAnim(RActor & tRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1378, defined at 2089
static void sHideHelmAndShoulders(RActor * ptRActorPrimary);  // RActorAttachments.cpp forward: called at 1383, defined at 2100
static void sModifyCollisionMessages(const RActorID idRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1292, defined at 1996
static void sSetFade(RActor & tRActor, const RMessage & msg);  // RActorAttachments.cpp forward: called at 1373, defined at 2049
#include "../../Common/TagMaps/ActorTags.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
enum { TAG_SURFACE_FOOTSTEP_PARTICLE_SYSTEM = 327684, TAG_SURFACE_RESIDUAL_FOOTSTEP_PARTICLE_SYSTEM = 327685 }; // DWARF SurfaceTags.h:27
// X/Client/RActors/RActorAnimation.cpp:1994.
uint32 RActorGetAnimTreeLayerHash(AnimLayer eAnimLayer);





















































void RActorHandleOnCreation(RActor & tRActor, const BOOL fFlippy)
{

    if (fFlippy)
        sHandleMsgKey(&tRActor, KeyedMessage(1110, 0.0f, Shape::DEFAULT_MATERIAL));
    else
        sHandleMsgKey(&tRActor, KeyedMessage(1000, 0.0f, Shape::DEFAULT_MATERIAL));





}






void RActorHandleOnHighlight(RActor & tRActor, const BOOL bEnableHighlight)
{

    GizmoType eGizmoType = tRActor.idACD;
    if (tRActor.idACD != ACDID_INVALID)
        eGizmoType = ACDGet(tRActor.idACD).GetGizmoType();



    sHandleMsgKey(&tRActor, KeyedMessage(1002, 0.0f, Shape::DEFAULT_MATERIAL));

    if (bEnableHighlight)
    {
        switch (eGizmoType)
        {
        case ACTORGIZMO_PORTAL:
        case ACTORGIZMO_TOWN_PORTAL:
        case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
        case ACTORGIZMO_SECRET_PORTAL:
        case ACTORGIZMO_BOSS_PORTAL:
        case ACTORGIZMO_SAVEPOINT_PORTAL:
        case ACTORGIZMO_DUNGEON_PORTAL:
        case ACTORGIZMO_RETURN_PORTAL:
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PORTAL_HIGHLIGHT), cgtPSODefaults);


            RActorAddAttachment(tRActor, ATTACHMENTTYPE_SOUND, GlobalSNOGet(GLOBALSNO_SOUND_PORTAL_HIGHLIGHT_LOOP), 0x29c, DEFAULT_HARDPOINT_NAME, FALSE);
            break;
        }
        sHandleMsgKey(&tRActor, KeyedMessage(1001, 0.0f, Shape::DEFAULT_MATERIAL));
    }
    else
    {
        switch (eGizmoType)
        {
        case ACTORGIZMO_PORTAL: case ACTORGIZMO_TOWN_PORTAL: case ACTORGIZMO_PAGE_OF_FATE_PORTAL: case ACTORGIZMO_SECRET_PORTAL:
        case ACTORGIZMO_BOSS_PORTAL: case ACTORGIZMO_SAVEPOINT_PORTAL: case ACTORGIZMO_DUNGEON_PORTAL: case ACTORGIZMO_RETURN_PORTAL:
            RActorClearActorAttachment(tRActor, 0x29c);
            break;
        }
        sHandleMsgKey(&tRActor, KeyedMessage(1004, 0.0f, Shape::DEFAULT_MATERIAL));
    }





}




static BOOL sRActorLayerFootstepEventFilter(TriggeredEventType eEventType)
{
    return eEventType == TRIGGEREDEVENTTYPE_FOOTSTEP;
}











static void sRActorPlayKeyframedRMessages(
    RActor * ptRActor,
    AnimChannel & tChannel,
    float flTCurrent,
    BOOL (*pfnEventTypeFilter)(TriggeredEventType))
{
    if (tChannel.IsStopped() || tChannel.tAnimPermID.snoAnim == SNO_NONE)
        return;



    // Inference: the void-pointer LockR result is the DWARF AnimationDefinition pointer.
    const AnimationDefinition * pdefAnim = (const AnimationDefinition *)gptGroupDefAnimation->LockR(tChannel.tAnimPermID.snoAnim, FALSE);

    float flFrameLast = tChannel.flTLast * tChannel.flFrameRate;
    float flCurrentFrame = tChannel.flFrameRate * flTCurrent;


    const AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[tChannel.tAnimPermID.nPermutationIndex];

    for (int32 i = 0; i < tAnimPerm.nKeyedAttachmentCount; i++)
    {
        const KeyframedAttachment & tKeyedAttachment = tAnimPerm.ptKeyedAttachments[i]; // Inference: fTrigger defaults false until a frame interval and filter select it.
        BOOL fTrigger = FALSE;
        if (flFrameLast <= flCurrentFrame)
        {


            if (tKeyedAttachment.flKeyframeIndex > flFrameLast && tKeyedAttachment.flKeyframeIndex <= flCurrentFrame)
            {

                if (!pfnEventTypeFilter || !pfnEventTypeFilter(tKeyedAttachment.tTriggeredEvent.eType))
                    fTrigger = TRUE;
            }
        }
        else
        {



            if (tKeyedAttachment.flKeyframeIndex > flFrameLast || tKeyedAttachment.flKeyframeIndex <= flCurrentFrame)
            {

                if (!pfnEventTypeFilter || !pfnEventTypeFilter(tKeyedAttachment.tTriggeredEvent.eType))
                    fTrigger = TRUE;
            }
        }
        // Inference: the merged line-0 branch executes only a selected, unfiltered event.
        if (fTrigger)
        {

            // Inference: the shared zero constant initializes the optional start offset.
            float flStartOffset = 0.0f;

            if (tChannel.flTLast == -1.0f && flCurrentFrame > tKeyedAttachment.flKeyframeIndex)
            {
                // Inference: original 60.0f converts animation frames to seconds.
                float flFrames = flCurrentFrame - tKeyedAttachment.flKeyframeIndex;
                flStartOffset = flFrames / (tAnimPerm.flFrameRate * 60.0f);
            }

            RActorSendRMessage(*ptRActor, tKeyedAttachment.tTriggeredEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, flStartOffset, PARENT_RACTOR);
        }
    }
    // Inference: UnlockR takes void *& and clears the typed definition pointer.
    gptGroupDefAnimation->UnlockR((void *&)pdefAnim);
}




void sRActorPlayLayeredKeyframedRMessages(
    RActor * ptRActor,
    BOOL bPlayRemaining)
{
    static const uint32 dwActionLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_ACTION);
    static const uint32 dwActionAltLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_ACTION_ALT);
    static const uint32 dwRunLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_RUN);
    static const uint32 dwWalkLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_WALK);
    static const uint32 dwDeathLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_DEATH);
    static const uint32 dwNPCLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_NPC);
    static const uint32 dwGetHitLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_GETHIT);
    static const uint32 dwSpecialMoveLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_SPECIAL_MOVE);
    // Inference: each layer starts unselected; the original initializes all nine indices to -1.
    int32 nActionLayer = -1;
    int32 nActionAltLayer = -1;
    int32 nDeathLayer = -1;
    int32 nRunLayer = -1;
    int32 nWalkLayer = -1;
    int32 nNPCLayer = -1;
    int32 nGetHitLayer = -1;
    int32 nSpecialMoveLayer = -1;
    int32 nOtherLayer = -1;
    AnimTree * ptAnimTree = ptRActor->ptActorAnim->ptAnimTree;

    if (!ptAnimTree)
        return;



    if (ptAnimTree->bPaused)
        return;




    ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);
    for (uint32 dwLeaf = 0; dwLeaf < ptAnimTree->dwNumChannels; dwLeaf++)
    {
        if (ptAnimTree->ptLeafState[dwLeaf].tUpdate.bActive)
        {
            uint32 dwLeafHash = pdefAnimTree->ptLeaf[dwLeaf].tLayerName.dwHash;

            if (nActionLayer == -1 && dwLeafHash == dwActionLayerHash)
            {
                nActionLayer = dwLeaf;
            }
            else if (nActionAltLayer == -1 && dwLeafHash == dwActionAltLayerHash)
            {
                nActionAltLayer = dwLeaf;
            }
            else if (nDeathLayer == -1 && dwLeafHash == dwDeathLayerHash)
            {
                nDeathLayer = dwLeaf;
            }
            else if (nRunLayer == -1 && dwLeafHash == dwRunLayerHash)
            {
                nRunLayer = dwLeaf;
            }
            else if (nWalkLayer == -1 && dwLeafHash == dwWalkLayerHash)
            {
                nWalkLayer = dwLeaf;
            }
            else if (nNPCLayer == -1 && dwLeafHash == dwNPCLayerHash)
            {
                nNPCLayer = dwLeaf;
            }
            else if (nGetHitLayer == -1 && dwLeafHash == dwGetHitLayerHash)
            {
                nGetHitLayer = dwLeaf;
            }
            else if (nSpecialMoveLayer == -1 && dwLeafHash == dwSpecialMoveLayerHash)
            {
                nSpecialMoveLayer = dwLeaf;
            }
            else
            {
                nOtherLayer = dwLeaf;
            }
        }
    }

    if (nActionLayer >= 0)
    {
        AnimChannel & tChannel = ptAnimTree->ptChannels[nActionLayer];
        if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
        {
            sRActorPlayKeyframedRMessages(ptRActor, tChannel, tChannel.flT, NULL); // Inference: successful named-layer playback suppresses the fallback.
            nOtherLayer = -1;
        }
    }
    else if (nActionAltLayer >= 0)
    {
        AnimChannel & tChannel = ptAnimTree->ptChannels[nActionAltLayer];
        if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
        {
            sRActorPlayKeyframedRMessages(ptRActor, tChannel, tChannel.flT, NULL); // Inference: same fallback suppression as the action layer.
            nOtherLayer = -1;
        }
    }

    if (nDeathLayer >= 0)
    {
        AnimChannel & tChannel = ptAnimTree->ptChannels[nDeathLayer];
        if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
        {
            sRActorPlayKeyframedRMessages(ptRActor, tChannel, !bPlayRemaining ? tChannel.flT : tChannel.flTMax, NULL); // Inference: playback suppresses the otherwise unclassified layer.
            nOtherLayer = -1;
        }
    }

    if (nSpecialMoveLayer >= 0)
    {
        AnimChannel & tChannel = ptAnimTree->ptChannels[nSpecialMoveLayer];
        if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
        {
            sRActorPlayKeyframedRMessages(ptRActor, tChannel, tChannel.flT, NULL); // Inference: playback suppresses the otherwise unclassified layer.
            nOtherLayer = -1;
        }
    }
    else
    {
        if (nRunLayer >= 0)
        {
            AnimChannel & tChannel = ptAnimTree->ptChannels[nRunLayer];
            if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
            {
                sRActorPlayKeyframedRMessages(ptRActor, tChannel, tChannel.flT, NULL); // Inference: playback suppresses the otherwise unclassified layer.
                nOtherLayer = -1;
            }
        }
        if (nWalkLayer >= 0)
        {
            AnimChannel & tChannel = ptAnimTree->ptChannels[nWalkLayer];
            if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
            {
                BOOL (*pfnEventTypeFilter)(TriggeredEventType) = nRunLayer >= 0 ? sRActorLayerFootstepEventFilter : NULL;
                sRActorPlayKeyframedRMessages(ptRActor, tChannel, tChannel.flT, pfnEventTypeFilter); // Inference: successful walk playback also suppresses the fallback.
                nOtherLayer = -1;
            }
        }
    }

    if (nNPCLayer >= 0)
    {
        AnimChannel & tChannel = ptAnimTree->ptChannels[nNPCLayer];
        if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
        {
            sRActorPlayKeyframedRMessages(ptRActor, tChannel, !bPlayRemaining ? tChannel.flT : tChannel.flTMax, NULL); // Inference: playback suppresses the otherwise unclassified layer.
            nOtherLayer = -1;
        }
    }

    if (nGetHitLayer >= 0)
    {
        AnimChannel & tChannel = ptAnimTree->ptChannels[nGetHitLayer];
        if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
        {
            sRActorPlayKeyframedRMessages(ptRActor, tChannel, tChannel.flT, NULL); // Inference: successful get-hit playback suppresses the fallback.
            nOtherLayer = -1;
        }
    }

    if (nOtherLayer >= 0)
    {
        AnimChannel & tChannel = ptAnimTree->ptChannels[nOtherLayer];
        if (!tChannel.IsStopped() && tChannel.tAnimPermID.snoAnim != SNO_NONE)
        {
            sRActorPlayKeyframedRMessages(ptRActor, tChannel, tChannel.flT, NULL);
        }
    }
}





void RActorPlayRemainingKeyframedRMessages(RActor * ptRActor)
{
    if (!ptRActor->ptActorAnim)
        return;





    if (RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {
        sRActorPlayLayeredKeyframedRMessages(ptRActor, TRUE);




    }
    else
    {
        sRActorPlayKeyframedRMessages(ptRActor, ptRActor->ptActorAnim->tAnimChannel, ptRActor->ptActorAnim->tAnimChannel.flTMax, NULL);
    }
}

void RActorHandleKeyframedRMessages(RActor * ptRActor)
{
    if (!ptRActor->ptActorAnim)
        return;





    if (RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {
        sRActorPlayLayeredKeyframedRMessages(ptRActor, FALSE);




    }
    else
    {
        sRActorPlayKeyframedRMessages(ptRActor, ptRActor->ptActorAnim->tAnimChannel, ptRActor->ptActorAnim->tAnimChannel.flT, NULL);
    }
}

void RActorHandleRMsg(const DataID idOwner, RMessage & msg)
{
    RActorID idRActors[3] = { (RActorID)idOwner, RACTORID_INVALID, RACTORID_INVALID };
    sHandleMsg(idRActors, msg, FALSE);
}







static void sHandleSnapWeaponAttachment(RActor * ptRActor, HardpointHash szWeaponHardpoint)
{
    if (!ptRActor)
        return;

    for (POS pos = ptRActor->tAttachmentList.GetHeadPosition(); pos; )
    {
        AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
        Attachment * ptAttachment = AttachmentTryToGet(idAttachment);
        if (!ptAttachment)
            continue;



        if (ptAttachment->idAttachedObject == -1)
            continue;



        if (ptAttachment->snoname.snoGroup != 1 /* SNO group Actor */)
            continue;






        if (!(ptAttachment->tHP[0].tHash == szWeaponHardpoint || ptAttachment->tHP[1].tHash == szWeaponHardpoint))
            continue;







        RActor * ptAttachedRActor = RActorTryToGet((RActorID)ptAttachment->idAttachedObject);
        if (!ptAttachedRActor) continue;

        if (ptAttachedRActor->ptActorAnim && ptAttachedRActor->ptActorAnim->ptRagdoll) ptAttachedRActor->ptActorAnim->ptRagdoll->SnapToBindPose();
    }






}






static void sHandleMsgKey(RActor * ptRActor, const KeyedMessage & tMsg)
{
    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

    switch (tMsg.dwMsgKey)
    {
    case MSGKEY_RESERVED_LEAVING_ANIMATION:
        {


            if (RActorGetRagdollType(ptRActor) == FULL_RAGDOLL_TYPE)
            {
                ::delete ptRActor->ptActorAnim->ptRagdoll;
                ptRActor->ptActorAnim->ptRagdoll = NULL;

                ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
                if (pdefAppearance->tStructure.dwFlags & (1 << STRUCTURE_HAS_PARTIAL_RAGDOLL_BIT))
                {
                    ptRActor->ptActorAnim->ptRagdoll = new PartialRagdoll(ptRActor->id);
                }
            }



            for (POS pos = ptRActor->tAttachmentList.GetHeadPosition(); pos; )
            {
                AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
                Attachment & tAttachment = AttachmentGet(idAttachment);

                if (tAttachment.dwTriggerFlags & (1 << TRIGGERFLAG_DELETE_ON_ANIMATION_END_BIT))
                {
                    AttachmentDelete(tAttachment);
                }
                else if (tAttachment.dwTriggerFlags & (1 << TRIGGERFLAG_OUTRO_ON_ANIMATION_END_BIT))
                {
                    AttachmentOutro(tAttachment);
                }
            }
        }
        break;

    case MSGKEY_RESERVED_RECREATE_DEAD:
        {





            for (POS pos = ptRActor->tAttachmentList.GetHeadPosition(); pos; )
            {
                AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
                Attachment & tAttachment = AttachmentGet(idAttachment);

                if (tAttachment.dwTriggerFlags & ((1 << TRIGGERFLAG_DELETE_ON_OWNER_DEATH_BIT) | (1 << TRIGGERFLAG_OUTRO_ON_OWNER_DEATH_BIT)))
                {
                    AttachmentDelete(tAttachment);
                }
            }


            ptRActor->tRMsgList.DeleteforFlags((1 << TRIGGERFLAG_DELETE_ON_OWNER_DEATH_BIT) | (1 << TRIGGERFLAG_OUTRO_ON_OWNER_DEATH_BIT));
        }
        break;

    case MSGKEY_RESERVED_DEAD:
        {


            for (POS pos = ptRActor->tAttachmentList.GetHeadPosition(); pos; )
            {
                AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
                Attachment & tAttachment = AttachmentGet(idAttachment);

                if (tAttachment.dwTriggerFlags & (1 << TRIGGERFLAG_DELETE_ON_OWNER_DEATH_BIT))
                {
                    AttachmentDelete(tAttachment);
                }
                else if (tAttachment.dwTriggerFlags & (1 << TRIGGERFLAG_OUTRO_ON_OWNER_DEATH_BIT))
                {
                    AttachmentOutro(tAttachment);
                }
            }
        }
        break;

    case MSGKEY_RESERVED_PLAYER_RESURRECT:
        {


            for (POS pos = ptRActor->tAttachmentList.GetHeadPosition(); pos; )
            {
                AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
                Attachment & tAttachment = AttachmentGet(idAttachment);

                if (tAttachment.dwTriggerFlags & (1 << TRIGGERFLAG_DELETE_ON_OWNER_RESURRECTION_BIT))
                {
                    AttachmentDelete(tAttachment);
                }
                else if (tAttachment.dwTriggerFlags & (1 << TRIGGERFLAG_OUTRO_ON_OWNER_RESURRECTION_BIT))
                {
                    AttachmentOutro(tAttachment);
                }
            }
        }
        break;

    case MSGKEY_RESERVED_HIGHLIGHT_DONE:
        {


            for (int32 i = 0; i < pdefActor->nMsgTriggeredEventCount; i++)
            {


                if (pdefActor->ptMsgTriggeredEvents[i].dwMsgKey == MSGKEY_RESERVED_HIGHLIGHT)
                {



                    AttachmentFindAndDelete(ptRActor->tAttachmentList, pdefActor->ptMsgTriggeredEvents[i].tTriggerEvent.dwID, TRUE);
                }
            }
        }
        break;

    case MSGKEY_RESERVED_SNAP_WEAPON_ATTACHMENT_RIGHT:
        sHandleSnapWeaponAttachment(ptRActor, RIGHT_WEAPON_HARDPOINT_NAME);
        break;

    case MSGKEY_RESERVED_SNAP_WEAPON_ATTACHMENT_LEFT:


        sHandleSnapWeaponAttachment(ptRActor, LEFT_WEAPON_HARDPOINT_NAME);
        break;
    }


    for (int32 i = 0; i < pdefActor->nMsgTriggeredEventCount; i++)
    {
        const MsgTriggeredEvent & tMsgTriggerEvent = pdefActor->ptMsgTriggeredEvents[i];

        if (MsgTriggeredEventShouldTrigger(tMsgTriggerEvent, tMsg))




            RActorSendRMessage(*ptRActor, tMsgTriggerEvent.tTriggerEvent, tMsg.flImpulseMagnitude, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
    }
}




void RActorAttachmentDetach(const RActorID idRActor, const AttachmentID idAttachment)
{

    Attachment * ptAttachment;
    RActor * ptRActor = RActorTryToGet(idRActor);
    ptAttachment = AttachmentTryToGet(idAttachment);
    if (!ptRActor || !ptAttachment)
        return;



    AttachmentDetach(*ptAttachment);
}



void RActorAttachmentDelete(RActorID idRActor, const AttachmentID idAttachment)
{

    Attachment * ptAttachment;
    RActor * ptRActor = RActorTryToGet(idRActor);
    ptAttachment = AttachmentTryToGet(idAttachment);
    if (!ptRActor || !ptAttachment)
        return;



    AttachmentDelete(*ptAttachment);
}







BOOL RActorIsAttachment(const RActor & tRActor)
{
    return tRActor.tParentData.m_eParentType != PARENT_NONE;
}












void RActorDeleteDynamicAttachments(RActor & tRActor)
{

    POS pos = tRActor.tAttachmentList.GetHeadPosition();
    while (pos)
    {
        AttachmentID idAttachment = tRActor.tAttachmentList.GetNext(pos);
        Attachment * ptAttachment = AttachmentTryToGet(idAttachment);
        if (ptAttachment && ptAttachment->tHP[0].tHash.IsDynamic())
        {
            AttachmentDelete(*ptAttachment);
        }
    }
}


















AttachmentID RActorSendRMessage(RActor & tRActor, const TriggerEvent & tTriggerEvent, const float flImpulse, const RActorID * idRActorSources, const Hardpoint * ptDynamicHardpoints, const int32 nNumDynamicHardpoints, const float flLifetime, const BOOL bForRepeat, const float flStartOffset, ParentType eType)
{
    RMessage msg;
    BOOL bRemote = RActorIsRemote(tRActor);
    if (msg.Initialize(tTriggerEvent, flImpulse, eType, tRActor.id, -1.0f, bForRepeat, bRemote, flStartOffset))
    {












        if (flLifetime != -1.0f)

            msg.flExpireTime = CGameGetClientTime() + flLifetime;



        for (int32 i = 0; i < nNumDynamicHardpoints; i++)
        {








            msg.tHardpoints[i] = ptDynamicHardpoints[i];
            msg.tHardpoints[i].tHash.SetDynamic(i);
        }

        if (msg.flDelay == 0.0f)
        {


            RActorID idRActors[3] = { tRActor.id, RACTORID_INVALID, RACTORID_INVALID }; const BOOL bUsesDynamicHardpoints = nNumDynamicHardpoints > 0;
            if (idRActorSources)
            {
                SigmaMemoryCopy(idRActors, idRActorSources, sizeof(idRActors));
            }
            return sHandleMsg(idRActors, msg, bUsesDynamicHardpoints);
        }
        else
        {
            tRActor.tRMsgList.QueueDelayedMessage(msg);
        }
    }

    return ATTACHMENTID_INVALID;
}






void RActorSendMsgKey(const RActorID idRActor, const KeyedMessage & tMsg, const MsgPassMethod eMsgPassMethod)
{

    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;


    switch (eMsgPassMethod)
    {
    case MSGPASS_TO_ACTORCHAIN:

        RActorSendMsgKey(RActorGetTopmostOwner(idRActor, FALSE, FALSE), tMsg, MSGPASS_TO_SELF_AND_CHILDREN);
        break;


    case MSGPASS_TO_THIS_ACTOR:

        sHandleMsgKey(ptRActor, tMsg);
        break;

    case MSGPASS_TO_SELF_AND_CHILDREN:
        sHandleMsgKey(ptRActor, tMsg);
        AttachmentListSendMsgToChildren(ptRActor->tAttachmentList, tMsg);
        break;
    case MSGPASS_TO_CHILDREN_ONLY:
        AttachmentListSendMsgToChildren(ptRActor->tAttachmentList, tMsg);
        break;
    }



}






static BOOL sHandleWeaponMessage(
    AttachmentID * pidAttachment,
    const RActorID * idRActors,
    RMessage & msg)
{
    for (int32 i = 0; i < MAX_EVENT_HARDPOINT_LINKS; i++)
    {
        const HardpointLink & tHardpointLink = msg.tTriggerEvent.tHardpointLinks[i];
        if (tHardpointLink.tHash == 0u)
            continue;



        RActorID idRActor = idRActors[tHardpointLink.eSource];
        if (idRActor == RACTORID_INVALID)
            continue;



        const RActor * ptRActor = RActorGet(idRActor);

        HardpointHash szWeaponHardpoint;
        HardpointHash szPhysicsHardpoint;
        szWeaponHardpoint = 0u;
        szPhysicsHardpoint = 0u;


        HardpointHash nHash = tHardpointLink.tHash;





        for (int32 j = 0; gLeftPhysicsHardpointNames[j].sName; j++)
        {
            if (nHash == gLeftPhysicsHardpointNames[j].nValue)
            {
                szWeaponHardpoint = LEFT_WEAPON_HARDPOINT_NAME;

                szPhysicsHardpoint = gPhysicsHardpointNames[j].nValue;
                break;
            }
        }





        if (szWeaponHardpoint == 0u)
        {
            for (int32 j = 0; gRightPhysicsHardpointNames[j].sName; j++)
            {
                if (nHash == gRightPhysicsHardpointNames[j].nValue)
                {
                    szWeaponHardpoint = RIGHT_WEAPON_HARDPOINT_NAME;

                    szPhysicsHardpoint = gPhysicsHardpointNames[j].nValue;
                    break;
                }
            }
        }




        if (szWeaponHardpoint == 0u)
            continue;




        POS pos = ptRActor->tAttachmentList.GetHeadPosition();
        while (pos)
        {
            AttachmentID idAttachment = ptRActor->tAttachmentList.GetNext(pos);
            Attachment * ptAttachment = AttachmentTryToGet(idAttachment);

            if (!ptAttachment)
                continue;



            if (ptAttachment->idAttachedObject == -1)
                continue;



            if (ptAttachment->snoname.snoGroup != ACTOR_SNO)
                continue;



            for (int32 j = 0; j < MAX_EVENT_HARDPOINT_LINKS; j++)
            {
                if (ptAttachment->tHP[j].tHash == szWeaponHardpoint)
                { // Inference: forwarding rebinds child link zero and clears the other links.
                    RMessage childMsg = msg;
                    childMsg.tTriggerEvent.tHardpointLinks[0].eSource = HARDPOINTSOURCE_PRIMARY;
                    childMsg.tTriggerEvent.tHardpointLinks[0].tHash = szPhysicsHardpoint;

                    for (int32 k = 1; k < MAX_EVENT_HARDPOINT_LINKS; k++)
                    {
                        childMsg.tTriggerEvent.tHardpointLinks[k].tHash = 0u;
                    }
                    // Inference: the Actor SNO group guard permits conversion from DataID to RActorID.
                    RActorID idChildRActors[HARDPOINTSOURCE_MAX_SOURCES] = { (RActorID)ptAttachment->idAttachedObject, RACTORID_INVALID, RACTORID_INVALID };
                    *pidAttachment = sHandleMsg(idChildRActors, childMsg, FALSE);
                    // Inference: successful forwarding returns immediately via the parent epilogue.
                    return TRUE;
                }
            }
        }
    }
    // Inference: exhaustion falls through to ordinary message handling.
    return FALSE;
}






void RActorAttachmentResolveHardpointLinks(
    RActorID * idRActors,
    RMessage & msg)
{




    RActorID idRActorPrimary = idRActors[0];



    for (int32 i = 0; i < MAX_EVENT_HARDPOINT_LINKS; i++)
    {
        const HardpointLink & tHardpointLink = msg.tTriggerEvent.tHardpointLinks[i];

        RActorID idRActor = idRActors[tHardpointLink.eSource];
        if (idRActor != RACTORID_INVALID)
        {
            const RActor * ptRActor = RActorGet(idRActor);
            ScopedAppearanceDefinitionLockR ptDefAppearance(ptRActor->snoAppearance);

            int32 nHardpointIndex = -1;

            if (HARDPOINT_CLOSEST_DAMAGE == tHardpointLink.tHash)
            {
                if (ptRActor->idACD != ACDID_INVALID)
                { // Inference: the integer attribute is the DWARF ACDNetworkName value consumed by ACDFromANN.
                    ACDNetworkName annAttacker = (ACDNetworkName)ACDGet(ptRActor->idACD).AttributesGetInt(ATTRIBUTE_LAST_DAMAGE_ACD);
                    ACDID idACDAttacker = ACDFromANN(annAttacker);
                    if (idACDAttacker != ACDID_INVALID)
                    {
                        WorldPlace wpAttacker = ACDGet(idACDAttacker).GetWorldPlaceCenter(); // Inference: convert the lock to its const pointer before the null-selection expression.
                        nHardpointIndex = RActorGetClosestDamageHardpointIndex(idRActorPrimary, wpAttacker, FALSE, idRActor == idRActorPrimary ? (const AppearanceDefinition *)ptDefAppearance : NULL);
                    }
                    else
                    {



                        ACDID idACDPlayer = LocalPlayerACDGet();
                        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer); // Inference: the null lookup branches around the world-place query.
                        if (ptACDPlayer)
                        {
                            WorldPlace wpPlayer = ptACDPlayer->GetWorldPlaceCenter(); // Inference: select the existing lock only for the primary actor.
                            nHardpointIndex = RActorGetClosestDamageHardpointIndex(idRActorPrimary, wpPlayer, TRUE, idRActor == idRActorPrimary ? (const AppearanceDefinition *)ptDefAppearance : NULL);
                        }
                    }
                }
            }


            if (ANIM_PERMUTATION_HARDPOINT_NAME == tHardpointLink.tHash)
            {
                if (ptRActor->ptActorAnim)
                {
                    if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
                    {
                        const AnimPermID & idPerm = ptRActor->ptActorAnim->tAnimChannel.tAnimPermID; // Inference: the void-pointer LockR result is the DWARF AnimationDefinition pointer.
                        const AnimationDefinition * pdefAnimation = (const AnimationDefinition *)gptGroupDefAnimation->LockR(idPerm.snoAnim, FALSE);


                        const AnimPermutation * ptPermutation = &pdefAnimation->ptPermutations[idPerm.nPermutationIndex];
                        nHardpointIndex = HardpointIndexFromName(ptDefAppearance, ptPermutation->tHardpointLink.tHash); // Inference: UnlockR takes void *& and clears this typed definition pointer.
                        gptGroupDefAnimation->UnlockR((void *&)pdefAnimation);
                    }
                    else
                    {
                        ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);
                        int32 nActionHardpointIndex = RActorAnimTreeLayerGetHardpointLinkIndex(*ptRActor, ANIM_LAYER_ACTION, pdefAnimTree, ptDefAppearance);
                        int32 nNPCHardpointIndex = RActorAnimTreeLayerGetHardpointLinkIndex(*ptRActor, ANIM_LAYER_NPC, pdefAnimTree, ptDefAppearance);
                        nHardpointIndex = nActionHardpointIndex != -1 ? nActionHardpointIndex : nNPCHardpointIndex;
                    }
                }
            }


            if (nHardpointIndex == -1)
            {
                nHardpointIndex = HardpointIndexFromName(ptDefAppearance, tHardpointLink.tHash);
            }
            if (nHardpointIndex != -1)
            {
                msg.tHardpoints[i] = ptDefAppearance->tStructure.ptHardpoints[nHardpointIndex];
            }
            else
            {
                msg.tHardpoints[i].transform = cgtPRTransformIdentity;
                msg.tHardpoints[i].nBoneIndex = -1;
                msg.tHardpoints[i].tHash = 0u;
            }
        }
    }
}




static AttachmentID sHandleMsg(
    RActorID * idRActors,
    RMessage & msg,
    BOOL bUsesDynamicHardpoints)
{




    const TriggerConditions & tConditions = msg.tTriggerEvent.tConditions;
    RActorID idRActorPrimary = idRActors[0];

    RActor * ptRActorPrimary = RActorGet(idRActorPrimary);

    AttachmentID idAttachment = ATTACHMENTID_INVALID;



    BOOL found = sHandleWeaponMessage(&idAttachment, idRActors, msg);
    // Inference: successful weapon forwarding exits through the shared epilogue.
    if (found)
        return idAttachment;


    if (tConditions.dwRepeatTimeMin != 0 || tConditions.dwRepeatTimeDelta != 0)
    {

        RActorSendRMessage(*ptRActorPrimary, msg.tTriggerEvent, msg.flImpulse, idRActors, NULL, 0, -1.0f, TRUE, 0.0f, PARENT_RACTOR);
    }



    if (!bUsesDynamicHardpoints)
    {
        RActorAttachmentResolveHardpointLinks(idRActors, msg);
    }


    switch (msg.tTriggerEvent.eType)
    {
    case TRIGGEREDEVENTTYPE_ADD_OBJECT:
        idAttachment = sAttachmentCreate(idRActors, msg);
        break;

    case TRIGGEREDEVENTTYPE_SPAWN_OBJECT:
        sAttachmentSpawn(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_ADD_ROPE:
        idAttachment = sRopeCreate(idRActors, msg);
        break;

    case TRIGGEREDEVENTTYPE_DETACH_OBJECT:
        AttachmentFindAndDetach(ptRActorPrimary->tAttachmentList, msg.tTriggerEvent.dwID);
        break;

    case TRIGGEREDEVENTTYPE_DELETE_OBJECT:
        AttachmentFindAndDelete(ptRActorPrimary->tAttachmentList, msg.tTriggerEvent.dwID, FALSE);
        // Inference: event deletion is the line-0 shared tail after either delete mode.
        ptRActorPrimary->tRMsgList.DeleteforEventID(msg.tTriggerEvent.dwID);
        break;


    case TRIGGEREDEVENTTYPE_OUTRO_OBJECT:
        AttachmentFindAndDelete(ptRActorPrimary->tAttachmentList, msg.tTriggerEvent.dwID, TRUE);
        // Inference: this shares the DeleteforEventID tail with DELETE_OBJECT.
        ptRActorPrimary->tRMsgList.DeleteforEventID(msg.tTriggerEvent.dwID);
        break;


    case TRIGGEREDEVENTTYPE_ADD_TRAIL:
        idAttachment = sTrailCreate(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_MSG:
        RActorSendMsgKey(idRActorPrimary, KeyedMessage(msg.tTriggerEvent.dwMsgKey, 0.0f, Shape::DEFAULT_MATERIAL), msg.tTriggerEvent.eMsgPassMethod);
        break;

    case TRIGGEREDEVENTTYPE_FOOTSTEP:
        sFootstepCreate(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_ATTACH_CAMERA:
        sAttachCamera(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_RESTORE_CAMERA:
        CGameGlobalsGet().SetCameraMode(CAMERAMODE_NORMAL);
        break;

    case TRIGGEREDEVENTTYPE_ATTACH_SPAWNED_ACTOR:
        sAttachSpawnedActor(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_FROST_BREATH:
        sFrostBreathCreate(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_ANIM:
        sPlayTriggeredAnim(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_TRANSPARENCY:
        sModifyTransparency(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_SHADOW:
        sModifyShadow(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_DISABLE_COLLISION_MESSAGES:
        sModifyCollisionMessages(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_CHANGE_LOOK:
        sChangeLook(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_CANCEL_LOOK:
        sCancelLook(idRActorPrimary, msg);
        break;

    case TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP:


        // Inference: RActorID storage is passed through the declared DataID array interface.
        idAttachment = EffectGroupAttachmentAdd(idRActorPrimary, (const DataID *)idRActors,


            msg.nParentType, msg.tTriggerEvent);
        break;




    case TRIGGEREDEVENTTYPE_ENABLE_RAGDOLL:
        RActorPhysicsEnableRagdoll(ptRActorPrimary);
        break;

    case TRIGGEREDEVENTTYPE_ENABLE_RAGDOLL_ANIMATION_OVERRIDE:
        RActorPhysicsSetAnimationOverride(ptRActorPrimary, TRUE);
        break;

    case TRIGGEREDEVENTTYPE_DISABLE_RAGDOLL_ANIMATION_OVERRIDE:
        RActorPhysicsSetAnimationOverride(ptRActorPrimary, FALSE);
        break;

    case TRIGGEREDEVENTTYPE_HITFLASH:
        RActorPlayHitFlash(ptRActorPrimary,
            msg.tTriggerEvent.rgbaColorAdd, msg.tTriggerEvent.nHitFlashAddDuration * (1.0f / 60.0f),
            msg.tTriggerEvent.rgbaColorMul, msg.tTriggerEvent.nHitFlashMulDuration * (1.0f / 60.0f), FALSE);
        break;


    case TRIGGEREDEVENTTYPE_DISABLE_CONSTRAINT:
        if (ptRActorPrimary->ptActorAnim && ptRActorPrimary->ptActorAnim->ptRagdoll)
        {

            ptRActorPrimary->ptActorAnim->ptRagdoll->DisableConstraint(msg.tTriggerEvent.tConstraintLink.dwHash);
        }
        break;




    case TRIGGEREDEVENTTYPE_MUSIC:
        SoundManager::PlayMusic(msg.tTriggerEvent.snoname.snoHandle, 0, FALSE);
        break;

    case TRIGGEREDEVENTTYPE_FRAMEOUT_ENABLE:
        {
            AppGlobals & tAppGlobals = sgtAppGlobals;
            tAppGlobals.fFrameOut = TRUE;


            Blizzard::Time::Timestamp utcTime = Blizzard::Time::GetTimestamp();
            Blizzard::Time::Timestamp localTime = Blizzard::Time::LocalBias(utcTime);
            Blizzard::Time::TimeRec timeRec;
            Blizzard::Time::BreakTime(localTime, timeRec);

            Blizzard::String::Format(tAppGlobals.szFrameOutPath, sizeof(tAppGlobals.szFrameOutPath),
                "FrameOut %02d_%02d_%02d %02d_%02d_%02d",
                timeRec.year, timeRec.month, timeRec.day, timeRec.hour,
                timeRec.minute, timeRec.second);
        }
        break;

    case TRIGGEREDEVENTTYPE_FRAMEOUT_DISABLE:
        sgtAppGlobals.fFrameOut = FALSE;
        break;

    case TRIGGEREDEVENTTYPE_FADE:
        sSetFade(*ptRActorPrimary, msg);
        break;


    case TRIGGEREDEVENTTYPE_OUTRO_ANIMATION:
        sSetOutroAnim(*ptRActorPrimary, msg);
        break;


    case TRIGGEREDEVENTTYPE_HIDE_HELM_AND_SHOULDERS:
        sHideHelmAndShoulders(ptRActorPrimary);
        break;

    case TRIGGEREDEVENTTYPE_NULL:
        break;


    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return idAttachment;
}




static BOOL sIsValidHardPoint(
    const SNO snoAppearance,
    HardpointHash szHardpointName)
{
    ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;
    // Inference: false until a hardpoint hash matches; its zero follows the shared cleanup.
    BOOL bValid = FALSE;

    for (int32 i = 0; i < tStructure.nHardpointCount; i++)
    {
        if (tStructure.ptHardpoints[i].tHash == szHardpointName)
        {
            bValid = TRUE;
            break;
        }
    }

    return bValid;
}




static AttachmentID sRopeCreate(
    const RActorID * idRActors,
    const RMessage & msg)
{
    if (ShouldDisableForLowFXMode(msg))
        return ATTACHMENTID_INVALID;



    SNO snoRope = msg.tTriggerEvent.snoname.snoHandle;














































    RopeAttachPoint tRopeAttachPoints[2];

    for (int32 i = 0; i < MAX_EVENT_HARDPOINT_LINKS; i++)
    {






        const HardpointLink & tHardpointLink = msg.tTriggerEvent.tHardpointLinks[i];
        const RActorID & idRActor = idRActors[tHardpointLink.eSource];
        RActor * ptRActor = RActorTryToGet(idRActor);
        // Inference: either missing endpoint actor returns the invalid attachment.
        if (!ptRActor)
            return ATTACHMENTID_INVALID;



        int32 nHardpointIndex = HardpointIndexFromName(ptRActor->snoAppearance, tHardpointLink.tHash);
        RopeAttachPoint & tAttachPoint = tRopeAttachPoints[i];

        if (nHardpointIndex != -1)
        {
            tAttachPoint.nRopeAttachType = ROPE_ATTACH_PARENT;
            tAttachPoint.tParentData.SetHardpointIndex(PARENT_RACTOR, idRActor, nHardpointIndex);
        }
        else if (i == 0)
        {
            tAttachPoint.nRopeAttachType = ROPE_ATTACH_PARENT_CENTER;
            tAttachPoint.tParentData.SetHardpointIndex(PARENT_RACTOR, idRActor, -1);
        }
        else
        {
            tAttachPoint.nRopeAttachType = ROPE_ATTACH_NONE;
            tAttachPoint.tParentData.Init();
        }
    }

    BOOL fIgnoreOwnerAlpha = msg.tTriggerEvent.fIgnoreOwnerAlpha;
    // Inference: render context defaults to zero if the topmost actor lookup fails.
    RenderContext eContext = RENDERCONTEXT_DEFAULT; // Inference: idParent is interpreted as RActorID by this owner-lookup API.
    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)msg.idParent, FALSE, FALSE);
    const RActor * ptOwnerRActor = RActorTryToGet(idRActorTopmost);
    if (ptOwnerRActor)
    {
        eContext = ptOwnerRActor->eContext;
    }

    DataID idAttachedObject = RopeAdd(snoRope, tRopeAttachPoints, fIgnoreOwnerAlpha, eContext);
    // Inference: this is the shared line-0 AttachmentAdd tail with sTrailCreate.
    return AttachmentAdd(msg, idAttachedObject);
}





static BOOL sGetAttachmentCreationParams(
    const RActorID idRActor,
    const RMessage & msg,
    PRTransform & transform)
{
    RActor * ptRActor = RActorTryToGet(idRActor);
    // Inference: the inlined lookup's null result shares the invalid-attachment exit.
    if (!ptRActor)
        return FALSE;





    if (msg.tHardpoints[0].tHash == 0u)
        return FALSE;





    RActorGetHardpointTransform(*ptRActor, msg.tHardpoints[0], transform);
    // Inference: only the complete transform path reaches the creation call.
    return TRUE;
}




static void sAttachmentSpawn(
    const RActorID idRActor,
    const RMessage & msg)
{
    PRTransform transform;

    BOOL fSuccess = sGetAttachmentCreationParams(idRActor, msg, transform);
    if (!fSuccess)
        return;



    ParentData tParentData;
    tParentData.Set(msg.nParentType, msg.idParent, msg.tHardpoints[0]);

    BOOL fOverrideScale = !msg.tTriggerEvent.fIgnoreOwnerScale;
    AttachmentSpawn(msg, tParentData, transform, msg.tTriggerEvent.flScale, fOverrideScale);
}




static AttachmentID sAttachmentCreate(
    const RActorID * idRActors,
    const RMessage & msg)
{
    PRTransform transform;

    BOOL fSuccess = sGetAttachmentCreationParams(idRActors[0], msg, transform);
    if (!fSuccess)
        return ATTACHMENTID_INVALID;



    ParentData tParentData;
    tParentData.Set(msg.nParentType, msg.idParent, msg.tHardpoints[0]);

    BOOL fOverrideScale = !msg.tTriggerEvent.fIgnoreOwnerScale;
    AttachmentID idAttachment = AttachmentCreate(msg, tParentData, transform, msg.tTriggerEvent.flScale, fOverrideScale);

    if (msg.tTriggerEvent.eAttachmentType == ATTACHMENTTYPE_PARTICLESYSTEM)
    {



        HardpointSource eAttractorSource = msg.tTriggerEvent.tHardpointLinks[1].eSource;
        if (eAttractorSource != HARDPOINTSOURCE_PRIMARY && idRActors[eAttractorSource] != RACTORID_INVALID)
        {
            Attachment * ptAttachment = AttachmentTryToGet(idAttachment);
            if (ptAttachment)
            {
                ParticleSystem * ptParticleSystem = ParticleSystemGet(ptAttachment->idAttachedObject);

                ptParticleSystem->SetAttractor(cgwpNull, idRActors[eAttractorSource], &msg.tTriggerEvent.tHardpointLinks[1]);
            }
        }
    }
    // Inference: all optional-attractor paths return the newly created attachment.
    return idAttachment;
}






static AttachmentID sTrailCreate(
    const RActorID idRActor,
    const RMessage & msg)
{
    if (ShouldDisableForLowFXMode(msg))
        return ATTACHMENTID_INVALID;



    RActor * ptRActor = RActorGet(idRActor);

    for (int32 i = 0; i < MAX_EVENT_HARDPOINT_LINKS; i++)
    {
        const HardpointLink & tHardpointLink = msg.tTriggerEvent.tHardpointLinks[i];

        if (!sIsValidHardPoint(ptRActor->snoAppearance, tHardpointLink.tHash))
            return ATTACHMENTID_INVALID;
    }





    BOOL fIgnoreOwnerAlpha = msg.tTriggerEvent.fIgnoreOwnerAlpha;
    SNO snoTrail = msg.tTriggerEvent.snoname.snoHandle;
    // Inference: the no-owner path passes render context zero.
    RenderContext eContext = RENDERCONTEXT_DEFAULT;
    RActorID idRActorTopmost = RActorGetTopmostOwner(idRActor, FALSE, FALSE);
    const RActor * ptOwnerRActor = RActorTryToGet(idRActorTopmost);
    if (ptOwnerRActor)
    {
        eContext = ptOwnerRActor->eContext;
    }

    DataID idAttachedObject = TrailAdd(snoTrail, idRActor, msg.tTriggerEvent.tHardpointLinks, fIgnoreOwnerAlpha, eContext);
    // Inference: rope and trail creation share the line-0 AttachmentAdd tail.
    return AttachmentAdd(msg, idAttachedObject);
}



static void sFootstepCreate(
    const RActorID idRActor,
    const RMessage & msg)
{
    const RActor * ptRActor = RActorGet(idRActor);

    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (ptACD && ptACD->IsObserver())
        return;




    if (msg.tHardpoints[0].tHash == 0u)
        return;






    if ((sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT)) &&
        PlayerIsLocal(idRActor))
        return;



    PRTransform transform;
    RActorGetHardpointTransform(*ptRActor, msg.tHardpoints[0], transform);



    if (!ptRActor->ptCActorMovement)
        return;












    const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
    SNO snoSurface = SNO_NONE;
    if (loc.idSScene != SSCENEID_INVALID)
    {
        if (!CSceneGetSurface(loc.idSScene, loc.transform.wp, snoSurface))
            return;
    }
    else
    {


        if (!CScenesGetSurface(loc.idSWorld, loc.transform.wp, snoSurface))
            return;
    }




    if (snoSurface == SNO_NONE)
        return;



    ScopedSurfaceDefinitionLockR pdefSurface(snoSurface);
    SurfaceType eSurfaceType = Math::Pin(pdefSurface->eType, SURFACETYPE_GRASS, SURFACETYPE_BLOOD);

    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
    // Inference: the default sound table is run; named cases select walk or slide.
    uint32 dwTagStart = TAG_ACTOR_FOOTSTEP_RUN_GRASS;
    switch (msg.tTriggerEvent.eFootstepType)
    {
    case FOOTSTEPTYPE_SLIDE:
        dwTagStart = TAG_ACTOR_FOOTSTEP_SLIDE_GRASS;
        break;
    case FOOTSTEPTYPE_WALK:
        dwTagStart = TAG_ACTOR_FOOTSTEP_WALK_GRASS;
        break;
    default:
        break;
    }




    SNO snoFootstep = TagMapGetSNO(pdefActor->hTagMap, dwTagStart + eSurfaceType);
    if (snoFootstep != SNO_NONE)
    { // Inference: bValidSound has no surviving location or use; retain its uninitialized declaration only.
        BOOL bValidSound;


        // Inference: priority 2 is the immediate passed by the original.
        if (Prefetch::AddToQueue(SNOName(SOUND_SNO, snoFootstep), 2))
        {
            PlaySoundOptions tOptions;
            tOptions.eContext = ptRActor->eContext;
            tOptions.pwp = &transform.wp;
            tOptions.fOccluded = (ptRActor->dwVisibilityFlags & (1 << RACTOR_NOT_IN_LOS_BIT)) != 0;

            SoundManager::PlaySound(snoFootstep, tOptions);
        }
    }

    BOOL fGenerateFootstepParticles = TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_FOOTSTEP_GENERATE_PARTICLES);




    if (!RActorIsVisibleToLocalPlayer(ptRActor))
        return;
    // Inference: visibility is queried even when the particle-generation flag is false.
    if (fGenerateFootstepParticles)
    {


        SNO snoFootstepParticleSystem = TagMapGetSNO(pdefSurface->hTagMap, TAG_SURFACE_FOOTSTEP_PARTICLE_SYSTEM);
        if (snoFootstepParticleSystem != SNO_NONE && Prefetch::AddToQueue(SNOName(PARTICLE_SNO, snoFootstepParticleSystem), 2))
        {
            ParticleSystemAddParams tParams(snoFootstepParticleSystem, transform);
            ParticleSystemAdd(tParams);
        }

        if (PlayerIsLocal(idRActor))
        {
            SNO snoResidualFootstepParticleSystem = TagMapGetSNO(pdefSurface->hTagMap, TAG_SURFACE_RESIDUAL_FOOTSTEP_PARTICLE_SYSTEM);
            Player * ptPlayer = LocalPlayerGet();
            uint32 dwNow = CGameGetClientTicks();
            if (snoResidualFootstepParticleSystem == SNO_NONE)
            {
                if (ptPlayer->snoLastResidualFootstep != SNO_NONE &&
                    dwNow - ptPlayer->dwLastResidualFootstepTime < RESIDUAL_FOOTSTEP_TIME)
                {
                    ParticleSystemAddParams tParams(ptPlayer->snoLastResidualFootstep, transform);
                    ParticleSystemAdd(tParams);
                }
            }
            else
            {
                ptPlayer->snoLastResidualFootstep = snoResidualFootstepParticleSystem;
                ptPlayer->dwLastResidualFootstepTime = dwNow;
            }
        }
    }
}




static void sAttachCamera(
    const RActorID idRActor,
    const RMessage & msg)
{
    const RActor * ptRActor = RActorGet(idRActor);


    if (msg.tHardpoints[0].tHash == 0u)
        return;





    CinematographyHardpointView(ptRActor->id, msg.tHardpoints[0]);
}




static void sAttachSpawnedActor(
    const RActorID idRActor,
    const RMessage & msg)
{
    RActor * ptRActor = RActorGet(idRActor);

    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    // Inference: no actor attributes means no spawned visual attachment.
    if (!ptACD)
        return;


    SNO snoActor = ptACD->AttributesGetInt(ATTRIBUTE_GIZMO_ACTOR_SNO_TO_SPAWN);
    float flScale = ptACD->AttributesGetFloat(ATTRIBUTE_GIZMO_ACTOR_TO_SPAWN_SCALE);
    if (flScale < EPSILON)
        flScale = 1.0f;



    TriggerEvent tTriggerEvent;
    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_ACTOR;
    tTriggerEvent.snoname.Set(ACTOR_SNO, snoActor);
    tTriggerEvent.dwID = msg.tTriggerEvent.dwID;
    tTriggerEvent.tHardpointLinks[0].tHash = msg.tTriggerEvent.tHardpointLinks[0].tHash;
    tTriggerEvent.flScale = flScale;
    RActorSendRMessage(*ptRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, PARENT_RACTOR);
}




static BOOL sGenerateFrostBreath(
    const RActor & tRActor)
{
    SSceneID idSScene = tRActor.idSScene;
    if (idSScene == SSCENEID_INVALID)
        return FALSE;



    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
    if (!ptSCD ||
        ptSCD->tSceneSpec.snoWeather == SNO_NONE)
        return FALSE;



    ScopedWeatherDefinitionLockR pdefWeather(ptSCD->tSceneSpec.snoWeather);
    // Inference: the caller tests the weather's frost-breath flag.
    return (pdefWeather->dwFlags & (1 << WEATHER_CAUSES_FROST_BREATH_BIT)) != 0;
}



static void sFrostBreathCreate(
    const RActorID idRActor,
    const RMessage & msg)
{
    const RActor * ptRActor = RActorGet(idRActor);


    if (msg.tHardpoints[0].tHash == 0u)
        return;





    if (sGenerateFrostBreath(*ptRActor))
    {
        SNO snoBreathParticle = GlobalSNOGet(GLOBALSNO_PARTICLE_FROST_BREATH);
        if (snoBreathParticle != SNO_NONE)
        {
            PRTransform transform;
            RActorGetHardpointTransform(*ptRActor, msg.tHardpoints[0], transform);

            ParticleSystemAddParams tParams(snoBreathParticle, transform);
            ParticleSystemAdd(tParams);
        }
    }
}






static void sPlayTriggeredAnim(
    const RActorID idRActor,
    const RMessage & msg)
{

    RActor * ptRActor = RActorGet(idRActor);

    BOOL bDisableBlending = ptRActor->dwCreationTick == CGameGetClientTicks();
    RActorPlayAnimation(ptRActor, msg.tTriggerEvent.dwAnimTag, bDisableBlending, FALSE);
}




static void sModifyTransparency(
    const RActorID idRActor,
    const RMessage & msg)
{






    RActor * ptRActor = RActorGet(idRActor);
    ptRActor->flAlpha = msg.tTriggerEvent.flAlpha;
}






static void sModifyShadow(
    const RActorID idRActor,
    const RMessage & msg)
{






    RActor * ptRActor = RActorGet(idRActor);
    BOOL bShadow = msg.tTriggerEvent.dwPinFlags;






    // Inference: the line-table groups the conditional flag update on one line.
    if (bShadow) ptRActor->dwFlags |= (1 << RACTOR_SHADOWS_ENABLED_BIT); else ptRActor->dwFlags &= ~(1 << RACTOR_SHADOWS_ENABLED_BIT);
}






static void sModifyCollisionMessages(
    const RActorID idRActor,
    const RMessage & msg)
{


    RActor * ptRActor = RActorGet(idRActor);
    BOOL bDisableCollisionMessages = msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLE_COLLISION_MESSAGES_BIT);
    // Inference: original and/orr/csel updates this flag as a single conditional statement.
    if (bDisableCollisionMessages) ptRActor->dwFlags |= (1 << RACTOR_DISABLE_COLLISION_MESSAGES_BIT); else ptRActor->dwFlags &= ~(1 << RACTOR_DISABLE_COLLISION_MESSAGES_BIT);
}






static void sChangeLook(
    const RActorID idRActor,
    const RMessage & msg)
{



    RActorChangeLook(idRActor, msg.tTriggerEvent.tLookLink.dwLookHash);
}









static void sCancelLook(
    const RActorID idRActor,
    const RMessage & msg)
{



    RActorCancelLook(idRActor, msg.tTriggerEvent.tLookLink.dwLookHash);
}









static void sSetFade(
    RActor & tRActor,
    const RMessage & msg)
{
    float flFadeTime = msg.tTriggerEvent.flFadeTime;
    if (flFadeTime <= EPSILON)
        return;
    // Inference: the line-0 comparison rejects fade times over the original literal 10000.
    if (flFadeTime > 10000.0f)
        return;







    BOOL bAffectsShadows = msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_FADE_AFFECTS_SHADOWS);
    BOOL bFadeIn = msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_FADE_IN);
    if (bAffectsShadows) tRActor.dwFlags |= (1 << RACTOR_FADE_AFFECTS_SHADOWS_BIT); else tRActor.dwFlags &= ~(1 << RACTOR_FADE_AFFECTS_SHADOWS_BIT);
    // Inference: original literal 60 converts seconds to client fade ticks.
    float flFadeTicks = 60.0f * msg.tTriggerEvent.flFadeTime;

    tRActor.flTriggerFadePerTick = 1.0f / flFadeTicks;
    if (bFadeIn)
    {

        tRActor.flTriggerFadeAlpha = 0.0f;
    }
    else
    {
        tRActor.flTriggerFadePerTick = -tRActor.flTriggerFadePerTick;
    }
}






static void sSetOutroAnim(
    RActor & tRActor,
    const RMessage & msg)
{
    OutroAnimationType eOutroAnimType = msg.tTriggerEvent.eOutroAnimType;
    RActorAnimTreeSetOutroAnim(tRActor, eOutroAnimType);
}




static void sHideHelmAndShoulders(
    RActor * ptRActorPrimary)
{


    if (!ACDIsPlayer(ptRActorPrimary->idACD))
        return;



    Player * ptPlayer = PlayerGetByACD(ptRActorPrimary->idACD);


    VisualInventory * ptVisualInventory = VisualInventoryGet(ptRActorPrimary->idACD);


    VisualInventory tVisualInventoryBlank;
    tVisualInventoryBlank.tEquipmentCurrent.Init();
    tVisualInventoryBlank.InitializeLast();
    tVisualInventoryBlank.tEquipmentLast.tVisualItem[VIS_HEAD] = ptVisualInventory->tEquipmentCurrent.tVisualItem[VIS_HEAD];
    tVisualInventoryBlank.tEquipmentLast.tVisualItem[VIS_SHOULDERS] = ptVisualInventory->tEquipmentCurrent.tVisualItem[VIS_SHOULDERS];
    VisualInventoryApply(ptRActorPrimary, tVisualInventoryBlank, VIT_PLAYER, PlayerGetPlayerClass(ptRActorPrimary->idACD), !PlayerIsMale(*ptPlayer));
}




void RActorGetHardpointTransform(
    const RActor & tRActor,
    const int32 nBoneIndex,
    const PRTransform & transformHPModelSpace,
    PRTransform & transform)
{
    PRTransform transformHardpoint;
    // Inference: the line-0 null branch shares the invalid-bone fallback.
    if (tRActor.ptActorAnim && nBoneIndex != -1)
    {

        const ActorAnim & tActorAnim = *tRActor.ptActorAnim;
        ProcessedBone * ptBone = tActorAnim.ptProcessedBoneList + nBoneIndex;










        transformHardpoint = ptBone->transformDelta * transformHPModelSpace;
    }
    else
    {
        transformHardpoint = transformHPModelSpace;
    }




    const PRSTransform & transformRActor = tRActor.transformClient;
    transform = transformRActor * transformHardpoint;
}




void RActorGetHardpointTransform(const RActor & tRActor, const Hardpoint & tHardpoint, PRTransform & transform)
{



    RActorGetHardpointTransform(tRActor, tHardpoint.nBoneIndex, tHardpoint.transform, transform);
}



int32 RActorGetClosestDamageHardpointIndex(
    const RActorID idRActor,
    WorldPlace wpPosition,
    BOOL bGetFarthest,
    const AppearanceDefinition * pdefAppearance)
{
    const RActor * ptRActor = RActorGet(idRActor);
    // Inference: the original incoming pointer controls the final unlock.
    BOOL fLocalLock = pdefAppearance == NULL;
    if (fLocalLock)
    { // Inference: LockR returns the DWARF AppearanceDefinition as void *.
        pdefAppearance = (const AppearanceDefinition *)gptGroupDefAppearance->LockR(ptRActor->snoAppearance, FALSE);
    }

    const Structure & tStructure = pdefAppearance->tStructure;
    // Inference: line-0 fmov initializes the distance accumulator to zero.
    WorldDistance wdCurrentMin = 0.0f;
    int32 nClosestHardpointIndex = -1;

    for (int32 i = 0; i < tStructure.nHardpointCount; i++)
    {
        if (tStructure.ptHardpoints[i].tHash.IsDamage() ||
            (DEFAULT_HARDPOINT_NAME == tStructure.ptHardpoints[i].tHash))
        {



            PRTransform transform;
            RActorGetHardpointTransformByIndex(*ptRActor, i, transform);
            WorldDistance wdDistance = Vector3D::DistanceSquared(wpPosition.wp, transform.wp);

            if (nClosestHardpointIndex == -1 || (bGetFarthest ? wdDistance > wdCurrentMin : wdDistance < wdCurrentMin))
            {
                wdCurrentMin = wdDistance;
                nClosestHardpointIndex = i;
            }
        }
    }
    if (fLocalLock)
    { // Inference: UnlockR takes void *& and clears the typed definition pointer.
        gptGroupDefAppearance->UnlockR((void *&)pdefAppearance);
    }

    return nClosestHardpointIndex;
}




void RActorGetHardpointByIndex(const RActor & tRActor, const int32 nHardpointIndex, Hardpoint & tHardpoint)
{


    int32 nHardpointIndexToUse = nHardpointIndex;
    ScopedAppearanceDefinitionLockR pdefAppearance(tRActor.snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;



    if (nHardpointIndexToUse == -1)
        nHardpointIndexToUse = tStructure.nHardpointCount - 1;
    tHardpoint = tStructure.ptHardpoints[nHardpointIndexToUse];




}




void RActorGetHardpointTransformByIndex(const RActor & tRActor, const int32 nHardpointIndex, PRTransform & transform)
{



    Hardpoint tHardpoint;
    RActorGetHardpointByIndex(tRActor, nHardpointIndex, tHardpoint);

    RActorGetHardpointTransform(tRActor, tHardpoint, transform);
}

























































void RActorClearActorAttachment(RActor & tRActor, const uint32 dwAttachmentID)
{
    AttachmentFindAndDelete(tRActor.tAttachmentList, dwAttachmentID, TRUE);
}






AttachmentID RActorAddAttachment(RActor & tRActor, const AttachmentType eAttachmentType, const SNO sno, const uint32 dwAttachmentID, HardpointHash szHardpointName, BOOL bIgnoreOwnerScale)
{





    TriggerEvent tTriggerEvent;

    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = eAttachmentType;
    tTriggerEvent.snoname.Set(TriggerEventGetSnoGroup(tTriggerEvent), sno);
    tTriggerEvent.dwID = dwAttachmentID;
    tTriggerEvent.fIgnoreOwnerScale = bIgnoreOwnerScale;
    tTriggerEvent.tHardpointLinks[0].tHash = szHardpointName;

    return RActorSendRMessage(tRActor, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
}




AttachmentID RActorAttachExistingRActor(const RActorID idRActorMain, const RActorID idRActorAttachment, HardpointHash szHardpointName)
{


    if (idRActorMain == RACTORID_INVALID)
        return ATTACHMENTID_INVALID;

    RActor * ptRActorMain = RActorTryToGet(idRActorMain);
    if (!ptRActorMain || !RActorTryToGet(idRActorAttachment))
        return ATTACHMENTID_INVALID;


    ParentData tParentData;
    tParentData.SetHardpoint(PARENT_RACTOR, idRActorMain, szHardpointName);

    RActorSetParent(idRActorAttachment, tParentData, TRUE, TRUE);

    TriggerEvent tTriggerEvent;
    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_ACTOR;
    tTriggerEvent.dwID = 0x42;
    tTriggerEvent.dwPinFlags = idRActorAttachment;
    tTriggerEvent.snoname.Set(1 /* SNO group Actor */, SNO_NONE);
    tTriggerEvent.fIgnoreOwnerScale = TRUE;

    tTriggerEvent.tHardpointLinks[0].tHash = szHardpointName;

    return RActorSendRMessage(*ptRActorMain, tTriggerEvent, 0.0f, NULL, NULL, 0, -1.0f, FALSE, 0.0f, 0);
}




void RActorDetachExistingRActor(
    const RActorID idRActor,
    ACDID idACDOwner)
{
    if (!RActorTryToGet(idRActor))
        return;



    RActor * ptRActor = RActorGet(idRActor);

    if (ptRActor->idACD == ACDID_INVALID || idACDOwner == ACDID_INVALID)
        return;


    // Inference: ACDGetOwner returns the actor DataID used by this RActor owner lookup.
    RActorID idRActorOwner = (RActorID)ACDGetOwner(idACDOwner);
    if (idRActorOwner == RACTORID_INVALID)
        return;



    RActor * ptRActorOwner = RActorGet(idRActorOwner);

    POS pos = ptRActorOwner->tAttachmentList.GetHeadPosition();
    while (pos)
    {
        AttachmentID idAttachment = ptRActorOwner->tAttachmentList.GetNext(pos);
        Attachment & tAttachment = AttachmentGet(idAttachment);

        if (tAttachment.snoname.snoGroup == ACTOR_SNO)
        { // Inference: the Actor SNO guard gives the attached DataID its RActorID meaning.
            RActor * ptRActorAttachment = RActorTryToGet((RActorID)tAttachment.idAttachedObject);

            if (ptRActorAttachment && ptRActorAttachment->idACD == ptRActor->idACD)
            {
                RActorAttachmentDetach(idRActorOwner, idAttachment);
                // Inference: the original detach path returns through a tail call.
                return;
            }
        }
    }

}



BOOL RActorIsRemote(const RActor & tRActor)
{








    ACDID idACDPlayer = ACDGetControllingActor(tRActor.idACD);
    if (ACDIsPlayer(idACDPlayer))
    {
        if (!PlayerIsLocal(idACDPlayer))
            return TRUE;
    }



    if (tRActor.dwFlagsEx & (1 << RACTOREX_FORCE_REMOTE_FX))
        return TRUE;



    ActorCommonData * ptACD = ACDTryToGet(tRActor.idACD);
    if (ptACD)
    {
        SNO snoMonster = ActorGetMonsterSNO(ptACD->snoActor);
        if (snoMonster != SNO_NONE)
        {

            if (TagMapGetInt(ScopedMonsterDefinitionLockR(snoMonster)->hTagMap, TAG_MONSTER_FORCE_REMOTE_PLAYER_EFFECTS))
                return TRUE;
        }
    }




    return FALSE;
}

