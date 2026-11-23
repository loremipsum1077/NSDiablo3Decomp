// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/GfxVB.h
//
// Trimmed: the vertex formats GfxVB.cpp does not touch are left out; add them back
// when something uses them.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/GfxColorFormats.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"

static const float MaxUVRange = 64.0f;  // line 44
static const float MaxUVRangeLM = 4.0f;  // line 45

enum {
    VBFORMAT_UNKNOWN = -1,
    VBFORMAT_NOTEX_L = 0,
    VBFORMAT_BILLBOARD = 1,
    VBFORMAT_1TEX_L = 2,
    VBFORMAT_SPRITE = 3,
    VBFORMAT_2TEX_L = 4,
    VBFORMAT_BUMPBASIS = 5,
    VBFORMAT_SKIN = 6,
    VBFORMAT_CLOTH = 7,
    VBFORMAT_D3DXATLAS = 8,
    VBFORMAT_2TEX_L_CPOS = 9,
    VBFORMAT_SKIN_CPOS = 10,
    VBFORMAT_BILLBOARD_PARTICLE = 11,
    NUM_VBFORMATS = 12,
};

typedef int32 VBFormat;

typedef RGBAColor UByte4;

enum {
    NUM_FATVERTEX_COLORSETS = 2,
    NUM_FATVERTEX_UVSETS = 2,
};

// X/Client/Gfx/GfxVB.h (DWARF reference)
struct UVSet {  // size 0x8
    float flU;  // 0x00
    float flV;  // 0x04
};
static_assert(sizeof(UVSet) == 0x8, "UVSet");

struct FatVertex {  // size 0x20
    Vector3D wpPos;  // 0x00
    UByte4 wvpNormal;  // 0x0c
    RGBAColor rgbaColor[NUM_FATVERTEX_COLORSETS];  // 0x10
    UByte4 vpUVSet[NUM_FATVERTEX_UVSETS];  // 0x18
};
static_assert(sizeof(FatVertex) == 0x20, "FatVertex");

struct VertexNoTexL {  // size 0x10
    Vector3D wpPos;  // 0x00
    VertexColor vcDiffuse;  // 0x0c
};
static_assert(sizeof(VertexNoTexL) == 0x10, "VertexNoTexL");

struct Vertex1TexL {  // size 0x1c
    Vector3D wpPos;  // 0x00
    UByte4 wvpNormal;  // 0x0c
    VertexColor vcDiffuse;  // 0x10
    VertexColor vcAuxColor;  // 0x14
    UByte4 vpUV0;  // 0x18
};
static_assert(sizeof(Vertex1TexL) == 0x1c, "Vertex1TexL");

struct Vertex2TexL {  // size 0x20
    Vector3D wpPos;  // 0x00
    UByte4 wvpNormal;  // 0x0c
    VertexColor vcDiffuse;  // 0x10
    VertexColor vcAuxColor;  // 0x14
    UByte4 vpUV0;  // 0x18
    UByte4 vpUV1;  // 0x1c
};
static_assert(sizeof(Vertex2TexL) == 0x20, "Vertex2TexL");

struct Vertex2TexLCPos {  // size 0x1c
    int16 nX;  // 0x00
    int16 nY;  // 0x02
    int16 nZ;  // 0x04
    int16 nPad;  // 0x06
    UByte4 wvpNormal;  // 0x08
    VertexColor vcDiffuse;  // 0x0c
    VertexColor vcAuxColor;  // 0x10
    UByte4 vpUV0;  // 0x14
    UByte4 vpUV1;  // 0x18
};
static_assert(sizeof(Vertex2TexLCPos) == 0x1c, "Vertex2TexLCPos");

struct VertexSkinCPos {  // size 0x24
    int16 nX;  // 0x00
    int16 nY;  // 0x02
    int16 nZ;  // 0x04
    int16 nPad;  // 0x06
    UByte4 wvpNormal;  // 0x08
    VertexColor vcDiffuse;  // 0x0c
    VertexColor vcAuxColor;  // 0x10
    UByte4 vpUV0;  // 0x14
    UByte4 vpUV1;  // 0x18
    uint32 dwMatrixIndices;  // 0x1c
    UByte4 vMatrixWeights;  // 0x20
};
static_assert(sizeof(VertexSkinCPos) == 0x24, "VertexSkinCPos");

