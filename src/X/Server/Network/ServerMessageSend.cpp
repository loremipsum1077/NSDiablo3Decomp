// Original file: X/Server/Network/ServerMessageSend.cpp
#include "./ServerMessageSend.h"
#include "./KnownList.h"
#include "../Actors/Actors.h"
#include "../PlayerList.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../SScene.h"
#include "../SWorld.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/Console/Avenger.h"
#include "../../Common/Definitions/SceneDefinition.h"
#include "../../Common/Definitions/WorldDefinition.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/World.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/DataStructures/XOptional.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/Actors/ActorEffectCommon.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/Gizmo.h"
#include "../../Common/GizmoSwig.h"
#include "../../Common/Hireling.h"
#include "../../Common/Items.h"
#include "../../Common/ItemsSwig.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/VisualInventory.h"
#include "../../Common/WorldPlace.h"
#include "../../Client/SNOPreload.h"
#include "../Actors/ActorEffect.h"
#include "../AI/ActorAI.h"
#include "../AI/BrainStance.h"
#include "../Movement/SActorMovement.h"
#include "../Powers/PowerAPI.h"
#include "../System/LevelArea.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/HirelingSwig.h"

int32 PlayerGetPetIndex(const Player & tPlayer, ACDID idACDPet);  // X/Common/Player.cpp:2571
PetType PlayerGetPetType(ACDID idACDPlayer, int32 nIndex);  // X/Common/Player.cpp:2744
PlayerClass PlayerGetPlayerClass(const ACDID idACD);  // X/Common/Player.cpp
void WorldCacheSceneValues(const WorldDefinition * pDefWorld);  // X/Common/World.cpp:240
void DRLGGetWorldSize(SNO snoWorld, int32 & nMinTileSizeInFeetX, int32 & nMinTileSizeInFeetY, int32 & nWorldSizeInFeetX, int32 & nWorldSizeInFeetY);  // X/Common/DRLGWorld.cpp:404
void DRLGGetDungeonStitchSize(const WorldCommonData & tWCD, int32 & nStitchSizeInFeetX, int32 & nStitchSizeInFeetY);  // X/Server/DRLGDungeon.cpp:10678
void SWorldSyncData(SWorldID idSWorld, const Player * ptPlayer);  // X/Server/SWorld.cpp:3658
uint32 SSceneGetAllSceneActorsSNO(SScene & tSScene, SNO * snoActors, uint32 uActorsCapacity);  // X/Server/SScene.cpp:1049
void PlayerSendHeroStateToClient(const ACDID idACD);  // X/Server/SPlayer.cpp:2432
void PlayerSendHeroStateSavedDataToClient(const ACDID idACD);  // X/Server/SPlayer.cpp:2474
void PlayerSendHeroStateSkillsToAllOtherClients(const ACDID idACD);  // X/Server/SPlayer.cpp:2493
void PlayerSendHeroStatePlayerFlagsToClient(const ACDID idACD);  // X/Server/SPlayer.cpp:2514
void PlayerSendHeroStateWaypointsToClient(const ACDID idACD);  // X/Server/SPlayer.cpp:2533
void PlayerSendHeroStateLastSavePointToClient(const ACDID idACD);  // X/Server/SPlayer.cpp:2552
void PlayerSendHeroStateLastWaypointLevelAreaToAllClients(const ACDID idACD);  // X/Server/SPlayer.cpp:2572
void PlayerSendHeroStateCurrencyToClient(const ACDID idACD, BOOL bOnlySendDirtyCurrencies);  // X/Server/SPlayer.cpp:2591

// X/Common/Networking/GameMessage.h. Message types and layouts this file sends that are not in
// src's GameMessage.h yet.
































































































































enum ACDEnterKnownFlags {
    EKF_FORCE_INVENTORY_BIT = 0,
    EKF_DOESNT_WALK_BIT = 1,
    EKF_IS_HIDDEN_BIT = 2,
    EKF_SET_GROUND_TIME = 3,
    EKF_NO_COLLISION = 4,
    EKF_NEW_INVENTORY_ITEM_BIT = 5,
    EKF_FORCE_REMOTE_FX = 6,
    EKF_COUNT = 7,
};















































// X/Common/Console/Avenger.h












void SSceneSetKnownByPlayer(SScene & tSScene, const Player & tPlayer);  // X/Server/SScene.cpp:1027
void ServerMessageSendTeamToPlayer(const PlayerList & listDestPlayers, const Team eTeam);  // X/Common/TeamCommon.cpp:462
void SPlayerAddDestinationToPrefetch(ACDID idACDPlayer, ACDID idACDGizmo);  // X/Server/SPlayer.cpp:13258
void SPlayerRefreshWaypointDestination(ACDID idACDPlayer);  // X/Server/SPlayer.cpp:13243
void ServerMessageSetPlayerActorInitial(const PlayerList & listDestPlayers, const int32 nPlayerIndex, const ActorCommonData & tACDPlayer);  // X/Server/Actors/Actors.cpp:4840
void SComplexEffectSendAllSources(const PlayerList & tListPlayers, const ACDID idACD);  // X/Server/Actors/SComplexEffect.cpp:126
int32 ItemGetNumAffixedSockets(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:5768
int32 ItemGetNumPredeterminedSockets(GBID gbidItem);  // X/Common/Items.cpp:5745
void NPCUpdateConversationIconAndInteraction(ActorCommonData & tACDNPC, const Player & tPlayer);  // X/Server/SNPC.cpp:438
void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eItemEffectType, int32 & nEffectLevel, const BOOL bWantOverrides, const BOOL bForTooltip);  // X/Common/VisualInventory.cpp:877

void ServerMessageSendMapRevealSceneToPlayer(const PlayerList & listDestPlayers, const SSceneID idSScene, BOOL bForceReveal);
void SendPlayerWarped(const PlayerList & listDestPlayers, const PlayerWarpReason eReason, const float flWarpFadeInSeconds);

//@ 71
static void sServerMessageSendPlayerEnterKnown(const PlayerList & listDestPlayers, const Player & tPlayer)


{
    PlayerEnterKnownMessage tMessage;
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    tMessage.ann = ANNFromACD(tPlayer.idPlayerACD);

    MessageSendToClient(listDestPlayers, SMSG_PLAYER_ENTER_KNOWN, &tMessage, sizeof(tMessage));
}

