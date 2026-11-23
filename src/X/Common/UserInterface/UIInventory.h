// X/Common/UserInterface/UIInventory.h
// Minimal declarations from build/dwarf/headers/X/Common/UserInterface/UIInventory.h.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../Client/Gfx/GfxCommon.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../GameBalance.h"
#include "../Items.h"
#include "../ItemsSwig.h"
#include "../Player.h"
#include "../../Sigma/RefString.h"

class ActorCommonData;
struct InventoryLocation;

namespace D3 { namespace Items { class Generator; } }

struct ItemTooltipAlternateInventory {  // size 0x28
    GBID gbidLegendaryPowers[3];  // 0x00
    blz::vector<const D3::Items::Generator *, blz::allocator<const D3::Items::Generator *> > tItems;  // 0x10
    ItemTooltipAlternateInventory();
};
static_assert(sizeof(ItemTooltipAlternateInventory) == 0x28, "ItemTooltipAlternateInventory");

typedef int32 TooltipType;

struct ItemTooltipParameterBlock {  // size 0x258
    // The constructor and destructor are implicit: the DWARF records both at line 29, the struct's own line.
    TooltipType eTooltipType;  // 0x00
    ItemEffectType eItemEffectType;  // 0x04
    ItemQualityLevel eIQL;  // 0x08
    uint32 dwTooltipFlags;  // 0x0c
    UIItemOverlay eOverlay;  // 0x10
    ImageTextureFrame tImageTexFrame;  // 0x14
    CRefString sName;  // 0x20
    CRefString sType;  // 0x38
    CRefString sStats;  // 0x50
    CRefString sRating;  // 0x68
    CRefString sSpecialStats;  // 0x80
    CRefString sSetItem;  // 0x98
    CRefString sFlavor;  // 0xb0
    CRefString sTradableTo;  // 0xc8
    CRefString sCost;  // 0xe0
    CRefString sDurability;  // 0xf8
    CRefString sClassReqs;  // 0x110
    CRefString sItemLevel;  // 0x128
    CRefString sRequirements;  // 0x140
    CRefString sInstruction;  // 0x158
    CRefString sSlot;  // 0x170
    CRefString sSocketCost;  // 0x188
    CRefString sRepairCost;  // 0x1a0
    CRefString sTransmog;  // 0x1b8
    CRefString sDye;  // 0x1d0
    CRefString sSocketStats[3];  // 0x1e8
    GBID sSocketGBIDs[3];  // 0x230
    ImageTextureFrame tSocketImageFrames[3];  // 0x23c
};
static_assert(sizeof(ItemTooltipParameterBlock) == 0x258, "ItemTooltipParameterBlock");

struct ItemTooltipPlayerData {  // size 0x18
    ItemTooltipPlayerData(const ActorCommonData *, const ItemTooltipAlternateInventory *);
    ItemTooltipPlayerData();
    PlayerClass ePlayerClass;  // 0x00
    BOOL fIsMale;  // 0x04
    const ActorCommonData * ptACDPlayer;  // 0x08
    const ItemTooltipAlternateInventory * ptAlternateInventory;  // 0x10
};
static_assert(sizeof(ItemTooltipPlayerData) == 0x18, "ItemTooltipPlayerData");

struct SocketCallbackData {  // size 0x10
    ItemTooltipParameterBlock * ptParams;  // 0x00
    const ItemTooltipPlayerData * ptTooltipPlayerData;  // 0x08
};
static_assert(sizeof(SocketCallbackData) == 0x10, "SocketCallbackData");

struct SetTooltipInfo {  // size 0x40
    blz::string setItemTooltipName;  // 0x00
    blz::vector<blz::pair<int, blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > >, blz::allocator<blz::pair<int, blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > > > > itemTooptipPairs;  // 0x28
};
static_assert(sizeof(SetTooltipInfo) == 0x40, "SetTooltipInfo");

CRefString GetSlotTooltip(GBID gbidItem);  // UIInventory.cpp:565

namespace UIInventory {

LPCSTR ItemGetLabelForSlot(const InventorySlotType eSlot, const BOOL fIs2Handed);
CRefString GetSetItemTooltipName(const GBID gbidSet);

CRefString GetSetItemTooltip(const ActorCommonData * ptACDPlayer, const PlayerClass ePlayerClass, const ActorCommonData & tACDItem, const blz::unordered_map<int, SetTooltipInfo, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, SetTooltipInfo> > > * pPrecachedSetItemTooltipStrings);
CRefString GetSetItemTooltip(const ActorCommonData * ptACDOwner, const ItemTooltipAlternateInventory * ptAlternateInventory, const PlayerClass ePlayerClass, const ActorCommonData & tACDItem, const blz::unordered_map<int, SetTooltipInfo, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, SetTooltipInfo> > > * pPrecachedSetItemTooltipStrings);
void GetHighestTypeEffectFromSocket(const GBID gbidItemToSocket, const GBHandle hGBItemSocketedInto, GBHandle & hGBHighestTypeEffect);
BOOL PopulateSocketData(const InventoryLocation & tInvLoc, void * pCallBackData);
void GetAttributesFromSocketEffect(GBHandle hGBEffect, GBID gbidItem, AttribGroupID idAttribGroup, CRefString * pstrOut);

}  // namespace UIInventory
