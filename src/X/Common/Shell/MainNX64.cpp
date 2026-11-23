// Original file: X/Common/Shell/MainNX64.cpp
// nnMain (with sMain, sRotateLogFile and sExitCleanup inlined), the static callbacks it
// registers, and the file statics behind the static initializer.
// references are not written yet.
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "../../../NintendoSDK/Include/nn/util/util_TypedStorage.h"
#include "../../Sigma/SigmaTypes.h"

#include <new>
#include <string.h>

#include "../../../NintendoSDK/Include/nn/os/os_DebugTypes.h"
namespace nn {
namespace mem {
class StandardAllocator {  // size 0x48 (NintendoSDK/Include/nn/mem/mem_StandardAllocator.h)
public:
    StandardAllocator();
    void Initialize(void *, size_t);
    void * Allocate(size_t);
    void * Allocate(size_t, size_t);
    void Free(void *);
    void * Reallocate(void *, size_t);
private:
    uint8 m_abStorage[0x48];
};
}  // namespace mem
namespace os {









void QueryMemoryInfo(MemoryInfo * info);
nn::Result SetMemoryHeapSize(size_t size);
nn::Result AllocateAddressRegion(uintptr_t * address, size_t size);
nn::Result AllocateMemoryPages(uintptr_t address, size_t size);
void SetMemoryAllocatorForThreadLocal(void * (*allocator)(size_t, size_t), void (*deallocator)(void *, size_t));
}  // namespace os
namespace util {
template <typename T, unsigned long Size, unsigned long Alignment>
inline T & Get(TypedStorage<T, Size, Alignment> & storage) { return *reinterpret_cast<T *>(&storage); }
}  // namespace util
}  // namespace nn

// Contrib/Contrib/fmodstudiosrc11005/lowlevel_api/platforms/nx/src/fmod_nx.h
struct FMOD_NX_THREADAFFINITY {  // size 0x24
    unsigned int mixer;  // 0x00
    unsigned int stream;  // 0x04
    unsigned int nonblocking;  // 0x08
    unsigned int file;  // 0x0c
    unsigned int geometry;  // 0x10
    unsigned int profiler;  // 0x14
    unsigned int studioUpdate;  // 0x18
    unsigned int studioLoadBank;  // 0x1c
    unsigned int studioLoadSample;  // 0x20
};
extern "C" int FMOD_NX_SetThreadAffinity(FMOD_NX_THREADAFFINITY * affinity);

// BlizzardCore/Source/System/Thread/ThreadAPI.cpp:285
namespace Blizzard { namespace Thread { void SetThreadAffinity(Thread thread, uint32 dwAffinityMask); } }
#include "../../../NintendoSDK/Include/nn/nn_BitTypes.h"
#include "../../../NintendoSDK/Include/nn/os/os_ThreadTypes.h"
#include "../../../NintendoSDK/Include/nn/diag/diag_LogTypes.h"
#include <stdio.h>
void InputPlatformDispose();  // X/Client/Input
void BuildLockerValidate();
char g_avscprintfScratch[1];
void * g_romCache;

static const size_t SYSTEM_MEMORY_SIZE = 0x30000000;
nn::util::TypedStorage<nn::mem::StandardAllocator, 72, 8> g_systemAllocator;
BOOL gbDevMemMode;
BOOL InputInitialize();  // X/Client/Input/Input.cpp:153
extern "C" void * malloc(size_t size);
extern "C" void free(void * p);
BOOL ShellInitialize(uint32 dwParam1, uint32 dwParam2);  // X/Common/Shell/ShellNX64.cpp
#include "../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Using.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdCore.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdMemory/bdMemory-std.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Console/Online.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../Game.h"
#include "../GameTesting.h"
#include "Shell.h"
#include "CmdLine.h"
#include "../OnlineService/OnlineService.h"
#include "../../Sigma/Sigma.h"
#include "../../Sigma/buildver.h"
#include "../../Sigma/StringManip.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/ErrorManager/ErrorManagerLogging.h"
static void sErrorManagerCleanup()
{
}



static void * sThreadLocalAlloc(size_t size, size_t alignment)
{
    return nn::util::Get(g_systemAllocator).Allocate(size, alignment);
}

static void sThreadLocalDealloc(void * ptr, size_t size)
{
    free(ptr);
}



