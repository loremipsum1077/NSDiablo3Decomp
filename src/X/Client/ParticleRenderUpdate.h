// Subset of X/Client/ParticleRenderUpdate.h (the ScreenManager.h include is left out), plus the
// declarations of the X/Client/ParticleRenderUpdate.cpp functions Particle.cpp calls.
// Original file: X/Client/ParticleRenderUpdate.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/RGBAColor.h"
#include "../Sigma/SigmaTypes.h"

struct TexAnimState;
struct TextureDefinition;
struct TextureFramesInfo;
struct ParticleSystem;
struct VertexBillboardParticle;
class Frustum;

struct ParticleSystemRenderUpdateParams {  // size 0xb0
    uint32 dwFlags;  // 0x00
    float flScale;  // 0x04
    float flStretch;  // 0x08
    Angle aAng;  // 0x0c
    RGBAColor rgbaColor;  // 0x10
    RGBAColor rgbaAlphaAux;  // 0x14
    Vector3D wvFoliageOffset;  // 0x18
    const CHAR * pTexAnimCompParams[4];  // 0x28
    uint32 dwTexAnimCompParamsSize[4];  // 0x48
    const TexAnimState * tTexAnimState[4];  // 0x58
    const TextureDefinition * ptTextureDef[4];  // 0x78
    SNO snoTexAnimStateTex[4];  // 0x98
    int32 nTexStageCount;  // 0xa8
};
static_assert(sizeof(ParticleSystemRenderUpdateParams) == 0xb0, "ParticleSystemRenderUpdateParams");

// X/Client/ParticleRenderUpdate.cpp:1214
void ParticleSystemGenerateVertices(ParticleSystem * ptParticleSystem, VertexBillboardParticle * ptVertices, ParticleSystemRenderUpdateParams & tParams, const Vector3D & wpPlayer, const Frustum & tFrustum, const Vector3D & wvCameraForward, const TextureFramesInfo * atTexFramesInfo, float flDeltaTime);
