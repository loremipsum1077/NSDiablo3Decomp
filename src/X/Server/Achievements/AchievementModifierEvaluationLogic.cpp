#include "AchievementModifierEvaluationLogic.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Powers/Powers.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorInventory.h"
#include "../../Common/OnlineService/Achievements.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Game.h"
#include "../../Common/GameTemplate.h"
#include "../../Common/NephalemRiftCommon.h"
#include "../SPlayer.h"
#include "../System/SNephalemRift.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Common/Definitions/GlobalDefinition.h"
extern const int32 sgnTransmogSlotsCount;
namespace Console { namespace Online { BOOL PartyIsPublic(); } }
ACDID ACDInventoryGetActiveWeapon(const ActorCommonData & tACD);  // X/Common/ActorInventory.cpp:5398
#include "../AI/BrainComponents.h"  // Inference: declares BrainAPIGetActiveComponent/ActiveComponent used at line 1057 (no DIE records the include).
namespace Achievement {
extern XVarUint32 arTimestamps[10];
 } // Achievement; dependency declarations below have global scope.
#include "../../Common/World.h"
#include "../../Common/Act.h"
#include "../SGameStats.h"
#include "../../Common/Crafting/ItemCraftingSwig.h"
// DWARF: X/Common/Items.cpp:10973.
void ItemsFindUpgradeableJewels(const Player * ptPlayer, BOOL bExcludeUnidentified, ACDIDArray & aridItems);
// DWARF: X/Common/Player.cpp:3205.
BOOL PlayerIsInLevelArea(const ACDID idACDPlayer, const SNO snoLevelArea);
// DWARF: X/Common/Act.cpp:214.
Act ActGetWaypointActByLevelArea(const SNO snoLevelArea);
namespace Achievement {
 } // Achievement; DWARF global item dependencies.
// DWARF: X/Common/Items.cpp:3191.
void ItemGetLabels(XGrowableList<int, int> & listLabels, const GBID gbidItem);
// DWARF: X/Common/Items.cpp:3403.
BOOL ItemIsTwoHanded(const ActorCommonData & tACDItem);
// DWARF: X/Common/Items.cpp:3467.
BOOL ItemUsesQuiver(const ActorCommonData & tACDItem);
// DWARF: X/Common/Items.cpp:3522.
BOOL ItemIsSocketed(const ActorCommonData & tACDItem);
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"  // Inference: header statics run by the original static initializer (ldn_Ipv4Address.h:153-163, ldn_MacAddress.h:101-106).

#include "../../Common/HirelingSwig.h"
namespace Achievement {




EvaluationData::EvaluationData()
    : m_ptAchievementsInterface(SGameGlobalsGet().ptAchievementsInterface)

    , m_ptPlayer(NULL), m_ptACD(NULL)
    , m_snoPower(SNO_NONE)

