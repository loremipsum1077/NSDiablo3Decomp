// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ShaderMapDefinition.h
// X/Common/Game.cpp registers the group definition (GroupDefinition::Create<ShaderMapGroupDef>).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include <stddef.h>
#include <stdint.h>

class CTagMap;
class XMemoryAllocatorInterface;
class XTypeDescriptor;

struct ShaderMapDefinition {  // size 0x20
    RequiredDefinitionHeader tHeader;  // 0x00
    uint32 dwFlags;  // 0x0c
    SerializeData serTagMap;  // 0x10
    XPtr64<CTagMap> hTagMap;  // 0x18
};
static_assert(sizeof(ShaderMapDefinition) == 0x20, "ShaderMapDefinition");

class ShaderMapGroupDef : public GroupDefinition {  // size 0xa0, line 34
public:
    ShaderMapGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 39; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13, line 42
    virtual SNOPackType GetSNOPackDestination(const void *) const {  // vtable slot 7, line 44
        return SNOPACKTYPE_CLIENTDATA;
    }
};
static_assert(sizeof(ShaderMapGroupDef) == 0xa0, "ShaderMapGroupDef");

extern ShaderMapGroupDef * gptGroupDefShaderMap;  // ShaderMapDefinition.cpp:53

extern const XTypeDescriptor * DT_SHADERMAP;  // ShaderMapDefinition.cpp:47

typedef ScopedDefinitionLockR<ShaderMapDefinition> ScopedShaderMapDefinitionLockR;
// Original inline specializations at line 68; shader maps use the resident definition.
template <> inline const ShaderMapDefinition * DefinitionLockR<ShaderMapDefinition>(SNO sno) {
    return static_cast<const ShaderMapDefinition *>(gptGroupDefShaderMap->LockRFast(sno));
}
template <> inline void DefinitionUnlockR<ShaderMapDefinition>(const ShaderMapDefinition *) {}

SNO ShaderMapGetDefaultShader(const SNO snoShaderMap, const BOOL fComplainIfFallbackIsMissing, const ShaderMapDefinition * pdefShaderMap);
SNO ShaderMapGetFadeShader(const SNO snoShaderMap, const ShaderMapDefinition * pdefShaderMap);
SNO ShaderMapGetGhostZPassShader(const ShaderMapDefinition * pdefShaderMap);
SNO ShaderMapGetHighlightShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned);
SNO ShaderMapGetOccludedShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned);
SNO ShaderMapGetPrepassShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned);
SNO ShaderMapGetMaskShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned);
SNO ShaderMapGetCookieShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fHasAnimation, const BOOL fTinted, const BOOL fIsCloth, const BOOL fIsBanner);
BOOL ShaderMapIsReflective(const SNO snoShaderMap);
