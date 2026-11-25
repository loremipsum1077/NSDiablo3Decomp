// Recovered from original DWARF; field names, types and offsets verified.
// Original file: X/Client/Network/CTrickle.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Gfx/GfxCommon.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/DataStructures/XPooledList.h"

typedef int32 TrickleInfoType;

struct ClientTrickleEntry {  // size 0x6c
    ACDNetworkName ann;  // 0x00
    SNO snoActor;  // 0x04
    WorldPlace tPlace;  // 0x08
    int32 nPlayerIndexOwner;  // 0x18
    SNO snoLevelArea;  // 0x1c
    float flHealthPercent;  // 0x20
    TrickleInfoType eType;  // 0x24
    uint32 dwTrickleFlags;  // 0x28
    ImageTextureFrame tIcon;  // 0x2c
    SNO snoMinimapStringlist;  // 0x34
    uint32 hStringListLabel;  // 0x38
    float fMinMinimapDistSq;  // 0x3c
    float fMaxMinimapDistSq;  // 0x40
    float flLastInjuredTime;  // 0x44
    int32 dwHeadstoneCorpseReviveTime;  // 0x48
    uint32 uEngagedWithRareTime;  // 0x4c
    uint32 uEngagedWithGoblinTime;  // 0x50
    BOOL bInCombat;  // 0x54
    int32 nTieredRiftRank;  // 0x58
    BOOL bBrawling;  // 0x5c
    BOOL bInUberFight;  // 0x60
    int32 gbidPortraitFrame;  /* 0x64 */  int32 unk_68;  // 0x68 (2.7.7: stored by CTrickleUpdate)
};
static_assert(sizeof(ClientTrickleEntry) == 0x6c, "ClientTrickleEntry");

typedef XPooledList<ClientTrickleEntry, const ClientTrickleEntry &> ClientTrickleList;


// Functions defined in this file (bodies are not in the DWARF):
//   line    22  ClientTrickleEntry::ClientTrickleEntry()  [inlined into 1 functions]
//   line    22  ClientTrickleEntry & ClientTrickleEntry::operator=(const ClientTrickleEntry &)  [inlined into 1 functions]

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(ClientTrickleEntry) == 0x6c, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, ann) == 0x0, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, snoActor) == 0x4, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, tPlace) == 0x8, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, nPlayerIndexOwner) == 0x18, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, snoLevelArea) == 0x1c, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, flHealthPercent) == 0x20, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, eType) == 0x24, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, dwTrickleFlags) == 0x28, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, tIcon) == 0x2c, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, snoMinimapStringlist) == 0x34, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, hStringListLabel) == 0x38, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, fMinMinimapDistSq) == 0x3c, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, fMaxMinimapDistSq) == 0x40, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, flLastInjuredTime) == 0x44, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, dwHeadstoneCorpseReviveTime) == 0x48, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, uEngagedWithRareTime) == 0x4c, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, uEngagedWithGoblinTime) == 0x50, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, bInCombat) == 0x54, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, nTieredRiftRank) == 0x58, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, bBrawling) == 0x5c, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, bInUberFight) == 0x60, "ClientTrickleEntry");
static_assert(offsetof(ClientTrickleEntry, gbidPortraitFrame) == 0x64, "ClientTrickleEntry");
