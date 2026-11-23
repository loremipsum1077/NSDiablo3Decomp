// Original file: X/Client/RWindow.cpp

#include "RWindow.h"
#include "CGame.h"
#include "SubObjectGfx.h"
#include "Definitions/TextureDefinition.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameTemplate.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/World.h"
#include "../Sigma/GfxColorFormats.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Memory/SigmaMemory.h"

// Not used directly: the original reaches these through an include chain, and their header-level
// statics (blz::ignore, SNONAME_NONE, cgplaceNull, the GfxVB.h normals) are part of this file's
// _GLOBAL__sub_I.
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
// X/Client/Weather.cpp:268 and :345
namespace Weather {
WeatherRWindowData * InitializeForRWindow();
void DisposeForRWindow(WeatherRWindowData * ptData);
}  // namespace Weather
// X/Client/Shake.cpp:109 and :122
ShakeRWindowData * ShakeInitializeForRWindow();
void ShakeDisposeForRWindow(ShakeRWindowData * ptData);
// X/Client/PostFX.h:11
enum EffectBuffer {
    EFFECTBUFFER_NONE = -1,
    EFFECTBUFFER_SCREEN_PING = 0,
    EFFECTBUFFER_SCREEN_PONG = 1,
    EFFECTBUFFER_BLUR_DISTORTION = 2,
    EFFECTBUFFER_SCREEN_HALF_PING = 3,
    EFFECTBUFFER_REFLECTION_0 = 4,
    EFFECTBUFFER_REFLECTION_1 = 5,
    EFFECTBUFFER_REFLECTION_DEPTH = 6,
    EFFECTBUFFER_REFRACTION = 7,
    EFFECTBUFFER_HDR = 8,
    EFFECTBUFFER_BACKBUFFER = 9,
    EFFECTBUFFER_DEPTH = 10,
    EFFECTBUFFER_DEPTH_PREV = 11,
    EFFECTBUFFER_BLOOM_PONG = 12,
    EFFECTBUFFER_SCREEN_PONG_X360_MINUS1 = 13,
    EFFECTBUFFER_SCREEN_PONG_X360_MINUS2 = 14,
    EFFECTBUFFER_COLOR = 15,
    EFFECTBUFFER_BACKBUFFER_DEPTH = 16,
    EFFECTBUFFER_HIGHLIGHT = 17,
    EFFECTBUFFER_MSAA_COLOR = 18,
    EFFECTBUFFER_MSAA_DEPTH = 19,
    EFFECTBUFFER_MSAA_COLOR_FMASK = 20,
    EFFECTBUFFER_SCREEN_PING_LDR = 21,
    EFFECTBUFFER_SCREEN_PONG_LDR = 22,
    EFFECTBUFFER_DEFAULT_SOFTPARTICLE_DEPTH = 23,
    EFFECTBUFFER_SSAO_NORMAL = 24,
    EFFECTBUFFER_SSAO = 25,
    EFFECTBUFFER_SSAO_PING = 26,
    EFFECTBUFFER_SSAO_PONG = 27,
    NUM_EFFECT_BUFFERS = 28,
};
// X/Client/PostFX.cpp:170 and :4208
LPCSTR PostFXGetEffectRTDebugName(int32 nType);
BOOL PostFXTestEnableHDR();
// X/Client/Gfx/GfxAPI.h:36
enum {
    GFX_NONE = 0,
    GFX_NULL = 1,
    GFX_D3D = 2,
    GFX_D3D11 = 3,
    GFX_OPENGL = 4,
    GFX_PS4GNM = 5,
    GFX_XBOXONED3D = 6,
    GFX_NX64NVN = 7,
};
typedef int32 GfxAPI;
// X/Client/Gfx/Gfx.cpp
GfxAPI GfxGetSystemID();
void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight);
void GfxGetCurrentFloatDimensions(float & flWidth, float & flHeight);
uint32 GfxGetMSAALevel();
BOOL GfxGetMSAAEnabled();
BOOL GfxRenderPresent();
void GfxSetRenderTarget(const SNO sno, const CubeMapFace eCubeMapFace);
void GfxSetDepthTarget(const SNO sno);
void GfxSetSwapChainRenderTarget(const SNO sno);
TextureFormat GfxGetDepthTextureBackbufferFormat();
// X/Client/Gfx/GfxCommon.h (the enum is anonymous there; mangled as int)
typedef int32 ResourceUsage;
// X/Client/Definitions/TextureDefinition.cpp:994
SNO TextureDefinitionCreateTexture(LPCSTR szTexDebugName, const TextureFormat eDesiredTexFormat, TextureFormat & eSupportedTextureFormat, const uint32 dwWidth, const uint32 dwHeight, const ResourceUsage eUsage, const uint32 dwTexCreateFlags, const uint32 dwMSAALevel, const uint32 dwTargetERAMOffset, const int32 nExponentBias);
#include "Definitions/PreferencesDefinition.h"




