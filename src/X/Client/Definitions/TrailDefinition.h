// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/TrailDefinition.h
// X/Common/Game.cpp registers the group definition (GroupDefinition::Create<TrailGroupDef>).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../Gfx/GfxMaterial.h"
#include "../InterpolationTrack.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
class XMemoryAllocatorInterface;
class XTypeDescriptor;

enum TrailFlags {
    TRAILDEF_DELETE_AT_ANIMATION_END_BIT = 0,
    TRAILDEF_INFINITE_DURATION_BIT = 1,
    TRAILDEF_SCALE_TEXTURE_TO_LENGTH = 2,
    TRAILDEF_EDGE_ALPHA = 3,
};

enum {
    TRAIL_LINE = 0,
    TRAIL_CROSS = 1,
    TRAIL_CIRCLE = 2,
};

typedef int32 TrailType;

struct TrailDefinition {  // size 0x158
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwTrailDefFlags;  // 0x0c
    int32 nLength;  // 0x10
    int32 nLengthDelta;  // 0x14
    TickTime tSystemDurationMin;  // 0x18
    TickTime tSystemDurationDelta;  // 0x1c
    TickTime tOutroDurationMin;  // 0x20
    TickTime tOutroDurationDelta;  // 0x24
    ColorPath pathColorOverTime;  // 0x28
    ColorPath pathColorOverLength;  // 0x58
    FloatPath pathAlphaOverTime;  // 0x88
    FloatPath pathAlphaOverLength;  // 0xb8
    TrailType nTrailType;  // 0xe8
    int32 nCircleSegments;  // 0xec
    UberMaterial tUberMaterial;  // 0xf0
};
static_assert(sizeof(TrailDefinition) == 0x158, "TrailDefinition");

class TrailGroupDef : public GroupDefinition {  // size 0xa0, line 69
public:
    TrailGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 74; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 77
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 79
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(TrailGroupDef) == 0xa0, "TrailGroupDef");

extern TrailGroupDef * gptGroupDefTrail;  // TrailDefinition.cpp:67

extern const XTypeDescriptor * DT_TRAIL;  // TrailDefinition.cpp:61

typedef ScopedDefinitionLockR<TrailDefinition> ScopedTrailDefinitionLockR;

template <> inline const TrailDefinition * DefinitionLockR<TrailDefinition>(SNO sno) {  // line 104
    return (const TrailDefinition *)gptGroupDefTrail->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<TrailDefinition>(const TrailDefinition * pdef) {  // line 105
    void * lpDef = (void *)pdef;
    gptGroupDefTrail->UnlockR(lpDef);
}
