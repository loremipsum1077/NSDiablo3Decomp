// Original file: X/Client/UserInterface/Console/UIConsoleEventQueue.cpp. Generated from build/try/p2-uib/eq.cpp by gen.py.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Gfx/GfxCommon.h"
#include "../UIID.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/Player.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/ImageHandle.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../ImageCollection.h"
#include "../../CPlayer.h"
#include "../../CGame.h"
#include "../../Sound/Sound.h"
#include "../UI.h"
#include "../UIStoryboard.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Interface/UIGame.h"
#include "ConsoleScreenManager.h"
ImageHandle StringToImageHandle(LPCSTR pszName); extern UIID UIWND_CONSOLE_HUD_PLAYER_SLOTS; extern UIID UIWND_CONSOLE_STORYBOARD_COOP_LOOT; extern UIID UIWND_OBJHUD; UIStoryboard & UI_GetStoryboardFromID(const UIID & id); UIRActor & UI_GetRActorFromID(const UIID & id); /* UIIDs.h, UI.cpp, ImageCollection.cpp */ namespace UIHUD { namespace Console { int32 PlayerGetHUDSlot(const Player * ptPlayer); } } namespace UIInventory { void GetComparisonValueStrings(ActorCommonData * ptACDOwner, const ActorCommonData & tACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, CRefString & sValEHP, CRefString & sValDmg, CRefString & sValHealing); } int32 ItemGetSpecialAffixCount(const ActorCommonData & tACDItem, int32 eClass); PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
#include "../Interface/UITutorials.h"
extern UITutorialManager gtTutorialMgr; /* UIConsoleHUD.cpp, UIInventory.cpp, Items.cpp, Player.cpp, UITutorialManager.h */ namespace UIEventQueue {
namespace Console {


enum {
    MAX_SHOWN_EVENTS = 4,
    MAX_SHOWN_EVENTS_MP = 1,
    TIME_BEFORE_FADING = 300,
    FADE_TIME = 60,
    DELAY_BETWEEN_BACKLOG_PURGES = 120,
    DELAY_BETWEEN_BACKLOG_ITEMS = 12,
    FLASH_DURATION = 120,
    DELAY_BETWEEN_CRITICAL_ITEMS = 180,
    REMAINING_TIME_TO_DISPLAY_ON_RESTORE = 30,
};



enum EventEntryType {
    EVENT_TYPE_NONE = 0,
    EVENT_TYPE_ITEM = 1,
    EVENT_TYPE_BUFF = 2,
    EVENT_TYPE_DEBUFF = 3,
    EVENT_TYPE_MESSAGE = 4,
    EVENT_TYPE_LONG_MESSAGE = 5,
    EVENT_TYPE_CRITICAL_MESSAGE = 6,
};

struct BacklogItem {
    ACDID idItem;
};
typedef blz::vector<BacklogItem> BacklogArray;



struct CriticalMessage {
    CRefString sMessage;
    CRefString sMessage2;
    ImageTextureFrame tImage;
};
typedef blz::vector<CriticalMessage> CriticalMessageArray;




class EventEntry {
public:
    uint32 dwEnteredTime;
    EventEntryType eType;
    CRefString sName;
    CRefString sDetails;
    ImageTextureFrame tImage;
    BOOL bUseLargeIcon;


    EventEntry()
        : dwEnteredTime(0)
        , eType(EVENT_TYPE_NONE)
    {
        bUseLargeIcon = FALSE;

        tImage = ImageTextureFrame();
    }
    EventEntry & operator=(const EventEntry & from)
    {

        dwEnteredTime = from.dwEnteredTime;
        eType = from.eType;
        sName = from.sName;
        sDetails = from.sDetails;
        tImage = from.tImage;

        bUseLargeIcon = from.bUseLargeIcon;
        return *this;
    }
};

class PlayerEventQueue {
public:
    int32 nNumEvents;
    BOOL bHidden;
    EventEntry tEntries[MAX_SHOWN_EVENTS];
    BacklogArray aItemBacklog;
    uint32 nNextProcessTime;
    uint32 nFlashActivationTime;
    uint32 nBacklogCount;
    BOOL bWaitForStoryboard;
    BOOL bInitialized;
    UIID idStoryboard;
    CriticalMessageArray arCriticalMessages;
    CriticalMessage tCurrentCriticalMessage;
    BOOL bShowingCriticalEvent;
    uint32 nNextCriticalMessageProcessTime;


    PlayerEventQueue()
        : nNumEvents(0)
        , bHidden(FALSE)
        , nNextProcessTime(0)
        , nFlashActivationTime(0)
        , nBacklogCount(0)
        , bWaitForStoryboard(FALSE)
        , bInitialized(FALSE)
        , idStoryboard(0)
        , bShowingCriticalEvent(FALSE)
        , nNextCriticalMessageProcessTime(0)
    {
    }
};

struct PlayerControls {
    UIWindow * ptWndEntry;
    UIText * ptTextName;
    UIText * ptTextInfo;
    UIText * ptLongMessageInfo;
    UIWindow * ptWndIcon;
    UIWindow * ptWndSmallIcon;
    UIWindow * ptBuffWindow;
    UIWindow * ptDebuffWindow;
};


class EventQueueGlobals {
public:
    PlayerEventQueue tPlayerQueue[4];
    PlayerControls tPlayerControlsArray[4][MAX_SHOWN_EVENTS];


    EventQueueGlobals()
    {
        SigmaMemoryClear(bPlayerControlsValid, sizeof(bPlayerControlsValid));
    }

    BOOL bPlayerControlsValid[4];
};

EventQueueGlobals * sgptEventQueueGlobals = NULL;


void InitializeForGame()
{
    sgptEventQueueGlobals = new EventQueueGlobals;
}




void DisposeForGame()
{
    delete sgptEventQueueGlobals;
    sgptEventQueueGlobals = NULL;
}












ImageHandle GetImageForItem(const ActorCommonData & tACDItem)
{
    GBID gbidItem = tACDItem.hGB.gbid;

    if (ItemIsEquippable(gbidItem))
    {
        if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 4))
        {
            if (ItemHasLabel(gbidItem, ITL_FIST_WEAPON))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Fist");
            if (ItemHasLabel(gbidItem, ITL_BOW))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Bow");
            if (ItemHasLabel(gbidItem, ITL_SWORD))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Sword");
            if (ItemHasLabel(gbidItem, ITL_CROSSBOW))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Crossbow");
            if (ItemHasLabel(gbidItem, ITL_HAND_CROSSBOW))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Crossbow");
            if (ItemHasLabel(gbidItem, ITL_DAGGER))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Dagger");
            if (ItemHasLabel(gbidItem, ITL_CEREMONIAL_DAGGER))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Dagger");
            if (ItemHasLabel(gbidItem, ITL_MACE))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Mace");
            if (ItemHasLabel(gbidItem, ITL_WAND))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Wand");
            if (ItemHasLabel(gbidItem, ITL_MIGHTY_WEAPON))
                return StringToImageHandle("Console_Artisan_WeaponSlot_MightyWeapon");
            if (ItemHasLabel(gbidItem, ITL_STAFF))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Staff");
            if (ItemHasLabel(gbidItem, ITL_COMBAT_STAFF))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Staff");
            if (ItemHasLabel(gbidItem, ITL_POLEARM))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Staff");
            if (ItemHasLabel(gbidItem, ITL_SPEAR))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Staff");
            if (ItemHasLabel(gbidItem, ITL_AXE))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Axe");
            if (ItemHasLabel(gbidItem, ITL_FLAIL))
                return StringToImageHandle("Console_GroundIcon_Crusader");
            if (ItemHasLabel(gbidItem, ITL_SCYTHE))
                return StringToImageHandle("Console_Artisan_WeaponSlot_Scythe");
            return StringToImageHandle("Console_EquippedInventoryRingSlotWeaponMainHand");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 3))
        {
            if (ItemHasLabel(gbidItem, ITL_QUIVER))
                return StringToImageHandle("Console_EquippedInventoryRingSlot_QuiverSmall");
            if (ItemHasLabel(gbidItem, ITL_MOJO))
                return StringToImageHandle("Console_EquippedInventoryRingSlot_MojoSmall");
            if (ItemHasLabel(gbidItem, ITL_ORB))
                return StringToImageHandle("Console_EquippedInventoryRingSlot_OrbSmall");
            if (ItemHasLabel(gbidItem, ITL_SHIELD))
                return StringToImageHandle("Console_EquippedInventoryRingSlot_ShieldSmall");
            if (ItemHasLabel(gbidItem, ITL_OFFHAND_NECROMANCER))
                return StringToImageHandle("Console_GroundIcon_NecromancerOffhand");
            return StringToImageHandle("Console_EquippedInventoryRingSlotWeaponOffHand");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 1))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotHelm");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 2))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotChest");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 5))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotHands");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 6))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotBelt");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 7))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotFeet");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 8))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotShoulder");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 9))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotLegs");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 10))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotWrist");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 13))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotNeck");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 12))
        {
            return StringToImageHandle("Console_EquippedInventoryRingSlotRing");
        }
        else if (ACDInventoryItemAllowedInThisSlotType(gbidItem, 22))
        {
            if (ItemHasLabel(gbidItem, ITL_ENCHANTRESS_SPECIAL))
                return StringToImageHandle("Console_EquippedInventoryRingSlotWeaponOffHand_Enchantress");
            if (ItemHasLabel(gbidItem, ITL_SCOUNDREL_SPECIAL))
                return StringToImageHandle("Console_EquippedInventoryRingSlotWeaponOffHand_Scoundrel");
            if (ItemHasLabel(gbidItem, ITL_TEMPLAR_SPECIAL))
                return StringToImageHandle("Console_EquippedInventoryRingSlotWeaponOffHand_Templar");
        }
    }

    if (ItemHasLabel(gbidItem, ITL_POTION))
        return StringToImageHandle("Console_GroundIcon_Potion");
    if (ItemHasLabel(gbidItem, ITL_SOCKETABLE))
        return StringToImageHandle("Console_GroundIcon_Gem");
    if (ItemHasLabel(gbidItem, ITL_BOOK))
        return StringToImageHandle("Console_GroundIcon_Book");

    return StringToImageHandle("Console_EquippedInventoryRingSlotBag");
}















































