// Original file: X/Common/TagMaps/SurfaceTags.h
#pragma once
#include "TagNumbers.h"

enum {
    TAG_SURFACE_SPLASH_PARTICLE_SYSTEM = 327680,
    TAG_SURFACE_SPLASH_RIPPLE_PARTICLE_SYSTEM = 327681,
    TAG_SURFACE_FOOTSTEP_PARTICLE_SYSTEM = 327684,
    TAG_SURFACE_RESIDUAL_FOOTSTEP_PARTICLE_SYSTEM = 327685,
};

// External definitions in SurfaceTags.cpp; original DWARF variable inventory.
struct AxeTagInfo;
extern const AxeTagInfo tSurfaceTagInfo[];
extern const int32 gnSurfaceTagCount;
