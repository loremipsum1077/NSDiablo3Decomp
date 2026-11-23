// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/CGame.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "UserInterface/Interface/UIFade.h"
#include "UserInterface/Interface/UIFunctions.h"
#include "../Common/CommonTypes.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Sigma/RGBAColor.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Math/Frustum.h"
#include "../Sigma/Random.h"
#include "../Sigma/RefString.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Stopwatch.h"

struct AmbientSound;
struct AnimationGlobals;
struct AttachmentGlobals;
struct BNetChatGlobals;
struct BillboardGlobals;
struct CActTransitionGlobals;
struct CActorMovementGameGlobals;
struct CComplexEffectGlobals;
struct CDebugDrawGlobals;
struct CPlayerGlobals;
struct CPlayerInputGlobals;
struct CSceneCollisionGlobals;
struct CSceneGlobals;
struct CTimedEventGlobals;
struct CWorldGlobals;
struct CameraManager;
struct Cinematography;
class ClothManager;
struct CScene;
struct CWorld;
struct Cutscene;
struct CutsceneGlobals;
struct DebugTextDrawGlobals;
struct EffectGroupGlobals;
struct EnvironmentGlobals;
class ExplosionManager;
struct Light;
struct LightGlobals;
struct NetStat;
struct ParentObjectGlobals;
struct ParticleGlobals;
struct ParticleSystem;
struct PhysicsGlobals;
struct PostFXGlobals;
struct RActor;
struct RActorAnimGlobals;
struct RActorCollisionGlobals;
struct RActorGfxGlobals;
struct RActorGlobals;
struct RMessageGlobals;
struct Player;
struct IRect2D;
struct Rect2D;
class RWindowMgr;
struct RopeGlobals;
struct SceneUIGlobals;
struct SelectionGlobals;
struct ShakeGlobals;
struct SkyGlobals;
struct SpriteGlobals;
struct SubObjectGlobals;
struct TexAnimGlobals;
struct TrailGlobals;
struct UIGlobals;
struct VariableResRWindowData;
struct VideoPrefsToConfirm;
struct WaterGlobals;
struct WorldState;

typedef int32 CWorldID;

enum {
    CGAMEGLOBALS_DISABLE_DRAWING_BIT = 0,
    CGAMEGLOBALS_DISPLAY_MAIN_ACTOR_STATE_BIT = 1,
    CGAMEGLOBALS_GAME_INITIALIZED_BIT = 2,
    CGAMEGLOBALS_TRACE_ATTACHMENTS_BIT = 3,
    CGAMEGLOBALS_ADJUST_CLIENT_CLOCK_BIT = 4,
    CGAMEGLOBALS_PVP_NO_END_BIT = 5,
    CGAMEGLOBALS_LOAD_COMPLETED_BIT = 6,
    CGAMEGLOBALS_WORLD_CHANGED_BIT = 7,
    CGAMEGLOBALS_FREE_CRAFTING_BIT = 8,
    CGAMEGLOBALS_GAME_PAUSED_BIT = 9,
    CGAMEGLOBALS_GAME_SOFT_PAUSED_BIT = 10,
    CGAMEGLOBALS_CLOCK_SLIP_DISABLED_BIT = 11,
    CGAMEGLOBALS_IN_UPDATE_RENDER_BIT = 12,
    CGAMEGLOBALS_LOCALE_COMMON_MISSING_BIT = 13,
    CGAMEGLOBALS_KANAIS_CUBE_FX_CHEAT_BIT = 14,
};

enum GameDrawFlags {
    DRAW_MESH_MODE_BIT = 0,
    DRAW_MARKER_DATA = 1,
    DRAW_PATHFINDING_BIT = 2,
    DRAW_PATHFINDING_CLIENT_BIT = 3,
    DRAW_NAVMESH_BIT = 4,
    DRAW_BRAIN_BIT = 5,
    DRAW_PLAYER_STATUS_BIT = 6,
    DRAW_WALKABLE_BIT = 7,
    DRAW_SCENE_BOUNDS_BIT = 8,
    DRAW_OCTREE_BIT = 9,
    DRAW_NAVMESH_OPAQUE_BIT = 10,
};

