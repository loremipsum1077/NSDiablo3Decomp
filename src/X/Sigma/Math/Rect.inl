inline void IRect2D::Set(const int32 _left, const int32 _top, const int32 _right, const int32 _bottom) {
    left = _left;
    top = _top;
    right = _right;
    bottom = _bottom;
}

inline uint32 IRect2D::Width() const {
    return right - left;
}

inline uint32 IRect2D::Height() const {
    return bottom - top;
}
inline BOOL IRect2D::IsEmpty() const { return Width() == 0 || Height() == 0; }  // line 190; tests at lines 193 and 43 (Height) in CameraScreenToWorldPoint
// The body is not in the DWARF; written from the copy inlined into
// X/Client/Gfx/NX64/NX64NVNRenderStates.cpp (GFXNX64NVN::ViewportSet).
inline BOOL IRect2D::operator!=(const IRect2D& rect) const {  // line 109
    return left != rect.left || top != rect.top || right != rect.right || bottom != rect.bottom;
}

// The body is not in the DWARF; written from the copies inlined into X/Common/ActorInventory.cpp
// (sTestSlotOverlap).
// Rect.inl:134; local rectIntersect at 139, Max/Min calls at 141-144 (DWARF inline tree of
// DRLGDungeonGenerator::DoesTilePassSpecialTileSpacing).
inline BOOL IRect2D::Intersect(const IRect2D & rectA, const IRect2D & rectB, IRect2D * prectIntersect) {
    IRect2D rectIntersect;
    rectIntersect.left = Math::Max(rectA.left, rectB.left);
    rectIntersect.right = Math::Min(rectA.right, rectB.right);
    rectIntersect.top = Math::Max(rectA.top, rectB.top);
    rectIntersect.bottom = Math::Min(rectA.bottom, rectB.bottom);
    if (rectIntersect.left > rectIntersect.right)
        return FALSE;
    if (prectIntersect)
        *prectIntersect = rectIntersect;
    return rectIntersect.top <= rectIntersect.bottom;
}

inline BOOL IRect2D::ContainsPoint(const IVector2D & p) const {  // line 84
    return p.x >= left && p.x < right && p.y >= top && p.y < bottom;
}

inline void Rect2D::Set(const float _left, const float _top, const float _right, const float _bottom) {
    left = _left;
    top = _top;
    right = _right;
    bottom = _bottom;
}

// The body is not in the DWARF; written from the copy inlined into X/Server/PlayerList.cpp
// (sIsInsideKnowRegion).
inline BOOL Rect2D::ContainsPoint(const float x, const float y) const {  // line 310
    return x >= left && x <= right && y >= top && y <= bottom;
}

// The bodies below are not in the DWARF; written from the copies inlined into
// X/Client/UserInterface/Controls/UIWindowControl.cpp.
inline BOOL Rect2D::IsEmpty() const {  // line 261
    if (right - left < EPSILON || bottom - top < EPSILON) {
        return TRUE;
    }
    return FALSE;
}

inline BOOL Rect2D::operator==(const Rect2D& rect) const {  // line 275
    return left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom;
}

inline BOOL Rect2D::operator!=(const Rect2D& rect) const {  // line 287
    return left != rect.left || top != rect.top || right != rect.right || bottom != rect.bottom;
}

inline BOOL Rect2D::ContainsPoint(const Vector2D& wp) const {  // line 299
    return wp.x >= left && wp.x <= right && wp.y >= top && wp.y <= bottom;
}

inline void Rect2D::Offset(const Vector2D& ptOffset, Rect2D& rectDest) const {  // line 333
    rectDest.left = left + ptOffset.x;
    rectDest.top = top + ptOffset.y;
    rectDest.right = right + ptOffset.x;
    rectDest.bottom = bottom + ptOffset.y;
}

inline void AABB::Set(const Vector3D& wp0, const Vector3D& wp1) {
    wp = (wp0 + wp1) * 0.5f;
    wvExt.x = Math::Abs(wp1.x - wp.x);
    wvExt.y = Math::Abs(wp1.y - wp.y);
    wvExt.z = Math::Abs(wp1.z - wp.z);
}

