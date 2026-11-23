// Original file: X/Client/Sound/AmbientSound.cpp
#include "../RActors/RActors.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "AmbientSound.h"
#include "Sound.h"
#include "../CGame.h"
#include "../CPlayer.h"
#include "../RWindow.h"
#include "../Definitions/AmbientSoundDefinition.h"
#include "../Definitions/SoundDefinition.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Game.h"
#include "../../Common/InactiveMarker.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/Cylinder.h"
#include "../../Sigma/Math/Frustum.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/Random.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.

namespace SoundManager { BOOL IsInitialized(); void PlayBkgrndAmbient(SNO snoAmbient, BOOL, float, BOOL); } void SoundFadeOut(const SoundID idSound);  // X/Client/Sound/Sound.cpp (PlayBkgrndAmbient's parameter names are not looked up)
void InactiveMarkerCreateFromWorldMarkerSet(SWorldID idSWorld, const InactiveMarkerOwner & tOwner, const PRTransform & transform, const Sphere & wsBounds);  // X/Common/InactiveMarker.cpp
typedef int32 CutsceneID; CutsceneID CutsceneGetFullScreenCutscene(); RWindowID RWindowIDFromContext(const RenderContext eContext); const AudioVolume * CWorldGetAudioVolumeForPlayerPosition();  // X/Client/Cutscene.cpp, X/Client/CWorld.cpp
namespace AmbientSounds { void InitializeForGame(); void DisposeForGame(); void SetLastSeenTime(Frustum & tFrustum); void Update(); void CheckSeenRecently(); void DeactivateInScene(SSceneID idSScene); void DeleteAll(const RenderContext eContext); } class Constraint; Constraint * RActorPhysicsGetConstraint(const RActor * ptRActor, uint32 dwConstraintHash);  // X/Client/RActors/RActorPhysics.cpp:385

enum {
    MAX_AMBIENTSOUND_INSTANCES = 256,
};

#include "../../Sigma/Math/Shape.h"
#include "../CWorld.h"
#include "../../Common/Definitions/QuestRangeDefinition.h"
// AudioVolume now comes from X/Client/CWorld.h (it was defined locally on this line).
#include "../Attachments.h"
#include "../Physics/DominoClient.h"






static const float TIME_BETWEEN_RETRY = 0.3f;









void AmbientSound::SetParent(const ParentData & tParentData)
{
    m_tParentData = tParentData;
}





void AmbientSound::UpdatePropertiesFromParent(const float flDeltaTime)
{

    if (m_tParentData.ParentIsValid())
    {



        PRTransform transform;
        m_tParentData.GetTransform(transform);

        SetPosition(transform, flDeltaTime);
    }
}



BOOL AmbientSound::sGetVelocityFromCloth(float & flVel) const
{

    Cloth * ptCloth = ClothManager::TryToGet(m_idCloth);
    if (!ptCloth)
    {
        return FALSE;
    }

    flVel = ptCloth->GetSoundData();
    return TRUE;
}





BOOL AmbientSound::sGetVelocityFromConstraint(uint32 dwConstraintHash, float & flVel) const
{







    if (!ParentIsRActor(m_tParentData.m_eParentType))
    {
        return FALSE;
    }

    RActor * ptRActor = RActorTryToGet((RActorID)m_tParentData.m_idParent);
    if (!ptRActor)
    {
        return FALSE;
    }


    Constraint * ptConstraint = RActorPhysicsGetConstraint(ptRActor, dwConstraintHash);



    if (!ptConstraint)
    {
        return FALSE;
    }

    flVel = ptConstraint->GetSoundData();
    return TRUE;
}



BOOL AmbientSound::sGetJointVelocitySoundParams(const AmbientSoundDefinition * pdefAmbient, float & flVolume, float & flPitch) const



{
    if (!(pdefAmbient->dwFlags & 1))
    {
        return FALSE;
    }
    float flVel;

    if (m_idCloth != CLOTHID_INVALID)
    {
        if (!sGetVelocityFromCloth(flVel))
        {
            return FALSE;
        }
    }
    else
    {

        if (!sGetVelocityFromConstraint(pdefAmbient->dwConstraintHash, flVel))
        {
            return FALSE;
        }
    }




    flVolume = 1.0f;
    if (pdefAmbient->flVolumeJointVelDelta > 1e-06f)
    {
        flVolume = Math::Pin((flVel - pdefAmbient->flVolumeJointVelMin * 60.0f) / (pdefAmbient->flVolumeJointVelDelta * 60.0f), 0.0f, 1.0f);
    }

    flPitch = 0.0f;
    if (flVolume > 0.0f)
    {
        if (pdefAmbient->flPitchJointVelDelta > 1e-06f)
        {
            flPitch = Math::Pin((flVel - pdefAmbient->flPitchJointVelMin * 60.0f) / (pdefAmbient->flPitchJointVelDelta * 60.0f), 0.0f, 1.0f);
        }
    }

    return TRUE;
}




