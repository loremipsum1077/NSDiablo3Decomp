// X/Common/ActorCommonData.cpp
// Functions sit on their original line numbers.

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "ActorCommonData.h"
#include "CommonTypes.h"
#include "Player.h"
#include "TeamCommon.h"
#include "Attributes/FastAttribute.h"
#include "ActorCommonDataSwig.h"
#include "Definitions/ActorDefinition.h"
#include "Game.h"
#include "SceneCommon.h"
#include "SNO/SNOFiles.h"
#include "Gizmo.h"
#include "GlobalSNO.h"
#include "Nav/NavigationMesh.h"
#include "TagMaps/Tags.h"
#include "../Server/Powers/PowerAPI.h"
#include "../Server/Powers/Powers.h"
#include "HirelingSwig.h"
#include "Monsters.h"
#include "NephalemRiftCommon.h"
#include "NPC.h"
#include "World.h"
#include "Networking/Network.h"
#include "Actors/ActorEffectCommon.h"
#include "Attributes/FastAttribConstants.h"
#include "../Server/SPlayer.h"
#include "GizmoSwig.h"
#include "Powers/PowerAPISwig.h"
#include "TagMaps/ActorTags.h"
#include "../Sigma/Math/Quaternion.h"
#include "Definitions/SkillKitDefinition.h"
#include "../Client/CPlayerInput.h"
#include "../Server/Actors/ActorEffect.h"
#include "../Server/Actors/Actors.h"
#include "../Sigma/Math/MathGeneral.h"
#include "Definitions/LevelAreaDefinition.h"
#include "Shell/CmdLine.h"
#include "Console/Avenger.h"
#include "Definitions/EncounterDefinition.h"
#include "Definitions/GameBalanceDefinition.h"
#include "../Server/Actors/SComplexEffect.h"
#include "GameSwig.h"
#include "ActorCommon.h"
#include "../Sigma/MemoryPools.h"
#include "Definitions/MonsterDefinition.h"
#include "Items.h"
#include "ActorInventory.h"
#include "Networking/GameMessage.h"
#include "../Server/PlayerList.h"
#include "PortalCommon.h"
#include "VisualInventory.h"
#include "Attributes/AttributeCompressor.h"
#include "../Client/Definitions/AppearanceDefinition.h"
#include "TagMaps/AnimTagsSwig.h"
#include <float.h>
#include "../Server/LuaScript/ScriptEvent.h"
#include "../Client/EffectGroup.h"
#include "../Server/AI/BrainStance.h"
#include "AnimPlayer.h"
#include "Definitions/AnimationDefinition.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Server/SGame.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../Client/RActors/RActors.h"
#include "../Client/Movement/CActorMovement.h"
#include "../Server/Movement/SActorMovement.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
BOOL sACDSetGroup(ActorCommonData & tACD, uint32 uNewGroup, POS & posGroupCur, uint32 & uACDGroupCur, const BOOL fModifyLiveActorCount, const ACDID idACDKiller);
void ACDPackDelete(ACDPack & tACDPack);
void AnimPlayerGetCurrentAnim(const ActorCommonData & tACD, SNO & snoAnim, int32 & nAnimPermutation);  // X/Common/AnimPlayer.cpp:1980
void ACDInitializeForGame();
void ACDsIntersectCapsule(SWorldID idSWorld, const Capsule & capsule, XPooledDataIDList & listACDs, BOOL bForPower);  // X/Common/Actors/ActorCollision.cpp:518
BOOL ItemIsAutoPickup(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:4183
WorldDistance AnimSNOGetWalkingSpeed(SNO snoAnim, float flSpeedFactor);  // X/Common/AnimPlayer.cpp:2101
#include "Powers/PowersCommon.h"
WalkType AnimTagGetWalkType(uint32 dwAnimTag);  // ActorCommonData.cpp:5425
enum KnownEntryType { KET_ACD = 8 };  // X/Server/Network/KnownList.h
void KnownListItemChanged(KnownEntryType eType, DataID idObject, BOOL bCheckForDuplicate);  // KnownList.cpp:1180
ACDID ACDInventoryGetUltimateContainer(const ActorCommonData & tACDItem);  // X/Common/ActorInventory.cpp:4224
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);  // X/Common/Items.cpp:1883
BOOL ItemUsableByHireling(const GBID gbidItem, const HirelingClass eHirelingClass);  // X/Common/Items.cpp:2074
void ServerQueueClientUpdate();  // X/Server/Network/Server.cpp:438
HirelingClass HirelingGetClass(const ActorCommonData & tACDHireling);  // X/Common/Hireling.h (Hireling.cpp:222)
void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwSphereTreeFlags);  // X/Common/Actors/ActorCollision.cpp:317 (no ActorCollision.h in src yet)
ACDID ExperienceGetUltimateOwner(const ActorCommonData & tACD);  // X/Server/Experience.cpp:840 (no Experience.h in src yet)
#include "../Server/Achievements/AchievementModifierEvaluationLogic.h"
#include "Achievements/AchievementDefines.h"
void PowerAPI_DoFindAllPossibleTargets(const PowerAPIChooseTargets & tTargetingInfo, XPooledDataIDList & listTargets); BOOL ItemIsGlyph(const ActorCommonData & tACDItem); void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer); namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); } namespace Console { namespace NFP { Player * GetPlayerByScanningPortal(ACDID idACDScanningPortal); } }  // X/Client/Console/NX64NFP.cpp:2737, X/Server/Powers/PowerAPI.cpp:6247, X/Common/Items.cpp:4252 (not yet declared in a header)
typedef int32 FlippyID; void FlippyDestroy(FlippyID idFlippy); void AnimPlayerDispose(ActorCommonData & tACD); void ACDInventoryDispose(ActorInventory *& ptInventory); void ACDVisualInventoryDispose(VisualInventory *& ptVisualInventory); void SScenePutACDInScene(SSceneID idSScene, ACDID idACD, BOOL bAdd); struct SScene; SScene * SSceneTryToGet(SSceneID idSScene); void PortalSpecifierDispose(ACDID idACD); void CComplexEffectRemoveAll(ActorCommonData & tACD); void SComplexEffectRemoveAll(const ACDID idACD); void ACDCollDisposeSphereNode(ActorCommonData & tACD); void LuaScriptDisposeACDVariableList(ACDID idACD); void ActorInventoryIterateGetItemPassivePowers(ACDID idACDSquare, InventorySlotType, void * plistPassiveSNOs); void SPlayerApplyPerkBuffs(ActorCommonData & tACDPlayer); void SPlayerApplyParagonBuff(ActorCommonData & tACDPlayer); void SPlayerRegisterPowerForRecharge(Player * ptPlayer, SNO snoPower); SNO ItemGetLegendaryPower(GBID gbidItem); void SPlayerApplyEquippedLegendaryPowerBuffs(ActorCommonData & tACDPlayer);  // ActorInventory.cpp:12255, SPlayer.cpp:11755, SPlayer.cpp:11975, Flippy.cpp:551, AnimPlayer.cpp:682, ActorInventory.cpp:1525, VisualInventory.cpp:254, SScene.cpp:513, PortalCommon.cpp:122, CComplexEffect.cpp:337, SComplexEffect.cpp:338, ActorCommonCollision.cpp:172, LuaScript.cpp:3775
#include "../Client/CWorld.h"
#include "../Server/Powers/PowerHelper.h"
#include "../Server/Timers.h"
#include "../Server/SGameManager.h"
struct EffectGroup;  // X/Client/EffectGroup.h
EffectGroup * EffectGroupTryToGet(EffectGroupID idEffectGroup);  // X/Client/EffectGroup.cpp
EffectGroupID EffectGroupCreateTracked(SNO snoEffectGroup, RActorID idRActorSource, RActorID idRActorTarget, int32 nUnused0, int32 nUnused1);  // X/Client/EffectGroup.cpp
void EffectGroupOutro(EffectGroupID idEffectGroup);  // X/Client/EffectGroup.cpp
void SQuestsOnGroupKilled(int32 nGroup);  // X/Server/Quests/SQuests.cpp:4620
void SQuestsOnGroupModified(int32 nGroup, int32 nCount);  // X/Server/Quests/SQuests.cpp:4631
float PowerComputeResistanceDamageMitigation(float flResistanceTotal, int32 nAttackerLevel); float PowerComputeDodgeChance(const ActorCommonData & tACDDefender, int32 nLevel, int32 eDeliveryType, BOOL bUsedForToughness); float PowerComputeArmorDamageMitigation(const ActorCommonData & tACDTarget, const int32 nAttackerLevel);  // PowersCommon.cpp:3760, 3776, 3819
void PowerActorGetWeaponDamageAndAttackSpeed(const ActorCommonData & tACD, float & flDamage, float & flAttackSpeed, const BOOL bWantBonuses, const PowerID idPower, const BOOL bUsedForDPS); ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD); BOOL ACDCanDualWield(const ActorCommonData & tACD);  // PowersCommon.cpp:4391, ActorInventory.cpp:5373, 11419
void ProximityDetectorEnabledStatusChanged(ActorID idActor, BOOL bEnabled); void BrainAPIDoIdle(ActorID idActor);  // X/Server/AI/ProximityDetectorBrain.cpp:1020, X/Server/AI/BrainAPI.cpp:3448
namespace Minimap { void ActivateMarker(const ACDID idACD); void DeactivateMarker(const ACDID idACD, BOOL bFromScript); }  // X/Server/Map/Minimap.cpp:1214, 1308
#include "Nav/NavZone.h"
#include "WorldCommonCollision.h"
#include "../Sigma/BlzExt/BlzVectorHelper.h"
// DWARF definition: X/Server/Experience.cpp:882; no Experience.h declaration was recovered.
extern int32 ExperienceGetPlayersEligibleForKill(ACDID idACDVictim, PlayerList & listPlayersToEarnXP);
#include "../Server/Network/ServerMessageSend.h"
#include "../Server/Network/KnownMessage.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Client/Gfx/GfxVB.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../Server/PVP.h"
#include "Actors/ActorCollision.h"
#include "../Server/SWorld.h"





























static const TypeDescriptorField RareMonsterNamesMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RareMonsterNamesMessage, tHeader), NULL, (1 << TDF_XML_SERIALIZATION)),
    TypeDescriptorField("", DT_INT, offsetof(RareMonsterNamesMessage, ann), NULL, (1 << TDF_XML_SERIALIZATION)),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(RareMonsterNamesMessage, pgbidRareNames), NULL, (1 << TDF_XML_SERIALIZATION), 2, 0),
    FixedArrayTypeDescriptorField("", DT_GBID, offsetof(RareMonsterNamesMessage, pgbidMonsterAffixes), NULL, (1 << TDF_XML_SERIALIZATION), 8, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RareMonsterNamesMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RareMonsterNamesMessage("RareMonsterNamesMessage", 0, RareMonsterNamesMessage_Fields, 5, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RARE_MONSTER_NAMES_MESSAGE = &TypeDescriptor_RareMonsterNamesMessage;

static const TypeDescriptorField RareItemNameMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RareItemNameMessage, tHeader), NULL, (1 << TDF_XML_SERIALIZATION)),
    TypeDescriptorField("", DT_INT, offsetof(RareItemNameMessage, ann), NULL, (1 << TDF_XML_SERIALIZATION)),
    TypeDescriptorField("", DT_RARE_ITEM_NAME, offsetof(RareItemNameMessage, tRareItemName), NULL, (1 << TDF_XML_SERIALIZATION)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RareItemNameMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_RareItemNameMessage("RareItemNameMessage", 0, RareItemNameMessage_Fields, 4, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_RARE_ITEM_NAME_MESSAGE = &TypeDescriptor_RareItemNameMessage;

static const TypeDescriptorField ACDCollFlagsMessage_Fields[4] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(ACDCollFlagsMessage, tHeader), NULL, (1 << TDF_XML_SERIALIZATION)),
    TypeDescriptorField("", DT_DATAID, offsetof(ACDCollFlagsMessage, ann), NULL, (1 << TDF_XML_SERIALIZATION)),
    BoundedTypeDescriptorField("", DT_INT, offsetof(ACDCollFlagsMessage, uCollFlags), NULL, NULL, (const void *)((1 << COLL_DYN_FLAG_COUNT) - 1)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ACDCollFlagsMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_ACDCollFlagsMessage("ACDCollFlagsMessage", 0, ACDCollFlagsMessage_Fields, 4, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ACD_COLL_FLAGS_MESSAGE = &TypeDescriptor_ACDCollFlagsMessage;

static const TypeDescriptorField CurrencyModifiedMessage_Fields[5] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(CurrencyModifiedMessage, tHeader), NULL, (1 << TDF_XML_SERIALIZATION)),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CurrencyModifiedMessage, bPlaySound), NULL, NULL, (const void *)1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CurrencyModifiedMessage, eCurrencyType), NULL, NULL, (const void *)NUM_CURRENCY_TYPES),
    BoundedTypeDescriptorField("", DT_INT, offsetof(CurrencyModifiedMessage, eReason), NULL, NULL, (const void *)GMR_COUNT),
    TypeDescriptorField(NULL, DT_NULL, sizeof(CurrencyModifiedMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_CurrencyModifiedMessage("CurrencyModifiedMessage", 0, CurrencyModifiedMessage_Fields, 5, NULL, (1 << TD_IS_COMPLEX_TYPE), NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CURRENCY_MODIFIED_MESSAGE = &TypeDescriptor_CurrencyModifiedMessage;




























void ACDInitializeForGame()
{


    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();

    tGCD.ptACDGlobals = (ACDGlobals *)SigmaMemoryNew(sizeof(ACDGlobals), 4, ptMemoryInterface, TRUE);
    tGCD.ptACDGlobals->ptACDs = TChainedDataArrayCreateTracked<ActorCommonData, ACDID>("ActorCommonData", ptMemoryInterface, 512);
    tGCD.ptACDGlobals->ptACDs->SetGrowCap(0x2000);
    tGCD.ptACDGlobals->poolQueuedACDMessageLists.Initialize();
    tGCD.ptACDGlobals->poolQueuedACDMessageLists.SetMemoryInterface(ptMemoryInterface);
    tGCD.ptACDGlobals->poolQueuedACDMessageLists.SetBlockCount_Tracked(1000);
    tGCD.ptACDGlobals->poolGBIDs.Initialize();
    tGCD.ptACDGlobals->poolGBIDs.SetMemoryInterface(ptMemoryInterface);
    tGCD.ptACDGlobals->poolGBIDs.SetBlockCount_Tracked(2048);


    tGCD.ptACDGlobals->listDirtyACDs.Initialize(MemoryPoolsGetDATAIDListPool());







    tGCD.ptACDGlobals->ptACDPacks = TChainedDataArrayCreateTracked<ACDPack, int>("ACDPacks", ptMemoryInterface, 512);
    tGCD.ptACDGlobals->ptACDPacks->SetGrowCap(0x2000);
    {
        tGCD.ptACDGlobals->tACDGroups.poolACDGroups.Initialize();
        tGCD.ptACDGlobals->tACDGroups.poolACDGroups.SetMemoryInterface(ptMemoryInterface);
        tGCD.ptACDGlobals->tACDGroups.poolACDGroups.SetBlockCount_Tracked(64);
        tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.Initialize(16, FALSE, ptMemoryInterface);

        ScopedGlobalDefinitionLockR ptGlobals;



        int nGroupCount = ptGlobals->ptServerData->serActorGroups.dwSize / sizeof(ActorGroup);

        int nHashTableSize = (ptGlobals->ptServerData->serActorGroups.dwSize / sizeof(ActorGroup)) > 64 ? nGroupCount : 64;




        tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.SetHashTableSize_Tracked(nHashTableSize);

        int nListCount = 4;

        tGCD.ptACDGlobals->tACDGroups.tAllocLists.Initialize(sizeof(ACDIDGroupListPooled), nListCount, ptMemoryInterface);

        for (int i = 0; i < nGroupCount; i++)
        {
            ActorGroup & tActorGroup = ptGlobals->ptServerData->arActorGroups[i];


            ACDGroup tACDGroup;
            tACDGroup.ptList = (ACDIDGroupListPooled *)tGCD.ptACDGlobals->tACDGroups.tAllocLists.Alloc_Tracked();

            SigmaMemoryClear(tACDGroup.ptList, sizeof(ACDIDGroupListPooled));
            tACDGroup.ptList->Initialize(&tGCD.ptACDGlobals->tACDGroups.poolACDGroups);
            tACDGroup.nCount = 0;
            tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.SetAt_Tracked(tActorGroup.uHash, tACDGroup);
        }
    }
}






void CACDInitializeForGame()
{

    ACDInitializeForGame();

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();


    tGCD.ptACDGlobals->patANNToACDLookup = (ACDID *)SigmaMemoryNew(sizeof(ACDID) * ANN_POOL_SIZE, 4, ptMemoryInterface, TRUE);
    for (int32 i = 0; i < ANN_POOL_SIZE; i++)
    {

        tGCD.ptACDGlobals->patANNToACDLookup[i] = ACDID_INVALID;
    }
}



void ACDDisposeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    ACDIterator tIt;
    while (ActorCommonData * ptACD = ActorCommonDataIterate(tIt))
    {











        ACDDelete(ptACD->id, FALSE);
    }

    tGCD.ptACDGlobals->ptACDs->DeleteAll();

    TChainedDataArrayDispose(tGCD.ptACDGlobals->ptACDs);

    tGCD.ptACDGlobals->poolQueuedACDMessageLists.Dispose();

    tGCD.ptACDGlobals->poolGBIDs.Dispose();


    tGCD.ptACDGlobals->listDirtyACDs.Dispose();

    TChainedDataArrayDispose(tGCD.ptACDGlobals->ptACDPacks);
    tGCD.ptACDGlobals->tACDGroups.poolACDGroups.Dispose();
    tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.Dispose();
    tGCD.ptACDGlobals->tACDGroups.tAllocLists.Dispose();


    SigmaMemoryFree(tGCD.ptACDGlobals->patANNToACDLookup, AppGetCurrentMemoryInterface()); tGCD.ptACDGlobals->patANNToACDLookup = NULL;

    SigmaMemoryFree(tGCD.ptACDGlobals, AppGetCurrentMemoryInterface()); tGCD.ptACDGlobals = NULL;
}






ACDPack & ACDPackGet(const ACDPackID idACDPack)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return *tGCD.ptACDGlobals->ptACDPacks->ElementGet(idACDPack);
}





ACDPack * ACDPackTryToGet(const ACDPackID idACDPack)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return tGCD.ptACDGlobals->ptACDPacks->ElementTryToGet(idACDPack);
}




void ACDPack::AddMember(ActorCommonData & tACDMember)
{




    listACDs.AddTail_Tracked(tACDMember.id);

    tACDMember.idACDPack = id;
}




ACDID ACDPack::GetLeader()
{
    return (ACDID)listACDs.GetHead();
}






void ACDPack::RemoveMember(ActorCommonData & tACDMember)
{


    POS pos = listACDs.Find(tACDMember.id);
    if (pos == NULL)
    {
        return;
    }

    listACDs.RemoveAt(pos);

    tACDMember.idACDPack = ACDPACKID_INVALID;

    if (listACDs.Count() == 0 &&
        nDeactivatedCount <= 0)
    {

        ACDPackDelete(*this);
    }
}




void ACDPack::RemoveDeactivated()
{


    nDeactivatedCount--;

    if (listACDs.Count() == 0 && nDeactivatedCount <= 0)
    {

        ACDPackDelete(*this);
    }
}




const ACDIDListPooled * ACDPack::GetMemberList()
{
    return &listACDs;
}




int32 ACDPack::GetTotalACDCount(BOOL bIncludeDyingACDs)
{

    if (!GameActiveGameCommonDataIsServer() ||
        bIncludeDyingACDs)
    {



        return listACDs.Count() + nDeactivatedCount;
    }
    else
    {
        int32 nCount = 0;
        POS pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)listACDs.GetNext(pos);




            if (ACDTryToGet(idACD) && !ActorFlagIsSet((ActorID)ACDGetOwner(idACD), ACTOR_DYING))
            {
                nCount++;
            }
        }












        return nCount + nDeactivatedCount;
    }
}
void ACDPackDelete(ACDPack & tACDPack)  // not `inline`: linker-removed out-of-line copy (low_pc 0, 0xd4) in the DWARF
{





    GameCommonData & tGCD = GameGetActiveGameCommonData();
    tGCD.ptACDGlobals->ptACDPacks->ElementDelete(&tACDPack);
}



ACDPackID ACDPackNew()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ACDPackID idACDPack = tGCD.ptACDGlobals->ptACDPacks->ElementNew();

    ACDPack & tACDPack = ACDPackGet(idACDPack);
    tACDPack.listACDs.SetPool(MemoryPoolsGetDATAIDListPool());
    tACDPack.nDeactivatedCount = 0;
    tACDPack.bBroadcastSent = FALSE;

    return idACDPack;
}















ACDID ACDCreate()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ACDID idACD = tGCD.ptACDGlobals->ptACDs->ElementNew();
    if (idACD == ACDID_INVALID)
    {
        int32 nCount = 0;
        ACDIterator tIt;




        while (ActorCommonDataIterate(tIt))
        {
            nCount++;
        }





        DisplayInternalError(NULL, "", 539, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }



    if (GameActiveGameCommonDataIsServer())
    {
        SGameGlobalsGet().tPerformanceTrackingInfo.m_uACDsCreatedThisIteration++;
        ServerQueueClientUpdate();
    }

    return idACD;
}

void ActorCommonData::InitAttributeSyncMask()
{
    uint8 uMask;



    if (GameActiveGameCommonDataIsServer())
    {
        ActorType _eActorType = GetActorType();
        uMask = 0;
        if (ACDIsPlayer(id) || ACDIsHireling(*this))
            uMask |= 0x81;

        if (IsGizmo())
        {
            uMask |= 0x04;
        }
        if (IsItem())
        {
            uMask |= 0x08;
        }
        if (IsMonster())
        {
            uMask |= 0x02;
        }
        if (_eActorType == ACTORTYPE_SERVER_PROP)
        {
            uMask |= 0x10;
        }
        if (_eActorType == ACTORTYPE_PROJECTILE)
        {
            uMask |= 0x40;
        }
    }
    else
    {
        uMask = 0xff;
    }



    uAttributeSyncMask = uMask;
}



































void ActorCommonData::Initialize(
    const ACDNetworkName & _ann,
    const DataID _idOwner,
    const SNO _snoActor,
    const GBHandle & _hGB,
    const SNOName & _tSNOName,
    uint32 _tName,
    const ActorLocation & _tLocation,
    const AttribGroupID idFastAttributeGroup,
    const BOOL fForceInventoryCreate,
    const BOOL bCollision)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();











    uint32 dwNow = GameGetServerTime();

    ann = _ann;
    idOwner = _idOwner;
    snoActor = _snoActor;
    tSNOName = SNONAME_NONE;
    hGB = _hGB;
    tSNOName = _tSNOName;
    eMonsterRarity = MR_NORMAL;



    tName = _tName;

    ptAnimPlayer = NULL;

    this->dwLastSeen = this->dwHitGroundTime = dwNow;
    this->dwKnownBits = 0;
    this->dwPlayerSeenBits = 0;
    this->dwPlayerLastSeenTime = dwNow;
    dwModifiedBits = 0;
    SetAttributeGroup(idFastAttributeGroup);
    idACDPack = ACDPACKID_INVALID; flShear = 0.0f;
    dwFlags = 0;

    posGroupPrimary = NULL;
    uGroupPrimary = (uint32)-1;
    posGroupSecondary = NULL;
    uGroupSecondary = (uint32)-1;
    nScriptState = 0;
    uAttributeSyncMask = 0;
    ptCollSphereNode = NULL;
    uHeroTagBitfield = 0;


    eGizmoType = ACTORGIZMO_UNDEFINED;
    eActorType = ACTORTYPE_INVALID;
    flHitPointsCurrent = 0.0f; eTeamOverride = TEAM_INVALID;

    eTeam = TEAM_INVALID; eHirelingClass = HC_NONE;
    fNeverTargetable = false;
    fIsUntargetable = false;
    fIsStealthed = false;
    nPetOwner = -1;
    fIsNPC = false;
    fIsDisabled = false;
    fIsAlive = false;
    fIsCharmed = false;
    fIsStunned = false;
    fHasAssignedHeroID = false;
    fHasSpawner = false;



    if (snoActor != SNO_NONE)
    {

        ScopedActorDefinitionLockR pdefActor(snoActor);
        if (TagMapTagExists(pdefActor->hTagMap, TAG_ACTOR_UNTARGETABLE))
        {
            fNeverTargetable = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_UNTARGETABLE) != 0;
        }
        else
        {
            fNeverTargetable = false;
        }
        eActorType = pdefActor->eType;
        if (IsItem())
        {
            eGizmoType = ACTORGIZMO_ITEM_INTERNAL;
        }
        else if (pdefActor->eType == ACTORTYPE_GIZMO)
        {
            eGizmoType = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TYPE);
            if (eGizmoType == ACTORGIZMO_SPAWNER)
            {
                BOOL bRenderOnlyInEditMode = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_RENDER_ONLY_IN_EDIT_MODE);
                if (bRenderOnlyInEditMode) dwFlags |= (1 << ACD_EDIT_MODE_ONLY_BIT); else dwFlags &= ~(1 << ACD_EDIT_MODE_ONLY_BIT);
            }
        }








    }


    if (tGCD.ptACDGlobals->patANNToACDLookup)
    {
        uint16 uIndex = (uint16)ann;







        tGCD.ptACDGlobals->patANNToACDLookup[uIndex] = id;
    }

    if (idFastAttrib != ATTRIBGROUPID_INVALID)
    {
        idPassiveAttribGroup = FastAttribGroupCreate(FAT_SIMPLE, -1);
        FastAttribMerge(idFastAttrib, idPassiveAttribGroup);

        if (GameActiveGameCommonDataIsServer() && ACDIsPlayer((ACDID)idOwner))
        {
            for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; ++i)
            {
                AttributesSetInt(MakeAttribKey(ATTRIBUTE_SLOT_UNEQUIPPED_TIME, i), GameGetServerTime());
            }
        }
    }
    else
    {
        idPassiveAttribGroup = ATTRIBGROUPID_INVALID;
    }
    idEnhancementAttribGroup = ATTRIBGROUPID_INVALID; idUpscaledAttribGroup = ATTRIBGROUPID_INVALID;


    listACDMessages.SetPool(&tGCD.ptACDGlobals->poolQueuedACDMessageLists);
    tListComplexEffects.SetPool(MemoryPoolsGetDATAIDListPool());

    listIdentifiedAffixGBIDs.SetPool(&tGCD.ptACDGlobals->poolGBIDs);
    listUnidentifiedAffixGBIDs.SetPool(&tGCD.ptACDGlobals->poolGBIDs);

    listBuffs.SetPool(MemoryPoolsGetDATAIDListPool());


    tLocation = _tLocation;
    if (tLocation.eActorLocation == AL_INVENTORY)
    {




        SigmaMemoryClear(&tLocation.tWorldLocation, sizeof(tLocation.tWorldLocation));
    }
    else if (tLocation.eActorLocation == AL_WORLD || tLocation.eActorLocation == AL_NONE)
    {





        tLocation.tInvLocation.idACDContainer = ACDID_INVALID; tLocation.tInvLocation.eSlot = SLOT_NONE;
        tLocation.tInvLocation.pt = cgptNull;
    }
    else
    {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    nLookLinkIndex = 0;


    ACDInventoryInitialize(*this, fForceInventoryCreate);
    ACDVisualInventoryInitialize(*this);
    pRareNameGBIDs[0] = pRareNameGBIDs[1] = GBID_NONE;
    tRareItemName.Initialize();
    for (int32 i = 0; i < NUM_MONSTER_AFFIXES; ++i)
    {
        pMonsterAffixGBIDs[i] = GBID_NONE;
    }






    AnimPlayerInit(*this);


    dwTimerCount = 0;





    ACDCollGetSettings(snoActor, tCollSettings);
    if (tLocation.eActorLocation == AL_WORLD)
    {
        tCollSettings.eTeam = GetTeam();

        idCollSWorld = SWORLDID_INVALID;
        if (bCollision)
        {
            UpdateCollision();
        }
    }
    unk_2c0.clear();
}



