// Original file: X/Client/UserInterface/Console/UIConsoleFastEquip.cpp
//
// The console "fast equip" popup: newly picked-up items, skills and runes queued per player.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../UIID.h"
#include "../UI.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Player.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "ConsoleScreenManager.h"
#include "../Interface/UIGame.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../Controls/UITextControl.h"
#include "../../Sound/Sound.h"
#include "UIConsoleNewItemIndicators.h"
#include "../../CPlayer.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
void CACDInventoryRequestDrop(ActorCommonData * ptACDItem, ACDID idACDOwner);  // X/Client/ACDClientInventory.cpp
namespace UIUtils { namespace Console { BOOL PlayerIsInActiveTieredRift(const Player * ptPlayer); void SetJunkFlag(ActorCommonData & ptACD, BOOL bJunk, Player * ptPlayer); } } BOOL ItemCanBeMarkedAsJunk(const ActorCommonData & tACDItem);  // UIConsoleUtils.cpp
namespace UIHUD { namespace Console { int32 PlayerGetHUDSlot(const Player * ptPlayer); } } namespace Tutorial { void Shown(Player & tPlayer, SNO snoTutorial); BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); } ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACDItem); BOOL ItemHasSockets(const ActorCommonData & tACDItem); uint32 PlayerGetLocalPlayerCount(); extern UIID UIWND_CONSOLE_HUD_PLAYER_SLOTS;
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr; /* UIConsoleHUD.cpp, Tutorial.cpp, CPlayer.cpp, UIIDs.cpp, UITutorialManager.h */ namespace UIEventQueue { namespace Console { void UnhideForPlayerIndex(int32 nPlayerIndex); void HideForPlayerIndex(int32 nPlayerIndex); } }  // UIConsoleEventQueue.cpp

namespace UIFastEquip {
namespace Console {

enum {
    MAX_FAST_EQUIP_QUEUE_LENGTH = 10,
    INVALID_INDEX = -1,
};

enum EntryType {
    ENTRY_INVALID = -1,
    ENTRY_ITEM = 0,
    ENTRY_SKILL = 1,
    ENTRY_RUNE = 2,
};















struct ItemID {
    union {
        ACDID idACDIDItem;
        SNO idSNOPower;
    };
    RuneType eRuneType;
    EntryType eEntryType;



    BOOL operator==(const ItemID & rhs) const
    {
        return idACDIDItem == rhs.idACDIDItem && eEntryType == rhs.eEntryType;
    }
};

typedef blz::vector<ItemID> ItemArray;
class PlayerItemList {
public:
    ~PlayerItemList() {}



    PlayerItemList() : m_nCurrentShownSlot(0), m_bActive(FALSE) {}
    BOOL AddItem(ACDID IDItem);
    void AddSkill(SNO snoSkill);
    void AddRune(SNO snoRune, RuneType eRuneType);
    BOOL AlreadyReported(SNO snoPower, RuneType eRuneType);
    void Show();
    void Hide();
    void CleanUp();
    void HideSlot();
    void GetNextValidSlot();
    void GetPrevValidSlot();
    BOOL CurrentSlotValid();
    void ShowCurrentItem();
    void RemoveItem(ItemID idItem, int32 index);
    void AddEntry(ItemID & tItem);
    BOOL IsValid(int32 nIndex);
    void UpdateLegend();
    void Clear();
    int32 m_nPlayerIndex;
    int32 m_nCurrentShownSlot;
    UIID m_idEntry;
    BOOL m_bActive;
    ItemArray m_aEntries;
};

class ItemLists {
public:












































    ~ItemLists() {}


    ItemLists()
    {
        for (int32 i = 0; i < 4; i++)
            tPlayerList[i].m_nPlayerIndex = i;
    }
    PlayerItemList tPlayerList[4];
};





static ItemLists * sgpItemLists;

void InitializeForGame()
{
    sgpItemLists = new ItemLists;
}




void DisposeForGame()
{
    delete sgpItemLists;
    sgpItemLists = NULL;
}





void sHandlePickupTutorials(const Player * ptPlayer, const ActorCommonData & tACDItem)
{
    SNO snoEventQueueTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_EVENT_QUEUE);
    if (!Tutorial::PlayerHasSeen(*ptPlayer, snoEventQueueTutorial))
    {
        UIID idEntry = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.EventList.EventSlot01", UIHUD::Console::PlayerGetHUDSlot(ptPlayer) + 1));
        gtTutorialMgr.ShowTutorial(idEntry, snoEventQueueTutorial, ptPlayer, TRUE);
        return;
    }

