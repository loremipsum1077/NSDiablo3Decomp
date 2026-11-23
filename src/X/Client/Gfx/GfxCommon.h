// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/GfxCommon.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ImageHandle.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

enum {
    MAX_TEXTURE_STAGES_SM3 = 16,
    MAX_TEXTURE_STAGES = 24,
    MAX_TEXTURE_SAMPLERS = 16,
    MAX_TEXTURE_MATRICES = 12,
    MAX_ACTIVE_LIGHTS = 16,
    MAX_RENDER_TARGETS = 2,
    MAX_DIRECTIONAL_LIGHTS = 16,
    MAX_POINT_LIGHTS = 16,
    MAX_SPOT_LIGHTS = 16,
    MAX_CYLINDER_LIGHTS = 16,
    MAX_POINT_LINEAR_LIGHTS = 16,
};

enum RenderLayer {
    RENDERLAYER_INVALID = -1,
    RENDERLAYER_SKY = 0,
    RENDERLAYER_DEPTHPASS_DEPRECATED = 1,
    RENDERLAYER_TRANSPARENT_GROUND = 2,
    RENDERLAYER_POSTFX_DISTORTION = 3,
    RENDERLAYER_WATER_SIM = 4,
    RENDERLAYER_OPAQUE = 5,
    RENDERLAYER_TRANSPARENT = 6,
    NUM_SIMPLE_RENDERLAYERS = 7,
    RENDERLAYER_BLOOM = 7,
    RENDERLAYER_POSTFX = 8,
    RENDERLAYER_PROJECTED_SHADOWMAP = 9,
    RENDERLAYER_UI = 10,
    RENDERLAYER_REFLECTIONS = 11,
    RENDERLAYER_FLOOR = 12,
    RENDERLAYER_AURAS = 13,
    RENDERLAYER_GROUND_FX_1 = 14,
    RENDERLAYER_PROJECTED_ADDITIVE = 15,
    RENDERLAYER_INTERACTIVE_FOG = 16,
    RENDERLAYER_HIGHLIGHT = 17,
    RENDERLAYER_BATTLENET_UI = 18,
    RENDERLAYER_UI_OVERLAY = 19,
    RENDERLAYER_DOMINO = 20,
    RENDERLAYER_DEPTH_PREPASS = 21,
    RENDERLAYER_OCCLUSION_QUERY = 22,
    RENDERLAYER_TRANSPARENT_MASKED = 23,
    RENDERLAYER_SHADOWMAP_STENCIL_BOUNDS = 24,
    RENDERLAYER_HIGHLIGHT_STENCIL_BOUNDS = 25,
    RENDERLAYER_MOTION_BLUR = 26,
    NUM_RENDERLAYERS = 27,
    RENDERLAYER_EARLY_OPAQUE = 1,
};

enum RenderContext {
    RENDERCONTEXT_INVALID = -1,
    RENDERCONTEXT_DEFAULT = 0,
    RENDERCONTEXT_CHARLIST_0_DEPRECATED = 1,
    RENDERCONTEXT_CHARLIST_1_DEPRECATED = 2,
    RENDERCONTEXT_CHARLIST_2_DEPRECATED = 3,
    RENDERCONTEXT_CHARLIST_3_DEPRECATED = 4,
    RENDERCONTEXT_TALKINGHEAD_0_DEPRECATED = 5,
    RENDERCONTEXT_TALKINGHEAD_1_DEPRECATED = 6,
    RENDERCONTEXT_HUD_0 = 7,
    RENDERCONTEXT_FRIENDLIST = 8,
    RENDERCONTEXT_BANNER_AND_SIGIL = 9,
    RENDERCONTEXT_ACHIEVEMENT_FANFARE = 10,
    RENDERCONTEXT_MAIN_WINDOW_START = 0,
    RENDERCONTEXT_MAIN_WINDOW_END = 10,
    RENDERCONTEXT_HOLODECK = 256,
};

enum DrawMode {
    DRAWMODE_POINTLIST = 0,
    DRAWMODE_TRIANGLELIST = 1,
    DRAWMODE_TRIANGLESTRIP = 2,
    DRAWMODE_TRIANGLEFAN = 3,
    DRAWMODE_LINELIST = 4,
    DRAWMODE_LINESTRIP = 5,
};

