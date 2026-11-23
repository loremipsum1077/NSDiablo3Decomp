// Original file: X/Common/VisualInventory.cpp
//
// The effect-type and hair-style tables are not written yet; descriptor definitions follow below.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "VisualInventory.h"
#line 7
#include "ActorCommonData.h"
#include "ActorInventory.h"
#include "Game.h"
#include "GameCommonData.h"
#include "GameSwig.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Items.h"
#include "Protobuf/HeroCommon.pb.h"
#include "Definitions/GlobalDefinition.h"
#include "Player.h"
#include "ItemsSwig.h"
#include "Definitions/AttachmentDefinition.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/Definitions.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/Tags.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Math/MathGeneral.h"

enum {
    INITIAL_VISUAL_INVENTORY_POOL_COUNT = 16,
};

struct VisualInventoryGlobals {  // size 0x30
    ChainedFixedBlockAllocator poolVisualInventory;  // 0x00
};
static_assert(sizeof(VisualInventoryGlobals) == 0x30, "VisualInventoryGlobals");

// Defined in X/Common/Definitions/AttachmentDefinition.cpp (DEFINE_HARDPOINT_NAME).
extern const HardpointHash SHEATH_LEFT_HARDPOINT_NAME;
extern const HardpointHash SHEATH_RIGHT_HARDPOINT_NAME;
extern const HardpointHash SHEATH_LEFT_HIP_HARDPOINT_NAME;
extern const HardpointHash SHEATH_RIGHT_HIP_HARDPOINT_NAME;
extern const HardpointHash SHEATH_SHIELD_HARDPOINT_NAME;

BOOL ItemIsHalfOfSwordFistComboForMonk(GBID gbidItem);  // Items.cpp:1202
BOOL ItemIsLegendary(GBID gbidItem);  // Items.cpp:5080
BOOL ItemIsBroken(const ActorCommonData & tACDItem);  // Items.cpp:8672
void ACDUpdateAppearance(ACDID idACD);  // X/Client/CActorCommonData.cpp:81
SavedDyeType DyeTypeTranslateToSavedDyeType(const DyeType eDye);  // Definitions/GlobalDefinition.cpp
BOOL ItemIsEquippedButDisabled(const ActorCommonData & tACDItem);  // Items.cpp:8690
BOOL ACDInventoryValidateSlotForACD(const ActorCommonData & tACD, const InventorySlotType eSlot);  // ActorInventory.cpp:2156
void VisualInventorySetItemInSlot(const ACDID idACD, const VisualInventorySlot eSlot, const GBID gbidItem, const DyeType eDyeType, const ItemEffectType eItemEffectType, const int32 nEffectLevel, const BOOL bItemIsBrokenOrDisabled);
void VisualInventoryResetSlot(const ACDID idACD, const InventorySlotType eInventorySlot);
void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eItemEffectType, int32 & nEffectLevel, const BOOL bWantOverrides, const BOOL bForTooltip);



























































void VisualInventoryInitializeForGame() {
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    VisualInventoryGlobals *& ptVisualInventoryGlobals = GameGetActiveGameCommonData().ptVisualInventoryGlobals;

    ptVisualInventoryGlobals = (VisualInventoryGlobals *)SigmaMemoryNew(sizeof(VisualInventoryGlobals), 4, ptMemoryInterface, TRUE);
    ptVisualInventoryGlobals->poolVisualInventory.Initialize(sizeof(VisualInventory), INITIAL_VISUAL_INVENTORY_POOL_COUNT, ptMemoryInterface);
    ptVisualInventoryGlobals->poolVisualInventory.SetBlockCount_Tracked(INITIAL_VISUAL_INVENTORY_POOL_COUNT);
    ptVisualInventoryGlobals->poolVisualInventory.SetGrow(XALLOC_GROW_LINEAR, INITIAL_VISUAL_INVENTORY_POOL_COUNT);
}










inline void VisualItem::Export(D3::Hero::VisualItem & tItem) const {
    if (gbidItem != GBID_NONE)
        tItem.set_gbid(gbidItem);
    SavedDyeType eSavedDyeType = DyeTypeTranslateToSavedDyeType(eDyeType);
    if (eSavedDyeType != SAVED_DYE_TYPE_NONE)
        tItem.set_dye_type(eSavedDyeType);
    if (eItemEffectType != ITEM_EFFECT_TYPE_NONE)
        tItem.set_item_effect_type(eItemEffectType);
    if (nEffectLevel != -1)
        tItem.set_effect_level(nEffectLevel);
}














inline void VisualCosmeticItem::Export(D3::Hero::VisualCosmeticItem & tItem) const {
    if (gbidItem != GBID_NONE)
        tItem.set_gbid(gbidItem);
}








