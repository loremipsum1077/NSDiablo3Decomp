// Original file: X/Server/Loot/Loot.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).
#include "../../Common/Definitions/TreasureClassDefinition.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "Loot.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SPlayer.h"
#include "../PlayerList.h"
#include "../../Common/Player.h"
#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Items.h"
#include "../../Common/Monsters.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Shell/CmdLine.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Common/GameConstants.h"
#include "../../Common/Definitions/HeroDefinition.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Common/Console/Avenger.h"
#include "../../Common/Crafting/Salvage.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/NPC.h"
#include "../../Common/Crafting/ItemCraftingSwig.h"
#include "../../Common/Crafting/ItemCrafting.h"
#include "../Actors/Actors.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Sigma/MemoryPools.h"
enum {
    GB_AFFIXES = 8, GB_ITEM_QUALITY_CLASSES = 27,  // X/Common/Definitions/GameBalanceDefinition.h:49, not adopted there yet (27 is also local in GameBalanceDefinition.cpp:57)
};
#include "../../Common/Networking/GameMessage.h"
enum {
    LOOT_TRACE_BUFFER_SIZE = 65536,
    ITEM_WEIGHT_ARRAY_MAX_SIZE = 1024,
    RARE_NAME_ARRAY_MAX_SIZE = 256,
    ITEM_TRADE_TICKS = 432000,
};

struct CatchupTreasureClass {  // size 0xc
    GlobalSNO eGlobalSNO;  // 0x00
    float flChance;  // 0x04
    uint32 uDelayTicks;  // 0x08
};

struct PlayerHasItemCallbackData {  // size 0x8
    GBID gbidItem;  // 0x00
    BOOL bFound;  // 0x04
};

struct BossMonsterPlayerKilledFlags {  // size 0x8
    GlobalSNO globalSNOMonster;  // 0x00
    uint32 dwFlag;  // 0x04
};







































// Partial copies of X/Common/Items.cpp's structs (real home X/Common/Items.h, not adopted yet).
struct Affix;
struct ItemType;
struct Item {  // size 0x38
    GBID gbid;  // 0x00
    uint32 dwFlags;  // 0x04
    int32 nItemLevel;  // 0x08
    Act eMinAct;  // 0x0c
    ItemType * ptItemType;  // 0x10
    int32 * pnDropWeights;  // 0x18
    int32 * pnVendorDropWeights;  // 0x20
    uint32 * pdwLabelMask;  // 0x28
    GBID gbidBaseItem;  // 0x30
    int32 nSeasonRequiredToDrop;  // 0x34
};
static_assert(sizeof(Item) == 0x38, "Item");
namespace Console {
namespace Online {
bool GetGBIDBlacklistData(GBID gbid, uint32 * flags);  // X/Client/Console/LobbyService.cpp:830
}  // namespace Online
}  // namespace Console
struct ItemAppGlobals {  // size 0x1d8
    ItemType tRootItemType; uint8 _unadopted0[0x68 - 0x58];  // 0x00, 0x58 (nItemTypes, ptItemTypes)
    int32 nItems;  // 0x68
    Item * ptItems;  // 0x70
    int32 nAffixes;  // 0x78
    Affix * ptAffixes;  // 0x80
    AffixTableEntry ** ptCachedAffixTableEntyPtrs;  // 0x88
    BOOL bCachedAffixTableEntriesValid; XGrowableMap<int, int, Item *, Item *> mapItems; uint8 _unadopted1[0x1d8 - 0x118];  // 0x90, 0x98, 0x118 (tLabelMaskAllocator .. end)
};
void ItemsValidateCachedAffixTableEntries();  // X/Common/Items.cpp:3255







void ACDSetRareItemName(ActorCommonData & tACD, const RareItemName & tRareItemName);  // X/Server/Actors/Actors.cpp:6074
int32 LootBuildValidAffixList(GBHandleList & lstAffixes, const LootSpecifier & tSpecifier, const AffixType eLastAffixType, const Affix * pPotentialAffixes, const int32 nPotentialAffixCount, AffixTableEntry ** ptPotentialAffixTableEntries, const uint32 dwFlags);

#include "../SGameConstants.h"  // below: declared here from their definitions elsewhere (headers not adopted yet).
void ItemGetValidTargetedClass(PlayerClass & eTargetedPlayerClass, HirelingClass & eTargetedHirelingClass, GBID gbidItem, PlayerClass eDesiredPlayerClass, RNG * ptRNGRandomClass, BOOL bCheckLegendaryClassDropWeight);  // X/Common/Items.cpp:2024
void ItemGetBonusRandomAffixes(GBHandle hgbItem, int32 & nMajor, int32 & nMinor, int32 & nRandom);  // X/Common/Items.cpp:5159
int32 ItemAffixGetFrequencyWeight(const AffixTableEntry * ptAffix, PlayerClass eTargetedPlayerClass, HirelingClass eTargetedHirelingClass);  // X/Common/Items.cpp:5721
BOOL ItemIsSetItem(const GBID gbid);  // X/Common/Items.cpp:9373
ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, LootSpecifier & tSpecifier, const int32 nInherentAffixes);  // X/Server/SItems.cpp:399
BOOL SPlayerGetHasFriends(const Player & tPlayer);  // X/Server/SPlayer.cpp:10684
ItemSellError ItemBaseCanBeSold(const GBHandle & hGB);  // X/Common/Items.cpp:3939
int32 ACDGetTieredLootRunLevel(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:9307
CrafterType NPCGetCrafterType(const ActorCommonData & tACD);  // X/Common/NPC.cpp
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // X/Common/Player.cpp
GBID CurrencyGetGBID(const CurrencyType eCurrencyType);  // X/Common/Items.cpp:10429
BOOL ItemIsLegendary(GBID gbidItem);  // X/Common/Items.cpp:5080
BOOL ItemIsEquippableByPlayerOrHireling(GBID gbidItem);  // X/Common/Items.cpp:9460
void TraceStat(LPCSTR sEventType, ACDID idACD, LPCSTR cpFmtStr, ...);  // X/Server/SGame.cpp:7255
void FlippiesCreateOnActor(const ActorCommonData & tACDSpawningActor, const ACDID idACDAssigned, const LootSpecifierArray & arLootSpecifiersToSpawn, const uint32 dwFlippyFlags, uint32 dwGameEventNumber, float flCameraBiasDist, const ACDID idACDKiller, const LooteeInfo * ptLootee, const Vector3D * pvForcedPositionBias, const LootContext eLootContext, XPooledDataIDList * plistSpawnedItemACDs);  // X/Server/Flippies.cpp:958

// Declared in Loot.h (functions defined below).
SNO ACDGetTreasureClass(const ActorCommonData & tACD, const ACDID idACDLooter);
BOOL LootGetLootSpecifiers(const LooteeInfo & tLootee, const ACDID idACDLooter, LootSpecifierArray & arSpecifiers, const TreasureClassDefinition * pdefTreasureClass, const BOOL bIgnoreQuestItems, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot);
void LootModifyBeforeSpawning(LootSpecifierArray & arLootSpecifiersToSpawn, const ActorCommonData & tACDLootee, ACDID idLooter);
int32 LootGetAffixFrequencyWeight(const AffixTableEntry * ptAffix, const LootSpecifier * ptSpecifier);
static BOOL sAffixExcludedFromAffixes(GBID gbidAffix, LootSpecifier & tSpecifier);
int32 ItemAffixGetNumSockets(GBID gbidItem, GBID gbidAffix);  // X/Common/Items.cpp:2474
static GBID sGetGoldGBIDFromAmount(uint32 nGoldAmount);  // out of line in 2.7.7 (0x7100894820)
SNO LootMonsterGetTreasureClass(const ActorCommonData & tACDMonster, const ACDID idACDLooter, const MonsterRarity eRarity);
BOOL LootMonsterSupportsFirstKillTreasureClass(SNO snoMonster);
void LootDropCatchupLoot(ActorCommonData & tACDVictim, Player * ptPlayer, uint32 dwGameEventNumber);
ACDID LootCreateItem(LootSpecifier & tSpecifier, const WorldPlace & tPlace, const ActorCommonData * ptACDSource); extern "C" int32 sub_710050e990(GBID gbidItem); extern "C" BOOL sub_710050e910(GBID gbidItem);
void LootDetermineInherentAffixes(LootSpecifier & tSpecifier);
void LootDetermineRandomAffixes(LootSpecifier & tSpecifier); void LootDetermineRareName(ActorCommonData & tACD, const LootSpecifier & tSpecifier);
BOOL ItemIsConsumablePotion(const GBID gbidItem); BOOL ItemIsSpecial(GBID gbidItem); BOOL ItemCanBeMagical(GBID gbidItem); GBID ItemsChooseLevelAppropriateItemFromBaseItem(GBID gbidBaseItem, int32 nItemLevel);  // X/Common/Items.cpp
void SGameStatsRecordLoot(ActorCommonData & tACDItem, LootContext eLootContext, const LooteeInfo * ptLootee);  /* X/Server/SGameStats.cpp:786 */ BOOL SACDInventoryPickupOrSpillOntoGround(ActorCommonData & tACDPlayer, ActorCommonData & tACDItem, const BOOL bTryStashFirst, ACDID * pidACDStackDest, const BOOL bAllowAutoEquip);
BOOL SCosmeticItems_LearnCosmeticItem(const ACDID idACDPlayer, const GBID gbidItem, CosmeticItemType eType, bool bPurchased); BOOL SCosmeticItems_LearnPet(const ACDID idACDPlayer, const GBID gbidItem, bool bPurchased);  // X/Server/SCosmeticItems.h
ACDID SItemGenerate(const OnlineService::ItemId & tItemId, const D3::Items::Generator & tGenerator, const WorldPlace & tPlace, const float flScale, uint32 tName);  // X/Server/SItems.cpp:337
void LootDetermineMajorMinorAffixCount(LootSpecifier & tSpecifier, int32 & nMajor, int32 & nMinor, int32 nTotalAffixes, int32 nInherentMajor, int32 nInherentMinor); static const LootDistEntry * sGetLootDistEntry(const int32 nLooteeLevel, const GameBalanceDefinition *& pLootDistDef, int32 nOverrideLevel); float LootGetFindValue(const ActorCommonData & tACDPlayer, Attrib eAttrib); float HirelingGetDiminishedAttribute(ACDID idACDPlayer, Attrib eAttrib); const GameBalanceDefinition * LootDistributionTableLockR(); uint64 PlayerSubtractFromDailyPlatinum(Player & tPlayer, uint64 uAmount); PlayerClass PlayerGetPlayerClass(const ACDID idACD); BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight); BOOL ItemHasLabelMask(const Item & tItem, const uint32 * pdwItemLabelMask); extern const SymbolicEntry gItemLabels[]; const char * SymbolToName(int32 nSymbol, const SymbolicEntry * pTable); BOOL ConditionEvaluate(const SNO snoCondition, const Player * ptPlayer, const ACDID idACD, const LooteeInfo * ptLooteeInfo); typedef blz::vector<CRefString, blz::allocator<CRefString> > XStringArray; static BOOL sGetLootSpecifiers(const LooteeInfo & tLootee, const ACDID idACDLooter, const TreasureClassDefinition & defTreasureClass, const GBID gbidInheritedQualityClass, LootSpecifierArray & arSpecifiers, XStringArray & tTCStack, SNO snoEvaluatedCondition, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot); static BOOL sGetSpecifiersFromModifier(const LooteeInfo & tLootee, const ACDID idACDLooter, const LootDropModifier & tModifier, const GBID gbidInheritedQualityClass, const SNO snoTreasureClass, LootSpecifierArray & arSpecifiers, XStringArray & tTCStack, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot); static const ItemLevelModifier * sGetItemLevelModifiers(const int32 nLooteeLevel, const GBID gbidItem, const GameBalanceDefinition *& pGBDef, BOOL & fAlwaysMagic); static void sRollForTargetedItem(const Player * ptPlayer, LootSpecifier & tSpecifier, BOOL bTargetRandomClass); void LootGetSpecifierForSpecifiedItem(LootSpecifier & tSpecifier, const ItemSpecifierData & tSpecificItem, const SharedLooteeParams & tLooteeParams, const Player * ptPlayer, BOOL bTargetRandomClass); const Item * ItemsGetItem(GBID gbidItem); int32 ItemGetItemLevel(const GBID gbidItem); CRefString GBIDToString(const GBHandle & hGB, const BOOL fUsedForXMLFields); void LootRollForAncientLegendary(LootSpecifier & tSpecifier, const ACDID idACDLooter); BOOL SPlayerIsInRift(const ActorCommonData & tACDPlayer); static GBID sPickLegendary(ActorCommonData * ptACDLooter, const SharedLooteeParams & tLooteeParams, PlayerClass ePlayerClass, ItemLabel eItemLabel1, ItemLabel eItemLabel2, BOOL bIgnoreClassLegendaryDropWeight); static BOOL sSpecifierArrayHasExistingGBID(const GBID gbidItem, const LootSpecifierArray & arSpecifiers); static void sDropSharedLootFromModifier(const LooteeInfo & tLootee, const TreasureClassDefinition & defTreasureClass, const LootDropModifier & tLootDropModifier, const GBID gbidInheritedQualityClass, SharedLootManager * ptSharedLoot); static uint64 sModifyQuantityForSpecificItem(const GBHandle hgbItem, uint64 uQuantity, const ActorCommonData & tACDLooter, const LooteeInfo & tLootee); static void sTryStackNewEntry(LootSpecifierArray & arSpecifiers, int32 nOldSpecifiersCount, const LootDropModifier & tLootDropModifier, ActorCommonData & tACDLooter); static float sGetOneOfModifierWeightForSinglePlayer(const LootDropModifier & tLootDropModifier, const Player * ptPlayer, const ActorCommonData * ptACDLooter, const LooteeInfo & tLootee, const LootSpecifierArray & arSpecifiers); static void sEvaluateAndGetSpecifiersFromModifier(const LooteeInfo & tLootee, const ACDID idACDLooter, const TreasureClassDefinition & defTreasureClass, const LootDropModifier & tLootDropModifier, const GBID gbidInheritedQualityClass, LootSpecifierArray & arSpecifiers, XStringArray & tTCStack, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot); static float sGetLegendaryMagicFindModifier(const ActorCommonData & tACDLooter); static int32 sLootFillGoldAmount(LootSpecifier & tSpecifier, const int32 nLooteeLevel, const ACDID idACDLooter, const BOOL bIgnoreGoldRange); static void sRemoveConsumablePotions(LootSpecifierArray & arLootSpecifiersToSpawn); CRefString GBHandleToString(const GBHandle & hGB); int32 PlayerGetNumberPlayersSharingGameConnection(const Player * ptPlayer); BOOL ItemUsableByHireling(const GBID gbidItem, const HirelingClass eHirelingClass); ItemQualityLevel ItemDetermineQualityLevel(LPCSTR szDebugStr, GBHandle hGBItem, const GBID * ptGBIDAffixes, const int32 nAffixCount, const int32 nBonusRandomAffixes); static void sUprankAffixByAncientRank(GBID & gbidAffix, const LootSpecifier & tSpecifier); int32 sItemGetAffixLevel(GBHandle hGBItem);
BOOL LootDropFriendGift(const Player * ptPlayer, const BOOL bAvenger, ItemQualityLevel eIQL); void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags); BOOL ItemTypeIsOfType(GBID gbidItemType, GBID gbidItemTypeToCheckAgainst); GBID LootChooseAffixFromList(const LootSpecifier & tSpecifier, AffixType & eLastAffixType, const GBHandleList & lstAffixes, const int32 nMaxWeight); GBHandle ItemsGetItemTypeGB(const GBHandle hGBItem); GBID ItemGetSet(const GBID gbid); BOOL ItemTypeMeetsOwnerClassRestrictions(const GBID gbidItemType, const PlayerClass ePlayerClass);
void LooteeInfo::InitializeFromACD(const ActorCommonData & tACDLootee, const ACDID idACDLooter)
{
    CrafterType eCrafterType;

    tDebugName = tACDLootee.tName;
    snoActor = SNO_NONE;
    eQuestAct = -1;

    if (tACDLootee.IsItem())
    {
        const GameBalanceDefinition * pdefGBItems;
        ItemDefinition * pdefItem = (ItemDefinition *)GBLockR(tACDLootee.hGB, pdefGBItems, TRUE);

        int32 nLegendaryItemLevelOverride = tACDLootee.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
        if (nLegendaryItemLevelOverride > 0)
        {
            tLooteeParams.nLevel = nLegendaryItemLevelOverride;
        }
        else
        {
            tLooteeParams.nLevel = pdefItem->nItemLevel;
        }

        GBUnlockR(pdefGBItems);
        flNoDropScalar = 0.0f;
        snoLevelArea = SNO_NONE;
        eMonsterRarity = -1;
    }
    else
    {
        tLooteeParams.nLevel = tACDLootee.AttributesGetInt(ATTRIBUTE_LEVEL);
        ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
        if (ptACDLooter)
        {
            int32 nLooterLevel = ptACDLooter->AttributesGetInt(ATTRIBUTE_LEVEL);
            tLooteeParams.nLevel = Math::Min(tLooteeParams.nLevel, nLooterLevel);
        }

        flNoDropScalar = MonsterGetNoDropScalar(tACDLootee.tSNOName);
        snoActor = tACDLootee.tSNOName.snoHandle;
        snoLevelArea = ACDGetLevelArea(tACDLootee);
        eMonsterRarity = tACDLootee.eMonsterRarity;
    }

    tLooteeParams.fIsVendor = FALSE;
    tLooteeParams.fIsCrafter = FALSE;
    tLooteeParams.nTieredLootRunLevel = -1;

    if (ACDIsInTieredLootRun(tACDLootee))
    {
        tLooteeParams.bDisableTormentCheck = TRUE;
        tLooteeParams.nTieredLootRunLevel = ACDGetTieredLootRunLevel(tACDLootee);
    }

    eCrafterType = NPCGetCrafterType(tACDLootee);
    if (eCrafterType == CRAFTER_TYPE_INVALID)
    {
        if (NPCCanTrade(tACDLootee)) tLooteeParams.fIsVendor = TRUE;
    }
    else
    {
        tLooteeParams.fIsCrafter = TRUE;
        tLooteeParams.bForceCrafterAncientLegendaryChance = TRUE;
    }

    if (tLooteeParams.nLevel <= 0)
    {
        tLooteeParams.nLevel = 1;
    }

    snoTreasureClass = ACDGetTreasureClass(tACDLootee, idACDLooter);
    hGBDebug = tACDLootee.GetGBHandle();
}











LootSpecifier::LootSpecifier() :
    snoActorForFlippy(SNO_NONE),
    hGBItem(cghgbNone),
    nGoldAmount(0),
    flGoldMultiplier(1.0f),
    eIQL(IQL_INVALID),
    nNumAffixes(0),
    nAffixLevel(0),
    tLooteeParams(),
    fAutoIdentify(FALSE),
    uDesiredStackCount(1),
    nMinLegendaryAffixes(0),
    nLegendaryItemLevel(0),
    eTargetedPlayerClass(PLAYER_CLASS_NONE),
    fAccountBound(FALSE),
    fBindOnEquip(FALSE),
    eTargetedHirelingClass(0),
    bTryStack(FALSE),
    bAddExtraMajorAffix(FALSE),
    bIgnoreGoldRange(FALSE), unk_e0(FALSE), unk_ec(0)
{
    for (int32 i = 0; i < LOOT_MAX_AFFIXES; i++)
    {
        arAffixGBIDs[i] = GBID_NONE;
    }

    gbidAllowedLegendaryAffixFamily = GBID_NONE;
    eAncientRank = 0;
}



SharedLootManager::SharedLootManager() :
    ptACDLootee(NULL),
    dwGameEventNumber(0),
    bAutoIdentify(FALSE),
    flCameraBiasDist(0.0f)
{
}



void LootSpecifier::AddAffix(GBID gbidAffix)
{

    if (!GBIDExists(gbidAffix, GB_AFFIXES))
        return;



    if (sAffixExcludedFromAffixes(gbidAffix, *this))
        return;



    if (nNumAffixes >= (int32)(sizeof(arAffixGBIDs) / sizeof(arAffixGBIDs[0])))
        return;










    arAffixGBIDs[nNumAffixes] = gbidAffix;
    nNumAffixes++;

}
// Copy of X/Common/Items.cpp's Affix (real home X/Common/Items.h, not adopted yet).
typedef int32 AffixGroup;
struct Affix {  // size 0x144
    GBID gbid; uint32 dwFlags; int32 nFrequencyWeight; int32 nClassFrequencyWeight[7]; int32 nHirelingFrequencyWeight[4];  // 0x00, 0x04, 0x08, 0x0c, 0x28
    int32 nAffixLevelMin; int32 nAffixLevelMax; GBID gbidAffixFamily0; GBID gbidAffixFamily1; PlayerClass ePlayerClass;  // 0x38, 0x3c, 0x40, 0x44, 0x48
    GBID arAllowedTypes[24];  // 0x4c
    GBID arLegendaryAllowedTypes[24];  // 0xac
    GBID gbidExclusionCategory;  // 0x10c
    GBID arExcludedCategories[6];  // 0x110
    uint32 dwAllowedQualityLevels; AffixType eAffixType; GBID gbidAssociatedAffix; ItemEffectType eItemEffectType; int32 nItemEffectLevel; GBID gbidConvertsTo; AffixGroup eAffixGroup;  // 0x128..0x140
};
static_assert(sizeof(Affix) == 0x144, "Affix");


BOOL LootSpecifier::IsTrashEligible()
{
    if (eIQL > IQL_RARE_AFFIX_END)
        return FALSE;

    if (ItemHasLabel(hGBItem.gbid, ITL_SOCKETABLE))
        return FALSE;

    if (ItemHasLabel(hGBItem.gbid, ITL_POTION))
        return FALSE;

    if (ItemHasLabel(hGBItem.gbid, ITL_UTILITY))
        return FALSE;

    if (ItemHasLabel(hGBItem.gbid, ITL_CRAFTING))
        return FALSE;

    if (ItemHasLabel(hGBItem.gbid, ITL_CRAFTING_PLAN))
        return FALSE;

    if (ItemHasLabel(hGBItem.gbid, ITL_ORNAMENT))
        return FALSE;

    return ItemBaseCanBeSold(hGBItem) == ISE_NONE;
}

// Copies of X/Common/Items.cpp's structs (real home X/Common/Items.h:223-326, not adopted yet).
struct ItemAffixSpecifier {  // size 0x50
    ItemAffixSpecifier();
    GBID gbidItem;  // 0x00
    const Affix * ptAffix;  // 0x08
    int32 nAffixLevel;  // 0x10
    ItemQualityLevel eIQL;  // 0x14
    AffixType eLastAffixType;  // 0x18
    uint32 dwFlags;  // 0x1c
    PlayerClass eTargetedPlayerClass;  // 0x20
    HirelingClass eTargetedHirelingClass;  // 0x24
    GBID arCurrentAffixes[6];  // 0x28
    int32 nCurrentAffixesCount;  // 0x40
    GBID gbidAllowedLegendaryAffixFamily;  // 0x44
    AncientRank eAncientRank;  // 0x48
};
static_assert(sizeof(ItemAffixSpecifier) == 0x50, "ItemAffixSpecifier");
BOOL ItemCanAddAffix(const ItemAffixSpecifier & tSpecifier, const AffixTableEntry * ptAffixTableEntry, AffixTableEntry ** ptSpecifierAffixTableEntries); BOOL ItemAffixAddsSockets(GBID gbidAffix); const void * GBTryLockR(const GBHandle & hGB, const GameBalanceDefinition *& pDef);


void LootSpecifier::FillInItemAffixSpecifier(ItemAffixSpecifier & tSpecifier) const
{
    tSpecifier.gbidItem = hGBItem.gbid;
    tSpecifier.eIQL = eIQL;
    tSpecifier.nAffixLevel = nAffixLevel;
    tSpecifier.eTargetedPlayerClass = eTargetedPlayerClass;
    tSpecifier.eTargetedHirelingClass = eTargetedHirelingClass;
    tSpecifier.gbidAllowedLegendaryAffixFamily = gbidAllowedLegendaryAffixFamily;
    tSpecifier.eAncientRank = eAncientRank;

    for (int32 i = 0; i < nNumAffixes; i++)
    {
        tSpecifier.arCurrentAffixes[i] = arAffixGBIDs[i];
    }
    tSpecifier.nCurrentAffixesCount = nNumAffixes;
}


void LootSpecifier::FillInTargetedClass(PlayerClass eDesiredPlayerClass, RNG * ptRNGRandomClass, BOOL bCheckLegendaryClassDropWeight)
{
    ItemGetValidTargetedClass(eTargetedPlayerClass, eTargetedHirelingClass, hGBItem.gbid, eDesiredPlayerClass, ptRNGRandomClass, bCheckLegendaryClassDropWeight);
}











































LPCSTR LootGetDebugStringForItemQuality(ItemQualityLevel eQualityLevel)
{
    switch (eQualityLevel)
    {
    case IQL_INFERIOR:
        return "Inferior";
    case IQL_NORMAL:
        return "Normal";
    case IQL_SUPERIOR:
        return "Superior";
    case IQL_MAGIC_1_AFFIX:
        return "Magic_1_Affix";
    case IQL_MAGIC_2_AFFIX:
        return "Magic_2_Affixes";
    case IQL_MAGIC_3_AFFIX:
        return "Magic_3_Affixes";
    case IQL_RARE_4_AFFIX:
        return "Rare_4_Affixes";
    case IQL_RARE_5_AFFIX:
        return "Rare_5_Affixes";
    case IQL_RARE_6_AFFIX:
        return "Rare_6_Affixes";
    case IQL_LEGENDARY:
        return "Legendary";
    case IQL_SPECIAL:
        return "Special";
    case IQL_NUM_ITEM_QUALITY_LEVELS:
        return "Set Item";
    }
    return "ERROR";
}
















CRefString LootGetDebugStringForItemQuality(const LootSpecifier & tLootSpecifier)
{
    CRefString sQuality;
    if (tLootSpecifier.eAncientRank == ANCIENT_RANK_ANCIENT)
    {
        sQuality = "Ancient ";
    }
    else if (tLootSpecifier.eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT)
    {
        sQuality = "Primal Ancient ";
    }

    ItemQualityLevel eIQL = tLootSpecifier.eIQL;
    if (ItemIsSetItem(tLootSpecifier.hGBItem.gbid))
    {
        eIQL = IQL_NUM_ITEM_QUALITY_LEVELS;
    }

    sQuality += LootGetDebugStringForItemQuality(eIQL);
    return sQuality;
}








void LootInitializeForGame()
{
    GameCommonData & tGCD = SGameGetGameCommonData();
    SGame & tSGame = SGameManager::GameGet();
    tGCD.arItemWeights = (ItemWeightInfo *)SigmaMemoryNew(sizeof(ItemWeightInfo) * ITEM_WEIGHT_ARRAY_MAX_SIZE, 4, tSGame.ptMemoryInterface, TRUE);
}







void LootDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    GameCommonData & tGCD = SGameGetGameCommonData();
    SigmaMemoryFree(tGCD.arItemWeights, tSGame.ptMemoryInterface);
    tGCD.arItemWeights = NULL;
}





int32 sChooseFromWeightArray(
    float * pWeights,
    int32 nNumWeights)
{
    float flMaxWeight = 0.0f;
    for (int32 i = 0; i < nNumWeights; i++)
    {
        flMaxWeight += pWeights[i];
    }
    if (flMaxWeight > 0.000001f)
    {
        int32 nRoll = Rand_Internal(SGameGlobalsGet().rng, (uint32)flMaxWeight - 1);
        float flCurrentWeight = 0.0f;

        for (int32 i = 0; i < nNumWeights; i++)
        {
            flCurrentWeight += pWeights[i];
            if (flCurrentWeight > nRoll)
                return i;
        }
    }
    return -1;
}