//@ 84
void ServerMessageSendAddPet(const PlayerList & listDestPlayers, const ACDID idACDPet)


{

    ActorCommonData & tACDPet = ACDGet(idACDPet);

    Player * ptOwner = tACDPet.GetPetOwnerPlayer();
    if (ptOwner == NULL)
    {
        return;
    }

    ActorCommonData * ptACDOwner = ACDTryToGet(ptOwner->idPlayerACD);
    if (ptACDOwner == NULL)
    {
        return;
    }

    ActorID idActorPet = (ActorID)ACDGetOwner(tACDPet.id);





    Actor * ptActorPet = ActorTryToGet(idActorPet);
    if (ptActorPet == NULL || ActorFlaggedForDeletion(idActorPet))
    {
        return;
    }


    int32 nIndex = PlayerGetPetIndex(*ptOwner, idACDPet);
    if (nIndex == -1)
    {
        return;
    }






    PetMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nOwner = ptOwner->nPlayerIndex;
    tMessage.annPet = ANNFromACD(idACDPet);
    tMessage.nType = PlayerGetPetType(ptOwner->idPlayerACD, nIndex);
    tMessage.nIndex = nIndex;

    MessageSendToClient(listDestPlayers, SMSG_ATTACH_PET, &tMessage, sizeof(tMessage));
}

//@ 142
static BOOL sIsOpenGizmo(const ACDID idACD)
{

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD && ptACD->IsGizmo())
    {
        GizmoType nGizmoType = ptACD->GetGizmoType();
        switch (nGizmoType)
        {
        case ACTORGIZMO_SWITCH:
            return GizmoSwitchIsOn(*ptACD);

        case ACTORGIZMO_DOOR:
            return GizmoDoorIsOpen(*ptACD);

        case ACTORGIZMO_CHEST:
            return GizmoChestIsOpen(*ptACD);

        case ACTORGIZMO_POWERUP:
            return GizmoPowerUpIsInactive(*ptACD);

        case ACTORGIZMO_HEALING_WELL:
        case ACTORGIZMO_POOL_OF_REFLECTION:
            return GizmoHealingWellIsEmpty(*ptACD);

        case ACTORGIZMO_OBJECTIVE_FINDER:
            return GizmoObjectiveFinderIsUsed(*ptACD);
        }
    }

    return FALSE;
}

//@ 182
static void sSendAvengerInformation(const ActorCommonData & tACD, const PlayerList & listDestPlayers)


{
    if (tACD.IsAvenger())
    {
        AvengerInfoMessage tMessage;
        tMessage.annTarget = ANNFromACD(tACD.id);
        tMessage.nPlayerIndexOfAvengerTarget = tACD.ptAvengerData->nPlayerIndex;
        Avenger::Console::VictimsCopyArrayToMessage(tACD.ptAvengerData->arVictims, tMessage);
        tMessage.eAvengerSource = tACD.ptAvengerData->eAvengerSource;
        tMessage.nAffixBucket = tACD.ptAvengerData->nAffixBucket;
        tMessage.bRetired = FALSE;
        Blizzard::String::Copy(tMessage.szName, tACD.ptAvengerData->sName.str(), sizeof(tMessage.szName));
        MessageSendToClient(listDestPlayers, SMSG_AVENGER_ENTER_KNOWN_LIST, &tMessage, sizeof(tMessage));
    }
}

//@ 204
static void sSendVisualInventoryForHirelingClass(const ActorCommonData & tACD, HirelingClass eClass, const Player & tThisPlayer, const PlayerList & listDestPlayers)




{

    if (!HirelingTestValid(tThisPlayer.idPlayerACD, eClass))
    {
        return;
    }

    const HirelingData * ptData = &tThisPlayer.tHirelingStable.tHireling[eClass];

    VisualInventoryMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.tEquipment.Init();


    for (int32 i = 0; i < NUM_HIRELING_SLOTS; i++)
    {
        InventorySlotType eSlot = (InventorySlotType)(SLOT_HIRELING_START + i);
        VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eSlot);
        if (eVisualSlot != -1)
        {
            ACDID idACDItem = ptData->tItems.idACDItems[i];
            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            if (ptACDItem)
            {
                VisualItem & tVisualItem = tMessage.tEquipment.tVisualItem[eVisualSlot];
                tVisualItem.eDyeType = ptACDItem->GetDyeType();
                tVisualItem.gbidItem = ptACDItem->hGB.gbid;
                ACDGetItemEffectTypeAndLevel(*ptACDItem, tVisualItem.eItemEffectType, tVisualItem.nEffectLevel, TRUE, FALSE);
            }
        }
    }




    MessageSendToClient(listDestPlayers, SMSG_VISUAL_INVENTORY, &tMessage, sizeof(tMessage));
}

//@ 253
static void sSendACDEnterKnown(const PlayerList & listDestPlayers, ActorCommonData & tACD, const Player & tThisPlayer, ACDIDListPooled & listACDsWithComplexEffects)




