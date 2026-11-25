// X/Client/Attachments.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/p2-ractor/atc/frag.cpp by build/try/p2-ractor/gen.py.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Attachments.h"
#include "CGame.h"
#include "Particle.h"
#include "Rope.h"
#include "Scene.h"
#include "ParticleInternal.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "Sound/Sound.h"
#include "Definitions/SoundDefinition.h"
#include "RActors/RActors.h"
#include "RActors/RActorAttachments.h"
#include "Definitions/PreferencesDefinition.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameTemplate.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"

enum {
    MAX_ATTACHMENTS = 1024,
};

// X/Client/Sound/Sound.cpp:1511.

void SoundFadeOut(const SoundID idSound);






void ShakePlayOutro(const int32 idShake);  // X/Client/Shake.cpp
void RActorDetach(const RActorID idRActor);  // X/Client/RActors/RActors.cpp  // X/Client/Shake.cpp
namespace Vibration {
void PlayOutro(const int32 idVibration);  // X/Client/Vibration.cpp
}

AttachmentID AttachmentAdd(const RMessage & msg, const DataID idAttachedObject);
void AttachmentDelete(Attachment & tAttachment);
void AttachmentDetach(Attachment & tAttachment);
BOOL AttachmentOutro(Attachment & tAttachment);
BOOL AttachmentIsLost(Attachment & tAttachment);
static XPooledDataIDList * sGetAttachmentList(const Attachment & tAttachment);
static DataID sAttachmentCreate(const RMessage & msg, const ParentData & tParentData, const PRTransform & transform, const float flScale, const float flLifetime, const BOOL fOverrideScale);























void AttachmentsInitialize()
{

    AttachmentGlobals *& ptAttachmentGlobals = CGameGet().ptAttachmentGlobals;
    ptAttachmentGlobals = (AttachmentGlobals *)SigmaMemoryNew(sizeof(AttachmentGlobals), 4, NULL, TRUE);

    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    if (eGameMode != GAMEMODE_PLAYING)
        ptAttachmentGlobals->ptAttachments = TGrowableDataArrayCreateTracked<Attachment, AttachmentID>("Attachments", ptMemoryInterface, MAX_ATTACHMENTS);
    else

        ptAttachmentGlobals->ptAttachments = TDataArrayCreateTracked<Attachment, AttachmentID>("Attachments", ptMemoryInterface, MAX_ATTACHMENTS);

}




void AttachmentsDispose()
{

    AttachmentGlobals *& ptAttachmentGlobals = CGameGet().ptAttachmentGlobals;
    TDataArrayDispose(ptAttachmentGlobals->ptAttachments);
    SigmaMemoryFree(ptAttachmentGlobals, AppGetCurrentMemoryInterface()); ptAttachmentGlobals = NULL;
}




Attachment * AttachmentTryToGet(const AttachmentID idAttachment)
{

    return CGameGet().ptAttachmentGlobals->ptAttachments->ElementTryToGet(idAttachment);
}



Attachment * AttachmentGet(const AttachmentID idAttachment)
{

    return CGameGet().ptAttachmentGlobals->ptAttachments->ElementGet(idAttachment);
}




BOOL ShouldDisableForLowFXMode(const RMessage & msg)
{

    BOOL fDisabledInLowFXMode = msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IN_LOWFX_MODE);
    if (fDisabledInLowFXMode && (Prefs::GetVideoPrefs().GetUseLowFXMode() || GameTypeIsPvP()))
        return TRUE;



    BOOL fEnabledInLowFXMode = msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_ENABLED_IN_LOWFX_MODE);
    if (fEnabledInLowFXMode && !Prefs::GetVideoPrefs().GetUseLowFXMode() && !GameTypeIsPvP())
        return TRUE;
    return FALSE;



}





































































































































































































































































































































AttachmentID AttachmentCreate(const RMessage & msg, const ParentData & tParentData, const PRTransform & transform, const float flScale, const BOOL fOverrideScale)
{





    DataID dataID = sAttachmentCreate(msg, tParentData, transform, flScale, -1.0f, fOverrideScale);
    if (dataID == DATAID_INVALID)
        return ATTACHMENTID_INVALID;




    return AttachmentAdd(msg, dataID);
}
























































































































































































































































void AttachmentsCleanupLostAttachments()
{

    CGame & tCGame = CGameGet();
    TDataArrayIterator<Attachment> tIt;
    while (tCGame.ptAttachmentGlobals->ptAttachments->Iterate(tIt))
    {
        Attachment & tAttachment = *tIt.ptElement;

        if (AttachmentIsLost(tAttachment))
        {
            AttachmentDelete(tAttachment);
        }
    }
}




void AttachmentsUpdate()
{

    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    if (eGameMode == GAMEMODE_PLAYING || eGameMode == GAMEMODE_HOLODECK || eGameMode == GAMEMODE_MENU)
    {





        AttachmentsCleanupLostAttachments();
    }
}


















































