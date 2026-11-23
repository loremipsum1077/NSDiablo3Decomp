// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer (see X/Common/NPC.cpp).
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "Hireling.h"
#include "ActorCommonData.h"
#include "GlobalSNO.h"
#include "Game.h"
#include "Player.h"
#include "Definitions/SkillKitDefinition.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Math/MathGeneral.h"
#include "Attributes/FastAttribute.h"
#include "Definitions/PowerDefinition.h"
#include "Powers/PowersCommon.h"
#include "Definitions/GlobalDefinition.h"
#include "Definitions/GameBalanceDefinition.h"
#include "GameBalance.h"
#include "HirelingSwig.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "ItemsSwig.h"
#include "ComponentStyle.h"
#include "Powers/PowerAPISwig.h"
#include "../Server/Actors/Actors.h"
#include "../Client/CPlayer.h"
#include <stddef.h>
enum {
    GB_HIRELINGS = 4,
};
// X/Common/Attributes/FastAttribute.cpp:4571
AttribGroupID FastAttribLightCreate(FastAttribType eType);
// Declared here because their headers are not adopted yet: X/Client/CHireling.h, X/Client/RActors/RActors.h.
void CHirelingSetAppearance(RActor * ptRActor, HirelingClass eClass, int32 nLevel);
void RActorComponentsSetSlot(RActor * ptRActor, const InventorySlotType eSlot, const ComponentStyle eCompStyle, const int32 nSubObjectLook, const int32 eDyeType, const BOOL bDisplayErrors);
// X/Common/Hireling.h (not adopted into src/X/Common/Hireling.h yet; copies live in ScriptUtil.cpp etc.)
struct HirelingSwapMessage { RequiredMessageHeader tHeader; HirelingClass eNewClass; };
static const TypeDescriptorField HirelingInfoUpdateMessage_Fields[13] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HirelingInfoUpdateMessage, tHeader), NULL, 1),
    // bound inferred from the static initializer (0..3); NUM_HIRELING_CLASSES - 1 is the name guess
    BoundedTypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, eClass), NULL, (const void *)0, (const void *)(NUM_HIRELING_CLASSES - 1)),
    BoundedTypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, fDead), NULL, (const void *)FALSE, (const void *)TRUE),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, gbidName), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, eValidClass), NULL, (const void *)0, (const void *)(NUM_HIRELING_CLASSES - 1)),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, arnPowerKeyParam[0]), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, arnPowerKeyParam[1]), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, arnPowerKeyParam[2]), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, arnPowerKeyParam[3]), (const void *)-1, 1),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, arnPowerKeyParam[4]), (const void *)-1, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, annItems), NULL, 1, 6, 0),
    TypeDescriptorField("", DT_INT, offsetof(HirelingInfoUpdateMessage, nPlayerIndex), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(HirelingInfoUpdateMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HirelingInfoUpdateMessage("HirelingInfoUpdateMessage", 0, HirelingInfoUpdateMessage_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HIRELING_INFO_UPDATE_MESSAGE = &TypeDescriptor_HirelingInfoUpdateMessage;



static const TypeDescriptorField HirelingSwapMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(HirelingSwapMessage, tHeader), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(HirelingSwapMessage, eNewClass), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(HirelingSwapMessage), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HirelingSwapMessage("HirelingSwapMessage", 0, HirelingSwapMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_HIRELING_SWAP_MESSAGE = &TypeDescriptor_HirelingSwapMessage;


// value from the image (data symbol 0x7100e49108); declared extern in X/Server/SHireling.cpp
extern const uint32 MAX_FOLLOWER_SWAP_TIME = 7200;



static void sClearHirelingInfo(HirelingInfo & tInfo)
{
    SigmaMemoryClear(&tInfo, sizeof(tInfo));
    tInfo.eClass = 0;
    tInfo.gbidName = GBID_NONE;
    for (int32 i = 0; i < 5; i++)
        tInfo.arnPowerKeyParam[i] = -1;
}







void HirelingDataInit(HirelingData & tData)
{
    sClearHirelingInfo(tData.tInfo);
    for (int32 i = 0; i < 6; i++)
        tData.tItems.idACDItems[i] = ACDID_INVALID;
}








static void sDestroyItem(
    ACDID idACDItem)
{



    ActorID idActor = (ActorID)ACDGetOwner(idACDItem);


    ActorDelete(idActor);
}




void HirelingDataDispose(
    HirelingData & tData)
{
    sClearHirelingInfo(tData.tInfo);



    if (GameActiveGameCommonDataIsServer())
    {

        for (int32 i = 0; i < 6; i++)
        {
            if (tData.tItems.idACDItems[i] != ACDID_INVALID)
            {
                sDestroyItem(tData.tItems.idACDItems[i]);
                tData.tItems.idACDItems[i] = ACDID_INVALID;
            }
        }
    }
}



void HirelingSavedDataSetDefaults(HirelingSavedData & tSavedData)
{
    for (int32 i = 0; i < 4; i++)
        sClearHirelingInfo(tSavedData.tHireling[i]);
    tSavedData.eActiveHireling = 0;
    tSavedData.dwAvailableHirelings = 0;
}









void HirelingStableInit(HirelingStable & tStable)
{
    for (int32 i = 0; i < 4; i++)
        HirelingDataInit(tStable.tHireling[i]);
    tStable.eActiveHireling = 0;
    tStable.eTypeToCreate = 0;
}










void HirelingStableDispose(HirelingStable & tStable)
{
    for (int32 i = 0; i < 4; i++)
        HirelingDataDispose(tStable.tHireling[i]);
    tStable.eActiveHireling = 0;
    tStable.eTypeToCreate = 0;
}









struct THirelingClassLookup {  // size 0x8
    GlobalGBID globalGBID;  // 0x00
    HirelingClass eClass;  // 0x04
};



static THirelingClassLookup sgtHirelingClassLookupTable[3] = {
    { GLOBALGBID_HIRELING_TEMPLAR, 1 },
    { GLOBALGBID_HIRELING_SCOUNDREL, 2 },
    { GLOBALGBID_HIRELING_ENCHANTRESS, 3 },
};

static const int32 sgnNumHirelingClassLookupTableEntries = sizeof(sgtHirelingClassLookupTable) / sizeof(sgtHirelingClassLookupTable[0]);




GBID HirelingGetGBID(HirelingClass eClass)
{
    for (int32 i = 0; i < sgnNumHirelingClassLookupTableEntries; i++)
    {
        if (sgtHirelingClassLookupTable[i].eClass == eClass)
            return GlobalGBIDGet(sgtHirelingClassLookupTable[i].globalGBID);
    }
    return GBID_NONE;
}








HirelingClass HirelingGetClass(const ActorCommonData & tACDHireling)
{
    HirelingClass eClass = 0;
    if (tACDHireling.IsMonster())
    {
        GBID gbID = tACDHireling.hGB.gbid;
        for (int32 i = 0; i < sgnNumHirelingClassLookupTableEntries; i++)
        {
            if (gbID == GlobalGBIDGet(sgtHirelingClassLookupTable[i].globalGBID))
            {
                eClass = sgtHirelingClassLookupTable[i].eClass;
                break;
            }
        }
    }
    return eClass;
}













SNO HirelingGetSNO(HirelingClass eClass, HirelingType eType)
{
    // Find the hireling's game balance entry from its class.
    for (int32 i = 0; i < sgnNumHirelingClassLookupTableEntries; i++)
    {
        if (sgtHirelingClassLookupTable[i].eClass == eClass)
        {
            const GameBalanceDefinition * pdefGB;
            GBHandle hGB(GB_HIRELINGS, GlobalGBIDGet(sgtHirelingClassLookupTable[i].globalGBID));

            SNO snoActor = SNO_NONE;
            const HirelingEntry * ptHireling = (const HirelingEntry *)GBLockR(hGB, pdefGB, TRUE);

            switch (eType)
            {
            case HT_NORMAL:
                snoActor = ptHireling->snoActor;
                break;

            case HT_PROXY:
                snoActor = ptHireling->snoProxyActor;
                break;

            case HT_SUMMONED:
                snoActor = ptHireling->snoSummonedActor;
                break;

            default:
                break;
            }

            GBUnlockR(pdefGB);
            return snoActor;
        }
    }

    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}




static void sHirelingCheatItemsForHireling(
    ACDID idACDHireling,
    Player & tPlayer)
{
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    // no line-table row of its own; implied by the branch to the epilogue
    if (!ptACDHireling)
        return;


    HirelingClass eClass = ACDGetHirelingClass(*ptACDHireling);




    if (eClass < HC_FIRST || eClass >= NUM_HIRELING_CLASSES)
        return;





    HirelingItemStore * ptItems = &tPlayer.tHirelingStable.tHireling[eClass].tItems;


    for (int32 i = 0; i < 6; i++)
    {

        InventorySlotType eSlot = SLOT_HIRELING_START + i;


        ptItems->idACDItems[i] = ACDInventoryGetSquareItem(*ptACDHireling, eSlot, cgptNull);
    }
}



static void sHirelingUnCheatItemsForHireling(
    ACDID idACDHireling,
    Player & tPlayer)
{
    ActorCommonData & tACDHireling = ACDGet(idACDHireling);
    HirelingClass eClass = ACDGetHirelingClass(tACDHireling);
    HirelingItemStore * ptItems = &tPlayer.tHirelingStable.tHireling[eClass].tItems;




    for (int32 i = 0; i < 6; i++)
    {
        ptItems->idACDItems[i] = ACDID_INVALID;
    }
}



void HirelingCheatItemsForActiveHirelingAndProxies(
    Player & tPlayer,
    BOOL bMoveToStable)
{

    ACDID idACDHireling = PlayerGetHireling(tPlayer.idPlayerACD);
    if (idACDHireling != ACDID_INVALID)
    {
        if (bMoveToStable)
        {
            sHirelingCheatItemsForHireling(idACDHireling, tPlayer);
        }
        else
        {
            sHirelingUnCheatItemsForHireling(idACDHireling, tPlayer);
        }
    }


    int32 nPos;
    ACDID idACDProxy = PlayerGetFirstPet(tPlayer.idPlayerACD, PETTYPE_HIRELING_PROXY, nPos);
    while (idACDProxy != ACDID_INVALID)
    {
        if (bMoveToStable)
        {
            sHirelingCheatItemsForHireling(idACDProxy, tPlayer);
        }
        else
        {
            sHirelingUnCheatItemsForHireling(idACDProxy, tPlayer);
        }

        idACDProxy = PlayerGetNextPet(tPlayer.idPlayerACD, PETTYPE_HIRELING_PROXY, nPos);
    }
}


BOOL HirelingTestValid(ACDID idACDPlayer, HirelingClass eClass)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return FALSE;

    HirelingInfo * ptInfo = &ptPlayer->tHirelingStable.tHireling[eClass].tInfo;
    return ptInfo->eClass != 0;
}













void HirelingSetTypeToCreate(ACDID idACDPlayer, HirelingClass eClass)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
        ptPlayer->tHirelingStable.eTypeToCreate = eClass;
}









HirelingClass HirelingGetTypeToCreate(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
        return ptPlayer->tHirelingStable.eTypeToCreate;
    return 0;
}









void HirelingSetTypeAvailable(ACDID idACDPlayer, HirelingClass eClass, BOOL fAvailable)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer)
        return;

    if (((ptPlayer->tHeroStateData.tSavedData.tHirelingSavedData.dwAvailableHirelings & (1 << eClass)) != 0) != (fAvailable != 0) && GameActiveGameCommonDataIsServer())
    {
        ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
        tACDPlayer.MarkHeroStateChanged();
    }

    if (fAvailable)
        ptPlayer->tHeroStateData.tSavedData.tHirelingSavedData.dwAvailableHirelings |= (1 << eClass);
    else
        ptPlayer->tHeroStateData.tSavedData.tHirelingSavedData.dwAvailableHirelings &= ~(1 << eClass);
}