{
    ACDEnterKnownMessage tMessage;
    tMessage.ann = ANNFromACD(tACD.id);
    tMessage.snoActor = tACD.snoActor;
    tMessage.dwFlags = 0;
    tMessage.eLocationType = tACD.tLocation.eActorLocation;
    switch (tMessage.eLocationType)
    {
    case AL_WORLD:
        {
            WorldLocationMessageData tWorldLocationData;
            tWorldLocationData.flScale = tACD.tLocation.tWorldLocation.flScale;
            tWorldLocationData.idSWorld = tACD.tLocation.tWorldLocation.idSWorld;
            tWorldLocationData.transform = tACD.tLocation.tWorldLocation.transform;

            tMessage.tWorldLocationData.Set(tWorldLocationData);
        }
        break;

    case AL_INVENTORY:
        {
            InventoryLocationMessageData tInvLocationData;
            tInvLocationData.eSlot = tACD.tLocation.tInvLocation.eSlot;
            tInvLocationData.pt = tACD.tLocation.tInvLocation.pt;
            tInvLocationData.annContainer = ANNFromACD(tACD.tLocation.tInvLocation.idACDContainer);
            tMessage.tInvLocationData.Set(tInvLocationData);

            ACDID idACDOwner = tACD.GetInventoryContainerOwner();



            Player * ptOwner = PlayerGetByACD(idACDOwner);
            if (ptOwner && PlayersAreLocalToEachOther(*ptOwner, tThisPlayer))
            {

                ACDID idACDOldOwner = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_KNOWN_BY_OWNER));
                if (idACDOwner != idACDOldOwner)
                {
                    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_KNOWN_BY_OWNER), idACDOwner);
                    tMessage.dwFlags |= (1 << EKF_NEW_INVENTORY_ITEM_BIT);
                }
            }
        }
        break;
    }

    tMessage.hGB = tACD.hGB;
    tMessage.tSNOName = tACD.tSNOName;

    if (tACD.IsItem())
    {
        tMessage.nRarity = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_ITEM_QUALITY_LEVEL));
    }
    else
    {
        tMessage.nRarity = tACD.eMonsterRarity;
    }
    tMessage.nLookLinkIndex = tACD.nLookLinkIndex;


    if (tACD.IsItem() &&
        (ItemHasSockets(tACD) || ItemGetNumAffixedSockets(tACD) > 0 ||
         ItemGetNumPredeterminedSockets(tACD.hGB.gbid) > 0))
    {
        tMessage.dwFlags |= (1 << EKF_FORCE_INVENTORY_BIT);
    }


    if (tACD.HasInventory())
    {
        tMessage.dwFlags |= (1 << EKF_FORCE_INVENTORY_BIT);
    }

    if (!tACD.GetActorMovement())
    {
        tMessage.dwFlags |= (1 << EKF_DOESNT_WALK_BIT);
    }

    if (GameGetServerTime() == tACD.dwHitGroundTime)
    {
        tMessage.dwFlags |= (1 << EKF_SET_GROUND_TIME);
    }

    if (!tACD.ptCollSphereNode)
    {
        tMessage.dwFlags |= (1 << EKF_NO_COLLISION);
    }

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCE_REMOTE_FX)))
    {
        tMessage.dwFlags |= (1 << EKF_FORCE_REMOTE_FX);
    }


    {
        ActorID idActor = (ActorID)tACD.idOwner;
        Actor * ptActor = ActorGet(idActor);


        if (ptActor->tMarkerOwner.snoMarkerSet != SNO_NONE)
        {
            tMessage.snoMarkerSet.Set(ptActor->tMarkerOwner.snoMarkerSet);
            tMessage.nMarkerIndex.Set(ptActor->tMarkerOwner.nMarkerIndex);


            ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptActor->tMarkerOwner.snoMarkerSet);
            SNO snoAmbientOcclusionOverrideTex = TagMapGetSNO(pdefMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex].hTagMap, TAG_MARKER_AO_OVERRIDE);

            if (snoAmbientOcclusionOverrideTex != SNO_NONE)
            {
                tMessage.snoAmbientOcclusionOverrideTex = snoAmbientOcclusionOverrideTex;
            }
        }
    }

    {
        ActorID idActor = (ActorID)tACD.idOwner;
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {
            ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
            if (pdefActor->dwFlags & (1 << ACTORDEF_SENDS_CREATION_TIME_BIT))
            {
                tMessage.dwTimeActorCreated = ptActor->dwTimeCreated;
            }
        }
    }

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_HAS_LOOK_OVERRIDE)))
    {
        int32 nLookIndex = 0;

        EnterKnownLookOverrides tLookOverrides;
        for (int32 x = 0; x < 6; x++)
        {
            tLookOverrides.aLookOverrides[x] = 0;
        }

        POS pos = tACD.listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = tACD.listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (!ptBuff || ptBuff->bDeleted || !FastAttribGetValueInt(ptBuff->idFastAttrib, FastAttribKey(ATTRIBUTE_HAS_LOOK_OVERRIDE)))
            {
                continue;
            }

            tLookOverrides.aLookOverrides[nLookIndex] = ptBuff->nLookOverrideHash;

            nLookIndex++;
            if (nLookIndex >= 6)
            {
                break;
            }
        }

        tMessage.tEnterKnownLookOverrides.Set(tLookOverrides);
    }

    MessageSendToClient(listDestPlayers, SMSG_ACD_ENTER_KNOWN, &tMessage, sizeof(tMessage));

    tACD.SendShear(listDestPlayers);

    tACD.SendAffix(listDestPlayers, 1);
    tACD.SendAffix(listDestPlayers, 2);


    if (tACD.IsItem())
    {
        if (!ItemIsUnidentified(tACD))
        {
            ItemQualityLevel eIQL = ItemGetQualityLevel(tACD);
            if (eIQL >= IQL_RARE_AFFIX_START && eIQL <= IQL_RARE_AFFIX_END)
            {
                tACD.SendRareItemName(listDestPlayers);
            }
        }

        if (ItemIsGift(tACD.hGB.gbid))
        {
            tACD.SendFriendRecipientInfo(listDestPlayers);
        }
    }
    else
    {
        switch (tACD.eMonsterRarity)
        {
        case MR_CHAMPION:
        case MR_RARE:
        case MR_RARE_MINION:
        case MR_UNIQUE:
        case MR_BOSS:
            tACD.SendRareMonsterName(listDestPlayers);
            break;
        }
    }

    if (tACD.ptResolvedDest)
    {
        tACD.SendPortalSpecifier(listDestPlayers);
    }

    if (tACD.IsNPC())
    {
        NPCUpdateConversationIconAndInteraction(tACD, tThisPlayer);
    }


    const Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer)
    {

        sServerMessageSendPlayerEnterKnown(listDestPlayers, *ptPlayer);

        tACD.SendVisualInventory(listDestPlayers);
    }

    tACD.SendCollision(listDestPlayers);
    tACD.SendAllAttributes(listDestPlayers);
    tACD.SendGroupInfo(listDestPlayers);


    {
        ANNDataMessage tANNMessage;
        tANNMessage.ann = tMessage.ann;
        MessageSendToClient(listDestPlayers, SMSG_UI_ENTER_KNOWN, &tANNMessage, sizeof(tANNMessage));
    }


    BOOL fPlayDeadRecreate = FALSE;
    BOOL fIsOpenGizmo = sIsOpenGizmo(tACD.id);
    BOOL fIsDeadPlayer = ptPlayer && !tACD.IsAlive();
    if (fIsOpenGizmo || fIsDeadPlayer)
    {






        AnimPlayerSendIdleAnimation(tACD, listDestPlayers);
        fPlayDeadRecreate = TRUE;
    }

    ActorID idActor = (ActorID)ACDGetOwner(tACD.id);
    Actor * ptActor = ActorGet(idActor);



    if (ptActor->ptSActorMovement)
    {
        ptActor->ptSActorMovement->ResetCache();
        ptActor->ptSActorMovement->SendMoveMessage(listDestPlayers);
    }




    if (ActorHasBrain(idActor))
    {
        EStanceType eStanceType = BrainGetStanceType(idActor);
        if (eStanceType == STANCE_DEAD)
        {
            fPlayDeadRecreate = TRUE;
        }
    }


    if (fPlayDeadRecreate)
    {
        ACDPlayEffectForPlayers(listDestPlayers, tACD.id, EFFECT_RECREATE_DEAD, 0);
    }

    int32 nDamageStateMax = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_MAX));
    if (nDamageStateMax > 0)
    {
        int32 nDamageStateCurr = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DAMAGE_STATE_CURRENT));
        if (nDamageStateCurr > 0)
        {
            ACDPlayEffectForPlayers(listDestPlayers, tACD.id, EFFECT_RECREATE_DAMAGE_STATE, nDamageStateCurr);
        }
    }

    ScopedActorDefinitionLockR pdefActor(tACD.snoActor);

    if (pdefActor->eType == ACTORTYPE_PROJECTILE)
    {
        SNO snoEffect = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PROJECTILE_EFFECT_SNO));
        if (snoEffect != SNO_NONE)
        {
            ACDPlayEffectForPlayers(listDestPlayers, tACD.id, EFFECT_PROJECTILE_EFFECT, snoEffect);
        }
    }


    if (tACD.IsItem() && !ItemIsUnidentified(tACD))
    {


        const ActorLocation & tActorLocation = tACD.GetLocation();
        if (tActorLocation.eActorLocation != AL_WORLD)
        {
            ACDPlayEffectForPlayers(listDestPlayers, tACD.id, EFFECT_ITEM_ELEMENTAL_EFFECT, 0);
        }
    }



    if (!fIsOpenGizmo && !fIsDeadPlayer)
    {
        AnimPlayerSendAnimations(tACD, listDestPlayers);
    }


    if (tACD.IsPet())
    {

        ServerMessageSendAddPet(listDestPlayers, tACD.id);
        tACD.SendVisualInventory(listDestPlayers);
    }


    if (tACD.IsNPC() && !tACD.IsMerchant() && tACD.HasInventory())
    {
        tACD.SendVisualInventory(listDestPlayers);
    }



    HirelingClass eHirelingClass = ACDGetHirelingClass(tACD);
    if (eHirelingClass != HC_NONE && !tACD.IsPet())
    {
        sSendVisualInventoryForHirelingClass(tACD, eHirelingClass, tThisPlayer, listDestPlayers);
    }



    if (tACD.IsAvenger())
    {
        sSendAvengerInformation(tACD, listDestPlayers);
    }








    if (tACD.HasComplexEffects())
    {
        listACDsWithComplexEffects.AddTail_Tracked(tACD.id);
    }

}

