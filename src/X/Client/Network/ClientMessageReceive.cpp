// Original file: X/Client/Network/ClientMessageReceive.cpp
// Line numbers in comments are the original ones (DWARF line table).

#include "../../Common/CommonTypes.h"
#include "../../Common/Game.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/Player.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/Definitions/HeroDefinition.h"
#include "../../Sigma/LanguageRules.h"
#include "../SNOPreload.h"
#include "../UserInterface/Console/UIConsoleNewItemIndicators.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Common/UserInterface/UITextControl.h"
#include "../../Common/Networking/GameMessage.h"
#include "../CGame.h"
#include "../CHero.h"
#include "../CPlayer.h"
#include "../Cutscene.h"
#include "../Sound/Sound.h"
#include "../Console/GamerProfile.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../UserInterface/UIHeroData.h"
#include "../UserInterface/Controls/UITextControl.h"
#include "../UserInterface/Console/ConsoleScreenManager.h"
#include "../UserInterface/Interface/UIBossElements.h"
#include "../UserInterface/Interface/UIConversations.h"
#include "../UserInterface/Interface/UILoreCommon.h"
#include "../Movement/CActorMovement.h"
#include "../UserInterface/Interface/UIGame.h"
#include "../../Common/Protobuf/GameMessage.pb.h"
#include "../UserInterface/Interface/UISetDungeon.h"
#include "../../Common/Crafting/ItemCrafting.h"
#include "../../Common/Definitions/Definitions.h"
#include "../../Common/VisualInventory.h"
#include "../../Common/Console/Avenger.h"
#include "../System/CVoteKick.h"
#include "../CWorld.h"
#include "../../Sigma/RefString.h"
#include "../UserInterface/Interface/UIQuestComplete.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Quests/Quests.h"
#include "../../Common/Protobuf/Quest.pb.h"
#include "../../Common/Protobuf/Leaderboard.pb.h"
#include "../../Common/Monsters.h"
#include "../../Common/Items.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/AnimPlayer.h"
#include "../UserInterface/UI.h"
#include "../UserInterface/Interface/UIBuffs.h"
#include "../UserInterface/Console/UIConsoleTransmogView.h"
#include "../UserInterface/Console/UIConsoleTraining.h"
#include "../UserInterface/Console/UIConsoleMultiplayerDeathMenu.h"
#include "../UserInterface/Console/UIConsoleEnchantView.h"
#include "../RActors/RActorACDEffects.h"
#include "../Prefetch.h"
#include "../Console/Online.h"
#include "../CActTransitions.h"
#include "../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Battlenet/Subsystems/FanfareManager.h"
#include "../../Sigma/UnicodeText.h"
#include "../../Sigma/SigmaThreadLocalStorage.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/DataStructures/XMap.h"
#include "../../Common/Target.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../Common/Protobuf/ConsoleMail.pb.h"
#include "../../Common/PlayerAccount.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/OnlineService/Achievements.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/GameTemplate.h"
#include "../../Common/GameBalance.h"
#include "../../Common/CosmeticItems.h"
#include "../../Common/CombatLog.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Attributes/AttributeCompressor.h"
#include "../Weather.h"
#include "../UserInterface/UIID.h"
#include "../UserInterface/Interface/UIHero.h"
#include "../UserInterface/Interface/UIConfirmation.h"
#include "../UserInterface/Interface/UIBlizzconEnd.h"
#include "../UserInterface/Console/UIConsoleDyeItemView.h"
#include "../Sprite.h"
#include "../RWindow.h"
#include "../RActors/RActors.h"
#include "../PostFX.h"
#include "../Console/LobbyService.h"
#include "../Camera/CameraManager.h"
#include "../Battlenet/Subsystems/ToastManager.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
// >>> EXTRA INCLUDES
#include "../../Common/Protobuf/Items.pb.h"
#include "../Console/NX64NFP.h"
#include "../UserInterface/Console/ConsoleModalNotificationManager.h"
#include "../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../UserInterface/Controls/UITutorialControl.h"
#include "../../Server/SGameManager.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/ItemMailCommon.h"
#include "../../Common/ActTransitions.h"
#include "../Definitions/MusicDefinition.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../../NintendoSDK/Include/nn/oe/oe_NotificationMessage.h"
#include "../../Common/PortalCommon.h"
#include "../../Server/Network/ServerMessageSend.h"
#include "../../Common/NPC.h"
#include "../../Common/Definitions/WorldDefinition.h"
#include "../../Common/Actors/ComplexEffect.h"
#include "../RActors/CComplexEffect.h"
#include "../../Common/TimedEvents.h"
// <<< EXTRA INCLUDES
#include "ClientMessageReceive.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

// ---- declarations whose headers are not in src yet ---------------------------------------
// X/Client/UserInterface/Interface/UIVictory.h
namespace UIVictory {
enum Mode {
    MODE_VANILLA = 0,
    MODE_XP1 = 1,
    MODE_ETERNAL = 2,
    MODE_BLIZZCON = 3,
    MODE_BETA = 4,
};
}  // namespace UIVictory
typedef int32 EChatColor;
typedef int32 QuitReason;
typedef int32 LogoutContext;

// Types the prototypes and the dispatcher need that no src header defines; copied from the DWARF
// reference headers (the comment above each names the original header).
// >>> LOCAL TYPES
// X/Common/Networking/GameMessage.h






// X/Common/Items.h









// X/Common/ActorCommonData.h





// X/Common/Networking/GameMessage.h











// X/Common/Player.h
struct PlayerLoadoutEquipmentUpdateMessage {  // size 0x30
    RequiredMessageHeader tHeader;  // 0x00
    int32 nLoadoutIndex;  // 0x08
    int32 nEquipmentIndex;  // 0x0c
    LoadoutItemData tNewItem;  // 0x10
};




















































































// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h
typedef int32 ServerShutdownType;
// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h












// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h






// X/Common/GameCommonData.h
struct GameSyncedDataMessage {  // size 0xd8
    RequiredMessageHeader tHeader;  // 0x00
    GameSyncedData tSyncedData;  // 0x08
};
// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h








// X/Common/GameTesting.h
enum ScreenshotView {
    SV_INVALID = 0,
    SV_FRONT = 1,
    SV_BACK = 2,
    SV_GAME = 3,
};
// X/Client/ConversationUtils.h
typedef int32 ConversationSpeaker;
// X/Common/ConversationCommon.h
struct PlayLineParams {  // size 0x4c
    SNO snoConversation;  // 0x00
    int32 nSpeakingPlayerIndex;  // 0x04
    BOOL bZoom;  // 0x08
    BOOL bFirstLine;  // 0x0c
    BOOL bHostile;  // 0x10
    BOOL bPlayerInitiated;  // 0x14
    uint32 dwLineUID;  // 0x18
    ConversationSpeaker eLineSpeaker;  // 0x1c
    ActorGender eLineGender;  // 0x20
    PlayerClass eLineClass;  // 0x24
    ConversationSpeaker eSpeakerTarget;  // 0x28
    SNO snoSpeakerActor;  // 0x2c
    ActorGender ePlayerGender;  // 0x30
    PlayerClass ePlayerClass;  // 0x34
    uint32 dwLineFlags;  // 0x38
    uint32 dwLineAnimTag;  // 0x3c
    uint32 dwLineDisplayTime;  // 0x40
    int32 nLineGUID;  // 0x44
    int32 nPriority;  // 0x48
};
// X/Common/ConversationCommon.h
struct PlayConvLineMessage {  // size 0x80
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annSpeaker;  // 0x08
    ACDNetworkName annParticipants[9];  // 0x0c
    PlayLineParams tParams;  // 0x30
    uint32 dwTimeToDisplay;  // 0x7c
};
// X/Common/Crafting/ItemCrafting.h
struct CrafterLevelUpMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    CrafterType eType;  // 0x08
    uint32 dwAnimTag;  // 0x0c
    uint32 dwNewIdle;  // 0x10
    int32 nLevel;  // 0x14
};
// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h
typedef int32 DeSyncReason;
// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h




// X/Common/Hireling.h
struct HirelingSwapMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    HirelingClass eNewClass;  // 0x08
};
// X/Common/ConversationCommon.h
struct EndConversationMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConversation;  // 0x08
    ACDNetworkName annSpeaker;  // 0x0c
    int8 nPlayerTalkingIndex;  // 0x10
};
// X/Common/ConversationCommon.h
struct StopConvLineMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nConvLineGUID;  // 0x08
    BOOL fStoppedByUser;  // 0x0c
};
// X/Client/Network/Client.h
struct EndOfTickMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwTickEnding;  // 0x08
    uint32 dwTickNext;  // 0x0c
};
// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h








// X/Common/Networking/GameMessage.h






// X/Common/Console/Avenger.h










// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h











// X/Common/Networking/GameMessage.h




// X/Common/Actors/ActorEffectCommon.h
struct PlayMusicMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoMusic;  // 0x08
};
// X/Common/Networking/GameMessage.h




// X/Common/Actors/ActorEffectCommon.h
struct PlayNonPositionalSoundMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoSound;  // 0x08
};
// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h






// NintendoSDK/Include/nn/oe/oe_NotificationMessage.h
typedef nn::Bit32 Message;
// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Player.h
struct StashIconStateMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    uint8 bStashIcons[13];  // 0x08
};
// X/Common/Networking/GameMessage.h












// X/Common/Networking/GameMessage.h



// X/Common/Networking/GameMessage.h








// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h
typedef int32 TrickleInfoType;
// X/Common/Networking/GameMessage.h
typedef int32 DebugDrawPrimType;
// X/Common/Player.h




















// X/Common/NPC.h
typedef int32 NPCMenu;
// X/Common/Networking/GameMessage.h





// X/Common/Crafting/ItemCrafting.h
struct CraftingResultsMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidItem;  // 0x0c
    ItemQualityLevel eIQL;  // 0x10
};
// X/Common/Networking/GameMessage.h


















// X/Common/Networking/GameMessage.h
typedef int32 ComplexEffectType;
// X/Common/Networking/GameMessage.h


























// X/Common/Networking/GameMessage.h













// <<< LOCAL TYPES

