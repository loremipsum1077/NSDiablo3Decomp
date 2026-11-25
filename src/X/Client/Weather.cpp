#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Attachments.h"
#include "CGame.h"
#include "CPlayer.h"
#include "Definitions/WeatherDefinition.h"
#include "Definitions/CAttachmentDefinition.h"
#include "Lights.h"
#include "RMessage.h"
#include "RActors/RActors.h"
#include "RWindow.h"
#include "Weather.h"
#include "../Common/CommonTypes.h"
#include "../Common/GameCommonData.h"
#include "../Common/GlobalSNO.h"
#include "../Common/World.h"
#include "../Sigma/DataStructures/XList.h"
#include "../Sigma/Math/Cylinder.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Shape.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"
#include "Camera/CameraLocation.h"
#include "CWorld.h"
#include "Definitions/LightDefinition.h"
#include "Particle.h"
#include "Sound/Sound.h"
#include "../Common/GameConstants.h"
struct WeatherRenderFogState {  // size 0x18  // X/Client/Weather.h:26 in the original
    BOOL bFogEnabled;  // 0x00
    RGBAColor rgbaFogColor;  // 0x04
    float flFogNear;  // 0x08
    float flFogEnd;  // 0x0c
    float flHeightFogNear;  // 0x10
    float flHeightFogEnd;  // 0x14
};
static_assert(sizeof(WeatherRenderFogState) == 0x18, "WeatherRenderFogState");
struct WeatherRenderData {  // size 0x1c  // X/Client/Weather.h:34
    float flIntensity;  // 0x00
    WeatherRenderFogState tFogState;  // 0x04
};
static_assert(sizeof(WeatherRenderData) == 0x1c, "WeatherRenderData");
enum {
    MAX_QUEUED_THUNDERCLAPS = 4,
    MAX_SUBGRIDS_AWAY = 2,
    NUM_SURROUNDING_SUBGRIDS = 25,
    SAMPLES_PER_SUBGRID_EDGE = 10,
    MAX_SPLASH_COUNT = 75,
    MAX_RIPPLE_COUNT = 150,
};


enum {
    ML_SUNLIGHT_SCENE = 0,
    ML_SUNLIGHT_ACTOR = 1,
    ML_PLAYERLIGHT_SCENE = 2,
    ML_PLAYERLIGHT_ACTOR = 3,
    ML_LOCALPLAYERHEADLIGHT = 4,
    NUM_MANUAL_LIGHTS = 5,
    NUM_FEEDER_LIGHTS = 10,
};

static const float SUBGRID_WIDTH = 30.0f;
static const float PLAYER_OFFSET_DISTANCE = 15.0f;


static const float MIN_FOG_DEPTH = 50.0f;
static const float MIN_HEIGHT_FOG_DEPTH = 5.0f;
#include "SNOPreload.h"
#include "../Common/SceneCommon.h"
#include "../Common/Definitions/QuestRangeDefinition.h"
#include "../Common/Definitions/WorldDefinition.h"
#include <float.h>




struct ManualLightState {  // size 0x44

    Vector3D wv;  // 0x00
    float wdInnerRadius;  // 0x0c
    float wdRadius;  // 0x10
    RGBAColorValue rgbavalAmbient;  // 0x14
    RGBAColorValue rgbavalDiffuse;  // 0x24
    RGBAColorValue rgbavalSpecular;  // 0x34
};
static_assert(sizeof(ManualLightState) == 0x44, "ManualLightState");
struct WeatherState {  // size 0x18c


    RGBAColorValue rgbavalFogColor;  // 0x00
    float flDistanceStart, flDistanceEnd;  // 0x10, 0x14
    float flHeightFogStart, flHeightFogEnd;  // 0x18, 0x1c

    float flZoomDistanceStart;  // 0x20
    float flZoomDistanceEnd;  // 0x24


    ManualLightState tLightState[5];  // 0x28

    float flShadowFactor;  // 0x17c
    float flAOFactor;  // 0x180
    float flAOShadowFactor;  // 0x184
    float flNormalMapFactor;  // 0x188
};
static_assert(sizeof(WeatherState) == 0x18c, "WeatherState");
typedef XList<float, float, 4> ThunderclapQueue;

struct Subgrid {  // size 0x198

    IVector2D pSubgridPos;  // 0x00
    float flZ[10][10];  // 0x08
};
typedef XList<Subgrid, Subgrid &, 25> SubgridList;
static_assert(sizeof(Subgrid) == 0x198, "Subgrid");
struct WeatherVolume {  // size 0x54

    Vector3D wpPos;  // 0x00
    Shape::Type eType;  // 0x0c
    AxialCylinder cyl;  // 0x10
    OBB obb;  // 0x24
    SNO snoWeather;  // 0x4c
    SNO snoQuestRange;  // 0x50
};
static_assert(sizeof(WeatherVolume) == 0x54, "WeatherVolume");
typedef blz::vector<WeatherVolume, blz::allocator<WeatherVolume> > WeatherVolumeArray;

struct WeatherRWindowData {  // size 0x2ed8

    float flTimeElapsed;  // 0x00

    SNO snoParticle;  // 0x04
    SNO snoFogParticle;  // 0x08
    ParticleSystemID idParticle;  // 0x0c
    ParticleSystemID idFogParticle;  // 0x10
    ParticleSystemID idParticleRainSplash;  // 0x14
    ParticleSystemID idParticleRainRipple;  // 0x18

    float flIntensity;  // 0x1c
    float flTargetIntensity;  // 0x20

    float flDepthOfFieldNear;  // 0x24
    float flDepthOfFieldFar;  // 0x28
    float flDepthOfFieldMax;  // 0x2c

    SNO snoWeatherPrev;  // 0x30
    WeatherState tTargetState;  // 0x34
    WeatherState tState;  // 0x1c0

    PRTransform tPRTransformLightning;  // 0x34c

    float flLightningCountdown;  // 0x368
    ThunderclapQueue tThunderclapQueue;  // 0x370

    ColorCorrectionParams tColorCorrection;  // 0x428
    ColorCorrectionParams tColorCorrectionPrev;  // 0x438
    ColorCorrectionParams tColorCorrectionPrev2;  // 0x448

    BOOL fHasValidCurrentState;  // 0x458

    IVector2D pLastSubgridPos;  // 0x45c
    SubgridList tSubgridList;  // 0x468

    LightID idFeederLights[10];  // 0x2e28
    LightID idManualLights[5];  // 0x2e50

    WeatherVolumeArray tWeatherVolumes;  // 0x2e68

    RActorID idRActorLitPlayer;  // 0x2e80

    WeatherRenderFogState tRenderFogState;  // 0x2e84

    WeatherRenderData tRenderData[2];  // 0x2e9c
};
static_assert(sizeof(WeatherRWindowData) == 0x2ed8, "WeatherRWindowData");
WeatherState cgtUnfoggedState;

static void sDeleteFeederLights(WeatherRWindowData & tData);



static void sLightSafeDelete(LightID & idLight) {
    if (idLight != -1) {
        LightDelete(idLight);
        idLight = -1;
    }
}
template <>
inline void Math::Lerp<RGBAColorValue>(const RGBAColorValue & t0, const RGBAColorValue & t1, const float t, RGBAColorValue & tResult) {
    Math::Lerp(t0.r, t1.r, t, tResult.r);
    Math::Lerp(t0.g, t1.g, t, tResult.g);
    Math::Lerp(t0.b, t1.b, t, tResult.b);
    Math::Lerp(t0.a, t1.a, t, tResult.a);
}
static void sDeleteFeederLights(WeatherRWindowData & tData) {
    for (int32 i = 0; i < NUM_FEEDER_LIGHTS; i++) {
        sLightSafeDelete(tData.idFeederLights[i]);
    }
}

LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & transform, int32 nUnused, RenderContext eContext);  // X/Client/Lights.cpp