//@ 631
static void sServerMessageSendWorldEnterToPlayer(const PlayerList & listDestPlayers, const SWorldID idSWorld)


{

    const Player & tPlayer = *listDestPlayers.GetHead();

    SWorld * ptSWorld = SWorldGet(idSWorld);


    ActorCommonData & tACD = ACDGet(tPlayer.idPlayerACD);
    EnterWorldMessage tMessage;
    tMessage.idSWorld = ptSWorld->tWCD.idSWorld;
    tMessage.wpLocation = tACD.GetPlace().wp;
    tMessage.snoWorld = ptSWorld->tWCD.snoWorld;
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;

    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_HAS_LOOK_OVERRIDE)))
    {
        int32 nLookIndex = 0;

        EnterKnownLookOverrides tLookOverrides;
        for (int32 x = 0; x < 6; x++)
        {
            tLookOverrides.aLookOverrides[x] = 0;
        }

        POS pos = tACD.listBuffs.GetHeadPosition();
        while (pos)
        {
            BuffID idBuff = tACD.listBuffs.GetNext(pos);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (!ptBuff || ptBuff->bDeleted || !FastAttribGetValueInt(ptBuff->idFastAttrib, FastAttribKey(ATTRIBUTE_HAS_LOOK_OVERRIDE)))
            {
                continue;
            }

            tLookOverrides.aLookOverrides[nLookIndex] = ptBuff->nLookOverrideHash;

            nLookIndex++;
            if (nLookIndex >= 6)
            {
                break;
            }
        }

        tMessage.tEnterKnownLookOverrides.Set(tLookOverrides);
    }

    MessageSendToClient(listDestPlayers, SMSG_ENTER_WORLD, &tMessage, sizeof(tMessage));




    if (tACD.IsAlive())
    {
        AnimPlayerSendAnimations(tACD, listDestPlayers);
    }
}

//@ 694
void ServerMessageSendWorldRevealToPlayer(const PlayerList & listDestPlayers, const SWorldID idSWorld)


{

    SWorld * ptSWorld = SWorldGet(idSWorld);

    RevealWorldMessage tMessage;
    tMessage.idSWorld = idSWorld;
    tMessage.snoWorld = ptSWorld->tWCD.snoWorld;
    tMessage.snoDungeonFinderSourceWorld = ptSWorld->tWCD.snoDungeonFinderSourceWorld;
    tMessage.nOriginX = ptSWorld->tWCD.pointWorldOrigin.x;
    tMessage.nOriginY = ptSWorld->tWCD.pointWorldOrigin.y;




    ScopedWorldDefinitionLockR pdefWorld(tMessage.snoWorld);
    WorldCacheSceneValues(pdefWorld);


    if (pdefWorld->bDRLG)
    {
        DRLGGetDungeonStitchSize(
            ptSWorld->tWCD,
            tMessage.nStitchSizeInFeetX,
            tMessage.nStitchSizeInFeetY);

        tMessage.nWorldSizeInFeetX = 0;
        tMessage.nWorldSizeInFeetY = 0;
    }
    else
    {
        DRLGGetWorldSize(
            ptSWorld->tWCD.snoWorld,
            tMessage.nStitchSizeInFeetX,
            tMessage.nStitchSizeInFeetY,
            tMessage.nWorldSizeInFeetX,
            tMessage.nWorldSizeInFeetY);
    }

    MessageSendToClient(listDestPlayers, SMSG_REVEAL_WORLD, &tMessage, sizeof(tMessage));

    ptSWorld->SendStatus(listDestPlayers);

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        SWorldSyncData(idSWorld, ptPlayer);
    }
}