// Prototypes of the functions this file calls whose declaring headers are not in src yet. Each line is
// the prototype the DWARF records for the definition (reference tree build/dwarf/headers); the comment
// above each group names the defining file.
// >>> GENERATED DECLS (build/try/agentE/mkblock.py; prototypes from the DWARF function lists)
// BlizzardCore/Source/System/Thread/ThreadLocalStorage.cpp
namespace Blizzard { namespace Thread {
void * GetLocalStorage(const Blizzard::Thread::TLSSlot * slot);
}}  // namespace Blizzard::Thread
// BlizzardCore/Source/System/Time/Time.cpp
namespace Blizzard { namespace Time {
Blizzard::Time::Timestamp FromUnixTime(Blizzard::Time::UnixTime unixtime);
}}  // namespace Blizzard::Time
// X/Client/Attachments.cpp
void AttachmentListHandleParentWarp(XPooledDataIDList & tAttachmentList);
// X/Client/CActTransitions.cpp
void CActTransitionEnded();
void CActTransitionStart(int32 nPlayerIndex, uint32 dwActTransitionTime, BOOL bRecreateGameWithParty);
// X/Client/CActorCommonData.cpp
void CACDSetAffix(ActorCommonData & tACD, const GBID gbidAffix, const BOOL fIdentified);
void ACDUpdateAppearance(ACDID idACD);
void CACDAttributeSetValue(ActorCommonData & tACD, FastAttribKey tKey, FastAttribValue tValue, const BOOL fBypassRActorChanges);
void ACDReapplyVisualInventory(ACDID idACD);
void CACDSetRareMonsterNamesAndAffixes(ActorCommonData & tACD, const GBID * pgbidRareNames, const GBID * pgbidMonsterAffixes);
// X/Client/CBlizzconTimer.cpp
void CBlizzconVictory();
// X/Client/CDebugDraw.cpp
void CDebugDrawProcessMessage(DebugDrawPrimMessage & tMessage);
// X/Client/CGame.cpp
void CGameStartTick(uint32 dwTick);
void CGameFirstClock(const uint32 dwFirstClock);
CGameGlobals & CGameGlobalsGet();
void CGameSetLoadComplete(Player * ptPlayer);
void CGameReceiveHeartbeat(uint32 dwHeartBeatTime);
GameCommonData & CGameGetGameCommonData();
SWorldID CGameGetCurrentSWorldID();
BOOL CGameInScriptedSequence();
void CGameLoadingPartRequired(const LoadingComponent eComponent);
void CGameLoadingScreenEnable(const LoadingComponent eComponent);
uint32 CGameGetPredictedServerTime();
void CGameLoadingScreenSetBlackOnly(BOOL bBlackOnly);
// X/Client/CGizmo.cpp
void GizmoOperateWaypoint(RActorID idRActor, const int32 nControllerIndex);
void GizmoConfirmOperation(ACDID idACDGizmo, Player * ptPlayer);
void GizmoActivateWaypointVisual(RActorID idRActor);
// X/Client/CHero.cpp
namespace CHero {
void OnGameFinished(Player * ptPlayer);
void SyncRandomSeeds(Player * ptPlayer, GenericBlobMessage * ptMessage);
void OnCharacterKilled(Player * ptPlayer, SNO snoKiller, MonsterRarity eKillerRarity, const GBID * pRareNameGBIDs, const CRefString & usKiller);
void OnAccountFlagsSync(Player * ptPlayer, uint32 uAccountFlags);
void OnCounterIncrement(Player * ptPlayer, const ConsoleCounterIncrementMessage & tIncrementMsg);
void OnSavedQuestUpdate(Player * ptPlayer, const PlayerSavedQuestUpdateMessage & tUpdateMsg);
void SyncRandomCubeSeeds(Player * ptPlayer, GenericBlobMessage * ptMessage);
void OnQuestHistoryUpdate(Player * ptPlayer, const PlayerQuestHistoryUpdateMessage & tUpdateMsg);
void OnAchievementsAwarded(Player * ptPlayer, const D3::GameMessage::AchievementsAwardedConsole & tUpdateMsg);
void OnAdventureModeUnlocked(Player * ptPlayer);
void OnChallengeRiftComplete(int32 nUserIndex, uint64 idLeaderboard, uint32 uRiftNumber, uint64 uScore);
void OnSavedConversationUpdate(Player * ptPlayer, const PlayerSavedConversationsUpdateMessage & tUpdateMsg);
void OnQuestRewardHistoryUpdate(Player * ptPlayer, const PlayerQuestRewardHistoryUpdateMessage & tUpdateMsg);
void OnAchievementCriteriaUpdate(int32 nControllerIndex, const D3::GameMessage::AchievementsCriteriaUpdateConsole & tUpdateMsg);
void OnChallengeRiftRewardEarned(int32 nUserIndex, D3::Account::ConsoleChallengeRiftReward tReward);
void OnChallengeRiftRewardClaimed(int32 nUserIndex, uint32 uRewardId);
void Save(int32 nControllerIndex, uint32 uReason);
}  // namespace CHero
// X/Client/CItems.cpp
void ItemSetIsBeingManipulated(ActorCommonData & tACDItem, const BOOL fVal, const uint32 dwTimeout);
// X/Client/CPlayer.cpp
void CPlayerWarped(const PlayerWarpReason eReason, float flWarpFadeInSecs);
BOOL PlayerIsLocal(const ACDID idACD);
BOOL PlayerIsLocal(RActorID idRActor);
BOOL PlayerIsLocal(const Player * ptPlayer);
BOOL PlayerIsLocal(int32 nPlayerIndex);
Player * LocalPlayerGet();
void CPlayerAddPetAt(Player & tPlayer, int32 nIndex, ACDID idACDPet, PetType eType);
void CPlayerSayError(uint32 dwSoundTag, RActorID idRActor, BOOL bIgnoreCooldown);
void CPlayerNewPlayer(const NewPlayerMessage & tNewPlayerMsg, int32 nControllerIndex);
void CPlayerEnterKnown(const int32 nPlayerIndex, const ACDID idACDPlayer, RActorID idRActorPlayer);
void CPlayerHandlePong(uint32 uPingSequence);
ACDID LocalPlayerACDGet();
void CPlayerLevelMessage(int32 nPlayerIndex, int32 nNewLevel);
void CPlayerRemovePlayer(const Player & tPlayer);
void CPlayerUpdateBanner(const PlayerAccountBanner & tBannerConfiguration, BannerRenderData & tBannerRenderData, BOOL fIsNamePlate);
RActorID LocalPlayerRActorGet();
CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound);
void CPlayerHandleSentMail(uint64 nMailId, int errCode);
Player * LocalPlayerGetPrimary();
void PlayersAddLocalPlayer(const int32 nControllerIndex, const int32 nPlayerIndex, const uint32 dwSyncedAnimSeed);
void PlayersSetLocalPlayer(const int32 nControllerIndex, const int32 nPlayerIndex, const uint32 dwSyncedAnimSeed);
void CPlayerAddCalloutEvent(const CalloutEvent & tEvent);
void CPlayerAltLevelMessage(int32 nPlayerIndex, int32 nNewAltLevel);
void CPlayerHandleFetchMail(const D3::GameMessage::MailContents & tMessage);
void CPlayerLocalPlayerLeft(int32 nControllerIndex);
void CPlayerSetFriendStatus(Player * ptPlayer);
void CPlayerSendPrefsToServer(Player * ptPlayer);
void PlayerRegisterFriendGift(Player * ptPlayer, ACDID idACD, int32 nOwnerIndex, const BOOL bAvenger);
uint32 PlayerGetLocalPlayerCount();
int32 PlayerLocalIndexToPlayerIndex(const int32 nLocalPlayerIndex);
void CPlayerAddFloatingAmountNumber(const int32 nControllerIndex, const WorldPlace & wpPlace, const int32 nAmount, const int32 nAmount2, const FloatingNumberType eType);
void CPlayerAddFloatingDamageNumber(const ActorCommonData & tACD, float flDamage, FloatingNumberType eType);
void CPlayerGetNewFriendGiftRecipient(Player * ptPlayer);
void CPlayerUpdateUpscaleNotification(Player * ptPlayer, int32 nUpscaleLevel);
void CPlayerAddFloatingGBIDAmountNumber(const int32 nControllerIndex, const WorldPlace & wpPlace, const int32 nAmount, const GBID gbidItem);
void CPlayerHandleMailOperationResponse(const D3::GameMessage::MailOperationResponse & tResponse);
void CPlayerHighestHeroSoloRiftLevelMessage(int32 nPlayerIndex, int32 nNewRiftLevel);
// X/Client/CPlayerInput.cpp
void CPlayerInputPickupFailed(BOOL bIsCurrency, const PickupResponse ePickupResponse, Player * ptPlayer);
void CPlayerInputCancelACDTarget(const ACDID idACD, Player * ptPlayer);
void CPlayerInputResetLoopingPower(Player * ptPlayer);
void CPlayerInputClearClientWalkPower(Player * ptPlayer);
void CPlayerInputPlayedNonPreplayedAnimPower(Player * ptPlayer);
// X/Client/CTimedEvents.cpp
void CTimedEventStart(const ActiveEvent & tEvent);
void CTimedEventFinished(const SNO snoTimedEvent);
void CTimedEventModified(const ActiveEvent & tModifiedEvent);
// X/Client/CTrade.cpp
void CTradeCancel(const Player * ptLocalPlayer, BOOL bForce, BOOL bSendMessage);
void CTradeHandleMessage(Player * ptLocalPlayer, TradeMessage & tMessage);
// X/Client/CVisualInventory.cpp
void VisualInventoryApply(RActor * ptRActor, VisualInventory & tVisualInventory, const VisualInventoryType eVisualInventoryType, const PlayerClass ePlayerClass, const BOOL fFemale);
void VisualInventoryNewEquipment(const ACDID idACD, VisualEquipment & tEquipment);
// X/Client/CWorld.cpp
void CWorldEnter(SWorldID idSWorld, GameMode eMode);
void CWorldChange(SWorldID idSWorld);
BOOL CWorldIsRetro();
void CWorldSetStatus(const SWorldID idSWorld, const uint32 dwWorldStatusBits);
CWorldID ClientWorldCreate(SNO snoWorld, SWorldID idSWorld, GameMode eMode, int32 nOriginX, int32 nOriginY, int32 nStitchSizeInFeetX, int32 nStitchSizeInFeetY, int32 nWorldSizeInFeetX, int32 nWorldSizeInFeetY, SNO snoDungeonFinderSourceWorld);
void ClientWorldDispose(CWorldID idCWorld);
SWorldID CWorldGetCurrentWorld();
CWorld * CWorldGetFromSWorldID(SWorldID idSWorld);
// X/Client/Camera/CameraManager.cpp
SNO CameraManagerGetDefaultObserver(BOOL bPlayingGameModeOnly);
// X/Client/Camera/Cinematography.cpp
void CinematographyFocus(const RActorID idTargetRActor, BOOL bSnapFocus, float flDuration);
void CinematographySetYaw(const Angle aAngle, BOOL bSnap, float flDuration);
void CinematographySetOrbit(const WorldPlace & wpToOrbit);
void CinematographyFixedView(RActorID idTargetRActor);
void CinematographyClearOrbit(BOOL bForce);
void CinematographySetObserver(const SNO snoObserver);
void CinematographyResetCameras();
void CinematographySetDefaultZoom(const float flZoom, const float flZoomSpeed);
void CinematographyCutToStaticView(const SNO snoObserver, const Vector3D & wpCenter);
void CinematographySetScriptedZoom(const float flZoom, const BOOL bSnap, const float flZoomDuration);
// X/Client/Console/GamerProfileInternal.cpp
namespace Console { namespace GamerProfile {
void OnChallengeUnlocked(int32 nUserIdx, OnlineService::AchievementId idAchievement);
}}  // namespace Console::GamerProfile
// X/Client/Console/GamerProfileNX64.cpp
namespace Console { namespace GamerProfile {
D3::Account::SavedDefinition & GetAccount(int32 nUserIdx);
const char * GetGamerTag(int32 nUserIdx);
void AwardAchievement(int32 nUserIdx, uint32 nAchievementId);
int32 GetPrimaryProfileUserIndex();
}}  // namespace Console::GamerProfile
// X/Client/Console/LobbyService.cpp
namespace Console { namespace Online {
void LeaderboardPostRiftTime(int32 nUserIndex, const D3::Leaderboard::PostScore & tScoreFromServer, Console::Online::LeaderboardPostCallback pfnCallback);
}}  // namespace Console::Online
// X/Client/Console/MailService.cpp
namespace Console { namespace Online {
void MailSend(int32 nUserIndex, Console::Online::MailType eType, const D3::Console::MailItem & tMail, bdOnlineUserID idDestinationUser, Console::Online::SendMailCallback pfnCallback);
}}  // namespace Console::Online
// X/Client/Console/NX64NFP.cpp
namespace Console { namespace NFP {
bool IsPlayerScanDesired(ACDID idPlayerACD);
void SetPlayerScanDesired(ACDID idACDPlayer, bool bIsScanDesired, ACDNetworkName annScanningPortal);
bool IsAnyPlayerScanDesired();
void ClearAllPlayerScansDesired();
}}  // namespace Console::NFP
// X/Client/Console/Online.cpp
namespace Console { namespace Online {
void PartyLeave();
void PartyEndGame();
void PartyLeaveAndRemake(uint32 uFlags);
}}  // namespace Console::Online
// X/Client/Console/PresenceManager.cpp
namespace Console { namespace Online {
BOOL PresenceGetRandomFriend(int32 nUserIndex, Console::Online::Friend & tOutFriend, bdOnlineUserID idBlacklistPlayer);
}}  // namespace Console::Online
// X/Client/Cutscene.cpp
void CutscenePlayActCutscene(const Act eAct, const BOOL fUseNoLogoVersion, CutsceneCallback pfnOnFinishedCallback, int32 nControllerIndex);
void CutsceneUnlockFinalCinematic(const BOOL fExpansion);
void CutscenePlayAnniversaryCinematic(const BOOL fIsMale, CutsceneCallback m_pfnOnFinishedCallback);
void CutscenePlayInterstitialCutscene(const Act eAct, const int32 nIndex, const PlayerClass ePlayerClass, const BOOL fIsMale, int32 nControllerIndex, CutsceneCallback pfnOnFinishedCallback);
void CutscenePlayFinalCinematicForLocalPlayer(const BOOL fExpansion, CutsceneCallback pfnOnFinishedCallback);
// X/Client/Definitions/PreferencesDefinition.cpp
namespace Prefs {
const GameplayPrefs & GetGameplayPrefs();
}  // namespace Prefs
// X/Client/EffectGroup.cpp
void EffectGroupCreateUntracked(const SNO snoEffectGroup, const RActorID idRActorSource, const RActorID idRActorDest, const uint32 dwAttachmentID, const BOOL bIgnoreOwnerAlpha, const BOOL bRemote);
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);
// X/Client/Network/CNetwork.cpp
void CNetworkClientDied(int32 nPlayerIndex);
void CNetworkPlayerDisconnected(int32 nPlayerIndex);
void CNetworkServerAckedResurrect();
// X/Client/Network/CTrickle.cpp
void CTrickleRemove(const ACDNetworkName ann);
void CTrickleUpdate(const TrickleMessage * pTrickleMessage);
// X/Client/Network/Client.cpp
void ClientMarkSeasonEnded();
void ClientReceiveHeartbeat(uint32 dwHeartBeatTime);
void ClientPutMessageOnIncomingQueue(uint8 * pMessage);
void ClientMarkChallengeRiftsDisabled();
// X/Client/PostFX.cpp
void PostFXFadeStart(const PostFXFadeParams & tFadeParams);
void PostFXFadeDisablePermanentFade();
// X/Client/Prefetch.cpp
namespace Prefetch {
BOOL AddToQueue(const SNOName & snoname, SNOFilesAsyncPriority ePriority);
void BlockUntilPriorityIdle(SNOFilesAsyncPriority ePriority);
}  // namespace Prefetch
// X/Client/Quests/CQuests.cpp
void CQuestsFlagACD(ActorCommonData & tACD);
void CQuestsUnFlagACD(ActorCommonData & tACD);
void CQuestsUpdateLog(const QuestUpdateMessage & tMsg);
void CQuestsUpdateMeter(const QuestMeterMessage & tMsg);
void CQuestsUpdateCounters(SNO snoQuest, SNO snoLevelArea, int32 nStepUID, int32 nObjectiveIndex, int32 nValue, BOOL bPassed);
void CQuestsOnInventoryUpdate(const ActorCommonData & tACD);
// X/Client/RActors/CComplexEffect.cpp
CComplexEffectID CComplexEffectAdd(const SComplexEffectID idSComplexEffect, const SNO snoEffectGroup, const ACDID idACDSource, const ACDID idACDDest, ComplexEffectType eType, uint32 dwParam1, uint32 dwParam2, BOOL bIgnoreOwnerAlpha);
// X/Client/RActors/RActorACDEffects.cpp
RopeID RActorACDRopeEffect(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const Vector3D & wpDest, const BOOL bIgnoreOwnerAlpha);
RopeID RActorACDRopeEffect(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const ACDID idACDTo, const RopePosition ePositionTo, const BOOL bIgnoreOwnerAlpha);
// X/Client/RActors/RActorAnimation.cpp
void RActorAnimationLookAt(RActorID idSource, RActorID idTarget);
// X/Client/RActors/RActorAttachments.cpp
void RActorPlayRemainingKeyframedRMessages(RActor * ptRActor);
// X/Client/RActors/RActors.cpp
void RActorDelete(RActorID idRActor);
void RActorFlippy(RActorID idRActor, const Vector3D & wpDest, const SNO snoFlippyParticle, const SNO snoActorLoot);
RActorID RActorFindByANN(const ACDNetworkName idANN);
void RActorACDExitKnown(ActorCommonData & tACD);
RActorID RActorCreateForACD(const ACDID idACDOwner, const SNO snoActor);
void RActorSetAimTarget(const RActorID idRActor, const ACDID idACDTarget, const TargetType eTargetType, const WorldPlace & tPlace);
void RActorSetFadeTimes(RActor * ptRActor, const float flTimeUntilFade, const float flFadeTime, const BOOL bRecurseForAttachments);
void RActorACDEnterKnown(const ACDEnterKnownMessage & tACDEnterMsg);
void RActorAddLookOverride(RActor * ptRActor, uint32 nLookHash);
void RActorChangeLookIndex(RActor & tRActor, const int32 nNewLook, const BOOL bOverridenLook);
void RActorProjectileStick(RActor * ptRActor, const SNO snoActorToStick, const Vector3D & wpSourceLoc);
int32 RActorGetCurrentWaypoint(const RActorID idRActor);
void RActorRemoveLookOverride(RActor * ptRActor, uint32 nLookHash);
void RActorRevertToNativeLook(RActor & tRActor);
void RActorLocalPlayerActorSet(const ACDID idACDPlayer, Player & tPlayer);
void RActorResetDeathFadeTimes(RActor * ptRActor);
void RActorRemoveNearestRagdoll(const ACDID idACDTarget, const SNO snoMonster);
void RActorPlayEffectFromMessage(const RActorID idRActor, const EffectType eEffectType, const int32 nEffectParameter, const BOOL bRemoteEffect);
void RActorUpdateClientTransform(RActor * ptRActor, const ActorDefinition * pdefActor, const ActorCommonData * ptACD);
int32 RActorGetLookOverrideLookIndex(RActor * ptRActor);
void RActorPlayHitEffectFromMessage(const RActorID idRActor, const ACDID idACDAttacker, const DamageType eDamageType, const BOOL fCrit);
BOOL RActorSetACDLocationFromMessage(ActorCommonData & tACD, const ActorLocation & tNewLocation, BOOL bFirstTime, const BOOL fNewItem, const BOOL fCreateActor);
void RActorResetActorEffectsForAttributes(RActor * ptRActor);
void RActorPlayHitEffectOverrideFromMessage(RActor * ptRActor, const ACDID idACDAttacker, const SNO snoEffectGroup);
// X/Client/RWindow.cpp
RWindowMgr * GetRWindowMgr();
// X/Client/SNOPreload.cpp
namespace SNOPreload {
void UpdateSkills();
void AddClientActor(const ACDNetworkName ann, const SNO snoActor, const WeaponClass eWeaponClass, const GBID * pgbidMonsterAffixes);
void AddClientScene(const SSceneID idSScene, const SNO snoScene, const SNO * ptSNOLevelAreas);
void UpdateCrafters();
void UpdateEquipped();
void UpdateHireling();
void RemoveClientActor(const ACDNetworkName ann);
void RemoveClientScene(const SSceneID idSScene);
BOOL PrefetchClientActor(const SNO snoActor, const GBID * pgbidMonsterAffixes, SNOFilesAsyncPriority ePriority);
BOOL PrefetchClientScene(const SNO snoScene, SNOFilesAsyncPriority ePriority);
BOOL PrefetchClientWorld(const SNO snoWorld, SNOFilesAsyncPriority ePriority);
BOOL PrefetchClientLevelArea(const SNO snoLevelArea, SNOFilesAsyncPriority ePriority);
}  // namespace SNOPreload
// X/Client/Scene.cpp
void CSceneSwap(const SwapSceneMessage & tSwapSceneMessage);
void CSceneReveal(const RevealSceneMessage & tRevealSceneMessage);
void CSceneDestroy(const DestroySceneMessage & tDestroySceneMessage);
// X/Client/Sound/Sound.cpp
namespace SoundManager {
void StopMusicOverlay();
void PlayMusic(const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle);
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
// X/Client/System/CVoteKick.cpp
namespace VoteKick { namespace Client {
void DoVoteKick(const VoteKickMessage * ptMessage);
void Failed(uint32 nPlayerIndex);
void Passed(uint32 nPlayerIndex);
void StartKick(uint32 nPlayerIndex);
}}  // namespace VoteKick::Client
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console { namespace Notifications {
void ShowAmiiboTutorial(int32 nControllerIndex);
void ShowLeaveGameDialog(int32 nControllerIndex, BOOL fShow, uint32 uMinTickDuration, uint32 uMaxTickDuration, uint32 uKickTickDuration);
}}  // namespace Console::Notifications
namespace Console { namespace Screens {
BOOL OpenToScreen(const Console::Screens::ScreenSets eScreenSet, const Console::Screens::ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition);
void CloseScreenSet(BOOL fClearScreenSetHistory, BOOL fSkipTransition);
BOOL IsScreenSetOpen();
FanfareManager & GetFanfareManager();
void CloseAllScreenSets(BOOL bSkipTransitions);
ConsoleModalNotificationManager & GetModalNotificationManager();
}}  // namespace Console::Screens
// X/Client/UserInterface/Console/UIConsoleBlacksmith.cpp
namespace UIBlacksmith { namespace Console {
void OnSalvageResults(const SalvageResultsMessage * pMsg);
void OnCraftingResults(const CraftingResultsMessage * pMsg);
void OnTrainingResults(const IntDataMessage * pMsg);
void OnBlacksmithChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce);
void OnRepair();
}}  // namespace UIBlacksmith::Console
// X/Client/UserInterface/Console/UIConsoleBloodShardVendor.cpp
namespace UIBloodShardVendor { namespace Console {
void OnItemDelivered(const ANNDataMessage * pMsg);
void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce);
}}  // namespace UIBloodShardVendor::Console
// X/Client/UserInterface/Console/UIConsoleEquipmentManager.cpp
namespace UIEquipmentManager {
void OnPlayerLoadoutDataMessageReceived(int32 nLoadoutIndex);
}  // namespace UIEquipmentManager
// X/Client/UserInterface/Console/UIConsoleEventQueue.cpp
namespace UIEventQueue { namespace Console {
void InvalidateUI();
void ReportItemPickup(const Player * ptPlayer, const ActorCommonData & tACDItem);
void DisplaySeasonEndingMessage(int32 nMin);
void DisplayLongMessageForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tImage, BOOL bUseLargeIcon);
}}  // namespace UIEventQueue::Console
// X/Client/UserInterface/Console/UIConsoleFastEquip.cpp
namespace UIFastEquip { namespace Console {
void OnPlayerLeft(const int32 nPlayerIndex);
void CloseForPlayer(const int32 nPlayerIndex);
}}  // namespace UIFastEquip::Console
// X/Client/UserInterface/Console/UIConsoleFollower.cpp
namespace UIFollower { namespace Console {
void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce);
}}  // namespace UIFollower::Console
// X/Client/UserInterface/Console/UIConsoleHUD.cpp
namespace UIHUD { namespace Console {
void PlayerLeft();
void PlayerChangedWorld(Player * ptPlayer);
void MarkSkillSlotAsChanged(const Player * ptPlayer, SkillButton eSkillButton);
}}  // namespace UIHUD::Console
// X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
namespace UIInventory { namespace Console {
void OnPlayerLeft();
}}  // namespace UIInventory::Console
// X/Client/UserInterface/Console/UIConsoleJewelUpgrade.cpp
namespace UIJewelUpgrade {
void UpgradeResults(const JewelUpgradeResultsMessage * ptMsg);
}  // namespace UIJewelUpgrade
// X/Client/UserInterface/Console/UIConsoleJeweler.cpp
namespace UIJeweler { namespace Console {
void OnJewelerChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce);
void OnCraftingResults(const CraftingResultsMessage * pMsg);
void OnTrainingResults(const IntDataMessage * pMsg);
}}  // namespace UIJeweler::Console
// X/Client/UserInterface/Console/UIConsoleJoinRift.cpp
namespace UIJoinRift {
void InviteStatusUpdate(const EncounterInviteStateMessage * ptStateMsg);
void RemotePlayersDeclined(int32 nDeclinedPlayerMap);
void Open(const RiftJoinMessage * ptRiftJoinMsg);
void Started();
void Canceled();
}  // namespace UIJoinRift
// X/Client/UserInterface/Console/UIConsoleLore.cpp
namespace UILore { namespace Console {
void OnLoreAcquired(const SNO snoLore);
}}  // namespace UILore::Console
// X/Client/UserInterface/Console/UIConsoleMail.cpp
namespace UIMail { namespace Console {
void PlayerGiftFailed(Player * ptReceivingPlayer);
void PlayerGiftRedeemed(Player * ptReceivingPlayer, D3::Items::Generator & tItemData);
}}  // namespace UIMail::Console
// X/Client/UserInterface/Console/UIConsoleMultiplayerDeathMenu.cpp
namespace UIMultiplayerDeathMenu { namespace Console {
void PlayerLeft(const int32 nControllerIndex);
}}  // namespace UIMultiplayerDeathMenu::Console
// X/Client/UserInterface/Console/UIConsoleMystic.cpp
namespace UIMystic { namespace Console {
void OnMysticChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce);
void OnDyeItemResults(const DyeItemMessage & tDyeItemResults);
void OnTrainingResults(const IntDataMessage * pMsg);
void OnTransmogResults(const TransmogItemMessage & tTransmogResults);
void OnEnchantAffixChoose(const EnchantAffixChooseMessage * ptMessage);
void OnEnchantAffixResults(const ANNDataMessage * ptMessage);
void OnUnlockTransmogResults(const GBIDDataMessage * ptMessage);
}}  // namespace UIMystic::Console
// X/Client/UserInterface/Console/UIConsoleNPC.cpp
namespace UINPC { namespace Console {
void InteractMenuEnable(const int32 nControllerIndex, ActorCommonData & tACDTalkingTo, const NPCInteraction * ptNPCInteraction, const int32 nNumInteractions, const NPCMenu ePreferredMenu);
void InteractMenuDisable(const int32 nControllerIndex);
}}  // namespace UINPC::Console
// X/Client/UserInterface/Console/UIConsolePartyTab.cpp
namespace UIParty { namespace Console {
void OnInspectAck(const int32 nPlayerIndex);
}}  // namespace UIParty::Console
// X/Client/UserInterface/Console/UIConsoleRiftEndScreen.cpp
namespace UIConsoleRiftEndScreen {
void Open(const int32 nControllerIndex, D3::GameMessage::RiftEndScreenInfo & tRiftEndScreenInfo);
}  // namespace UIConsoleRiftEndScreen
// X/Client/UserInterface/Console/UIConsoleSeasonJourney.cpp
namespace UISeasonJourney { namespace Console {
void GiftRedeemedResult(int32 nControllerIndex, BOOL bResult);
}}  // namespace UISeasonJourney::Console
// X/Client/UserInterface/Console/UIConsoleSeasonStash.cpp
namespace UISeasonStash { namespace Console {
void ItemRetrievedResult(int32 nControllerIndex, BOOL bResult);
}}  // namespace UISeasonStash::Console
// X/Client/UserInterface/Console/UIConsoleStash.cpp
namespace UIInventory { namespace Console {
void OnStashChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bForce);
}}  // namespace UIInventory::Console
// X/Client/UserInterface/Console/UIConsoleTransmute.cpp
namespace UITransmute {
void OnTransmuteFailed(const IntDataMessage & tMessage);
void OnTransmuteResults(const TransmuteResultsMessage & tMessage);
}  // namespace UITransmute
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils { namespace Console {
void UpdateRecipes(const Player * ptPlayer, const CrafterType eType, const BOOL fInitialInfo, const D3::ItemCrafting::CrafterData & tNewCrafterData, const CrafterData & tOldCrafterData);
void OpenArtisanScreen(ActorCommonData & tACDArtisan, const int32 nControllerIndex);
}}  // namespace UIUtils::Console
// X/Client/UserInterface/Console/UIConsoleWaypointMap.cpp
namespace UIWaypointMap {
void SetAct3HubEntryPointAct(Act eAct);
void Open(Player * ptPlayer, RActorID idRActor, Act eSelectedAct, BOOL bForTutorial);
}  // namespace UIWaypointMap
// X/Client/UserInterface/Console/UIInventoryConsole.cpp
namespace UIInventory { namespace Console {
void UpdateHeroModel();
void OnInventoryChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bCosmeticChanged, BOOL bForce);
void OnCraftingMaterialsChanged();
}}  // namespace UIInventory::Console
// X/Client/UserInterface/Console/UIShopConsole.cpp
namespace UIInventory { namespace Console {
void OnShopChanged(const ActorLocation & tOldLoc, const ActorLocation & tNewLoc, BOOL bCosmeticChanged, BOOL bForce);
}}  // namespace UIInventory::Console
namespace UIShop { namespace Console {
void Refresh(const Player * ptPlayer);
void OnRepair();
}}  // namespace UIShop::Console
// X/Client/UserInterface/Console/UISkillsConsole.cpp
namespace UISkills { namespace Console {
void SetNewSkillKit(const int32 nControllerIndex);
void OnInventoryChanged(const ActorLocation &, const ActorLocation & tNewLoc, BOOL);
void Refresh(const Player * ptPlayer);
}}  // namespace UISkills::Console
// X/Client/UserInterface/Controls/UITextControl.cpp
void UIText_RemoveFormattingCharacters(CRefString & usString, BOOL fPreserveHyperlinks, BOOL fValidateHyperlinks);
// X/Client/UserInterface/Interface/UIBlizzconEnd.cpp
namespace UIBlizzconEnd {
void Show(int32 nScreenToShow);
}  // namespace UIBlizzconEnd
// X/Client/UserInterface/Interface/UIBossElements.cpp
namespace UIBossEncounter {
void Enter_Open(Player * ptPlayer, const BossEncounterMessage * ptMsg);
void Enter_Close();
void OneDeclined(Player * ptPlayer, int32 nDeclinedPlayerIndex);
void JoinParty_Open(Player * ptPlayer, const BossEncounterMessage * ptMsg);
void InviteStatusUpdate(const EncounterInviteStateMessage * pState);
void Started();
void Canceled(BOOL bShowError);
}  // namespace UIBossEncounter
// X/Client/UserInterface/Interface/UIBuffs.cpp
namespace UIBuffs {
void PlayerLeftGame(const Player * ptPlayer);
void CleanUpBuffIconsForSkillButtons();
}  // namespace UIBuffs
// X/Client/UserInterface/Interface/UICallouts.cpp
namespace UICallouts {
BOOL IsTickerType(const KillCounterType eType);
void IncreasedCount(CalloutEvent * ptEvent);
void RefreshMassacreTimer();
}  // namespace UICallouts
// X/Client/UserInterface/Interface/UIChallenge.cpp
namespace UIChallenge {
void Show(SNO snoChallenge, BOOL bJoinExisting);
}  // namespace UIChallenge
// X/Client/UserInterface/Interface/UIConfirmation.cpp
namespace UIConfirmation {
void Close();
}  // namespace UIConfirmation
// X/Client/UserInterface/Interface/UIConversations.cpp
namespace UIConversation {
void EndConversation(const SNO snoConversation, const ACDNetworkName annSpeaker, const int32 nPlayerTalkingIndex);
void Pause(BOOL fPause);
void PlayLine(const ACDID idACDSpeaker, const ACDID * pidACDParticipants, const PlayLineParams & tParams, const uint32 dwTimeToDisplay);
void StopLine(const int32 nLineGUID, const BOOL fStoppedByUser);
}  // namespace UIConversation
// X/Client/UserInterface/Interface/UICountDown.cpp
namespace UICountDown {
void Show(float flDuration, LPCSTR szMessage, UIFunctionPtr pfnOnExpire, UIFunctionPtr pfnOnCanceled, BOOL bCancelOnMovement, BOOL bPositionHigh, SNO snoTimedEvent);
void Close();
BOOL IsVisible();
}  // namespace UICountDown
// X/Client/UserInterface/Interface/UIFollowerSwap.cpp
namespace UIFollowerSwap {
void Show(const HirelingClass eNewClass, int32 nControllerIndex, BOOL bForAutoDismiss);
void Close();
BOOL IsVisible();
}  // namespace UIFollowerSwap
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame {
void AddTagToACD(ACDID idACD, LPCSTR szUIControlName, LPCSTR szText, const SNO snoFont, const int32 nFontSize, const RGBAColor rgbaText, const RGBAColor rgbaDropShadow, const uint32 dwDuration, const float flYOffset);
void SetGameText(const CRefString & usMsg);
void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer);
void StartTrialTimer(const uint32 dwDelay);
void SetSpecialAreaText(LPCSTR uszText, LPCSTR uszTextSubtitle);
void OnPlayerLeavingGame(const Player * ptPlayerLeaving);
void HideLoopingAnimMeter(const int32 nControllerIndex);
void DisplayItemRequiredError(const GBID gbidItem, const StringLabelHandle hLabel);
void FlashTieredRiftProgressBar(float flProgress);
void ShowNewCrafterNotification(Player * ptPlayer, CrafterType eCrafterType);
void ShowNewHirelingNotification(Player * ptPlayer, HirelingClass eHirelingClass);
void ChallengeRift_SetTargetTimeNotification();
}  // namespace UIGame
// X/Client/UserInterface/Interface/UIGoldCounter.cpp
namespace UIGoldCounter {
void Expired(uint32 nGold);
void NewAmount(uint32 nGold);
}  // namespace UIGoldCounter
// X/Client/UserInterface/Interface/UIHero.cpp
namespace UIHero {
KilledByType GetKillerName(int32 nKillerPlayerIndex, SNO snoActor, SNO snoPowerDmgSource, MonsterRarity eMonsterRarity, const GBID * arKillerRareNameGBIDs, CRefString & strKiller, CRefString & strPower);
}  // namespace UIHero
// X/Client/UserInterface/Interface/UIHireling.cpp
namespace UIHireling {
void UpdateVisualEquipmentForHirelingNPC(const ActorCommonData & tACD);
}  // namespace UIHireling
// X/Client/UserInterface/Interface/UIInventory.cpp
namespace UIInventory {
void ApplySwapIndicator(const ActorCommonData & tACDItem);
void NotifyOfCurrencyPickup(Player & tPlayer, CurrencyType eCurrencyType);
void AddCraftingNotification(ACDNetworkName annItem);
void NotifyOfNewCraftingMatCurrency();
BOOL IsOpen();
}  // namespace UIInventory
// X/Client/UserInterface/Interface/UILoreCommon.cpp
namespace UILore {
void Pause(BOOL fPause);
void RefreshUI();
}  // namespace UILore
namespace UILoreNotify {
void OnLoreAcquired(const SNO snoLore);
}  // namespace UILoreNotify
namespace UILorePlayer {
void Close(const UIID &, BOOL bCancelLore);
}  // namespace UILorePlayer
// X/Client/UserInterface/Interface/UIMinimap.cpp
void UIMinimapUpdateZoom(const BOOL bSnapZoomLevel);
void UIMinimap_AddMarker(const MapMarkerInfoMessage * ptMsg);
void UIMinimapRevealScene(const SSceneID idSScene, const SNO snoScene, const PRTransform & transform, const uint8 * pInitialExploredData, const SWorldID idSWorld, const BOOL bRevealed);
void UIMinimap_EnterKnown(const ACDID idACD);
void UIMinimap_SetObjective(const MapMarkerInfoMessage * ptMsg);
void UIMinimap_DisplayANNName(ACDNetworkName ann, float flSeconds);
void UIMinimap_DisposeForWorld(const SWorldID idSWorld);
void UIMinimap_SetObjectiveCone(const ActorCommonData * ptACD);
// X/Client/UserInterface/Interface/UIQuestComplete.cpp
namespace UIRewards {
void ShowTieredRiftReward(const D3::Quests::QuestReward & tReward, const BOOL bFinishedInTime, const int32 nRiftDepth, const int32 nControllerIndex);
}  // namespace UIRewards
// X/Client/UserInterface/Interface/UIQuests.cpp
namespace UIQuests {
void OnChangeAct();
void QueueUpdate(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID, const BOOL bDisplayButton, const BOOL bFailed, const int32 nPreviousStepUID, const int32 nObjectiveIndex, const int32 nValue, const BOOL bPassed);
}  // namespace UIQuests
// X/Client/UserInterface/Interface/UISetDungeon.cpp
namespace UISetDungeon {
void OneDeclined(Player * ptPlayer, int32 nDeclinedPlayerIndex);
void ShowResults(const SetDungeonResultsMessage * ptResultsMsg);
void JoinParty_Open(Player * ptPlayer, const SetDungeonDialogMessage * ptMsg);
void InviteStatusUpdate(const EncounterInviteStateMessage * pState);
void ShowCannotStartMessage(Player * ptPlayer);
void Show(StringLabelHandle hLabelDescription, StringLabelHandle hLabelTitle, Player * ptPlayer);
void Started();
void Canceled(BOOL);
}  // namespace UISetDungeon
// X/Client/UserInterface/Interface/UIStartup.cpp
void UIStartup_GetHeroData(UIHeroData & tData);
void UIStartup_ClientDisconnected(const char * pszTitle, const char * pszText);
// X/Client/UserInterface/Interface/UIVictory.cpp
namespace UIVictory {
void Show(const UIVictory::Mode eMode);
}  // namespace UIVictory
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
void UI_ResetForPlayerMovement(Player * ptPlayer);
void UI_Print(LPCSTR szText, const BOOL fEchoToConsole);
void UI_Reset(bool bExpectUIRestore);
// X/Client/UserInterface/UIID.cpp
const UIID & GetUIIDNone();
// X/Client/UserInterface/UIMasterIdList.cpp
void UISideScreenCloseAll(uint32 dwSideScreenAlterMask);
void UISideScreenSetVisible(const UIElement eElement, const BOOL fVisible);
BOOL UISideScreenAnyAreVisible();
// X/Client/Weather.cpp
namespace Weather {
void TriggerLightning(WeatherRWindowData & tData);
}  // namespace Weather
// X/Common/Act.cpp
void ActCacheWaypoints(const Act eAct);
Act ActGetWaypointActByLevelArea(const SNO snoLevelArea);
// X/Common/ActorCommonCollision.cpp
void ACDCollDisposeSphereNode(ActorCommonData & tACD);
// X/Common/ActorCommonData.cpp
ACDID ACDFromANN(const ACDNetworkName ann);
ActorType ActorGetType(const SNOName & tSNOName);
BOOL ACDIsHireling(const ActorCommonData & tACD);
ACDID ACDGetPetOwner(ACDID idACDPet);
HirelingClass ACDGetHirelingClass(const ActorCommonData & tACD);
ActorCommonData * ActorCommonDataIterate(ACDIterator & tIterator);
// X/Common/ActorInventory.cpp
BOOL ACDInventoryInitialize(ActorCommonData & tACD, BOOL fForceInventoryCreate);
// X/Common/AnimPlayer.cpp
void AnimPlayerPlayIdle(ActorCommonData & tACD);
AnimReason AnimPlayerGetAnimReason(const ActorCommonData & tACD);
uint32 AnimPlayerPlayAnimation(ActorCommonData & tACD, uint32 dwAnimationTag, uint32 dwLength, BOOL bUseAnimationTranslation, AnimReason eAnimReason, float flUnitAnimStartTime, const BOOL bDontSendToClient, float flSpeedScalar);
void AnimPlayerWeaponSetChanged(ActorCommonData & tACD);
void AnimPlayerUpdateGatePosition(ActorCommonData & tACD);
void AnimPlayerIdleAnimationChanged(ActorCommonData & tACD, const uint32 dwAnimTag);
BOOL AnimPlayerShouldPlayAllMessages(ActorCommonData & tACD);
void AnimPlayerPlayAnimationFromMessage(ActorCommonData & tACD, const PlayAnimationMessage & tAnimMsg);
// X/Common/Attributes/AttributeCompressor.cpp
FastAttribKey AttributeCompresserAttribKeyFromNetAttributeKeyValue(const NetAttributeKeyValue & tKeyVal);
// X/Common/Attributes/FastAttribute.cpp
void FastAttribSetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey, float flValue);
// X/Common/CombatLog.cpp
namespace CombatLog {
void LogEvent(CombatLog::LogEventType eType, ACDID idACD1, ACDID idACD2, SNO snoData, float flParam1, float flParam2, uint32 dwParam1);
}  // namespace CombatLog
// X/Common/Console/ConsoleGameMessage.cpp
namespace Console { namespace Client {
BOOL MessageRecieve(void * pMessage);
}}  // namespace Console::Client
// X/Common/Crafting/ItemCrafting.cpp
namespace ItemCrafting_BackEnd {
CrafterType CrafterTypeFromMessageType(GameMessageType eType, BOOL & fInitialInfo);
}  // namespace ItemCrafting_BackEnd
// X/Common/Definitions/ActorDefinition.cpp
int32 ActorDefinitionGetTagMapValueInt(const ActorTag eTag, const SNO snoActor);
// X/Common/Definitions/LevelAreaDefinition.cpp
CRefString LevelAreaGetLocalizedName(SNO snoLevelArea);
// X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel);
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel, BOOL & bSuccess);
CRefString StringListFetch(const SNO snoStringList, LPCSTR szLabel);
BOOL StringListFetch(const SNO snoStringList, LPCSTR szLabel, CRefString & usReturnString);
CRefString LocalizedSprintf(LPCSTR cpFmtStr, ...);
CRefString StringListEntryReplace(LPCSTR uszFormatString, LPCSTR uszEscapeSequence, LPCSTR uszReplacement, int32 nMaxReplacements);
BOOL StringListFetchByLabel(const SNO snoStringList, const StringLabelHandle hLabel, CRefString & usReturn);
// X/Common/Game.cpp
BOOL GameIsStartUp();
void GameStartMenu(Act eLoadingScreenAct);
void AppDrawFlagSet(int32 nFlag, BOOL bValue);
GameType GameGetGameType();
BOOL AppDrawFlagIsSet(int32 nFlag);
BOOL GameCommonDataExists();
// X/Common/GameCommonData.cpp
BOOL GameIsOpenWorld();
Act GameGetCurrentAct();
uint32 GameGetServerTime();
OpenWorldMode GameGetOpenWorldMode();
BOOL GameIsWeeklyChallenge();
BOOL GameCommonPlayerIsPartyGuide(const Player & tPlayer);
// X/Common/GameTemplate.cpp
BOOL GameTypeIsPvP();
// X/Common/GameTesting.cpp
namespace GameTesting {
TestType GetTestType();
void OnHeartBeat(uint32 dwHeartBeatTime);
void OnGameBNetSetup();
void StopFPSCollection(BOOL finishLog);
uint32 GetDumpFPSFequency();
void ReceivedTestingDone();
BOOL IsAutomatedBNetTesting();
BOOL StartSkillFPSCollection(LPCSTR szFilename, SNO snoPower, int32 nRuneIndex, uint32 nDumpFPSFreq, BOOL bDumpFPSFormatJSON);
BOOL StartWorldFPSCollection(LPCSTR szFilename, SNO snoScene, uint32 nDumpFPSFreq, BOOL bDumpFPSFormatJSON);
}  // namespace GameTesting
// X/Common/GlobalSNO.cpp
SNO GlobalSNOGet(const GlobalSNO eGlobalSNO);
GBID GlobalGBIDGet(GlobalGBID eGlobalGBID);
// X/Common/Hireling.cpp
void CHirelingSetAppearance(RActor * ptRActor, HirelingClass eClass, int32 nLevel);
// X/Common/Items.cpp
BOOL ItemIsGold(const ActorCommonData & tACDItem);
BOOL ItemIsHealthPotion(const ActorCommonData & tACDItem);
// X/Common/Leaderboard.cpp
bool LeaderboardIsChallengeRift(uint64_t leaderboardId);
bool LeaderboardDecompressMessage(const blz::string & compressed, google::protobuf::Message * msg);
// X/Common/Monsters.cpp
CRefString MonsterGetName(const SNOName & tActorSNOName, const GBID * pRareNameGBIDs, MonsterRarity eRarity, const BOOL bSetTextColor);
MonsterFamily MonsterGetMonsterFamily(const SNOName & tActorSNOName);
// X/Common/NPC.cpp
CrafterType NPCGetCrafterType(const ActorCommonData & tACDVendor);
// X/Common/Networking/GameMessage.cpp
BOOL ParseProtocolBufferMessage(GameMessageType eType, uint8 * pMessageBuffer, google::protobuf::MessageLite & tMessage);
// X/Common/Networking/Network.cpp
uint32 NetworkGetProtocolHash();
// X/Common/OnlineService/OnlineService.cpp
namespace OnlineService {
BOOL IsPTR();
}  // namespace OnlineService
// X/Common/Player.cpp
BOOL ACDIsPlayer(const ACDID idACD);
BOOL PlayerIsMale(const Player & tPlayer);
Player * PlayerGetByACD(const ACDID idACD);
Player * PlayerGetByANN(const ACDNetworkName ann);
void PlayerRemovePet(Player & tPlayer, ACDID idACDPet, BOOL bDisplayChatMessage);
uint32 PlayersGetCount();
Player * PlayerGetNextAll(const Player * ptPlayer);
Player * PlayerGetFirstAll();
Player * PlayerGetNextInGame(const Player * ptPlayer);
int32 PlayerGetNumPlayers();
PlayerAvengerData * GetPlayerAvengerData(Player * ptPlayer, AvengerSource eAvengerSource);
Player * PlayerGetFirstInGame();
PlayerClass PlayerGetPlayerClass(const ACDID idACD);
Player * PlayerGetByPlayerIndex(int32 nPlayerIndex);
void PlayerAssignToTraitSlot(Player & tPlayer, const SNO snoPower, const int32 nSlot);
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
CalloutEventType CalloutEventFromKillCounter(const KillCounterType eKillCounterType);
void PlayerAssignToActiveSkillSlot(Player & tPlayer, const SNO snoPower, const RuneType eRune, const int32 nSlot);
void PlayerAssignToLegendaryPowerSlot(Player & tPlayer, const GBID gbidLegendaryItem, const int32 nSlot);
Player * PlayerGetByControllerIndexAlways(int32 nControllerIndex);
// X/Common/PortalCommon.cpp
void CPortalSpecifierInitialize(ACDID idACD, const ResolvedPortalDestination & tSpecifier);
// X/Common/Quests/Quests.cpp
CRefString QuestsGetUIName(SNO snoQuest);
BOOL QuestIsACDObjective(const ActorCommonData & tACD);
void QuestsActivateForAct(Act eAct);
QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACDPlayer);
BOOL QuestIsAnyCurrentKillQuest();
void QuestsAddOpenWorldTrackedSNO(SNO sno);
// X/Common/SNO/SNOFiles.cpp
uint32 SNOGetCoreDataHash();
BOOL SNOPackChunkLoadAct(Act eAct);
void SNOFilesSetCurrentAct(Act eAct);
// X/Common/Shell/CmdLine.cpp
// X/Common/TagMaps/Tags.cpp
int32 TagMapGetInt(const CTagMap * pTagMap, uint32 dwTag);
BOOL TagMapTagExists(const CTagMap * pTagMap, uint32 dwTag);
// X/Common/TeamCommon.cpp
void CRevealTeamMessage(void * pMessage);
CRefString CTeamGetPvPEndGameText(Team eWinningTeam);
TeamColor TeamGetEffectGroupIndex(Team eTeam);
// X/Common/Trade.cpp
BOOL TradeIsTradingWith(const ACDID idACDPlayer, const ACDID idACDTradingWith);
// X/Common/Tutorials.cpp
namespace Tutorial {
BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial);
void Shown(Player & tPlayer, const SNO snoTutorial);
}  // namespace Tutorial
// X/Common/UserInterface/UITextControl.cpp
void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor);
// X/Common/VisualInventory.cpp
VisualInventory * VisualInventoryGet(const ACDID idACD);
BOOL VisualInventoryDetectCosmeticChange(const ACDID idACD, const VisualEquipment & tEquipment);
// X/Sigma/ErrorManager/ErrorManager.cpp
void DisplayInternalError(LPCSTR pszMessage, LPCSTR szFile, int32 nLine, ErrorCode eErrorCode);
// X/Sigma/ErrorManager/ErrorManagerNX64.cpp
void ExitProgram(ErrorCode eCode);
// X/Sigma/LanguageRules.cpp
namespace LanguageRules {
void SetClientGender(const BOOL fMale);
const char * ApplyLanguageRules(const char * szUTF8Str);
}  // namespace LanguageRules
// X/Sigma/Memory/SigmaMemory.cpp
void SigmaMemoryCopy(void * lpDest, const void * lpSrc, size_t size);
void SigmaMemoryFree(void * lp, XMemoryAllocatorInterface * ptMemoryInterface);
void SigmaMemoryClear(void * lp, size_t size);
void operator delete[](void * pData);
void * operator new(size_t size);
// X/Sigma/UnicodeText.cpp
namespace UnicodeText {
CRefString FormatNumber(const int32 nNumber, const BOOL fGroupDigits, const FormatNumberStyle eStyle, XLocale eXLocale);
}  // namespace UnicodeText
// member  LPCSTR CRefString::Format(LPCSTR szFormat, ...)   [_ZN10CRefString6FormatEPKcz  X/Sigma/RefString.cpp:845]
// member  CRefString::CRefString(LPCSTR szString)   [_ZN10CRefStringC1EPKc  X/Sigma/RefString.cpp:467]
// member  CRefString::CRefString()   [_ZN10CRefStringC1Ev  X/Sigma/RefString.cpp:440]
// member  CRefString::~CRefString()   [_ZN10CRefStringD1Ev  X/Sigma/RefString.cpp:808]
// member  CRefString & CRefString::operator=(LPCSTR szString)   [_ZN10CRefStringaSEPKc  X/Sigma/RefString.cpp:986]
// member  CRefString & CRefString::operator=(const CRefString & szString)   [_ZN10CRefStringaSERKS_  X/Sigma/RefString.cpp:963]
// member  CRefString & CRefString::operator+=(LPCSTR szString)   [_ZN10CRefStringpLEPKc  X/Sigma/RefString.cpp:1011]
// member  void VisualItem::Set(const GBID _gbidItem, const DyeType _eDyeType, const ItemEffectType _eItemEffectType, const int32 _nEffectLevel)   [_ZN10VisualItem3SetEiiii  X/Common/VisualInventory.cpp:1139]
// member  void CrafterData::Import(const D3::ItemCrafting::CrafterData & tData)   [_ZN11CrafterData6ImportERKN2D312ItemCrafting11CrafterDataE  X/Common/Crafting/ItemCrafting.cpp:407]
// member  void UIInventory::Console::NewItemIndicatorManager<ACDID>::Reset()   [_ZN11UIInventory7Console23NewItemIndicatorManagerI5ACDIDE5ResetEv  X/Client/UserInterface/Console/UIConsoleNewItemIndicators.h:87]
// member  CalloutEvent::CalloutEvent(const CalloutEventType eType, const uint32 uXPRewarded)   [_ZN12CalloutEventC1Eij  X/Client/CPlayer.cpp:2413]
// member  void CActorMovement::SyncToPosition(const Vector3D & vPos, const BOOL bSnap)   [_ZN14CActorMovement14SyncToPositionERK8Vector3Di  X/Client/Movement/CActorMovement.cpp:2017]
// member  void CActorMovement::SendAckResponse(uint32 uAckValue)   [_ZN14CActorMovement15SendAckResponseEj  X/Client/Movement/CActorMovement.cpp:1366]
// member  void CActorMovement::ProcessFacingMessage(ACDTranslateFacingMessage & tMessage)   [_ZN14CActorMovement20ProcessFacingMessageER25ACDTranslateFacingMessage  X/Client/Movement/CActorMovement.cpp:1639]
// member  void CActorMovement::ProcessSnappedMessage(ACDTranslateSnappedMessage & tMessage)   [_ZN14CActorMovement21ProcessSnappedMessageER26ACDTranslateSnappedMessage  X/Client/Movement/CActorMovement.cpp:1570]
// member  void CActorMovement::ProcessTranslateMessage(ACDTranslateNormalMessage & tMessage)   [_ZN14CActorMovement23ProcessTranslateMessageER25ACDTranslateNormalMessage  X/Client/Movement/CActorMovement.cpp:1420]
// member  void CActorMovement::ProcessFixedUpdateMessage(ACDTranslateFixedUpdateMessage & tMessage)   [_ZN14CActorMovement25ProcessFixedUpdateMessageER30ACDTranslateFixedUpdateMessage  X/Client/Movement/CActorMovement.cpp:1684]
// member  void CActorMovement::ProcessTranslateArcMessage(ACDTranslateArcMessage & tMessage)   [_ZN14CActorMovement26ProcessTranslateArcMessageER22ACDTranslateArcMessage  X/Client/Movement/CActorMovement.cpp:1711]
// member  void CActorMovement::ProcessTranslateFixedMessage(ACDTranslateFixedMessage & tMessage)   [_ZN14CActorMovement28ProcessTranslateFixedMessageER24ACDTranslateFixedMessage  X/Client/Movement/CActorMovement.cpp:1672]
// member  void CActorMovement::ProcessTranslateDetPathMessage(ACDTranslateDetPathMessage & tMessage)   [_ZN14CActorMovement30ProcessTranslateDetPathMessageER26ACDTranslateDetPathMessage  X/Client/Movement/CActorMovement.cpp:1746]
// member  void CActorMovement::ProcessTranslateStoppedMessage(ACDTranslateStoppedMessage & tMessage)   [_ZN14CActorMovement30ProcessTranslateStoppedMessageER26ACDTranslateStoppedMessage  X/Client/Movement/CActorMovement.cpp:1848]
// member  void CActorMovement::ProcessTranslateDetPathSinMessage(ACDTranslateDetPathSinMessage & tMessage)   [_ZN14CActorMovement33ProcessTranslateDetPathSinMessageER29ACDTranslateDetPathSinMessage  X/Client/Movement/CActorMovement.cpp:1775]
// member  void CActorMovement::ProcessTranslateDetPathSpiralMessage(ACDTranslateDetPathSpiralMessage & tMessage)   [_ZN14CActorMovement36ProcessTranslateDetPathSpiralMessageER32ACDTranslateDetPathSpiralMessage  X/Client/Movement/CActorMovement.cpp:1802]
// member  void CActorMovement::ProcessTranslateDetPathPieWedgeMessage(ACDTranslateDetPathPieWedgeMessage & tMessage)   [_ZN14CActorMovement38ProcessTranslateDetPathPieWedgeMessageER34ACDTranslateDetPathPieWedgeMessage  X/Client/Movement/CActorMovement.cpp:1826]
// member  void FanfareManager::SendFanfare(OnlineService::AchievementId idAchievement, int32 nRank, const PlatinumAchievementAwardedMessage * ptMessage)   [_ZN14FanfareManager11SendFanfareEmiPK33PlatinumAchievementAwardedMessage  X/Client/UserInterface/Console/ConsoleFanfareManager.cpp:257]
// member  void FanfareManager::ClearFanfareQueue()   [_ZN14FanfareManager17ClearFanfareQueueEv  X/Client/UserInterface/Console/ConsoleFanfareManager.cpp:759]
// member  void FanfareManager::DisplayFanfareForPlayer(uint32 nPlayerIndex, OnlineService::AchievementId idAchievement)   [_ZN14FanfareManager23DisplayFanfareForPlayerEjm  X/Client/UserInterface/Console/ConsoleFanfareManager.cpp:151]
// member  ActorLocation & ActorCommonData::GetLocation()   [_ZN15ActorCommonData11GetLocationEv  X/Common/ActorCommonData.cpp:2476]
// member  void ActorCommonData::SetGBHandle(const GBHandle _hGB)   [_ZN15ActorCommonData11SetGBHandleE8GBHandle  X/Common/ActorCommonData.cpp:2830]
// member  BOOL ActorCommonData::GetHasNFPDevice()   [_ZN15ActorCommonData15GetHasNFPDeviceEv  X/Common/ActorCommonData.cpp:9138]
// member  ActorMovement * ActorCommonData::GetActorMovement()   [_ZN15ActorCommonData16GetActorMovementEv  X/Common/ActorCommonData.cpp:987]
// member  WorldLocation & ActorCommonData::GetWorldLocation()   [_ZN15ActorCommonData16GetWorldLocationEv  X/Common/ActorCommonData.cpp:1449]
// member  void ActorCommonData::AttributesUpdateCached()   [_ZN15ActorCommonData22AttributesUpdateCachedEv  X/Common/ActorAttributes.cpp:31]
// member  const void * GroupDefinition::LockR(const SNO sno, const BOOL fLockCanFail)   [_ZN15GroupDefinition5LockREii  X/Common/Definitions/Definitions.cpp:2370]
// member  void GroupDefinition::UnlockR(void *& lpDef)   [_ZN15GroupDefinition7UnlockRERPv  X/Common/Definitions/Definitions.cpp:2575]
// member  const void * GroupDefinition::LockRFast(const SNO sno)   [_ZN15GroupDefinition9LockRFastEi  X/Common/Definitions/Definitions.cpp:2425]
// member  void UIQuestComplete::OnQuestReward(const D3::Quests::QuestReward & tReward, const BOOL bCompleted, const BOOL bEquipped, const int32 nControllerIndex)   [_ZN15UIQuestComplete13OnQuestRewardERKN2D36Quests11QuestRewardEiii  X/Client/UserInterface/Interface/UIQuestComplete.cpp:184]
// member  void UIQuestComplete::OnMultiplayerQuestReward(const int32 nControllerIndex, const D3::Quests::QuestReward & tReward, const BOOL bCompleted, const BOOL bEquipped)   [_ZN15UIQuestComplete24OnMultiplayerQuestRewardEiRKN2D36Quests11QuestRewardEii  X/Client/UserInterface/Interface/UIQuestComplete.cpp:247]
// member  void VisualEquipment::Init()   [_ZN15VisualEquipment4InitEv  X/Common/VisualInventory.cpp:415]
// member  void VisualInventory::InitializeLast()   [_ZN15VisualInventory14InitializeLastEv  X/Common/VisualInventory.cpp:444]
// member  void CosmeticItemData::Import(const D3::CosmeticItems::CosmeticItems & tData)   [_ZN16CosmeticItemData6ImportERKN2D313CosmeticItems13CosmeticItemsE  X/Common/CosmeticItems.cpp:76]
// member  void CurrencySavedData::Import(const D3::Items::CurrencySavedData & tCurrencySavedData, BOOL bResetCurrency)   [_ZN17CurrencySavedData6ImportERKN2D35Items17CurrencySavedDataEi  X/Common/Player.cpp:3925]
// member  ImageTextureFrame::ImageTextureFrame(LPCSTR szDecoratedLabel)   [_ZN17ImageTextureFrameC1EPKc  X/Client/Gfx/GfxCommon.cpp:132]
// member  UITutorial * UITutorialManager::ShowTutorial(const UIID & idDesiredParent, const SNO snoTutorial, const Player * ptPlayer, const BOOL fIgnoreIfAlreadyBeingTracked)   [_ZN17UITutorialManager12ShowTutorialERK4UIIDiPK6Playeri  X/Client/UserInterface/Interface/UITutorials.cpp:199]
// member  void UITutorialManager::ShowTutorialFromMessage(const D3::GameMessage::TutorialMessage & tMessage)   [_ZN17UITutorialManager23ShowTutorialFromMessageERKN2D311GameMessage15TutorialMessageE  X/Client/UserInterface/Interface/UITutorials.cpp:559]
// member  void CrafterTransmogData::Import(const D3::GameBalance::BitPackedGbidArray & tPartitionData, const D3::GameBalance::BitPackedGbidArray * ptAccountData)   [_ZN19CrafterTransmogData6ImportERKN2D311GameBalance18BitPackedGbidArrayEPS3_  X/Common/Crafting/ItemCrafting.cpp:554]
// member  void PlayerAccountBanner::Import(const D3::Account::BannerConfiguration & tBanner)   [_ZN19PlayerAccountBanner6ImportERKN2D37Account19BannerConfigurationE  X/Common/PlayerAccount.cpp:129]
// member  NotificationCallbacks::~NotificationCallbacks()   [_ZN21NotificationCallbacksD2Ev  X/Client/Battlenet/Subsystems/ModalNotificationManager.h:55]
// member  void ExtractedLegendariesData::SetToDefaults()   [_ZN24ExtractedLegendariesData13SetToDefaultsEv  X/Common/Items.cpp:888]
// member  void ExtractedLegendariesData::Import(const D3::GameBalance::BitPackedGbidArray & tPartitionData)   [_ZN24ExtractedLegendariesData6ImportERKN2D311GameBalance18BitPackedGbidArrayE  X/Common/Items.cpp:912]
// member  NotificationCreationParams::NotificationCreationParams()   [_ZN26NotificationCreationParamsC1Ev  X/Client/Battlenet/Subsystems/ModalNotificationManager.cpp:41]
// member  ModalNotificationId ModalNotificationManagerBase::SetOkNotification(const NotificationCreationParams & tNotificationParam)   [_ZN28ModalNotificationManagerBase17SetOkNotificationERK26NotificationCreationParams  X/Client/Battlenet/Subsystems/ModalNotificationManager.cpp:160]
// member  D3::GameBalance::BitPackedGbidArray::BitPackedGbidArray()   [_ZN2D311GameBalance18BitPackedGbidArrayC1Ev  X/Common/Protobuf/GameBalance.pb.cc:409]
// member  D3::GameMessage::MailContents::MailContents()   [_ZN2D311GameMessage12MailContentsC1Ev  X/Common/Protobuf/GameMessage.pb.cc:23241]
// member  D3::GameMessage::PlayerBanner::PlayerBanner()   [_ZN2D311GameMessage12PlayerBannerC1Ev  X/Common/Protobuf/GameMessage.pb.cc:5265]
// member  D3::GameMessage::TutorialState::TutorialState()   [_ZN2D311GameMessage13TutorialStateC1Ev  X/Common/Protobuf/GameMessage.pb.cc:7194]
// member  D3::GameMessage::TutorialMessage::TutorialMessage()   [_ZN2D311GameMessage15TutorialMessageC1Ev  X/Common/Protobuf/GameMessage.pb.cc:6882]
// member  D3::GameMessage::RiftEndScreenInfo::RiftEndScreenInfo()   [_ZN2D311GameMessage17RiftEndScreenInfoC1Ev  X/Common/Protobuf/GameMessage.pb.cc:40115]
// member  D3::GameMessage::MailOperationResponse::MailOperationResponse()   [_ZN2D311GameMessage21MailOperationResponseC1Ev  X/Common/Protobuf/GameMessage.pb.cc:24083]
// member  D3::GameMessage::AchievementsAwardedConsole::AchievementsAwardedConsole()   [_ZN2D311GameMessage26AchievementsAwardedConsoleC1Ev  X/Common/Protobuf/GameMessage.pb.cc:42192]
// member  D3::GameMessage::RiftEndScreenAddParticipants::RiftEndScreenAddParticipants()   [_ZN2D311GameMessage28RiftEndScreenAddParticipantsC1Ev  X/Common/Protobuf/GameMessage.pb.cc:39437]
// member  D3::GameMessage::AchievementsCriteriaUpdateConsole::AchievementsCriteriaUpdateConsole()   [_ZN2D311GameMessage33AchievementsCriteriaUpdateConsoleC1Ev  X/Common/Protobuf/GameMessage.pb.cc:41971]
// member  D3::GameMessage::RiftStartPreloadingLeaderBoardData::RiftStartPreloadingLeaderBoardData()   [_ZN2D311GameMessage34RiftStartPreloadingLeaderBoardDataC1Ev  X/Common/Protobuf/GameMessage.pb.cc:40887]
// member  D3::Leaderboard::CompressedPostScore::CompressedPostScore()   [_ZN2D311Leaderboard19CompressedPostScoreC1Ev  X/Common/Protobuf/Leaderboard.pb.cc:5184]
// member  D3::Leaderboard::PostScore::PostScore()   [_ZN2D311Leaderboard9PostScoreC1Ev  X/Common/Protobuf/Leaderboard.pb.cc:4590]
// member  D3::ItemCrafting::CrafterData::CrafterData()   [_ZN2D312ItemCrafting11CrafterDataC1Ev  X/Common/Protobuf/ItemCrafting.pb.cc:189]
// member  D3::CosmeticItems::CosmeticItems::CosmeticItems()   [_ZN2D313CosmeticItems13CosmeticItemsC1Ev  X/Common/Protobuf/CosmeticItems.pb.cc:442]
// member  D3::Items::CurrencySavedData::CurrencySavedData()   [_ZN2D35Items17CurrencySavedDataC1Ev  X/Common/Protobuf/Items.pb.cc:5450]
// member  D3::Items::Generator::Generator()   [_ZN2D35Items9GeneratorC1Ev  X/Common/Protobuf/Items.pb.cc:873]
// member  D3::Quests::QuestStepComplete::QuestStepComplete()   [_ZN2D36Quests17QuestStepCompleteC1Ev  X/Common/Protobuf/Quest.pb.cc:663]
// member  D3::Quests::QuestTieredRiftComplete::QuestTieredRiftComplete()   [_ZN2D36Quests23QuestTieredRiftCompleteC1Ev  X/Common/Protobuf/Quest.pb.cc:983]
// member  D3::Account::ConsoleData::ConsoleData()   [_ZN2D37Account11ConsoleDataC1Ev  X/Common/Protobuf/Account.pb.cc:7234]
// member  D3::Account::ConsoleChallengeRiftReward::ConsoleChallengeRiftReward(const D3::Account::ConsoleChallengeRiftReward & from)   [_ZN2D37Account26ConsoleChallengeRiftRewardC1ERKS1_  X/Common/Protobuf/Account.pb.cc:6566]
// member  D3::Account::ConsoleChallengeRiftReward::ConsoleChallengeRiftReward()   [_ZN2D37Account26ConsoleChallengeRiftRewardC1Ev  X/Common/Protobuf/Account.pb.cc:6557]
// member  D3::Console::AvengerMail::AvengerMail()   [_ZN2D37Console11AvengerMailC1Ev  X/Common/Protobuf/ConsoleMail.pb.cc:679]
// member  D3::Console::MailItem::MailItem()   [_ZN2D37Console8MailItemC1Ev  X/Common/Protobuf/ConsoleMail.pb.cc:1112]
// member  void Avenger::Console::TryToGetFriendAvenger(BOOL bSendToServer)   [_ZN7Avenger7Console21TryToGetFriendAvengerEi  X/Common/Console/Avenger.cpp:393]
// member  void Avenger::Console::VictimsCopyMessageToArray(const AvengerInfoMessage & tMessage, AvengerVictimArray & arVictims)   [_ZN7Avenger7Console25VictimsCopyMessageToArrayERK18AvengerInfoMessageRN3blz6vectorI13AvengerVictimNS4_9allocatorIS6_EEEE  X/Common/Console/Avenger.cpp:973]
// member  void Avenger::Console::VictimsCopyArrayToProtobuf(const AvengerVictimArray & arVictims, google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> * arProtobuf)   [_ZN7Avenger7Console26VictimsCopyArrayToProtobufERKN3blz6vectorI13AvengerVictimNS1_9allocatorIS3_EEEEPN6google8protobuf16RepeatedPtrFieldIN2D37Account13AvengerVictimEEE  X/Common/Console/Avenger.cpp:1029]
// member  void Avenger::Console::InitializePlayerAvengerData(Player * ptPlayer, D3::Account::SavedDefinition & tDefinition)   [_ZN7Avenger7Console27InitializePlayerAvengerDataEP6PlayerRN2D37Account15SavedDefinitionE  X/Common/Console/Avenger.cpp:130]
// member  Console::Online::LobbyService * Console::Online::LobbyService::Get(int32 nUserIdx)   [_ZN7Console6Online12LobbyService3GetEi  X/Client/Console/LobbyService.cpp:886]
// member  size_t Blizzard::String::Copy(char * dest, const char * source, size_t length)   [_ZN8Blizzard6String4CopyEPcPKcm  BlizzardCore/Source/System/String/String.cpp:48]
// member  LPCSTR CRefString::str() const   [_ZNK10CRefString3strEv  X/Sigma/RefString.cpp:825]
// member  size_t CRefString::Length() const   [_ZNK10CRefString6LengthEv  X/Sigma/RefString.cpp:835]
// member  CRefString::operator const char *() const   [_ZNK10CRefStringcvPKcEv  X/Sigma/RefString.cpp:815]
// member  BOOL ActorMovement::IsMoving() const   [_ZNK13ActorMovement8IsMovingEv  X/Common/Movement/ActorMovement.cpp:1857]
// member  BOOL GameplayPrefs::GetDisableCutscenes() const   [_ZNK13GameplayPrefs19GetDisableCutscenesEv  X/Client/Definitions/PreferencesDefinition.cpp:2563]
// member  const OnlineService::AchievementDefinition * OnlineService::Achievements::GetAchievementDefinition(OnlineService::AchievementId id) const   [_ZNK13OnlineService12Achievements24GetAchievementDefinitionEm  X/Common/OnlineService/Achievements.cpp:272]
// member  GameMode GameCommonData::GameModeGet() const   [_ZNK14GameCommonData11GameModeGetEv  X/Common/GameCommonData.cpp:265]
// member  SNO ActorCommonData::GetActorSNO() const   [_ZNK15ActorCommonData11GetActorSNOEv  X/Common/ActorCommonData.cpp:4730]
// member  GizmoType ActorCommonData::GetGizmoType() const   [_ZNK15ActorCommonData12GetGizmoTypeEv  X/Common/Gizmo.cpp:72]
// member  BOOL ActorCommonData::IsMirrorImage() const   [_ZNK15ActorCommonData13IsMirrorImageEv  X/Common/ActorCommonData.cpp:1592]
// member  Quaternion ActorCommonData::GetOrientation() const   [_ZNK15ActorCommonData14GetOrientationEv  X/Common/ActorCommonData.cpp:2381]
// member  CurrencyType ActorCommonData::GetCurrencyType() const   [_ZNK15ActorCommonData15GetCurrencyTypeEv  X/Common/ActorCommonData.cpp:7738]
// member  int32 ActorCommonData::AttributesGetInt(FastAttribKey tKey) const   [_ZNK15ActorCommonData16AttributesGetIntE13FastAttribKey  X/Common/ActorAttributes.cpp:123]
// member  BOOL ActorCommonData::IsAlive() const   [_ZNK15ActorCommonData7IsAliveEv  X/Common/ActorCommonData.cpp:1612]
// member  BOOL ActorCommonData::IsGizmo() const   [_ZNK15ActorCommonData7IsGizmoEv  X/Common/Gizmo.cpp:165]
// member  WorldPlace ActorCommonData::GetPlace() const   [_ZNK15ActorCommonData8GetPlaceEv  X/Common/ActorCommonData.cpp:2367]
// member  float ActorCommonData::GetScale() const   [_ZNK15ActorCommonData8GetScaleEv  X/Common/ActorCommonData.cpp:2418]
// NO PROTOTYPE: _ZN2D311GameBalance18BitPackedGbidArrayD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage12MailContentsD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage12PlayerBannerD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage13TutorialStateD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage15TutorialMessageD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage17RiftEndScreenInfoD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage21MailOperationResponseD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage26AchievementsAwardedConsoleD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage28RiftEndScreenAddParticipantsD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage33AchievementsCriteriaUpdateConsoleD1Ev
// NO PROTOTYPE: _ZN2D311GameMessage34RiftStartPreloadingLeaderBoardDataD1Ev
// NO PROTOTYPE: _ZN2D311Leaderboard19CompressedPostScoreD1Ev
// NO PROTOTYPE: _ZN2D311Leaderboard9PostScoreD1Ev
// NO PROTOTYPE: _ZN2D312ItemCrafting11CrafterDataD1Ev
// NO PROTOTYPE: _ZN2D313CosmeticItems13CosmeticItemsD1Ev
// NO PROTOTYPE: _ZN2D35Items17CurrencySavedDataD1Ev
// NO PROTOTYPE: _ZN2D35Items9GeneratorD1Ev
// NO PROTOTYPE: _ZN2D36Quests17QuestStepCompleteD1Ev
// NO PROTOTYPE: _ZN2D36Quests23QuestTieredRiftCompleteD1Ev
// NO PROTOTYPE: _ZN2D37Account26ConsoleChallengeRiftRewardD1Ev
// NO PROTOTYPE: _ZN2D37Console8MailItemD1Ev
// NO PROTOTYPE: _ZN8XBaseMapIiijj20XFixedBlockAllocatorI9XMapAssocIijELi32EE14XArrayPregrownIPS2_S5_Li32EELi32EE9RemoveAllEv
// NO PROTOTYPE: _ZN8XBaseMapIjjii27XChainedFixedBlockAllocatorI9XMapAssocIjiEE14XGrowableArrayIPS2_S5_ELi0EE8RemoveAtEj

