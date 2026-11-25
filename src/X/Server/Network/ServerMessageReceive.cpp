// Original file: X/Server/Network/ServerMessageReceive.cpp
// Line numbers in comments are the original ones (DWARF line table).

#include "../../Common/CommonTypes.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Player.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/OnlineService/Callback/Callback.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/Console/Networking/Connection.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/XML/SAXHelper.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../Console/ConsolePlayerResolver.h"
#include "../SGame.h"
#include "XServerSocket.h"
// >>> EXTRA INCLUDES
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Nav/NavCellPath.h"
#include "../SWorld.h"
#include "../../Common/Definitions/ConversationDefinition.h"
#include "../SNPC.h"
#include "../../Client/UserInterface/Interface/UILoreCommon.h"
#include "../AI/BrainStance.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../Powers/Powers.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/GizmoSwig.h"
#include "../SPlayer.h"
#include "../Movement/SActorMovement.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../Actors/Actors.h"
#include "../../Common/NephalemRiftCommon.h"
#include "../../Common/NPC.h"
#include "../../Common/HirelingSwig.h"
#include "../../Common/OnlineService/Achievements.h"
#include "../PlayerList.h"
#include "../../Common/Crafting/Salvage.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Items.h"
#include "../SGameManager.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/Crafting/ItemCraftingSwig.h"
#include "../Powers/PowerScript.h"
#include "../../Common/Protobuf/GameMessage.pb.h"
#include "../../Common/Protobuf/Items.pb.h"
#include "../../Common/Protobuf/Account.pb.h"
// <<< EXTRA INCLUDES

// The NX SDK's <math.h> (musl) classifies a float by testing its bits; the build container's libc expands
// isnan() to a compiler builtin. Same workaround as isfinite() in Sigma/DataStructures/DataStructuresExternal.h
// (which supplies __FLOAT_BITS), until the SDK libc headers are available to the build.
#ifdef D3_NX_FLOAT_BITS_DEFINED
#undef isnan
#define isnan(x) ((__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000)
#endif

// ---- declarations whose headers are not in src yet ---------------------------------------
// X/Common/Networking/GameMessage.h (GameMessage.cpp:1178)
BOOL GetGameMessageTypeAndSize(GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor, uint32 & dwSize, const char *& pszMessageLabel);
// X/Server/Network/Server.h (Server.cpp:917, 2171)
void ServerSendOutOfGameMessage(ServerSocketID idServerSocket, void * pMessage, GameMessageType eType, uint32 dwSize);
ServerSocketID ServerGetSocketIDFromGameConnectionID(GameConnectionID idGameConnection);
// X/Server/Network/XServerSocket.h (XServerSocket.cpp:198, 1351, 1473)
BOOL XServerSocketIDValid(ServerSocketID idServerSocket);
uint32 XServerSocketGetMsgPermissions(ServerSocketID idServerSocket);
Console::GameNetwork::ConnectionRef XServerSocketGetGameConnection(ServerSocketID idServerSocket);
// X/Server/SGame.h (SGame.cpp:5747)
BOOL SGameValidateJoinRequest(ServerSocketID idServerSocket, SGameID idSGame, OnlineService::HeroId uHeroId, uint64 uAuthToken, XLocale eLocale);
// X/Server/Replay.h (Replay.cpp:693)
void ReplayServerReceiveMessage(int32 nPlayerIndex, void * pMessage);
// X/Server/Network/ClientValidation.h (ClientValidation.cpp:262)
BOOL CValidateSpamCheck(Player & tPlayer, const GameMessageType eMessageType, void * pMessage);
// X/Common/Console/ConsoleGameMessage.h (ConsoleGameMessage.cpp:26)
namespace Console { namespace Server { BOOL MessageRecieve(int32 nPlayerIndex, void * pMessage); } }

// X/Server/SPlayer.h (SPlayer.cpp:7750, 8209)
BOOL SPlayerIsLoggingOut(BOOL includeGracePeriod, Player & tPlayer);
void SPlayerRemoveInvulBuff(ActorCommonData & tACDPlayer);
// X/Server/Network/XServerSocket.h (XServerSocket.cpp:1454)
BOOL XServerSocketMarkMeaningfulMessage(ServerSocketID idServerSocket);

// Prototypes of the callees of the written cases; each line is the prototype the DWARF records for the
// definition (generated once by build/try/agentE/gen_decls.py; the comment names the defining file).
// X/Common/ActorInventory.cpp
BOOL SACDInventoryBuyStashSlots(const ACDID idACDPlayerActor);
// X/Common/Networking/GameMessage.cpp
BOOL ParseProtocolBufferMessage(GameMessageType eType, uint8 * pMessageBuffer, google::protobuf::MessageLite & tMessage);
// X/Common/Player.cpp
BOOL PlayersAreLocalToEachOther(const Player & tPlayer0, const Player & tPlayer1);
// X/Common/Tutorials.cpp
namespace Tutorial {
void Shown(Player & tPlayer, const SNO snoTutorial);
}  // namespace Tutorial
// X/Server/ConversationMgr.cpp
namespace ConversationMgr {
void AdvanceConversationForPlayer(const Player & tPlayer, SNO snoConv, int32 nLineUID);
void UpdateConversationAutoAdvanceForPlayer(const Player & tPlayer, SNO snoConv, int32 nLineUID, uint32 dwNextAdvanceTime);
void CancelConversationForPlayer(const Player & tPlayer);
}  // namespace ConversationMgr
// X/Server/Crafting/SItemCrafting.cpp
void SItemCrafting_TryRepairAll(Player & tPlayer, const BOOL fEquippedOnly);
// X/Server/ItemMail.cpp
void ItemMailHandleMailOperation(Player & tPlayerFrom, const D3::GameMessage::MailOperation & tOperationMessage);
// X/Server/Loot/Loot.cpp
void LootGenerateGift(const Player & tPlayer, ItemQualityLevel eIQL);
// X/Server/Network/ClientValidation.cpp
BOOL CValidatePlayerNearStash(const Player & tPlayer);
BOOL CValidatePlayerNearTradeNPC(const Player & tPlayer);
BOOL CValidateSNO(const SNOGroup snoGroup, const SNO sno);
BOOL CValidateEndGameRequest(const SNO snoSentinel, const Player & tPlayer);
BOOL CValidateActTransitionRequest(const SNO snoSentinel, const Player & tPlayer);
// X/Server/Network/ServerMessageReceive.cpp
// X/Server/SActTransitions.cpp
void SActTransitionStart(Player * ptPlayer, BOOL bRecreateGameWithParty);
// X/Server/SCosmeticItems.cpp
BOOL SCosmeticItems_EquipCosmeticItem(Player & tPlayer, const GBID gbidItem);
BOOL SCosmeticItems_UnequipCosmeticItem(Player & tPlayer, CosmeticItemType eType);
// X/Server/SGame.cpp
void SGameFinishPlayerSetup(Player & tNewPlayer);
void SGameStartGameEnd(Player & tPlayer);
void SGameHandlePauseRequest(Player & tPlayer, BOOL bPause);
void SGameHandleSoftPauseRequest(Player & tPlayer, BOOL bPause);
// X/Server/SHireling.cpp
void SHirelingHandleSwap(Player & tPlayer);
void SHireling_TryRetrain(Player & tPlayer);
// X/Server/SNPC.cpp
void NPCVendorStop(Player & tPlayer);
// X/Server/SPlayer.cpp
void SPlayerStartLogoutTimer(int32 nPlayerIndex, BOOL bIssueKickWarning, LogoutContext eContext);
void SPlayerAttemptLogout(int32 nPlayerIndex);
void SPlayerCancelLogoutTimer(int32 nPlayerIndex, LogoutContext eLogoutContext);
void SPlayerInspect(ACDID idACDPlayer, ACDID idACDToInspect);
void SPlayerInspectEnd(ACDID idACDPlayer);
void SPlayerReceivePing(Player & tPlayer);
void SPlayer_ClientLoadComplete(Player & tPlayerFrom);
BOOL SPlayerPurchaseParagonPointsReset(Player & tPlayer, int32 nCategory);
void SPlayerAddMailItemToInventory(const Player & tPlayerFrom, D3::Items::Generator & tGenerator, BOOL bEquip);
void SPlayerSetHasFriends(const Player & tPlayer, BOOL bHasFriends);
void SPlayerRetrieveSeasonalStashItemOrChallengeRiftReward(const Player & tPlayerFrom, D3::Items::Generator & tGenerator, InventorySlotType eInvSlot);
void PlayerSendHeroStateLoadoutToClient(Player & tPlayer, int32 nLoadoutIndex, BOOL bOnSave);
void SPlayerRedeemSeasonGift(Player & tPlayer, GBID gbidItem);
// X/Server/STalk.cpp
void STalkResetTalkingTo(Player & tPlayer);
// X/Server/STrade.cpp
void STradeHandleMessage(Player & tPlayerFrom, TradeMessage & tMessage);
// X/Server/System/SNephalemRift.cpp
namespace NephalemRiftManager {
void PlayerDeclineStart(Player * ptPlayer);
void PlayerDeclineInvite(Player * ptPlayer);
void PlayerAcceptInvite(Player * ptPlayer, BOOL bEmpowered);
void PlayerRequestedJoinInfo(Player * ptPlayer);
void PlayerVoteContinue(Player * ptPlayer, BOOL bEmpowered);
void PlayerVoteCancel(Player * ptPlayer);
void PlayerPaymentWarningAccepted(Player * ptPlayer, BOOL bEnterPortal);
}  // namespace NephalemRiftManager
// X/Server/System/SVoteKick.cpp
namespace VoteKick { namespace Server {
void VoteYes(Player * ptPlayerVoting);
void VoteNo(Player * ptPlayerVoting);
}}  // namespace VoteKick::Server
// X/Server/System/SetEncounter.cpp
namespace SetEncounterManager {
void PlayerVoteCancel(Player * ptPlayer);
void PlayerAcceptStart(Player * ptPlayer);
}  // namespace SetEncounterManager

namespace Avenger { namespace Console {
void SaveAccountInfoToAvengerData(Player * ptPlayer, const D3::Account::AvengerData & tAvengerAccountData);
void SaveAvengerToPlayer(const D3::Account::Avenger & tAvenger, PlayerAvengerData * ptPlayerAvengerData, AvengerSource eSource);
}}  // namespace Avenger::Console (X/Common/Console/Avenger.cpp)

// X/Common/ActorCommonData.cpp
ACDID ACDFromANN(const ACDNetworkName ann);
// X/Common/ActorInventory.cpp
BOOL SACDInventorySellStackPortion(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const uint64 uAmount);
void SACDInventoryProcessDropToPortraitRequest(ACDID idACDPlayer, ACDID idACDItem);
BOOL SACDInventoryBuy(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem);
BOOL SACDInventorySell(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem);
// X/Common/Player.cpp
ACDID PlayerGetHireling(ACDID idACDPlayer);
uint32 PlayerCalculatePlayerCountOnConnection(const GameConnectionID & tGameConnection);
// X/Server/AI/PlayerBrain.cpp
void PlayerBrainRequestAbortChannelled(ACDID idACDPlayer, SNO snoPower);
// X/Server/Network/ClientValidation.cpp
BOOL CValidateItemIsInPlayerInventory(const Player & tPlayer, const ACDID idACDItem, BOOL bExcludeHirelingSlots);
BOOL CValidateANN(const ACDNetworkName ann);
BOOL CValidatePlayerNearCrafter(const Player & tPlayer, const CrafterType eCrafterType);
BOOL CValidateItemCanBeSold(const Player & tPlayer, const ACDID idACDItem);
// X/Server/Network/Server.cpp
void ServerPlayerLeftOnConnection(GameConnectionID idGameConnection, const Player & tPlayer);
// X/Server/Powers/Powers.cpp
void PowerAbortLooping(ACDID idACDPlayer, SNO snoPower);
void PowerAbortClientWalk(ACDID idACDPlayer, SNO snoPower);
// X/Server/SGame.cpp
void SGamePause(BOOL bPaused);
// X/Server/SPlayer.cpp
void SPlayerDelete(Player & tPlayer);
// X/Server/System/BossEncounter.cpp
namespace BossEncounterManager {
void PlayerDeclineStart(Player * ptPlayer);
void PlayerDeclineInvite(Player * ptPlayer);
void PlayerAcceptInvite(Player * ptPlayer);
void PlayerAcceptStart(Player * ptPlayer);
void PlayerVoteContinue(Player * ptPlayer);
void PlayerVoteCancel(Player * ptPlayer);
}  // namespace BossEncounterManager
// X/Server/System/SetEncounter.cpp
namespace SetEncounterManager {
void PlayerDeclineInvite(Player * ptPlayer);
void PlayerAcceptInvite(Player * ptPlayer);
void PlayerVoteContinue(Player * ptPlayer);
}  // namespace SetEncounterManager

// X/Common/Crafting/ItemCrafting.cpp
int32 GetCraftLevel(const CrafterType eType, const Player & tPlayer);
// X/Common/Player.cpp
BOOL PlayerCanChangeParagon(const ActorCommonData & tACDPlayer);
// X/Common/SNO/SNOFiles.cpp
SNO SNOFileLookupGUID(const SNOGroup snoGroup, LPCSTR szPath);
// X/Server/Crafting/SItemCrafting.cpp
void SItemCrafting_TryTrain(Player & tPlayer);
void SItemCrafting_Reforge(ACDID idACDItem, Player & tPlayer);
// X/Server/Network/ClientValidation.cpp
BOOL CValidatePlayerIndex(const int32 nPlayerIndex);
BOOL CValidateUniString(const CHAR * puzString, const int32 nMaxLength);
// X/Server/Powers/PowerAPI.cpp
void PowerAPI_BuffFlagAllFromPowerForDeletion(ActorCommonData & tACD, SNO snoPower, int32 nBuffIndex, BuffRemovalReason eRemovalReason, ACDID idActorSource);
BOOL PowerAPI_BuffRequestCancel(const Player & tPlayerRequester, const SNO snoPower, const int32 nBuffIndex);
// X/Server/SPlayer.cpp
void SPlayerResetParagonPoints(Player & tPlayer, int32 nCategory);
void SPlayerSpendParagonPoints(Player & tPlayer, GBID gbidParagonBonus, int32 nPoints);
// X/Server/System/SVoteKick.cpp
namespace VoteKick { namespace Server {
void KickRequested(const Player & tPlayerAsking, const Player * ptTarget);
void TryVoteKick(VoteKickMessage * ptMessage);
}}  // namespace VoteKick::Server

// X/Common/Game.cpp
BOOL GameIsSeasonal();
BOOL GameCommonDataExists();
// X/Common/GameCommonData.cpp
BOOL GameCommonPlayerIsPartyGuide(const Player & tPlayer);
// X/Common/NephalemRiftCommon.cpp
namespace NephalemRift {
BOOL IsTieredRiftActive();
}  // namespace NephalemRift
// X/Common/Player.cpp
BOOL PlayerCanManipulateInventory(const ActorCommonData & tACDPlayer);
Player * PlayerGetByACD(const ACDID idACD);
BOOL PlayerCanChangeSkillorTrait(const ActorCommonData & tACDPlayer, BOOL bForHotbarSwapOnly);
// X/Common/Powers/PowersCommon.cpp
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower);
// X/Server/Loot/Loot.cpp
BOOL LootDropCraftingMaterial(const ActorCommonData * ptACDPlayer, const CurrencyType eCurrency, uint64 uAmount);
// X/Server/SGame.cpp
void SGameSetPartyGuideAccount(const Player & tPlayer);
void SGameUpdateDifficultyLevel(Player & tRequestingPlayer, int32 nNewDifficultyLevel);
// X/Server/SPlayer.cpp
SNO SPlayerGetActiveSkillInSlot(const Player & tPlayer, const int32 nSlot);
void SPlayerRemoveActiveSkillInSlot(Player & tPlayer, const int32 nSlot);

// X/Common/ChallengeRift.cpp
namespace ChallengeRift {
BOOL IsValidChallengeRiftSkill(SNO snoSkill);
RuneType GetRuneTypeForSkill(SNO snoSkill);
}  // namespace ChallengeRift
// X/Common/Definitions/GameBalanceDefinition.cpp
BOOL GBIDExists(GBID gbid, GameBalanceType eGBType);
// X/Common/GameCommonData.cpp
BOOL GameIsWeeklyChallenge();
// X/Common/Items.cpp
SNO ItemGetLegendaryPower(GBID gbidItem);
BOOL ItemPlayerHasExtractedLegendaryPower(const Player & tPlayer, GBID gbidItem);
BOOL ItemPlayerCanAssignLegendaryPower(const Player & tPlayer, GBID gbidItem, LegendaryPowerSlotType eSlotType);
// X/Common/Player.cpp
int32 PlayerGetNumAvailableSkillSlots(const ActorCommonData & tACDPlayer);
// X/Common/Powers/PowersCommon.cpp
BOOL PowerIsValidForSlot(AutoPowerDefinitionR pdefPower, const int32 nSlot);
BOOL PowerCanActorTrainActiveSkill(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
BOOL PowerActorMeetsSkillRuneRequirement(const ActorCommonData & tACD, SNO snoPower, const RuneType eRuneType);
// X/Server/SHireling.cpp
BOOL SHirelingTryChooseSkill(ActorCommonData & tACDHireling, SNO snoPower);
// X/Server/SPlayer.cpp
void SPlayerSwitchActiveSkills(Player & tPlayer, const SNO snoPower, const RuneType eRuneType, const int32 nSlot, const BOOL bApplyCooldown);
void SPlayerAssignToLegendaryPowerSlot(Player & tPlayer, const GBID gbidLegendaryItem, const int32 nSlot);

// X/Common/ActorInventory.cpp
void SACDInventoryDeleteItem(ActorCommonData & tACDItem, uint64 uCount, BOOL bMarkAsDirty, BOOL bFlagForDeletion);
void SACDInventoryRegisterItemGiftRecipient(ActorCommonData & tACDItem, uint64 nId, char * szPlayerName);
// X/Common/NPC.cpp
BOOL NPCIsEquipmentManagerVendor(const ActorCommonData & tACD);

// X/Common/Player.cpp
BOOL PlayerCanChangeLoadoutSkillsAndTraits(const Player & tPlayer, int32 nLoadoutIndex);
BOOL PlayerCanChangeLoadoutEquipment(const Player & tPlayer, int32 nLoadoutIndex);
// X/Server/SPlayer.cpp
void SPlayerLoadout_Equip(Player & tPlayer, int32 nLoadoutIndex);
void SPlayerLoadout_Save(Player & tPlayer, int32 nLoadoutIndex, LPCSTR szName);

// X/Common/Crafting/ItemCrafting.cpp
int32 CrafterGetMaxLevel(const CrafterType eType);
// X/Common/GameTemplate.cpp
BOOL GameIsAdventureMode();
// X/Common/NPC.cpp
BOOL NPCCanIdentify(const ActorCommonData & tACD);
CrafterType NPCGetCrafterType(const ActorCommonData & tACDVendor);
// X/Common/Networking/GameMessage.cpp
void MessageSendToClient(const PlayerList & listDestinations, GameMessageType eType, void * pMessage, uint32 dwMessageSize);
// X/Server/Crafting/SItemCrafting.cpp
void SItemCrafting_RemoveGems(ActorCommonData & tACDItem, Player & tPlayer);
BOOL SItemCrafting_TryRepair(ACDID idACDItem, Player & tPlayer);
// X/Server/Crafting/SSalvage.cpp
namespace SSalvage {
BOOL TrySalvage(Player & tPlayer, ActorCommonData & tACDItem);
void TrySalvageAll(Player & tPlayer, SalvageAllType eSalvageType);
}  // namespace SSalvage
// X/Server/Network/ClientValidation.cpp
BOOL CValidateItemIsInPlayerStash(const Player & tPlayer, const ACDID idACDItem);
// X/Server/SPlayer.cpp
BOOL SPlayerTryIdentifyAllWithCast(ACDID idACD, ACDID idGizmoTarget);

// X/Common/Player.cpp
bool PlayerCanPlayHandicap(int32 nLevel, int32 nHighestRiftLevel, X1HandicapLevel eHandicap, bool bPublicGame);
// X/Server/LuaScript/ScriptEvent.cpp
BOOL ScriptEventOnScriptedSequenceSkipped(ACDID idACDPlayer);
// X/Server/SPlayer.cpp
BOOL SPlayerInScriptedSequence(const Player & tPlayer);

// X/Common/Items.cpp
GBID ItemGetGBIDForDyeType(DyeType eDyeType);
// X/Server/Network/ClientValidation.cpp
BOOL CValidatePlayerHasPromoDyes(const Player & tPlayer);
// X/Server/SPlayer.cpp
void SPlayerSendBanner(const PlayerList & listDestPlayers, const Player & tPlayer);

// X/Common/ActorCommonData.cpp
HirelingClass ACDGetHirelingClass(const ActorCommonData & tACD);
// X/Common/Hireling.cpp
BOOL HirelingTestTypeAvailable(ACDID idACDPlayer, HirelingClass eClass);
// X/Common/NPC.cpp
BOOL NPCCanOperate(const ActorCommonData & tACDNPC, const ActorCommonData & tACDOperator, BOOL bFromDirectClick);
BOOL NPCCanTrade(const ActorCommonData & tACD);
// X/Common/Player.cpp
Player * ACDGetControllingPlayer(const ACDID idACD);
BOOL PlayerPetIsDismissableByPlayer(const ActorCommonData & tACDPet);
// X/Server/Network/ClientValidation.cpp
BOOL CValidatePlayerNearInteractNPC(const Player & tPlayer);
// X/Server/SHireling.cpp
BOOL SHirelingCanHire(const Player & tPlayer);
ACDID SHirelingCreate(ActorCommonData & tACDPlayer, ActorCommonData & tACDNextTo, const HirelingClass eHirelingClass, BOOL bForceCreate, HirelingType eType);
// X/Server/SNPC.cpp
void NPCOperate(ActorCommonData & tACDNPC, ActorCommonData & tACDPlayer, const NPCMenu ePreferredMenu, const BOOL fFromPower, const BOOL fSkipImportantConversations, const BOOL fForceOperateVendor);
void DismissPet(ActorCommonData & tACDPlayer, ActorCommonData & tACDPet, BOOL bPlayerDismissed);
void NPCOperateVendor(ActorCommonData & tACD, ActorCommonData & tACDOperator);

// X/Common/ActorInventory.cpp
DataID SACDInventoryAddToDirtyList(const ActorCommonData & tACDItem, ActorInventoryDatabaseAction eAction, InventorySlotType eCameFrom, InventorySlotType eDestination);
// X/Common/Items.cpp
BOOL ItemIsUnidentified(const ActorCommonData & tACDItem);
ItemEnchantAffixError ItemCanEnchantAffix(const ActorCommonData & tACDItem);
BOOL ItemHasEnchantedAffix(const ActorCommonData & tACDItem);
GBID ItemGetEnchantedAffixOld(const ActorCommonData & tACDItem);
BOOL ItemIsUpgradeableJewel(const ActorCommonData & tACDItem);
// X/Server/Crafting/SItemCrafting.cpp
void SItemCrafting_EnchantItemAffixStart(ACDID idACDItem, GBID gbidAffixToReroll, Player & tPlayer);
void SItemCrafting_EnchantItemAffixChoose(ACDID idACDItem, int32 nAffixChoice, Player & tPlayer);
void SItemCrafting_TryTransmog(ACDID idACDItem, GBID gbidTransmog, Player & tPlayer);
void SItemCrafting_TryDyeItem(ACDID idACDItem, DyeType eDyeType, Player & tPlayer);
struct TransmuteItemsMessage;
void SItemCrafting_TryTransmute(TransmuteItemsMessage * ptMsg, Player & tPlayer);
void SItemCrafting_UpgradeJewel(Player & tPlayer, ActorCommonData & tACDItem);
// X/Server/SItems.cpp
BOOL SItemPlayerIdentify(ActorCommonData & tACDItem, ActorCommonData * ptACDPlayer);

// X/Common/Crafting/ItemCrafting.cpp
BOOL ItemCrafting_PlayerKnowsRecipe(const Player & tPlayer, GBID gbidRecipe);
// X/Common/GlobalSNO.cpp
SNO GlobalSNOGet(const GlobalSNO eGlobalSNO);
// X/Common/NephalemRiftCommon.cpp
namespace NephalemRift {
uint64 GetEmpoweredRiftCost(int32 nRiftLevel);
}  // namespace NephalemRift
// X/Server/Actors/Actors.cpp
Actor * ActorTryToGet(ActorID idActor);
// X/Server/Crafting/SItemCrafting.cpp
ACDID SItemCrafting_TryCraftItem(Player & tPlayer, GBID gbidRecipe);
// X/Server/Network/ClientValidation.cpp
BOOL CValidateGBID(const GameBalanceType eType, const GBID gbid);
// X/Server/System/SNephalemRift.cpp
namespace NephalemRiftManager {
void PlayerAcceptStart(int32 nRiftTier, Player * ptPlayer, BOOL bEmpowered);
}  // namespace NephalemRiftManager
// X/Server/System/SetEncounter.cpp
namespace SetEncounterManager {
BOOL IsWaitingForPlayerToVote(Player * ptPlayer);
}  // namespace SetEncounterManager

// X/Server/AI/BrainComponents.cpp
BOOL DeleteLastHeadstone(ActorCommonData & tACDVictim);
// X/Server/Actors/Actors.cpp
void ActorSnapToPosition(const ActorID idActor, const WorldPlace & tPlace, BOOL bSnapToGround, BOOL bSendToClient);
// X/Server/SPlayer.cpp
void SPlayerResetInactionTime(const ACDID idACDPlayer);

// X/Common/Items.cpp
BOOL ItemHasLabel(const GBID gbidItem, const ItemLabel eItemLabel);
BOOL ItemCanBeAssignedToUsableItemButton(const ActorCommonData & tACDItem);
// X/Common/Player.cpp
void PlayerSetPotionBtnAssignment(Player * ptPlayer, const HotbarButtonData & tButtonData);
// X/Server/LuaScript/ScriptEvent.cpp
void ScriptEventOnUserEvent(LPCSTR szUserEvent, ACDID idACD, int32 nUserParam);
// X/Server/Network/ClientValidation.cpp
BOOL CValidateGBIDAllowNone(const GameBalanceType eType, const GBID gbid);
// X/Server/SPlayer.cpp
void SPlayerAddToSaveList(Player & tPlayer, uint32 dwHeroSaveOptions, PlayerSaveReason reason, SPlayerSaveMode eSaveMode);

// X/Common/ActorInventory.cpp
void SACDInventoryProcessGoldTransfer(ActorCommonData & tACDPlayerSrc, ActorCommonData & tACDPlayerDest, const uint64 uAmount);
// X/Common/Player.cpp
BOOL ACDIsPlayer(const ACDID idACD);
// X/Server/AI/BrainComponents.cpp
ACDID GetLastHeadstoneACD(const ActorCommonData & tACDVictim);
// X/Server/Network/ClientValidation.cpp
BOOL CValidatePlayerNearGizmo(const Player & tPlayer, const ActorCommonData & tACDGizmo);
// X/Server/SGizmo.cpp
void GizmoOperate(ActorCommonData & tACDGizmo, ActorCommonData * ptACDOperator, BOOL bForceOperation, BOOL bAlreadyConfirmed);

// X/Common/ActorInventory.cpp
BOOL SACDInventoryProcessDropRequest(const ACDID idACDWantingToDrop, const ACDID idACDItem, const BOOL bCheckAlive);
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);
GameError ACDInventoryItemAllowedInSlot(const ActorCommonData & tACDItem, const InventoryLocation & tInvLoc, const BOOL fSkipRequirements, const BOOL fSwapping);
// X/Common/Items.cpp
BOOL ItemIsSocketed(const ActorCommonData & tACDItem);
GameError ItemCanDrop(const ActorCommonData & tACDItem);
// X/Common/Player.cpp
void PlayerDisplayErrorMessage(ACDID idACDPlayer, GameError eError);
// X/Common/Trade.cpp
BOOL TradeIsTrading(const ACDID idACDPlayer);
// X/Server/AI/PlayerBrain.cpp
BOOL PlayerBrainTryInventoryMoveItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest);
// X/Server/Network/ClientValidation.cpp
BOOL CValidateIsInventoryLocationValidToPlayer(const Player & tPlayer, const InvLoc & tLocation);

