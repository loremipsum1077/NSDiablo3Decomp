// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Networking/GameMessage.h
// Every GameMessageType value and message struct of the original header. The first part was adopted and
// extended by hand; the second part (below the marker) was generated from the DWARF
// (build/dwarf/headers/X/Common/Networking/GameMessage.h). Do not keep local copies of these in .cpp files.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../WorldPlace.h"
#include "../SNO/SNOFiles.h"
#include "../ActorInventory.h"
#include "../Console/Avenger.h"
#include "../GameBalance.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/DataStructures/XOptional.h"
#include "../../Sigma/Math/Transform.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"

class PlayerList;

// GameMessageType values for actor movement (X/Server/Movement/SActorMovement.cpp).
enum {
    SMSG_ACD_TRANSLATE_NORMAL = 123,
    SMSG_ACD_TRANSLATE_SNAPPED = 124,
    SMSG_ACD_TRANSLATE_FACING = 125,
    SMSG_ACD_TRANSLATE_FIXED = 126,
    SMSG_ACD_TRANSLATE_ARC = 127,
    SMSG_ACD_TRANSLATE_DETPATH = 128,
    SMSG_ACD_TRANSLATE_DETPATH_SIN = 129,
    SMSG_ACD_TRANSLATE_DETPATH_SPIRAL = 130,
    SMSG_ACD_TRANSLATE_DETPATH_PIE_WEDGE = 131,
    SMSG_ACD_TRANSLATE_STOPPED = 132,
    SMSG_ACD_TRANSLATE_SYNC = 133,
    SMSG_ACD_TRANSLATE_FIXED_UPDATE = 134,
    CMSG_ACD_TRANSLATE_NORMAL = 135,
    CMSG_ACD_TRANSLATE_FACING = 136,
    CMSG_ACD_TRANSLATE_ACK = 137,
};

// GameMessageType values (original enumerators).
enum {
    SMSG_CONSOLE_SYNC_ACCOUNT_ACHIEVEMENT_CRITERIA_UPDATE = 445,
    SMSG_ACHIEVEMENT_AWARDED = 453,
};

enum {
    SMSG_SET_KEEP_TOC_LOADED = 13,
};

enum {
    SMSG_PLAY_EFFECT = 138,
    SMSG_PLAY_HIT_EFFECT = 139,
    SMSG_PLAY_HIT_EFFECT_OVERRIDE = 140,
    SMSG_EFFECT_GROUP_ACD_TO_ACD = 195,
    SMSG_ROPE_EFFECT_ACD_TO_ACD = 196,
    SMSG_ROPE_EFFECT_ACD_TO_PLACE = 197,
    SMSG_CHANGE_ACTOR = 202,
};

struct ACDChangeActorMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    SNO snoActor;  // 0x0c
};
static_assert(sizeof(ACDChangeActorMessage) == 0x10, "ACDChangeActorMessage");

enum {
    CMSG_TRADE = 185,
    SMSG_MAP_MARKER = 272,
    SMSG_MAP_OBJECTIVE_MARKER = 273,
};

struct MapMarkerInfoMessage {  // size 0x54
    RequiredMessageHeader tHeader;  // 0x00
    uint32 m_nHashedName;  // 0x08
    WorldPlace m_Place;  // 0x0c
    SNO m_ImageInfo;  // 0x1c
    uint32 m_hImage;  // 0x20
    SNO m_snoStringList;  // 0x24
    uint32 m_hLabel;  // 0x28
    BOOL m_bActive;  // 0x2c
    BOOL m_bCanBecomeArrow;  // 0x30
    BOOL m_bRespectsFoW;  // 0x34
    BOOL m_bIsPing;  // 0x38
    float m_fMaxDisplayRangeSq;  // 0x3c
    float m_fMinDisplayRangeSq;  // 0x40
    float m_fDiscoveryRangeSq;  // 0x44
    uint32 m_nPlayerUseFlags;  // 0x48
    SNO m_snoKnownActorOverride;  // 0x4c
    SNO m_snoQuestSource;  // 0x50
};
static_assert(sizeof(MapMarkerInfoMessage) == 0x54, "MapMarkerInfoMessage");

enum {
    TRADE_MSG_TRY_START = 0,
    TRADE_MSG_CANCEL = 1,
    TRADE_MSG_ACCEPT = 2,
    TRADE_MSG_START = 3,
    TRADE_MSG_GOLD = 4,
    TRADE_MSG_COMPLETE = 5,
    TRADE_MSG_ITEM = 6,
    TRADE_MSG_OTHER_BUSY = 7,
    TRADE_MSG_STATE = 8,
    TRADE_MSG_ITEM_REMOVED = 9,
};

typedef int32 eTradeMessageType;

struct TradeMessage {  // size 0x38
    RequiredMessageHeader tHeader;  // 0x00
    eTradeMessageType eMessageType;  // 0x08
    uint32 dwData;  // 0x0c
    uint32 dwData2;  // 0x10
    uint64 uData64;  // 0x18
    ACDNetworkName ann;  // 0x20
    ACDNetworkName arrItems[4];  // 0x24
};
static_assert(sizeof(TradeMessage) == 0x38, "TradeMessage");

enum {
    SMSG_SET_DUNGEON_ENCOUNTER_INVITE_STATE = 307,
    SMSG_SET_DUNGEON_CANCELED = 308,
    SMSG_SET_DUNGEON_ONE_DECLINED = 309,
    SMSG_SET_DUNGEON_JOIN = 310,
    SMSG_SET_DUNGEON_STARTED = 311,
};

// X/Common/Networking/GameMessage.h:443-445 (skeleton values).
enum {
    CMSG_TRY_REPAIR = 348,
    CMSG_TRY_REPAIR_ALL = 349,
    CMSG_TRY_REPAIR_EQUIPPED = 350,
};

struct SimpleMessage {  // size 0x8
    RequiredMessageHeader tHeader;  // 0x00
};
static_assert(sizeof(SimpleMessage) == 0x8, "SimpleMessage");

enum {
    SMSG_ITEM_DELETE_NOTIFY = 106,
};

struct IntDataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    int32 nData;  // 0x08
};
static_assert(sizeof(IntDataMessage) == 0xc, "IntDataMessage");

struct ANNDataMessage {  // size 0xc  (skeleton GameMessage.h:802)
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
};
static_assert(sizeof(ANNDataMessage) == 0xc, "ANNDataMessage");

struct EncounterInviteStateMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    uint8 m_AcceptedPlayers;  // 0x08
    uint8 m_PendingPlayers;  // 0x09
    uint8 m_DeclinedPlayers;  // 0x0a
    void Clear();
};
static_assert(sizeof(EncounterInviteStateMessage) == 0xc, "EncounterInviteStateMessage");

// The body is not in the DWARF; written from the copy inlined into
// X/Server/System/SetEncounter.cpp (SendEncounterInviteStateMessage).
inline void EncounterInviteStateMessage::Clear() {  // line 2169
    m_DeclinedPlayers = 0;
    m_AcceptedPlayers = 0;
    m_PendingPlayers = 0;
}

struct SetDungeonDialogMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    int32 nLabelDescription;  // 0x0c
    int32 nLabelTitle;  // 0x10
};
static_assert(sizeof(SetDungeonDialogMessage) == 0x14, "SetDungeonDialogMessage");

void MessageSendToClient(const PlayerList & listDestinations, GameMessageType eType, void * pMessage,
                         uint32 dwMessageSize);  // GameMessage.cpp:1468

namespace google { namespace protobuf { class MessageLite; } }
void MessageSendToClient(const PlayerList & listDestinations, GameMessageType eType,
                         const google::protobuf::MessageLite & tMessage);  // GameMessage.cpp:1546

// lines 669-671
enum {
    SMSG_CHALLENGE_RIFT_REWARD_CONSOLE = 574,
    SMSG_CHALLENGE_RIFT_CLAIM_REWARD_CONSOLE = 576,
};

struct DWordDataMessage {  // size 0xc (lines 723-727)
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwData;  // 0x08
};
static_assert(sizeof(DWordDataMessage) == 0xc, "DWordDataMessage");

// GameMessageType values for X/Server/Network/Server.cpp.
enum {
    SMSG_HEARTBEAT = 158,
};

enum {
    CMSG_START_VOTE_KICK = 407,
    CMSG_TRY_VOTE_KICK = 408,
    SMSG_CAN_VOTE_KICK = 409,
    SMSG_VOTE_KICK_VOTE = 410,
    CMSG_VOTE_KICK_VOTE = 411,
    SMSG_VOTE_KICK_FAILED = 412,
    SMSG_VOTE_KICK_PASSED = 413,
};

enum {
    SMSG_ACT_TRANSITION_STARTED = 262,
    SMSG_ACT_TRANSITION_ENDED = 263,
    SMSG_START_END_COUNTDOWN = 406,
};

struct PlayerIndexMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
};
static_assert(sizeof(PlayerIndexMessage) == 0xc, "PlayerIndexMessage");

struct VoteKickMessage {  // size 0x410
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndexToKick;  // 0x08
    int32 nPlayerIndexInitiating;  // 0x0c
    CHAR usMessage[1024];  // 0x10
};
static_assert(sizeof(VoteKickMessage) == 0x410, "VoteKickMessage");

enum {
    SMSG_TIMED_EVENT_STARTED = 258,
    SMSG_TIMED_EVENT_MODIFIED = 259,
    SMSG_TIMED_EVENT_FINISHED = 260,
    SMSG_TIMED_EVENT_RESET = 261,
};

struct SNODataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoData;  // 0x08
};
static_assert(sizeof(SNODataMessage) == 0xc, "SNODataMessage");
static_assert(__builtin_offsetof(SNODataMessage, snoData) == 0x8, "SNODataMessage::snoData");

// X/Server/SCosmeticItems.cpp
enum {
    SMSG_UPDATED_COSMETIC_ITEM_DATA = 540,
};

// Added for X/Server/Actors/Actors.cpp.
enum {
    SMSG_PLAYER_ACTOR_SET = 55,
};

// Added for X/Common/ActorCommonData.cpp.
enum {
    SMSG_PLAYER_GIFT_REGISTER_RECIPIENT = 524,
};

struct PlayerActorSetInitialMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    int32 nPlayerIndex;  // 0x0c
};
static_assert(sizeof(PlayerActorSetInitialMessage) == 0x10, "PlayerActorSetInitialMessage");
struct DungeonFinderCompletionTimeMessage {  // size 0x10 (GameMessage.h:2048 in the reference)
    RequiredMessageHeader tHeader;  // 0x00
    int32 nCompletionTimeMessage;  // 0x08
    int32 nPlayerIndex;  // 0x0c
};
static_assert(sizeof(DungeonFinderCompletionTimeMessage) == 0x10, "DungeonFinderCompletionTimeMessage");
enum {
    SMSG_DUNGEON_FINDER_COMPLETION_TIME = 578,  // GameMessage.h:674 in the reference
};
// GameMessageType values used by X/Server/SGame.cpp (original enumerators).
enum {
    SMSG_GAME_SYNCED_DATA = 201,
    SMSG_DUNGEON_FINDER_SEED = 482,
    SMSG_DUNGEON_FINDER_PROGRESS = 483,
    SMSG_DUNGEON_FINDER_SET_TIMEDEVENT = 485,
    SMSG_DUNGEON_FINDER_PARTICIPATING_PLAYER_COUNT = 486,
};

// Message types and structs sent by X/Common/ActorCommonData.cpp.
enum {
    SMSG_ACD_INVENTORY_POSITION = 68,
    SMSG_ACD_INVENTORY_UPDATE_ACTOR_SNO = 69,
    SMSG_AFFIX = 76,
    SMSG_ATTRIBUTES_SET_VALUES = 81,
    SMSG_VISUAL_INVENTORY = 82,
    SMSG_PROJECTILE_STICK = 83,
    SMSG_RARE_MONSTER_NAMES = 77,
    SMSG_RARE_ITEM_NAME = 78,
    SMSG_PORTAL_SPECIFIER = 79,
    SMSG_ACD_COLL_FLAGS = 191,
    SMSG_CURRENCY_MODIFIED = 192,
    SMSG_SHEAR = 215,
    SMSG_GROUP = 216,
};

struct ACDInventoryUpdateActorSNO {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    SNO snoActor;  // 0x0c
};
static_assert(sizeof(ACDInventoryUpdateActorSNO) == 0x10, "ACDInventoryUpdateActorSNO");

struct ACDShearMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    float flShear;  // 0x0c
};
static_assert(sizeof(ACDShearMessage) == 0x10, "ACDShearMessage");

struct ACDGroupMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    uint32 uPrimaryGroup;  // 0x0c
    uint32 uSecondaryGroup;  // 0x10
};
static_assert(sizeof(ACDGroupMessage) == 0x14, "ACDGroupMessage");

typedef int32 InventorySlotType;  // X/Common/ActorInventory.h
typedef int32 ActorLocationType;  // X/Common/ActorCommonData.h
typedef int32 AffixMessageFlag;  // X/Common/ActorCommonData.h

struct InventoryLocationMessageData {  // size 0x10
    ACDNetworkName annContainer;  // 0x00
    InventorySlotType eSlot;  // 0x04
    IVector2D pt;  // 0x08
};
static_assert(sizeof(InventoryLocationMessageData) == 0x10, "InventoryLocationMessageData");

struct ACDInventoryPositionMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    InventoryLocationMessageData tInvLocationData;  // 0x0c
    ActorLocationType tLocType;  // 0x1c
};
static_assert(sizeof(ACDInventoryPositionMessage) == 0x20, "ACDInventoryPositionMessage");

typedef int32 GBID;  // X/Common/GameBalance.h