namespace Prefs {
const VideoPrefs & GetVideoPrefs();
}  // namespace Prefs
// GameParamFlags comes from its owning GameTemplate.h; function in Game.cpp:5098.
BOOL GameIsParamFlagSet(GameParamFlags eFlag);
// X/Common/ActorCommonDataSwig.h:53
enum {
    TEAM_PLAYER_1 = 3,
};
#include "../Common/Player.h"




// X/Client/CPlayer.cpp:1612
Player * LocalPlayerGet();
// X/Common/World.cpp:775
BOOL WorldIsRotated(const SNO snoWorld);
// X/Client/Camera/CameraManager.cpp:969 and :1012
void CameraManagerBuildAdditiveCamera(const int32 nCameraIndex, const Vector3D & wpInitialTarget, const Angle aRotation);
void CameraManagerBuildRippleMapCamera(const int32 nCameraIndex, const Vector3D & wpTarget);

MainRWindow RWindowMgr::m_tMainRWindow;  // line 27

RWindowID RWindowIDFromContext(const RenderContext eContext) {  // line 40
    if (eContext == RENDERCONTEXT_INVALID)
        return RWindowInvalid;
    return RWindowMainWindow;
}

void RWindowMgr::InitializeForGame() {  // line 60
    CGame & tGame = CGameGet();

    tGame.ptRWindowMgr = new (SigmaMemoryNew(sizeof(RWindowMgr), 4, NULL, TRUE)) RWindowMgr();

    m_tMainRWindow.Initialize(RWindowMainWindow, RENDERCONTEXT_DEFAULT, 0, 5);
    GetRWindowMgr()->AddRWindow(m_tMainRWindow);
}

void RWindowMgr::DisposeForGame() {  // line 81
    RWindowMgr *& ptRWindowMgr = CGameGet().ptRWindowMgr;

    m_tMainRWindow.Dispose();
    ::delete ptRWindowMgr;
    ptRWindowMgr = NULL;
}

inline RWindow::RWindow() {  // line 96
    snoTexDefaultColorTarget = SNO_NONE;
    snoTexDefaultDepthTarget = SNO_NONE;
    for (int32 i = 0; i < NUM_EFFECT_BUFFERS; i++) {
        snoEffectBuffer[i] = SNO_NONE;
        snoEffectDepthBuffer[i] = SNO_NONE;
    }
    m_snoTexPMA = SNO_NONE;
    m_id = RWindowInvalid;
    m_nCameraIndex = 0;
    m_nPMACameraIndex = 0;

    m_ptWeatherRWindowData = Weather::InitializeForRWindow();
    m_ptShakeRWindowData = ShakeInitializeForRWindow();
    m_ptVariableResRWindowData = CGameVariableResInitializeForRWindow();
}

RWindow::~RWindow() {  // line 124
    Weather::DisposeForRWindow(m_ptWeatherRWindowData);
    ShakeDisposeForRWindow(m_ptShakeRWindowData);
    CGameVariableResDisposeForRWindow(m_ptVariableResRWindowData);
}

inline void RWindow::Initialize(const RWindowID id, const RenderContext eContext, const int32 nMainCameraIndex, const int32 nPMACameraIndex) {  // line 134
    m_id = id;
    for (int32 i = 0; i < 2; i++) {
        tAsyncData[i].dispList.Initialize(eContext, FALSE);
        for (int nRefl = 0; nRefl < 2; nRefl++)
            tAsyncData[i].dispListReflections[nRefl].Initialize(eContext, FALSE);
        tAsyncData[i].dispListProjected.Initialize(eContext, FALSE);
        tAsyncData[i].wpLocalPlayerPosition = Vector3D(0.0f, 0.0f, 0.0f);
        tAsyncData[i].fLocalPlayerValid = FALSE;
    }
    m_nCameraIndex = nMainCameraIndex;
    m_nPMACameraIndex = nPMACameraIndex;
}

