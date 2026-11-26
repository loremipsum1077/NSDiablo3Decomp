#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../NintendoSDK/Include/nn/os/os_EventCommon.h"
#include "../../../NintendoSDK/Include/nn/os/os_SystemEvent.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Player.h"
#include "../CGame.h"
#include "../../Common/Game.h"
#include "NX64NFP.h"
#include <new>
#include <string.h>
#include "../CPlayerUtil.h"
bool InputCSAIsActive();  // X/Client/Input/InputNX64.cpp
#include "../../Common/Networking/GameMessage.h"
// Declarations from DWARF: CGameMessage.cpp:45 and Interface/UIGame.cpp:6870.
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"  // header statics run by the static initializer (line table lists ldn_Ipv4Address.h/ldn_MacAddress.h); which header pulled it in originally is not known
namespace nn {  // nn/nn_Common.h (NN_UNEXPECTED_DEFAULT); called by OnTagActivated (line 1429).
namespace detail {
__attribute__((noreturn)) void UnexpectedDefaultImpl(const char *, const char *, int);
}
}
#include "../../Common/Definitions/LevelAreaDefinition.h"
uint32_t InputGetConnectedNpadIds(nn::hid::NpadIdType * atNpads);  // DWARF: X/Client/Input/InputNX64.cpp:2010 (no src header declares it)
int32_t InputGetControllerIndexForNpad(nn::hid::NpadIdType tNpadId);  // DWARF: X/Client/Input/InputNX64.cpp:2044
int32 ActorInventoryCountSNOActorItemsOnPlayer(const ACDID idACDContainer, const SNO snoItemActor, ACDID * ptidACDItem);  // DWARF: X/Common/ActorInventory.cpp:4186
namespace Console {
namespace NFP {


uint32 s_nNFPScanningSeconds;
uint32 s_nNFPDefaultCooldownMinutes;
uint32 s_nNFPGoblinCooldownMinutes;
uint32 s_nNFPPauseDelayFrames;
uint32 s_nNFPDefaultDemonSpawnDelay;
// DWARF NX64NFP.cpp:2753; needed by the real scan-failure callers below.
void OnNFPScanFailed(ApplicationData & tData, GameErrorData & tError);


namespace {
static const uint32 nTreasureGoblinIndex = 0;

static const int nThreadStackSize = 16384;
static char g_acThreadStack[5][nThreadStackSize] __attribute__((aligned(4096)));

static int g_InstanceCount = 0;



static ApplicationData g_tData;

struct ArgDeviceHandle {  // size 0x8
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x00
};

typedef nn::Result (*ProcessFunc)(const void *);






static const int nRetryCountMax = 5;
static const int nRetryInterval = 200;

nn::Result ProcessRetryFunction(const void * ptParam, ProcessFunc fnFunc)
{
    nn::Result tResult;
    for (int i = 0; i < nRetryCountMax; i++)
    {
        tResult = fnFunc(ptParam);
        if (!nn::nfp::ResultNeedRetry::Includes(tResult))
        {
            break;
        }
        nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(nRetryInterval));
    }
    return tResult;
}

struct ArgAttachEvents {  // size 0x18
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x00
    nn::os::SystemEventType * ptActivateEvent;  // 0x08
    nn::os::SystemEventType * ptDeactivateEvent;  // 0x10
};



nn::Result ProcessAttachEvents(const void * ptParam)
{
    const ArgAttachEvents * ptArg = static_cast<const ArgAttachEvents *>(ptParam);
    nn::Result tResult = nn::nfp::AttachActivateEvent(ptArg->ptActivateEvent, *ptArg->ptDeviceHandle);
    if (tResult.IsSuccess())
    {
        tResult = nn::nfp::AttachDeactivateEvent(ptArg->ptDeactivateEvent, *ptArg->ptDeviceHandle);
    }
    return tResult;
}




nn::Result ProcessStartDetection(const void * ptParam)
{
    const ArgDeviceHandle * ptArg = static_cast<const ArgDeviceHandle *>(ptParam);
    return nn::nfp::StartDetection(*ptArg->ptDeviceHandle);
}


nn::Result ProcessStopDetection(const void * ptParam)
{
    const ArgDeviceHandle * ptArg = static_cast<const ArgDeviceHandle *>(ptParam);
    return nn::nfp::StopDetection(*ptArg->ptDeviceHandle);
}


nn::Result ProcessMount(const void * ptParam)
{
    return ProcessRetryFunction(ptParam,
        [](const void * ptParam) -> nn::Result
        {
            const ArgDeviceHandle * ptArg = static_cast<const ArgDeviceHandle *>(ptParam);
            return nn::nfp::Mount(*ptArg->ptDeviceHandle, nn::nfp::ModelType_Amiibo);
        });
}






nn::Result ProcessUnmount(const void * ptParam)
{
    const ArgDeviceHandle * ptArg = static_cast<const ArgDeviceHandle *>(ptParam);
    return nn::nfp::Unmount(*ptArg->ptDeviceHandle);
}

struct ArgCreateApplicationArea {  // size 0x20
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x00
    nn::Bit32 nAccessId;  // 0x08
    void * ptInitialData;  // 0x10
    uint32_t nInitialDataSize;  // 0x18
};



nn::Result ProcessCreateApplicationArea(const void * ptParam)
{
    return ProcessRetryFunction(ptParam,
        [](const void * ptParam) -> nn::Result
        {
            const ArgCreateApplicationArea * ptArg = static_cast<const ArgCreateApplicationArea *>(ptParam);
            nn::nfp::ApplicationAreaCreateInfo info;
            info.accessId = ptArg->nAccessId;
            info.pInitialData = ptArg->ptInitialData;
            info.initialDataSize = ptArg->nInitialDataSize;
            return nn::nfp::CreateApplicationArea(*ptArg->ptDeviceHandle, info);
        });
}

struct ArgOpenApplicationArea {  // size 0x10
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x00
    nn::Bit32 nAccessId;  // 0x08
};








nn::Result ProcessOpenApplicationArea(const void * ptParam)
{
    const ArgOpenApplicationArea * ptArg = static_cast<const ArgOpenApplicationArea *>(ptParam);
    return nn::nfp::OpenApplicationArea(*ptArg->ptDeviceHandle, ptArg->nAccessId);
}

struct ArgGetApplicationArea {  // size 0x20
    void * atOutBuffer;  // 0x00
    size_t * pnOutSize;  // 0x08
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x10
    uint32_t nBufferSize;  // 0x18
};



nn::Result ProcessGetApplicationArea(const void * ptParam)
{
    const ArgGetApplicationArea * ptArg = static_cast<const ArgGetApplicationArea *>(ptParam);
    return nn::nfp::GetApplicationArea(ptArg->atOutBuffer, ptArg->pnOutSize, *ptArg->ptDeviceHandle, ptArg->nBufferSize);
}

struct ArgSetApplicationArea {  // size 0x18
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x00
    const void * ptData;  // 0x08
    uint32_t nDataSize;  // 0x10
};



nn::Result ProcessSetApplicationArea(const void * ptParam)
{
    const ArgSetApplicationArea * ptArg = static_cast<const ArgSetApplicationArea *>(ptParam);
    return nn::nfp::SetApplicationArea(*ptArg->ptDeviceHandle, ptArg->ptData, ptArg->nDataSize);
}


nn::Result ProcessFlush(const void * ptParam)
{
    return ProcessRetryFunction(ptParam,
        [](const void * ptParam) -> nn::Result
        {
            const ArgDeviceHandle * ptArg = static_cast<const ArgDeviceHandle *>(ptParam);
            return nn::nfp::Flush(*ptArg->ptDeviceHandle);
        });
}






nn::Result ProcessRestore(const void * ptParam)
{
    return ProcessRetryFunction(ptParam,
        [](const void * ptParam) -> nn::Result
        {
            const ArgDeviceHandle * ptArg = static_cast<const ArgDeviceHandle *>(ptParam);
            return nn::nfp::Restore(*ptArg->ptDeviceHandle);
        });
}

struct ArgGetTagInfo {  // size 0x10
    nn::nfp::TagInfo * ptOutInfo;  // 0x00
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x08
};







nn::Result ProcessGetTagInfo(const void * ptParam)
{
    const ArgGetTagInfo * ptArg = static_cast<const ArgGetTagInfo *>(ptParam);
    return nn::nfp::GetTagInfo(ptArg->ptOutInfo, *ptArg->ptDeviceHandle);
}

struct ArgGetRegisterInfo {  // size 0x10
    nn::nfp::RegisterInfo * ptOutInfo;  // 0x00
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x08
};



nn::Result ProcessGetRegisterInfo(const void * ptParam)
{
    const ArgGetRegisterInfo * ptArg = static_cast<const ArgGetRegisterInfo *>(ptParam);
    return nn::nfp::GetRegisterInfo(ptArg->ptOutInfo, *ptArg->ptDeviceHandle);
}

struct ArgGetCommonInfo {  // size 0x10
    nn::nfp::CommonInfo * ptOutInfo;  // 0x00
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x08
};



nn::Result ProcessGetCommonInfo(const void * ptParam)
{
    const ArgGetCommonInfo * ptArg = static_cast<const ArgGetCommonInfo *>(ptParam);
    return nn::nfp::GetCommonInfo(ptArg->ptOutInfo, *ptArg->ptDeviceHandle);
}

struct ArgGetModelInfo {  // size 0x10
    nn::nfp::ModelInfo * ptOutInfo;  // 0x00
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x08
};



nn::Result ProcessGetModelInfo(const void * ptParam)
{
    const ArgGetModelInfo * ptArg = static_cast<const ArgGetModelInfo *>(ptParam);
    return nn::nfp::GetModelInfo(ptArg->ptOutInfo, *ptArg->ptDeviceHandle);
}

struct ArgStartNicknameAndOwnerSettings {  // size 0x28
    nn::nfp::DeviceHandle * ptOutDeviceHandle;  // 0x00
    bool * pbOutIsRegistered;  // 0x08
    nn::nfp::RegisterInfo * ptOutRegisterInfo;  // 0x10
    const nn::nfp::AmiiboSettingsStartParam * ptStartParam;  // 0x18
    const nn::nfp::TagInfo * ptTagInfo;  // 0x20
};



nn::Result ProcessStartNicknameAndOwnerSettings(const void * ptParam)
{
    const ArgStartNicknameAndOwnerSettings * ptArg = static_cast<const ArgStartNicknameAndOwnerSettings *>(ptParam);
    return nn::nfp::StartNicknameAndOwnerSettings(ptArg->ptOutDeviceHandle, ptArg->pbOutIsRegistered, ptArg->ptOutRegisterInfo, *ptArg->ptStartParam, *ptArg->ptTagInfo);
}

struct ArgStartGameDataEraser {  // size 0x18
    nn::nfp::DeviceHandle * ptOutDeviceHandle;  // 0x00
    const nn::nfp::AmiiboSettingsStartParam * ptStartParam;  // 0x08
    const nn::nfp::TagInfo * ptTagInfo;  // 0x10
};








nn::Result ProcessStartGameDataEraser(const void * ptParam)
{
    const ArgStartGameDataEraser * ptArg = static_cast<const ArgStartGameDataEraser *>(ptParam);
    return nn::nfp::StartGameDataEraser(ptArg->ptOutDeviceHandle, *ptArg->ptStartParam, *ptArg->ptTagInfo);
}

struct ArgStartRestorer {  // size 0x18
    nn::nfp::DeviceHandle * ptOutDeviceHandle;  // 0x00
    const nn::nfp::AmiiboSettingsStartParam * ptStartParam;  // 0x08
    const nn::nfp::TagInfo * ptTagInfo;  // 0x10
};








nn::Result ProcessStartRestorer(const void * ptParam)
{
    const ArgStartRestorer * ptArg = static_cast<const ArgStartRestorer *>(ptParam);
    return nn::nfp::StartRestorer(ptArg->ptOutDeviceHandle, *ptArg->ptStartParam, *ptArg->ptTagInfo);
}

struct ArgGetDeviceState {  // size 0x10
    nn::nfp::DeviceState * ptOutDeviceState;  // 0x00
    const nn::nfp::DeviceHandle * ptDeviceHandle;  // 0x08
};








nn::Result ProcessGetDeviceState(const void * ptParam)
{
    const ArgGetDeviceState * ptArg = static_cast<const ArgGetDeviceState *>(ptParam);
    *ptArg->ptOutDeviceState = nn::nfp::GetDeviceState(*ptArg->ptDeviceHandle);
    return nn::ResultSuccess();
}

struct PlayerProcessEntry {  // size 0x10
    PlayerCommand eCommand;  // 0x00
    ProcessFunc fnFunc;  // 0x08
};



static const PlayerProcessEntry atPlayerProcessTable[PlayerCommand_CountMax] = {
    { PlayerCommand_None, NULL },
    { PlayerCommand_AttachEvents, ProcessAttachEvents },
    { PlayerCommand_StartDetection, ProcessStartDetection },
    { PlayerCommand_StopDetection, ProcessStopDetection },
    { PlayerCommand_Mount, ProcessMount },
    { PlayerCommand_Unmount, ProcessUnmount },
    { PlayerCommand_CreateApplicationArea, ProcessCreateApplicationArea },
    { PlayerCommand_OpenApplicationArea, ProcessOpenApplicationArea },
    { PlayerCommand_GetApplicationArea, ProcessGetApplicationArea },
    { PlayerCommand_SetApplicationArea, ProcessSetApplicationArea },
    { PlayerCommand_Flush, ProcessFlush },
    { PlayerCommand_Restore, ProcessRestore },
    { PlayerCommand_GetTagInfo, ProcessGetTagInfo },
    { PlayerCommand_GetRegisterInfo, ProcessGetRegisterInfo },
    { PlayerCommand_GetCommonInfo, ProcessGetCommonInfo },
    { PlayerCommand_GetModelInfo, ProcessGetModelInfo },
    { PlayerCommand_StartNicknameAndOwnerSettings, ProcessStartNicknameAndOwnerSettings },
    { PlayerCommand_StartGameDataEraser, ProcessStartGameDataEraser },
    { PlayerCommand_StartRestorer, ProcessStartRestorer },
    { PlayerCommand_GetDeviceState, ProcessGetDeviceState },
};



ProcessFunc GetPlayerProcessFunc(PlayerCommand eCommand)
{
    int nIndex = static_cast<int>(eCommand);
    return atPlayerProcessTable[nIndex].fnFunc;
}






static char g_acWorkBuffer[8112];


nn::Result ProcessInitialize(const void * pParam)
{
    nn::nfp::Initialize();
    return nn::ResultSuccess();
}



nn::Result ProcessFinalize(const void * pParam)
{
    nn::nfp::Finalize();
    return nn::ResultSuccess();
}

struct ArgListDevices {  // size 0x18
    DeviceInfo * ptOutBuffer;  // 0x00
    int * pnOutCount;  // 0x08
    int nBufferCount;  // 0x10
};




nn::Result ProcessListDevices(const void * pParam)
{
    const ArgListDevices * ptArg = static_cast<const ArgListDevices *>(pParam);
    nn::nfp::DeviceHandle atDeviceHandle[nDeviceCountMax];

    nn::Result tResult = nn::nfp::ListDevices(atDeviceHandle, ptArg->pnOutCount, ptArg->nBufferCount);
    if (tResult.IsFailure())
    {
        if (!nn::nfp::ResultNfcDeviceNotFound::Includes(tResult))
        {
            return tResult;
        }
        *ptArg->pnOutCount = 0;
    }

    for (int i = 0; i < *ptArg->pnOutCount; i++)
    {
        ptArg->ptOutBuffer[i].ptDeviceHandle = atDeviceHandle[i];
        tResult = nn::nfp::GetNpadId(&ptArg->ptOutBuffer[i].nNpadId, ptArg->ptOutBuffer[i].ptDeviceHandle);
        if (tResult.IsFailure())
        {
            return tResult;
        }
    }
    return nn::ResultSuccess();
}

struct ArgAttachAvailabilityChangeEvent {  // size 0x8
    nn::os::SystemEventType * ptAvailabilityChangeEvent;  // 0x00
};






nn::Result ProcessAttachAvailabilityChangeEvent(const void * ptParam)
{
    const ArgAttachAvailabilityChangeEvent * ptArg = static_cast<const ArgAttachAvailabilityChangeEvent *>(ptParam);
    nn::nfp::AttachAvailabilityChangeEvent(ptArg->ptAvailabilityChangeEvent);
    return nn::ResultSuccess();
}

struct ProcessEntry {  // size 0x10
    Command eCommand;  // 0x00
    ProcessFunc fnFunc;  // 0x08
};




static const ProcessEntry atProcessTable[Command_CountMax] = {
    { Command_None, NULL },
    { Command_Initialize, ProcessInitialize },
    { Command_Finalize, ProcessFinalize },
    { Command_ListDevices, ProcessListDevices },
    { Command_AttachAvailabilityChangeEvent, ProcessAttachAvailabilityChangeEvent },
};



ProcessFunc GetProcessFunc(Command eCommand)
{
    int index = static_cast<int>(eCommand);
    return atProcessTable[index].fnFunc;
}
}  // namespace






TagAccessorBase::TagAccessorBase()
    : m_bIsThreadRunnable(true), m_bIsRunning(false), m_nCommandCount(0),
      m_nExecutedCommandCount(0), m_nLastCommand(0), m_ptNotifyEvent(NULL)
{
    if (!(g_InstanceCount++ < 5))
        nn::diag::detail::AbortImpl("", "", "", 0);
    nn::os::InitializeEvent(&m_tRunEvent, false, nn::os::EventClearMode_AutoClear);
    { nn::Result tResult = nn::os::CreateThread(&m_tThread, NfpThreadFunc, this, g_acThreadStack[g_InstanceCount - 1], nThreadStackSize, 30);
      if (!tResult.IsSuccess()) nn::diag::detail::AbortImpl("", "", "", 0, &tResult, ""); }
    nn::os::SetThreadCoreMask(&m_tThread, -1, 7);
    char szThreadName[32];
    Blizzard::String::Format(szThreadName, sizeof(szThreadName), "TagAccessorThread %d", g_InstanceCount);
    szThreadName[sizeof(szThreadName) - 1] = '\0';
    nn::os::SetThreadName(&m_tThread, szThreadName);
    nn::os::StartThread(&m_tThread);
}









TagAccessorBase::~TagAccessorBase()
{
    g_InstanceCount--;
    m_bIsThreadRunnable = false;
    nn::os::SignalEvent(&m_tRunEvent);
    nn::os::WaitThread(&m_tThread);
    nn::os::DestroyThread(&m_tThread);
    nn::os::FinalizeEvent(&m_tRunEvent);
}
















nn::Result TagAccessorBase::GetLastResult() const
{
    return m_tLastResult;
}


























bool TagAccessorBase::GetCommandResult(nn::Result * ptOutResult, uint32_t nCommand) const
{



    int i;
    for (i = 0; m_anCommandList[i] != m_nLastCommand && i < m_nExecutedCommandCount; ++i)
    {
        if (m_anCommandList[i] == nCommand)
        {
            *ptOutResult = nn::ResultSuccess();
            return true;
        }
    }
    // Inference: the exit through the last command reads no further state (line table: no row before 605).
    if (m_anCommandList[i] == m_nLastCommand)
    {
        *ptOutResult = m_tLastResult;
        return true;
    }
    return false;
}




inline int TagAccessorBase::Enqueue(uint32_t nCommand, const void * ptParam, size_t nParamSize)
{
    int nIndex = m_nCommandCount++;
    m_anCommandList[nIndex] = nCommand;
    memcpy(&m_atParameterList[nIndex], ptParam, nParamSize);
    return nIndex;
}









inline void TagAccessorBase::Run(nn::os::EventType * ptEvent)
{
    m_bIsRunning = true;
    m_nExecutedCommandCount = 0;
    m_ptNotifyEvent = ptEvent;
    nn::os::ClearEvent(ptEvent);
    nn::os::SignalEvent(&m_tRunEvent);
}

















void TagAccessorBase::NfpThreadFunc(void * ptArg)
{
    TagAccessorBase * ptAccessor = static_cast<TagAccessorBase *>(ptArg);
    nn::os::WaitEvent(&ptAccessor->m_tRunEvent);
    while (ptAccessor->m_bIsThreadRunnable)
    {
        uint32_t nCommand = Command_None;
        nn::Result tResult = nn::ResultSuccess();
        int i;
        for (i = 0; i < ptAccessor->m_nCommandCount; i++)
        {
            if (ptAccessor->m_bIsCanceled || tResult.IsFailure())
            {
                break;
            }
            nCommand = ptAccessor->m_anCommandList[i];
            const ParameterBuffer & param = ptAccessor->m_atParameterList[i];
            tResult = ptAccessor->Execute(nCommand, param);
        }
        ptAccessor->m_tLastResult = tResult;
        ptAccessor->m_nLastCommand = nCommand;
        ptAccessor->m_nExecutedCommandCount = i;
        ptAccessor->m_nCommandCount = 0;
        ptAccessor->m_bIsRunning = false;
        nn::os::SignalEvent(ptAccessor->m_ptNotifyEvent);
        nn::os::WaitEvent(&ptAccessor->m_tRunEvent);
    }
}

























inline int PlayerTagAccessor::AttachEvents(nn::os::SystemEventType * ptActivateEvent, nn::os::SystemEventType * ptDeactivateEvent, const nn::nfp::DeviceHandle * ptDeviceHandle)
{
    ArgAttachEvents tArg;
    tArg.ptDeviceHandle = ptDeviceHandle;
    tArg.ptActivateEvent = ptActivateEvent;
    tArg.ptDeactivateEvent = ptDeactivateEvent;
    return Enqueue(PlayerCommand_AttachEvents, tArg);
}






inline int PlayerTagAccessor::StartDetection(
    const nn::nfp::DeviceHandle * ptDeviceHandle)
{

    return Enqueue(PlayerCommand_StartDetection, ptDeviceHandle);
}

inline int PlayerTagAccessor::StopDetection(const nn::nfp::DeviceHandle * ptDeviceHandle)
{
    ArgDeviceHandle tArg;
    tArg.ptDeviceHandle = ptDeviceHandle;
    return Enqueue(PlayerCommand_StopDetection, tArg);
}

inline int PlayerTagAccessor::Mount(
    const nn::nfp::DeviceHandle * ptDeviceHandle)
{

    return Enqueue(PlayerCommand_Mount, ptDeviceHandle);
}

inline int PlayerTagAccessor::Unmount(
    const nn::nfp::DeviceHandle * ptDeviceHandle)
{

    return Enqueue(PlayerCommand_Unmount, ptDeviceHandle);
}









































































inline int PlayerTagAccessor::GetTagInfo(
    nn::nfp::TagInfo * ptOutInfo,
    const nn::nfp::DeviceHandle * ptDeviceHandle)
{


    ArgGetTagInfo tArg;
    tArg.ptOutInfo = ptOutInfo;
    tArg.ptDeviceHandle = ptDeviceHandle;
    return Enqueue(PlayerCommand_GetTagInfo, tArg);
}













inline int PlayerTagAccessor::GetCommonInfo(
    nn::nfp::CommonInfo * ptOutInfo,
    const nn::nfp::DeviceHandle * ptDeviceHandle)
{


    ArgGetCommonInfo tArg;
    tArg.ptOutInfo = ptOutInfo;
    tArg.ptDeviceHandle = ptDeviceHandle;
    return Enqueue(PlayerCommand_GetCommonInfo, tArg);
}

inline int PlayerTagAccessor::GetModelInfo(
    nn::nfp::ModelInfo * ptOutInfo,
    const nn::nfp::DeviceHandle * ptDeviceHandle)
{


    ArgGetModelInfo tArg;
    tArg.ptOutInfo = ptOutInfo;
    tArg.ptDeviceHandle = ptDeviceHandle;
    return Enqueue(PlayerCommand_GetModelInfo, tArg);
}

































int PlayerTagAccessor::GetDeviceState(
    nn::nfp::DeviceState * ptOutDeviceState,
    const nn::nfp::DeviceHandle * ptDeviceHandle)
{

    ArgGetDeviceState tArg;
    tArg.ptOutDeviceState = ptOutDeviceState;
    tArg.ptDeviceHandle = ptDeviceHandle;

    return Enqueue(PlayerCommand_GetDeviceState, tArg);
}

bool PlayerTagAccessor::IsBusy()
{
    return TagAccessorBase::IsBusy() && !m_bIsWaitForAmiiboSetting;
}

nn::Result PlayerTagAccessor::Execute(uint32_t nCommand, const TagAccessorBase::ParameterBuffer & tParam)
{
    if (nCommand == PlayerCommand_StartNicknameAndOwnerSettings ||
        nCommand == PlayerCommand_StartGameDataEraser ||
        nCommand == PlayerCommand_StartRestorer)
    {
        m_bIsWaitForAmiiboSetting = true;
    }
    nn::Result tResult = GetPlayerProcessFunc(static_cast<PlayerCommand>(nCommand))(&tParam);
    m_bIsWaitForAmiiboSetting = false;
    return tResult;
}






inline TagAccessor & TagAccessor::CreateInstance()
{
    g_ptInstance = new (g_acWorkBuffer) TagAccessor();
    return *g_ptInstance;
}



inline TagAccessor & TagAccessor::GetInstance()
{
    return *g_ptInstance;
}

















inline PlayerTagAccessor & TagAccessor::GetPlayerTagAccessor(int nPlayerIndex)
{
    return m_atPlayerTagAccessor[nPlayerIndex];
}

inline int TagAccessor::Initialize()
{
    return Enqueue(Command_Initialize, NULL, 0);
}






inline int TagAccessor::ListDevices(DeviceInfo * ptOutBuffer, int * pnOutCount, int nBufferCount)
{





    ArgListDevices tArg;
    tArg.ptOutBuffer = ptOutBuffer;
    tArg.pnOutCount = pnOutCount;
    tArg.nBufferCount = nBufferCount;
    return Enqueue(Command_ListDevices, tArg);
}

inline int TagAccessor::AttachAvailabilityChangeEvent(nn::os::SystemEventType * ptAvailabilityChangeEvent)
{
    ArgAttachAvailabilityChangeEvent tArg;
    tArg.ptAvailabilityChangeEvent = ptAvailabilityChangeEvent;
    return Enqueue(Command_AttachAvailabilityChangeEvent, tArg);
}




bool TagAccessor::IsBusy()
{
    return TagAccessorBase::IsBusy();
}

nn::Result TagAccessor::Execute(uint32_t nCommand, const TagAccessorBase::ParameterBuffer & tParam)
{
    return GetProcessFunc(static_cast<Command>(nCommand))(&tParam);
}






namespace {
namespace {
// The low byte is not compared (IsSameCharacter), so its value here is not recovered.
const nn::Bit32 anCharacterIdList[1] = { 0x38C000 };
}
}














TagAccessor * TagAccessor::g_ptInstance = NULL;

namespace {
struct PlayerStateChain {  // size 0x18
    PlayerState ePrev;  // 0x00
    PlayerState eNext;  // 0x04
    void (*fnWriter)(int, ApplicationData &);  // 0x08
    nn::Result tResult;  // 0x10
};
typedef void (*UpdatePlayerStateFunc)(int, ApplicationData &);  // DWARF typedef (line 1050), needed by DoStartDetection
struct ApplicationAreaData {  // size 0x4
    uint32_t nCounter;  // 0x00
};

struct AmiiboSettingsData {  // size 0x3c
    bool bIsRegistered;  // 0x00
    nn::nfp::AmiiboSettingsStartParam tStartParam;  // 0x04
    nn::nfp::DeviceHandle tDeviceHandle;  // 0x34
};

struct PlayerWorkData {  // size 0x280
    PlayerStateChain tChain;  // 0x00
    nn::os::EventType tProcessFinishEvent;  // 0x18
    nn::nfp::DeviceHandle tDeviceHandle;  // 0x40
    nn::os::SystemEvent * ptActivateEvent;  // 0x48
    nn::os::SystemEvent * ptDeactivateEvent;  // 0x50
    nn::nfp::TagInfo tTagInfo;  // 0x58
    nn::nfp::ModelInfo tModelInfo;  // 0xb0
    nn::nfp::CommonInfo tCommonInfo;  // 0xf0
    nn::nfp::RegisterInfo tRegisterInfo;  // 0x130
    ApplicationAreaData tApplicationAreaData;  // 0x230
    size_t nApplicationAreaSize;  // 0x238
    AmiiboSettingsData tAmiiboSettingsData;  // 0x240
    nn::nfp::DeviceState tDeviceState;  // 0x27c
};

struct ApplicationWorkData {  // size 0xaa8
    nn::os::EventType tProcessFinishEvent;  // 0x00
    DeviceInfo atDeviceInfo[nDeviceCountMax];  // 0x28
    int nDeviceCount;  // 0x94
    int nDeviceIndex;  // 0x98
    PlayerWorkData atPlayerData[nPlayerCountMax];  // 0xa0
    nn::os::SystemEvent * ptAvailabilityChangeEvent;  // 0xaa0
};


























ApplicationWorkData g_tWork;
char g_acActivateEventBuffer[sizeof(nn::os::SystemEvent) * nPlayerCountMax] __attribute__((aligned(16)));
char g_acDeactivateEventBuffer[sizeof(nn::os::SystemEvent) * nPlayerCountMax] __attribute__((aligned(16)));






void DestroySystemEvents(int nPlayerIndex)
{
    if (g_tWork.atPlayerData[nPlayerIndex].ptActivateEvent != NULL)
    {
        g_tWork.atPlayerData[nPlayerIndex].ptActivateEvent->~SystemEvent();
        g_tWork.atPlayerData[nPlayerIndex].ptActivateEvent = NULL;
    }
    if (g_tWork.atPlayerData[nPlayerIndex].ptDeactivateEvent != NULL)
    {
        g_tWork.atPlayerData[nPlayerIndex].ptDeactivateEvent->~SystemEvent();
        g_tWork.atPlayerData[nPlayerIndex].ptDeactivateEvent = NULL;
    }
}


void CreateSystemEvent(int nPlayerIndex)
{
    DestroySystemEvents(nPlayerIndex);
    g_tWork.atPlayerData[nPlayerIndex].ptActivateEvent = new (&g_acActivateEventBuffer[sizeof(nn::os::SystemEvent) * nPlayerIndex]) nn::os::SystemEvent();
    g_tWork.atPlayerData[nPlayerIndex].ptDeactivateEvent = new (&g_acDeactivateEventBuffer[sizeof(nn::os::SystemEvent) * nPlayerIndex]) nn::os::SystemEvent();
}











uint32_t ListConnectedNpadIds(nn::hid::NpadIdType * anNpadId)
{
    uint32_t nCount = InputGetConnectedNpadIds(anNpadId);
    return nCount;
}

void NotifyServerNFPLostForAllPlayers()
{


    Player * ptPlayer = PlayerGetFirstAll();
    while (ptPlayer != NULL)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
        {

            ptACDPlayer->SetHasNFPDevice(FALSE);


            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(ptACDPlayer->id);
            MessageSendToServer(CMSG_LOST_NFP_DEVICE, &tMessage, sizeof(tMessage), ptPlayer);
        }
        ptPlayer = PlayerGetNextAll(ptPlayer);
    }

}
BOOL NotifyServerNFPActiveForController(int nPlayerControllerIndex)
{



    Player * ptPlayer = PlayerGetByControllerIndex(nPlayerControllerIndex);
    if (ptPlayer != NULL)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
        {
            if (!ptACDPlayer->GetHasNFPDevice())
            {

                ptACDPlayer->SetHasNFPDevice(TRUE);


                ANNDataMessage tMessage;
                tMessage.ann = ANNFromACD(ptACDPlayer->id);
                MessageSendToServer(CMSG_HAS_NFP_DEVICE, &tMessage, sizeof(tMessage), ptPlayer);
                return TRUE;
            }
        }
    }


