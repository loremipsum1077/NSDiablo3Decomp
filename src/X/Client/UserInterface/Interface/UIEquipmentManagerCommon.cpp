// Original file: X/Client/UserInterface/Interface/UIEquipmentManagerCommon.cpp
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/UserInterface/UIInventory.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "UIGame.h"
#include "UITextInputPrompt.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../../Sigma/RefString.h"
#include "../Controls/UIRActorControl.h"
#include "../../RActors/RActors.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../CPlayer.h"
#include "../../../Common/Items.h"
// Declared here because their headers are not adopted yet.
struct PlayerLoadoutSaveMessage { RequiredMessageHeader tHeader; int32 nLoadoutIndex; CHAR usName[53]; };  /* size 0x44 (2.7.7); X/Common/Networking/GameMessage.h */  /* X/Common/Networking/GameMessage.h */ void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  /* CGameMessage.cpp */ namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }  // UIGame.cpp
BOOL PlayerCanChangeLoadout(const Player & tPlayer, int32 nLoadoutIndex, GameError * peGameError); BOOL PlayerLoadoutSocketablesMatch(GBID gbidItem1, GBID gbidItem2); BOOL PlayerLoadoutItemIsInValidInventorySlot(const ActorCommonData & tACDItem, ACDID idACDPlayer);  /* Player.cpp */ CRefString LocalizedSprintf(LPCSTR szFormat, ...); void PlayerLoadoutGetDesiredSocketables(GBIDArray & arGBIDSocketables, const Player & tPlayer, int32 nLoadoutIndex); void PlayerLoadoutFindSocketables(XPooledDataIDList & listACDIDSocketables, const ActorCommonData & tACDPlayer, const GBIDArray * arGBIDSocketables);  /* Player.cpp */ extern const InventorySlotType cgarPlayerLoadoutSocketableSlotsOrder[9];  /* Player.cpp:173 */ DataIDMemoryPool * MemoryPoolsGetDATAIDListPool();  /* X/Sigma/MemoryPools.cpp */ typedef blz::unordered_map<ACDID, int, blz::hash<ACDID>, blz::equal_to<ACDID>, blz::allocator<blz::pair<const ACDID, int> > > ConsumedSocketablesMap;  // name invented
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL fIsFemale);  /* X/Client/Battlenet/UI/Util/UIUtil.cpp */ PlayerClass PlayerGetPlayerClass(const Player * ptPlayer); BOOL PlayerIsMale(const Player & tPlayer);  /* Player.cpp */ LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext);  // X/Client/Lights.cpp:1104
RActorID UI_CreateHeroUIRActor(const SNO snoActor, const SNO snoIrradianceTex, const VisualEquipment & tVisualEquipment, const VisualInventoryType eVIT, const PlayerClass ePlayerClass, const BOOL fFemale, const PRTransform & transform, const RenderContext eContext, const uint32 dwAnimTag, const float flScale, int32 nUnused);  // X/Client/UserInterface/UI.cpp:3324
typedef int32 AttachmentID; AttachmentID RActorAttachExistingRActor(const RActorID idRActorMain, const RActorID idRActorAttachment, HardpointHash szHardpointName);  /* RActors.cpp:2348 */ void VisualInventoryClearAttachments(RActor * ptRActor); void VisualInventoryApply(RActor * ptRActor, VisualInventory & tVisualInventory, const VisualInventoryType eVisualInventoryType, const PlayerClass ePlayerClass, const BOOL fFemale);  // X/Client/CVisualInventory.cpp
BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);  /* Items.cpp:1883 */ void ACDGetItemEffectTypeAndLevel(const ActorCommonData & tACD, ItemEffectType & eItemEffectType, int32 & nEffectLevel, const BOOL bWantOverrides, const BOOL bForTooltip);  // ActorCommonData.cpp:877
namespace UIEquipmentManager {
bool LoadoutCanFindItem(ACDID idACDItem); void AssignLoadoutItemSocketables(ACDID * arLoadoutEquipmentSockets, int32 & nLoadoutEquipmentSocketsCount, ConsumedSocketablesMap & arConsumedSocketables, const LoadoutItemData & tLoadoutItem, const XPooledDataIDList & listACDIDSocketables); void InitializeForGame(); void DisposeForGame(); int32 GetSelectedLoadout(); void SetSelectedLoadout(int32 nSelectedLoadout);
void EquipLoadoutInternal(const Player * ptPlayer); void SaveLoadoutInternal(BOOL bSuccessful, CRefString sInput, Player * ptPlayer); void RenameLoadoutInternal(BOOL bSuccessful, CRefString sInput, Player * ptPlayer); BOOL LoadoutGetLoadoutEquipError(const Player & tPlayer, int32 nLoadoutIndex, CRefString & sError, BOOL bDetailedError); BOOL LoadoutCanEquipItemInSlot(const Player & tPlayer, int32 nLoadoutIndex, InventorySlotType eSlot); void PromptForLoadoutName(const Player & tPlayer, CRefString sDialogTitle, CRefString sMessage, UITextInputPrompt::DialogCallback pfnCallback);
void ShowErrorMsg(const Player * ptPlayer, CRefString sMessage);  /* UIConsoleEquipmentManager.cpp */ void UpdateLoadoutPreview(const Player & tPlayer, const LoadoutSavedData & tLoadout, UIRActor & tRActor, bool bShowWaypoint);
}
struct EquipmentManagerCommonGlobals {  // size 0x10 (2.7.7: sub_7100242020 allocates 0x10); UIEquipmentManagerCommon.h
    void Initialize();
    ACDID idACDInteractNPC;  // 0x00
    int32 nSelectedLoadout;  // 0x04
    int32 unk_8;  // 0x08 (2.7.7: GetSelectedLoadout returns nSelectedLoadout + unk_8 * 10)
    int32 unk_c;  // 0x0c (2.7.7: sub_71002420a0 stores the clamped selection at 0xc + unk_8 * 4)
};

