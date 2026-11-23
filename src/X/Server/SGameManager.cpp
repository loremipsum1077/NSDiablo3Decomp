// Original file: X/Server/SGameManager.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).
#include "SGameManager.h"
#include "SGame.h"
#include "SGameConstants.h"
#include "../Common/Shell/CmdLine.h"
#include "../Client/CGame.h"
#include "../Client/Prefetch.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/RefString.h"
#include "../Sigma/SigmaProf.h"
#include "../../Contrib/Contrib/Domino/Include/dmId.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Types.h"  // header statics run by this file's static initializer (ldn_Ipv4Address.h:153-163, ldn_MacAddress.h:101-106)
// Declared here from their definitions elsewhere (headers not adopted yet).
enum SeverityLevel {  // X/Sigma/ErrorManager/ErrorManagerLogging.h
    SLVL_INTERNAL = 0,
    SLVL_DEBUG = 1,
    SLVL_INFO = 2,
    SLVL_WARNING = 3,
    SLVL_CRASH_INFO = 4,
    SLVL_ERROR = 5,
    SLVL_SERVICE_ERROR = 6,
    SLVL_FATAL = 7,
};
enum OutputStreamType {  // X/Sigma/ErrorManager/ErrorManagerLogging.h
    OUTPUTSTREAM_DEFAULT = 0,
};
typedef void (*ErrorManagerLogSink)(const char *, int32, SeverityLevel, OutputStreamType, LPCSTR, LPCSTR);  // (invented name)
void ErrorManagerLoggingRegisterLogSink(ErrorManagerLogSink pfnSink);  // X/Sigma/ErrorManager/ErrorManagerLogging.cpp
void ErrorManagerLoggingUnregisterLogSink(ErrorManagerLogSink pfnSink);  // X/Sigma/ErrorManager/ErrorManagerLogging.cpp
struct dmWorldDef;
dmWorldId dmWorld_Create(const dmWorldDef * pDef);  // Contrib/Domino/Dynamics/dmWorld.cpp
void dmWorld_Destroy(dmWorldId worldId);  // Contrib/Domino/Dynamics/dmWorld.cpp
namespace Blizzard { namespace Process { void Sleep(uint32 uMilliseconds); } }  // BlizzardCore Process.h
namespace Prefetch { void Update(uint32); }  // X/Client/Prefetch.cpp
void ServerLimitPacketHistorySize(SGame * ptSGame);  // X/Common/Networking/Network.cpp:1570
void ServerTogglePacketBuffers(SGame * ptSGame);  // X/Common/Networking/Network.cpp:1560
void SGameUpdateGame(SGame * ptSGame, uint32 dwGameUpdateCount);  // X/Server/SGame.cpp:6249
void ConfigureServerGameManagerWorkerThread(Blizzard::Thread::Thread ptThread, uint32 uIndex);  // platform thread config (NX64)
void ConfigureLuaDeadlockThread(Blizzard::Thread::Thread ptThread);  // platform thread config (NX64)
uint32 LuaDeadlockThreadProc(void * lpData);  // X/Server/LuaScript/LuaAbortThreadWatcher.cpp:42
template <>
uint32 XHashKey<const OnlineService::GameAccountHandle &>(const OnlineService::GameAccountHandle & gameAccount);  // X/Common/OnlineService/OnlineService.cpp:1010
static void _GameLogCallback(const char *, int32, SeverityLevel eLevel, const OutputStreamType, LPCSTR szMessage, LPCSTR szPrependLabel);  // line 1735
namespace OnlineServiceServer { BOOL StringToTimeRec(const blz::string & str, blz::string & errMsg, Blizzard::Time::TimeRec & tr); }  // X/Server/OnlineService/OnlineServiceServer.cpp
#include "../Client/Console/Online.h"
#include "../../BlizzardCore/Include/Blizzard/Sys/Atomic.h"