Vector3D AmbientSound::sGetBackgroundSoundPosition() const
{

    if (m_flCapsuleLength > 1e-06f)
    {


        ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
        if (ptACDPlayer)
        {
            Capsule capsule;
            capsule.Set(m_transform, 0.0f, m_flCapsuleLength);

            LineSeg3D lineseg;
            lineseg.Set(capsule.wp1, capsule.wp2);

            Vector3D wpClosest;
            lineseg.ClosestPoint(ptACDPlayer->GetPlace().wp, wpClosest);
            return wpClosest;
        }
    }






    return Vector3D(m_transform.wp);
}





BOOL AmbientSound::sCanPlay(const AudioVolume * ptVolume)
{


    if (m_fLinkedToAudioVolume)
    {
        if (!ptVolume || m_tMarkerName != ptVolume->tMarkerName)
        {
            return FALSE;
        }
    }


    if (m_snoQuestRange != SNO_NONE && !QuestRangeEvaluate(m_snoQuestRange, NULL))
    {
        return FALSE;
    }

    return TRUE;
}





BOOL AmbientSound::sBackgroundSoundCanPlay() const
{









    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (ptACDPlayer)
    {
        Vector3D wp = sGetBackgroundSoundPosition();
        const Vector3D & wpPlayer = ptACDPlayer->GetPlace().wp;
        float flCutoffRadius = GetRadius(m_snoAmbient);
        if (Vector3D::DistanceSquared(wp, wpPlayer) < flCutoffRadius * flCutoffRadius)
        {
            return TRUE;
        }
    }

    return FALSE;
}




void AmbientSound::sDeleteBackgroundSound()
{

    if (m_idBackground != SOUNDID_INVALID)
    {
        SoundFadeOut(m_idBackground);
        m_idBackground = SOUNDID_INVALID;
        m_flLastPlayedTime = CGameGetClientTime();
    }
}




void AmbientSound::Update(const AudioVolume * ptVolume)

