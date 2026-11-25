#include "../../Common/SNO/SNOName.h"  // X/Client/Console/Online.cpp. SNOName.h/WorldPlace.h/GfxVB.h first: the original static initializer sets SNONAME_NONE, cgplaceNull and cgc*Normal before the nn::ldn statics (DWARF variable order agrees); the original include chain is not known.
#include "../../Common/WorldPlace.h"  // Console online layer: the network thread and its locks, the main-thread event queue, and the
#include "../Gfx/GfxVB.h"  // Console::Online::Party* entry points that wrap the current party (ConsoleParty.cpp).

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdCommonAddr.h"
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdNet.h"
#include "../../../Contrib/Contrib/blz/include/blz/queue.h"
#include "ConsoleParty.h"
#include "Matchmaking.h"
#include "GamerProfile.h"
#include "Online.h"
#include "TaskManager.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../../NintendoSDK/Include/nn/nifm/nifm_Api.h"
#include "../../../Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../Common/OnlineService/OnlineService.h"
#include <stddef.h>
#include <stdint.h>
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryServer.h"
// X/Sigma/ErrorManager/ErrorManagerLogging.h (not in src/ yet).
enum SeverityLevel {
    SLVL_INTERNAL = 0,
    SLVL_DEBUG = 1,
    SLVL_INFO = 2,
    SLVL_WARNING = 3,
    SLVL_CRASH_INFO = 4,
    SLVL_ERROR = 5,
    SLVL_SERVICE_ERROR = 6,
    SLVL_FATAL = 7,
};
enum OutputStreamType {
    OUTPUTSTREAM_DEFAULT = 0,
};
BOOL TraceInternal_ShouldLog(const SeverityLevel eLevel, const uint32 dwFlags, const OutputStreamType eStream);
void TraceInternal_Log(const SeverityLevel eLevel, const uint32 dwFlags, const OutputStreamType eStream, LPCSTR szFormat, ...);
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Api.h"
namespace CHero { void Save(int32 nControllerIndex, uint32 uReason); }  // X/Client/CHero.cpp
namespace Console { namespace Utilities { void ShowSystemOnlineErrorDialog(); } }  // UtilitiesNX64.cpp
namespace Console { namespace Online { void GameCleanupGameConnections(); } }  // ConsoleGameConnection.cpp
#include "ConsoleGameConnection.h"
namespace Blizzard { namespace Process { bool Sleep(Time::Millisecond milliseconds); } } // DWARF Process_NX64.cpp:25; original declaration header unknown.
#include "../../Common/Quests/QuestsSwig.h"
#include "../CPlayer.h"

#include "../UserInterface/Console/ConsoleScreenManager.h"
#include "../UserInterface/Console/ConsoleModalNotificationManager.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../../NintendoSDK/Include/nn/account/account_Types.h"
// Signatures from definitions GamerProfileNX64.cpp:910 and GamerProfileInternal.cpp:533; original declaration locations unknown.
namespace Console { namespace GamerProfile { nn::account::Nickname GetUserNickName(int32); namespace Internal { void HandleUserSignedOut(int32); } } }
// GamerProfile.h:330 abstract DIE; body cannot be added under this pass's header rules.

BOOL SNOPackChunkLoadAct(Act eAct); // DWARF SNOFiles.cpp:11554; declaration owner inferred SNOFiles.h.
namespace Console { namespace Online { void PartyAddPlayer(int32 nControllerIndex, PartyAddPlayerCallback pfnCallback); } }
namespace Console { namespace UIOnlineActions { void SetGameParamsForHero(const D3::Account::SavedDefinition &, const D3::Hero::SavedDefinition &, Online::GameParams &); } } // DWARF definition OnlineActions.cpp:480; declaration owner OnlineActions.h inferred.
namespace Console { namespace GamerProfile { void LoadAccountDevOnly(int32 nUserIdx, PlayerClass ePlayerClass); } } // Definition GamerProfileNX64.cpp:1144; declaration header inferred GamerProfile.h.
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryClient.h"
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdLANDiscoveryListener.h"
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdSocket/bdInetAddr.h"
#include "../../../NintendoSDK/Include/nn/socket/socket_Api.h"
namespace Console { namespace UpdateManager { void ApplyUpdates(); } } // DWARF definition UpdateManager.cpp:430; original declaration header unknown.
#include "../../../Contrib/Contrib/DemonWare/bdSocket/bdSocketRouterAdHoc-switch.h"
namespace Console { namespace SaveMigration { void PerformSeasonMigration(); } }  // X/Client/Console/SaveMigration.cpp
namespace nn { namespace nifm { Result GetCurrentIpConfigInfo(in_addr *, in_addr *, in_addr *, in_addr *, in_addr *); } } // SDK import signature; return type from result local at3165, original declaration header inferred nifm_Api.h.
namespace Console {
namespace Online {
void QueueNetworkEventForMainThreadAlways(OnlineEventCallback pfnCallback); // DWARF definition Online.cpp:288; declared here because QueueNotificationAlways below needs it (original declaration in Online.h, line unknown).
extern PartyRef sptParty;  // ConsoleParty.cpp
template<typename... Args> inline void QueueNotificationAlways(OnlineService::SubscriptionEvent<Args...> & tEvent, Args... args) { QueueNetworkEventForMainThreadAlways(OnlineService::internal::method_binder_1<void, OnlineService::SubscriptionEvent<Args...>, Args...>::template bind<&OnlineService::SubscriptionEvent<Args...>::NotifySubscribers>(&tEvent, args...)); } // DWARF Online.h:1384-1389 (abstract DIE only, DW_INL_inlined, no concrete DIE); body from the inlined tree in sRecheckNetworkNotifyOrRestoreState: Callback<void()> from method_bind_1_0_const<...NotifySubscribers> passed to QueueNetworkEventForMainThreadAlways (original thunk 0x57160). Defined in the .cpp instead of Online.h:1384 to avoid a shared-header edit.
}  // namespace Online
}  // namespace Console
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdGameInfoFactory.h"
#include "../../../Contrib/Contrib/blz/include/blz/func_poly.h"
#include "../../Common/ThreadGlobals.h"
namespace nn { namespace crypto { void GenerateCryptographicallyRandomBytes(void * pOutBuffer, size_t bufferSize); } }  // original call 0x7100054120+0x4c
namespace Console {
namespace Online {
} // namespace Online
namespace UIOnlineActions { BOOL IsJoinPartyInProgress(); BOOL IsJoinAdHocNetworkInProgress(); } // DWARF definitions OnlineActions.cpp:419/427; declaration header inferred OnlineActions.h.
namespace Online {
void sAcceptedPartyInvitation(const UIID &); // Source-owned definition at3366.
void sDeclinedPartyInvitation(const UIID &); // Source-owned abstract DIE at3376.
} } // namespace Console::Online
namespace UIPartyDisplay { namespace Console { void FindLocalCommunicationGames(); } } // DWARF UIConsolePartyDisplay.cpp:1158; declaration header inferred.
namespace UIHeroSelect { namespace Console { void RefreshAdHocList(); } } // DWARF UIConsoleHeroSelect.cpp:836; declaration header inferred.
namespace Console { namespace Online {
} // namespace Online
namespace GamerProfile { int32 GetGuestSponsor(int32); BOOL GetUserPlayRegistrationKey(int32, nn::friends::PlayHistoryRegistrationKey &); } // DWARF GamerProfileNX64.cpp:938/792; original header inferred GamerProfile.h.
namespace SaveMigration { bool PerformSeasonMigration(int32); } // DWARF SaveMigration.cpp:1664; original declaration header inferred SaveMigration.h.
namespace Online {
// Inference: declaration owner inferred from DWARF definition OnlineActions.cpp:433.
} namespace UIOnlineActions { void SetJoinAdHocNetworkInProgress(); } namespace Online {
uint32 PartyGetLocalMemberCount(); // DWARF source-owned definition Online.cpp:4276.
void PartyLeave(); // DWARF source-owned definition Online.cpp:2922.
// DWARF Online.cpp:4217; source-owned declaration needed by the network thread.
void UpdatePartyMembers();




































// DWARF Online.h:141-145, empty class (size 1); declared locally without changing the shared header.
class ScopedNetworkThreadLockSuspend
{
public:
    ScopedNetworkThreadLockSuspend();
    ~ScopedNetworkThreadLockSuspend();
};





enum {
    NETWORK_THREAD_SLEEP_MS = 10,
    GAME_STATE_SYNC_INTERVAL_MS = 1000,
    MAX_WAIT_FOR_LIVE_CONNECTIVITY_MS = 2500,
    NETWORK_AUTOMATIC_CONNECTION_RETRY_DELAY_MS = 300000,
    ADHOC_STATION_ADVERTISE_UPDATE_DELAY = 30000,
    NP_NOTIFY_PLUS_DELAY_MS = 33,
};


// Declared in Online.h in the original.
BOOL PartyIsGameStarted();
BOOL PartyIsInGame();
BOOL PartySwitchWillForceDisconnect(PartyOnlineState eState);
void PartySwitchOnlineState(PartyOnlineState eState);
void ClearOutstandingInvite();
extern PartyList sgtParties;  // ConsoleParty.cpp
extern volatile int32 snGameTag;  // ConsoleParty.cpp




BOOL sbSkipCompatabilityCheck;









uint64 GetTitleVersion()
{
    return 0x115;
}






BOOL CheckVersionCompatability(uint64 uVersion)
{
    return uVersion == GetTitleVersion();
}
}  // namespace Online
}  // namespace Console
#include "../../../Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBuffer.h"
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdGameInfo.h"
namespace Console {
namespace Online {




class D3GameInfo : public bdGameInfo
{
public:
    D3GameInfo()
        : m_uTitleVersion(0)
    {
        SigmaMemoryClear(m_szName, sizeof(m_szName));
    }

    void SetName(const char * szName) { Blizzard::String::Copy(m_szName, szName, sizeof(m_szName)); }
    const char * GetName() const { return m_szName; }

    void SetGameInfo(const GameInfo & tGameInfo) { m_tGameInfo = tGameInfo; }
    const GameInfo & GetGameInfo() const { return m_tGameInfo; }

    GameInfo & GetGameInfo() { return m_tGameInfo; }

    virtual void serialize(bdBitBuffer & tBitBuffer) const

    {
        bdGameInfo::serialize(tBitBuffer);
        tBitBuffer.writeString(m_szName, sizeof(m_szName));
        tBitBuffer.writeUInt64(0x115);
        m_tGameInfo.Serialize(tBitBuffer);
    }

    virtual bdBool deserialize(const bdCommonAddrRef me, bdBitBuffer & tBitBuffer)


    {
        bdBool bOk = bdGameInfo::deserialize(me, tBitBuffer);
        bOk = bOk && tBitBuffer.readString(m_szName, sizeof(m_szName));
        bOk = bOk && tBitBuffer.readUInt64(m_uTitleVersion);

        return bOk && m_uTitleVersion == 0x115 && m_tGameInfo.Deserialize(tBitBuffer);
    }

private:
    char m_szName[260];  // 0x30
    bdUInt64 m_uTitleVersion;  // 0x138
    GameInfo m_tGameInfo;  // 0x140
};

typedef bdReference<D3GameInfo> D3GameInfoRef;



bdLANDiscoveryServer * sptLANDiscoveryServer;


PartyPendingJoinInfo * sptPendingPartyJoinInfo;
PartyPendingJoinInfo * sptInvitePartyJoinInfo;


PartyConnectionInfo sReceivedInviteConnectionInfo;

ModalNotificationId sidInvitationModal = INVALID_MODAL_NOTIFICATION_ID;






Blizzard::Lock::RMutex sgtEventQueueMutex;
blz::queue<blz::function<void ()> > sgtEventQueue;

void QueueNetworkEventForMainThread(OnlineEventCallback pfnCallback)
{
    ThreadGlobals * ptThread = ThreadGlobals::TryToGet();
    if (ptThread && ptThread->m_nThreadIndex == THREADINDEX_MAIN)
    {
        pfnCallback();
    }
    else
    {
        Blizzard::Lock::ScopedRMutex tLock(sgtEventQueueMutex);
        sgtEventQueue.push(blz::function<void ()>(pfnCallback));
    }
}

void QueueNetworkEventForMainThreadAlways(OnlineEventCallback pfnCallback)
{
    Blizzard::Lock::ScopedRMutex tLock(sgtEventQueueMutex);
    sgtEventQueue.push(blz::function<void ()>(pfnCallback));
}

PartyMemberAddedEvent sgtPartyMemberAddedEvent;
PartyMemberReceivedDigestEvent sgtPartyMemberReceivedDigestEvent;
PartyMemberRemovedEvent sgtPartyMemberRemovedEvent;
PartyInviteAcceptedEvent sgtPartyInviteAcceptedEvent;
PartyJoinedEvent sgtPartyJoinedEvent;
PartyGameInfoChangedEvent sgtPartyGameInfoChangedEvent;
PartyMemberVoiceChatTalkingEvent sgtPartyMemberVoiceChatTalkingEvent;
PartyMigratingEvent sgtPartyMigratingEvent;
PartyOnlineStateChangedEvent sgtPartyOnlineStateChangedEvent;
PSNSignInChangedEvent sgtPSNSignInChangedEvent;
PhysicalNetworkStatusChangedEvent sgtPhysicalNetworkStatusChangedEvent;
GameStartingEvent sgtGameStartingEvent;
GameEndingEvent sgtGameEndingEvent;
LobbyConnectionEvent sgtLobbyConnectionEvent;
NewMailEvent sgtNewMailEvent;



PlayTogetherEvent sgtPlayTogetherEvent;
SeasonsChangedEvent sgtSeasonsChangedEvent;

BOOL sbNetworkConnected;

NetworkMode seNetworkMode;
NetworkMode seDesiredNetworkMode;


NetworkMode seNetworkModeChangingTo;

BOOL sbNetworkModeReady;
BOOL sbNetworkModeChanging;
BOOL sbNetDialogPending;
Blizzard::Time::Millisecond suLastNetworkSwitchAttemptMS;



Blizzard::Time::Millisecond suLastAdHocStationStatusUpdateMS;

BOOL sbRequestNewAdHocResults;


NetworkMode GetNetworkMode() { return seNetworkMode; }
BOOL IsNetworkChanging() { return !sbNetworkModeReady || sbNetworkModeChanging; }
BOOL IsNetworkRunning() { return !IsNetworkChanging() && seNetworkMode != NET_OFFLINE; }

void SwitchToNetworkMode(NetworkMode eMode, BOOL bForce)
{
    if (seDesiredNetworkMode != eMode && (bForce || eMode > seNetworkMode))
    {
        int32 nPrimaryUser = GamerProfile::GetPrimaryProfileUserIndex();
        if (nPrimaryUser != -1 && eMode == NET_LAN && !bForce)
        {
            eMode = NET_ONLINE;
        }

        // DWARF: Math::Max<NetworkMode> inlined at 354 (Max(x = seDesiredNetworkMode, y = eMode), cols 40/67); no if at 349.





        seDesiredNetworkMode = bForce ? eMode : Math::Max(seDesiredNetworkMode, eMode);
        sbNetworkModeChanging = TRUE;
        if (seNetworkMode == NET_ADHOC)
        {
            suLastNetworkSwitchAttemptMS = 0;
        }
        else
        {
            suLastNetworkSwitchAttemptMS = Blizzard::Time::Milliseconds();
        }
    }
}






BOOL sbNetworkThreadDone;
BOOL sbForceNetworkThreadTerminate;



BOOL sbNetworkWasAvailableLastFrame;


TaskManager stNetworkTasks;
Blizzard::Thread::Thread stNetworkThread;

Blizzard::Lock::RMutex stNetworkThreadLock;
ScopedNetworkThreadLock::ScopedNetworkThreadLock()
{
    stNetworkThreadLock.Enter();
}

ScopedNetworkThreadLock::~ScopedNetworkThreadLock()
{
    stNetworkThreadLock.Leave();
}

// C2/D2 low_pc 0: standalone bodies uncheckable; original inlined lock suspension is needed by sNetworkThread.
ScopedNetworkThreadLockSuspend::ScopedNetworkThreadLockSuspend()
{
    stNetworkThreadLock.Leave();
}
ScopedNetworkThreadLockSuspend::~ScopedNetworkThreadLockSuspend()
{
    stNetworkThreadLock.Enter();
}

Blizzard::Lock::RMutex stNetworkAdHocOpLock;

ScopedAdHocOperationThreadLock::ScopedAdHocOperationThreadLock()
{
    stNetworkAdHocOpLock.Enter();
}

ScopedAdHocOperationThreadLock::~ScopedAdHocOperationThreadLock()
{
    stNetworkAdHocOpLock.Leave();
}


Blizzard::Lock::RMutex stNetworkSendLock;
ScopedNetworkSendLock::ScopedNetworkSendLock()
    : Blizzard::Lock::ScopedRMutex(stNetworkSendLock)
{
}


















































































































// Online.cpp:536. The task behind DeferNetworkEvent: it waits m_msWaitTime, then runs the callback.
struct DeferredEventTask : public Task
{