inline int32 sChooseFromItemWeightArray(int32 nNumWeights, int32 nMaxWeight)
{


    GameCommonData & tGCD = SGameGetGameCommonData();


    if (nMaxWeight <= 0)
        return -1;



    if (nNumWeights == 1)
        return 0;
    int32 nCurrentWeight = 0;


    int32 nRoll = Rand_Internal(SGameGlobalsGet().rng, nMaxWeight - 1);


    for (int32 i = 0; i < nNumWeights; i++)
    {
        nCurrentWeight += tGCD.arItemWeights[i].nItemWeight;
        if (nCurrentWeight > nRoll)
            return i;
    }
    return -1;
}
















static void sTraceStatForLoot(const ACDID idACDLootee, const ACDID idACDLooter, LootSpecifierArray & arLootSpecifiersToSpawn, uint32 dwGameEventNumber)
{
    CRefString sLooteeName = SNOToString(ACTOR_SNO, ACDGet(idACDLootee).snoActor, FALSE);
    uint32 uNumSpecifiers = arLootSpecifiersToSpawn.GetArraySize();

    ACDID idACDToUse = idACDLootee;
    if (idACDLooter != ACDID_INVALID)
        idACDToUse = idACDLooter;

    if (idACDToUse != ACDID_INVALID)
    {
        TraceStat("lootdrop", idACDToUse, "%u\t%s\t%u", dwGameEventNumber, sLooteeName.str(), uNumSpecifiers);
    }
}



















BOOL sAllowCoopLootThrottling(ActorCommonData & tACDLootee)
{
    SNOName tMonsterSNOName;

    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tACDLootee.snoActor, tMonsterSNOName);
    if (bHasMonsterSNO)
    {
        if (LootMonsterSupportsFirstKillTreasureClass(tMonsterSNOName.snoHandle))
            return FALSE;

        {
            ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
            if (pdefMonster->bDontThrottleDropsOnConsole)
                return FALSE;
        }
    }

    ScopedActorDefinitionLockR pdefActorLootee(tACDLootee.snoActor);
    if (TagMapGetInt(pdefActorLootee->hTagMap, TAG_ACTOR_SPAWN_DONT_THROTTLE_DROPS_ON_CONSOLE))
        return FALSE;

    return TRUE;
}


// X/Server/Loot/Loot.cpp:991 (defined below; the original header declaration is not in src).
BOOL LootDropLoot(ActorCommonData & tACDLootee, const ACDID idACDLooter, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot, BOOL bSuppressUnluckyBonus);


BOOL LootDropLootForEligiblePlayers(
    ActorCommonData & tACDLootee,
    PlayerList & listEligiblePlayers,
    const ACDID idACDPrimaryLooter,
    uint32 dwGameEventNumber,
    BOOL fAutoIdentify,
    float flCameraBiasDist,
    BOOL bSuppressUnluckyBonus)
{
    BOOL fDroppedLoot = FALSE;
    SharedLootManager tSharedLoot;
    tSharedLoot.ptACDLootee = &tACDLootee;
    tSharedLoot.dwGameEventNumber = dwGameEventNumber;
    tSharedLoot.bAutoIdentify = fAutoIdentify;
    tSharedLoot.flCameraBiasDist = flCameraBiasDist;













    PlayerList listLocalPlayers(PlayerListGetElementPool());
    ScopedGlobalDefinitionLockR pdefGlobal;

    int32 nCount = listEligiblePlayers.Count();

    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tACDLootee.snoActor, tMonsterSNOName);
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && nCount > 1 &&
        // operand order inferred from codegen (bHasMonsterSNO tested before nCount); both tests sit at column 65 or line 0
        bHasMonsterSNO &&
        LootMonsterSupportsFirstKillTreasureClass(tMonsterSNOName.snoHandle))
    {
        uint32 nPlayerToCheck = 0;
        uint32 nLuckyPlayer = RandRange_Internal(SGameGlobalsGet().rng, 0, nCount - 1);
        POS pos = listEligiblePlayers.GetHeadPosition();
        while (pos)
        {
            Player * ptPlayer = listEligiblePlayers.GetNext(pos);
            if (nPlayerToCheck++ == nLuckyPlayer)
            {
                fDroppedLoot |= LootDropLoot(tACDLootee, ptPlayer->idPlayerACD, idACDPrimaryLooter, dwGameEventNumber, fAutoIdentify, flCameraBiasDist, 1, &tSharedLoot, bSuppressUnluckyBonus);
                break;
            }
        }
    }
    else if (nCount > 1 && sAllowCoopLootThrottling(tACDLootee))
    {

        while (listEligiblePlayers.Count() > 0)
        {
            float flNumRolls = 1.0f;


            Player * ptPlayer = listEligiblePlayers.GetHead();
            listEligiblePlayers.RemoveHead();


            listLocalPlayers.RemoveAll();
            listLocalPlayers.AddTail_Tracked(ptPlayer);


            POS pos = listEligiblePlayers.GetHeadPosition();
            while (pos)
            {
                POS posCur = pos;
                Player * ptPlayerOther = listEligiblePlayers.GetNext(pos);

                if (PlayersAreLocalToEachOther(*ptPlayer, *ptPlayerOther))
                {
                    listLocalPlayers.AddTail_Tracked(ptPlayerOther);
                    listEligiblePlayers.RemoveAt(posCur);
                }
            }


            int32 nLocalEligiblePlayers = listLocalPlayers.Count();
            if (nLocalEligiblePlayers > 1)
            {

                flNumRolls += pdefGlobal->flLocalCoopDropsPerAdditionalPlayer * (nLocalEligiblePlayers - 1);


                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                flNumRolls -= flRoll;
            }


            int32 nNumRemaining = nLocalEligiblePlayers;
            while (nNumRemaining > 1)
            {
                int32 nIndex = Rand_Internal(SGameGlobalsGet().rng, nNumRemaining - 1);
                // no GetHeadPosition inline at this line in the DWARF: its head load is scheduled into Rand_Internal
                pos = listLocalPlayers.GetHeadPosition();
                for (int32 i = 0; i < nIndex; i++)
                {
                    listLocalPlayers.GetNext(pos);
                }

                listLocalPlayers.MoveToTail(pos);
                // the decrement has no row of its own (merged with the nNumRemaining - 1 at line 796)
                nNumRemaining--;
            }

            pos = listLocalPlayers.GetHeadPosition();
            while (pos && flNumRolls > 0.0f)
            {
                ptPlayer = listLocalPlayers.GetNext(pos);
                fDroppedLoot |= LootDropLoot(tACDLootee, ptPlayer->idPlayerACD, idACDPrimaryLooter, dwGameEventNumber, fAutoIdentify, flCameraBiasDist, nLocalEligiblePlayers, &tSharedLoot, bSuppressUnluckyBonus);

                flNumRolls -= 1.0f;
            }
        }
    }
    else
    {


        POS pos = listEligiblePlayers.GetHeadPosition();
        while (pos)
        {
            Player * ptPlayer = listEligiblePlayers.GetNext(pos);

            BOOL fDroppedLootForPlayer = LootDropLoot(tACDLootee, ptPlayer->idPlayerACD, idACDPrimaryLooter, dwGameEventNumber, fAutoIdentify, flCameraBiasDist, 1, &tSharedLoot, bSuppressUnluckyBonus);

            fDroppedLoot |= fDroppedLootForPlayer;
        }
    }

    // the return has no row of its own (line placement invented)
    return fDroppedLoot;



















}








static const CatchupTreasureClass ctCatchupTreasureClasses[] =
{
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_1, 1.0f, 3600 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_2, 1.0f, 3600 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_3, 0.5f, 3600 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_4, 0.4f, 5400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_5, 0.3f, 7200 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_6, 0.2f, 10800 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_7, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_8, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_9, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_10, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_11, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_12, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_13, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_14, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_15, 0.1f, 14400 },
    { GLOBALSNO_TREASURECLASS_CATCHUP_BOOST_16, 0.1f, 14400 },
};
static const int32 scgNumCatchupTreasureClasses = 16;


SNO LootGetCatchupLootTreasureClass(
    Player * ptPlayer)
{
    SNO snoTreasureClass = SNO_NONE;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {

        int32 nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
        if (nLevel >= 30)
        {
            int32 nIndex = ptACDPlayer->AttributesGetInt(ATTRIBUTE_BOOST_TREASURE_CLASS_INDEX);
            if (nIndex < scgNumCatchupTreasureClasses)
            {
                uint32 uNow = GameGetServerTime();
                uint32 uNextTime = ptACDPlayer->AttributesGetInt(ATTRIBUTE_BOOST_TREASURE_CLASS_NEXT_TIME);
                if (uNow >= uNextTime)
                {


                    const CatchupTreasureClass & tCatchupTreasureClass = ctCatchupTreasureClasses[nIndex];
                    if (tCatchupTreasureClass.flChance >= 1.0f ||  // inferred: member col 47, `||` col 64; the compiler folds this constant-table test into the unsigned nIndex < 2
                        RandFloat_Internal(SGameGetRNG()) <= tCatchupTreasureClass.flChance)
                    {
                        snoTreasureClass = GlobalSNOGet(tCatchupTreasureClass.eGlobalSNO);
                    }
                }
            }
        }
    }

    return snoTreasureClass;
}


void LootDropCatchupLoot(ActorCommonData & tACDVictim, PlayerList & listPlayersToEarnXP, uint32 dwGameEventNumber)
{
    POS pos = listPlayersToEarnXP.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listPlayersToEarnXP.GetNext(pos);
        LootDropCatchupLoot(tACDVictim, ptPlayer, dwGameEventNumber);
    }
}





void LootDropCatchupLoot(ActorCommonData & tACDVictim, Player * ptPlayer, uint32 dwGameEventNumber)
{
    SNO snoBoostTreasureClass = LootGetCatchupLootTreasureClass(ptPlayer);
    if (snoBoostTreasureClass == SNO_NONE)
        return;

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDVictim, ACDID_INVALID);
    tLootee.snoTreasureClass = snoBoostTreasureClass;

    int32 nPlayerLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
    if (nPlayerLevel > 0)
    {
        tLootee.tLooteeParams.nLevel = blz::min(tLootee.tLooteeParams.nLevel, nPlayerLevel);
    }

    LootSpecifierArray arLootSpecifiers;
    BOOL bSuccess = LootGetLootSpecifiers(tLootee, tACDPlayer.id, arLootSpecifiers, NULL, FALSE, 1, NULL);
    if (bSuccess)
    {
        PlayerClass eClass = PlayerGetPlayerClass(ptPlayer);
        for (int32 i = 0; i < arLootSpecifiers.GetArraySize(); i++)
        {
            arLootSpecifiers[i].FillInTargetedClass(eClass, NULL, TRUE);
        }

        LootModifyBeforeSpawning(arLootSpecifiers, tACDVictim, ACDID_INVALID);
        FlippiesCreateOnActor(tACDVictim, tACDPlayer.id, arLootSpecifiers, 0, dwGameEventNumber, 0.0f, tACDPlayer.id, &tLootee, NULL, LC_UNKNOWN, NULL);

        int32 nTCBoostIndex = tACDPlayer.AttributesGetInt(ATTRIBUTE_BOOST_TREASURE_CLASS_INDEX);
        if (nTCBoostIndex >= 0 && nTCBoostIndex < scgNumCatchupTreasureClasses)
        {
            const CatchupTreasureClass & tCatchupTreasureClass = ctCatchupTreasureClasses[nTCBoostIndex];
            uint32 uNextTime = GameGetServerTime() + tCatchupTreasureClass.uDelayTicks;
            tACDPlayer.AttributesSetInt(ATTRIBUTE_BOOST_TREASURE_CLASS_NEXT_TIME, uNextTime);
        }

        tACDPlayer.AttributesSetInt(ATTRIBUTE_BOOST_TREASURE_CLASS_INDEX, nTCBoostIndex + 1);
    }
}













BOOL LootDropLoot(ActorCommonData & tACDLootee, const ACDID idACDLooter, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist, int32 nLocalEligiblePlayers, SharedLootManager * ptSharedLoot, BOOL bSuppressUnluckyBonus)
{
    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDLootee, idACDLooter);
    tLootee.tLooteeParams.bSuppressUnluckyBonus = bSuppressUnluckyBonus;
    BOOL bIgnoreQuestItems = idACDLooter != idACDPrimaryLooter;

    LootSpecifierArray arLootSpecifiersToSpawn;
    BOOL fResult = LootGetLootSpecifiers(tLootee, idACDLooter, arLootSpecifiersToSpawn, NULL, bIgnoreQuestItems, nLocalEligiblePlayers, ptSharedLoot);

    if (dwGameEventNumber == (uint32)-1)
    {
        dwGameEventNumber = SGameGetEventNumber();
    }

    if (TraceStatEnabled())
    {
        sTraceStatForLoot(tACDLootee.id, idACDLooter, arLootSpecifiersToSpawn, dwGameEventNumber);
    }

    if (fResult)
    {
        if (arLootSpecifiersToSpawn.GetArraySize() > 0)
        {
            if (fAutoIdentify)
            {
                for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize(); i++)
                {
                    arLootSpecifiersToSpawn[i].fAutoIdentify = TRUE;
                }
            }

            LootModifyBeforeSpawning(arLootSpecifiersToSpawn, tACDLootee, idACDLooter);
            FlippiesCreateOnActor(tACDLootee, idACDLooter, arLootSpecifiersToSpawn, 0, dwGameEventNumber, flCameraBiasDist, idACDLooter, &tLootee, NULL, LC_UNKNOWN, NULL);
        }

        fResult = tLootee.snoTreasureClass != GlobalSNOGet(GLOBALSNO_TREASURECLASS_DROPNOTHING);
    }

    return fResult;
}























BOOL LootDropCraftingMaterial(const ActorCommonData * ptACDPlayer, const CurrencyType eCurrency, uint64 uAmount)
{
    if (!ptACDPlayer)
        return FALSE;

    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();

    GBID gbidCraftingMat = CurrencyGetGBID(eCurrency);

    tSpecifier.hGBItem.eType = GB_ITEMS;
    tSpecifier.hGBItem.gbid = gbidCraftingMat;

    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
    {
        return FALSE;
    }

    tSpecifier.fAutoIdentify = TRUE;
    tSpecifier.uDesiredStackCount = uAmount;
    tSpecifier.eIQL = IQL_NORMAL;
    tSpecifier.nAffixLevel = 1;

    ScopedItemDefinitionLockR ptItem(gbidCraftingMat);
    if (ptItem && ptItem->eDisplayedQualityLevel != IQL_INVALID)
    {
        tSpecifier.eIQL = ptItem->eDisplayedQualityLevel;
    }

    uint32 dwGameEventNumber = SGameGetEventNumber();

    ACDID idAssignedPlayer = ACDID_INVALID;
    if (GameIsSeasonal())
    {
        idAssignedPlayer = ptACDPlayer->id;
    }

    FlippiesCreateOnActor(*ptACDPlayer, idAssignedPlayer, arLootSpecifiersToSpawn, 0, dwGameEventNumber, 0.0f, ACDID_INVALID, NULL, NULL, LC_UNKNOWN, NULL);

    return TRUE;
}















void LootDropHealth(const ACDID idACDLootee,
                    uint32 dwGameEventNumber,
                    const ACDID idACDKiller,
                    GBID gbidItem)

{
    ActorCommonData * ptACD = ACDTryToGet(idACDLootee);
    if (ptACD == NULL)
        return;



    GBID gbidItemToDrop = gbidItem;

    if (gbidItemToDrop == GBID_NONE)
    {




        HealthDropInfo tHealthDropInfo;
        if (!MonsterGetHealthDropInfo(ptACD->tSNOName, ptACD->eMonsterRarity, tHealthDropInfo))
            return;



        if (tHealthDropInfo.flDropChance < EPSILON)
            return;



        gbidItemToDrop = tHealthDropInfo.gbidHealthItem;


        if (gbidItemToDrop == GBID_NONE)
            return;
    }


    if (dwGameEventNumber == (uint32)-1)
    {
        dwGameEventNumber = SGameGetEventNumber();
    }

    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();


    tSpecifier.hGBItem.eType = GB_ITEMS;
    tSpecifier.hGBItem.gbid = gbidItemToDrop;


    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
        return;






    tSpecifier.eIQL = IQL_NORMAL; tSpecifier.nAffixLevel = 1;
    tSpecifier.fAutoIdentify = TRUE;

    Angle aYaw = RandAngle(SGameGlobalsGet().rng);
    Vector3D vPosOffset(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);

    ScopedGlobalDefinitionLockR pdefGlobal;

    float flDistanceToDrop = pdefGlobal->ptServerData->flHealthGlobeDropDistance;
    vPosOffset *= flDistanceToDrop;

    const float HEALTH_GLOBE_OVER_POPULATION_RADIUS = 20.0f;
    const int32 HEALTH_GLOBE_OVER_POPULATION_MAX = 3;
    WorldPlace placeSpawn = ptACD->GetPlace();
    placeSpawn.wp += vPosOffset;
    XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectSphere(placeSpawn, HEALTH_GLOBE_OVER_POPULATION_RADIUS, ptACD->id, listACDsInRadius, 7);

    int32 nCount = 0;
    POS pos = listACDsInRadius.GetHeadPosition();
    while (pos)
    {
        ACDID idACDCurr = (ACDID)listACDsInRadius.GetNext(pos);
        ActorCommonData * ptACDCurr = ACDTryToGet(idACDCurr);
        if (ptACDCurr)
        {
            ItemUseType eUseType = ItemGetUseType(*ptACDCurr);
            if (eUseType == ITEM_USE_HEALTHGLYPH)
            {
                nCount++;
            }
        }
    }

    if (nCount < HEALTH_GLOBE_OVER_POPULATION_MAX)
    {



        FlippiesCreateOnActor(*ptACD, ACDID_INVALID, arLootSpecifiersToSpawn, 0, dwGameEventNumber, 0.0f, idACDKiller, NULL, &vPosOffset, LC_UNKNOWN, NULL);
    }
}



void LootDropNegativeHealthGlobe(const ACDID idACDTarget, const ACDID idACDKiller)
{

    if (idACDTarget == ACDID_INVALID)
        return;

    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);

    if (ptACDTarget == NULL)
        return;


    float flDropChance = ptACDTarget->AttributesGetFloat(ATTRIBUTE_NEGATIVE_HEALTH_GLOBE_SPAWN_CHANCE);
    if (flDropChance > EPSILON)
    {
        float flRandom = RandFloat_Internal(SGameGlobalsGet().rng);
        if (flRandom < flDropChance)
        {

            ActorAddStruct tActorAdd;
            tActorAdd.snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_NEGATIVE_HEALTH_GLOBE);
            tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_DOESNT_DEACTIVATE);
            tActorAdd.eFastAttribType = FAT_SIMPLE;
            tActorAdd.eTeam = ptACDTarget->GetTeam();
            tActorAdd.tPlace = ptACDTarget->GetPlace();
            tActorAdd.qOrientation = ptACDTarget->GetOrientation();
            tActorAdd.flScale = 1.0f;
            tActorAdd.tSNOName = SNOName(ACTOR_SNO, GlobalSNOGet(GLOBALSNO_ACTOR_NEGATIVE_HEALTH_GLOBE));
            tActorAdd.dwActorAddFlags |= (1 << ACTORADD_EVEN_WHEN_STUCK);
            tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_PERTURB);
            tActorAdd.dwActorAddFlags |= (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);


            ActorID idActorGlobe = ActorAdd(tActorAdd);
            if (idActorGlobe != ACTORID_INVALID)
            {
                ActorCommonData & tACDGlobe = ACDGet(ACDFindByActor(idActorGlobe));
                tACDGlobe.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, TRUE);
                tACDGlobe.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, TRUE);
                tACDGlobe.AttributesSetInt(ATTRIBUTE_INVULNERABLE, TRUE);
                ActorFlagSet(idActorGlobe, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
                tACDGlobe.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, TRUE);

                ActorDeleteSoon(idActorGlobe, 30 * TICKS_PER_SECOND);
            }
        }
    }
}



void LootDropShrineGlobe(const ACDID idACDLootee, uint32 dwGameEventNumber, const ACDID idACDKiller, GBID gbidItem)
{








    ActorCommonData * ptACD = ACDTryToGet(idACDLootee);
    if (ptACD == NULL)
        return;



    ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
    if (ptACDKiller == NULL)
        return;




    if (ptACDKiller->AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON))
        return;



    LootSpecifierArray arLootSpecifiersToSpawn;

    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();


    tSpecifier.hGBItem.eType = GB_ITEMS;

    tSpecifier.hGBItem.gbid = StringToGBID("PowerGlobe_v2_x1");





    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
        return;





    tSpecifier.eIQL = IQL_NORMAL; tSpecifier.nAffixLevel = 1;
    tSpecifier.fAutoIdentify = TRUE;

    Angle aYaw = RandAngle(SGameGlobalsGet().rng);
    Vector3D vPosOffset(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);

    ScopedGlobalDefinitionLockR pdefGlobal;

    float flDistanceToDrop = pdefGlobal->ptServerData->flHealthGlobeDropDistance;
    vPosOffset *= flDistanceToDrop;

    FlippiesCreateOnActor(*ptACD, ACDID_INVALID, arLootSpecifiersToSpawn, 0, dwGameEventNumber, 0.0f, idACDKiller, NULL, &vPosOffset, LC_UNKNOWN, NULL);
}


void LootDropLootRunProgressGlobes(const ACDID idACDLootee,
                                   const ACDID idACDKiller,
                                   const BOOL bIsTiered,
                                   const int32 nNumToDropOverride)

{
    ActorCommonData * ptACD = ACDTryToGet(idACDLootee);
    if (ptACD == NULL)
        return;



    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    BOOL bLootRunBossDead = (tSGameGlobals.dwFlags2 & (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED)) != 0;
    BOOL bLootRunBossActive = (tSGameGlobals.dwFlags2 & (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_ACTIVE)) != 0;


    if (bLootRunBossDead || bLootRunBossActive)
        return;




    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();


    tSpecifier.hGBItem.eType = GB_ITEMS;

    if (GameIsWeeklyChallenge())
    {
        tSpecifier.hGBItem.gbid = StringToGBID("Challenge_Rifts_Orb");
    }
    else if (bIsTiered) { tSpecifier.hGBItem.gbid = StringToGBID("p1_tiered_rifts_Orb"); }
    else { tSpecifier.hGBItem.gbid = StringToGBID("p1_normal_rifts_Orb"); }








    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
        return;





    tSpecifier.eIQL = IQL_NORMAL; tSpecifier.nAffixLevel = 1;
    tSpecifier.fAutoIdentify = TRUE;

    ScopedGlobalDefinitionLockR pdefGlobals;

    int32 nNumGlobesToDrop = nNumToDropOverride;
    if (nNumGlobesToDrop == 0)
    {
        if (ptACD->eMonsterRarity == MR_CHAMPION)
        {
            nNumGlobesToDrop = pdefGlobals->ptServerData->nLootRunChampionProgressGlobeCount;
        }
        else if (ptACD->eMonsterRarity == MR_RARE)
        {
            nNumGlobesToDrop = pdefGlobals->ptServerData->nLootRunRareProgressGlobeCount;
        }
    }

    Angle aYaw = RandAngle(SGameGlobalsGet().rng);
    float flOffset = pdefGlobals->ptServerData->flLootRunProgressGlobeDropRadiusMin;

    const int nAngleIncrementBase = 50;
    const int nAngleIncrementDelta = 19;

    for (int i = 0; i < nNumGlobesToDrop; i++)
    {
        aYaw = aYaw + Math::DegToAngle((float)nAngleIncrementBase) + Math::DegToAngle((float)Rand_Internal(SGameGlobalsGet().rng, nAngleIncrementDelta));
        flOffset += RandFloat_Internal(SGameGlobalsGet().rng) * pdefGlobals->ptServerData->flLootRunProgressGlobeDropRadiusDelta;
        Vector3D vPosOffset(Math::Cos(aYaw), Math::Sin(aYaw), 0.0f);
        vPosOffset *= flOffset;

        XPooledDataIDList listSpawnedACDs(MemoryPoolsGetDATAIDListPool());

        FlippiesCreateOnActor(*ptACD, ACDID_INVALID, arLootSpecifiersToSpawn, 0, 0, 0.0f, idACDKiller, NULL, &vPosOffset, LC_UNKNOWN, &listSpawnedACDs);

        POS pos = listSpawnedACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)listSpawnedACDs.GetNext(pos);
            ActorCommonData * ptACDItem = ACDTryToGet(idACD);
            if (ptACDItem)
            {
                ptACDItem->AttributesSetInt(ATTRIBUTE_ITEM_IGNORES_PICKUP_RADIUS_FOR_PICKUP, TRUE);
            }
        }
    }
}


static BOOL sCheckNoDrop(
    const float flNoDropScalar,
    const float flBaseNoDrop)
{
    BOOL bNoDrop = FALSE;
    if (flBaseNoDrop > 0.999999f)
    {
        bNoDrop = TRUE;
    }
    else if (flBaseNoDrop > 0.000001f)
    {

        float flNoDropChance = pow(1.0f / flBaseNoDrop, -flNoDropScalar);

        float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
        bNoDrop = flRoll < flNoDropChance;
    }
    return bNoDrop;
}








static CRefString sDebugTranslateItemLabelMask(const uint32 * pdwItemLabelMask,
                                               int32 nNumDWORDs)
{
    CRefString sTranslatedLabels("");
    for (int32 i = 0; i < nNumDWORDs; i++)
    {
        for (int32 iBit = 0; iBit < 32; iBit++)
        {
            if (pdwItemLabelMask[i] & (1 << iBit))
            {
                CRefString sLabel(SymbolToName(i * 32 + iBit, gItemLabels));
                if (sTranslatedLabels.Length())
                {
                    sTranslatedLabels += ", ";
                }
                sTranslatedLabels += (LPCSTR)sLabel;
            }
        }
    }
    return sTranslatedLabels;
}





































































extern "C" BOOL sub_710050e800(GBID gbidItem);
extern "C" BOOL sub_71004f8a30(GBID gbidItem);
extern "C" BOOL sub_710050eb20(GBID gbidItem);
extern "C" BOOL sub_7100897970(GBID gbidItem, const ActorCommonData * ptACDLooter);

// sPickItem_PreliminaryFilterOut (unnamed in the 2.7.7 functions.csv)
BOOL sPickItem_PreliminaryFilterOut(ACDID idACDLooter, const Item & tItem, BOOL bWantLegendary, BOOL bDisableTormentCheck, BOOL bDisableTormentDrops, Act eCurrentAct, int32 nHandicapLevel, BOOL bX1Enabled, BOOL bIgnoreSeason)
{










    if (tItem.eMinAct != ACT_INVALID && eCurrentAct != ACT_INVALID && tItem.eMinAct > eCurrentAct)
        return TRUE;




    if (tItem.dwFlags & ((1 << ITEM_SPECIAL_BIT) | (1 << ITEM_NO_RANDOM_DROP_BIT)))
        return TRUE;





    BOOL bIsLegendary = (tItem.dwFlags & (1 << ITEM_LEGENDARY_BIT)) != 0;
    if (bIsLegendary != bWantLegendary)
        return TRUE;




    if (!bX1Enabled && (tItem.dwFlags & (1 << ITEM_X1_ONLY_BIT)))
        return TRUE;




    if (!bDisableTormentCheck && (tItem.dwFlags & (1 << ITEM_TORMENT_ONLY_BIT)))
    {
        if (!(!bDisableTormentDrops && nHandicapLevel >= 4))
            return TRUE;
    }









    if (tItem.dwFlags & (1 << ITEM_TIERED_LOOT_RUN_ONLY_BIT))
    {
        ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
        if (ptACDLooter == NULL || !ACDIsInTieredLootRun(*ptACDLooter))
            return TRUE;
    }

    if (!bIgnoreSeason && !sub_7100897970(tItem.gbid, ACDTryToGet(idACDLooter)))
        return TRUE;








    return Console::Online::GetGBIDBlacklistData(tItem.gbid, NULL);




}

