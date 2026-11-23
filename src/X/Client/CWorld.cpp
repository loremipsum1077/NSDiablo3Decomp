// Original file: X/Client/CWorld.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).



#include "CWorld.h"
#include "CGame.h"
#include "CMarker.h"
#include "CPlayer.h"
#include "Lights.h"
#include "Rope.h"
#include "RWindow.h"
#include "Selection.h"
#include "Shake.h"
#include "SNOManager.h"
#include "SNOPreload.h"
#include "Vibration.h"
#include "Weather.h"
#include "RActors/RActors.h"
#include "Wind.h"
#include "Camera/CameraManager.h"
#include "Definitions/WeatherDefinition.h"
#include "UserInterface/UI.h"
#include "UserInterface/Console/ConsoleScreenManager.h"
#include "UserInterface/Interface/UIConversations.h"
#include "UserInterface/Interface/UIGame.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/Gizmo.h"
#include "../Common/GizmoSwig.h"
#include "../Common/World.h"
#include "../Common/WorldPlace.h"
#include "../Common/Definitions/MarkerSetDefinition.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Random.h"
#include "Scene.h"
#include "Sound/AmbientSound.h"
#include "../Common/GlobalSNO.h"
#include "../Common/SceneCommon.h"
#include "../Common/Definitions/LevelAreaDefinition.h"
#include "../Common/Marker/Marker.h"

// Functions of other files that no header in src declares yet (file:line from the DWARF).
void GfxEvictResources();  // X/Client/Gfx/Gfx.cpp:3526
void ParticlesInitializeForWorld();  // X/Client/Particle.cpp:581
void ParticlesDisposeForWorld();  // X/Client/Particle.cpp:634
void CPlayerInputInitializeForWorld(Player * ptPlayer);  // X/Client/CPlayerInput.cpp:308
void CPlayerInputDisposeForWorld();  // X/Client/CPlayerInput.cpp:316
void PlayersSetMainPlayerRActor(Player * ptPlayer, RActorID idRActor);  // X/Client/CPlayerInput.cpp:6776
void PostFXInitializeForWorld();  // X/Client/PostFX.cpp:854
void MarkersCreateInstancesFromWorld(SWorldID idSWorld, SSceneID idSScene);  // X/Common/Marker/Marker.cpp:2685
void CGameModeInitialize(const GameMode eMode);  // X/Client/CGameMode.cpp:687
void CGameModeDispose();  // X/Client/CGameMode.cpp:74
void WorldCacheSceneValues(const WorldDefinition * pDefWorld);  // X/Common/World.cpp:240
void WorldCommonDataInitialize(WorldCommonData & tWCD, SNO snoWorld, SWorldID idSWorld, int32 nOriginX, int32 nOriginY, int32 nStitchSizeInFeetX, int32 nStitchSizeInFeetY, int32 nWorldSizeInFeetX, int32 nWorldSizeInFeetY, SNO snoDungeonFinderSourceWorld);  // X/Common/World.cpp:94
void WorldCommonDataInitializePhysics(WorldCommonData & tWCD);  // X/Common/World.cpp:162
void WorldCommonDataDispose(WorldCommonData & tWCD);  // X/Common/World.cpp:184
void CScenesInitializeForWorld(SWorldID idSWorld);  // X/Client/Scene.cpp:295
void CScenesDisposeForWorld(SWorldID idSWorld);  // X/Client/Scene.cpp:313
BOOL DRLGGenerate(SWorldID idSWorld);  // X/Common/DRLG.cpp:116
void SceneCommonDeactivateInWorld(SWorldID idSWorld);  // X/Common/SceneCommon.cpp:2635
void LightsDeleteAll(RenderContext eContext);  // X/Client/Lights.cpp:433
namespace AmbientSounds {
void DeleteAll(const RenderContext eContext);  // X/Client/Sound/AmbientSound.cpp:839
}
void RActorsDeleteAll();  // X/Client/RActors/RActors.cpp:645
SNO RActorGetLevelArea(const RActorID idRActor);  // X/Client/RActors/RActors.cpp:11600
void CinematographyClearOrbit(BOOL bForce);  // X/Client/Camera/Cinematography.cpp:337
void CinematographySetObserver(const SNO snoObserver);  // X/Client/Camera/Cinematography.cpp:290
void CinematographySetZoom(const float flZoom, const float flZoomSpeed);  // X/Client/Camera/Cinematography.cpp:184
void ActorCollisionInitializeForGame();  // X/Common/Actors/ActorCollision.cpp:50
void ActorCollisionDisposeForGame();  // X/Common/Actors/ActorCollision.cpp:65
void ACDCollInitializeForGame();  // X/Common/ActorCommonCollision.cpp:49
void ACDCollDisposeForGame();  // X/Common/ActorCommonCollision.cpp:62
void NavMeshInitializeForGame();  // X/Common/Nav/NavigationMesh.cpp:158
void NavMeshDisposeForGame();  // X/Common/Nav/NavigationMesh.cpp:182
BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode);  // X/Client/Scene.cpp:3538
SSceneID RActorGetSSceneID(const RActorID idRActor);  // X/Client/RActors/RActors.cpp:11585
void EffectGroupCreateUntracked(const SNO snoEffectGroup, const RActorID idRActorSource, const uint32 dwAttachmentID, const BOOL bIgnoreOwnerAlpha, const BOOL bRemote);  // X/Client/EffectGroup.cpp:1299
namespace SoundManager {
void PlayMusic(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle);  // X/Client/Sound/Sound.cpp:2712
void SetReverbParams(const SNO snoReverb);  // X/Client/Sound/Sound.cpp
}


