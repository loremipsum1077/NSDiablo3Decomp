// Implemented from the DWARF line table (tools/line_evidence.py), the original
// disassembly and the reference decompilation
// (build/ghidra/decomp/X/Client/CVisualInventory.cpp).

#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/VisualInventory.h"
#include "RActors/RActors.h"
#include "RActors/RActorAttachments.h"

// line 40
static uint32 sGetIDForVisualInventorySlot(const VisualInventorySlot eVisSlot)
{
    switch (eVisSlot) {
    case VIS_HEAD:       return INTERNAL_ATTACHMENTID_VIS_HELM;
    case VIS_LEFT_HAND:  return INTERNAL_ATTACHMENTID_VIS_LEFT_HAND;
    case VIS_RIGHT_HAND: return INTERNAL_ATTACHMENTID_VIS_RIGHT_HAND;
    case VIS_SHOULDERS:  return INTERNAL_ATTACHMENTID_VIS_SHOULDERS;
    default:             return ATTACHMENTID_INVALID;
    }
}

// line 65
void VisualInventoryClearAttachments(RActor * ptRActor)
{
    const VisualInventorySlot tSlots[] = { VIS_HEAD, VIS_LEFT_HAND, VIS_RIGHT_HAND, VIS_SHOULDERS };

    for (int32 i = 0; i < 4; ++i) {
        uint32 dwID = sGetIDForVisualInventorySlot(tSlots[i]);
        RActorClearActorAttachment(*ptRActor, dwID);
    }

    RActorClearActorAttachment(*ptRActor, INTERNAL_ATTACHMENTID_VIS_PET);
}

// Missing visual-inventory functions recovered below; remaining compiler differences are annotated.
































#include "../Common/TagMaps/ActorTags.h"  // the CU's DWARF has the TAG_ACTOR_VISUALINV_* enumerators
#include "../Common/Player.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/TagMaps/Tags.h"
#include "../Sigma/TypeDescriptor/SymbolicValue.h"
#include "Definitions/AppearanceDefinition.h"
#include "Definitions/EffectGroupDefinition.h"
#include "Attachments.h"
#include "SubObjectGfx.h"
// Original API declarations missing from adopted headers; signatures follow original symbols.
Attachment* AttachmentGet(AttachmentID idAttachment);
AttachmentID RActorAddAttachment(RActor&, int32, SNO, uint32, HardpointHash, BOOL);
void RActorAddWeaponElementalEffects(const RActor*);
BOOL VisualInventoryIsHardpointASheath(HardpointHash);
VisualInventoryType VisualInventoryGetType(ACDID);
BOOL VisualInventoryGetAttachmentPoint(GBID, SNO, VisualInventorySlot, BOOL, Hardpoint&);
SNO VisualInventoryGetPetSnoFromGbid(GBID);
SNO GameBalanceGetActorSNO(const GBHandle&);
BOOL ItemHasLabel(GBID, int32);
BOOL GameTypeIsPvP();
int32 PlayerGetLocalTeam();
void ACDGetItemEffectTypeAndLevel(const ActorCommonData&, ItemEffectType&, int32&, BOOL, BOOL);
ACDID ACDInventoryGetPrimaryWeapon(ACDID);
SNO GetEffectGroupSNOForActorAndItemEffectType(SNO, ItemEffectType);
void RActorSetHairStyle(RActor*, HairStyle);
void LockAllSubObjectLookUpdate(RActor*);
void UnlockAllSubObjectLookUpdate(RActor*, const AppearanceDefinition*);
void RActorComponentsSetSlot(RActor*, int32, int32, int32, DyeType, BOOL);
void RActorAddCosmeticBuff(RActor*, int32, GBID, BOOL);
void RActorRemoveCosmeticBuff(RActor*, int32, GBID, BOOL);
void AppearanceGetNakedLook(SNO, int32&, int32&);
int32 AppearanceSubObjectFromLookKey(SNO, uint32);
uint32 StringHash(LPCSTR);
typedef int32 ComponentStyle; // Original appearance component-style scalar type.
extern const SymbolicEntry gComponentLooks[];
extern const HardpointHash RIGHT_SHOULDER_HARDPOINT_NAME;

