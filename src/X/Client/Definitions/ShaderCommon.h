// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Definitions/ShaderCommon.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum ShaderConstType {
    SHADERCONSTTYPE_FLOAT = 0,
    SHADERCONSTTYPE_INT = 1,
    SHADERCONSTTYPE_BOOL = 2,
};

enum {
    MAX_CLIP_PLANES = 6,
    MAX_REFLECTION_PLANES = 2,
};

struct TextureBinding {  // size 0x4
    int32 nStage;  // 0x00
};
static_assert(sizeof(TextureBinding) == 0x4, "TextureBinding");

struct ShaderConstBinding {  // size 0x10
    ShaderConstType eType;  // 0x00
    uint32 dwSize;  // 0x04
    int32 nCBuffer;  // 0x08
    int32 nRegister;  // 0x0c
};
static_assert(sizeof(ShaderConstBinding) == 0x10, "ShaderConstBinding");

enum {
    SHADERCONSTFASTBINDING_PIXEL_SHADER_BIT = 0,
    SHADERCONSTFASTBINDING_VERTEX_SHADER_BIT = 1,
};

struct ShaderConstFastBinding {  // size 0x24
    uint32 dwFlags;  // 0x00
    ShaderConstBinding tVSBinding;  // 0x04
    ShaderConstBinding tPSBinding;  // 0x14
};
static_assert(sizeof(ShaderConstFastBinding) == 0x24, "ShaderConstFastBinding");