    ~DeferredEventTask() {}

    DeferredEventTask(Blizzard::Time::Millisecond msWaitTime, OnlineEventCallback pfnCallback)
        : Task(stNetworkTasks),
          m_msStartTime(Blizzard::Time::Milliseconds()),
          m_msWaitTime(msWaitTime),
          m_pfnCallback(pfnCallback)
    {
    }

    virtual Task::Status GetStatus()
    {
        if (Blizzard::Time::GetTimeElapsed(m_msStartTime, Blizzard::Time::Milliseconds()) >= m_msWaitTime)
        {
            return TASK_DONE;
        }

        return TASK_IN_PROGRESS;
    }

    virtual void Start() {}

    virtual void OnComplete()
    {
        m_pfnCallback();
    }

    Blizzard::Time::Millisecond m_msStartTime;
    Blizzard::Time::Millisecond m_msWaitTime;
    OnlineEventCallback m_pfnCallback;
};







static void DeferNetworkEvent(uint32 dwMilliseconds, OnlineEventCallback pfnCallback)
{
    ::new DeferredEventTask(dwMilliseconds, pfnCallback);
}

void DeferNetworkEventForMainThread(uint32 dwMilliseconds, OnlineEventCallback pfnCallback)
{
    // (585)
    ::new DeferredEventTask(dwMilliseconds, OnlineEventCallback([pfnCallback]() {
        // (588)
        QueueNetworkEventForMainThread(pfnCallback);
    }));
}

}  // namespace Online
}  // namespace Console

BOOL IsSameAddress(bdCommonAddrRef ptAddr1, bdCommonAddrRef ptAddr2)
{
    if (ptAddr1 == ptAddr2)
        return TRUE;
    if (ptAddr1.isNull() || ptAddr2.isNull())
        return FALSE;
    return *ptAddr1 == *ptAddr2;
}


class D3LogSubscriber : public bdLogSubscriber
{
public:
    virtual void publish(const bdLogMessageType type,
                         const bdNChar8 *const channelName,
                         const bdNChar8 *const file,
                         const bdNChar8 *const function,
                         const bdUInt line,
                         const bdNChar8 *const msg) const
    {
        const bdNChar8 * filename = file;














        SeverityLevel eLevel = type == BD_LOG_WARNING ? SLVL_WARNING : (type == BD_LOG_ERROR ? SLVL_ERROR : SLVL_INFO);















        if (TraceInternal_ShouldLog(eLevel, 3, OUTPUTSTREAM_DEFAULT))
        {



            TraceInternal_Log(eLevel, 3, OUTPUTSTREAM_DEFAULT, "[BD] %s(%d): %s\n", filename, line, msg);
        }







    }
};
#include "LobbyService.h"
#include "MailService.h"
#include "PresenceManager.h"
#include "ConsoleGameConnection.h"
#include "../../Sigma/SigmaProf.h"
#include "../../../Contrib/Contrib/DemonWare/bdNet/bdLANDiscovery/bdGameInfoFactory.h"
void ConfigureBDNetworkThread(Blizzard::Thread::Thread ptThread);  // X/Common/Shell/MainNX64.cpp
namespace Console { namespace Online { BOOL LobbyIsConnected(int32 nUserIndex); uint32 sNetworkThread(void * pUnused); void sEventSeasonsChanged(); } }


static void sLobbyConnectedEvent(int32 nUserIndex)
{


    Console::Online::QueueNotification(Console::Online::sgtLobbyConnectionEvent, nUserIndex, Console::Online::LOBBY_EVENT_CONNECTED);
}

static void sLobbyDisconnectedEvent(int32 nUserIndex)
{


    Console::Online::DeferNetworkEvent(5000,
        [nUserIndex]()
        {
            if (!Console::Online::LobbyIsConnected(nUserIndex))
            {
                Console::Online::QueueNotification(Console::Online::sgtLobbyConnectionEvent, nUserIndex, Console::Online::LOBBY_EVENT_DISCONNECTED);
            }
        });
}
namespace Console { namespace Online {
void InitializeForApp()
{

    static D3LogSubscriber stLogSubscriber;
    stLogSubscriber.addChannel("warn");
    stLogSubscriber.addChannel("err");
    stLogSubscriber.addChannel("info");


    bdGameInfoFactory::getInstance()->setClass(new bdCreator<D3GameInfo, bdGameInfo>());




    GameConnection::InitializeForApp();

    LobbyService::Create(0x1671);
    MailService::Create();
    Matchmaker::Create();
    PresenceManager::Create();




    for (int32 i = 0; i < 4; ++i)
    {
        LobbyService::Get(i)->GetConnectEvent().Subscribe(sLobbyConnectedEvent);
        LobbyService::Get(i)->GetDisconnectEvent().Subscribe(sLobbyDisconnectedEvent);
    }

    sgtSeasonsChangedEvent.Subscribe(sEventSeasonsChanged);






































































































    sbNetworkConnected = FALSE;


    Blizzard::Thread::Create(sNetworkThread, NULL, &stNetworkThread, "BDNetwork", 1, 0x10000);


    ConfigureBDNetworkThread(stNetworkThread);

    SigmaProfRegisterThread(stNetworkThread, "BDNetwork", 0x1000);
    Blizzard::Thread::Resume(stNetworkThread);
}



// Inference: shutdown scopes and branch spelling follow original DWARF ranges and ordered calls.
void DisposeForApp()

{
    {
        ScopedNetworkThreadLock tLock;

        if (sptParty)
        {
            PartyLeave();
        }

        GameCleanupGameConnections();

        // Inference: copied range-for PartyRef and state exclusions follow original refcounts and comparisons.
        for (PartyRef ptParty : sgtParties)
        {
            if (ptParty->GetConnectionState() != Party::PARTY_NOT_CONNECTED && ptParty->GetConnectionState() != Party::PARTY_SHUTTING_DOWN)
            {


                ptParty->Leave();
            }
        }
        // Inference: four controller subscriptions, index0..3, follow the original unrolled calls.
        for (int32 i = 0; i < 4; ++i)
        {
            LobbyService::Get(i)->GetConnectEvent().Unsubscribe(sLobbyConnectedEvent);
            LobbyService::Get(i)->GetDisconnectEvent().Unsubscribe(sLobbyDisconnectedEvent);
        }

        LobbyService::DisconnectAll();
    }

    sgtSeasonsChangedEvent.Unsubscribe(sEventSeasonsChanged);

    // Inference: original stores TRUE before unregistering and waiting for the network thread.
    sbNetworkThreadDone = TRUE;
    SigmaProfUnregisterThread(stNetworkThread);
    Blizzard::Thread::WaitForever(stNetworkThread);
    Blizzard::Thread::Close(stNetworkThread);




    // Inference: original delete and pointer-clear share line886; source macro spelling unavailable.
    delete sptLANDiscoveryServer; sptLANDiscoveryServer = NULL;


    GameConnection::DisposeForApp();







}

















BOOL IsInPlatformParty(OnlineUserId idUser)
{
    return FALSE;
}









































}  // namespace Online
}  // namespace Console
#include "../../Common/Player.h"
#include "../../Common/Quests/Quests.h"




uint32 Console::Online::GameConnection::ms_idSequence;
uint32 Console::Online::GameConnection::ms_dwTotalMessageQueueUsed;
QuestInstance * GetLeadingMainLineQuest();  // X/Common/Quests/Quests.cpp
namespace Console {
namespace Online {

























void Idle()
{
    {
        blz::queue<blz::function<void ()> > tEventQueue;
        {
            // (1008)
            Blizzard::Lock::ScopedRMutex tLock(sgtEventQueueMutex);
            tEventQueue.swap(sgtEventQueue);
        }

        // (1012)
        while (!tEventQueue.empty())
        {
            tEventQueue.front()();
            tEventQueue.pop();
        }
    }

    // (1019)
    static Blizzard::Time::Millisecond suLastGameStateSyncTime = Blizzard::Time::Milliseconds();
    Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();
    Blizzard::Time::Millisecond uTimeSinceLastGameStateSync = Blizzard::Time::GetTimeElapsed(suLastGameStateSyncTime, now);

    // (1023)
    if (!GameIsStartUp() && uTimeSinceLastGameStateSync > 1000)
    {
        ScopedNetworkThreadLock tLock;
        PartyRef ptParty = sptParty;

        // (1030)
        if (ptParty &&
            ptParty->IsGameStarted() &&
            ptParty->IsInGame())
        {
            suLastGameStateSyncTime = now;

            // (1036)
            if (!GameIsWeeklyChallenge())
            {
                Act eAct = GameGetCurrentAct();
                if (eAct != ptParty->m_tGameParams.m_eAct)
                {
                    ptParty->m_tGameParams.m_eAct = eAct;
                    ptParty->MarkGameInfoDirty();
                }

                // (1045)
                QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
                if (ptQuestInstance && ptQuestInstance->snoQuest != -1)
                {
                    if (ptParty->m_tGameParams.m_snoQuest != ptQuestInstance->snoQuest ||
                        ptParty->m_tGameParams.m_nQuestStep != ptQuestInstance->nCurrentStepUID)
                    {
                        ptParty->m_tGameParams.m_snoQuest = ptQuestInstance->snoQuest;
                        ptParty->m_tGameParams.m_nQuestStep = ptQuestInstance->nCurrentStepUID;
                        ptParty->MarkGameInfoDirty();
                    }
                }

                // (1058)
                GameCommonData & tGCD = GameGetActiveGameCommonData();
                int32 nCurrentLevel = tGCD.tGameSyncedData.nMonsterLevel;
                if (nCurrentLevel != ptParty->m_tGameParams.m_nLevel)
                {
                    ptParty->m_tGameParams.m_nLevel = nCurrentLevel;
                    ptParty->MarkGameInfoDirty();
                }
            }

            // (1068)
            for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
            {
                PartyPlayer & tMember = ptParty->m_tPlayers[ptPlayer->nPlayerIndex];
                if (tMember.IsValid())
                {
                    Blizzard::String::Copy(ptPlayer->tAccount.usName, tMember.m_szName, sizeof(ptPlayer->tAccount.usName));
                }
            }
        }
    }

    // (1083)
    uint32 fPlayersInGame = 0;
    if (!GameIsStartUp())
    {
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            fPlayersInGame |= 1 << ptPlayer->nPlayerIndex;
        }
    }

    // (1091)
    PartyRef ptParty = sptParty;
    if (ptParty)
    {
        ptParty->m_fPlayersInGame = fPlayersInGame;
    }