extern "C" BOOL sub_7100897970(GBID gbidItem, const ActorCommonData * ptACDLooter)
{
    if (sub_710050e800(gbidItem) && !GameIsSeasonal())
        return FALSE;
    if (sub_710050e910(gbidItem) && !ptACDLooter->AttributesGetInt(ATTRIBUTE_ETHEREAL_ITEMS_UNLOCKED))
        return FALSE;
    if (sub_71004f8a30(gbidItem) && !ptACDLooter->AttributesGetInt(ATTRIBUTE_SOUL_SHARDS_UNLOCKED))
        return FALSE;
    if (sub_710050eb20(gbidItem) && !ptACDLooter->AttributesGetInt(ATTRIBUTE_SANCTIFIED_ITEMS_UNLOCKED))
        return FALSE;
    return TRUE;
}











static GBID sPickByItemLabel(const SharedLooteeParams & tLooteeParams,
                             const LootDropModifier & tModifier,
                             const ACDID idACDLooter)
{
    int32 nLooteeLevelToUse = Math::Max(1, tLooteeParams.nLevel);
    if (nLooteeLevelToUse < 1 || nLooteeLevelToUse > 110)  // 110: no enumerator found
    {
        return GBID_NONE;
    }



    GameCommonData & tGCD = SGameGetGameCommonData();
    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;

    int32 nMemberItems = ptItemAppGlobals->nItems;

    int32 nNumWeights = 0;
    int32 nMaxWeight = 0;


    ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
    BOOL bShouldTargetClass = FALSE;
    if (ptACDLooter)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flTargetedClassItemChancePercent = pdefGlobal->flTargetedClassItemChance;

        float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
        bShouldTargetClass = flRoll < flTargetedClassItemChancePercent;
    }







    Act eCurrentAct = GameGetCurrentAct();
    int32 nHandicapLevel = GameGetHandicapLevel();
    BOOL bX1Enabled = (OnlineService::GetServerContentFlags() & (1 << 1)) != 0;

    for (int32 nItemIdx = 0; nItemIdx < nMemberItems; nItemIdx++)
    {
        const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];

        BOOL bUseForcedILevel = tLooteeParams.nForcedILevel > 0;
        if (bUseForcedILevel && tItem.nItemLevel != tLooteeParams.nForcedILevel)
            continue;



        if (sPickItem_PreliminaryFilterOut(idACDLooter, tItem, FALSE, tLooteeParams.bDisableTormentCheck, tLooteeParams.bDisableTormentDrops, eCurrentAct, nHandicapLevel, bX1Enabled, FALSE))
            continue;





        if (bShouldTargetClass)
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(idACDLooter);
            if (!ItemMeetsOwnerClassRestrictions(tItem.gbid, ePlayerClass, FALSE))
                continue;
        }




        int32 * pnDropWeightsTable = tLooteeParams.fIsVendor ? tItem.pnVendorDropWeights : tItem.pnDropWeights;
        int32 nDropWeight = pnDropWeightsTable[nLooteeLevelToUse - 1];
        if (bUseForcedILevel)
        {
            nDropWeight = Math::Max(nDropWeight, 1);
        }

        if (nDropWeight > 0)
        {
            if (ItemHasLabelMask(tItem, tModifier.dwItemLabelMask) && nNumWeights < ITEM_WEIGHT_ARRAY_MAX_SIZE)
            {



                tGCD.arItemWeights[nNumWeights].nItemWeight = nDropWeight;
                tGCD.arItemWeights[nNumWeights].nItemIndex = nItemIdx;
                nMaxWeight += tGCD.arItemWeights[nNumWeights].nItemWeight;
                nNumWeights++;
            }
        }
    }


    if (nNumWeights < 1)
    {
        return GBID_NONE;
    }

    int32 nWeightIdx = sChooseFromItemWeightArray(nNumWeights, nMaxWeight);


    int32 nItemIdx = tGCD.arItemWeights[nWeightIdx].nItemIndex;


    const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];

    sDebugTranslateItemLabelMask(tModifier.dwItemLabelMask, sizeof(tModifier.dwItemLabelMask) / sizeof(tModifier.dwItemLabelMask[0])).str();  // argument of a trace call that was compiled away
    return tItem.gbid;
}




















static ItemQualityLevel sGetItemQualityLevel(const int32 nLooteeLevel,
                                             const ACDID idACDLooter,
                                             const GBID gbidQualityClass,
                                             const GBID gbidItem,
                                             const BOOL bSuppressMagicAndGoldFind,
                                             int32 nLocalEligiblePlayers)
{



    ItemQualityLevel eIQL = IQL_NORMAL;

    if (ItemIsShard(gbidItem))
    {
        eIQL = IQL_RARE_4_AFFIX;
    }

    else if (ItemCanBeMagical(gbidItem))
    {

        float flQualityLevelChance[IQL_NUM_ITEM_QUALITY_LEVELS];
        const GameBalanceDefinition * pGBDef;
        const LootDistEntry * ptLootDistEntry = sGetLootDistEntry(nLooteeLevel, pGBDef, -1);


        SigmaMemoryCopy(flQualityLevelChance, ptLootDistEntry->flQualityLevelChance, sizeof(flQualityLevelChance));
        GBUnlockR(pGBDef);


        BOOL fItemAlwaysMagic;
        const ItemLevelModifier * ptItemLevelMod = sGetItemLevelModifiers(nLooteeLevel, gbidItem, pGBDef, fItemAlwaysMagic);


        if (!ptItemLevelMod)
        {
            if (pGBDef) GBUnlockR(pGBDef);
            return IQL_NORMAL;
        }




        for (int32 i = 0; i < IQL_NUM_ITEM_QUALITY_LEVELS; i++)
        {
            flQualityLevelChance[i] *= ptItemLevelMod->flQualityLevelScalar[i];
        }
        GBUnlockR(pGBDef);


        if (gbidQualityClass != GBID_NONE)
        {
            GBHandle hGB(GB_ITEM_QUALITY_CLASSES, gbidQualityClass);
            const QualityClass * ptQualityClass = (const QualityClass *)GBLockR(hGB, pGBDef, TRUE);


            for (int32 i = 0; i < IQL_NUM_ITEM_QUALITY_LEVELS; i++)
            {
                flQualityLevelChance[i] = ptQualityClass->flQualityLevelScalar[i] * flQualityLevelChance[i]
                    + ptQualityClass->flQualityLevelAdd[i];
            }

            GBUnlockR(pGBDef);
        }


        float flMagicFindBonus = 0.0f;


        if (nLocalEligiblePlayers > 1)
        {

            ScopedGlobalDefinitionLockR pdefGlobal;
            float flBonusCoopMagicFind = pdefGlobal->flLocalCoopMagicFindBonusPerPlayer * (nLocalEligiblePlayers - 1);
            flMagicFindBonus += flBonusCoopMagicFind;
        }

        ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
        if (ptACDLooter
            && !ptACDLooter->AttributesGetInt(ATTRIBUTE_MAGIC_AND_GOLD_FIND_SUPPRESSED) && !bSuppressMagicAndGoldFind)
        {

            flMagicFindBonus += LootGetFindValue(*ptACDLooter, ATTRIBUTE_MAGIC_FIND_TOTAL);
        }


        for (int32 i = 0; i < IQL_NUM_ITEM_QUALITY_LEVELS; i++)
        {
            float flMagicFindMultiplier = (i >= IQL_RARE_4_AFFIX && i <= IQL_RARE_6_AFFIX) ? 0.3f : ((i == IQL_LEGENDARY) ? 0.1f : 1.0f);
            flQualityLevelChance[i] *= (flMagicFindBonus * flMagicFindMultiplier + 1.0f);
        }


        for (int32 i = IQL_NUM_ITEM_QUALITY_LEVELS - 1; i >= 0; i--)
        {
            if (i == IQL_LEGENDARY)
                continue;





            if (fItemAlwaysMagic && i < IQL_MAGIC_1_AFFIX)
            {
                eIQL = IQL_MAGIC_1_AFFIX;
                break;
            }


            if (RandFloat_Internal(SGameGlobalsGet().rng) < flQualityLevelChance[i]
                || i == IQL_INFERIOR)
            {





                eIQL = (ItemQualityLevel)i;
                break;
            }
        }
    }
    return eIQL;
}
int32 sItemGetCraftedLevel(GBHandle hGBItem)
{

    const GameBalanceDefinition * pdefGBItems;
    ItemDefinition * pdefItem = (ItemDefinition *)GBLockR(hGBItem, pdefGBItems, TRUE);
    if (!pdefItem)
    {
        return 1;
    }

    int32 nAffixLevel = pdefItem->nCrafterRequiredLevel > 0 ? pdefItem->nCrafterRequiredLevel : pdefItem->nItemLevel;
    GBUnlockR(pdefGBItems);
    return nAffixLevel;
}



int32 sItemGetAffixLevel(GBHandle hGBItem)
{

    const GameBalanceDefinition * pdefGBItems;
    ItemDefinition * pdefItem = (ItemDefinition *)GBLockR(hGBItem, pdefGBItems, TRUE);
    if (!pdefItem)
    {
        return 1;
    }

    int32 nAffixLevel = pdefItem->nItemLevel;
    GBUnlockR(pdefGBItems);
    return nAffixLevel;
}



static BOOL sUprankLegendaryAffix(GBID & gbidAffix)
{
    ScopedItemAffixLockR ptAffix(gbidAffix);

    if (ptAffix->gbidLegendaryUprankAffix != GBID_NONE && GBIDExists(ptAffix->gbidLegendaryUprankAffix, GB_AFFIXES))
    {
        gbidAffix = ptAffix->gbidLegendaryUprankAffix;
        return TRUE;
    }



    return FALSE;
}








static void sUprankAffixByAncientRank(GBID & gbidAffix, const LootSpecifier & tSpecifier)
{


    if (tSpecifier.eIQL == IQL_LEGENDARY ||
        ItemIsLegendary(tSpecifier.hGBItem.gbid))
    {



        AncientRank eAncientRank = tSpecifier.eAncientRank;
        while (eAncientRank > ANCIENT_RANK_NONE)
        {

            if (eAncientRank != ANCIENT_RANK_PRIMAL_ANCIENT && !sUprankLegendaryAffix(gbidAffix))
            {
                break;
            }

            eAncientRank--;
        }
    }
}


void LootGetSpecifierForSpecifiedItem(LootSpecifier & tSpecifier,
                                      const ItemSpecifierData & tSpecificItem,
                                      const SharedLooteeParams & tLooteeParams,
                                      const Player * ptPlayer,
                                      BOOL bTargetRandomClass)
{

    GBHandle hGBItem(GB_ITEMS, tSpecificItem.gbidItem);
    tSpecifier.hGBItem = hGBItem;

    tSpecifier.tLooteeParams = tLooteeParams;
    if (tSpecifier.tLooteeParams.fIsCrafter)
    {
        tSpecifier.nAffixLevel = sItemGetCraftedLevel(hGBItem);
    }
    else
    {
        tSpecifier.nAffixLevel = sItemGetAffixLevel(hGBItem);
    }

    if (tSpecificItem.bAccountBound)
    {
        tSpecifier.fAccountBound = TRUE;
    }


    if (ItemIsLegendary(hGBItem.gbid)
        && !tSpecifier.tLooteeParams.fIsCrafter
        && !tSpecifier.tLooteeParams.fIsVendor)
    {
        if (!tLooteeParams.bIgnoreLevelRestriction)
        {




            tSpecifier.nAffixLevel = tLooteeParams.nLevel; tSpecifier.nLegendaryItemLevel = tLooteeParams.nLevel;
        }
        else tSpecifier.nLegendaryItemLevel = tSpecifier.nAffixLevel;

        tSpecifier.eIQL = IQL_LEGENDARY;



        if (tLooteeParams.nLevel >= 61 && tLooteeParams.nLevel <= 63)
        {
            tSpecifier.nAffixLevel = 64;
            tSpecifier.nLegendaryItemLevel = 64;
        }
    }


    sRollForTargetedItem(ptPlayer, tSpecifier, bTargetRandomClass);

    BOOL bHirelingTargeted = ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_FOLLOWER_SPECIAL);
    if (ptPlayer && !bHirelingTargeted && ptPlayer->ePlayerClassPortrait != tSpecifier.eTargetedPlayerClass)
    {
        tSpecifier.tLooteeParams.bDisablePrimalAncientDrops = TRUE;
    }

    LootRollForAncientLegendary(tSpecifier, ptPlayer ? ptPlayer->idPlayerACD : ACDID_INVALID);

    LootDetermineInherentAffixes(tSpecifier);
    for (int32 i = 0; i < tSpecificItem.nNumAffixes; i++)
    {
        GBID gbidAffix = tSpecificItem.gbidAffixes[i];
        sUprankAffixByAncientRank(gbidAffix, tSpecifier);
        tSpecifier.AddAffix(gbidAffix);
    }

    int32 nAdditionalRandomAffixes = tSpecificItem.nAdditionalRandomAffixes;
    int32 nMajorAffixes = tSpecificItem.nAdditionalMajorAffixes;
    int32 nMinorAffixes = tSpecificItem.nAdditionalMinorAffixes;

    if (tSpecificItem.nAdditionalRandomAffixesDelta > 0)
    {
        nAdditionalRandomAffixes += Rand_Internal(SGameGlobalsGet().rng, tSpecificItem.nAdditionalRandomAffixesDelta);
    }


    if (tSpecifier.tLooteeParams.fIsCrafter)
    {












        int32 nInherentMajorAffixes = 0;
        int32 nInherentMinorAffixes = 0;
        for (int32 i = 0; i < tSpecifier.nNumAffixes; i++)
        {
            ScopedItemAffixLockR ptAffix(tSpecifier.arAffixGBIDs[i]);
            if (ptAffix->eAffixGroup == 0)  // AffixGroup enumerators are not adopted
            {
                nInherentMajorAffixes++;
            }
            else if (ptAffix->eAffixGroup == 1)
            {
                nInherentMinorAffixes++;
            }
        }

        LootDetermineMajorMinorAffixCount(tSpecifier, nMajorAffixes, nMinorAffixes, nAdditionalRandomAffixes + tSpecifier.nNumAffixes, nInherentMajorAffixes, nInherentMinorAffixes);


        if (nMajorAffixes > 0 || nMinorAffixes > 0)
        {
            nMajorAffixes = Math::Max(0, nMajorAffixes - nInherentMajorAffixes);
            nMinorAffixes = Math::Max(0, nMinorAffixes - nInherentMinorAffixes);
            nAdditionalRandomAffixes = 0;
        }
    }
    else
    {
        ItemGetBonusRandomAffixes(hGBItem, nMajorAffixes, nMinorAffixes, nAdditionalRandomAffixes);
    }

    int32 nTotalRandomAffixes = nMajorAffixes + nMinorAffixes + nAdditionalRandomAffixes;

    tSpecifier.eIQL = ItemDetermineQualityLevel("LootGetSpecifierForSpecifiedItem", hGBItem, tSpecificItem.gbidAffixes, tSpecificItem.nNumAffixes, nTotalRandomAffixes);

    tSpecifier.nMinLegendaryAffixes = tSpecificItem.nMinLegendaryAffixes;

    AffixType eLastAffixType = -1;  // no enumerator found for -1
    if (tSpecificItem.nNumAffixes == 1)
    {


        GBID gbidAffix = tSpecificItem.gbidAffixes[0];
        ScopedItemAffixLockR ptAffix(gbidAffix);

        eLastAffixType = ptAffix->eAffixType;
    }

    tSpecifier.AddRandomAffixes(eLastAffixType, nMajorAffixes, nMinorAffixes, nAdditionalRandomAffixes);
}



static BOOL sGetSpecifiersFromModifier(const LooteeInfo & tLootee,
                                       const ACDID idACDLooter,
                                       const LootDropModifier & tModifier,
                                       const GBID gbidInheritedQualityClass,
                                       const SNO snoTreasureClass,
                                       LootSpecifierArray & arSpecifiers,
                                       XStringArray & tTCStack,
                                       int32 nLocalEligiblePlayers,
                                       SharedLootManager * ptSharedLoot)
{
    LootDropModifier tModifierToUse = tModifier;

    if (gbidInheritedQualityClass != GBID_NONE && tModifier.gbidQualityClass == GBID_NONE)
    {

        tModifierToUse.gbidQualityClass = gbidInheritedQualityClass;
    }

    GBHandle hGBItem(GB_ITEMS, GBID_NONE);

    ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
    PlayerClass eLooterClass = PLAYER_CLASS_NONE;
    if (ptACDLooter)
    {
        eLooterClass = PlayerGetPlayerClass(idACDLooter);
    }

    PlayerClass eTargetedPlayerClass = PLAYER_CLASS_NONE;

    switch (tModifier.eDropType)
    {
    case LDT_TREASURECLASS:
    {
        if (tModifier.snoSubTreasureClass == SNO_NONE)
        {
            return FALSE;
        }

        ScopedTreasureClassDefinitionLockR pdefSubTreasureClass(tModifier.snoSubTreasureClass);

        sGetLootSpecifiers(tLootee, idACDLooter, *pdefSubTreasureClass, tModifierToUse.gbidQualityClass, arSpecifiers, tTCStack, tModifierToUse.snoCondition, nLocalEligiblePlayers, ptSharedLoot);
        return TRUE;
    }

    case LDT_LEGENDARY:
    {
        if (ptACDLooter)
        {
            ScopedGlobalDefinitionLockR pdefGlobal;
            int32 nTargetedLegendaryChance = ptACDLooter->AttributesGetInt(ATTRIBUTE_TARGETED_LEGENDARY_CHANCE);
            float flTargetedLegendaryChancePercent = nTargetedLegendaryChance / 100.0f;
            float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
            if (flRoll < flTargetedLegendaryChancePercent)
            {

                eTargetedPlayerClass = eLooterClass;

                ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_LEGENDARY_CHANCE, pdefGlobal->ptServerData->nTargetedLegendaryDropChanceBase);
            }
            else
            {
                int32 nTargetedLegendaryChanceIncrement = RandRange_Internal(SGameGlobalsGet().rng, pdefGlobal->ptServerData->nTargetedLegendaryDropChanceIncrement, pdefGlobal->ptServerData->nTargetedLegendaryDropChanceIncrementDelta);



                ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_LEGENDARY_CHANCE, nTargetedLegendaryChance + nTargetedLegendaryChanceIncrement);
            }
        }

        if (eTargetedPlayerClass == PLAYER_CLASS_NONE)
        {
            eTargetedPlayerClass = (PlayerClass)Rand_Internal(SGameGlobalsGet().rng, NUM_PLAYER_CLASSES - 1);
        }

        hGBItem.gbid = sPickLegendary(ptACDLooter, tLootee.tLooteeParams, eTargetedPlayerClass, tModifierToUse.nItemLabel, tModifierToUse.nItemLabel2, tLootee.tLooteeParams.bIgnoreClassLegendaryDropWeight);
        GBIDToString(hGBItem, FALSE).str();  // argument of a trace call that was compiled away
        break;
    }
    case LDT_CLASS_TARGETED_LEGENDARY:
    {

        if (ptACDLooter)
        {
            eTargetedPlayerClass = PlayerGetPlayerClass(idACDLooter);
        }

        if (eTargetedPlayerClass == PLAYER_CLASS_NONE)
        {
            eTargetedPlayerClass = (PlayerClass)Rand_Internal(SGameGlobalsGet().rng, NUM_PLAYER_CLASSES - 1);
        }

        hGBItem.gbid = sPickLegendary(ptACDLooter, tLootee.tLooteeParams, eTargetedPlayerClass, tModifierToUse.nItemLabel, tModifierToUse.nItemLabel2, FALSE);

        if (hGBItem.gbid == GBID_NONE)
        {


            hGBItem.gbid = sPickLegendary(ptACDLooter, tLootee.tLooteeParams, PLAYER_CLASS_NONE, tModifierToUse.nItemLabel, tModifierToUse.nItemLabel2, TRUE);

            if (hGBItem.gbid == GBID_NONE)
            {

                hGBItem.gbid = sPickByItemLabel(tLootee.tLooteeParams, tModifierToUse, idACDLooter);
            }
        }

        GBIDToString(hGBItem, FALSE).str();  // argument of a trace call that was compiled away
        break;
    }
    case LDT_ITEM:
    {

        if (tModifier.tSpecificItem.gbidItem == GBID_NONE)
        {
            return FALSE;
        }

        ScopedItemDefinitionLockR pdefItem(tModifier.tSpecificItem.gbidItem);








        if (Console::Online::GetGBIDBlacklistData(tModifier.tSpecificItem.gbidItem, NULL))
        {
            return FALSE;
        }










        if (ItemIsLegendary(tModifier.tSpecificItem.gbidItem))
        {

            if (tLootee.tLooteeParams.nLevel < ItemGetItemLevel(tModifier.tSpecificItem.gbidItem)
                && !tLootee.tLooteeParams.bIgnoreLevelRestriction)
            {
                return FALSE;
            }
        }

        Player * ptPlayer = PlayerGetByACD(idACDLooter);

        if (ptPlayer
            && tModifier.tSpecificItem.gbidItem != GBID_NONE
            && !tLootee.tLooteeParams.bIgnoreKnownRecipeCheck
            && ItemHasLabel(tModifier.tSpecificItem.gbidItem, ITL_CRAFTING_PLAN))
        {

            BOOL bFoundCraftingRecipe = FALSE;
            BOOL bFoundUnknown = FALSE;
            for (int32 i = 0; i < (int32)(sizeof(pdefItem->gbidCraftingRecipesToGrant) / sizeof(pdefItem->gbidCraftingRecipesToGrant[0])); i++)
            {
                if (pdefItem->gbidCraftingRecipesToGrant[i] != GBID_NONE)
                {
                    bFoundCraftingRecipe = TRUE;
                    if (!ItemCrafting_PlayerKnowsRecipe(*ptPlayer, pdefItem->gbidCraftingRecipesToGrant[i])
                        && !ItemCrafting_PlayerPossessesRecipe(*ptPlayer, pdefItem->gbidCraftingRecipesToGrant[i])
                        && !sSpecifierArrayHasExistingGBID(pdefItem->gbidCraftingRecipesToGrant[i], arSpecifiers))
                    {
                        bFoundUnknown = TRUE;
                        break;
                    }
                }
            }

            if (!bFoundCraftingRecipe)
            {









                for (int32 i = 0; i < (int32)(sizeof(pdefItem->gbidTransmogsToGrant) / sizeof(pdefItem->gbidTransmogsToGrant[0])); i++)
                {
                    if (pdefItem->gbidTransmogsToGrant[i] != GBID_NONE
                        // (line 2303)
                        && !ItemCrafting_IsTransmogUnlocked(pdefItem->gbidTransmogsToGrant[i], ptPlayer, NULL))
                    {
                        bFoundUnknown = TRUE;
                        break;
                    }
                }
            }

            if (!bFoundUnknown)
            {
                return FALSE;
            }
        }


        hGBItem.gbid = tModifier.tSpecificItem.gbidItem;
        GBIDToString(hGBItem, FALSE).str();  // argument of a trace call that was compiled away
    }
        break;

    case LDT_ITEMLABEL:
        hGBItem.gbid = sPickByItemLabel(tLootee.tLooteeParams, tModifierToUse, idACDLooter);
        break;
    default:
        break;
    }


    if (hGBItem.gbid == GBID_NONE)
    {
        return FALSE;
    }


    if (arSpecifiers.GetArraySize() >= 128)
    {
        return FALSE;
    }



    LootSpecifier & tSpecifier = arSpecifiers.AppendElement_Tracked();

    if (!ItemsGetItem(hGBItem.gbid))
    {
        return FALSE;
    }









    if (tModifier.eDropType == LDT_ITEM)
    {
        if (!tModifier.tSpecificItem.IsValid())
        {
            return FALSE;
        }


        LootGetSpecifierForSpecifiedItem(tSpecifier, tModifier.tSpecificItem, tLootee.tLooteeParams, PlayerGetByACD(idACDLooter), FALSE);
        tSpecifier.flGoldMultiplier = tModifier.flGoldMultiplier;
        tSpecifier.bIgnoreGoldRange = tModifier.bIgnoreGoldRanges;
        tSpecifier.bTryStack = tModifier.bTryStack;
    }
    else if (tModifier.eDropType == LDT_LEGENDARY || tModifier.eDropType == LDT_CLASS_TARGETED_LEGENDARY)
    {


        if (ItemIsLegendary(hGBItem.gbid))
        {
            tSpecifier.hGBItem = hGBItem;
            tSpecifier.tLooteeParams = tLootee.tLooteeParams;
            tSpecifier.nAffixLevel = tLootee.tLooteeParams.nLevel;
            tSpecifier.eIQL = IQL_LEGENDARY;
            tSpecifier.nLegendaryItemLevel = tLootee.tLooteeParams.nLevel;


            if (tLootee.tLooteeParams.nLevel >= 61 && tLootee.tLooteeParams.nLevel <= 63)
            {
                tSpecifier.nLegendaryItemLevel = 64;
                tSpecifier.nAffixLevel = 64;
            }

            tSpecifier.FillInTargetedClass(eTargetedPlayerClass, NULL, TRUE);


            BOOL bHirelingTargeted = ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_FOLLOWER_SPECIAL);
            if (!bHirelingTargeted && (eLooterClass == PLAYER_CLASS_NONE || eTargetedPlayerClass != eLooterClass))
            {
                tSpecifier.tLooteeParams.bDisablePrimalAncientDrops = TRUE;
            }
            LootRollForAncientLegendary(tSpecifier, idACDLooter);
        }
        else
        {
            tSpecifier.hGBItem = hGBItem;
            tSpecifier.tLooteeParams = tLootee.tLooteeParams;
            tSpecifier.nAffixLevel = sItemGetAffixLevel(hGBItem);
            tSpecifier.eIQL = IQL_RARE_6_AFFIX;
            tSpecifier.flGoldMultiplier = tModifierToUse.flGoldMultiplier;
            tSpecifier.bIgnoreGoldRange = tModifier.bIgnoreGoldRanges;
            tSpecifier.bTryStack = tModifier.bTryStack;

            tSpecifier.FillInTargetedClass(eTargetedPlayerClass, NULL, TRUE);
        }
    }
    else
    {
        tSpecifier.hGBItem = hGBItem;
        tSpecifier.tLooteeParams = tLootee.tLooteeParams;
        tSpecifier.nAffixLevel = sItemGetAffixLevel(hGBItem);
        tSpecifier.eIQL = sGetItemQualityLevel(tLootee.tLooteeParams.nLevel, idACDLooter, tModifierToUse.gbidQualityClass, hGBItem.gbid, tLootee.tLooteeParams.bSuppressMagicAndGoldFind, nLocalEligiblePlayers);
        tSpecifier.flGoldMultiplier = tModifierToUse.flGoldMultiplier;
        tSpecifier.bIgnoreGoldRange = tModifier.bIgnoreGoldRanges;
        tSpecifier.bTryStack = tModifier.bTryStack;

        Player * ptPlayer = PlayerGetByACD(idACDLooter);
        sRollForTargetedItem(ptPlayer, tSpecifier, FALSE);
    }


    (GBIDToString(tSpecifier.hGBItem, FALSE).str(),  // arguments of one trace call that was compiled away
     LootGetDebugStringForItemQuality(tSpecifier).str());

    return TRUE;
}


static BOOL sLootModifierIsLegendary(const LootDropModifier & tLootDropModifier)
{

    return tLootDropModifier.eDropType == LDT_LEGENDARY || tLootDropModifier.eDropType == LDT_CLASS_TARGETED_LEGENDARY || (tLootDropModifier.eDropType == LDT_ITEM

        && ItemIsLegendary(tLootDropModifier.tSpecificItem.gbidItem))
        || tLootDropModifier.bBossSpecificDrop;
}