BOOL HirelingTestTypeAvailable(ACDID idACDPlayer, HirelingClass eClass)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (eClass <= 0 || eClass >= 4 || !ptPlayer)
        return FALSE;
    return (ptPlayer->tHeroStateData.tSavedData.tHirelingSavedData.dwAvailableHirelings >> eClass) & 1;
}














static inline BOOL sHasConflictingPower(const ActorCommonData & tACDHireling, const SNO snoPower)
{
    SNO snoSkillKit = tACDHireling.GetSkillKit();
    if (snoSkillKit != SNO_NONE)
    {
        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
        if (pdefSkillKit)
        {
            int32 nMasteryReq = -1;
            BOOL bFound = FALSE;
            int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
            for (int32 i = 0; i < nCount; i++)
            {
                ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
                if (tEntry.snoPower == snoPower && !bFound)
                {
                    nMasteryReq = tEntry.nMasteryReq;
                    bFound = TRUE;
                }
            }

            if (!bFound || nMasteryReq < 0)
                return FALSE;

            for (int32 i = 0; i < nCount; i++)
            {
                ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
                if (tEntry.snoPower != snoPower && tEntry.nMasteryReq == nMasteryReq)
                {
                    int32 nSkillLevel = tACDHireling.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, tEntry.snoPower));
                    if (nSkillLevel > 0)
                        return TRUE;
                }
            }
        }
    }
    return FALSE;
}





























