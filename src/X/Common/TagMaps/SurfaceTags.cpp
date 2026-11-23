// Original file: X/Common/TagMaps/SurfaceTags.cpp

#include "SurfaceTags.h"
#include "TagNumbers.h"















const AxeTagInfo tSurfaceTagInfo[4] = {
    { TAG_SURFACE_SPLASH_PARTICLE_SYSTEM, AXETAGTYPE_PARTICLE, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_MISC, -1, -1, -1 },
    { TAG_SURFACE_SPLASH_RIPPLE_PARTICLE_SYSTEM, AXETAGTYPE_PARTICLE, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_MISC, -1, -1, -1 },
    { TAG_SURFACE_FOOTSTEP_PARTICLE_SYSTEM, AXETAGTYPE_PARTICLE, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_MISC, -1, -1, -1 },
    { TAG_SURFACE_RESIDUAL_FOOTSTEP_PARTICLE_SYSTEM, AXETAGTYPE_PARTICLE, (void *)&TAG_VALUE_NULL_SNO, 0, "", "", "", AXETAGGROUP_MISC, -1, -1, -1 },
};

const int32 gnSurfaceTagCount = sizeof(tSurfaceTagInfo) / sizeof(tSurfaceTagInfo[0]);