    // (1098)
    if (GameIsStartUp() || GameIsWeeklyChallenge())
    {
        MailSetGameState(false, false, false, 0);
    }
    else
    {
        Player * ptPlayer = PlayerGetByControllerIndex(GamerProfile::GetPrimaryProfileUserIndex());
        BOOL bIsHardcore = ptPlayer ? (ptPlayer->tHeroStateData.uPlayerFlags & 1) : FALSE;
        MailSetGameState(ptPlayer != NULL, bIsHardcore, GameIsSeasonal(), GameGetCurrentSeason());
    }
}






void sRecheckNetworkNotifyOrRestoreState(
    PartyOnlineState eState)
{
    if (!sbNetworkConnected)
    {
        QueueNotificationAlways(sgtPhysicalNetworkStatusChangedEvent, PHYSICAL_NETWORK_STATUS_NOT_CONNECTED);
    }
}













































































void sForceSubmitNetworkRequest()
{

    ScopedNetworkThreadLock tLock;

    if (nn::nifm::IsNetworkRequestOnHold())
    {
        nn::nifm::CancelNetworkRequest();
        nn::nifm::SubmitNetworkRequestAndWait();
    }

    Utilities::ShowSystemOnlineErrorDialog();
}






void sUpdateNetworkConnectionStatusForSwitch()
{

    BOOL bWasConnected = sbNetworkConnected;

    if (!nn::nifm::IsNetworkRequestOnHold())
    {

        ScopedNetworkThreadLock tLock;

        if (nn::nifm::IsNetworkAvailable())
        {
            sbNetworkConnected = TRUE;
        }
        else
        {
            sbNetworkConnected = FALSE;
            nn::nifm::SubmitNetworkRequest();
        }
    }


    if (bWasConnected != sbNetworkConnected)
    {
        ScopedNetworkThreadLock tLock;

        if (sbNetworkConnected)
        {
            QueueNotificationAlways(sgtPhysicalNetworkStatusChangedEvent, PHYSICAL_NETWORK_STATUS_CONNECTED);
        }
        else if (PartyIsInParty())
        {
            DeferNetworkEvent(3000,
                OnlineService::internal::function_binder_1<void, PartyOnlineState>::bind<&sRecheckNetworkNotifyOrRestoreState>(PartyGetOnlineState()));
        }
    }
}


BOOL IsPhysicalNetworkConnected()
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }

    return sbNetworkConnected;
}




BOOL sStartNetwork()

{



    if (seDesiredNetworkMode != NET_ADHOC && !IsPhysicalNetworkConnected())
    {
        return FALSE;
    }












    bdNetStartParams tNetParams;
    tNetParams.m_onlineGame = seDesiredNetworkMode == NET_ONLINE;






















































































































    in_addr addr;
    nn::Result result = nn::nifm::GetCurrentPrimaryIpAddress(&addr);

    if (result.IsSuccess())
    {
        tNetParams.m_localAddresses.pushBack(bdInetAddr(nn::socket::InetNtoa(addr)));
    }
    else
    {
        tNetParams.m_localAddresses.pushBack(bdInetAddr("0.0.0.0"));
    }

    in_addr SubnetMask;
    in_addr DefaultGateway;
    in_addr PreferredDns;
    in_addr alternateDNs;

    nn::nifm::GetCurrentIpConfigInfo(&addr, &SubnetMask, &DefaultGateway, &PreferredDns, &alternateDNs);

    tNetParams.m_UPnPConfig.m_gatewayAddr = bdInetAddr(nn::socket::InetNtoa(DefaultGateway));

    bool useAdHoc = seDesiredNetworkMode == NET_ADHOC;
    if (useAdHoc)
    {


        sbNetworkConnected = FALSE;


        // Inference: setAdHoc is called in both branches; original line table has a call setup at 1443:9 and 1447:9 sharing one call at line 0.
        QueueNotificationAlways(sgtPhysicalNetworkStatusChangedEvent, PHYSICAL_NETWORK_STATUS_NOT_CONNECTED);
        bdPlatformSocket::setAdHoc(useAdHoc);
    }
    else
    {
        bdPlatformSocket::setAdHoc(useAdHoc);
    }





    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
    ptNet->start(tNetParams);

    return TRUE;
}

// Original standalone body was linker-removed (low_pc 0); body uncheckable except its sNetworkThread inline instances.
void sStopNetwork()
{

    // Inference: original delete-and-clear operations share line 1465; original macro spelling unavailable.
    delete sptLANDiscoveryServer; sptLANDiscoveryServer = nullptr;

    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
    if (ptNet->getStatus() != bdNetImpl::BD_NET_STOPPED)
    {

        ptNet->stop();
    }
}
// Inference: branch spellings and empty source spans reconstructed from original line columns, blocks and branch targets. NON_MATCHING: 3996 vs 3988 bytes; at line 1535 the original branches on bNetworkIsRunning first and lays the else-if (1555) block first, ours tests bNetworkAvailable first; swapped else-if operands tried (3992, worse layout).
uint sNetworkThread(void * pUnused)
{


    ThreadGlobals::InitializeForThread();
    SigmaProfInitializeForThread();

    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_NETWORK;

    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();

    while (!sbForceNetworkThreadTerminate &&
        (!sbNetworkThreadDone ||
        !sgtParties.empty() ||
        LobbyService::IsAnyActive()))
    {

        Blizzard::Process::Sleep(NETWORK_THREAD_SLEEP_MS);











        if (seNetworkMode != NET_ADHOC)
        {
            sUpdateNetworkConnectionStatusForSwitch();
        }



        ScopedNetworkThreadLock tLock;





        BOOL bNetworkIsRunning = IsNetworkRunning();
        BOOL bNetworkAvailable = IsPhysicalNetworkConnected();






        // Operand order from the original columns: && at 31, the sbNetworkWasAvailableLastFrame load at 56.
        if (bNetworkAvailable && (bNetworkAvailable != sbNetworkWasAvailableLastFrame))
        {
            suLastNetworkSwitchAttemptMS = 0;
        }
        sbNetworkWasAvailableLastFrame = bNetworkAvailable;



        if (!bNetworkIsRunning &&
            bNetworkAvailable && !IsNetworkChanging())
        {
            // Original line 1539 calls GetPrimaryProfileUserIndex even though nPrimaryUser has no surviving use.
            int32 nPrimaryUser = GamerProfile::GetPrimaryProfileUserIndex();




            {
                Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();
                Blizzard::Time::Millisecond uTimeSinceLastAutomaticConnectAttempt = Blizzard::Time::GetTimeElapsed(suLastNetworkSwitchAttemptMS, now);
                if (suLastNetworkSwitchAttemptMS == 0 ||
                    uTimeSinceLastAutomaticConnectAttempt > NETWORK_AUTOMATIC_CONNECTION_RETRY_DELAY_MS)
                {

                    SwitchToNetworkMode(NET_ONLINE, FALSE);
                }
            }
        }
        else if (bNetworkIsRunning && !bNetworkAvailable)
        {



            // Inference: only an errored adhoc router or a non-adhoc mode reaches the forced offline request.
            if (seNetworkMode == NET_ADHOC)
            {
                bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
                bdSocketRouterAdHoc * RouterAdHoc = ptNet->getSocketRouterAdHoc();

                if (RouterAdHoc->getStatus() == bdSocketRouter::BD_SOCKET_ROUTER_ERROR)
                {
                    SwitchToNetworkMode(NET_OFFLINE, TRUE);
                }
            }
            else
            {

                SwitchToNetworkMode(NET_OFFLINE, TRUE);
            }
        }
        // DWARF locations explicitly record initial FALSE and constant TRUE for these locals.
        BOOL bNetworkModeChangeFailed = FALSE;
        BOOL bCanChangeNetworkMode = TRUE;



        if (!sbNetworkModeReady &&
            seDesiredNetworkMode != seNetworkModeChangingTo && seDesiredNetworkMode == NET_ADHOC)
        {


            sStopNetwork();
        }
        else if (sbNetworkModeReady && seNetworkMode != seDesiredNetworkMode && bCanChangeNetworkMode)
        {





            sbNetworkModeReady = FALSE;
            sbNetworkModeChanging = TRUE;

            seNetworkModeChangingTo = seDesiredNetworkMode;


            if (seDesiredNetworkMode == NET_OFFLINE)
            {
                sStopNetwork();
            }
            else if (seDesiredNetworkMode == NET_ADHOC)
            {



                LobbyService::DisconnectAll();
                sStopNetwork();
                if (!sStartNetwork())
                {
                    seDesiredNetworkMode = NET_OFFLINE;
                    bNetworkModeChangeFailed = TRUE;
                }
            }
            // Inference: existing online-to-LAN operation skips restarting the underlying network.
            else if (!(seNetworkMode == NET_ONLINE && seDesiredNetworkMode == NET_LAN))
            {
                sStopNetwork();
                if (!sStartNetwork())
                {
                    seDesiredNetworkMode = NET_OFFLINE;
                    bNetworkModeChangeFailed = TRUE;
                }
            }
        }







        if (!sbNetworkModeReady &&
            !sbNetDialogPending)
        {
            switch (ptNet->getStatus())
            {
            case bdNetImpl::BD_NET_PARAMS_CONFIG_ERROR:
            case bdNetImpl::BD_NET_BIND_FAILED:
            case bdNetImpl::BD_NET_ONLINE_FAILED:
            case bdNetImpl::BD_NET_INIT_FAILED:

                sStopNetwork();
                seNetworkMode = NET_OFFLINE;

                seDesiredNetworkMode = NET_OFFLINE;
                bNetworkModeChangeFailed = TRUE;


                sbNetworkModeReady = TRUE;
                sbNetworkModeChanging = FALSE;
                break;
            case bdNetImpl::BD_NET_PENDING:
            {



                ScopedNetworkThreadLockSuspend suspend;

                ptNet->pump();
                break;
            }
            case bdNetImpl::BD_NET_STOPPED:
            case bdNetImpl::BD_NET_DONE:






                sbNetworkModeReady = TRUE;
                seNetworkMode = seNetworkModeChangingTo;
                sbNetworkModeChanging = seNetworkMode != seDesiredNetworkMode;















                if (seNetworkMode == NET_ONLINE && !sbNetworkModeChanging)
                {
                    LobbyService::ConnectAll();
                }
                break;
            }


            if (sbNetworkModeReady)
            {

                for (PartyRef ptParty : sgtParties)
                {

                    // Inference: original compares state >=3, then the inclusive ADHOC/LAN range 1..2 against offline mode.
                    if ((ptParty->IsOnline() && seNetworkMode != NET_ONLINE) ||
                        ((ptParty->m_eOnlineState == PARTY_ADHOC || ptParty->m_eOnlineState == PARTY_LAN) && seNetworkMode == NET_OFFLINE))
                    {




                        if (ptParty->m_ptHostConnection)
                        {
                            ptParty->m_ptHostConnection->disconnect();
                        }

                        for (PeerRef ptPeer : ptParty->m_aptPeers)
                        {
                            ptPeer->m_ptConnection->disconnect();
                        }
                    }

                    if (ptParty->IsHost())
                    {



                        for (int32 nPlayerIndex = ptParty->GetFirstLocalPlayerIndex();
                             nPlayerIndex != -1;
                             nPlayerIndex = ptParty->GetNextLocalPlayerIndex(nPlayerIndex))
                        {
                            PartyPlayer & tPlayer = ptParty->m_tPlayers[nPlayerIndex];
                            tPlayer.m_ptAddr = seNetworkMode != NET_OFFLINE ? bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr() : bdCommonAddrRef();
                        }
                    }
                }
            }
        }
        if (ptNet->getStatus() == bdNetImpl::BD_NET_DONE)
        {

            {
                ScopedNetworkThreadLockSuspend suspend;
                ScopedAdHocOperationThreadLock tAdHocLock;

                ptNet->sendAll();
                ptNet->receiveAll();
            }
            ptNet->dispatchAll();


            if (seNetworkMode != NET_ADHOC)
            {


                ScopedNetworkThreadLockSuspend suspend;

                ptNet->getSocketRouter()->getQoSProber()->pump();
            }


            if (sptLANDiscoveryServer)
            {
                if (sptParty &&
                    sptParty->IsHost() &&
                    sptParty->IsLAN())
                {
                    sptLANDiscoveryServer->update();
                }
                else
                {
                    // Inference: original line-zero shared tail deletes and clears the discovery server.
                    delete sptLANDiscoveryServer;
                    sptLANDiscoveryServer = nullptr;
                }
            }
        }
        else
        {

            if (sptLANDiscoveryServer)
            {
                delete sptLANDiscoveryServer;
                sptLANDiscoveryServer = nullptr;
            }
        }


        if (sptPendingPartyJoinInfo &&
            !sbNetworkModeChanging)
        {
            // Inference: online joins need NET_ONLINE; local-communication joins need NET_ADHOC (original select3/1).
            NetworkMode eMinimumMode = sptPendingPartyJoinInfo->m_bIsOnline ? NET_ONLINE : NET_ADHOC;










            // Inference: rejection callback then delete-and-clear share each original cleanup source row; macro spelling unknown.
            if (sptPendingPartyJoinInfo->m_bIsOnline &&
                !GamerProfile::CanPlayOnline(sptPendingPartyJoinInfo->m_nControllerIndex))
            {
                QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, PartyJoinCallback>, const JoinResult &>::bind<&OnlineService::internal::callback_base<1, PartyJoinCallback>::execute>(sptPendingPartyJoinInfo->m_pfnCallback, JOIN_FAILED_NO_PRIVILEGE));
                delete sptPendingPartyJoinInfo; sptPendingPartyJoinInfo = nullptr;
            }
            else if (seNetworkMode >= eMinimumMode)
            {
                PartyRef ptNewParty = ::new Party(sptPendingPartyJoinInfo->m_nControllerIndex);
                ptNewParty->Join(*sptPendingPartyJoinInfo);
                delete sptPendingPartyJoinInfo; sptPendingPartyJoinInfo = nullptr;
            }
            else if (bNetworkModeChangeFailed)
            {
                // Inference: original bound result3 is JOIN_FAILED_UNABLE_TO_CONNECT.
                QueueNetworkEventForMainThread(OnlineService::internal::method_binder_1<void, OnlineService::internal::callback_base<1, PartyJoinCallback>, const JoinResult &>::bind<&OnlineService::internal::callback_base<1, PartyJoinCallback>::execute>(sptPendingPartyJoinInfo->m_pfnCallback, JOIN_FAILED_UNABLE_TO_CONNECT));
                delete sptPendingPartyJoinInfo; sptPendingPartyJoinInfo = nullptr;
            }
            else
            {
                SwitchToNetworkMode(eMinimumMode, FALSE);
            }
        }

        LobbyService::Idle();
        MailService::Idle();
        Matchmaker::Get()->Idle();
        PresenceManager::Idle();




        for (PartyList::iterator it = sgtParties.begin(); it != sgtParties.end();)
        {
            PartyRef ptParty = *it;
            ++it;

            ptParty->Idle();
        }

        UpdatePartyMembers();









































        stNetworkTasks.Process(TaskManager::PROCESS_CAN_START_TASKS);
    }

    {
        ScopedNetworkThreadLock tLock;

        sptParty = nullptr;
        sgtParties.clear();








        sStopNetwork();
    }



    SigmaProfDisposeForThread();
    ThreadGlobals::DisposeForThread();

    return 0;
}









































































































































































































































































































































