static float sGetLegendaryMagicFindModifier(const ActorCommonData & tACDLooter)
{
    float flModifier = 1.0f;
    if (!tACDLooter.AttributesGetInt(ATTRIBUTE_MAGIC_AND_GOLD_FIND_SUPPRESSED))
    {
        float flMagicFindMultiplier = 0.1f;
        float flMagicFindTotal = tACDLooter.AttributesGetFloat(ATTRIBUTE_MAGIC_FIND_TOTAL);
        if (ACDIsInTieredLootRun(tACDLooter))
        {
            flMagicFindTotal -= tACDLooter.AttributesGetFloat(ATTRIBUTE_MAGIC_FIND_HANDICAP);
            int32 nTieredLootRunLevel = ACDGetTieredLootRunLevel(tACDLooter);
            ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
            int32 nGameBalanceIndex = Math::Max(nTieredLootRunLevel, 0);
            nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);
            if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
            {
                const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];
                flMagicFindTotal += tTieredLootRunLevel.tLootRunValues.flMagicFind;
            }
        }
        flModifier = 1.0f + flMagicFindTotal * flMagicFindMultiplier;
    }
    return flModifier;
}













static float sGetLegendaryBonusMultiplierFromTieredRift(const int32 nTieredLootRunLevel)
{


    if (nTieredLootRunLevel == -1)
    {
        return 1.0f;
    }

    float flModifier = 1.0f;

    ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

    int32 nGameBalanceIndex = Math::Max(0, nTieredLootRunLevel);
    nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);

    if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
    {
        const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];

        flModifier = tTieredLootRunLevel.tLootRunValues.flLegendaryDropBonus + 1.0f;
    }

    return flModifier;
}

















void LootAddUnluckyAndAvengerTicks(ActorCommonData & tACD, uint32 uTicksToAdd)
{
    if (tACD.AttributesGetInt(ATTRIBUTE_ITEM_UNLUCKY_BONUS_ALLOW))
    {
        float flSecs = (float)uTicksToAdd * OO_TICKS_PER_SECOND;
        float flTotalSecs = tACD.AttributesGetFloat(ATTRIBUTE_ITEM_UNLUCKY_BONUS_SECS) + flSecs;
        tACD.AttributesSetFloat(ATTRIBUTE_ITEM_UNLUCKY_BONUS_SECS, flTotalSecs);
        float flTotalSecsAvengerSpawn = tACD.AttributesGetFloat(ATTRIBUTE_AVENGER_BUILDUP_SECS) + flSecs;
        tACD.AttributesSetFloat(ATTRIBUTE_AVENGER_BUILDUP_SECS, flTotalSecsAvengerSpawn);
        tACD.AttributesSetInt(ATTRIBUTE_ITEM_UNLUCKY_BONUS_ALLOW, FALSE);
    }
}









static float sGetUnluckyBonusChance(const ActorCommonData & tACDLooter)
{

    ScopedGlobalDefinitionLockR pdefGlobal;
    float flBonusSecs = tACDLooter.AttributesGetFloat(ATTRIBUTE_ITEM_UNLUCKY_BONUS_SECS);
    float flBonusHours = flBonusSecs / 3600.0f;
    flBonusHours = Math::Max(0.0f, flBonusHours - pdefGlobal->ptServerData->flUnluckyBonusMinHours);
    float flBonusChance = pdefGlobal->ptServerData->flUnluckyBonusChancePerHour * flBonusHours;
    return flBonusChance;
}



static float sGetRiftBonusMultiplier(const ActorCommonData & tACDLooter)
{

    if (SPlayerIsInRift(tACDLooter))
    {
        return 2.0f;
    }

    return 1.0f;
}


static float sGetLegendaryBonusMultiplierFromDifficulty(const ActorCommonData & tACDLooter)
{

    float flMult = 1.0f;

    if (!ACDIsInTieredLootRun(tACDLooter))
    {



        int32 nHandicap = GameGetHandicapLevel();
        if (nHandicap >= 0 && nHandicap < (int32)(sizeof(GlobalServerData::arLegendaryChanceMultiplier) / sizeof(float)))
        {
            ScopedGlobalDefinitionLockR pdefGlobal;
            flMult = pdefGlobal->ptServerData->arLegendaryChanceMultiplier[nHandicap];
        }
    }

    return flMult;
}




static void sConsumeUnluckyBonusPoolAmount(ActorCommonData & tACDLooter)
{

    tACDLooter.AttributesSetFloat(ATTRIBUTE_ITEM_UNLUCKY_BONUS_SECS, 0.0f);
}













#include "../SGameStats.h"
#include "../../Sigma/Stopwatch.h"






static void sEvaluateAndGetSpecifiersFromModifier(const LooteeInfo & tLootee,
                                                  const ACDID idACDLooter,
                                                  const TreasureClassDefinition & defTreasureClass,
                                                  const LootDropModifier & tLootDropModifier,
                                                  const GBID gbidInheritedQualityClass,
                                                  LootSpecifierArray & arSpecifiers,
                                                  XStringArray & tTCStack,
                                                  int32 nLocalEligiblePlayers,
                                                  SharedLootManager * ptSharedLoot)
{
    Player * ptPlayer = PlayerGetByACD(idACDLooter);

    if (tLootDropModifier.snoCondition != SNO_NONE
        && !ConditionEvaluate(tLootDropModifier.snoCondition, ptPlayer, idACDLooter, &tLootee))
    {
        return;
    }

    ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
    if (!ptACDLooter)
    {
        return;
    }



    float flChance = tLootDropModifier.flChance;

    BOOL bIsLegendary = sLootModifierIsLegendary(tLootDropModifier);

    if (!tLootee.tLooteeParams.bSuppressMagicAndGoldFind && bIsLegendary)
    {


        float flMagicFindMultiplier = sGetLegendaryMagicFindModifier(*ptACDLooter);
        flChance *= flMagicFindMultiplier;

        float flDifficultyBonusMultiplier = sGetLegendaryBonusMultiplierFromDifficulty(*ptACDLooter);
        flChance *= flDifficultyBonusMultiplier;

        float flRiftBonusMultiplier = sGetRiftBonusMultiplier(*ptACDLooter);
        flChance *= flRiftBonusMultiplier;

        float flTieredRiftBonusMultiplier = sGetLegendaryBonusMultiplierFromTieredRift(tLootee.tLooteeParams.nTieredLootRunLevel);
        flChance *= flTieredRiftBonusMultiplier;

        float flCommunityBuffBonusAmount = ptACDLooter->AttributesGetFloat(ATTRIBUTE_LEGENDARY_FIND_COMMUNITY_BUFF);
        flChance *= (flCommunityBuffBonusAmount + 1.0f);
    }


    if (!tLootee.tLooteeParams.bSuppressUnluckyBonus && bIsLegendary)
    {
        float flUnluckyBonusAmount = sGetUnluckyBonusChance(*ptACDLooter);
        flChance += flUnluckyBonusAmount;
    }

    if (flChance > 0.000001f
        && RandFloat_Internal(SGameGlobalsGet().rng) < flChance)
    {
        sGetSpecifiersFromModifier(tLootee, idACDLooter, tLootDropModifier, gbidInheritedQualityClass, defTreasureClass.tHeader.snoHandle, arSpecifiers, tTCStack, nLocalEligiblePlayers, ptSharedLoot);

        if (!tLootee.tLooteeParams.bSuppressUnluckyBonus && bIsLegendary)
        {

            if (tLootDropModifier.flChance < 0.999999f)
            {
                SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_RECEIVED_PITY_LEGENDARY, 0, 0, 1.0f);
                sConsumeUnluckyBonusPoolAmount(*ptACDLooter);
            }
        }
    }
}


// 2757 is an early continue: nPlayerLevel (2763) and the i loop (2764) are in the player block in the DWARF.
static void sDropSharedLootFromModifier(
    const LooteeInfo & tLootee,
                                        const TreasureClassDefinition & defTreasureClass,
                                        const LootDropModifier & tLootDropModifier,
                                        const GBID gbidInheritedQualityClass,
                                        SharedLootManager * ptSharedLoot)
{
    RNG tRNGStart = SGameGlobalsGet().rng;
    RNG tRNGToRestore = tRNGStart;

    PlayerList listEligiblePlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listEligiblePlayers, -1, FALSE, -1);

    POS posPlayer = listEligiblePlayers.GetHeadPosition();
    while (posPlayer)
    {

        Player * ptPlayer = listEligiblePlayers.GetNext(posPlayer);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDPlayer)
        {
            continue;
        }


        SGameGlobalsGet().rng = tRNGStart;


        BOOL bConditionsValid = TRUE;
        int32 nConditionsCount = ptSharedLoot->arConditionStack.GetArraySize();
        for (int32 i = 0; i < nConditionsCount; i++)
        {
            SNO snoCondition = ptSharedLoot->arConditionStack[i];
            if (snoCondition != SNO_NONE
                && !ConditionEvaluate(snoCondition, ptPlayer, ptPlayer->idPlayerACD, &tLootee))
            {
                bConditionsValid = FALSE;
                break;
            }
        }

        if (!bConditionsValid)
        {
            continue;
        }


        XStringArray tTCStack;
        LootSpecifierArray arLootSpecifiers;
        sEvaluateAndGetSpecifiersFromModifier(tLootee, ptPlayer->idPlayerACD, defTreasureClass, tLootDropModifier, gbidInheritedQualityClass, arLootSpecifiers, tTCStack, 1, NULL);


        int32 nLootSpecifiersCount = arLootSpecifiers.GetArraySize();
        if (nLootSpecifiersCount <= 0)
        {
            continue;
        }


        int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
        for (int32 i = 0; i < nLootSpecifiersCount; i++)
        {
            LootSpecifier & tLootSpecifier = arLootSpecifiers[i];

            tLootSpecifier.fAutoIdentify = ptSharedLoot->bAutoIdentify;
            tLootSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tLootSpecifier.hGBItem);
            if (tLootSpecifier.snoActorForFlippy == SNO_NONE)
            {
                arLootSpecifiers.RemoveAt(i);
                nLootSpecifiersCount--;
                i--;
                continue;
            }


            if (ItemHasLabel(tLootSpecifier.hGBItem.gbid, ITL_GOLD))
            {
                int32 nAmount = sLootFillGoldAmount(tLootSpecifier, nPlayerLevel, ptPlayer->idPlayerACD, tLootDropModifier.bIgnoreGoldRanges);
                if (nAmount <= 0)
                {
                    arLootSpecifiers.RemoveAt(i);
                    nLootSpecifiersCount--;
                    i--;
                }
            }
        }



        sRemoveConsumablePotions(arLootSpecifiers);
        LootModifyBeforeSpawning(arLootSpecifiers, *ptSharedLoot->ptACDLootee, ACDID_INVALID);


        FlippiesCreateOnActor(*ptSharedLoot->ptACDLootee, ptPlayer->idPlayerACD, arLootSpecifiers, 0, ptSharedLoot->dwGameEventNumber, ptSharedLoot->flCameraBiasDist, ptPlayer->idPlayerACD, &tLootee, NULL, (LootContext)0, NULL);






        tRNGToRestore = SGameGlobalsGet().rng;

    }
    SGameGlobalsGet().rng = tRNGToRestore;
}









static BOOL sPlayerHasItemCallback(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    PlayerHasItemCallbackData * ptCallbackData = (PlayerHasItemCallbackData *)pCallBackData;
    ActorCommonData & tACDHostItem = ACDGet(tInvLoc.idACDContainer);
    ACDID idACDGem = ACDInventoryGetSquareItem(tACDHostItem, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDGem != ACDID_INVALID)
    {
        ActorCommonData * ptACDGem = ACDTryToGet(idACDGem);
        if (ptACDGem && ptACDGem->hGB.gbid == ptCallbackData->gbidItem)
        {
            ptCallbackData->bFound = TRUE;
            return FALSE;
        }
    }

    return TRUE;
}







static const InventorySlotType scgtPlayerInventorySlotsJewels[16] = { SLOT_PLAYER_BACKPACK, SLOT_PLAYER_HEAD, SLOT_PLAYER_TORSO, SLOT_PLAYER_RIGHT_HAND, SLOT_PLAYER_LEFT_HAND, SLOT_PLAYER_HANDS, SLOT_PLAYER_WAIST, SLOT_PLAYER_FEET, SLOT_PLAYER_SHOULDERS, SLOT_PLAYER_LEGS, SLOT_PLAYER_BRACERS, SLOT_PLAYER_RIGHT_FINGER, SLOT_PLAYER_LEFT_FINGER, SLOT_PLAYER_NECK, SLOT_PLAYER_SHARED_STASH, SLOT_NONE };




















static BOOL sPlayerHasUpgradeableJewel(const Player * ptPlayer,
                                       GBID gbidItem)
{

    if (!ItemIsUpgradeableJewel(gbidItem))
        return FALSE;




    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;




    GBHandle hGBItem(GB_ITEMS, gbidItem);
    if (ACDInventoryGetItemCount(*ptACDPlayer, hGBItem, scgtPlayerInventorySlotsJewels, FALSE, TRUE) > 0)
        return TRUE;




    ACDID idACDHireling = PlayerGetHireling(ptACDPlayer->id);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling)
    {
        for (int32 s = SLOT_HIRELING_START; s < SLOT_HIRELING_END; s++)
        {
            InventorySlotType eSlots[2] = { s, SLOT_NONE };
            if (ACDInventoryGetItemCount(*ptACDHireling, hGBItem, eSlots, FALSE, TRUE) > 0)
                return TRUE;
        }
    }




    PlayerHasItemCallbackData tCallbackData;
    tCallbackData.gbidItem = gbidItem;
    tCallbackData.bFound = FALSE;

    for (int32 i = 0; i < 4; i++)
    {
        for (int32 s = 0; s < NUM_HIRELING_SLOTS; s++)
        {
            ACDID idACDItem = ptPlayer->tHirelingStable.tHireling[i].tItems.idACDItems[s];
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem
                && ItemHasSockets(*ptACDItem))
            {
                ACDInventoryIterateSlot(*ptACDItem, SLOT_ITEM_SOCKETS, NULL, sPlayerHasItemCallback, &tCallbackData, FALSE, FALSE);
                if (tCallbackData.bFound)
                    return TRUE;
            }
        }
    }
    return FALSE;
}





static BOOL sPlayerHasCosmetic(const Player * ptPlayer,
                               GBID gbidItem)
{






    for (int32 i = 0; i < ptPlayer->tCosmeticItemSavedData.tCosmeticItemData.nNumCosmeticItems; i++)
    {
        if (ptPlayer->tCosmeticItemSavedData.tCosmeticItemData.gbidCosmeticItems[i] == gbidItem)
            return TRUE;
    }
    return FALSE;
}






static const InventorySlotType scgtPlayerInventorySlotsCosmeticItems[3] = { SLOT_PLAYER_BACKPACK, SLOT_PLAYER_SHARED_STASH, SLOT_NONE };







static BOOL sPlayerHasCosmeticItem(const Player * ptPlayer,
                                   GBID gbidItem)
{

    if (!ItemIsCosmeticItem(gbidItem))
        return FALSE;




    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;




    GBHandle hGBItem(GB_ITEMS, gbidItem);
    if (ACDInventoryGetItemCount(*ptACDPlayer, hGBItem, scgtPlayerInventorySlotsCosmeticItems, FALSE, FALSE) > 0)
        return TRUE;

    return FALSE;
}




static BOOL sSpecifierArrayHasExistingGBID(const GBID gbidItem,
                                           const LootSpecifierArray & arSpecifiers)
{

    for (int32 j = 0; j < arSpecifiers.GetArraySize(); j++)
    {
        if (gbidItem != GBID_NONE && gbidItem ==
            arSpecifiers[j].hGBItem.gbid)
            return TRUE;
    }
    return FALSE;
}





static float sGetOneOfModifierWeightForSinglePlayer(const LootDropModifier & tLootDropModifier,
                                                    const Player * ptPlayer,
                                                    const ActorCommonData * ptACDLooter,
                                                    const LooteeInfo & tLootee,
                                                    const LootSpecifierArray & arSpecifiers)
{
    if (tLootDropModifier.snoCondition != SNO_NONE
        && !ConditionEvaluate(tLootDropModifier.snoCondition, ptPlayer, ptACDLooter->id, &tLootee))
    {
        return 0.0f;
    }

    if (ptPlayer
        && tLootDropModifier.eDropType == LDT_ITEM
        && tLootDropModifier.tSpecificItem.gbidItem != GBID_NONE)
    {
        GBID gbidItemToDrop = tLootDropModifier.tSpecificItem.gbidItem;

        ScopedItemDefinitionLockR pdefItem(gbidItemToDrop);








        if (Console::Online::GetGBIDBlacklistData(gbidItemToDrop, NULL))
        {
            return 0.0f;
        }











        if (ItemHasLabel(gbidItemToDrop, ITL_CRAFTING_PLAN))
        {

            BOOL bFoundUnknown = FALSE;
            for (int32 i = 0; i < (int32)(sizeof(pdefItem->gbidCraftingRecipesToGrant) / sizeof(pdefItem->gbidCraftingRecipesToGrant[0])); i++)
            {
                if (pdefItem->gbidCraftingRecipesToGrant[i] != GBID_NONE)
                {
                    if (!ItemCrafting_PlayerKnowsRecipe(*ptPlayer, pdefItem->gbidCraftingRecipesToGrant[i])
                        && !ItemCrafting_PlayerPossessesRecipe(*ptPlayer, gbidItemToDrop)
                        && !sSpecifierArrayHasExistingGBID(gbidItemToDrop, arSpecifiers))
                    {
                        bFoundUnknown = TRUE;
                        break;
                    }
                }
            }

            if (!bFoundUnknown)
            {
                return 0.0f;
            }
        }


        if (ItemIsUpgradeableJewel(tLootDropModifier.tSpecificItem.gbidItem)
            && sPlayerHasUpgradeableJewel(ptPlayer, tLootDropModifier.tSpecificItem.gbidItem))
        {
            return 0.0f;
        }


        if (ItemIsCosmeticItem(tLootDropModifier.tSpecificItem.gbidItem)
            && (sPlayerHasCosmetic(ptPlayer, tLootDropModifier.tSpecificItem.gbidItem)
                || sPlayerHasCosmeticItem(ptPlayer, tLootDropModifier.tSpecificItem.gbidItem)))
        {
            return 0.0f;
        }
    }

    float flWeight = (float)tLootDropModifier.nWeight;

    return flWeight;
}


static float sGetOneOfModifierWeightForPotentialSharedLoot(const LootDropModifier & tLootDropModifier,
                                                           const Player * ptPlayer,
                                                           const ActorCommonData * ptACDLooter,
                                                           const LooteeInfo & tLootee,
                                                           const SharedLootManager * ptSharedLoot,
                                                           const LootSpecifierArray & arSpecifiers)
{


    if (!ptSharedLoot || ptSharedLoot->arConditionStack.GetArraySize() <= 0)
    {
        return sGetOneOfModifierWeightForSinglePlayer(tLootDropModifier, ptPlayer, ptACDLooter, tLootee, arSpecifiers);
    }


    float flTotalWeight = 0.0f;

    PlayerList listEligiblePlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listEligiblePlayers, -1, FALSE, -1);

    POS posPlayer = listEligiblePlayers.GetHeadPosition();
    while (posPlayer)
    {

        Player * ptCurPlayer = listEligiblePlayers.GetNext(posPlayer);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptCurPlayer->idPlayerACD);
        if (!ptACDPlayer)  // inferred continue: 3121/3122 columns sit at loop-body depth, call block placed out of line
        {
            continue;
        }

        float flCurWeight = sGetOneOfModifierWeightForSinglePlayer(tLootDropModifier, ptCurPlayer, ptACDPlayer, tLootee, arSpecifiers);
        flTotalWeight += flCurWeight;
    }

    return flTotalWeight;
}


static uint64 sModifyQuantityForSpecificItem(const GBHandle hgbItem,
                                             uint64 uQuantity,
                                             const ActorCommonData & tACDLooter,
                                             const LooteeInfo & tLootee)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return uQuantity;





    if (tACDLooter.AttributesGetInt(ATTRIBUTE_DOUBLE_BOUNTY_BAGS))
    {
        ScopedItemDefinitionLockR pdefItem(hgbItem.gbid);
        if (pdefItem && (pdefItem->snoActor == GlobalSNOGet(GLOBALSNO_HORADRIC_CACHE)
            || pdefItem->snoActor == GlobalSNOGet(GLOBALSNO_HORADRIC_CACHE_BONUS)
            || pdefItem->snoActor == GlobalSNOGet(GLOBALSNO_HORADRIC_CACHE_GREATER)))
        {
            uQuantity *= 2;
        }
    }

    if (ItemHasLabel(hgbItem.gbid, ITL_BLOODSHARD))
    {
        if (tACDLooter.AttributesGetInt(ATTRIBUTE_DOUBLE_BLOODSHARDS))
        {
            uQuantity *= 2;
            uQuantity = Math::Min(uQuantity, ItemGetMaxStackSize(hgbItem, &tACDLooter, NULL));
        }

        float flBloodshardBonusPct = tACDLooter.AttributesGetFloat(ATTRIBUTE_BLOODSHARD_BONUS_PCT_POTION_BUFF);

        if (flBloodshardBonusPct > 0.000001f)
        {
            float flBonusAmount = flBloodshardBonusPct * uQuantity;
            uQuantity += (uint64)ceilf(flBonusAmount);
        }

        if (tLootee.tLooteeParams.nTieredLootRunLevel != -1
            && !tLootee.tLooteeParams.bSuppressTieredRiftBonuses)
        {
            ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

            int32 nGameBalanceIndex = Math::Max(0, tLootee.tLooteeParams.nTieredLootRunLevel);
            nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);

            if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
            {
                const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];

                uQuantity = Math::FloatToInt(uQuantity * (tTieredLootRunLevel.tLootRunValues.flBloodShardBonus + 1.0f));
            }
        }
    }

    Player * ptPlayer = PlayerGetByACD(tACDLooter.id);

    if (ItemHasLabel(hgbItem.gbid, ITL_PLATINUM))
    {
        if (GameGetDailyPlatinumLimit() == 0)
            return 0;



        if (hgbItem.gbid != GlobalGBIDGet(GLOBALGBID_CURRENCY_PLATINUM_UNCAPPED) && ptPlayer)
        {


            uQuantity = PlayerSubtractFromDailyPlatinum(*ptPlayer, uQuantity);
        }
    }


    if (ptPlayer)
    {

        if (ItemIsCosmeticItem(hgbItem.gbid)
            && (sPlayerHasCosmetic(ptPlayer, hgbItem.gbid)
                || sPlayerHasCosmeticItem(ptPlayer, hgbItem.gbid)))
        {
            return 0;
        }
    }

    return uQuantity;
}


static void sTryStackNewEntry(LootSpecifierArray & arSpecifiers,
                              int32 nOldSpecifiersCount,
                              const LootDropModifier & tLootDropModifier,
                              ActorCommonData & tACDLooter)
{

    if (tLootDropModifier.bTryStack)
    {
        int32 nNewSpecifiersCount = arSpecifiers.GetArraySize();
        if (nNewSpecifiersCount > nOldSpecifiersCount)
        {
            LootSpecifier & tSpeciferAdded = arSpecifiers[nNewSpecifiersCount - 1];
            uint64 uMaxStack = ItemGetMaxStackSize(tSpeciferAdded.hGBItem, &tACDLooter, NULL);
            if (uMaxStack > 0)
            {
                for (int32 nSpecifierIdx = 0; nSpecifierIdx < nNewSpecifiersCount - 1; nSpecifierIdx++)
                {
                    LootSpecifier & tSpecifierCurr = arSpecifiers[nSpecifierIdx];
                    if (tSpecifierCurr.hGBItem.gbid == tSpeciferAdded.hGBItem.gbid)
                    {

                        uint64 uCurrStack = tSpecifierCurr.uDesiredStackCount;
                        uint64 uDesiredStackCount = tSpeciferAdded.uDesiredStackCount;
                        if (uCurrStack <= uMaxStack)
                        {
                            uint64 uRoomLeft = uMaxStack - uCurrStack;
                            if (uRoomLeft >= uDesiredStackCount)
                            {

                                tSpecifierCurr.uDesiredStackCount = uCurrStack + uDesiredStackCount;
                                arSpecifiers.RemoveAt(nNewSpecifiersCount - 1);
                                return;
                            }
                            else
                            {


                                tSpecifierCurr.uDesiredStackCount = uMaxStack;
                            }
                        }
                    }
                }
            }
        }
    }
}





