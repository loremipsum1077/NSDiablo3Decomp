// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartWeatherVolumeMarker.h
// The DWARF records ~WeatherVolumeMarker (line 13, implicit; its deleting destructor is
// _ZN19WeatherVolumeMarkerD0Ev 0x710064c640), the constructor (line 16, inlined into
// SmartMarkerGlobals' constructor) and IsForClient (line 28) as defined in this header.

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

class WeatherVolumeMarker : public MarkerBase {  // size 0x18
public:
    WeatherVolumeMarker() : MarkerBase(WEATHER_SNO, MARKERTYPE_WEATHERVOLUME, "Weather Volume") {}
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
    virtual void GetAABB(const Marker & tMarker, AABB & aabbBounds);  // vtable slot 5
    virtual BOOL IsForClient(const Marker & tMarker) { return TRUE; }  // vtable slot 6; header line 28
    virtual void GetMarkerBounds(const Marker & tMarker, PRTransform & transform, Sphere & wsBounds);  // vtable slot 4
    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14
    virtual void Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected);  // vtable slot 15
private:
    void sGetShape(const Marker & tMarker, const PRTransform & transform, Shape & shape);
};
static_assert(sizeof(WeatherVolumeMarker) == 0x18, "WeatherVolumeMarker");