void VisualInventoryDisposeForGame() {
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    VisualInventoryGlobals *& ptVisualInventoryGlobals = GameGetActiveGameCommonData().ptVisualInventoryGlobals;

    if (ptVisualInventoryGlobals) {
        ptVisualInventoryGlobals->poolVisualInventory.Dispose();
        SigmaMemoryFree(ptVisualInventoryGlobals, ptMemoryInterface);
        ptVisualInventoryGlobals = NULL;
    }
}





static VisualInventoryType sGetVisualInventoryType(const ActorCommonData & tACD) {
    ScopedDefinitionLockR<ActorDefinition> pdefActor(tACD.snoActor);
    if (pdefActor->eType == ACTORTYPE_PLAYER)
        return VIT_PLAYER;
    if (tACD.IsMirrorImage())
        return VIT_PLAYER;
    BOOL bMerchant = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_NPC_TRADE, tACD.snoActor);
    if (ACDIsHireling(tACD) || (tACD.HasInventory() && !bMerchant))
        return VIT_HIRELING;
    return VIT_INVALID;
}















static VisualInventory * sAllocVisualInventory() {
    VisualInventory * ptVisualInventory =
        (VisualInventory *)GameGetActiveGameCommonData().ptVisualInventoryGlobals->poolVisualInventory.Alloc_Tracked();
    SigmaMemoryClear(ptVisualInventory, sizeof(VisualInventory));
    return ptVisualInventory;
}









void ACDVisualInventoryInitialize(ActorCommonData & tACD) {
    if (sGetVisualInventoryType(tACD) != VIT_INVALID) {
        tACD.ptVisualInventory = sAllocVisualInventory();
        tACD.ptVisualInventory->Init();
    } else {
        tACD.ptVisualInventory = NULL;
    }
}










static void sFreeVisualInventory(VisualInventory *& ptVisualInventory) {
    GameGetActiveGameCommonData().ptVisualInventoryGlobals->poolVisualInventory.Free(ptVisualInventory);
    ptVisualInventory = NULL;
}








void ACDVisualInventoryDispose(VisualInventory *& ptVisualInventory) {
    if (ptVisualInventory == NULL)
        return;
    sFreeVisualInventory(ptVisualInventory);
}









VisualInventorySlot VisualInventorySlotFromInventorySlot(const InventorySlotType eInventorySlot) {
    switch (eInventorySlot) {
    case SLOT_PLAYER_HEAD: return VIS_HEAD;
    case SLOT_PLAYER_TORSO: return VIS_TORSO;
    case SLOT_PLAYER_RIGHT_HAND: return VIS_RIGHT_HAND;
    case SLOT_PLAYER_LEFT_HAND: return VIS_LEFT_HAND;
    case SLOT_PLAYER_HANDS: return VIS_HANDS;
    case SLOT_PLAYER_FEET: return VIS_FEET;
    case SLOT_PLAYER_SHOULDERS: return VIS_SHOULDERS;
    case SLOT_PLAYER_LEGS: return VIS_LEGS;
    case SLOT_HIRELING_RIGHT_HAND: return VIS_RIGHT_HAND;
    case SLOT_HIRELING_LEFT_HAND: return VIS_LEFT_HAND;
    }
    return VIS_INVALID;
}





























InventorySlotType VisualInventorySlotToInventorySlot(const VisualInventorySlot eVisualInventorySlot) {
    switch (eVisualInventorySlot) {
    case VIS_HEAD: return SLOT_PLAYER_HEAD;
    case VIS_TORSO: return SLOT_PLAYER_TORSO;
    case VIS_FEET: return SLOT_PLAYER_FEET;
    case VIS_HANDS: return SLOT_PLAYER_HANDS;
    case VIS_LEFT_HAND: return SLOT_PLAYER_LEFT_HAND;
    case VIS_RIGHT_HAND: return SLOT_PLAYER_RIGHT_HAND;
    case VIS_SHOULDERS: return SLOT_PLAYER_SHOULDERS;
    case VIS_LEGS: return SLOT_PLAYER_LEGS;
    }
    return SLOT_NONE;
}







VisualInventory * VisualInventoryGet(const ACDID idACD) {
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return NULL;
    return ptACD->ptVisualInventory;
}








inline void VisualItem::SetDefaults() {
    Set(GBID_NONE, 0, 0, 0);
}





void VisualEquipment::Export(D3::Hero::VisualEquipment & tEquipmentList) const {
    tEquipmentList.Clear();
    for (int32 i = 0; i < VIS_NUM_SLOTS; i++) {
        tVisualItem[i].Export(*tEquipmentList.add_visual_item());
    }
    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++) {
        tCosmeticItems[i].Export(*tEquipmentList.add_cosmetic_item());
    }
}