#line 202 "X/Client/CVisualInventory.cpp"
static AttachmentID sCreateAttachmentForSlot(RActor* ptRActor, uint32 dwID, SNO snoActorToAttach, const VisualItem& tVisualItemNew, HardpointHash szHardpoint)
{
    AttachmentID idAttachment = ATTACHMENTID_INVALID;
    if (snoActorToAttach != SNO_NONE) {
        idAttachment = RActorAddAttachment(*ptRActor, 0, snoActorToAttach, dwID, szHardpoint, FALSE);
        if (idAttachment != ATTACHMENTID_INVALID) {
            Attachment* ptAttachment = AttachmentGet(idAttachment);
            if (ptAttachment && ptAttachment->idAttachedObject != RACTORID_INVALID) {
                RActor* ptRActorAttached = RActorGet(RActorID(ptAttachment->idAttachedObject));
                ptRActorAttached->eItemEffectType = tVisualItemNew.eItemEffectType;
                ptRActorAttached->nItemEffectLevel = tVisualItemNew.nEffectLevel;
                if (VisualInventoryIsHardpointASheath(szHardpoint)) ptRActorAttached->nItemEffectLevel = 0;
                RActorAddWeaponElementalEffects(ptRActorAttached);
                RActorGeoSetData& tGeoSetData = ptRActorAttached->tGeoSetData[0];
                DyeType eDyeType = tVisualItemNew.eDyeType;
                ScopedAppearanceDefinitionLockR pdefAppearance(ptRActorAttached->snoAppearance);
                const GeoSet& tGeoSet = pdefAppearance->tStructure.tGeoSets[0];
                for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i) {
                    SubObjectState& tSubObjectState = tGeoSetData.ptSubObjectState[i];
                    if (tSubObjectState.dwFlags & 1) tSubObjectState.eDyeType = eDyeType;
                }
            }
        }
    }
    return idAttachment;
}

#line 83 "X/Client/CVisualInventory.cpp"
static void sFindAlternateAttachmentActor(
    const SNO snoInitialActor,
    const PlayerClass ePlayerClass,
    const BOOL fFemale,
    const BOOL fUseActor2,
    const VisualInventorySlot eVisSlot,
    SNO& snoAlternateActor)
{














    ScopedActorDefinitionLockR pdefActor(snoInitialActor);


    if (pdefActor->tHeader.dwFlags & 2)
    {
        snoAlternateActor = SNO_NONE;
        return;
    }

    snoAlternateActor = snoInitialActor;
    if (!TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_VISUALINV_ENABLE_OVERRIDE))
        return;


    // INFERENCE: two switches, one per fUseActor2 branch (original lines 124 and 158, two class tables);
    // the barbarian tag for classes outside the tables comes from the table defaults. Layout from SNOPreload.cpp:4719.
    ActorTag eVisualInvActorTag;
    if (fUseActor2)
    {
        switch (ePlayerClass)
        {
        default:
        case PLAYER_CLASS_BARBARIAN:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR2_BARBARIAN_MALE;
            break;
        case PLAYER_CLASS_WIZARD:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR2_WIZARD_MALE;
            break;

        case PLAYER_CLASS_WITCHDOCTOR:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR2_WITCHDOCTOR_MALE;
            break;

        case PLAYER_CLASS_MONK:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR2_MONK_MALE;
            break;

        case PLAYER_CLASS_DEMONHUNTER:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR2_DEMONHUNTER_MALE;
            break;

        case PLAYER_CLASS_CRUSADER:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR2_CRUSADER_MALE;
            break;

        case PLAYER_CLASS_NECROMANCER:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR2_NECROMANCER_MALE;
            break;
        }
    }

    else
    {
        switch (ePlayerClass)
        {
        default:
        case PLAYER_CLASS_BARBARIAN:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_BARBARIAN_MALE;
            break;
        case PLAYER_CLASS_WIZARD:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_WIZARD_MALE;
            break;

        case PLAYER_CLASS_WITCHDOCTOR:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_WITCHDOCTOR_MALE;
            break;

        case PLAYER_CLASS_MONK:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_MONK_MALE;
            break;

        case PLAYER_CLASS_DEMONHUNTER:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_DEMONHUNTER_MALE;
            break;

        case PLAYER_CLASS_CRUSADER:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_CRUSADER_MALE;
            break;

        case PLAYER_CLASS_NECROMANCER:
            eVisualInvActorTag = TAG_ACTOR_VISUALINV_ACTOR_NECROMANCER_MALE;
            break;
        }
    }


    if (fFemale)
        eVisualInvActorTag |= 1;



    snoAlternateActor = TagMapGetSNO(pdefActor->hTagMap, eVisualInvActorTag);
}

