#include "ConsoleLicenses.h"
#include "../Player.h"
#include "../ActorCommonData.h"
#include "../ActorInventory.h"
#include "../Game.h"
#include "../Items.h"
#include "../CosmeticItems.h"
#include "../VisualInventory.h"
#include "../Crafting/ItemCrafting.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../Definitions/GlobalDefinition.h"
#include "../Definitions/TreasureClassDefinition.h"
#include "../SNO/SNOFiles.h"
#include "../Shell/CmdLine.h"
#include "../../Server/Loot/Loot.h"
#include "../../Client/Console/GamerProfile.h"
#include "../Protobuf/Account.pb.h"
// Missing declarations recovered from original function signatures.
BOOL ItemIsDye(const ActorCommonData& tACDItem);
void SPlayerKillActiveCosmeticPet(Player& tPlayer);
namespace Console {
namespace DLCManager {
uint32 GetLicenses();
}
}
#line 25 "X/Common/Console/ConsoleLicenses.cpp"
enum LicenseRequirement {
    LicenseAutomatic,
    LicenseRequired,
    LicenseRevokable,
};
#line 39 "X/Common/Console/ConsoleLicenses.cpp"
enum RewardDestination {
    RewardToInventory,
    RewardToStash,
};
#line 45 "X/Common/Console/ConsoleLicenses.cpp"
enum RewardSeasonal {
    AllowSeasonal,
    ExcludeSeasonal,
};
#line 51 "X/Common/Console/ConsoleLicenses.cpp"
struct ConsoleBonusItemsFromLicense {
    int32 nLicense;
    LicenseRequirement eLicenseRequirement;
    const char* szTreasureClass;
    RewardDestination eRewardDestination;
    RewardSeasonal eRewardSeasonal;
};
// Original table at0x1047de0, including all seven pointer targets and enumeration values.
#line 72 "X/Common/Console/ConsoleLicenses.cpp"
static ConsoleBonusItemsFromLicense s_arConsoleLicenseItems[] = {
    {LICENSE_LEGACY_PREORDER_BONUS, LicenseRequired, "Console_PS3PreorderItems", RewardToInventory, AllowSeasonal},
    {LICENSE_PREORDER_BONUS_ROS, LicenseRequired, "Console_PreorderItems", RewardToInventory, ExcludeSeasonal},
    {LICENSE_SWITCH_BONUS, LicenseAutomatic, "Console_SwitchBonusItems", RewardToStash, AllowSeasonal},
    {LICENSE_LINKED_BNET_ACCOUNT, LicenseRequired, "Console_LinkedBnetAccountItems", RewardToStash, ExcludeSeasonal},
    {LICENSE_LINKED_BNET_ACCOUNT_SWITCH, LicenseRequired, "Console_SwitchPromoAmulet", RewardToStash, ExcludeSeasonal},
    {LICENSE_OVERWATCH_CE, LicenseRequired, "Console_OverwatchCEItems", RewardToInventory, AllowSeasonal},
    {LICENSE_NECROMANCER, LicenseRevokable, "Console_NecromancerItems", RewardToInventory, AllowSeasonal},
};
#line 108 "X/Common/Console/ConsoleLicenses.cpp"
static void sRemoveUnlicensedTransmog(
    CrafterTransmogData& tTransmogData,
    GBID gbidUnlicensedTransmog)
{
    if (gbidUnlicensedTransmog == GBID_NONE)
    {
        return;
    }

    int32& nUnlockedTransmogs = tTransmogData.nUnlockedTransmogs;
    for (int32 i = 0; i < nUnlockedTransmogs; ++i)
    {
        if (ItemCrafting_ItemTransmogsMatch(tTransmogData.gbidUnlockedTransmogs[i], gbidUnlicensedTransmog))
        {
            tTransmogData.gbidUnlockedTransmogs[i] = tTransmogData.gbidUnlockedTransmogs[nUnlockedTransmogs - 1];
            --nUnlockedTransmogs;
            break;
        }
    }
}
#line 131 "X/Common/Console/ConsoleLicenses.cpp"
static BOOL sModifyItemIfUnlicensed(ActorCommonData* ptACDItem, GBID gbidUnlicensedItem)
{
    if (!ptACDItem) return FALSE;
    if (ptACDItem->hGB.gbid == gbidUnlicensedItem) {
        SACDInventoryDeleteItem(*ptACDItem, ptACDItem->GetStackCount(), FALSE, FALSE);
        return TRUE;
    }
    GBID gbidTransmog = ptACDItem->AttributesGetInt(static_cast<FastAttribKey>(0xfffff1a6));
    if (gbidTransmog != -1 && ItemCrafting_ItemTransmogsMatch(gbidTransmog, gbidUnlicensedItem))
        ptACDItem->AttributesSetInt(static_cast<FastAttribKey>(0xfffff1a6), -1);
    DyeType eItemDyeType = ptACDItem->GetDyeType();
    if (eItemDyeType != 0 && ItemIsDye(gbidUnlicensedItem) && !ItemIsDye(*ptACDItem)) {
        ScopedItemDefinitionLockR pdefUnlicensedDye(gbidUnlicensedItem);
        if (pdefUnlicensedDye) {
            for (int32 i = 0; i < static_cast<int32>(pdefUnlicensedDye->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); ++i) {
                if (pdefUnlicensedDye->ptAttributes[i].eAttribute == 0x193) {
                    SavedDyeType eUnlicensedSavedDyeType = static_cast<SavedDyeType>(ItemGetAttributeSpecifierValue(gbidUnlicensedItem, pdefUnlicensedDye->ptAttributes[i], 0, 0, FALSE, FALSE));
                    DyeType eUnlicensedDyeType = DyeTypeTranslateFromSavedDyeType(eUnlicensedSavedDyeType);
                    if (eUnlicensedDyeType == eItemDyeType)
                        ptACDItem->AttributesSetInt(static_cast<FastAttribKey>(0xfffff193), 0);
                    break;
                }
            }
        }
    }
    return FALSE;
}
#line 184 "X/Common/Console/ConsoleLicenses.cpp"
static void sDeleteUnlicensedItemsCB(ACDID idACDSquare, InventorySlotType, void* pUserData)
{
    GBID gbidUnlicensedItem = static_cast<GBID>(reinterpret_cast<intptr_t>(pUserData));
    ActorCommonData* ptACDItem = ACDTryToGet(idACDSquare);
    sModifyItemIfUnlicensed(ptACDItem, gbidUnlicensedItem);
}
#line 196 "X/Common/Console/ConsoleLicenses.cpp"
static void sRemoveUnlicensedTransmogCB(ACDID idACDSquare, InventorySlotType, void* pUserData)
{
    ActorCommonData* ptACDItem = ACDTryToGet(idACDSquare);
    if (ptACDItem) {
        GBID gbidTransmog = ptACDItem->AttributesGetInt(static_cast<FastAttribKey>(0xfffff1a6));
        GBID gbidUnlicensedItem = static_cast<GBID>(reinterpret_cast<intptr_t>(pUserData));
        if (gbidTransmog != -1 && ItemCrafting_ItemTransmogsMatch(gbidTransmog, gbidUnlicensedItem))
            ptACDItem->AttributesSetInt(static_cast<FastAttribKey>(0xfffff1a6), -1);
    }
}
#line 217 "X/Common/Console/ConsoleLicenses.cpp"
static void sDeleteUnlicensedItems(
    Player& tPlayer,
    ActorCommonData& tACDPlayer,
    GBID gbidUnlicensedItem)
{

    for (int32 s = 0; s < 17; ++s)
    {
        ACDInventoryIterateItems(tACDPlayer, s, 0, sDeleteUnlicensedItemsCB, reinterpret_cast<void*>(static_cast<intptr_t>(gbidUnlicensedItem)));
    }


    ACDID idACDHireling = PlayerGetHireling(tACDPlayer.id);
    ActorCommonData* ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling)
    {
        for (int32 s = 20; s < 26; ++s)
        {
            ACDInventoryIterateItems(*ptACDHireling, s, 0, sDeleteUnlicensedItemsCB, reinterpret_cast<void*>(static_cast<intptr_t>(gbidUnlicensedItem)));
        }
    }


    for (int32 i = 0; i < 4; ++i)
    {
        for (int32 s = 0; s < NUM_HIRELING_SLOTS; ++s)
        {
            ACDID& idACDItem = tPlayer.tHirelingStable.tHireling[i].tItems.idACDItems[s];
            ActorCommonData* ptACDItem = ACDTryToGet(idACDItem);
            if (sModifyItemIfUnlicensed(ptACDItem, gbidUnlicensedItem))
            {
                idACDItem = ACDID_INVALID;
            }
        }
    }


    sRemoveUnlicensedTransmog(tPlayer.tCrafterSavedData.tTransmogData, gbidUnlicensedItem);

    ScopedItemDefinitionLockR pdefUnlicensedItem(gbidUnlicensedItem);
    if (pdefUnlicensedItem && ItemHasLabel(gbidUnlicensedItem, 0x72))
    {
        for (int32 i = 0; i < 8; ++i)
        {
            GBID gbidUnlicensedTransmog = pdefUnlicensedItem->gbidTransmogsToGrant[i];
            if (gbidUnlicensedTransmog == GBID_NONE)
            {
                continue;
            }


            for (int32 s = 0; s < 17; ++s)
            {
                ACDInventoryIterateItems(tACDPlayer, s, 0, sRemoveUnlicensedTransmogCB, reinterpret_cast<void*>(static_cast<intptr_t>(gbidUnlicensedTransmog)));
            }


            ACDID idACDHireling2 = PlayerGetHireling(tACDPlayer.id);
            ActorCommonData* ptACDHireling2 = ACDTryToGet(idACDHireling2);
            if (ptACDHireling2)
            {
                for (int32 s = 20; s < 26; ++s)
                {
                    ACDInventoryIterateItems(*ptACDHireling2, s, 0, sRemoveUnlicensedTransmogCB, reinterpret_cast<void*>(static_cast<intptr_t>(gbidUnlicensedTransmog)));
                }
            }


            for (int32 j = 0; j < 4; ++j)
            {
                for (int32 s = 0; s < NUM_HIRELING_SLOTS; ++s)
                {
                    ACDID idACDItem = tPlayer.tHirelingStable.tHireling[j].tItems.idACDItems[s];
                    sRemoveUnlicensedTransmogCB(idACDItem, -1, reinterpret_cast<void*>(static_cast<intptr_t>(gbidUnlicensedTransmog)));
                }
            }


            sRemoveUnlicensedTransmog(tPlayer.tCrafterSavedData.tTransmogData, gbidUnlicensedTransmog);
        }
    }
}
#line 302 "X/Common/Console/ConsoleLicenses.cpp"
static void sDeleteUnlicensedCosmetic(Player& tPlayer, ActorCommonData& tACDPlayer, GBID gbidUnlicensedCosmetic)
{
    BOOL bDeleted = FALSE;
    for (int32 n = 0; n < tPlayer.tCosmeticItemSavedData.tCosmeticItemData.nNumCosmeticItems; ++n) {
        if (tPlayer.tCosmeticItemSavedData.tCosmeticItemData.gbidCosmeticItems[n] == gbidUnlicensedCosmetic) {
            tPlayer.tCosmeticItemSavedData.tCosmeticItemData.gbidCosmeticItems[n] = 0;
            bDeleted = TRUE;
        }
        if (bDeleted && n < tPlayer.tCosmeticItemSavedData.tCosmeticItemData.nNumCosmeticItems - 1)
            tPlayer.tCosmeticItemSavedData.tCosmeticItemData.gbidCosmeticItems[n] = tPlayer.tCosmeticItemSavedData.tCosmeticItemData.gbidCosmeticItems[n + 1];
    }
    if (bDeleted) --tPlayer.tCosmeticItemSavedData.tCosmeticItemData.nNumCosmeticItems;
    CosmeticItemType eType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbidUnlicensedCosmetic);
    VisualCosmeticItemSlot eSlot = eType - 1;
    if (static_cast<uint32>(eSlot) < 3 && VisualInventoryGetCosmeticItemInSlot(tPlayer.idPlayerACD, eSlot) == gbidUnlicensedCosmetic) {
        if (eSlot == 2) SPlayerKillActiveCosmeticPet(tPlayer);
        else VisualInventorySetCosmeticItemInSlot(tPlayer.idPlayerACD, eSlot, -1);
    }
}

