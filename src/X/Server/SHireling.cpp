// Original file: X/Server/SHireling.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Player.h"
#include "../Common/Hireling.h"
#include "../Common/ActorInventory.h"
#include "../Common/ItemsSwig.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameTemplate.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "SGameStats.h"
#include "SGameStatsSwig.h"
#include "../Common/HirelingSwig.h"
#include "LuaScript/ScriptEvent.h"
#include "../Common/Networking/GameMessage.h"
#include "PlayerList.h"
#include "../Common/Monsters.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Definitions/GlobalDefinition.h"

// Declared here from their definitions elsewhere (headers not adopted yet).
HirelingClass HirelingGetClass(const ActorCommonData & tACD);  // X/Common/Hireling.cpp
BOOL HirelingPowerGetRowColumn(const ActorCommonData & tACD, SNO snoPower, int32 & nRow, int32 & nColumn);  // X/Common/Hireling.cpp
void SHirelingSendUpdate(ACDID idACDPlayer, HirelingClass eClass);
ACDID SHirelingCreate(ActorCommonData & tACDPlayer, ActorCommonData & tACDNextTo, const HirelingClass eHirelingClass, BOOL bForceCreate, HirelingType eType);
BOOL SHirelingCanHire(const Player & tPlayer);
enum { NUM_HIRELING_POWER_SLOTS = 5 };
BOOL ACDIsHirelingProxy(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp:8221
void ACDInventoryRemoveToLimbo(const ACDID idACDContainer, ActorCommonData & tACDItem, BOOL bRemoveFromKnownList);  // X/Common/ActorInventory.cpp:1515
void ActorRefreshHirelingKnownActors(HirelingClass eClass);  // X/Server/Actors/Actors.cpp:7026
void SHirelingSetActive(ACDID idACDPlayer, HirelingClass eClass, BOOL bSave);
ACDID PlayerGetHirelingProxy(ACDID idACDPlayer, HirelingClass eClass);  // X/Common/Player.cpp

#include "../Common/Nav/NavigationMesh.h"
struct PowerAPIChooseOpenGround {  // size 0x20; belongs in X/Server/Powers/PowerAPI.h (PowerHelper.cpp has the same)
    NavPlace tNavPlace;  // 0x00
    float flSkipDistance;  // 0x18
    BOOL bUseActorPassability;  // 0x1c
};
BOOL PowerAPI_DoChooseOpenGround(SNO snoActor, float flScale, const PowerAPIChooseOpenGround & tOpenGroundInfo, WorldPlace & tPlace, NavMeshHandle & tNavMeshHandle);  // X/Server/Powers/PowerAPI.cpp
void PowerGetNavPlaceFromACD(const ActorCommonData * ptACD, NavPlace & tNavPlace);  // X/Common/Powers/PowersCommon.cpp
static void sGetInfoFromACD(HirelingInfo & tInfo, const ActorCommonData & tACD)
{
    tInfo.eClass = HirelingGetClass(tACD);
    tInfo.gbidName = tACD.pRareNameGBIDs[0];
    tInfo.fDead = !tACD.IsAlive();

    for (uint32 i = 0; i < NUM_HIRELING_POWER_SLOTS; i++)
    {
        tInfo.arnPowerKeyParam[i] = -1;
    }

    AttribGroupID idFastAttrib = tACD.AttributesGetAttributeGroup();
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        if (KeyGetAttrib(tKey) == ATTRIBUTE_SKILL)
        {
            int32 nPowerParam = KeyGetParam(tKey);
            int32 nRow;
            int32 nColumn;
            if (HirelingPowerGetRowColumn(tACD, nPowerParam, nRow, nColumn))
            {
                tInfo.arnPowerKeyParam[nRow] = nPowerParam;
            }
        }
    }
}


#include "../Common/Powers/PowerAPISwig.h"
#include "SGameManager.h"
#include "Powers/PowerAPI.h"
void ExperienceSetLevel(const ACDID idACD, int32 nLevels, const BOOL fUpdateClient);  // X/Server/Experience.h
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, const ACDID idACDContainer, const InventorySlotType eSlot, const int32 nSlotX, const int32 nSlotY);  // X/Common/ActorInventory.cpp
BOOL ACDInventoryDelete(ActorCommonData & tACDItem, BOOL bMarkItemAsDirty);  // X/Common/ActorInventory.cpp
void ACDInventoryRestoreFromLimbo(ActorCommonData & tACDItem, InventoryLocation & tLocation);  // X/Common/ActorInventory.cpp

