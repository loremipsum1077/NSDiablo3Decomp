// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyService.cpp
// Diagnostic text is inferred; this release retains only variadic arguments.
#include "bdLobbyService.h"
#include "bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "bdMessaging/bdMessaging.h"
#include "bdStats/bdStats.h"
#include "bdTitleUtilities/bdTitleUtilities.h"
#include "bdEventLog/bdEventLog.h"
#include "bdCounter/bdCounter.h"
#include "bdDML/bdDML.h"
#include "bdLinkedAccounts/bdLinkedAccounts.h"
#include "bdMatchMaking/bdMatchMaking.h"
#include "bdStorage/bdStorage.h"
#include "bdMail/bdMail.h"
#include "bdRichPresence/bdRichPresenceService.h"
#include "bdLobbyCommon/bdStructBufferDeserializerHelpers.h"
#include "bdLobbyCommon/bdUserAccountID.h"
#include "bdLobbyEventHandler.h"
#include "bdTencent/bdTencent.h"
#include "bdMatchMaking/bdSessionID.h"
#include "bdFeatureBan/bdFeatureBanInfo.h"
#include "bdMessaging/bdChannelInfo.h"
#include "bdMessaging/bdChannelMember.h"
#include "bdMarketplace/bdMarketplaceCurrency.h"
#include "bdReward/bdRewardEventJSON.h"
#include "bdReward/bdRewardEventJSONV2.h"
#include "../bdCore/bdUtilities/bdBase64.h"
#include "../bdNet/bdGetHostByName/bdGetHostByNameConfig.h"
#include "../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../bdSocket/bdSocketRouterAdHoc-switch.h"
#include "bdCommerce/bdCommerce.h"  // line table of cleanup()
#include "bdProfiles/bdProfiles.h"  // line table of cleanup()
#include "bdFriends/bdFriends.h"  // line table of cleanup()
#include "bdContentUnlock/bdContentUnlock.h"  // line table of cleanup()
#include "bdKeyArchive/bdKeyArchive.h"  // line table of cleanup()
#include "bdGroup/bdGroup.h"  // line table of cleanup()
#include "bdTags/bdTags.h"  // line table of cleanup()
#include "bdVoteRank/bdVoteRank.h"  // line table of cleanup()
#include "bdTwitch/bdTwitch.h"  // line table of cleanup()
#include "bdTwitter/bdTwitter.h"  // line table of cleanup()
#include "bdFacebook/bdFacebook.h"  // line table of cleanup()
#include "bdLinkCode/bdLinkCode.h"  // line table of cleanup()
#include "bdAntiCheat/bdAntiCheat.h"  // line table of cleanup()
#include "bdObjectStore/bdObjectStore.h"  // line table of cleanup()
#include "bdAchievementsEngine/bdAchievementsEngineService.h"  // line table of cleanup()
#include "bdABTesting/bdABTesting.h"  // line table of cleanup()
#include "bdUserGroups/bdUserGroups.h"  // line table of cleanup()
#include "bdPresence/bdPresenceService.h"  // line table of cleanup()
#include "bdSubscription/bdSubscription.h"  // line table of cleanup()
#include "bdFeatureBan/bdFeatureBan.h"  // line table of cleanup()
#include "bdCODO/bdCODOTeamMarketplace.h"  // line table of cleanup()
#include "bdLeague/bdLeague.h"  // line table of cleanup()
#include "bdRelayService/bdRelayService.h"  // line table of cleanup()
#include "bdVoting/bdVoting.h"  // line table of cleanup()
#include "bdUserBlockList/bdUserBlockList.h"  // line table of cleanup()
#include "bdDDL/bdDDL.h"  // line table of cleanup()
#include "bdMatchScheduler/bdMatchScheduler.h"  // line table of cleanup()
#include "bdESportsProvider/bdMLG.h"  // line table of cleanup()
#include "bdRedeemableCode/bdRedeemableCodeService.h"  // line table of cleanup()
#include "bdMarketingComms/bdMarketingComms.h"  // line table of cleanup()
#include "bdMarketplace/bdMarketplace.h"  // line table of cleanup()
#include "bdReward/bdReward.h"  // line table of cleanup()
#include "bdPublisherVariables/bdPublisherVariables.h"  // line table of cleanup()
#include "bdPlayerVote/bdPlayerVote.h"  // line table of cleanup()
#include "bdServerInventory/bdServerInventory.h"
#include "bdTeams/bdTeams.h"
#include "bdTeamShowcase/bdTeamShowcase.h"
// Minimal local declarations (the destructor is vtable slot 0 in the DWARF; cleanup() deletes through it).
// The full classes belong in the headers named; their base classes need headers src does not have yet.
class bdContentStreaming { public: virtual ~bdContentStreaming(); };  // bdContentStreaming/bdContentStreaming.h
class bdPooledStorage { public: virtual ~bdPooledStorage(); };  // bdPooledStorage/bdPooledStorage.h
class bdUmbrella { public: virtual ~bdUmbrella(); };  // bdSingleIdentity/bdUmbrella.h
class bdUno { public: virtual ~bdUno(); };  // bdSingleIdentity/bdUno.h
#include "bdMarketplace/bdMarketplaceInventory.h"
#include "bdMarketplace/bdMarketplaceCouponInventory.h"
#include "bdMarketplace/bdMarketplaceCouponInventoryV2.h"
#include "bdMarketplace/bdMarketplaceCouponInventoryV3.h"
#include "bdMarketplace/bdMarketplaceCouponInventoryV4.h"
#include "bdAchievementsEngine/bdAchievementsUpdatedPushMessage.h"
#include "bdAchievementsEngine/bdAchievementState.h"
#include "bdMatchMaking/bdSessionInvite.h"
#include "bdTencent/bdTencentAASRecord.h"
#include "bdTencent/bdTencentResponse.h"

















































































































bdBool bdLobbyService::connect(const bdAuthInfo * authInfo, const bdBool useEncryption)
{
    bdBool ok = false;
    if (m_isResolved)
    {
        ok = connect(m_resolvedLSGAddress, authInfo, 0x1FFFFF, 0x1FFFFF, useEncryption);
    }
    return ok;
}






bdBool bdLobbyService::connect(bdAddr lobbyServiceAddr,
                               const bdAuthInfo * authInfo,
                               bdUWord maxSendBufSize,
                               bdUWord maxRecvBufSize,
                               const bdBool useEncryption)
{
    bdBool ok = false;
    if (authInfo == NULL)
    {
        bdLogWarn("lobby service", "No auth info supplied.");  // text inferred
    }
    else
    {

        if (!m_lobbyConnection.isNull())
        {
            m_lobbyConnection->disconnect();
            cleanup();
        }

        m_encryptedConnection = useEncryption;
        m_authInfo = *authInfo;
        m_lobbyConnection = new bdLobbyConnection(lobbyServiceAddr, this, true, !useEncryption, maxSendBufSize, maxRecvBufSize);
        m_lobbyConnectionEstablished = false;
        ok = m_lobbyConnection->connect(m_authInfo);
        updateConnectionThrottleParams();
    }
    return ok;
}


























bdBool bdLobbyService::pumpDNSLookup(bdGetHostByName::bdStatus & status)
{
    if (!m_isResolved)
    {
        status = m_LSGLookup.getStatus();
        switch (status)
        {
        case bdGetHostByName::BD_LOOKUP_PENDING:
            m_LSGLookup.pump();
            break;
        case bdGetHostByName::BD_LOOKUP_SUCCEEDED:
            if (m_LSGLookup.getNumAddresses() > 0)
            {
                bdInAddr inAddr = m_LSGLookup.getAddressAt(0);
                bdInetAddr inetAddr(inAddr);
                bdNChar8 addrString[22];
                m_resolvedLSGAddress.set(inetAddr, m_LSGPort);
                m_resolvedLSGAddress.toString(addrString, sizeof(addrString));
                bdLogInfo("lobby service", "Resolved LSG address: %s", addrString);
                m_isResolved = true;
            }
            break;
        case bdGetHostByName::BD_LOOKUP_UNINITIALIZED:
        case bdGetHostByName::BD_LOOKUP_FAILED:
        case bdGetHostByName::BD_LOOKUP_CANCELLED:
        case bdGetHostByName::BD_LOOKUP_TIMED_OUT:
        case bdGetHostByName::BD_LOOKUP_ERROR:
            bdLogWarn("lobby service", "DNS lookup failed.");
            m_isResolved = true;
            break;
        }
    }
    return m_isResolved;
}










































void bdLobbyService::disconnect()
{
    if (m_lobbyConnection)
    {
        m_lobbyConnection->disconnect();
    }
    m_titleID = 0;
    cleanup();
}

















// Case 5 comes before case 1: the original line table has the task-reply copy at line 400 and the LSG copy at 414.
void bdLobbyService::pump()
{
    if (!m_lobbyConnection)
    {
        return;
    }



    bdByteBufferRef message;
    bdUByte8 messageType = 0;
    while (m_lobbyConnection && m_lobbyConnection->getMessageToDispatch(messageType, message))
    {
        switch (messageType)
        {
        case 5:
            bdLogInfo("lobby service", "Received task reply.");
            if (m_taskManager && message.notNull())
            {
                m_taskManager->handleTaskReply(message);
            }
            else
            {
                bdLogWarn("lobby service", "Cannot handle task reply.");
            }
            break;
        case 1:
            bdLogInfo("lobby service", "Received LSG task reply.");
            if (m_taskManager && message.notNull())
            {
                m_taskManager->handleLSGTaskReply(message);
            }
            else
            {
                bdLogWarn("lobby service", "Cannot handle LSG task reply.");
            }
            break;
        case 2:
            bdLogInfo("lobby service", "Received push message.");
            handlePushMessage(message);
            break;
        case 3:
        {
            bdUInt32 errorCode;
            if (message->readUInt32(errorCode))
            {
                m_errorCode = static_cast<bdLobbyErrorCode>(errorCode);
                bdLogWarn("lobby service", "Received error %u.", m_errorCode);
            }
            else
            {
                bdLogWarn("lobby service", "Cannot read error.");
            }
            break;
        }
        case 4:
        {
            bdUInt64 connectionID;
            if (message->readUInt64(connectionID) && m_taskManager)
            {
                bdLogInfo("lobby service", "Connection ID %llu.", connectionID);
                m_taskManager->m_connectionID = connectionID;
                m_lobbyConnectionEstablished = true;
            }
            else
            {
                bdLogWarn("lobby service", "Cannot read connection ID.");
            }
            break;
        }
        default:
            bdLogWarn("lobby service", "Unknown message type %u.", messageType);
            break;
        }
    }
    if (m_taskManager)
    {
        m_taskManager->cleanUpAsyncState();
    }
}


























