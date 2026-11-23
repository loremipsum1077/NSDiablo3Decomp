// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SItems.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 ItemQualityLevel;

typedef int32 InventorySlotType;

typedef int32 DyeType;


#include "../Sigma/Unicode.h"
struct LootSpecifier;
ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, LootSpecifier & tSpecifier, const int32 nInherentAffixes);

#include "../Common/GameBalance.h"
class ActorCommonData;
void SItemApplyTemporaryTransmog(ActorCommonData & tACDItem, GBID gbidTransmog, ActorCommonData & tACDOwner);  // SItems.cpp:1304
void SItemRemoveTemporaryTransmog(ActorCommonData & tACDItem, ActorCommonData & tACDOwner);  // SItems.cpp:1338

namespace OnlineService { struct ItemId; }
namespace D3 { namespace Items { class Generator; } }
struct WorldPlace;
// No header declaration recovered; owning-header placement inferred from SItems.cpp:337.
ACDID SItemGenerate(const OnlineService::ItemId & tItemId, const D3::Items::Generator & tGenerator, const WorldPlace & tPlace, const float flScale, uint32 tName);