static XVarScope s_scopeSGameManager("SGameManager", NULL);
XVarString s_varBroadcast(s_scopeSGameManager, "Broadcast", "");
XVarUint32 s_varKillGame(s_scopeSGameManager, "KillGame", 0);
XVarBool s_varDrainGames(s_scopeSGameManager, "DrainGames", false);
XVarUint32 s_varWorkerThreadCount(s_scopeSGameManager, "WorkerThreadCount", 0);
XVarUint32 s_varMaxGames(s_scopeSGameManager, "MaxGames", 1);
XVarUint32 s_varMaxGameUpdateCount(s_scopeSGameManager, "MaxGameUpdateCount", 2);
XVarUint32 s_varStatsUpdateSecondsInterval(s_scopeSGameManager, "StatsUpdateSecondsInterval", 0);
XVarUint32 s_varNumPlayersSecondsInterval(s_scopeSGameManager, "NumPlayersSecondsInterval", 0);
XVarUint32 s_varNumPlayersRoundingFactor(s_scopeSGameManager, "NumPlayersRoundingFactor", 10000);
XVarUint32 s_varGameChannelRetrySeconds(s_scopeSGameManager, "GameChannelRetrySeconds", 5);
XVarUint32 s_varDesiredScheduleHz(s_scopeSGameManager, "DesiredScheduleHz", 120);

XVarUint32 s_varStalledGameThresholdTicks(s_scopeSGameManager, "StalledGameThresholdTicks", 1800);
XVarUint32 s_varMainThreadSlowUpdateThresholdMicroseconds(s_scopeSGameManager, "ThreadSlowUpdateThresholdMicroseconds", 0);
XVarUint64 s_varMainThreadSlowestUpdateMicroseconds(s_scopeSGameManager, "MainThreadSlowestUpdateMicroseconds", 0);
XVarUint32 s_varStalledMainThreadThresholdSeconds(s_scopeSGameManager, "StalledMainThreadThresholdSeconds", 30);



XVarString s_varSeasonShutdownDate(s_scopeSGameManager, "SeasonShutdownDate", "");
XVarUint32 s_varSeasonShutdownWarnSecs(s_scopeSGameManager, "SeasonShutdownWarnSecs", 1800);

XVarBool s_varStalledGameDumpData(s_scopeSGameManager, "StalledGameDumpData", true);

XVarBool s_varLicenseRewards(s_scopeSGameManager, "LicenseRewards", true);
XVarString s_varManualJob(s_scopeSGameManager, "ManualJob", "");

XVarBool s_varErrorChatLoopback(s_scopeSGameManager, "ErrorChatLoopback", true);

static XVarScope s_scopeStats(s_scopeSGameManager, "Stats");
static XVarFloat s_varAvgMainThreadHz(s_scopeStats, "AvgMainThreadHz", 0.0f);
static XVarFloat s_varAvgWorkerThreadHz(s_scopeStats, "AvgWorkerThreadHz", 0.0f);
static XVarUint32 s_varNumGames(s_scopeStats, "NumGames", 0);
static XVarUint32 s_varNumClients(s_scopeStats, "NumClients", 0);
static XVarUint32 s_varNumContentLevel0Clients(s_scopeStats, "NumContentLevel0Clients", 0);
static XVarUint32 s_varNumContentLevel1Clients(s_scopeStats, "NumContentLevel1Clients", 0);
static XVarUint32 s_varNumContentLevel2Clients(s_scopeStats, "NumContentLevel2Clients", 0);
static XVarUint32 s_varNumSockets(s_scopeStats, "NumSockets", 0);
static XVarUint32 s_varAvgBytesSent(s_scopeStats, "AvgBytesSent", 0);
static XVarUint32 s_varAvgBytesRecieved(s_scopeStats, "AvgBytesRecieved", 0);
static XVarUint32 s_varPeakBytesSent(s_scopeStats, "PeakBytesSent", 0);
static XVarUint32 s_varPeakBytesRecieved(s_scopeStats, "PeakBytesRecieved", 0);
static XVarFloat s_varLoadUser(s_scopeStats, "LoadUser", 0.0f);
static XVarFloat s_varLoadSys(s_scopeStats, "LoadSys", 0.0f);
static XVarFloat s_varLoadIdle(s_scopeStats, "LoadIdle", 1.0f);
static XVarUint32 s_varNumGamesCreated(s_scopeStats, "NumGamesCreated", 0);
static XVarUint32 s_varNumGamesDestroyed(s_scopeStats, "NumGamesDestroyed", 0);
XVarBool s_varLogRiftStats(s_scopeStats, "LogRiftStats", true);





const float BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER1 = 0.1f;
const float BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER2 = 0.0f;

XVarFloat s_varBlizzconRiftTier1RewardDropChance(s_scopeSGameManager, "BlizzconRiftTier1RewardDropChance", BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER1);
XVarFloat s_varBlizzconRiftTier2RewardDropChance(s_scopeSGameManager, "BlizzconRiftTier2RewardDropChance", BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER2);