    if (!gtTutorialMgr.IsAnyShowing(0) && ItemGetDisplayedQualityLevel(tACDItem) > 2)
    {
        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_FIRST_MAGIC_ITEM), ptPlayer, TRUE);
    }

    if (!gtTutorialMgr.IsAnyShowing(0) && ItemHasSockets(tACDItem))
    {
        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_ITEMWITHSOCKETFOUND), ptPlayer, TRUE);
    }

    if (!gtTutorialMgr.IsAnyShowing(0) && ItemGetUseType(tACDItem) == ITEM_USE_TREASURE_BAG)
    {
        ScopedItemDefinitionLockR pDefItem(tACDItem.hGB.gbid);
        if (!pDefItem || pDefItem->snoActor != GlobalSNOGet(GLOBALSNO_CHALLENGE_RIFTS_REWARD_BAG))
        {
            gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_HORADRIC_CACHE), ptPlayer, TRUE);
        }
    }
}












void ReportItemPickup(const Player * ptPlayer, const ActorCommonData & tACDItem)
{
    if (!PlayerIsLocal(ptPlayer))
        return;

    if (!ItemIsEquippable(tACDItem) && ItemGetUseType(tACDItem) != ITEM_USE_TREASURE_BAG && ItemGetUseType(tACDItem) != ITEM_USE_PLAYER_GIFT)
        return;

    const InventoryLocation * ptInvLoc = tACDItem.TryToGetInventoryLocation();
    if (ptInvLoc && ptInvLoc->eSlot != SLOT_PLAYER_BACKPACK)
        return;

    PlayerItemList & tItemList = sgpItemLists->tPlayerList[ptPlayer->nPlayerIndex];
    if (tItemList.AddItem(tACDItem.id))
    {
        sHandlePickupTutorials(ptPlayer, tACDItem);
    }
}


















void ReportNewSkill(const Player * ptPlayer, const SNO snoPower)
{
    if (PlayerIsLocal(ptPlayer))
    {
        PlayerItemList & tItemList = sgpItemLists->tPlayerList[ptPlayer->nPlayerIndex];
        tItemList.AddSkill(snoPower);
    }
}











void ReportNewRune(const Player * ptPlayer, const SNO snoPower, const RuneType eRuneType)
{
    if (PlayerIsLocal(ptPlayer))
    {
        PlayerItemList & tItemList = sgpItemLists->tPlayerList[ptPlayer->nPlayerIndex];
        tItemList.AddRune(snoPower, eRuneType);
    }
}











void OpenFastEquip(const int32 nControllerIndex)
{
    if (::Console::Screens::IsScreenSetOpen())
        return;

    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);
    if (nPlayerIndex == INVALID_INDEX)
        return;

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer->IsAlive())
    {
        PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];
        if (!tItemList.m_bActive)
            tItemList.Show();
    }
}













void OnUp(const UIID &, const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);
    if (nPlayerIndex == INVALID_INDEX)
        return;

    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];
    if (tItemList.m_bActive)
    {
        tItemList.GetPrevValidSlot();
        tItemList.ShowCurrentItem();
    }
}











void OnDown(const UIID &, const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);
    if (nPlayerIndex == INVALID_INDEX)
        return;

    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];
    if (tItemList.m_bActive)
    {
        tItemList.GetNextValidSlot();
        tItemList.ShowCurrentItem();
    }
}






















































































































































































void OnButton3(const UIID &, const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == INVALID_INDEX)
        return;




    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];

    if (!tItemList.m_bActive || tItemList.m_nCurrentShownSlot == INVALID_INDEX)
        return;




    ItemID item = tItemList.m_aEntries[tItemList.m_nCurrentShownSlot];

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    switch (item.eEntryType)
    {
    case ENTRY_ITEM:
        {
            if (item.idACDIDItem == ACDID_INVALID)
                return;

            ACDID idItem = item.idACDIDItem;
            ActorCommonData & tACDItem = ACDGet(idItem);

            if (ItemCanBeMarkedAsJunk(tACDItem))
            {
                ptPlayer->ptNewItemManager->UnMarkAsNew(idItem);
                UIUtils::Console::SetJunkFlag(tACDItem, TRUE, ptPlayer);
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MARK_AS_JUNK), cgtPSODefaults);
                tItemList.RemoveItem(item, tItemList.m_nCurrentShownSlot);
                tItemList.ShowCurrentItem();
            }
            break;
        }
    case ENTRY_SKILL:
    case ENTRY_RUNE:
        tItemList.RemoveItem(item, tItemList.m_nCurrentShownSlot);
        break;
    }
}



