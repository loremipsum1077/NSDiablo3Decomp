inline BOOL IVector2D::operator==(const IVector2D& wpSrc) const {
    return x == wpSrc.x && y == wpSrc.y;
}

inline BOOL IVector2D::operator!=(const IVector2D& wpSrc) const {
    return x != wpSrc.x || y != wpSrc.y;
}

inline IVector2D IVector2D::operator+(const IVector2D& l) const {
    return IVector2D(x + l.x, y + l.y);
}

inline IVector2D IVector2D::operator-(const IVector2D& l) const {
    return IVector2D(x - l.x, y - l.y);
}

inline void IVector2D::Set(const int32 _x, const int32 _y) {
    x = _x;
    y = _y;
}

inline BOOL Vector2D::operator==(const Vector2D& wpSrc) const {
    return x == wpSrc.x && y == wpSrc.y;
}

inline BOOL Vector2D::operator!=(const Vector2D& wpSrc) const {
    return x != wpSrc.x || y != wpSrc.y;
}

inline Vector2D& Vector2D::operator=(const Vector2D& wpSrc) {
    x = wpSrc.x;
    y = wpSrc.y;
    return *this;
}

inline Vector2D Vector2D::operator+(const Vector2D& l) const {
    return Vector2D(x + l.x, y + l.y);
}

inline Vector2D& Vector2D::operator+=(const Vector2D& l) {
    x += l.x;
    y += l.y;
    return *this;
}

inline Vector2D Vector2D::operator-(const Vector2D& l) const {
    return Vector2D(x - l.x, y - l.y);
}

inline Vector2D& Vector2D::operator-=(const Vector2D& l) {
    x -= l.x;
    y -= l.y;
    return *this;
}

inline Vector2D operator*(const Vector2D& vec, const float flScale) {
    return Vector2D(vec.x * flScale, vec.y * flScale);
}

inline Vector2D operator*(const float flScale, const Vector2D& vec) {
    return Vector2D(vec.x * flScale, vec.y * flScale);  // original rows 189:22/24/39/41 (same columns as 181)
}

inline Vector2D& Vector2D::operator*=(const float flScale) {
    x *= flScale;
    y *= flScale;
    return *this;
}

// Line 443. Body written from the copy inlined into UIMinimap_SetObjectiveCone.
inline Angle Vector2D::ToYaw() const {
    return Math::ATan(y, x);
}

inline Vector2D Vector2D::operator-() const {
    return Vector2D(-x, -y);
}

inline const float* Vector2D::AsFloatArray() const {
    return &x;
}

inline void Vector2D::Set(const float _x, const float _y) {
    x = _x;
    y = _y;
}

inline float Vector2D::DistanceSquared(const Vector2D& wp0, const Vector2D& wp1) {
    float x = wp1.x - wp0.x;
    float y = wp1.y - wp0.y;
    return x * x + y * y;
}

inline float Vector3D::Distance2DSquared(const Vector3D& wp0, const Vector3D& wp1) {
    float x = wp1.x - wp0.x;
    float y = wp1.y - wp0.y;
    return x * x + y * y;
}

inline float Vector2D::Distance(const Vector2D& wp0, const Vector2D& wp1) {
    return Math::Sqrt(DistanceSquared(wp0, wp1));
}

inline float Vector3D::Distance2D(const Vector3D& wp0, const Vector3D& wp1) {
    float x = wp1.x - wp0.x;
    float y = wp1.y - wp0.y;
    return Math::Sqrt(x * x + y * y);
}

inline float Vector2D::Magnitude() const {
    return Math::Sqrt(x * x + y * y);
}

inline float Vector2D::MagnitudeSquared() const {
    return x * x + y * y;
}

inline float Vector2D::Normalize() {
    float wdMag = Magnitude();
    if (wdMag <= EPSILON) {
        return 0.0f;
    }

    float wdReciprocalMag = 1.0f / wdMag;
    *this *= wdReciprocalMag;
    return wdMag;
}

inline float Vector2D::DotProduct(const Vector2D& wvA, const Vector2D& wvB) {
    return wvA.x * wvB.x + wvA.y * wvB.y;
}

inline float Vector2D::CrossProduct(const Vector2D& wvA, const Vector2D& wvB) {
    return wvA.x * wvB.y - wvA.y * wvB.x;
}

// Line 427. The body is written from the copy inlined into CameraManagerBuildAdditiveCamera.
inline void Vector2D::Rotate(Angle angle) {
    float fSine = Math::Sin(angle);
    float fCos = Math::Cos(angle);

    // Invented temporary name; preserves the input components until both results are computed.
    Vector2D vecResult(fCos * x - fSine * y, fSine * x + fCos * y);
    *this = vecResult;
}

inline Vector3D& Vector3D::operator=(const Vector3D& wpSrc) {
    x = wpSrc.x;
    y = wpSrc.y;
    z = wpSrc.z;
    return *this;
}

inline BOOL Vector3D::operator==(const Vector3D& wpSrc) const {
    return x == wpSrc.x && y == wpSrc.y && z == wpSrc.z;
}

inline BOOL Vector3D::operator!=(const Vector3D& wpSrc) const {
    return x != wpSrc.x || y != wpSrc.y || z != wpSrc.z;
}