void sSynchronizePlayerHeroes()
{

    ScopedNetworkThreadLock tLock;

    if (sptParty && !GameIsStartUp() && !GameIsWeeklyChallenge())
    {
        for (int32 nPlayerIdx = sptParty->GetFirstLocalPlayerIndex(); nPlayerIdx != -1; nPlayerIdx = sptParty->GetNextLocalPlayerIndex(nPlayerIdx))
        {
            PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIdx];
            CHero::Save(tPlayer.m_nControllerIndex, 0x20000);
        }
    }
}








void sEventStartGame(
    uint32 idGameConnection,
    GameParams tGameParams)
{
    ScopedNetworkThreadLock tLock;

    if (!sptParty ||
        GamerProfile::GetPrimaryProfileUserIndex() == -1)
    {
        return;
    }


    UpdateManager::ApplyUpdates();



    if (GameIsStartUp())
    {
        Screens::CloseScreenSet(TRUE, TRUE);
    }

    switch (tGameParams.m_eGameType)
    { case GT_NOTHING:
        GamePlayWorld(tGameParams.m_snoDevWorld, tGameParams.m_dwDevCreationFlags, tGameParams.m_nHandicap, FALSE);
        return;
      case GT_DIABLO3:
      case GT_CHALLENGE_RIFTS: break;
      default: return;
    }



    BOOL bWantTieredRiftHeroArrayForFreeKey = FALSE;
    GameSyncedData & tSyncedData = GameGetActiveGameCommonData().tGameSyncedData;

    uint32 dwGameFlags = 0;
    if (tGameParams.m_eGameType == GT_DIABLO3)
    {
        if (tGameParams.m_bIsHardcore) dwGameFlags |= 1 << GAMECREATEPARAM_HARDCORE;
        if (tGameParams.m_bIsSeasonal) dwGameFlags |= 1 << GAMECREATEPARAM_SEASON;
        if (tGameParams.unk_14) dwGameFlags |= 1u << 20;

        BOOL bHasOpenRift = (tSyncedData.nOpenLootRunLevel == -1 || tSyncedData.nOpenLootRunLevel > 0) &&
            tSyncedData.bLootRunOpen &&
            !tSyncedData.bLootRunBossDead;

        BOOL bHasOpenLootRunChallenge = tSyncedData.nOpenLootRunLevel == 0 &&
            tSyncedData.bLootRunOpen && !tSyncedData.bLootRunChallengeCompleted;


        if (!GameIsStartUp() &&
            PartyIsHost() &&
            (bHasOpenRift || bHasOpenLootRunChallenge))
        {
            if (tSyncedData.nOpenLootRunLevel >= 0)
            {
                bWantTieredRiftHeroArrayForFreeKey = TRUE;
            }
            else
            {
                dwGameFlags |= 1 << GAMECREATEPARAM_AUTO_RIFT;
            }
        }
    }



    GameStartConsoleClient(idGameConnection, sptParty->IsHost(), tGameParams.m_eGameType,
        dwGameFlags, tGameParams.m_bResumeFromSave, tGameParams.m_eAct,
        tGameParams.m_ePreferredAdventureAct, tGameParams.m_snoQuest, tGameParams.m_nQuestStep,
        tGameParams.m_nHandicap, tGameParams.m_uEntranceGUIDOverride,
        bWantTieredRiftHeroArrayForFreeKey ? tSyncedData.nOpenLootRunLevel : -1,
        bWantTieredRiftHeroArrayForFreeKey ? tSyncedData.arTieredRiftParticipatingHeroID : NULL);






}


void PartyEndGame(); // Raw records only this CPP at 3951; same-file definition below, no recovered header declaration.

void sEventEndGame()

{
    ScopedNetworkThreadLock tLock;


    if (!GameIsStartUp() &&
        PlayerGetLocalPlayerCount() == 0 &&
        sptParty)
    {
        PartyEndGame();
        GameStartMenu(ACT_5);
    }

    sgtGameEndingEvent.NotifySubscribers();
}




void sEventSeasonsChanged()
{
    if (GameIsStartUp() && !GameIsAGameStartQueued())
    {
        SaveMigration::PerformSeasonMigration();
    }
}







bdNATType GetNATType()
{
    bdNATType eNATType;
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
    eNATType = me->getNATType();
    return eNATType;
}





















































































































































BOOL HasOutstandingInvite(int32 & nUserIdx)
{
    if (sptInvitePartyJoinInfo)
    {
        nUserIdx = sptInvitePartyJoinInfo->m_nControllerIndex;
        return TRUE;
    }

    return FALSE;
}



























BOOL IsInviteToCurrentParty()
{
    ScopedNetworkThreadLock tLock;
    PartyRef ptParty = sptParty;
    return sptInvitePartyJoinInfo && sptInvitePartyJoinInfo->m_ptAddress && ptParty && ptParty->IsSameParty(*sptInvitePartyJoinInfo);
}








void ClearOutstandingInvite()
{
    delete sptInvitePartyJoinInfo;
    sptInvitePartyJoinInfo = NULL;
}




GameParams::GameParams()
    : m_snoDevWorld(-1),
      m_dwDevCreationFlags(0),
      m_eGameType(1),
      m_bIsHardcore(FALSE),
      m_bIsSeasonal(FALSE),
      unk_14(FALSE),
      m_bResumeFromSave(FALSE),
      m_uSeason(0),
      m_nLevel(0),
      m_eAct(0),
      m_snoQuest(-1),
      m_nQuestStep(-1),
      m_nHandicap(0),
      m_nTag(0),
      m_uEntranceGUIDOverride((uint32)-1),
      m_ePreferredAdventureAct(0)
{
}




void GameParams::Serialize(bdBitBufferRef ptBitBuffer) const
{
    ptBitBuffer->writeInt32(m_eGameType);
    if (m_eGameType == GT_NOTHING)
    {
        ptBitBuffer->writeInt32(m_snoDevWorld);
        ptBitBuffer->writeUInt32(m_dwDevCreationFlags);
    }
    else
    {
        ptBitBuffer->writeBool(m_bIsHardcore);
        ptBitBuffer->writeBool(m_bResumeFromSave);
        ptBitBuffer->writeRangedInt32(m_eAct, ACT_INVALID, ACT_MAX_VALUE);
        ptBitBuffer->writeInt32(m_snoQuest);
        ptBitBuffer->writeInt32(m_nQuestStep);
        ptBitBuffer->writeInt32(m_nHandicap);
        ptBitBuffer->writeInt32(m_nTag);
        ptBitBuffer->writeUInt32(m_uEntranceGUIDOverride);
        ptBitBuffer->writeRangedInt32(m_ePreferredAdventureAct, ACT_INVALID, ACT_MAX_VALUE);
        ptBitBuffer->writeUInt32(m_bIsSeasonal ? m_uSeason : 0);

        if (m_eGameType == GT_CHALLENGE_RIFTS)
        {
            ptBitBuffer->writeUInt32(m_tWeeklyChallengeConfig.challenge_number());
            ptBitBuffer->writeInt64(m_tWeeklyChallengeConfig.challenge_end_unix_time_console());
        }
        ptBitBuffer->writeBool(unk_14);
    }
}




bdBool GameParams::Deserialize(bdBitBufferRef ptBitBuffer)
{
    int32 nGameType;
    bdBool bOk = ptBitBuffer->readInt32(nGameType);

    if (bOk)
    {
        m_eGameType = nGameType;
    }

    if (bOk && m_eGameType == GT_NOTHING)
    {

        bdInt32 snoDevWorld = 0;
        bOk = bOk && ptBitBuffer->readInt32(snoDevWorld)
            && ptBitBuffer->readUInt32(m_dwDevCreationFlags);
        m_snoDevWorld = snoDevWorld;

        m_bIsHardcore = m_bIsSeasonal = unk_14 = m_bResumeFromSave = m_uSeason = 0;

        m_eAct = ACT_1;
        m_snoQuest = SNO_NONE;

        m_nTag = OnlineService::GAME_TAG_ID_DEFAULT;
        m_ePreferredAdventureAct = ACT_1;
        m_tWeeklyChallengeConfig.Clear();
    }
    else
    {
        bdBool bIsHardcore = false;
        bdBool bResumeFromSave = false;
        bdBool bUnk14 = false;
        bdInt32 eAct = ACT_1;
        bdInt32 snoQuest = 0;
        bdInt32 ePreferredAdventureAct = ACT_1;

        bOk = bOk && ptBitBuffer->readBool(bIsHardcore)
            && ptBitBuffer->readBool(bResumeFromSave)
            && ptBitBuffer->readRangedInt32(eAct, ACT_INVALID, ACT_MAX_VALUE)
            && ptBitBuffer->readInt32(snoQuest)
            && ptBitBuffer->readInt32(m_nQuestStep)
            && ptBitBuffer->readInt32(m_nHandicap)
            && ptBitBuffer->readInt32(m_nTag)
            && ptBitBuffer->readUInt32(m_uEntranceGUIDOverride)
            && ptBitBuffer->readRangedInt32(ePreferredAdventureAct, ACT_INVALID, ACT_MAX_VALUE)
            && ptBitBuffer->readUInt32(m_uSeason);

        m_tWeeklyChallengeConfig.Clear();
        if (m_eGameType == GT_CHALLENGE_RIFTS)
        {
            bdUInt32 uChallengeNumber = 0;
            bdInt64 nChallengeEndUnixTime = 0;

            bOk = bOk && ptBitBuffer->readUInt32(uChallengeNumber)
                && ptBitBuffer->readInt64(nChallengeEndUnixTime);

            m_tWeeklyChallengeConfig.set_challenge_number(uChallengeNumber);
            m_tWeeklyChallengeConfig.set_challenge_end_unix_time_console(nChallengeEndUnixTime);
        }

        bOk = bOk && ptBitBuffer->readBool(bUnk14);
        unk_14 = bUnk14;
        m_bIsHardcore = bIsHardcore;
        m_bIsSeasonal = m_uSeason != 0;
        m_bResumeFromSave = bResumeFromSave;
        m_eAct = eAct;
        m_snoQuest = snoQuest;
        m_ePreferredAdventureAct = ePreferredAdventureAct;
    }

    return bOk;
}




void GameInfo::Serialize(bdBitBuffer & tBitBuffer) const
{
    tBitBuffer.writeString(m_szUsername, sizeof(m_szUsername));
    GamePresence::Serialize(tBitBuffer);
    for (uint32 i = 0; i < m_uNumPlayers; ++i)
    {
        const Player & tPlayer = m_tPlayers[i];
        tBitBuffer.writeFull(tPlayer.m_idUser);
        tPlayer.m_tUserPresence.Serialize(tBitBuffer);
    }
}





bdBool GameInfo::Deserialize(bdBitBuffer & tBitBuffer)
{
    bdBool bOk = tBitBuffer.readString(m_szUsername, sizeof(m_szUsername));
    bOk = bOk && GamePresence::Deserialize(tBitBuffer);
    for (uint32 i = 0; bOk && i < m_uNumPlayers; ++i)
    {
        Player & tPlayer = m_tPlayers[i];
        bOk = tBitBuffer.readFull(tPlayer.m_idUser) && tPlayer.m_tUserPresence.Deserialize(tBitBuffer);
    }
    return bOk;
}









void GamePresence::Serialize(bdBitBuffer & tBitBuffer) const
{
    tBitBuffer.writeRangedInt32(m_eOnlineState, PARTY_OFFLINE, NUM_PARTY_ONLINE_STATES);
    tBitBuffer.writeBool(m_bInGame);
    tBitBuffer.writeBool(m_bIsChallenge);
    tBitBuffer.writeBool(m_bIsHardcore);
    tBitBuffer.writeRangedInt32(m_eAct, -1, 3000);
    tBitBuffer.writeInt32(m_snoQuest);
    tBitBuffer.writeRangedUInt32(m_uNumPlayers, 0, 4, true);
    tBitBuffer.writeRangedInt32(m_nHandicap, -1, HC_NUM_HANDICAP_LEVELS);
    tBitBuffer.writeRangedUInt32(m_bIsSeasonal ? m_uSeason : 0, 0, 1024, true);
    tBitBuffer.writeBool(unk_14);
}




bdBool GamePresence::Deserialize(bdBitBuffer & tBitBuffer)
{
    bdInt32 eOnlineState = 0;
    bdBool bInGame = false;
    bdBool bIsChallenge = false;
    bdBool bIsHardcore = false;
    bdBool bUnk14 = false;
    bdInt32 eAct = 0;
    bdInt32 snoQuest = 0;
    bdInt32 nHandicap = 0;
    bdUInt32 uSeason = 0;

    bdBool bOk = tBitBuffer.readRangedInt32(eOnlineState, PARTY_OFFLINE, NUM_PARTY_ONLINE_STATES) &&
        tBitBuffer.readBool(bInGame) &&
        tBitBuffer.readBool(bIsChallenge) &&
        tBitBuffer.readBool(bIsHardcore) &&
        tBitBuffer.readRangedInt32(eAct, -1, 3000) &&
        tBitBuffer.readInt32(snoQuest) &&
        tBitBuffer.readRangedUInt32(m_uNumPlayers, 0, 4, true) &&
        tBitBuffer.readRangedInt32(nHandicap, -1, HC_NUM_HANDICAP_LEVELS) &&
        tBitBuffer.readRangedUInt32(uSeason, 0, 1024, true) &&
        tBitBuffer.readBool(bUnk14);

    m_eOnlineState = eOnlineState;
    m_bInGame = bInGame;
    m_bIsChallenge = bIsChallenge;
    m_bIsHardcore = bIsHardcore;
    m_bIsSeasonal = uSeason != 0;
    unk_14 = bUnk14;
    m_eAct = eAct;
    m_snoQuest = snoQuest;
    m_nHandicap = nHandicap;
    m_uSeason = uSeason;
    return bOk;
}