EquipmentManagerCommonGlobals * cgptEquipmentManagerGlobals = NULL;



void EquipmentManagerCommonGlobals::Initialize()
{

    idACDInteractNPC = ACDID_INVALID; nSelectedLoadout = 0;
}










bool UIEquipmentManager::LoadoutCanFindItem(ACDID idACDItem)

{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem)
    {

        const InventoryLocation * ptInvLoc = ptACDItem->TryToGetInventoryLocation();
        if (ptInvLoc)
        {
            if (ptInvLoc->eSlot >= 1 && ptInvLoc->eSlot <= 13) return true;
            if (ptInvLoc->eSlot == 0 || ptInvLoc->eSlot == SLOT_PLAYER_SHARED_STASH) return true;
        }
    }
    return false;
}







void UIEquipmentManager::AssignLoadoutItemSocketables(ACDID * arLoadoutEquipmentSockets,
    int32 & nLoadoutEquipmentSocketsCount,
    ConsumedSocketablesMap & arConsumedSocketables,
    const LoadoutItemData & tLoadoutItem,
    const XPooledDataIDList & listACDIDSocketables)

{
    if (!LoadoutCanFindItem(ACDFromANN(tLoadoutItem.annItem)))
    {
        return;
    }

    for (int32 nSocket = 0; nSocket < tLoadoutItem.nSocketsCount; nSocket++)
    {
        GBID gbidSocketable = tLoadoutItem.gbidSockets[nSocket];

        POS posSocketables = listACDIDSocketables.GetHeadPosition();
        while (posSocketables)
        {
            ACDID idACDSocketable = (ACDID)listACDIDSocketables.GetNext(posSocketables);

            ActorCommonData * ptACDSocketable = ACDTryToGet(idACDSocketable);
            if (!ptACDSocketable)
            {
                continue;
            }

            int32 nConsumedCount = 0;
            if (arConsumedSocketables.find(idACDSocketable) != arConsumedSocketables.end())
            {
                nConsumedCount = arConsumedSocketables[idACDSocketable];
            }

            int32 nStackCount = Math::Max(1, (int32)ptACDSocketable->GetStackCount());
            if (nConsumedCount >= nStackCount)
            {
                continue;
            }


            if (PlayerLoadoutSocketablesMatch(ptACDSocketable->hGB.gbid, gbidSocketable))
            {
                arLoadoutEquipmentSockets[nLoadoutEquipmentSocketsCount] = ptACDSocketable->id;
                nLoadoutEquipmentSocketsCount++;

                arConsumedSocketables[idACDSocketable] = nConsumedCount + 1;
                break;
            }
        }
    }
}




















































































































