inline Vector3D Vector3D::operator+(const Vector3D& l) const {
    return Vector3D(x + l.x, y + l.y, z + l.z);
}

inline Vector3D& Vector3D::operator+=(const Vector3D& l) {
    x += l.x;
    y += l.y;
    z += l.z;
    return *this;
}

inline Vector3D Vector3D::operator-(const Vector3D& l) const {
    return Vector3D(x - l.x, y - l.y, z - l.z);
}

inline Vector3D& Vector3D::operator-=(const Vector3D& l) {
    x -= l.x;
    y -= l.y;
    z -= l.z;
    return *this;
}

inline Vector3D operator*(const Vector3D& vec, const float flScale) {
    return Vector3D(vec.x * flScale, vec.y * flScale, vec.z * flScale);
}

inline Vector3D operator*(const float flScale, const Vector3D& vec) {
    return Vector3D(vec.x * flScale, vec.y * flScale, vec.z * flScale);  // original rows 590:22/24/39/41/56/58 (same columns as 582)
}

inline Vector3D& Vector3D::operator*=(const float flScale) {
    x *= flScale;
    y *= flScale;
    z *= flScale;
    return *this;
}

inline Vector3D Vector3D::operator-() const {
    return Vector3D(-x, -y, -z);
}

// Original line 637; the three stores are emitted in x/y/z order.
inline void Vector3D::Set(const float _x, const float _y, const float _z) {
    x = _x;
    y = _y;
    z = _z;
}

inline float Vector3D::DistanceSquared(const Vector3D& wp0, const Vector3D& wp1) {
    float x = wp1.x - wp0.x;
    float y = wp1.y - wp0.y;
    float z = wp1.z - wp0.z;
    return x * x + y * y + z * z;
}

inline float Vector3D::Distance(const Vector3D& wp0, const Vector3D& wp1) {
    return Math::Sqrt(DistanceSquared(wp0, wp1));
}

inline float Vector3D::Magnitude() const {
    return Math::Sqrt(x * x + y * y + z * z);
}

inline float Vector3D::MagnitudeSquared() const {
    return x * x + y * y + z * z;
}

inline float Vector3D::Normalize() {
    float wdMag = Magnitude();
    if (wdMag <= EPSILON) {
        return 0.0f;
    }

    float wdReciprocalMag = 1.0f / wdMag;
    *this *= wdReciprocalMag;
    return wdMag;
}

// Original Vector.inl:726 (a file-scope const, DW_AT_const_value 0x3c23d70a) and 728-733: wdMag at
// 731 (MagnitudeSquared), Math::Abs at 732. Inlined into sRActorCreate (RActors.cpp:1127) and
// ProjectileSeekPlace (Projectile.cpp:1460); 1.0f - wdMag and the <= test are from their code.
static const float EPSILON_FOR_NORMALIZED = 0.01f;

inline BOOL Vector3D::IsNormalized() const {
    float wdMag = MagnitudeSquared();
    return Math::Abs(1.0f - wdMag) <= EPSILON_FOR_NORMALIZED;
}

inline float Vector3D::DotProduct(const Vector3D& wvA, const Vector3D& wvB) {
    return wvA.x * wvB.x + wvA.y * wvB.y + wvA.z * wvB.z;
}

inline Vector3D Vector3D::CrossProduct(const Vector3D& wvA, const Vector3D& wvB) {
    return Vector3D(wvA.y * wvB.z - wvA.z * wvB.y,
                    wvA.z * wvB.x - wvA.x * wvB.z,
                    wvA.x * wvB.y - wvA.y * wvB.x);
}

inline Vector3D Vector3D::Min(const Vector3D& wvA, const Vector3D& wvB) {
    return Vector3D(Math::Min(wvA.x, wvB.x), Math::Min(wvA.y, wvB.y), Math::Min(wvA.z, wvB.z));
}

inline Vector3D Vector3D::Max(const Vector3D& wvA, const Vector3D& wvB) {
    return Vector3D(Math::Max(wvA.x, wvB.x), Math::Max(wvA.y, wvB.y), Math::Max(wvA.z, wvB.z));
}

inline Vector3D Vector3D::Abs(const Vector3D& wv) {
    return Vector3D(Math::Abs(wv.x), Math::Abs(wv.y), Math::Abs(wv.z));
}

// Line 774. The body is written from the copy inlined into sUpdateChildCameras (CameraManager.cpp).
inline Vector3D Vector3D::Reflect(const Vector3D& wv, const Vector3D& wvNormal) {
    return wvNormal * 2.0f * DotProduct(wv, wvNormal) - wv;
}

inline Vector4D Vector4D::operator-() const {
    return Vector4D(-x, -y, -z, -w);
}

inline void Vector4D::Set(const float _x, const float _y, const float _z, const float _w) {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

// Line 785. The body is written from the copy inlined into SoundInstance::SetPosition (Sound.cpp).
inline Vector3D Vector3D::Projection(const Vector3D& wv, const Vector3D& wvProjectionAxis) {
    return wvProjectionAxis * (DotProduct(wv, wvProjectionAxis) / wvProjectionAxis.MagnitudeSquared());
}
