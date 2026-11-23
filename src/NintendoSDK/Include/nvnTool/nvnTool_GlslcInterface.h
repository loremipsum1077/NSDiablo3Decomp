// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nvnTool/nvnTool_GlslcInterface.h

#pragma once

#include "../nvn/nvn.h"
#include <stddef.h>
#include <stdint.h>

enum GLSLCpiqUniformKind {
    GLSLC_PIQ_UNIFORM_KIND_PLAIN = 0,
    GLSLC_PIQ_UNIFORM_KIND_SAMPLER = 1,
    GLSLC_PIQ_UNIFORM_KIND_IMAGE = 2,
    GLSLC_PIQ_UNIFORM_KIND_TEXTURE = 3,
    GLSLC_PIQ_UNIFORM_KIND_INVALID = 2147483647,
};

enum GLSLCpiqTypeEnum {
    GLSLC_PIQ_TYPE_BOOL = 0,
    GLSLC_PIQ_TYPE_BVEC2 = 1,
    GLSLC_PIQ_TYPE_BVEC3 = 2,
    GLSLC_PIQ_TYPE_BVEC4 = 3,
    GLSLC_PIQ_TYPE_INT = 4,
    GLSLC_PIQ_TYPE_INT_VEC2 = 5,
    GLSLC_PIQ_TYPE_INT_VEC3 = 6,
    GLSLC_PIQ_TYPE_INT_VEC4 = 7,
    GLSLC_PIQ_TYPE_INT8 = 8,
    GLSLC_PIQ_TYPE_INT8_VEC2 = 9,
    GLSLC_PIQ_TYPE_INT8_VEC3 = 10,
    GLSLC_PIQ_TYPE_INT8_VEC4 = 11,
    GLSLC_PIQ_TYPE_INT16 = 12,
    GLSLC_PIQ_TYPE_INT16_VEC2 = 13,
    GLSLC_PIQ_TYPE_INT16_VEC3 = 14,
    GLSLC_PIQ_TYPE_INT16_VEC4 = 15,
    GLSLC_PIQ_TYPE_INT64 = 16,
    GLSLC_PIQ_TYPE_INT64_VEC2 = 17,
    GLSLC_PIQ_TYPE_INT64_VEC3 = 18,
    GLSLC_PIQ_TYPE_INT64_VEC4 = 19,
    GLSLC_PIQ_TYPE_UINT = 20,
    GLSLC_PIQ_TYPE_UINT_VEC2 = 21,
    GLSLC_PIQ_TYPE_UINT_VEC3 = 22,
    GLSLC_PIQ_TYPE_UINT_VEC4 = 23,
    GLSLC_PIQ_TYPE_UINT8 = 24,
    GLSLC_PIQ_TYPE_UINT8_VEC2 = 25,
    GLSLC_PIQ_TYPE_UINT8_VEC3 = 26,
    GLSLC_PIQ_TYPE_UINT8_VEC4 = 27,
    GLSLC_PIQ_TYPE_UINT16 = 28,
    GLSLC_PIQ_TYPE_UINT16_VEC2 = 29,
    GLSLC_PIQ_TYPE_UINT16_VEC3 = 30,
    GLSLC_PIQ_TYPE_UINT16_VEC4 = 31,
    GLSLC_PIQ_TYPE_UINT64 = 32,
    GLSLC_PIQ_TYPE_UINT64_VEC2 = 33,
    GLSLC_PIQ_TYPE_UINT64_VEC3 = 34,
    GLSLC_PIQ_TYPE_UINT64_VEC4 = 35,
    GLSLC_PIQ_TYPE_FLOAT = 36,
    GLSLC_PIQ_TYPE_FLOAT_VEC2 = 37,
    GLSLC_PIQ_TYPE_FLOAT_VEC3 = 38,
    GLSLC_PIQ_TYPE_FLOAT_VEC4 = 39,
    GLSLC_PIQ_TYPE_FLOAT16 = 40,
    GLSLC_PIQ_TYPE_FLOAT16_VEC2 = 41,
    GLSLC_PIQ_TYPE_FLOAT16_VEC3 = 42,
    GLSLC_PIQ_TYPE_FLOAT16_VEC4 = 43,
    GLSLC_PIQ_TYPE_DOUBLE = 44,
    GLSLC_PIQ_TYPE_DOUBLE_VEC2 = 45,
    GLSLC_PIQ_TYPE_DOUBLE_VEC3 = 46,
    GLSLC_PIQ_TYPE_DOUBLE_VEC4 = 47,
    GLSLC_PIQ_TYPE_MAT2 = 48,
    GLSLC_PIQ_TYPE_MAT3 = 49,
    GLSLC_PIQ_TYPE_MAT4 = 50,
    GLSLC_PIQ_TYPE_MAT2X3 = 51,
    GLSLC_PIQ_TYPE_MAT2X4 = 52,
    GLSLC_PIQ_TYPE_MAT3X2 = 53,
    GLSLC_PIQ_TYPE_MAT3X4 = 54,
    GLSLC_PIQ_TYPE_MAT4X2 = 55,
    GLSLC_PIQ_TYPE_MAT4X3 = 56,
    GLSLC_PIQ_TYPE_DMAT2 = 57,
    GLSLC_PIQ_TYPE_DMAT3 = 58,
    GLSLC_PIQ_TYPE_DMAT4 = 59,
    GLSLC_PIQ_TYPE_DMAT2X3 = 60,
    GLSLC_PIQ_TYPE_DMAT2X4 = 61,
    GLSLC_PIQ_TYPE_DMAT3X2 = 62,
    GLSLC_PIQ_TYPE_DMAT3X4 = 63,
    GLSLC_PIQ_TYPE_DMAT4X2 = 64,
    GLSLC_PIQ_TYPE_DMAT4X3 = 65,
    GLSLC_PIQ_TYPE_SAMPLER_1D = 66,
    GLSLC_PIQ_TYPE_SAMPLER_2D = 67,
    GLSLC_PIQ_TYPE_SAMPLER_3D = 68,
    GLSLC_PIQ_TYPE_SAMPLER_CUBE = 69,
    GLSLC_PIQ_TYPE_SAMPLER_1D_SHADOW = 70,
    GLSLC_PIQ_TYPE_SAMPLER_2D_SHADOW = 71,
    GLSLC_PIQ_TYPE_SAMPLER_1D_ARRAY = 72,
    GLSLC_PIQ_TYPE_SAMPLER_2D_ARRAY = 73,
    GLSLC_PIQ_TYPE_SAMPLER_1D_ARRAY_SHADOW = 74,
    GLSLC_PIQ_TYPE_SAMPLER_2D_ARRAY_SHADOW = 75,
    GLSLC_PIQ_TYPE_SAMPLER_2D_MULTISAMPLE = 76,
    GLSLC_PIQ_TYPE_SAMPLER_2D_MULTISAMPLE_ARRAY = 77,
    GLSLC_PIQ_TYPE_SAMPLER_CUBE_SHADOW = 78,
    GLSLC_PIQ_TYPE_SAMPLER_BUFFER = 79,
    GLSLC_PIQ_TYPE_SAMPLER_2D_RECT = 80,
    GLSLC_PIQ_TYPE_SAMPLER_2D_RECT_SHADOW = 81,
    GLSLC_PIQ_TYPE_INT_SAMPLER_1D = 82,
    GLSLC_PIQ_TYPE_INT_SAMPLER_2D = 83,
    GLSLC_PIQ_TYPE_INT_SAMPLER_3D = 84,
    GLSLC_PIQ_TYPE_INT_SAMPLER_CUBE = 85,
    GLSLC_PIQ_TYPE_INT_SAMPLER_1D_ARRAY = 86,
    GLSLC_PIQ_TYPE_INT_SAMPLER_2D_ARRAY = 87,
    GLSLC_PIQ_TYPE_INT_SAMPLER_2D_MULTISAMPLE = 88,
    GLSLC_PIQ_TYPE_INT_SAMPLER_2D_MULTISAMPLE_ARRAY = 89,
    GLSLC_PIQ_TYPE_INT_SAMPLER_BUFFER = 90,
    GLSLC_PIQ_TYPE_INT_SAMPLER_2D_RECT = 91,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_1D = 92,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_2D = 93,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_3D = 94,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_CUBE = 95,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_1D_ARRAY = 96,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_2D_ARRAY = 97,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_2D_MULTISAMPLE = 98,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_2D_MULTISAMPLE_ARRAY = 99,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_BUFFER = 100,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_2D_RECT = 101,
    GLSLC_PIQ_TYPE_IMAGE_1D = 102,
    GLSLC_PIQ_TYPE_IMAGE_2D = 103,
    GLSLC_PIQ_TYPE_IMAGE_3D = 104,
    GLSLC_PIQ_TYPE_IMAGE_2D_RECT = 105,
    GLSLC_PIQ_TYPE_IMAGE_CUBE = 106,
    GLSLC_PIQ_TYPE_IMAGE_BUFFER = 107,
    GLSLC_PIQ_TYPE_IMAGE_1D_ARRAY = 108,
    GLSLC_PIQ_TYPE_IMAGE_2D_ARRAY = 109,
    GLSLC_PIQ_TYPE_IMAGE_CUBE_MAP_ARRAY = 110,
    GLSLC_PIQ_TYPE_IMAGE_2D_MULTISAMPLE = 111,
    GLSLC_PIQ_TYPE_IMAGE_2D_MULTISAMPLE_ARRAY = 112,
    GLSLC_PIQ_TYPE_INT_IMAGE_1D = 113,
    GLSLC_PIQ_TYPE_INT_IMAGE_2D = 114,
    GLSLC_PIQ_TYPE_INT_IMAGE_3D = 115,
    GLSLC_PIQ_TYPE_INT_IMAGE_2D_RECT = 116,
    GLSLC_PIQ_TYPE_INT_IMAGE_CUBE = 117,
    GLSLC_PIQ_TYPE_INT_IMAGE_BUFFER = 118,
    GLSLC_PIQ_TYPE_INT_IMAGE_1D_ARRAY = 119,
    GLSLC_PIQ_TYPE_INT_IMAGE_2D_ARRAY = 120,
    GLSLC_PIQ_TYPE_INT_IMAGE_CUBE_MAP_ARRAY = 121,
    GLSLC_PIQ_TYPE_INT_IMAGE_2D_MULTISAMPLE = 122,
    GLSLC_PIQ_TYPE_INT_IMAGE_2D_MULTISAMPLE_ARRAY = 123,
    GLSLC_PIQ_TYPE_UINT_IMAGE_1D = 124,
    GLSLC_PIQ_TYPE_UINT_IMAGE_2D = 125,
    GLSLC_PIQ_TYPE_UINT_IMAGE_3D = 126,
    GLSLC_PIQ_TYPE_UINT_IMAGE_2D_RECT = 127,
    GLSLC_PIQ_TYPE_UINT_IMAGE_CUBE = 128,
    GLSLC_PIQ_TYPE_UINT_IMAGE_BUFFER = 129,
    GLSLC_PIQ_TYPE_UINT_IMAGE_1D_ARRAY = 130,
    GLSLC_PIQ_TYPE_UINT_IMAGE_2D_ARRAY = 131,
    GLSLC_PIQ_TYPE_UINT_IMAGE_CUBE_MAP_ARRAY = 132,
    GLSLC_PIQ_TYPE_UINT_IMAGE_2D_MULTISAMPLE = 133,
    GLSLC_PIQ_TYPE_UINT_IMAGE_2D_MULTISAMPLE_ARRAY = 134,
    GLSLC_PIQ_TYPE_SAMPLER_CUBE_MAP_ARRAY = 135,
    GLSLC_PIQ_TYPE_INT_SAMPLER_CUBE_MAP_ARRAY = 136,
    GLSLC_PIQ_TYPE_UINT_SAMPLER_CUBE_MAP_ARRAY = 137,
    GLSLC_PIQ_TYPE_SAMPLER_CUBE_MAP_ARRAY_SHADOW = 138,
    GLSLC_PIQ_TYPE_SAMPLER = 139,
    GLSLC_PIQ_TYPE_TEXTURE_1D = 140,
    GLSLC_PIQ_TYPE_TEXTURE_2D = 141,
    GLSLC_PIQ_TYPE_TEXTURE_3D = 142,
    GLSLC_PIQ_TYPE_TEXTURE_CUBE = 143,
    GLSLC_PIQ_TYPE_TEXTURE_1D_SHADOW = 144,
    GLSLC_PIQ_TYPE_TEXTURE_2D_SHADOW = 145,
    GLSLC_PIQ_TYPE_TEXTURE_1D_ARRAY = 146,
    GLSLC_PIQ_TYPE_TEXTURE_2D_ARRAY = 147,
    GLSLC_PIQ_TYPE_TEXTURE_1D_ARRAY_SHADOW = 148,
    GLSLC_PIQ_TYPE_TEXTURE_2D_ARRAY_SHADOW = 149,
    GLSLC_PIQ_TYPE_TEXTURE_2D_MULTISAMPLE = 150,
    GLSLC_PIQ_TYPE_TEXTURE_2D_MULTISAMPLE_ARRAY = 151,
    GLSLC_PIQ_TYPE_TEXTURE_CUBE_SHADOW = 152,
    GLSLC_PIQ_TYPE_TEXTURE_BUFFER = 153,
    GLSLC_PIQ_TYPE_TEXTURE_2D_RECT = 154,
    GLSLC_PIQ_TYPE_TEXTURE_2D_RECT_SHADOW = 155,
    GLSLC_PIQ_TYPE_TEXTURE_CUBE_MAP_ARRAY = 156,
    GLSLC_PIQ_TYPE_TEXTURE_CUBE_MAP_ARRAY_SHADOW = 157,
    GLSLC_PIQ_TYPE_INT_TEXTURE_1D = 158,
    GLSLC_PIQ_TYPE_INT_TEXTURE_2D = 159,
    GLSLC_PIQ_TYPE_INT_TEXTURE_3D = 160,
    GLSLC_PIQ_TYPE_INT_TEXTURE_CUBE = 161,
    GLSLC_PIQ_TYPE_INT_TEXTURE_1D_ARRAY = 162,
    GLSLC_PIQ_TYPE_INT_TEXTURE_2D_ARRAY = 163,
    GLSLC_PIQ_TYPE_INT_TEXTURE_2D_MULTISAMPLE = 164,
    GLSLC_PIQ_TYPE_INT_TEXTURE_2D_MULTISAMPLE_ARRAY = 165,
    GLSLC_PIQ_TYPE_INT_TEXTURE_BUFFER = 166,
    GLSLC_PIQ_TYPE_INT_TEXTURE_2D_RECT = 167,
    GLSLC_PIQ_TYPE_INT_TEXTURE_CUBE_MAP_ARRAY = 168,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_1D = 169,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_2D = 170,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_3D = 171,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_CUBE = 172,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_1D_ARRAY = 173,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_2D_ARRAY = 174,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_2D_MULTISAMPLE = 175,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_2D_MULTISAMPLE_ARRAY = 176,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_BUFFER = 177,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_2D_RECT = 178,
    GLSLC_PIQ_TYPE_UINT_TEXTURE_CUBE_MAP_ARRAY = 179,
    GLSLC_PIQ_INVALID_TYPE = 2147483647,
};

