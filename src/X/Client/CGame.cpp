#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "CGame.h"
#include "Camera/Camera.h"
#include "Font/Font.h"
#include "Font/FormattedText.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxVB.h"
#include "Console/Online.h"
#include "Battlenet/Hub.h"
#include "RWindow.h"
#include "SubObjectGfx.h"
#include "Definitions/TextureDefinition.h"
#include "UserInterface/Interface/UIFade.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/SceneCommon.h"
#include "../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/Files/XFiles.h"
#include "../Sigma/GamePerf.h"
#include "../Common/Definitions/WorldDefinition.h"
#include "../Common/World.h"
#include "../Common/GlobalSNO.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/Math/Matrix.h"
#include "../Sigma/Math/Plane.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/SigmaTiming.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/UnicodeText.h"
#include "../Common/ThreadGlobals.h"
#include "../Common/Subsystem.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
// X/Client/Prefetch.cpp
namespace Prefetch {
BOOL HasPriorityPending(BOOL bPriority);
void CancelAllActivePrefetches();
void Update(BOOL bBlocking);
void EnableHighPriority(BOOL bEnable);
}  // namespace Prefetch
#include "Gfx/NX64/NX64NVN.h"





extern GFXNX64NVN * gptGFXAPI;
// X/Common/Game.cpp
BOOL GameIsNetworked();
BOOL GameIsStartUp();
BOOL AppIsDrawingWorld();
void AppDrawFlagSet(int32 nFlag, BOOL bSet);
// X/Client/SNOPreload.cpp and X/Common/SNO
void SNOPackSetInstallationSpeed(BOOL bFast);
namespace SNOPreload {
void InitializeForGame();
void DisposeForGame();
}  // namespace SNOPreload
namespace SNOManager {
void OnLoadingScreenCanStart();
BOOL DisplayLoadingScreen();
}  // namespace SNOManager
// X/Client/Sound/Sound.cpp
namespace SoundManager {
void ResetControllers();
void SuppressSounds(uint32 dwSoundTypes);
void DeleteAll(uint32 dwSoundTypes, RenderContext eContext);
}  // namespace SoundManager
namespace Vibration {
void DeleteAll();
}  // namespace Vibration
namespace UIFade {
void Open(float flDuration, FadeColor eFadeColor);
}  // namespace UIFade
// X/Common/Game.cpp
BOOL AppDrawFlagIsSet(int32 nFlag);
// The subsystem entry points listed in the tables below.
void ProfUIInitializeForGame(); void ProfUIDisposeForGame();
void GameBalanceInitializeForGame(); void GameBalanceDisposeForGame();
void PhysicsInitialize(); void PhysicsDispose();
void CTimedEventsInitializeForGame(); void CTimedEventsDisposeForGame();
void CActTransitionsInitializeForGame(); void CActTransitionsDisposeForGame();
void FastAttribInitializeForGame(); void FastAttribDisposeForGame();
void ParentObjectsInitialize(); void ParentObjectsDispose();
void AttachmentsInitialize(); void AttachmentsDispose();
void LightsInitializeForGame(); void LightsDisposeForGame();
void TrailsInitializeForGame(); void TrailsDisposeForGame();
void ParticlesInitializeForGame(); void ParticlesDisposeForGame();
void RopesInitializeForGame(); void RopesDisposeForGame();
void ShakeInitializeForGame(); void ShakeDisposeForGame();
void ActsInitializeForGame(); void ActsDisposeForGame();
void MarkersInitializeForGame(); void MarkersDisposeForGame();
void InactiveMarkerInitializeForGame(); void InactiveMarkerDisposeForGame();
void PortalsInitializeForGame(); void PortalsDisposeForGame();
void CScenesInitializeForGame(); void CScenesDisposeForGame();
void CWorldsInitializeForGame(); void CWorldsDisposeForGame();
void ActorInventoryInitializeForGame(); void ActorInventoryDisposeForGame();
void VisualInventoryInitializeForGame(); void VisualInventoryDisposeForGame();
void CActorMovementInitializeForGame(); void CActorMovementDisposeForGame();
void ItemsInitializeForGame(); void ItemsDisposeForGame();
void CACDInitializeForGame(); void ACDDisposeForGame();
void EffectGroupInitializeForGame(); void EffectGroupDisposeForGame();
void RActorsInitializeForGame(); void RActorsDisposeForGame();
void CutscenesInitializeForGame(); void CutscenesDisposeForGame();
void UI_InitializeForGame(); void UI_DisposeForGame();
void CinematographyInitialize(); void CinematographyDispose();
void RMessagesInitialize(); void RMessagesDispose();
void DebugTextInitialize(); void DebugTextDispose();
void CDebugDrawInitialize(); void CDebugDrawDispose();
void CComplexEffectInitializeForGame(); void CComplexEffectDisposeForGame();
void CNetworkInitializeForGame(); void CNetworkDisposeForGame();
void ClientMessageInitializeForGame(); void ClientMessageDisposeForGame();
void CPlayerInitializeForGame(); void CPlayerDisposeForGame();
void CPlayerInputInitializeForGame(); void CPlayerInputDisposeForGame();
void CTrickleInitializeForGame(); void CTrickleDisposeForGame();
void QuestsInitializeForGame(); void QuestsDisposeForGame();
void NavCellPathInitializeForGame(); void NavCellPathDisposeForGame();
void CTeamsInitializeForGame(); void TeamsDisposeForGame();
void CGameModeInitializeForGame(); void CGameModeDisposeForGame();
void CameraManagerInitializeForGame(); void CameraManagerDisposeForGame();
void SpritesInitialize(); void SpritesDispose();
void CGameGfxInit(); void CGameGfxDispose();
void CBlizzconTimerInitializeForGame(); void CBlizzconTimerDisposeForGame();
void CGameAsyncRenderInit(); void CGameAsyncRenderDispose();
void TexAnimInitializeForGame(); void TexAnimDisposeForGame();
void SubObjectGfxInitializeForGame(); void SubObjectGfxDisposeForGame();
void PostFXInitializeForGame(); void PostFXDisposeForGame();
void GameGfxSystemInit(); void GameGfxSystemDispose();
struct dmProfile;
class DominoWorld {
public:
    void Splash();
    void Update(float fTimeStep);
    void UpdateAsyncKick(float fTimeStep);
    void UpdateAsyncWait();
    void GetProfile(dmProfile * pProfile) const;
    static void InitializeForGame();
    static void DisposeForGame();
};
class ClothManager {
public:
    static void InitializeForGame();
    static void DisposeForGame();
    static void Update(float flDeltaTime, BOOL fFixedUpdateOnly);
    static void WaitForCompletion();
    static void CopyToAsyncVertices();
};
class ExplosionManager {
public:
    void Update(float flDeltaTime);
    static void InitializeForGame();
    static void DisposeForGame();
};
namespace SoundManager { void InitializeForGame(); void DisposeForGame(); }
namespace AmbientSounds { void InitializeForGame(); void DisposeForGame(); void Update(); }
namespace Vibration { void InitializeForGame(); void DisposeForGame(); void Update(); }
namespace ChromaEffects { void InitializeForGame(); void DisposeForGame(); void Update(); }
namespace UIStartScreen { namespace Console { void StartScreenInitializeForGame(); void StartScreenDisposeForGame(); } }
namespace Weather { void InitializeForGame(); void DisposeForGame(); }
namespace Sky { void InitializeForGame(); void DisposeForGame(); void Update(); }
namespace Wind { void InitializeForGame(); void DisposeForGame(); void Update(float flDeltaTime); }
namespace Console { namespace Screens { void InitializeForGame(); void DisposeForGame(); } }
namespace Console { namespace UIRActorManager { void InitializeForGame(); void DisposeForGame(); } }
namespace LocalPlayerClamping { void Init(); void Dispose(); }
namespace CHero { void InitializeForGame(); void DisposeForGame(); }
namespace Console { namespace HeroPublishing { void InitializeForGame(); void DisposeForGame(); } }
namespace Avenger { namespace Console { void InitializeForGame(); void DisposeForGame(); } }
#include "Definitions/PreferencesDefinition.h"





























struct VideoPrefs;
namespace Prefs {
const GameplayPrefs & GetGameplayPrefs();
void Restore();
VideoPrefs & GetMutableVideoPrefs();
const VideoPrefs & GetVideoPrefs();
}  // namespace Prefs

class LoadingScreenManager {  // size 0x40
public:
    LoadingScreenManager();
    void Enable();
    void Disable();
    void Completed(const LoadingComponent eComponent);
    void Required(const LoadingComponent eComponent);
    void ServerCompleted(const LoadingComponent eComponent);
    void ServerRequired(const LoadingComponent eComponent);
    BOOL IsBlackOnly() const;
    void SetBlackOnly(BOOL bBlackOnly);
    void Update();
    void UpdateStartFrame();
    uint32 GetAge() const;
    BOOL HasExpired() const;
    BOOL IsEnabled() const;
    void DrawStats();
    void ScheduleFade(float flDuration, UIFade::FadeColor eFadeColor);
    BOOL GetResetTooltip();
    void SetResetTooltip(BOOL bReset);
    uint32 GetLastTooltipChangeTime();
    void SetLastTooltipChangeTime(uint32);
    uint32 GetCurrentTip();
    void SetCurrentTip(uint32 nCurrentTip);
    static const uint32 CURRENT_TIP_NONE = 9999;
private:
    uint32 m_nState;  // 0x00
    uint32 m_nLastState;  // 0x04
    uint32 m_nLastEnabledFrame;  // 0x08
    uint32 m_nLastEnableBlackOnly;  // 0x0c
    uint32 m_nServerState;  // 0x10
    uint32 m_nServerLastEnabledFrame;  // 0x14
    uint32 m_nStartFrame;  // 0x18
    BOOL m_bExpired;  // 0x1c
    BOOL m_bBlackOnly;  // 0x20
    BOOL m_bFadeScheduled;  // 0x24
    float m_flDuration;  // 0x28
    UIFade::FadeColor m_eFadeColor;  // 0x2c
    BOOL m_bResetTooltip;  // 0x30
    uint32 m_msLastTooltipChangeTime;  // 0x34
    uint32 m_nCurrentTip;  // 0x38
    Blizzard::Time::Millisecond m_tStartTime;  // 0x3c
};

struct AsyncExtensionBuffer {  // size 0x20
    uint32 dwBufferOffset;  // 0x00
    uint32 dwBufferSize;  // 0x04
    uint8 * pData;  // 0x08
    uint32 dwLastUpdateFrameUsed;  // 0x10
    AsyncExtensionBuffer * ptNext;  // 0x18
};

class AsyncRender {  // size 0x10148
public:
    AsyncRender(uint32, uint32);
    ~AsyncRender();
    void TerminateThread();
    BOOL IsSynched() const;
    void Suspend();
    void Resume();
    BOOL Flush();
    void GPUFlush();
    void SwapRenderData(AsyncRenderDeviceStatus eStatus);
    void CancelFrame();
    void Kick(const BOOL bHiResScreenshotMode, const BOOL bFlushPrimitives);
    BOOL RegisterDelayedCallback(AsyncRenderCallback callback, void * pParam1, void * pParam2);
    void RegisterGPUResourceDelayedDeletion(AsyncRenderCallback callback, void * pParam1, void * pParam2);
    void ProcessDelayedCallbacks();
    void * GetMemory(uint32 dwSize);























































    BOOL IsUsingRenderWorkers() const { return m_bUsingRenderWorker; }
    uint32 GetDrawFrameCount();
    uint32 GetUpdateFrameCount();









    uint32 GetRenderDataIndex(AsyncRenderDataIndex eIndex) {








        return m_dwCurrentRenderDataIndex[eIndex];
    }
    struct AsyncRenderCallbackItem {  // size 0x18
        AsyncRenderCallback callback;  // 0x00
        void * pParam1;  // 0x08
        void * pParam2;  // 0x10
    };
    struct RenderData {  // size 0x50



        RenderData() {}
        ~RenderData() {}
        BOOL m_fCanceled;  // 0x00
        char * m_pMemoryBuffer;  // 0x08
        uint32 m_dwMemoryBufferOffset;  // 0x10
        AsyncExtensionBuffer * m_ptExtensionBuffers;  // 0x18
        AsyncExtensionBuffer * m_ptCurrentExtensionBuffer;  // 0x20
        blz::vector<AsyncRender::AsyncRenderCallbackItem, blz::allocator<AsyncRender::AsyncRenderCallbackItem> > m_arDelayedCallbackItem;  // 0x28
        Blizzard::Time::Microsecond m_tRenderTime;  // 0x40
        Blizzard::Time::Microsecond m_tGPUTime;  // 0x48
    };
    struct GPUResourceDelayedDeleteItem {  // size 0x20
        AsyncRenderCallback callback;  // 0x00
        void * pParam1;  // 0x08
        void * pParam2;  // 0x10
        uint32 uiFrameCount;  // 0x18
    };
    struct RenderWorkerData {  // size 0x10
        AsyncRender * ptAsyncRender;  // 0x00
        int32 nWorkerIndex;  // 0x08
    };
private:
    AsyncRender::GPUResourceDelayedDeleteItem m_pGPUResourceDelayedDeleteItem[2048];  // 0x00
    uint32 m_dwGPUResourceDelayedDeleteItemCount;  // 0x10000
    Blizzard::Lock::RMutex m_tGPUResourceDelayedCritical;  // 0x10008
    Blizzard::Lock::RMutex m_tResourceDelayedCritical;  // 0x10028
    Blizzard::Lock::REvent m_tRenderKickEvent[1];  // 0x10048
    Blizzard::Lock::REvent m_tRenderCompletionEvent[1];  // 0x10054
    Blizzard::Thread::Thread m_tThread[1];  // 0x10060
    AsyncRender::RenderWorkerData m_tWorkerData[1];  // 0x10068
    enum RenderOperation {
        RENDEROP_NONE = 0,
        RENDEROP_SUSPEND = 1,
        RENDEROP_RESUME = 2,
    };
    AsyncRender::RenderOperation m_eOperation;  // 0x10078
    BOOL m_bHiResScreenShotMode;  // 0x1007c
    BOOL m_bFlushPrimitives;  // 0x10080
    BOOL m_bUsingRenderWorker;  // 0x10084
    BOOL m_fDone;  // 0x10088
    BOOL m_fSynched;  // 0x1008c
    AsyncRender::RenderData tRenderData[2];  // 0x10090
    uint32 m_dwMemoryBufferSize;  // 0x10130
    uint32 m_dwExtensionBuffersSize;  // 0x10134
    uint32 m_dwDrawFrameCount;  // 0x10138
    uint32 m_dwCurrentRenderDataIndex[2];  // 0x1013c
    void AsyncRenderProc();
    static uint sAsyncRenderWorkerProc(void * pParam);
};
static_assert(sizeof(AsyncRender) == 0x10148, "AsyncRender");
























uint AsyncRender::sAsyncRenderWorkerProc(void * pParam) {


    RenderWorkerData * ptWorkerData = (RenderWorkerData *)pParam;
    if (ptWorkerData->nWorkerIndex == 0) {

        ptWorkerData->ptAsyncRender->AsyncRenderProc();
    }






    return 0;
}







inline LoadingScreenManager::LoadingScreenManager() {





    m_nState = m_nLastState = m_nLastEnabledFrame = m_nLastEnableBlackOnly = m_nServerState = m_nServerLastEnabledFrame = 0;




    m_bExpired = m_bBlackOnly = m_bFadeScheduled = FALSE;
    m_flDuration = 0.0f;
    m_eFadeColor = UIFade::FADE_COLOR_BLACK;


    m_nStartFrame = (uint32)-1;


    m_bResetTooltip = TRUE;
    m_msLastTooltipChangeTime = 0;
    m_nCurrentTip = CURRENT_TIP_NONE;
    m_tStartTime = 0;
}
inline void LoadingScreenManager::Enable() {

    Required(LSC_MANDATORY);
}

inline void LoadingScreenManager::Disable() {

    Completed(LSC_ALL);
}

inline void LoadingScreenManager::Completed(const LoadingComponent eComponent) {


    uint32 nOldState = m_nState;
    m_nState &= ~eComponent;
    if (nOldState != m_nState) {

        Update();
    }
}

inline void LoadingScreenManager::Required(const LoadingComponent eComponent) {


    uint32 nOldState = m_nState;
    m_nState |= eComponent;
    if (nOldState == m_nState)
        return;

    if (!m_bBlackOnly) {


        if (nOldState == 0 && (eComponent & ~(LSC_PLAYER_PRE_WARPED | LSC_PLAYER_WARPED)) == 0) {



            m_bBlackOnly = !Prefetch::HasPriorityPending(TRUE);
            if (m_bBlackOnly) {

                m_nLastEnableBlackOnly = Blizzard::Time::Milliseconds();
            }
        }
    } else {




        if (m_nState & LSC_PLAYER_ENTER_KNOWN) {

            m_bBlackOnly = FALSE;
        }
    }

    Update();
}


inline void LoadingScreenManager::ServerCompleted(const LoadingComponent eComponent) {


    m_nServerState &= ~eComponent;
}

inline void LoadingScreenManager::ServerRequired(const LoadingComponent eComponent) {


    m_bBlackOnly = FALSE;
    m_nServerState |= eComponent;
}

inline BOOL LoadingScreenManager::IsBlackOnly() const {


    return m_bBlackOnly;
}

inline void LoadingScreenManager::SetBlackOnly(BOOL bBlackOnly) {


    m_bBlackOnly = bBlackOnly;
}



void LoadingScreenManager::Update() {
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    if (!(tCGameGlobals.dwFlags & (1 << CGAMEGLOBALS_GAME_INITIALIZED_BIT)))
        return;



    uint32 dwCurrentMS = Blizzard::Time::Milliseconds();
    uint32 nFramesSinceTheLastLoadingScreen = dwCurrentMS - m_nLastEnabledFrame;
    uint32 nFramesSinceTheLastServerLoadingScreen = dwCurrentMS - m_nServerLastEnabledFrame;

    BOOL bKeepLoadingScreenForAFewFrames = nFramesSinceTheLastServerLoadingScreen < 2000 || nFramesSinceTheLastLoadingScreen < 64;


    if (dwCurrentMS - m_nLastEnableBlackOnly > 1000) {

        m_bBlackOnly = FALSE;
    }

    if (bKeepLoadingScreenForAFewFrames && m_nLastEnabledFrame) {

        m_nState |= LSC_TIME_OUT;
    } else {


        m_nState &= ~LSC_TIME_OUT;
    }
    // One condition: the original line table puts both tests on 593 (m_nState 593:29, m_bBlackOnly 593:33).
    if (m_nState && !m_bBlackOnly) {

        SNOManager::OnLoadingScreenCanStart();

        if (SNOManager::DisplayLoadingScreen()) {



            m_nState |= LSC_SNO_MANAGER;
        } else {













            m_nState &= ~LSC_SNO_MANAGER;
        }
    }

    if (m_nState & ~LSC_TIME_OUT) {

        m_nLastEnabledFrame = dwCurrentMS;
    }

    if (m_nServerState & ~LSC_TIME_OUT) {

        m_nServerLastEnabledFrame = dwCurrentMS;


        m_bBlackOnly = FALSE;
    }



    if (m_nState && GameIsPaused() && !GameIsStartUp()) {

        m_nState = 0;



    } // Not an else-if: original columns 642:14/30/40/54 put the parenthesised tests on one separate `if`.
    if ((m_nLastState == m_nState) && ((m_nState & ~LSC_TIME_OUT) == LSC_PLAYER_PRE_WARPED) &&
               (float)(Blizzard::Time::Milliseconds() - m_tStartTime) / 1000.0f >= 60.0f) {

        Completed(LSC_PLAYER_PRE_WARPED);
    }

    UpdateStartFrame();

    if (m_nState == 0 && m_nLastState != 0) {



        Prefetch::EnableHighPriority(TRUE);




        SNOPackSetInstallationSpeed(TRUE);


        SoundManager::SuppressSounds(0);

        tCGameGlobals.dwLoadingScreenDoneFrame = sgtAppGlobals.dwFrameCounter;

        m_bBlackOnly = FALSE;
    }





    if (m_nState != 0 && m_nLastState == 0) {





        m_tStartTime = Blizzard::Time::Milliseconds();

        SoundManager::SuppressSounds(~(uint32)6);

        Vibration::DeleteAll();
        Prefetch::EnableHighPriority(FALSE);
        m_bExpired = FALSE;

        SNOPackSetInstallationSpeed(FALSE);
    }





    if (m_nState == 0 && m_nServerState == 0) {

        if (m_bFadeScheduled) {

            UIFade::Open(m_flDuration, m_eFadeColor);
            m_bFadeScheduled = FALSE;
        }
    }

    m_nLastState = m_nState;
}

inline void LoadingScreenManager::UpdateStartFrame() {
    // No IsEnabled() inline in the DWARF; columns 708:13/22/25 are `m_nState || m_nServerState`.
    if (m_nState || m_nServerState) {

        uint32 nCurrentFrame = sgtAppGlobals.dwFrameCounter;
        if (m_nStartFrame > nCurrentFrame) {

            m_nStartFrame = nCurrentFrame;
        }
    } else {


        m_nStartFrame = (uint32)-1;
    }
}















inline BOOL LoadingScreenManager::IsEnabled() const {

    return m_nState != 0 || m_nServerState != 0;
}














inline void LoadingScreenManager::ScheduleFade(float flDuration, UIFade::FadeColor eFadeColor) {



    m_bFadeScheduled = TRUE;
    m_flDuration = flDuration;
    m_eFadeColor = eFadeColor;
    if (m_nState == 0) {

        Update();
    }
}

inline BOOL LoadingScreenManager::GetResetTooltip() {


    return m_bResetTooltip;
}

inline void LoadingScreenManager::SetResetTooltip(BOOL bReset) {


    m_bResetTooltip = bReset;
}

inline uint32 LoadingScreenManager::GetLastTooltipChangeTime() {


    return m_msLastTooltipChangeTime;
}

inline void LoadingScreenManager::SetLastTooltipChangeTime(uint32 msTime) {


    m_msLastTooltipChangeTime = msTime;
}

inline uint32 LoadingScreenManager::GetCurrentTip() {


    return m_nCurrentTip;
}

inline void LoadingScreenManager::SetCurrentTip(uint32 nCurrentTip) {


    m_nCurrentTip = nCurrentTip;
}


















































static SubsystemSetup sgtCGameGfxSubsystems[5] = {
    { "TexAnim", TexAnimInitializeForGame, TexAnimDisposeForGame },
    { "RWindowMgr", RWindowMgr::InitializeForGame, RWindowMgr::DisposeForGame },
    { "SubObjectGfx", SubObjectGfxInitializeForGame, SubObjectGfxDisposeForGame },
    { "PostFX", PostFXInitializeForGame, PostFXDisposeForGame },
    { "GameGfx", GameGfxSystemInit, GameGfxSystemDispose },
};











static int32 sgnNumCGameGfxSubsystems = 5;


