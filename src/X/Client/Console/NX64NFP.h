// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/NX64NFP.h
// The nn::nfp and nn::hid SDK headers are not in src/ yet; the types below are copied from the
// DWARF of nn/nfp/nfp_Types.h and nn/hid/hid_NpadCommonTypes.h.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../../../NintendoSDK/Include/nn/nn_BitTypes.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "../../../NintendoSDK/Include/nn/result/result_ErrorResult.h"
#include "../../../NintendoSDK/Include/nn/os/os_EventTypes.h"
#include "../../../NintendoSDK/Include/nn/os/os_SystemEventTypes.h"
#include "../../../NintendoSDK/Include/nn/os/os_ThreadTypes.h"
#include "../../../NintendoSDK/Include/nn/os/os_EventCommon.h"
#include "../../Common/CommonTypes.h"

// nn/hid/hid_NpadCommonTypes.h
namespace nn { namespace hid {
typedef uint32_t NpadIdType;
} }

// nn/nfp/nfp_Types.h
namespace nn { namespace nfp {
enum DeviceState {
    DeviceState_Init = 0,
    DeviceState_Search = 1,
    DeviceState_Active = 2,
    DeviceState_Deactive = 3,
    DeviceState_Mount = 4,
    DeviceState_Unexpected = 5,
};
enum ModelType {
    ModelType_Amiibo = 0,
};
struct DeviceHandle {  // size 0x8
    nn::Bit32 _id;  // 0x00
    char _reserved[4];  // 0x04
};
struct TagId {  // size 0x20
    char uid[10];  // 0x00
    uint8_t length;  // 0x0a
    char _reserved[21];  // 0x0b
};
struct TagInfo {  // size 0x58
    nn::nfp::TagId tagId;  // 0x00
    nn::Bit32 protocol;  // 0x20
    nn::Bit32 type;  // 0x24
    char _reserved[48];  // 0x28
};
struct Date {  // size 0x4
    int16_t year;  // 0x00
    int8_t month;  // 0x02
    int8_t day;  // 0x03
};
struct ApplicationAreaCreateInfo {  // size 0x10
    nn::Bit32 accessId;  // 0x00
    uint32_t initialDataSize;  // 0x04
    void * pInitialData;  // 0x08
};
struct CommonInfo {  // size 0x40
    nn::nfp::Date lastWriteDate;  // 0x00
    uint16_t writeCounter;  // 0x04
    uint8_t nfpVersion;  // 0x06
    char _reserved1[1];  // 0x07
    uint32_t applicationAreaSize;  // 0x08
    char _reserved2[52];  // 0x0c
};
struct RegisterInfo {  // size 0x100
    char miiData[0x58];  // 0x00 (nn::mii::CharInfo)
    nn::nfp::Date registerDate;  // 0x58
    char nickname[41];  // 0x5c
    nn::Bit8 fontRegion;  // 0x85
    char _reserved[122];  // 0x86
};
struct ModelInfo {  // size 0x40
    char characterId[3];  // 0x00
    nn::Bit8 seriesId;  // 0x03
    uint16_t numberingId;  // 0x04
    nn::Bit8 nfpType;  // 0x06
    char _reserved[57];  // 0x07
};
struct AmiiboSettingsStartParam {  // size 0x30
    nn::nfp::DeviceHandle deviceHandle;  // 0x00
    char _debug[32];  // 0x08
    nn::Bit8 optionFlags;  // 0x28
    char _reserved[7];  // 0x29
};
static_assert(sizeof(TagInfo) == 0x58, "TagInfo");
static_assert(sizeof(RegisterInfo) == 0x100, "RegisterInfo");
static_assert(sizeof(AmiiboSettingsStartParam) == 0x30, "AmiiboSettingsStartParam");
} }