bdLobbyConnection::Status bdLobbyService::getStatus()
{
    bdLobbyConnection::Status connectionStatus = bdLobbyConnection::BD_NOT_CONNECTED;
    if (m_lobbyConnection)
    {
        connectionStatus = m_lobbyConnection->getStatus();
        if (!m_lobbyConnectionEstablished && connectionStatus == bdLobbyConnection::BD_CONNECTED)
        {
            connectionStatus = bdLobbyConnection::BD_CONNECTING;
        }
    }
    return connectionStatus;
}












































void bdLobbyService::registerEventHandler(bdLobbyEventHandler * eventHandler)
{
    m_eventHandler = eventHandler;
}
















bdMessaging * bdLobbyService::getMessaging()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_messaging == NULL)
        {
            m_messaging = new bdMessaging(m_taskManager);
        }
        return m_messaging;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}

bdMatchMaking * bdLobbyService::getMatchMaking(const bdNChar8 * context)
{
    bdMatchMaking * matchmaking;
    if (getStatus() != bdLobbyConnection::BD_CONNECTED)
    {
        bdLogWarn("lobby service", "Cannot get service when not connected.");
        return NULL;
    }











    if (!m_matchmakings.get(context, matchmaking))
    {
        matchmaking = new bdMatchMaking(m_taskManager, context);
        m_matchmakings.put(context, matchmaking);
    }
    return matchmaking;
}
bdStats * bdLobbyService::getStats()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_statsManager == NULL)
        {
            m_statsManager = new bdStats(m_taskManager);
        }
        return m_statsManager;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}





















































bdStorage * bdLobbyService::getStorage(const bdNChar8 * context)
{
    bdStorage * storage = NULL;

    if (getStatus() != bdLobbyConnection::BD_CONNECTED)
    {
        bdLogWarn("lobby service", "Cannot get service when not connected.");
        return NULL;
    }
    if (!m_storageManagers.containsKey(context))
    {
        storage = new bdStorage(m_taskManager, context);
        m_storageManagers.put(context, storage);
    }
    m_storageManagers.get(context, storage);
    return storage;
}


bdMail * bdLobbyService::getMail(const bdNChar8 * context)
{
    bdMail * mail = NULL;

    if (getStatus() != bdLobbyConnection::BD_CONNECTED)
    {
        bdLogWarn("lobby service", "Cannot get service when not connected.");
        return NULL;
    }
    if (!m_mailManagers.containsKey(context))
    {
        mail = new bdMail(m_taskManager, context);
        m_mailManagers.put(context, mail);
    }
    m_mailManagers.get(context, mail);
    return mail;
}

















bdTitleUtilities * bdLobbyService::getTitleUtilities()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_titleUtilitiesManager == NULL)
        {
            m_titleUtilitiesManager = new bdTitleUtilities(m_taskManager);
        }
        return m_titleUtilitiesManager;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}

bdEventLog * bdLobbyService::getEventLog()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_eventLogManager == NULL)
        {
            m_eventLogManager = new bdEventLog(m_taskManager, true);
        }
        return m_eventLogManager;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}

















bdCounter * bdLobbyService::getCounter()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_counter == NULL)
        {
            m_counter = new bdCounter(m_taskManager);
        }
        return m_counter;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}
















































































































































































































bdDML * bdLobbyService::getDML()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_DML == NULL)
        {
            m_DML = new bdDML(m_taskManager);
        }
        return m_DML;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}




















































































bdRichPresenceService * bdLobbyService::getRichPresence(const bdNChar8 * context)
{
    bdRichPresenceService * richPresence = NULL;

    if (getStatus() != bdLobbyConnection::BD_CONNECTED)
    {
        bdLogWarn("lobby service", "Cannot get service when not connected.");
        return NULL;
    }
    if (!m_richPresenceManagers.containsKey(context))
    {
        richPresence = new bdRichPresenceService(m_taskManager, context);
        m_richPresenceManagers.put(context, richPresence);
    }
    m_richPresenceManagers.get(context, richPresence);
    return richPresence;
}




































































































inline bdTencent * bdLobbyService::getTencent()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_tencent == NULL)
        {






            m_tencent = new bdTencent(m_taskManager);
        }
        return m_tencent;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}






























































bdLinkedAccounts * bdLobbyService::getLinkedAccounts()
{
    if (getStatus() == bdLobbyConnection::BD_CONNECTED)
    {
        if (m_linkedAccounts == NULL)
        {
            m_linkedAccounts = new bdLinkedAccounts(m_taskManager);
        }
        return m_linkedAccounts;
    }
    bdLogWarn("lobby service", "Cannot get service when not connected.");
    return NULL;
}
































































































































































































bdLobbyErrorCode bdLobbyService::getErrorCode()
{
    return m_errorCode;
}






























































































bdLobbyService::bdLobbyService(const bdNChar8 * lsgAddress, const bdPort lsgPort)
    : bdLobbyConnectionListener(), m_taskManager(NULL), m_services(4, 0.75f), m_pushHandlers(4, 0.75f), m_profiles(NULL), m_messaging(NULL), m_matchmakings(4, 0.75f),
      m_statsManager(NULL), m_friendsManager(NULL), m_contentUnlockManager(NULL), m_titleUtilitiesManager(NULL), m_eventLogManager(NULL), m_keyArchive(NULL),
      m_counter(NULL), m_group(NULL), m_contentStreamingManager(NULL), m_pooledStorageManager(NULL), m_tags(NULL), m_voteRankManager(NULL), m_twitch(NULL),
      m_twitter(NULL), m_facebook(NULL), m_linkCode(NULL), m_antiCheat(NULL), m_DML(NULL), m_objectStore(NULL), m_achievementsEngineService(NULL),
      m_abTesting(NULL), m_umbrella(NULL), m_uno(NULL), m_userGroups(NULL), m_presence(NULL), m_commerce(NULL), m_subscription(NULL), m_featureBan(NULL),
      m_tencent(NULL), m_CODO(NULL), m_CODOTeamMarketplace(NULL), m_league(NULL), m_relayService(NULL), m_linkedAccounts(NULL), m_voting(NULL),
      m_userBlockList(NULL), m_marketingCommsManagers(4, 0.75f), m_DDL(NULL), m_serverInventory(NULL), m_redeemableCodeServiceManagers(4, 0.75f),
      m_matchScheduler(NULL), m_mlg(NULL), m_marketplaceManagers(4, 0.75f), m_rewardManagers(4, 0.75f), m_publisherVariablesManagers(4, 0.75f),
      m_storageManagers(4, 0.75f), m_teamsManagers(4, 0.75f), m_teamShowcaseManagers(4, 0.75f), m_mailManagers(4, 0.75f), m_linkCodeManagers(4, 0.75f),
      m_richPresenceManagers(4, 0.75f), m_playerVoteManagers(4, 0.75f), m_LSGLookup(0x10000), m_LSGAddress(lsgAddress), m_LSGPort(lsgPort),
      m_resolvedLSGAddress(), m_isResolved(false), m_titleID(0), m_eventHandler(NULL), m_lobbyConnection(), m_lobbyConnectionEstablished(false),
      m_authInfo(), m_errorCode(BD_NO_ERROR), m_markTasksThrottled(false), m_uploadThrottled(false), m_uploadThrottleBytesPerMs(100),
      m_uploadThrottleBucketSize(10000)
{
    m_LSGLookup.start(m_LSGAddress, bdGetHostByNameConfig());
}



























































bdLobbyService::~bdLobbyService()
{
    cleanup();
}

void bdLobbyService::onConnect(bdLobbyConnectionRef connection)
{
    m_lobbyConnectionEstablished = false;





    m_taskManager = new bdRemoteTaskManager(connection, m_encryptedConnection);
    bdLogInfo("lobby service", "Connected to lobby service.");
}

void bdLobbyService::onDisconnect(bdLobbyConnectionRef)
{
    bdLogInfo("lobby service", "Disconnected from lobby service.");
}

