// Original file: X/Common/Networking/ByteswapMessage.cpp

#include "GameMessage.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Sigma/Networking/XDecoder.h"
#include "../../Sigma/Networking/XEncoder.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../CombatLog.h"
#include "../Definitions/GameBalanceDefinition.h"

namespace OnlineService { extern const XTypeDescriptor * DT_OPEN_GAME_TO_PUBLIC_MESSAGE; }  // X/Common/OnlineService/OnlineService.cpp

// X/Common/CombatLog.h (src's header does not carry the message yet)
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
    Message();
};
static_assert(sizeof(Message) == 0x24, "Message");
extern const XTypeDescriptor * DT_COMBAT_LOG_MESSAGE;  // X/Common/CombatLog.cpp
}  // namespace CombatLog

// X/Common/ConversationCommon.h
typedef int32 ConversationSpeaker;
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
static_assert(sizeof(PlayLineParams) == 0x4c, "PlayLineParams");

// X/Common/Items.h
namespace D3 { namespace Items { class Generator; class RareItemName; } }









static_assert(sizeof(RareItemName) == 0x10, "RareItemName");

#include "../ActTransitions.h"
#include "../ActorCommonData.h"
#include "../ActorInventory.h"
#include "../Actors/ActorEffectCommon.h"
#include "../AnimPlayer.h"
#include "../Attributes/AttributeCompressor.h"
#include "../Console/Avenger.h"
#include "../Definitions/HeroDefinition.h"
#include "../Definitions/WorldDefinition.h"
#include "../Hireling.h"
#include "../ItemMailCommon.h"
#include "../Movement/ActorMovement.h"
#include "../NPC.h"
#include "BattlenetMessages.h"
#include "../Player.h"
#include "../PortalCommon.h"
#include "../Quests/Quests.h"
#include "../TimedEvents.h"
#include "../VisualInventory.h"
#include "../World.h"

// Message structs that no src header carries yet; layouts from the DWARF headers named.
// X/Common/ConversationCommon.h
struct AdvanceConvMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConv;  // 0x08
    int32 nLineUID;  // 0x0c
};
static_assert(sizeof(AdvanceConvMessage) == 0x10, "AdvanceConvMessage");

struct EndConversationMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConversation;  // 0x08
    ACDNetworkName annSpeaker;  // 0x0c
    int8 nPlayerTalkingIndex;  // 0x10
};
static_assert(sizeof(EndConversationMessage) == 0x14, "EndConversationMessage");

struct PlayConvLineMessage {  // size 0x80
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annSpeaker;  // 0x08
    ACDNetworkName annParticipants[9];  // 0x0c
    PlayLineParams tParams;  // 0x30
    uint32 dwTimeToDisplay;  // 0x7c
};
static_assert(sizeof(PlayConvLineMessage) == 0x80, "PlayConvLineMessage");

struct StopConvLineMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nConvLineGUID;  // 0x08
    BOOL fStoppedByUser;  // 0x0c
};
static_assert(sizeof(StopConvLineMessage) == 0x10, "StopConvLineMessage");

struct UpdateConvAutoAdvanceMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConv;  // 0x08
    int32 nLineUID;  // 0x0c
    uint32 dwNextAdvanceTime;  // 0x10
};
static_assert(sizeof(UpdateConvAutoAdvanceMessage) == 0x14, "UpdateConvAutoAdvanceMessage");

// X/Common/Crafting/ItemCrafting.h
struct CrafterLevelUpMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    CrafterType eType;  // 0x08
    uint32 dwAnimTag;  // 0x0c
    uint32 dwNewIdle;  // 0x10
    int32 nLevel;  // 0x14
};
static_assert(sizeof(CrafterLevelUpMessage) == 0x18, "CrafterLevelUpMessage");

struct CraftingResultsMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidItem;  // 0x0c
    ItemQualityLevel eIQL;  // 0x10
};
static_assert(sizeof(CraftingResultsMessage) == 0x14, "CraftingResultsMessage");

struct CraftItemsMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    GBID gbidRecipe;  // 0x08
    int32 nCount;  // 0x0c
};
static_assert(sizeof(CraftItemsMessage) == 0x10, "CraftItemsMessage");

struct DyeItemMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    DyeType eDyeType;  // 0x0c
};
static_assert(sizeof(DyeItemMessage) == 0x10, "DyeItemMessage");

struct EnchantAffixChooseMessage {  // size 0x40
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    EnchantAffixChoice tEnchantAffixChoices[6];  // 0x0c
    int32 nEnchantAffixChoicesCount;  // 0x3c
};
static_assert(sizeof(EnchantAffixChooseMessage) == 0x40, "EnchantAffixChooseMessage");

struct EnchantAffixMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidAffixToReroll;  // 0x0c
};
static_assert(sizeof(EnchantAffixMessage) == 0x10, "EnchantAffixMessage");

struct TransmogItemMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidTransmog;  // 0x0c
};
static_assert(sizeof(TransmogItemMessage) == 0x10, "TransmogItemMessage");





















// X/Client/Network/Client.h
struct EndOfTickMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwTickEnding;  // 0x08
    uint32 dwTickNext;  // 0x0c
};
static_assert(sizeof(EndOfTickMessage) == 0x10, "EndOfTickMessage");

// X/Common/GameCommonData.h
struct GameSyncedDataMessage {  // size 0xd8
    RequiredMessageHeader tHeader;  // 0x00
    GameSyncedData tSyncedData;  // 0x08
};
static_assert(sizeof(GameSyncedDataMessage) == 0xd8, "GameSyncedDataMessage");

// X/Common/Hireling.h
struct HirelingSwapMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    HirelingClass eNewClass;  // 0x08
};
static_assert(sizeof(HirelingSwapMessage) == 0xc, "HirelingSwapMessage");

// X/Common/Player.h
struct LogoutContextMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    LogoutContext eLogoutContext;  // 0x08
};
static_assert(sizeof(LogoutContextMessage) == 0xc, "LogoutContextMessage");














struct PlayerLoadoutEquipmentUpdateMessage {  // size 0x30
    RequiredMessageHeader tHeader;  // 0x00
    int32 nLoadoutIndex;  // 0x08
    int32 nEquipmentIndex;  // 0x0c
    LoadoutItemData tNewItem;  // 0x10
};
static_assert(sizeof(PlayerLoadoutEquipmentUpdateMessage) == 0x40, "PlayerLoadoutEquipmentUpdateMessage");









struct PlayerLoadoutSaveMessage {  // size 0x40
    RequiredMessageHeader tHeader;  // 0x00
    int32 nLoadoutIndex;  // 0x08
    CHAR usName[49];  // 0x0c
};
static_assert(sizeof(PlayerLoadoutSaveMessage) == 0x40, "PlayerLoadoutSaveMessage");

struct PlayerLoadoutSetSlotMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    int32 nLoadoutIndex;  // 0x08
    InventorySlotType eSlot;  // 0x0c
    ACDNetworkName annItem;  // 0x10
};
static_assert(sizeof(PlayerLoadoutSetSlotMessage) == 0x14, "PlayerLoadoutSetSlotMessage");















struct StashIconStateMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    uint8 bStashIcons[12];  // 0x08
};
static_assert(sizeof(StashIconStateMessage) == 0x14, "StashIconStateMessage");

// X/Common/Actors/ActorEffectCommon.h
struct PlayMusicMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoMusic;  // 0x08
};
static_assert(sizeof(PlayMusicMessage) == 0xc, "PlayMusicMessage");

struct PlayNonPositionalSoundMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoSound;  // 0x08
};
static_assert(sizeof(PlayNonPositionalSoundMessage) == 0xc, "PlayNonPositionalSoundMessage");

// X/Common/ActorCommonData.h





static_assert(sizeof(RareItemNameMessage) == 0x1c, "RareItemNameMessage");