struct AffixMessage {  // size 0x94
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    AffixMessageFlag eAffixArray;  // 0x0c
    GBID aAffixGBIDs[32];  // 0x10
    uint32 dwAffixArrayCount;  // 0x90
};
static_assert(sizeof(AffixMessage) == 0x94, "AffixMessage");

// X/Common/Player.cpp (PlayerDisplayErrorMessage)
enum {
    SMSG_DISPLAY_ERROR = 232,
};

// GameMessageType values for the crafter data messages (X/Common/Crafting/ItemCrafting.cpp).
enum {
    SMSG_CRAFTING_DATA_BLACKSMITH = 275,
    SMSG_CRAFTING_DATA_JEWELER = 276,
    SMSG_CRAFTING_DATA_MYSTIC = 277,
    SMSG_CRAFTING_DATA_HORADRIM = 278,
    SMSG_CRAFTING_DATA_TEST = 279,
    SMSG_CRAFTING_DATA_TRANSMOG = 280,
    SMSG_CRAFTING_DATA_BLACKSMITH_INITIAL = 281,
    SMSG_CRAFTING_DATA_JEWELER_INITIAL = 282,
    SMSG_CRAFTING_DATA_MYSTIC_INITIAL = 283,
    SMSG_CRAFTING_DATA_HORADRIM_INITIAL = 284,
    SMSG_CRAFTING_DATA_TEST_INITIAL = 285,
};

// More of GameMessage.h, for X/Server/SPlayer.cpp.
enum {
    SMSG_PLAYER_KICK_TIMERS = 47,
    SMSG_PLAYER_BANNER = 48,
    SMSG_HERO_STATE = 56,
    SMSG_HERO_STATE_SAVEDDATA = 57,
    SMSG_HERO_STATE_PLAYERFLAGS = 58,
    SMSG_HERO_STATE_WAYPOINTS = 59,
    SMSG_HERO_STATE_LASTSAVEPOINT = 60,
    SMSG_HERO_STATE_LASTWAYPOINTLEVELAREA = 61,
    SMSG_HERO_STATE_SKILLS = 62,
    SMSG_INSPECT = 189,
    SMSG_CURRENCY_DATA_FULL = 287,
    SMSG_CURRENCY_DATA_PARTIAL = 288,
    SMSG_EXTRACTED_LEGENDARIES_DATA_FULL = 289,
    SMSG_LOAD_LEGENDARY_POWER_ICONS = 291,
    SMSG_HANDLE_EVENT_SKIP = 386,
};

struct PlayerDWordDataMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwData;  // 0x08
    int32 nPlayerIndex;  // 0x0c
};
static_assert(sizeof(PlayerDWordDataMessage) == 0x10, "PlayerDWordDataMessage");

struct PlayerDWord2DataMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    uint32 unk_08;  // 0x08
    uint32 unk_0c;  // 0x0c
    int32 unk_10;  // 0x10
};
static_assert(sizeof(PlayerDWord2DataMessage) == 0x14, "PlayerDWord2DataMessage");

// X/Server/SWorld.cpp (SWorld::SendStatus)
enum {
    SMSG_WORLD_STATUS = 213,
    SMSG_WORLD_SYNCED_DATA = 481,
};

struct WorldStatusMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SWorldID idSWorld;  // 0x08
    uint32 dwWorldStatusBits;  // 0x0c
};
static_assert(sizeof(WorldStatusMessage) == 0x10, "WorldStatusMessage");

// GameMessageType values X/Client/CPlayer.cpp sends.
enum {
    CMSG_JOIN_CONSOLE_GAME = 18,
    CMSG_START_LOGOUT_TIMER = 27,
    CMSG_ACCEPTED_DEATH = 33,
    CMSG_FINISHED_GAME_SETUP = 44,
    CMSG_INSPECT_START = 187,
    CMSG_SEND_PLAYER_PREFS = 389,
    CMSG_SET_FRIEND_STATUS = 522,
};

struct JoinConsoleGameMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    XLocale eXLocale;  // 0x08
    uint8 uControllerIndex;  // 0x0c
    uint32 uLicenses;  // 0x10
};
static_assert(sizeof(JoinConsoleGameMessage) == 0x14, "JoinConsoleGameMessage");

enum {
    SMSG_PORTED_TO_WAYPOINT = 211,
};

struct PortedToWaypointMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    SNO snoLevelArea;  // 0x0c
};
static_assert(sizeof(PortedToWaypointMessage) == 0x10, "PortedToWaypointMessage");

// line 170
enum {
    SMSG_SAVEPOINT_INFO = 73,
};

struct SavePointInfoMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoLevelArea;  // 0x08
};
static_assert(sizeof(SavePointInfoMessage) == 0xc, "SavePointInfoMessage");

// line 171
enum {
    SMSG_HEARTHPORTAL_INFO = 74,
};

struct HearthPortalInfoMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoLevelArea;  // 0x08
    SNO unk_c;  // 0x0c (2.7.7: DT_SNO filtered to WORLD_SNO; GizmoSendHearthPortalToPlayer stores the return place's world SNO)
    ACDNetworkName annPortal;  // 0x10
    BOOL bResetUI;  // 0x14
    BOOL bIsTieredRift;  // 0x18
};
static_assert(sizeof(HearthPortalInfoMessage) == 0x1c, "HearthPortalInfoMessage");

struct RiftJoinMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    uint32 nPlayerIndex;  // 0x08
    int32 nRiftTier;  // 0x0c
    uint32 uRiftStartServerTime;  // 0x10
    uint32 nPlayerKeyTiers;  // 0x14
};
static_assert(sizeof(RiftJoinMessage) == 0x18, "RiftJoinMessage");

struct RiftStartAcceptedMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 unk_08;  // 0x08
    BOOL unk_0c;  // 0x0c
};
static_assert(sizeof(RiftStartAcceptedMessage) == 0x10, "RiftStartAcceptedMessage");

// X/Common/Networking/GameMessage.h:413-431 (rift encounter messages).
enum {
    SMSG_RIFT_START_ENCOUNTER = 316,
    CMSG_RIFT_START_ACCEPTED = 317,
    CMSG_RIFT_START_DECLINED = 318,
    SMSG_RIFT_INVITE_STATE = 319,
    SMSG_RIFT_JOIN_ENCOUNTER = 320,
    CMSG_RIFT_JOIN_DECLINED = 321,
    CMSG_RIFT_JOIN_ACCEPTED = 322,
    CMSG_RIFT_JOIN_INFO_REQUEST = 323,
    SMSG_RIFT_STARTED = 324,
    SMSG_RIFT_CANCELED = 325,
    SMSG_RIFT_ONE_DECLINED = 326,
    CMSG_RIFT_VOTE_CONTINUE = 327,
    CMSG_RIFT_VOTE_CANCEL = 328,
    SMSG_RIFT_PAYMENT_WARNING = 329,
    CMSG_RIFT_PAYMENT_ACCEPTED = 330,
};

enum {
    SMSG_OPEN_WORLD_TRACKED_SNO = 494,
};

enum {
    SMSG_RETURNPOINT_INFO = 75,
};

struct ReturnPointInfoMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoLevelArea;  // 0x08
};
static_assert(sizeof(ReturnPointInfoMessage) == 0xc, "ReturnPointInfoMessage");

enum {
    SMSG_ACTIVATE_WAYPOINT = 150,
};

struct WaypointActivatedMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annWaypoint;  // 0x08
    ACDNetworkName annActivatingPlayer;  // 0x0c
    SNO snoLevelArea;  // 0x10
    BOOL bAnnounce;  // 0x14
};
static_assert(sizeof(WaypointActivatedMessage) == 0x18, "WaypointActivatedMessage");

// GameMessageType values for quests (X/Server/Quests/SQuests.cpp).
enum {
    SMSG_QUEST_UPDATE = 168,
    SMSG_QUEST_METER = 169,
    SMSG_QUEST_FLAGS = 170,
    SMSG_QUEST_COUNTER = 171,
    SMSG_QUEST_REWARD = 172,
    SMSG_TIERED_RIFT_REWARD = 173,
    SMSG_DYNAMIC_QUEST_OBJECTIVE_UPDATE = 476,
    SMSG_DYNAMIC_QUEST_RESET = 480,
};

// GameMessageType values for animation (X/Common/AnimPlayer.cpp).
enum {
    SMSG_PLAY_ANIMATION = 121,
    SMSG_ACD_SET_IDLE_ANIM = 190,
};

enum {
    SMSG_NPC_INTERACT = 163,
    SMSG_OPEN_COSMETICS = 166,
    SMSG_OPEN_WORLD_MODE_CHANGE_PROMPT = 489,
    SMSG_EQUIPMENT_MANAGER_OPEN = 565,
};

struct OpenWorldModeChangeMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    int32 eOpenWorldMode;  // 0x08 (OpenWorldMode)
    int32 eAct;  // 0x0c (Act)
    int32 nParam;  // 0x10
};
static_assert(sizeof(OpenWorldModeChangeMessage) == 0x14, "OpenWorldModeChangeMessage");

struct SalvageResultsMessage {  // size 0x34
    RequiredMessageHeader tHeader;  // 0x00
    GBID gbidOriginalItem;  // 0x08
    int32 eIQLOriginalItem;  // 0x0c (ItemQualityLevel)
    GBID gbidMaterials[4];  // 0x10
    int32 nMaterialsCounts[4];  // 0x20
    int32 nMaterialsResults;  // 0x30
};
static_assert(sizeof(SalvageResultsMessage) == 0x34, "SalvageResultsMessage");

// ---- The rest of the original GameMessage.h, generated from the DWARF (was GameMessageFull.h) ----

typedef int32 Act;
typedef int32 ActorLocationType;
typedef int32 ActorTag;
typedef int32 AffixMessageFlag;
typedef int32 CalloutEventType;
typedef int32 ComplexEffectType;
typedef int32 CurrencyType;
typedef int32 DeSyncReason;
typedef int32 DebugDrawPrimType;
typedef int32 EChatColor;
typedef int32 FloatingNumberType;
typedef int32 GameMessageType;
typedef int32 InventorySlotType;
typedef int32 ItemQualityLevel;
typedef int32 KillCounterType;
typedef int32 LogoutContext;
typedef int32 MonsterRarity;
typedef int32 OpenWorldMode;
typedef int32 PickupResponse;
typedef int32 PlayerClass;
typedef int32 PlayerWarpReason;
typedef int32 QuitReason;
typedef int32 SComplexEffectID;
typedef int32 SalvageAllType;
typedef int32 ServerShutdownType;
typedef int32 TargetType;
typedef int32 Team;
typedef int32 TeamColor;
typedef int32 TrickleInfoType;
typedef int32 TryChatDestination;
typedef int32 UIElement;
typedef int32 UIElementAction;
typedef int32 WeaponClass;
typedef int32 eTradeMessageType;
typedef uint64 AchievementId;