enum GLSLCdebugInfoLevel_enum {
    GLSLC_DEBUG_LEVEL_NONE = 0,
    GLSLC_DEBUG_LEVEL_G0 = 1,
    GLSLC_DEBUG_LEVEL_G1 = 2,
    GLSLC_DEBUG_LEVEL_G2 = 3,
};

typedef GLSLCdebugInfoLevel_enum GLSLCdebugInfoLevelEnum;

enum GLSLClanguageTypeEnum_enum {
    GLSLC_LANGUAGE_GLSL = 0,
    GLSLC_LANGUAGE_GLES = 1,
    GLSLC_LANGUAGE_SPIRV = 2,
};

typedef GLSLClanguageTypeEnum_enum GLSLClanguageTypeEnum;

enum SpillControlEnum_enum {
    DEFAULT_SPILL = 0,
    NO_SPILL = 1,
};

typedef SpillControlEnum_enum SpillControlEnum;

enum GLSLCoptLevel_enum {
    GLSLC_OPTLEVEL_DEFAULT = 0,
    GLSLC_OPTLEVEL_NONE = 1,
};

typedef GLSLCoptLevel_enum GLSLCoptLevelEnum;

enum GLSLCunrollControl_enum {
    GLSLC_LOOP_UNROLL_DEFAULT = 0,
    GLSLC_LOOP_UNROLL_NONE = 1,
    GLSLC_LOOP_UNROLL_ALL = 2,
};

