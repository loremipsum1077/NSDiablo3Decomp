// Checks Matrix4x4::operator*(const Matrix4x4 &) (X/Sigma/Math/Matrix.inl). The binary keeps one
// out-of-line copy, attributed to X/Client/CGame.cpp; taking its address emits it here.
#include "X/Sigma/Math/Matrix.h"

Matrix4x4 (Matrix4x4::*g_pfnMatrix4x4Multiply)(const Matrix4x4 &) const = &Matrix4x4::operator*;
