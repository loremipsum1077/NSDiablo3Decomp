// Original file: X/Server/SCosmeticItems.h. The server side of learning and equipping cosmetics.
#pragma once

#include "../Common/CommonTypes.h"
#include "../Common/CosmeticItems.h"

struct Player;

enum {
    SECONDS_PER_DAY = 86400,
};

BOOL SCosmeticItems_LearnCosmeticItem(const ACDID idACDPlayer, const GBID gbidItem, CosmeticItemType eType, bool bPurchased);
BOOL SCosmeticItems_LearnPet(const ACDID idACDPlayer, const GBID gbidItem, bool bPurchased);
void SCosmeticItems_SendPlayerCosmeticItemInfo(const Player & tPlayer);
BOOL SCosmeticItems_EquipCosmeticItem(Player & tPlayer, const GBID gbidItem);
BOOL SCosmeticItems_UnequipCosmeticItem(Player & tPlayer, CosmeticItemType eType);