typedef GLSLCunrollControl_enum GLSLCunrollControlEnum;

enum GLSLCwarnUninitControl_enum {
    GLSLC_WARN_UNINIT_DEFAULT = 0,
    GLSLC_WARN_UNINIT_NONE = 1,
    GLSLC_WARN_UNINIT_ALL = 2,
};

typedef GLSLCwarnUninitControl_enum GLSLCwarnUninitControlEnum;

enum GLSLCsectionTypeEnum_enum {
    GLSLC_SECTION_TYPE_GPU_CODE = 0,
    GLSLC_SECTION_TYPE_ASM_DUMP = 1,
    GLSLC_SECTION_TYPE_PERF_STATS = 2,
    GLSLC_SECTION_TYPE_REFLECTION = 3,
    GLSLC_SECTION_TYPE_DEBUG_INFO = 4,
    GLSLC_NUM_SECTION_TYPES = 5,
    GLSLC_SECTION_TYPE_ENUM_LARGE = 2147483647,
};

typedef GLSLCsectionTypeEnum_enum GLSLCsectionTypeEnum;

struct GLSLCpiqName_rec {  // size 0x28
    uint32_t nameOffset;  // 0x00
    uint32_t nameLength;  // 0x04
    uint8_t reserved[32];  // 0x08
};
static_assert(sizeof(GLSLCpiqName_rec) == 0x28, "GLSLCpiqName_rec");

