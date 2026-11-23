#include "Rect.h"

#include "MathGeneral.h"
#include "Ray.h"
#include "Sphere.h"

#include <stddef.h>
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

static const TypeDescriptorField AABB_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(AABB, wp), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(AABB, wvExt), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AABB), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AABB("AABB", 0, AABB_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_AABB = &TypeDescriptor_AABB;

static const TypeDescriptorField OBB_Fields[] = {
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(OBB, transform), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(OBB, wvExt), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(OBB), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_OBB("OBB", 0, OBB_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_OBB = &TypeDescriptor_OBB;

const IRect2D cgirectNull(0, 0, 0, 0);
const Rect2D cgrectNull(0.0f, 0.0f, 0.0f, 0.0f);

void IRect2D::ToUnitCoords(const Rect2D& rectBasis, Rect2D& rectResult) const {
    float flXScale = rectBasis.Width();
    float flYScale = rectBasis.Height();

    if (flXScale < EPSILON) {
        rectResult.right = 0.0f;
        rectResult.left = 0.0f;
    } else {
        rectResult.right = (right - rectBasis.left) / flXScale;
        rectResult.left = (left - rectBasis.left) / flXScale;
    }

    if (flYScale < EPSILON) {
        rectResult.bottom = 0.0f;
        rectResult.top = 0.0f;
    } else {
        rectResult.bottom = (bottom - rectBasis.top) / flYScale;
        rectResult.top = (top - rectBasis.top) / flYScale;
    }
}

void Rect2D::ToPoints(Vector2D* wpCorners, const int32 n90DegreeRotations) const {
    if (n90DegreeRotations == 0) {
        wpCorners[CORNER_TL].Set(left, top);
        wpCorners[CORNER_BR].Set(right, bottom);
        wpCorners[CORNER_TR].Set(right, top);
        wpCorners[CORNER_BL].Set(left, bottom);
    } else {
        wpCorners[(CORNER_TL + n90DegreeRotations) % NUM_CORNERS].Set(left, top);
        wpCorners[(CORNER_BR + n90DegreeRotations) % NUM_CORNERS].Set(right, bottom);
        wpCorners[(CORNER_TR + n90DegreeRotations) % NUM_CORNERS].Set(right, top);
        wpCorners[(CORNER_BL + n90DegreeRotations) % NUM_CORNERS].Set(left, bottom);
    }
}

BOOL Rect2D::Intersect(const Rect2D& rectA, const Rect2D& rectB, Rect2D* prectIntersect) {
    Rect2D rectIntersect;

    rectIntersect.left = Math::Max(rectA.left, rectB.left);
    rectIntersect.right = Math::Min(rectA.right, rectB.right);
    rectIntersect.top = Math::Max(rectA.top, rectB.top);
    rectIntersect.bottom = Math::Min(rectA.bottom, rectB.bottom);

    if (prectIntersect) {
        *prectIntersect = rectIntersect;
    }

    if (rectIntersect.left <= rectIntersect.right && rectIntersect.top <= rectIntersect.bottom) {
        return TRUE;
    }

    if (prectIntersect) {
        prectIntersect->Set(0.0f, 0.0f, 0.0f, 0.0f);
    }
    return FALSE;
}

float Rect2D::Width() const {
    return right - left;
}

float Rect2D::Height() const {
    return bottom - top;
}

void Rect2D::ToUnitCoords(const Rect2D& rectBasis, Rect2D& rectResult) const {
    float flXScale = rectBasis.Width();
    float flYScale = rectBasis.Height();

    if (flXScale < EPSILON) {
        rectResult.right = 0.0f;
        rectResult.left = 0.0f;
    } else {
        rectResult.right = (right - rectBasis.left) / flXScale;
        rectResult.left = (left - rectBasis.left) / flXScale;
    }

    if (flYScale < EPSILON) {
        rectResult.bottom = 0.0f;
        rectResult.top = 0.0f;
    } else {
        rectResult.bottom = (bottom - rectBasis.top) / flYScale;
        rectResult.top = (top - rectBasis.top) / flYScale;
    }
}

void Rect2D::FromUnitCoords(const Rect2D& rectBasis, IRect2D& rectResult) const {
    float flXScale = rectBasis.Width();
    float flYScale = rectBasis.Height();

    rectResult.right = Math::FloatToInt(right * flXScale + rectBasis.left);
    rectResult.bottom = Math::FloatToInt(bottom * flYScale + rectBasis.top);
    rectResult.left = Math::FloatToInt(left * flXScale + rectBasis.left);
    rectResult.top = Math::FloatToInt(top * flYScale + rectBasis.top);
}

void Rect2D::FromUnitCoords(const Rect2D& rectBasis, Rect2D& rectResult) const {
    float flXScale = rectBasis.Width();
    float flYScale = rectBasis.Height();

    rectResult.right = right * flXScale + rectBasis.left;
    rectResult.bottom = bottom * flYScale + rectBasis.top;
    rectResult.left = left * flXScale + rectBasis.left;
    rectResult.top = top * flYScale + rectBasis.top;
}

void Rect2D::FromUnitCoords(const IRect2D& rectBasis, IRect2D& rectResult) const {
    float flXScale = rectBasis.Width();
    float flYScale = rectBasis.Height();

    rectResult.right = Math::FloatToInt(right * flXScale) + rectBasis.left;
    rectResult.bottom = Math::FloatToInt(bottom * flYScale) + rectBasis.top;
    rectResult.left = Math::FloatToInt(left * flXScale) + rectBasis.left;
    rectResult.top = Math::FloatToInt(top * flYScale) + rectBasis.top;
}

void Rect2D::AlignToRect(const Rect2D& rectParent, const AlignPosition eHorizontal, const AlignPosition eVertical,
                         const float flPadding) {
    float flWidth = right - left;
    float flHeight = bottom - top;

    switch (eHorizontal) {
    case AP_HORIZONTAL_LEFT:
        right = rectParent.left - flPadding;
        left = right - flWidth;
        break;
    case AP_HORIZONTAL_INSIDE_LEFT:
        left = rectParent.left;
        right = left + flWidth;
        break;
    case AP_VERTICAL_CENTER:
    case AP_HORIZONTAL_CENTER: {
        float flParentWidth = rectParent.right - rectParent.left + 1.0f;
        float flParentCenter = rectParent.left + flParentWidth * 0.5f;
        left = flParentCenter - flWidth * 0.5f;
        right = left + flWidth;
        break;
    }
    case AP_HORIZONTAL_INSIDE_RIGHT:
        right = rectParent.right;
        left = right - flWidth;
        break;
    case AP_HORIZONTAL_RIGHT:
        left = rectParent.right + flPadding;
        right = left + flWidth;
        break;
    default:
        break;
    }

    switch (eVertical) {
    case AP_VERTICAL_ABOVE:
        bottom = rectParent.top - flPadding;
        top = bottom - flHeight;
        break;
    case AP_VERTICAL_TOP:
        top = rectParent.top;
        bottom = top + flHeight;
        break;
    case AP_VERTICAL_CENTER:
    case AP_HORIZONTAL_CENTER: {
        float flParentHeight = rectParent.bottom - rectParent.top + 1.0f;
        float flParentCenter = rectParent.top + flParentHeight * 0.5f;
        top = flParentCenter - flHeight * 0.5f;
        bottom = top + flHeight;
        break;
    }
    case AP_VERTICAL_BOTTOM:
        bottom = rectParent.bottom;
        top = bottom - flHeight;
        break;
    case AP_VERTICAL_BELOW:
        top = rectParent.bottom + flPadding;
        bottom = top + flHeight;
        break;
    default:
        break;
    }
}

void Rect2D::ConstrainToRect(const Rect2D& rectParent, const float flIndent) {
    float flXOffset = left - rectParent.left - flIndent;
    if (flXOffset < 0.0f) {
        left -= flXOffset;
        right -= flXOffset;
    }
    flXOffset = right - rectParent.right + flIndent;
    if (flXOffset > 0.0f) {
        left -= flXOffset;
        right -= flXOffset;
    }

    float flYOffset = top - rectParent.top - flIndent;
    if (flYOffset < 0.0f) {
        top -= flYOffset;
        bottom -= flYOffset;
    }
    flYOffset = bottom - rectParent.bottom + flIndent;
    if (flYOffset > 0.0f) {
        top -= flYOffset;
        bottom -= flYOffset;
    }
}

void AABB::ComputeBounds(Vector3D& wpMin, Vector3D& wpMax) const {
    Vector3D wp0 = wp - wvExt;
    Vector3D wp1 = wp + wvExt;

    wpMin.Set(Math::Min(wp0.x, wp1.x),
              Math::Min(wp0.y, wp1.y),
              Math::Min(wp0.z, wp1.z));

    wpMax.Set(Math::Max(wp0.x, wp1.x),
              Math::Max(wp0.y, wp1.y),
              Math::Max(wp0.z, wp1.z));
}

inline void AABB::ComputeBounds(Vector3D* wpBounds) const {
    wpBounds[0] = wp - wvExt;
    wpBounds[7] = wp + wvExt;

    wpBounds[1].Set(wpBounds[7].x, wpBounds[0].y, wpBounds[0].z);
    wpBounds[2].Set(wpBounds[0].x, wpBounds[7].y, wpBounds[0].z);
    wpBounds[3].Set(wpBounds[7].x, wpBounds[7].y, wpBounds[0].z);
    wpBounds[4].Set(wpBounds[0].x, wpBounds[0].y, wpBounds[7].z);
    wpBounds[5].Set(wpBounds[7].x, wpBounds[0].y, wpBounds[7].z);
    wpBounds[6].Set(wpBounds[0].x, wpBounds[7].y, wpBounds[7].z);
}

void AABB::GetMinimalBoundingSphere(Sphere& ws) const {
    ws.wpCenter = wp;
    ws.wdRadius = wvExt.Magnitude() * cgflSqrt3;
}

void AABB::Inflate(float flAmount) {
    float flHalfAmount = flAmount * 0.5f;
    wvExt.x += flHalfAmount;
    wvExt.y += flHalfAmount;
    wvExt.z += flHalfAmount;
}

void AABB::ComputeEdges(LineSeg3D* wpEdges) const {
    Vector3D wpBounds[8];
    ComputeBounds(wpBounds);

    wpEdges[0].Set(wpBounds[0], wpBounds[1]);
    wpEdges[1].Set(wpBounds[1], wpBounds[3]);
    wpEdges[2].Set(wpBounds[3], wpBounds[2]);
    wpEdges[3].Set(wpBounds[2], wpBounds[0]);
    wpEdges[4].Set(wpBounds[4], wpBounds[5]);
    wpEdges[5].Set(wpBounds[5], wpBounds[7]);
    wpEdges[6].Set(wpBounds[7], wpBounds[6]);
    wpEdges[7].Set(wpBounds[6], wpBounds[4]);
    wpEdges[8].Set(wpBounds[0], wpBounds[4]);
    wpEdges[9].Set(wpBounds[1], wpBounds[5]);
    wpEdges[10].Set(wpBounds[2], wpBounds[6]);
    wpEdges[11].Set(wpBounds[3], wpBounds[7]);
}

void OBB::GetBasisVectors(Vector3D& wvU, Vector3D& wvV, Vector3D& wvW) const {
    wvU = transform.Rotate(Vector3D(wvExt.x, 0.0f, 0.0f));
    wvV = transform.Rotate(Vector3D(0.0f, wvExt.y, 0.0f));
    wvW = transform.Rotate(Vector3D(0.0f, 0.0f, wvExt.z));
}

void OBB::ComputeBounds(Vector3D* wpBounds) const {
    Vector3D wvU;
    Vector3D wvV;
    Vector3D wvW;
    GetBasisVectors(wvU, wvV, wvW);

    wpBounds[0] = transform.wp - wvU - wvV - wvW;
    wpBounds[1] = transform.wp + wvU - wvV - wvW;
    wpBounds[2] = transform.wp - wvU + wvV - wvW;
    wpBounds[3] = transform.wp + wvU + wvV - wvW;
    wpBounds[4] = transform.wp - wvU - wvV + wvW;
    wpBounds[5] = transform.wp + wvU - wvV + wvW;
    wpBounds[6] = transform.wp - wvU + wvV + wvW;
    wpBounds[7] = transform.wp + wvU + wvV + wvW;
}

void OBB::ComputeEdges(LineSeg3D* wpEdges) const {
    Vector3D wpBounds[8];
    ComputeBounds(wpBounds);

    wpEdges[0].Set(wpBounds[0], wpBounds[1]);
    wpEdges[1].Set(wpBounds[1], wpBounds[3]);
    wpEdges[2].Set(wpBounds[3], wpBounds[2]);
    wpEdges[3].Set(wpBounds[2], wpBounds[0]);
    wpEdges[4].Set(wpBounds[4], wpBounds[5]);
    wpEdges[5].Set(wpBounds[5], wpBounds[7]);
    wpEdges[6].Set(wpBounds[7], wpBounds[6]);
    wpEdges[7].Set(wpBounds[6], wpBounds[4]);
    wpEdges[8].Set(wpBounds[0], wpBounds[4]);
    wpEdges[9].Set(wpBounds[1], wpBounds[5]);
    wpEdges[10].Set(wpBounds[2], wpBounds[6]);
    wpEdges[11].Set(wpBounds[3], wpBounds[7]);
}