void UIEquipmentManager::InitializeForGame()

{
    cgptEquipmentManagerGlobals = new EquipmentManagerCommonGlobals;
    cgptEquipmentManagerGlobals->Initialize();
}



void UIEquipmentManager::DisposeForGame()

{
    delete cgptEquipmentManagerGlobals;
    cgptEquipmentManagerGlobals = NULL;
}

































int32 UIEquipmentManager::GetSelectedLoadout()

{
    return cgptEquipmentManagerGlobals->nSelectedLoadout + cgptEquipmentManagerGlobals->unk_8 * 10;
}



void UIEquipmentManager::SetSelectedLoadout(int32 nSelectedLoadout)



{
    cgptEquipmentManagerGlobals->nSelectedLoadout = Math::Pin(nSelectedLoadout, 0, 9);




}


void UIEquipmentManager::EquipLoadoutInternal(const Player * ptPlayer)

{
    if (!ptPlayer)
    {
        return;
    }

    int32 nSelectedLoadout = GetSelectedLoadout();
    if (ptPlayer->tLoadouts[nSelectedLoadout].bEmpty)
    {
        return;
    }

    GameError eGameError = -1;
    if (!PlayerCanChangeLoadout(*ptPlayer, nSelectedLoadout, &eGameError))
    {
        UIGame::DisplayError(GameErrorData(eGameError), FALSE, NULL);
        return;
    }

    IntDataMessage tMessage;
    tMessage.nData = nSelectedLoadout;
    MessageSendToServer(CMSG_EQUIP_LOADOUT, &tMessage, sizeof(tMessage), ptPlayer);
}



void UIEquipmentManager::SaveLoadoutInternal(BOOL bSuccessful,
    CRefString sInput,
    Player * ptPlayer)

{
    if (!bSuccessful || !ptPlayer)
    {
        return;
    }

    if (sInput.Length() == 0)
    {
        ShowErrorMsg(ptPlayer, StringListFetch("Hero:EquipmentManager_EmptyNameError"));
        return;
    }
















    int32 nSelectedLoadout = GetSelectedLoadout();

    PlayerLoadoutSaveMessage tMessage;
    tMessage.nLoadoutIndex = nSelectedLoadout;
    Blizzard::String::Copy(tMessage.usName, sInput.str(), sizeof(tMessage.usName));
    MessageSendToServer(CMSG_SAVE_LOADOUT, &tMessage, sizeof(tMessage), ptPlayer);
}





















void UIEquipmentManager::RenameLoadoutInternal(BOOL bSuccessful,
    CRefString sInput,
    Player * ptPlayer)

{
    if (!bSuccessful || !ptPlayer)
    {
        return;
    }

    if (sInput.Length() == 0)
    {
        ShowErrorMsg(ptPlayer, StringListFetch("Hero:EquipmentManager_EmptyNameError"));
        return;
    }
















    int32 nSelectedLoadout = GetSelectedLoadout();
    LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[nSelectedLoadout];
    Blizzard::String::Copy(tLoadout.usName, sInput.str(), sizeof(tLoadout.usName));

    PlayerLoadoutSaveMessage tMessage;
    tMessage.nLoadoutIndex = nSelectedLoadout;
    Blizzard::String::Copy(tMessage.usName, sInput.str(), sizeof(tMessage.usName));
    MessageSendToServer(CMSG_SET_LOADOUT_NAME, &tMessage, sizeof(tMessage), ptPlayer);
}



















































BOOL UIEquipmentManager::LoadoutGetLoadoutEquipError(const Player & tPlayer,
    int32 nLoadoutIndex,
    CRefString & sError,
    BOOL bDetailedError)