enum CameraMode {
    CAMERAMODE_NORMAL = 0,
    CAMERAMODE_FREE_CAMERA = 1,
    CAMERAMODE_AV = 2,
    CAMERAMODE_TOP_DOWN = 3,
};

struct LoadingScreenData {  // size 0x40
    CRefString sOverrideTip;  // 0x00
    uint32 dwTotalElapsedTicks;  // 0x18
    uint32 dwLastRenderTick;  // 0x1c
    uint32 nCurrentTip;  // 0x20
    BOOL bDrawingInProgress;  // 0x24
    BOOL bDrawingBlackInProgress;  // 0x28
    BOOL bHostMigrationDisplayFirstTip;  // 0x2c
    BOOL bHostMigrationTips;  // 0x30
    BOOL bTipOverridden;  // 0x34
    int32 nMaxLocalHeroLevel;  // 0x38
};
static_assert(sizeof(LoadingScreenData) == 0x40, "LoadingScreenData");

enum PauseGameDelta {
    PAUSEGAMEDELTA_NO_CHANGE = 0,
    PAUSEGAMEDELTA_REQUEST_PAUSE = 1,
    PAUSEGAMEDELTA_REQUEST_UNPAUSE = 2,
};

enum SoftPauseGameDelta {
    SOFTPAUSEGAMEDELTA_NO_CHANGE = 0,
    SOFTPAUSEGAMEDELTA_REQUEST_PAUSE = 1,
    SOFTPAUSEGAMEDELTA_REQUEST_UNPAUSE = 2,
};

enum LoadingComponent {
    LSC_PLAYER_ENTER_KNOWN = 1,
    LSC_PLAYER_PRE_WARPED = 2,
    LSC_PLAYER_WARPED = 4,
    LSC_SNO_MANAGER = 8,
    LSC_TIME_OUT = 16,
    LSC_ALL = 31,
    LSC_MANDATORY = 25,
};

class CGameGlobals {  // size 0x170
public:
    uint32 dwFlags;  // 0x00
    uint32 dwMaxGameTime;  // 0x04
    uint32 dwPredictedServerTime;  // 0x08
    float flLastLocalGameTimeUpdate;  // 0x0c
    float flPredictedTimeDeltaDesired;  // 0x10
    float flPredictedTimeDeltaCurrent;  // 0x14
    float flLagCompensationTicks;  // 0x18
    int32 nClientLatencySmoothBufferTime;  // 0x1c
    uint32 dwClientTimeLastHeartBeat;  // 0x20
    RNG rngLocal;  // 0x24
    uint32 dwDrawFlags;  // 0x2c
    SNO snoCurrentWorld;  // 0x30
    CWorldID idCurrentCWorld;  // 0x34
    SWorldID idCurrentSWorld;  // 0x38
    BOOL bCanRunIdleLoop;  // 0x3c
    uint32 dwLoadingScreenDoneFrame;  // 0x40
    LoadingScreenData tLoadingScreenData;  // 0x48
    BOOL bPVPPaused;  // 0x88
    BOOL bInScriptedSequence;  // 0x8c
    BOOL bScriptedSequenceCancelled;  // 0x90
    Frustum tFadeFrustum;  // 0x94
    BOOL bFadeFrustumValid;  // 0xf4
    SNO snoLevelAreaSavePoint;  // 0xf8
    SNO snoLevelAreaHearthPortal;  // 0xfc
    BOOL bHearthPortalIsTieredRift;  // 0x100
    SNO snoLevelAreaReturnPoint;  // 0x104
    float flDungeonFinderProgress;  // 0x108
    int32 nDungeonFinderSeed;  // 0x10c
    int32 nDungeonFinderParticipatingPlayerCount;  // 0x110
    SNO snoDungeonFinderTimedEvent;  // 0x114
    int32 nChallengeRiftCompletionMilliseconds;  // 0x118
    CRefString sChallengeRiftCompletionMillisecondsHeroName;  // 0x120
    float nChallengeRiftSprintMilliseconds;  // 0x138
    VideoPrefsToConfirm * pdefFallbackPrefs;  // 0x140
    UIFunctionPtr pfnOnAccept;  // 0x148
    UIFunctionPtr pfnOnCancel;  // 0x150
    BOOL fHardReset;  // 0x158
    uint32 dwGfxStartTime;  // 0x15c
    PauseGameDelta ePauseGameDelta;  // 0x160
    int32 nPauseRefCount;  // 0x164
    SoftPauseGameDelta eSoftPauseGameDelta;  // 0x168
    void Init();
    CameraMode GetCameraMode() const;
    void SetCameraMode(const CameraMode _eCameraMode);
private:
    CameraMode eCameraMode;  // 0x16c
};
static_assert(sizeof(CGameGlobals) == 0x170, "CGameGlobals");