void VisualEquipment::Import(const D3::Hero::VisualEquipment & tEquipmentList) {
    for (int32 i = 0; i < VIS_NUM_SLOTS && i < tEquipmentList.visual_item_size(); i++) {
        tVisualItem[i].Import(tEquipmentList.visual_item(i));
    }
    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++) {
        if (i < tEquipmentList.cosmetic_item_size())
            tCosmeticItems[i].Import(tEquipmentList.cosmetic_item(i));
        else
            tCosmeticItems[i].gbidItem = GBID_NONE;
    }
}











BOOL VisualEquipment::operator==(const VisualEquipment & tVisualEquipment) const {
    for (int32 i = 0; i < VIS_NUM_SLOTS; i++) {
        if (tVisualItem[i] != tVisualEquipment.tVisualItem[i])
            return FALSE;
    }
    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++) {
        if (tCosmeticItems[i] != tVisualEquipment.tCosmeticItems[i])
            return FALSE;
    }
    return TRUE;
}












void VisualEquipment::Init() {
    for (int32 i = 0; i < VIS_NUM_SLOTS; i++) {
        tVisualItem[i].SetDefaults();
    }
    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++) {
        tCosmeticItems[i].SetDefaults();
    }
}









void VisualInventory::Init() {
    tEquipmentCurrent.Init();
    InitializeLast();
}








void VisualInventory::InitializeLast() {
    for (int32 i = 0; i < VIS_NUM_SLOTS; i++) {
        VisualItem & tVisualItem = tEquipmentLast.tVisualItem[i];
        tVisualItem.Set(GBID_NONE, 0, -1, -1);
    }
    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++) {
        tEquipmentLast.tCosmeticItems[i].gbidItem = GBID_NONE;
    }
}











void VisualInventorySetItemInSlot(const ACDID idACD, const VisualInventorySlot eSlot, const GBID gbidItem, const DyeType eDyeType, const ItemEffectType eItemEffectType, const int32 nEffectLevel, const BOOL bItemIsBrokenOrDisabled) {
    GBID gbidItemToUse = gbidItem;
    ItemEffectType eItemEffectTypeToUse = eItemEffectType;
    int32 nEffectLevelToUse = nEffectLevel;
    VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
    if (ptVisualInventory == NULL)
        return;
    if (bItemIsBrokenOrDisabled)
        gbidItemToUse = GBID_NONE;
    if (ItemIsLegendary(gbidItem)) {
        nEffectLevelToUse = -1;
        eItemEffectTypeToUse = ITEM_EFFECT_TYPE_NONE;
    }
    VisualEquipment & tEquipmentCurrent = ptVisualInventory->tEquipmentCurrent;
    VisualItem & tVisualItem = tEquipmentCurrent.tVisualItem[eSlot];
    if (tVisualItem.gbidItem == gbidItemToUse &&
        tVisualItem.eItemEffectType == eItemEffectTypeToUse &&
        tVisualItem.nEffectLevel == nEffectLevelToUse &&
        tVisualItem.eDyeType == eDyeType)
        return;
    tVisualItem.Set(gbidItemToUse, eDyeType, eItemEffectTypeToUse, nEffectLevelToUse);
    if (GameActiveGameCommonDataIsServer()) {
        ActorCommonData & tACD = ACDGet(idACD);
        tACD.ModifiedMark(ACDMODIFIED_VISUAL_INVENTORY);
    } else {
        ACDUpdateAppearance(idACD);
    }
}






























inline void VisualInventorySetItemInSlot(const ACDID idACD, const VisualInventorySlot eSlot, const VisualItem & tItem, const BOOL bItemIsBrokenOrDisabled) {
    VisualInventorySetItemInSlot(idACD, eSlot, tItem.gbidItem, tItem.eDyeType, tItem.eItemEffectType, tItem.nEffectLevel, bItemIsBrokenOrDisabled);
}








void VisualInventorySetCosmeticItemInSlot(const ACDID idACD, const VisualCosmeticItemSlot eSlot, const GBID gbidItem) {
    GBID gbidItemToUse = gbidItem;
    VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
    if (ptVisualInventory == NULL)
        return;
    VisualEquipment & tEquipmentCurrent = ptVisualInventory->tEquipmentCurrent;
    VisualCosmeticItem & tItem = tEquipmentCurrent.tCosmeticItems[eSlot];
    if (tItem.gbidItem == gbidItemToUse)
        return;
    tItem.Set(gbidItemToUse);
    if (GameActiveGameCommonDataIsServer()) {
        ACDGet(idACD).ModifiedMark(ACDMODIFIED_VISUAL_INVENTORY);
    } else {
        ACDUpdateAppearance(idACD);
    }
}




















GBID VisualInventoryGetCosmeticItemInSlot(const ACDID idACD, const VisualCosmeticItemSlot eSlot) {
    VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
    if (ptVisualInventory == NULL)
        return GBID_NONE;
    VisualEquipment & tEquipmentCurrent = ptVisualInventory->tEquipmentCurrent;
    VisualCosmeticItem & tItem = tEquipmentCurrent.tCosmeticItems[eSlot];
    return tItem.gbidItem;
}