void ActorCommonData::UpdateCollision()
{
    const WorldLocation & tWorldLoc = GetWorldLocation();
    UpdateCollision(tWorldLoc);
}

void ACDCollUpdateSphereNode(ActorCommonData & tACD, const WorldLocation & tWorldLocation);  // ActorCommonCollision.cpp:212 (not declared in a header)


void ActorCommonData::UpdateCollision(
    const WorldLocation & tWorldLoc)
{
    ScopedActorDefinitionLockR ptActorDef(snoActor);

    switch (tCollSettings.eCollisionShape)
    {
    case COLLISION_SHAPE_CYLINDER:
    {
        AxialCylinder & tCyl = *(AxialCylinder *)tCollSettings.arAxialCylinderBytes;  // GetCyl() is not inlined in the original here; same idiom as SActorMovement.cpp:844
        tCyl.wpBase = tWorldLoc.transform * ptActorDef->tCollData.wcylCollision.wpBase;
        tCyl.wdHeight = ptActorDef->tCollData.wcylCollision.wdHeight * tWorldLoc.flScale;
        tCyl.wdRadius = ActorGetCollisionCylinderRadius(ptActorDef) * tWorldLoc.flScale;
        break;
    }

    case COLLISION_SHAPE_BOX:
    {
        OBB & tObb = *(OBB *)tCollSettings.arOBBBytes;  // GetOBB() is not inlined in the original here
        tObb.transform = tWorldLoc.transform;
        tObb.transform.wp = tWorldLoc.transform * ptActorDef->tCollData.aabbCollision.wp;
        tObb.wvExt = ptActorDef->tCollData.aabbCollision.wvExt * tWorldLoc.flScale;
        break;
    }
    }

    if (tCollSettings.eCollisionShape != COLLISION_SHAPE_NONE)
    {
        ACDCollUpdateSphereNode(*this, tWorldLoc);
    }

    tCollSettings.eTeam = GetTeam();
}




void ActorCommonData::EnableCollision(BOOL bEnable)
{
    BOOL bCurEnable = tCollSettings.uDynamicMyFlags != 0;
    if (bCurEnable != bEnable)
    {
        uint32 uDynFlagsInit = tCollSettings.uDynamicMyFlags;
        if (bEnable)
        {
            ScopedActorDefinitionLockR ptActorDef(snoActor);
            tCollSettings.uDynamicMyFlags = ptActorDef->tCollData.tCollFlags.uDynamicMyFlags;
        }
        else
        {
            tCollSettings.uDynamicMyFlags = 0;
        }

        if (uDynFlagsInit != tCollSettings.uDynamicMyFlags)
        {
            ModifiedMark(ACDMODIFIED_COLL_FLAGS);
        }
    }
}







void ActorCommonData::SendCollision(const PlayerList & listDestPlayers)
{
    ACDCollFlagsMessage tMessage;

    tMessage.ann = ANNFromACD(id);
    tMessage.uCollFlags = tCollSettings.uDynamicMyFlags;
    MessageSendToClient(listDestPlayers, SMSG_ACD_COLL_FLAGS, &tMessage, sizeof(tMessage));
}


static ActorMovement * sGetActorMovement(DataID idOwner)
{
    if (GameActiveGameCommonDataIsClient())
    {
        RActor * ptRActor = RActorTryToGet((RActorID)idOwner);
        if (ptRActor == NULL)
        {
            return NULL;
        }
        return ptRActor->ptCActorMovement;
    }
    else
    {
        Actor * ptActor = ActorTryToGet((ActorID)idOwner);
        if (ptActor == NULL)
        {
            return NULL;
        }
        return ptActor->ptSActorMovement;
    }
}



















const ActorMovement * ActorCommonData::GetActorMovement() const
{
    return sGetActorMovement(idOwner);
}



ActorMovement * ActorCommonData::GetActorMovement()
{
    return sGetActorMovement(idOwner);
}




// NON_MATCHING: ours keeps TChainedDataArray<ActorCommonData>::ElementDeleteByID out of line (the original inlines it and ~ActorCommonData here); the inliner deferral of ~ActorCommonData depends on list-destructor call sites in functions of this file that are not written yet
void ACDDelete(
    const ACDID idACD,
    BOOL bWorldDeleted)
{
    if (idACD == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACD = ACDGet(idACD);




    if (GameActiveGameCommonDataIsClient() && tACD.snoActor == SNOFileLookupGUID(ACTOR_SNO, "Amiibo_Scan_Portal"))
    {
        if (CGameGlobalsGet().idCurrentCWorld != CWORLDID_INVALID)
        {

            Player * ptPlayer = Console::NFP::GetPlayerByScanningPortal(idACD);
            if (ptPlayer)
            {
                UIGame::DisplayError(GameErrorData(GAMEERROR_AMIIBO_SCAN_FAILED), FALSE, ptPlayer);


                ANNDataMessage tMessage;
                tMessage.ann = ANNFromACD(ptPlayer->idPlayerACD);
                MessageSendToServer(CMSG_AMIIBO_SCAN_END, &tMessage, sizeof(tMessage), ptPlayer);
            }
        }
    }



    if (GameActiveGameCommonDataIsServer())
    {
        FlippyID idFlippy = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FLIPPY_ID));
        if (idFlippy != -1)
        {
            FlippyDestroy(idFlippy);
        }

        TimerID idTimer = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_TRADE_TIMER_ID));
        if (TimerExists(idTimer))
        {
            TimerUnregister(idTimer);
        }
    }


    if (tACD.IsNecroCorpse())
    {

        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {
            if (PlayerGetPlayerClass(ptPlayer) == PLAYER_CLASS_NECROMANCER)
            {
                ACDID * iter = blz::find(ptPlayer->listNecroCorpses.begin(), ptPlayer->listNecroCorpses.end(), tACD.id);
                if (iter != ptPlayer->listNecroCorpses.end())
                {
                    ptPlayer->listNecroCorpses.erase(iter);
                }
            }
        }
    }













    if (!tACD.tListComplexEffects.IsEmpty())
    {
        if (GameActiveGameCommonDataIsServer())
        {

            SComplexEffectRemoveAll(idACD);
        }
        else
        {


            CComplexEffectRemoveAll(tACD);
        }
    }



    ACDPack * ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (ptACDPack)
    {
        ptACDPack->RemoveMember(tACD);
    }


    if (tACD.idPassiveAttribGroup != ATTRIBGROUPID_INVALID)
    {
        FastAttribGroupDelete(tACD.idPassiveAttribGroup);
        tACD.idPassiveAttribGroup = ATTRIBGROUPID_INVALID;
    }

    if (tACD.idEnhancementAttribGroup != ATTRIBGROUPID_INVALID)
    {
        FastAttribGroupDelete(tACD.idEnhancementAttribGroup);
        tACD.idEnhancementAttribGroup = ATTRIBGROUPID_INVALID;
    }

    if (tACD.idUpscaledAttribGroup != ATTRIBGROUPID_INVALID)
    {
        FastAttribGroupDelete(tACD.idUpscaledAttribGroup);
        tACD.idUpscaledAttribGroup = ATTRIBGROUPID_INVALID;
    }

    if (tACD.idFastAttrib != ATTRIBGROUPID_INVALID)
    {
        FastAttribGroupDelete(tACD.idFastAttrib);
        tACD.idFastAttrib = ATTRIBGROUPID_INVALID;

        tACD.ptFastAttribGroup = NULL;
    }



    if (tACD.ptFriendGiftData)
    {
        delete tACD.ptFriendGiftData; tACD.ptFriendGiftData = NULL;
    }
    if (tACD.ptAvengerData)
    {
        delete tACD.ptAvengerData; tACD.ptAvengerData = NULL;
    }


    AnimPlayerDispose(tACD);

    ActorInventory * ptInventory = tACD.ptInventory;
    ACDInventoryDispose(ptInventory);
    tACD.ptInventory = ptInventory;

    VisualInventory * ptVisualInventory = tACD.ptVisualInventory;
    ACDVisualInventoryDispose(ptVisualInventory);
    tACD.ptVisualInventory = ptVisualInventory;

    PortalSpecifierDispose(idACD);

    tACD.listIdentifiedAffixGBIDs.RemoveAll();
    tACD.listUnidentifiedAffixGBIDs.RemoveAll();



    if (GameActiveGameCommonDataIsServer())
    {
        tACD.SetPrimaryGroup(0xFFFFFFFF, bWorldDeleted, ACDID_INVALID);
        tACD.SetSecondaryGroup(0xFFFFFFFF, bWorldDeleted, ACDID_INVALID);
    }

    if (tACD.ptCollSphereNode)
    {
        ACDCollDisposeSphereNode(tACD);
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    BOOL fGCDIsServer = GameActiveGameCommonDataIsServer();
    if (fGCDIsServer)
    {

        LuaScriptDisposeACDVariableList(idACD);
    }



    if (tGCD.ptACDGlobals->patANNToACDLookup)
    {
        uint16 uIndex = (uint16)tACD.ann;
        if (tGCD.ptACDGlobals->patANNToACDLookup[uIndex] == idACD)
        {
            tGCD.ptACDGlobals->patANNToACDLookup[uIndex] = ACDID_INVALID;
        }
    }

    if (fGCDIsServer)
    {

        if (tACD.tLocation.eActorLocation == AL_WORLD &&
            SGameManager::GameGet().ptSScenes &&
            SSceneTryToGet(tACD.tLocation.tWorldLocation.idSScene))
        {
            SScenePutACDInScene(tACD.tLocation.tWorldLocation.idSScene, tACD.id, FALSE);
        }
    }


    tGCD.ptACDGlobals->ptACDs->ElementDeleteByID(idACD);


    if (GameActiveGameCommonDataIsServer())
    {
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        if (fGCDIsServer && !(tSGameGlobals.dwFlags & (1 << SGAME_SIMULATING)))
        {
            ServerQueueClientUpdate();
        }
    }
    tACD.unk_2c0.clear();
}

BOOL PlayerClassCanDualWield(const PlayerClass ePlayerClass);  // Player.cpp:2332
WeaponClass GetWeaponClass(const GBID gbidItemLeft, const GBID gbidItemRight);  // ActorInventory.cpp:11233
BOOL ItemIsHalfOfSwordFistComboForMonk(GBID gbidItem);  // Items.cpp:1202

static BOOL sIsDualWielding(
    const PlayerClass ePlayerClass,
    const GBID gbidItemLeft,
    const GBID gbidItemRight)
{
    if (!PlayerClassCanDualWield(ePlayerClass))
    {
        return FALSE;
    }

    WeaponClass eClassLeft = GetWeaponClass(gbidItemLeft, GBID_NONE);
    if (eClassLeft != WEAPONCLASS_1HSWING && eClassLeft != WEAPONCLASS_1HTHRUST && eClassLeft != WEAPONCLASS_1HFIST && eClassLeft != WEAPONCLASS_HANDXBOW)
    {
        return FALSE;
    }




    WeaponClass eClassRight = GetWeaponClass(gbidItemRight, GBID_NONE);
    if (eClassRight != WEAPONCLASS_1HSWING && eClassRight != WEAPONCLASS_1HTHRUST && eClassRight != WEAPONCLASS_1HFIST && eClassRight != WEAPONCLASS_HANDXBOW)
    {
        return FALSE;
    }






    return TRUE;
}

WeaponClass GetWeaponClassForAnim(
    const PlayerClass ePlayerClass,
    const GBID gbidItemLeft,
    const GBID gbidItemRight)
{
    WeaponClass eWeaponClass = GetWeaponClass(gbidItemLeft, gbidItemRight);
    BOOL fIsHoldingSpecialOffhand = ItemHasLabel(gbidItemLeft, ITL_ORB) || ItemHasLabel(gbidItemRight, ITL_ORB) || ItemHasLabel(gbidItemLeft, ITL_MOJO) || ItemHasLabel(gbidItemRight, ITL_MOJO) || ItemHasLabel(gbidItemRight, ITL_OFFHAND_NECROMANCER);
    BOOL fIsDualWielding = sIsDualWielding(ePlayerClass, gbidItemLeft, gbidItemRight);
    BOOL fIsHoldingShield = ItemHasLabel(gbidItemRight, ITL_SHIELD) || ItemHasLabel(gbidItemRight, ITL_SHIELD_CRUSADER) || ItemHasLabel(gbidItemLeft, ITL_SHIELD) || ItemHasLabel(gbidItemLeft, ITL_SHIELD_CRUSADER);

    if (eWeaponClass == WEAPONCLASS_NONE)
    {

        if (fIsHoldingSpecialOffhand)
        {
            return WEAPONCLASS_HTH_WITH_ORB;
        }


        if (fIsHoldingShield)
        {
            return WEAPONCLASS_HTH_WITH_SHIELD;
        }

        return WEAPONCLASS_NONE;
    }


    if (eWeaponClass != WEAPONCLASS_DUALWIELD && fIsDualWielding)
    {

        if (ePlayerClass == PLAYER_CLASS_MONK)
        {
            if ((ItemIsHalfOfSwordFistComboForMonk(gbidItemLeft) && ItemHasLabel(gbidItemRight, ITL_FIST_WEAPON)) ||
                (ItemHasLabel(gbidItemLeft, ITL_FIST_WEAPON) && ItemIsHalfOfSwordFistComboForMonk(gbidItemRight)))
            {
                return WEAPONCLASS_DUALWIELD_SWORD_FIST;
            }

            if (ItemHasLabel(gbidItemLeft, ITL_FIST_WEAPON) && ItemHasLabel(gbidItemRight, ITL_FIST_WEAPON))
            {
                return WEAPONCLASS_DUALWIELD_FIST_FIST;
            }
        }

        return WEAPONCLASS_DUALWIELD;
    }
    if (fIsHoldingSpecialOffhand)
    {







        // case order inferred: the original compare chain tests 1, 2, 8 with the WAND case falling through
        switch (eWeaponClass)
        {
        case WEAPONCLASS_1HSWING:
            return WEAPONCLASS_1HSWING_WITH_ORB;
        case WEAPONCLASS_WAND:
            return WEAPONCLASS_WAND_WITH_ORB;
        case WEAPONCLASS_1HTHRUST:
            return WEAPONCLASS_1HTHRUST_WITH_ORB;
        default:
            return WEAPONCLASS_HTH_WITH_ORB;
        }
    }


    if ((ePlayerClass == PLAYER_CLASS_CRUSADER || ePlayerClass == PLAYER_CLASS_NECROMANCER) && fIsHoldingShield)
    {
        switch (eWeaponClass)
        {
        case WEAPONCLASS_1HSWING:
            return WEAPONCLASS_1HSWING_WITH_SHIELD;
        case WEAPONCLASS_1HTHRUST:
            return WEAPONCLASS_1HTHRUST_WITH_SHIELD;
        case WEAPONCLASS_2HSWING:
            return WEAPONCLASS_2HSWING_WITH_SHIELD;
        case WEAPONCLASS_2HTHRUST:
            return WEAPONCLASS_2HTHRUST_WITH_SHIELD;
        case WEAPONCLASS_STAFF:
            return WEAPONCLASS_STAFF_WITH_SHIELD;
        case WEAPONCLASS_2H_FLAIL:
            return WEAPONCLASS_2HFLAIL_WITH_SHIELD;
        default:
            return WEAPONCLASS_HTH_WITH_SHIELD;
        }
    }









    return eWeaponClass;
}


BOOL AnimPlayerACDIsInConversationAndNotInCombat(const ActorCommonData & tACD);  // AnimPlayer.cpp:185

WeaponClass ActorCommonData::GetWeaponClassForAnim(
    const ActorDefinition * pdefActor) const
{


    InventoryType eInventoryType = ACDInventoryGetType(*this);
    if (eInventoryType != INVENTORY_PLAYER && eInventoryType != INVENTORY_HIRELING && eInventoryType != INVENTORY_GENERIC)
    {
        WeaponClass eWeaponClass = AttributesGetInt(FastAttribKey(ATTRIBUTE_CURRENT_WEAPONCLASS));
        if (eWeaponClass != -1)
        {
            return eWeaponClass;
        }



        BOOL fLocalLock = FALSE;
        if (!pdefActor)
        {
            pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(snoActor, FALSE);
            fLocalLock = TRUE;
        }





        WeaponClass eClass = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_ANIM_WEAPON_CLASS);

        if (fLocalLock)
        {
            gptGroupDefActor->UnlockR((void *&)pdefActor);
        }
        return eClass;
    }

    if (eInventoryType == INVENTORY_PLAYER)
    {
        WeaponClass ePlayerWeaponClassAnimOverride = AttributesGetInt(FastAttribKey(ATTRIBUTE_PLAYER_WEAPONCLASS_ANIM_OVERRIDE));
        if (ePlayerWeaponClassAnimOverride != -1)
        {
            return ePlayerWeaponClassAnimOverride;
        }
    }

    VisualInventorySlot eVisualSlotLeftHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_LEFT_HAND);
    VisualInventorySlot eVisualSlotRightHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_RIGHT_HAND);

    GBID gbidItemLeft = GBID_NONE;
    GBID gbidItemRight = GBID_NONE;

    VisualInventory * _ptVisualInventory = VisualInventoryGet(id);
    if (_ptVisualInventory)
    {
        VisualItem & tVisualItemLeftHand = _ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotLeftHand];
        VisualItem & tVisualItemRightHand = _ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotRightHand];

        gbidItemLeft = tVisualItemLeftHand.gbidItem;
        gbidItemRight = tVisualItemRightHand.gbidItem;
    }



    if (AttributesGetInt(FastAttribKey(ATTRIBUTE_WEAPONS_SHEATHED)) || AnimPlayerACDIsInConversationAndNotInCombat(*this))
    {
        gbidItemLeft = GBID_NONE;
        gbidItemRight = GBID_NONE;
    }

    PlayerClass ePlayerClass = PlayerGetPlayerClass(id);
    return ::GetWeaponClassForAnim(ePlayerClass, gbidItemLeft, gbidItemRight);
}


SNO ActorCommonData::GetSpecificAnim(const uint32 dwTag) const
{
    WeaponClass eWeaponClass = GetWeaponClassForAnim(NULL);
    return ActorGetAnim(snoActor, eWeaponClass, dwTag);
}
namespace Console {
namespace Online {
bool GetSNOBlacklistData(const SNOName & sno, uint32 * flags);  // X/Client/Console/LobbyService.cpp:851 (not declared in a header)
}
}
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower);  // X/Common/Powers/PowersCommon.cpp:4470
void ActorCommonData::GetSpecificAnimSet(const uint32 dwTag, SNO * psnoAnimSet, uint32 * pdwAnimSetTag) const
{
    WeaponClass eWeaponClass = GetWeaponClassForAnim(NULL);
    ActorGetSpecificAnimSet(snoActor, eWeaponClass, dwTag, psnoAnimSet, pdwAnimSetTag);
}








WorldLocation & ActorCommonData::GetWorldLocation()
{
    return tLocation.tWorldLocation;
}














const WorldLocation & ActorCommonData::GetWorldLocation() const
{
    return tLocation.tWorldLocation;
}













InventoryLocation & ActorCommonData::GetInventoryLocation()
{
    return tLocation.tInvLocation;
}





const InventoryLocation & ActorCommonData::GetInventoryLocation() const
{
    return tLocation.tInvLocation;
}




const InventoryLocation * ActorCommonData::TryToGetInventoryLocation() const
{
    if (tLocation.eActorLocation == AL_INVENTORY)
    {
        return &tLocation.tInvLocation;
    }
    return NULL;
}




ACDID ActorCommonData::GetInventoryContainerOwner() const
{
    const ActorLocation & _tLocation = tLocation;




    if (GetLocationType() == AL_INVENTORY)
    {
        const ActorCommonData * ptACDOwner = ACDTryToGet(_tLocation.tInvLocation.idACDContainer);
        if (ptACDOwner)
        {
            return ptACDOwner->GetInventoryContainerOwner();
        }
    }

    return id;
}
WorldPlace ActorCommonData::GetWorldPlaceCenter() const
{


    WorldPlace wpCenterPlace = GetPlace();

    wpCenterPlace.wp = wpCenterPlace.wp + cgwvZAxis * (GetHeight() * 0.5f);
    return wpCenterPlace;
}



BOOL ActorCommonData::IsPlayerTeam() const
{
    Team _eTeam = GetTeam();
    return _eTeam >= TEAM_PLAYER_FIRST && _eTeam <= TEAM_PLAYER_LAST;
}





BOOL ActorCommonData::IsMonster() const
{
    return eActorType == ACTORTYPE_MONSTER;
}










BOOL ActorCommonData::IsNecroCorpse() const
{
    GizmoType eGizmoType = GetGizmoType();
    return eGizmoType == ACTORGIZMO_NECRO_CORPSE;
}






BOOL ActorCommonData::IsAvenger() const
{
    if (ptAvengerData && ptAvengerData->eAvengerSource != AVENGER_SOURCE_INVALID)
        return TRUE;
    return FALSE;
}








BOOL ActorCommonData::IsMirrorImage() const
{


    ScopedActorDefinitionLockR pdefActor(snoActor);
    return TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_IS_MIRROR_IMAGE);
}



BOOL ActorCommonData::IsMonsterTeam() const
{
    Team _eTeam = GetNaturalTeam();
    return TeamIsMonsterTeam(_eTeam);
}





BOOL ActorCommonData::IsAlive() const
{
    return fIsAlive;
}














BOOL ActorCommonData::IsObserver() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_OBSERVER)) != 0;
}




BOOL ActorCommonData::IsMyPet(int32 nPlayerIndex) const
{
    Player * ptPlayer = GetPetOwnerPlayer();
    return ptPlayer != NULL && ptPlayer->nPlayerIndex == nPlayerIndex;
}















ACDID ACDGetPetOwner(ACDID idACDPet)
{

    ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
    if (ptACDPet)
    {
        Player * ptPlayer = ptACDPet->GetPetOwnerPlayer();
        if (ptPlayer)
        {
            return ptPlayer->idPlayerACD;
        }
    }

    return ACDID_INVALID;
}



ACDID ACDGetSummoner(ACDID idACD)
{

    ActorCommonData * ptACDSummoned = ACDTryToGet(idACD);
    if (ptACDSummoned)
    {
        return (ACDID)ptACDSummoned->AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID));
    }
    return ACDID_INVALID;
}




ACDID ACDGetControllingActor(ACDID idACDPet)
{

    ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
    if (ptACDPet)
    {

        Player * ptPlayer = ptACDPet->GetPetOwnerPlayer();
        if (ptPlayer)
        {
            return ptPlayer->idPlayerACD;
        }

        ACDID idACDSummoner = (ACDID)ptACDPet->AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID));
        if (idACDSummoner != ACDID_INVALID &&

            ACDIsPlayer(idACDSummoner))
        {
            return idACDSummoner;
        }
    }

    return idACDPet;
}



Player * ActorCommonData::GetPetOwnerPlayer() const
{
    int32 nOwner = nPetOwner;
    if (nOwner != -1)
    {
        return PlayerGetByPlayerIndex(nOwner);
    }
    return NULL;
}












































BOOL ActorCommonData::IsLoading() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_LOADING)) != 0;
}




void ActorCommonData::SetEnabled(
    const BOOL bEnable)
{



    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_ACTOR_DISABLED_BUFF);
    ActorID idActor = (ActorID)ACDGetOwner(id);
    if (!bEnable)
    {
        PowerAPIBuffStartData tBuffStartData(-1);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.bInfinite = true;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_DISABLED), 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_INVULNERABLE), 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_HIDDEN), 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_NPC_DISABLE_OPERATION), 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_UNTARGETABLE), 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST), 1);
        PowerAPI_BuffAddToACD(tBuffStartData, *this);
        EnableCollision(FALSE);
        BrainAPIDoIdle(idActor);
        Minimap::DeactivateMarker(id, FALSE);

        ProximityDetectorEnabledStatusChanged(idActor, FALSE);
    }
    else
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, this, ACDID_INVALID);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff)
        {
            Minimap::ActivateMarker(id);
            PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            EnableCollision(TRUE);

            ProximityDetectorEnabledStatusChanged(idActor, TRUE);
        }
    }

    KnownListItemChanged(KET_ACD, id, FALSE);
}




BOOL ActorCommonData::IsEnabled() const
{
    return !fIsDisabled;
}








void ActorCommonData::SetTargetable(const BOOL bTargetable)
{
    if (bTargetable == TRUE)
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_UNTARGETABLE), 0);
    }
    else
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_UNTARGETABLE), 1);
    }
}




BOOL ActorCommonData::IsNeverTargetable() const
{
    return fNeverTargetable;
}














BOOL ActorCommonData::IsTargetable() const
{
    return !fIsUntargetable && !IsNeverTargetable();
}

















BOOL ACDIsFriendly(Team eTeam1, Team eTeam2)
{
    if (eTeam1 == TEAM_INVALID || eTeam2 == TEAM_INVALID)
    {
        return FALSE;
    }
    return TeamsAreFriendly(eTeam1, eTeam2);
}







BOOL ACDIsFriendly(const ActorCommonData & tACD1, const ActorCommonData & tACD2, BOOL bCharmedAlliesAreEnemies)
{


    Team eTeam1 = tACD1.GetTeam();
    Team eTeam2 = tACD2.GetTeam();

    if (!ACDIsFriendly(eTeam1, eTeam2))
    {
        return FALSE;
    }
    if (bCharmedAlliesAreEnemies)
    {
        return !tACD1.IsCharmed() && !tACD2.IsCharmed();
    }
    return TRUE;
}






