XVarBool s_varExperimentalScheduling(s_scopeSGameManager, "ExperimentalScheduling", false);
static bool s_bExperimentalScheduling;














SGameManager * SGameManager::s_ptGameManager = NULL;





void SGameManager::InitializeForApp()
{
    s_ptGameManager = new SGameManager();
    s_ptGameManager->ResumeWorkerThreads();
}






void SGameManager::DisposeForApp()
{
    delete s_ptGameManager;
    s_ptGameManager = NULL;
}



























// NON_MATCHING: register allocation (original keeps this+0xd0 in x28 and the vector size/capacity addresses in callee-saved registers); 16 bytes longer
SGameManager::SGameManager()
    : m_bWorkerThreadsTerminate(false)
    , m_tWorkerThreadSemaphore(0)
{
    s_bExperimentalScheduling = s_varExperimentalScheduling.Get();


    m_uWorkerThreadCount = 0;
    m_patWorkerThreads = NULL;

    _UpdateMaxGames();













    m_uWorkerThreadCount = 1;


    s_varWorkerThreadCount.Set(1u, 3);

    m_ptSGames = TDataArrayCreateTracked<SGame, int>("SGames", NULL, MAX_SGAMES);

    if (m_uWorkerThreadCount)
    {
        m_patWorkerThreads = (SGameThread *)SigmaMemoryNew(m_uWorkerThreadCount * sizeof(SGameThread), 4, NULL, TRUE);





        for (uint32 i = 0; i < m_uWorkerThreadCount; i++)
        {
            SGameThread * ptWorkerThread = &m_patWorkerThreads[i];

            ptWorkerThread->hWakeEvent.Create();

            BOOL bSuccess = Blizzard::Thread::Create(_ThreadProc, ptWorkerThread, &ptWorkerThread->tThread, "ServerGameManagerWorkerThread", 1, 0x40000);

            Blizzard::Thread::SetPriority(ptWorkerThread->tThread, Blizzard::Thread::PRIORITY_ABOVE_NORMAL);



            ConfigureServerGameManagerWorkerThread(ptWorkerThread->tThread, i);









            m_tLuaDeadlockThreadData.taGameThreadData.push_back(&ptWorkerThread->tLuaDeadlockGameThreadData);

            CRefString sThreadName;
            sThreadName.Format("SGame Simulation Thread %d", i);
            SigmaProfRegisterThread(ptWorkerThread->tThread, sThreadName, 0x1000);
        }
    }

    BOOL bSuccess = Blizzard::Thread::Create(LuaDeadlockThreadProc, &m_tLuaDeadlockThreadData, &m_tLuaDeadlockThread, "LuaDeadLockThread", 1, 0);







    ConfigureLuaDeadlockThread(m_tLuaDeadlockThread);



    m_tLuaDeadlockThreadData.bCheckForMainThreadStall = FALSE;
    m_tLuaDeadlockThreadData.uMainLoopUpdateCount = 0;
    m_tLuaDeadlockThreadData.taGameThreadData.push_back(&m_tMainThread_LuaDeadlockGameThreadData);
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData = &m_tMainThread_LuaDeadlockGameThreadData;


    Blizzard::Thread::Resume(m_tLuaDeadlockThread);

    m_lastStats = 0;
    m_nShutdownTime = 0;
    m_nShutdownMessageTime = 0;
    m_nDelayTime = 0;
    m_nDrainTime = 0;
    m_nShutdownSecPostDelay = 0;
    m_bShuttingDown = false;
    m_nSeasonShutdownTime = 0;
    m_bSeasonShutdownHandled = FALSE;
    m_cachedSeasonShutdownDate = "";

    m_uLastMainThreadTick = 0;
    m_uTotalMainThreadUpdates = 0;
    m_uTotalGameTicksScheduled = 0;
    m_uTotalGamesScheduled = 0;

    m_atSimComplete[0].m_uCount = 0;
    m_atSimComplete[1].m_uCount = 0;
    m_uActiveSimCompleteIndex = 0;

    m_lastFetchNumServicePlayersTime = 0;
    m_eTriggerShutdownType = SST_SHUTDOWN;
    m_nTriggerShutdownTimeWarning = 0;
    m_eShutdownType = SST_SHUTDOWN;
    m_bTriggerSendBroadcast = FALSE;

    m_lastWorkSeconds = 0;

    m_uGameFrameUpdateCount = 0;
    m_uLastGameClockTicksWork = 0;

    ErrorManagerLoggingRegisterLogSink(_GameLogCallback);
}