void CWorldEnter(
    SWorldID idSWorld,
    GameMode eMode)
{
    GameCommonData & tGCD = CGameGetGameCommonData();
    tGCD.GameModeSet(eMode);





    GfxEvictResources();

    CWorld * ptCWorld = CWorldGetFromSWorldID(idSWorld);
    CWorldID idCWorld = ptCWorld->id;

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    tCGameGlobals.snoCurrentWorld = ptCWorld->tWCD.snoWorld;
    tCGameGlobals.idCurrentCWorld = idCWorld;
    tCGameGlobals.idCurrentSWorld = idSWorld;


    CameraManagerInitializeForWorld();






    SNOManager::OnLoadingScreenCanStart();


    SNOPreload::SetClientWorld(ptCWorld->tWCD.snoWorld);

    Weather::InitializeForWorld(GetRWindowMgr()->GetRWindow(0));
    Wind::InitializeForWorld();
    ParticlesInitializeForWorld();
    SelectionInitializeForWorld();
    CPlayerInputInitializeForWorld(LocalPlayerGet());

    WorldCommonData & tWCD = WorldGetCommonData(CGameGetCurrentSWorldID());
    {
        ScopedWorldDefinitionLockR ptDefWorld(tWCD.snoWorld);
        const Environment * ptEnvironment = &ptDefWorld->tEnvironment;

        ptCWorld->snoIrradianceTex = ptEnvironment->snoIrradianceTex;
        ptCWorld->snoIrradianceTexDead = ptEnvironment->snoIrradianceTexDead;
        ptCWorld->flDeformationScale = ptDefWorld->flDeformationScale;
    }


    WorldCommonDataInitializePhysics(tWCD);


    PostFXInitializeForWorld();
    UI_InitializeForWorld();

    MarkersCreateInstancesFromWorld(idSWorld, -1);

    CGameModeInitialize(eMode);

    CWorldGlobals * ptCWorldGlobals = CGameGet().ptCWorldGlobals;
    ptCWorldGlobals->tAudioVolumes.clear();

    ScopedWorldDefinitionLockR pdefWorld(tWCD.snoWorld);
    if (CGameGetClientTime() >= ptCWorld->flIntroMusicNextTime)
    {
        ptCWorldGlobals->snoIntroMusic = pdefWorld->tEnvironment.snoIntroMusic;
        if (ptCWorldGlobals->snoIntroMusic != SNO_NONE)
        {
            float flCooldownDelta = Math::Max(0.0f, pdefWorld->tEnvironment.flIntroMusicCooldownSecsMax - pdefWorld->tEnvironment.flIntroMusicCooldownSecsMin);
            flCooldownDelta *= RandFloat_Internal(CGameGlobalsGet().rngLocal);
            float flCooldownSecs = pdefWorld->tEnvironment.flIntroMusicCooldownSecsMin + flCooldownDelta;
            float flCooldownTime =
                Math::Max(flCooldownSecs, 0.0f);
            ptCWorld->flIntroMusicNextTime = CGameGetClientTime() + flCooldownTime;
        }
    }











}