BOOL ActorCommonData::CanTargetNecroCorpse(ACDID idACDCorpse) const
{

    ActorCommonData * ptACDCorpse = ACDTryToGet(idACDCorpse);
    Player * ptPlayer = NULL;
    BOOL bPlayerIsOwner = FALSE;






    if (!ptACDCorpse)
        return FALSE;

    if (ACDIsPlayer(id))
        ptPlayer = PlayerGetByACD(id);
    else if (IsPet())
        ptPlayer = GetPetOwnerPlayer();




    if (!ptPlayer || ptPlayer->ePlayerClassPortrait != PLAYER_CLASS_NECROMANCER)
        return FALSE;




    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {








        if (ptPlayer->tAccount.tHandle.m_id == ptACDCorpse->GetNecroCorpseOwnerPlayerID(i))
        {
            bPlayerIsOwner = TRUE;
            break;
        }
    }

    return bPlayerIsOwner;
}




BOOL ACDIsHostile(Team eTeam1, Team eTeam2)
{
    if (eTeam1 == TEAM_INVALID || eTeam2 == TEAM_INVALID)
    {
        return FALSE;
    }
    return !TeamsAreFriendly(eTeam1, eTeam2);
}








BOOL ACDIsHostile(const ActorCommonData & tACD1, const ActorCommonData & tACD2, BOOL bCharmedAlliesAreEnemies)
{


    Team eTeam1 = tACD1.GetTeam();
    Team eTeam2 = tACD2.GetTeam();

    if (ACDIsHostile(eTeam1, eTeam2))
    {
        return TRUE;
    }
    if (bCharmedAlliesAreEnemies)
    {
        return tACD1.IsCharmed() || tACD2.IsCharmed();
    }
    return FALSE;
}
BOOL ACDIsOnTheSameTeam(
    const ActorCommonData & tACD1,
    const ActorCommonData & tACD2)
{
    return tACD1.GetTeam() == tACD2.GetTeam();
}




BOOL ACDCanHaveEnemies(Team eTeam)
{
    return eTeam != TEAM_SCENERY && eTeam != TEAM_NEUTRAL;
}











void ActorLocationInit(ActorLocation & tLocation, const WorldPlace & tPlace, const Quaternion & q, const float flScale)
{




    tLocation.eActorLocation = (tPlace.idSWorld == SWORLDID_INVALID) ? AL_NONE : AL_WORLD;








    tLocation.tWorldLocation.idSWorld = tPlace.idSWorld;

    if (tPlace.idSWorld != SWORLDID_INVALID)
    {
        tLocation.tWorldLocation.idSScene = SceneCommonGetSceneIDFromWorldPlace(tPlace, FALSE);
    }
    else
    {
        tLocation.tWorldLocation.idSScene = SSCENEID_INVALID;
    }


    tLocation.tWorldLocation.transform.Set(q, tPlace.wp);

    ActorLocationSetScale(tLocation, flScale);

    tLocation.tWorldLocation.wsBounds.Set(cgwpNull, 0.0f);
    tLocation.tWorldLocation.wcylBounds.Set(cgwpNull, 0.0f, 0.0f);
    tLocation.tWorldLocation.ptSphereNode = NULL;
}



void ActorLocationSetScale(ActorLocation & tLocation, const float flScale)
{
    tLocation.tWorldLocation.flScale = Math::Max(flScale, 0.0f);
}






ACDID ACDFromANN(const ACDNetworkName ann)
{

    ACDID idACD = (ACDID)ann;


    if (GameActiveGameCommonDataIsClient())
    {
        if (ann == ANN_INVALID)
            return ACDID_INVALID;


        GameCommonData & tGCD = GameGetActiveGameCommonData();
        uint16 uIndex = (uint16)ann;

        if (uIndex >= ANN_POOL_SIZE)
            return ACDID_INVALID;


        idACD = tGCD.ptACDGlobals->patANNToACDLookup[uIndex];


        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD == NULL || ptACD->ann != ann)
            return ACDID_INVALID;
    }











    return idACD;
}

ACDNetworkName ANNFromACD(ACDID idACD)
{

    if (GameActiveGameCommonDataIsClient())
    {

        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD)
        {
            return ptACD->ann;
        }
        return ANN_INVALID;
    }










    return (ACDNetworkName)idACD;
}





























SWorldID ActorCommonData::GetSWorldID() const
{
    if (tLocation.eActorLocation != AL_WORLD)
    {
        return SWORLDID_INVALID;
    }
    return tLocation.tWorldLocation.idSWorld;
}






BOOL ActorCommonData::IsInSWorldID(const SWorldID idSWorld) const
{
    if (!IsInWorld())
    {
        return FALSE;
    }
    const WorldLocation & _tLocation = GetWorldLocation();
    return _tLocation.idSWorld == idSWorld;
}













BOOL ActorCommonData::IsInWorld() const
{
    return tLocation.eActorLocation == AL_WORLD;
}












BOOL ActorCommonData::IsInInventory() const
{
    return tLocation.eActorLocation == AL_INVENTORY;
}












WorldDistance ActorCommonData::GetHeight() const
{
    return tLocation.tWorldLocation.wcylBounds.wdHeight;
}





WorldDistance ACDGetDistanceSquared(const ActorCommonData & tACDFrom, const ActorCommonData & tACDTo, const BOOL bWantAs2D)
{




    WorldPlace tPlaceFrom = tACDFrom.GetPlace();
    WorldPlace tPlaceTo = tACDTo.GetPlace();


























    if (bWantAs2D)
    {
        return Vector3D::Distance2DSquared(tPlaceFrom.wp, tPlaceTo.wp);
    }
    return Vector3D::DistanceSquared(tPlaceFrom.wp, tPlaceTo.wp);
}




BOOL ACDWithinDistance(const ActorCommonData & tACD1, const ActorCommonData & tACD2, const float flDistance)
{




    WorldPlace tPlace1 = tACD1.GetPlace();
    WorldPlace tPlace2 = tACD2.GetPlace();
    if (tPlace1.idSWorld != tPlace2.idSWorld)
    {
        return FALSE;
    }



    if (Vector3D::DistanceSquared(tPlace1.wp, tPlace2.wp) > flDistance * flDistance)
    {
        return FALSE;
    }
    return TRUE;
}



WorldDistance ActorCommonData::GetRadius() const
{
    return tLocation.tWorldLocation.wcylBounds.wdRadius;
}





WorldPlace ActorCommonData::GetPlace() const
{
    const WorldLocation & _tLocation = tLocation.tWorldLocation;
    WorldPlace tPlace;
    tPlace.wp = _tLocation.transform.wp;
    tPlace.idSWorld = _tLocation.idSWorld;
    return tPlace;
}






Quaternion ActorCommonData::GetOrientation() const
{
    return Quaternion(tLocation.tWorldLocation.transform.q);
}




Vector3D ActorCommonData::GetForward() const
{
    Vector3D vForward(cgwvXAxis);
    Angle aFacing = GetOrientation().ToYaw();
    Vector3D::Rotate(cgwvZAxis, aFacing, vForward);
    return vForward;
}



Vector2D ActorCommonData::GetForward2D() const
{
    Angle aFacing = GetOrientation().ToYaw();
    return Vector2D(Math::Cos(aFacing), Math::Sin(aFacing));
}














float ActorCommonData::GetScale() const
{
    return tLocation.tWorldLocation.flScale;
}









ActorLocationType ActorCommonData::GetLocationType() const
{
    return tLocation.eActorLocation;
}


InventorySlotType ActorCommonData::GetLocationSlot() const
{
    return tLocation.tInvLocation.eSlot;
}




void ActorCommonData::ChangeLocation(ActorLocationType nLocation)
{

    ActorLocationType ePrevious = tLocation.eActorLocation;


    tLocation.eActorLocation = nLocation;


    if (IsItem() && GameActiveGameCommonDataIsServer() &&
        (nLocation == AL_INVENTORY || ePrevious == AL_INVENTORY))
    {
        KnownListItemChanged(KET_ACD, id, FALSE);
        ModifiedMark(ACDMODIFIED_INVENTORY_POSITION);
    }
}















ActorLocation & ActorCommonData::GetLocation()
{
    return tLocation;
}





const ActorLocation & ActorCommonData::GetLocation() const
{
    return tLocation;
}





void ActorCommonData::SetOwner(const DataID _idOwner)
{
    idOwner = _idOwner;
}





BOOL ACDInSameWorld(const ActorCommonData & tACD1, const ActorCommonData & tACD2)
{




    if (!tACD1.IsInWorld() || !tACD2.IsInWorld())
    {
        return FALSE;
    }
    const WorldLocation & tLocation1 = tACD1.GetWorldLocation();
    const WorldLocation & tLocation = tACD2.GetWorldLocation();
    return tLocation1.idSWorld == tLocation.idSWorld;
}












ActorCommonData * ActorCommonDataIterate(ACDIterator & tIterator)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ActorCommonData * ptACD = tGCD.ptACDGlobals->ptACDs->Iterate(tIterator);
    return ptACD;
}




void ACDSendServerUpdates(
    const PlayerList & listDestPlayers)
{
    PlayerList listSinglePlayer(PlayerListGetElementPool());

    GameCommonData & tGCD = GameGetActiveGameCommonData();










    while (!tGCD.ptACDGlobals->listDirtyACDs.IsEmpty())
    {
        ACDID idACD = (ACDID)tGCD.ptACDGlobals->listDirtyACDs.GetHead();
        tGCD.ptACDGlobals->listDirtyACDs.RemoveHead();

        ActorCommonData * ptACD = ACDTryToGet(idACD);


        if (ptACD && ptACD->IsModifiedAny())
        {
            POS pos = listDestPlayers.GetHeadPosition();
            while (pos)
            {
                Player * ptPlayer = listDestPlayers.GetNext(pos);


                if (ptACD->IsKnownByPlayer(*ptPlayer))
                {


                    listSinglePlayer.RemoveAll();
                    listSinglePlayer.AddTail_Tracked(ptPlayer);
                    ptACD->SendServerUpdateMessage(listSinglePlayer);
                }
            }



            ptACD->ModifiedClear();
        }
    }











}



void ACDModifiedClearAll()
{










    GameCommonData & tGCD = GameGetActiveGameCommonData();






    while (!tGCD.ptACDGlobals->listDirtyACDs.IsEmpty())
    {
        ACDID idACD = (ACDID)tGCD.ptACDGlobals->listDirtyACDs.GetHead();
        tGCD.ptACDGlobals->listDirtyACDs.RemoveHead();

        ActorCommonData * ptACD = ACDTryToGet(idACD);


        if (ptACD && ptACD->IsModifiedAny())
        {
            ptACD->ModifiedClear();
        }
    }
}















void ActorCommonData::ChangeWorld(
    const WorldPlace & tPlace,
    const Quaternion & q)
{

    // Surviving call at 2656; erased diagnostic expression is not recoverable.
    GameActiveGameCommonDataIsClient();





    if (!GameActiveGameCommonDataIsClient())
    {
        if (GetLocationType() == AL_WORLD)
        {

            ActorCollisionDispose(*this);
        }
        else
        {
            ChangeLocation(AL_WORLD);
        }
    }

    ActorMovement * ptActorMovement = GetActorMovement();
    if (ptActorMovement)
    {
        ptActorMovement->PrepareToSnapToNewPlace();
    }

    WorldLocation & loc = tLocation.tWorldLocation;
    loc.transform.Set(q, tPlace.wp);
    loc.idSWorld = tPlace.idSWorld;
    loc.idSScene = SceneCommonGetSceneIDFromWorldPlace(tPlace, FALSE);

    if (!GameActiveGameCommonDataIsClient())
    {



        Player * ptPlayer = PlayerGetByACD(id);
        if (ptPlayer)
        {
            SWorld * ptSWorld = SWorldTryToGet(tPlace.idSWorld);








            if (IsDueling())
            {
                PVPRemoveDuelingBuff(*ptPlayer);

                ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDPlayer)
                {
                    PlayerList listDestPlayers(PlayerListGetElementPool());
                    PlayerListGetSingle(listDestPlayers, *ptPlayer);

                    FloatDataMessage tMessage;
                    tMessage.flData = ptACDPlayer->AttributesGetFloat(ATTRIBUTE_HITPOINTS_CUR);

                    MessageSendToClient(listDestPlayers, SMSG_FORCE_HEALTH_RESYNC, &tMessage, sizeof(tMessage));
                }



                SPlayerRecalculateUpscalingLevels();
            }


            SPlayerApplyLoadingBuff(*this, *this, FALSE, TRUE);
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, *ptPlayer);

            ServerMessageSendWorldRevealToPlayer(listDestPlayers, tPlace.idSWorld);

        }
        else
        {
            if (IsDueling())
            {
                PVPRemoveFollowerDuelingBuff(*this);
            }
        }


        ActorCollisionInitialize(*this);

        ActorID idActor = (ActorID)idOwner;
        Angle aNewYaw = q.ToYaw();
        if (ptActorMovement)
        {
            ptActorMovement->SetNewFacing(aNewYaw, TRUE, TRUE);
            ptActorMovement->ResetCache();
        }
        else
        {
            ActorInternalSetCurrentFacing(idActor, aNewYaw);
        }

        ActorUpdateOrientation(idActor, TRUE);
    }


    if (ptActorMovement)
    {
        ptActorMovement->SnapToNewPlace(tPlace, TRUE, FALSE, TRUE, 0);
    }
}




void ActorCommonData::MarkHeroStateChanged()
{
    dwFlags |= (1 << ACD_SEND_HERO_STATE_BIT);
}





void ActorCommonData::MarkHeroStateSavedDataChanged()
{
    dwFlags |= (1 << ACD_SEND_HERO_STATE_SAVEDDATA_BIT);
}





void ActorCommonData::MarkHeroStatePlayerFlagsChanged()
{
    dwFlags |= (1 << ACD_SEND_HERO_STATE_PLAYERFLAGS_BIT);
}





void ActorCommonData::MarkHeroStateWaypointsChanged()
{
    dwFlags |= (1 << ACD_SEND_HERO_STATE_WAYPOINTS_BIT);
}





void ActorCommonData::MarkHeroStateLastSavePointChanged()
{
    dwFlags |= (1 << ACD_SEND_HERO_STATE_LASTSAVEPOINT_BIT);
}





void ActorCommonData::MarkHeroStateLastWaypointLevelAreaChanged()
{
    dwFlags |= (1 << ACD_SEND_HERO_STATE_LAST_WAYPOINT_LEVELAREA_BIT);
}





void ActorCommonData::MarkHeroStateCurrencyChanged()
{
    dwFlags |= (1 << ACD_SEND_HERO_STATE_CURRENCY_BIT);
}




void ActorCommonData::SetGBHandle(const GBHandle _hGB)
{
    hGB = _hGB;
}




GBHandle ActorCommonData::GetGBHandle() const
{
    return hGB;
}



// NON_MATCHING: register swap only (original keeps this in x20 and gbidTransmog in x19; ours the reverse)
GBID ActorCommonData::GetVisualGBID() const

{
    GBID gbidTransmog = AttributesGetInt(FastAttribKey(ATTRIBUTE_TRANSMOG_GBID));
    GBID gbidTransmogTempOverride = AttributesGetInt(FastAttribKey(ATTRIBUTE_TRANSMOG_GBID_TEMP_OVERRIDE));
    if (gbidTransmogTempOverride != GBID_NONE)
    {
        gbidTransmog = gbidTransmogTempOverride;
    }

    if (gbidTransmog != GBID_NONE)
    {

        ACDID idACDOwner = ACDInventoryGetUltimateContainer(*this);
        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
        if (ptACDOwner == NULL)
            return gbidTransmog;
        if (ACDIsHireling(*ptACDOwner))
        {
            HirelingClass _eHirelingClass = HirelingGetClass(*ptACDOwner);
            if (_eHirelingClass != HC_NONE &&
                ItemUsableByHireling(gbidTransmog, _eHirelingClass))
            {
                return gbidTransmog;
            }
        }
        else
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(ptACDOwner);
            if (ePlayerClass != PLAYER_CLASS_NONE &&
                ItemMeetsOwnerClassRestrictions(gbidTransmog, ePlayerClass, FALSE))
            {
                return gbidTransmog;
            }
        }
    }




    return hGB.gbid;
}



BOOL ActorCommonData::HasInventory() const
{
    return ptInventory != NULL;
}









BOOL ActorCommonData::HasVisualInventory() const
{
    return ptVisualInventory != NULL;
}









void ActorCommonData::SetTimeStamp(uint32 dwTime)
{
    dwLastSeen = dwTime;
}




uint32 ActorCommonData::GetTimeStamp() const
{
    return dwLastSeen;
}




void ActorCommonData::SetHitGroundTime(uint32 dwTime)
{
    dwHitGroundTime = dwTime;
}





uint32 ActorCommonData::GetHitGroundTime() const
{
    return dwHitGroundTime;
}





void ActorCommonData::SetKnownByPlayer(const Player & tPlayer, BOOL bKnown)
{
    if (bKnown)
    {
        dwKnownBits |= (1 << tPlayer.nPlayerIndex);
    }
    else
    {
        dwKnownBits &= ~(1 << tPlayer.nPlayerIndex);
    }
}




BOOL ActorCommonData::IsKnownByPlayer(const Player & tPlayer) const
{
    return (dwKnownBits >> tPlayer.nPlayerIndex) & 1;
}



























Angle ACDGetYaw(ACDID idACD)
{
    if (GameActiveGameCommonDataIsClient())
    {
        RActorID idRActor = (RActorID)ACDGetOwner(idACD);
        RActor * ptRActor = RActorGet(idRActor);
        return ptRActor->aYaw;
    }
    else
    {
        ActorID idActor = (ActorID)ACDGetOwner(idACD);
        Actor * ptActor = ActorGet(idActor);
        return ptActor->aYaw;
    }
}















void ACDSetYaw(ACDID idACD, const Angle aYaw)
{


    ActorCommonData * ptACD = ACDTryToGet(idACD);

    if (ptACD)
    {
        ActorMovement * ptMove = ptACD->GetActorMovement();
        if (ptMove)
        {
            ptMove->SetNewFacing(aYaw, FALSE, TRUE);
        }
    }
}



void ActorCommonData::SetFacing(const WorldPlace & placeDest, BOOL bSnapToFacing)
{

    if (GameActiveGameCommonDataIsClient())
    {


        WorldPlace placeCurrent = GetPlace();

        if (!WorldPlaceInSameWorld(placeCurrent, placeDest))
        {
            return;
        }

        RActorID idRActor = (RActorID)ACDGetOwner(id);
        RActor * ptRActor = RActorGet(idRActor);

        if (ptRActor->ptCActorMovement)
        {
            ptRActor->ptCActorMovement->FaceTarget(placeDest.wp, bSnapToFacing);
        }
    }
    else
    {



        ActorID idActor = (ActorID)idOwner;
        Actor * ptActor = ActorGet(idActor);

        if (ptActor->ptSActorMovement)
        {
            ptActor->ptSActorMovement->FaceTarget(placeDest.wp, bSnapToFacing);
        }
    }
}






void ActorCommonData::Heal(float flHitPointsToHeal, const BOOL bSendEvent)
{


    if (!fIsAlive)
        return;



    if (flHitPointsToHeal > 0.0f && AttributesGetInt(FastAttribKey(ATTRIBUTE_HEALING_SUPPRESSED)))
        return;




    float flHPCurrent = GetHitPointsCurrent();



    float flHitPointPercent = 1.0f + AttributesGetFloat(FastAttribKey(ATTRIBUTE_BONUS_HEALING_RECEIVED_PERCENT)) - AttributesGetFloat(FastAttribKey(ATTRIBUTE_REDUCED_HEALING_RECEIVED_PERCENT));


    flHitPointPercent = Math::Max(flHitPointPercent, 0.0f);
    flHitPointsToHeal *= flHitPointPercent;

    BOOL bChanged = SetHitPoints(Math::Min(flHPCurrent + flHitPointsToHeal, GetHitPointsMaxTotal()), bSendEvent, TRUE);
    if (bChanged)
    {


        if (GameActiveGameCommonDataIsServer() && ACDIsPlayer(id))
        {

            PlayerRecordHeal(*this, flHitPointsToHeal);
        }
    }
}


void ActorCommonData::HealFromResourceSpent(float flResourceSpent, ResourceType eType)
{


    float flManaHeals = ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, eType);
    if (flManaHeals > 0.000001f)
    {
        float flHealingAmount = flManaHeals * flResourceSpent;
        float flMaxHealth = GetHitPointsMaxTotal();
        float flCurrentHealth = GetHitPointsCurrent();
        flHealingAmount = Math::Min(flHealingAmount, flMaxHealth - flCurrentHealth);
        Heal(flHealingAmount, TRUE);
    }
}




static const float HITPOINTS_MAX = 3.402823466e+38f;

BOOL ActorCommonData::SetHitPoints(const float flHitPoints, const BOOL bSendEvent, const BOOL bCheckHealthFrozen)
{
    BOOL bValueChanged = FALSE;

    if (bCheckHealthFrozen && AttributesGetInt(FastAttribKey(ATTRIBUTE_HITPOINTS_FROZEN)))
        return FALSE;




    float flNewHP = Math::Pin(flHitPoints, 0.0f, FLT_MAX);


    float flDamageCapPercent = AttributesGetFloat(FastAttribKey(ATTRIBUTE_DAMAGE_CAP_PERCENT));
    if (flDamageCapPercent > EPSILON)
    {
        float flHPMaxTotal = GetHitPointsMaxTotal();
        if (flNewHP < flDamageCapPercent * flHPMaxTotal)
            flNewHP = flDamageCapPercent * flHPMaxTotal;
    }



    float flHitPointsPrev = AttributesSetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_CUR), flNewHP);

























    if (Math::Abs(flHitPointsPrev - flNewHP) > EPSILON)
    {

        if (bSendEvent)
        {
            float _flHitPointsMax = GetHitPointsMaxTotal();
            ScriptEventOnActorHealthEnteredRange(id, flHitPointsPrev, flNewHP, _flHitPointsMax);
        }
        bValueChanged = TRUE;
    }

    return bValueChanged;
}










void ActorCommonData::SetHitPointsMax(const float flHitPointsMax)
{
    float flHPMax = Math::Pin(flHitPointsMax, 0.0f, HITPOINTS_MAX);
    AttributesSetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_MAX), flHPMax);
}





float ActorCommonData::AddResource(ResourceType eResourceType, float flAmount, BOOL bApplyOnHitBonuses)
{


    if (bApplyOnHitBonuses)
    {

        float flBonusPct = 1.0f + ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_HIT_BONUS_PCT, eResourceType);
        float flFlatBonus = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_ON_HIT_BONUS, eResourceType);
        flAmount = flAmount * flBonusPct + flFlatBonus;
    }


    float flCur = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eResourceType);
    float flMax = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResourceType);

    float flNewCur = Math::Pin(flCur + flAmount, 0.0f, flMax);
    ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flNewCur, eResourceType);

    return flNewCur - flCur;
}





BOOL ActorCommonData::SpendResource(float flAmountToSpend, BOOL bIgnoreMinimumResourceCost, ResourceType eType, Power * ptPower, BOOL bDoHeal)
{




    BOOL bIsPlayer = ACDIsPlayer(id);
    if (bIsPlayer)
    {


        flAmountToSpend = Math::Max(bIgnoreMinimumResourceCost ? 0.0f : 1.0f, flAmountToSpend);
    }






    if (eType == RESOURCE_TYPE_INVALID)
    {
        eType = GetPrimaryResourceType();
    }


    float flOldValue = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eType);
    if (flAmountToSpend > flOldValue)
    {
        return FALSE;
    }

    ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flOldValue - flAmountToSpend, eType);

    if (bDoHeal)
    {
        HealFromResourceSpent(flAmountToSpend, eType);
    }



    if (bIsPlayer)
    {




        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(PlayerGetByACD(id));

        if (tEvaluationData.GetPlayer())
        {
            tEvaluationData.SetPower(ptPower);

            tEvaluationData.EvaluateEvent(
                tEvaluationData.GetPlayer(),
                Achievement::ACH_EVENT_USE_RESOURCE,
                eType,
                (int32)ceilf(flAmountToSpend));
        }
    }



    return TRUE;
}







PickupResponse ActorCommonData::AutoPickup(const ActorCommonData & tACDItem)
{

    EffectType ePickupEffect = ItemHasLabel(tACDItem.hGB.gbid, ITL_POTION) ? EFFECT_PLAY_POTION_AUTOPICKUP_SOUND : EFFECT_INVALID;




    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_BLOODSHARD))
    {
        ePickupEffect = EFFECT_BLOOD_SHARD_ON_PICKUP;
    }

    return SACDInventoryPickup(ACDGetControllingActor(id), tACDItem.id, ePickupEffect, TRUE, FALSE, NULL, TRUE, FALSE, IsPet(), TRUE);
}





void ActorCommonData::PickupNearby()
{

    if (GameActiveGameCommonDataIsClient())
    {
        return;
    }


    ScopedActorDefinitionLockR pdefActor(snoActor);
    BOOL bIsPlayer = ACDIsPlayer(id);
    if (!bIsPlayer &&
        !TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_PICKUP_ITEMS) &&
        AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE)) != PETTYPE_COMPANION)
    {
        return;
    }



    if (!bIsPlayer && !GetPetOwnerPlayer())
    {
        return;
    }

    if (IsObserver())
    {
        return;
    }


    ScopedGlobalDefinitionLockR pdefGlobal;


    const WorldDistance wdPickupRadius = pdefGlobal->ptServerData->flBaseGoldPickupRadius + AttributesGetFloat(FastAttribKey(ATTRIBUTE_GOLD_PICKUP_RADIUS));
    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectSphere(GetPlace(), wdPickupRadius, id, listACDs, 4);

    PickupResponse eErrorResponse = PICKUP_RESPONSE_OK;
    ACDID idACDItemError = ACDID_INVALID;
    BOOL bErrorItemIsCurrency = FALSE;
    POS pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDItem = (ACDID)listACDs.GetNext(pos);


        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
        if (!ptACDItem)
        {
            continue;
        }

        if (ptACDItem->IsItem() &&
            ItemIsAutoPickup(*ptACDItem) &&
            (!IsPet() || ItemIsGold(*ptACDItem)))
        {
            if (ItemIsGlyph(*ptACDItem) && AttributesGetInt(FastAttribKey(ATTRIBUTE_GHOSTED)))
            {
                continue;
            }

            if (ItemIsGlyph(*ptACDItem) && AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_IGNORES_PICKUP_RADIUS_FOR_PICKUP)))
            {
                if (Vector3D::Distance2DSquared(GetPlace().wp, ptACDItem->GetPlace().wp) > pdefGlobal->ptServerData->flBaseGoldPickupRadius)
                {
                    continue;
                }
            }


            if (ptACDItem->dwFlags & (1 << ACD_IS_BEING_PICKED_UP))
            {
                continue;
            }



                ptACDItem->dwFlags |= (1 << ACD_IS_BEING_PICKED_UP);

                BOOL bIsCurrency = ItemIsCurrency(*ptACDItem);
                PickupResponse eResponse = AutoPickup(*ptACDItem);
                if (eResponse != PICKUP_RESPONSE_OK && eResponse != PICKUP_RESPONSE_OWNED_BY_OTHER)
                {
                    bErrorItemIsCurrency = bIsCurrency;
                    idACDItemError = idACDItem;
                    eErrorResponse = eResponse;
                }


                ptACDItem->dwFlags &= ~(1 << ACD_IS_BEING_PICKED_UP);

        }
    }

    if (eErrorResponse != PICKUP_RESPONSE_OK)
    {
        Player * ptPlayer = PlayerGetByACD(id);
        if (ptPlayer)
        {
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetSingle(listDestPlayers, *ptPlayer);
            ACDPickupFailedMessage tMessage;
            tMessage.bIsCurrency = bErrorItemIsCurrency;
            tMessage.ePickupResponse = eErrorResponse;
            tMessage.annPickee = ANNFromACD(ptPlayer->idPlayerACD);
            MessageSendToClient(listDestPlayers, SMSG_ACD_PICKUP_FAILED, &tMessage, sizeof(tMessage));
        }
    }

}