AttachmentID AttachmentAdd(const RMessage & msg, const DataID idAttachedObject)
{


    AttachmentGlobals * ptAttachmentGlobals = CGameGet().ptAttachmentGlobals;
    AttachmentID idAttachment = ptAttachmentGlobals->ptAttachments->ElementNew();

    if (idAttachment == ATTACHMENTID_INVALID)
        return ATTACHMENTID_INVALID;











    Attachment & tAttachment = *AttachmentGet(idAttachment);
    tAttachment.idAttachedObject = idAttachedObject;

    SNOGroup snoGroup = TriggerEventGetSnoGroup(msg.tTriggerEvent);

    tAttachment.snoname.Set(snoGroup, msg.tTriggerEvent.snoname.snoHandle);
    tAttachment.dwTriggerFlags = msg.tTriggerEvent.dwTriggerFlags;
    tAttachment.dwID = msg.tTriggerEvent.dwID;

    tAttachment.nParentType = msg.nParentType;
    tAttachment.idParent = msg.idParent;

    if (tAttachment.nParentType == PARENT_PARTICLE)
    {
        Particle * ptParticle = ParticleTryToGet(tAttachment.idParent);
        if (ptParticle)
        {
            tAttachment.idListOwner = ptParticle->tParams.idParticleSystem;
        }
    }
    else
    {
        tAttachment.idListOwner = tAttachment.idParent;
    }


    for (int32 i = 0; i < 2; i++)
    {
        tAttachment.tHP[i].tHash = msg.tHardpoints[i].tHash;
    }

    XPooledDataIDList * ptAttachmentList = sGetAttachmentList(tAttachment);
    if (!ptAttachmentList) { ptAttachmentGlobals->ptAttachments->ElementDeleteByID(idAttachment); return ATTACHMENTID_INVALID; }















    ptAttachmentList->AddTail_Tracked(idAttachment);


    return idAttachment;
}













Attachment * AttachmentFind(XPooledDataIDList & tAttachmentList, const uint32 dwID)
{


    POS posAffected = tAttachmentList.GetHeadPosition();
    while (posAffected)
    {
        AttachmentID idAttachment = tAttachmentList.GetNext(posAffected);
        Attachment * ptAffectedInstance = AttachmentTryToGet(idAttachment);



        if (ptAffectedInstance && ptAffectedInstance->dwID == dwID)
            return ptAffectedInstance;
    }





    return NULL;
}




void AttachmentFindAndDelete(XPooledDataIDList & tAttachmentList, const uint32 dwID, const BOOL fOutro)



{
    POS posAffected = tAttachmentList.GetHeadPosition();
    while (posAffected)
    {
        AttachmentID idAttachment = tAttachmentList.GetNext(posAffected);
        Attachment * ptAffectedInstance = AttachmentTryToGet(idAttachment);



        if (ptAffectedInstance &&
            ptAffectedInstance->dwID == dwID)
        {
            if (fOutro)
            {
                AttachmentOutro(*ptAffectedInstance);
            }
            else
            {
                AttachmentDelete(*ptAffectedInstance);
            }
        }
    }
}





void AttachmentFindAndDetach(XPooledDataIDList & tAttachmentList, const uint32 dwID)


{
    POS posAffected = tAttachmentList.GetHeadPosition();
    while (posAffected)
    {
        AttachmentID idAttachment = tAttachmentList.GetNext(posAffected);
        Attachment * ptAffectedInstance = AttachmentTryToGet(idAttachment);



        if (ptAffectedInstance &&
            ptAffectedInstance->dwID == dwID)
        {
            AttachmentDetach(*ptAffectedInstance);
        }
    }
}





void AttachmentDetach(Attachment & tAttachment)
{

    XPooledDataIDList * ptAttachmentList = sGetAttachmentList(tAttachment);
    if (ptAttachmentList)
    {
        POS pos = ptAttachmentList->Find(tAttachment.id);
        if (pos)
            ptAttachmentList->RemoveAt(pos);
    }







    if (CGameTraceAttachmentsEnabled()) {}






    SNOName snoname = tAttachment.snoname;
    DataID idAttachedObject = tAttachment.idAttachedObject;

    tAttachment.idAttachedObject = DATAID_INVALID;
    CGameGet().ptAttachmentGlobals->ptAttachments->ElementDelete(&tAttachment);

    if (idAttachedObject == DATAID_INVALID)
        return;



    switch (snoname.snoGroup)
    {
    case ACTOR_SNO:
        RActorDetach((RActorID)idAttachedObject);
        break;

    case PARTICLE_SNO:
        {
            ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(idAttachedObject);
            if (ptParticleSystem)
            {
                ptParticleSystem->Detach();
                ptParticleSystem->PlayOutro();
            }
        }
        break;

    case ROPE_SNO:
        RopePlayOutro(idAttachedObject);
        break;

    case SHAKE_SNO:



        ShakePlayOutro(idAttachedObject);
        break;

    case VIBRATION_SNO:

        Vibration::PlayOutro(idAttachedObject);
        break;

    default:
        break;
    }



}





































































