CWorldID ClientWorldCreate(
    SNO snoWorld,
    SWorldID idSWorld,
    GameMode eMode,
    int32 nOriginX,
    int32 nOriginY,
    int32 nStitchSizeInFeetX,
    int32 nStitchSizeInFeetY,
    int32 nWorldSizeInFeetX,
    int32 nWorldSizeInFeetY,
    SNO snoDungeonFinderSourceWorld)
{

    GameCommonData & tGCD = CGameGetGameCommonData();













    tGCD.GameModeSet(eMode);





































    CGame & tCGame = CGameGet();

    CWorldID idCWorld = tCGame.ptCWorlds->ElementNew();

    CWorld * ptCWorld = CWorldGet(idCWorld);




    SNOPreload::SetClientWorld(snoWorld);


    if (idSWorld == SWORLDID_INVALID)
    {

        SNOPreload::WaitForFullClientWorld(snoWorld); idSWorld = idCWorld;
    }





    {
        ScopedWorldDefinitionLockR pdefWorld(snoWorld);
        WorldCacheSceneValues(pdefWorld);
    }

    WorldCommonDataInitialize(ptCWorld->tWCD, snoWorld, idSWorld, nOriginX, nOriginY, nStitchSizeInFeetX, nStitchSizeInFeetY, nWorldSizeInFeetX, nWorldSizeInFeetY, snoDungeonFinderSourceWorld);

    CScenesInitializeForWorld(idSWorld);

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    if (eMode != GAMEMODE_PLAYING && eMode != GAMEMODE_EDITSCENE)
    {



        tCGameGlobals.snoCurrentWorld = snoWorld;
        tCGameGlobals.idCurrentCWorld = idCWorld;

        tCGameGlobals.idCurrentSWorld = idSWorld;

        DRLGGenerate(idSWorld);
    }

    if (eMode != GAMEMODE_PLAYING)
    {
        CWorldEnter(idSWorld, eMode);

        tCGameGlobals.bCanRunIdleLoop = TRUE;
    }

    ptCWorld->flIntroMusicNextTime = 0.0f;

    return idCWorld;
}




void CWorldLeave()

{

    CGameAsyncRenderFlush();
    CGameAsyncRenderGPUFlush();


    CGameModeDispose();

    SWorldID idSWorld = CGameGetCurrentSWorldID();




    SceneCommonDeactivateInWorld(idSWorld);

    UI_DisposeForWorld();


    LightsDeleteAll(RENDERCONTEXT_DEFAULT);

    RopesDeleteAll();
    ShakeDeleteAll(RENDERCONTEXT_DEFAULT);
    AmbientSounds::DeleteAll(RENDERCONTEXT_DEFAULT);
    UIConversation::OnWorldLeave();
    MarkersDeleteForWorld(idSWorld);
    RActorsDeleteAll();
    CPlayerInputDisposeForWorld();
    SelectionDisposeForWorld();
    ParticlesDisposeForWorld();
    CameraManagerDisposeForWorld();
    Weather::DisposeForWorld(GetRWindowMgr()->GetRWindow(0));
    Wind::DisposeForWorld();


    Vibration::DeleteAll();
    CWorld * ptCWorld = CWorldGetFromSWorldID(idSWorld);
    WorldCommonDataInitializePhysics(ptCWorld->tWCD);



    CinematographyClearOrbit(FALSE);
    CinematographySetObserver(CameraManagerGetDefaultObserver(FALSE));


    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    tCGameGlobals.snoCurrentWorld = SNO_NONE;
    tCGameGlobals.idCurrentCWorld = CWORLDID_INVALID;
    tCGameGlobals.idCurrentSWorld = SWORLDID_INVALID;

    CWorldGlobals * ptCWorldGlobals = CGameGet().ptCWorldGlobals;
    ptCWorldGlobals->snoCurrentAmbient = INVALID_SNO;
    ptCWorldGlobals->snoCurrentReverb = INVALID_SNO;




}