{
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    if (!sCanPlay(ptVolume))
    {
        sDeleteBackgroundSound();
        return;
    }

    float flClientTime = CGameGetClientTime();
    float flDeltaTime = CGameGetRealDeltaTime();
    float flPrevTime = tCGameGlobals.flLastLocalGameTimeUpdate;

    if (m_tParentData.m_eParentType != PARENT_NONE)
    {
        UpdatePropertiesFromParent(flDeltaTime);
    }
    else
    {


        if (m_flCapsuleLength > 1e-06f)
        {
            UpdateBackgroundSoundPosition(flDeltaTime);
        }
    }

    ScopedAmbientSoundDefinitionLockR pdefAmbient(m_snoAmbient);

    if (pdefAmbient->snoBackground != SNO_NONE)
    {

        if (!m_fGlobalAmbient)
        {

            if (!sBackgroundSoundCanPlay())
            {
                sDeleteBackgroundSound();
            }
            else
            {

                float flVolume = 1.0f, flPitchFactor = 0.0f;

                BOOL fAdjustForJointVelocity = sGetJointVelocitySoundParams(pdefAmbient, flVolume, flPitchFactor);

                SoundInstance * ptSoundInstance = SoundTryToGet(m_idBackground);

                if (!ptSoundInstance)
                {
                    if (m_flLastPlayedTime + TIME_BETWEEN_RETRY < flClientTime)
                    {
                        Vector3D wp = sGetBackgroundSoundPosition();

                        PlaySoundOptions tOptions;
                        tOptions.eContext = m_eContext;
                        tOptions.pwp = &wp;
                        tOptions.flInitialFadeAtten = flVolume;
                        m_idBackground = SoundManager::PlaySound(pdefAmbient->snoBackground, tOptions);
                        m_flLastPlayedTime = flClientTime;
                    }
                }

                if (fAdjustForJointVelocity)
                {
                    ptSoundInstance = SoundTryToGet(m_idBackground);
                    if (ptSoundInstance)
                    {
                        ptSoundInstance->SetControlledVolumeAtten(flVolume);
                        if (flVolume > 0.0f)
                        {
                            ptSoundInstance->SetPitchSemitones(flPitchFactor * pdefAmbient->flPitchSemitones);
                        }
                    }
                }
            }
        }
    }

    for (int32 i = 0; i < 2; i++)
    {

        if (m_flTimeUntilRandom[i] != -1.0f)
        {
            if (m_flTimeUntilRandom[i] != 0.0f)
            {
                m_flTimeUntilRandom[i] = Math::Max(m_flTimeUntilRandom[i] - flDeltaTime, 0.0f);
            }
            else
            {
                const RandomAmbientSoundParams & tParams = pdefAmbient->tRandomSounds[i];
                if (tParams.snoSound != SNO_NONE)
                {
                    Angle a = RandAngle(tCGameGlobals.rngLocal);
                    float flRadius = RandRangeFloat_Internal(tCGameGlobals.rngLocal, tParams.flRadiusMin, tParams.flRadiusDelta);
                    Vector3D wp(m_transform.wp);
                    if (m_fGlobalAmbient)
                    {
                        ACDID idACDLocalPlayer = LocalPlayerACDGet();
                        if (idACDLocalPlayer != ACDID_INVALID)
                        {
                            wp = ACDGet(idACDLocalPlayer).GetPlace().wp;
                        }
                    }
                    wp.x += flRadius * Math::Cos(a);
                    wp.y += flRadius * Math::Sin(a);

                    PlaySoundOptions tOptions;
                    tOptions.eContext = m_eContext;
                    tOptions.pwp = &wp;
                    SoundManager::PlaySound(tParams.snoSound, tOptions);
                }

                m_flTimeUntilRandom[i] = (float)RandRange_Internal(tCGameGlobals.rngLocal, tParams.dwPeriodMin, tParams.dwPeriodDelta) * (1.0f / 60.0f);
            }
        }
    }
    if (pdefAmbient->snoFixedIntervalSound != SNO_NONE
        && pdefAmbient->dwIntervalLength != 0)
    {
        float flPrevInterval = (flPrevTime - (float)pdefAmbient->dwIntervalOffset * (1.0f / 60.0f)) / ((float)pdefAmbient->dwIntervalLength * (1.0f / 60.0f));
        float flCurInterval = (flClientTime - (float)pdefAmbient->dwIntervalOffset * (1.0f / 60.0f)) / ((float)pdefAmbient->dwIntervalLength * (1.0f / 60.0f));
        if (flCurInterval - flPrevInterval > 0.0f)
        {
            PlaySoundOptions tOptions;
            tOptions.eContext = m_eContext;
            tOptions.pwp = &m_transform.wp;
            SoundManager::PlaySound(pdefAmbient->snoFixedIntervalSound, tOptions);
        }
    }
}




Sphere AmbientSound::sGetBoundingSphere() const
{

    float flCutoffRadius = GetRadius(m_snoAmbient) + m_flCapsuleLength;







    Sphere ws;
    ws.Set(m_transform.wp, flCutoffRadius);
    return ws;
}




void AmbientSound::SetLastSeenTime(const Frustum & tFrustum, const float flTime)
{


    Sphere ws = sGetBoundingSphere();
    BOOL bInside = Intersect::FrustumAndSphere(tFrustum, ws);
    if (bInside)
    {
        m_flLastSeen = flTime;
    }
}