void OnButton2(const UIID &, const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);
    if (nPlayerIndex == INVALID_INDEX)
        return;

    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];
    if (tItemList.m_bActive)
    {
        tItemList.Hide();
        tItemList.CleanUp();
    }
}











void OnButton4(const UIID &, const int32 nControllerIndex)
{

    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == INVALID_INDEX)
        return;




    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];

    if (!tItemList.m_bActive || tItemList.m_nCurrentShownSlot == INVALID_INDEX)
        return;




    ItemID item = tItemList.m_aEntries[tItemList.m_nCurrentShownSlot];

    if (item.eEntryType != ENTRY_ITEM || item.idACDIDItem == ACDID_INVALID)
        return;

    ACDID idItem = item.idACDIDItem;
    ActorCommonData * ptACDItem = ACDTryToGet(idItem);








    if (ItemGetUseType(*ptACDItem) == ITEM_USE_PLAYER_GIFT)
        return;



    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    if (ptPlayer && UIUtils::Console::PlayerIsInActiveTieredRift(ptPlayer))
        return;



    CACDInventoryRequestDrop(ptACDItem, ptPlayer->idPlayerACD);

    tItemList.RemoveItem(item, tItemList.m_nCurrentShownSlot);
}



void OnLStick(const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);
    if (nPlayerIndex == INVALID_INDEX)
        return;

    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];
    if (tItemList.m_bActive)
    {
        tItemList.Hide();
        tItemList.CleanUp();
    }
}










inline BOOL PlayerItemList::AddItem(ACDID IDItem)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {
        ItemID tItem;
        tItem.idACDIDItem = IDItem;
        tItem.eEntryType = ENTRY_ITEM;
        AddEntry(tItem);
        return TRUE;
    }
    return FALSE;
}







inline BOOL PlayerItemList::AlreadyReported(SNO snoPower, RuneType eRuneType)
{
    for (uint32 n = 0; n < m_aEntries.size(); n++)
    {
        if (m_aEntries[n].eEntryType == ENTRY_SKILL)
        {
            if (m_aEntries[n].idSNOPower == snoPower)
                return TRUE;
        }
        else if (m_aEntries[n].eEntryType == ENTRY_RUNE && m_aEntries[n].idSNOPower == snoPower && m_aEntries[n].eRuneType == eRuneType)
        {
            return TRUE;
        }
    }
    return FALSE;
}



void PlayerItemList::AddSkill(SNO snoSkill)
{
    if (AlreadyReported(snoSkill, -1))
        return;
    Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {
        ItemID tItem;
        tItem.idSNOPower = snoSkill;
        tItem.eEntryType = ENTRY_SKILL;
        AddEntry(tItem);
    }
}







void PlayerItemList::AddRune(SNO snoRune, RuneType eRuneType)
{
    if (AlreadyReported(snoRune, eRuneType))
        return;
    Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer)
    {
        ItemID tItem;
        tItem.idSNOPower = snoRune;
        tItem.eRuneType = eRuneType;
        tItem.eEntryType = ENTRY_RUNE;
        AddEntry(tItem);
    }
}









void PlayerItemList::AddEntry(ItemID & tItem)
{
    m_aEntries.push_back(tItem);
    if (!m_bActive)
    {
        if (m_aEntries.size() > MAX_FAST_EQUIP_QUEUE_LENGTH)
        {
            blzext::vector_erase_at(m_aEntries, 0);
        }
        m_nCurrentShownSlot = m_aEntries.size() - 1;
    }
}




void PlayerItemList::RemoveItem(ItemID idItem, int32 index)
{


    if (index == INVALID_INDEX)
    {
        ItemArray::const_iterator iter = blz::find(m_aEntries.begin(), m_aEntries.end(), idItem);
        if (iter != m_aEntries.end())
        {
            index = iter - m_aEntries.begin();
        }
    }





    if (index != INVALID_INDEX)
    {
        blzext::vector_erase_at(m_aEntries, index);
    }

    GetPrevValidSlot();

    if (m_bActive)
    {
        if (m_nCurrentShownSlot == INVALID_INDEX)
        {
            Hide();
            return;
        }

        ShowCurrentItem();
    }
}