    return FALSE;
}
void NotifyServerNFPLostForController(
    int nPlayerControllerIndex)
{


    Player * ptPlayer = PlayerGetByControllerIndex(nPlayerControllerIndex);
    if (ptPlayer != NULL)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL)
        {
            if (ptACDPlayer->GetHasNFPDevice())
            {

                ptACDPlayer->SetHasNFPDevice(FALSE);


                ANNDataMessage tMessage;
                tMessage.ann = ANNFromACD(ptACDPlayer->id);
                MessageSendToServer(CMSG_LOST_NFP_DEVICE, &tMessage, sizeof(tMessage), ptPlayer);
            }
        }
    }
}








void UpdateNpad(ApplicationData & tData)
{

    nn::hid::NpadIdType anConnectedNpadIds[5];
    int nConnectedNpadCount = ListConnectedNpadIds(anConnectedNpadIds);

    bool abWasConnected[nConnectedNpadCount];
    for (int i = 0; i < nConnectedNpadCount; i++)
    {
        abWasConnected[i] = false;
    }

    for (PlayerData & tPlayerData : tData.atPlayerData)
    {
        bool bIsConnected = false;
        for (int i = 0; i < nConnectedNpadCount; i++)
        {

            if (tPlayerData.bIsEnabled &&
                tPlayerData.nNpadId == anConnectedNpadIds[i])
            {


                abWasConnected[i] = true;
                bIsConnected = true;
                break;
            }
        }



        if (tPlayerData.bIsEnabled && !bIsConnected)
        {


            NotifyServerNFPLostForController(tPlayerData.nControllerIndex);

            tPlayerData.bIsEnabled = false;
            tPlayerData.bIsNfcDevice = false;

            tPlayerData.eState = PlayerState_None;
        }
    }



    for (int i = 0; i < nConnectedNpadCount; i++)
    {
        if (abWasConnected[i])
        {
            continue;
        }


        for (PlayerData & tPlayerData : tData.atPlayerData)
        {
            if (!tPlayerData.bIsEnabled)
            {
                tPlayerData.bIsEnabled = true;
                tPlayerData.bIsNfcDevice = false;
                tPlayerData.nNpadId = anConnectedNpadIds[i];


                const int32 nControllerIndex = InputGetControllerIndexForNpad(tPlayerData.nNpadId);
                tPlayerData.nControllerIndex = nControllerIndex < 0 ? 0 : nControllerIndex;






                tPlayerData.eState = PlayerState_None;
                break;
            }
        }
    }
}