enum CompareFunc {
    COMPARE_UNKNOWN = 0,
    COMPARE_NEVER = 1,
    COMPARE_LESS = 2,
    COMPARE_EQUAL = 3,
    COMPARE_LESSEQUAL = 4,
    COMPARE_GREATER = 5,
    COMPARE_NOTEQUAL = 6,
    COMPARE_GREATEREQUAL = 7,
    COMPARE_ALWAYS = 8,
    COMPARE_SIZE = 9,
};

enum StencilOp {
    STENCILOP_UNKNOWN = 0,
    STENCILOP_KEEP = 1,
    STENCILOP_ZERO = 2,
    STENCILOP_REPLACE = 3,
    STENCILOP_INCRSAT = 4,
    STENCILOP_DECRSAT = 5,
    STENCILOP_INVERT = 6,
    STENCILOP_INCR = 7,
    STENCILOP_DECR = 8,
    STENCILOP_SIZE = 9,
};

enum TextureAddressMode {
    TEXTUREADDRESSMODE_UNKNOWN = 0,
    TEXTUREADDRESSMODE_CLAMP = 1,
    TEXTUREADDRESSMODE_WRAP = 2,
    TEXTUREADDRESSMODE_MIRROR = 3,
    TEXTUREADDRESSMODE_SIZE = 4,
};

enum BlendFactor {
    BLENDFACTOR_UNKNOWN = 0,
    BLENDFACTOR_ZERO = 1,
    BLENDFACTOR_ONE = 2,
    BLENDFACTOR_SRCCOLOR = 3,
    BLENDFACTOR_INVSRCCOLOR = 4,
    BLENDFACTOR_SRCALPHA = 5,
    BLENDFACTOR_INVSRCALPHA = 6,
    BLENDFACTOR_DESTCOLOR = 7,
    BLENDFACTOR_INVDESTCOLOR = 8,
    BLENDFACTOR_DESTALPHA = 9,
    BLENDFACTOR_INVDESTALPHA = 10,
    BLENDFACTOR_BLENDFACTOR = 11,
    BLENDFACTOR_SIZE = 12,
};

enum BlendOp {
    BLENDOP_UNKNOWN = 0,
    BLENDOP_ADD = 1,
    BLENDOP_SUBTRACT = 2,
    BLENDOP_REVSUBTRACT = 3,
    BLENDOP_MIN = 4,
    BLENDOP_MAX = 5,
    BLENDOP_SIZE = 6,
};

enum FilterMode {
    FILTERMODE_UNKNOWN = 0,
    FILTERMODE_POINT_SAMPLED = 1,
    FILTERMODE_UNUSED = 2,
    FILTERMODE_BILINEAR = 3,
    FILTERMODE_TRILINEAR = 4,
    FILTERMODE_ANISOTROPIC = 5,
    FILTERMODE_PREF = 6,
    FILTERMODE_QUINCUNX = 7,
    FILTERMODE_PCF = 8,
    FILTERMODE_SIZE = 9,
};

enum CullMode {
    CULLMODE_UNKNOWN = 0,
    CULLMODE_NONE = 1,
    CULLMODE_CW = 2,
    CULLMODE_CCW = 3,
    CULLMODE_SIZE = 4,
};

enum FillMode {
    FILLMODE_SOLID = 0,
    FILLMODE_WIREFRAME = 1,
    FILLMODE_FLAT = 2,
    FILLMODE_UNKNOWN = 3,
    FILLMODE_SIZE = 4,
};

enum CubeMapFace {
    CUBEMAPFACE_POSX = 0,
    CUBEMAPFACE_NEGX = 1,
    CUBEMAPFACE_POSY = 2,
    CUBEMAPFACE_NEGY = 3,
    CUBEMAPFACE_POSZ = 4,
    CUBEMAPFACE_NEGZ = 5,
    NUM_CUBEMAPFACES = 6,
};