void PlayerItemList::Show()
{

    Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);




    int32 nPlayerCount = PlayerGetLocalPlayerCount();

    UIID idHUDRoot;
    if (nPlayerCount == 1)
    {
        idHUDRoot = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, ".Player1");
    }
    else
    {
        int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);
        idHUDRoot = UIGame::GetPlayerHUDID(nHudSlot);
    }

    m_idEntry = UIIDCreateFromParent(idHUDRoot, ".Local.FastAccessMenu.FastAccessSlot");

    if (!CurrentSlotValid())
    {
        GetNextValidSlot();
    }

    if (m_nCurrentShownSlot != INVALID_INDEX)
    {
        UIWindow & tWndEntry = UI_GetWindowFromID(m_idEntry);
        tWndEntry.SetFilterControllerIndex(ptPlayer->nControllerIndex);
        tWndEntry.SetActive(TRUE);
        ShowCurrentItem();
        m_bActive = TRUE;
        UIEventQueue::Console::HideForPlayerIndex(m_nPlayerIndex);
        UIID idLegend;
        idLegend.SetFromParent(m_idEntry, "FastAccessLegend");
        Tutorial::Shown(*ptPlayer, GlobalSNOGet(GLOBALSNO_TUTORIAL_EVENT_QUEUE));
        gtTutorialMgr.ShowTutorial(idLegend, GlobalSNOGet(GLOBALSNO_TUTORIAL_FAST_EQUIP), ptPlayer, TRUE);
    }
}
















inline BOOL PlayerItemList::CurrentSlotValid()
{
    return m_nCurrentShownSlot != INVALID_INDEX && IsValid(m_nCurrentShownSlot);
}












































































































































inline void PlayerItemList::Hide()
{

    m_bActive = FALSE;

    UIWindow & tWndEntry = UI_GetWindowFromID(m_idEntry);
    tWndEntry.SetFilterControllerIndex(-1);
    tWndEntry.SetActive(FALSE);

    UIEventQueue::Console::UnhideForPlayerIndex(m_nPlayerIndex);
}




















inline void PlayerItemList::GetNextValidSlot()
{
    for (int32 n = 0; n < m_aEntries.size(); n++)
    {
        int32 nNextEntry = (m_nCurrentShownSlot + n + 1) % m_aEntries.size();
        BOOL bValid = IsValid(nNextEntry);
        if (bValid)
        {
            m_nCurrentShownSlot = nNextEntry;
            return;
        }
        m_aEntries[nNextEntry].idACDIDItem = ACDID_INVALID;
    }
    m_nCurrentShownSlot = INVALID_INDEX;
}







inline void PlayerItemList::GetPrevValidSlot()
{
    for (int32 n = 0; n < m_aEntries.size(); n++)
    {
        int32 nPrevEntry = (m_nCurrentShownSlot - n - 1 + (int32)m_aEntries.size()) % m_aEntries.size();
        BOOL bValid = IsValid(nPrevEntry);
        if (bValid)
        {
            m_nCurrentShownSlot = nPrevEntry;
            return;
        }
        m_aEntries[nPrevEntry].idACDIDItem = ACDID_INVALID;
    }
    m_nCurrentShownSlot = INVALID_INDEX;
}