void ActorCommonData::PickupLine(Vector3D wpStart, Vector3D wpEnd)
{

    if (!ACDIsPlayer(id))
        return;

    const WorldDistance wdPickupRadius = 1.5f;
    POS pos;




    Capsule wCapsule;
    wCapsule.wp1 = wpStart;
    wCapsule.wp2 = wpEnd;
    wCapsule.flRadius = wdPickupRadius;

    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());

    ACDsIntersectCapsule(GetPlace().idSWorld, wCapsule, listACDs, FALSE);

    pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACDItem = (ACDID)listACDs.GetNext(pos);

        ActorCommonData & tACDItem = ACDGet(idACDItem);
        if (tACDItem.IsItem() &&
            ItemIsAutoPickup(tACDItem))
        {

            AutoPickup(tACDItem);
        }
    }

}
BOOL ActorCommonData::ShouldUseAltLevels() const
{
    if (ACDIsPlayer(id))
    {
        int32 nLevel = AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
        int32 nLevelCap = AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL_CAP));
        if (nLevelCap >= 60 && nLevel >= nLevelCap)
        {
            return TRUE;
        }
    }
    return FALSE;
}







BOOL ActorCommonData::CanGainMoreExperience() const
{
    if (ShouldUseAltLevels())
    {
        int32 nLevel = AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL));
        return nLevel < 20000;
    }
    int32 nLevel = AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    int32 nLevelCap = AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL_CAP));
    return nLevel < nLevelCap;
}









void ActorCommonData::RestorePermanentPowers()
{

    if (IsObserver())
    {
        return;
    }


    if (PlayerGetPlayerClass(id) == PLAYER_CLASS_BARBARIAN)
    {

        PowerStartData tPowerStartData;
        PowerStartDataInit(tPowerStartData);
        tPowerStartData.idACD = id;
        tPowerStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_BARBARIAN_FURY);
        PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
    }

    ApplyAllPassives(FALSE);

}


void ActorCommonData::ApplyAllPassives(
    BOOL bApplyOnlyDualWieldPassives)
{




    if (!ACDIsPlayer(id) && !ACDIsHireling(*this))
    {
        return;
    }

    if (!fIsAlive)
    {
        return;
    }

    if (!IsInWorld())
    {
        return;
    }

    SNO snoSkillKit = GetSkillKit();
    if (snoSkillKit == SNO_NONE)
    {
        return;
    }

    if (dwFlags & (1 << ACD_SUPPRESS_PASSIVE_APPLICATION))
    {
        return;
    }

    int32 eBodySlotStart = -1;
    int32 eBodySlotEnd = -1;
    if (ACDIsPlayer(id))
    {
        eBodySlotStart = SLOT_PLAYER_BODYSLOT_START;
        eBodySlotEnd = SLOT_PLAYER_BODYSLOT_END;
    }

    else if (ACDIsHireling(*this))
    {
        eBodySlotStart = SLOT_HIRELING_START;
        eBodySlotEnd = SLOT_HIRELING_END;
    }


    XPooledDataIDList listPassiveSNOs(MemoryPoolsGetDATAIDListPool());

    for (int32 i = eBodySlotStart; i < eBodySlotEnd; i++)
    {
        ACDInventoryIterateItems(*this, i, 0, ActorInventoryIterateGetItemPassivePowers, &listPassiveSNOs);
    }



    {
        POS posBuff = listBuffs.GetHeadPosition();
        while (posBuff)
        {
            BuffID idBuff = listBuffs.GetNext(posBuff);

            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff &&
                !ptBuff->bDeleted &&

                ptBuff->snoPower == GlobalSNOGet(GLOBALSNO_POWERS_SET_ITEM_BONUS_BUFF))
            {
                FastAttribIterator tIt(ptBuff->idFastAttrib, TRUE);
                while (FastAttribIterate(tIt))
                {
                    FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
                    Attrib eAttribute = KeyGetAttrib(tKey);

                    CRefString sStringListLabel;
                    if (eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                    {
                        SNO snoPassivePower = KeyGetParam(tKey);
                        listPassiveSNOs.AddTail_Tracked(snoPassivePower);
                    }
                }
            }
        }
    }



    Player * ptPlayer = PlayerGetByACD(id);
    if (ptPlayer)
    {
        for (int32 i = 0; i < 3; i++)
        {
            GBID gbidLegendary = ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[i];
            if (gbidLegendary != GBID_NONE)
            {

                SNO snoPower = ItemGetLegendaryPower(gbidLegendary);
                if (snoPower != SNO_NONE && !listPassiveSNOs.Find(snoPower))
                {
                    listPassiveSNOs.AddTail_Tracked(snoPower);
                }
            }
        }
    }


















    POS pos = listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff &&
            !ptBuff->bDeleted &&
            ptBuff->bInfinite)
        {



            if (bApplyOnlyDualWieldPassives &&

                !PowerAffectedByDualWield(ptBuff->snoPower))
            {
                continue;
            }


            if (PowerIsPassive(AutoPowerDefinitionR(ptBuff->snoPower)))
            {
                if (ptBuff->bRemoveOnPassiveApplication)
                {
                    BuffRemovalReason eRemovalReason = BUFF_REMOVAL_REASON_REPLACED;

                    SNO snoBuff = ptBuff->snoPower;
                    if (PowerIsPassive(AutoPowerDefinitionR(snoBuff)) && PowerGeneralTagGetBool(snoBuff, TAG_POWER_IS_ITEM_POWER))
                    {
                        if (!listPassiveSNOs.Find(snoBuff))
                        {
                            eRemovalReason = BUFF_REMOVAL_REASON_ITEM_REMOVED;
                        }
                    }

                    PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, eRemovalReason, TRUE);
                }

                AttributesSetInt(MakeAttribKey(ATTRIBUTE_TRAIT, ptBuff->snoPower), 0);
            }
        }
    }
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);



    XPooledDataIDList listDelayedTraitSNOs(MemoryPoolsGetDATAIDListPool());
    int32 nCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
    for (int32 i = 0; i < nCount; i++)
    {
        TraitEntry & tEntry = pdefSkillKit->arTraitEntries[i];
        if (tEntry.snoPower != SNO_NONE)
        {
            if (bApplyOnlyDualWieldPassives &&

                !PowerAffectedByDualWield(tEntry.snoPower))
            {
                continue;
            }


            if (PowerApplyPassiveAfterItemPassives(tEntry.snoPower))
            {
                listDelayedTraitSNOs.AddTail_Tracked(tEntry.snoPower);
            }
            else
            {
                ApplyPassive(tEntry.snoPower, FALSE);
            }
        }
    }





    SPlayerApplyPerkBuffs(*this);

    SPlayerApplyParagonBuff(*this);

    SPlayerApplyEquippedLegendaryPowerBuffs(*this);



    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_DUAL_WIELD_BUFF);
    if (snoBuff != SNO_NONE)
    {
        if (ACDIsDualWielding(*this, TRUE))
        {
            PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
            tBuffStartData.bInfinite = TRUE;
            tBuffStartData.idACDSource = id;
            tBuffStartData.snoPower = snoBuff;
            tBuffStartData.eStackType = 0;
            tBuffStartData.bKeepForProfileSnapshot = TRUE;
            tBuffStartData.bPositiveEffect = TRUE;
            float flAttackSpeedIncrease = PowerGetFormulaValue(this, snoBuff, TAG_POWER_ATTACK_SPEED_PERCENT_INCREASE_MIN, -1, ATTRIBGROUPID_INVALID, 0);
            FastAttribSetValueFloat(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT), flAttackSpeedIncrease);
            PowerAPI_BuffAddToACD(tBuffStartData, *this);
        }
        else
        {
            BuffID idBuffDualWield = PowerAPI_BuffFindOnACD(snoBuff, *this, ACDID_INVALID);
            if (idBuffDualWield != -1)
            {
                PowerAPI_BuffFlagForDeletion(idBuffDualWield, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            }
        }
    }

    POS posPassive = listPassiveSNOs.GetHeadPosition();
    while (posPassive)
    {
        SNO snoPowerPassive = listPassiveSNOs.GetNext(posPassive);
        if (PowerApplyPassiveAfterItemPassives(snoPowerPassive))
        {
            listDelayedTraitSNOs.AddTail_Tracked(snoPowerPassive);
        }
        else
        {
            ApplyPassive(snoPowerPassive, TRUE);
        }
    }

    POS posBuff = listBuffs.GetHeadPosition();
    while (posBuff)
    {
        BuffID idBuff = listBuffs.GetNext(posBuff);

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff &&
            !ptBuff->bDeleted &&

            ptBuff->snoPower == GlobalSNOGet(GLOBALSNO_POWERS_SET_ITEM_BONUS_BUFF))
        {
            FastAttribIterator tIt(ptBuff->idFastAttrib, TRUE);
            while (FastAttribIterate(tIt))
            {
                FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
                Attrib eAttribute = KeyGetAttrib(tKey);

                CRefString sStringListLabel;
                if (eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                {
                    SNO snoPassivePower = KeyGetParam(tKey);
                    if (PowerApplyPassiveAfterItemPassives(snoPassivePower))
                    {
                        listDelayedTraitSNOs.AddTail_Tracked(snoPassivePower);
                    }
                    else
                    {
                        ApplyPassive(snoPassivePower, TRUE);
                    }
                }
            }
        }
    }


    POS posTrait = listDelayedTraitSNOs.GetHeadPosition();
    while (posTrait)
    {
        SNO snoPowerTrait = listDelayedTraitSNOs.GetNext(posTrait);
        ApplyPassive(snoPowerTrait, TRUE);
    }



    nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    for (int32 i = 0; i < nCount; i++)
    {
        ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        if (tEntry.snoPower != SNO_NONE)
        {
            if (bApplyOnlyDualWieldPassives &&

                !PowerAffectedByDualWield(tEntry.snoPower))
            {
                continue;
            }


            ApplyPassive(tEntry.snoPower, FALSE);

            int32 nMaxCharges = PowerActorGetMaxCharges(*this, tEntry.snoPower);
            if (nMaxCharges > 0)
            {
                SPlayerRegisterPowerForRecharge(PlayerGetByACD(id), tEntry.snoPower);
            }
        }
    }






}



void ActorCommonData::ApplyPassive(
    const SNO snoPower,
    const BOOL bIgnoreSkillLevel)
{




    int32 nLevel = AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower));
    if (nLevel > 0 || bIgnoreSkillLevel)
    {


        if (Console::Online::GetSNOBlacklistData(SNOName(POWER_SNO, snoPower), NULL))
        {
            return;
        }












        SNO snoPassivePower = snoPower;

        if (!PowerIsPassive(AutoPowerDefinitionR(snoPower)))
        {
            snoPassivePower = PowerGeneralTagGetSNO(snoPower, TAG_POWER_SYNERGY_POWER);
            if (snoPassivePower != SNO_NONE)
            {
                AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPassivePower), nLevel);
                AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPassivePower), AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower)));
                AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPassivePower), AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower)));
                AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPassivePower), AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower)));
                AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPassivePower), AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower)));
                AttributesSetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPassivePower), AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower)));
            }
        }


        if (PowerIsPassive(AutoPowerDefinitionR(snoPower)))
        {
            AttributesSetInt(MakeAttribKey(ATTRIBUTE_TRAIT, snoPower), 1);
        }

        if (snoPassivePower != SNO_NONE)
        {
            if (PowerIsCoolDownDone(*this, snoPassivePower))
            {
                PowerStartData tPowerStartData;
                PowerStartDataInit(tPowerStartData);
                tPowerStartData.idACD = id;
                tPowerStartData.snoPower = snoPassivePower;
                PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
            }
        }
    }

}



int32 ActorCommonData::GetRuneUnlockLevel(const SNO snoPower, const RuneType eRuneType) const
{
    if (eRuneType == RUNE_TYPE_INVALID)
    {
        return 0;
    }

    SNO snoSkillKit = GetSkillKit();
    const ActiveSkillEntry * ptActiveSkillEntry = SkillKitGetMatchingActiveSkillEntry(snoSkillKit, snoPower);
    if (ptActiveSkillEntry == NULL)
    {
        return 0;
    }
    return ptActiveSkillEntry->GetRuneLevelRequirement(eRuneType);
}






const int32 ActorCommonData::GetComboLevel() const
{
    if (GameActiveGameCommonDataIsClient())
    {
        return CPlayerInputGetComboLevel(PlayerGetByACD(id));
    }
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_COMBO_LEVEL));
}





void ActorCommonData::SetComboLevel(int32 nComboLevel)
{
    if (GameActiveGameCommonDataIsClient())
    {
        CPlayerInputSetComboLevel(nComboLevel, PlayerGetByACD(id));
        return;
    }
    AttributesSetInt(FastAttribKey(ATTRIBUTE_COMBO_LEVEL), nComboLevel);
}








static BOOL sResetResourceToFull(
    const ACDID idACD,
    const ResourceType eResourceType)
{
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    if (ActorFlagIsSet(idActor, ACTOR_GOD_MODE_BIT))
    {
        return TRUE;
    }



    return eResourceType == RESOURCE_TYPE_MANA ||
        eResourceType == RESOURCE_TYPE_ARCANUM ||
        eResourceType == RESOURCE_TYPE_SPIRIT ||
        eResourceType == RESOURCE_TYPE_HATRED ||
        eResourceType == RESOURCE_TYPE_DISCIPLINE ||
        eResourceType == RESOURCE_TYPE_FAITH ||
        eResourceType == RESOURCE_TYPE_ESSENCE;






}


static float sGetResurrectResourceValue(
    const ActorCommonData & tACD,
    const ResourceType eResourceType,
    BOOL bFromResurrection)
{
    float flManaMax = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_EFFECTIVE_MAX, eResourceType);
    float flValue = flManaMax;
    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
    if (bFromResurrection &&
        !ActorFlagIsSet(idActor, ACTOR_GOD_MODE_BIT) && eResourceType == RESOURCE_TYPE_SPIRIT)
    {

        flValue = Math::Min(75.0f, flManaMax);
    }

    return flValue;
}




BOOL ActorCommonData::ResetLifeStats(
    BOOL bForHealing,
    BOOL bSendEvent,
    BOOL bRestoreResource,
    BOOL bResetComboLevels,
    BOOL bFromResurrection)
{







    BOOL fDidHealing = FALSE;


    BOOL fDidStatusRestore = RestoreStatus();


    float _flHitPointsCurrent = GetHitPointsCurrent();
    if (!fIsAlive)
    {

        SetHitPoints(1.0f, FALSE, FALSE);
        fDidHealing = TRUE;
    }

    if (IsInWorld())
    {
        RestorePermanentPowers();
    }


    float flHitPointsMax = GetHitPointsMaxTotal();
    if (_flHitPointsCurrent != flHitPointsMax)
    {
        SetHitPoints(flHitPointsMax, bSendEvent, FALSE);
        fDidHealing = TRUE;
    }

    if (bRestoreResource)
    {
        ResourceType eResourceType = GetPrimaryResourceType();
        ResourceType eSecondaryType = GetSecondaryResourceType();
        if (eResourceType != RESOURCE_TYPE_INVALID)
        {
            BOOL fResetToMax = sResetResourceToFull(id, eResourceType);
            if (fResetToMax)
            {
                float flManaRestore = sGetResurrectResourceValue(*this, eResourceType, bFromResurrection);
                float flManaCurrent = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eResourceType);
                if (flManaCurrent < flManaRestore)
                {
                    ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flManaRestore, eResourceType);
                    fDidHealing = TRUE;
                }

                if (eSecondaryType != RESOURCE_TYPE_INVALID)
                {
                    flManaRestore = sGetResurrectResourceValue(*this, eSecondaryType, bFromResurrection);
                    flManaCurrent = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eSecondaryType);
                    if (flManaCurrent < flManaRestore)
                    {
                        ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, flManaRestore, eSecondaryType);
                        fDidHealing = TRUE;
                    }
                }
            }
            else
            {

                if (!bForHealing)
                {
                    ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, 0.0f, eResourceType);
                    ResourceAttributeSetFloat(ATTRIBUTE_RESOURCE_CUR, 0.0f, eSecondaryType);
                }
            }
        }
    }

    if (bResetComboLevels)
    {
        AttributesSetInt(ATTRIBUTE_COMBO_LEVEL, 0);
        AttributesSetInt(ATTRIBUTE_COMBO_TIME_LAST_MOVE, 0);
    }

    return fDidHealing || fDidStatusRestore;
}






BOOL ActorCommonData::RestoreStatus()
{
    BOOL bDidRestore = FALSE;



    POS posBuff = listBuffs.GetHeadPosition();

    while (posBuff)
    {
        BuffID idBuff = listBuffs.GetNext(posBuff);

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff && !ptBuff->bDeleted &&
            !ptBuff->bPositiveEffect &&
            ptBuff->bRemoveOnDeath)
        {
            bDidRestore = TRUE;
            PowerAPI_BuffFlagForDeletion(ptBuff->id, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }


    int32 nStunned = AttributesGetInt(FastAttribKey(ATTRIBUTE_STUNNED));
    if (nStunned)
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_STUNNED), 0);
        bDidRestore = TRUE;
    }

    int32 nForceGripped = AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCE_GRIPPED));
    if (nForceGripped)
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_FORCE_GRIPPED), 0);
        bDidRestore = TRUE;
    }

    int32 nFrozen = AttributesGetInt(FastAttribKey(ATTRIBUTE_FROZEN));
    if (nFrozen)
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_FROZEN), 0);
        AttributesSetFloat(FastAttribKey(ATTRIBUTE_FREEZE_DAMAGE), 0.0f);
        bDidRestore = TRUE;
    }

    int32 nChilled = AttributesGetInt(FastAttribKey(ATTRIBUTE_CHILLED));
    if (nChilled)
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_CHILLED), 0);
        AttributesSetFloat(FastAttribKey(ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT), 0.0f);
        bDidRestore = TRUE;
    }

    int32 nSlow = AttributesGetInt(FastAttribKey(ATTRIBUTE_SLOW));
    if (nSlow)
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_SLOW), 0);
        AttributesSetFloat(FastAttribKey(ATTRIBUTE_MOVEMENT_SCALAR_REDUCTION_PERCENT), 0.0f);
        bDidRestore = TRUE;
    }

    return bDidRestore;
}




void ActorCommonData::ResetRestoredPlayerStats()
{

    for (int32 iTimerType = 0; iTimerType < NUM_ATTRIBUTE_TIMER_TYPES; iTimerType++)
    {
        int32 nDuration = AttributesGetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, iTimerType));
        if (nDuration > 0)
        {
            const ActorID idActor = (ActorID)idOwner;
            ActorTimerEffectsExpireGroup(idActor, iTimerType);
            AttributesSetInt(MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, iTimerType), 0);
        }
    }
}





void ActorCommonData::HealAndRestoreAllies()
{
    NavPlace tNavPlace;
    float fRadius = 50.0f;

    ACDGetNavMeshHandleWithPlace(*this, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);


    PowerAPIChooseTargets tChooseTargets;
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerHelper_ChooseTargetsInit(POWERID_INVALID, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, fRadius, tChooseTargets.tGeometry);
    tChooseTargets.dwFlags |= (1 << CTF_DONT_PERFORM_LOS_CHECK);
    PowerAPI_DoFindAllPossibleTargets(tChooseTargets, listTargets);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData & tACDTarget = ACDGet(idACDTarget);


        if (ACDAffectedByHealthGlobes(tACDTarget) &&
            ACDIsFriendly(tACDTarget, *this, FALSE))
        {
            tACDTarget.ResetLifeStats(TRUE, TRUE, FALSE, FALSE, FALSE);



            BOOL bRestoreResource = tACDTarget.AttributesGetInt(FastAttribKey(ATTRIBUTE_HEALING_WELL_RESTORES_RESOURCE));
            if (bRestoreResource)
            {
                ResourceType ePrimaryResourceType = tACDTarget.GetPrimaryResourceType();
                if (ePrimaryResourceType != RESOURCE_TYPE_INVALID)
                {
                    float flMax = tACDTarget.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, ePrimaryResourceType);
                    tACDTarget.AddResource(ePrimaryResourceType, flMax, FALSE);
                }

                ResourceType eSecondaryResourceType = tACDTarget.GetSecondaryResourceType();
                if (eSecondaryResourceType != RESOURCE_TYPE_INVALID)
                {
                    float flMax = tACDTarget.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eSecondaryResourceType);
                    tACDTarget.AddResource(eSecondaryResourceType, flMax, FALSE);
                }
            }



            BOOL fPlayEffect = !(tACDTarget.IsPet() ||
                                 tACDTarget.IsNPC());
            if (fPlayEffect)
            {




                ACDPlayEffect(tACDTarget.id, EFFECT_HEAL, 0, ACDID_INVALID);
            }
        }
    }
}







BOOL ActorCommonData::CanAnimate() const
{
    BOOL bCanAnimate = FALSE;
    ScopedActorDefinitionLockR pdefActor(snoActor);
    ScopedAppearanceDefinitionLockR pdefAppearance(pdefActor->snoAppearance);
    const Structure & tStructure = pdefAppearance->tStructure;
    if (!(tStructure.dwFlags & 1))
    {
        bCanAnimate = ActorGetAnim(snoActor, WEAPONCLASS_NONE, TAG_ACTOR_ANIMATION_NEUTRAL) != SNO_NONE;
    }
    return bCanAnimate;
}





















































































LevelAreaID ACDGetLevelAreaID(const ActorCommonData & tACD)
{
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACD, tNavMeshHandle);
    if (NavMeshHandleIsNone(tNavMeshHandle))
    {
        return -1;
    }
    return NavMeshHandleGetLevelAreaID(tNavMeshHandle, FALSE);
}












SNO ACDGetLevelArea(const ActorCommonData & tACD)
{
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACD, tNavMeshHandle);
    return NavMeshHandleGetLevelArea(tNavMeshHandle);
}





BOOL ACDIsInTown(const ActorCommonData & tACD)
{
    SNO snoLevelArea = ACDGetLevelArea(tACD);
    if (snoLevelArea != SNO_NONE)
    {
        return LevelAreaIsTown(snoLevelArea);
    }
    return FALSE;
}





BOOL ACDIsInTownPortalArea(const ActorCommonData & tACD)
{
    SSceneID idSScene = ACDGetCurrentSSceneID(tACD);
    if (idSScene == SSCENEID_INVALID)
    {
        return FALSE;
    }
    return SceneCommonCanTownPortal(idSScene, tACD);
}






BOOL ACDIsInNoBanterArea(const ActorCommonData & tACD)
{
    SNO snoLevelArea = ACDGetLevelArea(tACD);
    if (snoLevelArea != SNO_NONE)
    {
        return LevelAreaDisallowsBanter(snoLevelArea);
    }
    return FALSE;
}





void ACDIterateRadius(const WorldDistance & wdRadius, const ACDID idACDSource, ACDIterateRadiusFunc pfnIterateRadius, void * pData)
{




    POS pos;
    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectSphere(ACDGet(idACDSource).GetPlace(), wdRadius, idACDSource, listACDs,
                        7);




    pos = listACDs.GetHeadPosition();
    while (pos)
    {


        ACDID idACD = (ACDID)listACDs.GetNext(pos);
        if (idACD != ACDID_INVALID && pfnIterateRadius)
        {







            if (pfnIterateRadius(wdRadius, idACDSource, idACD, pData) == ACD_ITERATE_STOP)
            {
                break;
            }
        }
    }
}


ActorType ActorCommonData::GetActorType() const
{
    return eActorType;
}










Team ActorCommonData::GetTeam() const
{
    if (eTeamOverride != TEAM_INVALID)
    {
        return eTeamOverride;
    }
    return eTeam;
}














Team ActorCommonData::GetNaturalTeam() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_TEAM));
}





void ActorCommonData::SetTeam(Team _eTeam)
{
    AttributesSetInt(FastAttribKey(ATTRIBUTE_TEAM), _eTeam);
    eTeam = _eTeam;
    tCollSettings.eTeam = (uint8)_eTeam;
    Player * ptPlayer = PlayerGetByACD(id);
    if (ptPlayer)
    {
        ptPlayer->eTeam = _eTeam;
    }
}









void ActorCommonData::GetStartLocation(WorldPlace & tPlace, Quaternion & q)
{

    BOOL bResult = FALSE;










    if (SceneCommonDataTryToGet(tLocation.tWorldLocation.idSScene) != NULL)
    {
        NavMeshHandle tNavMeshHandle;
        ACDGetNavMeshHandle(*this, tNavMeshHandle);
        if (!NavMeshHandleIsNone(tNavMeshHandle))
        {
            PassabilitySettings tPassability;
            PassabilitySettingsInit(tPassability);
            tPassability.ePassabilityType = PASSABILITY_MONSTER;

            NavMeshHandle tNavMeshHandleResult;
            bResult = FindPerturbedSpawnLocation(snoActor, tNavMeshHandle, tPassability, 0.0f, 0, tPlace, tNavMeshHandleResult, TEAM_MONSTER_0);
        }
    }

    if (!bResult)
    {
        tPlace = GetPlace();
    }

    q = GetOrientation();
}

































void ACDGetNavMeshHandle(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle)
{
    if (!tACD.IsInWorld())
    {
        tNavMeshHandle.idSScene = SSCENEID_INVALID;
        tNavMeshHandle.nSquareIndex = -1;
        return;
    }
    const WorldLocation & tLoc = tACD.GetWorldLocation();
    {
        WorldPlace tPlace;
        tPlace.idSWorld = tLoc.idSWorld;
        tPlace.wp = tLoc.transform.wp;
        ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
    }
}





void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace)
{
    if (!tACD.IsInWorld())
    {
        tNavMeshHandle.idSScene = SSCENEID_INVALID;
        tNavMeshHandle.nSquareIndex = -1;
        return;
    }
    const WorldLocation & tLoc = tACD.GetWorldLocation();
    tPlace.idSWorld = tLoc.idSWorld;
    tPlace.wp = tLoc.transform.wp;
    ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
}








SSceneID ACDGetCurrentSSceneID(const ActorCommonData & tACD)
{
    if (!tACD.IsInWorld())
    {
        return SSCENEID_INVALID;
    }
    return tACD.GetWorldLocation().idSScene;
}

































SNO ActorCommonData::GetActorSNO() const
{
    return snoActor;
}