struct VertexCloth {  // size 0xc
    Vector3D wpModelSpacePos;  // 0x00
};
static_assert(sizeof(VertexCloth) == 0xc, "VertexCloth");

struct VertexElement {  // size 0x18
    uint16 wOffset;  // 0x00
    uint32 dwType;  // 0x04
    uint8 bUsage;  // 0x08
    uint8 bIndex;  // 0x09
    const char * szSemantic;  // 0x10
};
static_assert(sizeof(VertexElement) == 0x18, "VertexElement");

typedef XGrowableList<VertexElement, VertexElement &> VertexElementList;

enum {
    VBID_INVALID = -1,
};

typedef int32 VBID;

// X/Client/Gfx/GfxVB.cpp
BOOL GfxVBAllocTracked(const int32 eUsage, const VBFormat eVBFormat, const uint32 dwCount, const BOOL fPurgeIfNecessary, const void * lpInitialData, VBID & vbid, LPCSTR szFile, int32 nLine, LPCSTR szDebugString);
void GfxVBRelease(const VBID vbid);
void * GfxVBLockAll(const VBID vbid);
void GfxVBUnlock(const VBID vbid);
BOOL GfxVBIsValid(const VBID vbid);
void GfxVBSetStream(const int32 nStreamIndex, const uint32 dwStreamOffset, const VBID vbid);

struct VBFormatInfo {  // size 0x10
    uint32 dwSize;  // 0x00
    VBID vbidPooled;  // 0x04
    VertexElementList * ptElemList;  // 0x08
};
static_assert(sizeof(VBFormatInfo) == 0x10, "VBFormatInfo");

struct PackedVB {  // size 0x8
    int32 nVertCount;  // 0x00
    VBID vbid;  // 0x04
    // line 326; the only constructor the DWARF declares. XGrowableList's node constructor stores
    // 0xffffffff00000000 through it (AppearanceDefinition.cpp sPackVB).
    PackedVB() : nVertCount(0), vbid(VBID_INVALID) {}
};
static_assert(sizeof(PackedVB) == 0x8, "PackedVB");

typedef XGrowableList<PackedVB, PackedVB &> PackedVBList;

struct PackedVBEntry {  // size 0x10
    PackedVB * ptPVB;  // 0x00
    uint32 dwVertexOffset;  // 0x08
    uint32 dwStreamOffset;  // 0x0c
};
static_assert(sizeof(PackedVBEntry) == 0x10, "PackedVBEntry");

struct PackedVBEntry64 {  // size 0x10
    XPtr64<PackedVB> ptPVB;  // 0x00
    uint32 dwVertexOffset;  // 0x08
    uint32 dwStreamOffset;  // 0x0c
};
static_assert(sizeof(PackedVBEntry64) == 0x10, "PackedVBEntry64");

// Every translation unit that includes this header initializes its own copy of these in its
// _GLOBAL__sub_I. Values from the original's stores.
static const RGBAColor cgcDefaultNormal(128, 127, 255, 0);  // line 351
static const RGBAColor cgcNullNormal(128, 127, 128, 0);  // line 352
static const RGBAColor cgcXAxisNormal(255, 127, 128, 0);  // line 353
static const RGBAColor cgcZAxisNormal(128, 127, 255, 0);  // line 354

// line 553
inline void Vertex2TexLSet(Vertex2TexL & vDest, const Vector3D & wpPos, const UByte4 wvpNormal,
                           const VertexColor vcDiffuse, const VertexColor vcAuxColor, const UByte4 vpUVSet0,
                           const UByte4 vpUVSet1) {
    vDest.wpPos = wpPos;
    vDest.wvpNormal = wvpNormal;
    vDest.vcDiffuse = vcDiffuse;
    vDest.vcAuxColor = vcAuxColor;
    vDest.vpUV0 = vpUVSet0;
    vDest.vpUV1 = vpUVSet1;
}

