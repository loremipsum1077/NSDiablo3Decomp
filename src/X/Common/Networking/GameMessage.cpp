// Original file: X/Common/Networking/GameMessage.cpp

#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "GameMessage.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_WORLD_PLACE;  // X/Common/WorldPlace.cpp:21
extern const XTypeDescriptor * DT_GBHANDLE;  // X/Common/Definitions/GameBalanceDefinition.cpp:126

// The message type descriptors. The tables below were generated (build/try/agentD/gen_cpp.py)
// from the original static initializer's constructor calls and the DWARF struct layouts.
// The field names are "" in the binary unless shown.

static const TypeDescriptorField RequiredMessageHeader_Fields[3] = {
    BoundedTypeDescriptorField("", DT_INT, 4, NULL, NULL, (const void *)0x253),
    TypeDescriptorField("", DT_BYTE, 2, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 8, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RequiredMessageHeader("RequiredMessageHeader", 0, RequiredMessageHeader_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER = &TypeDescriptor_RequiredMessageHeader;

static const TypeDescriptorField SimpleMessage_Fields[2] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SimpleMessage, tHeader), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SimpleMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SimpleMessage("SimpleMessage", 0, SimpleMessage_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SIMPLE_MESSAGE = &TypeDescriptor_SimpleMessage;

static const TypeDescriptorField GenericBlobMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GenericBlobMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GenericBlobMessage, uBlobSize), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GenericBlobMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GenericBlobMessage("GenericBlobMessage", 0, GenericBlobMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GENERIC_BLOB_MESSAGE = &TypeDescriptor_GenericBlobMessage;

static const TypeDescriptorField ChunkMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ChunkMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ChunkMessage, unk_08), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ChunkMessage, unk_0c), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ChunkMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ChunkMessage("ChunkMessage", 0, ChunkMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CHUNK_MESSAGE = &TypeDescriptor_ChunkMessage;

static const TypeDescriptorField BoolDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(BoolDataMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(BoolDataMessage, bVal), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BoolDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BoolDataMessage("BoolDataMessage", 0, BoolDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BOOL_DATA_MESSAGE = &TypeDescriptor_BoolDataMessage;
extern const int32 MAX_FADE_TIME = 1800;  // line 52
static const TypeDescriptorField ByteDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ByteDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(ByteDataMessage, bData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ByteDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ByteDataMessage("ByteDataMessage", 0, ByteDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BYTE_DATA_MESSAGE = &TypeDescriptor_ByteDataMessage;

static const TypeDescriptorField PlayerIndexMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerIndexMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerIndexMessage, nPlayerIndex), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerIndexMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerIndexMessage("PlayerIndexMessage", 0, PlayerIndexMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_INDEX_MESSAGE = &TypeDescriptor_PlayerIndexMessage;

static const TypeDescriptorField DataIDDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DataIDDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DataIDDataMessage, id), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DataIDDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DataIDDataMessage("DataIDDataMessage", 0, DataIDDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DATAID_DATA_MESSAGE = &TypeDescriptor_DataIDDataMessage;

static const TypeDescriptorField DWordDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DWordDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DWordDataMessage, dwData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DWordDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DWordDataMessage("DWordDataMessage", 0, DWordDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DWORD_DATA_MESSAGE = &TypeDescriptor_DWordDataMessage;

static const TypeDescriptorField PlayerDWordDataMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerDWordDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerDWordDataMessage, dwData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerDWordDataMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerDWordDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerDWordDataMessage("PlayerDWordDataMessage", 0, PlayerDWordDataMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_DWORD_DATA_MESSAGE = &TypeDescriptor_PlayerDWordDataMessage;

static const TypeDescriptorField PlayerDWord2DataMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerDWord2DataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerDWord2DataMessage, unk_08), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerDWord2DataMessage, unk_0c), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerDWord2DataMessage, unk_10), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerDWord2DataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerDWord2DataMessage("PlayerDWord2DataMessage", 0, PlayerDWord2DataMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_DWORD2_DATA_MESSAGE = &TypeDescriptor_PlayerDWord2DataMessage;

static const TypeDescriptorField IntDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(IntDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(IntDataMessage, nData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(IntDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_IntDataMessage("IntDataMessage", 0, IntDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INT_DATA_MESSAGE = &TypeDescriptor_IntDataMessage;

static const TypeDescriptorField Int64DataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(Int64DataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(Int64DataMessage, nData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Int64DataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Int64DataMessage("Int64DataMessage", 0, Int64DataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INT64_DATA_MESSAGE = &TypeDescriptor_Int64DataMessage;

static const TypeDescriptorField UInt64DataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(UInt64DataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(UInt64DataMessage, uData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UInt64DataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UInt64DataMessage("UInt64DataMessage", 0, UInt64DataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_UINT64_DATA_MESSAGE = &TypeDescriptor_UInt64DataMessage;

static const TypeDescriptorField FloatDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(FloatDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(FloatDataMessage, flData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FloatDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FloatDataMessage("FloatDataMessage", 0, FloatDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_FLOAT_DATA_MESSAGE = &TypeDescriptor_FloatDataMessage;

static const TypeDescriptorField SNODataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SNODataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(SNODataMessage, snoData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SNODataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SNODataMessage("SNODataMessage", 0, SNODataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SNO_DATA_MESSAGE = &TypeDescriptor_SNODataMessage;

static const TypeDescriptorField SNONameDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SNONameDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(SNONameDataMessage, snoname), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SNONameDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SNONameDataMessage("SNONameDataMessage", 0, SNONameDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SNONAME_DATA_MESSAGE = &TypeDescriptor_SNONameDataMessage;

static const TypeDescriptorField PreloadACDDataMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PreloadACDDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PreloadACDDataMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(PreloadACDDataMessage, snoActor), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PreloadACDDataMessage, eWeaponClass), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(PreloadACDDataMessage, gbidMonsterAffixes), NULL, 1, 8, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PreloadACDDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PreloadACDDataMessage("PreloadACDDataMessage", 0, PreloadACDDataMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PRELOAD_ACD_DATA_MESSAGE = &TypeDescriptor_PreloadACDDataMessage;

static const TypeDescriptorField PrefetchActorDataMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PrefetchActorDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(PrefetchActorDataMessage, snoActor), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(PrefetchActorDataMessage, gbidMonsterAffixes), NULL, 1, 8, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PrefetchActorDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PrefetchActorDataMessage("PrefetchActorDataMessage", 0, PrefetchActorDataMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PREFETCH_ACTOR_DATA_MESSAGE = &TypeDescriptor_PrefetchActorDataMessage;

static const TypeDescriptorField GBIDDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GBIDDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_GBID, offsetof(GBIDDataMessage, gbidData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GBIDDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GBIDDataMessage("GBIDDataMessage", 0, GBIDDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GBID_DATA_MESSAGE = &TypeDescriptor_GBIDDataMessage;

static const TypeDescriptorField DisplayGameTextMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DisplayGameTextMessage, tHeader), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(DisplayGameTextMessage, szMsg), "", 1, 0x400, -1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(DisplayGameTextMessage, nParam1), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(DisplayGameTextMessage, nParam2), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DisplayGameTextMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DisplayGameTextMessage("DisplayGameTextMessage", 0, DisplayGameTextMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DISPLAY_GAME_TEXT_MESSAGE = &TypeDescriptor_DisplayGameTextMessage;

static const TypeDescriptorField BroadcastTextMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(BroadcastTextMessage, tHeader), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(BroadcastTextMessage, szMsg), "", 1, 0x400, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BroadcastTextMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BroadcastTextMessage("BroadcastTextMessage", 0, BroadcastTextMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BROADCAST_TEXT_MESSAGE = &TypeDescriptor_BroadcastTextMessage;

static const TypeDescriptorField ANNDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ANNDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ANNDataMessage, ann), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ANNDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ANNDataMessage("ANNDataMessage", 0, ANNDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ANN_DATA_MESSAGE = &TypeDescriptor_ANNDataMessage;

static const TypeDescriptorField JoinLANGameMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(JoinLANGameMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(JoinLANGameMessage, idSGame), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(JoinLANGameMessage, szAccount), "", 1, 0x80, -1),
    CharArrayTypeDescriptorField("", offsetof(JoinLANGameMessage, szHero), "", 1, 0x31, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(JoinLANGameMessage, eXLocale), NULL, (const void *)0x2, (const void *)0x12),
    TypeDescriptorField(NULL, DT_NULL, sizeof(JoinLANGameMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_JoinLANGameMessage("JoinLANGameMessage", 0, JoinLANGameMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_JOIN_LAN_GAME_MESSAGE = &TypeDescriptor_JoinLANGameMessage;

static const TypeDescriptorField JoinConsoleGameMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(JoinConsoleGameMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(JoinConsoleGameMessage, eXLocale), NULL, (const void *)0x2, (const void *)0x12),
    TypeDescriptorField("", DT_BYTE, offsetof(JoinConsoleGameMessage, uControllerIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(JoinConsoleGameMessage, uLicenses), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(JoinConsoleGameMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_JoinConsoleGameMessage("JoinConsoleGameMessage", 0, JoinConsoleGameMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_JOIN_CONSOLE_GAME_MESSAGE = &TypeDescriptor_JoinConsoleGameMessage;

static const TypeDescriptorField QuitGameMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(QuitGameMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(QuitGameMessage, eReason), NULL, NULL, (const void *)0xd),
    TypeDescriptorField(NULL, DT_NULL, sizeof(QuitGameMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_QuitGameMessage("QuitGameMessage", 0, QuitGameMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_QUIT_GAME_MESSAGE = &TypeDescriptor_QuitGameMessage;

static const TypeDescriptorField ConnectionEstablishedMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ConnectionEstablishedMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ConnectionEstablishedMessage, nPlayerIndex), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(ConnectionEstablishedMessage, dwAnimSyncedSeed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConnectionEstablishedMessage, dwSnoCoreDataHash), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConnectionEstablishedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConnectionEstablishedMessage("ConnectionEstablishedMessage", 0, ConnectionEstablishedMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CONNECTION_ESTABLISHED_MESSAGE = &TypeDescriptor_ConnectionEstablishedMessage;

static const TypeDescriptorField GameSetupMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GameSetupMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSetupMessage, dwFirstHeartbeat), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameSetupMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GameSetupMessage("GameSetupMessage", 0, GameSetupMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GAME_SETUP_MESSAGE = &TypeDescriptor_GameSetupMessage;

static const TypeDescriptorField EnterWorldMessage_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(EnterWorldMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(EnterWorldMessage, wpLocation), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EnterWorldMessage, idSWorld), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(EnterWorldMessage, snoWorld), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EnterWorldMessage, nPlayerIndex), NULL, 1),
    OptionalTypeDescriptorField("", DT_ENTER_KNOWN_LOOK_OVERRIDES, offsetof(EnterWorldMessage, tEnterKnownLookOverrides), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EnterWorldMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EnterWorldMessage("EnterWorldMessage", 0, EnterWorldMessage_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ENTER_WORLD_MESSAGE = &TypeDescriptor_EnterWorldMessage;

static const TypeDescriptorField RevealWorldMessage_Fields[11] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RevealWorldMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealWorldMessage, idSWorld), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(RevealWorldMessage, snoWorld), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealWorldMessage, nOriginX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealWorldMessage, nOriginY), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealWorldMessage, nStitchSizeInFeetX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealWorldMessage, nStitchSizeInFeetY), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealWorldMessage, nWorldSizeInFeetX), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RevealWorldMessage, nWorldSizeInFeetY), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(RevealWorldMessage, snoDungeonFinderSourceWorld), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RevealWorldMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RevealWorldMessage("RevealWorldMessage", 0, RevealWorldMessage_Fields, 11, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_REVEAL_WORLD_MESSAGE = &TypeDescriptor_RevealWorldMessage;

static const TypeDescriptorField WorldLocationMessageData_Fields[4] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(WorldLocationMessageData, flScale), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(WorldLocationMessageData, transform), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WorldLocationMessageData, idSWorld), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WorldLocationMessageData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldLocationMessageData("WorldLocationMessageData", 0, WorldLocationMessageData_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_WORLD_LOCATION_MESSAGE_DATA = &TypeDescriptor_WorldLocationMessageData;

static const TypeDescriptorField InventoryLocationMessageData_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(InventoryLocationMessageData, annContainer), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(InventoryLocationMessageData, eSlot), NULL, (const void *)-1, (const void *)0x19),
    TypeDescriptorField("", DT_IVECTOR2D, offsetof(InventoryLocationMessageData, pt), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InventoryLocationMessageData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventoryLocationMessageData("InventoryLocationMessageData", 0, InventoryLocationMessageData_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INVENTORY_LOCATION_MESSAGE_DATA = &TypeDescriptor_InventoryLocationMessageData;

static const TypeDescriptorField ACDEnterKnownMessage_Fields[18] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDEnterKnownMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ACDEnterKnownMessage, snoActor), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, dwFlags), NULL, NULL, (const void *)0x7f),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, eLocationType), NULL, (const void *)-1, (const void *)0x1),
    OptionalTypeDescriptorField("", DT_WORLD_LOCATION_MESSAGE_DATA, offsetof(ACDEnterKnownMessage, tWorldLocationData), NULL, 1),
    OptionalTypeDescriptorField("", DT_INVENTORY_LOCATION_MESSAGE_DATA, offsetof(ACDEnterKnownMessage, tInvLocationData), NULL, 1),
    TypeDescriptorField("", DT_GBHANDLE, offsetof(ACDEnterKnownMessage, hGB), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, tSNOName), NULL, 1),
    TypeDescriptorField("", DT_INT, 0x60, NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, nRarity), NULL, (const void *)-1, (const void *)0xa),
    TypeDescriptorField("", DT_BYTE, offsetof(ACDEnterKnownMessage, nLookLinkIndex), NULL, 1),
    OptionalTypeDescriptorField("", DT_SNO, offsetof(ACDEnterKnownMessage, snoAmbientOcclusionOverrideTex), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, snoMarkerSet), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, nMarkerIndex), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(ACDEnterKnownMessage, dwTimeActorCreated), NULL, 1),
    OptionalTypeDescriptorField("", DT_ENTER_KNOWN_LOOK_OVERRIDES, offsetof(ACDEnterKnownMessage, tEnterKnownLookOverrides), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDEnterKnownMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDEnterKnownMessage("ACDEnterKnownMessage", 0, ACDEnterKnownMessage_Fields, 18, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_ENTER_KNOWN_MESSAGE = &TypeDescriptor_ACDEnterKnownMessage;

static const TypeDescriptorField EnterKnownLookOverrides_Fields[2] = {
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(EnterKnownLookOverrides, aLookOverrides), NULL, 1, 6, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EnterKnownLookOverrides), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EnterKnownLookOverrides("EnterKnownLookOverrides", 0, EnterKnownLookOverrides_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ENTER_KNOWN_LOOK_OVERRIDES = &TypeDescriptor_EnterKnownLookOverrides;

static const TypeDescriptorField PlayerEnterKnownMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerEnterKnownMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerEnterKnownMessage, nPlayerIndex), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(PlayerEnterKnownMessage, ann), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerEnterKnownMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerEnterKnownMessage("PlayerEnterKnownMessage", 0, PlayerEnterKnownMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_ENTER_KNOWN_MESSAGE = &TypeDescriptor_PlayerEnterKnownMessage;

static const TypeDescriptorField ACDInventoryPositionMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDInventoryPositionMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDInventoryPositionMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INVENTORY_LOCATION_MESSAGE_DATA, offsetof(ACDInventoryPositionMessage, tInvLocationData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDInventoryPositionMessage, tLocType), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDInventoryPositionMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDInventoryPositionMessage("ACDInventoryPositionMessage", 0, ACDInventoryPositionMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_INVENTORY_POSITION_MESSAGE = &TypeDescriptor_ACDInventoryPositionMessage;

static const TypeDescriptorField ACDInventoryUpdateActorSNO_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDInventoryUpdateActorSNO, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDInventoryUpdateActorSNO, ann), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ACDInventoryUpdateActorSNO, snoActor), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDInventoryUpdateActorSNO), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDInventoryUpdateActorSNO("ACDInventoryUpdateActorSNO", 0, ACDInventoryUpdateActorSNO_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_INVENTORY_UPDATE_ACTOR_SNO = &TypeDescriptor_ACDInventoryUpdateActorSNO;

static const TypeDescriptorField ACDWorldPositionMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDWorldPositionMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDWorldPositionMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_WORLD_LOCATION_MESSAGE_DATA, offsetof(ACDWorldPositionMessage, tWorldLocationData), NULL, 1),
    OptionalTypeDescriptorField("", DT_ENTER_KNOWN_LOOK_OVERRIDES, offsetof(ACDWorldPositionMessage, tLookOverrides), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDWorldPositionMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDWorldPositionMessage("ACDWorldPositionMessage", 0, ACDWorldPositionMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_WORLD_POSITION_MESSAGE = &TypeDescriptor_ACDWorldPositionMessage;

static const TypeDescriptorField ACDShearMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDShearMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDShearMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ACDShearMessage, flShear), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDShearMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDShearMessage("ACDShearMessage", 0, ACDShearMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_SHEAR_MESSAGE = &TypeDescriptor_ACDShearMessage;

static const TypeDescriptorField ACDGroupMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDGroupMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDGroupMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDGroupMessage, uPrimaryGroup), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDGroupMessage, uSecondaryGroup), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDGroupMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDGroupMessage("ACDGroupMessage", 0, ACDGroupMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_GROUP_MESSAGE = &TypeDescriptor_ACDGroupMessage;

static const TypeDescriptorField ACDChangeActorMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDChangeActorMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDChangeActorMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ACDChangeActorMessage, snoActor), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDChangeActorMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDChangeActorMessage("ACDChangeActorMessage", 0, ACDChangeActorMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_CHANGE_ACTOR_MESSAGE = &TypeDescriptor_ACDChangeActorMessage;

static const TypeDescriptorField ACDPickupFailedMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDPickupFailedMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDPickupFailedMessage, bIsCurrency), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDPickupFailedMessage, ePickupResponse), NULL, NULL, (const void *)0x7),
    TypeDescriptorField("", DT_INT, offsetof(ACDPickupFailedMessage, annPickee), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDPickupFailedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDPickupFailedMessage("ACDPickupFailedMessage", 0, ACDPickupFailedMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_PICKUP_FAILED_MESSAGE = &TypeDescriptor_ACDPickupFailedMessage;

static const TypeDescriptorField AffixMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(AffixMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AffixMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(AffixMessage, eAffixArray), NULL, NULL, (const void *)0x2),
    FixedArrayTypeDescriptorField("aAffixGBIDs", DT_GBID, offsetof(AffixMessage, aAffixGBIDs), NULL, 8, 0x20, 0x80),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AffixMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AffixMessage("AffixMessage", 0, AffixMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_AFFIX_MESSAGE = &TypeDescriptor_AffixMessage;

static const TypeDescriptorField ProjectileStickMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ProjectileStickMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ProjectileStickMessage, wpSourceLoc), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ProjectileStickMessage, annTarget), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ProjectileStickMessage, snoActorAttachment), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ProjectileStickMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ProjectileStickMessage("ProjectileStickMessage", 0, ProjectileStickMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PROJECTILE_STICK_MESSAGE = &TypeDescriptor_ProjectileStickMessage;

static const TypeDescriptorField PlayerActorSetInitialMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerActorSetInitialMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerActorSetInitialMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerActorSetInitialMessage, nPlayerIndex), NULL, NULL, (const void *)0x3),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerActorSetInitialMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerActorSetInitialMessage("PlayerActorSetInitialMessage", 0, PlayerActorSetInitialMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_ACTOR_SET_INITIAL_MESSAGE = &TypeDescriptor_PlayerActorSetInitialMessage;

static const TypeDescriptorField AnimPreplayData_Fields[4] = {
    TypeDescriptorField("", DT_INT, offsetof(AnimPreplayData, dwAnimPrePlayDataFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimPreplayData, dwServerTimeAnimStarted), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimPreplayData, dwSyncedSeed), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimPreplayData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimPreplayData("AnimPreplayData", 0, AnimPreplayData_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ANIM_PREPLAY_DATA = &TypeDescriptor_AnimPreplayData;

static const TypeDescriptorField TargetMessage_Fields[11] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TargetMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TargetMessage, eType), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(TargetMessage, annTarget), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(TargetMessage, tPlace), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(TargetMessage, snoPower), "", POWER_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(TargetMessage, annItemBeingUsed), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TargetMessage, nComboLevel), NULL, NULL, (const void *)0x2),
    OptionalTypeDescriptorField("", DT_ANIM_PREPLAY_DATA, offsetof(TargetMessage, tAnimPreplayData), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TargetMessage, dwFlags), NULL, 1),
    OptionalTypeDescriptorField("", DT_FLOAT, offsetof(TargetMessage, flBattlePayClickStats), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TargetMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TargetMessage("TargetMessage", 0, TargetMessage_Fields, 11, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TARGET_MESSAGE = &TypeDescriptor_TargetMessage;

static const TypeDescriptorField SecondaryAnimationPowerMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SecondaryAnimationPowerMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SecondaryAnimationPowerMessage, snoPower), "", POWER_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(SecondaryAnimationPowerMessage, annTarget), NULL, 1),
    OptionalTypeDescriptorField("", DT_ANIM_PREPLAY_DATA, offsetof(SecondaryAnimationPowerMessage, tAnimPreplayData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SecondaryAnimationPowerMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SecondaryAnimationPowerMessage("SecondaryAnimationPowerMessage", 0, SecondaryAnimationPowerMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SECONDARY_ANIMATION_POWER_MESSAGE = &TypeDescriptor_SecondaryAnimationPowerMessage;

static const TypeDescriptorField LoopingAnimationPowerMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(LoopingAnimationPowerMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LoopingAnimationPowerMessage, snoPower), "", POWER_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LoopingAnimationPowerMessage, snoData0), "", POWER_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(LoopingAnimationPowerMessage, nData1), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LoopingAnimationPowerMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LoopingAnimationPowerMessage("LoopingAnimationPowerMessage", 0, LoopingAnimationPowerMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LOOPING_ANIMATION_POWER_MESSAGE = &TypeDescriptor_LoopingAnimationPowerMessage;

static const TypeDescriptorField TryConsoleCommand_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TryConsoleCommand, tHeader), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(TryConsoleCommand, usMessage), "", 1, 0x400, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TryConsoleCommand, nPlayerIndex), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(TryConsoleCommand, tPlaceTarget), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TryConsoleCommand, annTarget), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TryConsoleCommand, dwFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TryConsoleCommand), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TryConsoleCommand("TryConsoleCommand", 0, TryConsoleCommand_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRY_CONSOLE_COMMAND = &TypeDescriptor_TryConsoleCommand;

static const TypeDescriptorField TryChatMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TryChatMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TryChatMessage, eDestination), NULL, NULL, (const void *)0x6),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TryChatMessage, nPlayerTarget), NULL, (const void *)-1, (const void *)0x3),
    CharArrayTypeDescriptorField("", offsetof(TryChatMessage, usMessage), "", 1, 0x400, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TryChatMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TryChatMessage("TryChatMessage", 0, TryChatMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRY_CHAT_MESSAGE = &TypeDescriptor_TryChatMessage;

static const TypeDescriptorField ChatMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ChatMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ChatMessage, eColor), NULL, NULL, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ChatMessage, nPlayerIndex), NULL, (const void *)-1, (const void *)0x3),
    CharArrayTypeDescriptorField("", offsetof(ChatMessage, usMessage), "", 1, 0x400, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ChatMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ChatMessage("ChatMessage", 0, ChatMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CHAT_MESSAGE = &TypeDescriptor_ChatMessage;

static const TypeDescriptorField VoteKickMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(VoteKickMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VoteKickMessage, nPlayerIndexToKick), NULL, (const void *)-1, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VoteKickMessage, nPlayerIndexInitiating), NULL, (const void *)-1, (const void *)0x3),
    CharArrayTypeDescriptorField("", offsetof(VoteKickMessage, usMessage), "", 1, 0x400, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VoteKickMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_VoteKickMessage("VoteKickMessage", 0, VoteKickMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_VOTE_KICK_MESSAGE = &TypeDescriptor_VoteKickMessage;

static const TypeDescriptorField VictimMessage_Fields[10] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(VictimMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VictimMessage, nPlayerVictimIndex), NULL, NULL, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VictimMessage, nKillerLevel), NULL, NULL, (const void *)0x64),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VictimMessage, nKillerPlayerIndex), NULL, (const void *)-1, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VictimMessage, eKillerMonsterRarity), NULL, (const void *)-1, (const void *)0xb),
    FilterTypeDescriptorField("", DT_SNO, offsetof(VictimMessage, snoKillerActor), "", ACTOR_SNO, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VictimMessage, eKillerTeam), NULL, (const void *)-1, (const void *)0x17),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(VictimMessage, pKillerRareNameGBIDs), NULL, 1, 2, 0),
    FilterTypeDescriptorField("", DT_SNO, offsetof(VictimMessage, snoPowerDmgSource), "", POWER_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VictimMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_VictimMessage("VictimMessage", 0, VictimMessage_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_VICTIM_MESSAGE = &TypeDescriptor_VictimMessage;

static const TypeDescriptorField LootRunAppearedMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(LootRunAppearedMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LootRunAppearedMessage, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LootRunAppearedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LootRunAppearedMessage("LootRunAppearedMessage", 0, LootRunAppearedMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LOOTRUN_APPEARED_MESSAGE = &TypeDescriptor_LootRunAppearedMessage;

static const TypeDescriptorField CombatEngagementMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CombatEngagementMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CombatEngagementMessage, nPlayerIndex), NULL, (const void *)-1, (const void *)0x3),
    FilterTypeDescriptorField("", DT_SNO, offsetof(CombatEngagementMessage, snoActorMonster), "", ACTOR_SNO, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CombatEngagementMessage, eMonsterRarity), NULL, (const void *)-1, (const void *)0xb),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(CombatEngagementMessage, pRareNameGBIDs), NULL, 1, 2, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CombatEngagementMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CombatEngagementMessage("CombatEngagementMessage", 0, CombatEngagementMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_COMBAT_ENGAGEMENT_MESSAGE = &TypeDescriptor_CombatEngagementMessage;

static const TypeDescriptorField DuelResultMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DuelResultMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(DuelResultMessage, nPlayerVictimIndex), NULL, NULL, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(DuelResultMessage, nPlayerKillerIndex), NULL, (const void *)-1, (const void *)0x3),
    FilterTypeDescriptorField("", DT_SNO, offsetof(DuelResultMessage, snoPowerDmgSource), "", POWER_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DuelResultMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DuelResultMessage("DuelResultMessage", 0, DuelResultMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DUEL_RESULT_MESSAGE = &TypeDescriptor_DuelResultMessage;

static const TypeDescriptorField KillCountMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(KillCountMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(KillCountMessage, nPlayerIndex), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(KillCountMessage, nPlayerKills), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(KillCountMessage, nDeaths), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(KillCountMessage, nAssists), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(KillCountMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_KillCountMessage("KillCountMessage", 0, KillCountMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_KILL_COUNT_MESSAGE = &TypeDescriptor_KillCountMessage;

static const TypeDescriptorField InvLoc_Fields[5] = {
    TypeDescriptorField("", DT_INT, 0, NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, 4, NULL, (const void *)-1, (const void *)0x19),
    TypeDescriptorField("", DT_INT, 8, NULL, 1),
    TypeDescriptorField("", DT_INT, 0xc, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 0x10, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InvLoc("InvLoc", 0, InvLoc_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INV_LOC = &TypeDescriptor_InvLoc;

static const TypeDescriptorField InventoryRequestQuickMoveMessage_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, 0, NULL, 1),
    TypeDescriptorField("", DT_INT, 8, NULL, 1),
    TypeDescriptorField("", DT_INT, 0xc, NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, 0x10, NULL, (const void *)-1, (const void *)0x19),
    TypeDescriptorField("", DT_INT, 0x14, NULL, 1),
    TypeDescriptorField("", DT_INT, 0x18, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 0x1c, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventoryRequestQuickMoveMessage("InventoryRequestQuickMoveMessage", 0, InventoryRequestQuickMoveMessage_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INVENTORY_REQUEST_QUICK_MOVE_MESSAGE = &TypeDescriptor_InventoryRequestQuickMoveMessage;

static const TypeDescriptorField InventoryRequestMoveMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, 0, NULL, 1),
    TypeDescriptorField("", DT_INT, 8, NULL, 1),
    TypeDescriptorField("", DT_INV_LOC, 0xc, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 0x1c, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventoryRequestMoveMessage("InventoryRequestMoveMessage", 0, InventoryRequestMoveMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INVENTORY_REQUEST_MOVE_MESSAGE = &TypeDescriptor_InventoryRequestMoveMessage;

static const TypeDescriptorField ChangeUsableItemMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ChangeUsableItemMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ChangeUsableItemMessage, annItem), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ChangeUsableItemMessage, gbidItem), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ChangeUsableItemMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ChangeUsableItemMessage("ChangeUsableItemMessage", 0, ChangeUsableItemMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CHANGE_USABLE_ITEM_MESSAGE = &TypeDescriptor_ChangeUsableItemMessage;

static const TypeDescriptorField StackPortionMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(StackPortionMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(StackPortionMessage, annItem), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(StackPortionMessage, uAmount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(StackPortionMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_StackPortionMessage("StackPortionMessage", 0, StackPortionMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_STACK_PORTION_MESSAGE = &TypeDescriptor_StackPortionMessage;

static const TypeDescriptorField CurrencyMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CurrencyMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CurrencyMessage, eCurrency), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(CurrencyMessage, uAmount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CurrencyMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CurrencyMessage("CurrencyMessage", 0, CurrencyMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CURRENCY_MESSAGE = &TypeDescriptor_CurrencyMessage;

static const TypeDescriptorField InventorySplitStackMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, 0, NULL, 1),
    TypeDescriptorField("", DT_INT, 8, NULL, 1),
    TypeDescriptorField("", DT_INT64, 0x10, NULL, 1),
    TypeDescriptorField("", DT_INV_LOC, 0x18, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 0x28, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventorySplitStackMessage("InventorySplitStackMessage", 0, InventorySplitStackMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INVENTORY_SPLIT_STACK_MESSAGE = &TypeDescriptor_InventorySplitStackMessage;

static const TypeDescriptorField InventoryStackTransferMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(InventoryStackTransferMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(InventoryStackTransferMessage, annItemSrc), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(InventoryStackTransferMessage, annItemDest), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(InventoryStackTransferMessage, uAmount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InventoryStackTransferMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventoryStackTransferMessage("InventoryStackTransferMessage", 0, InventoryStackTransferMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INVENTORY_STACK_TRANSFER_MESSAGE = &TypeDescriptor_InventoryStackTransferMessage;

static const TypeDescriptorField GoldTransferMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GoldTransferMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GoldTransferMessage, annPlayer), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(GoldTransferMessage, uAmount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GoldTransferMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GoldTransferMessage("GoldTransferMessage", 0, GoldTransferMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GOLD_TRANSFER_MESSAGE = &TypeDescriptor_GoldTransferMessage;

static const TypeDescriptorField InventoryRequestUseMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(InventoryRequestUseMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(InventoryRequestUseMessage, annItemToUse), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(InventoryRequestUseMessage, eType), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(InventoryRequestUseMessage, annTarget), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(InventoryRequestUseMessage, tPlace), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InventoryRequestUseMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventoryRequestUseMessage("InventoryRequestUseMessage", 0, InventoryRequestUseMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INVENTORY_REQUEST_USE_MESSAGE = &TypeDescriptor_InventoryRequestUseMessage;

static const TypeDescriptorField InventoryRequestSocketMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(InventoryRequestSocketMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(InventoryRequestSocketMessage, annGem), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(InventoryRequestSocketMessage, annItemToReceiveGem), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InventoryRequestSocketMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InventoryRequestSocketMessage("InventoryRequestSocketMessage", 0, InventoryRequestSocketMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INVENTORY_REQUEST_SOCKET_MESSAGE = &TypeDescriptor_InventoryRequestSocketMessage;

static const TypeDescriptorField HelperDetachMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HelperDetachMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HelperDetachMessage, annHelper), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HelperDetachMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HelperDetachMessage("HelperDetachMessage", 0, HelperDetachMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HELPER_DETACH_MESSAGE = &TypeDescriptor_HelperDetachMessage;

static const TypeDescriptorField PetMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PetMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PetMessage, nOwner), NULL, NULL, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PetMessage, nIndex), NULL, NULL, (const void *)0x3f),
    TypeDescriptorField("", DT_INT, offsetof(PetMessage, annPet), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PetMessage, nType), NULL, (const void *)-1, (const void *)0x4c),  // 2.7.7: 77 pet types
    TypeDescriptorField(NULL, DT_NULL, sizeof(PetMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PetMessage("PetMessage", 0, PetMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PET_MESSAGE = &TypeDescriptor_PetMessage;

static const TypeDescriptorField PetDetachMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PetDetachMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PetDetachMessage, annPet), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PetDetachMessage, bDisplayChatMessage), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PetDetachMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PetDetachMessage("PetDetachMessage", 0, PetDetachMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PET_DETACH_MESSAGE = &TypeDescriptor_PetDetachMessage;

static const TypeDescriptorField FlippyMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(FlippyMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FlippyMessage, annLauncher), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(FlippyMessage, snoFlippyParticle), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(FlippyMessage, snoActorLoot), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(FlippyMessage, wpDest), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FlippyMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FlippyMessage("FlippyMessage", 0, FlippyMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_FLIPPY_MESSAGE = &TypeDescriptor_FlippyMessage;

static const TypeDescriptorField ComplexEffectAddMessage_Fields[10] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ComplexEffectAddMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ComplexEffectAddMessage, idSComplexEffect), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ComplexEffectAddMessage, eType), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(ComplexEffectAddMessage, snoEffectGroup), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ComplexEffectAddMessage, annSource), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ComplexEffectAddMessage, annDest), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ComplexEffectAddMessage, dwParam1), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ComplexEffectAddMessage, dwParam2), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ComplexEffectAddMessage, bIgnoreOwnerAlpha), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ComplexEffectAddMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ComplexEffectAddMessage("ComplexEffectAddMessage", 0, ComplexEffectAddMessage_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_COMPLEX_EFFECT_ADD_MESSAGE = &TypeDescriptor_ComplexEffectAddMessage;

static const TypeDescriptorField PlayerLevel_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerLevel, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerLevel, nPlayerIndex), NULL, NULL, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerLevel, nNewLevel), NULL, NULL, (const void *)0x4e20),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerLevel), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerLevel("PlayerLevel", 0, PlayerLevel_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_LEVEL = &TypeDescriptor_PlayerLevel;

static const TypeDescriptorField AimTargetMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(AimTargetMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AimTargetMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(AimTargetMessage, eType), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(AimTargetMessage, annTarget), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(AimTargetMessage, tPlace), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AimTargetMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AimTargetMessage("AimTargetMessage", 0, AimTargetMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_AIM_TARGET_MESSAGE = &TypeDescriptor_AimTargetMessage;

static const TypeDescriptorField ACDChangeGBHandleMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDChangeGBHandleMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDChangeGBHandleMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_GBHANDLE, offsetof(ACDChangeGBHandleMessage, hGB), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDChangeGBHandleMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDChangeGBHandleMessage("ACDChangeGBHandleMessage", 0, ACDChangeGBHandleMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_CHANGE_GBHANDLE_MESSAGE = &TypeDescriptor_ACDChangeGBHandleMessage;

static const TypeDescriptorField TrickleMessage_Fields[26] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TrickleMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TrickleMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(TrickleMessage, snoActor), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(TrickleMessage, tPlace), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, nPlayerIndexOwner), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField("", DT_SNO, offsetof(TrickleMessage, snoLevelArea), NULL, 1),
    OptionalTypeDescriptorField("", DT_FLOAT, offsetof(TrickleMessage, flHealthPercent), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, eTrickleInfoType), NULL, NULL, (const void *)0xe),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, dwTrickleFlags), NULL, NULL, (const void *)0x3ff),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, dwHeadstoneCorpseReviveTime), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, snoMinimapIcon), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, hMinimapStringLabel), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, hMinimapIconLabel), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, snoStringList), NULL, 1),
    OptionalTypeDescriptorField("", DT_FLOAT, offsetof(TrickleMessage, fMinDisplayRangeSq), NULL, 1),
    OptionalTypeDescriptorField("", DT_FLOAT, offsetof(TrickleMessage, fMaxDisplayRangeSq), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, nAction), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, uEngagedWithRareTime), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, uEngagedWithGoblinTime), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, bInCombat), NULL, NULL, (const void *)0x1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, nTieredRiftRank), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, bBrawling), NULL, NULL, (const void *)0x1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, bInUberFight), NULL, NULL, (const void *)0x1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, gbidPortraitFrame), NULL, 1),
    OptionalBoundedTypeDescriptorField("", DT_INT, offsetof(TrickleMessage, unk_b4), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TrickleMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TrickleMessage("TrickleMessage", 0, TrickleMessage_Fields, 26, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRICKLE_MESSAGE = &TypeDescriptor_TrickleMessage;

static const TypeDescriptorField PlayerIntValMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerIntValMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerIntValMessage, nPlayerIndex), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(PlayerIntValMessage, nVal), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerIntValMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerIntValMessage("PlayerIntValMessage", 0, PlayerIntValMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_INTVAL_MESSAGE = &TypeDescriptor_PlayerIntValMessage;

static const TypeDescriptorField UIElementMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(UIElementMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(UIElementMessage, eElement), NULL, (const void *)-1, NULL),
    BoundedTypeDescriptorField("", DT_INT, offsetof(UIElementMessage, eAction), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIElementMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIElementMessage("UIElementMessage", 0, UIElementMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_UI_ELEMENT_MESSAGE = &TypeDescriptor_UIElementMessage;

static const TypeDescriptorField PlasmaAttachMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlasmaAttachMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlasmaAttachMessage, annPlasma), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlasmaAttachMessage, annAttachTo), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlasmaAttachMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlasmaAttachMessage("PlasmaAttachMessage", 0, PlasmaAttachMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLASMA_ATTACH_MESSAGE = &TypeDescriptor_PlasmaAttachMessage;

static const TypeDescriptorField RitualTetherEffectMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RitualTetherEffectMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RitualTetherEffectMessage, annFrom), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(RitualTetherEffectMessage, tPlaceDest), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RitualTetherEffectMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RitualTetherEffectMessage("RitualTetherEffectMessage", 0, RitualTetherEffectMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_RITUAL_TETHER_EFFECT_MESSAGE = &TypeDescriptor_RitualTetherEffectMessage;

static const TypeDescriptorField RevealTeamMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RevealTeamMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RevealTeamMessage, eTeam), NULL, (const void *)-1, (const void *)0x16),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RevealTeamMessage, dwTeamFlags), NULL, NULL, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RevealTeamMessage, eTeamColoring), NULL, (const void *)-1, (const void *)0x2),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RevealTeamMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RevealTeamMessage("RevealTeamMessage", 0, RevealTeamMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_REVEAL_TEAM_MESSAGE = &TypeDescriptor_RevealTeamMessage;

static const TypeDescriptorField DeathFadeTimeMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DeathFadeTimeMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DeathFadeTimeMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(DeathFadeTimeMessage, nTimeUntilFade), NULL, (const void *)-1, (const void *)0x708),
    BoundedTypeDescriptorField("", DT_INT, offsetof(DeathFadeTimeMessage, nFadeTime), NULL, NULL, (const void *)0x708),
    BoundedTypeDescriptorField("", DT_INT, offsetof(DeathFadeTimeMessage, bRecurseForAttachments), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DeathFadeTimeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DeathFadeTimeMessage("DeathFadeTimeMessage", 0, DeathFadeTimeMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DEATH_FADE_TIME_MESSAGE = &TypeDescriptor_DeathFadeTimeMessage;

static const TypeDescriptorField MapRevealSceneMessage_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(MapRevealSceneMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MapRevealSceneMessage, idSScene), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MapRevealSceneMessage, snoScene), "", SCENE_SNO, 0, -1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(MapRevealSceneMessage, transform), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MapRevealSceneMessage, idSWorld), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(MapRevealSceneMessage, bRevealed), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MapRevealSceneMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MapRevealSceneMessage("MapRevealSceneMessage", 0, MapRevealSceneMessage_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_MAP_REVEAL_SCENE_MESSAGE = &TypeDescriptor_MapRevealSceneMessage;

static const TypeDescriptorField SavePointInfoMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SavePointInfoMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SavePointInfoMessage, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SavePointInfoMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SavePointInfoMessage("SavePointInfoMessage", 0, SavePointInfoMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVE_POINT_INFO_MESSAGE = &TypeDescriptor_SavePointInfoMessage;

static const TypeDescriptorField HearthPortalInfoMessage_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HearthPortalInfoMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(HearthPortalInfoMessage, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(HearthPortalInfoMessage, unk_c), "", WORLD_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(HearthPortalInfoMessage, annPortal), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(HearthPortalInfoMessage, bResetUI), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(HearthPortalInfoMessage, bIsTieredRift), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HearthPortalInfoMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HearthPortalInfoMessage("HearthPortalInfoMessage", 0, HearthPortalInfoMessage_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HEARTHPORTAL_INFO_MESSAGE = &TypeDescriptor_HearthPortalInfoMessage;

static const TypeDescriptorField ReturnPointInfoMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ReturnPointInfoMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ReturnPointInfoMessage, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ReturnPointInfoMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ReturnPointInfoMessage("ReturnPointInfoMessage", 0, ReturnPointInfoMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_RETURN_POINT_INFO_MESSAGE = &TypeDescriptor_ReturnPointInfoMessage;

static const TypeDescriptorField ACDLookAtMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDLookAtMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDLookAtMessage, annSrc), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ACDLookAtMessage, annTarget), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDLookAtMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ACDLookAtMessage("ACDLookAtMessage", 0, ACDLookAtMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACD_LOOK_AT_MESSAGE = &TypeDescriptor_ACDLookAtMessage;

static const TypeDescriptorField KillCounterUpdateMessage_Fields[7] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(KillCounterUpdateMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(KillCounterUpdateMessage, eType), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_INT, offsetof(KillCounterUpdateMessage, uCount), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(KillCounterUpdateMessage, flXPMultiplier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(KillCounterUpdateMessage, uTotalTime), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(KillCounterUpdateMessage, bExpired), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(KillCounterUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_KillCounterUpdateMessage("KillCounterUpdateMessage", 0, KillCounterUpdateMessage_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_KILL_COUNTER_UPDATE_MESSAGE = &TypeDescriptor_KillCounterUpdateMessage;

static const TypeDescriptorField CurrencyCounterUpdateMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CurrencyCounterUpdateMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CurrencyCounterUpdateMessage, uAmountCollected), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CurrencyCounterUpdateMessage, eCurrencyType), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CurrencyCounterUpdateMessage, bExpired), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CurrencyCounterUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CurrencyCounterUpdateMessage("CurrencyCounterUpdateMessage", 0, CurrencyCounterUpdateMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CURRENCY_COUNTER_UPDATE_MESSAGE = &TypeDescriptor_CurrencyCounterUpdateMessage;

static const TypeDescriptorField LowHealthCombatMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(LowHealthCombatMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(LowHealthCombatMessage, flHealthPercent), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LowHealthCombatMessage, uXPRewarded), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LowHealthCombatMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LowHealthCombatMessage("LowHealthCombatMessage", 0, LowHealthCombatMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LOW_HEALTH_COMBAT_MESSAGE = &TypeDescriptor_LowHealthCombatMessage;

static const TypeDescriptorField SaviorMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SaviorMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SaviorMessage, annSavedPlayer), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SaviorMessage, uXPRewarded), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SaviorMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SaviorMessage("SaviorMessage", 0, SaviorMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SAVIOR_MESSAGE = &TypeDescriptor_SaviorMessage;

static const TypeDescriptorField FloatingNumberMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(FloatingNumberMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FloatingNumberMessage, annTarget), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(FloatingNumberMessage, flDamage), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(FloatingNumberMessage, eType), NULL, NULL, (const void *)0x26),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FloatingNumberMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FloatingNumberMessage("FloatingNumberMessage", 0, FloatingNumberMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_FLOATING_NUMBER_MESSAGE = &TypeDescriptor_FloatingNumberMessage;

static const TypeDescriptorField FloatingGBIDAmountMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(FloatingGBIDAmountMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(FloatingGBIDAmountMessage, wpPlace), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FloatingGBIDAmountMessage, nAmount), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(FloatingGBIDAmountMessage, gbidItem), "", 2, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FloatingGBIDAmountMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FloatingGBIDAmountMessage("FloatingGBIDAmountMessage", 0, FloatingGBIDAmountMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_FLOATING_GBID_AMOUNT_MESSAGE = &TypeDescriptor_FloatingGBIDAmountMessage;

static const TypeDescriptorField FloatingAmountMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(FloatingAmountMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(FloatingAmountMessage, wpPlace), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FloatingAmountMessage, nAmount), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(FloatingAmountMessage, nAmount2), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(FloatingAmountMessage, eType), NULL, NULL, (const void *)0x26),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FloatingAmountMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FloatingAmountMessage("FloatingAmountMessage", 0, FloatingAmountMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_FLOATING_AMOUNT_MESSAGE = &TypeDescriptor_FloatingAmountMessage;

static const TypeDescriptorField CalloutMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CalloutMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CalloutMessage, eType), NULL, NULL, (const void *)0x1a),
    OptionalTypeDescriptorField("", DT_FLOAT, offsetof(CalloutMessage, flAmount), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(CalloutMessage, uTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CalloutMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CalloutMessage("CalloutMessage", 0, CalloutMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CALLOUT_MESSAGE = &TypeDescriptor_CalloutMessage;

static const TypeDescriptorField RemoveRagdollMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RemoveRagdollMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RemoveRagdollMessage, annTarget), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(RemoveRagdollMessage, snoMonster), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RemoveRagdollMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RemoveRagdollMessage("RemoveRagdollMessage", 0, RemoveRagdollMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_REMOVE_RAGDOLL_MESSAGE = &TypeDescriptor_RemoveRagdollMessage;

static const TypeDescriptorField WorldStatusMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(WorldStatusMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WorldStatusMessage, idSWorld), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(WorldStatusMessage, dwWorldStatusBits), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WorldStatusMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldStatusMessage("WorldStatusMessage", 0, WorldStatusMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_WORLD_STATUS_MESSAGE = &TypeDescriptor_WorldStatusMessage;

static const TypeDescriptorField WorldDeletedMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(WorldDeletedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WorldDeletedMessage, idSWorld), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WorldDeletedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WorldDeletedMessage("WorldDeletedMessage", 0, WorldDeletedMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_WORLD_DELETED_MESSAGE = &TypeDescriptor_WorldDeletedMessage;

static const TypeDescriptorField BlizzconCVarsMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(BlizzconCVarsMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(BlizzconCVarsMessage, bMultiplayerEnabled), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(BlizzconCVarsMessage, bDHunterEnabled), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(BlizzconCVarsMessage, bBlizzconDropEnabled), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BlizzconCVarsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BlizzconCVarsMessage("BlizzconCVarsMessage", 0, BlizzconCVarsMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BLIZZCON_CVARS_MESSAGE = &TypeDescriptor_BlizzconCVarsMessage;

static const TypeDescriptorField WaypointActivatedMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(WaypointActivatedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WaypointActivatedMessage, annWaypoint), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WaypointActivatedMessage, annActivatingPlayer), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(WaypointActivatedMessage, snoLevelArea), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(WaypointActivatedMessage, bAnnounce), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WaypointActivatedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WaypointActivatedMessage("WaypointActivatedMessage", 0, WaypointActivatedMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_WAYPOINT_ACTIVATED_MESSAGE = &TypeDescriptor_WaypointActivatedMessage;

static const TypeDescriptorField TryWaypointMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TryWaypointMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TryWaypointMessage, annWaypoint), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TryWaypointMessage, nWaypoint), NULL, NULL, (const void *)0x64),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TryWaypointMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TryWaypointMessage("TryWaypointMessage", 0, TryWaypointMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRY_WAYPOINT_MESSAGE = &TypeDescriptor_TryWaypointMessage;

static const TypeDescriptorField LoreMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(LoreMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(LoreMessage, snoLore), "", LORE_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LoreMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LoreMessage("LoreMessage", 0, LoreMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LORE_MESSAGE = &TypeDescriptor_LoreMessage;

static const TypeDescriptorField PRTransformMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PRTransformMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(PRTransformMessage, tTransform), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PRTransformMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PRTransformMessage("PRTransformMessage", 0, PRTransformMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PRTRANSFORM_MESSAGE = &TypeDescriptor_PRTransformMessage;

static const TypeDescriptorField PlayerQuestMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerQuestMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerQuestMessage, nPlayerIndex), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField("", DT_SNO, offsetof(PlayerQuestMessage, snoQuest), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(PlayerQuestMessage, snoLevelArea), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerQuestMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerQuestMessage("PlayerQuestMessage", 0, PlayerQuestMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_QUEST_MESSAGE = &TypeDescriptor_PlayerQuestMessage;

static const TypeDescriptorField PlayerDeSyncSnapMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerDeSyncSnapMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(PlayerDeSyncSnapMessage, placeSnap), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerDeSyncSnapMessage, eReason), NULL, NULL, (const void *)0x3),
    TypeDescriptorField("", DT_SNO, offsetof(PlayerDeSyncSnapMessage, snoPowerCurrent), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerDeSyncSnapMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerDeSyncSnapMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerDeSyncSnapMessage("PlayerDeSyncSnapMessage", 0, PlayerDeSyncSnapMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_DESYNC_SNAP_MESSAGE = &TypeDescriptor_PlayerDeSyncSnapMessage;

static const TypeDescriptorField TradeMessage_Fields[8] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TradeMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TradeMessage, eMessageType), NULL, NULL, (const void *)0x9),
    TypeDescriptorField("", DT_INT, offsetof(TradeMessage, dwData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TradeMessage, dwData2), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(TradeMessage, uData64), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TradeMessage, ann), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(TradeMessage, arrItems), NULL, 1, 4, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TradeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TradeMessage("TradeMessage", 0, TradeMessage_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRADE_MESSAGE = &TypeDescriptor_TradeMessage;

static const TypeDescriptorField PVPTokenRedemptionMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PVPTokenRedemptionMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PVPTokenRedemptionMessage, nItems), NULL, NULL, (const void *)4),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(PVPTokenRedemptionMessage, arrItems), NULL, 1, 4, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(PVPTokenRedemptionMessage, arrGBIDs), NULL, 1, 4, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(PVPTokenRedemptionMessage, nStackSize), NULL, 1, 4, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PVPTokenRedemptionMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PVPTokenRedemptionMessage("PVPTokenRedemptionMessage", 0, PVPTokenRedemptionMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PVP_TOKEN_REDEMPTION_MESSAGE = &TypeDescriptor_PVPTokenRedemptionMessage;

static const TypeDescriptorField MapMarkerInfoMessage_Fields[18] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(MapMarkerInfoMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_nHashedName), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(MapMarkerInfoMessage, m_Place), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_ImageInfo), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MapMarkerInfoMessage, m_snoStringList), "", STRINGLIST_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MapMarkerInfoMessage, m_snoKnownActorOverride), "", ACTOR_SNO, 0, -1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(MapMarkerInfoMessage, m_snoQuestSource), "", QUEST_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_hLabel), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MapMarkerInfoMessage, m_fMaxDisplayRangeSq), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MapMarkerInfoMessage, m_fMinDisplayRangeSq), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(MapMarkerInfoMessage, m_fDiscoveryRangeSq), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_hImage), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_bActive), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_bCanBecomeArrow), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_bRespectsFoW), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_bIsPing), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(MapMarkerInfoMessage, m_nPlayerUseFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MapMarkerInfoMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MapMarkerInfoMessage("MapMarkerInfoMessage", 0, MapMarkerInfoMessage_Fields, 18, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_MAP_MARKER_MESSAGE = &TypeDescriptor_MapMarkerInfoMessage;

static const TypeDescriptorField BossEncounterMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(BossEncounterMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BossEncounterMessage, nPlayerIndex), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(BossEncounterMessage, snoEncounter), "", BOSS_ENCOUNTER_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BossEncounterMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BossEncounterMessage("BossEncounterMessage", 0, BossEncounterMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BOSS_ENCOUNTER_MESSAGE = &TypeDescriptor_BossEncounterMessage;

static const TypeDescriptorField SetDungeonMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SetDungeonMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SetDungeonMessage, nPlayerIndex), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(SetDungeonMessage, snoEncounter), "", ENCOUNTER_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SetDungeonMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SetDungeonMessage("SetDungeonMessage", 0, SetDungeonMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SET_DUNGEON_MESSAGE = &TypeDescriptor_SetDungeonMessage;

static const TypeDescriptorField EncounterInviteStateMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(EncounterInviteStateMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(EncounterInviteStateMessage, m_AcceptedPlayers), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(EncounterInviteStateMessage, m_PendingPlayers), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(EncounterInviteStateMessage, m_DeclinedPlayers), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EncounterInviteStateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EncounterInviteStateMessage("EncounterInviteStateMessage", 0, EncounterInviteStateMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BOSS_ENCOUNTER_INVITE_MESSAGE = &TypeDescriptor_EncounterInviteStateMessage;

static const TypeDescriptorField RiftStartAcceptedMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RiftStartAcceptedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RiftStartAcceptedMessage, unk_08), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RiftStartAcceptedMessage, unk_0c), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RiftStartAcceptedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RiftStartAcceptedMessage("RiftStartAcceptedMessage", 0, RiftStartAcceptedMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_RIFT_START_ACCEPTED_MESSAGE = &TypeDescriptor_RiftStartAcceptedMessage;

static const TypeDescriptorField RiftJoinMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RiftJoinMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RiftJoinMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RiftJoinMessage, nRiftTier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RiftJoinMessage, uRiftStartServerTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RiftJoinMessage, nPlayerKeyTiers), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RiftJoinMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RiftJoinMessage("RiftJoinMessage", 0, RiftJoinMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_RIFT_JOIN_MESSAGE = &TypeDescriptor_RiftJoinMessage;

static const TypeDescriptorField ActTransitionMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ActTransitionMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ActTransitionMessage, eAct), NULL, (const void *)-1, (const void *)0xbb8),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ActTransitionMessage, bOnJoin), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActTransitionMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActTransitionMessage("ActTransitionMessage", 0, ActTransitionMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACT_TRANSITION_MESSAGE = &TypeDescriptor_ActTransitionMessage;

static const TypeDescriptorField InterstitialMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(InterstitialMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(InterstitialMessage, ePlayerClass), NULL, (const void *)-1, (const void *)0x7),
    BoundedTypeDescriptorField("", DT_INT, offsetof(InterstitialMessage, fIsMale), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(InterstitialMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_InterstitialMessage("InterstitialMessage", 0, InterstitialMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_INTERSTITIAL_MESSAGE = &TypeDescriptor_InterstitialMessage;

static const TypeDescriptorField CameraFocusMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CameraFocusMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(CameraFocusMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CameraFocusMessage, bSnap), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CameraFocusMessage, flDuration), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CameraFocusMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CameraFocusMessage("CameraFocusMessage", 0, CameraFocusMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CAMERA_FOCUS_MESSAGE = &TypeDescriptor_CameraFocusMessage;

static const TypeDescriptorField DebugDrawPrimMessage_Fields[12] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DebugDrawPrimMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DebugDrawPrimMessage, uID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DebugDrawPrimMessage, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DebugDrawPrimMessage, idSWorld), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(DebugDrawPrimMessage, vPos0), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(DebugDrawPrimMessage, vPos1), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DebugDrawPrimMessage, flLength), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(DebugDrawPrimMessage, flRadius), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DebugDrawPrimMessage, uLifetimeInTicks), NULL, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(DebugDrawPrimMessage, rgbaColor), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(DebugDrawPrimMessage, szText), "", 1, 0x80, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DebugDrawPrimMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DebugDrawPrimMessage("DebugDrawPrimMessage", 0, DebugDrawPrimMessage_Fields, 12, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DEBUG_DRAW_PRIM_MESSAGE = &TypeDescriptor_DebugDrawPrimMessage;

static const TypeDescriptorField DebugDrawMovementTypeMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DebugDrawMovementTypeMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DebugDrawMovementTypeMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DebugDrawMovementTypeMessage, dwTimeout), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(DebugDrawMovementTypeMessage, szText), "", 1, 0x80, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DebugDrawMovementTypeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DebugDrawMovementTypeMessage("DebugDrawMovementTypeMessage", 0, DebugDrawMovementTypeMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DEBUG_DRAW_MOVEMENT_TYPE_MESSAGE = &TypeDescriptor_DebugDrawMovementTypeMessage;

static const TypeDescriptorField CameraZoomMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CameraZoomMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CameraZoomMessage, flZoom), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CameraZoomMessage, bSnap), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CameraZoomMessage, flDuration), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CameraZoomMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CameraZoomMessage("CameraZoomMessage", 0, CameraZoomMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CAMERA_ZOOM_MESSAGE = &TypeDescriptor_CameraZoomMessage;

static const TypeDescriptorField CameraYawMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CameraYawMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(CameraYawMessage, aAngle), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CameraYawMessage, bSnap), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_FLOAT, offsetof(CameraYawMessage, flDuration), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CameraYawMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_CameraYawMessage("CameraYawMessage", 0, CameraYawMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CAMERA_YAW_MESSAGE = &TypeDescriptor_CameraYawMessage;

static const TypeDescriptorField BossZoomMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(BossZoomMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(BossZoomMessage, flZoom), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(BossZoomMessage, flZoomSpeed), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BossZoomMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BossZoomMessage("BossZoomMessage", 0, BossZoomMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BOSS_ZOOM_MESSAGE = &TypeDescriptor_BossZoomMessage;

static const TypeDescriptorField PlayCutsceneMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayCutsceneMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayCutsceneMessage, nIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayCutsceneMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayCutsceneMessage("PlayCutsceneMessage", 0, PlayCutsceneMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAY_CUTSCENE_MESSAGE = &TypeDescriptor_PlayCutsceneMessage;

static const TypeDescriptorField PlayerWarpedMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerWarpedMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerWarpedMessage, eWarpReason), NULL, NULL, (const void *)0x11),
    TypeDescriptorField("", DT_FLOAT, offsetof(PlayerWarpedMessage, flWarpFadeInSeconds), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerWarpedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerWarpedMessage("PlayerWarpedMessage", 0, PlayerWarpedMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_WARPED_MESSAGE = &TypeDescriptor_PlayerWarpedMessage;

static const TypeDescriptorField DebugActorTooltipMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DebugActorTooltipMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DebugActorTooltipMessage, ann), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(DebugActorTooltipMessage, szTooltip), "", 1, 0x400, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DebugActorTooltipMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DebugActorTooltipMessage("DebugActorTooltipMessage", 0, DebugActorTooltipMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DISPLAY_ACTOR_DEBUG_TOOLTIP = &TypeDescriptor_DebugActorTooltipMessage;

static const TypeDescriptorField SalvageResultsMessage_Fields[8] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SalvageResultsMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_GBID, offsetof(SalvageResultsMessage, gbidOriginalItem), "", 2, 0, -1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SalvageResultsMessage, eIQLOriginalItem), NULL, (const void *)-1, (const void *)0xb),
    FilteredFixedArrayTypeDescriptorField("", DT_GBID, offsetof(SalvageResultsMessage, gbidMaterials), "", 0, 4, 2, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(SalvageResultsMessage, nMaterialsCounts), NULL, 1, 4, 0),
    TypeDescriptorField("", DT_INT, offsetof(SalvageResultsMessage, nMaterialsResults), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SalvageResultsMessage, unk_34), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SalvageResultsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SalvageResultsMessage("SalvageResultsMessage", 0, SalvageResultsMessage_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SALVAGE_RESULTS_MESSAGE = &TypeDescriptor_SalvageResultsMessage;

static const TypeDescriptorField TrySalvageAllMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(TrySalvageAllMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(TrySalvageAllMessage, eSalvageType), NULL, NULL, (const void *)0x2),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TrySalvageAllMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TrySalvageAllMessage("TrySalvageAllMessage", 0, TrySalvageAllMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRY_SALVAGE_ALL_MESSAGE = &TypeDescriptor_TrySalvageAllMessage;

static const TypeDescriptorField GameTestingWorldSamplingStartMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GameTestingWorldSamplingStartMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(GameTestingWorldSamplingStartMessage, snoWorld), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(GameTestingWorldSamplingStartMessage, snoScene), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameTestingWorldSamplingStartMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GameTestingWorldSamplingStartMessage("GameTestingWorldSamplingStartMessage", 0, GameTestingWorldSamplingStartMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GAME_TESTING_WORLD_SAMPLING_START_MESSAGE = &TypeDescriptor_GameTestingWorldSamplingStartMessage;

static const TypeDescriptorField GameTestingSkillSamplingStartMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GameTestingSkillSamplingStartMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(GameTestingSkillSamplingStartMessage, snoPower), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameTestingSkillSamplingStartMessage, nRuneIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameTestingSkillSamplingStartMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GameTestingSkillSamplingStartMessage("GameTestingSkillSamplingStartMessage", 0, GameTestingSkillSamplingStartMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GAME_TESTING_SKILL_SAMPLING_START_MESSAGE = &TypeDescriptor_GameTestingSkillSamplingStartMessage;

static const TypeDescriptorField RequestBuffCancelMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RequestBuffCancelMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(RequestBuffCancelMessage, snoPower), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RequestBuffCancelMessage, nBuffIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RequestBuffCancelMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RequestBuffCancelMessage("RequestBuffCancelMessage", 0, RequestBuffCancelMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_REQUEST_BUFF_CANCEL_MESSAGE = &TypeDescriptor_RequestBuffCancelMessage;

static const TypeDescriptorField PlayErrorSoundMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayErrorSoundMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayErrorSoundMessage, dwSoundTag), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayErrorSoundMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayErrorSoundMessage("PlayErrorSoundMessage", 0, PlayErrorSoundMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAY_ERROR_SOUND_MESSAGE = &TypeDescriptor_PlayErrorSoundMessage;

static const TypeDescriptorField HirelingRequestLearnSkillMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HirelingRequestLearnSkillMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingRequestLearnSkillMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(HirelingRequestLearnSkillMessage, snoSkill), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HirelingRequestLearnSkillMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HirelingRequestLearnSkillMessage("HirelingRequestLearnSkillMessage", 0, HirelingRequestLearnSkillMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HIRELING_REQUEST_LEARN_SKILL_MESSAGE = &TypeDescriptor_HirelingRequestLearnSkillMessage;

static const TypeDescriptorField LogoutTickTimeMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(LogoutTickTimeMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(LogoutTickTimeMessage, eContext), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(LogoutTickTimeMessage, uMinWaitTicks), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LogoutTickTimeMessage, uMaxWaitTicks), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(LogoutTickTimeMessage, uKickTicks), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LogoutTickTimeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LogoutTickTimeMessage("LogoutTickTimeMessage", 0, LogoutTickTimeMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_LOGOUT_TICKTIME_MESSAGE = &TypeDescriptor_LogoutTickTimeMessage;

static const TypeDescriptorField VersionsMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(VersionsMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(VersionsMessage, uSNOPackHash), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(VersionsMessage, uProtocolHash), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(VersionsMessage, szBuildVer), "", 1, 0x20, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VersionsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_VersionsMessage("VersionsMessage", 0, VersionsMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_VERSIONS_MESSAGE = &TypeDescriptor_VersionsMessage;

static const TypeDescriptorField HandicapMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HandicapMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HandicapMessage, nHandicapLevel), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HandicapMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HandicapMessage("HandicapMessage", 0, HandicapMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HANDICAP_MESSAGE = &TypeDescriptor_HandicapMessage;

static const TypeDescriptorField PlayerSetCameraObserverMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerSetCameraObserverMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayerSetCameraObserverMessage, snoObserver), "", OBSERVER_SNO, 0, -1),
    OptionalTypeDescriptorField("", DT_WORLD_PLACE, offsetof(PlayerSetCameraObserverMessage, wpToOrbit), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSetCameraObserverMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSetCameraObserverMessage("PlayerSetCameraObserverMessage", 0, PlayerSetCameraObserverMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_SET_CAMERA_OBSERVER_MESSAGE = &TypeDescriptor_PlayerSetCameraObserverMessage;

static const TypeDescriptorField PlayerSetCameraOrbitMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerSetCameraOrbitMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(PlayerSetCameraOrbitMessage, wpToOrbit), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSetCameraOrbitMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSetCameraOrbitMessage("PlayerSetCameraOrbitMessage", 0, PlayerSetCameraOrbitMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_SET_CAMERA_ORBIT_MESSAGE = &TypeDescriptor_PlayerSetCameraOrbitMessage;

static const TypeDescriptorField PlayerSetCameraDefaultsMessage_Fields[2] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerSetCameraDefaultsMessage, tHeader), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerSetCameraDefaultsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerSetCameraDefaultsMessage("PlayerSetCameraDefaultsMessage", 0, PlayerSetCameraDefaultsMessage_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_SET_CAMERA_DEFAULTS_MESSAGE = &TypeDescriptor_PlayerSetCameraDefaultsMessage;

static const TypeDescriptorField OpenWorldModeChangeMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(OpenWorldModeChangeMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(OpenWorldModeChangeMessage, eOpenWorldMode), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(OpenWorldModeChangeMessage, eAct), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(OpenWorldModeChangeMessage, nParam), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(OpenWorldModeChangeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_OpenWorldModeChangeMessage("OpenWorldModeChangeMessage", 0, OpenWorldModeChangeMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_OPEN_WORLD_MODE_CHANGE_MESSAGE = &TypeDescriptor_OpenWorldModeChangeMessage;

static const TypeDescriptorField PlayerConsoleSetCameraDefaultsMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerConsoleSetCameraDefaultsMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayerConsoleSetCameraDefaultsMessage, bForce), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerConsoleSetCameraDefaultsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerConsoleSetCameraDefaultsMessage("PlayerConsoleSetCameraDefaultsMessage", 0, PlayerConsoleSetCameraDefaultsMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_CONSOLE_SET_CAMERA_DEFAULTS_MESSAGE = &TypeDescriptor_PlayerConsoleSetCameraDefaultsMessage;

static const TypeDescriptorField PlayerClearClientWalkPowerMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerClearClientWalkPowerMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerClearClientWalkPowerMessage, ann), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayerClearClientWalkPowerMessage, snoPower), "", POWER_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerClearClientWalkPowerMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerClearClientWalkPowerMessage("PlayerClearClientWalkPowerMessage", 0, PlayerClearClientWalkPowerMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_CLEAR_CLIENT_WALK_POWER_MESSAGE = &TypeDescriptor_PlayerClearClientWalkPowerMessage;

static const TypeDescriptorField ActorLookOverrideChangedMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ActorLookOverrideChangedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActorLookOverrideChangedMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActorLookOverrideChangedMessage, nChange), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActorLookOverrideChangedMessage, nLookOverrideHash), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActorLookOverrideChangedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActorLookOverrideChangedMessage("ActorLookOverrideChangedMessage", 0, ActorLookOverrideChangedMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACTOR_LOOK_OVERRIDE_CHANGED = &TypeDescriptor_ActorLookOverrideChangedMessage;

static const TypeDescriptorField PortedToPlayerMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PortedToPlayerMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PortedToPlayerMessage, nPlayerIndexPorted), NULL, (const void *)-1, (const void *)0x3),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PortedToPlayerMessage, nPlayerIndexTarget), NULL, (const void *)-1, (const void *)0x3),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PortedToPlayerMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PortedToPlayerMessage("PortedToPlayerMessage", 0, PortedToPlayerMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PORTED_TO_PLAYER_MESSAGE = &TypeDescriptor_PortedToPlayerMessage;

static const TypeDescriptorField PortedToWaypointMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PortedToWaypointMessage, tHeader), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PortedToWaypointMessage, nPlayerIndex), NULL, (const void *)-1, (const void *)0x3),
    FilterTypeDescriptorField("", DT_SNO, offsetof(PortedToWaypointMessage, snoLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PortedToWaypointMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PortedToWaypointMessage("PortedToWaypointMessage", 0, PortedToWaypointMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PORTED_TO_WAYPOINT_MESSAGE = &TypeDescriptor_PortedToWaypointMessage;

static const TypeDescriptorField ItemJunkFlagChangedMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ItemJunkFlagChangedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ItemJunkFlagChangedMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ItemJunkFlagChangedMessage, bJunkFlag), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ItemJunkFlagChangedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ItemJunkFlagChangedMessage("ItemJunkFlagChangedMessage", 0, ItemJunkFlagChangedMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_JUNK_FLAG_MESSAGE = &TypeDescriptor_ItemJunkFlagChangedMessage;

static const TypeDescriptorField AchievementProgressXboxOneMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(AchievementProgressXboxOneMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AchievementProgressXboxOneMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AchievementProgressXboxOneMessage, nProfileAwardId), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AchievementProgressXboxOneMessage, nProgress), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AchievementProgressXboxOneMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AchievementProgressXboxOneMessage("AchievementProgressXboxOneMessage", 0, AchievementProgressXboxOneMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACHIEVEMENT_PROGRESS_XBOXONE_MESSAGE = &TypeDescriptor_AchievementProgressXboxOneMessage;

static const TypeDescriptorField ConsoleCounterIncrementMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ConsoleCounterIncrementMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConsoleCounterIncrementMessage, nCounter), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ConsoleCounterIncrementMessage, uValue), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConsoleCounterIncrementMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConsoleCounterIncrementMessage("ConsoleCounterIncrementMessage", 0, ConsoleCounterIncrementMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CONSOLE_COUNTER_INCREMENT_MESSAGE = &TypeDescriptor_ConsoleCounterIncrementMessage;

static const TypeDescriptorField ServerNotificationDataMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ServerNotificationDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ServerNotificationDataMessage, uSec), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ServerNotificationDataMessage, eType), NULL, NULL, (const void *)0x3),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ServerNotificationDataMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ServerNotificationDataMessage("ServerNotificationDataMessage", 0, ServerNotificationDataMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SERVER_NOTIFICATION_DATA_MESSAGE = &TypeDescriptor_ServerNotificationDataMessage;

static const TypeDescriptorField JewelUpgradeResultsMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(JewelUpgradeResultsMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(JewelUpgradeResultsMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(JewelUpgradeResultsMessage, nNumRanksGained), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(JewelUpgradeResultsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_JewelUpgradeResultsMessage("JewelUpgradeResultsMessage", 0, JewelUpgradeResultsMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_JEWEL_UPGRADE_RESULTS_MESSAGE = &TypeDescriptor_JewelUpgradeResultsMessage;

static const TypeDescriptorField DungeonFinderClosingMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DungeonFinderClosingMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DungeonFinderClosingMessage, nTickClosing), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DungeonFinderClosingMessage, nRiftLevel), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DungeonFinderClosingMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DungeonFinderClosingMessage("DungeonFinderClosingMessage", 0, DungeonFinderClosingMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DUNGEON_FINDER_CLOSING_MESSAGE = &TypeDescriptor_DungeonFinderClosingMessage;

static const TypeDescriptorField UberBosssClosingMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(UberBosssClosingMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(UberBosssClosingMessage, unk_08), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(UberBosssClosingMessage, unk_0c), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UberBosssClosingMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UberBosssClosingMessage("UberBosssClosingMessage", 0, UberBosssClosingMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_UBER_BOSSS_CLOSING_MESSAGE = &TypeDescriptor_UberBosssClosingMessage;

static const TypeDescriptorField WarningCountdownNotificationMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(WarningCountdownNotificationMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(WarningCountdownNotificationMessage, unk_08), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(WarningCountdownNotificationMessage, unk_0c), "", 1, sizeof(WarningCountdownNotificationMessage::unk_0c), -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(WarningCountdownNotificationMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_WarningCountdownNotificationMessage("WarningCountdownNotificationMessage", 0, WarningCountdownNotificationMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_WARNING_COUNTDOWN_NOTIFICATION_MESSAGE = &TypeDescriptor_WarningCountdownNotificationMessage;

static const TypeDescriptorField BlizzconEndScreenMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(BlizzconEndScreenMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BlizzconEndScreenMessage, nRewardTier), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(BlizzconEndScreenMessage, ann), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BlizzconEndScreenMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BlizzconEndScreenMessage("BlizzconEndScreenMessage", 0, BlizzconEndScreenMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BLIZZCON_END_SCREEN_MESSAGE = &TypeDescriptor_BlizzconEndScreenMessage;

static const TypeDescriptorField HoradricQuestCursedRealmResults_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HoradricQuestCursedRealmResults, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HoradricQuestCursedRealmResults, nActIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HoradricQuestCursedRealmResults, nRewardAmount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HoradricQuestCursedRealmResults), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HoradricQuestCursedRealmResults("HoradricQuestCursedRealmResults", 0, HoradricQuestCursedRealmResults_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_HORADRIC_QUEST_CURSED_REALM_RESULTS_MESSAGE = &TypeDescriptor_HoradricQuestCursedRealmResults;

static const TypeDescriptorField SetDungeonDialogMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SetDungeonDialogMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SetDungeonDialogMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SetDungeonDialogMessage, nLabelDescription), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SetDungeonDialogMessage, nLabelTitle), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SetDungeonDialogMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SetDungeonDialogMessage("SetDungeonDialogMessage", 0, SetDungeonDialogMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SET_DUNGEON_DIALOG_MESSAGE = &TypeDescriptor_SetDungeonDialogMessage;

static const TypeDescriptorField FirstOfTheDayMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(FirstOfTheDayMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FirstOfTheDayMessage, eType), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(FirstOfTheDayMessage, uCurrentPlatinum), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(FirstOfTheDayMessage, uPlatinumIncrement), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FirstOfTheDayMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FirstOfTheDayMessage("FirstOfTheDayMessage", 0, FirstOfTheDayMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_FIRST_OF_THE_DAY_MESSAGE = &TypeDescriptor_FirstOfTheDayMessage;

static const TypeDescriptorField PlatinumAwardedMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlatinumAwardedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(PlatinumAwardedMessage, uCurrentPlatinum), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(PlatinumAwardedMessage, uPlatinumIncrement), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlatinumAwardedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlatinumAwardedMessage("PlatinumAwardedMessage", 0, PlatinumAwardedMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLATINUM_AWARDED_MESSAGE = &TypeDescriptor_PlatinumAwardedMessage;

static const TypeDescriptorField PlatinumAchievementAwardedMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlatinumAchievementAwardedMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(PlatinumAchievementAwardedMessage, uCurrentPlatinum), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(PlatinumAchievementAwardedMessage, uPlatinumIncrement), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(PlatinumAchievementAwardedMessage, idAchievement), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlatinumAchievementAwardedMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlatinumAchievementAwardedMessage("PlatinumAchievementAwardedMessage", 0, PlatinumAchievementAwardedMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLATINUM_ACHIEVEMENT_AWARDED_MESSAGE = &TypeDescriptor_PlatinumAchievementAwardedMessage;

static const TypeDescriptorField SetDungeonResultsMessage_Fields[14] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(SetDungeonResultsMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(SetDungeonResultsMessage, snoQuestKill), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, nQuestKillMonsterCount), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(SetDungeonResultsMessage, snoQuestBonus1), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, bQuestBonus1Success), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_SNO, offsetof(SetDungeonResultsMessage, snoQuestBonus2), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, bQuestBonus2Success), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_SNO, offsetof(SetDungeonResultsMessage, snoQuestMastery), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, bQuestMasterySuccess), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, bQuestKillSuccess), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, bShowTotalTime), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, nTimeTaken), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SetDungeonResultsMessage, nTargetTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SetDungeonResultsMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_SetDungeonResultsMessage("SetDungeonResultsMessage", 0, SetDungeonResultsMessage_Fields, 14, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SET_DUNGEON_RESULTS_MESSAGE = &TypeDescriptor_SetDungeonResultsMessage;

static const TypeDescriptorField DungeonFinderCompletionTimeMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DungeonFinderCompletionTimeMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DungeonFinderCompletionTimeMessage, nCompletionTimeMessage), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DungeonFinderCompletionTimeMessage, nPlayerIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DungeonFinderCompletionTimeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DungeonFinderCompletionTimeMessage("DungeonFinderCompletionTimeMessage", 0, DungeonFinderCompletionTimeMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SET_DUNGEON_FINDER_TIME_MESSAGE = &TypeDescriptor_DungeonFinderCompletionTimeMessage;

static const TypeDescriptorField AvengerInfoMessage_Fields[9] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, 0, NULL, 1),
    CharArrayTypeDescriptorField("", 8, "", 1, 0x20, -1),
    CharArrayTypeDescriptorField("", 0x28, "", 1, 0x10, -1),
    TypeDescriptorField("", DT_INT, 0x38, NULL, 1),
    TypeDescriptorField("", DT_INT, 0x3c, NULL, 1),
    TypeDescriptorField("", DT_INT, 0x40, NULL, 1),
    TypeDescriptorField("", DT_INT, 0x44, NULL, 1),
    TypeDescriptorField("", DT_INT, 0x48, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 0x4c, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AvengerInfoMessage("AvengerInfoMessage", 0, AvengerInfoMessage_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_AVENGER_INFO_MESSAGE = &TypeDescriptor_AvengerInfoMessage;

static const TypeDescriptorField AvengerUpdateSourceMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, 0, NULL, 1),
    TypeDescriptorField("", DT_INT, 8, NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, 0xc, NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AvengerUpdateSourceMessage("AvengerUpdateSourceMessage", 0, AvengerUpdateSourceMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_AVENGER_UPDATE_SOURCE_MESSAGE = &TypeDescriptor_AvengerUpdateSourceMessage;

static const TypeDescriptorField PlayerGiftMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayerGiftMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerGiftMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(PlayerGiftMessage, nId), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayerGiftMessage, nOwnerIndex), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(PlayerGiftMessage, szPlayerName), "", 1, 0x20, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayerGiftMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayerGiftMessage("PlayerGiftMessage", 0, PlayerGiftMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAYER_GIFT_MESSAGE = &TypeDescriptor_PlayerGiftMessage;

// 2.7.7 dark-alchemy message descriptors. In 2.7.7 they are built, in this order, by the static
// init sub_71004b4c50 of a translation unit that is not in the source tree yet (0x4b2140..0x4b7340);
// they live here until that file exists.
static const TypeDescriptorField ActivateNodeMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ActivateNodeMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActivateNodeMessage, unk_08), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ActivateNodeMessage, unk_0c), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ActivateNodeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ActivateNodeMessage("ActivateNodeMessage", 0, ActivateNodeMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ACTIVATE_NODE_MESSAGE = &TypeDescriptor_ActivateNodeMessage;

static const TypeDescriptorField DectivateNodeMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(DectivateNodeMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DectivateNodeMessage, unk_08), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DectivateNodeMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DectivateNodeMessage("DectivateNodeMessage", 0, DectivateNodeMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_DECTIVATE_NODE_MESSAGE = &TypeDescriptor_DectivateNodeMessage;

static const TypeDescriptorField GizmoInteractMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GizmoInteractMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GizmoInteractMessage, unk_08), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GizmoInteractMessage, unk_0c), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GizmoInteractMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_GizmoInteractMessage("GizmoInteractMessage", 0, GizmoInteractMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_GIZMO_INTERACT_MESSAGE = &TypeDescriptor_GizmoInteractMessage;


#include "../Game.h"
#include "../Player.h"
#include "../../Server/PlayerList.h"
#include "../../Sigma/RefString.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/message_lite.h"

BOOL GetGameMessageTypeAndSize(GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor, uint32 & dwSize, const char *& pszMessageLabel);  // ByteswapMessage.cpp:74
void ServerPutMessageOnOutgoingQueue(GameConnectionID idGameConnection, void * pMessage);  // X/Server/Network/Server.cpp
BOOL PlayersAreLocalToEachOther(const Player & tPlayer0, const Player & tPlayer1);  // X/Common/Player.cpp

// The case list was generated from the original's lookup (values) and the DWARF enumerator names.
BOOL MessageIsMultiCast(GameMessageType eType)
{
    switch (eType)
    {
        case SMSG_TRY_CONSOLE_COMMAND:
        case SMSG_NEW_PLAYER:
        case SMSG_ACD_ENTER_KNOWN:
        case SMSG_ACD_EXIT_KNOWN:
        case SMSG_ACD_WORLD_POSITION:
        case SMSG_ACD_INVENTORY_POSITION:
        case SMSG_ACD_INVENTORY_UPDATE_ACTOR_SNO:
        case SMSG_AFFIX:
        case SMSG_RARE_MONSTER_NAMES:
        case SMSG_RARE_ITEM_NAME:
        case SMSG_PORTAL_SPECIFIER:
        case SMSG_ATTRIBUTE_SET_VALUE:
        case SMSG_ATTRIBUTES_SET_VALUES:
        case SMSG_VISUAL_INVENTORY:
        case SMSG_PROJECTILE_STICK:
        case SMSG_PLAY_ANIMATION:
        case SMSG_PLAY_IDLE_ANIMATION:
        case SMSG_ACD_TRANSLATE_NORMAL:
        case SMSG_ACD_TRANSLATE_SNAPPED:
        case SMSG_ACD_TRANSLATE_FACING:
        case SMSG_ACD_TRANSLATE_FIXED:
        case SMSG_ACD_TRANSLATE_ARC:
        case SMSG_ACD_TRANSLATE_DETPATH:
        case SMSG_ACD_TRANSLATE_DETPATH_SIN:
        case SMSG_ACD_TRANSLATE_DETPATH_SPIRAL:
        case SMSG_ACD_TRANSLATE_DETPATH_PIE_WEDGE:
        case SMSG_ACD_TRANSLATE_STOPPED:
        case SMSG_ACD_TRANSLATE_SYNC:
        case SMSG_ACD_TRANSLATE_FIXED_UPDATE:
        case SMSG_PLAY_EFFECT:
        case SMSG_PLAY_HIT_EFFECT:
        case SMSG_PLAY_HIT_EFFECT_OVERRIDE:
        case SMSG_PLAY_NONPOSITIONAL_SOUND:
        case SMSG_PLAY_ERROR_SOUND:
        case SMSG_PLAY_MUSIC:
        case SMSG_PLAY_CUTSCENE:
        case SMSG_PLAY_FINAL_CUTSCENE:
        case SMSG_PLAY_ANNIVERSARY_CUTSCENE:
        case SMSG_SCOMPLEX_EFFECT_ADD:
        case SMSG_FLIPPY:
        case SMSG_ACTIVATE_WAYPOINT:
        case SMSG_HEARTBEAT:
        case SMSG_QUEST_REWARD:
        case SMSG_TIERED_RIFT_REWARD:
        case SMSG_PLAYER_LEVEL:
        case SMSG_PLAYER_ALT_LEVEL:
        case SMSG_ACD_SET_IDLE_ANIM:
        case SMSG_ACD_COLL_FLAGS:
        case SMSG_CURRENCY_MODIFIED:
        case SMSG_ACT:
        case SMSG_INTERSTITIAL:
        case SMSG_EFFECT_GROUP_ACD_TO_ACD:
        case SMSG_ROPE_EFFECT_ACD_TO_ACD:
        case SMSG_ROPE_EFFECT_ACD_TO_PLACE:
        case SMSG_INVENTORY_CREATE:
        case SMSG_SHRINE_TEXT_DISPLAY:
        case SMSG_ACTIVATE_POOL_OF_REFLECTION:
        case SMSG_GAME_SYNCED_DATA:
        case SMSG_CHANGE_ACTOR:
        case SMSG_PLAYER_WARPED:
        case SMSG_VICTIM:
        case SMSG_KILLCOUNT:
        case SMSG_WORLD_STATUS:
        case SMSG_TRIGGER_LIGHTNING:
        case SMSG_PLAY_CONV_LINE:
        case SMSG_STOP_CONV_LINE:
        case SMSG_END_CONV:
        case SMSG_LORE_LEARNED:
        case SMSG_BOSS_JOIN_ENCOUNTER:
        case SMSG_BOSS_ENCOUNTER_INVITE_STATE:
        case SMSG_BOSS_ENCOUNTER_STARTED:
        case SMSG_BOSS_ENCOUNTER_CANCELED:
        case SMSG_BOSS_ENCOUNTER_ONE_DECLINED:
        case SMSG_SET_DUNGEON_ENCOUNTER_INVITE_STATE:
        case SMSG_SET_DUNGEON_CANCELED:
        case SMSG_SET_DUNGEON_ONE_DECLINED:
        case SMSG_SET_DUNGEON_JOIN:
        case SMSG_RIFT_INVITE_STATE:
        case SMSG_RIFT_JOIN_ENCOUNTER:
        case SMSG_CAMERA_FOCUS_ACD:
        case SMSG_ACHIEVEMENT_AWARDED:
        case SMSG_ACHIEVEMENT_PROGRESS_XBOXONE:
        case SMSG_SHRINE_GLOBE_TEXT_DISPLAY:
        case SMSG_ACTOR_LOOK_OVERRIDE_CHANGED:
        case SMSG_UPSCALING_LEVEL_CHANGED:
        case SMSG_RIFT_PARTICIPANTS_ADDED:
        case SMSG_SHOW_RIFT_END_SCREEN:
        case SMSG_PRELOAD_LOOT_RUN_LEADERBOARD_INFO:
        case SMSG_AMIIBO_SCAN_BEGIN:
        case SMSG_AMIIBO_SCAN_END:
        case SMSG_SYNC_AMIIBO_SCAN_TIME:
            return FALSE;
        default:
            return TRUE;
    }
}

BOOL MessageCanBeDropped(GameMessageType eType)
{
    switch (eType)
    {
        case SMSG_PLAY_EFFECT:
        case SMSG_PLAY_HIT_EFFECT:
        case SMSG_PLAY_HIT_EFFECT_OVERRIDE:
        case SMSG_PLAY_NONPOSITIONAL_SOUND:
        case SMSG_PLAY_ERROR_SOUND:
        case SMSG_FLOATING_NUMBER:
        case SMSG_FLOATING_AMOUNT:
            return TRUE;
        default:
            return FALSE;
    }
}

void MessageSendToClient(const PlayerList & listDestinations, GameMessageType eType, void * pMessage, uint32 dwMessageSize)
{
    RequiredMessageHeader * ptHeader;

    if (GameGetParts() == GAME_PARTS_HIDDEN_SERVER)
        return;

    if (listDestinations.IsEmpty())
        return;

    ptHeader = (RequiredMessageHeader *)pMessage;
    ptHeader->eType = eType;
    ptHeader->dwSize = (uint16)dwMessageSize;

    POS posPlayer = listDestinations.GetHeadPosition();
    while (posPlayer)
    {
        Player * ptPlayer = listDestinations.GetNext(posPlayer);
        if (ptPlayer->idGameConnection == -1)
            continue;

        if (!MessageIsMultiCast(eType))
        {
            BOOL bWasSentToConsole = FALSE;
            for (POS posPlayer2 = listDestinations.GetHeadPosition(); posPlayer2 && !bWasSentToConsole; )
            {
                Player * ptPlayer2 = listDestinations.GetNext(posPlayer2);
                if (ptPlayer == ptPlayer2)
                    break;
                bWasSentToConsole = PlayersAreLocalToEachOther(*ptPlayer, *ptPlayer2);
            }
            if (bWasSentToConsole)
                continue;
        }

        ptHeader->nControllerIndex = ptPlayer->nControllerIndex;
        ServerPutMessageOnOutgoingQueue(ptPlayer->idGameConnection, pMessage);
    }
}

uint32 ComposeProtocolBufferMessage(const google::protobuf::MessageLite & tMessage, GameMessageType eType, uint8 * pMessageBuffer, uint32 uBufferSize);

// NON_MATCHING: register allocation only (eType in w21 vs w20, remaining size in x21 vs x20, buffer pointer in x22)
void MessageSendToClient(const PlayerList & listDestinations, GameMessageType eType, const google::protobuf::MessageLite & tMessage)
{
    // 2.7.7: a message too big for one buffer is composed in a heap buffer and sent as SMSG_CHUNKED pieces.
    uint32 uTotalSize;
    {
        const XTypeDescriptor * pTypeDescriptor = NULL;
        uint32 dwSize = 0;
        const char * pszLabel = NULL;
        GetGameMessageTypeAndSize(eType, pTypeDescriptor, dwSize, pszLabel);
        uTotalSize = dwSize + tMessage.ByteSize();
    }

    uint8 auBlobMessage[32768];
    if (uTotalSize > sizeof(auBlobMessage))
    {
        blz::vector<uint8> vMessage;
        vMessage.resize(uTotalSize);
        ComposeProtocolBufferMessage(tMessage, eType, vMessage.data(), uTotalSize);

        GenericBlobMessage tBlobHeader = {};
        SigmaMemoryCopy(&tBlobHeader, vMessage.data(), sizeof(GenericBlobMessage));
        tBlobHeader.tHeader.eType = eType;
        SigmaMemoryCopy(vMessage.data(), &tBlobHeader, sizeof(GenericBlobMessage));

        const uint8 * pData = vMessage.data();
        size_t uRemaining = uTotalSize;
        while (uRemaining > 0)
        {
            size_t uChunkSize = uRemaining < 0x7f28 ? uRemaining : 0x7f28;
            ChunkMessage tChunkMessage = {};
            tChunkMessage.unk_08 = (uint32)uChunkSize;
            tChunkMessage.unk_0c = uRemaining <= 0x7f28;
            SigmaMemoryCopy(auBlobMessage, &tChunkMessage, sizeof(ChunkMessage));
            SigmaMemoryCopy(auBlobMessage + sizeof(ChunkMessage), pData, uChunkSize);
            MessageSendToClient(listDestinations, SMSG_CHUNKED, auBlobMessage, (uint32)uChunkSize + sizeof(ChunkMessage));
            pData += uChunkSize;
            uRemaining -= uChunkSize;
        }
    }
    else
    {
        uint32 uTotalMessageSize = ComposeProtocolBufferMessage(tMessage, eType, auBlobMessage, sizeof(auBlobMessage));
        MessageSendToClient(listDestinations, eType, auBlobMessage, uTotalMessageSize);
    }
}

uint32 ComposeProtocolBufferMessage(const google::protobuf::MessageLite & tMessage, GameMessageType eType, uint8 * pMessageBuffer, uint32 uBufferSize)
{
    GenericBlobMessage * pGenericBlobMessage = (GenericBlobMessage *)pMessageBuffer;
    const XTypeDescriptor * pTypeDescriptor;
    uint32 dwSize;
    const char * pszLabel;
    GetGameMessageTypeAndSize(eType, pTypeDescriptor, dwSize, pszLabel);

    uint8 * pBlobData = pMessageBuffer + sizeof(GenericBlobMessage);
    int32 nBlobSize = tMessage.ByteSize();
    tMessage.SerializeWithCachedSizesToArray(pBlobData);
    pGenericBlobMessage->uBlobSize = nBlobSize;
    return nBlobSize + sizeof(GenericBlobMessage);
}

BOOL ParseProtocolBufferMessage(GameMessageType eType, uint8 * pMessageBuffer, google::protobuf::MessageLite & tMessage)
{
    GenericBlobMessage * pGenericBlobMessage = (GenericBlobMessage *)pMessageBuffer;
    const XTypeDescriptor * pTypeDescriptor;
    uint32 dwSize;
    const char * pszLabel;
    GetGameMessageTypeAndSize(eType, pTypeDescriptor, dwSize, pszLabel);
    if (pTypeDescriptor != DT_GENERIC_BLOB_MESSAGE)
        return FALSE;

    uint8 * pBlobData = pMessageBuffer + sizeof(GenericBlobMessage);
    tMessage.Clear();
    return tMessage.ParseFromArray(pBlobData, pGenericBlobMessage->uBlobSize);
}

CRefString GameMessageTypeToName(GameMessageType eMessageType)
{
    CRefString sName;
    const XTypeDescriptor * pTypeDescriptor = NULL;
    uint32 dwSize = 0;
    const char * pszMessageLabel;
    if (GetGameMessageTypeAndSize(eMessageType, pTypeDescriptor, dwSize, pszMessageLabel))
        sName = pszMessageLabel;
    else
        sName = "Unknown GameMessageType";
    return sName;
}