// X/Common/ActorInventory.cpp
BOOL SACDInventoryProcessDropStackPortionRequest(const ACDID idACDWantingToDrop, ActorCommonData & tACDItem, const uint64 uAmount);
ACDID ACDInventoryGetItemInSlot(const InventoryLocation & tInvLoc);
void SACDInventoryProcessSplitStack(ActorCommonData & tACDItem, const uint64 uAmount, const InventoryLocation & tInvLoc);
// X/Server/STrade.cpp
void STradeCheckForModified(const ActorCommonData & tACDItem, BOOL bRemoveItem);

// X/Common/Player.cpp
BOOL PlayerCanReviveAtCheckpoint(const ActorCommonData & tACDPlayer);
BOOL PlayerCanReviveAtCorpse(const ActorCommonData & tACDPlayer);
// X/Server/AI/BrainComponents.cpp
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce);
BOOL BrainAPIResurrectByType(ACDID idACD, RevivalType eRevivalType);
// X/Server/Network/ServerMessageSend.cpp
void ServerMessageSendAcceptedDeathAck(const ACDID idACDPlayer);
// X/Server/Powers/PowersSwig.cpp
void PowerReduceAllCooldowns(ACDID idActor, float flReduction);
// X/Server/SPlayer.cpp
BOOL SPlayerIsInRift(const ActorCommonData & tACDPlayer);
// X/Server/STimedEvents.cpp
void STimedEventAddArtificiallyElapsedTime(const SNO snoTimedEvent, int32 nTicks);
// X/Server/System/SNephalemRift.cpp
namespace NephalemRiftManager {
int32 RiftPlayerCount();
BOOL PlayerPaidForRift(const Player * ptPlayer);
void AddArtificiallyElapsedTime(int32 nTicks);
}  // namespace NephalemRiftManager

// X/Common/Attributes/FastAttribute.cpp
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);
// X/Common/Powers/PowersCommon.cpp
BOOL PowerTargetsContactPlaneOnly(AutoPowerDefinitionR pdefPower);
BOOL PowerIsComboPower(SNO snoPower);
BOOL PowerIsGizmo(AutoPowerDefinitionR pdefPower);
BOOL PowerIsTargetWithinValidZRange(const ACDID idACDPlayer, const Target & tTarget);
BOOL PowerChecksPathAndCliptoPathableAttackRadius(SNO snoPower);
BOOL PowerClipsTargetToAttackRadius(AutoPowerDefinitionR pdefPower);
BOOL PowerRequiresTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
PowerTemplate PowerGetTemplate(AutoPowerDefinitionR pdefPower, ACDID idACDOwner);
BOOL PowerIsPrimary(SNO snoPower, ACDID idACD);
SNO PowerConsumedItem(AutoPowerDefinitionR pdefPower);
// X/Server/AI/PlayerBrain.cpp
void PlayerBrainTarget(const ACDID idACDPlayer, const Target & tTarget, const SNO snoPower, const ACDID idACDItemBeingUsed, const PreplayedAnimData & tPreplayedAnimData, const int32 nComboLevel, const uint32 dwTargetMessageFlags);
// X/Server/Actors/ActorPowers.cpp
PowerID ActorPowersGetPrimary(ActorID idActor);
// X/Server/Network/ClientValidation.cpp
BOOL CValidateTargetType(const TargetType eType);
BOOL CValidateTargetPlace(const WorldPlace & tWorldPlace, const ActorCommonData & tACDCaster);
BOOL CValidateANNAllowInvalid(const ACDNetworkName ann);
BOOL CValidateANNInWorldAllowInvalid(const ACDNetworkName ann);
BOOL CValidatePowerUsableByPlayer(const ActorCommonData & tACD, const SNO snoPower);
BOOL CValidatePreplayAnimDataFlags(ActorCommonData & tACD, const PreplayedAnimData & tAnimData, const SNO snoPower);
// X/Server/Powers/PowerHelper.cpp
void PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(const ActorCommonData * ptACDPlayer, const SNO snoPower, WorldPlace & wpTarget, const BOOL bFromPlayersFeet);
struct TargetMessage;
void PowerHelper_CheckPathAndClipToPathableAttackRadius(const ACDID idACDPlayer, TargetMessage * ptTargetMsg);
void PowerHelper_ClipTargetMessageToPowerAttackRadius(const ACDID idACDPlayer, TargetMessage * ptTargetMsg);
// X/Server/Powers/Powers.cpp
Power * PowerTryToGet(PowerID idPower);
// X/Server/SPlayer.cpp

// X/Common/AnimPlayer.cpp
void AnimPlayerPlaySecondaryAnim(ActorCommonData & tACD, uint32 uAnimTag, PreplayedAnimData & tPreplayedAnimData, SNO snoPower);
// X/Common/Powers/PowersCommon.cpp
BOOL PowerRequiresActorTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
BOOL PowerCastTargetNecroCorpsesOnly(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);
uint32 PowerACDCanStartPowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIsAI, BOOL bFreeCast);
uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim);
// X/Server/AI/BrainStance.cpp
PowerID BrainStanceDoPower(ActorID idActor, const Target & tTarget, SNO snoPower, EBrainPowerResult eReason, void * ptExtraPowerStartData, const PreplayedAnimData * ptPreplayedAnimData, int32 nComboLevel, uint32 dwTargetMessageFlags);
// X/Server/Network/ClientValidation.cpp
BOOL CValidatePreplayAnimDataTimeStarted(const PreplayedAnimData & tAnimData);
// X/Server/Powers/Powers.cpp
PowerID PowerStart(ACDID idACD, SNO snoPower, const Target & tTarget);
uint32 PowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIsAI, BOOL bFreeCast);

// X/Common/ActorInventory.cpp
void ACDInventorySlotGetSize(const InventorySlotType eSlot, IVector2D & tSize);
BOOL SACDInventoryProcessSocketRequest(ActorCommonData & tACDPlayer, ActorCommonData & tACDGem, ActorCommonData & tACDItemToReceiveGem);
void SACDInventoryProcessStackTransfer(ActorCommonData & tACDItemSrc, ActorCommonData & tACDItemDest, const uint64 uAmount);
// X/Common/GameTemplate.cpp
BOOL GameTypeIsPvP();
// X/Common/Items.cpp
uint64 ItemGetMaxStackSize(const GBHandle hgbItem, const ActorCommonData * ptACDOwner, const ActorCommonData * ptACDItem);
BOOL ItemIsUsable(const ActorCommonData & tACDItem);
ItemUseType ItemGetUseType(const ActorCommonData & tACDItem);
BOOL ItemUseIsTargettedInInventory(const ItemUseType eItemUseType);
// X/Server/AI/PlayerBrain.cpp
BOOL PlayerBrainTryInventoryQuickMoveItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, InventoryLocation & tInvLocDest, const int32 * pnSlotYMinMax);
BOOL PlayerBrainTryInventoryStackItem(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest);
void PlayerBrainTryInventoryUseItem(const Player & tPlayer, const ACDID idACDItem, const Target & tTarget);
// X/Server/Network/ClientValidation.cpp
BOOL CValidateIsInventoryLocationValidForQuickMove(const ActorCommonData & tACDItem, const InvLoc & tLoc);
BOOL CValidateItemIsInPlayerInventoryForSocketing(const Player & tPlayer, const ACDID idACDItem);

// X/Common/GameCommonData.cpp
// X/Common/Player.cpp
SNO PlayerGetPowerInTraitSlot(const Player & tPlayer, const int32 nSlot);
// X/Common/Powers/PowersCommon.cpp
BOOL PowerCanActorTrainTrait(const ActorCommonData & tACD, SNO snoPower);
// X/Server/SPlayer.cpp
void SPlayerSwapActiveSkillSlots(Player & tPlayer, const int32 nSlot0, const int32 nSlot1);
void SPlayerRemoveTraitInSlot(Player & tPlayer, const int32 nSlot);
void SPlayerSwitchTraits(Player & tPlayer, const SNO snoPower, const int32 nSlot, const BOOL bApplyCooldown);

// X/Common/Player.h (Player.cpp:346)
extern const int32 cgnTraitSlotLevelReq[4];

// X/Common/Act.cpp
// X/Common/ActorCommonData.cpp
BOOL ACDIsInTown(const ActorCommonData & tACD);
// X/Common/Definitions/ConversationDefinition.cpp
uint32 ConversationTypeGetUsageFlags(const ConversationType eConvType);
// X/Common/NPC.cpp
// X/Common/Player.cpp
BOOL PlayerCanUsePortal(const ActorCommonData & tACDPlayer);
// X/Server/Network/ClientValidation.cpp
// X/Server/Network/ServerMessageSend.cpp
void SendPlayerCancelWarp(const ACDID idACDPlayer);
// X/Server/Powers/PowerHelper.cpp
BOOL PowerHelper_DoFirstTimeCosts(PowerID idPower, BOOL bWantCooldown);
// X/Server/Powers/Powers.cpp
void PowerStartDataInit(PowerStartData & tPowerStartData);
PowerID PowerStartEx(const PowerStartData & tPowerStartData, AttribGroupID idFastAttribSource, BOOL bForceStart, BOOL bIgnoreRange);
// X/Server/SGizmo.cpp
void GizmoUseWaypoint(Player & tPlayer, ACDID idACDWaypoint, int32 nWaypointNumber);
// X/Server/SNPC.cpp
BOOL NPCGetConversations(ACDID idACDPlayer, ACDID idACDNPC, ConversationArray & tConvArray);
void NPCStartConversation(ActorCommonData & tACDPlayer, ActorCommonData & tACDTalkingTo, const SNO snoConversation, const NPCInteractionType);
// X/Server/SPlayer.cpp
BOOL SPlayerCheckCanWarpToOtherPlayer(Player * ptPlayerWarping, Player * ptPlayerDest);
BOOL SPlayerCheckCanWarpToWaypoint(Player * ptPlayer, uint32 uWaypointIndex);

// X/Common/Nav/NavCellPath.cpp
TPathPoint2DPool * NavCellPathGetPointPool();
// X/Common/Nav/NavZone.cpp
BOOL NavCellHandleFromWorldPlace(const WorldPlace & tPlace, NavCellHandle & hNavCell);
const NavZoneInstance * NavCellHandleToZoneInstance(const NavCellHandle & hNavCell);
BOOL NavCellModifyForRoundedCorners(const NavZoneInstance * ptZoneInstance, NavCellHandle & hNavCell, Vector2D & vPos, uint16 uOneRequiredFlags);
uint32 NavCellGetStaticFlags(const WorldPlace & tPlace);
// X/Common/Powers/PowersCommon.cpp
BOOL PowerIsCancellableByWalking(AutoPowerDefinitionR pdefPower);
// X/Server/Actors/ActorPowers.cpp
SNO ActorPowersGetPrimaryPower(ActorID idActor);
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame);
// X/Server/SPlayer.cpp
BOOL SPlayerIsLocal(const Player & tPlayer);

// X/Server/Movement/SActorMovement.cpp:83
extern XVarUint32 s_varMovementVerboseMessage;

// Types the cases need that no src header defines; copied from the DWARF reference headers.
// >>> LOCAL TYPES
// X/Common/Networking/GameMessage.h
typedef int32 DeSyncReason;
// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h





// X/Common/Act.h
struct CachedWaypoint {  // size 0x2c
    Act eSourceAct;  // 0x00
    WaypointInfo tInfo;  // 0x04
};
// X/Common/Act.h
typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray;
// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h





// X/Common/ActorInventory.h








// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h






// X/Common/Networking/GameMessage.h












// X/Common/ActorInventory.h






// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Crafting/ItemCrafting.h
struct CraftItemsMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    GBID gbidRecipe;  // 0x08
    int32 nCount;  // 0x0c
};
// X/Common/Crafting/ItemCrafting.h
struct DyeItemMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    DyeType eDyeType;  // 0x0c
};
// X/Common/Crafting/ItemCrafting.h
struct TransmogItemMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidTransmog;  // 0x0c
};
// X/Common/Crafting/ItemCrafting.h
struct EnchantAffixMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidAffixToReroll;  // 0x0c
};









// X/Common/Networking/GameMessage.h










// X/Common/Player.h
struct PlayerLoadoutSaveMessage {  // size 0x44
    RequiredMessageHeader tHeader;  // 0x00
    int32 nLoadoutIndex;  // 0x08
    CHAR usName[53];  // 0x0c
};
// X/Common/Networking/GameMessage.h







// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h




// X/Common/Networking/GameMessage.h





// X/Common/Networking/GameMessage.h




// X/Common/ConversationCommon.h
struct UpdateConvAutoAdvanceMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConv;  // 0x08
    int32 nLineUID;  // 0x0c
    uint32 dwNextAdvanceTime;  // 0x10
};
// X/Common/ConversationCommon.h
struct AdvanceConvMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConv;  // 0x08
    int32 nLineUID;  // 0x0c
};
// X/Common/Player.h
struct StashIconStateMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    uint8 bStashIcons[13];  // 0x08
};
// X/Common/Player.h
struct LogoutContextMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    LogoutContext eLogoutContext;  // 0x08
};
enum { CTU_SHOWN_IN_TALK_MENU_BIT = 10 };  // ConversationDefinition.h
// ConversationDefinition is kept local (as in SPlayer.cpp): the shared header does not adopt it.
struct ConversationDefinition {  // size 0x110
    RequiredDefinitionHeader tHeader;  // 0x00
    int32 eConvType;  // 0x0c (ConversationType)
    ConversationIcon eConvIcon;  // 0x10
    uint8 _unadopted0[0x24 - 0x14];  // 0x14
    uint32 dwFlags;  // 0x24
    uint8 _unadopted1[0x110 - 0x28];  // 0x28
};
template <> inline const ConversationDefinition * DefinitionLockR<ConversationDefinition>(SNO sno) { return (const ConversationDefinition *)gptGroupDefConversation->LockR(sno, FALSE); }
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition * pdef) { void * lpDef = (void *)pdef; gptGroupDefConversation->UnlockR(lpDef); }
typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;
// <<< LOCAL TYPES
// X/Common/Act.cpp
const CachedWaypointArray & ActGetCachedWaypointArray();

// ---- message definitions local to this file (X/Common/Networking/GameMessage.h) ------------
// GameMessageType values this file uses that no included header defines.
// >>> GENERATED OPCODES (build/try/agentE/mkblock.py; values from the DWARF)


































































































































































// <<< GENERATED OPCODES











//@ 135
static void sTraceMessage(const void * pMessage)
{
    const RequiredMessageHeader * ptHeader = (const RequiredMessageHeader *)pMessage;

    uint32 dwNow = GameGetServerTime();
    (void)dwNow;

    const XTypeDescriptor * pTypeDescriptor = NULL;
    uint32 dwSize = 0;
    const char * pszMessageLabel;
    GetGameMessageTypeAndSize(ptHeader->eType, pTypeDescriptor, dwSize, pszMessageLabel);
    CRefString sLastPacket;
    SAXLogString sax(&sLastPacket);
    TypeDescriptorField field(pTypeDescriptor, pTypeDescriptor->GetName());
    pTypeDescriptor->SerializeToXML(sax, TypeDescriptorFieldSerializationInfo(field, pMessage), pMessage);
}

//@ 158
void sServerMessageReceive_ConsolePlayerResolved(Console::PlayerResolver * ptPlayerResolver, ServerSocketID idServerSocket, OnlineService::ErrorCode nErrorCode)
{
    if (!XServerSocketIDValid(idServerSocket))
    {
        return;
    }

    SGameID idSGame = AppServerGetFirstGame();

    if (nErrorCode != 0 ||
        ptPlayerResolver->ValidatePlayers(idSGame) != 0 ||
        ptPlayerResolver->AppendPlayersToGame(idSGame) != 0)
    {
        QuitGameMessage tQuitMessage;
        tQuitMessage.eReason = QUIT_REASON_JOIN_FAILED;
        ServerSendOutOfGameMessage(idServerSocket, &tQuitMessage, SMSG_QUIT, sizeof(tQuitMessage));

        XServerSocketDisconnect(idServerSocket, SDR_JOIN_FAILED);
        return;
    }

    OnlineService::PlayerResolveData & tResolveData = ptPlayerResolver->GetResolvedPlayer(0);
    SGameValidateJoinRequest(
        idServerSocket,
        idSGame,
        tResolveData.uHeroId,
        tResolveData.uAuthToken,
        tResolveData.eLocale);
}