class XTypeDescriptor;
// Type descriptors of the message structs (defined in GameMessage.cpp).
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;
extern const XTypeDescriptor * DT_SIMPLE_MESSAGE;
extern const XTypeDescriptor * DT_GENERIC_BLOB_MESSAGE;
extern const XTypeDescriptor * DT_CHUNK_MESSAGE;
extern const XTypeDescriptor * DT_BOOL_DATA_MESSAGE;
extern const XTypeDescriptor * DT_BYTE_DATA_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_INDEX_MESSAGE;
extern const XTypeDescriptor * DT_DATAID_DATA_MESSAGE;
extern const XTypeDescriptor * DT_DWORD_DATA_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_DWORD_DATA_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_DWORD2_DATA_MESSAGE;
extern const XTypeDescriptor * DT_INT_DATA_MESSAGE;
extern const XTypeDescriptor * DT_INT64_DATA_MESSAGE;
extern const XTypeDescriptor * DT_UINT64_DATA_MESSAGE;
extern const XTypeDescriptor * DT_FLOAT_DATA_MESSAGE;
extern const XTypeDescriptor * DT_SNO_DATA_MESSAGE;
extern const XTypeDescriptor * DT_SNONAME_DATA_MESSAGE;
extern const XTypeDescriptor * DT_PRELOAD_ACD_DATA_MESSAGE;
extern const XTypeDescriptor * DT_PREFETCH_ACTOR_DATA_MESSAGE;
extern const XTypeDescriptor * DT_GBID_DATA_MESSAGE;
extern const XTypeDescriptor * DT_DISPLAY_GAME_TEXT_MESSAGE;
extern const XTypeDescriptor * DT_BROADCAST_TEXT_MESSAGE;
extern const XTypeDescriptor * DT_ANN_DATA_MESSAGE;
extern const XTypeDescriptor * DT_JOIN_LAN_GAME_MESSAGE;
extern const XTypeDescriptor * DT_JOIN_CONSOLE_GAME_MESSAGE;
extern const XTypeDescriptor * DT_QUIT_GAME_MESSAGE;
extern const XTypeDescriptor * DT_CONNECTION_ESTABLISHED_MESSAGE;
extern const XTypeDescriptor * DT_GAME_SETUP_MESSAGE;
extern const XTypeDescriptor * DT_ENTER_WORLD_MESSAGE;
extern const XTypeDescriptor * DT_REVEAL_WORLD_MESSAGE;
extern const XTypeDescriptor * DT_WORLD_LOCATION_MESSAGE_DATA;
extern const XTypeDescriptor * DT_INVENTORY_LOCATION_MESSAGE_DATA;
extern const XTypeDescriptor * DT_ACD_ENTER_KNOWN_MESSAGE;
extern const XTypeDescriptor * DT_ENTER_KNOWN_LOOK_OVERRIDES;
extern const XTypeDescriptor * DT_PLAYER_ENTER_KNOWN_MESSAGE;
extern const XTypeDescriptor * DT_ACD_INVENTORY_POSITION_MESSAGE;
extern const XTypeDescriptor * DT_ACD_INVENTORY_UPDATE_ACTOR_SNO;
extern const XTypeDescriptor * DT_ACD_WORLD_POSITION_MESSAGE;
extern const XTypeDescriptor * DT_ACD_SHEAR_MESSAGE;
extern const XTypeDescriptor * DT_ACD_GROUP_MESSAGE;
extern const XTypeDescriptor * DT_ACD_CHANGE_ACTOR_MESSAGE;
extern const XTypeDescriptor * DT_ACD_PICKUP_FAILED_MESSAGE;
extern const XTypeDescriptor * DT_AFFIX_MESSAGE;
extern const XTypeDescriptor * DT_PROJECTILE_STICK_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_ACTOR_SET_INITIAL_MESSAGE;
extern const XTypeDescriptor * DT_ANIM_PREPLAY_DATA;
extern const XTypeDescriptor * DT_TARGET_MESSAGE;
extern const XTypeDescriptor * DT_SECONDARY_ANIMATION_POWER_MESSAGE;
extern const XTypeDescriptor * DT_LOOPING_ANIMATION_POWER_MESSAGE;
extern const XTypeDescriptor * DT_TRY_CONSOLE_COMMAND;
extern const XTypeDescriptor * DT_TRY_CHAT_MESSAGE;
extern const XTypeDescriptor * DT_CHAT_MESSAGE;
extern const XTypeDescriptor * DT_VOTE_KICK_MESSAGE;
extern const XTypeDescriptor * DT_VICTIM_MESSAGE;
extern const XTypeDescriptor * DT_LOOTRUN_APPEARED_MESSAGE;
extern const XTypeDescriptor * DT_COMBAT_ENGAGEMENT_MESSAGE;
extern const XTypeDescriptor * DT_DUEL_RESULT_MESSAGE;
extern const XTypeDescriptor * DT_KILL_COUNT_MESSAGE;
extern const XTypeDescriptor * DT_INV_LOC;
extern const XTypeDescriptor * DT_INVENTORY_REQUEST_QUICK_MOVE_MESSAGE;
extern const XTypeDescriptor * DT_INVENTORY_REQUEST_MOVE_MESSAGE;
extern const XTypeDescriptor * DT_CHANGE_USABLE_ITEM_MESSAGE;
extern const XTypeDescriptor * DT_STACK_PORTION_MESSAGE;
extern const XTypeDescriptor * DT_CURRENCY_MESSAGE;
extern const XTypeDescriptor * DT_INVENTORY_SPLIT_STACK_MESSAGE;
extern const XTypeDescriptor * DT_INVENTORY_STACK_TRANSFER_MESSAGE;
extern const XTypeDescriptor * DT_GOLD_TRANSFER_MESSAGE;
extern const XTypeDescriptor * DT_INVENTORY_REQUEST_USE_MESSAGE;
extern const XTypeDescriptor * DT_INVENTORY_REQUEST_SOCKET_MESSAGE;
extern const XTypeDescriptor * DT_HELPER_DETACH_MESSAGE;
extern const XTypeDescriptor * DT_PET_MESSAGE;
extern const XTypeDescriptor * DT_PET_DETACH_MESSAGE;
extern const XTypeDescriptor * DT_FLIPPY_MESSAGE;
extern const XTypeDescriptor * DT_COMPLEX_EFFECT_ADD_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_LEVEL;
extern const XTypeDescriptor * DT_AIM_TARGET_MESSAGE;
extern const XTypeDescriptor * DT_ACD_CHANGE_GBHANDLE_MESSAGE;
extern const XTypeDescriptor * DT_TRICKLE_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_INTVAL_MESSAGE;
extern const XTypeDescriptor * DT_UI_ELEMENT_MESSAGE;
extern const XTypeDescriptor * DT_PLASMA_ATTACH_MESSAGE;
extern const XTypeDescriptor * DT_RITUAL_TETHER_EFFECT_MESSAGE;
extern const XTypeDescriptor * DT_REVEAL_TEAM_MESSAGE;
extern const XTypeDescriptor * DT_DEATH_FADE_TIME_MESSAGE;
extern const XTypeDescriptor * DT_MAP_REVEAL_SCENE_MESSAGE;
extern const XTypeDescriptor * DT_SAVE_POINT_INFO_MESSAGE;
extern const XTypeDescriptor * DT_HEARTHPORTAL_INFO_MESSAGE;
extern const XTypeDescriptor * DT_RETURN_POINT_INFO_MESSAGE;
extern const XTypeDescriptor * DT_ACD_LOOK_AT_MESSAGE;
extern const XTypeDescriptor * DT_KILL_COUNTER_UPDATE_MESSAGE;
extern const XTypeDescriptor * DT_CURRENCY_COUNTER_UPDATE_MESSAGE;
extern const XTypeDescriptor * DT_LOW_HEALTH_COMBAT_MESSAGE;
extern const XTypeDescriptor * DT_SAVIOR_MESSAGE;
extern const XTypeDescriptor * DT_FLOATING_NUMBER_MESSAGE;
extern const XTypeDescriptor * DT_FLOATING_GBID_AMOUNT_MESSAGE;
extern const XTypeDescriptor * DT_FLOATING_AMOUNT_MESSAGE;
extern const XTypeDescriptor * DT_CALLOUT_MESSAGE;
extern const XTypeDescriptor * DT_REMOVE_RAGDOLL_MESSAGE;
extern const XTypeDescriptor * DT_WORLD_STATUS_MESSAGE;
extern const XTypeDescriptor * DT_WORLD_DELETED_MESSAGE;
extern const XTypeDescriptor * DT_BLIZZCON_CVARS_MESSAGE;
extern const XTypeDescriptor * DT_WAYPOINT_ACTIVATED_MESSAGE;
extern const XTypeDescriptor * DT_TRY_WAYPOINT_MESSAGE;
extern const XTypeDescriptor * DT_LORE_MESSAGE;
extern const XTypeDescriptor * DT_PRTRANSFORM_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_QUEST_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_DESYNC_SNAP_MESSAGE;
extern const XTypeDescriptor * DT_TRADE_MESSAGE;
extern const XTypeDescriptor * DT_PVP_TOKEN_REDEMPTION_MESSAGE;
extern const XTypeDescriptor * DT_MAP_MARKER_MESSAGE;
extern const XTypeDescriptor * DT_BOSS_ENCOUNTER_MESSAGE;
extern const XTypeDescriptor * DT_SET_DUNGEON_MESSAGE;
extern const XTypeDescriptor * DT_BOSS_ENCOUNTER_INVITE_MESSAGE;
extern const XTypeDescriptor * DT_RIFT_START_ACCEPTED_MESSAGE;
extern const XTypeDescriptor * DT_RIFT_JOIN_MESSAGE;
extern const XTypeDescriptor * DT_ACT_TRANSITION_MESSAGE;
extern const XTypeDescriptor * DT_INTERSTITIAL_MESSAGE;
extern const XTypeDescriptor * DT_CAMERA_FOCUS_MESSAGE;
extern const XTypeDescriptor * DT_DEBUG_DRAW_PRIM_MESSAGE;
extern const XTypeDescriptor * DT_DEBUG_DRAW_MOVEMENT_TYPE_MESSAGE;
extern const XTypeDescriptor * DT_CAMERA_ZOOM_MESSAGE;
extern const XTypeDescriptor * DT_CAMERA_YAW_MESSAGE;
extern const XTypeDescriptor * DT_BOSS_ZOOM_MESSAGE;
extern const XTypeDescriptor * DT_PLAY_CUTSCENE_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_WARPED_MESSAGE;
extern const XTypeDescriptor * DT_DISPLAY_ACTOR_DEBUG_TOOLTIP;
extern const XTypeDescriptor * DT_SALVAGE_RESULTS_MESSAGE;
extern const XTypeDescriptor * DT_TRY_SALVAGE_ALL_MESSAGE;
extern const XTypeDescriptor * DT_GAME_TESTING_WORLD_SAMPLING_START_MESSAGE;
extern const XTypeDescriptor * DT_GAME_TESTING_SKILL_SAMPLING_START_MESSAGE;
extern const XTypeDescriptor * DT_REQUEST_BUFF_CANCEL_MESSAGE;
extern const XTypeDescriptor * DT_PLAY_ERROR_SOUND_MESSAGE;
extern const XTypeDescriptor * DT_HIRELING_REQUEST_LEARN_SKILL_MESSAGE;
extern const XTypeDescriptor * DT_LOGOUT_TICKTIME_MESSAGE;
extern const XTypeDescriptor * DT_VERSIONS_MESSAGE;
extern const XTypeDescriptor * DT_HANDICAP_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_SET_CAMERA_OBSERVER_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_SET_CAMERA_ORBIT_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_SET_CAMERA_DEFAULTS_MESSAGE;
extern const XTypeDescriptor * DT_OPEN_WORLD_MODE_CHANGE_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_CONSOLE_SET_CAMERA_DEFAULTS_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_CLEAR_CLIENT_WALK_POWER_MESSAGE;
extern const XTypeDescriptor * DT_ACTOR_LOOK_OVERRIDE_CHANGED;
extern const XTypeDescriptor * DT_PORTED_TO_PLAYER_MESSAGE;
extern const XTypeDescriptor * DT_PORTED_TO_WAYPOINT_MESSAGE;
extern const XTypeDescriptor * DT_JUNK_FLAG_MESSAGE;
extern const XTypeDescriptor * DT_ACHIEVEMENT_PROGRESS_XBOXONE_MESSAGE;
extern const XTypeDescriptor * DT_CONSOLE_COUNTER_INCREMENT_MESSAGE;
extern const XTypeDescriptor * DT_SERVER_NOTIFICATION_DATA_MESSAGE;
extern const XTypeDescriptor * DT_JEWEL_UPGRADE_RESULTS_MESSAGE;
extern const XTypeDescriptor * DT_DUNGEON_FINDER_CLOSING_MESSAGE;
extern const XTypeDescriptor * DT_UBER_BOSSS_CLOSING_MESSAGE;
extern const XTypeDescriptor * DT_WARNING_COUNTDOWN_NOTIFICATION_MESSAGE;
extern const XTypeDescriptor * DT_BLIZZCON_END_SCREEN_MESSAGE;
extern const XTypeDescriptor * DT_HORADRIC_QUEST_CURSED_REALM_RESULTS_MESSAGE;
extern const XTypeDescriptor * DT_SET_DUNGEON_DIALOG_MESSAGE;
extern const XTypeDescriptor * DT_FIRST_OF_THE_DAY_MESSAGE;
extern const XTypeDescriptor * DT_PLATINUM_AWARDED_MESSAGE;
extern const XTypeDescriptor * DT_PLATINUM_ACHIEVEMENT_AWARDED_MESSAGE;
extern const XTypeDescriptor * DT_SET_DUNGEON_RESULTS_MESSAGE;
extern const XTypeDescriptor * DT_SET_DUNGEON_FINDER_TIME_MESSAGE;
extern const XTypeDescriptor * DT_AVENGER_INFO_MESSAGE;
extern const XTypeDescriptor * DT_AVENGER_UPDATE_SOURCE_MESSAGE;
extern const XTypeDescriptor * DT_PLAYER_GIFT_MESSAGE;

namespace OnlineService { typedef uint64 AchievementId; }  // X/Common/OnlineService/Achievements.h

// X/Common/ActorInventory.h (src keeps these two in X/Client/CActorInventoryDecl.inc)
struct InventoryRequestQuickMoveMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    ACDNetworkName annOwner;  // 0x0c
    InventorySlotType eSlot;  // 0x10
    int32 nSlotMinY;  // 0x14
    int32 nSlotMaxY;  // 0x18
};
static_assert(sizeof(InventoryRequestQuickMoveMessage) == 0x1c, "InventoryRequestQuickMoveMessage");

struct InventorySplitStackMessage {  // size 0x28
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    uint64 uAmount;  // 0x10
    InvLoc tLocation;  // 0x18
};
static_assert(sizeof(InventorySplitStackMessage) == 0x28, "InventorySplitStackMessage");

// X/Common/Console/Avenger.h (src keeps it local to Avenger.cpp)
struct AvengerInfoMessage {  // size 0x4c
    RequiredMessageHeader tHeader;  // 0x00
    CHAR szName[32];  // 0x08
    CHAR szVictims[16];  // 0x28
    ACDNetworkName annTarget;  // 0x38
    AvengerSource eAvengerSource;  // 0x3c
    int32 nPlayerIndexOfAvengerTarget;  // 0x40
    int32 nAffixBucket;  // 0x44
    BOOL bRetired;  // 0x48
};
static_assert(sizeof(AvengerInfoMessage) == 0x4c, "AvengerInfoMessage");