#line 263 "X/Client/CVisualInventory.cpp"
static void sMarkAttachmentAsUsingSheathedHardpoint(
    AttachmentID idAttachment)
{
    // INFERENCE: both locals sit in a DWARF lexical block, so a test guards them; the original's
    if (idAttachment != ATTACHMENTID_INVALID) {   // cmn w0, #1 after line 472 carries line 0, not 472.
        Attachment* ptAttachment = AttachmentGet(idAttachment);

        RActor * ptRActorAttached = RActorGet(RActorID(ptAttachment->idAttachedObject));
        ptRActorAttached->dwFlags |= 0x20000;
    }
}

#line 278 "X/Client/CVisualInventory.cpp"
static void sUpdateAppearanceByLocation(
    RActor* ptRActor,
    VisualItem& tVisualItemNew,
    const VisualItem& tVisualItemOld,
    VisualInventorySlot eVisSlot,
    PlayerClass ePlayerClass,
    BOOL fFemale)
{
    BOOL fSheathWeapons = (ptRActor->dwFlags >> 16) & 1;
    BOOL bHideWeapons = FALSE;
    if (ItemHasLabel(tVisualItemNew.gbidItem, 0x4d))
    {



        BOOL bFistsHidden = (ptRActor->dwFlags >> 22) & 1;
        bHideWeapons = bFistsHidden;
        fSheathWeapons = FALSE;
    }



    if (ptRActor->dwFlags & 0x800000)
        bHideWeapons = TRUE;



    if (eVisSlot == VIS_LEFT_HAND && (ptRActor->dwFlagsEx & 4))
        bHideWeapons = TRUE;



    if (eVisSlot == VIS_RIGHT_HAND && (ptRActor->dwFlagsEx & 8))
        bHideWeapons = TRUE;




    ItemEffectType eOriginalItemEffectType = tVisualItemNew.eItemEffectType;
    int32 nOriginalItemEffectLevel = tVisualItemNew.nEffectLevel;

    ActorCommonData* ptACD = ACDTryToGet(ptRActor->idACD);
    Player* ptPlayer = PlayerGetByACD(ptRActor->idACD);
    if (ptACD && ptPlayer)
    {
        if (ptRActor->ptLookOverrideMap->Count() > 0 &&



            tVisualItemNew.gbidItem != GBID_NONE &&

            (eVisSlot == VIS_HEAD || eVisSlot == VIS_SHOULDERS))
            return;





        if (eVisSlot == VIS_LEFT_HAND &&
            ptACD->AttributesAttributeExists(0xfffff42f))
        {
            ACDID idACDPrimaryWeapon = ACDInventoryGetPrimaryWeapon(ptACD->id);
            ActorCommonData* ptACDWeapon = ACDTryToGet(idACDPrimaryWeapon);
            if (ptACDWeapon)
            {
                ItemEffectType eItemEffectOverrideType = 0;
                int32 nItemEffectOverrideLevel = 0;
                ACDGetItemEffectTypeAndLevel(*ptACDWeapon, eItemEffectOverrideType, nItemEffectOverrideLevel, TRUE, FALSE);

                tVisualItemNew.eItemEffectType = eItemEffectOverrideType;
                tVisualItemNew.nEffectLevel = nItemEffectOverrideLevel;
            }
            else
            {
                GBID gbidItemLeftHand = tVisualItemNew.gbidItem;
                ItemEffectType eOverrideItemEffect = ptACD->AttributesGetInt(0xfffff42f);
                if (gbidItemLeftHand != GBID_NONE)
                {
                    ScopedItemDefinitionLockR pdefItemLeftHand(gbidItemLeftHand);
                    if (pdefItemLeftHand)
                    {

                        SNO snoEffectGroup = GetEffectGroupSNOForActorAndItemEffectType(pdefItemLeftHand->snoActor, eOverrideItemEffect);
                        if (snoEffectGroup != SNO_NONE)
                        {
                            ScopedEffectGroupDefinitionLockR ptEffectGroupDef(snoEffectGroup);
                            if (ptEffectGroupDef)
                            {
                                tVisualItemNew.eItemEffectType = eOverrideItemEffect;
                                tVisualItemNew.nEffectLevel = ptEffectGroupDef->nEffectCount - 1;
                            }
                        }
                    }
                }
            }
        }
    }


    GBID gbidLastItem = tVisualItemOld.gbidItem;


    if (gbidLastItem != GBID_NONE && gbidLastItem != INVALID_GBID)
    {
        GBHandle hGB(2, gbidLastItem);
        SNO snoActorLastItem = GameBalanceGetActorSNO(hGB);

        Hardpoint tHardPoint;
        if (VisualInventoryGetAttachmentPoint(gbidLastItem, snoActorLastItem, eVisSlot, fSheathWeapons, tHardPoint))
        {
            uint32 dwID = sGetIDForVisualInventorySlot(eVisSlot);
            if (dwID != ATTACHMENTID_INVALID)
            {
                RActorClearActorAttachment(*ptRActor, dwID);
            }
        }
    }


    GBID gbidNewItem = tVisualItemNew.gbidItem;

    if (GameTypeIsPvP() && GameGetGameType() != 5)
    {

        ptPlayer = PlayerGetByACD(ptRActor->idACD);

        if (ptPlayer)
        {
            Team eTeam = ptPlayer->eTeam;
            if (eTeam == PlayerGetLocalTeam())
                tVisualItemNew.eDyeType = 11;
            else
                tVisualItemNew.eDyeType = 16;
        }
        else
        {





            ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
            if (pdefActor->eType == ACTORTYPE_PLAYER)
            {
                tVisualItemNew.eDyeType = 11;
            }
        }
    }



    if (tVisualItemNew.eDyeType == 1 || gbidNewItem == GBID_NONE || gbidNewItem == INVALID_GBID)
    {


        if (eVisSlot == VIS_HEAD)
        {
            RActorSetHairStyle(ptRActor, HAIR_DEFAULT);
        }
        return;
    }

    if ((eVisSlot == VIS_LEFT_HAND || eVisSlot == VIS_RIGHT_HAND) && bHideWeapons)
        return;
    else
    {   // DWARF: hGB, snoActorBaseItem and tHardPoint (447-450) sit in a block, so the attachment code is the else of the 440 test



        GBHandle hGB(2, gbidNewItem);
        SNO snoActorBaseItem = GameBalanceGetActorSNO(hGB);

        Hardpoint tHardPoint;
        if (VisualInventoryGetAttachmentPoint(gbidNewItem, snoActorBaseItem, eVisSlot, fSheathWeapons, tHardPoint) &&
            !(tHardPoint.tHash == uint32(0)))
        {
            uint32 dwID = sGetIDForVisualInventorySlot(eVisSlot);
            if (dwID != ATTACHMENTID_INVALID)
            {
                SNO snoActorToAttach1;
                sFindAlternateAttachmentActor(snoActorBaseItem, ePlayerClass, fFemale, FALSE, eVisSlot, snoActorToAttach1);

                if (eVisSlot == VIS_RIGHT_HAND)
                {

                    SNO snoActorToAttach2;
                    sFindAlternateAttachmentActor(snoActorBaseItem, ePlayerClass, fFemale, TRUE, eVisSlot, snoActorToAttach2);
                    if (snoActorToAttach2 != SNO_NONE)
                        snoActorToAttach1 = snoActorToAttach2;
                }



                AttachmentID idAttachment = sCreateAttachmentForSlot(ptRActor, dwID, snoActorToAttach1, tVisualItemNew, tHardPoint.tHash);
                if (fSheathWeapons)
                {
                    sMarkAttachmentAsUsingSheathedHardpoint(idAttachment);
                }

                switch (eVisSlot)
                {
                case VIS_SHOULDERS:
                    {

                        SNO snoActorToAttach2;
                        sFindAlternateAttachmentActor(snoActorBaseItem, ePlayerClass, fFemale, TRUE, eVisSlot, snoActorToAttach2);
                        sCreateAttachmentForSlot(ptRActor, dwID, snoActorToAttach2, tVisualItemNew, RIGHT_SHOULDER_HARDPOINT_NAME);
                    }
                    break;

                case VIS_HEAD:
                    {
                        HairStyle eHairStyle = HAIR_DEFAULT;
                        if (snoActorToAttach1 != SNO_NONE)
                        {
                            eHairStyle = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_FORCES_HELMET_HAIR, snoActorToAttach1);
                        }

                        RActorSetHairStyle(ptRActor, eHairStyle);
                    }
                    break;
                }
            }
        }
    }


    if (eVisSlot == VIS_LEFT_HAND && ptACD)
    {


        if (
            ptACD->AttributesAttributeExists(0xfffff42f))
        {
            tVisualItemNew.eItemEffectType = eOriginalItemEffectType;
            tVisualItemNew.nEffectLevel = nOriginalItemEffectLevel;
        }
    }
}