//@ 748
void ServerMessageSendSceneToPlayer(const PlayerList & listDestPlayers, const SSceneID idSScene)


{
    SScene * ptSScene = SSceneGet(idSScene);

    RevealSceneMessage tMessage;


    tMessage.idSScene = ptSScene->tSCD.idSScene;
    tMessage.tSceneSpec = ptSScene->tSCD.tSceneSpec;
    tMessage.idSWorld = ptSScene->tSCD.idSWorld;
    tMessage.transform = ptSScene->tSCD.transformWorld;
    tMessage.snoScene = ptSScene->tSCD.snoScene;
    tMessage.idSSceneParent = ptSScene->tSCD.idSSceneParent;
    tMessage.snoSceneGroup = ptSScene->tSCD.snoSceneGroup;
    tMessage.bIsVista = ptSScene->bIsVista;
    SceneCommonData & tSCD = SceneCommonDataGet(idSScene);
    tMessage.uLabelCount = 0;
    POS pos = tSCD.listAppliedLabels.GetHeadPosition();
    while (pos && tMessage.uLabelCount < 256)
    {
        tMessage.arAppliedLabels[tMessage.uLabelCount] = tSCD.listAppliedLabels.GetNext(pos);
        tMessage.uLabelCount++;
    }

    tMessage.uActorCount = SSceneGetAllSceneActorsSNO(*ptSScene, tMessage.snoActors, 256);

    MessageSendToClient(listDestPlayers, SMSG_REVEAL_SCENE, &tMessage, sizeof(tMessage));


    PlayerList listAllPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listAllPlayers, -1, FALSE, -1);
    ServerMessageSendMapRevealSceneToPlayer(listAllPlayers, idSScene, FALSE);
}

//@ 786
void ServerMessageSendPrefetchWorldToPlayer(const PlayerList & listDestPlayers, const SNO snoWorld)


{
    PrefetchDataMessage tMessage;
    tMessage.sno = snoWorld;
    MessageSendToClient(listDestPlayers, SMSG_PREFETCH_WORLD, &tMessage, sizeof(tMessage));
}

//@ 797
void ServerMessageSendPrefetchSceneToPlayer(const PlayerList & listDestPlayers, const SNO snoScene)


{
    PrefetchDataMessage tMessage;
    tMessage.sno = snoScene;
    MessageSendToClient(listDestPlayers, SMSG_PREFETCH_SCENE, &tMessage, sizeof(tMessage));
}

//@ 808
void ServerMessageSendPrefetchLevelAreaToPlayer(const PlayerList & listDestPlayers, const SNO snoLevelArea)


{
    PrefetchDataMessage tMessage;
    tMessage.sno = snoLevelArea;
    MessageSendToClient(listDestPlayers, SMSG_PREFETCH_LEVELAREA, &tMessage, sizeof(tMessage));
}

//@ 819
void ServerMessageSendPreloadAddSceneToPlayer(const PlayerList & listDestPlayers, const SSceneID idSScene, const SNO snoScene, const SNO * ptSNOLevelAreas)




{
    PreloadSceneDataMessage tMessage;
    tMessage.idSScene = idSScene;
    tMessage.snoScene = snoScene;
    for (int32 i = 0; i < 4; i++)
    {
        tMessage.snoLevelAreas[i] = ptSNOLevelAreas[i];
    }
    MessageSendToClient(listDestPlayers, SMSG_PRELOAD_ADD_SCENE, &tMessage, sizeof(tMessage));
}

//@ 837
void ServerMessageSendPreloadAddACDToPlayer(const PlayerList & listDestPlayers, const ACDID idACD, const SNO snoActor, const WeaponClass eWeaponClass, const GBID * pgbidMonsterAffixes)





{
    PreloadACDDataMessage tMessage;
    tMessage.ann = ANNFromACD(idACD);
    tMessage.snoActor = snoActor;
    tMessage.eWeaponClass = eWeaponClass;
    for (int32 i = 0; i < 8; i++)
    {
        tMessage.gbidMonsterAffixes[i] = pgbidMonsterAffixes[i];
    }
    MessageSendToClient(listDestPlayers, SMSG_PRELOAD_ADD_ACD, &tMessage, sizeof(tMessage));
}

//@ 857
void ServerMessageSendPreloadRemoveSceneForPlayer(const PlayerList & listDestPlayers, const SSceneID idSScene)


{
    PreloadSceneDataMessage tMessage;
    tMessage.idSScene = idSScene;


    MessageSendToClient(listDestPlayers, SMSG_PRELOAD_REMOVE_SCENE, &tMessage, sizeof(tMessage));
}

//@ 870
void ServerMessageSendPreloadRemoveACDForPlayer(const PlayerList & listDestPlayers, const ACDID idACD)


{
    PreloadACDDataMessage tMessage;
    tMessage.ann = ANNFromACD(idACD);
    tMessage.snoActor = SNO_NONE;
    MessageSendToClient(listDestPlayers, SMSG_PRELOAD_REMOVE_ACD, &tMessage, sizeof(tMessage));
}

//@ 883
void ServerMessageSendMapRevealSceneToPlayer(const PlayerList & listDestPlayers, const SSceneID idSScene, BOOL bForceReveal)



{

    SScene * ptSScene = SSceneGet(idSScene);
    MapRevealSceneMessage tMessage;
    ScopedSceneDefinitionLockR pdefScene(ptSScene->tSCD.snoScene);
    tMessage.bRevealed = (pdefScene->dwFlags >> 3) & 1;










    tMessage.idSScene = idSScene;
    tMessage.idSWorld = ptSScene->tSCD.idSWorld;
    tMessage.snoScene = ptSScene->tSCD.snoScene;
    tMessage.transform = ptSScene->tSCD.transformWorld;

    MessageSendToClient(listDestPlayers, SMSG_MAP_REVEAL_SCENE, &tMessage, sizeof(tMessage));
}

//@ 913
void ServerMessageDestroySceneForPlayer(const PlayerList & listDestPlayers, const SSceneID idSScene)