// <<< GENERATED DECLS

// ---- message definitions local to this file (X/Common/Networking/GameMessage.h) ------------
// GameMessageType values this file uses that no included header defines.
// >>> GENERATED OPCODES (build/try/agentE/mkblock.py; values from the DWARF)































































































































































































































































// <<< GENERATED OPCODES

enum {
    CHAT_COLOR_NORMAL = 0,
    CHAT_COLOR_WHISPER = 1,
    CHAT_COLOR_SHOUT = 2,
    CHAT_COLOR_ERROR = 3,
    NUM_CHAT_COLORS = 4,
};



















































static BattlenetMessageHandler s_pfnBattlenetMessageHandler;  // line 253
static uint32 s_uServerProtocolHash;  // line 254

// line 318. Removed by the linker (DW_AT_low_pc 0); it is the only writer of
// s_pfnBattlenetMessageHandler, so it is written from its DWARF signature.
void ClientMessageRegisterBattlenetHandler(BattlenetMessageHandler pfnHandler)
{
    s_pfnBattlenetMessageHandler = pfnHandler;
}

//@ 336


void ClientMessageReceiveChat(const CRefString & uSpeakerDisplayName, EChatColor eColor, LPCSTR uzFilteredMessage)
{
    if (uzFilteredMessage == NULL)
        return;

    CRefString sMessage(uzFilteredMessage);
    UIText_RemoveFormattingCharacters(sMessage, TRUE, TRUE);
    if (sMessage.Length() == 0)
        return;

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CHAT_MESSAGE), cgtPSODefaults);

    CRefString uText;
    switch (eColor)  // original line 364; whisper block 366-368, shout 372-374, says 378-380
    {
    case CHAT_COLOR_WHISPER:
        uText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WhisperedTo");
        uText = StringListEntryReplace(uText.str(), "{s1}", uSpeakerDisplayName.str(), 999);
        uText = StringListEntryReplace(uText.str(), "{s2}", sMessage.str(), 999);
        break;
    case CHAT_COLOR_SHOUT:
        uText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "Shouts");
        uText = StringListEntryReplace(uText.str(), "{s1}", uSpeakerDisplayName.str(), 999);
        uText = StringListEntryReplace(uText.str(), "{s2}", sMessage.str(), 999);
        break;
    default:
        uText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "Says");
        uText = StringListEntryReplace(uText.str(), "{s1}", uSpeakerDisplayName.str(), 999);
        uText = StringListEntryReplace(uText.str(), "{s2}", sMessage.str(), 999);
        break;
    }

    switch (eColor)  // Set_Text_Color 11 at line 387, 9 at 390, 0 at 393
    {
    case CHAT_COLOR_WHISPER:
        Set_Text_Color(uText, 11);
        break;
    case CHAT_COLOR_SHOUT:
        Set_Text_Color(uText, 9);
        break;
    default:
        Set_Text_Color(uText, 0);
        break;
    }
}

extern "C" void sub_710017a580(uint32 node);
extern "C" void sub_710017a5b0(ACDID source, ACDID player);

//@ 326. Inlined into ClientMessageReceiveGameMessage (SMSG_CHAT); body from that copy.
void ClientMessageReceiveChat(const Player & tPlayerFrom, EChatColor eColor, LPCSTR uzFilteredMessage)
{
    ClientMessageReceiveChat(CPlayerGetServiceName(tPlayerFrom, FALSE), eColor, uzFilteredMessage);
}

//@ 285. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sHandleActTransition(const Act eAct, int32 nControllerIndex)
{
    if (CmdLineGetParams()->tBitArray.Get(38))
    {
        return;
    }

    if (GameTesting::IsAutomatedBNetTesting())
    {
        return;
    }

    if (!Prefs::GetGameplayPrefs().GetDisableCutscenes() && eAct > ACT_1 && eAct <= ACT_5)
    {
        if (Console::Screens::IsScreenSetOpen())
        {
            Console::Screens::CloseScreenSet(TRUE, FALSE);
        }

        BOOL fNoLogoVersion = eAct == ACT_5;
        CutscenePlayActCutscene(eAct, fNoLogoVersion, NULL, nControllerIndex);
    }
}

//@ 407. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandleLootRunAppearedMessage(const LootRunAppearedMessage * ptMessage)
{
    CRefString sLevelArea = LevelAreaGetLocalizedName(ptMessage->snoLevelArea);

    CRefString usMessage = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "LootRunAppeared"), sLevelArea.str(), NULL);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_COMBAT_WARNING);
    UI_Print(sAfterLanguageRules, TRUE);

    if (CGameGetClientDeltaTime() > 1.0f)
    {
        SNO snoSound = GlobalSNOGet(GLOBALSNO_SOUND_LOOTRUN_APPEARED);
        if (snoSound != SNO_NONE)
        {
            SoundManager::PlaySound(snoSound, cgtPSODefaults);
        }
    }
}

//@ 433. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandleVictimMessage(const VictimMessage * ptMessage)
{
    static const FontSpecifier eTeamColors[] =  // values from the image (0x7100e36fd8, 12 bytes)
    {
        FONTCOLOR_RED,
        FONTCOLOR_BLUE,
        FONTCOLOR_WHITE,
    };

    Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerVictimIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    if (PlayerIsLocal(ptPlayer) && !(ptPlayer->tHeroStateData.uPlayerFlags & 1))
    {
        Tutorial::Shown(*ptPlayer, GlobalSNOGet(GLOBALSNO_TUTORIAL_DAMAGE_TAKEN));
    }

    int32 nVictimTeamIndex = TeamGetEffectGroupIndex(ptPlayer->eTeam);
    FontSpecifier eVictimColor = nVictimTeamIndex != -1 ? eTeamColors[nVictimTeamIndex] : FONTCOLOR_WHITE;

    MonsterRarity nRarity = ptMessage->eKillerMonsterRarity;
    if (ptMessage->eKillerTeam == ptPlayer->eTeam)
    {
        nRarity = 0;
    }

    CRefString usKiller;

    CRefString usPlayer = CPlayerGetServiceName(*ptPlayer, FALSE);
    CRefString usPower;

    KilledByType eType = UIHero::GetKillerName(ptMessage->nKillerPlayerIndex, ptMessage->snoKillerActor, ptMessage->snoPowerDmgSource, nRarity, ptMessage->pKillerRareNameGBIDs, usKiller, usPower);

    CHero::OnCharacterKilled(ptPlayer, ptMessage->snoKillerActor, ptMessage->eKillerMonsterRarity, ptMessage->pKillerRareNameGBIDs, usKiller);

    if (eType == KILLED_BY_PLAYER)
    {
        int32 nKillerTeamIndex = TeamGetEffectGroupIndex(ptMessage->eKillerTeam);
        if (nKillerTeamIndex != -1)
        {
            FontSpecifier eKillerColor = eTeamColors[nKillerTeamIndex];
            Set_Text_Color(usKiller, eKillerColor);
            Set_Text_Color(usPlayer, eVictimColor);
        }
    }
    else if (eType == KILLED_BY_NORMAL || eType == KILLED_BY_RARE)
    {
        int32 nKillerTeamIndex = TeamGetEffectGroupIndex(ptMessage->eKillerTeam);
        if (nKillerTeamIndex != -1)
        {
            FontSpecifier eKillerColor = eTeamColors[nKillerTeamIndex];
            Set_Text_Color(usKiller, eKillerColor);
        }
    }

    CRefString usMessage;
    CRefString sLabel;

    switch (eType)
    {
    case KILLED_BY_MYSTERY:
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledByMessageMystery");
        break;

    case KILLED_BY_PLAYER:
        if (usPower.Length() == 0)
        {
            usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledByMessagePvPPowerUnknown");
        }
        else
        {
            usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledByMessagePvP");
            usMessage = StringListEntryReplace(usMessage, "{POWER}", usPower, 999);
        }
        break;

    case KILLED_BY_PLAYER_PET:
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledByMessagePvPPet");
        usMessage = StringListEntryReplace(usMessage, "{POWER}", usPower, 999);
        break;

    case KILLED_BY_NORMAL:
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledByMessageNormal");
        break;

    case KILLED_BY_RARE:
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledByMessageRare");
        break;
    }

    usMessage = StringListEntryReplace(usMessage, "{Player}", usPlayer, 999);
    usMessage = StringListEntryReplace(usMessage, "{MONSTER}", usKiller, 999);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);

    if (GameTypeIsPvP())
    {
        if (PlayerIsLocal(ptPlayer))
        {
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PVP_LOCALPLAYER_ISKILLED), cgtPSODefaults);
        }
    }
    else
    {
        UI_Print(sAfterLanguageRules, TRUE);
    }
}

//@ 597. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandleEngagedMessage(const CombatEngagementMessage * ptMessage)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    CRefString usPlayerName = CPlayerGetServiceName(*ptPlayer, FALSE);
    SNOName tSNOName(ACTOR_SNO, ptMessage->snoActorMonster);
    CRefString usMonsterVictim = MonsterGetName(tSNOName, ptMessage->pRareNameGBIDs, ptMessage->eMonsterRarity, TRUE);

    SNO snoSound = SNO_NONE;
    CRefString usMessage;
    MonsterFamily eMonsterFamily = MonsterGetMonsterFamily(tSNOName);
    if (eMonsterFamily == 11)
    {
        LPCTSTR szCombatIcon = GameCommonPlayerIsPartyGuide(*ptPlayer) ? "Marker_Guide_TreasureGoblin" : "Marker_NPC_TreasureGoblin";
        usPlayerName = CRefString().Format("{icon:%s, %0.1f}%s", szCombatIcon, 2.5, usPlayerName.str());
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "EngagedEliteTreasureGoblin");

        if (ptMessage->snoActorMonster == GlobalSNOGet(GLOBALSNO_ACTOR_COW_GOBLIN))
        {
            snoSound = GlobalSNOGet(GLOBALSNO_SOUND_COW_GOBLIN_ENGAGED);
        }
        else
        {
            snoSound = GlobalSNOGet(GLOBALSNO_SOUND_GOBLIN_ENGAGED);
        }
    }
    else if (ptMessage->eMonsterRarity == 2 || ptMessage->eMonsterRarity == 4 || ptMessage->eMonsterRarity == 7)
    {
        LPCTSTR szCombatIcon = GameCommonPlayerIsPartyGuide(*ptPlayer) ? "Marker_Guide_CombatElite" : "Marker_NPC_CombatElite";
        usPlayerName = CRefString().Format("{icon:%s, %0.1f}%s", szCombatIcon, 2.5, usPlayerName.str());
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "EngagedEliteRare");

        if (PlayerGetNumPlayers() > 1)
        {
            snoSound = GlobalSNOGet(GLOBALSNO_SOUND_MONSTER_ENGAGED);
        }
    }
    else
    {
        LPCTSTR szCombatIcon = GameCommonPlayerIsPartyGuide(*ptPlayer) ? "Marker_Guide_CombatElite" : "Marker_NPC_CombatElite";
        usPlayerName = CRefString().Format("{icon:%s, %0.1f}%s", szCombatIcon, 2.5, usPlayerName.str());
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "EngagedEliteChampion");

        if (PlayerGetNumPlayers() > 1)
        {
            snoSound = GlobalSNOGet(GLOBALSNO_SOUND_MONSTER_ENGAGED);
        }
    }

    usMessage = StringListEntryReplace(usMessage, "{PLAYER}", usPlayerName, 999);
    usMessage = StringListEntryReplace(usMessage, "{MONSTER}", usMonsterVictim, 999);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_COMBAT_WARNING);
    UI_Print(sAfterLanguageRules, TRUE);

    if (snoSound != SNO_NONE)
    {
        SoundManager::PlaySound(snoSound, cgtPSODefaults);
    }
}

//@ 669. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandleEliteKilledMessage(const CombatEngagementMessage * ptMessage)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    CRefString usMessage;
    SNOName tSNOName(ACTOR_SNO, ptMessage->snoActorMonster);
    MonsterFamily eMonsterFamily = MonsterGetMonsterFamily(tSNOName);

    if (eMonsterFamily != 11 && (ptMessage->eMonsterRarity == 2 || ptMessage->eMonsterRarity == 4 || ptMessage->eMonsterRarity == 7))
    {
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledEliteRare");
    }
    else
    {
        usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KilledEliteChampion");
    }

    CRefString usPlayerName = CPlayerGetServiceName(*ptPlayer, FALSE);
    usMessage = StringListEntryReplace(usMessage, "{PLAYER}", usPlayerName, 999);

    CRefString usMonsterVictim = MonsterGetName(tSNOName, ptMessage->pRareNameGBIDs, ptMessage->eMonsterRarity, TRUE);
    usMessage = StringListEntryReplace(usMessage, "{MONSTER}", usMonsterVictim, 999);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_COMBAT_WARNING);
    UI_Print(sAfterLanguageRules, TRUE);

    SNO snoSound = GlobalSNOGet(GLOBALSNO_SOUND_ELITE_KILLED);
    if (snoSound != SNO_NONE)
    {
        SoundManager::PlaySound(snoSound, cgtPSODefaults);
    }
}

//@ 715. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandleTreasureGoblinEscapedMessage(const CombatEngagementMessage * ptMessage)
{
    CRefString usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "TreasureGoblinEscaped");

    SNOName tSNOName(ACTOR_SNO, ptMessage->snoActorMonster);
    CRefString usMonsterVictim = MonsterGetName(tSNOName, ptMessage->pRareNameGBIDs, ptMessage->eMonsterRarity, TRUE);
    usMessage = StringListEntryReplace(usMessage, "{MONSTER}", usMonsterVictim, 999);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_COMBAT_WARNING);
    UI_Print(sAfterLanguageRules, TRUE);
}

//@ 732. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandleTownPortalMessage(const PlayerIndexMessage * ptMessage)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    CRefString usPlayer = CPlayerGetServiceName(*ptPlayer, FALSE);
    CRefString usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "UsedTownPortal");
    usMessage = StringListEntryReplace(usMessage, "{PLAYER}", usPlayer, 999);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);
    UI_Print(sAfterLanguageRules, TRUE);
}

//@ 758. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandlePortedToPlayerMessage(const PortedToPlayerMessage * ptMessage)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndexPorted);
    if (ptPlayer == NULL)
    {
        return;
    }

    Player * ptPlayerTarget = PlayerGetByPlayerIndex(ptMessage->nPlayerIndexTarget);
    if (ptPlayerTarget == NULL)
    {
        return;
    }

    CRefString usPlayer = CPlayerGetServiceName(*ptPlayer, FALSE);
    CRefString usPlayerTarget = CPlayerGetServiceName(*ptPlayerTarget, FALSE);
    CRefString usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PortedToPlayer");
    usMessage = StringListEntryReplace(usMessage, "{PLAYER}", usPlayer, 999);
    usMessage = StringListEntryReplace(usMessage, "{PLAYERTARGET}", usPlayerTarget, 999);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);
    UI_Print(sAfterLanguageRules, TRUE);
}

//@ 786. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sClientHandlePortedToWaypointMessage(const PortedToWaypointMessage * ptMessage)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
    if (ptPlayer == NULL)
    {
        return;
    }

    CRefString usPlayer = CPlayerGetServiceName(*ptPlayer, FALSE);
    CRefString usLevelArea = LevelAreaGetLocalizedName(ptMessage->snoLevelArea);
    CRefString usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PortedToWaypoint");
    usMessage = StringListEntryReplace(usMessage, "{PLAYER}", usPlayer, 999);
    usMessage = StringListEntryReplace(usMessage, "{LEVELAREA}", usLevelArea, 999);

    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);
    UI_Print(sAfterLanguageRules, TRUE);
}

//@ 808. Inlined into ClientMessageReceiveGameMessage; body from that copy.
static void sClientHandleKillCountMessage(const KillCountMessage * ptMessage)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
    if (ptPlayer != NULL)
    {
        ptPlayer->nPlayerKills = ptMessage->nPlayerKills;
        ptPlayer->nDeaths = ptMessage->nDeaths;
        ptPlayer->nAssists = ptMessage->nAssists;
    }
}

//@ 828. Inlined into ClientMessageReceiveGameMessage; body from that copy, locals from the abstract DIE.
static void sSetScreenshotView(const ScreenshotView eView)
{
    ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
    if (ptACD == NULL)
    {
        return;
    }

    const WorldLocation & loc = ptACD->GetWorldLocation();
    SNO snoObserver = SNO_NONE;
    if (eView == 1 || eView == 2)
    {
        snoObserver = GlobalSNOGet(GLOBALSNO_OBSERVER_SCREENSHOT_FRONT);
    }
    else if (eView == 3)
    {
        CinematographyFixedView(LocalPlayerRActorGet());
        CinematographyResetCameras();
        return;
    }

    CinematographyCutToStaticView(snoObserver, loc.transform.wp);
}

//@ 862
static void sCutsceneCallbackXP1()
{
    UIVictory::Show(UIVictory::MODE_ETERNAL);
}

//@ 875
static void sCutsceneCallbackVanilla()
{
    UIVictory::Show(UIVictory::MODE_VANILLA);
}

//@ 883
void ClientMessageReceiveAllMessage(void * pMessage)
{
    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;

    Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
    if (ptPlayer == NULL)
    {
        ptPlayer = LocalPlayerGet();
    }

    BOOL bNetTrace = sgtAppGlobals.dwDebugFlags & 1;
    uint32 dwNow = CGameGetClientTicks();

    if (s_pfnBattlenetMessageHandler != NULL &&
        s_pfnBattlenetMessageHandler(pMessage, bNetTrace, dwNow))
    {
        return;
    }

    switch (ptHeader->eType)
    {
    case SMSG_GAME_SETUP:
        {
            GameSetupMessage * ptGameSetupMsg = (GameSetupMessage *)pMessage;
            CGameFirstClock(ptGameSetupMsg->dwFirstHeartbeat);

            SimpleMessage tMessage;
            MessageSendToServer(CMSG_FINISHED_GAME_SETUP, &tMessage, sizeof(tMessage), ptPlayer);

            CPlayerSendPrefsToServer(ptPlayer);

            if (GameTesting::IsAutomatedBNetTesting())
            {
                GameTesting::OnGameBNetSetup();
            }
        }
        break;

    case SMSG_CONNECTION_ESTABLISHED:
        {
            ConnectionEstablishedMessage * ptConnectionMsg = (ConnectionEstablishedMessage *)pMessage;
            int32 nControllerIndex = ptHeader->nControllerIndex;

            PlayersSetLocalPlayer(nControllerIndex, ptConnectionMsg->nPlayerIndex, ptConnectionMsg->dwAnimSyncedSeed);

            if (PlayerGetLocalPlayerCount() <= 1)
            {
                UIHeroData tHeroData;
                UIStartup_GetHeroData(tHeroData);
                UIBossEncounter::Canceled(0);

                CRefString sHeroFilename(tHeroData.szHeroFile);

                Player * ptPlayerConnected = PlayerGetByPlayerIndex(ptConnectionMsg->nPlayerIndex);
                Blizzard::String::Copy(ptPlayerConnected->szHeroFilename, sHeroFilename, sizeof(ptPlayerConnected->szHeroFilename));
            }
        }
        break;

    case SMSG_CONNECTION_ADD_PLAYER:
        {
            ConnectionEstablishedMessage * ptConnectionMsg = (ConnectionEstablishedMessage *)pMessage;
            int32 nControllerIndex = ptHeader->nControllerIndex;

            PlayersAddLocalPlayer(nControllerIndex, ptConnectionMsg->nPlayerIndex, ptConnectionMsg->dwAnimSyncedSeed);
        }
        break;

    case SMSG_LOGOUT_TIMER_STARTED:
        {
            LogoutTickTimeMessage * ptLogoutTimeMsg = (LogoutTickTimeMessage *)pMessage;
            int32 nControllerIndex = ptHeader->nControllerIndex;
            Console::Notifications::ShowLeaveGameDialog(
                nControllerIndex,
                1,
                ptLogoutTimeMsg->uMinWaitTicks,
                ptLogoutTimeMsg->uMaxWaitTicks,
                ptLogoutTimeMsg->uKickTicks);
        }
        break;

    case SMSG_QUIT:
        {
            QuitGameMessage * ptQuitMsg = (QuitGameMessage *)pMessage;
            int32 nControllerIndex = ptHeader->nControllerIndex;
            if (ptPlayer != NULL &&
                ptQuitMsg->eReason != QUIT_REASON_HARDCORE_DEATH && ptQuitMsg->eReason != QUIT_REASON_GAME_END)
            {
                CHero::Save(ptPlayer->nControllerIndex, 0x40000);
            }

            switch (ptQuitMsg->eReason)
            {
            case QUIT_REASON_REMOVED:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "RemovedFromGameTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "RemovedFromGameText"));
                break;

            case QUIT_REASON_KICKED_BY_PLAYERS:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KickedFromGameTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "KickedFromGameText"));
                break;

            case QUIT_REASON_MISMATCHING_PROTOCOL_VERSION:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "DisconnectedFromServerTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "ProtocolMismatchText"));
                break;

            case QUIT_REASON_JOIN_FAILED:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "UnableToJoinGameTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "UnableToJoinGameDescription"));
                break;

            case QUIT_REASON_ASSET_MISMATCH:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "DisconnectedFromServerTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "AssetMismatchDesc"));
                break;

            case QUIT_REASON_ACCOUNT_CREATE_FAILED:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "DisconnectedFromServerTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "ErrorLoadingAccount"));
                break;

            case QUIT_REASON_SHUTDOWN_GAME:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "DisconnectedFromServerTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "GameTerminated"));
                break;

            case QUIT_REASON_HARDCORE_DEATH:
                {
                    Console::Notifications::ShowLeaveGameDialog(nControllerIndex, 0, 0, 0, 0);
                    if (Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
                    {
                        for (int8 i = 0; i < 4; i++)
                        {
                            if (PlayerGetByControllerIndex(i) != NULL)
                            {
                                CHero::Save(i, 0x40000);
                            }
                        }
                        if (GameCommonDataExists())
                        {
                            GameGetActiveGameCommonData().tGameSyncedData.eAct = ACT_1;
                        }
                        CGameLoadingScreenEnable((LoadingComponent)0x1f);
                        Console::Online::PartyLeaveAndRemake(0);
                        GameStartMenu(ACT_5);
                    }
                }
                break;

            case QUIT_REASON_GAME_END:
                {
                    UICountDown::Close();

                    Console::Screens::CloseScreenSet(TRUE, FALSE);

                    if (ptPlayer != NULL)
                    {
                        CHero::OnGameFinished(ptPlayer);

                        CHero::Save(ptPlayer->nControllerIndex, 0x100000);
                    }

                    if (Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndex)
                    {
                        BOOL fExpansion = GameGetCurrentAct() == ACT_5;

                        CutsceneCallback pfnOnFinishedCallback = fExpansion ? sCutsceneCallbackXP1 : sCutsceneCallbackVanilla;
                        if (!Prefs::GetGameplayPrefs().GetDisableCutscenes())
                        {
                            CutscenePlayFinalCinematicForLocalPlayer(fExpansion, pfnOnFinishedCallback);
                        }
                        else
                        {
                            pfnOnFinishedCallback();
                        }
                    }
                }
                break;

            case QUIT_REASON_NOT_LEGAL_FOR_RESTRICTED_PLAYER:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "RestrictedPlayerNotLegal"));
                break;

            case QUIT_REASON_GAME_HOST_LEFT_NEWLY_CREATED_GAME:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "RemovedFromGameTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "LeftNewlyCreated"));
                break;

            case QUIT_REASON_GAME_ALREADY_OVER:
                UIStartup_ClientDisconnected(
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle"),
                    StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "CantJoinGameGone"));
                break;

            default:
                {
                    int32 nControllerIndexScoped = ptHeader->nControllerIndex;
                    Console::Notifications::ShowLeaveGameDialog(nControllerIndexScoped, 0, 0, 0, 0);
                    if (Console::GamerProfile::GetPrimaryProfileUserIndex() == nControllerIndexScoped)
                    {
                        Console::Online::PartyEndGame();
                        GameStartMenu(GameGetCurrentAct());
                    }
                }
                break;
            }
        }
        break;

    case SMSG_HEARTBEAT:
        {
            DWordDataMessage * ptHeartbeatMsg = (DWordDataMessage *)pMessage;
            uint32 dwHeartBeatTime = ptHeartbeatMsg->dwData;

            CGameReceiveHeartbeat(dwHeartBeatTime);

            ClientReceiveHeartbeat(dwHeartBeatTime);

            if (GameTesting::IsAutomatedBNetTesting())
            {
                GameTesting::OnHeartBeat(dwHeartBeatTime);
            }
        }
        break;

    case SMSG_VERSIONS:
        {
            VersionsMessage * ptVersionsMessage = (VersionsMessage *)pMessage;
            s_uServerProtocolHash = ptVersionsMessage->uProtocolHash;

            if (ptVersionsMessage->uSNOPackHash != SNOGetCoreDataHash() ||
                ptVersionsMessage->uProtocolHash != NetworkGetProtocolHash())
            {
                if (!GameTesting::IsAutomatedBNetTesting())
                {
                    UIStartup_ClientDisconnected(
                        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "DisconnectedFromServerTitle"),
                        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "AssetMismatchDesc"));
                }
            }
        }
        break;

    case SMSG_PONG:
        {
            DWordDataMessage * ptMsg = (DWordDataMessage *)pMessage;
            CPlayerHandlePong(ptMsg->dwData);
        }
        break;

    case SMSG_SPING:
        {
            SimpleMessage tMessage;
            MessageSendToServer(CMSG_SPONG, &tMessage, sizeof(tMessage), ptPlayer);
        }
        break;

    case SMSG_PAUSE_STATE_CHANGE:
        {
            const BoolDataMessage * ptMessage = (const BoolDataMessage *)pMessage;
            BOOL isNowPaused = ptMessage->bVal;
            BOOL wasPaused = CGameGlobalsGet().dwFlags & 0x200;
            (void)wasPaused;
            BOOL wasAnyPaused = CGameGlobalsGet().dwFlags & 0x600;
            if (isNowPaused)
                CGameGlobalsGet().dwFlags |= 0x200;
            else
                CGameGlobalsGet().dwFlags &= ~0x200;
            BOOL isAnyPaused = CGameGlobalsGet().dwFlags & 0x600;
            if (wasAnyPaused != isAnyPaused)
            {
                UIConversation::Pause(isAnyPaused);
                UILore::Pause(isAnyPaused);
            }
        }
        break;

    case SMSG_SOFT_PAUSE_STATE_CHANGE:
        {
            const BoolDataMessage * ptMessage = (const BoolDataMessage *)pMessage;
            BOOL isNowPaused = ptMessage->bVal;
            BOOL wasPaused = CGameGlobalsGet().dwFlags & 0x400;
            (void)wasPaused;
            BOOL wasAnyPaused = CGameGlobalsGet().dwFlags & 0x600;
            if (isNowPaused)
                CGameGlobalsGet().dwFlags |= 0x400;
            else
                CGameGlobalsGet().dwFlags &= ~0x400;
            if (isNowPaused)
            {
                CGameGlobalsGet().dwMaxGameTime++;
            }
            BOOL isAnyPaused = CGameGlobalsGet().dwFlags & 0x600;
            if (wasAnyPaused != isAnyPaused)
            {
                UIConversation::Pause(isAnyPaused);
                UILore::Pause(isAnyPaused);
            }
        }
        break;

    case SMSG_ACCOUNT_FLAGS_SYNC:
        {
            const DWordDataMessage * ptMessage = (const DWordDataMessage *)pMessage;
            CHero::OnAccountFlagsSync(ptPlayer, ptMessage->dwData);
        }
        break;

    case SMSG_ADVENTURE_MODE_UNLOCKED:
        CHero::OnAdventureModeUnlocked(ptPlayer);
        break;

    case SMSG_ACCOUNT_STASH_TABS_REWARDED_FOR_SEASONS_SYNC:
        break;

    default:
        ClientPutMessageOnIncomingQueue((uint8 *)pMessage);
        break;
    }
}