#include "../../Sigma/Files/XFiles.h"
#include "../../Sigma/Memory/SigmaMemory.h"
namespace nn {
namespace os {
struct UserExceptionInfo;  // NintendoSDK/Include/nn/os/os_UserExceptionHandlerTypes.h:83
int GetHostArgc();
char ** GetHostArgv();
ThreadType * GetCurrentThread();
void SetThreadCoreMask(ThreadType * pThread, int idealCore, Bit64 affinityMask);
void ChangeThreadPriority(ThreadType * pThread, int priority);
void SetUserExceptionHandler(void (*pHandler)(UserExceptionInfo *), void * pStack, size_t stackSize, UserExceptionInfo * pExceptionInfo);
}  // namespace os
namespace fs {
nn::Result QueryMountRomCacheSize(size_t * pOutValue);
nn::Result MountRom(const char * name, void * pCacheBuffer, size_t cacheBufferSize);
void Unmount(const char * name);
}  // namespace fs
namespace time {
nn::Result Initialize();
nn::Result Finalize();
}  // namespace time
namespace oe {
void FinishStartupLogo();
}  // namespace oe
namespace socket {
class Config;
nn::Result Initialize(const Config & config);
nn::Result Finalize();
}  // namespace socket
}  // namespace nn
// X/Client/Gfx/NX64/NX64NVN.h:400 (that header is not included here).
class GFXNX64NVN {
public:
    static void EarlyInit();
};
extern CmdLineParams gtCmdLineParams;  // X/Common/Shell/CmdLine.cpp:161
void ConfigureMainThread(nn::os::ThreadType * tThread)
{
    nn::os::SetThreadCoreMask(tThread, -1, 7);  // -1: nn::os::IdealCoreDontCare (SDK name, not in the DWARF)
    nn::os::ChangeThreadPriority(tThread, 15);
}
void ConfigureRenderThread(Blizzard::Thread::Thread tThread, uint32 uIndex)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureJobThread(Blizzard::Thread::Thread tThread, uint32 uIndex)
{
    Blizzard::Thread::SetPriority(tThread, Blizzard::Thread::PRIORITY_NORMAL);
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureFMODThreads()
{
    FMOD_NX_THREADAFFINITY affinity;
    affinity.nonblocking = 7;
    affinity.file = 7;
    affinity.geometry = 7;
    affinity.profiler = 7;
    affinity.mixer = 7;
    affinity.stream = 7;
    affinity.studioLoadSample = 7;
    affinity.studioUpdate = 7;
    affinity.studioLoadBank = 7;
    FMOD_NX_SetThreadAffinity(&affinity);
}

void ConfigureBDNetworkThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureConsoleSaveLoadThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
    Blizzard::Thread::SetPriority(tThread, Blizzard::Thread::PRIORITY_LOWEST);
}
void ConfigureUpdatePrivilegesThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
    Blizzard::Thread::SetPriority(tThread, Blizzard::Thread::PRIORITY_LOWEST);
}




void ConfigureLuaDeadlockThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureServerIdleThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureCutsceneDecodeThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureCutsceneAsyncThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureClientPreloadAsyncThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
    Blizzard::Thread::SetPriority(tThread, Blizzard::Thread::PRIORITY_LOWEST);
}
void ConfigureServerPreloadAsyncThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
    Blizzard::Thread::SetPriority(tThread, Blizzard::Thread::PRIORITY_LOWEST);
}




void ConfigureSNOFilesAsyncThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureServerGameManagerWorkerThread(Blizzard::Thread::Thread tThread, uint32 uIndex)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}




void ConfigureTaskThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}





void ConfigureGamePerfTestFileThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
    Blizzard::Thread::SetPriority(tThread, Blizzard::Thread::PRIORITY_LOWEST);
}
void ConfigureUISpriteThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
    Blizzard::Thread::SetPriority(tThread, Blizzard::Thread::PRIORITY_NORMAL);
}
void ConfigureDominoAsyncPhysicsThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void ConfigureCSAThread(Blizzard::Thread::Thread tThread)
{
    Blizzard::Thread::SetThreadAffinity(tThread, 7);
}
void CmdLineParse(LPCSTR szCmdLine);  // X/Common/Shell/CmdLine.cpp:224
void HyperlinkRemoveFormattingFromString(char * pszModifiedString, const char * pszString, int32 nModifiedBufSize);  // X/Common/Hyperlink.cpp:551
void ErrorManagerSetCustomDescriptionCallback(PFNCUSTOMDESCCALLBACK pfnCustomDescCallback);  // X/Sigma/ErrorManager

