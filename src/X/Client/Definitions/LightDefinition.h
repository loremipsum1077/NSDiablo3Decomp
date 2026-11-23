// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/LightDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<LightGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../Gfx/GfxCommon.h"
#include "../Gfx/GfxMaterial.h"
#include "../InterpolationTrack.h"

class XMemoryAllocatorInterface;
struct FileReference;
class XTypeDescriptor;

enum {
    LIGHT_DEF_INFINITE_DURATION_BIT = 0,
    LIGHT_DEF_TIME_OF_DAY_BIT_DEPRECATED = 1,
    LIGHT_DEF_CROSSFADE_WITH_PLAYER_DIST_BIT = 2,
    LIGHT_DEF_UNUSED3_BIT = 3,
    LIGHT_DEF_UNUSED4_BIT = 4,
    LIGHT_DEF_UNUSED5_BIT = 5,
    LIGHT_DEF_UNUSED6_BIT = 6,
    LIGHT_DEF_UNUSED7_BIT = 7,
    LIGHT_DEF_UNUSED8_BIT = 8,
    LIGHT_DEF_RANDOM_TIME_OFFSET_BIT = 9,
    LIGHT_DEF_DISABLED_IN_OPEN_WORLD_BIT = 10,
    LIGHT_DEF_DISABLED_IN_DUNGEONFINDER_BIT = 11,
};

enum {
    LIGHTIGNORE_ACTORS_BIT = 0,
    LIGHTIGNORE_INTERIORS_BIT = 1,
    LIGHTIGNORE_EXTERIORS_BIT = 3,
    LIGHTIGNORE_SCENES_AND_SCENE_ACTORS_BIT = 4,
};

struct LightStateParams {  // size 0x270
    FloatPath pathInnerRadius;  // 0x00
    FloatPath pathRadius;  // 0x30
    FloatPath pathAttenuation[3];  // 0x60
    AnglePath pathHotspot;  // 0xf0
    AnglePath pathFalloff;  // 0x120
    ColorPath pathAmbientColor;  // 0x150
    FloatPath pathAmbientIntensity;  // 0x180
    ColorPath pathColor;  // 0x1b0
    FloatPath pathIntensity;  // 0x1e0
    ColorPath pathSpecularColor;  // 0x210
    FloatPath pathSpecularIntensity;  // 0x240
    void GetDefaultData();
};
static_assert(sizeof(LightStateParams) == 0x270, "LightStateParams");

struct LightDefinition {  // size 0x520
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    uint32 dwIgnoreActorTypeFlags;  // 0x10
    LightType eType;  // 0x14
    MaterialTexture tMatTexProjected;  // 0x18
    TickTime nSystemDuration;  // 0x30
    int32 nPriority;  // 0x34
    float flMaxPlayerDist;  // 0x38
    LightStateParams tStateA;  // 0x40
    LightStateParams tStateB;  // 0x2b0
};
static_assert(sizeof(LightDefinition) == 0x520, "LightDefinition");

class LightGroupDef : public GroupDefinition {  // size 0xa0
public:
    LightGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 101
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_CLIENTDATA; }  // vtable slot 7, line 103
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
};
static_assert(sizeof(LightGroupDef) == 0xa0, "LightGroupDef");

extern LightGroupDef * gptGroupDefLight;  // LightDefinition.cpp

extern const XTypeDescriptor * DT_LIGHTSTATEPARAMS;  // LightDefinition.cpp:57
extern const XTypeDescriptor * DT_LIGHT;  // LightDefinition.cpp:73

typedef ScopedDefinitionLockR<LightDefinition> ScopedLightDefinitionLockR;

template <> inline const LightDefinition * DefinitionLockR<LightDefinition>(SNO sno) {  // line 129
    return (const LightDefinition *)gptGroupDefLight->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<LightDefinition>(const LightDefinition * pdef) {  // line 130
    void * lpDef = (void *)pdef;
    gptGroupDefLight->UnlockR(lpDef);
}
