#include "Curve.h"

inline void CurveCatmullRomEvaluate(const Vector3D& wpControl0, const Vector3D& wpControl1,
                                    const Vector3D& wpControl2, const Vector3D& wpControl3, const float flT,
                                    Vector3D& wpResult) {
    float flTSqr = flT * flT;
    float flTCub = flTSqr * flT;

    float flC0 = (2.0f * flTSqr - flTCub - flT) * 0.5f;
    float flC1 = (3.0f * flTCub - 5.0f * flTSqr + 2.0f) * 0.5f;
    float flC2 = (4.0f * flTSqr - 3.0f * flTCub + flT) * 0.5f;
    float flC3 = (flTCub - flTSqr) * 0.5f;

    wpResult = wpControl0 * flC0 + wpControl1 * flC1 + wpControl2 * flC2 + wpControl3 * flC3;
}

void CurveCatmullRomEvaluate(const Vector3D* pwpControl, const float flT, Vector3D& wpResult) {
    CurveCatmullRomEvaluate(pwpControl[0], pwpControl[1], pwpControl[2], pwpControl[3], flT, wpResult);
}