typedef int32 AmbientSoundID;

typedef int32 LightID;

struct CGame {  // size 0xc28
    GameClock tRealClock;  // 0x00
    uint32 dwRealTicks;  // 0x20
    uint32 dwRealClockTime;  // 0x24
    uint32 dwRealClockTicksElapsed;  // 0x28
    float flRealTime;  // 0x2c
    float flRealDeltaTime;  // 0x30
    GameClock tClientClock;  // 0x34
    uint32 dwClientTicks;  // 0x54
    uint32 dwClientClockTime;  // 0x58
    uint32 dwClientClockTicksElapsed;  // 0x5c
    float flClientTime;  // 0x60
    float flClientDeltaTime;  // 0x64
    GameParams tGameStartParams;  // 0x68
    GameParams tQueuedGameStartParams;  // 0x428
    BOOL bUpdateRigidBodyPhysics;  // 0x7e8
    BOOL bLastClientUpdate;  // 0x7ec
    BOOL bUpdateUIPositions;  // 0x7f0
    NetStat * ptNetStats;  // 0x7f8
    CGameGlobals * ptCGameGlobals;  // 0x800
    GameCommonData tGCD;  // 0x808
    Blizzard::Lock::Semaphore tClientUpdateLock;  // 0xa40
    SceneUIGlobals * ptSceneUIGlobals;  // 0xa68
    PhysicsGlobals * ptPhysicsGlobals;  // 0xa70
    BillboardGlobals * ptBillboardGlobals;  // 0xa78
    TChainedDataArray<Light, int> * ptLights;  // 0xa80
    TDataArray<Cutscene, int> * ptCutscenes;  // 0xa88
    LightGlobals * ptLightGlobals;  // 0xa90
    TChainedDataArray<RActor, RActorID> * ptRActors;  // 0xa98
    RActorGlobals * ptRActorGlobals;  // 0xaa0
    ClothManager * ptClothManager;  // 0xaa8
    ExplosionManager * ptExplosionManager;  // 0xab0
    WaterGlobals * ptWaterGlobals;  // 0xab8
    TrailGlobals * ptTrailGlobals;  // 0xac0
    RopeGlobals * ptRopeGlobals;  // 0xac8
    TDataArray<ParticleSystem, int> * ptParticleSystems;  // 0xad0
    ParticleGlobals * ptParticleGlobals;  // 0xad8
    Cinematography * ptCinematography;  // 0xae0
    TDataArray<AmbientSound, int> * ptAmbientSounds;  // 0xae8
    AttachmentGlobals * ptAttachmentGlobals;  // 0xaf0
    ParentObjectGlobals * ptParentObjectGlobals;  // 0xaf8
    RMessageGlobals * ptRMessageGlobals;  // 0xb00
    DebugTextDrawGlobals * ptDebugTextDrawGlobals;  // 0xb08
    CDebugDrawGlobals * ptDebugDrawGlobals;  // 0xb10
    TDataArray<CScene, int> * ptCScenes;  // 0xb18
    CSceneGlobals * ptCSceneGlobals;  // 0xb20
    CSceneCollisionGlobals * ptCSceneCollisionGlobals;  // 0xb28
    RActorCollisionGlobals * ptRActorCollisionGlobals;  // 0xb30
    RActorAnimGlobals * ptRActorAnimGlobals;  // 0xb38
    RActorGfxGlobals * ptRActorGfxGlobals;  // 0xb40
    SelectionGlobals * ptSelectionGlobals;  // 0xb48
    SkyGlobals * ptSkyGlobals;  // 0xb50
    EnvironmentGlobals * ptEnvGlobals;  // 0xb58
    AnimationGlobals * ptAnimGlobals;  // 0xb60
    SpriteGlobals * ptSpriteGlobals;  // 0xb68
    SubObjectGlobals * ptSubObjectGlobals;  // 0xb70
    RWindowMgr * ptRWindowMgr;  // 0xb78
    UIGlobals * ptUIGlobals;  // 0xb80
    CameraManager * ptCameraManager;  // 0xb88
    TDataArray<CWorld, int> * ptCWorlds;  // 0xb90
    CWorldGlobals * ptCWorldGlobals;  // 0xb98
    CPlayerGlobals * ptCPlayerGlobals;  // 0xba0
    CPlayerInputGlobals * ptCPlayerInputGlobals;  // 0xba8
    PostFXGlobals * ptPostFXGlobals;  // 0xbb0
    CutsceneGlobals * ptCutsceneGlobals;  // 0xbb8
    BNetChatGlobals * ptBNetGlobals;  // 0xbc0
    WorldState * ptWorldStateGlobals;  // 0xbc8
    ShakeGlobals * ptShakeGlobals;  // 0xbd0
    EffectGroupGlobals * ptEffectGroupGlobals;  // 0xbd8
    CComplexEffectGlobals * ptCComplexEffectGlobals;  // 0xbe0
    TexAnimGlobals * ptTexAnimGlobals;  // 0xbe8
    CTimedEventGlobals * ptCTimedEventGlobals;  // 0xbf0
    CActTransitionGlobals * ptCActTransitionGlobals;  // 0xbf8
    CActorMovementGameGlobals * ptCActorMovementGlobals;  // 0xc00
    Blizzard::Lock::Mutex tObjectUpdateGlobalLock;  // 0xc08
};
static_assert(sizeof(CGame) == 0xc28, "CGame");