void VisualInventoryResetSlot(const ACDID idACD, const InventorySlotType eInventorySlot) {
    VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eInventorySlot);
    if (eVisualSlot == VIS_INVALID)
        return;
    ActorCommonData & tACD = ACDGet(idACD);
    if (!ACDInventoryValidateSlotForACD(tACD, eInventorySlot))
        return;
    ACDID idACDItem = ACDInventoryGetSquareItem(tACD, eInventorySlot, cgptNull);
    GBID gbidItem = GBID_NONE;
    DyeType eDyeType = DYE_TYPE_NONE;
    ItemEffectType eItemEffectType = ITEM_EFFECT_TYPE_NONE;
    int32 nEffectLevel = -1;
    BOOL bIsBrokenOrDisabled = FALSE;
    if (idACDItem != ACDID_INVALID) {
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem) {
            gbidItem = ptACDItem->GetVisualGBID();
            ACDGetItemEffectTypeAndLevel(*ptACDItem, eItemEffectType, nEffectLevel, TRUE, FALSE);
            eDyeType = ptACDItem->GetDyeType();
            bIsBrokenOrDisabled = ItemIsBroken(*ptACDItem) || ItemIsEquippedButDisabled(*ptACDItem);
        }
    }
    VisualInventorySetItemInSlot(idACD, eVisualSlot, gbidItem, eDyeType, eItemEffectType, nEffectLevel, bIsBrokenOrDisabled);
}



















void VisualInventoryResetAllSlots(const ACDID idACD) {
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return;
    if (ACDIsPlayer(idACD)) {
        for (int32 i = SLOT_PLAYER_START; i < SLOT_PLAYER_END; i++) {
            InventorySlotType eSlot = i;
            VisualInventoryResetSlot(idACD, eSlot);
        }
    } else if (ACDIsHireling(*ptACD)) {
        for (int32 i = SLOT_HIRELING_START; i < SLOT_HIRELING_END; i++) {
            InventorySlotType eSlot = i;
            VisualInventoryResetSlot(idACD, eSlot);
        }
    }
}



































































































































































SNO GetEffectGroupSNOForActorAndItemEffectType(const SNO snoActor, const ItemEffectType eItemEffectType) {
    ActorTag dwEffectTag = TAG_ACTOR_INVALID;
    switch (eItemEffectType) {
    case ITEM_EFFECT_TYPE_LIGHTNING:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_LIGHTNING_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_COLD:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_ICE_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_FIRE:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_FIRE_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_POISON:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_POISON_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_ARCANE:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_ARCANE_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_WITCHDOCTOR_DAMAGE:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_WITCHDOCTOR_DAMAGE_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_LIFESTEAL:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_LIFESTEAL_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_MANASTEAL:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_MANASTEAL_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_MAGIC_FIND:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_MAGIC_FIND_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_GOLD_FIND:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_GOLD_FIND_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_ATTACK_SPEED_BONUS:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_ATTACK_SPEED_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_CAST_SPEED_BONUS:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_CAST_SPEED_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_HOLY:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_HOLY_EFFECTGROUP;
        break;
    case ITEM_EFFECT_TYPE_WIZARD_DAMAGE:
        dwEffectTag = TAG_ACTOR_EFFECT_WEAPON_WIZARD_DAMAGE_EFFECTGROUP;
        break;
    }
    SNO snoEffectGroup = SNO_NONE;
    ScopedActorDefinitionLockR ptActorDef(snoActor);
    if (ptActorDef)
        snoEffectGroup = TagMapGetSNO(ptActorDef->hTagMap, dwEffectTag);
    return snoEffectGroup;
}































































