HirelingTrainReason HirelingCanTrainSkillReason(const ActorCommonData & tACDHireling, const SNO snoPower)
{
    if (snoPower == SNO_NONE)
        return HIRELING_TRAIN_INVALID_SKILL;

    if (tACDHireling.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower)) > 0)
        return HIRELING_TRAIN_ALREADY_PURCHASED;

    BOOL bMeetsMasteryRequirement = PowerActorMeetsActiveSkillMasteryRequirement(tACDHireling, AutoPowerDefinitionR(snoPower));
    if (!bMeetsMasteryRequirement)
        return HIRELING_TRAIN_LEVEL_REQUIREMENT_NOT_MET;

    if (sHasConflictingPower(tACDHireling, snoPower))
        return HIRELING_TRAIN_CONFLICTING_SKILL_PURCHASED;

    return HIRELING_TRAIN_OK;
}














int32 HirelingGetUnlockLevelBase(const SkillKitDefinition * pdefSkillKit)
{
    int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    if (nCount == 0)
        return 0;

    int32 nLevelBase = 70;
    for (int32 i = 0; i < nCount; i++)
    {
        const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        nLevelBase = Math::Min(nLevelBase, tEntry.nMasteryReq);
    }
    return nLevelBase;
}









SNO HirelingGetPower(const ActorCommonData & tACDHireling, const int32 nRow, const int32 nColumn)
{
    SNO snoSkillKit = tACDHireling.GetSkillKit();
    if (snoSkillKit == SNO_NONE)
        return SNO_NONE;

    SNO snoPower = SNO_NONE;
    int32 nCurColumn = 0;
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    int32 nUnlockLevelBase = HirelingGetUnlockLevelBase(pdefSkillKit);
    int32 nMasteryReq = nUnlockLevelBase + nRow * 5;
    for (int32 i = 0; i < nCount; i++)
    {
        const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        if (tEntry.snoPower != SNO_NONE && tEntry.nMasteryReq == nMasteryReq)
        {
            if (nCurColumn == nColumn)
                snoPower = tEntry.snoPower;
            nCurColumn++;
        }
    }
    return snoPower;
}






