void AmbientSound::sDeactivate()
{




    Sphere wsBounds = sGetBoundingSphere();

    SceneCommonData * ptSCD = SceneCommonDataTryToGet(m_idSScene);
    if (ptSCD)
    {
        InactiveMarkerCreateFromMarkerSet(*ptSCD, m_tMarkerOwner, m_transform, wsBounds);
    }
    else
    {
        if (m_idSWorld == SWORLDID_INVALID)
        {
            DisplayInternalError(NULL, "", 488, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }

        InactiveMarkerCreateFromWorldMarkerSet(m_idSWorld, m_tMarkerOwner, m_transform, wsBounds);
    }

    Delete();
}











void AmbientSound::CheckSeenRecently()
{

    if (m_tMarkerOwner.snoMarkerSet != SNO_NONE && m_idSScene != SSCENEID_INVALID)
    {









        float flNow = CGameGetClientTime();
        if (m_flLastSeen >= flNow)
        {
            return;
        }

        sDeactivate();
    }
}



void AmbientSound::DeactivateInScene(SSceneID idSScene)
{

    if (m_idSScene == idSScene && m_tMarkerOwner.snoMarkerSet != SNO_NONE)
    {
        sDeactivate();
    }
}












AmbientSoundID AmbientSound::Add(const SNO snoAmbient, const PRTransform & transform, const RenderContext eContext, const float flCapsuleLength, const BOOL fGlobalAmbient)





{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_NO_SOUND_BIT))
    {
        return AMBIENTSOUNDID_INVALID;
    }

    CGame & tCGame = CGameGet();
    AmbientSoundID id = tCGame.ptAmbientSounds->ElementNew();
    if (id == AMBIENTSOUNDID_INVALID)
    {
        return AMBIENTSOUNDID_INVALID;
    }

    AmbientSound * ptSound = AmbientSoundGet(id);
    ptSound->m_snoAmbient = snoAmbient;
    ptSound->m_transform = transform;
    ptSound->m_eContext = eContext;
    ptSound->m_flCapsuleLength = flCapsuleLength;

    ScopedAmbientSoundDefinitionLockR pdefAmbient(snoAmbient);


    if (fGlobalAmbient && RWindowIDFromContext(eContext) == RWindowMainWindow)
    {
        SoundManager::PlayBkgrndAmbient(pdefAmbient->snoBackground, FALSE, 2.0f, FALSE);
    }


    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    for (int32 i = 0; i < 2; i++)
    {
        const RandomAmbientSoundParams & tParams = pdefAmbient->tRandomSounds[i];

        uint32 dwTickUntilRandom = RandRange_Internal(tCGameGlobals.rngLocal, tParams.dwPeriodMin, tParams.dwPeriodDelta);
        ptSound->m_flTimeUntilRandom[i] = (dwTickUntilRandom == 0xffffffff) ? -1.0f : (float)dwTickUntilRandom * (1.0f / 60.0f);
    }

    float flClientTime = CGameGetClientTime();
    ptSound->m_flLastPlayedTime = flClientTime - TIME_BETWEEN_RETRY;
    ptSound->m_flLastSeen = flClientTime;
    ptSound->m_idSScene = SSCENEID_INVALID;

    ptSound->m_tMarkerOwner.Initialize();
    ptSound->m_fLinkedToAudioVolume = FALSE;
    ptSound->m_fGlobalAmbient = fGlobalAmbient;
    ptSound->m_tParentData.Init();
    ptSound->m_idCloth = -1;
    ptSound->m_snoQuestRange = SNO_NONE;

    return id;
}




void AmbientSound::SwitchGlobalAmbient(const SNO snoAmbient)
{

    m_snoAmbient = snoAmbient;

    ScopedAmbientSoundDefinitionLockR pdefAmbient(snoAmbient);

    if (RWindowIDFromContext(m_eContext) == RWindowMainWindow)
    {
        SoundManager::PlayBkgrndAmbient(pdefAmbient->snoBackground, FALSE, 2.0f, FALSE);
    }

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    for (int32 i = 0; i < 2; i++)
    {
        const RandomAmbientSoundParams & tParams = pdefAmbient->tRandomSounds[i];

        uint32 dwTickUntilRandom = RandRange_Internal(tCGameGlobals.rngLocal, tParams.dwPeriodMin, tParams.dwPeriodDelta);
        m_flTimeUntilRandom[i] = (dwTickUntilRandom == 0xffffffff) ? -1.0f : (float)dwTickUntilRandom * (1.0f / 60.0f);
    }

    m_flLastSeen = CGameGetClientTime();
}





void AmbientSound::Delete()
{

    if (m_fGlobalAmbient)
    {
        if (RWindowIDFromContext(m_eContext) == RWindowMainWindow)
        {
            SoundManager::PlayBkgrndAmbient(SNO_NONE, FALSE, 2.0f, FALSE);
        }
    }
    else
    {
        sDeleteBackgroundSound();
    }

    CGame & tCGame = CGameGet();
    tCGame.ptAmbientSounds->ElementDeleteByID(id);
}




void AmbientSound::UpdateBackgroundSoundPosition(const float flDeltaTime)
{

    if (!SoundManager::IsInitialized())
    {
        return;
    }

    SoundInstance * ptSoundInstance = SoundTryToGet(m_idBackground);
    if (ptSoundInstance)
    {
        Vector3D wp = sGetBackgroundSoundPosition();
        ptSoundInstance->SetPosition(wp, flDeltaTime);
    }
}