void ClientWorldDispose(CWorldID idCWorld)

{
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    CGame & tCGame = CGameGet();

    CWorld * ptCWorld = CWorldGet(idCWorld);

    if (SNOPreload::GetClientWorld() == ptCWorld->tWCD.snoWorld)
    {
        SNOPreload::SetClientWorld(SNO_NONE);
    }


    if (tCGameGlobals.idCurrentSWorld == ptCWorld->tWCD.idSWorld)
    {
        CWorldLeave();
    }

    CScenesDisposeForWorld(ptCWorld->tWCD.idSWorld);

    WorldCommonDataDispose(ptCWorld->tWCD);

    tCGame.ptCWorlds->ElementDeleteByID(idCWorld);


}












































































































































































































































































































































































































































































































































































































































































CWorld * CWorldGetFromSWorldID(SWorldID idSWorld)
{

    CGame & tCGame = CGameGet();



    TDataArrayIterator<CWorld> tIt;
    while (tCGame.ptCWorlds->Iterate(tIt))
    {
        CWorld * ptCWorld = tIt.ptElement;
        if (ptCWorld->tWCD.idSWorld == idSWorld)
        {
            return ptCWorld;
        }
    }


    return NULL;
}



SNO WorldGetLevelAreaSNOFromRActor(RActorID idRActor)
{

    return RActorGetLevelArea(idRActor);
}




WorldPlace ClientPointToPlace(const Vector3D & wp)

{
    WorldPlace tPlace;
    tPlace.wp = wp;
    tPlace.idSWorld = CGameGetCurrentSWorldID();
    return tPlace;
}




static void sResolveLinkToGizmo(AudioVolume & tVolume)
{

    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return;




    TDataArrayIterator<RActor> tIt;
    while (RActor * ptRActor = tCGame.ptRActors->Iterate(tIt))
    {





        if (ptRActor->tMinimapInfo.snoMarkerSet != SNO_NONE)
        {
            ScopedDefinitionLockR<MarkerSetDefinition> pMarkerSet(ptRActor->tMinimapInfo.snoMarkerSet);
            if (ptRActor->tMinimapInfo.nMarkerIndex >= 0 && ptRActor->tMinimapInfo.nMarkerIndex < pMarkerSet->tMarkerSet.GetArraySize())
            {




                const Marker & tMarker = pMarkerSet->tMarkerSet[ptRActor->tMinimapInfo.nMarkerIndex];

                if (tVolume.tGizmoMarkerName == tMarker.dwHash)
                {
                    tVolume.idACDGizmoParent = ptRActor->idACD;
                    return;
                }
            }
        }
    }
}




const AudioVolume * CWorldGetAudioVolumeForPlayerPosition()