static void sGetPrettyText(
    LPCSTR pszString,
    char * pszModifiedBuf,
    int32 nModifiedBufSize)
{
    HyperlinkRemoveFormattingFromString(pszModifiedBuf, pszString, nModifiedBufSize);
}
void GameEndOfCrash();  // X/Common/GameCommonData.cpp:283
static void* sBDAllocate(const bdUWord size) { return SigmaMemoryNew(size, 4, NULL, FALSE); }
static void sBDDeallocate(void * p) { SigmaMemoryFree(p, NULL); }
static void* sBDReallocate(void * p, const bdUWord size) { return SigmaMemoryResize(p, NULL, size, 4); }
static void* sBDAllocateAligned(const bdUWord size, const bdUWord align) { return SigmaMemoryNew(size, align, NULL, FALSE); }
static void sBDDeallocateAligned(void * p) { SigmaMemoryFree(p, NULL); }
static void* sBDReallocateAligned(void * p, const bdUWord size, const bdUWord align) { return SigmaMemoryResize(p, NULL, size, align); }
void ShellEventLoop();  // X/Common/Shell/ShellNX64.cpp
void ShellDispose();  // X/Common/Shell/ShellNX64.cpp
void InitializeFilesNX64(const char * szMountName, bool bMountedROM);  // parameter names invented
void InitializeTimeNX64();
static void sMainShutdown()

{

    nn::fs::Unmount("data");
    if (NULL != g_romCache)
    {
        free(g_romCache);
    }

    g_romCache = NULL;
}

static void sExitCleanup()
{

    OnlineService::SetShellDisposing(TRUE);
    ShellDispose();
    AppDispose();
    bdCore::quit();
    InputPlatformDispose();
    SigmaDispose();






    { nn::Result nn_result = nn::time::Finalize(); }
    { nn::Result nn_result = nn::socket::Finalize(); }
}
namespace nn {
namespace socket {
const int DefaultSocketAllocatorSize = 131072;  // NintendoSDK/Include/nn/socket/socket_Constants.h:51-63
const int DefaultConcurrencyLimit = 14;
const int LibraryVersion = 4;
class Config {  // NintendoSDK/Include/nn/socket/socket_Config.h:47, size 0x58
public:
    Config(void * pMemoryPool, size_t memoryPoolSize, size_t allocatorPoolSize,
           size_t tcpInitialSendBufferSize, size_t tcpInitialReceiveBufferSize,
           size_t tcpAutoSendBufferSizeMax, size_t tcpAutoReceiveBufferSizeMax,
           size_t udpSendBufferSize, size_t udpReceiveBufferSize,
           int socketBufferEfficiency, int concurrencyCountMax)
        : m_Version(LibraryVersion), m_IsSystemClient(false), m_pMemoryPool(pMemoryPool), m_MemoryPoolSize(memoryPoolSize),
          m_AllocatorPoolSize(allocatorPoolSize), m_TcpInitialSendBufferSize(tcpInitialSendBufferSize),
          m_TcpInitialReceiveBufferSize(tcpInitialReceiveBufferSize), m_TcpAutoSendBufferSizeMax(tcpAutoSendBufferSizeMax),
          m_TcpAutoReceiveBufferSizeMax(tcpAutoReceiveBufferSizeMax), m_UdpSendBufferSize(udpSendBufferSize),
          m_UdpReceiveBufferSize(udpReceiveBufferSize), m_SocketBufferEfficiency(socketBufferEfficiency),
          m_ConcurrencyCountMax(concurrencyCountMax)
    {
    }
private:
    uint32_t m_Version;  // 0x00
protected:
    bool m_IsSystemClient;  // 0x04
    void * m_pMemoryPool;  // 0x08
    size_t m_MemoryPoolSize;  // 0x10
    size_t m_AllocatorPoolSize;  // 0x18
    size_t m_TcpInitialSendBufferSize;  // 0x20
    size_t m_TcpInitialReceiveBufferSize;  // 0x28
    size_t m_TcpAutoSendBufferSizeMax;  // 0x30
    size_t m_TcpAutoReceiveBufferSizeMax;  // 0x38
    size_t m_UdpSendBufferSize;  // 0x40
    size_t m_UdpReceiveBufferSize;  // 0x48
    int m_SocketBufferEfficiency;  // 0x50
    int m_ConcurrencyCountMax;  // 0x54
};
// socket_Config.h:446 (size 0x58)
class ConfigDefault : public Config {
public:
    static const size_t DefaultTcpInitialSendBufferSize = 32768;
    static const size_t DefaultTcpInitialReceiveBufferSize = 65536;
    static const size_t DefaultTcpAutoSendBufferSizeMax = 262144;
    static const size_t DefaultTcpAutoReceiveBufferSizeMax = 262144;
    static const size_t DefaultUdpSendBufferSize = 9216;
    static const size_t DefaultUdpReceiveBufferSize = 42240;
    static const int DefaultSocketBufferEfficiency = 4;
    ConfigDefault(void * pMemoryPool, size_t memoryPoolSize, size_t allocatorPoolSize)
        : Config(pMemoryPool, memoryPoolSize, allocatorPoolSize,
                 DefaultTcpInitialSendBufferSize, DefaultTcpInitialReceiveBufferSize,
                 DefaultTcpAutoSendBufferSizeMax, DefaultTcpAutoReceiveBufferSizeMax,
                 DefaultUdpSendBufferSize, DefaultUdpReceiveBufferSize,
                 DefaultSocketBufferEfficiency, DefaultConcurrencyLimit)
    {
    }
};
// socket_Config.h:615 (size 0x601000)
class ConfigDefaultWithMemory : public ConfigDefault {
public:
    ConfigDefaultWithMemory()
        : ConfigDefault(m_MemoryPool, sizeof(m_MemoryPool), DefaultSocketAllocatorSize)
    {
    }
private:
    alignas(4096) uint8_t m_MemoryPool[6291456];  // 0x1000
};
}  // namespace socket
namespace diag {
struct AbortInfo;
typedef void (*AbortObserver)(const AbortInfo &);  // diag_AbortObserver.h:45
struct AbortObserverHolder {  // diag_AbortObserver.h:52, size 0x18
    AbortObserver observer;  // 0x00
    AbortObserverHolder * next;  // 0x08
    bool isRegistered;  // 0x10
};
struct AbortInfo {  // diag_AbortTypes.h:40, size 0x30
    int abortReason;  // 0x00 AbortReason
    const LogMessage * message;  // 0x08
    const char * condition;  // 0x10
    const char * functionName;  // 0x18
    const char * fileName;  // 0x20
    int lineNumber;  // 0x28
};
void InitializeAbortObserverHolder(AbortObserverHolder * pHolder, AbortObserver observer);
void RegisterAbortObserver(AbortObserverHolder * pHolder);
void EnableDefaultAbortObserver(bool isEnabled);
}  // namespace diag
}  // namespace nn
static blz::string sBuildVersionString;