//@ 206
BOOL ServerMessageReceiveOutsideGame(ServerSocketID idServerSocket, void * pMessage)
{
    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;

    BOOL bNetTrace = sgtAppGlobals.dwDebugFlags & 1;

    if (bNetTrace)
    {
        sTraceMessage(pMessage);
    }

    switch (ptHeader->eType)
    {
    case CMSG_JOIN_CONSOLE_GAME:
        {
            JoinConsoleGameMessage * ptJoinGameMsg = (JoinConsoleGameMessage *)pMessage;

            if (PlayersGetCount() >= 4)
            {
                return TRUE;
            }

            uint32 idConsoleJoinEntityId = (idServerSocket << 2) | ptJoinGameMsg->uControllerIndex;

            Console::PlayerResolver tPlayerResolver;
            tPlayerResolver.AddPlayer(
                idConsoleJoinEntityId,
                OnlineService::GameAccountHandle(idConsoleJoinEntityId, OnlineService::PROGRAM_D3, 1),
                "",
                2,
                0);

            OnlineService::PlayerResolveData & tResolvedPlayer = tPlayerResolver.GetResolvedPlayer(0);
            tResolvedPlayer.nControllerIndex = ptJoinGameMsg->uControllerIndex;
            tResolvedPlayer.eLocale = ptJoinGameMsg->eXLocale;
            tResolvedPlayer.uConsoleLicenses = ptJoinGameMsg->uLicenses;

            if (!tPlayerResolver.LoadFromParty(XServerSocketGetGameConnection(idServerSocket)))
            {
                return FALSE;
            }

            tPlayerResolver.Begin(OnlineService::internal::function_binder_3<void, Console::PlayerResolver *, ServerSocketID, OnlineService::ErrorCode>::bind<&sServerMessageReceive_ConsolePlayerResolved>(&tPlayerResolver, idServerSocket), 0);
            return TRUE;
        }
    }

    return FALSE;
}

// lines 428-538. The three tables are not in the image (clang folded the lookups into switches); their
// contents are the case sets of those switches, written here in ascending order. The original order is
// not recoverable.
static const GameMessageType gLegalMessagesWhilePaused[25] =  // line 428
{
    CMSG_LEAVE_CONSOLE_GAME,
    CMSG_START_LOGOUT_TIMER,
    CMSG_LOGOUT_NOW,
    CMSG_CANCEL_LOGOUT,
    CMSG_CHANNELLED_POWER_ABORTED,
    CMSG_CLIENT_WALK_POWER_ABORTED,
    CMSG_ACD_TRANSLATE_ACK,
    CMSG_TRADE,
    CMSG_REQUEST_PROMOTE_GAME_TO_PUBLIC,
    CMSG_SEND_PLAYER_PREFS,
    CMSG_PING,
    CMSG_LOAD_COMPLETE,
    CMSG_REQUEST_PAUSE_STATE_CHANGE,
    CMSG_SET_DUNGEON_ACCEPT,
    CMSG_REQUEST_SOFT_PAUSE_STATE_CHANGE,
    CMSG_TRY_LOWER_DIFFICULTY,
    CMSG_TRY_RAISE_DIFFICULTY,
    CMSG_TRY_CHANGE_DIFFICULTY,
    CMSG_MAIL_ITEM_SENT,
    CMSG_MAIL_CURRENCY_SENT,
    CMSG_SAVE_LOADOUT,
    CMSG_REDEEM_SEASON_REWARD_CONSOLE,
    CMSG_AMIIBO_SCAN_END,
    CMSG_HAS_NFP_DEVICE,
    CMSG_LOST_NFP_DEVICE,
};

// line 467. The DWARF records 51 entries; the soft-pause switch accepts the 25 values above plus the
// 47 below, so four entries (duplicates of other entries) cannot be recovered and are left out.
static const GameMessageType gLegalMessagesWhileSoftPaused[] =
{
    CMSG_TRY_DROP,
    CMSG_TRY_INVENTORY_MOVE,
    CMSG_TRY_INVENTORY_QUICK_MOVE,
    CMSG_TRY_INVENTORY_SOCKET,
    CMSG_TRY_INVENTORY_STACK,
    CMSG_TRY_INVENTORY_SPLIT_STACK,
    CMSG_TRY_INVENTORY_DROP_STACK_PORTION,
    CMSG_TRY_INVENTORY_STACK_TRANSFER,
    CMSG_TRY_GOLD_TRANSFER,
    CMSG_TRY_INVENTORY_SWAP_TO_BODY,
    CMSG_TRY_INVENTORY_USE,
    CMSG_TRY_ASSIGN_SKILL_TO_SLOT,
    CMSG_UNASSIGN_SKILL,
    CMSG_TRY_ASSIGN_TRAITS,
    CMSG_TRY_ASSIGN_LEGENDARY_POWER,
    CMSG_TRY_CHOOSE_HIRELING_SKILL,
    CMSG_PLAYER_CHANGE_POTION_BUTTON,
    CMSG_TRY_WAYPOINT,
    CMSG_PLAYER_LORE_PLAYING,
    CMSG_INSPECT_START,
    CMSG_INSPECT_END,
    CMSG_UPDATE_CONV_AUTO_ADVANCE,
    CMSG_SHARED_STASH_TRY_BUY,
    CMSG_BOSS_START_DECLINED,
    CMSG_RIFT_START_ACCEPTED,
    CMSG_TRY_ENCHANT_AFFIX,
    CMSG_CHOOSE_ENCHANT_AFFIX,
    CMSG_TRY_TRANSMOG,
    CMSG_TRY_DYE_ITEM,
    CMSG_TRY_TRANSMUTE,
    CMSG_TRY_IDENTIFY_WITH_CAST,
    CMSG_SHOWN_TUTORIAL,
    CMSG_NPC_INTERACT_PET_DISMISS,
    CMSG_ACT_TRANSITION_REQUEST_OPEN_WORLD,
    CMSG_ITEM_JUNK_FLAG_CHANGED,
    CMSG_PARAGON_POINT_SPEND,
    CMSG_PARAGON_POINTS_RESET,
    CMSG_PARAGON_PURCHASE_RESET_CATEGORY,
    CMSG_TELEPORT_TO_WAYPOINT,
    CMSG_SEND_SEASONAL_ITEM_TO_STASH,
    CMSG_SEND_SEASONAL_ITEM_TO_INVENTORY,
    CMSG_DROP_CRAFTING_MATERIAL,
    CMSG_DESTROY_GIFT,
    CMSG_PLAYER_GIFT_SENT,
    CMSG_EQUIP_COSMETIC_ITEM,
    CMSG_UNEQUIP_COSMETIC_ITEM,
    CMSG_EQUIP_LOADOUT,
};

static const GameMessageType gLegalMessagesWhileLoggingOut[15] =  // line 524
{
    CMSG_LEAVE_CONSOLE_GAME,
    CMSG_START_LOGOUT_TIMER,
    CMSG_LOGOUT_NOW,
    CMSG_CANCEL_LOGOUT,
    CMSG_CHANNELLED_POWER_ABORTED,
    CMSG_CLIENT_WALK_POWER_ABORTED,
    CMSG_ACD_TRANSLATE_ACK,
    CMSG_REQUEST_PROMOTE_GAME_TO_PUBLIC,
    CMSG_SEND_PLAYER_PREFS,
    CMSG_PING,
    CMSG_LOAD_COMPLETE,
    CMSG_REQUEST_PAUSE_STATE_CHANGE,
    CMSG_AMIIBO_SCAN_END,
    CMSG_HAS_NFP_DEVICE,
    CMSG_LOST_NFP_DEVICE,
};