// Defined in X/Common/Definitions/AttachmentDefinition.cpp (DEFINE_HARDPOINT_NAME).
extern const HardpointHash LEFT_WEAPON_HARDPOINT_NAME, RIGHT_WEAPON_HARDPOINT_NAME;
extern const HardpointHash LEFT_FIST_HARDPOINT_NAME, RIGHT_FIST_HARDPOINT_NAME;
extern const HardpointHash SHIELD_HARDPOINT_NAME, HELM_HARDPOINT_NAME, LEFT_SHOULDER_HARDPOINT_NAME;
BOOL VisualInventoryGetAttachmentPoint(
    const GBID gbidItem,
    const SNO snoActorItem,
    const VisualInventorySlot eVisSlot,
    const BOOL fUseSheath,
    Hardpoint & tHardPoint)
{
    GBHandle hGB(GB_ITEMS, gbidItem);


    if (ItemHasLabel(gbidItem, ITL_QUIVER))
    {
        return FALSE;
    }

    if (fUseSheath)
    {
        if (ItemHasLabel(gbidItem, ITL_SHIELD))
        {
            tHardPoint.tHash = SHEATH_SHIELD_HARDPOINT_NAME;
            return TRUE;
        }
        else if (ItemHasLabel(gbidItem, ITL_ORB) || ItemHasLabel(gbidItem, ITL_MOJO) || ItemHasLabel(gbidItem, ITL_OFFHAND_NECROMANCER))
        {
            tHardPoint.tHash = (uint32)0;
            return TRUE;
        }
        else
        {
            BOOL fUseHip = ItemHasLabel(gbidItem, ITL_1_HAND) && !ItemHasLabel(gbidItem, ITL_SPEAR) &&
                !ItemHasLabel(gbidItem, ITL_HAND_CROSSBOW);

            switch (eVisSlot)
            {
            case VIS_RIGHT_HAND:
                {
                    tHardPoint.tHash = fUseHip ? SHEATH_LEFT_HIP_HARDPOINT_NAME : SHEATH_LEFT_HARDPOINT_NAME;
                    return TRUE;
                }
            case VIS_LEFT_HAND:
                {
                    tHardPoint.tHash = fUseHip ? SHEATH_RIGHT_HIP_HARDPOINT_NAME : SHEATH_RIGHT_HARDPOINT_NAME;
                    return TRUE;
                }
            }
        }
    }


    WeaponClass eWeaponClass = (WeaponClass)ActorDefinitionGetTagMapValueInt(TAG_ACTOR_WEAPON_CLASS, snoActorItem);
    switch (eWeaponClass)
    {
    case WEAPONCLASS_2HSWING:
    case WEAPONCLASS_2HTHRUST:
    case WEAPONCLASS_XBOW:
    case WEAPONCLASS_2H_AXE_MACE:
    case WEAPONCLASS_2H_FLAIL:
        {
            tHardPoint.tHash = RIGHT_WEAPON_HARDPOINT_NAME;
            return TRUE;
        }

    case WEAPONCLASS_BOW:
        {
            tHardPoint.tHash = LEFT_WEAPON_HARDPOINT_NAME;
            return TRUE;
        }

    case WEAPONCLASS_1HFIST:
        {



            switch (eVisSlot)
            {
            case VIS_RIGHT_HAND:
                {
                    tHardPoint.tHash = LEFT_FIST_HARDPOINT_NAME;
                    return TRUE;
                }
            case VIS_LEFT_HAND:
                {
                    tHardPoint.tHash = RIGHT_FIST_HARDPOINT_NAME;
                    return TRUE;
                }
            }
            break;
        }

    case WEAPONCLASS_HTH:
    case WEAPONCLASS_1HSWING:
    case WEAPONCLASS_1HTHRUST:
    case WEAPONCLASS_STAFF:
    case WEAPONCLASS_WAND:
    case WEAPONCLASS_HANDXBOW:
        {

            switch (eVisSlot)
            {
            case VIS_RIGHT_HAND:
                {
                    tHardPoint.tHash = LEFT_WEAPON_HARDPOINT_NAME;
                    return TRUE;
                }
            case VIS_LEFT_HAND:
                {
                    tHardPoint.tHash = RIGHT_WEAPON_HARDPOINT_NAME;
                    return TRUE;
                }
            }
            break;
        }
    }


    if (ItemHasLabel(gbidItem, ITL_SHIELD))
    {
        tHardPoint.tHash = SHIELD_HARDPOINT_NAME;
        return TRUE;
    }

    if (ItemHasLabel(gbidItem, ITL_OFFHAND))
    {
        tHardPoint.tHash = LEFT_WEAPON_HARDPOINT_NAME;
        return TRUE;
    }




    if (ItemHasLabel(gbidItem, ITL_ARMOR) && eVisSlot == VIS_HEAD)
    {
        tHardPoint.tHash = HELM_HARDPOINT_NAME;
        return TRUE;
    }

    if (ItemHasLabel(gbidItem, ITL_SHOULDERS))
    {
        tHardPoint.tHash = LEFT_SHOULDER_HARDPOINT_NAME;
        return TRUE;
    }

    switch (eVisSlot)
    {
    case VIS_RIGHT_HAND:
        {
            tHardPoint.tHash = SHIELD_HARDPOINT_NAME;
            return TRUE;
        }
    case VIS_LEFT_HAND:
        {
            tHardPoint.tHash = RIGHT_WEAPON_HARDPOINT_NAME;
            return TRUE;
        }
    }

    return FALSE;
}



BOOL VisualInventoryIsHardpointASheath(HardpointHash szHardpoint) {
    if (szHardpoint == SHEATH_SHIELD_HARDPOINT_NAME || szHardpoint == SHEATH_LEFT_HIP_HARDPOINT_NAME ||
        szHardpoint == SHEATH_RIGHT_HIP_HARDPOINT_NAME || szHardpoint == SHEATH_LEFT_HARDPOINT_NAME ||
        szHardpoint == SHEATH_RIGHT_HARDPOINT_NAME)
        return TRUE;
    return FALSE;
}