static CHAR sCrashInfoFileName[14] = "CrashInfo.txt";
static CHAR sCrashInfoFilePath[4096];

alignas(32) char UserExceptionHandlerStack[16384];
static void UserExceptionHandler(nn::os::UserExceptionInfo * info)
{




    FILE * pFile = fopen_ex(sCrashInfoFilePath, "w");
    if (pFile)
    {




        fwrite(sBuildVersionString.c_str(), 1, sBuildVersionString.size(), pFile);
        fclose(pFile);
    }


}




static void sRotateLogFile(char * szlogFilePath)
{
    if (Blizzard::File::Exists(szlogFilePath))
    {
        char* szFilename = Blizzard::String::FindFilename(szlogFilePath);
        char* szExtension = Blizzard::String::FindExtensionWithLeadingDot(szlogFilePath);


        *szExtension = '\0';

        char szPath[128];
        size_t nLen = szFilename - szlogFilePath;

        SigmaMemoryCopy(szPath, szlogFilePath, nLen);
        szPath[nLen] = '\0';

        char szRenameTo[128];

        uint32 nCounter = 0;

        const char * szFormatString = "%s_%s-%d.txt";

        Blizzard::String::Format(szRenameTo, sizeof(szRenameTo), szFormatString, szPath, szFilename, nCounter);

        while (Blizzard::File::Exists(szRenameTo))
        {

            Blizzard::String::Format(szRenameTo, sizeof(szRenameTo), szFormatString, szPath, szFilename, ++nCounter);
        }

        static const int32 s_nMaxLogCounter = 8;  // value inferred from the `and w5, w21, #7` applied to nCounter + 1

        char szDeleteNext[256];

        Blizzard::String::Format(szDeleteNext, sizeof(szDeleteNext), szFormatString, szPath, szFilename, (nCounter + 1) % s_nMaxLogCounter);

        Blizzard::File::Delete(szDeleteNext);

        *szExtension = '.';

        Blizzard::File::Move(szlogFilePath, szRenameTo);
    }
}