void InvalidateUI()
{
    for (uint32 n = 0; n < 4; n++)
    {
        sgptEventQueueGlobals->bPlayerControlsValid[n] = FALSE;
    }
}





void ValidateUIControls(const int32 nPlayerIndex, const UIID & idHUDRoot)
{
    if (!sgptEventQueueGlobals->bPlayerControlsValid[nPlayerIndex])
    {
        sgptEventQueueGlobals->bPlayerControlsValid[nPlayerIndex] = TRUE;
        SigmaMemoryClear(sgptEventQueueGlobals->tPlayerControlsArray[nPlayerIndex], sizeof(sgptEventQueueGlobals->tPlayerControlsArray[nPlayerIndex]));

        Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
        int32 nSlotsToValidate = PlayerIsLocal(ptPlayer) ? MAX_SHOWN_EVENTS : MAX_SHOWN_EVENTS_MP;

        for (int32 i = 0; i < nSlotsToValidate; i++)
        {





            UIID idEntry = GetUIIDNone();
            if (PlayerIsLocal(ptPlayer))
            {
                idEntry = UIIDCreateFromParent(idHUDRoot, CRefString().Format(".Local.EventList.EventSlot%02d", i + 1));
            }
            else
            {


                idEntry = UIIDCreateFromParent(idHUDRoot, CRefString().Format(".Online.RemoteEventSlot"));
            }

            if (UI_TestID(idEntry))
            {
                PlayerControls & tCtrls = sgptEventQueueGlobals->tPlayerControlsArray[nPlayerIndex][i];
                SigmaMemoryClear(&tCtrls, sizeof(tCtrls));

                tCtrls.ptWndEntry = &UI_GetWindowFromID(idEntry);
                UIID idName;
                idName.SetFromParent(idEntry, "Item");
                UIID idInfo;
                idInfo.SetFromParent(idEntry, "Info");
                UIID idLongMsg;
                idLongMsg.SetFromParent(idEntry, "largeMessage");
                UIID idIcon;
                idIcon.SetFromParent(idEntry, "Icon");
                UIID idSmallIcon;
                idSmallIcon.SetFromParent(idEntry, "SmallIcon");

                tCtrls.ptTextName = &UI_GetTextFromID(idName);
                tCtrls.ptTextInfo = &UI_GetTextFromID(idInfo);
                tCtrls.ptLongMessageInfo = &UI_GetTextFromID(idLongMsg);
                tCtrls.ptWndIcon = &UI_GetWindowFromID(idIcon);
                tCtrls.ptWndSmallIcon = &UI_GetWindowFromID(idSmallIcon);
                UIID idBuff;
                idBuff.SetFromParent(idEntry, "Buff");
                tCtrls.ptBuffWindow = &UI_GetWindowFromID(idBuff);
                UIID idDebuff;
                idDebuff.SetFromParent(idEntry, "Debuff");
                tCtrls.ptDebuffWindow = &UI_GetWindowFromID(idDebuff);
            }
        }
    }
}