{
    SScene * ptSScene = SSceneTryToGet(idSScene);




    DestroySceneMessage tMessage;
    tMessage.idSWorld = ptSScene->tSCD.idSWorld;
    tMessage.idSScene = ptSScene->tSCD.idSScene;

    MessageSendToClient(listDestPlayers, SMSG_DESTROY_SCENE, &tMessage, sizeof(tMessage));
}

//@ 930
void ServerMessageSendSceneSwapToPlayer(const PlayerList & listDestPlayers, const SSceneID idSSceneOld, const SSceneID idSSceneNew)



{
    SScene * ptSScene = SSceneGet(idSSceneNew);



    SwapSceneMessage tMessage;
    tMessage.idSWorld = ptSScene->tSCD.idSWorld;
    tMessage.idSSceneOld = idSSceneOld;
    tMessage.idSSceneNew = idSSceneNew;


    MessageSendToClient(listDestPlayers, SMSG_SWAP_SCENE, &tMessage, sizeof(tMessage));
}

//@ 949
void ServerMessageSendEnterMessages(const PlayerList & listDestPlayers, const KnownEntryListID idEnterList, const Player & tPlayer)



{
    ACDIDListPooled listACDsWithComplexEffects(MemoryPoolsGetDATAIDListPool());

    POS pos = KnownEntryListGetFirst(idEnterList);
    while (pos)
    {
        const KnownEntry tKnownEntry = KnownEntryListGetNext(idEnterList, pos);

        switch (tKnownEntry.eType)
        {
        case KET_ACD:
            {

                ACDID idACD = (ACDID)tKnownEntry.id;

                ActorCommonData & tACD = ACDGet(idACD);




                tACD.SetKnownByPlayer(tPlayer, TRUE);


                sSendACDEnterKnown(listDestPlayers, tACD, tPlayer, listACDsWithComplexEffects);




                if (tACD.id == tPlayer.idPlayerACD)
                {
                    ServerMessageSetPlayerActorInitial(listDestPlayers, tPlayer.nPlayerIndex, tACD);
                }
            }
            break;


        case KET_WORLD_ENTER:
            {
                sServerMessageSendWorldEnterToPlayer(listDestPlayers, (SWorldID)tKnownEntry.id);
            }
            break;


        case KET_WORLD_REVEAL:
            {
                ServerMessageSendWorldRevealToPlayer(listDestPlayers, (SWorldID)tKnownEntry.id);
            }
            break;


        case KET_SCENE:
        case KET_SUBSCENE:
            {
                SSceneID idSScene = (SSceneID)tKnownEntry.id;
                SScene * ptSScene = SSceneGet(idSScene);
                SSceneSetKnownByPlayer(*ptSScene, tPlayer);

                ServerMessageSendSceneToPlayer(listDestPlayers, idSScene);
            }
            break;


        case KET_TEAM:
            {
                ServerMessageSendTeamToPlayer(listDestPlayers, (Team)tKnownEntry.id);
            }
            break;


        case KET_PREFETCH_SCENE:
            {
                SScene * ptSScene = SSceneGet((SSceneID)tKnownEntry.id);

                SNO snoLevelAreas[4];
                for (int32 i = 0; i < 4; i++)
                {
                    snoLevelAreas[i] = LevelAreaGetSNO(ptSScene->arLevelAreaIds[i]);
                }
                ServerMessageSendPreloadAddSceneToPlayer(listDestPlayers, (SSceneID)tKnownEntry.id, ptSScene->tSCD.snoScene, snoLevelAreas);


                SNOPreload::PrefetchServerScene(ptSScene->tSCD.snoScene, snoLevelAreas);
            }
            break;




        case KET_PREFETCH_ACD:
            {
                ActorCommonData & tACD = ACDGet((ACDID)tKnownEntry.id);



                if (tACD.IsGizmo())
                {
                    GizmoType eGizmoType = tACD.GetGizmoType();
                    if (GizmoIsPortal(eGizmoType) || eGizmoType == ACTORGIZMO_HEARTH_PORTAL)
                    {
                        SPlayerAddDestinationToPrefetch(tPlayer.idPlayerACD, (ACDID)tKnownEntry.id);
                    }
                    else if (eGizmoType == ACTORGIZMO_WAYPOINT)
                    {
                        SPlayerRefreshWaypointDestination(tPlayer.idPlayerACD);
                    }
                }



                Actor * ptActor = ActorTryToGet((ActorID)tACD.idOwner);

                ServerMessageSendPreloadAddACDToPlayer(listDestPlayers, (ACDID)tKnownEntry.id, tACD.snoActor, WEAPONCLASS_NONE, tACD.pMonsterAffixGBIDs);
            }
            break;






        default:
            DisplayInternalError(NULL, "", 1071, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            break;
        }
    }




    POS posACD = listACDsWithComplexEffects.GetHeadPosition();
    while (posACD)
    {
        const ACDID idACD = (ACDID)listACDsWithComplexEffects.GetNext(posACD);

        SComplexEffectSendAllSources(listDestPlayers, idACD);
    }
}

//@ 1090
static void sServerMessageSendACDExitMessages(const PlayerList & listDestPlayers, const ACDID idACD)






{
    ANNDataMessage tMessage;
    tMessage.ann = ANNFromACD(idACD);

    MessageSendToClient(listDestPlayers, SMSG_ACD_EXIT_KNOWN, &tMessage, sizeof(tMessage));
}

//@ 1106
void ServerMessageSendExitMessages(const PlayerList & listDestPlayers, const KnownEntryListID idExitList, const Player & tPlayer)



{
    POS pos = KnownEntryListGetFirst(idExitList);
    while (pos)
    {
        const KnownEntry tKnownEntry = KnownEntryListGetNext(idExitList, pos);

        switch (tKnownEntry.eType)
        {
        case KET_ACD:
            {



                ActorCommonData * ptACD = ACDTryToGet((ACDID)tKnownEntry.id);
                if (ptACD)
                {
                    ptACD->SetKnownByPlayer(tPlayer, FALSE);
                }


                sServerMessageSendACDExitMessages(listDestPlayers, (ACDID)tKnownEntry.id);
            }
            break;








        case KET_SCENE:
        case KET_SUBSCENE:
            ServerMessageDestroySceneForPlayer(listDestPlayers, (SSceneID)tKnownEntry.id);
            break;

        case KET_TEAM:
        case KET_WORLD_REVEAL:
        case KET_WORLD_ENTER:
            break;

























        case KET_PREFETCH_SCENE:
            ServerMessageSendPreloadRemoveSceneForPlayer(listDestPlayers, (SSceneID)tKnownEntry.id);
            break;





        case KET_PREFETCH_ACD:
            ServerMessageSendPreloadRemoveACDForPlayer(listDestPlayers, (ACDID)tKnownEntry.id);
            break;





        default:
            DisplayInternalError(NULL, "", 1186, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            break;
        }
    }
}

//@ 1195
void ServerMessageSendPlayerState(const ACDID idPlayerACD)
{

    ActorCommonData & tACD = ACDGet(idPlayerACD);


    if (tACD.dwFlags & (1 << ACD_SEND_HERO_STATE_BIT))
    {

        PlayerSendHeroStateToClient(idPlayerACD);


        tACD.dwFlags &= ~(1 << ACD_SEND_HERO_STATE_BIT);
    }

    if (tACD.dwFlags & (1 << ACD_SEND_HERO_STATE_SAVEDDATA_BIT))
    {
        PlayerSendHeroStateSkillsToAllOtherClients(idPlayerACD);
        PlayerSendHeroStateSavedDataToClient(idPlayerACD);
        tACD.dwFlags &= ~(1 << ACD_SEND_HERO_STATE_SAVEDDATA_BIT);
    }

    if (tACD.dwFlags & (1 << ACD_SEND_HERO_STATE_PLAYERFLAGS_BIT))
    {
        PlayerSendHeroStatePlayerFlagsToClient(idPlayerACD);
        tACD.dwFlags &= ~(1 << ACD_SEND_HERO_STATE_PLAYERFLAGS_BIT);
    }

    if (tACD.dwFlags & (1 << ACD_SEND_HERO_STATE_WAYPOINTS_BIT))
    {
        PlayerSendHeroStateWaypointsToClient(idPlayerACD);
        tACD.dwFlags &= ~(1 << ACD_SEND_HERO_STATE_WAYPOINTS_BIT);
    }

    if (tACD.dwFlags & (1 << ACD_SEND_HERO_STATE_LASTSAVEPOINT_BIT))
    {
        PlayerSendHeroStateLastSavePointToClient(idPlayerACD);
        tACD.dwFlags &= ~(1 << ACD_SEND_HERO_STATE_LASTSAVEPOINT_BIT);
    }

    if (tACD.dwFlags & (1 << ACD_SEND_HERO_STATE_CURRENCY_BIT))
    {
        PlayerSendHeroStateCurrencyToClient(idPlayerACD, TRUE);
        tACD.dwFlags &= ~(1 << ACD_SEND_HERO_STATE_CURRENCY_BIT);
    }

    if (tACD.dwFlags & (1 << ACD_SEND_HERO_STATE_LAST_WAYPOINT_LEVELAREA_BIT))
    {
        PlayerSendHeroStateLastWaypointLevelAreaToAllClients(idPlayerACD);
        tACD.dwFlags &= ~(1 << ACD_SEND_HERO_STATE_LAST_WAYPOINT_LEVELAREA_BIT);
    }
}

//@ 1252
static void sSendKnownEntryUpdateMessages(const PlayerList & listDestPlayers, const KnownEntry & tKnownEntry)


{
    switch (tKnownEntry.eType)
    {
    case KET_ACD:
        {

            ACDGet((ACDID)tKnownEntry.id).SendServerUpdateMessage(listDestPlayers);
        }
        break;

    case KET_SCENE:
    case KET_SUBSCENE:
    case KET_PREFETCH_SCENE:
    case KET_PREFETCH_ACD:
    case KET_TEAM:
    case KET_WORLD_ENTER:
        break;

    case KET_WORLD_REVEAL:
        {
            SWorldGet((SWorldID)tKnownEntry.id)->SendServerUpdateMessage(listDestPlayers);
        }
        break;














    default:
        DisplayInternalError(NULL, "", 1292, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        break;
    }
}

//@ 1315
void ServerMessageSendUpdateMessagesFromKnownEntryList(const PlayerList & listDestPlayers, const KnownEntryListID idUpdateList)


{


    POS pos = KnownEntryListGetFirst(idUpdateList);
    while (pos)
    {
        KnownEntry & tKnownEntry = KnownEntryListGetNext(idUpdateList, pos);
        sSendKnownEntryUpdateMessages(listDestPlayers, tKnownEntry);
    }
}

//@ 1331
void SendPlayerLevelMessage(const ACDID idPlayerACD, Attrib eAttribLevel)


{
    Player * ptPlayer = PlayerGetByACD(idPlayerACD);
    GameMessageType eMessage;
    PlayerLevel tMessage;

    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    tMessage.nNewLevel = ACDGet(idPlayerACD).AttributesGetInt(FastAttribKey(eAttribLevel));

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllConnected(listDestPlayers, -1, FALSE);


    eMessage = (eAttribLevel == ATTRIBUTE_ALT_LEVEL) ? SMSG_PLAYER_ALT_LEVEL : SMSG_PLAYER_LEVEL;




    MessageSendToClient(listDestPlayers, eMessage, &tMessage, sizeof(tMessage));
}

//@ 1357
void SendCurrentActMessage(const Player & tPlayer, const BOOL bOnJoin)


{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);

    ActTransitionMessage tMessage;
    tMessage.eAct = GameGetCurrentAct();
    tMessage.bOnJoin = bOnJoin;
    MessageSendToClient(listDestPlayers, SMSG_ACT, &tMessage, sizeof(tMessage));
}

//@ 1372
void SendInterstitialMessage(const Player & tPlayer)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer);

    InterstitialMessage tMessage;
    tMessage.ePlayerClass = PlayerGetPlayerClass(tPlayer.idPlayerACD);
    tMessage.fIsMale = PlayerIsMale(tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_INTERSTITIAL, &tMessage, sizeof(tMessage));
}