static SubsystemSetup sgtCGameSubsystems[64] = {
    { "ProfUI", ProfUIInitializeForGame, ProfUIDisposeForGame },
    { "GameBalance", GameBalanceInitializeForGame, GameBalanceDisposeForGame },
    { "Physics", PhysicsInitialize, PhysicsDispose },
    { "DominoWorld", DominoWorld::InitializeForGame, DominoWorld::DisposeForGame },
    { "Cloth", ClothManager::InitializeForGame, ClothManager::DisposeForGame },
    { "Explosions", ExplosionManager::InitializeForGame, ExplosionManager::DisposeForGame },
    { "CTimedEvents", CTimedEventsInitializeForGame, CTimedEventsDisposeForGame },
    { "CActTransitions", CActTransitionsInitializeForGame, CActTransitionsDisposeForGame },
    { "FastAttrib", FastAttribInitializeForGame, FastAttribDisposeForGame },
    { "ParentObjects", ParentObjectsInitialize, ParentObjectsDispose },
    { "Attachments", AttachmentsInitialize, AttachmentsDispose },
    { "Lights", LightsInitializeForGame, LightsDisposeForGame },
    { "Trails", TrailsInitializeForGame, TrailsDisposeForGame },
    { "Particles", ParticlesInitializeForGame, ParticlesDisposeForGame },
    { "Sound", SoundManager::InitializeForGame, SoundManager::DisposeForGame },
    { "AmbientSounds", AmbientSounds::InitializeForGame, AmbientSounds::DisposeForGame },
    { "Rope", RopesInitializeForGame, RopesDisposeForGame },
    { "Shake", ShakeInitializeForGame, ShakeDisposeForGame },
    { "Vibration", Vibration::InitializeForGame, Vibration::DisposeForGame },
    { "ChromaEffects", ChromaEffects::InitializeForGame, ChromaEffects::DisposeForGame },
    { "Acts", ActsInitializeForGame, ActsDisposeForGame },
    { "Markers", MarkersInitializeForGame, MarkersDisposeForGame },
    { "InactiveMarkers", InactiveMarkerInitializeForGame, InactiveMarkerDisposeForGame },
    { "Portals", PortalsInitializeForGame, PortalsDisposeForGame },
    { "CScenes", CScenesInitializeForGame, CScenesDisposeForGame },
    { "CWorlds", CWorldsInitializeForGame, CWorldsDisposeForGame },
    { "ActorInventory", ActorInventoryInitializeForGame, ActorInventoryDisposeForGame },
    { "VisualInventory", VisualInventoryInitializeForGame, VisualInventoryDisposeForGame },
    { "CActorMovement", CActorMovementInitializeForGame, CActorMovementDisposeForGame },
    { "Items", ItemsInitializeForGame, ItemsDisposeForGame },
    { "ACD", CACDInitializeForGame, ACDDisposeForGame },
    { "EffectGroup", EffectGroupInitializeForGame, EffectGroupDisposeForGame },
    { "RActors", RActorsInitializeForGame, RActorsDisposeForGame },
    { "Cutscenes", CutscenesInitializeForGame, CutscenesDisposeForGame },
    { "UIStartScreen", UIStartScreen::Console::StartScreenInitializeForGame, UIStartScreen::Console::StartScreenDisposeForGame },
    { "UI", UI_InitializeForGame, UI_DisposeForGame },
    { "Cinematography", CinematographyInitialize, CinematographyDispose },
    { "Weather", Weather::InitializeForGame, Weather::DisposeForGame },
    { "RMessages", RMessagesInitialize, RMessagesDispose },
    { "DebugText", DebugTextInitialize, DebugTextDispose },
    { "CDebugDraw", CDebugDrawInitialize, CDebugDrawDispose },
    { "CComplexEffect", CComplexEffectInitializeForGame, CComplexEffectDisposeForGame },
    { "CNetwork", CNetworkInitializeForGame, CNetworkDisposeForGame },
    { "ClientMessage", ClientMessageInitializeForGame, ClientMessageDisposeForGame },
    { "CPlayer", CPlayerInitializeForGame, CPlayerDisposeForGame },
    { "CPlayerInput", CPlayerInputInitializeForGame, CPlayerInputDisposeForGame },
    { "CTrickle", CTrickleInitializeForGame, CTrickleDisposeForGame },
    { "Quests", QuestsInitializeForGame, QuestsDisposeForGame },
    { "NavCellPath", NavCellPathInitializeForGame, NavCellPathDisposeForGame },
    { "CTeams", CTeamsInitializeForGame, TeamsDisposeForGame },
    { "GameMode", CGameModeInitializeForGame, CGameModeDisposeForGame },
    { "CameraManager", CameraManagerInitializeForGame, CameraManagerDisposeForGame },
    { "Sky", Sky::InitializeForGame, Sky::DisposeForGame },
    { "Wind", Wind::InitializeForGame, Wind::DisposeForGame },
    { "Sprites", SpritesInitialize, SpritesDispose },
    { "CGameGfx", CGameGfxInit, CGameGfxDispose },
    { "CBlizzconTimer", CBlizzconTimerInitializeForGame, CBlizzconTimerDisposeForGame },
    { "GameAsyncRender", CGameAsyncRenderInit, CGameAsyncRenderDispose },
    { "ConsoleScreenManager", Console::Screens::InitializeForGame, Console::Screens::DisposeForGame },
    { "ConsoleUIRActorManager", Console::UIRActorManager::InitializeForGame, Console::UIRActorManager::DisposeForGame },
    { "LocalPlayerClamping", LocalPlayerClamping::Init, LocalPlayerClamping::Dispose },
    { "CHero", CHero::InitializeForGame, CHero::DisposeForGame },
    { "HeroPublishing", Console::HeroPublishing::InitializeForGame, Console::HeroPublishing::DisposeForGame },
    { "Avenger", Avenger::Console::InitializeForGame, Avenger::Console::DisposeForGame },
};



































static int32 sgnNumCGameSubsystems = 64;


static LoadingScreenManager s_tLoadingScreenManager;

class ScopedClientUpdateLock {  // size 0x1
public:
    ScopedClientUpdateLock();
    ~ScopedClientUpdateLock();
};
inline ScopedClientUpdateLock::ScopedClientUpdateLock() {
    CGameClientUpdateLock();
}
inline ScopedClientUpdateLock::~ScopedClientUpdateLock() {

    CGameClientUpdateUnlock();
}




static AsyncRender * sptAsyncRender = NULL;

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
struct UIID;
#include "../Common/Player.h"










// X/Client/Definitions/*Definition.cpp
// GroupDefinition comes from X/Common/Definitions/Definitions.h (through SceneCommon.h).
class FontGroupDef : public GroupDefinition {};
#include "Definitions/AppearanceDefinition.h"
extern FontGroupDef * gptGroupDefFont;
extern AppearanceGroupDef * gptGroupDefAppearance;
// X/Common/Shell/Shell.h:11 and :160
enum AppState {
    APPSTATE_STARTING = 0,
    APPSTATE_IDLE = 1,
    APPSTATE_FINISHED = 2,
};
extern AppState geAppState;
inline AppState SHELL_APP_STATE_GET() {
    return geAppState;
}
// X/Common/Shell/CmdLine.h:104. Only the test members are declared.
typedef int32 TestType;
struct CmdLineParams {
    uint8 _unadopted[0x6684];
    TestType eTestType;  // 0x6684
    CHAR szTestFilter[4096];  // 0x6688
    CHAR szTestListPath[4096];  // 0x7688
    CHAR szTestEntryPoint[4096];  // 0x8688
    uint32 dwMinFPSScreenshot;  // 0x9688
    int32 nTestWaitTime;  // 0x968c
    int32 nTestSampleTime;  // 0x9690
    int32 nTestDumpFreq;  // 0x9694
};
const CmdLineParams * CmdLineGetParams();
namespace GameTesting {
void SetTestType(TestType eTestType);
void SetDumpFPSFequency(uint32 dwFrequency);
void SetSampleTime(uint32 dwTime);
void SetWorldAdvanceWaitTime(uint32 dwTime);
void SetFilter(LPCSTR szFilter);
void SetListPath(LPCSTR szPath);
void SetEntryPoint(LPCSTR szEntryPoint);
void SetMinFPSToTriggerScreenshot(uint32 dwMinFPS);
}  // namespace GameTesting
// X/Sigma/Networking
#include "../Sigma/Networking/XNetAddress.h"





// X/Client/Network/Client.cpp
void ClientConnect(const XNetAddress & tAddress, void (*pfnOnConnected)());
// X/Client/CWorld.cpp
void ClientWorldCreate(SNO snoWorld, int32 nParam1, GameMode eMode, int32 nParam3, int32 nParam4, int32 nParam5, int32 nParam6, int32 nParam7, int32 nParam8, int32 nParam9);
void ClientWorldDispose(CWorldID idWorld);
// X/Client/CGameMode.cpp
void CGameModeDispose();
// X/Client/Cursor.cpp
class CursorMgr {
public:
    void ResetCursorState(BOOL bForce);
    void Draw() {}  // invented: an inline member that compiles to nothing; only the call to PlayerGetCursorMgr remains
};
CursorMgr & PlayerGetCursorMgr();
// X/Common/Definitions/Definitions.cpp
void DefinitionPurgeNonRequired();
// X/Client/UserInterface
void UI_GameInitialized();
void UI_AspectRatioChanged();
void UI_BeginDrawLoadingScreen();
void UI_DrawLoadingScreen(uint32 dwPercent);
void UI_DisplayLoadingScreenTooltip(BOOL bForce);
// X/Client/Camera/CameraManager.cpp
void CameraManagerSetAspectRatio(float flAspectRatio);
// X/Client/Gfx
void GfxApplyChangedPrefs();
BOOL GfxDeviceReset();
void GfxIBFreeAll(BOOL fHardReset);
void GfxVBFreeAll(BOOL fHardReset);
void GfxGetCurrentDimensions(uint32 & dwWidth, uint32 & dwHeight);
void GfxRectDrawDeferred(const Rect2D & rect, const RGBAColor & rgba, float flZ, RenderLayer eRenderLayer, int32 nParam);
BOOL PostFXGfxInit();
void PostFXGfxDispose();
BOOL SubObjectGfxInit();
void SubObjectGfxDispose();
BOOL ParticlesGfxInit();
void ParticlesGfxDispose();
BOOL CutscenesGfxInit();
void CutscenesGfxDispose();
BOOL GfxPrimitivesGfxInit();
void GfxPrimitivesGfxDispose();
BOOL RopesGfxInit();
void RopesGfxDispose();
BOOL TrailsGfxInit();
void TrailsGfxDispose();
BOOL SpritesGfxInit();
void SpritesGfxDispose();
BOOL InputGfxInit();
void InputGfxDispose();
BOOL UIMinimap_GfxInit();
void UIMinimap_GfxDispose();
// X/Client/Definitions/PreferencesDefinition.cpp
// X/Client/Cutscene.cpp
void CutsceneTryToCancel(const UIID & idDialog);
// X/Client/Gfx/Gfx.cpp
void GfxAcquireThreadOwnership();
void GfxReleaseThreadOwnership();
BOOL GfxHasPrefsChanged();
void GfxDeviceGetState(BOOL & fCanRender, BOOL & fMustBeReset);
BOOL GfxRenderPresentLastStatus(BOOL bWait);
void ParticleGfxAsyncSwapRender(AsyncRenderDeviceStatus eStatus);
void SpritesGfxAsyncSwapRender(AsyncRenderDeviceStatus eStatus);
void SubObjectGfxOcclusionHighlightingClearQueries();
void InputShowPlatformCursor(BOOL bShow, BOOL bForce);
// X/Client/Gfx/NX64 and X/Sigma/SigmaProf.cpp
void ConfigureRenderThread(Blizzard::Thread::Thread tThread, uint32 dwCore);
void SigmaProfRegisterThread(Blizzard::Thread::Thread tThread, LPCSTR szName, uint16 wFlags);
void SigmaProfUnregisterThread(Blizzard::Thread::Thread tThread);
// X/Client/Definitions/ShaderDefinition.cpp:? 
#include "Definitions/ShaderDefinition.h"
extern ShaderGroupDef * gptGroupDefShader;
// X/Client/SNO/SNOManager.cpp
namespace SNOManager {
void ReloadRequired(SNOGroup eGroup);
}  // namespace SNOManager
// X/Common/Definitions/StringList.cpp
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Client/UserInterface/Interface/UIOptionsConfirmation.cpp
namespace UIOptionsConfirmation {
void Show(LPCSTR szMessage, UIFunctionPtr pfnOnAccept, UIFunctionPtr pfnOnCancel);
void CancelAndClose();
}  // namespace UIOptionsConfirmation
// X/Client/Console/Notifications.cpp
namespace Console {
namespace Notifications {
BOOL HostMigrationLoadscreenTipEnabled();
}  // namespace Notifications
}  // namespace Console
// X/Client/RWindowMgr.cpp
RWindowMgr * GetRWindowMgr();
// X/Common/Definitions/StringList.cpp and X/Sigma/ErrorManager
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
void ErrorMessageBox(LPCSTR szMessage, LPCSTR szHeader);
// X/Common/GlobalSNO.cpp
SNO GlobalSNOGet(int32 eGlobalSNO);
// X/Client/UserInterface/UIDialogs.cpp
template <typename T, typename TREF> class XGrowableList;
XGrowableList<UIID, const UIID &> & UI_GetInGameDlgStack();
void UI_PopDialog(XGrowableList<UIID, const UIID &> & tStack, BOOL bImmediate);
// GameSetActiveGameCommonData: see X/Common/Game.h
// X/Client/Gfx/Gfx.cpp
float GfxGetAspectRatio();
BOOL GfxIsLetterboxed();
void GfxBlockUntilIdle();
void GfxNewFrame();
BOOL GfxDeviceIsSoftReset();
void GfxSetPrefsChanged();
void GfxForceDeviceReset(BOOL fHardReset);
void GfxInitializeLastState();
BOOL GfxDynamicStuffInit();
void GfxDynamicStuffDispose();
// X/Client/UserInterface/UIUtils.cpp
void ConvertNDCToPixels(const Rect2D & rectNDC, IRect2D & rectPixels);
void ConvertPixelsToUIC(const IRect2D & rectPixels, Rect2D & rectUIC);
void ConvertPixelsToUIC(const Vector2D & vecPixels, Vector2D & vecUIC);
// X/Client/UserInterface/UI.cpp
#include "UserInterface/UI.h"






UI & UIGet();
void UI_EndDrawLoadingScreen();
BOOL UI_GfxInit();
void UI_GfxDispose();
// X/Client/UserInterface/UIID.cpp
const UIID & GetUIIDNone();
// X/Common/Player.cpp
Player * PlayerGetFirstInGame();
Player * PlayerGetNextInGame(const Player * ptPlayer);
// X/Client/CPlayer.cpp
BOOL PlayerIsLocal(const Player * ptPlayer);
void CPlayerPlayLoadingAnim();
// Declarations for the render path (sDraw and the functions it inlines).
// X/Client/Cutscene.h:12
enum {
    CUTSCENEID_INVALID = -1,
};
typedef int32 CutsceneID;
// X/Client/RActors/RActors.h:278 and :596. Only the members CGame.cpp reads are declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    SSceneID idSScene;  // 0x04
    ACDID idACD;  // 0x08
    SNO snoActor;  // 0x0c
    SNO snoMonster;  // 0x10
    ActorLocation tLocation;  // 0x18
    uint8 _unadopted[0x410 - 0x88];  // 0x88 (invented name: transformClient .. end)
};
static_assert(sizeof(RActor) == 0x410, "RActor");

inline RActor * RActorTryToGet(const RActorID idRActor) {  // line 596
    if (idRActor == RACTORID_INVALID)
        return NULL;
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return NULL;
    return tCGame.ptRActors->ElementTryToGet(idRActor);
}

// X/Client/Lights.h:26 and :171. Only the members CGame.cpp reads are declared.
struct Light {  // size 0x170
    LightID id;  // 0x00
    SNO sno;  // 0x04
    uint32 dwFlags;  // 0x08
    float flTimeElapsed;  // 0x0c
    LightState tState;  // 0x10
    uint8 _unadopted[0x170 - 0x98];  // 0x98 (invented name: fInnerRadiusInterpolationFraction .. end)
};
static_assert(sizeof(Light) == 0x170, "Light");

inline Light * LightTryToGet(LightID id) {  // line 171
    return CGameGet().ptLights->ElementTryToGet(id);
}

// X/Client/Scene.h and X/Client/SceneAttachments.cpp
typedef int32 CSceneID;
struct CScene {
    CSceneID id;  // 0x00
};
BOOL CSceneDataArrayIterate(TDataArrayIterator<CScene> & tIterator, const CGame & tCGame, CSceneDataArrayIterateMode eIterateMode);
BOOL CSceneGetHardpointTransformByLabel(const CSceneID idCScene, HardpointHash szHardpointLabel, PRTransform & transform);
void CScenesRenderUpdate(RWindow & tRWindow, const float flDeltaTime, const float flClientTime);
// X/Client/Weather.cpp
namespace Weather {
void PrepareRender(WeatherRWindowData & tData);
RActorID GetLitPlayer(const WeatherRWindowData & tData);
LightID GetSunLightScene(const WeatherRWindowData & tData);
LightID GetLocalPlayerHeadlight(const WeatherRWindowData & tData);
}  // namespace Weather
// X/Client/Camera/CameraManager.cpp
const Camera & CameraManagerGetCamera(const int32 nCameraIndex);
void CameraManagerBuildShadowCamera(const int32 nCameraIndex, const Vector3D & targetPos);
void CameraManagerPrepareRender();
void CameraManagerSetHighResScreenshotMode(const uint32 dwTile, const uint32 dwTilesPerRow);
// X/Client/Lights.cpp, X/Client/Particle.cpp, X/Client/RActors/RActorGfx.cpp and friends
void LightsPrepareRender();
void LightsRenderUpdate(RWindow & tRWindow);
void ParticlesRenderUpdate(RWindow & tRWindow, const float flDeltaTime);
void ParticlesMultiRenderUpdateFinish();
void RActorGfxRenderUpdate(RWindow & tRWindow, const float flDeltaTime, const float flClientTime);
void CGameModeRenderUpdate(RWindow & tRWindow);
void RopeRenderUpdate(RWindow & tRWindow);
void TrailsRenderUpdate(RWindow & tRWindow);
void PostFXRenderUpdate();
void PostFXSetFrameFlag(const uint32 dwBit, const BOOL fValue);
void CutscenesRenderUpdate();
CutsceneID CutsceneGetFullScreenCutscene();
void CPlayerRenderUpdate();
BOOL CPlayerIsGhosted();
void BuildLockerDrawWatermark();
const char * BuildConfigStr();
void UI_PreDraw();
void UI_Draw();
BOOL UI_DrawingFullscreenUI();
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
typedef int32 BackgroundHeroSlot;
class FrontendBackgroundHeroManager {
public:
    BOOL IsBackgroundHeroDead(BackgroundHeroSlot eSlot) const;
};
namespace Console {
namespace Screens {
FrontendBackgroundHeroManager & GetFrontendBackgroundHeroManager();
}  // namespace Screens
}  // namespace Console
// X/Client/Physics/DominoClient.cpp (the class is declared with the subsystem entry points)
extern DominoWorld * gptPhysicsAPIImpl;
// X/Sigma/RGBAColor.cpp and X/Client/Font
extern const RGBAColor crgbaTextShadow;
// X/Sigma/Files/XFilesCore.cpp, X/Common/Sno/SNOFiles.cpp and X/Client/Gfx
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
BOOL FileDirectoryCreate(FileReference & tFileRef, const BOOL fCreateSubDirs);
BOOL FileExists(const FileReference & tFileRef);
CRefString SNOFilesGetUserDataPath(LPCSTR szBranchName);
void GfxScreenshot(Pixel32 *& lpBuffer, uint32 & dwWidth, uint32 & dwHeight, uint32 & dwPitch);
void TGAExportTexture(FileReference & tRef, const uint32 dwWidth, const uint32 dwHeight, const Pixel32 * ptTile, const uint32 dwTileX, const uint32 dwTileY, const uint32 dwTileWidth, const uint32 dwTileHeight);
// Declarations for ClientIdle and the functions it inlines.
// X/Client/Input/Input.h:263



BOOL InputIsCursorInWindow();
BOOL InputAppHasCapture();
void InputCursorSet(CursorTypes eCursorType);
void InputUpdate();
void InputResetMouseWheelDelta();
// X/Common/Game.cpp
BOOL GameIsServerOnly();
BOOL GameLoopRunning();
BOOL GameClockDebugLag(uint32 dwGameClockTicks, GameClock & tGameClock);
void GameClockAdvanceStart(GameClock & tGameClock, const uint32 dwGameClockTicks, uint32 & dwClockTicksToUse, uint32 & dwGameTicksToUse, const uint32 dwTicksPerSecond);
void GameClockAdvanceComplete(GameClock & tGameClock, const BOOL bResetGameClockTime, const uint32 dwUsedClockTicks, const uint32 dwUsedGameTicks, const uint32 dwTicksPerSecond);
BOOL GameIsWeeklyChallenge();
namespace NephalemRift {
BOOL IsTieredRiftActive();
}  // namespace NephalemRift
// X/Client/SNOManager.cpp
namespace SNOManager {
void Update();
BOOL HasBlockedLastFrame();
}  // namespace SNOManager
// X/Client/Network
void CNetworkReadUpdate();
void ClientSendOutgoingMessages();
void ClientProcessIncomingMessagesForCurrentTick();
// X/Client/UserInterface/Console/UISaveIcon.cpp and X/Client/Debug
namespace Console {
namespace UISaveIcon {
void Update();
}  // namespace UISaveIcon
void UpdateControllerDebugView();
void UpdateTargetDebugging();
}  // namespace Console
// X/Sigma/Memory/SigmaMemory.cpp
BOOL SigmaMemorySetTraceAllocations(BOOL bEnable);
// X/Client/CPlayer.cpp and X/Client/CPlayerInput.cpp
void PlayerInputUpdate(Player * ptPlayer);
void CPlayersUpdate();
void CPlayersFixedUpdate();
RActorID LocalPlayerRActorGet();
ACDID LocalPlayerACDGet();
Player * LocalPlayerGet();
BOOL PlayersAreAllLocal();
void CPlayerInputClearCachedPower(Player * ptPlayer);
void CPlayerLocalPlayerJoin(int32 nControllerIndex);
// X/Client/RActors and friends
SSceneID RActorGetSSceneID(const RActorID idRActor);
void RActorGetCenterPoint(const RActor * ptRActor, Vector3D & wpCenter);
void RActorsUpdateACD();
void RActorsUpdateFullscreenParticleSystems();
void RActorsCollisionUpdate();
void RActorUpdateQueued();
void ParticleGlobalsUpdate();
void ParticlesUpdateFullscreenParticles();
void ParticleSystemDisturbFoliageKick();
void ParticleSystemDisturbFoliageWait();
void ParentObjectsUpdate(float flDeltaTime);
void ParentObjectsFixedUpdate(float flDeltaTime);
void AttachmentsUpdate();
void CComplexEffectUpdate();
void TrailsClientUpdate(float flDeltaTime);
void TrailsUpdate();
void LightsUpdate();
void CWorldUpdate();
void CBlizzconTimerUpdate();
void CActivationUpdate();
void SceneResetRActorFadeGroups();
void CScenesDeleteAllFlagged();
void CutscenesUpdate();
void ShakeUpdate(RWindow & tRWindow, float flDeltaTime);
void UI_Update(const float flDeltaTime);
void CinematographyUpdate(float flDeltaTime, const BOOL fForceUpdateOfAllParams);
void CameraManagerRestoreCurrentCameraToMainCamera();
namespace Weather {
void Update(RWindow & tRWindow, float flDeltaTime, const BOOL fDeleteInsteadOfOutro);
}  // namespace Weather
// X/Common/SceneCommon.cpp and X/Common/Sno/SNOFiles.cpp
SceneCommonData * SceneCommonDataArrayIterate(SceneCommonDataIterator & tSCDIterator, SceneCommonData *& ptSCD, CSceneDataArrayIterateMode eIterateMode);
CRefString SNOToString(const SNOGroup snoGroup, const SNO snoHandle, const BOOL fUsedForXMLFields);
// X/Sigma/Math
extern const Vector3D cgwvZAxis;
#include "../Common/Networking/GameMessage.h"




// Declarations for AsyncRender::AsyncRenderProc and the render draw it inlines.
// X/Client/Gfx/GfxAPI.h:36 and :39
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
GfxAPI GfxGetSystemID();
void GfxRenderSuspend();
void GfxRenderResume();
BOOL GfxGetMSAAEnabled();
BOOL GfxRenderBegin();
void GfxRenderEnd();
void GfxResetStates();
void GfxBeginWorker(int32 nWorkerIndex);
void GfxEndWorker();
void GfxSetRenderTargetToSwapChainRT();
void GfxSetDepthTarget(const SNO sno);
void GfxRenderClear(BOOL fClearTarget, BOOL fClearZBuffer, BOOL fClearStencilBuffer, const RGBAColor & rgbaTargetColor);
void GfxViewportSet(const IRect2D & rectViewport);
void GfxFinalizePerformanceStats();
SNO PostFXGetBuffer(const EffectBuffer eBuffer);
BOOL AppRenderLayerIsHidden(int32 nFlag);
void SpritesDraw(const RenderLayer eRenderLayer);
void SpritesFlush();
void Gfx2DPrimitivesDraw(const RenderLayer eRenderLayer);
void GfxPrimitivesFlush();
void CutscenesRenderFullscreen();
const CameraState & CameraManagerGetCameraState(const int32 nCameraIndex);
void CameraManagerSetCurrentCamera(const int32 nCameraIndex);
void TraceBootStop();
void SigmaProfInitializeForThread();
void SigmaProfDisposeForThread();
void SigmaProfNewFrame(Blizzard::Thread::Thread ptThread);
namespace Console {
namespace IO {
void InitializeForThread();
void DisposeForThread();
}  // namespace IO
}  // namespace Console
// Declarations for CGameWriteQAPerfLogs (the Gfx.h stats types come in through SubObjectGfx.h).






