void UserPresence::Serialize(bdBitBuffer & tBitBuffer) const
{
    tBitBuffer.writeRangedInt32(m_eClass, -1, 7);
    tBitBuffer.writeRangedUInt32(m_uLevel, 0, 70, true);
    tBitBuffer.writeRangedUInt32(m_uParagonLevel, 0, 20000, true);
    tBitBuffer.writeBool(m_bFemale);
    tBitBuffer.writeInt32(m_gbidPortrait);
}




bdBool UserPresence::Deserialize(bdBitBuffer & tBitBuffer)
{
    bdInt32 eClass = 0;
    bdBool bOk = tBitBuffer.readRangedInt32(eClass, -1, 7) &&
        tBitBuffer.readRangedUInt32(m_uLevel, 0, 70, true) &&
        tBitBuffer.readRangedUInt32(m_uParagonLevel, 0, 20000, true) &&
        tBitBuffer.readBool(m_bFemale) &&
        tBitBuffer.readInt32(m_gbidPortrait);
    m_eClass = eClass;
    return bOk;
}







void PartyCreate(
    int32 nControllerIndex,
    GameParams & tGameParams,
    BOOL bOfflineOnly)
{




    ScopedNetworkThreadLock tLock;
    sptParty = ::new Party(nControllerIndex);
    sptParty->Create(tGameParams);

    PartyAddPlayer(nControllerIndex, PartyAddPlayerCallback());

    PartyOnlineState eOnlineState = PARTY_ONLINE_FRIENDSONLY;

    if (nControllerIndex == -1 || bOfflineOnly)
        eOnlineState = PARTY_OFFLINE;
    else if (!GamerProfile::IsSignedIn(nControllerIndex) ||
             !IsPhysicalNetworkConnected())
        eOnlineState = PARTY_OFFLINE;


    else if (!SNOPackChunkLoadAct(ACT_OPEN_WORLD))
        eOnlineState = PARTY_OFFLINE;


    else if (!GamerProfile::CanPlayOnline(nControllerIndex))
        eOnlineState = PARTY_OFFLINE;







    sptParty->SetOnlineState(eOnlineState);
}




void PartyLeave()
{

    ScopedNetworkThreadLock tLock;

    Matchmaker::Get()->UnregisterGame();
    sptParty->Leave();
    GameCleanupGameConnections();
}













void PartyLeaveAndRemake(
    uint32 uFlags)
{


    ScopedNetworkThreadLock tLock;

    if (!sptParty)
    {
        return;
    }

    PartyOnlineState eOnlineState = sptParty->GetOnlineState();


    int32 uPrimaryPlayer = sptParty->GetControllerIndex();
    blzext::fixed_vector<int, 4> auLocalPlayers;

    for (int32 nPlayerIndex = sptParty->GetFirstLocalPlayerIndex();
         nPlayerIndex != -1;
         nPlayerIndex = sptParty->GetNextLocalPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIndex];
        auLocalPlayers.push_back(tPlayer.m_nControllerIndex);
    }

    GameParams tGameParams;

    if (sptParty->m_tGameParams.m_eGameType == GT_DIABLO3)
    {
        tGameParams = sptParty->m_tGameParams;
    }
    else if (sptParty->m_tGameParams.m_eGameType == GT_CHALLENGE_RIFTS &&
        sptParty->IsHost() &&
        GetNetworkTimestamp() < Blizzard::Time::FromUnixTime64(sptParty->m_tGameParams.m_tWeeklyChallengeConfig.challenge_end_unix_time_console()))
    {
        tGameParams = sptParty->m_tGameParams;
        uFlags &= ~1u;
    }
    else
    {
        uFlags |= 1;
    }

    if ((uFlags & 1) &&
        GamerProfile::IsHeroLoaded(uPrimaryPlayer))
    {
        const D3::Account::SavedDefinition & tAccountDefinition = GamerProfile::GetAccount(uPrimaryPlayer);
        const D3::Hero::SavedDefinition & tHeroDefinition = GamerProfile::GetHero(uPrimaryPlayer);
        UIOnlineActions::SetGameParamsForHero(tAccountDefinition, tHeroDefinition, tGameParams);
    }


    PartyLeave();


    PartyCreate(uPrimaryPlayer, tGameParams, TRUE);
    for (bdInt32 nControllerIndex : auLocalPlayers)
    {
        PartyAddPlayer(nControllerIndex, PartyAddPlayerCallback());
    }

    if (!(uFlags & 2))
    {
        PartySwitchOnlineState(eOnlineState);
    }
}




class PartyDiscoveryListener : public bdLANDiscoveryListener, public bdLANDiscoveryClient
{
public:
    BD_CLASS_ALLOCATOR(PartyDiscoveryListener);

    PartyDiscoveryListener(PartyInfo * ptParties, uint32 uMaxParties)
        : m_ptParties(ptParties)
        , m_uMaxParties(uMaxParties)
        , m_uFoundParties(0)
    {
        registerListener(this);
    }

    virtual ~PartyDiscoveryListener()
    {
        unregisterListener(this);
    }


    virtual void onDiscovery(bdGameInfoRef ptGameInfo)
    {
        if (m_uFoundParties < m_uMaxParties)
        {
            D3GameInfoRef ptD3GameInfo = reference_cast<D3GameInfo>(ptGameInfo);

            if (!ptD3GameInfo->getHostAddr()->isLoopback())
            {
                PartyInfo * ptPartyInfo = &m_ptParties[m_uFoundParties++];

                ptPartyInfo->m_ptAddress = ptD3GameInfo->getHostAddr();
                ptPartyInfo->m_tSecID = ptD3GameInfo->getSecurityID();
                ptPartyInfo->m_tSecKey = ptD3GameInfo->getSecurityKey();
                Blizzard::String::Copy(ptPartyInfo->m_szName, ptD3GameInfo->GetName(), sizeof(ptPartyInfo->m_szName));
                ptPartyInfo->m_bIsOnline = FALSE;

                ptPartyInfo->m_tGameInfo = ptD3GameInfo->GetGameInfo();
            }
        }
    }

    PartyInfo * m_ptParties;
    uint32 m_uMaxParties;
    uint32 m_uFoundParties;
};
static_assert(sizeof(PartyDiscoveryListener) == 0x60, "PartyDiscoveryListener");



PartyConnectionInfo::PartyConnectionInfo()
    : m_bIsOnline(FALSE)
{
    m_bIsAdHocGame = FALSE;
    SigmaMemoryClear(&m_Network, sizeof(m_Network));
}







PartyConnectionInfo::PartyConnectionInfo(bdCommonAddrRef ptAddress, bdSecurityID tSecID, bdSecurityKey tSecKey, BOOL bIsOnline)
    : m_bIsAdHocGame(FALSE),
      m_ptAddress(ptAddress),
      m_tSecID(tSecID),
      m_tSecKey(tSecKey),
      m_bIsOnline(bIsOnline)
{
}










PartyInfo::PartyInfo()
{
    m_szName[0] = '\0';
}




void PartyConnectionInfo::Serialize(bdBitBufferRef ptBitBuffer) const
{
    m_ptAddress->serialize(ptBitBuffer);
    ptBitBuffer->writeFull(m_tSecID.ab);
    ptBitBuffer->writeFull(m_tSecKey.ab);
}





bdBool PartyConnectionInfo::Deserialize(bdBitBufferRef ptBitBuffer)
{
    bdCommonAddrRef me = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();

    m_ptAddress = ::new bdCommonAddr();

    bdBool bOk = m_ptAddress->deserialize(me, ptBitBuffer);
    bOk = bOk && ptBitBuffer->readFull(m_tSecID.ab);
    bOk = bOk && ptBitBuffer->readFull(m_tSecKey.ab);
    m_bIsOnline = TRUE;
    return bOk;
}






BOOL PartyConnectionInfo::IsSameParty(const PartyConnectionInfo & tOtherConnection) const
{
    BOOL bSame = IsSameAddress(m_ptAddress, tOtherConnection.m_ptAddress);
    bSame = bSame &&
        bdMemcmp(m_tSecID.ab, tOtherConnection.m_tSecID.ab, sizeof(m_tSecID.ab)) == 0 &&
        bdMemcmp(m_tSecKey.ab, tOtherConnection.m_tSecKey.ab, sizeof(m_tSecID.ab)) == 0;
    return bSame;
}






uint32 PartyFind(
    int32 nControllerIndex,
    PartyInfo * ptParties,
    uint32 uMaxParties)
{



    uint32 NumFoundParties = 0;

    PartyDiscoveryListener tPartyDiscoveryClient(ptParties, uMaxParties);

    bdInetAddr tBroadcastAddress = bdInetAddr::Broadcast();


    in_addr inAddrIpAddress;
    in_addr inAddrSubnetMask;
    in_addr inAddrDefaultGateway;
    in_addr inAddrPreferredDns;
    in_addr inAddrAlternateDns;
    nn::Result result = nn::nifm::GetCurrentIpConfigInfo(&inAddrIpAddress, &inAddrSubnetMask, &inAddrDefaultGateway, &inAddrPreferredDns, &inAddrAlternateDns);








    uint32_t ipAddressAsUINT = nn::socket::InetNtohl(inAddrIpAddress.s_addr);
    uint32_t subnetMaskAsUINT = nn::socket::InetNtohl(inAddrSubnetMask.s_addr);

    uint32 broadcastAddressAsUInt = ipAddressAsUINT | ~subnetMaskAsUINT;

    tBroadcastAddress = bdInetAddr(nn::socket::InetHtonl(broadcastAddressAsUInt));


    bdBool bOk = tPartyDiscoveryClient.discover(0x1671, 1.0f, tBroadcastAddress, 50000);

    while (bOk && tPartyDiscoveryClient.getStatus() == bdLANDiscoveryClient::BD_PENDING)
    {
        Blizzard::Process::Sleep(NETWORK_THREAD_SLEEP_MS);
        tPartyDiscoveryClient.update();
    }

    NumFoundParties = tPartyDiscoveryClient.m_uFoundParties;




    return NumFoundParties;
}


void PartyFindAdHoc(blz::list<PartyInfo> & tFindResult, uint32 uMaxParties)
{


    if (seNetworkMode == NET_ADHOC)
    {


        bdNetImpl * const ptNet = bdSingleton<bdNetImpl>::getInstance();
        bdSocketRouterAdHoc * RouterAdHoc = ptNet->getSocketRouterAdHoc();

        bdAddrHandleRef HostHandle;
        bool SuccessfullyJoinedNetwork;
        if (RouterAdHoc)
        {

            ScopedAdHocOperationThreadLock tAdHocLock;

            nn::ldn::NetworkInfo tNetworks[24];
            int32 nScanResultCount;
            nn::ldn::ScanFilter filter = {};
            filter.networkId.intentId.localCommunicationId = 0x01001b300b9be000ULL;
            filter.flag = nn::ldn::ScanFilterFlag_LocalCommunicationId;

            RouterAdHoc->scan(tNetworks, 24, nScanResultCount, filter, 0);

            if (nScanResultCount > 0)
            {
                for (int32 scanIdx = 0; scanIdx < nScanResultCount && scanIdx < uMaxParties; ++scanIdx)
                {
                    tFindResult.push_back();
                    PartyInfo & tPartyInfo = tFindResult.back();

                    tPartyInfo.m_bIsAdHocGame = TRUE;
                    memcpy(&tPartyInfo.m_Network, &tNetworks[scanIdx], sizeof(nn::ldn::NetworkInfo));


                    bdSocketRouterAdHoc::bdAdvertiseData * advertisedata = reinterpret_cast<bdSocketRouterAdHoc::bdAdvertiseData *>(tNetworks[scanIdx].ldn.advertiseData);


                    SwitchAdhocCustomData * customData = reinterpret_cast<SwitchAdhocCustomData *>(advertisedata->m_customData);
                    tPartyInfo.m_tSecID = advertisedata->m_secID;
                    tPartyInfo.m_tSecKey = advertisedata->m_secKey;

                    SigmaMemoryCopy(tPartyInfo.m_szName, tNetworks[scanIdx].ldn.nodes[0].userName, sizeof(tPartyInfo.m_szName));
                    SigmaMemoryCopy(&tPartyInfo.m_tGameInfo, &customData->m_gameInfo, sizeof(GameInfo));
                    tPartyInfo.m_UniqueID = customData->m_uniqueID;
                }
            }
        }
    }
}

void RefreshAdHocList()
{
    sbRequestNewAdHocResults = TRUE;
}

