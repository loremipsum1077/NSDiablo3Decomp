// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SGameManager.h
// Subset: SGameManager::GameGet(), SGameManager::GetGameCount(), SGameManager::GetCurrentGame()
// and SGameManager::SignalKillGame().

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/ThreadGlobals.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "SGame.h"
#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Contrib/Contrib/blz/include/blz/atomic.h"
#include "../../Contrib/Contrib/blz/include/blz/deque.h"
#include "../../Contrib/Contrib/blz/include/blz/semaphore.h"
#include "../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Contrib/Contrib/blz/include/blz/list.h"
#include "../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "LuaScript/LuaAbortThreadWatcher.h"
#include "Network/XServerSocket.h"

struct dmWorldDef;
struct dmWorldId;

enum {
    SGAMEMANAGER_SIGNAL_BROADCAST_BIT = 0,
    SGAMEMANAGER_SIGNAL_KILL_GAME_BIT = 1,
    SGAMEMANAGER_SIGNAL_DISABLE_PLAYER_SAVE_BIT = 2,
    SGAMEMANAGER_SIGNAL_REMOVE_PLAYERS_BIT = 3,
    SGAMEMANAGER_SIGNAL_DUMP_GAME_INFO_BIT = 4,
    SGAMEMANAGER_SIGNAL_LOG_NEXT_UPDATE_BIT = 5,
    SGAMEMANAGER_SIGNAL_CLEAR_SAVE_IN_PROGESS = 6,
};

struct SGameThread {  // size 0x28
    Blizzard::Thread::Thread tThread;  // 0x00
    Blizzard::Lock::Event hWakeEvent;  // 0x08
    LuaDeadlockGameThreadData tLuaDeadlockGameThreadData;  // 0x14
};
static_assert(sizeof(SGameThread) == 0x28, "SGameThread");