GfxPerfStats * GfxGetPerfStats();
#include "Particle.h"








void ParticlesGetCount(ParticleCounts & tParticleCounts);
#include "../../Contrib/Contrib/Domino/Include/dmStructures.h"























static_assert(sizeof(dmProfile) == 0x54, "dmProfile");
// X/Common/Networking/GameMessage.h:688 and :760







// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/Network/Client.cpp
void ClientReceiveHeartbeat(uint32 dwHeartBeatTime);
// X/Client/Network/NetStat.cpp
void NetStatClearList();
// X/Client/Camera/CameraManager.cpp
void CameraManagerResetCameras();
// X/Client/Camera/Cinematography.cpp
enum CinematographyState {
    CINEMATOGRAPHY_STATE_3 = 3,  // invented name; the value CGameGlobals::SetCameraMode passes
};
void CinematographyStop();
void CinematographySetState(CinematographyState eState);
// X/Client/CGameMode.cpp
void CGameModeResetCameraToFollowPlayer();
// X/Sigma/GamePerf.cpp
void GamePerfNewFrame();
// X/Common/Shell/ShellNX64.cpp
void ShellEventLoop();
// X/Client/Sound/Sound.cpp
namespace SoundManager {
void Update();
}  // namespace SoundManager
// X/Client/Gfx/Gfx.cpp:86
extern const Rect2D cgrectViewportDefault;
// X/Client/Gfx/GfxCommon.h:456
static const float MAX_RESOLUTION_ASPECT_RATIO = 16.0f / 9.0f;
static void sInitHighResScreenshot(FileReference & tRefHighResScreenshot, uint32 & dwScreenShotTiles, uint32 & dwScreenShotTilesSq) {
    static uint32 dwNextScreenshot = 0;
    dwScreenShotTiles = 1;
    if (AppDrawFlagIsSet(91)) {
        AppDrawFlagSet(22, TRUE);
        FileReference tRefDir;
        CRefString sDirName = SNOFilesGetUserDataPath(NULL) + CRefString("\\Screenshots");
        FileReferenceInit(tRefDir, sDirName.str());
        FileDirectoryCreate(tRefDir, TRUE);
        BOOL fFileExists;
        do {
            CRefString sFilename = SNOFilesGetUserDataPath(NULL) + CRefString("\\Screenshots\\") + CRefString(CRefString().Format("HiResScreenshot%03d.tga", dwNextScreenshot));
            FileReferenceInit(tRefHighResScreenshot, sFilename);
            fFileExists = FileExists(tRefHighResScreenshot);
            dwNextScreenshot++;
        } while (fFileExists);
        dwScreenShotTiles = 3;
    }
    dwScreenShotTilesSq = dwScreenShotTiles * dwScreenShotTiles;
}
static void sPerTileInitHighResScreenshot(const uint32 dwTile, const uint32 dwScreenShotTiles) {
    CameraManagerSetHighResScreenshotMode(dwTile, dwScreenShotTiles);
}
static void sPerTileFinishHighResScreenshot(FileReference & tRefHighResScreenshot, const uint32 dwTile, const uint32 dwScreenShotTiles) {
    uint32 dwPitch;
    uint32 dwHeight;
    uint32 dwWidth;
    Pixel32 * lpBuffer;
    GfxScreenshot(lpBuffer, dwWidth, dwHeight, dwPitch);
    Pixel32 * lpBufferOffset = lpBuffer;
    TGAExportTexture(tRefHighResScreenshot, dwWidth * dwScreenShotTiles, dwHeight * dwScreenShotTiles, lpBufferOffset,
                     dwTile % dwScreenShotTiles, dwTile / dwScreenShotTiles, dwWidth, dwHeight);
    void * lpVoidBuffer = lpBuffer;
    gptGroupDefTexture->MemFree(lpVoidBuffer);
}
static void sFinishHighResScreenshot() {
    if (AppDrawFlagIsSet(91)) {
        CameraManagerSetHighResScreenshotMode(0, 0);
        AppDrawFlagSet(22, FALSE);
        AppDrawFlagSet(91, FALSE);
    }
}

struct VariableResRWindowData {  // size 0x44
    uint32 dwMinUs;  // 0x00
    uint32 dwMaxUs;  // 0x04
    uint32 dwAdjustCooldown;  // 0x08
    float flMinPercent;  // 0x0c
    float flMaxPercent;  // 0x10
    uint32 dwVariableResFrameIndex;  // 0x14
    float flPercent;  // 0x18
    float flPercentIncr;  // 0x1c
    uint32 dwAvgHistoryUs;  // 0x20
    uint32 VariableResHistoryUs[8];  // 0x24
};

enum FrameRateBoundTypes {
    FRAMERATEBOUNDTYPE_MAINTHREAD = 0,
    FRAMERATEBOUNDTYPE_RENDERTHREAD = 1,
    FRAMERATEBOUNDTYPE_GPU = 2,
    NUM_FRAMERATEBOUNDTYPES = 3,
};
VariableResRWindowData * CGameVariableResInitializeForRWindow() {
    VariableResRWindowData * ptVariableResRWindowData = new (SigmaMemoryNew(sizeof(VariableResRWindowData), 4, NULL, TRUE)) VariableResRWindowData();
    ptVariableResRWindowData->dwMinUs = 14250;
    ptVariableResRWindowData->dwMaxUs = 15250;
    ptVariableResRWindowData->dwAdjustCooldown = 8;
    ptVariableResRWindowData->flMinPercent = 0.7f;
    ptVariableResRWindowData->flMaxPercent = 1.0f;
    ptVariableResRWindowData->dwVariableResFrameIndex = 0;
    ptVariableResRWindowData->flPercent = 1.0f;
    ptVariableResRWindowData->flPercentIncr = 0.05f;

    return ptVariableResRWindowData;
}
void CGameVariableResDisposeForRWindow(VariableResRWindowData * ptData) {
    delete ptData;
}
void CGameVariableResRenderUpdate(VariableResRWindowData & tData) {
    if (AppDrawFlagIsSet(122)) {
        TextRenderProperties tProps(GlobalSNOGet(637), 13, 1.0f, 1.0f);
        CRefString strInfo;
        strInfo.Format("Variable Res Percent: %4.2f, GPU Profile: %d", tData.flPercent, tData.dwAvgHistoryUs);
        Vector2D vecUIC;
        {
            const RGBAColor rgbaText = crgbaWhite;
            Vector2D vPos(100.0f, 150.0f);
            ConvertPixelsToUIC(vPos, vecUIC);
        }
        FontStringDrawUIC(tProps, FormattedText(strInfo, 5), vecUIC, crgbaWhite, NULL, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    }
}
inline BOOL CGameVariableResEnabled() {
    return !AppDrawFlagIsSet(121);
}
void CGameVariableResUpdate(VariableResRWindowData & tData) {
    tData.VariableResHistoryUs[tData.dwVariableResFrameIndex % 8] = gptGFXAPI->DeviceFrameTimeUs();
    tData.dwVariableResFrameIndex++;
    uint32 dwCount = 0;
    uint32 dwTotalUs = 0;
    for (uint32 i = 0; i < tData.dwVariableResFrameIndex && i < 8; i++) {
        dwTotalUs += tData.VariableResHistoryUs[i];
        dwCount++;
    }
    tData.dwAvgHistoryUs = dwTotalUs / Math::Max(dwCount, 1u);
    if (CGameVariableResEnabled()) {
        if (tData.dwAdjustCooldown == 0) {
            if (tData.dwAvgHistoryUs < tData.dwMinUs) {
                tData.flPercent += tData.flPercentIncr;
                tData.dwAdjustCooldown = 8;
            } else if (tData.dwAvgHistoryUs > tData.dwMaxUs) {
                tData.flPercent -= tData.flPercentIncr;
                tData.dwAdjustCooldown = 8;
            }
            tData.flPercent = Math::Pin(tData.flPercent, tData.flMinPercent, tData.flMaxPercent);
        } else {
            tData.dwAdjustCooldown--;
        }
    } else {
        tData.flPercent = 1.0f;
        tData.dwAdjustCooldown = 0;
    }
}
static BOOL sCheckForGfxResetFallback();

static BOOL sCheckGfxReady(BOOL fProcessReset) {
    BOOL fCanRender;
    BOOL fMustBeReset;
    if (!fProcessReset && GfxHasPrefsChanged())
        return FALSE;
    GfxApplyChangedPrefs();
    GfxDeviceGetState(fCanRender, fMustBeReset);
    if (!GfxRenderPresentLastStatus(TRUE))
        return FALSE;
    if (fMustBeReset) {
        if (!fProcessReset)
            return FALSE;
        CGameAsyncRenderFlush();
        CGameAsyncRenderGPUFlush();
        GameUnloadShaders();
        GameGfxDispose();
        SubObjectGfxOcclusionHighlightingClearQueries();
        if (!GfxDeviceReset()) {
            GfxForceDeviceReset(TRUE);
            GameGfxDispose();
            if (!GfxDeviceReset())
                return FALSE;
        }
        GameGfxInit();
        GameReloadRequiredDefinitions();
        CGameGlobals & tCGameGlobals = CGameGlobalsGet();
        tCGameGlobals.dwGfxStartTime = GetMachineTicks();
    }
    if (!fCanRender) {
        InputShowPlatformCursor(TRUE, FALSE);
        return FALSE;
    }
    if (sCheckForGfxResetFallback())
        return FALSE;
    CGame & tClientGame = CGameGet();
    if (tClientGame.bUpdateUIPositions) {
        GameAspectRatioChanged();
    }
    return TRUE;
}
static void sDrawUIRenderLayer(const RenderLayer eRenderLayer) {
    if (AppRenderLayerIsHidden(eRenderLayer))
        return;
    SpritesDraw(eRenderLayer);
    Gfx2DPrimitivesDraw(eRenderLayer);
}

































































































































































static Blizzard::Time::Millisecond s_tLastTime;
static float s_frameTimeFiltered = 33.3f;
static BOOL s_fSlippedClock;

static FrameRateBoundTypes s_tRenderFrameRateBoundType = FRAMERATEBOUNDTYPE_GPU;

static FrameRateBoundTypes s_tFrameRateBoundType;

static int32 s_tFrameRateBoundTypeCounters[NUM_FRAMERATEBOUNDTYPES];










static const char * sFrameRateBoundTypeToString(FrameRateBoundTypes tFrameRateBoundType) {


    switch (tFrameRateBoundType) {
        case FRAMERATEBOUNDTYPE_MAINTHREAD:
            return "Main Thread";
        case FRAMERATEBOUNDTYPE_RENDERTHREAD:
            return "Render Thread";
        case FRAMERATEBOUNDTYPE_GPU:
            return "GPU";
    }
    return "";
}




float s_nReleaseFPSPosX = -180.0f;
float s_nReleaseFPSPosY = 290.0f;
float s_nReleaseFPSLineHeight = 14.0f;
float s_nReleaseFPSValue;




static RGBAColor sGetFrameTimeColor(const float frameTime) {

    if (frameTime > 50.0f)
        return crgbaRed;


    if (frameTime > 33.4f)
        return crgbaOrange;


    if (frameTime > 22.2f)
        return crgbaYellow;


    if (frameTime > 17.0f)
        return crgbaWhite;


    return crgbaGreen;
}




static void sDrawReleaseFPS() {


    Blizzard::Time::Millisecond tCurrentTime = Blizzard::Time::Milliseconds();
    Blizzard::Time::Millisecond tElapsedTime = tCurrentTime - s_tLastTime;
    s_tLastTime = tCurrentTime;

    float nElapsedTimeMs = (float)tElapsedTime;

    if (tElapsedTime < 5000) {

        s_frameTimeFiltered = nElapsedTimeMs * 0.1f + s_frameTimeFiltered * 0.9f;
    }

    CRefString strFPS;

    if (s_frameTimeFiltered <= 0.000001f) {

        s_frameTimeFiltered = 0.1f;
    }


    Rect2D rectViewportUIC;
    CGameGetSafeViewportUIC(rectViewportUIC);
    Vector2D vecUIC(rectViewportUIC.right + s_nReleaseFPSPosX, rectViewportUIC.top + s_nReleaseFPSPosY);
    TextRenderProperties tProps(GlobalSNOGet(637), 13, 1.0f, 1.0f);
    RGBAColor rgbaColor = sGetFrameTimeColor(s_frameTimeFiltered);

    s_nReleaseFPSValue = 1000.0f / s_frameTimeFiltered;

    strFPS.Format("%3.1f FPS", s_nReleaseFPSValue);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), 5, FALSE), vecUIC, rgbaColor, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);

    vecUIC.x += 145.0f;
    FontStringDrawUIC(tProps, FormattedText(BuildConfigStr(), 5, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    vecUIC.x += -145.0f;

    vecUIC.y += s_nReleaseFPSLineHeight;
    strFPS.Format("avg: %3.1f ms, real: %3.1f ms", s_frameTimeFiltered, nElapsedTimeMs);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), 5, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);

    vecUIC.y += s_nReleaseFPSLineHeight;
    strFPS.Format("slip: %d, %s", s_fSlippedClock, sFrameRateBoundTypeToString(s_tFrameRateBoundType));
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), 5, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);

    {
        float flStreamed = GroupDefinition::GetGlobalCacheStreamed();
        vecUIC.y += s_nReleaseFPSLineHeight;
        strFPS.Format("Thrashing: %4.1f", flStreamed);
        rgbaColor = (flStreamed == 0.0f) ? crgbaGreen : ((flStreamed < 0.5f) ? crgbaYellow : crgbaRed);
        FontStringDrawUIC(tProps, FormattedText(strFPS.str(), 5, FALSE), vecUIC, rgbaColor, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    }


    float mainTimeSingle = sgtAppGlobals.mainTimerHistory.Get(FrameTimerHistory::PERIOD_SINGLE) / 1000.0f;
    float mainTimeShort = sgtAppGlobals.mainTimerHistory.Get(FrameTimerHistory::PERIOD_SHORT) / 1000.0f;
    float mainTimeLong = sgtAppGlobals.mainTimerHistory.Get(FrameTimerHistory::PERIOD_LONG) / 1000.0f;
    vecUIC.y += s_nReleaseFPSLineHeight;
    strFPS.Format("Main: %3.2f [ %3.2f | %3.2f ]", mainTimeSingle, mainTimeShort, mainTimeLong);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), 5, FALSE), vecUIC, sGetFrameTimeColor(mainTimeShort), &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);

    float renderTimeSingle = sgtAppGlobals.renderTimerHistory.Get(FrameTimerHistory::PERIOD_SINGLE) / 1000.0f;
    float renderTimeShort = sgtAppGlobals.renderTimerHistory.Get(FrameTimerHistory::PERIOD_SHORT) / 1000.0f;
    float renderTimeLong = sgtAppGlobals.renderTimerHistory.Get(FrameTimerHistory::PERIOD_LONG) / 1000.0f;
    vecUIC.y += s_nReleaseFPSLineHeight;
    strFPS.Format("Draw: %3.2f [ %3.2f | %3.2f ]", renderTimeSingle, renderTimeShort, renderTimeLong);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), 5, FALSE), vecUIC, sGetFrameTimeColor(renderTimeShort), &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);

    float gpuTimeSingle = sgtAppGlobals.gpuTimerHistory.Get(FrameTimerHistory::PERIOD_SINGLE) / 1000.0f;
    float gpuTimeShort = sgtAppGlobals.gpuTimerHistory.Get(FrameTimerHistory::PERIOD_SHORT) / 1000.0f;
    float gpuTimeLong = sgtAppGlobals.gpuTimerHistory.Get(FrameTimerHistory::PERIOD_LONG) / 1000.0f;
    vecUIC.y += s_nReleaseFPSLineHeight;
    strFPS.Format("GPU: %3.2f [ %3.2f | %3.2f ]", gpuTimeSingle, gpuTimeShort, gpuTimeLong);
    FontStringDrawUIC(tProps, FormattedText(strFPS.str(), 5, FALSE), vecUIC, sGetFrameTimeColor(gpuTimeShort), &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
}





static void sDrawFPSSimple() {
    {
        static uint32 sdwLast = 0;



        uint32 dwCurrentTick = GetMachineTicks();
        uint32 dwDuration = 1000;

        if (dwCurrentTick > sdwLast + dwDuration) {

            sdwLast = dwCurrentTick;
            GamePerfCollectAverages();
        }
    }


    GamePerfStats & tPerfStats = GamePerfStatsGet();




    float fps = tPerfStats.flAvgFPS;


    CRefString sVal = UnicodeText::FormatNumber(fps, TRUE, FNS_NORMAL, 0, 0, XLOCALE_INVALID);
    CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(706), "FPSFormat"), sVal.str(), NULL);



    Rect2D rectViewportUIC;
    CGameGetSafeViewportUIC(rectViewportUIC);
    TextRenderProperties tProps(GlobalSNOGet(637), 13, 1.0f, 1.0f);
    FormattedText tFormattedText(sMsg, 5);

    Vector2D vecSizeUIC;
    FontStringGetRenderedSizeUIC(tProps, tFormattedText, vecSizeUIC);
    Vector2D vecUIC(rectViewportUIC.right - vecSizeUIC.x + -15.0f, rectViewportUIC.bottom - vecSizeUIC.y + -15.0f);
    FontStringDrawUIC(tProps, tFormattedText, vecUIC, crgbaWhite, &crgbaTextShadow, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
}























































static void sLogQAPerfStats() {


    if (sgtAppGlobals.eDebugDisplayMode == DDM_FPS_QA) {




        CGameWriteQAPerfLogs();
    }
}
float s_flRenderUpdateTime;




static void sBuildShadowCamera(RWindow & tRWindow) {


    Vector3D wpPlayerPos(cgwpNull);
    RActorID idRActorForShadowCamera = Weather::GetLitPlayer(tRWindow.GetWeatherRWindowData());
    const RActor * ptRActorPlayer = RActorTryToGet(idRActorForShadowCamera);
    if (ptRActorPlayer) {


        wpPlayerPos = ptRActorPlayer->tLocation.tWorldLocation.transform.wp;
    } else {



        GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
        if (eGameMode == GAMEMODE_MENU) {

            CGame & tCGame = CGameGet();
            TDataArrayIterator<CScene> tIt;
            if (CSceneDataArrayIterate(tIt, tCGame, SkipHolodeckScenes)) {

                CScene * ptScene = tIt.ptElement;
                PRTransform transform(cgqIdentity, cgwpNull);
                if (CSceneGetHardpointTransformByLabel(ptScene->id, HardpointHash("HP_PartyLeader"), transform)) {

                    wpPlayerPos = transform.wp;
                }
            }
        }
    }


    Light * ptLight = LightTryToGet(Weather::GetSunLightScene(tRWindow.GetWeatherRWindowData()));
    if (!ptLight) {


        ptLight = LightTryToGet(Weather::GetLocalPlayerHeadlight(tRWindow.GetWeatherRWindowData()));
        if (ptLight) {

            LightState & tState = ptLight->tState;
            wpPlayerPos = tState.wp;
        }
    } else {


        GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
        if (eGameMode != GAMEMODE_MENU) {

            RWindow & tMainRWindow = GetRWindowMgr()->GetRWindow(0);
            if (tMainRWindow.LocalPlayerWorldPositionIsValid()) {



                wpPlayerPos = tMainRWindow.GetLocalPlayerWorldPosition();
            }
        }
    }


    CameraManagerBuildShadowCamera(1, wpPlayerPos);
}




static void sRenderUpdate(GameMode eGameMode, RWindowMgr & tRWindowMgr, BOOL fUIOnly) {





    DebugRenderMode eDRM = sgtAppGlobals.eDebugRenderMode;
    BOOL fDrawUI;
    if (eDRM == DRM_AO || eDRM == DRM_AO_SM || eDRM == DRM_ENVMAP || eDRM == DRM_GLOSS ||



        (eGameMode != GAMEMODE_HOLODECK && eDRM == DRM_FLATSHADED)) {
        fDrawUI = FALSE;
    } else {
        UI_PreDraw();
        fDrawUI = TRUE;
    }
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    BOOL bRenderingFullScreenCutscene = UI_DrawingFullscreenUI();

    sBuildShadowCamera(tRWindowMgr.GetRWindow(0));


    float flClientTime = CGameGetClientTime();
    float flDeltaTime = flClientTime - tCGameGlobals.flLastLocalGameTimeUpdate;
    tCGameGlobals.flLastLocalGameTimeUpdate = flClientTime;

    PostFXRenderUpdate();


    PostFXSetFrameFlag(2, s_tLoadingScreenManager.IsEnabled());








    if (!fUIOnly && !bRenderingFullScreenCutscene) {

        for (int32 i = 0; i < tRWindowMgr.GetRWindowCount(); i++) {
            RWindow & tRWindow = tRWindowMgr.GetRWindow(i);
            tRWindowMgr.SetCurrentRWindow(tRWindow);


            if (!tRWindow.ShouldRender())
                continue;



            tRWindow.BuildCameras();

            if (!AppDrawFlagIsSet(19)) {

                CScenesRenderUpdate(tRWindow, flDeltaTime, flClientTime);
            }

            {
                Stopwatch tTimer;
                ParticlesRenderUpdate(tRWindow, flDeltaTime);
                s_flRenderUpdateTime = s_flRenderUpdateTime * 0.9f + tTimer.Check() * 1000.0f * 0.1f;
            }

            LightsRenderUpdate(tRWindow);
            RActorGfxRenderUpdate(tRWindow, flDeltaTime, flClientTime);

            CGameModeRenderUpdate(tRWindow);

            if (!AppDrawFlagIsSet(30)) {

                RopeRenderUpdate(tRWindow);
            }

            if (!AppDrawFlagIsSet(29)) {

                TrailsRenderUpdate(tRWindow);
            }





            if (tRWindow.GetRWindowID() == RWindowMainWindow) {

                CGameVariableResRenderUpdate(tRWindow.GetVariableResRWindowData());
            }
        }
        tRWindowMgr.ResetCurrentRWindowToDefault();



        gptPhysicsAPIImpl->Splash();
        SubObjectGfxUpdate(flDeltaTime, flClientTime);
    }








    sLogQAPerfStats();

    if (AppDrawFlagIsSet(99)) {

        sDrawReleaseFPS();
    }

    if (!CGameLoadingScreenIsEnabled()) {

        CutscenesRenderUpdate();
    }

    tRWindowMgr.DisplayListsSort();


    if (fDrawUI) {



        if (!AppDrawFlagIsSet(22)) {
            BOOL fHideCursor = AppDrawFlagIsSet(23);

            if ((eGameMode == GAMEMODE_PLAYING || eGameMode == GAMEMODE_MENU) && !fHideCursor && CutsceneGetFullScreenCutscene() == CUTSCENEID_INVALID) {


                PlayerGetCursorMgr().Draw();
            }
        }

        UI_Draw();
    }

    CPlayerRenderUpdate();

    if (sgtAppGlobals.eDebugDisplayMode == DDM_FPS_SIMPLE) {

        sDrawFPSSimple();
    }






























    BuildLockerDrawWatermark();


    ParticlesMultiRenderUpdateFinish();
}




static void sRenderDrawFullScreenCutscene(RWindow & tRWindow, IRect2D & rectUIViewport) {




    if (GfxGetSystemID() == GFX_NX64NVN) {

        GfxSetRenderTargetToSwapChainRT();
        GfxSetDepthTarget(tRWindow.GetDefaultDepthTarget());

    } else if (GfxGetMSAAEnabled()) {

        GfxSetRenderTargetToSwapChainRT();
        GfxSetDepthTarget(PostFXGetBuffer(EFFECTBUFFER_BACKBUFFER_DEPTH));
    } else {


        tRWindow.SetRenderAndDepthTargetToDefaults();
    }

    GfxRenderClear(TRUE, TRUE, TRUE, crgbaBlack);
    GfxViewportSet(rectUIViewport);



    CutscenesRenderFullscreen();


    sDrawUIRenderLayer(RENDERLAYER_UI);
    sDrawUIRenderLayer(RENDERLAYER_UI_OVERLAY);
}

