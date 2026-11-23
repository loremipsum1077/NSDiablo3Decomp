// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/NPC.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"
#include "CommonTypes.h"

typedef int32 ConversationIcon;

typedef int32 CrafterType;

typedef int32 OpenWorldMode;

enum {
    NIT_INVALID = 0,
    NIT_CANCEL = 1,
    NIT_TALK_IMPORTANT = 2,
    NIT_TALK_DIALOG = 3,
    NIT_PET_DISMISS = 4,
    NIT_CRAFT = 5,
    NIT_IDENTIFY = 6,
    NIT_HIRE = 7,
    NIT_INVENTORY = 8,
    NIT_CUSTOMIZATION = 9,
    NIT_EQUIPMENT_MANAGER = 10,
    NIT_NUM_TYPES = 11,
};

typedef int32 NPCInteractionType;

enum {
    NM_DEFAULT = 0,
    NM_TALK = 1,
    NM_COUNT = 2,
};

typedef int32 NPCMenu;

enum NPCOptionBit {
    NPC_OPTION_ENABLED = 0,
    NPC_OPTION_CONVERSATION_HEARD = 1,
    NPC_OPTION_COUNT = 2,
};

struct NPCInteraction {  // size 0x10
    NPCInteractionType eType;  // 0x00
    uint32 dwData;  // 0x04
    uint32 dwData2;  // 0x08
    uint32 dwFlags;  // 0x0c
};
static_assert(sizeof(NPCInteraction) == 0x10, "NPCInteraction");

struct NPCInteractOptionsMessage {  // size 0x1f4
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName annTalkingTo;  // 0x08
    NPCInteraction tNPCInteraction[30];  // 0x0c
    int32 nNumInteractions;  // 0x1ec
    NPCMenu ePreferredMenu;  // 0x1f0
};
static_assert(sizeof(NPCInteractOptionsMessage) == 0x1f4, "NPCInteractOptionsMessage");

typedef int32 LevelAreaID;

// Declared here from its definition in X/Common/NPC.cpp.
ACDID NPCIsAnyPlayerTalkingTo(ACDID idACDTalkingTo);  // NPC.cpp:246

class ActorCommonData;
BOOL NPCCanTrade(const ActorCommonData & tACD);  // NPC.cpp:191

class CRefString;
BOOL NPCGetUIName(const ActorCommonData & tACD, CRefString & usText);  // NPC.cpp:323