SGameManager::~SGameManager()
{


    ErrorManagerLoggingUnregisterLogSink(_GameLogCallback);

    TDataArrayDispose(m_ptSGames);


    m_tLuaDeadlockThreadData.hExitThreadEvent.Set();
    Blizzard::Thread::WaitForever(m_tLuaDeadlockThread);
    Blizzard::Thread::Close(m_tLuaDeadlockThread);


    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData = NULL;


    m_bWorkerThreadsTerminate = true;


    if (s_bExperimentalScheduling)
    {
        for (uint32 i = 0; i < m_uWorkerThreadCount; i++)
        {
            m_tWorkerThreadSemaphore.post();
        }
    }



    for (uint32 i = 0; i < m_uWorkerThreadCount; i++)
    {
        SGameThread * ptWorkerThread = &m_patWorkerThreads[i];
        SigmaProfUnregisterThread(ptWorkerThread->tThread);

        if (!s_bExperimentalScheduling)
        {
            ptWorkerThread->hWakeEvent.Set();
        }

        Blizzard::Thread::WaitForever(ptWorkerThread->tThread);
        Blizzard::Thread::Close(ptWorkerThread->tThread);

        ptWorkerThread->hWakeEvent.Destroy();
    }


    SigmaMemoryFree(m_patWorkerThreads, NULL); m_patWorkerThreads = NULL;
}
SGameID SGameManager::GameAllocate()
{
    s_varNumGamesCreated.Increment(1);
    return s_ptGameManager->m_ptSGames->ElementNew();
}






void SGameManager::GameDelete(SGameID idSGame)
{
    s_varNumGamesDestroyed.Increment(1);
    s_ptGameManager->m_ptSGames->ElementDeleteByID(idSGame);
}










dmWorldId SGameManager::CreateDominoWorld(const dmWorldDef * pWorldDef)
{
    Blizzard::Lock::ScopedMutex scopedMutex(s_ptGameManager->m_tDominoWorldLock);
    return dmWorld_Create(pWorldDef);
}



void SGameManager::DestroyDominoWorld(dmWorldId id)
{
    Blizzard::Lock::ScopedMutex scopedMutex(s_ptGameManager->m_tDominoWorldLock);
    dmWorld_Destroy(id);
}



inline void SGameManager::_UpdateMaxGames()
{

    uint32 uVarMaxGames = s_varMaxGames.Get();
    m_uMaxGames = Math::Min<int>(uVarMaxGames, MAX_SGAMES);
    if (uVarMaxGames != m_uMaxGames)
    {
        s_varMaxGames.Set(m_uMaxGames, 3);
    }
}




SGame * SGameManager::GameIterate(SGameIterator & tIterator)
{
    return s_ptGameManager->m_ptSGames->Iterate(tIterator);
}








void SGameManager::DisposeGames()
{


    s_ptGameManager->m_tLuaDeadlockThreadData.bCheckForMainThreadStall = FALSE;


    while (s_ptGameManager->m_ptSGames->GetTotalCount() > 0)
    {

        s_ptGameManager->_ClearSimCompleteList();


        SGameIterator tIt;
        while (s_ptGameManager->m_ptSGames->Iterate(tIt))
        {
            SGameID idSGame = tIt->id;

            if (!IsUpdating(idSGame))
            {

                SGameStopServer(idSGame);
            }
        }


        s_ptGameManager->_ProcessSocketDeleteList();

        if (s_ptGameManager->m_ptSGames->GetTotalCount() > 0)
        {


            Blizzard::Process::Sleep(5);
        }


        Prefetch::Update(0);
    }

}



void SGameManager::QueueGameFrameUpdate(uint32 uGameFrameUpdateCount)
{
    s_ptGameManager->m_uGameFrameUpdateCount += uGameFrameUpdateCount;
}

























