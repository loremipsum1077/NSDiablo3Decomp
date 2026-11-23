// X/Common/UserInterface/UIInventory.cpp

#include "UIInventory.h"
#include "../ActorCommonData.h"
#include "../Definitions/GameBalanceDefinition.h"
#include "../Items.h"
#include "../ItemsSwig.h"
#include "../ActorInventory.h"
#include "../Attributes/FastAttribConstantsSwig.h"
#include "../Attributes/FastAttributeStructs.h"
#include "../Definitions/StringListDefinition.h"
#include "../Game.h"
#include "../GlobalSNO.h"
#include "../Attributes/AttributeEnums.h"
#include "../Attributes/FastAttribute.h"
#include "../SNO/SNOFiles.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/UnicodeText.h"
#include "../../Client/Input/Input.h"
#include "UIToolTips.h"
#include "UITextControl.h"
#include "../Player.h"
#include "../Powers/PowersCommon.h"
#include "../Definitions/PowerDefinition.h"
#include "../Protobuf/Items.pb.h"

#include <float.h>

// GB_ITEMS comes from GameBalanceDefinition.h.


// 0x71005c60c0  size 96  (UIInventory.cpp:40)
ItemTooltipPlayerData::ItemTooltipPlayerData(const ActorCommonData * ptACDPlayer, const ItemTooltipAlternateInventory * ptAlternateInventory)
{
    ePlayerClass = 1;
    fIsMale = TRUE;

    if (ptACDPlayer)
    {
        Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);

        ePlayerClass = PlayerGetPlayerClass(ptACDPlayer);
        fIsMale = PlayerIsMale(*ptPlayer);
    }
    this->ptACDPlayer = ptACDPlayer;
    this->ptAlternateInventory = ptAlternateInventory;
}

// 0x71005c6120  size 212  (UIInventory.cpp:61)
LPCSTR UIInventory::ItemGetLabelForSlot(const InventorySlotType eSlot, const BOOL fIs2Handed)
{
    LPCSTR szLabel;

    switch (eSlot)
    {
    case SLOT_PLAYER_RIGHT_HAND:
    case SLOT_HIRELING_RIGHT_HAND:
        szLabel = "OffHand";
        break;
    case SLOT_PLAYER_HEAD:
        szLabel = "Head";
        break;
    case SLOT_PLAYER_TORSO:
        szLabel = "Chest";
        break;
    case SLOT_PLAYER_LEFT_HAND:
    case SLOT_HIRELING_LEFT_HAND:
        szLabel = fIs2Handed ? "2Hand" : "1Hand";
        break;
    case SLOT_PLAYER_HANDS:
        szLabel = "Hands";
        break;
    case SLOT_PLAYER_WAIST:
        szLabel = "Waist";
        break;
    case SLOT_PLAYER_FEET:
        szLabel = "Feet";
        break;
    case SLOT_PLAYER_SHOULDERS:
        szLabel = "Shoulders";
        break;
    case SLOT_PLAYER_LEGS:
        szLabel = "Legs";
        break;
    case SLOT_PLAYER_BRACERS:
        szLabel = "Wrists";
        break;
    case SLOT_PLAYER_RIGHT_FINGER:
    case SLOT_PLAYER_LEFT_FINGER:
    case SLOT_HIRELING_RIGHT_FINGER:
    case SLOT_HIRELING_LEFT_FINGER:
        szLabel = "Finger";
        break;
    case SLOT_PLAYER_NECK:
    case SLOT_HIRELING_NECK:
        szLabel = "Neck";
        break;
    case SLOT_HIRELING_SPECIAL:
        szLabel = "FollowerSpecial";
        break;
    default:
        szLabel = "";
        break;
    }

    return szLabel;
}

// (UIInventory.cpp:118) inlined into GetSetItemTooltip
inline CRefString UIInventory::GetSetItemTooltipName(const GBID gbidSet)
{
    SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SETS);
    CRefString usSetName = StringListFetch(snoStringList, GBIDToString(gbidSet, GB_SET_ITEM_BONUSES, FALSE).str());
    Set_Text_Color(usSetName, FONTCOLOR_GREEN);
    return usSetName;
}