// Rect.inl:361.  The body is written from the copy inlined into
// X/Common/Marker/SmartSceneGroupMarker.cpp GetAABB (line 35): a local
// `wdExt`, Vector3D::Set inlined at Rect.inl:366, then the three wvExt stores.
inline void AABB::Set(const Vector3D wp0, const float wdSize) {  // line 361
    const float wdExt = wdSize * 0.5f;
    wp.Set(wp0.x + wdExt, wp0.y + wdExt, wp0.z + wdExt);
    wvExt.x = wdExt;
    wvExt.y = wdExt;
    wvExt.z = wdExt;
}

inline void AABB::Set(const OBB& obb) {
    Vector3D wpBounds[8];
    obb.ComputeBounds(wpBounds);

    Vector3D wpMin(MAX_WORLDDISTANCE, MAX_WORLDDISTANCE, MAX_WORLDDISTANCE);
    Vector3D wpMax(-MAX_WORLDDISTANCE, -MAX_WORLDDISTANCE, -MAX_WORLDDISTANCE);

    for (int32 i = 0; i < 8; i++) {
        wpMin.x = Math::Min(wpMin.x, wpBounds[i].x); wpMin.y = Math::Min(wpMin.y, wpBounds[i].y); wpMin.z = Math::Min(wpMin.z, wpBounds[i].z);
        wpMax.x = Math::Max(wpMax.x, wpBounds[i].x); wpMax.y = Math::Max(wpMax.y, wpBounds[i].y); wpMax.z = Math::Max(wpMax.z, wpBounds[i].z);
    }

    Set(wpMin, wpMax);
}

inline BOOL AABB::ContainsPoint(const Vector3D& _wp) const {
    return _wp.x >= wp.x - wvExt.x && _wp.x <= wp.x + wvExt.x &&
           _wp.y >= wp.y - wvExt.y && _wp.y <= wp.y + wvExt.y &&
           _wp.z >= wp.z - wvExt.z && _wp.z <= wp.z + wvExt.z;
}

// Rect.inl:446.  The body is written from the copy inlined into
// X/Common/Marker/SmartSceneGroupMarker.cpp GetAABB (line 51): per-axis
// Math::Min/Max at Rect.inl:453-458 then AABB::Set(wpMin, wpMax) at line 460.
inline void AABB::Union(const AABB& aabbA, const AABB& aabbB, AABB& aabbUnion) {  // line 446
    Vector3D wpMin, wpMax;
    wpMin.x = Math::Min(aabbA.wp.x - aabbA.wvExt.x, aabbB.wp.x - aabbB.wvExt.x);
    wpMax.x = Math::Max(aabbA.wp.x + aabbA.wvExt.x, aabbB.wp.x + aabbB.wvExt.x);
    wpMin.y = Math::Min(aabbA.wp.y - aabbA.wvExt.y, aabbB.wp.y - aabbB.wvExt.y);
    wpMax.y = Math::Max(aabbA.wp.y + aabbA.wvExt.y, aabbB.wp.y + aabbB.wvExt.y);
    wpMin.z = Math::Min(aabbA.wp.z - aabbA.wvExt.z, aabbB.wp.z - aabbB.wvExt.z);
    wpMax.z = Math::Max(aabbA.wp.z + aabbA.wvExt.z, aabbB.wp.z + aabbB.wvExt.z);
    aabbUnion.Set(wpMin, wpMax);
}

// Rect.inl:556. DWARF: wpTemp declared at 559 before transformInv at 560, Inverse at 560 and
// operator* at 561 (assigned, not copy-initialised). With this order the original's out-of-line
// calls in CWorldGetAudioVolumeForPlayerPosition and Weather::GetWeatherToUse are reproduced.
inline BOOL OBB::ContainsPoint(const Vector3D& wp) const {
    Vector3D wpTemp;
    PRTransform transformInv = transform.Inverse();
    wpTemp = transformInv * wp;

    return wpTemp.x >= -wvExt.x && wpTemp.x <= wvExt.x &&
           wpTemp.y >= -wvExt.y && wpTemp.y <= wvExt.y &&
           wpTemp.z >= -wvExt.z && wpTemp.z <= wvExt.z;
}

// Line 249. The body is not in the DWARF; written from TextureGetFrameRectUIScaledPixels
// (X/Client/Definitions/TextureDefinition.cpp), which converts each edge to float.
inline void Rect2D::Set(const IRect2D& rect) {
    left = (float)rect.left;
    top = (float)rect.top;
    right = (float)rect.right;
    bottom = (float)rect.bottom;
}
