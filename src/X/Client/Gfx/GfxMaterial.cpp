#include "X/Client/Gfx/GfxMaterial.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "GfxVB.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include <stddef.h>
// GfxMaterial.h DWARF typedef; adopted header currently supplies only enumerators.
typedef int32 OverlayType;
#line 23 "X/Client/Gfx/GfxMaterial.cpp"
extern const Material cgmatDefault = {
    RGBAColorValue(1, 1, 1, 1),
    RGBAColorValue(1, 1, 1, 1),
    RGBAColorValue(0, 0, 0, 1),
    RGBAColorValue(0, 0, 0, 1),
    0.0f,
    0,
};
#line 33 "X/Client/Gfx/GfxMaterial.cpp"
extern const MaterialTexture cgtMatTexDefault = {
    {0, 0},
    0,
    SNO_NONE,
    0,
};
#line 42 "X/Client/Gfx/GfxMaterial.cpp"
static const TypeDescriptorField Material_Fields[] = {
    TypeDescriptorField("", DT_RGBACOLORVALUE, offsetof(Material, rgbavalAmbient), 0, 1),
    TypeDescriptorField("", DT_RGBACOLORVALUE, offsetof(Material, rgbavalDiffuse), 0, 1),
    TypeDescriptorField("", DT_RGBACOLORVALUE, offsetof(Material, rgbavalSpecular), 0, 1),
    TypeDescriptorField("", DT_RGBACOLORVALUE, offsetof(Material, rgbavalEmissive), 0, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(Material, flPower), 0, 1),
    TypeDescriptorField("", DT_INT, offsetof(Material, nSortPri), 0, 1),
    TypeDescriptorField(0, DT_NULL, sizeof(Material), 0, 0),
};
#line 49 "X/Client/Gfx/GfxMaterial.cpp"
ComplexTypeDescriptor TypeDescriptor_Material("Material", 0, Material_Fields, 7, 0, 0x80, 0, 0, 0, 0);
const XTypeDescriptor* DT_GFXMATERIAL = &TypeDescriptor_Material;
#line 51 "X/Client/Gfx/GfxMaterial.cpp"
static const TypeDescriptorField MaterialTexture_Fields[] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(MaterialTexture, snoTex), "", 44, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(MaterialTexture, serTexAnimCompParams), 0, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(MaterialTexture, pTexAnimCompParams), 0x700020, -8),
    TypeDescriptorField("", DT_INT, offsetof(MaterialTexture, dwPad), 0, 0),
    TypeDescriptorField(0, DT_NULL, sizeof(MaterialTexture), 0, 0),
};
#line 56 "X/Client/Gfx/GfxMaterial.cpp"
ComplexTypeDescriptor TypeDescriptor_MaterialTexture("MaterialTexture", 0, MaterialTexture_Fields, 5, 0, 0x80, 0, 0, 0, 0);
const XTypeDescriptor* DT_MATERIALTEXTURE = &TypeDescriptor_MaterialTexture;
#line 58 "X/Client/Gfx/GfxMaterial.cpp"
static const TypeDescriptorField MaterialTextureEntry_Fields[] = {
    TypeDescriptorField("", DT_INT, offsetof(MaterialTextureEntry, eShaderTex), 0, 1),
    TypeDescriptorField("", DT_INT, offsetof(MaterialTextureEntry, dwPad), 0, 0),
    TypeDescriptorField("", DT_MATERIALTEXTURE, offsetof(MaterialTextureEntry, tMatTex), 0, 1),
    TypeDescriptorField(0, DT_NULL, sizeof(MaterialTextureEntry), 0, 0),
};
#line 62 "X/Client/Gfx/GfxMaterial.cpp"
ComplexTypeDescriptor TypeDescriptor_MaterialTextureEntry("MaterialTextureEntry", 0, MaterialTextureEntry_Fields, 4, 0, 0x80, 0, 0, 0, 0);
const XTypeDescriptor* DT_MATERIALTEXTUREENTRY = &TypeDescriptor_MaterialTextureEntry;
#line 64 "X/Client/Gfx/GfxMaterial.cpp"
static const TypeDescriptorField UberMaterial_Fields[] = {
    TypeDescriptorField("", DT_GFXMATERIAL, offsetof(UberMaterial, mat), 0, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(UberMaterial, snoShaderMap), "", 36, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UberMaterial, serMatTexList), 0, 0),
    VariableArrayTypeDescriptorField("", DT_MATERIALTEXTUREENTRY, offsetof(UberMaterial, ptMatTexList), 0x21, -12),
    TypeDescriptorField(0, DT_NULL, sizeof(UberMaterial), 0, 0),
};
#line 68 "X/Client/Gfx/GfxMaterial.cpp"
ComplexTypeDescriptor TypeDescriptor_UberMaterial("UberMaterial", 0, UberMaterial_Fields, 5, 0, 0x80, 0, 0, 0, 0);
const XTypeDescriptor* DT_UBERMATERIAL = &TypeDescriptor_UberMaterial;



int32 MaterialGetTextureCount(const UberMaterial & tUberMaterial) {
    int32 nCount = tUberMaterial.serMatTexList.ElementCount(sizeof(MaterialTextureEntry));
    return nCount;
}