BOOL HirelingPowerGetRowColumn(const ActorCommonData & tACDHireling, SNO snoPower, int32 & nRow, int32 & nColumn)
{
    int32 nCurrColumn[5] = { 0 };
    if (snoPower == SNO_NONE)
        return FALSE;

    SNO snoSkillKit = tACDHireling.GetSkillKit();
    if (snoSkillKit == SNO_NONE)
        return FALSE;

    {
        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
        int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
        int32 nUnlockLevelBase = HirelingGetUnlockLevelBase(pdefSkillKit);
        for (int32 i = 0; i < nCount; i++)
        {
            ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
            int32 nCurrentRow = (tEntry.nMasteryReq - nUnlockLevelBase) / 5;
            if (tEntry.snoPower == snoPower)
            {
                nRow = nCurrentRow;
                nColumn = nCurrColumn[nCurrentRow];
                return TRUE;
            }
            nCurrColumn[nCurrentRow]++;
        }
    }
    return FALSE;
}
























BOOL HirelingHasUnspentSkillPoints(const ActorCommonData & tACDHireling)
{
    SNO snoSkillKit = tACDHireling.GetSkillKit();
    if (snoSkillKit != SNO_NONE)
    {
        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
        if (pdefSkillKit)
        {
            int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
            for (int32 i = 0; i < nCount; i++)
            {
                ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
                HirelingTrainReason eReason = HirelingCanTrainSkillReason(tACDHireling, tEntry.snoPower);
                if (eReason == HIRELING_TRAIN_OK)
                    return TRUE;
            }
        }
    }
    return FALSE;
}