#line 364 "X/Common/Console/ConsoleLicenses.cpp"
void ConsoleUpdateBonusItems(Player& tPlayer, ActorCommonData& tACDPlayer, const uint32 uConsoleLicenses)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) || GameIsWeeklyChallenge()) return;
    for (int32 i = 0; i < 7; ++i) {
        ConsoleBonusItemsFromLicense& tLicenseInfo = s_arConsoleLicenseItems[i];
        LooteeInfo tLootee;
        tLootee.InitializeFromACD(tACDPlayer, ACDID_INVALID);
        tLootee.snoTreasureClass = SNOFileLookupGUID(52, tLicenseInfo.szTreasureClass);
        tLootee.tLooteeParams.bSuppressMagicAndGoldFind = TRUE;
        tLootee.tLooteeParams.bSuppressUnluckyBonus = TRUE;
        tLootee.tLooteeParams.bIgnoreKnownRecipeCheck = TRUE;
        tLootee.tLooteeParams.bSuppressGiftGeneration = TRUE;
        tLootee.tLooteeParams.bIgnoreLevelRestriction = TRUE;
        if (tLootee.snoTreasureClass == -1) {
            tPlayer.uAcceptedLicenseBitsHero &= ~(uint64(1) << tLicenseInfo.nLicense);
            continue;
        }
        if (tLicenseInfo.eLicenseRequirement == LicenseAutomatic || (uConsoleLicenses & (1U << tLicenseInfo.nLicense))) {
            if (GameIsSeasonal() && tLicenseInfo.eRewardSeasonal == ExcludeSeasonal) continue;
            if (tPlayer.uAcceptedLicenseBitsHero & (uint64(1) << tLicenseInfo.nLicense)) continue;
            BOOL bSendToStash = tLicenseInfo.eRewardDestination == RewardToStash;
            LootGiveTreasureToPlayer(tLootee, tACDPlayer, 0, 0, bSendToStash, TRUE);
            tPlayer.uAcceptedLicenseBitsHero |= uint64(1) << tLicenseInfo.nLicense;
        }
        else {
            BOOL bRevoke = (uConsoleLicenses & (1U << LICENSE_CROSS_PLATFORM_IMPORT)) || tLicenseInfo.eLicenseRequirement == LicenseRevokable;
            if (bRevoke) {
                LootSpecifierArray arLootSpecifiers;
                if (LootGetLootSpecifiers(tLootee, tACDPlayer.id, arLootSpecifiers, 0, FALSE, 1, 0)) {
                    int32 nNumItems = arLootSpecifiers.GetArraySize();
                    for (int32 j = 0; j < nNumItems; ++j) {
                        sDeleteUnlicensedItems(tPlayer, tACDPlayer, arLootSpecifiers[j].hGBItem.gbid);
                        GBID gbidItem = arLootSpecifiers[j].hGBItem.gbid;
                        if (ItemIsCosmeticItem(gbidItem))
                            sDeleteUnlicensedCosmetic(tPlayer, tACDPlayer, gbidItem);
                    }
                }
                ScopedTreasureClassDefinitionLockR pdefTreasureClass(tLootee.snoTreasureClass);
                for (int32 n = 0; n < pdefTreasureClass->nModifierCount; ++n) {
                    LootDropModifier& tLootDropModifier = pdefTreasureClass->ptModifierList[n];
                    GBID gbidItem = tLootDropModifier.tSpecificItem.gbidItem;
                    if (ItemIsCosmeticItem(gbidItem)) sDeleteUnlicensedCosmetic(tPlayer, tACDPlayer, gbidItem);
                }
                tPlayer.uAcceptedLicenseBitsHero &= ~(uint64(1) << tLicenseInfo.nLicense);
            }
        }
    }
}
#line 482 "X/Common/Console/ConsoleLicenses.cpp"
uint32 GetConsoleMaxHeroesAllowed(int32 nUserIndex)
{
    if (static_cast<uint32>(nUserIndex) >= 4) return 0;
    uint32 uAllowedHeroes = 12;
    uint32 uLicenses = Console::DLCManager::GetLicenses();
    uLicenses |= Console::GamerProfile::GetAccount(nUserIndex).console_data().legacy_license_bits();
    if (uLicenses & (1U << LICENSE_PREORDER_BONUS_ROS)) uAllowedHeroes = 15;
    if (uLicenses & (1U << LICENSE_NECROMANCER)) uAllowedHeroes += 2;
    return uAllowedHeroes;
}