// The SDK calls NX64NFP.cpp makes, declared here; their headers are not in src/ yet.
namespace nn {
class TimeSpan {  // nn/nn_TimeSpan.h
public:
    static TimeSpan FromMilliSeconds(int64_t nMilliSeconds) { TimeSpan t; t.m_nNanoSeconds = nMilliSeconds * 1000 * 1000; return t; }
    int64_t m_nNanoSeconds;
};
namespace diag { namespace detail {  // nn/diag/detail/diag_DetailAssert.h
__attribute__((noreturn)) void AbortImpl(const char *, const char *, const char *, int);
__attribute__((noreturn)) void AbortImpl(const char *, const char *, const char *, int, const nn::Result *, const char *, ...);
} }
namespace os {  // nn/os/os_Event.h, nn/os/os_Thread.h, nn/os/os_SystemEvent.h
typedef void (*ThreadFunction)(void *);
void InitializeEvent(EventType *, bool, EventClearMode);
void FinalizeEvent(EventType *);
void SignalEvent(EventType *);
void ClearEvent(EventType *);
void WaitEvent(EventType *);
bool TryWaitEvent(EventType *);
nn::Result CreateThread(ThreadType *, ThreadFunction, void *, void *, size_t, int);
void SetThreadCoreMask(ThreadType *, int, nn::Bit64);
void SetThreadName(ThreadType *, const char *);
void StartThread(ThreadType *);
void WaitThread(ThreadType *);
void DestroyThread(ThreadType *);
void SleepThread(nn::TimeSpan);
}
namespace nfp {  // nn/nfp/nfp_Api.h, nn/nfp/nfp_Result.h
void Initialize();
void Finalize();
nn::Result ListDevices(DeviceHandle *, int *, int);
nn::Result GetNpadId(nn::hid::NpadIdType *, const DeviceHandle &);
void AttachAvailabilityChangeEvent(nn::os::SystemEventType *);
nn::Result AttachActivateEvent(nn::os::SystemEventType *, const DeviceHandle &);
nn::Result AttachDeactivateEvent(nn::os::SystemEventType *, const DeviceHandle &);
nn::Result StartDetection(const DeviceHandle &);
nn::Result StopDetection(const DeviceHandle &);
nn::Result Mount(const DeviceHandle &, ModelType);
nn::Result Unmount(const DeviceHandle &);
nn::Result CreateApplicationArea(const DeviceHandle &, const ApplicationAreaCreateInfo &);
nn::Result OpenApplicationArea(const DeviceHandle &, nn::Bit32);
nn::Result GetApplicationArea(void *, size_t *, const DeviceHandle &, size_t);
nn::Result SetApplicationArea(const DeviceHandle &, const void *, size_t);
nn::Result Flush(const DeviceHandle &);
nn::Result Restore(const DeviceHandle &);
nn::Result GetTagInfo(TagInfo *, const DeviceHandle &);
nn::Result GetRegisterInfo(RegisterInfo *, const DeviceHandle &);
nn::Result GetCommonInfo(CommonInfo *, const DeviceHandle &);
nn::Result GetModelInfo(ModelInfo *, const DeviceHandle &);
nn::Result StartNicknameAndOwnerSettings(DeviceHandle *, bool *, RegisterInfo *, const AmiiboSettingsStartParam &, const TagInfo &);
nn::Result StartGameDataEraser(DeviceHandle *, const AmiiboSettingsStartParam &, const TagInfo &);
nn::Result StartRestorer(DeviceHandle *, const AmiiboSettingsStartParam &, const TagInfo &);
DeviceState GetDeviceState(const DeviceHandle &);
// nn/nfp/nfp_Result.h. The DWARF has ErrorResultBase<115, 64> + ErrorRange<115, 64, 80> and
// ErrorRange<115, 88, 96>; the derived classes themselves are not in the DWARF.
class ResultNfcDeviceNotFound : public nn::result::detail::ErrorResultBase<115, 64>, public nn::result::detail::ErrorRange<115, 64, 80> {
};
class ResultNeedRetry : public nn::result::detail::ErrorResultBase<115, 88>, public nn::result::detail::ErrorRange<115, 88, 96> {
};
}
}