// 0x71005c6200  size 116  (UIInventory.cpp:131)
int32 SortBonuses(const void * arg0, const void * arg1)
{
    GBHandle * pthGB1 = (GBHandle *)arg1;
    GBHandle * pthGB0 = (GBHandle *)arg0;

    const GameBalanceDefinition * pdefGB0;
    SetItemBonusTableEntry * pdefSetItemBonus0 = (SetItemBonusTableEntry *)GBLockR(*pthGB0, pdefGB0, TRUE);
    const GameBalanceDefinition * pdefGB1;
    SetItemBonusTableEntry * pdefSetItemBonus1 = (SetItemBonusTableEntry *)GBLockR(*pthGB1, pdefGB1, TRUE);
    int32 nLevel0 = pdefSetItemBonus0->nRequired;
    int32 nLevel1 = pdefSetItemBonus1->nRequired;

    GBUnlockR(pdefGB0);
    GBUnlockR(pdefGB1);

    return nLevel0 - nLevel1;
}

// 0x71005c6280  size 32  (UIInventory.cpp:154)
CRefString UIInventory::GetSetItemTooltip(const ActorCommonData * ptACDPlayer, const PlayerClass ePlayerClass, const ActorCommonData & tACDItem, const blz::unordered_map<int, SetTooltipInfo, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, SetTooltipInfo> > > * pPrecachedSetItemTooltipStrings)
{
    return GetSetItemTooltip(ptACDPlayer, NULL, ePlayerClass, tACDItem, pPrecachedSetItemTooltipStrings);
}

// (UIInventory.cpp:166) inlined into GetSetItemTooltip
inline BOOL sHasItem(const ActorCommonData * ptACDOwner, GBHandle & hGB, InventorySlotType * pSlots, const ItemTooltipAlternateInventory * ptAlternateInventory, int32 & nItemsEquipped)
{
    int32 nItems = 0;

    if (pSlots)
    {
        ACDID idACDItemToCheck = ACDID_INVALID;
        nItems = ActorInventoryCountItemsOfType(*ptACDOwner, hGB, pSlots, &idACDItemToCheck);
    }
    else if (ptAlternateInventory)
    {
        for (const D3::Items::Generator * const * it = ptAlternateInventory->tItems.cbegin(); it != ptAlternateInventory->tItems.cend(); ++it)
        {
            const D3::Items::Generator * item = *it;
            if (item->gb_handle().game_balance_type() == hGB.eType &&
                item->gb_handle().gbid() == hGB.gbid)
            {
                nItems++;
                nItemsEquipped++;
                break;
            }
        }
    }

    return nItems > 0;
}