void ProcessItem(const Player * ptPlayer, const ActorCommonData & tACDItem);


void PushNextItem(int32 nPlayerIndex)
{
    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[nPlayerIndex];

    tEvents.bWaitForStoryboard = FALSE;

    if (tEvents.nBacklogCount > 0)
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
        ActorCommonData * ptACD = ACDTryToGet(tEvents.aItemBacklog[0].idItem);
        if (ptPlayer && ptACD)
        {
            ProcessItem(ptPlayer, *ptACD);
        }

        tEvents.aItemBacklog.erase(tEvents.aItemBacklog.begin());
        tEvents.nBacklogCount--;
        tEvents.nNextProcessTime = CGameGetRealTicks() + DELAY_BETWEEN_BACKLOG_ITEMS;
    }
    else
    {
        tEvents.nNextProcessTime = CGameGetRealTicks() + DELAY_BETWEEN_BACKLOG_PURGES;
    }
}





void sPlayFlash(const int32 nPlayerIndex)
{
    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    if (ptPlayer)
    {
        int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);
        UIID idFlashParent = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.CoopLootFX", nHudSlot + 1));
        UI_GetWindowFromID(idFlashParent).SetActive(TRUE);
        UIID idFlash = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.CoopLootFX.Flash", nHudSlot + 1));
        UI_GetRActorFromID(idFlash).SetActive(TRUE);

        PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[nPlayerIndex];
        tEvents.nFlashActivationTime = CGameGetRealTicks();
    }
}