// NON_MATCHING: size 3364 vs 3372 (was 3396): entry spills now match; remaining: the shared 3521-3531 exit tail is laid out right after the ACDTryToGet test (original: at the end, reached by b.eq), bSharedLootConditionAdded is spilled here (w27 in the original) and defTreasureClass stays in x27 (original reloads it). Fixed: DWARF lines of sCheckNoDrop/sChooseFromWeightArray/params, continue in the weight helper and both j loops, nested resplendent if, nQuantity block
static BOOL sGetLootSpecifiers(
    const LooteeInfo & tLootee,
    const ACDID idACDLooter,
    const TreasureClassDefinition & defTreasureClass,
    const GBID gbidInheritedQualityClass,
    LootSpecifierArray & arSpecifiers,
    XStringArray & tTCStack,
    SNO snoEvaluatedCondition,
    int32 nLocalEligiblePlayers,
    SharedLootManager * ptSharedLoot)
{
    if (sCheckNoDrop(tLootee.flNoDropScalar, defTreasureClass.flNoDrop))
    {
        return TRUE;
    }

    if (defTreasureClass.nModifierCount <= 0)
    {
        return TRUE;
    }


    BOOL bSharedLootConditionAdded = FALSE;
    if (ptSharedLoot)
    {

        BOOL bIsSharedTreasureClass = (defTreasureClass.eChooseType == TCC_ALL_OF_SHARED || defTreasureClass.eChooseType == TCC_ONE_OF_SHARED);

        if (bIsSharedTreasureClass)
        {
            if (ptSharedLoot->arEvaluatedTreasureClasses.Find(defTreasureClass.tHeader.snoHandle) >= 0)
            {
                return TRUE;
            }
            ptSharedLoot->arEvaluatedTreasureClasses.Add_Tracked(defTreasureClass.tHeader.snoHandle);


            if (ptSharedLoot->arConditionStack.GetArraySize() <= 0)
            {
                ptSharedLoot->arConditionStack.Add_Tracked(snoEvaluatedCondition);
                bSharedLootConditionAdded = TRUE;
            }
        }


        if (!bSharedLootConditionAdded && snoEvaluatedCondition != SNO_NONE

            && ptSharedLoot->arConditionStack.GetArraySize() > 0)
        {
            ptSharedLoot->arConditionStack.Add_Tracked(snoEvaluatedCondition);
            bSharedLootConditionAdded = TRUE;
        }
    }

    BOOL fTraceLootEnabled = SGameTraceLootEnabled();
    if (fTraceLootEnabled)
    {
        tTCStack.push_back(CRefString(SNOToString(TREASURECLASS_SNO, defTreasureClass.tHeader.snoHandle, FALSE).str()));
        CRefString sStackText(tTCStack[0].str());
        for (uint32 i = 1; i < tTCStack.size(); i++)
        {
            sStackText += CRefString().Format("->%s", tTCStack[i].str());
        }

        sStackText.str();  // argument of a trace call that was compiled away
    }

    Player * ptPlayer = NULL;
    ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
    if (ptACDLooter)
    {
        ptPlayer = PlayerGetByACD(idACDLooter);

        if (ptPlayer)
        {
            if (defTreasureClass.tHeader.snoHandle == GlobalSNOGet(GLOBALSNO_TREASURECLASS_RESPLENDENT_CHEST))
            {
                SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_LOOTED_RESPLENDENT_IN_LEVEL_AREA, ACDGetLevelArea(*ptACDLooter), SGameGetStatDataKey(), 1.0f);
            }
        }

        switch (defTreasureClass.eChooseType)
        {
            case TCC_ONE_OF:
            case TCC_ONE_OF_SHARED:
            {
                float * pflWeight = (float *)alloca(sizeof(float) * defTreasureClass.nModifierCount);
                float flOrigWeightTotal = 0.0f;
                for (int32 i = 0; i < defTreasureClass.nModifierCount; i++)
                {
                    const LootDropModifier & tLootDropModifier = defTreasureClass.ptModifierList[i];
                    pflWeight[i] = sGetOneOfModifierWeightForPotentialSharedLoot(tLootDropModifier, ptPlayer, ptACDLooter, tLootee, ptSharedLoot, arSpecifiers);
                    flOrigWeightTotal += pflWeight[i];
                }

                const LootDropModifier * ptLootDropModifierSelected = NULL;
                if (flOrigWeightTotal > 0.000001f)
                {


                    for (int32 i = 0; i < defTreasureClass.nModifierCount; i++)
                    {
                        const LootDropModifier & tLootDropModifier = defTreasureClass.ptModifierList[i];
                        if (tLootDropModifier.eDropType == LDT_LEGENDARY || tLootDropModifier.eDropType == LDT_CLASS_TARGETED_LEGENDARY)
                        {

                            float flNaturalChance = pflWeight[i] / flOrigWeightTotal;
                            float flChance = flNaturalChance;

                            if (!tLootee.tLooteeParams.bSuppressMagicAndGoldFind)
                            {
                                float flMagicFindMultiplier = sGetLegendaryMagicFindModifier(*ptACDLooter);
                                flChance *= flMagicFindMultiplier;

                                float flDifficultyBonusMultiplier = sGetLegendaryBonusMultiplierFromDifficulty(*ptACDLooter);
                                flChance *= flDifficultyBonusMultiplier;

                                float flRiftBonusMultiplier = sGetRiftBonusMultiplier(*ptACDLooter);
                                flChance *= flRiftBonusMultiplier;

                                float flTieredRiftBonusMultiplier = sGetLegendaryBonusMultiplierFromTieredRift(tLootee.tLooteeParams.nTieredLootRunLevel);
                                flChance *= flTieredRiftBonusMultiplier;

                                float flCommunityBuffBonusAmount = ptACDLooter->AttributesGetFloat(ATTRIBUTE_LEGENDARY_FIND_COMMUNITY_BUFF);
                                flChance *= (flCommunityBuffBonusAmount + 1.0f);
                            }

                            if (!tLootee.tLooteeParams.bSuppressUnluckyBonus)
                            {
                                float flUnluckyBonusAmount = sGetUnluckyBonusChance(*ptACDLooter);
                                flChance += flUnluckyBonusAmount;
                            }

                            if (flChance > 0.000001f
                                && RandFloat_Internal(SGameGlobalsGet().rng) < flChance)
                            {
                                ptLootDropModifierSelected = &defTreasureClass.ptModifierList[i];

                                if (!tLootee.tLooteeParams.bSuppressUnluckyBonus)
                                {



                                    if (flNaturalChance < 0.999999f || tLootDropModifier.flChance < 0.999999f)
                                    {

                                        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_RECEIVED_PITY_LEGENDARY, 0, 0, 1.0f);
                                        sConsumeUnluckyBonusPoolAmount(*ptACDLooter);
                                    }
                                }
                                break;
                            }

                            pflWeight[i] = 0.0f;
                        }
                    }



                    if (!ptLootDropModifierSelected)
                    {
                        int32 iModifier = sChooseFromWeightArray(pflWeight, defTreasureClass.nModifierCount);
                        if (iModifier >= 0)
                        {

                            ptLootDropModifierSelected = &defTreasureClass.ptModifierList[iModifier];
                        }
                    }
                }
                if (!ptLootDropModifierSelected)
                {


                    if (fTraceLootEnabled)
                    {
                        tTCStack.pop_back();
                    }
                    return FALSE;
                }
                else  // inferred: nQuantity (3452 col 40) sits one level deeper, in its own DWARF lexical block
                {

                    uint64 nQuantity = RandRange_Internal(SGameGlobalsGet().rng, ptLootDropModifierSelected->nQuantity, ptLootDropModifierSelected->nQuantityDelta);

                    if (ptLootDropModifierSelected->eDropType == LDT_ITEM
                        && ptLootDropModifierSelected->tSpecificItem.gbidItem != GBID_NONE)
                    {
                        nQuantity = sModifyQuantityForSpecificItem(GBHandle(GB_ITEMS, ptLootDropModifierSelected->tSpecificItem.gbidItem), nQuantity, *ptACDLooter, tLootee);
                    }

                    for (int j = 0; j < nQuantity; j++)
                    {


                        if (ptSharedLoot && ptSharedLoot->arConditionStack.GetArraySize() > 0
                            && ptLootDropModifierSelected->eDropType != LDT_TREASURECLASS)
                        {
                            sDropSharedLootFromModifier(tLootee, defTreasureClass, *ptLootDropModifierSelected, gbidInheritedQualityClass, ptSharedLoot);
                            continue;  // inferred: 3472/3473 calls at col 25, loop-body depth
                        }

                        int32 nOldSpecifiersCount = arSpecifiers.GetArraySize();
                        sGetSpecifiersFromModifier(tLootee, idACDLooter, *ptLootDropModifierSelected, gbidInheritedQualityClass, defTreasureClass.tHeader.snoHandle, arSpecifiers, tTCStack, nLocalEligiblePlayers, ptSharedLoot);
                        sTryStackNewEntry(arSpecifiers, nOldSpecifiersCount, *ptLootDropModifierSelected, *ptACDLooter);
                    }
                }
                break;
            }

            case TCC_ALL_OF:
            case TCC_ALL_OF_SHARED:
            {
                for (int32 iModifier = 0; iModifier < defTreasureClass.nModifierCount; iModifier++)
                {
                    LootDropModifier & tLootDropModifier = defTreasureClass.ptModifierList[iModifier];


                    uint64 nQuantity = RandRange_Internal(SGameGlobalsGet().rng, tLootDropModifier.nQuantity, tLootDropModifier.nQuantityDelta);

                    if (tLootDropModifier.eDropType == LDT_ITEM
                        && tLootDropModifier.tSpecificItem.gbidItem != GBID_NONE)
                    {
                        nQuantity = sModifyQuantityForSpecificItem(GBHandle(GB_ITEMS, tLootDropModifier.tSpecificItem.gbidItem), nQuantity, *ptACDLooter, tLootee);
                    }


                    for (int j = 0; j < nQuantity; j++)
                    {


                        if (ptSharedLoot && ptSharedLoot->arConditionStack.GetArraySize() > 0
                            && tLootDropModifier.eDropType != LDT_TREASURECLASS)
                        {
                            sDropSharedLootFromModifier(tLootee, defTreasureClass, tLootDropModifier, gbidInheritedQualityClass, ptSharedLoot);
                            continue;  // inferred: 3508/3509 calls at col 25 (with case indent), loop-body depth
                        }

                        int32 nOldSpecifiersCount = arSpecifiers.GetArraySize();
                        sEvaluateAndGetSpecifiersFromModifier(tLootee, idACDLooter, defTreasureClass, tLootDropModifier, gbidInheritedQualityClass, arSpecifiers, tTCStack, nLocalEligiblePlayers, ptSharedLoot);
                        sTryStackNewEntry(arSpecifiers, nOldSpecifiersCount, tLootDropModifier, *ptACDLooter);
                    }
                }

                break;
            }
            default:
                break;
        }
    }


    if (bSharedLootConditionAdded)
    {
        ptSharedLoot->arConditionStack.RemoveAt(ptSharedLoot->arConditionStack.GetArraySize() - 1);
    }

    if (fTraceLootEnabled)
    {
        tTCStack.pop_back();
    }
    return TRUE;
}


static int32 sChooseGoldFromDistEntry(
    const LootDistEntry * ptLootDistEntry)
{

    float flGoldWeights[3];
    for (int32 i = 0; i < 3; i++)
    {
        flGoldWeights[i] = (float)ptLootDistEntry->nGoldWeight[i];
    }

    int32 nGoldMin = 0;
    int32 nGoldDelta = 0;
    int32 nGoldWeightIdx = sChooseFromWeightArray(flGoldWeights, 3);
    if (nGoldWeightIdx >= 0)
    {
        nGoldMin = ptLootDistEntry->nGoldMin[nGoldWeightIdx];
        nGoldDelta = ptLootDistEntry->nGoldDelta[nGoldWeightIdx];
    }

    return RandRange_Internal(SGameGlobalsGet().rng, nGoldMin, nGoldDelta);
}



static int32 sLootFillGoldAmount(LootSpecifier & tSpecifier,
                                 const int32 nLooteeLevel,
                                 const ACDID idACDLooter,
                                 const BOOL bIgnoreGoldRange)
{
    const GameBalanceDefinition * pLootDistDef;
    const LootDistEntry * ptLootDistEntry = sGetLootDistEntry(nLooteeLevel, pLootDistDef, -1);


    tSpecifier.nGoldAmount = sChooseGoldFromDistEntry(ptLootDistEntry);

    if (bIgnoreGoldRange)
    {
        tSpecifier.nGoldAmount = 1;
    }

    ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
    if (ptACDLooter
        && !ptACDLooter->AttributesGetInt(ATTRIBUTE_MAGIC_AND_GOLD_FIND_SUPPRESSED))
    {
        float flGoldFind = LootGetFindValue(*ptACDLooter, ATTRIBUTE_GOLD_FIND_TOTAL);
        if (tSpecifier.tLooteeParams.fIsTreasureBag || tSpecifier.tLooteeParams.bSuppressMagicAndGoldFind)
        {
            flGoldFind = 0.0f;
        }

        if (tSpecifier.tLooteeParams.nTieredLootRunLevel >= 0 && !tSpecifier.tLooteeParams.fIsTreasureBag)
        {
            ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));

            int32 nGameBalanceIndex = Math::Max(0, tSpecifier.tLooteeParams.nTieredLootRunLevel);
            nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);

            if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
            {
                const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex];

                flGoldFind += tTieredLootRunLevel.tLootRunValues.flGoldFind;
            }
        }

        tSpecifier.nGoldAmount = (int32)(tSpecifier.nGoldAmount * (flGoldFind + 1.0f));
    }

    tSpecifier.nGoldAmount = Math::FloatRoundToInt(tSpecifier.flGoldMultiplier * tSpecifier.nGoldAmount);

    GBUnlockR(pLootDistDef);

    return tSpecifier.nGoldAmount;
}






static void sLootSetGoldValue(ActorCommonData & tACD, const int32 nGoldAmount)
{


    tACD.AttributesSetInt(ATTRIBUTE_GOLD, nGoldAmount);
    tACD.AttributesSetInt(ATTRIBUTE_ITEM_QUALITY_LEVEL, IQL_NORMAL);
}





















static void sSetupLootSpecifierForGold(
    LootSpecifier & tSpecifier,
    int32 nGoldAmount)
{
    tSpecifier.hGBItem.eType = GB_ITEMS;
    tSpecifier.hGBItem.gbid = sGetGoldGBIDFromAmount(nGoldAmount);
    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    tSpecifier.eIQL = IQL_NORMAL; tSpecifier.nAffixLevel = 0;  // one line: original rows 3646-3651 hold six statements and the eIQL/nAffixLevel stores are merged at 3649
    tSpecifier.nGoldAmount = nGoldAmount;
    tSpecifier.fAutoIdentify = FALSE;
}



void LootDropExplicitGold(const ActorCommonData & tACDLootee, const ACDID idACDAssigned, const int32 nGoldAmount)
{
    if (nGoldAmount <= 0)
        return;

    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();
    sSetupLootSpecifierForGold(tSpecifier, nGoldAmount);

    uint32 dwGameEventNumber = SGameGetEventNumber();
    FlippiesCreateOnActor(tACDLootee, idACDAssigned, arLootSpecifiersToSpawn, 0, dwGameEventNumber, 0.0f, idACDAssigned, NULL, NULL, LC_UNKNOWN, NULL);
}










static void sRemoveSpecifierFromArray(LootSpecifierArray & arSpecifiers,
                                      int32 nIndex)
{



    int32 nArraySize = arSpecifiers.GetArraySize();

    nArraySize--;

    if (nIndex < nArraySize)
    {
        SigmaMemoryCopy(&arSpecifiers[nIndex], &arSpecifiers[nArraySize], sizeof(LootSpecifier));
    }
    arSpecifiers.SetArraySize_Tracked(nArraySize);
}



BOOL LootGetLootSpecifiers(const LooteeInfo & tLootee,
                           const ACDID idACDLooter,
                           LootSpecifierArray & arSpecifiers,
                           const TreasureClassDefinition * pdefTreasureClass,
                           const BOOL bIgnoreQuestItems,
                           int32 nLocalEligiblePlayers,
                           SharedLootManager * ptSharedLoot)
{


    Stopwatch tStopwatch;


    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;


    if (ptItemAppGlobals->tRootItemType.arChildItemTypes.size() == 0
        && ptItemAppGlobals->tRootItemType.arItems.size() == 0)
    {
        return FALSE;
    }




    if (idACDLooter != ACDID_INVALID)
    {
        const ActorID idActorLooter = (ActorID)ACDGetOwner(idACDLooter);
        const Actor * ptActorLooter = ActorGet(idActorLooter);

        ScopedActorDefinitionLockR ptDefActorLooter(ptActorLooter->snoActor);
        BOOL bForbiddenByTag = FALSE;
        if (TagMapTagExists(ptDefActorLooter->hTagMap, TAG_ACTOR_NEVER_A_LOOTER))
        {
            bForbiddenByTag = TagMapGetInt(ptDefActorLooter->hTagMap, TAG_ACTOR_NEVER_A_LOOTER);
        }

        if (bForbiddenByTag == TRUE)
        {
            return FALSE;
        }
    }







    BOOL fRetVal = FALSE;
    if (tLootee.snoTreasureClass == SNO_NONE
        && !pdefTreasureClass)
    {
        CRefString sGbid("None");
        if (tLootee.hGBDebug.eType != -1)
        {
            sGbid = GBHandleToString(tLootee.hGBDebug);
        }
    }
    else
    {



        BOOL fUsingPassedInTreasureClass = TRUE;

        if (!pdefTreasureClass)
        {
            fUsingPassedInTreasureClass = FALSE;
            pdefTreasureClass = (const TreasureClassDefinition *)gptGroupDefTreasureClass->LockR(tLootee.snoTreasureClass, FALSE);
        }



        XStringArray tTCStack;
        float flNumRolls = 1.0f;

        ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
        if (ptACDLooter)
        {


            float flTreasureFind = ptACDLooter->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_TREASURE_FIND, pdefTreasureClass->tHeader.snoHandle));
            flNumRolls = flTreasureFind + 1.0f;



            float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
            flNumRolls -= flRoll;
        }

        while (flNumRolls > 0.0f)
        {
            fRetVal = sGetLootSpecifiers(tLootee, idACDLooter, *pdefTreasureClass, GBID_NONE, arSpecifiers, tTCStack, SNO_NONE, nLocalEligiblePlayers, ptSharedLoot) || fRetVal;

            flNumRolls -= 1.0f;
        }

        if (!fUsingPassedInTreasureClass)
        {
            gptGroupDefTreasureClass->UnlockR((void *&)pdefTreasureClass);
        }
    }

    if (fRetVal)
    {

        int32 i = 0;
        while (i < arSpecifiers.GetArraySize())
        {
            LootSpecifier & tSpecifier = arSpecifiers[i];
            if (ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_GOLD))
            {
                int32 nAmount = sLootFillGoldAmount(tSpecifier, tLootee.tLooteeParams.nLevel, idACDLooter, tSpecifier.bIgnoreGoldRange);

                if (nAmount == 0)
                {
                    sRemoveSpecifierFromArray(arSpecifiers, i);
                    continue;
                }
            }

            if (bIgnoreQuestItems)
            {
                BOOL bUnique = ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_QUEST) && !ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_BOOK);
                if (bUnique)
                {
                    sRemoveSpecifierFromArray(arSpecifiers, i);
                    continue;
                }
            }

            if (tLootee.tLooteeParams.bDisableNewCraftingDrops)
            {
                BOOL bIsCraftingRelated = ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_CRAFTING)
                    || ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_CRAFTING_PLAN)
                    || ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_CRAFTING_PLAN_LEGENDARY)
                    || ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_CRAFTING_PLAN_GENERIC);
                if (bIsCraftingRelated)
                {
                    sRemoveSpecifierFromArray(arSpecifiers, i);
                    continue;
                }
            }


            SNO snoActor = GameBalanceGetActorSNO(tSpecifier.hGBItem);
            if (snoActor == SNO_NONE)
            {
                return FALSE;
            }










            tSpecifier.snoActorForFlippy = snoActor;
            i++;
        }
    }









    sRemoveConsumablePotions(arSpecifiers);

    float fDropLootTime = tStopwatch.Check() * 1000.0f;
    if (fDropLootTime >= 15.0f)
    {
        SGameTraceLootEnabled();  // condition of a trace call that was compiled away
    }


    return fRetVal;
}


static const LootDistEntry * sGetLootDistEntry(
    const int32 nLooteeLevel,
    const GameBalanceDefinition *& pLootDistDef,
    int32 nOverrideLevel)
{
    int32 nLooteeLevelToUse = nLooteeLevel;
    if (nOverrideLevel != -1)
    {
        nLooteeLevelToUse = nOverrideLevel;
    }

    pLootDistDef = LootDistributionTableLockR();

    if (!pLootDistDef->tLootDistTable.ptLootDistributions
        || pLootDistDef->tLootDistTable.serLootDistributions.dwSize == 0)
    {
        return NULL;
    }


    const LootDistEntry * ptLootDistTable = pLootDistDef->tLootDistTable.ptLootDistributions;
    const LootDistEntry * ptLootDistEntry = NULL;
    int32 nNumDistributionEntries = pLootDistDef->tLootDistTable.serLootDistributions.dwSize / sizeof(LootDistEntry);



    int32 nMinLevel = 0;
    int32 nMinLevelIdx = 0;
    int32 nMaxLevel = 0;
    int32 nMaxLevelIdx = 0;


    for (int32 nDistIndex = 0; nDistIndex < nNumDistributionEntries; nDistIndex++)
    {
        if (ptLootDistTable[nDistIndex].nLevel == nLooteeLevelToUse)
        {
            ptLootDistEntry = &ptLootDistTable[nDistIndex]; break;
        }


        if (ptLootDistTable[nDistIndex].nLevel < nMinLevel)
        {
            nMinLevel = ptLootDistTable[nDistIndex].nLevel;
            nMinLevelIdx = nDistIndex;
        }
        if (ptLootDistTable[nDistIndex].nLevel > nMaxLevel)
        {
            nMaxLevel = ptLootDistTable[nDistIndex].nLevel;
            nMaxLevelIdx = nDistIndex;
        }
    }


    if (ptLootDistEntry == NULL)
    {
    if (nLooteeLevelToUse > nMaxLevel)
    {
        ptLootDistEntry = &ptLootDistTable[nMaxLevelIdx];
    }
    if (nLooteeLevelToUse < nMinLevel)
    {
        ptLootDistEntry = &ptLootDistTable[nMinLevelIdx];
    } }
    return ptLootDistEntry;
}



static const ItemLevelModifier * sGetItemLevelModifiers(const int32 nLooteeLevel,
                                                        const GBID gbidItem,
                                                        const GameBalanceDefinition *& pGBDef,
                                                        BOOL & fAlwaysMagic)
{
    Item * ptItem = NULL;
    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;


    pGBDef = NULL;
    if (!ptItemAppGlobals->mapItems.Lookup(gbidItem, ptItem))
    {
        DisplayInternalError(NULL, "", 3965, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }




    fAlwaysMagic = ptItem->ptItemType->fAlwaysMagic;

    pGBDef = (const GameBalanceDefinition *)gptGroupDefGameBalance->LockRFast(GlobalSNOGet(GLOBALSNO_GB_ITEM_LEVEL_MODIFIERS));


    if (!pGBDef->tItemLevelModTable.ptItemLevelModifiers
        || pGBDef->tItemLevelModTable.serItemLevelModifiers.dwSize == 0)
    {
        DisplayInternalError(NULL, "", 3978, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    const ItemLevelModifier * ptItemLevelModTable = pGBDef->tItemLevelModTable.ptItemLevelModifiers;
    int32 nNumEntries = pGBDef->tItemLevelModTable.serItemLevelModifiers.dwSize / sizeof(ItemLevelModifier);

    int32 nLevelDifference = nLooteeLevel - ptItem->nItemLevel;
    for (int32 i = 0; i < nNumEntries; i++)
    {
        if (ptItemLevelModTable[i].nLevelDiff == nLevelDifference)
        {
            return &ptItemLevelModTable[i];
        }
    }
    return NULL;
}





SNO ACDGetTreasureClass(const ActorCommonData & tACD, const ACDID idACDLooter)
{
    SNO snoTreasureClass = SNO_NONE;
    if (tACD.AttributesGetInt(ATTRIBUTE_DROPS_NO_LOOT))
    {
        snoTreasureClass = GlobalSNOGet(GLOBALSNO_TREASURECLASS_DROPNOTHING);
    }
    else if (tACD.IsAvenger() && !GameIsSeasonal())
    {
        if (tACD.ptAvengerData->eAvengerSource == AVENGER_SOURCE_FRIEND)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(tACD.ptAvengerData->nPlayerIndex);
            if (ptPlayer && ptPlayer->idPlayerACD == idACDLooter)
            {
                LootDropFriendGift(ptPlayer, TRUE, IQL_LEGENDARY);
            }
        }

        snoTreasureClass = GlobalSNOGet(GLOBALSNO_TREASURECLASS_AVENGER_REWARD);
    }
    else
    {
        snoTreasureClass = tACD.AttributesGetInt(ATTRIBUTE_TREASURE_CLASS);
        if (snoTreasureClass == SNO_NONE)
        {
            if (tACD.IsMonster())
            {
                snoTreasureClass = LootMonsterGetTreasureClass(tACD, idACDLooter, tACD.eMonsterRarity);
            }
            else if (tACD.IsGizmo())
            {
                ScopedActorDefinitionLockR ptActorDef(tACD.snoActor);
                SNO snoTreasureClass0 = TagMapGetSNO(ptActorDef->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);
                snoTreasureClass = snoTreasureClass0;
            }
            else if (tACD.IsItem())
            {
                snoTreasureClass = SalvageCommon::GetTreasureClass(tACD);
            }
        }
    }

    return snoTreasureClass;
}












































































































static GBID sGetItemBasedOnItem(GBID gbidBaseItem, int32 nLooteeMLvl, uint32 dwItemFlagToCheck)
{



    GameCommonData & tGCD = SGameGetGameCommonData();
    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;

    int32 nMemberItems = ptItemAppGlobals->nItems;
    int32 nMaxWeight = 0;
    int32 nNumWeights = 0;


    for (int32 nItemIdx = 0; nItemIdx < nMemberItems; nItemIdx++)
    {

        if (!(ptItemAppGlobals->ptItems[nItemIdx].dwFlags & dwItemFlagToCheck))
            continue;



        if (ptItemAppGlobals->ptItems[nItemIdx].dwFlags & (1 << ITEM_NO_RANDOM_DROP_BIT))
            continue;






        if (nLooteeMLvl > 0 && ptItemAppGlobals->ptItems[nItemIdx].nItemLevel > nLooteeMLvl)
            continue;




        if (ptItemAppGlobals->ptItems[nItemIdx].gbidBaseItem != gbidBaseItem)
            continue;





        tGCD.arItemWeights[nNumWeights].nItemWeight = 1;
        tGCD.arItemWeights[nNumWeights].nItemIndex = nItemIdx;
        nMaxWeight += tGCD.arItemWeights[nNumWeights].nItemWeight;
        nNumWeights++;
    }


    if (nNumWeights > 0)
    {
        int32 nChosen = sChooseFromItemWeightArray(nNumWeights, nMaxWeight);


        return ptItemAppGlobals->ptItems[tGCD.arItemWeights[nChosen].nItemIndex].gbid;
    }



    return GBID_NONE;
}







// sGetGoldGBIDFromAmount (0x7100894820): emitted among the deferred statics after LootPickGem, so static
static GBID sGetGoldGBIDFromAmount(uint32 nGoldAmount)
{
    if (nGoldAmount > 2000)
    {
        return GlobalGBIDGet(GLOBALGBID_ITEM_GOLD5);
    }
    else if (nGoldAmount > 500)
    {
        return GlobalGBIDGet(GLOBALGBID_ITEM_GOLD4);
    }
    else if (nGoldAmount > 100)
    {
        return GlobalGBIDGet(GLOBALGBID_ITEM_GOLD3);
    }
    else if (nGoldAmount > 10)
    {
        return GlobalGBIDGet(GLOBALGBID_ITEM_GOLD2);
    }
    else if (nGoldAmount > 0)
    {
        return GlobalGBIDGet(GLOBALGBID_ITEM_GOLD1);
    }
    else
    {
        DisplayInternalError(NULL, "", 4451, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


}


static ACDID sLootCreateGold(LootSpecifier & tSpecifier, const WorldPlace & tPlace)
{


    tSpecifier.hGBItem.gbid = sGetGoldGBIDFromAmount(tSpecifier.nGoldAmount);

    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);
    tSpecifier.hGBItem.Export(*tGenerator.mutable_gb_handle());
    tGenerator.set_seed(RandGetSeed(SGameGlobalsGet().rng));


    if (GameIsSeasonal())
    {
        tGenerator.set_season_created(GameGetCurrentSeason());
    }

    ACDID idACD = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, tPlace, 0.0f, 0);
    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(idACD);
        sLootSetGoldValue(tACD, tSpecifier.nGoldAmount);
    }
    return idACD;
}




static BOOL sAffixIsAlreadyIdentified(GBHandle hGB)
{



    ScopedItemAffixLockR ptAffix(hGB.gbid);
    if (!ptAffix)
        return FALSE;






    return ptAffix->eAffixType == 2 || ptAffix->eAffixType == 6;


}



ACDID LootQuickCreateItem(const GBHandle & hGB, const BOOL fAutoIdentify)
{
    LootSpecifier tSpecifier;

    tSpecifier.hGBItem = hGB;
    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(hGB);
    tSpecifier.fAutoIdentify = fAutoIdentify;

    LootDetermineInherentAffixes(tSpecifier);

    tSpecifier.eIQL = ItemDetermineQualityLevel("LootQuickCreateItem", tSpecifier, tSpecifier.nNumAffixes);

    return LootCreateItem(tSpecifier, cgplaceNull, NULL);
}























// NON_MATCHING: 2 of 663 insns: register choice x9/x10 for the ptItems base in the inlined ItemsChooseLevelAppropriateItemFromBaseItem / sChooseFromItemWeightArray tail.
ACDID LootCreateItem(
    LootSpecifier & tSpecifier,
    const WorldPlace & tPlace,
    const ActorCommonData * ptACDSource)
{
    // AffixType 2 and 6 above have no names in the adopted headers.


    if (tSpecifier.hGBItem.gbid == GBID_NONE || !GBIDExists(tSpecifier.hGBItem.gbid, GB_ITEMS))
        return ACDID_INVALID;



    if (ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_GOLD))
    {
        return sLootCreateGold(tSpecifier, tPlace);
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {

        if (tSpecifier.hGBItem.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_BLIZZCON_PRIZE) &&  // nested: cond col 13, `&&` col 87
            !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT + 1))
        {
            return ACDID_INVALID;
        }
    }



    BOOL fBoundToGame = FALSE;
    BOOL fAccountBound = tSpecifier.fAccountBound;
    BOOL fBindOnEquip = tSpecifier.fBindOnEquip;
    (void)fBoundToGame;




































































    if (ItemIsConsumablePotion(tSpecifier.hGBItem.gbid))
    {
        return ACDID_INVALID;
    }
    // Rows 4446:24/4446:5: a switch on eIQL (LEGENDARY/SPECIAL share one block; body indented 12 per columns 4452-4477).
    switch (tSpecifier.eIQL)
    {
    case IQL_LEGENDARY:
    case IQL_SPECIAL:
        {
            GBID gbidLegendaryItem;
            if ((tSpecifier.eIQL == IQL_LEGENDARY && (ItemIsLegendary(tSpecifier.hGBItem.gbid) || ItemIsGift(tSpecifier.hGBItem.gbid))) ||
                (tSpecifier.eIQL == IQL_SPECIAL && ItemIsSpecial(tSpecifier.hGBItem.gbid)))
            {
                gbidLegendaryItem = tSpecifier.hGBItem.gbid;
            }
            else
            {
                uint32 dwFlagToCheck = (tSpecifier.eIQL == IQL_LEGENDARY) ? (1 << ITEM_LEGENDARY_BIT) : (1 << ITEM_SPECIAL_BIT);
                int32 nLooteeMLevel = -1;
                if (tSpecifier.tLooteeParams.nForcedILevel <= 0)
                {
                    nLooteeMLevel = tSpecifier.tLooteeParams.nLevel;
                }

                gbidLegendaryItem = sGetItemBasedOnItem(tSpecifier.hGBItem.gbid, nLooteeMLevel, dwFlagToCheck);
            }

            if (gbidLegendaryItem == GBID_NONE)
            {

                ItemQualityLevel eIQLFallback = (tSpecifier.eIQL == IQL_LEGENDARY) ? IQL_RARE_6_AFFIX : IQL_MAGIC_3_AFFIX;
                tSpecifier.eIQL = eIQLFallback;
            }
            else
            {
                tSpecifier.hGBItem.gbid = gbidLegendaryItem;
            }
        }
        break;
    default:
        break;
    }
    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);

    const D3::Items::Generator & tDefault = D3::Items::Generator::default_instance();











    tGenerator.set_seed(RandGetSeed(SGameGetRNG()));


    tGenerator.mutable_gb_handle()->set_game_balance_type(tSpecifier.hGBItem.eType);
    tGenerator.mutable_gb_handle()->set_gbid(tSpecifier.hGBItem.gbid);
    ItemQualityLevel eIQLToUse = tSpecifier.eIQL;
    tGenerator.set_stack_size(tSpecifier.uDesiredStackCount);






    BOOL fIdentified = TRUE;












    if (tSpecifier.nLegendaryItemLevel != 0 &&
        !tSpecifier.tLooteeParams.bIgnoreLevelRestriction &&
        ItemCanBeMagical(tSpecifier.hGBItem.gbid))
    {
        tGenerator.set_legendary_item_level(tSpecifier.nLegendaryItemLevel);

        if (ItemIsLegendary(tSpecifier.hGBItem.gbid))
        {
            ScopedItemDefinitionLockR ptItemDef(tSpecifier.hGBItem.gbid);
            if (ptItemDef->dwFlags & (1 << ITEM_TEST_ONLY_BIT))
            {
                tGenerator.set_legendary_base_item_gbid(tSpecifier.hGBItem.gbid);
            }
            else
            {
                GBID gbidBaseItem = ItemsChooseLevelAppropriateItemFromBaseItem(ptItemDef->gbidBaseItem, tSpecifier.nLegendaryItemLevel);
                if (!GBIDExists(gbidBaseItem, GB_ITEMS))
                    return ACDID_INVALID;
                tGenerator.set_legendary_base_item_gbid(gbidBaseItem);
            }






        }
    }

    if (!ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_GOLD) &&
        tSpecifier.nNumAffixes == 0 && tSpecifier.unk_a0.empty())
    {
        LootDetermineInherentAffixes(tSpecifier);
        LootDetermineRandomAffixes(tSpecifier);
    }



    for (int32 i = 0; i < tSpecifier.nNumAffixes; i++)
    {
        tGenerator.add_base_affixes(tSpecifier.arAffixGBIDs[i]);
        GBHandle hGBAffix(GB_AFFIXES, tSpecifier.arAffixGBIDs[i]);
        fIdentified = fIdentified && sAffixIsAlreadyIdentified(hGBAffix);
    }
    for (blz::unordered_map<int, int>::iterator it = tSpecifier.unk_a0.begin(); it != tSpecifier.unk_a0.end(); ++it)
    {
        int32 nAffix = it->first; int32 nUnlockRank = it->second;
        D3::Items::UnlockableRandomAffixProperties * ptProperties = tGenerator.add_unlockable_random_affix_properties();
        ptProperties->set_affix(nAffix);
        ptProperties->set_unlock_rank(nUnlockRank);
    }


    if (ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_JUNK))
    {
        eIQLToUse = IQL_NORMAL;
    }





















    if (eIQLToUse >= IQL_AFFIX_COUNT_START && eIQLToUse < IQL_AFFIX_COUNT_END && tSpecifier.nNumAffixes <= 0)
    {
        eIQLToUse = IQL_NORMAL;
    }













    if (eIQLToUse != tDefault.item_quality_level())
    {
        tGenerator.set_item_quality_level(eIQLToUse);
    }


    switch (eIQLToUse)
    {
    case IQL_MAGIC_1_AFFIX: case IQL_MAGIC_2_AFFIX: case IQL_MAGIC_3_AFFIX: fIdentified = TRUE; break;
    case IQL_LEGENDARY: fIdentified = FALSE; break;
    case IQL_SPECIAL: fIdentified = TRUE; break;
    default:


        if (eIQLToUse >= IQL_RARE_AFFIX_START && eIQLToUse <= IQL_RARE_AFFIX_END)
        {

            AppGlobals & tAppGlobals = sgtAppGlobals;
            if (!(tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_DONT_IDENTIFY_RARES_BIT)))
                fIdentified = TRUE;
        }
        break;
    }


    if (ItemIsUpgradeableJewel(tSpecifier.hGBItem.gbid))
    {
        fIdentified = TRUE;
        int32 nJewelRank = sub_710050e990(tSpecifier.hGBItem.gbid);
        if (nJewelRank > 0)
        {
            tGenerator.set_jewel_rank(nJewelRank);
        }
    }


    if (tSpecifier.fAutoIdentify)
    {
        fIdentified = TRUE;
    }

    uint32 uFlags = tGenerator.flags();
    uFlags = fIdentified ? (uFlags | (1 << IGF_IDENTIFIED_BIT)) : (uFlags & ~(1 << IGF_IDENTIFIED_BIT));
    uFlags = (tSpecifier.eAncientRank == ANCIENT_RANK_ANCIENT) ? (uFlags | (1 << IGF_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_ANCIENT_BIT));
    uFlags = (tSpecifier.eAncientRank == ANCIENT_RANK_PRIMAL_ANCIENT) ? (uFlags | (1 << IGF_PRIMAL_ANCIENT_BIT)) : (uFlags & ~(1 << IGF_PRIMAL_ANCIENT_BIT));
    tGenerator.set_flags(uFlags);







    // Inference: DWARF loclists show fAccountBound = 1 and fBindOnEquip = 0 after the ItemHasLabel call, so a socketable bind-on-equip item becomes account-bound here.
    if (fBindOnEquip && ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_SOCKETABLE))
    {
        fBindOnEquip = FALSE;
        fAccountBound = TRUE;
    }















    // Inference: no rows; this test folds away once the flags above are threaded.

    if (fBindOnEquip)
    {
        tGenerator.set_item_binding_level(IBL_ACCOUNT_EQUIP);
    }
    else if (fAccountBound)
    {
        tGenerator.set_item_binding_level(IBL_ACCOUNT_PICKUP);
    }


    if (GameIsSeasonal())
    {
        tGenerator.set_season_created(GameGetCurrentSeason());
    }

    ACDID idACD = SItemGenerate(OnlineService::ItemId::INVALID, tGenerator, tPlace, 0.0f, 0);
    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(idACD);
        if (eIQLToUse >= IQL_RARE_AFFIX_START && eIQLToUse <= IQL_RARE_AFFIX_END)
        {
            LootDetermineRareName(tACD, tSpecifier);
        }
    }































    return idACD;
}