class SGameManager {  // size 0x280
public:
    static const int32 MAX_SGAMES = 1;
    static void InitializeForApp();  // X/Server/SGameManager.cpp
    static void DisposeForApp();  // X/Server/SGameManager.cpp
    static BOOL Exists();  // line 83, inline: s_ptGameManager != NULL
    static void DisposeGames();  // X/Server/SGameManager.cpp
    static void SetCurrentGame(SGameID idSGame);  // line 112
    static SGame & GameGet(SGameID idSGame);  // line 161
    static SGame * GameTryToGet(SGameID idSGame);  // line 155
    static BOOL GameIsValid(SGameID idSGame);  // line 190
    static SGame & GameGet();
    static void SocketMarkForDelete(ServerSocketID idServerSocket, SocketDisconnectReason eReason);  // line 93
    static SGame * GameIterate(SGameIterator & tIterator);  // X/Server/SGameManager.cpp
    static uint32 GetGameCount();  // line 196
    static SGameID GetCurrentGame();  // line 130
    static SGameID TryToGetCurrentGame();  // line 136
    static BOOL ErrorMgrGetCurrentGameID(uint32 & dwID);  // line 143
    static BOOL IsUpdating(SGameID idSGame);  // line 214
    static bool SignalKillGame(SGameID idSGame);  // X/Server/SGameManager.cpp:893
    static void QueueGameFrameUpdate(uint32 uGameFrameUpdateCount);  // X/Server/SGameManager.cpp:479
    static void UpdateGames(uint32 uTotalGameClockTicks);  // X/Server/SGameManager.cpp:508
    static void AddAccountSignal(SGameID idSGame, GameSignalType eSignal, const OnlineService::GameAccountHandle & tAccount, XVariant && tContext, OnlineServiceServer::ProtoMessageCallback && tCb);  // X/Server/SGameManager.cpp:1025
    static dmWorldId CreateDominoWorld(const dmWorldDef * pWorldDef);  // X/Server/SGameManager.cpp
    static void DestroyDominoWorld(dmWorldId id);  // X/Server/SGameManager.cpp
    static void ClearAccountSGameId(const OnlineService::GameAccountHandle & tGameAccountId);  // X/Server/SGameManager.cpp:1069
    static SGameID GameAllocate();
    static void GameDelete(SGameID idSGame);
    static float GetAverageWorkerThreadHz();
    static bool SignalClearSaveInProgress(SGameID idSGame);
    void ResumeWorkerThreads();
private:
    SGameManager();
    ~SGameManager();
    void _UpdateMaxGames();
    static uint _ThreadProc(void * ptParam);
    void _UpdateGames();
    void _UpdateGame(SGameID idSGame);
    void _ClearSimCompleteList();
    void _ScheduleGamesForSimulation();
    void _QueueSimGame(SGame * ptSGame);
    SGameID _DequeueSimGame();
    void _UpdateShutdownTimer();
    void _UpdateDelayTimer();
    void _UpdateDrainTimer();
    void _UpdateSeasonShutdownTimer();
    void _UpdateCachedSeasonShutdownTime();
    BOOL _GameRequiresShutdownMessage(SGame * ptSGame);
    void _ProcessSocketDeleteList();
    void _SetGameSignalBits(SGame & tSGame, uint32 uSignalBits);
    static SGameManager * s_ptGameManager;
    TDataArray<SGame, int> * m_ptSGames;  // 0x00
    uint32 m_uMaxGames;  // 0x08
    uint32 m_uGameFrameUpdateCount;  // 0x0c
    uint32 m_uLastGameClockTicksWork;  // 0x10
    uint32 m_uWorkerThreadCount;  // 0x14
    SGameThread * m_patWorkerThreads;  // 0x18
    Blizzard::Lock::RMutex m_tDominoWorldLock;  // 0x20
    Blizzard::Lock::RMutex m_tSimPendingLock;  // 0x40
    blz::deque<int, blz::allocator<int> > m_tSimPendingQueue;  // 0x60
    struct SimCompleteStruct {  // size 0x8
        void AddGame(SGameID idSGame);
        SGameID m_auGames[1];  // 0x00
        uint32 m_uCount;  // 0x04
    };
    Blizzard::Lock::RMutex m_tSimCompleteLock;  // 0x80
    SGameManager::SimCompleteStruct m_atSimComplete[2];  // 0xa0
    uint32 m_uActiveSimCompleteIndex;  // 0xb0
    Blizzard::Thread::Thread m_tLuaDeadlockThread;  // 0xb8
    LuaDeadlockThreadData m_tLuaDeadlockThreadData;  // 0xc0
    LuaDeadlockGameThreadData m_tMainThread_LuaDeadlockGameThreadData;  // 0xf0
    blz::atomic<bool> m_bWorkerThreadsTerminate;  // 0x104
    blz::semaphore m_tWorkerThreadSemaphore;  // 0x108
    Blizzard::Lock::RMutex m_tSocketDeleteLock;  // 0x130
    blz::list<SocketAndReason, blz::allocator<SocketAndReason> > m_tSocketDeleteList;  // 0x150
    typedef blz::unordered_map<OnlineService::GameAccountHandle, int, blz::hash<OnlineService::GameAccountHandle>, blz::equal_to<OnlineService::GameAccountHandle>, blz::allocator<blz::pair<const OnlineService::GameAccountHandle, int> > > GameAccountToSGameIdMap;
    SGameManager::GameAccountToSGameIdMap m_tGameAccountToSGameIdMap;  // 0x168
    Blizzard::Lock::RMutex m_tGameAccountToSGameIdMapLock;  // 0x188
    typedef blz::unordered_map<OnlineService::ChannelId, int, blz::hash<OnlineService::ChannelId>, blz::equal_to<OnlineService::ChannelId>, blz::allocator<blz::pair<const OnlineService::ChannelId, int> > > PartyIdToSGameIdMap;
    SGameManager::PartyIdToSGameIdMap m_tPartyIdToSGameIdMap;  // 0x1a8
    Blizzard::Time::Second m_lastStats;  // 0x1c8
    Blizzard::Time::UnixTime64 m_nShutdownTime;  // 0x1d0
    Blizzard::Time::UnixTime64 m_nShutdownMessageTime;  // 0x1d8
    Blizzard::Time::UnixTime64 m_nDelayTime;  // 0x1e0
    Blizzard::Time::UnixTime64 m_nDrainTime;  // 0x1e8
    int32 m_nShutdownSecPostDelay;  // 0x1f0
    bool m_bShuttingDown;  // 0x1f4
    Blizzard::Time::UnixTime64 m_nSeasonShutdownTime;  // 0x1f8
    BOOL m_bSeasonShutdownHandled;  // 0x200
    blz::string m_cachedSeasonShutdownDate;  // 0x208
    Blizzard::Time::Microsecond m_uLastMainThreadTick;  // 0x230
    uint32 m_uTotalMainThreadUpdates;  // 0x238
    uint32 m_uTotalGameTicksScheduled;  // 0x23c
    uint32 m_uTotalGamesScheduled;  // 0x240
    blz::vector<unsigned int, blz::allocator<unsigned int> > m_numServicePlayers;  // 0x248
    Blizzard::Time::Second m_lastFetchNumServicePlayersTime;  // 0x260
    int32 m_eTriggerShutdownType;  // 0x264 (ServerShutdownType)
    int32 m_nTriggerShutdownTimeWarning;  // 0x268
    int32 m_eShutdownType;  // 0x26c (ServerShutdownType)
    BOOL m_bTriggerSendBroadcast;  // 0x270
    Blizzard::Time::UnixTime64 m_lastWorkSeconds;  // 0x278
};
static_assert(sizeof(SGameManager) == 0x280, "SGameManager");