//@ 1544
static void sRefreshWaypoints(uint32 dwOldWaypoints, uint32 dwNewWaypoints)
{
    ACDIterator tIt;
    while (ActorCommonDataIterate(tIt))
    {
        ActorCommonData * ptACD = tIt.ptElement;

        if (ptACD->IsGizmo() && ptACD->GetGizmoType() == ACTORGIZMO_WAYPOINT)
        {
            int32 nWaypointNumber = RActorGetCurrentWaypoint((RActorID)ptACD->idOwner);

            BOOL fWasActivated = dwOldWaypoints & (1 << nWaypointNumber);
            BOOL fIsActivated = dwNewWaypoints & (1 << nWaypointNumber);
            if (!fWasActivated && fIsActivated)
            {
                GizmoActivateWaypointVisual((RActorID)ptACD->idOwner);
            }
        }
    }
}

// Classes whose headers are not in src yet: only the members this file calls are declared.
// X/Common/CombatLog.h:30
namespace CombatLog {
struct Message {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    CombatLog::LogEventType eEventType;  // 0x08
    ACDNetworkName idACDSource;  // 0x0c
    ACDNetworkName idACDTarget;  // 0x10
    SNO snoData;  // 0x14
    float flParam1;  // 0x18
    float flParam2;  // 0x1c
    uint32 dwParam1;  // 0x20
};
}  // namespace CombatLog
// X/Client/UserInterface/Interface/UITutorials.h
#include "../UserInterface/Interface/UITutorials.h"




extern UITutorialManager gtTutorialMgr;  // UITutorials.cpp
extern UIID UIDLG_SCRIPTED_SEQUENCE;  // X/Client/UserInterface/UIIDs.h
extern UIID UIWND_SCRIPTED_SEQUENCE_LETTERBOX_TOP;
extern UIID UIWND_SCRIPTED_SEQUENCE_LETTERBOX_BOTTOM;
extern UIID UIWND_EQUIPMENT_MANAGER_NAVIGATION_ARMORY;