// 0x71005c62a0  size 4696  (UIInventory.cpp:201)
CRefString UIInventory::GetSetItemTooltip(const ActorCommonData * ptACDOwner, const ItemTooltipAlternateInventory * ptAlternateInventory, const PlayerClass ePlayerClass, const ActorCommonData & tACDItem, const blz::unordered_map<int, SetTooltipInfo, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, SetTooltipInfo> > > * pPrecachedSetItemTooltipStrings)
{
    if (ItemIsUnidentified(tACDItem))
    {
        return "";
    }

    CRefString usToolTip;

    ScopedItemDefinitionLockR ptItem(tACDItem.hGB.gbid);
    if (!ptItem)
    {
        return usToolTip;
    }

    GBID gbidSet = ptItem->gbidSet;

    if (gbidSet != GBID_NONE && gbidSet != 0)
    {
        const SetTooltipInfo * pSetTooltipInfo = NULL;
        if (pPrecachedSetItemTooltipStrings)
        {
            const blz::unordered_map<int, SetTooltipInfo, blz::hash<int>, blz::equal_to<int>, blz::allocator<blz::pair<const int, SetTooltipInfo> > >::const_iterator & it = pPrecachedSetItemTooltipStrings->find(gbidSet);
            if (it != pPrecachedSetItemTooltipStrings->end())
            {
                pSetTooltipInfo = &it->second;
            }
        }

        int32 nCount = 0;
        if (ptACDOwner)
        {
            nCount = ptACDOwner->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SET_ITEM_COUNT, gbidSet));

            if (nCount >= 2)
            {
                nCount += ptACDOwner->AttributesGetInt(ATTRIBUTE_SET_ITEM_DISCOUNT);
            }
        }

        if (pSetTooltipInfo)
        {
            usToolTip += pSetTooltipInfo->setItemTooltipName.c_str();
        }
        else
        {
            usToolTip += GetSetItemTooltipName(gbidSet);
            usToolTip += "\n";
        }

        CRefString usSetItems;

        InventorySlotType eSlotListPlayer[] =
        {
            SLOT_PLAYER_HEAD,
            SLOT_PLAYER_TORSO,
            SLOT_PLAYER_HANDS,
            SLOT_PLAYER_WAIST,
            SLOT_PLAYER_FEET,
            SLOT_PLAYER_SHOULDERS,
            SLOT_PLAYER_LEGS,
            SLOT_PLAYER_BRACERS,
            SLOT_PLAYER_NECK,
            SLOT_PLAYER_LEFT_FINGER,
            SLOT_PLAYER_RIGHT_FINGER,
            SLOT_PLAYER_LEFT_HAND,
            SLOT_PLAYER_RIGHT_HAND,
            SLOT_NONE,
        };

        InventorySlotType eSlotListHireling[] =
        {
            SLOT_HIRELING_LEFT_HAND,
            SLOT_HIRELING_RIGHT_HAND,
            SLOT_HIRELING_SPECIAL,
            SLOT_HIRELING_LEFT_FINGER,
            SLOT_HIRELING_RIGHT_FINGER,
            SLOT_HIRELING_NECK,
            SLOT_NONE,
        };

        InventorySlotType * pSlots = NULL;
        if (ptACDOwner)
        {
            pSlots = ACDIsHireling(*ptACDOwner) ? eSlotListHireling : eSlotListPlayer;
        }

        if (pSetTooltipInfo)
        {
            for (const blz::pair<int, blz::string> & itemTooptipPair : pSetTooltipInfo->itemTooptipPairs)
            {
                CRefString usSetItem = itemTooptipPair.second.c_str();
                GBHandle hGB(GB_ITEMS, itemTooptipPair.first);
                BOOL bHasItem = sHasItem(ptACDOwner, hGB, pSlots, ptAlternateInventory, nCount);
                Set_Text_Color(usSetItem, bHasItem ? FONTCOLOR_WHITE : FONTCOLOR_GREY);

                usSetItems += usSetItem;
            }
        }
        else
        {
            POS pos;
            GBHandleList listGBHandles(GBGetHandlePool());
            GBEnumerate(GB_ITEMS, listGBHandles);

            pos = listGBHandles.GetHeadPosition();
            while (pos)
            {
                GBHandle & hGB = listGBHandles.GetNext(pos);
                const GameBalanceDefinition * pdefGBItems;
                ItemDefinition * pdefItem = (ItemDefinition *)GBLockR(hGB, pdefGBItems, TRUE);
                if (pdefItem->gbidSet == gbidSet)
                {
                    CRefString usSetItem;
                    usSetItem += "    ";
                    usSetItem += ItemGetDisplayName(hGB.gbid);

                    CRefString sSlot = GetSlotTooltip(hGB.gbid);
                    if (sSlot.Length())
                    {
                        usSetItem += CRefString(" (") + sSlot + CRefString(")");
                    }
                    usSetItem += "\n";

                    BOOL bHasItem = sHasItem(ptACDOwner, hGB, pSlots, ptAlternateInventory, nCount);
                    Set_Text_Color(usSetItem, bHasItem ? FONTCOLOR_WHITE : FONTCOLOR_GREY);

                    usSetItems += usSetItem;
                }

                GBUnlockR(pdefGBItems);
            }
        }

        if (ptAlternateInventory && nCount >= 2)
        {
            static const GBID gbidRingOfRoyalGrandeurOld = StringToGBID("Unique_Ring_107_x1");
            static const GBID gbidRingOfRoyalGrandeurNew = StringToGBID("P3_Unique_Ring_107");

            bool bFoundRing = false;
            for (const D3::Items::Generator * const * it = ptAlternateInventory->tItems.cbegin(); it != ptAlternateInventory->tItems.cend(); ++it)
            {
                const D3::Items::Generator * item = *it;
                GBID gbid = item->gb_handle().gbid();
                if (gbid == gbidRingOfRoyalGrandeurOld || gbid == gbidRingOfRoyalGrandeurNew)
                {
                    bFoundRing = true;
                    break;
                }
            }

            if (!bFoundRing)
            {
                for (int32 i = 0; i < 3; i++)
                {
                    if (ptAlternateInventory->gbidLegendaryPowers[i] == gbidRingOfRoyalGrandeurOld ||
                        ptAlternateInventory->gbidLegendaryPowers[i] == gbidRingOfRoyalGrandeurNew)
                    {
                        bFoundRing = true;
                        break;
                    }
                }
            }

            if (bFoundRing)
            {
                nCount++;
            }
        }

        usToolTip += usSetItems;

        CRefString usSetBonuses;
        GBHandleList listGBSetBonuses(GBGetHandlePool());
        GBEnumerate(GB_SET_ITEM_BONUSES, listGBSetBonuses);

        GBHandleList listGBSetBonusesSorted(GBGetHandlePool());

        POS pos = listGBSetBonuses.GetTailPosition();
        while (pos)
        {
            GBHandle & hGB = listGBSetBonuses.GetPrev(pos);

            const GameBalanceDefinition * pdefGBItems;
            SetItemBonusTableEntry * pdefSetItemBonus = (SetItemBonusTableEntry *)GBLockR(hGB, pdefGBItems, TRUE);
            if (pdefSetItemBonus->gbidSet == gbidSet &&
                pdefSetItemBonus->nRequired > 0)
            {
                listGBSetBonusesSorted.AddTail_Tracked(hGB);
            }
            GBUnlockR(pdefGBItems);
        }

        listGBSetBonusesSorted.Sort(SortBonuses);

        RNG tRNG;
        RandSetSeed_Internal(tRNG, 0);

        SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SETS);
        SNO snoItemPassivePowerStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS);
        CRefString usSetPrefix = StringListFetch(snoStringList, "SetPrefix");

        pos = listGBSetBonusesSorted.GetTailPosition();
        while (pos)
        {
            GBHandle & hGB = listGBSetBonusesSorted.GetPrev(pos);

            const GameBalanceDefinition * pdefGBItems;
            SetItemBonusTableEntry * pdefSetItemBonus = (SetItemBonusTableEntry *)GBLockR(hGB, pdefGBItems, TRUE);
            CRefString usBonus;
            usBonus += LocalizedSprintf(usSetPrefix.str(), UnicodeText::FormatNumber(pdefSetItemBonus->nRequired, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
            usBonus += "\n";

            AttribGroupID idAttribGroupMin = FastAttribGroupCreate(FAT_ITEM, -1);

            AttribGroupID idAttribGroupMax = FastAttribGroupCreate(FAT_ITEM, -1);

            AttribGroupID idAttribGroupItemPassivesMin = FastAttribGroupCreate(FAT_ITEM, -1);

            for (int32 i = 0; i < MAX_SET_ITEM_BONUS_ATTRIBUTES; i++)
            {
                const AttributeSpecifier & tAttribute = pdefSetItemBonus->ptSetItemBonusAttributes[i];
                if (tAttribute.eAttribute == ATTRIBUTE_NONE)
                {
                    continue;
                }

                int32 nAttributeParam = tAttribute.nParam;
                FastAttribKey tKey = MakeAttribKey(tAttribute.eAttribute, nAttributeParam);
                AttributeDataType eAttribType = KeyGetDataType(tKey);
                if (eAttribType == ATTRIBUTEDT_UNKNOWN)
                {
                    continue;
                }

                float flMinAffixValue = AffixGetFormulaMinValue(tAttribute, NULL, GBID_NONE, &tRNG, NULL);
                float flMaxAffixValue = AffixGetFormulaMaxValue(tAttribute, NULL, GBID_NONE, &tRNG, NULL);

                if (flMinAffixValue < FLT_EPSILON && flMaxAffixValue < FLT_EPSILON)
                {
                    continue;
                }

                if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                {
                    FastAttribSetValueFloat(idAttribGroupItemPassivesMin, tKey, flMinAffixValue);
                }
                else
                {
                    FastAttribSetValueFloat(idAttribGroupMin, tKey, flMinAffixValue);
                    FastAttribSetValueFloat(idAttribGroupMax, tKey, flMaxAffixValue);
                }
            }

            usBonus += ItemGetAttributeTip("    {icon:bullet} ", idAttribGroupMin, idAttribGroupMax, TTVC_NORMAL_COLORING);

            usBonus += ItemGetPowerBonusAttributeDescriptions("    {icon:bullet} ", &tACDItem, idAttribGroupMin, -1, ePlayerClass, TTVC_NO_COLORING, NULL, NULL);

            FastAttribGroupDelete(idAttribGroupMin);
            FastAttribGroupDelete(idAttribGroupMax);

            for (int32 i = 0; i < MAX_SET_ITEM_BONUS_ATTRIBUTES; i++)
            {
                const AttributeSpecifier & tAttribute = pdefSetItemBonus->ptSetItemBonusAttributes[i];
                if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                {
                    CRefString sStringListLabel = AttribToString(ATTRIBUTE_ITEM_POWER_PASSIVE);

                    usBonus += "    {icon:bullet} ";

                    StringLabelHandle hLabel = StringListGetLabelHandle(SNOToString(SNOName(POWER_SNO, tAttribute.nParam), FALSE).str());

                    CRefString usAttributeFormatSpecifier;
                    if (!StringListFetchByLabel(snoItemPassivePowerStringList, hLabel, usAttributeFormatSpecifier))
                    {
                        CRefString usErrorTip;
                        usErrorTip.Format("Missing item passive power label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoItemPassivePowerStringList, FALSE).str(), hLabel, sStringListLabel.str());
                        usBonus += usErrorTip;
                    }
                    else
                    {
                        usBonus += UIToolTips_ProcessTip(&tACDItem, idAttribGroupItemPassivesMin, -1, tAttribute.nParam, 1, usAttributeFormatSpecifier, TTVC_NO_COLORING, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)tAttribute.eAttribute, NULL);

                        if (tAttribute.nParam != SNO_NONE)
                        {
                            PlayerClass eClassRestriction = PowerGetClassRestriction(AutoPowerDefinitionR(tAttribute.nParam));
                            if (eClassRestriction != PLAYER_CLASS_NONE && eClassRestriction != ePlayerClass)
                            {
                                CRefString sClassName = PlayerGetClassUIName(eClassRestriction, TRUE);
                                CRefString sRestriction = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS), "ClassRestriction").str(),
                                                                           sClassName.str(), NULL);
                                Set_Text_Color(sRestriction, FONTCOLOR_RED);
                                usBonus += " ";
                                usBonus += sRestriction;
                            }
                        }

                        usBonus += "\n";
                    }
                }
            }

            FastAttribGroupDelete(idAttribGroupItemPassivesMin);

            if (pdefSetItemBonus->nRequired <= nCount)
            {
                Set_Text_Color(usBonus, FONTCOLOR_GREEN);
            }
            else
            {
                Set_Text_Color(usBonus, FONTCOLOR_GREY);
            }

            usToolTip += usBonus;
            GBUnlockR(pdefGBItems);
        }
    }

    return usToolTip;
}