BOOL ConnectToPartyAdHoc(PartyConnectionInfo & tConnectionInfo)
{
    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
    bdSocketRouterAdHoc * RouterAdHoc = ptNet->getSocketRouterAdHoc();


    if (RouterAdHoc)
    {

        ScopedAdHocOperationThreadLock tAdHocLock;

        RouterAdHoc->leaveNetwork();

        static const uint8_t passphrase[16] = { 0xce, 0x83, 0x3a, 0x55, 0xd6, 0x92, 0xca, 0x7d, 0x57, 0x88, 0x7e, 0x2e, 0x2f, 0xce, 0x7a, 0xb7 };





        nn::ldn::SecurityConfig security = {};

        security.securityMode = nn::ldn::SecurityMode_Debug;
        SigmaMemoryCopy(security.passphrase, passphrase, sizeof(passphrase));
        security.passphraseSize = sizeof(passphrase);

        nn::ldn::UserConfig user = {};
        nn::account::Nickname NickName = GamerProfile::GetUserNickName(GamerProfile::GetPrimaryProfileUserIndex());
        strncpy(user.userName, NickName.name, 32);

        bdAddrHandleRef HostHandle;



        bool hideErrors = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);
        BOOL SuccessfullyJoinedNetwork = RouterAdHoc->connectNetwork(tConnectionInfo.m_Network, security, user, HostHandle, 0, false, hideErrors);

        if (SuccessfullyJoinedNetwork)
        {

            bdCommonAddrRef ptLocalAddr = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
            for (int32 nPlayerIdx = sptParty->GetFirstLocalPlayerIndex(); nPlayerIdx != -1;

                 nPlayerIdx = sptParty->GetNextLocalPlayerIndex(nPlayerIdx))
            {
                PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIdx];
                tPlayer.m_ptAddr = ptLocalAddr;
            }


            bdCommonAddrRef HostAddr;
            bdSecurityID secId;
            RouterAdHoc->getAddressMap()->addrToCommonAddr(HostHandle, HostAddr, secId);

            tConnectionInfo.m_ptAddress = HostAddr;

            return TRUE;
        }
    }


    return FALSE;
}


void sReceivedPartyInvitation(
    blz::string szSenderName,
    PartyConnectionInfo tConnectionInfo,
    int32 nUserIndex)
{
    if (sidInvitationModal != INVALID_MODAL_NOTIFICATION_ID)
    {
        return;
    }



    if (UIOnlineActions::IsJoinPartyInProgress())
    {
        if (sReceivedInviteConnectionInfo.IsSameParty(tConnectionInfo))
        {
            return;
        }
    }


    {
        ScopedNetworkThreadLock tLock;
        if (sptParty)
        {
            if (sptParty->IsSameParty(tConnectionInfo))
            {
                return;
            }
        }
    }



    NotificationCreationParams tParams;
    tParams.nControllerIndex = nUserIndex;
    tParams.uszMessage = LocalizedSprintf(StringListFetch("Console:GameInviteBodyNX64").str(), szSenderName.c_str());
    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sAcceptedPartyInvitation>();
    tParams.tCallbacks.pfnOnClickCancelButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sDeclinedPartyInvitation>();

    sidInvitationModal = Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    sReceivedInviteConnectionInfo = tConnectionInfo;

}

void sAcceptedPartyInvitation(
    const UIID &)
{
    delete sptInvitePartyJoinInfo; sptInvitePartyJoinInfo = NULL;

    sptInvitePartyJoinInfo = new PartyPendingJoinInfo(sReceivedInviteConnectionInfo, 0, JOIN_TYPE_INVITE, PartyJoinCallback());
    sidInvitationModal = INVALID_MODAL_NOTIFICATION_ID;
}


void sDeclinedPartyInvitation(
    const UIID &)
{
    sidInvitationModal = INVALID_MODAL_NOTIFICATION_ID;
}

void sHandleMultipleLogon(
    const int32 nUserIndex)
{
    nn::account::Nickname tNickName = GamerProfile::GetUserNickName(nUserIndex);

    int32 nPrimaryUser = GamerProfile::GetPrimaryProfileUserIndex();


    GamerProfile::Internal::HandleUserSignedOut(nUserIndex);

    NotificationCreationParams tParams;


    tParams.nControllerIndex = -1;

    if (nPrimaryUser == nUserIndex)
    {
        tParams.uszMessage = LocalizedSprintf(StringListFetch("ConsoleErrors:DuplicateLogonPrimaryNX64").str(), tNickName.name);
    }
    else
    {
        tParams.uszMessage = LocalizedSprintf(StringListFetch("ConsoleErrors:DuplicateLogonNonPrimaryNX64").str(), tNickName.name, tNickName.name);
    }

    Screens::GetModalNotificationManager().SetOkNotification(tParams);
}









// Inference: failure-only restoration/callback order from binder 0x57330; DWARF low_pc=0 standalone body uncheckable, needed by PartyJoin's real callback.
void sRestorePartyStateOnJoinFailure(
    PartyJoinCallback pfnCallback,
    PartyOnlineState eState,
    JoinResult eResult)
{
    if (eResult != JOIN_SUCCESS)
    {
        PartySwitchOnlineState(eState);
    }

    pfnCallback(eResult);
}





// Inference: branch spelling and early returns reconstructed from the original line table and calls.
void PartyJoin(
    PartyConnectionInfo & tPartyInfo,
    JoinType eJoinType,
    PartyJoinCallback pfnCallback)
{








    ScopedNetworkThreadLock tLock;


    if (!tPartyInfo.m_bIsAdHocGame)
    {
        // Inference: loopback rejects with JOIN_FAILED; same-party returns JOIN_SUCCESS below.
        if (tPartyInfo.m_ptAddress->isLoopback())
        {
            pfnCallback(JOIN_FAILED);
            return;
        }

        if (sptParty &&
            sptParty->IsSameParty(tPartyInfo))
        {
            pfnCallback(JOIN_SUCCESS);
            return;
        }
    }
    // Inference: local-only parties remain intact; host/state tests follow original short-circuit paths.
    if (!PartyIsHost() ||
        (PartyGetMemberCount() != PartyGetLocalMemberCount() &&
         (PartyIsInGame() ||
          tPartyInfo.m_bIsOnline != PartyIsOnline() ||

          PartyGetOnlineState() == PARTY_ADHOC)))
    {


        pfnCallback = OnlineService::internal::function_binder_3<void, PartyJoinCallback, PartyOnlineState, JoinResult>::bind<&sRestorePartyStateOnJoinFailure>(pfnCallback, PartyGetOnlineState());

        // Inference: original flag value 2 is PARTY_REMAKE_DEFER_STATE_CHANGE bit 1 (Online.h:406).
        uint32 uFlags = 2;


        PartyLeaveAndRemake(uFlags);
        // Inference: original loading-screen Act argument is 400; named constant unavailable.
        if (!GameIsStartUp())
        {
            GameStartMenu(400);
        }
    }



    // Inference: ad-hoc connection failure and same-party exits preserve the original callback results.
    if (tPartyInfo.m_bIsAdHocGame)
    {
        UIOnlineActions::SetJoinAdHocNetworkInProgress();
        ConnectToPartyAdHoc(tPartyInfo);

        if (tPartyInfo.m_ptAddress.isNull())
        {
            ClearOutstandingInvite();
            pfnCallback(JOIN_FAILED_UNABLE_TO_CONNECT);
            return;
        }

        if (tPartyInfo.m_ptAddress->isLoopback())
        {
            pfnCallback(JOIN_FAILED);
            return;
        }

        if (sptParty &&
            sptParty->IsSameParty(tPartyInfo))
        {
            pfnCallback(JOIN_SUCCESS);
            return;
        }
    }




    sptPendingPartyJoinInfo = ::new PartyPendingJoinInfo(tPartyInfo, sptParty->GetControllerIndex(), eJoinType, pfnCallback);
    QueueNetworkEventForMainThreadAlways(sSynchronizePlayerHeroes);
}




void PartyMerge(PartyConnectionInfo & tPartyInfo)
{
    sptPendingPartyJoinInfo = new PartyPendingJoinInfo(tPartyInfo, sptParty->GetControllerIndex(), JOIN_TYPE_MERGE, PartyJoinCallback());
}

void PartyJoinCancel()
{

    ScopedNetworkThreadLock tLock;

    for (PartyList::iterator it = sgtParties.begin(); it != sgtParties.end(); ++it)
    {
        PartyRef ptParty = *it;
        if (ptParty != sptParty && (ptParty->GetConnectionState() == Party::PARTY_CONNECTING || ptParty->GetConnectionState() == Party::PARTY_NEGOTIATING))
        {
            ptParty->Leave();
        }
    }
}













BOOL GetInviteConnectionInfo(PartyConnectionInfo & tPartyInfo, BOOL & bIsInvite)
{

    ScopedNetworkThreadLock tLock;

    if (sptInvitePartyJoinInfo)
    {
        tPartyInfo = *sptInvitePartyJoinInfo;
        bIsInvite = sptInvitePartyJoinInfo->m_eJoinType == JOIN_TYPE_INVITE;
        ClearOutstandingInvite();
        return TRUE;
    }

    return FALSE;
}





void PartyAddPlayer(
    int32 nControllerIndex,
    PartyAddPlayerCallback pfnCallback)
{
    ScopedNetworkThreadLock tNetworkLock;



    for (int32 nPlayerIndex = sptParty->GetFirstLocalPlayerIndex(); nPlayerIndex != -1; nPlayerIndex = sptParty->GetNextLocalPlayerIndex(nPlayerIndex))
    {
        PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIndex];
        if (tPlayer.IsLocal(nControllerIndex))
        {
            if (pfnCallback)
            {
                pfnCallback(TRUE);
            }
            return;
        }
    }



    const char * szPlayerName = GamerProfile::GetGamerTag(nControllerIndex);
    PlayerType ePlayerType = PLAYER_REGULAR;
    int32 nSponsorIndex = 0;
    BOOL bBadReputation = FALSE;

    if (GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
    {
        ePlayerType = PLAYER_PRIMARY;
    }
    else if (GamerProfile::IsGuest(nControllerIndex))
    {
        ePlayerType = PLAYER_GUEST;
        nSponsorIndex = GamerProfile::GetGuestSponsor(nControllerIndex);
    }

    SaveMigration::PerformSeasonMigration(nControllerIndex);

    OnlineUserId idOnlineUser;
    SigmaMemoryClear(&idOnlineUser, sizeof(idOnlineUser));

    AdditionalPlatformInformation tAdditionalPlatformInfo;
    SigmaMemoryClear(&tAdditionalPlatformInfo, sizeof(tAdditionalPlatformInfo));
































    nn::account::NetworkServiceAccountId idUserNSA;
    BOOL bGotNSA = GamerProfile::GetUserNetworkServiceAccountId(nControllerIndex, idUserNSA);
    if (bGotNSA)
    {
        idOnlineUser = idUserNSA.id;
    }

    GamerProfile::GetUserPlayRegistrationKey(nControllerIndex, tAdditionalPlatformInfo.m_tPlayRegistrationKey);


    if (sptParty->IsOnline())
    {

        if (!GamerProfile::CanPlayOnline(nControllerIndex))
        {

            sptParty->SetOnlineState(PARTY_OFFLINE);
        }
    }










    BOOL bHasVoicePrivs = TRUE;

    if (sptParty->IsHost())
    {

        PartyPlayer * ptPlayer = sptParty->AddPlayer(
            bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr(),
            nControllerIndex,
            szPlayerName,
            tAdditionalPlatformInfo,
            idOnlineUser,
            ePlayerType,
            nSponsorIndex,
            bHasVoicePrivs,
            bBadReputation);

        if (!ptPlayer)
        {
            if (pfnCallback)
            {
                pfnCallback(FALSE);
            }
            return;
        }


        ptPlayer->UpdateAccountAndHero();

        if (ptPlayer->IsPrimary())
        {
            sptParty->SetHostPlayer(ptPlayer->m_idPlayer);
        }

        else
        {
            ptPlayer->m_bInGame = sptParty->IsInGame() != FALSE;
        }


        sptParty->SendPartyInfo(NULL);
        sptParty->SendHeroDigest(ptPlayer->m_idPlayer, NULL, NULL);

        if (pfnCallback)
        {
            pfnCallback(TRUE);
        }
    }
    else
    {
        if (!sptParty->m_apfnAddPlayerCallbacks[nControllerIndex])
        {
            sptParty->m_apfnAddPlayerCallbacks[nControllerIndex] = pfnCallback;






            D3PartyMsgAddPlayerRequest tJoinRequest(
                nControllerIndex,
                szPlayerName,
                tAdditionalPlatformInfo,
                idOnlineUser,
                ePlayerType,
                nSponsorIndex,
                bHasVoicePrivs,
                bBadReputation);

            bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_ADDPLAYER_REQ, false);
            tJoinRequest.Serialize(ptMessage->getPayload());
            sptParty->SendToHost(ptMessage);
        }
    }
}



void PartyRemovePlayer(
    int32 nControllerIndex)
{
    ScopedNetworkThreadLock tNetworkLock;






    if (sptParty->GetControllerIndex() == nControllerIndex)
    {


        for (int32 nPlayerIndex = sptParty->GetFirstLocalPlayerIndex();
             nPlayerIndex != -1;
             nPlayerIndex = sptParty->GetNextLocalPlayerIndex(nPlayerIndex))
        {
            const PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIndex];
            if (tPlayer.m_nControllerIndex != nControllerIndex && !tPlayer.m_bIsLeaving)
            {
                sptParty->m_nControllerIndex = tPlayer.m_nControllerIndex;
                break;
            }
        }
    }


    if (sptParty->IsHost())
    {
        sptParty->RemovePlayer(bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr(), nControllerIndex);


        sptParty->SendPartyInfo(NULL);
    }
    else
    {
        D3PartyMsgRemovePlayer tLeaveRequest(nControllerIndex);

        bdMessageRef ptMessage = ::new bdMessage(D3_MSG_PARTY_REMOVEPLAYER, false);
        tLeaveRequest.Serialize(ptMessage->getPayload());
        sptParty->SendToHost(ptMessage);

        int32 nPlayerIndex = sptParty->FindLocalPlayer(nControllerIndex);
        if (nPlayerIndex != -1)
        {
            PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIndex];
            tPlayer.m_bIsLeaving = TRUE;
        }
    }




}




void PartyUpdatePlayerHero(int32 nControllerIndex)
{
    ScopedNetworkThreadLock tNetworkLock;
    int32 nPlayerIndex = sptParty->FindLocalPlayer(nControllerIndex);
    if (nPlayerIndex != -1)
    {
        PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIndex];
        tPlayer.UpdateAccountAndHero();
        if (sptParty->IsHost())
        {
            sptParty->SendHeroDigest(tPlayer.m_idPlayer, NULL, NULL);
            sptParty->MarkGameInfoDirty();
        }
        else
        {
            sptParty->SendPlayerAccountAndHeroToHost(tPlayer, Party::SEND_ACCOUNT_AND_HERO);
        }
    }
}