static void sSetInfoForACD(HirelingInfo & tInfo, ActorCommonData & tACD)
{



    tACD.pRareNameGBIDs[0] = tInfo.gbidName;



    for (uint32 i = 0; i < NUM_HIRELING_POWER_SLOTS; i++)
        if (tInfo.arnPowerKeyParam[i] != -1)
            tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, tInfo.arnPowerKeyParam[i]), 1);




    ACDID idACDOwner = ACDGetPetOwner(tACD.id);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner)
    {

        int32 nAdjustedLevel = ptACDOwner->AttributesGetInt(ATTRIBUTE_LEVEL);
        ExperienceSetLevel(tACD.id, nAdjustedLevel, FALSE);
    }


    tACD.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
}

static ACDID sRemoveHirelingItem(ACDID idACDHireling, InventorySlotType eSlot, const IVector2D & pt)
{

    ActorCommonData & tACDHireling = ACDGet(idACDHireling);



    ACDID idACDItem = ACDInventoryGetSquareItem(tACDHireling, eSlot, pt);
    if (idACDItem == ACDID_INVALID)
    {
        return ACDID_INVALID;
    }

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
    {
        return ACDID_INVALID;
    }
    ACDInventoryRemoveToLimbo(idACDHireling, *ptACDItem, FALSE);




    return ptACDItem->id;
}



static void sMoveItemsToItemStore(HirelingItemStore & tStore, ACDID idACDHireling)
{




    for (int32 i = 0; i < NUM_HIRELING_SLOTS; i++)
    {




        tStore.idACDItems[i] = sRemoveHirelingItem(idACDHireling, (InventorySlotType)(SLOT_HIRELING_START + i), cgptNull);
    }
}


static void sSetItemsForACD(HirelingItemStore & tItems, ActorCommonData & tACDHireling)
{




    for (int32 i = 0; i < NUM_HIRELING_SLOTS; i++)
    {
        InventoryLocation tLocation;

        ACDInventoryLocationSet(tLocation, tACDHireling.id, (InventorySlotType)(SLOT_HIRELING_START + i), 0, 0);


        ACDID idACDItem = ACDInventoryGetSquareItem(tACDHireling, (InventorySlotType)(SLOT_HIRELING_START + i), tLocation.pt);
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (ptACDItem)
        {

            ACDInventoryDelete(*ptACDItem, TRUE);
        }



        if (tItems.idACDItems[i] != ACDID_INVALID)
        {

            ActorCommonData & tACDItem = ACDGet(tItems.idACDItems[i]);

            ACDInventoryRestoreFromLimbo(tACDItem, tLocation);


            tItems.idACDItems[i] = ACDID_INVALID;
        }
    }



    tACDHireling.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
}
void SHirelingMoveToStable(ACDID idACDPlayer, ActorCommonData & tACDHireling, BOOL bPlayerDismissed)
{



    BOOL bProxy = ACDIsHirelingProxy(tACDHireling);


    Player * ptPlayer = PlayerGetByACD(idACDPlayer);



    HirelingClass eHireling = HirelingGetClass(tACDHireling);



    HirelingData * ptData = &ptPlayer->tHirelingStable.tHireling[eHireling];


    HirelingDataDispose(*ptData);
    HirelingDataInit(*ptData);


    sGetInfoFromACD(ptData->tInfo, tACDHireling);


    sMoveItemsToItemStore(ptData->tItems, tACDHireling.id);


    ptData->tInfo.fDead = FALSE;


    if (!bProxy)
    {
        SHirelingSetActive(idACDPlayer, HC_NONE, bPlayerDismissed);


        ActorRefreshHirelingKnownActors(eHireling);
    }



    SHirelingSendUpdate(idACDPlayer, eHireling);
}
SNO HirelingGetSNO(HirelingClass eClass, HirelingType eType);  // X/Common/Hireling.cpp
ACDID SpellEffect_SummonHirelingGuts(ACDID idACDPlayer, HirelingClass eClass, const PowerAPISummonActor & tSummonInfo, int32 nUnknown);  // X/Server/Powers/SpellEffect.cpp
int32 SPlayerAddPet(ACDID idACDPlayer, PetType eType, ACDID idACDPet, BOOL fSendNow, int32 nMaxCount, float (*pCompareFunc)(ACDID));  // X/Server/SPlayer.cpp