inline void RWindow::Dispose() {  // line 160
    for (int32 i = 0; i < 2; i++) {
        tAsyncData[i].dispList.Dispose();
        for (int nRefl = 0; nRefl < 2; nRefl++)
            tAsyncData[i].dispListReflections[nRefl].Dispose();
        tAsyncData[i].dispListProjected.Dispose();
    }
}

BOOL RWindow::PostFXGfxInit() {  // line 177
    uint32 renderTargetReduction = 1;
    IVector2D vecScreen = GetPixelDimensions();
    uint32 dwScreenWidth = vecScreen.x;
    uint32 dwRenderWidth;
    TextureFormat eDefaultColorFormat = TEXTUREFORMAT_ARGB8888;
    uint32 dwRenderHeight;
    uint32 dwScreenHeight = vecScreen.y;
    int dwRenderMSAALevel = GfxGetMSAALevel();
    dwRenderWidth = dwScreenWidth >> renderTargetReduction;
    dwRenderHeight = dwScreenHeight >> renderTargetReduction;

    for (int32 i = 0; i < NUM_EFFECT_BUFFERS; i++) {
        uint32 dwWidth = dwScreenWidth;
        uint32 dwHeight = dwScreenHeight;
        TextureFormat eFormat = eDefaultColorFormat;
        int32 nExponentBias = 0;
        TextureFormat eDepthFormat = TEXTUREFORMAT_UNKNOWN;
        uint32 dwMSAALevel = 1;
        uint32 dwTexCreateFlags = 1;
        uint32 dwTargetERAMOffset = 0;

        snoEffectBuffer[i] = SNO_NONE;
        snoEffectDepthBuffer[i] = SNO_NONE;
        switch (i) {
        case EFFECTBUFFER_SCREEN_PING:
        case EFFECTBUFFER_SCREEN_PONG:
        case EFFECTBUFFER_BACKBUFFER:
            break;
        case EFFECTBUFFER_REFRACTION:
            if (!GfxGetMSAAEnabled() || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D) {
                dwWidth = dwRenderWidth;
                dwHeight = dwRenderHeight;
            }
            break;
        case EFFECTBUFFER_SCREEN_HALF_PING:
        case EFFECTBUFFER_BLOOM_PONG:
            eFormat = TEXTUREFORMAT_A16B16G16R16F;
            dwWidth = dwRenderWidth >> 1;
            dwHeight = dwRenderHeight >> 1;
            break;
        case EFFECTBUFFER_BLUR_DISTORTION:
            if (!PostFXTestEnableHDR())
                eFormat = TEXTUREFORMAT_ARGB8888;
            break;
        case EFFECTBUFFER_SCREEN_PING_LDR:
        case EFFECTBUFFER_SCREEN_PONG_LDR:
            eFormat = TEXTUREFORMAT_ARGB8888;
            break;
        case EFFECTBUFFER_REFLECTION_0:
        case EFFECTBUFFER_REFLECTION_1:
            if (!Prefs::GetVideoPrefs().GetReflectionQuality())
                continue;
            dwWidth = dwRenderWidth;
            dwHeight = dwRenderHeight;
            break;
        case EFFECTBUFFER_COLOR:
        case EFFECTBUFFER_HIGHLIGHT:
            if (!GfxGetMSAAEnabled())
                continue;
            break;
        case EFFECTBUFFER_DEFAULT_SOFTPARTICLE_DEPTH:
            eDepthFormat = TEXTUREFORMAT_D24X8;
            dwWidth = 32;
            dwHeight = 32;
            break;
        case EFFECTBUFFER_HDR:
            if (!PostFXTestEnableHDR())
                continue;
            break;
        case EFFECTBUFFER_SSAO:
            if (GfxGetSystemID() != GFX_D3D && GfxGetSystemID() != GFX_D3D11 && GfxGetSystemID() != GFX_PS4GNM && GfxGetSystemID() != GFX_XBOXONED3D)
                continue;
            eFormat = TEXTUREFORMAT_A8;
            break;
        case EFFECTBUFFER_SSAO_PING:
        case EFFECTBUFFER_SSAO_PONG:
            if (GfxGetSystemID() != GFX_D3D && GfxGetSystemID() != GFX_D3D11 && GfxGetSystemID() != GFX_PS4GNM && GfxGetSystemID() != GFX_XBOXONED3D)
                continue;
            eFormat = TEXTUREFORMAT_A8;
            dwWidth = dwRenderWidth;
            dwHeight = dwRenderHeight;
            break;
        case EFFECTBUFFER_SSAO_NORMAL:
            if (GfxGetSystemID() != GFX_D3D && GfxGetSystemID() != GFX_D3D11 && GfxGetSystemID() != GFX_PS4GNM && GfxGetSystemID() != GFX_XBOXONED3D)
                continue;
            eFormat = TEXTUREFORMAT_ARGB8888;
            dwWidth = dwRenderWidth;
            dwHeight = dwRenderHeight;
            break;
        default:
            continue;
        }

        snoEffectBuffer[i] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(i),
            eFormat, eFormat, dwWidth, dwHeight, RESOURCEUSAGE_IMMUTABLE, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, nExponentBias);
        if (snoEffectBuffer[i] == SNO_NONE)
            return FALSE;

        if (eDepthFormat != TEXTUREFORMAT_UNKNOWN) {
            snoEffectDepthBuffer[i] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(i),
                eDepthFormat, eDepthFormat, dwWidth, dwHeight, RESOURCEUSAGE_IMMUTABLE, 2, dwMSAALevel, dwTargetERAMOffset, nExponentBias);
            if (snoEffectDepthBuffer[i] == SNO_NONE)
                return FALSE;
        }
    }

    GfxSetSwapChainRenderTarget(snoEffectBuffer[EFFECTBUFFER_BACKBUFFER]);
    SNO snoDefaultColorTarget = snoEffectBuffer[EFFECTBUFFER_BACKBUFFER];
    if (GfxGetMSAAEnabled())
        snoDefaultColorTarget = snoEffectBuffer[EFFECTBUFFER_COLOR];

    if (GfxGetMSAAEnabled()) {
        uint32 dwMSAALevel = 1;
        uint32 dwTargetERAMOffset = 0;
        TextureFormat eFormat = GfxGetDepthTextureBackbufferFormat();
        uint32 dwTexCreateFlags = 2;
        snoEffectBuffer[EFFECTBUFFER_BACKBUFFER_DEPTH] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(EFFECTBUFFER_BACKBUFFER_DEPTH),
            eFormat, eFormat, dwScreenWidth, dwScreenHeight, RESOURCEUSAGE_IMMUTABLE, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, 0);
        if (snoEffectBuffer[EFFECTBUFFER_BACKBUFFER_DEPTH] == SNO_NONE)
            return FALSE;
    }

    SNO snoDefaultDepthTarget;
    {
        uint32 dwMSAALevel = 1;
        uint32 dwTargetERAMOffset = 0;
        TextureFormat eFormat = GfxGetDepthTextureBackbufferFormat();
        uint32 dwTexCreateFlags = 2;
        snoEffectBuffer[EFFECTBUFFER_DEPTH] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(EFFECTBUFFER_DEPTH),
            eFormat, eFormat, dwScreenWidth, dwScreenHeight, RESOURCEUSAGE_IMMUTABLE, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, 0);
        if (snoEffectBuffer[EFFECTBUFFER_DEPTH] == SNO_NONE)
            return FALSE;

        if (GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D) {
            snoEffectBuffer[EFFECTBUFFER_DEPTH_PREV] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(EFFECTBUFFER_DEPTH_PREV),
                eFormat, eFormat, dwScreenWidth, dwScreenHeight, RESOURCEUSAGE_IMMUTABLE, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, 0);
            if (snoEffectBuffer[EFFECTBUFFER_DEPTH_PREV] == SNO_NONE)
                return FALSE;
        }

        if (GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D)
            snoDefaultDepthTarget = snoEffectBuffer[EFFECTBUFFER_DEPTH_PREV];
        else
            snoDefaultDepthTarget = snoEffectBuffer[EFFECTBUFFER_DEPTH];
    }

    snoTexDefaultColorTarget = snoDefaultColorTarget;
    snoTexDefaultDepthTarget = snoDefaultDepthTarget;

    if (GfxGetMSAAEnabled()) {
        uint32 dwMSAALevel = dwRenderMSAALevel;
        uint32 dwTargetERAMOffset = 0;
        TextureFormat eFormat = TEXTUREFORMAT_ARGB8888;
        uint32 dwTexCreateFlags = 1;
        snoEffectBuffer[EFFECTBUFFER_MSAA_COLOR] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(EFFECTBUFFER_MSAA_COLOR),
            eFormat, eFormat, dwScreenWidth, dwScreenHeight, RESOURCEUSAGE_IMMUTABLE, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, 0);
        if (snoEffectBuffer[EFFECTBUFFER_MSAA_COLOR] == SNO_NONE)
            return FALSE;
    }

    if (GfxGetMSAAEnabled()) {
        uint32 dwMSAALevel = dwRenderMSAALevel;
        uint32 dwTargetERAMOffset = 0;
        TextureFormat eFormat = GfxGetDepthTextureBackbufferFormat();
        uint32 dwTexCreateFlags = 2;
        snoEffectBuffer[EFFECTBUFFER_MSAA_DEPTH] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(EFFECTBUFFER_MSAA_DEPTH),
            eFormat, eFormat, dwScreenWidth, dwScreenHeight, RESOURCEUSAGE_IMMUTABLE, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, 0);
        if (snoEffectBuffer[EFFECTBUFFER_MSAA_DEPTH] == SNO_NONE)
            return FALSE;
    }

    {
        int nEffectsTexWidth = 1024;
        TextureFormat eFormat = TEXTUREFORMAT_ARGB8888;
        m_snoTexPMA = TextureDefinitionCreateTexture("PMA RT", eFormat, eFormat, nEffectsTexWidth, nEffectsTexWidth, RESOURCEUSAGE_IMMUTABLE, 1, 1, 0, 0);
    }

    if (Prefs::GetVideoPrefs().GetReflectionQuality() > 0) {
        uint32 dwWidth = dwRenderWidth;
        uint32 dwHeight = dwRenderHeight;
        uint32 dwMSAALevel = 1;
        TextureFormat eFormat = GfxGetDepthTextureBackbufferFormat();
        uint32 dwTexCreateFlags = 2;
        uint32 dwTargetERAMOffset = 0;
        snoEffectBuffer[EFFECTBUFFER_REFLECTION_DEPTH] = TextureDefinitionCreateTexture(PostFXGetEffectRTDebugName(EFFECTBUFFER_REFLECTION_DEPTH),
            eFormat, eFormat, dwWidth, dwHeight, RESOURCEUSAGE_IMMUTABLE, dwTexCreateFlags, dwMSAALevel, dwTargetERAMOffset, 0);
        if (snoEffectBuffer[EFFECTBUFFER_REFLECTION_DEPTH] == SNO_NONE)
            return FALSE;
    }

    return TRUE;
}