{
    sError = "";

    const ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }

    nLoadoutIndex = Math::Pin(nLoadoutIndex, 0, 9);
    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nLoadoutIndex];
    if (tLoadout.bEmpty)
    {
        return FALSE;
    }

    for (int32 nBodySlotIndex = 0; nBodySlotIndex < 13; nBodySlotIndex++)
    {
        ACDNetworkName annItem = tLoadout.tEquipment[nBodySlotIndex].annItem;
        ACDID idACDItem = ACDFromANN(annItem);
        if (!LoadoutCanFindItem(idACDItem))
        {
            if (annItem != ANN_INVALID)
            {
                CRefString sSlotLabel = UIInventory::ItemGetLabelForSlot(nBodySlotIndex + 1, FALSE);
                if (sSlotLabel.Length() > 0)
                {
                    if (sError.Length() == 0)
                    {
                        sError = StringListFetch(bDetailedError ? "Hero:EquipmentManager_WarningMissingItems" : "ConsoleEquipmentManager:WarningMissingItems");
                        if (!bDetailedError)
                        {
                            return TRUE;
                        }
                    }

                    sError += CRefString("\n") + StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sSlotLabel);
                }
            }
        }
        else
        {
            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (!PlayerLoadoutItemIsInValidInventorySlot(tACDItem, tPlayer.idPlayerACD))
            {
                CRefString sSlotLabel = UIInventory::ItemGetLabelForSlot(nBodySlotIndex + 1, FALSE);
                if (sSlotLabel.Length() > 0)
                {
                    if (sError.Length() == 0)
                    {
                        sError = StringListFetch(bDetailedError ? "Hero:EquipmentManager_WarningMissingItems" : "ConsoleEquipmentManager:WarningMissingItems");
                        if (!bDetailedError)
                        {
                            return TRUE;
                        }
                    }


                    sError += CRefString("\n") + StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sSlotLabel);
                }
            }
        }
    }

    if (sError.Length() > 0)
    {
        return TRUE;
    }


    GBIDArray arGBIDSocketables;
    PlayerLoadoutGetDesiredSocketables(arGBIDSocketables, tPlayer, nLoadoutIndex);

    XPooledDataIDList listACDIDSocketables(MemoryPoolsGetDATAIDListPool());
    PlayerLoadoutFindSocketables(listACDIDSocketables, *ptACDPlayer, &arGBIDSocketables);

    ACDID arLoadoutEquipmentSockets[13][3];
    int32 nLoadoutEquipmentSocketsCount[13];
    SigmaMemoryClear(nLoadoutEquipmentSocketsCount, sizeof(nLoadoutEquipmentSocketsCount));

    ConsumedSocketablesMap arConsumedSocketables;
    for (int32 nOrderedSlot = 0; cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] != -1; nOrderedSlot++)
    {
        int32 nEquipmentSlotIndex = cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] - 1;

        const LoadoutItemData & tLoadoutItem = tLoadout.tEquipment[nEquipmentSlotIndex];

        AssignLoadoutItemSocketables(arLoadoutEquipmentSockets[nEquipmentSlotIndex], nLoadoutEquipmentSocketsCount[nEquipmentSlotIndex], arConsumedSocketables, tLoadoutItem, listACDIDSocketables);
    }

    for (int32 nBodySlotIndex = 0; nBodySlotIndex < 13; nBodySlotIndex++)
    {
        if (tLoadout.tEquipment[nBodySlotIndex].nSocketsCount != nLoadoutEquipmentSocketsCount[nBodySlotIndex])
        {
            CRefString sSlotLabel = UIInventory::ItemGetLabelForSlot(nBodySlotIndex + 1, FALSE);
            if (sSlotLabel.Length() > 0)
            {
                if (sError.Length() == 0)
                {
                    sError = StringListFetch(bDetailedError ? "Hero:EquipmentManager_WarningMissingGems" : "ConsoleEquipmentManager:WarningMissingGems");
                    if (!bDetailedError)
                    {
                        return TRUE;
                    }
                }

                sError += CRefString("\n") + StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sSlotLabel);
            }
        }
    }

    return sError.Length() > 0;





}