int32 LootBuildValidAffixList(GBHandleList & lstAffixes, const LootSpecifier & tSpecifier, const AffixType eLastAffixType, const uint32 dwFlags)
{
    ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;
    ItemsValidateCachedAffixTableEntries();
    int32 bRes = LootBuildValidAffixList(lstAffixes, tSpecifier, eLastAffixType, ptItemAppGlobals->ptAffixes, ptItemAppGlobals->nAffixes, ptItemAppGlobals->ptCachedAffixTableEntyPtrs, dwFlags);
    return bRes;
}





















int32 LootBuildValidAffixList(GBHandleList & lstAffixes,
                              const LootSpecifier & tSpecifier,
                              const AffixType eLastAffixType,
                              const Affix * pPotentialAffixes,
                              const int32 nPotentialAffixCount,
                              AffixTableEntry ** ptPotentialAffixTableEntries,
                              const uint32 dwFlags)
{
    int32 nMaxWeight = 0;



    int32 nAffixLevelBonus = 0;
    if (tSpecifier.hGBItem.gbid != GBID_NONE)
    {
        const GameBalanceDefinition * pdefGBItems = NULL;
        const ItemDefinition * pItemDef = (const ItemDefinition *)GBLockR(tSpecifier.hGBItem, pdefGBItems, TRUE);
        if (pItemDef)
        {
            nAffixLevelBonus = pItemDef->nAffixLevelBonus;
            GBUnlockR(pdefGBItems);
        }
    }

    if (tSpecifier.tLooteeParams.nLevel > 0)
    {
        int32 nLevelDiff = tSpecifier.tLooteeParams.nLevel - tSpecifier.nAffixLevel;
        nAffixLevelBonus = Math::Max(nAffixLevelBonus, nLevelDiff);
    }

    ItemAffixSpecifier tAffixSpecifier;
    tSpecifier.FillInItemAffixSpecifier(tAffixSpecifier);
    tAffixSpecifier.nAffixLevel += nAffixLevelBonus;
    tAffixSpecifier.eLastAffixType = eLastAffixType;
    tAffixSpecifier.dwFlags = dwFlags;
    if (tSpecifier.tLooteeParams.fIsCrafter)
    {
        tAffixSpecifier.dwFlags |= (1 << 5);
    }
    if (tSpecifier.tLooteeParams.fIsVendor)
    {
        tAffixSpecifier.dwFlags |= (1 << 6);
    }

    AffixTableEntry * ptSpecifierAffixTableEntries[LOOT_MAX_AFFIXES];
    const GameBalanceDefinition * ptSpecifierAffixGameBalances[LOOT_MAX_AFFIXES];



    for (int32 i = 0; i < tSpecifier.nNumAffixes; i++)
    {
        GBHandle hGB(GB_AFFIXES, tSpecifier.arAffixGBIDs[i]);
        ptSpecifierAffixTableEntries[i] = (AffixTableEntry *)GBLockR(hGB, ptSpecifierAffixGameBalances[i], TRUE);
    }




    BOOL bPrimalPrioritizeSockets = (dwFlags & (1 << 10)) != 0;
    BOOL bPrimalDisallowSockets = (dwFlags & (1 << 11)) != 0;
    int32 nHighestSocketCount = -1;
    int32 nHighestSocketAffixIndex = -1;

    for (int32 i = 0; i < nPotentialAffixCount; i++)
    {
        const Affix & tAffix = pPotentialAffixes[i];
        tAffixSpecifier.ptAffix = &tAffix;
        if (ItemCanAddAffix(tAffixSpecifier, ptPotentialAffixTableEntries[i], ptSpecifierAffixTableEntries))
        {
            GBHandle hGB(GB_AFFIXES, tAffix.gbid);

            const GameBalanceDefinition * pdefGB;
            AffixTableEntry * ptAffix = (AffixTableEntry *)GBTryLockR(hGB, pdefGB);
            if (ptAffix)
            {
                GBUnlockR(pdefGB);

                int32 nFrequencyWeight = LootGetAffixFrequencyWeight(ptAffix, &tSpecifier);

                if (ItemAffixAddsSockets(tAffix.gbid))
                {
                    if (bPrimalPrioritizeSockets)
                    {

                        int32 nNumSockets = ItemAffixGetNumSockets(tSpecifier.hGBItem.gbid, tAffix.gbid);
                        if (nNumSockets > nHighestSocketCount)
                        {
                            nHighestSocketCount = nNumSockets;
                            nHighestSocketAffixIndex = i;
                        }
                    }
                    else if (bPrimalDisallowSockets)
                    {
                        continue;
                    }
                }


                nMaxWeight += nFrequencyWeight;
                lstAffixes.AddTail_Tracked(hGB);
            }
        }
    }






    if (nHighestSocketAffixIndex >= 0)
    {
        const Affix & tAffix = pPotentialAffixes[nHighestSocketAffixIndex];
        tAffixSpecifier.ptAffix = &tAffix;
        GBHandle hGB(GB_AFFIXES, tAffix.gbid);

        const GameBalanceDefinition * pdefGB;
        AffixTableEntry * ptAffix = (AffixTableEntry *)GBTryLockR(hGB, pdefGB);
        if (ptAffix)
        {
            GBUnlockR(pdefGB);

            int32 nFrequencyWeight = LootGetAffixFrequencyWeight(ptAffix, &tSpecifier);
            nMaxWeight = nFrequencyWeight;
            lstAffixes.RemoveAll();
            lstAffixes.AddTail_Tracked(hGB);
        }
    }

    for (int32 i = 0; i < tSpecifier.nNumAffixes; i++)
    {
        GBUnlockR(ptSpecifierAffixGameBalances[i]);
    }

    return nMaxWeight;
}



GBID LootChooseAffixFromList(const LootSpecifier & tSpecifier, AffixType & eLastAffixType, const GBHandleList & lstAffixes, int32 nMaxWeight)
{

    int32 nCurrentWeight = 0;

    if (nMaxWeight <= 0)
        return GBID_NONE;





    int32 nDropRoll = Rand_Internal(SGameGlobalsGet().rng, nMaxWeight - 1);
    GBHandle hGB = cghgbNone;
    POS pos = lstAffixes.GetHeadPosition();
    while (pos)
    {
        hGB = lstAffixes.GetNext(pos);



        ScopedItemAffixLockR ptAffix(hGB.gbid);
        int32 nFrequencyWeight = LootGetAffixFrequencyWeight(ptAffix, &tSpecifier);

        nCurrentWeight += nFrequencyWeight;

        if (nDropRoll < nCurrentWeight)
        {
            eLastAffixType = ptAffix->eAffixType;
            break;
        }
    }

















    if (tSpecifier.eIQL == IQL_LEGENDARY)
    {
        sUprankLegendaryAffix(hGB.gbid);
        sUprankAffixByAncientRank(hGB.gbid, tSpecifier);
    }


    return hGB.gbid;
}


int32 LootGetAffixFrequencyWeight(const AffixTableEntry * ptAffix, const LootSpecifier * ptSpecifier)
{
    if (ptSpecifier)
    {
        return ItemAffixGetFrequencyWeight(ptAffix, ptSpecifier->eTargetedPlayerClass, ptSpecifier->eTargetedHirelingClass);
    }
    return ItemAffixGetFrequencyWeight(ptAffix, PLAYER_CLASS_NONE, 0);
}






static void sChooseAffixFromFamily(LootSpecifier & tSpecifier,
                                   const GBID & gbidAffixFamily)
{

    GBHandleList lstAffixes(GBGetHandlePool());


    LootBuildValidAffixList(lstAffixes, tSpecifier, -1, 0);

    int32 nMaxWeight = 0;
    GBHandle hGB = cghgbNone;
    POS pos = lstAffixes.GetHeadPosition();
    while (pos)
    {
        POS posCur = pos;
        hGB = lstAffixes.GetNext(pos);

        ScopedItemAffixLockR ptAffix(hGB.gbid);



        if (ptAffix->gbidAffixFamily0 != gbidAffixFamily &&
            ptAffix->gbidAffixFamily1 != gbidAffixFamily)
        {
            lstAffixes.RemoveAt(posCur);
            continue;
        }

        int32 nFrequencyWeight = LootGetAffixFrequencyWeight(ptAffix, &tSpecifier);

        nMaxWeight += nFrequencyWeight;
    }


    AffixType eLastAffixType = -1;
    GBID gbidAffix = LootChooseAffixFromList(tSpecifier, eLastAffixType, lstAffixes, nMaxWeight);
    if (gbidAffix != GBID_NONE)
    {
        tSpecifier.AddAffix(hGB.gbid);
    }
}


static BOOL sGetLegendaryAffixesFromFamily(LootSpecifier & tSpecifier,
                                           GBHandleList & lstPossibleAffixes,
                                           const GBID & gbidAffixFamily,
                                           int32 nLegendaryAffixFamilyLevel,
                                           BOOL bUseLegendaryAllowedTypes,
                                           BOOL bChooseOnlyOne)
{
    GBHandleList lstAffixes(GBGetHandlePool());


    uint32 uFlags = (1 << 0) | (1 << 1) | (1 << 2);


    if (bUseLegendaryAllowedTypes)
    {
        uFlags |= (1 << 3);
    }
    LootBuildValidAffixList(lstAffixes, tSpecifier, -1, uFlags);

    int32 nWinningLevel = 0;
    int32 bMultipleWinners = FALSE;
    GBHandle hGBWinner = cghgbNone;
    POS pos = lstAffixes.GetHeadPosition();
    while (pos)
    {
        POS posCur = pos;
        GBHandle hGB = lstAffixes.GetNext(pos);

        ScopedItemAffixLockR ptAffix(hGB.gbid);



        if (ptAffix->gbidAffixFamily0 != gbidAffixFamily &&
            ptAffix->gbidAffixFamily1 != gbidAffixFamily)
        {
            lstAffixes.RemoveAt(posCur);
            continue;
        }

        if (ptAffix->nAffixLevelMin == nWinningLevel)
        {
            bMultipleWinners = TRUE;
        }


        if (ptAffix->nAffixLevelMin > nWinningLevel &&
            ptAffix->nAffixLevelMin <= nLegendaryAffixFamilyLevel)
        {
            hGBWinner = hGB;
            nWinningLevel = ptAffix->nAffixLevelMin;
            bMultipleWinners = FALSE;
        }
    }

    if (bMultipleWinners)
    {
        GBHandleList lstWinnerAffixes(GBGetHandlePool());


        pos = lstAffixes.GetHeadPosition();
        while (pos)
        {
            GBHandle hGB = lstAffixes.GetNext(pos);
            ScopedItemAffixLockR ptAffix(hGB.gbid);


            if (ptAffix->nAffixLevelMin == nWinningLevel && ptAffix->nAffixLevelMin <= nLegendaryAffixFamilyLevel)
            {
                if (bChooseOnlyOne)
                {

                    lstWinnerAffixes.AddTail_Tracked(hGB);
                }
                else
                {
                    lstPossibleAffixes.AddTail_Tracked(hGB);
                }
            }
        }

        if (bChooseOnlyOne)
        {

            int32 itemRand = Rand_Internal(SGameGlobalsGet().rng, lstWinnerAffixes.Count() - 1);
            POS itemPos = lstWinnerAffixes.GetHeadPosition();
            for (int32 i = 0; i < itemRand; i++) { lstWinnerAffixes.GetNext(itemPos); }
            hGBWinner = lstWinnerAffixes.GetAt(itemPos);
            lstPossibleAffixes.AddTail_Tracked(hGBWinner);
        }
    }
    else
    {
        lstPossibleAffixes.AddTail_Tracked(hGBWinner);
    }

    pos = lstPossibleAffixes.GetHeadPosition();
    while (pos)
    {
        GBHandle & hGB = lstPossibleAffixes.GetNext(pos);
        if (hGB.gbid == GBID_NONE)
        {
            return FALSE;
        }



        if (tSpecifier.eIQL == IQL_LEGENDARY)
        {
            sUprankLegendaryAffix(hGB.gbid);
            sUprankAffixByAncientRank(hGB.gbid, tSpecifier);
        }
    }

    return TRUE;
}


static void sChooseLegendaryAffixFromFamily(LootSpecifier & tSpecifier,
                                            const GBID & gbidAffixFamily,
                                            int32 nLegendaryAffixFamilyLevel,
                                            BOOL bUseLegendaryAllowedTypes)
{

    GBHandleList lstAffixes(GBGetHandlePool());

    BOOL bResult = sGetLegendaryAffixesFromFamily(tSpecifier, lstAffixes, gbidAffixFamily, nLegendaryAffixFamilyLevel, bUseLegendaryAllowedTypes, TRUE);
    if (!bResult || lstAffixes.IsEmpty())
    {
        return;
    }







    GBID gbidAffix = lstAffixes.GetHead().gbid;
    if (tSpecifier.eAncientRank == 2 && ItemAffixAddsSockets(gbidAffix))
    {
        GBID gbidItemType = ItemGetItemTypeGBID(tSpecifier.hGBItem.gbid);
        if (ItemTypeIsOfType(gbidItemType, GlobalGBIDGet(GLOBALGBID_ITEMTYPE_WEAPON)))
        {

            tSpecifier.bAddExtraMajorAffix = TRUE;
            return;
        }
    }


    tSpecifier.AddAffix(gbidAffix);
}


// Removed by the linker in the shipped image (DWARF: low_pc 0, 0x20 bytes, line 5182); written from its DWARF record only. It is the second caller that keeps sGetLegendaryAffixesFromFamily out of line.
bool GetLegendaryAffixesFromFamily(LootSpecifier & tSpecifier,
                                   GBHandleList & lstPossibleAffixes,
                                   const GBID & gbidAffixFamily,
                                   int32 nLegendaryAffixFamilyLevel,
                                   BOOL bUseLegendaryAllowedTypes)
{

    BOOL bResult = sGetLegendaryAffixesFromFamily(tSpecifier, lstPossibleAffixes, gbidAffixFamily, nLegendaryAffixFamilyLevel, bUseLegendaryAllowedTypes, FALSE);
    return bResult != FALSE;
}









void LootSpecifier::AddRandomAffixes(AffixType & eLastAffixType,
                                     int32 nMajorAffixes,
                                     int32 nMinorAffixes,
                                     int32 nRandomAffixes)

{
    int32 nTotalRandomAffixes = nMajorAffixes + nMinorAffixes + nRandomAffixes;

    int32 nForceLegendaryAffix = 0;
    if (eIQL == IQL_LEGENDARY)
    {
        nForceLegendaryAffix = nMinLegendaryAffixes;
    }

    if (nNumAffixes + nTotalRandomAffixes > LOOT_MAX_AFFIXES)
    {








        nTotalRandomAffixes = LOOT_MAX_AFFIXES - nNumAffixes;
    }

    uint32 dwFlags = 0;
    for (int32 i = 0; i < nTotalRandomAffixes; i++)
    {









        GBHandleList lstAffixes(GBGetHandlePool());

        if (eAncientRank == 2)
        {
            GBID gbidItemType = ItemGetItemTypeGBID(hGBItem.gbid);
            if (ItemTypeIsOfType(gbidItemType, GlobalGBIDGet(GLOBALGBID_ITEMTYPE_ARMOR)) ||
                ItemTypeIsOfType(gbidItemType, GlobalGBIDGet(GLOBALGBID_ITEMTYPE_JEWELRY)))
            {
                dwFlags |= (1 << 10);
            }
            else if (ItemTypeIsOfType(gbidItemType, GlobalGBIDGet(GLOBALGBID_ITEMTYPE_WEAPON)))
            {
                dwFlags |= (1 << 11);
            }
        }

        if (nForceLegendaryAffix > 0) { dwFlags |= (1 << 2); } else { dwFlags &= ~(1 << 2); }
        nForceLegendaryAffix--;

        BOOL bNeedMajorAffixes = nMajorAffixes > 0;
        BOOL bNeedMinorAffixes = nMinorAffixes > 0;

        if (bNeedMajorAffixes)
        {
            nMajorAffixes--;
            dwFlags |= (1 << 7);
            dwFlags &= ~(1 << 8);
        }
        else if (bNeedMinorAffixes)
        {
            nMinorAffixes--; dwFlags &= ~(1 << 7);
            dwFlags |= (1 << 8);
        }


        int32 nMaxWeight = LootBuildValidAffixList(lstAffixes, *this, eLastAffixType, dwFlags);

        GBID gbidAffix = LootChooseAffixFromList(*this, eLastAffixType, lstAffixes, nMaxWeight);
        if (gbidAffix != GBID_NONE)
        {
            AddAffix(gbidAffix);
        }
    }
}

// NON_MATCHING: 1 of 83 insns: the first "nTotalAffixes + nNumAffixes" add (shard path) has its operands swapped (add w10, w8, w9 vs w9, w8); tried the operand order, ">= 6", an explicit sum and the check inside the while condition. 2.7.7 adds the unk_e0 trim loop.
void LootDetermineRandomAffixes(LootSpecifier & tSpecifier)
{
    int32 nRandomAffixes = 0;
    int32 nMajorAffixes = 0;
    int32 nMinorAffixes = 0;
    LootDetermineMajorMinorAffixCount(tSpecifier, nMajorAffixes, nMinorAffixes, 0, 0, 0);
    ItemGetBonusRandomAffixes(tSpecifier.hGBItem, nMajorAffixes, nMinorAffixes, nRandomAffixes);

    if (tSpecifier.bAddExtraMajorAffix)
    {
        nMajorAffixes++;
    }

    if (ItemIsShard(tSpecifier.hGBItem.gbid))
    {
        nRandomAffixes = ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_GREATERSHARD) ? 3 : 2;
        nMajorAffixes = 0;
        nMinorAffixes = 0;
    }

    int32 nTotalAffixes = nRandomAffixes + nMajorAffixes + nMinorAffixes;
    if (tSpecifier.unk_e0 && nTotalAffixes + tSpecifier.nNumAffixes > 5)
    {
        do
        {
            if (nMinorAffixes > 0)
                nMinorAffixes--;
            else if (nRandomAffixes > 0)
                nRandomAffixes--;
            else if (nMajorAffixes > 0)
                nMajorAffixes--;
            else
                return;
            nTotalAffixes = nRandomAffixes + nMajorAffixes + nMinorAffixes;
        } while (nTotalAffixes + tSpecifier.nNumAffixes > 5);
    }

    if (nTotalAffixes > 0 && tSpecifier.nAffixLevel > 0)
    {
        AffixType eLastAffixType = -1;
        tSpecifier.AddRandomAffixes(eLastAffixType, nMajorAffixes, nMinorAffixes, nRandomAffixes);
    }
}




void LootDetermineInherentAffixes(LootSpecifier & tSpecifier)

{
    if (!ItemIsLegendary(tSpecifier.hGBItem.gbid) &&
        !ItemIsSpecial(tSpecifier.hGBItem.gbid))
    {


        GBHandle hGBItemType = ItemsGetItemTypeGB(tSpecifier.hGBItem);

        const GameBalanceDefinition * pdefGB;
        ItemTypeDefinition * ptItemType = (ItemTypeDefinition *)GBLockR(hGBItemType, pdefGB, TRUE);
        if (!ptItemType)
        {
            return;
        }



        GBHandle hGBAffix(GB_AFFIXES, GBID_NONE);
        for (int32 i = 0; i < 3; i++)
        {
            if (ptItemType->gbidInherentAffix[i] != GBID_NONE)
            {
                hGBAffix.gbid = ptItemType->gbidInherentAffix[i];
                sUprankAffixByAncientRank(hGBAffix.gbid, tSpecifier);
                tSpecifier.AddAffix(hGBAffix.gbid);
            }
        }
        for (int32 i = 0; i < 1; i++)
        {
            if (ptItemType->gbidInherentAffixFamily[i] != GBID_NONE)
            {

                sChooseAffixFromFamily(tSpecifier, ptItemType->gbidInherentAffixFamily[i]);
            }
        }
        GBUnlockR(pdefGB);
    }
    else
    {

        const GameBalanceDefinition * pdefGB;
        ItemDefinition * pItemDef = (ItemDefinition *)GBLockR(tSpecifier.hGBItem, pdefGB, TRUE);
        if (!pItemDef) { return; }

        for (int32 i = 0; i < 6; i++)
        {
            if (pItemDef->gbidLegendaryAffixFamily[i] != GBID_NONE)
            {
                int32 nAffixFamilyLevel = pItemDef->nLegendaryAffixFamilyLevel[i];
                if (tSpecifier.nLegendaryItemLevel != 0)
                {
                    nAffixFamilyLevel = tSpecifier.nLegendaryItemLevel;
                }
                else if (tSpecifier.tLooteeParams.fIsCrafter &&
                         nAffixFamilyLevel < 1)
                {
                    nAffixFamilyLevel = tSpecifier.nAffixLevel;
                }

                sChooseLegendaryAffixFromFamily(tSpecifier,
                                                pItemDef->gbidLegendaryAffixFamily[i],
                                                nAffixFamilyLevel,
                                                pItemDef->bUseLegendaryAllowedTypes[i]);
            }
        }

        GBUnlockR(pdefGB);
    }
}