// GameMessageType values.
enum {
    MSG_RANGE_ANYTIME_START = 0,
    MSG_INVALID = 0,
    CMSG_TRY_CONSOLE_COMMAND = 1,
    SMSG_TRY_CONSOLE_COMMAND = 2,
    SMSG_QUIT = 3,
    SMSG_PRELOAD_ADD_SCENE = 4,
    SMSG_PRELOAD_ADD_ACD = 5,
    SMSG_PRELOAD_REMOVE_SCENE = 6,
    SMSG_PRELOAD_REMOVE_ACD = 7,
    SMSG_PREFETCH_WORLD = 8,
    SMSG_PREFETCH_SCENE = 9,
    SMSG_PREFETCH_LEVELAREA = 10,
    SMSG_PREFETCH_ACTOR = 11,
    SMSG_PLAYER_WARPING = 12,
    MSG_RANGE_CONSOLE_START = 12,
    MSG_RANGE_CONSOLE_END = 14,
    MSG_RANGE_PREGAME_START = 14,
    CMSG_REQUEST_LAN_GAME_PARAMS = 14,
    SMSG_LAN_GAME_PARAMS = 15,
    CMSG_JOIN_BNET_GAME = 16,
    CMSG_JOIN_LAN_GAME = 17,
    CMSG_LEAVE_CONSOLE_GAME = 19,
    SMSG_VERSIONS = 20,
    MSG_RANGE_INGAME_START = 21,
    SMSG_CRAFTER_OPEN = 21,
    CMSG_VENDOR_CLOSE = 22,
    SMSG_SHOP_OPEN = 23,
    CMSG_SHOP_TRY_BUY = 24,
    CMSG_SHOP_TRY_SELL = 25,
    CMSG_SHOP_TRY_SELL_STACK_PORTION = 26,
    SMSG_LOGOUT_TIMER_STARTED = 28,
    CMSG_LOGOUT_NOW = 29,
    CMSG_CANCEL_LOGOUT = 30,
    SMSG_PLAYER_DELETE = 31,
    SMSG_PLAYER_HAS_DIED = 32,
    SMSG_ACCEPTED_DEATH_ACK = 34,
    SMSG_TUTORIAL = 35,
    SMSG_TUTORIAL_STATE = 36,
    SMSG_STASH_ICON_STATE = 37,
    CMSG_STASH_ICON_STATE = 38,
    SMSG_COMBAT_LOG_ENTRY = 39,
    CMSG_COMBAT_LOG_TOGGLE = 40,
    SMSG_CONNECTION_ESTABLISHED = 41,
    SMSG_CONNECTION_ADD_PLAYER = 42,
    SMSG_GAME_SETUP = 43,
    SMSG_FINISHED_GAME_SETUP_DONE = 45,
    SMSG_NEW_PLAYER = 46,
    SMSG_ENTER_WORLD = 49,
    SMSG_REVEAL_SCENE = 50,
    SMSG_DESTROY_SCENE = 51,
    SMSG_SWAP_SCENE = 52,
    SMSG_REVEAL_WORLD = 53,
    SMSG_REVEAL_TEAM = 54,
    SMSG_ACD_ENTER_KNOWN = 63,
    SMSG_ACD_EXIT_KNOWN = 64,
    SMSG_PLAYER_ENTER_KNOWN = 65,
    SMSG_UI_ENTER_KNOWN = 66,
    SMSG_ACD_WORLD_POSITION = 67,
    SMSG_TRICKLE_UPDATE = 70,
    SMSG_TRICKLE_REMOVE = 71,
    SMSG_MAP_REVEAL_SCENE = 72,
    SMSG_ATTRIBUTE_SET_VALUE = 80,
    CMSG_TARGET = 84,
    CMSG_SECONDARY_ANIMATION_POWER = 85,
    CMSG_LOOPING_ANIMATION_POWER = 86,
    CMSG_EXECUTE_NON_PRIMARY_POWER = 87,
    CMSG_CHANNELLED_POWER_ABORTED = 88,
    CMSG_LOOPING_POWER_ABORTED = 89,
    CMSG_CLIENT_WALK_POWER_ABORTED = 90,
    CMSG_TRY_CHAT = 91,
    SMSG_CHAT = 92,
    CMSG_TRY_DROP = 93,
    CMSG_TRY_INVENTORY_MOVE = 94,
    CMSG_TRY_INVENTORY_QUICK_MOVE = 95,
    CMSG_TRY_INVENTORY_SOCKET = 96,
    CMSG_TRY_INVENTORY_STACK = 97,
    CMSG_TRY_INVENTORY_SPLIT_STACK = 98,
    CMSG_TRY_INVENTORY_DROP_STACK_PORTION = 99,
    CMSG_TRY_INVENTORY_STACK_TRANSFER = 100,
    CMSG_TRY_GOLD_TRANSFER = 101,
    SMSG_STACK_NOTIFY = 102,
    SMSG_STACK_DECREASE_NOTIFY = 103,
    SMSG_IDENTIFY_NOTIFY = 104,
    SMSG_DYE_NOTIFY = 105,
    CMSG_TRY_INVENTORY_SWAP_TO_BODY = 107,
    CMSG_TRY_INVENTORY_USE = 108,
    CMSG_HELPER_DETACH = 109,
    CMSG_TRY_ASSIGN_SKILL_TO_SLOT = 110,
    CMSG_UNASSIGN_SKILL = 111,
    CMSG_TRY_ASSIGN_TRAITS = 112,
    CMSG_TRY_ASSIGN_LEGENDARY_POWER = 113,
    SMSG_ASSIGNED_SKILL_TO_SLOT = 114,
    SMSG_ASSIGNED_TRAIT_TO_SLOT = 115,
    SMSG_ASSIGNED_LEGENDARY_POWER_TO_SLOT = 116,
    CMSG_TRY_CHOOSE_HIRELING_SKILL = 117,
    CMSG_TRY_PORTRAIT_DROP = 118,
    CMSG_PLAYER_CHANGE_POTION_BUTTON = 119,
    SMSG_AUTOASSIGNED_TO_BUTTON = 120,
    SMSG_PLAY_IDLE_ANIMATION = 122,
    SMSG_PLAY_NONPOSITIONAL_SOUND = 141,
    SMSG_PLAY_ERROR_SOUND = 142,
    SMSG_PLAY_MUSIC = 143,
    SMSG_STOP_MUSIC_OVERLAY = 144,
    SMSG_PLAY_CUTSCENE = 145,
    SMSG_PLAY_FINAL_CUTSCENE = 146,
    SMSG_PLAY_ANNIVERSARY_CUTSCENE = 147,
    SMSG_SCOMPLEX_EFFECT_ADD = 148,
    SMSG_FLIPPY = 149,
    SMSG_OPERATE_WAYPOINT = 151,
    SMSG_CONFIRM_OPERATION = 152,
    SMSG_OPEN_BANNER_CUSTOMIZATION = 153,
    CMSG_OPERATE_GIZMO = 154,
    CMSG_ACCEPT_RESURRECT = 155,
    SMSG_AIM_TARGET = 156,
    SMSG_ACD_CHANGE_GB_HANDLE = 157,
    SMSG_PVP_ARENA_WIN = 159,
    SMSG_PVP_ARENA_LOSE = 160,
    CCMSG_END_OF_TICK = 161,
    CMSG_TRY_WAYPOINT = 162,
    CMSG_NPC_TALK_REQUEST = 164,
    CMSG_NPC_VENDOR_REQUEST = 165,
    SMSG_FORCE_NPC_INTERACT_CLOSE = 167,
    SMSG_PLAYER_LEVEL = 174,
    SMSG_PLAYER_ALT_LEVEL = 175,
    SMSG_PLAYER_HIGHEST_HERO_SOLO_RIFT_LEVEL = 176,
    SMSG_SHARED_STASH_OPEN = 177,
    SMSG_ACD_PICKUP_FAILED = 178,
    SMSG_ATTACH_PET = 179,
    SMSG_DETACH_PET = 180,
    SMSG_SET_HIRELING_DEAD = 181,
    SMSG_UI_ELEMENT_ACTION = 182,
    CMSG_PLAYER_BUSY = 183,
    CMSG_PLAYER_LORE_PLAYING = 184,
    SMSG_TRADE = 186,
    CMSG_INSPECT_END = 188,
    SMSG_ACT = 193,
    SMSG_INTERSTITIAL = 194,
    SMSG_INVENTORY_CREATE = 198,
    SMSG_SHRINE_TEXT_DISPLAY = 199,
    SMSG_ACTIVATE_POOL_OF_REFLECTION = 200,
    SMSG_LOOTRUN_APPEARED = 203,
    SMSG_PLAYER_WARPED = 204,
    SMSG_VICTIM = 205,
    SMSG_MONSTER_ENGAGED = 206,
    SMSG_ELITE_KILLED = 207,
    SMSG_TREASURE_GOBLIN_ESCAPED = 208,
    SMSG_TOWN_PORTAL_USED = 209,
    SMSG_PORTED_TO_PLAYER = 210,
    SMSG_KILLCOUNT = 212,
    SMSG_TRIGGER_LIGHTNING = 214,
    CMSG_KEEPALIVE = 217,
    SMSG_PLAY_CONV_LINE = 218,
    SMSG_STOP_CONV_LINE = 219,
    CMSG_REQUEST_CONV_ADVANCE = 220,
    CMSG_UPDATE_CONV_AUTO_ADVANCE = 221,
    CMSG_REQUEST_CONV_CANCEL = 222,
    SMSG_END_CONV = 223,
    SMSG_CONFIRM_HIRELING_SWAP = 224,
    SMSG_HIRELING_AUTO_DISMISS = 225,
    CMSG_REQUEST_HIRELING_SWAP = 226,
    SMSG_SET_DEATH_FADE_TIMES = 227,
    SMSG_RESET_DEATH_FADE_TIMES = 228,
    SMSG_REMOVE_FROM_SPHERE_LIST = 229,
    SMSG_DISPLAY_GAME_TEXT = 230,
    SMSG_DISPLAY_GAME_CHAT_TEXT = 231,
    SMSG_DISPLAY_VAGUE_LOCKED_ERROR = 233,
    SMSG_DISPLAY_ITEM_REQUIRED_ERROR = 234,
    SMSG_CANCEL_ACD_TARGET = 235,
    SMSG_RESET_ACD_EFFECTS = 236,
    SMSG_LOOK_AT_ACD = 237,
    SMSG_KILL_COUNTER_UPDATE = 238,
    SMSG_LOW_HEALTH_COMBAT = 239,
    SMSG_SAVIOR = 240,
    SMSG_KILL_COUNTER_REFRESH = 241,
    SMSG_CURRENCY_COUNTER_UPDATE = 242,
    SMSG_FLOATING_NUMBER = 243,
    SMSG_FLOATING_GBID_AMOUNT = 244,
    SMSG_FLOATING_AMOUNT = 245,
    SMSG_REMOVE_NEAREST_RAGDOLL = 246,
    SMSG_PREFETCH = 247,
    SMSG_LORE_LEARNED = 248,
    CMSG_AV_CAMERA_MODE_TOGGLE = 249,
    CMSG_AV_CAMERA_LOCATION = 250,
    SMSG_TRIAL_OVER = 251,
    SMSG_DEMO_OVER = 252,
    SMGS_BLIZZCON_RIFT_ENDED = 253,
    SMSG_BETA_OVER = 254,
    SMSG_WORLD_DELETED = 255,
    SMSG_PVP_ARENA_RESTART = 256,
    SMSG_SET_SCREENSHOT_VIEW = 257,
    CMSG_SHARED_STASH_TRY_BUY = 264,
    SMSG_PLAYER_ACTIVATED_QUEST = 265,
    SMSG_PLAYER_COMPLETED_QUEST = 266,
    SMSG_PLAYER_DESYNC_SNAP = 267,
    CMSG_TRY_SALVAGE = 268,
    CMSG_TRY_SALVAGE_ALL = 269,
    SMSG_SALVAGE_RESULT = 270,
    SMSG_RETURNED_GEMS_NOTIFICATION = 271,
    SMSG_OBJECTIVE_FINDER_ACTIVATED = 274,
    SMSG_CRAFTING_DATA_TRANSMOG_INITIAL = 286,
    SMSG_EXTRACTED_LEGENDARIES_DATA_PARTIAL = 290,
    SMSG_RESET_EXTRACTED_LEGENDARIES = 292,
    CMSG_REQUEST_DEBUG_ACTOR_TOOLTIP = 293,
    SMSG_DISPLAY_DEBUG_ACTOR_TOOLTIP = 294,
    SMSG_BOSS_JOIN_ENCOUNTER = 295,
    CMSG_BOSS_JOIN_ACCEPTED = 296,
    CMSG_BOSS_START_ACCEPTED = 297,
    SMSG_BOSS_START_ENCOUNTER = 298,
    CMSG_BOSS_START_DECLINED = 299,
    CMSG_BOSS_JOIN_DECLINED = 300,
    SMSG_BOSS_ENCOUNTER_INVITE_STATE = 301,
    SMSG_BOSS_ENCOUNTER_STARTED = 302,
    SMSG_BOSS_ENCOUNTER_CANCELED = 303,
    SMSG_BOSS_ENCOUNTER_ONE_DECLINED = 304,
    CMSG_BOSS_VOTE_CONTINUE = 305,
    CMSG_BOSS_VOTE_CANCEL = 306,
    CMSG_SET_DUNGEON_JOIN_DECLINED = 312,
    CMSG_SET_DUNGEON_JOIN_ACCEPTED = 313,
    CMSG_SET_DUNGEON_VOTE_CONTINUE = 314,
    CMSG_SET_DUNGEON_VOTE_CANCEL = 315,
    SMSG_RIFT_START_PRELOADING_LEADERBOARD = 331,
    CMSG_TIERED_RIFT_CLOSE_REQUESTED = 332,
    SMSG_CAMERA_SCRIPTED_SEQUENCE_START = 333,
    SMSG_CAMERA_SCRIPTED_SEQUENCE_STOP = 334,
    SMSG_CAMERA_FOCUS_ACD = 335,
    SMSG_CAMERA_ZOOM = 336,
    SMSG_CAMERA_YAW = 337,
    SMSG_CAMERA_TRIGGER_FADE = 338,
    SMSG_CAMERA_TRIGGER_FADE_TO_BLACK = 339,
    SMSG_CAMERA_TRIGGER_INSTANT_FADE_TO_BLACK = 340,
    SMSG_BOSS_ZOOM = 341,
    CMSG_TRY_REFORGE = 342,
    SMSG_REFORGE_RESULTS = 343,
    CMSG_TRY_ENCHANT_AFFIX = 344,
    SMSG_CHOOSE_ENCHANT_AFFIX = 345,
    CMSG_CHOOSE_ENCHANT_AFFIX = 346,
    SMSG_ENCHANT_AFFIX_RESULTS = 347,
    SMSG_REPAIR_ACK = 351,
    SMSG_UNLOCK_TRANSMOG_RESULTS = 352,
    CMSG_TRY_TRANSMOG = 353,
    SMSG_TRANSMOG_RESULTS = 354,
    CMSG_TRY_DYE_ITEM = 355,
    SMSG_DYE_ITEM_RESULTS = 356,
    CMSG_UNLOCKED_DYES_REQUEST = 357,
    SMSG_UNLOCKED_DYES_RESULTS = 358,
    CMSG_TRY_TRANSMUTE = 359,
    SMSG_TRANSMUTE_RESULTS = 360,
    SMSG_TRANSMUTE_FAILED = 361,
    CMSG_VENDOR_TRY_TRAIN = 362,
    SMSG_VENDOR_TRAIN_RESULTS = 363,
    SMSG_MYSTERY_ITEM_RESULTS = 364,
    CMSG_TRY_UPGRADE_JEWEL = 365,
    SMSG_UPGRADE_JEWEL_RESULTS = 366,
    SMSG_DEBUG_DRAW_PRIM = 367,
    SMSG_DEBUG_UNLUCKY_BONUS = 368,
    CMSG_TRY_CRAFT_ITEM = 369,
    CMSG_TRY_CRAFT_ITEMS = 370,
    SMSG_CRAFTING_RESULT = 371,
    SMSG_VENDOR_LEVEL_UP = 372,
    CMSG_TRY_IDENTIFY_WITH_CAST = 373,
    CMSG_TRY_IDENTIFY_ALL_WITH_CAST = 374,
    CMSG_TRY_REMOVE_GEMS = 375,
    SMSG_CALLOUT_EVENT = 376,
    SMSG_CURRENT_HANDICAP_LEVEL = 377,
    CMSG_FOLLOWER_RETRAIN_REQUEST = 378,
    SMSG_GAME_TESTING_DONE = 379,
    SMSG_GAME_TESTING_START_WORLD_SAMPLING = 380,
    SMSG_GAME_TESTING_START_SKILL_SAMPLING = 381,
    SMSG_GAME_TESTING_STOP_SAMPLING = 382,
    CMSG_SHOWN_TUTORIAL = 383,
    CMSG_REQUEST_BUFF_CANCEL = 384,
    CMSG_REQUEST_EVENT_SKIP = 385,
    CMSG_REQUEST_PROMOTE_GAME_TO_PUBLIC = 387,
    SMSG_GAME_PROMOTED_TO_PUBLIC = 388,
    CMSG_PING = 390,
    SMSG_PONG = 391,
    SMSG_SPING = 392,
    CMSG_SPONG = 393,
    SMSG_SYSTEM_NOTIFICATION = 394,
    SMSG_SYSTEM_BROADCAST = 395,
    CMSG_LOAD_COMPLETE = 396,
    CMSG_NPC_INTERACT_CANCEL = 397,
    CMSG_NPC_INTERACT_TALK_DIALOG = 398,
    CMSG_NPC_INTERACT_PET_DISMISS = 399,
    CMSG_NPC_INTERACT_CRAFT = 400,
    CMSG_NPC_INTERACT_HIRE = 401,
    CMSG_NPC_INTERACT_INVENTORY = 402,
    CMSG_GAME_END_REQUEST = 403,
    CMSG_ACT_TRANSITION_REQUEST = 404,
    CMSG_ACT_TRANSITION_REQUEST_OPEN_WORLD = 405,
    CMSG_REQUEST_PAUSE_STATE_CHANGE = 414,
    SMSG_PAUSE_STATE_CHANGE = 415,
    SMSG_SOFT_PAUSE_STATE_CHANGE = 416,
    SMSG_ACCOUNT_FLAGS_SYNC = 417,
    SMSG_ADVENTURE_MODE_UNLOCKED = 418,
    SMSG_ADVENTURE_MODE_UNLOCKED_DEPRECATED = 418,  // 2.7.7 label; SMSG_ADVENTURE_MODE_UNLOCKED kept for existing uses
    SMSG_ACCOUNT_STASH_TABS_REWARDED_FOR_SEASONS_SYNC = 419,
    CMSG_TRIED_SALVAGE = 420,
    CMSG_POLL_MAIL = 421,
    CMSG_FETCH_MAIL = 422,
    CMSG_SEND_MAIL = 423,
    CMSG_MAIL_OPERATION = 424,
    SMSG_MAIL_CONTENTS = 425,
    SMSG_MAIL_SENT = 426,
    SMSG_MAIL_OPERATION_RESPONSE = 427,
    SMSG_PVP_KILL = 428,
    SMSG_PVP_ASSIST = 429,
    CMSG_TRY_REDEEM_PVP_TOKENS = 430,
    SMSG_PVP_TOKEN_REDEMPTION_RESULTS = 431,
    SMSG_SET_CAMERA_OBSERVER = 432,
    SMSG_SET_CAMERA_ORBIT = 433,
    SMSG_SET_CAMERA_DEFAULTS = 434,
    SMSG_CONSOLE_SET_CAMERA_DEFAULTS = 435,
    SMSG_CONSOLE_VALIDATE_HERO = 436,
    SMSG_CONSOLE_VALIDATE_ACCOUNT = 437,
    SMSG_CONSOLE_SAVE = 438,
    SMSG_CONSOLE_SYNC_HERO_SAVED_QUESTS = 439,
    SMSG_CONSOLE_SYNC_HERO_QUEST_HISTORY = 440,
    SMSG_CONSOLE_SYNC_HERO_QUEST_REWARD_HISTORY = 441,
    SMSG_CONSOLE_SYNC_HERO_SAVED_CONVERSATIONS = 442,
    SMSG_CONSOLE_SYNC_HERO_ACTIVE_HIRELING = 443,
    SMSG_CONSOLE_SYNC_HERO_TIME_PLAYED = 444,
    SMSG_CONSOLE_COUNTER_INCREMENT = 446,
    SMSG_CONSOLE_GAME_ENDED = 447,
    CMSG_SAVE_BANNER = 448,
    SMSG_SAVE_BANNER_RESPONSE = 449,
    SMSG_ACHIEVEMENTS_AVAILABILITY = 450,
    CMSG_ACHIEVEMENTS_GET_SNAPSHOT = 451,
    SMSG_ACHIEVEMENTS_SNAPSHOT = 452,
    SMSG_ACHIEVEMENT_PROGRESS_XBOXONE = 454,
    SMSG_CLEAR_FANFARE_QUEUE = 455,
    SMSG_MULTIPLAYER_BUFF_ON = 456,
    SMSG_MULTIPLAYER_BUFF_OFF = 457,
    SMSG_DUEL_RESULT = 458,
    SMSG_SHRINE_GLOBE_TEXT_DISPLAY = 459,
    CMSG_ITEM_JUNK_FLAG_CHANGED = 460,
    CMSG_PARAGON_POINT_SPEND = 461,
    CMSG_PARAGON_POINTS_RESET = 462,
    CMSG_PARAGON_PURCHASE_RESET_CATEGORY = 463,
    CMSG_REQUEST_PROMOTE_TO_PARTY_GUIDE = 464,
    SMSG_PLAYER_CLEAR_CLIENT_WALK_POWER = 465,
    SMSG_CHALLENGE_START = 466,
    SMSG_CHALLENGE_STARTED = 467,
    CMSG_CHALLENGE_ACCEPT = 468,
    SMSG_SET_DUNGEON_START = 469,
    CMSG_SET_DUNGEON_ACCEPT = 470,
    SMSG_SET_DUNGEON_CANNOT_START = 471,
    SMSG_SET_DUNGEON_CLOSING = 472,
    CMSG_WARP_DEAD_PLAYER = 473,
    CMSG_REQUEST_SOFT_PAUSE_STATE_CHANGE = 474,
    SMSG_ACTOR_LOOK_OVERRIDE_CHANGED = 475,
    CMSG_TELEPORT_TO_PLAYER = 477,
    CMSG_TELEPORT_TO_WAYPOINT = 478,
    CMSG_SUMMON_VANITY_PET = 479,
    SMSG_DUNGEON_FINDER_PROGRESS_GLYPH_PICK_UP = 484,
    CMSG_DUNGEON_FINDER_BOSS_START = 487,
    SMSG_NEW_SKILLKIT = 488,
    CMSG_OPEN_WORLD_MODE_CHANGE_REQUEST = 490,
    SMSG_OPEN_WORLD_MODE_CHANGE = 491,
    CMSG_OPEN_WORLD_MODE_CHANGE_ACCEPT = 492,
    CMSG_OPEN_WORLD_MODE_RESET_REQUEST = 493,
    SMSG_PLAY_OPEN_WORLD_TUTORIAL = 495,
    CMSG_GAMBLE = 496,
    CMSG_TRY_LOWER_DIFFICULTY = 497,
    SMSG_DIFFICULTY_LOWERED = 498,
    SMSG_DIFFICULTY_LOWERED_SAVE_PREFS = 499,
    SMSG_DIFFICULTY_LOWERED_FLAG = 500,
    CMSG_TRY_RAISE_DIFFICULTY = 501,
    SMSG_DIFFICULTY_RAISED = 502,
    SMSG_DIFFICULTY_RAISED_SAVE_PREFS = 503,
    SMSG_DIFFICULTY_RAISED_FLAG = 504,
    SMSG_HIRELING_UNLOCKED = 505,
    CMSG_TRY_CHANGE_DIFFICULTY = 506,
    SMSG_DIFFICULTY_CHANGED = 507,
    SMSG_MAILBOX_OPEN = 508,
    CMSG_MAIL_ITEM_SENT = 509,
    CMSG_MAIL_CURRENCY_SENT = 510,
    CMSG_MAIL_CURRENCY_RECEIVED = 511,
    CMSG_MAIL_ITEM_RECEIVED = 512,
    CMSG_MAIL_ITEM_RECEIVED_EQUIP = 513,
    CMSG_GIFT_ITEM_RECEIVED = 514,
    SMSG_AVENGER_CREATION = 515,
    SMSG_AVENGER_ENTER_KNOWN_LIST = 516,
    SMSG_AVENGER_SPAWN = 517,
    SMSG_AVENGER_KILLED = 518,
    CMSG_AVENGER_SEND_ACCOUNT_INFO = 519,
    CMSG_AVENGER_SEND_FRIEND_AVENGER = 520,
    CMSG_AVENGER_UPDATE_SOURCE = 521,
    CMSG_PLAYER_GIFT_REGISTER_RECIPIENT = 523,
    SMSG_PLAYER_GIFT_ITEM_GENERATED = 525,
    SMSG_PLAYER_GIFT_ITEM_INVENTORY_FULL = 526,
    SMSG_RIFT_POST_SCORE = 527,
    CMSG_SEND_SEASONAL_ITEM_TO_STASH = 528,
    CMSG_SEND_SEASONAL_ITEM_TO_INVENTORY = 529,
    SMSG_STASH_ITEM_RETRIEVED = 530,
    SMSG_UPDATED_VENDOR_RANDOM_SEEDS = 531,
    SMSG_UPDATED_PARTITION_WIDE_CUBE_RANDOM_SEEDS = 532,
    CMSG_DROP_CRAFTING_MATERIAL = 533,
    CMSG_DESTROY_GIFT = 534,
    CMSG_PLAYER_GIFT_SENT = 535,
    SMSG_UPSCALING_LEVEL_CHANGED = 536,
    SMSG_DUNGEON_FINDER_CLOSING = 537,
    CMSG_FINISHED_OPEN_WORLD_TUTORIAL = 538,
    SMSG_FORCE_UI_RESET = 539,
    SMSG_CLOSE_UI_COUNTDOWN = 541,
    CMSG_PLAYER_CHOOSE_RIFT_REWARD_TYPE = 542,
    SMSG_FORCE_HEALTH_RESYNC = 543,
    SMSG_WEEKLY_CHALLENGE_COMPLETED = 544,
    SMSG_CONSOLE_SYNC_HERO_PARTITION_FLAGS = 545,
    SMSG_DEBUG_DRAW_MOVEMENT_TYPE = 546,
    CMSG_EQUIP_COSMETIC_ITEM = 547,
    CMSG_UNEQUIP_COSMETIC_ITEM = 548,
    SMSG_CURSED_REALM_RESULTS = 549,
    SMSG_FIRST_OF_THE_DAY = 550,
    SMSG_PLATINUM_AWARDED = 551,
    SMSG_PLATINUM_ACHIEVEMENT_AWARDED = 552,
    SMSG_RIFT_PARTICIPANTS_ADDED = 553,
    SMSG_SHOW_RIFT_END_SCREEN = 554,
    SMSG_PRELOAD_LOOT_RUN_LEADERBOARD_INFO = 555,
    SMSG_SET_DUNGEON_RESULTS_MESSAGE = 556,
    CMSG_EQUIPMENT_MANAGER_CLOSE = 557,
    CMSG_REQUEST_LOADOUT = 558,
    CMSG_EQUIP_LOADOUT = 559,
    CMSG_SAVE_LOADOUT = 560,
    CMSG_CLEAR_LOADOUT = 561,
    CMSG_SET_LOADOUT_SLOT = 562,
    CMSG_SET_LOADOUT_TAB_ICON = 563,
    CMSG_SET_LOADOUT_NAME = 564,
    SMSG_LOADOUT_DATA = 566,
    SMSG_LOADOUT_SAVED = 567,
    SMSG_LOADOUT_EMPTY = 568,
    SMSG_LOADOUT_UPDATE = 569,
    SMSG_LOADOUT_EQUIP_NOTIFICATION = 570,
    CMSG_SWAP_HOTBAR_SKILLS = 571,
    CMSG_REDEEM_SEASON_REWARD_CONSOLE = 572,
    SMSG_REDEEM_SEASON_REWARD_CONSOLE = 573,
    SMSG_CHALLENGE_RIFT_REWARD_PC = 575,
    SMSG_CHALLENGE_RIFT_INSPECT_ARMORRACK = 577,
    SMSG_GREED_REALM_CLOSING = 579,
    SMSG_SWARM_RIFT_CLOSING = 580,
    SMSG_UBER_BOSS_WORLD_CLOSING = 581,
    SMSG_WARNING_COUNTDOWN_NOTIFY = 582,
    SMSG_AMIIBO_SCAN_BEGIN = 583,
    SMSG_AMIIBO_SCAN_END = 584,
    SMSG_SYNC_AMIIBO_SCAN_TIME = 585,
    CMSG_AMIIBO_SCAN_END = 586,
    CMSG_HAS_NFP_DEVICE = 587,
    CMSG_LOST_NFP_DEVICE = 588,
    SMSG_PLAYER_CHEATED_DEATH = 589,
    SMSG_UNLOCKABLE_RANDOM_AFFIX = 590,
    CMSG_DARK_ALCHEMY_ACTIVATE_NODE = 591,
    SMSG_DARK_ALCHEMY_NODE_ACTIVATED = 592,
    SMSG_DARK_ALCHEMY_INTERACT = 593,
    SMSG_CHUNKED = 594,
    MSG_RANGE_INGAME_END = 595,
    XMSG_SAVE_BANNER_COMPLETE = 595,
    MSG_NUM_MESSAGES = 596,
};

