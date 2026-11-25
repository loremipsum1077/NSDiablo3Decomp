// Original file: X/Client/Definitions/ShaderMapDefinition.cpp

#include "ShaderMapDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../../Common/WorldPlace.h"
// For the RGBAColor normals, which the original also initializes here; it reaches GfxVB.h
// through an include chain the DWARF does not record.
#include "../Gfx/GfxVB.h"
#include "ShaderDefinition.h"
#include "PreferencesDefinition.h"
#include "../Gfx/GfxCommon.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/TagMaps/ShaderMapTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>
// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

enum {  // line 25
    SHADERMAP_COMMON_SNO_VERSION = 26,
    SHADERMAP_XML_VERSION = 2,
    SHADERMAP_PC_SNO_VERSION = 26,
    SHADERMAP_CONSOLE_SNO_VERSION = 30,
    SHADERMAP_AXE_SNO_VERSION = 28,
};

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Client/Gfx/Gfx.cpp; no adopted header declares them yet.
HardwareClass GfxGetHWClass();
BOOL GfxGetMSAAEnabled();



static const TypeDescriptorField ShaderMapDefinition_Fields[4] = {  // line 44
    TypeDescriptorField("", DT_INT, offsetof(ShaderMapDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ShaderMapDefinition, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(ShaderMapDefinition, hTagMap), 0x21,
                              (int32)offsetof(ShaderMapDefinition, serTagMap) - (int32)offsetof(ShaderMapDefinition, hTagMap), TAG_SHADERMAP),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ShaderMapDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ShaderMapDefinition("ShaderMap", SHADERMAP_XML_VERSION, ShaderMapDefinition_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SHADERMAP = &TypeDescriptor_ShaderMapDefinition;

ShaderMapGroupDef * gptGroupDefShaderMap;  // line 53

// line 59
ShaderMapGroupDef::ShaderMapGroupDef()
    : GroupDefinition("ShaderMap", SHADERMAP_SNO,
                      SNOVersions(SHADERMAP_PC_SNO_VERSION,
                                  SHADERMAP_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  SHADERMAP_CONSOLE_SNO_VERSION,
                                  SHADERMAP_CONSOLE_SNO_VERSION,
                                  SHADERMAP_CONSOLE_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(ShaderMapDefinition), DT_SHADERMAP, 0x2005, 0x1a) {
}

// line 81
uint32 ShaderMapGroupDef::GetMaxCount() {
    return 740;
}

































































































































































// line 195
static SNO sGetReflectionShader(CTagMap * hTagMap, const HardwareClass eHWClass, int nReflectionQuality) {
    switch (eHWClass) {
        case HARDWARECLASS_HIGH:
        case HARDWARECLASS_ULTRA:
        case HARDWARECLASS_EXTREME:
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_REFLECTION))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_REFLECTION);
            break;
        case HARDWARECLASS_MEDIUM:
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_REFLECTION))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_REFLECTION);
            break;
        case HARDWARECLASS_SLOW:
        case HARDWARECLASS_LOW:
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MIN_REFLECTION))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MIN_REFLECTION);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_REFLECTION))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_REFLECTION);
            break;
        default:
            break;
    }
    return SNO_NONE;
}

