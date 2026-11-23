// Original file: X/Common/ConversationCommon.cpp

#include "CommonTypes.h"
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp

#include "GameSwig.h"
#include "GlobalSNO.h"

// X/Common/ConversationCommon.h, which is not adopted yet.
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

struct PlayConvLineMessage {  // size 0x80
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annSpeaker;  // 0x08
    ACDNetworkName annParticipants[9];  // 0x0c
    PlayLineParams tParams;  // 0x30
    uint32 dwTimeToDisplay;  // 0x7c
};
static_assert(sizeof(PlayConvLineMessage) == 0x80, "PlayConvLineMessage");

struct AdvanceConvMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConv;  // 0x08
    int32 nLineUID;  // 0x0c
};
static_assert(sizeof(AdvanceConvMessage) == 0x10, "AdvanceConvMessage");

struct UpdateConvAutoAdvanceMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConv;  // 0x08
    int32 nLineUID;  // 0x0c
    uint32 dwNextAdvanceTime;  // 0x10
};
static_assert(sizeof(UpdateConvAutoAdvanceMessage) == 0x14, "UpdateConvAutoAdvanceMessage");

struct StopConvLineMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    int32 nConvLineGUID;  // 0x08
    BOOL fStoppedByUser;  // 0x0c
};
static_assert(sizeof(StopConvLineMessage) == 0x10, "StopConvLineMessage");

struct EndConversationMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoConversation;  // 0x08
    ACDNetworkName annSpeaker;  // 0x0c
    int8 nPlayerTalkingIndex;  // 0x10
};
static_assert(sizeof(EndConversationMessage) == 0x14, "EndConversationMessage");

extern const uint32 gcdwConvAutoAdvanceTicks = 30;  // line 19

// line 89
SNO ConversationGetEndGameConversation(uint32 uContentFlags)
{
    SNO snoEndGameConversation = GlobalSNOGet(GLOBALSNO_END_GAME_CONVERSATION_VANILLA);
    if (uContentFlags & (1 << 4))
    {
        snoEndGameConversation = GlobalSNOGet(GLOBALSNO_END_GAME_CONVERSATION_X1);
    }

    return snoEndGameConversation;
}

// The field names are "" in the binary.
static const TypeDescriptorField PlayLineParams_Fields[19] = {
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayLineParams, snoConversation), "", CONVERSATION_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, nSpeakingPlayerIndex), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayLineParams, bZoom), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayLineParams, bFirstLine), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayLineParams, bHostile), NULL, NULL, (const void *)0x1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayLineParams, bPlayerInitiated), NULL, NULL, (const void *)0x1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, dwLineUID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, eLineSpeaker), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, eLineGender), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, eLineClass), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, ePlayerGender), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, ePlayerClass), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayLineParams, snoSpeakerActor), "", ACTOR_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, dwLineFlags), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, dwLineAnimTag), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, dwLineDisplayTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, nLineGUID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayLineParams, nPriority), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayLineParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayLineParams("PlayLineParams", 0, PlayLineParams_Fields, 19, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAY_LINE_PARAMS = &TypeDescriptor_PlayLineParams;

static const TypeDescriptorField PlayConvLineMessage_Fields[6] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayConvLineMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayConvLineMessage, annSpeaker), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(PlayConvLineMessage, annParticipants), NULL, 1, 9, 0),
    TypeDescriptorField("", DT_PLAY_LINE_PARAMS, offsetof(PlayConvLineMessage, tParams), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayConvLineMessage, dwTimeToDisplay), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayConvLineMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_PlayConvLineMessage("PlayConvLineMessage", 0, PlayConvLineMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PLAY_CONV_LINE_MESSAGE = &TypeDescriptor_PlayConvLineMessage;

static const TypeDescriptorField AdvanceConvMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(AdvanceConvMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AdvanceConvMessage, snoConv), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AdvanceConvMessage, nLineUID), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AdvanceConvMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AdvanceConvMessage("AdvanceConvMessage", 0, AdvanceConvMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ADVANCE_CONV_MESSAGE = &TypeDescriptor_AdvanceConvMessage;

static const TypeDescriptorField UpdateConvAutoAdvanceMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(UpdateConvAutoAdvanceMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(UpdateConvAutoAdvanceMessage, snoConv), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(UpdateConvAutoAdvanceMessage, nLineUID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(UpdateConvAutoAdvanceMessage, dwNextAdvanceTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UpdateConvAutoAdvanceMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UpdateConvAutoAdvanceMessage("UpdateConvAutoAdvanceMessage", 0, UpdateConvAutoAdvanceMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_UPDATE_CONV_AUTO_ADVANCE_MESSAGE = &TypeDescriptor_UpdateConvAutoAdvanceMessage;

static const TypeDescriptorField StopConvLineMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(StopConvLineMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(StopConvLineMessage, nConvLineGUID), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(StopConvLineMessage, fStoppedByUser), NULL, NULL, (const void *)0x1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(StopConvLineMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_StopConvLineMessage("StopConvLineMessage", 0, StopConvLineMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_STOP_CONV_LINE_MESSAGE = &TypeDescriptor_StopConvLineMessage;

static const TypeDescriptorField EndConversationMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(EndConversationMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EndConversationMessage, snoConversation), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EndConversationMessage, annSpeaker), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EndConversationMessage, nPlayerTalkingIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EndConversationMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EndConversationMessage("EndConversationMessage", 0, EndConversationMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_CONV_END_MESSAGE = &TypeDescriptor_EndConversationMessage;