inline BOOL HirelingHasSpentSkillPoints(const ActorCommonData & tACDHireling)
{
    SNO snoSkillKit = tACDHireling.GetSkillKit();
    if (snoSkillKit != SNO_NONE)
    {
        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
        if (pdefSkillKit)
        {
            int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
            for (int32 i = 0; i < nCount; i++)
            {
                ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
                if (tEntry.snoPower != SNO_NONE && tACDHireling.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, tEntry.snoPower)) > 0)
                    return TRUE;
            }
        }
    }
    return FALSE;
}








BOOL HirelingDoesLevelGrantNewSkillRow(const ActorCommonData & tACDHireling, int32 nLevel)
{
    SNO snoSkillKit = tACDHireling.GetSkillKit();
    if (snoSkillKit == SNO_NONE)
        return FALSE;

    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    if (pdefSkillKit)
    {
        int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
        for (int32 i = 0; i < nCount; i++)
        {
            ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
            if (tEntry.nMasteryReq == nLevel)
                return TRUE;
        }
    }
    return FALSE;
}









BOOL HirelingCanRetrain(ActorCommonData & tACDHireling, ActorCommonData & tACDPlayer)
{
    if (!tACDHireling.IsAlive())
        return FALSE;

    if (tACDHireling.AttributesGetInt(ATTRIBUTE_LEVEL) < 5)
        return FALSE;

    return HirelingHasSpentSkillPoints(tACDHireling);
}






























void CHirelingUpdateAppearance(
    const ActorCommonData & tACDHireling,
    RActor * ptRActor,
    HirelingClass eClass)
{
    int32 nLevel = 1;

    if (eClass == HC_NONE)
    {
        eClass = ACDGetHirelingClass(tACDHireling);
        if (eClass == HC_NONE)
            return;
    }





    Player * ptPlayer = LocalPlayerGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {
        nLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    }

    CHirelingSetAppearance(ptRActor, eClass, nLevel);
}


