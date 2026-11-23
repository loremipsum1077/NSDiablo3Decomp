#include "Shell.h"
#include "../../Client/Gfx/GfxCommon.h"
#include "../../Sigma/DataStructures/XBitArray.h"
#include "../../../NintendoSDK/Include/nn/oe/oe_NotificationMessage.h"
#include "../../../NintendoSDK/Include/nn/oe/oe_OperationModeApis.h"
#include "../../../NintendoSDK/Include/nn/oe/oe_Performance-spec.nx.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "CmdLine.h"






const CmdLineParams * CmdLineGetParams();
// X/Client/Gfx/Gfx.cpp
BOOL GfxInit();
void GfxDispose();
void GfxValidateDisplayMode(const DisplayMode & tMode, DisplayMode & tModeValidated);
BOOL GfxWindowOpen(const DisplayMode & tMode);
void GfxWindowChangeDisplayMode(const DisplayMode & tMode);
DisplayMode GfxGetDesiredDisplayMode();
// X/Client/CGame.cpp
void ConsoleGfxInit();
void ConsoleGfxDispose();
BOOL CGameAsyncRenderFlush();
void CGameAsyncRenderGPUFlush();
// X/Client/UserInterface/UIUtils.cpp
void SetUICConversionConstansts();
// X/Client/UserInterface/UI.cpp
void UI_GfxInit();
// X/Client/Sound/Sound.cpp
namespace SoundManager {
BOOL Initialize();
void Dispose();
}  // namespace SoundManager
// X/Client/Definitions/PreferencesDefinition.cpp
namespace Prefs {
void ResetToBlizzConDefaults();
}  // namespace Prefs
// X/Common/Game.cpp
void GameDisposeGames();
BOOL GameIsStartUp();
// X/Common/Player.h
struct Player;
enum PlayerSaveReason {
    PlayerSaveReason_Console = 64,
};
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
// X/Client/CHero.cpp
namespace CHero {
void Save(int32 nControllerIndex, uint32 uReason);
}  // namespace CHero
// X/Client/Input/InputNX64.cpp
void InputOnProgramResume();
// X/Client/Console/DLCManager.cpp, X/Client/Console/NX64NFP.cpp
namespace Console {
namespace DLCManager {
void Process();
}  // namespace DLCManager
namespace NFP {
void Process();
}  // namespace NFP
}  // namespace Console

AppState geAppState;
BOOL gbPausedForOS;
BOOL gbRenderTargetsCreated;
nn::oe::FocusState geFocusState;
nn::oe::OperationMode geOperationMode;
nn::oe::PerformanceMode gePerformanceMode;
BOOL gbSaveOnQuit;

static void sUpdateFocusState();
static void sOnResume();
static void sUpdateOperationMode();
static void sUpdatePerformanceMode(bool bUpdateDisplayMode);
static void sSaveHeros();
BOOL ShellPlatformInitialize(HWND hWndParent, const DisplayMode & tDisplayMode);
void ShellPlatformDispose(BOOL bDisposeGame);

BOOL ShellInitialize(HINSTANCE hInstance, HWND hWndParent)
{
    if (SHELL_APP_STATE_GET() != APPSTATE_FINISHED)
    {
        SHELL_APP_STATE_SET(APPSTATE_STARTING);
    }

    nn::oe::SetFocusHandlingMode(nn::oe::FocusHandlingMode_Notify);
    nn::oe::SetPerformanceConfiguration(nn::oe::PerformanceMode_Normal, nn::oe::PerformanceConfiguration_Cpu1020MhzGpu384MhzEmc1331Mhz);
    nn::oe::SetPerformanceConfiguration(nn::oe::PerformanceMode_Boost, nn::oe::PerformanceConfiguration_Cpu1020MhzGpu768MhzEmc1600Mhz);
    nn::oe::SetResumeNotificationEnabled(true);
    nn::oe::SetOperationModeChangedNotificationEnabled(true);
    nn::oe::SetPerformanceModeChangedNotificationEnabled(true);

    sUpdateFocusState();
    sUpdateOperationMode();
    sUpdatePerformanceMode(false);

    if (!GfxInit())
        return FALSE;

    DisplayMode tDisplayMode = GfxGetDesiredDisplayMode();

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        Prefs::ResetToBlizzConDefaults();
    }

    return ShellPlatformInitialize(hWndParent, tDisplayMode);
}