    , m_ptPower(NULL), m_ptPowerDamage(NULL), m_ptBuff(NULL), m_ptEvaluatingPlayer(NULL), m_nCursedChestMonsterKillCount(0)
    , m_eCrafterType(-1)
    , m_ptEquipmentInfo(NULL)



{
}


EvaluationData::~EvaluationData()
{
    if (m_ptEquipmentInfo) delete m_ptEquipmentInfo; m_ptEquipmentInfo = NULL;
}


const Player * EvaluationData::GetPlayer() const
{
    return m_ptPlayer;
}


void EvaluationData::SetPlayer(const Player * ptPlayer)
{

    m_ptPlayer = ptPlayer;
}


const ActorCommonData * EvaluationData::GetACD() const
{
    return m_ptACD;
}


void EvaluationData::SetACD(const ActorCommonData * ptACD)
{

    m_ptACD = ptACD;
}


const Power * EvaluationData::GetPower() const
{
    return m_ptPower;
}


const SNO EvaluationData::GetPowerSNO() const
{
    return m_snoPower;
}


const PowerAPIDamage * EvaluationData::GetPowerDamage() const
{
    return m_ptPowerDamage;
}


const PowerAPIBuffInstance * EvaluationData::GetBuff() const
{
    return m_ptBuff;
}


void EvaluationData::SetPower(SNO snoPower)
{

    m_snoPower = snoPower;

    m_ptPower = NULL; m_ptPowerDamage = NULL; m_ptBuff = NULL;

}


void EvaluationData::SetPower(const Power * ptPower)
{

    m_ptPower = ptPower;
    m_snoPower = ptPower ? ptPower->snoPower : SNO_NONE;

    m_ptPowerDamage = NULL; m_ptBuff = NULL;
}


void EvaluationData::SetPower(const PowerStartData * ptPowerStartData)
{

    m_snoPower = ptPowerStartData ? ptPowerStartData->snoPower : SNO_NONE;

    m_ptPower = NULL; m_ptPowerDamage = NULL; m_ptBuff = NULL;

}


void EvaluationData::SetPower(const PowerAPIDamage * ptPowerDamage)
{

    m_ptPowerDamage = ptPowerDamage;
    m_ptPower = ptPowerDamage ? PowerTryToGet(ptPowerDamage->idPower) : NULL;
    m_snoPower = m_ptPower ? PowerAPI_GetDamageSourcePowerSNO(ptPowerDamage, NULL) : SNO_NONE;
    m_ptBuff = NULL;
}


void EvaluationData::SetBuff(const PowerAPIBuffInstance * ptBuff)
{

    m_ptPower = ptBuff ? PowerTryToGet(ptBuff->idPower) : NULL;
    m_snoPower = ptBuff ? ptBuff->snoPower : SNO_NONE;

    m_ptPowerDamage = NULL; m_ptBuff = ptBuff;
}


const Achievement::EquipmentInfo * EvaluationData::GetEquipmentInfo()
{
    if (m_ptEquipmentInfo)
    {
        return m_ptEquipmentInfo;
    }

    if (m_ptPlayer)
    {
        SetEquipmentInfo(m_ptPlayer->idPlayerACD);
    }


    return m_ptEquipmentInfo;
}


const CrafterType EvaluationData::GetCrafterType() const
{
    return m_eCrafterType;
}


void EvaluationData::SetCrafterType(CrafterType eCrafterType)
{

    m_eCrafterType = eCrafterType;
}



void EvaluationData::SetCursedChestKillCount(const int32 nKillCount)
{

    m_nCursedChestMonsterKillCount = nKillCount;
}


int32 EvaluationData::GetCursedChestKillCount() const
{
    return m_nCursedChestMonsterKillCount;
}







bool EvaluateModifiers(EvaluationData * ptData, uint64 modifierID, uint64 operand)
{
    bool bResult;
    if (ptData)
    {




        bResult = ptData->Evaluate(modifierID, operand);
    }
    return bResult;
}
void EvaluationData::EvaluateEvent(const Player * ptPlayer, Achievement::EventID eEventID, uint32 operand, uint32 qty)
{




    m_ptEvaluatingPlayer = ptPlayer;





    if (ptPlayer->bIsGuestPlayerServerOnly)
        return;





    if (GameIsWeeklyChallenge())
        return;




    const OnlineService::GameAccountHandle & gameAccount = ptPlayer->tAccount.tHandle;
    if (m_ptAchievementsInterface)
    {
        m_ptAchievementsInterface->EvaluateEvent(gameAccount,
            eEventID,
            operand,
            qty,
            SGameManager::GameGet().ptSGameGlobals->tAchievementTime,
            OnlineService::internal::function_bind_1_2<bool, EvaluationData *, uint64, uint64, &EvaluateModifiers>(this));
    }
}


void EvaluationData::EvaluateEvent(const Player * ptPlayer, Achievement::EventID eEventID, uint32 qty)
{



    m_ptEvaluatingPlayer = ptPlayer;





    if (ptPlayer->bIsGuestPlayerServerOnly)
        return;





    if (GameIsWeeklyChallenge())
        return;




    const OnlineService::GameAccountHandle & gameAccount = ptPlayer->tAccount.tHandle;
    if (m_ptAchievementsInterface)
    {
        m_ptAchievementsInterface->EvaluateEvent(gameAccount,
            eEventID,
            qty,
            SGameManager::GameGet().ptSGameGlobals->tAchievementTime,
            OnlineService::internal::function_bind_1_2<bool, EvaluationData *, uint64, uint64, &EvaluateModifiers>(this));
    }
}


bool EvaluationData::Evaluate(uint64 modifierID, uint64 operand)
{

    if (EvaluateGameStateModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluatePlayerModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluatePowerModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluateMonsterModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluateItemModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluatePetModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluateEquipmentModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluateCrafterTypeModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluateBuffModifiers(modifierID, operand))
    {
        return true;
    }

    if (EvaluateMiscellaneousModifiers(modifierID, operand))
    {
        return true;
    }

    return false;
}



bool EvaluationData::EvaluateGameStateModifiers(
    uint64 modifierId,
    uint64 operand) const
{
    switch (modifierId)
    {
        case MODIFIER_IN_PVP_GAME:

            return GameTypeIsPvP() != FALSE;



        case MODIFIER_IN_COOP_GAME:

            return PlayersGetCount() > 1;



        case MODIFIER_NUMBER_PLAYERS_IN_GAME_EQUALS:

            return PlayersGetCount() == operand;



        case MODIFIER_NUMBER_PLAYERS_IN_GAME_GREATER_THAN:

            return PlayersGetCount() > operand;



        case MODIFIER_DEPRECATED_GAME_DIFFICULTY_EQUALS:

            return 0 == operand;



        case MODIFIER_DEPRECATED_GAME_DIFFICULTY_GREATER_THAN_EQUALS:

            return 0 >= operand;





        case MODIFIER_GAME_HANDICAP_EQUALS:

            return GameGetHandicapLevel() == operand;



        case MODIFIER_GAME_HANDICAP_GREATER_THAN_EQUALS:

            return GameGetHandicapLevel() >= operand;



        case MODIFIER_GAME_IS_OPEN_WORLD_MODE:

            return GameIsOpenWorld() != FALSE;



        case MODIFIER_GAME_IS_SEASONAL:

            return GameIsSeasonal() != FALSE;



        case MODIFIER_GAME_IS_AT_LEAST_LEVEL:

            return GameGetMonsterLevel() >= operand;



        case MODIFIER_GAME_IS_PUBLIC:
        {


            return Console::Online::PartyIsPublic() &&
                   !SPlayersAreAllLocal();

        }




        case MODIFIER_GAME_IS_PRIVATE:
        {


            return !Console::Online::PartyIsPublic() ||
                   SPlayersAreAllLocal();

        }



        case MODIFIER_ALL_PLAYERS_ARE_DIFFERENT_CLASSES:
        {


            bool arClassesInUse[7]; SigmaMemoryFill(arClassesInUse, 0, sizeof(arClassesInUse));

            for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            {
                PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);

                if (arClassesInUse[ePlayerClass])
                {
                    return false;
                }


                arClassesInUse[ePlayerClass] = true;
            }

            return true;
        }



        case MODIFIER_GAME_TIMESTAMP_REACHED:
        {
            if (operand < sizeof(arTimestamps) / sizeof(arTimestamps[0]))
            {


                Blizzard::Time::UnixTime tUnixTime = arTimestamps[operand].Get();
                return Blizzard::Time::GetTimestamp() >= Blizzard::Time::FromUnixTime64(tUnixTime);
            }
            return false;
        }

        case MODIFIER_GAME_TIME_IS_LESS_THAN:

            return GameGetServerTime() <= operand * 3600;



        case MODIFIER_GAME_IS_TIERED_LOOT_RUN_NOT_ACTIVE:

            return NephalemRift::IsTieredRiftActive() == FALSE;


        case MODIFIER_GAME_IS_TIERED_LOOT_RUN_SOLO:

            if (NephalemRift::IsTieredRiftActive())
            {
                if (NephalemRiftManager::RiftPlayerCount() == 1)
                {  // The rift count branches to the shared false exit at 0x7b44ec.
                    return true;
                }
            }
            break;
    }

    return false;



}