BOOL PlayerItemList::IsValid(int32 nIndex)
{
    if (nIndex < m_aEntries.size())
    {




        ItemID idItem = m_aEntries[nIndex];

        if (idItem.eEntryType == ENTRY_ITEM)
        {
            ActorCommonData * ptACDItem = ACDTryToGet(idItem.idACDIDItem);
            if (!ptACDItem)
                return FALSE;








            if (PlayerGetByPlayerIndex(m_nPlayerIndex)->idPlayerACD != ptACDItem->GetInventoryContainerOwner())
                return FALSE;
            const ActorLocation & tLocation = ptACDItem->GetLocation();
            ActorLocationType eLocation = tLocation.eActorLocation;
            if (eLocation != 1 || tLocation.tInvLocation.eSlot == SLOT_PLAYER_BUYBACK || tLocation.tInvLocation.eSlot == SLOT_PLAYER_SHARED_STASH)
                return FALSE;
            return ptACDItem->AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED) == 0;
        }



        if (idItem.idSNOPower == SNO_NONE)
            return FALSE;





        Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);

        if (idItem.eEntryType == ENTRY_SKILL)
        {
            for (int32 i = 0; i < 6; i++)
            {
                if (ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == idItem.idSNOPower)
                    return FALSE;
            }
        }
        else if (idItem.eEntryType == ENTRY_RUNE)
        {
            for (int32 i = 0; i < 6; i++)
            {
                if (ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == idItem.idSNOPower && ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].eRuneType == idItem.eRuneType)
                    return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

// The entry checks at 1196 and 1264 read the selected entry directly.
void PlayerItemList::UpdateLegend()

{
    if (m_aEntries[m_nCurrentShownSlot].eEntryType == ENTRY_ITEM)
    {
        ACDID idACDItem = m_aEntries[m_nCurrentShownSlot].idACDIDItem;

        Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);

        ActorCommonData & tACDItem = ACDGet(idACDItem);
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);

        BOOL bEquippable = ItemIsEquippable(tACDItem) && !ItemIsUnidentified(tACDItem);

        BOOL bInActiveTieredRift = UIUtils::Console::PlayerIsInActiveTieredRift(ptPlayer);

        if (bEquippable)
        {
            GameError eError;
            bEquippable = ItemOwnerMeetsRequirements(tACDItem, tACDPlayer, &eError);
        }

        UIID idLegend;

        idLegend.SetFromParent(m_idEntry, "FastAccessLegend");

        if (!UI_TestID(idLegend))
        {
            UIWindow & tWndEntry = UI_GetWindowFromID(m_idEntry);
            tWndEntry.SetActive(FALSE);
            return;
        }

        UIText & tLegendText = UI_GetTextFromID(idLegend);

        tLegendText.SetActive(TRUE);

        CRefString sLegend;

        if (!bInActiveTieredRift && bEquippable)
        {
            sLegend += (LocalizedSprintf(StringListFetch("Console:FastEquipLegendEquip")) + CRefString("  "));
        }

        if (ItemCanBeMarkedAsJunk(tACDItem))
        {
            sLegend += (LocalizedSprintf(StringListFetch("Console:FastEquipLegendJunk")) + CRefString("  "));
        }

        if (ItemGetUseType(tACDItem) == ITEM_USE_TREASURE_BAG)
        {
            sLegend = LocalizedSprintf(StringListFetch("Console:FastEquipLegendOpen")) + CRefString("  ");
        }
        else if (ItemGetUseType(tACDItem) == ITEM_USE_PLAYER_GIFT)
        {
            sLegend = LocalizedSprintf(StringListFetch("Console:FastEquipLegendSend")) + CRefString("  ");
        }
        else
        {
            if (!bInActiveTieredRift)
            {
                sLegend += (LocalizedSprintf(StringListFetch("Console:FastEquipLegendDrop")) + CRefString("  "));
            }
        }

        sLegend += LocalizedSprintf(StringListFetch("Console:FastEquipLegendClose"));
        // The inlined control setter is recorded at 1260.
        tLegendText.SetControllerIndexOverride(ptPlayer->nControllerIndex);
        tLegendText.SetText(sLegend, FALSE);

    }
    else if (m_aEntries[m_nCurrentShownSlot].eEntryType != ENTRY_INVALID)
    {
        UIID idLegend;

        idLegend.SetFromParent(m_idEntry, "FastAccessLegend");

        UIText & tLegendText = UI_GetTextFromID(idLegend);

        tLegendText.SetActive(TRUE);

        tLegendText.SetText(LocalizedSprintf(StringListFetch("Console:FastEquipSkillLegend")), FALSE);
    }
}


inline void PlayerItemList::Clear()
{
    m_aEntries.clear();
}


inline void PlayerItemList::CleanUp()
{

    for (int32 n = m_aEntries.size(); n > 0; n--)
    {
        if (m_aEntries[n - 1].eEntryType == ENTRY_INVALID)
        {
            blzext::vector_erase_at(m_aEntries, n - 1);
        }
    }

    if (m_aEntries.size() > MAX_FAST_EQUIP_QUEUE_LENGTH)
    {
        int32 nItemsToRemove = (int32)m_aEntries.size() - MAX_FAST_EQUIP_QUEUE_LENGTH;
        for (int32 n = 0; n < nItemsToRemove; n++)
        {
            blzext::vector_erase_at(m_aEntries, 0);
        }
    }
}