nn::nfp::DeviceHandle * GetDeviceHandle(int nPlayerIndex)
{
    return &g_tWork.atPlayerData[nPlayerIndex].tDeviceHandle;
}


nn::os::SystemEvent & GetActivateEvent(int nPlayerIndex)
{
    return *g_tWork.atPlayerData[nPlayerIndex].ptActivateEvent;
}


nn::os::SystemEvent & GetDeactivateEvent(int nPlayerIndex)
{
    return *g_tWork.atPlayerData[nPlayerIndex].ptDeactivateEvent;
}






void DoStartDetection(int nPlayerIndex, ApplicationData & tData, PlayerState eNext, UpdatePlayerStateFunc fnWriter)
{



    g_tWork.atPlayerData[nPlayerIndex].tChain.ePrev = tData.atPlayerData[nPlayerIndex].eState;
    g_tWork.atPlayerData[nPlayerIndex].tChain.eNext = eNext;
    g_tWork.atPlayerData[nPlayerIndex].tChain.fnWriter = fnWriter;
    g_tWork.atPlayerData[nPlayerIndex].tChain.tResult = nn::ResultSuccess();



    GetActivateEvent(nPlayerIndex).Clear();
    GetDeactivateEvent(nPlayerIndex).Clear();

    PlayerTagAccessor & tAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);
    tAccessor.StartDetection(GetDeviceHandle(nPlayerIndex));
    tAccessor.Run(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent);
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_StartDetection;
}