// 0x71005c7500  size 756  (UIInventory.cpp:565)
CRefString GetSlotTooltip(GBID gbidItem)
{
    static const InventorySlotType sgePaperDollSlots[] =
    {
        SLOT_PLAYER_HEAD,
        SLOT_PLAYER_TORSO,
        SLOT_PLAYER_LEFT_HAND,
        SLOT_PLAYER_RIGHT_HAND,
        SLOT_PLAYER_HANDS,
        SLOT_PLAYER_WAIST,
        SLOT_PLAYER_FEET,
        SLOT_PLAYER_SHOULDERS,
        SLOT_PLAYER_LEGS,
        SLOT_PLAYER_BRACERS,
        SLOT_PLAYER_NECK,
        SLOT_PLAYER_LEFT_FINGER,
        SLOT_PLAYER_RIGHT_FINGER,
        SLOT_HIRELING_LEFT_HAND,
        SLOT_HIRELING_RIGHT_HAND,
        SLOT_HIRELING_SPECIAL,
        SLOT_HIRELING_LEFT_FINGER,
        SLOT_HIRELING_RIGHT_FINGER,
        SLOT_HIRELING_NECK,
    };

    InventorySlotType eSlotToUse = SLOT_NONE;
    int32 nNumPaperDollSlots = sizeof(sgePaperDollSlots) / sizeof(sgePaperDollSlots[0]);

    for (int32 i = 0; i < nNumPaperDollSlots; i++)
    {
        if (ACDInventoryItemAllowedInThisSlotType(gbidItem, sgePaperDollSlots[i]))
        {
            eSlotToUse = sgePaperDollSlots[i];
            break;
        }
    }

    if (eSlotToUse != SLOT_NONE)
    {
        BOOL fIs2Handed = ItemHasLabel(gbidItem, ITL_2_HAND);
        CRefString sSlotLabel = UIInventory::ItemGetLabelForSlot(eSlotToUse, fIs2Handed);

        if (sSlotLabel.Length())
        {
            return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sSlotLabel);
        }
    }

    return "";
}