bool EvaluationData::EvaluatePlayerModifiers(
    uint64 modifierId,
    uint64 operand) const
{
    const Player * ptPlayer = GetPlayer();
    if (!ptPlayer)
        return false;



    switch (modifierId)
    {
        case MODIFIER_PLAYER_LEVEL_EQUALS:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            return ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) == operand;
        }


        case MODIFIER_PLAYER_LEVEL_LESS_THAN:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            return ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) < operand;
        }


        case MODIFIER_PLAYER_LEVEL_GREATER_THAN:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            return ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL) > operand;
        }


        case MODIFIER_PLAYER_PARAGON_LEVEL_EQUALS:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            return ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_ALT_LEVEL) == operand;
        }


        case MODIFIER_PLAYER_PARAGON_LEVEL_LESS_THAN:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            return ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_ALT_LEVEL) < operand;
        }


        case MODIFIER_PLAYER_PARAGON_LEVEL_GREATER_THAN:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            return ptACDPlayer && ptACDPlayer->AttributesGetInt(ATTRIBUTE_ALT_LEVEL) > operand;
        }


        case MODIFIER_PLAYER_IS_HARDCORE:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if ((ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) && ptACDPlayer && ptACDPlayer->IsAlive())
                return true; // Inference: if/return form (no row); the original branches on IsAlive into the shared true exit 0x7b4f8c.
            break;
        }




        case MODIFIER_PLAYER_IS_SOFTCORE:
        {
            return !(ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE));
        }


        case MODIFIER_PLAYER_CLASS_EQUALS:
        {
            PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer->idPlayerACD);
            return ePlayerClass == (PlayerClass)operand;
        }


        case MODIFIER_IS_ANOTHER_PLAYER:
        {
            return m_ptEvaluatingPlayer->nPlayerIndex != ptPlayer->nPlayerIndex;
        }



        case MODIFIER_PLAYER_HEALTH_PERCENTAGE_IS_LESS_THAN:
        {
            ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
            return ptPlayerACD && ptPlayerACD->GetHitpointPercent() * 100.0f < (float)operand;
        }


        case MODIFIER_NUMBER_OF_PETS_GREATER_THAN: {
            // Inference: the zero initial count is folded into the loop setup/shared empty-list return.
            int32 nPetCount = 0;

            int32 nPos;
            ACDID idACDPet = PlayerGetFirstPet(ptPlayer->idPlayerACD, PETTYPE_NONE, nPos);


            while (idACDPet != ACDID_INVALID)
            {
                ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
                if (ptACDPet)
                {
                    PetType ePetType = ptACDPet->AttributesGetInt(ATTRIBUTE_PET_TYPE);
                    if (IsPetTypeEvaluable(ePetType))
                    {
                        ++nPetCount;
                    }
                }


                idACDPet = PlayerGetNextPet(ptPlayer->idPlayerACD, PETTYPE_NONE, nPos);
            }

            return nPetCount > operand;
        }



        case MODIFIER_NUMBER_OF_ZOMBIE_DOG_PETS_GREATER_THAN:
        {
            if (PlayerGetPlayerClass(ptPlayer) != PLAYER_CLASS_WITCHDOCTOR)
                return false;


            // Inference: the loop's initial zero count has no distinct line-table row.
            int32 nZombieDogCount = 0;


            int32 nPos;
            ACDID idACDZombieDog = PlayerGetFirstPet(ptPlayer->idPlayerACD, PETTYPE_GENERIC_PET_0, nPos);
            while (idACDZombieDog != ACDID_INVALID)
            {
                ++nZombieDogCount;



                idACDZombieDog = PlayerGetNextPet(ptPlayer->idPlayerACD, PETTYPE_GENERIC_PET_0, nPos);
            }

            return nZombieDogCount > operand;
        }


        case MODIFIER_ANOTHER_PLAYER_HAS_COMPANION:
        {
            for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
            {
                if (ptPlayerCurr == ptPlayer)
                    continue;


                // Original +0x778 loads idPlayerACD from evaluated ptPlayer (x20), not loop ptPlayerCurr (x22).
                ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptPlayerACD)
                {
                    int32 nPos;
                    ACDID idACDPet = PlayerGetFirstPet(ptPlayerACD->id, PETTYPE_COMPANION, nPos); // Original 0x7b4cf4 reuses the looked-up id (w21), so the argument is ptPlayerACD->id, as at 709.


                    while (idACDPet != ACDID_INVALID)
                    {
                        ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
                        if (ptACDPet && ptACDPet->snoActor == operand)
                        {
                            return true;
                        }





                        idACDPet = PlayerGetNextPet(ptPlayerACD->id, PETTYPE_COMPANION, nPos);
                    }
                }
            }
            return false; // Inference: own return after the loop (no row); the original zeroes w21 in the loop latch (0x7b4d88) instead of the shared false block.
        }



        case MODIFIER_PLAYER_IS_DEAD:
        {
            ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);


            return ptPlayerACD && !ptPlayerACD->IsAlive();
        }




        case MODIFIER_PLAYER_HAS_STASH_OF_SIZE_GREATER_THAN_EQUALS:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer)
            {
                int32 nSlotCount = ptACDPlayer->AttributesGetInt(ATTRIBUTE_SHARED_STASH_SLOTS);
                return nSlotCount >= operand;
            }
            break;
        }



        case MODIFIER_BLACKSMITH_LEVEL_GREATER_THAN_EQUALS:
        {
            int32 nCrafterLevel = GetCraftLevel(CRAFTER_TYPE_BLACKSMITH, *ptPlayer);
            int32 nCrafterRank = CrafterGetRank(CRAFTER_TYPE_BLACKSMITH, nCrafterLevel);
            return nCrafterRank >= operand;
        }


        case MODIFIER_JEWELER_LEVEL_GREATER_THAN_EQUALS:
        {
            int32 nCrafterLevel = GetCraftLevel(CRAFTER_TYPE_JEWEL, *ptPlayer);
            int32 nCrafterRank = CrafterGetRank(CRAFTER_TYPE_JEWEL, nCrafterLevel);
            return nCrafterRank >= operand;
        }


        case MODIFIER_MYSTIC_LEVEL_GREATER_THAN_EQUALS:
        {
            int32 nCrafterLevel = GetCraftLevel(CRAFTER_TYPE_MYSTIC, *ptPlayer);
            int32 nCrafterRank = CrafterGetRank(CRAFTER_TYPE_MYSTIC, nCrafterLevel);
            return nCrafterRank >= operand;
        }


        case MODIFIER_CUBE_LEVEL_GREATER_THAN_EQUALS:
        {
            int32 nCrafterLevel = GetCraftLevel(CRAFTER_TYPE_HORADRIM, *ptPlayer);
            int32 nCrafterRank = CrafterGetRank(CRAFTER_TYPE_HORADRIM, nCrafterLevel);
            return nCrafterRank >= operand;
        }


        case MODIFIER_PLAYER_HAS_COMPLETED_BOUNTIES_GREATER_THAN_EQUALS:
        { // Original 0x7b52a4 fcvtzu: the float counter converts straight to uint32.
            uint32 nCompletedBounties = SGameStatsPlayerCounterFetch(ptPlayer, STAT_COUNTER_PLAYER_BOUNTY_COMPLETED_ALL, 0, 0);
            return nCompletedBounties >= (uint32)operand;
        }


        case MODIFIER_BLACKSMITH_RECIPES_GREATER_THAN_EQUALS: {
            // Inference: zero initial recipe count is folded into the loop setup/empty-list return.
            int32 nRecipeCount = 0;
            const CrafterData & tData = ptPlayer->tCrafterSavedData.tCrafterData[CRAFTER_TYPE_BLACKSMITH];
            for (int32 i = 0; i < tData.nNumRecipes; ++i)
            {
                if (!GBIDExists(tData.gbidRecipes[i], GB_RECIPES))
                    continue;




                ScopedRecipeEntryLockR pdefRecipeEntry(tData.gbidRecipes[i]);


                if (pdefRecipeEntry && (pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
                    ++nRecipeCount;


            }
            return nRecipeCount >= operand;
        }



        case MODIFIER_JEWELER_RECIPES_GREATER_THAN_EQUALS: {
            // Inference: zero initial recipe count is folded into the loop setup/empty-list return.
            int32 nRecipeCount = 0;
            const CrafterData & tData = ptPlayer->tCrafterSavedData.tCrafterData[CRAFTER_TYPE_JEWEL];
            for (int32 i = 0; i < tData.nNumRecipes; ++i)
            {
                if (!GBIDExists(tData.gbidRecipes[i], GB_RECIPES))
                    continue;




                ScopedRecipeEntryLockR pdefRecipeEntry(tData.gbidRecipes[i]);
                if (pdefRecipeEntry && (pdefRecipeEntry->dwFlags & (1 << RECIPE_DROPPED_BIT)))
                    ++nRecipeCount;


            }
            return nRecipeCount >= operand;
        }



        case MODIFIER_PLAYER_EXTRACTED_POWERS_GREATER_THAN_EQUALS:
        {
            int32 nExtractedLegendariesCount = ptPlayer->tCrafterSavedData.tExtractedLegendariesData.nExtractedLegendariesCount;
            return nExtractedLegendariesCount >= operand;
        }


        case MODIFIER_PLAYER_HAS_POWER_ON_HOTBAR:
        {
            SNO snoPower = (SNO)operand; // Inference for next line: seven original calls imply the inclusive NUM_SKILL_BUTTONS bound.
            for (int32 i = 0; i <= NUM_SKILL_BUTTONS; ++i)
            {
                const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, i);
                if (tButtonData.m_snoPower == snoPower)
                    return true;
            }
            return false; // Inference: own return after the loop (no row); the last unrolled test's cset at 0x7b5428 needs a single-predecessor exit.
        }





        case MODIFIER_PLAYER_IS_IN_LOOT_RUN:
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer && ptACDPlayer->IsInWorld())
            {
                return WorldIsDungeonFinderPlaceHolder(WorldCommonGetWorldSNO(ptACDPlayer->GetSWorldID())) != FALSE;
            }
            break;
        }





        case MODIFIER_PLAYER_IS_IN_TIERED_LOOT_RUN:
        {
            return NephalemRiftManager::GetActiveRiftLevel() + 1 >= (int32)operand && NephalemRiftManager::PlayerPaidForRift(ptPlayer);
        }


        case MODIFIER_PLAYER_IS_NOT_IN_LEVEL_AREA:
        {
            return !PlayerIsInLevelArea(ptPlayer->idPlayerACD, (SNO)operand);
        }


        case MODIFIER_PLAYER_IS_IN_ACT:
        {
            SNO snoLastWaypointLevelArea = PlayerGetLastWaypointLevelArea(ptPlayer);
            Act eSelectedAct = ActGetWaypointActByLevelArea(snoLastWaypointLevelArea);
            return eSelectedAct == operand;
        }



        case MODIFIER_PLAYER_HAS_BUFF:
        {
            PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

            ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptPlayerACD)
            {
                POS pos = ptPlayerACD->listBuffs.GetHeadPosition();
                while (pos)
                {
                    BuffID idBuff = ptPlayerACD->listBuffs.GetNext(pos);

                    PowerAPIBuffInstance * ptBuff = ptPowerGlobals->pArrayBuffs->ElementGet(idBuff);
                    if (!ptBuff->bDeleted && ptBuff->snoPower == operand)
                        return true;
                }
            }
            return false;
        }




        case MODIFIER_PLAYER_NOT_HAS_BUFF:
        {
            PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

            ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptPlayerACD)
            {
                POS pos = ptPlayerACD->listBuffs.GetHeadPosition();
                while (pos)
                {
                    BuffID idBuff = ptPlayerACD->listBuffs.GetNext(pos);

                    PowerAPIBuffInstance * ptBuff = ptPowerGlobals->pArrayBuffs->ElementGet(idBuff);
                    if (!ptBuff->bDeleted && ptBuff->snoPower == operand)
                        return false;
                }
            }
            return true;
        }
        // Inference: explicit default spells the original shared false-result exit; no distinct line row survives.
        default: break;

        case MODIFIER_HAS_1_LEGENDARY_GEM_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_2_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_3_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_4_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_5_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_6_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_7_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_8_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_9_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_10_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_11_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_12_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_13_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_14_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_15_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_16_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_17_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_18_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_19_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        case MODIFIER_HAS_20_LEGENDARY_GEMS_GREATER_THAN_LEVEL:
        {
            // Inference: contiguous modifier values encode required count starting at one.
            uint64 nRequiredKeysOfLevel = modifierId - MODIFIER_HAS_1_LEGENDARY_GEM_GREATER_THAN_LEVEL + 1;

            ACDIDArray aridItems;
            ItemsFindUpgradeableJewels(ptPlayer, TRUE, aridItems);
            // Inference: the zero count is folded into the loop setup and empty-array path.
            int32 nNumKeysAboveLevel = 0;
            for (int32 i = 0; i < (int32)aridItems.size(); ++i)
            {
                ACDID idACDItem = aridItems[i];
                ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
                if (ptACDItem && ptACDItem->AttributesGetInt(ATTRIBUTE_JEWEL_RANK) > operand)
                    ++nNumKeysAboveLevel;
            }


            // The comparison after vector destruction branches to the shared true exit (0x7b4660).
            if (nNumKeysAboveLevel >= nRequiredKeysOfLevel) return true;
        }
    }
    return false;
}