enum ShaderTex {
    SHADERTEX_NONE = 0,
    SHADERTEX_FIRST_SHADERTEX = 1,
    SHADERTEX_DIFFUSE = 1,
    SHADERTEX_LIGHTMAP = 2,
    SHADERTEX_BUMPMAP = 3,
    SHADERTEX_ENVMAP = 4,
    SHADERTEX_GLOSS = 5,
    SHADERTEX_GLOW = 6,
    SHADERTEX_REFLECTION = 7,
    SHADERTEX_IRRADIANCE = 8,
    SHADERTEX_REFRACTION = 9,
    SHADERTEX_DISTORTION = 10,
    SHADERTEX_DIFFUSE_LAYER_1 = 11,
    SHADERTEX_ALPHA_LAYER_1 = 12,
    SHADERTEX_DIFFUSE_LAYER_2 = 13,
    SHADERTEX_ALPHA_LAYER_2 = 14,
    SHADERTEX_DIFFUSE_LAYER_3 = 15,
    SHADERTEX_ALPHA_LAYER_3 = 16,
    SHADERTEX_DIFFUSE_LAYER_4 = 17,
    SHADERTEX_CLUTTER_MASK = 18,
    SHADERTEX_ALPHA = 19,
    SHADERTEX_WATER = 20,
    SHADERTEX_VIGNETTE = 21,
    SHADERTEX_SHADOW = 22,
    SHADERTEX_SHADOW_COLOR = 23,
    SHADERTEX_SHADOW_MASK = 24,
    SHADERTEX_DIFFUSE_OVERLAY = 25,
    SHADERTEX_OVERLAY_BEGIN = 26,
    SHADERTEX_OVERLAY_ARCANE_DEATH = 26,
    SHADERTEX_OVERLAY_FIRE_DEATH = 27,
    SHADERTEX_OVERLAY_LIGHTNING_DEATH = 28,
    SHADERTEX_OVERLAY_COLD_DEATH = 29,
    SHADERTEX_OVERLAY_POISON_DEATH = 30,
    SHADERTEX_OVERLAY_PLAGUE_DEATH = 31,
    SHADERTEX_OVERLAY_ZOMBIFIED = 32,
    SHADERTEX_OVERLAY_SAND = 33,
    SHADERTEX_OVERLAY_CRITICAL_HIT_BLOOD = 34,
    SHADERTEX_OVERLAY_STONED = 35,
    SHADERTEX_OVERLAY_CHILLED = 36,
    SHADERTEX_OVERLAY_FROZEN = 37,
    SHADERTEX_OVERLAY_POISONED = 38,
    SHADERTEX_OVERLAY_END = 38,
    SHADERTEX_DEPTH = 39,
    SHADERTEX_DIFFUSE_OVERLAY_1 = 40,
    SHADERTEX_DIFFUSE_OVERLAY_2 = 41,
    SHADERTEX_ALPHA_LAYER_4 = 42,
    SHADERTEX_DIFFUSE_LAYER_5 = 43,
    SHADERTEX_ALPHA_LAYER_5 = 44,
    SHADERTEX_DIFFUSE_LAYER_6 = 45,
    SHADERTEX_ALPHA_LAYER_6 = 46,
    SHADERTEX_BUMPMAP_1 = 47,
    SHADERTEX_BUMPMAP_2 = 48,
    SHADERTEX_BUMPMAP_3 = 49,
    SHADERTEX_BUMPMAP_4 = 50,
    SHADERTEX_BUMPMAP_5 = 51,
    SHADERTEX_BUMPMAP_6 = 52,
    SHADERTEX_INTERACTIVE_FOG = 53,
    SHADERTEX_TINT = 54,
    SHADERTEX_RIVER_MASK = 55,
    SHADERTEX_DYE_TINT_RAMP = 56,
    SHADERTEX_BANNER = 57,
    SHADERTEX_PMA_MASK = 58,
    SHADERTEX_BANNER_TINT_RAMP = 59,
    SHADERTEX_REFLECTION_1 = 60,
    SHADERTEX_SSAO = 61,
    NUM_SHADER_TEXTURES = 62,
};

enum StreamIndex {
    STREAMINDEX_DEFAULT = 0,
    STREAMINDEX_BUMP = 1,
    STREAMINDEX_CLOTH = 2,
    MAX_ACTIVE_VERTEX_STREAMS = 3,
};

enum GfxLockFlags {
    GFXLOCK_DISCARD_BIT = 0,
    GFXLOCK_READONLY_BIT = 1,
};

enum {
    TEXCREATE_RENDER_TARGET_BIT = 0,
    TEXCREATE_DEPTH_TARGET_BIT = 1,
    TEXCREATE_AUTOGENERATE_MIPMAPS_BIT = 2,
    TEXCREATE_RENDER_TARGET_NO_TILE_BIT = 3,
    TEXCREATE_USE_ESRAM_BIT = 4,
    TEXCREATE_USE_FMASK_BIT = 5,
    TEXCREATE_RETAIN_CPU_TEXTURE_BIT = 6,
};

