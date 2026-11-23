// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/GfxMaterial.h
// Material and texture-list structs on the DWARF layouts. MaterialTexture::~MaterialTexture
// (0x71001d48c0) is the implicit destructor (it only runs XPtr64's), so it is not spelled out.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "GfxCommon.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/RGBAColor.h"

enum {
    OPA_FRONT = 0,
    OPA_SIDE = 1,
    OPA_TOP = 2,
};

typedef int32 OverlayProjectionAxis;

enum {
    OVERLAYTYPE_NONE = 0,
    OVERLAYTYPE_ARCANE_DEATH = 1,
    OVERLAYTYPE_FIRE_DEATH = 2,
    OVERLAYTYPE_LIGHTNING_DEATH = 3,
    OVERLAYTYPE_COLD_DEATH = 4,
    OVERLAYTYPE_POISON_DEATH = 5,
    OVERLAYTYPE_PLAGUE_DEATH = 6,
    OVERLAYTYPE_ZOMBIFIED = 7,
    OVERLAYTYPE_SAND = 8,
    OVERLAYTYPE_CRITICAL_HIT_BLOOD = 9,
    OVERLAYTYPE_STONED = 10,
    OVERLAYTYPE_CHILLED = 11,
    OVERLAYTYPE_FROZEN = 12,
    OVERLAYTYPE_POISONED = 13,
};

struct Material {  // size 0x48
    RGBAColorValue rgbavalAmbient;  // 0x00
    RGBAColorValue rgbavalDiffuse;  // 0x10
    RGBAColorValue rgbavalSpecular;  // 0x20
    RGBAColorValue rgbavalEmissive;  // 0x30
    float flPower;  // 0x40
    int32 nSortPri;  // 0x44
};
static_assert(sizeof(Material) == 0x48, "Material");

enum TexAnimCompType {
    TEXANIMCOMP_SIMPLETRANS = 0,
    TEXANIMCOMP_SIMPLETRANS2 = 1,
    TEXANIMCOMP_SIMPLETRANSVEL = 2,
};

struct TexAnimCompParams {  // size 0x4
    TexAnimCompType eCompType;  // 0x00
};
static_assert(sizeof(TexAnimCompParams) == 0x4, "TexAnimCompParams");

struct TexAnimCompSimpleTransParams : public TexAnimCompParams {  // size 0xc
    float mTex_11;  // 0x04
    float mTex_22;  // 0x08
};
static_assert(sizeof(TexAnimCompSimpleTransParams) == 0xc, "TexAnimCompSimpleTransParams");

struct TexAnimCompSimpleTransVelParams : public TexAnimCompParams {  // size 0x14
    float mTex_11;  // 0x04
    float mTex_22;  // 0x08
    float flUTransVel;  // 0x0c
    float flVTransVel;  // 0x10
};
static_assert(sizeof(TexAnimCompSimpleTransVelParams) == 0x14, "TexAnimCompSimpleTransVelParams");

struct MaterialTexture {  // size 0x18
    SerializeData serTexAnimCompParams;  // 0x00
    XPtr64<char> pTexAnimCompParams;  // 0x08
    SNO snoTex;  // 0x10
    uint32 dwPad;  // 0x14
};
static_assert(sizeof(MaterialTexture) == 0x18, "MaterialTexture");

struct MaterialTextureEntry {  // size 0x20
    ShaderTex eShaderTex;  // 0x00
    uint32 dwPad;  // 0x04
    MaterialTexture tMatTex;  // 0x08
};
static_assert(sizeof(MaterialTextureEntry) == 0x20, "MaterialTextureEntry");

struct UberMaterial {  // size 0x68
    SNO snoShaderMap;  // 0x00
    Material mat;  // 0x04
    SerializeData serMatTexList;  // 0x4c
    XPtr64<MaterialTextureEntry> ptMatTexList;  // 0x58
    uint32 dwMatTexSortHash;  // 0x60
};
static_assert(sizeof(UberMaterial) == 0x68, "UberMaterial");

class XTypeDescriptor;
extern const XTypeDescriptor * DT_UBERMATERIAL;  // GfxMaterial.cpp:68

void MaterialSetDefaultValues(UberMaterial & tUberMaterial);  // GfxMaterial.cpp:423