// line 227
SNO ShaderMapGetDefaultShader(const SNO snoShaderMap, const BOOL fComplainIfFallbackIsMissing, const ShaderMapDefinition * pdefShaderMap) {
    if (snoShaderMap == SNO_NONE)
        return SNO_NONE;

    CTagMap * hTagMap;
    if (pdefShaderMap) {
        hTagMap = pdefShaderMap->hTagMap;
    } else {
        ScopedShaderMapDefinitionLockR pdefShaderMapScoped(snoShaderMap);
        hTagMap = pdefShaderMapScoped->hTagMap;
    }

    HardwareClass eHWClass = GfxGetHWClass();

    if (Prefs::GetVideoPrefs().GetReflectionQuality() > 0) {
        SNO snoShader = sGetReflectionShader(hTagMap, eHWClass, Prefs::GetVideoPrefs().GetReflectionQuality());
        if (snoShader != SNO_NONE)
            return snoShader;
    }

    switch (eHWClass) {
        case HARDWARECLASS_HIGH:
        case HARDWARECLASS_ULTRA:
        case HARDWARECLASS_EXTREME:
            if (GfxGetMSAAEnabled()) {
                if (TagMapTagExists(hTagMap, TAG_SHADERMAP_ALPHAMASK_DEFAULT))
                    return TagMapGetSNO(hTagMap, TAG_SHADERMAP_ALPHAMASK_DEFAULT);
            }
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_PP_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_PP_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_CONSOLE_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_CONSOLE_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_DEFAULT);
            break;
        case HARDWARECLASS_MEDIUM:
            if (GfxGetMSAAEnabled()) {
                if (TagMapTagExists(hTagMap, TAG_SHADERMAP_ALPHAMASK_DEFAULT))
                    return TagMapGetSNO(hTagMap, TAG_SHADERMAP_ALPHAMASK_DEFAULT);
            }
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_PP_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_PP_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_CONSOLE_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_CONSOLE_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_DEFAULT);
            break;
        case HARDWARECLASS_SLOW:
        case HARDWARECLASS_LOW:
            if (GfxGetMSAAEnabled()) {
                if (TagMapTagExists(hTagMap, TAG_SHADERMAP_ALPHAMASK_DEFAULT))
                    return TagMapGetSNO(hTagMap, TAG_SHADERMAP_ALPHAMASK_DEFAULT);
            }
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_PP_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_PP_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_CONSOLE_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_CONSOLE_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MIN_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MIN_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_DEFAULT);
            break;
        default:
            break;
    }

    if (fComplainIfFallbackIsMissing) {
        DisplayInternalError(NULL, "", 324, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return SNO_NONE;
}

// The scoped lock and tag-map conversion share the fallback block at original lines 350-351.
#line 333 "X/Client/Definitions/ShaderMapDefinition.cpp"
SNO ShaderMapGetFadeShader(
    const SNO snoShaderMap,
    const ShaderMapDefinition * pdefShaderMap)
{
    if (snoShaderMap == SNO_NONE)
    {
        return SNO_NONE;
    }

    CTagMap * hTagMap;
    if (pdefShaderMap)
    {
        hTagMap = pdefShaderMap->hTagMap;
    }
    else
    {

        ScopedShaderMapDefinitionLockR pdefMyShaderMap(snoShaderMap);
        hTagMap = pdefMyShaderMap->hTagMap;
    }

    HardwareClass eHWClass = GfxGetHWClass();
    switch (eHWClass) // The original groups the three high hardware classes.
    {
        case HARDWARECLASS_HIGH:
        case HARDWARECLASS_ULTRA:
        case HARDWARECLASS_EXTREME:

            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_PP_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_PP_FADE);





            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_FADE);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_FADE);
            break;
        case HARDWARECLASS_MEDIUM:
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_PP_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_PP_FADE);





            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_FADE);
            break;
        case HARDWARECLASS_SLOW:
        case HARDWARECLASS_LOW:

            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_PP_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_PP_FADE);





            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MIN_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MIN_FADE);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_FADE))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_FADE);
            break;

    }


    switch (eHWClass) // Each class falls back to its own default tag, then medium.
    {
        case HARDWARECLASS_HIGH:
        case HARDWARECLASS_ULTRA:
        case HARDWARECLASS_EXTREME:
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_HIGH_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGH_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_DEFAULT);
            break;
        case HARDWARECLASS_MEDIUM:
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_DEFAULT);
            break;
        case HARDWARECLASS_SLOW:
        case HARDWARECLASS_LOW:
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MIN_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MIN_DEFAULT);
            if (TagMapTagExists(hTagMap, TAG_SHADERMAP_MED_DEFAULT))
                return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MED_DEFAULT);
            break;
    }

    return SNO_NONE;
}

#line 392 "X/Client/Definitions/ShaderMapDefinition.cpp"
// line 477
SNO ShaderMapGetGhostZPassShader(const ShaderMapDefinition * pdefShaderMap) {
    CTagMap * hTagMap = pdefShaderMap->hTagMap;
    if (TagMapTagExists(hTagMap, TAG_SHADERMAP_GHOST_ZPASS))
        return TagMapGetSNO(hTagMap, TAG_SHADERMAP_GHOST_ZPASS);
    return SNO_NONE;
}