ACDID ACDFindClosestOfType(const SNO snoActor, const WorldDistance & wdRadius, const WorldPlace & tPlaceCenter)
{


    WorldDistance wdClosest = 0.0f;
    ACDID idACDClosest = ACDID_INVALID;
    POS pos;


    XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
    ACDsIntersectSphere(tPlaceCenter, wdRadius, ACDID_INVALID, listACDs, 7);

    pos = listACDs.GetHeadPosition();
    while (pos)
    {
        ACDID idACD = (ACDID)listACDs.GetNext(pos);


        ActorCommonData & tACD = ACDGet(idACD);
        if (tACD.GetActorSNO() == snoActor)
        {

            WorldDistance wdDist = Vector3D::DistanceSquared(tPlaceCenter.wp, tACD.GetPlace().wp);



            if (idACDClosest == ACDID_INVALID || wdDist < wdClosest)
            {
                idACDClosest = idACD;
                wdClosest = wdDist;
            }
        }
    }

    return idACDClosest;
}








































int32 ACDCountNearbySummonedUnits(const ActorCommonData & tACDSummoner, const WorldDistance & wdRadius)
{

    int32 nCount = 0;


    int32 dwSummonerID = tACDSummoner.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONER_ID));
    if (dwSummonerID != -1)
    {
        POS pos;
        XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(tACDSummoner.GetPlace(), wdRadius, ACDID_INVALID, listACDs, 7);

        pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)listACDs.GetNext(pos);


            ActorCommonData & tACD = ACDGet(idACD);
            if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID)) == dwSummonerID && tACD.IsAlive())
            {
                nCount++;
            }
        }

    }

    return nCount;
}


void ACDGetNearbySummonedUnits(const ActorCommonData & tACDSummoner, const WorldDistance & wdRadius, XPooledDataIDList & list)
{



    int32 dwSummonerID = tACDSummoner.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONER_ID));
    if (dwSummonerID != -1)
    {

        POS pos;
        XPooledDataIDList listACDs(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(tACDSummoner.GetPlace(), wdRadius, ACDID_INVALID, listACDs, 7);

        pos = listACDs.GetHeadPosition();
        while (pos)
        {
            ACDID idACD = (ACDID)listACDs.GetNext(pos);


            ActorCommonData & tACD = ACDGet(idACD);
            if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONED_BY_ACDID)) == dwSummonerID && tACD.IsAlive())
            {

                list.AddTail_Tracked(idACD);
            }
        }
    }
}



























































BOOL ACDIsInDeactivatedScene(const ActorCommonData & tACD)
{
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACD, tNavMeshHandle);
    return !NavMeshHandleIsActivated(tNavMeshHandle);
}













void ActorCommonData::ChangeActor(SNO snoNewActor)
{
    ACDPlayEffectChangeActor(id, snoNewActor);
    snoActor = snoNewActor;
    Actor * ptActor = ActorGet((ActorID)idOwner);
    ptActor->snoActor = snoNewActor;
}








DyeType ActorCommonData::GetDyeType() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_DYE_TYPE));
}





void ActorCommonData::ComplexEffectTrack(DataID idComplexEffect)
{

    ComplexEffectIDListPooled & _tListComplexEffects = tListComplexEffects;




    if (_tListComplexEffects.Find(idComplexEffect))
    {



        if (GameActiveGameCommonDataIsServer()) { SComplexEffect tSComplexEffect; SComplexEffectGet(id, idComplexEffect, tSComplexEffect); }
        return;
    }
















    _tListComplexEffects.AddTail_Tracked(idComplexEffect);
}
void ActorCommonData::ComplexEffectUntrack(const DataID idComplexEffect)
{

    ComplexEffectIDListPooled & _tListComplexEffects = tListComplexEffects;




    POS pos = _tListComplexEffects.Find(idComplexEffect);
    if (pos)
    {




        _tListComplexEffects.RemoveAt(pos);
    }
}
BOOL ActorCommonData::HasComplexEffects() const
{
    return !tListComplexEffects.IsEmpty();
}




void ActorCommonData::Hide(const BOOL bHidden)
{
    AttributesSetInt(FastAttribKey(ATTRIBUTE_HIDDEN), bHidden);
}




BOOL ActorCommonData::IsHidden() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_HIDDEN)) == TRUE;
}




BOOL ActorCommonData::IsStealthed() const
{
    return fIsStealthed;
}








BOOL ActorCommonData::IsEnraged() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_ENRAGED)) == TRUE;
}




void ActorCommonData::SetObserver(const BOOL bObserver)
{






    SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_OBSERVER_BUFF);

    if (bObserver)
    {
        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.snoPower = snoPower;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.bInfinite = true;

        tBuffStartData.bRemoveOnDeath = false;
        tBuffStartData.bPositiveEffect = true;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_OBSERVER), 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_UNTARGETABLE), 1);
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_INVULNERABLE), 1);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED), 1.0f);
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, FastAttribKey(ATTRIBUTE_MOVEMENT_SCALAR), 2.0f);

        tCollSettings.uStaticOneRequiredFlags = 7;

        tCollSettings.uDynamicFlagsNoOverlap = 0;
        tCollSettings.uDynamicMyFlags = 0;


        PowerAPI_BuffAddToACD(tBuffStartData, *this);
    }
    else
    {
        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPower, this, ACDID_INVALID);

        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff != NULL) { PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            ScopedActorDefinitionLockR ptDefActor(snoActor);
            tCollSettings.uStaticOneRequiredFlags = ptDefActor->tCollData.tCollFlags.uStaticOneRequiredFlags;
            tCollSettings.uDynamicMyFlags = ptDefActor->tCollData.tCollFlags.uDynamicMyFlags;
            tCollSettings.uDynamicFlagsNoOverlap = ptDefActor->tCollData.tCollFlags.uDynamicFlagsNoOverlap;
        }
    }
}


static void sApplyAttributeBuff(ActorCommonData & tACD, Attrib eAttrib, SNO snoPowerBuff, BOOL bSet)
{




    if (bSet)
    {


        PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
        tBuffStartData.bInfinite = true;
        tBuffStartData.snoPower = snoPowerBuff;
        tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
        tBuffStartData.bPositiveEffect = true;
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, FastAttribKey(eAttrib), 1);
        PowerAPI_BuffAddToACD(tBuffStartData, tACD);
    }
    else
    {

        BuffID idBuff = PowerAPI_BuffFindOnACD(snoPowerBuff, &tACD, ACDID_INVALID);
        if (idBuff != BUFFID_INVALID)
        {

            PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
        }
    }
}
void ActorCommonData::SetInvulnerable(const BOOL bInvulnerable)
{
    sApplyAttributeBuff(*this, ATTRIBUTE_INVULNERABLE, GlobalSNOGet(GLOBALSNO_POWERS_INVULNERABLE_BUFF), bInvulnerable);
}








void ActorCommonData::SetTakesNoDamage(const BOOL bNoDamage)
{
    sApplyAttributeBuff(*this, ATTRIBUTE_NO_DAMAGE, GlobalSNOGet(GLOBALSNO_POWERS_TAKES_NO_DAMAGE_BUFF), bNoDamage);
}








void ActorCommonData::SetCannotBeAddedToAITargetList(const BOOL bUntargetable)
{
    sApplyAttributeBuff(*this, ATTRIBUTE_CANNOT_BE_ADDED_TO_AI_TARGET_LIST, GlobalSNOGet(GLOBALSNO_POWERS_CANNOT_BE_ADDED_TO_AI_TARGET_LIST_BUFF), bUntargetable);
}








void ActorCommonData::SetDoesFakeDamage(const BOOL bFakeDamage)
{
    sApplyAttributeBuff(*this, ATTRIBUTE_DOES_FAKE_DAMAGE, GlobalSNOGet(GLOBALSNO_POWERS_DOES_FAKE_DAMAGE_BUFF), bFakeDamage);
}








void ActorCommonData::SetCannotDie(const BOOL bCannotDie)
{
    const float flDamageCap = 0.01f;


    AttributesSetFloat(FastAttribKey(ATTRIBUTE_DAMAGE_CAP_PERCENT), flDamageCap);
}
















BOOL ActorCommonData::IsInCombat() const
{
    if (!ACDIsPlayer(id) && !IsPet())
    {
        return TRUE;
    }
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_COMBAT));
}






BOOL ActorCommonData::IsBusy() const
{
    if (!ACDIsPlayer(id))
    {
        return FALSE;
    }
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_BUSY));
}





BOOL ActorCommonData::IsInConversation() const
{
    if (!ACDIsPlayer(id))
    {
        return FALSE;
    }
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_CONVERSATION));
}





BOOL ActorCommonData::IsInKnockback() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_KNOCKBACK));
}





void ActorCommonData::SetCombatStatus(BOOL bInCombat)
{
    AttributesSetInt(FastAttribKey(ATTRIBUTE_IN_COMBAT), bInCombat);
}





void ActorCommonData::SetCombatWithPlayerStatus(BOOL bInCombat)
{
    AttributesSetInt(FastAttribKey(ATTRIBUTE_IN_COMBAT_WITH_PLAYER), bInCombat);
}





BOOL ActorCommonData::IsAttached() const
{

    ACDNetworkName idANN = (ACDNetworkName)AttributesGetInt(FastAttribKey(ATTRIBUTE_ATTACHED_TO_ANN));
    return ACDTryToGet(ACDFromANN(idANN)) != NULL;
}









BOOL ActorCommonData::HasAttachedACD() const
{

    ACDNetworkName idANN = (ACDNetworkName)AttributesGetInt(FastAttribKey(ATTRIBUTE_ATTACHMENT_ANN));
    return ACDTryToGet(ACDFromANN(idANN)) != NULL;
}









void ActorCommonData::AttachACD(
    ActorCommonData & tACDAttachment)
{


    if (!tACDAttachment.fIsAlive)
    {
        return;
    }

    if (tACDAttachment.IsAttached() || IsAttached())
    {
        return;
    }

    if (tACDAttachment.HasAttachedACD() || HasAttachedACD())
    {
        return;
    }

    if (AnimPlayerAnimExists(*this, TAG_ACTOR_ANIMATION_GRAB_IDLE))
    {
        AnimPlayerSetIdleAnimation(*this, TAG_ACTOR_ANIMATION_GRAB_IDLE);
    }

    tACDAttachment.AttributesSetInt(ATTRIBUTE_ATTACHED_TO_ANN, ANNFromACD(id));
    AttributesSetInt(ATTRIBUTE_ATTACHMENT_ANN, ANNFromACD(tACDAttachment.id));


    BrainUpCallOnPowerStop((ActorID)ACDGetOwner(tACDAttachment.id), POWERID_INVALID, TRUE);

}




void ActorCommonData::DetachACD(ActorCommonData & tACDAttachment)
{




    tACDAttachment.AttributesSetInt(FastAttribKey(ATTRIBUTE_ATTACHED_TO_ANN), ANN_INVALID);
    AttributesSetInt(FastAttribKey(ATTRIBUTE_ATTACHMENT_ANN), ANN_INVALID);

    AnimPlayerSetIdleAnimation(*this, TAG_ACTOR_ANIMATION_NEUTRAL);


    if (tACDAttachment.fIsAlive)
    {

        BrainUpCallOnPowerStop((ActorID)ACDGetOwner(tACDAttachment.id), POWERID_INVALID, TRUE);
    }

    AnimPlayerPlayAnimation(tACDAttachment, TAG_ACTOR_ANIMATION_KNOCK_BACK_LAND, (uint32)-1, TRUE, ANIM_REASON_POWER, 0.0f, FALSE, 1.0f);
}







ACDID ActorCommonData::GetAttachedACD() const
{
    return ACDFromANN((ACDNetworkName)AttributesGetInt(FastAttribKey(ATTRIBUTE_ATTACHMENT_ANN)));
}





ACDID ActorCommonData::GetAttachedToACD() const
{
    return ACDFromANN((ACDNetworkName)AttributesGetInt(FastAttribKey(ATTRIBUTE_ATTACHED_TO_ANN)));
}





BOOL ActorCommonData::ShouldRun() const
{

    if (!ACDIsPlayer(id))
    {
        return FALSE;
    }


    AppGlobals & tAppGlobals = sgtAppGlobals;
    if (tAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FORCE_WALK_BIT))
    {
        return FALSE;
    }


    if (IsFeared() || ACDIsInTown(*this)) return TRUE;

    return TRUE;
}






WalkType AnimTagGetWalkType(uint32 dwAnimTag)
{
    WalkType eWalkType = 4;

    switch (dwAnimTag)
    {
    case TAG_ACTOR_ANIMATION_WALK:
    case TAG_ACTOR_ANIMATION_WALKSTART:
    case TAG_ACTOR_ANIMATION_WALKEND:
    case TAG_ACTOR_ANIMATION_TOWNWALK:
    case TAG_ACTOR_ANIMATION_TOWNWALKSTART:
    case TAG_ACTOR_ANIMATION_TOWNWALKEND:
    case TAG_ACTOR_ANIMATION_WALK_WARM_UP:
    case TAG_ACTOR_ANIMATION_WALK_COOL_DOWN:
    case TAG_ACTOR_ANIMATION_TOWN_WALK_COOL_DOWN:
        eWalkType = 0;
        break;
    case TAG_ACTOR_ANIMATION_RUN:
    case TAG_ACTOR_ANIMATION_RUNSTART:
    case TAG_ACTOR_ANIMATION_RUNEND:
    case TAG_ACTOR_ANIMATION_TOWNRUN:
    case TAG_ACTOR_ANIMATION_TOWNRUNSTART:
    case TAG_ACTOR_ANIMATION_TOWNRUNEND:
    case TAG_ACTOR_ANIMATION_RUN_WARM_UP:
    case TAG_ACTOR_ANIMATION_RUN_COOL_DOWN:
    case TAG_ACTOR_ANIMATION_TOWN_RUN_COOL_DOWN:
    case TAG_ACTOR_ANIMATION_RUNAWAY:
    case TAG_ACTOR_ANIMATION_CHARGING_RUN:
    case TAG_ACTOR_ANIMATION_CIRCLE:
    case TAG_ACTOR_ANIMATION_CIRCLELEFT:
    case TAG_ACTOR_ANIMATION_CIRCLERIGHT:
    case TAG_ACTOR_ANIMATION_CIRCLE_START:
    case TAG_ACTOR_ANIMATION_CIRCLELEFT_START:
    case TAG_ACTOR_ANIMATION_CIRCLERIGHT_START:
    case TAG_ACTOR_ANIMATION_CIRCLE_END:
    case TAG_ACTOR_ANIMATION_CIRCLELEFT_END:
    case TAG_ACTOR_ANIMATION_CIRCLERIGHT_END:
        eWalkType = 1;
        break;
    case TAG_ACTOR_ANIMATION_SPRINT:
    case TAG_ACTOR_ANIMATION_SPRINTSTART:
    case TAG_ACTOR_ANIMATION_SPRINTEND:
    case TAG_ACTOR_ANIMATION_BULL_RUSH:
    case TAG_ACTOR_ANIMATION_BULL_RUSH_START:
    case TAG_ACTOR_ANIMATION_BULL_RUSH_END:
        eWalkType = 2;
        break;
    case TAG_ACTOR_ANIMATION_STRAFE:
    case TAG_ACTOR_ANIMATION_STRAFELEFT:
    case TAG_ACTOR_ANIMATION_STRAFERIGHT:
    case TAG_ACTOR_ANIMATION_STRAFE_START:
    case TAG_ACTOR_ANIMATION_STRAFELEFT_START:
    case TAG_ACTOR_ANIMATION_STRAFERIGHT_START:
    case TAG_ACTOR_ANIMATION_STRAFE_END:
    case TAG_ACTOR_ANIMATION_STRAFELEFT_END:
    case TAG_ACTOR_ANIMATION_STRAFERIGHT_END:
        eWalkType = 3;
        break;
    }

    return eWalkType;
}







BOOL ACDIsProjectile(ACDID idACD)
{




    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor && ptActor->ptProjectile)
    {
        return TRUE;
    }
    return FALSE;
}





BOOL ActorCommonData::CanBeImmobilized() const
{
    if (AttributesGetInt(FastAttribKey(ATTRIBUTE_SLOWDOWN_IMMUNE)) > 0 ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_INVULNERABLE)) > 0 ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_STUN_IMMUNE)) > 0 ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_UNINTERRUPTIBLE)) > 0 ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_ROOT_IMMUNE)) > 0)
    {
        return FALSE;
    }
    return TRUE;
}






BOOL ActorCommonData::IsImmobilized() const
// if / return TRUE / return FALSE inferred: the original returns TRUE from the closing-brace line (5542) and the third test from 5536
{
    if (AttributesGetInt(FastAttribKey(ATTRIBUTE_IMMOBOLIZED)) > 0 ||
        IsRooted() ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_POWER_IMMOBILIZED)) != 0)
    {
        return TRUE;
    }

    return FALSE;
}




BOOL ActorCommonData::IsFeared() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_FEARED)) > 0;
}




BOOL ActorCommonData::IsStunned() const
{
    return fIsStunned;
}










BOOL ActorCommonData::IsBlind() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_BLIND)) > 0;
}





BOOL ActorCommonData::IsPoisoned() const
{
    return AttributesGetInt(MakeAttribKey(ATTRIBUTE_BUFF_VISUAL_EFFECT, ABT_POISONED)) > 0;
}





BOOL ActorCommonData::IsCharmed() const
{
    return fIsCharmed;
}









BOOL ActorCommonData::HasSpawner() const
{
    return fHasSpawner;
}









BOOL ActorCommonData::IsTaunted() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_TAUNTED)) > 0;
}





BOOL ActorCommonData::IsDueling() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_DUELING)) > 0;
}





ResourceType ActorCommonData::GetPrimaryResourceType() const
{

    return AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_TYPE_PRIMARY));
}




ResourceType ActorCommonData::GetSecondaryResourceType() const
{

    return AttributesGetInt(FastAttribKey(ATTRIBUTE_RESOURCE_TYPE_SECONDARY));
}



inline ResourceType ActorCommonData::sResolveResourceType(
    Attrib eAttrib,
    ResourceType eResourceType) const
{
    if (eResourceType == RESOURCE_TYPE_INVALID)
    {
        eResourceType = GetPrimaryResourceType();
    }
    return eResourceType;
}











































void ActorCommonData::ResourceAttributeSetFloat(Attrib eAttrib, float flVal, ResourceType eResourceType)
{




    eResourceType = sResolveResourceType(eAttrib, eResourceType);
    if (eResourceType < RESOURCE_TYPE_COUNT && eResourceType != RESOURCE_TYPE_INVALID)
    {
        float flOriginalValue = AttributesGetFloat(MakeAttribKey(eAttrib, eResourceType));
        float flDelta = flVal - flOriginalValue;

        if (eAttrib == ATTRIBUTE_RESOURCE_CUR && flDelta > 0.0f)
        {
            flVal += flDelta * AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESOURCE_GAIN_BONUS_PERCENT, eResourceType));
            float flMax = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResourceType);


            flVal = Math::Min(flVal, flMax);


            if (flVal == flMax && flVal != flOriginalValue)
            {





                Player * ptPlayer = PlayerGetByACD(id);
                if (ptPlayer)
                {
                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetPlayer(ptPlayer);
                    tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_MAXED_OUT_RESOURCE, eResourceType, 1);
                }
            }
        }

        else if (eAttrib == ATTRIBUTE_RESOURCE_CUR && flDelta < 0.0f)
        {
            float flMax = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eResourceType);
            if (flOriginalValue == flMax)
            {





                Player * ptPlayer = PlayerGetByACD(id);
                if (ptPlayer)
                {
                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetPlayer(ptPlayer);
                    tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_RESOURCE_NO_LONGER_MAXED, eResourceType, 1);
                }
            }
        }


        AttributesSetFloat(MakeAttribKey(eAttrib, eResourceType), flVal);

        if (flDelta > 0.0f)
        {
            PowerAPI_BuffApplyOnResourceGainEffects(id, flDelta);

            if (ACDIsPlayer(id))
            {
                Player * ptPlayer = PlayerGetByACD(id);
                if (ptPlayer)
                {





                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetPlayer(ptPlayer);

                    tEvaluationData.EvaluateEvent(
                        tEvaluationData.GetPlayer(), Achievement::ACH_EVENT_PLAYER_GAINED_RESOURCE, eResourceType, 1);




                }
            }
        }
    }
}







float ActorCommonData::ResourceAttributeGetFloat(
    Attrib eAttrib,
    ResourceType eResourceType) const
{
    eResourceType = sResolveResourceType(eAttrib, eResourceType);
    if (eResourceType >= RESOURCE_TYPE_COUNT || eResourceType == RESOURCE_TYPE_INVALID)  // early-return form inferred from the inlined copies in ResetLifeStats (block order)
        return 0.0f;
    return AttributesGetFloat(MakeAttribKey(eAttrib, eResourceType));


}














BOOL ActorCommonData::HasResurrectionPower() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_RESURRECTION_POWER)) != SNO_NONE;
}




BOOL ActorCommonData::IsRooted() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_IS_ROOTED)) != 0;
}




ACDID ActorCommonData::GetRootTarget() const
{
    return (ACDID)AttributesGetInt(FastAttribKey(ATTRIBUTE_ROOT_TARGET_ACD));
}




BOOL ActorCommonData::IsBurrowed() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_BURROWED)) != 0;
}




BOOL ActorCommonData::IsCCD() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_SLOW)) == 1 ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_CHILLED)) == 1 ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_STUNNED)) == 1 ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_CHILLED)) == 1 ||
        IsImmobilized() ||
        IsFeared() ||
        AttributesGetInt(FastAttribKey(ATTRIBUTE_FROZEN)) == 1 ||
        fIsCharmed ||
        IsBlind();
}



BOOL ActorCommonData::IsSlowed() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_SLOW)) == TRUE || AttributesGetInt(FastAttribKey(ATTRIBUTE_CHILLED)) == TRUE;
}





BOOL ActorCommonData::ModifyCurrencyAmount(int64 nAmount, CurrencyType eCurrencyType, GoldModifiedReason eReason, BOOL bForceNoSound)
{






    BOOL bPlaySound = !bForceNoSound &&

                      nAmount > 0;






    int64 nCount = ItemPlayerGetCurrencyCount(*this, eCurrencyType);
    if (nCount + nAmount < 0 && nAmount < 0)
    {
        return FALSE;
    }





    ItemPlayerModifyCurrency(*this, eCurrencyType, nAmount);
    SendCurrencyModifiedMessage(bPlaySound, eCurrencyType, eReason);
    return TRUE;
}
void ActorCommonData::SendCurrencyModifiedMessage(const BOOL bPlaySound, const CurrencyType eCurrencyType, const GoldModifiedReason eReason)
{









    if (ACDIsPlayer(id))
    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        Player * ptPlayer = PlayerGetByACD(id);
        if (ptPlayer)
        {
            PlayerListGetSingle(listDestPlayers, *ptPlayer);
            CurrencyModifiedMessage tMessage;

            tMessage.bPlaySound = bPlaySound;
            tMessage.eCurrencyType = eCurrencyType;
            tMessage.eReason = eReason;
            MessageSendToClient(listDestPlayers, SMSG_CURRENCY_MODIFIED, &tMessage, sizeof(tMessage));

            if (eCurrencyType >= CURRENCY_CRAFTING_MAT_START)
            {
                int32 nCurrenciesDiscovered = AttributesGetInt(FastAttribKey(ATTRIBUTE_CURRENCIES_DISCOVERED));
                if (!(nCurrenciesDiscovered & (1 << eCurrencyType)))
                {


                    nCurrenciesDiscovered |= (1 << eCurrencyType);
                    AttributesSetInt(FastAttribKey(ATTRIBUTE_CURRENCIES_DISCOVERED), nCurrenciesDiscovered);
                }
            }
        }
    }
}


int64 ActorCommonData::GetCurrencyAmount(const CurrencyType eCurrencyType) const
{
    return ItemPlayerGetCurrencyCount(*this, eCurrencyType);
}





void ActorCommonData::SetCurrencyAmount(int64 nAmount, const CurrencyType eCurrencyType, const GoldModifiedReason eReason)
{
    int64 nCount = GetCurrencyAmount(eCurrencyType);
    if (nCount == nAmount)
    {
        return;
    }
    ItemPlayerSetCurrency(*this, eCurrencyType, nAmount);
    SendCurrencyModifiedMessage(TRUE, eCurrencyType, eReason);
}








BOOL ActorCommonData::CanAffordCurrencyCost(int64 nCost, CurrencyType eCurrencyType) const
{
    return GetCurrencyAmount(eCurrencyType) >= nCost;
}

















































































































