static void sSnapToTargetState(
    WeatherRWindowData & tData)
{
    tData.tState = tData.tTargetState;
    tData.flIntensity = tData.flTargetIntensity;
    tData.tColorCorrectionPrev = tData.tColorCorrection;
    tData.tColorCorrectionPrev2 = tData.tColorCorrectionPrev;
}




void Weather::InitializeForGame() {
}








void Weather::DisposeForGame() {
}








void Weather::InitializeForWorld(RWindow & tRWindow) {
    WeatherRWindowData & tData = tRWindow.GetWeatherRWindowData();
    tData.fHasValidCurrentState = FALSE;
    tData.idManualLights[ML_SUNLIGHT_SCENE] = LightAdd(2, GlobalSNOGet(GLOBALSNO_LIGHT_MANUAL_SUNLIGHT_SCENE), cgtPRTransformIdentity, 0, RENDERCONTEXT_DEFAULT);
    tData.idManualLights[ML_SUNLIGHT_ACTOR] = LightAdd(2, GlobalSNOGet(GLOBALSNO_LIGHT_MANUAL_SUNLIGHT_ACTOR), cgtPRTransformIdentity, 0, RENDERCONTEXT_DEFAULT);
    tData.tWeatherVolumes.clear();
}





















WeatherRWindowData * Weather::InitializeForRWindow()

{
    WeatherRWindowData * ptWeatherRWindowData = new (SigmaMemoryNew(sizeof(WeatherRWindowData), 4, NULL, TRUE)) WeatherRWindowData();
    WeatherRWindowData & tData = *ptWeatherRWindowData;

    tData.snoParticle = SNO_NONE;
    tData.snoFogParticle = SNO_NONE;

    tData.tColorCorrection.snoTexSaturationVignette = SNO_NONE;
    tData.tColorCorrection.snoTexCorrectionA = SNO_NONE;
    tData.tColorCorrection.snoTexCorrectionB = SNO_NONE;
    tData.tColorCorrection.fRetro = FALSE;

    tData.tColorCorrectionPrev = tData.tColorCorrection;
    tData.tColorCorrectionPrev2 = tData.tColorCorrection;

    tData.idParticle = PARTICLESYSTEMID_INVALID;
    tData.idFogParticle = PARTICLESYSTEMID_INVALID;
    tData.idParticleRainSplash = PARTICLESYSTEMID_INVALID;
    tData.idParticleRainRipple = PARTICLESYSTEMID_INVALID;
    // Inference: lines 291-315 sit one indent level deeper (line-table columns), so they are in a bare block.
    {
        cgtUnfoggedState.rgbavalFogColor = RGBAColorValue(0.0f, 0.0f, 0.0f, 1.0f);
        cgtUnfoggedState.flDistanceStart = 500.0f;
        cgtUnfoggedState.flDistanceEnd = 550.0f;
        cgtUnfoggedState.flHeightFogStart = -200.0f;
        cgtUnfoggedState.flHeightFogEnd = -250.0f;

        cgtUnfoggedState.flZoomDistanceStart = cgtUnfoggedState.flDistanceStart;
        cgtUnfoggedState.flZoomDistanceEnd = cgtUnfoggedState.flDistanceEnd;


        for (int32 i = 0; i < NUM_MANUAL_LIGHTS; i++)
        {
            ManualLightState & tLightState = cgtUnfoggedState.tLightState[i];
            tLightState.wv = Vector3D(0.0f, 0.0f, 1.0f);
            tLightState.wdInnerRadius = 0.0f;
            tLightState.wdRadius = 0.0f;
            tLightState.rgbavalAmbient = RGBAColorValue(0.0f, 0.0f, 0.0f, 1.0f);
            tLightState.rgbavalDiffuse = RGBAColorValue(0.0f, 0.0f, 0.0f, 1.0f);
            tLightState.rgbavalSpecular = RGBAColorValue(0.0f, 0.0f, 0.0f, 1.0f);
        }

        cgtUnfoggedState.flShadowFactor = 0.0f;
        cgtUnfoggedState.flAOFactor = 1.0f;
        cgtUnfoggedState.flAOShadowFactor = 1.0f;
        cgtUnfoggedState.flNormalMapFactor = 1.0f;
    }

    tData.tTargetState = cgtUnfoggedState;
    tData.tState = cgtUnfoggedState;

    tData.tPRTransformLightning.Set(Vector3D(0.0f, -100.0f, 50.0f),
                                    Vector3D(0.0f, 0.0f, 0.0f),
                                    Vector3D(0.0f, 0.0f, 1.0f));
    tData.flLightningCountdown = 5.0f;






    // both fills attribute to 332:33 in the DWARF; kept on one line
    memset(tData.idManualLights, 0xff, sizeof(tData.idManualLights)); memset(tData.idFeederLights, 0xff, sizeof(tData.idFeederLights));


    tData.snoWeatherPrev = SNO_NONE;

    tData.idRActorLitPlayer = RACTORID_INVALID;

    return ptWeatherRWindowData;
}




void Weather::DisposeForRWindow(WeatherRWindowData * ptData) {
    delete ptData;
}

void AttachmentAdd(const RMessage & tMessage, int32 idAttachment);  // X/Client/Attachments.cpp
void RActorAttachmentResolveHardpointLinks(RActorID * pidRActors, RMessage & tMessage);  // X/Client/RActors/RActorAttachments.cpp



static void sCreateManualLightForPlayer(LightID & idLight, const RActorID idRActor, const SNO snoLight, HardpointHash szHardpointName) {
    idLight = LightAdd(2, snoLight, cgtPRTransformIdentity, 0, RENDERCONTEXT_DEFAULT);

    TriggerEvent tTriggerEvent;
    TriggerEventInit(tTriggerEvent);
    tTriggerEvent.eType = TRIGGEREDEVENTTYPE_ADD_OBJECT;
    tTriggerEvent.eAttachmentType = ATTACHMENTTYPE_LIGHT;
    tTriggerEvent.snoname.Set(LIGHT_SNO, snoLight);
    tTriggerEvent.dwID = 678;
    tTriggerEvent.tHardpointLinks[0].tHash = szHardpointName;

    RMessage tMessage;
    if (tMessage.Initialize(tTriggerEvent, 0.0f, PARENT_RACTOR, idRActor, -1.0f, FALSE, FALSE, 0.0f)) {
        RActorID idRActors[3] = { idRActor, RACTORID_INVALID, RACTORID_INVALID };
        RActorAttachmentResolveHardpointLinks(idRActors, tMessage);
        AttachmentAdd(tMessage, idLight);

        ParentData tParentData;
        tParentData.SetHardpoint(PARENT_LOCAL_PLAYER_CENTROID, idRActor, szHardpointName);
        RActor * ptRActor = RActorTryToGet(idRActor);
        if (ptRActor && HardpointIndexFromName(ptRActor->snoAppearance, szHardpointName) == -1) {
            tParentData.Set(PARENT_LOCAL_PLAYER_CENTROID, idRActor, -1, PRTransform(cgtPRTransformIdentity.q, Vector3D(0.0f, 0.0f, 10.0f)), 1.0f);
        }
        LightSetParent(idLight, tParentData);
    }
}

const CameraState & CameraManagerGetCameraState(const int32 nCameraIndex);  // X/Client/Camera/CameraManager.cpp:668
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);  // X/Client/Sound/Sound.cpp:1810
SoundID PlayBkgrndAmbient(const SNO snoSound, const int32 nBkgrndAmbientController, const float flBlendTime, const BOOL fOverlay);  // X/Client/Sound/Sound.cpp:2331
}  // namespace SoundManager
void EffectGroupCreateUntracked(const SNO snoEffectGroup, const RActorID idRActorSource, const uint32 dwAttachmentID, const BOOL bIgnoreOwnerAlpha, const BOOL bRemote);  // X/Client/EffectGroup.cpp:1299
void ParticleSystemDelete(const ParticleSystemID idParticleSystem);  // X/Client/Particle.cpp:2191
BOOL CScenesGetFloorHeight(SWorldID idSWorld, const Vector3D & wp, float & fFloorHeight);  // X/Client/Scene.cpp:4553
void CWorldResetForRetro();  // X/Client/CWorld.cpp:1447

















