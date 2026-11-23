inline Matrix3x3 Matrix3x3::operator*(const Matrix3x3& m) const {
    Matrix3x3 mResult;

    mResult._11 = _11 * m._11 + _12 * m._21 + _13 * m._31;
    mResult._12 = _11 * m._12 + _12 * m._22 + _13 * m._32;
    mResult._13 = _11 * m._13 + _12 * m._23 + _13 * m._33;

    mResult._21 = _21 * m._11 + _22 * m._21 + _23 * m._31;
    mResult._22 = _21 * m._12 + _22 * m._22 + _23 * m._32;
    mResult._23 = _21 * m._13 + _22 * m._23 + _23 * m._33;

    mResult._31 = _31 * m._11 + _32 * m._21 + _33 * m._31;
    mResult._32 = _31 * m._12 + _32 * m._22 + _33 * m._32;
    mResult._33 = _31 * m._13 + _32 * m._23 + _33 * m._33;

    return mResult;
}

inline Matrix3x3 Matrix3x3::operator*(const float flScale) const {
    Matrix3x3 mResult;

    mResult._11 = _11 * flScale;
    mResult._12 = _12 * flScale;
    mResult._13 = _13 * flScale;

    mResult._21 = _21 * flScale;
    mResult._22 = _22 * flScale;
    mResult._23 = _23 * flScale;

    mResult._31 = _31 * flScale;
    mResult._32 = _32 * flScale;
    mResult._33 = _33 * flScale;

    return mResult;
}

inline Matrix3x3& Matrix3x3::operator*=(const float flScale) {
    *this = *this * flScale;
    return *this;
}

inline Matrix3x3 Matrix3x3::Inverse() const {
    float flDet = Determinant();

    if (Math::Abs(flDet) > EPSILON) {
        float flInvDet = 1.0f / flDet;
        Matrix3x3 mResult;
        Adjoint(mResult);
        mResult *= flInvDet;
        return mResult;
    }

    return cgm3x3Identity;
}

inline void Matrix3x3::SetBasisVectors(const Vector3D& wv0, const Vector3D& wv1, const Vector3D& wv2) {
    SetBasisVectorX(wv0);
    SetBasisVectorY(wv1);
    SetBasisVectorZ(wv2);
}

inline void Matrix3x3::SetBasisVectorX(const Vector3D& vBasis) {
    _11 = vBasis.x;
    _12 = vBasis.y;
    _13 = vBasis.z;
}

inline void Matrix3x3::SetBasisVectorY(const Vector3D& vBasis) {
    _21 = vBasis.x;
    _22 = vBasis.y;
    _23 = vBasis.z;
}

inline void Matrix3x3::SetBasisVectorZ(const Vector3D& vBasis) {
    _31 = vBasis.x;
    _32 = vBasis.y;
    _33 = vBasis.z;
}

inline float Matrix3x3::Determinant() const {
    return _11 * _22 * _33 - _11 * _23 * _32 + _12 * _23 * _31 - _12 * _21 * _33 + _13 * _21 * _32 -
           _13 * _22 * _31;
}

inline void Matrix3x3::Adjoint(Matrix3x3& mResult) const {
    mResult._11 = _22 * _33 - _23 * _32;
    mResult._12 = -_12 * _33 + _13 * _32;
    mResult._13 = _12 * _23 - _13 * _22;

    mResult._21 = -_21 * _33 + _23 * _31;
    mResult._22 = _11 * _33 - _13 * _31;
    mResult._23 = -_11 * _23 + _13 * _21;

    mResult._31 = _21 * _32 - _22 * _31;
    mResult._32 = -_11 * _32 + _12 * _31;
    mResult._33 = _11 * _22 - _12 * _21;
}

inline void Matrix3x3::SetChangeBasis(const Vector3D& wvSrcX, const Vector3D& wvSrcY, const Vector3D& wvSrcZ,
                                      const Vector3D& wvDestX, const Vector3D& wvDestY,
                                      const Vector3D& wvDestZ) {
    Matrix3x3 mSrc;
    Matrix3x3 mDest;

    mSrc.SetBasisVectors(wvSrcX, wvSrcY, wvSrcZ);
    mDest.SetBasisVectors(wvDestX, wvDestY, wvDestZ);
    *this = mSrc.Inverse() * mDest;
}

inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4& m) const {
    return Matrix4x4(_11 * m._11 + _12 * m._21 + _13 * m._31 + _14 * m._41,
                     _11 * m._12 + _12 * m._22 + _13 * m._32 + _14 * m._42,
                     _11 * m._13 + _12 * m._23 + _13 * m._33 + _14 * m._43,
                     _11 * m._14 + _12 * m._24 + _13 * m._34 + _14 * m._44,
                     _21 * m._11 + _22 * m._21 + _23 * m._31 + _24 * m._41,
                     _21 * m._12 + _22 * m._22 + _23 * m._32 + _24 * m._42,
                     _21 * m._13 + _22 * m._23 + _23 * m._33 + _24 * m._43,
                     _21 * m._14 + _22 * m._24 + _23 * m._34 + _24 * m._44,
                     _31 * m._11 + _32 * m._21 + _33 * m._31 + _34 * m._41,
                     _31 * m._12 + _32 * m._22 + _33 * m._32 + _34 * m._42,
                     _31 * m._13 + _32 * m._23 + _33 * m._33 + _34 * m._43,
                     _31 * m._14 + _32 * m._24 + _33 * m._34 + _34 * m._44,
                     _41 * m._11 + _42 * m._21 + _43 * m._31 + _44 * m._41,
                     _41 * m._12 + _42 * m._22 + _43 * m._32 + _44 * m._42,
                     _41 * m._13 + _42 * m._23 + _43 * m._33 + _44 * m._43,
                     _41 * m._14 + _42 * m._24 + _43 * m._34 + _44 * m._44);
}

inline void Matrix4x4::Set(const Matrix3x3& mRotation, const Vector3D& wpTranslation) {
    _11 = mRotation._11;
    _12 = mRotation._12;
    _13 = mRotation._13;
    _14 = 0.0f;
    _21 = mRotation._21;
    _22 = mRotation._22;
    _23 = mRotation._23;
    _24 = 0.0f;
    _31 = mRotation._31;
    _32 = mRotation._32;
    _33 = mRotation._33;
    _34 = 0.0f;
    _41 = wpTranslation.x;
    _42 = wpTranslation.y;
    _43 = wpTranslation.z;
    _44 = 1.0f;
}

// Line 626. The original reads every element (lines 629 to 643, two per line) before writing any
// (646 to 660), so a self-assignment is safe; the DWARF keeps the local fl11.
inline Matrix4x4& Matrix4x4::operator=(const Matrix4x4& m) {
    float fl11 = m._11, fl12 = m._12;
    float fl13 = m._13, fl14 = m._14;
    float fl21 = m._21, fl22 = m._22;
    float fl23 = m._23, fl24 = m._24;
    float fl31 = m._31, fl32 = m._32;
    float fl33 = m._33, fl34 = m._34;
    float fl41 = m._41, fl42 = m._42;
    float fl43 = m._43, fl44 = m._44;

    _11 = fl11; _12 = fl12;
    _13 = fl13; _14 = fl14;
    _21 = fl21; _22 = fl22;
    _23 = fl23; _24 = fl24;
    _31 = fl31; _32 = fl32;
    _33 = fl33; _34 = fl34;
    _41 = fl41; _42 = fl42;
    _43 = fl43; _44 = fl44;
    return *this;
}

// Line 668; one element per line at 672-687. The constructor form is proven by GfxShaderSetTextures
// (X/Client/Gfx/GfxShader.cpp, through Matrix4x4::Inverse); a local filled member by member does not match there.
inline Matrix4x4 Matrix4x4::operator*(const float flScale) const {
    return Matrix4x4(
        flScale * _11,
        flScale * _12,
        flScale * _13,
        flScale * _14,
        flScale * _21,
        flScale * _22,
        flScale * _23,
        flScale * _24,
        flScale * _31,
        flScale * _32,
        flScale * _33,
        flScale * _34,
        flScale * _41,
        flScale * _42,
        flScale * _43,
        flScale * _44);
}
inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& m) { *this = *this * m; return *this; }  // line 619; operator= inlined and operator* called at line 622 (CameraSetPerspectiveProjection)
inline Matrix4x4& Matrix4x4::operator*=(const float flScale) {
    *this = *this * flScale;
    return *this;
}

// Line 938. No inline site survives in CameraManager DWARF; this body is recovered
// from the transposed stores in sUpdateChildCameras. Other compile units retain inline sites.
inline Matrix4x4 Matrix4x4::Transpose() const {
    return Matrix4x4(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44);
}

inline Vector4D operator*(const Vector4D& v, const Matrix4x4& m) {
    Vector4D vResult;

    vResult.x = v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41;
    vResult.y = v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42;
    vResult.z = v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43;
    vResult.w = v.x * m._14 + v.y * m._24 + v.z * m._34 + v.w * m._44;

    return vResult;
}