void CHirelingSetAppearance(
    RActor * ptRActor,
    HirelingClass eClass,
    int32 nLevel)
{


    GBHandle hGB(GB_HIRELINGS, HirelingGetGBID(eClass));
    const GameBalanceDefinition * pdefHirelings;
    const HirelingEntry * pHireling = (const HirelingEntry *)GBLockR(hGB, pdefHirelings, TRUE);


    int32 nLightLookLevel = (int32)pHireling->flAttributesHirelingLightLookLevel;
    int32 nMediumLookLevel = (int32)pHireling->flAttributesHirelingMediumLookLevel;
    int32 nHeavyLookLevel = (int32)pHireling->flAttributesHirelingHeavyLookLevel;

    ComponentStyle eCompStyle = COMPONENTSTYLE_NONE;
    if (nLevel >= nHeavyLookLevel)
    {
        eCompStyle = COMPONENTSTYLE_HEAVY_A;
    }
    else if (nLevel >= nMediumLookLevel)
    {
        eCompStyle = COMPONENTSTYLE_MEDIUM_A;
    }
    else if (nLevel >= nLightLookLevel)
    {
        eCompStyle = COMPONENTSTYLE_LIGHT_A;
    }


    // 0 is the dye type (DyeType enumerator for 0 not found in src)
    RActorComponentsSetSlot(ptRActor, SLOT_PLAYER_TORSO, eCompStyle, 0, 0, FALSE);
    GBUnlockR(pdefHirelings);
}


// NON_MATCHING: 508 vs 524. The original keeps eAttribCapped/eAttribCappedSubtotal as csel on a fresh eAttrib == 0x41 test
// inside the player block (936/937); ours folds them to constants. if/ternary/switch forms and reduced 928 conditions tried.
float HirelingGetDiminishedAttribute(
    ACDID idACDPlayer,
    Attrib eAttrib)
{
    ACDID idACDHireling = PlayerGetHireling(idACDPlayer);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling)
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        float flRatio = pdefGlobal->flHirelingDiminishedBonusRatio;
        float flUncappedValue = flRatio * ptACDHireling->AttributesGetFloat(FastAttribKey(eAttrib));



        // Inference: lines 911-926 unknown beyond the decl lines 915/916; selection written as an if.
        Attrib eAttribCapped = ATTRIBUTE_NONE;
        Attrib eAttribCappedSubtotal = ATTRIBUTE_NONE;
        if (eAttrib == ATTRIBUTE_MAGIC_FIND_TOTAL)
        {
            eAttribCappedSubtotal = ATTRIBUTE_MAGIC_FIND_CAPPED_SUBTOTAL;
            eAttribCapped = ATTRIBUTE_MAGIC_FIND;
        }





        float flFinalValue = flUncappedValue;
        if (idACDPlayer != ACDID_INVALID && eAttribCappedSubtotal != ATTRIBUTE_NONE && eAttribCapped != ATTRIBUTE_NONE)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
            if (ptACDPlayer)
            {
                AttribGroupID idAttribTempPlayer = FastAttribGroupCreate(1, -1);
                AttribGroupID idAttribTempHireling = FastAttribLightCreate(1);

                float flPlayerCappedValue = ptACDPlayer->AttributesGetFloat(FastAttribKey(eAttribCappedSubtotal));
                FastAttribSetValueFloat(idAttribTempPlayer, FastAttribKey(eAttribCapped), flPlayerCappedValue);
                FastAttribSetValueFloat(idAttribTempHireling, FastAttribKey(eAttribCapped), flFinalValue);
                FastAttribMerge(idAttribTempPlayer, idAttribTempHireling);

                float flCappedResult = FastAttribGetValueFloat(idAttribTempPlayer, FastAttribKey(eAttrib));
                flFinalValue = flCappedResult - flPlayerCappedValue;
                flFinalValue = Math::Max(0.0f, flFinalValue);

                FastAttribUnmerge(idAttribTempPlayer, idAttribTempHireling);
                FastAttribGroupDelete(idAttribTempHireling);
                FastAttribGroupDelete(idAttribTempPlayer);
            }
        }
        return flFinalValue;
    }

    return 0.0f;

}