// Type descriptors defined in other files.
extern const XTypeDescriptor * DT_ACD_CLIENT_TRANSLATE_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_COLL_FLAGS_MESSAGE;  // X/Common/ActorCommonData.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_ACK_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_ARC_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_PIE_WEDGE_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_SIN_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_DETPATH_SPIRAL_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_FACING_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_FIXED_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_FIXED_UPDATE_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_NORMAL_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_SNAPPED_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_STOPPED_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACD_TRANSLATE_SYNC_MESSAGE;  // X/Common/Movement/ActorMovement.cpp
extern const XTypeDescriptor * DT_ACT_TRANSITION_STARTED_MESSAGE;  // X/Common/ActTransitions.cpp
extern const XTypeDescriptor * DT_ADVANCE_CONV_MESSAGE;  // X/Common/ConversationCommon.cpp
extern const XTypeDescriptor * DT_ASSIGN_LEGENDARY_POWER_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_ASSIGN_SKILL_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_ASSIGN_TRAITS_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_ATTRIBUTES_SET_VALUES_MESSAGE;  // X/Common/Attributes/AttributeCompressor.cpp
extern const XTypeDescriptor * DT_ATTRIBUTE_SET_VALUE_MESSAGE;  // X/Common/Attributes/AttributeCompressor.cpp
extern const XTypeDescriptor * DT_CONV_END_MESSAGE;  // X/Common/ConversationCommon.cpp
extern const XTypeDescriptor * DT_CRAFTER_LEVEL_UP_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_CRAFTING_RESULT_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_CRAFT_ITEMS_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_CURRENCY_MODIFIED_MESSAGE;  // X/Common/ActorCommonData.cpp
extern const XTypeDescriptor * DT_DESTROY_SCENE_MESSAGE;  // X/Common/Definitions/WorldDefinition.cpp
extern const XTypeDescriptor * DT_DYE_ITEM_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_DYNAMIC_QUEST_OBJECTIVE_MESSAGE;  // X/Common/Quests/Quests.cpp
extern const XTypeDescriptor * DT_EFFECT_GROUP_ACD_TO_ACD_MESSAGE;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_ENCHANT_AFFIX_CHOOSE_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_ENCHANT_AFFIX_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_END_OF_TICK_MESSAGE;  // X/Client/Network/Client.cpp
extern const XTypeDescriptor * DT_GAME_SYNCED_DATA_MESSAGE;  // X/Common/GameCommonData.cpp
extern const XTypeDescriptor * DT_HERO_STATE_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_HIRELING_INFO_UPDATE_MESSAGE;  // X/Common/Hireling.cpp
extern const XTypeDescriptor * DT_HIRELING_SWAP_MESSAGE;  // X/Common/Hireling.cpp
extern const XTypeDescriptor * DT_JOIN_BNET_GAME_MESSAGE;  // X/Common/Networking/BattlenetMessages.cpp
extern const XTypeDescriptor * DT_LAST_WAYPOINT_LEVELAREA_DATA_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_LOGOUT_CONTEXT_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_MAIL_SEND_MESSAGE;  // X/Common/ItemMailCommon.cpp
extern const XTypeDescriptor * DT_MAIL_SENT_RESPONSE_MESSAGE;  // X/Common/ItemMailCommon.cpp
extern const XTypeDescriptor * DT_NEW_PLAYER_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_NPC_INTERACT_OPTIONS_MESSAGE;  // X/Common/NPC.cpp
extern const XTypeDescriptor * DT_PLAYER_KICK_TIMER_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_PLAYER_LOADOUT_DATA_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_PLAYER_LOADOUT_EQUIPMENT_UPDATE_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_PLAYER_LOADOUT_EQUIP_RESULTS_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_PLAYER_LOADOUT_SAVE_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_PLAYER_LOADOUT_SET_SLOT_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_PLAYER_LOADOUT_TAB_ICON_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_PLAYER_SAVED_DATA_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_PLAYER_SKILLS_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_PLAY_ANIMATION_MESSAGE;  // X/Common/AnimPlayer.cpp
extern const XTypeDescriptor * DT_PLAY_CONV_LINE_MESSAGE;  // X/Common/ConversationCommon.cpp
extern const XTypeDescriptor * DT_PLAY_EFFECT_MESSAGE;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_PLAY_HIT_EFFECT_MESSAGE;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_PLAY_HIT_EFFECT_OVERRIDE_MESSAGE;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_PLAY_MUSIC_MESSAGE;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_PLAY_NONPOSITIONAL_SOUND_MESSAGE;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_PORTAL_SPECIFIER_MESSAGE;  // X/Common/PortalCommon.cpp
extern const XTypeDescriptor * DT_PREFETCH_DATA_MESSAGE;  // X/Common/Definitions/WorldDefinition.cpp
extern const XTypeDescriptor * DT_PRELOAD_SCENE_DATA_MESSAGE;  // X/Common/Definitions/WorldDefinition.cpp
extern const XTypeDescriptor * DT_QUEST_COUNTER_MESSAGE;  // X/Common/Quests/Quests.cpp
extern const XTypeDescriptor * DT_QUEST_FLAGS_MESSAGE;  // X/Common/Quests/Quests.cpp
extern const XTypeDescriptor * DT_QUEST_HISTORY_UPDATE_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_QUEST_METER_MESSAGE;  // X/Common/Quests/Quests.cpp
extern const XTypeDescriptor * DT_QUEST_REWARD_HISTORY_UPDATE_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_QUEST_UPDATE_MESSAGE;  // X/Common/Quests/Quests.cpp
extern const XTypeDescriptor * DT_RARE_ITEM_NAME_MESSAGE;  // X/Common/ActorCommonData.cpp
extern const XTypeDescriptor * DT_RARE_MONSTER_NAMES_MESSAGE;  // X/Common/ActorCommonData.cpp
extern const XTypeDescriptor * DT_REVEAL_SCENE_MESSAGE;  // X/Common/Definitions/WorldDefinition.cpp
extern const XTypeDescriptor * DT_ROPE_EFFECT_MESSAGE_ACD_TO_ACD;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_ROPE_EFFECT_MESSAGE_ACD_TO_PLACE;  // X/Common/Actors/ActorEffectCommon.cpp
extern const XTypeDescriptor * DT_SAVED_CONVERSATIONS_UPDATE_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_SAVED_QUEST_UPDATE_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_SAVE_POINT_DATA_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_SET_IDLE_ANIMATION_MESSAGE;  // X/Common/AnimPlayer.cpp
extern const XTypeDescriptor * DT_SPEND_PARAGON_POINTS_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_STASH_ICON_STATE_MESSAGE;  // X/Common/Player.cpp
extern const XTypeDescriptor * DT_STOP_CONV_LINE_MESSAGE;  // X/Common/ConversationCommon.cpp
extern const XTypeDescriptor * DT_SWAP_SCENE_MESSAGE;  // X/Common/Definitions/WorldDefinition.cpp
extern const XTypeDescriptor * DT_SWAP_SKILLS_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_TIMED_EVENT_STARTED_MESSAGE;  // X/Common/TimedEvents.cpp
extern const XTypeDescriptor * DT_TRANSMOG_ITEM_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_TRANSMUTE_ITEMS_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_TRANSMUTE_RESULTS_MESSAGE;  // X/Common/Crafting/ItemCrafting.cpp
extern const XTypeDescriptor * DT_UNASSIGN_SKILL_MESSAGE;  // X/Common/Definitions/HeroDefinition.cpp
extern const XTypeDescriptor * DT_UPDATE_CONV_AUTO_ADVANCE_MESSAGE;  // X/Common/ConversationCommon.cpp
extern const XTypeDescriptor * DT_VISUAL_INVENTORY_MESSAGE;  // X/Common/VisualInventory.cpp
extern const XTypeDescriptor * DT_WORLD_SYNCED_DATA_MESSAGE;  // X/Common/World.cpp

inline XDecoder::Result GenericDecode(XDecoder & tDecoder, const XTypeDescriptor * ptDescriptor, void * pMessage, uint32 dwSize)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    TypeDescriptorField field(ptDescriptor, NULL);
    XDecoder::Result eResult = ptDescriptor->Decode(pMessage, field, tDecoder);
    ((RequiredMessageHeader *)pMessage)->dwSize = dwSize;
    return eResult;
}

// One message per source line in the original (the line table gives each case a single line),
// and every label equals its enumerator's spelling (including the misspelt
// SMGS_BLIZZCON_RIFT_ENDED), so the cases came from a macro that stringizes the enumerator.
// The macro's name and parameter list are invented. The list was generated from the original
// function's jump table and case blocks (build/try/agentD/bs_extract.py, bs_gen.py).
#define MESSAGE_CASE(eType, dtDescriptor, tStruct) \
    case eType: pTypeDescriptor = dtDescriptor; dwSize = sizeof(tStruct); pszMessageLabel = #eType; break;

