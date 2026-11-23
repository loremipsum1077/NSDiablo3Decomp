inline void Triangle2D::Set(const Vector2D& _wp0, const Vector2D& _wp1, const Vector2D& _wp2) {
    wp0 = _wp0;
    wp1 = _wp1;
    wp2 = _wp2;
}

// line 29
inline BOOL Triangle2D::ContainsPoint(const Vector2D& p) const {
    float flB0 = (wp1.x - wp0.x) * (wp2.y - wp0.y) - (wp2.x - wp0.x) * (wp1.y - wp0.y);
    if (Math::Abs(flB0) < 1e-6f)
        return FALSE;

    float flB1 = (wp2.x - p.x) * (wp0.y - p.y) - (wp0.x - p.x) * (wp2.y - p.y);
    if (flB1 < 0.0f && flB0 > 0.0f)
        return FALSE;
    if (flB1 > 0.0f && flB0 < 0.0f)
        return FALSE;

    float flB2 = (wp0.x - p.x) * (wp1.y - p.y) - (wp1.x - p.x) * (wp0.y - p.y);
    if (flB2 < 0.0f && flB0 > 0.0f)
        return FALSE;
    if (flB2 > 0.0f && flB0 < 0.0f)
        return FALSE;

    float flB3 = (wp1.x - p.x) * (wp2.y - p.y) - (wp2.x - p.x) * (wp1.y - p.y);
    if (flB3 < 0.0f && flB0 > 0.0f)
        return FALSE;
    if (flB3 > 0.0f && flB0 < 0.0f)
        return FALSE;

    return TRUE;
}

inline void Triangle3D::Set(const Vector3D& _wp0, const Vector3D& _wp1, const Vector3D& _wp2) {
    wp0 = _wp0;
    wp1 = _wp1;
    wp2 = _wp2;
}

// line 158
inline void Triangle3D::GetFaceNormal(Vector3D& wvFaceNormal) const {
    wvFaceNormal = Vector3D::CrossProduct(wp1 - wp0, wp2 - wp0);
    wvFaceNormal.Normalize();
}

// line 169
inline void Triangle3D::PointFromBarycentric(const float& flU, const float& flV, Vector3D& wp) const {
    float flW = 1.0f - flU - flV;
    wp = wp0 * flW + wp1 * flU + wp2 * flV;
}