//@ 1581
// The cases are in original source order. Three things here are not visible in the generated code but decide which
// of the ~810 branches into the shared `return TRUE` block the tail merger looks at together (it takes them 150 at
// a time, in the order of the switch's case list, last case first), so they are kept as the evidence allows:
//  - Case labels that reach no code (SMSG_AVENGER_SPAWN, SMSG_OPEN_COSMETICS, SMSG_PLATINUM_ACHIEVEMENT_AWARDED
//    below). When the optimizer drops such a label it moves the switch's last case into its place, so each one
//    reorders the list. The original has unexplained source lines at these three places and the opcodes next to
//    them are unhandled, but WHICH labels the original has there is inferred, not recorded: at least three are
//    needed, the first before SMSG_PLAY_ANIMATION and two more between SMSG_PLAY_CUTSCENE and SMSG_RIFT_START_ENCOUNTER.
//  - The early-out in SMSG_PLAYER_DESYNC_SNAP is a `return TRUE` (not `break`): it puts the shared return block
//    in that scope. The original line table has the early-out (5454) and the statement after it (5460) there.
//  - SMSG_ACTIVATE_WAYPOINT is the if/else the DWARF records (two sibling `sMessage`).
BOOL ClientMessageReceiveGameMessage(void * pMessage)
{
    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;

    BOOL bNetTrace = sgtAppGlobals.dwDebugFlags & 1;
    BOOL bNetTraceKnown = CmdLineGetParams()->tBitArray.Get(0);
    uint32 dwNow = GameGetServerTime();
    (void)bNetTrace;
    (void)bNetTraceKnown;

    if (Console::Client::MessageRecieve(pMessage))
    {
        return TRUE;
    }

    switch (ptHeader->eType)
    {
    case SMSG_PLAYER_DELETE:
        {
            PlayerIndexMessage * ptDisconnectMsg = (PlayerIndexMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptDisconnectMsg->nPlayerIndex);

            if (ptPlayer != NULL)
            {
                Player * ptLocalPlayer = LocalPlayerGet();
                if (ptLocalPlayer != NULL && TradeIsTradingWith(ptLocalPlayer->idPlayerACD, ptPlayer->idPlayerACD))
                {
                    CTradeCancel(ptLocalPlayer, FALSE, TRUE);
                }

                UIGame::OnPlayerLeavingGame(ptPlayer);

                if (PlayerIsLocal(ptDisconnectMsg->nPlayerIndex))
                {
                    UIFastEquip::Console::OnPlayerLeft(ptPlayer->nPlayerIndex);
                    CPlayerLocalPlayerLeft((uint8)ptPlayer->nControllerIndex);
                    UIGame::HideLoopingAnimMeter(ptPlayer->nControllerIndex);
                }

                CPlayerRemovePlayer(*ptPlayer);
            }

            UIBuffs::PlayerLeftGame(ptPlayer);
            CNetworkPlayerDisconnected(ptDisconnectMsg->nPlayerIndex);
            UIInventory::Console::OnPlayerLeft();
            UIHUD::Console::PlayerLeft();
            UIEventQueue::Console::InvalidateUI();

            if (ptPlayer != NULL)
            {
                UIMultiplayerDeathMenu::Console::PlayerLeft(ptPlayer->nControllerIndex);
            }

            SNOPreload::UpdateEquipped();
            SNOPreload::UpdateSkills();
        }
        break;

    case SMSG_PLAYER_HAS_DIED:
        {
            PlayerIndexMessage * ptPlayerIndexMsg = (PlayerIndexMessage *)pMessage;
            CNetworkClientDied(ptPlayerIndexMsg->nPlayerIndex);
            UIFastEquip::Console::CloseForPlayer(ptPlayerIndexMsg->nPlayerIndex);
        }
        break;

    case SMSG_ACCEPTED_DEATH_ACK:
        CNetworkServerAckedResurrect();
        break;

    case SMSG_NEW_PLAYER:
        {
            NewPlayerMessage * ptNewPlayerMsg = (NewPlayerMessage *)pMessage;

            int32 nControllerIndex = ptHeader->nControllerIndex;

            UIEventQueue::Console::InvalidateUI();

            CPlayerNewPlayer(*ptNewPlayerMsg, nControllerIndex);

            Player * ptLocalPlayer = LocalPlayerGet();
            if (ptLocalPlayer != NULL && ptLocalPlayer->eTeam == ptNewPlayerMsg->eTeam)
            {
                LanguageRules::SetClientGender(PlayerIsMale(*ptLocalPlayer));

                UIHeroData tHeroData;
                UIStartup_GetHeroData(tHeroData);
                Blizzard::String::Copy(ptLocalPlayer->szHeroFilename, tHeroData.szHeroFile, sizeof(ptLocalPlayer->szHeroFilename));
            }
        }
        break;

    case SMSG_PLAYER_KICK_TIMERS:
        {
            PlayerKickTimerMessage * ptTimerMsg = (PlayerKickTimerMessage *)pMessage;
            Player * ptLocalPlayer = LocalPlayerGet();
            ptLocalPlayer->dwLastKickAttempt = ptTimerMsg->dwLastKickAttempt;
            ptLocalPlayer->dwLastSuccessfulKick = ptTimerMsg->dwLastSuccessfulKick;
        }
        break;

    case SMSG_PLAYER_BANNER:
        {
            D3::GameMessage::PlayerBanner tBannerMessage;
            if (ParseProtocolBufferMessage(SMSG_PLAYER_BANNER, (uint8 *)pMessage, tBannerMessage))
            {
                Player * ptPlayer = PlayerGetByPlayerIndex(tBannerMessage.player_index());

                ptPlayer->tAccount.tBanner.Import(tBannerMessage.banner());
                CPlayerUpdateBanner(ptPlayer->tAccount.tBanner, ptPlayer->tBannerRenderData, FALSE);
            }
        }
        break;

    case SMSG_BOSS_ENCOUNTER_INVITE_STATE:
        {
            const EncounterInviteStateMessage * ptStateMsg = (const EncounterInviteStateMessage *)pMessage;
            UIBossEncounter::InviteStatusUpdate(ptStateMsg);
        }
        break;

    case SMSG_SET_DUNGEON_ENCOUNTER_INVITE_STATE:
        {
            const EncounterInviteStateMessage * ptStateMsg = (const EncounterInviteStateMessage *)pMessage;
            UISetDungeon::InviteStatusUpdate(ptStateMsg);
        }
        break;

    case SMSG_BOSS_ENCOUNTER_STARTED:
        UIBossEncounter::Started();
        break;

    case SMSG_BOSS_ENCOUNTER_CANCELED:
        UIBossEncounter::Canceled(TRUE);
        break;

    case SMSG_SET_DUNGEON_CANCELED:
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                ptPlayer->bSetDungeonActive = FALSE;
            }

            UISetDungeon::Canceled(TRUE);
        }
        break;

    case SMSG_SET_DUNGEON_STARTED:
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                ptPlayer->bSetDungeonActive = TRUE;
            }

            UISetDungeon::Started();
        }
        break;

    case SMSG_SET_DUNGEON_RESULTS_MESSAGE:
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                ptPlayer->bSetDungeonActive = FALSE;
            }

            const SetDungeonResultsMessage * ptResultsMsg = (const SetDungeonResultsMessage *)pMessage;
            UISetDungeon::ShowResults(ptResultsMsg);
        }
        break;

    case SMSG_BOSS_ENCOUNTER_ONE_DECLINED:
        {
            const IntDataMessage * ptMessage = (const IntDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UIBossEncounter::OneDeclined(ptPlayer, ptMessage->nData);
        }
        break;

    case SMSG_SET_DUNGEON_ONE_DECLINED:
        {
            const IntDataMessage * ptMessage = (const IntDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UISetDungeon::OneDeclined(ptPlayer, ptMessage->nData);
        }
        break;

    case SMSG_CHALLENGE_START:
        {
            SNODataMessage * ptMessage = (SNODataMessage *)pMessage;
            UIChallenge::Show(ptMessage->snoData, FALSE);
        }
        break;

    case SMSG_CHALLENGE_STARTED:
        {
            ActorCommonData * ptACD = ACDTryToGet(LocalPlayerACDGet());
            if (ptACD != NULL)
            {
                ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_BOSS_ENCOUNTER));
            }
        }
        break;

    case SMSG_ACTOR_LOOK_OVERRIDE_CHANGED:
        {
            ActorLookOverrideChangedMessage * ptActorLookOverrideChangedMessage = (ActorLookOverrideChangedMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptActorLookOverrideChangedMessage->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            ActorCommonData * ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet());
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);
            RActor * ptRActor = RActorTryToGet(idRActor);

            if (ptACD != NULL && ptRActor != NULL)
            {
                if (ptActorLookOverrideChangedMessage->nChange > 0)
                {
                    RActorAddLookOverride(ptRActor, ptActorLookOverrideChangedMessage->nLookOverrideHash);
                }
                else if (ptActorLookOverrideChangedMessage->nChange < 0)
                {
                    RActorRemoveLookOverride(ptRActor, ptActorLookOverrideChangedMessage->nLookOverrideHash);
                }

                int32 nLookOverrideIndex = RActorGetLookOverrideLookIndex(ptRActor);
                if (nLookOverrideIndex > 0)
                {
                    RActorChangeLookIndex(*ptRActor, nLookOverrideIndex, TRUE);

                    if (ACDIsPlayer(idACD))
                    {
                        Player * ptPlayer = PlayerGetByACD(idACD);
                        VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
                        VisualInventory tVisualInventoryBlank;
                        tVisualInventoryBlank.tEquipmentCurrent.Init();
                        tVisualInventoryBlank.InitializeLast();
                        tVisualInventoryBlank.tEquipmentLast.tVisualItem[0] = ptVisualInventory->tEquipmentCurrent.tVisualItem[0];
                        tVisualInventoryBlank.tEquipmentLast.tVisualItem[6] = ptVisualInventory->tEquipmentCurrent.tVisualItem[6];

                        VisualInventoryApply(ptRActor, tVisualInventoryBlank, 0, PlayerGetPlayerClass(idACD), !PlayerIsMale(*ptPlayer));
                    }
                }
                else
                {
                    if (AnimPlayerShouldPlayAllMessages(*ptACD))
                    {
                        RActorPlayRemainingKeyframedRMessages(ptRActor);
                    }

                    if (VisualInventoryGet(idACD) != NULL || !ptACD->IsMirrorImage())
                    {
                        RActorRevertToNativeLook(*ptRActor);

                        VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
                        if (ptVisualInventory != NULL)
                        {
                            VisualEquipment & tEquipmentLast = ptVisualInventory->tEquipmentLast;
                            for (int32 i = 0; i < 8; i++)
                            {
                                if (i != 4 && i != 5)
                                {
                                    VisualItem & tVisualItem = tEquipmentLast.tVisualItem[i];
                                    tVisualItem.Set(-2, 0, -1, -1);
                                }
                            }
                        }

                        ACDUpdateAppearance(idACD);

                        if (idACD == ptACDLocalPlayer->id && ptACD->IsAlive())
                        {
                            const CActorMovement * ptCActorMovement = ptRActor->ptCActorMovement;
                            if (ptCActorMovement != NULL && !ptCActorMovement->IsMoving())
                            {
                                AnimReason eAnimReason = AnimPlayerGetAnimReason(*ptACD);
                                if (eAnimReason != ANIM_REASON_STANCE && eAnimReason != ANIM_REASON_PREPLAY_POWER && eAnimReason != ANIM_REASON_DEATH && eAnimReason != ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH)
                                {
                                    AnimPlayerPlayIdle(*ptACD);
                                }
                            }
                        }
                    }
                }
            }
        }
        break;

    case SMSG_ENTER_WORLD:
        {
            const EnterWorldMessage * ptEnterWorldMsg = (const EnterWorldMessage *)pMessage;

            CWorld * ptCWorld = CWorldGetFromSWorldID(ptEnterWorldMsg->idSWorld);
            (void)ptCWorld;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptEnterWorldMsg->nPlayerIndex);
            if (ptPlayer != NULL && ptPlayer->idPlayerACD != ACDID_INVALID)
            {
                ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

                ActorLocation tNewLocation;
                tNewLocation.eActorLocation = 0;
                tNewLocation.tWorldLocation.flScale = tACDPlayer.GetScale();
                tNewLocation.tWorldLocation.idSWorld = ptEnterWorldMsg->idSWorld;

                tNewLocation.tWorldLocation.transform.q = tACDPlayer.GetOrientation();
                tNewLocation.tWorldLocation.transform.wp = ptEnterWorldMsg->wpLocation;

                RActorSetACDLocationFromMessage(tACDPlayer, tNewLocation, FALSE, FALSE, TRUE);

                if (CWorldGetCurrentWorld() == ptEnterWorldMsg->idSWorld)
                {
                    UIHUD::Console::PlayerChangedWorld(ptPlayer);
                    break;
                }

                CWorldChange(ptEnterWorldMsg->idSWorld);

                UIHUD::Console::PlayerChangedWorld(ptPlayer);
                for (Player * ptOtherPlayer = PlayerGetFirstAll(); ptOtherPlayer != NULL; ptOtherPlayer = PlayerGetNextAll(ptOtherPlayer))
                {
                    if (ptPlayer->nPlayerIndex != ptOtherPlayer->nPlayerIndex && PlayerIsLocal(ptOtherPlayer))
                    {
                        UIHUD::Console::PlayerChangedWorld(ptOtherPlayer);
                    }
                }

                if (ptEnterWorldMsg->tEnterKnownLookOverrides.Exists())
                {
                    EnterKnownLookOverrides tLookOverrides = ptEnterWorldMsg->tEnterKnownLookOverrides;

                    RActor * ptRActor = RActorTryToGet((RActorID)ACDGetOwner(tACDPlayer.id));
                    if (ptRActor != NULL)
                    {
                        for (int x = 0; x < 6; x++)
                        {
                            if (tLookOverrides.aLookOverrides[x] != 0)
                            {
                                RActorAddLookOverride(ptRActor, tLookOverrides.aLookOverrides[x]);
                            }
                        }

                        int32 nLookLinkIndex = RActorGetLookOverrideLookIndex(ptRActor);
                        if (nLookLinkIndex > 0)
                        {
                            RActorChangeLookIndex(*ptRActor, nLookLinkIndex, TRUE);

                            VisualInventory * ptVisualInventory = VisualInventoryGet(tACDPlayer.id);
                            VisualInventory tVisualInventoryBlank;
                            tVisualInventoryBlank.tEquipmentCurrent.Init();
                            tVisualInventoryBlank.InitializeLast();
                            tVisualInventoryBlank.tEquipmentLast.tVisualItem[0] = ptVisualInventory->tEquipmentCurrent.tVisualItem[0];
                            tVisualInventoryBlank.tEquipmentLast.tVisualItem[6] = ptVisualInventory->tEquipmentCurrent.tVisualItem[6];

                            VisualInventoryApply(ptRActor, tVisualInventoryBlank, 0, PlayerGetPlayerClass(tACDPlayer.id), !PlayerIsMale(*ptPlayer));
                        }
                    }
                }
            }
            else if (CWorldGetCurrentWorld() == SWORLDID_INVALID)
            {
                CWorldEnter(ptEnterWorldMsg->idSWorld, TRUE);
            }

            CGameSetLoadComplete(ptPlayer);
            CGameGlobalsGet().dwFlags |= 0x80;
        }
        break;

    case SMSG_REVEAL_WORLD:
        {
            RevealWorldMessage * ptRevealWorldMsg = (RevealWorldMessage *)pMessage;

            CWorld * ptCWorld = CWorldGetFromSWorldID(ptRevealWorldMsg->idSWorld);
            if (ptCWorld == NULL)
            {
                ClientWorldCreate(
                    ptRevealWorldMsg->snoWorld,
                    ptRevealWorldMsg->idSWorld,
                    (GameMode)1,
                    ptRevealWorldMsg->nOriginX,
                    ptRevealWorldMsg->nOriginY,
                    ptRevealWorldMsg->nStitchSizeInFeetX,
                    ptRevealWorldMsg->nStitchSizeInFeetY,
                    ptRevealWorldMsg->nWorldSizeInFeetX,
                    ptRevealWorldMsg->nWorldSizeInFeetY,
                    ptRevealWorldMsg->snoDungeonFinderSourceWorld);
            }
        }
        break;

    case SMSG_REVEAL_SCENE:
        {
            RevealSceneMessage * ptRevealSceneMsg = (RevealSceneMessage *)pMessage;
            ptRevealSceneMsg->tSceneSpec.tCachedValues.dwCachedValuesValid = 0;
            CSceneReveal(*ptRevealSceneMsg);
        }
        break;

    case SMSG_DESTROY_SCENE:
        {
            DestroySceneMessage * ptDestroySceneMsg = (DestroySceneMessage *)pMessage;
            CSceneDestroy(*ptDestroySceneMsg);
        }
        break;

    case SMSG_SWAP_SCENE:
        {
            SwapSceneMessage * ptSwapSceneMsg = (SwapSceneMessage *)pMessage;
            CSceneSwap(*ptSwapSceneMsg);
        }
        break;

    case SMSG_REVEAL_TEAM:
        CRevealTeamMessage(pMessage);
        break;

    case SMSG_HERO_STATE:
        {
            HeroStateMessage * ptStateMsg = (HeroStateMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptStateMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                if (!(ptPlayer->tHeroStateData.uPlayerFlags & 4) && (ptStateMsg->tStateData.uPlayerFlags & 4))
                {
                    ptPlayer->ptNewItemManager->Reset();
                    ptPlayer->ptNewEliteItemManager->Reset();
                }

                if (ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints != ptStateMsg->tStateData.tSavedData.m_ActivatedWaypoints)
                {
                    sRefreshWaypoints(ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints, ptStateMsg->tStateData.tSavedData.m_ActivatedWaypoints);
                }

                SigmaMemoryCopy(&ptPlayer->tHeroStateData, &ptStateMsg->tStateData, sizeof(ptPlayer->tHeroStateData));

                LanguageRules::SetClientGender(PlayerIsMale(*ptPlayer));

                ptPlayer->uLastPlayedTimeUpdate = dwNow;
            }
        }
        break;

    case SMSG_HERO_STATE_SKILLS:
        {
            PlayerSkillsMessage * ptStateMsg = (PlayerSkillsMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptStateMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                SigmaMemoryCopy(ptPlayer->tHeroStateData.tSavedData.snoTraits, ptStateMsg->snoTraits, sizeof(ptStateMsg->snoTraits));
                SigmaMemoryCopy(ptPlayer->tHeroStateData.tSavedData.tActiveSkills, ptStateMsg->tActiveSkills, sizeof(ptStateMsg->tActiveSkills));
                SigmaMemoryCopy(ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers, ptStateMsg->gbidLegendaryPowers, sizeof(ptStateMsg->gbidLegendaryPowers));

                SNOPreload::UpdateSkills();
            }
        }
        break;

    case SMSG_HERO_STATE_SAVEDDATA:
        {
            PlayerSavedDataMessage * ptStateMsg = (PlayerSavedDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptStateMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                if (ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints != ptStateMsg->tSavedData.m_ActivatedWaypoints)
                {
                    sRefreshWaypoints(ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints, ptStateMsg->tSavedData.m_ActivatedWaypoints);
                }

                SigmaMemoryCopy(&ptPlayer->tHeroStateData.tSavedData, &ptStateMsg->tSavedData, sizeof(ptPlayer->tHeroStateData.tSavedData));

                SNOPreload::UpdateSkills();
            }
        }
        break;

    case SMSG_HERO_STATE_PLAYERFLAGS:
        {
            PlayerDWordDataMessage * ptStateMsg = (PlayerDWordDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptStateMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                ptPlayer->tHeroStateData.uPlayerFlags = ptStateMsg->dwData;
            }
        }
        break;

    case SMSG_HERO_STATE_WAYPOINTS:
        {
            PlayerDWordDataMessage * ptStateMsg = (PlayerDWordDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptStateMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                if (ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints != ptStateMsg->dwData)
                {
                    sRefreshWaypoints(ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints, ptStateMsg->dwData);
                }

                ptPlayer->tHeroStateData.tSavedData.m_ActivatedWaypoints = ptStateMsg->dwData;
            }
        }
        break;

    case SMSG_HERO_STATE_LASTSAVEPOINT:
        {
            SavePointDataMessage * ptStateMsg = (SavePointDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptStateMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                SigmaMemoryCopy(&ptPlayer->tHeroStateData.tSavedData.tLastSavePoint, &ptStateMsg->tData, sizeof(ptStateMsg->tData));
            }
        }
        break;

    case SMSG_HERO_STATE_LASTWAYPOINTLEVELAREA:
        {
            LastWaypointLevelAreaDataMessage * ptStateMsg = (LastWaypointLevelAreaDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptStateMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                if (GameIsOpenWorld() &&
                    PlayerIsLocal(ptPlayer) &&
                    ptStateMsg->snoLevelArea != ptPlayer->snoLastWaypointLevelArea &&
                    ptPlayer->nControllerIndex == Console::GamerProfile::GetPrimaryProfileUserIndex())
                {
                    if (ptStateMsg->snoLevelArea == GlobalSNOGet(GLOBALSNO_LEVELAREA_ACT_3_HUB))
                    {
                        Act eAct = ActGetWaypointActByLevelArea(ptPlayer->snoLastWaypointLevelArea);
                        if (eAct == ACT_INVALID)
                        {
                            eAct = GameGetActiveGameCommonData().tGameSyncedData.eOpenWorldDefaultAct;
                        }

                        if (eAct == ACT_4)
                        {
                            UIWaypointMap::SetAct3HubEntryPointAct(eAct);
                        }
                        else
                        {
                            UIWaypointMap::SetAct3HubEntryPointAct(ACT_INVALID);
                        }
                    }
                    else
                    {
                        UIWaypointMap::SetAct3HubEntryPointAct(ACT_INVALID);
                    }
                }

                ptPlayer->snoLastWaypointLevelArea = ptStateMsg->snoLevelArea;
            }
        }
        break;

    case SMSG_TUTORIAL_STATE:
        {
            D3::GameMessage::TutorialState tMessage;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tMessage))
            {
                DisplayInternalError(NULL, "", 2377, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                if (tMessage.seen_tutorials().size() != 0)
                {
                    SigmaMemoryCopy(ptPlayer->tSeenTutorials.GetRawBuffer(), tMessage.seen_tutorials().c_str(), tMessage.seen_tutorials().size());
                }
            }
        }
        break;

    case SMSG_STASH_ICON_STATE:
        {
            StashIconStateMessage * ptMsg = (StashIconStateMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                SigmaMemoryCopy(ptPlayer->bStashIcons, ptMsg->bStashIcons, sizeof(ptMsg->bStashIcons));
            }
        }
        break;

    case SMSG_ACD_ENTER_KNOWN:
        {
            ACDEnterKnownMessage * ptACDEnterMsg = (ACDEnterKnownMessage *)pMessage;
            RActorACDEnterKnown(*ptACDEnterMsg);
        }
        break;

    case SMSG_ACD_EXIT_KNOWN:
        {
            ANNDataMessage * ptACDExitMsg = (ANNDataMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptACDExitMsg->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                RActorACDExitKnown(*ptACD);
            }
        }
        break;

    case SMSG_PLAYER_ENTER_KNOWN:
        {
            PlayerEnterKnownMessage * ptPlayerEnterMsg = (PlayerEnterKnownMessage *)pMessage;

            ACDID idACDPlayer = ACDFromANN(ptPlayerEnterMsg->ann);

            CPlayerEnterKnown(ptPlayerEnterMsg->nPlayerIndex, idACDPlayer, (RActorID)ACDGetOwner(idACDPlayer));
        }
        break;

    case SMSG_UI_ENTER_KNOWN:
        {
            ANNDataMessage * ptUIEnterKnownMessage = (ANNDataMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptUIEnterKnownMessage->ann);
            if (idACD != ACDID_INVALID)
            {
                UIMinimap_EnterKnown(idACD);
            }
        }
        break;

    case SMSG_ACD_WORLD_POSITION:
        {
            ACDWorldPositionMessage * ptACDWorldPositionMsg = (ACDWorldPositionMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptACDWorldPositionMsg->ann);
            if (idACD == ACDID_INVALID)
            {
                break;
            }

            ActorLocation tNewLocation;
            tNewLocation.eActorLocation = 0;
            tNewLocation.tWorldLocation.flScale = ptACDWorldPositionMsg->tWorldLocationData.flScale;
            tNewLocation.tWorldLocation.idSWorld = ptACDWorldPositionMsg->tWorldLocationData.idSWorld;
            tNewLocation.tWorldLocation.transform = ptACDWorldPositionMsg->tWorldLocationData.transform;

            ActorCommonData & tACD = ACDGet(idACD);
            RActorSetACDLocationFromMessage(tACD, tNewLocation, FALSE, FALSE, TRUE);

            if (ptACDWorldPositionMsg->tLookOverrides.Exists())
            {
                EnterKnownLookOverrides tLookOverrides = ptACDWorldPositionMsg->tLookOverrides;

                RActor * ptRActor = RActorTryToGet((RActorID)ACDGetOwner(tACD.id));
                if (ptRActor != NULL)
                {
                    ptRActor->ptLookOverrideMap->RemoveAll();

                    for (int x = 0; x < 6; x++)
                    {
                        if (tLookOverrides.aLookOverrides[x] != 0)
                        {
                            RActorAddLookOverride(ptRActor, tLookOverrides.aLookOverrides[x]);
                        }
                    }

                    int32 nLookLinkIndex = RActorGetLookOverrideLookIndex(ptRActor);
                    if (nLookLinkIndex > 0)
                    {
                        RActorChangeLookIndex(*ptRActor, nLookLinkIndex, TRUE);
                    }
                }
            }
        }
        break;

    case SMSG_SHEAR:
        {
            ACDShearMessage * ptACDShearMsg = (ACDShearMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptACDShearMsg->ann);
            if (idACD == ACDID_INVALID)
            {
                DisplayInternalError(NULL, "", 2533, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            RActorID idRActor = (RActorID)ACDGetOwner(idACD);
            if (idRActor != RACTORID_INVALID)
            {
                RActor * ptRActor = RActorGet(idRActor);
                ptRActor->flShear = ptACDShearMsg->flShear;
                RActorUpdateClientTransform(ptRActor, NULL, NULL);
            }
        }
        break;

    case SMSG_GROUP:
        {
            ACDGroupMessage * ptGroupMessage = (ACDGroupMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptGroupMessage->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                BOOL bCheckQuests = QuestIsAnyCurrentKillQuest();

                BOOL bCurrentQuestObjective = bCheckQuests ? QuestIsACDObjective(*ptACD) : FALSE;

                ptACD->uGroupPrimary = ptGroupMessage->uPrimaryGroup;
                ptACD->uGroupSecondary = ptGroupMessage->uSecondaryGroup;

                if (bCheckQuests)
                {
                    BOOL bIsNowObjective = QuestIsACDObjective(*ptACD);

                    if (bCurrentQuestObjective && !bIsNowObjective)
                    {
                        CQuestsUnFlagACD(*ptACD);
                    }
                    else if (!bCurrentQuestObjective && bIsNowObjective)
                    {
                        CQuestsFlagACD(*ptACD);
                    }
                }
            }
        }
        break;

    case SMSG_ACD_INVENTORY_POSITION:
        {
            ACDInventoryPositionMessage * ptACDInventoryPositionMsg = (ACDInventoryPositionMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptACDInventoryPositionMsg->ann);
            if (idACD == ACDID_INVALID)
            {
                break;
            }

            ActorLocation tNewLocation;
            tNewLocation.eActorLocation = ptACDInventoryPositionMsg->tLocType;
            tNewLocation.tInvLocation.eSlot = ptACDInventoryPositionMsg->tInvLocationData.eSlot;
            tNewLocation.tInvLocation.pt = ptACDInventoryPositionMsg->tInvLocationData.pt;
            tNewLocation.tInvLocation.idACDContainer = ACDFromANN(ptACDInventoryPositionMsg->tInvLocationData.annContainer);

            ActorCommonData & tACD = ACDGet(idACD);
            ActorLocation tOldLocation = tACD.GetLocation();
            BOOL fDeletedACD = RActorSetACDLocationFromMessage(tACD, tNewLocation, FALSE, FALSE, TRUE);
            if (!fDeletedACD)
            {
                ItemSetIsBeingManipulated(tACD, FALSE, 120);

                if (tOldLocation.eActorLocation == 1)
                {
                    UIInventory::ApplySwapIndicator(tACD);
                }

                CQuestsOnInventoryUpdate(tACD);
            }

            SNOPreload::UpdateEquipped();

            UIInventory::Console::OnInventoryChanged(tOldLocation, tNewLocation, FALSE, FALSE);
            UIFollower::Console::OnInventoryChanged(tOldLocation, tNewLocation, FALSE);
            UISkills::Console::OnInventoryChanged(tOldLocation, tNewLocation, FALSE);
            UIInventory::Console::OnStashChanged(tOldLocation, tNewLocation, FALSE);
            UIInventory::Console::OnShopChanged(tOldLocation, tNewLocation, FALSE, FALSE);
            UIJeweler::Console::OnJewelerChanged(tOldLocation, tNewLocation, FALSE);
            UIBlacksmith::Console::OnBlacksmithChanged(tOldLocation, tNewLocation, FALSE);
            UIMystic::Console::OnMysticChanged(tOldLocation, tNewLocation, FALSE);
            UIBloodShardVendor::Console::OnInventoryChanged(tOldLocation, tNewLocation, FALSE);
        }
        break;

    case SMSG_ACD_INVENTORY_UPDATE_ACTOR_SNO:
        {
            ACDInventoryUpdateActorSNO * ptACDInventoryUpdateActorSNO = (ACDInventoryUpdateActorSNO *)pMessage;

            ACDID idACD = ACDFromANN(ptACDInventoryUpdateActorSNO->ann);
            if (idACD != ACDID_INVALID)
            {
                ActorCommonData & tACD = ACDGet(idACD);
                tACD.snoActor = ptACDInventoryUpdateActorSNO->snoActor;
            }
        }
        break;

    case SMSG_PLAYER_ACTOR_SET:
        {
            PlayerActorSetInitialMessage * ptActorSetMsg = (PlayerActorSetInitialMessage *)pMessage;
            ACDID idACDPlayer = ACDFromANN(ptActorSetMsg->ann);
            if (idACDPlayer != ACDID_INVALID)
            {
                Player * ptPlayer = PlayerGetByPlayerIndex(ptActorSetMsg->nPlayerIndex);
                RActorLocalPlayerActorSet(idACDPlayer, *ptPlayer);

                GameCommonData & tGCD = CGameGetGameCommonData();
                if (tGCD.GameModeGet() == 1 && !GameIsStartUp())
                {
                    UIMinimapUpdateZoom(TRUE);
                }
            }
        }
        break;

    case SMSG_TUTORIAL:
        {
            D3::GameMessage::TutorialMessage tMessage;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tMessage))
            {
                DisplayInternalError(NULL, "", 2712, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            gtTutorialMgr.ShowTutorialFromMessage(tMessage);
        }
        break;

    case SMSG_VISUAL_INVENTORY:
        {
            VisualInventoryMessage * ptVisualInventoryMessage = (VisualInventoryMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptVisualInventoryMessage->ann);

            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                BOOL bCosmeticChanged = VisualInventoryDetectCosmeticChange(idACD, ptVisualInventoryMessage->tEquipment);

                VisualInventoryNewEquipment(ptACD->id, ptVisualInventoryMessage->tEquipment);

                AnimPlayerWeaponSetChanged(*ptACD);

                UIHireling::UpdateVisualEquipmentForHirelingNPC(*ptACD);

                SNOPreload::UpdateEquipped();

                Player * ptPlayer = PlayerGetByPlayerIndex(PlayerLocalIndexToPlayerIndex(ptHeader->nControllerIndex));
                UIShop::Console::Refresh(ptPlayer);

                ActorLocation tLocation;
                tLocation.eActorLocation = 1;
                tLocation.tInvLocation.idACDContainer = idACD;
                UIInventory::Console::OnInventoryChanged(tLocation, tLocation, bCosmeticChanged, TRUE);
                UIFollower::Console::OnInventoryChanged(tLocation, tLocation, TRUE);
                UISkills::Console::OnInventoryChanged(tLocation, tLocation, TRUE);
                UIInventory::Console::OnStashChanged(tLocation, tLocation, TRUE);
                UIInventory::Console::OnShopChanged(tLocation, tLocation, bCosmeticChanged, TRUE);
            }
        }
        break;

    case SMSG_ACD_CHANGE_GB_HANDLE:
        {
            ACDChangeGBHandleMessage * ptGBMsg = (ACDChangeGBHandleMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptGBMsg->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                ptACD->SetGBHandle(ptGBMsg->hGB);
            }
        }
        break;

    case SMSG_AFFIX:
        {
            AffixMessage * ptAffixMsg = (AffixMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptAffixMsg->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                BOOL fIdentified = ptAffixMsg->eAffixArray == 1;

                GBIDPooledList & listAffixGBIDs = fIdentified ? ptACD->listIdentifiedAffixGBIDs : ptACD->listUnidentifiedAffixGBIDs;
                listAffixGBIDs.RemoveAll();

                for (uint32 i = 0; i < ptAffixMsg->dwAffixArrayCount; i++)
                {
                    CACDSetAffix(*ptACD, ptAffixMsg->aAffixGBIDs[i], fIdentified);
                }
            }
        }
        break;

    case SMSG_PORTAL_SPECIFIER:
        {
            PortalSpecifierMessage * ptPortalSpecifierMsg = (PortalSpecifierMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptPortalSpecifierMsg->ann);
            if (idACD != ACDID_INVALID)
            {
                CPortalSpecifierInitialize(idACD, ptPortalSpecifierMsg->tResolvedDest);
            }
        }
        break;

    case SMSG_RARE_MONSTER_NAMES:
        {
            RareMonsterNamesMessage * ptRareNamesMsg = (RareMonsterNamesMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptRareNamesMsg->ann);
            if (idACD != ACDID_INVALID)
            {
                ActorCommonData & tACD = ACDGet(idACD);
                CACDSetRareMonsterNamesAndAffixes(tACD, ptRareNamesMsg->pgbidRareNames, ptRareNamesMsg->pgbidMonsterAffixes);
            }
        }
        break;

    case SMSG_RARE_ITEM_NAME:
        {
            RareItemNameMessage * ptRareItemNameMsg = (RareItemNameMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptRareItemNameMsg->ann);
            if (idACD != ACDID_INVALID)
            {
                ActorCommonData & tACD = ACDGet(idACD);


                tACD.tRareItemName = ptRareItemNameMsg->tRareItemName;
            }
        }
        break;

    case SMSG_INVENTORY_CREATE:
        {
            ANNDataMessage * ptInventoryCreateMessage = (ANNDataMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptInventoryCreateMessage->ann);
            if (idACD != ACDID_INVALID)
            {
                ActorCommonData & tACD = ACDGet(idACD);
                ACDInventoryInitialize(tACD, TRUE);
            }
        }
        break;

    case SMSG_ATTRIBUTE_SET_VALUE:
        {
            AttributeSetValueMessage * ptAttribSetFloatMsg = (AttributeSetValueMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptAttribSetFloatMsg->ann);
            FastAttribKey tKey = AttributeCompresserAttribKeyFromNetAttributeKeyValue(ptAttribSetFloatMsg->tKeyVal);
            if (idACD != ACDID_INVALID)
            {
                ActorCommonData & tACD = ACDGet(idACD);
                CACDAttributeSetValue(tACD, tKey, FastAttribValue(ptAttribSetFloatMsg->tKeyVal.nValue), FALSE);
                tACD.AttributesUpdateCached();
            }
        }
        break;

    case SMSG_ATTRIBUTES_SET_VALUES:
        {
            AttributesSetValuesMessage * ptAttribsSetValsMessage = (AttributesSetValuesMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptAttribsSetValsMessage->ann);
            if (idACD != ACDID_INVALID)
            {
                ActorCommonData & tACD = ACDGet(idACD);

                for (int32 i = 0; i < ptAttribsSetValsMessage->nAttributeCount; i++)
                {
                    CACDAttributeSetValue(
                        tACD,
                        AttributeCompresserAttribKeyFromNetAttributeKeyValue(ptAttribsSetValsMessage->atKeyVals[i]),
                        FastAttribValue(ptAttribsSetValsMessage->atKeyVals[i].nValue),
                        FALSE);
                }

                tACD.AttributesUpdateCached();
            }
        }
        break;

    case SMSG_PROJECTILE_STICK:
        {
            ProjectileStickMessage * ptProjectileStickMsg = (ProjectileStickMessage *)pMessage;
            RActorID idRActor = RActorFindByANN(ptProjectileStickMsg->annTarget);
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL)
            {
                RActorProjectileStick(ptRActor, ptProjectileStickMsg->snoActorAttachment, ptProjectileStickMsg->wpSourceLoc);
            }
        }
        break;

    case SMSG_COMBAT_LOG_ENTRY:
        {
            CombatLog::Message * ptLogMessage = (CombatLog::Message *)pMessage;

            CombatLog::LogEvent(
                ptLogMessage->eEventType,
                ACDFromANN(ptLogMessage->idACDSource),
                ACDFromANN(ptLogMessage->idACDTarget),
                ptLogMessage->snoData,
                ptLogMessage->flParam1,
                ptLogMessage->flParam2,
                ptLogMessage->dwParam1);
        }
        break;

    case SMSG_CHAT:
        {
            ChatMessage * ptChatMessage = (ChatMessage *)pMessage;
            if (ptChatMessage->nPlayerIndex != -1)
            {
                const Player * ptPlayer = PlayerGetByPlayerIndex(ptChatMessage->nPlayerIndex);
                if (ptPlayer != NULL)
                {
                    ClientMessageReceiveChat(*ptPlayer, ptChatMessage->eColor, ptChatMessage->usMessage);

                    if (!PlayerIsLocal(ptPlayer))
                    {
                        UIMinimap_DisplayANNName(ptPlayer->tClientInfo.ann, 3.0f);
                    }
                }
            }
            else
            {
                {
                    CRefString sMsg(ptChatMessage->usMessage);
                    if (ptChatMessage->eColor == CHAT_COLOR_ERROR)
                    {
                        Set_Text_Color(sMsg, 23);
                    }
                    else
                    {
                        Set_Text_Color(sMsg, 5);
                    }
                    UI_Print(sMsg, TRUE);
                }

                CRefString sFinalMsg;
                if (ptChatMessage->eColor != CHAT_COLOR_ERROR)
                {
                    sFinalMsg += "CHAT: ";
                }
                sFinalMsg += ptChatMessage->usMessage;
                sFinalMsg += "\n";
            }
        }
        break;

    case SMSG_VICTIM:
        {
            VictimMessage * ptMessage = (VictimMessage *)pMessage;
            sClientHandleVictimMessage(ptMessage);
        }
        break;

    case SMSG_LOOTRUN_APPEARED:
        {
            LootRunAppearedMessage * ptMessage = (LootRunAppearedMessage *)pMessage;
            sClientHandleLootRunAppearedMessage(ptMessage);
        }
        break;

    case SMSG_MONSTER_ENGAGED:
        {
            CombatEngagementMessage * ptMessage = (CombatEngagementMessage *)pMessage;
            sClientHandleEngagedMessage(ptMessage);
        }
        break;

    case SMSG_ELITE_KILLED:
        {
            CombatEngagementMessage * ptMessage = (CombatEngagementMessage *)pMessage;
            sClientHandleEliteKilledMessage(ptMessage);
        }
        break;

    case SMSG_TREASURE_GOBLIN_ESCAPED:
        {
            CombatEngagementMessage * ptMessage = (CombatEngagementMessage *)pMessage;
            sClientHandleTreasureGoblinEscapedMessage(ptMessage);
        }
        break;

    case SMSG_TOWN_PORTAL_USED:
        {
            PlayerIndexMessage * ptMessage = (PlayerIndexMessage *)pMessage;
            sClientHandleTownPortalMessage(ptMessage);
        }
        break;

    case SMSG_PORTED_TO_PLAYER:
        {
            PortedToPlayerMessage * ptMessage = (PortedToPlayerMessage *)pMessage;
            sClientHandlePortedToPlayerMessage(ptMessage);
        }
        break;

    case SMSG_PORTED_TO_WAYPOINT:
        {
            PortedToWaypointMessage * ptMessage = (PortedToWaypointMessage *)pMessage;
            sClientHandlePortedToWaypointMessage(ptMessage);
        }
        break;

    case SMSG_AVENGER_CREATION:
        {
            AvengerInfoMessage * ptAvengerCreationMessage = (AvengerInfoMessage *)pMessage;
            ACDID idACDTarget = ACDFromANN(ptAvengerCreationMessage->annTarget);
            ActorCommonData * ptACDAvengerMob = ACDTryToGet(idACDTarget);
            int32 nVictimPlayerIndex = ptAvengerCreationMessage->nPlayerIndexOfAvengerTarget;
            BOOL bFriendAvenger = ptAvengerCreationMessage->eAvengerSource == AVENGER_SOURCE_FRIEND;
            Player * ptPlayer = PlayerGetByPlayerIndex(nVictimPlayerIndex);

            if (ptACDAvengerMob != NULL)
            {
                if (ptACDAvengerMob->ptAvengerData == NULL)
                {
                    ptACDAvengerMob->ptAvengerData = new AvengerData;
                }

                ptACDAvengerMob->ptAvengerData->sName = bFriendAvenger ? Console::GamerProfile::GetGamerTag(ptPlayer->nControllerIndex) : ptPlayer->usHeroName;

                Avenger::Console::VictimsCopyMessageToArray(*ptAvengerCreationMessage, ptACDAvengerMob->ptAvengerData->arVictims);
                ptACDAvengerMob->ptAvengerData->eAvengerSource = ptAvengerCreationMessage->eAvengerSource;
                ptACDAvengerMob->ptAvengerData->nAffixBucket = ptAvengerCreationMessage->nAffixBucket;
                ptACDAvengerMob->eMonsterRarity = 4;

                uint32 nTotalVictims = ptACDAvengerMob->ptAvengerData->arVictims.size();

                PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(ptPlayer, ptAvengerCreationMessage->eAvengerSource);

                if (ptAvengerCreationMessage->bRetired)
                {
                    ptPlayerAvengerData->ClearCurrentAvenger();
                }
                else
                {
                    Console::Online::Friend tFriend;
                    if (bFriendAvenger)
                    {
                        if (PlayerIsLocal(ptPlayer) && Console::Online::PresenceGetRandomFriend(ptPlayer->nControllerIndex, tFriend, 0))
                        {
                            if (nTotalVictims > 1)
                            {
                                ptPlayerAvengerData->ClearCurrentAvenger();
                            }

                            D3::Console::MailItem tMailItem;
                            D3::Console::AvengerMail * ptAvengerDetails = tMailItem.mutable_avenger_mail();

                            ptAvengerDetails->set_sent_from(0);
                            ptAvengerDetails->set_affix_bucket(ptAvengerCreationMessage->nAffixBucket);
                            Avenger::Console::VictimsCopyArrayToProtobuf(ptACDAvengerMob->ptAvengerData->arVictims, ptAvengerDetails->mutable_victims());

                            if (sgtAppGlobals.dwDebugFlags2 & 0x10000)
                            {
                                Console::Online::MailSend(ptPlayer->nControllerIndex, Console::Online::MAIL_TYPE_EVENT, tMailItem, Console::Online::LobbyService::Get(ptPlayer->nControllerIndex)->GetOnlineUserID(), Console::Online::SendMailCallback());
                            }
                            else
                            {
                                Console::Online::MailSend(ptPlayer->nControllerIndex, Console::Online::MAIL_TYPE_EVENT, tMailItem, tFriend.m_idBDUser, Console::Online::SendMailCallback());
                            }
                        }
                    }
                    else
                    {
                        ptPlayerAvengerData->bHasAvenger = TRUE;
                        ptPlayerAvengerData->sAvengerName = ptACDAvengerMob->ptAvengerData->sName;
                        ptPlayerAvengerData->eAvengerSource = ptAvengerCreationMessage->eAvengerSource;
                        ptPlayerAvengerData->nAvengerAffixBucket = ptAvengerCreationMessage->nAffixBucket;
                        Avenger::Console::VictimsCopyMessageToArray(*ptAvengerCreationMessage, ptPlayerAvengerData->arVictims);
                    }
                }
            }
        }
        break;

    case SMSG_AVENGER_ENTER_KNOWN_LIST:
        {
            AvengerInfoMessage * ptAvengerSpawnMessage = (AvengerInfoMessage *)pMessage;
            ACDID idACDTarget = ACDFromANN(ptAvengerSpawnMessage->annTarget);
            ActorCommonData * ptACDAvengerMob = ACDTryToGet(idACDTarget);
            if (ptACDAvengerMob != NULL)
            {
                if (ptACDAvengerMob->ptAvengerData == NULL)
                {
                    ptACDAvengerMob->ptAvengerData = new AvengerData;
                }

                ptACDAvengerMob->ptAvengerData->sName = ptAvengerSpawnMessage->szName;
                Avenger::Console::VictimsCopyMessageToArray(*ptAvengerSpawnMessage, ptACDAvengerMob->ptAvengerData->arVictims);
                ptACDAvengerMob->ptAvengerData->eAvengerSource = ptAvengerSpawnMessage->eAvengerSource;
                ptACDAvengerMob->ptAvengerData->nAffixBucket = ptAvengerSpawnMessage->nAffixBucket;
                ptACDAvengerMob->eMonsterRarity = 4;
            }
        }
        break;

    case SMSG_AVENGER_SPAWN: // no code in the original; label inferred (see the note above the function)
        break;

    case SMSG_AVENGER_KILLED:
        {
            AvengerInfoMessage * ptAvengerKilledMessage = (AvengerInfoMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptAvengerKilledMessage->nPlayerIndexOfAvengerTarget);
            PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(ptPlayer, ptAvengerKilledMessage->eAvengerSource);
            if (ptPlayer != NULL)
            {
                ptPlayerAvengerData->ClearCurrentAvenger();

                if (ptAvengerKilledMessage->eAvengerSource == 3 && PlayerIsLocal(ptPlayer->nControllerIndex))
                {
                    Avenger::Console::TryToGetFriendAvenger(TRUE);
                }
            }
        }
        break;

    case SMSG_DUEL_RESULT:
        {
            DuelResultMessage * ptMessage = (DuelResultMessage *)pMessage;
            Player * ptPlayerVictim = PlayerGetByPlayerIndex(ptMessage->nPlayerVictimIndex);
            Player * ptPlayerKiller = PlayerGetByPlayerIndex(ptMessage->nPlayerKillerIndex);

            if (ptPlayerVictim != NULL && ptPlayerKiller != NULL)
            {
                if (PlayerIsLocal(ptPlayerKiller))
                {
                    ImageTextureFrame tImage;

                    CRefString sDisplayString = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS), "KilledPlayer"), CPlayerGetServiceName(*ptPlayerVictim, FALSE).str(), NULL);
                    UIEventQueue::Console::DisplayLongMessageForPlayer(sDisplayString, ptPlayerKiller, tImage, FALSE);
                }
            }
        }
        break;

    case SMSG_KILLCOUNT:
        {
            KillCountMessage * ptMessage = (KillCountMessage *)pMessage;
            sClientHandleKillCountMessage(ptMessage);
        }
        break;

    case SMSG_PLAYER_CLEAR_CLIENT_WALK_POWER:
        {
            PlayerClearClientWalkPowerMessage * ptMessage = (PlayerClearClientWalkPowerMessage *)pMessage;

            ACDID idACDPlayer = ACDFromANN(ptMessage->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACDPlayer);
            if (idRActor != RACTORID_INVALID && PlayerIsLocal(idRActor))
            {
                CPlayerInputClearClientWalkPower(PlayerGetByACD(idACDPlayer));
            }
        }
        break;

    case SMSG_SHRINE_TEXT_DISPLAY:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            ACDID idACDShrine = ACDFromANN(ptMessage->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACDShrine);
            if (ptACD != NULL)
            {
                ScopedActorDefinitionLockR pdefActor(ptACD->GetActorSNO());
                if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_GIZMO_POWERUP_ACTIVATION_MSG_LABEL))
                {
                    StringLabelHandle hLabel = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_POWERUP_ACTIVATION_MSG_LABEL);
                    CRefString sText;
                    BOOL bFound = StringListFetchByLabel(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), hLabel, sText);
                    if (bFound)
                    {
                        CRefString sUIControlName(CRefString().Format("powerup %d on %d", idACDShrine, ptACD->id));
                        const uint32 SHRINE_TEXT_DURATION = 300;

                        UIGame::AddTagToACD(idACDShrine, sUIControlName, sText, GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF), 16, crgbaWhite, crgbaBlack, SHRINE_TEXT_DURATION, 0.0f);
                    }
                }
            }
        }
        break;

    case SMSG_ACTIVATE_POOL_OF_REFLECTION:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            ACDID idACDShrine = ACDFromANN(ptMessage->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACDShrine);
            if (ptACD != NULL)
            {
                CRefString sText;
                if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "PoolOfReflectionBuff", sText))
                {
                    CRefString sUIControlName(CRefString().Format("pool of reflection %d", idACDShrine));
                    const uint32 SHRINE_TEXT_DURATION = 300;

                    UIGame::AddTagToACD(idACDShrine, sUIControlName, sText, GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF), 16, crgbaWhite, crgbaBlack, SHRINE_TEXT_DURATION, 0.0f);
                }
            }
        }
        break;

    case SMSG_SHRINE_GLOBE_TEXT_DISPLAY:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            ACDID idACDShrineGlobe = ACDFromANN(ptMessage->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACDShrineGlobe);
            (void)ptACD;
        }
        break;

    case SMSG_PLAY_ANIMATION:
        {
            PlayAnimationMessage * ptAnimMsg = (PlayAnimationMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptAnimMsg->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL && ptACD->ptAnimPlayer != NULL)
            {
                if (AnimPlayerShouldPlayAllMessages(*ptACD))
                {
                    RActorID idRActor = (RActorID)ACDGetOwner(ptACD->id);
                    if (idRActor != RACTORID_INVALID)
                    {
                        RActorPlayRemainingKeyframedRMessages(RActorGet(idRActor));
                    }
                }

                AnimPlayerPlayAnimationFromMessage(*ptACD, *ptAnimMsg);

                if (PlayerIsLocal(idACD) && ptAnimMsg->eAnimReason == ANIM_REASON_POWER)
                {
                    CPlayerInputPlayedNonPreplayedAnimPower(PlayerGetByACD(idACD));
                }
            }
        }
        break;

    case SMSG_PLAY_IDLE_ANIMATION:
        {
            ANNDataMessage * ptAnimMsg = (ANNDataMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptAnimMsg->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL && ptACD->ptAnimPlayer != NULL)
            {
                if (AnimPlayerShouldPlayAllMessages(*ptACD))
                {
                    RActorID idRActor = (RActorID)ACDGetOwner(ptACD->id);
                    if (idRActor != RACTORID_INVALID)
                    {
                        RActorPlayRemainingKeyframedRMessages(RActorGet(idRActor));
                    }
                }

                if (!PlayerIsLocal(idACD))
                {
                    AnimPlayerPlayIdle(*ptACD);
                }
                else
                {
                    AnimReason eAnimReason = AnimPlayerGetAnimReason(*ptACD);
                    if (eAnimReason != ANIM_REASON_PREPLAY_POWER && eAnimReason != ANIM_REASON_PREPLAY_CHANNELLED)
                    {
                        AnimPlayerPlayIdle(*ptACD);
                        CPlayerInputResetLoopingPower(PlayerGetByACD(idACD));
                    }
                }
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_NORMAL:
        {
            ACDTranslateNormalMessage * ptTranslateMsg = (ACDTranslateNormalMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            if (!PlayerIsLocal(idRActor) || ptTranslateMsg->eMoveType != 7)
            {
                RActor * ptRActor = RActorTryToGet(idRActor);
                if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
                {
                    ptRActor->ptCActorMovement->ProcessTranslateMessage(*ptTranslateMsg);
                }
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_SNAPPED:
        {
            ACDTranslateSnappedMessage * ptTranslateMsg = (ACDTranslateSnappedMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);
            if (idRActor != RACTORID_INVALID)
            {
                RActor * ptRActor = RActorTryToGet(idRActor);
                if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
                {
                    ptRActor->ptCActorMovement->ProcessSnappedMessage(*ptTranslateMsg);
                    AttachmentListHandleParentWarp(ptRActor->tAttachmentList);
                }

                if (PlayerIsLocal(idRActor))
                {
                    ActorCommonData * ptACDPlayer = ACDTryToGet(idACD);
                    if (ptACDPlayer != NULL)
                    {
                        Player * ptPlayer = PlayerGetByACD(idACD);
                        if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_LOADING))
                        {
                            CGameSetLoadComplete(ptPlayer);
                        }
                        UI_ResetForPlayerMovement(ptPlayer);
                    }
                }
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_FACING:
        {
            ACDTranslateFacingMessage * ptTranslateMsg = (ACDTranslateFacingMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessFacingMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_FIXED:
        {
            ACDTranslateFixedMessage * ptTranslateMsg = (ACDTranslateFixedMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessTranslateFixedMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_FIXED_UPDATE:
        {
            ACDTranslateFixedUpdateMessage * ptTranslateMsg = (ACDTranslateFixedUpdateMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessFixedUpdateMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_ARC:
        {
            ACDTranslateArcMessage * ptTranslateMsg = (ACDTranslateArcMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessTranslateArcMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_DETPATH:
        {
            ACDTranslateDetPathMessage * ptTranslateMsg = (ACDTranslateDetPathMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessTranslateDetPathMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_DETPATH_SIN:
        {
            ACDTranslateDetPathSinMessage * ptTranslateMsg = (ACDTranslateDetPathSinMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessTranslateDetPathSinMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_DETPATH_SPIRAL:
        {
            ACDTranslateDetPathSpiralMessage * ptTranslateMsg = (ACDTranslateDetPathSpiralMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessTranslateDetPathSpiralMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_DETPATH_PIE_WEDGE:
        {
            ACDTranslateDetPathPieWedgeMessage * ptTranslateMsg = (ACDTranslateDetPathPieWedgeMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessTranslateDetPathPieWedgeMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_STOPPED:
        {
            ACDTranslateStoppedMessage * ptTranslateMsg = (ACDTranslateStoppedMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                ptRActor->ptCActorMovement->ProcessTranslateStoppedMessage(*ptTranslateMsg);
            }
        }
        break;

    case SMSG_ACD_TRANSLATE_SYNC:
        {
            ACDTranslateSyncMessage * ptTranslateMsg = (ACDTranslateSyncMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            RActorID idRActor = (RActorID)ACDGetOwner(idACD);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL && ptRActor->ptCActorMovement != NULL)
            {
                BOOL bSnap = FALSE;
                if (ptTranslateMsg->bSnap.Exists())
                {
                    bSnap = ptTranslateMsg->bSnap;
                }
                ptRActor->ptCActorMovement->SyncToPosition(ptTranslateMsg->vPos, bSnap);
                if (ptTranslateMsg->uAckValue.Exists())
                {
                    ptRActor->ptCActorMovement->SendAckResponse(ptTranslateMsg->uAckValue);
                }
            }
        }
        break;

    case SMSG_ACD_COLL_FLAGS:
        {
            ACDCollFlagsMessage * ptCollMsg = (ACDCollFlagsMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptCollMsg->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                ptACD->tCollSettings.uDynamicMyFlags = ptCollMsg->uCollFlags;
            }
        }
        break;

    case SMSG_CURRENCY_MODIFIED:
        {
            CurrencyModifiedMessage * ptModifiedMsg = (CurrencyModifiedMessage *)pMessage;
            if (ptModifiedMsg->bPlaySound)
            {
                SNO snoSound;
                if (ptModifiedMsg->eReason == GMR_SOLD)
                {
                    snoSound = GlobalSNOGet(GLOBALSNO_SOUND_ITEM_SOLD);
                }
                else if (ptModifiedMsg->eCurrencyType == CURRENCY_BLOODSHARDS)
                {
                    snoSound = GlobalSNOGet(GLOBALSNO_SOUND_BLOODSHARDS_PICKUP);
                }
                else if (ptModifiedMsg->eCurrencyType == CURRENCY_PLATINUM)
                {
                    snoSound = GlobalSNOGet(GLOBALSNO_SOUND_PLATINUM_PICKUP);
                }
                else
                {
                    snoSound = GlobalSNOGet(CWorldIsRetro() ? GLOBALSNO_SOUND_GOLD_PICKUP_RETRO : GLOBALSNO_SOUND_GOLD_PICKUP);
                }
                SoundManager::PlaySound(snoSound, cgtPSODefaults);
            }

            UIInventory::Console::OnCraftingMaterialsChanged();

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                if (ptModifiedMsg->eReason == GMR_PICKUP || ptModifiedMsg->eReason == GMR_DAILY_FIRST)
                {
                    UIInventory::NotifyOfCurrencyPickup(*ptPlayer, ptModifiedMsg->eCurrencyType);
                }

                if (ptModifiedMsg->eCurrencyType >= CURRENCY_CRAFTING_MAT_START)
                {
                    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptACDPlayer != NULL && !(ptACDPlayer->AttributesGetInt(ATTRIBUTE_CURRENCIES_DISCOVERED) & (1 << ptModifiedMsg->eCurrencyType)))
                    {
                        UIInventory::NotifyOfNewCraftingMatCurrency();
                    }
                }
            }
        }
        break;

    case SMSG_PLAY_EFFECT:
        {
            PlayEffectMessage * ptEffectMsg = (PlayEffectMessage *)pMessage;
            EffectType eEffectType = ptEffectMsg->eEffectType;
            RActorID idRActor = RActorFindByANN(ptEffectMsg->ann);
            int32 nEffectParameter = ptEffectMsg->nEffectParameter.Exists() ? ptEffectMsg->nEffectParameter.Get() : 0;
            ACDNetworkName annPlayer = ptEffectMsg->annPlayer.Exists() ? ptEffectMsg->annPlayer.Get() : ANN_INVALID;
            BOOL bRemoteEffect = FALSE;
            if (annPlayer != ANN_INVALID)
            {
                RActorID idRActorPlayer = RActorFindByANN(annPlayer);
                bRemoteEffect = !PlayerIsLocal(idRActorPlayer);
            }

            if (idRActor != RACTORID_INVALID)
            {
                RActorPlayEffectFromMessage(idRActor, eEffectType, nEffectParameter, bRemoteEffect);
            }
        }
        break;

    case SMSG_PLAY_HIT_EFFECT:
        {
            PlayHitEffectMessage * ptEffectMsg = (PlayHitEffectMessage *)pMessage;
            RActorID idRActor = RActorFindByANN(ptEffectMsg->ann);
            if (idRActor != RACTORID_INVALID)
            {
                RActorPlayHitEffectFromMessage(idRActor, ACDFromANN(ptEffectMsg->annAttacker), ptEffectMsg->eDamageType, ptEffectMsg->fCrit);
            }
        }
        break;

    case SMSG_PLAY_HIT_EFFECT_OVERRIDE:
        {
            PlayHitEffectOverrideMessage * ptEffectMsg = (PlayHitEffectOverrideMessage *)pMessage;
            RActorID idRActor = RActorFindByANN(ptEffectMsg->ann);

            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL)
            {
                RActorPlayHitEffectOverrideFromMessage(ptRActor, ACDFromANN(ptEffectMsg->annAttacker), ptEffectMsg->snoEffectGroup);
            }
        }
        break;

    case SMSG_PLAY_NONPOSITIONAL_SOUND:
        {
            PlayNonPositionalSoundMessage * ptSoundMsg = (PlayNonPositionalSoundMessage *)pMessage;
            SoundManager::PlaySound(ptSoundMsg->snoSound, cgtPSODefaults);
        }
        break;

    case SMSG_PLAY_ERROR_SOUND:
        {
            PlayErrorSoundMessage * ptSoundMsg = (PlayErrorSoundMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            CPlayerSayError(ptSoundMsg->dwSoundTag, ptPlayer->idPlayerRActor, FALSE);
        }
        break;

    case SMSG_PLAY_MUSIC:
        {
            PlayMusicMessage * ptMusicMsg = (PlayMusicMessage *)pMessage;

            if (ptMusicMsg->snoMusic != SNO_NONE)
            {
                ScopedMusicDefinitionLockR pdefMusic(ptMusicMsg->snoMusic);
                if (pdefMusic->dwFlags & 1)
                {
                    break;
                }
            }

            SoundManager::PlayMusic(ptMusicMsg->snoMusic, 0, FALSE);
        }
        break;

    case SMSG_STOP_MUSIC_OVERLAY:
        SoundManager::StopMusicOverlay();
        break;

    case SMSG_PLAY_CUTSCENE:
        {
            if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
            {
                PlayCutsceneMessage * ptCutsceneMsg = (PlayCutsceneMessage *)pMessage;
                Player * ptPlayer = LocalPlayerGet();
                if (ptPlayer != NULL)
                {
                    if (!Prefs::GetGameplayPrefs().GetDisableCutscenes())
                    {
                        if (Console::Screens::IsScreenSetOpen())
                        {
                            Console::Screens::CloseScreenSet(TRUE, FALSE);
                        }

                        PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
                        BOOL fIsMale = PlayerIsMale(*ptPlayer);
                        CutscenePlayInterstitialCutscene(GameGetCurrentAct(), ptCutsceneMsg->nIndex, ePlayerClass, fIsMale, ptHeader->nControllerIndex, NULL);
                    }
                }
            }
        }
        break;

    case SMSG_PLAY_ANNIVERSARY_CUTSCENE:
        {
            Player * ptPlayer = LocalPlayerGet();
            if (ptPlayer != NULL)
            {
                if (!Prefs::GetGameplayPrefs().GetDisableCutscenesRaw())
                {
                    if (Console::Screens::IsScreenSetOpen())
                    {
                        Console::Screens::CloseScreenSet(TRUE, FALSE);
                    }

                    BOOL fIsMale = PlayerIsMale(*ptPlayer);
                    CutscenePlayAnniversaryCinematic(fIsMale, NULL);
                }
            }
        }
        break;

    case SMSG_PLAY_FINAL_CUTSCENE:
        CutsceneUnlockFinalCinematic(FALSE);
        break;

    case SMSG_SCOMPLEX_EFFECT_ADD:
        {
            ComplexEffectAddMessage * ptComplexEffectAddMessage = (ComplexEffectAddMessage *)pMessage;

            CComplexEffectAdd(
                ptComplexEffectAddMessage->idSComplexEffect,
                ptComplexEffectAddMessage->snoEffectGroup,
                ACDFromANN(ptComplexEffectAddMessage->annSource),
                ACDFromANN(ptComplexEffectAddMessage->annDest),
                ptComplexEffectAddMessage->eType,
                ptComplexEffectAddMessage->dwParam1,
                ptComplexEffectAddMessage->dwParam2,
                ptComplexEffectAddMessage->bIgnoreOwnerAlpha);
        }
        break;

    case SMSG_FLIPPY:
        {
            SWorldID idSWorld = CGameGetCurrentSWorldID();
            if (idSWorld != -1)
            {
                FlippyMessage * ptFlippyMsg = (FlippyMessage *)pMessage;

                RActorID idRActor = RActorFindByANN(ptFlippyMsg->annLauncher);
                RActorFlippy(idRActor, ptFlippyMsg->wpDest, ptFlippyMsg->snoFlippyParticle, ptFlippyMsg->snoActorLoot);
            }
        }
        break;

    case SMSG_NPC_INTERACT:
        {
            NPCInteractOptionsMessage * ptNPCInteractMsg = (NPCInteractOptionsMessage *)pMessage;

            ACDID idACDTalkingTo = ACDFromANN(ptNPCInteractMsg->annTalkingTo);
            ActorCommonData * ptACDTalkingTo = ACDTryToGet(idACDTalkingTo);
            if (ptACDTalkingTo != NULL)
            {
                UINPC::Console::InteractMenuEnable(
                    ptHeader->nControllerIndex,
                    *ptACDTalkingTo,
                    ptNPCInteractMsg->tNPCInteraction,
                    ptNPCInteractMsg->nNumInteractions,
                    ptNPCInteractMsg->ePreferredMenu);
            }
        }
        break;

    case SMSG_OPEN_COSMETICS: // no code in the original; label inferred
        break;

    case SMSG_FORCE_NPC_INTERACT_CLOSE:
        {
            Console::Screens::CloseScreenSet(TRUE, FALSE);
            UINPC::Console::InteractMenuDisable(ptHeader->nControllerIndex);
        }
        break;

    case SMSG_ATTACH_PET:
        {
            PetMessage * ptMsg = (PetMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptMsg->nOwner);
            if (ptPlayer != NULL)
            {
                CPlayerAddPetAt(*ptPlayer, ptMsg->nIndex, ACDFromANN(ptMsg->annPet), (PetType)ptMsg->nType);
            }

            SNOPreload::UpdateHireling();
            SNOPreload::UpdateSkills();
            SNOPreload::UpdateEquipped();
        }
        break;

    case SMSG_DETACH_PET:
        {
            PetDetachMessage * ptMsg = (PetDetachMessage *)pMessage;

            ACDID idACDPet = ACDFromANN(ptMsg->annPet);
            Player * ptPlayer = PlayerGetByACD(ACDGetPetOwner(idACDPet));
            if (ptPlayer == NULL)
            {
                ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            }

            if (PlayerIsLocal(ptPlayer))
            {
                PlayerRemovePet(*ptPlayer, idACDPet, ptMsg->bDisplayChatMessage);
            }

            SNOPreload::UpdateHireling();
            SNOPreload::UpdateSkills();
            SNOPreload::UpdateEquipped();
        }
        break;

    case SMSG_SET_HIRELING_DEAD:
        {
            HirelingInfoUpdateMessage * ptMsg = (HirelingInfoUpdateMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptMsg->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                HirelingData & tData = ptPlayer->tHirelingStable.tHireling[ptMsg->eClass];
                tData.tInfo.eClass = ptMsg->eClass;
                tData.tInfo.gbidName = ptMsg->gbidName;
                tData.tInfo.fDead = ptMsg->fDead;
                tData.tInfo.eClass = ptMsg->eValidClass;
                SigmaMemoryCopy(tData.tInfo.arnPowerKeyParam, ptMsg->arnPowerKeyParam, sizeof(tData.tInfo.arnPowerKeyParam));
                for (uint32 i = 0; i < NUM_HIRELING_SLOTS; i++)
                {
                    tData.tItems.idACDItems[i] = ACDFromANN(ptMsg->annItems[i]);
                }

                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer != NULL)
                {
                    int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);

                    ACDIterator tIt;
                    while (ActorCommonDataIterate(tIt))
                    {
                        const ActorCommonData * ptACD = tIt.ptElement;

                        HirelingClass eClass = ACDGetHirelingClass(*ptACD);
                        if (eClass == ptMsg->eClass && !ACDIsHireling(*ptACD))
                        {
                            RActor * ptRActor = RActorTryToGet((RActorID)ptACD->idOwner);
                            if (ptRActor != NULL)
                            {
                                CHirelingSetAppearance(ptRActor, eClass, nLevel);
                            }
                        }
                    }
                }
            }
        }
        break;

    case SMSG_QUEST_UPDATE:
        {
            QuestUpdateMessage * ptMsg = (QuestUpdateMessage *)pMessage;
            CQuestsUpdateLog(*ptMsg);
        }
        break;

    case SMSG_QUEST_METER:
        {
            QuestMeterMessage * ptMsg = (QuestMeterMessage *)pMessage;
            CQuestsUpdateMeter(*ptMsg);
        }
        break;

    case SMSG_QUEST_FLAGS:
        {
            QuestFlagsMessage * ptMsg = (QuestFlagsMessage *)pMessage;

            QuestInstance * ptQuestInstance = QuestsGetQuestInstance(ptMsg->snoQuest, ACDID_INVALID);
            if (ptQuestInstance != NULL)
            {
                ptQuestInstance->dwFlags = ptMsg->dwFlags;
            }
        }
        break;

    case SMSG_QUEST_COUNTER:
        {
            QuestCounterMessage * ptMsg = (QuestCounterMessage *)pMessage;
            CQuestsUpdateCounters(ptMsg->snoQuest, ptMsg->snoLevelArea, ptMsg->nStepUID, ptMsg->nObjectiveIndex, ptMsg->nValue, ptMsg->bCompleted);
        }
        break;

    case SMSG_QUEST_REWARD:
        {
            D3::Quests::QuestStepComplete tQuestStepComplete;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tQuestStepComplete))
            {
                DisplayInternalError(NULL, "", 4232, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            if (PlayerGetLocalPlayerCount() > 1)
            {
                UIQuestComplete::OnMultiplayerQuestReward(ptHeader->nControllerIndex, tQuestStepComplete.reward(), tQuestStepComplete.is_quest_complete(), tQuestStepComplete.was_reward_autoequipped());
            }
            else
            {
                UIQuestComplete::OnQuestReward(tQuestStepComplete.reward(), tQuestStepComplete.is_quest_complete(), tQuestStepComplete.was_reward_autoequipped(), ptHeader->nControllerIndex);
            }
        }
        break;

    case SMSG_TIERED_RIFT_REWARD:
        {
            D3::Quests::QuestTieredRiftComplete tTieredRiftStepComplete;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tTieredRiftStepComplete))
            {
                DisplayInternalError(NULL, "", 4264, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            UIRewards::ShowTieredRiftReward(tTieredRiftStepComplete.reward(), tTieredRiftStepComplete.is_finished_in_time(), tTieredRiftStepComplete.rift_depth(), -1);
        }
        break;

    case SMSG_DYNAMIC_QUEST_OBJECTIVE_UPDATE:
        {
            DynamicQuestObjectiveMessage * ptMsg = (DynamicQuestObjectiveMessage *)pMessage;
            GameCommonData & tGCD = GameGetActiveGameCommonData();

            int32 nMatchingIndex = -1;
            int32 nFreeIndex = -1;
            for (int32 i = 0; i < 10; i++)
            {
                QuestStepObjective & tObjective = tGCD.ptQuestGlobals->tDynamicQuestObjectives[i];
                if (nMatchingIndex == -1 && tObjective.nUID == ptMsg->nUID)
                {
                    nMatchingIndex = i;
                }
                if (nFreeIndex == -1 && tObjective.eObjectiveType == -1)
                {
                    nFreeIndex = i;
                }
            }

            if (nMatchingIndex != -1)
            {
                tGCD.ptQuestGlobals->arDynamicQuestObjectiveState[nMatchingIndex] = ptMsg->nState;
                tGCD.ptQuestGlobals->arDynamicQuestObjectiveCompleted[nMatchingIndex] = ptMsg->bCompleted;
                tGCD.ptQuestGlobals->arDynamicQuestObjectiveHidden[nMatchingIndex] = ptMsg->bHidden;

                const int32 DYNAMIC_QUEST_ASSIGNED_STEP = 2;
                UIQuests::QueueUpdate(GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC), SNO_NONE, DYNAMIC_QUEST_ASSIGNED_STEP, FALSE, FALSE, DYNAMIC_QUEST_ASSIGNED_STEP, nMatchingIndex, ptMsg->nState, ptMsg->bCompleted);
            }
            else if (nFreeIndex != -1)
            {
                QuestStepObjective & tObjective = tGCD.ptQuestGlobals->tDynamicQuestObjectives[nFreeIndex];
                tObjective.nUID = ptMsg->nUID;
                tObjective.eObjectiveType = ptMsg->eObjectiveType;
                tObjective.nParam1 = ptMsg->nParam1;
                tObjective.nParam2 = ptMsg->nParam2;
                tObjective.snoname1 = ptMsg->snoname1;
                tObjective.snoname2 = ptMsg->snoname2;
                tObjective.gbidParam1 = ptMsg->gbidParam1;
                tObjective.gbidParam2 = ptMsg->gbidParam2;
                tObjective.bHiddenUntilComplete = ptMsg->bHiddenUntilComplete;
                tObjective.dwFlags = ptMsg->dwFlags;
                tObjective.gbidItemToShow = ptMsg->gbidItemToShow;

                tGCD.ptQuestGlobals->arDynamicQuestObjectiveState[nFreeIndex] = ptMsg->nState;
                tGCD.ptQuestGlobals->arDynamicQuestObjectiveCompleted[nFreeIndex] = ptMsg->bCompleted;
                tGCD.ptQuestGlobals->arDynamicQuestObjectiveHidden[nFreeIndex] = ptMsg->bHidden;

                const int32 DYNAMIC_QUEST_ASSIGNED_STEP = 2;
                UIQuests::QueueUpdate(GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC), SNO_NONE, DYNAMIC_QUEST_ASSIGNED_STEP, FALSE, FALSE, DYNAMIC_QUEST_ASSIGNED_STEP, nFreeIndex, ptMsg->nState, ptMsg->bCompleted);
            }
        }
        break;

    case SMSG_DYNAMIC_QUEST_RESET:
        {
            GameCommonData & tGCD = GameGetActiveGameCommonData();
            QuestGlobals * ptQuestGlobals = tGCD.ptQuestGlobals;
            SigmaMemoryClear(ptQuestGlobals->tDynamicQuestObjectives, sizeof(ptQuestGlobals->tDynamicQuestObjectives));
            for (int32 i = 0; i < 10; i++)
            {
                ptQuestGlobals->tDynamicQuestObjectives[i].eObjectiveType = -1;
            }
            for (int32 i = 0; i < 10; i++)
            {
                ptQuestGlobals->arDynamicQuestObjectiveState[i] = 0;
                ptQuestGlobals->arDynamicQuestObjectiveCompleted[i] = FALSE;
                ptQuestGlobals->arDynamicQuestObjectiveHidden[i] = FALSE;
            }

            const int32 DYNAMIC_QUEST_ASSIGNED_STEP = 2;
            UIQuests::QueueUpdate(GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC), SNO_NONE, DYNAMIC_QUEST_ASSIGNED_STEP, FALSE, FALSE, DYNAMIC_QUEST_ASSIGNED_STEP, -1, 0, FALSE);
        }
        break;

    case SMSG_PLAYER_LEVEL:
        {
            PlayerLevel * ptMsg = (PlayerLevel *)pMessage;
            CPlayerLevelMessage(ptMsg->nPlayerIndex, ptMsg->nNewLevel);
        }
        break;

    case SMSG_PLAYER_ALT_LEVEL:
        {
            PlayerLevel * ptMsg = (PlayerLevel *)pMessage;
            CPlayerAltLevelMessage(ptMsg->nPlayerIndex, ptMsg->nNewLevel);
        }
        break;

    case SMSG_PLAYER_HIGHEST_HERO_SOLO_RIFT_LEVEL:
        {
            PlayerIntValMessage * ptMsg = (PlayerIntValMessage *)pMessage;
            CPlayerHighestHeroSoloRiftLevelMessage(ptMsg->nPlayerIndex, ptMsg->nVal);
        }
        break;

    case SMSG_ACTIVATE_WAYPOINT:
        {
            WaypointActivatedMessage * ptWaypointMsg = (WaypointActivatedMessage *)pMessage;

            RActorID idRActor = RActorFindByANN(ptWaypointMsg->annWaypoint);
            GizmoActivateWaypointVisual(idRActor);

            if (ptWaypointMsg->bAnnounce)
            {
                Player * ptPlayer = PlayerGetByANN(ptWaypointMsg->annActivatingPlayer);
                if (ptPlayer != NULL)
                {
                    CRefString sMessage = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "WaypointFoundByPlayer").str(), CPlayerGetServiceName(*ptPlayer, FALSE).str(), LevelAreaGetLocalizedName(ptWaypointMsg->snoLevelArea).str(), NULL);
                    Set_Text_Color(sMessage, FONTCOLOR_SYSTEM);
                    UI_Print(sMessage, TRUE);
                }
                else
                {
                    CRefString sMessage = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "WaypointFound").str(), LevelAreaGetLocalizedName(ptWaypointMsg->snoLevelArea).str(), NULL);
                    Set_Text_Color(sMessage, FONTCOLOR_SYSTEM);
                    UI_Print(sMessage, TRUE);
                }
            }
        }
        break;

    case SMSG_OPERATE_WAYPOINT:
        {
            ANNDataMessage * ptWaypointMsg = (ANNDataMessage *)pMessage;

            RActorID idRActor = RActorFindByANN(ptWaypointMsg->ann);
            GizmoOperateWaypoint(idRActor, ptHeader->nControllerIndex);
        }
        break;

    case SMSG_CONFIRM_OPERATION:
        {
            ANNDataMessage * ptConfirmMsg = (ANNDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            GizmoConfirmOperation(ACDFromANN(ptConfirmMsg->ann), ptPlayer);
        }
        break;

    case SMSG_OPEN_BANNER_CUSTOMIZATION:
        if (!AppDrawFlagIsSet(22) && !Console::Screens::IsScreenSetOpen())
        {
            Console::Screens::OpenToScreen(Console::Screens::BANNER_SCREEN_SET, 0, ptHeader->nControllerIndex, FALSE);
        }
        break;

    case SMSG_AIM_TARGET:
        {
            AimTargetMessage * ptAimTargetMsg = (AimTargetMessage *)pMessage;
            RActorID idRActor = RActorFindByANN(ptAimTargetMsg->ann);
            if (idRActor != RACTORID_INVALID)
            {
                RActorSetAimTarget(idRActor, ACDFromANN(ptAimTargetMsg->annTarget), ptAimTargetMsg->eType, ptAimTargetMsg->tPlace);
            }
        }
        break;

    case SMSG_INSPECT:
        {
            PlayerIndexMessage * ptMessage = (PlayerIndexMessage *)pMessage;
            UIParty::Console::OnInspectAck(ptMessage->nPlayerIndex);
        }
        break;

    case SMSG_ACD_SET_IDLE_ANIM:
        {
            SetIdleAnimationMessage * ptSetIdleAnimMsg = (SetIdleAnimationMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptSetIdleAnimMsg->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                AnimPlayerIdleAnimationChanged(*ptACD, ptSetIdleAnimMsg->dwAnimTag);
                AnimPlayerUpdateGatePosition(*ptACD);
            }
        }
        break;

    case SMSG_CRAFTER_OPEN:
        {
            ANNDataMessage * ptCrafterOpenMsg = (ANNDataMessage *)pMessage;
            ACDID idACDVendor = ACDFromANN(ptCrafterOpenMsg->ann);

            ActorCommonData * ptACDVendor = ACDTryToGet(idACDVendor);
            if (ptACDVendor != NULL)
            {
                Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
                if (ptPlayer != NULL)
                {
                    ptPlayer->idACDActiveVendor = idACDVendor;
                    UIUtils::Console::OpenArtisanScreen(*ptACDVendor, ptPlayer->nControllerIndex);
                }
            }
        }
        break;

    case SMSG_SHOP_OPEN:
        {
            ANNDataMessage * ptShopOpenMsg = (ANNDataMessage *)pMessage;

            ACDID idACDVendor = ACDFromANN(ptShopOpenMsg->ann);

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            if (ptPlayer != NULL && !Console::Screens::IsScreenSetOpen())
            {
                ptPlayer->idACDActiveVendor = idACDVendor;
                ActorCommonData * ptACDVendor = ACDTryToGet(idACDVendor);
                if (ptACDVendor != NULL)
                {
                    Console::Screens::ScreenSets eScreenSet = Console::Screens::SHOP_SCREEN_SET;
                    Console::Screens::ScreenID idScreen = 0;
                    if (ptACDVendor->GetCurrencyType() == CURRENCY_BLOODSHARDS)
                    {
                        eScreenSet = Console::Screens::BLOODSHARD_VENDOR_SCREEN_SET;
                    }

                    Console::Screens::OpenToScreen(eScreenSet, idScreen, ptPlayer->nControllerIndex, FALSE);
                }
            }
        }
        break;

    case SMSG_SHARED_STASH_OPEN:
        {
            ANNDataMessage * ptStashOpenMsg = (ANNDataMessage *)pMessage;

            ACDID idACDStash = ACDFromANN(ptStashOpenMsg->ann);
            (void)idACDStash;
            if (!Console::Screens::IsScreenSetOpen())
            {
                Console::Screens::OpenToScreen(Console::Screens::STASH_SCREEN_SET, 0, ptHeader->nControllerIndex, FALSE);
            }
        }
        break;

    case SMSG_MAILBOX_OPEN:
        {
            ANNDataMessage * ptMailboxOpenMsg = (ANNDataMessage *)pMessage;

            if (!Console::Screens::IsScreenSetOpen())
            {
                ACDID idACDMailbox = ACDFromANN(ptMailboxOpenMsg->ann);
                (void)idACDMailbox;
                Console::Screens::OpenToScreen(Console::Screens::MAIL_SCREEN_SET, 0, ptHeader->nControllerIndex, FALSE);
            }
        }
        break;

    case SMSG_REDEEM_SEASON_REWARD_CONSOLE:
        {
            BoolDataMessage * ptMessage = (BoolDataMessage *)pMessage;
            UISeasonJourney::Console::GiftRedeemedResult(ptHeader->nControllerIndex, ptMessage->bVal);
        }
        break;

    case SMSG_STASH_ITEM_RETRIEVED:
        {
            BoolDataMessage * ptMessage = (BoolDataMessage *)pMessage;
            UISeasonStash::Console::ItemRetrievedResult(ptHeader->nControllerIndex, ptMessage->bVal);
        }
        break;

    case SMSG_ACD_PICKUP_FAILED:
        {
            ACDPickupFailedMessage * ptPickupMsg = (ACDPickupFailedMessage *)pMessage;

            Player * ptPlayer = PlayerGetByANN(ptPickupMsg->annPickee);
            if (ptPlayer != NULL)
            {
                CPlayerInputPickupFailed(ptPickupMsg->bIsCurrency, ptPickupMsg->ePickupResponse, ptPlayer);
            }
        }
        break;

    case SMSG_TRICKLE_UPDATE:
        {
            TrickleMessage * ptTrickleUpdateMsg = (TrickleMessage *)pMessage;
            CTrickleUpdate(ptTrickleUpdateMsg);
        }
        break;

    case SMSG_TRICKLE_REMOVE:
        {
            ANNDataMessage * ptTrickleRemoveMsg = (ANNDataMessage *)pMessage;
            CTrickleRemove(ptTrickleRemoveMsg->ann);
        }
        break;

    case SMSG_MAP_REVEAL_SCENE:
        {
            MapRevealSceneMessage * ptMapRevealSceneMessage = (MapRevealSceneMessage *)pMessage;
            UIMinimapRevealScene(
                ptMapRevealSceneMessage->idSScene,
                ptMapRevealSceneMessage->snoScene,
                ptMapRevealSceneMessage->transform,
                NULL,
                ptMapRevealSceneMessage->idSWorld,
                ptMapRevealSceneMessage->bRevealed);
        }
        break;

    case SMSG_SAVEPOINT_INFO:
        {
            SavePointInfoMessage * ptSavePointInfoMessage = (SavePointInfoMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.snoLevelAreaSavePoint = ptSavePointInfoMessage->snoLevelArea;
        }
        break;

    case SMSG_DARK_ALCHEMY_NODE_ACTIVATED:
        sub_710017a580(static_cast<DWordDataMessage *>(pMessage)->dwData);
        break;

    case SMSG_DARK_ALCHEMY_INTERACT:
        {
            const auto & message = *static_cast<GizmoInteractMessage *>(pMessage);
            ACDID source = ACDFromANN(static_cast<ACDNetworkName>(message.unk_08));
            ACDID player = ACDFromANN(static_cast<ACDNetworkName>(message.unk_0c));
            sub_710017a5b0(source, player);
        }
        break;

    case SMSG_HEARTHPORTAL_INFO:
        {
            HearthPortalInfoMessage * ptHearthPortalInfoMessage = (HearthPortalInfoMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.snoLevelAreaHearthPortal = ptHearthPortalInfoMessage->snoLevelArea;
            tCGameGlobals.unk_100 = ptHearthPortalInfoMessage->unk_c;
            tCGameGlobals.bHearthPortalIsTieredRift = ptHearthPortalInfoMessage->bIsTieredRift;
            if (ptHearthPortalInfoMessage->bResetUI)
            {
                UI_ResetForPlayerMovement(LocalPlayerGet());
            }
        }
        break;

    case SMSG_RETURNPOINT_INFO:
        {
            SavePointInfoMessage * ptSavePointInfoMessage = (SavePointInfoMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.snoLevelAreaReturnPoint = ptSavePointInfoMessage->snoLevelArea;
        }
        break;

    case SMSG_PVP_ARENA_WIN:
    case SMSG_PVP_ARENA_LOSE:
        {
            DataIDDataMessage * ptDHackWinMsg = (DataIDDataMessage *)pMessage;
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_UI_PVP_ROUND_ENDS), cgtPSODefaults);
            CRefString usEndText = CTeamGetPvPEndGameText((Team)ptDHackWinMsg->id);
            UIGame::SetSpecialAreaText(usEndText, NULL);

            CGameGet().ptCGameGlobals->bPVPPaused = TRUE;
        }
        break;

    case SMSG_PVP_ARENA_RESTART:
        {
            UIGame::SetSpecialAreaText(StringListFetch(GlobalSNOGet(721), "NewRound"), NULL);
            SoundManager::PlaySound(GlobalSNOGet(225), cgtPSODefaults);
            CGameGet().ptCGameGlobals->bPVPPaused = FALSE;
        }
        break;

    case SMSG_ACT:
        {
            ActTransitionMessage * ptActMessage = (ActTransitionMessage *)pMessage;
            Act eAct = ptActMessage->eAct;

            GameCommonData & tGCD = GameGetActiveGameCommonData();
            QuestGlobals *& ptQuestGlobals = tGCD.ptQuestGlobals;
            if (eAct != ptQuestGlobals->eCurrentAct)
            {
                QuestsActivateForAct(eAct);

                if (GameGetGameType() == 1 && !CmdLineGetParams()->tBitArray.Get(38) && !ptActMessage->bOnJoin)
                {
                    sHandleActTransition(eAct, ptHeader->nControllerIndex);
                }

                UISideScreenCloseAll(0xffffffff);
                UIQuests::OnChangeAct();
            }
        }
        break;

    case SMSG_VOTE_KICK_VOTE:
        VoteKick::Client::DoVoteKick((const VoteKickMessage *)pMessage);
        break;

    case SMSG_VOTE_KICK_FAILED:
        {
            const PlayerIndexMessage * ptMessage = (const PlayerIndexMessage *)pMessage;
            VoteKick::Client::Failed(ptMessage->nPlayerIndex);
        }
        break;

    case SMSG_VOTE_KICK_PASSED:
        {
            const PlayerIndexMessage * ptMessage = (const PlayerIndexMessage *)pMessage;
            VoteKick::Client::Passed(ptMessage->nPlayerIndex);
        }
        break;

    case SMSG_CAN_VOTE_KICK:
        {
            const PlayerIndexMessage * ptMessage = (const PlayerIndexMessage *)pMessage;
            VoteKick::Client::StartKick(ptMessage->nPlayerIndex);
        }
        break;

    case SMSG_INTERSTITIAL:
        if (!CmdLineGetParams()->tBitArray.Get(38) && GameGetGameType() == 1)
        {
            InterstitialMessage * ptInterstitialMessage = (InterstitialMessage *)pMessage;

            if (!Prefs::GetGameplayPrefs().GetDisableCutscenes())
            {
                Act eAct = GameGetCurrentAct();
                if (eAct >= ACT_1 && eAct <= ACT_5)
                {
                    int32 nIndex = 0;

                    if (Console::Screens::IsScreenSetOpen())
                    {
                        Console::Screens::CloseScreenSet(TRUE, FALSE);
                    }

                    CutscenePlayInterstitialCutscene(eAct, nIndex, ptInterstitialMessage->ePlayerClass, ptInterstitialMessage->fIsMale, ptHeader->nControllerIndex, NULL);
                }
            }
        }
        break;

    case SMSG_EFFECT_GROUP_ACD_TO_ACD:
        {
            const EffectGroupACDToACDMessage * ptEffectGroupACDToACDMessage = (const EffectGroupACDToACDMessage *)pMessage;

            ACDID idACDFrom = ACDFromANN(ptEffectGroupACDToACDMessage->annFrom);
            ACDID idACDTo = ACDFromANN(ptEffectGroupACDToACDMessage->annTo);

            RActorID idRActorPrimary = (RActorID)ACDGetOwner(idACDFrom);
            RActorID idRActorSecondary = (RActorID)ACDGetOwner(idACDTo);
            if (idRActorPrimary != RACTORID_INVALID && idRActorSecondary != RACTORID_INVALID)
            {
                EffectGroupCreateUntracked(ptEffectGroupACDToACDMessage->snoEffectGroup, idRActorPrimary, idRActorSecondary, 0, FALSE, FALSE);
            }
        }
        break;

    case SMSG_ROPE_EFFECT_ACD_TO_ACD:
        {
            RopeEffectMessageACDToACD * tRopeEffectMessageACDToACD = (RopeEffectMessageACDToACD *)pMessage;

            ACDID idACDFrom = ACDFromANN(tRopeEffectMessageACDToACD->annFrom);
            ACDID idACDTo = ACDFromANN(tRopeEffectMessageACDToACD->annTo);
            if (idACDFrom != ACDID_INVALID && idACDTo != ACDID_INVALID)
            {
                RActorACDRopeEffect(
                    tRopeEffectMessageACDToACD->snoRope,
                    idACDFrom,
                    tRopeEffectMessageACDToACD->ePositionFrom,
                    idACDTo,
                    tRopeEffectMessageACDToACD->ePositionTo,
                    tRopeEffectMessageACDToACD->bIgnoreOwnerAlpha);
            }
        }
        break;

    case SMSG_ROPE_EFFECT_ACD_TO_PLACE:
        {
            RopeEffectMessageACDToPlace * tRopeEffectMessageACDToPlace = (RopeEffectMessageACDToPlace *)pMessage;

            RActorACDRopeEffect(
                tRopeEffectMessageACDToPlace->snoRope,
                ACDFromANN(tRopeEffectMessageACDToPlace->annFrom),
                tRopeEffectMessageACDToPlace->ePositionFrom,
                tRopeEffectMessageACDToPlace->tPlaceTo.wp,
                tRopeEffectMessageACDToPlace->bIgnoreOwnerAlpha);
        }
        break;

    case SMSG_UI_ELEMENT_ACTION:
        {
            UIElementMessage * ptUIElementMessage = (UIElementMessage *)pMessage;
            UIElement eElement = ptUIElementMessage->eElement;
            UIElementAction eAction = ptUIElementMessage->eAction;
            UISideScreenSetVisible(eElement, eAction == 0);
        }
        break;

    case SMSG_VENDOR_LEVEL_UP:
        {
            CrafterLevelUpMessage * ptMessage = (CrafterLevelUpMessage *)pMessage;

            if (ptMessage->nLevel == 1)
            {
                Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
                UIGame::ShowNewCrafterNotification(ptPlayer, ptMessage->eType);
            }

            ACDIterator tIt;
            while (ActorCommonDataIterate(tIt))
            {
                ActorCommonData * ptACD = tIt.ptElement;
                if (ptACD == NULL)
                {
                    continue;
                }

                CrafterType eType = NPCGetCrafterType(*ptACD);
                if (eType == ptMessage->eType && (ActorGetType(ptACD->tSNOName) == ACTORTYPE_SERVER_PROP || ActorDefinitionGetTagMapValueInt(TAG_ACTOR_UPGRADE_WITH_CRAFTER, ptACD->snoActor)))
                {
                    AnimPlayerPlayAnimation(*ptACD, ptMessage->dwAnimTag, 0xffffffff, TRUE, ANIM_REASON_GIZMO, 0.0f, FALSE, 1.0f);
                    AnimPlayerIdleAnimationChanged(*ptACD, ptMessage->dwNewIdle);
                }
            }
        }
        break;

    case SMSG_CHANGE_ACTOR:
        {
            ACDChangeActorMessage * ptChangeActorMessage = (ACDChangeActorMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptChangeActorMessage->ann);

            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                if (ptACD->snoActor != ptChangeActorMessage->snoActor)
                {
                    RActorID idRActor = (RActorID)ACDGetOwner(idACD);
                    if (idRActor != RACTORID_INVALID)
                    {
                        RActorDelete(idRActor);
                        RActorCreateForACD(idACD, ptChangeActorMessage->snoActor);
                    }
                }
                else
                {
                    ACDReapplyVisualInventory(idACD);
                }
            }
        }
        break;

    case SMSG_PLAYER_WARPED:
        {
            Prefetch::BlockUntilPriorityIdle(SNOFilesAsyncPriority_Medium);
            PlayerWarpedMessage * ptPlayerWarpedMessage = (PlayerWarpedMessage *)pMessage;
            CPlayerWarped(ptPlayerWarpedMessage->eWarpReason, ptPlayerWarpedMessage->flWarpFadeInSeconds);
        }
        break;

    case SMSG_GAME_SYNCED_DATA:
        {
            GameSyncedDataMessage * ptGameSyncedDataMessage = (GameSyncedDataMessage *)pMessage;
            GameCommonData & tGCD = GameGetActiveGameCommonData();

            if (!SNOPackChunkLoadAct(ptGameSyncedDataMessage->tSyncedData.eAct))
            {
                Console::Notifications::ShowLeaveGameDialog(Console::GamerProfile::GetPrimaryProfileUserIndex(), FALSE, 0, 0, 0);
                for (int8 i = 0; i < 4; i++)
                {
                    if (PlayerGetByControllerIndex(i) != NULL)
                    {
                        CHero::Save(i, 0x40000);
                    }
                }

                if (GameCommonDataExists())
                {
                    GameGetActiveGameCommonData().tGameSyncedData.eAct = ACT_1;
                }

                CGameLoadingScreenEnable(LSC_ALL);
                Console::Online::PartyLeave();
                GameStartMenu(ACT_5);
            }
            else
            {
                CGameLoadingScreenSetBlackOnly(FALSE);

                tGCD.tGameSyncedData = ptGameSyncedDataMessage->tSyncedData;

                if (SGameManager::GetGameCount() == 0)
                {
                    SNOFilesSetCurrentAct(ptGameSyncedDataMessage->tSyncedData.eAct);
                }

                ActCacheWaypoints(ptGameSyncedDataMessage->tSyncedData.eAct);

                if (GameIsOpenWorld())
                {
                    GameGetOpenWorldMode();
                }
            }
        }
        break;

    case SMSG_WORLD_STATUS:
        {
            WorldStatusMessage * ptWorldStatusMessage = (WorldStatusMessage *)pMessage;
            CWorldSetStatus(ptWorldStatusMessage->idSWorld, ptWorldStatusMessage->dwWorldStatusBits);
        }
        break;

    case SMSG_WORLD_SYNCED_DATA:
        {
            WorldSyncedDataMessage * ptWorldSyncedDataMessage = (WorldSyncedDataMessage *)pMessage;

            CWorld * ptCWorld = CWorldGetFromSWorldID(ptWorldSyncedDataMessage->idSWorld);
            if (ptCWorld != NULL)
            {
                ptCWorld->tWCD.tWorldSyncedData = ptWorldSyncedDataMessage->tSyncedData;
            }
        }
        break;

    case SMSG_WORLD_DELETED:
        {
            WorldDeletedMessage * ptWorldDeletedMessage = (WorldDeletedMessage *)pMessage;

            CWorld * ptCWorld = CWorldGetFromSWorldID(ptWorldDeletedMessage->idSWorld);
            if (ptCWorld != NULL)
            {
                ClientWorldDispose(ptCWorld->id);
            }

            UIMinimap_DisposeForWorld(ptWorldDeletedMessage->idSWorld);
        }
        break;

    case SMSG_TRIGGER_LIGHTNING:
        Weather::TriggerLightning(GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData());
        break;

    case CCMSG_END_OF_TICK:
        {
            EndOfTickMessage * ptEndOfTickMessage = (EndOfTickMessage *)pMessage;
            uint32 dwNextTickTime = ptEndOfTickMessage->dwTickNext;
            CGameStartTick(dwNextTickTime);
        }
        break;

    case SMSG_PLAY_CONV_LINE:
        {
            PlayConvLineMessage * ptPlayConvLineMessage = (PlayConvLineMessage *)pMessage;
            ACDID idACDSpeaker = ACDFromANN(ptPlayConvLineMessage->annSpeaker);

            ACDID idACDParticipants[9];
            for (int32 i = 0; i < 9; i++)
            {
                idACDParticipants[i] = ACDFromANN(ptPlayConvLineMessage->annParticipants[i]);
            }

            UIConversation::PlayLine(idACDSpeaker, idACDParticipants, ptPlayConvLineMessage->tParams, ptPlayConvLineMessage->dwTimeToDisplay);
        }
        break;

    case SMSG_STOP_CONV_LINE:
        {
            StopConvLineMessage * ptStopConvLineMessage = (StopConvLineMessage *)pMessage;
            UIConversation::StopLine(ptStopConvLineMessage->nConvLineGUID, ptStopConvLineMessage->fStoppedByUser);
        }
        break;

    case SMSG_END_CONV:
        {
            EndConversationMessage * ptEndMessage = (EndConversationMessage *)pMessage;
            UIConversation::EndConversation(ptEndMessage->snoConversation, ptEndMessage->annSpeaker, ptEndMessage->nPlayerTalkingIndex);
        }
        break;

    case SMSG_CONFIRM_HIRELING_SWAP:
        {
            HirelingSwapMessage * ptSwapMessage = (HirelingSwapMessage *)pMessage;
            UIFollowerSwap::Show(ptSwapMessage->eNewClass, ptHeader->nControllerIndex, FALSE);
        }
        break;

    case SMSG_HIRELING_AUTO_DISMISS:
        {
            if (!CGameInScriptedSequence())
            {
                HirelingSwapMessage * ptSwapMessage = (HirelingSwapMessage *)pMessage;
                UIFollowerSwap::Show(ptSwapMessage->eNewClass, ptHeader->nControllerIndex, TRUE);
            }
        }
        break;

    case SMSG_SET_DEATH_FADE_TIMES:
        {
            DeathFadeTimeMessage * ptDeathFadeTimeMsg = (DeathFadeTimeMessage *)pMessage;

            ACDID idACD = ACDFromANN(ptDeathFadeTimeMsg->ann);
            if (idACD != ACDID_INVALID)
            {
                RActorID idRActor = (RActorID)ACDGetOwner(idACD);
                RActor * ptRActor = RActorTryToGet(idRActor);
                if (ptRActor != NULL)
                {
                    RActorSetFadeTimes(ptRActor, ptDeathFadeTimeMsg->nTimeUntilFade * (1.0f / 60.0f), ptDeathFadeTimeMsg->nFadeTime * (1.0f / 60.0f), ptDeathFadeTimeMsg->bRecurseForAttachments);
                }
            }
        }
        break;

    case SMSG_RESET_DEATH_FADE_TIMES:
        {
            ANNDataMessage * ptANNMessage = (ANNDataMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptANNMessage->ann);

            RActorID idRActor = (RActorID)ACDGetOwner(idACD);
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL)
            {
                RActorResetDeathFadeTimes(ptRActor);
            }
        }
        break;

    case SMSG_PLAYER_ACTIVATED_QUEST:
        {
            PlayerQuestMessage * ptMessage = (PlayerQuestMessage *)pMessage;
            Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            if (ptPlayer == NULL)
            {
                break;
            }

            CRefString usMessage;

            ScopedQuestDefinitionLockR pdefQuest(ptMessage->snoQuest);
            switch (pdefQuest->eQuestType)
            {
            case QUEST_TYPE_MAIN_QUEST:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerActivatedQuest");
                break;
            case QUEST_TYPE_EVENT:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerActivatedEvent");
                break;
            case QUEST_TYPE_CHALLENGE:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerActivatedChallenge");
                break;
            case QUEST_TYPE_BOUNTY:
            case QUEST_TYPE_HORADRIC_QUEST:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerActivatedBounty");
                break;
            case QUEST_TYPE_SIDE_QUEST:
            case QUEST_TYPE_PAGE_OF_FATE:
            case QUEST_TYPE_SET_DUNGEON:
            case QUEST_TYPE_SET_DUNGEON_BONUS:
            case QUEST_TYPE_SET_DUNGEON_MASTERY:
            case QUEST_TYPE_SET_DUNGEON_TRACKER:
                usMessage = "";
                break;
            default:
                DisplayInternalError(NULL, "", 5346, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            CRefString usPlayer = CPlayerGetServiceName(*ptPlayer, FALSE);
            CRefString usQuest = QuestsGetUIName(ptMessage->snoQuest);
            CRefString usLevelAreaName = LevelAreaGetLocalizedName(ptMessage->snoLevelArea);
            usMessage = StringListEntryReplace(usMessage, "{QUEST}", usQuest, 999);
            usMessage = StringListEntryReplace(usMessage, "{Player}", usPlayer, 999);
            usMessage = StringListEntryReplace(usMessage, "{LEVELAREA}", usLevelAreaName, 999);

            CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
            Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);
            UI_Print(sAfterLanguageRules, TRUE);
        }
        break;

    case SMSG_PLAYER_COMPLETED_QUEST:
        {
            PlayerQuestMessage * ptMessage = (PlayerQuestMessage *)pMessage;
            Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            if (ptPlayer == NULL)
            {
                break;
            }

            CRefString usMessage;

            ScopedQuestDefinitionLockR pdefQuest(ptMessage->snoQuest);
            switch (pdefQuest->eQuestType)
            {
            case QUEST_TYPE_MAIN_QUEST:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerCompletedQuest");
                break;
            case QUEST_TYPE_EVENT:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerCompletedEvent");
                break;
            case QUEST_TYPE_CHALLENGE:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerCompletedChallenge");
                break;
            case QUEST_TYPE_BOUNTY:
            case QUEST_TYPE_HORADRIC_QUEST:
                usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerCompletedBounty");
                break;
            case QUEST_TYPE_SIDE_QUEST:
            case QUEST_TYPE_PAGE_OF_FATE:
                usMessage = "";
                break;
            default:
                DisplayInternalError(NULL, "", 5409, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            CRefString usPlayer = CPlayerGetServiceName(*ptPlayer, FALSE);
            CRefString usQuest = QuestsGetUIName(ptMessage->snoQuest);
            usMessage = StringListEntryReplace(usMessage, "{QUEST}", usQuest, 999);
            usMessage = StringListEntryReplace(usMessage, "{Player}", usPlayer, 999);

            CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
            Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);
            UI_Print(sAfterLanguageRules, TRUE);
        }
        break;

    case SMSG_PLAYER_DESYNC_SNAP:
        {
            PlayerDeSyncSnapMessage * ptMessage = (PlayerDeSyncSnapMessage *)pMessage;

            const Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            ActorCommonData * ptACD = ptPlayer != NULL ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
            if (ptACD != NULL)
            {
                WorldPlace placeCurr = ptACD->GetPlace();
                if (placeCurr.idSWorld != ptMessage->placeSnap.idSWorld)
                {
                    return TRUE;
                }

                ActorMovement * ptMove = ptACD->GetActorMovement();
                if (ptMove != NULL)
                {
                    ptMove->SnapToNewPlace(ptMessage->placeSnap, FALSE, FALSE, TRUE, 0);
                }
            }
        }
        break;

    case SMSG_REMOVE_FROM_SPHERE_LIST:
        {
            ANNDataMessage * ptANNMessage = (ANNDataMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptANNMessage->ann);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD != NULL)
            {
                ACDCollDisposeSphereNode(*ptACD);
            }
        }
        break;

    case SMSG_DISPLAY_GAME_TEXT:
    case SMSG_DISPLAY_GAME_CHAT_TEXT:
        {
            DisplayGameTextMessage * ptMessage = (DisplayGameTextMessage *)pMessage;
            CRefString sText = StringListFetch(ptMessage->szMsg);

            if (ptMessage->nParam1.Exists())
            {
                CRefString sParam;
                sParam.Format("%d", ptMessage->nParam1.Get());
                sText = StringListEntryReplace(sText, "{Param1}", sParam, 999);
            }

            if (ptMessage->nParam2.Exists())
            {
                CRefString sParam;
                sParam.Format("%d", ptMessage->nParam2.Get());
                sText = StringListEntryReplace(sText, "{Param2}", sParam, 999);
            }

            CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(sText.str()));
            if (ptHeader->eType == SMSG_DISPLAY_GAME_TEXT)
            {
                UIGame::SetGameText(sAfterLanguageRules);
            }
            else
            {
                UI_Print(sAfterLanguageRules, FALSE);
            }
        }
        break;

    case SMSG_DISPLAY_ERROR:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UIGame::DisplayError(GameErrorData(ptMessage->nData), FALSE, ptPlayer);
        }
        break;

    case SMSG_DISPLAY_VAGUE_LOCKED_ERROR:
        {
            DWordDataMessage * ptMessage = (DWordDataMessage *)pMessage;
            StringLabelHandle hLabel = ptMessage->dwData;
            UIGame::DisplayItemRequiredError(GBID_NONE, hLabel);
        }
        break;

    case SMSG_DISPLAY_ITEM_REQUIRED_ERROR:
        {
            GBIDDataMessage * ptMessage = (GBIDDataMessage *)pMessage;
            UIGame::DisplayItemRequiredError(ptMessage->gbidData, 0);
        }
        break;

    case SMSG_SET_DUNGEON_START:
        {
            SetDungeonDialogMessage * ptMessage = (SetDungeonDialogMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UISetDungeon::Show(ptMessage->nLabelDescription, ptMessage->nLabelTitle, ptPlayer);
        }
        break;

    case SMSG_SET_DUNGEON_CANNOT_START:
        {
            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UISetDungeon::ShowCannotStartMessage(ptPlayer);
        }
        break;

    case SMSG_CANCEL_ACD_TARGET:
        {
            ANNDataMessage * ptCancelACDTargetMsg = (ANNDataMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptCancelACDTargetMsg->ann);
            if (idACD != ACDID_INVALID)
            {
                CPlayerInputCancelACDTarget(idACD, LocalPlayerGet());
            }
        }
        break;

    case SMSG_RESET_ACD_EFFECTS:
        {
            ANNDataMessage * ptCancelACDTargetMsg = (ANNDataMessage *)pMessage;
            ACDID idACD = ACDFromANN(ptCancelACDTargetMsg->ann);

            RActorID idRActor = (RActorID)ACDGetOwner(idACD);
            RActor * ptRActor = RActorTryToGet(idRActor);
            if (ptRActor != NULL)
            {
                RActorResetActorEffectsForAttributes(ptRActor);
            }
        }
        break;

    case SMSG_AUTOASSIGNED_TO_BUTTON:
        {
            IntDataMessage * ptAutoAssignMsg = (IntDataMessage *)pMessage;

            SkillButton eSkillButton = (SkillButton)ptAutoAssignMsg->nData;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UIHUD::Console::MarkSkillSlotAsChanged(ptPlayer, eSkillButton);
        }
        break;

    case SMSG_LOOK_AT_ACD:
        {
            ACDLookAtMessage * ptACDLookAtMessage = (ACDLookAtMessage *)pMessage;
            ACDID idACDSource = ACDFromANN(ptACDLookAtMessage->annSrc);
            ACDID idACDTarget = ACDFromANN(ptACDLookAtMessage->annTarget);

            RActorID idSource = (RActorID)ACDGetOwner(idACDSource);
            if (idSource != RACTORID_INVALID)
            {
                RActorID idTarget = (RActorID)ACDGetOwner(idACDTarget);
                RActorAnimationLookAt(idSource, idTarget);
            }
        }
        break;

    case SMSG_CAMERA_FOCUS_ACD:
        if (CGameInScriptedSequence())
        {
            CameraFocusMessage * ptCameraFocusMessage = (CameraFocusMessage *)pMessage;

            ACDID idACDTarget = ACDFromANN(ptCameraFocusMessage->ann);
            if (idACDTarget != ACDID_INVALID)
            {
                RActorID idTarget = (RActorID)ACDGetOwner(idACDTarget);
                if (idTarget != RACTORID_INVALID)
                {
                    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
                    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
                    if (ptACDTarget != NULL && ptACDPlayer != NULL && ptACDTarget->GetPlace().idSWorld == ptACDPlayer->GetPlace().idSWorld)
                    {
                        CinematographyFocus(idTarget, ptCameraFocusMessage->bSnap, ptCameraFocusMessage->flDuration);
                    }
                }
            }
            else
            {
                CinematographyFocus(RACTORID_INVALID, ptCameraFocusMessage->bSnap, ptCameraFocusMessage->flDuration);
            }
        }
        break;

    case SMSG_HANDLE_EVENT_SKIP:
        {
            if (PlayerGetNumPlayers() > 1)
            {
                PlayerIndexMessage * ptSkipMessage = (PlayerIndexMessage *)pMessage;
                Player * ptPlayer = PlayerGetByPlayerIndex(ptSkipMessage->nPlayerIndex);
                if (ptPlayer != NULL)
                {
                    CRefString usMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "PlayerSkippedSequence");
                    CRefString usPlayer = CPlayerGetServiceName(*ptPlayer, FALSE);
                    usMessage = StringListEntryReplace(usMessage, "{Player}", usPlayer, 999);

                    CRefString sAfterLanguageRules(LanguageRules::ApplyLanguageRules(usMessage.str()));
                    Set_Text_Color(sAfterLanguageRules, FONTCOLOR_SYSTEM);
                    UI_Print(sAfterLanguageRules, TRUE);
                }
            }

            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.bScriptedSequenceCancelled = TRUE;
        }
        break;

    case SMSG_CAMERA_YAW:
        {
            if (CGameInScriptedSequence())
            {
                CameraYawMessage * ptYawMessage = (CameraYawMessage *)pMessage;
                CinematographySetYaw(ptYawMessage->aAngle, ptYawMessage->bSnap, ptYawMessage->flDuration);
            }
        }
        break;

    case SMSG_CAMERA_ZOOM:
        {
            if (CGameInScriptedSequence())
            {
                CameraZoomMessage * ptZoomMessage = (CameraZoomMessage *)pMessage;
                CinematographySetScriptedZoom(ptZoomMessage->flZoom, ptZoomMessage->bSnap, ptZoomMessage->flDuration);
            }
        }
        break;

    case SMSG_CAMERA_TRIGGER_FADE:
        if (CGameInScriptedSequence())
        {
            PostFXFadeParams tFadeParams;
            tFadeParams.flStartAlpha = 1.0f;
            tFadeParams.flEndAlpha = 0.0f;
            tFadeParams.pfnFadeCallback = NULL;
            tFadeParams.flFadeTimeInSeconds = 1.0f;
            tFadeParams.rgbaColor = crgbaBlack;
            tFadeParams.flZ = 1.0f;
            tFadeParams.fKeepOn = FALSE;
            PostFXFadeStart(tFadeParams);
        }
        break;

    case SMSG_CAMERA_TRIGGER_FADE_TO_BLACK:
        if (CGameInScriptedSequence())
        {
            BoolDataMessage * ptFadeToBlackMessage = (BoolDataMessage *)pMessage;

            PostFXFadeParams tFadeParams;
            tFadeParams.flStartAlpha = 0.0f;
            tFadeParams.flEndAlpha = 1.0f;
            tFadeParams.fKeepOn = ptFadeToBlackMessage->bVal;
            tFadeParams.pfnFadeCallback = NULL;
            tFadeParams.flFadeTimeInSeconds = 1.0f;
            tFadeParams.rgbaColor = crgbaBlack;
            tFadeParams.flZ = 1.0f;
            PostFXFadeStart(tFadeParams);
        }
        break;

    case SMSG_CAMERA_TRIGGER_INSTANT_FADE_TO_BLACK:
        if (CGameInScriptedSequence())
        {
            PostFXFadeParams tFadeParams;
            tFadeParams.flStartAlpha = 0.0f;
            tFadeParams.flEndAlpha = 1.0f;
            tFadeParams.pfnFadeCallback = NULL;
            tFadeParams.flFadeTimeInSeconds = 0.0f;
            tFadeParams.rgbaColor = crgbaBlack;
            tFadeParams.flZ = 1.0f;
            tFadeParams.fKeepOn = TRUE;
            PostFXFadeStart(tFadeParams);
        }
        break;

    case SMSG_CAMERA_SCRIPTED_SEQUENCE_START:
        if (GameTesting::GetTestType() == 0)
        {
            const BoolDataMessage * ptMessage = (const BoolDataMessage *)pMessage;
            if (!ptMessage->bVal && Prefs::GetGameplayPrefs().GetDisableCutscenes())
            {
                BoolDataMessage tMessage;
                tMessage.bVal = TRUE;
                MessageSendToServer(CMSG_REQUEST_EVENT_SKIP, &tMessage, sizeof(tMessage), LocalPlayerGet());
            }

            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.bInScriptedSequence = TRUE;
            tCGameGlobals.bScriptedSequenceCancelled = FALSE;

            if (UISideScreenAnyAreVisible())
            {
                UISideScreenCloseAll(0xffffffff);
            }

            if (UIFollowerSwap::IsVisible())
            {
                UIFollowerSwap::Close();
            }

            if (Console::Screens::IsScreenSetOpen())
            {
                Console::Screens::CloseScreenSet(TRUE, TRUE);
            }

            UI_GetWindowFromID(cgUIIDNormalLayer).SetActive(FALSE);
            AppDrawFlagSet(28, TRUE);
            UILorePlayer::Close(GetUIIDNone(), TRUE);

            UI_GetWindowFromID(UIDLG_SCRIPTED_SEQUENCE).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_SCRIPTED_SEQUENCE_LETTERBOX_TOP).SetWindowColor(crgbaBlack);
            UI_GetWindowFromID(UIWND_SCRIPTED_SEQUENCE_LETTERBOX_BOTTOM).SetWindowColor(crgbaBlack);

            CinematographySetYaw(0.0f, TRUE, 0.0f);
            CinematographySetScriptedZoom(0.0f, TRUE, 0.0f);
        }
        break;

    case SMSG_CAMERA_SCRIPTED_SEQUENCE_STOP:
        {
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            if (tCGameGlobals.bInScriptedSequence)
            {
                tCGameGlobals.bInScriptedSequence = FALSE;
                UIConfirmation::Close();
                PostFXFadeDisablePermanentFade();
                UI_GetWindowFromID(cgUIIDNormalLayer).SetActive(TRUE);
                AppDrawFlagSet(28, FALSE);
                UI_GetWindowFromID(UIDLG_SCRIPTED_SEQUENCE).SetActive(FALSE);
            }
        }
        break;

    case SMSG_BOSS_ZOOM:
        {
            BossZoomMessage * ptBossZoomMessage = (BossZoomMessage *)pMessage;
            float flZoom = ptBossZoomMessage->flZoom;
            float flZoomSpeed = ptBossZoomMessage->flZoomSpeed;
            CinematographySetDefaultZoom(flZoom, flZoomSpeed);
        }
        break;

    case SMSG_CALLOUT_EVENT:
        {
            CalloutMessage * ptMessage = (CalloutMessage *)pMessage;
            Player * ptPlayer = LocalPlayerGet();
            if (ptPlayer != NULL)
            {
                CalloutEvent tEvent(ptMessage->eType, 0);
                tEvent.uCount = 0;
                tEvent.bNewRecord = FALSE;
                tEvent.uTotalTime = ptMessage->uTime.Exists() ? ptMessage->uTime.Get() : 0;
                CPlayerAddCalloutEvent(tEvent);
            }
        }
        break;

    case SMSG_CURRENT_HANDICAP_LEVEL:
        {
            const HandicapMessage * ptMessage = (const HandicapMessage *)pMessage;

            GameCommonData & tGCD = GameGetActiveGameCommonData();
            tGCD.nHandicapLevel = ptMessage->nHandicapLevel;
        }
        break;

    case SMSG_START_END_COUNTDOWN:
        if (!UICountDown::IsVisible() && PlayersGetCount() > 1)
        {
            BOOL bFound = FALSE;
            CRefString sDisplay = StringListFetch("H2OLayout:GameEndsIn", bFound);

            ActTransitionStartedMessage * ptStartMsg = (ActTransitionStartedMessage *)pMessage;
            uint32 dwPredictedNow = CGameGetPredictedServerTime();
            uint32 dwTransitionDuration = ptStartMsg->dwActTransitionTime - dwPredictedNow;

            UICountDown::Show(dwTransitionDuration * (1.0f / 60.0f), bFound ? sDisplay.str() : "", NULL, NULL, FALSE, FALSE, SNO_NONE);
        }
        break;

    case SMSG_KILL_COUNTER_UPDATE:
        {
            const KillCounterUpdateMessage * ptKillCounterUpdateMessage = (const KillCounterUpdateMessage *)pMessage;
            Player * ptPlayer = LocalPlayerGet();
            if (ptPlayer != NULL)
            {
                CalloutEvent tEvent(CalloutEventFromKillCounter(ptKillCounterUpdateMessage->eType), 0);
                tEvent.uCount = ptKillCounterUpdateMessage->uCount;
                tEvent.flXPMultiplier = ptKillCounterUpdateMessage->flXPMultiplier;
                tEvent.uTotalTime = ptKillCounterUpdateMessage->uTotalTime;

                if (UICallouts::IsTickerType(ptKillCounterUpdateMessage->eType))
                {
                    if (!ptKillCounterUpdateMessage->bExpired)
                    {
                        UICallouts::IncreasedCount(&tEvent);
                    }
                    else
                    {
                        CPlayerAddCalloutEvent(tEvent);
                    }
                }
                else
                {
                    CPlayerAddCalloutEvent(tEvent);
                }
            }
        }
        break;

    case SMSG_KILL_COUNTER_REFRESH:
        UICallouts::RefreshMassacreTimer();
        break;

    case SMSG_CURRENCY_COUNTER_UPDATE:
        {
            const CurrencyCounterUpdateMessage * ptMessage = (const CurrencyCounterUpdateMessage *)pMessage;
            if (ptMessage->eCurrencyType == 0)
            {
                if (ptMessage->bExpired)
                {
                    UIGoldCounter::Expired(ptMessage->uAmountCollected);
                }
                else
                {
                    UIGoldCounter::NewAmount(ptMessage->uAmountCollected);
                }
            }
        }
        break;

    case SMSG_PLATINUM_ACHIEVEMENT_AWARDED: // no code in the original; label inferred
        break;

    case SMSG_RIFT_PARTICIPANTS_ADDED:
        {
            D3::GameMessage::RiftEndScreenAddParticipants tRiftParticipantsAdded;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tRiftParticipantsAdded))
            {
                DisplayInternalError(NULL, "", 6137, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
        break;

    case SMSG_SHOW_RIFT_END_SCREEN:
        {
            D3::GameMessage::RiftEndScreenInfo tRiftEndScreenInfo;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tRiftEndScreenInfo))
            {
                DisplayInternalError(NULL, "", 6155, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            if (ptHeader->nControllerIndex == Console::GamerProfile::GetPrimaryProfileUserIndex())
            {
                UIConsoleRiftEndScreen::Open(ptHeader->nControllerIndex, tRiftEndScreenInfo);
            }
        }
        break;

    case SMSG_PRELOAD_LOOT_RUN_LEADERBOARD_INFO:
        {
            D3::GameMessage::RiftStartPreloadingLeaderBoardData tPreloadData;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tPreloadData))
            {
                DisplayInternalError(NULL, "", 6181, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
        }
        break;

    case SMSG_LOW_HEALTH_COMBAT:
        {
            LowHealthCombatMessage * ptLowHealthCombatMessage = (LowHealthCombatMessage *)pMessage;

            CalloutEvent tEvent(6, ptLowHealthCombatMessage->uXPRewarded);
            tEvent.flHealthPercent = ptLowHealthCombatMessage->flHealthPercent;
            CPlayerAddCalloutEvent(tEvent);
        }
        break;

    case SMSG_SAVIOR:
        {
            SaviorMessage * ptSaviorMessage = (SaviorMessage *)pMessage;

            CalloutEvent tEvent(5, ptSaviorMessage->uXPRewarded);
            tEvent.idACDSavedPlayer = ACDFromANN(ptSaviorMessage->annSavedPlayer);
            CPlayerAddCalloutEvent(tEvent);
        }
        break;

    case SMSG_FLOATING_NUMBER:
        {
            FloatingNumberMessage * ptFloatingNumberMessage = (FloatingNumberMessage *)pMessage;
            ACDID idACDTarget = ACDFromANN(ptFloatingNumberMessage->annTarget);
            ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
            if (ptACDTarget != NULL)
            {
                CPlayerAddFloatingDamageNumber(*ptACDTarget, ptFloatingNumberMessage->flDamage, ptFloatingNumberMessage->eType);
            }
        }
        break;

    case SMSG_FLOATING_GBID_AMOUNT:
        {
            FloatingGBIDAmountMessage * ptFloatingAmountMessage = (FloatingGBIDAmountMessage *)pMessage;

            CPlayerAddFloatingGBIDAmountNumber(ptHeader->nControllerIndex, ptFloatingAmountMessage->wpPlace, ptFloatingAmountMessage->nAmount, ptFloatingAmountMessage->gbidItem);
        }
        break;

    case SMSG_FLOATING_AMOUNT:
        {
            FloatingAmountMessage * ptFloatingAmountMessage = (FloatingAmountMessage *)pMessage;

            CPlayerAddFloatingAmountNumber(
                ptHeader->nControllerIndex,
                ptFloatingAmountMessage->wpPlace,
                ptFloatingAmountMessage->nAmount,
                ptFloatingAmountMessage->nAmount2.Exists() ? ptFloatingAmountMessage->nAmount2.Get() : 0,
                ptFloatingAmountMessage->eType);
        }
        break;

    case SMSG_REMOVE_NEAREST_RAGDOLL:
        {
            RemoveRagdollMessage * ptRagdollMessage = (RemoveRagdollMessage *)pMessage;
            ACDID idACDTarget = ACDFromANN(ptRagdollMessage->annTarget);
            if (ACDTryToGet(idACDTarget) != NULL)
            {
                RActorRemoveNearestRagdoll(idACDTarget, ptRagdollMessage->snoMonster);
            }
        }
        break;

    case SMSG_PREFETCH:
        {
            SNONameDataMessage * ptPrefetchMessage = (SNONameDataMessage *)pMessage;
            Prefetch::AddToQueue(ptPrefetchMessage->snoname, SNOFilesAsyncPriority_High);
        }
        break;

    case SMSG_LORE_LEARNED:
        {
            LoreMessage * ptLoreMessage = (LoreMessage *)pMessage;

            for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
            {
                if (PlayerIsLocal(ptPlayer))
                {
                    if (ptPlayer->tHeroStateData.tSavedData.tLearnedLore.Find(ptLoreMessage->snoLore) == -1)
                    {
                        ptPlayer->tHeroStateData.tSavedData.tLearnedLore.Add(ptLoreMessage->snoLore);
                    }
                }
            }

            UILoreNotify::OnLoreAcquired(ptLoreMessage->snoLore);
            UILore::Console::OnLoreAcquired(ptLoreMessage->snoLore);
            UILore::RefreshUI();
        }
        break;

    case SMSG_TRADE:
        {
            TradeMessage * ptTradeMessage = (TradeMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            CTradeHandleMessage(ptPlayer, *ptTradeMessage);
        }
        break;

    case SMSG_TRIAL_OVER:
        {
            DWordDataMessage * ptMsg = (DWordDataMessage *)pMessage;
            UIGame::StartTrialTimer(ptMsg->dwData);
        }
        break;

    case SMSG_DEMO_OVER:
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            CBlizzconVictory();
        }
        break;

    case SMGS_BLIZZCON_RIFT_ENDED:
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            UIBlizzconEnd::Show(FALSE);
        }
        break;

    case SMSG_BETA_OVER:
        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            if (OnlineService::IsPTR())
            {
                UIVictory::Show(UIVictory::MODE_BETA);
            }
        }
        break;

    case SMSG_SET_SCREENSHOT_VIEW:
        {
            IntDataMessage * ptScreenshotViewMsg = (IntDataMessage *)pMessage;

            sSetScreenshotView((ScreenshotView)ptScreenshotViewMsg->nData);
        }
        break;

    case SMSG_TIMED_EVENT_STARTED:
        {
            TimedEventStartedMessage * ptStartMsg = (TimedEventStartedMessage *)pMessage;
            CTimedEventStart(ptStartMsg->tEvent);
        }
        break;

    case SMSG_TIMED_EVENT_MODIFIED:
        {
            TimedEventStartedMessage * ptModifiedMsg = (TimedEventStartedMessage *)pMessage;
            CTimedEventModified(ptModifiedMsg->tEvent);
        }
        break;

    case SMSG_ACT_TRANSITION_STARTED:
        {
            ActTransitionStartedMessage * ptStartMsg = (ActTransitionStartedMessage *)pMessage;
            CActTransitionStart(ptStartMsg->nPlayerIndex, ptStartMsg->dwActTransitionTime, ptStartMsg->bRecreateGameWithParty);
        }
        break;

    case SMSG_ACT_TRANSITION_ENDED:
        CActTransitionEnded();
        break;

    case SMSG_MAP_MARKER:
        {
            MapMarkerInfoMessage * ptMsg = (MapMarkerInfoMessage *)pMessage;
            UIMinimap_AddMarker(ptMsg);
        }
        break;

    case SMSG_MAP_OBJECTIVE_MARKER:
        {
            MapMarkerInfoMessage * ptMsg = (MapMarkerInfoMessage *)pMessage;
            UIMinimap_SetObjective(ptMsg);
        }
        break;

    case SMSG_OBJECTIVE_FINDER_ACTIVATED:
        {
            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;
            ACDID idACDGizmo = ACDFromANN(ptMsg->ann);
            ActorCommonData * ptACDGizmo = ACDTryToGet(idACDGizmo);
            if (ptACDGizmo != NULL && ptACDGizmo->IsGizmo())
            {
                UIMinimap_SetObjectiveCone(ptACDGizmo);
            }
        }
        break;

    case SMSG_CRAFTING_DATA_BLACKSMITH:
    case SMSG_CRAFTING_DATA_JEWELER:

    case SMSG_CRAFTING_DATA_MYSTIC:
    case SMSG_CRAFTING_DATA_HORADRIM:

    case SMSG_CRAFTING_DATA_TEST:
    case SMSG_CRAFTING_DATA_BLACKSMITH_INITIAL:

    case SMSG_CRAFTING_DATA_JEWELER_INITIAL:
    case SMSG_CRAFTING_DATA_MYSTIC_INITIAL:

    case SMSG_CRAFTING_DATA_HORADRIM_INITIAL:
    case SMSG_CRAFTING_DATA_TEST_INITIAL:
        {
            D3::ItemCrafting::CrafterData tCrafterData;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tCrafterData))
            {
                DisplayInternalError(NULL, "", 6472, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            BOOL fInitialInfo;
            CrafterType eType = ItemCrafting_BackEnd::CrafterTypeFromMessageType(ptHeader->eType, fInitialInfo);
            if (eType != -1)
            {
                Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
                UIUtils::Console::UpdateRecipes(ptPlayer, eType, fInitialInfo, tCrafterData, ptPlayer->tCrafterSavedData.tCrafterData[eType]);
                ptPlayer->tCrafterSavedData.tCrafterData[eType].Import(tCrafterData);
                SNOPreload::UpdateCrafters();
            }
        }
        break;

    case SMSG_TIMED_EVENT_FINISHED:
    case SMSG_TIMED_EVENT_RESET:
        {
            SNODataMessage * ptMsg = (SNODataMessage *)pMessage;
            CTimedEventFinished(ptMsg->snoData);
        }
        break;

    case SMSG_CRAFTING_DATA_TRANSMOG:
    case SMSG_CRAFTING_DATA_TRANSMOG_INITIAL:
        {
            D3::GameBalance::BitPackedGbidArray tTransmogData;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tTransmogData))
            {
                DisplayInternalError(NULL, "", 6522, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                ptPlayer->tCrafterSavedData.tTransmogData.Import(tTransmogData, NULL);
            }
        }
        break;

    case SMSG_UPDATED_COSMETIC_ITEM_DATA:
        {
            D3::CosmeticItems::CosmeticItems tCosmeticItemData;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tCosmeticItemData))
            {
                DisplayInternalError(NULL, "", 6561, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                ptPlayer->tCosmeticItemSavedData.tCosmeticItemData.Import(tCosmeticItemData);
            }
        }
        break;

    case SMSG_CURRENCY_DATA_FULL:
    case SMSG_CURRENCY_DATA_PARTIAL:
        {
            D3::Items::CurrencySavedData tCurrencySavedData;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tCurrencySavedData))
            {
                DisplayInternalError(NULL, "", 6593, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);

            BOOL bResetCurrencyData = ptHeader->eType == SMSG_CURRENCY_DATA_FULL;
            ptPlayer->tCurrencySavedData.Import(tCurrencySavedData, bResetCurrencyData);
        }
        break;

    case SMSG_EXTRACTED_LEGENDARIES_DATA_FULL:
    case SMSG_EXTRACTED_LEGENDARIES_DATA_PARTIAL:
        {
            D3::GameBalance::BitPackedGbidArray tExtractedLegendariesData;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tExtractedLegendariesData))
            {
                DisplayInternalError(NULL, "", 6623, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                if (ptHeader->eType == SMSG_EXTRACTED_LEGENDARIES_DATA_FULL)
                {
                    ptPlayer->tCrafterSavedData.tExtractedLegendariesData.SetToDefaults();
                }
                ptPlayer->tCrafterSavedData.tExtractedLegendariesData.Import(tExtractedLegendariesData);
            }
        }
        break;

    case SMSG_LOAD_LEGENDARY_POWER_ICONS:
        {
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            (void)ptPlayer;
        }
        break;

    case SMSG_RESET_EXTRACTED_LEGENDARIES:
        {
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                ptPlayer->tCrafterSavedData.tExtractedLegendariesData.SetToDefaults();
            }
        }
        break;

    case SMSG_ASSIGNED_SKILL_TO_SLOT:
        {
            AssignSkillMessage * ptMessage = (AssignSkillMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            PlayerAssignToActiveSkillSlot(*ptPlayer, ptMessage->snoPower, ptMessage->nRuneType, ptMessage->nSlot);
            UISkills::Console::Refresh(ptPlayer);
            UIBuffs::CleanUpBuffIconsForSkillButtons();



            ptPlayer->tClientInfo.GameTickOfLastHeldPower.RemoveAll();

            SNOPreload::UpdateSkills();
        }
        break;

    case SMSG_ASSIGNED_TRAIT_TO_SLOT:
        {
            AssignSkillMessage * ptMessage = (AssignSkillMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            PlayerAssignToTraitSlot(*ptPlayer, ptMessage->snoPower, ptMessage->nSlot);
            UISkills::Console::Refresh(ptPlayer);
            SNOPreload::UpdateSkills();
        }
        break;

    case SMSG_ASSIGNED_LEGENDARY_POWER_TO_SLOT:
        {
            AssignLegendaryPowerMessage * ptMessage = (AssignLegendaryPowerMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            PlayerAssignToLegendaryPowerSlot(*ptPlayer, ptMessage->gbidLegendaryItem, ptMessage->nSlot);
            SNOPreload::UpdateSkills();
        }
        break;

    case SMSG_BOSS_JOIN_ENCOUNTER:
        {
            const BossEncounterMessage * ptMessage = (const BossEncounterMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UIBossEncounter::Enter_Close();
            UIBossEncounter::JoinParty_Open(ptPlayer, ptMessage);
        }
        break;

    case SMSG_SET_DUNGEON_JOIN:
        {
            const SetDungeonDialogMessage * ptMessage = (const SetDungeonDialogMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UISetDungeon::JoinParty_Open(ptPlayer, ptMessage);
        }
        break;

    case SMSG_BOSS_START_ENCOUNTER:
        {
            const BossEncounterMessage * ptMessage = (const BossEncounterMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UIBossEncounter::Enter_Open(ptPlayer, ptMessage);
        }
        break;

    case SMSG_RIFT_START_ENCOUNTER:
        Console::Screens::OpenToScreen(Console::Screens::RIFT_ENTRY_SCREEN_SET, 0, ptHeader->nControllerIndex, FALSE);
        break;

    case SMSG_RIFT_INVITE_STATE:
        {
            const EncounterInviteStateMessage * ptStateMsg = (const EncounterInviteStateMessage *)pMessage;
            UIJoinRift::InviteStatusUpdate(ptStateMsg);
        }
        break;

    case SMSG_RIFT_JOIN_ENCOUNTER:
        {
            const RiftJoinMessage * ptStateMsg = (const RiftJoinMessage *)pMessage;
            UIJoinRift::Open(ptStateMsg);
        }
        break;

    case SMSG_RIFT_STARTED:
        UIJoinRift::Started();
        break;

    case SMSG_RIFT_CANCELED:
        UIJoinRift::Canceled();
        break;

    case SMSG_RIFT_ONE_DECLINED:
        {
            const IntDataMessage * ptOneDeclinedMessage = (const IntDataMessage *)pMessage;
            UIJoinRift::RemotePlayersDeclined(ptOneDeclinedMessage->nData);
        }
        break;

    case SMSG_CHOOSE_ENCHANT_AFFIX:
        {
            const EnchantAffixChooseMessage * ptMessage = (const EnchantAffixChooseMessage *)pMessage;
            UIMystic::Console::OnEnchantAffixChoose(ptMessage);
        }
        break;

    case SMSG_ENCHANT_AFFIX_RESULTS:
        {
            const ANNDataMessage * ptMessage = (const ANNDataMessage *)pMessage;
            UIMystic::Console::OnEnchantAffixResults(ptMessage);
        }
        break;

    case SMSG_VENDOR_TRAIN_RESULTS:
        {
            const IntDataMessage * ptMessage = (const IntDataMessage *)pMessage;
            UIBlacksmith::Console::OnTrainingResults(ptMessage);
            UIJeweler::Console::OnTrainingResults(ptMessage);
            UIMystic::Console::OnTrainingResults(ptMessage);
        }
        break;

    case SMSG_REPAIR_ACK:
        UIBlacksmith::Console::OnRepair();
        UIShop::Console::OnRepair();
        break;

    case SMSG_UNLOCK_TRANSMOG_RESULTS:
        {
            const GBIDDataMessage * ptMessage = (const GBIDDataMessage *)pMessage;
            UIMystic::Console::OnUnlockTransmogResults(ptMessage);
        }
        break;

    case SMSG_TRANSMOG_RESULTS:
        {
            const TransmogItemMessage * ptMessage = (const TransmogItemMessage *)pMessage;
            UIMystic::Console::OnTransmogResults(*ptMessage);
        }
        break;

    case SMSG_DYE_ITEM_RESULTS:
        {
            const DyeItemMessage * ptMessage = (const DyeItemMessage *)pMessage;
            UIMystic::Console::OnDyeItemResults(*ptMessage);
        }
        break;

    case SMSG_TRANSMUTE_RESULTS:
        {
            const TransmuteResultsMessage * ptMessage = (const TransmuteResultsMessage *)pMessage;
            UITransmute::OnTransmuteResults(*ptMessage);
        }
        break;

    case SMSG_TRANSMUTE_FAILED:
        {
            const IntDataMessage * ptMessage = (const IntDataMessage *)pMessage;
            UITransmute::OnTransmuteFailed(*ptMessage);
        }
        break;

    case SMSG_MYSTERY_ITEM_RESULTS:
        {
            const ANNDataMessage * ptMessage = (const ANNDataMessage *)pMessage;
            UIBloodShardVendor::Console::OnItemDelivered(ptMessage);
        }
        break;

    case SMSG_UPGRADE_JEWEL_RESULTS:
        {
            const JewelUpgradeResultsMessage * ptMessage = (const JewelUpgradeResultsMessage *)pMessage;
            UIJewelUpgrade::UpgradeResults(ptMessage);
        }
        break;

    case SMSG_DEBUG_DRAW_PRIM:
        {
            DebugDrawPrimMessage * ptDrawPrimMsg = (DebugDrawPrimMessage *)pMessage;
            CDebugDrawProcessMessage(*ptDrawPrimMsg);
        }
        break;

    case SMSG_CRAFTING_RESULT:
        {
            CraftingResultsMessage * ptMessage = (CraftingResultsMessage *)pMessage;
            UIJeweler::Console::OnCraftingResults(ptMessage);
            UIBlacksmith::Console::OnCraftingResults(ptMessage);
        }
        break;

    case SMSG_SALVAGE_RESULT:
        {
            SalvageResultsMessage * ptMessage = (SalvageResultsMessage *)pMessage;
            UIBlacksmith::Console::OnSalvageResults(ptMessage);
        }
        break;

    case SMSG_LOADOUT_EQUIP_NOTIFICATION:
        {
            PlayerLoadoutEquipResultsMessage * ptMessage = (PlayerLoadoutEquipResultsMessage *)pMessage;

            CRefString sMsg;
            if (ptMessage->bSocketsChanged)
            {
                sMsg = StringListFetch("Hero:EquipmentManager_EquipNotificationWithGems");
            }
            else
            {
                sMsg = StringListFetch("Hero:EquipmentManager_EquipNotification");
            }

            ActorCommonData * ptACDTutorialItem = NULL;
            for (int32 i = 0; i < ptMessage->nRemoveItemANNsCount; i++)
            {
                ACDNetworkName annRemovedItem = ptMessage->arRemovedItemANNs[i];
                ActorCommonData * ptACDRemovedItem = ACDTryToGet(ACDFromANN(annRemovedItem));
                if (ptACDRemovedItem != NULL)
                {
                    if (ptACDTutorialItem == NULL)
                    {
                        ptACDTutorialItem = ptACDRemovedItem;
                    }
                    UIInventory::AddCraftingNotification(annRemovedItem);
                }
            }

            if (ptACDTutorialItem != NULL)
            {
                gtTutorialMgr.ShowTutorial(UIWND_EQUIPMENT_MANAGER_NAVIGATION_ARMORY, GlobalSNOGet(GLOBALSNO_TUTORIAL_EQUIPMENT_MANAGER_INVENTORY), NULL, TRUE);
            }

            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_EQUIP_LOADOUT), cgtPSODefaults);

            Set_Text_Color(sMsg, FONTCOLOR_SYSTEM);
            UI_Print(sMsg, TRUE);
        }
        break;

    case SMSG_STACK_NOTIFY:
        {
            ANNDataMessage * ptStackMessage = (ANNDataMessage *)pMessage;
            ACDID idACDItem = ACDFromANN(ptStackMessage->ann);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem != NULL)
            {
                if (UIInventory::IsOpen())
                {
                    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_UI_STACK_ITEMS), cgtPSODefaults);
                }

                if (!ItemIsGold(*ptACDItem) && !ItemIsHealthPotion(*ptACDItem))
                {
                    UIEventQueue::Console::ReportItemPickup(PlayerGetByControllerIndex(ptHeader->nControllerIndex), *ptACDItem);
                }
            }
        }
        break;

    case SMSG_STACK_DECREASE_NOTIFY:
        {
            ANNDataMessage * ptStackMessage = (ANNDataMessage *)pMessage;
            ACDID idACDItem = ACDFromANN(ptStackMessage->ann);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem != NULL)
            {
                const ActorLocation & tLocation = ptACDItem->GetLocation();
                if (ptACDItem->GetLocation().eActorLocation == 1)
                {
                    UIInventory::Console::OnInventoryChanged(tLocation, tLocation, FALSE, TRUE);
                    UIFollower::Console::OnInventoryChanged(tLocation, tLocation, TRUE);
                    UISkills::Console::OnInventoryChanged(tLocation, tLocation, TRUE);
                    UIInventory::Console::OnStashChanged(tLocation, tLocation, TRUE);
                    UIInventory::Console::OnShopChanged(tLocation, tLocation, FALSE, TRUE);
                }
            }
        }
        break;

    case SMSG_IDENTIFY_NOTIFY:
        {
            ANNDataMessage * ptIDMessage = (ANNDataMessage *)pMessage;
            ACDID idACDItem = ACDFromANN(ptIDMessage->ann);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            (void)ptACDItem;
        }
        break;

    case SMSG_DYE_NOTIFY:
        UIInventory::Console::UpdateHeroModel();
        break;

    case SMSG_PVP_KILL:
        {
            PlayerIndexMessage * ptKillMsg = (PlayerIndexMessage *)pMessage;

            Player * ptKilledPlayer = PlayerGetByPlayerIndex(ptKillMsg->nPlayerIndex);
            if (ptKilledPlayer != NULL)
            {
                SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS);
                CRefString sDisplayString = LocalizedSprintf(StringListFetch(snoStringList, "KilledPlayer"), CPlayerGetServiceName(*ptKilledPlayer, FALSE).str(), NULL);
                UIGame::SetSpecialAreaText(sDisplayString.str(), NULL);
            }
        }
        break;

    case SMSG_PVP_ASSIST:
        {
            PlayerIndexMessage * ptKillMsg = (PlayerIndexMessage *)pMessage;

            Player * ptKilledPlayer = PlayerGetByPlayerIndex(ptKillMsg->nPlayerIndex);
            if (ptKilledPlayer != NULL)
            {
                SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS);
                CRefString sDisplayString = LocalizedSprintf(StringListFetch(snoStringList, "AssistInPlayerKill"), CPlayerGetServiceName(*ptKilledPlayer, FALSE).str(), NULL);
                UIGame::SetSpecialAreaText(sDisplayString.str(), NULL);

                PlaySoundOptions tOptions;
                tOptions.nPermutationIndex = 4;
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PVP_CALLOUT_KILLEVENT), tOptions);
            }
        }
        break;

    case SMSG_GAME_TESTING_DONE:
        if (ptHeader->nControllerIndex == 0)
        {
            GameTesting::ReceivedTestingDone();
        }
        break;

    case SMSG_GAME_TESTING_START_WORLD_SAMPLING:
        {
            GameTestingWorldSamplingStartMessage * ptStartMessage = (GameTestingWorldSamplingStartMessage *)pMessage;

            CRefString sFileName;
            if (ptHeader->nControllerIndex == 0)
            {
                sFileName.Format("W%d-S%d", ptStartMessage->snoWorld, ptStartMessage->snoScene);
                GameTesting::StartWorldFPSCollection(sFileName.str(), ptStartMessage->snoScene, GameTesting::GetDumpFPSFequency(), CmdLineGetParams()->tBitArray.Get(48));
            }
        }
        break;

    case SMSG_GAME_TESTING_START_SKILL_SAMPLING:
        {
            GameTestingSkillSamplingStartMessage * ptStartMessage = (GameTestingSkillSamplingStartMessage *)pMessage;

            CRefString sFileName;
            if (ptHeader->nControllerIndex == 0)
            {
                sFileName.Format("P%d-%d", ptStartMessage->snoPower, ptStartMessage->nRuneIndex);
                GameTesting::StartSkillFPSCollection(sFileName.str(), ptStartMessage->snoPower, ptStartMessage->nRuneIndex, GameTesting::GetDumpFPSFequency(), CmdLineGetParams()->tBitArray.Get(48));
            }
        }
        break;

    case SMSG_GAME_TESTING_STOP_SAMPLING:
        if (ptHeader->nControllerIndex == 0)
        {
            GameTesting::StopFPSCollection(TRUE);
        }
        break;

    case SMSG_SYSTEM_NOTIFICATION:
        {
            ServerNotificationDataMessage * ptMsg = (ServerNotificationDataMessage *)pMessage;
            CRefString sTime;
            CRefString sError;

            if (ptMsg->uSec > 60)
            {
                sTime = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes").str(), UnicodeText::FormatNumber((int32)ptMsg->uSec / 60, TRUE, FNS_NORMAL, 0).str(), NULL);
            }
            else
            {
                sTime = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralSeconds").str(), UnicodeText::FormatNumber((int32)ptMsg->uSec, TRUE, FNS_NORMAL, 0).str(), NULL);
            }

            CRefString msg;
            switch (ptMsg->eType)
            {
            case 0:
                msg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "BnetServerShutdown");
                break;
            case 1:
                msg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "BnetServerRestart");
                break;
            case 2:
                msg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "SeasonEnded");
                if (ptMsg->uSec < 30)
                {
                    ClientMarkSeasonEnded();
                }
                else if (ptHeader->nControllerIndex == LocalPlayerGetPrimary()->nControllerIndex)
                {
                    int32 nMin = ptMsg->uSec / 60;
                    switch (nMin)
                    {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 15:
                    case 30:
                        UIEventQueue::Console::DisplaySeasonEndingMessage(nMin);
                        break;
                    }
                }
                break;
            case 3:
                msg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "ChallengeRiftsDisabled");
                ClientMarkChallengeRiftsDisabled();
                break;
            }

            sError = LocalizedSprintf(msg.str(), sTime.str(), NULL);
        }
        break;

    case SMSG_SYSTEM_BROADCAST:
        {
            BroadcastTextMessage * ptMsg = (BroadcastTextMessage *)pMessage;
            CRefString sMsg;
            sMsg.Format("SYSTEM: %s", ptMsg->szMsg);
        }
        break;

    case SMSG_SET_CAMERA_OBSERVER:
        {
            PlayerSetCameraObserverMessage * ptMsg = (PlayerSetCameraObserverMessage *)pMessage;
            CinematographySetObserver(ptMsg->snoObserver);
            if (ptMsg->wpToOrbit.Exists())
            {
                CinematographySetOrbit(ptMsg->wpToOrbit.Get());
            }
        }
        break;

    case SMSG_SET_CAMERA_ORBIT:
        {
            PlayerSetCameraOrbitMessage * ptMsg = (PlayerSetCameraOrbitMessage *)pMessage;
            CinematographySetOrbit(ptMsg->wpToOrbit);
        }
        break;

    case SMSG_SET_CAMERA_DEFAULTS:
        {
            CinematographyClearOrbit(FALSE);
            CinematographySetObserver(CameraManagerGetDefaultObserver(0));
        }
        break;

    case SMSG_CONSOLE_SET_CAMERA_DEFAULTS:
        {
            PlayerConsoleSetCameraDefaultsMessage * ptMsg = (PlayerConsoleSetCameraDefaultsMessage *)pMessage;
            CinematographyClearOrbit(ptMsg->bForce);
            CinematographySetObserver(CameraManagerGetDefaultObserver(0));
        }
        break;

    case SMSG_MAIL_CONTENTS:
        {
            D3::GameMessage::MailContents tMessage;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tMessage))
            {
                DisplayInternalError(NULL, "", 7477, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            CPlayerHandleFetchMail(tMessage);
        }
        break;

    case SMSG_MAIL_SENT:
        {
            MailSentResponseMessage * ptResultMsg = (MailSentResponseMessage *)pMessage;
            CPlayerHandleSentMail(ptResultMsg->idMail, ptResultMsg->idErr);
        }
        break;

    case SMSG_MAIL_OPERATION_RESPONSE:
        {
            D3::GameMessage::MailOperationResponse tMessage;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tMessage))
            {
                DisplayInternalError(NULL, "", 7510, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            CPlayerHandleMailOperationResponse(tMessage);
        }
        break;

    case SMSG_ACHIEVEMENT_AWARDED:
        {
            D3::GameMessage::AchievementsAwardedConsole tMessage;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tMessage))
            {
                DisplayInternalError(NULL, "", 7586, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }

            Player * ptPlayer = PlayerGetByPlayerIndex(tMessage.player_index());

            AppGlobals & tAppGlobals = sgtAppGlobals;
            Console::Achievements * ptAchievements = tAppGlobals.ptAchievements;
            if (ptPlayer != NULL)
            {
                CHero::OnAchievementsAwarded(ptPlayer, tMessage);

                for (OnlineService::AchievementId idAchievement : tMessage.achievements())
                {
                    const OnlineService::AchievementDefinition * ptDef = ptAchievements->GetAchievementDefinition(idAchievement);
                    if (ptDef != NULL)
                    {
                        if (PlayerIsLocal(ptPlayer) && ptDef->m_profileAwardId != 0xffffffff)
                        {
                            Console::GamerProfile::AwardAchievement(ptPlayer->nControllerIndex, ptDef->m_profileAwardId);
                        }

                        if (!(ptDef->m_flags & 0x500))
                        {
                            if (PlayerIsLocal(ptPlayer))
                            {
                                if (PlayerGetLocalPlayerCount() == 1)
                                {
                                    Console::Screens::GetFanfareManager().SendFanfare(idAchievement, 0, NULL);
                                }
                                else
                                {
                                    Console::Screens::GetFanfareManager().DisplayFanfareForPlayer(ptPlayer->nPlayerIndex, idAchievement);
                                }

                                Console::GamerProfile::OnChallengeUnlocked(ptPlayer->nControllerIndex, idAchievement);
                            }
                            else
                            {
                                Console::Screens::GetFanfareManager().DisplayFanfareForPlayer(ptPlayer->nPlayerIndex, idAchievement);
                            }
                        }
                    }
                }
            }
        }
        break;

    case SMSG_CLEAR_FANFARE_QUEUE:
        Console::Screens::GetFanfareManager().ClearFanfareQueue();
        break;

    case SMSG_DUNGEON_FINDER_SEED:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.nDungeonFinderSeed = ptMessage->nData;
        }
        break;

    case SMSG_DUNGEON_FINDER_PARTICIPATING_PLAYER_COUNT:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.nDungeonFinderParticipatingPlayerCount = ptMessage->nData;
        }
        break;

    case SMSG_DUNGEON_FINDER_COMPLETION_TIME:
        {
            DungeonFinderCompletionTimeMessage * ptMessage = (DungeonFinderCompletionTimeMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.nChallengeRiftCompletionMilliseconds = ptMessage->nCompletionTimeMessage;
            tCGameGlobals.sChallengeRiftCompletionMillisecondsHeroName = CRefString();

            Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                tCGameGlobals.sChallengeRiftCompletionMillisecondsHeroName = ptPlayer->tAccount.usName;
            }
        }
        break;

    case SMSG_DUNGEON_FINDER_SET_TIMEDEVENT:
        {
            SNODataMessage * ptMessage = (SNODataMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.snoDungeonFinderTimedEvent = ptMessage->snoData;

            if (GameIsWeeklyChallenge())
            {
                UIGame::ChallengeRift_SetTargetTimeNotification();
            }
        }
        break;

    case SMSG_DUNGEON_FINDER_PROGRESS:
        {
            FloatDataMessage * ptMessage = (FloatDataMessage *)pMessage;
            CGameGlobals & tCGameGlobals = CGameGlobalsGet();
            tCGameGlobals.flDungeonFinderProgress = ptMessage->flData;
        }
        break;

    case SMSG_DUNGEON_FINDER_PROGRESS_GLYPH_PICK_UP:
        {
            FloatDataMessage * ptMessage = (FloatDataMessage *)pMessage;
            UIGame::FlashTieredRiftProgressBar(ptMessage->flData);
        }
        break;

    case SMSG_NEW_SKILLKIT:
        UISkills::Console::SetNewSkillKit(ptHeader->nControllerIndex);
        break;

    case SMSG_OPEN_WORLD_TRACKED_SNO:
        {
            SNODataMessage * ptMessage = (SNODataMessage *)pMessage;
            QuestsAddOpenWorldTrackedSNO(ptMessage->snoData);
        }
        break;

    case SMSG_PLAY_OPEN_WORLD_TUTORIAL:
        {
            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UIWaypointMap::Open(ptPlayer, RACTORID_INVALID, -1, TRUE);
        }
        break;

    case SMSG_HIRELING_UNLOCKED:
        {
            HirelingSwapMessage * ptMessage = (HirelingSwapMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            UIGame::ShowNewHirelingNotification(ptPlayer, ptMessage->eNewClass);
        }
        break;

    case SMSG_MULTIPLAYER_BUFF_ON:
    case SMSG_MULTIPLAYER_BUFF_OFF:
        {
            LPCSTR szLabel = ptHeader->eType == SMSG_MULTIPLAYER_BUFF_ON ? "MultiplayerBuffTurnedOn" : "MultiplayerBuffTurnedOff";
            CRefString usMultiplayerBuffMessage = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE), szLabel);

            if (ptHeader->eType == SMSG_MULTIPLAYER_BUFF_ON)
            {
                int32 nPlayers = PlayerGetNumPlayers();
                if (nPlayers == 1)
                {
                    break;
                }

                CRefString usBonusAmount = UnicodeText::FormatNumber((nPlayers - 1) * 10, TRUE, FNS_NORMAL, 0);
                usMultiplayerBuffMessage = LocalizedSprintf(usMultiplayerBuffMessage.str(), usBonusAmount.str(), NULL);
            }

            CRefString sTextureName(CRefString().Format("UI:Buff_Multiplayer"));

            for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer != NULL; ptPlayer = PlayerGetNextAll(ptPlayer))
            {
                UIEventQueue::Console::DisplayLongMessageForPlayer(usMultiplayerBuffMessage, ptPlayer, ImageTextureFrame(sTextureName), FALSE);
            }
        }
        break;

    case SMSG_CONSOLE_SAVE:
        {
            DWordDataMessage * ptSaveMessage = (DWordDataMessage *)pMessage;
            CHero::Save(ptHeader->nControllerIndex, ptSaveMessage->dwData);
        }
        break;

    case SMSG_FINISHED_GAME_SETUP_DONE:
        {
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            ptPlayer->bDidFirstTimeSetup = TRUE;

            D3::Account::SavedDefinition & tDefinition = Console::GamerProfile::GetAccount(ptHeader->nControllerIndex);
            CPlayerSetFriendStatus(ptPlayer);
            CPlayerGetNewFriendGiftRecipient(ptPlayer);
            if (!GameIsWeeklyChallenge())
            {
                Avenger::Console::InitializePlayerAvengerData(ptPlayer, tDefinition);
            }
        }
        break;

    case SMSG_CONSOLE_SYNC_HERO_SAVED_QUESTS:
        {
            PlayerSavedQuestUpdateMessage * ptUpdateMessage = (PlayerSavedQuestUpdateMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            CHero::OnSavedQuestUpdate(ptPlayer, *ptUpdateMessage);
        }
        break;

    case SMSG_CONSOLE_SYNC_HERO_QUEST_HISTORY:
        {
            PlayerQuestHistoryUpdateMessage * ptUpdateMessage = (PlayerQuestHistoryUpdateMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            CHero::OnQuestHistoryUpdate(ptPlayer, *ptUpdateMessage);
        }
        break;

    case SMSG_CONSOLE_SYNC_HERO_QUEST_REWARD_HISTORY:
        {
            PlayerQuestRewardHistoryUpdateMessage * ptUpdateMessage = (PlayerQuestRewardHistoryUpdateMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            CHero::OnQuestRewardHistoryUpdate(ptPlayer, *ptUpdateMessage);
        }
        break;

    case SMSG_CONSOLE_SYNC_HERO_SAVED_CONVERSATIONS:
        {
            PlayerSavedConversationsUpdateMessage * ptUpdateMessage = (PlayerSavedConversationsUpdateMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            CHero::OnSavedConversationUpdate(ptPlayer, *ptUpdateMessage);
        }
        break;

    case SMSG_CONSOLE_SYNC_HERO_ACTIVE_HIRELING:
        {
            IntDataMessage * ptUpdateMessage = (IntDataMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            ptPlayer->tHeroStateData.tSavedData.tHirelingSavedData.eActiveHireling = (HirelingClass)ptUpdateMessage->nData;
        }
        break;

    case SMSG_CONSOLE_SYNC_HERO_PARTITION_FLAGS:
        {
            IntDataMessage * ptUpdateMessage = (IntDataMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            ptPlayer->tAccount.uPartitionFlags = ptUpdateMessage->nData;
        }
        break;

    case SMSG_CONSOLE_SYNC_HERO_TIME_PLAYED:
        {
            DWordDataMessage * ptUpdateMessage = (DWordDataMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL && ptUpdateMessage->dwData > ptPlayer->tHeroStateData.tSavedData.uSecondsPlayed)
            {
                ptPlayer->tHeroStateData.tSavedData.uSecondsPlayed = ptUpdateMessage->dwData;
                ptPlayer->uLastPlayedTimeUpdate = dwNow;
            }
        }
        break;

    case SMSG_CONSOLE_SYNC_ACCOUNT_ACHIEVEMENT_CRITERIA_UPDATE:
        {
            D3::GameMessage::AchievementsCriteriaUpdateConsole tCriteriaUpdate;
            if (!ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tCriteriaUpdate))
            {
                DisplayInternalError(NULL, "", 7995, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            CHero::OnAchievementCriteriaUpdate(ptHeader->nControllerIndex, tCriteriaUpdate);
        }
        break;

    case SMSG_CONSOLE_COUNTER_INCREMENT:
        {
            ConsoleCounterIncrementMessage * ptIncrementMessage = (ConsoleCounterIncrementMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            CHero::OnCounterIncrement(ptPlayer, *ptIncrementMessage);
        }
        break;

    case SMSG_UPDATED_VENDOR_RANDOM_SEEDS:
        {
            GenericBlobMessage * ptMessage = (GenericBlobMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            CHero::SyncRandomSeeds(ptPlayer, ptMessage);
        }
        break;

    case SMSG_UPDATED_PARTITION_WIDE_CUBE_RANDOM_SEEDS:
        {
            GenericBlobMessage * ptMessage = (GenericBlobMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            CHero::SyncRandomCubeSeeds(ptPlayer, ptMessage);
        }
        break;

    case SMSG_CONSOLE_GAME_ENDED:
        if (ptHeader->nControllerIndex == Console::GamerProfile::GetPrimaryProfileUserIndex())
        {
            Console::Online::PartyEndGame();
        }
        break;

    case SMSG_PRELOAD_ADD_SCENE:
        {
            PreloadSceneDataMessage * ptPrefetchMessage = (PreloadSceneDataMessage *)pMessage;
            SNOPreload::AddClientScene(ptPrefetchMessage->idSScene, ptPrefetchMessage->snoScene, ptPrefetchMessage->snoLevelAreas);
        }
        break;

    case SMSG_PRELOAD_ADD_ACD:
        {
            PreloadACDDataMessage * ptPrefetchMessage = (PreloadACDDataMessage *)pMessage;
            SNOPreload::AddClientActor(ptPrefetchMessage->ann, ptPrefetchMessage->snoActor, ptPrefetchMessage->eWeaponClass, ptPrefetchMessage->gbidMonsterAffixes);
        }
        break;

    case SMSG_PRELOAD_REMOVE_SCENE:
        {
            PreloadSceneDataMessage * ptPrefetchMessage = (PreloadSceneDataMessage *)pMessage;
            SNOPreload::RemoveClientScene(ptPrefetchMessage->idSScene);
        }
        break;

    case SMSG_PRELOAD_REMOVE_ACD:
        {
            PreloadACDDataMessage * ptPrefetchMessage = (PreloadACDDataMessage *)pMessage;
            SNOPreload::RemoveClientActor(ptPrefetchMessage->ann);
        }
        break;

    case SMSG_PREFETCH_SCENE:
        {
            PrefetchDataMessage * ptPrefetchMessage = (PrefetchDataMessage *)pMessage;
            SNOPreload::PrefetchClientScene(ptPrefetchMessage->sno, FALSE);
        }
        break;

    case SMSG_PREFETCH_WORLD:
        {
            PrefetchDataMessage * ptPrefetchMessage = (PrefetchDataMessage *)pMessage;
            SNOPreload::PrefetchClientWorld(ptPrefetchMessage->sno, FALSE);
        }
        break;

    case SMSG_PREFETCH_LEVELAREA:
        {
            PrefetchDataMessage * ptPrefetchMessage = (PrefetchDataMessage *)pMessage;
            SNOPreload::PrefetchClientLevelArea(ptPrefetchMessage->sno, FALSE);
        }
        break;

    case SMSG_PREFETCH_ACTOR:
        {
            PrefetchActorDataMessage * ptPrefetchMessage = (PrefetchActorDataMessage *)pMessage;
            SNOPreload::PrefetchClientActor(ptPrefetchMessage->snoActor, ptPrefetchMessage->gbidMonsterAffixes, TRUE);
        }
        break;

    case SMSG_PLAYER_WARPING:
        Console::Screens::CloseScreenSet(TRUE, FALSE);
        CGameLoadingPartRequired(LSC_PLAYER_WARPED);
        break;

    case SMSG_DIFFICULTY_LOWERED:
        if (GameCommonDataExists())
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            GameCommonData & tGCD = CGameGetGameCommonData();
            tGCD.nHandicapLevel = ptMessage->nData;
            tGCD.dwDifficultyChangedFlags |= 1;
        }
        break;

    case SMSG_DIFFICULTY_LOWERED_FLAG:
        if (GameCommonDataExists())
        {
            GameCommonData & tGCD = CGameGetGameCommonData();
            tGCD.dwDifficultyChangedFlags |= 1;
        }
        break;

    case SMSG_DIFFICULTY_RAISED_FLAG:
        if (GameCommonDataExists())
        {
            GameCommonData & tGCD = CGameGetGameCommonData();
            tGCD.dwDifficultyChangedFlags |= 2;
        }
        break;

    case SMSG_DIFFICULTY_RAISED:
        if (GameCommonDataExists())
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            GameCommonData & tGCD = CGameGetGameCommonData();
            tGCD.nHandicapLevel = ptMessage->nData;
            tGCD.dwDifficultyChangedFlags |= 2;
        }
        break;

    case SMSG_DIFFICULTY_CHANGED:
        if (GameCommonDataExists())
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            GameCommonData & tGCD = CGameGetGameCommonData();
            tGCD.nHandicapLevel = ptMessage->nData;
        }
        break;

    case SMSG_PLAYER_GIFT_REGISTER_RECIPIENT:
        {
            PlayerGiftMessage * ptMessage = (PlayerGiftMessage *)pMessage;
            ACDID idItem = ACDFromANN(ptMessage->ann);
            ActorCommonData * ptACDItem = ACDTryToGet(idItem);
            if (ptACDItem != NULL)
            {
                if (ptACDItem->ptFriendGiftData == NULL)
                {
                    ptACDItem->ptFriendGiftData = new FriendGiftData;
                }

                if (ptMessage->nId != 0)
                {
                    ptACDItem->ptFriendGiftData->idRecipient = ptMessage->nId;
                    ptACDItem->ptFriendGiftData->sNameRecipient = ptMessage->szPlayerName;
                }
                else
                {
                    Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
                    BOOL bAvenger = ptACDItem->hGB.gbid == GlobalGBIDGet(GLOBALGBID_CONSOLE_AVENGER_GIFT);
                    PlayerRegisterFriendGift(ptPlayer, idItem, ptMessage->nOwnerIndex, bAvenger);
                }
            }
        }
        break;

    case SMSG_PLAYER_GIFT_ITEM_GENERATED:
        {
            D3::Items::Generator tItemData;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tItemData))
            {
                Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
                UIMail::Console::PlayerGiftRedeemed(ptPlayer, tItemData);
            }
        }
        break;

    case SMSG_PLAYER_GIFT_ITEM_INVENTORY_FULL:
        {
            Player * ptPlayer = PlayerGetByControllerIndex(ptHeader->nControllerIndex);
            UIMail::Console::PlayerGiftFailed(ptPlayer);
        }
        break;

    case SMSG_RIFT_POST_SCORE:
        {
            D3::Leaderboard::CompressedPostScore tCompressed;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tCompressed))
            {
                D3::Leaderboard::PostScore tScore;
                if (LeaderboardDecompressMessage(tCompressed.data(), &tScore))
                {
                    if (LeaderboardIsChallengeRift(tScore.category_id()))
                    {
                        CHero::OnChallengeRiftComplete(ptHeader->nControllerIndex, tScore.category_id(), tScore.partition_id(), tScore.score());
                    }

                    Console::Online::LeaderboardPostRiftTime(ptHeader->nControllerIndex, tScore, Console::Online::LeaderboardPostCallback());
                }
            }
        }
        break;

    case SMSG_UPSCALING_LEVEL_CHANGED:
        {
            PlayerDWordDataMessage * ptMessage = (PlayerDWordDataMessage *)pMessage;
            int32 nUpscaleLevel = ptMessage->dwData;
            Player * ptPlayer = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            CPlayerUpdateUpscaleNotification(ptPlayer, nUpscaleLevel);
        }
        break;

    case SMSG_DUNGEON_FINDER_CLOSING:
        {
            DungeonFinderClosingMessage * ptMessage = (DungeonFinderClosingMessage *)pMessage;
            CRefString sDisplay = StringListFetch("General:LootRunClosesWarning");
            BOOL bPositionWarningHigh = FALSE;

            if (ptMessage->nRiftLevel >= 0)
            {
                sDisplay = StringListFetch("General:TieredLootRunClosesWarning");
                bPositionWarningHigh = TRUE;
            }

            uint32 dwEndTime = ptMessage->nTickClosing;
            if (dwEndTime > CGameGetPredictedServerTime())
            {
                uint32 dwTimeRemaining = dwEndTime - CGameGetPredictedServerTime();
                float flSeconds = dwTimeRemaining / 60.0f;
                UICountDown::Show(flSeconds, sDisplay.str(), NULL, NULL, FALSE, bPositionWarningHigh, SNO_NONE);
                CGameGet().ptUIGlobals->dwRiftClosingTick = dwEndTime;
            }
        }
        break;

    case SMSG_SET_DUNGEON_CLOSING:
        {
            DungeonFinderClosingMessage * ptMessage = (DungeonFinderClosingMessage *)pMessage;

            CRefString sDisplay = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_SET_DUNGEON_MESSAGES), "SetDungeonClosesWarning");
            BOOL bPositionWarningHigh = FALSE;

            uint32 dwEndTime = ptMessage->nTickClosing;
            if (dwEndTime > CGameGetPredictedServerTime())
            {
                UICountDown::Show((dwEndTime - CGameGetPredictedServerTime()) / 60.0f, sDisplay.str(), NULL, NULL, FALSE, bPositionWarningHigh, SNO_NONE);
                CGameGet().ptUIGlobals->dwRiftClosingTick = dwEndTime;
            }
        }
        break;

    case SMSG_GREED_REALM_CLOSING:
        {
            DungeonFinderClosingMessage * ptMessage = (DungeonFinderClosingMessage *)pMessage;

            CRefString sDisplay = StringListFetch("General:GreedRealmClosesWarning");
            BOOL bPositionWarningHigh = FALSE;

            uint32 dwEndTime = ptMessage->nTickClosing;
            if (dwEndTime > CGameGetPredictedServerTime())
            {
                UICountDown::Show((dwEndTime - CGameGetPredictedServerTime()) / 60.0f, sDisplay.str(), NULL, NULL, FALSE, bPositionWarningHigh, SNO_NONE);
                CGameGet().ptUIGlobals->dwRiftClosingTick = dwEndTime;
            }
        }
        break;

    case SMSG_FORCE_UI_RESET:
        UI_Reset(false);
        break;

    case SMSG_CLOSE_UI_COUNTDOWN:
        if (UICountDown::IsVisible())
        {
            UICountDown::Close();
        }
        break;

    case SMSG_FORCE_HEALTH_RESYNC:
        {
            FloatDataMessage * ptMessage = (FloatDataMessage *)pMessage;

            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);

            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer != NULL)
            {
                FastAttribSetValueFloat(ptACDPlayer->idFastAttrib, ATTRIBUTE_HITPOINTS_CUR, ptMessage->flData);
            }
        }
        break;

    case SMSG_CHALLENGE_RIFT_INSPECT_ARMORRACK:
        if (GameIsWeeklyChallenge())
        {
            Console::Screens::OpenToScreen(Console::Screens::GAMEPLAY_SCREEN_SET, 0, ptHeader->nControllerIndex, FALSE);
        }
        break;

    case SMSG_CHALLENGE_RIFT_CLAIM_REWARD_CONSOLE:
        {
            DWordDataMessage * ptMessage = (DWordDataMessage *)pMessage;
            if (ptMessage->dwData == 0xffffffff)
            {
                NotificationCreationParams tParams;
                tParams.uszMessage = StringListFetch("ConsoleInventory:InventoryFull");
                tParams.nControllerIndex = ptHeader->nControllerIndex;
                Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
            }
            else
            {
                CHero::OnChallengeRiftRewardClaimed(ptHeader->nControllerIndex, ptMessage->dwData);
            }
        }
        break;

    case SMSG_CHALLENGE_RIFT_REWARD_CONSOLE:
        {
            D3::Account::ConsoleChallengeRiftReward tReward;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tReward))
            {
                CHero::OnChallengeRiftRewardEarned(ptHeader->nControllerIndex, tReward);
            }
        }
        break;

    case SMSG_EQUIPMENT_MANAGER_OPEN:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;
            ActorCommonData * ptACDNPC = ACDTryToGet(ACDFromANN(ptMessage->ann));
            if (ptACDNPC != NULL && !Console::Screens::IsScreenSetOpen())
            {
                Console::Screens::OpenToScreen(Console::Screens::EQUIPMENT_MANAGER_SCREEN_SET, 0, ptHeader->nControllerIndex, FALSE);
            }
        }
        break;

    case SMSG_LOADOUT_DATA:
    case SMSG_LOADOUT_SAVED:
        {
            PlayerLoadoutDataMessage * ptMessage = (PlayerLoadoutDataMessage *)pMessage;

            int32 nLoadoutIndex = ptMessage->nLoadoutIndex;
            if (nLoadoutIndex >= 0 && nLoadoutIndex < 10)
            {
                Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);

                LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[nLoadoutIndex];
                tLoadout.bEmpty = FALSE;
                SigmaMemoryCopy(tLoadout.tEquipment, ptMessage->tEquipment, sizeof(tLoadout.tEquipment));
                SigmaMemoryCopy(tLoadout.tActiveSkills, ptMessage->tActiveSkills, sizeof(tLoadout.tActiveSkills));
                SigmaMemoryCopy(tLoadout.snoTraits, ptMessage->snoTraits, sizeof(tLoadout.snoTraits));
                SigmaMemoryCopy(tLoadout.gbidLegendaryPowers, ptMessage->gbidLegendaryPowers, sizeof(tLoadout.gbidLegendaryPowers));
                Blizzard::String::Copy(tLoadout.usName, ptMessage->usName, sizeof(tLoadout.usName));
                tLoadout.tEquippedPotion = ptMessage->tEquippedPotion;
                tLoadout.nTabIcon = ptMessage->nTabIcon;

                UIEquipmentManager::OnPlayerLoadoutDataMessageReceived(nLoadoutIndex);
            }

            if (ptHeader->eType == SMSG_LOADOUT_SAVED)
            {
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SAVE_LOADOUT), cgtPSODefaults);
            }
        }
        break;

    case SMSG_LOADOUT_EMPTY:
        {
            PlayerLoadoutTabIconMessage * ptMessage = (PlayerLoadoutTabIconMessage *)pMessage;

            int32 nLoadoutIndex = ptMessage->nLoadoutIndex;
            if (nLoadoutIndex >= 0 && nLoadoutIndex < 10)
            {
                Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);

                LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[nLoadoutIndex];
                tLoadout.Initialize();

                tLoadout.nTabIcon = ptMessage->nTabIcon;
            }
        }
        break;

    case SMSG_LOADOUT_UPDATE:
        {
            PlayerLoadoutEquipmentUpdateMessage * ptMessage = (PlayerLoadoutEquipmentUpdateMessage *)pMessage;

            int32 nLoadoutIndex = ptMessage->nLoadoutIndex;
            int32 nEquipmentIndex = ptMessage->nEquipmentIndex;
            if (nLoadoutIndex >= 0 && nLoadoutIndex < 10 && nEquipmentIndex >= 0 && nEquipmentIndex < 13)
            {
                Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);

                LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[nLoadoutIndex];
                tLoadout.tEquipment[nEquipmentIndex] = ptMessage->tNewItem;
            }
        }
        break;

    case SMSG_AMIIBO_SCAN_BEGIN:
        {
            ACDLookAtMessage * ptMessage = (ACDLookAtMessage *)pMessage;
            ACDID idACDPlayer = ACDFromANN(ptMessage->annSrc);

            ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
            if (ptACDPlayer != NULL && ptACDPlayer->GetHasNFPDevice())
            {
                Console::Screens::CloseAllScreenSets(FALSE);

                if (!Console::NFP::IsPlayerScanDesired(idACDPlayer) && Console::NFP::IsAnyPlayerScanDesired())
                {
                    Console::NFP::ClearAllPlayerScansDesired();
                }

                ACDNetworkName annScanningPortal = ptMessage->annTarget;
                Console::NFP::SetPlayerScanDesired(idACDPlayer, true, annScanningPortal);

                Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
                SNO snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_AMIIBO_USAGE);
                if (!Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial))
                {
                    Console::Notifications::ShowAmiiboTutorial(ptHeader->nControllerIndex);
                    Tutorial::Shown(*ptPlayer, GlobalSNOGet(GLOBALSNO_TUTORIAL_AMIIBO_USAGE));
                }
            }
        }
        break;

    case SMSG_AMIIBO_SCAN_END:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;
            ACDID idACDPlayer = ACDFromANN(ptMessage->ann);
            if (idACDPlayer != ACDID_INVALID && Console::NFP::IsPlayerScanDesired(idACDPlayer))
            {
                Console::NFP::SetPlayerScanDesired(idACDPlayer, false, ANN_INVALID);
            }
        }
        break;

    case SMSG_SYNC_AMIIBO_SCAN_TIME:
        {
            PlayerDWordDataMessage * ptMessage = (PlayerDWordDataMessage *)pMessage;
            Player * ptPlayer = PlayerGetByControllerIndexAlways(ptHeader->nControllerIndex);
            if (ptPlayer != NULL)
            {
                Blizzard::Time::UnixTime tScanTime = ptMessage->nPlayerIndex;
                Console::NFP::NFPScanResult eScanResult = (Console::NFP::NFPScanResult)ptMessage->dwData;

                D3::Account::SavedDefinition & tSavedDefinition = Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
                D3::Account::ConsoleData * tConsoleData = tSavedDefinition.mutable_console_data();

                switch (eScanResult)
                {
                case 0:
                    ptPlayer->tNFPDefaultScanTime = Blizzard::Time::FromUnixTime(tScanTime);
                    tConsoleData->set_nfp_default_time(tScanTime);
                    break;

                case 1:
                    ptPlayer->tNFPGoblinScanTime = Blizzard::Time::FromUnixTime(tScanTime);
                    tConsoleData->set_nfp_goblin_time(tScanTime);
                    break;
                }
            }
        }
        break;

    default:
        break;
    }

    return TRUE;
}

//@ 8767
uint32 ClientMessageReceiveGetServerProtocolHash()
{
    return s_uServerProtocolHash;
}