// Declarations for sRenderDrawDefault and the helpers it inlines.
// X/Client/Camera/CameraManager.h:8 and :12
enum {
    NUM_REFLECTION_PLANES = 2,
};
enum {
    CAMERA_CURRENT = -1,
    CAMERA_DEFAULT_VIEW = 0,
    CAMERA_SHADOW = 1,
    CAMERA_REFLECTION_0 = 2,
    CAMERA_REFLECTION_1 = 3,
};
// X/Client/Weather.h:12 and :22
struct WeatherRenderFogState {  // size 0x18
    BOOL bFogEnabled;  // 0x00
    RGBAColor rgbaFogColor;  // 0x04
    float flFogNear;  // 0x08
    float flFogEnd;  // 0x0c
    float flHeightFogNear;  // 0x10
    float flHeightFogEnd;  // 0x14
};
static_assert(sizeof(WeatherRenderFogState) == 0x18, "WeatherRenderFogState");
struct WeatherRenderData {  // size 0x1c
    float flIntensity;  // 0x00
    WeatherRenderFogState tFogState;  // 0x04
};
static_assert(sizeof(WeatherRenderData) == 0x1c, "WeatherRenderData");
namespace Weather {
BOOL GetFogColor(const WeatherRWindowData & tData, RGBAColor & rgbaFogColor);
const WeatherRenderData * GetRenderData(const WeatherRWindowData & tData);
}  // namespace Weather






// X/Client/PostFX.h:44
enum {
    POSTFX_FRAME_HAS_SOMETHING_DISTORTING = 0,
    POSTFX_FRAME_GHOSTED = 1,
    POSTFX_FRAME_DISABLE_TEXT_HIGHLIGHT = 2,
};
// X/Client/Gfx/Gfx.cpp
void GfxSetReflectionPlane(int32 nIndex, const float * pflPlane);
void GfxSetClipPlane(int32 nIndex, const float * pflPlane);
const Matrix4x4 & GfxViewMatrixGet();
const Matrix4x4 & GfxProjectionMatrixGet();
void GfxViewMatrixSet(const Matrix4x4 & mView);
void GfxProjectionMatrixSet(const Matrix4x4 & mProjection);
void GfxFogSetEnabled(BOOL fEnabled);
void GfxFogSetParams(const RGBAColor & rgbaFogColor, const float flFogStart, const float flFogEnd, const float flHeightFogStart, const float flHeightFogEnd);
void GfxSetRenderTarget(const SNO sno, const CubeMapFace eCubeMapFace);
void GfxSetRenderAndDepthTarget(const SNO snoRenderTarget, const SNO snoDepthTarget, const CubeMapFace eCubeMapFace);
void GfxGetRenderTargets(RenderTarget * ptRenderTargets);
void GfxSetRenderTargets(const RenderTarget * ptRenderTargets);
SNO GfxGetDepthTarget();
void GfxRestoreRenderTarget(const SNO snoTex);
void GfxGetRenderTargetDimensions(uint32 & dwWidth, uint32 & dwHeight);
void GfxViewportScaleBiasSet(const Vector4D & vViewportScaleBias);
const DisplayMode & GfxGetCurrentDisplayMode();
void Gfx3DPrimitivesDraw(const RenderLayer eRenderLayer);
void GfxTriOverlaysDraw(const RenderLayer eRenderLayer);
// X/Client/PostFX.cpp
int32 PostFXGetNumReflectionPlanes();
Plane & PostFXGetReflectionPlane(int32 nPlaneIndex);
BOOL PostFXEnabled();
BOOL PostFXGetFrameFlag(const uint32 dwBit);
void PostFXComputeViewportUVScaleBias(Vector4D & vViewportScaleBias, uint32 dwWidth, uint32 dwHeight, const IRect2D & rectViewport);
void PostFXCopyRenderTarget(const uint32 dwRTIndex, const SNO snoDestTexture);
void PostFXCopyRenderTargetMS(const uint32 dwRTIndex, const SNO snoDestTexture);
void PostFXCopyDepthTarget(const SNO snoDestTexture);
void PostFXResolveMSAAColorTarget(const uint32 dwRTIndex, const SNO snoDestTexture);
void PostFXResolveMSAADepthTarget(const SNO snoDestTexture);
void PostFXFilterRender(const SNO snoShader, const SNO snoTexture);
void PostFXFilterRender(const SNO snoShader, const SNO snoTexture0, const SNO snoTexture1, const SNO snoTexture2, const SNO snoTexture3);
void PostFXSSAO(RWindow & tRWindow, const IRect2D & rectViewport, const SNO snoDepthTexture);
void PostFXDraw(RWindow & tRWindow, const IRect2D & rectViewport);
void PostFXHighlight(RWindow & tRWindow, const IRect2D & rectViewport);
SNO PostFXFSAA(RWindow & tRWindow, const IRect2D & rectViewport);
void PostFXFadeDrawDeferred();
void PostFXGamma();
// X/Client/DisplayList.cpp, X/Client/SubObjectGfx.cpp, X/Client/Particle*.cpp, X/Client/CGameMode.cpp
void DisplayListBuildProjectionLayers(RWindow & tRWindow);
void DisplayListDrawProjectionLayers(RWindow & tRWindow);
BOOL SubObjectGetLocalPlayerIsGhosted();
void SubObjectGfxDebugDepthDrawDeferred(RWindow & tRWindow);
void ParticleDrawSetUseClearedSoftParticleDepth(BOOL fUseCleared);
void CGameModeDraw(DisplayList & dispList, const RenderLayer eRenderLayer);
uint32 CGameAsyncGetRenderDataIndex(AsyncRenderDataIndex eIndex);

static void sDrawRenderLayer(DisplayList & dispList, const RenderLayer eRenderLayer, const BOOL fDrawingForReflection) {
    if (AppRenderLayerIsHidden(eRenderLayer))
        return;
    dispList.DrawRenderLayer(eRenderLayer);
    if (fDrawingForReflection || eRenderLayer == RENDERLAYER_DEPTHPASS_DEPRECATED)
        return;
    CGameModeDraw(dispList, eRenderLayer);
    Gfx3DPrimitivesDraw(eRenderLayer);
    GfxTriOverlaysDraw(eRenderLayer);
}

static Plane sTransformPlane(const Plane & wplane, const Matrix4x4 & matTransform) {
    Matrix4x4 matInvTranspose = matTransform.Inverse().Transpose();
    Vector4D vecPlane(wplane.wvNormal.x, wplane.wvNormal.y, wplane.wvNormal.z, wplane.wdD);
    Vector4D vecTransPlane = vecPlane * matInvTranspose;
    Plane planeTrans;
    planeTrans.wvNormal.x = vecTransPlane.x;
    planeTrans.wvNormal.y = vecTransPlane.y;
    planeTrans.wvNormal.z = vecTransPlane.z;
    float Magn = planeTrans.wvNormal.Magnitude();
    planeTrans.wdD = vecTransPlane.w * (1.0f / Magn);
    planeTrans.wvNormal *= 1.0f / Magn;
    return planeTrans;
}

static void sSceneDrawForReflections(DisplayList & dispList, int nPlaneIndex) {
    CameraManagerSetCurrentCamera(CAMERA_REFLECTION_0 + nPlaneIndex);
    Plane plane = PostFXGetReflectionPlane(nPlaneIndex);
    Matrix4x4 matView = GfxViewMatrixGet();
    Matrix4x4 matProjection = GfxProjectionMatrixGet();
    Matrix4x4 matViewProjection = matView * matProjection;
    Plane planeClip = sTransformPlane(plane, matViewProjection);
    float flClipPlane[4];
    flClipPlane[0] = planeClip.wvNormal.x;
    flClipPlane[1] = planeClip.wvNormal.y;
    flClipPlane[2] = planeClip.wvNormal.z;
    flClipPlane[3] = planeClip.wdD;
    GfxSetClipPlane(0, flClipPlane);
    sDrawRenderLayer(dispList, RENDERLAYER_DEPTHPASS_DEPRECATED, TRUE);
    sDrawRenderLayer(dispList, RENDERLAYER_FLOOR, TRUE);
    sDrawRenderLayer(dispList, RENDERLAYER_OPAQUE, TRUE);
    sDrawRenderLayer(dispList, RENDERLAYER_TRANSPARENT_GROUND, TRUE);
    sDrawRenderLayer(dispList, RENDERLAYER_TRANSPARENT, TRUE);
    CameraManagerRestoreCurrentCameraToMainCamera();
    GfxSetClipPlane(0, NULL);
}

static void sSceneDraw(RWindow & tRWindow, const IRect2D & rectViewport, const RGBAColor & rgbaClearColor) {
    DisplayList & dispListMain = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)].dispList;
    IRect2D rectRender = rectViewport;
    if (GfxGetMSAAEnabled()) {
        GfxSetRenderAndDepthTarget(PostFXGetBuffer(EFFECTBUFFER_MSAA_COLOR), PostFXGetBuffer(EFFECTBUFFER_MSAA_DEPTH), CUBEMAPFACE_POSX);
    }
    {
        uint32 dwWidth, dwHeight;
        GfxGetRenderTargetDimensions(dwWidth, dwHeight);
        Vector4D vViewportScaleBias;
        PostFXComputeViewportUVScaleBias(vViewportScaleBias, dwWidth, dwHeight, rectRender);
        GfxViewportScaleBiasSet(vViewportScaleBias);
    }
    GfxRenderClear(FALSE, TRUE, TRUE, rgbaClearColor);
    GfxViewportSet(rectRender);
    if (AppDrawFlagIsSet(104)) {
        RenderTarget tRenderTargets[2];
        GfxGetRenderTargets(tRenderTargets);
        SNO snoDepthTarget = GfxGetDepthTarget();
        GfxSetRenderAndDepthTarget(SNO_NONE, snoDepthTarget, CUBEMAPFACE_POSX);
        sDrawRenderLayer(dispListMain, RENDERLAYER_DEPTH_PREPASS, FALSE);
        GfxSetRenderTargets(tRenderTargets);
        GfxSetDepthTarget(snoDepthTarget);
    }
    if (GfxGetSystemID() == GFX_D3D || GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D) {
        if (AppDrawFlagIsSet(114) && AppDrawFlagIsSet(104)) {
            SNO snoDepthTexture = GfxGetDepthTarget();
            if (GfxGetSystemID() != GFX_PS4GNM && GfxGetSystemID() != GFX_XBOXONED3D &&
                GfxGetMSAAEnabled()) {
                snoDepthTexture = PostFXGetBuffer(GfxGetSystemID() == GFX_D3D11 ? EFFECTBUFFER_DEPTH_PREV : EFFECTBUFFER_DEPTH);
                PostFXResolveMSAADepthTarget(snoDepthTexture);
            }
            PostFXSSAO(tRWindow, rectRender, snoDepthTexture);
        } else {
            RenderTarget tRenderTargets[2];
            GfxGetRenderTargets(tRenderTargets);
            SNO snoDepthTarget = GfxGetDepthTarget();
            GfxSetRenderAndDepthTarget(PostFXGetBuffer(EFFECTBUFFER_SSAO), SNO_NONE, CUBEMAPFACE_POSX);
            GfxRenderClear(TRUE, FALSE, FALSE, RGBAColor(255, 255, 255, 255));
            GfxSetRenderTargets(tRenderTargets);
            GfxSetDepthTarget(snoDepthTarget);
        }
    }
    GfxViewportSet(rectRender);
    GfxRenderClear(TRUE, FALSE, FALSE, rgbaClearColor);
    sDrawRenderLayer(dispListMain, RENDERLAYER_DEPTHPASS_DEPRECATED, FALSE);
    sDrawRenderLayer(dispListMain, RENDERLAYER_FLOOR, FALSE);
    sDrawRenderLayer(dispListMain, RENDERLAYER_OPAQUE, FALSE);
    sDrawRenderLayer(dispListMain, RENDERLAYER_OCCLUSION_QUERY, FALSE);
    if (!AppDrawFlagIsSet(24)) {
        sDrawRenderLayer(dispListMain, RENDERLAYER_SKY, FALSE);
    }
    sDrawRenderLayer(dispListMain, RENDERLAYER_TRANSPARENT_GROUND, FALSE);
    sDrawRenderLayer(dispListMain, RENDERLAYER_GROUND_FX_1, FALSE);
    sDrawRenderLayer(dispListMain, RENDERLAYER_AURAS, FALSE);
    GameMode eMode = CGameGetGameCommonData().GameModeGet();
    if (eMode == GAMEMODE_PLAYING || eMode == GAMEMODE_HOLODECK || eMode == GAMEMODE_MENU) {
        if (!AppRenderLayerIsHidden(RENDERLAYER_PROJECTED_ADDITIVE)) {
            DisplayListDrawProjectionLayers(tRWindow);
        }
    }
    if (!GfxGetMSAAEnabled()) {
        PostFXCopyRenderTarget(0, PostFXGetBuffer(EFFECTBUFFER_REFRACTION));
    } else {
        PostFXCopyRenderTargetMS(0, PostFXGetBuffer(EFFECTBUFFER_REFRACTION));
    }
    GfxViewportSet(rectRender);
    sDrawRenderLayer(dispListMain, RENDERLAYER_REFLECTIONS, FALSE);
    if (GfxGetMSAAEnabled()) {
        if (GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D) {
            PostFXResolveMSAADepthTarget(PostFXGetBuffer(EFFECTBUFFER_DEPTH_PREV));
        } else {
            PostFXResolveMSAADepthTarget(PostFXGetBuffer(EFFECTBUFFER_DEPTH));
        }
    } else {
        if (GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D) {
            PostFXCopyDepthTarget(PostFXGetBuffer(EFFECTBUFFER_DEPTH_PREV));
        }
    }
    GfxViewportSet(rectRender);
    sDrawRenderLayer(dispListMain, RENDERLAYER_TRANSPARENT, FALSE);
    sDrawRenderLayer(dispListMain, RENDERLAYER_TRANSPARENT_MASKED, FALSE);
    sDrawRenderLayer(dispListMain, RENDERLAYER_BLOOM, FALSE);
    if (AppDrawFlagIsSet(52)) {
        CameraManagerRestoreCurrentCameraToMainCamera();
        sDrawRenderLayer(dispListMain, RENDERLAYER_DOMINO, FALSE);
    }
    if (GfxGetMSAAEnabled()) {
        Matrix4x4 viewMat = GfxViewMatrixGet();
        Matrix4x4 projMat = GfxProjectionMatrixGet();
        PostFXResolveMSAAColorTarget(0, PostFXGetBuffer(EFFECTBUFFER_COLOR));
        GfxViewportSet(rectRender);
        if (!AppDrawFlagIsSet(28) &&
            sgtAppGlobals.eDebugRenderMode != DRM_DEPTH_COMPLEXITY) {
            GfxRenderClear(TRUE, FALSE, FALSE, RGBAColor(0, 0, 0, 0));
            CameraManagerRestoreCurrentCameraToMainCamera();
            sDrawRenderLayer(dispListMain, RENDERLAYER_HIGHLIGHT, FALSE);
            PostFXCopyRenderTargetMS(0, PostFXGetBuffer(EFFECTBUFFER_HIGHLIGHT));
        }
        GfxSetRenderTarget(PostFXGetBuffer(EFFECTBUFFER_COLOR), CUBEMAPFACE_POSX);
        GfxSetDepthTarget(tRWindow.GetDefaultDepthTarget());
        GfxViewportSet(rectRender);
        GfxViewMatrixSet(viewMat);
        GfxProjectionMatrixSet(projMat);
    }
}

static void sGetSkyColor(RWindow & tRWindow, RGBAColor & rgbaSkyColor) {
    if (AppDrawFlagIsSet(83)) {
        rgbaSkyColor = sgtAppGlobals.rgbaGreenscreen;
    } else {
        if (sgtAppGlobals.eDebugRenderMode != DRM_DEPTH_COMPLEXITY) {
            const WorldCommonData * ptWCD = WorldTryGetCommonData(CGameGetCurrentSWorldID());
            if (ptWCD) {
                ScopedWorldDefinitionLockR pdefWorld(ptWCD->snoWorld);
                const Environment * ptEnvironment = &pdefWorld->tEnvironment;
                rgbaSkyColor = ptEnvironment->rgbaSkyColor;
            } else {
                rgbaSkyColor = crgbaBlack;
            }
        }
        Weather::GetFogColor(tRWindow.GetWeatherRWindowData(), rgbaSkyColor);
    }
    rgbaSkyColor.a = 0;
}

static void sPrepareReflectionPlanes() {
    if (Prefs::GetVideoPrefs().GetReflectionQuality() > 0) {
        for (int nPlaneIndex = 0; nPlaneIndex < NUM_REFLECTION_PLANES; nPlaneIndex++) {
            if (nPlaneIndex < PostFXGetNumReflectionPlanes()) {
                Plane plane = PostFXGetReflectionPlane(nPlaneIndex);
                Matrix4x4 matView = GfxViewMatrixGet();
                Plane planeTrans = sTransformPlane(plane, matView);
                float flReflectionPlane[4] = { planeTrans.wvNormal.x, planeTrans.wvNormal.y,
                                               planeTrans.wvNormal.z, planeTrans.wdD };
                GfxSetReflectionPlane(nPlaneIndex, flReflectionPlane);
            } else {
                float flReflectionPlane[4] = { 0.0f, 0.0f, 0.0f, 1000.0f };
                GfxSetReflectionPlane(nPlaneIndex, flReflectionPlane);
            }
        }
    } else {
        float flReflectionPlane[4] = { 0.0f, 0.0f, 0.0f, 1000.0f };
        for (int nPlaneIndex = 0; nPlaneIndex < NUM_REFLECTION_PLANES; nPlaneIndex++) {
            GfxSetReflectionPlane(nPlaneIndex, flReflectionPlane);
        }
    }
}

static void sRenderProjectionAndReflection(RWindow & tRWindow) {
    const WeatherRenderFogState & tRenderFogState = Weather::GetRenderData(tRWindow.GetWeatherRWindowData())->tFogState;
    GfxFogSetEnabled(tRenderFogState.bFogEnabled);
    GfxFogSetParams(tRenderFogState.rgbaFogColor, tRenderFogState.flFogNear, tRenderFogState.flFogEnd, tRenderFogState.flHeightFogNear, tRenderFogState.flHeightFogEnd);
    DisplayListBuildProjectionLayers(tRWindow);
    if (Prefs::GetVideoPrefs().GetReflectionQuality() > 0) {
        RGBAColor rgbaClear(0, 0, 0, 0);
        for (int nPlaneIndex = 0; nPlaneIndex < PostFXGetNumReflectionPlanes(); nPlaneIndex++) {
            GfxFogSetEnabled(TRUE);
            GfxFogSetParams(rgbaClear, 0.0f, 6.0f, -10000.0f, -10000.0f);
            SNO snoReflectionTarget = PostFXGetBuffer((EffectBuffer)(EFFECTBUFFER_REFLECTION_0 + nPlaneIndex));
            GfxSetRenderAndDepthTarget(snoReflectionTarget,
                                       PostFXGetBuffer(EFFECTBUFFER_REFLECTION_DEPTH), CUBEMAPFACE_POSX);
            GfxRenderClear(TRUE, TRUE, TRUE, rgbaClear);
            sSceneDrawForReflections(tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)].dispListReflections[nPlaneIndex], nPlaneIndex);
        }
    }
}

static void sDrawPostFX(RWindow & tRWindow, const IRect2D & rectViewport) {
    if (!AppRenderLayerIsHidden(RENDERLAYER_POSTFX)) {
        DisplayList & dispListMain = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)].dispList;
        if (!dispListMain.IsRenderLayerEmpty(RENDERLAYER_MOTION_BLUR)) {
            PostFXCopyRenderTarget(0, PostFXGetBuffer(EFFECTBUFFER_SCREEN_PING));
            GfxSetRenderTarget(PostFXGetBuffer(EFFECTBUFFER_SCREEN_PONG), CUBEMAPFACE_POSX);
            GfxRenderClear(TRUE, FALSE, FALSE, RGBAColor(128, 128, 0, 0));
            GfxViewportSet(rectViewport);
            sDrawRenderLayer(dispListMain, RENDERLAYER_MOTION_BLUR, FALSE);
            GfxSetRenderTarget(tRWindow.GetDefaultColorTarget(), CUBEMAPFACE_POSX);
            GfxViewportSet(rectViewport);
            PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_MOTION_BLUR_SHADER), PostFXGetBuffer(EFFECTBUFFER_SCREEN_PING), PostFXGetBuffer(EFFECTBUFFER_SCREEN_PONG), SNO_NONE, SNO_NONE);
        }
        if (PostFXGetFrameFlag(POSTFX_FRAME_HAS_SOMETHING_DISTORTING) &&
            AppDrawFlagIsSet(97) &&
            !AppDrawFlagIsSet(26)) {
            RenderTarget tRenderTargets[2] = { { PostFXGetBuffer(EFFECTBUFFER_BLUR_DISTORTION), CUBEMAPFACE_POSX }, { SNO_NONE, CUBEMAPFACE_POSX } };
            GfxSetRenderTargets(tRenderTargets);
            GfxSetDepthTarget(tRWindow.GetDefaultDepthTarget());
            GfxRenderClear(TRUE, FALSE, FALSE, RGBAColor(128, 128, 0, 0));
            GfxViewportSet(rectViewport);
            GfxRenderClear(FALSE, FALSE, TRUE, RGBAColor(0, 0, 0, 0));
            sDrawRenderLayer(dispListMain, RENDERLAYER_POSTFX_DISTORTION, FALSE);
            GfxSetRenderTarget(tRWindow.GetDefaultColorTarget(), CUBEMAPFACE_POSX);
            GfxViewportSet(rectViewport);
        }
        BOOL fGhosted = SubObjectGetLocalPlayerIsGhosted();
        PostFXSetFrameFlag(POSTFX_FRAME_GHOSTED, fGhosted);
        PostFXDraw(tRWindow, rectViewport);
    }
}

static void sDrawHighlightPartA(RWindow & tRWindow, IRect2D & rectMainViewport) {
    DisplayList & dispListMain = tRWindow.tAsyncData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)].dispList;
    RenderTarget tRenderTargets[2] = { { PostFXGetBuffer(EFFECTBUFFER_SCREEN_PING), CUBEMAPFACE_POSX }, { SNO_NONE, CUBEMAPFACE_POSX } };
    GfxSetRenderTargets(tRenderTargets);
    GfxSetDepthTarget(tRWindow.GetDefaultDepthTarget());
    GfxViewportSet(rectMainViewport);
    CameraManagerRestoreCurrentCameraToMainCamera();
    if (!GfxGetMSAAEnabled()) {
        GfxRenderClear(TRUE, FALSE, FALSE, RGBAColor(0, 0, 0, 0));
        GfxViewportSet(rectMainViewport);
        sDrawRenderLayer(dispListMain, RENDERLAYER_HIGHLIGHT, FALSE);
    }
    // The original keeps this in a local the DWARF does not show; the name is invented.
    BOOL fHighlightEmpty = dispListMain.IsRenderLayerEmpty(RENDERLAYER_HIGHLIGHT);
    if (!fHighlightEmpty) {
        GfxRenderClear(FALSE, FALSE, TRUE, RGBAColor(0, 0, 0, 0));
        dispListMain.DrawBoundingStencil(RENDERLAYER_HIGHLIGHT_STENCIL_BOUNDS);
        PostFXHighlight(tRWindow, rectMainViewport);
    }
    GfxSetRenderTarget(tRWindow.GetDefaultColorTarget(), CUBEMAPFACE_POSX);
    GfxViewportSet(rectMainViewport);
    if (fHighlightEmpty) {
        GfxRestoreRenderTarget(tRWindow.GetDefaultColorTarget());
    }
}