typedef GLSLCpiqName_rec GLSLCpiqName;

struct GLSLCuniformBlockInfo_rec {  // size 0x6c
    GLSLCpiqName nameInfo;  // 0x00
    uint32_t size;  // 0x28
    uint32_t numActiveVariables;  // 0x2c
    NVNshaderStageBits stagesReferencedIn;  // 0x30
    int32_t bindings[6];  // 0x34
    uint8_t reserved[32];  // 0x4c
};
static_assert(sizeof(GLSLCuniformBlockInfo_rec) == 0x6c, "GLSLCuniformBlockInfo_rec");

typedef GLSLCuniformBlockInfo_rec GLSLCuniformBlockInfo;

struct GLSLCuniform_rec {  // size 0xa4
    GLSLCpiqName nameInfo;  // 0x00
    GLSLCpiqTypeEnum type;  // 0x28
    int32_t blockNdx;  // 0x2c
    int32_t blockOffset;  // 0x30
    uint32_t sizeOfArray;  // 0x34
    uint32_t arrayStride;  // 0x38
    int32_t matrixStride;  // 0x3c
    uint32_t isRowMajor;  // 0x40
    NVNshaderStageBits stagesReferencedIn;  // 0x44
    int32_t bindings[6];  // 0x48
    GLSLCpiqUniformKind kind;  // 0x60
    uint8_t isInUBO;  // 0x64
    uint8_t isArray;  // 0x65
    uint8_t reserved[62];  // 0x66
};
static_assert(sizeof(GLSLCuniform_rec) == 0xa4, "GLSLCuniform_rec");

