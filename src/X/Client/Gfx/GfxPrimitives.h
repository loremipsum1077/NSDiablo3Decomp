// Adopted from the DWARF skeleton (build/dwarf/headers/X/Client/Gfx/GfxPrimitives.h);
// hand-maintained from here on.
// Original file: X/Client/Gfx/GfxPrimitives.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/SigmaTypes.h"
#include "GfxCommon.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Sphere.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Common/GlobalSNO.h"

struct Triangle2D;
struct VertexNoTexL;

enum GfxLineStyle {
    GFX_LINE_STYLE_SOLID = 0,
    GFX_LINE_STYLE_DASHED = 1,
};

enum GfxFillStyle {
    GFX_FILL_SOLID = 0,
    GFX_WIREFRAME = 1,
};

class TriangleList2D {  // size 0x10
public:
    TriangleList2D();
    ~TriangleList2D();
    BOOL Add(const Triangle2D & wtri, const float flZ, const RGBAColor & rgbaColor);
    void Draw() const;
    BOOL GetTriangle(int32 nIndex, Triangle2D & wtri) const;
    int32 GetCount() const;
    void Clear() { m_nCount = 0; }  // line 109
private:
    int32 m_nCount;  // 0x00
    VertexNoTexL * m_tVerts;  // 0x08
};
static_assert(sizeof(TriangleList2D) == 0x10, "TriangleList2D");

inline int32 TriangleList2D::GetCount() const { return m_nCount; }  // line 103

// X/Client/Gfx/GfxPrimitives.cpp:171
//   _Z19GfxRectDrawDeferredRK6Rect2DRK9RGBAColorf11RenderLayeri
void GfxRectDrawDeferred( const Rect2D & wrRect,
                          const RGBAColor & rgbaColor,
                          const float flZ,
                          const RenderLayer eRenderLayerOverride,
                          BOOL fInRenderDraw );

// GfxPrimitives.cpp:2199
TriangleList2D * GfxPrimitivesGetSharedTriList2D();

// Functions defined in this file (bodies are not in the DWARF):
//   line   103  int32 TriangleList2D::GetCount() const  [inlined into 1 functions]
//   line   109  void TriangleList2D::Clear()  [inlined into 5 functions]

// Declared here for its definition in the DWARF owner X/Client/Gfx/GfxPrimitives.cpp:1085
// (0x71001d8600).
void GfxSphereDrawDeferred(const Sphere & ws, RGBAColor rgbaColor, GlobalSNO snoShader,
                           const RenderContext eContext);

// GfxPrimitives.cpp:471 and :1119.
struct OBB;
struct AxialCylinder;
void GfxOBB3DDrawDeferred(const OBB & obb, const RGBAColor & rgbaColor, GlobalSNO snoShader,
                          GfxLineStyle eLineStyle, const RenderContext eContext);
void GfxAxialCylinderDrawDeferred(const AxialCylinder & wcyl, const RGBAColor & rgbaColor,
                                  GlobalSNO snoShader, const RenderContext eContext);

// GfxPrimitives.cpp:357
void GfxLine3DDrawDeferred(const Vector3D & wp0, const Vector3D & wp1, const RGBAColor & rgbaColor,
                           GlobalSNO snoShader, const RenderContext eContext);