enum {
    ASYNC_RENDER_DATA_UPDATE = 0,
    ASYNC_RENDER_DATA_DRAW = 1,
};

enum {
    ASYNC_RENDER_DEVICE_AVAILABLE = 0,
    ASYNC_RENDER_DEVICE_NOT_AVAILABLE = 1,
};

typedef int32 AsyncRenderDataIndex;

typedef int32 AsyncRenderDeviceStatus;

typedef void (*AsyncRenderCallback)(void *, void *);


// AppGlobals (X/Common/Game.h:940) and sgtAppGlobals (X/Common/Game.cpp:412) come from Game.h.




































inline CGame & CGameGet() {  // line 760
    AppGlobals & tAppGlobals = sgtAppGlobals;
    return *tAppGlobals.ptCGameActive;
}

inline BOOL CGameIsValid() {  // line 773
    return sgtAppGlobals.ptCGameActive != NULL;
}

inline uint32 CGameGetClientTicks() {  // line 785
    return CGameGet().dwClientTicks;
}

inline float CGameGetClientTime() {  // line 793
    return CGameGet().flClientTime;
}

inline float CGameGetClientDeltaTime() {  // line 801
    return CGameGet().flClientDeltaTime;
}

inline uint32 CGameGetRealTicks() {  // line 809
    return CGameGet().dwRealTicks;
}

inline float CGameGetRealTime() {  // line 818
    return CGameGet().flRealTime;
}

inline float CGameGetRealDeltaTime() {  // line 827
    return CGameGet().flRealDeltaTime;
}