bool EvaluationData::EvaluateEquipmentModifiers(uint64 modifierId, uint64 operand)

{
    switch (modifierId)
    {

        case MODIFIER_ALL_EQUIPPED_ITEMS_DYED_DIFF:
        {
            const Achievement::EquipmentInfo * ptEquipmentInfo = GetEquipmentInfo();
            return ptEquipmentInfo && ptEquipmentInfo->IsAllEquipmentDyedDifferently();
        }

        case MODIFIER_ALL_EQUIPPED_ITEMS_DYED_SAME:
        {

            const Achievement::EquipmentInfo * ptEquipmentInfo = GetEquipmentInfo();
            return ptEquipmentInfo && ptEquipmentInfo->IsAllEquipmentDyedColor(DyeTypeTranslateFromSavedDyeType((SavedDyeType)operand));
        }

        case MODIFIER_EQUIPMENT_LEVEL_GREATER_THAN_EQUAL_TO:
        {

            const Achievement::EquipmentInfo * ptEquipmentInfo = GetEquipmentInfo();
            return ptEquipmentInfo && (uint64)ptEquipmentInfo->nMinRequiredLevel >= operand;
        }

        case MODIFIER_HAS_NOTHING_EQUIPPED:
        {

            const Achievement::EquipmentInfo * ptEquipmentInfo = GetEquipmentInfo();
            return ptEquipmentInfo && ptEquipmentInfo->nEquipmentCount == 0;
        }

        case MODIFIER_HAS_ANYTHING_EQUIPPED:
        {

            const Achievement::EquipmentInfo * ptEquipmentInfo = GetEquipmentInfo();
            return ptEquipmentInfo && ptEquipmentInfo->nEquipmentCount > 0;
        }

        case MODIFIER_HAS_ITEM_TYPE_EQUIPPED:
        {

            const Achievement::EquipmentInfo * ptEquipmentInfo = GetEquipmentInfo();
            return ptEquipmentInfo && ptEquipmentInfo->HasItemLabelEquipped((ItemLabel)operand);
        }

        case MODIFIER_HAS_NO_SET_ITEMS_EQUIPPED:
        {

            const Achievement::EquipmentInfo * ptEquipmentInfo = GetEquipmentInfo();
            return ptEquipmentInfo && ptEquipmentInfo->HasNoSetItems();
        }
    }

    return false;

}