#line 521 "X/Client/CVisualInventory.cpp"
static void sShowSubObjectForVisualItem(
    RActor* ptRActor,
    const VisualItem& tVisualItem,
    const InventorySlotType eInventorySlot,
    const BOOL bIsHireling)
{
	GBHandle hGB(2, tVisualItem.gbidItem);
	SNO snoActorItem = GameBalanceGetActorSNO(hGB);

	ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

	ComponentStyle eCompStyle;
	int32 nSubObjectLook;
	AppearanceGetNakedLook(pdefActor->snoAppearance, eCompStyle, nSubObjectLook);

	if (snoActorItem != SNO_NONE)
	{

		eCompStyle = ActorDefinitionGetTagMapValueInt(0x10400, snoActorItem);

		// Lines 540-563 leave no rows in the original (code eliminated here; bIsHireling is FALSE at the only call).






















		uint32 dwLookKey = ActorDefinitionGetTagMapValueInt(0x10401, snoActorItem);


		for (int32 i = 0; gComponentLooks[i].sName; ++i)
		{
			if (StringHash(gComponentLooks[i].sName) == dwLookKey)
			{
				dwLookKey = gComponentLooks[i].nValue;
				break;
			}
		}

		nSubObjectLook = AppearanceSubObjectFromLookKey(pdefActor->snoAppearance, dwLookKey);
	}

	DyeType eDyeType = tVisualItem.eDyeType;
    if (GameTypeIsPvP() && GameGetGameType() != 5)
    {
        Player* ptPlayer = PlayerGetByACD(ptRActor->idACD);

        if (ptPlayer)
        {
            Team eTeam = ptPlayer->eTeam;
            if (eTeam == PlayerGetLocalTeam())
            {
                eDyeType = 11;
            }
            else
            {
                eDyeType = 16;
            }
        }
        else
        {

            ScopedActorDefinitionLockR pdefActor2(ptRActor->snoActor);
            if (pdefActor2->eType == ACTORTYPE_PLAYER)
            {
                eDyeType = 11;
            }
        }
    }

    if (eInventorySlot != 3 && eInventorySlot != 4 &&
        ptRActor->ptLookOverrideMap->Count() > 0)
    {
        return;
    }




	RActorComponentsSetSlot(ptRActor, eInventorySlot, eCompStyle, nSubObjectLook, eDyeType, TRUE);
}
#line 620 "X/Client/CVisualInventory.cpp"
struct SlotPair {
    VisualInventorySlot eVisualSlot;
    InventorySlotType eInvSlot;
};
#line 628 "X/Client/CVisualInventory.cpp"
static void sVisualInventoryApplyToBodySlots(RActor* ptRActor, VisualInventory& tVisualInventory, VisualInventoryType eVisualInventoryType)
{
    // Original data at0xe3d28c; final pair is the sentinel.
    static const SlotPair tSlotPairBody[] = {
        {VIS_TORSO, 2},
        {VIS_FEET, 7},
        {VIS_HANDS, 5},
        {VIS_LEGS, 9},
        {VIS_INVALID, -1}
    };
    if (eVisualInventoryType == VIT_HIRELING) return;
    for (int32 i = 0; tSlotPairBody[i].eVisualSlot != VIS_INVALID; ++i) {
        const VisualItem& tVisualItemNew = tVisualInventory.tEquipmentCurrent.tVisualItem[tSlotPairBody[i].eVisualSlot];
        const VisualItem& tVisualItemOld = tVisualInventory.tEquipmentLast.tVisualItem[tSlotPairBody[i].eVisualSlot];
        if (tVisualItemNew != tVisualItemOld) {
            InventorySlotType eInvSlot = tSlotPairBody[i].eInvSlot;
            sShowSubObjectForVisualItem(ptRActor, tVisualItemNew, eInvSlot, eVisualInventoryType == VIT_HIRELING);
            tVisualInventory.tEquipmentLast.tVisualItem[tSlotPairBody[i].eVisualSlot] = tVisualItemNew;
        }
    }
}