BOOL IsActiveForController(const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);
    if (nPlayerIndex < 0)
        return FALSE;
    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];
    return tItemList.m_bActive;
}






void CloseForPlayer(const int32 nPlayerIndex)
{
    if (nPlayerIndex < 0)
        return;

    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];
    if (tItemList.m_bActive)
    {
        tItemList.Hide();
        tItemList.CleanUp();
    }
}


void Close()
{
    for (int32 i = 0; i < 4; i++)
        CloseForPlayer(i);
}




void OnPlayerLeft(const int32 nPlayerIndex)
{
    if (nPlayerIndex >= 0)
    {
        CloseForPlayer(nPlayerIndex);
        sgpItemLists->tPlayerList[nPlayerIndex].Clear();
    }
}

}  // namespace Console
}  // namespace UIFastEquip

#include "../../ImageCollection.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
// Owner-local declarations from UIInventory.h, UIConsoleInventoryFilters.h, UIConsoleEventQueue.h,
// UISkills.h, UIConsoleMail.h, Items.h, ACDInventory.h, ACDClientInventory.h, and Message.h.
namespace UIInventory {
void GetComparisonValueStrings(ActorCommonData * ptACDOwner, const ActorCommonData & tACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, CRefString & sValEHP, CRefString & sValDmg, CRefString & sValHealing);
void QuickMoveToEmptySlot(const ActorCommonData & tACD, ActorCommonData & tACDItem, int32 & nNumSlots, InventorySlotType * eValidSlots, BOOL bUseOffhand, Player * ptPlayer);
namespace Console {
BOOL ItemCanBeWornByThisPlayer(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);
}
}
namespace UIEventQueue {
namespace Console {
ImageHandle GetImageForItem(const ActorCommonData & tACDItem);
void DisplayItemEquippedMessageForPlayer(const Player * ptPlayer, const ActorCommonData & tACDItem);
}
}
namespace UISkills {
BOOL GetName(SNO snoPower, CRefString & sName);
int32 SkillCategoryGetSlot(const SkillCategory eCategory);
}
namespace UIMail {
namespace Console {
void SendGift(const ActorCommonData & tACD, int32 nControllerIndex);
}
}
CRefString PowerGetRuneName(SNO snoPower, RuneType eRune);
int32 ItemGetSpecialAffixCount(const ActorCommonData & tACDItem, int32 eClass);
void CACDInventoryRequestUse(ActorCommonData & tACDItem, ACDID idACDTarget, BOOL bFromHotbar, Player * ptPlayer);
void ACDInventoryWhereCanThisGo(const ActorCommonData & tACD, GBID gbidItem, int32 & nNumSlots, InventorySlotType * peSlots);
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
namespace UIFastEquip {
namespace Console {
#line 390 "X/Client/UserInterface/Console/UIConsoleFastEquip.cpp"
void OnButton1(
    const UIID &,
    const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == -1)
        return;



    PlayerItemList & tItemList = sgpItemLists->tPlayerList[nPlayerIndex];

    if (!tItemList.m_bActive || tItemList.m_nCurrentShownSlot == INVALID_INDEX)
        return;



    ItemID item = tItemList.m_aEntries[tItemList.m_nCurrentShownSlot];

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);

    if (UIUtils::Console::PlayerIsInActiveTieredRift(ptPlayer))
        return;



    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return;