BOOL PartyIsHost()
{
    PartyRef ptParty = sptParty;
    return ptParty && ptParty->IsHost();
}






BOOL PartyCanStartGame()
{
    PartyRef ptParty = sptParty;
    return ptParty && ptParty->CanStartGame();
}






BOOL PartyIsGameStarted()
{
    PartyRef ptParty = sptParty;
    return ptParty && ptParty->IsGameStarted();
}






BOOL PartyIsInGame()
{
    PartyRef ptParty = sptParty;
    return ptParty && ptParty->IsInGame();
}






BOOL PartyIsOnline()
{
    PartyRef ptParty = sptParty;
    return ptParty && ptParty->IsOnline();
}






BOOL PartyIsPublic()
{
    PartyRef ptParty = sptParty;
    return ptParty && ptParty->IsPublic();
}







BOOL PartyStartGame()
{
    ScopedNetworkThreadLock tNetworkLock;
    return sptParty->StartGame();
}








void PartyServerEndingGame()
{
    ScopedNetworkThreadLock tLock;
    sptParty->ServerEndingGame();
}









void PartyEndGame()
{

    ScopedNetworkThreadLock tLock;

    if (PartyIsInGame())
    {
        sptParty->EndGame();
    }
}






BOOL PartyIsInParty()
{
    return sptParty != NULL;
}





BOOL PartyIsFull()
{
    PartyRef ptParty = sptParty;
    return ptParty && ptParty->m_uNumPlayers >= ptParty->m_uMaxPlayers;
}






BOOL PartyIsInParty(int32 nControllerIndex)
{
    ScopedNetworkThreadLock tLock;
    return sptParty && sptParty->FindLocalPlayer(nControllerIndex) != -1;
}


















void PartySetGameIsPaused(BOOL bIsPaused)
{

    ScopedNetworkThreadLock tLock;

    if (sptParty->IsHost())
    {
        sptParty->m_bIsPaused = bIsPaused;
        sptParty->SendPartyInfo(NULL);
    }
}








void PartySetGameIsSoftPaused(BOOL bIsPaused)
{

    ScopedNetworkThreadLock tLock;

    if (sptParty->IsHost())
    {
        sptParty->m_bIsSoftPaused = bIsPaused;
        sptParty->SendPartyInfo(NULL);
    }
}








BOOL PartyIsGamePaused()
{
    PartyRef ptParty = sptParty;
    return ptParty && (ptParty->m_bIsPaused || ptParty->m_bIsSoftPaused);
}






void PartySetMatchmakingEnabled(BOOL bMatchmakingEnabled)
{
    PartyRef ptParty = sptParty;
    if (ptParty)
    {
        ptParty->m_bMatchmakingEnabled = bMatchmakingEnabled;
    }
}







int32 PartyFindPlayer(
    GameNetwork::ConnectionRef ptGameConnection,
    int32 nControllerIndex,
    BOOL & bIsPrimary,
    BOOL & bIsGuest,
    BOOL & bIsLocal,
    char * szUsername,
    uint32 uUsernameLen)
{
    ScopedNetworkThreadLock tLock;





    bdConnectionRef ptConnection = reference_cast<GameConnection>(ptGameConnection)->GetConnection();

    for (int32 i = sptParty->GetFirstPlayerIndex(); i != -1; i = sptParty->GetNextPlayerIndex(i))
    {
        PartyPlayer & tPlayer = sptParty->m_tPlayers[i];

        BOOL bIsMatch;
        if (ptConnection)
        {
            bIsMatch = tPlayer.IsAddressAndIndex(ptConnection->getAddress(), nControllerIndex);
        }
        else
        {
            bIsMatch = tPlayer.IsLocal(nControllerIndex);
        }

        if (bIsMatch)
        {
            bIsPrimary = tPlayer.IsPrimary();
            bIsGuest = tPlayer.IsGuest();
            bIsLocal = tPlayer.IsLocal();
            if (szUsername)
            {
                Blizzard::String::Copy(szUsername, tPlayer.m_szName, uUsernameLen);
            }
            return i;
        }
    }

    return -1;
}





























































































static Blizzard::Lock::RMutex stPartyMembersMutex;
static PartyMember stPartyMembers[MAX_PEERS];
static uint32 suPartyMembersCount;

void UpdatePartyMembers()
{

    Blizzard::Lock::ScopedMutex tLock(stPartyMembersMutex);

    suPartyMembersCount = 0;
    if (!sptParty)
    {
        return;
    }

    for (int32 nPlayerIdx = sptParty->GetFirstPlayerIndex(); nPlayerIdx != -1; nPlayerIdx = sptParty->GetNextPlayerIndex(nPlayerIdx))
    {
        PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIdx];
        PartyMember & tMember = stPartyMembers[suPartyMembersCount++];
        tPlayer.Export(tMember, TRUE);
    }
}





uint32 PartyGetMembers(PartyMember * ptMembers, BOOL bDigests, BOOL bLocalOnly)
{
    uint32 uNumMembers = 0;

    Blizzard::Lock::ScopedMutex tLock(stPartyMembersMutex);

    for (uint32 nMemberIndex = 0; nMemberIndex < suPartyMembersCount; ++nMemberIndex)
    {
        const PartyMember & tSource = stPartyMembers[nMemberIndex];
        if (!bLocalOnly || tSource.bIsLocal)
        {
            PartyMember & tTarget = ptMembers[uNumMembers++];
            tSource.Copy(tTarget, bDigests);
        }
    }

    return uNumMembers;
}







uint32 PartyGetMemberCount()
{
    PartyRef ptParty = sptParty;
    return ptParty ? ptParty->m_uNumPlayers : 0;
}






uint32 PartyGetLocalMemberCount()
{
    uint32 uNumPlayers = 0;
    PartyRef ptParty = sptParty;
    if (ptParty)
    {
        for (int32 nPlayerIdx = ptParty->GetFirstLocalPlayerIndex(); nPlayerIdx != -1; nPlayerIdx = ptParty->GetNextLocalPlayerIndex(nPlayerIdx))
        {
            uNumPlayers++;
        }
    }

    return uNumPlayers;
}










uint32 PartyGetMemberCountToJoinOnlineGame()

{
    PartyRef ptParty = sptParty;

    if (!ptParty)
        return 0;


    if (!ptParty->IsHost() || !ptParty->IsOnline())
    {

        return PartyGetLocalMemberCount();
    }


    return PartyGetMemberCount();

}




int32 PartyGetHostIndex()
{
    PartyRef ptParty = sptParty;
    return ptParty ? ptParty->m_idHostPlayer : -1;
}






BOOL PartyGetPartyMemberForPlayer(
    const Player * ptPlayer,
    PartyMember & tPartyMember)
{
    ScopedNetworkThreadLock tLock;

    if (!sptParty)
        return FALSE;



    PartyPlayer & tPlayer = sptParty->m_tPlayers[ptPlayer->nPlayerIndex];
    if (!tPlayer.IsValid() ||
        (tPlayer.IsLocal() && tPlayer.m_bIsLeaving))
        return FALSE;



    tPlayer.Export(tPartyMember, FALSE);

    return TRUE;
}





BOOL PartyGetPlayerAccountAndHero(
    int32 nPlayerIndex,
    D3::Account::SavedDefinition * ptAccountDefinition,
    D3::Hero::SavedDefinition * ptHeroDefinition)
{
    ScopedNetworkThreadLock tLock;

    if (!sptParty ||
        !sptParty->IsInGame() ||
        !sptParty->IsHost())
        return FALSE;



    if (nPlayerIndex == PLAYERID_HOST)
    {
        for (nPlayerIndex = sptParty->GetFirstLocalPlayerIndex();
             nPlayerIndex != -1;
             nPlayerIndex = sptParty->GetNextLocalPlayerIndex(nPlayerIndex))
        {
            PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIndex];
            if (tPlayer.m_nControllerIndex == sptParty->GetControllerIndex())
                break;
        }
    }





    PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIndex];
    if (!tPlayer.IsValid())
        return FALSE;



    if (ptAccountDefinition != NULL)
    {
        const D3::Account::SavedDefinition * ptPlayerAccountDefinition = tPlayer.GetAccount();
        if (ptPlayerAccountDefinition == NULL || !ptPlayerAccountDefinition->has_digest())
            return FALSE;



        *ptAccountDefinition = *ptPlayerAccountDefinition;
    }

    if (ptHeroDefinition != NULL)
    {
        const D3::Hero::SavedDefinition * ptPlayerHeroDefinition = tPlayer.GetHero();
        if (ptPlayerHeroDefinition == NULL || !ptPlayerHeroDefinition->has_digest())
            return FALSE;



        *ptHeroDefinition = *ptPlayerHeroDefinition;
    }

    return TRUE;
}




void PartyUpdateCachedRemotePlayerAccountAndHero(
    int32 nPartyIndex,
    OnlineService::HeroId idHero,
    const D3::Account::SavedDefinition & tAccountDefinition,
    const D3::Hero::SavedDefinition & tHeroDefinition)
{
    ScopedNetworkThreadLock tLock;

    if (!sptParty ||
        !sptParty->IsInGame() ||
        !sptParty->IsHost())
        return;





    PartyPlayer & tPlayer = sptParty->m_tPlayers[nPartyIndex];
    if (!tPlayer.IsValid() ||
        tPlayer.IsLocal() ||
        tPlayer.m_ptAccountDefinition == NULL)
        return;



    if (tPlayer.m_ptHeroDefinition == NULL ||
        !tPlayer.m_ptHeroDefinition->has_digest() ||
        tPlayer.m_ptHeroDefinition->digest().hero_id().id_low() != idHero)
        return;





    *tPlayer.m_ptAccountDefinition = tAccountDefinition;
    *tPlayer.m_ptHeroDefinition = tHeroDefinition;


    tPlayer.m_ptHeroDefinition->mutable_digest()->set_deprecated_alt_level(tPlayer.m_ptAccountDefinition->partitions(0).alt_level());
}




void PartyUpdateCachedLocalPlayerAccountAndHero(int32 nPartyIndex, const D3::Account::SavedDefinition &, const D3::Hero::SavedDefinition &)
{
    ScopedNetworkThreadLock tLock;
    if (sptParty)
    {
        PartyPlayer & tPlayer = sptParty->m_tPlayers[nPartyIndex];
        if (tPlayer.IsValid() && tPlayer.IsLocal())
        {
            tPlayer.UpdateAccountAndHero();
        }
    }
}















BOOL PartyIsPlayerAndHeroStillPresent(int32 nPartyIndex, OnlineService::HeroId idHero)
{
    ScopedNetworkThreadLock tLock;
    if (sptParty && sptParty->IsInGame() && sptParty->IsHost())
    {
        PartyPlayer & tPlayer = sptParty->m_tPlayers[nPartyIndex];
        if (tPlayer.IsValid())
        {
            const D3::Hero::SavedDefinition * ptDefinition = tPlayer.GetHero();
            if (ptDefinition)
            {
                return ptDefinition->digest().hero_id().id_low() == idHero;
            }
        }
    }

    return FALSE;
}













void sPartyMatchmakeCallback(
    PartyInfo * ptPartyInfos,
    uint32 uMaxParties,
    PartyFindCallback pfnCallback,
    MMSearchResult eResult)
{
    MatchmakerResults * ptResults = new MatchmakerResults[uMaxParties];

    uint32 uNumResults = Matchmaker::Get()->GetSearchResults(ptResults, uMaxParties);

    for (uint32 i = 0; i < uNumResults; ++i)
    {
        MatchmakerResults & tResult = ptResults[i];
        PartyInfo & tPartyInfo = ptPartyInfos[i];

        tPartyInfo.m_ptAddress = tResult.ptAddress;
        tPartyInfo.m_tSecID = tResult.tSecID;
        tPartyInfo.m_tSecKey = tResult.tSecKey;
        tPartyInfo.m_bIsOnline = TRUE;
    }

    delete[] ptResults;


    QueueNetworkEventForMainThread(OnlineService::internal::method_binder_2<void, OnlineService::internal::callback_base<2, PartyFindCallback>, const MMSearchResult &, uint32>::bind<&OnlineService::internal::callback_base<2, PartyFindCallback>::execute>(pfnCallback, eResult, uNumResults));
}

void PartyMatchmake(
    PartyInfo * ptParties,
    uint32 uMaxParties,
    PartyFindCallback pfnCallback)
{
    ScopedNetworkThreadLock tLock;




    MatchParameters tParams;
    sptParty->GetMatchParameters(tParams);



















    Matchmaker::Get()->Search(
        tParams,
        OnlineService::internal::function_binder_4<void, PartyInfo *, uint32, PartyFindCallback, MMSearchResult>::bind<&sPartyMatchmakeCallback>(ptParties, uMaxParties, pfnCallback));
}















void PartyMatchmakeCancel()
{
    ScopedNetworkThreadLock tLock;
    Matchmaker::Get()->Cancel();
}





void PartyForceMigration()
{
    PartyRef ptParty = sptParty;
    if (ptParty)
    {
        ptParty->ForceMigrate();
    }
}






PartyOnlineState PartyGetOnlineState()
{
    PartyRef ptParty = sptParty;
    return ptParty ? ptParty->GetOnlineState() : PARTY_OFFLINE;
}






PartyOnlineState PartyGetPreferredOnlineState()
{
    PartyRef ptParty = sptParty;
    return ptParty ? ptParty->GetPreferredOnlineState() : PARTY_OFFLINE;
}