// NON_MATCHING: vector<Player *> append loop caches capacity and strength-reduces growth differently; 3196 vs 3204 bytes, stack frame 0x250 vs 0x240.
void ACDHandleNecromancerCorpseSpawn(
    const ActorCommonData & tACDVictim)
{





    if (!tACDVictim.IsMonster() && !tACDVictim.IsPlayerTeam())
    {
        return;
    }

    if (tACDVictim.IsMonster())
    {
        ScopedMonsterDefinitionLockR pdefMonster(tACDVictim.GetMonsterSNO());
        if (pdefMonster->bDoesNotDropNecroCorpse)
        {
            return;
        }
    }

    PlayerList listPlayersToCheckCorpseSpawns(PlayerListGetElementPool());
    int32 nNumPlayers = ExperienceGetPlayersEligibleForKill(tACDVictim.id, listPlayersToCheckCorpseSpawns);

    if (nNumPlayers == 0)
    {
        return;
    }

    ScopedGlobalDefinitionLockR pdefGlobal;

    int32 nMaxCorpses = pdefGlobal->ptServerData->nCorpsesMax;





    blz::vector<Player *, blz::allocator<Player *> > arPlayersHandled;


    POS pos = listPlayersToCheckCorpseSpawns.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listPlayersToCheckCorpseSpawns.GetNext(pos);
        if (PlayerGetPlayerClass(ptPlayer) != PLAYER_CLASS_NECROMANCER)
        {
            continue;
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDPlayer)
        {
            continue;
        }

        if (ACDIsOnTheSameTeam(*ptACDPlayer, tACDVictim))
        {
            continue;
        }


        if (blzext::vector_find(arPlayersHandled, ptPlayer) != -1)
        {
            continue;
        }


        PlayerList listLocalPlayers(PlayerListGetElementPool());
        PlayerListGetAllPlayersLocalToPlayer(listLocalPlayers, ptPlayer);

        POS posPlayer = listLocalPlayers.GetHeadPosition();
        while (posPlayer)
        {
            Player * ptLocalPlayer = listLocalPlayers.GetNext(posPlayer);
            arPlayersHandled.push_back(ptLocalPlayer);
        }


        NavCellHandle hNavCellStart, hNavCellSearch;
        Vector3D wpSearch;
        float flMaxSearchRadius = 5.0f;

        WorldPlace placeStart = tACDVictim.GetPlace();
        WorldPlace placeSpawn = tACDVictim.GetPlace();
        uint16 uOneRequiredFlags = 1 << CELL_STATIC_ALLOW_WALK;

        BOOL bFoundPlace = FALSE;


        if (NavCellHandleFromWorldPlace(placeStart, hNavCellStart))
        {
            const NavZoneInstance * ptNavZoneInstance = NavCellHandleToZoneInstance(hNavCellStart);
            NavCellModifyForRoundedCorners(ptNavZoneInstance, hNavCellStart, placeStart.wp.As2D(), uOneRequiredFlags);

            uint32 uObjectNoOverlapFlags = NavMeshGetDynamicFlagsForPassability(PASSABILITY_FLIPPIES);
            uint16 uObjectNoPassthroughFlags = (1 << CELL_STATIC_SUBSCENE) | (1 << CELL_STATIC_NO_SPAWN) | (1 << CELL_STATIC_ALLOW_GHOST);
            if (WorldCollSearchForLocation(
                placeStart,
                uOneRequiredFlags,
                uObjectNoOverlapFlags,
                1.25f,
                0.0f,
                flMaxSearchRadius,
                ACDID_INVALID,
                wpSearch,
                hNavCellSearch,
                TEAM_NEUTRAL,
                uObjectNoPassthroughFlags, ACDID_INVALID))
            {
                placeSpawn.idSWorld = placeStart.idSWorld;
                placeSpawn.wp = wpSearch;
                bFoundPlace = TRUE;
            }
        }



        NavMeshHandle tNavMeshHandle;
        if (ScenesCommonNavMeshNodeFromPlace(placeStart, &tNavMeshHandle, FALSE, FALSE))
        {
            uint32 dwSquareStaticFlags = NavMeshHandleGetStaticFlags(tNavMeshHandle);
            if (dwSquareStaticFlags & (1 << CELL_STATIC_ALLOW_WALK))
            {
                NavMeshGetCenterPlace(tNavMeshHandle, placeSpawn);
                bFoundPlace = TRUE;
            }
        }

        if (bFoundPlace)
        {
            SNO snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_CORPSE_FOR_NECROMANCER);
            ActorAddStruct tActorAdd;
            tActorAdd.dwActorAddFlags = (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_ADD_IF_LOCATION_SEARCH_FAILS);
            tActorAdd.tPlace = tACDVictim.GetPlace();
            tActorAdd.qOrientation.FromYaw(RandAngle(SGameGlobalsGet().rng));
            tActorAdd.eTeam = ptACDPlayer->GetTeam();
            tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
            tActorAdd.tSNOName.snoHandle = snoActor;
            tActorAdd.snoActor = snoActor;

            ActorID idCorpse = GBAdd(tActorAdd);

            ACDID idACDCorpse = ACDFindByActor(idCorpse);
            ActorCommonData * ptACDCorpse = ACDTryToGet(idACDCorpse);

            if (ptACDCorpse)
            {
                SNO snoEffect = GlobalSNOGet(GLOBALSNO_EFFECTGROUP_NECRO_CORPSE_SPAWN);
                ACDPlayEffectForPlayer(ptACDPlayer->id, idACDCorpse, EFFECT_NECRO_CORPSE_SPAWN, snoEffect);

                SNO snoMonster = ActorGetMonsterSNO(tACDVictim.snoActor);
                ptACDCorpse->AttributesSetInt(FastAttribKey(ATTRIBUTE_NECROMANCER_CORPSE_SOURCE_MONSTER_SNO), snoMonster);
                ptACDCorpse->SetAssignedHeroID(ptPlayer);
                ptACDCorpse->ModifiedMark(ACDMODIFIED_ATTRIBUTES);




                POS posPlayerCorpseOwner = listLocalPlayers.GetHeadPosition();
                uint32 nPlayerIndex = 0;
                while (posPlayerCorpseOwner)
                {
                    Player * ptLocalPlayer = listLocalPlayers.GetNext(posPlayerCorpseOwner);

                    if (PlayerGetPlayerClass(ptLocalPlayer) == PLAYER_CLASS_NECROMANCER)
                    {
                        ptLocalPlayer->listNecroCorpses.push_back(idACDCorpse);
                        ptACDCorpse->SetNecroCorpseOwnerPlayerID(ptLocalPlayer->tAccount.tHandle.m_id, nPlayerIndex++);
                    }
                }
                ptACDCorpse->AttributesSetInt(FastAttribKey(ATTRIBUTE_NECROMANCER_CORPSE_CHARGES), nPlayerIndex);



                if (ptPlayer->listNecroCorpses.size() > nMaxCorpses)
                {

                    ACDID idCorpseToDelete = ptPlayer->listNecroCorpses[0];
                    blzext::vector_erase_at(ptPlayer->listNecroCorpses, 0);
                    ActorDelete((ActorID)ACDGetOwner(idCorpseToDelete));
                }
            }
        }
    }

}



void sAddNearbyCorpsesToList(
    const float flRadius,
    const Vector3D & wpPos,
    const Player * ptPlayer,
    const ActorCommonData * ptACDPlayer,
    blz::vector<ACDID> & listCorpses)
{
    float flSearchRadiusSquared = flRadius * flRadius;
    for (uint32 i = 0; i < ptPlayer->listNecroCorpses.size(); i++)
    {
        ACDID idCorpse = ptPlayer->listNecroCorpses[i];







        ActorCommonData * ptACDCorpse = ACDTryToGet(idCorpse);
        if (ptACDCorpse && ACDInSameWorld(*ptACDCorpse, *ptACDPlayer))
        {
            float flDistance = Vector3D::DistanceSquared(ptACDCorpse->GetPlace().wp, wpPos);

            if (flDistance < flSearchRadiusSquared)
            {
                listCorpses.push_back(ptACDCorpse->id);
            }
        }
    }
}



blz::vector<ACDID> ACDGetNecroCorpsesForPlayerAroundPosition(
    const Player * ptPlayer,
    Vector3D wpPos,
    SNO snoPower)
{
    blz::vector<ACDID> listCorpses;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return listCorpses;
    }


    if (snoPower == SNO_NONE)
    {
        int32 nTopPriority = -1;
        for (int32 nSlot = 0; nSlot < NUM_SKILL_SLOTS; nSlot++)
        {
            SNO snoSkill = ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].snoSkill;
            if (snoSkill != SNO_NONE)
            {
                int32 nSkillPriority = (int32)PowerGetFormulaValue(ptACDPlayer, snoSkill, TAG_POWER_NECRO_CORPSE_HIGHLIGHT_CORPSES, -1, ATTRIBGROUPID_INVALID, 0);
                if (nSkillPriority > nTopPriority)
                {
                    nTopPriority = nSkillPriority;
                    snoPower = snoSkill;
                }
            }
        }
    }

    if (snoPower != SNO_NONE)
    {
        float flSearchRadiusAroundCursor = PowerGetFormulaValue(ptACDPlayer, snoPower, TAG_POWER_NECRO_CORPSE_SEARCH_RADIUS_AROUND_CURSOR, -1, ATTRIBGROUPID_INVALID, 0);
        if (flSearchRadiusAroundCursor > EPSILON)
        {
            sAddNearbyCorpsesToList(flSearchRadiusAroundCursor, wpPos, ptPlayer, ptACDPlayer, listCorpses);
        }

        float flSearchRadiusAroundSelf = PowerGetFormulaValue(ptACDPlayer, snoPower, TAG_POWER_NECRO_CORPSE_SEARCH_RADIUS_AROUND_SELF, -1, ATTRIBGROUPID_INVALID, 0);
        if (flSearchRadiusAroundSelf > EPSILON)
        {
            wpPos = ptACDPlayer->GetPlace().wp;
            sAddNearbyCorpsesToList(flSearchRadiusAroundSelf, wpPos, ptPlayer, ptACDPlayer, listCorpses);
        }
    }

    return listCorpses;
}



BOOL ActorCommonData::IsCorpse() const
{
    BOOL bCorpse = FALSE;
    {
        ScopedActorDefinitionLockR ptDefActor(snoActor);
        bCorpse = TagMapGetInt(ptDefActor->hTagMap, TAG_ACTOR_IS_CORPSE) != 0;
    }
    return bCorpse;
}
// Not `inline`: the DWARF has a linker-removed out-of-line copy (low_pc 0) of the same size as ours.
BOOL ActorCommonData::IsModifiedAny() const
{


    return dwModifiedBits != 0;
}


BOOL ActorCommonData::IsModifiedSpecific(const ACDModifiedType eModifiedType) const
{
    return (dwModifiedBits >> eModifiedType) & 1;
}




void ActorCommonData::ModifiedClear()
{

    for (int32 i = 0; i < ACDMODIFIED_NUM_TYPES; i++)
    {
        ACDModifiedType eModifiedType = (ACDModifiedType)i;

        if (IsModifiedSpecific(eModifiedType))
        {

            switch (eModifiedType)
            {
            case ACDMODIFIED_ATTRIBUTES:


                FastAttribClearModified(idFastAttrib);
                break;




            case ACDMODIFIED_ONE_SHOTS:

                listACDMessages.RemoveAll();
                break;





            case ACDMODIFIED_MOVEMENT:
                {
                    ActorMovement * ptMovement = GetActorMovement();

                    if (ptMovement)
                    {
                        ptMovement->PostMessage();
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    dwModifiedBits = 0;
}


void ActorCommonData::ModifiedClear(const ACDModifiedType eModifiedType)
{
    dwModifiedBits &= ~(1 << eModifiedType);
}




void ActorCommonData::ModifiedMark(const ACDModifiedType eModifiedType)
{








    if (!IsModifiedAny())
    {
        GameCommonData & tGCD = GameGetActiveGameCommonData();




        tGCD.ptACDGlobals->listDirtyACDs.AddTail_Tracked(id);
    }



    if (eModifiedType == ACDMODIFIED_ATTRIBUTES)
    {

        AttributesUpdateCached();
    }


    dwModifiedBits |= (1 << eModifiedType);

    ServerQueueClientUpdate();
}

void ActorCommonData::SendSegmentIfModified(
    const PlayerList & listDestPlayers,
    const ACDModifiedType eModifiedType,
    bool bForce)
{







    if (!IsModifiedSpecific(eModifiedType))
    {
        return;
    }


    switch (eModifiedType)
    {
    case ACDMODIFIED_ATTRIBUTES:
    {
        AttributesSetValuesMessage tMessage = {};
        tMessage.ann = ANNFromACD(id);
        tMessage.nAttributeCount = 0;

        FastAttribModifiedIterator tIt(idFastAttrib);
        while (FastAttribModifiedIterate(tIt))
        {
            FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
            uint8 uSyncFlags = KeyGetSyncFlags(tKey);
            if (uAttributeSyncMask & uSyncFlags)
            {
                AttributeCompressorFillNetAttributeKeyValue(
                    tMessage.atKeyVals[tMessage.nAttributeCount],
                    tKey,
                    tIt.tEntryBuffer.tValue);
                tMessage.nAttributeCount++;


                if (tMessage.nAttributeCount == 15)
                {
                    MessageSendToClient(listDestPlayers, SMSG_ATTRIBUTES_SET_VALUES, &tMessage, sizeof(tMessage));
                    tMessage.nAttributeCount = 0;
                }

                if ((uSyncFlags & 0x20) &&
                    (uAttributeSyncMask & 0x81))
                {
                    Player * ptPlayer = PlayerGetByACD(id);
                    if (ptPlayer)
                    {
                        SPlayerRecordSentSpeedValue(*ptPlayer, *this, tIt.tEntryBuffer.tValue.flValue);
                    }
                }
            }



            ActorID idActor = (ActorID)idOwner;
            ActorTriggerAttributeUpdate(idActor);
        }


        if (tMessage.nAttributeCount == 1)
        {

            AttributeSetValueMessage tSmallerMessage = {};
            tSmallerMessage.ann = tMessage.ann;
            tSmallerMessage.tKeyVal = tMessage.atKeyVals[0];
            MessageSendToClient(listDestPlayers, SMSG_ATTRIBUTE_SET_VALUE, &tSmallerMessage, sizeof(tSmallerMessage));
        }
        else if (tMessage.nAttributeCount > 1)
        {
            MessageSendToClient(listDestPlayers, SMSG_ATTRIBUTES_SET_VALUES, &tMessage, sizeof(tMessage));
        }
        break;
    }

    case ACDMODIFIED_ONE_SHOTS:
    {

        const Player * ptPlayer = listDestPlayers.GetHead();


        POS pos = listACDMessages.GetHeadPosition();
        while (pos)
        {
            const KnownMessage & tKnownMessage = listACDMessages.GetNext(pos);


            if (tKnownMessage.dwDestinationsPlayerIndexMask & (1 << ptPlayer->nPlayerIndex))
            {
                KnownMessageSend(listDestPlayers, tKnownMessage);
            }
        }
        break;
    }

    case ACDMODIFIED_VISUAL_INVENTORY:
    {
        SendVisualInventory(listDestPlayers);
















        if (ptVisualInventory)
        {
            SigmaMemoryCopy(&ptVisualInventory->tEquipmentLast, &ptVisualInventory->tEquipmentCurrent, sizeof(ptVisualInventory->tEquipmentCurrent));
        }
        break;
    }

    case ACDMODIFIED_AFFIX:
    {


        SendAffix(listDestPlayers, AFFIX_MESSAGE_FLAG_IDENTIFIED);
        SendAffix(listDestPlayers, AFFIX_MESSAGE_FLAG_UNIDENTIFIED);
        break;
    }
    case ACDMODIFIED_GB_HANDLE:
    {

        ACDChangeGBHandleMessage tMessage;
        tMessage.ann = ANNFromACD(id);
        tMessage.hGB = hGB;

        MessageSendToClient(listDestPlayers, SMSG_ACD_CHANGE_GB_HANDLE, &tMessage, sizeof(tMessage));
        break;
    }
    case ACDMODIFIED_RARE_MONSTER_NAME:
    {

        SendRareMonsterName(listDestPlayers);
        break;
    }
    case ACDMODIFIED_RARE_ITEM_NAME:
    {

        SendRareItemName(listDestPlayers);
        break;
    }
    case ACDMODIFIED_INVENTORY_CREATED:
    {

        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(id);

        MessageSendToClient(listDestPlayers, SMSG_INVENTORY_CREATE, &tMessage, sizeof(tMessage));
        break;
    }
    case ACDMODIFIED_PORTAL_SPECIFIER:
    {

        SendPortalSpecifier(listDestPlayers);
        break;
    }
    case ACDMODIFIED_WORLD_POSITION:
    {



        Player * ptPlayer = listDestPlayers.GetHead();
        SWorldID idSWorldMe = GetSWorldID();
        SWorldID idSWorldPlayer = ACDGet(ptPlayer->idPlayerACD).GetSWorldID();
        if (idSWorldPlayer == SWORLDID_INVALID || idSWorldMe == SWORLDID_INVALID || idSWorldMe == idSWorldPlayer)
        {


            SendWorldPosition(listDestPlayers);
            SendShear(listDestPlayers);
        }
        break;
    }
    case ACDMODIFIED_INVENTORY_POSITION:
    {

        SendInventoryPosition(listDestPlayers);
        break;
    }
    case ACDMODIFIED_INVENTORY_ACTOR_SNO:
    {

        SendInventoryActorSNO(listDestPlayers);
        break;
    }
    case ACDMODIFIED_MOVEMENT:
    {

        SActorMovementSendMoveMessage(listDestPlayers, id);
        break;
    }
    case ACDMODIFIED_COLL_FLAGS:
    {

        SendCollision(listDestPlayers);
        break;
    }
    case ACDMODIFIED_WARPED:
    {


        Player & tDestPlayer = *listDestPlayers.GetHead();
        BOOL fSendWarpedMessage = tDestPlayer.idPlayerACD == id;


        if (!fSendWarpedMessage && tDestPlayer.bIsPrimaryPlayerServerOnly)
        {
            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                if (ptPlayer->idPlayerACD == id)
                {
                    if (PlayersAreLocalToEachOther(tDestPlayer, *ptPlayer))
                    {
                        fSendWarpedMessage = TRUE;
                    }
                    break;
                }
            }
        }

        if (fSendWarpedMessage)
        {

            SendPlayerWarped(listDestPlayers, tDestPlayer.eWarpReason, tDestPlayer.flWarpFadeInSecs);
            tDestPlayer.eWarpReason = WARP_UNKNOWN;
        }
        break;
    }
    case ACDMODIFIED_GROUP:
    {

        SendGroupInfo(listDestPlayers);
        break;
    }
    case ACDMODIFIED_COMPLEX_EFFECTS:
    {

        SComplexEffectSendAllSources(listDestPlayers, id);
        break;
    }
    }

}



void ActorCommonData::SendServerUpdateMessage(const PlayerList & listDestPlayers)
{

    if (IsModifiedAny())
    {
        for (int32 i = 0; i < ACDMODIFIED_NUM_TYPES; i++)
        {

            SendSegmentIfModified(listDestPlayers, i, false);
        }
    }
}



void ActorCommonData::SendAllAttributes(const PlayerList & listDestPlayers)
{

    AttributesSetValuesMessage tMessage = {};
    tMessage.ann = ANNFromACD(id); tMessage.nAttributeCount = 0;

    FastAttribIterator tIt(idFastAttrib, TRUE);

    while (FastAttribIterate(tIt))
    {
        FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
        if (uAttributeSyncMask & KeyGetSyncFlags(tKey))
        {

            AttributeCompressorFillNetAttributeKeyValue(
                tMessage.atKeyVals[tMessage.nAttributeCount], tKey,

                tIt.tEntryBuffer.tValue);
            tMessage.nAttributeCount++;


            if (tMessage.nAttributeCount == 15)
            {

                MessageSendToClient(listDestPlayers, SMSG_ATTRIBUTES_SET_VALUES, &tMessage, sizeof(tMessage));

                tMessage.nAttributeCount = 0;
            }
        }
    }


    if (tMessage.nAttributeCount > 0)
    {
        MessageSendToClient(listDestPlayers, SMSG_ATTRIBUTES_SET_VALUES, &tMessage, sizeof(tMessage));
    }
}

void ActorCommonData::SendVisualInventory(const PlayerList & listDestPlayers)
{

    InventoryType eInventoryType = ACDInventoryGetType(*this);
    if (eInventoryType == INVENTORY_NONE && !IsMirrorImage())
    {

        return;
    }


    VisualInventory * _ptVisualInventory = VisualInventoryGet(id);
    if (_ptVisualInventory == NULL) return;
    {
        VisualInventoryMessage tMessage;
        tMessage.ann = ANNFromACD(id);
        tMessage.tEquipment = _ptVisualInventory->tEquipmentCurrent;

        MessageSendToClient(listDestPlayers, SMSG_VISUAL_INVENTORY, &tMessage, sizeof(tMessage));
    }
}



























void ActorCommonData::SendAffix(const PlayerList & listDestPlayers, AffixMessageFlag eAffixMessageFlag)
{








    if (ACDIsPlayer(id))
    {
        return;
    }


    const GBIDPooledList & listAffixGBIDs = (eAffixMessageFlag == 1) ? listIdentifiedAffixGBIDs : listUnidentifiedAffixGBIDs;

    AffixMessage tMessage;
    tMessage.ann = ANNFromACD(id); tMessage.eAffixArray = eAffixMessageFlag;




















    POS pos = listAffixGBIDs.GetHeadPosition();
    tMessage.dwAffixArrayCount = 0;




    while (pos)
    {

        GBID gbid = listAffixGBIDs.GetNext(pos);




















        tMessage.aAffixGBIDs[tMessage.dwAffixArrayCount] = gbid;


        tMessage.dwAffixArrayCount++;
    }


    MessageSendToClient(listDestPlayers, SMSG_AFFIX, &tMessage, sizeof(tMessage));
}
void ActorCommonData::SendPortalSpecifier(const PlayerList & listDestPlayers)
{





    if (!ptResolvedDest->IsValid())
    {
        return;
    }



    PortalSpecifierMessage tMessage;
    tMessage.ann = ANNFromACD(id);
    tMessage.tResolvedDest = *ptResolvedDest;

    MessageSendToClient(listDestPlayers, SMSG_PORTAL_SPECIFIER, &tMessage, sizeof(tMessage));
}



void ActorCommonData::SendRareMonsterName(const PlayerList & listDestPlayers)
{

    RareMonsterNamesMessage tMessage;




    tMessage.ann = ANNFromACD(id);


    memcpy(tMessage.pgbidRareNames, pRareNameGBIDs, sizeof(tMessage.pgbidRareNames));



    memcpy(tMessage.pgbidMonsterAffixes, pMonsterAffixGBIDs, sizeof(tMessage.pgbidMonsterAffixes));


    MessageSendToClient(listDestPlayers, SMSG_RARE_MONSTER_NAMES, &tMessage, sizeof(tMessage));
}



void ActorCommonData::SendRareItemName(
    const PlayerList & listDestPlayers)
{
    RareItemNameMessage tMessage;
    tMessage.ann = ANNFromACD(id);
    tMessage.tRareItemName = tRareItemName;
    MessageSendToClient(listDestPlayers, SMSG_RARE_ITEM_NAME, &tMessage, sizeof(tMessage));
}



void ActorCommonData::SendWorldPosition(const PlayerList & listDestPlayers)
{






    ACDWorldPositionMessage tMessage;
    tMessage.ann = ANNFromACD(id);
    tMessage.tWorldLocationData.flScale = tLocation.tWorldLocation.flScale;
    tMessage.tWorldLocationData.idSWorld = tLocation.tWorldLocation.idSWorld;
    tMessage.tWorldLocationData.transform = tLocation.tWorldLocation.transform;

    if (AttributesGetInt(ATTRIBUTE_HAS_LOOK_OVERRIDE) && !ACDIsPlayer(id))
    {

        int32 nLookIndex = 0;
        EnterKnownLookOverrides tLookOverrides;
        for (int32 x = 0; x < 6; x++)
        {
            tLookOverrides.aLookOverrides[x] = 0;
        }

        POS pos = listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (!ptBuff || ptBuff->bDeleted || !FastAttribGetValueInt(ptBuff->idFastAttrib, ATTRIBUTE_HAS_LOOK_OVERRIDE))
                continue;



            tLookOverrides.aLookOverrides[nLookIndex] = ptBuff->nLookOverrideHash;
            nLookIndex++;
            if (nLookIndex >= 6)
            {
                break;
            }
        }



        tMessage.tLookOverrides.Set(tLookOverrides);
    }

    MessageSendToClient(listDestPlayers, SMSG_ACD_WORLD_POSITION, &tMessage, sizeof(tMessage));

}



void ActorCommonData::SendInventoryPosition(const PlayerList & listDestPlayers)
{


    if (tLocation.eActorLocation != AL_INVENTORY)
    {
        return;
    }










    ACDInventoryPositionMessage tMessage;
    tMessage.ann = ANNFromACD(id);
    tMessage.tInvLocationData.eSlot = tLocation.tInvLocation.eSlot;
    tMessage.tInvLocationData.pt = tLocation.tInvLocation.pt;
    tMessage.tInvLocationData.annContainer = ANNFromACD(tLocation.tInvLocation.idACDContainer);
    tMessage.tLocType = GetLocationType();


    MessageSendToClient(listDestPlayers, SMSG_ACD_INVENTORY_POSITION, &tMessage, sizeof(tMessage));
}


void ActorCommonData::SendInventoryActorSNO(const PlayerList & listDestPlayers)
{
    ACDInventoryUpdateActorSNO tMessage;



    tMessage.ann = ANNFromACD(id);
    tMessage.snoActor = snoActor;

    MessageSendToClient(listDestPlayers, SMSG_ACD_INVENTORY_UPDATE_ACTOR_SNO, &tMessage, sizeof(tMessage));
}



void ActorCommonData::SendShear(const PlayerList & listDestPlayers)
{

    if (dwFlags & (1 << ACD_USES_SHEAR_BIT))
    {
        ACDShearMessage tShearMessage;
        tShearMessage.ann = ANNFromACD(id);
        tShearMessage.flShear = flShear;

        MessageSendToClient(listDestPlayers, SMSG_SHEAR, &tShearMessage, sizeof(tShearMessage));
    }
}



void ActorCommonData::SendGroupInfo(const PlayerList & listDestPlayers)
{
    ACDGroupMessage tMessage;

    tMessage.ann = ANNFromACD(id);
    tMessage.uPrimaryGroup = uGroupPrimary;
    tMessage.uSecondaryGroup = uGroupSecondary;

    MessageSendToClient(listDestPlayers, SMSG_GROUP, &tMessage, sizeof(tMessage));
}




void ActorCommonData::SendFriendRecipientInfo(const PlayerList & listDestPlayers)
{
    PlayerGiftMessage tMessage;

    tMessage.ann = ANNFromACD(id);

    if (ptFriendGiftData)
    {
        tMessage.nId = ptFriendGiftData->idRecipient;
        Blizzard::String::Copy(tMessage.szPlayerName, ptFriendGiftData->sNameRecipient.str(), 32);
    }
    else
    {

        tMessage.nId = 0;
        tMessage.szPlayerName[0] = '\0';
        tMessage.nOwnerIndex = AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_PLAYER_RECIPIENT));
    }


    MessageSendToClient(listDestPlayers, SMSG_PLAYER_GIFT_REGISTER_RECIPIENT, &tMessage, sizeof(tMessage));
}

BOOL ACDAffectedByHealthGlobes(const ActorCommonData & tACD)
{
    if (!tACD.fIsAlive)
    {
        return FALSE;
    }
    if (tACD.IsObserver())
    {
        return FALSE;
    }
    if (ACDIsPlayer(tACD.id))
    {
        return TRUE;
    }
    if (tACD.IsPet() && tACD.IsTargetable())
    {
        return TRUE;
    }
    return FALSE;
}














SNO ActorCommonData::GetSkillKit() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_SKILLKIT));
}





void ActorCommonData::SetSkillKit(const SNO snoSkillKit, BOOL bResetIfSame)
{
    if (snoSkillKit == SNO_NONE)
        return;
    SNO snoCurrentSkillKit = AttributesGetInt(FastAttribKey(ATTRIBUTE_SKILLKIT));
    if (bResetIfSame || snoCurrentSkillKit != snoSkillKit)
    {
        AttributesSetInt(FastAttribKey(ATTRIBUTE_SKILLKIT), snoSkillKit);
        PowerResetSkills(*this, TRUE);
    }
}
















void ActorCommonData::ChangeDamageState(const int32 nValue)
{
    AttributesSetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_CURRENT), nValue);
    ACDPlayEffect(id, EFFECT_CHANGE_DAMAGE_STATE, nValue, ACDID_INVALID);
}







int32 ActorCommonData::IncDamageState()
{
    int nValue = AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_CURRENT));
    int nMax = AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_MAX));
    if (nValue < 15 && nValue < nMax)
    {
        nValue = Math::Max(nValue, 0) + 1;
        AttributesSetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_CURRENT), nValue);
        ACDPlayEffect(id, EFFECT_CHANGE_DAMAGE_STATE, nValue, ACDID_INVALID);
    }
    return nValue;
}









BOOL ActorCommonData::ConvertsDamageToDamageState() const
{
    if (GetGizmoType() == ACTORGIZMO_BREAKABLEDOOR)
    {
        return TRUE;
    }
    BOOL bConvertsDamage = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_CONVERTS_DAMAGE_TO_DAMAGE_STATES, snoActor);
    return bConvertsDamage;
}