    // Original lines 424 and 428 distinguish the entry kind and validate its item ID.
    if (item.eEntryType == ENTRY_ITEM)
    {
        ACDID idItem = item.idACDIDItem;

        if (idItem == ACDID_INVALID)
            return;



        ActorCommonData * ptACDItem = ACDTryToGet(idItem);
        if (!ptACDItem)
            return;



        ActorCommonData & tACDItem = *ptACDItem;

        if (ItemGetUseType(tACDItem) == ITEM_USE_TREASURE_BAG)
        {
            CACDInventoryRequestUse(tACDItem, ACDID_INVALID, TRUE, ptPlayer);
            tItemList.RemoveItem(item, tItemList.m_nCurrentShownSlot);
            tItemList.Hide();
            return;
        }



        if (ItemGetUseType(tACDItem) == ITEM_USE_PLAYER_GIFT)
        {
            UIMail::Console::SendGift(tACDItem, nControllerIndex);
            tItemList.RemoveItem(item, tItemList.m_nCurrentShownSlot);
            tItemList.Hide();
            return;
        }



        if (ItemIsUnidentified(tACDItem))
            return;



        if (!UIInventory::Console::ItemCanBeWornByThisPlayer(*ptACDPlayer, tACDItem))
            return;



        int32 nNumSlots = 0;
        InventorySlotType eValidSlots[4];
        ACDInventoryWhereCanThisGo(*ptACDPlayer, tACDItem.hGB.gbid, nNumSlots, eValidSlots);
        if (eValidSlots[0] == -1)
            return;



        BOOL bEquippable = ItemIsEquippable(tACDItem);
        // The requirements call uses the nested error local and updates bEquippable.
        if (bEquippable)
        {
            GameError eError;
            bEquippable = ItemOwnerMeetsRequirements(tACDItem, *ptACDPlayer, &eError);
        }

        if (!bEquippable)
            return;



        ptPlayer->ptNewItemManager->UnMarkAsNew(idItem);

        UIInventory::QuickMoveToEmptySlot(*ptACDPlayer, tACDItem, nNumSlots, eValidSlots, FALSE, ptPlayer);

        tItemList.RemoveItem(item, tItemList.m_nCurrentShownSlot);
        tItemList.Hide();

        UIEventQueue::Console::DisplayItemEquippedMessageForPlayer(ptPlayer, tACDItem);
    }
    else if (item.eEntryType != ENTRY_INVALID)
    {

        RuneType eRuneType = -1;
        int32 nSlot = -1;
        if (item.eEntryType == ENTRY_RUNE)
        {
            eRuneType = item.eRuneType;



            for (int32 i = 0; i < 6; ++i)
            {
                if (ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == item.idSNOPower)
                {
                    nSlot = i;
                    break;
                }
            }
        }
        if (nSlot == -1)
        {

            SNO snoSkillKit = ptACDPlayer->GetSkillKit();
            ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);

            int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);

            for (int32 i = 0; i < nCount; ++i)
            {
                ActiveSkillEntry & tSkill = pdefSkillKit->arActiveSkillEntries[i];

                if (tSkill.snoPower == item.idSNOPower)

                    nSlot = UISkills::SkillCategoryGetSlot(tSkill.eCategoryNew);
            }
        }


        if (nSlot != -1)
        {
            AssignSkillMessage tMessage;
            SigmaMemoryClear(&tMessage, sizeof(tMessage));
            tMessage.snoPower = item.idSNOPower;
            tMessage.nRuneType = eRuneType;
            tMessage.nSlot = nSlot;
            MessageSendToServer(CMSG_TRY_ASSIGN_SKILL_TO_SLOT, &tMessage, sizeof(tMessage), ptPlayer);
        }

        tItemList.RemoveItem(item, tItemList.m_nCurrentShownSlot);
        tItemList.Hide();
    }
}
// Inference: no-selection early return expresses the original common epilogue.
#line 922
void PlayerItemList::ShowCurrentItem()
{
    if (m_nCurrentShownSlot == INVALID_INDEX)
    {
        UIID idName;
        idName.SetFromParent(m_idEntry, "Item");
        UIText & tTextName = UI_GetTextFromID(idName);
        tTextName.SetActive(FALSE);
        UIID idInfo;
        idInfo.SetFromParent(m_idEntry, "Info");
        UIText & tTextInfo = UI_GetTextFromID(idInfo);
        tTextInfo.SetActive(FALSE);
        UIID idIcon;
        idIcon.SetFromParent(m_idEntry, "Icon");
        UIWindow & tWndIcon = UI_GetWindowFromID(idIcon);
        tWndIcon.SetActive(FALSE);
        return;
    }
    if (m_aEntries[m_nCurrentShownSlot].eEntryType == ENTRY_ITEM)
    {
#line 933
        ACDID idACDItem = m_aEntries[m_nCurrentShownSlot].idACDIDItem;
#line 935
        Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);
#line 937
        ActorCommonData & tACDItem = ACDGet(idACDItem);
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
#line 940
        UIID idName, idInfo, idIcon, idSmallIcon;
        idName.SetFromParent(m_idEntry, "Item");
        idInfo.SetFromParent(m_idEntry, "Info");
        idIcon.SetFromParent(m_idEntry, "Icon");
        idSmallIcon.SetFromParent(m_idEntry, "SmallIcon");
#line 947
        UIText & tTextName = UI_GetTextFromID(idName);
        tTextName.SetActive(TRUE);
#line 950
        UIText & tTextInfo = UI_GetTextFromID(idInfo);
        tTextInfo.SetActive(TRUE);
#line 953
        CRefString sValEHP, sValDmg, sValHealing;
        UIInventory::GetComparisonValueStrings(&tACDPlayer, tACDItem, FALSE, FALSE, sValEHP, sValDmg, sValHealing);
        tTextName.SetText(ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE), FALSE);
