#include "../Definitions/MarkerSetDefinition.h"
#include "SmartMarker.h"
#include "SmartActorMarker.h"
#include "SmartAdventureMarker.h"
#include "SmartAmbientSoundMarker.h"
#include "SmartAudioVolumeMarker.h"
#include "SmartCameraMarker.h"
#include "SmartConductorMarker.h"
#include "SmartEffectGroupMarker.h"
#include "SmartEncounterMarker.h"
#include "SmartEventMarker.h"
#include "SmartGizmoLocationMarker.h"
#include "SmartLightMarker.h"
#include "SmartMinimapMarker.h"
#include "SmartParticleSystemMarker.h"
#include "SmartSceneGroupMarker.h"
#include "SmartSceneLocationMarker.h"
#include "SmartSceneMarker.h"
#include "SmartScriptMarker.h"
#include "SmartWeatherVolumeMarker.h"
class SmartMarkerGlobals {  // size 0x358
public:
    SmartMarkerGlobals()
    {
        m_fInitialized = FALSE;
        for (int32 i = 0; i < 52; ++i)
            ptGizmoLocationMarkers[i] = NULL;
    }




    ~SmartMarkerGlobals()
    {
        for (int32 i = 0; i < 52; ++i)
        {
            if (ptGizmoLocationMarkers[i])
                ::delete ptGizmoLocationMarkers[i];
            ptGizmoLocationMarkers[i] = NULL;
        }
    }
    void Init()
    {
        m_fInitialized = TRUE;
        for (int32 i = 0; i < 52; ++i)
            ptGizmoLocationMarkers[i] = new GizmoLocationMarker(i);
    }



    BOOL IsInitialized()
    {

        return m_fInitialized;
    }

    MarkerBase * GetMarkerObjectFromType(MarkerType eType)
    {

        if (eType >= MARKERTYPE_GIZMOLOC && eType < MARKERTYPE_GIZMOLOC + 52)
            return ptGizmoLocationMarkers[eType - MARKERTYPE_GIZMOLOC];

        switch (eType)
        {
        case MARKERTYPE_NONE: return &tBaseMarker;
        case MARKERTYPE_ACTOR: return &tActorMarker;
        case MARKERTYPE_LIGHT: return &tLightMarker;
        case MARKERTYPE_CAMERA: return &tCameraMarker;
        case MARKERTYPE_AUDIOVOLUME: return &tAudioVolumeMarker;
        case MARKERTYPE_WEATHERVOLUME: return &tWeatherVolumeMarker;
        case MARKERTYPE_AMBIENTSOUND: return &tAmbientSoundMarker;
        case MARKERTYPE_PARTICLESYSTEM: return &tParticleSystemMarker;
        case MARKERTYPE_SCENE: return &tSceneMarker;
        case MARKERTYPE_ENCOUNTER: return &tEncounterMarker;
        case MARKERTYPE_SCENEGROUP: return &tSceneGroupMarker;
        case MARKERTYPE_SCRIPT: return &tScriptMarker;
        case MARKERTYPE_ADVENTURE: return &tAdventureMarker;
        case MARKERTYPE_SCENE_LOCATION: return &tSceneLocationMarker;
        case MARKERTYPE_MINIMAP: return &tMinimapMarker;
        case MARKERTYPE_EVENT: return &tEventMarker;
        case MARKERTYPE_CONDUCTOR: return &tConductorMarker;
        case MARKERTYPE_EFFECTGROUP: return &tEffectGroupMarker;
        default: return NULL;
        }
    }

private:
    BOOL m_fInitialized;  // 0x00
    MarkerBase tBaseMarker;  // 0x08
    ActorMarker tActorMarker;  // 0x20
    CameraMarker tCameraMarker;  // 0x38
    AudioVolumeMarker tAudioVolumeMarker;  // 0x50
    WeatherVolumeMarker tWeatherVolumeMarker;  // 0x68
    AmbientSoundMarker tAmbientSoundMarker;  // 0x80
    LightMarker tLightMarker;  // 0x98
    EffectGroupMarker tEffectGroupMarker;  // 0xb0
    ParticleSystemMarker tParticleSystemMarker;  // 0xc8
    EncounterMarker tEncounterMarker;  // 0xe0
    SceneGroupMarker tSceneGroupMarker;  // 0xf8
    SceneMarker tSceneMarker;  // 0x110
    ScriptMarker tScriptMarker;  // 0x128
    AdventureMarker tAdventureMarker;  // 0x140
    EventMarker tEventMarker;  // 0x158
    MinimapMarker tMinimapMarker;  // 0x170
    SceneLocationMarker tSceneLocationMarker;  // 0x188
    ConductorMarker tConductorMarker;  // 0x1a0
    GizmoLocationMarker * ptGizmoLocationMarkers[52];  // 0x1b8
};
static_assert(sizeof(SmartMarkerGlobals) == 0x358, "SmartMarkerGlobals");
#include "../../Client/Gfx/GfxVB.h"  // was reached through SmartMarker.h:18 (the original SmartMarker.h does not include it)






































SmartMarkerGlobals tSmartMarkerGlobals;










SmartMarker::SmartMarker(const Marker & tMarker)
{
    m_pMarker = (Marker *)&tMarker;
    m_pInterface = GetMarkerObjectFromType(tMarker.eType);
}


















SmartMarker::SmartMarker(const MarkerSetDefinition * pdefMarkerSet, int32 nMarkerIndex)
    : m_pMarker(NULL), m_pInterface(NULL)
{
    MarkerSetDefinition * pdefMarkerSetTemp = (MarkerSetDefinition *)pdefMarkerSet;
    m_pMarker = &pdefMarkerSetTemp->tMarkerSet[nMarkerIndex];
    m_pInterface = GetMarkerObjectFromType(m_pMarker->eType);
}


















MarkerBase * SmartMarker::GetMarkerObjectFromType(MarkerType eType)
{
    if (!tSmartMarkerGlobals.IsInitialized())
        tSmartMarkerGlobals.Init();
    return tSmartMarkerGlobals.GetMarkerObjectFromType(eType);
}




Marker & MarkerGet(SmartMarker & sMarker)
{
    return *sMarker.m_pMarker;
}

const Marker & MarkerGet(const SmartMarker & sMarker)
{
    return *sMarker.m_pMarker;
}









SmartMarker::SmartMarker(MarkerType eType)
    : m_pMarker(NULL)
{
    m_pInterface = GetMarkerObjectFromType(eType);
}