BOOL AttachmentOutro(Attachment & tAttachment)
{


    BOOL fSuccessfullyOutroed = FALSE;



    switch (tAttachment.snoname.snoGroup)
    {
    case ROPE_SNO:
        RopePlayOutro(tAttachment.idAttachedObject);
        fSuccessfullyOutroed = TRUE;
        break;

    case PARTICLE_SNO:
        {
            ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(tAttachment.idAttachedObject);
            if (ptParticleSystem)
            {
                ptParticleSystem->PlayOutro();
            }
            fSuccessfullyOutroed = TRUE;
        }
        break;

    case SHAKE_SNO:
        ShakePlayOutro(tAttachment.idAttachedObject);
        fSuccessfullyOutroed = TRUE;
        break;

    case VIBRATION_SNO:

        Vibration::PlayOutro(tAttachment.idAttachedObject);
        fSuccessfullyOutroed = TRUE;
        break;

    case SOUND_SNO:
        {

            SoundInstance * ptSound = SoundTryToGet(tAttachment.idAttachedObject);
            if (ptSound)
            {







                ScopedSoundDefinitionLockR pdefSound(ptSound->tSoundParams.snoSound);
                if (pdefSound->dwFlags & (1 << SOUND_DELETED_WITH_SOURCE_BIT))
                {

                    SoundFadeOut(tAttachment.idAttachedObject);
                }
                fSuccessfullyOutroed = TRUE;
            }
        }
        break;
    }

    if (fSuccessfullyOutroed)
    {
        if (CGameTraceAttachmentsEnabled()) {}
    }
    else
    {




        AttachmentDelete(tAttachment);
    }
    return fSuccessfullyOutroed;
}



























































void AttachmentListHandleParentDeletion(XPooledDataIDList & tAttachmentList)

{
    POS pos = tAttachmentList.GetHeadPosition();
    while (pos)
    {
        AttachmentID idAttachment = tAttachmentList.GetNext(pos);
        Attachment * ptAttachment = AttachmentTryToGet(idAttachment);




        if (ptAttachment && AttachmentOutro(*ptAttachment))
        {
            AttachmentDetach(*ptAttachment);
        }
    }
}




static XPooledDataIDList * sGetAttachmentList(const Attachment & tAttachment)