void DoStartDetectionForTagRead(int nPlayerIndex, ApplicationData & tData)
{
    DoStartDetection(nPlayerIndex, tData, PlayerState_TagRead, NULL);
}









void OnTagActivated(int nPlayerIndex, ApplicationData & tData)
{



    PlayerTagAccessor & tAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);
    switch (g_tWork.atPlayerData[nPlayerIndex].tChain.eNext)
    {
    case PlayerState_TagRead:



















        tAccessor.GetTagInfo(&g_tWork.atPlayerData[nPlayerIndex].tTagInfo, GetDeviceHandle(nPlayerIndex));
        tAccessor.Mount(GetDeviceHandle(nPlayerIndex));
        tAccessor.GetModelInfo(&g_tWork.atPlayerData[nPlayerIndex].tModelInfo, GetDeviceHandle(nPlayerIndex));











        tAccessor.Unmount(GetDeviceHandle(nPlayerIndex));
        tAccessor.Run(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent);
        tData.atPlayerData[nPlayerIndex].eState = PlayerState_TagRead;
        break;
    case PlayerState_TagWrite:



        tAccessor.GetTagInfo(&g_tWork.atPlayerData[nPlayerIndex].tTagInfo, GetDeviceHandle(nPlayerIndex));
        tAccessor.Run(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent);
        tData.atPlayerData[nPlayerIndex].eState = PlayerState_TagCheck;
        break;

    default: nn::detail::UnexpectedDefaultImpl("", "", 0);
    }


}
void OnTagChecked(int nPlayerIndex, ApplicationData & tData)
{





    (*g_tWork.atPlayerData[nPlayerIndex].tChain.fnWriter)(nPlayerIndex, tData);
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_TagWrite;
}