void VisualItem::Set(const GBID _gbidItem, const DyeType _eDyeType, const ItemEffectType _eItemEffectType, const int32 _nEffectLevel) {
    gbidItem = _gbidItem;
    eDyeType = _eDyeType;
    eItemEffectType = _eItemEffectType;
    nEffectLevel = _nEffectLevel;
}








VisualInventoryType VisualInventoryGetType(
    const ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);

    if (tACD.IsMirrorImage())
    {
        return VIT_PLAYER;
    }

    InventoryType eInventoryType = ACDInventoryGetType(tACD);
    switch (eInventoryType)
    {
    case INVENTORY_PLAYER:
        return VIT_PLAYER;
    case INVENTORY_HIRELING:
    case INVENTORY_GENERIC:
        return VIT_HIRELING;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


}




void VisualInventoryCopy(ACDID idACDSource, ACDID idACDDest) {
    VisualInventory * ptVisualInventorySource = VisualInventoryGet(idACDSource);
    if (ptVisualInventorySource == NULL)
        return;
    for (int32 i = 0; i < VIS_NUM_SLOTS; i++) {
        VisualItem & tItem = ptVisualInventorySource->tEquipmentCurrent.tVisualItem[i];
        VisualInventorySetItemInSlot(idACDDest, i, tItem, FALSE);
    }
    for (int32 i = 0; i < VCIS_NUM_SLOTS; i++) {
        VisualCosmeticItem & tItem = ptVisualInventorySource->tEquipmentCurrent.tCosmeticItems[i];
        VisualInventorySetCosmeticItemInSlot(idACDDest, i, tItem.gbidItem);
    }
}













void VisualEquipment::FixOffHandWeapons() {
    BOOL bIsOffhand = ItemHasLabel(tVisualItem[VIS_RIGHT_HAND].gbidItem, ITL_OFFHAND);
    if (tVisualItem[VIS_LEFT_HAND].gbidItem == GBID_NONE &&
        tVisualItem[VIS_RIGHT_HAND].gbidItem != GBID_NONE && !bIsOffhand) {
        tVisualItem[VIS_LEFT_HAND] = tVisualItem[VIS_RIGHT_HAND];
        tVisualItem[VIS_RIGHT_HAND].SetDefaults();
    }
}








void VisualEquipment::LimitEffects() {
    for (int32 i = 0; i < VIS_NUM_SLOTS; i++) {
        VisualItem & tItem = tVisualItem[i];
        tItem.nEffectLevel = Math::Min(tItem.nEffectLevel, 1);
    }
}







void VisualInventoryUpdateItem(ActorCommonData & tACDItem) {
    const InventoryLocation * ptLocation = tACDItem.TryToGetInventoryLocation();
    if (ptLocation == NULL || ptLocation->idACDContainer == ACDID_INVALID)
        return;
    ActorCommonData * ptACDContainer = ACDTryToGet(ptLocation->idACDContainer);
    if (ptACDContainer && ptACDContainer->HasVisualInventory()) {
        VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(ptLocation->eSlot);
        if (eVisualSlot != VIS_INVALID) {
            DyeType eDyeType = tACDItem.GetDyeType();
            ItemEffectType eItemEffectType;
            int32 nEffectLevel;
            ACDGetItemEffectTypeAndLevel(tACDItem, eItemEffectType, nEffectLevel, TRUE, FALSE);
            VisualInventorySetItemInSlot(ptLocation->idACDContainer, eVisualSlot, tACDItem.GetVisualGBID(), eDyeType, eItemEffectType, nEffectLevel, ItemIsBroken(tACDItem));
        }
    }
}










BOOL VisualInventoryShouldSwapWeapons(PlayerClass ePlayerClass, GBID gbidItemLeft, GBID gbidItemRight) {
    if (ePlayerClass == PLAYER_CLASS_MONK && ItemHasLabel(gbidItemLeft, ITL_FIST_WEAPON) &&
        ItemIsHalfOfSwordFistComboForMonk(gbidItemRight))
        return TRUE;
    return FALSE;
}


































