// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/TexAnimation.h
// Texture-animation state structs on the DWARF layouts; nothing here has code.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "AnimationCommon.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/Math/MathGeneral.h"

struct PRSTransform;
struct RActor;
struct TexFrame;

struct TexAnimState {  // size 0x48
    float flLastUpdated;  // 0x00
    float flUTrans;  // 0x04
    float flUTransVel;  // 0x08
    float flVTrans;  // 0x0c
    float flVTransVel;  // 0x10
    Angle aRot;  // 0x14
    Angle aRotVel;  // 0x18
    SNO snoTex;  // 0x1c
    AnimState tAnimState;  // 0x20
    union {  // size 0x8
        RActor * ptRActor;  // 0x00
        PRSTransform * ptTransform;  // 0x00
    };  // 0x38
    int32 nBone;  // 0x40
};
static_assert(sizeof(TexAnimState) == 0x48, "TexAnimState");

struct ObjectTexAnimParam {  // size 0x4
    float flValue;  // 0x00
};
static_assert(sizeof(ObjectTexAnimParam) == 0x4, "ObjectTexAnimParam");

enum TexAnimMatrixType {
    TAMT_IDENTITY = 0,
    TAMT_TRANSLATE_ONLY = 1,
    TAMT_ROTATE_ONLY = 2,
    TAMT_TRANSLATE_SCALE = 3,
    TAMT_ROTATE_TRANSLATE = 4,
};

struct TextureFramesInfo {  // size 0x18
    uint32 dwWidth;  // 0x00
    uint32 dwHeight;  // 0x04
    uint32 dwFrameCount;  // 0x08
    TexFrame * ptTexFrames;  // 0x10
};
static_assert(sizeof(TextureFramesInfo) == 0x18, "TextureFramesInfo");


struct UberMaterial;
struct RActor;
void MaterialFreeTexAnimState(TexAnimState *& arTexAnimStates);  // TexAnimation.cpp:1102
TexAnimState * MaterialAllocTexAnimState(const UberMaterial & tUberMaterial, int32 & nTexAnimStateCount);  // TexAnimation.cpp:1071
void TexAnimInitialize(const UberMaterial & tUberMaterial, TexAnimState * ptTexAnimState, RActor * ptRActor);  // TexAnimation.cpp:195
