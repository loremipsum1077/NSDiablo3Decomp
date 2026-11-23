// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nvn/nvn.h

#pragma once

enum NVNshaderStage {
    NVN_SHADER_STAGE_VERTEX = 0,
    NVN_SHADER_STAGE_FRAGMENT = 1,
    NVN_SHADER_STAGE_GEOMETRY = 2,
    NVN_SHADER_STAGE_TESS_CONTROL = 3,
    NVN_SHADER_STAGE_TESS_EVALUATION = 4,
    NVN_SHADER_STAGE_COMPUTE = 5,
    NVN_SHADER_STAGE_LARGE = 2147483647,
};

typedef NVNshaderStage NVNshaderStage;

enum NVNshaderStageBits {
    NVN_SHADER_STAGE_VERTEX_BIT = 1,
    NVN_SHADER_STAGE_FRAGMENT_BIT = 2,
    NVN_SHADER_STAGE_GEOMETRY_BIT = 4,
    NVN_SHADER_STAGE_TESS_CONTROL_BIT = 8,
    NVN_SHADER_STAGE_TESS_EVALUATION_BIT = 16,
    NVN_SHADER_STAGE_COMPUTE_BIT = 32,
    NVN_SHADER_STAGE_ALL_GRAPHICS_BITS = 31,
    NVN_SHADER_STAGE_BITS_LARGE = 2147483647,
};

typedef NVNshaderStageBits NVNshaderStageBits;