// 0x71005c7800  size 592  (UIInventory.cpp:620)
void UIInventory::GetHighestTypeEffectFromSocket(const GBID gbidItemToSocket, const GBHandle hGBItemSocketedInto, GBHandle & hGBHighestTypeEffect)
{
    POS pos;
    GBHandleList lstEffects(GBGetHandlePool());
    GBEnumerate(GB_SOCKETED_EFFECTS, lstEffects);

    int32 nMostParents = -2;
    const GameBalanceDefinition * pdefGB;

    pos = lstEffects.GetHeadPosition();

    hGBHighestTypeEffect = cghgbNone;
    while (pos)
    {
        const GBHandle & hGB = lstEffects.GetNext(pos);
        SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGB, pdefGB, TRUE);

        if (ptEffectDef->gbidItemBeingSocketed == gbidItemToSocket &&
            (ptEffectDef->gbidItemTypeSocketedInto == StringToGBID("All") ||
             ItemIsOfType(hGBItemSocketedInto, ptEffectDef->gbidItemTypeSocketedInto)))
        {
            int32 nNumParentsOfType = -1;

            if (ptEffectDef->gbidItemTypeSocketedInto != StringToGBID("All"))
            {
                const ItemType * ptItemType = ItemsGetItemType(ptEffectDef->gbidItemTypeSocketedInto);

                while (ptItemType)
                {
                    nNumParentsOfType++;
                    ptItemType = ptItemType->ptParentType;
                }
            }

            if (nNumParentsOfType > nMostParents)
            {
                hGBHighestTypeEffect = hGB;
                nMostParents = nNumParentsOfType;
            }
        }

        GBUnlockR(pdefGB);
    }
}