struct GenericBlobMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    uint32 uBlobSize;  // 0x08
};
static_assert(sizeof(GenericBlobMessage) == 0xc, "GenericBlobMessage");

struct ChunkMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 unk_08;  // 0x08
    BOOL unk_0c;  // 0x0c
};
static_assert(sizeof(ChunkMessage) == 0x10, "ChunkMessage");

struct BoolDataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    BOOL bVal;  // 0x08
};
static_assert(sizeof(BoolDataMessage) == 0xc, "BoolDataMessage");

struct ByteDataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    uint8 bData;  // 0x08
};
static_assert(sizeof(ByteDataMessage) == 0xc, "ByteDataMessage");

struct DataIDDataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    DataID id;  // 0x08
};
static_assert(sizeof(DataIDDataMessage) == 0xc, "DataIDDataMessage");

struct Int64DataMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int64 nData;  // 0x08
};
static_assert(sizeof(Int64DataMessage) == 0x10, "Int64DataMessage");

struct UInt64DataMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint64 uData;  // 0x08
};
static_assert(sizeof(UInt64DataMessage) == 0x10, "UInt64DataMessage");

struct FloatDataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    float flData;  // 0x08
};
static_assert(sizeof(FloatDataMessage) == 0xc, "FloatDataMessage");

struct SNONameDataMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNOName snoname;  // 0x08
};
static_assert(sizeof(SNONameDataMessage) == 0x10, "SNONameDataMessage");