static void sNxAbort(const nn::diag::AbortInfo & abortInfo)
{
    static CHAR szAbortMessage[1024];
    Blizzard::String::Format(szAbortMessage, sizeof(szAbortMessage), abortInfo.message->format, abortInfo.message->args);
    // szReason (DWARF line 494) has no location or value; whatever set it left no code.
    const char * szReason;
}

namespace Blizzard {
namespace Thread {
void SetDefaultStackSize(int stackSize);  // BlizzardCore Thread API
}  // namespace Thread
}  // namespace Blizzard
namespace GameTesting {
void Initialize();  // X/Common/GameTesting.cpp
}  // namespace GameTesting
namespace Console {
namespace Online {
void CreateDummySetup(const DummySetupParams & tParams);  // X/Client/Console/Online.cpp:5156
}  // namespace Online
}  // namespace Console















static nn::diag::AbortObserverHolder sNxAbortObserverHolder;




static int sMain(
    LPSTR lpCmdLine)
{
    Blizzard::Thread::SetDefaultStackSize(16384);

    nn::os::ThreadType * mainThreadId = nn::os::GetCurrentThread();
    ConfigureMainThread(mainThreadId);


    { nn::Result nn_result = nn::time::Initialize(); }
    static nn::socket::ConfigDefaultWithMemory nnSocketConfig;
    { nn::Result nn_result = nn::socket::Initialize(nnSocketConfig); }

    nn::diag::InitializeAbortObserverHolder(&sNxAbortObserverHolder, sNxAbort);
    nn::diag::RegisterAbortObserver(&sNxAbortObserverHolder);
    nn::diag::EnableDefaultAbortObserver(false);








    InitializeTimeNX64();


    static const char * dataMountName = "data";
    size_t romCacheSize = 0;
    { nn::Result nn_result = nn::fs::QueryMountRomCacheSize(&romCacheSize); }
    g_romCache = malloc(romCacheSize);
    nn::Result result = nn::fs::MountRom(dataMountName, g_romCache, romCacheSize);
    bool bMountedROM = true;
    if (result.IsFailure())
    {
        bMountedROM = false;
        free(g_romCache);
        g_romCache = NULL;
    }




    InitializeFilesNX64(dataMountName, bMountedROM);





    BuildLockerValidate();





    BOOL fSuccess;

    CmdLineParse(lpCmdLine);

    ErrorManagerSetLogStreamFilename(OUTPUTSTREAM_DEFAULT, "D3Debug.txt");





    ErrorManagerEnableLocalLogging(FALSE);


    GFXNX64NVN::EarlyInit();

    SigmaInit("Diablo III", 36);










    ErrorManagerSetDisplayErrorPrepCallback(sErrorManagerCleanup);
    ErrorManagerSetEndOfCrashCallback(GameEndOfCrash);
    ErrorManagerSetRemoveAppTextEncodingCallback(sGetPrettyText);

    ErrorManagerSetCustomDescriptionCallback(GameGetCustomCrashDescription);








    Blizzard::Using::Register("MainNX64", sMainShutdown);
    Blizzard::Using::DependsOn("ErrorManager::Logging", "MainNX64");

    bdCore::init(false);
    bdMemory::setAllocateFunc(sBDAllocate);
    bdMemory::setDeallocateFunc(sBDDeallocate);
    bdMemory::setReallocateFunc(sBDReallocate);
    bdMemory::setAlignedAllocateFunc(sBDAllocateAligned);
    bdMemory::setAlignedDeallocateFunc(sBDDeallocateAligned);
    bdMemory::setAlignedReallocateFunc(sBDReallocateAligned);





    if (!InputInitialize())
    {
        return FALSE;
    }


    sBuildVersionString.format("<BUILD>%s</BUILD>\n", BuildVersionCodeData());



    Blizzard::String::Format(sCrashInfoFilePath, sizeof(sCrashInfoFilePath), "%s/%s", ErrorManagerGetReportingFolder(), sCrashInfoFileName);

    sRotateLogFile(sCrashInfoFilePath);


    nn::os::SetUserExceptionHandler(UserExceptionHandler, UserExceptionHandlerStack, sizeof(UserExceptionHandlerStack), NULL);

    fSuccess = AppInitialize();
    if (!fSuccess)
    {
        return FALSE;
    }

    fSuccess = ShellInitialize(0, 0);
    if (!fSuccess)
    {
        return FALSE;
    }

    if (gtCmdLineParams.eTestType != TESTTYPE_NONE)
    {
        ErrorManagerSuppressUserInteraction(TRUE);
        GameTesting::Initialize();
    }
    else {
        if (AppIsBlizzardServer())
        {
            GameStartServer();
        }
        else if (strlen(gtCmdLineParams.sLoadWorld) > 0)
        {
            SNO snoWorld = SNOFileLookupGUID(WORLD_SNO, gtCmdLineParams.sLoadWorld);
            if (snoWorld != SNO_NONE)
            {
                Console::Online::DummySetupParams tParams;
                tParams.m_snoWorld = snoWorld;
                tParams.m_dwCreationFlags = 0x800;  // flag name unknown
                tParams.m_dwLocalPlayerCount = CmdLineGetParams()->nTestPlayerNum;

                if (CmdLineGetParams()->eTestClassOverride != -1)
                {
                    tParams.m_playerClasses[0] = CmdLineGetParams()->eTestClassOverride;
                    tParams.m_playerClasses[1] = CmdLineGetParams()->eTestClassOverride;
                    tParams.m_playerClasses[2] = CmdLineGetParams()->eTestClassOverride;
                    tParams.m_playerClasses[3] = CmdLineGetParams()->eTestClassOverride;
                }
                else
                {
                    tParams.m_playerClasses[0] = 0;
                    tParams.m_playerClasses[1] = 1;
                    tParams.m_playerClasses[2] = 2;
                    tParams.m_playerClasses[3] = 3;
                }
                Console::Online::CreateDummySetup(tParams);
            }
            else
            {
                GameStartMenu(ACT_5);
            }
        }
        else
        {
            GameStartMenu(ACT_5);
        }
    }


    if (SHELL_APP_STATE_GET() != APPSTATE_FINISHED)
    {
        SHELL_APP_STATE_SET(APPSTATE_IDLE);
    }

    nn::oe::FinishStartupLogo();

    ShellEnableSaveOnQuit(TRUE);

    bool bActive = true;


    while (SHELL_APP_STATE_GET() != APPSTATE_FINISHED)
    {







        ShellEventLoop();
        AppIdle();
    }


    // bActive (DWARF line 725) has no location; nothing left of it.
    sExitCleanup();

    return fSuccess;
}