typedef GLSLCuniform_rec GLSLCuniformInfo;

struct GLSLCProgramInputInfo_rec {  // size 0x5c
    GLSLCpiqName nameInfo;  // 0x00
    GLSLCpiqTypeEnum type;  // 0x28
    uint32_t sizeOfArray;  // 0x2c
    int32_t location;  // 0x30
    NVNshaderStageBits stagesReferencedIn;  // 0x34
    uint8_t isArray;  // 0x38
    uint8_t isPerPatch;  // 0x39
    uint8_t reserved[34];  // 0x3a
};
static_assert(sizeof(GLSLCProgramInputInfo_rec) == 0x5c, "GLSLCProgramInputInfo_rec");

typedef GLSLCProgramInputInfo_rec GLSLCprogramInputInfo;

struct GLSLCoptionFlags_rec {  // size 0x8
    uint32_t glslSeparable : 1;  // 0x00
    uint32_t outputAssembly : 1;  // 0x00
    uint32_t outputGpuBinaries : 1;  // 0x00
    uint32_t outputPerfStats : 1;  // 0x00
    uint32_t outputShaderReflection : 1;  // 0x00
    GLSLClanguageTypeEnum language : 4;  // 0x00
    GLSLCdebugInfoLevelEnum outputDebugInfo : 4;  // 0x00
    SpillControlEnum spillControl : 4;  // 0x00
    uint32_t outputThinGpuBinaries : 1;  // 0x00
    uint32_t tessellationAndPassthroughGS : 1;  // 0x00
    uint32_t prioritizeConsecutiveTextureInstructions : 1;  // 0x00
    uint32_t enableFastMathMask : 6;  // 0x00
    GLSLCoptLevelEnum optLevel : 3;  // 0x00
    GLSLCunrollControlEnum unrollControl : 3;  // 0x00
    uint32_t errorOnScratchMemUsage : 1;  // 0x04
    uint32_t enableCBFOptimization : 1;  // 0x04
    uint32_t enableWarpCulling : 1;  // 0x04
    uint32_t enableMultithreadCompilation : 1;  // 0x04
    GLSLCwarnUninitControlEnum warnUninitControl : 3;  // 0x04
};
static_assert(sizeof(GLSLCoptionFlags_rec) == 0x8, "GLSLCoptionFlags_rec");