void SGameManager::UpdateGames(
    uint32 uTotalGameClockTicks)
{


    s_ptGameManager->_UpdateMaxGames();



























    Blizzard::Time::UnixTime64 uCurrentTime = Blizzard::Time::ToUnixTime64(Console::Online::GetNetworkTimestamp());

    if (s_ptGameManager->m_lastWorkSeconds != uCurrentTime)
    {
        s_ptGameManager->m_lastWorkSeconds = uCurrentTime;

        s_ptGameManager->_UpdateCachedSeasonShutdownTime();
    }







    s_ptGameManager->_UpdateShutdownTimer();

    s_ptGameManager->_UpdateDelayTimer();

    s_ptGameManager->_UpdateDrainTimer();

    s_ptGameManager->_UpdateSeasonShutdownTimer();

    s_ptGameManager->m_uTotalMainThreadUpdates++;































    s_ptGameManager->_ClearSimCompleteList();



    s_ptGameManager->_ScheduleGamesForSimulation();































    SGameID idUtilityGame = ThreadGlobals::Get().m_idUtilityGame;
    if (idUtilityGame != -1)
    {
        SGame * ptSGame = GameTryToGet(idUtilityGame);
        if (ptSGame && ptSGame->uScheduledGameUpdateCount)
        {
            ptSGame->eSimState = SGAME_SIMULATING;
            SGameUpdateGame(ptSGame, ptSGame->uScheduledGameUpdateCount);
            ptSGame->eSimState = SGAME_SLEEPING;
        }
    }


    s_ptGameManager->_ProcessSocketDeleteList();






    Blizzard::Lock::Atomic::Increment((volatile int32 *)&s_ptGameManager->m_tLuaDeadlockThreadData.uMainLoopUpdateCount);
    s_ptGameManager->m_tLuaDeadlockThreadData.bCheckForMainThreadStall = TRUE;


    s_ptGameManager->m_uLastGameClockTicksWork = uTotalGameClockTicks;
}



void SGameManager::_UpdateGames()
{

    sgtAppGlobals.tGlobalServerSemaphore.WaitForever();

    SGameID idSGame;
    while ((idSGame = _DequeueSimGame()) != SGAMEID_INVALID)
    {
        _UpdateGame(idSGame);
    }

    sgtAppGlobals.tGlobalServerSemaphore.Set();
}












inline void SGameManager::_UpdateGame(SGameID idSGame)
{

    SGame * ptSGame = GameTryToGet(idSGame);
    if (ptSGame)
    {

        SGameUpdateGame(ptSGame, ptSGame->uScheduledGameUpdateCount);

        m_tSimCompleteLock.Enter();
        m_atSimComplete[m_uActiveSimCompleteIndex].AddGame(idSGame);
        m_tSimCompleteLock.Leave();
    }
}




static Blizzard::Time::Millisecond s_tLastServerIdle;
static Blizzard::Time::Millisecond s_tMinServerFrameMS = 16;

static void sLimitServerFrameRate()
{
    Blizzard::Time::Millisecond tCurrentTime = Blizzard::Time::Milliseconds();
    Blizzard::Time::Millisecond tElapsed = tCurrentTime - s_tLastServerIdle;
    if (tElapsed < s_tMinServerFrameMS)
    {
        Blizzard::Process::Sleep(s_tMinServerFrameMS - tElapsed);
    }
    s_tLastServerIdle = Blizzard::Time::Milliseconds();
}






uint SGameManager::_ThreadProc(void * ptParam)
{
    ThreadGlobals::InitializeForThread();
    SigmaProfInitializeForThread();

    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_SERVERWORKER;

    SGameThread * ptThread = (SGameThread *)ptParam;


    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData = &ptThread->tLuaDeadlockGameThreadData;

    do
    {




        sLimitServerFrameRate();

        s_ptGameManager->_UpdateGames();




















    } while (!s_ptGameManager->m_bWorkerThreadsTerminate);






    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData = NULL;

    ThreadGlobals::DisposeForThread();

    return 0;
}







float SGameManager::GetAverageWorkerThreadHz()
{
    return s_varAvgWorkerThreadHz.Get();
}








































































































bool SGameManager::SignalKillGame(SGameID idSGame)
{

    if (CmdLineGetParams()->eTestType != TESTTYPE_NONE)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    if (Exists())
    {

        SGame * ptGame = GameTryToGet(idSGame);
        if (ptGame && ptGame->ptSGameGlobals)
        {
            if ((ptGame->tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)) == 0)
            {
                uint32 uSignalFlags = (1 << SGAMEMANAGER_SIGNAL_KILL_GAME_BIT) |
                                      (1 << SGAMEMANAGER_SIGNAL_DISABLE_PLAYER_SAVE_BIT) |
                                      (1 << SGAMEMANAGER_SIGNAL_REMOVE_PLAYERS_BIT);




                s_ptGameManager->_SetGameSignalBits(*ptGame, uSignalFlags);
                return true;
            }
        }
    }
    return false;
}



















