bool EvaluationData::EvaluateMonsterModifiers(
    uint64 modifierId,
    uint64 operand) const
{
    const ActorCommonData * ptACDMonsterActor = GetACD();
    if (!ptACDMonsterActor || !ptACDMonsterActor->IsMonster())
    {
        return false;
    }

    switch (modifierId)
    {
        case MODIFIER_MONSTER_ACTIVE_BRAIN_COMPONENT:
        {
            ActorID idActor = (ActorID)ACDGetOwner(ptACDMonsterActor->id);
            return BrainAPIGetActiveComponent(idActor) == operand;
        }


        case MODIFIER_MONSTER_RARITY_EQUALS:
        {
            return ptACDMonsterActor->eMonsterRarity == operand;
        }


        case MODIFIER_MONSTER_RARITY_GREATER_THAN:
        { // Inference: original compares rarity <= operand (unsigned, cset ls); operand-first spelling.
            return operand >= ptACDMonsterActor->eMonsterRarity;
        }


        case MODIFIER_MONSTER_BELONGS_TO_FAMILY:
        {
            MonsterFamily eMonsterFamily = MonsterGetMonsterFamily(SNOName(ACTOR_SNO, ptACDMonsterActor->snoActor));
            return eMonsterFamily == operand;
        }


        case MODIFIER_MONSTER_TYPE_IS:
        {
            MonsterType eMonsterType = MonsterGetMonsterType(SNOName(ACTOR_SNO, ptACDMonsterActor->snoActor));
            return eMonsterType == operand;
        }


        case MODIFIER_BELONGS_TO_ACTOR_GROUP:
        {
            return ptACDMonsterActor->uGroupPrimary == operand ||
                ptACDMonsterActor->uGroupSecondary == operand;
        }


        case MODIFIER_SLAIN_BY_MONSTER:
        {
            return ptACDMonsterActor->GetActorSNO() == operand;
        }


        case MODIFIER_MONSTER_IS_CHARMED:
        {
            return ptACDMonsterActor->IsCharmed() == TRUE;
        }


        case MODIFIER_MONSTER_IS_AT_LEAST_PLAYER_MAX_LEVEL:
        {
            const Player * ptPlayer = GetPlayer();
            const ActorCommonData * ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;
            if (!ptACDPlayer)
            {
                return false;
            }

            int32 nLevelCap = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL_CAP);
            return ptACDMonsterActor->AttributesGetInt(ATTRIBUTE_LEVEL) >= nLevelCap;
        }


        case MODIFIER_MONSTER_HAS_BUFF:
        {
            PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;

            POS pos = ptACDMonsterActor->listBuffs.GetHeadPosition();
            while (pos)
            {
                BuffID idBuff = ptACDMonsterActor->listBuffs.GetNext(pos);

                PowerAPIBuffInstance * ptBuff = ptPowerGlobals->pArrayBuffs->ElementGet(idBuff);
                if (!ptBuff->bDeleted)
                {
                    Power * ptPower = PowerTryToGet(ptBuff->idPower);
                    if (ptPower &&

                        ptPower->snoPower == operand)
                    {
                        return true;
                    }
                }
            }
            return false;
        }



        case MODIFIER_MONSTER_IS_CURSED:
        {

            const int32 NUM_CURSES = 3;
            static FastAttribKey arNecroCursedAttribKeys[NUM_CURSES] = {
                MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSED, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_LEECH_PASSIVE_MANAGER)),
                MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSED, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_DECREPIFY_PASSIVE_MANAGER)),
                MakeAttribKey(ATTRIBUTE_NECROMANCER_CURSED, GlobalSNOGet(GLOBALSNO_POWERS_NECRO_FRAILTY_PASSIVE_MANAGER)),
            };

            for (int32 i = 0; i < NUM_CURSES; i++)
            {
                int32 nOwnerPlayerIndex = ptACDMonsterActor->AttributesGetInt(arNecroCursedAttribKeys[i]);
                if (nOwnerPlayerIndex != -1) return true;
            }
            return false;
        }
    }

    return false;
}







