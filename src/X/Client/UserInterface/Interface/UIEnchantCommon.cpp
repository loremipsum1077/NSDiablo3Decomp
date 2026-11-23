#include "UIEnchantCommon.h"

#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../../Sigma/Random.h"
// Declared here because their headers are not adopted yet; each comment names the defining file.
typedef int32 AttribGroupID;  // X/Common/Items.h
typedef int32 ToolTipValueColoring;  // X/Common/Items.h
typedef BOOL (*ItemDescriptionColorOverride)(CRefString &, const ActorCommonData &, const FastAttribKey &, void *);  // X/Common/Items.h
typedef XGrowableMap<int, int, int, int> ItemAffixFastAttribMap;  // X/Common/Items.h
struct EnchantAffixChoice {  // size 0x8, X/Common/Items.h
    GBID gbidAffix;  // 0x00
    uint32 dwSeed;  // 0x04
};
BOOL ItemHasEnchantedAffix(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
GBID ItemGetEnchantedAffixOld(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
GBID ItemGetEnchantedAffixNew(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
uint32 ItemGetEnchantedAffixSeed(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsAffixInherent(const ActorCommonData & tACDItem, const GBID gbidAffix, BOOL bIgnoreLegendaryAffixFamilies);  // X/Common/Items.cpp
void ItemGeneratorToDefaults(D3::Items::Generator & tGenerator);  // X/Common/Items.cpp
ACDID CItemGenerate(ACDNetworkName ann, const D3::Items::Generator & tGenerator, ItemAffixFastAttribMap * arAffixSpecificAttributes);  // X/Common/Items.cpp
void ItemGeneratorPopulateFromACDID(const ActorCommonData & tACDItem, D3::Items::Generator & tGenerator, BOOL bSetRestoredBit);  // X/Common/Items.cpp
CRefString ItemGetPowerBonusAttributeDescriptions(const CRefString & sLinePrefix, const ActorCommonData * ptACDItem, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const PlayerClass ePlayerClass, ToolTipValueColoring eColoringType, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData);  // X/Common/Items.cpp
CRefString ItemGetAttributeTip(LPCTSTR sLinePrefix, AttribGroupID idFastAttribMin, AttribGroupID idFastAttribMax, ToolTipValueColoring eColoringType);  // X/Common/Items.cpp
void ACDDelete(const ACDID idACD, BOOL bWorldDeleted);  // X/Common/ActorCommonData.cpp
int32 ItemCompareAffixes(const void * lhs, const void * rhs);  // X/Client/CItems.cpp
void FastAttribGroupDelete(AttribGroupID idAttribGroup);  // X/Common/Attributes/FastAttribute.cpp
// Declared here because their headers are not adopted yet; each comment names the defining file.
typedef int32 HirelingClass;  // X/Common/GameBalance.h
typedef int32 AncientRank;  // X/Common/GameBalance.h
struct Affix;  // X/Common/Definitions/GameBalanceDefinition.h
struct ItemAffixSpecifier {  // size 0x50, X/Common/Items.h
    ItemAffixSpecifier();
    GBID gbidItem;  // 0x00
    const Affix * ptAffix;  // 0x08
    int32 nAffixLevel;  // 0x10
    int32 eIQL;  // 0x14
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













enum {
    VAF_PICK_MAJOR_AFFIXES = 7,  // X/Common/Items.h
    VAF_PICK_MINOR_AFFIXES = 8,
    VAF_CANT_ADD_SOCKETS = 9,
};
enum {
    AFFIXGROUP_MAJOR = 0,  // X/Common/Definitions/GameBalanceDefinition.h
    AFFIXGROUP_MINOR = 1,
};
static const int32 MAX_AFFIX_ATTRIBUTES = 4;  // GameBalanceDefinition.h:1377
static const int32 MAX_LEGENDARY_AFFIX_FAMILIES = 6;  // GameBalanceDefinition.h:458
int32 ItemGetItemLevel(const GBID gbidItem);  // X/Common/Items.cpp
int32 ItemGetQualityLevel(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
void ItemGetValidTargetedClass(PlayerClass & ePlayerClass, HirelingClass & eHirelingClass, const GBID gbidItem, const PlayerClass eRequestedClass, RNG * ptRNG, BOOL bIgnoreHirelings);  // X/Common/Items.cpp
void ItemCrafting_GetValidAffixes(const ItemAffixSpecifier & tAffixSpecifier, GBIDArray & arValidAffixes, int32 nMaxCount);  // X/Common/Crafting/ItemCrafting.cpp
int32 ItemAffixGetFrequencyWeight(const AffixTableEntry * ptAffix, PlayerClass ePlayerClass, HirelingClass eHirelingClass);  // X/Common/Items.cpp
BOOL ItemAffixAddsSockets(const GBID gbidAffix);  // X/Common/Items.cpp
void ItemUpdateMinMaxAttributeStats(AttribGroupID idAttribGroupMin, AttribGroupID idAttribGroupMax, GBID gbidItem, const AttributeSpecifier * arAttributes, int32 nAttributeCount, int32 nFlags, blz::vector<int, blz::allocator<int> > * parValues, XGrowableMap<int, int, int, int> * pmapValues, const ActorCommonData * ptACDItem);  // X/Common/Items.cpp
AttribGroupID FastAttribGroupCreate(int32 eType, int32 nParam);  // X/Common/Attributes/FastAttribute.cpp
void Set_Text_Color(CRefString & sText, int32 eColor);  // X/Common/UserInterface/UITextControl.cpp
// The includes and declarations above are the only physical offset; the rest of the file keeps the original line numbers.
#line 29
static LPCSTR pszBulletPrefix = "{icon:bullet}";

static LPCSTR pszEnchantedPrefix = "{icon:enchanted}";

namespace UIEnchant {


//----------------------------------------------------------------------------
void EnumItemAffixes(
    ACDID idACDItem,
    PlayerClass ePlayerClass,
    AffixInfoArray & arAffixes)
{

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
    {
        return;
    }

    GBID gbidEnchantRerolledAffix = ItemGetEnchantedAffixOld(*ptACDItem);
    GBID gbidEnchantReplacementAffix = ItemGetEnchantedAffixNew(*ptACDItem);
    BOOL bEnchanted = ItemHasEnchantedAffix(*ptACDItem);



    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);
    ItemGeneratorPopulateFromACDID(*ptACDItem, tGenerator, TRUE);


    ItemAffixFastAttribMap arAffixSpecificAttributes(16, NULL);
    ACDID idACDNewItem = CItemGenerate((ACDNetworkName)0x2038, tGenerator, &arAffixSpecificAttributes);
    ActorCommonData * ptACDNewItem = ACDTryToGet(idACDNewItem);
    if (ptACDNewItem != NULL)
    {


        ACDDelete(idACDNewItem, FALSE);
    }

    BOOL bEnchantAffixesMatch = (gbidEnchantRerolledAffix == gbidEnchantReplacementAffix);
    blz::vector<int, blz::allocator<int> > arSortedAffixGBIDs;
    POS pos = arAffixSpecificAttributes.GetStartAssoc();
    while (pos)
    {
        GBID gbidAffix;
        AttribGroupID idAffixFastAttrib;
        arAffixSpecificAttributes.GetNextAssoc(pos, gbidAffix, idAffixFastAttrib);
        arSortedAffixGBIDs.push_back(gbidAffix);
    }
    blzext::vector_sort(arSortedAffixGBIDs, ItemCompareAffixes);


    for (uint32 i = 0; i < arSortedAffixGBIDs.size(); i++)
    {
        GBID gbidAffix = arSortedAffixGBIDs[i];
        AttribGroupID idAffixFastAttrib;
        if (!arAffixSpecificAttributes.Lookup(gbidAffix, idAffixFastAttrib))
        {
            continue;
        }



        if (!bEnchantAffixesMatch)
        {
            if (gbidAffix == gbidEnchantReplacementAffix)
            {
                continue;
            }


            if (gbidAffix == gbidEnchantRerolledAffix)
            {
                AttribGroupID idReplacementFastAttrib;
                if (arAffixSpecificAttributes.Lookup(gbidEnchantReplacementAffix, idReplacementFastAttrib))
                {
                    FastAttribGroupDelete(idAffixFastAttrib);
                    idAffixFastAttrib = idReplacementFastAttrib;
                }
            }
        }



        BOOL bInherentAffix = ItemIsAffixInherent(*ptACDItem, gbidAffix, TRUE);
        if (bInherentAffix || (bEnchanted && gbidAffix != gbidEnchantRerolledAffix))
        {

            FastAttribGroupDelete(idAffixFastAttrib);
            continue;
        }




        LPCSTR pszStringPrefix = (!bEnchantAffixesMatch && gbidAffix == gbidEnchantRerolledAffix) ? pszEnchantedPrefix : pszBulletPrefix;



        CRefString sStringPrefix(CRefString().Format("%s ", pszStringPrefix));


        CRefString sAffixText = ItemGetAttributeTip(sStringPrefix.str(), idAffixFastAttrib, -1, TRUE);
        sAffixText += ItemGetPowerBonusAttributeDescriptions(sStringPrefix, NULL, idAffixFastAttrib, -1, ePlayerClass, TRUE, NULL, NULL);


        FastAttribGroupDelete(idAffixFastAttrib);


        AffixInfo tAffixInfo(gbidAffix, FALSE, sAffixText);
        arAffixes.push_back(tAffixInfo);
    }
}



//----------------------------------------------------------------------------
void EnumReplacementItemAffixes(
    ACDID idACDItem,
    PlayerClass ePlayerClass,
    GBID gbidSelectedAffix,
    const EnchantAffixChoice * arEnchantAffixChoices,
    int32 nEnchantAffixChoicesCount,
    AffixInfoArray & arAffixes,
    int32 & nOutOriginalAffixIndex)
{

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
    {
        return;
    }


    D3::Items::Generator tBaseGenerator;
    ItemGeneratorToDefaults(tBaseGenerator);
    ItemGeneratorPopulateFromACDID(*ptACDItem, tBaseGenerator, TRUE);

    for (int32 i = 0; i < nEnchantAffixChoicesCount; i++)
    {
        GBID gbidAffix = arEnchantAffixChoices[i].gbidAffix;

        D3::Items::Generator tGenerator(tBaseGenerator);
        if (gbidAffix != -1)
        {
            tGenerator.set_enchanted_affix_old(gbidSelectedAffix);
            tGenerator.set_enchanted_affix_new(gbidAffix);
            tGenerator.set_enchanted_affix_seed(arEnchantAffixChoices[i].dwSeed);
        }
        else
        {

            nOutOriginalAffixIndex = i;
            gbidAffix = gbidSelectedAffix;
            GBID gbidEnchantedAffixNew = ItemGetEnchantedAffixNew(*ptACDItem);
            if (gbidEnchantedAffixNew != -1)
            {
                tGenerator.set_enchanted_affix_old(gbidSelectedAffix);
                tGenerator.set_enchanted_affix_new(gbidEnchantedAffixNew);
                tGenerator.set_enchanted_affix_seed(ItemGetEnchantedAffixSeed(*ptACDItem));
                gbidAffix = gbidEnchantedAffixNew;
            }
        }







        ItemAffixFastAttribMap arAffixSpecificAttributes(16, NULL);
        ACDID idACDNewItem = CItemGenerate((ACDNetworkName)0x2038, tGenerator, &arAffixSpecificAttributes);
        ACDDelete(idACDNewItem, FALSE);

        CRefString sStringPrefix(CRefString().Format("%s ", pszBulletPrefix));

        CRefString sAffixText;
        POS pos = arAffixSpecificAttributes.GetStartAssoc();
        while (pos)
        {
            GBID gbidItemAffix;
            AttribGroupID idAffixFastAttrib;
            arAffixSpecificAttributes.GetNextAssoc(pos, gbidItemAffix, idAffixFastAttrib);

            if (gbidItemAffix == gbidAffix)
            {
                sAffixText = ItemGetAttributeTip(sStringPrefix.str(), idAffixFastAttrib, -1, TRUE);
                sAffixText += ItemGetPowerBonusAttributeDescriptions(sStringPrefix, NULL, idAffixFastAttrib, -1, ePlayerClass, TRUE, NULL, NULL);
            }


            FastAttribGroupDelete(idAffixFastAttrib);
        }

        if (gbidAffix != -1)
        {

            AffixInfo tAffixInfo(gbidAffix, FALSE, sAffixText);
            arAffixes.push_back(tAffixInfo);
        }
    }
}

}  // namespace UIEnchant


struct CraftingGroupedAffix {
    AttributeSpecifier arAffixAttributes[MAX_AFFIX_ATTRIBUTES];  // 0x00
    AttribGroupID idAttribGroupMin;  // 0x60
    AttribGroupID idAttribGroupMax;  // 0x64
};
static_assert(sizeof(CraftingGroupedAffix) == 0x68, "CraftingGroupedAffix");

namespace UIEnchant {



void EnumPossibleReplacementAffixes(
    ACDID idACDItem,
    PlayerClass ePlayerClass,
    int32 nAffixLevelOverride,
    uint32 dwOverrideFlags,
    GBID gbidAffixToReplace,
    ReplacementAffixInfoArray & arReplacementAffixes)
{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem == NULL)
    {
        return;
    }


    int32 nLegendaryItemLevel = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
    if (nLegendaryItemLevel > 0 && nAffixLevelOverride < 1)
    {
        nAffixLevelOverride = nLegendaryItemLevel;
    }


    ItemAffixSpecifier tAffixSpecifier;
    tAffixSpecifier.gbidItem = ptACDItem->hGB.gbid;
    tAffixSpecifier.nAffixLevel = (nAffixLevelOverride > 0) ? nAffixLevelOverride : ItemGetItemLevel(ptACDItem->hGB.gbid);
    tAffixSpecifier.eIQL = ItemGetQualityLevel(*ptACDItem);
    tAffixSpecifier.eAncientRank = ptACDItem->AttributesGetInt(ATTRIBUTE_ANCIENT_RANK);
    tAffixSpecifier.dwFlags = dwOverrideFlags;


    RNG tRNG;
    RandInit(tRNG);

    if (ItemHasEnchantedAffix(*ptACDItem) &&
        ptACDItem->AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_SEED) != 0)
    {
        tRNG.dwSeed = ptACDItem->AttributesGetInt(ATTRIBUTE_ENCHANTED_AFFIX_SEED);
    }
    else
    {
        tRNG.dwSeed = ptACDItem->AttributesGetInt(ATTRIBUTE_SEED);
    }

    ItemGetValidTargetedClass(tAffixSpecifier.eTargetedPlayerClass, tAffixSpecifier.eTargetedHirelingClass, ptACDItem->hGB.gbid, ePlayerClass, &tRNG, FALSE);


    BOOL bAffixValid = FALSE;
    if (gbidAffixToReplace != GBID_NONE)
    {

        POS pos = ptACDItem->listIdentifiedAffixGBIDs.GetHeadPosition();
        while (pos)
        {
            GBID gbidAffix = ptACDItem->listIdentifiedAffixGBIDs.GetNext(pos);
            if (gbidAffix == gbidAffixToReplace)
            {
                bAffixValid = TRUE;
                break;
            }
        }


        ScopedItemAffixLockR pdefAffixToReplace(gbidAffixToReplace);
        if (bAffixValid && pdefAffixToReplace)
        {
            pos = ptACDItem->listIdentifiedAffixGBIDs.GetHeadPosition();
            while (pos)
            {
                GBID gbidAffix = ptACDItem->listIdentifiedAffixGBIDs.GetNext(pos);
                if (gbidAffix != gbidAffixToReplace)
                {
                    tAffixSpecifier.arCurrentAffixes[tAffixSpecifier.nCurrentAffixesCount] = gbidAffix;
                    tAffixSpecifier.nCurrentAffixesCount++;
                }
            }

            if (pdefAffixToReplace->eAffixGroup == AFFIXGROUP_MAJOR)
                tAffixSpecifier.dwFlags |= (1 << VAF_PICK_MAJOR_AFFIXES);
            else if (pdefAffixToReplace->eAffixGroup == AFFIXGROUP_MINOR)
                tAffixSpecifier.dwFlags |= (1 << VAF_PICK_MINOR_AFFIXES);





            if (ptACDItem->AttributesGetInt(ATTRIBUTE_CONSUMABLE_ADD_SOCKETS))
            {
                tAffixSpecifier.dwFlags |= (1 << VAF_CANT_ADD_SOCKETS);
            }





            ScopedItemDefinitionLockR pdefItem(ptACDItem->hGB.gbid);
            if (pdefItem)
            {
                for (int32 i = 0; i < MAX_LEGENDARY_AFFIX_FAMILIES; i++)
                {
                    GBID gbidLegendaryAffixFamily = pdefItem->gbidLegendaryAffixFamily[i];
                    if (gbidLegendaryAffixFamily != GBID_NONE &&
                        pdefItem->bUseLegendaryAllowedTypes[i] &&
                        (pdefAffixToReplace->gbidAffixFamily0 == gbidLegendaryAffixFamily ||
                         pdefAffixToReplace->gbidAffixFamily1 == gbidLegendaryAffixFamily))
                    {
                        tAffixSpecifier.gbidAllowedLegendaryAffixFamily = gbidLegendaryAffixFamily;
                        break;
                    }
                }
            }
        }
    }


    GBIDArray arValidAffixes;
    ItemCrafting_GetValidAffixes(tAffixSpecifier, arValidAffixes, -1);


    if (bAffixValid)
    {
        ScopedItemAffixLockR pdefAffix(gbidAffixToReplace);
        if (pdefAffix)
        {
            int32 nAffixFrequencyWeight = ItemAffixGetFrequencyWeight(pdefAffix, tAffixSpecifier.eTargetedPlayerClass, tAffixSpecifier.eTargetedHirelingClass);
            if (nAffixFrequencyWeight > 0 &&
                (!(tAffixSpecifier.dwFlags & (1 << VAF_CANT_ADD_SOCKETS)) || !ItemAffixAddsSockets(pdefAffix->tHeader.gbid)))
            {
                arValidAffixes.push_back(gbidAffixToReplace);
            }
        }
    }


    blzext::vector_sort(arValidAffixes, ItemCompareAffixes);
    uint32 nValidAffixesCount = arValidAffixes.size();

    uint32 nGroupedAffixesCount = 0;
    blz::vector<CraftingGroupedAffix, blz::allocator<CraftingGroupedAffix> > arGroupedAffixes;

















    for (uint32 v = 0; v < nValidAffixesCount; v++)
    {
        ScopedItemAffixLockR pdefAffix(arValidAffixes[v]);
        if (pdefAffix == NULL)
        {
            continue;
        }
        nGroupedAffixesCount = arGroupedAffixes.size();
        BOOL bAddNewGroupedAffix = TRUE;

        for (uint32 g = 0; g < nGroupedAffixesCount; g++)
        {
            const CraftingGroupedAffix & tGroupedAffix = arGroupedAffixes[g];

            BOOL bMatches = TRUE;
            for (int32 a = 0; a < MAX_AFFIX_ATTRIBUTES; a++)
                if (tGroupedAffix.arAffixAttributes[a] != pdefAffix->ptAffixAttributes[a])
                {
                    bMatches = FALSE;
                    break;
                }



            if (bMatches)
            {
                ItemUpdateMinMaxAttributeStats(tGroupedAffix.idAttribGroupMin, tGroupedAffix.idAttribGroupMax, ptACDItem->hGB.gbid, pdefAffix->ptAffixAttributes, MAX_AFFIX_ATTRIBUTES, 0, NULL, NULL, ptACDItem);
                bAddNewGroupedAffix = FALSE;
                break;
            }
        }


        if (bAddNewGroupedAffix)
        {
            CraftingGroupedAffix tNewGroupedAffix;
            SigmaMemoryCopy(tNewGroupedAffix.arAffixAttributes, pdefAffix->ptAffixAttributes, sizeof(tNewGroupedAffix.arAffixAttributes));
            tNewGroupedAffix.idAttribGroupMin = FastAttribGroupCreate(2, -1);
            tNewGroupedAffix.idAttribGroupMax = FastAttribGroupCreate(2, -1);

            ItemUpdateMinMaxAttributeStats(tNewGroupedAffix.idAttribGroupMin, tNewGroupedAffix.idAttribGroupMax, ptACDItem->hGB.gbid, pdefAffix->ptAffixAttributes, MAX_AFFIX_ATTRIBUTES, 0, NULL, NULL, ptACDItem);

            arGroupedAffixes.push_back(tNewGroupedAffix);
        }
    }

    nGroupedAffixesCount = arGroupedAffixes.size();

    for (uint32 i = 0; i < nGroupedAffixesCount; i++)
    {
        const CraftingGroupedAffix & tGroupedAffix = arGroupedAffixes[i];

        CRefString sAffixStats = ItemGetAttributeTip("{icon:bullet} ", tGroupedAffix.idAttribGroupMin, tGroupedAffix.idAttribGroupMax, 2);
        sAffixStats += ItemGetPowerBonusAttributeDescriptions("{icon:bullet} ", NULL, tGroupedAffix.idAttribGroupMin, tGroupedAffix.idAttribGroupMax, ePlayerClass, 2, NULL, NULL);
        Set_Text_Color(sAffixStats, FONTCOLOR_BONUS);


        FastAttribGroupDelete(tGroupedAffix.idAttribGroupMax);
        FastAttribGroupDelete(tGroupedAffix.idAttribGroupMin);


        arReplacementAffixes.push_back(sAffixStats);
    }
}

}  // namespace UIEnchant
