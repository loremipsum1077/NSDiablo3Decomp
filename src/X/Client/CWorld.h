// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/CWorld.h
// AudioVolume sits on line 30 and CWorldGet on line 146 (2.6.2 DWARF). The function declarations
// are for X/Client/CWorld.cpp; the DWARF gives their signatures but not the header declaring them.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "CGame.h"
#include "../Common/CommonTypes.h"
#include "../Common/World.h"
#include "../Sigma/Math/Shape.h"

// Defined in X/Client/CWorld.cpp:1390.
BOOL CWorldIsRetro();

struct Marker;

struct AudioSettings {  // size 0xc
    SNO snoMusic;  // 0x00
    SNO snoAmbient;  // 0x04
    SNO snoReverb;  // 0x08
};
static_assert(sizeof(AudioSettings) == 0xc, "AudioSettings");

// Defined in X/Client/CWorld.cpp:1575.
void CWorldAddAudioVolume(const Shape & shape, const Marker & tMarker, const AudioSettings & tSettings, const SNO snoOnEntryEffectGroup);

struct AudioVolume {  // size 0x150
    Shape shape;  // 0x00
    uint32 tMarkerName;  // 0x128
    AudioSettings tSettings;  // 0x12c
    SNO snoOnEntryEffectGroup;  // 0x138
    BOOL fLinkedToGizmo;  // 0x13c
    ACDID idACDGizmoParent;  // 0x140
    float flLastResolveTime;  // 0x144
    uint32 tGizmoMarkerName;  // 0x148
};
static_assert(sizeof(AudioVolume) == 0x150, "AudioVolume");

typedef blz::vector<AudioVolume, blz::allocator<AudioVolume> > AudioVolumeArray;

struct CWorldGlobals {  // size 0x30
    SNO snoCurrentMusic;  // 0x00
    SNO snoIntroMusic;  // 0x04
    BOOL fMusicDisabled;  // 0x08
    SNO snoCurrentAmbient;  // 0x0c
    SNO snoCurrentReverb;  // 0x10
    AmbientSoundID idAmbient;  // 0x14
    AudioVolumeArray tAudioVolumes;  // 0x18
};
static_assert(sizeof(CWorldGlobals) == 0x30, "CWorldGlobals");

typedef int32 LightID;

enum {
    CWORLDID_INVALID = -1,
};

typedef int32 CWorldID;

struct CWorld {  // size 0x98
    CWorldID id;  // 0x00
    WorldCommonData tWCD;  // 0x08
    SNO snoIrradianceTex;  // 0x78
    SNO snoIrradianceTexDead;  // 0x7c
    float flDeformationScale;  // 0x80
    int32 nLightCount;  // 0x84
    LightID * pidLights;  // 0x88
    float flIntroMusicNextTime;  // 0x90
};
static_assert(sizeof(CWorld) == 0x98, "CWorld");

// Defined in X/Client/CWorld.cpp.
void CWorldEnter(SWorldID idSWorld, GameMode eMode);  // line 90
void CWorldLeave();  // line 310
void ClientWorldDispose(CWorldID idCWorld);  // line 373
CWorld * CWorldGetFromSWorldID(SWorldID idSWorld);  // line 1052


































































inline CWorld * CWorldGet(CWorldID idCWorld)  // line 146
{

    return CGameGet().ptCWorlds->ElementGet(idCWorld);
}
