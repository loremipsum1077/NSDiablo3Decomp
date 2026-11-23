// Layout check for X/Client/Gfx/GfxMaterial.h against the DWARF (byte_size and
// data_member_location). Compiled, not compared: it defines no function the binary has.
#include "X/Client/Gfx/GfxMaterial.h"

#include <cstddef>

#define CHECK(T, m, off) static_assert(offsetof(T, m) == off, #T "::" #m)

static_assert(sizeof(Material) == 0x48, "Material");
CHECK(Material, rgbavalDiffuse, 0x10);
CHECK(Material, rgbavalSpecular, 0x20);
CHECK(Material, rgbavalEmissive, 0x30);
CHECK(Material, flPower, 0x40);
CHECK(Material, nSortPri, 0x44);

static_assert(sizeof(TexAnimCompParams) == 0x4, "TexAnimCompParams");
static_assert(sizeof(TexAnimCompSimpleTransParams) == 0xc, "TexAnimCompSimpleTransParams");
CHECK(TexAnimCompSimpleTransParams, mTex_11, 0x04);
CHECK(TexAnimCompSimpleTransParams, mTex_22, 0x08);
static_assert(sizeof(TexAnimCompSimpleTransVelParams) == 0x14, "TexAnimCompSimpleTransVelParams");
CHECK(TexAnimCompSimpleTransVelParams, flUTransVel, 0x0c);
CHECK(TexAnimCompSimpleTransVelParams, flVTransVel, 0x10);

static_assert(sizeof(MaterialTexture) == 0x18, "MaterialTexture");
CHECK(MaterialTexture, pTexAnimCompParams, 0x08);
CHECK(MaterialTexture, snoTex, 0x10);
CHECK(MaterialTexture, dwPad, 0x14);

static_assert(sizeof(MaterialTextureEntry) == 0x20, "MaterialTextureEntry");
CHECK(MaterialTextureEntry, dwPad, 0x04);
CHECK(MaterialTextureEntry, tMatTex, 0x08);

static_assert(sizeof(UberMaterial) == 0x68, "UberMaterial");
CHECK(UberMaterial, mat, 0x04);
CHECK(UberMaterial, serMatTexList, 0x4c);
CHECK(UberMaterial, ptMatTexList, 0x58);
CHECK(UberMaterial, dwMatTexSortHash, 0x60);