void bdLobbyService::cleanup()
{


    if (m_taskManager)
    {
        delete m_taskManager;
        m_taskManager = NULL;
    }

    bdHashMap<bdString, bdService *, bdHashingClass>::Iterator servIter;
    for (servIter = m_services.getIterator(); servIter; m_services.next(servIter))
    {
        bdService * s = m_services.getValue(servIter);
        delete s;
    }
    m_services.clear();

    m_pushHandlers.clear();

    if (m_commerce)
    {
        delete m_commerce;
        m_commerce = NULL;
    }
    if (m_profiles)
    {
        delete m_profiles;
        m_profiles = NULL;
    }
    if (m_messaging)
    {
        delete m_messaging;
        m_messaging = NULL;
    }







    bdHashMap<bdString, bdMatchMaking *, bdHashingClass>::Iterator it;
    for (it = m_matchmakings.getIterator(); it; m_matchmakings.next(it))
    {
        delete m_matchmakings.getValue(it);
    }
    m_matchmakings.clear();


    if (m_statsManager)
    {
        delete m_statsManager;
        m_statsManager = NULL;
    }

    if (m_friendsManager)
    {
        delete m_friendsManager;
        m_friendsManager = NULL;
    }

    if (m_contentUnlockManager)
    {
        delete m_contentUnlockManager;
        m_contentUnlockManager = NULL;
    }

    if (m_titleUtilitiesManager)
    {
        delete m_titleUtilitiesManager;
        m_titleUtilitiesManager = NULL;
    }

    if (m_eventLogManager)
    {
        delete m_eventLogManager;
        m_eventLogManager = NULL;
    }

    if (m_keyArchive)
    {
        delete m_keyArchive;
        m_keyArchive = NULL;
    }

    if (m_counter)
    {
        delete m_counter;
        m_counter = NULL;
    }

    if (m_group)
    {
        delete m_group;
        m_group = NULL;
    }

    if (m_contentStreamingManager)
    {
        delete m_contentStreamingManager;
        m_contentStreamingManager = NULL;
    }

    if (m_pooledStorageManager)
    {
        delete m_pooledStorageManager;
        m_pooledStorageManager = NULL;
    }








    if (m_tags)
    {
        delete m_tags;
        m_tags = NULL;
    }
    if (m_voteRankManager)
    {
        delete m_voteRankManager;
        m_voteRankManager = NULL;
    }
    if (m_twitch)
    {
        delete m_twitch;
        m_twitch = NULL;
    }
    if (m_twitter)
    {
        delete m_twitter;
        m_twitter = NULL;
    }
    if (m_facebook)
    {
        delete m_facebook;
        m_facebook = NULL;
    }














    if (m_linkCode)
    {
        delete m_linkCode;
        m_linkCode = NULL;
    }
    if (m_antiCheat)
    {
        delete m_antiCheat;
        m_antiCheat = NULL;
    }
    if (m_DML)
    {
        delete m_DML;
        m_DML = NULL;
    }
    if (m_objectStore)
    {
        delete m_objectStore;
        m_objectStore = NULL;
    }
    if (m_achievementsEngineService)
    {
        delete m_achievementsEngineService;
        m_achievementsEngineService = NULL;
    }
    if (m_abTesting)
    {
        delete m_abTesting;
        m_abTesting = NULL;
    }
    if (m_umbrella)
    {
        delete m_umbrella;
        m_umbrella = NULL;
    }
    if (m_uno)
    {
        delete m_uno;
        m_uno = NULL;
    }
    if (m_userGroups)
    {
        delete m_userGroups;
        m_userGroups = NULL;
    }
    if (m_presence)
    {
        delete m_presence;
        m_presence = NULL;
    }
    if (m_subscription)
    {
        delete m_subscription;
        m_subscription = NULL;
    }
    if (m_featureBan)
    {
        delete m_featureBan;
        m_featureBan = NULL;
    }
    if (m_tencent)
    {
        delete m_tencent;
        m_tencent = NULL;
    }
    if (m_CODO)
    {
        delete m_CODO;
        m_CODO = NULL;
    }
    if (m_CODOTeamMarketplace)
    {
        delete m_CODOTeamMarketplace;
        m_CODOTeamMarketplace = NULL;
    }
    if (m_league)
    {
        delete m_league;
        m_league = NULL;
    }
    if (m_relayService)
    {
        delete m_relayService;
        m_relayService = NULL;
    }
    if (m_linkedAccounts)
    {
        delete m_linkedAccounts;
        m_linkedAccounts = NULL;
    }
    if (m_voting)
    {
        delete m_voting;
        m_voting = NULL;
    }
    if (m_userBlockList)
    {
        delete m_userBlockList;
        m_userBlockList = NULL;
    }
    if (m_DDL)
    {
        delete m_DDL;
        m_DDL = NULL;
    }
    if (m_serverInventory)
    {
        delete m_serverInventory;
        m_serverInventory = NULL;
    }
    if (m_matchScheduler)
    {
        delete m_matchScheduler;
        m_matchScheduler = NULL;
    }
    if (m_mlg)
    {
        delete m_mlg;
        m_mlg = NULL;
    }

    bdHashMap<bdString, bdRedeemableCodeService *, bdHashingClass>::Iterator redeemableCodeServiceIt = m_redeemableCodeServiceManagers.getIterator();
    while (redeemableCodeServiceIt)
    {
        delete m_redeemableCodeServiceManagers.getValue(redeemableCodeServiceIt);
        m_redeemableCodeServiceManagers.next(redeemableCodeServiceIt);
    }
    m_redeemableCodeServiceManagers.clear();

    bdHashMap<bdString, bdMarketingComms *, bdHashingClass>::Iterator marketingCommsIt = m_marketingCommsManagers.getIterator();
    while (marketingCommsIt)
    {
        delete m_marketingCommsManagers.getValue(marketingCommsIt);
        m_marketingCommsManagers.next(marketingCommsIt);
    }
    m_marketingCommsManagers.clear();

    bdHashMap<bdString, bdMarketplace *, bdHashingClass>::Iterator marketplaceIt = m_marketplaceManagers.getIterator();
    while (marketplaceIt)
    {
        delete m_marketplaceManagers.getValue(marketplaceIt);
        m_marketplaceManagers.next(marketplaceIt);
    }
    m_marketplaceManagers.clear();

    bdHashMap<bdString, bdReward *, bdHashingClass>::Iterator rewardIt = m_rewardManagers.getIterator();
    while (rewardIt)
    {
        delete m_rewardManagers.getValue(rewardIt);
        m_rewardManagers.next(rewardIt);
    }
    m_rewardManagers.clear();











    bdHashMap<bdString, bdStorage *, bdHashingClass>::Iterator storageIt = m_storageManagers.getIterator();
    while (storageIt)
    {
        delete m_storageManagers.getValue(storageIt);
        m_storageManagers.next(storageIt);
    }
    m_storageManagers.clear();

    bdHashMap<bdString, bdTeams *, bdHashingClass>::Iterator teamsIt = m_teamsManagers.getIterator();
    while (teamsIt)
    {
        delete m_teamsManagers.getValue(teamsIt);
        m_teamsManagers.next(teamsIt);
    }
    m_teamsManagers.clear();

    bdHashMap<bdString, bdTeamShowcase *, bdHashingClass>::Iterator teamShowcaseIt = m_teamShowcaseManagers.getIterator();
    while (teamShowcaseIt)
    {
        delete m_teamShowcaseManagers.getValue(teamShowcaseIt);
        m_teamShowcaseManagers.next(teamShowcaseIt);
    }
    m_teamShowcaseManagers.clear();

    bdHashMap<bdString, bdMail *, bdHashingClass>::Iterator mailIt = m_mailManagers.getIterator();
    while (mailIt)
    {
        delete m_mailManagers.getValue(mailIt);
        m_mailManagers.next(mailIt);
    }
    m_mailManagers.clear();

    bdHashMap<bdString, bdLinkCode *, bdHashingClass>::Iterator linkcodeIt = m_linkCodeManagers.getIterator();
    while (linkcodeIt)
    {
        delete m_linkCodeManagers.getValue(linkcodeIt);
        m_linkCodeManagers.next(linkcodeIt);
    }
    m_linkCodeManagers.clear();

    bdHashMap<bdString, bdPublisherVariables *, bdHashingClass>::Iterator publisherVariablesIt = m_publisherVariablesManagers.getIterator();
    while (publisherVariablesIt)
    {
        delete m_publisherVariablesManagers.getValue(publisherVariablesIt);
        m_publisherVariablesManagers.next(publisherVariablesIt);
    }
    m_publisherVariablesManagers.clear();

    bdHashMap<bdString, bdRichPresenceService *, bdHashingClass>::Iterator richPresenceServiceIt = m_richPresenceManagers.getIterator();
    while (richPresenceServiceIt)
    {
        delete m_richPresenceManagers.getValue(richPresenceServiceIt);
        m_richPresenceManagers.next(richPresenceServiceIt);
    }
    m_richPresenceManagers.clear();

    bdHashMap<bdString, bdPlayerVote *, bdHashingClass>::Iterator playerVoteIt = m_playerVoteManagers.getIterator();
    while (playerVoteIt)
    {
        delete m_playerVoteManagers.getValue(playerVoteIt);
        m_playerVoteManagers.next(playerVoteIt);
    }
    m_playerVoteManagers.clear();

    m_lobbyConnection = NULL;
    m_lobbyConnectionEstablished = false;
}
// bdLobbyService.h:26/61 only forward-declare bdPushMessageHandler and the DWARF has a declaration only (limited debug
// info: its vtable is emitted elsewhere). INFERENCE: handlePushMessage calls vtable slot 3 (+0x18) with (msgType, message) at +0x29c; slot 2 and every name here are inferred.
// info: its vtable is emitted elsewhere). INFERENCE: handlePushMessage calls vtable slot 3 (+0x18) with (eventType, message) at +0x29c; slot 2 and every name here are inferred.
class bdPushMessageHandler { public: virtual ~bdPushMessageHandler(); virtual bdEventType getEventType() = 0; virtual bdBool handlePushMessage(bdEventType eventType, bdByteBufferRef message) = 0; };
void bdLobbyService::handlePushMessage(bdByteBufferRef message)
{
    bdLogInfo("lobby service", "Received push message.");  // text inferred; release logging keeps only format arguments

    bdUInt msgType = 0;
    bdBool ok = message->readUInt32(msgType);
    if (!ok)
    {
        bdLogWarn("lobby service", "Failed to read push message type.");  // text inferred
        return;
    }


    bdEventType eventType = static_cast<bdEventType>(msgType);
    if (eventType == BD_ASYNCHRONOUS_RESULT)
    {
        m_taskManager->handleAsyncResult(message);
        return;
    }
    if (eventType == BD_EVENT_LOG_FILTERED_CATEGORIES)
    {
        ok = handleEventLogFilteredCategories(message);
        if(!ok)
        {
            bdLogWarn("lobby service", "Failed to handle push message %u.", eventType);  // text inferred
        }
        return;
    }

    bdPushMessageHandler* handler = NULL;
    if (m_pushHandlers.get(eventType, handler))
    {
        handler->handlePushMessage(eventType, message);
        return;
    }


    if (!m_eventHandler)
    {
        bdLogWarn("lobby service", "No event handler for push message %u.", eventType);  // text inferred
        return;
    }



    bdLogInfo("lobby service", "Handling push message %u.", eventType);  // text inferred

    switch (eventType)
    {
        case BD_NEW_NOTIFICATION:
            m_eventHandler->onNewNotification();
            break;
        case BD_FRIENDSHIP_PROPOSAL:
            m_eventHandler->onFriendshipProposal();
            break;
        case BD_TEAM_PROPOSAL:
            m_eventHandler->onTeamProposal();
            break;
        case BD_FRIEND_CONNECTED:
        case BD_FRIEND_DISCONNECTED:
            ok = handleFriendDisconnected(message, eventType);
            break;
        case BD_TEAMMEMBER_CONNECTED:
        case BD_TEAMMEMBER_DISCONNECTED:
            ok = handleTeamMemberConnection(message, eventType);
            break;
        case BD_SESSION_INVITATION:
            ok = handleSessionInvitation(message);
            break;
        case BD_CHANNEL_CHAT_BROADCAST_MSG:
        case BD_CHANNEL_CHAT_WHISPER_MSG:
            ok = handleChannelChatMsg(message);
            break;
        case BD_CHANNEL_USER_SUBSCRIBED:
        case BD_CHANNEL_USER_UNSUBSCRIBED:
            ok = handleChannelUserSubscription(message, eventType);
            break;
        case BD_CHANNEL_USER_PROMOTED:
            ok = handleChannelUserPromoted(message);
            break;
        case BD_CHANNEL_USER_KICKED:
            ok = handleChannelUserKicked(message);
            break;
        case BD_FRIEND_CHAT_MSG:
            ok = handleFriendChatMsg(message);
            break;
        case BD_TEAM_CHAT_MSG:
            ok = handleTeamChatMsg(message);
            break;
        case BD_FRIEND_RICH_PRESENCE_UPDATED:
            ok = handleFriendRichPresenceUpdated(message);
            break;
        case BD_NOTIFY_LEAVE:
            ok = handleNotifyLeave(message);
            break;
        case BD_NEW_MAIL:
            ok = handleNewMail();
            break;
        case BD_CHALLENGES_RECEIVED:
            ok = handleChallengesReceived(message);
            break;
        case BD_MULTIPLE_LOGONS:
            ok = handleMultipleLogons(message);
            break;
        case BD_PLAYER_DISCONNECTED:
            ok = handlePlayerDisconnected(message);
            break;
        case BD_PLAYER_BANNED:
            ok = handlePlayerBanned(message);
            break;
        case BD_CONSOLE_BANNED:
            ok = handleConsoleBanned(message);
            break;
        case BD_MULTIPLE_LINKED_ACCOUNT_LOGONS:
            ok = handleMultipleLinkedAccountLogons(message);
            break;
        case BD_LINKED_ACCOUNT_STATUS_CHANGE:
            ok = handleLinkedAccountStatusChange(message);
            break;
        case BD_NOT_WHITE_LISTED:
            ok = handleNotWhiteListed(message);
            break;
        case BD_NOT_WHITE_LISTED_WITH_MESSAGE:
            ok = handleNotWhiteListedWithMessage(message);
            break;
        case BD_GLOBAL_INSTANT_MESSAGE:
            ok = handleGlobalInstantMessage(message);
            break;
        case BD_FEATURE_BAN:
            ok = handleFeatureBan(message);
            break;
        case BD_TOTP_CHALLENGE:
            ok = handleTOTPMessage(message);
            break;
        case BD_GMSG_GROUP_MESSAGE:
        case BD_GMSG_BROADCAST:
            ok = handleGMSGBroadcast(eventType, message);
            break;
        case BD_TENCENT_AAS_RECORD:
            ok = handleTencentAASRecord(message);
            break;
        case BD_TENCENT_ANTIBOT_DATA:
            ok = handleTencentAntiBotData(message);
            break;
        case BD_TENCENT_ANTIBOT_PUNISH:
            ok = handleTencentAntiBotPunish(message);
            break;
        case BD_TENCENT_ANTIBOT_SERVER_READY:
            ok = handleTencentAntibotServerReady(message);
            break;
        case BD_TENCENT_USER_LEAVE_REASON:
            ok = handleTencentUserLeaveReason(message);
            break;
        case BD_TENCENT_LOUDSPEAKER_MESSAGE:
            ok = handleTencentLoudSpeakerMessage(message);
            break;
        case BD_CHANNEL_USER_MUTED:
            ok = handleChannelUserMuted(message);
            break;
        case BD_STABILISED:
            ok = handleStabilised(message);
            break;
        case BD_TENCENT_REWARD:
            ok = handleTencentReward(message);
            break;
        case BD_TENCENT_NO_REWARD:
            ok = handleTencentNoRewardReceived(message);
            break;
        case BD_TENCENT_SECURITY_RATING:
            ok = handleTSSResponse(message);
            break;
        case BD_MARKETPLACE_COUPONS_GRANTED:
            ok = handleMarketplaceCouponsGranted(message);
            break;
        case BD_MARKETPLACE_ITEMS_EXPIRED:
            ok = handleMarketplaceItemsExpired(message);
            break;
        case BD_MARKETPLACE_ITEMS_UPDATED:
            ok = handleMarketplaceItemsUpdated(message);
            break;
        case BD_MARKETPLACE_COUPONS_UPDATED:
            ok = handleMarketplaceCouponsUpdated(message);
            break;
        case BD_MARKETPLACE_COUPONS_UPDATED_V2:
            ok = handleMarketplaceCouponsUpdatedV2(message);
            break;
        case BD_MARKETPLACE_COUPONS_UPDATED_V3:
            ok = handleMarketplaceCouponsUpdatedV3(message);
            break;
        case BD_MARKETPLACE_COUPONS_UPDATED_V4:
            ok = handleMarketplaceCouponsUpdatedV4(message);
            break;
        case BD_MARKETPLACE_BALANCE_UPDATED:
            ok = handleMarketplaceBalanceUpdated(message);
            break;
        case BD_REWARD_ACHIEVEMENT_CLAIMED:
            ok = handleClaimAchievement(message);
            break;
        case BD_MARKETPLACE_DEPOSIT_GRANTED:
            ok = handleMarketplaceDepositGranted(message);
            break;
        case BD_MARKETPLACE_ITEMS_GRANTED:
            ok = handleMarketplaceItemsGranted(message);
            break;
        case BD_MARKETPLACE_ITEMS_GRANTED_WITH_INVENTORY_QUANTITY:
            ok = handleMarketplaceItemsGrantedWithInventoryQuantity(message);
            break;
        case BD_MARKETPLACE_ENTITLEMENTS_GRANTED:
            ok = handleMarketplaceEntitlementsGranted(message);
            break;
        case BD_STORAGE_WEBSERVICE_WRITE:
            ok = handleWebserviceStorageFileWrite(message);
            break;
        case BD_REWARD_EVENT_MESSAGE:
            ok = handleRewardEventMessage(message);
            break;
        case BD_REWARD_ACHIEVEMENT_MESSAGE:
            ok = handleRewardAchievementMessage(message);
            break;
        case BD_ACHIEVEMENTS_UPDATED:
            ok = handleAchievementsUpdated(message);
            break;
        case BD_TENCENT_NAME_CHANGED:
            ok = handleTencentNameChanged(message);
            break;
        case BD_PUBLISHER_VARIABLES_UPDATE_MESSAGE:
            ok = handlePublisherVariablesUpdateMessage(message);
            break;
        case BD_USER_PRIVATE_PROFILE_UPDATED:
            ok = handlePrivateProfileUpdate(message);
            break;
        case BD_TEAM_MEMBER_RICH_PRESENCE_UPDATED:
            ok = handleTeamMemberRichPresenceUpdated(message);
            break;
        case BD_TEAM_MEMBER_USER_NAME_UPDATED:
            ok = handleTeamMemberUserNameUpdated(message);
            break;
        case BD_TITLE_VERSION_DISABLED:
            ok = handleTitleVersionDisabled(message);
            break;

        case BD_ASYNCHRONOUS_RESULT:  // INFERENCE: jump-table entry 20 goes straight to the exit (+0xc0), not to default
            break;
        case BD_MARKETING_COMMS_ASSIGNMENTS_AVAILABLE:
            ok = handleMarketingCommsAssignmentsAvailable();
            break;
        case BD_CODO_TEAM_MARKETPLACE_LEVEL_CHANGED:
            ok = handleCODOTeamMarketplaceLevelChange(message);
            break;
        case BD_SUBSCRIBED_RICH_PRESENCE_UPDATED:
            ok = handleSubscribedRichPresenceUpdated(message);
            break;
        case BD_PLAYER_LOGON_TIME_PROHIBITED:
            ok = handleLogonTimeProhibited(message);
            break;
        case BD_PLAYER_LOGON_TIME_PROHIBITED_WARNING:
            ok = handleLogonTimeProhibitedWarning(message);
            break;

        default:
            ok = false;  // INFERENCE: default enters the log at +0x25e0 directly (the if (!ok) test below folded)
            break;
    }
    if (!ok)
    {
        bdLogWarn("lobby service", "Failed to handle push message %u.", eventType);  // text inferred
    }
}