#line 668 "X/Client/CVisualInventory.cpp"
static void sVisualInventoryApplyToAttachmentSlots(RActor* ptRActor, VisualInventory& tVisualInventory, const VisualInventoryType eVisualInventoryType, const PlayerClass ePlayerClass, const BOOL fFemale)
{
    // Original constant arrays at0xe3d2b4 and0xe3d2c8.
    static const VisualInventorySlot tSlotPlayerAttached[] = { VIS_HEAD, VIS_LEFT_HAND, VIS_RIGHT_HAND, VIS_SHOULDERS, VIS_INVALID };
    static const VisualInventorySlot tSlotHirelingAttached[] = { VIS_LEFT_HAND, VIS_RIGHT_HAND, VIS_INVALID };
    const VisualInventorySlot* ptSlotAttachedArray = eVisualInventoryType == VIT_HIRELING ? tSlotHirelingAttached : tSlotPlayerAttached;
    VisualItem& tVisualItemL = tVisualInventory.tEquipmentCurrent.tVisualItem[VIS_LEFT_HAND];
    VisualItem& tVisualItemR = tVisualInventory.tEquipmentCurrent.tVisualItem[VIS_RIGHT_HAND];
    if (VisualInventoryShouldSwapWeapons(ePlayerClass, tVisualItemL.gbidItem, tVisualItemR.gbidItem)) {
        Math::Swap(tVisualItemL, tVisualItemR);  // DWARF: Math::Swap<VisualItem> inlined at 703



    }
    for (int32 i = 0; ptSlotAttachedArray[i] != VIS_INVALID; ++i) {
        VisualInventorySlot eVisualSlot = ptSlotAttachedArray[i];
        VisualItem& tVisualItemNew = tVisualInventory.tEquipmentCurrent.tVisualItem[eVisualSlot];
        VisualItem& tVisualItemOld = tVisualInventory.tEquipmentLast.tVisualItem[eVisualSlot];
        if (tVisualItemNew != tVisualItemOld) sUpdateAppearanceByLocation(ptRActor, tVisualItemNew, tVisualItemOld, eVisualSlot, ePlayerClass, fFemale);
        tVisualItemOld = tVisualItemNew;
    }
}