//@ 1403
void SendPlayerCancelWarp(const ACDID idACDPlayer)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);

    SendPlayerWarped(listDestPlayers, 14, 0.0f);
}

//@ 1414
void SendPlayerWarped(const PlayerList & listDestPlayers, const PlayerWarpReason eReason, const float flWarpFadeInSeconds)


{
    PlayerWarpedMessage tMessage;
    tMessage.eWarpReason = eReason;
    tMessage.flWarpFadeInSeconds = flWarpFadeInSeconds;
    MessageSendToClient(listDestPlayers, SMSG_PLAYER_WARPED, &tMessage, sizeof(tMessage));
    POS pos = listDestPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        (void)ptPlayer;
    }
}

//@ 1433
void SendPlayerWarping(const ACDID idACDPlayer)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);

    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_PLAYER_WARPING, &tMessage, sizeof(tMessage));



    Avenger::Console::TryToRemoveAvenger(idACDPlayer);

}

//@ 1450
void ServerMessageSendForceInterfaceClose(const ACDID idACDPlayer)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);

    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_FORCE_NPC_INTERACT_CLOSE, &tMessage, sizeof(tMessage));
}

//@ 1463
void sVictimMessageInit(VictimMessage & tMessage)
{
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nKillerPlayerIndex = -1;
    tMessage.snoKillerActor = SNO_NONE;
}