extern "C" void nnMain()
{
    const int argc = nn::os::GetHostArgc();
    char ** const argv = nn::os::GetHostArgv();
    const size_t COMMAND_LINE_SIZE = 1024;
    char szCommandLine[COMMAND_LINE_SIZE];

    szCommandLine[0] = 0;
    if (argc > 1)
    {
        strcat_safe(szCommandLine, COMMAND_LINE_SIZE, argv[1]);
        for (int32 i = 2; i < argc; i++)
        {
            strcat_safe(szCommandLine, COMMAND_LINE_SIZE, " ");
            strcat_safe(szCommandLine, COMMAND_LINE_SIZE, argv[i]);
        }
    }
    szCommandLine[COMMAND_LINE_SIZE - 1] = 0;


    sMain(szCommandLine);
}




extern "C" void nninitStartup()
{
    nn::os::MemoryInfo info;
    nn::os::QueryMemoryInfo(&info);
    if (info.totalAvailableMemorySize > 0x100000000ull)
    {
        gbDevMemMode = TRUE;
    }

    nn::os::SetMemoryHeapSize(0);

    uintptr_t address = 0;
    nn::os::AllocateAddressRegion(&address, SYSTEM_MEMORY_SIZE);
    nn::os::AllocateMemoryPages(address, SYSTEM_MEMORY_SIZE);

    new (&nn::util::Get(g_systemAllocator)) nn::mem::StandardAllocator();
    nn::util::Get(g_systemAllocator).Initialize((void *)address, SYSTEM_MEMORY_SIZE);

    nn::os::SetMemoryAllocatorForThreadLocal(sThreadLocalAlloc, sThreadLocalDealloc);
}









extern "C" void * malloc(size_t size)
{
    return nn::util::Get(g_systemAllocator).Allocate(size);
}

extern "C" void free(void * p)
{
    if (p != NULL)
    {
        nn::util::Get(g_systemAllocator).Free(p);
    }
}

extern "C" void * calloc(size_t num, size_t size)
{
    size_t sum = num * size;
    void * p = malloc(sum);
    if (p != NULL)
    {
        memset(p, 0, sum);
    }
    return p;
}


extern "C" void * realloc(void * p, size_t newSize)
{
    return nn::util::Get(g_systemAllocator).Reallocate(p, newSize);
}

extern "C" void * aligned_alloc(size_t alignment, size_t size)
{
    return nn::util::Get(g_systemAllocator).Allocate(size, alignment);
}