static void sHirelingSetLevelAttributes(ActorCommonData & tACDHireling, int32 nLevel)
{



    if (nLevel >= 1 && nLevel <= 70)
    {

        HirelingClass eClass = HirelingGetClass(tACDHireling);
        if (eClass != HC_NONE)
        {

            ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
            const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
            float flDamageReduction = tExpTable.ptExperienceLevels[nLevel - 1].arHirelingDamageAbsorbPercent[eClass];

            tACDHireling.AttributesSetFloat(ATTRIBUTE_DAMAGE_ABSORB_PERCENT_ALL, flDamageReduction);
        }
    }

    ScopedGlobalDefinitionLockR ptGlobals;
    tACDHireling.AttributesSetFloat(ATTRIBUTE_CORE_ATTRIBUTES_FROM_ITEM_BONUS_MULTIPLIER, ptGlobals->ptServerData->flHirelingCoreAttributeBonusMultiplier);
}



ACDID SHirelingLoadFromStable(ACDID idACDPlayer, HirelingClass eClass, const WorldPlace & placeSummonLocation, const Quaternion & qOrientation, HirelingType eType)
{





    Player * ptPlayer = PlayerGetByACD(idACDPlayer);




    HirelingData * ptData = &ptPlayer->tHirelingStable.tHireling[eClass];


    SNO sno = HirelingGetSNO(eClass, eType);




    PowerAPISummonActor tSummonInfo;
    SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));
    tSummonInfo.snoActor = sno;
    tSummonInfo.nActorLevel = 1;
    tSummonInfo.placeSummonLocation = placeSummonLocation;
    tSummonInfo.qOrientation = qOrientation;
    tSummonInfo.eTeamOverride = -1;

    ACDID idACDHireling = SpellEffect_SummonHirelingGuts(idACDPlayer, eClass, tSummonInfo, 5);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling)
    {
        SPlayerAddPet(idACDPlayer, (eType == HT_NORMAL) ? PETTYPE_HIRELING : PETTYPE_HIRELING_PROXY, idACDHireling, FALSE, -1, NULL);


        sSetInfoForACD(ptData->tInfo, *ptACDHireling);


        sSetItemsForACD(ptData->tItems, *ptACDHireling);

        sHirelingSetLevelAttributes(*ptACDHireling, ptACDHireling->AttributesGetInt(ATTRIBUTE_LEVEL));
    }

    return idACDHireling;
}



ACDID SHirelingLoadFromStable(ACDID idACDPlayer, HirelingClass eClass)
{


    ACDID idACDCurrentHireling = PlayerGetHireling(idACDPlayer);
    if (idACDCurrentHireling != ACDID_INVALID)
    {
        return idACDCurrentHireling;
    }


    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (!ptPlayer || !SHirelingCanHire(*ptPlayer))
    {
        return ACDID_INVALID;
    }

    PowerAPIChooseOpenGround tOpenGroundInfo;
    SigmaMemoryClear(&tOpenGroundInfo, sizeof(tOpenGroundInfo));
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    PowerGetNavPlaceFromACD(&tACDPlayer, tOpenGroundInfo.tNavPlace);

    float flScale = 1.0f;
    SNO sno = HirelingGetSNO(eClass, HT_NORMAL);


    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY))
    {

        return SHirelingLoadFromStable(idACDPlayer, eClass, tACDPlayer.GetPlace(), cgqIdentity, HT_NORMAL);
    }

    NavMeshHandle tNavMeshHandleResult;
    WorldPlace placeSummonLocation;
    if (!PowerAPI_DoChooseOpenGround(sno, flScale, tOpenGroundInfo, placeSummonLocation, tNavMeshHandleResult))
    {
        return ACDID_INVALID;
    }

    return SHirelingLoadFromStable(idACDPlayer, eClass, placeSummonLocation, cgqIdentity, HT_NORMAL);
}