bool EvaluationData::EvaluateItemModifiers(
    uint64 modifierId,
    uint64 operand) const
{
    const ActorCommonData * ptACDItem = GetACD();
    if (!ptACDItem || !ptACDItem->IsItem())
    {
        return false;
    }

    switch (modifierId)
    {
        case MODIFIER_ITEM_EQUALS:
        { // Inference: original GBID comparison zero-extends the 32-bit identifier.
            return uint32(ptACDItem->hGB.gbid) == operand;
        }

        case MODIFIER_ITEM_HAS_USE_TYPE:
        {

            ItemUseType eItemUseType = ItemGetUseType(*ptACDItem);
            return eItemUseType == operand;
        }

        case MODIFIER_ITEM_QUALITY_EQUALS:
        {

            ItemQualityLevel eItemQualityLevel = ItemGetQualityLevel(*ptACDItem);
            return eItemQualityLevel == operand;
        }

        case MODIFIER_ITEM_QUALITY_GREATER_THAN:
        {

            ItemQualityLevel eItemQualityLevel = ItemGetQualityLevel(*ptACDItem);
            return eItemQualityLevel > operand;
        }

        case MODIFIER_GEM_TYPE_EQUALS:
        {

            if (ItemIsGem(*ptACDItem))
            {
                ScopedItemDefinitionLockR ptDefGem(ptACDItem->hGB.gbid);
                return ptDefGem->eGemType == operand;
            }
            break;
        }

        case MODIFIER_GEM_QUALITY_IS:
        {
            return ItemIsGem(*ptACDItem) && ItemCrafting_GetGemQuality(ptACDItem->hGB.gbid) == operand;
        }

        case MODIFIER_GEM_QUALITY_GREATER_THAN:
        {

            return ItemIsGem(*ptACDItem) && ItemCrafting_GetGemQuality(ptACDItem->hGB.gbid) > operand;
        }

        case MODIFIER_ITEM_LEVEL_EQUALS:
        {

            int32 nRequiredLevel = ItemGetLevelRequirement(ptACDItem->hGB.gbid, ptACDItem, FALSE);
            return nRequiredLevel == operand;
        }

        case MODIFIER_ITEM_LEVEL_GREATER_THAN:
        {

            int32 nRequiredLevel = ItemGetLevelRequirement(ptACDItem->hGB.gbid, ptACDItem, FALSE);
            return nRequiredLevel > operand;
        }

        case MODIFIER_ITEM_TYPE_EQUALS:
        {

            XGrowableList<int, int> labels;
            ItemGetLabels(labels, ptACDItem->hGB.gbid);

            POS pos = labels.GetHeadPosition();
            while (pos)
            {
                ItemLabel eItemLabel = labels.GetNext(pos);
                if (eItemLabel == operand)
                {
                    return true;
                }
            }
            // Inference: exhausted label search reaches the common false result.
            return false;
        }

        case MODIFIER_ITEM_HAS_AFFIX:
        {

            const GBIDPooledList & listAffixGBIDs = ItemIsUnidentified(*ptACDItem) ?
                ptACDItem->listUnidentifiedAffixGBIDs : ptACDItem->listIdentifiedAffixGBIDs;

            POS pos = listAffixGBIDs.GetHeadPosition();
            while (pos)
            { // Inference: original affix-GBID comparison zero-extends the identifier.
                GBID gbidAffix = listAffixGBIDs.GetNext(pos);
                if (uint32(gbidAffix) == operand)
                {
                    return true;
                }
            }
            break;
        }

        case MODIFIER_ITEM_HAS_AFFIX_FAMILY:
        {

            const GBIDPooledList & listAffixGBIDs = ItemIsUnidentified(*ptACDItem) ?
                ptACDItem->listUnidentifiedAffixGBIDs : ptACDItem->listIdentifiedAffixGBIDs;

            bool bResult = false;
            POS pos = listAffixGBIDs.GetHeadPosition();
            while (pos && !bResult)
            {
                const GameBalanceDefinition * pdefGBAffixes = NULL;
                GBHandle hGB(8 /* GB_AFFIXES, original GameBalanceDefinition.h:34 */, listAffixGBIDs.GetNext(pos)); // Inference: DWARF ptAffix types the GBLockR void result below; no const is removed.
                const AffixTableEntry * ptAffix = static_cast<const AffixTableEntry *>(GBLockR(hGB, pdefGBAffixes, TRUE));
                // Inference: unsigned GBID comparisons follow the original zero-extending loads.
                if (uint32(ptAffix->gbidAffixFamily0) == operand ||
                    uint32(ptAffix->gbidAffixFamily1) == operand)
                {
                    bResult = true;
                }

                GBUnlockR(pdefGBAffixes);
            }
            return bResult;
        }
    }

    // Inference: unmatched modifiers and failed gem tests share the false-return tail.
    return false;

}


bool EvaluationData::EvaluatePowerModifiers(uint64 modifierId, uint64 operand) const


{
    SNO snoPower = GetPowerSNO();
    if (snoPower == SNO_NONE)
        return false;



    switch (modifierId)
    {
        case MODIFIER_POWER_EQUALS:
        {

            return snoPower == (SNO)operand;
        }

        case MODIFIER_POWER_IS_PASSIVE:
        {

            return PowerIsPassive(AutoPowerDefinitionR(snoPower)) != FALSE;
        }

        case MODIFIER_POWER_IS_NOT_PASSIVE:
        {

            return PowerIsPassive(AutoPowerDefinitionR(snoPower)) == FALSE;
        }

        case MODIFIER_POWER_IS_BASIC_ATTACK:
        {

            return PowerIsBasicAttack(AutoPowerDefinitionR(snoPower)) != FALSE;
        }

        case MODIFIER_POWER_IS_NOT_BASIC_ATTACK:
        {

            return PowerIsBasicAttack(AutoPowerDefinitionR(snoPower)) == FALSE;
        }

        case MODIFIER_POWER_DID_KNOCKBACK:
        {

            const PowerAPIDamage * ptPowerDamage = GetPowerDamage();
            if (ptPowerDamage)
            {
                return ptPowerDamage->eKnockBack != 0;
            }
            break;
        }
    }

    return false;
}


bool EvaluationData::EvaluateCrafterTypeModifiers(uint64 modifierId, uint64 operand) const
{


    CrafterType eCrafterType = GetCrafterType();
    switch (modifierId)
    {
        case MODIFIER_CRAFTER_CLASS_IS:
        {






            if (eCrafterType != -1)
                return eCrafterType == operand;
            break;
        }
    }

    return false;
}
bool EvaluationData::EvaluateBuffModifiers(uint64 modifierId, uint64 operand) const
{


    const PowerAPIBuffInstance * ptBuff = GetBuff();
    if (!ptBuff)
        return false;




    switch (modifierId)
    {
        case MODIFIER_POWER_BUFF_DURATION_GREATER_THAN_EQUAL:
        {
            uint32 dwNow = GameGetServerTime();
            uint32 dwTickDuration = dwNow - ptBuff->dwInitialInstanceStartTick;
            return dwTickDuration >= (uint32)operand;
        }
        case MODIFIER_POWER_BUFF_DURATION_LESS_THAN_EQUAL:
        {

            uint32 dwNow = GameGetServerTime();
            uint32 dwTickDuration = dwNow - ptBuff->dwInitialInstanceStartTick;
            return dwTickDuration <= (uint32)operand;
        }
        case MODIFIER_POWER_BUFF_INDEX_EQUALS:
        {


            return ptBuff->nBuffIndex == (int32)operand;
        }
    }

    return false;
}



bool EvaluationData::EvaluatePetModifiers(uint64 modifierId, uint64 operand) const
{


    const ActorCommonData * ptACDPet = GetACD();
    if (!ptACDPet || !ptACDPet->IsPet())
        return false;

    switch (modifierId)
    {
        case MODIFIER_HIRELING_CLASS_EQUALS:
        {


            return ACDGetHirelingClass(*ptACDPet) == operand;
        }
    }

    return false;
}