struct PreloadACDDataMessage {  // size 0x34
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    SNO snoActor;  // 0x0c
    WeaponClass eWeaponClass;  // 0x10
    GBID gbidMonsterAffixes[8];  // 0x14
};
static_assert(sizeof(PreloadACDDataMessage) == 0x34, "PreloadACDDataMessage");

struct PrefetchActorDataMessage {  // size 0x2c
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoActor;  // 0x08
    GBID gbidMonsterAffixes[8];  // 0x0c
};
static_assert(sizeof(PrefetchActorDataMessage) == 0x2c, "PrefetchActorDataMessage");

struct GBIDDataMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    GBID gbidData;  // 0x08
};
static_assert(sizeof(GBIDDataMessage) == 0xc, "GBIDDataMessage");

struct DisplayGameTextMessage {  // size 0x418
    RequiredMessageHeader tHeader;  // 0x00
    CHAR szMsg[1024];  // 0x08
    XOptional<int> nParam1;  // 0x408
    XOptional<int> nParam2;  // 0x410
};
static_assert(sizeof(DisplayGameTextMessage) == 0x418, "DisplayGameTextMessage");

struct JoinLANGameMessage {  // size 0xc4
    RequiredMessageHeader tHeader;  // 0x00
    SGameID idSGame;  // 0x08
    CHAR szAccount[128];  // 0x0c
    CHAR szHero[49];  // 0x8c
    XLocale eXLocale;  // 0xc0
};
static_assert(sizeof(JoinLANGameMessage) == 0xc4, "JoinLANGameMessage");

struct BroadcastTextMessage {  // size 0x408
    RequiredMessageHeader tHeader;  // 0x00
    CHAR szMsg[1024];  // 0x08
};
static_assert(sizeof(BroadcastTextMessage) == 0x408, "BroadcastTextMessage");

enum {
    QUIT_REASON_NORMAL = 0,
    QUIT_REASON_REMOVED = 1,
    QUIT_REASON_MISMATCHING_PROTOCOL_VERSION = 2,
    QUIT_REASON_JOIN_FAILED = 3,
    QUIT_REASON_ASSET_MISMATCH = 4,
    QUIT_REASON_ACCOUNT_CREATE_FAILED = 5,
    QUIT_REASON_USING_NDB = 6,
    QUIT_REASON_SHUTDOWN_GAME = 7,
    QUIT_REASON_HARDCORE_DEATH = 8,
    QUIT_REASON_KICKED_BY_PLAYERS = 9,
    QUIT_REASON_GAME_END = 10,
    QUIT_REASON_NOT_LEGAL_FOR_RESTRICTED_PLAYER = 11,
    QUIT_REASON_GAME_HOST_LEFT_NEWLY_CREATED_GAME = 12,
    QUIT_REASON_GAME_ALREADY_OVER = 13,
    QUIT_REASON_COUNT = 14,
};

struct QuitGameMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    QuitReason eReason;  // 0x08
};
static_assert(sizeof(QuitGameMessage) == 0xc, "QuitGameMessage");

struct ConnectionEstablishedMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    uint32 dwAnimSyncedSeed;  // 0x0c
    uint32 dwSnoCoreDataHash;  // 0x10
};
static_assert(sizeof(ConnectionEstablishedMessage) == 0x14, "ConnectionEstablishedMessage");

struct GameSetupMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    uint32 dwFirstHeartbeat;  // 0x08
};
static_assert(sizeof(GameSetupMessage) == 0xc, "GameSetupMessage");

struct EnterKnownLookOverrides {  // size 0x18
    uint32 aLookOverrides[6];  // 0x00
};
static_assert(sizeof(EnterKnownLookOverrides) == 0x18, "EnterKnownLookOverrides");

struct EnterWorldMessage {  // size 0x3c
    RequiredMessageHeader tHeader;  // 0x00
    Vector3D wpLocation;  // 0x08
    SWorldID idSWorld;  // 0x14
    SNO snoWorld;  // 0x18
    int32 nPlayerIndex;  // 0x1c
    XOptional<EnterKnownLookOverrides> tEnterKnownLookOverrides;  // 0x20
};
static_assert(sizeof(EnterWorldMessage) == 0x3c, "EnterWorldMessage");

struct RevealWorldMessage {  // size 0x2c
    RequiredMessageHeader tHeader;  // 0x00
    SWorldID idSWorld;  // 0x08
    SNO snoWorld;  // 0x0c
    int32 nOriginX;  // 0x10
    int32 nOriginY;  // 0x14
    int32 nStitchSizeInFeetX;  // 0x18
    int32 nStitchSizeInFeetY;  // 0x1c
    int32 nWorldSizeInFeetX;  // 0x20
    int32 nWorldSizeInFeetY;  // 0x24
    SNO snoDungeonFinderSourceWorld;  // 0x28
};
static_assert(sizeof(RevealWorldMessage) == 0x2c, "RevealWorldMessage");

struct WorldLocationMessageData {  // size 0x24
    float flScale;  // 0x00
    PRTransform transform;  // 0x04
    SWorldID idSWorld;  // 0x20
};
static_assert(sizeof(WorldLocationMessageData) == 0x24, "WorldLocationMessageData");

struct ACDEnterKnownMessage {  // size 0xa8
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    SNO snoActor;  // 0x0c
    uint32 dwFlags;  // 0x10
    ActorLocationType eLocationType;  // 0x14
    XOptional<WorldLocationMessageData> tWorldLocationData;  // 0x18
    XOptional<InventoryLocationMessageData> tInvLocationData;  // 0x40
    GBHandle hGB;  // 0x54
    SNOName tSNOName;  // 0x5c
    int32 nRarity;  // 0x64
    uint8 nLookLinkIndex;  // 0x68
    XOptional<int> snoMarkerSet;  // 0x6c
    XOptional<int> nMarkerIndex;  // 0x74
    XOptional<int> snoAmbientOcclusionOverrideTex;  // 0x7c
    XOptional<unsigned int> dwTimeActorCreated;  // 0x84
    XOptional<EnterKnownLookOverrides> tEnterKnownLookOverrides;  // 0x8c
};
static_assert(sizeof(ACDEnterKnownMessage) == 0xa8, "ACDEnterKnownMessage");

struct PlayerEnterKnownMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    ACDNetworkName ann;  // 0x0c
};
static_assert(sizeof(PlayerEnterKnownMessage) == 0x10, "PlayerEnterKnownMessage");

struct ACDWorldPositionMessage {  // size 0x4c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    WorldLocationMessageData tWorldLocationData;  // 0x0c
    XOptional<EnterKnownLookOverrides> tLookOverrides;  // 0x30
};
static_assert(sizeof(ACDWorldPositionMessage) == 0x4c, "ACDWorldPositionMessage");

struct ACDPickupFailedMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    BOOL bIsCurrency;  // 0x08
    PickupResponse ePickupResponse;  // 0x0c
    ACDNetworkName annPickee;  // 0x10
};
static_assert(sizeof(ACDPickupFailedMessage) == 0x14, "ACDPickupFailedMessage");

struct ProjectileStickMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    Vector3D wpSourceLoc;  // 0x08
    ACDNetworkName annTarget;  // 0x14
    SNO snoActorAttachment;  // 0x18
};
static_assert(sizeof(ProjectileStickMessage) == 0x1c, "ProjectileStickMessage");

struct AnimPreplayData {  // size 0xc
    uint32 dwAnimPrePlayDataFlags;  // 0x00
    uint32 dwServerTimeAnimStarted;  // 0x04
    uint32 dwSyncedSeed;  // 0x08
    AnimPreplayData() : dwAnimPrePlayDataFlags(0), dwServerTimeAnimStarted(0), dwSyncedSeed(0) {}  // GameMessage.h:1347; body from the copies inlined into CPlayerUsePowerOnTarget (three zero stores).
};
static_assert(sizeof(AnimPreplayData) == 0xc, "AnimPreplayData");

struct TargetMessage {  // size 0x4c
    RequiredMessageHeader tHeader;  // 0x00
    TargetType eType;  // 0x08
    ACDNetworkName annTarget;  // 0x0c
    WorldPlace tPlace;  // 0x10
    SNO snoPower;  // 0x20
    ACDNetworkName annItemBeingUsed;  // 0x24
    int32 nComboLevel;  // 0x28
    XOptional<AnimPreplayData> tAnimPreplayData;  // 0x2c
    XOptional<unsigned int> dwFlags;  // 0x3c
    XOptional<float> flBattlePayClickStats;  // 0x44
};
static_assert(sizeof(TargetMessage) == 0x4c, "TargetMessage");

struct SecondaryAnimationPowerMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoPower;  // 0x08
    ACDNetworkName annTarget;  // 0x0c
    XOptional<AnimPreplayData> tAnimPreplayData;  // 0x10
};
static_assert(sizeof(SecondaryAnimationPowerMessage) == 0x20, "SecondaryAnimationPowerMessage");

struct LoopingAnimationPowerMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoPower;  // 0x08
    SNO snoData0;  // 0x0c
    int32 nData1;  // 0x10
};
static_assert(sizeof(LoopingAnimationPowerMessage) == 0x14, "LoopingAnimationPowerMessage");

struct TryConsoleCommand {  // size 0x424
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    CHAR usMessage[1024];  // 0x0c
    WorldPlace tPlaceTarget;  // 0x40c
    ACDNetworkName annTarget;  // 0x41c
    uint32 dwFlags;  // 0x420
};
static_assert(sizeof(TryConsoleCommand) == 0x424, "TryConsoleCommand");

struct TryChatMessage {  // size 0x410
    RequiredMessageHeader tHeader;  // 0x00
    TryChatDestination eDestination;  // 0x08
    int32 nPlayerTarget;  // 0x0c
    CHAR usMessage[1024];  // 0x10
};
static_assert(sizeof(TryChatMessage) == 0x410, "TryChatMessage");

struct ChatMessage {  // size 0x410
    RequiredMessageHeader tHeader;  // 0x00
    EChatColor eColor;  // 0x08
    int32 nPlayerIndex;  // 0x0c
    CHAR usMessage[1024];  // 0x10
};
static_assert(sizeof(ChatMessage) == 0x410, "ChatMessage");

struct VictimMessage {  // size 0x2c
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerVictimIndex;  // 0x08
    int32 nKillerLevel;  // 0x0c
    int32 nKillerPlayerIndex;  // 0x10
    MonsterRarity eKillerMonsterRarity;  // 0x14
    SNO snoKillerActor;  // 0x18
    Team eKillerTeam;  // 0x1c
    GBID pKillerRareNameGBIDs[2];  // 0x20
    SNO snoPowerDmgSource;  // 0x28
};
static_assert(sizeof(VictimMessage) == 0x2c, "VictimMessage");