void DoStopDetection(int nPlayerIndex, ApplicationData & tData, PlayerState eNext, nn::Result tResult)
{


    g_tWork.atPlayerData[nPlayerIndex].tChain.ePrev = tData.atPlayerData[nPlayerIndex].eState;
    g_tWork.atPlayerData[nPlayerIndex].tChain.eNext = eNext;
    g_tWork.atPlayerData[nPlayerIndex].tChain.tResult = tResult;

    PlayerTagAccessor & accessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);
    accessor.StopDetection(GetDeviceHandle(nPlayerIndex));
    accessor.Run(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent);
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_StopDetection;
}


void DoStopDetectionForNext(int nPlayerIndex, ApplicationData & tData, PlayerState eNext)
{
    DoStopDetection(nPlayerIndex, tData, eNext, nn::ResultSuccess());
}



















void WaitTagRelease(int nPlayerIndex, ApplicationData & tData, PlayerState eNext, nn::Result tResult)
{
    g_tWork.atPlayerData[nPlayerIndex].tChain.ePrev = tData.atPlayerData[nPlayerIndex].eState;
    g_tWork.atPlayerData[nPlayerIndex].tChain.eNext = eNext;
    g_tWork.atPlayerData[nPlayerIndex].tChain.tResult = tResult;
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_TagRelease;
}





void WaitTagReleaseForNext(int nPlayerIndex, ApplicationData & tData, PlayerState eNext)
{
    WaitTagRelease(nPlayerIndex, tData, eNext, nn::ResultSuccess());
}


void WaitTagReleaseForError(int nPlayerIndex, ApplicationData & tData, nn::Result tResult)
{
    WaitTagRelease(nPlayerIndex, tData, PlayerState_Error, tResult);
}




















void UpdateStateNone(ApplicationData & tData)
{
    nn::os::InitializeEvent(&g_tWork.tProcessFinishEvent, false, nn::os::EventClearMode_AutoClear);
    for (int i = 0; i < nPlayerCountMax; i++)
    {
        nn::os::InitializeEvent(&g_tWork.atPlayerData[i].tProcessFinishEvent, false, nn::os::EventClearMode_AutoClear);
    }

    TagAccessor::CreateInstance();
    TagAccessor::GetInstance().Initialize();
    g_tWork.ptAvailabilityChangeEvent = new nn::os::SystemEvent();
    TagAccessor::GetInstance().AttachAvailabilityChangeEvent(g_tWork.ptAvailabilityChangeEvent->GetBase());
    TagAccessor::GetInstance().Run(&g_tWork.tProcessFinishEvent);
    tData.eState = State_Initializing;
}




void UpdateStateInitializing(ApplicationData & tData)
{
    if (nn::os::TryWaitEvent(&g_tWork.tProcessFinishEvent))
    {
        tData.eState = TagAccessor::GetInstance().GetLastResult().IsSuccess() ? State_Initialized : State_None;
    }
}

void UpdateStateInitialized(ApplicationData & tData);
void UpdateStateDeviceListing(ApplicationData & tData);












void UpdateStateInitialized(
    ApplicationData & tData)

{
    if (tData.bAnyPlayerHasNFP)
    {
        NotifyServerNFPLostForAllPlayers();

        tData.bAnyPlayerHasNFP = FALSE;
    }


    g_tWork.nDeviceCount = 0;

    TagAccessor & tAccessor = TagAccessor::GetInstance();
    tAccessor.ListDevices(g_tWork.atDeviceInfo, &g_tWork.nDeviceCount, nDeviceCountMax);
    tAccessor.Run(&g_tWork.tProcessFinishEvent);
    tData.eState = State_DeviceListing;
}


void UpdateStateDeviceListing(
    ApplicationData & tData)

{

    if (nn::os::TryWaitEvent(&g_tWork.tProcessFinishEvent))
    {
        TagAccessor & tAccessor = TagAccessor::GetInstance();
        bool bExistResult;
        nn::Result tCommandResult;
        bExistResult = tAccessor.GetResult(&tCommandResult, Command_ListDevices);
        if (!bExistResult || tCommandResult.IsFailure())
        {
            tData.eState = State_Initialized;
            return;
        }

        UpdateNpad(tData);

        for (int i = 0; i < nPlayerCountMax; i++)
        {
            if (tData.atPlayerData[i].bIsEnabled)
            {



                bool bIsNfcDevice = false;
                nn::nfp::DeviceHandle tDeviceHandle;
                for (int j = 0; j < g_tWork.nDeviceCount; j++)
                {
                    if (tData.atPlayerData[i].nNpadId == g_tWork.atDeviceInfo[j].nNpadId)
                    {
                        tDeviceHandle = g_tWork.atDeviceInfo[j].ptDeviceHandle;
                        bIsNfcDevice = true;
                        break;
                    }
                }

                if (!bIsNfcDevice)
                {
                    if (tData.bAnyPlayerHasNFP)
                    {


                        NotifyServerNFPLostForController(tData.atPlayerData[i].nControllerIndex);
                    }

                    tData.atPlayerData[i].bIsNfcDevice = false;
                    tData.atPlayerData[i].eState = PlayerState_None;
                }
                else
                {
                    if (tData.atPlayerData[i].bIsNfcDevice && tData.atPlayerData[i].eState != PlayerState_None)
                    {
                        if (tData.atPlayerData[i].eState == PlayerState_TagSearch)
                        {
                            PlayerTagAccessor & playerAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(i);
                            playerAccessor.GetDeviceState(&g_tWork.atPlayerData[i].tDeviceState, GetDeviceHandle(i));
                            playerAccessor.Run(&g_tWork.atPlayerData[i].tProcessFinishEvent);
                            tData.atPlayerData[i].eState = PlayerState_DeviceCheck;
                        }
                    }
                    else
                    {
                        g_tWork.atPlayerData[i].tDeviceHandle = tDeviceHandle;
                        tData.atPlayerData[i].eState = PlayerState_DeviceInitialize;
                        tData.atPlayerData[i].bIsNfcDevice = true;
                    }
                }
            }
        }
        tData.eState = State_Idle;
    }
}
void UpdateStateIdle(ApplicationData & tData)
{
    if (g_tWork.ptAvailabilityChangeEvent->TryWait())
    {
        tData.eState = State_Initialized;
    }
}








void UpdatePlayerStateNone(int nPlayerIndex, ApplicationData & tData)
{
}







void UpdatePlayerStateDeviceInitialize(
    int nPlayerIndex,
    ApplicationData & tData)

{
    PlayerTagAccessor & tAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);

    CreateSystemEvent(nPlayerIndex);
    tAccessor.AttachEvents(
        GetActivateEvent(nPlayerIndex).GetBase(),
        GetDeactivateEvent(nPlayerIndex).GetBase(),
        &g_tWork.atPlayerData[nPlayerIndex].tDeviceHandle);


    tAccessor.Run(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent);
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_AttachEvent;
}

void UpdatePlayerStateAttachEvent(int nPlayerIndex, ApplicationData & tData)
{
    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        nn::Result tResult = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex).GetLastResult();
        if (tResult.IsSuccess())
        {
            tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
        }
        else
        {
            tData.atPlayerData[nPlayerIndex].eState = PlayerState_Error;
            tData.atPlayerData[nPlayerIndex].tLastResult = tResult;
        }
    }
}