//@ 545. Inlined into ServerMessageReceive; body from that copy.
static BOOL sMessageAllowWhilePaused(GameMessageType eType, int32 nPlayerIndex)
{
    for (uint32 i = 0; i < sizeof(gLegalMessagesWhilePaused) / sizeof(gLegalMessagesWhilePaused[0]); i++)
    {
        if (gLegalMessagesWhilePaused[i] == eType)
        {
            return TRUE;
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwPauseMessageDropCount < sizeof(tSGameGlobals.pauseMessages) / sizeof(tSGameGlobals.pauseMessages[0]))
    {
        tSGameGlobals.pauseMessages[tSGameGlobals.dwPauseMessageDropCount].eType = eType;
        tSGameGlobals.pauseMessages[tSGameGlobals.dwPauseMessageDropCount].nPlayerIndex = nPlayerIndex;
    }
    tSGameGlobals.dwPauseMessageDropCount++;

    const XTypeDescriptor * pTypeDescriptor;
    uint32 dwSize;
    const char * pszLabel;
    GetGameMessageTypeAndSize(eType, pTypeDescriptor, dwSize, pszLabel);

    return FALSE;
}

//@ 579. Inlined into ServerMessageReceive; body from that copy.
static BOOL sMessageAllowWhileSoftPaused(GameMessageType eType, int32 nPlayerIndex)
{
    for (uint32 i = 0; i < sizeof(gLegalMessagesWhileSoftPaused) / sizeof(gLegalMessagesWhileSoftPaused[0]); i++)
    {
        if (gLegalMessagesWhileSoftPaused[i] == eType)
        {
            return TRUE;
        }
    }

    // The soft-pause switch of the original also accepts every entry of gLegalMessagesWhilePaused and
    // drops refused messages into the same pause log; the DWARF shows no nested inlined call, so the
    // second lookup and the logging are written out here (how the original shared them is not known).
    for (uint32 i = 0; i < sizeof(gLegalMessagesWhilePaused) / sizeof(gLegalMessagesWhilePaused[0]); i++)
    {
        if (gLegalMessagesWhilePaused[i] == eType)
        {
            return TRUE;
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwPauseMessageDropCount < sizeof(tSGameGlobals.pauseMessages) / sizeof(tSGameGlobals.pauseMessages[0]))
    {
        tSGameGlobals.pauseMessages[tSGameGlobals.dwPauseMessageDropCount].eType = eType;
        tSGameGlobals.pauseMessages[tSGameGlobals.dwPauseMessageDropCount].nPlayerIndex = nPlayerIndex;
    }
    tSGameGlobals.dwPauseMessageDropCount++;

    const XTypeDescriptor * pTypeDescriptor;
    uint32 dwSize;
    const char * pszLabel;
    GetGameMessageTypeAndSize(eType, pTypeDescriptor, dwSize, pszLabel);

    return FALSE;
}

//@ 622. Inlined into ServerMessageReceive; body from that copy.
static BOOL sMessageAllowWhileLoggingOut(GameMessageType eType)
{
    for (uint32 i = 0; i < sizeof(gLegalMessagesWhileLoggingOut) / sizeof(gLegalMessagesWhileLoggingOut[0]); i++)
    {
        if (gLegalMessagesWhileLoggingOut[i] == eType)
        {
            return TRUE;
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.dwLoggingOutMessageDropCount++;

    const XTypeDescriptor * pTypeDescriptor;
    uint32 dwSize;
    const char * pszLabel;
    GetGameMessageTypeAndSize(eType, pTypeDescriptor, dwSize, pszLabel);

    return FALSE;
}

//@ 654. Inlined into ServerMessageReceive; body from that copy.
BOOL sMessageIsPlayerInitiated(GameMessageType eType)
{
    switch (eType)
    {
    case CMSG_TRY_CONSOLE_COMMAND:
    case CMSG_FINISHED_GAME_SETUP:
    case CMSG_ACD_TRANSLATE_ACK:
    case CMSG_PLAYER_BUSY:
    case CMSG_SHOWN_TUTORIAL:
    case CMSG_SEND_PLAYER_PREFS:
    case CMSG_LOAD_COMPLETE:
    case CMSG_NPC_INTERACT_CANCEL:
    case CMSG_REQUEST_SOFT_PAUSE_STATE_CHANGE:
        return FALSE;
    }

    return TRUE;
}

//@ 679
// Early-outs: `break` and `return` compile to the same code here, but they order the exit block's
// predecessors differently (tail merging looks at 150 at a time). CMSG_PLAYER_CHANGE_POTION_BUTTON and
// CMSG_TELEPORT_TO_WAYPOINT use `return`, as the original line table shows for their last conditions.
void ServerMessageReceive(int32 nPlayerIndex, void * pMessage)
{
    Player & tPlayerFrom = *PlayerGetByPlayerIndex(nPlayerIndex);
    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;

    ServerSocketID idServerSocket = ServerGetSocketIDFromGameConnectionID(tPlayerFrom.idGameConnection);
    if (idServerSocket != -1)
    {
        uint32 dwSocketPermissions = XServerSocketGetMsgPermissions(idServerSocket);

        if (!(dwSocketPermissions & 4) && ptHeader->eType >= MSG_RANGE_INGAME_START && ptHeader->eType <= MSG_RANGE_INGAME_END)
        {
            XServerSocketDisconnect(idServerSocket, SDR_PERMISSIONS);
            return;
        }
    }

    BOOL bNetTrace = sgtAppGlobals.dwDebugFlags & 1;
    uint32 dwNow = GameGetServerTime();

    if (bNetTrace)
    {
        sTraceMessage(pMessage);
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    int32 dwCurAppTime = AppGetTicks();
    tSGameGlobals.dwLastAppTimeInput = dwCurAppTime;

    ReplayServerReceiveMessage(nPlayerIndex, pMessage);

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayerFrom.idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return;
    }

    if (!CValidateSpamCheck(tPlayerFrom, ptHeader->eType, pMessage))
    {
        return;
    }

    if (Console::Server::MessageRecieve(nPlayerIndex, pMessage))
    {
        return;
    }

    if (tSGameGlobals.dwFlags & (1 << 23))
    {
        if (!sMessageAllowWhilePaused(ptHeader->eType, nPlayerIndex))
        {
            return;
        }
    }

    if (tSGameGlobals.dwFlags & (1 << 24))
    {
        if (!sMessageAllowWhileSoftPaused(ptHeader->eType, nPlayerIndex))
        {
            return;
        }
    }

    if (SPlayerIsLoggingOut(FALSE, tPlayerFrom))
    {
        if (!sMessageAllowWhileLoggingOut(ptHeader->eType))
        {
            return;
        }
    }

    if (ptHeader->eType != CMSG_UPDATE_CONV_AUTO_ADVANCE && ptHeader->eType != CMSG_PING && ptHeader->eType < MSG_RANGE_INGAME_END)
    {
        XServerSocketMarkMeaningfulMessage(idServerSocket);

        tPlayerFrom.dwLastInputTime = dwNow;

        if (sMessageIsPlayerInitiated(ptHeader->eType))
        {
            SPlayerRemoveInvulBuff(*ptACDPlayer);
        }
    }

    switch (ptHeader->eType)
    {
    case CMSG_COMBAT_LOG_TOGGLE:
    case CMSG_EXECUTE_NON_PRIMARY_POWER:
    case CMSG_HELPER_DETACH:
    case CMSG_REQUEST_DEBUG_ACTOR_TOOLTIP:
    case CMSG_ACT_TRANSITION_REQUEST_OPEN_WORLD:
    case CMSG_TRY_REDEEM_PVP_TOKENS:
    case CMSG_CHALLENGE_ACCEPT:
    case CMSG_SUMMON_VANITY_PET:
    case CMSG_DUNGEON_FINDER_BOSS_START:
    case CMSG_OPEN_WORLD_MODE_CHANGE_REQUEST:
    case CMSG_OPEN_WORLD_MODE_CHANGE_ACCEPT:
    case CMSG_OPEN_WORLD_MODE_RESET_REQUEST:
    case CMSG_PLAYER_CHOOSE_RIFT_REWARD_TYPE:
    case CMSG_CLEAR_LOADOUT:
    case CMSG_SET_LOADOUT_SLOT:
    case MSG_RANGE_INGAME_END:
        break;

    case CMSG_START_LOGOUT_TIMER:
        {
            LogoutContextMessage * ptLogoutMsg = (LogoutContextMessage *)pMessage;
            SPlayerStartLogoutTimer(nPlayerIndex, FALSE, ptLogoutMsg->eLogoutContext);
        }
        break;

    case CMSG_LOGOUT_NOW:
        SPlayerAttemptLogout(nPlayerIndex);
        break;

    case CMSG_CANCEL_LOGOUT:
        {
            LogoutContextMessage * ptLogoutMsg = (LogoutContextMessage *)pMessage;
            SPlayerCancelLogoutTimer(nPlayerIndex, ptLogoutMsg->eLogoutContext);
        }
        break;

    case CMSG_ACCEPTED_DEATH:
        {
            IntDataMessage * ptDeathMsg = (IntDataMessage *)pMessage;

            if (ptACDPlayer->IsAlive())
            {
                break;
            }

            {
                BOOL bResurrectionAllowed = TRUE;

                if ((uint32)ptACDPlayer->AttributesGetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME) < dwNow)
                {
                    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
                    {
                        if (PlayersAreLocalToEachOther(*ptPlayer, tPlayerFrom) &&
                            ptPlayer->nPlayerIndex != tPlayerFrom.nPlayerIndex)
                        {
                            ActorCommonData & tACDOtherPlayer = ACDGet(ptPlayer->idPlayerACD);
                            tACDOtherPlayer.AttributesSetInt(ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME, 0);
                        }
                    }
                }

                if ((uint32)ptACDPlayer->AttributesGetInt(GameIsParamFlagSet(GPF_RESURRECTION_DELAY) ? ATTRIBUTE_RESPAWN_GAMETIME : ATTRIBUTE_CHECKPOINT_RESURRECTION_ALLOWED_GAMETIME) >= dwNow)
                {
                    bResurrectionAllowed = FALSE;
                }
                else
                {
                    int32 nCorpseDelay = ptACDPlayer->AttributesGetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
                    if (nCorpseDelay > 0)
                    {
                        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
                        {
                            if (PlayersAreLocalToEachOther(*ptPlayer, tPlayerFrom) &&
                                ptPlayer->nPlayerIndex != tPlayerFrom.nPlayerIndex)
                            {
                                ActorCommonData & tACDOtherPlayer = ACDGet(ptPlayer->idPlayerACD);
                                if (tACDOtherPlayer.IsAlive())
                                {
                                    bResurrectionAllowed = FALSE;
                                    break;
                                }
                            }
                        }

                        if (bResurrectionAllowed)
                        {
                            ptACDPlayer->AttributesSetInt(ATTRIBUTE_CORPSE_RESURRECTION_ALLOWED_GAMETIME, 0);
                        }
                    }
                }

                if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
                {
                    bResurrectionAllowed = TRUE;
                }

                if (!bResurrectionAllowed)
                {
                    break;
                }

                ServerMessageSendAcceptedDeathAck(tPlayerFrom.idPlayerACD);

                if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_RESURRECT_AS_OBSERVER))
                {
                    BrainAPIDoResurrect((ActorID)ACDGetOwner(tPlayerFrom.idPlayerACD), 0);
                    break;
                }

                switch (ptDeathMsg->nData)
                {
                case 0:
                    break;

                case 1:
                    if (!PlayerCanReviveAtCheckpoint(*ptACDPlayer))
                    {
                        return;
                    }
                    break;

                case 2:
                    if (!PlayerCanReviveAtCorpse(*ptACDPlayer))
                    {
                        return;
                    }

                    if (SPlayerIsInRift(*ptACDPlayer) &&
                        NephalemRift::IsTieredRiftActive() &&
                        NephalemRiftManager::RiftPlayerCount() == 1 &&
                        NephalemRiftManager::PlayerPaidForRift(&tPlayerFrom))
                    {
                        int32 nAllowedResTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_CORPSE_RESURRECTION_ALLOWED_GAMETIME);
                        if (nAllowedResTime != 0)
                        {
                            int32 nRemainingTime = nAllowedResTime - GameGetServerTime();
                            if (nRemainingTime > 0)
                            {
                                NephalemRiftManager::AddArtificiallyElapsedTime(nRemainingTime);
                                SNO snoTimer = GlobalSNOGet(GLOBALSNO_TIMEDEVENT_TIERED_RIFT_TIMER);
                                STimedEventAddArtificiallyElapsedTime(snoTimer, nRemainingTime);
                                PowerReduceAllCooldowns(ptACDPlayer->id, (float)nRemainingTime / 60.0f);
                            }
                        }
                    }
                    break;

                default:
                    return;
                }

                BrainAPIResurrectByType(tPlayerFrom.idPlayerACD, ptDeathMsg->nData);
            }
        }
        break;

    case CMSG_FINISHED_GAME_SETUP:
        if (!tPlayerFrom.bDidFirstTimeSetup)
        {
            SGameFinishPlayerSetup(tPlayerFrom);
        }
        break;

    case CMSG_TARGET:
        {
            SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

            TargetMessage * ptTargetMsg = (TargetMessage *)pMessage;

            if (!CValidateTargetType(ptTargetMsg->eType))
            {
                break;
            }

            if (!CValidateSNO(POWER_SNO, ptTargetMsg->snoPower))
            {
                break;
            }

            if (!CValidateTargetPlace(ptTargetMsg->tPlace, *ptACDPlayer))
            {
                break;
            }

            if (!CValidateANNAllowInvalid(ptTargetMsg->annTarget))
            {
                if (ptTargetMsg->eType == TT_ACTOR)
                {
                    ptTargetMsg->eType = TT_POINT;

                    if (PowerTargetsContactPlaneOnly(AutoPowerDefinitionR(ptTargetMsg->snoPower)))
                    {
                        PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(ptACDPlayer, ptTargetMsg->snoPower, ptTargetMsg->tPlace, FALSE);
                    }
                }
            }

            if (!CValidateANNInWorldAllowInvalid(ptTargetMsg->annTarget))
            {
                break;
            }

            if (!CValidateANNAllowInvalid(ptTargetMsg->annItemBeingUsed))
            {
                break;
            }

            if (ptTargetMsg->eType == TT_ACTOR && ptTargetMsg->annTarget == -1)
            {
                break;
            }

            int32 nCurrentServerComboLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_COMBO_LEVEL);

            if (ptTargetMsg->nComboLevel != 0)
            {
                if (!PowerIsComboPower(ptTargetMsg->snoPower))
                {
                    break;
                }

                if (ptTargetMsg->nComboLevel != 0 &&
                    ptTargetMsg->nComboLevel != (nCurrentServerComboLevel + 1) % 3)
                {
                    break;
                }

                {
                    ScopedGlobalDefinitionLockR pdefGlobal;
                    uint32 dwComboTimeout = Math::FloatToInt(pdefGlobal->ptServerData->flComboServerTimeout * 60.0f);
                    if (ptTargetMsg->nComboLevel != 0 &&
                        ptACDPlayer->AttributesGetInt(ATTRIBUTE_COMBO_TIME_LAST_MOVE) + dwComboTimeout < GameGetServerTime())
                    {
                        break;
                    }
                }
            }

            if (ptTargetMsg->nComboLevel <= nCurrentServerComboLevel)
            {
                // Both values are fetched and dropped in the original (arguments of a call compiled out).
                ptACDPlayer->AttributesGetInt(ATTRIBUTE_COMBO_TIME_LAST_MOVE);
                GameGetServerTime();
            }

            if (PowerIsGizmo(AutoPowerDefinitionR(ptTargetMsg->snoPower)))
            {
                if (ptTargetMsg->eType != TT_ACTOR)
                {
                    break;
                }

                ActorCommonData * ptACDTarget = ACDTryToGet(ACDFromANN(ptTargetMsg->annTarget));

                if (ptACDTarget != NULL && ptACDTarget->GetGizmoType() == ACTORGIZMO_HEARTH_PORTAL)
                {
                    if (!tPlayerFrom.tHearthPortalInfo.IsValid())
                    {
                        break;
                    }
                }
            }

            if (ptTargetMsg->eType == TT_POINT)
            {
                WorldPlace wpPlayer = ptACDPlayer->GetPlace();
                Target tTarget;
                tTarget.InitializeFromPlace(ptTargetMsg->tPlace);

                if (!PowerIsTargetWithinValidZRange(ptACDPlayer->id, tTarget))
                {
                    if (PowerTargetsContactPlaneOnly(AutoPowerDefinitionR(ptTargetMsg->snoPower)))
                    {
                        PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(ptACDPlayer, ptTargetMsg->snoPower, ptTargetMsg->tPlace, TRUE);
                    }
                    else
                    {
                        ptTargetMsg->tPlace.wp.z = wpPlayer.wp.z;
                    }
                }
            }

            if (PowerChecksPathAndCliptoPathableAttackRadius(ptTargetMsg->snoPower))
            {
                PowerHelper_CheckPathAndClipToPathableAttackRadius(tPlayerFrom.idPlayerACD, ptTargetMsg);
            }

            if (PowerClipsTargetToAttackRadius(AutoPowerDefinitionR(ptTargetMsg->snoPower)))
            {
                PowerHelper_ClipTargetMessageToPowerAttackRadius(tPlayerFrom.idPlayerACD, ptTargetMsg);
            }

            if (PowerIsComboPower(ptTargetMsg->snoPower))
            {
                ptACDPlayer->SetComboLevel(ptTargetMsg->nComboLevel);
            }

            Target tTarget;
            SNO snoPower = ptTargetMsg->snoPower;
            if (ptTargetMsg->eType == TT_ACTOR)
            {
                BOOL bUseTargetFallbackPosition = !PowerIsGizmo(AutoPowerDefinitionR(snoPower));
                tTarget.InitializeFromACD(ACDFromANN(ptTargetMsg->annTarget), bUseTargetFallbackPosition, FALSE);
                if (PowerTargetsContactPlaneOnly(AutoPowerDefinitionR(ptTargetMsg->snoPower)) && bUseTargetFallbackPosition)
                {
                    PowerHelper_SetWorldPlaceHeightToPowerContactOffsetHeight(ptACDPlayer, snoPower, tTarget.tPlace, FALSE);
                }
            }
            else if (ptTargetMsg->eType == TT_POINT)
            {
                tTarget.InitializeFromPlace(ptTargetMsg->tPlace);
            }
            else
            {
                break;
            }

            if (PowerRequiresTarget(*ptACDPlayer, AutoPowerDefinitionR(snoPower)) && tTarget.IsNone())
            {
                break;
            }

            PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), tPlayerFrom.idPlayerACD);

            BOOL bIsAmiiboPower = FALSE;
            ActorCommonData * tACDItemBeingUsed = ACDTryToGet(ACDFromANN(ptTargetMsg->annItemBeingUsed));
            if (tACDItemBeingUsed != NULL)
            {
                bIsAmiiboPower = eTemplate == PROC_POWER && tACDItemBeingUsed->snoActor == SNOFileLookupGUID(ACTOR_SNO, "Amiibo_Portal_Device");
            }

            if (!bIsAmiiboPower)
            {
                switch (eTemplate)
                {
                case GIZMO_POWER:
                    break;

                case ANIMATION_POWER:
                case SPECIALWALK_POWER:
                case ATTACKMOVE_POWER:
                    if (!CValidatePowerUsableByPlayer(*ptACDPlayer, snoPower))
                    {
                        BOOL bLogClientFailure = TRUE;

                        if (ptACDPlayer->AttributesAttributeExists(ATTRIBUTE_SKILL_OVERRIDE_STARTED_OR_ENDED_ACTIVE))
                        {
                            bLogClientFailure = !ptACDPlayer->AttributesAttributeExists(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE_STARTED_OR_ENDED, snoPower));
                        }
                        (void)bLogClientFailure;

                        return;
                    }
                    break;

                default:
                    return;
                }
            }

            if (PowerIsPrimary(snoPower, ptACDPlayer->id))
            {
                ActorMovement * ptMove = ptACDPlayer->GetActorMovement();
                if (ptMove != NULL && ptMove->IsInArcingMovement())
                {
                    break;
                }
            }

            PreplayedAnimData tPreplayedAnimData;
            if (ptTargetMsg->tAnimPreplayData.Exists())
            {
                tPreplayedAnimData.dwPrePlayedAnimFlags = ptTargetMsg->tAnimPreplayData.Get().dwAnimPrePlayDataFlags;
                tPreplayedAnimData.dwServerTimeAnimStarted = ptTargetMsg->tAnimPreplayData.Get().dwServerTimeAnimStarted;
                tPreplayedAnimData.dwSyncedSeed = ptTargetMsg->tAnimPreplayData.Get().dwSyncedSeed;

                if (!CValidatePreplayAnimDataFlags(*ptACDPlayer, tPreplayedAnimData, snoPower))
                {
                    break;
                }
            }

            ACDID idACDItemBeingUsed = ACDFromANN(ptTargetMsg->annItemBeingUsed);

            SNO snoItemConsumed = PowerConsumedItem(AutoPowerDefinitionR(snoPower));
            if (snoItemConsumed != SNO_NONE)
            {
                ActorCommonData * ptACDItemBeingUsed = ACDTryToGet(idACDItemBeingUsed);
                if (ptACDItemBeingUsed == NULL || ptACDItemBeingUsed->snoActor != snoItemConsumed)
                {
                    break;
                }

                if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItemBeingUsed, TRUE))
                {
                    break;
                }
            }

            {
                ActorID idActor = (ActorID)ACDGetOwner(ptACDPlayer->id);
                PowerID idPowerPrimary = ActorPowersGetPrimary(idActor);
                Power * ptPrimaryPower = PowerTryToGet(idPowerPrimary);
                if (ptPrimaryPower != NULL)
                {
                    PowerAbortLooping(ptACDPlayer->id, ptPrimaryPower->snoPower);
                }
            }

            PlayerBrainTarget(
                tPlayerFrom.idPlayerACD,
                tTarget,
                snoPower,
                idACDItemBeingUsed,
                tPreplayedAnimData,
                ptTargetMsg->nComboLevel,
                ptTargetMsg->dwFlags.Exists() ? ptTargetMsg->dwFlags.Get() : 0);
        }
        break;

    case CMSG_SECONDARY_ANIMATION_POWER:
        {
            SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

            SecondaryAnimationPowerMessage * ptMessage = (SecondaryAnimationPowerMessage *)pMessage;
            ACDID idACDPlayer = tPlayerFrom.idPlayerACD;
            SNO snoPower = ptMessage->snoPower;

            if (!CValidateSNO(POWER_SNO, snoPower) || idACDPlayer == ACDID_INVALID)
            {
                break;
            }

            ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
            if (ptACD == NULL)
            {
                break;
            }

            PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), tPlayerFrom.idPlayerACD);
            if (eTemplate != SECONDARY_ANIMATION_POWER)
            {
                break;
            }

            if (!CValidatePowerUsableByPlayer(*ptACD, snoPower))
            {
                BOOL bLogClientFailure = TRUE;

                if (ptACD->AttributesAttributeExists(ATTRIBUTE_SKILL_OVERRIDE_STARTED_OR_ENDED_ACTIVE))
                {
                    bLogClientFailure = !ptACD->AttributesAttributeExists(MakeAttribKey(ATTRIBUTE_SKILL_OVERRIDE_STARTED_OR_ENDED, snoPower));
                }
                (void)bLogClientFailure;

                break;
            }

            Target tTarget(cgTargetNone);
            if (PowerRequiresActorTarget(*ptACD, AutoPowerDefinitionR(snoPower)) ||
                PowerRequiresTarget(*ptACD, AutoPowerDefinitionR(snoPower)) ||
                PowerCastTargetNecroCorpsesOnly(*ptACD, AutoPowerDefinitionR(snoPower)))
            {
                tTarget.InitializeFromACD(ACDFromANN(ptMessage->annTarget), FALSE, FALSE);
            }

            uint32 uFailureReason = PowerACDCanStartPowerFailureReasons(*ptACD, AutoPowerDefinitionR(snoPower), &tTarget, FALSE, FALSE);
            if (uFailureReason != 0)
            {
                break;
            }

            if (ptACD->AttributesGetInt(ATTRIBUTE_QUEUE_DEATH))
            {
                break;
            }

            PreplayedAnimData tPreplayedAnimData;
            if (ptMessage->tAnimPreplayData.Exists())
            {
                ActorMovement * ptMove = ptACD->GetActorMovement();
                if (ptMove == NULL || !ptMove->IsMoving())
                {
                    tPreplayedAnimData.dwPrePlayedAnimFlags = ptMessage->tAnimPreplayData.Get().dwAnimPrePlayDataFlags;
                    tPreplayedAnimData.dwServerTimeAnimStarted = ptMessage->tAnimPreplayData.Get().dwServerTimeAnimStarted;
                    tPreplayedAnimData.dwSyncedSeed = ptMessage->tAnimPreplayData.Get().dwSyncedSeed;

                    if (!CValidatePreplayAnimDataFlags(*ptACD, tPreplayedAnimData, snoPower))
                    {
                        break;
                    }

                    if (!CValidatePreplayAnimDataTimeStarted(tPreplayedAnimData))
                    {
                        break;
                    }

                    {
                        uint32 uAnimTag = PowerDetermineAnimation(AutoPowerDefinitionR(snoPower), ptACD->id, tTarget, (tPreplayedAnimData.dwPrePlayedAnimFlags & (1 << 1)) != 0);
                        AnimPlayerPlaySecondaryAnim(*ptACD, uAnimTag, tPreplayedAnimData, snoPower);
                    }
                }
            }

            PowerIsPrimary(snoPower, tPlayerFrom.idPlayerACD);

            {
                ActorID idActor = (ActorID)ACDGetOwner(ptACD->id);
                PowerID idPowerPrimary = ActorPowersGetPrimary(idActor);
                Power * ptPrimaryPower = PowerTryToGet(idPowerPrimary);
                if (ptPrimaryPower != NULL)
                {
                    PowerAbortLooping(ptACD->id, ptPrimaryPower->snoPower);
                }
            }

            PowerStart(ptACD->id, snoPower, tTarget);
        }
        break;

    case CMSG_LOOPING_ANIMATION_POWER:
        {
            SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

            LoopingAnimationPowerMessage * ptMessage = (LoopingAnimationPowerMessage *)pMessage;
            SNO snoPower = ptMessage->snoPower;

            if (!CValidateSNO(POWER_SNO, snoPower))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

            PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), tPlayerFrom.idPlayerACD);
            if (eTemplate != LOOPING_ANIMATION_POWER)
            {
                break;
            }

            if (!CValidatePowerUsableByPlayer(tACDPlayer, snoPower))
            {
                break;
            }

            uint32 uFailureReason = PowerFailureReasons(tACDPlayer, AutoPowerDefinitionR(snoPower), &cgTargetNone, FALSE, FALSE);
            if (uFailureReason != 0)
            {
                break;
            }

            ActorID idPlayerActor = (ActorID)ACDGetOwner(tPlayerFrom.idPlayerACD);
            PowerID idPower = BrainStanceDoPower(idPlayerActor, cgTargetNone, snoPower, BRAIN_POWER_START, NULL, NULL, 0, 0);
            Power * ptPower = PowerTryToGet(idPower);
            if (ptPower != NULL)
            {
                ptPower->dwParam = ptMessage->snoData0;
                ptPower->flParam = (float)ptMessage->nData1;
            }
        }
        break;

    case CMSG_CHANNELLED_POWER_ABORTED:
        {
            DWordDataMessage * ptMessage = (DWordDataMessage *)pMessage;

            ACDID idACDPlayer = tPlayerFrom.idPlayerACD;
            SNO snoPower = ptMessage->dwData;

            if (!CValidateSNO(POWER_SNO, snoPower))
            {
                break;
            }

            PlayerBrainRequestAbortChannelled(idACDPlayer, snoPower);
        }
        break;

    case CMSG_LOOPING_POWER_ABORTED:
        {
            DWordDataMessage * ptMessage = (DWordDataMessage *)pMessage;

            ACDID idACDPlayer = tPlayerFrom.idPlayerACD;
            SNO snoPower = ptMessage->dwData;

            if (!CValidateSNO(POWER_SNO, snoPower))
            {
                break;
            }

            PowerAbortLooping(idACDPlayer, snoPower);
        }
        break;

    case CMSG_CLIENT_WALK_POWER_ABORTED:
        {
            DWordDataMessage * ptMessage = (DWordDataMessage *)pMessage;

            ACDID idACDPlayer = tPlayerFrom.idPlayerACD;
            SNO snoPower = ptMessage->dwData;

            if (!CValidateSNO(POWER_SNO, snoPower))
            {
                break;
            }

            PowerAbortClientWalk(idACDPlayer, snoPower);
        }
        break;

    case CMSG_TRY_WAYPOINT:
        {
            SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

            TryWaypointMessage * ptMessage = (TryWaypointMessage *)pMessage;

            ACDID idACDWaypoint = ACDFromANN(ptMessage->annWaypoint);

            const float WAYPOINT_USE_MIN_DISTANCE_SQR = 400.0f;

            if (!ptACDPlayer->IsAlive())
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            ActorCommonData * ptACDWaypoint = ACDTryToGet(idACDWaypoint);
            if (ptACDWaypoint == NULL)
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            if (ptACDWaypoint->GetGizmoType() != ACTORGIZMO_WAYPOINT)
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            if (ptACDWaypoint->tLocation.eActorLocation != 0)
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            WorldPlace placePlayer = ptACDPlayer->GetPlace();
            WorldPlace placeWaypoint = ptACDWaypoint->GetPlace();
            SWorld * ptSWorld = SWorldTryToGet(placeWaypoint.idSWorld);
            if (ptSWorld == NULL)
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            if (placePlayer.idSWorld != placeWaypoint.idSWorld)
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            {
                float flDistanceSquared = Vector2D::DistanceSquared(placePlayer.wp.As2D(), placeWaypoint.wp.As2D());
                if (flDistanceSquared > WAYPOINT_USE_MIN_DISTANCE_SQR)
                {
                    SendPlayerCancelWarp(ptACDPlayer->id);
                    break;
                }
            }

            if (!PlayerCanUsePortal(*ptACDPlayer))
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            BOOL bValidWaypointIndex = FALSE;
            const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
            size_t nCount = tWaypoints.size();

            if (ptMessage->nWaypoint < nCount)
            {
                BOOL bActive = SGameGlobalsGet().WaypointIsActive(ptMessage->nWaypoint);
                if (bActive)
                {
                    bValidWaypointIndex = TRUE;
                }
            }

            if (!bValidWaypointIndex)
            {
                SendPlayerCancelWarp(ptACDPlayer->id);
                break;
            }

            GizmoUseWaypoint(tPlayerFrom, idACDWaypoint, ptMessage->nWaypoint);
        }
        break;

    case CMSG_OPERATE_GIZMO:
        {
            SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

            ANNDataMessage * ptOperateMsg = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptOperateMsg->ann))
            {
                break;
            }

            ActorCommonData * ptACDGizmo = ACDTryToGet(ACDFromANN(ptOperateMsg->ann));
            if (ptACDGizmo != NULL)
            {
                ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

                if (!ptACDGizmo->IsGizmo())
                {
                    break;
                }

                if (!CValidatePlayerNearGizmo(tPlayerFrom, *ptACDGizmo))
                {
                    break;
                }

                if (ptACDGizmo->GetGizmoType() == ACTORGIZMO_HEADSTONE)
                {
                    break;
                }

                GizmoOperate(*ptACDGizmo, &tACDPlayer, 0, TRUE);
            }
        }
        break;

    case CMSG_ACCEPT_RESURRECT:
        {
            SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

            if (tACDPlayer.IsAlive())
            {
                break;
            }

            if (!tACDPlayer.AttributesGetInt(ATTRIBUTE_WAITING_TO_ACCEPT_RESURRECTION))
            {
                break;
            }

            ACDID idACDHeadStone = GetLastHeadstoneACD(tACDPlayer);
            ActorCommonData * ptACDHeadstone = ACDTryToGet(idACDHeadStone);
            if (ptACDHeadstone == NULL)
            {
                break;
            }

            ACDID idACDOperator = (ACDID)ptACDHeadstone->AttributesGetInt(ATTRIBUTE_GIZMO_OPERATOR_ACDID);
            ActorCommonData * ptACDOperator = ACDIsPlayer(idACDOperator) ? ACDTryToGet(idACDOperator) : NULL;

            GizmoOperate(*ptACDHeadstone, ptACDOperator, 0, TRUE);
        }
        break;

    case CMSG_ACD_TRANSLATE_NORMAL:
        {
            ACDClientTranslateMessage * ptTranslateMsg = (ACDClientTranslateMessage *)pMessage;
            uint32 uMoveFlags = ptTranslateMsg->uMoveFlags;
            if (uMoveFlags >= (1 << MOVEBIT_COUNT))
            {
                break;
            }

            {
                ActorID idActor = (ActorID)ACDGetOwner(tPlayerFrom.idPlayerACD);
                Actor * ptActor = ActorTryToGet(idActor);
                ActorCommonData * ptACD = ACDTryToGet(tPlayerFrom.idPlayerACD);
                if (ptActor != NULL && ptActor->ptSActorMovement != NULL && ptACD != NULL)
                {
                    CRefString strError;
                    SNO snoPower = SNO_NONE;
                    if (ptTranslateMsg->snoPower.Exists())
                    {
                        snoPower = ptTranslateMsg->snoPower.Get();
                    }

                    BOOL bPlayerIsLocal = SPlayerIsLocal(tPlayerFrom);

                    if (!bPlayerIsLocal)
                    {
                        MoveValidationError eError = ptActor->ptSActorMovement->ValidateClientMovementMessage(ptTranslateMsg->uTickBase, ptTranslateMsg->uTicksAhead, ptTranslateMsg->vDest, ptTranslateMsg->flMovementSpeed, snoPower, strError);
                        if (eError != MOVEVALIDATION_OK && eError != MOVEVALIDATION_IGNORE && eError != MOVEVALIDATION_SNAP)
                        {
                            // The DWARF records a local `ValidationErrorSeverity eSeverity` (line 2152) here; nothing of
                            // it survives in the code, so it is left out.
                            if (s_varMovementVerboseMessage.Get())
                            {
                                CRefString strHackDetected("Speed Hack Detected! (");
                                strHackDetected += strError;
                                strHackDetected += ")\n";
                            }
                        }

                        if (eError != MOVEVALIDATION_OK && eError != MOVEVALIDATION_IGNORE)
                        {
                            CRefString strVerbose = ptActor->ptSActorMovement->GetVerboseMovementMessage();
                            if (strVerbose.Length() > 0)
                            {
                            }
                        }

                        if (eError != MOVEVALIDATION_OK)
                        {
                            if (eError != MOVEVALIDATION_IGNORE)
                            {
                                PlayerDeSyncSnapMessage tMessage;
                                tMessage.placeSnap = ptACD->GetPlace();
                                tMessage.eReason = 1;  // DeSyncReason; no enumerator recorded
                                tMessage.snoPowerCurrent = SNO_NONE;

                                tMessage.nPlayerIndex = tPlayerFrom.nPlayerIndex;

                                PlayerList listDestPlayers(PlayerListGetElementPool());
                                listDestPlayers.AddTail_Tracked(&tPlayerFrom);
                                MessageSendToClient(listDestPlayers, SMSG_PLAYER_DESYNC_SNAP, &tMessage, sizeof(tMessage));
                            }

                            break;
                        }
                    }

                    SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

                    BOOL bAllowedMove = TRUE;

                    SWorldID idSWorldACD = ptACD->GetSWorldID();
                    WorldPlace placeDest;
                    placeDest.idSWorld = idSWorldACD;
                    placeDest.wp = ptTranslateMsg->vDest;

                    if (!bPlayerIsLocal)
                    {
                        ACDCollSettings tCollSettings;
                        tCollSettings = ptACD->tCollSettings;

                        // Dynamic collision flags the validation ignores; no enumerators recorded.
                        tCollSettings.uDynamicFlagsNoOverlap &= ~0x37b;

                        ACDCollParams tACDCollParams;
                        tACDCollParams.uObjectNoOverlapFlags = tCollSettings.uDynamicFlagsNoOverlap;
                        tACDCollParams.eTeam = ptACD->GetTeam();

                        PathPoint2DList listPathPoints;
                        listPathPoints.SetPool(NavCellPathGetPointPool());

                        WorldPlace placeTest(placeDest);

                        {
                            NavCellHandle hNavCellTest;
                            if (NavCellHandleFromWorldPlace(placeTest, hNavCellTest))
                            {
                                const NavZoneInstance * ptNavZoneInstance = NavCellHandleToZoneInstance(hNavCellTest);
                                NavCellModifyForRoundedCorners(ptNavZoneInstance, hNavCellTest, placeTest.wp.As2D(), tCollSettings.uStaticOneRequiredFlags);
                            }
                        }

                        uint32 uStaticFlagsAtDest = NavCellGetStaticFlags(placeTest);
                        if ((uStaticFlagsAtDest & tCollSettings.uStaticOneRequiredFlags) == 0 ||
                            (uStaticFlagsAtDest & tCollSettings.uStaticPreventFlags) != 0)
                        {
                            bAllowedMove = FALSE;
                        }
                        else
                        {
                            PathResult ePathResult = ptACD->GetActorMovement()->FindPath(placeTest, listPathPoints, tCollSettings.uStaticOneRequiredFlags, tCollSettings.uStaticPreventFlags, &tACDCollParams);
                            bAllowedMove = (ePathResult == PATH_RESULT_NOT_NECESSARY || ePathResult == PATH_RESULT_FOUND);
                        }
                    }

                    if (!bAllowedMove)
                    {
                        WorldPlace placeStart = ptACD->GetPlace();

                        if (s_varMovementVerboseMessage.Get())
                        {
                        }

                        PlayerDeSyncSnapMessage tMessage;
                        tMessage.placeSnap = ptACD->GetPlace();
                        tMessage.eReason = 1;  // DeSyncReason; no enumerator recorded
                        tMessage.snoPowerCurrent = SNO_NONE;

                        tMessage.nPlayerIndex = tPlayerFrom.nPlayerIndex;

                        PlayerList listDestPlayers(PlayerListGetElementPool());
                        listDestPlayers.AddTail_Tracked(&tPlayerFrom);
                        MessageSendToClient(listDestPlayers, SMSG_PLAYER_DESYNC_SNAP, &tMessage, sizeof(tMessage));

                        break;
                    }

                    if (!isfinite(ptTranslateMsg->aDesiredYaw) || isnan(ptTranslateMsg->aDesiredYaw))
                    {
                        break;
                    }

                    ActorAnimationTag eAnimTag = (ActorAnimationTag)ptTranslateMsg->eAnimTag;
                    float flSpeed = ptTranslateMsg->flMovementSpeed;
                    Angle aYaw = Math::NormalizeAngle(ptTranslateMsg->aDesiredYaw);
                    ptActor->ptSActorMovement->ClientMove(placeDest, flSpeed, eAnimTag, aYaw);

                    SNO snoPrimaryPower = ActorPowersGetPrimaryPower(idActor);
                    if (snoPrimaryPower != SNO_NONE && PowerIsCancellableByWalking(AutoPowerDefinitionR(snoPrimaryPower)))
                    {
                        ActorPowersInterruptPrimary(idActor, TRUE);
                    }
                }
            }
        }
        break;

    case CMSG_ACD_TRANSLATE_FACING:
        {
            ACDTranslateFacingMessage * ptTranslateMsg = (ACDTranslateFacingMessage *)pMessage;

            if (tPlayerFrom.idPlayerACD != ACDFromANN(ptTranslateMsg->ann))
            {
                break;
            }

            if (!isfinite(ptTranslateMsg->aDesiredYaw) || isnan(ptTranslateMsg->aDesiredYaw))
            {
                break;
            }

            ACDID idACD = ACDFromANN(ptTranslateMsg->ann);
            if (idACD != ACDID_INVALID)
            {
                ActorID idActor = (ActorID)ACDGetOwner(idACD);
                Actor * ptActor = ActorTryToGet(idActor);
                ActorCommonData * ptACD = ACDTryToGet(idACD);
                if (ptActor != NULL && ptActor->ptSActorMovement != NULL && ptACD != NULL)
                {
                    SPlayerResetInactionTime(idACD);

                    ptActor->ptSActorMovement->ClientFace(ptTranslateMsg->aDesiredYaw,
                                                          ptTranslateMsg->bSnapFacing);
                }
            }
        }
        break;

    case CMSG_ACD_TRANSLATE_ACK:
        {
            ACDTranslateAckMessage * ptTranslateAckMsg = (ACDTranslateAckMessage *)pMessage;

            {
                ActorID idActor = (ActorID)ACDGetOwner(tPlayerFrom.idPlayerACD);
                Actor * ptActor = ActorTryToGet(idActor);
                if (ptActor != NULL && ptActor->ptSActorMovement != NULL)
                {
                    ptActor->ptSActorMovement->AcceptAckValue(ptTranslateAckMsg->uAckValue);
                }
            }
        }
        break;

    case CMSG_TRY_INVENTORY_MOVE:
        {
            InventoryRequestMoveMessage * ptMsg = (InventoryRequestMoveMessage *)pMessage;

            if (!CValidateANN(ptMsg->annItem))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            if (!PlayerCanManipulateInventory(tACDPlayer))
            {
                break;
            }

            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (!CValidateIsInventoryLocationValidToPlayer(tPlayerFrom, ptMsg->tLocation))
            {
                break;
            }

            if (ItemIsSocketed(tACDItem))
            {
                break;
            }

            ACDID idACDOwner = ACDFromANN(ptMsg->tLocation.annOwner);
            InventoryLocation tInvLocDst;
            ACDInventoryLocationSet(tInvLocDst,
                                    idACDOwner,
                                    ptMsg->tLocation.eSlot,
                                    ptMsg->tLocation.nSlotX,
                                    ptMsg->tLocation.nSlotY);

            if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLocDst, FALSE, FALSE) != GAMEERROR_NONE)
            {
                break;
            }

            if (ptMsg->tLocation.eSlot == SLOT_PLAYER_BUYBACK)
            {
                SACDInventorySell(tACDPlayer, tACDItem);
            }
            else
            {
                PlayerBrainTryInventoryMoveItem(tACDPlayer, tACDItem, tInvLocDst);
            }
        }
        break;

    case CMSG_TRY_INVENTORY_QUICK_MOVE:
        {
            InventoryRequestQuickMoveMessage * ptMsg = (InventoryRequestQuickMoveMessage *)pMessage;

            if (!CValidateANN(ptMsg->annItem))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            if (!PlayerCanManipulateInventory(tACDPlayer))
            {
                break;
            }

            ActorCommonData & tACDItem = ACDGet(idACDItem);

            InvLoc tLoc;
            tLoc.annOwner = ptMsg->annOwner;
            tLoc.eSlot = ptMsg->eSlot;
            tLoc.nSlotX = 0;
            tLoc.nSlotY = 0;
            if (!CValidateIsInventoryLocationValidToPlayer(tPlayerFrom, tLoc))
            {
                break;
            }

            if (ItemIsSocketed(tACDItem))
            {
                break;
            }

            if (!CValidateIsInventoryLocationValidForQuickMove(tACDItem, tLoc))
            {
                break;
            }

            ACDID idACDOwner = ACDFromANN(ptMsg->annOwner);
            InventoryLocation tInvLocDst;
            ACDInventoryLocationSet(tInvLocDst,
                                    idACDOwner,
                                    tLoc.eSlot,
                                    tLoc.nSlotX,
                                    tLoc.nSlotY);

            if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLocDst, FALSE, FALSE) != GAMEERROR_NONE)
            {
                break;
            }

            IVector2D tSizeSlot;
            ACDInventorySlotGetSize(tInvLocDst.eSlot, tSizeSlot);

            int32 nSlotYMinMax[2];
            nSlotYMinMax[0] = ptMsg->nSlotMinY;
            nSlotYMinMax[1] = ptMsg->nSlotMaxY;

            if (nSlotYMinMax[0] < 0 || nSlotYMinMax[1] < 0)
            {
                break;
            }
            if (nSlotYMinMax[0] > tSizeSlot.y || nSlotYMinMax[1] > tSizeSlot.y || nSlotYMinMax[0] > nSlotYMinMax[1])
            {
                break;
            }

            PlayerBrainTryInventoryQuickMoveItem(tACDPlayer, tACDItem, tInvLocDst, nSlotYMinMax);
        }
        break;

    case CMSG_TRY_INVENTORY_SOCKET:
        {
            InventoryRequestSocketMessage * ptMsg = (InventoryRequestSocketMessage *)pMessage;

            ACDID idACDGem = ACDFromANN(ptMsg->annGem);

            if (!CValidateItemIsInPlayerInventoryForSocketing(tPlayerFrom, idACDGem))
            {
                break;
            }

            ACDID idACDItemToReceiveGem = ACDFromANN(ptMsg->annItemToReceiveGem);

            if (!CValidateItemIsInPlayerInventoryForSocketing(tPlayerFrom, idACDItemToReceiveGem))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            if (!PlayerCanManipulateInventory(tACDPlayer))
            {
                break;
            }

            ActorCommonData & tACDGem = ACDGet(idACDGem);
            if (ItemIsSocketed(tACDGem))
            {
                break;
            }

            ActorCommonData & tACDItemToReceiveGem = ACDGet(idACDItemToReceiveGem);

            if (ItemIsUnidentified(tACDItemToReceiveGem))
            {
                break;
            }

            STradeCheckForModified(tACDGem, TRUE);

            if (!SACDInventoryProcessSocketRequest(tACDPlayer, tACDGem, tACDItemToReceiveGem))
            {
                break;
            }

            STradeCheckForModified(tACDItemToReceiveGem, FALSE);
        }
        break;

    case CMSG_TRY_INVENTORY_STACK:
        {
            InventoryRequestMoveMessage * ptMsg = (InventoryRequestMoveMessage *)pMessage;

            if (!CValidateANN(ptMsg->annItem))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            if (!PlayerCanManipulateInventory(tACDPlayer))
            {
                break;
            }

            if (!CValidateIsInventoryLocationValidToPlayer(tPlayerFrom, ptMsg->tLocation))
            {
                break;
            }

            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (ItemIsSocketed(tACDItem))
            {
                break;
            }

            ACDID idACDOwner = ACDFromANN(ptMsg->tLocation.annOwner);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem == NULL)
            {
                break;
            }

            if (ItemGetMaxStackSize(ptACDItem->hGB, NULL, NULL) < 2)
            {
                break;
            }

            STradeCheckForModified(*ptACDItem, TRUE);

            InventoryLocation tInvLoc;
            ACDInventoryLocationSet(tInvLoc,
                                    idACDOwner,
                                    ptMsg->tLocation.eSlot,
                                    ptMsg->tLocation.nSlotX,
                                    ptMsg->tLocation.nSlotY);

            if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLoc, FALSE, FALSE) != GAMEERROR_NONE)
            {
                break;
            }

            PlayerBrainTryInventoryStackItem(tACDPlayer, *ptACDItem, tInvLoc);
        }
        break;

    case CMSG_TRY_INVENTORY_DROP_STACK_PORTION:
        {
            StackPortionMessage * ptMsg = (StackPortionMessage *)pMessage;

            if (!CValidateANN(ptMsg->annItem))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);
            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            uint64 uStackSize = tACDItem.GetStackCount();
            if (ptMsg->uAmount == 0 || ptMsg->uAmount > uStackSize)
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            if (!PlayerCanManipulateInventory(tACDPlayer))
            {
                break;
            }

            if (ItemCanDrop(tACDItem) != GAMEERROR_NONE)
            {
                break;
            }

            STradeCheckForModified(tACDItem, TRUE);

            if (ptMsg->uAmount == uStackSize)
            {
                SACDInventoryProcessDropRequest(tPlayerFrom.idPlayerACD, tACDItem.id, TRUE);
            }
            else
            {
                SACDInventoryProcessDropStackPortionRequest(tPlayerFrom.idPlayerACD, tACDItem, ptMsg->uAmount);
            }
        }
        break;

    case CMSG_TRY_INVENTORY_SPLIT_STACK:
        {
            InventorySplitStackMessage * ptMsg = (InventorySplitStackMessage *)pMessage;

            if (!CValidateANN(ptMsg->annItem))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);
            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            uint64 uStackSize = tACDItem.GetStackCount();
            if (ptMsg->uAmount == 0 || ptMsg->uAmount >= uStackSize)
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            if (ptMsg->tLocation.eSlot != SLOT_PLAYER_SHARED_STASH && ptMsg->tLocation.eSlot != SLOT_PLAYER_BACKPACK)
            {
                break;
            }

            if (!CValidateIsInventoryLocationValidToPlayer(tPlayerFrom, ptMsg->tLocation))
            {
                break;
            }

            ACDID idACDOwner = ACDFromANN(ptMsg->tLocation.annOwner);

            if (tPlayerFrom.idPlayerACD != idACDOwner)
            {
                break;
            }

            InventoryLocation tInvLoc;
            ACDInventoryLocationSet(tInvLoc,
                                    idACDOwner,
                                    ptMsg->tLocation.eSlot,
                                    ptMsg->tLocation.nSlotX,
                                    ptMsg->tLocation.nSlotY);

            if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLoc, FALSE, FALSE) != GAMEERROR_NONE)
            {
                break;
            }

            ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLoc);
            if (idACDItemInSlot != ACDID_INVALID)
            {
                break;
            }

            STradeCheckForModified(tACDItem, FALSE);

            SACDInventoryProcessSplitStack(tACDItem, ptMsg->uAmount, tInvLoc);
        }
        break;

    case CMSG_DROP_CRAFTING_MATERIAL:
        {
            CurrencyMessage * ptMsg = (CurrencyMessage *)pMessage;

            if (ptMsg->eCurrency < CURRENCY_CRAFTING_MAT_START || ptMsg->eCurrency >= NUM_CURRENCY_TYPES)
            {
                break;
            }

            uint64 uAmount = ptACDPlayer->GetCurrencyAmount(ptMsg->eCurrency);
            if (uAmount < ptMsg->uAmount)
            {
                break;
            }

            if (!PlayerCanManipulateInventory(*ptACDPlayer))
            {
                break;
            }

            if (GameIsSeasonal())
            {
                break;
            }

            if (LootDropCraftingMaterial(ptACDPlayer, ptMsg->eCurrency, ptMsg->uAmount))
            {
                ptACDPlayer->ModifyCurrencyAmount(-(int64)ptMsg->uAmount, ptMsg->eCurrency, GMR_PICKUP, FALSE);
            }
        }
        break;

    case CMSG_TRY_INVENTORY_STACK_TRANSFER:
        {
            InventoryStackTransferMessage * ptMsg = (InventoryStackTransferMessage *)pMessage;

            if (ptMsg->annItemSrc == ptMsg->annItemDest)
            {
                break;
            }

            ACDID idACDItemSrc = ACDFromANN(ptMsg->annItemSrc);
            ActorCommonData * ptACDItemSrc = ACDTryToGet(idACDItemSrc);
            if (ptACDItemSrc == NULL)
            {
                break;
            }

            ACDID idACDItemDest = ACDFromANN(ptMsg->annItemDest);
            ActorCommonData * ptACDItemDest = ACDTryToGet(idACDItemDest);
            if (ptACDItemDest == NULL)
            {
                break;
            }

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItemSrc, FALSE))
            {
                break;
            }

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItemDest, FALSE))
            {
                break;
            }

            InventoryLocation & tInvLocationSrc = ptACDItemSrc->GetInventoryLocation();
            InventoryLocation & tInvLocationDest = ptACDItemDest->GetInventoryLocation();

            if (tInvLocationSrc.idACDContainer != tPlayerFrom.idPlayerACD ||
                tInvLocationDest.idACDContainer != tPlayerFrom.idPlayerACD)
            {
                break;
            }

            if (ptMsg->uAmount > ptACDItemSrc->GetStackCount())
            {
                break;
            }

            STradeCheckForModified(*ptACDItemSrc, TRUE);
            STradeCheckForModified(*ptACDItemDest, FALSE);

            SACDInventoryProcessStackTransfer(*ptACDItemSrc, *ptACDItemDest, ptMsg->uAmount);
        }
        break;

    case CMSG_TRY_GOLD_TRANSFER:
        {
            GoldTransferMessage * ptMsg = (GoldTransferMessage *)pMessage;

            ACDID idACDPlayerDst = ACDFromANN(ptMsg->annPlayer);
            ActorCommonData * ptACDPlayerDst = ACDTryToGet(idACDPlayerDst);
            if (ptACDPlayerDst == NULL)
            {
                break;
            }

            Player * ptPlayer = PlayerGetByACD(idACDPlayerDst);
            if (ptPlayer != NULL && SPlayerIsLoggingOut(FALSE, *ptPlayer))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            uint64 uSrcGold = tACDPlayer.GetCurrencyAmount(CURRENCY_GOLD);
            if (ptMsg->uAmount > uSrcGold)
            {
                break;
            }

            SACDInventoryProcessGoldTransfer(tACDPlayer, *ptACDPlayerDst, ptMsg->uAmount);
        }
        break;

    case CMSG_TRY_INVENTORY_USE:
        {
            SPlayerResetInactionTime(tPlayerFrom.idPlayerACD);

            InventoryRequestUseMessage * ptInvUseMsg = (InventoryRequestUseMessage *)pMessage;

            ACDID idACDItemToUse = ACDFromANN(ptInvUseMsg->annItemToUse);
            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItemToUse, TRUE))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            ActorCommonData & tACDItemToUse = ACDGet(idACDItemToUse);
            if (!ItemIsUsable(tACDItemToUse))
            {
                break;
            }

            ItemUseType eItemUseType = ItemGetUseType(tACDItemToUse);
            if (ItemUseIsTargettedInInventory(eItemUseType))
            {
                ACDID idACDTarget = ACDFromANN(ptInvUseMsg->annTarget);
                if (idACDTarget == ACDID_INVALID || idACDItemToUse == idACDTarget)
                {
                    break;
                }

                if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDTarget, FALSE))
                {
                    break;
                }
            }

            if (GameTypeIsPvP() && eItemUseType != ITEM_USE_HEALTHPOTION)
            {
                break;
            }

            if (!CValidateTargetType(ptInvUseMsg->eType))
            {
                break;
            }

            STradeCheckForModified(tACDItemToUse, TRUE);

            Target tTarget;
            tTarget.eType = ptInvUseMsg->eType;
            tTarget.idACDTarget = ACDFromANN(ptInvUseMsg->annTarget);
            tTarget.tPlace = ptInvUseMsg->tPlace;

            PlayerBrainTryInventoryUseItem(
                tPlayerFrom,
                ACDFromANN(ptInvUseMsg->annItemToUse),
                tTarget);
        }
        break;

    case CMSG_TRY_ASSIGN_SKILL_TO_SLOT:
        {
            AssignSkillMessage * ptMessage = (AssignSkillMessage *)pMessage;

            if (!CValidateSNO(POWER_SNO, ptMessage->snoPower))
            {
                break;
            }

            int32 nAvailableSlots = PlayerGetNumAvailableSkillSlots(*ptACDPlayer);
            if (ptMessage->nSlot < 0 || ptMessage->nSlot >= nAvailableSlots)
            {
                break;
            }

            if (!PowerIsValidForSlot(ptMessage->snoPower, ptMessage->nSlot))
            {
                break;
            }

            if (!ptACDPlayer->IsAlive())
            {
                break;
            }

            if (!PowerCanActorTrainActiveSkill(*ptACDPlayer, ptMessage->snoPower))
            {
                break;
            }

            if (!PowerIsCoolDownDone(*ptACDPlayer, ptMessage->snoPower))
            {
                break;
            }

            if (!PowerActorMeetsSkillRuneRequirement(*ptACDPlayer, ptMessage->snoPower, ptMessage->nRuneType))
            {
                break;
            }

            if (!PowerIsCoolDownDone(*ptACDPlayer, SPlayerGetActiveSkillInSlot(tPlayerFrom, ptMessage->nSlot)))
            {
                break;
            }

            if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
            {
                break;
            }

            if (GameIsWeeklyChallenge() &&
                (!ChallengeRift::IsValidChallengeRiftSkill(ptMessage->snoPower) || ChallengeRift::GetRuneTypeForSkill(ptMessage->snoPower) != ptMessage->nRuneType))
            {
                break;
            }

            BOOL bApplyCooldown = FALSE;
            SPlayerSwitchActiveSkills(tPlayerFrom, ptMessage->snoPower, ptMessage->nRuneType, ptMessage->nSlot, bApplyCooldown);
        }
        break;

    case CMSG_UNASSIGN_SKILL:
        {
            UnassignSkillMessage * ptMessage = (UnassignSkillMessage *)pMessage;

            if (!ptACDPlayer->IsAlive())
            {
                break;
            }

            if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
            {
                break;
            }

            SNO snoCurrentAssignment = SPlayerGetActiveSkillInSlot(tPlayerFrom, ptMessage->nSlot);

            if (snoCurrentAssignment != SNO_NONE)
            {
                if (!PowerIsCoolDownDone(*ptACDPlayer, snoCurrentAssignment))
                {
                    break;
                }
            }

            SPlayerRemoveActiveSkillInSlot(tPlayerFrom, ptMessage->nSlot);
        }
        break;

    case CMSG_SWAP_HOTBAR_SKILLS:
        {
            SwapSkillsMessage * ptMessage = (SwapSkillsMessage *)pMessage;

            ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayerFrom.idPlayerACD);
            if (ptACDPlayer == NULL)
            {
                break;
            }

            if (!ptACDPlayer->IsAlive())
            {
                break;
            }

            if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, TRUE))
            {
                break;
            }

            int32 nAvailableSlots = PlayerGetNumAvailableSkillSlots(*ptACDPlayer);
            if (ptMessage->nSlot0 < 0 || ptMessage->nSlot0 >= nAvailableSlots ||
                ptMessage->nSlot1 < 0 || ptMessage->nSlot1 >= nAvailableSlots)
            {
                break;
            }

            SNO snoCurrentAssignment0 = SPlayerGetActiveSkillInSlot(tPlayerFrom, ptMessage->nSlot0);
            if (snoCurrentAssignment0 != SNO_NONE && !PowerIsValidForSlot(AutoPowerDefinitionR(snoCurrentAssignment0), ptMessage->nSlot1))
            {
                break;
            }

            SNO snoCurrentAssignment1 = SPlayerGetActiveSkillInSlot(tPlayerFrom, ptMessage->nSlot1);
            if (snoCurrentAssignment1 != SNO_NONE && !PowerIsValidForSlot(AutoPowerDefinitionR(snoCurrentAssignment1), ptMessage->nSlot0))
            {
                break;
            }

            SPlayerSwapActiveSkillSlots(tPlayerFrom, ptMessage->nSlot0, ptMessage->nSlot1);
        }
        break;

    case CMSG_TRY_ASSIGN_TRAITS:
        {
            AssignTraitsMessage * ptMessage = (AssignTraitsMessage *)pMessage;

            if (!ptACDPlayer->IsAlive())
            {
                break;
            }

            if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
            {
                break;
            }

            if (GameIsWeeklyChallenge())
            {
                break;
            }

            int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);

            BOOL arHadPower[4];
            int32 nNumSuccesses = 0;
            for (int32 nSlot = 0; nSlot < 4; nSlot++)
            {
                if (ptMessage->snoPower[nSlot] != SNO_NONE)
                {
                    if (!CValidateSNO(POWER_SNO, ptMessage->snoPower[nSlot]))
                    {
                        return;
                    }

                    for (int32 i = 0; i < 4; i++)
                    {
                        if (i != nSlot &&
                            ptMessage->snoPower[i] == ptMessage->snoPower[nSlot] && ptMessage->snoPower[i] != SNO_NONE)
                        {
                            return;
                        }
                    }

                    if (!PowerCanActorTrainTrait(*ptACDPlayer, ptMessage->snoPower[nSlot]))
                    {
                        return;
                    }

                    if (nPlayerLevel < cgnTraitSlotLevelReq[nSlot])
                    {
                        return;
                    }

                    nNumSuccesses++;
                }

                SNO snoPowerOld = PlayerGetPowerInTraitSlot(tPlayerFrom, nSlot);
                arHadPower[nSlot] = snoPowerOld != SNO_NONE;

                if (ptMessage->snoPower[nSlot] != snoPowerOld)
                {
                    if (ptMessage->snoPower[nSlot] == SNO_NONE)
                    {
                        return;
                    }

                    if (!PowerIsCoolDownDone(*ptACDPlayer, ptMessage->snoPower[nSlot]))
                    {
                        return;
                    }

                    if (snoPowerOld != SNO_NONE && !PowerIsCoolDownDone(*ptACDPlayer, snoPowerOld))
                    {
                        return;
                    }
                }
            }

            if (nNumSuccesses == 0)
            {
                break;
            }

            for (int32 nSlot = 0; nSlot < 4; nSlot++)
            {
                SNO snoPowerOld = PlayerGetPowerInTraitSlot(tPlayerFrom, nSlot);
                SNO snoPowerIncomingToThisSlot = ptMessage->snoPower[nSlot];
                if (snoPowerIncomingToThisSlot != snoPowerOld)
                {
                    for (int32 nSlotExisting = 0; nSlotExisting < 4; nSlotExisting++)
                    {
                        if (nSlotExisting != nSlot)
                        {
                            SNO snoPowerInSlot = PlayerGetPowerInTraitSlot(tPlayerFrom, nSlotExisting);
                            if (snoPowerInSlot == snoPowerIncomingToThisSlot)
                            {
                                SPlayerRemoveTraitInSlot(tPlayerFrom, nSlotExisting);
                            }
                        }
                    }

                    SPlayerSwitchTraits(tPlayerFrom, snoPowerIncomingToThisSlot, nSlot, arHadPower[nSlot]);
                }
            }
        }
        break;

    case CMSG_TRY_ASSIGN_LEGENDARY_POWER:
        {
            AssignLegendaryPowerMessage * ptMessage = (AssignLegendaryPowerMessage *)pMessage;

            if ((uint32)ptMessage->nSlot > 2)
            {
                break;
            }

            if (ptMessage->gbidLegendaryItem != GBID_NONE)
            {
                if (!GBIDExists(ptMessage->gbidLegendaryItem, GB_ITEMS))
                {
                    break;
                }

                if (ItemGetLegendaryPower(ptMessage->gbidLegendaryItem) == SNO_NONE)
                {
                    break;
                }

                if (!ItemPlayerHasExtractedLegendaryPower(tPlayerFrom, ptMessage->gbidLegendaryItem))
                {
                    break;
                }

                if (!ItemPlayerCanAssignLegendaryPower(tPlayerFrom, ptMessage->gbidLegendaryItem, ptMessage->nSlot))
                {
                    break;
                }
            }

            if (!ptACDPlayer->IsAlive())
            {
                break;
            }

            if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
            {
                break;
            }

            SPlayerAssignToLegendaryPowerSlot(tPlayerFrom, ptMessage->gbidLegendaryItem, ptMessage->nSlot);
        }
        break;

    case CMSG_TRY_CHOOSE_HIRELING_SKILL:
        {
            HirelingRequestLearnSkillMessage * ptMessage = (HirelingRequestLearnSkillMessage *)pMessage;
            ACDID idACDHireling = ACDFromANN(ptMessage->ann);

            ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
            if (ptACDHireling == NULL || !ptACDHireling->IsMyPet(nPlayerIndex))
            {
                break;
            }

            if (!CValidateSNO(POWER_SNO, ptMessage->snoSkill))
            {
                break;
            }

            if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
            {
                break;
            }

            SHirelingTryChooseSkill(*ptACDHireling, ptMessage->snoSkill);
        }
        break;

    case CMSG_VENDOR_CLOSE:
        NPCVendorStop(tPlayerFrom);
        break;

    case CMSG_TRY_DROP:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMessage->ann))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMessage->ann);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            if (!tACDPlayer.IsAlive())
            {
                break;
            }

            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (ItemIsSocketed(tACDItem))
            {
                break;
            }

            if (TradeIsTrading(tPlayerFrom.idPlayerACD))
            {
                break;
            }

            if (SPlayerInScriptedSequence(tPlayerFrom))
            {
                break;
            }

            if (ItemCanDrop(tACDItem) != GAMEERROR_NONE)
            {
                break;
            }

            if (!SACDInventoryProcessDropRequest(tPlayerFrom.idPlayerACD, idACDItem, TRUE))
            {
                PlayerDisplayErrorMessage(tACDPlayer.id, 130 /* GAMEERROR_CANNOT_DO_THAT_RIGHT_NOW */);
            }
        }
        break;

    case CMSG_TRY_PORTRAIT_DROP:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            if (PlayerGetHireling(tPlayerFrom.idPlayerACD) == ACDID_INVALID)
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMessage->ann);
            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            SACDInventoryProcessDropToPortraitRequest(tPlayerFrom.idPlayerACD, idACDItem);
        }
        break;

    case CMSG_SHOP_TRY_BUY:
        {
            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMsg->ann))
            {
                break;
            }

            if (!CValidatePlayerNearCrafter(tPlayerFrom, -1))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->ann);
            ActorCommonData & tACDItem = ACDGet(idACDItem);
            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

            SACDInventoryBuy(tACDPlayer, tACDItem);
        }
        break;

    case CMSG_SHOP_TRY_SELL:
        {
            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMsg->ann))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->ann);
            if (!CValidateItemCanBeSold(tPlayerFrom, idACDItem))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem != NULL)
            {
                SACDInventorySell(tACDPlayer, *ptACDItem);
            }
        }
        break;

    case CMSG_SHOP_TRY_SELL_STACK_PORTION:
        {
            StackPortionMessage * ptMsg = (StackPortionMessage *)pMessage;

            if (!CValidateANN(ptMsg->annItem))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);
            if (!CValidateItemCanBeSold(tPlayerFrom, idACDItem))
            {
                break;
            }

            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem != NULL)
            {
                uint64 uStackSize = ptACDItem->GetStackCount();
                if (ptMsg->uAmount == 0 || ptMsg->uAmount > uStackSize)
                {
                    break;
                }

                ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
                if (ptMsg->uAmount == uStackSize)
                {
                    SACDInventorySell(tACDPlayer, *ptACDItem);
                }
                else
                {
                    SACDInventorySellStackPortion(tACDPlayer, *ptACDItem, ptMsg->uAmount);
                }
            }
        }
        break;

    case CMSG_PLAYER_CHANGE_POTION_BUTTON:
        {
            ChangeUsableItemMessage * ptPotionChangeMsg = (ChangeUsableItemMessage *)pMessage;

            if (!CValidateGBIDAllowNone(GB_ITEMS, ptPotionChangeMsg->gbidItem))
            {
                return;
            }

            HotbarButtonData tButtonData;
            if (ptPotionChangeMsg->gbidItem == GBID_NONE)
            {
                if (ptPotionChangeMsg->annItem != ANN_INVALID)
                {
                    if (!CValidateANN(ptPotionChangeMsg->annItem))
                    {
                        return;
                    }

                    ACDID idACDItem = ACDFromANN(ptPotionChangeMsg->annItem);
                    ActorCommonData & tACDItem = ACDGet(idACDItem);
                    if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, TRUE))
                    {
                        return;
                    }

                    if (!ItemCanBeAssignedToUsableItemButton(tACDItem) ||
                        ItemIsUnidentified(tACDItem))
                    {
                        return;
                    }
                }

                tButtonData.SetItemANN(ptPotionChangeMsg->annItem);
            }
            else
            {
                if (!ItemHasLabel(ptPotionChangeMsg->gbidItem, ITL_POTION))
                {
                    return;
                }

                tButtonData.SetItemGBID(ptPotionChangeMsg->gbidItem);
            }

            PlayerSetPotionBtnAssignment(&tPlayerFrom, tButtonData);
        }
        break;

    case CMSG_REQUEST_CONV_ADVANCE:
        {
            AdvanceConvMessage * ptMsg = (AdvanceConvMessage *)pMessage;
            ConversationMgr::AdvanceConversationForPlayer(tPlayerFrom, ptMsg->snoConv, ptMsg->nLineUID);
        }
        break;

    case CMSG_UPDATE_CONV_AUTO_ADVANCE:
        {
            UpdateConvAutoAdvanceMessage * ptMsg = (UpdateConvAutoAdvanceMessage *)pMessage;
            ConversationMgr::UpdateConversationAutoAdvanceForPlayer(tPlayerFrom, ptMsg->snoConv, ptMsg->nLineUID, ptMsg->dwNextAdvanceTime);
        }
        break;

    case CMSG_REQUEST_CONV_CANCEL:
        ConversationMgr::CancelConversationForPlayer(tPlayerFrom);
        break;

    case CMSG_REQUEST_HIRELING_SWAP:
        SHirelingHandleSwap(tPlayerFrom);
        break;

    case CMSG_INSPECT_START:
        {
            PlayerIndexMessage * tInspect = (PlayerIndexMessage *)pMessage;

            Player * ptPlayer = PlayerGetByPlayerIndex(tInspect->nPlayerIndex);
            if (ptPlayer != NULL)
            {
                SPlayerInspect(tPlayerFrom.idPlayerACD, ptPlayer->idPlayerACD);
            }
        }
        break;

    case CMSG_INSPECT_END:
        SPlayerInspectEnd(tPlayerFrom.idPlayerACD);
        break;

    case CMSG_NPC_INTERACT_TALK_DIALOG:
        {
            SNODataMessage * ptMsg = (SNODataMessage *)pMessage;

            if (!CValidateSNO(CONVERSATION_SNO, ptMsg->snoData))
            {
                break;
            }

            ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayerFrom.tTalkData.idACDTalkingTo);

            BOOL bPetOfPlayer = FALSE;
            if (ptACDTalkingTo != NULL && ptACDTalkingTo->IsPet())
            {
                bPetOfPlayer = ptACDTalkingTo->GetPetOwnerPlayer() == &tPlayerFrom;
            }

            if (!bPetOfPlayer && !CValidatePlayerNearInteractNPC(tPlayerFrom))
            {
                break;
            }

            ActorCommonData & tACDTalkingTo = ACDGet(tPlayerFrom.tTalkData.idACDTalkingTo);
            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

            if (!NPCCanOperate(tACDTalkingTo, tACDPlayer, FALSE))
            {
                break;
            }

            ConversationArray tConvArray;
            NPCGetConversations(tACDPlayer.id, tPlayerFrom.tTalkData.idACDTalkingTo, tConvArray);

            BOOL bFound = FALSE;

            for (uint32 i = 0; i < tConvArray.size(); i++)
            {
                SNO snoConv = tConvArray[i];
                if (snoConv == SNO_NONE)
                {
                    continue;
                }

                ScopedConversationDefinitionLockR ptConv(snoConv);

                uint32 dwUsageFlags = ConversationTypeGetUsageFlags(ptConv->eConvType);
                if (dwUsageFlags & (1 << CTU_SHOWN_IN_TALK_MENU_BIT))
                {
                    if (snoConv == ptMsg->snoData)
                    {
                        bFound = TRUE;
                        break;
                    }
                }
            }

            if (bFound)
            {
                NPCStartConversation(tACDPlayer, tACDTalkingTo, ptMsg->snoData, NIT_TALK_DIALOG);

                STalkResetTalkingTo(tPlayerFrom);
            }
        }
        break;

    case CMSG_NPC_INTERACT_PET_DISMISS:
        {
            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMsg->ann))
            {
                break;
            }

            ACDID idACDPet = ACDFromANN(ptMsg->ann);
            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            ActorCommonData & tACDPet = ACDGet(idACDPet);

            if (tACDPet.GetPetOwnerPlayer() == NULL)
            {
                break;
            }

            if (ACDGetControllingPlayer(idACDPet) != &tPlayerFrom)
            {
                break;
            }

            if (!PlayerPetIsDismissableByPlayer(tACDPet))
            {
                break;
            }

            DismissPet(tACDPlayer, tACDPet, TRUE);
        }
        break;

    case CMSG_NPC_INTERACT_CRAFT:
        {
            if (!CValidatePlayerNearInteractNPC(tPlayerFrom))
            {
                break;
            }

            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
            ActorCommonData & tACDVendor = ACDGet(tPlayerFrom.tTalkData.idACDTalkingTo);

            if (NPCGetCrafterType(tACDVendor) == CRAFTER_TYPE_INVALID &&
                !NPCCanTrade(tACDVendor))
            {
                break;
            }

            NPCOperateVendor(tACDVendor, tACDPlayer);
        }
        break;

    case CMSG_NPC_INTERACT_HIRE:
        {
            if (!CValidatePlayerNearInteractNPC(tPlayerFrom))
            {
                break;
            }

            if (!SHirelingCanHire(tPlayerFrom))
            {
                break;
            }

            ActorCommonData & tACDTalkingTo = ACDGet(tPlayerFrom.tTalkData.idACDTalkingTo);

            HirelingClass eClass = ACDGetHirelingClass(tACDTalkingTo);
            if (eClass == HC_NONE)
            {
                break;
            }

            if (!HirelingTestTypeAvailable(tPlayerFrom.idPlayerACD, eClass) && !tACDTalkingTo.IsKnownByPlayer(tPlayerFrom))
            {
                break;
            }

            {
                ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
                SHirelingCreate(tACDPlayer, tACDTalkingTo, eClass, TRUE, FALSE);
            }

            STalkResetTalkingTo(tPlayerFrom);
        }
        break;

    case CMSG_NPC_INTERACT_INVENTORY:
        {
            if (!CValidatePlayerNearInteractNPC(tPlayerFrom))
            {
                break;
            }

            ActorCommonData & tACDTalkingTo = ACDGet(tPlayerFrom.tTalkData.idACDTalkingTo);

            HirelingClass eClass = ACDGetHirelingClass(tACDTalkingTo);
            if (eClass == HC_NONE)
            {
                break;
            }

            if (!tACDTalkingTo.IsKnownByPlayer(tPlayerFrom))
            {
                break;
            }

            {
                ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);
                SHirelingCreate(tACDPlayer, tACDTalkingTo, eClass, TRUE, TRUE);
            }

            STalkResetTalkingTo(tPlayerFrom);
        }
        break;

    case CMSG_NPC_INTERACT_CANCEL:
        STalkResetTalkingTo(tPlayerFrom);
        break;

    case CMSG_NPC_TALK_REQUEST:
        {
            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMsg->ann))
            {
                break;
            }

            ACDID idACDNPC = ACDFromANN(ptMsg->ann);
            ActorCommonData & tACDNPC = ACDGet(idACDNPC);
            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

            if (!NPCCanOperate(tACDNPC, tACDPlayer, FALSE))
            {
                break;
            }

            NPCOperate(tACDNPC, tACDPlayer, TRUE, FALSE, TRUE, FALSE);
        }
        break;

    case CMSG_NPC_VENDOR_REQUEST:
        {
            const ANNDataMessage * ptMsg = (const ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMsg->ann))
            {
                break;
            }

            ACDID idACDNPC = ACDFromANN(ptMsg->ann);
            ActorCommonData & tACDNPC = ACDGet(idACDNPC);
            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

            if (!NPCCanOperate(tACDNPC, tACDPlayer, FALSE))
            {
                break;
            }

            NPCOperate(tACDNPC, tACDPlayer, FALSE, FALSE, TRUE, TRUE);
        }
        break;

    case CMSG_GAME_END_REQUEST:
        {
            const SNODataMessage * ptMsg = (const SNODataMessage *)pMessage;
            if (CValidateEndGameRequest(ptMsg->snoData, tPlayerFrom))
            {
                SGameStartGameEnd(tPlayerFrom);
            }
        }
        break;

    case CMSG_ACT_TRANSITION_REQUEST:
        {
            const SNODataMessage * ptMsg = (const SNODataMessage *)pMessage;
            if (CValidateActTransitionRequest(ptMsg->snoData, tPlayerFrom))
            {
                SActTransitionStart(&tPlayerFrom, FALSE);
            }
        }
        break;

    case CMSG_TRADE:
        {
            TradeMessage * ptMsg = (TradeMessage *)pMessage;
            STradeHandleMessage(tPlayerFrom, *ptMsg);
        }
        break;

    case CMSG_TRY_IDENTIFY_WITH_CAST:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMessage->ann))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMessage->ann);
            ActorCommonData & tACDItem = ACDGet(idACDItem);
            ActorCommonData & tACDPlayer = ACDGet(tPlayerFrom.idPlayerACD);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            if (!ItemIsUnidentified(tACDItem))
            {
                break;
            }

            SItemPlayerIdentify(tACDItem, &tACDPlayer);
            SACDInventoryAddToDirtyList(tACDItem, AIDA_UPDATE_IDENTIFY, tACDItem.GetLocationSlot(), tACDItem.GetLocationSlot());
        }
        break;

    case CMSG_TRY_IDENTIFY_ALL_WITH_CAST:
        {
            ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayerFrom.tTalkData.idACDTalkingTo);
            if (ptACDTalkingTo == NULL || !NPCCanIdentify(*ptACDTalkingTo))
            {
                break;
            }

            SPlayerTryIdentifyAllWithCast(tPlayerFrom.idPlayerACD, ACDID_INVALID);
        }
        break;

    case CMSG_TELEPORT_TO_PLAYER:
        {
            PlayerIndexMessage * ptMessage = (PlayerIndexMessage *)pMessage;

            Player * ptPlayerTarget = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            if (ptPlayerTarget == NULL)
            {
                break;
            }

            if (SPlayerCheckCanWarpToOtherPlayer(&tPlayerFrom, ptPlayerTarget))
            {
                SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_TELEPORT_TO_PLAYER_CAST);
                if (ACDIsInTown(*ptACDPlayer))
                {
                    snoPower = GlobalSNOGet(GLOBALSNO_POWERS_TELEPORT_TO_PLAYER);
                }

                PowerID idPowerPrimary = ActorPowersGetPrimary((ActorID)ACDGetOwner(tPlayerFrom.idPlayerACD));
                Power * ptPowerPrimary = PowerTryToGet(idPowerPrimary);
                if (ptPowerPrimary != NULL && ptPowerPrimary->snoPower == snoPower &&
                    ptPowerPrimary->dwParam == (uint32)ptPlayerTarget->idPlayerACD)
                {
                    break;
                }

                Target tTarget;
                tTarget.Clear();

                uint32 uFailureReason = PowerFailureReasons(*ptACDPlayer, AutoPowerDefinitionR(snoPower), &tTarget, FALSE, FALSE);
                if (uFailureReason & ~(1 << PS_NOT_CLOSE_ENOUGH_BIT))
                {
                    break;
                }

                PowerStartData tPowerStartData;
                PowerStartDataInit(tPowerStartData);
                tPowerStartData.idACD = tPowerStartData.idACDAlternateSource = tPlayerFrom.idPlayerACD;
                tPowerStartData.snoPower = snoPower;
                tPowerStartData.dwParam = ptPlayerTarget->idPlayerACD;

                PowerID idPower = PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
                Power * ptPower = PowerTryToGet(idPower);
                if (ptPower != NULL)
                {
                    PowerHelper_DoFirstTimeCosts(idPower, TRUE);
                }
            }
        }
        break;

    case CMSG_TELEPORT_TO_WAYPOINT:
        {
            DWordDataMessage * ptMsg = (DWordDataMessage *)pMessage;
            uint32 uWaypointIndex = ptMsg->dwData;

            if (!ptACDPlayer->IsAlive())
            {
                return;
            }

            {
                const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
                size_t nCount = tWaypoints.size();

                if (uWaypointIndex >= nCount ||
                    !SGameGlobalsGet().WaypointIsActive(uWaypointIndex))
                {
                    return;
                }
            }

            if (!SPlayerCheckCanWarpToWaypoint(&tPlayerFrom, uWaypointIndex))
            {
                return;
            }

            SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_TELEPORT_TO_WAYPOINT_CAST);
            if (ACDIsInTown(*ptACDPlayer))
            {
                snoPower = GlobalSNOGet(GLOBALSNO_POWERS_TELEPORT_TO_WAYPOINT);
            }

            PowerID idPowerPrimary = ActorPowersGetPrimary((ActorID)ACDGetOwner(tPlayerFrom.idPlayerACD));
            Power * ptPowerPrimary = PowerTryToGet(idPowerPrimary);
            if (ptPowerPrimary != NULL &&
                ptPowerPrimary->snoPower == snoPower &&
                ptPowerPrimary->dwParam == uWaypointIndex)
            {
                return;
            }

            uint32 uFailureReason = PowerFailureReasons(*ptACDPlayer, AutoPowerDefinitionR(snoPower), &cgTargetNone, FALSE, FALSE);
            if (uFailureReason & ~(1 << PS_NOT_CLOSE_ENOUGH_BIT))
            {
                return;
            }

            PowerStartData tPowerStartData;
            PowerStartDataInit(tPowerStartData);
            tPowerStartData.idACD = tPowerStartData.idACDAlternateSource = tPlayerFrom.idPlayerACD;
            tPowerStartData.snoPower = snoPower;
            tPowerStartData.dwParam = uWaypointIndex;

            PowerID idPower = PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
            Power * ptPower = PowerTryToGet(idPower);
            if (ptPower != NULL)
            {
                PowerHelper_DoFirstTimeCosts(idPower, TRUE);
            }
        }
        break;

    case CMSG_SHOWN_TUTORIAL:
        {
            SNODataMessage * ptMessage = (SNODataMessage *)pMessage;
            if (CValidateSNO(TUTORIAL_SNO, ptMessage->snoData))
            {
                Tutorial::Shown(tPlayerFrom, ptMessage->snoData);
            }
        }
        break;

    case CMSG_FINISHED_OPEN_WORLD_TUTORIAL:
        ScriptEventOnUserEvent("OpenWorldTutorialFinished", tPlayerFrom.idPlayerACD, 0);

        if (!(tPlayerFrom.tAccount.uDigestFlags & (1 << D3::Account::Digest_Flags_ADVENTURE_MODE_TUTORIAL_PLAYED)))
        {
            tPlayerFrom.tAccount.uDigestFlags |= (1 << D3::Account::Digest_Flags_ADVENTURE_MODE_TUTORIAL_PLAYED);
            SPlayerAddToSaveList(tPlayerFrom, 0, PlayerSaveReason_AcctFlags, PLAYER_SAVE_MODE_END_OF_FRAME);

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, tPlayerFrom);

            DWordDataMessage tMessage;
            tMessage.dwData = tPlayerFrom.tAccount.uDigestFlags;
            MessageSendToClient(listDestPlayers, SMSG_ACCOUNT_FLAGS_SYNC, &tMessage, sizeof(tMessage));
        }
        break;

    case CMSG_TRY_REMOVE_GEMS:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;
            ACDID idACDItem = ACDFromANN(ptMessage->ann);

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_JEWEL))
            {
                break;
            }

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE) && !CValidateItemIsInPlayerStash(tPlayerFrom, idACDItem))
            {
                break;
            }

            SItemCrafting_RemoveGems(ACDGet(idACDItem), tPlayerFrom);
        }
        break;

    case CMSG_SHARED_STASH_TRY_BUY:
        if (CValidatePlayerNearStash(tPlayerFrom))
        {
            SACDInventoryBuyStashSlots(tPlayerFrom.idPlayerACD);
        }
        break;

    case CMSG_TRY_SALVAGE:
        {
            ANNDataMessage * ptMSG = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMSG->ann))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMSG->ann);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_BLACKSMITH) &&
                !CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_JEWEL) &&
                !CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            SSalvage::TrySalvage(tPlayerFrom, ACDGet(idACDItem));
        }
        break;

    case CMSG_TRY_SALVAGE_ALL:
        {
            TrySalvageAllMessage * ptMessage = (TrySalvageAllMessage *)pMessage;
            if ((uint32)ptMessage->eSalvageType > 2)
            {
                break;
            }

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_BLACKSMITH) &&
                !CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_JEWEL) &&
                !CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            const ActorCommonData * ptACDNPC = ACDTryToGet(tPlayerFrom.idACDActiveVendor);
            CrafterType eCrafterType = NPCGetCrafterType(*ptACDNPC);

            int32 nCrafterLevel = GetCraftLevel(eCrafterType, tPlayerFrom);
            int32 nCrafterMaxLevel = CrafterGetMaxLevel(eCrafterType);
            if (nCrafterLevel < nCrafterMaxLevel && !GameIsAdventureMode())
            {
                break;
            }

            SSalvage::TrySalvageAll(tPlayerFrom, ptMessage->eSalvageType);
        }
        break;

    case CMSG_PLAYER_BUSY:
        {
            BoolDataMessage * ptMsg = (BoolDataMessage *)pMessage;
            BOOL bBusy = ptMsg->bVal;

            ptACDPlayer->AttributesSetInt(ATTRIBUTE_BUSY, bBusy);
        }
        break;

    case CMSG_PLAYER_LORE_PLAYING:
        {
            BoolDataMessage * ptMsg = (BoolDataMessage *)pMessage;
            BOOL bLorePlaying = ptMsg->bVal;

            for (Player * ptCurrPlayer = PlayerGetFirstInGame(); ptCurrPlayer != NULL; ptCurrPlayer = PlayerGetNextInGame(ptCurrPlayer))
            {
                if (PlayersAreLocalToEachOther(*ptCurrPlayer, tPlayerFrom))
                {
                    ActorCommonData * ptScopedACDPlayer = ACDTryToGet(ptCurrPlayer->idPlayerACD);
                    if (ptScopedACDPlayer != NULL)
                    {
                        ptScopedACDPlayer->AttributesSetInt(ATTRIBUTE_LORE_PLAYING, bLorePlaying);
                    }
                }
            }
        }
        break;

    case CMSG_BOSS_JOIN_DECLINED:
        {
            BossEncounterManager::PlayerDeclineInvite(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    BossEncounterManager::PlayerDeclineInvite(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_BOSS_JOIN_ACCEPTED:
        {
            BossEncounterManager::PlayerAcceptInvite(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    BossEncounterManager::PlayerAcceptInvite(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_BOSS_VOTE_CONTINUE:
        {
            BossEncounterManager::PlayerVoteContinue(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    BossEncounterManager::PlayerVoteContinue(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_BOSS_VOTE_CANCEL:
        {
            BossEncounterManager::PlayerVoteCancel(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    BossEncounterManager::PlayerVoteCancel(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_BOSS_START_ACCEPTED:
        {
            BossEncounterManager::PlayerAcceptStart(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    BossEncounterManager::PlayerAcceptStart(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_BOSS_START_DECLINED:
        {
            BossEncounterManager::PlayerDeclineStart(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    BossEncounterManager::PlayerDeclineStart(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_RIFT_START_ACCEPTED:
        {
            Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
            if (ptPlayer != NULL && SetEncounterManager::IsWaitingForPlayerToVote(ptPlayer))
            {
                break;
            }

            const IntDataMessage * ptRiftMessage = (const IntDataMessage *)pMessage;
            BOOL bEmpowered = FALSE;

            int32 nRiftLevel = ptRiftMessage->nData;
            if (nRiftLevel > 0)
            {
                bEmpowered = (nRiftLevel >> 30) & 1;
                nRiftLevel &= ~(1 << 30);
            }

            if (nRiftLevel >= 0 &&
                ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) != 70)
            {
                break;
            }

            GameCommonData & tGCD = GameGetActiveGameCommonData();
            int32 nCurrentLevel = tGCD.tGameSyncedData.nMonsterLevel;
            if (nRiftLevel >= 0 && nCurrentLevel != 70)
            {
                break;
            }

            ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

            if (nRiftLevel < -1 ||
                nRiftLevel >= (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
            {
                break;
            }

            if (bEmpowered)
            {
                if (nRiftLevel < 0)
                {
                    break;
                }

                uint64 nEmpowerCost = NephalemRift::GetEmpoweredRiftCost(nRiftLevel);
                if (nEmpowerCost > (uint64)ptACDPlayer->GetCurrencyAmount(CURRENCY_GOLD))
                {
                    break;
                }
            }

            NephalemRiftManager::PlayerAcceptStart(nRiftLevel, &tPlayerFrom, bEmpowered);
        }
        break;

    case CMSG_RIFT_START_DECLINED:
        NephalemRiftManager::PlayerDeclineStart(&tPlayerFrom);
        break;

    case CMSG_RIFT_JOIN_DECLINED:
        NephalemRiftManager::PlayerDeclineInvite(&tPlayerFrom);
        break;

    case CMSG_RIFT_JOIN_ACCEPTED:
        {
            const BoolDataMessage * ptRiftMessage = (const BoolDataMessage *)pMessage;
            NephalemRiftManager::PlayerAcceptInvite(&tPlayerFrom, ptRiftMessage->bVal);
        }
        break;

    case CMSG_RIFT_JOIN_INFO_REQUEST:
        NephalemRiftManager::PlayerRequestedJoinInfo(&tPlayerFrom);
        break;

    case CMSG_RIFT_VOTE_CANCEL:
        NephalemRiftManager::PlayerVoteCancel(&tPlayerFrom);
        break;

    case CMSG_RIFT_VOTE_CONTINUE:
        {
            const BoolDataMessage * ptRiftMessage = (const BoolDataMessage *)pMessage;
            NephalemRiftManager::PlayerVoteContinue(&tPlayerFrom, ptRiftMessage->bVal);
        }
        break;

    case CMSG_RIFT_PAYMENT_ACCEPTED:
        {
            const BoolDataMessage * ptBoolMessage = (const BoolDataMessage *)pMessage;
            NephalemRiftManager::PlayerPaymentWarningAccepted(&tPlayerFrom, ptBoolMessage->bVal);
        }
        break;

    case CMSG_SET_DUNGEON_ACCEPT:
        SetEncounterManager::PlayerAcceptStart(&tPlayerFrom);
        break;

    case CMSG_SET_DUNGEON_JOIN_ACCEPTED:
        {
            SetEncounterManager::PlayerAcceptInvite(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    SetEncounterManager::PlayerAcceptInvite(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_SET_DUNGEON_VOTE_CONTINUE:
        {
            SetEncounterManager::PlayerVoteContinue(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    SetEncounterManager::PlayerVoteContinue(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_SET_DUNGEON_VOTE_CANCEL:
        SetEncounterManager::PlayerVoteCancel(&tPlayerFrom);
        break;

    case CMSG_SET_DUNGEON_JOIN_DECLINED:
        {
            SetEncounterManager::PlayerDeclineInvite(&tPlayerFrom);

            for (Player * ptPlayerCompare = PlayerGetFirstInGame(); ptPlayerCompare != NULL; ptPlayerCompare = PlayerGetNextInGame(ptPlayerCompare))
            {
                if (PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerCompare))
                {
                    SetEncounterManager::PlayerDeclineInvite(ptPlayerCompare);
                }
            }
        }
        break;

    case CMSG_TRY_CRAFT_ITEM:
    case CMSG_TRY_CRAFT_ITEMS:
        {
            GBID gbidRecipe = GBID_NONE;
            int32 nCount = 1;
            if (ptHeader->eType == CMSG_TRY_CRAFT_ITEM)
            {
                GBIDDataMessage * ptMessage = (GBIDDataMessage *)pMessage;
                gbidRecipe = ptMessage->gbidData;
            }
            else if (ptHeader->eType == CMSG_TRY_CRAFT_ITEMS)
            {
                CraftItemsMessage * ptMessage = (CraftItemsMessage *)pMessage;
                gbidRecipe = ptMessage->gbidRecipe;
                nCount = ptMessage->nCount;
            }

            if (!CValidateGBID(GB_RECIPES, gbidRecipe) ||
                nCount < 1 || nCount > 256)
            {
                break;
            }

            ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);

            if (pdefRecipeEntry->eCrafterType < 0 || pdefRecipeEntry->eCrafterType > CRAFTER_TYPE_LAST_IMPROVEABLE)
            {
                break;
            }

            if (!CValidatePlayerNearCrafter(tPlayerFrom, pdefRecipeEntry->eCrafterType))
            {
                break;
            }

            if (!ItemCrafting_PlayerKnowsRecipe(tPlayerFrom, gbidRecipe))
            {
                break;
            }

            nCount = Math::Min(nCount, 256);
            for (int32 i = 0; i < nCount; i++)
            {
                ACDID idACDItem = SItemCrafting_TryCraftItem(tPlayerFrom, gbidRecipe);
                ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
                if (ptACDItem == NULL || !ptACDItem->IsInInventory())
                {
                    break;
                }
            }
        }
        break;

    case CMSG_TRY_REFORGE:
        {
            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->ann);
            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            SItemCrafting_Reforge(idACDItem, tPlayerFrom);
        }
        break;

    case CMSG_TRY_ENCHANT_AFFIX:
        {
            EnchantAffixMessage * ptMsg = (EnchantAffixMessage *)pMessage;

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            if (!GBIDExists(ptMsg->gbidAffixToReroll, 8 /* GB_AFFIXES */))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem == NULL || !CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            if (ItemIsUnidentified(*ptACDItem))
            {
                break;
            }

            if (ItemCanEnchantAffix(*ptACDItem) != 0)
            {
                break;
            }

            if (ItemHasEnchantedAffix(*ptACDItem))
            {
                GBID gbidEnchantedAffixOld = ItemGetEnchantedAffixOld(*ptACDItem);
                if (gbidEnchantedAffixOld != ptMsg->gbidAffixToReroll)
                {
                    break;
                }
            }

            SItemCrafting_EnchantItemAffixStart(idACDItem, ptMsg->gbidAffixToReroll, tPlayerFrom);
        }
        break;

    case CMSG_CHOOSE_ENCHANT_AFFIX:
        {
            IntDataMessage * ptMsg = (IntDataMessage *)pMessage;

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            ACDID idACDPendingItem = tPlayerFrom.tVendorData.idACDEnchantAffixPendingItem;
            ActorCommonData * ptACDItem = ACDTryToGet(idACDPendingItem);
            if (ptACDItem == NULL || !CValidateItemIsInPlayerInventory(tPlayerFrom, idACDPendingItem, FALSE))
            {
                break;
            }

            if (ptMsg->nData < 0 || ptMsg->nData >= tPlayerFrom.tVendorData.nEnchantAffixChoicesCount)
            {
                break;
            }

            SItemCrafting_EnchantItemAffixChoose(idACDPendingItem, ptMsg->nData, tPlayerFrom);
        }
        break;

    case CMSG_TRY_REPAIR:
        {
            if (!CValidatePlayerNearTradeNPC(tPlayerFrom))
            {
                break;
            }

            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;
            ACDID idACDItem = ACDFromANN(ptMsg->ann);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            if (SItemCrafting_TryRepair(idACDItem, tPlayerFrom))
            {
                SimpleMessage tMessage;
                PlayerList listDestPlayers(PlayerListGetElementPool());
                listDestPlayers.AddTail_Tracked(&tPlayerFrom);
                MessageSendToClient(listDestPlayers, SMSG_REPAIR_ACK, &tMessage, sizeof(tMessage));
            }
        }
        break;

    case CMSG_TRY_REPAIR_EQUIPPED:
        if (CValidatePlayerNearTradeNPC(tPlayerFrom))
        {
            SItemCrafting_TryRepairAll(tPlayerFrom, TRUE);
        }
        break;

    case CMSG_TRY_REPAIR_ALL:
        if (CValidatePlayerNearTradeNPC(tPlayerFrom))
        {
            SItemCrafting_TryRepairAll(tPlayerFrom, FALSE);
        }
        break;

    case CMSG_TRY_TRANSMOG:
        {
            TransmogItemMessage * ptMsg = (TransmogItemMessage *)pMessage;

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem == NULL || !CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            SItemCrafting_TryTransmog(idACDItem, ptMsg->gbidTransmog, tPlayerFrom);
        }
        break;

    case CMSG_TRY_DYE_ITEM:
        {
            DyeItemMessage * ptMsg = (DyeItemMessage *)pMessage;

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMsg->annItem);
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem == NULL || !CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            if (ptMsg->eDyeType == DYE_TYPE_20 || ptMsg->eDyeType == DYE_TYPE_13)
            {
                if (!CValidatePlayerHasPromoDyes(tPlayerFrom))
                {
                    break;
                }
            }

            SItemCrafting_TryDyeItem(idACDItem, ptMsg->eDyeType, tPlayerFrom);
        }
        break;

    case CMSG_UNLOCKED_DYES_REQUEST:
        {
            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_MYSTIC))
            {
                break;
            }

            BOOL bHasCollectorsEditionDyes = CValidatePlayerHasPromoDyes(tPlayerFrom);

            ACDID idACDPlayer = tPlayerFrom.idPlayerACD;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, idACDPlayer);
            D3::GameMessage::UnlockedDyesData tDyesData;

            if (bHasCollectorsEditionDyes)
            {
                tDyesData.add_dye_gbids(ItemGetGBIDForDyeType(DYE_TYPE_13));
                tDyesData.add_dye_gbids(ItemGetGBIDForDyeType(DYE_TYPE_20));
            }

            MessageSendToClient(listDestPlayers, SMSG_UNLOCKED_DYES_RESULTS, tDyesData);
        }
        break;

    case CMSG_TRY_TRANSMUTE:
        {
            TransmuteItemsMessage * ptMsg = (TransmuteItemsMessage *)pMessage;

            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_HORADRIM))
            {
                break;
            }

            int32 nTotalIngredients = ptMsg->nItemsCount + ptMsg->nCurrenciesCount;

            if (nTotalIngredients < 0 || nTotalIngredients > 9)
            {
                break;
            }

            for (int32 i = 0; i < ptMsg->nItemsCount; i++)
            {
                ACDID idACDItem = ACDFromANN(ptMsg->annItems[i]);
                if (ACDTryToGet(idACDItem) == NULL)
                {
                    return;
                }

                if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE) && !CValidateItemIsInPlayerStash(tPlayerFrom, idACDItem))
                {
                    return;
                }
            }

            SItemCrafting_TryTransmute(ptMsg, tPlayerFrom);
        }
        break;

    case CMSG_VENDOR_TRY_TRAIN:
        if (!CValidatePlayerNearCrafter(tPlayerFrom, tPlayerFrom.eActiveCrafterType))
        {
            break;
        }

        if (!GetCraftLevel(tPlayerFrom.eActiveCrafterType, tPlayerFrom))
        {
            break;
        }

        SItemCrafting_TryTrain(tPlayerFrom);
        break;

    case CMSG_TRY_UPGRADE_JEWEL:
        {
            if (!CValidatePlayerNearCrafter(tPlayerFrom, CRAFTER_TYPE_JEWEL_UPGRADE))
            {
                break;
            }

            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;
            ACDID idACDItem = ACDFromANN(ptMsg->ann);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE) && !CValidateItemIsInPlayerStash(tPlayerFrom, idACDItem))
            {
                break;
            }

            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (!ItemIsUpgradeableJewel(tACDItem))
            {
                break;
            }

            if (ItemIsUnidentified(tACDItem))
            {
                break;
            }

            {
                NephalemRift::RIFT_REWARD_CHOICE_STATE eChoiceState = (NephalemRift::RIFT_REWARD_CHOICE_STATE)ptACDPlayer->AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE);
                if (eChoiceState != NephalemRift::RIFT_GEM_UPGRADE)
                {
                    break;
                }

                int32 nMaxUpgrades = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_MAX);
                if (nMaxUpgrades <= 0)
                {
                    break;
                }

                int32 nUpgradesRemaining = nMaxUpgrades - ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_USED);
                if (nUpgradesRemaining <= 0)
                {
                    break;
                }
            }

            SItemCrafting_UpgradeJewel(tPlayerFrom, tACDItem);
        }
        break;

    case CMSG_FOLLOWER_RETRAIN_REQUEST:
        SHireling_TryRetrain(tPlayerFrom);
        break;

    case CMSG_REQUEST_BUFF_CANCEL:
        {
            RequestBuffCancelMessage * ptBuffCancelMsg = (RequestBuffCancelMessage *)pMessage;

            SNO snoPower = ptBuffCancelMsg->snoPower;

            if (!CValidateSNO(POWER_SNO, snoPower))
            {
                break;
            }

            int32 nBuffIndex = ptBuffCancelMsg->nBuffIndex;

            PowerAPI_BuffRequestCancel(tPlayerFrom, snoPower, nBuffIndex);
        }
        break;

    case CMSG_REQUEST_EVENT_SKIP:
        if (!SPlayerInScriptedSequence(tPlayerFrom))
        {
            break;
        }

        if (ScriptEventOnScriptedSequenceSkipped(tPlayerFrom.idPlayerACD))
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

            {
                PlayerIndexMessage tMessage;
                tMessage.nPlayerIndex = tPlayerFrom.nPlayerIndex;
                MessageSendToClient(listDestPlayers, SMSG_HANDLE_EVENT_SKIP, &tMessage, sizeof(tMessage));
            }
        }
        break;

    case CMSG_REQUEST_PROMOTE_GAME_TO_PUBLIC:
        {
            OnlineService::OpenGameToPublicMessage * ptOpenGameToPublicMessage = (OnlineService::OpenGameToPublicMessage *)pMessage;

            SGame & tSGame = SGameManager::GameGet();

            if (!(tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT)))
            {
                for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
                {
                    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                    int32 nPlayerLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
                    int32 nPlayerHeroHighestSoloRift = tACDPlayer.AttributesGetInt(ATTRIBUTE_HIGHEST_HERO_SOLO_RIFT_LEVEL);
                    if (!PlayerCanPlayHandicap(nPlayerLevel,
                                               nPlayerHeroHighestSoloRift,
                                               GameGetHandicapLevel(),
                                               true))
                    {
                        return;
                    }
                }

                tSGame.tStartParams.dwCreationFlags |= (1 << GAMECREATEPARAM_PUBLIC_BIT);
                tSGame.ptSGameGlobals->tMatchmakingData.dwGameMadePublicTick = GameGetServerTime();

                SGameSetPartyGuideAccount(tPlayerFrom);
            }

            SGamePause(FALSE);

            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

            MessageSendToClient(listDestPlayers, SMSG_GAME_PROMOTED_TO_PUBLIC, ptOpenGameToPublicMessage, sizeof(*ptOpenGameToPublicMessage));
        }
        break;

    case CMSG_SEND_PLAYER_PREFS:
        {
            DWordDataMessage * ptMsg = (DWordDataMessage *)pMessage;
            if (ptMsg->dwData <= 32)
            {
                tPlayerFrom.dwServerSidePrefFlags = ptMsg->dwData;
            }
        }
        break;

    case CMSG_PING:
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(&tPlayerFrom);
            MessageSendToClient(listDestPlayers, SMSG_PONG, pMessage, sizeof(DWordDataMessage));
        }
        break;

    case CMSG_SPONG:
        SPlayerReceivePing(tPlayerFrom);
        break;

    case CMSG_LOAD_COMPLETE:
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                if (PlayersAreLocalToEachOther(*ptPlayer, tPlayerFrom))
                {
                    SPlayer_ClientLoadComplete(*ptPlayer);
                }
            }
        }
        break;

    case CMSG_TRY_VOTE_KICK:
        {
            PlayerIndexMessage * ptMsg = (PlayerIndexMessage *)pMessage;

            if (!CValidatePlayerIndex(ptMsg->nPlayerIndex))
            {
                break;
            }

            VoteKick::Server::KickRequested(tPlayerFrom, PlayerGetByPlayerIndex(ptMsg->nPlayerIndex));
        }
        break;

    case CMSG_START_VOTE_KICK:
        {
            VoteKickMessage * ptMsg = (VoteKickMessage *)pMessage;
            if (ptMsg->nPlayerIndexInitiating != tPlayerFrom.nPlayerIndex)
            {
                break;
            }

            if (!CValidateUniString(ptMsg->usMessage, sizeof(ptMsg->usMessage)))
            {
                break;
            }

            if (!CValidatePlayerIndex(ptMsg->nPlayerIndexToKick))
            {
                break;
            }

            VoteKick::Server::TryVoteKick(ptMsg);
        }
        break;

    case CMSG_VOTE_KICK_VOTE:
        {
            BoolDataMessage * ptMsg = (BoolDataMessage *)pMessage;
            if (ptMsg->bVal)
            {
                VoteKick::Server::VoteYes(&tPlayerFrom);
            }
            else
            {
                VoteKick::Server::VoteNo(&tPlayerFrom);
            }
        }
        break;

    case CMSG_REQUEST_PAUSE_STATE_CHANGE:
        {
            BoolDataMessage * ptMsg = (BoolDataMessage *)pMessage;
            SGameHandlePauseRequest(tPlayerFrom, ptMsg->bVal);
        }
        break;

    case CMSG_REQUEST_SOFT_PAUSE_STATE_CHANGE:
        {
            BoolDataMessage * ptMsg = (BoolDataMessage *)pMessage;
            SGameHandleSoftPauseRequest(tPlayerFrom, ptMsg->bVal);
        }
        break;

    case CMSG_TRIED_SALVAGE:
        tPlayerFrom.tHeroStateData.uPlayerFlags |= 0x10;
        break;

    case CMSG_MAIL_OPERATION:
        {
            D3::GameMessage::MailOperation tMailOperation;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tMailOperation))
            {
                ItemMailHandleMailOperation(tPlayerFrom, tMailOperation);
            }
        }
        break;

    case CMSG_JOIN_CONSOLE_GAME:
        ServerMessageReceiveOutsideGame(idServerSocket, pMessage);
        break;

    case CMSG_STASH_ICON_STATE:
        {
            StashIconStateMessage * ptMsg = (StashIconStateMessage *)pMessage;
            SigmaMemoryCopy(tPlayerFrom.bStashIcons, ptMsg->bStashIcons, sizeof(ptMsg->bStashIcons));
        }
        break;

    case CMSG_SAVE_BANNER:
        {
            D3::Account::BannerConfiguration tConfig;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tConfig))
            {
                tPlayerFrom.tAccount.tBanner.Import(tConfig);
                PlayerList listDestPlayersAll(PlayerListGetElementPool());
                PlayerListGetAllConnected(listDestPlayersAll, -1, FALSE);
                SPlayerSendBanner(listDestPlayersAll, tPlayerFrom);
            }
        }
        break;

    case CMSG_REQUEST_PROMOTE_TO_PARTY_GUIDE:
        {
            ANNDataMessage * ptMsg = (ANNDataMessage *)pMessage;

            SGame & tSGame = SGameManager::GameGet();
            if (!(tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT)))
            {
                break;
            }

            if (!GameCommonPlayerIsPartyGuide(tPlayerFrom))
            {
                break;
            }

            if (!CValidateANN(ptMsg->ann))
            {
                break;
            }

            Player * ptPlayerTarget = PlayerGetByACD(ACDFromANN(ptMsg->ann));
            if (ptPlayerTarget == NULL)
            {
                break;
            }

            SGameSetPartyGuideAccount(*ptPlayerTarget);
        }
        break;

    case CMSG_TRY_LOWER_DIFFICULTY:
        {
            if (!GameCommonDataExists())
            {
                break;
            }

            GameCommonData & tGCD = GameGetActiveGameCommonData();
            if (tGCD.nHandicapLevel <= 0)
            {
                break;
            }

            if (tGCD.dwDifficultyChangedFlags & 2)
            {
                break;
            }

            if (NephalemRift::IsTieredRiftActive())
            {
                break;
            }

            if (tGCD.tGameSyncedData.bSetDungeonActive)
            {
                break;
            }

            SGame & tSGame = SGameManager::GameGet();
            if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
            {
                break;
            }

            if (tPlayerFrom.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE))
            {
                break;
            }

            SGameUpdateDifficultyLevel(tPlayerFrom, tGCD.nHandicapLevel - 1);
        }
        break;

    case CMSG_TRY_RAISE_DIFFICULTY:
        {
            if (!GameCommonDataExists())
            {
                break;
            }

            GameCommonData & tGCD = GameGetActiveGameCommonData();
            if (tGCD.dwDifficultyChangedFlags & 1)
            {
                break;
            }

            if (tGCD.nHandicapLevel > 0)
            {
                break;
            }

            if (NephalemRift::IsTieredRiftActive())
            {
                break;
            }

            if (tGCD.tGameSyncedData.bSetDungeonActive)
            {
                break;
            }

            SGame & tSGame = SGameManager::GameGet();
            if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
            {
                break;
            }

            if (tPlayerFrom.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE))
            {
                break;
            }

            SGameUpdateDifficultyLevel(tPlayerFrom, tGCD.nHandicapLevel + 1);
        }
        break;

    case CMSG_TRY_CHANGE_DIFFICULTY:
        {
            if (!GameCommonDataExists())
            {
                break;
            }

            IntDataMessage * ptMsg = (IntDataMessage *)pMessage;
            if ((uint32)ptMsg->nData > 16)
            {
                break;
            }

            GameCommonData & tGCD = GameGetActiveGameCommonData();
            if (tGCD.nHandicapLevel == ptMsg->nData)
            {
                break;
            }

            SGame & tSGame = SGameManager::GameGet();
            if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
            {
                break;
            }

            if (tPlayerFrom.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE))
            {
                break;
            }

            SGameUpdateDifficultyLevel(tPlayerFrom, ptMsg->nData);
        }
        break;

    case CMSG_LEAVE_CONSOLE_GAME:
        if (PlayerCalculatePlayerCountOnConnection(tPlayerFrom.idGameConnection) >= 2)
        {
            ServerPlayerLeftOnConnection(tPlayerFrom.idGameConnection, tPlayerFrom);
            SPlayerDelete(tPlayerFrom);

            SGamePause(FALSE);
        }
        break;

    case CMSG_ITEM_JUNK_FLAG_CHANGED:
        {
            ItemJunkFlagChangedMessage * ptMessage = (ItemJunkFlagChangedMessage *)pMessage;

            if (!CValidateANN(ptMessage->ann))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMessage->ann);
            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            tACDItem.AttributesSetInt(ATTRIBUTE_ITEM_FLAGGED_AS_JUNK, ptMessage->bJunkFlag);
        }
        break;

    case CMSG_PARAGON_POINT_SPEND:
        {
            SpendParagonPointsMessage * ptMessage = (SpendParagonPointsMessage *)pMessage;
            if (ptMessage->nPoints <= 0)
            {
                break;
            }

            if (!PlayerCanChangeParagon(*ptACDPlayer))
            {
                break;
            }

            SPlayerSpendParagonPoints(tPlayerFrom, ptMessage->gbidParagonBonus, ptMessage->nPoints);
        }
        break;

    case CMSG_PARAGON_POINTS_RESET:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            if ((uint32)ptMessage->nData > 3)
            {
                break;
            }

            if (!PlayerCanChangeParagon(*ptACDPlayer))
            {
                break;
            }

            SPlayerResetParagonPoints(tPlayerFrom, ptMessage->nData);
        }
        break;

    case CMSG_PARAGON_PURCHASE_RESET_CATEGORY:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            if ((uint32)ptMessage->nData <= 3)
            {
                SPlayerPurchaseParagonPointsReset(tPlayerFrom, ptMessage->nData);
            }
        }
        break;

    case CMSG_WARP_DEAD_PLAYER:
        {
            if (ptACDPlayer->IsAlive())
            {
                break;
            }

            PlayerIndexMessage * ptMessage = (PlayerIndexMessage *)pMessage;

            Player * ptPlayerOther = PlayerGetByPlayerIndex(ptMessage->nPlayerIndex);
            if (ptPlayerOther != NULL)
            {
                PlayersAreLocalToEachOther(tPlayerFrom, *ptPlayerOther);

                {
                    ActorCommonData * ptACDPlayerOther = ACDTryToGet(ptPlayerOther->idPlayerACD);
                    if (ptACDPlayerOther->IsAlive())
                    {
                        WorldPlace tPlace = ptACDPlayerOther->GetPlace();

                        ActorID idActor = (ActorID)ACDGetOwner(tPlayerFrom.idPlayerACD);
                        Actor * ptActor = ActorTryToGet(idActor);
                        if (ptActor != NULL)
                        {
                            DeleteLastHeadstone(*ptACDPlayer);
                            WorldLocation & loc = ptACDPlayer->GetWorldLocation();

                            if (tPlace.idSWorld == loc.idSWorld)
                            {
                                ActorSnapToPosition(ptActor->id, tPlace, TRUE, TRUE);
                            }
                        }
                    }
                }
            }
        }
        break;

    case CMSG_MAIL_ITEM_SENT:
        {
            StackPortionMessage * ptMessage = (StackPortionMessage *)pMessage;

            if (!CValidateANN(ptMessage->annItem))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMessage->annItem);
            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            SACDInventoryDeleteItem(tACDItem, ptMessage->uAmount, TRUE, FALSE);
        }
        break;

    case CMSG_MAIL_CURRENCY_SENT:
        {
            CurrencyMessage * ptMessage = (CurrencyMessage *)pMessage;
            ptACDPlayer->ModifyCurrencyAmount(-(int64)ptMessage->uAmount, ptMessage->eCurrency, 14, FALSE);
        }
        break;

    case CMSG_MAIL_CURRENCY_RECEIVED:
        {
            CurrencyMessage * ptMessage = (CurrencyMessage *)pMessage;
            ptACDPlayer->ModifyCurrencyAmount(ptMessage->uAmount, ptMessage->eCurrency, 14, FALSE);
        }
        break;

    case CMSG_MAIL_ITEM_RECEIVED:
        {
            D3::Items::Generator tGenerator;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tGenerator))
            {
                SPlayerAddMailItemToInventory(tPlayerFrom, tGenerator, FALSE);
            }
        }
        break;

    case CMSG_MAIL_ITEM_RECEIVED_EQUIP:
        {
            D3::Items::Generator tGenerator;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tGenerator))
            {
                SPlayerAddMailItemToInventory(tPlayerFrom, tGenerator, TRUE);
            }
        }
        break;

    case CMSG_GIFT_ITEM_RECEIVED:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            LootGenerateGift(tPlayerFrom, ptMessage->nData);
        }
        break;

    case CMSG_DESTROY_GIFT:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            ACDID idItem = ACDFromANN(ptMessage->ann);

            ActorCommonData * ptACDItem = ACDTryToGet(idItem);
            if (ptACDItem != NULL)
            {
                SACDInventoryDeleteItem(*ptACDItem, 1, TRUE, FALSE);
            }
        }
        break;

    case CMSG_SEND_SEASONAL_ITEM_TO_STASH:
        {
            D3::Items::Generator tGenerator;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tGenerator))
            {
                SPlayerRetrieveSeasonalStashItemOrChallengeRiftReward(tPlayerFrom, tGenerator, SLOT_PLAYER_SHARED_STASH);
            }
        }
        break;

    case CMSG_SEND_SEASONAL_ITEM_TO_INVENTORY:
        {
            D3::Items::Generator tGenerator;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tGenerator))
            {
                SPlayerRetrieveSeasonalStashItemOrChallengeRiftReward(tPlayerFrom, tGenerator, SLOT_PLAYER_BACKPACK);
            }
        }
        break;

    case CMSG_AVENGER_SEND_ACCOUNT_INFO:
        {
            D3::Account::AvengerData tAvengerData;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tAvengerData))
            {
                Avenger::Console::SaveAccountInfoToAvengerData(&tPlayerFrom, tAvengerData);
            }
        }
        break;

    case CMSG_AVENGER_SEND_FRIEND_AVENGER:
        {
            D3::Account::Avenger tFriendAvenger;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tFriendAvenger))
            {
                Avenger::Console::SaveAvengerToPlayer(tFriendAvenger, &tPlayerFrom.tFriendAvenger, AVENGER_SOURCE_FRIEND);
            }
        }
        break;

    case CMSG_AVENGER_UPDATE_SOURCE:
        {
            AvengerUpdateSourceMessage * ptMessage = (AvengerUpdateSourceMessage *)pMessage;
            tPlayerFrom.eAvengerSource = ptMessage->eAvengerSource;
        }
        break;

    case CMSG_SET_FRIEND_STATUS:
        {
            BoolDataMessage * ptMessage = (BoolDataMessage *)pMessage;
            SPlayerSetHasFriends(tPlayerFrom, ptMessage->bVal);
        }
        break;

    case CMSG_PLAYER_GIFT_REGISTER_RECIPIENT:
        {
            PlayerGiftMessage * ptMessage = (PlayerGiftMessage *)pMessage;

            if (!CValidateANN(ptMessage->ann))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMessage->ann);

            ActorCommonData & tACDItem = ACDGet(idACDItem);

            SACDInventoryRegisterItemGiftRecipient(tACDItem, ptMessage->nId, ptMessage->szPlayerName);
        }
        break;

    case CMSG_PLAYER_GIFT_SENT:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;

            if (!CValidateANN(ptMessage->ann))
            {
                break;
            }

            ACDID idACDItem = ACDFromANN(ptMessage->ann);
            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (!CValidateItemIsInPlayerInventory(tPlayerFrom, idACDItem, FALSE))
            {
                break;
            }

            SACDInventoryDeleteItem(tACDItem, 1, TRUE, FALSE);
        }
        break;

    case CMSG_REDEEM_SEASON_REWARD_CONSOLE:
        {
            GBIDDataMessage * ptMessage = (GBIDDataMessage *)pMessage;
            SPlayerRedeemSeasonGift(tPlayerFrom, ptMessage->gbidData);
        }
        break;

    case CMSG_EQUIP_COSMETIC_ITEM:
        {
            GBIDDataMessage * ptEquipGBIDMessage = (GBIDDataMessage *)pMessage;
            SCosmeticItems_EquipCosmeticItem(tPlayerFrom, ptEquipGBIDMessage->gbidData);
        }
        break;

    case CMSG_UNEQUIP_COSMETIC_ITEM:
        {
            IntDataMessage * ptUnequipMessage = (IntDataMessage *)pMessage;
            SCosmeticItems_UnequipCosmeticItem(tPlayerFrom, ptUnequipMessage->nData);
        }
        break;

    case CMSG_ACHIEVEMENTS_GET_SNAPSHOT:
        {
            D3::GameMessage::AchievementsGetSnapshot tSnapshotRequest;
            if (ParseProtocolBufferMessage(ptHeader->eType, (uint8 *)pMessage, tSnapshotRequest))
            {
                D3::GameMessage::AchievementsSnapshot tResult;
                *tResult.mutable_game_account_id() = tSnapshotRequest.game_account_id();

                if (tSGameGlobals.ptAchievementsInterface == NULL || !tSGameGlobals.ptAchievementsInterface->GetSnapshot(tSnapshotRequest.game_account_id(), *tResult.mutable_snapshot()))
                {
                    tResult.set_error_code(OnlineService::ERROR_GLOBAL_NO_DATA);
                }

                PlayerList listDestPlayers(PlayerListGetElementPool());
                listDestPlayers.AddTail_Tracked(&tPlayerFrom);
                MessageSendToClient(listDestPlayers, SMSG_ACHIEVEMENTS_SNAPSHOT, tResult);
            }
        }
        break;

    case CMSG_EQUIPMENT_MANAGER_CLOSE:
        {
            ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayerFrom.tTalkData.idACDTalkingTo);
            if (ptACDTalkingTo != NULL && NPCIsEquipmentManagerVendor(*ptACDTalkingTo))
            {
                STalkResetTalkingTo(tPlayerFrom);
            }
        }
        break;

    case CMSG_REQUEST_LOADOUT:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            PlayerSendHeroStateLoadoutToClient(tPlayerFrom, ptMessage->nData, FALSE);
        }
        break;

    case CMSG_EQUIP_LOADOUT:
        {
            IntDataMessage * ptMessage = (IntDataMessage *)pMessage;
            int32 nLoadoutIndex = ptMessage->nData;

            ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayerFrom.tTalkData.idACDTalkingTo);

            if (!CValidatePlayerNearStash(tPlayerFrom) && (ptACDTalkingTo == NULL || !NPCIsEquipmentManagerVendor(*ptACDTalkingTo)))
            {
                break;
            }

            ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayerFrom.idPlayerACD);
            if (!PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
            {
                break;
            }

            if (!PlayerCanChangeLoadoutSkillsAndTraits(tPlayerFrom, nLoadoutIndex))
            {
                break;
            }

            if (!PlayerCanChangeLoadoutEquipment(tPlayerFrom, nLoadoutIndex))
            {
                break;
            }

            SPlayerLoadout_Equip(tPlayerFrom, nLoadoutIndex);
        }
        break;

    case CMSG_SAVE_LOADOUT:
        {
            PlayerLoadoutSaveMessage * ptMessage = (PlayerLoadoutSaveMessage *)pMessage;

            ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayerFrom.tTalkData.idACDTalkingTo);

            CRefString sName(ptMessage->usName);

            if (!CValidatePlayerNearStash(tPlayerFrom) && (ptACDTalkingTo == NULL || !NPCIsEquipmentManagerVendor(*ptACDTalkingTo)))
            {
                break;
            }

            SPlayerLoadout_Save(tPlayerFrom, ptMessage->nLoadoutIndex, sName);
        }
        break;

    case CMSG_SET_LOADOUT_TAB_ICON:
        {
            PlayerLoadoutTabIconMessage * ptMessage = (PlayerLoadoutTabIconMessage *)pMessage;

            ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayerFrom.tTalkData.idACDTalkingTo);
            if (ptACDTalkingTo == NULL || !NPCIsEquipmentManagerVendor(*ptACDTalkingTo))
            {
                break;
            }

            int32 nLoadoutIndex = ptMessage->nLoadoutIndex;
            if (nLoadoutIndex < 0 || nLoadoutIndex >= 5)
            {
                break;
            }

            LoadoutSavedData & tLoadout = tPlayerFrom.tLoadouts[nLoadoutIndex];
            tLoadout.nTabIcon = ptMessage->nTabIcon;
        }
        break;

    case CMSG_SET_LOADOUT_NAME:
        {
            PlayerLoadoutSaveMessage * ptMessage = (PlayerLoadoutSaveMessage *)pMessage;

            ActorCommonData * ptACDTalkingTo = ACDTryToGet(tPlayerFrom.tTalkData.idACDTalkingTo);
            if (ptACDTalkingTo == NULL || !NPCIsEquipmentManagerVendor(*ptACDTalkingTo))
            {
                break;
            }

            int32 nLoadoutIndex = ptMessage->nLoadoutIndex;
            if (nLoadoutIndex < 0 || nLoadoutIndex >= 5)
            {
                break;
            }

            LoadoutSavedData & tLoadout = tPlayerFrom.tLoadouts[nLoadoutIndex];
            if (tLoadout.bEmpty)
            {
                break;
            }

            CRefString sName(ptMessage->usName);

            Blizzard::String::Copy(tLoadout.usName, sName.str(), sizeof(tLoadout.usName));

            PlayerSendHeroStateLoadoutToClient(tPlayerFrom, nLoadoutIndex, FALSE);
        }
        break;

    case CMSG_AMIIBO_SCAN_END:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;
            ACDID idACDPlayer = ACDFromANN(ptMessage->ann);
            if (idACDPlayer == ACDID_INVALID)
            {
                break;
            }

            ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
            if (ptACDPlayer == NULL)
            {
                break;
            }

            PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDPlayer, SNOFileLookupGUID(POWER_SNO, "AmiiboPortal"), -1, 0, ACDID_INVALID);
        }
        break;

    case CMSG_HAS_NFP_DEVICE:
    case CMSG_LOST_NFP_DEVICE:
        {
            ANNDataMessage * ptMessage = (ANNDataMessage *)pMessage;
            ACDID idACDPlayer = ACDFromANN(ptMessage->ann);
            if (idACDPlayer == ACDID_INVALID)
            {
                break;
            }

            ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
            if (ptACDPlayer == NULL)
            {
                break;
            }

            BOOL bHasNFPDevice = TRUE;
            if (ptHeader->eType == CMSG_LOST_NFP_DEVICE)
            {
                PowerAPI_BuffFlagAllFromPowerForDeletion(*ptACDPlayer, SNOFileLookupGUID(POWER_SNO, "AmiiboPortal"), -1, 0, ACDID_INVALID);
                bHasNFPDevice = FALSE;
            }

            ptACDPlayer->SetHasNFPDevice(bHasNFPDevice);
        }
        break;

    default:
        XServerSocketDisconnect(idServerSocket, SDR_UNKNOWN_MESSAGE);
        break;
    }
}