// The functions X/Client/CGame.cpp defines for other files. The reference header does not list
// them; they are declared here in the .cpp's line order.
VariableResRWindowData * CGameVariableResInitializeForRWindow();
void CGameVariableResDisposeForRWindow(VariableResRWindowData * ptData);
void CGameVariableResRenderUpdate(VariableResRWindowData & tData);
void CGameVariableResUpdate(VariableResRWindowData & tData);
void CGameInitializeGame(const GameParams & tParams);
void CGameDisposeGame();
void CGameGlobalsInit(const GameParams & tParams);
void CGameGlobalsDispose();
void GameAspectRatioChanged();
float CGameGetLetterboxedViewportNDC(Rect2D & rectScreenNDC);
float CGameGetLetterboxedViewportNDC(const float flAspectRatio, Rect2D & rectScreenNDC);
void CGameGetLetterboxedViewportPixels(IRect2D & rectScreen);
void CGameGetLetterboxedViewportUIC(Rect2D & rectUIC);
void CGameGetSafeViewportUIC(Rect2D & rectUIC);
void CGameGfxInit();
void CGameGfxDispose();
BOOL sGameGfxInit(BOOL fSkipCGame);
void sGameGfxSystemDispose(BOOL fSkipCGame);
void sGameGfxSystemInit(BOOL fSkipCGame);
void GameGfxSystemInit();
void GameGfxInit();
void GameGfxDispose();
void GameGfxSystemDispose();
void GameReloadRequiredDefinitions();
void GameUnloadShaders();
void GamePause(BOOL bPause);
void GameRefPause(BOOL bPause);
void GameSoftPause(BOOL bPause);
void CancelAllChannelingAbilities();
BOOL GameIsPaused();
BOOL GameIsSoftPaused();
void sClientCheckClock(uint32 uTicksElapsed);
void CGameWriteQAPerfLogs();
BOOL CGameLoadingScreenCanProgress();
void CGameProgressLoadingScreen(BOOL bInTightLoop);
void CGameClientUpdateLock();
void CGameClientUpdateUnlock();
BOOL ClientIdle(uint32 dwGameClockTicks);
BOOL CGameLoadingScreenHasExpired();
BOOL CGameLoadingScreenIsEnabled();
BOOL CGameLoadingScreenIsBlackOnly();
void CGameLoadingScreenSetBlackOnly(BOOL bBlackOnly);
void CGameLoadingScreenEnable(const LoadingComponent eComponent);
void CGameLoadingScreenDisable(const LoadingComponent eComponent);
void CGameLoadingScreenScheduleFade(float flDuration, UIFade::FadeColor eFadeColor);
void CGameLoadingPartCompleted(const LoadingComponent eComponent);
void CGameLoadingPartRequired(const LoadingComponent eComponent);
void CGameServerLoadingPartCompleted(const LoadingComponent eComponent);
void CGameServerLoadingPartRequired(const LoadingComponent eComponent);
void GameSetDrawFlagsToDefaults();
SNO CGameGetCurrentWorld();
SWorldID CGameGetCurrentSWorldID();
GameCommonData & CGameGetGameCommonData();
CGameGlobals & CGameGlobalsGet();
CGameGlobals * CGameGlobalsTryToGet();
void CGameFirstClock(const uint32 dwFirstClock);
void CGameStartTick(uint32 dwTick);
void CGameReceiveHeartbeat(uint32 dwHeartBeatTime);
NetStat * CGameGetNetStat();
uint32 CGameGetPredictedServerTime();
BOOL CGameDisplayHealNumbers();
BOOL CGameDisplayDamageNumbers();
BOOL CGameDisplayDefensiveMessages();
BOOL CGameDisplayCriticals();
BOOL CGameInScriptedSequence();
DebugSoundMode CGameGetDebugSoundMode();
BOOL CGameTraceAttachmentsEnabled();
void CGameSetLoadComplete(Player * ptPlayer);
BOOL CGameAsyncRenderFlush();
void CGameAsyncRenderGPUFlush();
uint32 CGameAsyncGetDrawFrameCount();
uint32 CGameAsyncGetUpdateFrameCount();
uint32 CGameAsyncGetRenderDataIndex(AsyncRenderDataIndex eIndex);
BOOL CGameAsyncRenderRegisterDelayedCallback(AsyncRenderCallback callback, void * pParam1, void * pParam2);
void CGameAsyncRenderRegisterGPUResourceDelayedDeletion(AsyncRenderCallback callback, void * pParam1, void * pParam2);
void * CGameAsyncRenderGetMemory(uint32 dwSize);
void CGameAsyncRenderInit();
void CGameAsyncRenderDispose();
void ConsoleGfxInit();
void ConsoleGfxDispose();

// CGame.h:836 and :858. The bodies are not in the DWARF; they are written from the copies inlined
// into X/Common/Game.cpp (GameRandRangeInt, GameRandRangeFloat).
inline int32 ClientRandRangeInt(const int32 nMin, const int32 nMax) {  // line 836
    RNG & tRNG = CGameGlobalsGet().rngLocal;  // line 840
    return RandRange_Internal(tRNG, nMin, nMax - nMin);  // line 841
}
inline float ClientRandRangeFloat(const float flMin, const float flMax) {  // line 858
    return RandRangeFloat_Internal(CGameGlobalsGet().rngLocal, flMin, flMax - flMin);  // lines 862-863
}
