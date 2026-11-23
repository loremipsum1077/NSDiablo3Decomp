// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SNPC.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"

class ActorCommonData;
struct Player;

typedef int32 NPCMenu;

typedef int32 HirelingClass;

typedef blz::vector<int, blz::allocator<int> > ConversationArray;

// Declared here from their definitions in X/Server/SNPC.cpp.
void NPCSetTalkTarget(ActorCommonData & tACDNPC, ActorCommonData * ptACDTarget);  // SNPC.cpp:1032
void NPCVendorStop(Player & tPlayer);  // SNPC.cpp:1148

BOOL HelperIsLevelAreaAllowed(const ActorCommonData & tACD, SNO snoLevelArea);  // SNPC.cpp

SNO NPCGetConversationsFromList(SNO snoConvList, ACDID idACDPlayer, ACDID idACDNPC, ConversationArray * ptConvArray);