int32 ActorCommonData::GetDamageStateMax() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_MAX));
}





void ActorCommonData::GetContactOffset(Vector3D & wpContactOffset, uint32 nContactFrame) const
{

    WorldLocation tWorldLoc = tLocation.tWorldLocation;
    SNO snoAnim = SNO_NONE;
    int32 nPermutation = -1;


    if (nContactFrame >= 4)
    {
        return;
    }



    AnimPlayerGetCurrentAnim(*this, snoAnim, nPermutation);

    if (snoAnim != SNO_NONE)
    {
        ScopedAnimationDefinitionLockR ptAnimDef(snoAnim);




        wpContactOffset = ptAnimDef->ptPermutations[nPermutation].wpContactOffset[nContactFrame];
    }
    else
    {

        ScopedActorDefinitionLockR ptActorDef(GetActorSNO());

        wpContactOffset = ptActorDef->wpLocationPowerSrc;
    }


    wpContactOffset = tWorldLoc.transform.q.Rotate(wpContactOffset);
    wpContactOffset *= tWorldLoc.flScale;

    wpContactOffset += tWorldLoc.transform.wp;
}

































































ACDID ACDFindBySNOName(const SNOName & tSNOName)
{
    ACDIterator tIt;

    while (ActorCommonData * ptACD = ActorCommonDataIterate(tIt))
    {

        if (tSNOName == ptACD->tSNOName)
        {
            return ptACD->id;
        }
    }

    return ACDID_INVALID;
}



void ACDLookAtACD(
    const ACDID idACDSrc,
    const ACDID idACDTarget)
{




    ACDLookAtMessage tMessage;
    tMessage.annSrc = ANNFromACD(idACDSrc);
    tMessage.annTarget = ANNFromACD(idACDTarget);

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, 0, -1);

    MessageSendToClient(listDestPlayers, SMSG_LOOK_AT_ACD, &tMessage, sizeof(tMessage));
}



// Not `inline`: the DWARF has a linker-removed out-of-line copy (low_pc 0) of the same size as ours.
void ActorCommonData::sSetAttrib64(Attrib eAttribHi, Attrib eAttribLo, int nParam, uint64 uValue)
{
    uint32 uLo = (uint32)(uValue & 0xffffffff);
    uint32 uHi = (uint32)(uValue >> 32);






    AttributesSetInt(MakeAttribKey(eAttribHi, nParam), uHi);
    AttributesSetInt(MakeAttribKey(eAttribLo, nParam), uLo);
}

// Not `inline`: the DWARF has a linker-removed out-of-line copy (low_pc 0) of the same size as ours.
uint64 ActorCommonData::sGetAttrib64(Attrib eAttribHi, Attrib eAttribLo, int nParam) const
{
    uint32 uHi = AttributesGetInt(MakeAttribKey(eAttribHi, nParam));
    uint32 uLo = AttributesGetInt(MakeAttribKey(eAttribLo, nParam));
    uint64 uValue = ((uint64)uHi << 32) | uLo;
    return uValue;
}







void ActorCommonData::SetAssignedHeroID(uint64 idHero)
{
    sSetAttrib64(ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_HIGH, ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_LOW, 0, idHero);
    idAssignedHero = idHero;
    idAssignedGameConnection = GAMECONNECTIONID_INVALID;
}











void ActorCommonData::SetAssignedHeroID(const Player * ptPlayer)
{

    if (ptPlayer)
    {

        sSetAttrib64(ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_HIGH, ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_LOW, 0, ptPlayer->uHeroId);
        idAssignedHero = ptPlayer->uHeroId;
        idAssignedGameConnection = ptPlayer->idGameConnection;
    }
    else
    {









        ClearAssignedHero();
    }
}
uint64 ActorCommonData::GetAssignedHeroID() const
{
    return idAssignedHero;
}




const uint64 ActorCommonData::GetAssignedHeroIDLow() const
{
    return sGetAttrib64(ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_HIGH, ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_LOW, 0);
}






BOOL ActorCommonData::HasAssignedHeroID() const
{
    return fHasAssignedHeroID;
}













BOOL ActorCommonData::DoesPlayerMatchAssignedHeroID(const Player * ptPlayer) const
{

    if (ptPlayer == NULL || !fHasAssignedHeroID)
    {
        return TRUE;
    }


    uint64 uAssignedHeroIDLow = GetAssignedHeroIDLow();
    if (ptPlayer->uHeroId == uAssignedHeroIDLow)
    {
        return TRUE;
    }


    if (idAssignedGameConnection != -1 &&
        idAssignedGameConnection == ptPlayer->idGameConnection)
    {
        return TRUE;
    }

    return FALSE;
}


void ActorCommonData::ClearAssignedHero()
{
    sSetAttrib64(ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_HIGH, ATTRIBUTE_ITEM_ASSIGNED_HERO_ID_LOW, 0, 0);
    idAssignedHero = OnlineService::INVALID_HERO_ID;
    idAssignedGameConnection = GAMECONNECTIONID_INVALID;
}












void ActorCommonData::SetAssignedGameConnectionID(GameConnectionID idGameConnection)
{
    idAssignedGameConnection = idGameConnection;
}




GameConnectionID ActorCommonData::GetAssignedGameConnectionID() const
{
    return idAssignedGameConnection;
}





void ActorCommonData::TagForExp(const ACDID idACDAttacker)
{
    ACDID idACDPlayer = ACDID_INVALID;
    ActorCommonData * ptACDAttacker = ACDTryToGet(idACDAttacker);
    if (ptACDAttacker != NULL)
    {
        idACDPlayer = ExperienceGetUltimateOwner(*ptACDAttacker);
    }

    if (ACDIsPlayer(idACDPlayer))
    {
        Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (ptPlayer != NULL)
        {
            uint32 uExperienceId = SPlayerGetExperienceId(*ptPlayer);
            uHeroTagBitfield |= 1 << uExperienceId;
        }
    }
}









BOOL ActorCommonData::GetExpTagPlayer(const Player & tPlayer) const
{
    if (tPlayer.uHeroId == OnlineService::INVALID_HERO_ID)
    {
        return FALSE;
    }
    uint32 uExperienceId = SPlayerGetExperienceId(tPlayer);
    return (uHeroTagBitfield >> uExperienceId) & 1;
}












void ActorCommonData::SetStoneOfRecallUnlocked(BOOL fUnlocked)
{

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        fUnlocked = FALSE;
    }





    SNO snoStoneOfRecallPower = GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL);
    AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoStoneOfRecallPower), fUnlocked);

    Player * ptPlayer = PlayerGetByACD(id);
    if (ptPlayer)
    {
        PlayerSetPlayerGameFlag(ptPlayer, "StoneOfRecallUnlocked", fUnlocked);
    }
}



BOOL ActorCommonData::AccountIsTradeRestricted() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_ACCOUNT_TRADING_RESTRICTED)) != 0;
}




uint32 ActorCommonData::GenerateSummonerID()
{
    uint32 idSummoner = AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMONER_ID));
    if (idSummoner == ACDID_INVALID)
    {
        idSummoner = id;
        AttributesSetInt(FastAttribKey(ATTRIBUTE_SUMMONER_ID), idSummoner);
    }
    return idSummoner;
}





BOOL ActorCommonData::IsNPC() const
{
    return IsMonster() && fIsNPC;
}













BOOL ActorCommonData::IsHelper() const
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_IS_HELPER)) != 0;
}




BOOL ActorCommonData::IsMerchant() const
{
    if (IsNPC() && NPCCanTrade(*this))
    {
        return TRUE;
    }
    return FALSE;
}






CurrencyType ActorCommonData::GetCurrencyType() const
{
    return ActorDefinitionGetTagMapValueInt(TAG_ACTOR_CURRENCY_TYPE, snoActor);
}





float ActorCommonData::GetHitPointsCurrent() const
{
    return flHitPointsCurrent;
}

























float ActorCommonData::GetHitPointsMaxTotal() const
{
    return AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
}





float ActorCommonData::GetHitpointPercent() const
{
    float flHealth = 0.0f;

    float flMaxHP = GetHitPointsMaxTotal();
    if (flMaxHP < -EPSILON || flMaxHP > EPSILON)
    {
        float flCurrentHP = GetHitPointsCurrent();
        flHealth = Math::Pin(flCurrentHP, 0.0f, flMaxHP) / flMaxHP;
    }
    return flHealth;
}







BOOL ActorCommonData::IsNormalMonster() const
{
    if (!IsMonster())
    {
        return FALSE;
    }



    MonsterRarity _eMonsterRarity = eMonsterRarity;
    switch (_eMonsterRarity) { case MR_CHAMPION: case MR_RARE: case MR_UNIQUE: case MR_BOSS: return FALSE; default: return TRUE; }
}










BOOL ActorCommonData::IsUndeadMonster() const
{
    if (!IsMonster())
    {
        return FALSE;
    }
    return MonsterGetMonsterType(tSNOName) == MT_UNDEAD;
}










BOOL ActorCommonData::IsLargeMonster() const
{
    if (!IsMonster())
    {
        return FALSE;
    }
    return MonsterIsLargeMonster(tSNOName) != FALSE;
}











BOOL ActorCommonData::DoesMonsterGrantEliteBuff() const
{
    BOOL bGrantBuff = FALSE;




    switch (eMonsterRarity)
    {










    case 1:
        {






            ACDPack * ptACDPack = ACDPackTryToGet(idACDPack);
            if (ptACDPack != NULL)
            {
                int32 nACDsLeft = ptACDPack->GetTotalACDCount(FALSE);
                bGrantBuff = nACDsLeft == 0;
            }
        }   break;
    case 2:
    case 4:
    case 7:
        bGrantBuff = AttributesGetInt(FastAttribKey(ATTRIBUTE_DROPS_NO_LOOT)) == 0;
        break;
    default: break;
    }

    return bGrantBuff;
}
BOOL sACDSetGroup(
    ActorCommonData & tACD,
    uint32 uNewGroup,
    POS & posGroupCur,
    uint32 & uACDGroupCur,
    const BOOL fModifyLiveActorCount,
    const ACDID idACDKiller)
{
    if (uACDGroupCur == uNewGroup)
    {
        return TRUE;
    }


    GameCommonData & tGCD = GameGetActiveGameCommonData();

    if (posGroupCur != NULL)
    {

        ACDGroup tACDGroup;
        if (tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(uACDGroupCur, tACDGroup))
        {
            if (fModifyLiveActorCount)
            {
                tACDGroup.nCount--;
                tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.SetAt_Tracked(uACDGroupCur, tACDGroup);

                if (AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT))
                {
                }



                if (tACDGroup.nCount == 0)
                {
                    SQuestsOnGroupKilled(uACDGroupCur);
                    ScriptEventOnGroupKilled(uACDGroupCur, idACDKiller);
                }
                else
                {
                    SQuestsOnGroupModified(uACDGroupCur, tACDGroup.nCount);
                }
            }


            tACDGroup.ptList->RemoveAt(posGroupCur);
        }

        posGroupCur = NULL;
        uACDGroupCur = (uint32)-1;
    }

    if (uNewGroup != (uint32)-1)
    {

        ACDGroup tACDGroup;
        if (!tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(uNewGroup, tACDGroup))
        {
            return FALSE;
        }


        if (fModifyLiveActorCount)
        {
            tACDGroup.nCount++;
            tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.SetAt_Tracked(uNewGroup, tACDGroup);
        }

        if (AppDrawFlagIsSet(APP_DRAW_DEBUG_QUESTS_BIT))
        {
        }







        posGroupCur = tACDGroup.ptList->AddTail_Tracked(tACD.id);
        uACDGroupCur = uNewGroup;
    }

    tACD.ModifiedMark(ACDMODIFIED_GROUP);

    return TRUE;
}





BOOL ACDGroupExists(uint32 uGroupHash)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();

    ACDGroup tACDGroup;
    if (tGCD.ptACDGlobals->tACDGroups.tMapACDGroups.Lookup(uGroupHash, tACDGroup))
    {
        return TRUE;
    }

    return FALSE;
}





BOOL ActorCommonData::SetPrimaryGroup(uint32 uGroupHash, const BOOL fModifyLiveActorCount, const ACDID idACDKiller)
{
    return sACDSetGroup(*this, uGroupHash, posGroupPrimary, uGroupPrimary, fModifyLiveActorCount, idACDKiller);
}













BOOL ActorCommonData::SetSecondaryGroup(uint32 uGroupHash, const BOOL fModifyLiveActorCount, const ACDID idACDKiller)
{
    return sACDSetGroup(*this, uGroupHash, posGroupSecondary, uGroupSecondary, fModifyLiveActorCount, idACDKiller);
}













void ActorCommonData::InheritGroupsFromACD(const ActorCommonData & tACDParent)
{
    SetPrimaryGroup(tACDParent.uGroupPrimary, TRUE, ACDID_INVALID);
    SetSecondaryGroup(tACDParent.uGroupSecondary, TRUE, ACDID_INVALID);
}





void ActorCommonData::RemoveFromGroups(const ACDID idACDKiller)
{
    SetPrimaryGroup(0xffffffff, TRUE, idACDKiller);
    SetSecondaryGroup(0xffffffff, TRUE, idACDKiller);
}





float ActorCommonData::GetWalkingSpeed(uint32 dwAnimTag) const
{
    static bool bErrorReported = false;


    WalkType eWalkType = AnimTagGetWalkType(dwAnimTag);







    float wdSpeed = (eWalkType == 0 || eWalkType == 4) ? AttributesGetFloat(FastAttribKey(ATTRIBUTE_WALKING_RATE_TOTAL)) :



                    eWalkType == 2 ? AttributesGetFloat(FastAttribKey(ATTRIBUTE_SPRINTING_RATE_TOTAL)) : eWalkType == 3 ? AttributesGetFloat(FastAttribKey(ATTRIBUTE_STRAFING_RATE_TOTAL)) : AttributesGetFloat(FastAttribKey(ATTRIBUTE_RUNNING_RATE_TOTAL));












    if (!(wdSpeed > 1e-6f))
    {




        SNO snoAnim = GetSpecificAnim(dwAnimTag);
        if (snoAnim == SNO_NONE)
        {
            wdSpeed = 0.3f;
            if (!bErrorReported)
            {
                bErrorReported = true;
            }
        }
        else
        {
            float fSpeedFactor = PowerGetAnimationSpeedForPower(id, AutoPowerDefinitionR(GlobalSNOGet(GLOBALSNO_POWERS_WALK)), -1);
            float flVelocity = AnimSNOGetWalkingSpeed(snoAnim, fSpeedFactor);
            wdSpeed = flVelocity;
        }
    }

    return wdSpeed;
}



BOOL ActorIsEnvActor(SNO snoActor)
{


    ScopedActorDefinitionLockR pdefActor(snoActor);
    if (pdefActor && pdefActor->eType == ACTORTYPE_ENVIRONMENT)
    {
        return TRUE;
    }
    return FALSE;
}






SNO ActorCommonData::GetMonsterSNO() const
{
    SNO snoMonster = SNO_NONE;
    if (tSNOName.snoGroup == ACTOR_SNO)
    {
        SNOName tMonsterSNOName;
        BOOL bHasMonsterSNO = MonsterGetSNONameFromActor(tSNOName.snoHandle, tMonsterSNOName);
        if (bHasMonsterSNO)
        {
            snoMonster = tMonsterSNOName.snoHandle;
        }
    }
    return snoMonster;
}






ActorType ActorGetType(const SNOName & tSNOName)
{
    if (tSNOName.snoGroup == ACTOR_SNO)
    {
        ScopedActorDefinitionLockR pdefActor(tSNOName.snoHandle);
        if (pdefActor)
        {
            return pdefActor->eType;
        }
    }
    return ACTORTYPE_INVALID;
}





void ActorCommonData::SetBossBar(BOOL bUsingBossBar)
{
    AttributesSetInt(FastAttribKey(ATTRIBUTE_USING_BOSSBAR), bUsingBossBar != FALSE);
}





BOOL ACDIsHireling(const ActorCommonData & tACD)
{
    HirelingClass eHireling = HirelingGetClass(tACD);
    return eHireling != HC_NONE;
}










BOOL ACDIsFollower(const ActorCommonData & tACD)
{
    PetType ePetType = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE));
    return ePetType == PETTYPE_FOLLOWER;
}










BOOL ACDIsHirelingProxy(const ActorCommonData & tACD)
{
    PetType ePetType = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE));
    return ePetType == PETTYPE_HIRELING_PROXY;
}










BOOL ACDIsFollowerProxy(const ActorCommonData & tACD)
{
    PetType ePetType = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE));
    return ePetType == PETTYPE_FOLLOWER_PROXY;
}









HirelingClass ACDGetHirelingClass(const ActorCommonData & tACD)
{
    return tACD.eHirelingClass;
}

























EliteClass ActorCommonData::GetEliteClass() const
{

    if (ACDIsPlayer(id) &&
        (GameTypeIsPvP() || IsDueling()))
    {
        return ELITECLASS_PLAYER;
    }

    if (IsMonster())
    {
        switch (eMonsterRarity)
        {
            case MR_CHAMPION: return ELITECLASS_CHAMPION;
            case MR_RARE: return ELITECLASS_RARE;
            case MR_RARE_MINION: return ELITECLASS_RARE_MINION;
        }



        SNO snoMonster = GetMonsterSNO();
        if (snoMonster != SNO_NONE)
        {
            ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
            if (pdefMonster->dwFlags & (1 << MONSTERDEF_IS_ELITE_BIT))
            {
                return ELITECLASS_TOUGH;
            }
        }
    }

    return ELITECLASS_INVALID;
}





BOOL ActorCommonData::IsElite() const
{
    return GetEliteClass() != ELITECLASS_INVALID;
}








void ActorCommonData::GetEliteModifierData(const GameBalanceDefinition ** ppdefGB, const EliteModifierData ** ppEliteModifierData) const
{

    *ppdefGB = NULL;
    *ppEliteModifierData = NULL;


    CRefString sGBIDStr("Normal");

    EliteClass eClass = GetEliteClass();
    switch (eClass)
    {
    case ELITECLASS_INVALID:
        if (eMonsterRarity != MR_NORMAL)
            return;
        sGBIDStr += " Normal";
        break;









    case ELITECLASS_CHAMPION:
        sGBIDStr += " Champion";
        break;
    case ELITECLASS_RARE:
        sGBIDStr += " Rare";
        break;
    case ELITECLASS_RARE_MINION:
        sGBIDStr += " Minion";
        break;
    case ELITECLASS_TOUGH:
        sGBIDStr += " Tough";
        break;
    case ELITECLASS_PLAYER:
        sGBIDStr = "Player";
        break;
    }



    GBHandle _hGB;
    _hGB.eType = GB_ELITE_MODIFIERS;
    _hGB.gbid = StringToGBID(sGBIDStr);

    *ppEliteModifierData = (const EliteModifierData *)GBLockR(_hGB, *ppdefGB, TRUE);
}





void ActorCommonData::AddClientEffect(
    SNO snoEffect,
    ParentType eTypeOverride)
{
    if (!SNOFileExists(EFFECTGROUP_SNO, snoEffect))
    {
        return;
    }

    EffectGroupID idEffect;
    if (tClientEffects.Lookup(snoEffect, idEffect) && EffectGroupTryToGet(idEffect))
    {
        return;
    }

    idEffect = EffectGroupCreateTracked(snoEffect, (RActorID)idOwner, RACTORID_INVALID, eTypeOverride, 0);
    tClientEffects.SetAt_Tracked(snoEffect, idEffect);
}

void ActorCommonData::RemoveClientEffect(SNO snoEffect)
{
    if (!SNOFileExists(EFFECTGROUP_SNO, snoEffect) || tClientEffects.IsEmpty())
    {
        return;
    }




    EffectGroupID idEffect;
    if (tClientEffects.Lookup(snoEffect, idEffect))
    {
        tClientEffects.RemoveAt(snoEffect);
        EffectGroupOutro(idEffect);
    }
}



void ActorCommonData::RemoveAllClientEffects()
{
    POS pos = tClientEffects.GetStartAssoc();
    while (pos)
    {
        EffectGroupID idEffect;
        SNO snoEffect;


        tClientEffects.GetNextAssoc(pos, snoEffect, idEffect);
        EffectGroupOutro(idEffect);
    }
}



uint64 ActorCommonData::GetStackCount() const
{
    return sGetAttrib64(ATTRIBUTE_ITEM_STACK_COUNT_HI, ATTRIBUTE_ITEM_STACK_COUNT_LO, -1);
}












uint64 ActorCommonData::GetExperienceNext() const
{
    return sGetAttrib64(ATTRIBUTE_EXPERIENCE_NEXT_HI, ATTRIBUTE_EXPERIENCE_NEXT_LO, -1);
}




void ActorCommonData::SetExperienceNext(uint64 uValue)
{
    sSetAttrib64(ATTRIBUTE_EXPERIENCE_NEXT_HI, ATTRIBUTE_EXPERIENCE_NEXT_LO, -1, uValue);
}




uint64 ActorCommonData::GetAltExperienceNext() const
{
    return sGetAttrib64(ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI, ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, -1);
}




void ActorCommonData::SetAltExperienceNext(uint64 uValue)
{
    sSetAttrib64(ATTRIBUTE_ALT_EXPERIENCE_NEXT_HI, ATTRIBUTE_ALT_EXPERIENCE_NEXT_LO, -1, uValue);
}




uint64 ActorCommonData::GetPvPExperienceNext() const
{
    return sGetAttrib64(ATTRIBUTE_PVP_EXPERIENCE_NEXT_HI, ATTRIBUTE_PVP_EXPERIENCE_NEXT_LO, -1);
}




void ActorCommonData::SetPvPExperienceNext(uint64 uValue)
{
    sSetAttrib64(ATTRIBUTE_PVP_EXPERIENCE_NEXT_HI, ATTRIBUTE_PVP_EXPERIENCE_NEXT_LO, -1, uValue);
}




uint64 ActorCommonData::PvPGetPvPExperienceGainedThisGame() const
{
    return sGetAttrib64(ATTRIBUTE_PVP_PVP_EXPERIENCE_GAINED_THIS_GAME_HI, ATTRIBUTE_PVP_PVP_EXPERIENCE_GAINED_THIS_GAME_LO, -1);
}




void ActorCommonData::PvPSetPvPExperienceGainedThisGame(uint64 uValue)
{
    sSetAttrib64(ATTRIBUTE_PVP_PVP_EXPERIENCE_GAINED_THIS_GAME_HI, ATTRIBUTE_PVP_PVP_EXPERIENCE_GAINED_THIS_GAME_LO, -1, uValue);
}




uint64 ActorCommonData::PvPGetNormalExperienceGainedThisGame() const
{
    return sGetAttrib64(ATTRIBUTE_PVP_NORMAL_EXPERIENCE_GAINED_THIS_GAME_HI, ATTRIBUTE_PVP_NORMAL_EXPERIENCE_GAINED_THIS_GAME_LO, -1);
}




void ActorCommonData::PvPSetNormalExperienceGainedThisGame(uint64 uValue)
{
    sSetAttrib64(ATTRIBUTE_PVP_NORMAL_EXPERIENCE_GAINED_THIS_GAME_HI, ATTRIBUTE_PVP_NORMAL_EXPERIENCE_GAINED_THIS_GAME_LO, -1, uValue);
}




uint64 ActorCommonData::GetExperienceGranted() const
{
    return sGetAttrib64(ATTRIBUTE_EXPERIENCE_GRANTED_HI, ATTRIBUTE_EXPERIENCE_GRANTED_LO, -1);
}





void ActorCommonData::SetExperienceGranted(uint64 uValue)
{
    sSetAttrib64(ATTRIBUTE_EXPERIENCE_GRANTED_HI, ATTRIBUTE_EXPERIENCE_GRANTED_LO, -1, uValue);
}




uint64 ActorCommonData::GetRestExperience() const
{
    return sGetAttrib64(ATTRIBUTE_REST_EXPERIENCE_HI, ATTRIBUTE_REST_EXPERIENCE_LO, -1);
}




void ActorCommonData::SetRestExperience(uint64 uValue)
{
    sSetAttrib64(ATTRIBUTE_REST_EXPERIENCE_HI, ATTRIBUTE_REST_EXPERIENCE_LO, -1, uValue);
}




void ActorCommonData::SetItemTradePlayerID(uint64 nPlayerID, int32 nPlayerIndex)
{
    sSetAttrib64(ATTRIBUTE_ITEM_TRADE_PLAYER_ID_HIGH, ATTRIBUTE_ITEM_TRADE_PLAYER_ID_LOW, nPlayerIndex, nPlayerID);
}






uint64 ActorCommonData::GetItemTradePlayerID(int32 nPlayerIndex) const
{
    return sGetAttrib64(ATTRIBUTE_ITEM_TRADE_PLAYER_ID_HIGH, ATTRIBUTE_ITEM_TRADE_PLAYER_ID_LOW, nPlayerIndex);
}




void ActorCommonData::SetItemSourcePlayerID(uint64 nPlayerID)
{
    sSetAttrib64(ATTRIBUTE_ITEM_SOURCE_PLAYER_ID_HIGH, ATTRIBUTE_ITEM_SOURCE_PLAYER_ID_LOW, 0, nPlayerID);
}




uint64 ActorCommonData::GetItemSourcePlayerID() const
{
    return sGetAttrib64(ATTRIBUTE_ITEM_SOURCE_PLAYER_ID_HIGH, ATTRIBUTE_ITEM_SOURCE_PLAYER_ID_LOW, 0);
}




void ActorCommonData::SetNecroCorpseOwnerPlayerID(uint64 nPlayerID, int32 nPlayerIndex)
{
    sSetAttrib64(ATTRIBUTE_NECROMANCER_CORPSE_OWNER_PLAYER_ID_HIGH, ATTRIBUTE_NECROMANCER_CORPSE_OWNER_PLAYER_ID_LOW, nPlayerIndex, nPlayerID);
}




// Not `inline`: the DWARF has a linker-removed out-of-line copy (low_pc 0) of the same size as ours.
uint64 ActorCommonData::GetNecroCorpseOwnerPlayerID(int32 nPlayerIndex) const
{

    return sGetAttrib64(ATTRIBUTE_NECROMANCER_CORPSE_OWNER_PLAYER_ID_HIGH, ATTRIBUTE_NECROMANCER_CORPSE_OWNER_PLAYER_ID_LOW, nPlayerIndex);
}
























