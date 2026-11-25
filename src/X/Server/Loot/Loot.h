// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Loot/Loot.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/GameBalance.h"
#include "../../Common/Items.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XArray.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"

class ActorCommonData;
struct RNG;
struct ItemAffixSpecifier;  // X/Common/Items.cpp
struct AffixTableEntry;  // X/Common/Items.cpp
typedef int32 HirelingClass;  // X/Common/Items.h

enum {
    LOOT_MAX_AFFIXES = 6,
    LOOT_MAX_SPECIFIERS = 128,
};

enum {
    LC_UNKNOWN = 0,
    LC_TREASUREBAG = 1,
    LC_RANDOMITEMVENDOR = 2,
};

typedef int32 LootContext;

typedef int32 ItemQualityLevel;

struct SharedLooteeParams {  // size 0x54
    // Line 40.
    SharedLooteeParams()
    {
        nLevel = -1;
        nForcedILevel = -1;
        nTieredLootRunLevel = -1;
        nAncientChanceHandicapLevelOverride = -1;
        memset(&fIsVendor, 0, sizeof(SharedLooteeParams) - offsetof(SharedLooteeParams, fIsVendor));
    }
    int32 nLevel;  // 0x00
    int32 nForcedILevel;  // 0x04
    int32 nTieredLootRunLevel;  // 0x08
    int32 nAncientChanceHandicapLevelOverride;  // 0x0c
    BOOL fIsVendor;  // 0x10
    BOOL fIsTreasureBag;  // 0x14
    BOOL fIsCrafter;  // 0x18
    BOOL bSuppressMagicAndGoldFind;  // 0x1c
    BOOL bSuppressUnluckyBonus;  // 0x20
    BOOL bSuppressTieredRiftBonuses;  // 0x24
    BOOL bDisableTormentDrops;  // 0x28
    BOOL bDisableTormentCheck;  // 0x2c
    BOOL bIgnoreLevelRestriction;  // 0x30
    BOOL bIgnoreClassLegendaryDropWeight;  // 0x34
    BOOL bIgnoreKnownRecipeCheck;  // 0x38
    BOOL bForceCrafterAncientLegendaryChance;  // 0x3c
    BOOL bDisableAncientDrops;  // 0x40
    BOOL bDisablePrimalAncientDrops;  // 0x44
    BOOL bDisableNewCraftingDrops;  // 0x48
    BOOL bSuppressGiftGeneration;  // 0x4c
    int32 unk_50;  // 0x50 (2.7.7; zeroed by the memset)
};
static_assert(sizeof(SharedLooteeParams) == 0x54, "SharedLooteeParams");

struct LooteeInfo {  // size 0x78
    LooteeInfo() {}  // line 90
    SNO snoTreasureClass;  // 0x00
    SharedLooteeParams tLooteeParams;  // 0x04
    float flNoDropScalar;  // 0x58
    SNO snoActor;  // 0x5c
    SNO snoLevelArea;  // 0x60
    MonsterRarity eMonsterRarity;  // 0x64
    Act eQuestAct;  // 0x68
    uint32 tDebugName;  // 0x6c
    GBHandle hGBDebug;  // 0x70
    void InitializeFromACD(const ActorCommonData & tACDLootee, const ACDID idACDLooter);
};
static_assert(sizeof(LooteeInfo) == 0x78, "LooteeInfo");