void AmbientSound::SetPosition(const PRTransform & transform, const float flDeltaTime)
{


    m_transform = transform;
    UpdateBackgroundSoundPosition(flDeltaTime);
}




















































void AmbientSounds::InitializeForGame()
{

    CGame & tCGame = CGameGet();
    tCGame.ptAmbientSounds = TDataArrayCreateTracked<AmbientSound, int>("Ambient Sounds", AppGetCurrentMemoryInterface(), MAX_AMBIENTSOUND_INSTANCES);
}




void AmbientSounds::DisposeForGame()
{

    DeleteAll(RENDERCONTEXT_INVALID);

    CGame & tCGame = CGameGet();
    if (tCGame.ptAmbientSounds)
    {
        TDataArrayDispose(tCGame.ptAmbientSounds);
    }
}




void AmbientSounds::SetLastSeenTime(Frustum & tFrustum)
{

    float flNow = CGameGetClientTime();

    CGame & tCGame = CGameGet();
    TDataArrayIterator<AmbientSound> tIt;
    while (tCGame.ptAmbientSounds->Iterate(tIt))
    {
        AmbientSound * ptSound = tIt.ptElement;

        ptSound->SetLastSeenTime(tFrustum, flNow);
    }
}




void AmbientSounds::Update()
{

    if (!SoundManager::IsInitialized())
    {
        return;
    }

    if (CutsceneGetFullScreenCutscene() != -1)
    {
        return;
    }
    const AudioVolume * ptVolume = CWorldGetAudioVolumeForPlayerPosition();
    CGame & tCGame = CGameGet();
    TDataArrayIterator<AmbientSound> tIt;
    while (tCGame.ptAmbientSounds->Iterate(tIt))
    {
        AmbientSound * ptSound = tIt.ptElement;

        ptSound->Update(ptVolume);
    }
}


void AmbientSounds::CheckSeenRecently()
{

    CGame & tCGame = CGameGet();
    TDataArrayIterator<AmbientSound> tIt;
    while (tCGame.ptAmbientSounds->Iterate(tIt))
    {
        AmbientSound * ptSound = tIt.ptElement;

        ptSound->CheckSeenRecently();
    }
}




void AmbientSounds::DeactivateInScene(SSceneID idSScene)
{

    CGame & tCGame = CGameGet();
    if (!tCGame.ptAmbientSounds)
    {
        return;
    }



    TDataArrayIterator<AmbientSound> tIt;
    while (tCGame.ptAmbientSounds->Iterate(tIt))
    {
        AmbientSound * ptSound = tIt.ptElement;
        ptSound->DeactivateInScene(idSScene);
    }
}




void AmbientSounds::DeleteAll(const RenderContext eContext)
{

    CGame & tCGame = CGameGet();
    if (!tCGame.ptAmbientSounds)
    {
        return;
    }

    TDataArrayIterator<AmbientSound> tIt;
    while (tCGame.ptAmbientSounds->Iterate(tIt))
    {
        AmbientSound * ptSound = tIt.ptElement;
        if (ptSound && ptSound->m_eContext == eContext)
        {
            ptSound->Delete();
        }
    }
}




















































float AmbientSound::GetRadius(SNO snoAmbient)
{
    if (snoAmbient == SNO_NONE)
    {
        return 0.0f;
    }
    ScopedAmbientSoundDefinitionLockR pdefAmbSound(snoAmbient);




    if (pdefAmbSound->snoBackground != SNO_NONE)
    {
        ScopedSoundDefinitionLockR pdefSound(pdefAmbSound->snoBackground);
        return pdefSound->wdFalloffRadiusMin + pdefSound->wdFalloffRadiusDelta;
    }
    for (int32 i = 0; i < 2; i++)
    {
        const RandomAmbientSoundParams & tParams = pdefAmbSound->tRandomSounds[i];
        if (tParams.snoSound != SNO_NONE)
        {
        return tParams.flRadiusMin + tParams.flRadiusDelta;
        }
    }
    if (pdefAmbSound->snoFixedIntervalSound != SNO_NONE)
    {
        ScopedSoundDefinitionLockR pdefSound(pdefAmbSound->snoFixedIntervalSound);
        return pdefSound->wdFalloffRadiusMin + pdefSound->wdFalloffRadiusDelta;
    }
    return 0.0f;
}