bdBool bdLobbyService::handleFriendDisconnected(bdByteBufferRef message, bdEventType eventType)
{
    bdUInt64 friendUID = 0;
    bdBool ok = message->readUInt64(friendUID);

    bdNChar8 friendName[65] = {0};
    ok = ok && message->readString(friendName, 64);

    if (ok)
    {
        bdUByte8 status = (eventType == BD_FRIEND_CONNECTED);
        m_eventHandler->onPlayerStatusUpdate(friendUID, friendName, status);
    }
    return ok;
}



bdBool bdLobbyService::handleTeamMemberConnection(bdByteBufferRef message, bdEventType eventType)
{
    bdUInt64 senderUID = 0;
    bdBool ok = message->readUInt64(senderUID);
    bdNChar8 senderName[65] = {0};
    ok = ok && message->readString(senderName, 64);
    bdTeamID teamID = 0;
    ok = ok && message->readUInt64(teamID);
    bdNChar8 teamName[65] = {0};
    ok = ok && message->readString(teamName, 64);
    if (ok)
    {
        bdUByte8 status = eventType == BD_TEAMMEMBER_CONNECTED;
        m_eventHandler->onTeamMemberStatusUpdate(senderUID, senderName, teamID, teamName, status);
    }
    return ok;
}






bdBool bdLobbyService::handleSessionInvitation(bdByteBufferRef message)
{
    bdSessionInvite invite;
    bdBool ok = invite.deserialize(message);
    if (ok)
    {
        m_eventHandler->onSessionInvite(invite);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read session invitation.");  // text inferred
    }
    return ok;
}