void SHirelingSaveStableInfo(Player & tPlayer)
{


    ACDID idACDHireling = PlayerGetHireling(tPlayer.idPlayerACD);
    if (idACDHireling != ACDID_INVALID)
    {
        ActorCommonData & tACDHireling = ACDGet(idACDHireling);
        HirelingClass eClass = HirelingGetClass(tACDHireling);


        sGetInfoFromACD(tPlayer.tHirelingStable.tHireling[eClass].tInfo, tACDHireling);
    }



    tPlayer.tHeroStateData.tSavedData.tHirelingSavedData.eActiveHireling = tPlayer.tHirelingStable.eActiveHireling;


    for (int32 i = 0; i < NUM_HIRELING_CLASSES; i++)
    {
        ACDID idACDHirelingProxy = PlayerGetHirelingProxy(tPlayer.idPlayerACD, (HirelingClass)i);
        if (idACDHirelingProxy != ACDID_INVALID)
        {

            ActorCommonData & tACDHirelingProxy = ACDGet(idACDHirelingProxy);


            sGetInfoFromACD(tPlayer.tHirelingStable.tHireling[i].tInfo, tACDHirelingProxy);
        }
    }


    for (int32 i = 0; i < NUM_HIRELING_CLASSES; i++)
    {
        SigmaMemoryCopy(&tPlayer.tHeroStateData.tSavedData.tHirelingSavedData.tHireling[i], &tPlayer.tHirelingStable.tHireling[i], sizeof(HirelingInfo));
    }
}






void SHirelingRestoreStableInfo(Player & tPlayer)
{
    for (int32 i = 0; i < 4; i++)
    {
        SigmaMemoryCopy(&tPlayer.tHirelingStable.tHireling[i].tInfo, &tPlayer.tHeroStateData.tSavedData.tHirelingSavedData.tHireling[i], sizeof(HirelingInfo));
    }
    tPlayer.tHirelingStable.eActiveHireling = tPlayer.tHeroStateData.tSavedData.tHirelingSavedData.eActiveHireling;
    tPlayer.uConsoleSyncFlags |= (1 << PCSF_ACTIVE_HIRELING);
}


#include "../Common/Definitions/SkillKitDefinition.h"
extern const uint32 MAX_FOLLOWER_SWAP_TIME;  // defined in another file (data symbol 0x7100e49108)
BOOL HirelingCanRetrain(ActorCommonData & tACDHireling, ActorCommonData & tACDPlayer);  // X/Common/Hireling.cpp
void SHirelingClearSkills(ActorCommonData & tACDHireling);
void SHirelingRespec(ActorCommonData & tACDHireling);
void SGameStatsPlayerTimedEventFinish(Player * ptPlayer, int32 eStat, int32 nParam1, int32 nParam2);  // X/Server/SGameStats.cpp
void ExperienceUpdatePerLevelHitpointFactorVitality(ActorCommonData & tACD);  // X/Server/Experience.cpp:1293
void SGameStatsPlayerTimedEventStart(Player * ptPlayer, int32 eStat, int32 nParam1, int32 nParam2, int32 nParam3);  // X/Server/SGameStats.cpp
void ACDInventoryEquipmentOrSkillChanged(ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:9033


void SHirelingSendStableInfoToClient(Player & tPlayer)
{
    for (int32 i = 1; i < 4; i++)
    {
        SHirelingSendUpdate(tPlayer.idPlayerACD, i);
    }
}






void SHirelingSendUpdate(ACDID idACDPlayer, HirelingClass eClass)
{


    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        HirelingData * ptData = &ptPlayer->tHirelingStable.tHireling[eClass];







        HirelingInfoUpdateMessage tMessage;
        SigmaMemoryClear(&tMessage, sizeof(tMessage));
        tMessage.eClass = eClass;
        tMessage.fDead = ptData->tInfo.fDead;
        tMessage.gbidName = ptData->tInfo.gbidName;
        tMessage.eValidClass = ptData->tInfo.eClass;

        SigmaMemoryCopy(tMessage.arnPowerKeyParam, ptData->tInfo.arnPowerKeyParam, sizeof(tMessage.arnPowerKeyParam));


        for (uint32 i = 0; i < NUM_HIRELING_SLOTS; i++)
            tMessage.annItems[i] = ANNFromACD(ptData->tItems.idACDItems[i]);


        tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;


        PlayerList listDestPlayer(PlayerListGetElementPool());
        PlayerListGetSingle(listDestPlayer, *ptPlayer);


        MessageSendToClient(listDestPlayer, 0xb5, &tMessage, sizeof(tMessage));  // 181: SMSG_SET_HIRELING_DEAD in the DWARF enum
    }


}
static inline void sHirelingAttributeInc(ActorCommonData & tACDHireling, Attrib eAttrib, int32 nAmount)
{




    float flOldValue = tACDHireling.AttributesGetBaseFloat(eAttrib);
    tACDHireling.AttributesSetFloat(eAttrib, flOldValue + nAmount);
}