// 0x71005c7a50  size 696  (UIInventory.cpp:671)
CRefString sGetAttributeStringFromSocket(const ActorCommonData & tACDSocketItem, const ActorCommonData & tACDParent)
{
    if (ItemIsJewel(tACDSocketItem))
    {
        CRefString usTooltip;

        if (ItemIsUpgradeableJewel(tACDSocketItem))
        {
            CRefString usJewelName = ItemGetDisplayName(tACDSocketItem, TRUE, FALSE, FALSE, TRUE);

            int32 nJewelRank = tACDSocketItem.AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
            ScopedItemDefinitionLockR pdefItem(tACDSocketItem.hGB.gbid);
            CRefString usNumRank = UnicodeText::FormatNumber(nJewelRank, TRUE, FNS_NORMAL, XLOCALE_INVALID);
            CRefString usNumMaxRank = UnicodeText::FormatNumber(pdefItem->nJewelMaxRank, TRUE, FNS_NORMAL, XLOCALE_INVALID);

            LPCTSTR szRankLabel = "UpgradeableJewelSocketedName";
            if (pdefItem && pdefItem->nJewelMaxRank >= 0 && nJewelRank < pdefItem->nJewelMaxRank)
            {
                szRankLabel = "UpgradeableJewelSocketedNameHasMax";
            }
            else if (pdefItem && pdefItem->nJewelMaxRank >= 0 && nJewelRank >= pdefItem->nJewelMaxRank)
            {
                szRankLabel = "UpgradeableJewelSocketedNameMax";
            }

            usTooltip += LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), szRankLabel), usJewelName.str(), usNumRank.str(), usNumMaxRank.str(), NULL);

            usTooltip += "\n";
        }

        usTooltip += ItemGetAttributeDescriptions(tACDSocketItem, PLAYER_CLASS_NONE, InputCtrlKeyIsDown(INPUTCONTEXT_DEFAULT), TRUE, NULL, NULL, TRUE, ACDID_INVALID);
        return usTooltip;
    }

    if (ItemIsGem(tACDSocketItem))
    {
        GBHandle hGBHighestTypeEffect;
        UIInventory::GetHighestTypeEffectFromSocket(tACDSocketItem.hGB.gbid,
                                                    tACDParent.hGB,
                                                    hGBHighestTypeEffect);

        if (hGBHighestTypeEffect.gbid != GBID_NONE)
        {
            CRefString strAttrib;
            AttribGroupID idAttribGroup = FastAttribGroupCreate(FAT_ITEM, -1);
            UIInventory::GetAttributesFromSocketEffect(hGBHighestTypeEffect, tACDParent.hGB.gbid, idAttribGroup, &strAttrib);
            FastAttribGroupDelete(idAttribGroup);
            return strAttrib;
        }
    }

    CRefString usError("Error ! Socket Not Found!");
    return usError;
}