void sStoryboardEndCallback_P1(const UIID & idCaller)
{
    sPlayFlash(0);
    PushNextItem(0);
}


void sStoryboardEndCallback_P2(const UIID & idCaller)
{
    sPlayFlash(1);
    PushNextItem(1);
}


void sStoryboardEndCallback_P3(const UIID & idCaller)
{
    sPlayFlash(2);
    PushNextItem(2);
}


void sStoryboardEndCallback_P4(const UIID & idCaller)
{
    sPlayFlash(3);
    PushNextItem(3);
}

void AddCriticalMessage(int32 nPlayerIndex, CriticalMessage & tCriticalMessage);



void sUpdateForPlayer(
    const Player * ptPlayer,
    const int32 nMaxShown,
    const UIID & idHUDRoot)
{
    int32 nPlayerIndex = ptPlayer->nPlayerIndex;
    ValidateUIControls(nPlayerIndex, idHUDRoot);

    int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);

    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[nPlayerIndex];
    if (!tEvents.bInitialized)
    {
        tEvents.idStoryboard = UIIDCreateFromParent(UIWND_CONSOLE_STORYBOARD_COOP_LOOT, CRefString().Format(".CoOpLootAnimP%d", nHudSlot + 1));

        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(tEvents.idStoryboard);

        switch (nPlayerIndex)
        {
        case 0:
            tStoryboard.SetOnEndCallback(sStoryboardEndCallback_P1);
            break;
        case 1:
            tStoryboard.SetOnEndCallback(sStoryboardEndCallback_P2);
            break;
        case 2:
            tStoryboard.SetOnEndCallback(sStoryboardEndCallback_P3);
            break;
        case 3:
            tStoryboard.SetOnEndCallback(sStoryboardEndCallback_P4);
            break;
        }






        tEvents.bInitialized = TRUE;
    }

    if (tEvents.nFlashActivationTime > 0 && tEvents.nFlashActivationTime + FLASH_DURATION < CGameGetRealTicks())
    {
        UIID idFlashParent = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.CoopLootFX", nHudSlot + 1));
        UI_GetWindowFromID(idFlashParent).SetActive(FALSE);

        UIID idFlash = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.CoopLootFX.Flash", nHudSlot + 1));
        UI_GetRActorFromID(idFlash).SetActive(FALSE);

        tEvents.nFlashActivationTime = 0;
    }

    uint32 dwCurrTime = CGameGetRealTicks();

    if (!::Console::Screens::IsScreenSetOpen() && !::Console::Screens::IsModalDialogVisible() &&

        !tEvents.bHidden && dwCurrTime > tEvents.nNextCriticalMessageProcessTime)
    {
        if (tEvents.arCriticalMessages.size() > 0)
        {
            AddCriticalMessage(nPlayerIndex, tEvents.arCriticalMessages[0]);
            blzext::vector_erase_at(tEvents.arCriticalMessages, 0);
        }
        else if (tEvents.bShowingCriticalEvent)
        {
            tEvents.bShowingCriticalEvent = FALSE;
        }
    }


    if (tEvents.nBacklogCount == 0)
    {
        if (tEvents.aItemBacklog.size() > 0 && dwCurrTime > tEvents.nNextProcessTime)
        {
            tEvents.nBacklogCount = tEvents.aItemBacklog.size();
            tEvents.bWaitForStoryboard = TRUE;

            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(tEvents.idStoryboard);

            tStoryboard.Play(FALSE, 0.0f);

            if (nHudSlot % 2 == 0)
            {
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_COOP_LOOT_LEFT), cgtPSODefaults);
            }
            else
            {
                SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_COOP_LOOT_RIGHT), cgtPSODefaults);
            }
        }
    }
    else
    {
        if (!tEvents.bWaitForStoryboard)
        {
            if (dwCurrTime >= tEvents.nNextProcessTime)
            {
                PushNextItem(nPlayerIndex);
            }
        }
    }
    // 626: nNumToShow (not tEvents.nNumEvents) as the left operand is inferred: only this form gives the original cset/and/add
    int32 nNumToShow = Math::Min(tEvents.nNumEvents, nMaxShown);

    if (tEvents.bShowingCriticalEvent && nNumToShow < nMaxShown)
    {
        nNumToShow++;
    }

    if (tEvents.bHidden)
    {
        nNumToShow = 0;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);

    if (ptACD && !ptACD->IsAlive())
    {
        nNumToShow = 0;
    }

    float ftBaseOfQuestUI = 0.0f;
    if (nHudSlot == 0)
    {
        UIWindow & tWnd = UI_GetWindowFromID(UIWND_OBJHUD);

        Rect2D tRect;
        tWnd.GetWindowRect(tRect);
        ftBaseOfQuestUI = tRect.bottom;
    }

    for (int32 i = 0; i < MAX_SHOWN_EVENTS; i++)
    {
        PlayerControls & tCtrls = sgptEventQueueGlobals->tPlayerControlsArray[nPlayerIndex][i];
        if (!tCtrls.ptWndEntry)
        {
            continue;
        }

        if (i >= nNumToShow)
        {
            tCtrls.ptWndEntry->SetActive(FALSE);
            continue;
        }

        if (nHudSlot == 0)
        {
            Rect2D tRect;
            tCtrls.ptWndEntry->GetWindowRect(tRect);
            if (tRect.top < ftBaseOfQuestUI)
            {
                tCtrls.ptWndEntry->SetActive(FALSE);
                continue;
            }
        }

        EventEntry & tEntry = tEvents.tEntries[i];
        uint32 dwElapsedTime = dwCurrTime - tEntry.dwEnteredTime;
        if (dwElapsedTime > TIME_BEFORE_FADING + FADE_TIME)
        {
            tCtrls.ptWndEntry->SetActive(FALSE);
            continue;
        }

        tCtrls.ptWndEntry->SetActive(TRUE);
        // DWARF lexical block 690-700 holds bAlpha: a bare scope (inferred); 691-699 if/else order inferred from the
        // original block layout (255 on the fall-through path); 697:55/91 and 698:40/48 columns fit functional casts
        {
            uint8 bAlpha;
            if (dwElapsedTime < TIME_BEFORE_FADING)
            {
                bAlpha = 255;
            }
            else
            {
                float flAlpha = 1.0f - float(dwElapsedTime - TIME_BEFORE_FADING) / FADE_TIME;
                bAlpha = uint8(flAlpha * 255.0f);
            }
            tCtrls.ptWndEntry->SetTransparency(bAlpha);
        }

        if (tEntry.eType == EVENT_TYPE_BUFF ||
            tEntry.eType == EVENT_TYPE_DEBUFF ||
            tEntry.eType == EVENT_TYPE_LONG_MESSAGE)
        {
            tCtrls.ptLongMessageInfo->SetText(tEntry.sDetails, FALSE);
            tCtrls.ptTextName->SetActive(FALSE);
            tCtrls.ptTextInfo->SetActive(FALSE);
            tCtrls.ptLongMessageInfo->SetActive(TRUE);
        }
        else
        {
            tCtrls.ptTextName->SetText(tEntry.sName, FALSE);
            tCtrls.ptTextInfo->SetText(tEntry.sDetails, FALSE);
            tCtrls.ptTextName->SetActive(TRUE);
            tCtrls.ptTextInfo->SetActive(TRUE);
            tCtrls.ptLongMessageInfo->SetActive(FALSE);
        }

        tCtrls.ptBuffWindow->SetActive(FALSE);
        tCtrls.ptDebuffWindow->SetActive(FALSE);

        if (tEntry.tImage.snoTexture != SNO_NONE)
        {
            if (tEntry.bUseLargeIcon)
            {
                tCtrls.ptWndIcon->SetTextureFrame(tEntry.tImage);
                tCtrls.ptWndIcon->SetActive(TRUE);
                tCtrls.ptWndSmallIcon->SetActive(FALSE);
            }
            else
            {
                tCtrls.ptWndSmallIcon->SetTextureFrame(tEntry.tImage);
                tCtrls.ptWndSmallIcon->SetActive(TRUE);
                tCtrls.ptWndIcon->SetActive(FALSE);
            }

            if (tEntry.eType == EVENT_TYPE_BUFF)
            {
                tCtrls.ptBuffWindow->SetActive(TRUE);
            }
            else if (tEntry.eType == EVENT_TYPE_DEBUFF)
            {
                tCtrls.ptDebuffWindow->SetActive(TRUE);
            }
        }
        else
        {
            tCtrls.ptWndIcon->SetActive(FALSE);
            tCtrls.ptWndSmallIcon->SetActive(FALSE);
        }
    }
}




