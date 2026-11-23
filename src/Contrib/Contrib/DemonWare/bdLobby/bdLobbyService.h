// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyService.h

#pragma once

#include "../bdCore/bdContainers/bdByteBuffer.h"
#include "../bdCore/bdContainers/bdHashMap.h"
#include "../bdCore/bdString/bdString.h"
#include "../bdCore/bdReference/bdReference.h"
#include "../bdCore/bdSocket/bdAddr.h"
#include "../bdCore/bdSocket/bdPort.h"
#include "bdAuthTicket.h"
#include "bdLobbyCommon/bdEnvironmentString.h"
#include "bdLobbyCommon/bdLobbyServicesCommon.h"
#include "bdLobbyConnection.h"
#include "bdLobbyConnectionListener.h"
#include "bdLobbyErrorCodes.h"
#include "bdMessaging/bdLobbyMessageType.h"
#include "../bdNet/bdGetHostByName/bdGetHostByName.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdABTesting;
class bdService;
class bdPushMessageHandler;
class bdAchievementsEngineService;
class bdAntiCheat;
class bdCODO;
class bdCODOTeamMarketplace;
class bdCommerce;
class bdContentStreaming;
class bdContentUnlock;
class bdCounter;
class bdDDL;
class bdDML;
class bdEventLog;
class bdFacebook;
class bdFeatureBan;
class bdFriends;
class bdGroup;
class bdKeyArchive;
class bdLeague;
class bdLinkCode;
class bdLinkedAccounts;
class bdLobbyEventHandler;
class bdLobbyServicesConnectionInfo;
class bdMLG;
class bdMail;
class bdMarketingComms;
class bdMarketplace;
class bdMatchMaking;
class bdMatchScheduler;
class bdMessaging;
class bdObjectStore;
class bdPlayerVote;
class bdPooledStorage;
class bdPresenceService;
class bdProfiles;
class bdPublisherVariables;
class bdPushMessageHandler;
class bdRedeemableCodeService;
class bdRelayService;
class bdRemoteTaskManager;
class bdReward;
class bdRichPresenceService;
class bdServerInventory;
class bdStats;
class bdStorage;
class bdSubscription;
class bdTags;
class bdTeamShowcase;
class bdTeams;
class bdTencent;
class bdTitleUtilities;
class bdTwitch;
class bdTwitter;
class bdUmbrella;
class bdUno;
class bdUserBlockList;
class bdUserGroups;
class bdVoteRank;
class bdVoting;

typedef bdReference<bdLobbyConnection> bdLobbyConnectionRef;