#line 723 "X/Client/CVisualInventory.cpp"
static void sVisualInventoryApplyCosmetics(RActor* ptRActor, VisualInventory& tVisualInventory)
{
    // Inferred two-entry loop spelling for original separately unrolled cosmetic updates.
    for (int32 i = VCIS_WINGS; i <= VCIS_PENNANT; ++i) {
        if (tVisualInventory.tEquipmentCurrent.tCosmeticItems[i] != tVisualInventory.tEquipmentLast.tCosmeticItems[i]) {
            RActorRemoveCosmeticBuff(ptRActor, i + 1, tVisualInventory.tEquipmentLast.tCosmeticItems[i].gbidItem, TRUE);
            if (tVisualInventory.tEquipmentCurrent.tCosmeticItems[i].gbidItem != GBID_NONE)
                RActorAddCosmeticBuff(ptRActor, i + 1, tVisualInventory.tEquipmentCurrent.tCosmeticItems[i].gbidItem, TRUE);
        }
        tVisualInventory.tEquipmentLast.tCosmeticItems[i] = tVisualInventory.tEquipmentCurrent.tCosmeticItems[i];
    }
    if (tVisualInventory.tEquipmentCurrent.tCosmeticItems[VCIS_PET] != tVisualInventory.tEquipmentLast.tCosmeticItems[VCIS_PET]) {
        if (PlayerGetByRActor(ptRActor->id)) return;
        uint32 dwID = INTERNAL_ATTACHMENTID_VIS_PET;
        RActorClearActorAttachment(*ptRActor, dwID);
        if (tVisualInventory.tEquipmentCurrent.tCosmeticItems[VCIS_PET].gbidItem != GBID_NONE) {
            // Inferred local name for original pet actor SNO return value.
            SNO snoPet = VisualInventoryGetPetSnoFromGbid(tVisualInventory.tEquipmentCurrent.tCosmeticItems[VCIS_PET].gbidItem);
            if (snoPet != SNO_NONE) RActorAddAttachment(*ptRActor, 0, snoPet, dwID, HardpointHash("HP_Pet"), TRUE);
        }
    }
    tVisualInventory.tEquipmentLast.tCosmeticItems[VCIS_PET] = tVisualInventory.tEquipmentCurrent.tCosmeticItems[VCIS_PET];
    tVisualInventory.tEquipmentLast.tCosmeticItems[VCIS_PORTRAIT] = tVisualInventory.tEquipmentCurrent.tCosmeticItems[VCIS_PORTRAIT];
}

