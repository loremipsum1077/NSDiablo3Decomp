// Original file: X/Server/Network/KnownList.h
// Subset written for X/Server/Network/KnownList.cpp.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"

struct Player;
class Frustum;

enum KnownEntryType {
    KET_INVALID = 0,
    KET_TEAM = 1,
    KET_WORLD_REVEAL = 2,
    KET_WORLD_ENTER = 3,
    KET_PREFETCH_SCENE = 4,
    KET_PREFETCH_ACD = 5,
    KET_SCENE = 6,
    KET_SUBSCENE = 7,
    KET_ACD = 8,
    KET_NUM_TYPES = 9,
};

struct KnownEntry {  // size 0x8
    KnownEntryType eType;  // 0x00
    DataID id;  // 0x04
    BOOL operator==(const KnownEntry & tEntry) const
    {
        return eType == tEntry.eType && id == tEntry.id;
    }
};
static_assert(sizeof(KnownEntry) == 0x8, "KnownEntry");

typedef int32 KnownListID;

enum {
    KNOWNENTRYLIST_INVALID = -1,
};

typedef int32 KnownEntryListID;

void KnownListInitializeForGame();
void KnownListDisposeForGame();
KnownListID KnownListAlloc();
void KnownListFree(KnownListID idKnownList);
KnownEntryListID KnownEntryListAlloc();
void KnownEntryListFree(KnownEntryListID idKnownEntryList);
void KnownListItemAdded(KnownEntryType eType, DataID idObject);
void KnownListItemDeleted(KnownEntryType eType, DataID idObject);
void KnownListItemChanged(KnownEntryType eType, DataID idObject, BOOL bCheckForDuplicate);
BOOL AddToKnownList(const KnownEntryType eType, const DataID idObject, KnownListID idKnownList, const Player & tPlayerKnownListOwner, BOOL bCheckDuplicates, BOOL bKnowInventory);
void KnownListBuild(Player & tPlayer, KnownListID idKnownList);
void KnownListBuildChangedLists(KnownListID idListOld, KnownListID idListNew, KnownEntryListID idListAdded, KnownEntryListID idListRemoved, KnownEntryListID idListIntersected);
void KnownListResetDeletedItems();
void KnownListResetAddedItems();
void KnownListResetChangedItems();
void KnownListClear(KnownListID idKnownList, BOOL bClearAcdKnownByPlayer, const Player * ptPlayer);
POS KnownEntryListGetFirst(KnownEntryListID idKnownEntryList);
KnownEntry & KnownEntryListGetNext(KnownEntryListID idKnownEntryList, POS & pos);
void KnownListProcessAddedEntries(Player * ptPlayer, KnownEntryListID idList);
void KnownListProcessDeletedEntries(Player * ptPlayer, KnownEntryListID idList);
void KnownListForceUpdateForAllPlayers();
void KnownListForceUpdate(Player & tPlayer);