{
    RActorID idPlayerRActor = LocalPlayerRActorGet();
    const RActor * ptRActor = RActorTryToGet(idPlayerRActor);
    if (ptRActor)
    {
        const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;

        CGame & tCGame = CGameGet();
        CWorldGlobals * ptCWorldGlobals = tCGame.ptCWorldGlobals;

        for (uint32 i = 0; i != ptCWorldGlobals->tAudioVolumes.size(); i++)
        {
            AudioVolume & tVolume = ptCWorldGlobals->tAudioVolumes[i];

            if (tVolume.fLinkedToGizmo)
            {
                ActorCommonData * ptACDGizmo = ACDTryToGet(tVolume.idACDGizmoParent);
                if (!ptACDGizmo)
                {
                    if (tCGame.flClientTime - tVolume.flLastResolveTime > 1.0f)
                    {
                        sResolveLinkToGizmo(tVolume);
                        ptACDGizmo = ACDTryToGet(tVolume.idACDGizmoParent);
                        tVolume.flLastResolveTime = tCGame.flClientTime;
                    }
                }
                if (ptACDGizmo)
                {
                    GizmoType nGizmoType = ptACDGizmo->GetGizmoType();
                    if (nGizmoType == ACTORGIZMO_CHEST)
                        if (GizmoChestIsOpen(*ptACDGizmo))
                            continue;
                }
            }



            switch (tVolume.shape.eType)
            {
            case Shape::SPHERE:

                if (tVolume.shape.sphere.ContainsPoint(loc.transform.wp))
                    return &tVolume;
                break;




            case Shape::BOX:

                if (tVolume.shape.box.ContainsPoint(loc.transform.wp))
                    return &tVolume;
                break;
            }
        }
    }








    return NULL;
}



static BOOL sUseRetroMusic()
{

    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (!ptACDPlayer)
        return FALSE;



    SNO snoLevelArea = ACDGetLevelArea(*ptACDPlayer);
    if (snoLevelArea == SNO_NONE)
        return FALSE;



    ScopedDefinitionLockR<LevelAreaDefinition> pdefLevelArea(snoLevelArea);
    if (!(pdefLevelArea->dwFlags & (1 << LEVEL_AREA_FLAG_TOWN)))
        return FALSE;



    return (GameGetActiveGameCommonData().tGameSyncedData.dwGameSyncedFlags & (1 << GAME_SYNCED_FLAGS_ANNIVERSARY_DUNGEON)) != 0;





}




static void sGetAudioSettingsForPosition(const SceneSpecification & tSceneSpec, AudioSettings & tSettings)
{



    tSettings.snoMusic = tSceneSpec.snoMusic;
    tSettings.snoAmbient = tSceneSpec.snoAmbient;
    tSettings.snoReverb = tSceneSpec.snoReverb;


    const AudioVolume * ptAudioVolume = CWorldGetAudioVolumeForPlayerPosition();
    if (ptAudioVolume)
    {

        if (ptAudioVolume->tSettings.snoReverb != SNO_NONE)
            tSettings.snoReverb = ptAudioVolume->tSettings.snoReverb;


        if (ptAudioVolume->tSettings.snoMusic != SNO_NONE)
            tSettings.snoMusic = ptAudioVolume->tSettings.snoMusic;


        if (ptAudioVolume->tSettings.snoAmbient != SNO_NONE)
            tSettings.snoAmbient = ptAudioVolume->tSettings.snoAmbient;
    }



    if (sUseRetroMusic())
    {
        tSettings.snoMusic = GlobalSNOGet(GLOBALSNO_MUSIC_RETRO);
    }


    static const AudioVolume * ptAudioVolumePrev = NULL;
    if (ptAudioVolumePrev != ptAudioVolume)
    {
        if (ptAudioVolume)
        {
            EffectGroupCreateUntracked(ptAudioVolume->snoOnEntryEffectGroup, LocalPlayerRActorGet(), 0, FALSE, FALSE);
        }
        ptAudioVolumePrev = ptAudioVolume;
    }
}




void CWorldUpdate()