bdBool bdLobbyService::handleChannelChatMsg(bdByteBufferRef message)
{
    bdUInt64 senderUID = 0;
    bdBool ok = message->readUInt64(senderUID);
    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));
    bdUInt64 channelID = 0;
    ok = ok && message->readUInt64(channelID);
    bdUByte8 msg[1024] = {0};
    bdUInt32 msgLength = sizeof(msg);
    ok = ok && message->readBlob(msg, msgLength);
    bdUInt32 messageType = 0;
    ok = ok && message->readUInt32(messageType);
    if (ok)
    {
        m_eventHandler->onChatChannelMessage(channelID, senderUID, senderName, messageType, msg, msgLength);
    }
    return ok;
}








bdBool bdLobbyService::handleChannelUserSubscription(bdByteBufferRef message, bdEventType eventType)
{
    bdChannelMember member;
    bdChannelInfo channel;
    bdBool ok = message->readUInt64(member.m_userID);
    ok = ok && message->readString(member.m_userName, sizeof(member.m_userName));
    ok = ok && message->readUInt64(channel.m_channelID);
    bdUByte8 status = eventType == BD_CHANNEL_USER_SUBSCRIBED;
    bdUByte8 versionField = 0;
    if (ok)
    {
        bdBool hasVersionField = message->readUByte8(versionField);
        if (hasVersionField && versionField > 1)
        {
            if (!message->readUInt32(member.m_subscribedTimestamp) || !channel.deserialize(message))
            {
                ok = false;
            }
        }
        if (ok)
        {
            m_eventHandler->onChatChannelUpdate(channel, member, status);
        }
    }
    return ok;
}






bdBool bdLobbyService::handleChannelUserPromoted(bdByteBufferRef message)
{
    bdChannelInfo channel;
    bdChannelMember previousAdmin;
    bdChannelMember newAdmin;
    bdUByte8 version = 0;
    bdBool ok = message->readUByte8(version);
    if (ok && version > 1)
    {
        ok = ok && channel.deserialize(message);
        ok = ok && previousAdmin.deserialize(message);
        ok = ok && newAdmin.deserialize(message);
        if (ok)
        {
            m_eventHandler->onChatChannelUserPromoted(channel, previousAdmin, newAdmin);
        }
    }
    else
    {
        bdLogWarn("lobby service", "Unsupported channel user promoted version.");
    }
    return ok;
}




bdBool bdLobbyService::handleChannelUserKicked(bdByteBufferRef message)
{
    bdChannelInfo channel;
    bdChannelMember kickedBy;
    bdChannelMember kickedMember;
    bdUInt32 kickReason = 0;
    bdBool wasAddedToBanlist = false;
    bdUByte8 versionNumber = 0;
    bdBool ok = message->readUByte8(versionNumber);
    if (ok && versionNumber > 1)
    {
        ok = ok && channel.deserialize(message);
        ok = ok && kickedBy.deserialize(message);
        ok = ok && kickedMember.deserialize(message);
        ok = ok && message->readUInt32(kickReason);
        ok = ok && message->readBool(wasAddedToBanlist);
        if (ok)
        {
            m_eventHandler->onChatChannelUserKicked(channel, kickedBy, kickedMember, kickReason, wasAddedToBanlist);
            m_eventHandler->onChatChannelUpdate(channel, kickedMember, 0);
        }
    }
    else
    {
        bdLogWarn("lobby service", "Unsupported channel user kicked version.");
    }
    return ok;
}






bdBool bdLobbyService::handleFriendChatMsg(bdByteBufferRef message)
{
    bdUInt64 senderID = 0;
    bdBool ok = message->readUInt64(senderID);
    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));

    bdUByte8 msg[1024] = {0};
    bdUInt32 msgLength = sizeof(msg);
    ok = ok && message->readBlob(msg, msgLength);

    if (ok)
    {
        m_eventHandler->onInstantMessage(senderID, senderName, msg, msgLength, 0);
    }
    return ok;
}



bdBool bdLobbyService::handleTeamChatMsg(bdByteBufferRef message)
{
    bdUInt64 senderID = 0;
    bdBool ok = message->readUInt64(senderID);
    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));
    bdTeamID teamID = 0;
    ok = ok && message->readUInt64(teamID);
    bdNChar8 teamName[65] = {0};
    ok = ok && message->readString(teamName, 64);
    bdUByte8 msg[1024] = {0};
    bdUInt32 msgLength = sizeof(msg);
    ok = ok && message->readBlob(msg, msgLength);
    if (ok)
    {
        m_eventHandler->onInstantMessage(senderID, senderName, msg, msgLength, teamID);
    }
    return ok;
}






bdBool bdLobbyService::handleFriendRichPresenceUpdated(bdByteBufferRef message)
{
    bdUInt64 senderID = 0;
    bdBool ok = message->readUInt64(senderID);
    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));
    bdUByte8 richPresence[1024];
    bdUInt richPresenceSize = sizeof(richPresence);
    ok = ok && message->readBlob(richPresence, richPresenceSize);

    if (ok)
    {
        m_eventHandler->onRichPresenceUpdate(senderID, senderName, richPresence, richPresenceSize, 0);
    }
    return ok;
}


bdBool bdLobbyService::handleNotifyLeave(bdByteBufferRef message)
{
    bdOnlineUserID userID = 0;
    bdBool ok = message->readUInt64(userID);
    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));
    bdSessionID sessionID;
    bdUInt length = sizeof(sessionID.m_sessionID.ab);
    ok = ok && message->readBlob(sessionID.m_sessionID.ab, length);
    if (!ok)
    {
        bdLogWarn("lobby service", "Failed to read notify leave message.");
        return false;
    }





    m_eventHandler->onNotifyLeave(userID, sessionID);
    return true;
}
bdBool bdLobbyService::handleNewMail()
{
    m_eventHandler->onNewMail();
    return true;
}


bdBool bdLobbyService::handleChallengesReceived(bdByteBufferRef message)
{
    bdOnlineUserID userID = 0;
    bdBool ok = message->readUInt64(userID);

    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));

    bdAntiCheatChallengesRef challenges(new bdAntiCheatChallenges());
    ok = ok && challenges->deserialize(message);

    if (ok)
    {
        m_eventHandler->onChallengesReceived(userID, challenges);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read challenges.");
    }
    return ok;
}


bdBool bdLobbyService::handleMultipleLogons(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdBool ok = message->readUInt64(recipientID);
    if (ok)
    {
        m_eventHandler->onMultipleLogon(recipientID);
    }
    return ok;
}


bdBool bdLobbyService::handlePlayerDisconnected(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdUInt32 intReason = 0;
    bdBool ok = message->readUInt64(recipientID);
    ok = ok && message->readUInt32(intReason);
    bdAntiCheat::bdAntiCheatReason reason = static_cast<bdAntiCheat::bdAntiCheatReason>(intReason);

    if (ok)
    {
        m_eventHandler->onPlayerDisconnected(recipientID, reason);
    }
    return ok;
}


bdBool bdLobbyService::handlePlayerBanned(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdUInt32 reputationLevel = 0;
    bdInt64 expiry = -1;
    bdUInt32 reason = 0;
    bdBool ok = message->readUInt64(recipientID);
    ok = ok && message->readUInt32(reputationLevel);
    ok = ok && message->readInt64(expiry);
    ok = ok && message->readUInt32(reason);

    if (ok)
    {
        m_eventHandler->onPlayerBanned(recipientID, reputationLevel, expiry, reason);
    }
    return ok;
}


bdBool bdLobbyService::handleConsoleBanned(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdBool ok = message->readUInt64(recipientID);

    if (ok)
    {
        m_eventHandler->onConsoleBanned(recipientID);
    }
    return ok;
}


bdBool bdLobbyService::handleMultipleLinkedAccountLogons(bdByteBufferRef message)
{
    bdUserAccountID userAccountID;
    bdBool ok = ContextSerialization::readUserID(message, userAccountID);
    if (ok)
    {
        m_eventHandler->onMultipleLinkedAccountLogons(userAccountID);
    }
    return ok;
}




bdBool bdLobbyService::handleLinkedAccountStatusChange(bdByteBufferRef message)
{
    bdUserAccountID userAccountID;
    bdBool ok = ContextSerialization::readUserID(message, userAccountID);
    if (ok)
    {
        m_eventHandler->onLinkedAccountStatusChange(userAccountID);
    }
    return ok;
}



bdBool bdLobbyService::handleNotWhiteListed(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdBool ok = message->readUInt64(recipientID);

    if (ok)
    {
        m_eventHandler->onNotWhiteListed(recipientID);
    }
    return ok;
}


bdBool bdLobbyService::handleNotWhiteListedWithMessage(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdBool ok = message->readUInt64(recipientID);
    bdNChar8 msg[141] = {0};
    ok = ok && message->readString(msg, 140);

    if (ok)
    {
        m_eventHandler->onNotWhiteListedWithMessage(recipientID, msg);
    }
    return ok;
}


bdBool bdLobbyService::handleTOTPMessage(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdBool ok = message->readUInt64(recipientID);

    if (ok)
    {
        m_eventHandler->onTOTPMessage(recipientID);
    }
    return ok;
}


bdBool bdLobbyService::handleGlobalInstantMessage(bdByteBufferRef message)
{
    bdUInt64 senderUID = 0;
    bdBool ok = message->readUInt64(senderUID);

    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));

    bdUByte8 msg[4096] = {0};
    bdUInt32 msgLength = sizeof(msg);
    ok = ok && message->readBlob(msg, msgLength);

    if (ok)
    {
        m_eventHandler->onGlobalInstantMessage(senderUID, senderName, msg, msgLength);
    }
    return ok;
}


bdBool bdLobbyService::handleFeatureBan(bdByteBufferRef message)
{
    bdUByte8 version = 0;
    bdBool ok = message->readUByte8(version);
    bdUInt32 numBans = 0;
    ok = ok && message->readUInt32(numBans);
    if (version == 1)
    {
        for (bdUInt32 i = 0; i < numBans && ok; ++i)
        {
            bdFeatureBanInfo ban;
            ok = ok && ban.deserialize(message);
            if (ok)
            {
                m_eventHandler->onFeatureBan(ban);
            }
        }
    }
    else
    {
        bdLogWarn("lobby service", "Unknown feature ban version.");
    }
    return ok;
}