BOOL PartyIsOnlineStateChanging(PartyOnlineState & ePendingState)
{
    if (!sptParty)
    {
        return FALSE;
    }

    ePendingState = sptParty->m_ePendingOnlineState;
    return sptParty->IsOnlineStateChanging();
}
PartySwitchOnlineResult PartyCanSwitchOnlineState(PartyOnlineState eState)
{
    PartySwitchOnlineResult eResult = PARTY_SWITCH_ONLINE_OK;
    ScopedNetworkThreadLock tLock;



    if (sptParty->GetOnlineState() != eState)
    {








        if (eState != PARTY_OFFLINE && eState != PARTY_ADHOC && (!IsPhysicalNetworkConnected() || eState == PARTY_LAN))
        {
            eResult = PARTY_SWITCH_NO_NETWORK_CONNECTION;
        }
        else
        {










            eResult = sptParty->CheckOnlineStateEligibility(eState);

            if (eResult == PARTY_SWITCH_ONLINE_OK)
            {




                if (PartySwitchWillForceDisconnect(eState))
                {
                    eResult = PARTY_SWITCH_ONLINE_WILL_DISCONNECT;
                }
            }
        }
    }

    return eResult;
}

BOOL PartySwitchWillForceDisconnect(PartyOnlineState eState)
{


    if (eState == PARTY_OFFLINE || eState == PARTY_ADHOC ||


        (eState == PARTY_LAN && sptParty->IsOnline()) || sptParty->IsLAN() || !sptParty->IsHost())
    {

        for (int32 nPlayerIdx = sptParty->GetFirstPlayerIndex(); nPlayerIdx != -1; nPlayerIdx = sptParty->GetNextPlayerIndex(nPlayerIdx))
        {

            PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIdx];
            if (!tPlayer.IsLocal())
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}




void PartySwitchOnlineState(PartyOnlineState eState)
{

    ScopedNetworkThreadLock tLock;

    if (!sptParty || !sptParty->IsHost())
    {
        return;
    }

    if (eState == PARTY_LAN)
    {
        eState = PARTY_OFFLINE;
    }

    sptParty->SetOnlineState(eState);
}








void PartySwitchPreferredOnlineState(PartyOnlineState eState)
{
    ScopedNetworkThreadLock tLock;
    if (sptParty)
    {
        sptParty->SetPreferredOnlineState(eState);
    }
}










BOOL PartyGetGameParams(GameParams & tInfo)
{


    PartyRef ptParty = sptParty;
    if (ptParty)
    {
        ScopedNetworkThreadLock tLock;
        tInfo = ptParty->m_tGameParams;
        return TRUE;
    }

    return FALSE;
}



BOOL PartyGetGameTag(int32 & pnTag)
{
    PartyRef ptParty = sptParty;
    if (ptParty)
    {
        pnTag = snGameTag;
        return TRUE;
    }

    return FALSE;
}







void PartySetGameParams(
    const GameParams & tInfo)
{
    ScopedNetworkThreadLock tLock;



    if (sptParty->IsHost())
    {



        uint32 uPreviousChallengeNumber = sptParty->m_tGameParams.m_tWeeklyChallengeConfig.challenge_number();

        sptParty->m_tGameParams = tInfo;

        if (sptParty->m_tGameParams.m_eAct == ACT_OPEN_WORLD)
        {
            sptParty->m_tGameParams.m_snoQuest = SNO_NONE;
            sptParty->m_tGameParams.m_nQuestStep = QUEST_UNASSIGNED_STEP_UID;
            sptParty->m_tGameParams.m_bResumeFromSave = FALSE;
        }

        sptParty->SendPartyInfo(NULL);

        if (sptParty->m_tGameParams.m_eGameType == GT_CHALLENGE_RIFTS &&
            sptParty->m_tGameParams.m_tWeeklyChallengeConfig.challenge_number() != uPreviousChallengeNumber)
        {
            sptParty->SendChallengeRift(NULL);
        }

        sgtPartyGameInfoChangedEvent.NotifySubscribers();
    }
}







namespace Metrics {

static uint32 s_uLocalPlayerHighWaterMark;

void UpdateLocalPlayerCount(
    uint32 uNumLocalPlayers)
{
    int32 nPrimaryIndex = GamerProfile::GetPrimaryProfileUserIndex();
    if (uNumLocalPlayers == 0 || nPrimaryIndex == -1)
    {
        s_uLocalPlayerHighWaterMark = 0;
        return;
    }

    if (uNumLocalPlayers <= 4 && uNumLocalPlayers > s_uLocalPlayerHighWaterMark &&
        LobbyIsConnected(nPrimaryIndex))
    {

        LobbyService::CounterVector atCounters;


        if (s_uLocalPlayerHighWaterMark != 0)
        {
            atCounters.emplace_back(GlobalCounter(s_uLocalPlayerHighWaterMark), -1);
        }
        atCounters.emplace_back(GlobalCounter(uNumLocalPlayers), 1);

        Blizzard::Lock::ScopedRMutex tLock(stNetworkThreadLock);
        LobbyService::Get(nPrimaryIndex)->UpdateGlobalCounters(atCounters);
        s_uLocalPlayerHighWaterMark = uNumLocalPlayers;

    }
}

} // namespace Metrics





void LANUpdateGameInfo(
    const char * szGameName,
    const bdSecurityID & tSecID,
    const bdSecurityKey & tSecKey,
    const GameInfo & tGameInfo)
{



    char szRealGameName[260];

    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
    const bdCommonAddrRef ptLocalAddr = ptNet->getLocalCommonAddr();









    Blizzard::String::Copy(szRealGameName, szGameName, sizeof(szRealGameName));



    if (!sptLANDiscoveryServer)
    {
        sptLANDiscoveryServer = new bdLANDiscoveryServer;
    }


    D3GameInfoRef ptGameInfo = reference_cast<D3GameInfo>(sptLANDiscoveryServer->getGameInfo());
    if (!ptGameInfo)
    {
        ptGameInfo = static_cast<D3GameInfo *>(bdGameInfoFactory::getInstance()->create());
        sptLANDiscoveryServer->start(ptGameInfo, bdInetAddr::Any(), 50000);
    }


    ptGameInfo->SetName(szRealGameName);
    ptGameInfo->setTitleID(0x1671);
    ptGameInfo->setHostAddr(ptLocalAddr);
    ptGameInfo->setSecurityID(tSecID);
    ptGameInfo->setSecurityKey(tSecKey);
    ptGameInfo->SetGameInfo(tGameInfo);

}





SwitchUniqueID GetSwitchUniqueID()
{
    static SwitchUniqueID s_UniqueID = []() {
        SwitchUniqueID tUniqueID = {};
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            nn::crypto::GenerateCryptographicallyRandomBytes(&tUniqueID, sizeof(tUniqueID));
        }
        return tUniqueID;
    }();

    return s_UniqueID;
}


void ADHocUpdateHost(
    const bdSecurityID & tSecID,
    const bdSecurityKey & tSecKey,
    const GameInfo & tGameInfo,
    BOOL bForceUpdateGameInfo)
{
    bdNetImpl *const ptNet = bdSingleton<bdNetImpl>::getInstance();
    const bdCommonAddrRef ptLocalAddr = ptNet->getLocalCommonAddr();


    ScopedAdHocOperationThreadLock tAdHocLock;

    bdSocketRouterAdHoc * RouterAdHoc = ptNet->getSocketRouterAdHoc();


    if (RouterAdHoc && !UIOnlineActions::IsJoinAdHocNetworkInProgress())
    {
        if (nn::ldn::GetState() == nn::ldn::State_Station || nn::ldn::GetState() == nn::ldn::State_StationConnected)
        {
            RouterAdHoc->leaveNetwork();
        }
        else if (nn::ldn::GetState() == nn::ldn::State_AccessPointCreated || nn::ldn::GetState() == nn::ldn::State_AccessPoint)
        {


            if (sbRequestNewAdHocResults && sgtParties.size() < 2)
            {

                if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
                {
                    UIPartyDisplay::Console::FindLocalCommunicationGames();
                }
                else
                {
                    UIHeroSelect::Console::RefreshAdHocList();
                }

                sbRequestNewAdHocResults = FALSE;
            }


            Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();
            Blizzard::Time::Millisecond uTimeSinceLastStationUpdate = Blizzard::Time::GetTimeElapsed(suLastAdHocStationStatusUpdateMS, now);
            if (bForceUpdateGameInfo || uTimeSinceLastStationUpdate > ADHOC_STATION_ADVERTISE_UPDATE_DELAY)
            {
                SwitchAdhocCustomData advertiseData;
                advertiseData.m_gameInfo = tGameInfo;
                advertiseData.m_uniqueID = GetSwitchUniqueID();

                uint32 sizeofCustomData = sizeof(advertiseData);
                RouterAdHoc->updateAdvertiseData(reinterpret_cast<const bdNChar8 *>(&advertiseData), sizeofCustomData, tSecID, tSecKey);
                suLastAdHocStationStatusUpdateMS = Blizzard::Time::Milliseconds();
            }
        }

        else if (nn::ldn::GetState() == nn::ldn::State_Initialized)
        {
            nn::ldn::NetworkConfig networkConfig = {};
            networkConfig.intentId.localCommunicationId = 0x01001b300b9be000;
            networkConfig.intentId.sceneId = 1;
            networkConfig.channel = 0;
            networkConfig.nodeCountMax = 4;
            networkConfig.localCommunicationVersion = 0;

            static const uint8_t passphrase[16] = { 0xce, 0x83, 0x3a, 0x55, 0xd6, 0x92, 0xca, 0x7d, 0x57, 0x88, 0x7e, 0x2e, 0x2f, 0xce, 0x7a, 0xb7 };




            nn::ldn::SecurityConfig security = {};

            security.securityMode = nn::ldn::SecurityMode_Debug;
            SigmaMemoryCopy(security.passphrase, passphrase, sizeof(passphrase));
            security.passphraseSize = sizeof(passphrase);

            nn::ldn::UserConfig user = {};
            nn::account::Nickname NickName = GamerProfile::GetUserNickName(GamerProfile::GetPrimaryProfileUserIndex());
            strncpy(user.userName, NickName.name, 32);

            SwitchAdhocCustomData advertiseData;
            advertiseData.m_gameInfo = tGameInfo;
            advertiseData.m_uniqueID = GetSwitchUniqueID();

            uint32 sizeofCustomData = sizeof(advertiseData);

            if (RouterAdHoc->createNetwork(networkConfig, security, user, reinterpret_cast<const bdNChar8 *>(&advertiseData), sizeofCustomData, tSecID, tSecKey))
            {


                bdCommonAddrRef ptLocalAddr = bdSingleton<bdNetImpl>::getInstance()->getLocalCommonAddr();
                for (int32 nPlayerIdx = sptParty->GetFirstLocalPlayerIndex(); nPlayerIdx != -1;

                     nPlayerIdx = sptParty->GetNextLocalPlayerIndex(nPlayerIdx))
                {
                    PartyPlayer & tPlayer = sptParty->m_tPlayers[nPlayerIdx];
                    tPlayer.m_ptAddr = ptLocalAddr;
                }


                sbRequestNewAdHocResults = TRUE;


                suLastAdHocStationStatusUpdateMS = Blizzard::Time::Milliseconds();
            }
        }
    }
}




















































































void CreateDummySetup(
    const DummySetupParams & tParams)
{
    for (uint32 i = 0; i < tParams.m_dwLocalPlayerCount; ++i)
    {
        GamerProfile::LoadAccountDevOnly(i, tParams.m_playerClasses[i]);
    }

    GameParams tGameParams;
    tGameParams.m_eGameType = -1;
    tGameParams.m_snoDevWorld = tParams.m_snoWorld;
    tGameParams.m_dwDevCreationFlags = tParams.m_dwCreationFlags;
    tGameParams.m_nHandicap = tParams.m_nHandicap;

    PartyCreate(0, tGameParams, tParams.m_dwLocalPlayerCount > 1);

    for (uint32 i = 1; i < tParams.m_dwLocalPlayerCount; ++i)
    {
        PartyAddPlayer(i, PartyAddPlayerCallback());
    }

    PartyStartGame();
}















PartyMemberAddedEvent & GetPartyMemberAddedEvent()
{
    return sgtPartyMemberAddedEvent;
}




PartyMemberReceivedDigestEvent & GetPartyMemberReceivedDigestEvent()
{
    return sgtPartyMemberReceivedDigestEvent;
}




PartyMemberRemovedEvent & GetPartyMemberRemovedEvent()
{
    return sgtPartyMemberRemovedEvent;
}




PartyInviteAcceptedEvent & GetPartyInviteAcceptedEvent()
{
    return sgtPartyInviteAcceptedEvent;
}




PartyJoinedEvent & GetPartyJoinedEvent()
{
    return sgtPartyJoinedEvent;
}




PartyGameInfoChangedEvent & GetPartyGameInfoChangedEvent()
{
    return sgtPartyGameInfoChangedEvent;
}




PartyMemberVoiceChatTalkingEvent & GetPartyMemberVoiceChatTalkingEvent()
{
    return sgtPartyMemberVoiceChatTalkingEvent;
}




PartyMigratingEvent & GetPartyMigratingEvent()
{
    return sgtPartyMigratingEvent;
}




PartyOnlineStateChangedEvent & GetPartyOnlineStateChangedEvent()
{
    return sgtPartyOnlineStateChangedEvent;
}




PSNSignInChangedEvent & GetPSNSignInChangedEvent()
{
    return sgtPSNSignInChangedEvent;
}




PhysicalNetworkStatusChangedEvent & GetPhysicalNetworkStatusChangedEvent()
{
    return sgtPhysicalNetworkStatusChangedEvent;
}




GameStartingEvent & GetGameStartingEvent()
{
    return sgtGameStartingEvent;
}




GameEndingEvent & GetGameEndingEvent()
{
    return sgtGameEndingEvent;
}




LobbyConnectionEvent & GetLobbyConnectionEvent()
{
    return sgtLobbyConnectionEvent;
}




NewMailEvent & GetNewMailEvent()
{
    return sgtNewMailEvent;
}














PlayTogetherEvent & GetPlayTogetherEvent()
{
    return sgtPlayTogetherEvent;
}




SeasonsChangedEvent & GetSeasonsChangedEvent()
{
    return sgtSeasonsChangedEvent;
}

}  // namespace Online
}  // namespace Console