#line 958
        CRefString szComparison = "";
        if (ItemGetUseType(tACDItem) != ITEM_USE_TREASURE_BAG && ItemGetUseType(tACDItem) != ITEM_USE_PLAYER_GIFT)
        {
            szComparison = LocalizedSprintf(StringListFetch("Console:ItemPickupEvent"), sValDmg.str(), sValEHP.str(), sValHealing.str());
#line 965
            int32 nSocketCount = tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS));
            if (nSocketCount > 0)
                szComparison += CRefString(" ") + LocalizedSprintf(StringListFetch("Console:SocketedItemPickupEvent"), UnicodeText::FormatNumber(nSocketCount, TRUE, FNS_NORMAL, 0).str());
#line 973
            int32 nSpecialAffixes = ItemGetSpecialAffixCount(tACDItem, PlayerGetPlayerClass(ptPlayer)) - (nSocketCount > 0 ? 1 : 0);
            if (nSpecialAffixes > 0)
                szComparison += CRefString(" ") + LocalizedSprintf(StringListFetch("Console:SpecialItemPickupEvent"), UnicodeText::FormatNumber(nSpecialAffixes, TRUE, FNS_NORMAL, 0).str());
        }
        tTextInfo.SetText(szComparison.str(), FALSE);
        UI_GetWindowFromID(idSmallIcon).SetActive(FALSE);
#line 987
        UIWindow & tWndIcon = UI_GetWindowFromID(idIcon);
        tWndIcon.SetActive(TRUE);
#line 990
        ImageHandle hImage = UIEventQueue::Console::GetImageForItem(tACDItem);
#line 992
        ImageTextureFrame tFrame;
        ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tFrame);
        tWndIcon.SetTextureFrame(tFrame);
    }
    else
    {
#line 998
        SNO idSNO = m_aEntries[m_nCurrentShownSlot].idSNOPower;
#line 1000
        UIID idName, idInfo, idIcon, idSmallIcon;
        idName.SetFromParent(m_idEntry, "Item");
        idInfo.SetFromParent(m_idEntry, "Info");
        idIcon.SetFromParent(m_idEntry, "Icon");
        idSmallIcon.SetFromParent(m_idEntry, "SmallIcon");
#line 1008
        UIText & tTextName = UI_GetTextFromID(idName);
        tTextName.SetActive(TRUE);
#line 1012
        CRefString sName;
        UISkills::GetName(idSNO, sName);
#line 1016
        CRefString sTitle = StringListFetch("H2OLayout:SkillUnlockedTitle");
        UIText & tTextInfo = UI_GetTextFromID(idInfo);
        if (m_aEntries[m_nCurrentShownSlot].eEntryType == ENTRY_RUNE)
        {
            tTextName.SetText(LocalizedSprintf(StringListFetch("Console:FastEquipRuneTitle"), sName.str(), NULL), FALSE);
#line 1023
            CRefString sRuneName = PowerGetRuneName(idSNO, m_aEntries[m_nCurrentShownSlot].eRuneType);
            tTextInfo.SetActive(TRUE);
            tTextInfo.SetText(sRuneName, FALSE);
        }
        else
        {
            tTextName.SetText(sName, FALSE);
            tTextInfo.SetActive(FALSE);
        }
        UI_GetWindowFromID(idIcon).SetActive(FALSE);
#line 1035
        UIWindow & tWndIcon = UI_GetWindowFromID(idSmallIcon);
        tWndIcon.SetActive(TRUE);
#line 1038
        ScopedPowerDefinitionLockR pdefPower(idSNO);
#line 1040
        ImageHandle hImage = PowerGeneralTagGetInt(pdefPower, TAG_POWER_ICON_NORMAL);
#line 1042
        ImageTextureFrame tFrame;
        ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tFrame);
        tWndIcon.SetTextureFrame(tFrame);
    }
    UpdateLegend();
}
}
}