bdBool bdLobbyService::handleGMSGBroadcast(bdEventType eventType, bdByteBufferRef message)
{
    bdOnlineUserID senderUID = 0;
    bdNChar8 senderName[64];
    senderName[0] = '\0';
    bdUByte8 msg[1024];
    bdUInt32 msgLength = sizeof(msg);

    bdBool ok = message->readUInt64(senderUID);
    ok = ok && message->readString(senderName, sizeof(senderName));
    ok = ok && message->readBlob(msg, msgLength);
    if (ok)
    {
        if (eventType == BD_GMSG_GROUP_MESSAGE)
        {
            m_eventHandler->onGroupMessagingMessage(senderUID, senderName, msg, msgLength);
        }
        else
        {
            m_eventHandler->onBroadcastMessage(senderUID, senderName, msg, msgLength);
        }
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read GMSG broadcast.");
    }
    return ok;
}



bdBool bdLobbyService::handleTencentAASRecord(bdByteBufferRef message)
{
    bdTencentAASRecord record;
    bdBool ok = record.deserialize(message);
    if (ok)
    {
        m_eventHandler->onTencentAASRecordReceived(record);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read Tencent AAS record.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleTencentAntiBotData(bdByteBufferRef message)
{
    bdUInt32 length = 0;
    const bdUInt32 maxDataLen = 43693;
    bdNChar8 data[maxDataLen];
    bdUByte8 version = 0;
    bdByte8 decodedData[32768];

    bdBool ok = message->readUByte8(version);
    ok = ok && message->readString(data, maxDataLen);
    ok = ok && message->readUInt32(length);

    bdUInt32 b64DataLen = bdStrnlen(data, maxDataLen);
    if (ok)
    {
        if (version == 1)
        {
            bdBase64::decode(data, b64DataLen, decodedData, sizeof(decodedData));
            m_eventHandler->onTencentAntibotData(reinterpret_cast<bdNChar8 *>(decodedData), length);
        }
        else
        {
            bdLogWarn("lobby service", "Unknown antibot data version.");
        }
    }
    return ok;
}




bdBool bdLobbyService::handleTencentAntiBotPunish(bdByteBufferRef message)
{
    bdUInt32 punishMode = 0;
    bdUInt32 punishTime = 0;
    bdUInt32 punishReasonLen = 0;
    bdUInt32 punishInfoLen = 0;
    const bdUInt32 maxInfoLen = 10925;
    bdNChar8 punishReason[maxInfoLen];
    bdNChar8 punishInfo[maxInfoLen];
    bdUByte8 version = 0;
    bdByte8 decodedReason[8192];
    bdByte8 decodedInfo[8192];

    bdBool ok = message->readUByte8(version);
    ok = ok && message->readUInt32(punishMode);
    ok = ok && message->readUInt32(punishTime);
    ok = ok && message->readString(punishReason, maxInfoLen);
    ok = ok && message->readUInt32(punishReasonLen);
    ok = ok && message->readString(punishInfo, maxInfoLen);
    ok = ok && message->readUInt32(punishInfoLen);

    bdUInt32 b64ReasonLen = bdStrnlen(punishReason, maxInfoLen);
    bdUInt32 b64InfoLen = bdStrnlen(punishInfo, maxInfoLen);
    if (ok)
    {
        if (version == 1)
        {
            bdBase64::decode(punishReason, b64ReasonLen, decodedReason, sizeof(decodedReason));
            bdBase64::decode(punishInfo, b64InfoLen, decodedInfo, sizeof(decodedInfo));
            m_eventHandler->onTencentAntibotPunish(punishMode, punishTime, reinterpret_cast<bdNChar8 *>(decodedReason), punishReasonLen, reinterpret_cast<bdNChar8 *>(decodedInfo), punishInfoLen);
        }
        else
        {
            bdLogWarn("lobby service", "Unknown antibot punish version.");
        }
    }
    return ok;
}









bdBool bdLobbyService::handleTencentAntibotServerReady(bdByteBufferRef message)
{
    bdUByte8 version = 0;
    bdBool ok = message->readUByte8(version);
    bdUInt32 interval = 30;
    message->readUInt32(interval);

    bdTencent * tencent = getTencent();
    if (tencent)
    {
        tencent->setAntibotSendDataInterval(interval);
    }

    if (ok)
    {
        if (version == 1)
        {
            m_eventHandler->onTencentAntibotServerReady();
        }
        else
        {
            bdLogWarn("lobby service", "Unknown antibot server ready version.");
        }
    }
    return ok;
}







bdBool bdLobbyService::handleTencentUserLeaveReason(bdByteBufferRef message)
{
    bdOnlineUserID userID = 0;
    bdBool ok = message->readUInt64(userID);
    bdNChar8 senderName[64] = {0};
    ok = ok && message->readString(senderName, sizeof(senderName));
    bdSessionID sessionID;
    bdUInt length = sizeof(sessionID.m_sessionID.ab);
    ok = ok && message->readBlob(sessionID.m_sessionID.ab, length);
    bdUInt32 reason = 0;
    ok = ok && message->readUInt32(reason);
    if (!ok)
    {
        bdLogWarn("lobby service", "Failed to read user leave reason message.");
        return false;
    }





    m_eventHandler->onTencentNotifyLeaveReason(userID, sessionID, reason);
    return true;
}




bdBool bdLobbyService::handleTencentLoudSpeakerMessage(bdByteBufferRef message)
{
    bdOnlineUserID senderUID = 0;
    bdNChar8 senderName[64] = {0};
    bdUInt32 speakerType = 0;
    bdUByte8 msg[4096] = {0};
    bdUInt32 msgLength = sizeof(msg);

    bdBool ok = message->readUInt64(senderUID);
    ok = ok && message->readString(senderName, sizeof(senderName));
    ok = ok && message->readUInt32(speakerType);
    ok = ok && message->readBlob(msg, msgLength);
    if (ok)
    {
        m_eventHandler->onTencentLoudSpeakerMessage(senderUID, senderName, static_cast<bdCODOLoudSpeakerType>(speakerType), msg, msgLength);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read loud speaker message.");
    }
    return ok;
}







bdBool bdLobbyService::handleChannelUserMuted(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdUInt64 unmuteTime = 0;
    bdBool ok = message->readUInt64(recipientID);
    ok = ok && message->readUInt64(unmuteTime);
    if(ok)
    {
        m_eventHandler->onPlayerMuted(recipientID, unmuteTime);
    }
    return ok;
}


bdBool bdLobbyService::handleStabilised(bdByteBufferRef message)
{
    bdOnlineUserID recipientID = 0;
    bdUInt64 stabilisationTime = 0;
    bdBool ok = message->readUInt64(recipientID);
    ok = ok && message->readUInt64(stabilisationTime);
    if(ok)
    {
        m_eventHandler->onPlayerStabilised(recipientID, stabilisationTime);
    }
    return ok;
}


bdBool bdLobbyService::handleTencentReward(bdByteBufferRef message)
{
    bdOnlineUserID playerID = 0;
    bdUInt32 eventCodeLength = 0;
    bdNChar8 eventCode[45];
    bdUInt32 msgLength = 0;
    bdUByte8 msg[512];

    bdBool ok = message->readUInt64(playerID);
    ok = ok && message->readUInt32(eventCodeLength);
    ok = ok && message->readString(eventCode, sizeof(eventCode));
    ok = ok && message->readUInt32(msgLength);
    ok = ok && message->readBlob(msg, msgLength);
    if (ok)
    {
        m_eventHandler->onTencentPlayerReward(playerID, eventCodeLength, eventCode, msgLength, msg);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read tencent reward message.");
    }
    return ok;
}


bdBool bdLobbyService::handleTencentNoRewardReceived(bdByteBufferRef message)
{
    bdOnlineUserID playerID = 0;
    bdUInt32 eventCodeLength = 0;
    bdNChar8 eventCode[45];

    bdBool ok = message->readUInt64(playerID);
    ok = ok && message->readUInt32(eventCodeLength);
    ok = ok && message->readString(eventCode, sizeof(eventCode));
    if (ok)
    {
        m_eventHandler->onTencentPlayerNoReward(playerID, eventCodeLength, eventCode);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read Tencent no-reward message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleTSSResponse(bdByteBufferRef message)
{
    bdTSSStructMessage data;
    bdBool ok = bdStructBufferDeserializerHelpers::deserializeFromBuffer(message, data);

    if (ok)
    {
        m_eventHandler->onTSSResponseReceived(data);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read TSS response.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleTencentNameChanged(bdByteBufferRef message)
{
    bdNChar8 extraData[1024] = {0};
    bdBool ok = message->readString(extraData, sizeof(extraData));
    if(ok)
    {
        m_eventHandler->onTencentUsernameChanged(extraData,
                                                 bdStrnlen(extraData, sizeof(extraData)));
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read Tencent name change.");  // text inferred
    }
    return ok;
}



bdBool bdLobbyService::handleEventLogFilteredCategories(bdByteBufferRef message)
{
    bdUInt32 numFilteredCategories = 0;
    bdUInt32 category = 0;
    bdBool ok = false;

    if (m_eventLogManager)
    {
        m_eventLogManager->resetFilteredCategories();
        ok = message->readUInt32(numFilteredCategories);
        for (bdUInt32 i = 0; i < numFilteredCategories; ++i)
        {
            ok = ok && message->readUInt32(category) && m_eventLogManager->addFilteredCategory(category);
        }
        if (ok)
        {
            m_eventLogManager->setReceivedCategoryData(true);
        }
    }
    else
    {
        bdLogWarn("lobby service", "Event log not created.");
    }
    if (!ok)
    {
        bdLogWarn("lobby service", "Failed to handle filtered categories.");
    }
    return ok;
}



bdBool bdLobbyService::handleMarketplaceCouponsGranted(bdByteBufferRef message)
{
    bdUInt32 reason;
    bdUInt32 numCoupons;
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    reason = 0;
    numCoupons = 0;
    bdNChar8 couponIDs[6][25] = {{0}};

    ContextSerialization::readUserID(message, playerID);
    bdBool ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readUInt32(reason);
    ok = ok && message->readUInt32(numCoupons);
    for (bdUInt32 i = 0; i < numCoupons; ++i)
    {
        ok = ok && message->readString(couponIDs[i], sizeof(couponIDs[i]));
    }
    if (ok)
    {
        m_eventHandler->onMarketplaceCouponsGranted(playerID, context, reason, numCoupons, couponIDs);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read coupons granted message.");
    }
    return ok;
}





bdBool bdLobbyService::handleMarketplaceItemsExpired(bdByteBufferRef message)
{
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdUInt32 reason = 0;
    bdUInt32 numItems = 0;
    bdUInt32 itemIDs[128] = {0};

    ContextSerialization::readUserID(message, playerID);
    bdBool ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readUInt32(reason);
    ok = ok && message->readUInt32(numItems);
    for (bdUInt32 i = 0; i < numItems; ++i)
    {
        ok = ok && message->readUInt32(itemIDs[i]);
    }
    if (ok)
    {
        m_eventHandler->onMarketplaceItemsExpired(playerID, context, reason, numItems, itemIDs);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read items expired message.");
    }
    return ok;
}







bdBool bdLobbyService::handleMarketplaceItemsUpdated(bdByteBufferRef message)
{
    bdMarketplaceInventory updates[6];


    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdUInt32 numUpdates = 0;


    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ContextSerialization::readContext(message, context, sizeof(context));


    ok = ok && message->readUInt32(numUpdates);
    if (numUpdates > 6)
    {
        numUpdates = 6;
        bdLogWarn("lobby service", "Too many inventory updates, truncating to %u of %u.", numUpdates, 6);  // text inferred
    }

    for (bdUInt32 i = 0; i < numUpdates; ++i)
    {
        ok = ok && updates[i].deserialize(message, playerID);
    }
    if (ok)
    {
        m_eventHandler->onInventoryItemUpdate(playerID, context, numUpdates, updates);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read inventory updates message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleMarketplaceCouponsUpdated(bdByteBufferRef message)
{
    bdMarketplaceCouponInventory updates[6];

    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdUInt32 numUpdates = 0;


    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ContextSerialization::readContext(message, context, sizeof(context));


    ok = ok && message->readUInt32(numUpdates);
    if (numUpdates > 6)
    {
        numUpdates = 6;
        bdLogWarn("lobby service", "Too many coupon updates, truncating to %u of %u.", numUpdates, 6);  // text inferred
    }

    for (bdUInt32 i = 0; i < numUpdates; ++i)
    {
        ok = ok && updates[i].deserialize(message);
    }
    if (ok)
    {
        m_eventHandler->onInventoryCouponUpdate(playerID, context, numUpdates, updates);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read coupon updates message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleMarketplaceCouponsUpdatedV2(bdByteBufferRef message)
{
    bdMarketplaceCouponInventoryV2 updates[6];

    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdUInt32 numUpdates = 0;


    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ok && ContextSerialization::readContext(message, context, sizeof(context));


    ok = ok && message->readUInt32(numUpdates);
    if (numUpdates > 6)
    {
        numUpdates = 6;
        bdLogWarn("lobby service", "Too many coupon updates, truncating to %u of %u.", numUpdates, 6);  // text inferred
    }

    for (bdUInt32 i = 0; i < numUpdates; ++i)
    {
        ok = ok && updates[i].deserialize(message);
    }
    if (ok)
    {
        m_eventHandler->onInventoryCouponUpdateV2(playerID, context, numUpdates, updates);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read coupon updates message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleMarketplaceCouponsUpdatedV3(bdByteBufferRef message)
{
    bdMarketplaceCouponInventoryV3 updates[6];

    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdUInt32 numUpdates = 0;


    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ok && ContextSerialization::readContext(message, context, sizeof(context));


    ok = ok && message->readUInt32(numUpdates);
    if (numUpdates > 6)
    {
        numUpdates = 6;
        bdLogWarn("lobby service", "Too many coupon updates, truncating to %u of %u.", numUpdates, 6);  // text inferred
    }

    for (bdUInt32 i = 0; i < numUpdates; ++i)
    {
        ok = ok && updates[i].deserialize(message);
    }
    if (ok && m_eventHandler)
    {
        m_eventHandler->onInventoryCouponUpdateV3(playerID, context, numUpdates, updates);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read coupon updates message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleMarketplaceCouponsUpdatedV4(bdByteBufferRef message)
{
    bdMarketplaceCouponInventoryV4 updates[6];

    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdUInt32 numUpdates = 0;


    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ok && ContextSerialization::readContext(message, context, sizeof(context));


    ok = ok && message->readUInt32(numUpdates);
    if (numUpdates > 6)
    {
        numUpdates = 6;
        bdLogWarn("lobby service", "Too many coupon updates, truncating to %u of %u.", numUpdates, 6);  // text inferred
    }

    for (bdUInt32 i = 0; i < numUpdates; ++i)
    {
        ok = ok && updates[i].deserialize(message);
    }
    if (ok && m_eventHandler)
    {
        m_eventHandler->onInventoryCouponUpdateV4(playerID, context, numUpdates, updates);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read coupon updates message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleMarketplaceBalanceUpdated(bdByteBufferRef message)
{
    bdUInt32 numUpdates;
    bdMarketplaceCurrency updates[6];
    bdUserAccountID playerID;
    numUpdates = 0;

    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ok && message->readUInt32(numUpdates);
    if (numUpdates > 6)
    {
        numUpdates = 6;
        bdLogWarn("lobby service", "Too many balance updates, truncating to %u of %u.", numUpdates, 6);  // text inferred
    }
    for (bdUInt32 i = 0; i < numUpdates; ++i)
    {
        ok = ok && updates[i].deserialize(message);
    }
    if (ok)
    {
        m_eventHandler->onBalanceUpdate(playerID, numUpdates, updates);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read balance updates message.");
    }
    return ok;
}






bdBool bdLobbyService::handleClaimAchievement(bdByteBufferRef message)
{
    bdMarketplaceInventory updates[32];
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdUInt32 reason = 0;
    bdUInt32 numItems = 0;


    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readUInt32(reason);

    ok = ok && message->readUInt32(numItems);
    if (numItems > 32)
    {
        numItems = 32;
        bdLogWarn("lobby service", "Too many claimed items, truncating to %u of %u.", numItems, 32);  // text inferred
    }

    for (bdUInt32 i = 0; i < numItems; ++i)
    {
        ok = ok && updates[i].deserialize(message, playerID);
    }
    if (ok)
    {
        m_eventHandler->onClaimAchievement(playerID, context, reason, numItems, updates);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read claim achievement message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleMarketplaceDepositGranted(bdByteBufferRef message)
{
    bdUInt32 numDeposits;
    bdMarketplaceCurrency deposits_granted[6];
    bdUserAccountID playerID;
    numDeposits = 0;

    bdBool ok = ContextSerialization::readUserID(message, playerID);
    ok = ok && message->readUInt32(numDeposits);
    if (numDeposits > 6)
    {
        numDeposits = 6;
        bdLogWarn("lobby service", "Too many deposits granted, truncating to %u of %u.", numDeposits, 6);
    }
    for (bdUInt32 i = 0; i < numDeposits; ++i)
    {
        ok = ok && deposits_granted[i].deserialize(message);
    }
    if (ok)
    {
        m_eventHandler->onDepositGranted(playerID, numDeposits, deposits_granted);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read deposits granted message.");
    }
    return ok;
}






bdBool bdLobbyService::handleMarketplaceItemsGranted(bdByteBufferRef message)
{
    bdUInt32 numItems;
    bdUInt32 items_granted[6];
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    numItems = 0;

    ContextSerialization::readUserID(message, playerID);
    bdBool ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readUInt32(numItems);
    if (numItems > 6)
    {
        numItems = 6;
        bdLogWarn("lobby service", "Too many items granted, truncating to %u of %u.", numItems, 6);
    }
    for (bdUInt32 i = 0; i < numItems; ++i)
    {
        ok = ok && message->readUInt32(items_granted[i]);
    }
    if (ok)
    {
        m_eventHandler->onItemsGranted(playerID, context, numItems, items_granted);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read items granted message.");
    }
    return ok;
}







bdBool bdLobbyService::handleMarketplaceItemsGrantedWithInventoryQuantity(bdByteBufferRef message)
{
    bdUInt32 numItems;
    bdUInt32 items_granted[6];
    bdUInt32 item_quantities[6];
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    numItems = 0;

    ContextSerialization::readUserID(message, playerID);
    bdBool ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readUInt32(numItems);
    if (numItems > 6)
    {
        numItems = 6;
        bdLogWarn("lobby service", "Too many items granted, truncating to %u of %u.", numItems, 6);
    }
    for (bdUInt32 i = 0; i < numItems; ++i)
    {
        ok = ok && message->readUInt32(items_granted[i]);
        ok = ok && message->readUInt32(item_quantities[i]);
    }
    if (ok)
    {
        m_eventHandler->onItemsGranted(playerID, context, numItems, items_granted, item_quantities);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read items granted message.");
    }
    return ok;
}







bdBool bdLobbyService::handleMarketplaceEntitlementsGranted(bdByteBufferRef message)
{
    bdUInt32 numEntitlements;
    bdUInt32 entitlements[6];
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    numEntitlements = 0;

    ContextSerialization::readUserID(message, playerID);
    bdBool ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readUInt32(numEntitlements);
    if (numEntitlements > 6)
    {
        numEntitlements = 6;
        bdLogWarn("lobby service", "Too many entitlements granted, truncating to %u of %u.", numEntitlements, 6);
    }
    for (bdUInt32 i = 0; i < numEntitlements; ++i)
    {
        ok = ok && message->readUInt32(entitlements[i]);
    }
    if (ok)
    {
        m_eventHandler->onEntitlementsGranted(playerID, context, numEntitlements, entitlements);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read entitlements granted message.");
    }
    return ok;
}







bdBool bdLobbyService::handleWebserviceStorageFileWrite(bdByteBufferRef message)
{
    bdUByte8 version = 0;
    bdBool ok = message->readUByte8(version);
    if (version != 1)
    {
        bdLogWarn("lobby service", "Unknown webservice storage file write version.");
        ok = false;
    }
    bdNChar8 context[16] = {0};
    ok = ok && ContextSerialization::readContext(message, context, sizeof(context));
    bdUInt32 numFilenames = 0;
    ok = ok && message->readUInt32(numFilenames);
    if (ok && numFilenames > 64)
    {
        numFilenames = 64;
        bdLogWarn("lobby service", "Too many filenames, truncating to %u of %u.", numFilenames, 64);
    }
    for (bdUInt32 i = 0; ok && i < numFilenames; ++i)
    {
        bdNChar8 filename[128];
        ok = message->readString(filename, sizeof(filename));
        if (ok)
        {
            m_eventHandler->onWebserviceStorageFileWrite(context, filename);
        }
    }
    if (!ok)
    {
        bdLogWarn("lobby service", "Failed to handle webservice storage file write.");
    }
    return ok;
}















bdBool bdLobbyService::handleRewardEventMessage(bdByteBufferRef message)
{
    bdUByte8 version = 0;
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdInt32 rewardEventType = 0;
    bdUInt32 numEvents = 0;

    bdBool ok = message->readUByte8(version);
    if (version != 1)
    {
        bdLogWarn("lobby service", "Unsupported reward message version.");
        ok = false;
    }
    ok = ok && ContextSerialization::readUserID(message, playerID);
    ok = ok && ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readInt32(rewardEventType);
    ok = ok && message->readUInt32(numEvents);
    if (numEvents > 6)
    {
        numEvents = 6;
        bdLogWarn("lobby service", "Too many reward events, truncating to %u of %u.", numEvents, 6);
    }
    if (rewardEventType == 1)
    {
        for (bdUInt32 i = 0; i < numEvents; ++i)
        {
            bdRewardEventJSONV2 eventV2;
            ok = ok && eventV2.deserialize(message);
            if (ok)
            {
                m_eventHandler->onRewardEventJSONMessage(playerID, context, eventV2);
                bdRewardEventJSON eventV1;
                eventV1.set(eventV2.getJsonData());
                m_eventHandler->onRewardEventJSONMessage(playerID, context, eventV1);
            }
            else
            {
                bdLogWarn("lobby service", "Failed to read reward event.");
            }
        }
    }
    else
    {
        bdLogWarn("lobby service", "Unsupported reward event type %d.", rewardEventType);
        ok = false;
    }
    return ok;
}
























bdBool bdLobbyService::handleRewardAchievementMessage(bdByteBufferRef message)
{
    bdUByte8 version = 0;
    bdUserAccountID playerID;
    bdNChar8 context[16] = {0};
    bdInt32 rewardEventType = 0;
    bdUInt32 numEvents = 0;

    bdBool ok = message->readUByte8(version);
    if (version != 1)
    {
        bdLogWarn("lobby service", "Unsupported reward message version.");
        ok = false;
    }
    ok = ok && ContextSerialization::readUserID(message, playerID);
    ok = ok && ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readInt32(rewardEventType);
    ok = ok && message->readUInt32(numEvents);
    if (numEvents > 6)
    {
        numEvents = 6;
        bdLogWarn("lobby service", "Too many reward events, truncating to %u of %u.", numEvents, 6);
    }
    if (rewardEventType == 1)
    {
        for (bdUInt32 i = 0; i < numEvents; ++i)
        {
            bdRewardEventJSONV2 eventV2;
            ok = ok && eventV2.deserialize(message);
            if (ok)
            {
                m_eventHandler->onRewardAchievementJSONMessage(playerID, context, eventV2);
                bdRewardEventJSON eventV1;
                eventV1.set(eventV2.getJsonData());
                m_eventHandler->onRewardAchievementJSONMessage(playerID, context, eventV1);
            }
        }
    }
    else
    {
        bdLogWarn("lobby service", "Unsupported reward event type %d.", rewardEventType);
        ok = false;
    }
    return ok;
}
























bdBool bdLobbyService::handleAchievementsUpdated(bdByteBufferRef message)
{
    bdAchievementsUpdatedPushMessage pushMessage;
    bdBool ok = bdStructBufferDeserializerHelpers::deserializeFromBuffer(message, pushMessage);
    if (ok)
    {
        m_eventHandler->onAchievementsUpdated(pushMessage);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read achievements updated message.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleMarketingCommsAssignmentsAvailable()
{
    m_eventHandler->onMarketingCommsAssignmentsAvailable();
    return true;
}


bdBool bdLobbyService::handlePublisherVariablesUpdateMessage(bdByteBufferRef message)
{
    bdUInt32 version = 0;
    bdBool isMajor = false;
    bdNChar8 context[16] = {0};
    bdNChar8 nameSpace[16] = {0};

    bdBool ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readString(nameSpace, sizeof(nameSpace));
    ok = ok && message->readUInt32(version);
    ok = ok && message->readBool(isMajor);
    if (ok)
    {
        bdUInt16 majorVersion = static_cast<bdUInt16>(version >> 16);
        bdUInt16 minorVersion = static_cast<bdUInt16>(version);
        m_eventHandler->onPublisherVariablesUpdate(context, nameSpace, majorVersion, minorVersion, isMajor);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read publisher variables update message.");
    }
    return ok;
}




bdBool bdLobbyService::handleTeamMemberRichPresenceUpdated(bdByteBufferRef message)
{
    bdOnlineUserID userID = 0;
    bdUByte8 richPresence[1024];
    bdUInt richPresenceSize = sizeof(richPresence);
    bdBool isOnline = false;

    bdBool ok = message->readUInt64(userID);
    ok = ok && message->readBlob(richPresence, richPresenceSize);
    ok = ok && message->readBool(isOnline);

    if (ok)
    {
        m_eventHandler->onTeamMemberRichPresenceUpdate(userID, richPresence, richPresenceSize, isOnline);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read team member rich presence update.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleTeamMemberUserNameUpdated(bdByteBufferRef message)
{
    bdOnlineUserID userID = 0;
    bdNChar8 userName[65];

    bdBool ok = message->readUInt64(userID);
    ok = ok && message->readString(userName, 64);

    if (ok)
    {
        m_eventHandler->onTeamMemberUserNameUpdate(userID, userName);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read team member user name update.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handlePrivateProfileUpdate(bdByteBufferRef message)
{
    bdBool fromWebservice = false;


    m_eventHandler->onUserPrivateProfileUpdated();

    bdBool ok = message->readBool(fromWebservice);
    if (ok)
    {
        m_eventHandler->onUserPrivateProfileUpdatedWithCallerInfo(fromWebservice);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read private profile update.");  // text inferred
    }
    return ok;
}


bdBool bdLobbyService::handleTitleVersionDisabled(bdByteBufferRef message)
{
    bdUInt64 recipientID = 0;
    bdBool ok = message->readUInt64(recipientID);

    if (ok)
    {
        m_eventHandler->onTitleVersionDisabled(recipientID);
    }
    return ok;
}


bdBool bdLobbyService::handleCODOTeamMarketplaceLevelChange(bdByteBufferRef message)
{
    bdUInt64 teamID = 0;
    bdUInt32 currentTeamLevel = 0;
    bdUInt32 newTeamLevel = 0;
    bdBool ok = message->readUInt64(teamID);
    ok = ok && message->readUInt32(currentTeamLevel);
    ok = ok && message->readUInt32(newTeamLevel);

    if (ok)
    {
        m_eventHandler->onCODOTeamMarketplaceLevelChange(teamID, currentTeamLevel, newTeamLevel);
    }
    return ok;
}


bdBool bdLobbyService::handleSubscribedRichPresenceUpdated(bdByteBufferRef message)
{
    bdTitleID titleID;
    bdUByte8 updateSource;
    bdNChar8 context[16] = {0};
    titleID = 0;
    updateSource = 0;
    bdRichPresenceData richPresence;

    bdBool ok = ContextSerialization::readContext(message, context, sizeof(context));
    ok = ok && message->readUInt32(titleID);
    ok = ok && message->readUByte8(updateSource);
    ok = ok && richPresence.deserialize(message);
    if (ok)
    {
        bdRichPresenceSource source = updateSource;
        m_eventHandler->onSubscribedRichPresenceUpdate(context, titleID, source, richPresence);
    }
    else
    {
        bdLogWarn("lobby service", "Failed to read subscribed rich presence update.");
    }
    return ok;
}

bdBool bdLobbyService::handleLogonTimeProhibited(bdByteBufferRef message)
{
    bdUByte8 msg[1024] = {0};
    bdUInt32 msgLength = sizeof(msg);

    bdBool ok = message->readBlob(msg, msgLength);

    if (ok)
    {
        m_eventHandler->onLogonTimeProhibited(msg, msgLength);
    }
    return ok;
}


bdBool bdLobbyService::handleLogonTimeProhibitedWarning(bdByteBufferRef message)
{
    bdUInt32 timeRemaining = 0;
    bdUByte8 msg[1024] = {0};
    bdUInt32 msgLength = sizeof(msg);

    bdBool ok = message->readUInt32(timeRemaining);
    ok = ok && message->readBlob(msg, msgLength);

    if (ok)
    {
        m_eventHandler->onLogonTimeProhibitedWarning(timeRemaining, msg, msgLength);
    }
    return ok;
}



















const bdNChar8 * bdLobbyErrorCodeToString(const bdLobbyErrorCode code)
{
    const bdNChar8 * desc = "Kuddelmuddel";
    return desc;
}
























































void bdLobbyService::updateConnectionThrottleParams()
{
    if (m_lobbyConnection)
    {
        m_lobbyConnection->getThrottleBucket().setTokensRestricted(m_uploadThrottled);
        m_lobbyConnection->getThrottleBucket().setRates(m_uploadThrottleBytesPerMs, m_uploadThrottleBucketSize);
        m_lobbyConnection->getThrottleBucket().setMinFragment(600);
        m_lobbyConnection->setTasksAreThrottled(m_markTasksThrottled);
    }
}