BOOL UIEquipmentManager::LoadoutCanEquipItemInSlot(const Player & tPlayer,
    int32 nLoadoutIndex,
    InventorySlotType eSlot)

{
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
    {
        return FALSE;
    }


    nLoadoutIndex = Math::Pin(nLoadoutIndex, 0, 9);
    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nLoadoutIndex];
    if (!tLoadout.bEmpty)
    {



    int32 nBodySlotIndex = eSlot - 1;


    ACDNetworkName annItem = tLoadout.tEquipment[nBodySlotIndex].annItem;
    ACDID idACDItem = ACDFromANN(annItem);
    if (!LoadoutCanFindItem(idACDItem))
    {
        if (annItem == ANN_INVALID)
        {
            return TRUE;
        }
    }
    else
    {
        GBIDArray arGBIDSocketables;
        PlayerLoadoutGetDesiredSocketables(arGBIDSocketables, tPlayer, nLoadoutIndex);

        XPooledDataIDList listACDIDSocketables(MemoryPoolsGetDATAIDListPool());
        PlayerLoadoutFindSocketables(listACDIDSocketables, *ptACDPlayer, &arGBIDSocketables);

        ACDID arLoadoutEquipmentSockets[13][3];
        int32 nLoadoutEquipmentSocketsCount[13];
        SigmaMemoryClear(nLoadoutEquipmentSocketsCount, sizeof(nLoadoutEquipmentSocketsCount));

        ConsumedSocketablesMap arConsumedSocketables;
        for (int32 nOrderedSlot = 0; cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] != -1; nOrderedSlot++)
        {
            int32 nEquipmentSlotIndex = cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] - 1;

            const LoadoutItemData & tLoadoutItem = tLoadout.tEquipment[nEquipmentSlotIndex];

            AssignLoadoutItemSocketables(arLoadoutEquipmentSockets[nEquipmentSlotIndex], nLoadoutEquipmentSocketsCount[nEquipmentSlotIndex], arConsumedSocketables, tLoadoutItem, listACDIDSocketables);
        }

        if (tLoadout.tEquipment[nBodySlotIndex].nSocketsCount == nLoadoutEquipmentSocketsCount[nBodySlotIndex])
        {
            return TRUE;
        }
    }

    }

    return FALSE;
}
































































static SNO sGetHeroWaypointSNO()
{

    return GlobalSNOGet(GLOBALSNO_ACTOR_FRONTEND_HERO_WAYPOINT_MUTED);
}



void UIEquipmentManager::UpdateLoadoutPreview(const Player & tPlayer,
    const LoadoutSavedData & tLoadout,
    UIRActor & tRActor,
    bool bShowWaypoint)

