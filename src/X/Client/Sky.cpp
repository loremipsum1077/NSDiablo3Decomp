// Original file: X/Client/Sky.cpp
//
// Reconstructed from the DWARF and the original disassembly. The functions live in namespace Sky
// and sit on their original lines (39, 51 and 61); the gaps are the original header and spacing.

#include "CGame.h"
#include "RWindow.h"
#include "RActors/RActors.h"
#include "Camera/CameraLocation.h"
#include "Definitions/WeatherDefinition.h"
#include "../Common/World.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Memory/SigmaMemory.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
// X/Client/Weather.h
namespace Weather {
SNO GetWeatherToUse(const RWindow & tRWindow);  // Weather.cpp:1189
}
// X/Client/Camera/CameraManager.h
const CameraState & CameraManagerGetCameraState(const int32 nCameraIndex);  // CameraManager.cpp:668

struct SkyGlobals {  // size 0x4
    RActorID idSkyboxActor;  // 0x00
};
static_assert(sizeof(SkyGlobals) == 0x4, "SkyGlobals");

namespace Sky {








void InitializeForGame() {
    SkyGlobals *& ptSkyGlobals = CGameGet().ptSkyGlobals;
    ptSkyGlobals = (SkyGlobals *)SigmaMemoryNew(sizeof(SkyGlobals), 4, NULL, TRUE);
    ptSkyGlobals->idSkyboxActor = RACTORID_INVALID;
}







void DisposeForGame() {
    SkyGlobals *& ptSkyGlobals = CGameGet().ptSkyGlobals;
    SigmaMemoryFree(ptSkyGlobals, NULL);
    ptSkyGlobals = NULL;
}





void Update() {
    SkyGlobals * ptSkyGlobals = CGameGet().ptSkyGlobals;
    SWorldID idSWorld = CGameGetCurrentSWorldID();
    if (idSWorld == SWORLDID_INVALID)
        return;
    const WorldCommonData & tWCD = WorldGetCommonData(idSWorld);
    ScopedWorldDefinitionLockR ptDefWorld(tWCD.snoWorld);
    const Environment * ptEnvironment = &ptDefWorld->tEnvironment;
    SNO snoSkyboxActor = ptEnvironment->snoSkyboxActor;
    if (tWCD.snoDungeonFinderSourceWorld != SNO_NONE) {
        ScopedWorldDefinitionLockR pdefWorldDungeonfinderTileset(tWCD.snoDungeonFinderSourceWorld);
        snoSkyboxActor = pdefWorldDungeonfinderTileset->tEnvironment.snoSkyboxActor;
    }
    SNO snoWeather = Weather::GetWeatherToUse(GetRWindowMgr()->GetCurrentRWindow());
    if (snoWeather != SNO_NONE) {
        ScopedWeatherDefinitionLockR pdefWeather(snoWeather);
        if (pdefWeather->snoSkyboxActor != SNO_NONE)
            snoSkyboxActor = pdefWeather->snoSkyboxActor;
    }
    if (snoSkyboxActor != SNO_NONE) {
        RActor * ptRActor = RActorTryToGet(ptSkyGlobals->idSkyboxActor);
        if (ptRActor == NULL || ptRActor->snoActor != snoSkyboxActor) {
            if (ptRActor != NULL)
                RActorFlagForDeletion(ptRActor);
            ptSkyGlobals->idSkyboxActor = RActorCreate(snoSkyboxActor, 1.0f, -1.0f, cgtPRTransformIdentity, 0, RENDERCONTEXT_DEFAULT, -1);
            ptRActor = RActorGet(ptSkyGlobals->idSkyboxActor);
            ptRActor->dwFlags &= ~(1 << RACTOR_SELECTABLE_BIT);
        }
        const CameraState & cmState = CameraManagerGetCameraState(0);
        RActorSnapToPosition(ptRActor, cmState.transform.wp, -1, NULL, NULL);
    }
}

}  // namespace Sky