SNO VisualInventoryGetPetSnoFromGbid(GBID gbidItem) {
    SNO snoActor = SNO_NONE;
    if (GBIDExists(gbidItem, GB_ITEMS)) {
        ScopedItemDefinitionLockR pdefPetItem(gbidItem);
        if (pdefPetItem) {
            for (int32 i = 0; i < (int32)(pdefPetItem->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++) {
                const AttributeSpecifier & tSpecifier = pdefPetItem->ptAttributes[i];
                if (tSpecifier.eAttribute == ATTRIBUTE_COSMETIC_PET_ACTOR) {
                    snoActor = tSpecifier.nParam;
                    break;
                }
            }
        }
    }
    return snoActor;
}







#include "../Client/Definitions/EffectGroupDefinition.h"
#include "../Client/Definitions/ParticleDefinition.h"
#include "../../Contrib/Contrib/blz/include/blz/list.h"


SNO VisualInventoryGetPennantSnoFromGbid(
    GBID gbidItem)
{

    if (GBIDExists(gbidItem, GB_ITEMS))
    {
        ScopedItemDefinitionLockR pdefPetItem(gbidItem);
        if (pdefPetItem &&
            pdefPetItem->snoMainEffect != SNO_NONE)
        {
            blz::list<SNO> listEffectGroups(1, pdefPetItem->snoMainEffect);
            while (listEffectGroups.size() > 0)
            {
                ScopedEffectGroupDefinitionLockR pdefEffectGroup(listEffectGroups.front());
                if (pdefEffectGroup)
                {
                    for (int32 i = 0; i < pdefEffectGroup->nEffectCount; ++i)
                    {
                        const EffectItem & tEffectItem = pdefEffectGroup->ptEffects[i];
                        switch (tEffectItem.tMsgTriggeredEvents.tTriggerEvent.eType)
                        {
                            case TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP:
                            {
                                listEffectGroups.push_back(tEffectItem.tMsgTriggeredEvents.tTriggerEvent.snoname.snoHandle);
                                break;
                            }
                            case TRIGGEREDEVENTTYPE_ADD_OBJECT:
                            {
                                switch (tEffectItem.tMsgTriggeredEvents.tTriggerEvent.eAttachmentType)
                                {
                                    case ATTACHMENTTYPE_ACTOR:
                                    {
                                        return tEffectItem.tMsgTriggeredEvents.tTriggerEvent.snoname.snoHandle;
                                    }
                                    case ATTACHMENTTYPE_PARTICLESYSTEM:
                                    {

                                        ScopedParticleDefinitionLockR pdefParticles(tEffectItem.tMsgTriggeredEvents.tTriggerEvent.snoname.snoHandle);
                                        if (pdefParticles)
                                        {
                                            return pdefParticles->snoActor;
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }




                listEffectGroups.pop_front();
            }
        }
    }


    return SNO_NONE;

}


BOOL VisualInventoryDetectCosmeticChange(const ACDID idACD, const VisualEquipment & tEquipment) {
    VisualInventory * ptVisualInventory = VisualInventoryGet(idACD);
    if (ptVisualInventory) {
        for (int32 i = 0; i < VCIS_NUM_SLOTS; i++) {
            if (ptVisualInventory->tEquipmentCurrent.tCosmeticItems[i].gbidItem != tEquipment.tCosmeticItems[i].gbidItem)
                return TRUE;
        }
    }
    return FALSE;
}

// VisualItem::Import and VisualCosmeticItem::Import are inline members of VisualInventory.h
// (lines 90 and 133); their bodies are here so that header need not include HeroCommon.pb.h.
inline void VisualItem::Import(const D3::Hero::VisualItem & tItem) {
    gbidItem = tItem.gbid();
    eDyeType = DyeTypeTranslateFromSavedDyeType(tItem.dye_type());
    eItemEffectType = tItem.item_effect_type();
    nEffectLevel = tItem.effect_level();
}

inline void VisualCosmeticItem::Import(const D3::Hero::VisualCosmeticItem & tItem) {
    gbidItem = tItem.gbid();
}




// GameBalanceDefinition.h:49 and Items.h: original DWARF declarations.
enum { GB_AFFIXES = 8 };
BOOL ItemIsSpecial(GBID gbidItem);
GBID ItemGetHighestAffixWithEffectType(const ActorCommonData & tACDItem, BOOL bForTooltip);



// Inference: early exits express original branches to the common return; effect
// descriptors use the original constructor bounds 22,14,-1,30 and registration flags128.
#line 877 "X/Common/VisualInventory.cpp"
void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eItemEffectType, int32 & nEffectLevel, const BOOL bWantOverrides, const BOOL bForTooltip)
{
    nEffectLevel = -1;
    eItemEffectType = ITEM_EFFECT_TYPE_NONE;
    if (ItemIsLegendary(tACD.hGB.gbid) || ItemIsSpecial(tACD.hGB.gbid))
        return;
#line 901
    GBID gbidHighestAffix = ItemGetHighestAffixWithEffectType(tACD, bForTooltip);
    if (gbidHighestAffix != GBID_NONE)
    {
#line 904
        const GameBalanceDefinition * pdefGBAffixes = NULL;
#line 905
        GBHandle hGB(GB_AFFIXES, gbidHighestAffix);
#line 906
        const AffixTableEntry * ptAffix = (const AffixTableEntry *)GBLockR(hGB, pdefGBAffixes, TRUE);
        if (ptAffix)
        {
            eItemEffectType = ptAffix->eItemEffectType;
            nEffectLevel = ptAffix->nItemEffectLevel;
            GBUnlockR(pdefGBAffixes);
        }
    }
    if (!ACDInventoryItemAllowedInThisSlotType(tACD.hGB.gbid, SLOT_PLAYER_RIGHT_HAND) && !ACDInventoryItemAllowedInThisSlotType(tACD.hGB.gbid, SLOT_PLAYER_LEFT_HAND))
    {
        eItemEffectType = ITEM_EFFECT_TYPE_NONE;
        nEffectLevel = -1;
        return;
    }
    if (GameActiveGameCommonDataIsClient() && bWantOverrides)
    {
#line 926
        ActorCommonData * ptACDOwner = ACDTryToGet(tACD.GetInventoryContainerOwner());
#line 927
        GBID gbidItem = tACD.GetGBHandle().gbid;
        if (ptACDOwner && ptACDOwner->AttributesAttributeExists(FastAttribKey(ATTRIBUTE_WEAPON_EFFECT_OVERRIDE)) && !ItemHasLabel(gbidItem, ITL_OFFHAND))
        {
#line 932
            ItemEffectType eOverrideItemEffect = ptACDOwner->AttributesGetInt(FastAttribKey(ATTRIBUTE_WEAPON_EFFECT_OVERRIDE));
#line 935
            SNO snoEffectGroup = GetEffectGroupSNOForActorAndItemEffectType(tACD.snoActor, eOverrideItemEffect);
            if (snoEffectGroup != SNO_NONE)
            {
#line 938
                ScopedEffectGroupDefinitionLockR ptEffectGroupDef(snoEffectGroup);
                eItemEffectType = eOverrideItemEffect;
                nEffectLevel = ptEffectGroupDef->nEffectCount - 1;
            }
        }
    }
    if (GameIsParamFlagSet(GPF_LIMIT_WEAPON_EFFECTS))
        nEffectLevel = Math::Min(nEffectLevel, 1);
}











































































































































#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
// D3DescribedType.cpp: original exported descriptor pointers.
extern const XTypeDescriptor * DT_GBID;
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;
#line 86 "X/Common/VisualInventory.cpp"
static const TypeDescriptorField VisualItem_Fields[5] = {
    TypeDescriptorField("", DT_GBID, offsetof(VisualItem, gbidItem), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VisualItem, eDyeType), NULL, NULL, (const void *)22),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VisualItem, eItemEffectType), NULL, NULL, (const void *)(NUM_ITEM_EFFECT_TYPES - 1)),
    BoundedTypeDescriptorField("", DT_INT, offsetof(VisualItem, nEffectLevel), NULL, (const void *)-1, (const void *)30),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VisualItem), NULL, 0),
};
#line 91
ComplexTypeDescriptor TypeDescriptor_VisualItem("VisualItem", 0, VisualItem_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_VISUAL_ITEM = &TypeDescriptor_VisualItem;
#line 93
static const TypeDescriptorField VisualCosmeticItem_Fields[2] = {
    TypeDescriptorField("", DT_GBID, offsetof(VisualCosmeticItem, gbidItem), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VisualCosmeticItem), NULL, 0),
};
#line 95
ComplexTypeDescriptor TypeDescriptor_VisualCosmeticItem("VisualCosmeticItem", 0, VisualCosmeticItem_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_VISUAL_COSMETIC_ITEM = &TypeDescriptor_VisualCosmeticItem;
#line 98
static const TypeDescriptorField VisualEquipment_Fields[3] = {
    FixedArrayTypeDescriptorField("", DT_VISUAL_ITEM, offsetof(VisualEquipment, tVisualItem), NULL, 1, VIS_NUM_SLOTS, 0),
    FixedArrayTypeDescriptorField("", DT_VISUAL_COSMETIC_ITEM, offsetof(VisualEquipment, tCosmeticItems), NULL, 1, VCIS_NUM_SLOTS, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VisualEquipment), NULL, 0),
};
#line 101
ComplexTypeDescriptor TypeDescriptor_VisualEquipment("VisualEquipment", 0, VisualEquipment_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_VISUAL_EQUIPMENT = &TypeDescriptor_VisualEquipment;
#line 104
static const TypeDescriptorField VisualInventoryMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(VisualInventoryMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(VisualInventoryMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_VISUAL_EQUIPMENT, offsetof(VisualInventoryMessage, tEquipment), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(VisualInventoryMessage), NULL, 0),
};
#line 108
ComplexTypeDescriptor TypeDescriptor_VisualInventoryMessage("VisualInventoryMessage", 0, VisualInventoryMessage_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_VISUAL_INVENTORY_MESSAGE = &TypeDescriptor_VisualInventoryMessage;