void MaterialListSort(UberMaterial & tUberMaterial) {
    uint32 dwCount = MaterialGetTextureCount(tUberMaterial);
    if (dwCount > 1) {
        for (uint32 i = 0; i < dwCount - 1; ++i) {
            ShaderTex eShaderTex = tUberMaterial.ptMatTexList[i].eShaderTex;
            for (uint32 j = i + 1; j < dwCount; ++j) {
                ShaderTex eShaderTexCmp = tUberMaterial.ptMatTexList[j].eShaderTex;
                if (eShaderTexCmp < eShaderTex) {
                    MaterialTextureEntry tTemp = tUberMaterial.ptMatTexList[j];
                    tUberMaterial.ptMatTexList[j] = tUberMaterial.ptMatTexList[i];
                    tUberMaterial.ptMatTexList[i] = tTemp;
                    eShaderTex = eShaderTexCmp;
                }
            }
        }
    }
}

uint32 MaterialTextureBuildHash(const UberMaterial & tUberMaterial) {
    int32 nCount = tUberMaterial.serMatTexList.dwSize / sizeof(MaterialTextureEntry);
    uint32 dwMatTexSortHash = nCount;
    const MaterialTextureEntry * arMatTexList = tUberMaterial.ptMatTexList;
    for (int32 i = 0; i < nCount; ++i) {
        const MaterialTextureEntry & tMat = arMatTexList[i];
        if (tMat.tMatTex.snoTex != SNO_NONE) {
            dwMatTexSortHash += tMat.tMatTex.snoTex;
        }
    }
    return dwMatTexSortHash;
}
// The array lookup returns early for a null array. The loop bounds handle an empty range.
#line 310 "X/Client/Gfx/GfxMaterial.cpp"
const MaterialTextureEntry* MaterialLookup(
    const MaterialTextureEntry* arMaterialTex,
    int32 nCount,
    ShaderTex eShaderTex)
{
    if (!arMaterialTex)
    {
        return 0;
    }

    const MaterialTextureEntry* ptMatTexEntry = 0;
    int32 nFirst = 0;
    int32 nLast = nCount - 1;

    while (nFirst <= nLast)
    {
        int32 nMid = (nFirst + nLast) >> 1;

        ShaderTex eShaderTexCmp = arMaterialTex[nMid].eShaderTex;

        if (eShaderTex > eShaderTexCmp)
        {

            nFirst = nMid + 1;
        }
        else if (eShaderTex < eShaderTexCmp)
        {

            nLast = nMid - 1;
        }
        else
        {
            ptMatTexEntry = &arMaterialTex[nMid];
            break;
        }
    }

    return ptMatTexEntry;

}
#line 354 "X/Client/Gfx/GfxMaterial.cpp"
const MaterialTextureEntry* MaterialLookup(
    const UberMaterial& tUberMaterial,
    ShaderTex eShaderTex)
{
    int32 nCount = MaterialGetTextureCount(tUberMaterial);

    return MaterialLookup(tUberMaterial.ptMatTexList, nCount, eShaderTex);
}
#line 366 "X/Client/Gfx/GfxMaterial.cpp"
MaterialTextureEntry* MaterialLookup(
    UberMaterial& tUberMaterial,
    ShaderTex eShaderTex)
{
    int32 nCount = MaterialGetTextureCount(tUberMaterial);
    // DWARF requires mutable result but inlines the const-returning array overload at line 372.
    return const_cast<MaterialTextureEntry*>(MaterialLookup(tUberMaterial.ptMatTexList, nCount, eShaderTex));
}
#line 423 "X/Client/Gfx/GfxMaterial.cpp"
void MaterialSetDefaultValues(UberMaterial& tUberMaterial)
{
    tUberMaterial.snoShaderMap = SNO_NONE;
    tUberMaterial.dwMatTexSortHash = 0;
    tUberMaterial.mat = cgmatDefault;
    tUberMaterial.ptMatTexList = 0;
    tUberMaterial.serMatTexList.dwSize = 0;
}
#line 552 "X/Client/Gfx/GfxMaterial.cpp"
ShaderTex OverlayTypeToShaderTex(const OverlayType eOverlayType)
{
    switch (eOverlayType) {
    case OVERLAYTYPE_ARCANE_DEATH: return SHADERTEX_OVERLAY_ARCANE_DEATH;
    case OVERLAYTYPE_FIRE_DEATH: return SHADERTEX_OVERLAY_FIRE_DEATH;
    case OVERLAYTYPE_LIGHTNING_DEATH: return SHADERTEX_OVERLAY_LIGHTNING_DEATH;
    case OVERLAYTYPE_COLD_DEATH: return SHADERTEX_OVERLAY_COLD_DEATH;
    case OVERLAYTYPE_POISON_DEATH: return SHADERTEX_OVERLAY_POISON_DEATH;
    case OVERLAYTYPE_PLAGUE_DEATH: return SHADERTEX_OVERLAY_PLAGUE_DEATH;
    case OVERLAYTYPE_ZOMBIFIED: return SHADERTEX_OVERLAY_ZOMBIFIED;
    case OVERLAYTYPE_SAND: return SHADERTEX_OVERLAY_SAND;
    case OVERLAYTYPE_CRITICAL_HIT_BLOOD: return SHADERTEX_OVERLAY_CRITICAL_HIT_BLOOD;
    case OVERLAYTYPE_STONED: return SHADERTEX_OVERLAY_STONED;
    case OVERLAYTYPE_CHILLED: return SHADERTEX_OVERLAY_CHILLED;
    case OVERLAYTYPE_FROZEN: return SHADERTEX_OVERLAY_FROZEN;
    case OVERLAYTYPE_POISONED: return SHADERTEX_OVERLAY_POISONED;
    default: return SHADERTEX_NONE;
    }
}
