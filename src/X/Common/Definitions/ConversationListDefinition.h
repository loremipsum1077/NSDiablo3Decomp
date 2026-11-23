// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/ConversationListDefinition.h
// Subset: the group definition class and its global, which X/Common/Game.cpp registers
// (GroupDefinition::Create<ConversationListGroupDef>). The definition struct itself is not adopted yet.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../X/Common/Definitions/Definitions.h"
#include "../../../X/Common/SNO/SNOFiles.h"
#include "../../../X/Sigma/SigmaTypes.h"

class XMemoryAllocatorInterface;

class ConversationListGroupDef : public GroupDefinition {  // size 0xa0
public:
    ConversationListGroupDef();
    GROUPDEFINITION_MEMORY_FUNCTIONS()  // vtable slots 2-5; the macro name is invented, see Definitions.h
    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return TRUE; }  // vtable slot 13
    virtual SNOPackType GetSNOPackDestination(const void *) const { return SNOPACKTYPE_SERVERDATA; }  // vtable slot 7
};
static_assert(sizeof(ConversationListGroupDef) == 0xa0, "ConversationListGroupDef");

extern ConversationListGroupDef * gptGroupDefConversationList;  // ConversationListDefinition.cpp

struct ConversationListEntry {  // size 0x14c
    SNO snoConv;  // 0x00
    int32 eConditionReqs;  // 0x04 (ConversationConditionReqs)
    uint32 dwFlags;  // 0x08
    int32 eCrafterType;  // 0x0c (CrafterType)
    GBID gbidItem;  // 0x10
    CHAR szLabel[128];  // 0x14
    CHAR szPlayerFlag[128];  // 0x94
    SNO snoQuestCurrent;  // 0x114
    int32 nStepUIDCurrent;  // 0x118
    SNO snoQuestAssigned;  // 0x11c
    SNO snoQuestActive;  // 0x120
    SNO snoQuestComplete;  // 0x124
    SNO snoQuestRange;  // 0x128
    SNO arLevelArea[6];  // 0x12c
    int32 eSpecialEventFlag;  // 0x144 (SpecialEventFlag)
    uint32 uClassFlags;  // 0x148
};
static_assert(sizeof(ConversationListEntry) == 0x14c, "ConversationListEntry");

struct ConversationListDefinition {  // size 0x20
    RequiredDefinitionHeader tHeader;  // 0x00
    XPtr64<ConversationListEntry> ptConvList;  // 0x10
    SerializeData serConvList;  // 0x18
};
static_assert(sizeof(ConversationListDefinition) == 0x20, "ConversationListDefinition");

typedef ScopedDefinitionLockR<ConversationListDefinition> ScopedConversationListDefinitionLockR;

// line 151. LockRFast with an empty unlock: no UnlockR appears in SNPC.cpp NPCGetConversationsFromList.
template <> inline const ConversationListDefinition * DefinitionLockR<ConversationListDefinition>(SNO sno) {
    return (const ConversationListDefinition *)gptGroupDefConversationList->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<ConversationListDefinition>(const ConversationListDefinition *) {}

enum {
    CLE_CRAFTER_UNLOCK_NEGATE_BIT = 0,
    CLE_GAME_FLAG_NEGATE_BIT = 1,
    CLE_HAS_ITEM_NEGATE_BIT = 2,
    CLE_QUEST_STEP_CURRENT_NEGATE_BIT = 3,
    CLE_QUEST_ASSIGNED_NEGATE_BIT = 4,
    CLE_QUEST_COMPLETE_NEGATE_BIT = 5,
    CLE_QUEST_RANGE_NEGATE_BIT = 6,
    CLE_NEAR_HOSTILES_NEGATE_BIT = 7,
    CLE_IS_NEAR_HOSTILES_BIT = 8,
    CLE_CONVERSATION_UNLOCKED_NEGATE_BIT = 9,
    CLE_IS_CONVERSATION_UNLOCKED_BIT = 10,
    CLE_CONVERSATION_HEARD_NEGATE_BIT = 11,
    CLE_CONVERSATION_HEARD_BIT = 12,
    CLE_LEVEL_AREA_NEGATE_BIT = 13,
    CLE_CONVERSATION_HAS_ALL_PARTICIPANTS_NEGATE_BIT = 14,
    CLE_CONVERSATION_HAS_ALL_PARTICIPANTS_BIT = 15,
    CLE_QUEST_ACTIVE_NEGATE_BIT = 16,
    CLE_DIVIDER_BIT = 17,
    CLE_PLAYER_FLAG_NEGATE_BIT = 18,
    CLE_SPECIAL_EVENT_FLAG_NEGATE_BIT = 19,
    CLE_ADVENTURE_MODE_BIT = 20,
    CLE_ADVENTURE_MODE_NEGATE_BIT = 21,
    CLE_DUNGEON_FINDER_BIT = 22,
    CLE_DUNGEON_FINDER_NEGATE_BIT = 23,
    CLE_REQUIRES_DUNGEON_FINDER_PAYMENT_BIT = 24,
    CLE_REQUIRES_DUNGEON_FINDER_PAYMENT_NEGATE_BIT = 25,
};

struct Player;
class ActorCommonData;
BOOL ConversationListEntryEvaluate(const ConversationListEntry & tEntry, const Player & tPlayer, const ActorCommonData * ptACDNPC);  // ConversationListDefinition.cpp:352