bool SGameManager::SignalClearSaveInProgress(SGameID idSGame)
{
    if (Exists())
    {
        SGame * ptGame = GameTryToGet(idSGame);
        if (ptGame && ptGame->ptSGameGlobals)
        {
            uint32 uSignalFlags = (1 << SGAMEMANAGER_SIGNAL_CLEAR_SAVE_IN_PROGESS);
            s_ptGameManager->_SetGameSignalBits(*ptGame, uSignalFlags);
            return true;
        }
    }
    return false;
}



































void SGameManager::AddAccountSignal(
    SGameID idSGame,
    GameSignalType eSignal,
    const OnlineService::GameAccountHandle & tAccount,
    XVariant && tContext,
    OnlineServiceServer::ProtoMessageCallback && tCb)
{
    if (s_ptGameManager == NULL)
        return;

    SGame * ptGame = GameTryToGet(idSGame);
    if (ptGame && ptGame->ptSGameGlobals)
    {
        GameSignal tAccountSignal;
        tAccountSignal.eSignal = eSignal;
        tAccountSignal.tAccountId = tAccount;
        tAccountSignal.tContext = static_cast<XVariant &&>(tContext);
        tAccountSignal.tCb = tCb;

        ptGame->ptSGameGlobals->ptInactiveSignals->tGameSignalsList.AddTail_Tracked(tAccountSignal);
    }
}






















void SGameManager::ClearAccountSGameId(const OnlineService::GameAccountHandle & tGameAccountId)
{
    if (s_ptGameManager == NULL)
        return;


    SGame & tGame = GameGet();
    Blizzard::Lock::ScopedMutex tMutex(s_ptGameManager->m_tGameAccountToSGameIdMapLock);

    GameAccountToSGameIdMap::iterator it = s_ptGameManager->m_tGameAccountToSGameIdMap.find(tGameAccountId);


    if (it != s_ptGameManager->m_tGameAccountToSGameIdMap.end() && it->second == tGame.id)
    {

        s_ptGameManager->m_tGameAccountToSGameIdMap.erase(tGameAccountId);
    }
}

































































void SGameManager::_ClearSimCompleteList()
{
    SimCompleteStruct & tCompleteList = m_atSimComplete[m_uActiveSimCompleteIndex];


    m_tSimCompleteLock.Enter();
    m_uActiveSimCompleteIndex = !m_uActiveSimCompleteIndex;
    m_tSimCompleteLock.Leave();


    for (uint32 i = 0; i < tCompleteList.m_uCount; i++)
    {
        SGame * ptSGame = GameTryToGet(tCompleteList.m_auGames[i]);
        if (ptSGame)
        {
            ptSGame->eSimState = SGAME_SLEEPING;
            ptSGame->uScheduledGameUpdateCount = 0;




            if ((ptSGame->ptSGameGlobals->dwFlags & (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN)) &&
                !(ptSGame->ptSGameGlobals->dwFlags2 & (1 << SGAMEGLOBALS2_SAVE_IN_PROGRESS)) &&
                ptSGame->ptSGameGlobals->tOrphanedPacketBufferList.IsEmpty())
            {

                Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tGlobalDisposeServerMutex);

                SGameStopServer(ptSGame->id);
            }
            else
            {

                ServerLimitPacketHistorySize(ptSGame);
            }
        }
    }

    tCompleteList.m_uCount = 0;
}