bool EvaluationData::EvaluateMiscellaneousModifiers(
    uint64 modifierId,
    uint64 operand) const
{


    switch (modifierId)
    {
        case MODIFIER_GIZMO_OPERATED_BY_PLAYER:
        {
            const Player * ptPlayer = GetPlayer();
            const ActorCommonData * ptACD = GetACD();

            if (ptPlayer && ptACD)
            {

                if (ptACD->AttributesAttributeExists(FastAttribKey(ATTRIBUTE_GIZMO_OPERATOR_ACDID)))
                {  // A zero attribute-exists result reaches the false exit at 0x7b5f90.
                    ACDID idACDOperator = (ACDID)ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_GIZMO_OPERATOR_ACDID));


                    return idACDOperator == ptPlayer->idPlayerACD;
                }
            }
            break;
        }



        case MODIFIER_GIZMO_EQUALS:
        {
            const ActorCommonData * ptACD = GetACD();
            return ptACD && ptACD->snoActor == (SNO)operand;
        }


        case MODIFIER_SLAIN_BY_ENVIRONMENT:
        {
            const ActorCommonData * ptACD = GetACD();
            if (ptACD && ptACD->IsWreckableObject())
            {
                return ptACD->GetActorSNO() == operand;
            }
            break;
        }



        case MODIFIER_ACTIVE_WEAPON_EQUALS:
        {
            const Player * ptPlayer = GetPlayer();
            if (ptPlayer)
            {
                const ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACD)
                {
                    ACDID idACDWeapon = ACDInventoryGetActiveWeapon(*ptACD);
                    if (idACDWeapon != ACDID_INVALID)
                    {  // The original skips invalid weapon IDs at 0x7b6050.
                        ActorCommonData * ptACDWeapon = ACDTryToGet(idACDWeapon);
                        if (ptACDWeapon)
                        {
                            return (uint32)ptACDWeapon->hGB.gbid == operand;
                        }
                    }
                }
            }
            break;
        }



        case MODIFIER_POWER_DISTANCE_TO_TARGET:
        {
            const ActorCommonData * ptACD = GetACD();
            const Power * ptPower = GetPower();

            if (ptACD && ptPower &&
                ptACD->GetLocation().eActorLocation == AL_WORLD)
            {



                Vector3D wpStart = ptPower->wpStartLocation.wp;
                Vector3D wpEnd = ptACD->GetLocation().tWorldLocation.transform.wp;
                // The original subtracts the start coordinates from the end coordinates (0x7b5f00).
                WorldDistance wdActual = Vector3D::Distance(wpStart, wpEnd);
                return wdActual >= (float)operand;
            }
            break;
        }

        case MODIFIER_CURSED_CHEST_EVENT_KILLED_MONSTER_COUNT_GREATER_THAN:
        {
            return GetCursedChestKillCount() > operand;
        }
    }

    return false;

}


// LINKER-REMOVED: DWARF size 0x2c; address-0 rows put a 2-instruction store at 1565:5 before the zero stores (1567, 1575): the owner ID gets a non-zero constant, inferred ACDID_INVALID (the inlined copy in SetEquipmentInfo loses it to the caller's own store).
EquipmentInfo::EquipmentInfo()
    : tACDIDEquipmentOwner(ACDID_INVALID)
{
    nEquipmentCount = nMinRequiredLevel = nNumSocketedItems = nNumDyedItems = nNumEnchantedItems = nNumLegendaryTransmogrifiedItems = nNumMaxLevelLegendaryItems = nNumSetItems = 0;
    bTwoHandedWeaponEquipped = false;





    nUsedDyesCount = 0;
    // Inference: original zero stores cover both bitsets.
    memset(setUsedDyes, 0, sizeof(setUsedDyes)); memset(setItemLabels, 0, sizeof(setItemLabels));
}



bool EquipmentInfo::IsDyeUsed(DyeType eDye) const
{
    return (setUsedDyes[eDye >> 5] & (1 << (eDye & 31))) != 0;
}

int32 EquipmentInfo::GetDyeUsedCount() const
{
    return nUsedDyesCount;
}
// DWARF low_pc 0: standalone body is not checkable; required by sUpdateEquipmentInfoFromSlot.
void EquipmentInfo::SetDyeUsed(DyeType eDye)
{
    if (!IsDyeUsed(eDye))
    {
        setUsedDyes[eDye >> 5] |= (1 << (eDye & 31));
        nUsedDyesCount++;
    }
}
// DWARF low_pc 0: standalone body is not checkable; required by SetEquipmentInfo.
void EquipmentInfo::ClearDyeUsed()
{ // Inference: original i values 0/32 imply a 32-bit stride over the dye bitset.
    for (int32 i = 0; i < MAX_DYE_VALUE; i += 32) setUsedDyes[i >> 5] = 0;
    nUsedDyesCount = 0;
}

// DWARF low_pc 0: standalone body is not checkable; required by sUpdateEquipmentInfoFromSlot.
void EquipmentInfo::SetItemLabelEquipped(ItemLabel eItemLabel)
{
    setItemLabels[eItemLabel >> 5] |= (1 << (eItemLabel & 31));
}
// DWARF low_pc 0: standalone body is not checkable; required by SetEquipmentInfo.
void EquipmentInfo::ClearItemLabelEquipped()
{ // Inference: original i=32 and five zero words imply a 32-bit stride over the label bitset.
    for (int32 i = 0; i < NUM_ITEM_LABELS; i += 32) setItemLabels[i >> 5] = 0;
}


bool EquipmentInfo::IsAllEquipmentDyed() const
{
    if (ACDIsPlayer(tACDIDEquipmentOwner))
    {
        const int32 nMaxDyeableSlotCount = 6;
        return nNumDyedItems == nMaxDyeableSlotCount;
    }

    return false;
}


bool EquipmentInfo::IsAllEquipmentDyedColor(DyeType eDyeType) const
{
    if (IsAllEquipmentDyedOneColor())
    {
        DyeType eDyeUsed = 0;
        for (int32 i = 0; i < 23; i++)
        {
            if (IsDyeUsed(i))
            {
                eDyeUsed = i;
                break;
            }
        }
        return eDyeUsed == eDyeType;
    }

    return false;
}



bool EquipmentInfo::IsAllEquipmentDyedDifferently() const
{
    if (ACDIsPlayer(tACDIDEquipmentOwner))
    {
        const uint32 nMaxDyeableSlotCount = 6;
        return nUsedDyesCount >= nMaxDyeableSlotCount;
    }

    return false;
}


bool EquipmentInfo::IsAllEquipmentEnchanted() const
{
    int32 nNumSlots = ACDIsPlayer(tACDIDEquipmentOwner) ? 13 : 6;

    if (bTwoHandedWeaponEquipped)
        nNumSlots--;

    return nNumEnchantedItems >= nNumSlots;
}




bool EquipmentInfo::IsAllEquipmentTransmogrifiedToLegendary() const
{
    if (ACDIsPlayer(tACDIDEquipmentOwner))
    {
        int32 nNumSlots;  // DWARF has no location for this local, and no initialization survives.





        // The original loads the member and global directly on this return line (0x7b65dc–0x7b65ec).
        return nNumLegendaryTransmogrifiedItems >= sgnTransmogSlotsCount;
    }

    return false;
}


bool EquipmentInfo::IsAllHirelingEquipmentMaxLevelLegendary() const
{
    ActorCommonData * ptACD = ACDTryToGet(tACDIDEquipmentOwner);
    if (!ptACD)
        return false;

    if (ACDIsHireling(*ptACD))
    {
        int32 nNumSlots = bTwoHandedWeaponEquipped ? 5 : 6;
        return nNumMaxLevelLegendaryItems >= nNumSlots;
    }

    return false;
}





bool EquipmentInfo::IsAllEquipmentMaxLevelLegendary() const
{
    if (ACDIsPlayer(tACDIDEquipmentOwner))
    {
        int32 nNumSlots = bTwoHandedWeaponEquipped ? 12 : 13;
        return nNumMaxLevelLegendaryItems >= nNumSlots;
    }

    return false;
}