RActorID Weather::GetLitPlayer(const WeatherRWindowData & tData) {
    return tData.idRActorLitPlayer;
}

// NON_MATCHING: 396 vs 160 bytes; sDeleteFeederLights is inlined here until its other caller (Update) exists




void Weather::AddPlayerLights(WeatherRWindowData & tData, const RActorID idRActor) {
    if (!RActorTryToGet(idRActor)) {
        return;
    }
    tData.idRActorLitPlayer = idRActor;

    LightDelete(tData.idManualLights[ML_PLAYERLIGHT_SCENE]);
    sCreateManualLightForPlayer(tData.idManualLights[ML_PLAYERLIGHT_SCENE], idRActor, GlobalSNOGet(GLOBALSNO_LIGHT_MANUAL_PLAYERLIGHT_SCENE), HardpointHash("HP_light"));
    LightDelete(tData.idManualLights[ML_PLAYERLIGHT_ACTOR]);
    sCreateManualLightForPlayer(tData.idManualLights[ML_PLAYERLIGHT_ACTOR], idRActor, GlobalSNOGet(GLOBALSNO_LIGHT_MANUAL_PLAYERLIGHT_ACTOR), HardpointHash("HP_light"));
    LightDelete(tData.idManualLights[ML_LOCALPLAYERHEADLIGHT]);
    sCreateManualLightForPlayer(tData.idManualLights[ML_LOCALPLAYERHEADLIGHT], idRActor, GlobalSNOGet(GLOBALSNO_LIGHT_MANUAL_HEADLIGHT), HardpointHash("HP_head"));
}





































void Weather::DisposeForWorld(RWindow & tRWindow) {
    WeatherRWindowData & tData = tRWindow.GetWeatherRWindowData();
    for (int32 i = 0; i < NUM_MANUAL_LIGHTS; i++) {
        sLightSafeDelete(tData.idManualLights[i]);
    }
    tData.snoWeatherPrev = SNO_NONE;
    sDeleteFeederLights(tData);
}









static void sApplyStateToLight(
    Light * ptLight,
    const ManualLightState & tLightState)
{
    ptLight->tState.wv = tLightState.wv;
    ptLight->tState.tInterpState.wdInnerRadius = tLightState.wdInnerRadius * ptLight->flRadiusScalar;
    ptLight->tState.tInterpState.wdRadius = tLightState.wdRadius * ptLight->flRadiusScalar;
    ptLight->tState.tInterpState.rgbavalAmbient = tLightState.rgbavalAmbient;
    ptLight->tState.tInterpState.rgbavalDiffuse = tLightState.rgbavalDiffuse;
    ptLight->tState.tInterpState.rgbavalSpecular = tLightState.rgbavalSpecular;
}




static void sApplyWeatherState(
    WeatherRWindowData & tData)
{
    for (int32 i = 0; i < NUM_MANUAL_LIGHTS; ++i)
    {
        Light * ptLight = LightTryToGet(tData.idManualLights[i]);
        if (ptLight)
        {
            sApplyStateToLight(ptLight, tData.tState.tLightState[i]);
        }
    }

    const CameraState & cmState = CameraManagerGetCameraState(-1);
    WeatherState & tState = tData.tState;
    WeatherRenderFogState & tRenderFogState = tData.tRenderFogState;

    BOOL fFogged = TRUE;  // no code of its own: constant-folded (bFogEnabled store at 515 is an immediate 1, block 518-542 has no guard branch)
    tRenderFogState.bFogEnabled = fFogged;
    if (fFogged)  // inferred guard: block 518-542 is entered without a test
    {
        float flFogEnd = Math::Min(tState.flDistanceEnd, cmState.flFarPlane);
        float flFogNear = Math::Min(tState.flDistanceStart, flFogEnd - MIN_FOG_DEPTH);


        if (tState.flZoomDistanceEnd != 0.0f || tState.flZoomDistanceStart != 0.0f)
        {
            float flZoomFogEnd = Math::Min(tState.flZoomDistanceEnd, cmState.flFarPlane);
            float flZoomFogNear = Math::Min(tState.flZoomDistanceStart, flFogEnd - MIN_FOG_DEPTH);

            Math::Lerp(flZoomFogEnd, flFogEnd, cmState.flZoomFactor, flFogEnd);
            Math::Lerp(flZoomFogNear, flFogNear, cmState.flZoomFactor, flFogNear);
        }


        float flHeightFogEnd = tState.flHeightFogEnd;
        float flHeightFogNear = Math::Max(tState.flHeightFogStart, flHeightFogEnd + MIN_HEIGHT_FOG_DEPTH);

        RGBAColor rgbaColor;
        rgbaColor.Set(tState.rgbavalFogColor);

        tRenderFogState.rgbaFogColor = rgbaColor;
        tRenderFogState.flFogNear = flFogNear;
        tRenderFogState.flFogEnd = flFogEnd;
        tRenderFogState.flHeightFogNear = flHeightFogNear;
        tRenderFogState.flHeightFogEnd = flHeightFogEnd;
    }
}













static void sHandleLightning(
    WeatherRWindowData & tData,
    const WeatherDefinition * pdefWeather,
    float flDeltaTime)
{

    float flCurrentTime = CGameGetClientTime();
    BOOL fKeepGoing = TRUE;  // no instruction of its own: folded into the loop exit branch at 569
    while (tData.tThunderclapQueue.Count() != 0 && fKeepGoing)
    {
        float flThunderTime = tData.tThunderclapQueue.GetHead();
        if (flThunderTime < flCurrentTime)
        {
            tData.tThunderclapQueue.RemoveHead();
            if (pdefWeather->snoThunder != -1)
            {
                SoundManager::PlaySound(pdefWeather->snoThunder, cgtPSODefaults);
            }
        }
        else
        {
            fKeepGoing = FALSE;  // inferred: b.pl at 569 leaves the loop
        }
    }

    float flIntensity = Weather::GetIntensity(tData);

    tData.flLightningCountdown = Math::Max(tData.flLightningCountdown - flDeltaTime, 0.0f);
    if (flIntensity > pdefWeather->flLightningThreshold && tData.flLightningCountdown == 0.0f)
    {

        RNG & rng = CGameGlobalsGet().rngLocal;




        float flLightningIntensity = (flIntensity - pdefWeather->flLightningThreshold) / (1.0f - pdefWeather->flLightningThreshold);
        uint32 dwLightningDelayDelta = Math::FloatToInt(pdefWeather->dwLightningDelayDelta * (1.0f + flLightningIntensity * -0.8f));
        tData.flLightningCountdown = (pdefWeather->dwLightningDelayMin + Rand_Internal(rng, dwLightningDelayDelta)) * (1.0f / 60.0f);


        if (pdefWeather->snoLightning != -1)
        {
            LightAdd(0, pdefWeather->snoLightning, tData.tPRTransformLightning, 0, RENDERCONTEXT_DEFAULT);
        }
        if (pdefWeather->snoLightningEffectGroup != -1)
        {
            RActorID idPlayerRActor = LocalPlayerRActorGet();
            if (idPlayerRActor != RACTORID_INVALID)
            {
                EffectGroupCreateUntracked(pdefWeather->snoLightningEffectGroup, idPlayerRActor, 0, FALSE, FALSE);
            }
        }


        float flThunderclapTime = flCurrentTime + (1.0f - flLightningIntensity) * (pdefWeather->dwThunderDelayMax * (1.0f / 60.0f));
        if (tData.tThunderclapQueue.Count() < MAX_QUEUED_THUNDERCLAPS)
        {
            tData.tThunderclapQueue.AddTail_Tracked(flThunderclapTime);
        }
    }
}