static inline void sHirelingAttributeAward(ActorCommonData & tACDHireling, int32 nLevel)
{



    if (nLevel < 1 || nLevel > 70)
        return;


    HirelingClass eClass = HirelingGetClass(tACDHireling);

    if (eClass == HC_NONE)
        return;




    const ExperienceTable & tExpTable = ScopedGameBalanceDefinitionLockR(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE))->tExperienceTable;


    const AttributeAwardDefinition & tAward = tExpTable.ptExperienceLevels[nLevel - 1].tHirelingAttributeAward[eClass];
    sHirelingAttributeInc(tACDHireling, ATTRIBUTE_STRENGTH, tAward.nStrength);
    sHirelingAttributeInc(tACDHireling, ATTRIBUTE_DEXTERITY, tAward.nDexterity);
    sHirelingAttributeInc(tACDHireling, ATTRIBUTE_INTELLIGENCE, tAward.nIntelligence);
    sHirelingAttributeInc(tACDHireling, ATTRIBUTE_VITALITY, tAward.nVitality);
}



void SHirelingLevelUp(ActorCommonData & tACDHireling, int32 nLevel)
{


    tACDHireling.AttributesSetInt(ATTRIBUTE_LEVEL, nLevel);


    sHirelingAttributeAward(tACDHireling, nLevel);

    ExperienceUpdatePerLevelHitpointFactorVitality(tACDHireling);



    SNOName tMonsterSNOName;
    BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tACDHireling.snoActor, tMonsterSNOName);
    if (bHasMonsterSNO)
    {






        SNO snoMonster = tMonsterSNOName.snoHandle;

        float flHPMin = GetHirelingMonsterLevelAttribute(nLevel, snoMonster, GBA_HITPOINTS, GBA_HIRELING_HITPOINTS);
        float flHPDelta = GetHirelingMonsterLevelAttribute(nLevel, snoMonster, GBA_HPRANGE, GBA_HIRELING_HPRANGE);
        float flHitPoints = flHPMin + flHPDelta * 0.5f;

        tACDHireling.AttributesSetFloat(ATTRIBUTE_HITPOINTS_MAX, flHitPoints);

        float flHPRegen = GetHirelingMonsterLevelAttribute(nLevel, snoMonster, GBA_HPREGEN, GBA_HIRELING_HPREGEN);
        tACDHireling.AttributesSetFloat(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND, flHPRegen);

        sHirelingSetLevelAttributes(tACDHireling, nLevel);
    }
}
#include "LuaScript/ScriptEventParams.h"
#include "LuaScript/ScriptEventSwig.h"
#include "Achievements/AchievementModifierEvaluationLogic.h"
#include "../Common/Achievements/AchievementDefines.h"
void NPCDismiss(ActorCommonData & tACDPlayer, BOOL bPlayerDismissed, BOOL bDisplayChatMessage);  // X/Server/SNPC.cpp:774
void NPCDismissProxy(ActorCommonData & tACDPlayer, HirelingClass eClass);  // X/Server/SNPC.cpp:793
ACDID SpellEffect_SummonHireling(ACDID idACDOwner, PowerAPISummonActor & tSummonInfo, HirelingType eType);  // X/Server/Powers/SpellEffect.cpp:377
struct HirelingSwapMessage {  // size 0xc; DWARF X/Common/Hireling.h:71 (same local copy in LuaScript/ScriptUtil.cpp)
    RequiredMessageHeader tHeader;  // 0x00
    HirelingClass eNewClass;  // 0x08
};
static_assert(sizeof(HirelingSwapMessage) == 0xc, "HirelingSwapMessage");












void SHirelingClearSkills(ActorCommonData & tACDHireling)
{
    AttribGroupID idFastAttrib = tACDHireling.AttributesGetAttributeGroup();
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey tKey = tIt.tEntryBuffer.tKey;
        if (KeyGetAttrib(tKey) == ATTRIBUTE_SKILL)
        {
            tACDHireling.AttributesSetInt(tKey, 0);
        }
    }
    tACDHireling.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, GlobalSNOGet(GLOBALSNO_POWERS_PUNCH)), 1);
}