inline Matrix4x4 Matrix4x4::Inverse() const {
    float flDet = Determinant();
    Matrix4x4 mResult;

    if (Math::Abs(flDet) > EPSILON) {
        float flInvDet = 1.0f / flDet;
        Adjoint(mResult);
        mResult *= flInvDet;
        return mResult;
    }

    return cgm4x4Identity;
}

// Line 540. Body written from the copy inlined into UIMinimap::sInput.
inline Vector2D Matrix3x3::Transform2x2(const Vector2D& v, const Matrix3x3& m) {
    return Vector2D(m._11 * v.x + m._21 * v.y, m._12 * v.x + v.y * m._22);
}

// Line 191. Body written from the copies inlined into UIMinimap.cpp (sWorldToMapUIC).
inline Vector2D operator*(const Vector2D& v, const Matrix3x3& m) {
    return Vector2D(m._11 * v.x + m._21 * v.y + m._31, v.x * m._12 + v.y * m._22 + m._32);
}


// Line 219 (parameter 220, identity copy 222). Body written from the copy inlined into
// UIMinimap::UpdateTransform (X/Client/UserInterface/Interface/UIMinimap.cpp:3772): the product there
// keeps the identity entries and scales _11/_22 only.
inline void Matrix3x3::SetScale(const float flScale) {
    *this = cgm3x3Identity;
    _11 = flScale;
    _22 = flScale;
}

// Line 278 (parameter 279, locals 281/282, identity copy 284). Body written from the copy inlined into
// UIMinimap::UpdateTransform (UIMinimap.cpp:3769): Sin before Cos, identity in the untouched entries.
inline void Matrix3x3::SetRotationZ(const Angle ang) {
    float dSineAngle = Math::Sin(ang);
    float dCosineAngle = Math::Cos(ang);

    *this = cgm3x3Identity;
    _11 = dCosineAngle;
    _12 = dSineAngle;
    _21 = -dSineAngle;
    _22 = dCosineAngle;
}

inline Vector3D operator*(const Vector3D& v, const Matrix3x3& m) {
    return Vector3D(v.x * m._11 + v.y * m._21 + v.z * m._31, v.x * m._12 + v.y * m._22 + v.z * m._32,
                    v.x * m._13 + v.y * m._23 + v.z * m._33);
}

inline Vector3D Matrix4x4::ExtractBasisVectorZ() const {
    return Vector3D(_31, _32, _33);
}

// Line 993 (statements at 1000-1002). The x row multiplies matrix-first: GfxShaderSetLights
// (X/Client/Gfx/GfxShader.cpp) only matches that operand order.
inline Vector3D operator*(const Vector3D& v, const Matrix4x4& m) {
    Vector3D vResult;

    vResult.x = m._11 * v.x + m._21 * v.y + m._31 * v.z + m._41;
    vResult.y = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
    vResult.z = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;

    return vResult;
}

// Line 978 (statements at 985-987). Body written from the copies inlined into sSetCommonVSConstants and
// GfxShaderSetLights (X/Client/Gfx/GfxShader.cpp).
inline Vector3D Matrix4x4::Transform3x3(const Vector3D& v, const Matrix4x4& m) {
    Vector3D vResult;

    vResult.x = m._11 * v.x + m._21 * v.y + m._31 * v.z;
    vResult.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
    vResult.z = v.x * m._13 + v.y * m._23 + v.z * m._33;

    return vResult;
}

// Line 918 (return on 921). The one inlined copy (sGetUVCoordsForTexture, X/Client/ParticleRenderUpdate.cpp:761)
// reads _41 and _42 only.
inline Vector3D Matrix4x4::ExtractTranslation() const {
    return Vector3D(_41, _42, _43);
}

// Line 319 (locals 323-326, statements 328-336). Body written from the copy inlined into
// sCreateSrcLight (X/Client/Weather.cpp:1110): the products at 328/329/334/335 and the zero
// _23 / unit _33 entries follow from the Vector3D(0, 0, 1) * mLight rows there.
inline void Matrix3x3::SetAltAz(const Angle aAltitude, const Angle aAzimuth) {
    float flCosAz = Math::Cos(aAzimuth);
    float flCosAlt = Math::Cos(aAltitude);
    float flSinAz = Math::Sin(aAzimuth);
    float flSinAlt = Math::Sin(aAltitude);

    _11 = flCosAz * flCosAlt;
    _12 = flCosAlt * flSinAz;
    _13 = flSinAlt;
    _21 = -flSinAz;
    _22 = flCosAz;
    _23 = 0.0f;
    _31 = -flCosAz * flSinAlt;
    _32 = -flSinAz * flSinAlt;
    _33 = flCosAlt;
}