void UpdatePlayerStateIdle(int nPlayerIndex, ApplicationData & tData);





void UpdatePlayerStateIdle(
    int nPlayerIndex,
    ApplicationData & tData)

{
    BOOL bMessageSent = NotifyServerNFPActiveForController(tData.atPlayerData[nPlayerIndex].nControllerIndex);
    if (bMessageSent)
    {
        tData.bAnyPlayerHasNFP = TRUE;
    }


    bool bIsNFPScanDesired = tData.atPlayerData[nPlayerIndex].bIsScanDesired;

    if (bIsNFPScanDesired)
    {
        DoStartDetectionForTagRead(nPlayerIndex, tData);
    }
}


void UpdatePlayerStateStartDetection(int nPlayerIndex, ApplicationData & tData)
{
    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        nn::Result tResult = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex).GetLastResult();
        if (tResult.IsSuccess())
        {
            tData.atPlayerData[nPlayerIndex].eState = PlayerState_TagSearch;
        }
        else
        {
            tData.atPlayerData[nPlayerIndex].eState = PlayerState_Error;
            tData.atPlayerData[nPlayerIndex].tLastResult = tResult;
        }
    }
}







void UpdatePlayerStateStopDetection(int nPlayerIndex, ApplicationData & tData)
{
    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        tData.atPlayerData[nPlayerIndex].eState = g_tWork.atPlayerData[nPlayerIndex].tChain.eNext;
        tData.atPlayerData[nPlayerIndex].tLastResult = g_tWork.atPlayerData[nPlayerIndex].tChain.tResult;
    }
}





void UpdatePlayerStateTagRelease(int nPlayerIndex, ApplicationData & tData)
{
    PlayerTagAccessor & tAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);
    tAccessor.StopDetection(GetDeviceHandle(nPlayerIndex));
    tAccessor.Run(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent);
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_StopDetection;
}



void UpdatePlayerStateTagSearch(
    int nPlayerIndex,
    ApplicationData & tData)

{
    BOOL bMessageSent = NotifyServerNFPActiveForController(tData.atPlayerData[nPlayerIndex].nControllerIndex);
    if (bMessageSent)
    {
        tData.bAnyPlayerHasNFP = TRUE;
    }


    bool bIsNFPScanDesired = tData.atPlayerData[nPlayerIndex].bIsScanDesired;









    if (GetActivateEvent(nPlayerIndex).TryWait())
    {

        OnTagActivated(nPlayerIndex, tData);
    }
    else if (!bIsNFPScanDesired)
    {

        DoStopDetectionForNext(nPlayerIndex, tData, PlayerState_Idle);
    }
}


void UpdatePlayerStateDeviceCheck(
    int nPlayerIndex,
    ApplicationData & tData)

{
    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        if (g_tWork.atPlayerData[nPlayerIndex].tDeviceState != nn::nfp::DeviceState_Search &&
            g_tWork.atPlayerData[nPlayerIndex].tDeviceState != nn::nfp::DeviceState_Active)
        {
            tData.atPlayerData[nPlayerIndex].eState = PlayerState_Error;
            if (g_tWork.atPlayerData[nPlayerIndex].tDeviceState == nn::nfp::DeviceState_Deactive)
            {
                // Inference: ErrorResultBase<115, 96> (0xc073) is in the DWARF; its nfp result class name is not.
                tData.atPlayerData[nPlayerIndex].tLastResult = nn::result::detail::ErrorResultBase<115, 96>();
            }
            else
            {
                tData.atPlayerData[nPlayerIndex].tLastResult = nn::nfp::ResultNfcDeviceNotFound();
            }
        }
        else
        {

            tData.atPlayerData[nPlayerIndex].eState = PlayerState_TagSearch;
        }
    }
}

void UpdatePlayerStateTagRead(
    int nPlayerIndex,
    ApplicationData & tData)

{

    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        PlayerTagAccessor & tAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);
        nn::Result tResult = tAccessor.GetLastResult();
        bool bIsModelUpdated = false;  // Inference: DWARF local (line 1873); its uses left no instructions and are not recovered.


        bool bExistResult;
        nn::Result tCommandResult;
        bExistResult = tAccessor.GetResult(&tCommandResult, PlayerCommand_GetTagInfo);
        if (bExistResult && tCommandResult.IsSuccess())
        {
            tData.atPlayerData[nPlayerIndex].tTagInfo = g_tWork.atPlayerData[nPlayerIndex].tTagInfo;
        }
        else
        {
            std::memset(&tData.atPlayerData[nPlayerIndex].tTagInfo, 0, sizeof(tData.atPlayerData[nPlayerIndex].tTagInfo));
        }
        bExistResult = tAccessor.GetResult(&tCommandResult, PlayerCommand_GetModelInfo);
        if (bExistResult && tCommandResult.IsSuccess())
        {
            tData.atPlayerData[nPlayerIndex].tModelInfo = g_tWork.atPlayerData[nPlayerIndex].tModelInfo;
        }
        else
        {

            std::memset(&tData.atPlayerData[nPlayerIndex].tModelInfo, 0, sizeof(tData.atPlayerData[nPlayerIndex].tModelInfo));
        }
        std::memset(&tData.atPlayerData[nPlayerIndex].tCommonInfo, 0, sizeof(tData.atPlayerData[nPlayerIndex].tCommonInfo));
        std::memset(&tData.atPlayerData[nPlayerIndex].tRegisterInfo, 0, sizeof(tData.atPlayerData[nPlayerIndex].tRegisterInfo));











        tData.atPlayerData[nPlayerIndex].nCounter = 0;




        if (tResult.IsSuccess())
        {

            WaitTagReleaseForNext(nPlayerIndex, tData, PlayerState_TagEnabled);
        }
        else if (tResult <= nn::result::detail::ErrorRange<115, 128, 129>())
        {

            // Inference: the nfp result class names of the ErrorRange instances are not in the DWARF.
            WaitTagReleaseForNext(nPlayerIndex, tData, PlayerState_NeedCreate);
        }
        else if (tResult <= nn::result::detail::ErrorRange<115, 136, 137>())
        {

            WaitTagReleaseForNext(nPlayerIndex, tData, PlayerState_NeedRestore);
        }
        else if (tResult <= nn::result::detail::ErrorRange<115, 144, 145>())
        {

            WaitTagReleaseForNext(nPlayerIndex, tData, PlayerState_NeedFormat);
        }
        else if (tResult <= nn::result::detail::ErrorRange<115, 120, 121>())
        {

            WaitTagReleaseForNext(nPlayerIndex, tData, PlayerState_NeedRegister);
        }
        else if (tResult <= nn::result::detail::ErrorRange<115, 152, 153>())
        {

            WaitTagReleaseForNext(nPlayerIndex, tData, PlayerState_NeedDelete);
        }
        else
        {

            WaitTagReleaseForError(nPlayerIndex, tData, tResult);
        }
    }
}


void UpdatePlayerStateTagCheck(
    int nPlayerIndex,
    ApplicationData & tData)

{

    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        PlayerTagAccessor & tAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);
        nn::Result tResult = tAccessor.GetLastResult();
        if (tResult.IsSuccess())
        {
            if (IsEqualId(g_tWork.atPlayerData[nPlayerIndex].tTagInfo.tagId, tData.atPlayerData[nPlayerIndex].tTagInfo.tagId))
            {
                OnTagChecked(nPlayerIndex, tData);
            }
            else
            {
                // Inference: ErrorResultBase<115, 96> (0xc073) is in the DWARF; its nfp result class name is not.
                WaitTagReleaseForError(nPlayerIndex, tData, nn::result::detail::ErrorResultBase<115, 96>());
            }
        }
        else
        {
            WaitTagReleaseForError(nPlayerIndex, tData, tResult);
        }
    }
}

void UpdatePlayerStateTagWrite(int nPlayerIndex, ApplicationData & tData)
{
    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        nn::Result tResult = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex).GetLastResult();
        if (tResult.IsSuccess())
        {
            WaitTagReleaseForNext(nPlayerIndex, tData, PlayerState_Idle);
        }
        else
        {
            WaitTagReleaseForError(nPlayerIndex, tData, tResult);
        }
    }
}







void UpdatePlayerStateTagEnabled(int nPlayerIndex, ApplicationData & tData)
{
    bool bIsScanDesired = tData.atPlayerData[nPlayerIndex].bIsScanDesired;
    if (tData.idACDScanResultPlayer != ACDID_INVALID)
    {
        nn::Bit32 nCharacterId = DecodeCharacterId(tData.atPlayerData[nPlayerIndex].tModelInfo.characterId);
        if (IsSameCharacter(nCharacterId, anCharacterIdList[nTreasureGoblinIndex]))
            OnNFPScanSucceeded(tData, NFPScanResult_TreasureGoblinFigure);
        else
            OnNFPScanSucceeded(tData, NFPScanResult_GenericFigure);
    }
    if (!bIsScanDesired)
    {
        tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
    }
}



























































void UpdatePlayerStateNeedCreate(int nPlayerIndex, ApplicationData & tData)
{
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
}