namespace Console {
namespace NFP {

static const int nDeviceCountMax = 9;  // line 28
static const int nPlayerCountMax = 4;  // line 33

typedef char CharacterId[3];

enum State {
    State_None = 0,
    State_Initializing = 1,
    State_Initialized = 2,
    State_DeviceListing = 3,
    State_Idle = 4,
    State_CountMax = 5,
};

enum PlayerState {
    PlayerState_None = 0,
    PlayerState_DeviceInitialize = 1,
    PlayerState_AttachEvent = 2,
    PlayerState_Idle = 3,
    PlayerState_StartDetection = 4,
    PlayerState_StopDetection = 5,
    PlayerState_TagRelease = 6,
    PlayerState_TagSearch = 7,
    PlayerState_DeviceCheck = 8,
    PlayerState_TagRead = 9,
    PlayerState_TagCheck = 10,
    PlayerState_TagWrite = 11,
    PlayerState_TagEnabled = 12,
    PlayerState_NeedCreate = 13,
    PlayerState_NeedRestore = 14,
    PlayerState_NeedFormat = 15,
    PlayerState_NeedRegister = 16,
    PlayerState_NeedDelete = 17,
    PlayerState_AmiiboNotSupported = 18,
    PlayerState_AmiiboSettings = 19,
    PlayerState_Error = 20,
    PlayerState_CountMax = 21,
};

enum NFPScanResult {
    NFPScanResult_GenericFigure = 0,
    NFPScanResult_TreasureGoblinFigure = 1,
};

struct DeviceInfo {  // size 0xc
    nn::nfp::DeviceHandle ptDeviceHandle;  // 0x00
    nn::hid::NpadIdType nNpadId;  // 0x08
};
static_assert(sizeof(DeviceInfo) == 0xc, "DeviceInfo");

struct PlayerData {  // size 0x1f8
    bool bIsScanDesired;  // 0x00
    bool bIsEnabled;  // 0x01
    bool bIsNfcDevice;  // 0x02
    PlayerState eState;  // 0x04
    nn::Result tLastResult;  // 0x08
    uint32_t nTransitFrame;  // 0x0c
    nn::hid::NpadIdType nNpadId;  // 0x10
    nn::Bit8 nLedPattern;  // 0x14
    nn::nfp::TagInfo tTagInfo;  // 0x18
    nn::nfp::ModelInfo tModelInfo;  // 0x70
    nn::nfp::CommonInfo tCommonInfo;  // 0xb0
    nn::nfp::RegisterInfo tRegisterInfo;  // 0xf0
    uint32_t nCounter;  // 0x1f0
    uint32_t nControllerIndex;  // 0x1f4
};
static_assert(sizeof(PlayerData) == 0x1f8, "PlayerData");

struct ApplicationData {  // size 0x800
    State eState;  // 0x00
    nn::Result tLastResult;  // 0x04
    uint32_t nFrame;  // 0x08
    uint32_t nTransitFrame;  // 0x0c
    uint32_t nLastPausedFrame;  // 0x10
    PlayerData atPlayerData[4];  // 0x14
    ACDID idACDScanResultPlayer;  // 0x7f4
    ACDNetworkName annScanningPortal;  // 0x7f8
    BOOL bAnyPlayerHasNFP;  // 0x7fc
};
static_assert(sizeof(ApplicationData) == 0x800, "ApplicationData");

class TagAccessorBase {  // size 0x650
    TagAccessorBase(const TagAccessorBase &);
    TagAccessorBase & operator=(const TagAccessorBase &);
public:
    static const int nCommandCountMax = 16;
    static const uint32_t Command_None = 0;
    static const int nParameterElementCountMax = 8;
    struct ParameterBuffer {  // size 0x40
        uintptr_t anReserved[8];  // 0x00
    };
    int GetEnqueuedCount() const;
    int GetExecutedCount() const;
    nn::Result GetLastResult() const;
    bool GetNthResult(nn::Result *, int) const;
    bool GetCommandResult(nn::Result * ptOutResult, uint32_t nCommand) const;
    void Run(nn::os::EventType * ptEvent);
    void Cancel();
    // line 244
    virtual bool IsBusy()
    {
        return m_bIsRunning;
    }
protected:
    TagAccessorBase();
    virtual ~TagAccessorBase();
    // line 265
    template <typename T> int Enqueue(uint32_t nCommand, const T & tParam)
    {
        return Enqueue(nCommand, &tParam, sizeof(T));
    }
    int Enqueue(uint32_t nCommand, const void * ptParam, size_t nParamSize);
    virtual nn::Result Execute(uint32_t nCommand, const ParameterBuffer & tParam) = 0;
    static void NfpThreadFunc(void * ptArg);
private:
    volatile bool m_bIsThreadRunnable;  // 0x08
    volatile bool m_bIsRunning;  // 0x09
    volatile bool m_bIsCanceled;  // 0x0a
    int m_nCommandCount;  // 0x0c
    uint32_t m_anCommandList[16];  // 0x10
    ParameterBuffer m_atParameterList[16];  // 0x50
    int m_nExecutedCommandCount;  // 0x450
    uint32_t m_nLastCommand;  // 0x454
    nn::Result m_tLastResult;  // 0x458
    nn::os::EventType * m_ptNotifyEvent;  // 0x460
    nn::os::ThreadType m_tThread;  // 0x468
    nn::os::EventType m_tRunEvent;  // 0x628
};
static_assert(sizeof(TagAccessorBase) == 0x650, "TagAccessorBase");

enum PlayerCommand {
    PlayerCommand_None = 0,
    PlayerCommand_AttachEvents = 1,
    PlayerCommand_StartDetection = 2,
    PlayerCommand_StopDetection = 3,
    PlayerCommand_Mount = 4,
    PlayerCommand_Unmount = 5,
    PlayerCommand_CreateApplicationArea = 6,
    PlayerCommand_OpenApplicationArea = 7,
    PlayerCommand_GetApplicationArea = 8,
    PlayerCommand_SetApplicationArea = 9,
    PlayerCommand_Flush = 10,
    PlayerCommand_Restore = 11,
    PlayerCommand_GetTagInfo = 12,
    PlayerCommand_GetRegisterInfo = 13,
    PlayerCommand_GetCommonInfo = 14,
    PlayerCommand_GetModelInfo = 15,
    PlayerCommand_StartNicknameAndOwnerSettings = 16,
    PlayerCommand_StartGameDataEraser = 17,
    PlayerCommand_StartRestorer = 18,
    PlayerCommand_GetDeviceState = 19,
    PlayerCommand_CountMax = 20,
};

class PlayerTagAccessor : public TagAccessorBase {  // size 0x658
    PlayerTagAccessor(const PlayerTagAccessor &);
    PlayerTagAccessor & operator=(const PlayerTagAccessor &);
public:
    int AttachEvents(nn::os::SystemEventType * ptActivateEvent, nn::os::SystemEventType * ptDeactivateEvent, const nn::nfp::DeviceHandle * ptDeviceHandle);
    int StartDetection(const nn::nfp::DeviceHandle * ptDeviceHandle);
    int StopDetection(const nn::nfp::DeviceHandle * ptDeviceHandle);
    int Mount(const nn::nfp::DeviceHandle * ptDeviceHandle);
    int Unmount(const nn::nfp::DeviceHandle * ptDeviceHandle);
    int GetTagInfo(nn::nfp::TagInfo * ptOutInfo, const nn::nfp::DeviceHandle * ptDeviceHandle);
    int GetCommonInfo(nn::nfp::CommonInfo * ptOutInfo, const nn::nfp::DeviceHandle * ptDeviceHandle);
    int GetModelInfo(nn::nfp::ModelInfo * ptOutInfo, const nn::nfp::DeviceHandle * ptDeviceHandle);
    int GetDeviceState(nn::nfp::DeviceState * ptOutDeviceState, const nn::nfp::DeviceHandle * ptDeviceHandle);
    // line 608; in-class body (no out-of-line DIE), inlines GetCommandResult at line 610.
    bool GetResult(nn::Result * ptOutResult, PlayerCommand eCommand) const
    {
        return GetCommandResult(ptOutResult, eCommand);
    }
    virtual bool IsBusy();
    // line 622
    PlayerTagAccessor() : m_bIsWaitForAmiiboSetting(false)
    {
    }
    // line 627
    virtual ~PlayerTagAccessor()
    {
    }
protected:
    virtual nn::Result Execute(uint32_t nCommand, const TagAccessorBase::ParameterBuffer & tParam);
private:
    bool m_bIsWaitForAmiiboSetting;  // 0x650
};
static_assert(sizeof(PlayerTagAccessor) == 0x658, "PlayerTagAccessor");

enum Command {
    Command_None = 0,
    Command_Initialize = 1,
    Command_Finalize = 2,
    Command_ListDevices = 3,
    Command_AttachAvailabilityChangeEvent = 4,
    Command_CountMax = 5,
};

class TagAccessor : public TagAccessorBase {  // size 0x1fb0
    TagAccessor(const TagAccessor &);
    TagAccessor & operator=(const TagAccessor &);
public:
    static TagAccessor & CreateInstance();
    static TagAccessor & GetInstance();
    static void DestroyInstance();
    static bool HasInstance();
    int Initialize();
    int Finalize();
    int ListDevices(DeviceInfo * ptOutBuffer, int * pnOutCount, int nBufferCount);
    int AttachAvailabilityChangeEvent(nn::os::SystemEventType * ptAvailabilityChangeEvent);
    // In-class body (no out-of-line DIE); inferred like PlayerTagAccessor::GetResult.
    bool GetResult(nn::Result * ptOutResult, Command eCommand) const
    {
        return GetCommandResult(ptOutResult, eCommand);
    }
    virtual bool IsBusy();
    PlayerTagAccessor & GetPlayerTagAccessor(int nPlayerIndex);
protected:
    virtual nn::Result Execute(uint32_t nCommand, const TagAccessorBase::ParameterBuffer & tParam);
private:
    // line 802
    TagAccessor()
    {
    }
    // line 807
    virtual ~TagAccessor()
    {
    }
    static TagAccessor * g_ptInstance;
    PlayerTagAccessor m_atPlayerTagAccessor[4];  // 0x650
};
static_assert(sizeof(TagAccessor) == 0x1fb0, "TagAccessor");

void UpdateState(ApplicationData & tData);
void OnNFPScanSucceeded(ApplicationData & tData, NFPScanResult eScanResult);

}  // namespace NFP
}  // namespace Console

namespace Console {
namespace NFP {
// line 943
inline nn::Bit32 DecodeCharacterId(const CharacterId & acCharacterId)
{
    return (static_cast<nn::Bit32>(static_cast<uint8_t>(acCharacterId[0])) << 16) |
           (static_cast<nn::Bit32>(static_cast<uint8_t>(acCharacterId[1])) << 8) |
           static_cast<nn::Bit32>(static_cast<uint8_t>(acCharacterId[2]));
}

// line 956: the variation (low byte) is ignored.
inline bool IsSameCharacter(nn::Bit32 nId1, nn::Bit32 nId2)
{
    return (nId1 & 0xFFFFFF00) == nId2;
}

// line 981 (DWARF: declared inline, abstract instance only). Body inferred from the copy inlined
// into UpdatePlayerStateTagCheck (line 983: length compare, then memcmp of the uid bytes).
inline bool IsEqualId(const nn::nfp::TagId & nId1, const nn::nfp::TagId & nId2)
{
    return nId1.length == nId2.length && memcmp(nId1.uid, nId2.uid, nId1.length) == 0;
}
}  // namespace NFP
}  // namespace Console