BOOL GetGameMessageTypeAndSize(GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor, uint32 & dwSize, const char *& pszMessageLabel)
{
    pTypeDescriptor = NULL;
    dwSize = 0;

    switch (eMessageType)
    {
        MESSAGE_CASE(SMSG_GAME_SETUP, DT_GAME_SETUP_MESSAGE, GameSetupMessage)
        MESSAGE_CASE(SMSG_CONNECTION_ESTABLISHED, DT_CONNECTION_ESTABLISHED_MESSAGE, ConnectionEstablishedMessage)
        MESSAGE_CASE(SMSG_CONNECTION_ADD_PLAYER, DT_CONNECTION_ESTABLISHED_MESSAGE, ConnectionEstablishedMessage)
        MESSAGE_CASE(SMSG_QUIT, DT_QUIT_GAME_MESSAGE, QuitGameMessage)
        MESSAGE_CASE(SMSG_PRELOAD_ADD_SCENE, DT_PRELOAD_SCENE_DATA_MESSAGE, PreloadSceneDataMessage)
        MESSAGE_CASE(SMSG_PRELOAD_ADD_ACD, DT_PRELOAD_ACD_DATA_MESSAGE, PreloadACDDataMessage)
        MESSAGE_CASE(SMSG_PRELOAD_REMOVE_SCENE, DT_PRELOAD_SCENE_DATA_MESSAGE, PreloadSceneDataMessage)
        MESSAGE_CASE(SMSG_PRELOAD_REMOVE_ACD, DT_PRELOAD_ACD_DATA_MESSAGE, PreloadACDDataMessage)
        MESSAGE_CASE(SMSG_PREFETCH_SCENE, DT_PREFETCH_DATA_MESSAGE, PrefetchDataMessage)
        MESSAGE_CASE(SMSG_PREFETCH_WORLD, DT_PREFETCH_DATA_MESSAGE, PrefetchDataMessage)
        MESSAGE_CASE(SMSG_PREFETCH_LEVELAREA, DT_PREFETCH_DATA_MESSAGE, PrefetchDataMessage)
        MESSAGE_CASE(SMSG_PREFETCH_ACTOR, DT_PREFETCH_ACTOR_DATA_MESSAGE, PrefetchActorDataMessage)
        MESSAGE_CASE(SMSG_PLAYER_WARPING, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SET_KEEP_TOC_LOADED, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(CMSG_PING, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_PONG, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_SPING, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SPONG, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SYSTEM_NOTIFICATION, DT_SERVER_NOTIFICATION_DATA_MESSAGE, ServerNotificationDataMessage)
        MESSAGE_CASE(SMSG_SYSTEM_BROADCAST, DT_BROADCAST_TEXT_MESSAGE, BroadcastTextMessage)
        MESSAGE_CASE(CMSG_LOAD_COMPLETE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_NPC_INTERACT_CANCEL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_NPC_INTERACT_TALK_DIALOG, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(CMSG_NPC_INTERACT_PET_DISMISS, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_NPC_INTERACT_CRAFT, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_NPC_INTERACT_HIRE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_NPC_INTERACT_INVENTORY, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_GAME_END_REQUEST, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(CMSG_ACT_TRANSITION_REQUEST, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(CMSG_ACT_TRANSITION_REQUEST_OPEN_WORLD, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_START_END_COUNTDOWN, DT_ACT_TRANSITION_STARTED_MESSAGE, ActTransitionStartedMessage)
        MESSAGE_CASE(CMSG_START_VOTE_KICK, DT_VOTE_KICK_MESSAGE, VoteKickMessage)
        MESSAGE_CASE(CMSG_TRY_VOTE_KICK, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_CAN_VOTE_KICK, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_VOTE_KICK_VOTE, DT_VOTE_KICK_MESSAGE, VoteKickMessage)
        MESSAGE_CASE(CMSG_VOTE_KICK_VOTE, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_VOTE_KICK_FAILED, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_VOTE_KICK_PASSED, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_REQUEST_PAUSE_STATE_CHANGE, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_PAUSE_STATE_CHANGE, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_SOFT_PAUSE_STATE_CHANGE, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_ACCOUNT_FLAGS_SYNC, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_ADVENTURE_MODE_UNLOCKED_DEPRECATED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_ACCOUNT_STASH_TABS_REWARDED_FOR_SEASONS_SYNC, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(CMSG_TRIED_SALVAGE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_POLL_MAIL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_FETCH_MAIL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SEND_MAIL, DT_MAIL_SEND_MESSAGE, MailSendMessage)
        MESSAGE_CASE(CMSG_MAIL_OPERATION, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_MAIL_CONTENTS, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_MAIL_SENT, DT_MAIL_SENT_RESPONSE_MESSAGE, MailSentResponseMessage)
        MESSAGE_CASE(SMSG_MAIL_OPERATION_RESPONSE, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_PVP_KILL, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_PVP_ASSIST, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_TRY_REDEEM_PVP_TOKENS, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_PVP_TOKEN_REDEMPTION_RESULTS, DT_PVP_TOKEN_REDEMPTION_MESSAGE, PVPTokenRedemptionMessage)
        MESSAGE_CASE(SMSG_SET_CAMERA_OBSERVER, DT_PLAYER_SET_CAMERA_OBSERVER_MESSAGE, PlayerSetCameraObserverMessage)
        MESSAGE_CASE(SMSG_SET_CAMERA_ORBIT, DT_PLAYER_SET_CAMERA_ORBIT_MESSAGE, PlayerSetCameraOrbitMessage)
        MESSAGE_CASE(SMSG_SET_CAMERA_DEFAULTS, DT_PLAYER_SET_CAMERA_DEFAULTS_MESSAGE, PlayerSetCameraDefaultsMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SET_CAMERA_DEFAULTS, DT_PLAYER_CONSOLE_SET_CAMERA_DEFAULTS_MESSAGE, PlayerConsoleSetCameraDefaultsMessage)
        MESSAGE_CASE(SMSG_CONSOLE_VALIDATE_HERO, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CONSOLE_VALIDATE_ACCOUNT, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SAVE, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_HERO_SAVED_QUESTS, DT_SAVED_QUEST_UPDATE_MESSAGE, PlayerSavedQuestUpdateMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_HERO_QUEST_HISTORY, DT_QUEST_HISTORY_UPDATE_MESSAGE, PlayerQuestHistoryUpdateMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_HERO_QUEST_REWARD_HISTORY, DT_QUEST_REWARD_HISTORY_UPDATE_MESSAGE, PlayerQuestRewardHistoryUpdateMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_HERO_SAVED_CONVERSATIONS, DT_SAVED_CONVERSATIONS_UPDATE_MESSAGE, PlayerSavedConversationsUpdateMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_HERO_ACTIVE_HIRELING, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_HERO_TIME_PLAYED, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_ACCOUNT_ACHIEVEMENT_CRITERIA_UPDATE, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CONSOLE_COUNTER_INCREMENT, DT_CONSOLE_COUNTER_INCREMENT_MESSAGE, ConsoleCounterIncrementMessage)
        MESSAGE_CASE(SMSG_CONSOLE_GAME_ENDED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SAVE_BANNER, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_SAVE_BANNER_RESPONSE, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_ACHIEVEMENTS_AVAILABILITY, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(CMSG_ACHIEVEMENTS_GET_SNAPSHOT, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_ACHIEVEMENTS_SNAPSHOT, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_ACHIEVEMENT_AWARDED, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_ACHIEVEMENT_PROGRESS_XBOXONE, DT_ACHIEVEMENT_PROGRESS_XBOXONE_MESSAGE, AchievementProgressXboxOneMessage)
        MESSAGE_CASE(SMSG_CLEAR_FANFARE_QUEUE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_MULTIPLAYER_BUFF_ON, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_MULTIPLAYER_BUFF_OFF, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_DUEL_RESULT, DT_DUEL_RESULT_MESSAGE, DuelResultMessage)
        MESSAGE_CASE(SMSG_HEARTBEAT, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_VERSIONS, DT_VERSIONS_MESSAGE, VersionsMessage)
        MESSAGE_CASE(SMSG_PLAYER_DELETE, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_PLAYER_HAS_DIED, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_ACCEPTED_DEATH_ACK, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_TUTORIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_TUTORIAL_STATE, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_STASH_ICON_STATE, DT_STASH_ICON_STATE_MESSAGE, StashIconStateMessage)
        MESSAGE_CASE(CMSG_STASH_ICON_STATE, DT_STASH_ICON_STATE_MESSAGE, StashIconStateMessage)
        MESSAGE_CASE(SMSG_NEW_PLAYER, DT_NEW_PLAYER_MESSAGE, NewPlayerMessage)
        MESSAGE_CASE(SMSG_PLAYER_KICK_TIMERS, DT_PLAYER_KICK_TIMER_MESSAGE, PlayerKickTimerMessage)
        MESSAGE_CASE(SMSG_PLAYER_BANNER, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_ENTER_WORLD, DT_ENTER_WORLD_MESSAGE, EnterWorldMessage)
        MESSAGE_CASE(SMSG_REVEAL_WORLD, DT_REVEAL_WORLD_MESSAGE, RevealWorldMessage)
        MESSAGE_CASE(SMSG_REVEAL_SCENE, DT_REVEAL_SCENE_MESSAGE, RevealSceneMessage)
        MESSAGE_CASE(SMSG_DESTROY_SCENE, DT_DESTROY_SCENE_MESSAGE, DestroySceneMessage)
        MESSAGE_CASE(SMSG_SWAP_SCENE, DT_SWAP_SCENE_MESSAGE, SwapSceneMessage)
        MESSAGE_CASE(SMSG_REVEAL_TEAM, DT_REVEAL_TEAM_MESSAGE, RevealTeamMessage)
        MESSAGE_CASE(SMSG_HERO_STATE, DT_HERO_STATE_MESSAGE, HeroStateMessage)
        MESSAGE_CASE(SMSG_HERO_STATE_SAVEDDATA, DT_PLAYER_SAVED_DATA_MESSAGE, PlayerSavedDataMessage)
        MESSAGE_CASE(SMSG_HERO_STATE_SKILLS, DT_PLAYER_SKILLS_MESSAGE, PlayerSkillsMessage)
        MESSAGE_CASE(SMSG_HERO_STATE_PLAYERFLAGS, DT_PLAYER_DWORD2_DATA_MESSAGE, PlayerDWord2DataMessage)
        MESSAGE_CASE(SMSG_HERO_STATE_WAYPOINTS, DT_PLAYER_DWORD_DATA_MESSAGE, PlayerDWordDataMessage)
        MESSAGE_CASE(SMSG_HERO_STATE_LASTSAVEPOINT, DT_SAVE_POINT_DATA_MESSAGE, SavePointDataMessage)
        MESSAGE_CASE(SMSG_HERO_STATE_LASTWAYPOINTLEVELAREA, DT_LAST_WAYPOINT_LEVELAREA_DATA_MESSAGE, LastWaypointLevelAreaDataMessage)
        MESSAGE_CASE(SMSG_ACD_ENTER_KNOWN, DT_ACD_ENTER_KNOWN_MESSAGE, ACDEnterKnownMessage)
        MESSAGE_CASE(SMSG_ACD_EXIT_KNOWN, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_PLAYER_ENTER_KNOWN, DT_PLAYER_ENTER_KNOWN_MESSAGE, PlayerEnterKnownMessage)
        MESSAGE_CASE(SMSG_UI_ENTER_KNOWN, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_ACD_WORLD_POSITION, DT_ACD_WORLD_POSITION_MESSAGE, ACDWorldPositionMessage)
        MESSAGE_CASE(SMSG_ACD_INVENTORY_POSITION, DT_ACD_INVENTORY_POSITION_MESSAGE, ACDInventoryPositionMessage)
        MESSAGE_CASE(SMSG_ACD_INVENTORY_UPDATE_ACTOR_SNO, DT_ACD_INVENTORY_UPDATE_ACTOR_SNO, ACDInventoryUpdateActorSNO)
        MESSAGE_CASE(SMSG_PLAYER_ACTOR_SET, DT_PLAYER_ACTOR_SET_INITIAL_MESSAGE, PlayerActorSetInitialMessage)
        MESSAGE_CASE(SMSG_VISUAL_INVENTORY, DT_VISUAL_INVENTORY_MESSAGE, VisualInventoryMessage)
        MESSAGE_CASE(SMSG_ACD_CHANGE_GB_HANDLE, DT_ACD_CHANGE_GBHANDLE_MESSAGE, ACDChangeGBHandleMessage)
        MESSAGE_CASE(SMSG_AFFIX, DT_AFFIX_MESSAGE, AffixMessage)
        MESSAGE_CASE(SMSG_PORTAL_SPECIFIER, DT_PORTAL_SPECIFIER_MESSAGE, PortalSpecifierMessage)
        MESSAGE_CASE(SMSG_RARE_MONSTER_NAMES, DT_RARE_MONSTER_NAMES_MESSAGE, RareMonsterNamesMessage)
        MESSAGE_CASE(SMSG_RARE_ITEM_NAME, DT_RARE_ITEM_NAME_MESSAGE, RareItemNameMessage)
        MESSAGE_CASE(SMSG_INVENTORY_CREATE, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_ATTRIBUTE_SET_VALUE, DT_ATTRIBUTE_SET_VALUE_MESSAGE, AttributeSetValueMessage)
        MESSAGE_CASE(SMSG_PROJECTILE_STICK, DT_PROJECTILE_STICK_MESSAGE, ProjectileStickMessage)
        MESSAGE_CASE(SMSG_ATTRIBUTES_SET_VALUES, DT_ATTRIBUTES_SET_VALUES_MESSAGE, AttributesSetValuesMessage)
        MESSAGE_CASE(SMSG_CHAT, DT_CHAT_MESSAGE, ChatMessage)
        MESSAGE_CASE(SMSG_LOOTRUN_APPEARED, DT_LOOTRUN_APPEARED_MESSAGE, LootRunAppearedMessage)
        MESSAGE_CASE(SMSG_VICTIM, DT_VICTIM_MESSAGE, VictimMessage)
        MESSAGE_CASE(SMSG_MONSTER_ENGAGED, DT_COMBAT_ENGAGEMENT_MESSAGE, CombatEngagementMessage)
        MESSAGE_CASE(SMSG_ELITE_KILLED, DT_COMBAT_ENGAGEMENT_MESSAGE, CombatEngagementMessage)
        MESSAGE_CASE(SMSG_TREASURE_GOBLIN_ESCAPED, DT_COMBAT_ENGAGEMENT_MESSAGE, CombatEngagementMessage)
        MESSAGE_CASE(SMSG_TOWN_PORTAL_USED, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_PORTED_TO_PLAYER, DT_PORTED_TO_PLAYER_MESSAGE, PortedToPlayerMessage)
        MESSAGE_CASE(SMSG_PORTED_TO_WAYPOINT, DT_PORTED_TO_WAYPOINT_MESSAGE, PortedToWaypointMessage)
        MESSAGE_CASE(SMSG_KILLCOUNT, DT_KILL_COUNT_MESSAGE, KillCountMessage)
        MESSAGE_CASE(SMSG_SHRINE_TEXT_DISPLAY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_ACTIVATE_POOL_OF_REFLECTION, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_SHRINE_GLOBE_TEXT_DISPLAY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_ITEM_JUNK_FLAG_CHANGED, DT_JUNK_FLAG_MESSAGE, ItemJunkFlagChangedMessage)
        MESSAGE_CASE(CMSG_PARAGON_POINT_SPEND, DT_SPEND_PARAGON_POINTS_MESSAGE, SpendParagonPointsMessage)
        MESSAGE_CASE(CMSG_PARAGON_POINTS_RESET, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_PARAGON_PURCHASE_RESET_CATEGORY, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_REQUEST_PROMOTE_TO_PARTY_GUIDE, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_PLAYER_CLEAR_CLIENT_WALK_POWER, DT_PLAYER_CLEAR_CLIENT_WALK_POWER_MESSAGE, PlayerClearClientWalkPowerMessage)
        MESSAGE_CASE(SMSG_CHALLENGE_START, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(SMSG_CHALLENGE_STARTED, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(CMSG_CHALLENGE_ACCEPT, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(SMSG_PLAY_ANIMATION, DT_PLAY_ANIMATION_MESSAGE, PlayAnimationMessage)
        MESSAGE_CASE(SMSG_PLAY_IDLE_ANIMATION, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_NORMAL, DT_ACD_TRANSLATE_NORMAL_MESSAGE, ACDTranslateNormalMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_SNAPPED, DT_ACD_TRANSLATE_SNAPPED_MESSAGE, ACDTranslateSnappedMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_FACING, DT_ACD_TRANSLATE_FACING_MESSAGE, ACDTranslateFacingMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_FIXED, DT_ACD_TRANSLATE_FIXED_MESSAGE, ACDTranslateFixedMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_ARC, DT_ACD_TRANSLATE_ARC_MESSAGE, ACDTranslateArcMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_DETPATH, DT_ACD_TRANSLATE_DETPATH_MESSAGE, ACDTranslateDetPathMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_DETPATH_SIN, DT_ACD_TRANSLATE_DETPATH_SIN_MESSAGE, ACDTranslateDetPathSinMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_DETPATH_SPIRAL, DT_ACD_TRANSLATE_DETPATH_SPIRAL_MESSAGE, ACDTranslateDetPathSpiralMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_DETPATH_PIE_WEDGE, DT_ACD_TRANSLATE_DETPATH_PIE_WEDGE_MESSAGE, ACDTranslateDetPathPieWedgeMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_STOPPED, DT_ACD_TRANSLATE_STOPPED_MESSAGE, ACDTranslateStoppedMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_SYNC, DT_ACD_TRANSLATE_SYNC_MESSAGE, ACDTranslateSyncMessage)
        MESSAGE_CASE(SMSG_ACD_TRANSLATE_FIXED_UPDATE, DT_ACD_TRANSLATE_FIXED_UPDATE_MESSAGE, ACDTranslateFixedUpdateMessage)
        MESSAGE_CASE(SMSG_ACD_COLL_FLAGS, DT_ACD_COLL_FLAGS_MESSAGE, ACDCollFlagsMessage)
        MESSAGE_CASE(SMSG_CURRENCY_MODIFIED, DT_CURRENCY_MODIFIED_MESSAGE, CurrencyModifiedMessage)
        MESSAGE_CASE(SMSG_PLAY_EFFECT, DT_PLAY_EFFECT_MESSAGE, PlayEffectMessage)
        MESSAGE_CASE(SMSG_PLAY_HIT_EFFECT, DT_PLAY_HIT_EFFECT_MESSAGE, PlayHitEffectMessage)
        MESSAGE_CASE(SMSG_PLAY_HIT_EFFECT_OVERRIDE, DT_PLAY_HIT_EFFECT_OVERRIDE_MESSAGE, PlayHitEffectOverrideMessage)
        MESSAGE_CASE(SMSG_PLAY_NONPOSITIONAL_SOUND, DT_PLAY_NONPOSITIONAL_SOUND_MESSAGE, PlayNonPositionalSoundMessage)
        MESSAGE_CASE(SMSG_PLAY_ERROR_SOUND, DT_PLAY_ERROR_SOUND_MESSAGE, PlayErrorSoundMessage)
        MESSAGE_CASE(SMSG_PLAY_MUSIC, DT_PLAY_MUSIC_MESSAGE, PlayMusicMessage)
        MESSAGE_CASE(SMSG_STOP_MUSIC_OVERLAY, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_PLAY_CUTSCENE, DT_PLAY_CUTSCENE_MESSAGE, PlayCutsceneMessage)
        MESSAGE_CASE(SMSG_PLAY_ANNIVERSARY_CUTSCENE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_PLAY_FINAL_CUTSCENE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_FLIPPY, DT_FLIPPY_MESSAGE, FlippyMessage)
        MESSAGE_CASE(SMSG_NPC_INTERACT, DT_NPC_INTERACT_OPTIONS_MESSAGE, NPCInteractOptionsMessage)
        MESSAGE_CASE(SMSG_FORCE_NPC_INTERACT_CLOSE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_ATTACH_PET, DT_PET_MESSAGE, PetMessage)
        MESSAGE_CASE(SMSG_DETACH_PET, DT_PET_DETACH_MESSAGE, PetDetachMessage)
        MESSAGE_CASE(SMSG_SET_HIRELING_DEAD, DT_HIRELING_INFO_UPDATE_MESSAGE, HirelingInfoUpdateMessage)
        MESSAGE_CASE(SMSG_QUEST_UPDATE, DT_QUEST_UPDATE_MESSAGE, QuestUpdateMessage)
        MESSAGE_CASE(SMSG_QUEST_METER, DT_QUEST_METER_MESSAGE, QuestMeterMessage)
        MESSAGE_CASE(SMSG_QUEST_FLAGS, DT_QUEST_FLAGS_MESSAGE, QuestFlagsMessage)
        MESSAGE_CASE(SMSG_QUEST_COUNTER, DT_QUEST_COUNTER_MESSAGE, QuestCounterMessage)
        MESSAGE_CASE(SMSG_QUEST_REWARD, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_TIERED_RIFT_REWARD, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_PLAYER_LEVEL, DT_PLAYER_LEVEL, PlayerLevel)
        MESSAGE_CASE(SMSG_PLAYER_ALT_LEVEL, DT_PLAYER_LEVEL, PlayerLevel)
        MESSAGE_CASE(SMSG_PLAYER_HIGHEST_HERO_SOLO_RIFT_LEVEL, DT_PLAYER_INTVAL_MESSAGE, PlayerIntValMessage)
        MESSAGE_CASE(SMSG_ACTIVATE_WAYPOINT, DT_WAYPOINT_ACTIVATED_MESSAGE, WaypointActivatedMessage)
        MESSAGE_CASE(SMSG_OPERATE_WAYPOINT, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_CONFIRM_OPERATION, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_OPEN_BANNER_CUSTOMIZATION, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_OPEN_COSMETICS, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_OPERATE_GIZMO, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_ACCEPT_RESURRECT, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_AIM_TARGET, DT_AIM_TARGET_MESSAGE, AimTargetMessage)
        MESSAGE_CASE(SMSG_INSPECT, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(SMSG_ACD_SET_IDLE_ANIM, DT_SET_IDLE_ANIMATION_MESSAGE, SetIdleAnimationMessage)
        MESSAGE_CASE(SMSG_CRAFTER_OPEN, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_SHOP_OPEN, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_SHARED_STASH_OPEN, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_ACD_PICKUP_FAILED, DT_ACD_PICKUP_FAILED_MESSAGE, ACDPickupFailedMessage)
        MESSAGE_CASE(SMSG_TRICKLE_UPDATE, DT_TRICKLE_MESSAGE, TrickleMessage)
        MESSAGE_CASE(SMSG_TRICKLE_REMOVE, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_MAP_REVEAL_SCENE, DT_MAP_REVEAL_SCENE_MESSAGE, MapRevealSceneMessage)
        MESSAGE_CASE(SMSG_SAVEPOINT_INFO, DT_SAVE_POINT_INFO_MESSAGE, SavePointInfoMessage)
        MESSAGE_CASE(SMSG_HEARTHPORTAL_INFO, DT_HEARTHPORTAL_INFO_MESSAGE, HearthPortalInfoMessage)
        MESSAGE_CASE(SMSG_RETURNPOINT_INFO, DT_RETURN_POINT_INFO_MESSAGE, ReturnPointInfoMessage)
        MESSAGE_CASE(SMSG_PVP_ARENA_LOSE, DT_DATAID_DATA_MESSAGE, DataIDDataMessage)
        MESSAGE_CASE(SMSG_PVP_ARENA_WIN, DT_DATAID_DATA_MESSAGE, DataIDDataMessage)
        MESSAGE_CASE(CMSG_TRADE, DT_TRADE_MESSAGE, TradeMessage)
        MESSAGE_CASE(SMSG_TRADE, DT_TRADE_MESSAGE, TradeMessage)
        MESSAGE_CASE(SMSG_ACT, DT_ACT_TRANSITION_MESSAGE, ActTransitionMessage)
        MESSAGE_CASE(SMSG_INTERSTITIAL, DT_INTERSTITIAL_MESSAGE, InterstitialMessage)
        MESSAGE_CASE(SMSG_ROPE_EFFECT_ACD_TO_ACD, DT_ROPE_EFFECT_MESSAGE_ACD_TO_ACD, RopeEffectMessageACDToACD)
        MESSAGE_CASE(SMSG_ROPE_EFFECT_ACD_TO_PLACE, DT_ROPE_EFFECT_MESSAGE_ACD_TO_PLACE, RopeEffectMessageACDToPlace)
        MESSAGE_CASE(SMSG_UI_ELEMENT_ACTION, DT_UI_ELEMENT_MESSAGE, UIElementMessage)
        MESSAGE_CASE(CMSG_PLAYER_BUSY, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(CMSG_PLAYER_LORE_PLAYING, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_CHANGE_ACTOR, DT_ACD_CHANGE_ACTOR_MESSAGE, ACDChangeActorMessage)
        MESSAGE_CASE(SMSG_PLAYER_WARPED, DT_PLAYER_WARPED_MESSAGE, PlayerWarpedMessage)
        MESSAGE_CASE(SMSG_GAME_SYNCED_DATA, DT_GAME_SYNCED_DATA_MESSAGE, GameSyncedDataMessage)
        MESSAGE_CASE(CCMSG_END_OF_TICK, DT_END_OF_TICK_MESSAGE, EndOfTickMessage)
        MESSAGE_CASE(CMSG_REQUEST_LAN_GAME_PARAMS, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(CMSG_JOIN_BNET_GAME, DT_JOIN_BNET_GAME_MESSAGE, JoinBNetGameMessage)
        MESSAGE_CASE(CMSG_JOIN_LAN_GAME, DT_JOIN_LAN_GAME_MESSAGE, JoinLANGameMessage)
        MESSAGE_CASE(CMSG_JOIN_CONSOLE_GAME, DT_JOIN_CONSOLE_GAME_MESSAGE, JoinConsoleGameMessage)
        MESSAGE_CASE(CMSG_LEAVE_CONSOLE_GAME, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_START_LOGOUT_TIMER, DT_LOGOUT_CONTEXT_MESSAGE, LogoutContextMessage)
        MESSAGE_CASE(SMSG_LOGOUT_TIMER_STARTED, DT_LOGOUT_TICKTIME_MESSAGE, LogoutTickTimeMessage)
        MESSAGE_CASE(CMSG_LOGOUT_NOW, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_CANCEL_LOGOUT, DT_LOGOUT_CONTEXT_MESSAGE, LogoutContextMessage)
        MESSAGE_CASE(CMSG_ACCEPTED_DEATH, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_TARGET, DT_TARGET_MESSAGE, TargetMessage)
        MESSAGE_CASE(CMSG_SECONDARY_ANIMATION_POWER, DT_SECONDARY_ANIMATION_POWER_MESSAGE, SecondaryAnimationPowerMessage)
        MESSAGE_CASE(CMSG_LOOPING_ANIMATION_POWER, DT_LOOPING_ANIMATION_POWER_MESSAGE, LoopingAnimationPowerMessage)
        MESSAGE_CASE(CMSG_EXECUTE_NON_PRIMARY_POWER, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(CMSG_CHANNELLED_POWER_ABORTED, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(CMSG_LOOPING_POWER_ABORTED, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(CMSG_CLIENT_WALK_POWER_ABORTED, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(CMSG_TRY_CONSOLE_COMMAND, DT_TRY_CONSOLE_COMMAND, TryConsoleCommand)
        MESSAGE_CASE(SMSG_TRY_CONSOLE_COMMAND, DT_TRY_CONSOLE_COMMAND, TryConsoleCommand)
        MESSAGE_CASE(CMSG_TRY_CHAT, DT_TRY_CHAT_MESSAGE, TryChatMessage)
        MESSAGE_CASE(CMSG_TRY_WAYPOINT, DT_TRY_WAYPOINT_MESSAGE, TryWaypointMessage)
        MESSAGE_CASE(CMSG_ACD_TRANSLATE_NORMAL, DT_ACD_CLIENT_TRANSLATE_MESSAGE, ACDClientTranslateMessage)
        MESSAGE_CASE(CMSG_ACD_TRANSLATE_FACING, DT_ACD_TRANSLATE_FACING_MESSAGE, ACDTranslateFacingMessage)
        MESSAGE_CASE(CMSG_ACD_TRANSLATE_ACK, DT_ACD_TRANSLATE_ACK_MESSAGE, ACDTranslateAckMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_SWAP_TO_BODY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_MOVE, DT_INVENTORY_REQUEST_MOVE_MESSAGE, InventoryRequestMoveMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_QUICK_MOVE, DT_INVENTORY_REQUEST_QUICK_MOVE_MESSAGE, InventoryRequestQuickMoveMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_STACK, DT_INVENTORY_REQUEST_MOVE_MESSAGE, InventoryRequestMoveMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_SPLIT_STACK, DT_INVENTORY_SPLIT_STACK_MESSAGE, InventorySplitStackMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_DROP_STACK_PORTION, DT_STACK_PORTION_MESSAGE, StackPortionMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_STACK_TRANSFER, DT_INVENTORY_STACK_TRANSFER_MESSAGE, InventoryStackTransferMessage)
        MESSAGE_CASE(CMSG_TRY_GOLD_TRANSFER, DT_GOLD_TRANSFER_MESSAGE, GoldTransferMessage)
        MESSAGE_CASE(SMSG_STACK_NOTIFY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_STACK_DECREASE_NOTIFY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_IDENTIFY_NOTIFY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_DYE_NOTIFY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_ITEM_DELETE_NOTIFY, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_SOCKET, DT_INVENTORY_REQUEST_SOCKET_MESSAGE, InventoryRequestSocketMessage)
        MESSAGE_CASE(CMSG_TRY_INVENTORY_USE, DT_INVENTORY_REQUEST_USE_MESSAGE, InventoryRequestUseMessage)
        MESSAGE_CASE(CMSG_HELPER_DETACH, DT_HELPER_DETACH_MESSAGE, HelperDetachMessage)
        MESSAGE_CASE(CMSG_TRY_ASSIGN_SKILL_TO_SLOT, DT_ASSIGN_SKILL_MESSAGE, AssignSkillMessage)
        MESSAGE_CASE(CMSG_UNASSIGN_SKILL, DT_UNASSIGN_SKILL_MESSAGE, UnassignSkillMessage)
        MESSAGE_CASE(CMSG_TRY_ASSIGN_TRAITS, DT_ASSIGN_TRAITS_MESSAGE, AssignTraitsMessage)
        MESSAGE_CASE(CMSG_TRY_ASSIGN_LEGENDARY_POWER, DT_ASSIGN_LEGENDARY_POWER_MESSAGE, AssignLegendaryPowerMessage)
        MESSAGE_CASE(SMSG_ASSIGNED_SKILL_TO_SLOT, DT_ASSIGN_SKILL_MESSAGE, AssignSkillMessage)
        MESSAGE_CASE(SMSG_ASSIGNED_TRAIT_TO_SLOT, DT_ASSIGN_SKILL_MESSAGE, AssignSkillMessage)
        MESSAGE_CASE(SMSG_ASSIGNED_LEGENDARY_POWER_TO_SLOT, DT_ASSIGN_LEGENDARY_POWER_MESSAGE, AssignLegendaryPowerMessage)
        MESSAGE_CASE(CMSG_TRY_CHOOSE_HIRELING_SKILL, DT_HIRELING_REQUEST_LEARN_SKILL_MESSAGE, HirelingRequestLearnSkillMessage)
        MESSAGE_CASE(CMSG_VENDOR_CLOSE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_TRY_DROP, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_PORTRAIT_DROP, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_SHOP_TRY_BUY, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_SHOP_TRY_SELL, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_SHOP_TRY_SELL_STACK_PORTION, DT_STACK_PORTION_MESSAGE, StackPortionMessage)
        MESSAGE_CASE(CMSG_PLAYER_CHANGE_POTION_BUTTON, DT_CHANGE_USABLE_ITEM_MESSAGE, ChangeUsableItemMessage)
        MESSAGE_CASE(SMSG_AUTOASSIGNED_TO_BUTTON, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_INSPECT_START, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_INSPECT_END, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_NPC_TALK_REQUEST, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_NPC_VENDOR_REQUEST, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_WORLD_STATUS, DT_WORLD_STATUS_MESSAGE, WorldStatusMessage)
        MESSAGE_CASE(SMSG_TRIGGER_LIGHTNING, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SCOMPLEX_EFFECT_ADD, DT_COMPLEX_EFFECT_ADD_MESSAGE, ComplexEffectAddMessage)
        MESSAGE_CASE(SMSG_EFFECT_GROUP_ACD_TO_ACD, DT_EFFECT_GROUP_ACD_TO_ACD_MESSAGE, EffectGroupACDToACDMessage)
        MESSAGE_CASE(SMSG_SHEAR, DT_ACD_SHEAR_MESSAGE, ACDShearMessage)
        MESSAGE_CASE(SMSG_GROUP, DT_ACD_GROUP_MESSAGE, ACDGroupMessage)
        MESSAGE_CASE(CMSG_KEEPALIVE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_FINISHED_GAME_SETUP, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_FINISHED_GAME_SETUP_DONE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_PLAY_CONV_LINE, DT_PLAY_CONV_LINE_MESSAGE, PlayConvLineMessage)
        MESSAGE_CASE(SMSG_STOP_CONV_LINE, DT_STOP_CONV_LINE_MESSAGE, StopConvLineMessage)
        MESSAGE_CASE(SMSG_END_CONV, DT_CONV_END_MESSAGE, EndConversationMessage)
        MESSAGE_CASE(SMSG_CONFIRM_HIRELING_SWAP, DT_HIRELING_SWAP_MESSAGE, HirelingSwapMessage)
        MESSAGE_CASE(SMSG_HIRELING_AUTO_DISMISS, DT_HIRELING_SWAP_MESSAGE, HirelingSwapMessage)
        MESSAGE_CASE(CMSG_REQUEST_HIRELING_SWAP, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_REQUEST_CONV_ADVANCE, DT_ADVANCE_CONV_MESSAGE, AdvanceConvMessage)
        MESSAGE_CASE(CMSG_UPDATE_CONV_AUTO_ADVANCE, DT_UPDATE_CONV_AUTO_ADVANCE_MESSAGE, UpdateConvAutoAdvanceMessage)
        MESSAGE_CASE(CMSG_REQUEST_CONV_CANCEL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SET_DEATH_FADE_TIMES, DT_DEATH_FADE_TIME_MESSAGE, DeathFadeTimeMessage)
        MESSAGE_CASE(SMSG_RESET_DEATH_FADE_TIMES, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_REMOVE_FROM_SPHERE_LIST, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_DISPLAY_GAME_TEXT, DT_DISPLAY_GAME_TEXT_MESSAGE, DisplayGameTextMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_START, DT_SET_DUNGEON_DIALOG_MESSAGE, SetDungeonDialogMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_CANNOT_START, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_CLOSING, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_WARP_DEAD_PLAYER, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_REQUEST_SOFT_PAUSE_STATE_CHANGE, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_ACTOR_LOOK_OVERRIDE_CHANGED, DT_ACTOR_LOOK_OVERRIDE_CHANGED, ActorLookOverrideChangedMessage)
        MESSAGE_CASE(SMSG_DYNAMIC_QUEST_OBJECTIVE_UPDATE, DT_DYNAMIC_QUEST_OBJECTIVE_MESSAGE, DynamicQuestObjectiveMessage)
        MESSAGE_CASE(CMSG_TELEPORT_TO_PLAYER, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_TELEPORT_TO_WAYPOINT, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(CMSG_SUMMON_VANITY_PET, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(SMSG_DYNAMIC_QUEST_RESET, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_WORLD_SYNCED_DATA, DT_WORLD_SYNCED_DATA_MESSAGE, WorldSyncedDataMessage)
        MESSAGE_CASE(SMSG_DUNGEON_FINDER_SEED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_DUNGEON_FINDER_PROGRESS, DT_FLOAT_DATA_MESSAGE, FloatDataMessage)
        MESSAGE_CASE(SMSG_DUNGEON_FINDER_PROGRESS_GLYPH_PICK_UP, DT_FLOAT_DATA_MESSAGE, FloatDataMessage)
        MESSAGE_CASE(SMSG_DUNGEON_FINDER_SET_TIMEDEVENT, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(SMSG_DUNGEON_FINDER_PARTICIPATING_PLAYER_COUNT, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_DUNGEON_FINDER_BOSS_START, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_NEW_SKILLKIT, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_OPEN_WORLD_MODE_CHANGE_PROMPT, DT_OPEN_WORLD_MODE_CHANGE_MESSAGE, OpenWorldModeChangeMessage)
        MESSAGE_CASE(CMSG_OPEN_WORLD_MODE_CHANGE_REQUEST, DT_OPEN_WORLD_MODE_CHANGE_MESSAGE, OpenWorldModeChangeMessage)
        MESSAGE_CASE(SMSG_OPEN_WORLD_MODE_CHANGE, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_OPEN_WORLD_MODE_CHANGE_ACCEPT, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_OPEN_WORLD_MODE_RESET_REQUEST, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_OPEN_WORLD_TRACKED_SNO, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(SMSG_PLAY_OPEN_WORLD_TUTORIAL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_GAMBLE, DT_GBID_DATA_MESSAGE, GBIDDataMessage)
        MESSAGE_CASE(CMSG_TRY_LOWER_DIFFICULTY, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_DIFFICULTY_LOWERED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_DIFFICULTY_LOWERED_SAVE_PREFS, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_DIFFICULTY_LOWERED_FLAG, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_TRY_RAISE_DIFFICULTY, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_DIFFICULTY_RAISED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_DIFFICULTY_RAISED_SAVE_PREFS, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_DIFFICULTY_RAISED_FLAG, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_HIRELING_UNLOCKED, DT_HIRELING_SWAP_MESSAGE, HirelingSwapMessage)
        MESSAGE_CASE(CMSG_TRY_CHANGE_DIFFICULTY, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_DIFFICULTY_CHANGED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_MAILBOX_OPEN, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_MAIL_ITEM_SENT, DT_STACK_PORTION_MESSAGE, StackPortionMessage)
        MESSAGE_CASE(CMSG_MAIL_CURRENCY_SENT, DT_CURRENCY_MESSAGE, CurrencyMessage)
        MESSAGE_CASE(CMSG_MAIL_CURRENCY_RECEIVED, DT_CURRENCY_MESSAGE, CurrencyMessage)
        MESSAGE_CASE(CMSG_MAIL_ITEM_RECEIVED, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_MAIL_ITEM_RECEIVED_EQUIP, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_GIFT_ITEM_RECEIVED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_AVENGER_CREATION, DT_AVENGER_INFO_MESSAGE, AvengerInfoMessage)
        MESSAGE_CASE(SMSG_AVENGER_ENTER_KNOWN_LIST, DT_AVENGER_INFO_MESSAGE, AvengerInfoMessage)
        MESSAGE_CASE(SMSG_AVENGER_SPAWN, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_AVENGER_KILLED, DT_AVENGER_INFO_MESSAGE, AvengerInfoMessage)
        MESSAGE_CASE(CMSG_AVENGER_SEND_ACCOUNT_INFO, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_AVENGER_SEND_FRIEND_AVENGER, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_AVENGER_UPDATE_SOURCE, DT_AVENGER_UPDATE_SOURCE_MESSAGE, AvengerUpdateSourceMessage)
        MESSAGE_CASE(CMSG_SET_FRIEND_STATUS, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(CMSG_PLAYER_GIFT_REGISTER_RECIPIENT, DT_PLAYER_GIFT_MESSAGE, PlayerGiftMessage)
        MESSAGE_CASE(SMSG_PLAYER_GIFT_REGISTER_RECIPIENT, DT_PLAYER_GIFT_MESSAGE, PlayerGiftMessage)
        MESSAGE_CASE(SMSG_PLAYER_GIFT_ITEM_GENERATED, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_PLAYER_GIFT_ITEM_INVENTORY_FULL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RIFT_POST_SCORE, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_SEND_SEASONAL_ITEM_TO_STASH, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_SEND_SEASONAL_ITEM_TO_INVENTORY, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_STASH_ITEM_RETRIEVED, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_UPDATED_VENDOR_RANDOM_SEEDS, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_UPDATED_PARTITION_WIDE_CUBE_RANDOM_SEEDS, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_DROP_CRAFTING_MATERIAL, DT_CURRENCY_MESSAGE, CurrencyMessage)
        MESSAGE_CASE(CMSG_DESTROY_GIFT, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_PLAYER_GIFT_SENT, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_UPSCALING_LEVEL_CHANGED, DT_PLAYER_DWORD_DATA_MESSAGE, PlayerDWordDataMessage)
        MESSAGE_CASE(SMSG_DUNGEON_FINDER_CLOSING, DT_DUNGEON_FINDER_CLOSING_MESSAGE, DungeonFinderClosingMessage)
        MESSAGE_CASE(CMSG_FINISHED_OPEN_WORLD_TUTORIAL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_FORCE_UI_RESET, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_UPDATED_COSMETIC_ITEM_DATA, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CLOSE_UI_COUNTDOWN, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_PLAYER_CHOOSE_RIFT_REWARD_TYPE, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_FORCE_HEALTH_RESYNC, DT_FLOAT_DATA_MESSAGE, FloatDataMessage)
        MESSAGE_CASE(SMSG_WEEKLY_CHALLENGE_COMPLETED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_CONSOLE_SYNC_HERO_PARTITION_FLAGS, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_DEBUG_DRAW_MOVEMENT_TYPE, DT_DEBUG_DRAW_MOVEMENT_TYPE_MESSAGE, DebugDrawMovementTypeMessage)
        MESSAGE_CASE(CMSG_EQUIP_COSMETIC_ITEM, DT_GBID_DATA_MESSAGE, GBIDDataMessage)
        MESSAGE_CASE(CMSG_UNEQUIP_COSMETIC_ITEM, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_CURSED_REALM_RESULTS, DT_HORADRIC_QUEST_CURSED_REALM_RESULTS_MESSAGE, HoradricQuestCursedRealmResults)
        MESSAGE_CASE(SMSG_FIRST_OF_THE_DAY, DT_FIRST_OF_THE_DAY_MESSAGE, FirstOfTheDayMessage)
        MESSAGE_CASE(SMSG_PLATINUM_AWARDED, DT_PLATINUM_AWARDED_MESSAGE, PlatinumAwardedMessage)
        MESSAGE_CASE(SMSG_PLATINUM_ACHIEVEMENT_AWARDED, DT_PLATINUM_ACHIEVEMENT_AWARDED_MESSAGE, PlatinumAchievementAwardedMessage)
        MESSAGE_CASE(SMSG_RIFT_PARTICIPANTS_ADDED, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_SHOW_RIFT_END_SCREEN, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_PRELOAD_LOOT_RUN_LEADERBOARD_INFO, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_RESULTS_MESSAGE, DT_SET_DUNGEON_RESULTS_MESSAGE, SetDungeonResultsMessage)
        MESSAGE_CASE(CMSG_EQUIPMENT_MANAGER_CLOSE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_REQUEST_LOADOUT, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_EQUIP_LOADOUT, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_SAVE_LOADOUT, DT_PLAYER_LOADOUT_SAVE_MESSAGE, PlayerLoadoutSaveMessage)
        MESSAGE_CASE(CMSG_CLEAR_LOADOUT, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_SET_LOADOUT_SLOT, DT_PLAYER_LOADOUT_SET_SLOT_MESSAGE, PlayerLoadoutSetSlotMessage)
        MESSAGE_CASE(CMSG_SET_LOADOUT_TAB_ICON, DT_PLAYER_LOADOUT_TAB_ICON_MESSAGE, PlayerLoadoutTabIconMessage)
        MESSAGE_CASE(CMSG_SET_LOADOUT_NAME, DT_PLAYER_LOADOUT_SAVE_MESSAGE, PlayerLoadoutSaveMessage)
        MESSAGE_CASE(SMSG_EQUIPMENT_MANAGER_OPEN, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_LOADOUT_DATA, DT_PLAYER_LOADOUT_DATA_MESSAGE, PlayerLoadoutDataMessage)
        MESSAGE_CASE(SMSG_LOADOUT_SAVED, DT_PLAYER_LOADOUT_DATA_MESSAGE, PlayerLoadoutDataMessage)
        MESSAGE_CASE(SMSG_LOADOUT_EMPTY, DT_PLAYER_LOADOUT_TAB_ICON_MESSAGE, PlayerLoadoutTabIconMessage)
        MESSAGE_CASE(SMSG_LOADOUT_UPDATE, DT_PLAYER_LOADOUT_EQUIPMENT_UPDATE_MESSAGE, PlayerLoadoutEquipmentUpdateMessage)
        MESSAGE_CASE(SMSG_LOADOUT_EQUIP_NOTIFICATION, DT_PLAYER_LOADOUT_EQUIP_RESULTS_MESSAGE, PlayerLoadoutEquipResultsMessage)
        MESSAGE_CASE(CMSG_SWAP_HOTBAR_SKILLS, DT_SWAP_SKILLS_MESSAGE, SwapSkillsMessage)
        MESSAGE_CASE(CMSG_REDEEM_SEASON_REWARD_CONSOLE, DT_GBID_DATA_MESSAGE, GBIDDataMessage)
        MESSAGE_CASE(SMSG_REDEEM_SEASON_REWARD_CONSOLE, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_CHALLENGE_RIFT_REWARD_CONSOLE, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CHALLENGE_RIFT_REWARD_PC, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_CHALLENGE_RIFT_CLAIM_REWARD_CONSOLE, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_CHALLENGE_RIFT_INSPECT_ARMORRACK, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_DUNGEON_FINDER_COMPLETION_TIME, DT_SET_DUNGEON_FINDER_TIME_MESSAGE, DungeonFinderCompletionTimeMessage)
        MESSAGE_CASE(SMSG_GREED_REALM_CLOSING, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_SWARM_RIFT_CLOSING, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_UBER_BOSS_WORLD_CLOSING, DT_UBER_BOSSS_CLOSING_MESSAGE, UberBosssClosingMessage)
        MESSAGE_CASE(SMSG_WARNING_COUNTDOWN_NOTIFY, DT_WARNING_COUNTDOWN_NOTIFICATION_MESSAGE, WarningCountdownNotificationMessage)
        MESSAGE_CASE(SMSG_AMIIBO_SCAN_BEGIN, DT_ACD_LOOK_AT_MESSAGE, ACDLookAtMessage)
        MESSAGE_CASE(SMSG_AMIIBO_SCAN_END, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_SYNC_AMIIBO_SCAN_TIME, DT_PLAYER_DWORD_DATA_MESSAGE, PlayerDWordDataMessage)
        MESSAGE_CASE(CMSG_AMIIBO_SCAN_END, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_HAS_NFP_DEVICE, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_LOST_NFP_DEVICE, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_PLAYER_CHEATED_DEATH, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        // 2.7.7: SMSG_UNLOCKABLE_RANDOM_AFFIX (UnlockableRandomAffixMessage), CMSG_DARK_ALCHEMY_ACTIVATE_NODE
        // (ActivateNodeMessage) go here once their descriptors exist.
        MESSAGE_CASE(SMSG_DARK_ALCHEMY_NODE_ACTIVATED, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        // 2.7.7: SMSG_DARK_ALCHEMY_INTERACT (GizmoInteractMessage) goes here once its descriptor exists.
        MESSAGE_CASE(SMSG_CHUNKED, DT_CHUNK_MESSAGE, ChunkMessage)
        MESSAGE_CASE(CMSG_SET_DUNGEON_ACCEPT, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_DISPLAY_GAME_CHAT_TEXT, DT_DISPLAY_GAME_TEXT_MESSAGE, DisplayGameTextMessage)
        MESSAGE_CASE(SMSG_DISPLAY_ERROR, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_DISPLAY_VAGUE_LOCKED_ERROR, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_DISPLAY_ITEM_REQUIRED_ERROR, DT_GBID_DATA_MESSAGE, GBIDDataMessage)
        MESSAGE_CASE(SMSG_CANCEL_ACD_TARGET, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_RESET_ACD_EFFECTS, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_LOOK_AT_ACD, DT_ACD_LOOK_AT_MESSAGE, ACDLookAtMessage)
        MESSAGE_CASE(SMSG_KILL_COUNTER_UPDATE, DT_KILL_COUNTER_UPDATE_MESSAGE, KillCounterUpdateMessage)
        MESSAGE_CASE(SMSG_CURRENCY_COUNTER_UPDATE, DT_CURRENCY_COUNTER_UPDATE_MESSAGE, CurrencyCounterUpdateMessage)
        MESSAGE_CASE(SMSG_KILL_COUNTER_REFRESH, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_LOW_HEALTH_COMBAT, DT_LOW_HEALTH_COMBAT_MESSAGE, LowHealthCombatMessage)
        MESSAGE_CASE(SMSG_SAVIOR, DT_SAVIOR_MESSAGE, SaviorMessage)
        MESSAGE_CASE(SMSG_FLOATING_NUMBER, DT_FLOATING_NUMBER_MESSAGE, FloatingNumberMessage)
        MESSAGE_CASE(SMSG_FLOATING_GBID_AMOUNT, DT_FLOATING_GBID_AMOUNT_MESSAGE, FloatingGBIDAmountMessage)
        MESSAGE_CASE(SMSG_FLOATING_AMOUNT, DT_FLOATING_AMOUNT_MESSAGE, FloatingAmountMessage)
        MESSAGE_CASE(SMSG_REMOVE_NEAREST_RAGDOLL, DT_REMOVE_RAGDOLL_MESSAGE, RemoveRagdollMessage)
        MESSAGE_CASE(SMSG_PREFETCH, DT_SNONAME_DATA_MESSAGE, SNONameDataMessage)
        MESSAGE_CASE(SMSG_LORE_LEARNED, DT_LORE_MESSAGE, LoreMessage)
        MESSAGE_CASE(CMSG_AV_CAMERA_MODE_TOGGLE, DT_PRTRANSFORM_MESSAGE, PRTransformMessage)
        MESSAGE_CASE(CMSG_AV_CAMERA_LOCATION, DT_PRTRANSFORM_MESSAGE, PRTransformMessage)
        MESSAGE_CASE(SMSG_TRIAL_OVER, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(SMSG_DEMO_OVER, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMGS_BLIZZCON_RIFT_ENDED, DT_BLIZZCON_END_SCREEN_MESSAGE, BlizzconEndScreenMessage)
        MESSAGE_CASE(SMSG_BETA_OVER, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_WORLD_DELETED, DT_WORLD_DELETED_MESSAGE, WorldDeletedMessage)
        MESSAGE_CASE(SMSG_PVP_ARENA_RESTART, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SET_SCREENSHOT_VIEW, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_TIMED_EVENT_STARTED, DT_TIMED_EVENT_STARTED_MESSAGE, TimedEventStartedMessage)
        MESSAGE_CASE(SMSG_TIMED_EVENT_MODIFIED, DT_TIMED_EVENT_STARTED_MESSAGE, TimedEventStartedMessage)
        MESSAGE_CASE(SMSG_TIMED_EVENT_FINISHED, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(SMSG_TIMED_EVENT_RESET, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(SMSG_ACT_TRANSITION_STARTED, DT_ACT_TRANSITION_STARTED_MESSAGE, ActTransitionStartedMessage)
        MESSAGE_CASE(SMSG_ACT_TRANSITION_ENDED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SHARED_STASH_TRY_BUY, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_PLAYER_ACTIVATED_QUEST, DT_PLAYER_QUEST_MESSAGE, PlayerQuestMessage)
        MESSAGE_CASE(SMSG_PLAYER_COMPLETED_QUEST, DT_PLAYER_QUEST_MESSAGE, PlayerQuestMessage)
        MESSAGE_CASE(SMSG_PLAYER_DESYNC_SNAP, DT_PLAYER_DESYNC_SNAP_MESSAGE, PlayerDeSyncSnapMessage)
        MESSAGE_CASE(CMSG_TRY_SALVAGE, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_SALVAGE_ALL, DT_TRY_SALVAGE_ALL_MESSAGE, TrySalvageAllMessage)
        MESSAGE_CASE(SMSG_SALVAGE_RESULT, DT_SALVAGE_RESULTS_MESSAGE, SalvageResultsMessage)
        MESSAGE_CASE(SMSG_RETURNED_GEMS_NOTIFICATION, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_COMBAT_LOG_ENTRY, CombatLog::DT_COMBAT_LOG_MESSAGE, CombatLog::Message)
        MESSAGE_CASE(CMSG_COMBAT_LOG_TOGGLE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_MAP_MARKER, DT_MAP_MARKER_MESSAGE, MapMarkerInfoMessage)
        MESSAGE_CASE(SMSG_MAP_OBJECTIVE_MARKER, DT_MAP_MARKER_MESSAGE, MapMarkerInfoMessage)
        MESSAGE_CASE(SMSG_OBJECTIVE_FINDER_ACTIVATED, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_BLACKSMITH, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_JEWELER, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_MYSTIC, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_HORADRIM, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_TEST, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_TRANSMOG, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_BLACKSMITH_INITIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_JEWELER_INITIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_MYSTIC_INITIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_HORADRIM_INITIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_TEST_INITIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CRAFTING_DATA_TRANSMOG_INITIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CURRENCY_DATA_FULL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_CURRENCY_DATA_PARTIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_EXTRACTED_LEGENDARIES_DATA_FULL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_EXTRACTED_LEGENDARIES_DATA_PARTIAL, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_LOAD_LEGENDARY_POWER_ICONS, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RESET_EXTRACTED_LEGENDARIES, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_REQUEST_DEBUG_ACTOR_TOOLTIP, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_DISPLAY_DEBUG_ACTOR_TOOLTIP, DT_DISPLAY_ACTOR_DEBUG_TOOLTIP, DebugActorTooltipMessage)
        MESSAGE_CASE(SMSG_BOSS_JOIN_ENCOUNTER, DT_BOSS_ENCOUNTER_MESSAGE, BossEncounterMessage)
        MESSAGE_CASE(CMSG_BOSS_JOIN_ACCEPTED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_BOSS_START_ACCEPTED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_BOSS_START_ENCOUNTER, DT_BOSS_ENCOUNTER_MESSAGE, BossEncounterMessage)
        MESSAGE_CASE(CMSG_BOSS_START_DECLINED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_BOSS_JOIN_DECLINED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_BOSS_ENCOUNTER_INVITE_STATE, DT_BOSS_ENCOUNTER_INVITE_MESSAGE, EncounterInviteStateMessage)
        MESSAGE_CASE(SMSG_BOSS_ENCOUNTER_STARTED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_BOSS_ENCOUNTER_CANCELED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_BOSS_ENCOUNTER_ONE_DECLINED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_BOSS_VOTE_CONTINUE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_BOSS_VOTE_CANCEL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_ENCOUNTER_INVITE_STATE, DT_BOSS_ENCOUNTER_INVITE_MESSAGE, EncounterInviteStateMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_CANCELED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_ONE_DECLINED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_JOIN, DT_SET_DUNGEON_DIALOG_MESSAGE, SetDungeonDialogMessage)
        MESSAGE_CASE(SMSG_SET_DUNGEON_STARTED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SET_DUNGEON_JOIN_DECLINED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SET_DUNGEON_JOIN_ACCEPTED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SET_DUNGEON_VOTE_CONTINUE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SET_DUNGEON_VOTE_CANCEL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RIFT_START_ENCOUNTER, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_RIFT_START_ACCEPTED, DT_RIFT_START_ACCEPTED_MESSAGE, RiftStartAcceptedMessage)
        MESSAGE_CASE(CMSG_RIFT_START_DECLINED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RIFT_INVITE_STATE, DT_BOSS_ENCOUNTER_INVITE_MESSAGE, EncounterInviteStateMessage)
        MESSAGE_CASE(SMSG_RIFT_JOIN_ENCOUNTER, DT_RIFT_JOIN_MESSAGE, RiftJoinMessage)
        MESSAGE_CASE(CMSG_RIFT_JOIN_DECLINED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_RIFT_JOIN_ACCEPTED, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(CMSG_RIFT_JOIN_INFO_REQUEST, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RIFT_STARTED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RIFT_CANCELED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RIFT_ONE_DECLINED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_RIFT_VOTE_CONTINUE, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(CMSG_RIFT_VOTE_CANCEL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_RIFT_PAYMENT_WARNING, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_RIFT_PAYMENT_ACCEPTED, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_RIFT_START_PRELOADING_LEADERBOARD, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_TIERED_RIFT_CLOSE_REQUESTED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_CAMERA_SCRIPTED_SEQUENCE_START, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_CAMERA_SCRIPTED_SEQUENCE_STOP, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_CAMERA_FOCUS_ACD, DT_CAMERA_FOCUS_MESSAGE, CameraFocusMessage)
        MESSAGE_CASE(SMSG_CAMERA_ZOOM, DT_CAMERA_ZOOM_MESSAGE, CameraZoomMessage)
        MESSAGE_CASE(SMSG_CAMERA_YAW, DT_CAMERA_YAW_MESSAGE, CameraYawMessage)
        MESSAGE_CASE(SMSG_CAMERA_TRIGGER_FADE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_CAMERA_TRIGGER_FADE_TO_BLACK, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_CAMERA_TRIGGER_INSTANT_FADE_TO_BLACK, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_BOSS_ZOOM, DT_BOSS_ZOOM_MESSAGE, BossZoomMessage)
        MESSAGE_CASE(CMSG_TRY_REFORGE, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_REFORGE_RESULTS, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_ENCHANT_AFFIX, DT_ENCHANT_AFFIX_MESSAGE, EnchantAffixMessage)
        MESSAGE_CASE(SMSG_CHOOSE_ENCHANT_AFFIX, DT_ENCHANT_AFFIX_CHOOSE_MESSAGE, EnchantAffixChooseMessage)
        MESSAGE_CASE(CMSG_CHOOSE_ENCHANT_AFFIX, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_ENCHANT_AFFIX_RESULTS, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_REPAIR, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_REPAIR_ALL, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_TRY_REPAIR_EQUIPPED, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_REPAIR_ACK, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_UNLOCK_TRANSMOG_RESULTS, DT_GBID_DATA_MESSAGE, GBIDDataMessage)
        MESSAGE_CASE(CMSG_TRY_TRANSMOG, DT_TRANSMOG_ITEM_MESSAGE, TransmogItemMessage)
        MESSAGE_CASE(SMSG_TRANSMOG_RESULTS, DT_TRANSMOG_ITEM_MESSAGE, TransmogItemMessage)
        MESSAGE_CASE(CMSG_TRY_DYE_ITEM, DT_DYE_ITEM_MESSAGE, DyeItemMessage)
        MESSAGE_CASE(SMSG_DYE_ITEM_RESULTS, DT_DYE_ITEM_MESSAGE, DyeItemMessage)
        MESSAGE_CASE(CMSG_UNLOCKED_DYES_REQUEST, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(SMSG_UNLOCKED_DYES_RESULTS, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)
        MESSAGE_CASE(CMSG_TRY_TRANSMUTE, DT_TRANSMUTE_ITEMS_MESSAGE, TransmuteItemsMessage)
        MESSAGE_CASE(SMSG_TRANSMUTE_RESULTS, DT_TRANSMUTE_RESULTS_MESSAGE, TransmuteResultsMessage)
        MESSAGE_CASE(SMSG_TRANSMUTE_FAILED, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(CMSG_VENDOR_TRY_TRAIN, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_VENDOR_TRAIN_RESULTS, DT_INT_DATA_MESSAGE, IntDataMessage)
        MESSAGE_CASE(SMSG_MYSTERY_ITEM_RESULTS, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_UPGRADE_JEWEL, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_UPGRADE_JEWEL_RESULTS, DT_JEWEL_UPGRADE_RESULTS_MESSAGE, JewelUpgradeResultsMessage)
        MESSAGE_CASE(SMSG_DEBUG_DRAW_PRIM, DT_DEBUG_DRAW_PRIM_MESSAGE, DebugDrawPrimMessage)
        MESSAGE_CASE(SMSG_DEBUG_UNLUCKY_BONUS, DT_FLOAT_DATA_MESSAGE, FloatDataMessage)
        MESSAGE_CASE(CMSG_TRY_CRAFT_ITEM, DT_GBID_DATA_MESSAGE, GBIDDataMessage)
        MESSAGE_CASE(CMSG_TRY_CRAFT_ITEMS, DT_CRAFT_ITEMS_MESSAGE, CraftItemsMessage)
        MESSAGE_CASE(SMSG_CRAFTING_RESULT, DT_CRAFTING_RESULT_MESSAGE, CraftingResultsMessage)
        MESSAGE_CASE(SMSG_VENDOR_LEVEL_UP, DT_CRAFTER_LEVEL_UP_MESSAGE, CrafterLevelUpMessage)
        MESSAGE_CASE(CMSG_TRY_IDENTIFY_WITH_CAST, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(CMSG_TRY_IDENTIFY_ALL_WITH_CAST, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_TRY_REMOVE_GEMS, DT_ANN_DATA_MESSAGE, ANNDataMessage)
        MESSAGE_CASE(SMSG_CALLOUT_EVENT, DT_CALLOUT_MESSAGE, CalloutMessage)
        MESSAGE_CASE(SMSG_CURRENT_HANDICAP_LEVEL, DT_HANDICAP_MESSAGE, HandicapMessage)
        MESSAGE_CASE(CMSG_FOLLOWER_RETRAIN_REQUEST, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_GAME_TESTING_DONE, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(SMSG_GAME_TESTING_START_WORLD_SAMPLING, DT_GAME_TESTING_WORLD_SAMPLING_START_MESSAGE, GameTestingWorldSamplingStartMessage)
        MESSAGE_CASE(SMSG_GAME_TESTING_START_SKILL_SAMPLING, DT_GAME_TESTING_SKILL_SAMPLING_START_MESSAGE, GameTestingSkillSamplingStartMessage)
        MESSAGE_CASE(SMSG_GAME_TESTING_STOP_SAMPLING, DT_SIMPLE_MESSAGE, SimpleMessage)
        MESSAGE_CASE(CMSG_SHOWN_TUTORIAL, DT_SNO_DATA_MESSAGE, SNODataMessage)
        MESSAGE_CASE(CMSG_REQUEST_BUFF_CANCEL, DT_REQUEST_BUFF_CANCEL_MESSAGE, RequestBuffCancelMessage)
        MESSAGE_CASE(CMSG_REQUEST_EVENT_SKIP, DT_BOOL_DATA_MESSAGE, BoolDataMessage)
        MESSAGE_CASE(SMSG_HANDLE_EVENT_SKIP, DT_PLAYER_INDEX_MESSAGE, PlayerIndexMessage)
        MESSAGE_CASE(CMSG_REQUEST_PROMOTE_GAME_TO_PUBLIC, OnlineService::DT_OPEN_GAME_TO_PUBLIC_MESSAGE, OnlineService::OpenGameToPublicMessage)
        MESSAGE_CASE(SMSG_GAME_PROMOTED_TO_PUBLIC, OnlineService::DT_OPEN_GAME_TO_PUBLIC_MESSAGE, OnlineService::OpenGameToPublicMessage)
        MESSAGE_CASE(CMSG_SEND_PLAYER_PREFS, DT_DWORD_DATA_MESSAGE, DWordDataMessage)
        MESSAGE_CASE(XMSG_SAVE_BANNER_COMPLETE, DT_GENERIC_BLOB_MESSAGE, GenericBlobMessage)

        default:
            return FALSE;
    }

    return TRUE;
}

inline BOOL GenericEncode(XEncoder & tEncoder, const XTypeDescriptor * ptDescriptor, void * pMessage)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    TypeDescriptorField field(ptDescriptor, NULL);
    return ptDescriptor->Encode(pMessage, field, tEncoder);
}

BOOL EncodeMessage(void * pMessage, XEncoder & tEncoder)
{
    RequiredMessageHeader * ptHeader = (RequiredMessageHeader *)pMessage;
    const XTypeDescriptor * pTypeDescriptor = NULL;
    BOOL bRetVal;
    uint32 dwSize;
    const char * pszLabel;

    GetGameMessageTypeAndSize(ptHeader->eType, pTypeDescriptor, dwSize, pszLabel);

    uint32 dwUsedBits = tEncoder.Used();
    bRetVal = GenericEncode(tEncoder, pTypeDescriptor, pMessage);

    if (bRetVal == TRUE)
    {
        if (pTypeDescriptor == DT_GENERIC_BLOB_MESSAGE)
        {
            GenericBlobMessage * pBlobMessage = (GenericBlobMessage *)pMessage;
            if (tEncoder.SizeBytes() - tEncoder.UsedBytes() < pBlobMessage->uBlobSize)
            {
                bRetVal = FALSE;
            }
            else
            {
                uint8 * pBlobData = (uint8 *)pMessage + sizeof(GenericBlobMessage);
                tEncoder.PutBytesAligned8(pBlobData, pBlobMessage->uBlobSize);
            }
        }
        else if (pTypeDescriptor == DT_CHUNK_MESSAGE)
        {
            ChunkMessage tChunkMessage = {};
            SigmaMemoryCopy(&tChunkMessage, pMessage, sizeof(ChunkMessage));
            if (tEncoder.SizeBytes() - tEncoder.UsedBytes() < tChunkMessage.unk_08)
            {
                bRetVal = FALSE;
            }
            else
            {
                uint8 * pChunkData = (uint8 *)pMessage + sizeof(ChunkMessage);
                tEncoder.PutBytesAligned8(pChunkData, tChunkMessage.unk_08);
            }
        }
        else if (ptHeader->eType == SMSG_ATTRIBUTE_SET_VALUE)
        {
            bRetVal = AttributeCompressor::Encode(tEncoder, ((AttributeSetValueMessage *)pMessage)->tKeyVal);
        }
        else if (ptHeader->eType == SMSG_ATTRIBUTES_SET_VALUES)
        {
            AttributesSetValuesMessage * ptAttributesMessage = (AttributesSetValuesMessage *)pMessage;
            for (int32 i = 0; i < ptAttributesMessage->nAttributeCount; i++)
            {
                bRetVal = AttributeCompressor::Encode(tEncoder, ptAttributesMessage->atKeyVals[i]);
                if (!bRetVal)
                    break;
            }
        }
    }

    if (!bRetVal)
    {
        tEncoder.SetUsed(dwUsedBits);
    }

    return bRetVal;
}

XDecoder::Result DecodeMessage(GameMessageType eMessageType, void * pDecodeToBuffer, uint32 dwBufferSize, XDecoder & tDecoder)
{
    const XTypeDescriptor * pTypeDescriptor = NULL;
    uint32 dwSize = 0;
    const char * pszLabel;

    if (!GetGameMessageTypeAndSize(eMessageType, pTypeDescriptor, dwSize, pszLabel))
    {
        return XDecoder::RESULT_CORRUPTED;
    }

    XDecoder::Result eResult = GenericDecode(tDecoder, pTypeDescriptor, pDecodeToBuffer, dwSize);
    if (eResult != XDecoder::RESULT_VALID)
    {
        return eResult;
    }

    if (pTypeDescriptor == DT_GENERIC_BLOB_MESSAGE)
    {
        GenericBlobMessage * pBlobMessage = (GenericBlobMessage *)pDecodeToBuffer;
        uint32 dwNewSize = pBlobMessage->tHeader.dwSize + pBlobMessage->uBlobSize;
        pBlobMessage->tHeader.dwSize = dwNewSize;
        if (dwNewSize > 0x8000)
        {
            return XDecoder::RESULT_CORRUPTED;
        }

        if (tDecoder.SizeBytes() - tDecoder.UsedBytes() < pBlobMessage->uBlobSize)
        {
            return XDecoder::RESULT_CORRUPTED;
        }

        uint8 * pBlobLoc = (uint8 *)pDecodeToBuffer + sizeof(GenericBlobMessage);
        return tDecoder.GetBytesAligned8(pBlobLoc, pBlobMessage->uBlobSize);
    }
    else if (pTypeDescriptor == DT_CHUNK_MESSAGE)
    {
        ChunkMessage tChunkMessage;
        SigmaMemoryCopy(&tChunkMessage, pDecodeToBuffer, sizeof(ChunkMessage));
        uint32 dwNewSize = tChunkMessage.tHeader.dwSize + tChunkMessage.unk_08;
        tChunkMessage.tHeader.dwSize = dwNewSize;
        SigmaMemoryCopy(pDecodeToBuffer, &tChunkMessage, sizeof(ChunkMessage));
        if (dwNewSize > 0x8000)
        {
            return XDecoder::RESULT_CORRUPTED;
        }

        if (tDecoder.SizeBytes() - tDecoder.UsedBytes() < tChunkMessage.unk_08)
        {
            return XDecoder::RESULT_CORRUPTED;
        }

        uint8 * pChunkLoc = (uint8 *)pDecodeToBuffer + sizeof(ChunkMessage);
        return tDecoder.GetBytesAligned8(pChunkLoc, tChunkMessage.unk_08);
    }
    else if (eMessageType == SMSG_ATTRIBUTE_SET_VALUE)
    {
        return AttributeCompressor::Decode(tDecoder, ((AttributeSetValueMessage *)pDecodeToBuffer)->tKeyVal);
    }
    else if (eMessageType == SMSG_ATTRIBUTES_SET_VALUES)
    {
        AttributesSetValuesMessage * ptAttributesMessage = (AttributesSetValuesMessage *)pDecodeToBuffer;
        for (int32 i = 0; i < ptAttributesMessage->nAttributeCount; i++)
        {
            eResult = AttributeCompressor::Decode(tDecoder, ptAttributesMessage->atKeyVals[i]);
            if (eResult != XDecoder::RESULT_VALID)
            {
                return eResult;
            }
        }
    }

    return XDecoder::RESULT_VALID;
}