enum {
    GFXSHADOWTECH_FIRST = 0,
    GFXSHADOWTECH_REFRAST = 0,
    GFXSHADOWTECH_DF16 = 1,
    GFXSHADOWTECH_FETCH4 = 2,
    GFXSHADOWTECH_PCF = 3,
    GFXSHADOWTECH_VSM = 4,
    GFXSHADOWTECH_LAST = 5,
    GFXSHADOWTECH_ATI_FETCH4_LOD_BIAS = 512,
};

enum PlaybackMode {
    PLAYBACKMODE_NORMAL = 0,
    PLAYBACKMODE_LOOP = 2,
};

enum {
    DISPLAYMODE_IS_FALLBACK_BIT = 0,
    DISPLAYMODE_IS_DEFAULT_BIT = 1,
    DISPLAYMODE_VSYNC_BIT = 2,
    DISPLAYMODE_SCREENSHOT_ENABLE_BIT = 3,
    DISPLAYMODE_REFERENCE_RASTERIZER_BIT = 4,
};

enum {
    WINDOWMODE_FULLSCREEN = 0,
    WINDOWMODE_WINDOWED = 1,
    WINDOWMODE_FULLSCREEN_WINDOWED = 2,
    NUM_WINDOWMODES = 3,
};

typedef int32 WindowMode;

enum TexAnimType {
    TEXANIMTYPE_NONE = 0,
    TEXANIMTYPE_FIXED = 1,
    TEXANIMTYPE_TRANSLATION = 2,
    TEXANIMTYPE_FRAMEBYFRAME = 3,
    TEXANIMTYPE_WORLDREL_CUBEMAP = 4,
    TEXANIMTYPE_MAYA_KEYFRAME0 = 5,
    TEXANIMTYPE_MAYA_KEYFRAME1 = 6,
};

enum TexAnimFlag {
    TEXANIMFLAG_U_INIT_RAND = 0,
    TEXANIMFLAG_V_INIT_RAND = 1,
    TEXANIMFLAG_USE_GAME_TIME = 2,
    TEXANIMFLAG_CENTER_ROTATION = 3,
};

enum LightType {
    LIGHTTYPE_DIRECTIONAL = 0,
    LIGHTTYPE_POINT = 1,
    LIGHTTYPE_SPOTLIGHT = 2,
    LIGHTTYPE_CYLINDRICAL = 3,
    LIGHTTYPE_POINT_LINEAR = 4,
};

enum {
    RESOURCEUSAGE_IMMUTABLE = 0,
    RESOURCEUSAGE_STATIC = 1,
    RESOURCEUSAGE_DYNAMIC = 2,
    RESOURCEUSAGE_PROXY = 3,
};

typedef int32 ResourceUsage;

enum {
    PCIVENDOR_ATI = 4098,
    PCIVENDOR_INTEL = 32902,
    PCIVENDOR_NVIDIA = 4318,
};

typedef void * HTexture;

typedef void * HVB;

typedef void * HIB;

struct TexData {  // size 0x8
    union {  // size 0x8
        HTexture htex;  // 0x00
        uint64 dwForce64Bit;  // 0x00
    };  // 0x00
};
static_assert(sizeof(TexData) == 0x8, "TexData");

struct TexFrame {  // size 0x14
    ImageHandle hImageHandle;  // 0x00
    float flU0;  // 0x04
    float flV0;  // 0x08
    float flU1;  // 0x0c
    float flV1;  // 0x10
};
static_assert(sizeof(TexFrame) == 0x14, "TexFrame");

struct ImageTextureFrame {  // size 0x8
    SNO snoTexture;  // 0x00
    int32 nFrame;  // 0x04
    ImageTextureFrame() {  // line 506
        snoTexture = SNO_NONE;
        nFrame = -1;
    }
    ImageTextureFrame(const SNO snoTexture, const int32 nFrame) {  // line 512
        this->snoTexture = snoTexture;
        this->nFrame = nFrame;
    }
    ImageTextureFrame(LPCSTR);
    BOOL operator!=(const ImageTextureFrame & other) const {  // line 523
        return snoTexture != other.snoTexture || nFrame != other.nFrame;
    }
    BOOL operator==(const ImageTextureFrame & other) const {  // line 528
        return snoTexture == other.snoTexture && nFrame == other.nFrame;
    }
};
static_assert(sizeof(ImageTextureFrame) == 0x8, "ImageTextureFrame");