class bdLobbyService : public bdLobbyConnectionListener {  // size 0x1710
public:
    BD_CLASS_ALLOCATOR(bdLobbyService);
    bdLobbyService();
    virtual ~bdLobbyService();  // vtable slot 0
    bdLobbyService(const bdLobbyServicesConnectionInfo &);
    bdBool pumpLSGLookup(bdGetHostByName::bdStatus *);
    bdBool connect(const bdAuthInfo * authInfo, const bdBool useEncryption);
    bdBool connect(bdAddr lobbyServiceAddr, const bdAuthInfo * authInfo, bdUWord maxSendBufSize, bdUWord maxRecvBufSize, const bdBool useEncryption);
    bdLobbyService(const bdNChar8 *, const bdPort);
    bdBool connect(bdAddr, const bdAuthInfo *, const bdBool);
    bdBool connect(bdAddr, const bdAuthInfo *, void *, bdUWord, void *, bdUWord, const bdBool);
    bdBool pumpDNSLookup(bdGetHostByName::bdStatus & status);
    void disconnect();
    bdBool isConnected();
    void pump();
    bdLobbyConnection::Status getStatus();
    void registerEventHandler(bdLobbyEventHandler * eventHandler);
    bdBool registerPushMessageHandler(bdPushMessageHandler *);
    bdBool unregisterPushMessageHandler(bdPushMessageHandler *);
    bdMessaging * getMessaging();
    bdMatchMaking * getMatchMaking(const bdNChar8 * context);
    bdProfiles * getProfiles();
    bdStats * getStats();
    bdTeams * getTeams(const bdNChar8 *);
    bdTeamShowcase * getTeamShowcase(const bdNChar8 *);
    bdFriends * getFriends();
    bdStorage * getStorage(const bdNChar8 * context);
    bdMail * getMail(const bdNChar8 * context);
    bdContentUnlock * getContentUnlock();
    bdTitleUtilities * getTitleUtilities();
    bdEventLog * getEventLog();
    bdKeyArchive * getKeyArchive();
    bdCounter * getCounter();
    bdGroup * getGroup();
    bdContentStreaming * getContentStreaming();
    void deleteContentStreaming();
    bdPooledStorage * getPooledStorage();
    void deletePooledStorage();
    bdTags * getTags();
    bdVoteRank * getVoteRank();
    bdTwitch * getTwitch();
    bdTwitter * getTwitter();
    bdFacebook * getFacebook();
    bdLinkCode * getLinkCode(const bdNChar8 *);
    bdAntiCheat * getAntiCheat();
    bdDML * getDML();
    bdUmbrella * getUmbrella();
    bdUno * getUno();
    bdUserGroups * getUserGroups();
    bdRichPresenceService * getRichPresence(const bdNChar8 * context);
    bdPresenceService * getPresence();
    bdMarketplace * getMarketplace(const bdNChar8 *);
    bdCommerce * getCommerce();
    bdSubscription * getSubscription();
    bdFeatureBan * getFeatureBan();
    bdTencent * getTencent();
    bdCODO * getCODO();
    bdCODOTeamMarketplace * getCODOTeamMarketplace();
    bdLeague * getLeague();
    bdRelayService * getRelayService();
    bdLinkedAccounts * getLinkedAccounts();
    bdVoting * getVoting();
    bdMarketingComms * getMarketingComms(const bdNChar8 *);
    bdUserBlockList * getUserBlockList();
    bdReward * getRewards(const bdNChar8 *);
    bdRemoteTaskManager * getTaskManager();
    bdPublisherVariables * getPublisherVariables(const bdNChar8 *);
    bdServerInventory * getServerInventory();
    bdRedeemableCodeService * getRedeemableCodeService(const bdNChar8 *);
    bdMatchScheduler * getMatchScheduler();
    bdMLG * getMLG();
    bdDDL * getDDL();
    bdObjectStore * getObjectStore(bdEnvironment, const bdNChar8 *);
    bdPlayerVote * getPlayerVoting(const bdNChar8 *);
    bdAchievementsEngineService * getAchievementsEngineService();
    bdABTesting * getABTesting();
    bdLobbyErrorCode getErrorCode();
    void getHandleInfo(bdInt &, bdBool &, bdBool &, bdFloat32 &);
    void setTasksAreThrottled(bdBool);
    void setUploadThrottle(bdBool);
    void setUploadThrottleRate(bdUInt32, bdUInt32);
    bdAuthInfo * getAuthInfo();
    bdNChar8 * getABTestingToken();
    bdBool setABTestingTokenAndExpiry(const bdNChar8 *const, const bdUInt64);
protected:
    virtual void onConnect(bdLobbyConnectionRef connection);  // vtable slot 2
    virtual void onDisconnect(bdLobbyConnectionRef);  // vtable slot 4
    void cleanup();
    void handlePushMessage(bdByteBufferRef message);
    bdBool handleFriendDisconnected(bdByteBufferRef message, bdEventType eventType);
    bdBool handleTeamMemberConnection(bdByteBufferRef message, bdEventType eventType);
    bdBool handleSessionInvitation(bdByteBufferRef message);
    bdBool handleChannelChatMsg(bdByteBufferRef message);
    bdBool handleChannelUserSubscription(bdByteBufferRef message, bdEventType eventType);
    bdBool handleChannelUserPromoted(bdByteBufferRef message);
    bdBool handleChannelUserKicked(bdByteBufferRef message);
    bdBool handleFriendChatMsg(bdByteBufferRef message);
    bdBool handleTeamChatMsg(bdByteBufferRef message);
    bdBool handleFriendRichPresenceUpdated(bdByteBufferRef message);
    bdBool handleNotifyLeave(bdByteBufferRef message);
    bdBool handleNewMail();
    bdBool handleChallengesReceived(bdByteBufferRef message);
    bdBool handleMultipleLogons(bdByteBufferRef message);
    bdBool handlePlayerDisconnected(bdByteBufferRef message);
    bdBool handlePlayerBanned(bdByteBufferRef message);
    bdBool handleConsoleBanned(bdByteBufferRef message);
    bdBool handleMultipleLinkedAccountLogons(bdByteBufferRef message);
    bdBool handleLinkedAccountStatusChange(bdByteBufferRef message);
    bdBool handleQueuedMatchingData(bdByteBufferRef);
    bdBool handleNotWhiteListed(bdByteBufferRef message);
    bdBool handleNotWhiteListedWithMessage(bdByteBufferRef message);
    bdBool handleGlobalInstantMessage(bdByteBufferRef message);
    bdBool handleFeatureBan(bdByteBufferRef message);
    bdBool handleGMSGBroadcast(bdEventType eventType, bdByteBufferRef message);
    bdBool handleTencentAASRecord(bdByteBufferRef message);
    bdBool handleTencentAntiBotData(bdByteBufferRef message);
    bdBool handleTencentAntiBotPunish(bdByteBufferRef message);
    bdBool handleTencentUserLeaveReason(bdByteBufferRef message);
    bdBool handleTencentAntibotServerReady(bdByteBufferRef message);
    bdBool handleTencentLoudSpeakerMessage(bdByteBufferRef message);
    bdBool handleChannelUserMuted(bdByteBufferRef message);
    bdBool handleStabilised(bdByteBufferRef message);
    bdBool handleTencentReward(bdByteBufferRef message);
    bdBool handleTencentNoRewardReceived(bdByteBufferRef message);
    bdBool handleTSSResponse(bdByteBufferRef message);
    bdBool handleTencentNameChanged(bdByteBufferRef message);
    bdBool handleEventLogFilteredCategories(bdByteBufferRef message);
    bdBool handleMarketplaceCouponsGranted(bdByteBufferRef message);
    bdBool handleMarketplaceItemsExpired(bdByteBufferRef message);
    bdBool handleMarketplaceItemsUpdated(bdByteBufferRef message);
    bdBool handleMarketplaceCouponsUpdated(bdByteBufferRef message);
    bdBool handleMarketplaceCouponsUpdatedV2(bdByteBufferRef message);
    bdBool handleMarketplaceCouponsUpdatedV3(bdByteBufferRef message);
    bdBool handleMarketplaceCouponsUpdatedV4(bdByteBufferRef message);
    bdBool handleTOTPMessage(bdByteBufferRef message);
    bdBool handleClaimAchievement(bdByteBufferRef message);
    bdBool handleMarketplaceBalanceUpdated(bdByteBufferRef message);
    bdBool handleMarketplaceDepositGranted(bdByteBufferRef message);
    bdBool handleMarketplaceItemsGranted(bdByteBufferRef message);
    bdBool handleMarketplaceItemsGrantedWithInventoryQuantity(bdByteBufferRef message);
    bdBool handleMarketplaceEntitlementsGranted(bdByteBufferRef message);
    bdBool handleWebserviceStorageFileWrite(bdByteBufferRef message);
    bdBool handleRewardEventMessage(bdByteBufferRef message);
    bdBool handleRewardAchievementMessage(bdByteBufferRef message);
    bdBool handleAchievementsUpdated(bdByteBufferRef message);
    bdBool handleMarketingCommsAssignmentsAvailable();
    bdBool handlePublisherVariablesUpdateMessage(bdByteBufferRef message);
    bdBool handleTeamMemberRichPresenceUpdated(bdByteBufferRef message);
    bdBool handleTeamMemberUserNameUpdated(bdByteBufferRef message);
    bdBool handleTitleVersionDisabled(bdByteBufferRef message);
    bdBool handlePrivateProfileUpdate(bdByteBufferRef message);
    bdBool handleCODOTeamMarketplaceLevelChange(bdByteBufferRef message);
    bdBool handleSubscribedRichPresenceUpdated(bdByteBufferRef message);
    bdBool handleLogonTimeProhibited(bdByteBufferRef message);
    bdBool handleLogonTimeProhibitedWarning(bdByteBufferRef message);
    void updateConnectionThrottleParams();
    bdRemoteTaskManager * m_taskManager;  // 0x08
    bdHashMap<bdString, bdService *, bdHashingClass> m_services;  // 0x10
    bdHashMap<bdEventType, bdPushMessageHandler *, bdHashingClass> m_pushHandlers;  // 0x30
    bdProfiles * m_profiles;  // 0x50
    bdMessaging * m_messaging;  // 0x58
    bdHashMap<bdString, bdMatchMaking *, bdHashingClass> m_matchmakings;  // 0x60
    bdStats * m_statsManager;  // 0x80
    bdFriends * m_friendsManager;  // 0x88
    bdContentUnlock * m_contentUnlockManager;  // 0x90
    bdTitleUtilities * m_titleUtilitiesManager;  // 0x98
    bdEventLog * m_eventLogManager;  // 0xa0
    bdKeyArchive * m_keyArchive;  // 0xa8
    bdCounter * m_counter;  // 0xb0
    bdGroup * m_group;  // 0xb8
    bdContentStreaming * m_contentStreamingManager;  // 0xc0
    bdPooledStorage * m_pooledStorageManager;  // 0xc8
    bdTags * m_tags;  // 0xd0
    bdVoteRank * m_voteRankManager;  // 0xd8
    bdTwitch * m_twitch;  // 0xe0
    bdTwitter * m_twitter;  // 0xe8
    bdFacebook * m_facebook;  // 0xf0
    bdLinkCode * m_linkCode;  // 0xf8
    bdAntiCheat * m_antiCheat;  // 0x100
    bdDML * m_DML;  // 0x108
    bdObjectStore * m_objectStore;  // 0x110
    bdAchievementsEngineService * m_achievementsEngineService;  // 0x118
    bdABTesting * m_abTesting;  // 0x120
    bdUmbrella * m_umbrella;  // 0x128
    bdUno * m_uno;  // 0x130
    bdUserGroups * m_userGroups;  // 0x138
    bdPresenceService * m_presence;  // 0x140
    bdCommerce * m_commerce;  // 0x148
    bdSubscription * m_subscription;  // 0x150
    bdFeatureBan * m_featureBan;  // 0x158
    bdTencent * m_tencent;  // 0x160
    bdCODO * m_CODO;  // 0x168
    bdCODOTeamMarketplace * m_CODOTeamMarketplace;  // 0x170
    bdLeague * m_league;  // 0x178
    bdRelayService * m_relayService;  // 0x180
    bdLinkedAccounts * m_linkedAccounts;  // 0x188
    bdVoting * m_voting;  // 0x190
    bdUserBlockList * m_userBlockList;  // 0x198
    bdHashMap<bdString, bdMarketingComms *, bdHashingClass> m_marketingCommsManagers;  // 0x1a0
    bdDDL * m_DDL;  // 0x1c0
    bdServerInventory * m_serverInventory;  // 0x1c8
    bdHashMap<bdString, bdRedeemableCodeService *, bdHashingClass> m_redeemableCodeServiceManagers;  // 0x1d0
    bdMatchScheduler * m_matchScheduler;  // 0x1f0
    bdMLG * m_mlg;  // 0x1f8
    bdHashMap<bdString, bdMarketplace *, bdHashingClass> m_marketplaceManagers;  // 0x200
    bdHashMap<bdString, bdReward *, bdHashingClass> m_rewardManagers;  // 0x220
    bdHashMap<bdString, bdPublisherVariables *, bdHashingClass> m_publisherVariablesManagers;  // 0x240
    bdHashMap<bdString, bdStorage *, bdHashingClass> m_storageManagers;  // 0x260
    bdHashMap<bdString, bdTeams *, bdHashingClass> m_teamsManagers;  // 0x280
    bdHashMap<bdString, bdTeamShowcase *, bdHashingClass> m_teamShowcaseManagers;  // 0x2a0
    bdHashMap<bdString, bdMail *, bdHashingClass> m_mailManagers;  // 0x2c0
    bdHashMap<bdString, bdLinkCode *, bdHashingClass> m_linkCodeManagers;  // 0x2e0
    bdHashMap<bdString, bdRichPresenceService *, bdHashingClass> m_richPresenceManagers;  // 0x300
    bdHashMap<bdString, bdPlayerVote *, bdHashingClass> m_playerVoteManagers;  // 0x320
    bdGetHostByName m_LSGLookup;  // 0x340
    const bdNChar8 * m_LSGAddress;  // 0x588
    bdPort m_LSGPort;  // 0x590
    bdAddr m_resolvedLSGAddress;  // 0x594
    bdBool m_isResolved;  // 0x61c
    bdTitleID m_titleID;  // 0x620
    bdLobbyEventHandler * m_eventHandler;  // 0x628
    bdLobbyConnectionRef m_lobbyConnection;  // 0x630
    bdBool m_lobbyConnectionEstablished;  // 0x638
    bdAuthInfo m_authInfo;  // 0x640
    bdBool m_encryptedConnection;  // 0x6f0
    bdLobbyErrorCode m_errorCode;  // 0x6f4
    bdBool m_markTasksThrottled;  // 0x6f8
    bdBool m_uploadThrottled;  // 0x6f9
    bdUInt32 m_uploadThrottleBytesPerMs;  // 0x6fc
    bdUInt32 m_uploadThrottleBucketSize;  // 0x700
    bdNChar8 m_abTestingToken[4096];  // 0x704
    bdUInt64 m_abTestingTokenExpiresInSeconds;  // 0x1708
};
static_assert(sizeof(bdLobbyService) == 0x1710, "bdLobbyService");


// Functions defined in this file (bodies are not in the DWARF):
//   line   129  void bdLobbyService::operator delete(void * p)  [inlined into 1 functions]