struct LootRunAppearedMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoLevelArea;  // 0x08
};
static_assert(sizeof(LootRunAppearedMessage) == 0xc, "LootRunAppearedMessage");

struct CombatEngagementMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    SNO snoActorMonster;  // 0x0c
    MonsterRarity eMonsterRarity;  // 0x10
    GBID pRareNameGBIDs[2];  // 0x14
};
static_assert(sizeof(CombatEngagementMessage) == 0x1c, "CombatEngagementMessage");

struct DuelResultMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerVictimIndex;  // 0x08
    int32 nPlayerKillerIndex;  // 0x0c
    SNO snoPowerDmgSource;  // 0x10
};
static_assert(sizeof(DuelResultMessage) == 0x14, "DuelResultMessage");

struct KillCountMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    int32 nPlayerKills;  // 0x0c
    int32 nDeaths;  // 0x10
    int32 nAssists;  // 0x14
};
static_assert(sizeof(KillCountMessage) == 0x18, "KillCountMessage");

struct StackPortionMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    uint64 uAmount;  // 0x10
};
static_assert(sizeof(StackPortionMessage) == 0x18, "StackPortionMessage");

struct CurrencyMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    CurrencyType eCurrency;  // 0x08
    uint64 uAmount;  // 0x10
};
static_assert(sizeof(CurrencyMessage) == 0x18, "CurrencyMessage");

struct ChangeUsableItemMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItem;  // 0x08
    GBID gbidItem;  // 0x0c
};
static_assert(sizeof(ChangeUsableItemMessage) == 0x10, "ChangeUsableItemMessage");

struct InventoryStackTransferMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItemSrc;  // 0x08
    ACDNetworkName annItemDest;  // 0x0c
    uint64 uAmount;  // 0x10
};
static_assert(sizeof(InventoryStackTransferMessage) == 0x18, "InventoryStackTransferMessage");

struct GoldTransferMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annPlayer;  // 0x08
    uint64 uAmount;  // 0x10
};
static_assert(sizeof(GoldTransferMessage) == 0x18, "GoldTransferMessage");

struct InventoryRequestSocketMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annGem;  // 0x08
    ACDNetworkName annItemToReceiveGem;  // 0x0c
};
static_assert(sizeof(InventoryRequestSocketMessage) == 0x10, "InventoryRequestSocketMessage");

struct InventoryRequestUseMessage {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annItemToUse;  // 0x08
    TargetType eType;  // 0x0c
    ACDNetworkName annTarget;  // 0x10
    WorldPlace tPlace;  // 0x14
};
static_assert(sizeof(InventoryRequestUseMessage) == 0x24, "InventoryRequestUseMessage");

struct HelperDetachMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annHelper;  // 0x08
};
static_assert(sizeof(HelperDetachMessage) == 0xc, "HelperDetachMessage");

struct PetMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    int32 nOwner;  // 0x08
    int32 nIndex;  // 0x0c
    ACDNetworkName annPet;  // 0x10
    int32 nType;  // 0x14
};
static_assert(sizeof(PetMessage) == 0x18, "PetMessage");

struct PetDetachMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annPet;  // 0x08
    BOOL bDisplayChatMessage;  // 0x0c
};
static_assert(sizeof(PetDetachMessage) == 0x10, "PetDetachMessage");

struct FlippyMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annLauncher;  // 0x08
    SNO snoFlippyParticle;  // 0x0c
    SNO snoActorLoot;  // 0x10
    Vector3D wpDest;  // 0x14
};
static_assert(sizeof(FlippyMessage) == 0x20, "FlippyMessage");

struct ComplexEffectAddMessage {  // size 0x28
    RequiredMessageHeader tHeader;  // 0x00
    SComplexEffectID idSComplexEffect;  // 0x08
    ComplexEffectType eType;  // 0x0c
    SNO snoEffectGroup;  // 0x10
    ACDNetworkName annSource;  // 0x14
    ACDNetworkName annDest;  // 0x18
    uint32 dwParam1;  // 0x1c
    uint32 dwParam2;  // 0x20
    uint32 bIgnoreOwnerAlpha;  // 0x24
};
static_assert(sizeof(ComplexEffectAddMessage) == 0x28, "ComplexEffectAddMessage");

struct PlayerLevel {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    int32 nNewLevel;  // 0x0c
};
static_assert(sizeof(PlayerLevel) == 0x10, "PlayerLevel");

struct AimTargetMessage {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    TargetType eType;  // 0x0c
    ACDNetworkName annTarget;  // 0x10
    WorldPlace tPlace;  // 0x14
};
static_assert(sizeof(AimTargetMessage) == 0x24, "AimTargetMessage");

struct ACDChangeGBHandleMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    GBHandle hGB;  // 0x0c
};
static_assert(sizeof(ACDChangeGBHandleMessage) == 0x14, "ACDChangeGBHandleMessage");

enum TrickleFlagBits {
    TRICKLE_FLAG_DEAD_BIT = 0,
    TRICKLE_FLAG_OBSERVER_BIT = 1,
    TRICKLE_FLAG_NOT_ON_MAP = 2,
    TRICKLE_FLAG_HIDDEN = 3,
    TRICKLE_FLAG_BECOMES_ARROW = 4,
    TRICKLE_FLAG_RESPECTS_FOW = 5,
    TRICKLE_FLAG_PVP_PLAYER_REVEALED_ON_MAP = 6,
    TRICKLE_FLAG_GHOSTED_BIT = 7,
    TRICKLE_FLAG_HUNTER = 8,
    TRICKLE_FLAG_LOOT_RUN_BOSS = 9,
    TRICKLE_FLAG_COUNT = 10,
};

enum TrickleAction {
    TRICKLE_ACTION_NONE = 0,
    TRICKLE_ACTION_BUSY = 1,
    TRICKLE_ACTION_TOWN_PORTAL = 2,
    TRICKLE_ACTION_BOUNTY = 3,
    TRICKLE_ACTION_DUNGEON_FINDER = 4,
    TRICKLE_ACTION_IN_TOWN = 5,
    TRICKLE_ACTION_IN_BOSS_FIGHT = 6,
    TRICKLE_ACTION_IN_COMBAT = 7,
};

struct TrickleMessage {  // size 0xbc
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    SNO snoActor;  // 0x0c
    WorldPlace tPlace;  // 0x10
    XOptional<int> nPlayerIndexOwner;  // 0x20
    SNO snoLevelArea;  // 0x28
    XOptional<float> flHealthPercent;  // 0x2c
    TrickleInfoType eTrickleInfoType;  // 0x34
    uint32 dwTrickleFlags;  // 0x38
    XOptional<int> dwHeadstoneCorpseReviveTime;  // 0x3c
    XOptional<int> snoMinimapIcon;  // 0x44
    XOptional<unsigned int> hMinimapStringLabel;  // 0x4c
    XOptional<unsigned int> hMinimapIconLabel;  // 0x54
    XOptional<int> snoStringList;  // 0x5c
    XOptional<float> fMinDisplayRangeSq;  // 0x64
    XOptional<float> fMaxDisplayRangeSq;  // 0x6c
    XOptional<int> nAction;  // 0x74
    XOptional<unsigned int> uEngagedWithRareTime;  // 0x7c
    XOptional<unsigned int> uEngagedWithGoblinTime;  // 0x84
    XOptional<int> bInCombat;  // 0x8c
    XOptional<int> nTieredRiftRank;  // 0x94
    XOptional<int> bBrawling;  // 0x9c
    XOptional<int> bInUberFight;  // 0xa4
    XOptional<int> gbidPortraitFrame;  /* 0xac */  XOptional<int> unk_b4;  // 0xb4 (2.7.7: copied into ClientTrickleEntry::unk_68 by CTrickleUpdate)
};
static_assert(sizeof(TrickleMessage) == 0xbc, "TrickleMessage");

struct PlayerIntValMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    int32 nVal;  // 0x0c
};
static_assert(sizeof(PlayerIntValMessage) == 0x10, "PlayerIntValMessage");

struct UIElementMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    UIElement eElement;  // 0x08
    UIElementAction eAction;  // 0x0c
};
static_assert(sizeof(UIElementMessage) == 0x10, "UIElementMessage");

struct PlasmaAttachMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annPlasma;  // 0x08
    ACDNetworkName annAttachTo;  // 0x0c
};
static_assert(sizeof(PlasmaAttachMessage) == 0x10, "PlasmaAttachMessage");

struct RitualTetherEffectMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annFrom;  // 0x08
    WorldPlace tPlaceDest;  // 0x0c
};
static_assert(sizeof(RitualTetherEffectMessage) == 0x1c, "RitualTetherEffectMessage");

struct RevealTeamMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    Team eTeam;  // 0x08
    uint32 dwTeamFlags;  // 0x0c
    TeamColor eTeamColoring;  // 0x10
};
static_assert(sizeof(RevealTeamMessage) == 0x14, "RevealTeamMessage");

struct DeathFadeTimeMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    int32 nTimeUntilFade;  // 0x0c
    int32 nFadeTime;  // 0x10
    BOOL bRecurseForAttachments;  // 0x14
};
static_assert(sizeof(DeathFadeTimeMessage) == 0x18, "DeathFadeTimeMessage");

struct MapRevealSceneMessage {  // size 0x34
    RequiredMessageHeader tHeader;  // 0x00
    SSceneID idSScene;  // 0x08
    SNO snoScene;  // 0x0c
    PRTransform transform;  // 0x10
    SWorldID idSWorld;  // 0x2c
    BOOL bRevealed;  // 0x30
};
static_assert(sizeof(MapRevealSceneMessage) == 0x34, "MapRevealSceneMessage");

struct ACDLookAtMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annSrc;  // 0x08
    ACDNetworkName annTarget;  // 0x0c
};
static_assert(sizeof(ACDLookAtMessage) == 0x10, "ACDLookAtMessage");

struct KillCounterUpdateMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    KillCounterType eType;  // 0x08
    uint32 uCount;  // 0x0c
    float flXPMultiplier;  // 0x10
    uint32 uTotalTime;  // 0x14
    BOOL bExpired;  // 0x18
};
static_assert(sizeof(KillCounterUpdateMessage) == 0x1c, "KillCounterUpdateMessage");

struct CurrencyCounterUpdateMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    uint32 uAmountCollected;  // 0x08
    CurrencyType eCurrencyType;  // 0x0c
    BOOL bExpired;  // 0x10
};
static_assert(sizeof(CurrencyCounterUpdateMessage) == 0x14, "CurrencyCounterUpdateMessage");

struct LowHealthCombatMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    float flHealthPercent;  // 0x08
    uint32 uXPRewarded;  // 0x0c
};
static_assert(sizeof(LowHealthCombatMessage) == 0x10, "LowHealthCombatMessage");

struct SaviorMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annSavedPlayer;  // 0x08
    uint32 uXPRewarded;  // 0x0c
};
static_assert(sizeof(SaviorMessage) == 0x10, "SaviorMessage");

struct FloatingNumberMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annTarget;  // 0x08
    float flDamage;  // 0x0c
    FloatingNumberType eType;  // 0x10
};
static_assert(sizeof(FloatingNumberMessage) == 0x14, "FloatingNumberMessage");

struct FloatingGBIDAmountMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    WorldPlace wpPlace;  // 0x08
    int32 nAmount;  // 0x18
    GBID gbidItem;  // 0x1c
};
static_assert(sizeof(FloatingGBIDAmountMessage) == 0x20, "FloatingGBIDAmountMessage");

struct FloatingAmountMessage {  // size 0x28
    RequiredMessageHeader tHeader;  // 0x00
    WorldPlace wpPlace;  // 0x08
    int32 nAmount;  // 0x18
    XOptional<int> nAmount2;  // 0x1c
    FloatingNumberType eType;  // 0x24
};
static_assert(sizeof(FloatingAmountMessage) == 0x28, "FloatingAmountMessage");

struct CalloutMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    XOptional<float> flAmount;  // 0x08
    XOptional<unsigned int> uTime;  // 0x10
    CalloutEventType eType;  // 0x18
};
static_assert(sizeof(CalloutMessage) == 0x1c, "CalloutMessage");

struct RemoveRagdollMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annTarget;  // 0x08
    SNO snoMonster;  // 0x0c
};
static_assert(sizeof(RemoveRagdollMessage) == 0x10, "RemoveRagdollMessage");

struct TryWaypointMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annWaypoint;  // 0x08
    uint32 nWaypoint;  // 0x0c
};
static_assert(sizeof(TryWaypointMessage) == 0x10, "TryWaypointMessage");

struct LoreMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoLore;  // 0x08
};
static_assert(sizeof(LoreMessage) == 0xc, "LoreMessage");

struct PRTransformMessage {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    PRTransform tTransform;  // 0x08
};
static_assert(sizeof(PRTransformMessage) == 0x24, "PRTransformMessage");

struct WorldDeletedMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SWorldID idSWorld;  // 0x08
};
static_assert(sizeof(WorldDeletedMessage) == 0xc, "WorldDeletedMessage");

struct PlayerQuestMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndex;  // 0x08
    SNO snoQuest;  // 0x0c
    SNO snoLevelArea;  // 0x10
};
static_assert(sizeof(PlayerQuestMessage) == 0x14, "PlayerQuestMessage");

struct PlayerDeSyncSnapMessage {  // size 0x24
    RequiredMessageHeader tHeader;  // 0x00
    WorldPlace placeSnap;  // 0x08
    DeSyncReason eReason;  // 0x18
    SNO snoPowerCurrent;  // 0x1c
    int32 nPlayerIndex;  // 0x20
};
static_assert(sizeof(PlayerDeSyncSnapMessage) == 0x24, "PlayerDeSyncSnapMessage");

