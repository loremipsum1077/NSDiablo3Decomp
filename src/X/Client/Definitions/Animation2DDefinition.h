// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/Animation2DDefinition.h
// Subset: Anim2DFrame, Animation2DDefinition and the group definition pointer that
// X/Client/Animation2D.cpp reads.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Gfx/GfxCommon.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

typedef int32 ImageCollectionType;

enum {
    ANIM_FRAME_NAME_LENGTH = 64,
};

struct Anim2DFrame {  // size 0x44
    CHAR szName[64];  // 0x00
    RGBAColor rgbaColor;  // 0x40
};
static_assert(sizeof(Anim2DFrame) == 0x44, "Anim2DFrame");

struct Animation2DDefinition {  // size 0x40
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    ImageCollectionType eCollection;  // 0x10
    FrameAnim tFrameAnim;  // 0x14
    SNO snoSound;  // 0x28
    int32 nFrameCount;  // 0x2c
    SerializeData serFrames;  // 0x30
    XPtr64<Anim2DFrame> ptFrames;  // 0x38
};
static_assert(sizeof(Animation2DDefinition) == 0x40, "Animation2DDefinition");

class Animation2DGroupDef : public GroupDefinition {  // size 0xa0
public:
    Animation2DGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5, line 56; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 59
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7, line 61
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(Animation2DGroupDef) == 0xa0, "Animation2DGroupDef");

extern Animation2DGroupDef * gptGroupDefAnimation2D;  // Animation2DDefinition.cpp:52
