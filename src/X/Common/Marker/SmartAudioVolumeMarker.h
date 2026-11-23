// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartAudioVolumeMarker.h

#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct AABB;
struct InactiveMarkerOwner;
struct Marker;
struct PRTransform;
struct Shape;
struct Sphere;

class AudioVolumeMarker : public MarkerBase {  // size 0x18
public:
    AudioVolumeMarker() : MarkerBase(SNOGROUP_NONE, MARKERTYPE_AUDIOVOLUME, "Audio Volume") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5
    virtual BOOL IsForClient(const Marker & tMarker) { return TRUE; }  // vtable slot 6
    virtual void GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds);  // vtable slot 4
    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner &);  // vtable slot 14
    virtual void Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected);  // vtable slot 15
private:
    void sGetShape(const Marker & tMarker, const PRTransform & transform, Shape & shape);
};
static_assert(sizeof(AudioVolumeMarker) == 0x18, "AudioVolumeMarker");


// Functions defined in this file (bodies are not in the DWARF):
//   line    13  AudioVolumeMarker::~AudioVolumeMarker()  [0x71006cad90]
//   line    16  AudioVolumeMarker::AudioVolumeMarker()  [inlined into 1 functions]
//   line    28  BOOL AudioVolumeMarker::IsForClient(const Marker & tMarker)  [0x71006cad80]