BOOL ShellPlatformInitialize(HWND hWndParent, const DisplayMode & tDisplayMode)
{
    DisplayMode tModeValidated;
    GfxValidateDisplayMode(tDisplayMode, tModeValidated);

    if (!GfxWindowOpen(tModeValidated))
        return FALSE;

    ConsoleGfxInit();
    SetUICConversionConstansts();
    SoundManager::Initialize();

    gbRenderTargetsCreated = TRUE;
    GfxWindowChangeDisplayMode(GfxGetDesiredDisplayMode());

    return TRUE;
}

void ShellDispose()
{
    ShellPlatformDispose(TRUE);
}

void ShellPlatformDispose(BOOL bDisposeGame)
{
    if (bDisposeGame)
    {
        GameDisposeGames();
    }

    ConsoleGfxDispose();
    SoundManager::Dispose();
    GfxDispose();
}

BOOL ShellWindowChangeDisplayMode(const DisplayMode & tMode)
{
    return FALSE;
}

void ShellInvalidateWindow()
{
}

BOOL ShellIsWindowMinimized()
{
    return gbPausedForOS;
}

void ShellEventLoop()
{
    nn::oe::Message message;
    while (nn::oe::TryPopNotificationMessage(&message))
    {
        switch (message)
        {
        case nn::oe::MessageFocusStateChanged:
            sUpdateFocusState();
            break;

        case nn::oe::MessageResume:
            sOnResume();
            break;

        case nn::oe::MessageOperationModeChanged:
            sUpdateOperationMode();
            break;

        case nn::oe::MessagePerformanceModeChanged:
            sUpdatePerformanceMode(true);
            break;

        case nn::oe::MessageExitRequest:
            if (gbSaveOnQuit)
            {
                if (!GameIsStartUp())
                    sSaveHeros();
                ShellEnableSaveOnQuit(FALSE);
            }
            break;
        }
    }

    Console::DLCManager::Process();
    Console::NFP::Process();
}

BOOL ShellAppCanInitialize()
{
    return TRUE;
}

void ShellEnableSaveOnQuit(BOOL bEnabled)
{
    if (gbSaveOnQuit == bEnabled)
        return;

    if (bEnabled)
        nn::oe::EnterExitRequestHandlingSection();
    else
        nn::oe::LeaveExitRequestHandlingSection();

    gbSaveOnQuit = bEnabled;
}

static void sUpdateFocusState()
{
    geFocusState = nn::oe::GetCurrentFocusState();
    gbPausedForOS = geFocusState != nn::oe::FocusState_InFocus;
}

static void sOnResume()
{
    InputOnProgramResume();
}

static void sUpdateOperationMode()
{
    geOperationMode = nn::oe::GetOperationMode();
}

static void sUpdatePerformanceMode(bool bUpdateDisplayMode)
{
    nn::oe::PerformanceMode priorPerformanceMode = gePerformanceMode;
    gePerformanceMode = nn::oe::GetPerformanceMode();

    if (bUpdateDisplayMode && priorPerformanceMode != gePerformanceMode)
    {
        CGameAsyncRenderFlush();
        CGameAsyncRenderGPUFlush();
        GfxWindowChangeDisplayMode(GfxGetDesiredDisplayMode());
        SetUICConversionConstansts();
        UI_GfxInit();
    }
}

static void sSaveHeros()
{
    for (int32 i = 0; i < MAX_LOCAL_PLAYERS; i++)
    {
        if (PlayerGetByControllerIndex(i))
            CHero::Save(i, PlayerSaveReason_Console);
    }
}

BOOL ShellRenderTargetsCreated()
{
    return gbRenderTargetsCreated;
}

BOOL ShellIsInBoostMode()
{
    return gePerformanceMode == nn::oe::PerformanceMode_Boost;
}

BOOL ShellShouldSelectFullDisplayMode()
{
    return !ShellRenderTargetsCreated() || ShellIsInBoostMode();
}