struct FrameAnim {  // size 0x14
    PlaybackMode ePlaybackMode;  // 0x00
    float flFrameRateMin;  // 0x04
    float flFrameRateDelta;  // 0x08
    uint32 dwStartFrameMin;  // 0x0c
    uint32 dwStartFrameDelta;  // 0x10
};
static_assert(sizeof(FrameAnim) == 0x14, "FrameAnim");

struct TexAnimParams {  // size 0x90
    TexAnimType eTexAnimType;  // 0x00
    Matrix4x4 mTex;  // 0x04
    float flUTransInitial;  // 0x44
    float flUTransVel;  // 0x48
    float flUTransVelDelta;  // 0x4c
    float flVTransInitial;  // 0x50
    float flVTransVel;  // 0x54
    float flVTransVelDelta;  // 0x58
    Angle aRotInitial;  // 0x5c
    Angle aRotVel;  // 0x60
    Angle aRotVelDelta;  // 0x64
    BOOL fPinTranslation;  // 0x68
    BOOL fUseObjectValueForU;  // 0x6c
    BOOL fUseObjectValueForV;  // 0x70
    BOOL fScaleUVsToFrame;  // 0x74
    FrameAnim tFrameAnim;  // 0x78
    uint32 dwAnimFlags;  // 0x8c
};
static_assert(sizeof(TexAnimParams) == 0x90, "TexAnimParams");

struct LightInterpState {  // size 0x50
    WorldDistance wdInnerRadius;  // 0x00
    WorldDistance wdRadius;  // 0x04
    float flRadMinInnerRadInverse;  // 0x08
    float flAtten[3];  // 0x0c
    Angle aHotspot;  // 0x18
    Angle aFalloff;  // 0x1c
    RGBAColorValue rgbavalAmbient;  // 0x20
    RGBAColorValue rgbavalDiffuse;  // 0x30
    RGBAColorValue rgbavalSpecular;  // 0x40
};
static_assert(sizeof(LightInterpState) == 0x50, "LightInterpState");

struct LightState {  // size 0x88
    LightType eType;  // 0x00
    PRTransform transform;  // 0x04
    Vector3D wp;  // 0x20
    Vector3D wv;  // 0x2c
    LightInterpState tInterpState;  // 0x38
};
static_assert(sizeof(LightState) == 0x88, "LightState");

struct DisplayMode {  // size 0x38
    uint32 dwFlags;  // 0x00
    WindowMode dwWindowMode;  // 0x04
    int32 nWinLeft;  // 0x08
    int32 nWinTop;  // 0x0c
    int32 nWinWidth;  // 0x10
    int32 nWinHeight;  // 0x14
    uint32 dwUIOptWidth;  // 0x18
    uint32 dwUIOptHeight;  // 0x1c
    uint32 dwWidth;  // 0x20
    uint32 dwHeight;  // 0x24
    int32 nRefreshRate;  // 0x28
    uint32 dwBitDepth;  // 0x2c
    uint32 dwMSAALevel;  // 0x30
    float flAspectRatio;  // 0x34
};
static_assert(sizeof(DisplayMode) == 0x38, "DisplayMode");

typedef float ShaderFloat[4];

typedef int32 ShaderInt[4];

struct DirectionalLight {  // size 0x30
    Vector3D wv;  // 0x00
    float flPad0;  // 0x0c
    RGBAColorValue rgbavalDiffuse;  // 0x10
    RGBAColorValue rgbavalSpecular;  // 0x20
};
static_assert(sizeof(DirectionalLight) == 0x30, "DirectionalLight");

struct PointLight {  // size 0x50
    Vector4D wp;  // 0x00
    float flAtten[4];  // 0x10
    RGBAColorValue rgbavalAmbient;  // 0x20
    RGBAColorValue rgbavalDiffuse;  // 0x30
    RGBAColorValue rgbavalSpecular;  // 0x40
};
static_assert(sizeof(PointLight) == 0x50, "PointLight");