typedef GLSLCoptionFlags_rec GLSLCoptionFlags;

struct GLSLCsectionHeaderCommon_rec {  // size 0x2c
    uint32_t size;  // 0x00
    uint32_t dataOffset;  // 0x04
    GLSLCsectionTypeEnum type;  // 0x08
    uint8_t reserved[32];  // 0x0c
};
static_assert(sizeof(GLSLCsectionHeaderCommon_rec) == 0x2c, "GLSLCsectionHeaderCommon_rec");

typedef GLSLCsectionHeaderCommon_rec GLSLCsectionHeaderCommon;

struct GLSLCprogramReflectionHeader_rec {  // size 0x8c
    GLSLCsectionHeaderCommon common;  // 0x00
    uint32_t numUniformBlocks;  // 0x2c
    uint32_t uniformBlockOffset;  // 0x30
    uint32_t numUniforms;  // 0x34
    uint32_t uniformOffset;  // 0x38
    uint32_t numProgramInputs;  // 0x3c
    uint32_t programInputsOffset;  // 0x40
    uint32_t numProgramOutputs;  // 0x44
    uint32_t programOutputsOffset;  // 0x48
    uint32_t numSsbo;  // 0x4c
    uint32_t ssboOffset;  // 0x50
    uint32_t numBufferVariables;  // 0x54
    uint32_t bufferVariableOffset;  // 0x58
    uint32_t numXfbVaryings;  // 0x5c
    uint32_t xfbVaryingsOffset;  // 0x60
    uint32_t stringPoolSize;  // 0x64
    uint32_t stringPoolOffset;  // 0x68
    uint32_t shaderInfoOffset;  // 0x6c
    uint32_t numSubroutines;  // 0x70
    uint32_t subroutineOffset;  // 0x74
    uint32_t numSubroutineUniforms;  // 0x78
    uint32_t subroutineUniformOffset;  // 0x7c
    uint32_t subroutineCompatibleIndexPoolSize;  // 0x80
    uint32_t subroutineCompatibleIndexPoolOffset;  // 0x84
    uint8_t reserved[4];  // 0x88
};
static_assert(sizeof(GLSLCprogramReflectionHeader_rec) == 0x8c, "GLSLCprogramReflectionHeader_rec");

typedef GLSLCprogramReflectionHeader_rec GLSLCprogramReflectionHeader;

struct GLSLCgpuCodeHeader_rec {  // size 0x90
    GLSLCsectionHeaderCommon common;  // 0x00
    NVNshaderStage stage;  // 0x2c
    uint32_t controlOffset;  // 0x30
    uint32_t dataOffset;  // 0x34
    uint32_t dataSize;  // 0x38
    uint32_t controlSize;  // 0x3c
    uint32_t scratchMemBytesPerWarp;  // 0x40
    uint32_t scratchMemBytesRecommended;  // 0x44
    uint32_t asmDumpSectionIdx;  // 0x48
    uint32_t perfStatsSectionNdx;  // 0x4c
    uint32_t subroutineLinkageMapOffset;  // 0x50
    uint32_t subroutineLinkageMapSize;  // 0x54
    uint8_t reserved[56];  // 0x58
};
static_assert(sizeof(GLSLCgpuCodeHeader_rec) == 0x90, "GLSLCgpuCodeHeader_rec");