static void sRenderScenes(RWindow & tRWindow, IRect2D & rectMainViewport) {
    const WeatherRenderFogState & tRenderFogState = Weather::GetRenderData(tRWindow.GetWeatherRWindowData())->tFogState;
    GfxFogSetEnabled(tRenderFogState.bFogEnabled);
    GfxFogSetParams(tRenderFogState.rgbaFogColor, tRenderFogState.flFogNear, tRenderFogState.flFogEnd, tRenderFogState.flHeightFogNear, tRenderFogState.flHeightFogEnd);
    tRWindow.SetRenderAndDepthTargetToDefaults();
    RGBAColor rgbaSkyColor;
    sGetSkyColor(tRWindow, rgbaSkyColor);
    if (sgtAppGlobals.eDebugRenderMode == DRM_DEPTH_COMPLEXITY) {
        rgbaSkyColor = crgbaBlack;
    }
    IRect2D rectViewport3D = rectMainViewport;
    if (CGameVariableResEnabled() && GetRWindowMgr()->GetCurrentRWindowID() == RWindowMainWindow) {
        ScopedTextureDefinitionLockR pdefTex(tRWindow.GetDefaultColorTarget());
        VariableResRWindowData & tData = tRWindow.GetVariableResRWindowData();
        uint32 dwWidth, dwHeight;
        gptGFXAPI->GetRenderTargetCurrentResolution(pdefTex->tTexData.htex, dwWidth, dwHeight);
        dwWidth = (uint32)(tData.flPercent * dwWidth);
        dwHeight = (uint32)(tData.flPercent * dwHeight);
        rectViewport3D.Set(0, 0, dwWidth, dwHeight);
    }
    sSceneDraw(tRWindow, rectViewport3D, rgbaSkyColor);
    if (PostFXEnabled()) {
        sDrawPostFX(tRWindow, rectViewport3D);
    }
    if (!AppDrawFlagIsSet(28) &&
        sgtAppGlobals.eDebugRenderMode != DRM_DEPTH_COMPLEXITY) {
        sDrawHighlightPartA(tRWindow, rectViewport3D);
    }
    if (GfxGetSystemID() == GFX_D3D || GfxGetSystemID() == GFX_D3D11 || GfxGetSystemID() == GFX_PS4GNM || GfxGetSystemID() == GFX_XBOXONED3D) {
        if (AppDrawFlagIsSet(116)) {
            PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_COPY_ALPHA_SHADER), PostFXGetBuffer(EFFECTBUFFER_SSAO));
        }
        if (AppDrawFlagIsSet(117)) {
            PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_COPY_SHADER), PostFXGetBuffer(EFFECTBUFFER_SSAO_NORMAL));
        }
    }
    {
        SNO snoFXAAResult = PostFXFSAA(tRWindow, rectViewport3D);
        GfxSetRenderTargetToSwapChainRT();
        GfxSetDepthTarget(SNO_NONE);
        PostFXFilterRender(GlobalSNOGet(GLOBALSNO_POSTFX_UPSCALE_SHADER), snoFXAAResult);
    }
}

static void sRenderUI(RWindow & tRWindow, IRect2D & rectUIViewport, IRect2D & rectMainViewport) {
    const WeatherRenderFogState & tRenderFogState = Weather::GetRenderData(tRWindow.GetWeatherRWindowData())->tFogState;
    GfxFogSetEnabled(tRenderFogState.bFogEnabled);
    GfxFogSetParams(tRenderFogState.rgbaFogColor, tRenderFogState.flFogNear, tRenderFogState.flFogEnd, tRenderFogState.flHeightFogNear, tRenderFogState.flHeightFogEnd);
    if (GfxGetMSAAEnabled()) {
        GfxSetRenderTargetToSwapChainRT();
        GfxSetDepthTarget(PostFXGetBuffer(EFFECTBUFFER_BACKBUFFER_DEPTH));
    } else if (GfxGetSystemID() == GFX_NX64NVN) {
        GfxSetRenderTargetToSwapChainRT();
        GfxSetDepthTarget(tRWindow.GetDefaultDepthTarget());
    } else {
        tRWindow.SetRenderAndDepthTargetToDefaults();
    }
    PostFXFadeDrawDeferred();
    if (sgtAppGlobals.eDebugRenderMode == DRM_PMA_DEPTH_COMPLEXITY) {
        SubObjectGfxDebugDepthDrawDeferred(tRWindow);
    }
    if (GfxGetMSAAEnabled() || GfxGetSystemID() == GFX_NX64NVN) {
        ParticleDrawSetUseClearedSoftParticleDepth(TRUE);
    }
    GfxRenderClear(FALSE, TRUE, TRUE, RGBAColor(0, 0, 0, 0));
    GfxViewportSet(rectUIViewport);
    sDrawUIRenderLayer(RENDERLAYER_UI);
    sDrawUIRenderLayer(RENDERLAYER_UI_OVERLAY);
    if (GfxGetMSAAEnabled() || GfxGetSystemID() == GFX_NX64NVN) {
        ParticleDrawSetUseClearedSoftParticleDepth(FALSE);
    }
    GfxViewportSet(rectUIViewport);
    sDrawUIRenderLayer(RENDERLAYER_BATTLENET_UI);
    if (GfxIsLetterboxed() && rectMainViewport.Width() != 0 && rectMainViewport.Height() != 0) {
        RGBAColor rgbaBlack(0, 0, 0, 0);
        IRect2D rectTopBar;
        rectTopBar.Set(rectUIViewport.left, rectUIViewport.top,
                       rectUIViewport.right, rectMainViewport.top);
        GfxViewportSet(rectTopBar);
        GfxRenderClear(TRUE, TRUE, TRUE, rgbaBlack);
        IRect2D rectBottomBar;
        rectBottomBar.Set(rectUIViewport.left, rectMainViewport.bottom - 1,
                          rectUIViewport.right, rectUIViewport.bottom);
        GfxViewportSet(rectBottomBar);
        GfxRenderClear(TRUE, TRUE, TRUE, rgbaBlack);
        GfxViewportSet(rectMainViewport);
    }
    if (GfxGetSystemID() == GFX_D3D11 && GfxGetCurrentDisplayMode().dwWindowMode == 0) {
        PostFXGamma();
    }
}

// NON_MATCHING: 36 bytes short. In each of the three inlined sTransformPlane copies the original
// reloads _11 of the inverse through a pointer that selects cgm4x4Identity or mResult, where ours
// keeps all 16 values in registers; the register allocation after that (x19/x27 for the two rects)
// also differs. Everything else lines up.
#line 2920
static void sRenderDrawDefault(
    RWindow & tRWindow,
    IRect2D & rectUIViewport,
    IRect2D & rectMainViewport)
{
    sPrepareReflectionPlanes();















    sRenderProjectionAndReflection(tRWindow);
    sRenderScenes(tRWindow, rectMainViewport);
    sRenderUI(tRWindow, rectUIViewport, rectMainViewport);




}
#line 2905
















































static BOOL sGetViewportRects(const RWindow & tRWindow, IRect2D & rectMainViewport, IRect2D & rectUIViewport) {




    IVector2D vDim = tRWindow.GetPixelDimensions();
    IRect2D rectFullScreenPixels(0, 0, vDim.x, vDim.y);


    rectUIViewport = rectFullScreenPixels;


    const CameraState & cmState = CameraManagerGetCameraState(tRWindow.GetCameraIndex());
    cmState.rectViewport.FromUnitCoords(rectFullScreenPixels, rectMainViewport);




    BOOL fViewportIsValid = rectMainViewport.Width() != 0 && rectMainViewport.Height() != 0;
    return fViewportIsValid;
}







static void sRenderDraw(RWindowMgr & tRWindowMgr, BOOL bFlushPrimitives) {





    if (UI_DrawingFullscreenUI()) {

        if (GfxRenderBegin()) {

            GfxBeginWorker(0);
            IRect2D rectMainViewport;
            IRect2D rectUIViewport;
            RWindow & tMainRWindow = tRWindowMgr.GetMainRWindow();

            BOOL fViewportIsValid = sGetViewportRects(tMainRWindow, rectMainViewport, rectUIViewport);
            if (fViewportIsValid) {

                CameraManagerSetCurrentCamera(tMainRWindow.GetCameraIndex());
                sRenderDrawFullScreenCutscene(tMainRWindow, rectUIViewport);
            }

            GfxEndWorker();
            GfxRenderEnd();
            GfxResetStates();
        }
    } else {


        for (int32 i = 0; i < tRWindowMgr.GetRWindowCount(); i++) {

            RWindow & tRWindow = tRWindowMgr.GetRWindow(i);
            tRWindowMgr.SetCurrentRWindow(tRWindow);

            if (!tRWindow.ShouldRender())
                continue;



            if (!GfxRenderBegin())
                continue;
            GfxBeginWorker(0);
            IRect2D rectMainViewport;
            IRect2D rectUIViewport;
            BOOL fViewportIsValid = sGetViewportRects(tRWindow, rectMainViewport, rectUIViewport);
            if (fViewportIsValid) {

                tRWindow.SetRenderAndDepthTargetToDefaults();
                CameraManagerSetCurrentCamera(tRWindow.GetCameraIndex());

                sRenderDrawDefault(tRWindow, rectUIViewport, rectMainViewport);
            }

            GfxEndWorker();
            GfxRenderEnd();
            GfxResetStates();
        }


        tRWindowMgr.ResetCurrentRWindowToDefault();
        CameraManagerSetCurrentCamera(tRWindowMgr.GetCurrentRWindow().GetCameraIndex());
    }

    if (bFlushPrimitives) {

        SpritesFlush();
        GfxPrimitivesFlush();
    }




    TraceBootStop();

    Blizzard::Time::Microsecond us = Blizzard::Time::Microseconds();



    tRWindowMgr.RenderPresentAll();

    GfxFinalizePerformanceStats();


    for (int32 i = 0; i < tRWindowMgr.GetRWindowCount(); i++) {

        RWindow & tRWindow = tRWindowMgr.GetRWindow(i);
        if (tRWindow.GetRWindowID() == RWindowMainWindow) {

            VariableResRWindowData & tData = tRWindowMgr.GetCurrentRWindow().GetVariableResRWindowData();
            CGameVariableResUpdate(tData);
        }
    }



    s_tRenderFrameRateBoundType = (Blizzard::Time::Microseconds() - us > 500) ? FRAMERATEBOUNDTYPE_GPU : FRAMERATEBOUNDTYPE_RENDERTHREAD;
}








static void sDraw(BOOL fUIOnly, BOOL fAllowDeviceReset);
static void sDrawLoadingScreen();
static void sStopDrawingLoadingScreen(BOOL bResetTooltip);
static void sConnectedToGameServer();
static void sSendLoadCompleteMessage(Player * ptPlayer);

// NON_MATCHING: 5628 vs 5632 on DWARF lines 3086-3308 (fDraw 3190 added). It MATCHES when the code after
// 6352 is numbered 9 or 25 lines later (o01, cause unknown: no line constants differ); see notes.
#line 3086
static void sDraw(
    BOOL fUIOnly,
    BOOL fAllowDeviceReset)
{







    FileReference tRefHighResScreenshot;
    uint32 dwScreenShotTiles;
    uint32 dwScreenShotTilesSq;
    sInitHighResScreenshot(tRefHighResScreenshot, dwScreenShotTiles, dwScreenShotTilesSq);




    if (AppDrawFlagIsSet(91)) {

        SubObjectGfxOcclusionHighlightingSetEnabled(FALSE);
    }







    GameCommonData & tGCD = CGameGetGameCommonData();
    GameMode eGameMode = tGCD.GameModeGet();



    LightsPrepareRender();



    RWindowMgr & tRWindowMgr = *GetRWindowMgr();
    RWindow & tMainRWindow = tRWindowMgr.GetRWindow(0);
    const Camera & camMainView = CameraManagerGetCamera(tMainRWindow.GetCameraIndex());
    Weather::PrepareRender(tMainRWindow.GetWeatherRWindowData());
    SubObjectGfxPrepareRender();














    tMainRWindow.SetLocalPlayerWorldPosition(camMainView.cmState.wvLookAt, TRUE);






    SubObjectSetLocalPlayerIsGhosted(CPlayerIsGhosted() || tGCD.tGameSyncedData.bPVPGameOver ||
                                     (GameIsStartUp() && Console::Screens::GetFrontendBackgroundHeroManager().IsBackgroundHeroDead(0)));


    tRWindowMgr.DisplayListsBegin();


    sRenderUpdate(eGameMode, tRWindowMgr, fUIOnly);






















    ClothManager::CopyToAsyncVertices();


    if (AppDrawFlagIsSet(91) && Math::Abs(camMainView.cmState.flFarPlane - camMainView.cmState.flNearPlane) > 0.000001f) {



        sptAsyncRender->Flush();

        BOOL fDraw = sCheckGfxReady(FALSE);
        if (!fDraw) {

            sptAsyncRender->CancelFrame();
            sptAsyncRender->SwapRenderData(ASYNC_RENDER_DEVICE_AVAILABLE);
            sptAsyncRender->Kick(FALSE, TRUE);
            sptAsyncRender->Flush();
            if (fAllowDeviceReset) {
                sCheckGfxReady(TRUE);
            }
        } else {







            sptAsyncRender->SwapRenderData(ASYNC_RENDER_DEVICE_AVAILABLE);





            for (uint32 i = 0; i < dwScreenShotTilesSq; i++) {

                sPerTileInitHighResScreenshot(i, dwScreenShotTiles);



                CameraManagerPrepareRender();


                BOOL bFlushPrimitives = (i == dwScreenShotTilesSq - 1);

                sptAsyncRender->Kick(TRUE, bFlushPrimitives);




                sPerTileFinishHighResScreenshot(tRefHighResScreenshot, i, dwScreenShotTiles);
            }

            sFinishHighResScreenshot();
        }
        tRWindowMgr.DisplayListsEnd();


        SubObjectGfxOcclusionHighlightingFrameEnd();
    } else {





        Blizzard::Time::Microsecond us = Blizzard::Time::Microseconds();

        sptAsyncRender->Flush();

        if (!sCheckGfxReady(FALSE)) {

            sptAsyncRender->CancelFrame();
            sptAsyncRender->SwapRenderData(ASYNC_RENDER_DEVICE_AVAILABLE);
            sptAsyncRender->Kick(FALSE, TRUE);
            sptAsyncRender->Flush();
            if (fAllowDeviceReset) {
                sCheckGfxReady(TRUE);
            }
        } else {








            if (!(sgtAppGlobals.dwDebugFlags2 & 0x400)) {

                s_tFrameRateBoundType = (Blizzard::Time::Microseconds() - us > 500) ? s_tRenderFrameRateBoundType : FRAMERATEBOUNDTYPE_MAINTHREAD;
                s_tFrameRateBoundTypeCounters[s_tFrameRateBoundType]++;
            }

            sptAsyncRender->SwapRenderData(ASYNC_RENDER_DEVICE_AVAILABLE);


            CameraManagerPrepareRender();

            sptAsyncRender->Kick(FALSE, TRUE);

            if (sgtAppGlobals.dwDebugFlags2 & 0x400) {

                sptAsyncRender->Flush();
                s_tFrameRateBoundType = (s_tRenderFrameRateBoundType == FRAMERATEBOUNDTYPE_RENDERTHREAD) ? FRAMERATEBOUNDTYPE_MAINTHREAD : s_tRenderFrameRateBoundType;
                s_tFrameRateBoundTypeCounters[s_tFrameRateBoundType]++;
            }
        }
    }







    if (AppDrawFlagIsSet(91)) {

        SubObjectGfxOcclusionHighlightingSetEnabled(TRUE);
    }









}
#line 3310



void CGameInitializeGame(const GameParams & tParams) {

    static BOOL sfInitialLaunch = FALSE;
    if (tParams.snoInitialWorld != -1) {


        SNOPackSetInstallationSpeed(FALSE);



        SoundManager::ResetControllers();
        SoundManager::SuppressSounds(0);



        if (!sfInitialLaunch) {
            sfInitialLaunch = TRUE;
        } else {



            CGameLoadingPartRequired(LSC_SNO_MANAGER);
        }



        SNOPreload::InitializeForGame();







        SubsystemsInitialize(sgtCGameSubsystems, sgnNumCGameSubsystems);


        if (tParams.nGameParts == GAME_PARTS_CLIENT) {

            ClientWorldCreate(tParams.snoInitialWorld, -1, tParams.eMode, 0, 0, 0, 0, 0, 0, -1);





        } else if (GameIsNetworked()) {






            XNetAddress tAddress(tParams.idGameConnection, 0);

            ClientConnect(tAddress, sConnectedToGameServer);
        }


        CGameGlobals & tCGameGlobals = CGameGlobalsGet();

        const CmdLineParams * ptCmdLineParams = CmdLineGetParams();
        if (ptCmdLineParams->eTestType) {

            GameTesting::SetTestType(ptCmdLineParams->eTestType);
            if (ptCmdLineParams->nTestDumpFreq)
                GameTesting::SetDumpFPSFequency(ptCmdLineParams->nTestDumpFreq);

            if (ptCmdLineParams->nTestSampleTime)
                GameTesting::SetSampleTime(ptCmdLineParams->nTestSampleTime);

            if (ptCmdLineParams->nTestWaitTime)
                GameTesting::SetWorldAdvanceWaitTime(ptCmdLineParams->nTestWaitTime);

            if (ptCmdLineParams->szTestFilter[0])
                GameTesting::SetFilter(ptCmdLineParams->szTestFilter);

            if (ptCmdLineParams->szTestListPath[0])
                GameTesting::SetListPath(ptCmdLineParams->szTestListPath);

            if (ptCmdLineParams->szTestEntryPoint[0])
                GameTesting::SetEntryPoint(ptCmdLineParams->szTestEntryPoint);

            if (ptCmdLineParams->dwMinFPSScreenshot)
                GameTesting::SetMinFPSToTriggerScreenshot(ptCmdLineParams->dwMinFPSScreenshot);
        }



        AppDrawFlagSet(22, FALSE);
        AppDrawFlagSet(28, FALSE);









        tCGameGlobals.dwFlags |= (1 << CGAMEGLOBALS_GAME_INITIALIZED_BIT);

        UI_GameInitialized();


        SNOPackSetInstallationSpeed(TRUE);


        tCGameGlobals.dwLoadingScreenDoneFrame = sgtAppGlobals.dwFrameCounter;
    }
}







static void sPurgeDefinitionsIfNeeded() {


    BOOL fPurgeNonRequired = TRUE;





















    if (fPurgeNonRequired) {

        DefinitionPurgeNonRequired();
    }
}





void CGameDisposeGame() {



    if (SHELL_APP_STATE_GET() != APPSTATE_FINISHED) {

        CGameProgressLoadingScreen(FALSE);
    }


    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    if (tCGameGlobals.dwFlags & (1 << CGAMEGLOBALS_GAME_INITIALIZED_BIT)) {

        tCGameGlobals.dwFlags &= ~(1 << CGAMEGLOBALS_GAME_INITIALIZED_BIT);








        CGameModeDispose();


        CGameAsyncRenderFlush();
        CGameAsyncRenderGPUFlush();


        PlayerGetCursorMgr().ResetCursorState(FALSE);

        uint32 dwSoundTypesToDelete = ~(uint32)2;
        SoundManager::DeleteAll(dwSoundTypesToDelete, RENDERCONTEXT_INVALID);

        if (tCGameGlobals.idCurrentCWorld != -1) {

            ClientWorldDispose(tCGameGlobals.idCurrentCWorld);
        }


        SNOPreload::DisposeForGame();


        SubsystemsDispose(sgtCGameSubsystems, sgnNumCGameSubsystems);









        sPurgeDefinitionsIfNeeded();
    }



    CGameGlobalsDispose();
}






void CGameGlobalsInit(const GameParams & tParams) {
    AppGlobals & tAppGlobals = sgtAppGlobals;





    tAppGlobals.ptCGameActive->tGameStartParams = tParams;


    tAppGlobals.ptCGameActive->ptCGameGlobals = (CGameGlobals *)SigmaMemoryNew(sizeof(CGameGlobals), 4, NULL, TRUE);
    CGameGlobals & tCGameGlobals = *tAppGlobals.ptCGameActive->ptCGameGlobals;

    tCGameGlobals.Init();

    GameSetActiveGameCommonData(&tAppGlobals.ptCGameActive->tGCD, TRUE);
    tAppGlobals.ptCGameActive->tGCD.Initialize(tParams);











    CGame & tClientGame = CGameGet();

    tClientGame.bLastClientUpdate = TRUE;
    tClientGame.bUpdateUIPositions = FALSE;


    tClientGame.dwRealTicks = 100000;
    tClientGame.dwRealClockTime = 5000000;
    tClientGame.flRealTime = 1666.6667f;
    GameClockInitialize(tClientGame.tRealClock);


    tClientGame.dwClientTicks = 0;
    tClientGame.dwClientClockTime = 0;
    tClientGame.flClientTime = 0.0f;
    GameClockInitialize(tClientGame.tClientClock);

    tCGameGlobals.dwMaxGameTime = tParams.dwMaxGameTime;
    tCGameGlobals.dwPredictedServerTime = 0;
    tCGameGlobals.dwPredictedServerTime = (tParams.dwMaxGameTime == (uint32)-1) ? 0 : tParams.dwMaxGameTime;




    tCGameGlobals.pdefFallbackPrefs = NULL;
    tCGameGlobals.fHardReset = FALSE;
    tCGameGlobals.dwGfxStartTime = 0;

    if (tAppGlobals.dwDebugFlags & 4) {

        NetStatClearList();
    }
}




inline void CGameGlobalsDispose() {
    AppGlobals & tAppGlobals = sgtAppGlobals;



    SigmaMemoryFree(tAppGlobals.ptCGameActive->ptCGameGlobals->pdefFallbackPrefs, NULL);
    tAppGlobals.ptCGameActive->ptCGameGlobals->pdefFallbackPrefs = NULL;
    SigmaMemoryFree(tAppGlobals.ptCGameActive->ptCGameGlobals, NULL);
    tAppGlobals.ptCGameActive->ptCGameGlobals = NULL;








    tAppGlobals.ptCGameActive->tGCD.Dispose();
}












inline void GameAspectRatioChanged() {


    CGame & tClientGame = CGameGet();
    if (AppIsDrawingWorld()) {

        CameraManagerSetAspectRatio(GfxGetAspectRatio());
        UI_AspectRatioChanged();
        tClientGame.bUpdateUIPositions = FALSE;
    } else {



        tClientGame.bUpdateUIPositions = TRUE;
    }
}




float CGameGetLetterboxedViewportNDC(Rect2D & rectScreenNDC) {


    return CGameGetLetterboxedViewportNDC(GfxGetAspectRatio(), rectScreenNDC);
}




float CGameGetLetterboxedViewportNDC(const float flAspectRatio, Rect2D & rectScreenNDC) {



    rectScreenNDC = cgrectViewportDefault;

    if (GfxIsLetterboxed()) {

        float flFrac = Math::Min(flAspectRatio / MAX_RESOLUTION_ASPECT_RATIO, 1.0f);
        float flShift = (1.0f - flFrac) * 0.5f;
        rectScreenNDC.Set(0.0f, flShift, 1.0f, flFrac + flShift);
        return MAX_RESOLUTION_ASPECT_RATIO;
    }



    return flAspectRatio;
}



void CGameGetLetterboxedViewportPixels(IRect2D & rectScreen) {


    Rect2D rectScreenNDC;
    CGameGetLetterboxedViewportNDC(rectScreenNDC);
    ConvertNDCToPixels(rectScreenNDC, rectScreen);
}















void CGameGetLetterboxedViewportUIC(Rect2D & rectUIC) {


    IRect2D rectScreen;
    CGameGetLetterboxedViewportPixels(rectScreen);
    ConvertPixelsToUIC(rectScreen, rectUIC);
}




void CGameGetSafeViewportUIC(Rect2D & rectUIC) {


    CGameGetLetterboxedViewportUIC(rectUIC);

    UIGet().tSafeZone.ApplyToRect(rectUIC);
}




void CGameGfxInit() {


    GfxDynamicStuffInit();
    ParticlesGfxInit();
    CutscenesGfxInit();
    GfxPrimitivesGfxInit();
    RopesGfxInit();
    TrailsGfxInit();
    SpritesGfxInit();

    InputGfxInit();

    UI_GfxInit();
    UIMinimap_GfxInit();
}













void CGameGfxDispose() {


    sStopDrawingLoadingScreen(FALSE);







    UIMinimap_GfxDispose();
    UI_GfxDispose();

    InputGfxDispose();

    SpritesGfxDispose();
    TrailsGfxDispose();
    RopesGfxDispose();
    GfxPrimitivesGfxDispose();
    CutscenesGfxDispose();
    ParticlesGfxDispose();
    GfxDynamicStuffDispose();
}




BOOL sGameGfxInit(BOOL fSkipCGame) {



    GfxInitializeLastState();

    if (!PostFXGfxInit())
        return FALSE;


    if (!SubObjectGfxInit())
        return FALSE;



    if (!fSkipCGame) {

        CGameGfxInit();
    }

    return TRUE;
}




inline void sGameGfxPurgeAssets(BOOL fHardReset) {


    gptGroupDefFont->DeleteAll();
    if (fHardReset) {

        gptGroupDefTexture->DeleteAll();
        gptGroupDefAppearance->DeleteAll();
    } else {














        gptGroupDefTexture->PurgeAllDynamic();
    }
}