inline bool EquipmentInfo::HasItemLabelEquipped(ItemLabel eItemLabel) const
{

    return (setItemLabels[eItemLabel >> 5] & (1 << (eItemLabel & 31))) != 0;
}


inline bool EquipmentInfo::HasNoSetItems() const
{

    if (ACDIsPlayer(tACDIDEquipmentOwner))
    {
        if (nNumSetItems == 0) return true;
    }

    return false;
}





bool EquipmentInfo::IsAllEquipmentDyedOneColor() const
{
    return IsAllEquipmentDyed() && nUsedDyesCount == 1;
}


bool EquipmentInfo::IsFullyEquipped() const
{
    int32 nNumSlots = ACDIsPlayer(tACDIDEquipmentOwner) ? 13 : 6;

    if (bTwoHandedWeaponEquipped)
        nNumSlots--;

    return nEquipmentCount >= nNumSlots;
}










void sUpdateEquipmentInfoFromSlot(
    ACDID idACDSquare,
    InventorySlotType eSlot,
    void * pCallbackData)
{ // Inference: DWARF EquipmentInfo reference binds the void callback payload to its original typed object.
    EquipmentInfo & tEquipmentInfo = *static_cast<EquipmentInfo *>(pCallbackData);

    ActorCommonData * ptACDItem = ACDTryToGet(idACDSquare);
    // Inference: missing items take the shared return after the ACD lookup.
    if (ptACDItem)
    {
        int32 nRequiredLevel = ItemGetLevelRequirement(ptACDItem->hGB.gbid, ptACDItem, FALSE);
        tEquipmentInfo.nMinRequiredLevel = Math::Min(tEquipmentInfo.nMinRequiredLevel, nRequiredLevel);

        DyeType eDyeType = ptACDItem->GetDyeType();
        if (eDyeType != 0)
        {
            tEquipmentInfo.nNumDyedItems++;
            tEquipmentInfo.SetDyeUsed(eDyeType);
        }

        if (ItemHasEnchantedAffix(*ptACDItem))
        {
            tEquipmentInfo.nNumEnchantedItems++;
        }

        if (ItemIsSocketed(*ptACDItem))
        {
            tEquipmentInfo.nNumSocketedItems++;
        }

        if (ItemIsLegendary(ptACDItem->hGB.gbid))
        {
            int32 nItemLevel = ItemGetItemLevel(ptACDItem->hGB.gbid);
            int32 nLegendaryItemLevel = ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE);
            if (nLegendaryItemLevel)
            {
                nItemLevel = nLegendaryItemLevel;
            }

            if (nItemLevel >= HERO_LEVEL_MAX)
            {
                tEquipmentInfo.nNumMaxLevelLegendaryItems++;
            }
        }

        if (ItemIsSetItem(ptACDItem->hGB.gbid))
        {
            tEquipmentInfo.nNumSetItems++;
        }

        GBID gbidTransmog = ptACDItem->AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
        if (gbidTransmog != GBID_NONE && ItemIsLegendary(gbidTransmog))
        {
            tEquipmentInfo.nNumLegendaryTransmogrifiedItems++;
        }

        XGrowableList<int, int> labels;
        ItemGetLabels(labels, ptACDItem->hGB.gbid);
        POS pos = labels.GetHeadPosition();
        while (pos)
        {
            tEquipmentInfo.SetItemLabelEquipped(labels.GetNext(pos));
        }

        if (eSlot == SLOT_PLAYER_LEFT_HAND || eSlot == SLOT_HIRELING_LEFT_HAND)
        {
            if (ItemIsTwoHanded(*ptACDItem))
            {






                if (!ItemUsesQuiver(*ptACDItem) || eSlot == SLOT_HIRELING_LEFT_HAND)
                {
                    tEquipmentInfo.bTwoHandedWeaponEquipped = true;
                }
            }
        }


        tEquipmentInfo.nEquipmentCount++;
    }
}

// DWARF inlined-only helper: standalone body is not checkable; required by SetEquipmentInfo. Inference: class switch returns false for Scoundrel/Enchantress, matching the caller inversion.
BOOL sCanEquipOffhand(
    ActorCommonData & tACDOwner)
{
    switch (ACDGetHirelingClass(tACDOwner))
    {
        case HC_SCOUNDREL:
        case HC_ENCHANTRESS:
            return FALSE;
        default:
            return TRUE;
    }
}






void EvaluationData::SetEquipmentInfo(
    ACDID idACDOwner)
{

    delete m_ptEquipmentInfo;

    m_ptEquipmentInfo = new EquipmentInfo;

    m_ptEquipmentInfo->tACDIDEquipmentOwner = idACDOwner;

    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    m_ptEquipmentInfo->bTwoHandedWeaponEquipped = ptACDOwner == NULL ? false : !sCanEquipOffhand(*ptACDOwner); // Inference: `== NULL` order; the original null flag (1 on failed lookups, cbz at 0x7b3d2c) is the same compare as `!ptACDOwner` below.
    m_ptEquipmentInfo->nNumSocketedItems = 0;
    m_ptEquipmentInfo->nNumDyedItems = 0;
    m_ptEquipmentInfo->nNumEnchantedItems = 0;
    m_ptEquipmentInfo->nNumLegendaryTransmogrifiedItems = 0;
    m_ptEquipmentInfo->nNumMaxLevelLegendaryItems = 0;
    m_ptEquipmentInfo->nNumSetItems = 0;
    m_ptEquipmentInfo->nEquipmentCount = 0;
    m_ptEquipmentInfo->ClearDyeUsed();
    m_ptEquipmentInfo->ClearItemLabelEquipped();

    if (!ptACDOwner)
    {
        m_ptEquipmentInfo->nMinRequiredLevel = 0;
        return;
    }

    m_ptEquipmentInfo->nMinRequiredLevel = 0x7fffffff;

    if (ACDIsPlayer(idACDOwner))
    { // Inference: original thirteen calls unroll the contiguous player body-slot loop.
        for (int32 i = SLOT_PLAYER_BODYSLOT_START; i < SLOT_PLAYER_BODYSLOT_END; i++)
        {
            ACDInventoryIterateItems(*ptACDOwner, i, 0, sUpdateEquipmentInfoFromSlot, m_ptEquipmentInfo);
        }
    }
    else if (ACDIsHireling(*ptACDOwner))
    { // Inference: original six calls unroll the contiguous hireling slot loop.
        for (int32 i = SLOT_HIRELING_START; i < SLOT_HIRELING_END; i++)
        {
            ACDInventoryIterateItems(*ptACDOwner, i, 0, sUpdateEquipmentInfoFromSlot, m_ptEquipmentInfo);
        }
    }
}





























































}  // namespace Achievement