template <>
inline void Math::Lerp<ManualLightState>(const ManualLightState & tState1, const ManualLightState & tState2, const float flT, ManualLightState & tStateResult) {
    Math::Lerp(tState1.wv.x, tState2.wv.x, flT, tStateResult.wv.x); Math::Lerp(tState1.wv.y, tState2.wv.y, flT, tStateResult.wv.y);
    Math::Lerp(tState1.wv.z, tState2.wv.z, flT, tStateResult.wv.z); Math::Lerp(tState1.wdInnerRadius, tState2.wdInnerRadius, flT, tStateResult.wdInnerRadius);
    Math::Lerp(tState1.wdRadius, tState2.wdRadius, flT, tStateResult.wdRadius); Math::Lerp(tState1.rgbavalAmbient.r, tState2.rgbavalAmbient.r, flT, tStateResult.rgbavalAmbient.r);
    Math::Lerp(tState1.rgbavalAmbient.g, tState2.rgbavalAmbient.g, flT, tStateResult.rgbavalAmbient.g); Math::Lerp(tState1.rgbavalAmbient.b, tState2.rgbavalAmbient.b, flT, tStateResult.rgbavalAmbient.b);
    Math::Lerp(tState1.rgbavalAmbient.a, tState2.rgbavalAmbient.a, flT, tStateResult.rgbavalAmbient.a); Math::Lerp(tState1.rgbavalDiffuse.r, tState2.rgbavalDiffuse.r, flT, tStateResult.rgbavalDiffuse.r);
    Math::Lerp(tState1.rgbavalDiffuse.g, tState2.rgbavalDiffuse.g, flT, tStateResult.rgbavalDiffuse.g); Math::Lerp(tState1.rgbavalDiffuse.b, tState2.rgbavalDiffuse.b, flT, tStateResult.rgbavalDiffuse.b);
    Math::Lerp(tState1.rgbavalDiffuse.a, tState2.rgbavalDiffuse.a, flT, tStateResult.rgbavalDiffuse.a); Math::Lerp(tState1.rgbavalSpecular.r, tState2.rgbavalSpecular.r, flT, tStateResult.rgbavalSpecular.r);
    Math::Lerp(tState1.rgbavalSpecular.g, tState2.rgbavalSpecular.g, flT, tStateResult.rgbavalSpecular.g); Math::Lerp(tState1.rgbavalSpecular.b, tState2.rgbavalSpecular.b, flT, tStateResult.rgbavalSpecular.b);
    Math::Lerp(tState1.rgbavalSpecular.a, tState2.rgbavalSpecular.a, flT, tStateResult.rgbavalSpecular.a);
}

// NON_MATCHING: fog-color fsub/fmul/fadd interleave and the t0/t1 base registers in the light loop are swapped


template <>
void Math::Lerp<WeatherState>(
    const WeatherState & tState1,
    const WeatherState & tState2,
    const float flT,
    WeatherState & tStateResult)
{
    tStateResult.flDistanceStart = Math::Lerp(tState1.flDistanceStart, tState2.flDistanceStart, flT);
	tStateResult.flDistanceEnd = Math::Lerp(tState1.flDistanceEnd, tState2.flDistanceEnd, flT);
	tStateResult.flHeightFogStart = Math::Lerp(tState1.flHeightFogStart, tState2.flHeightFogStart, flT);
	tStateResult.flHeightFogEnd = Math::Lerp(tState1.flHeightFogEnd, tState2.flHeightFogEnd, flT);

    tStateResult.flZoomDistanceStart = Math::Lerp(tState1.flZoomDistanceStart, tState2.flZoomDistanceStart, flT);
    tStateResult.flZoomDistanceEnd = Math::Lerp(tState1.flZoomDistanceEnd, tState2.flZoomDistanceEnd, flT);

	tStateResult.rgbavalFogColor = Math::Lerp(tState1.rgbavalFogColor, tState2.rgbavalFogColor, flT);

    for (int32 i = 0; i < NUM_MANUAL_LIGHTS; i++) {
        tStateResult.tLightState[i] = Math::Lerp(tState1.tLightState[i], tState2.tLightState[i], flT);
    }


    tStateResult.flShadowFactor = Math::Lerp(tState1.flShadowFactor, tState2.flShadowFactor, flT);
	tStateResult.flAOFactor = Math::Lerp(tState1.flAOFactor, tState2.flAOFactor, flT);
	tStateResult.flAOShadowFactor = Math::Lerp(tState1.flAOShadowFactor, tState2.flAOShadowFactor, flT);
	tStateResult.flNormalMapFactor = Math::Lerp(tState1.flNormalMapFactor, tState2.flNormalMapFactor, flT);
}





static void sParticleSystemDeleteOrOutro(
    ParticleSystemID & idParticleSystem,
    const BOOL fDeleteInsteadOfOutro)
{
    ParticleSystem * ptParticleSystem = ParticleSystemTryToGet(idParticleSystem);
    if (ptParticleSystem)
    {
        if (fDeleteInsteadOfOutro)
        {
            ParticleSystemDelete(idParticleSystem);
        }
        else
        {
            ptParticleSystem->PlayOutro();
        }
    }
    idParticleSystem = -1;  // ParticleSystemID invalid value: str of w26/w20 (-1) after the branches
}




static void sSetWeatherParticleSystem(
    WeatherRWindowData & tData,
    const SNO snoWeatherParticle,
    const BOOL fDeleteInsteadOfOutro)
{
    if (tData.snoParticle == snoWeatherParticle &&
        (tData.idParticle == PARTICLESYSTEMID_INVALID || ParticleSystemTryToGet(tData.idParticle)) &&
        (tData.idParticleRainSplash == PARTICLESYSTEMID_INVALID || ParticleSystemTryToGet(tData.idParticleRainSplash)) &&
        (tData.idParticleRainRipple == PARTICLESYSTEMID_INVALID || ParticleSystemTryToGet(tData.idParticleRainRipple)))
    {
        return;
    }

    if (tData.idParticle != PARTICLESYSTEMID_INVALID)
    {
        sParticleSystemDeleteOrOutro(tData.idParticle, fDeleteInsteadOfOutro);
    }
    if (tData.idParticleRainSplash != PARTICLESYSTEMID_INVALID)
    {
        sParticleSystemDeleteOrOutro(tData.idParticleRainSplash, fDeleteInsteadOfOutro);
    }
    if (tData.idParticleRainRipple != PARTICLESYSTEMID_INVALID)
    {
        sParticleSystemDeleteOrOutro(tData.idParticleRainRipple, fDeleteInsteadOfOutro);
    }

    if (snoWeatherParticle != SNO_NONE)
    {
        tData.idParticle = ParticleSystemAdd(ParticleSystemAddParams(snoWeatherParticle, cgtPRTransformIdentity));
        tData.idParticleRainSplash = ParticleSystemAdd(ParticleSystemAddParams(GlobalSNOGet(GLOBALSNO_PARTICLE_RAIN_IMPACT), cgtPRTransformIdentity));
        tData.idParticleRainRipple = ParticleSystemAdd(ParticleSystemAddParams(GlobalSNOGet(GLOBALSNO_PARTICLE_RAIN_RIPPLE), cgtPRTransformIdentity));
    }

    tData.snoParticle = snoWeatherParticle;
}




static void sSetFogParticleSystem(
    WeatherRWindowData & tData,
    const SNO snoFogParticle,
    const BOOL fDeleteInsteadOfOutro)
{
    if (snoFogParticle == tData.snoFogParticle)
    {
        return;
    }

    if (tData.idFogParticle != -1)
    {
        sParticleSystemDeleteOrOutro(tData.idFogParticle, fDeleteInsteadOfOutro);
    }

    if (snoFogParticle != -1)
    {
        tData.idFogParticle = ParticleSystemAdd(ParticleSystemAddParams(snoFogParticle, cgtPRTransformIdentity));
    }

    tData.snoFogParticle = snoFogParticle;
}




static void sSetColorCorrection(
    WeatherRWindowData & tData,
    const ColorCorrectionParams & tParams)
{
    tData.tColorCorrection = tParams;
}