struct SpotLight {  // size 0x60
    Vector3D wp;  // 0x00
    float flPad0;  // 0x0c
    Vector3D wv;  // 0x10
    float flPad1;  // 0x1c
    ShaderFloat flRadii;  // 0x20
    RGBAColorValue rgbavalAmbient;  // 0x30
    RGBAColorValue rgbavalDiffuse;  // 0x40
    RGBAColorValue rgbavalSpecular;  // 0x50
};
static_assert(sizeof(SpotLight) == 0x60, "SpotLight");

struct CylindricalLight {  // size 0x60
    Vector3D wp;  // 0x00
    float flPad0;  // 0x0c
    Vector3D wv;  // 0x10
    float flPad1;  // 0x1c
    ShaderFloat flRadii;  // 0x20
    RGBAColorValue rgbavalAmbient;  // 0x30
    RGBAColorValue rgbavalDiffuse;  // 0x40
    RGBAColorValue rgbavalSpecular;  // 0x50
};
static_assert(sizeof(CylindricalLight) == 0x60, "CylindricalLight");

struct PointLinearLight {  // size 0x40
    Vector3D wp;  // 0x00
    float flPad0;  // 0x0c
    ShaderFloat rgbavalAmbient_radiiA;  // 0x10
    ShaderFloat rgbavalDiffuse_radiiB;  // 0x20
    RGBAColorValue rgbavalSpecular;  // 0x30
};
static_assert(sizeof(PointLinearLight) == 0x40, "PointLinearLight");

enum {
    DISPLAY_SUPPORTS_GAMMA_CONTROL_BIT = 0,
    DISPLAY_SUPPORTS_GAMMA_CALIBRATION_BIT = 1,
    DISPLAY_SUPPORTS_HARDWARE_CURSOR_BIT = 2,
    DISPLAY_SUPPORTS_4K_TEX_BIT = 3,
    DISPLAY_SUPPORTS_SM4_BIT = 4,
};

enum HardwareClass {
    HARDWARECLASS_UNSUPPORTED = 0,
    HARDWARECLASS_SLOW = 1,
    HARDWARECLASS_LOW = 2,
    HARDWARECLASS_MEDIUM = 3,
    HARDWARECLASS_HIGH = 4,
    HARDWARECLASS_ULTRA = 5,
    HARDWARECLASS_EXTREME = 6,
};

enum {
    REGISTERS_PER_BONE_ENTRY = 3,
    MAX_BONES_PER_PASS = 45,
};

struct DisplayCaps {  // size 0x10
    uint32 dwFlags;  // 0x00
    uint32 dwMaxVertexStreams;  // 0x04
    uint32 dwMaxAnisotropy;  // 0x08
    uint32 dwMaxSimultaneousRTs;  // 0x0c
};
static_assert(sizeof(DisplayCaps) == 0x10, "DisplayCaps");

struct GfxOptions {  // size 0x8
    float flGammaValue;  // 0x00
    BOOL fUseReferenceRasterizer;  // 0x04
};
static_assert(sizeof(GfxOptions) == 0x8, "GfxOptions");

struct GfxDeviceDesc {  // size 0x198
    uint32 vendorID;  // 0x00
    uint32 deviceID;  // 0x04
    char deviceDescription[128];  // 0x08
    char driverVersion[128];  // 0x88
    char apiDescription[128];  // 0x108
    uint64 videoMemSize;  // 0x188
    uint64 agpMemSize;  // 0x190
};
static_assert(sizeof(GfxDeviceDesc) == 0x198, "GfxDeviceDesc");

// Defined in GfxCommon.cpp.
extern const TexAnimParams cgtTexAnimParamsDefault;
extern const TexFrame cgtTexFrameDefault;
extern const ImageTextureFrame cgtImageTextureFrameNull;
extern const DisplayMode cgtDisplayModeDefault;

// X/Client/Gfx/GfxCommon.h:644 (inlined into X/Client/PostFX.cpp; body written from those copies).
inline void ShaderFloatSet(ShaderFloat & tReg, const float flVal0, const float flVal1, const float flVal2, const float flVal3) {
    tReg[0] = flVal0;
    tReg[1] = flVal1;
    tReg[2] = flVal2;
    tReg[3] = flVal3;
}

// X/Client/Gfx/GfxCommon.h:657
inline void ShaderFloatSet(ShaderFloat & tReg, const float flVal) {
    tReg[0] = flVal;
    tReg[1] = flVal;
    tReg[2] = flVal;
    tReg[3] = flVal;
}