void UpdateForPlayer(const Player * ptPlayer)
{
    if (ptPlayer && ptPlayer->idPlayerACD != ACDID_INVALID)
    {
        int32 nLocalPlayerCount = PlayerGetLocalPlayerCount();
        uint32 nItemsToShow = (nLocalPlayerCount <= 2 && PlayerIsLocal(ptPlayer)) ? MAX_SHOWN_EVENTS : MAX_SHOWN_EVENTS_MP;
        int32 nHudSlot = UIHUD::Console::PlayerGetHUDSlot(ptPlayer);
        UIID idHUDRoot = UIGame::GetPlayerHUDID(nHudSlot);
        sUpdateForPlayer(ptPlayer, nItemsToShow, idHUDRoot);
    }
}









inline BOOL sShouldAddEntryForPlayer(const Player * ptPlayer)
{


    PlayerIsLocal(ptPlayer);





    if (CGameLoadingScreenIsEnabled())
    {
        return FALSE;
    }

    if (::Console::Screens::IsScreenSetOpen())
    {
        return FALSE;
    }

    return TRUE;
}




inline void sMakeSpaceInQueue(PlayerEventQueue & tEvents)
{

    tEvents.nNumEvents = Math::Min(tEvents.nNumEvents + 1, (int32)MAX_SHOWN_EVENTS);
    int32 nFirst = tEvents.bShowingCriticalEvent ? 1 : 0;


    for (int32 i = MAX_SHOWN_EVENTS - 1; i > nFirst; i--)
    {
        tEvents.tEntries[i] = tEvents.tEntries[i - 1];
    }
}

