void SHirelingHandleSwap(Player & tPlayer)
{



    if (tPlayer.eRequestedHirelingClass == 0)
        return;





    uint32 dwNow = GameGetServerTime();
    if (tPlayer.dwRequestedHirelingTimeStamp + MAX_FOLLOWER_SWAP_TIME < dwNow)
        return;




    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
        return;

    if (ptACDPlayer->IsDueling())
        return;




    ActorCommonData * ptACDLocation = ACDTryToGet(tPlayer.idRequestedHirelingLocation);
    SHirelingCreate(*ptACDPlayer, ptACDLocation ? *ptACDLocation : *ptACDPlayer, tPlayer.eRequestedHirelingClass, TRUE, 0);
}

ACDID SHirelingCreate(
    ActorCommonData & tACDPlayer,
    ActorCommonData & tACDNextTo,
    const HirelingClass eHirelingClass,
    BOOL bForceCreate,
    HirelingType eType)
{
    Player *ptPlayer = PlayerGetByACD(tACDPlayer.id);
    ACDID idACDCurrentHireling = PlayerGetHireling(tACDPlayer.id);
    ActorCommonData * ptACDCurrentHireling = ACDTryToGet(idACDCurrentHireling);
    HirelingClass eClass = ptACDCurrentHireling ? HirelingGetClass(*ptACDCurrentHireling) : HC_NONE;

    // inferred: invalid-class test (line 655 col 36 is the && / || operator)
    if (eHirelingClass < HC_FIRST || eHirelingClass >= NUM_HIRELING_CLASSES)
    {
        if (eType == HT_NORMAL)
        {
            if (!bForceCreate && idACDCurrentHireling != ACDID_INVALID)
            {
                PlayerList listDestPlayers(PlayerListGetElementPool());
                listDestPlayers.AddTail_Tracked(ptPlayer);

                HirelingSwapMessage tMessage;
                tMessage.eNewClass = eClass;
                MessageSendToClient(listDestPlayers, SMSG_HIRELING_AUTO_DISMISS, &tMessage, sizeof(tMessage));
            }

            NPCDismiss(tACDPlayer, FALSE, !bForceCreate);


            ptPlayer->eRequestedHirelingClass = HC_NONE;
        }
        // inferred: shared tail mov w25,#-1 (+0x868), no row of its own
        return ACDID_INVALID;
    }


    if (!HirelingTestTypeAvailable(tACDPlayer.id, eHirelingClass))
    {

        HirelingSwapMessage tMessage;
        tMessage.eNewClass = eHirelingClass;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_HIRELING_UNLOCKED, &tMessage, sizeof(tMessage));
    }

    HirelingSetTypeAvailable(tACDPlayer.id, eHirelingClass, TRUE);



    if (eClass == eHirelingClass)
        return idACDCurrentHireling;



    if (eType == HT_NORMAL)
    {

        if (PlayerGetNumPlayers() > 1)
        {


            if (!bForceCreate)
            {
                PlayerList listDestPlayers(PlayerListGetElementPool());
                listDestPlayers.AddTail_Tracked(ptPlayer);

                HirelingSwapMessage tMessage;
                tMessage.eNewClass = eHirelingClass;
                MessageSendToClient(listDestPlayers, SMSG_HIRELING_AUTO_DISMISS, &tMessage, sizeof(tMessage));
            }
            return ACDID_INVALID;
        }
    }

    if (!bForceCreate && eType == HT_NORMAL)
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);



        ptPlayer->idRequestedHirelingLocation = tACDNextTo.id;
        ptPlayer->eRequestedHirelingClass = eHirelingClass;
        ptPlayer->dwRequestedHirelingTimeStamp = GameGetServerTime();

        HirelingSwapMessage tMessage;
        tMessage.eNewClass = eHirelingClass;
        MessageSendToClient(listDestPlayers, SMSG_CONFIRM_HIRELING_SWAP, &tMessage, sizeof(tMessage));
        return ACDID_INVALID;
    }

    if (eType == HT_NORMAL)
    {

        ptPlayer->idRequestedHirelingLocation = ACDID_INVALID;
        ptPlayer->eRequestedHirelingClass = HC_NONE;
        ptPlayer->dwRequestedHirelingTimeStamp = 0;
    }
    else
    {


        ACDID idACDCurrentProxy = PlayerGetHirelingProxy(tACDPlayer.id, eHirelingClass);
        if (idACDCurrentProxy != ACDID_INVALID)
            return idACDCurrentProxy;
    }

    // inferred: second eType test (jump-threaded into the one at line 736; NPCDismiss rows at 756+ follow line 747)
    if (eType == HT_NORMAL)
    {

        NPCDismiss(tACDPlayer, TRUE, TRUE);


        NPCDismissProxy(tACDPlayer, eHirelingClass);







        // inferred scope: tEvaluationData block range excludes NPCDismiss; tab-indented (cols 32/20, dtor col 3)
		{
			Achievement::EvaluationData tEvaluationData;
			tEvaluationData.SetPlayer(ptPlayer);

			tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_ACQUIRE_HIRELING, eHirelingClass, 1);




		}
    }


    ACDID idACDHireling;

    WorldPlace placeSummonLocation;
    Quaternion qOrientation;
    NavMeshHandle tNavMeshHandle;

    ACDGetNavMeshHandleWithPlace(tACDNextTo, tNavMeshHandle, placeSummonLocation);
    qOrientation = tACDNextTo.GetOrientation();



    WorldPlace tPlacePlayer = tACDPlayer.GetPlace();
    if (!WorldPlaceInSameWorld(tPlacePlayer, placeSummonLocation))
    {
        placeSummonLocation = tPlacePlayer;
    }

    if (HirelingTestValid(tACDPlayer.id, eHirelingClass))
    {
        idACDHireling = SHirelingLoadFromStable(tACDPlayer.id, eHirelingClass, placeSummonLocation, qOrientation, eType);
    }
    else
    {
        PowerAPISummonActor tSummonInfo;

        SigmaMemoryClear(&tSummonInfo, sizeof(tSummonInfo));

        tSummonInfo.placeSummonLocation = placeSummonLocation;
        tSummonInfo.qOrientation = qOrientation;
        tSummonInfo.eTeamOverride = -1;  // TEAM_INVALID not visible here; sibling SHirelingLoadFromStable writes -1


        HirelingSetTypeToCreate(tACDPlayer.id, eHirelingClass);

        idACDHireling = SpellEffect_SummonHireling(tACDPlayer.id, tSummonInfo, eType);
    }

    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling)
    {
        int32 nLevel = ptACDHireling->AttributesGetInt(ATTRIBUTE_LEVEL);
        sHirelingSetLevelAttributes(*ptACDHireling, nLevel);

        ACDIDParam tParam1(idACDHireling);
        ACDIDParam tParam2(tACDPlayer.id);
        ScriptEventParamList tEventDefParams(&tParam1, &tParam2);
        ScriptEventDispatchEvent(EVENT_HIRELING_HIRED, &tEventDefParams, NULL, NULL, 0.0f, ACDID_INVALID);
    }

    return idACDHireling;
}