{
    switch (tAttachment.nParentType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        {
            RActor * ptRActor = RActorTryToGet((RActorID)tAttachment.idListOwner);
            if (!ptRActor)
                return NULL;


            return &ptRActor->tAttachmentList;
        }

    case PARENT_PARTICLE_SYSTEM:
    case PARENT_PARTICLE:
        {

            ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(tAttachment.idListOwner);
            if (!ptParticleSystem)
                return NULL;


            return &ptParticleSystem->tAttachmentList;
        }

    case PARENT_ROPE:
        {


            Rope * ptRope = RopeTryToGet(tAttachment.idListOwner);
            if (!ptRope)
                return NULL;


            return &ptRope->tAttachmentList;
        }

    case PARENT_SCENE:
        {

            CScene * ptCScene = CSceneTryToGet(tAttachment.idListOwner);
            if (!ptCScene)
                return NULL;


            return &ptCScene->tAttachmentList;
        }

    default:

        DisplayInternalError(NULL, "", 1372, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



}




void AttachmentListSendMsgToChildren(XPooledDataIDList & tAttachmentList, const KeyedMessage & tMsg)
{

    POS pos = tAttachmentList.GetHeadPosition();
    while (pos)
    {

        AttachmentID idAttachment = tAttachmentList.GetNext(pos);
        Attachment * ptAttachment = AttachmentTryToGet(idAttachment);

        if (!ptAttachment ||
            ptAttachment->idAttachedObject == DATAID_INVALID)
            continue;



        switch (ptAttachment->snoname.snoGroup)
        {
        case ACTOR_SNO:
            {



                if (tMsg.dwMsgKey == MSGKEY_RESERVED_LEAVING_ANIMATION)
                {
                    RActor * ptRActor = RActorTryToGet((RActorID)ptAttachment->idAttachedObject);
                    if (!ptRActor || ptRActor->ptActorAnim)
                        break;
                }



                RActorSendMsgKey((RActorID)ptAttachment->idAttachedObject, tMsg, MSGPASS_TO_SELF_AND_CHILDREN);
            }
            break;

        case PARTICLE_SNO:
            {
                ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(ptAttachment->idAttachedObject);
                if (ptParticleSystem)
                {
                    ptParticleSystem->SendMsgKey(tMsg, MSGPASS_TO_SELF_AND_CHILDREN);
                }
            }
            break;
        }
    }
}


















































































































































































BOOL MsgTriggeredEventShouldTrigger(const MsgTriggeredEvent & tEvent, const KeyedMessage & tMsg)
{


    if (tEvent.dwMsgKey != tMsg.dwMsgKey)
        return FALSE;

    const TriggerConditions & tConditions = tEvent.tTriggerEvent.tConditions;


    if (tMsg.flImpulseMagnitude > EPSILON)
    {
        if (tMsg.flImpulseMagnitude < tConditions.flImpulseMin)
            return FALSE;



        if ((tConditions.dwConditionFlags & (1 << TCF_USE_IMPULSE_DELTA_BIT)) &&
            tMsg.flImpulseMagnitude > tConditions.flImpulseMin + tConditions.flImpulseDelta)
            return FALSE;
    }




    return tMsg.eMaterial == tConditions.eMaterial;
}







bool ParentIsRActor(ParentType eType)
{
    return eType == 0 || eType == 5;
}

#include "Trails.h"
#include "Shake.h"
#include "Vibration.h"
#include "Lights.h"
#include "Explosions.h"
#include "Sound/AmbientSound.h"
// Authentic declarations from Trails.cpp, Shake.cpp and Sound.h.
Trail * TrailTryToGet(TrailID id);
void TrailDelete(TrailID id);
Shake * ShakeGet(ShakeID id);
BOOL ShakeExists(ShakeID id);
namespace SoundManager {
BOOL IsInitialized();
}
// Sound.h:479, declared inline in the original DWARF.
inline SoundInstance * SoundGet(SoundID id)
{
    return gptSounds->ElementGet(id);
}
// Owner-local recovery of Explosions.h definitions omitted by the adopted header.
struct Explosion {  // size 0x80
    void Initialize(SNO snoExplosion, const PRTransform & transform, const RenderContext eContext);
    void UpdatePropertiesFromParent();
    void Update();
    ExplosionID id;  // 0x00
    SNO m_snoExplosion;  // 0x04
    float m_flMinScale;  // 0x08
    float m_flDeltaScale;  // 0x0c
    float m_flChance;  // 0x10
    float m_flDuration;  // 0x14
    Vector3D m_wvLocalDirection;  // 0x18
    ExplosionImpulse m_tImpulse;  // 0x24
    ParentData m_tParentData;  // 0x54
};
static_assert(sizeof(Explosion) == 0x80, "Explosion");

class ExplosionManager {  // size 0xa818
public:
    static void InitializeForGame();
    static void DisposeForGame();
    ExplosionID Create(SNO snoExplosion, const PRTransform & transform, const RenderContext eContext);
    void Destroy(ExplosionID id);
    void SetParent(ExplosionID id, const ParentData & tParentData);
    Explosion * Get(ExplosionID id);
    Explosion * TryToGet(ExplosionID id);
    void Update(float flDeltaTime);
    void DrawDebugView(RWindow &);
    enum {
        MAX_EXPLOSIONS = 128,
    };
private:
    TDataArray<Explosion, int> * m_ptExplosions;  // 0x00
    ExplosionEntry m_pEntryPool[128];  // 0x08
    ExplosionEntry * m_pFreeEntryList;  // 0xa408
    ExplosionEntry * m_pEntries[128];  // 0xa410
    int32 m_nEntryCount;  // 0xa810
};
static_assert(sizeof(ExplosionManager) == 0xa818, "ExplosionManager");



void ParticleSystemDelete(const ParticleSystemID idParticleSystem);
void CSceneDelete(const CSceneID idCScene);
#line 485
void AttachmentSpawn(const RMessage & msg, const ParentData & tParentData, const PRTransform & transform, const float flScale, const BOOL fOverrideScale)
{
#line 493
    float flLifetime = -1.0f;
    if (msg.tTriggerEvent.dwSpawnedActorLife)
        flLifetime = msg.tTriggerEvent.dwSpawnedActorLife * (1.0f / 60.0f);
#line 500
    DataID dataID = sAttachmentCreate(msg, tParentData, transform, flScale, flLifetime, fOverrideScale);
    if (dataID == DATAID_INVALID)
        return;
#line 507
    SNOGroup snoGroup = TriggerEventGetSnoGroup(msg.tTriggerEvent);
    switch (snoGroup)
    {
    case ACTOR_SNO:
        {
#line 512
            RActor * ptRActor = RActorGet((RActorID)dataID);
            ptRActor->tParentData.Init();
        }
        break;
    case LIGHT_SNO:
        {
#line 519
            Light * ptLight = LightGet(dataID);
            ptLight->tParentData.Init();
        }
        break;
    case EXPLOSION_SNO:
        {
#line 526
            Explosion * ptExplosion = CGameGet().ptExplosionManager->Get(dataID);
            ptExplosion->m_tParentData.Init();
        }
        break;
    case SOUND_SNO:
        {
#line 533
            SoundInstance * ptSound = SoundGet(dataID);
            ptSound->tParentData.Init();
        }
        break;
    case PARTICLE_SNO:
        {
#line 563
            ParticleSystem * ptParticleSystem = ParticleSystemGet(dataID);
            ptParticleSystem->tParentData.Init();
        }
        break;
    case SHAKE_SNO:
        {
#line 540
            Shake * ptShake = ShakeGet(dataID);
            ptShake->tParentData.Init();
        }
        break;
    case VIBRATION_SNO:
        {
#line 548
            Vibration::VibrationInfo * ptVibration = Vibration::Get(dataID);
            ptVibration->tParentData.Init();
        }
        break;
    case AMBIENTSOUND_SNO:
        {
#line 556
            AmbientSound * ptAmbientSound = AmbientSoundGet(dataID);
            ptAmbientSound->m_tParentData.Init();
        }
        break;
    case SCENE_SNO:
        DisplayInternalError(NULL, "", 569, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    case ROPE_SNO:
        DisplayInternalError(NULL, "", 573, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    case TRAIL_SNO:
        DisplayInternalError(NULL, "", 577, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }
}
#line 1058
void AttachmentDelete(Attachment & tAttachment)
{
#line 1061
    XPooledDataIDList * ptAttachmentList = sGetAttachmentList(tAttachment);
    if (ptAttachmentList)
    {
#line 1064
        POS pos = ptAttachmentList->Find(tAttachment.id);
        if (pos)
            ptAttachmentList->RemoveAt(pos);
    }
    CGameTraceAttachmentsEnabled();
#line 1082
    SNOName snoname = tAttachment.snoname;
#line 1083
    DataID idAttachedObject = tAttachment.idAttachedObject;
    CGameGet().ptAttachmentGlobals->ptAttachments->ElementDelete(&tAttachment);
    if (idAttachedObject == DATAID_INVALID)
        return;
    switch (snoname.snoGroup)
    {
    case ACTOR_SNO: RActorDelete((RActorID)idAttachedObject); break;
    case LIGHT_SNO: LightDelete(idAttachedObject); break;
    case EXPLOSION_SNO: CGameGet().ptExplosionManager->Destroy(idAttachedObject); break;
    case PARTICLE_SNO: ParticleSystemDelete(idAttachedObject); break;
    case ROPE_SNO: RopeDelete(idAttachedObject); break;
    case SHAKE_SNO: ShakeDelete(idAttachedObject); break;
    case VIBRATION_SNO: Vibration::Delete(idAttachedObject); break;
    case SOUND_SNO: SoundFadeOut(idAttachedObject); break;
    case AMBIENTSOUND_SNO:
        {
#line 1126
            AmbientSound * ptAmbientSound = AmbientSoundTryToGet(idAttachedObject);
            if (ptAmbientSound)
                ptAmbientSound->Delete();
        }
        break;
    case SCENE_SNO: CSceneDelete(idAttachedObject); break;
    case TRAIL_SNO: TrailDelete(idAttachedObject); break;
    default:
        DisplayInternalError(NULL, "", 1153, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}
#line 1236
void AttachmentListHandleParentWarp(XPooledDataIDList & tAttachmentList)
{
#line 1239
    POS pos = tAttachmentList.GetHeadPosition();
    while (pos)
    {
#line 1242
        AttachmentID idAttachment = tAttachmentList.GetNext(pos);
#line 1243
        Attachment * ptAttachment = AttachmentTryToGet(idAttachment);
        if (!ptAttachment)
            continue;
        switch (ptAttachment->snoname.snoGroup)
        {
        case TRAIL_SNO:
            {
#line 1249
                Trail * ptAttachedTrail = TrailTryToGet(ptAttachment->idAttachedObject);
                if (ptAttachedTrail)
                    ptAttachedTrail->flTimeParentWarped = CGameGetClientTime() + (1.0f / 60.0f);
            }
            break;
        case ACTOR_SNO:
            {
#line 1257
                RActor * ptRActor = RActorTryToGet((RActorID)ptAttachment->idAttachedObject);
                if (ptRActor)
                    AttachmentListHandleParentWarp(ptRActor->tAttachmentList);
            }
            break;
        case PARTICLE_SNO:
            {
#line 1265
                ParticleSystem * ptAttachedParticleSystem = ParticleSystemTryToGet(ptAttachment->idAttachedObject);
                if (ptAttachedParticleSystem)
                {
                    AttachmentListHandleParentWarp(ptAttachedParticleSystem->tAttachmentList);
#line 1270
                    POS posParticle = ptAttachedParticleSystem->tParticleRActorList.GetHeadPosition();
                    while (posParticle)
                    {
#line 1273
                        RActorID idRActor = (RActorID)ptAttachedParticleSystem->tParticleRActorList.GetNext(posParticle);
#line 1274
                        RActor * ptRActor = RActorTryToGet(idRActor);
                        if (ptRActor)
                            AttachmentListHandleParentWarp(ptRActor->tAttachmentList);
                    }
                }
            }
            break;
        }
    }
}
#line 1430
BOOL AttachmentIsLost(Attachment & tAttachment)
{
    if (tAttachment.idAttachedObject == DATAID_INVALID)
        return TRUE;
#line 1438
    BOOL fDelete = FALSE;
    switch (tAttachment.snoname.snoGroup)
    {
    case ACTOR_SNO: fDelete = RActorTryToGet((RActorID)tAttachment.idAttachedObject) == NULL; break;
    case PARTICLE_SNO: fDelete = ParticleSystemTryToGet(tAttachment.idAttachedObject) == NULL; break;
    case LIGHT_SNO: fDelete = LightTryToGet(tAttachment.idAttachedObject) == NULL; break;
    case EXPLOSION_SNO: fDelete = CGameGet().ptExplosionManager->TryToGet(tAttachment.idAttachedObject) == NULL; break;
    case SOUND_SNO:
        if (!SoundManager::IsInitialized())
        {
            tAttachment.idAttachedObject = DATAID_INVALID;
            fDelete = TRUE;
        }
        else
            fDelete = SoundTryToGet(tAttachment.idAttachedObject) == NULL;
        break;
    case SHAKE_SNO: fDelete = !ShakeExists(tAttachment.idAttachedObject); break;
    case VIBRATION_SNO: fDelete = !Vibration::Exists(tAttachment.idAttachedObject); break;
    case AMBIENTSOUND_SNO: fDelete = AmbientSoundTryToGet(tAttachment.idAttachedObject) == NULL; break;
    case SCENE_SNO: fDelete = CSceneTryToGet(tAttachment.idAttachedObject) == NULL; break;
    case TRAIL_SNO: fDelete = TrailTryToGet(tAttachment.idAttachedObject) == NULL; break;
    case ROPE_SNO: fDelete = RopeTryToGet(tAttachment.idAttachedObject) == NULL; break;
    default:
        DisplayInternalError(NULL, "", 1503, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return fDelete;
}

#include "../Common/ActorCommonData.h"
#include "Definitions/VibrationDefinition.h"
// Authentic external declarations from the corresponding owner .cpp DWARF.
void ParticleSystemDelete(const ParticleSystemID idParticleSystem);
void CSceneDelete(const CSceneID idCScene);
BOOL RActorIsCritterWithSNO(SNO snoActor);
void RActorSetParent(const RActorID idRActor, const ParentData & tParentData, const BOOL fIgnoreOwnerAlpha, const BOOL fIgnoreOwnerScale);
BOOL PlayerIsLocal(RActorID idRActor);
BOOL PlayerIsLocal(ACDID idACD);
BOOL PlayerIsLocal(int32 nPlayerIndex);
BOOL ACDIsPlayer(ACDID idACD);
BOOL ACDIsPet(ACDID idACD);
LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext);
ShakeID ShakeAdd(const SNO snoShake, const PRTransform & transform, const RenderContext eContext);
void ShakeSetParent(const ShakeID idShake, const ParentData & tParentData);
void SceneSpecInitialize(SceneSpecification & tSceneSpec);
CSceneID CSceneAdd(const SNO snoScene, const SSceneID idSScene, const SceneSpecification & tSceneSpec, const SWorldID idSWorld, const PRTransform & transform, const SSceneID idSSceneParent, const SNO snoSceneGroup, BOOL bIsAttachment, BOOL bIsVista, const RenderContext eContext, const SNO * snoActors, uint32 uActorCount);
void CSceneSetParent(const CSceneID idCScene, const ParentData & tParentData);
// Inferred structured switch/early exits from original branches. Literal flags 1/4,
// visibility mask 2, sound message key 69 and attribute 0x502 are original immediates.
// Actor, sound and shake cases re-laid on the original rows/columns (212-237, 292-337, 338-387).
#line 143
static DataID sAttachmentCreate(const RMessage & msg, const ParentData & tParentData, const PRTransform & transform, const float flScale, const float flLifetime, const BOOL fOverrideScale)
{
#line 151
    DataID dataID = DATAID_INVALID;
#line 153
    RenderContext eContext = RENDERCONTEXT_DEFAULT;
    switch (msg.nParentType)
    {
    case PARENT_RACTOR:
    case PARENT_RACTOR_TOP:
        {
#line 156
            RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)msg.idParent, FALSE, FALSE);
#line 157
            const RActor * ptOwnerRActor = RActorTryToGet(idRActorTopmost);
            if (ptOwnerRActor)
            {
#line 160
                BOOL fDisabledIfOwnerHidden = msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_OWNER_HIDDEN);
                if (fDisabledIfOwnerHidden && (ptOwnerRActor->dwFlags & ((1 << RACTOR_HIDE_BIT) | (1 << RACTOR_NEVER_DRAW_BIT))))
                    return DATAID_INVALID;
                eContext = ptOwnerRActor->eContext;
            }
        }
        break;
    case PARENT_PARTICLE_SYSTEM:
        {
#line 171
            ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(msg.idParent);
            if (ptParticleSystem)
                eContext = (RenderContext)ptParticleSystem->eContext;
        }
        break;
    case PARENT_PARTICLE:
        {
#line 179
            Particle * ptParticle = ParticleTryToGet(msg.idParent);
            if (ptParticle)
            {
#line 182
                ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(ptParticle->idParticleSystem);
                if (ptParticleSystem)
                    eContext = (RenderContext)ptParticleSystem->eContext;
            }
        }
        break;
    case PARENT_ROPE:
        {
#line 191
            Rope * ptRope = RopeTryToGet(msg.idParent);
            if (ptRope)
                eContext = ptRope->eContext;
        }
        break;
    }
    if (ShouldDisableForLowFXMode(msg))
        return DATAID_INVALID;
    if (msg.bRemote && (msg.tTriggerEvent.dwTriggerFlags & (1 << TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER)))
        return DATAID_INVALID;
    switch (msg.tTriggerEvent.eAttachmentType)
    {
    case ATTACHMENTTYPE_ACTOR:
        {
#line 212
            uint32 dwAddFlags = 0;
            if (fOverrideScale)
                dwAddFlags |= 4;


            if (!RActorIsCritterWithSNO(msg.tTriggerEvent.snoname.snoHandle))
                dwAddFlags |= 1;





            if (msg.tTriggerEvent.snoname.snoHandle != SNO_NONE)
            {
                dataID = RActorCreate(msg.tTriggerEvent.snoname.snoHandle, flScale, flLifetime, transform, dwAddFlags, eContext, SSCENEID_INVALID);
            }


            // 231:43: the RActorTryToGet test of the pin-flags actor id.
            else if (RActorTryToGet((RActorID)msg.tTriggerEvent.dwPinFlags))
            {
                dataID = msg.tTriggerEvent.dwPinFlags;
            }


            RActorSetParent((RActorID)dataID, tParentData, msg.tTriggerEvent.fIgnoreOwnerAlpha, msg.tTriggerEvent.fIgnoreOwnerScale);
        }
        break;
    case ATTACHMENTTYPE_PARTICLESYSTEM:
        {
#line 243
            ParticleSystemAddParams tParams(msg.tTriggerEvent.snoname.snoHandle, transform);
            tParams.tParentData = tParentData;
            tParams.fIgnoreOwnerAlpha = msg.tTriggerEvent.fIgnoreOwnerAlpha;
            tParams.eContext = eContext;
            if (msg.tTriggerEvent.fUseParticleScalar)
                tParams.flParticleScalar = msg.tTriggerEvent.flParticleScalar;
            tParams.bRemote = (msg.tTriggerEvent.dwTriggerFlags >> TRIGGERFLAG_DISABLED_IF_REMOTE_PLAYER) & 1;
            dataID = ParticleSystemAdd(tParams);
            if (dataID != DATAID_INVALID)
            {
#line 256
                ParticleSystem * ptParticleSystem = ParticleSystemGet(dataID);
                if (ParentIsRActor(tParentData.m_eParentType) && !(msg.tHardpoints[1].tHash == DEFAULT_HARDPOINT_NAME))
                {
#line 261
                    HardpointLink tLink;
                    tLink.tHash = msg.tHardpoints[1].tHash;
                    tLink.eSource = HARDPOINTSOURCE_PRIMARY;
                    ptParticleSystem->SetAttractor(cgwpNull, (RActorID)msg.idParent, &tLink);
                }
            }
        }
        break;
    case ATTACHMENTTYPE_LIGHT:
        {
#line 272
            uint32 dwFlags = 0;
#line 274
            int32 nPriorityBoost = 0;
            if (ParentIsRActor(tParentData.m_eParentType))
            {
#line 277
                RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)msg.idParent, FALSE, FALSE);
                if (PlayerIsLocal(idRActorTopmost))
                    nPriorityBoost = 1;
            }
            dataID = LightAdd(dwFlags, msg.tTriggerEvent.snoname.snoHandle, transform, nPriorityBoost, eContext);
            LightSetParent(dataID, tParentData);
        }
        break;
    case ATTACHMENTTYPE_SOUND:
        {
#line 292
            BOOL bValidSound = TRUE;

            if (bValidSound)
            {


                BOOL fPlayAs2D = (msg.tTriggerEvent.dwTriggerFlags >> TRIGGERFLAG_SOUND_PLAY_AS_2D) & 1;

                int32 nPermIndex = -1;
                if (msg.tTriggerEvent.dwID == 69)
                    nPermIndex = msg.tTriggerEvent.dwPinFlags;



                BOOL fOccluded = FALSE;
                if (ParentIsRActor(msg.nParentType))
                {
                    RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)msg.idParent, FALSE, FALSE);
                    const RActor * ptOwnerRActor = RActorGet(idRActorTopmost);
                    if (ptOwnerRActor->dwFlagsEx & (1 << RACTOREX_SOUNDS_MUTED))  // 311:29 tbnz straight to the shared DATAID_INVALID return; nPermIndex..ptSound are scoped inside the bValidSound block (DWARF)
                        return DATAID_INVALID;


                    fOccluded = (ptOwnerRActor->dwVisibilityFlags >> 1) & 1;
                    if (ptOwnerRActor->eContext != RENDERCONTEXT_DEFAULT && ptOwnerRActor->eContext != RENDERCONTEXT_HOLODECK)
                        fPlayAs2D = TRUE;
                }




                PlaySoundOptions tOptions;
                tOptions.eContext = eContext;
                tOptions.pwp = &transform.wp;
                tOptions.nPermutationIndex = nPermIndex;
                tOptions.fForce2D = fPlayAs2D;
                tOptions.fOccluded = fOccluded;
                tOptions.flImpulse = msg.flImpulse;
                tOptions.flStartOffset = msg.flStartOffset;
                dataID = SoundManager::PlaySound(msg.tTriggerEvent.snoname.snoHandle, tOptions);
                SoundInstance * ptSound = SoundTryToGet(dataID);
                if (ptSound)
                    ptSound->SetParent(tParentData);
            }
        }
        break;
#line 338
    case ATTACHMENTTYPE_SHAKE:
        {



            if (ParentIsRActor(msg.nParentType))
            {
                RActorID idRActorTopmost = RActorGetTopmostOwner((RActorID)msg.idParent, FALSE, FALSE);
                const RActor * ptRActorOwner = RActorTryToGet(idRActorTopmost);
                if (ptRActorOwner)
                {
                    ACDID idACDPlayer = ACDID_INVALID;  // dead initialiser (no row); the original re-reads ptRActorOwner->idACD after every call
                    if (ACDIsPlayer(ptRActorOwner->idACD))
                    {
                        idACDPlayer = ptRActorOwner->idACD;
                    }
                    else if (ACDIsPet(ptRActorOwner->idACD))
                    {
                        idACDPlayer = ACDGetPetOwner(ptRActorOwner->idACD);
                    }
                    else
                    {
                        ActorCommonData * ptACDOwner = ACDTryToGet(ptRActorOwner->idACD);
                        if (ptACDOwner)
                        {
                            ACDNetworkName idANNEffectOwner = (ACDNetworkName)ptACDOwner->AttributesGetInt((FastAttribKey)0xfffff507);
                            if (idANNEffectOwner != ANN_INVALID)
                            {
                                idACDPlayer = ACDFromANN(idANNEffectOwner);
                            }
                        }
                    }

                    if (ACDTryToGet(idACDPlayer))
                    {
                        if (!PlayerIsLocal(idACDPlayer))
                            return DATAID_INVALID;
                    }
                }
            }




            dataID = ShakeAdd(msg.tTriggerEvent.snoname.snoHandle, transform, eContext);
            ShakeSetParent(dataID, tParentData);
        }
        break;
    case ATTACHMENTTYPE_VIBRATION:
        {


#line 390
            const VibrationDefinition * pdefVibration = (const VibrationDefinition *)gptGroupDefVibration->LockRFast(msg.tTriggerEvent.snoname.snoHandle);  // no DefinitionLockR inline in the DWARF: LockRFast is called at 390:60
#line 393
            int32 nPlayerIndex = 0;
#line 391
            bool bAllPlayers = (pdefVibration->dwFlags & (1 << VIBRATION_DEF_ALL_PLAYERS)) != 0;
            if (bAllPlayers)
            {
                for (nPlayerIndex = 0; nPlayerIndex < 4; ++nPlayerIndex)
                    if (PlayerIsLocal(nPlayerIndex))
                        dataID = Vibration::Add(msg.tTriggerEvent.snoname.snoHandle, transform, nPlayerIndex);
            }
            else if (tParentData.HasLocalPlayerAsParent())
            {
                nPlayerIndex = tParentData.GetLocalParentPlayerIndex();
                dataID = Vibration::Add(msg.tTriggerEvent.snoname.snoHandle, transform, nPlayerIndex);
            }
            Vibration::SetParent(dataID, tParentData);
        }
        break;
    case ATTACHMENTTYPE_AMBIENTSOUND:
        {
            dataID = AmbientSound::Add(msg.tTriggerEvent.snoname.snoHandle, transform, eContext, 0.0f, FALSE);
#line 419
            AmbientSound * ptAmbientSound = AmbientSoundTryToGet(dataID);
            if (ptAmbientSound)
                ptAmbientSound->SetParent(tParentData);
        }
        break;
    case ATTACHMENTTYPE_EXPLOSION:
        {
#line 429
            ExplosionManager * ptManager = CGameGet().ptExplosionManager;
            dataID = ptManager->Create(msg.tTriggerEvent.snoname.snoHandle, transform, eContext);
            if (dataID != DATAID_INVALID)
                ptManager->SetParent(dataID, tParentData);
        }
        break;
    case ATTACHMENTTYPE_SCENE:
        {
#line 442
            const RActor * ptRActor = RActorGet((RActorID)msg.idParent);
#line 443
            const WorldLocation & wl = ptRActor->tLocation.tWorldLocation;
#line 445
            SceneSpecification tSceneSpec;
            SceneSpecInitialize(tSceneSpec);
#line 448
            SWorldID idSWorld = CGameGetCurrentSWorldID();
            dataID = CSceneAdd(msg.tTriggerEvent.snoname.snoHandle, SSCENEID_INVALID, tSceneSpec, idSWorld, wl.transform, SSCENEID_INVALID, SNO_NONE, TRUE, FALSE, eContext, NULL, 0);
            CSceneSetParent(dataID, tParentData);
        }
        break;
    }
    return dataID;
}