{
    CGame & tCGame = CGameGet();
    SceneCommonData * ptSCD = NULL;
    if (tCGame.tGCD.GameModeGet() == GAMEMODE_MENU)
    {




        CScene * ptCScene;
        TDataArrayIterator<CScene> tIterator;
        while (CSceneDataArrayIterate(tIterator, CGameGet(), SkipHolodeckScenes))
        {
            ptCScene = tIterator.ptElement;
            ptSCD = &ptCScene->tSCD;
        }
    }
    else
    {
        SSceneID idSScene = RActorGetSSceneID(LocalPlayerRActorGet());
        ptSCD = SceneCommonDataTryToGet(idSScene);
    }

    if (!ptSCD)
        return;



    AudioSettings tAudio;
    sGetAudioSettingsForPosition(ptSCD->tSceneSpec, tAudio);

    CWorldGlobals * ptCWorldGlobals = tCGame.ptCWorldGlobals;

    SNO snoSceneMusic = tAudio.snoMusic;
    ptCWorldGlobals->snoCurrentMusic = snoSceneMusic;
    if (!ptCWorldGlobals->fMusicDisabled)
    {
        SoundManager::PlayMusic(snoSceneMusic, 0, FALSE);

        if (ptCWorldGlobals->snoIntroMusic != SNO_NONE)
        {
            SoundManager::PlayMusic(ptCWorldGlobals->snoIntroMusic, 0, FALSE);
            ptCWorldGlobals->snoIntroMusic = SNO_NONE;
        }
    }

    if (tAudio.snoAmbient != ptCWorldGlobals->snoCurrentAmbient)
    {
        ptCWorldGlobals->snoCurrentAmbient = tAudio.snoAmbient;

        if (tAudio.snoAmbient != SNO_NONE)
        {
            AmbientSound * ptAmbientSound = AmbientSoundTryToGet(ptCWorldGlobals->idAmbient);
            if (!ptAmbientSound)
            {
                ptCWorldGlobals->idAmbient = AmbientSound::Add(tAudio.snoAmbient, cgtPRTransformIdentity, RENDERCONTEXT_DEFAULT, 0.0f, TRUE);
            }
            else
            {
                ptAmbientSound->SwitchGlobalAmbient(tAudio.snoAmbient);
            }
        }
        else
        {
            AmbientSound * ptAmbientSound = AmbientSoundTryToGet(ptCWorldGlobals->idAmbient);
            if (ptAmbientSound)
            {
                ptAmbientSound->Delete();
            }
            ptCWorldGlobals->idAmbient = AMBIENTSOUNDID_INVALID;
        }
    }

    if (tAudio.snoReverb != ptCWorldGlobals->snoCurrentReverb)
    {
        ptCWorldGlobals->snoCurrentReverb = tAudio.snoReverb;
        SoundManager::SetReverbParams(tAudio.snoReverb);
    }
}




void CWorldDisableAreaMusic(const BOOL fDisabled)
{

    if (CGameIsValid())
    {
        CWorldGlobals * ptCWorldGlobals = CGameGet().ptCWorldGlobals;
        if (ptCWorldGlobals)
        {
            ptCWorldGlobals->fMusicDisabled = fDisabled;
        }
    }
}




BOOL CWorldIsRetro()

{
    RWindowMgr * ptRWindowMgr = GetRWindowMgr();
    if (!ptRWindowMgr)
    {
        return FALSE;
    }

    ColorCorrectionParams * ptParams;
    ColorCorrectionParams * ptParamsPrev;
    ColorCorrectionParams * ptParamsPrev2;
    Weather::GetColorCorrection(ptRWindowMgr->GetCurrentRWindow().GetWeatherRWindowData(), &ptParams, &ptParamsPrev, &ptParamsPrev2);

    return ptParams->fRetro != (sgtAppGlobals.eDebugRenderMode == DRM_RETRO);
}




BOOL CWorldPhysicsAreRetro()

{







    if (Console::Screens::IsScreenSetOpen())
    {
        return FALSE;
    }



    return CWorldIsRetro();
}




BOOL CWorldIsRetroForLighting()

{
    if (sgtAppGlobals.dwDebugFlags3 & (1 << APP_GLOBAL3_RETRO_LIGHTING_DISABLED))
    {
        return !CWorldIsRetro();
    }

    return CWorldIsRetro();
}