// 0x71005c7fc0  size 640  (UIInventory.cpp:735)
BOOL UIInventory::PopulateSocketData(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    SocketCallbackData & tData = *(SocketCallbackData *)pCallBackData;
    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);

    int32 nMaxSockets = tACDContainer.AttributesGetInt(ATTRIBUTE_SOCKETS);
    int32 nIndex = tInvLoc.pt.y;
    if (nIndex >= nMaxSockets)
    {
        return FALSE;
    }

    if (ItemHasEnchantedAffix(tACDContainer))
    {
        GBID gbidEnchantedAffixNew = ItemGetEnchantedAffixNew(tACDContainer);
        int32 nEnchantedSockets = ItemAffixGetNumSockets(tACDContainer.hGB.gbid, gbidEnchantedAffixNew);
        if (nIndex >= nMaxSockets - nEnchantedSockets)
        {
            tData.ptParams->sSocketStats[nIndex] = "{icon:enchanted} ";
        }
    }

    ACDID idACDItem = ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem)
    {
        tData.ptParams->sSocketGBIDs[nIndex] = ptACDItem->hGB.gbid;

        if (GameActiveGameCommonDataIsClient())
        {
            ACDInventoryGetItemDrawInfo(*ptACDItem, tData.ptParams->tSocketImageFrames[nIndex], ITEMDRAW_SOCKETED,
                                        tData.ptTooltipPlayerData->ePlayerClass, tData.ptTooltipPlayerData->fIsMale);
        }

        tData.ptParams->sSocketStats[nIndex] += sGetAttributeStringFromSocket(*ptACDItem, tACDContainer);
    }
    else
    {
        tData.ptParams->sSocketGBIDs[nIndex] = GBID_NONE;

        if (GameActiveGameCommonDataIsClient())
        {
            tData.ptParams->tSocketImageFrames[nIndex] = cgtImageTextureFrameNull;
        }

        tData.ptParams->sSocketStats[nIndex] += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), guszItemLabelEmptySocket);
    }

    return TRUE;
}