void SGameManager::_ScheduleGamesForSimulation()
{
    uint32 uStalledGameThreshold = s_varStalledGameThresholdTicks.Get();
    if (CGameLoadingScreenIsEnabled())
    {
        uStalledGameThreshold *= 10;
    }










    SGameIterator tIt;







    while (SGame * ptSGame = GameIterate(tIt))
    {



        uint32 uOldPendingCount = ptSGame->uPendingGameUpdateCount;
        ptSGame->uPendingGameUpdateCount += m_uGameFrameUpdateCount;
        if (uOldPendingCount > uStalledGameThreshold)
        {
        }
























        if (m_bTriggerSendBroadcast)
        {
            _SetGameSignalBits(*ptSGame, (1 << SGAMEMANAGER_SIGNAL_BROADCAST_BIT));
        }

        if (m_nTriggerShutdownTimeWarning &&

            _GameRequiresShutdownMessage(ptSGame))
        {
            ptSGame->ptSGameGlobals->ptInactiveSignals->nShutdownWarningTime = m_nTriggerShutdownTimeWarning;
            ptSGame->ptSGameGlobals->ptInactiveSignals->eShutdownType = m_eTriggerShutdownType;
        }



        if (ptSGame->eSimState == SGAME_SLEEPING && ptSGame->uPendingGameUpdateCount)
        {


            ptSGame->uScheduledGameUpdateCount = ptSGame->uPendingGameUpdateCount;

            ptSGame->uPendingGameUpdateCount = 0;
            ServerTogglePacketBuffers(ptSGame);

            ExternalGameSignals * ptTempExternalSignals = ptSGame->ptSGameGlobals->ptInactiveSignals;
            ptSGame->ptSGameGlobals->ptInactiveSignals = ptSGame->ptSGameGlobals->ptActiveSignals;
            ptSGame->ptSGameGlobals->ptActiveSignals = ptTempExternalSignals;




            if (!(ptSGame->tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)))
            {
                m_uTotalGameTicksScheduled += ptSGame->uScheduledGameUpdateCount;
                m_uTotalGamesScheduled++;
                ptSGame->eSimState = SGAME_SIMULATING;
                _QueueSimGame(ptSGame);

                if (s_bExperimentalScheduling && m_uWorkerThreadCount)
                {
                    m_tWorkerThreadSemaphore.post();
                }
            }
        }
    }

    m_uGameFrameUpdateCount = 0;
    m_bTriggerSendBroadcast = FALSE;
    m_eTriggerShutdownType = SST_SHUTDOWN;
    m_nTriggerShutdownTimeWarning = 0;
}

void SGameManager::_QueueSimGame(SGame * ptSGame)
{

    Blizzard::Lock::ScopedMutex scopedMutex(m_tSimPendingLock);
    m_tSimPendingQueue.push_back(ptSGame->id);
}



inline SGameID SGameManager::_DequeueSimGame()
{

    Blizzard::Lock::ScopedMutex scopedMutex(m_tSimPendingLock);
    SGameID idRetGame = SGAMEID_INVALID;

    if (!m_tSimPendingQueue.empty())
    {
        idRetGame = m_tSimPendingQueue.front();
        m_tSimPendingQueue.pop_front();
    }

    return idRetGame;
}



inline void SGameManager::_UpdateShutdownTimer()
{

    if (m_bShuttingDown)
    {
        int64 diff = m_nShutdownTime - m_lastWorkSeconds;
        if (diff % 60 == 0 || diff == 30 || diff == 10)
        {


            if (m_nShutdownMessageTime != m_lastWorkSeconds)
            {
                m_nShutdownMessageTime = m_lastWorkSeconds;



                m_nTriggerShutdownTimeWarning = (int32)diff;
                m_eTriggerShutdownType = m_eShutdownType;
            }
        }
        if (diff <= 0)
        {
            SGameIterator tIterator;


            while (SGame * ptSGame = GameIterate(tIterator))
            {

                if (!(ptSGame->tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)))
                {
                    _SetGameSignalBits(*ptSGame, (1 << SGAMEMANAGER_SIGNAL_KILL_GAME_BIT));
                }
            }

            m_bShuttingDown = false;
        }
    }
}



inline void SGameManager::_UpdateDelayTimer()
{

    if (m_nDelayTime != 0)
    {

        if (m_nDelayTime <= m_lastWorkSeconds)
        {

            m_bShuttingDown = true;
            m_nShutdownTime = m_lastWorkSeconds + m_nShutdownSecPostDelay;
            m_nDelayTime = 0;
            m_nShutdownSecPostDelay = 0;
        }
    }
}



inline void SGameManager::_UpdateDrainTimer()
{

    if (m_nDrainTime != 0)
    {

        if (m_nDrainTime <= m_lastWorkSeconds)
        {

            s_varDrainGames.Set(true, 3);
            m_nDrainTime = 0;
        }
    }
}