struct BlizzconCVarsMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    BOOL bMultiplayerEnabled;  // 0x08
    BOOL bDHunterEnabled;  // 0x0c
    BOOL bBlizzconDropEnabled;  // 0x10
};
static_assert(sizeof(BlizzconCVarsMessage) == 0x14, "BlizzconCVarsMessage");

struct CameraFocusMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    BOOL bSnap;  // 0x0c
    float flDuration;  // 0x10
};
static_assert(sizeof(CameraFocusMessage) == 0x14, "CameraFocusMessage");

struct PVPTokenRedemptionMessage {  // size 0x3c
    RequiredMessageHeader tHeader;  // 0x00
    uint32 nItems;  // 0x08
    ACDNetworkName arrItems[4];  // 0x0c
    GBID arrGBIDs[4];  // 0x1c
    uint32 nStackSize[4];  // 0x2c
};
static_assert(sizeof(PVPTokenRedemptionMessage) == 0x3c, "PVPTokenRedemptionMessage");

struct ActTransitionMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    Act eAct;  // 0x08
    BOOL bOnJoin;  // 0x0c
};
static_assert(sizeof(ActTransitionMessage) == 0x10, "ActTransitionMessage");

struct InterstitialMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    PlayerClass ePlayerClass;  // 0x08
    BOOL fIsMale;  // 0x0c
};
static_assert(sizeof(InterstitialMessage) == 0x10, "InterstitialMessage");

struct BossEncounterMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 nPlayerIndex;  // 0x08
    SNO snoEncounter;  // 0x0c
};
static_assert(sizeof(BossEncounterMessage) == 0x10, "BossEncounterMessage");

struct SetDungeonMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 nPlayerIndex;  // 0x08
    SNO snoEncounter;  // 0x0c
};
static_assert(sizeof(SetDungeonMessage) == 0x10, "SetDungeonMessage");

struct DebugDrawPrimMessage {  // size 0xbc
    RequiredMessageHeader tHeader;  // 0x00
    uint32 uID;  // 0x08
    DebugDrawPrimType eType;  // 0x0c
    SWorldID idSWorld;  // 0x10
    Vector3D vPos0;  // 0x14
    Vector3D vPos1;  // 0x20
    float flLength;  // 0x2c
    float flRadius;  // 0x30
    uint32 uLifetimeInTicks;  // 0x34
    RGBAColor rgbaColor;  // 0x38
    CHAR szText[128];  // 0x3c
};
static_assert(sizeof(DebugDrawPrimMessage) == 0xbc, "DebugDrawPrimMessage");

struct DebugDrawMovementTypeMessage {  // size 0x90
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    uint32 dwTimeout;  // 0x0c
    CHAR szText[128];  // 0x10
};
static_assert(sizeof(DebugDrawMovementTypeMessage) == 0x90, "DebugDrawMovementTypeMessage");

struct CameraZoomMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    float flZoom;  // 0x08
    BOOL bSnap;  // 0x0c
    float flDuration;  // 0x10
};
static_assert(sizeof(CameraZoomMessage) == 0x14, "CameraZoomMessage");

struct CameraYawMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    Angle aAngle;  // 0x08
    BOOL bSnap;  // 0x0c
    float flDuration;  // 0x10
};
static_assert(sizeof(CameraYawMessage) == 0x14, "CameraYawMessage");

struct BossZoomMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    float flZoom;  // 0x08
    float flZoomSpeed;  // 0x0c
};
static_assert(sizeof(BossZoomMessage) == 0x10, "BossZoomMessage");

struct PlayCutsceneMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    int32 nIndex;  // 0x08
};
static_assert(sizeof(PlayCutsceneMessage) == 0xc, "PlayCutsceneMessage");

struct PlayerWarpedMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    PlayerWarpReason eWarpReason;  // 0x08
    float flWarpFadeInSeconds;  // 0x0c
};
static_assert(sizeof(PlayerWarpedMessage) == 0x10, "PlayerWarpedMessage");

struct DebugActorTooltipMessage {  // size 0x40c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    CHAR szTooltip[1024];  // 0x0c
};
static_assert(sizeof(DebugActorTooltipMessage) == 0x40c, "DebugActorTooltipMessage");

struct TrySalvageAllMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    SalvageAllType eSalvageType;  // 0x08
};
static_assert(sizeof(TrySalvageAllMessage) == 0xc, "TrySalvageAllMessage");

struct GameTestingWorldSamplingStartMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoWorld;  // 0x08
    SNO snoScene;  // 0x0c
};
static_assert(sizeof(GameTestingWorldSamplingStartMessage) == 0x10, "GameTestingWorldSamplingStartMessage");

struct GameTestingSkillSamplingStartMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoPower;  // 0x08
    int32 nRuneIndex;  // 0x0c
};
static_assert(sizeof(GameTestingSkillSamplingStartMessage) == 0x10, "GameTestingSkillSamplingStartMessage");

struct RequestBuffCancelMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoPower;  // 0x08
    int32 nBuffIndex;  // 0x0c
};
static_assert(sizeof(RequestBuffCancelMessage) == 0x10, "RequestBuffCancelMessage");

struct PlayErrorSoundMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    ActorTag dwSoundTag;  // 0x08
};
static_assert(sizeof(PlayErrorSoundMessage) == 0xc, "PlayErrorSoundMessage");

struct HirelingRequestLearnSkillMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    SNO snoSkill;  // 0x0c
};
static_assert(sizeof(HirelingRequestLearnSkillMessage) == 0x10, "HirelingRequestLearnSkillMessage");

struct LogoutTickTimeMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    LogoutContext eContext;  // 0x08
    uint32 uMinWaitTicks;  // 0x0c
    uint32 uMaxWaitTicks;  // 0x10
    uint32 uKickTicks;  // 0x14
};
static_assert(sizeof(LogoutTickTimeMessage) == 0x18, "LogoutTickTimeMessage");

struct VersionsMessage {  // size 0x30
    RequiredMessageHeader tHeader;  // 0x00
    uint32 uSNOPackHash;  // 0x08
    uint32 uProtocolHash;  // 0x0c
    char szBuildVer[32];  // 0x10
};
static_assert(sizeof(VersionsMessage) == 0x30, "VersionsMessage");

struct HandicapMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    int32 nHandicapLevel;  // 0x08
};
static_assert(sizeof(HandicapMessage) == 0xc, "HandicapMessage");

struct PlayerSetCameraObserverMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoObserver;  // 0x08
    XOptional<WorldPlace> wpToOrbit;  // 0x0c
};
static_assert(sizeof(PlayerSetCameraObserverMessage) == 0x20, "PlayerSetCameraObserverMessage");

struct PlayerSetCameraOrbitMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    WorldPlace wpToOrbit;  // 0x08
};
static_assert(sizeof(PlayerSetCameraOrbitMessage) == 0x18, "PlayerSetCameraOrbitMessage");

struct PlayerSetCameraDefaultsMessage {  // size 0x8
    RequiredMessageHeader tHeader;  // 0x00
};
static_assert(sizeof(PlayerSetCameraDefaultsMessage) == 0x8, "PlayerSetCameraDefaultsMessage");

struct PlayerConsoleSetCameraDefaultsMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    BOOL bForce;  // 0x08
};
static_assert(sizeof(PlayerConsoleSetCameraDefaultsMessage) == 0xc, "PlayerConsoleSetCameraDefaultsMessage");

struct PlayerClearClientWalkPowerMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    SNO snoPower;  // 0x0c
};
static_assert(sizeof(PlayerClearClientWalkPowerMessage) == 0x10, "PlayerClearClientWalkPowerMessage");

struct ActorLookOverrideChangedMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    int32 nChange;  // 0x0c
    uint32 nLookOverrideHash;  // 0x10
};
static_assert(sizeof(ActorLookOverrideChangedMessage) == 0x14, "ActorLookOverrideChangedMessage");

struct PortedToPlayerMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nPlayerIndexPorted;  // 0x08
    int32 nPlayerIndexTarget;  // 0x0c
};
static_assert(sizeof(PortedToPlayerMessage) == 0x10, "PortedToPlayerMessage");

struct ItemJunkFlagChangedMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    BOOL bJunkFlag;  // 0x0c
};
static_assert(sizeof(ItemJunkFlagChangedMessage) == 0x10, "ItemJunkFlagChangedMessage");

struct AchievementProgressXboxOneMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    uint32 nPlayerIndex;  // 0x08
    uint32 nProfileAwardId;  // 0x0c
    uint32 nProgress;  // 0x10
};
static_assert(sizeof(AchievementProgressXboxOneMessage) == 0x14, "AchievementProgressXboxOneMessage");

struct ConsoleCounterIncrementMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nCounter;  // 0x08
    uint32 uValue;  // 0x0c
};
static_assert(sizeof(ConsoleCounterIncrementMessage) == 0x10, "ConsoleCounterIncrementMessage");

struct ServerNotificationDataMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 uSec;  // 0x08
    ServerShutdownType eType;  // 0x0c
};
static_assert(sizeof(ServerNotificationDataMessage) == 0x10, "ServerNotificationDataMessage");

struct PlayerGiftMessage {  // size 0x40
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    uint64 nId;  // 0x10
    int32 nOwnerIndex;  // 0x18
    char szPlayerName[32];  // 0x1c
};
static_assert(sizeof(PlayerGiftMessage) == 0x40, "PlayerGiftMessage");

struct JewelUpgradeResultsMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    int32 nNumRanksGained;  // 0x0c
};
static_assert(sizeof(JewelUpgradeResultsMessage) == 0x10, "JewelUpgradeResultsMessage");

struct DungeonFinderClosingMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    uint32 nTickClosing;  // 0x08
    int32 nRiftLevel;  // 0x0c
};
static_assert(sizeof(DungeonFinderClosingMessage) == 0x10, "DungeonFinderClosingMessage");

struct UberBosssClosingMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 unk_08;  // 0x08
    int32 unk_0c;  // 0x0c
};
static_assert(sizeof(UberBosssClosingMessage) == 0x10, "UberBosssClosingMessage");

struct WarningCountdownNotificationMessage {  // size 0x10c
    RequiredMessageHeader tHeader;  // 0x00
    int32 unk_08;  // 0x08
    CHAR unk_0c[255];  // 0x0c
};
static_assert(sizeof(WarningCountdownNotificationMessage) == 0x10c, "WarningCountdownNotificationMessage");

// 2.7.7 dark-alchemy messages; descriptors are built in the static init sub_71004b4c50 (a TU not in the source tree yet).
struct ActivateNodeMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 unk_08;  // 0x08
    int32 unk_0c;  // 0x0c
};
static_assert(sizeof(ActivateNodeMessage) == 0x10, "ActivateNodeMessage");

struct DectivateNodeMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    int32 unk_08;  // 0x08
};
static_assert(sizeof(DectivateNodeMessage) == 0xc, "DectivateNodeMessage");

struct GizmoInteractMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 unk_08;  // 0x08
    int32 unk_0c;  // 0x0c
};
static_assert(sizeof(GizmoInteractMessage) == 0x10, "GizmoInteractMessage");

struct BlizzconEndScreenMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nRewardTier;  // 0x08
    ACDNetworkName ann;  // 0x0c
};
static_assert(sizeof(BlizzconEndScreenMessage) == 0x10, "BlizzconEndScreenMessage");

struct HoradricQuestCursedRealmResults {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nActIndex;  // 0x08
    int32 nRewardAmount;  // 0x0c
};
static_assert(sizeof(HoradricQuestCursedRealmResults) == 0x10, "HoradricQuestCursedRealmResults");

struct FirstOfTheDayMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    uint32 eType;  // 0x08
    uint64 uCurrentPlatinum;  // 0x10
    uint64 uPlatinumIncrement;  // 0x18
};
static_assert(sizeof(FirstOfTheDayMessage) == 0x20, "FirstOfTheDayMessage");

struct PlatinumAwardedMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    uint64 uCurrentPlatinum;  // 0x08
    uint64 uPlatinumIncrement;  // 0x10
};
static_assert(sizeof(PlatinumAwardedMessage) == 0x18, "PlatinumAwardedMessage");

struct PlatinumAchievementAwardedMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    uint64 uCurrentPlatinum;  // 0x08
    uint64 uPlatinumIncrement;  // 0x10
    OnlineService::AchievementId idAchievement;  // 0x18
};
static_assert(sizeof(PlatinumAchievementAwardedMessage) == 0x20, "PlatinumAchievementAwardedMessage");

struct SetDungeonResultsMessage {  // size 0x38
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoQuestKill;  // 0x08
    int32 nQuestKillMonsterCount;  // 0x0c
    SNO snoQuestBonus1;  // 0x10
    BOOL bQuestBonus1Success;  // 0x14
    SNO snoQuestBonus2;  // 0x18
    BOOL bQuestBonus2Success;  // 0x1c
    SNO snoQuestMastery;  // 0x20
    BOOL bQuestMasterySuccess;  // 0x24
    BOOL bQuestKillSuccess;  // 0x28
    BOOL bShowTotalTime;  // 0x2c
    int32 nTimeTaken;  // 0x30
    int32 nTargetTime;  // 0x34
};
static_assert(sizeof(SetDungeonResultsMessage) == 0x38, "SetDungeonResultsMessage");

#line 1280 "X/Common/Networking/GameMessage.h"
enum { AFFIX_MESSAGE_FLAG_INVALID = 0, AFFIX_MESSAGE_FLAG_IDENTIFIED = 1, AFFIX_MESSAGE_FLAG_UNIDENTIFIED = 2, AFFIX_MESSAGE_FLAG_COUNT = 3 };