void LootDetermineMajorMinorAffixCount(LootSpecifier & tSpecifier, int32 & nMajor, int32 & nMinor, int32 nTotalAffixes, int32 nInherentMajor, int32 nInherentMinor)
{
    BOOL bAddMajor = FALSE;
    BOOL bAddMinor = FALSE;

    if (nTotalAffixes == 0)
    {
        switch (tSpecifier.eIQL)
        {
        case IQL_INFERIOR:
        case IQL_SUPERIOR:
        case IQL_MAGIC_1_AFFIX:
            nTotalAffixes = 1;
            break;
        case IQL_MAGIC_2_AFFIX:
            nTotalAffixes = 2;
            break;
        case IQL_MAGIC_3_AFFIX:
            nTotalAffixes = 3;
            break;
        case IQL_RARE_4_AFFIX:
            nTotalAffixes = 4;
            break;
        case IQL_RARE_5_AFFIX:
            nTotalAffixes = 5;
            break;
        case IQL_RARE_6_AFFIX:
            nTotalAffixes = 6;
            break;
        default:
            break;
        }
    }

    nMajor = 0;
    nMinor = 0;

    int32 nRand = Rand_Internal(SGameGlobalsGet().rng, 2);
    switch (nTotalAffixes)
    {
    case 1:
        bAddMinor = nRand == 0;
        bAddMajor = !bAddMinor;
        break;
    case 2:
        nMajor = 1;
        bAddMajor = nRand == 0;
        bAddMinor = !bAddMajor;
        break;
    case 3:
        nMajor = 2;
        nMinor = 1;
        break;
    case 4:
        nMajor = 2;
        nMinor = 1;
        bAddMinor = nRand == 0;
        bAddMajor = !bAddMinor;
        break;
    case 5:
        nMajor = 3;
        nMinor = 1;
        bAddMajor = nRand == 0;
        bAddMinor = !bAddMajor;
        break;
    case 6:
        nMajor = 4;
        nMinor = 2;
        break;
    default:
        nMajor = 0;
        nMinor = 0;
        break;
    }

    if (nMajor < nInherentMajor)
    {
        nMajor = nInherentMajor;
        nMinor = nTotalAffixes - nInherentMajor;
    }
    else if (nMinor < nInherentMinor)
    {
        nMajor = nTotalAffixes - nInherentMinor;
        nMinor = nInherentMinor;
    }
    else if (bAddMajor)
    {
        nMajor++;
    }
    else if (bAddMinor)
    {
        nMinor++;
    }
}
































static BOOL sAffixExcludedFromAffixes(GBID gbidAffix, LootSpecifier & tSpecifier)
{


    ScopedItemAffixLockR ptAffix(gbidAffix);




    BOOL bFoundConflict = FALSE;
    uint32 uSocketCount = ItemAffixGetNumSockets(GBID_NONE, gbidAffix);
    for (int i = 0; i < 6; i++)
    {   GBID gbidExistingAffix = tSpecifier.arAffixGBIDs[i];

        if (gbidExistingAffix == GBID_NONE)
            continue;



        uSocketCount += ItemAffixGetNumSockets(GBID_NONE, gbidExistingAffix);

        ScopedItemAffixLockR ptExistingAffix(gbidExistingAffix);

        for (int j = 0; j < 6; j++)
        {   if ((ptAffix->gbidExclusionCategory != GBID_NONE &&

                 ptAffix->gbidExclusionCategory == ptExistingAffix->arExcludedCategories[j]) ||





                (ptExistingAffix->gbidExclusionCategory != GBID_NONE && ptExistingAffix->gbidExclusionCategory == ptAffix->arExcludedCategories[j]))
            {
                bFoundConflict = TRUE;
                break;
            }
        }





        if (bFoundConflict) break;
    }





    return bFoundConflict || uSocketCount > 3;

}




inline int32 sSelectRandomEntryFromStringList(SNO snoStringList)
{

    if (snoStringList == SNO_NONE)
        return -1;
    ScopedStringListDefinitionLockR pdefStringList(snoStringList);
    if (pdefStringList->dwStringCount == 0)
        return -1;



    return Rand_Internal(SGameGlobalsGet().rng, pdefStringList->dwStringCount - 1);

}





void LootDetermineRareName(ActorCommonData & tACD, const LootSpecifier & tSpecifier)
{

    RareItemName tRareItemName;

    tRareItemName.fItemNameIsPrefix = Rand_Internal(SGameGlobalsGet().rng, 1);


    int32 nAffixIndex = Rand_Internal(SGameGlobalsGet().rng, tSpecifier.nNumAffixes - 1);

    {   GBID gbidToFind = tSpecifier.arAffixGBIDs[nAffixIndex];

        ScopedItemAffixLockR ptAffix(gbidToFind);


        tRareItemName.snoAffixStringList = tRareItemName.fItemNameIsPrefix ? ptAffix->snoRareNameSuffixStringList : ptAffix->snoRareNamePrefixStringList;
        tRareItemName.nAffixStringListIndex = sSelectRandomEntryFromStringList(tRareItemName.snoAffixStringList);
    }




    GBHandle hGBItem(GB_ITEMS, tACD.hGB.gbid);

    const GameBalanceDefinition * pdefGB;
    ItemDefinition * pItemDef = (ItemDefinition *)GBLockR(hGBItem, pdefGB, TRUE);

    if (pItemDef)
    {   SNO snoStringList = tRareItemName.fItemNameIsPrefix ? pItemDef->snoRareNamePrefixStringList : pItemDef->snoRareNameSuffixStringList;
        tRareItemName.nItemStringListIndex = sSelectRandomEntryFromStringList(snoStringList);
        GBUnlockR(pdefGB);
    }
    else
    {
        tRareItemName.nItemStringListIndex = 0;
    }

    ACDSetRareItemName(tACD, tRareItemName);
}


































































































































































































































void LootGiveTreasureToPlayer(const LooteeInfo & tLootee, ActorCommonData & tACDPlayer, XPooledDataIDList * listLootACDs, LootContext eLootContext, BOOL bTryStashFirst, BOOL bForceAccountBinding)
{




    LootSpecifierArray arLootSpecifiers;



    if (LootGetLootSpecifiers(tLootee, tACDPlayer.id, arLootSpecifiers, NULL, FALSE, 1, NULL))
    {
        int32 nNumItems = arLootSpecifiers.GetArraySize();

        for (int32 i = 0; i < nNumItems; i++)
        {
            arLootSpecifiers[i].fAutoIdentify = TRUE;


            ACDID idACDItem = LootCreateItem(arLootSpecifiers[i], cgplaceNull, NULL);
            if (idACDItem == ACDID_INVALID)
                continue;



            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (!ItemHasLabel(tACDItem.hGB.gbid, ITL_QUEST))
            {
                Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
                tACDItem.SetAssignedHeroID(ptPlayer);
            }

            if (listLootACDs && !ItemIsGold(tACDItem))
            {
                listLootACDs->AddTail_Tracked(idACDItem);
            }



            if (ItemIsCosmeticItem(tACDItem.hGB.gbid))
            {
                CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID(tACDItem.hGB.gbid);
                switch (eCosmeticType)
                {
                case COSMETIC_ITEM_PET:
                    SCosmeticItems_LearnPet(tACDPlayer.id, tACDItem.hGB.gbid, false);
                    break;
                case COSMETIC_ITEM_WINGS:
                case COSMETIC_ITEM_PENNANT:
                case COSMETIC_ITEM_FRAME:

                    SCosmeticItems_LearnCosmeticItem(tACDPlayer.id, tACDItem.hGB.gbid, eCosmeticType, false);
                    break;
                }
                continue;
            }

            if (bForceAccountBinding)
            {
                tACDItem.AttributesSetInt(ATTRIBUTE_CONSOLE_PROMO_ITEM, TRUE);
            }



            SGameStatsRecordLoot(tACDItem, eLootContext, &tLootee);

            SACDInventoryPickupOrSpillOntoGround(tACDPlayer, tACDItem, bTryStashFirst, NULL, TRUE);
        }
    }
}









static const BossMonsterPlayerKilledFlags cgarBossMonsterPlayerKilledFlags[] =
{
    { GLOBALSNO_MONSTER_SKELETON_KING, BOSSKILLFLAG_SKELETON_KING },
    { GLOBALSNO_MONSTER_BUTCHER, BOSSKILLFLAG_BUTCHER },
    { GLOBALSNO_MONSTER_BELIAL, BOSSKILLFLAG_BELIAL },
    { GLOBALSNO_MONSTER_AZMODAN, BOSSKILLFLAG_AZMODAN },
    { GLOBALSNO_MONSTER_DIABLO, BOSSKILLFLAG_DIABLO },
    { GLOBALSNO_MONSTER_WRETCHED_QUEEN, BOSSKILLFLAG_WRETCHED_QUEEN },
    { GLOBALSNO_MONSTER_SPIDER_QUEEN, BOSSKILLFLAG_SPIDER_QUEEN },
    { GLOBALSNO_MONSTER_MAGHDA, BOSSKILLFLAG_MAGHDA },
    { GLOBALSNO_MONSTER_ZOLTUN_KULLE, BOSSKILLFLAG_ZOLTAN_KULLE },
    { GLOBALSNO_MONSTER_GLUTTONY, BOSSKILLFLAG_GLUTTONY },
    { GLOBALSNO_MONSTER_SIEGEBREAKER_DEMON, BOSSKILLFLAG_SIEGEBREAKER_DEMON },
    { GLOBALSNO_MONSTER_MISTRESS_OF_PAIN, BOSSKILLFLAG_MISTRESS_OF_PAIN },
    { GLOBALSNO_MONSTER_DESPAIR, BOSSKILLFLAG_DESPAIR },
    { GLOBALSNO_MONSTER_IZUAL, BOSSKILLFLAG_IZUAL },
    { GLOBALSNO_MONSTER_UNIQUE_UNBURIED, BOSSKILLFLAG_UNIQUE_UNBURIED },
    { GLOBALSNO_MONSTER_MALTHAEL, BOSSKILLFLAG_MALTHAEL },
    { GLOBALSNO_MONSTER_GREED, BOSSKILLFLAG_GREED },
    { GLOBALSNO_MONSTER_ADRIA, BOSSKILLFLAG_ADRIA },
    { GLOBALSNO_MONSTER_URZAEL, BOSSKILLFLAG_URZAEL },
    { GLOBALSNO_MONSTER_UNIQUE_DEATH_MAIDEN, BOSSKILLFLAG_UNIQUE_DEATH_MAIDEN },
    { GLOBALSNO_MONSTER_ANNIVERSARY_DIABLO, BOSSKILLFLAG_ANNIVERSARY_DIABLO },
};

BOOL LootMonsterSupportsFirstKillTreasureClass(SNO snoMonster)
{
    for (int32 i = 0; i < (int32)(sizeof(cgarBossMonsterPlayerKilledFlags) / sizeof(cgarBossMonsterPlayerKilledFlags[0])); i++)
    {
        if (GlobalSNOGet(cgarBossMonsterPlayerKilledFlags[i].globalSNOMonster) == snoMonster)
            return TRUE;
    }
    return FALSE;
}






































SNO LootMonsterGetTreasureClass(const ActorCommonData & tACDMonster,
                                const ACDID idACDLooter,
                                const MonsterRarity eRarity)

{
    SNOName tMonsterSNOName;

    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tACDMonster.tSNOName.snoHandle, tMonsterSNOName);
    if (!bHasMonsterSNO)
    {
        return SNO_NONE;
    }

    ScopedMonsterDefinitionLockR pdefMonster(tMonsterSNOName.snoHandle);
    SNO snoTreasureClass = SNO_NONE;

    if (pdefMonster->snoTreasureClassFirstKill != SNO_NONE &&
        GameIsStoryMode())
    {
        BOOL bFlagFound = FALSE;
        uint32 dwPlayerFlag = 0;

        for (int32 i = 0; i < (int32)(sizeof(cgarBossMonsterPlayerKilledFlags) / sizeof(cgarBossMonsterPlayerKilledFlags[0])); i++)
        {
            if (GlobalSNOGet(cgarBossMonsterPlayerKilledFlags[i].globalSNOMonster) == tMonsterSNOName.snoHandle)
            {
                dwPlayerFlag = cgarBossMonsterPlayerKilledFlags[i].dwFlag;
                bFlagFound = TRUE;
                break;
            }
        }






        if (ACDIsPlayer(idACDLooter) && bFlagFound)
        {
            Player * ptPlayer = PlayerGetByACD(idACDLooter);
            if (ptPlayer)
            {

                ptPlayer->dwBossesKilledFlags |= (1 << dwPlayerFlag);

                DifficultyLevel eDifficulty = GameGetDifficultyLevel();




                if (!(ptPlayer->tHeroStateData.uBossKillFlags[eDifficulty] & (1 << dwPlayerFlag)))
                {
                    ptPlayer->tHeroStateData.uBossKillFlags[eDifficulty] |= (1 << dwPlayerFlag);

                    ActorCommonData & tACDLooter = ACDGet(idACDLooter);
                    tACDLooter.MarkHeroStateChanged();

                    SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_Diablo, PLAYER_SAVE_MODE_END_OF_FRAME);
                    return pdefMonster->snoTreasureClassFirstKill;
                }
            }
        }
    }

    switch (eRarity)
    {
    case MR_RARE:



        snoTreasureClass = pdefMonster->snoTreasureClassRare;
        break;
    case MR_CHAMPION:
        {



            ACDPack * ptACDPack = ACDPackTryToGet(tACDMonster.idACDPack);
            if (ptACDPack)
            {
                int32 nACDsLeft = ptACDPack->GetTotalACDCount(FALSE);
                if (nACDsLeft > 0)
                {
                    snoTreasureClass = pdefMonster->snoTreasureClassChampionLight;
                    break;
                }
            }

            snoTreasureClass = pdefMonster->snoTreasureClassChampion;
        }
        break;
    default:



        snoTreasureClass = pdefMonster->snoTreasureClass;
        break;
    }



    return snoTreasureClass;
}



float LootGetFindValue(const ActorCommonData & tACDPlayer,
                       Attrib eAttrib)
{
    float flMagicFind = 0.0f;
    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_MAGIC_AND_GOLD_FIND_SUPPRESSED)) return flMagicFind;

    flMagicFind += tACDPlayer.AttributesGetFloat(eAttrib);


    flMagicFind += HirelingGetDiminishedAttribute(tACDPlayer.id, eAttrib);
    return flMagicFind;
}





































































// X/Common/Items.cpp:3913 (real home X/Common/Items.h, not adopted there yet)
int32 ItemGetBaseCost(const GBHandle & hGB, const CurrencyType eCurrencyType);
// X/Server/Loot/Loot.cpp:6516 (defined below)
void LootCheckForPlayerGift(const Player * ptPlayer, ItemQualityLevel eIQL);
static void sConvertTrashItemsToGold(
    LootSpecifierArray & arLootSpecifiersToSpawn,
    const ActorCommonData & tACDLootee)
{
    if (tACDLootee.eMonsterRarity == MR_CHAMPION || tACDLootee.eMonsterRarity == MR_RARE || tACDLootee.eMonsterRarity == MR_UNIQUE || tACDLootee.eMonsterRarity == MR_BOSS)  // mask 0x96 = rarities 1,2,4,7 (bit test over r<8)
        return;  // exit leaves helper: branch to the code after the inlined call (evidence: b.eq falls into B01)














    if(tACDLootee.IsChest())
    {
        ScopedActorDefinitionLockR pdefActor(tACDLootee.snoActor);
        SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);

        if (snoTreasureClass == GlobalSNOGet(GLOBALSNO_TREASURECLASS_RESPLENDENT_CHEST))
            return;


    }



    ScopedActorDefinitionLockR pdefActorLootee(tACDLootee.snoActor);
    if (TagMapGetInt(pdefActorLootee->hTagMap, TAG_ACTOR_SPAWN_DONT_CONVERT_TRASH_TO_GOLD))
        return;



    ScopedGlobalDefinitionLockR pdefGlobals;
    if (pdefGlobals->ptServerData)
    {

        const DifficultyTuningParams & tTuningParams = pdefGlobals->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(FALSE)];
        float flChance = tTuningParams.flTrashItemConvertChanceWhite;


        ScopedGlobalDefinitionLockR pdefGlobal;
        if (flChance == 0.0f) flChance = pdefGlobal->flGreyToGoldConversionChance;



        if (flChance > 0.000001f)
        {
            for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize(); ++i)
            {
                LootSpecifier & tSpecifier = arLootSpecifiersToSpawn[i];

                if (tSpecifier.IsTrashEligible())
                {
                    if (tSpecifier.eIQL >= IQL_MAGIC_AFFIX_START && tSpecifier.eIQL <= IQL_MAGIC_AFFIX_END)

                        flChance = tTuningParams.flTrashItemConvertChanceMagic;

                    else if (tSpecifier.eIQL >= IQL_RARE_AFFIX_START && tSpecifier.eIQL <= IQL_RARE_AFFIX_END)

                        flChance = tTuningParams.flTrashItemConvertChanceRare;


                    else if (tSpecifier.eIQL == IQL_INFERIOR)

                        flChance = pdefGlobal->flGreyToGoldConversionChance;



                    float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                    if (flRoll < flChance)
                    {

                        int32 nCost = ItemGetBaseCost(tSpecifier.hGBItem, CURRENCY_GOLD);
                        if (nCost > 0)
                        {
                            sSetupLootSpecifierForGold(tSpecifier, nCost);
                        }
                    }
                }
            }
        }
    }
}



static void sConvertPlansToGold(
    LootSpecifierArray & arLootSpecifiersToSpawn)
{
    for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize(); i++)
    {
        LootSpecifier & tSpecifier = arLootSpecifiersToSpawn[i];

        if (ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_CRAFTING_PLAN))
        {

            int32 nCost = ItemGetBaseCost(tSpecifier.hGBItem, CURRENCY_GOLD);
            if (nCost > 0)
            {
                sSetupLootSpecifierForGold(tSpecifier, nCost);
            }
        }
    }
}



static void sCoalesceGoldPiles(
    LootSpecifierArray & arLootSpecifiersToSpawn)
{

    if (PlayerGetNumPlayers() <= 1) return;







    LootSpecifier * ptGoldSpecifier = NULL;

    for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize();)
    {
        LootSpecifier & tSpecifier = arLootSpecifiersToSpawn[i];









        if (tSpecifier.nGoldAmount != 0)
        {
            if (ptGoldSpecifier == NULL)
            {
                ptGoldSpecifier = &tSpecifier;
            }
            else
            {

                ptGoldSpecifier->nGoldAmount += tSpecifier.nGoldAmount;
                arLootSpecifiersToSpawn.RemoveAt(i);
                continue;
            }
        }

        i++;
    }



    if (ptGoldSpecifier != NULL)
        ptGoldSpecifier->hGBItem.gbid = sGetGoldGBIDFromAmount(ptGoldSpecifier->nGoldAmount);
}





static void sScaleDownGoldPiles(
    LootSpecifierArray & arLootSpecifiersToSpawn)
{

    ScopedGlobalDefinitionLockR pdefGlobal;
    float flScale = pdefGlobal->flGoldReductionScalar;

    if (flScale >= 1.0f) return;




    for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize();)
    {
        LootSpecifier & tSpecifier = arLootSpecifiersToSpawn[i];

        if (tSpecifier.nGoldAmount != 0)
        {

            tSpecifier.nGoldAmount = (int32)((float)tSpecifier.nGoldAmount * flScale);
            if (tSpecifier.nGoldAmount == 0)
            {
                arLootSpecifiersToSpawn.RemoveAt(i);
                continue;
            }
            else
            {

                tSpecifier.hGBItem.gbid = sGetGoldGBIDFromAmount(tSpecifier.nGoldAmount);
            }
        }

        i++;
    }
}


static void sSpawnPlayerGifts(
    LootSpecifierArray & arLootSpecifiersToSpawn,
    ACDID idLooter)
{
    for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize(); ++i)
    {
        LootSpecifier & tSpecifier = arLootSpecifiersToSpawn[i];

        if (tSpecifier.nGoldAmount == 0 && tSpecifier.eIQL >= IQL_RARE_AFFIX_START)
        {
            Player * ptPlayer = PlayerGetByACD(idLooter);
            if (ptPlayer)
            {
                LootCheckForPlayerGift(ptPlayer, tSpecifier.eIQL);
            }
        }
    }
}

static void sValidateLocalCoopTargetedLoot(
    LootSpecifierArray & arLootSpecifiersToSpawn,
    ACDID idLooter)
{
    if (idLooter != ACDID_INVALID)
    {
        Player * ptPlayer = PlayerGetByACD(idLooter);
        if (ptPlayer && PlayerGetNumberPlayersSharingGameConnection(ptPlayer) > 1)
        {
            for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize(); ++i)
            {
                LootSpecifier & tSpecifier = arLootSpecifiersToSpawn[i];

                if (ItemIsEquippable(tSpecifier.hGBItem.gbid))
                {
                    PlayerClass ePlayerClass = PlayerGetPlayerClass(idLooter);
                    if (!ItemMeetsOwnerClassRestrictions(tSpecifier.hGBItem.gbid, ePlayerClass, FALSE))
                    {

                        int32 nCost = ItemGetBaseCost(tSpecifier.hGBItem, CURRENCY_GOLD);
                        if (nCost > 0)
                        {
                            sSetupLootSpecifierForGold(tSpecifier, nCost);
                        }
                    }
                }
            }
        }
    }
}


BOOL LootDropFriendGift(const Player * ptPlayer, const BOOL bAvenger, ItemQualityLevel eIQL)
{
    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACD)
        return FALSE;

    LootSpecifierArray arLootSpecifiersToSpawn;
    LootSpecifier & tSpecifier = arLootSpecifiersToSpawn.AppendElement_Tracked();

    tSpecifier.hGBItem.eType = GB_ITEMS;
    tSpecifier.hGBItem.gbid = StringToGBID(bAvenger ? "ConsoleAvengerGift" : "ConsoleFriendGift");
    tSpecifier.snoActorForFlippy = GameBalanceGetActorSNO(tSpecifier.hGBItem);
    if (tSpecifier.snoActorForFlippy == SNO_NONE)
        return FALSE;

    tSpecifier.eIQL = eIQL;
    tSpecifier.nAffixLevel = 1;
    tSpecifier.fAutoIdentify = TRUE;

    FlippiesCreateOnActor(*ptACD, ptPlayer->idPlayerACD, arLootSpecifiersToSpawn, 0, SGameGetEventNumber(), 0.0f, ptPlayer->idPlayerACD, NULL, NULL, LC_UNKNOWN, NULL);
    return TRUE;
}






















void LootCheckForPlayerGift(const Player * ptPlayer, ItemQualityLevel eIQL)
{
    if (SPlayerGetHasFriends(*ptPlayer) && !CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT) && !GameIsSeasonal())
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flPlayerGiftChancePercent = pdefGlobal->flFriendGiftDropChance;
        if (eIQL == IQL_LEGENDARY && RandFloat_Internal(SGameGlobalsGet().rng) < flPlayerGiftChancePercent)
        {
            LootDropFriendGift(ptPlayer, FALSE, IQL_LEGENDARY);
        }
    }
}






// X/Common/ActorInventory.cpp:10712 (not declared in src's ActorInventory.h yet)
PickupResponse ACDInventoryCanPickup(const ActorCommonData & tACDPickee, const ActorCommonData & tACDItem, BOOL bTryStashFirst, PickupResult * ptResult, BOOL bTryStashOnly, BOOL bAllowAutoEquip);



// NON_MATCHING: The signed level cutoff uses cmp #11/b.ge where the target uses cmp #10/b.gt.
void LootGenerateGift(
    const Player & tPlayer,
    ItemQualityLevel eIQL)
{


    SNO snoTreasureClass = GlobalSNOGet(GLOBALSNO_TREASURECLASS_PLAYER_GIFT_LEGENDARY);

    ACDID idACDPlayer = tPlayer.idPlayerACD;
    if (idACDPlayer == ACDID_INVALID)
        return;



    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDPlayer, ACDID_INVALID);
    tLootee.snoTreasureClass = snoTreasureClass;
    tLootee.tLooteeParams.bSuppressGiftGeneration = TRUE;
    // 11 is written as a literal: column 55 of the original condition suggests a longer spelling (a named constant) not found in src
    if (tLootee.tLooteeParams.nLevel < 11)
    {
        tLootee.tLooteeParams.nLevel = 11;
    }

    LootSpecifierArray arLootSpecifiersToSpawn;

    BOOL bGotSpecifier = LootGetLootSpecifiers(tLootee, idACDPlayer, arLootSpecifiersToSpawn, NULL, FALSE, 1, NULL);

    if (bGotSpecifier && arLootSpecifiersToSpawn.GetArraySize() != 0)
    {
        LootSpecifier & tGiftSpecifier = arLootSpecifiersToSpawn[0];

        ACDID idACDItem = LootCreateItem(tGiftSpecifier, cgplaceNull, NULL);
        if (idACDItem != ACDID_INVALID)
        {
            ActorCommonData & tACDItem = ACDGet(idACDItem);

            if (ACDInventoryCanPickup(tACDPlayer, tACDItem, FALSE, NULL, FALSE, TRUE) == PICKUP_RESPONSE_OK)
            {
                SACDInventoryPickupOrSpillOntoGround(tACDPlayer, tACDItem, FALSE, NULL, TRUE);

                D3::Items::Generator tItemGenerator;
                ItemGeneratorToDefaults(tItemGenerator);
                ItemGeneratorPopulateFromACDID(tACDItem, tItemGenerator, TRUE);

                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetSingle(listDestPlayers, idACDPlayer);
                MessageSendToClient(listDestPlayers, SMSG_PLAYER_GIFT_ITEM_GENERATED, tItemGenerator);
            }
            else
            {
                ActorDelete((ActorID)ACDGetOwner(idACDItem));

                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetSingle(listDestPlayers, idACDPlayer);
                SimpleMessage tMessage;
                MessageSendToClient(listDestPlayers, SMSG_PLAYER_GIFT_ITEM_INVENTORY_FULL, &tMessage, sizeof(tMessage));

            }
        }
    }

}



// NON_MATCHING: 8 of 414 insns: in the sSetupLootSpecifierForGold copy inlined at line 6298 (sConvertTrashItemsToGold) the GBHandle address / nCost registers are swapped (x24/x25) and the nGoldAmount store is scheduled after the snoActorForFlippy store; the other inlined copies match. Tried flRoll compare order; helper on DWARF lines 3642-3651 (no change). Register choice only.
void LootModifyBeforeSpawning(
    LootSpecifierArray & arLootSpecifiersToSpawn,
    const ActorCommonData & tACDLootee,
    ACDID idLooter)
{

    sConvertTrashItemsToGold(arLootSpecifiersToSpawn, tACDLootee);
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        sConvertPlansToGold(arLootSpecifiersToSpawn);
    }

    sCoalesceGoldPiles(arLootSpecifiersToSpawn);


    sValidateLocalCoopTargetedLoot(arLootSpecifiersToSpawn, idLooter);
    sScaleDownGoldPiles(arLootSpecifiersToSpawn);
    if (!GameIsSeasonal())
    {
        sSpawnPlayerGifts(arLootSpecifiersToSpawn, idLooter);
    }

}



























static void sRemoveConsumablePotions(LootSpecifierArray & arLootSpecifiersToSpawn)
{

    for (int32 i = 0; i < arLootSpecifiersToSpawn.GetArraySize(); i++)
    {
        LootSpecifier & tSpecifier = arLootSpecifiersToSpawn[i];

        if (ItemIsConsumablePotion(tSpecifier.hGBItem.gbid))
        {
            arLootSpecifiersToSpawn.RemoveAt(i);
            i--;
        }
    }
}