static void sBuildSubgrid(
    Subgrid & tSubgrid,
    const IVector2D & pSubgrid,
    const float flPlayerZ)
{
    tSubgrid.pSubgridPos = pSubgrid;
    SWorldID idSWorld = CGameGetCurrentSWorldID();

    for (int32 y = 0; y < SAMPLES_PER_SUBGRID_EDGE; y++)
    {
        for (int32 x = 0; x < SAMPLES_PER_SUBGRID_EDGE; x++)
        {
            tSubgrid.flZ[y][x] = -999.0f;

            Vector3D wpTop, wpBottom;  // wpBottom has no location (unused in the optimised code)
            wpTop.Set((x / 10.0f + pSubgrid.x) * SUBGRID_WIDTH, (y / 10.0f + pSubgrid.y) * SUBGRID_WIDTH, flPlayerZ + 200.0f);





            float fFloorHeight = 0.0f;
            if (CScenesGetFloorHeight(idSWorld, wpTop, fFloorHeight))
            {
                tSubgrid.flZ[y][x] = fFloorHeight + 0.1f;
            }
        }
    }
}

















static Subgrid * sFindSubgrid(
    WeatherRWindowData & tData,
    IVector2D & pSubgridPos)
{
    POS pos = tData.tSubgridList.GetHeadPosition();
    while (pos)
    {
        Subgrid & tSubgrid = tData.tSubgridList.GetNext(pos);
        if (tSubgrid.pSubgridPos == pSubgridPos)
        {
            return &tSubgrid;
        }
    }

    return NULL;  // no instruction of its own: the callers branch on a null result (cbz at 821 falls out of the loop)
}




static void sRemoveSubgridsNotAroundPlayer(
    WeatherRWindowData & tData,
    IVector2D & pPlayerPos)
{
    POS pos = tData.tSubgridList.GetHeadPosition();
    while (pos)
    {
        POS posCur = pos;
        Subgrid & tSubgrid = tData.tSubgridList.GetNext(pos);

        if (abs(tSubgrid.pSubgridPos.x - pPlayerPos.x) > MAX_SUBGRIDS_AWAY ||
            abs(tSubgrid.pSubgridPos.y - pPlayerPos.y) > MAX_SUBGRIDS_AWAY)
        {
            tData.tSubgridList.RemoveAt(posCur);
        }
    }
}




static void sBuildSubgridsAroundPlayer(
    WeatherRWindowData & tData,
    const IVector2D & pPlayerPos,
    const float flPlayerZ)
{
    for (int32 y = -MAX_SUBGRIDS_AWAY; y <= MAX_SUBGRIDS_AWAY; y++)
    {
        for (int32 x = -MAX_SUBGRIDS_AWAY; x <= MAX_SUBGRIDS_AWAY; x++)
        {
            IVector2D pPos(x + pPlayerPos.x, y + pPlayerPos.y);

            if (!sFindSubgrid(tData, pPos))
            {
                Subgrid tSubgrid;
                sBuildSubgrid(tSubgrid, pPos, flPlayerZ);
                tData.tSubgridList.AddTail_Tracked(tSubgrid);
            }
        }
    }
}




static float sGetSubgridHeight(
    WeatherRWindowData & tData,
    const Vector3D & wpPos)
{
    Vector2D pSubgridPos(wpPos.x / SUBGRID_WIDTH, wpPos.y / SUBGRID_WIDTH);
    IVector2D pISubgrid(Math::FloatFloorToInt(pSubgridPos.x), Math::FloatFloorToInt(pSubgridPos.y));




    Subgrid * ptSubgrid = sFindSubgrid(tData, pISubgrid);
    if (ptSubgrid) {



        Vector2D pSample(
            (pSubgridPos.x - pISubgrid.x) * SAMPLES_PER_SUBGRID_EDGE,
            (pSubgridPos.y - pISubgrid.y) * SAMPLES_PER_SUBGRID_EDGE);

        IVector2D pISample(Math::FloatToInt(pSample.x), Math::FloatToInt(pSample.y));




        return ptSubgrid->flZ[pISample.y][pISample.x];
    }
    return -999.0f;  // inference: no code of its own (value is hoisted into a register shared with the caller's wpPos.z = ... stores)
}



































































static void sCreateRainSplashParticles(
    WeatherRWindowData & tData)
{
    RActorID idLocalPlayerRActor = LocalPlayerRActorGet();
    if (idLocalPlayerRActor == RACTORID_INVALID) {
        return;
    }


    ParticleSystem* ptParticleSystem = ParticleSystemTryToGet(tData.idParticleRainSplash);
    if (!ptParticleSystem) {
        return;
    }


    int32 nDesiredCount = Math::FloatToInt(MAX_SPLASH_COUNT * Weather::GetIntensity(tData));
    if (nDesiredCount == 0) {
        return;
    }


    const RActor * ptRActorPlayer = RActorGet(idLocalPlayerRActor);
    const WorldLocation & locRActorPlayer = ptRActorPlayer->tLocation.tWorldLocation;

    Vector3D wvOffsetPlayerPos(
        locRActorPlayer.transform.wp.x - PLAYER_OFFSET_DISTANCE,
        locRActorPlayer.transform.wp.y - PLAYER_OFFSET_DISTANCE,
        locRActorPlayer.transform.wp.z);

    IVector2D pOffsetPlayerPos(
        Math::FloatToInt(wvOffsetPlayerPos.x / SUBGRID_WIDTH),
        Math::FloatToInt(wvOffsetPlayerPos.y / SUBGRID_WIDTH));

    if (pOffsetPlayerPos != tData.pLastSubgridPos) {

        sRemoveSubgridsNotAroundPlayer(tData, pOffsetPlayerPos);
        sBuildSubgridsAroundPlayer(tData, pOffsetPlayerPos, wvOffsetPlayerPos.z);
        tData.pLastSubgridPos = pOffsetPlayerPos;
    }

    RNG& rng = CGameGlobalsGet().rngLocal;

    const float SUBGRID_EXTENT_EPSILON = 0.5f;
    Rect2D rectExtents;
    rectExtents.Set(
        (tData.pLastSubgridPos.x - MAX_SUBGRIDS_AWAY) * SUBGRID_WIDTH + SUBGRID_EXTENT_EPSILON,
        (tData.pLastSubgridPos.y - MAX_SUBGRIDS_AWAY) * SUBGRID_WIDTH + SUBGRID_EXTENT_EPSILON,
        (tData.pLastSubgridPos.x + MAX_SUBGRIDS_AWAY + 1) * SUBGRID_WIDTH - SUBGRID_EXTENT_EPSILON,
        (tData.pLastSubgridPos.y + MAX_SUBGRIDS_AWAY + 1) * SUBGRID_WIDTH - SUBGRID_EXTENT_EPSILON);
    float flRange = rectExtents.Width();
    Vector2D wpCenter = rectExtents.Center();

    int32 nTries = nDesiredCount * 2;

    ScopedParticleSystemLock tLock(ptParticleSystem);
    while (ptParticleSystem->GetParticleCount() < nDesiredCount && nTries > 0) {

        Vector3D wpPos;
        wpPos.x = wpCenter.x + RandRangeFloat_Internal(rng, -flRange * 0.5f, flRange);
        wpPos.y = wpCenter.y + RandRangeFloat_Internal(rng, -flRange * 0.5f, flRange);
        wpPos.z = sGetSubgridHeight(tData, wpPos);

        if (wpPos.z > -998.0f) {

            ptParticleSystem->CreateParticle(wpPos, nullptr);
        }

        nTries--;
    }





}