void SGameManager::_UpdateSeasonShutdownTimer()
{
    if (m_bSeasonShutdownHandled ||
        m_nSeasonShutdownTime == 0)
        return;
    Blizzard::Time::UnixTime64 secondsLeft = m_nSeasonShutdownTime - m_lastWorkSeconds;
    if (secondsLeft <= 0)
    {




        SGameIterator tIt; while (SGame * ptSGame = GameIterate(tIt))
        {

            if ((ptSGame->tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_SEASON)) && !(ptSGame->tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)))
            {
                _SetGameSignalBits(*ptSGame, 1 << SGAMEMANAGER_SIGNAL_KILL_GAME_BIT);
            }
        }
        m_bSeasonShutdownHandled = TRUE;
    }
    else if (secondsLeft <= s_varSeasonShutdownWarnSecs.Get() &&
             (secondsLeft % 60 == 0 || secondsLeft == 30 || secondsLeft == 10) &&




             m_nShutdownMessageTime != m_lastWorkSeconds)
    {
        m_nShutdownMessageTime = m_lastWorkSeconds;



        m_nTriggerShutdownTimeWarning = (int32)secondsLeft;
        m_eTriggerShutdownType = SST_SEASON_ENDED;
    }


}



void SGameManager::_UpdateCachedSeasonShutdownTime()
{
    blz::string shutdownDate = s_varSeasonShutdownDate.Get();

    if (m_cachedSeasonShutdownDate != shutdownDate)
    {

        Blizzard::Time::TimeRec tr; blz::string errMsg;
        m_nSeasonShutdownTime = 0;

        if (!shutdownDate.empty() &&

            OnlineServiceServer::StringToTimeRec(shutdownDate, errMsg, tr))
        {




            Blizzard::Time::Timestamp time = Blizzard::Time::MakeTime(tr);
            m_nSeasonShutdownTime = Blizzard::Time::ToUnixTime64(time);
        }

        m_cachedSeasonShutdownDate = shutdownDate;


        m_bSeasonShutdownHandled = m_nSeasonShutdownTime < m_lastWorkSeconds;







    }
}



inline BOOL SGameManager::_GameRequiresShutdownMessage(SGame * ptSGame)
{

    return m_eTriggerShutdownType != SST_SEASON_ENDED || (ptSGame->tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_SEASON));
}







void SGameManager::_ProcessSocketDeleteList()
{


    blz::list<SocketAndReason> tWorkingSocketDeleteList;
    m_tSocketDeleteLock.Enter();
    tWorkingSocketDeleteList.splice(tWorkingSocketDeleteList.end(), m_tSocketDeleteList);
    m_tSocketDeleteLock.Leave();

    while (tWorkingSocketDeleteList.size())
    {
        SocketAndReason info = tWorkingSocketDeleteList.front();

        tWorkingSocketDeleteList.pop_front();

        ServerSocketID idServerSocket = info.socketId;
        SGameID gameId = XServerSocketGetSGameID(idServerSocket);
        XServerSocketSetSGameID(idServerSocket, SGAMEID_INVALID);
        XServerSocketDisconnect(idServerSocket, info.reason);
        XServerSocketFinishDisconnect(idServerSocket, info.reason, gameId);
    }

}





inline void SGameManager::_SetGameSignalBits(SGame & tSGame, uint32 uSignalBits)
{
    SGameGlobals * ptSGameGlobals = tSGame.ptSGameGlobals;
    ptSGameGlobals->ptInactiveSignals->uGameManagerSignalFlags |= uSignalBits;
}





inline void SGameManager::ResumeWorkerThreads()
{
    for (uint32 i = 0; i < m_uWorkerThreadCount; i++)
    {
        Blizzard::Thread::Resume(m_patWorkerThreads[i].tThread);
    }
}



























































































































































































static void _GameLogCallback(const char *, int32, SeverityLevel eLevel, const OutputStreamType, LPCSTR szMessage, LPCSTR szPrependLabel)
{
    // Keep a short history of errors per game so they can be echoed to chat.



    uint32 uGameId;
    if ((eLevel == SLVL_ERROR || eLevel == SLVL_SERVICE_ERROR) &&
        SGameManager::Exists() &&
        SGameManager::ErrorMgrGetCurrentGameID(uGameId) &&
        uGameId != SGAMEID_INVALID &&
        s_varErrorChatLoopback.Get())
    {
        // Loopback




        SGame & tSGame = SGameManager::GameGet();
        if (tSGame.ptSGameGlobals &&
            !(tSGame.ptSGameGlobals->dwFlags & (1 << SGAMEGLOBALS_SHUTTING_DOWN)))
        {




            uint32 nLogHistoryIndex = (tSGame.ptSGameGlobals->nLogHistoryCurrentIndex + 1) % 10;
            Blizzard::String::Copy(tSGame.ptSGameGlobals->szLogHistory[nLogHistoryIndex], szMessage, 80);
            tSGame.ptSGameGlobals->nLogHistoryCurrentIndex = nLogHistoryIndex;
        }
    }
}