inline BOOL ItemIsFullyIdentified(const ActorCommonData & tACDItem)
{



    ItemQualityLevel eIQL = ItemGetQualityLevel(tACDItem);
    if (eIQL >= IQL_AFFIX_COUNT_START && eIQL <= IQL_AFFIX_COUNT_END)
    {
        if (!ItemIsUnidentified(tACDItem))
        {
            if (tACDItem.listIdentifiedAffixGBIDs.Count() == 0)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}





void ProcessItem(const Player * ptPlayer, const ActorCommonData & tACDItem)
{
    ImageHandle hImage;
    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
    sMakeSpaceInQueue(tEvents);

    EventEntry & tNewEntry = tEvents.tEntries[0];
    tNewEntry.dwEnteredTime = CGameGetRealTicks();
    tNewEntry.eType = EVENT_TYPE_ITEM;
    tNewEntry.bUseLargeIcon = TRUE;
    hImage = GetImageForItem(tACDItem);
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tNewEntry.tImage);
    tNewEntry.sName = ItemGetDisplayName(tACDItem, TRUE, FALSE, TRUE, TRUE);

    CRefString szComparison = "";
    if (ItemIsEquippable(tACDItem))
    {
        CRefString sValEHP;
        CRefString sValDmg;
        CRefString sValHealing;
        ActorCommonData * tACDPlayer = &ACDGet(ptPlayer->idPlayerACD);
        UIInventory::GetComparisonValueStrings(tACDPlayer, tACDItem, FALSE, FALSE, sValEHP, sValDmg, sValHealing);
        szComparison = LocalizedSprintf(StringListFetch("Console:ItemPickupEvent"), sValDmg.str(), sValEHP.str(), sValHealing.str());

        int32 nSocketCount = tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS));
        if (nSocketCount > 0)
        {
            szComparison += CRefString(" ") + LocalizedSprintf(StringListFetch("Console:SocketedItemPickupEvent"), UnicodeText::FormatNumber(nSocketCount, TRUE, FNS_NORMAL, (XLocale)0).str());
        }

        int32 nSpecialAffixes = ItemGetSpecialAffixCount(tACDItem, PlayerGetPlayerClass(ptPlayer)) - (nSocketCount > 0 ? 1 : 0);
        if (nSpecialAffixes > 0)
        {
            szComparison += CRefString(" ") + LocalizedSprintf(StringListFetch("Console:SpecialItemPickupEvent"), UnicodeText::FormatNumber(nSpecialAffixes, TRUE, FNS_NORMAL, (XLocale)0).str());
        }
    }
    tNewEntry.sDetails = szComparison;

    if (ItemIsSocketable(tACDItem))
    {
        UIID idEntry = UIIDCreateFromParent(UIWND_CONSOLE_HUD_PLAYER_SLOTS, CRefString().Format(".Player%d.Local.EventList.EventSlot01", UIHUD::Console::PlayerGetHUDSlot(ptPlayer) + 1));
        gtTutorialMgr.ShowTutorial(idEntry, GlobalSNOGet(GLOBALSNO_TUTORIAL_GEMPICKEDUP), ptPlayer, TRUE);
    }

    if (tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_TIERED_LOOT_RUN_KEY_LEVEL)) >= 0)
    {
        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_TIERED_LOOT_RUN_KEY_INVENTORY), ptPlayer, TRUE);
    }
}

