#line 784 "X/Client/CVisualInventory.cpp"
void VisualInventoryApply(
    RActor* ptRActor,
    VisualInventory& tVisualInventory,
    const VisualInventoryType eVisualInventoryType,
    const PlayerClass ePlayerClass,
    const BOOL fFemale)
{
    LockAllSubObjectLookUpdate(ptRActor);

    sVisualInventoryApplyToBodySlots(ptRActor, tVisualInventory, eVisualInventoryType);
    sVisualInventoryApplyToAttachmentSlots(ptRActor, tVisualInventory, eVisualInventoryType, ePlayerClass, fFemale);
    sVisualInventoryApplyCosmetics(ptRActor, tVisualInventory);

    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    UnlockAllSubObjectLookUpdate(ptRActor, pdefAppearance);
}
#line 803 "X/Client/CVisualInventory.cpp"
void VisualInventoryNewEquipment(const ACDID idACD, VisualEquipment& tEquipment)
{
    VisualInventory* ptVisualInventory = VisualInventoryGet(idACD);
    if (ptVisualInventory) {
        ptVisualInventory->tEquipmentCurrent = tEquipment;
        ptVisualInventory->tEquipmentCurrent.FixOffHandWeapons();
        Player* ptPlayer = PlayerGetByACD(idACD);
        BOOL fFemale = FALSE;
        PlayerClass ePlayerClass = -1;
        if (ptPlayer) {
            ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
            fFemale = (ptPlayer->tHeroStateData.uPlayerFlags >> 1) & 1;
        }
        VisualInventoryType eVisualInventoryType = VisualInventoryGetType(idACD);
        const RActorID idRActor = RActorID(ACDGetOwner(idACD));
        RActor* ptRActor = RActorTryToGet(idRActor);
        if (ptRActor) VisualInventoryApply(ptRActor, *ptVisualInventory, eVisualInventoryType, ePlayerClass, fFemale);
    }
}
#line 832 "X/Client/CVisualInventory.cpp"
void VisualInventoryUpdateSheathed(const ACDID idACD)
{
    const VisualInventorySlot tSlots[] = { VIS_LEFT_HAND, VIS_RIGHT_HAND, VIS_INVALID };
    VisualInventory* ptVisualInventory = VisualInventoryGet(idACD);
    if (!ptVisualInventory) return;
    const RActorID idRActor = RActorID(ACDGetOwner(idACD));
    RActor* ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor) return;
    Player* ptPlayer = PlayerGetByACD(idACD);
    BOOL fFemale = FALSE;
    PlayerClass ePlayerClass = -1;
    if (ptPlayer) {
        ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
        fFemale = (ptPlayer->tHeroStateData.uPlayerFlags >> 1) & 1;
    }
    for (int32 i = 0; tSlots[i] != VIS_INVALID; ++i) {
        VisualInventorySlot eVisualSlot = tSlots[i];
        VisualItem& tVisualItemNew = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlot];
        sUpdateAppearanceByLocation(ptRActor, tVisualItemNew, tVisualItemNew, eVisualSlot, ePlayerClass, fFemale);
    }
}