typedef GLSLCgpuCodeHeader_rec GLSLCgpuCodeHeader;

struct GLSLCasmDumpHeader_rec {  // size 0x6c
    GLSLCsectionHeaderCommon common;  // 0x00
    NVNshaderStage stage;  // 0x2c
    uint8_t reserved[60];  // 0x30
};
static_assert(sizeof(GLSLCasmDumpHeader_rec) == 0x6c, "GLSLCasmDumpHeader_rec");

typedef GLSLCasmDumpHeader_rec GLSLCasmDumpHeader;

struct GLSLCgenericHeader_rec {  // size 0x4c
    GLSLCsectionHeaderCommon common;  // 0x00
    uint8_t reserved[32];  // 0x2c
};
static_assert(sizeof(GLSLCgenericHeader_rec) == 0x4c, "GLSLCgenericHeader_rec");

typedef GLSLCgenericHeader_rec GLSLCgenericHeader;

struct GLSLCperfStatsHeader_rec {  // size 0x4c
    GLSLCsectionHeaderCommon common;  // 0x00
    uint8_t reserved[32];  // 0x2c
};
static_assert(sizeof(GLSLCperfStatsHeader_rec) == 0x4c, "GLSLCperfStatsHeader_rec");

typedef GLSLCperfStatsHeader_rec GLSLCperfStatsHeader;

struct GLSLCdebugDataHash_rec {  // size 0x18
    uint32_t buildId[4];  // 0x00
    uint32_t debugHashLo32;  // 0x10
    uint32_t debugHashHi32;  // 0x14
};
static_assert(sizeof(GLSLCdebugDataHash_rec) == 0x18, "GLSLCdebugDataHash_rec");

typedef GLSLCdebugDataHash_rec GLSLCdebugDataHash;

struct GLSLCdebugInfoHeader_rec {  // size 0x6c
    GLSLCsectionHeaderCommon common;  // 0x00
    GLSLCdebugDataHash debugDataHash;  // 0x2c
    uint8_t reserved[40];  // 0x44
};
static_assert(sizeof(GLSLCdebugInfoHeader_rec) == 0x6c, "GLSLCdebugInfoHeader_rec");

typedef GLSLCdebugInfoHeader_rec GLSLCdebugInfoHeader;

union GLSLCsectionHeaderUnion_rec {  // size 0x90
    GLSLCgenericHeader genericHeader;  // 0x00
    GLSLCgpuCodeHeader gpuCodeHeader;  // 0x00
    GLSLCasmDumpHeader asmDumpHeader;  // 0x00
    GLSLCperfStatsHeader perfStatsHeader;  // 0x00
    GLSLCprogramReflectionHeader programReflectionHeader;  // 0x00
    GLSLCdebugInfoHeader debugInfoHeader;  // 0x00
};
static_assert(sizeof(GLSLCsectionHeaderUnion_rec) == 0x90, "GLSLCsectionHeaderUnion_rec");

typedef GLSLCsectionHeaderUnion_rec GLSLCsectionHeaderUnion;

struct GLSLCversion_rec {  // size 0x34
    uint32_t apiMajor;  // 0x00
    uint32_t apiMinor;  // 0x04
    uint32_t gpuCodeVersionMajor;  // 0x08
    uint32_t gpuCodeVersionMinor;  // 0x0c
    uint32_t package;  // 0x10
    uint8_t reserved[32];  // 0x14
};
static_assert(sizeof(GLSLCversion_rec) == 0x34, "GLSLCversion_rec");

typedef GLSLCversion_rec GLSLCversion;

struct GLSLCoutput_rec {  // size 0x120
    uint32_t magic;  // 0x00
    uint32_t reservedBits;  // 0x04
    GLSLCoptionFlags optionFlags;  // 0x08
    GLSLCversion versionInfo;  // 0x10
    uint32_t size;  // 0x44
    uint32_t dataOffset;  // 0x48
    uint32_t numSections;  // 0x4c
    uint8_t reserved[64];  // 0x50
    GLSLCsectionHeaderUnion headers[1];  // 0x90
};
static_assert(sizeof(GLSLCoutput_rec) == 0x120, "GLSLCoutput_rec");

typedef GLSLCoutput_rec GLSLCoutput;