void UpdatePlayerStateNeedRestore(int nPlayerIndex, ApplicationData & tData)
{
    GameErrorData tError(GAMEERROR_UNK_195);
    OnNFPScanFailed(tData, tError);
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
}






































void UpdatePlayerStateNeedFormat(int nPlayerIndex, ApplicationData & tData)
{
    GameErrorData tError(GAMEERROR_UNK_195);
    OnNFPScanFailed(tData, tError);
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
}












void UpdatePlayerStateNeedRegister(int nPlayerIndex, ApplicationData & tData)
{
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
}


























void UpdatePlayerStateNeedDelete(int nPlayerIndex, ApplicationData & tData)
{
    tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
}

void UpdatePlayerStateAmiiboNotSupported(int nPlayerIndex, ApplicationData & tData);
void UpdatePlayerStateAmiiboSettings(int nPlayerIndex, ApplicationData & tData);
void UpdatePlayerStateError(int nPlayerIndex, ApplicationData & tData);

typedef void (*UpdateStateFunc)(ApplicationData &);

struct UpdateStateEntry {  // size 0x10
    State nState;  // 0x00
    UpdateStateFunc fnFunc;  // 0x08
};













void UpdatePlayerStateAmiiboNotSupported(
    int nPlayerIndex,
    ApplicationData & tData)

{
    GameErrorData tError(GAMEERROR_AMIIBO_INVALID);
    OnNFPScanFailed(tData, tError);


    tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
}


void UpdatePlayerStateAmiiboSettings(
    int nPlayerIndex,
    ApplicationData & tData)

{

    if (nn::os::TryWaitEvent(&g_tWork.atPlayerData[nPlayerIndex].tProcessFinishEvent))
    {
        PlayerTagAccessor & tAccessor = TagAccessor::GetInstance().GetPlayerTagAccessor(nPlayerIndex);
        nn::Result tResult = tAccessor.GetLastResult();
        if (tResult.IsSuccess())
        {
            DoStartDetectionForTagRead(nPlayerIndex, tData);
        }
        else
        {
            tData.atPlayerData[nPlayerIndex].eState = PlayerState_Error;
            tData.atPlayerData[nPlayerIndex].tLastResult = tResult;
        }
    }
}


void UpdatePlayerStateError(
    int nPlayerIndex,
    ApplicationData & tData)

{

    bool bIsNFPScanDesired = tData.atPlayerData[nPlayerIndex].bIsScanDesired;




    if (tData.atPlayerData[nPlayerIndex].tLastResult <= nn::nfp::ResultNfcDeviceNotFound())
    {
        if (bIsNFPScanDesired && tData.idACDScanResultPlayer != ACDID_INVALID)
        {
            GameErrorData tError(GAMEERROR_AMIIBO_SCAN_FAILED);
            OnNFPScanFailed(tData, tError);
        }

        tData.atPlayerData[nPlayerIndex].eState = PlayerState_None;
        if (tData.eState == State_Idle)
        {
            tData.eState = State_Initialized;
        }
    }
    else
    {
        bool bHasScanPlayer = tData.idACDScanResultPlayer != ACDID_INVALID;
        bool bReportError = !nn::result::detail::ErrorRange<115, 96, 104>::Includes(tData.atPlayerData[nPlayerIndex].tLastResult);
        if (bReportError && bHasScanPlayer && bIsNFPScanDesired)
        {
            GameErrorData tError;
            // Inference: ErrorRange<115, 184, 185> (line 2333) and <115, 176, 184> (line 2334) are in the DWARF; their nfp class names are not.
            if (nn::result::detail::ErrorRange<115, 184, 185>::Includes(tData.atPlayerData[nPlayerIndex].tLastResult) ||
                nn::result::detail::ErrorRange<115, 176, 184>::Includes(tData.atPlayerData[nPlayerIndex].tLastResult))
            {
                tError.eError = GAMEERROR_AMIIBO_INVALID;
            }
            else
            {
                tError.eError = GAMEERROR_AMIIBO_ERROR;
            }
            OnNFPScanFailed(tData, tError);
        }



        tData.atPlayerData[nPlayerIndex].eState = PlayerState_Idle;
    }
    tData.atPlayerData[nPlayerIndex].tLastResult = nn::ResultSuccess();
}









const UpdateStateEntry atUpdateStateTable[State_CountMax] = {
    { State_None, UpdateStateNone },
    { State_Initializing, UpdateStateInitializing },
    { State_Initialized, UpdateStateInitialized },
    { State_DeviceListing, UpdateStateDeviceListing },
    { State_Idle, UpdateStateIdle },
};




UpdateStateFunc GetUpdateStateFunc(State nState)
{
    int nIndex = static_cast<int>(nState);
    return atUpdateStateTable[nIndex].fnFunc;
}

typedef void (*UpdatePlayerStateFunc)(int, ApplicationData &);

struct UpdatePlayerStateEntry {  // size 0x10
    PlayerState nState;  // 0x00
    UpdatePlayerStateFunc fnFunc;  // 0x08
};





const UpdatePlayerStateEntry atUpdatePlayerStateTable[PlayerState_CountMax] = {
    { PlayerState_None, UpdatePlayerStateNone },
    { PlayerState_DeviceInitialize, UpdatePlayerStateDeviceInitialize },
    { PlayerState_AttachEvent, UpdatePlayerStateAttachEvent },
    { PlayerState_Idle, UpdatePlayerStateIdle },
    { PlayerState_StartDetection, UpdatePlayerStateStartDetection },
    { PlayerState_StopDetection, UpdatePlayerStateStopDetection },
    { PlayerState_TagRelease, UpdatePlayerStateTagRelease },
    { PlayerState_TagSearch, UpdatePlayerStateTagSearch },
    { PlayerState_DeviceCheck, UpdatePlayerStateDeviceCheck },
    { PlayerState_TagRead, UpdatePlayerStateTagRead },
    { PlayerState_TagCheck, UpdatePlayerStateTagCheck },
    { PlayerState_TagWrite, UpdatePlayerStateTagWrite },
    { PlayerState_TagEnabled, UpdatePlayerStateTagEnabled },
    { PlayerState_NeedCreate, UpdatePlayerStateNeedCreate },
    { PlayerState_NeedRestore, UpdatePlayerStateNeedRestore },
    { PlayerState_NeedFormat, UpdatePlayerStateNeedFormat },
    { PlayerState_NeedRegister, UpdatePlayerStateNeedRegister },
    { PlayerState_NeedDelete, UpdatePlayerStateNeedDelete },
    { PlayerState_AmiiboNotSupported, UpdatePlayerStateAmiiboNotSupported },
    { PlayerState_AmiiboSettings, UpdatePlayerStateAmiiboSettings },
    { PlayerState_Error, UpdatePlayerStateError },
};




UpdatePlayerStateFunc GetUpdatePlayerStateFunc(PlayerState eState)
{
    int nIndex = static_cast<int>(eState);
    return atUpdatePlayerStateTable[nIndex].fnFunc;
}
}  // namespace




















































void UpdateState(
    ApplicationData & tData)
{

    tData.nFrame++;
    State nLastState = tData.eState;

    GetUpdateStateFunc(nLastState)(tData);
    if (nLastState != tData.eState)
    {
        tData.nTransitFrame = tData.nFrame;
    }



    BOOL bIsGamePaused = GameIsPaused() || GameIsSoftPaused() || CGameLoadingScreenIsEnabled();
    if (bIsGamePaused)
    {
        tData.nLastPausedFrame = tData.nFrame;
    }
    else
    {
        bIsGamePaused = tData.nFrame - tData.nLastPausedFrame <= s_nNFPPauseDelayFrames;
    }
    BOOL bIsStartup = GameIsStartUp();
    BOOL bInputCSAIsActive = InputCSAIsActive();
    for (int i = 0; i < nPlayerCountMax; i++)
    {
        PlayerData & tPlayerData = tData.atPlayerData[i];




        BOOL bAllowUpdatesWhilePausedForInputChanges = FALSE;
        if (bIsGamePaused && !bInputCSAIsActive)
        {
            Player * ptPlayer = PlayerGetByControllerIndex(tPlayerData.nControllerIndex);
            if (ptPlayer != NULL)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer != NULL)
                {
                    bAllowUpdatesWhilePausedForInputChanges = !ptACDPlayer->GetHasNFPDevice();
                }
            }
        }







        if (!CGameLoadingScreenIsEnabled() && (bIsStartup || !bIsGamePaused || bInputCSAIsActive || bAllowUpdatesWhilePausedForInputChanges))
        {
            PlayerState nLastPlayerState = tPlayerData.eState;

            GetUpdatePlayerStateFunc(nLastPlayerState)(i, tData);
            if (nLastPlayerState != tPlayerData.eState)
            {
                tPlayerData.nTransitFrame = tData.nFrame;
            }
        }
    }


}



















































void InitializeForApp()
{
    g_tData.nLastPausedFrame = 0;
    for (PlayerData & tPlayerData : g_tData.atPlayerData)
    {
        tPlayerData.bIsScanDesired = false;
    }
}





void DisposeForApp()
{
}