// Defined in X/Server/SGameManager.cpp:69 and :81 (scope s_scopeSGameManager).
class XVarUint32;
extern XVarUint32 s_varWorkerThreadCount;
extern XVarUint32 s_varStalledMainThreadThresholdSeconds;

inline BOOL SGameManager::Exists() {  // line 83
    return s_ptGameManager != NULL;
}

inline SGame & SGameManager::GameGet(SGameID idSGame) {  // line 161
    return *s_ptGameManager->m_ptSGames->ElementGet(idSGame);  // line 166
}

inline void SGameManager::SetCurrentGame(SGameID idSGame) {  // line 112
    ThreadGlobals::Get().m_idSGameCurrent = idSGame;  // line 115
    ThreadGlobals::Get().m_ptSGameCurrent = (idSGame != -1) ? &GameGet(idSGame) : NULL;  // line 116
}

// line 130: DWARF records this inline at SGameManager.h:130 (ThreadGlobals::Get inlined at
// SGameManager.h:133), inlined into LuaHook.cpp:67's abort hook.
inline SGameID SGameManager::GetCurrentGame() {
    return ThreadGlobals::Get().m_idSGameCurrent;
}

inline void SGameManager::SocketMarkForDelete(ServerSocketID idServerSocket, SocketDisconnectReason eReason) {  // line 93
    SocketAndReason tSocketAndReason = { idServerSocket, eReason };
    s_ptGameManager->m_tSocketDeleteLock.Enter();
    s_ptGameManager->m_tSocketDeleteList.push_back(tSocketAndReason);
    s_ptGameManager->m_tSocketDeleteLock.Leave();
}

inline SGame & SGameManager::GameGet() {  // line 169
    return *ThreadGlobals::Get().m_ptSGameCurrent;
}

inline uint32 SGameManager::GetGameCount() {  // line 196
    if (s_ptGameManager == NULL || s_ptGameManager->m_ptSGames == NULL)
        return 0;
    return s_ptGameManager->m_ptSGames->GetTotalCount();  // line 199
}

inline SGameID SGameManager::TryToGetCurrentGame() {  // line 136
    ThreadGlobals * ptThreadGlobals = ThreadGlobals::TryToGet();  // line 139
    if (ptThreadGlobals == NULL)
        return SGAMEID_INVALID;
    return ptThreadGlobals->m_idSGameCurrent;
}

inline BOOL SGameManager::ErrorMgrGetCurrentGameID(uint32 & dwID) {  // line 143
    ThreadGlobals * ptThreadGlobals = ThreadGlobals::TryToGet();  // line 146
    if (ptThreadGlobals == NULL)
        return FALSE;
    dwID = ptThreadGlobals->m_idSGameCurrent;  // line 149
    return TRUE;
}

inline SGame * SGameManager::GameTryToGet(SGameID idSGame) {  // line 155
    // No invalid-id check of its own: the original's only one is ElementTryToGet's
    // (DataArraysTemplated.h:593, seen in SignalClearSaveInProgress).
    return s_ptGameManager->m_ptSGames->ElementTryToGet(idSGame);  // line 158
}

// Body written from the copies inlined into X/Server/Network/XServerSocket.cpp
// (XServerSocketDisconnect, sXServerReceive).
inline BOOL SGameManager::GameIsValid(SGameID idSGame) {  // line 190
    return s_ptGameManager->m_ptSGames->ElementTryToGet(idSGame) != NULL;  // line 193
}

inline BOOL SGameManager::IsUpdating(SGameID idSGame) {  // line 214
    SGame * ptSGame = GameTryToGet(idSGame);  // line 217
    return ptSGame && ptSGame->eSimState == SGAME_SIMULATING;  // line 218
}

inline void SGameManager::SimCompleteStruct::AddGame(SGameID idSGame) {  // line 384
    m_auGames[m_uCount] = idSGame;  // line 386
    m_uCount++;  // line 387
}

extern const float BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER1;  // defined X/Server/SGameManager.cpp:119 (DW_AT_external; SGame.cpp loads it through the GOT); header home not in the DWARF
extern const float BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER2;  // defined X/Server/SGameManager.cpp:120
