#pragma once

#include "../CommonTypes.h"
#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/SigmaTypes.h"

struct InactiveMarkerOwner;
struct Marker;
struct PRTransform;

class CameraMarker : public MarkerBase {  // size 0x18
public:

    CameraMarker() : MarkerBase(OBSERVER_SNO, MARKERTYPE_CAMERA, "Camera") {}


    // ~CameraMarker is implicit (the DWARF marks it artificial); it is virtual
    // because ~MarkerBase is, so its deleting destructor is this file's 0x71006cae40.
    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3


    virtual BOOL IsForClient(const Marker &) {  // vtable slot 6

        return TRUE;
    }









    virtual DataID CreateForClientGame(const SWorldID idSWorld, const SSceneID idSScene, const PRTransform & transform, const Marker & tMarker, const InactiveMarkerOwner & tMarkerOwner);  // vtable slot 14






    virtual void Draw(const Marker & tMarker, const PRTransform & transform, const BOOL bSelected);  // vtable slot 15
};
static_assert(sizeof(CameraMarker) == 0x18, "CameraMarker");