{
    if (!tLoadout.bEmpty)
    {
        PlayerClass ePlayerClass = PlayerGetPlayerClass(&tPlayer);
        BOOL bIsMale = PlayerIsMale(tPlayer);



        RActor * ptRActor = RActorTryToGet(tRActor.GetRActor());
        if (!ptRActor)
        {
            tRActor.Cleanup();

            SNO snoActor = GetFrontendCharacterSNO(ePlayerClass, !bIsMale);
            VisualEquipment tEquipment = VisualInventoryGet(LocalPlayerACDGet())->tEquipmentCurrent;
            for (int32 i = 0; i < 4; i++)
            {
                tEquipment.tCosmeticItems[i].gbidItem = -1;
            }

            PRTransform transform = cgtPRTransformIdentity;



            RActorID idRActorNew = UI_CreateHeroUIRActor(snoActor,
                GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE),
                tEquipment,
                VIT_PLAYER,
                ePlayerClass,
                !bIsMale,
                transform,
                tRActor.GetRenderContext(), 0x68700, 1.0f, 0);

            tRActor.SetRActor(idRActorNew);

            ptRActor = RActorTryToGet(idRActorNew);


            ptRActor->dwFlags |= 0x20000;


            tRActor.SetLight(LightAdd(0, GlobalSNOGet(GLOBALSNO_LIGHT_3DCHARACTER), PRTransform(cgqIdentity, cgwpNull), 0, tRActor.GetRenderContext()));

            if (bShowWaypoint)
            {

                SNO snoActorWaypoint = sGetHeroWaypointSNO();
                RActorID idWaypoint = RActorCreate(snoActorWaypoint,
                    1.0f,
                    -1.0f,
                    PRTransform(cgqIdentity, cgwpNull),
                    0,
                    ptRActor->eContext,
                    -1);
                RActorAttachExistingRActor(ptRActor->id,
                    idWaypoint,
                    HardpointHash("Default"));
            }
        }


        VisualInventory tVisualInventory;
        tVisualInventory.Init();

        VisualEquipment & tVisualEquipment = tVisualInventory.tEquipmentCurrent;
        for (int32 nBodySlotIndex = 0; nBodySlotIndex < 13; nBodySlotIndex++)
        {
            InventorySlotType eInvSlot = nBodySlotIndex + 1;
            VisualInventorySlot eVisSlot = VisualInventorySlotFromInventorySlot(eInvSlot);
            if (eVisSlot == VIS_INVALID)
            {
                continue;
            }

            ACDID idACDItem = ACDFromANN(tLoadout.tEquipment[nBodySlotIndex].annItem);
            if (!LoadoutCanFindItem(idACDItem))
            {
                if (eVisSlot == VIS_LEFT_HAND || eVisSlot == VIS_RIGHT_HAND)
                {
                    tVisualEquipment.FixOffHandWeapons();
                }
                continue;
            }

            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (!ItemMeetsOwnerClassRestrictions(tACDItem.hGB.gbid, ePlayerClass, FALSE))
            {
                if (eVisSlot == VIS_LEFT_HAND || eVisSlot == VIS_RIGHT_HAND)
                {
                    tVisualEquipment.FixOffHandWeapons();
                }
                continue;
            }

            GBID gbidTransmog = tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_TRANSMOG_GBID));
            tVisualEquipment.tVisualItem[eVisSlot].gbidItem = (gbidTransmog != -1) ? gbidTransmog : tACDItem.hGB.gbid;
            tVisualEquipment.tVisualItem[eVisSlot].eDyeType = tACDItem.GetDyeType();

            if (ItemIsLegendary(tACDItem.hGB.gbid))
            {
                tVisualEquipment.tVisualItem[eVisSlot].eItemEffectType = 0;
                tVisualEquipment.tVisualItem[eVisSlot].nEffectLevel = -1;
            }
            else
            {
                ACDGetItemEffectTypeAndLevel(tACDItem, tVisualEquipment.tVisualItem[eVisSlot].eItemEffectType, tVisualEquipment.tVisualItem[eVisSlot].nEffectLevel, TRUE, FALSE);
            }
        }

        VisualInventoryClearAttachments(ptRActor);
        VisualInventoryApply(ptRActor, tVisualInventory, VIT_PLAYER, ePlayerClass, !bIsMale);
        UIRActor::PlayAnimForEquipment(ptRActor, tVisualEquipment, ePlayerClass, 0x68700, FALSE);
    }
    else
    {
        tRActor.Cleanup();
    }
}



void UIEquipmentManager::PromptForLoadoutName(const Player & tPlayer,
    CRefString sDialogTitle,
    CRefString sMessage,
    UITextInputPrompt::DialogCallback pfnCallback)

{
    int32 nControllerIndex = tPlayer.nControllerIndex;










    int32 nSelectedLoadout = GetSelectedLoadout();
    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[nSelectedLoadout];

    CRefString sLoadoutName;
    if (!tLoadout.bEmpty && strlen(tLoadout.usName) > 0)
    {
        sLoadoutName = tLoadout.usName;
    }
    else
    {
        sLoadoutName = LocalizedSprintf(StringListFetch("Hero:EquipmentManager_DefaultGearSetName"), CRefString().Format("%d", nSelectedLoadout + 1), NULL);
    }

    UITextInputPrompt::Show(sDialogTitle.str(), sMessage.str(), sLoadoutName.str(), pfnCallback, TRUE, 13, nControllerIndex);
}