struct LootSpecifier {  // size 0xf0
    LootSpecifier();
    void AddAffix(GBID gbidAffix);
    void AddRandomAffixes(AffixType & eLastAffixType, int32 nMajorAffixes, int32 nMinorAffixes, int32 nRandomAffixes);
    void ClearAffixes();
    BOOL IsTrashEligible();
    void FillInItemAffixSpecifier(ItemAffixSpecifier & tSpecifier) const;
    void FillInTargetedClass(PlayerClass eDesiredPlayerClass, RNG * ptRNGRandomClass, BOOL bCheckLegendaryClassDropWeight);
    SNO snoActorForFlippy;  // 0x00
    GBHandle hGBItem;  // 0x04
    uint32 nGoldAmount;  // 0x0c
    float flGoldMultiplier;  // 0x10
    ItemQualityLevel eIQL;  // 0x14
    int32 nAffixLevel;  // 0x18
    int32 nLegendaryItemLevel;  // 0x1c
    PlayerClass eTargetedPlayerClass;  // 0x20
    HirelingClass eTargetedHirelingClass;  // 0x24
    SharedLooteeParams tLooteeParams;  // 0x28
    BOOL fAutoIdentify;  // 0x7c
    GBID arAffixGBIDs[6];  // 0x80
    int32 nNumAffixes;  // 0x98
    blz::unordered_map<int, int> unk_a0;  // 0xa0 (2.7.7: default-constructed with 8 buckets, freed when an XArray entry is removed; node key/value are 32-bit)
    uint64 uDesiredStackCount;  // 0xc0
    int32 nMinLegendaryAffixes;  // 0xc8
    BOOL fAccountBound;  // 0xcc
    BOOL fBindOnEquip;  // 0xd0
    GBID gbidAllowedLegendaryAffixFamily;  // 0xd4
    AncientRank eAncientRank;  // 0xd8
    BOOL bAddExtraMajorAffix;  // 0xdc
    BOOL unk_e0;  // 0xe0 (2.7.7: when set, LootDetermineRandomAffixes trims the random affix counts so that they plus nNumAffixes stay at most 5)
    BOOL bIgnoreGoldRange;  // 0xe4 (sGetLootSpecifiers passes [spec+0xe4] to sLootFillGoldAmount at 0x88ba24)
    BOOL bTryStack;  // 0xe8 (transmute code stores 1 at spec+0xe8 next to uDesiredStackCount, 0x8832fc)
    int32 unk_ec;  // 0xec
};
static_assert(sizeof(LootSpecifier) == 0xf0, "LootSpecifier");

typedef XArray<LootSpecifier, const LootSpecifier &, 128> LootSpecifierArray;

struct SharedLootManager {  // size 0x88
    SharedLootManager();
    SNOArray arConditionStack;  // 0x00
    SNOArray arEvaluatedTreasureClasses;  // 0x38
    const ActorCommonData * ptACDLootee;  // 0x70
    uint32 dwGameEventNumber;  // 0x78
    BOOL bAutoIdentify;  // 0x7c
    float flCameraBiasDist;  // 0x80
};
static_assert(sizeof(SharedLootManager) == 0x88, "SharedLootManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    40  SharedLooteeParams::SharedLooteeParams()  [inlined into 29 functions]
//   line    90  LooteeInfo::LooteeInfo()  [inlined into 16 functions]
//   line   164  SharedLootManager::~SharedLootManager()  [0x71007d9770]

struct WorldPlace;
void LootGiveTreasureToPlayer(const LooteeInfo & tLootee, ActorCommonData & tACDPlayer, XPooledDataIDList * listLootACDs, LootContext eLootContext, BOOL bTryStashFirst, BOOL bForceAccountBinding);  // Loot.cpp:5872
BOOL LootGetLootSpecifiers(const LooteeInfo & tLootee, const ACDID idACDLooter, LootSpecifierArray & arSpecifiers, const TreasureClassDefinition * pdefTreasureClass, const BOOL bIgnoreQuestItems, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot);  // Loot.cpp:3697
ACDID LootCreateItem(LootSpecifier & tSpecifier, const WorldPlace & tPlace, const ActorCommonData * ptACDSource);  // Loot.cpp:4339

void LootDetermineInherentAffixes(LootSpecifier & tSpecifier);

void LootModifyBeforeSpawning(LootSpecifierArray & arLootSpecifiersToSpawn, const ActorCommonData & tACDLootee, ACDID idLooter);  // Loot.cpp:6609

void LootDropHealth(const ACDID idACDLootee, uint32 dwGameEventNumber, const ACDID idACDKiller, GBID gbidItem);  // Loot.cpp:1112

void LootDropExplicitGold(const ActorCommonData & tACDLootee, const ACDID idACDAssigned, const int32 nGoldAmount);  // Loot.cpp:3656