static void sCreateRainRippleParticles(
    WeatherRWindowData & tData)
{
    RActorID idLocalPlayerRActor = LocalPlayerRActorGet();
    if (idLocalPlayerRActor == RACTORID_INVALID) {
        return;
    }


    ParticleSystem* ptParticleSystem = ParticleSystemTryToGet(tData.idParticleRainRipple);
    if (!ptParticleSystem) {
        return;
    }



    float flIntensity = Weather::GetIntensity(tData);
    int32 nDesiredCount = Math::FloatToInt(MAX_RIPPLE_COUNT * flIntensity * flIntensity);
    if (nDesiredCount == 0) {
        return;
    }


    RNG& rng = CGameGlobalsGet().rngLocal;
    float flRange = cwdRippleMapWidth * 0.6f;

    ScopedParticleSystemLock tLock(ptParticleSystem);
    while (ptParticleSystem->GetParticleCount() < nDesiredCount) {

        Vector3D wpPos;
        wpPos.x = RandRangeFloat_Internal(rng, -flRange * 0.5f, flRange);
        wpPos.y = RandRangeFloat_Internal(rng, -flRange * 0.5f, flRange);
        wpPos.z = 0.0f;

        ptParticleSystem->CreateParticle(wpPos, nullptr);
    }
}




static LightID sCreateSrcLight(
    const SNO snoSunLight,
    const WeatherStateParams & tParams)
{
    if (snoSunLight == SNO_NONE)
    {
        return -1;
    }

    LightID idLight = LightAdd(1, snoSunLight, cgtPRTransformIdentity, 0, RENDERCONTEXT_DEFAULT);


    Light * ptLight = LightGet(idLight);
    Matrix3x3 mLight;
    mLight.SetAltAz(
        Math::DegToAngle(Math::Pin(tParams.flSunAltitude, 0.0f, 75.0f)),
        Math::DegToAngle(Math::Pin(tParams.flSunAzimuth, 0.0f, 360.0f)));
    Vector3D wv = Vector3D(0.0f, 0.0f, 1.0f) * mLight;
    ptLight->tState.wv = -wv;

    return idLight;
}




static void sUpdateWeatherState(
    WeatherState & tState,
    const WeatherStateParams & tParams)
{
    if (tParams.fIsFogged) {

        tState.rgbavalFogColor.Set(tParams.rgbaFogColor);

        tState.flDistanceStart = tParams.flFogStart;
        tState.flDistanceEnd = tParams.flFogEnd;
        tState.flHeightFogStart = tParams.flHeightFogStart;
        tState.flHeightFogEnd = tParams.flHeightFogEnd;

        if (Math::Abs(tState.flHeightFogStart) < 0.000001f && Math::Abs(tState.flHeightFogEnd) < 0.000001f) {



            tState.flHeightFogStart = cgtUnfoggedState.flHeightFogStart;
            tState.flHeightFogEnd = cgtUnfoggedState.flHeightFogEnd;
        }

        tState.flZoomDistanceStart = tParams.flZoomFogStart;
        tState.flZoomDistanceEnd = tParams.flZoomFogEnd;
    }


    tState.flShadowFactor = tParams.rgbaShadowColor.r / 255.0f;
    tState.flAOFactor = tParams.flAOFactor;
    tState.flAOShadowFactor = tParams.flAOShadowFactor;
    tState.flNormalMapFactor = tParams.flNormalMapFactor;
}




static void sUpdateManualLightState(
    ManualLightState & tLightState,
    const Light * ptLight)
{
    tLightState.wv = ptLight->tState.wv;
    tLightState.wdInnerRadius = ptLight->tState.tInterpState.wdInnerRadius;
    tLightState.wdRadius = ptLight->tState.tInterpState.wdRadius;
    tLightState.rgbavalAmbient = ptLight->tState.tInterpState.rgbavalAmbient;
    tLightState.rgbavalDiffuse = ptLight->tState.tInterpState.rgbavalDiffuse;
    tLightState.rgbavalSpecular = ptLight->tState.tInterpState.rgbavalSpecular;
}




static void sTrackBestVolume(
    float & flBestDistSq,
    SNO & snoWeatherToUse,
    const WeatherVolume & tWeatherVolume,
    const float flDistSq)
{
    // inferred: no row of its own; the caller's best distance becomes flDistSq before the prefetch (s9 -> s8 at +0x2b8/+0x2c0)
    flBestDistSq = flDistSq;
    if (SNOPreload::PrefetchClientWeather(tWeatherVolume.snoWeather, SNOFilesAsyncPriority_High))
    {
        snoWeatherToUse = tWeatherVolume.snoWeather;
    }
}