void ReportItemPickup(const Player * ptPlayer, const ActorCommonData & tACDItem)
{
    if (!sShouldAddEntryForPlayer(ptPlayer))
    {
        return;
    }

    if (!ItemIsFullyIdentified(tACDItem))
    {
        return;
    }

    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
    if (PlayerGetLocalPlayerCount() >= 2)
    {
        BacklogItem tInfo;
        tInfo.idItem = tACDItem.id;
        tEvents.aItemBacklog.push_back(tInfo);
    }
    else
    {
        ProcessItem(ptPlayer, tACDItem);
    }
}








void ReportBuff(const Player * ptPlayer, const CRefString & sBuff, const ImageHandle & hImage, BOOL bHarmful)
{
    if (!sShouldAddEntryForPlayer(ptPlayer))
    {
        return;
    }






    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
    sMakeSpaceInQueue(tEvents);

    EventEntry & tNewEntry = tEvents.tEntries[0];

    tNewEntry.dwEnteredTime = CGameGetRealTicks();

    tNewEntry.eType = bHarmful ? EVENT_TYPE_DEBUFF : EVENT_TYPE_BUFF;
    tNewEntry.sName = "";
    tNewEntry.sDetails = sBuff;
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tNewEntry.tImage);
    tNewEntry.bUseLargeIcon = FALSE;
}

void AddCriticalMessage(int32 nPlayerIndex, CriticalMessage & tCriticalMessage)
{
    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[nPlayerIndex];
    EventEntry & tNewEntry = tEvents.tEntries[0];

    if (tNewEntry.eType != EVENT_TYPE_CRITICAL_MESSAGE)
    {
        sMakeSpaceInQueue(tEvents);
    }

    tEvents.tCurrentCriticalMessage.sMessage = tCriticalMessage.sMessage;
    tEvents.tCurrentCriticalMessage.sMessage2 = tCriticalMessage.sMessage2;
    tEvents.tCurrentCriticalMessage.tImage = tCriticalMessage.tImage;
    tEvents.nNextCriticalMessageProcessTime = CGameGetRealTicks() + DELAY_BETWEEN_CRITICAL_ITEMS;
    tEvents.bShowingCriticalEvent = TRUE;

    tNewEntry.dwEnteredTime = CGameGetRealTicks();
    tNewEntry.eType = EVENT_TYPE_CRITICAL_MESSAGE;
    tNewEntry.sName = tCriticalMessage.sMessage;
    tNewEntry.sDetails = tCriticalMessage.sMessage2;
    tNewEntry.tImage = tCriticalMessage.tImage;
    tNewEntry.bUseLargeIcon = FALSE;
}









inline BOOL CurrentlyDisplayingMessage(const CRefString & sMessage2, const ImageTextureFrame & tImage, const Player * ptPlayer)
{
    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
    uint32 nItemsToShow = (PlayerGetLocalPlayerCount() <= 2 && PlayerIsLocal(ptPlayer)) ? MAX_SHOWN_EVENTS : MAX_SHOWN_EVENTS_MP;
    uint32 dwCurrTime = CGameGetRealTicks();

    for (uint n = 0; n < nItemsToShow; n++)
    {
        EventEntry & tEntry = tEvents.tEntries[n];
        if (dwCurrTime - tEntry.dwEnteredTime <= TIME_BEFORE_FADING + FADE_TIME &&
            tEntry.sDetails == sMessage2 &&
            tEntry.tImage.snoTexture == tImage.snoTexture &&
            tEntry.tImage.nFrame == tImage.nFrame)
        {
            return TRUE;
        }
    }

    return FALSE;
}






void DisplayMessageForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageHandle hImage, CRefString * psMessageLine2, BOOL bUseLargeIcon)
{
    if (psMessageLine2)
    {
        ImageTextureFrame tImage;
        ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tImage);
        if (CurrentlyDisplayingMessage(*psMessageLine2, tImage, ptPlayer))
        {
            return;
        }
    }

    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
    sMakeSpaceInQueue(tEvents);

    EventEntry & tNewEntry = tEvents.tEntries[0];
    tNewEntry.dwEnteredTime = CGameGetRealTicks();
    tNewEntry.eType = EVENT_TYPE_MESSAGE;
    tNewEntry.sName = sMessage;
    tNewEntry.bUseLargeIcon = bUseLargeIcon;
    if (psMessageLine2)
    {
        tNewEntry.sDetails = *psMessageLine2;
    }
    else
    {
        tNewEntry.sDetails = "";
    }
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, hImage, tNewEntry.tImage);
}















