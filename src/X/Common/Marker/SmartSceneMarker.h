// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Marker/SmartSceneMarker.h
// SceneMarker adds no data members to MarkerBase, so its size stays 0x18.
// The DWARF puts the class on line 13, the constructor on line 16 and
// GetActorForEdit (vtable slot 3) on line 21.

#pragma once

#include "SmartMarkerBase.h"
#include "../SNO/SNOFiles.h"


class SceneMarker : public MarkerBase {  // size 0x18
public:

    SceneMarker() : MarkerBase(SCENE_SNO, MARKERTYPE_SCENE, "Scene") {}

    // ~SceneMarker is implicit (the DWARF marks it artificial); it is virtual
    // because ~MarkerBase is.

    virtual SNO GetActorForEdit(SNO snoSource);  // vtable slot 3
};
static_assert(sizeof(SceneMarker) == 0x18, "SceneMarker");