void sGameGfxSystemDispose(BOOL fSkipCGame) {
    AppGlobals & tAppGlobals = sgtAppGlobals;


    Prefetch::CancelAllActivePrefetches();


    BOOL fHardReset = !GfxDeviceIsSoftReset();

    if (!tAppGlobals.nGameGfxInitCount) {

        sGameGfxPurgeAssets(fHardReset);
        return;
    }
    tAppGlobals.nGameGfxInitCount = 0;

    if (!fSkipCGame) {

        CGameGfxDispose();
    }

    sGameGfxPurgeAssets(fHardReset);

    SubObjectGfxDispose();
    PostFXGfxDispose();









    GfxIBFreeAll(fHardReset);
    GfxVBFreeAll(fHardReset);
}






void sGameGfxSystemInit(BOOL fSkipCGame) {
    AppGlobals & tAppGlobals = sgtAppGlobals;

    if (tAppGlobals.nGameGfxInitCount)
        return;


    tAppGlobals.nGameGfxInitCount = 1;

    if (!sGameGfxInit(fSkipCGame)) {
        BOOL fTryLowest = TRUE;
        BOOL fInVideoDialog = FALSE;









        sGameGfxSystemDispose(fSkipCGame);
        Prefs::Restore();
        GfxSetPrefsChanged();
        GfxApplyChangedPrefs();
        GfxForceDeviceReset(TRUE);

        GfxDeviceReset();
        if (sGameGfxInit(fSkipCGame)) {
            fTryLowest = FALSE;
        }




        if (fTryLowest) {

            sGameGfxSystemDispose(fSkipCGame);


            Prefs::GetMutableVideoPrefs().SetUseAntialiasing(FALSE);
            Prefs::GetMutableVideoPrefs().SetShadowQuality(0);
            Prefs::GetMutableVideoPrefs().SetReflectionQuality(0);
            GfxSetPrefsChanged();
            GfxApplyChangedPrefs();
            GfxForceDeviceReset(TRUE);
            GfxDeviceReset();

            if (!sGameGfxInit(fSkipCGame)) {













                SNO snoStringList = GlobalSNOGet(714);
                ErrorMessageBox(StringListFetch(snoStringList, "GameGfxInit_ErrorText").str(),
                                StringListFetch(snoStringList, "GameGfxInit_ErrorHeader").str());
                ExitProgram(ERROR_NONE);
            }
        }









        UI_PopDialog(UI_GetInGameDlgStack(), fInVideoDialog);
    }

    GameAspectRatioChanged();
}




void GameGfxSystemInit() {


    sGameGfxSystemInit(TRUE);
}




inline void GameGfxInit() {


    sGameGfxSystemInit(FALSE);
}




inline void GameGfxDispose() {


    sGameGfxSystemDispose(FALSE);
}




void GameGfxSystemDispose() {


    sGameGfxSystemDispose(TRUE);
}




inline void GameReloadRequiredDefinitions() {















    SNOManager::ReloadRequired(SHADER_SNO);
    SNOManager::ReloadRequired(FONT_SNO);
    SNOManager::ReloadRequired(TEXTURE_SNO);
    SNOManager::ReloadRequired(APPEARANCE_SNO);
}




inline void GameUnloadShaders() {





    sStopDrawingLoadingScreen(FALSE);



    gptGroupDefShader->DeleteAll();
}















void GamePause(BOOL bPause) {






    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
        return;














    PauseGameDelta eCancel;
    PauseGameDelta eRequest;
    if (bPause) {
        CGameGlobalsGet().nPauseRefCount = 1;
        eRequest = PAUSEGAMEDELTA_REQUEST_PAUSE;
        eCancel = PAUSEGAMEDELTA_REQUEST_UNPAUSE;
    } else {
        CGameGlobalsGet().nPauseRefCount = 0;
        eRequest = PAUSEGAMEDELTA_REQUEST_UNPAUSE;
        eCancel = PAUSEGAMEDELTA_REQUEST_PAUSE;
    }
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    tCGameGlobals.ePauseGameDelta = (tCGameGlobals.ePauseGameDelta == eCancel) ? PAUSEGAMEDELTA_NO_CHANGE : eRequest;
}
























void GameRefPause(BOOL bPause) {


    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
        return;



    PauseGameDelta eCancel;
    PauseGameDelta eRequest;
    if (bPause) {
        CGameGlobalsGet().nPauseRefCount++;

        if (CGameGlobalsGet().nPauseRefCount > 1)
            return;
        eRequest = PAUSEGAMEDELTA_REQUEST_PAUSE;
        eCancel = PAUSEGAMEDELTA_REQUEST_UNPAUSE;
    } else {














        CGameGlobalsGet().nPauseRefCount--;

        if (CGameGlobalsGet().nPauseRefCount > 0)
            return;
        eRequest = PAUSEGAMEDELTA_REQUEST_UNPAUSE;
        eCancel = PAUSEGAMEDELTA_REQUEST_PAUSE;
    }

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    tCGameGlobals.ePauseGameDelta = (tCGameGlobals.ePauseGameDelta == eCancel) ? PAUSEGAMEDELTA_NO_CHANGE : eRequest;
}












void GameSoftPause(BOOL bPause) {


    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
        return;














    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    if (tCGameGlobals.eSoftPauseGameDelta == (bPause ? SOFTPAUSEGAMEDELTA_REQUEST_UNPAUSE : SOFTPAUSEGAMEDELTA_REQUEST_PAUSE))
        tCGameGlobals.eSoftPauseGameDelta = SOFTPAUSEGAMEDELTA_NO_CHANGE;
    else
        tCGameGlobals.eSoftPauseGameDelta = bPause ? SOFTPAUSEGAMEDELTA_REQUEST_PAUSE : SOFTPAUSEGAMEDELTA_REQUEST_UNPAUSE;
}































static void sCancelChannelledAbilities() {


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {

        if (PlayerIsLocal(ptPlayer)) {

            if (ptPlayer->tClientInfo.snoCurrentLoopingPower != -1) {
                SNODataMessage tMessage;

                tMessage.snoData = ptPlayer->tClientInfo.snoCurrentLoopingPower;
                MessageSendToServer(89, &tMessage, sizeof(tMessage), ptPlayer);

                ptPlayer->tClientInfo.snoCurrentLoopingPower = -1;
            }
        }
    }
}




void CancelAllChannelingAbilities() {


    sCancelChannelledAbilities();
}




static void sClearCachedAbilities() {


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {

        if (PlayerIsLocal(ptPlayer)) {

            CPlayerInputClearCachedPower(ptPlayer);
        }
    }
}


static void sSendGamePauseRequest(BOOL bPause) {






    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
        return;



    if (GameIsWeeklyChallenge() && NephalemRift::IsTieredRiftActive())
        return;




    if (bPause) {

        sCancelChannelledAbilities();

        sClearCachedAbilities();
    }


    if (!PlayersAreAllLocal())
        return;

    BoolDataMessage tMessage;
    tMessage.bVal = bPause;
    MessageSendToServer(413, &tMessage, sizeof(tMessage), LocalPlayerGet());
}





static void sSendGameSoftPauseRequest(BOOL bPause) {






    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
        return;




    if (bPause) {

        sCancelChannelledAbilities();

        sClearCachedAbilities();
    }


    if (GameIsWeeklyChallenge() && NephalemRift::IsTieredRiftActive())
        return;



    if (!PlayersAreAllLocal())
        return;

    BoolDataMessage tMessage;
    tMessage.bVal = bPause;
    MessageSendToServer(473, &tMessage, sizeof(tMessage), LocalPlayerGet());
}





BOOL GameIsPaused() {


    return (CGameGlobalsGet().dwFlags >> CGAMEGLOBALS_GAME_PAUSED_BIT) & 1;
}




BOOL GameIsSoftPaused() {


    return (CGameGlobalsGet().dwFlags >> CGAMEGLOBALS_GAME_SOFT_PAUSED_BIT) & 1;
}




static void sUpdateRealClock(const uint32 dwGameClockTicks, uint32 & dwRealGameTicksElapsed, uint32 & dwRealClockTicksElapsed, float & flRealDeltaTime) {


    CGame & tClientData = CGameGet();
    GameClock & tRealClock = tClientData.tRealClock;




    GameClockAdvanceStart(tRealClock, dwGameClockTicks, dwRealClockTicksElapsed, dwRealGameTicksElapsed, 60);

    BOOL bResetMachineTime = FALSE;

    if (dwRealGameTicksElapsed > 60) {
        bResetMachineTime = TRUE;

        dwRealClockTicksElapsed = 50;
        dwRealGameTicksElapsed = 1;
    }


    GameClockAdvanceComplete(tRealClock, bResetMachineTime, dwRealClockTicksElapsed, dwRealGameTicksElapsed, 60);


    flRealDeltaTime = (float)(tClientData.dwRealClockTime + dwRealClockTicksElapsed) / 3000.0f - tClientData.flRealTime;
}




static BOOL sSlipTheClock(CGame & tCGame, const uint32 dwClientUpdateCount, const uint32 dwTicksPerSec) {




    GameMode eMode = CGameGetGameCommonData().GameModeGet();
    if (dwClientUpdateCount > 10 && eMode == GAMEMODE_HOLODECK)
        return TRUE;






    const int32 CLIENT_SLIP_TIME = 2;

    if (dwClientUpdateCount > dwTicksPerSec || tCGame.dwClientTicks < CLIENT_SLIP_TIME)
        return TRUE;
















    if (CGameGlobalsGet().dwFlags & (1 << CGAMEGLOBALS_WORLD_CHANGED_BIT)) {


        CGameGlobalsGet().dwFlags &= ~(1 << CGAMEGLOBALS_WORLD_CHANGED_BIT);
        return TRUE;
    }

    return FALSE;
}




static void sUpdateClientClock(const uint32 dwAppClockTicks, uint32 & dwGameTicksElapsed, uint32 & dwGameClockTicksElapsed, float & flGameDeltaTime, BOOL & fForceSyncTimes) {
    static BOOL s_fWasActive = FALSE;





    CGame & tCGame = CGameGet();
    GameClock & tClientClock = tCGame.tClientClock;

    AppGlobals & tAppGlobals = sgtAppGlobals;
    uint32 dwTicksPerSec = tAppGlobals.GetTicksPerSec();
    BOOL bResetMachineTime = FALSE;
    s_fSlippedClock = FALSE;



    if (GameClockDebugLag(dwAppClockTicks, tClientClock)) {

        dwGameTicksElapsed = 0;
        dwGameClockTicksElapsed = 0;
        flGameDeltaTime = 0.0f;
        fForceSyncTimes = FALSE;
        return;
    }
    bResetMachineTime = (dwTicksPerSec == 0) || (dwTicksPerSec != tClientClock.dwLastTicksPerSec);

    GameClockAdvanceStart(tClientClock, dwAppClockTicks, dwGameClockTicksElapsed, dwGameTicksElapsed, dwTicksPerSec);

    if (sSlipTheClock(tCGame, dwGameTicksElapsed, dwTicksPerSec)) {

        dwGameTicksElapsed = (dwTicksPerSec > 0) ? 1 : 0;
        dwGameClockTicksElapsed = dwGameTicksElapsed * 50;
        bResetMachineTime = TRUE;
        s_fSlippedClock = TRUE;
    } else {
        CGameGlobals & tCGameGlobals = CGameGlobalsGet();



        if (!s_fWasActive) {



































            dwGameTicksElapsed = (dwTicksPerSec > 0) ? 1 : 0;
            dwGameClockTicksElapsed = dwGameTicksElapsed * 50;
            bResetMachineTime = TRUE;
            s_fSlippedClock = TRUE;
            s_fWasActive = TRUE;
        } else if (!(tCGameGlobals.dwFlags & (1 << CGAMEGLOBALS_CLOCK_SLIP_DISABLED_BIT))) {
            uint32 dwMaxUpdates = 2;
            if (dwGameTicksElapsed > dwMaxUpdates) {
                s_fSlippedClock = TRUE;
                uint32 dwScaledClockTicksToRemove = (dwGameTicksElapsed - dwMaxUpdates) * GAME_CLOCK_TICKS_PER_SECOND / dwTicksPerSec * dwTicksPerSec / 60;
                dwGameClockTicksElapsed -= dwScaledClockTicksToRemove;
                dwGameTicksElapsed = dwMaxUpdates;
            }
        }
    }



    if (SNOManager::HasBlockedLastFrame()) {


        dwGameTicksElapsed = (dwTicksPerSec > 0) ? 1 : 0;
        dwGameClockTicksElapsed = dwGameTicksElapsed * 50;
        bResetMachineTime = TRUE;
        s_fSlippedClock = TRUE;
        s_fWasActive = TRUE;
    }





    GameClockAdvanceComplete(tClientClock, bResetMachineTime, dwGameClockTicksElapsed, dwGameTicksElapsed, dwTicksPerSec);


    if (dwGameClockTicksElapsed) {

        flGameDeltaTime = (float)(tCGame.dwClientClockTime + dwGameClockTicksElapsed) / 3000.0f - tCGame.flClientTime;



        flGameDeltaTime = Math::Max(flGameDeltaTime, 1.0f / 6000.0f);
    } else {
        flGameDeltaTime = 0.0f;
    }
    fForceSyncTimes = bResetMachineTime;
}




static const float TICK_ADJUSTMENT_MAX_PER_TICK = 3.0f;
static const float TICK_ADJUSTMENT_DELTA_MAX = 90.0f;
static const float TICK_ADJUSTMENT_DELTA_MIN = 0.01f;
static const float OO_TICK_ADJUSTMENT_DELTA_MAX = 1.0f / TICK_ADJUSTMENT_DELTA_MAX;

void sClientCheckClock(uint32 uTicksElapsed) {


    CGame & tClientGame = CGameGet();
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    float flDesiredTickOffset = tCGameGlobals.flPredictedTimeDeltaDesired;
    float flTickError = flDesiredTickOffset - tCGameGlobals.flPredictedTimeDeltaCurrent;
    float flABSDelta = Math::Abs(flTickError);
    float flDeltaMax = uTicksElapsed * TICK_ADJUSTMENT_DELTA_MAX;
    float flDeltaMin = uTicksElapsed * TICK_ADJUSTMENT_DELTA_MIN;
    if (flABSDelta > flDeltaMax || flABSDelta < flDeltaMin) {


        tCGameGlobals.dwPredictedServerTime = Math::FloatRoundToInt(flDesiredTickOffset) + tClientGame.dwClientTicks;
        tCGameGlobals.flPredictedTimeDeltaCurrent = flDesiredTickOffset;    } else {



        float flUnitAdjustment = flTickError * OO_TICK_ADJUSTMENT_DELTA_MAX;
        flUnitAdjustment = Math::Pin(flUnitAdjustment, -1.0f, 1.0f);
        float flTickAdjustment = flUnitAdjustment * TICK_ADJUSTMENT_MAX_PER_TICK;
        flTickAdjustment *= uTicksElapsed;

        tCGameGlobals.flPredictedTimeDeltaCurrent += flTickAdjustment;
        tCGameGlobals.dwPredictedServerTime = Math::FloatRoundToInt(tCGameGlobals.flPredictedTimeDeltaCurrent) + tClientGame.dwClientTicks;
    }
}

























































































































































static void sClientVariableUpdate(CGame & tClientData, float flDeltaTime) {





    ParticleGlobalsUpdate();
    RActorsUpdateFullscreenParticleSystems();


    ParentObjectsUpdate(flDeltaTime);



    AttachmentsUpdate();
    RActorsCollisionUpdate();

    ClothManager::Update(flDeltaTime, FALSE);
    CComplexEffectUpdate();
    TrailsClientUpdate(flDeltaTime);
    tClientData.ptExplosionManager->Update(flDeltaTime);
    Wind::Update(flDeltaTime);


    for (int32 x = 0; x < GetRWindowMgr()->GetRWindowCount(); x++) {

        RWindow & tRWindow = GetRWindowMgr()->GetRWindow(x);
        if (tRWindow.ShouldRender()) {

            Weather::Update(tRWindow, flDeltaTime, FALSE);
        }
    }

    ClothManager::WaitForCompletion();
}





static void sValidatePlayerOnGoodGround() {

    SSceneID idSScene = RActorGetSSceneID(LocalPlayerRActorGet());
    if (idSScene == SSCENEID_INVALID)
        return;

    if (SceneCommonDataTryToGet(idSScene))
        return;
    ACDGet(LocalPlayerACDGet()).GetWorldLocation();

    {



        SceneCommonDataIterator tIter;
        SceneCommonData * ptSCD;

        while (SceneCommonDataArrayIterate(tIter, ptSCD, SkipHolodeckScenes)) {

            CRefString sScene;
            CRefString sActivated;
            CRefString sNavMesh;
            sActivated = (ptSCD->dwSceneFlags & 1) ? "Active" : "Inactive";
            sScene = SNOToString(SCENE_SNO, ptSCD->snoScene, FALSE);
            sNavMesh = "None";

            if (ptSCD->ptNavMeshInst) {

                sNavMesh = "Valid";
            }
        }
    }
}












































static void sClientTickUpdate() {




    ClientProcessIncomingMessagesForCurrentTick();

    RActorsUpdateACD();

    sValidatePlayerOnGoodGround();

    sClientCheckClock(1);
}




static void sRenderFrameUpdate(CGame & tClientData, uint32 dwRealUpdateCount, uint32 dwRealClockUpdateCount, float flRealDeltaTime, BOOL bFullscreenCutscenePlaying) {






    if (!bFullscreenCutscenePlaying) {



        CinematographyUpdate(flRealDeltaTime, FALSE);

        Prefetch::Update(FALSE);

        {
            RWindowMgr & tRWindowMgr = *GetRWindowMgr();
            for (int32 i = 0; i < tRWindowMgr.GetRWindowCount(); i++) {
                RWindow & tRWindow = tRWindowMgr.GetRWindow(i);
                if (tRWindow.ShouldRender()) {

                    ParticlesUpdateFullscreenParticles();
                    ShakeUpdate(tRWindow, flRealDeltaTime);
                }
            }
        }
        UI_Update(flRealDeltaTime);

        ParticleSystemDisturbFoliageKick();


        Vibration::Update();

        AmbientSounds::Update();
        SoundManager::Update();
        CutscenesUpdate();
        CPlayersFixedUpdate();
        ChromaEffects::Update();

        ParticleSystemDisturbFoliageWait();


        if (GameIsPaused() || GameIsSoftPaused()) {

            float flSafeRealDeltaTime = Math::Min(flRealDeltaTime, 0.05f);

            gptPhysicsAPIImpl->UpdateAsyncWait();
            gptPhysicsAPIImpl->UpdateAsyncKick(flSafeRealDeltaTime);
            gptPhysicsAPIImpl->Update(flSafeRealDeltaTime);
            gptPhysicsAPIImpl->UpdateAsyncWait();

            ParentObjectsFixedUpdate(flSafeRealDeltaTime);


            ClothManager::Update(flSafeRealDeltaTime, TRUE);
            ClothManager::WaitForCompletion();
        }
    } else {


        Prefetch::Update(FALSE);

        ParticleSystemDisturbFoliageKick();

        Vibration::Update();


        CutscenesUpdate();
        ParticleSystemDisturbFoliageWait();
    }

    tClientData.dwRealTicks += dwRealUpdateCount;
    tClientData.dwRealClockTime += dwRealClockUpdateCount;
    tClientData.flRealTime += flRealDeltaTime;
}




static void sComputeFadeFrustum() {

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    tCGameGlobals.bFadeFrustumValid = FALSE;
    RActorID idPlayerRActor = LocalPlayerRActorGet();
    RActor * ptRActorPlayer = RActorTryToGet(idPlayerRActor);
    if (ptRActorPlayer) {


        const Camera & cam = CameraManagerGetCamera(-1);
        Vector3D wpActor;
        RActorGetCenterPoint(ptRActorPlayer, wpActor);
        tCGameGlobals.tFadeFrustum.Set(wpActor, cam.cmState.transform.wp, cgwvZAxis, 0.6981317f, 0.75f, 0.0f, 100.0f);
        tCGameGlobals.bFadeFrustumValid = TRUE;
    }

    SceneResetRActorFadeGroups();
}



static void sDrawLoadingScreen() {


    CGameGlobals & tGameGlobals = CGameGlobalsGet();
    LoadingScreenData & tLSD = tGameGlobals.tLoadingScreenData;
    const Blizzard::Time::Millisecond TOOLTIP_ROTATION_TIME = 14000;
    Blizzard::Time::Millisecond uNow = Blizzard::Time::Milliseconds();



    if (tLSD.bDrawingInProgress) {

        if (tLSD.bDrawingBlackInProgress != s_tLoadingScreenManager.IsBlackOnly()) {

            tLSD.bDrawingInProgress = FALSE;
            if (!tLSD.bDrawingBlackInProgress) {

                UI_EndDrawLoadingScreen();
            }
        }
    }


    if (!tLSD.bDrawingInProgress) {


        CGameAsyncRenderFlush();
        CGameAsyncRenderGPUFlush();



        GetRWindowMgr()->DisplayListsCompact();

        tLSD.bDrawingBlackInProgress = s_tLoadingScreenManager.IsBlackOnly();
        if (!tLSD.bDrawingBlackInProgress) {

            UI_BeginDrawLoadingScreen();
            if (!s_tLoadingScreenManager.GetLastTooltipChangeTime()) {

                s_tLoadingScreenManager.SetResetTooltip(TRUE);
            }
            if (s_tLoadingScreenManager.GetCurrentTip() != LoadingScreenManager::CURRENT_TIP_NONE) {

                tLSD.nCurrentTip = s_tLoadingScreenManager.GetCurrentTip();
            }
            UI_DisplayLoadingScreenTooltip(FALSE);
        }





        tLSD.bDrawingInProgress = TRUE;
        tLSD.dwTotalElapsedTicks = 0;
        tLSD.dwLastRenderTick = 0;


        CutsceneTryToCancel(GetUIIDNone());
    }


    if (!tLSD.bDrawingBlackInProgress) {

        if (s_tLoadingScreenManager.GetResetTooltip()) {


            tLSD.bHostMigrationTips = Console::Notifications::HostMigrationLoadscreenTipEnabled();
            tLSD.bHostMigrationDisplayFirstTip = TRUE;
        }
        if (s_tLoadingScreenManager.GetResetTooltip() ||
            Blizzard::Time::GetTimeElapsed(s_tLoadingScreenManager.GetLastTooltipChangeTime(), uNow) >= TOOLTIP_ROTATION_TIME) {

            s_tLoadingScreenManager.SetResetTooltip(FALSE);
            s_tLoadingScreenManager.SetLastTooltipChangeTime(uNow);
            UI_DisplayLoadingScreenTooltip(TRUE);
            s_tLoadingScreenManager.SetCurrentTip(tLSD.nCurrentTip);
        }
    }



    uint32 dwCurrent = GetMachineTicks();
    uint32 dwElapsed = dwCurrent - tLSD.dwLastRenderTick;




    const uint32 MAX_TIME_BETWEEN_FRAMES = 100;

    dwElapsed = Math::Min(dwElapsed, MAX_TIME_BETWEEN_FRAMES);
    tLSD.dwTotalElapsedTicks += dwElapsed;

    tLSD.dwLastRenderTick = dwCurrent;








    if (tLSD.bDrawingBlackInProgress) {



        uint32 dwScreenWidth, dwScreenHeight;
        GfxGetCurrentDimensions(dwScreenWidth, dwScreenHeight);

        Rect2D rect;
        rect.Set(0.0f, 0.0f, (float)dwScreenWidth, (float)dwScreenHeight);
        GfxRectDrawDeferred(rect, crgbaBlack, 0.0f, RENDERLAYER_INVALID, 0);
    } else {

        UI_DrawLoadingScreen(tLSD.dwTotalElapsedTicks * 60 / 1000);
    }
}













































static void sStopDrawingLoadingScreen(BOOL bResetTooltip) {



    CGameGlobals * ptCGameGlobals = CGameGlobalsTryToGet();
    if (!ptCGameGlobals)
        return;


    LoadingScreenData & tLSD = ptCGameGlobals->tLoadingScreenData;

    if (ptCGameGlobals->dwFlags & (1 << CGAMEGLOBALS_LOAD_COMPLETED_BIT)) {
        BOOL fLogged = FALSE;

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {

            if (PlayerIsLocal(ptPlayer)) {
                if (!fLogged) {
                    fLogged = TRUE;
                }


















                sSendLoadCompleteMessage(ptPlayer);
            }
        }
    }

    if (tLSD.bDrawingInProgress) {


        if (bResetTooltip) {

            s_tLoadingScreenManager.SetResetTooltip(TRUE);
        }

        tLSD.bDrawingInProgress = FALSE;

        if (!tLSD.bDrawingBlackInProgress) {

            UI_EndDrawLoadingScreen();
        }
    }
}