void SHireling_TryRetrain(Player & tPlayer)
{

    ACDID idACDHireling = PlayerGetHireling(tPlayer.idPlayerACD);
    ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
    if (ptACDHireling == NULL)
        return;




    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer == NULL)
        return;





    if (HirelingCanRetrain(*ptACDHireling, *ptACDPlayer))
    {





        SHirelingRespec(*ptACDHireling);
    }
}










void SHirelingRespec(ActorCommonData & tACDHireling)
{



    SHirelingClearSkills(tACDHireling);


    tACDHireling.ApplyAllPassives(FALSE);


    ScriptEventOnUserEventSentToActor("HirelingRetrain", tACDHireling.id, tACDHireling.id, 0);
}




void SHirelingSetActive(ACDID idACDPlayer, HirelingClass eClass, BOOL bSave)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {



        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    if (ptPlayer->tHirelingStable.eActiveHireling != eClass)
    {

        SGameStatsPlayerTimedEventFinish(PlayerGetByACD(idACDPlayer), STAT_COUNTER_HIRELING_UPTIME,

                                         ptPlayer->tHirelingStable.eActiveHireling, 0);


        SGameStatsPlayerTimedEventStart(PlayerGetByACD(idACDPlayer), STAT_COUNTER_HIRELING_UPTIME, eClass, 0, TRUE);
    }







    if (bSave)
    {

        ptPlayer->tHirelingStable.eActiveHireling = eClass;


        ptPlayer->uConsoleSyncFlags |= (1 << PCSF_ACTIVE_HIRELING);
    }
}