void CWorldResetForRetro()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    GameMode eGameMode = tGCD.GameModeGet();
    if (eGameMode == GAMEMODE_PLAYING)
    {
        CinematographySetObserver(CameraManagerGetDefaultObserver(FALSE));
        CameraManagerResetCameras();

        if (CWorldIsRetro())
        {
            CinematographySetZoom(0.0f, 1.0f);
        }


        UIGame::InitializeForPlayer();
    }
}




void CWorldChange(SWorldID idSWorld)

{
    PlayersSetMainPlayerRActor(LocalPlayerGet(), RACTORID_INVALID);
    CPlayerMainRActorTeleported();

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    if (tCGameGlobals.idCurrentSWorld != SWORLDID_INVALID)
    {
        CWorldLeave();
    }

    CWorldEnter(idSWorld, GAMEMODE_PLAYING);
}



SWorldID CWorldGetCurrentWorld()

{
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    return tCGameGlobals.idCurrentSWorld;
}



void CWorldSetStatus(
    const SWorldID idSWorld,
    const uint32 dwWorldStatusBits)
{
    WorldCommonData & tWCD = WorldGetCommonData(idSWorld);


    tWCD.dwWorldStatusBits = dwWorldStatusBits;







}




void CWorldsInitializeForGame()

{
    ActorCollisionInitializeForGame();
    ACDCollInitializeForGame();
    NavMeshInitializeForGame();

    CGame & tCGame = CGameGet();
    tCGame.ptCWorlds = TDataArrayCreateTracked<CWorld, CWorldID>("CWorlds", AppGetCurrentMemoryInterface(), 256);

    CWorldGlobals *& ptCWorldGlobals = tCGame.ptCWorldGlobals;
    ptCWorldGlobals = (CWorldGlobals *)SigmaMemoryNew(sizeof(CWorldGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);
    ptCWorldGlobals->fMusicDisabled = FALSE;
    ptCWorldGlobals->snoCurrentMusic = SNO_NONE;
    ptCWorldGlobals->snoIntroMusic = SNO_NONE;
    ptCWorldGlobals->snoCurrentAmbient = INVALID_SNO;
    ptCWorldGlobals->snoCurrentReverb = INVALID_SNO;
    ptCWorldGlobals->idAmbient = -1;

    new (&ptCWorldGlobals->tAudioVolumes) AudioVolumeArray();
}




void CWorldsDisposeForGame()

{
    CGame & tCGame = CGameGet();
    TDataArrayIterator<CWorld> tIt;
    while (tCGame.ptCWorlds->Iterate(tIt))
    {
        ClientWorldDispose(tIt->id);
    }

    TDataArrayDispose(tCGame.ptCWorlds);

    CWorldGlobals *& ptCWorldGlobals = tCGame.ptCWorldGlobals;
    ptCWorldGlobals->tAudioVolumes.~AudioVolumeArray();

    SigmaMemoryFree(ptCWorldGlobals, AppGetCurrentMemoryInterface()); ptCWorldGlobals = NULL;

    ActorCollisionDisposeForGame();
    ACDCollDisposeForGame();
    NavMeshDisposeForGame();
}













void CWorldAddAudioVolume(const Shape & shape, const Marker & tMarker, const AudioSettings & tSettings, const SNO snoOnEntryEffectGroup)




{
    CWorldGlobals * ptCWorldGlobals = CGameGet().ptCWorldGlobals;
    if (ptCWorldGlobals)
    {
        AudioVolume tVolume;
        tVolume.shape = shape;
        tVolume.tSettings = tSettings;
        tVolume.tMarkerName = tMarker.dwHash;
        tVolume.snoOnEntryEffectGroup = snoOnEntryEffectGroup;
        tVolume.flLastResolveTime = 0.0f;

        tVolume.fLinkedToGizmo = FALSE;
        if (tMarker.nLinkCount)
        {
            const MarkerLink & tLink = tMarker.ptMarkerLinks[0];
            tVolume.fLinkedToGizmo = TRUE;
            tVolume.tGizmoMarkerName = tLink.dwHash;
        }

        ptCWorldGlobals->tAudioVolumes.push_back(tVolume);
    }
}