// line 647
inline void VertexClothSet(VertexCloth & vDest, const Vector3D & wpModelSpacePos) {
    vDest.wpModelSpacePos = wpModelSpacePos;
}

// line 580
inline void Vertex1TexLSet(Vertex1TexL & vDest, const Vector3D & wpPos, const UByte4 wvNormal,
                           const VertexColor vcDiffuse, const VertexColor vcAuxColor, const UByte4 vpUVSet0) {
    vDest.wpPos = wpPos;
    vDest.wvpNormal = wvNormal;
    vDest.vcDiffuse = vcDiffuse;
    vDest.vcAuxColor = vcAuxColor;
    vDest.vpUV0 = vpUVSet0;
}

// Defined in X/Client/Gfx/GfxVB.cpp:1254 and :1314.
Vector2D UByte4TexCoordToVector2D(const UByte4 packed);
Vector2D UByte4TexCoordToVector2DLM(const UByte4 packed);
// Defined in X/Client/Gfx/GfxVB.cpp:437; called from X/Client/ParticleRenderUpdate.cpp.
UByte4 Vector2DTexCoordToUByte4(const Vector2D & uv);

// DWARF GfxVB.h:203 (members on 205-213). Appended here so no existing line moves.
struct VertexBillboardParticle {  // size 0x38
    Vector3D wpPos;  // 0x00
    VertexColor vcDiffuse;  // 0x0c
    VertexColor vcAuxColor;  // 0x10
    UByte4 vpUV0;  // 0x14
    UByte4 vpUV1;  // 0x18
    UByte4 vpUV2;  // 0x1c
    UByte4 vpUV3;  // 0x20
    UByte4 qBillboard;  // 0x24
    Vector4D wpOffsetAngle;  // 0x28
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(UVSet) == 0x8, "UVSet");
static_assert(offsetof(UVSet, flU) == 0x0, "UVSet");
static_assert(offsetof(UVSet, flV) == 0x4, "UVSet");
static_assert(sizeof(FatVertex) == 0x20, "FatVertex");
static_assert(offsetof(FatVertex, wpPos) == 0x0, "FatVertex");
static_assert(offsetof(FatVertex, wvpNormal) == 0xc, "FatVertex");
static_assert(offsetof(FatVertex, rgbaColor) == 0x10, "FatVertex");
static_assert(offsetof(FatVertex, vpUVSet) == 0x18, "FatVertex");
static_assert(sizeof(VertexNoTexL) == 0x10, "VertexNoTexL");
static_assert(offsetof(VertexNoTexL, wpPos) == 0x0, "VertexNoTexL");
static_assert(offsetof(VertexNoTexL, vcDiffuse) == 0xc, "VertexNoTexL");
static_assert(sizeof(Vertex1TexL) == 0x1c, "Vertex1TexL");
static_assert(offsetof(Vertex1TexL, wpPos) == 0x0, "Vertex1TexL");
static_assert(offsetof(Vertex1TexL, wvpNormal) == 0xc, "Vertex1TexL");
static_assert(offsetof(Vertex1TexL, vcDiffuse) == 0x10, "Vertex1TexL");
static_assert(offsetof(Vertex1TexL, vcAuxColor) == 0x14, "Vertex1TexL");
static_assert(offsetof(Vertex1TexL, vpUV0) == 0x18, "Vertex1TexL");
static_assert(sizeof(Vertex2TexL) == 0x20, "Vertex2TexL");
static_assert(offsetof(Vertex2TexL, wpPos) == 0x0, "Vertex2TexL");
static_assert(offsetof(Vertex2TexL, wvpNormal) == 0xc, "Vertex2TexL");
static_assert(offsetof(Vertex2TexL, vcDiffuse) == 0x10, "Vertex2TexL");
static_assert(offsetof(Vertex2TexL, vcAuxColor) == 0x14, "Vertex2TexL");
static_assert(offsetof(Vertex2TexL, vpUV0) == 0x18, "Vertex2TexL");
static_assert(offsetof(Vertex2TexL, vpUV1) == 0x1c, "Vertex2TexL");
static_assert(sizeof(VertexCloth) == 0xc, "VertexCloth");
static_assert(offsetof(VertexCloth, wpModelSpacePos) == 0x0, "VertexCloth");
static_assert(sizeof(VertexBillboardParticle) == 0x38, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, wpPos) == 0x0, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, vcDiffuse) == 0xc, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, vcAuxColor) == 0x10, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, vpUV0) == 0x14, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, vpUV1) == 0x18, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, vpUV2) == 0x1c, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, vpUV3) == 0x20, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, qBillboard) == 0x24, "VertexBillboardParticle");
static_assert(offsetof(VertexBillboardParticle, wpOffsetAngle) == 0x28, "VertexBillboardParticle");
static_assert(sizeof(Vertex2TexLCPos) == 0x1c, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, nX) == 0x0, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, nY) == 0x2, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, nZ) == 0x4, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, nPad) == 0x6, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, wvpNormal) == 0x8, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, vcDiffuse) == 0xc, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, vcAuxColor) == 0x10, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, vpUV0) == 0x14, "Vertex2TexLCPos");
static_assert(offsetof(Vertex2TexLCPos, vpUV1) == 0x18, "Vertex2TexLCPos");
static_assert(sizeof(VertexSkinCPos) == 0x24, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, nX) == 0x0, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, nY) == 0x2, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, nZ) == 0x4, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, nPad) == 0x6, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, wvpNormal) == 0x8, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, vcDiffuse) == 0xc, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, vcAuxColor) == 0x10, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, vpUV0) == 0x14, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, vpUV1) == 0x18, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, dwMatrixIndices) == 0x1c, "VertexSkinCPos");
static_assert(offsetof(VertexSkinCPos, vMatrixWeights) == 0x20, "VertexSkinCPos");
static_assert(sizeof(VertexElement) == 0x18, "VertexElement");
static_assert(offsetof(VertexElement, wOffset) == 0x0, "VertexElement");
static_assert(offsetof(VertexElement, dwType) == 0x4, "VertexElement");
static_assert(offsetof(VertexElement, bUsage) == 0x8, "VertexElement");
static_assert(offsetof(VertexElement, bIndex) == 0x9, "VertexElement");
static_assert(offsetof(VertexElement, szSemantic) == 0x10, "VertexElement");
static_assert(sizeof(VBFormatInfo) == 0x10, "VBFormatInfo");
static_assert(offsetof(VBFormatInfo, dwSize) == 0x0, "VBFormatInfo");
static_assert(offsetof(VBFormatInfo, vbidPooled) == 0x4, "VBFormatInfo");
static_assert(offsetof(VBFormatInfo, ptElemList) == 0x8, "VBFormatInfo");
static_assert(sizeof(PackedVB) == 0x8, "PackedVB");
static_assert(offsetof(PackedVB, nVertCount) == 0x0, "PackedVB");
static_assert(offsetof(PackedVB, vbid) == 0x4, "PackedVB");
static_assert(sizeof(PackedVBEntry) == 0x10, "PackedVBEntry");
static_assert(offsetof(PackedVBEntry, ptPVB) == 0x0, "PackedVBEntry");
static_assert(offsetof(PackedVBEntry, dwVertexOffset) == 0x8, "PackedVBEntry");
static_assert(offsetof(PackedVBEntry, dwStreamOffset) == 0xc, "PackedVBEntry");
static_assert(sizeof(PackedVBEntry64) == 0x10, "PackedVBEntry64");
static_assert(offsetof(PackedVBEntry64, ptPVB) == 0x0, "PackedVBEntry64");
static_assert(offsetof(PackedVBEntry64, dwVertexOffset) == 0x8, "PackedVBEntry64");
static_assert(offsetof(PackedVBEntry64, dwStreamOffset) == 0xc, "PackedVBEntry64");