void RWindow::PostFXGfxDispose() {  // line 746
    gptGroupDefTexture->Purge(m_snoTexPMA, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
    m_snoTexPMA = SNO_NONE;
    for (int32 i = 0; i < NUM_EFFECT_BUFFERS; i++) {
        gptGroupDefTexture->Purge(snoEffectBuffer[i], FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
        gptGroupDefTexture->Purge(snoEffectDepthBuffer[i], FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
        snoEffectBuffer[i] = SNO_NONE;
        snoEffectDepthBuffer[i] = SNO_NONE;
    }
}

inline void RWindow::DisplayListsCompact() {  // line 970
    for (int32 i = 0; i < 2; i++) {
        tAsyncData[i].dispList.Compact();
        for (int nRefl = 0; nRefl < 2; nRefl++)
            tAsyncData[i].dispListReflections[nRefl].Compact();
        tAsyncData[i].dispListProjected.Compact();
    }
}

inline void RWindow::DisplayListsBegin() {  // line 986
    AsyncRWindowData & tAsync = tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    tAsync.dispList.RenderBegin();
    for (int nRefl = 0; nRefl < 2; nRefl++)
        tAsync.dispListReflections[nRefl].RenderBegin();
    tAsync.dispListProjected.RenderBegin();
}

inline void RWindow::DisplayListsSort() {  // line 1000
    AsyncRWindowData & tAsync = tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    tAsync.dispList.Sort();
    for (int nRefl = 0; nRefl < 2; nRefl++)
        tAsync.dispListReflections[nRefl].Sort();
    tAsync.dispListProjected.Sort();
}

void RWindow::DisplayListsEnd() {  // line 1015
    AsyncRWindowData & tAsync = tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];
    tAsync.dispList.RenderEnd();
    for (int nRefl = 0; nRefl < 2; nRefl++)
        tAsync.dispListReflections[nRefl].RenderEnd();
    tAsync.dispListProjected.RenderEnd();
}

void RWindow::SetRenderAndDepthTargetToDefaults() {  // line 1031
    GfxSetRenderTarget(GetDefaultColorTarget(), CUBEMAPFACE_POSX);
    GfxSetDepthTarget(GetDefaultDepthTarget());
}

RenderContext RWindow::GetRenderContext() const {  // line 1075
    const AsyncRWindowData & tAsyncRVData = tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    return tAsyncRVData.GetRenderContext();
}

void RWindow::SetLocalPlayerWorldPosition(const Vector3D & wpPosition, const BOOL fValid) {  // line 1085
    AsyncRWindowData & tAsyncRVData = tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    tAsyncRVData.wpLocalPlayerPosition = wpPosition;
    tAsyncRVData.fLocalPlayerValid = fValid;
}

BOOL RWindow::LocalPlayerWorldPositionIsValid() const {  // line 1097
    const AsyncRWindowData & tAsyncRVData = tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    return tAsyncRVData.fLocalPlayerValid;
}

const Vector3D & RWindow::GetLocalPlayerWorldPosition() const {  // line 1107
    const AsyncRWindowData & tAsyncRVData = tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    return tAsyncRVData.wpLocalPlayerPosition;
}

void MainRWindow::Present() const {  // line 1118
    GfxRenderPresent();
}

BOOL MainRWindow::ShouldRender() const {  // line 1127
    return CGameGetGameCommonData().GameModeGet() != GAMEMODE_HOLODECK;
}

void MainRWindow::BuildCameras() {  // line 1149
    const Vector3D & wpPlayerPos = GetLocalPlayerWorldPosition();

    SNO snoWorld = CGameGetCurrentWorld();
    Angle aRotation = WorldIsRotated(snoWorld) ? EIGHTH_CIRCLE : 0.0f;

    if (GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1) && LocalPlayerGet()->eTeam == TEAM_PLAYER_1)
        aRotation = Math::NormalizeAngle(aRotation + HALF_CIRCLE);

    CameraManagerBuildAdditiveCamera(GetPMACameraIndex(), wpPlayerPos, aRotation);
    CameraManagerBuildRippleMapCamera(6, wpPlayerPos);
}

SNO MainRWindow::GetCurrentWorld() const {  // line 1174
    return CGameGetCurrentWorld();
}

IVector2D MainRWindow::GetPixelDimensions() const {  // line 1183
    IVector2D vecResult;
    GfxGetCurrentDimensions((uint32 &)vecResult.x, (uint32 &)vecResult.y);
    return vecResult;
}

Vector2D MainRWindow::GetPixelDimensionsFloat() const {  // line 1195
    Vector2D vecResult;
    GfxGetCurrentFloatDimensions(vecResult.x, vecResult.y);
    return vecResult;
}

RWindowMgr::RWindowMgr() {  // line 1209
    m_ptCurrentRWindow = NULL;
}

RWindowMgr::~RWindowMgr() {  // line 1217
}

void RWindowMgr::AddRWindow(RWindow & tRWindow) {  // line 1225
    m_tRWindowArray.push_back(&tRWindow);
    if (m_tRWindowArray.size() == 1)
        ResetCurrentRWindowToDefault();
}

void RWindowMgr::DisplayListsBegin() {  // line 1239
    for (uint32 i = 0; i < m_tRWindowArray.size(); i++)
        m_tRWindowArray[i]->DisplayListsBegin();
}

void RWindowMgr::DisplayListsSort() {  // line 1250
    for (uint32 i = 0; i < m_tRWindowArray.size(); i++)
        m_tRWindowArray[i]->DisplayListsSort();
}

void RWindowMgr::DisplayListsEnd() {  // line 1262
    for (uint32 i = 0; i < m_tRWindowArray.size(); i++)
        m_tRWindowArray[i]->DisplayListsEnd();
}

void RWindowMgr::DisplayListsCompact() {  // line 1274
    for (uint32 i = 0; i < m_tRWindowArray.size(); i++)
        m_tRWindowArray[i]->DisplayListsCompact();
}

void RWindowMgr::RenderPresentAll() {  // line 1286
    for (uint32 i = 0; i < m_tRWindowArray.size(); i++) {
        if (m_tRWindowArray[i]->ShouldRender())
            m_tRWindowArray[i]->Present();
    }
}

RWindow * RWindowMgr::GetActiveRWindowForAudio() {  // line 1324
    return &GetRWindow(0);
}

RWindowMgr * GetRWindowMgr() {  // line 1355
    return CGameGet().ptRWindowMgr;
}