int32 sGetLegendaryItemDropWeightIfValidToDrop(const Item & tItem, ActorCommonData * ptACDLooter, const SharedLooteeParams & tLooteeParams, PlayerClass ePlayerClass, ItemLabel eItemLabel1, ItemLabel eItemLabel2, int32 nLooteeLevelToUse, BOOL bHardcore, BOOL bX1Enabled, Act eCurrentAct, BOOL bIgnoreClassLegendaryDropWeight)
{
    if (tLooteeParams.fIsVendor && sub_710050e910(tItem.gbid))
        return 0;









    BOOL bUseForcedILevel = tLooteeParams.nForcedILevel > 0;
    if (bUseForcedILevel && tItem.nItemLevel != tLooteeParams.nForcedILevel)
        return 0;



    int32 nHandicapLevel = GameGetHandicapLevel();
    if (sPickItem_PreliminaryFilterOut(ptACDLooter ? ptACDLooter->id : ACDID_INVALID, tItem, TRUE, tLooteeParams.bDisableTormentCheck, tLooteeParams.bDisableTormentDrops, eCurrentAct, nHandicapLevel, bX1Enabled, FALSE))
        return 0;




    if ((tItem.dwFlags & (1 << ITEM_HARDCORE_ONLY_BIT)) && !bHardcore)
        return 0;






    if (eItemLabel1 != ITL_INVALID &&
        !ItemHasLabel(tItem.gbid, eItemLabel1))
        return 0;



    if (eItemLabel2 != ITL_INVALID &&
        !ItemHasLabel(tItem.gbid, eItemLabel2))
        return 0;




    const GameBalanceDefinition * pdefGBItems;
    GBHandle hGB(GB_ITEMS, tItem.gbid);
    ItemDefinition * pdefItem = (ItemDefinition *)GBLockR(hGB, pdefGBItems, TRUE);
    if (pdefItem == NULL)
        return 0;



    int32 nDropWeight = pdefItem->nLegendaryDropWeight;
    if (ePlayerClass >= 0 && ePlayerClass < 7 && !bIgnoreClassLegendaryDropWeight)
    {
        nDropWeight *= pdefItem->nClassLegendaryDropWeight[ePlayerClass];
    }
    if (nDropWeight <= 0)
    {
        GBUnlockR(pdefGBItems);
        return 0;
    }

    int32 nItemLevel = pdefItem->nItemLevel;

    GBUnlockR(pdefGBItems);



    return (nItemLevel > nLooteeLevelToUse) ? 0 : nDropWeight;



}








































































static GBID sPickLegendary(ActorCommonData * ptACDLooter,
                           const SharedLooteeParams & tLooteeParams,
                           PlayerClass ePlayerClass,
                           ItemLabel eItemLabel1,
                           ItemLabel eItemLabel2,
                           BOOL bIgnoreClassLegendaryDropWeight)
{
    Act eCurrentAct = GameGetCurrentAct();

    int32 nLooteeLevelToUse = Math::Max(1, tLooteeParams.nLevel);

    if (nLooteeLevelToUse < 1 || nLooteeLevelToUse > 110)  // 110: no enumerator found
    {
        return GBID_NONE;
    }



    BOOL bHardcore = FALSE;

    GameCommonData & tGCD = SGameGetGameCommonData();
    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;

    int32 nMemberItems = ptItemAppGlobals->nItems;
    int32 nNumWeights = 0;
    int32 nMaxWeight = 0;
    BOOL bX1Enabled = (OnlineService::GetServerContentFlags() & (1 << 1)) != 0;
    BOOL bForceSeasonalLegendary = FALSE;  // DWARF local with no surviving use



























    uint32 nCurrentSeason = GameGetCurrentSeason();
    for (int32 nItemIdx = 0; nItemIdx < nMemberItems; nItemIdx++)
    {
        const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];

        if (tLooteeParams.unk_50 && !ItemIsEquippableByPlayerOrHireling(tItem.gbid))
            continue;



        int32 nDropWeight = sGetLegendaryItemDropWeightIfValidToDrop(tItem, ptACDLooter, tLooteeParams, ePlayerClass, eItemLabel1, eItemLabel2, nLooteeLevelToUse, bHardcore, bX1Enabled, eCurrentAct, bIgnoreClassLegendaryDropWeight);
        if (nNumWeights < ITEM_WEIGHT_ARRAY_MAX_SIZE && nDropWeight > 0)
        {



            tGCD.arItemWeights[nNumWeights].nItemWeight = nDropWeight;
            tGCD.arItemWeights[nNumWeights].nItemIndex = nItemIdx;
            nMaxWeight += tGCD.arItemWeights[nNumWeights].nItemWeight;
            nNumWeights++;
        }
    }


    if (nNumWeights < 1)
    {
        return GBID_NONE;
    }

    int32 nWeightIdx = sChooseFromItemWeightArray(nNumWeights, nMaxWeight);


    int32 nItemIdx = tGCD.arItemWeights[nWeightIdx].nItemIndex;


    const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];


    if (ptACDLooter && nCurrentSeason != 0 && tItem.nSeasonRequiredToDrop == nCurrentSeason)
    {
        ptACDLooter->AttributesSetInt(ATTRIBUTE_RECEIVED_SEASONAL_LEGENDARY, TRUE);
    }


    return tItem.gbid;
}



static void sRollForTargetedItem(
    const Player * ptPlayer,
                                 LootSpecifier & tSpecifier,
                                 BOOL bTargetRandomClass)
{
    BOOL bTargeted = tSpecifier.eIQL > IQL_MAGIC_1_AFFIX || ItemIsLegendary(tSpecifier.hGBItem.gbid);
    BOOL bHirelingTargeted = ItemHasLabel(tSpecifier.hGBItem.gbid, ITL_FOLLOWER_SPECIAL);
    PlayerClass eTargetedPlayerClass = PLAYER_CLASS_NONE;
    ScopedItemDefinitionLockR pdefItem(tSpecifier.hGBItem.gbid);
    ScopedGlobalDefinitionLockR pdefGlobal;

    if (ptPlayer)
    {
        PlayerClass eClass = PlayerGetPlayerClass(ptPlayer);
        ActorCommonData * ptACDLooter = ACDTryToGet(ptPlayer->idPlayerACD);

        BOOL bOverrideDropChance = PlayerGetNumberPlayersSharingGameConnection(ptPlayer) > 1;




        if (ItemIsLegendary(tSpecifier.hGBItem.gbid))
        {
            if (!bTargetRandomClass && ptACDLooter

                && ItemMeetsOwnerClassRestrictions(tSpecifier.hGBItem.gbid, eClass, FALSE)
                && ItemIsEquippable(tSpecifier.hGBItem.gbid)
                && pdefItem->nClassLegendaryDropWeight[PlayerGetPlayerClass(ptPlayer)] > 0)
            {

                int32 nTargetedLegendaryChance = ptACDLooter->AttributesGetInt(ATTRIBUTE_TARGETED_LEGENDARY_CHANCE);
                float flTargetedLegendaryChancePercent = nTargetedLegendaryChance / 100.0f;
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                if (flRoll < flTargetedLegendaryChancePercent || bOverrideDropChance)  // inferred order: the `||` sits at col 63 (after the roll compare); same for 6991/7020
                {

                    eTargetedPlayerClass = eClass;

                    ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_LEGENDARY_CHANCE, pdefGlobal->ptServerData->nTargetedLegendaryDropChanceBase);
                }
                else
                {
                    int32 nTargetedLegendaryChanceIncrement = RandRange_Internal(SGameGlobalsGet().rng, pdefGlobal->ptServerData->nTargetedLegendaryDropChanceIncrement, pdefGlobal->ptServerData->nTargetedLegendaryDropChanceIncrementDelta);



                    ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_LEGENDARY_CHANCE, nTargetedLegendaryChance + nTargetedLegendaryChanceIncrement);
                }
            }
        }
        else if (tSpecifier.eIQL >= IQL_RARE_4_AFFIX && tSpecifier.eIQL <= IQL_RARE_6_AFFIX)
        {
            if (!bTargetRandomClass && ptACDLooter

                && ItemMeetsOwnerClassRestrictions(tSpecifier.hGBItem.gbid, eClass, FALSE)
                && (ItemIsEquippable(tSpecifier.hGBItem.gbid)
                    || ItemIsShard(tSpecifier.hGBItem.gbid)))
            {
                int32 nTargetedRareChance = ptACDLooter->AttributesGetInt(ATTRIBUTE_TARGETED_RARE_CHANCE);
                float flTargetedRareChancePercent = nTargetedRareChance / 100.0f;
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                if (flRoll < flTargetedRareChancePercent || bOverrideDropChance)
                {

                    eTargetedPlayerClass = eClass;

                    ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_RARE_CHANCE, pdefGlobal->ptServerData->nTargetedRareDropChanceBase);
                }
                else
                {
                    int32 nTargetedRareChanceIncrement = RandRange_Internal(SGameGlobalsGet().rng, pdefGlobal->ptServerData->nTargetedRareDropChanceIncrement, pdefGlobal->ptServerData->nTargetedRareDropChanceIncrementDelta);



                    ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_RARE_CHANCE, nTargetedRareChance + nTargetedRareChanceIncrement);
                }
            }
        }

        else if (tSpecifier.eIQL >= IQL_MAGIC_1_AFFIX && tSpecifier.eIQL <= IQL_MAGIC_3_AFFIX)
        {
            if (!bTargetRandomClass && ptACDLooter

                && ItemMeetsOwnerClassRestrictions(tSpecifier.hGBItem.gbid, eClass, FALSE)
                && (ItemIsEquippable(tSpecifier.hGBItem.gbid)
                    || ItemIsShard(tSpecifier.hGBItem.gbid)))
            {
                int32 nTargetedMagicChance = ptACDLooter->AttributesGetInt(ATTRIBUTE_TARGETED_MAGIC_CHANCE);
                float flTargetedMagicChancePercent = nTargetedMagicChance / 100.0f;
                float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
                if (flRoll < flTargetedMagicChancePercent || bOverrideDropChance)
                {

                    eTargetedPlayerClass = eClass;

                    ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_MAGIC_CHANCE, pdefGlobal->ptServerData->nTargetedMagicDropChanceBase);
                }
                else
                {
                    int32 nTargetedMagicChanceIncrement = RandRange_Internal(SGameGlobalsGet().rng, pdefGlobal->ptServerData->nTargetedMagicDropChanceIncrement, pdefGlobal->ptServerData->nTargetedMagicDropChanceIncrementDelta);



                    ptACDLooter->AttributesSetInt(ATTRIBUTE_TARGETED_MAGIC_CHANCE, nTargetedMagicChance + nTargetedMagicChanceIncrement);
                }
            }
        }


        if (!bHirelingTargeted && eTargetedPlayerClass == PLAYER_CLASS_NONE

            && ptPlayer->tHirelingStable.eActiveHireling != 0
            && !ItemIsLegendary(tSpecifier.hGBItem.gbid)

            && RandFloat_Internal(SGameGlobalsGet().rng) < pdefGlobal->ptServerData->flHirelingTargetedItemChance)
        {
            for (int32 i = 1; i < 4; i++)
            {
                if (ItemUsableByHireling(tSpecifier.hGBItem.gbid, (HirelingClass)i))
                {
                    bHirelingTargeted = TRUE;
                    break;
                }
            }
        }
    }



    if (bHirelingTargeted)
    {
        if (tSpecifier.eTargetedHirelingClass == 0)
        {

            HirelingClass eCurrentHireling = 0;
            if (ptPlayer)
            {
                ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
                ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
                if (ptACDHireling)
                {
                    eCurrentHireling = HirelingGetClass(*ptACDHireling);
                }
            }


            float flTotalWeight = 0.0f;
            float flEligibleHirelingWeights[4];
            SigmaMemoryFill(flEligibleHirelingWeights, 0, sizeof(flEligibleHirelingWeights));

            for (int32 i = 1; i < 4; i++)
            {
                if (pdefGlobal->ptServerData->flHirelingTargetedInactiveChance < EPSILON && i != eCurrentHireling)  // inferred: member load col 47, `&&` col 90 (EPSILON fits), b.mi skip; ItemUsableByHireling at col 21 (not nested)
                {
                    continue;
                }

                if (ItemUsableByHireling(tSpecifier.hGBItem.gbid, (HirelingClass)i))
                {
                    flEligibleHirelingWeights[i] = 1.0f;
                    flTotalWeight += flEligibleHirelingWeights[i];
                }

            }

            if (eCurrentHireling != 0 && flEligibleHirelingWeights[eCurrentHireling] > 0.0f)
            {
                float flTotalWeightExcludingCurrent = flTotalWeight - flEligibleHirelingWeights[eCurrentHireling];
                if (flTotalWeightExcludingCurrent > 0.000001f)
                {
                    float flModifiedWeight = flTotalWeightExcludingCurrent / pdefGlobal->ptServerData->flHirelingTargetedInactiveChance - flTotalWeightExcludingCurrent;


                    flTotalWeight += flModifiedWeight - flEligibleHirelingWeights[eCurrentHireling];
                    flEligibleHirelingWeights[eCurrentHireling] = flModifiedWeight;
                }
            }

            if (flTotalWeight < EPSILON)  // inferred early return: b.mi skip, flRandWeight (7114 col 34) is at this depth and in the same DWARF block
            {
                return;
            }


            float flRandWeight = RandFloat_Internal(SGameGlobalsGet().rng) * flTotalWeight;
            for (int32 i = 1; i < 4; i++)
            {
                if (flEligibleHirelingWeights[i] < EPSILON)  // inferred continue: b.mi skip and 7122/7128 columns sit one level shallower
                {
                    continue;
                }

                tSpecifier.eTargetedHirelingClass = (HirelingClass)i;
                if (flRandWeight <= flEligibleHirelingWeights[i])
                {
                    break;
                }

                flRandWeight -= flEligibleHirelingWeights[i];
            }
        }
    }
    else if (bTargeted && tSpecifier.eTargetedPlayerClass == PLAYER_CLASS_NONE)
    {
        tSpecifier.FillInTargetedClass(eTargetedPlayerClass, NULL, TRUE);
    }
}

BOOL GameCommonGameFlagIsSet(LPCSTR szFlagLabel); namespace NephalemRiftManager { int32 RiftPlayerCount(); }
// NON_MATCHING: 2.7.7 logic is complete (sub_710050e910 always-ancient check, seasonal solo tiered-rift guaranteed primal with ATTRIBUTE_TIERED_RIFT_PRIMAL_RECEIVED); the remaining diff is block layout only: the original puts the "LootRunTieredRiftFailed not set" path (looter / rift-level / attribute checks) out of line at the end of the function. Tried the flag in a local.
void LootRollForAncientLegendary(LootSpecifier & tSpecifier, const ACDID idACDLooter)
{
    if (tSpecifier.tLooteeParams.bDisableAncientDrops)
    {
        return;
    }

    BOOL bIsValidLevel = tSpecifier.nLegendaryItemLevel >= 70 ||
        (tSpecifier.tLooteeParams.fIsCrafter && tSpecifier.tLooteeParams.nLevel >= 70);
    if (!bIsValidLevel)
    {
        return;
    }

    if (!ItemIsLegendary(tSpecifier.hGBItem.gbid))
    {
        return;
    }

    if (sub_710050e910(tSpecifier.hGBItem.gbid))
    {
        tSpecifier.eAncientRank = ANCIENT_RANK_ANCIENT;
        return;
    }

    if (!ItemIsEquippableByPlayerOrHireling(tSpecifier.hGBItem.gbid))
    {
        return;
    }

    float flAncientChance = 0.0f;
    if (tSpecifier.tLooteeParams.bForceCrafterAncientLegendaryChance)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        flAncientChance = pdefGlobal->ptServerData->flCrafterAncientLegendaryChance;
    }
    else if (tSpecifier.tLooteeParams.nTieredLootRunLevel >= 0)
    {
        ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
        int32 nGameBalanceIndex = Math::Max(tSpecifier.tLooteeParams.nTieredLootRunLevel, 0);
        nGameBalanceIndex = Math::Min(nGameBalanceIndex, (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)) - 1);
        if (nGameBalanceIndex >= 0 && nGameBalanceIndex < (int32)pdefGB->tTieredLootRunLevelTable.serTieredLootRunLevels.ElementCount(sizeof(TieredLootRunLevel)))
        {
            flAncientChance = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nGameBalanceIndex].tLootRunValues.flAncientLegendaryChance;
        }
    }
    else
    {
        int32 nHandicap = GameGetHandicapLevel();
        if (tSpecifier.tLooteeParams.nAncientChanceHandicapLevelOverride >= 0)
        {
            nHandicap = tSpecifier.tLooteeParams.nAncientChanceHandicapLevelOverride;
        }

        if (nHandicap >= 4)
        {
            ScopedGlobalDefinitionLockR pdefGlobal;
            flAncientChance = pdefGlobal->ptServerData->arAncientLegendaryChance[nHandicap - 4];
        }
    }

    BOOL bAncient = RandFloat_Internal(SGameGetRNG()) <= flAncientChance;

    if (GameIsSeasonal() && tSpecifier.tLooteeParams.nTieredLootRunLevel > 0 && NephalemRiftManager::RiftPlayerCount() == 1)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        int32 nMinimumAchievedRiftLevelToRollPrimalAncients = pdefGlobal->ptServerData->nMinimumAchievedRiftLevelToRollPrimalAncients;
        if (!GameCommonGameFlagIsSet("LootRunTieredRiftFailed") &&
            idACDLooter != ACDID_INVALID &&
            tSpecifier.tLooteeParams.nTieredLootRunLevel + 1 >= nMinimumAchievedRiftLevelToRollPrimalAncients)
        {
            ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
            if (ptACDLooter && !ptACDLooter->AttributesGetInt(ATTRIBUTE_TIERED_RIFT_PRIMAL_RECEIVED))
            {
                tSpecifier.eAncientRank = ANCIENT_RANK_PRIMAL_ANCIENT;
                tSpecifier.fAccountBound = TRUE;
                ptACDLooter->AttributesSetInt(ATTRIBUTE_TIERED_RIFT_PRIMAL_RECEIVED, TRUE);
            }
        }
    }

    if (bAncient && tSpecifier.eAncientRank != ANCIENT_RANK_PRIMAL_ANCIENT)
    {
        tSpecifier.eAncientRank = ANCIENT_RANK_ANCIENT;
        if (idACDLooter != ACDID_INVALID && !tSpecifier.tLooteeParams.bDisablePrimalAncientDrops)
        {
            ActorCommonData * ptACDLooter = ACDTryToGet(idACDLooter);
            if (ptACDLooter)
            {
                ScopedGlobalDefinitionLockR pdefGlobal;
                int32 nHighestSoloRiftLevel = ptACDLooter->AttributesGetInt(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL);
                int32 nMinimumAchievedRiftLevelToRollPrimalAncients = pdefGlobal->ptServerData->nMinimumAchievedRiftLevelToRollPrimalAncients;
                if (nHighestSoloRiftLevel >= nMinimumAchievedRiftLevelToRollPrimalAncients)
                {
                    flAncientChance = pdefGlobal->ptServerData->flPrimalAncientLegendaryChance;
                    if (RandFloat_Internal(SGameGetRNG()) <= flAncientChance)
                    {
                        tSpecifier.eAncientRank = ANCIENT_RANK_PRIMAL_ANCIENT;
                    }
                }
            }
        }
    }
}




GlobalGBID arUpgradeRareGBIDItemTypes[8] =
{
    GLOBALGBID_ITEMTYPE_HELM,
    GLOBALGBID_ITEMTYPE_BELT,
    GLOBALGBID_ITEMTYPE_GLOVES,
    GLOBALGBID_ITEMTYPE_BOOTS,
    GLOBALGBID_ITEMTYPE_SHOULDERS,
    GLOBALGBID_ITEMTYPE_CHESTARMOR,
    GLOBALGBID_ITEMTYPE_LEGS,
    GLOBALGBID_ITEMTYPE_BRACERS,
};



GBID LootPickLegendaryOfItemType(const Player & tPlayer,
                                 GBID gbidItemType,
                                 int32 nItemLevel)

{

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return GBID_NONE;
    }


    Act eCurrentAct = GameGetCurrentAct();
    int32 nHandicapLevel = GameGetHandicapLevel();
    BOOL bX1Enabled = (OnlineService::GetServerContentFlags() & (1 << 1)) != 0;


    PlayerClass ePlayerClass = PlayerGetPlayerClass(&tPlayer);
    BOOL bHardcore = tPlayer.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE);


    if (!ItemTypeMeetsOwnerClassRestrictions(gbidItemType, ePlayerClass))
    {
        ePlayerClass = PLAYER_CLASS_NONE;
    }



    GBID gbidItemTypeSecondary = GBID_NONE;
    for (int32 i = 0; i < (int32)(sizeof(arUpgradeRareGBIDItemTypes) / sizeof(arUpgradeRareGBIDItemTypes[0])); i++)
    {
        if (GlobalGBIDGet(arUpgradeRareGBIDItemTypes[i]) == gbidItemType)
        {

            CRefString sItemType = GBIDToString(gbidItemType);
            switch (ePlayerClass)
            {
            case PLAYER_CLASS_DEMONHUNTER:
                sItemType += "_DemonHunter";
                break;
            case PLAYER_CLASS_BARBARIAN:
                sItemType += "_Barbarian";
                break;
            case PLAYER_CLASS_WIZARD:
                sItemType += "_Wizard";
                break;
            case PLAYER_CLASS_WITCHDOCTOR:
                sItemType += "_WitchDoctor";
                break;
            case PLAYER_CLASS_MONK:
                sItemType += "_Monk";
                break;
            case PLAYER_CLASS_CRUSADER:
                sItemType += "_Crusader";
                break;
            case PLAYER_CLASS_NECROMANCER:
                sItemType += "_Necromancer";
                break;
            }
            gbidItemTypeSecondary = StringToGBID(sItemType);
            break;
        }
    }


    if (nItemLevel < 0)
    {
        nItemLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    }


    GameCommonData & tGCD = SGameGetGameCommonData();
    int32 nNumWeights = 0;
    int32 nMaxWeight = 0;

    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;



    int32 nMemberItems = ptItemAppGlobals->nItems;
    for (int32 nItemIdx = 0; nItemIdx <= nMemberItems; ++nItemIdx)
    {


        if (nItemIdx == nMemberItems)
        {
            if (nNumWeights < 1 && ePlayerClass != PLAYER_CLASS_NONE)
            {
                nItemIdx = -1; ePlayerClass = PLAYER_CLASS_NONE;
            }
            continue;
        }


        const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];

        if (tItem.ptItemType->gbid != gbidItemType && ((gbidItemTypeSecondary == GBID_NONE) |  // bitwise OR: the original evaluates both operands without a branch
            (tItem.ptItemType->gbid != gbidItemTypeSecondary)))
        {
            continue;
        }

        if (sPickItem_PreliminaryFilterOut(ptACDPlayer->id, tItem, TRUE, TRUE, FALSE, eCurrentAct, nHandicapLevel, bX1Enabled, FALSE))
        {
            continue;
        }

        if ((tItem.dwFlags & (1 << ITEM_HARDCORE_ONLY_BIT)) && !bHardcore)
        {
            continue;
        }

        ScopedItemDefinitionLockR pdefItem(tItem.gbid);
        if (pdefItem->nItemLevel > nItemLevel)
        {
            continue;
        }

        int32 nDropWeight = pdefItem->nLegendaryDropWeight;
        if (ePlayerClass != PLAYER_CLASS_NONE)
        {
            nDropWeight *= pdefItem->nClassLegendaryDropWeight[ePlayerClass];
        }

        if (nNumWeights < ITEM_WEIGHT_ARRAY_MAX_SIZE && nDropWeight > 0)
        {






            tGCD.arItemWeights[nNumWeights].nItemWeight = nDropWeight;
            tGCD.arItemWeights[nNumWeights].nItemIndex = nItemIdx;
            nMaxWeight += nDropWeight;
            nNumWeights++;
        }
    }


    if (nNumWeights < 1)
    {
        return GBID_NONE;
    }


    int32 nWeightIdx = sChooseFromItemWeightArray(nNumWeights, nMaxWeight);


    int32 nItemIdx = tGCD.arItemWeights[nWeightIdx].nItemIndex;


    const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];


    uint32 nCurrentSeason = GameGetCurrentSeason();
    if (nCurrentSeason != 0 && tItem.nSeasonRequiredToDrop == nCurrentSeason)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_RECEIVED_SEASONAL_LEGENDARY, TRUE);
    }

    return tItem.gbid;
}


GBID LootPickItemFromSet(const Player & tPlayer,
                         GBID gbidSet,
                         GBID gbidItemSkip,
                         int32 nItemLevel)

{

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return GBID_NONE;
    }


    Act eCurrentAct = GameGetCurrentAct();
    int32 nHandicapLevel = GameGetHandicapLevel();
    BOOL bX1Enabled = (OnlineService::GetServerContentFlags() & (1 << 1)) != 0;


    BOOL bHardcore = tPlayer.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE);


    if (nItemLevel < 0)
    {
        nItemLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    }


    GameCommonData & tGCD = SGameGetGameCommonData();
    int32 nNumWeights = 0;
    int32 nMaxWeight = 0;

    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;



    int32 nMemberItems = ptItemAppGlobals->nItems;
    for (int32 nItemIdx = 0; nItemIdx < nMemberItems; ++nItemIdx)
    {
        const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];

        if (ItemGetSet(tItem.gbid) != gbidSet)
        {
            continue;
        }

        if (tItem.gbid == gbidItemSkip)
        {
            continue;
        }

        if (sPickItem_PreliminaryFilterOut(ptACDPlayer->id, tItem, TRUE, TRUE, FALSE, eCurrentAct, nHandicapLevel, bX1Enabled, TRUE))
        {
            continue;
        }

        if ((tItem.dwFlags & (1 << ITEM_HARDCORE_ONLY_BIT)) && !bHardcore)
        {
            continue;
        }

        ScopedItemDefinitionLockR pdefItem(tItem.gbid);
        if (pdefItem->nItemLevel > nItemLevel)
        {
            continue;
        }

        int32 nDropWeight = pdefItem->nLegendaryDropWeight;

        if (nNumWeights < ITEM_WEIGHT_ARRAY_MAX_SIZE && nDropWeight > 0)
        {






            tGCD.arItemWeights[nNumWeights].nItemWeight = nDropWeight;
            tGCD.arItemWeights[nNumWeights].nItemIndex = nItemIdx;
            nMaxWeight += nDropWeight;
            nNumWeights++;
        }
    }


    if (nNumWeights < 1)
    {
        return GBID_NONE;
    }


    int32 nWeightIdx = sChooseFromItemWeightArray(nNumWeights, nMaxWeight);


    int32 nItemIdx = tGCD.arItemWeights[nWeightIdx].nItemIndex;


    const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];


    uint32 nCurrentSeason = GameGetCurrentSeason();
    if (nCurrentSeason != 0 && tItem.nSeasonRequiredToDrop == nCurrentSeason)
    {
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_RECEIVED_SEASONAL_LEGENDARY, TRUE);
    }

    return tItem.gbid;
}

GBID LootPickGem(int32 nGemQuality, GemType eGemType)
{

    const ItemAppGlobals * ptItemAppGlobals = sgtAppGlobals.ptItemAppGlobals;
    int32 nMemberItems = ptItemAppGlobals->nItems;



    for (int32 nItemIdx = 0;
         nItemIdx < nMemberItems; ++nItemIdx)
    {
        const Item & tItem = ptItemAppGlobals->ptItems[nItemIdx];

        ScopedItemDefinitionLockR pdefItem(tItem.gbid);
        if (pdefItem->eGemType == eGemType && ItemCrafting_GetGemQuality(tItem.gbid) == nGemQuality)
        {
            CRefString sActorName = SNOToString(SNOGroup(1), pdefItem->snoActor, FALSE);

            if (Blizzard::String::Find(sActorName.str(), "x1_") != NULL)
            {
                return tItem.gbid;
            }
        }
    }

    return GBID_NONE;
}
