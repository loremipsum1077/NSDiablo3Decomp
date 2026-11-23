// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/RopeDefinition.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Gfx/GfxMaterial.h"
#include "../InterpolationTrack.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
struct MsgTriggeredEvent;
class XMemoryAllocatorInterface;
class XTypeDescriptor;

enum {
    ROPEDEF_WEIGHT_END_BIT = 0,
    ROPEDEF_CONSTRAIN_TO_HARDPOINT_ORIENTATION_BIT = 1,
    ROPEDEF_DELETE_AT_ANIMATION_END_BIT = 2,
    ROPEDEF_INFINITE_DURATION_BIT = 3,
    ROPEDEF_TEXTURE_REPEAT_COUNT_IS_PER_FOOT = 4,
    ROPEDEF_USE_KNOT_COUNT_PER_FOOT = 5,
    ROPEDEF_SNAP_DESTINATION_TO_RENDERED_FLOOR_BIT = 6,
    ROPEDEF_SCALE_JITTER_BY_ROPE_LENGTH_BIT = 7,
    ROPEDEF_PICK_MAX_ENDPOINT_ALPHA_BIT = 8,
};

enum {
    ROPE_HARDPOINT_HEAD = 0,
    ROPE_HARDPOINT_TAIL = 1,
    ROPE_HARDPOINT_KNOTS = 2,
};

enum {
    ROPE_MSGKEY_CREATE = 4000,
    ROPE_MSGKEY_DELETE = 4001,
};

struct RopeDefinition {  // size 0x460
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwRopeFlags;  // 0x0c
    int32 nWidth;  // 0x10
    float flKnotsPerFoot;  // 0x14
    float flMass;  // 0x18
    float flMassTaperFactor;  // 0x1c
    float flMassEnd;  // 0x20
    float fTextureRepeatCount;  // 0x24
    float flStiffness;  // 0x28
    UberMaterial tUberMaterial;  // 0x30
    SNO snoPhysics;  // 0x98
    TickTime tSystemDurationMin;  // 0x9c
    TickTime tSystemDurationDelta;  // 0xa0
    TickTime tOutroDurationMin;  // 0xa4
    TickTime tOutroDurationDelta;  // 0xa8
    ColorPath pathColorOverTime;  // 0xb0
    ColorPath pathColorOverLength;  // 0xe0
    FloatPath pathAlphaOverTime;  // 0x110
    FloatPath pathAlphaOverLength;  // 0x140
    FloatPath pathWidthOverTime;  // 0x170
    FloatPath pathWidthOverLength;  // 0x1a0
    FloatPath pathSourcePointWorldUpMotion;  // 0x1d0
    FloatPath pathSourcePointAlongRopeMotion;  // 0x200
    FloatPath pathSourcePointPerpendicularMotion;  // 0x230
    FloatPath pathDestPointWorldUpMotion;  // 0x260
    FloatPath pathDestPointAlongRopeMotion;  // 0x290
    FloatPath pathDestPointPerpendicularMotion;  // 0x2c0
    VectorPath pathJitterAmplitude;  // 0x2f0
    FloatPath pathJitterRate;  // 0x320
    float flJitterApproachSpeed;  // 0x350
    float flExpectedLength;  // 0x354
    FloatPath pathSeparation;  // 0x358
    FloatPath pathXAmpOverLength;  // 0x388
    FloatPath pathYAmpOverLength;  // 0x3b8
    FloatPath pathFreqOverTime;  // 0x3e8
    FloatPath pathPhaseSpeedOverTime;  // 0x418
    SerializeData serMsgTriggeredEvents;  // 0x448
    int32 nMsgTriggeredEventCount;  // 0x450
    XPtr64<MsgTriggeredEvent> ptMsgTriggeredEvents;  // 0x458
};
static_assert(sizeof(RopeDefinition) == 0x460, "RopeDefinition");

class RopeGroupDef : public GroupDefinition {  // size 0xa0, line 107
public:
    RopeGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 112; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 115
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 117
        return SNOPACKTYPE_CLIENTDATA;
    }
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(RopeGroupDef) == 0xa0, "RopeGroupDef");

extern RopeGroupDef * gptGroupDefRope;  // RopeDefinition.cpp:95

extern const XTypeDescriptor * DT_ROPE;  // RopeDefinition.cpp:89

typedef ScopedDefinitionLockR<RopeDefinition> ScopedRopeDefinitionLockR;

// Lines 147 and 148; the DWARF marks both inlined into RopeAdd and RopeRenderInDisplayList.
// Bodies follow the other definition types (BossEncounterDefinition.h:67): lock, unlock.
template <> inline const RopeDefinition * DefinitionLockR<RopeDefinition>(SNO sno) {  // line 147
    return (const RopeDefinition *)gptGroupDefRope->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<RopeDefinition>(const RopeDefinition * pdef) {  // line 148
    gptGroupDefRope->UnlockR((void *&)pdef);
}