static void sSendLoadCompleteMessage(Player * ptPlayer) {



    GameMode eMode = CGameGetGameCommonData().GameModeGet();
    if (eMode != GAMEMODE_PLAYING)
        return;




    if (ptPlayer->idPlayerACD != ACDID_INVALID) {

        CPlayerPlayLoadingAnim();
    }

    SimpleMessage tMessage;
    MessageSendToServer(395, &tMessage, sizeof(tMessage), ptPlayer);

    CGameGlobalsGet().dwFlags &= ~(1 << CGAMEGLOBALS_LOAD_COMPLETED_BIT);
}





void CGameWriteQAPerfLogs() {



    GamePerfStats & tPerfStats = GamePerfStatsGet();
    BOOL fPeriodicDumpToTrace = FALSE;



    tPerfStats.nDumpFPSCount++;

    if (tPerfStats.nDumpFPSCount >= tPerfStats.nDumpFPSFreq) {

        tPerfStats.nDumpFPSCount = 0;
        GamePerfCollectAverages();
        fPeriodicDumpToTrace = TRUE;
    }
    CRefString sTitle = CRefString().Format("QA FPS Writes On - %5.1f", tPerfStats.flAvgFPS);
    Vector2D vecUIC(10.0f, 10.0f);
    TextRenderProperties tProps(GlobalSNOGet(637), 13, 1.0f, 1.0f);
    FontStringDrawUIC(tProps, FormattedText(sTitle.str(), 5, FALSE), vecUIC, crgbaWhite, &crgbaTextShadow, 0.0f, TRUE, NULL, RENDERLAYER_INVALID, FALSE);

    if (fPeriodicDumpToTrace && tPerfStats.fileDumpFPS) {


        if (tPerfStats.bFPSCollectionStart) {

            for (int32 i = 0; i < NUM_FRAMERATEBOUNDTYPES; i++) {

                s_tFrameRateBoundTypeCounters[i] = 0;
            }
            tPerfStats.bFPSCollectionStart = FALSE;
            return;
        }

        if (tPerfStats.bDumpFPSFormatJSON) {
            if (!tPerfStats.bDumpFPSStart) {
                tPerfStats.fileDumpFPS->Printf(",\n");
            }
            tPerfStats.bDumpFPSStart = FALSE;
            Blizzard::Time::Timestamp utcTime = Blizzard::Time::GetTimestamp();
            Blizzard::Time::TimeRec timeRec;
            Blizzard::Time::BreakTime(utcTime, timeRec);
            CRefString datetimeStr;
            datetimeStr.Format("%04d-%02d-%02d %02d:%02d:%02d.%09u UTC", timeRec.year, timeRec.month, timeRec.day, timeRec.hour, timeRec.minute, timeRec.second, timeRec.nanosecond);

            tPerfStats.fileDumpFPS->Printf("\t\t\t\t{\n");
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"datetime\" : \"%s\",\n", datetimeStr.str());
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"fps\" : %6.2f,\n", tPerfStats.flAvgFPS);
        } else {

            tPerfStats.fileDumpFPS->Printf("%6.2f", tPerfStats.flAvgFPS);
        }

        const GfxPerfStats & tGfxStats = *GfxGetPerfStats();
        const GfxPerfStatsFrame & tFrameStats = tGfxStats.tLast;

        if (tPerfStats.bDumpFPSFormatJSON) {

            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"batches\" : [");
            for (int32 i = 0; i < 6; i++) {


                if (i > 0)
                    tPerfStats.fileDumpFPS->Printf(", ");
                tPerfStats.fileDumpFPS->Printf("%d", tFrameStats.tBinStats[i].nDrawCalls);
            }
            tPerfStats.fileDumpFPS->Printf("],\n");

            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"tris\" : [");
            for (int32 i = 0; i < 6; i++) {


                if (i > 0)
                    tPerfStats.fileDumpFPS->Printf(", ");
                tPerfStats.fileDumpFPS->Printf("%d", tFrameStats.tBinStats[i].dwPolys);
            }
            tPerfStats.fileDumpFPS->Printf("],\n");
        } else {


            for (int32 i = 0; i < 6; i++) {
                tPerfStats.fileDumpFPS->Printf(" bin(%d batches(%d) tris(%d))", i,
                                               tFrameStats.tBinStats[i].nDrawCalls, tFrameStats.tBinStats[i].dwPolys);
            }
            tPerfStats.fileDumpFPS->Printf("\n");
        }

































        ParticleCounts tParticleCounts;
        ParticlesGetCount(tParticleCounts);
        if (tPerfStats.bDumpFPSFormatJSON) {
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"num_particle_systems\" : %d,\n", tParticleCounts.nSystemCurrentTotal);
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"num_particles\" : %d,\n", tParticleCounts.nParticlesCurrentTotal);
        } else {

            tPerfStats.fileDumpFPS->Printf("Total Particle Systems %d. Total Particles %d.\n",
                                           tParticleCounts.nSystemCurrentTotal, tParticleCounts.nParticlesCurrentTotal);
        }


        dmProfile profile;
        gptPhysicsAPIImpl->GetProfile(&profile);
        if (tPerfStats.bDumpFPSFormatJSON) {
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"domino_step_ms\" : %6.2f,\n", profile.step);
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"num_static_rigid_bodies\" : %d,\n", profile.staticCount);
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"num_kinematic_rigid_bodies\" : %d,\n", profile.kinematicCount);
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"num_dynamic_rigid_bodies\" : %d,\n", profile.dynamicCount);
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"num_awake_bodies\" : %d,\n", profile.awakeCount);
        } else {

            tPerfStats.fileDumpFPS->Printf("Domino Step (ms) %6.2f. Static Body Count %d. Kinematic Body Count %d. Dynamic Body Count %d. Awake Body Count %d\n",
                                           profile.aveStep, profile.staticCount, profile.kinematicCount, profile.dynamicCount, profile.awakeCount);
        }

        float flVariableResPercent = RWindowMgr().GetMainRWindow().GetVariableResRWindowData().flPercent;
        uint32 dwAvgHistoryUs = RWindowMgr().GetMainRWindow().GetVariableResRWindowData().dwAvgHistoryUs;
        if (tPerfStats.bDumpFPSFormatJSON) {
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"variable_res_percent\" : %6.2f,\n", flVariableResPercent);
            tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"device_frame_us\" : %d,\n", dwAvgHistoryUs);
        } else {

            tPerfStats.fileDumpFPS->Printf("Variable Res Percent %6.2f. Device Frame us: %d\n", flVariableResPercent, dwAvgHistoryUs);
        }


        if (tPerfStats.bDumpFPSFormatJSON) {

            for (int32 i = 0; i < NUM_FRAMERATEBOUNDTYPES; i++) {

                if (i == NUM_FRAMERATEBOUNDTYPES - 1) {
                    tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"%s bound\" : \"%d\"\n", sFrameRateBoundTypeToString((FrameRateBoundTypes)i), s_tFrameRateBoundTypeCounters[i]);
                } else {


                    tPerfStats.fileDumpFPS->Printf("\t\t\t\t\t\"%s bound\" : \"%d\",\n", sFrameRateBoundTypeToString((FrameRateBoundTypes)i), s_tFrameRateBoundTypeCounters[i]);
                }
                s_tFrameRateBoundTypeCounters[i] = 0;
            }

            tPerfStats.fileDumpFPS->Printf("\t\t\t\t}");
        } else {



            for (int32 i = 0; i < NUM_FRAMERATEBOUNDTYPES; i++) {

                tPerfStats.fileDumpFPS->Printf("\"%s bound\" : %d\"\n", sFrameRateBoundTypeToString((FrameRateBoundTypes)i), s_tFrameRateBoundTypeCounters[i]);
                s_tFrameRateBoundTypeCounters[i] = 0;
            }
            tPerfStats.fileDumpFPS->Printf("\n");
        }
    }
}
















































BOOL CGameLoadingScreenCanProgress() {

    CGameGlobals * ptCGameGlobals = CGameGlobalsTryToGet();
    if (ptCGameGlobals && (ptCGameGlobals->dwFlags & ((1 << CGAMEGLOBALS_GAME_INITIALIZED_BIT) | (1 << CGAMEGLOBALS_IN_UPDATE_RENDER_BIT))) == (1 << CGAMEGLOBALS_GAME_INITIALIZED_BIT))
        return TRUE;




    return FALSE;
}



void CGameProgressLoadingScreen(BOOL bInTightLoop) {







    if (!CGameLoadingScreenCanProgress())
        return;

    CameraManagerResetCameras();
    sDrawLoadingScreen();

    sDraw(TRUE, FALSE);

    GamePerfNewFrame();
    GfxNewFrame();


    if (bInTightLoop) {

        s_tLoadingScreenManager.Update();

        ShellEventLoop();
    }
















    SoundManager::Update();
}











void CGameClientUpdateLock() {


    BOOL fMainThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_MAIN;
    Blizzard::Time::Millisecond dwCurMS = Blizzard::Time::Milliseconds();
    if (fMainThread) {
        AppGlobals & tAppGlobals = sgtAppGlobals;
        Blizzard::Lock::Semaphore & tClientUpdateLock = tAppGlobals.ptCGameActive->tClientUpdateLock;


        while (!tClientUpdateLock.Wait(1)) {

            Prefetch::Update(FALSE);
            if (Blizzard::Time::Milliseconds() - dwCurMS > 16) {

                dwCurMS = Blizzard::Time::Milliseconds();
            }
        }
    } else {
        AppGlobals & tAppGlobals = sgtAppGlobals;



        tAppGlobals.ptCGameActive->tClientUpdateLock.WaitForever();
    }
}




void CGameClientUpdateUnlock() {
    AppGlobals & tAppGlobals = sgtAppGlobals;



    tAppGlobals.ptCGameActive->tClientUpdateLock.Set();
}





static void sClientFrameUpdate() {



    CActivationUpdate();

    TrailsUpdate();
    LightsUpdate();
    Sky::Update();
    CWorldUpdate();
    CBlizzconTimerUpdate();
    CPlayersUpdate();
    RActorUpdateQueued();


    Console::UpdateControllerDebugView();
    Console::UpdateTargetDebugging();
}





static void sUpdateSimulation(uint32 dwClientGameUpdateCount, uint32 dwClientClockUpdateCount, float flClientDeltaTime) {




    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    gptPhysicsAPIImpl->UpdateAsyncWait();


    uint32 dwRemainingClientTicks = dwClientGameUpdateCount;
    uint32 dwRemainingClientClockTicks = dwClientClockUpdateCount;
    float flRemainingClientDeltaTime = flClientDeltaTime;
    const uint32 dwMaxClockTimeStep = 100;
    const uint32 dwDefaultClockTimeStep = 50;
    BOOL fUpdated;







    CGame & tClientData = CGameGet();















    do {
        fUpdated = FALSE;



        if (dwRemainingClientTicks) {



            sClientTickUpdate();

            tClientData.dwClientTicks++;
            dwRemainingClientTicks--;
            fUpdated = TRUE;
        }

        if (dwRemainingClientClockTicks > dwMaxClockTimeStep || (dwRemainingClientClockTicks && dwRemainingClientTicks == 0)) {





            uint32 dwTimeStep = (dwRemainingClientClockTicks >= dwMaxClockTimeStep) ? dwDefaultClockTimeStep : dwRemainingClientClockTicks;



            dwRemainingClientClockTicks -= dwTimeStep;
            tClientData.bLastClientUpdate = (dwRemainingClientClockTicks == 0);

            float flTimeStep = dwRemainingClientClockTicks ? Math::Min(flRemainingClientDeltaTime, dwTimeStep / 3000.0f) : flRemainingClientDeltaTime;


            flTimeStep = Math::Max(flTimeStep, 1.0f / 6000.0f);

            tClientData.dwClientClockTicksElapsed = dwTimeStep;
            tClientData.flClientDeltaTime = flTimeStep;


            tClientData.bUpdateRigidBodyPhysics = (dwRemainingClientClockTicks == 0);
            if (tClientData.bUpdateRigidBodyPhysics) {

                gptPhysicsAPIImpl->Update(flClientDeltaTime);
            }






            sComputeFadeFrustum();

            sClientVariableUpdate(tClientData, flTimeStep);

            tCGameGlobals.bFadeFrustumValid = FALSE;

            tClientData.dwClientClockTime += dwTimeStep;
            tClientData.flClientTime += flTimeStep;


            flRemainingClientDeltaTime -= flTimeStep;
            fUpdated = TRUE;
        }
    } while (fUpdated);
    tClientData.dwClientClockTicksElapsed = dwClientClockUpdateCount;
    tClientData.flClientDeltaTime = flClientDeltaTime;




    sClientFrameUpdate();

    gptPhysicsAPIImpl->UpdateAsyncKick(Math::Min(flClientDeltaTime, 0.05f));
}




static void sRetrieveUpdateCount(uint32 dwAppClockTicks, uint32 & dwRealGameUpdateCount, uint32 & dwRealClockUpdateCount, float & flRealDeltaTime, uint32 & dwGameUpdateCount, uint32 & dwGameClockUpdateCount, float & flGameDeltaTime, BOOL & fForceSyncTimes) {









    CGame & tClientData = CGameGet();
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    sUpdateRealClock(dwAppClockTicks, dwRealGameUpdateCount, dwRealClockUpdateCount, flRealDeltaTime);



    sUpdateClientClock(dwAppClockTicks, dwGameUpdateCount, dwGameClockUpdateCount, flGameDeltaTime, fForceSyncTimes);

    if (tCGameGlobals.dwFlags & ((1 << CGAMEGLOBALS_GAME_PAUSED_BIT) | (1 << CGAMEGLOBALS_GAME_SOFT_PAUSED_BIT))) {


        if (tClientData.dwClientTicks) {

            dwGameUpdateCount = 0;
            dwGameClockUpdateCount = 0;
            flGameDeltaTime = 0.0f;
            fForceSyncTimes = FALSE;
        }
    }
}

// NON_MATCHING: 24 bytes long; register allocation differs and the inlined sSlipTheClock
// uses a branch where the original uses a select.

BOOL ClientIdle(uint32 dwGameClockTicks) {



    ScopedClientUpdateLock tClientUpdate;

    s_tLoadingScreenManager.Update();












    if (GameIsServerOnly())
        return FALSE;


















    SNOManager::Update();











    CGame & tClientData = CGameGet();
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    CNetworkReadUpdate();
    Hub::Process();














    BOOL bCanRunIdleLoop = tCGameGlobals.bCanRunIdleLoop;





































    Console::UISaveIcon::Update();


    if (!GameLoopRunning() ||
        !bCanRunIdleLoop) {





        if (InputIsCursorInWindow() || InputAppHasCapture()) {





            InputCursorSet(CURSOR_EDITSELECT);
        }


        CGameProgressLoadingScreen(FALSE);






















        Prefetch::Update(FALSE);


        ClientSendOutgoingMessages();

        return FALSE;
    }






    BOOL bPrevTraceAllocs = tClientData.dwClientTicks ? SigmaMemorySetTraceAllocations(TRUE) : FALSE;



    uint32 dwRealUpdateCount;
    uint32 dwRealClockUpdateCount;
    float flRealDeltaTime;
    uint32 dwClientUpdateCount;
    BOOL fUpdatedInput = FALSE;
    uint32 dwClientClockUpdateCount;
    float flClientDeltaTime;
    BOOL fForceSyncTime;

    sRetrieveUpdateCount(dwGameClockTicks, dwRealUpdateCount, dwRealClockUpdateCount, flRealDeltaTime, dwClientUpdateCount, dwClientClockUpdateCount, flClientDeltaTime, fForceSyncTime);
    tClientData.dwRealClockTicksElapsed = dwRealClockUpdateCount;
    tClientData.flRealDeltaTime = flRealDeltaTime;

    if ((tCGameGlobals.dwFlags & (1 << CGAMEGLOBALS_GAME_PAUSED_BIT)) && tClientData.dwClientTicks) {




        dwClientUpdateCount = 0;
    }


    if (tClientData.dwClientTicks && (tCGameGlobals.dwFlags & (1 << CGAMEGLOBALS_GAME_SOFT_PAUSED_BIT))) {




        dwClientUpdateCount = 0;
    }


    BOOL bFullScreenCutscenePlaying = (CutsceneGetFullScreenCutscene() != CUTSCENEID_INVALID);


    if (dwRealUpdateCount || tClientData.dwClientTicks) {


        InputUpdate();
        fUpdatedInput = TRUE;
        if (!bFullScreenCutscenePlaying) {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {

                if (PlayerIsLocal(ptPlayer)) {

                    PlayerInputUpdate(ptPlayer);
                }
            }
        }
    }









    if (!bFullScreenCutscenePlaying) {

        sUpdateSimulation(dwClientUpdateCount, dwClientClockUpdateCount, flClientDeltaTime);


        if (fForceSyncTime) {

            tClientData.dwClientClockTime = tClientData.dwClientTicks * GAME_CLOCK_TICKS_PER_SECOND / 60;
            tClientData.flClientTime = tClientData.dwClientClockTime / 3000.0f;
        }
    } else {


        sClientCheckClock(dwClientUpdateCount);
        ClientProcessIncomingMessagesForCurrentTick();
    }

    if (!dwClientUpdateCount) {

        if (tCGameGlobals.dwFlags & (1 << CGAMEGLOBALS_GAME_SOFT_PAUSED_BIT)) {

            CPlayersUpdate();
            ClientProcessIncomingMessagesForCurrentTick();

        } else if (tCGameGlobals.dwFlags & (1 << CGAMEGLOBALS_GAME_PAUSED_BIT)) {

            ClientProcessIncomingMessagesForCurrentTick();
        }
    }



    if (CGameGlobalsGet().ePauseGameDelta == PAUSEGAMEDELTA_REQUEST_PAUSE) {


        if (!CGameLoadingScreenIsEnabled()) {

            sSendGamePauseRequest(TRUE);
            CGameGlobalsGet().ePauseGameDelta = PAUSEGAMEDELTA_NO_CHANGE;
        }
    } else if (CGameGlobalsGet().ePauseGameDelta == PAUSEGAMEDELTA_REQUEST_UNPAUSE) {


        sSendGamePauseRequest(FALSE);
        CGameGlobalsGet().ePauseGameDelta = PAUSEGAMEDELTA_NO_CHANGE;

    } else if (CGameGlobalsGet().eSoftPauseGameDelta == SOFTPAUSEGAMEDELTA_REQUEST_PAUSE) {


        if (!CGameLoadingScreenIsEnabled()) {

            sSendGameSoftPauseRequest(TRUE);
            CGameGlobalsGet().eSoftPauseGameDelta = SOFTPAUSEGAMEDELTA_NO_CHANGE;
        }
    } else if (CGameGlobalsGet().eSoftPauseGameDelta == SOFTPAUSEGAMEDELTA_REQUEST_UNPAUSE) {


        sSendGameSoftPauseRequest(FALSE);
        CGameGlobalsGet().eSoftPauseGameDelta = SOFTPAUSEGAMEDELTA_NO_CHANGE;
    }

    ClientSendOutgoingMessages();

    BOOL fLoadingScreenDrawn;



    if (CGameLoadingScreenIsEnabled()) {

        sDrawLoadingScreen();
        fLoadingScreenDrawn = TRUE;
    } else {


        sStopDrawingLoadingScreen(TRUE);
        fLoadingScreenDrawn = FALSE;
    }
    if (!tClientData.dwClientTicks)
        return FALSE;




    CGameGlobalsGet().dwFlags |= (1 << CGAMEGLOBALS_IN_UPDATE_RENDER_BIT);



    sRenderFrameUpdate(tClientData, dwRealUpdateCount, dwRealClockUpdateCount, flRealDeltaTime, bFullScreenCutscenePlaying);


    CameraManagerRestoreCurrentCameraToMainCamera();




    BOOL fDraw = (tClientData.dwRealTicks != 100000);




    if (fDraw) {




        sDraw(fLoadingScreenDrawn, TRUE);



        GamePerfNewFrame();
        GfxNewFrame();
    }

    CGameGlobalsGet().dwFlags &= ~(1 << CGAMEGLOBALS_IN_UPDATE_RENDER_BIT);

    if (fUpdatedInput) {

        InputResetMouseWheelDelta();
    }


    if (!CGameLoadingScreenIsEnabled()) {



        CScenesDeleteAllFlagged();
    }











    SigmaMemorySetTraceAllocations(bPrevTraceAllocs);

    return fDraw;
}











BOOL CGameLoadingScreenHasExpired() {


    return FALSE;
}






BOOL CGameLoadingScreenIsEnabled() {


    return s_tLoadingScreenManager.IsEnabled();
}




















BOOL CGameLoadingScreenIsBlackOnly() {


    return s_tLoadingScreenManager.IsBlackOnly();
}






void CGameLoadingScreenSetBlackOnly(BOOL bBlackOnly) {


    s_tLoadingScreenManager.SetBlackOnly(bBlackOnly);
}







void CGameLoadingScreenEnable(const LoadingComponent eComponent) {


    s_tLoadingScreenManager.Enable();
}














void CGameLoadingScreenDisable(const LoadingComponent eComponent) {



    s_tLoadingScreenManager.Disable();
}
















void CGameLoadingScreenScheduleFade(float flDuration, UIFade::FadeColor eFadeColor) {


    s_tLoadingScreenManager.ScheduleFade(flDuration, eFadeColor);
}




void CGameLoadingPartCompleted(const LoadingComponent eComponent) {


    s_tLoadingScreenManager.Completed(eComponent);
}



void CGameLoadingPartRequired(const LoadingComponent eComponent) {


    s_tLoadingScreenManager.Required(eComponent);
}



void CGameServerLoadingPartCompleted(const LoadingComponent eComponent) {


    s_tLoadingScreenManager.ServerCompleted(eComponent);
}



void CGameServerLoadingPartRequired(const LoadingComponent eComponent) {


    s_tLoadingScreenManager.ServerRequired(eComponent);
}





void GameSetDrawFlagsToDefaults() {



    CGameGlobalsGet().dwDrawFlags = 0;
}















SNO CGameGetCurrentWorld() {



    return CGameGlobalsGet().snoCurrentWorld;
}




SWorldID CGameGetCurrentSWorldID()

{
	CGameGlobals & tCGameGlobals = CGameGlobalsGet();
	return tCGameGlobals.idCurrentSWorld;
}



#line 6319
GameCommonData & CGameGetGameCommonData()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;









	return tAppGlobals.ptCGameActive->tGCD;
}




CGameGlobals & CGameGlobalsGet()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;








	return *tAppGlobals.ptCGameActive->ptCGameGlobals;
}
#line 6352





CGameGlobals * CGameGlobalsTryToGet() {
    AppGlobals & tAppGlobals = sgtAppGlobals;


    if (tAppGlobals.ptCGameActive) {
        return tAppGlobals.ptCGameActive->ptCGameGlobals;
    }









    return NULL;
}



void CGameFirstClock(const uint32 dwFirstClock) {


    CGame & tClientGame = CGameGet();
    GameCommonData & tGCD = tClientGame.tGCD;
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();


    tGCD.dwServerTime = dwFirstClock - 1;
    tCGameGlobals.dwMaxGameTime = tCGameGlobals.dwPredictedServerTime = tGCD.dwServerTime;

    tCGameGlobals.flPredictedTimeDeltaDesired = 0.0f;
    tCGameGlobals.flPredictedTimeDeltaCurrent = 0.0f;


    tCGameGlobals.flLagCompensationTicks = 0.0f;
    tCGameGlobals.dwClientTimeLastHeartBeat = tClientGame.dwClientTicks;
    tCGameGlobals.dwFlags |= (1 << CGAMEGLOBALS_ADJUST_CLIENT_CLOCK_BIT);



    ClientReceiveHeartbeat(dwFirstClock - 1);
}




void CGameStartTick(uint32 dwTick) {









    CGameGetGameCommonData().dwServerTime = dwTick;
}




void CGameReceiveHeartbeat(uint32 dwHeartBeatTime) {






    CGame & tClientGame = CGameGet();
    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    tCGameGlobals.flPredictedTimeDeltaDesired = (float)(int32)(dwHeartBeatTime - tClientGame.dwClientTicks);

    tCGameGlobals.dwMaxGameTime = dwHeartBeatTime;
    tCGameGlobals.dwClientTimeLastHeartBeat = tClientGame.dwClientTicks;
    tCGameGlobals.dwFlags |= (1 << CGAMEGLOBALS_ADJUST_CLIENT_CLOCK_BIT);

    if (!tCGameGlobals.bCanRunIdleLoop) {
        AppGlobals & tAppGlobals = sgtAppGlobals;

        float fTime = tAppGlobals.tStartTimeStopwatch.Check();




        tCGameGlobals.bCanRunIdleLoop = TRUE;
    }
}