// 0x71005c7d10  size 684  (UIInventory.cpp:791)
void UIInventory::GetAttributesFromSocketEffect(GBHandle hGBEffect, GBID gbidItem, AttribGroupID idAttribGroup, CRefString * pstrOut)
{
    if (pstrOut)
    {
        *pstrOut = "{icon:bullet} ";
    }

    const GameBalanceDefinition * pdefGB;
    SocketedEffectsTableEntry * ptEnchantment = (SocketedEffectsTableEntry *)GBLockR(hGBEffect, pdefGB, TRUE);
    if (!ptEnchantment)
    {
        return;
    }

    RNG tRNG;
    RandSetSeed_Internal(tRNG, 0);

    SNO snoItemPassivePowerStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS);

    for (uint32 i = 0; i < 3; i++)
    {
        const AttributeSpecifier & tAttribute = ptEnchantment->ptAttributes[i];
        if (tAttribute.eAttribute == ATTRIBUTE_NONE)
        {
            continue;
        }

        int32 nAttributeParam = tAttribute.nParam;
        FastAttribKey tKey = MakeAttribKey(tAttribute.eAttribute, nAttributeParam);
        AttributeDataType eAttribType = KeyGetDataType(tKey);
        if (eAttribType == ATTRIBUTEDT_UNKNOWN)
        {
            continue;
        }

        float flAffixValue = AffixGetFormulaValue(tAttribute.pbAttributeFormula,
                                                  tAttribute.serAttributeFormula.dwSize,
                                                  NULL, gbidItem, &tRNG, 0, 0);

        if (flAffixValue < FLT_EPSILON)
        {
            continue;
        }

        FastAttribSetValueFloat(idAttribGroup, tKey, flAffixValue);

        if (pstrOut && tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
        {
            CRefString sStringListLabel = AttribToString(ATTRIBUTE_ITEM_POWER_PASSIVE);
            StringLabelHandle hLabel = StringListGetLabelHandle(SNOToString(SNOName(POWER_SNO, tAttribute.nParam), FALSE).str());

            CRefString usAttributeFormatSpecifier;
            if (!StringListFetchByLabel(snoItemPassivePowerStringList, hLabel, usAttributeFormatSpecifier))
            {
                CRefString usErrorTip;
                usErrorTip.Format("Missing item passive power label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoItemPassivePowerStringList, FALSE).str(), hLabel, sStringListLabel.str());
                *pstrOut += usErrorTip;
            }
            else
            {
                *pstrOut += UIToolTips_ProcessTip(NULL, idAttribGroup, -1, tAttribute.nParam, 1, usAttributeFormatSpecifier, TTVC_NORMAL_COLORING, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)tAttribute.eAttribute, NULL);
            }
        }
    }

    if (pstrOut)
    {
        *pstrOut += ItemGetAttributeTip("", idAttribGroup, -1, TTVC_NORMAL_COLORING);
    }

    GBUnlockR(pdefGB);
}