uint64 ActorCommonData::AddToStackCount(uint64 uAddAmount, uint64 uMax)
{



    uint64 uOrigCount = Math::Max<uint64>(1, GetStackCount());
    if (uOrigCount >= uMax)
    {
        return 0;
    }


    uint64 uAdded = uAddAmount;
    uint64 uNewCount = uOrigCount + uAddAmount;
    if (uNewCount > uMax)
    {
        uNewCount = uMax;

        uAdded = uMax - uOrigCount;
    }

    sSetAttrib64(ATTRIBUTE_ITEM_STACK_COUNT_HI, ATTRIBUTE_ITEM_STACK_COUNT_LO, -1, uNewCount);


    if (uAdded)
    {

        ACDID idACDOwner = ACDInventoryGetUltimateContainer(*this);
        Player * ptPlayer = PlayerGetByACD(idACDOwner);
        if (ptPlayer)
        {
            ANNDataMessage tMessage;
            tMessage.ann = ANNFromACD(id);

            PlayerList listDestPlayers(PlayerListGetElementPool());
            listDestPlayers.AddTail_Tracked(ptPlayer);
            MessageSendToClient(listDestPlayers, SMSG_STACK_NOTIFY, &tMessage, sizeof(tMessage));
        }
    }


    return uAdded;
}



void ActorCommonData::SubtractFromStackCount(uint64 uSubtractAmount)
{

    if (uSubtractAmount == 0)
    {
        return;
    }


    uint64 uCount = GetStackCount();
    if (uCount == 0 || uCount == 0x8000000000000000ULL)
    {
        return;
    }


    if (uSubtractAmount >= uCount)
    {
        uCount = 0;
    }
    else
    {
        uCount -= uSubtractAmount;
    }

    sSetAttrib64(ATTRIBUTE_ITEM_STACK_COUNT_HI, ATTRIBUTE_ITEM_STACK_COUNT_LO, -1, uCount);



    ACDID idACDOwner = ACDInventoryGetUltimateContainer(*this);
    Player * ptPlayer = PlayerGetByACD(idACDOwner);
    if (ptPlayer)
    {
        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(id);

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_STACK_DECREASE_NOTIFY, &tMessage, sizeof(tMessage));
    }
}




void ActorCommonData::SetStackCount(uint64 uNewCount)
{

    if (GameActiveGameCommonDataIsServer())
    {
        sSetAttrib64(ATTRIBUTE_ITEM_STACK_COUNT_HI, ATTRIBUTE_ITEM_STACK_COUNT_LO, -1, uNewCount);
    }
    else
    {

        uint32 uLo = (uint32)(uNewCount & 0xffffffff);
        uint32 uHi = (uint32)(uNewCount >> 32);
        FastAttribSetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_ITEM_STACK_COUNT_HI, -1), uHi);
        FastAttribSetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_ITEM_STACK_COUNT_LO, -1), uLo);
    }
}



uint64 ActorCommonData::GetUnlockTime() const
{
    return sGetAttrib64(ATTRIBUTE_UNLOCK_TIME_HI, ATTRIBUTE_UNLOCK_TIME_LO, -1);
}




void ActorCommonData::SetUnlockTime(uint64 uNewTime)
{
    if (GameActiveGameCommonDataIsServer())
    {
        sSetAttrib64(ATTRIBUTE_UNLOCK_TIME_HI, ATTRIBUTE_UNLOCK_TIME_LO, -1, uNewTime);
    }
    else
    {
        uint32 uLo = (uint32)(uNewTime & 0xffffffff);
        uint32 uHi = (uint32)(uNewTime >> 32);
        FastAttribSetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_UNLOCK_TIME_HI, -1), uHi);
        FastAttribSetValueInt(idFastAttrib, MakeAttribKey(ATTRIBUTE_UNLOCK_TIME_LO, -1), uLo);
    }
}






















AttribGroupID ActorCommonData::AttributesRefreshTimerGroup(AttributeTimerType eTimerType, int32 nDuration)
{
    FastAttribKey tTimerKey = MakeAttribKey(ATTRIBUTE_ATTRIBUTE_TIMER, eTimerType);
    if (AttributesGetInt(tTimerKey) < nDuration)
    {
        AttributesSetInt(tTimerKey, nDuration);
    }
    AttribGroupID idTimerGroup = ActorTimerEffectsGetGroup((ActorID)idOwner, eTimerType);
    return idTimerGroup;
}











float ActorCommonData::GetDPSValue(
    BOOL bDualWielding) const
{
    if (!FastAttribGroupIsValid(idFastAttrib))
    {
        return 0.0f;
    }

    float flDPS = 0.0f;
    if (!bDualWielding)
    {
        int32 eAttackTypeOriginal = AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND));

        ACDID idACDActiveWeapon = ACDInventoryGetPrimaryWeapon(id);
        ACDID idACDOffhandWeapon = ACDInventoryGetSecondaryWeaponOrShield(id);

        if (idACDActiveWeapon != ACDID_INVALID)
        {
            FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 0);
        }
        else if (ACDCanDualWield(*this) && idACDOffhandWeapon != ACDID_INVALID)
        {
            ActorCommonData * ptACDOffhandWeapon = ACDTryToGet(idACDOffhandWeapon);
            if (ptACDOffhandWeapon && ItemHasLabel(ptACDOffhandWeapon->hGB.gbid, ITL_WEAPON))
            {
                FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 1);
            }
        }

        float flDamage, flAttackSpeed;
        PowerActorGetWeaponDamageAndAttackSpeed(*this, flDamage, flAttackSpeed, TRUE, -1, TRUE);
        flDPS = flDamage * flAttackSpeed;

        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), eAttackTypeOriginal);
    }
    else
    {

        int32 eAttackTypeOriginal = AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND));
        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 0);
        float flDamageMain, flAttackSpeedMain;
        PowerActorGetWeaponDamageAndAttackSpeed(*this, flDamageMain, flAttackSpeedMain, TRUE, -1, TRUE);
        float flDamageOffhand, flAttackSpeedOffhand;
        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 1);
        PowerActorGetWeaponDamageAndAttackSpeed(*this, flDamageOffhand, flAttackSpeedOffhand, TRUE, -1, TRUE);


        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), eAttackTypeOriginal);

        float flSecPerAttackMain = 1.0f;
        if (flAttackSpeedMain > EPSILON)
        {
            flSecPerAttackMain = 1.0f / flAttackSpeedMain;
        }

        float flSecPerAttackOffhand = 1.0f;
        if (flAttackSpeedOffhand > EPSILON)
        {
            flSecPerAttackOffhand = 1.0f / flAttackSpeedOffhand;
        }

        float flTotalTime = flSecPerAttackMain + flSecPerAttackOffhand;
        flDPS = flAttackSpeedMain * flDamageMain * (flSecPerAttackMain / flTotalTime) + flAttackSpeedOffhand * flDamageOffhand * (flSecPerAttackOffhand / flTotalTime);
    }

    float flUIPercentDamInc = 1.0f + AttributesGetFloat(FastAttribKey(ATTRIBUTE_PLAYER_UI_ONLY_PERCENT_DAMAGE_INCREASE));
    return flDPS * flUIPercentDamInc;
}





void ActorCommonData::GetWeaponDamageAndAttackSpeed(
    BOOL bDualWielding,
    float & flMainHandDamage,
    float & flMainHandSpeed,
    float & flOffHandDamage,
    float & flOffHandSpeed) const
{

    if (!FastAttribGroupIsValid(idFastAttrib))
    {
        return;
    }

    flMainHandDamage = 0.0f;
    flMainHandSpeed = 0.0f;
    flOffHandDamage = 0.0f;
    flOffHandSpeed = 0.0f;

    if (!bDualWielding)
    {
        int32 eAttackTypeOriginal = AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND));

        ACDID idACDActiveWeapon = ACDInventoryGetPrimaryWeapon(id);
        ACDID idACDOffhandWeapon = ACDInventoryGetSecondaryWeaponOrShield(id);

        if (idACDActiveWeapon != ACDID_INVALID)
        {
            FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 0);
            PowerActorGetWeaponDamageAndAttackSpeed(*this, flMainHandDamage, flMainHandSpeed, TRUE, -1, TRUE);
        }
        else if (ACDCanDualWield(*this) && idACDOffhandWeapon != ACDID_INVALID)
        {
            ActorCommonData * ptACDOffhandWeapon = ACDTryToGet(idACDOffhandWeapon);
            if (ptACDOffhandWeapon && ItemHasLabel(ptACDOffhandWeapon->hGB.gbid, ITL_WEAPON))
            {
                FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 1);
                PowerActorGetWeaponDamageAndAttackSpeed(*this, flOffHandDamage, flOffHandSpeed, TRUE, -1, TRUE);
            }
        }

        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), eAttackTypeOriginal);
    }
    else
    {

        int32 eAttackTypeOriginal = AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND));
        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 0);
        PowerActorGetWeaponDamageAndAttackSpeed(*this, flMainHandDamage, flMainHandSpeed, TRUE, -1, TRUE);

        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 1);
        PowerActorGetWeaponDamageAndAttackSpeed(*this, flOffHandDamage, flOffHandSpeed, TRUE, -1, TRUE);

        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), eAttackTypeOriginal);
    }
}

























































































































float ActorCommonData::GetDamageReductionPercent() const
{

    int32 nLevel = AttributesGetInt(FastAttribKey(ATTRIBUTE_EFFECTIVE_LEVEL));
    float flPercentArmorMitigation = PowerComputeArmorDamageMitigation(*this, nLevel);


    float flResistanceTotal = 0.0f;
    int32 nResistanceTypes = 0;
    float flResistancePercentTotal = 0.0f;
    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES - 1; nDamageType++)
    {


        float flResistance = AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, nDamageType));
        float flBonusReductionPercent = AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_TYPE, nDamageType));
        flResistanceTotal += flResistance;
        flResistancePercentTotal += flBonusReductionPercent;
        nResistanceTypes++;
    }

    float flResistanceAverage;
    float flResistancePercentAverage;


    flResistanceAverage = flResistanceTotal / nResistanceTypes;
    flResistancePercentAverage = flResistancePercentTotal / nResistanceTypes;

    float flPercentMitigationFromResistance = PowerComputeResistanceDamageMitigation(flResistanceAverage, nLevel);

    float flDodgeChance = PowerComputeDodgeChance(*this, nLevel, 0, TRUE);
    float flReductionMelee = AttributesGetFloat(FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_MELEE));
    float flReductionRanged = AttributesGetFloat(FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_RANGED));
    float flReductionElites = AttributesGetFloat(FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_REDUCTION_FROM_ELITES));
    float flAvgDmgReductionFromMeleeRangedElites = (flReductionMelee + flReductionRanged + flReductionElites) / 3.0f;

    float flReductionPercent = 1.0f - (1.0f - flPercentArmorMitigation) *
                                      (1.0f - flDodgeChance) *
                                      (1.0f - flPercentMitigationFromResistance) *
                                      (1.0f - flAvgDmgReductionFromMeleeRangedElites) *
                                      (1.0f - flResistancePercentAverage);
    return flReductionPercent;
}




float ActorCommonData::GetEHPValue() const
{
    float flHP = GetHitPointsMaxTotal();
    float flDamageReduction = GetDamageReductionPercent();
    float flEHP = GetEHPValue(flHP, flDamageReduction);
    return flEHP;
}




float ActorCommonData::GetEHPValue(float flHP, float flDmgReductionPercent)
{
    float flEffectiveHitPoints = 0.0f;
    float flDivisor = 1.0f - flDmgReductionPercent;
    if (flDivisor > 0.000001f)
    {
        flEffectiveHitPoints = flHP / flDivisor;
    }
    return flEffectiveHitPoints;
}





BOOL ActorCommonData::GetHasNFPDevice()
{
    return AttributesGetInt(FastAttribKey(ATTRIBUTE_PLAYER_HAS_NFP_DEVICE));
}


void ActorCommonData::SetHasNFPDevice(BOOL bHasNFPDevice)
{
    AttributesSetInt(FastAttribKey(ATTRIBUTE_PLAYER_HAS_NFP_DEVICE), bHasNFPDevice);
}






BOOL ACDIsPortal(const ActorCommonData & tACD)
{
    if (!tACD.IsGizmo())
    {
        return FALSE;
    }
    return GizmoIsPortal(tACD.GetGizmoType());
}





BOOL ACDIgnoresProjectiles(const ActorCommonData & tACD)
{

    if (tACD.IsItem()) return TRUE;

    BOOL bIgnoresProjectiles = FALSE;
    {

        ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
        if (TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_AI_IGNORES_PROJECTILES) ||




            tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PROJECTILE_PASS_THROUGH)) ||



            tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED)) - 1.0f >= -EPSILON)
        {
            bIgnoresProjectiles = TRUE;
        }
    }
    return bIgnoresProjectiles;
}






BOOL ACDBlocksProjectiles(const ActorCommonData & tACD)
{
    return tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_BLOCKS_PROJECTILES)) > 0;
}













BOOL ACDCanBeHighlighted(ActorCommonData * ptACD)
{


    GizmoType eGizmoType = ptACD->GetGizmoType();
    if (eGizmoType == ACTORGIZMO_SIGN || eGizmoType == ACTORGIZMO_DESTROYABLE_OBJECT || eGizmoType == ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT)
    {
        return FALSE;
    }




    if (ptACD->IsNecroCorpse())
    {
        return TRUE;
    }


    if (ptACD->fIsAlive)
    {
        return TRUE;
    }


    if (ptACD->IsItem())
    {
        return TRUE;
    }




    return eGizmoType != ACTORGIZMO_UNDEFINED;
}



BOOL ACDIsTrackedInOpenWorld(const ActorCommonData & tACD)
{
    if (tACD.IsMonster() && tACD.eMonsterRarity == MR_BOSS)
    {
        return TRUE;
    }
    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
    return TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_TRACKED_IN_OPEN_WORLD);
}








BOOL ACDIsInLootRun(const ActorCommonData & tACD)
{
    SWorldID idSWorld = tACD.GetSWorldID();
    if (idSWorld == SWORLDID_INVALID)
    {
        return FALSE;
    }
    SNO snoWorld = WorldCommonGetWorldSNO(idSWorld);
    if (snoWorld == SNO_NONE)
    {
        return FALSE;
    }
    BOOL bIsDungeonFinder = WorldIsDungeonFinderPlaceHolder(snoWorld);
    return bIsDungeonFinder;
}






BOOL ACDIsInTieredLootRun(const ActorCommonData & tACD)
{
    return FastAttribGetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL)) >= 0;
}




BOOL ACDIsInChallengeRift(const ActorCommonData & tACD)
{
    return FastAttribGetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_IN_CHALLENGE_RIFT));
}




int32 ACDGetTieredLootRunLevel(const ActorCommonData & tACD)
{
    return FastAttribGetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_IN_TIERED_LOOT_RUN_LEVEL));
}




BOOL ACDIsParticipatingInTieredLootRun(const ActorCommonData & tACD)
{
    return NephalemRift::IsTieredRiftActive() && FastAttribGetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_PARTICIPATING_IN_TIERED_LOOT_RUN)) && !GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead;
}




BOOL ACDIsParticipatingInSetDungeon(const ActorCommonData & tACD)
{
    return FastAttribGetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON)) != 0;
}




BOOL ACDIsLootRunBoss(const ActorCommonData & tACD)
{

    SNO snoEncounter = GlobalSNOGet(GLOBALSNO_ENCOUNTER_DUNGEON_FINDER_BOSS);
    ScopedEncounterDefinitionLockR ptEncounterDef(snoEncounter);

    int32 nCount = ptEncounterDef->serSpawnOptions.dwSize / sizeof(EncounterSpawnOption);

    for (int32 i = 0; i < nCount; i++)
    {
        if (ptEncounterDef->arSpawnOptions[i].snoSpawn == tACD.snoActor)
        {
            return TRUE;
        }
    }

    return FALSE;
}



int32 ACDGetBloodShardCap(const ActorCommonData & tACD)
{

    if (!ACDIsPlayer(tACD.id))
    {
        return -1;
    }

    GBID gbidBloodShard = GlobalGBIDGet(GLOBALGBID_CURRENCY_BLOODSHARDS);
    ScopedItemDefinitionLockR ptItemDef(gbidBloodShard);
    if (!ptItemDef)
    {
        return -1;
    }



    int32 nStackSize = ptItemDef->nMaxStackSize;
    int32 nHighestSoloRiftLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_HIGHEST_SOLO_RIFT_LEVEL));

    return nStackSize + nHighestSoloRiftLevel * 10;

}















BOOL ACDTrySetHighestUnlockedRiftLevel(ActorCommonData & tACD, const int32 nCompletedRiftLevel)
{
    if (!ACDIsPlayer(tACD.id))
        return FALSE;
    int32 nUnlockedLevel = nCompletedRiftLevel + 1;
    nUnlockedLevel = Math::Min(nUnlockedLevel, NephalemRift::GetMaxTieredRiftLevel());
    int32 nHighestUnlockedRiftLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL));
    if (nUnlockedLevel > nHighestUnlockedRiftLevel)
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_HIGHEST_UNLOCKED_RIFT_LEVEL), nUnlockedLevel);
        return TRUE;
    }
    return FALSE;
}

#line 7333
void ActorCommonData::GetContactOffsetForAnim(
    const SNO snoAnim,
    Vector3D & wpContactOffset,
    const uint32 nContactFrame,
    const uint32 nPermutation,
    BOOL bWantWorldPosition) const
{
    if (nContactFrame >= MAX_NUM_ANIM_CONTACT_FRAMES)
    {
        return;
    }


    WorldLocation tWorldLoc = tLocation.tWorldLocation;

    if (snoAnim != SNO_NONE)
    {
        ScopedAnimationDefinitionLockR ptAnimDef(snoAnim);

        if (nPermutation >= ptAnimDef->nPermutationCount)
        {
            return;
        }


        wpContactOffset = ptAnimDef->ptPermutations[nPermutation].wpContactOffset[nContactFrame];
    }
    else
    {
        ScopedActorDefinitionLockR ptActorDef(GetActorSNO());

        wpContactOffset = ptActorDef->wpLocationPowerSrc;
    }

    wpContactOffset = tWorldLoc.transform.q.Rotate(wpContactOffset);
    wpContactOffset *= tWorldLoc.flScale;

    if (bWantWorldPosition)
    {
        wpContactOffset += tWorldLoc.transform.wp;
    }
}

#line 4326
void LocationSetBounds(
    SNO snoActor,
    WorldLocation & tLocation,
    const ActorDefinition * pdefActor)
{
    BOOL fLocalLock = pdefActor == NULL;
    if (fLocalLock)
    {
        pdefActor = (const ActorDefinition *)gptGroupDefActor->LockR(snoActor, FALSE);

    }



    float flScaledCylHeight, flScaledCylRadius;

    if (pdefActor->eType == ACTORTYPE_ITEM)
    {

        const float ITEM_CYLINDER_HEIGHT = 2.0f;
        const float SMALL_ITEM_CYLINDER_RADIUS = 3.0f;
        const float DEFAULT_ITEM_CYLINDER_RADIUS = 5.0f;


        ActorTopology eTopology = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_TOPOLOGY);


        flScaledCylHeight = ITEM_CYLINDER_HEIGHT;
        flScaledCylRadius = eTopology == ACTORTOPOLOGY_SMALL ? SMALL_ITEM_CYLINDER_RADIUS : DEFAULT_ITEM_CYLINDER_RADIUS;





        Vector3D wpCenter = tLocation.transform.wp;
        wpCenter.z += ITEM_CYLINDER_HEIGHT * 0.5f;
        tLocation.wsBounds.Set(wpCenter, flScaledCylRadius);
    }
    else
    {
        flScaledCylHeight = pdefActor->wcylBounds.wdHeight * tLocation.flScale;
        flScaledCylRadius = pdefActor->wcylBounds.wdRadius * tLocation.flScale;

        float flScaledSphereRadius = pdefActor->wsBounds.wdRadius * tLocation.flScale;
        float flHalfHeight = flScaledCylHeight * 0.5f;
        float flCylBoundRadius = Math::Sqrt(flHalfHeight * flHalfHeight + flScaledCylRadius * flScaledCylRadius);
        if (flScaledSphereRadius < flCylBoundRadius)
        {

            Vector3D wpCenter = tLocation.transform.wp;
            wpCenter.z += flHalfHeight;
            tLocation.wsBounds.Set(wpCenter, flCylBoundRadius);
        }
        else
        {
            Vector3D wpSphereCenter = pdefActor->wsBounds.wpCenter;
            Vector3D wpCenter = wpSphereCenter * tLocation.flScale;
            wpCenter = tLocation.transform.q.Rotate(wpCenter);
            tLocation.wsBounds.Set(tLocation.transform.wp + wpCenter, flScaledSphereRadius);
        }
    }

    tLocation.wcylBounds.Set(tLocation.transform.wp, flScaledCylHeight, flScaledCylRadius);

    if (fLocalLock)
    {
        gptGroupDefActor->UnlockR((void *&)pdefActor);
    }
}

#line 8949
float ActorCommonData::GetHealingValue(
    BOOL bDualWielding,
    float * pflBaseHealing) const
{














    float flLifePerSecond = AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_TOTAL));
    float flLifePerKill = AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_ON_KILL_TOTAL));

    float flLifePerHit = AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_ON_HIT));
    float flLifeStealPercent = AttributesGetFloat(FastAttribKey(ATTRIBUTE_STEAL_HEALTH_PERCENT));
    float flLifePerFurySpent = 0.0f;
    float flLifePerSpiritSpent = 0.0f;
    float flLifePerWrathSpent = 0.0f;
    float flRegenPerSecFury = 0.0f;
    float flRegenPerSecSpirit = 0.0f;
    float flRegenPerSecWrath = 0.0f;

    if (GetPrimaryResourceType() == RESOURCE_TYPE_FURY ||
        GetSecondaryResourceType() == RESOURCE_TYPE_FURY)
    {
        flLifePerFurySpent = ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, RESOURCE_TYPE_FURY);
        flRegenPerSecFury = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_FURY);
    }

    if (GetPrimaryResourceType() == RESOURCE_TYPE_SPIRIT ||
        GetSecondaryResourceType() == RESOURCE_TYPE_SPIRIT)
    {
        flLifePerSpiritSpent = ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, RESOURCE_TYPE_SPIRIT);
        flRegenPerSecSpirit = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_SPIRIT);
    }

    if (GetPrimaryResourceType() == RESOURCE_TYPE_FAITH ||
        GetSecondaryResourceType() == RESOURCE_TYPE_FAITH)
    {
        flLifePerWrathSpent = ResourceAttributeGetFloat(ATTRIBUTE_SPENDING_RESOURCE_HEALS_PERCENT, RESOURCE_TYPE_FAITH);
        flRegenPerSecWrath = ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_REGEN_TOTAL, RESOURCE_TYPE_FAITH);
    }

    float flHealthGlobeHealingBonusPercent = AttributesGetFloat(FastAttribKey(ATTRIBUTE_INCREASED_HEALTH_FROM_GLOBES_PERCENT_TOTAL));
    float flHealthGlobeHealingBonusFlat = AttributesGetFloat(FastAttribKey(ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH)) * (1.0f + flHealthGlobeHealingBonusPercent);

    float flAttacksPerSecond = 1.0f;
    if (!ACDIsDualWielding(*this, TRUE))
    {
        flAttacksPerSecond = AttributesGetFloat(FastAttribKey(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL));
    }
    else
    {
        int32 nCurrentHand = FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND));
        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 0);
        float flAPSMain = AttributesGetFloat(FastAttribKey(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL));
        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), 1);
        float flAPSOffHand = AttributesGetFloat(FastAttribKey(ATTRIBUTE_ATTACKS_PER_SECOND_TOTAL));
        FastAttribSetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), nCurrentHand);
        flAttacksPerSecond = flAPSMain;
        if (flAPSOffHand > EPSILON)
        {
            flAttacksPerSecond = flAPSMain / flAPSOffHand;
        }
    }





    float flLifeStealScalar = 1.0f;
    int32 nLevel = AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
    ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
    if (nLevel > 0)
    {
        flLifeStealScalar = tExpTable.ptExperienceLevels[Math::Min(nLevel - 1, (int32)HERO_ALT_LEVEL_REPEAT_INDEX)].flLifeStealMultiplier;
    }

    float flDPS = GetDPSValue(bDualWielding);

    float flEHP = GetEHPValue();
    float flHealing =
        flLifePerSecond +
        flLifePerHit * flAttacksPerSecond +
        flLifePerKill * 0.16f +
        flLifeStealPercent * flDPS * flLifeStealScalar +
        flLifePerFurySpent * (flRegenPerSecFury + flAttacksPerSecond * 4.5f) +
        flLifePerSpiritSpent * (flRegenPerSecSpirit + flAttacksPerSecond * 9.0f) +
        flLifePerWrathSpent * (flRegenPerSecWrath + flAttacksPerSecond * 3.75f) +
        flHealthGlobeHealingBonusFlat * 0.08f +
        flHealthGlobeHealingBonusPercent * (flEHP * 0.1f) * 0.08f;

    if (pflBaseHealing)
    {
        *pflBaseHealing = flHealing;
    }


    float flEHPPerSecond;
    flEHPPerSecond = flHealing / GetHitPointsMaxTotal();
    return flEHP * flEHPPerSecond;

}

#line 5992
BOOL ACDRollDropHealth(
    const ActorCommonData & tACDVictim,
    const ACDID idACDKiller,
    const BOOL fSpender)
{





    if (!tACDVictim.IsWreckableObject() &&
        !ACDIsPlayer(tACDVictim.id) &&
        tACDVictim.IsMonster())
    {
        if (tACDVictim.tSNOName.snoGroup == ACTOR_SNO)
        {
            HealthDropInfo tHealthDropInfo;
            if (MonsterGetHealthDropInfo(tACDVictim.tSNOName, tACDVictim.eMonsterRarity, tHealthDropInfo))
            {
                ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);




                ScopedGlobalDefinitionLockR pdefGlobal;

                float flDropChance = pdefGlobal->ptServerData->flBaseGlobeDropChance;
                if (fSpender)
                {
                    flDropChance = blz::max(flDropChance, pdefGlobal->ptServerData->flSpenderGlobeDropChance);
                }
                if (!tACDVictim.IsNormalMonster())
                {
                    flDropChance = blz::max(flDropChance, pdefGlobal->ptServerData->flSpecialMonsterGlobeDropChance);
                }

                if (ptACDKiller)
                {
                    flDropChance += ptACDKiller->AttributesGetFloat(FastAttribKey(ATTRIBUTE_HEALTH_GLOBE_BONUS_CHANCE));
                    flDropChance *= 1.0f + ptACDKiller->AttributesGetFloat(FastAttribKey(ATTRIBUTE_HEALTH_GLOBE_BONUS_CHANCE_MULT_PERCENT));

                    int32 nNumPlayers = PlayerGetNumPlayersForDifficulty();
                    int32 nPlayerMult = nNumPlayers - 1;
                    if (nPlayerMult > 0)
                    {
                        ScopedGlobalDefinitionLockR pdefGlobal2;

                        const DifficultyTuningParams & tTuningParams = pdefGlobal2->ptServerData->tDifficultyParams[GameGetDifficultyParamsLevel(FALSE)];

                        flDropChance /= 1.0f + nPlayerMult * tTuningParams.flHealthGlobeReductionPercentPerPlayer;
                    }
                }

                if (flDropChance > EPSILON &&
                    RandFloat_Internal(SGameGlobalsGet().rng) < flDropChance)
                {
                    return TRUE;
                }
            }
        }
    }


    return FALSE;
}









































































































































































