//@ 1470
void ServerMessageSendVictimMsg(const int32 nPlayerVictimIndex, const ACDID idACDKiller, const SNO snoPowerDmgSource)



{
    PlayerList listDestPlayers(PlayerListGetElementPool());

    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

    VictimMessage tMessage;
    sVictimMessageInit(tMessage);

    tMessage.nPlayerVictimIndex = nPlayerVictimIndex;
    tMessage.snoPowerDmgSource = snoPowerDmgSource;

    ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
    ACDID idACDSummoner = ACDGetSummoner(idACDKiller);

    if (ACDIsPlayer(idACDKiller))
    {
        Player * ptKillerPlayer = PlayerGetByACD(idACDKiller);

        tMessage.nKillerPlayerIndex = ptKillerPlayer->nPlayerIndex;
        tMessage.nKillerLevel = ptKillerPlayer->nCurrentLevel;
        tMessage.eKillerMonsterRarity = 2;
        tMessage.eKillerTeam = ptKillerPlayer->eTeam;
    }
    else if (idACDSummoner != ACDID_INVALID && ACDIsPlayer(idACDSummoner))
    {
        Player * ptKillerPlayer = PlayerGetByACD(idACDSummoner);

        tMessage.nKillerPlayerIndex = ptKillerPlayer->nPlayerIndex;
        tMessage.nKillerLevel = ptKillerPlayer->nCurrentLevel;
        tMessage.eKillerMonsterRarity = 2;
        tMessage.eKillerTeam = ptKillerPlayer->eTeam;
        tMessage.snoKillerActor = ptACDKiller->tSNOName.snoHandle;
    }
    else if (ptACDKiller)
    {
        tMessage.nKillerLevel = ptACDKiller->AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
        tMessage.eKillerTeam = ptACDKiller->GetTeam();
        tMessage.eKillerMonsterRarity = ptACDKiller->eMonsterRarity;




        tMessage.snoKillerActor = ptACDKiller->snoActor;
        SigmaMemoryCopy(tMessage.pKillerRareNameGBIDs, ptACDKiller->pRareNameGBIDs, sizeof(tMessage.pKillerRareNameGBIDs));
    }

    MessageSendToClient(listDestPlayers, SMSG_VICTIM, &tMessage, sizeof(tMessage));
}

//@ 1541
void ServerMessageSendEliteEngagedMsg(const Player & tPlayer, const ActorCommonData & tACDMonster)


{
    if (tPlayer.nPlayerIndex < 0)
    {
        return;
    }
    if (tACDMonster.GetMonsterSNO() == GlobalSNOGet(GLOBALSNO_MONSTER_TERROR_DIABLO))
    {
        return;
    }




    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);


    CombatEngagementMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    tMessage.snoActorMonster = tACDMonster.snoActor;
    tMessage.eMonsterRarity = tACDMonster.eMonsterRarity;
    SigmaMemoryCopy(tMessage.pRareNameGBIDs, tACDMonster.pRareNameGBIDs, sizeof(tMessage.pRareNameGBIDs));

    MessageSendToClient(listDestPlayers, SMSG_MONSTER_ENGAGED, &tMessage, sizeof(tMessage));
}

//@ 1573
void ServerMessageSendEliteKilledMsg(const Player & tPlayer, const ActorCommonData & tACDMonster)


{
    if (tPlayer.nPlayerIndex < 0 || tPlayer.nPlayerIndex >= 4)
    {
        return;
    }




    if (tACDMonster.GetMonsterSNO() == GlobalSNOGet(GLOBALSNO_MONSTER_TERROR_DIABLO))
    {
        return;
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);


    CombatEngagementMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nPlayerIndex = tPlayer.nPlayerIndex;
    tMessage.snoActorMonster = tACDMonster.snoActor;
    tMessage.eMonsterRarity = tACDMonster.eMonsterRarity;
    SigmaMemoryCopy(tMessage.pRareNameGBIDs, tACDMonster.pRareNameGBIDs, sizeof(tMessage.pRareNameGBIDs));

    MessageSendToClient(listDestPlayers, SMSG_ELITE_KILLED, &tMessage, sizeof(tMessage));
}

//@ 1605
void ServerMessageSendTreasureGoblinEscapedMsg(const ActorCommonData & tACDMonster)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

    CombatEngagementMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nPlayerIndex = -1;
    tMessage.snoActorMonster = tACDMonster.snoActor;
    tMessage.eMonsterRarity = tACDMonster.eMonsterRarity;
    SigmaMemoryCopy(tMessage.pRareNameGBIDs, tACDMonster.pRareNameGBIDs, sizeof(tMessage.pRareNameGBIDs));

    MessageSendToClient(listDestPlayers, SMSG_TREASURE_GOBLIN_ESCAPED, &tMessage, sizeof(tMessage));
}

//@ 1624
void ServerMessageSendKillCountMsg(const ACDID idACDPlayer)
{

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }


    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);


    KillCountMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    tMessage.nPlayerIndex = ptPlayer->nPlayerIndex;
    tMessage.nPlayerKills = ptPlayer->nPlayerKills;
    tMessage.nAssists = ptPlayer->nAssists;
    tMessage.nDeaths = ptPlayer->nDeaths;

    MessageSendToClient(listDestPlayers, SMSG_KILLCOUNT, &tMessage, sizeof(tMessage));
}

//@ 1652
void ServerMessageSendAcceptedDeathAck(const ACDID idACDPlayer)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, idACDPlayer);

    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayers, SMSG_ACCEPTED_DEATH_ACK, &tMessage, sizeof(tMessage));
}