// line 490
SNO ShaderMapGetHighlightShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned) {
    CTagMap * hTagMap = pdefShaderMap->hTagMap;

    if (GfxGetMSAAEnabled()) {
        if (TagMapTagExists(hTagMap, TAG_SHADERMAP_ALPHAMASK_HIGHLIGHT))
            return TagMapGetSNO(hTagMap, TAG_SHADERMAP_ALPHAMASK_HIGHLIGHT);
    }

    SNO sno = TagMapGetSNO(hTagMap, TAG_SHADERMAP_HIGHLIGHT);
    if (sno != SNO_NONE)
        return sno;

    return GlobalSNOGet(fSkinned ? GLOBALSNO_RACTORGFX_SHADER_HIGHLIGHT_SKIN_ZTEST : GLOBALSNO_RACTORGFX_SHADER_HIGHLIGHT_ZTEST);
}

// line 516
SNO ShaderMapGetOccludedShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned) {
    CTagMap * hTagMap = pdefShaderMap->hTagMap;

    SNO sno = TagMapGetSNO(hTagMap, TAG_SHADERMAP_OCCLUDED);
    if (sno != SNO_NONE)
        return sno;

    if (fSkinned)
        return GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_OCCLUDED_SKIN_ZTEST);
    return GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_OCCLUDED_ZTEST);
}

// line 537
SNO ShaderMapGetPrepassShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned) {
    CTagMap * hTagMap = pdefShaderMap->hTagMap;

    if (GfxGetMSAAEnabled()) {
        if (TagMapTagExists(hTagMap, TAG_SHADERMAP_PREPASS_ALPHAMASK))
            return TagMapGetSNO(hTagMap, TAG_SHADERMAP_PREPASS_ALPHAMASK);
    }
    if (TagMapTagExists(hTagMap, TAG_SHADERMAP_PREPASS))
        return TagMapGetSNO(hTagMap, TAG_SHADERMAP_PREPASS);
    return SNO_NONE;
}

// line 559
SNO ShaderMapGetMaskShader(const ShaderMapDefinition * pdefShaderMap, const BOOL fSkinned) {
    CTagMap * hTagMap = pdefShaderMap->hTagMap;
    return TagMapGetSNO(hTagMap, TAG_SHADERMAP_MASK);
}

// The fallback branches test animation and cloth, then tint, then banner at original lines 587-591.
#line 572 "X/Client/Definitions/ShaderMapDefinition.cpp"
SNO ShaderMapGetCookieShader(
    const ShaderMapDefinition * pdefShaderMap,
    const BOOL fHasAnimation,
    const BOOL fTinted,
    const BOOL fIsCloth,
    const BOOL fIsBanner)
{
    CTagMap * hTagMap = pdefShaderMap->hTagMap;

    SNO snoShader = TagMapGetSNO(hTagMap, TAG_SHADERMAP_COOKIE);
    if (snoShader == SNO_NONE)
    {



        if (!fHasAnimation || fIsCloth)
        {
            if (!fTinted)
            {
                if (fIsBanner)
                {
                    snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_COOKIE_BANNER);
                }
                else
                {
                    snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_COOKIE);
                }
            }
            else
            {
                snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_COOKIE_TINT);
            }
        }
        else
        {
            if (fTinted)
                snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_COOKIE_SKIN_TINT);
            else
                snoShader = GlobalSNOGet(GLOBALSNO_RACTORGFX_SHADER_COOKIE_SKIN);
        }
    }




    return snoShader;
}

#line 469 "X/Client/Definitions/ShaderMapDefinition.cpp"
// line 620
BOOL ShaderMapIsReflective(const SNO snoShaderMap) {
    ScopedShaderMapDefinitionLockR pdefShaderMap(snoShaderMap);
    CTagMap * hTagMap = pdefShaderMap->hTagMap;

    POS pos = TagMapGetStartPosition(hTagMap);
    while (pos) {
        uint32 dwTag = TagMapGetNextTag(hTagMap, pos);
        SNO snoShader = TagMapGetSNO(hTagMap, dwTag);
        ScopedShaderDefinitionLockR pdefShader(snoShader);
        for (int32 nPass = 0; nPass < pdefShader->nPassCount; nPass++) {
            RenderPass & tRenderPass = pdefShader->ptPasses[nPass];
            for (uint32 dwTexState = 0; dwTexState < tRenderPass.dwStageCount; dwTexState++) {
                if (tRenderPass.ptsParams[dwTexState].eShaderTex == SHADERTEX_REFLECTION)
                    return TRUE;
            }
        }
    }
    return FALSE;
}