void DisplayItemEquippedMessageForPlayer(const Player * ptPlayer, const ActorCommonData & tACDItem)
{
    if (!ItemIsFullyIdentified(tACDItem))
    {
        return;
    }





    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
    sMakeSpaceInQueue(tEvents);

    EventEntry & tNewEntry = tEvents.tEntries[0];
    tNewEntry.dwEnteredTime = CGameGetRealTicks();
    tNewEntry.eType = EVENT_TYPE_MESSAGE;
    tNewEntry.sName = LocalizedSprintf(StringListFetch("ConsoleUI:EventQueueEquipped"));
    tNewEntry.sDetails = ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE);
    tNewEntry.bUseLargeIcon = TRUE;
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, GetImageForItem(tACDItem), tNewEntry.tImage);
}






void DisplayMessageWithIconForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tImage, CRefString * psMessageLine2, BOOL bUseLargeIcon, BOOL bCritical)
{
    if (bCritical)
    {
        CriticalMessage tCriticalMessage;
        tCriticalMessage.sMessage = sMessage;
        tCriticalMessage.sMessage2 = psMessageLine2 ? *psMessageLine2 : CRefString("");
        tCriticalMessage.tImage = tImage;
        sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex].arCriticalMessages.push_back(tCriticalMessage);
    }
    else
    {
        if (psMessageLine2 && CurrentlyDisplayingMessage(*psMessageLine2, tImage, ptPlayer))
        {
            return;
        }

        PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
        sMakeSpaceInQueue(tEvents);

        EventEntry & tNewEntry = tEvents.tEntries[0];
        tNewEntry.dwEnteredTime = CGameGetRealTicks();
        tNewEntry.eType = EVENT_TYPE_MESSAGE;
        tNewEntry.sName = sMessage;
        tNewEntry.bUseLargeIcon = bUseLargeIcon;
        if (psMessageLine2)
        {
            tNewEntry.sDetails = *psMessageLine2;
        }
        else
        {
            tNewEntry.sDetails = "";
        }
        tNewEntry.tImage = tImage;
    }
}


















void DisplaySeasonEndingMessage(int32 nMin)
{
    Player * ptPlayer = LocalPlayerGetPrimary();
    if (ptPlayer)
    {
        CRefString sTextureName = CRefString().Format("UI:ConsoleSeasonJourneyRingLeaf");
        DisplayMessageWithIconForPlayer(LocalizedSprintf(StringListFetch("Console:SeasonEndingWarning"), UnicodeText::FormatNumber(nMin, TRUE, FNS_NORMAL, (XLocale)0).str()), ptPlayer, ImageTextureFrame(sTextureName), NULL, FALSE, TRUE);
    }
}










void DisplayLongMessageForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tImage, BOOL bUseLargeIcon)
{
    if (CurrentlyDisplayingMessage(sMessage, tImage, ptPlayer))
    {
        return;
    }

    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[ptPlayer->nPlayerIndex];
    sMakeSpaceInQueue(tEvents);

    EventEntry & tNewEntry = tEvents.tEntries[0];
    tNewEntry.dwEnteredTime = CGameGetRealTicks();
    tNewEntry.eType = EVENT_TYPE_LONG_MESSAGE;
    tNewEntry.sName = "";
    tNewEntry.bUseLargeIcon = bUseLargeIcon;
    tNewEntry.sDetails = sMessage;
    tNewEntry.tImage = tImage;
}
















void HideForPlayerIndex(int32 nPlayerIndex)
{
    if (nPlayerIndex != -1)
    {
        PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[nPlayerIndex];
        if (tEvents.bShowingCriticalEvent && CGameGetRealTicks() - tEvents.nNextCriticalMessageProcessTime < REMAINING_TIME_TO_DISPLAY_ON_RESTORE)
        {
            tEvents.bShowingCriticalEvent = FALSE;
            tEvents.nNextCriticalMessageProcessTime = 0;
        }
        tEvents.nNumEvents = 0;
        tEvents.bHidden = TRUE;
    }
}









void UnhideForPlayerIndex(int32 nPlayerIndex)
{
    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[nPlayerIndex];
    tEvents.bHidden = FALSE;
    if (tEvents.bShowingCriticalEvent)
    {
        AddCriticalMessage(nPlayerIndex, tEvents.tCurrentCriticalMessage);
    }
}






void InvalidateForPlayer(int32 nPlayerIndex)
{
    PlayerEventQueue & tEvents = sgptEventQueueGlobals->tPlayerQueue[nPlayerIndex];
    tEvents.bInitialized = FALSE;
}

}  // namespace Console
}  // namespace UIEventQueue