NetStat * CGameGetNetStat() {





    return CGameGet().ptNetStats;
}




static void sConnectedToGameServer() {


















































    Console::Online::PartyMember atMembers[4];
    uint32 uNumMembers = Console::Online::PartyGetLocalMembers(atMembers, TRUE);

    for (uint32 i = 0; i < uNumMembers; i++) {



        CPlayerLocalPlayerJoin(atMembers[i].nControllerIndex);
    }



    ClientSendOutgoingMessages();
}




uint32 CGameGetPredictedServerTime() {




    return CGameGlobalsGet().dwPredictedServerTime;
}




BOOL CGameDisplayHealNumbers() {


    return Prefs::GetGameplayPrefs().GetShowHealNumbers();
}




BOOL CGameDisplayDamageNumbers() {


    return Prefs::GetGameplayPrefs().GetShowDamageNumbers();
}



BOOL CGameDisplayDefensiveMessages() {


    return Prefs::GetGameplayPrefs().GetShowDefensiveMessages();
}



BOOL CGameDisplayCriticals() {


    return Prefs::GetGameplayPrefs().GetShowCriticals();
}



BOOL CGameInScriptedSequence() {



    return CGameGlobalsGet().bInScriptedSequence;
}






























static void sGfxChangeCleanup() {


    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    SigmaMemoryFree(tCGameGlobals.pdefFallbackPrefs, NULL);
    tCGameGlobals.dwGfxStartTime = 0;
    tCGameGlobals.pdefFallbackPrefs = NULL;
    tCGameGlobals.pfnOnAccept = NULL;
    tCGameGlobals.pfnOnCancel = NULL;
}

static void OnAccept(const UIID &) {


    CGameGlobals & tCGameGlobals = CGameGlobalsGet();








    if (tCGameGlobals.pfnOnAccept) {

        tCGameGlobals.pfnOnAccept(GetUIIDNone());
    }

    sGfxChangeCleanup();
}




static void sGfxChangeCancel() {


    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    Prefs::GetMutableVideoPrefs().tVideoPrefsToConfirm = *tCGameGlobals.pdefFallbackPrefs;



    GfxSetPrefsChanged();
    GfxForceDeviceReset(tCGameGlobals.fHardReset);








    if (tCGameGlobals.pfnOnCancel) {

        tCGameGlobals.pfnOnCancel(GetUIIDNone());
    }

    sGfxChangeCleanup();
}



static void OnCancel(const UIID &) {

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();
    tCGameGlobals.dwGfxStartTime = 0;
    sGfxChangeCancel();
}



static BOOL sCheckForGfxResetFallback() {


    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    if (!tCGameGlobals.pdefFallbackPrefs)
        return FALSE;


    const uint32 dwTimeOut = 15000;

    uint32 dwElapsed = GetMachineTicks() - tCGameGlobals.dwGfxStartTime;
    int32 dwSecsRemaining = (dwElapsed >= dwTimeOut) ? 0 : (dwTimeOut - dwElapsed) / 1000;

    CRefString sMsg = LocalizedSprintf(StringListFetch(GlobalSNOGet(706), "AcceptOptions"),
                                       UnicodeText::FormatNumber(dwSecsRemaining, TRUE, FNS_NORMAL, 0).str(), NULL);






    UIOptionsConfirmation::Show(sMsg, OnAccept, OnCancel);

    if (dwElapsed >= dwTimeOut) {


        UIOptionsConfirmation::CancelAndClose();
        return TRUE;
    }

    return FALSE;
}



void CGameGlobals::Init() {



    eCameraMode = CAMERAMODE_NORMAL;

    flLastLocalGameTimeUpdate = 0.0f;
    bCanRunIdleLoop = FALSE;
    CGameLoadingScreenDisable(LSC_ALL);

    tLoadingScreenData.dwTotalElapsedTicks = 0;
    tLoadingScreenData.dwLastRenderTick = 0;
    tLoadingScreenData.nCurrentTip = 0;
    tLoadingScreenData.bDrawingInProgress = FALSE;
    tLoadingScreenData.nMaxLocalHeroLevel = 0;

    bPVPPaused = FALSE;
    bInScriptedSequence = FALSE;


    flPredictedTimeDeltaDesired = 0.0f;
    flPredictedTimeDeltaCurrent = 0.0f;
    flLagCompensationTicks = 0.0f;
    nClientLatencySmoothBufferTime = 0;
    dwClientTimeLastHeartBeat = 0;
    snoCurrentWorld = -1;
    idCurrentCWorld = -1;
    idCurrentSWorld = -1;

    RandInit(rngLocal);

    dwFlags &= ~(1 << CGAMEGLOBALS_DISPLAY_MAIN_ACTOR_STATE_BIT);

    snoLevelAreaSavePoint = -1;
    snoLevelAreaHearthPortal = -1;
    bHearthPortalIsTieredRift = FALSE;
    snoLevelAreaReturnPoint = -1;

    flDungeonFinderProgress = 0.0f;
    nDungeonFinderSeed = 0;
    nDungeonFinderParticipatingPlayerCount = 0;
    snoDungeonFinderTimedEvent = -1;
    nChallengeRiftCompletionMilliseconds = -1;
    sChallengeRiftCompletionMillisecondsHeroName = CRefString();

    nChallengeRiftSprintMilliseconds = 0;

    ePauseGameDelta = PAUSEGAMEDELTA_NO_CHANGE;
    eSoftPauseGameDelta = SOFTPAUSEGAMEDELTA_NO_CHANGE;
}




CameraMode CGameGlobals::GetCameraMode() const {


    return eCameraMode;
}




void CGameGlobals::SetCameraMode(const CameraMode _eCameraMode) {


























    switch (_eCameraMode) {
        case CAMERAMODE_NORMAL:










            CGameModeResetCameraToFollowPlayer();
            break;
        case CAMERAMODE_FREE_CAMERA:
            CinematographyStop();
            break;
        case CAMERAMODE_AV:










            CinematographySetState(CINEMATOGRAPHY_STATE_3);
            break;
        case CAMERAMODE_TOP_DOWN:
            CinematographyStop();
            break;
    }
    eCameraMode = _eCameraMode;
}


DebugSoundMode CGameGetDebugSoundMode() {


    return sgtAppGlobals.eDebugSoundMode;
}




BOOL CGameTraceAttachmentsEnabled() {

    CGameGlobals * ptCGameGlobals = CGameGlobalsTryToGet();

    if (!ptCGameGlobals)
        return FALSE;



    return (ptCGameGlobals->dwFlags >> CGAMEGLOBALS_TRACE_ATTACHMENTS_BIT) & 1;
}



void CGameSetLoadComplete(Player * ptPlayer) {




    CGameGlobals * ptCGameGlobals = CGameGlobalsTryToGet();
    if (ptCGameGlobals) {

        if (ptCGameGlobals->dwFlags & (1 << CGAMEGLOBALS_LOAD_COMPLETED_BIT))
            return;



        ptCGameGlobals->dwFlags |= (1 << CGAMEGLOBALS_LOAD_COMPLETED_BIT);
    }













    if (!CGameLoadingScreenIsEnabled()) {






        sSendLoadCompleteMessage(ptPlayer);
    }
}




BOOL CGameAsyncRenderFlush() {


    if (sptAsyncRender) {

        return sptAsyncRender->Flush();
    }


    return FALSE;
}



























void CGameAsyncRenderGPUFlush() {


    if (sptAsyncRender) {
        sptAsyncRender->GPUFlush();
    }
}



















uint32 CGameAsyncGetDrawFrameCount() {


    if (sptAsyncRender) {
        return sptAsyncRender->GetDrawFrameCount();
    }
    return 0;
}






uint32 CGameAsyncGetUpdateFrameCount() {


    if (sptAsyncRender) {
        return sptAsyncRender->GetUpdateFrameCount();
    }
    return 0;
}

#line 7005
uint32 CGameAsyncGetRenderDataIndex(
    AsyncRenderDataIndex eIndex)
{
    if (sptAsyncRender)
    {
        return sptAsyncRender->GetRenderDataIndex(eIndex);
    }
    return 0;

}





BOOL CGameAsyncRenderRegisterDelayedCallback(AsyncRenderCallback callback, void * pParam1, void * pParam2) {




    if (sptAsyncRender) {

        return sptAsyncRender->RegisterDelayedCallback(callback, pParam1, pParam2);
    }


    return FALSE;
}



































void CGameAsyncRenderRegisterGPUResourceDelayedDeletion(AsyncRenderCallback callback, void * pParam1, void * pParam2) {




    if (sptAsyncRender) {
        sptAsyncRender->RegisterGPUResourceDelayedDeletion(callback, pParam1, pParam2);
    } else {



        callback(pParam1, pParam2);
    }
}






Blizzard::Lock::RSpinlock stMemoryParallelLock;

void * CGameAsyncRenderGetMemory(uint32 dwSize) {


    if (sptAsyncRender) {
        Blizzard::Lock::SpinlockEnter(stMemoryParallelLock);



        void * pMemory = sptAsyncRender->GetMemory(dwSize);
        Blizzard::Lock::SpinlockLeave(stMemoryParallelLock);
        return pMemory;
    }
    return NULL;
}





























void CGameAsyncRenderInit() {


    sptAsyncRender = new AsyncRender(0x40000, 0x10000);
}




void CGameAsyncRenderDispose() {


    delete sptAsyncRender;
    sptAsyncRender = NULL;
}


















AsyncRender::AsyncRender(uint32 dwAsyncMemoryBufferSize, uint32 dwAsyncMemoryExtensionBuffersSize)
    : m_dwGPUResourceDelayedDeleteItemCount(0)

    , m_bFlushPrimitives(TRUE)
    , m_bUsingRenderWorker(FALSE)
    , m_fDone(FALSE)
    , m_fSynched(FALSE)
    , m_dwMemoryBufferSize(dwAsyncMemoryBufferSize)
    , m_dwExtensionBuffersSize(dwAsyncMemoryExtensionBuffersSize)
    , m_dwDrawFrameCount(0) {
    m_dwCurrentRenderDataIndex[ASYNC_RENDER_DATA_UPDATE] = 0;
    m_dwCurrentRenderDataIndex[ASYNC_RENDER_DATA_DRAW] = 1;


    for (int32 i = 0; i < 2; i++) {
        tRenderData[i].m_fCanceled = FALSE;
        tRenderData[i].m_pMemoryBuffer = new char[dwAsyncMemoryBufferSize];
        tRenderData[i].m_dwMemoryBufferOffset = 0;
        tRenderData[i].m_ptExtensionBuffers = tRenderData[i].m_ptCurrentExtensionBuffer = NULL;


        tRenderData[i].m_tRenderTime = 0;
        tRenderData[i].m_tGPUTime = 0;
    }


    const char * aszRenderThreadNames[3] = {
        "RenderThread (Main)",
        "RenderThread (Worker 1)",  // invented; only the first name is in the binary
        "RenderThread (Worker 2)",  // invented
    };


    for (int32 worker = 0; worker < 1; worker++) {

        AsyncRender::RenderWorkerData & workerData = m_tWorkerData[worker];
        workerData.ptAsyncRender = this;
        workerData.nWorkerIndex = worker;

        BOOL bSuccess = Blizzard::Thread::Create(sAsyncRenderWorkerProc, &workerData, &m_tThread[worker], aszRenderThreadNames[worker], 1, 0x40000);


















        ConfigureRenderThread(m_tThread[worker], 0);


        SigmaProfRegisterThread(m_tThread[worker], aszRenderThreadNames[worker], 0x1000);

        Blizzard::Thread::Resume(m_tThread[worker]);
    }
}




AsyncRender::~AsyncRender() {

    for (int32 worker = 0; worker < 1; worker++) {

        SigmaProfUnregisterThread(m_tThread[worker]);
    }

    Flush();


    SwapRenderData(ASYNC_RENDER_DEVICE_AVAILABLE);
    ProcessDelayedCallbacks();

    GfxReleaseThreadOwnership();

    TerminateThread();

    GfxAcquireThreadOwnership();

    GPUFlush();

    for (int32 i = 0; i < 2; i++) {

        AsyncExtensionBuffer * ptExtensionBuffer = tRenderData[i].m_ptExtensionBuffers;
        while (ptExtensionBuffer) {

            AsyncExtensionBuffer * ptExtensionBufferToDel = ptExtensionBuffer;
            ptExtensionBuffer = ptExtensionBuffer->ptNext;
            SigmaMemoryFree(ptExtensionBufferToDel, NULL);
        }
        delete[] tRenderData[i].m_pMemoryBuffer;
    }
}





inline uint32 AsyncRender::GetDrawFrameCount() {


    return m_dwDrawFrameCount;
}




inline uint32 AsyncRender::GetUpdateFrameCount() {


    return m_dwDrawFrameCount + 1;
}




inline BOOL AsyncRender::RegisterDelayedCallback(AsyncRenderCallback callback, void * pParam1, void * pParam2) {


    Blizzard::Lock::ScopedMutex tLock(m_tResourceDelayedCritical);


    RenderData & tData = tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];

    AsyncRenderCallbackItem & tItem = blzext::vector_append_element(tData.m_arDelayedCallbackItem);
    tItem.callback = callback;
    tItem.pParam1 = pParam1;
    tItem.pParam2 = pParam2;

    return TRUE;
}
















































inline void AsyncRender::RegisterGPUResourceDelayedDeletion(AsyncRenderCallback callback, void * pParam1, void * pParam2) {





    Blizzard::Lock::ScopedMutex tLock(m_tGPUResourceDelayedCritical);

    if (m_dwGPUResourceDelayedDeleteItemCount >= 2048) {

        XASSERT(FALSE);
    }

    GPUResourceDelayedDeleteItem & tItem = m_pGPUResourceDelayedDeleteItem[m_dwGPUResourceDelayedDeleteItemCount++];
    tItem.callback = callback;
    tItem.pParam1 = pParam1;
    tItem.pParam2 = pParam2;
    tItem.uiFrameCount = 4;
}




void AsyncRender::ProcessDelayedCallbacks() {



    RenderData & tData = tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];


    size_t dwCount = tData.m_arDelayedCallbackItem.size();
    for (size_t i = 0; i < dwCount; i++) {

        AsyncRenderCallbackItem & item = tData.m_arDelayedCallbackItem[i];
        item.callback(item.pParam1, item.pParam2);
    }










    tData.m_arDelayedCallbackItem.clear();

    {
        Blizzard::Lock::ScopedRMutex tLock(m_tGPUResourceDelayedCritical);

        uint32 dwNewGPUResourceDelayedDeleteItemCount = 0;
        for (uint32 i = 0; i < m_dwGPUResourceDelayedDeleteItemCount; i++) {


            GPUResourceDelayedDeleteItem & item = m_pGPUResourceDelayedDeleteItem[i];
            if (item.uiFrameCount == 0) {

                item.callback(item.pParam1, item.pParam2);
            } else {


                item.uiFrameCount--;
                if (i != dwNewGPUResourceDelayedDeleteItemCount) {

                    m_pGPUResourceDelayedDeleteItem[dwNewGPUResourceDelayedDeleteItemCount++] = item;
                } else {
                    dwNewGPUResourceDelayedDeleteItemCount++;
                }
            }
        }












        m_dwGPUResourceDelayedDeleteItemCount = dwNewGPUResourceDelayedDeleteItemCount;
    }
}
































void * AsyncRender::GetMemory(uint32 dwSize) {


    dwSize = (dwSize + 15) & ~15;

    RenderData & tData = tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)];
    void * pData;

    if (tData.m_dwMemoryBufferOffset + dwSize > m_dwMemoryBufferSize) {

        tData.m_dwMemoryBufferOffset += dwSize;


        if (!tData.m_ptCurrentExtensionBuffer || tData.m_ptCurrentExtensionBuffer->dwBufferOffset + dwSize > tData.m_ptCurrentExtensionBuffer->dwBufferSize) {


            if (tData.m_ptCurrentExtensionBuffer && tData.m_ptCurrentExtensionBuffer->ptNext) {
                tData.m_ptCurrentExtensionBuffer = tData.m_ptCurrentExtensionBuffer->ptNext;
            } else {






                uint32 dwAlignedSizeOf = sizeof(AsyncExtensionBuffer);
                AsyncExtensionBuffer * ptNewExtensionBuffer = (AsyncExtensionBuffer *)SigmaMemoryNew(m_dwExtensionBuffersSize + dwAlignedSizeOf, 4, NULL, FALSE);
                ptNewExtensionBuffer->dwBufferSize = m_dwExtensionBuffersSize;
                ptNewExtensionBuffer->dwBufferOffset = 0;
                ptNewExtensionBuffer->pData = (uint8 *)ptNewExtensionBuffer + dwAlignedSizeOf;
                ptNewExtensionBuffer->ptNext = NULL;
                if (tData.m_ptCurrentExtensionBuffer) {

                    tData.m_ptCurrentExtensionBuffer->ptNext = ptNewExtensionBuffer;
                } else {


                    tData.m_ptExtensionBuffers = ptNewExtensionBuffer;
                }

                tData.m_ptCurrentExtensionBuffer = ptNewExtensionBuffer;
            }
        }

        tData.m_ptCurrentExtensionBuffer->dwLastUpdateFrameUsed = GetUpdateFrameCount();
        void * pData = tData.m_ptCurrentExtensionBuffer->pData + tData.m_ptCurrentExtensionBuffer->dwBufferOffset;
        tData.m_ptCurrentExtensionBuffer->dwBufferOffset += dwSize;
        return pData;
    }


    pData = tData.m_pMemoryBuffer + tData.m_dwMemoryBufferOffset;
    tData.m_dwMemoryBufferOffset += dwSize;

    return pData;
}










































BOOL AsyncRender::Flush() {

    if (ThreadGlobals::Get().m_nThreadIndex != THREADINDEX_MAIN) {


        m_tRenderCompletionEvent[0].WaitForever();
        m_tRenderCompletionEvent[0].Set();
        return FALSE;
    }

    if (m_fSynched)
        return FALSE;

    {

        AppGlobals & tAppGlobals = sgtAppGlobals;
        FrameTimer::IdleScope mainIdle(tAppGlobals.mainTimer);

        m_tRenderCompletionEvent[0].WaitForever();
    }
    m_fSynched = TRUE;


    RenderData & tData = tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];

    sgtAppGlobals.renderTimerHistory.Append(tData.m_tRenderTime);
    sgtAppGlobals.gpuTimerHistory.Append(tData.m_tGPUTime);

    AsyncExtensionBuffer * ptExtensionBuffer = tData.m_ptExtensionBuffers;
    AsyncExtensionBuffer * ptPrevBuffer = NULL;
    BOOL fStartDeletion = FALSE;
    while (ptExtensionBuffer) {

        AsyncExtensionBuffer * ptToDelete = ptExtensionBuffer;
        ptExtensionBuffer = ptExtensionBuffer->ptNext;
        if (ptToDelete->dwLastUpdateFrameUsed < GetUpdateFrameCount() - 8 && !fStartDeletion) {

            if (ptPrevBuffer) {
                ptPrevBuffer->ptNext = NULL;
            } else {

                tData.m_ptExtensionBuffers = NULL;
            }

            fStartDeletion = TRUE;
        }


        if (fStartDeletion) {

            SigmaMemoryFree(ptToDelete, NULL);
        } else {


            ptToDelete->dwBufferOffset = 0;
            ptPrevBuffer = ptToDelete;
        }

    }
    tData.m_ptCurrentExtensionBuffer = tData.m_ptExtensionBuffers;

    tData.m_dwMemoryBufferOffset = 0;

    GfxAcquireThreadOwnership();

    ProcessDelayedCallbacks();

    return TRUE;
}




inline void AsyncRender::GPUFlush() {





    Blizzard::Lock::ScopedMutex tLock(m_tGPUResourceDelayedCritical);


    GfxBlockUntilIdle();


    for (uint32 i = 0; i < m_dwGPUResourceDelayedDeleteItemCount; i++) {

        GPUResourceDelayedDeleteItem & tItem = m_pGPUResourceDelayedDeleteItem[i];
        tItem.callback(tItem.pParam1, tItem.pParam2);
    }

    m_dwGPUResourceDelayedDeleteItemCount = 0;


    m_dwDrawFrameCount += 4;
}




inline void AsyncRender::CancelFrame() {



    tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_UPDATE)].m_fCanceled = TRUE;
}




inline void AsyncRender::SwapRenderData(AsyncRenderDeviceStatus eStatus) {



    uint32 eRenderDrawIndex = m_dwCurrentRenderDataIndex[ASYNC_RENDER_DATA_DRAW];
    m_dwCurrentRenderDataIndex[ASYNC_RENDER_DATA_DRAW] = m_dwCurrentRenderDataIndex[ASYNC_RENDER_DATA_UPDATE];
    m_dwCurrentRenderDataIndex[ASYNC_RENDER_DATA_UPDATE] = eRenderDrawIndex;





    ParticleGfxAsyncSwapRender(eStatus);
    SpritesGfxAsyncSwapRender(eStatus);

    m_dwDrawFrameCount++;
}




inline void AsyncRender::Kick(const BOOL bHiResScreenshotMode, const BOOL bFlushPrimitives) {





    m_eOperation = RENDEROP_NONE;
    m_bHiResScreenShotMode = bHiResScreenshotMode;
    m_bFlushPrimitives = bFlushPrimitives;

    GfxReleaseThreadOwnership();

    m_fSynched = FALSE;
    m_tRenderKickEvent[0].Set();
}










































































































void AsyncRender::AsyncRenderProc() {


    Console::IO::InitializeForThread();

    ThreadGlobals::InitializeForThread();


    SigmaProfInitializeForThread();


    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_ASYNCRENDER;


    m_tRenderCompletionEvent[0].Set();



    m_tRenderKickEvent[0].WaitForever();

    while (!m_fDone) {




        GfxAcquireThreadOwnership();

        if (m_eOperation != RENDEROP_NONE) {
            switch (m_eOperation) {
                case RENDEROP_SUSPEND:


                    GfxRenderSuspend();
                    break;
                case RENDEROP_RESUME:
                    GfxRenderResume();
                    break;
            }



            GfxReleaseThreadOwnership();
            m_tRenderCompletionEvent[0].Set();
        } else {


            sgtAppGlobals.renderTimer.Start();


            GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);

            RWindowMgr & tRWindowMgr = *GetRWindowMgr();

            RenderData & tData = tRenderData[CGameAsyncGetRenderDataIndex(ASYNC_RENDER_DATA_DRAW)];

            if (sCheckGfxReady(FALSE) == TRUE && !tData.m_fCanceled) {




                sRenderDraw(tRWindowMgr, m_bFlushPrimitives);
            } else {


                if (m_bFlushPrimitives) {

                    SpritesFlush();
                    GfxPrimitivesFlush();
                }
            }

            if (!m_bHiResScreenShotMode) {

                tRWindowMgr.DisplayListsEnd();


                SubObjectGfxOcclusionHighlightingFrameEnd();
            }


            sgtAppGlobals.renderTimer.Stop();

            tData.m_tGPUTime = gptGFXAPI->DeviceFrameTimeUs();
            tData.m_tRenderTime = sgtAppGlobals.renderTimer.Get();

            sgtAppGlobals.renderTimer.Reset();

            GfxReleaseThreadOwnership();

            tData.m_fCanceled = FALSE;
            m_tRenderCompletionEvent[0].Set();


            SigmaProfNewFrame(NULL);
        }
        m_tRenderKickEvent[0].WaitForever();
    }

    SigmaProfDisposeForThread();


    ThreadGlobals::DisposeForThread();

    Console::IO::DisposeForThread();
}





inline void AsyncRender::TerminateThread() {

    if (!m_fDone) {

        m_fDone = TRUE;



        m_tRenderKickEvent[0].Set();

        Blizzard::Thread::WaitForever(m_tThread[0]);
        Blizzard::Thread::Close(m_tThread[0]);
    }
}





void ConsoleGfxInit() {



    SubsystemsInitialize(sgtCGameGfxSubsystems, sgnNumCGameGfxSubsystems);
}

void ConsoleGfxDispose() {



    SubsystemsDispose(sgtCGameGfxSubsystems, sgnNumCGameGfxSubsystems);
}