SNO Weather::GetWeatherToUse(
    const RWindow & tRWindow)
{
    const WeatherRWindowData & tData = tRWindow.GetWeatherRWindowData();
    SNO snoWeatherToUse = SNO_NONE;














    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    switch (eGameMode)
    {
        case GAMEMODE_EDITUNITLAYOUT:
        case GAMEMODE_MENU:
        {







            SNO snoCurrentWorld = CGameGetCurrentWorld();
            if (snoCurrentWorld != SNO_NONE)
            {
                ScopedWorldDefinitionLockR pdefWorld(snoCurrentWorld);
                snoWeatherToUse = pdefWorld->tEnvironment.snoWeather;
            }
        }
        break;








        case GAMEMODE_PLAYING:
        {

            RActorID idPlayerRActor = LocalPlayerRActorGet();
            const RActor * ptRActor = RActorTryToGet(idPlayerRActor);
            if (ptRActor)
            {
                float bestDistSq = FLT_MAX;
                const WorldLocation & loc = ptRActor->tLocation.tWorldLocation;
                // inferred: the original latch is `cmp x8, x27; b.ne` (exact != test against size()); `<` gives b.hi
                for (uint32 i = 0; i != tData.tWeatherVolumes.size(); i++)
                {
                    const WeatherVolume & tWeatherVolume = tData.tWeatherVolumes[i];

                    if (tWeatherVolume.snoQuestRange != SNO_NONE && !QuestRangeEvaluate(tWeatherVolume.snoQuestRange, NULL))
                    {
                        continue;
                    }

                    switch (tWeatherVolume.eType)
                    {
                        case Shape::AXIAL_CYLINDER:
                        {
                            Vector3D v = loc.transform.wp - tWeatherVolume.cyl.wpBase;
                            float distSq = v.x * v.x + v.y * v.y;

                            if (distSq < bestDistSq &&
                                distSq < tWeatherVolume.cyl.wdRadius * tWeatherVolume.cyl.wdRadius)
                            {
                                sTrackBestVolume(bestDistSq, snoWeatherToUse, tWeatherVolume, distSq);
                            }
                        }
                        break;

                        case Shape::BOX:
                        {
                            Vector3D v = loc.transform.wp - tWeatherVolume.obb.transform.wp;
                            float distSq = v.x * v.x + v.y * v.y;

                            if (distSq < bestDistSq &&
                                tWeatherVolume.obb.ContainsPoint(loc.transform.wp))
                            {
                                sTrackBestVolume(bestDistSq, snoWeatherToUse, tWeatherVolume, distSq);
                            }
                        }
                        break;

                        default:
                            DisplayInternalError(NULL, "", 1285, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                    }
                }

                if (snoWeatherToUse == SNO_NONE)
                {
                    const WorldLocation & locScoped = ptRActor->tLocation.tWorldLocation;
                    WorldCommonData & tWCD = WorldGetCommonData(locScoped.idSWorld);
                    if (tWCD.tWorldSyncedData.snoWeatherOverride != SNO_NONE)
                    {
                        snoWeatherToUse = tWCD.tWorldSyncedData.snoWeatherOverride;
                    }
                }
            }

            if (snoWeatherToUse == SNO_NONE)
            {
                // the original's `cmn w19, #1` here is threaded into line 1293
                RActorID idRActor = LocalPlayerRActorGet();
                RActor * ptRActorScoped = RActorTryToGet(idRActor);
                if (ptRActorScoped)
                {
                    SSceneID idSScene = ptRActorScoped->idSScene;
                    SceneCommonData * ptSCD = SceneCommonDataTryToGet(idSScene);
                    if (ptSCD)
                    {
                        snoWeatherToUse = ptSCD->tSceneSpec.snoWeather;
                    }
                }
            }

            if (snoWeatherToUse != SNO_NONE && GameIsOpenWorld())
            {
                ScopedWeatherDefinitionLockR pdefWeather(snoWeatherToUse);
                int nWeatherCount = pdefWeather->serRandomWeatherSNOs.dwSize / sizeof(int);
                if (nWeatherCount > 0)
                {
                    RNG tRNG;
                    RandInit(tRNG);
                    tRNG.dwSeed = GameGetActiveGameCommonData().tGameSyncedData.nRandomWeatherSeed;


                    if (pdefWeather->eWeatherGroup != WEATHER_GROUP_NONE)
                    {
                        // inferred: lines 1328-1336 leave no rows; the add is folded into the 1324 store (csel at 1327)
                        tRNG.dwSeed += pdefWeather->eWeatherGroup;
                    }
                    else
                    {
                        tRNG.dwSeed += snoWeatherToUse;
                    }

                    int weatherIndex = Rand_Internal(tRNG, nWeatherCount - 1);
                    snoWeatherToUse = pdefWeather->arRandomWeatherSNOs[weatherIndex];
                }
            }
        }
        break;
    }
    return snoWeatherToUse;
}





static void sValidateFeederLightType(
    const SNO snoWeather,
    const SNO snoFeederLight,
    const LightDefinition * pdefManualLight,
    const int32 nLight)
{
    ScopedLightDefinitionLockR pdefFeederLight(snoFeederLight);

    if (pdefFeederLight->eType == pdefManualLight->eType)
    {
        return;
    }

    CRefString sSlotName;
    switch (nLight)
    {
        case ML_SUNLIGHT_SCENE: sSlotName = "Sunlight Scene"; break;
        case ML_SUNLIGHT_ACTOR: sSlotName = "Sunlight Actor"; break;
        case ML_PLAYERLIGHT_SCENE: sSlotName = "Player Scene Light"; break;
        case ML_PLAYERLIGHT_ACTOR: sSlotName = "Player Actor Light"; break;
        case ML_LOCALPLAYERHEADLIGHT: sSlotName = "Local Head Light"; break;
        default:
            DisplayInternalError(NULL, "", 1372, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Lines 1374-1380 leave no instructions: whatever reported the mismatch (it would use snoWeather
    // and sSlotName, the only reason both exist) is compiled out in this build.
}






// NON_MATCHING: 9644 vs 9644 bytes, 1 instruction differs: `orr x8, x20, x23` (line 1451, fExclude) has its operands swapped (register commute only); tried both operand orders, !(a || b), one-line if/else.
void Weather::Update(
    RWindow & tRWindow,
    float flDeltaTime,
    const BOOL fDeleteInsteadOfOutro)
{
    WeatherRWindowData & tData = tRWindow.GetWeatherRWindowData();

    tData.flTimeElapsed += flDeltaTime;

    SNO snoWeatherToUse = GetWeatherToUse(tRWindow);
    if (snoWeatherToUse == SNO_NONE) {


        sSetWeatherParticleSystem(tData, snoWeatherToUse, fDeleteInsteadOfOutro);
        sSetFogParticleSystem(tData, snoWeatherToUse, fDeleteInsteadOfOutro);

        tData.snoWeatherPrev = snoWeatherToUse;
        sDeleteFeederLights(tData);
        tData.tTargetState = cgtUnfoggedState;
        SoundManager::PlayBkgrndAmbient(snoWeatherToUse, 1, 2.0f, FALSE);
        return;
    }

    ScopedWeatherDefinitionLockR pdefWeather(snoWeatherToUse);



    const BOOL fWasRetro = CWorldIsRetro();

    if (snoWeatherToUse != tData.snoWeatherPrev) {

        tData.snoWeatherPrev = snoWeatherToUse;
        sDeleteFeederLights(tData);
        sSetColorCorrection(tData, pdefWeather->tColorCorrection);








        tData.idFeederLights[ML_SUNLIGHT_SCENE * 2] = sCreateSrcLight(pdefWeather->tParamsNotRaining.snoSunLightScene, pdefWeather->tParamsNotRaining);
        tData.idFeederLights[(ML_SUNLIGHT_SCENE * 2) + 1] = sCreateSrcLight(pdefWeather->tParamsWhenRaining.snoSunLightScene, pdefWeather->tParamsWhenRaining);
        tData.idFeederLights[ML_SUNLIGHT_ACTOR * 2] = sCreateSrcLight(pdefWeather->tParamsNotRaining.snoSunLightActor, pdefWeather->tParamsNotRaining);
        tData.idFeederLights[(ML_SUNLIGHT_ACTOR * 2) + 1] = sCreateSrcLight(pdefWeather->tParamsWhenRaining.snoSunLightActor, pdefWeather->tParamsWhenRaining);


        tData.idFeederLights[ML_PLAYERLIGHT_SCENE * 2] = sCreateSrcLight(pdefWeather->tParamsNotRaining.snoPlayerLightScene, pdefWeather->tParamsNotRaining);
        tData.idFeederLights[(ML_PLAYERLIGHT_SCENE * 2) + 1] = sCreateSrcLight(pdefWeather->tParamsWhenRaining.snoPlayerLightScene, pdefWeather->tParamsWhenRaining);
        tData.idFeederLights[ML_PLAYERLIGHT_ACTOR * 2] = sCreateSrcLight(pdefWeather->tParamsNotRaining.snoPlayerLightActor, pdefWeather->tParamsNotRaining);
        tData.idFeederLights[(ML_PLAYERLIGHT_ACTOR * 2) + 1] = sCreateSrcLight(pdefWeather->tParamsWhenRaining.snoPlayerLightActor, pdefWeather->tParamsWhenRaining);
        tData.idFeederLights[ML_LOCALPLAYERHEADLIGHT * 2] = sCreateSrcLight(pdefWeather->tParamsNotRaining.snoLocalPlayerHeadLight, pdefWeather->tParamsNotRaining);
        tData.idFeederLights[(ML_LOCALPLAYERHEADLIGHT * 2) + 1] = sCreateSrcLight(pdefWeather->tParamsWhenRaining.snoLocalPlayerHeadLight, pdefWeather->tParamsWhenRaining);


        for (int32 i = 0; i < NUM_MANUAL_LIGHTS; i++) {

            Light* ptLight = LightTryToGet(tData.idManualLights[i]);
            if (ptLight) {

                ScopedLightDefinitionLockR pdefManualLight(ptLight->sno);
                Light* ptFeederLight0 = LightTryToGet(tData.idFeederLights[i * 2]);
                Light* ptFeederLight1 = LightTryToGet(tData.idFeederLights[i * 2 + 1]);

                BOOL fExclude = ptFeederLight0 == 0 && ptFeederLight1 == 0;
                if (fExclude)
                    ptLight->dwFlags |= (1 << LIGHT_IS_EXCLUDED_BIT);
                else
                    ptLight->dwFlags &= ~(1 << LIGHT_IS_EXCLUDED_BIT);


                SNO snoLightForIgnoreFlags = ptLight->sno;
                if (ptFeederLight0) {

                    sValidateFeederLightType(snoWeatherToUse, ptFeederLight0->sno, pdefManualLight, i);
                    snoLightForIgnoreFlags = ptFeederLight0->sno;
                }

                else if (ptFeederLight1) {

                    sValidateFeederLightType(snoWeatherToUse, ptFeederLight1->sno, pdefManualLight, i);
                    snoLightForIgnoreFlags = ptFeederLight1->sno;
                }







                if (snoLightForIgnoreFlags != SNO_NONE) {

                    ScopedLightDefinitionLockR pdefLight(snoLightForIgnoreFlags);
                    ptLight->dwDynamicIgnoreFlags = pdefLight->dwIgnoreActorTypeFlags;
                }
            }
        }
    }

    WeatherState tTargetStateNoRain = cgtUnfoggedState;
    WeatherState tTargetStateWithRain = cgtUnfoggedState;

    sUpdateWeatherState(tTargetStateNoRain, pdefWeather->tParamsNotRaining);
    sUpdateWeatherState(tTargetStateWithRain, pdefWeather->tParamsWhenRaining);

    for (int32 i = 0; i < NUM_MANUAL_LIGHTS; i++) {

        Light* ptLight = LightTryToGet(tData.idFeederLights[i * 2]);
        if (ptLight) {

            sUpdateManualLightState(tTargetStateNoRain.tLightState[i], ptLight);
        }
    }
    for (int32 i = 0; i < NUM_MANUAL_LIGHTS; i++) {

        Light* ptLight = LightTryToGet(tData.idFeederLights[i * 2 + 1]);
        if (ptLight) {

            sUpdateManualLightState(tTargetStateWithRain.tLightState[i], ptLight);
        }
    }


    {
        InterpTime tInterpTime;
        tInterpTime.Init();
        tInterpTime.nTimeType = TICK_TIME_LOOPING;
        tInterpTime.flTime = tData.flTimeElapsed;
        tInterpTime.flScale = pdefWeather->dwWeatherDuration * OO_TICKS_PER_SECOND;

        pdefWeather->pathWeatherIntensity.GetInterpolatedValueByTime(1.0f, tInterpTime, tData.flTargetIntensity);
    }

    float flApproachSpeed = pdefWeather->flBlendApproachSpeed;
    float flSoundBlendTime = 2.0f;
    if (!tData.fHasValidCurrentState || (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_SNAP_WEATHER_BIT))) {
        flApproachSpeed = 1.0f;
        flSoundBlendTime = 0.0f;
    }



    tData.flIntensity = Math::Lerp(tData.flIntensity, tData.flTargetIntensity, flApproachSpeed);


    float flIntensity = GetIntensity(tData);
    Math::Lerp(tTargetStateNoRain, tTargetStateWithRain, flIntensity, tData.tTargetState);


    Math::Lerp(tData.tState, tData.tTargetState, flApproachSpeed, tData.tState);
    tData.flDepthOfFieldNear = Math::Lerp(tData.flDepthOfFieldNear, pdefWeather->flDepthOfFieldNear, flApproachSpeed);
    tData.flDepthOfFieldFar = Math::Lerp(tData.flDepthOfFieldFar, pdefWeather->flDepthOfFieldFar, flApproachSpeed);
    tData.flDepthOfFieldMax = Math::Lerp(tData.flDepthOfFieldMax, pdefWeather->flDepthOfFieldMax, flApproachSpeed);

    tData.fHasValidCurrentState = TRUE;

    sApplyWeatherState(tData);
    sSetWeatherParticleSystem(tData, pdefWeather->snoWeatherParticle, fDeleteInsteadOfOutro);
    sSetFogParticleSystem(tData, pdefWeather->snoFogParticle, fDeleteInsteadOfOutro);

    if (pdefWeather->dwFlags & (1 << WEATHER_CAUSES_RIPPLES_BIT)) {

        sCreateRainSplashParticles(tData);
        sCreateRainRippleParticles(tData);
    }

    if (pdefWeather->snoLightning != SNO_NONE) {

        sHandleLightning(tData, pdefWeather, flDeltaTime);
    }
    SoundManager::PlayBkgrndAmbient(pdefWeather->snoSound, 1, flSoundBlendTime, FALSE);

    if (fWasRetro != CWorldIsRetro()) {

        sSnapToTargetState(tData);

        CWorldResetForRetro();
    }
}




float Weather::GetIntensity(const WeatherRWindowData & tData) {
    if (CGameGetGameCommonData().GameModeGet() == GAMEMODE_PLAYING) {
        RActor * ptRActor = RActorTryToGet(LocalPlayerRActorGet());
        if (ptRActor) {
            WorldCommonData & tWorldData = WorldGetCommonData(ptRActor->tLocation.tWorldLocation.idSWorld);
            if (tWorldData.tWorldSyncedData.dwWeatherIntensityOverrideEnd > CGameGetPredictedServerTime()) {
                return tWorldData.tWorldSyncedData.flWeatherIntensityOverride;
            }
        }
        return tData.flIntensity;
    }
    return 0.0f;
}

















void Weather::Warped(RWindow & tRWindow) {
    WeatherRWindowData & tData = tRWindow.GetWeatherRWindowData();
    if (tData.idFogParticle != PARTICLESYSTEMID_INVALID)
        sParticleSystemDeleteOrOutro(tData.idFogParticle, TRUE);
    tData.snoFogParticle = SNO_NONE;
    Update(tRWindow, 0.0f, TRUE);
    tData.tState = tData.tTargetState;
    tData.flIntensity = tData.flTargetIntensity;
    tData.tColorCorrectionPrev = tData.tColorCorrection;
    tData.tColorCorrectionPrev2 = tData.tColorCorrection;
}
















void Weather::TriggerLightning(WeatherRWindowData & tData) {
    tData.flLightningCountdown = 0.0f;
}






LightID Weather::GetLocalPlayerHeadlight(const WeatherRWindowData & tData) {
    return tData.idManualLights[ML_LOCALPLAYERHEADLIGHT];
}






LightID Weather::GetSunLightScene(const WeatherRWindowData & tData) {
    if (tData.idFeederLights[0] == -1 && tData.idFeederLights[1] == -1) {
        return -1;
    }
    return tData.idManualLights[ML_SUNLIGHT_SCENE];
}










void Weather::GetShadowColor(const WeatherRWindowData & tData, float & flShadowFactor, float & flAOFactor, float & flAOShadowFactor) {
    flShadowFactor = tData.tState.flShadowFactor;
    flAOFactor = tData.tState.flAOFactor;
    flAOShadowFactor = tData.tState.flAOShadowFactor;
}









float Weather::GetNormalMapFactor(const WeatherRWindowData & tData) {
    return tData.tState.flNormalMapFactor;
}








BOOL Weather::GetFogColor(const WeatherRWindowData & tData, RGBAColor & rgbaFogColor) {
    if (tData.snoWeatherPrev == SNO_NONE) {
        return FALSE;
    }
    rgbaFogColor.Set(tData.tState.rgbavalFogColor);
    return TRUE;
}








void Weather::GetDepthOfField(const WeatherRWindowData & tData, float & flDepthOfFieldNear, float & flDepthOfFieldFar, float & flDepthOfFieldMax) {
    flDepthOfFieldNear = tData.flDepthOfFieldNear;
    flDepthOfFieldFar = tData.flDepthOfFieldFar;
    flDepthOfFieldMax = tData.flDepthOfFieldMax;
}









void Weather::GetColorCorrection(WeatherRWindowData & tData, ColorCorrectionParams ** pptParams, ColorCorrectionParams ** pptParamsPrev, ColorCorrectionParams ** pptParamsPrev2) {
    *pptParams = &tData.tColorCorrection;
    *pptParamsPrev = &tData.tColorCorrectionPrev;
    *pptParamsPrev2 = &tData.tColorCorrectionPrev2;
}









void Weather::AddWeatherVolumeCylinder(WeatherRWindowData & tData, const AxialCylinder & cyl, const SNO snoSource, const SNO snoQuestRange) {
    WeatherVolume tVolume;
    tVolume.eType = Shape::AXIAL_CYLINDER;
    tVolume.cyl = cyl;
    tVolume.snoWeather = snoSource;
    tVolume.snoQuestRange = snoQuestRange;
    tData.tWeatherVolumes.push_back(tVolume);
}









void Weather::AddWeatherVolumeOBB(WeatherRWindowData & tData, const OBB & obb, const SNO snoSource, const SNO snoQuestRange) {
    WeatherVolume tVolume;
    tVolume.eType = Shape::BOX;
    tVolume.obb = obb;
    tVolume.snoWeather = snoSource;
    tVolume.snoQuestRange = snoQuestRange;
    tData.tWeatherVolumes.push_back(tVolume);
}









void Weather::PrepareRender(WeatherRWindowData & tData) {
    WeatherRenderData & tRenderData = tData.tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    tRenderData.flIntensity = GetIntensity(tData);
    tRenderData.tFogState = tData.tRenderFogState;
}






const WeatherRenderData * Weather::GetRenderData(const WeatherRWindowData & tData) {
    return &tData.tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];
}