static void sUnlearnConflictingPower(ActorCommonData & tACDHireling, SNO snoPower)
{


    SNO snoSkillKit = tACDHireling.GetSkillKit();
    if (snoSkillKit == SNO_NONE)
        return;
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    if (pdefSkillKit)
    {
        int32 nMasteryReq = -1;
        BOOL bFound = FALSE;
        int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
        for (int32 i = 0; i < nCount; i++)
        {

            if (pdefSkillKit->arActiveSkillEntries[i].snoPower == snoPower && !bFound)
            {








                nMasteryReq = pdefSkillKit->arActiveSkillEntries[i].nMasteryReq;
                bFound = TRUE;
            }
        }

        if (!bFound || nMasteryReq < 0)
            return;











        for (int32 i = 0; i < nCount; i++)
        {
            ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
            if (tEntry.snoPower != snoPower && tEntry.nMasteryReq == nMasteryReq)
            {

                tACDHireling.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, tEntry.snoPower), 0);
            }
        }
    }
}





BOOL SHirelingTryChooseSkill(ActorCommonData & tACDHireling, SNO snoPower)
{



    if (snoPower == SNO_NONE)
        return FALSE;

    {
        HirelingTrainReason eReason = HirelingCanTrainSkillReason(tACDHireling, snoPower);
        if (eReason == HIRELING_TRAIN_CONFLICTING_SKILL_PURCHASED)
        {


            sUnlearnConflictingPower(tACDHireling, snoPower);
            eReason = HirelingCanTrainSkillReason(tACDHireling, snoPower);
        }

        if (eReason != HIRELING_TRAIN_OK)
            return FALSE;

        tACDHireling.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), 1);
        ACDInventoryEquipmentOrSkillChanged(tACDHireling);
        {
            HirelingClass eClass = HirelingGetClass(tACDHireling);
            SGameStatsPlayerCounterIncrement(tACDHireling.GetPetOwnerPlayer(), STAT_COUNTER_SPENT_HIRELING_SKILL_POINT, snoPower, eClass, 1.0f);
        }

        ScriptEventOnHirelingLearnedSkill(tACDHireling.id, snoPower);
    }

    return TRUE;
}
ACDID SHirelingGetNearby(ACDID idACDOrigin, float flRangeSq, HirelingClass eHirelingClass)
{

    ACDID idValidHirelings[4];

    if (flRangeSq < 1e-6f)
        flRangeSq = 3600.0f;

    ActorCommonData & tACDOrigin = ACDGet(idACDOrigin);
    int32 nCount = 0;
    const WorldLocation & tWorldLocOrigin = tACDOrigin.GetWorldLocation();

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        ACDID idACDHireling = PlayerGetHireling(ptPlayer->idPlayerACD);
        ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);

        if (ptACDHireling && ptACDHireling->IsAlive())
        {
            if (eHirelingClass != HC_NONE && HirelingGetClass(*ptACDHireling) != eHirelingClass)
                continue;

            const WorldLocation & tWorldLocHireling = ptACDHireling->GetWorldLocation();
            if (tWorldLocOrigin.idSWorld == tWorldLocHireling.idSWorld)
            {

                if (ACDGetDistanceSquared(tACDOrigin, *ptACDHireling, FALSE) < flRangeSq)
                {

                    idValidHirelings[nCount++] = ptACDHireling->id;
                }
            }
        }
    }

    if (nCount > 0)
    {

        int32 nIndex = Rand_Internal(SGameGlobalsGet().rng, nCount - 1);
        return idValidHirelings[nIndex];
    }

    return ACDID_INVALID;
}








void SHirelingRehireAfterPlayerLeft()
{
    if (!GameIsWeeklyChallenge() && PlayerGetNumPlayers() == 1 && !GameTypeIsPvP())
    {
        Player * ptPlayer = PlayerGetFirstInGame();
        if (ptPlayer && SHirelingCanHire(*ptPlayer))
        {
            HirelingClass eClass = ptPlayer->tHirelingStable.eActiveHireling;
            if (eClass != 0)
            {
                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer)
                {
                    SHirelingCreate(*ptACDPlayer, *ptACDPlayer, eClass, TRUE, 0);
                }
            }
        }
    }
}
















BOOL SHirelingCanHire(const Player & tPlayer)
{

    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (ptACDPlayer)
    {
        if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN) == 0)
        {




            if (!ptACDPlayer->IsDueling())
            {




                if (PlayerGetNumPlayers() == 1)
                {
                    if (!GameTypeIsPvP())
                    {
                        if (ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER) == 0)
                        {
                            return TRUE;
                        }
                    }
                }
            }
        }
    }

    return FALSE;
}