void Process()
{
    UpdateState(g_tData);
}


bool IsPlayerScanDesired(ACDID idPlayerACD)
{
    Player * ptPlayer = PlayerGetByACD(idPlayerACD);
    if (ptPlayer == NULL)
    {
        return false;
    }
    for (int i = 0; i < nPlayerCountMax; i++)
    {
        if (g_tData.atPlayerData[i].nControllerIndex == ptPlayer->nControllerIndex)
        {
            return g_tData.atPlayerData[i].bIsScanDesired;
        }
    }
    return false;
}








bool IsAnyPlayerScanDesired()
{
    for (int i = 0; i < nPlayerCountMax; i++)
    {
        if (g_tData.atPlayerData[i].bIsScanDesired)
        {
            return true;
        }
    }
    return false;
}




void ClearAllPlayerScansDesired()

{



    if (g_tData.idACDScanResultPlayer != ACDID_INVALID)
    {

        GameErrorData tError(GAMEERROR_AMIIBO_SCAN_FAILED);
        OnNFPScanFailed(g_tData, tError);
    }
    // Inference: four unrolled stores recover the DWARF i/tPlayerData loop.
    for (int32 i = 0; i < nPlayerCountMax; i++)
    {
        PlayerData & tPlayerData = g_tData.atPlayerData[i];
        tPlayerData.bIsScanDesired = false;
    }
}

void SetPlayerScanDesired(
    ACDID idACDPlayer,
    bool bIsScanDesired,
    ACDNetworkName annScanningPortal)
{


    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }
    // Inference: unrolled controller comparisons select the first matching PlayerData.
    PlayerData * ptPlayerData = NULL;
    for (int32 i = 0; i < nPlayerCountMax; i++)
    {
        PlayerData & tPlayerData = g_tData.atPlayerData[i];
        if (tPlayerData.nControllerIndex == ptPlayer->nControllerIndex)
        {
            ptPlayerData = &tPlayerData;
            break;
        }
    }

    // Inference: DWARF retains bPrevIsScanDesired without a location; its named snapshot is unused.
    bool bPrevIsScanDesired = ptPlayerData->bIsScanDesired;
    if (bIsScanDesired)
    {
        g_tData.idACDScanResultPlayer = idACDPlayer;
        g_tData.annScanningPortal = annScanningPortal;
    }











    else
    {
        if (g_tData.idACDScanResultPlayer == idACDPlayer)
        {

            GameErrorData tError(GAMEERROR_AMIIBO_SCAN_FAILED);
            OnNFPScanFailed(g_tData, tError);
        }
        g_tData.idACDScanResultPlayer = ACDID_INVALID;
        g_tData.annScanningPortal = ANN_INVALID;
    }

    ptPlayerData->bIsScanDesired = bIsScanDesired;


}

Player * GetPlayerByScanningPortal(ACDID idACDScanningPortal)
{
    ACDNetworkName annScanningPortal = ANNFromACD(idACDScanningPortal);
    if (g_tData.annScanningPortal != ANN_INVALID && g_tData.annScanningPortal == annScanningPortal &&
        g_tData.idACDScanResultPlayer != ACDID_INVALID)
    {
        return PlayerGetByACD(g_tData.idACDScanResultPlayer);
    }
    return NULL;
}
}  // namespace NFP
namespace Online { Blizzard::Time::Timestamp GetNetworkTimestamp(); }  // X/Client/Console/Online.h:206
namespace GamerProfile { D3::Account::SavedDefinition & GetAccount(int32 nUserIdx); }  // X/Client/Console/GamerProfile.h:14
namespace NFP {


void OnNFPScanFailed(  // DWARF low_pc 0: standalone body is not checkable; required by genuine scan-failure callers.
    ApplicationData & tData,
    GameErrorData & tError)
{
    ACDID idACDPlayer = tData.idACDScanResultPlayer; // Inference: the following unused DWARF local snapshots the portal.
    ACDNetworkName annScanningPortal = tData.annScanningPortal;

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    { // Inference: the null-player branch exits before displaying, sending, or resetting.
        return;
    }


    UIGame::DisplayError(tError, FALSE, ptPlayer);


    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(idACDPlayer);
    MessageSendToServer(CMSG_AMIIBO_SCAN_END, &tMessage, sizeof(tMessage), ptPlayer);


    for (int i = 0; i < 4; ++i)
    {
        if (tData.atPlayerData[i].nControllerIndex == ptPlayer->nControllerIndex)
        {
            tData.atPlayerData[i].bIsScanDesired = false;
            break;
        }
    }

    tData.idACDScanResultPlayer = ACDID_INVALID;
    tData.annScanningPortal = ANN_INVALID;
}
void OnNFPScanSucceeded(
    ApplicationData & tData,
    NFPScanResult eScanResult)
{


    if (eScanResult == NFPScanResult_GenericFigure || eScanResult == NFPScanResult_TreasureGoblinFigure)
    {
        ACDID idACDPlayer = tData.idACDScanResultPlayer;
        ACDNetworkName annScanningPortal = tData.annScanningPortal;

        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (idACDPlayer == ACDID_INVALID || ptPlayer == NULL)
        {
            return;
        }


        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer == NULL)
        {
            return;
        }


        Blizzard::Time::Timestamp tTimeSinceScan;
        Blizzard::Time::Timestamp tCooldownTime;
        Blizzard::Time::Timestamp tNow = Online::GetNetworkTimestamp();
        if (eScanResult == NFPScanResult_GenericFigure)
        {
            tTimeSinceScan = tNow - ptPlayer->tNFPDefaultScanTime;
            tCooldownTime = s_nNFPDefaultCooldownMinutes * 60000000000ULL;
        }
        else
        {
            tTimeSinceScan = tNow - ptPlayer->tNFPGoblinScanTime;
            tCooldownTime = s_nNFPGoblinCooldownMinutes * 60000000000ULL;
        }

        if (tTimeSinceScan < tCooldownTime)
        {


            GameErrorData tError(GAMEERROR_AMIIBO_POWER_IN_COOLDOWN);
            tError.dwVal = (tCooldownTime - tTimeSinceScan) / 1000000000ULL;  // Inference: nanoseconds to seconds (unsigned divide in the original).
            OnNFPScanFailed(tData, tError);
            return;
        }


        ACDID idPortalDeviceACD = ACDID_INVALID;
        ActorInventoryCountSNOActorItemsOnPlayer(idACDPlayer, SNOFileLookupGUID(ACTOR_SNO, "Amiibo_Portal_Device"), &idPortalDeviceACD);

        if (idPortalDeviceACD != ACDID_INVALID)
        {
            TargetMessage tMessage;
            tMessage.eType = TT_ACTOR;

            ActorCommonData * ptACDScanningPortal = ACDTryToGet(ACDFromANN(annScanningPortal));


            ActorCommonData * ptACDTarget = (ptACDScanningPortal != NULL) ? ptACDScanningPortal : ptACDPlayer;
            tMessage.tPlace = ptACDTarget->GetPlace();
            tMessage.annTarget = annScanningPortal;
            tMessage.annItemBeingUsed = ANNFromACD(idPortalDeviceACD);
            tMessage.nComboLevel = 0;  // Inference: the original stores 0 at nComboLevel together with annItemBeingUsed (stp at line 2843).


            if (eScanResult == NFPScanResult_TreasureGoblinFigure)
            {
                tMessage.snoPower = SNOFileLookupGUID(POWER_SNO, "AmiiboPortal_TreasureGoblin");
            }
            else
            {



                BOOL bIsInTown = FALSE;

                SNO snoLevelArea = ACDGetLevelArea(*ptACDTarget);
                if (snoLevelArea != SNO_NONE)
                {
                    bIsInTown = LevelAreaIsTown(snoLevelArea);
                }





                if (bIsInTown)
                {


                    GameErrorData tError(GAMEERROR_POWER_IN_TOWN);
                    OnNFPScanFailed(tData, tError);
                    return;
                }


                tMessage.snoPower = SNOFileLookupGUID(POWER_SNO, "AmiiboPortal_Default");
            }





            D3::Account::SavedDefinition & tSavedDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
            D3::Account::ConsoleData * tConsoleData = tSavedDefinition.mutable_console_data();
            if (eScanResult == NFPScanResult_GenericFigure)
            {
                ptPlayer->tNFPDefaultScanTime = tNow;
                tConsoleData->set_nfp_default_time(Blizzard::Time::ToUnixTime(tNow));
            }
            else
            {

                ptPlayer->tNFPGoblinScanTime = tNow;
                tConsoleData->set_nfp_goblin_time(Blizzard::Time::ToUnixTime(tNow));
            }


            Target tTarget;
            tTarget.InitializeFromPlace(tMessage.tPlace);
            CPlayerUtilSpawnPowerClickEffect(tMessage.snoPower, tTarget);
            MessageSendToServer(CMSG_TARGET, &tMessage, sizeof(tMessage), ptPlayer);
        }
    }









    tData.idACDScanResultPlayer = ACDID_INVALID;
    tData.annScanningPortal = ANN_INVALID;

}

}  // namespace NFP
}  // namespace Console
