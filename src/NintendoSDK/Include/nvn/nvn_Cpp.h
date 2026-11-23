// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nvn/nvn_Cpp.h (the DWARF also names it NVN/; one file here)
//
// Trimmed to the classes NX64NVNShaders.cpp and NX64NVNRenderStates.cpp name. Every NVN object is an
// opaque `reserved` block; the methods call through the pfncpp_nvn* function pointers that
// nvn::nvnLoadCPPProcs fills in (nvn_CppFuncPtrImpl.h). Add classes back as files need them.

#pragma once

#include "../nvn/nvn.h"
#include <stddef.h>
#include <stdint.h>

typedef uint8_t NVNboolean;
typedef int NVNdebugDomainId;
typedef void * NVNnativeWindow;
typedef int NVNstorageClass;
typedef void * NVNsubroutineLinkageMapPtr;

// The SDK aligns every opaque object to 8 bytes (its NVN_PRE_ALIGN macro); a char block alone
// would not, and GFXNX64NVN::Globals and NVNState depend on it.
#define NVN_PRE_ALIGN alignas(8)

namespace nvn {

typedef uint64_t BufferAddress;
typedef uint64_t TextureHandle;

class Device;
class MemoryPool;
class MemoryPoolBuilder;
class Buffer;
class BufferBuilder;
class Program;
class VertexAttribState;
struct ShaderData;
class Texture;
class TextureBuilder;
class TextureView;
class WindowBuilder;
class QueueBuilder;
class DeviceBuilder;
class EventBuilder;
class Queue;
class TexturePool;
struct WindowAcquireTextureResult;
struct WindowOriginMode;
struct DepthMode;
struct EventSignalMode;

struct CounterData {  // size 0x10
    uint64_t counter;  // 0x00
    uint64_t timestamp;  // 0x08
};
static_assert(sizeof(CounterData) == 0x10, "CounterData");

struct CopyRegion {  // size 0x18
    int xoffset;  // 0x00
    int yoffset;  // 0x04
    int zoffset;  // 0x08
    int width;  // 0x0c
    int height;  // 0x10
    int depth;  // 0x14
};
static_assert(sizeof(CopyRegion) == 0x18, "CopyRegion");

struct CounterType {  // size 0x4
    enum Enum {
        TIMESTAMP = 0,
        SAMPLES_PASSED = 1,
    };
private:
    nvn::CounterType::Enum m_value;  // 0x00
public:
    CounterType() {}
    CounterType(nvn::CounterType::Enum other) : m_value(other) {}
    CounterType(const nvn::CounterType & other) : m_value(other.m_value) {}
    operator nvn::CounterType::Enum() const { return m_value; }
};
static_assert(sizeof(CounterType) == 0x4, "CounterType");

struct Format {  // size 0x4
    enum Enum {
        NONE = 0,
        R8 = 1,
        R8SN = 2,
        R8UI = 3,
        R8I = 4,
        R16F = 5,
        R16 = 6,
        R16SN = 7,
        R16UI = 8,
        R16I = 9,
        R32F = 10,
        R32UI = 11,
        R32I = 12,
        RG8 = 13,
        RG8SN = 14,
        RG8UI = 15,
        RG8I = 16,
        RG16F = 17,
        RG16 = 18,
        RG16SN = 19,
        RG16UI = 20,
        RG16I = 21,
        RG32F = 22,
        RG32UI = 23,
        RG32I = 24,
        RGB8 = 25,
        RGB8SN = 26,
        RGB8UI = 27,
        RGB8I = 28,
        RGB16F = 29,
        RGB16 = 30,
        RGB16SN = 31,
        RGB16UI = 32,
        RGB16I = 33,
        RGB32F = 34,
        RGB32UI = 35,
        RGB32I = 36,
        RGBA8 = 37,
        RGBA8SN = 38,
        RGBA8UI = 39,
        RGBA8I = 40,
        RGBA16F = 41,
        RGBA16 = 42,
        RGBA16SN = 43,
        RGBA16UI = 44,
        RGBA16I = 45,
        RGBA32F = 46,
        RGBA32UI = 47,
        RGBA32I = 48,
        STENCIL8 = 49,
        DEPTH16 = 50,
        DEPTH24 = 51,
        DEPTH32F = 52,
        DEPTH24_STENCIL8 = 53,
        DEPTH32F_STENCIL8 = 54,
        RGBX8_SRGB = 55,
        RGBA8_SRGB = 56,
        RGBA4 = 57,
        RGB5 = 58,
        RGB5A1 = 59,
        RGB565 = 60,
        RGB10A2 = 61,
        RGB10A2UI = 62,
        R11G11B10F = 63,
        RGB9E5F = 64,
        RGB_DXT1 = 65,
        RGBA_DXT1 = 66,
        RGBA_DXT3 = 67,
        RGBA_DXT5 = 68,
        RGB_DXT1_SRGB = 69,
        RGBA_DXT1_SRGB = 70,
        RGBA_DXT3_SRGB = 71,
        RGBA_DXT5_SRGB = 72,
        RGTC1_UNORM = 73,
        RGTC1_SNORM = 74,
        RGTC2_UNORM = 75,
        RGTC2_SNORM = 76,
        BPTC_UNORM = 77,
        BPTC_UNORM_SRGB = 78,
        BPTC_SFLOAT = 79,
        BPTC_UFLOAT = 80,
        R8_UI2F = 81,
        R8_I2F = 82,
        R16_UI2F = 83,
        R16_I2F = 84,
        R32_UI2F = 85,
        R32_I2F = 86,
        RG8_UI2F = 87,
        RG8_I2F = 88,
        RG16_UI2F = 89,
        RG16_I2F = 90,
        RG32_UI2F = 91,
        RG32_I2F = 92,
        RGB8_UI2F = 93,
        RGB8_I2F = 94,
        RGB16_UI2F = 95,
        RGB16_I2F = 96,
        RGB32_UI2F = 97,
        RGB32_I2F = 98,
        RGBA8_UI2F = 99,
        RGBA8_I2F = 100,
        RGBA16_UI2F = 101,
        RGBA16_I2F = 102,
        RGBA32_UI2F = 103,
        RGBA32_I2F = 104,
        RGB10A2SN = 105,
        RGB10A2I = 106,
        RGB10A2_UI2F = 107,
        RGB10A2_I2F = 108,
        RGBX8 = 109,
        RGBX8SN = 110,
        RGBX8UI = 111,
        RGBX8I = 112,
        RGBX16F = 113,
        RGBX16 = 114,
        RGBX16SN = 115,
        RGBX16UI = 116,
        RGBX16I = 117,
        RGBX32F = 118,
        RGBX32UI = 119,
        RGBX32I = 120,
        RGBA_ASTC_4x4 = 121,
        RGBA_ASTC_5x4 = 122,
        RGBA_ASTC_5x5 = 123,
        RGBA_ASTC_6x5 = 124,
        RGBA_ASTC_6x6 = 125,
        RGBA_ASTC_8x5 = 126,
        RGBA_ASTC_8x6 = 127,
        RGBA_ASTC_8x8 = 128,
        RGBA_ASTC_10x5 = 129,
        RGBA_ASTC_10x6 = 130,
        RGBA_ASTC_10x8 = 131,
        RGBA_ASTC_10x10 = 132,
        RGBA_ASTC_12x10 = 133,
        RGBA_ASTC_12x12 = 134,
        RGBA_ASTC_4x4_SRGB = 135,
        RGBA_ASTC_5x4_SRGB = 136,
        RGBA_ASTC_5x5_SRGB = 137,
        RGBA_ASTC_6x5_SRGB = 138,
        RGBA_ASTC_6x6_SRGB = 139,
        RGBA_ASTC_8x5_SRGB = 140,
        RGBA_ASTC_8x6_SRGB = 141,
        RGBA_ASTC_8x8_SRGB = 142,
        RGBA_ASTC_10x5_SRGB = 143,
        RGBA_ASTC_10x6_SRGB = 144,
        RGBA_ASTC_10x8_SRGB = 145,
        RGBA_ASTC_10x10_SRGB = 146,
        RGBA_ASTC_12x10_SRGB = 147,
        RGBA_ASTC_12x12_SRGB = 148,
        BGR565 = 149,
        BGR5 = 150,
        BGR5A1 = 151,
        A1BGR5 = 152,
        BGRX8 = 153,
        BGRA8 = 154,
        BGRX8_SRGB = 155,
        BGRA8_SRGB = 156,
        NVN_FORMAT_LARGE = 2147483647,
    };
private:
    nvn::Format::Enum m_value;  // 0x00
public:
    Format() {}
    Format(const nvn::Format & other) : m_value(other.m_value) {}
    Format(nvn::Format::Enum e) : m_value(e) {}  // line 1638
    operator nvn::Format::Enum() const { return m_value; }
    bool operator!=(const nvn::Format::Enum & e) const { return m_value != e; }
    bool operator==(const nvn::Format::Enum & e) const { return m_value == e; }
};
static_assert(sizeof(Format) == 0x4, "Format");

struct MemoryPoolFlags {  // size 0x4
    enum Enum {
        CPU_NO_ACCESS = 1,
        CPU_UNCACHED = 2,
        CPU_CACHED = 4,
        GPU_NO_ACCESS = 8,
        GPU_UNCACHED = 16,
        GPU_CACHED = 32,
        SHADER_CODE = 64,
        COMPRESSIBLE = 128,
        PHYSICAL = 256,
        VIRTUAL = 512,
        NVN_MEMORY_POOL_FLAGS_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    MemoryPoolFlags() {}
    MemoryPoolFlags(nvn::MemoryPoolFlags::Enum e) : m_value(e) {}
    MemoryPoolFlags(int other) : m_value(other) {}  // nvn_Cpp.h:3310
    // User-provided in the SDK (the DWARF shows it inlined), so the type is passed by reference.
    MemoryPoolFlags(const nvn::MemoryPoolFlags & other) : m_value(other.m_value) {}
    operator int() const { return m_value; }
    nvn::MemoryPoolFlags operator|(const nvn::MemoryPoolFlags & c) const { return nvn::MemoryPoolFlags(nvn::MemoryPoolFlags::Enum(m_value | c.m_value)); }
    nvn::MemoryPoolFlags operator|(const nvn::MemoryPoolFlags::Enum & c) const { return nvn::MemoryPoolFlags(nvn::MemoryPoolFlags::Enum(m_value | c)); }
};
static_assert(sizeof(MemoryPoolFlags) == 0x4, "MemoryPoolFlags");

struct CopyFlags {  // size 0x4
    enum Enum {
        NONE = 0,
        LINEAR_FILTER = 1,
        ENGINE_2D = 2,
        MIRROR_X = 4,
        MIRROR_Y = 8,
        MIRROR_Z = 16,
        NVN_COPY_FLAGS_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    CopyFlags() {}
    CopyFlags(nvn::CopyFlags::Enum e) : m_value(e) {}
    CopyFlags(const nvn::CopyFlags & other) : m_value(other.m_value) {}
    operator int() const { return m_value; }
};
static_assert(sizeof(CopyFlags) == 0x4, "CopyFlags");

struct DeviceFlagBits {  // size 0x4
    enum Enum {
        DEBUG_ENABLE = 1,
        DEBUG_SKIP_CALLS_ON_ERROR = 2,
        DEBUG_DRAW_VALIDATION = 4,
        DEFERRED_FINALIZE = 8,
        DEBUG_DRAW_VALIDATION_HEAVY = 16,
        DEBUG_ENABLE_LEVEL_0 = 32,
        DEBUG_ENABLE_LEVEL_1 = 64,
        DEBUG_ENABLE_LEVEL_2 = 1,
        DEBUG_ENABLE_LEVEL_3 = 4,
        DEBUG_ENABLE_LEVEL_4 = 16,
        DEFERRED_FIRMWARE_MEMORY_RECLAIM = 128,
        ENABLE_SEPARATE_SAMPLER_TEXTURE_SUPPORT = 256,
        NVN_DEVICE_FLAG_BITS_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    DeviceFlagBits() {}
    DeviceFlagBits(nvn::DeviceFlagBits::Enum e) : m_value(e) {}
    DeviceFlagBits(int other) : m_value(other) {}
    DeviceFlagBits(const nvn::DeviceFlagBits & other) : m_value(other.m_value) {}
    operator int() const { return m_value; }
};
static_assert(sizeof(DeviceFlagBits) == 0x4, "DeviceFlagBits");

class DeviceBuilder {  // size 0x40
protected:
    char reserved[64];  // 0x00
public:
    void SetDefaults();
    void SetFlags(nvn::DeviceFlagBits flags);
};
static_assert(sizeof(DeviceBuilder) == 0x40, "DeviceBuilder");

class EventBuilder {  // size 0x20
protected:
    char reserved[32];  // 0x00
public:
    nvn::EventBuilder & SetDefaults();
    nvn::EventBuilder & SetStorage(const nvn::MemoryPool * pool, int64_t offset);
};
static_assert(sizeof(EventBuilder) == 0x20, "EventBuilder");

// The NVN enum wrappers below have a user-provided copy constructor in the SDK (the DWARF
// shows it inlined), like MemoryPoolFlags.
struct DepthFunc {  // size 0x4
    enum Enum {
        NEVER = 1,
        LESS = 2,
        EQUAL = 3,
        LEQUAL = 4,
        GREATER = 5,
        NOTEQUAL = 6,
        GEQUAL = 7,
        ALWAYS = 8,
        NVN_DEPTH_FUNC_LARGE = 2147483647,
    };
private:
    nvn::DepthFunc::Enum m_value;  // 0x00
public:
    DepthFunc() {}
    DepthFunc(nvn::DepthFunc::Enum other) : m_value(other) {}
    DepthFunc(const nvn::DepthFunc & other) : m_value(other.m_value) {}
    operator nvn::DepthFunc::Enum() const { return m_value; }
};
static_assert(sizeof(DepthFunc) == 0x4, "DepthFunc");

struct Face {  // size 0x4
    enum Enum {
        NONE = 0,
        FRONT = 1,
        BACK = 2,
        FRONT_AND_BACK = 3,
        NVN_FACE_LARGE = 2147483647,
    };
private:
    nvn::Face::Enum m_value;  // 0x00
public:
    Face() {}
    Face(nvn::Face::Enum other) : m_value(other) {}
    Face(const nvn::Face & other) : m_value(other.m_value) {}
    operator nvn::Face::Enum() const { return m_value; }
};
static_assert(sizeof(Face) == 0x4, "Face");

struct StencilFunc {  // size 0x4
    enum Enum {
        NEVER = 1,
        LESS = 2,
        EQUAL = 3,
        LEQUAL = 4,
        GREATER = 5,
        NOTEQUAL = 6,
        GEQUAL = 7,
        ALWAYS = 8,
        NVN_STENCIL_FUNC_LARGE = 2147483647,
    };
private:
    nvn::StencilFunc::Enum m_value;  // 0x00
public:
    StencilFunc() {}
    StencilFunc(nvn::StencilFunc::Enum other) : m_value(other) {}
    StencilFunc(const nvn::StencilFunc & other) : m_value(other.m_value) {}
    operator nvn::StencilFunc::Enum() const { return m_value; }
};
static_assert(sizeof(StencilFunc) == 0x4, "StencilFunc");

struct StencilOp {  // size 0x4
    enum Enum {
        KEEP = 1,
        ZERO = 2,
        REPLACE = 3,
        INCR = 4,
        DECR = 5,
        INVERT = 6,
        INCR_WRAP = 7,
        DECR_WRAP = 8,
        NVN_STENCIL_OP_LARGE = 2147483647,
    };
private:
    nvn::StencilOp::Enum m_value;  // 0x00
public:
    StencilOp() {}
    StencilOp(nvn::StencilOp::Enum other) : m_value(other) {}
    StencilOp(const nvn::StencilOp & other) : m_value(other.m_value) {}
    operator nvn::StencilOp::Enum() const { return m_value; }
};
static_assert(sizeof(StencilOp) == 0x4, "StencilOp");

struct ShaderData {  // size 0x10
    nvn::BufferAddress data;  // 0x00
    const void * control;  // 0x08
};
static_assert(sizeof(ShaderData) == 0x10, "ShaderData");

typedef NVNboolean (*ProgramInitializeFunc)(nvn::Program *, nvn::Device *);
typedef void (*ProgramFinalizeFunc)(nvn::Program *);
typedef NVNboolean (*ProgramSetShadersFunc)(nvn::Program *, int, const nvn::ShaderData *);
typedef void (*MemoryPoolBuilderSetDeviceFunc)(nvn::MemoryPoolBuilder *, nvn::Device *);
typedef void (*MemoryPoolBuilderSetDefaultsFunc)(nvn::MemoryPoolBuilder *);
typedef void (*MemoryPoolBuilderSetStorageFunc)(nvn::MemoryPoolBuilder *, void *, size_t);
typedef void (*MemoryPoolBuilderSetFlagsFunc)(nvn::MemoryPoolBuilder *, int);
typedef NVNboolean (*MemoryPoolInitializeFunc)(nvn::MemoryPool *, const nvn::MemoryPoolBuilder *);
typedef void (*MemoryPoolFinalizeFunc)(nvn::MemoryPool *);
typedef void (*BufferBuilderSetDeviceFunc)(nvn::BufferBuilder *, nvn::Device *);
typedef void (*BufferBuilderSetDefaultsFunc)(nvn::BufferBuilder *);
typedef void (*BufferBuilderSetStorageFunc)(nvn::BufferBuilder *, nvn::MemoryPool *, ptrdiff_t, size_t);
typedef NVNboolean (*BufferInitializeFunc)(nvn::Buffer *, const nvn::BufferBuilder *);
typedef void (*BufferFinalizeFunc)(nvn::Buffer *);
typedef nvn::BufferAddress (*BufferGetAddressFunc)(const nvn::Buffer *);
typedef ptrdiff_t (*BufferGetMemoryOffsetFunc)(const nvn::Buffer *);
typedef void (*VertexAttribStateSetDefaultsFunc)(nvn::VertexAttribState *);
// Takes the format as its int value: passing nvn::Format by value reorders the argument loads
// in GFXNX64NVN::sGetVertexAttributeBindings.
typedef void (*VertexAttribStateSetFormatFunc)(nvn::VertexAttribState *, int, ptrdiff_t);
typedef void (*VertexAttribStateSetStreamIndexFunc)(nvn::VertexAttribState *, int);
class DepthStencilState;
typedef void (*DepthStencilStateSetDepthTestEnableFunc)(nvn::DepthStencilState *, NVNboolean);
typedef void (*DepthStencilStateSetDepthWriteEnableFunc)(nvn::DepthStencilState *, NVNboolean);
typedef void (*DepthStencilStateSetDepthFuncFunc)(nvn::DepthStencilState *, nvn::DepthFunc::Enum);
typedef void (*DepthStencilStateSetStencilTestEnableFunc)(nvn::DepthStencilState *, NVNboolean);
typedef void (*DepthStencilStateSetStencilFuncFunc)(nvn::DepthStencilState *, nvn::Face::Enum, nvn::StencilFunc::Enum);
typedef void (*DepthStencilStateSetStencilOpFunc)(nvn::DepthStencilState *, nvn::Face::Enum, nvn::StencilOp::Enum, nvn::StencilOp::Enum, nvn::StencilOp::Enum);

class CommandBuffer;
typedef void (*QueueFinishFunc)(nvn::Queue *);
typedef ptrdiff_t (*TextureGetStrideFunc)(const nvn::Texture *);
typedef void (*TextureFlushTexelsFunc)(const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *);
typedef void (*CommandBufferReportCounterFunc)(nvn::CommandBuffer *, nvn::CounterType::Enum, nvn::BufferAddress);
typedef void (*CommandBufferResetCounterFunc)(nvn::CommandBuffer *, nvn::CounterType::Enum);

typedef nvn::BufferAddress (*MemoryPoolGetBufferAddressFunc)(const nvn::MemoryPool *);
typedef void (*TextureViewSetDefaultsFunc)(nvn::TextureView *);
class Window;
typedef void (*WindowSetCropFunc)(nvn::Window *, int, int, int, int);
typedef void (*QueuePresentTextureFunc)(nvn::Queue *, nvn::Window *, int);
extern nvn::WindowSetCropFunc pfncpp_nvnWindowSetCrop;
extern nvn::QueuePresentTextureFunc pfncpp_nvnQueuePresentTexture;
class Event;
typedef uint64_t CommandHandle;
typedef void (*CommandBufferSignalEventFunc)(nvn::CommandBuffer *, const nvn::Event *, int, int, int, uint32_t);
typedef nvn::CommandHandle (*CommandBufferEndRecordingFunc)(nvn::CommandBuffer *);
typedef void (*QueueSubmitCommandsFunc)(nvn::Queue *, int, const nvn::CommandHandle *);
extern nvn::CommandBufferSignalEventFunc pfncpp_nvnCommandBufferSignalEvent;
extern nvn::CommandBufferEndRecordingFunc pfncpp_nvnCommandBufferEndRecording;
extern nvn::QueueSubmitCommandsFunc pfncpp_nvnQueueSubmitCommands;
class Sync;
typedef void (*CommandBufferWaitSyncFunc)(nvn::CommandBuffer *, const nvn::Sync *);
extern nvn::CommandBufferWaitSyncFunc pfncpp_nvnCommandBufferWaitSync;
class Sampler;
class SamplerPool;
typedef void (*SamplerPoolRegisterSamplerFunc)(const nvn::SamplerPool *, int, const nvn::Sampler *);
extern nvn::SamplerPoolRegisterSamplerFunc pfncpp_nvnSamplerPoolRegisterSampler;
typedef void (*CommandBufferClearColorFunc)(nvn::CommandBuffer *, int, const float *, int);
typedef void (*CommandBufferClearDepthStencilFunc)(nvn::CommandBuffer *, float, NVNboolean, int, int);
typedef void (*CommandBufferSetStencilCullCriteriaFunc)(nvn::CommandBuffer *, nvn::StencilFunc::Enum, int, int);
extern nvn::CommandBufferClearColorFunc pfncpp_nvnCommandBufferClearColor;
extern nvn::CommandBufferClearDepthStencilFunc pfncpp_nvnCommandBufferClearDepthStencil;
extern nvn::CommandBufferSetStencilCullCriteriaFunc pfncpp_nvnCommandBufferSetStencilCullCriteria;
typedef void (*TextureFinalizeFunc)(nvn::Texture *);
extern nvn::TextureFinalizeFunc pfncpp_nvnTextureFinalize;
typedef void (*TextureViewSetLayersFunc)(nvn::TextureView *, int, int);
typedef void (*TextureViewSetTargetFunc)(nvn::TextureView *, int);
typedef ptrdiff_t (*TextureGetViewOffsetFunc)(const nvn::Texture *, const nvn::TextureView *);
extern nvn::TextureViewSetDefaultsFunc pfncpp_nvnTextureViewSetDefaults;
extern nvn::TextureViewSetLayersFunc pfncpp_nvnTextureViewSetLayers;
extern nvn::TextureViewSetTargetFunc pfncpp_nvnTextureViewSetTarget;
extern nvn::TextureGetViewOffsetFunc pfncpp_nvnTextureGetViewOffset;
// nvn_CppFuncPtrImpl.h:88, 238, 245, 471, 472
extern nvn::MemoryPoolGetBufferAddressFunc pfncpp_nvnMemoryPoolGetBufferAddress;
extern nvn::QueueFinishFunc pfncpp_nvnQueueFinish;
extern nvn::TextureGetStrideFunc pfncpp_nvnTextureGetStride;
extern nvn::TextureFlushTexelsFunc pfncpp_nvnTextureFlushTexels;
extern nvn::CommandBufferReportCounterFunc pfncpp_nvnCommandBufferReportCounter;
extern nvn::CommandBufferResetCounterFunc pfncpp_nvnCommandBufferResetCounter;

// nvn_CppFuncPtrImpl.h:110-364
extern nvn::ProgramInitializeFunc pfncpp_nvnProgramInitialize;
extern nvn::ProgramFinalizeFunc pfncpp_nvnProgramFinalize;
extern nvn::ProgramSetShadersFunc pfncpp_nvnProgramSetShaders;
extern nvn::MemoryPoolBuilderSetDeviceFunc pfncpp_nvnMemoryPoolBuilderSetDevice;
extern nvn::MemoryPoolBuilderSetDefaultsFunc pfncpp_nvnMemoryPoolBuilderSetDefaults;
extern nvn::MemoryPoolBuilderSetStorageFunc pfncpp_nvnMemoryPoolBuilderSetStorage;
extern nvn::MemoryPoolBuilderSetFlagsFunc pfncpp_nvnMemoryPoolBuilderSetFlags;
extern nvn::MemoryPoolInitializeFunc pfncpp_nvnMemoryPoolInitialize;
extern nvn::MemoryPoolFinalizeFunc pfncpp_nvnMemoryPoolFinalize;
extern nvn::BufferBuilderSetDeviceFunc pfncpp_nvnBufferBuilderSetDevice;
extern nvn::BufferBuilderSetDefaultsFunc pfncpp_nvnBufferBuilderSetDefaults;
extern nvn::BufferBuilderSetStorageFunc pfncpp_nvnBufferBuilderSetStorage;
extern nvn::BufferInitializeFunc pfncpp_nvnBufferInitialize;
extern nvn::BufferFinalizeFunc pfncpp_nvnBufferFinalize;
extern nvn::BufferGetAddressFunc pfncpp_nvnBufferGetAddress;
extern nvn::BufferGetMemoryOffsetFunc pfncpp_nvnBufferGetMemoryOffset;
extern nvn::VertexAttribStateSetDefaultsFunc pfncpp_nvnVertexAttribStateSetDefaults;
extern nvn::VertexAttribStateSetFormatFunc pfncpp_nvnVertexAttribStateSetFormat;
extern nvn::VertexAttribStateSetStreamIndexFunc pfncpp_nvnVertexAttribStateSetStreamIndex;
extern nvn::DepthStencilStateSetDepthTestEnableFunc pfncpp_nvnDepthStencilStateSetDepthTestEnable;
extern nvn::DepthStencilStateSetDepthWriteEnableFunc pfncpp_nvnDepthStencilStateSetDepthWriteEnable;
extern nvn::DepthStencilStateSetDepthFuncFunc pfncpp_nvnDepthStencilStateSetDepthFunc;
extern nvn::DepthStencilStateSetStencilTestEnableFunc pfncpp_nvnDepthStencilStateSetStencilTestEnable;
extern nvn::DepthStencilStateSetStencilFuncFunc pfncpp_nvnDepthStencilStateSetStencilFunc;
extern nvn::DepthStencilStateSetStencilOpFunc pfncpp_nvnDepthStencilStateSetStencilOp;

class NVN_PRE_ALIGN Device {  // size 0x3000
protected:
    char reserved[12288];  // 0x00
    Device(const nvn::Device &);
    const nvn::Device & operator=(const nvn::Device &);
public:
    Device() {}
    nvn::TextureHandle GetTextureHandle(int textureID, int samplerID) const;
    uint64_t GetTimestampInNanoseconds(const nvn::CounterData * counterData) const;
    NVNboolean Initialize(const nvn::DeviceBuilder * builder);
    void SetWindowOriginMode(nvn::WindowOriginMode windowOriginMode);
    void SetDepthMode(nvn::DepthMode depthMode);
};
static_assert(sizeof(Device) == 0x3000, "Device");

class VertexAttribState {  // size 0x4
protected:
    char reserved[4];  // 0x00
public:
    nvn::VertexAttribState & SetDefaults();
    nvn::VertexAttribState & SetFormat(nvn::Format format, ptrdiff_t relativeOffset);
    nvn::VertexAttribState & SetStreamIndex(int streamIndex);
};
static_assert(sizeof(VertexAttribState) == 0x4, "VertexAttribState");

class NVN_PRE_ALIGN VertexStreamState {  // size 0x8
protected:
    char reserved[8];  // 0x00
public:
    nvn::VertexStreamState & SetDefaults();
    nvn::VertexStreamState & SetStride(ptrdiff_t stride);
};
static_assert(sizeof(VertexStreamState) == 0x8, "VertexStreamState");

class NVN_PRE_ALIGN Program {  // size 0xc0
protected:
    char reserved[192];  // 0x00
    Program(const nvn::Program &);
    const nvn::Program & operator=(const nvn::Program &);
public:
    Program() {}
    NVNboolean Initialize(nvn::Device * device);
    void Finalize();
    NVNboolean SetShaders(int count, const nvn::ShaderData * stageData);
};
static_assert(sizeof(Program) == 0xc0, "Program");

class NVN_PRE_ALIGN MemoryPoolBuilder {  // size 0x40
protected:
    char reserved[64];  // 0x00
public:
    nvn::MemoryPoolBuilder & SetDevice(nvn::Device * device);
    nvn::MemoryPoolBuilder & SetDefaults();
    nvn::MemoryPoolBuilder & SetStorage(void * memory, size_t size);
    nvn::MemoryPoolBuilder & SetFlags(nvn::MemoryPoolFlags flags);
};
static_assert(sizeof(MemoryPoolBuilder) == 0x40, "MemoryPoolBuilder");

class NVN_PRE_ALIGN MemoryPool {  // size 0x100
protected:
    char reserved[256];  // 0x00
    MemoryPool(const nvn::MemoryPool &);
    const nvn::MemoryPool & operator=(const nvn::MemoryPool &);
public:
    MemoryPool() {}
    NVNboolean Initialize(const nvn::MemoryPoolBuilder * builder);
    void Finalize();
    nvn::BufferAddress GetBufferAddress() const;
    void FlushMappedRange(ptrdiff_t offset, size_t size) const;
};
static_assert(sizeof(MemoryPool) == 0x100, "MemoryPool");

class NVN_PRE_ALIGN BufferBuilder {  // size 0x40
protected:
    char reserved[64];  // 0x00
public:
    nvn::BufferBuilder & SetDevice(nvn::Device * device);
    nvn::BufferBuilder & SetDefaults();
    nvn::BufferBuilder & SetStorage(nvn::MemoryPool * pool, ptrdiff_t offset, size_t size);
};
static_assert(sizeof(BufferBuilder) == 0x40, "BufferBuilder");

class NVN_PRE_ALIGN Buffer {  // size 0x30
protected:
    char reserved[48];  // 0x00
    Buffer(const nvn::Buffer &);
    const nvn::Buffer & operator=(const nvn::Buffer &);
public:
    Buffer() {}
    NVNboolean Initialize(const nvn::BufferBuilder * builder);
    void Finalize();
    nvn::BufferAddress GetAddress() const;
    ptrdiff_t GetMemoryOffset() const;
};
static_assert(sizeof(Buffer) == 0x30, "Buffer");

class NVN_PRE_ALIGN Sync {  // size 0x40
protected:
    char reserved[64];  // 0x00
    Sync(const nvn::Sync &);
    const nvn::Sync & operator=(const nvn::Sync &);
public:
    Sync() {}
    NVNboolean Initialize(nvn::Device * device);
};
static_assert(sizeof(Sync) == 0x40, "Sync");

class NVN_PRE_ALIGN Window {  // size 0x180
protected:
    char reserved[384];  // 0x00
    Window(const nvn::Window &);
    const nvn::Window & operator=(const nvn::Window &);
public:
    Window() {}
    void SetCrop(int x, int y, int w, int h);
    NVNboolean Initialize(const nvn::WindowBuilder * builder);
    nvn::WindowAcquireTextureResult AcquireTexture(nvn::Sync * textureAvailableSync, int * textureIndex);
};
static_assert(sizeof(Window) == 0x180, "Window");

class NVN_PRE_ALIGN Event {  // size 0x40
protected:
    char reserved[64];  // 0x00
    Event(const nvn::Event &);
    const nvn::Event & operator=(const nvn::Event &);
public:
    Event() {}
    uint32_t GetValue() const;
    void Finalize();
    NVNboolean Initialize(const nvn::EventBuilder * builder);
    void Signal(nvn::EventSignalMode mode, uint32_t value);
};
static_assert(sizeof(Event) == 0x40, "Event");

// nvn_Cpp.h: EventSignalMode / EventSignalLocation / EventSignalFlags (DWARF enumerators)
struct EventSignalMode {  // size 0x4
    enum Enum {
        WRITE = 0,
        ADD = 1,
        NVN_EVENT_SIGNAL_MODE_LARGE = 2147483647,
    };
private:
    nvn::EventSignalMode::Enum m_value;  // 0x00
public:
    EventSignalMode() {}
    EventSignalMode(nvn::EventSignalMode::Enum e) : m_value(e) {}
    operator nvn::EventSignalMode::Enum() const { return m_value; }
};

struct EventSignalLocation {  // size 0x4
    enum Enum {
        TOP = 0,
        VERTEX_PIPE = 1,
        BOTTOM = 2,
        NVN_EVENT_SIGNAL_LOCATION_LARGE = 2147483647,
    };
private:
    nvn::EventSignalLocation::Enum m_value;  // 0x00
public:
    EventSignalLocation() {}
    EventSignalLocation(nvn::EventSignalLocation::Enum e) : m_value(e) {}
    operator nvn::EventSignalLocation::Enum() const { return m_value; }
};

struct EventSignalFlags {  // size 0x4
    enum Enum {
        NO_WAIT_MEMORY = 1,
        NVN_EVENT_SIGNAL_FLAGS_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    EventSignalFlags() {}
    EventSignalFlags(nvn::EventSignalFlags::Enum e) : m_value(e) {}
    operator int() const { return m_value; }
};

// nvn_Cpp.h: ClearColorMask (DWARF)
struct ClearColorMask {  // size 0x4
    enum Enum {
        R = 1,
        G = 2,
        B = 4,
        A = 8,
        RGBA = 15,
        NVN_CLEAR_COLOR_MASK_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    ClearColorMask() {}
    ClearColorMask(nvn::ClearColorMask::Enum e) : m_value(e) {}
    operator int() const { return m_value; }
};

class VertexStreamState;
// Added for NX64NVNEngine.cpp (sApplyState, sCreateGlobalStateObjects); layouts and names from the DWARF.
struct BlendFunc {  // size 0x4
    enum Enum {
        ZERO = 1,
        ONE = 2,
        SRC_COLOR = 3,
        ONE_MINUS_SRC_COLOR = 4,
        SRC_ALPHA = 5,
        ONE_MINUS_SRC_ALPHA = 6,
        DST_ALPHA = 7,
        ONE_MINUS_DST_ALPHA = 8,
        DST_COLOR = 9,
        ONE_MINUS_DST_COLOR = 10,
        SRC_ALPHA_SATURATE = 11,
        SRC1_COLOR = 16,
        ONE_MINUS_SRC1_COLOR = 17,
        SRC1_ALPHA = 18,
        ONE_MINUS_SRC1_ALPHA = 19,
        CONSTANT_COLOR = 97,
        ONE_MINUS_CONSTANT_COLOR = 98,
        CONSTANT_ALPHA = 99,
        ONE_MINUS_CONSTANT_ALPHA = 100,
        NVN_BLEND_FUNC_LARGE = 2147483647,
    };
private:
    nvn::BlendFunc::Enum m_value;  // 0x00
public:
    BlendFunc() {}
    BlendFunc(nvn::BlendFunc::Enum other) : m_value(other) {}
    BlendFunc(const nvn::BlendFunc & other) : m_value(other.m_value) {}
    operator nvn::BlendFunc::Enum() const { return m_value; }
};
static_assert(sizeof(BlendFunc) == 0x4, "BlendFunc");

struct BlendEquation {  // size 0x4
    enum Enum {
        ADD = 1,
        SUB = 2,
        REVERSE_SUB = 3,
        MIN = 4,
        MAX = 5,
        NVN_BLEND_EQUATION_LARGE = 2147483647,
    };
private:
    nvn::BlendEquation::Enum m_value;  // 0x00
public:
    BlendEquation() {}
    BlendEquation(nvn::BlendEquation::Enum other) : m_value(other) {}
    BlendEquation(const nvn::BlendEquation & other) : m_value(other.m_value) {}
    operator nvn::BlendEquation::Enum() const { return m_value; }
};
static_assert(sizeof(BlendEquation) == 0x4, "BlendEquation");

struct MagFilter {  // size 0x4
    enum Enum {
        NEAREST = 0,
        LINEAR = 1,
        NVN_MAG_FILTER_LARGE = 2147483647,
    };
private:
    nvn::MagFilter::Enum m_value;  // 0x00
public:
    MagFilter() {}
    MagFilter(nvn::MagFilter::Enum other) : m_value(other) {}
    MagFilter(const nvn::MagFilter & other) : m_value(other.m_value) {}
    operator nvn::MagFilter::Enum() const { return m_value; }
};
static_assert(sizeof(MagFilter) == 0x4, "MagFilter");

struct MinFilter {  // size 0x4
    enum Enum {
        NEAREST = 0,
        LINEAR = 1,
        NEAREST_MIPMAP_NEAREST = 2,
        LINEAR_MIPMAP_NEAREST = 3,
        NEAREST_MIPMAP_LINEAR = 4,
        LINEAR_MIPMAP_LINEAR = 5,
        NVN_MIN_FILTER_LARGE = 2147483647,
    };
private:
    nvn::MinFilter::Enum m_value;  // 0x00
public:
    MinFilter() {}
    MinFilter(nvn::MinFilter::Enum other) : m_value(other) {}
    MinFilter(const nvn::MinFilter & other) : m_value(other.m_value) {}
    operator nvn::MinFilter::Enum() const { return m_value; }
};
static_assert(sizeof(MinFilter) == 0x4, "MinFilter");

struct WrapMode {  // size 0x4
    enum Enum {
        CLAMP = 0,
        REPEAT = 1,
        MIRROR_CLAMP = 2,
        MIRROR_CLAMP_TO_EDGE = 3,
        MIRROR_CLAMP_TO_BORDER = 4,
        CLAMP_TO_BORDER = 5,
        MIRRORED_REPEAT = 6,
        CLAMP_TO_EDGE = 7,
        NVN_WRAP_MODE_LARGE = 2147483647,
    };
private:
    nvn::WrapMode::Enum m_value;  // 0x00
public:
    WrapMode() {}
    WrapMode(nvn::WrapMode::Enum other) : m_value(other) {}
    WrapMode(const nvn::WrapMode & other) : m_value(other.m_value) {}
    operator nvn::WrapMode::Enum() const { return m_value; }
};
static_assert(sizeof(WrapMode) == 0x4, "WrapMode");

struct FrontFace {  // size 0x4
    enum Enum {
        CW = 0,
        CCW = 1,
        NVN_FRONT_FACE_LARGE = 2147483647,
    };
private:
    nvn::FrontFace::Enum m_value;  // 0x00
public:
    FrontFace() {}
    FrontFace(nvn::FrontFace::Enum other) : m_value(other) {}
    FrontFace(const nvn::FrontFace & other) : m_value(other.m_value) {}
    operator nvn::FrontFace::Enum() const { return m_value; }
};
static_assert(sizeof(FrontFace) == 0x4, "FrontFace");

struct PolygonMode {  // size 0x4
    enum Enum {
        POINT = 0,
        LINE = 1,
        FILL = 2,
        NVN_POLYGON_MODE_LARGE = 2147483647,
    };
private:
    nvn::PolygonMode::Enum m_value;  // 0x00
public:
    PolygonMode() {}
    PolygonMode(nvn::PolygonMode::Enum other) : m_value(other) {}
    PolygonMode(const nvn::PolygonMode & other) : m_value(other.m_value) {}
    operator nvn::PolygonMode::Enum() const { return m_value; }
};
static_assert(sizeof(PolygonMode) == 0x4, "PolygonMode");

struct ShaderStage {  // size 0x4
    enum Enum {
        VERTEX = 0,
        FRAGMENT = 1,
        GEOMETRY = 2,
        TESS_CONTROL = 3,
        TESS_EVALUATION = 4,
        COMPUTE = 5,
        NVN_SHADER_STAGE_LARGE = 2147483647,
    };
private:
    nvn::ShaderStage::Enum m_value;  // 0x00
public:
    ShaderStage() {}
    ShaderStage(nvn::ShaderStage::Enum other) : m_value(other) {}
    ShaderStage(const nvn::ShaderStage & other) : m_value(other.m_value) {}
    operator nvn::ShaderStage::Enum() const { return m_value; }
};
static_assert(sizeof(ShaderStage) == 0x4, "ShaderStage");

struct ShaderStageBits {  // size 0x4
    enum Enum {
        VERTEX = 1,
        FRAGMENT = 2,
        GEOMETRY = 4,
        TESS_CONTROL = 8,
        TESS_EVALUATION = 16,
        COMPUTE = 32,
        ALL_GRAPHICS_BITS = 31,
        NVN_SHADER_STAGE_BITS_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    ShaderStageBits() {}
    ShaderStageBits(nvn::ShaderStageBits::Enum other) : m_value(other) {}
    operator int() const { return m_value; }
    nvn::ShaderStageBits operator|(const nvn::ShaderStageBits::Enum & c) const { return nvn::ShaderStageBits(nvn::ShaderStageBits::Enum(m_value | c)); }
};
static_assert(sizeof(ShaderStageBits) == 0x4, "ShaderStageBits");

struct BarrierBits {  // size 0x4
    enum Enum {
        ORDER_PRIMITIVES = 1,
        ORDER_FRAGMENTS = 2,
        ORDER_FRAGMENTS_TILED = 4,
        ORDER_INDIRECT_DATA = 8,
        INVALIDATE_TEXTURE = 16,
        INVALIDATE_SHADER = 32,
        INVALIDATE_TEXTURE_DESCRIPTOR = 64,
        INVALIDATE_ZCULL = 128,
        NVN_BARRIER_BITS_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    BarrierBits() {}
    BarrierBits(nvn::BarrierBits::Enum other) : m_value(other) {}
    operator int() const { return m_value; }
    nvn::BarrierBits operator|(const nvn::BarrierBits::Enum & c) const { return nvn::BarrierBits(nvn::BarrierBits::Enum(m_value | c)); }
};
static_assert(sizeof(BarrierBits) == 0x4, "BarrierBits");

// Alignments are the DWARF's DW_AT_alignment for each class (ColorState, ChannelMaskState, PolygonState: 4;
// BlendState, SamplerBuilder: 8); the members are char arrays, so the SDK set them explicitly.
struct BufferRange {  // size 0x10
    nvn::BufferAddress address;  // 0x00
    uint64_t size;  // 0x08
};
static_assert(sizeof(BufferRange) == 0x10, "BufferRange");

class NVN_PRE_ALIGN BlendState {  // size 0x8
protected:
    char reserved[8];  // 0x00
public:
    nvn::BlendState & SetDefaults();
    nvn::BlendState & SetBlendFunc(nvn::BlendFunc srcFunc, nvn::BlendFunc dstFunc, nvn::BlendFunc srcFuncAlpha, nvn::BlendFunc dstFuncAlpha);
    nvn::BlendState & SetBlendEquation(nvn::BlendEquation modeRGB, nvn::BlendEquation modeAlpha);
};
static_assert(sizeof(BlendState) == 0x8, "BlendState");

class alignas(4) ChannelMaskState {  // size 0x4
protected:
    char reserved[4];  // 0x00
public:
    nvn::ChannelMaskState & SetDefaults();
    nvn::ChannelMaskState & SetChannelMask(int index, NVNboolean r, NVNboolean g, NVNboolean b, NVNboolean a);
};
static_assert(sizeof(ChannelMaskState) == 0x4, "ChannelMaskState");

class alignas(4) ColorState {  // size 0x4
protected:
    char reserved[4];  // 0x00
public:
    nvn::ColorState & SetDefaults();
    nvn::ColorState & SetBlendEnable(int index, NVNboolean enable);
};
static_assert(sizeof(ColorState) == 0x4, "ColorState");

class alignas(4) PolygonState {  // size 0x4
protected:
    char reserved[4];  // 0x00
public:
    nvn::PolygonState & SetDefaults();
    nvn::PolygonState & SetCullFace(nvn::Face face);
    nvn::PolygonState & SetFrontFace(nvn::FrontFace face);
    nvn::PolygonState & SetPolygonMode(nvn::PolygonMode polygonMode);
};
static_assert(sizeof(PolygonState) == 0x4, "PolygonState");

class NVN_PRE_ALIGN SamplerBuilder {  // size 0x60
protected:
    char reserved[96];  // 0x00
public:
    nvn::SamplerBuilder & SetDevice(nvn::Device * device);
    nvn::SamplerBuilder & SetDefaults();
    nvn::SamplerBuilder & SetMinMagFilter(nvn::MinFilter min, nvn::MagFilter mag);
    nvn::SamplerBuilder & SetWrapMode(nvn::WrapMode s, nvn::WrapMode t, nvn::WrapMode r);
    nvn::SamplerBuilder & SetLodBias(float bias);
    nvn::SamplerBuilder & SetLodSnap(float lodSnap);
};
static_assert(sizeof(SamplerBuilder) == 0x60, "SamplerBuilder");

typedef nvn::TextureHandle (*DeviceGetTextureHandleFunc)(const nvn::Device *, int, int);
typedef void (*VertexStreamStateSetDefaultsFunc)(nvn::VertexStreamState *);
typedef void (*CommandBufferBindBlendStateFunc)(nvn::CommandBuffer *, const nvn::BlendState *);
typedef void (*CommandBufferBindChannelMaskStateFunc)(nvn::CommandBuffer *, const nvn::ChannelMaskState *);
typedef void (*CommandBufferBindColorStateFunc)(nvn::CommandBuffer *, const nvn::ColorState *);
typedef void (*CommandBufferBindPolygonStateFunc)(nvn::CommandBuffer *, const nvn::PolygonState *);
typedef void (*CommandBufferBindDepthStencilStateFunc)(nvn::CommandBuffer *, const nvn::DepthStencilState *);
typedef void (*CommandBufferBindVertexAttribStateFunc)(nvn::CommandBuffer *, int, const nvn::VertexAttribState *);
typedef void (*CommandBufferBindVertexStreamStateFunc)(nvn::CommandBuffer *, int, const nvn::VertexStreamState *);
typedef void (*CommandBufferBindProgramFunc)(nvn::CommandBuffer *, const nvn::Program *, int);
typedef void (*CommandBufferBindVertexBuffersFunc)(nvn::CommandBuffer *, int, int, const nvn::BufferRange *);
typedef void (*CommandBufferBindTextureFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, nvn::TextureHandle);
typedef void (*CommandBufferSetViewportFunc)(nvn::CommandBuffer *, int, int, int, int);
typedef void (*CommandBufferSetDepthRangeFunc)(nvn::CommandBuffer *, float, float);
typedef void (*CommandBufferSetStencilRefFunc)(nvn::CommandBuffer *, nvn::Face::Enum, int);
typedef void (*CommandBufferSetBlendColorFunc)(nvn::CommandBuffer *, const float *);
typedef void (*CommandBufferUpdateUniformBufferFunc)(nvn::CommandBuffer *, nvn::BufferAddress, size_t, ptrdiff_t, size_t, const void *);
typedef void (*CommandBufferBarrierFunc)(nvn::CommandBuffer *, int);
typedef void (*SamplerBuilderSetDeviceFunc)(nvn::SamplerBuilder *, nvn::Device *);
typedef void (*SamplerBuilderSetDefaultsFunc)(nvn::SamplerBuilder *);
typedef void (*SamplerBuilderSetMinMagFilterFunc)(nvn::SamplerBuilder *, nvn::MinFilter::Enum, nvn::MagFilter::Enum);
typedef void (*SamplerBuilderSetWrapModeFunc)(nvn::SamplerBuilder *, nvn::WrapMode::Enum, nvn::WrapMode::Enum, nvn::WrapMode::Enum);
typedef void (*SamplerBuilderSetLodBiasFunc)(nvn::SamplerBuilder *, float);
typedef void (*SamplerBuilderSetLodSnapFunc)(nvn::SamplerBuilder *, float);
typedef NVNboolean (*SamplerInitializeFunc)(nvn::Sampler *, const nvn::SamplerBuilder *);
typedef void (*BlendStateSetDefaultsFunc)(nvn::BlendState *);
typedef void (*BlendStateSetBlendFuncFunc)(nvn::BlendState *, nvn::BlendFunc::Enum, nvn::BlendFunc::Enum, nvn::BlendFunc::Enum, nvn::BlendFunc::Enum);
typedef void (*BlendStateSetBlendEquationFunc)(nvn::BlendState *, nvn::BlendEquation::Enum, nvn::BlendEquation::Enum);
typedef void (*ColorStateSetDefaultsFunc)(nvn::ColorState *);
typedef void (*ColorStateSetBlendEnableFunc)(nvn::ColorState *, int, NVNboolean);
typedef void (*ChannelMaskStateSetDefaultsFunc)(nvn::ChannelMaskState *);
typedef void (*ChannelMaskStateSetChannelMaskFunc)(nvn::ChannelMaskState *, int, NVNboolean, NVNboolean, NVNboolean, NVNboolean);
typedef void (*PolygonStateSetDefaultsFunc)(nvn::PolygonState *);
typedef void (*PolygonStateSetCullFaceFunc)(nvn::PolygonState *, nvn::Face::Enum);
typedef void (*PolygonStateSetFrontFaceFunc)(nvn::PolygonState *, nvn::FrontFace::Enum);
typedef void (*PolygonStateSetPolygonModeFunc)(nvn::PolygonState *, nvn::PolygonMode::Enum);
extern nvn::DeviceGetTextureHandleFunc pfncpp_nvnDeviceGetTextureHandle;
extern nvn::VertexStreamStateSetDefaultsFunc pfncpp_nvnVertexStreamStateSetDefaults;
extern nvn::CommandBufferBindBlendStateFunc pfncpp_nvnCommandBufferBindBlendState;
extern nvn::CommandBufferBindChannelMaskStateFunc pfncpp_nvnCommandBufferBindChannelMaskState;
extern nvn::CommandBufferBindColorStateFunc pfncpp_nvnCommandBufferBindColorState;
extern nvn::CommandBufferBindPolygonStateFunc pfncpp_nvnCommandBufferBindPolygonState;
extern nvn::CommandBufferBindDepthStencilStateFunc pfncpp_nvnCommandBufferBindDepthStencilState;
extern nvn::CommandBufferBindVertexAttribStateFunc pfncpp_nvnCommandBufferBindVertexAttribState;
extern nvn::CommandBufferBindVertexStreamStateFunc pfncpp_nvnCommandBufferBindVertexStreamState;
extern nvn::CommandBufferBindProgramFunc pfncpp_nvnCommandBufferBindProgram;
extern nvn::CommandBufferBindVertexBuffersFunc pfncpp_nvnCommandBufferBindVertexBuffers;
extern nvn::CommandBufferBindTextureFunc pfncpp_nvnCommandBufferBindTexture;
extern nvn::CommandBufferSetViewportFunc pfncpp_nvnCommandBufferSetViewport;
extern nvn::CommandBufferSetDepthRangeFunc pfncpp_nvnCommandBufferSetDepthRange;
extern nvn::CommandBufferSetStencilRefFunc pfncpp_nvnCommandBufferSetStencilRef;
extern nvn::CommandBufferSetBlendColorFunc pfncpp_nvnCommandBufferSetBlendColor;
extern nvn::CommandBufferUpdateUniformBufferFunc pfncpp_nvnCommandBufferUpdateUniformBuffer;
extern nvn::CommandBufferBarrierFunc pfncpp_nvnCommandBufferBarrier;
extern nvn::SamplerBuilderSetDeviceFunc pfncpp_nvnSamplerBuilderSetDevice;
extern nvn::SamplerBuilderSetDefaultsFunc pfncpp_nvnSamplerBuilderSetDefaults;
extern nvn::SamplerBuilderSetMinMagFilterFunc pfncpp_nvnSamplerBuilderSetMinMagFilter;
extern nvn::SamplerBuilderSetWrapModeFunc pfncpp_nvnSamplerBuilderSetWrapMode;
extern nvn::SamplerBuilderSetLodBiasFunc pfncpp_nvnSamplerBuilderSetLodBias;
extern nvn::SamplerBuilderSetLodSnapFunc pfncpp_nvnSamplerBuilderSetLodSnap;
extern nvn::SamplerInitializeFunc pfncpp_nvnSamplerInitialize;
extern nvn::BlendStateSetDefaultsFunc pfncpp_nvnBlendStateSetDefaults;
extern nvn::BlendStateSetBlendFuncFunc pfncpp_nvnBlendStateSetBlendFunc;
extern nvn::BlendStateSetBlendEquationFunc pfncpp_nvnBlendStateSetBlendEquation;
extern nvn::ColorStateSetDefaultsFunc pfncpp_nvnColorStateSetDefaults;
extern nvn::ColorStateSetBlendEnableFunc pfncpp_nvnColorStateSetBlendEnable;
extern nvn::ChannelMaskStateSetDefaultsFunc pfncpp_nvnChannelMaskStateSetDefaults;
extern nvn::ChannelMaskStateSetChannelMaskFunc pfncpp_nvnChannelMaskStateSetChannelMask;
extern nvn::PolygonStateSetDefaultsFunc pfncpp_nvnPolygonStateSetDefaults;
extern nvn::PolygonStateSetCullFaceFunc pfncpp_nvnPolygonStateSetCullFace;
extern nvn::PolygonStateSetFrontFaceFunc pfncpp_nvnPolygonStateSetFrontFace;
extern nvn::PolygonStateSetPolygonModeFunc pfncpp_nvnPolygonStateSetPolygonMode;

struct DrawPrimitive {  // size 0x4
    enum Enum {
        POINTS = 0,
        LINES = 1,
        LINE_LOOP = 2,
        LINE_STRIP = 3,
        TRIANGLES = 4,
        TRIANGLE_STRIP = 5,
        TRIANGLE_FAN = 6,
        QUADS = 7,
        QUAD_STRIP = 8,
        POLYGON = 9,
        LINES_ADJACENCY = 10,
        LINE_STRIP_ADJACENCY = 11,
        TRIANGLES_ADJACENCY = 12,
        TRIANGLE_STRIP_ADJACENCY = 13,
        PATCHES = 14,
        NVN_DRAW_PRIMITIVE_LARGE = 2147483647,
    };
private:
    nvn::DrawPrimitive::Enum m_value;  // 0x00
public:
    DrawPrimitive() {}
    DrawPrimitive(nvn::DrawPrimitive::Enum other) : m_value(other) {}
    operator nvn::DrawPrimitive::Enum() const { return m_value; }
};
static_assert(sizeof(DrawPrimitive) == 0x4, "DrawPrimitive");

struct IndexType {  // size 0x4
    enum Enum {
        UNSIGNED_BYTE = 0,
        UNSIGNED_SHORT = 1,
        UNSIGNED_INT = 2,
        NVN_INDEX_TYPE_LARGE = 2147483647,
    };
private:
    nvn::IndexType::Enum m_value;  // 0x00
public:
    IndexType() {}
    IndexType(nvn::IndexType::Enum other) : m_value(other) {}
    operator nvn::IndexType::Enum() const { return m_value; }
};
static_assert(sizeof(IndexType) == 0x4, "IndexType");

// nvn_CppFuncPtrImpl.h:126, 419, 421, 476
typedef void (*MemoryPoolFlushMappedRangeFunc)(const nvn::MemoryPool *, ptrdiff_t, size_t);
typedef void (*CommandBufferDrawArraysFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, int, int);
typedef void (*CommandBufferDrawElementsBaseVertexFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::IndexType::Enum, int, nvn::BufferAddress, int);
typedef void (*CommandBufferSetRenderTargetsFunc)(nvn::CommandBuffer *, int, const nvn::Texture *const *, const nvn::TextureView *const *, const nvn::Texture *, const nvn::TextureView *);
extern nvn::MemoryPoolFlushMappedRangeFunc pfncpp_nvnMemoryPoolFlushMappedRange;
extern nvn::CommandBufferDrawArraysFunc pfncpp_nvnCommandBufferDrawArrays;
extern nvn::CommandBufferDrawElementsBaseVertexFunc pfncpp_nvnCommandBufferDrawElementsBaseVertex;
extern nvn::CommandBufferSetRenderTargetsFunc pfncpp_nvnCommandBufferSetRenderTargets;

// nvn_Cpp.h (2.6.2 DWARF), placed ahead of CommandBuffer for SetMemoryCallback
struct CommandBufferMemoryEvent {  // size 0x4
    enum Enum {
        OUT_OF_COMMAND_MEMORY = 0,
        OUT_OF_CONTROL_MEMORY = 1,
        NVN_COMMAND_BUFFER_MEMORY_EVENT_LARGE = 2147483647,
    };
private:
    nvn::CommandBufferMemoryEvent::Enum m_value;  // 0x00
public:
    CommandBufferMemoryEvent();
    operator nvn::CommandBufferMemoryEvent::Enum() const;
    bool operator!=(const nvn::CommandBufferMemoryEvent::Enum &) const;
    bool operator!=(const nvn::CommandBufferMemoryEvent &) const;
    bool operator==(const nvn::CommandBufferMemoryEvent::Enum &) const;
    bool operator==(const nvn::CommandBufferMemoryEvent &) const;
};
static_assert(sizeof(CommandBufferMemoryEvent) == 0x4, "CommandBufferMemoryEvent");

class CommandBuffer;
typedef void (*CommandBufferMemoryCallbackFunc)(nvn::CommandBuffer *, nvn::CommandBufferMemoryEvent::Enum, size_t, void *);

class NVN_PRE_ALIGN CommandBuffer {  // size 0xa0
protected:
    char reserved[160];  // 0x00
public:
    void ClearColor(int index, const float * color, nvn::ClearColorMask mask);
    void WaitSync(const nvn::Sync * sync);
    void SignalEvent(const nvn::Event * event, nvn::EventSignalMode mode, nvn::EventSignalLocation location, nvn::EventSignalFlags flags, uint32_t value);
    nvn::CommandHandle EndRecording();
    void ClearDepthStencil(float depthValue, NVNboolean depthMask, int stencilValue, int stencilMask);
    void SetStencilCullCriteria(nvn::StencilFunc func, int ref, int mask);
    void ReportCounter(nvn::CounterType counter, nvn::BufferAddress buffer);
    void ResetCounter(nvn::CounterType counter);
    void BindBlendState(const nvn::BlendState * blend);
    void BindChannelMaskState(const nvn::ChannelMaskState * channelMask);
    void BindColorState(const nvn::ColorState * color);
    void BindPolygonState(const nvn::PolygonState * polygon);
    void BindDepthStencilState(const nvn::DepthStencilState * depthStencil);
    void BindVertexAttribState(int numAttribs, const nvn::VertexAttribState * attribs);
    void BindVertexStreamState(int numStreams, const nvn::VertexStreamState * streams);
    void BindProgram(const nvn::Program * program, nvn::ShaderStageBits stages);
    void BindVertexBuffers(int first, int count, const nvn::BufferRange * buffers);
    void BindTexture(nvn::ShaderStage stage, int index, nvn::TextureHandle texture);
    void SetViewport(int x, int y, int w, int h);
    void SetDepthRange(float n, float f);
    void SetStencilRef(nvn::Face faces, int ref);
    void SetBlendColor(const float * blendColor);
    void UpdateUniformBuffer(nvn::BufferAddress buffer, size_t bufferSize, ptrdiff_t updateOffset, size_t updateSize, const void * data);
    void Barrier(nvn::BarrierBits barrier);
    void DrawArrays(nvn::DrawPrimitive mode, int first, int count);
    void DrawElementsBaseVertex(nvn::DrawPrimitive mode, nvn::IndexType type, int count, nvn::BufferAddress indexBuffer, int baseVertex);
    void SetRenderTargets(int numColors, const nvn::Texture *const * colors, const nvn::TextureView *const * colorViews, const nvn::Texture * depthStencil, const nvn::TextureView * depthStencilView);
    void Finalize();
    NVNboolean Initialize(nvn::Device * device);
    void SetMemoryCallback(nvn::CommandBufferMemoryCallbackFunc callback);
    void AddCommandMemory(const nvn::MemoryPool * pool, ptrdiff_t offset, size_t size);
    void AddControlMemory(void * memory, size_t size);
    void CopyTextureToTexture(const nvn::Texture * srcTexture, const nvn::TextureView * srcView, const nvn::CopyRegion * srcRegion, const nvn::Texture * dstTexture, const nvn::TextureView * dstView, const nvn::CopyRegion * dstRegion, nvn::CopyFlags flags);
    void BeginRecording();
    void BindUniformBuffer(nvn::ShaderStage stage, int index, nvn::BufferAddress buffer, size_t size);
    void SetTexturePool(const nvn::TexturePool * texturePool);
    void SetSamplerPool(const nvn::SamplerPool * samplerPool);
};
static_assert(sizeof(CommandBuffer) == 0xa0, "CommandBuffer");

class NVN_PRE_ALIGN DepthStencilState {  // size 0x8
protected:
    char reserved[8];  // 0x00
public:
    nvn::DepthStencilState & SetDepthTestEnable(NVNboolean enable);
    nvn::DepthStencilState & SetDepthWriteEnable(NVNboolean enable);
    nvn::DepthStencilState & SetDepthFunc(nvn::DepthFunc func);
    nvn::DepthStencilState & SetStencilTestEnable(NVNboolean enable);
    nvn::DepthStencilState & SetStencilFunc(nvn::Face faces, nvn::StencilFunc func);
    nvn::DepthStencilState & SetStencilOp(nvn::Face faces, nvn::StencilOp fail, nvn::StencilOp depthFail, nvn::StencilOp depthPass);
    nvn::DepthStencilState & SetDefaults();
};
static_assert(sizeof(DepthStencilState) == 0x8, "DepthStencilState");

class NVN_PRE_ALIGN Queue {  // size 0x2000
protected:
    char reserved[8192];  // 0x00
    Queue(const nvn::Queue &);
    const nvn::Queue & operator=(const nvn::Queue &);
public:
    Queue() {}
    void Finish();
    void SubmitCommands(int numCommands, const nvn::CommandHandle * handles);
    void PresentTexture(nvn::Window * window, int textureIndex);
    NVNboolean Initialize(const nvn::QueueBuilder * builder);
};
static_assert(sizeof(Queue) == 0x2000, "Queue");

class NVN_PRE_ALIGN Texture {  // size 0xc0
protected:
    char reserved[192];  // 0x00
    Texture(const nvn::Texture &);
    const nvn::Texture & operator=(const nvn::Texture &);
public:
    Texture() {}
    ptrdiff_t GetStride() const;
    ptrdiff_t GetViewOffset(const nvn::TextureView * textureView) const;
    void Finalize();
    void FlushTexels(const nvn::TextureView * textureView, const nvn::CopyRegion * region) const;
    NVNboolean Initialize(const nvn::TextureBuilder * builder);
};
static_assert(sizeof(Texture) == 0xc0, "Texture");

class NVN_PRE_ALIGN Sampler {  // size 0x60
protected:
    char reserved[96];  // 0x00
    Sampler(const nvn::Sampler &);
    const nvn::Sampler & operator=(const nvn::Sampler &);
public:
    Sampler() = default;  // trivial: NX64NVNEngine.cpp's static NVNSampler array has no run-time initializer in the original
    NVNboolean Initialize(const nvn::SamplerBuilder * builder);
};
static_assert(sizeof(Sampler) == 0x60, "Sampler");

class NVN_PRE_ALIGN SamplerPool {  // size 0x20
protected:
    char reserved[32];  // 0x00
    SamplerPool(const nvn::SamplerPool &);
    const nvn::SamplerPool & operator=(const nvn::SamplerPool &);
public:
    SamplerPool() {}
    void RegisterSampler(int id, const nvn::Sampler * sampler) const;
    NVNboolean Initialize(const nvn::MemoryPool * memoryPool, ptrdiff_t offset, int numDescriptors);
    void Finalize();
};
static_assert(sizeof(SamplerPool) == 0x20, "SamplerPool");

class NVN_PRE_ALIGN TexturePool {  // size 0x20
protected:
    char reserved[32];  // 0x00
    TexturePool(const nvn::TexturePool &);
    const nvn::TexturePool & operator=(const nvn::TexturePool &);
public:
    TexturePool() {}
    void RegisterTexture(int id, const nvn::Texture * texture, const nvn::TextureView * view) const;
    NVNboolean Initialize(const nvn::MemoryPool * memoryPool, ptrdiff_t offset, int numDescriptors);
    void Finalize();
};
static_assert(sizeof(TexturePool) == 0x20, "TexturePool");

// nvn_Cpp.h: TextureTarget (DWARF enumerators)
struct TextureTarget {  // size 0x4
    enum Enum {
        TARGET_1D = 0,
        TARGET_2D = 1,
        TARGET_3D = 2,
        TARGET_1D_ARRAY = 3,
        TARGET_2D_ARRAY = 4,
        TARGET_2D_MULTISAMPLE = 5,
        TARGET_2D_MULTISAMPLE_ARRAY = 6,
        TARGET_RECTANGLE = 7,
        TARGET_CUBEMAP = 8,
        TARGET_CUBEMAP_ARRAY = 9,
        TARGET_BUFFER = 10,
        NVN_TEXTURE_TARGET_LARGE = 2147483647,
    };
private:
    nvn::TextureTarget::Enum m_value;  // 0x00
public:
    TextureTarget() {}
    TextureTarget(nvn::TextureTarget::Enum e) : m_value(e) {}
    operator nvn::TextureTarget::Enum() const { return m_value; }
};

class NVN_PRE_ALIGN TextureView {  // size 0x28
protected:
    char reserved[40];  // 0x00
public:
    TextureView() {}
    nvn::TextureView & SetDefaults();
    nvn::TextureView & SetLayers(int baseLayer, int numLayers);
    nvn::TextureView & SetTarget(nvn::TextureTarget target);
};
static_assert(sizeof(TextureView) == 0x28, "TextureView");

// The rest of the entry-point table (nvn_CppFuncPtrImpl.h:31-528), for nvn::nvnLoadCPPProcs: every
// typedef, enum wrapper and forward declaration below is copied from the 2.6.2 DWARF
// (build/dwarf/headers/NintendoSDK/Include/NVN/nvn_Cpp.h) by build/try/agentB_nvnprocs.py. The enum
// wrappers' members are declared as the DWARF lists them, without bodies; give one a body when a
// file first uses it.
typedef uint64_t ImageHandle;
typedef uint64_t TextureAddress;
class DeviceBuilder;
union QueueErrorInfo;
class QueueBuilder;
class WindowBuilder;
struct Rectangle;
struct MappingRequest;
class TextureBuilder;
struct TextureSparseTileLayout;
class MultisampleState;
struct DrawTextureRegion;
class EventBuilder;
typedef void (*DeviceBuilderSetDefaultsFunc)(nvn::DeviceBuilder *);
typedef void (*DeviceBuilderSetFlagsFunc)(nvn::DeviceBuilder *, int);
typedef int (*DeviceBuilderGetFlagsFunc)(const nvn::DeviceBuilder *);
typedef NVNboolean (*DeviceInitializeFunc)(nvn::Device *, const nvn::DeviceBuilder *);
typedef void (*DeviceFinalizeFunc)(nvn::Device *);
typedef void (*DeviceSetDebugLabelFunc)(nvn::Device *, const char *);
typedef void (*GenericFuncPtrFunc)();
typedef nvn::GenericFuncPtrFunc (*DeviceGetProcAddressFunc)(const nvn::Device *, const char *);
struct DeviceInfo {  // size 0x4
    enum Enum {
        API_MAJOR_VERSION = 0,
        API_MINOR_VERSION = 1,
        UNIFORM_BUFFER_BINDINGS_PER_STAGE = 2,
        MAX_UNIFORM_BUFFER_SIZE = 3,
        UNIFORM_BUFFER_ALIGNMENT = 4,
        COLOR_BUFFER_BINDINGS = 5,
        VERTEX_BUFFER_BINDINGS = 6,
        TRANSFORM_FEEDBACK_BUFFER_BINDINGS = 7,
        SHADER_STORAGE_BUFFER_BINDINGS_PER_STAGE = 8,
        TEXTURE_BINDINGS_PER_STAGE = 9,
        COUNTER_ALIGNMENT = 10,
        TRANSFORM_FEEDBACK_BUFFER_ALIGNMENT = 11,
        TRANSFORM_FEEDBACK_CONTROL_ALIGNMENT = 12,
        INDIRECT_DRAW_ALIGNMENT = 13,
        VERTEX_ATTRIBUTES = 14,
        TEXTURE_DESCRIPTOR_SIZE = 15,
        SAMPLER_DESCRIPTOR_SIZE = 16,
        RESERVED_TEXTURE_DESCRIPTORS = 17,
        RESERVED_SAMPLER_DESCRIPTORS = 18,
        COMMAND_BUFFER_COMMAND_ALIGNMENT = 19,
        COMMAND_BUFFER_CONTROL_ALIGNMENT = 20,
        COMMAND_BUFFER_MIN_COMMAND_SIZE = 21,
        COMMAND_BUFFER_MIN_CONTROL_SIZE = 22,
        SHADER_SCRATCH_MEMORY_SCALE_FACTOR_MINIMUM = 23,
        SHADER_SCRATCH_MEMORY_SCALE_FACTOR_RECOMMENDED = 24,
        SHADER_SCRATCH_MEMORY_ALIGNMENT = 25,
        SHADER_SCRATCH_MEMORY_GRANULARITY = 26,
        MAX_TEXTURE_ANISOTROPY = 27,
        MAX_COMPUTE_WORK_GROUP_SIZE_X = 28,
        MAX_COMPUTE_WORK_GROUP_SIZE_Y = 29,
        MAX_COMPUTE_WORK_GROUP_SIZE_Z = 30,
        MAX_COMPUTE_WORK_GROUP_SIZE_THREADS = 31,
        MAX_COMPUTE_DISPATCH_WORK_GROUPS_X = 32,
        MAX_COMPUTE_DISPATCH_WORK_GROUPS_Y = 33,
        MAX_COMPUTE_DISPATCH_WORK_GROUPS_Z = 34,
        IMAGE_BINDINGS_PER_STAGE = 35,
        MAX_TEXTURE_POOL_SIZE = 36,
        MAX_SAMPLER_POOL_SIZE = 37,
        MAX_VIEWPORTS = 38,
        MEMPOOL_TEXTURE_OBJECT_PAGE_ALIGNMENT = 39,
        SUPPORTS_MIN_MAX_FILTERING = 40,
        SUPPORTS_STENCIL8_FORMAT = 41,
        SUPPORTS_ASTC_FORMATS = 42,
        L2_SIZE = 43,
        MAX_TEXTURE_LEVELS = 44,
        MAX_TEXTURE_LAYERS = 45,
        GLSLC_MAX_SUPPORTED_GPU_CODE_MAJOR_VERSION = 46,
        GLSLC_MIN_SUPPORTED_GPU_CODE_MAJOR_VERSION = 47,
        GLSLC_MAX_SUPPORTED_GPU_CODE_MINOR_VERSION = 48,
        GLSLC_MIN_SUPPORTED_GPU_CODE_MINOR_VERSION = 49,
        SUPPORTS_CONSERVATIVE_RASTER = 50,
        SUBPIXEL_BITS = 51,
        MAX_SUBPIXEL_BIAS_BITS = 52,
        INDIRECT_DISPATCH_ALIGNMENT = 53,
        ZCULL_SAVE_RESTORE_ALIGNMENT = 54,
        SHADER_SCRATCH_MEMORY_COMPUTE_SCALE_FACTOR_MINIMUM = 55,
        LINEAR_TEXTURE_STRIDE_ALIGNMENT = 56,
        LINEAR_RENDER_TARGET_STRIDE_ALIGNMENT = 57,
        MEMORY_POOL_PAGE_SIZE = 58,
        SUPPORTS_ZERO_FROM_UNMAPPED_VIRTUAL_POOL_PAGES = 59,
        UNIFORM_BUFFER_UPDATE_ALIGNMENT = 60,
        MAX_TEXTURE_SIZE = 61,
        MAX_BUFFER_TEXTURE_SIZE = 62,
        MAX_3D_TEXTURE_SIZE = 63,
        MAX_CUBE_MAP_TEXTURE_SIZE = 64,
        MAX_RECTANGLE_TEXTURE_SIZE = 65,
        SUPPORTS_PASSTHROUGH_GEOMETRY_SHADERS = 66,
        SUPPORTS_VIEWPORT_SWIZZLE = 67,
        SUPPORTS_SPARSE_TILED_PACKAGED_TEXTURES = 68,
        SUPPORTS_ADVANCED_BLEND_MODES = 69,
        MAX_PRESENT_INTERVAL = 70,
        SUPPORTS_DRAW_TEXTURE = 71,
        SUPPORTS_TARGET_INDEPENDENT_RASTERIZATION = 72,
        SUPPORTS_FRAGMENT_COVERAGE_TO_COLOR = 73,
        SUPPORTS_POST_DEPTH_COVERAGE = 74,
        SUPPORTS_IMAGES_USING_TEXTURE_HANDLES = 75,
        SUPPORTS_SAMPLE_LOCATIONS = 76,
        MAX_SAMPLE_LOCATION_TABLE_ENTRIES = 77,
        SHADER_CODE_MEMORY_POOL_PADDING_SIZE = 78,
        MAX_PATCH_SIZE = 79,
        QUEUE_COMMAND_MEMORY_GRANULARITY = 80,
        QUEUE_COMMAND_MEMORY_MIN_SIZE = 81,
        QUEUE_COMMAND_MEMORY_DEFAULT_SIZE = 82,
        QUEUE_COMPUTE_MEMORY_GRANULARITY = 83,
        QUEUE_COMPUTE_MEMORY_MIN_SIZE = 84,
        QUEUE_COMPUTE_MEMORY_DEFAULT_SIZE = 85,
        QUEUE_COMMAND_MEMORY_MIN_FLUSH_THRESHOLD = 86,
        SUPPORTS_FRAGMENT_SHADER_INTERLOCK = 87,
        MAX_TEXTURES_PER_WINDOW = 88,
        MIN_TEXTURES_PER_WINDOW = 89,
        SUPPORTS_DEBUG_LAYER = 90,
        QUEUE_CONTROL_MEMORY_MIN_SIZE = 91,
        QUEUE_CONTROL_MEMORY_DEFAULT_SIZE = 92,
        QUEUE_CONTROL_MEMORY_GRANULARITY = 93,
        SEPARATE_TEXTURE_BINDINGS_PER_STAGE = 94,
        SEPARATE_SAMPLER_BINDINGS_PER_STAGE = 95,
        DEBUG_GROUPS_MAX_DOMAIN_ID = 96,
        EVENTS_SUPPORT_REDUCTION_OPERATIONS = 97,
        NVN_DEVICE_INFO_LARGE = 2147483647,
    };
private:
    nvn::DeviceInfo::Enum m_value;  // 0x00
public:
    DeviceInfo();
    operator nvn::DeviceInfo::Enum() const;
    bool operator!=(const nvn::DeviceInfo::Enum &) const;
    bool operator!=(const nvn::DeviceInfo &) const;
    bool operator==(const nvn::DeviceInfo::Enum &) const;
    bool operator==(const nvn::DeviceInfo &) const;
};
static_assert(sizeof(DeviceInfo) == 0x4, "DeviceInfo");

typedef void (*DeviceGetIntegerFunc)(const nvn::Device *, nvn::DeviceInfo::Enum, int *);
typedef uint64_t (*DeviceGetCurrentTimestampInNanosecondsFunc)(const nvn::Device *);
typedef void (*DeviceSetIntermediateShaderCacheFunc)(nvn::Device *, int);
typedef nvn::TextureHandle (*DeviceGetTexelFetchHandleFunc)(const nvn::Device *, int);
typedef nvn::ImageHandle (*DeviceGetImageHandleFunc)(const nvn::Device *, int);
struct DebugCallbackSource {  // size 0x4
    enum Enum {
        API = 0,
        NVN_DEBUG_CALLBACK_SOURCE_LARGE = 2147483647,
    };
private:
    nvn::DebugCallbackSource::Enum m_value;  // 0x00
public:
    DebugCallbackSource();
    operator nvn::DebugCallbackSource::Enum() const;
    bool operator!=(const nvn::DebugCallbackSource::Enum &) const;
    bool operator!=(const nvn::DebugCallbackSource &) const;
    bool operator==(const nvn::DebugCallbackSource::Enum &) const;
    bool operator==(const nvn::DebugCallbackSource &) const;
};
static_assert(sizeof(DebugCallbackSource) == 0x4, "DebugCallbackSource");

struct DebugCallbackType {  // size 0x4
    enum Enum {
        API_ERROR = 0,
        API_WARNING = 1,
        NVN_DEBUG_CALLBACK_TYPE_LARGE = 2147483647,
    };
private:
    nvn::DebugCallbackType::Enum m_value;  // 0x00
public:
    DebugCallbackType();
    operator nvn::DebugCallbackType::Enum() const;
    bool operator!=(const nvn::DebugCallbackType::Enum &) const;
    bool operator!=(const nvn::DebugCallbackType &) const;
    bool operator==(const nvn::DebugCallbackType::Enum &) const;
    bool operator==(const nvn::DebugCallbackType &) const;
};
static_assert(sizeof(DebugCallbackType) == 0x4, "DebugCallbackType");

struct DebugCallbackSeverity {  // size 0x4
    enum Enum {
        HIGH = 0,
        MEDIUM = 1,
        LOW = 2,
        NOTIFICATION = 3,
        NVN_DEBUG_CALLBACK_SEVERITY_LARGE = 2147483647,
    };
private:
    nvn::DebugCallbackSeverity::Enum m_value;  // 0x00
public:
    DebugCallbackSeverity();
    operator nvn::DebugCallbackSeverity::Enum() const;
    bool operator!=(const nvn::DebugCallbackSeverity::Enum &) const;
    bool operator!=(const nvn::DebugCallbackSeverity &) const;
    bool operator==(const nvn::DebugCallbackSeverity::Enum &) const;
    bool operator==(const nvn::DebugCallbackSeverity &) const;
};
static_assert(sizeof(DebugCallbackSeverity) == 0x4, "DebugCallbackSeverity");

typedef void (*DebugCallbackFunc)(nvn::DebugCallbackSource::Enum, nvn::DebugCallbackType::Enum, int, nvn::DebugCallbackSeverity::Enum, const char *, void *);
typedef void (*DeviceInstallDebugCallbackFunc)(nvn::Device *, const nvn::DebugCallbackFunc, void *, NVNboolean);
typedef NVNdebugDomainId (*DeviceGenerateDebugDomainIdFunc)(const nvn::Device *, const char *);
struct WindowOriginMode {  // size 0x4
    enum Enum {
        LOWER_LEFT = 0,
        UPPER_LEFT = 1,
        NVN_WINDOW_ORIGIN_MODE_LARGE = 2147483647,
    };
private:
    nvn::WindowOriginMode::Enum m_value;  // 0x00
public:
    WindowOriginMode() {}
    WindowOriginMode(nvn::WindowOriginMode::Enum other) : m_value(other) {}
    WindowOriginMode(const nvn::WindowOriginMode & other) : m_value(other.m_value) {}
    operator nvn::WindowOriginMode::Enum() const { return m_value; }
    bool operator!=(const nvn::WindowOriginMode::Enum &) const;
    bool operator!=(const nvn::WindowOriginMode &) const;
    bool operator==(const nvn::WindowOriginMode::Enum &) const;
    bool operator==(const nvn::WindowOriginMode &) const;
};
static_assert(sizeof(WindowOriginMode) == 0x4, "WindowOriginMode");

typedef void (*DeviceSetWindowOriginModeFunc)(nvn::Device *, nvn::WindowOriginMode::Enum);
struct DepthMode {  // size 0x4
    enum Enum {
        NEAR_IS_MINUS_W = 0,
        NEAR_IS_ZERO = 1,
        NVN_DEPTH_MODE_LARGE = 2147483647,
    };
private:
    nvn::DepthMode::Enum m_value;  // 0x00
public:
    DepthMode() {}
    DepthMode(nvn::DepthMode::Enum other) : m_value(other) {}
    DepthMode(const nvn::DepthMode & other) : m_value(other.m_value) {}
    operator nvn::DepthMode::Enum() const { return m_value; }
    bool operator!=(const nvn::DepthMode::Enum &) const;
    bool operator!=(const nvn::DepthMode &) const;
    bool operator==(const nvn::DepthMode::Enum &) const;
    bool operator==(const nvn::DepthMode &) const;
};
static_assert(sizeof(DepthMode) == 0x4, "DepthMode");

typedef void (*DeviceSetDepthModeFunc)(nvn::Device *, nvn::DepthMode::Enum);
typedef NVNboolean (*DeviceRegisterFastClearColorFunc)(nvn::Device *, const float *, nvn::Format::Enum);
typedef NVNboolean (*DeviceRegisterFastClearColoriFunc)(nvn::Device *, const int *, nvn::Format::Enum);
typedef NVNboolean (*DeviceRegisterFastClearColoruiFunc)(nvn::Device *, const uint32_t *, nvn::Format::Enum);
typedef NVNboolean (*DeviceRegisterFastClearDepthFunc)(nvn::Device *, float);
typedef nvn::WindowOriginMode::Enum (*DeviceGetWindowOriginModeFunc)(const nvn::Device *);
typedef nvn::DepthMode::Enum (*DeviceGetDepthModeFunc)(const nvn::Device *);
typedef uint64_t (*DeviceGetTimestampInNanosecondsFunc)(const nvn::Device *, const nvn::CounterData *);
typedef void (*DeviceApplyDeferredFinalizesFunc)(nvn::Device *, int);
typedef void (*DeviceFinalizeCommandHandleFunc)(nvn::Device *, nvn::CommandHandle);
struct DebugObjectType {  // size 0x4
    enum Enum {
        WINDOW = 0,
        QUEUE = 1,
        COMMAND_BUFFER = 2,
        MEMORY_POOL = 3,
        SYNC = 7,
        PROGRAM = 9,
        TEXTURE_POOL = 10,
        SAMPLER_POOL = 11,
        NVN_DEBUG_OBJECT_TYPE_LARGE = 2147483647,
    };
private:
    nvn::DebugObjectType::Enum m_value;  // 0x00
public:
    DebugObjectType();
    operator nvn::DebugObjectType::Enum() const;
    bool operator!=(const nvn::DebugObjectType::Enum &) const;
    bool operator!=(const nvn::DebugObjectType &) const;
    bool operator==(const nvn::DebugObjectType::Enum &) const;
    bool operator==(const nvn::DebugObjectType &) const;
};
static_assert(sizeof(DebugObjectType) == 0x4, "DebugObjectType");

typedef void (*WalkDebugDatabaseCallbackFunc)(void *, void *);
typedef void (*DeviceWalkDebugDatabaseFunc)(const nvn::Device *, nvn::DebugObjectType::Enum, nvn::WalkDebugDatabaseCallbackFunc, void *);
struct SeparateTextureHandle {  // size 0x8
    uint64_t value;  // 0x00
};
static_assert(sizeof(SeparateTextureHandle) == 0x8, "SeparateTextureHandle");

typedef nvn::SeparateTextureHandle (*DeviceGetSeparateTextureHandleFunc)(const nvn::Device *, int);
struct SeparateSamplerHandle {  // size 0x8
    uint64_t value;  // 0x00
};
static_assert(sizeof(SeparateSamplerHandle) == 0x8, "SeparateSamplerHandle");

typedef nvn::SeparateSamplerHandle (*DeviceGetSeparateSamplerHandleFunc)(const nvn::Device *, int);
typedef NVNboolean (*DeviceIsExternalDebuggerAttachedFunc)(const nvn::Device *);
struct QueueGetErrorResult {  // size 0x4
    enum Enum {
        GPU_NO_ERROR = 0,
        GPU_ERROR_UNKNOWN = 1,
        GPU_ERROR_MMU_FAULT = 2,
        GPU_ERROR_PBDMA_EXCEPTION = 3,
        GPU_ERROR_ENGINE_EXCEPTION = 4,
        GPU_ERROR_TIMEOUT = 5,
        NVN_QUEUE_GET_ERROR_RESULT_LARGE = 2147483647,
    };
private:
    nvn::QueueGetErrorResult::Enum m_value;  // 0x00
public:
    QueueGetErrorResult();
    operator nvn::QueueGetErrorResult::Enum() const;
    bool operator!=(const nvn::QueueGetErrorResult::Enum &) const;
    bool operator!=(const nvn::QueueGetErrorResult &) const;
    bool operator==(const nvn::QueueGetErrorResult::Enum &) const;
    bool operator==(const nvn::QueueGetErrorResult &) const;
};
static_assert(sizeof(QueueGetErrorResult) == 0x4, "QueueGetErrorResult");

typedef nvn::QueueGetErrorResult::Enum (*QueueGetErrorFunc)(nvn::Queue *, nvn::QueueErrorInfo *);
typedef size_t (*QueueGetTotalCommandMemoryUsedFunc)(nvn::Queue *);
typedef size_t (*QueueGetTotalControlMemoryUsedFunc)(nvn::Queue *);
typedef size_t (*QueueGetTotalComputeMemoryUsedFunc)(nvn::Queue *);
typedef void (*QueueResetMemoryUsageCountsFunc)(nvn::Queue *);
typedef void (*QueueBuilderSetDeviceFunc)(nvn::QueueBuilder *, nvn::Device *);
typedef void (*QueueBuilderSetDefaultsFunc)(nvn::QueueBuilder *);
typedef void (*QueueBuilderSetFlagsFunc)(nvn::QueueBuilder *, int);
typedef void (*QueueBuilderSetCommandMemorySizeFunc)(nvn::QueueBuilder *, size_t);
typedef void (*QueueBuilderSetComputeMemorySizeFunc)(nvn::QueueBuilder *, size_t);
typedef void (*QueueBuilderSetControlMemorySizeFunc)(nvn::QueueBuilder *, size_t);
typedef size_t (*QueueBuilderGetQueueMemorySizeFunc)(const nvn::QueueBuilder *);
typedef void (*QueueBuilderSetQueueMemoryFunc)(nvn::QueueBuilder *, void *, size_t);
typedef void (*QueueBuilderSetCommandFlushThresholdFunc)(nvn::QueueBuilder *, size_t);
typedef const nvn::Device *(*QueueBuilderGetDeviceFunc)(const nvn::QueueBuilder *);
typedef int (*QueueBuilderGetFlagsFunc)(const nvn::QueueBuilder *);
typedef NVNboolean (*QueueBuilderGetCommandMemorySizeFunc)(const nvn::QueueBuilder *, int *);
typedef NVNboolean (*QueueBuilderGetComputeMemorySizeFunc)(const nvn::QueueBuilder *, int *);
typedef NVNboolean (*QueueBuilderGetControlMemorySizeFunc)(const nvn::QueueBuilder *, int *);
typedef NVNboolean (*QueueBuilderGetCommandFlushThresholdFunc)(const nvn::QueueBuilder *, int *);
typedef int (*QueueBuilderGetMemorySizeFunc)(const nvn::QueueBuilder *);
typedef void *(*QueueBuilderGetMemoryFunc)(const nvn::QueueBuilder *);
typedef NVNboolean (*QueueInitializeFunc)(nvn::Queue *, const nvn::QueueBuilder *);
typedef void (*QueueFinalizeFunc)(nvn::Queue *);
typedef void (*QueueSetDebugLabelFunc)(nvn::Queue *, const char *);
typedef void (*QueueFlushFunc)(nvn::Queue *);
struct QueueAcquireTextureResult {  // size 0x4
    enum Enum {
        SUCCESS = 0,
        NATIVE_ERROR = 1,
        NVN_QUEUE_ACQUIRE_TEXTURE_RESULT_LARGE = 2147483647,
    };
private:
    nvn::QueueAcquireTextureResult::Enum m_value;  // 0x00
public:
    QueueAcquireTextureResult();
    operator nvn::QueueAcquireTextureResult::Enum() const;
    bool operator!=(const nvn::QueueAcquireTextureResult::Enum &) const;
    bool operator!=(const nvn::QueueAcquireTextureResult &) const;
    bool operator==(const nvn::QueueAcquireTextureResult::Enum &) const;
    bool operator==(const nvn::QueueAcquireTextureResult &) const;
};
static_assert(sizeof(QueueAcquireTextureResult) == 0x4, "QueueAcquireTextureResult");

typedef nvn::QueueAcquireTextureResult::Enum (*QueueAcquireTextureFunc)(nvn::Queue *, nvn::Window *, int *);
typedef void (*WindowBuilderSetDeviceFunc)(nvn::WindowBuilder *, nvn::Device *);
typedef void (*WindowBuilderSetDefaultsFunc)(nvn::WindowBuilder *);
typedef void (*WindowBuilderSetNativeWindowFunc)(nvn::WindowBuilder *, NVNnativeWindow);
typedef void (*WindowBuilderSetTexturesFunc)(nvn::WindowBuilder *, int, nvn::Texture *const *);
typedef void (*WindowBuilderSetPresentIntervalFunc)(nvn::WindowBuilder *, int);
typedef const nvn::Device *(*WindowBuilderGetDeviceFunc)(const nvn::WindowBuilder *);
typedef int (*WindowBuilderGetNumTexturesFunc)(const nvn::WindowBuilder *);
typedef const nvn::Texture *(*WindowBuilderGetTextureFunc)(const nvn::WindowBuilder *, int);
typedef NVNnativeWindow (*WindowBuilderGetNativeWindowFunc)(const nvn::WindowBuilder *);
typedef int (*WindowBuilderGetPresentIntervalFunc)(const nvn::WindowBuilder *);
typedef NVNboolean (*WindowInitializeFunc)(nvn::Window *, const nvn::WindowBuilder *);
typedef void (*WindowFinalizeFunc)(nvn::Window *);
typedef void (*WindowSetDebugLabelFunc)(nvn::Window *, const char *);
struct WindowAcquireTextureResult {  // size 0x4
    enum Enum {
        SUCCESS = 0,
        NATIVE_ERROR = 1,
        NVN_WINDOW_ACQUIRE_TEXTURE_RESULT_LARGE = 2147483647,
    };
private:
    nvn::WindowAcquireTextureResult::Enum m_value;  // 0x00
public:
    WindowAcquireTextureResult() {}
    WindowAcquireTextureResult(nvn::WindowAcquireTextureResult::Enum other) : m_value(other) {}
    WindowAcquireTextureResult(const nvn::WindowAcquireTextureResult & other) : m_value(other.m_value) {}
    operator nvn::WindowAcquireTextureResult::Enum() const { return m_value; }
    bool operator!=(const nvn::WindowAcquireTextureResult::Enum &) const;
    bool operator!=(const nvn::WindowAcquireTextureResult &) const;
    bool operator==(const nvn::WindowAcquireTextureResult::Enum &) const;
    bool operator==(const nvn::WindowAcquireTextureResult &) const;
};
static_assert(sizeof(WindowAcquireTextureResult) == 0x4, "WindowAcquireTextureResult");

typedef nvn::WindowAcquireTextureResult::Enum (*WindowAcquireTextureFunc)(nvn::Window *, nvn::Sync *, int *);
typedef NVNnativeWindow (*WindowGetNativeWindowFunc)(const nvn::Window *);
typedef int (*WindowGetPresentIntervalFunc)(const nvn::Window *);
typedef void (*WindowSetPresentIntervalFunc)(nvn::Window *, int);
typedef void (*WindowGetCropFunc)(const nvn::Window *, nvn::Rectangle *);
typedef void (*ProgramSetDebugLabelFunc)(nvn::Program *, const char *);
typedef const nvn::Device *(*MemoryPoolBuilderGetDeviceFunc)(const nvn::MemoryPoolBuilder *);
typedef void *(*MemoryPoolBuilderGetMemoryFunc)(const nvn::MemoryPoolBuilder *);
typedef size_t (*MemoryPoolBuilderGetSizeFunc)(const nvn::MemoryPoolBuilder *);
typedef int (*MemoryPoolBuilderGetFlagsFunc)(const nvn::MemoryPoolBuilder *);
typedef void (*MemoryPoolSetDebugLabelFunc)(nvn::MemoryPool *, const char *);
typedef void *(*MemoryPoolMapFunc)(const nvn::MemoryPool *);
typedef void (*MemoryPoolInvalidateMappedRangeFunc)(const nvn::MemoryPool *, ptrdiff_t, size_t);
typedef NVNboolean (*MemoryPoolMapVirtualFunc)(nvn::MemoryPool *, int, const nvn::MappingRequest *);
typedef size_t (*MemoryPoolGetSizeFunc)(const nvn::MemoryPool *);
typedef int (*MemoryPoolGetFlagsFunc)(const nvn::MemoryPool *);
typedef NVNboolean (*TexturePoolInitializeFunc)(nvn::TexturePool *, const nvn::MemoryPool *, ptrdiff_t, int);
typedef void (*TexturePoolSetDebugLabelFunc)(nvn::TexturePool *, const char *);
typedef void (*TexturePoolFinalizeFunc)(nvn::TexturePool *);
typedef void (*TexturePoolRegisterTextureFunc)(const nvn::TexturePool *, int, const nvn::Texture *, const nvn::TextureView *);
typedef void (*TexturePoolRegisterImageFunc)(const nvn::TexturePool *, int, const nvn::Texture *, const nvn::TextureView *);
typedef const nvn::MemoryPool *(*TexturePoolGetMemoryPoolFunc)(const nvn::TexturePool *);
typedef ptrdiff_t (*TexturePoolGetMemoryOffsetFunc)(const nvn::TexturePool *);
typedef int (*TexturePoolGetSizeFunc)(const nvn::TexturePool *);
typedef NVNboolean (*SamplerPoolInitializeFunc)(nvn::SamplerPool *, const nvn::MemoryPool *, ptrdiff_t, int);
typedef void (*SamplerPoolSetDebugLabelFunc)(nvn::SamplerPool *, const char *);
typedef void (*SamplerPoolFinalizeFunc)(nvn::SamplerPool *);
typedef void (*SamplerPoolRegisterSamplerBuilderFunc)(const nvn::SamplerPool *, int, const nvn::SamplerBuilder *);
typedef const nvn::MemoryPool *(*SamplerPoolGetMemoryPoolFunc)(const nvn::SamplerPool *);
typedef ptrdiff_t (*SamplerPoolGetMemoryOffsetFunc)(const nvn::SamplerPool *);
typedef int (*SamplerPoolGetSizeFunc)(const nvn::SamplerPool *);
typedef const nvn::Device *(*BufferBuilderGetDeviceFunc)(const nvn::BufferBuilder *);
typedef nvn::MemoryPool *(*BufferBuilderGetMemoryPoolFunc)(const nvn::BufferBuilder *);
typedef ptrdiff_t (*BufferBuilderGetMemoryOffsetFunc)(const nvn::BufferBuilder *);
typedef size_t (*BufferBuilderGetSizeFunc)(const nvn::BufferBuilder *);
typedef void (*BufferSetDebugLabelFunc)(nvn::Buffer *, const char *);
typedef void *(*BufferMapFunc)(const nvn::Buffer *);
typedef void (*BufferFlushMappedRangeFunc)(const nvn::Buffer *, ptrdiff_t, size_t);
typedef void (*BufferInvalidateMappedRangeFunc)(const nvn::Buffer *, ptrdiff_t, size_t);
typedef nvn::MemoryPool *(*BufferGetMemoryPoolFunc)(const nvn::Buffer *);
typedef size_t (*BufferGetSizeFunc)(const nvn::Buffer *);
typedef uint64_t (*BufferGetDebugIDFunc)(const nvn::Buffer *);
typedef void (*TextureBuilderSetDeviceFunc)(nvn::TextureBuilder *, nvn::Device *);
typedef void (*TextureBuilderSetDefaultsFunc)(nvn::TextureBuilder *);
typedef void (*TextureBuilderSetFlagsFunc)(nvn::TextureBuilder *, int);
typedef void (*TextureBuilderSetTargetFunc)(nvn::TextureBuilder *, nvn::TextureTarget::Enum);
typedef void (*TextureBuilderSetWidthFunc)(nvn::TextureBuilder *, int);
typedef void (*TextureBuilderSetHeightFunc)(nvn::TextureBuilder *, int);
typedef void (*TextureBuilderSetDepthFunc)(nvn::TextureBuilder *, int);
typedef void (*TextureBuilderSetSize1DFunc)(nvn::TextureBuilder *, int);
typedef void (*TextureBuilderSetSize2DFunc)(nvn::TextureBuilder *, int, int);
typedef void (*TextureBuilderSetSize3DFunc)(nvn::TextureBuilder *, int, int, int);
typedef void (*TextureBuilderSetLevelsFunc)(nvn::TextureBuilder *, int);
typedef void (*TextureBuilderSetFormatFunc)(nvn::TextureBuilder *, nvn::Format::Enum);
typedef void (*TextureBuilderSetSamplesFunc)(nvn::TextureBuilder *, int);
struct TextureSwizzle {  // size 0x4
    enum Enum {
        ZERO = 0,
        ONE = 1,
        R = 2,
        G = 3,
        B = 4,
        A = 5,
        NVN_TEXTURE_SWIZZLE_LARGE = 2147483647,
    };
private:
    nvn::TextureSwizzle::Enum m_value;  // 0x00
public:
    TextureSwizzle() {}
    TextureSwizzle(const nvn::TextureSwizzle & other) : m_value(other.m_value) {}
    TextureSwizzle(nvn::TextureSwizzle::Enum e) : m_value(e) {}
    operator nvn::TextureSwizzle::Enum() const { return m_value; }
    bool operator!=(const nvn::TextureSwizzle::Enum &) const;
    bool operator!=(const nvn::TextureSwizzle &) const;
    bool operator==(const nvn::TextureSwizzle::Enum &) const;
    bool operator==(const nvn::TextureSwizzle &) const;
};
static_assert(sizeof(TextureSwizzle) == 0x4, "TextureSwizzle");

typedef void (*TextureBuilderSetSwizzleFunc)(nvn::TextureBuilder *, nvn::TextureSwizzle::Enum, nvn::TextureSwizzle::Enum, nvn::TextureSwizzle::Enum, nvn::TextureSwizzle::Enum);
struct TextureDepthStencilMode {  // size 0x4
    enum Enum {
        DEPTH = 0,
        STENCIL = 1,
        NVN_TEXTURE_DEPTH_STENCIL_MODE_LARGE = 2147483647,
    };
private:
    nvn::TextureDepthStencilMode::Enum m_value;  // 0x00
public:
    TextureDepthStencilMode();
    operator nvn::TextureDepthStencilMode::Enum() const;
    bool operator!=(const nvn::TextureDepthStencilMode::Enum &) const;
    bool operator!=(const nvn::TextureDepthStencilMode &) const;
    bool operator==(const nvn::TextureDepthStencilMode::Enum &) const;
    bool operator==(const nvn::TextureDepthStencilMode &) const;
};
static_assert(sizeof(TextureDepthStencilMode) == 0x4, "TextureDepthStencilMode");

typedef void (*TextureBuilderSetDepthStencilModeFunc)(nvn::TextureBuilder *, nvn::TextureDepthStencilMode::Enum);
typedef size_t (*TextureBuilderGetStorageSizeFunc)(const nvn::TextureBuilder *);
typedef size_t (*TextureBuilderGetStorageAlignmentFunc)(const nvn::TextureBuilder *);
typedef void (*TextureBuilderSetStorageFunc)(nvn::TextureBuilder *, nvn::MemoryPool *, ptrdiff_t);
typedef void (*TextureBuilderSetPackagedTextureDataFunc)(nvn::TextureBuilder *, const void *);
struct PackagedTextureLayout {  // size 0x8
    char layout[8];  // 0x00
};
static_assert(sizeof(PackagedTextureLayout) == 0x8, "PackagedTextureLayout");

typedef void (*TextureBuilderSetPackagedTextureLayoutFunc)(nvn::TextureBuilder *, const nvn::PackagedTextureLayout *);
typedef void (*TextureBuilderSetStrideFunc)(nvn::TextureBuilder *, ptrdiff_t);
typedef void (*TextureBuilderSetGLTextureNameFunc)(nvn::TextureBuilder *, uint32_t);
typedef NVNstorageClass (*TextureBuilderGetStorageClassFunc)(const nvn::TextureBuilder *);
typedef const nvn::Device *(*TextureBuilderGetDeviceFunc)(const nvn::TextureBuilder *);
typedef int (*TextureBuilderGetFlagsFunc)(const nvn::TextureBuilder *);
typedef nvn::TextureTarget::Enum (*TextureBuilderGetTargetFunc)(const nvn::TextureBuilder *);
typedef int (*TextureBuilderGetWidthFunc)(const nvn::TextureBuilder *);
typedef int (*TextureBuilderGetHeightFunc)(const nvn::TextureBuilder *);
typedef int (*TextureBuilderGetDepthFunc)(const nvn::TextureBuilder *);
typedef int (*TextureBuilderGetLevelsFunc)(const nvn::TextureBuilder *);
typedef nvn::Format::Enum (*TextureBuilderGetFormatFunc)(const nvn::TextureBuilder *);
typedef int (*TextureBuilderGetSamplesFunc)(const nvn::TextureBuilder *);
typedef void (*TextureBuilderGetSwizzleFunc)(const nvn::TextureBuilder *, nvn::TextureSwizzle *, nvn::TextureSwizzle *, nvn::TextureSwizzle *, nvn::TextureSwizzle *);
typedef nvn::TextureDepthStencilMode::Enum (*TextureBuilderGetDepthStencilModeFunc)(const nvn::TextureBuilder *);
typedef const void *(*TextureBuilderGetPackagedTextureDataFunc)(const nvn::TextureBuilder *);
typedef NVNboolean (*TextureBuilderGetPackagedTextureLayoutFunc)(const nvn::TextureBuilder *, nvn::PackagedTextureLayout *);
typedef ptrdiff_t (*TextureBuilderGetStrideFunc)(const nvn::TextureBuilder *);
typedef void (*TextureBuilderGetSparseTileLayoutFunc)(const nvn::TextureBuilder *, nvn::TextureSparseTileLayout *);
typedef uint32_t (*TextureBuilderGetGLTextureNameFunc)(const nvn::TextureBuilder *);
typedef size_t (*TextureBuilderGetZCullStorageSizeFunc)(const nvn::TextureBuilder *);
typedef nvn::MemoryPool *(*TextureBuilderGetMemoryPoolFunc)(const nvn::TextureBuilder *);
typedef ptrdiff_t (*TextureBuilderGetMemoryOffsetFunc)(const nvn::TextureBuilder *);
typedef void (*TextureViewSetLevelsFunc)(nvn::TextureView *, int, int);
typedef void (*TextureViewSetFormatFunc)(nvn::TextureView *, nvn::Format::Enum);
typedef void (*TextureViewSetSwizzleFunc)(nvn::TextureView *, nvn::TextureSwizzle::Enum, nvn::TextureSwizzle::Enum, nvn::TextureSwizzle::Enum, nvn::TextureSwizzle::Enum);
typedef void (*TextureViewSetDepthStencilModeFunc)(nvn::TextureView *, nvn::TextureDepthStencilMode::Enum);
typedef NVNboolean (*TextureViewGetLevelsFunc)(const nvn::TextureView *, int *, int *);
typedef NVNboolean (*TextureViewGetLayersFunc)(const nvn::TextureView *, int *, int *);
typedef NVNboolean (*TextureViewGetFormatFunc)(const nvn::TextureView *, nvn::Format *);
typedef NVNboolean (*TextureViewGetSwizzleFunc)(const nvn::TextureView *, nvn::TextureSwizzle *, nvn::TextureSwizzle *, nvn::TextureSwizzle *, nvn::TextureSwizzle *);
typedef NVNboolean (*TextureViewGetDepthStencilModeFunc)(const nvn::TextureView *, nvn::TextureDepthStencilMode *);
typedef NVNboolean (*TextureViewGetTargetFunc)(const nvn::TextureView *, nvn::TextureTarget *);
typedef NVNboolean (*TextureViewCompareFunc)(const nvn::TextureView *, const nvn::TextureView *);
typedef NVNboolean (*TextureInitializeFunc)(nvn::Texture *, const nvn::TextureBuilder *);
typedef size_t (*TextureGetZCullStorageSizeFunc)(const nvn::Texture *);
typedef void (*TextureSetDebugLabelFunc)(nvn::Texture *, const char *);
typedef NVNstorageClass (*TextureGetStorageClassFunc)(const nvn::Texture *);
typedef int (*TextureGetFlagsFunc)(const nvn::Texture *);
typedef nvn::TextureTarget::Enum (*TextureGetTargetFunc)(const nvn::Texture *);
typedef int (*TextureGetWidthFunc)(const nvn::Texture *);
typedef int (*TextureGetHeightFunc)(const nvn::Texture *);
typedef int (*TextureGetDepthFunc)(const nvn::Texture *);
typedef int (*TextureGetLevelsFunc)(const nvn::Texture *);
typedef nvn::Format::Enum (*TextureGetFormatFunc)(const nvn::Texture *);
typedef int (*TextureGetSamplesFunc)(const nvn::Texture *);
typedef void (*TextureGetSwizzleFunc)(const nvn::Texture *, nvn::TextureSwizzle *, nvn::TextureSwizzle *, nvn::TextureSwizzle *, nvn::TextureSwizzle *);
typedef nvn::TextureDepthStencilMode::Enum (*TextureGetDepthStencilModeFunc)(const nvn::Texture *);
typedef nvn::TextureAddress (*TextureGetTextureAddressFunc)(const nvn::Texture *);
typedef void (*TextureGetSparseTileLayoutFunc)(const nvn::Texture *, nvn::TextureSparseTileLayout *);
typedef void (*TextureWriteTexelsFunc)(const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, const void *);
typedef void (*TextureWriteTexelsStridedFunc)(const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, const void *, ptrdiff_t, ptrdiff_t);
typedef void (*TextureReadTexelsFunc)(const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, void *);
typedef void (*TextureReadTexelsStridedFunc)(const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, void *, ptrdiff_t, ptrdiff_t);
typedef void (*TextureInvalidateTexelsFunc)(const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *);
typedef nvn::MemoryPool *(*TextureGetMemoryPoolFunc)(const nvn::Texture *);
typedef ptrdiff_t (*TextureGetMemoryOffsetFunc)(const nvn::Texture *);
typedef int (*TextureGetStorageSizeFunc)(const nvn::Texture *);
typedef NVNboolean (*TextureCompareFunc)(const nvn::Texture *, const nvn::Texture *);
typedef uint64_t (*TextureGetDebugIDFunc)(const nvn::Texture *);
typedef void (*SamplerBuilderSetLodClampFunc)(nvn::SamplerBuilder *, float, float);
struct CompareMode {  // size 0x4
    enum Enum {
        NONE = 0,
        COMPARE_R_TO_TEXTURE = 1,
        NVN_COMPARE_MODE_LARGE = 2147483647,
    };
private:
    nvn::CompareMode::Enum m_value;  // 0x00
public:
    CompareMode();
    operator nvn::CompareMode::Enum() const;
    bool operator!=(const nvn::CompareMode::Enum &) const;
    bool operator!=(const nvn::CompareMode &) const;
    bool operator==(const nvn::CompareMode::Enum &) const;
    bool operator==(const nvn::CompareMode &) const;
};
static_assert(sizeof(CompareMode) == 0x4, "CompareMode");

struct CompareFunc {  // size 0x4
    enum Enum {
        NEVER = 1,
        LESS = 2,
        EQUAL = 3,
        LEQUAL = 4,
        GREATER = 5,
        NOTEQUAL = 6,
        GEQUAL = 7,
        ALWAYS = 8,
        NVN_COMPARE_FUNC_LARGE = 2147483647,
    };
private:
    nvn::CompareFunc::Enum m_value;  // 0x00
public:
    CompareFunc();
    operator nvn::CompareFunc::Enum() const;
    bool operator!=(const nvn::CompareFunc::Enum &) const;
    bool operator!=(const nvn::CompareFunc &) const;
    bool operator==(const nvn::CompareFunc::Enum &) const;
    bool operator==(const nvn::CompareFunc &) const;
};
static_assert(sizeof(CompareFunc) == 0x4, "CompareFunc");

typedef void (*SamplerBuilderSetCompareFunc)(nvn::SamplerBuilder *, nvn::CompareMode::Enum, nvn::CompareFunc::Enum);
typedef void (*SamplerBuilderSetBorderColorFunc)(nvn::SamplerBuilder *, const float *);
typedef void (*SamplerBuilderSetBorderColoriFunc)(nvn::SamplerBuilder *, const int *);
typedef void (*SamplerBuilderSetBorderColoruiFunc)(nvn::SamplerBuilder *, const uint32_t *);
typedef void (*SamplerBuilderSetMaxAnisotropyFunc)(nvn::SamplerBuilder *, float);
struct SamplerReduction {  // size 0x4
    enum Enum {
        AVERAGE = 0,
        MIN = 1,
        MAX = 2,
        NVN_SAMPLER_REDUCTION_LARGE = 2147483647,
    };
private:
    nvn::SamplerReduction::Enum m_value;  // 0x00
public:
    SamplerReduction();
    operator nvn::SamplerReduction::Enum() const;
    bool operator!=(const nvn::SamplerReduction::Enum &) const;
    bool operator!=(const nvn::SamplerReduction &) const;
    bool operator==(const nvn::SamplerReduction::Enum &) const;
    bool operator==(const nvn::SamplerReduction &) const;
};
static_assert(sizeof(SamplerReduction) == 0x4, "SamplerReduction");

typedef void (*SamplerBuilderSetReductionFilterFunc)(nvn::SamplerBuilder *, nvn::SamplerReduction::Enum);
typedef const nvn::Device *(*SamplerBuilderGetDeviceFunc)(const nvn::SamplerBuilder *);
typedef void (*SamplerBuilderGetMinMagFilterFunc)(const nvn::SamplerBuilder *, nvn::MinFilter *, nvn::MagFilter *);
typedef void (*SamplerBuilderGetWrapModeFunc)(const nvn::SamplerBuilder *, nvn::WrapMode *, nvn::WrapMode *, nvn::WrapMode *);
typedef void (*SamplerBuilderGetLodClampFunc)(const nvn::SamplerBuilder *, float *, float *);
typedef float (*SamplerBuilderGetLodBiasFunc)(const nvn::SamplerBuilder *);
typedef void (*SamplerBuilderGetCompareFunc)(const nvn::SamplerBuilder *, nvn::CompareMode *, nvn::CompareFunc *);
typedef void (*SamplerBuilderGetBorderColorFunc)(const nvn::SamplerBuilder *, float *);
typedef void (*SamplerBuilderGetBorderColoriFunc)(const nvn::SamplerBuilder *, int *);
typedef void (*SamplerBuilderGetBorderColoruiFunc)(const nvn::SamplerBuilder *, uint32_t *);
typedef float (*SamplerBuilderGetMaxAnisotropyFunc)(const nvn::SamplerBuilder *);
typedef nvn::SamplerReduction::Enum (*SamplerBuilderGetReductionFilterFunc)(const nvn::SamplerBuilder *);
typedef float (*SamplerBuilderGetLodSnapFunc)(const nvn::SamplerBuilder *);
typedef void (*SamplerFinalizeFunc)(nvn::Sampler *);
typedef void (*SamplerSetDebugLabelFunc)(nvn::Sampler *, const char *);
typedef void (*SamplerGetMinMagFilterFunc)(const nvn::Sampler *, nvn::MinFilter *, nvn::MagFilter *);
typedef void (*SamplerGetWrapModeFunc)(const nvn::Sampler *, nvn::WrapMode *, nvn::WrapMode *, nvn::WrapMode *);
typedef void (*SamplerGetLodClampFunc)(const nvn::Sampler *, float *, float *);
typedef float (*SamplerGetLodBiasFunc)(const nvn::Sampler *);
typedef void (*SamplerGetCompareFunc)(const nvn::Sampler *, nvn::CompareMode *, nvn::CompareFunc *);
typedef void (*SamplerGetBorderColorFunc)(const nvn::Sampler *, float *);
typedef void (*SamplerGetBorderColoriFunc)(const nvn::Sampler *, int *);
typedef void (*SamplerGetBorderColoruiFunc)(const nvn::Sampler *, uint32_t *);
typedef float (*SamplerGetMaxAnisotropyFunc)(const nvn::Sampler *);
typedef nvn::SamplerReduction::Enum (*SamplerGetReductionFilterFunc)(const nvn::Sampler *);
typedef NVNboolean (*SamplerCompareFunc)(const nvn::Sampler *, const nvn::Sampler *);
typedef uint64_t (*SamplerGetDebugIDFunc)(const nvn::Sampler *);
typedef void (*BlendStateSetBlendTargetFunc)(nvn::BlendState *, int);
struct BlendAdvancedMode {  // size 0x4
    enum Enum {
        BLEND_NONE = 0,
        BLEND_ZERO = 5,
        BLEND_SRC = 6,
        BLEND_DST = 7,
        BLEND_SRC_OVER = 8,
        BLEND_DST_OVER = 9,
        BLEND_SRC_IN = 10,
        BLEND_DST_IN = 11,
        BLEND_SRC_OUT = 12,
        BLEND_DST_OUT = 13,
        BLEND_SRC_ATOP = 14,
        BLEND_DST_ATOP = 15,
        BLEND_XOR = 16,
        BLEND_PLUS = 17,
        BLEND_PLUS_CLAMPED = 18,
        BLEND_PLUS_CLAMPED_ALPHA = 19,
        BLEND_PLUS_DARKER = 20,
        BLEND_MULTIPLY = 21,
        BLEND_SCREEN = 22,
        BLEND_OVERLAY = 23,
        BLEND_DARKEN = 24,
        BLEND_LIGHTEN = 25,
        BLEND_COLORDODGE = 26,
        BLEND_COLORBURN = 27,
        BLEND_HARDLIGHT = 28,
        BLEND_SOFTLIGHT = 29,
        BLEND_DIFFERENCE = 30,
        BLEND_MINUS = 31,
        BLEND_MINUS_CLAMPED = 32,
        BLEND_EXCLUSION = 33,
        BLEND_CONTRAST = 34,
        BLEND_INVERT = 35,
        BLEND_INVERT_RGB = 36,
        BLEND_INVERT_OVG = 37,
        BLEND_LINEARDODGE = 38,
        BLEND_LINEARBURN = 39,
        BLEND_VIVIDLIGHT = 40,
        BLEND_LINEARLIGHT = 41,
        BLEND_PINLIGHT = 42,
        BLEND_HARDMIX = 43,
        BLEND_RED = 44,
        BLEND_GREEN = 45,
        BLEND_BLUE = 46,
        BLEND_HSL_HUE = 47,
        BLEND_HSL_SATURATION = 48,
        BLEND_HSL_COLOR = 49,
        BLEND_HSL_LUMINOSITY = 50,
        NVN_BLEND_ADVANCED_MODE_LARGE = 2147483647,
    };
private:
    nvn::BlendAdvancedMode::Enum m_value;  // 0x00
public:
    BlendAdvancedMode();
    operator nvn::BlendAdvancedMode::Enum() const;
    bool operator!=(const nvn::BlendAdvancedMode::Enum &) const;
    bool operator!=(const nvn::BlendAdvancedMode &) const;
    bool operator==(const nvn::BlendAdvancedMode::Enum &) const;
    bool operator==(const nvn::BlendAdvancedMode &) const;
};
static_assert(sizeof(BlendAdvancedMode) == 0x4, "BlendAdvancedMode");

typedef void (*BlendStateSetAdvancedModeFunc)(nvn::BlendState *, nvn::BlendAdvancedMode::Enum);
struct BlendAdvancedOverlap {  // size 0x4
    enum Enum {
        UNCORRELATED = 0,
        DISJOINT = 1,
        CONJOINT = 2,
        NVN_BLEND_ADVANCED_OVERLAP_LARGE = 2147483647,
    };
private:
    nvn::BlendAdvancedOverlap::Enum m_value;  // 0x00
public:
    BlendAdvancedOverlap();
    operator nvn::BlendAdvancedOverlap::Enum() const;
    bool operator!=(const nvn::BlendAdvancedOverlap::Enum &) const;
    bool operator!=(const nvn::BlendAdvancedOverlap &) const;
    bool operator==(const nvn::BlendAdvancedOverlap::Enum &) const;
    bool operator==(const nvn::BlendAdvancedOverlap &) const;
};
static_assert(sizeof(BlendAdvancedOverlap) == 0x4, "BlendAdvancedOverlap");

typedef void (*BlendStateSetAdvancedOverlapFunc)(nvn::BlendState *, nvn::BlendAdvancedOverlap::Enum);
typedef void (*BlendStateSetAdvancedPremultipliedSrcFunc)(nvn::BlendState *, NVNboolean);
typedef void (*BlendStateSetAdvancedNormalizedDstFunc)(nvn::BlendState *, NVNboolean);
typedef int (*BlendStateGetBlendTargetFunc)(const nvn::BlendState *);
typedef void (*BlendStateGetBlendFuncFunc)(const nvn::BlendState *, nvn::BlendFunc *, nvn::BlendFunc *, nvn::BlendFunc *, nvn::BlendFunc *);
typedef void (*BlendStateGetBlendEquationFunc)(const nvn::BlendState *, nvn::BlendEquation *, nvn::BlendEquation *);
typedef nvn::BlendAdvancedMode::Enum (*BlendStateGetAdvancedModeFunc)(const nvn::BlendState *);
typedef nvn::BlendAdvancedOverlap::Enum (*BlendStateGetAdvancedOverlapFunc)(const nvn::BlendState *);
typedef NVNboolean (*BlendStateGetAdvancedPremultipliedSrcFunc)(const nvn::BlendState *);
typedef NVNboolean (*BlendStateGetAdvancedNormalizedDstFunc)(const nvn::BlendState *);
struct LogicOp {  // size 0x4
    enum Enum {
        CLEAR = 0,
        AND = 1,
        AND_REVERSE = 2,
        COPY = 3,
        AND_INVERTED = 4,
        NOOP = 5,
        XOR = 6,
        OR = 7,
        NOR = 8,
        EQUIV = 9,
        INVERT = 10,
        OR_REVERSE = 11,
        COPY_INVERTED = 12,
        OR_INVERTED = 13,
        NAND = 14,
        SET = 15,
        NVN_LOGIC_OP_LARGE = 2147483647,
    };
private:
    nvn::LogicOp::Enum m_value;  // 0x00
public:
    LogicOp();
    operator nvn::LogicOp::Enum() const;
    bool operator!=(const nvn::LogicOp::Enum &) const;
    bool operator!=(const nvn::LogicOp &) const;
    bool operator==(const nvn::LogicOp::Enum &) const;
    bool operator==(const nvn::LogicOp &) const;
};
static_assert(sizeof(LogicOp) == 0x4, "LogicOp");

typedef void (*ColorStateSetLogicOpFunc)(nvn::ColorState *, nvn::LogicOp::Enum);
struct AlphaFunc {  // size 0x4
    enum Enum {
        NEVER = 1,
        LESS = 2,
        EQUAL = 3,
        LEQUAL = 4,
        GREATER = 5,
        NOTEQUAL = 6,
        GEQUAL = 7,
        ALWAYS = 8,
        NVN_ALPHA_FUNC_LARGE = 2147483647,
    };
private:
    nvn::AlphaFunc::Enum m_value;  // 0x00
public:
    AlphaFunc();
    operator nvn::AlphaFunc::Enum() const;
    bool operator!=(const nvn::AlphaFunc::Enum &) const;
    bool operator!=(const nvn::AlphaFunc &) const;
    bool operator==(const nvn::AlphaFunc::Enum &) const;
    bool operator==(const nvn::AlphaFunc &) const;
};
static_assert(sizeof(AlphaFunc) == 0x4, "AlphaFunc");

typedef void (*ColorStateSetAlphaTestFunc)(nvn::ColorState *, nvn::AlphaFunc::Enum);
typedef NVNboolean (*ColorStateGetBlendEnableFunc)(const nvn::ColorState *, int);
typedef nvn::LogicOp::Enum (*ColorStateGetLogicOpFunc)(const nvn::ColorState *);
typedef nvn::AlphaFunc::Enum (*ColorStateGetAlphaTestFunc)(const nvn::ColorState *);
typedef void (*ChannelMaskStateGetChannelMaskFunc)(const nvn::ChannelMaskState *, int, NVNboolean *, NVNboolean *, NVNboolean *, NVNboolean *);
typedef void (*MultisampleStateSetDefaultsFunc)(nvn::MultisampleState *);
typedef void (*MultisampleStateSetMultisampleEnableFunc)(nvn::MultisampleState *, NVNboolean);
typedef void (*MultisampleStateSetSamplesFunc)(nvn::MultisampleState *, int);
typedef void (*MultisampleStateSetAlphaToCoverageEnableFunc)(nvn::MultisampleState *, NVNboolean);
typedef void (*MultisampleStateSetAlphaToCoverageDitherFunc)(nvn::MultisampleState *, NVNboolean);
typedef NVNboolean (*MultisampleStateGetMultisampleEnableFunc)(const nvn::MultisampleState *);
typedef int (*MultisampleStateGetSamplesFunc)(const nvn::MultisampleState *);
typedef NVNboolean (*MultisampleStateGetAlphaToCoverageEnableFunc)(const nvn::MultisampleState *);
typedef NVNboolean (*MultisampleStateGetAlphaToCoverageDitherFunc)(const nvn::MultisampleState *);
typedef void (*MultisampleStateSetRasterSamplesFunc)(nvn::MultisampleState *, int);
typedef int (*MultisampleStateGetRasterSamplesFunc)(nvn::MultisampleState *);
struct CoverageModulationMode {  // size 0x4
    enum Enum {
        NONE = 0,
        RGB = 1,
        ALPHA = 2,
        RGBA = 3,
        NVN_COVERAGE_MODULATION_MODE_LARGE = 2147483647,
    };
private:
    nvn::CoverageModulationMode::Enum m_value;  // 0x00
public:
    CoverageModulationMode();
    operator nvn::CoverageModulationMode::Enum() const;
    bool operator!=(const nvn::CoverageModulationMode::Enum &) const;
    bool operator!=(const nvn::CoverageModulationMode &) const;
    bool operator==(const nvn::CoverageModulationMode::Enum &) const;
    bool operator==(const nvn::CoverageModulationMode &) const;
};
static_assert(sizeof(CoverageModulationMode) == 0x4, "CoverageModulationMode");

typedef void (*MultisampleStateSetCoverageModulationModeFunc)(nvn::MultisampleState *, nvn::CoverageModulationMode::Enum);
typedef nvn::CoverageModulationMode::Enum (*MultisampleStateGetCoverageModulationModeFunc)(const nvn::MultisampleState *);
typedef void (*MultisampleStateSetCoverageToColorEnableFunc)(nvn::MultisampleState *, NVNboolean);
typedef NVNboolean (*MultisampleStateGetCoverageToColorEnableFunc)(const nvn::MultisampleState *);
typedef void (*MultisampleStateSetCoverageToColorOutputFunc)(nvn::MultisampleState *, int);
typedef int (*MultisampleStateGetCoverageToColorOutputFunc)(const nvn::MultisampleState *);
typedef void (*MultisampleStateSetSampleLocationsEnableFunc)(nvn::MultisampleState *, NVNboolean);
typedef NVNboolean (*MultisampleStateGetSampleLocationsEnableFunc)(const nvn::MultisampleState *);
typedef void (*MultisampleStateGetSampleLocationsGridFunc)(nvn::MultisampleState *, int *, int *);
typedef void (*MultisampleStateSetSampleLocationsGridEnableFunc)(nvn::MultisampleState *, NVNboolean);
typedef NVNboolean (*MultisampleStateGetSampleLocationsGridEnableFunc)(const nvn::MultisampleState *);
typedef void (*MultisampleStateSetSampleLocationsFunc)(nvn::MultisampleState *, int, int, const float *);
typedef void (*PolygonStateSetPolygonOffsetEnablesFunc)(nvn::PolygonState *, int);
typedef nvn::Face::Enum (*PolygonStateGetCullFaceFunc)(const nvn::PolygonState *);
typedef nvn::FrontFace::Enum (*PolygonStateGetFrontFaceFunc)(const nvn::PolygonState *);
typedef nvn::PolygonMode::Enum (*PolygonStateGetPolygonModeFunc)(const nvn::PolygonState *);
typedef int (*PolygonStateGetPolygonOffsetEnablesFunc)(const nvn::PolygonState *);
typedef void (*DepthStencilStateSetDefaultsFunc)(nvn::DepthStencilState *);
typedef NVNboolean (*DepthStencilStateGetDepthTestEnableFunc)(const nvn::DepthStencilState *);
typedef NVNboolean (*DepthStencilStateGetDepthWriteEnableFunc)(const nvn::DepthStencilState *);
typedef nvn::DepthFunc::Enum (*DepthStencilStateGetDepthFuncFunc)(const nvn::DepthStencilState *);
typedef NVNboolean (*DepthStencilStateGetStencilTestEnableFunc)(const nvn::DepthStencilState *);
typedef nvn::StencilFunc::Enum (*DepthStencilStateGetStencilFuncFunc)(const nvn::DepthStencilState *, nvn::Face::Enum);
typedef void (*DepthStencilStateGetStencilOpFunc)(const nvn::DepthStencilState *, nvn::Face::Enum, nvn::StencilOp *, nvn::StencilOp *, nvn::StencilOp *);
typedef void (*VertexAttribStateGetFormatFunc)(const nvn::VertexAttribState *, nvn::Format *, ptrdiff_t *);
typedef int (*VertexAttribStateGetStreamIndexFunc)(const nvn::VertexAttribState *);
typedef void (*VertexStreamStateSetStrideFunc)(nvn::VertexStreamState *, ptrdiff_t);
typedef void (*VertexStreamStateSetDivisorFunc)(nvn::VertexStreamState *, int);
typedef ptrdiff_t (*VertexStreamStateGetStrideFunc)(const nvn::VertexStreamState *);
typedef int (*VertexStreamStateGetDivisorFunc)(const nvn::VertexStreamState *);
typedef NVNboolean (*CommandBufferInitializeFunc)(nvn::CommandBuffer *, nvn::Device *);
typedef void (*CommandBufferFinalizeFunc)(nvn::CommandBuffer *);
typedef void (*CommandBufferSetDebugLabelFunc)(nvn::CommandBuffer *, const char *);
typedef void (*CommandBufferSetMemoryCallbackFunc)(nvn::CommandBuffer *, nvn::CommandBufferMemoryCallbackFunc);
typedef void (*CommandBufferSetMemoryCallbackDataFunc)(nvn::CommandBuffer *, void *);
typedef void (*CommandBufferAddCommandMemoryFunc)(nvn::CommandBuffer *, const nvn::MemoryPool *, ptrdiff_t, size_t);
typedef void (*CommandBufferAddControlMemoryFunc)(nvn::CommandBuffer *, void *, size_t);
typedef size_t (*CommandBufferGetCommandMemorySizeFunc)(const nvn::CommandBuffer *);
typedef size_t (*CommandBufferGetCommandMemoryUsedFunc)(const nvn::CommandBuffer *);
typedef size_t (*CommandBufferGetCommandMemoryFreeFunc)(const nvn::CommandBuffer *);
typedef size_t (*CommandBufferGetControlMemorySizeFunc)(const nvn::CommandBuffer *);
typedef size_t (*CommandBufferGetControlMemoryUsedFunc)(const nvn::CommandBuffer *);
typedef size_t (*CommandBufferGetControlMemoryFreeFunc)(const nvn::CommandBuffer *);
typedef void (*CommandBufferBeginRecordingFunc)(nvn::CommandBuffer *);
typedef void (*CommandBufferCallCommandsFunc)(nvn::CommandBuffer *, int, const nvn::CommandHandle *);
typedef void (*CommandBufferCopyCommandsFunc)(nvn::CommandBuffer *, int, const nvn::CommandHandle *);
typedef void (*CommandBufferBindMultisampleStateFunc)(nvn::CommandBuffer *, const nvn::MultisampleState *);
typedef void (*CommandBufferBindVertexBufferFunc)(nvn::CommandBuffer *, int, nvn::BufferAddress, size_t);
typedef void (*CommandBufferBindUniformBufferFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, nvn::BufferAddress, size_t);
typedef void (*CommandBufferBindUniformBuffersFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, int, const nvn::BufferRange *);
typedef void (*CommandBufferBindTransformFeedbackBufferFunc)(nvn::CommandBuffer *, int, nvn::BufferAddress, size_t);
typedef void (*CommandBufferBindTransformFeedbackBuffersFunc)(nvn::CommandBuffer *, int, int, const nvn::BufferRange *);
typedef void (*CommandBufferBindStorageBufferFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, nvn::BufferAddress, size_t);
typedef void (*CommandBufferBindStorageBuffersFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, int, const nvn::BufferRange *);
typedef void (*CommandBufferBindTexturesFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, int, const nvn::TextureHandle *);
typedef void (*CommandBufferBindImageFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, nvn::ImageHandle);
typedef void (*CommandBufferBindImagesFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, int, const nvn::ImageHandle *);
typedef void (*CommandBufferSetPatchSizeFunc)(nvn::CommandBuffer *, int);
typedef void (*CommandBufferSetInnerTessellationLevelsFunc)(nvn::CommandBuffer *, const float *);
typedef void (*CommandBufferSetOuterTessellationLevelsFunc)(nvn::CommandBuffer *, const float *);
typedef void (*CommandBufferSetPrimitiveRestartFunc)(nvn::CommandBuffer *, NVNboolean, int);
typedef void (*CommandBufferBeginTransformFeedbackFunc)(nvn::CommandBuffer *, nvn::BufferAddress);
typedef void (*CommandBufferEndTransformFeedbackFunc)(nvn::CommandBuffer *, nvn::BufferAddress);
typedef void (*CommandBufferPauseTransformFeedbackFunc)(nvn::CommandBuffer *, nvn::BufferAddress);
typedef void (*CommandBufferResumeTransformFeedbackFunc)(nvn::CommandBuffer *, nvn::BufferAddress);
typedef void (*CommandBufferDrawTransformFeedbackFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::BufferAddress);
typedef void (*CommandBufferDrawElementsFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::IndexType::Enum, int, nvn::BufferAddress);
typedef void (*CommandBufferDrawArraysInstancedFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, int, int, int, int);
typedef void (*CommandBufferDrawElementsInstancedFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::IndexType::Enum, int, nvn::BufferAddress, int, int, int);
typedef void (*CommandBufferDrawArraysIndirectFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::BufferAddress);
typedef void (*CommandBufferDrawElementsIndirectFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::IndexType::Enum, nvn::BufferAddress, nvn::BufferAddress);
typedef void (*CommandBufferMultiDrawArraysIndirectCountFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::BufferAddress, nvn::BufferAddress, int, ptrdiff_t);
typedef void (*CommandBufferMultiDrawElementsIndirectCountFunc)(nvn::CommandBuffer *, nvn::DrawPrimitive::Enum, nvn::IndexType::Enum, nvn::BufferAddress, nvn::BufferAddress, nvn::BufferAddress, int, ptrdiff_t);
typedef void (*CommandBufferClearColoriFunc)(nvn::CommandBuffer *, int, const int *, int);
typedef void (*CommandBufferClearColoruiFunc)(nvn::CommandBuffer *, int, const uint32_t *, int);
typedef void (*CommandBufferDispatchComputeFunc)(nvn::CommandBuffer *, int, int, int);
typedef void (*CommandBufferDispatchComputeIndirectFunc)(nvn::CommandBuffer *, nvn::BufferAddress);
typedef void (*CommandBufferSetViewportsFunc)(nvn::CommandBuffer *, int, int, const float *);
struct ViewportSwizzle {  // size 0x4
    enum Enum {
        POSITIVE_X = 0,
        NEGATIVE_X = 1,
        POSITIVE_Y = 2,
        NEGATIVE_Y = 3,
        POSITIVE_Z = 4,
        NEGATIVE_Z = 5,
        POSITIVE_W = 6,
        NEGATIVE_W = 7,
        NVN_VIEWPORT_SWIZZLE_LARGE = 2147483647,
    };
private:
    nvn::ViewportSwizzle::Enum m_value;  // 0x00
public:
    ViewportSwizzle();
    operator nvn::ViewportSwizzle::Enum() const;
    bool operator!=(const nvn::ViewportSwizzle::Enum &) const;
    bool operator!=(const nvn::ViewportSwizzle &) const;
    bool operator==(const nvn::ViewportSwizzle::Enum &) const;
    bool operator==(const nvn::ViewportSwizzle &) const;
};
static_assert(sizeof(ViewportSwizzle) == 0x4, "ViewportSwizzle");

typedef void (*CommandBufferSetViewportSwizzlesFunc)(nvn::CommandBuffer *, int, int, const nvn::ViewportSwizzle *);
typedef void (*CommandBufferSetScissorFunc)(nvn::CommandBuffer *, int, int, int, int);
typedef void (*CommandBufferSetScissorsFunc)(nvn::CommandBuffer *, int, int, const int *);
typedef void (*CommandBufferSetDepthBoundsFunc)(nvn::CommandBuffer *, NVNboolean, float, float);
typedef void (*CommandBufferSetDepthRangesFunc)(nvn::CommandBuffer *, int, int, const float *);
struct TiledCacheAction {  // size 0x4
    enum Enum {
        ENABLE = 1,
        DISABLE = 2,
        FLUSH = 3,
        FLUSH_NO_TILING = 4,
        ENABLE_RENDER_TARGET_BINNING = 5,
        DISABLE_RENDER_TARGET_BINNING = 6,
        NVN_TILED_CACHE_ACTION_LARGE = 2147483647,
    };
private:
    nvn::TiledCacheAction::Enum m_value;  // 0x00
public:
    TiledCacheAction();
    operator nvn::TiledCacheAction::Enum() const;
    bool operator!=(const nvn::TiledCacheAction::Enum &) const;
    bool operator!=(const nvn::TiledCacheAction &) const;
    bool operator==(const nvn::TiledCacheAction::Enum &) const;
    bool operator==(const nvn::TiledCacheAction &) const;
};
static_assert(sizeof(TiledCacheAction) == 0x4, "TiledCacheAction");

typedef void (*CommandBufferSetTiledCacheActionFunc)(nvn::CommandBuffer *, nvn::TiledCacheAction::Enum);
typedef void (*CommandBufferSetTiledCacheTileSizeFunc)(nvn::CommandBuffer *, int, int);
typedef void (*CommandBufferBindSeparateTextureFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, nvn::SeparateTextureHandle);
typedef void (*CommandBufferBindSeparateSamplerFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, nvn::SeparateSamplerHandle);
typedef void (*CommandBufferBindSeparateTexturesFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, int, const nvn::SeparateTextureHandle *);
typedef void (*CommandBufferBindSeparateSamplersFunc)(nvn::CommandBuffer *, nvn::ShaderStage::Enum, int, int, const nvn::SeparateSamplerHandle *);
typedef void (*CommandBufferSetStencilValueMaskFunc)(nvn::CommandBuffer *, nvn::Face::Enum, int);
typedef void (*CommandBufferSetStencilMaskFunc)(nvn::CommandBuffer *, nvn::Face::Enum, int);
typedef void (*CommandBufferSetPointSizeFunc)(nvn::CommandBuffer *, float);
typedef void (*CommandBufferSetLineWidthFunc)(nvn::CommandBuffer *, float);
typedef void (*CommandBufferSetPolygonOffsetClampFunc)(nvn::CommandBuffer *, float, float, float);
typedef void (*CommandBufferSetAlphaRefFunc)(nvn::CommandBuffer *, float);
typedef void (*CommandBufferSetSampleMaskFunc)(nvn::CommandBuffer *, int);
typedef void (*CommandBufferSetRasterizerDiscardFunc)(nvn::CommandBuffer *, NVNboolean);
typedef void (*CommandBufferSetDepthClampFunc)(nvn::CommandBuffer *, NVNboolean);
typedef void (*CommandBufferSetConservativeRasterEnableFunc)(nvn::CommandBuffer *, NVNboolean);
typedef void (*CommandBufferSetConservativeRasterDilateFunc)(nvn::CommandBuffer *, float);
typedef void (*CommandBufferSetSubpixelPrecisionBiasFunc)(nvn::CommandBuffer *, int, int);
typedef void (*CommandBufferCopyBufferToTextureFunc)(nvn::CommandBuffer *, nvn::BufferAddress, const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, int);
typedef void (*CommandBufferCopyTextureToBufferFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, nvn::BufferAddress, int);
typedef void (*CommandBufferCopyTextureToTextureFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, int);
typedef void (*CommandBufferCopyBufferToBufferFunc)(nvn::CommandBuffer *, nvn::BufferAddress, nvn::BufferAddress, size_t, int);
typedef void (*CommandBufferClearBufferFunc)(nvn::CommandBuffer *, nvn::BufferAddress, size_t, uint32_t);
typedef void (*CommandBufferClearTextureFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, const float *, int);
typedef void (*CommandBufferClearTextureiFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, const int *, int);
typedef void (*CommandBufferClearTextureuiFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::TextureView *, const nvn::CopyRegion *, const uint32_t *, int);
typedef void (*CommandBufferReportValueFunc)(nvn::CommandBuffer *, uint32_t, nvn::BufferAddress);
typedef void (*CommandBufferSetRenderEnableFunc)(nvn::CommandBuffer *, NVNboolean);
struct ConditionalRenderMode {  // size 0x4
    enum Enum {
        RENDER_IF_EQUAL = 0,
        RENDER_IF_NOT_EQUAL = 1,
        NVN_CONDITIONAL_RENDER_MODE_LARGE = 2147483647,
    };
private:
    nvn::ConditionalRenderMode::Enum m_value;  // 0x00
public:
    ConditionalRenderMode();
    operator nvn::ConditionalRenderMode::Enum() const;
    bool operator!=(const nvn::ConditionalRenderMode::Enum &) const;
    bool operator!=(const nvn::ConditionalRenderMode &) const;
    bool operator==(const nvn::ConditionalRenderMode::Enum &) const;
    bool operator==(const nvn::ConditionalRenderMode &) const;
};
static_assert(sizeof(ConditionalRenderMode) == 0x4, "ConditionalRenderMode");

typedef void (*CommandBufferSetRenderEnableConditionalFunc)(nvn::CommandBuffer *, nvn::ConditionalRenderMode::Enum, nvn::BufferAddress);
typedef void (*CommandBufferDiscardColorFunc)(nvn::CommandBuffer *, int);
typedef void (*CommandBufferDiscardDepthStencilFunc)(nvn::CommandBuffer *);
typedef void (*CommandBufferDownsampleFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::Texture *);
typedef void (*CommandBufferTiledDownsampleFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::Texture *);
typedef void (*CommandBufferDownsampleTextureViewFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::TextureView *, const nvn::Texture *, const nvn::TextureView *);
typedef void (*CommandBufferTiledDownsampleTextureViewFunc)(nvn::CommandBuffer *, const nvn::Texture *, const nvn::TextureView *, const nvn::Texture *, const nvn::TextureView *);
struct SyncCondition {  // size 0x4
    enum Enum {
        ALL_GPU_COMMANDS_COMPLETE = 0,
        GRAPHICS_WORLD_SPACE_COMPLETE = 1,
        NVN_SYNC_CONDITION_LARGE = 2147483647,
    };
private:
    nvn::SyncCondition::Enum m_value;  // 0x00
public:
    SyncCondition();
    operator nvn::SyncCondition::Enum() const;
    bool operator!=(const nvn::SyncCondition::Enum &) const;
    bool operator!=(const nvn::SyncCondition &) const;
    bool operator==(const nvn::SyncCondition::Enum &) const;
    bool operator==(const nvn::SyncCondition &) const;
};
static_assert(sizeof(SyncCondition) == 0x4, "SyncCondition");

typedef void (*CommandBufferFenceSyncFunc)(nvn::CommandBuffer *, nvn::Sync *, nvn::SyncCondition::Enum, int);
typedef void (*CommandBufferSetTexturePoolFunc)(nvn::CommandBuffer *, const nvn::TexturePool *);
typedef void (*CommandBufferSetSamplerPoolFunc)(nvn::CommandBuffer *, const nvn::SamplerPool *);
typedef void (*CommandBufferSetShaderScratchMemoryFunc)(nvn::CommandBuffer *, const nvn::MemoryPool *, ptrdiff_t, size_t);
typedef void (*CommandBufferSaveZCullDataFunc)(nvn::CommandBuffer *, nvn::BufferAddress, size_t);
typedef void (*CommandBufferRestoreZCullDataFunc)(nvn::CommandBuffer *, nvn::BufferAddress, size_t);
typedef void (*CommandBufferSetCopyRowStrideFunc)(nvn::CommandBuffer *, ptrdiff_t);
typedef void (*CommandBufferSetCopyImageStrideFunc)(nvn::CommandBuffer *, ptrdiff_t);
typedef ptrdiff_t (*CommandBufferGetCopyRowStrideFunc)(const nvn::CommandBuffer *);
typedef ptrdiff_t (*CommandBufferGetCopyImageStrideFunc)(const nvn::CommandBuffer *);
typedef void (*CommandBufferDrawTextureFunc)(nvn::CommandBuffer *, nvn::TextureHandle, const nvn::DrawTextureRegion *, const nvn::DrawTextureRegion *);
typedef NVNboolean (*ProgramSetSubroutineLinkageFunc)(nvn::Program *, int, const NVNsubroutineLinkageMapPtr *);
typedef void (*CommandBufferSetProgramSubroutinesFunc)(nvn::CommandBuffer *, nvn::Program *, nvn::ShaderStage::Enum, const int, const int, const int *);
typedef void (*CommandBufferBindCoverageModulationTableFunc)(nvn::CommandBuffer *, const float *);
typedef void (*CommandBufferResolveDepthBufferFunc)(nvn::CommandBuffer *);
typedef void (*CommandBufferSetColorReductionEnableFunc)(nvn::CommandBuffer *, NVNboolean);
struct FormatClass {  // size 0x4
    enum Enum {
        UNORM8 = 0,
        UNORM10 = 1,
        UNORM16 = 2,
        FP11 = 3,
        FP16 = 4,
        SRGB8 = 5,
        NVN_FORMAT_CLASS_LARGE = 2147483647,
    };
private:
    nvn::FormatClass::Enum m_value;  // 0x00
public:
    FormatClass();
    operator nvn::FormatClass::Enum() const;
    bool operator!=(const nvn::FormatClass::Enum &) const;
    bool operator!=(const nvn::FormatClass &) const;
    bool operator==(const nvn::FormatClass::Enum &) const;
    bool operator==(const nvn::FormatClass &) const;
};
static_assert(sizeof(FormatClass) == 0x4, "FormatClass");

typedef void (*CommandBufferSetColorReductionThresholdsFunc)(nvn::CommandBuffer *, nvn::FormatClass::Enum, int, int);
typedef void (*CommandBufferPushDebugGroupStaticFunc)(nvn::CommandBuffer *, uint32_t, const char *);
typedef void (*CommandBufferPushDebugGroupDynamicFunc)(nvn::CommandBuffer *, uint32_t, const char *);
typedef void (*CommandBufferPushDebugGroupFunc)(nvn::CommandBuffer *, const char *);
typedef void (*CommandBufferPopDebugGroupFunc)(nvn::CommandBuffer *);
typedef void (*CommandBufferPopDebugGroupIdFunc)(nvn::CommandBuffer *, uint32_t);
typedef void (*CommandBufferInsertDebugMarkerStaticFunc)(nvn::CommandBuffer *, uint32_t, const char *);
typedef void (*CommandBufferInsertDebugMarkerDynamicFunc)(nvn::CommandBuffer *, uint32_t, const char *);
typedef void (*CommandBufferInsertDebugMarkerFunc)(nvn::CommandBuffer *, const char *);
typedef nvn::CommandBufferMemoryCallbackFunc (*CommandBufferGetMemoryCallbackFunc)(const nvn::CommandBuffer *);
typedef void *(*CommandBufferGetMemoryCallbackDataFunc)(const nvn::CommandBuffer *);
typedef NVNboolean (*CommandBufferIsRecordingFunc)(const nvn::CommandBuffer *);
typedef NVNboolean (*SyncInitializeFunc)(nvn::Sync *, nvn::Device *);
typedef void (*SyncFinalizeFunc)(nvn::Sync *);
typedef void (*SyncSetDebugLabelFunc)(nvn::Sync *, const char *);
typedef void (*QueueFenceSyncFunc)(nvn::Queue *, nvn::Sync *, nvn::SyncCondition::Enum, int);
struct SyncWaitResult {  // size 0x4
    enum Enum {
        ALREADY_SIGNALED = 0,
        CONDITION_SATISFIED = 1,
        TIMEOUT_EXPIRED = 2,
        FAILED = 3,
        NVN_SYNC_WAIT_RESULT_LARGE = 2147483647,
    };
private:
    nvn::SyncWaitResult::Enum m_value;  // 0x00
public:
    SyncWaitResult();
    operator nvn::SyncWaitResult::Enum() const;
    bool operator!=(const nvn::SyncWaitResult::Enum &) const;
    bool operator!=(const nvn::SyncWaitResult &) const;
    bool operator==(const nvn::SyncWaitResult::Enum &) const;
    bool operator==(const nvn::SyncWaitResult &) const;
};
static_assert(sizeof(SyncWaitResult) == 0x4, "SyncWaitResult");

typedef nvn::SyncWaitResult::Enum (*SyncWaitFunc)(const nvn::Sync *, uint64_t);
typedef NVNboolean (*QueueWaitSyncFunc)(nvn::Queue *, const nvn::Sync *);
typedef void (*EventBuilderSetDefaultsFunc)(nvn::EventBuilder *);
typedef void (*EventBuilderSetStorageFunc)(nvn::EventBuilder *, const nvn::MemoryPool *, int64_t);
typedef const nvn::MemoryPool *(*EventBuilderGetStorageFunc)(const nvn::EventBuilder *, int64_t *);
typedef NVNboolean (*EventInitializeFunc)(nvn::Event *, const nvn::EventBuilder *);
typedef void (*EventFinalizeFunc)(nvn::Event *);
typedef uint32_t (*EventGetValueFunc)(const nvn::Event *);
typedef void (*EventSignalFunc)(nvn::Event *, nvn::EventSignalMode::Enum, uint32_t);
struct EventWaitMode {  // size 0x4
    enum Enum {
        EQUAL = 0,
        GEQUAL_WRAP = 1,
        NVN_EVENT_WAIT_MODE_LARGE = 2147483647,
    };
private:
    nvn::EventWaitMode::Enum m_value;  // 0x00
public:
    EventWaitMode();
    operator nvn::EventWaitMode::Enum() const;
    bool operator!=(const nvn::EventWaitMode::Enum &) const;
    bool operator!=(const nvn::EventWaitMode &) const;
    bool operator==(const nvn::EventWaitMode::Enum &) const;
    bool operator==(const nvn::EventWaitMode &) const;
};
static_assert(sizeof(EventWaitMode) == 0x4, "EventWaitMode");

typedef void (*CommandBufferWaitEventFunc)(nvn::CommandBuffer *, const nvn::Event *, nvn::EventWaitMode::Enum, uint32_t);
extern nvn::DeviceBuilderSetDefaultsFunc pfncpp_nvnDeviceBuilderSetDefaults;
extern nvn::DeviceBuilderSetFlagsFunc pfncpp_nvnDeviceBuilderSetFlags;
extern nvn::DeviceBuilderGetFlagsFunc pfncpp_nvnDeviceBuilderGetFlags;
extern nvn::DeviceInitializeFunc pfncpp_nvnDeviceInitialize;
extern nvn::DeviceFinalizeFunc pfncpp_nvnDeviceFinalize;
extern nvn::DeviceSetDebugLabelFunc pfncpp_nvnDeviceSetDebugLabel;
extern nvn::DeviceGetProcAddressFunc pfncpp_nvnDeviceGetProcAddress;
extern nvn::DeviceGetIntegerFunc pfncpp_nvnDeviceGetInteger;
extern nvn::DeviceGetCurrentTimestampInNanosecondsFunc pfncpp_nvnDeviceGetCurrentTimestampInNanoseconds;
extern nvn::DeviceSetIntermediateShaderCacheFunc pfncpp_nvnDeviceSetIntermediateShaderCache;
extern nvn::DeviceGetTexelFetchHandleFunc pfncpp_nvnDeviceGetTexelFetchHandle;
extern nvn::DeviceGetImageHandleFunc pfncpp_nvnDeviceGetImageHandle;
extern nvn::DeviceInstallDebugCallbackFunc pfncpp_nvnDeviceInstallDebugCallback;
extern nvn::DeviceGenerateDebugDomainIdFunc pfncpp_nvnDeviceGenerateDebugDomainId;
extern nvn::DeviceSetWindowOriginModeFunc pfncpp_nvnDeviceSetWindowOriginMode;
extern nvn::DeviceSetDepthModeFunc pfncpp_nvnDeviceSetDepthMode;
extern nvn::DeviceRegisterFastClearColorFunc pfncpp_nvnDeviceRegisterFastClearColor;
extern nvn::DeviceRegisterFastClearColoriFunc pfncpp_nvnDeviceRegisterFastClearColori;
extern nvn::DeviceRegisterFastClearColoruiFunc pfncpp_nvnDeviceRegisterFastClearColorui;
extern nvn::DeviceRegisterFastClearDepthFunc pfncpp_nvnDeviceRegisterFastClearDepth;
extern nvn::DeviceGetWindowOriginModeFunc pfncpp_nvnDeviceGetWindowOriginMode;
extern nvn::DeviceGetDepthModeFunc pfncpp_nvnDeviceGetDepthMode;
extern nvn::DeviceGetTimestampInNanosecondsFunc pfncpp_nvnDeviceGetTimestampInNanoseconds;
extern nvn::DeviceApplyDeferredFinalizesFunc pfncpp_nvnDeviceApplyDeferredFinalizes;
extern nvn::DeviceFinalizeCommandHandleFunc pfncpp_nvnDeviceFinalizeCommandHandle;
extern nvn::DeviceWalkDebugDatabaseFunc pfncpp_nvnDeviceWalkDebugDatabase;
extern nvn::DeviceGetSeparateTextureHandleFunc pfncpp_nvnDeviceGetSeparateTextureHandle;
extern nvn::DeviceGetSeparateSamplerHandleFunc pfncpp_nvnDeviceGetSeparateSamplerHandle;
extern nvn::DeviceIsExternalDebuggerAttachedFunc pfncpp_nvnDeviceIsExternalDebuggerAttached;
extern nvn::QueueGetErrorFunc pfncpp_nvnQueueGetError;
extern nvn::QueueGetTotalCommandMemoryUsedFunc pfncpp_nvnQueueGetTotalCommandMemoryUsed;
extern nvn::QueueGetTotalControlMemoryUsedFunc pfncpp_nvnQueueGetTotalControlMemoryUsed;
extern nvn::QueueGetTotalComputeMemoryUsedFunc pfncpp_nvnQueueGetTotalComputeMemoryUsed;
extern nvn::QueueResetMemoryUsageCountsFunc pfncpp_nvnQueueResetMemoryUsageCounts;
extern nvn::QueueBuilderSetDeviceFunc pfncpp_nvnQueueBuilderSetDevice;
extern nvn::QueueBuilderSetDefaultsFunc pfncpp_nvnQueueBuilderSetDefaults;
extern nvn::QueueBuilderSetFlagsFunc pfncpp_nvnQueueBuilderSetFlags;
extern nvn::QueueBuilderSetCommandMemorySizeFunc pfncpp_nvnQueueBuilderSetCommandMemorySize;
extern nvn::QueueBuilderSetComputeMemorySizeFunc pfncpp_nvnQueueBuilderSetComputeMemorySize;
extern nvn::QueueBuilderSetControlMemorySizeFunc pfncpp_nvnQueueBuilderSetControlMemorySize;
extern nvn::QueueBuilderGetQueueMemorySizeFunc pfncpp_nvnQueueBuilderGetQueueMemorySize;
extern nvn::QueueBuilderSetQueueMemoryFunc pfncpp_nvnQueueBuilderSetQueueMemory;
extern nvn::QueueBuilderSetCommandFlushThresholdFunc pfncpp_nvnQueueBuilderSetCommandFlushThreshold;
extern nvn::QueueBuilderGetDeviceFunc pfncpp_nvnQueueBuilderGetDevice;
extern nvn::QueueBuilderGetFlagsFunc pfncpp_nvnQueueBuilderGetFlags;
extern nvn::QueueBuilderGetCommandMemorySizeFunc pfncpp_nvnQueueBuilderGetCommandMemorySize;
extern nvn::QueueBuilderGetComputeMemorySizeFunc pfncpp_nvnQueueBuilderGetComputeMemorySize;
extern nvn::QueueBuilderGetControlMemorySizeFunc pfncpp_nvnQueueBuilderGetControlMemorySize;
extern nvn::QueueBuilderGetCommandFlushThresholdFunc pfncpp_nvnQueueBuilderGetCommandFlushThreshold;
extern nvn::QueueBuilderGetMemorySizeFunc pfncpp_nvnQueueBuilderGetMemorySize;
extern nvn::QueueBuilderGetMemoryFunc pfncpp_nvnQueueBuilderGetMemory;
extern nvn::QueueInitializeFunc pfncpp_nvnQueueInitialize;
extern nvn::QueueFinalizeFunc pfncpp_nvnQueueFinalize;
extern nvn::QueueSetDebugLabelFunc pfncpp_nvnQueueSetDebugLabel;
extern nvn::QueueFlushFunc pfncpp_nvnQueueFlush;
extern nvn::QueueAcquireTextureFunc pfncpp_nvnQueueAcquireTexture;
extern nvn::WindowBuilderSetDeviceFunc pfncpp_nvnWindowBuilderSetDevice;
extern nvn::WindowBuilderSetDefaultsFunc pfncpp_nvnWindowBuilderSetDefaults;
extern nvn::WindowBuilderSetNativeWindowFunc pfncpp_nvnWindowBuilderSetNativeWindow;
extern nvn::WindowBuilderSetTexturesFunc pfncpp_nvnWindowBuilderSetTextures;
extern nvn::WindowBuilderSetPresentIntervalFunc pfncpp_nvnWindowBuilderSetPresentInterval;
extern nvn::WindowBuilderGetDeviceFunc pfncpp_nvnWindowBuilderGetDevice;
extern nvn::WindowBuilderGetNumTexturesFunc pfncpp_nvnWindowBuilderGetNumTextures;
extern nvn::WindowBuilderGetTextureFunc pfncpp_nvnWindowBuilderGetTexture;
extern nvn::WindowBuilderGetNativeWindowFunc pfncpp_nvnWindowBuilderGetNativeWindow;
extern nvn::WindowBuilderGetPresentIntervalFunc pfncpp_nvnWindowBuilderGetPresentInterval;
extern nvn::WindowInitializeFunc pfncpp_nvnWindowInitialize;
extern nvn::WindowFinalizeFunc pfncpp_nvnWindowFinalize;
extern nvn::WindowSetDebugLabelFunc pfncpp_nvnWindowSetDebugLabel;
extern nvn::WindowAcquireTextureFunc pfncpp_nvnWindowAcquireTexture;
extern nvn::WindowGetNativeWindowFunc pfncpp_nvnWindowGetNativeWindow;
extern nvn::WindowGetPresentIntervalFunc pfncpp_nvnWindowGetPresentInterval;
extern nvn::WindowSetPresentIntervalFunc pfncpp_nvnWindowSetPresentInterval;
extern nvn::WindowGetCropFunc pfncpp_nvnWindowGetCrop;
extern nvn::ProgramSetDebugLabelFunc pfncpp_nvnProgramSetDebugLabel;
extern nvn::MemoryPoolBuilderGetDeviceFunc pfncpp_nvnMemoryPoolBuilderGetDevice;
extern nvn::MemoryPoolBuilderGetMemoryFunc pfncpp_nvnMemoryPoolBuilderGetMemory;
extern nvn::MemoryPoolBuilderGetSizeFunc pfncpp_nvnMemoryPoolBuilderGetSize;
extern nvn::MemoryPoolBuilderGetFlagsFunc pfncpp_nvnMemoryPoolBuilderGetFlags;
extern nvn::MemoryPoolSetDebugLabelFunc pfncpp_nvnMemoryPoolSetDebugLabel;
extern nvn::MemoryPoolMapFunc pfncpp_nvnMemoryPoolMap;
extern nvn::MemoryPoolInvalidateMappedRangeFunc pfncpp_nvnMemoryPoolInvalidateMappedRange;
extern nvn::MemoryPoolMapVirtualFunc pfncpp_nvnMemoryPoolMapVirtual;
extern nvn::MemoryPoolGetSizeFunc pfncpp_nvnMemoryPoolGetSize;
extern nvn::MemoryPoolGetFlagsFunc pfncpp_nvnMemoryPoolGetFlags;
extern nvn::TexturePoolInitializeFunc pfncpp_nvnTexturePoolInitialize;
extern nvn::TexturePoolSetDebugLabelFunc pfncpp_nvnTexturePoolSetDebugLabel;
extern nvn::TexturePoolFinalizeFunc pfncpp_nvnTexturePoolFinalize;
extern nvn::TexturePoolRegisterTextureFunc pfncpp_nvnTexturePoolRegisterTexture;
extern nvn::TexturePoolRegisterImageFunc pfncpp_nvnTexturePoolRegisterImage;
extern nvn::TexturePoolGetMemoryPoolFunc pfncpp_nvnTexturePoolGetMemoryPool;
extern nvn::TexturePoolGetMemoryOffsetFunc pfncpp_nvnTexturePoolGetMemoryOffset;
extern nvn::TexturePoolGetSizeFunc pfncpp_nvnTexturePoolGetSize;
extern nvn::SamplerPoolInitializeFunc pfncpp_nvnSamplerPoolInitialize;
extern nvn::SamplerPoolSetDebugLabelFunc pfncpp_nvnSamplerPoolSetDebugLabel;
extern nvn::SamplerPoolFinalizeFunc pfncpp_nvnSamplerPoolFinalize;
extern nvn::SamplerPoolRegisterSamplerBuilderFunc pfncpp_nvnSamplerPoolRegisterSamplerBuilder;
extern nvn::SamplerPoolGetMemoryPoolFunc pfncpp_nvnSamplerPoolGetMemoryPool;
extern nvn::SamplerPoolGetMemoryOffsetFunc pfncpp_nvnSamplerPoolGetMemoryOffset;
extern nvn::SamplerPoolGetSizeFunc pfncpp_nvnSamplerPoolGetSize;
extern nvn::BufferBuilderGetDeviceFunc pfncpp_nvnBufferBuilderGetDevice;
extern nvn::BufferBuilderGetMemoryPoolFunc pfncpp_nvnBufferBuilderGetMemoryPool;
extern nvn::BufferBuilderGetMemoryOffsetFunc pfncpp_nvnBufferBuilderGetMemoryOffset;
extern nvn::BufferBuilderGetSizeFunc pfncpp_nvnBufferBuilderGetSize;
extern nvn::BufferSetDebugLabelFunc pfncpp_nvnBufferSetDebugLabel;
extern nvn::BufferMapFunc pfncpp_nvnBufferMap;
extern nvn::BufferFlushMappedRangeFunc pfncpp_nvnBufferFlushMappedRange;
extern nvn::BufferInvalidateMappedRangeFunc pfncpp_nvnBufferInvalidateMappedRange;
extern nvn::BufferGetMemoryPoolFunc pfncpp_nvnBufferGetMemoryPool;
extern nvn::BufferGetSizeFunc pfncpp_nvnBufferGetSize;
extern nvn::BufferGetDebugIDFunc pfncpp_nvnBufferGetDebugID;
extern nvn::TextureBuilderSetDeviceFunc pfncpp_nvnTextureBuilderSetDevice;
extern nvn::TextureBuilderSetDefaultsFunc pfncpp_nvnTextureBuilderSetDefaults;
extern nvn::TextureBuilderSetFlagsFunc pfncpp_nvnTextureBuilderSetFlags;
extern nvn::TextureBuilderSetTargetFunc pfncpp_nvnTextureBuilderSetTarget;
extern nvn::TextureBuilderSetWidthFunc pfncpp_nvnTextureBuilderSetWidth;
extern nvn::TextureBuilderSetHeightFunc pfncpp_nvnTextureBuilderSetHeight;
extern nvn::TextureBuilderSetDepthFunc pfncpp_nvnTextureBuilderSetDepth;
extern nvn::TextureBuilderSetSize1DFunc pfncpp_nvnTextureBuilderSetSize1D;
extern nvn::TextureBuilderSetSize2DFunc pfncpp_nvnTextureBuilderSetSize2D;
extern nvn::TextureBuilderSetSize3DFunc pfncpp_nvnTextureBuilderSetSize3D;
extern nvn::TextureBuilderSetLevelsFunc pfncpp_nvnTextureBuilderSetLevels;
extern nvn::TextureBuilderSetFormatFunc pfncpp_nvnTextureBuilderSetFormat;
extern nvn::TextureBuilderSetSamplesFunc pfncpp_nvnTextureBuilderSetSamples;
extern nvn::TextureBuilderSetSwizzleFunc pfncpp_nvnTextureBuilderSetSwizzle;
extern nvn::TextureBuilderSetDepthStencilModeFunc pfncpp_nvnTextureBuilderSetDepthStencilMode;
extern nvn::TextureBuilderGetStorageSizeFunc pfncpp_nvnTextureBuilderGetStorageSize;
extern nvn::TextureBuilderGetStorageAlignmentFunc pfncpp_nvnTextureBuilderGetStorageAlignment;
extern nvn::TextureBuilderSetStorageFunc pfncpp_nvnTextureBuilderSetStorage;
extern nvn::TextureBuilderSetPackagedTextureDataFunc pfncpp_nvnTextureBuilderSetPackagedTextureData;
extern nvn::TextureBuilderSetPackagedTextureLayoutFunc pfncpp_nvnTextureBuilderSetPackagedTextureLayout;
extern nvn::TextureBuilderSetStrideFunc pfncpp_nvnTextureBuilderSetStride;
extern nvn::TextureBuilderSetGLTextureNameFunc pfncpp_nvnTextureBuilderSetGLTextureName;
extern nvn::TextureBuilderGetStorageClassFunc pfncpp_nvnTextureBuilderGetStorageClass;
extern nvn::TextureBuilderGetDeviceFunc pfncpp_nvnTextureBuilderGetDevice;
extern nvn::TextureBuilderGetFlagsFunc pfncpp_nvnTextureBuilderGetFlags;
extern nvn::TextureBuilderGetTargetFunc pfncpp_nvnTextureBuilderGetTarget;
extern nvn::TextureBuilderGetWidthFunc pfncpp_nvnTextureBuilderGetWidth;
extern nvn::TextureBuilderGetHeightFunc pfncpp_nvnTextureBuilderGetHeight;
extern nvn::TextureBuilderGetDepthFunc pfncpp_nvnTextureBuilderGetDepth;
extern nvn::TextureBuilderGetLevelsFunc pfncpp_nvnTextureBuilderGetLevels;
extern nvn::TextureBuilderGetFormatFunc pfncpp_nvnTextureBuilderGetFormat;
extern nvn::TextureBuilderGetSamplesFunc pfncpp_nvnTextureBuilderGetSamples;
extern nvn::TextureBuilderGetSwizzleFunc pfncpp_nvnTextureBuilderGetSwizzle;
extern nvn::TextureBuilderGetDepthStencilModeFunc pfncpp_nvnTextureBuilderGetDepthStencilMode;
extern nvn::TextureBuilderGetPackagedTextureDataFunc pfncpp_nvnTextureBuilderGetPackagedTextureData;
extern nvn::TextureBuilderGetPackagedTextureLayoutFunc pfncpp_nvnTextureBuilderGetPackagedTextureLayout;
extern nvn::TextureBuilderGetStrideFunc pfncpp_nvnTextureBuilderGetStride;
extern nvn::TextureBuilderGetSparseTileLayoutFunc pfncpp_nvnTextureBuilderGetSparseTileLayout;
extern nvn::TextureBuilderGetGLTextureNameFunc pfncpp_nvnTextureBuilderGetGLTextureName;
extern nvn::TextureBuilderGetZCullStorageSizeFunc pfncpp_nvnTextureBuilderGetZCullStorageSize;
extern nvn::TextureBuilderGetMemoryPoolFunc pfncpp_nvnTextureBuilderGetMemoryPool;
extern nvn::TextureBuilderGetMemoryOffsetFunc pfncpp_nvnTextureBuilderGetMemoryOffset;
extern nvn::TextureViewSetLevelsFunc pfncpp_nvnTextureViewSetLevels;
extern nvn::TextureViewSetFormatFunc pfncpp_nvnTextureViewSetFormat;
extern nvn::TextureViewSetSwizzleFunc pfncpp_nvnTextureViewSetSwizzle;
extern nvn::TextureViewSetDepthStencilModeFunc pfncpp_nvnTextureViewSetDepthStencilMode;
extern nvn::TextureViewGetLevelsFunc pfncpp_nvnTextureViewGetLevels;
extern nvn::TextureViewGetLayersFunc pfncpp_nvnTextureViewGetLayers;
extern nvn::TextureViewGetFormatFunc pfncpp_nvnTextureViewGetFormat;
extern nvn::TextureViewGetSwizzleFunc pfncpp_nvnTextureViewGetSwizzle;
extern nvn::TextureViewGetDepthStencilModeFunc pfncpp_nvnTextureViewGetDepthStencilMode;
extern nvn::TextureViewGetTargetFunc pfncpp_nvnTextureViewGetTarget;
extern nvn::TextureViewCompareFunc pfncpp_nvnTextureViewCompare;
extern nvn::TextureInitializeFunc pfncpp_nvnTextureInitialize;
extern nvn::TextureGetZCullStorageSizeFunc pfncpp_nvnTextureGetZCullStorageSize;
extern nvn::TextureSetDebugLabelFunc pfncpp_nvnTextureSetDebugLabel;
extern nvn::TextureGetStorageClassFunc pfncpp_nvnTextureGetStorageClass;
extern nvn::TextureGetFlagsFunc pfncpp_nvnTextureGetFlags;
extern nvn::TextureGetTargetFunc pfncpp_nvnTextureGetTarget;
extern nvn::TextureGetWidthFunc pfncpp_nvnTextureGetWidth;
extern nvn::TextureGetHeightFunc pfncpp_nvnTextureGetHeight;
extern nvn::TextureGetDepthFunc pfncpp_nvnTextureGetDepth;
extern nvn::TextureGetLevelsFunc pfncpp_nvnTextureGetLevels;
extern nvn::TextureGetFormatFunc pfncpp_nvnTextureGetFormat;
extern nvn::TextureGetSamplesFunc pfncpp_nvnTextureGetSamples;
extern nvn::TextureGetSwizzleFunc pfncpp_nvnTextureGetSwizzle;
extern nvn::TextureGetDepthStencilModeFunc pfncpp_nvnTextureGetDepthStencilMode;
extern nvn::TextureGetTextureAddressFunc pfncpp_nvnTextureGetTextureAddress;
extern nvn::TextureGetSparseTileLayoutFunc pfncpp_nvnTextureGetSparseTileLayout;
extern nvn::TextureWriteTexelsFunc pfncpp_nvnTextureWriteTexels;
extern nvn::TextureWriteTexelsStridedFunc pfncpp_nvnTextureWriteTexelsStrided;
extern nvn::TextureReadTexelsFunc pfncpp_nvnTextureReadTexels;
extern nvn::TextureReadTexelsStridedFunc pfncpp_nvnTextureReadTexelsStrided;
extern nvn::TextureInvalidateTexelsFunc pfncpp_nvnTextureInvalidateTexels;
extern nvn::TextureGetMemoryPoolFunc pfncpp_nvnTextureGetMemoryPool;
extern nvn::TextureGetMemoryOffsetFunc pfncpp_nvnTextureGetMemoryOffset;
extern nvn::TextureGetStorageSizeFunc pfncpp_nvnTextureGetStorageSize;
extern nvn::TextureCompareFunc pfncpp_nvnTextureCompare;
extern nvn::TextureGetDebugIDFunc pfncpp_nvnTextureGetDebugID;
extern nvn::SamplerBuilderSetLodClampFunc pfncpp_nvnSamplerBuilderSetLodClamp;
extern nvn::SamplerBuilderSetCompareFunc pfncpp_nvnSamplerBuilderSetCompare;
extern nvn::SamplerBuilderSetBorderColorFunc pfncpp_nvnSamplerBuilderSetBorderColor;
extern nvn::SamplerBuilderSetBorderColoriFunc pfncpp_nvnSamplerBuilderSetBorderColori;
extern nvn::SamplerBuilderSetBorderColoruiFunc pfncpp_nvnSamplerBuilderSetBorderColorui;
extern nvn::SamplerBuilderSetMaxAnisotropyFunc pfncpp_nvnSamplerBuilderSetMaxAnisotropy;
extern nvn::SamplerBuilderSetReductionFilterFunc pfncpp_nvnSamplerBuilderSetReductionFilter;
extern nvn::SamplerBuilderGetDeviceFunc pfncpp_nvnSamplerBuilderGetDevice;
extern nvn::SamplerBuilderGetMinMagFilterFunc pfncpp_nvnSamplerBuilderGetMinMagFilter;
extern nvn::SamplerBuilderGetWrapModeFunc pfncpp_nvnSamplerBuilderGetWrapMode;
extern nvn::SamplerBuilderGetLodClampFunc pfncpp_nvnSamplerBuilderGetLodClamp;
extern nvn::SamplerBuilderGetLodBiasFunc pfncpp_nvnSamplerBuilderGetLodBias;
extern nvn::SamplerBuilderGetCompareFunc pfncpp_nvnSamplerBuilderGetCompare;
extern nvn::SamplerBuilderGetBorderColorFunc pfncpp_nvnSamplerBuilderGetBorderColor;
extern nvn::SamplerBuilderGetBorderColoriFunc pfncpp_nvnSamplerBuilderGetBorderColori;
extern nvn::SamplerBuilderGetBorderColoruiFunc pfncpp_nvnSamplerBuilderGetBorderColorui;
extern nvn::SamplerBuilderGetMaxAnisotropyFunc pfncpp_nvnSamplerBuilderGetMaxAnisotropy;
extern nvn::SamplerBuilderGetReductionFilterFunc pfncpp_nvnSamplerBuilderGetReductionFilter;
extern nvn::SamplerBuilderGetLodSnapFunc pfncpp_nvnSamplerBuilderGetLodSnap;
extern nvn::SamplerFinalizeFunc pfncpp_nvnSamplerFinalize;
extern nvn::SamplerSetDebugLabelFunc pfncpp_nvnSamplerSetDebugLabel;
extern nvn::SamplerGetMinMagFilterFunc pfncpp_nvnSamplerGetMinMagFilter;
extern nvn::SamplerGetWrapModeFunc pfncpp_nvnSamplerGetWrapMode;
extern nvn::SamplerGetLodClampFunc pfncpp_nvnSamplerGetLodClamp;
extern nvn::SamplerGetLodBiasFunc pfncpp_nvnSamplerGetLodBias;
extern nvn::SamplerGetCompareFunc pfncpp_nvnSamplerGetCompare;
extern nvn::SamplerGetBorderColorFunc pfncpp_nvnSamplerGetBorderColor;
extern nvn::SamplerGetBorderColoriFunc pfncpp_nvnSamplerGetBorderColori;
extern nvn::SamplerGetBorderColoruiFunc pfncpp_nvnSamplerGetBorderColorui;
extern nvn::SamplerGetMaxAnisotropyFunc pfncpp_nvnSamplerGetMaxAnisotropy;
extern nvn::SamplerGetReductionFilterFunc pfncpp_nvnSamplerGetReductionFilter;
extern nvn::SamplerCompareFunc pfncpp_nvnSamplerCompare;
extern nvn::SamplerGetDebugIDFunc pfncpp_nvnSamplerGetDebugID;
extern nvn::BlendStateSetBlendTargetFunc pfncpp_nvnBlendStateSetBlendTarget;
extern nvn::BlendStateSetAdvancedModeFunc pfncpp_nvnBlendStateSetAdvancedMode;
extern nvn::BlendStateSetAdvancedOverlapFunc pfncpp_nvnBlendStateSetAdvancedOverlap;
extern nvn::BlendStateSetAdvancedPremultipliedSrcFunc pfncpp_nvnBlendStateSetAdvancedPremultipliedSrc;
extern nvn::BlendStateSetAdvancedNormalizedDstFunc pfncpp_nvnBlendStateSetAdvancedNormalizedDst;
extern nvn::BlendStateGetBlendTargetFunc pfncpp_nvnBlendStateGetBlendTarget;
extern nvn::BlendStateGetBlendFuncFunc pfncpp_nvnBlendStateGetBlendFunc;
extern nvn::BlendStateGetBlendEquationFunc pfncpp_nvnBlendStateGetBlendEquation;
extern nvn::BlendStateGetAdvancedModeFunc pfncpp_nvnBlendStateGetAdvancedMode;
extern nvn::BlendStateGetAdvancedOverlapFunc pfncpp_nvnBlendStateGetAdvancedOverlap;
extern nvn::BlendStateGetAdvancedPremultipliedSrcFunc pfncpp_nvnBlendStateGetAdvancedPremultipliedSrc;
extern nvn::BlendStateGetAdvancedNormalizedDstFunc pfncpp_nvnBlendStateGetAdvancedNormalizedDst;
extern nvn::ColorStateSetLogicOpFunc pfncpp_nvnColorStateSetLogicOp;
extern nvn::ColorStateSetAlphaTestFunc pfncpp_nvnColorStateSetAlphaTest;
extern nvn::ColorStateGetBlendEnableFunc pfncpp_nvnColorStateGetBlendEnable;
extern nvn::ColorStateGetLogicOpFunc pfncpp_nvnColorStateGetLogicOp;
extern nvn::ColorStateGetAlphaTestFunc pfncpp_nvnColorStateGetAlphaTest;
extern nvn::ChannelMaskStateGetChannelMaskFunc pfncpp_nvnChannelMaskStateGetChannelMask;
extern nvn::MultisampleStateSetDefaultsFunc pfncpp_nvnMultisampleStateSetDefaults;
extern nvn::MultisampleStateSetMultisampleEnableFunc pfncpp_nvnMultisampleStateSetMultisampleEnable;
extern nvn::MultisampleStateSetSamplesFunc pfncpp_nvnMultisampleStateSetSamples;
extern nvn::MultisampleStateSetAlphaToCoverageEnableFunc pfncpp_nvnMultisampleStateSetAlphaToCoverageEnable;
extern nvn::MultisampleStateSetAlphaToCoverageDitherFunc pfncpp_nvnMultisampleStateSetAlphaToCoverageDither;
extern nvn::MultisampleStateGetMultisampleEnableFunc pfncpp_nvnMultisampleStateGetMultisampleEnable;
extern nvn::MultisampleStateGetSamplesFunc pfncpp_nvnMultisampleStateGetSamples;
extern nvn::MultisampleStateGetAlphaToCoverageEnableFunc pfncpp_nvnMultisampleStateGetAlphaToCoverageEnable;
extern nvn::MultisampleStateGetAlphaToCoverageDitherFunc pfncpp_nvnMultisampleStateGetAlphaToCoverageDither;
extern nvn::MultisampleStateSetRasterSamplesFunc pfncpp_nvnMultisampleStateSetRasterSamples;
extern nvn::MultisampleStateGetRasterSamplesFunc pfncpp_nvnMultisampleStateGetRasterSamples;
extern nvn::MultisampleStateSetCoverageModulationModeFunc pfncpp_nvnMultisampleStateSetCoverageModulationMode;
extern nvn::MultisampleStateGetCoverageModulationModeFunc pfncpp_nvnMultisampleStateGetCoverageModulationMode;
extern nvn::MultisampleStateSetCoverageToColorEnableFunc pfncpp_nvnMultisampleStateSetCoverageToColorEnable;
extern nvn::MultisampleStateGetCoverageToColorEnableFunc pfncpp_nvnMultisampleStateGetCoverageToColorEnable;
extern nvn::MultisampleStateSetCoverageToColorOutputFunc pfncpp_nvnMultisampleStateSetCoverageToColorOutput;
extern nvn::MultisampleStateGetCoverageToColorOutputFunc pfncpp_nvnMultisampleStateGetCoverageToColorOutput;
extern nvn::MultisampleStateSetSampleLocationsEnableFunc pfncpp_nvnMultisampleStateSetSampleLocationsEnable;
extern nvn::MultisampleStateGetSampleLocationsEnableFunc pfncpp_nvnMultisampleStateGetSampleLocationsEnable;
extern nvn::MultisampleStateGetSampleLocationsGridFunc pfncpp_nvnMultisampleStateGetSampleLocationsGrid;
extern nvn::MultisampleStateSetSampleLocationsGridEnableFunc pfncpp_nvnMultisampleStateSetSampleLocationsGridEnable;
extern nvn::MultisampleStateGetSampleLocationsGridEnableFunc pfncpp_nvnMultisampleStateGetSampleLocationsGridEnable;
extern nvn::MultisampleStateSetSampleLocationsFunc pfncpp_nvnMultisampleStateSetSampleLocations;
extern nvn::PolygonStateSetPolygonOffsetEnablesFunc pfncpp_nvnPolygonStateSetPolygonOffsetEnables;
extern nvn::PolygonStateGetCullFaceFunc pfncpp_nvnPolygonStateGetCullFace;
extern nvn::PolygonStateGetFrontFaceFunc pfncpp_nvnPolygonStateGetFrontFace;
extern nvn::PolygonStateGetPolygonModeFunc pfncpp_nvnPolygonStateGetPolygonMode;
extern nvn::PolygonStateGetPolygonOffsetEnablesFunc pfncpp_nvnPolygonStateGetPolygonOffsetEnables;
extern nvn::DepthStencilStateSetDefaultsFunc pfncpp_nvnDepthStencilStateSetDefaults;
extern nvn::DepthStencilStateGetDepthTestEnableFunc pfncpp_nvnDepthStencilStateGetDepthTestEnable;
extern nvn::DepthStencilStateGetDepthWriteEnableFunc pfncpp_nvnDepthStencilStateGetDepthWriteEnable;
extern nvn::DepthStencilStateGetDepthFuncFunc pfncpp_nvnDepthStencilStateGetDepthFunc;
extern nvn::DepthStencilStateGetStencilTestEnableFunc pfncpp_nvnDepthStencilStateGetStencilTestEnable;
extern nvn::DepthStencilStateGetStencilFuncFunc pfncpp_nvnDepthStencilStateGetStencilFunc;
extern nvn::DepthStencilStateGetStencilOpFunc pfncpp_nvnDepthStencilStateGetStencilOp;
extern nvn::VertexAttribStateGetFormatFunc pfncpp_nvnVertexAttribStateGetFormat;
extern nvn::VertexAttribStateGetStreamIndexFunc pfncpp_nvnVertexAttribStateGetStreamIndex;
extern nvn::VertexStreamStateSetStrideFunc pfncpp_nvnVertexStreamStateSetStride;
extern nvn::VertexStreamStateSetDivisorFunc pfncpp_nvnVertexStreamStateSetDivisor;
extern nvn::VertexStreamStateGetStrideFunc pfncpp_nvnVertexStreamStateGetStride;
extern nvn::VertexStreamStateGetDivisorFunc pfncpp_nvnVertexStreamStateGetDivisor;
extern nvn::CommandBufferInitializeFunc pfncpp_nvnCommandBufferInitialize;
extern nvn::CommandBufferFinalizeFunc pfncpp_nvnCommandBufferFinalize;
extern nvn::CommandBufferSetDebugLabelFunc pfncpp_nvnCommandBufferSetDebugLabel;
extern nvn::CommandBufferSetMemoryCallbackFunc pfncpp_nvnCommandBufferSetMemoryCallback;
extern nvn::CommandBufferSetMemoryCallbackDataFunc pfncpp_nvnCommandBufferSetMemoryCallbackData;
extern nvn::CommandBufferAddCommandMemoryFunc pfncpp_nvnCommandBufferAddCommandMemory;
extern nvn::CommandBufferAddControlMemoryFunc pfncpp_nvnCommandBufferAddControlMemory;
extern nvn::CommandBufferGetCommandMemorySizeFunc pfncpp_nvnCommandBufferGetCommandMemorySize;
extern nvn::CommandBufferGetCommandMemoryUsedFunc pfncpp_nvnCommandBufferGetCommandMemoryUsed;
extern nvn::CommandBufferGetCommandMemoryFreeFunc pfncpp_nvnCommandBufferGetCommandMemoryFree;
extern nvn::CommandBufferGetControlMemorySizeFunc pfncpp_nvnCommandBufferGetControlMemorySize;
extern nvn::CommandBufferGetControlMemoryUsedFunc pfncpp_nvnCommandBufferGetControlMemoryUsed;
extern nvn::CommandBufferGetControlMemoryFreeFunc pfncpp_nvnCommandBufferGetControlMemoryFree;
extern nvn::CommandBufferBeginRecordingFunc pfncpp_nvnCommandBufferBeginRecording;
extern nvn::CommandBufferCallCommandsFunc pfncpp_nvnCommandBufferCallCommands;
extern nvn::CommandBufferCopyCommandsFunc pfncpp_nvnCommandBufferCopyCommands;
extern nvn::CommandBufferBindMultisampleStateFunc pfncpp_nvnCommandBufferBindMultisampleState;
extern nvn::CommandBufferBindVertexBufferFunc pfncpp_nvnCommandBufferBindVertexBuffer;
extern nvn::CommandBufferBindUniformBufferFunc pfncpp_nvnCommandBufferBindUniformBuffer;
extern nvn::CommandBufferBindUniformBuffersFunc pfncpp_nvnCommandBufferBindUniformBuffers;
extern nvn::CommandBufferBindTransformFeedbackBufferFunc pfncpp_nvnCommandBufferBindTransformFeedbackBuffer;
extern nvn::CommandBufferBindTransformFeedbackBuffersFunc pfncpp_nvnCommandBufferBindTransformFeedbackBuffers;
extern nvn::CommandBufferBindStorageBufferFunc pfncpp_nvnCommandBufferBindStorageBuffer;
extern nvn::CommandBufferBindStorageBuffersFunc pfncpp_nvnCommandBufferBindStorageBuffers;
extern nvn::CommandBufferBindTexturesFunc pfncpp_nvnCommandBufferBindTextures;
extern nvn::CommandBufferBindImageFunc pfncpp_nvnCommandBufferBindImage;
extern nvn::CommandBufferBindImagesFunc pfncpp_nvnCommandBufferBindImages;
extern nvn::CommandBufferSetPatchSizeFunc pfncpp_nvnCommandBufferSetPatchSize;
extern nvn::CommandBufferSetInnerTessellationLevelsFunc pfncpp_nvnCommandBufferSetInnerTessellationLevels;
extern nvn::CommandBufferSetOuterTessellationLevelsFunc pfncpp_nvnCommandBufferSetOuterTessellationLevels;
extern nvn::CommandBufferSetPrimitiveRestartFunc pfncpp_nvnCommandBufferSetPrimitiveRestart;
extern nvn::CommandBufferBeginTransformFeedbackFunc pfncpp_nvnCommandBufferBeginTransformFeedback;
extern nvn::CommandBufferEndTransformFeedbackFunc pfncpp_nvnCommandBufferEndTransformFeedback;
extern nvn::CommandBufferPauseTransformFeedbackFunc pfncpp_nvnCommandBufferPauseTransformFeedback;
extern nvn::CommandBufferResumeTransformFeedbackFunc pfncpp_nvnCommandBufferResumeTransformFeedback;
extern nvn::CommandBufferDrawTransformFeedbackFunc pfncpp_nvnCommandBufferDrawTransformFeedback;
extern nvn::CommandBufferDrawElementsFunc pfncpp_nvnCommandBufferDrawElements;
extern nvn::CommandBufferDrawArraysInstancedFunc pfncpp_nvnCommandBufferDrawArraysInstanced;
extern nvn::CommandBufferDrawElementsInstancedFunc pfncpp_nvnCommandBufferDrawElementsInstanced;
extern nvn::CommandBufferDrawArraysIndirectFunc pfncpp_nvnCommandBufferDrawArraysIndirect;
extern nvn::CommandBufferDrawElementsIndirectFunc pfncpp_nvnCommandBufferDrawElementsIndirect;
extern nvn::CommandBufferMultiDrawArraysIndirectCountFunc pfncpp_nvnCommandBufferMultiDrawArraysIndirectCount;
extern nvn::CommandBufferMultiDrawElementsIndirectCountFunc pfncpp_nvnCommandBufferMultiDrawElementsIndirectCount;
extern nvn::CommandBufferClearColoriFunc pfncpp_nvnCommandBufferClearColori;
extern nvn::CommandBufferClearColoruiFunc pfncpp_nvnCommandBufferClearColorui;
extern nvn::CommandBufferDispatchComputeFunc pfncpp_nvnCommandBufferDispatchCompute;
extern nvn::CommandBufferDispatchComputeIndirectFunc pfncpp_nvnCommandBufferDispatchComputeIndirect;
extern nvn::CommandBufferSetViewportsFunc pfncpp_nvnCommandBufferSetViewports;
extern nvn::CommandBufferSetViewportSwizzlesFunc pfncpp_nvnCommandBufferSetViewportSwizzles;
extern nvn::CommandBufferSetScissorFunc pfncpp_nvnCommandBufferSetScissor;
extern nvn::CommandBufferSetScissorsFunc pfncpp_nvnCommandBufferSetScissors;
extern nvn::CommandBufferSetDepthBoundsFunc pfncpp_nvnCommandBufferSetDepthBounds;
extern nvn::CommandBufferSetDepthRangesFunc pfncpp_nvnCommandBufferSetDepthRanges;
extern nvn::CommandBufferSetTiledCacheActionFunc pfncpp_nvnCommandBufferSetTiledCacheAction;
extern nvn::CommandBufferSetTiledCacheTileSizeFunc pfncpp_nvnCommandBufferSetTiledCacheTileSize;
extern nvn::CommandBufferBindSeparateTextureFunc pfncpp_nvnCommandBufferBindSeparateTexture;
extern nvn::CommandBufferBindSeparateSamplerFunc pfncpp_nvnCommandBufferBindSeparateSampler;
extern nvn::CommandBufferBindSeparateTexturesFunc pfncpp_nvnCommandBufferBindSeparateTextures;
extern nvn::CommandBufferBindSeparateSamplersFunc pfncpp_nvnCommandBufferBindSeparateSamplers;
extern nvn::CommandBufferSetStencilValueMaskFunc pfncpp_nvnCommandBufferSetStencilValueMask;
extern nvn::CommandBufferSetStencilMaskFunc pfncpp_nvnCommandBufferSetStencilMask;
extern nvn::CommandBufferSetPointSizeFunc pfncpp_nvnCommandBufferSetPointSize;
extern nvn::CommandBufferSetLineWidthFunc pfncpp_nvnCommandBufferSetLineWidth;
extern nvn::CommandBufferSetPolygonOffsetClampFunc pfncpp_nvnCommandBufferSetPolygonOffsetClamp;
extern nvn::CommandBufferSetAlphaRefFunc pfncpp_nvnCommandBufferSetAlphaRef;
extern nvn::CommandBufferSetSampleMaskFunc pfncpp_nvnCommandBufferSetSampleMask;
extern nvn::CommandBufferSetRasterizerDiscardFunc pfncpp_nvnCommandBufferSetRasterizerDiscard;
extern nvn::CommandBufferSetDepthClampFunc pfncpp_nvnCommandBufferSetDepthClamp;
extern nvn::CommandBufferSetConservativeRasterEnableFunc pfncpp_nvnCommandBufferSetConservativeRasterEnable;
extern nvn::CommandBufferSetConservativeRasterDilateFunc pfncpp_nvnCommandBufferSetConservativeRasterDilate;
extern nvn::CommandBufferSetSubpixelPrecisionBiasFunc pfncpp_nvnCommandBufferSetSubpixelPrecisionBias;
extern nvn::CommandBufferCopyBufferToTextureFunc pfncpp_nvnCommandBufferCopyBufferToTexture;
extern nvn::CommandBufferCopyTextureToBufferFunc pfncpp_nvnCommandBufferCopyTextureToBuffer;
extern nvn::CommandBufferCopyTextureToTextureFunc pfncpp_nvnCommandBufferCopyTextureToTexture;
extern nvn::CommandBufferCopyBufferToBufferFunc pfncpp_nvnCommandBufferCopyBufferToBuffer;
extern nvn::CommandBufferClearBufferFunc pfncpp_nvnCommandBufferClearBuffer;
extern nvn::CommandBufferClearTextureFunc pfncpp_nvnCommandBufferClearTexture;
extern nvn::CommandBufferClearTextureiFunc pfncpp_nvnCommandBufferClearTexturei;
extern nvn::CommandBufferClearTextureuiFunc pfncpp_nvnCommandBufferClearTextureui;
extern nvn::CommandBufferReportValueFunc pfncpp_nvnCommandBufferReportValue;
extern nvn::CommandBufferSetRenderEnableFunc pfncpp_nvnCommandBufferSetRenderEnable;
extern nvn::CommandBufferSetRenderEnableConditionalFunc pfncpp_nvnCommandBufferSetRenderEnableConditional;
extern nvn::CommandBufferDiscardColorFunc pfncpp_nvnCommandBufferDiscardColor;
extern nvn::CommandBufferDiscardDepthStencilFunc pfncpp_nvnCommandBufferDiscardDepthStencil;
extern nvn::CommandBufferDownsampleFunc pfncpp_nvnCommandBufferDownsample;
extern nvn::CommandBufferTiledDownsampleFunc pfncpp_nvnCommandBufferTiledDownsample;
extern nvn::CommandBufferDownsampleTextureViewFunc pfncpp_nvnCommandBufferDownsampleTextureView;
extern nvn::CommandBufferTiledDownsampleTextureViewFunc pfncpp_nvnCommandBufferTiledDownsampleTextureView;
extern nvn::CommandBufferFenceSyncFunc pfncpp_nvnCommandBufferFenceSync;
extern nvn::CommandBufferSetTexturePoolFunc pfncpp_nvnCommandBufferSetTexturePool;
extern nvn::CommandBufferSetSamplerPoolFunc pfncpp_nvnCommandBufferSetSamplerPool;
extern nvn::CommandBufferSetShaderScratchMemoryFunc pfncpp_nvnCommandBufferSetShaderScratchMemory;
extern nvn::CommandBufferSaveZCullDataFunc pfncpp_nvnCommandBufferSaveZCullData;
extern nvn::CommandBufferRestoreZCullDataFunc pfncpp_nvnCommandBufferRestoreZCullData;
extern nvn::CommandBufferSetCopyRowStrideFunc pfncpp_nvnCommandBufferSetCopyRowStride;
extern nvn::CommandBufferSetCopyImageStrideFunc pfncpp_nvnCommandBufferSetCopyImageStride;
extern nvn::CommandBufferGetCopyRowStrideFunc pfncpp_nvnCommandBufferGetCopyRowStride;
extern nvn::CommandBufferGetCopyImageStrideFunc pfncpp_nvnCommandBufferGetCopyImageStride;
extern nvn::CommandBufferDrawTextureFunc pfncpp_nvnCommandBufferDrawTexture;
extern nvn::ProgramSetSubroutineLinkageFunc pfncpp_nvnProgramSetSubroutineLinkage;
extern nvn::CommandBufferSetProgramSubroutinesFunc pfncpp_nvnCommandBufferSetProgramSubroutines;
extern nvn::CommandBufferBindCoverageModulationTableFunc pfncpp_nvnCommandBufferBindCoverageModulationTable;
extern nvn::CommandBufferResolveDepthBufferFunc pfncpp_nvnCommandBufferResolveDepthBuffer;
extern nvn::CommandBufferSetColorReductionEnableFunc pfncpp_nvnCommandBufferSetColorReductionEnable;
extern nvn::CommandBufferSetColorReductionThresholdsFunc pfncpp_nvnCommandBufferSetColorReductionThresholds;
extern nvn::CommandBufferPushDebugGroupStaticFunc pfncpp_nvnCommandBufferPushDebugGroupStatic;
extern nvn::CommandBufferPushDebugGroupDynamicFunc pfncpp_nvnCommandBufferPushDebugGroupDynamic;
extern nvn::CommandBufferPushDebugGroupFunc pfncpp_nvnCommandBufferPushDebugGroup;
extern nvn::CommandBufferPopDebugGroupFunc pfncpp_nvnCommandBufferPopDebugGroup;
extern nvn::CommandBufferPopDebugGroupIdFunc pfncpp_nvnCommandBufferPopDebugGroupId;
extern nvn::CommandBufferInsertDebugMarkerStaticFunc pfncpp_nvnCommandBufferInsertDebugMarkerStatic;
extern nvn::CommandBufferInsertDebugMarkerDynamicFunc pfncpp_nvnCommandBufferInsertDebugMarkerDynamic;
extern nvn::CommandBufferInsertDebugMarkerFunc pfncpp_nvnCommandBufferInsertDebugMarker;
extern nvn::CommandBufferGetMemoryCallbackFunc pfncpp_nvnCommandBufferGetMemoryCallback;
extern nvn::CommandBufferGetMemoryCallbackDataFunc pfncpp_nvnCommandBufferGetMemoryCallbackData;
extern nvn::CommandBufferIsRecordingFunc pfncpp_nvnCommandBufferIsRecording;
extern nvn::SyncInitializeFunc pfncpp_nvnSyncInitialize;
extern nvn::SyncFinalizeFunc pfncpp_nvnSyncFinalize;
extern nvn::SyncSetDebugLabelFunc pfncpp_nvnSyncSetDebugLabel;
extern nvn::QueueFenceSyncFunc pfncpp_nvnQueueFenceSync;
extern nvn::SyncWaitFunc pfncpp_nvnSyncWait;
extern nvn::QueueWaitSyncFunc pfncpp_nvnQueueWaitSync;
extern nvn::EventBuilderSetDefaultsFunc pfncpp_nvnEventBuilderSetDefaults;
extern nvn::EventBuilderSetStorageFunc pfncpp_nvnEventBuilderSetStorage;
extern nvn::EventBuilderGetStorageFunc pfncpp_nvnEventBuilderGetStorage;
extern nvn::EventInitializeFunc pfncpp_nvnEventInitialize;
extern nvn::EventFinalizeFunc pfncpp_nvnEventFinalize;
extern nvn::EventGetValueFunc pfncpp_nvnEventGetValue;
extern nvn::EventSignalFunc pfncpp_nvnEventSignal;
extern nvn::CommandBufferWaitEventFunc pfncpp_nvnCommandBufferWaitEvent;


// nvn_Cpp.h:3726 (2.6.2 DWARF). Bit-set wrapper; only the members NX64NVNCore.cpp uses have bodies.
struct TextureFlags {  // size 0x4
    enum Enum {
        DISPLAY = 1,
        VIDEO_DECODE = 2,
        IMAGE = 4,
        COMPRESSIBLE = 8,
        LINEAR = 16,
        SPARSE = 32,
        LINEAR_RENDER_TARGET = 64,
        ADAPTIVE_ZCULL = 128,
        DEPTH16_PREFER_FAST_CLEAR = 256,
        MINIMAL_LAYOUT = 512,
        ZCULL_SUPPORT_STENCIL = 1024,
        NVN_TEXTURE_FLAGS_LARGE = 2147483647,
    };
private:
    int m_value;  // 0x00
public:
    TextureFlags() {}
    TextureFlags(int value) : m_value(value) {}
    operator int() const { return m_value; }
    nvn::TextureFlags & operator|=(const nvn::TextureFlags::Enum & other) { m_value |= other; return *this; }
};
static_assert(sizeof(TextureFlags) == 0x4, "TextureFlags");

class NVN_PRE_ALIGN TextureBuilder {  // size 0x80
protected:
    char reserved[128];  // 0x00
public:
    nvn::TextureBuilder & SetDevice(nvn::Device * device);
    nvn::TextureBuilder & SetDefaults();
    nvn::TextureBuilder & SetFlags(nvn::TextureFlags flags);
    nvn::TextureBuilder & SetTarget(nvn::TextureTarget target);
    nvn::TextureBuilder & SetFormat(nvn::Format format);
    nvn::TextureBuilder & SetSwizzle(nvn::TextureSwizzle r, nvn::TextureSwizzle g, nvn::TextureSwizzle b, nvn::TextureSwizzle a);
    nvn::TextureBuilder & SetSize3D(int width, int height, int depth);
    nvn::TextureBuilder & SetLevels(int numLevels);
    nvn::TextureBuilder & SetStride(ptrdiff_t stride);
    size_t GetStorageSize() const;
    size_t GetStorageAlignment() const;
    nvn::TextureBuilder & SetStorage(nvn::MemoryPool * pool, ptrdiff_t offset);
};
static_assert(sizeof(TextureBuilder) == 0x80, "TextureBuilder");

class NVN_PRE_ALIGN WindowBuilder {  // size 0x40
protected:
    char reserved[64];  // 0x00
public:
    nvn::WindowBuilder & SetDevice(nvn::Device * device);
    nvn::WindowBuilder & SetDefaults();
    nvn::WindowBuilder & SetNativeWindow(NVNnativeWindow nativeWindow);
    nvn::WindowBuilder & SetTextures(int numTextures, nvn::Texture *const * textures);
    nvn::WindowBuilder & SetPresentInterval(int presentInterval);
};
static_assert(sizeof(WindowBuilder) == 0x40, "WindowBuilder");

class NVN_PRE_ALIGN QueueBuilder {  // size 0x40
protected:
    char reserved[64];  // 0x00
public:
    nvn::QueueBuilder & SetDevice(nvn::Device * device);
    nvn::QueueBuilder & SetDefaults();
    nvn::QueueBuilder & SetComputeMemorySize(size_t size);
    nvn::QueueBuilder & SetCommandMemorySize(size_t size);
    nvn::QueueBuilder & SetCommandFlushThreshold(size_t threshold);
    size_t GetQueueMemorySize() const;
    nvn::QueueBuilder & SetQueueMemory(void * memory, size_t size);
};
static_assert(sizeof(QueueBuilder) == 0x40, "QueueBuilder");

}  // namespace nvn

#include "nvn_CppMethods.h"
