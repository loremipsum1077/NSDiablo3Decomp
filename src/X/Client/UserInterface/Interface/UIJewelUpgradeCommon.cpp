#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because their headers are not adopted yet (X/Common/ActorCommonData.cpp,
// X/Common/Crafting/ItemCrafting.cpp, X/Common/Definitions/StringListDefinition.cpp).
int32 ACDGetTieredLootRunLevel(const ActorCommonData & tACD);
float ItemCrafting_GetJewelUpgradeChance(const ActorCommonData & tACDJewel, int32 nTieredLootRunLevel);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
namespace UIJewelUpgrade {





int32 GetNumBonusUpgradesLeftForPlayer(const Player * ptPlayer) {
    int32 nRemainingUpgrades = 0;
    if (ptPlayer != NULL) {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL) {
            int32 nBonusUpgrades = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_BONUS);
            int32 nUpgradesUsed = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_USED);
            nRemainingUpgrades = nBonusUpgrades - nUpgradesUsed;
        }
    }
    return Math::Max(nRemainingUpgrades, 0);
}








int32 GetNumUpgradesLeftForPlayer(const Player * ptPlayer) {
    int32 nRemainingUpgrades = 0;
    if (ptPlayer != NULL) {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer != NULL) {
            int32 nMaxUpgrades = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_MAX) + ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_BONUS);
            int32 nUpgradesUsed = ptACDPlayer->AttributesGetInt(ATTRIBUTE_JEWEL_UPGRADES_USED);
            nRemainingUpgrades = nMaxUpgrades - nUpgradesUsed;
        }
    }
    return nRemainingUpgrades;
}










BOOL IsJewelMaxRank(const ActorCommonData & tACDJewel) {
    int32 nMaxRank = 0;
    ScopedItemDefinitionLockR pdefItem(tACDJewel.hGB.gbid);
    if (pdefItem != NULL) {
        nMaxRank = pdefItem->nJewelMaxRank;
    }
    if (nMaxRank >= 0 && nMaxRank <= tACDJewel.AttributesGetInt(ATTRIBUTE_JEWEL_RANK)) {
        return TRUE;
    }
    return FALSE;
}























































CRefString GetJewelRankString(ACDID idJewel) {
    CRefString sRank;
    ActorCommonData * ptACDJewel = ACDTryToGet(idJewel);
    if (ptACDJewel != NULL) {
        int32 nJewelRank = ptACDJewel->AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
        sRank = LocalizedSprintf(StringListFetch("Vendor:RankLabel").str(), UnicodeText::FormatNumber(nJewelRank, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), (LPCSTR)0);
    }
    return sRank;
}










CRefString GetUpgradeChanceString(const Player * ptPlayer, ACDID idJewel) {
    CRefString sGemUpgradeChance;
    int32 nNumUpgradesLeft = GetNumUpgradesLeftForPlayer(ptPlayer);
    if (nNumUpgradesLeft > 0) {
        ActorCommonData * ptACDVendor = ptPlayer ? ACDTryToGet(ptPlayer->idACDActiveVendor) : NULL;
        ActorCommonData * ptACDItem = ACDTryToGet(idJewel);
        if (ptACDVendor != NULL && ptACDItem != NULL) {
            int32 nTieredLootRunLevel = ACDGetTieredLootRunLevel(*ptACDVendor);
            float flJewelUpgradeChance = ItemCrafting_GetJewelUpgradeChance(*ptACDItem, nTieredLootRunLevel) * 100.0f;
            int32 nJewelUpgradeChanceDecimal = Math::FloatRoundToInt(flJewelUpgradeChance);
            CRefString sUpgradeChance;
            sUpgradeChance.Format("%d%%", nJewelUpgradeChanceDecimal);
            sGemUpgradeChance = LocalizedSprintf(StringListFetch("Vendor:JewelUpgradeChance").str(), sUpgradeChance.str(), (LPCSTR)0);
        }
    }
    return sGemUpgradeChance;
}

}  // namespace UIJewelUpgrade
