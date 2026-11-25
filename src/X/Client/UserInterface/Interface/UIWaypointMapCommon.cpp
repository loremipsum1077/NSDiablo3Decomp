#include "UIWaypointMapCommon.h"
#include "UIWaypointsCommon.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIBlinkerControl.h"
#include "../Controls/UIStackPanelControl.h"
#include "../Controls/UITextControl.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Act.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/GameTemplate.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Definitions/ActDefinition.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/Quests/QuestsSwig.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Quests/Quests.h"
#include "../../RActors/RActors.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Prefetch.h"
#include "../../SNOManager.h"
#include "../../../Common/SNO/SymbolicValue.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "UITutorials.h"
// Declared here because their headers are not adopted yet; each comment names the defining file.
struct CachedWaypoint {  // size 0x2c (X/Common/Act.h:13)
    Act eSourceAct;  // 0x00
    WaypointInfo tInfo;  // 0x04
};
typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray;  // X/Common/Act.h:19
const CachedWaypointArray & ActGetCachedWaypointArray();  // X/Common/Act.cpp:146
int32 ActGetWaypointByLevelArea(const SNO snoLevelArea, BOOL bCheckActivationState);  // X/Common/Act.cpp:156
SNO QuestGetActiveBountyForLevelArea(SNO snoLevelArea, BOOL bCheckAdditionalLevelAreas);  // X/Common/Quests/Quests.cpp
BOOL QuestIsComplete(const QuestInstance & tQuest); CRefString QuestsGetUIName(SNO snoQuest);  // Quests.cpp
BOOL LevelAreaIsTown(SNO snoLevelArea);  // LevelAreaDefinition.cpp:463
void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor);  // X/Common/UserInterface/UITextControl.cpp:26
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
extern UIID UIDLG_WAYPOINTMAP_PAGE, UIWND_WAYPOINTMAP_WORLDMAP, UIWND_WAYPOINTMAP_OVERLAY_POI;  // 0x71017d90f0, 0x71017d9110, 0x71017d9108
extern UIID UIWND_WAYPOINTMAP_OVERLAY_BOUNTY, UIWND_WAYPOINTMAP_OVERLAY_BOUNTY_LIST, UITXT_WAYPOINTMAP_TITLE;  // 0x71017d9118, 0x71017d9120, 0x71017d9130
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot);  // X/Client/UserInterface/UI.cpp
extern UITutorialManager gtTutorialMgr; CRefString DisplayActName(Act eAct);  // UIUtil.cpp:3036
#include "../../../Common/Shell/CmdLine.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIItemButtonControl.h"
#include "../../../Common/Items.h"
UIWaypointMap::TutorialState::TutorialState()
{
    bOpenForTutorial = FALSE;
    eMapTutorialState = MAP_TUTORIAL_STATE_INVALID;
    idTutorialSound = SOUNDID_INVALID;
    dwTickNextTutorialAdvance = 0;
}


BOOL UIWaypointMap::IsOpen()
{

    return UI_TestID( UIDLG_WAYPOINTMAP_PAGE ) &&
           UI_GetWindowFromID( UIDLG_WAYPOINTMAP_PAGE ).IsActive();
}

// DWARF: X/Common/GameCommonData.cpp:380.
Act ActEnumFromActIndex(const int32 nIndex);
// DWARF: X/Client/UserInterface/Console/UIConsoleWaypointMap.cpp:124.
namespace UIWaypointMap { Act GetWaypointActForPlayer(const Player * ptPlayer); }
// DWARF: X/Common/Player.cpp:5188.
BOOL PlayersAreHostileToEachOther(const Player * ptPlayer0, const Player * ptPlayer1);
// DWARF: X/Client/CPlayer.cpp:1576.
void CPlayerGetPortraitImage(const Player * ptPlayer, ImageTextureFrame & tFrame, BOOL bUseReversedPortrait);
// DWARF: X/Common/Quests/Quests.cpp:2420.
BOOL QuestGetFirstStepReward(const Player & tPlayer, SNO snoQuest, uint64 & nXPGranted, int32 & nGoldGranted, SNO & snoRecipe, SNO & snoTreasureClass);
// DWARF: X/Client/UserInterface/UI.cpp:2503.
UIItemButton & UI_GetItemButtonFromID(const UIID & id);

void UIWaypointMap::ShowActMap( Act eAct )
{

    UI_GetWindowFromID( UIDLG_WAYPOINTMAP_PAGE ).SetActive( TRUE );
    UI_GetWindowFromID( UIWND_WAYPOINTMAP_WORLDMAP ).SetActive( FALSE );

    UIWindow & tOverlayWindow = UI_GetWindowFromID( UIWND_WAYPOINTMAP_OVERLAY_POI );
    tOverlayWindow.SetActive( TRUE );



    LPCSTR szTextureBase = "UI:Console_WaypointMap_ACT0%d";

    tOverlayWindow.SetTextureFrame( CRefString().Format( szTextureBase, eAct / 100 + 1 ) );


    SNOName snonameTexture( TEXTURE_SNO, tOverlayWindow.GetTextureFrame().snoTexture );
    if ( !Prefetch::AddToQueue( snonameTexture, 2 ) )
    {
        SNONameArray textures;
        textures.push_back( snonameTexture );

        SNOManager::WaitForSNOs( textures, NULL, FALSE );
    }


    CRefString sTitle;
    if ( GameIsOpenWorld() )
    {
        sTitle = StringListFetch( CRefString().Format( "WaypointMap:%sOpenWorldModeTitle", SymbolToName( eAct, gActs ) ) );
    }
    else
    {
        sTitle = StringListFetch( CRefString().Format( "WaypointMap:%sStoryModeTitle", SymbolToName( eAct, gActs ) ) );
    }

    UI_GetTextFromID( UITXT_WAYPOINTMAP_TITLE ).SetText( sTitle, FALSE );
}




static void sClearWaypointControls()
{


    UIWindow & tOverlayWindow = UI_GetWindowFromID( UIWND_WAYPOINTMAP_OVERLAY_POI );
    const UIControlItemArray & arChildren = tOverlayWindow.GetChildList();

    while ( arChildren.size() > 0 )
    {
        const UIID & idChild = arChildren[arChildren.size() - 1].id;
        if ( UI_TestID( idChild ) )
        {
            UI_DeleteControl( idChild, TRUE );
        }
    }
}
#include "../../../Common/Game.h"
// DWARF: X/Common/Act.cpp:214.
Act ActGetWaypointActByLevelArea(const SNO snoLevelArea);
// DWARF: X/Client/UserInterface/Console/UIConsoleWaypointMap.cpp:1330.
namespace UIWaypointMap { void GetBountyCountForAct(Act eAct, int32 & nBountyCount, int32 & nCompletedBountyCount); }
#include "../../Definitions/SoundDefinition.h"
#include "../Console/ControllerActions.h"
#include "../Console/ConsoleScreenManager.h"
#include "../../../Common/Definitions/HeroDefinition.h"
// DWARF: UIConsoleWaypointMap.cpp:1310,1321,1457, respectively.
namespace UIWaypointMap { void SetupControlsForAct(Act eAct); void SetupControlsForWorldView(); void ShowTutorialText(const CRefString & sText); }


void UIWaypointMap::ShowWorldMap()
{

    UI_GetWindowFromID( UIDLG_WAYPOINTMAP_PAGE ).SetActive( TRUE );
    UI_GetWindowFromID( UIWND_WAYPOINTMAP_WORLDMAP ).SetActive( TRUE );
    UI_GetWindowFromID( UIWND_WAYPOINTMAP_OVERLAY_POI ).SetActive( FALSE );
    UI_GetWindowFromID( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY ).SetActive( FALSE );

    sClearWaypointControls();


    CRefString sTitle;
    if ( GameIsOpenWorld() )
    {
        sTitle = StringListFetch( "WaypointMap:OpenWorldModeTitle" );
    }
    else
    {
        sTitle = StringListFetch( "WaypointMap:StoryModeTitle" );
    }

    UI_GetTextFromID( UITXT_WAYPOINTMAP_TITLE ).SetText( sTitle, FALSE );


    gtTutorialMgr.CloseTutorialIfOpen( GlobalSNOGet( GLOBALSNO_TUTORIAL_WORLDMAPOPEN ) );
}



void UIWaypointMap::SetupWorldControls(
    Player * ptPlayerViewing)
{
    Act eCurrentAct = GetWaypointActForPlayer( ptPlayerViewing );

    // Inference: original loop starts at zero and terminates at five act indices.
    for ( int32 i = 0; i < 5; i++ )
    {
        Act eAct = ActEnumFromActIndex( i );
        CRefString sActWindow = CRefString().Format( ".Act%dOpen", i + 1 );

        UIID idActWindow = UIIDCreateFromParent( UIWND_WAYPOINTMAP_WORLDMAP, sActWindow.str() );
        UIWindow & tActWindow = UI_GetWindowFromID( idActWindow );
        tActWindow.SetUserData( (void *)(intptr_t)eAct );
        // Inference: DWARF initial TRUE and bit38/index4 tests hide the last act at BlizzCon.
        BOOL fActVisible = TRUE;
        if ( CmdLineGetParams()->tBitArray.Get( CMDLINE_BLIZZCON_BIT ) &&
             i == 4 )
        {
            fActVisible = FALSE;
        }
        tActWindow.SetActive( fActVisible );

        UIID idSelected;
        idSelected.SetFromParent( tActWindow.GetID(), "LayoutRoot.Selected" );
        UIWindow & tWindowSelected = UI_GetWindowFromID( idSelected );

        if ( eCurrentAct == eAct )
        {
            tWindowSelected.SetActive( TRUE );
        }
        else
        {
            tWindowSelected.SetActive( FALSE );
        }
        // Inference: original increment precedes formatting; remaining slots are cleared below.
        int32 nPlayersInAct = 0;
        for ( const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll( ptPlayer ) )
        {
            if ( PlayersAreHostileToEachOther( ptPlayer, ptPlayerViewing ) )
            {
                continue;
            }

            if ( GetWaypointActForPlayer( ptPlayer ) == eAct )
            {
                UIID idPlayerIndicator = UIIDCreateFromParent( idActWindow, CRefString().Format( ".LayoutRoot.P%d", ++nPlayersInAct ) );
                UIButton & tButtonPlayerIndicator = UI_GetButtonFromID( idPlayerIndicator );
                tButtonPlayerIndicator.SetActive( TRUE );
                ImageTextureFrame tTexFrame;
                CPlayerGetPortraitImage( ptPlayer, tTexFrame, FALSE );
                tButtonPlayerIndicator.SetAllTextureFrames( tTexFrame );
                tButtonPlayerIndicator.SetUserData( (void *)(intptr_t)ptPlayer->nPlayerIndex );
            }
        }



        // Inference: four portrait slots, one-based names; j has no surviving DWARF location.
        for ( int32 j = nPlayersInAct; j < 4; j++ )
        {
            UIID idPlayerIndicator = UIIDCreateFromParent( idActWindow, CRefString().Format( ".LayoutRoot.P%d", j + 1 ) );
            UI_GetButtonFromID( idPlayerIndicator ).SetActive( FALSE );
        }
    }
}
// DWARF: X/Common/GameCommonData.cpp:395.
int32 ActIndexFromActEnum(const Act eAct);


void UIWaypointMap::UpdateWorldControls(
    Player * ptPlayerViewing,
    BOOL bHighlightActs)
{
    Act eCurrentAct = GetWaypointActForPlayer( ptPlayerViewing );


    UIWindow & tWorldMapWindow = UI_GetWindowFromID( UIWND_WAYPOINTMAP_WORLDMAP );
    // Inference: the original iterates five act indices from zero.
    for ( int32 i = 0; i < 5; i++ )
    {
        Act eAct = ActEnumFromActIndex( i );
        CRefString sActWindow = CRefString().Format( "Act%dOpen", i + 1 );

        UIID idActWindow;
        idActWindow.SetFromParent( tWorldMapWindow.GetID(), sActWindow.str() );
        // Inference: the lexical rift-icon block is unconditional in this binary.
        GameCommonData & tGCD = GameGetActiveGameCommonData();
        {
            UIID idLootRun = UIIDCreateFromParent( idActWindow, ".LayoutRoot.Interest.LootRun" );
            if ( ActGetWaypointActByLevelArea( tGCD.tGameSyncedData.snoDungeonFinderLevelArea ) == eAct )
            {
                UI_GetWindowFromID( idLootRun ).SetActive( TRUE );
                UI_GetWindowFromID( idLootRun ).SetUserData( (void *)(intptr_t)tGCD.tGameSyncedData.snoDungeonFinderLevelArea );
            }
            else
            {
                UI_GetWindowFromID( idLootRun ).SetActive( FALSE );
                UI_GetWindowFromID( idLootRun ).SetUserData( NULL );
            }
        }

        UIID idSelected = UIIDCreateFromParent( idActWindow, ".LayoutRoot.Selected" );
        UIWindow & tWindowSelected = UI_GetWindowFromID( idSelected );

        UIID idBlinker = UIIDCreateFromParent( idActWindow, ".LayoutRoot.Callout" );
        // Inference: highlight only the current act; the other branch explicitly clears it.
        if ( eCurrentAct == eAct )
        {
            tWindowSelected.SetActive( TRUE );
            UI_GetBlinkerFromID( idBlinker ).SetActive( bHighlightActs );
        }
        else
        {
            tWindowSelected.SetActive( FALSE );
            UI_GetBlinkerFromID( idBlinker ).SetActive( FALSE );
        }
        // Inference: preincrement produces one-based portrait slot names, as in SetupWorldControls.
        int32 nPlayersInAct = 0;
        for ( const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll( ptPlayer ) )
        {
            if ( PlayersAreHostileToEachOther( ptPlayer, ptPlayerViewing ) )
            {
                continue;
            }

            if ( GetWaypointActForPlayer( ptPlayer ) == eAct )
            {
                UIID idPlayerIndicator;
                idPlayerIndicator.SetFromParent( idActWindow, CRefString().Format( "LayoutRoot.P%d", ++nPlayersInAct ) );
                UIButton & tButtonPlayerIndicator = UI_GetButtonFromID( idPlayerIndicator );
                tButtonPlayerIndicator.SetActive( TRUE );
                ImageTextureFrame tTexFrame;
                CPlayerGetPortraitImage( ptPlayer, tTexFrame, FALSE );
                tButtonPlayerIndicator.SetAllTextureFrames( tTexFrame );
                tButtonPlayerIndicator.SetUserData( (void *)(intptr_t)ptPlayer->nPlayerIndex );
            }
        }



        // Inference: clear the remaining four portrait slots; j has no surviving location.
        for ( int32 j = nPlayersInAct; j < 4; j++ )
        {
            UIID idPlayerIndicator;
            idPlayerIndicator.SetFromParent( idActWindow, CRefString().Format( "LayoutRoot.P%d", j + 1 ) );
            UIButton & tButtonPlayerIndicator = UI_GetButtonFromID( idPlayerIndicator );
            tButtonPlayerIndicator.SetActive( FALSE );
        }

        int32 nNumBounties = 0;
        int32 nNumCompleteBounties = 0;
        GetBountyCountForAct( eAct, nNumBounties, nNumCompleteBounties );
        // Inference: a positive bounty count enables the icon; equality enables its completed overlay.
        UIID idBounty = UIIDCreateFromParent( idActWindow, ".LayoutRoot.Interest.Bounty" );
        if ( nNumBounties > 0 )
        {
            UI_GetWindowFromID( idBounty ).SetActive( TRUE );
            UI_GetWindowFromID( idBounty ).SetUserData( (void *)(intptr_t)eAct );

            UIID idBountyCompleteOverlay = UIIDCreateFromParent( idActWindow, ".LayoutRoot.Interest.Bounty.Complete" );
            UI_GetWindowFromID( idBountyCompleteOverlay ).SetActive( nNumBounties == nNumCompleteBounties );
        }
        else
        {
            UI_GetWindowFromID( idBounty ).SetActive( FALSE );
        }

        UIID idStatus = UIIDCreateFromParent( idActWindow, ".LayoutRoot.Status" );
        UIText & tTextStatus = UI_GetTextFromID( idStatus );
        tTextStatus.SetActive( FALSE );
    }
}




void UIWaypointMap::CountBountiesAtWaypoints( Act eAct, int32 & nBountyCount, int32 & nCompletedBountyCount )
{


    nBountyCount = 0;
    nCompletedBountyCount = 0;

    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    uint32 nCount = tWaypoints.size();

    for ( uint32 i = 0; i < nCount; i++ )
    {
        if ( tWaypoints[i].eSourceAct == eAct )
        {
            SNO snoLevelArea = tWaypoints[i].tInfo.snoLevelArea;

            SNO snoBounty = QuestGetActiveBountyForLevelArea( snoLevelArea, FALSE );
            if ( snoBounty != SNO_NONE )
            {
                nBountyCount++;

                QuestInstance * ptQuestInstance = QuestsGetQuestInstance( snoBounty, ACDID_INVALID );
                if ( ptQuestInstance && QuestIsComplete( *ptQuestInstance ) )
                {
                    nCompletedBountyCount++;
                }
            }
        }
    }
}





CRefString UIWaypointMap::GetBountyTooltipTextForAct( Act eAct )
{


    CRefString usTip;

    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    uint32 nCount = tWaypoints.size();

    for ( uint32 i = 0; i < nCount; i++ )
    {
        if ( tWaypoints[i].eSourceAct == eAct )
        {
            SNO snoLevelArea = tWaypoints[i].tInfo.snoLevelArea;

            SNO snoBountyQuest = QuestGetActiveBountyForLevelArea( snoLevelArea, FALSE );
            if ( snoBountyQuest == SNO_NONE )
                continue;



            ScopedQuestDefinitionLockR pdefBounty( snoBountyQuest );
            if ( pdefBounty && pdefBounty->eQuestType == QUEST_TYPE_HORADRIC_QUEST )
                continue;




            if ( usTip.Length() )
            {
                usTip += "\n";
            }


            CRefString sQuestName = QuestsGetUIName( snoBountyQuest );
            QuestInstance * ptQuestInstance = QuestsGetQuestInstance( snoBountyQuest, ACDID_INVALID );
            if ( ptQuestInstance && QuestIsComplete( *ptQuestInstance ) )
            {
                usTip += StringListFetch( "WaypointMap:Completed" );
                usTip += " ";

                Set_Text_Color( sQuestName, FONTCOLOR_GREY );
            }



            usTip += sQuestName;
        }
    }


    return usTip;
}

































































































































































































































































































































































































































































































































































































static void sClearBountyListConsoleUI()
{


    UIStackPanel & tBountyList = UI_GetStackPanelFromID( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY_LIST );
    const UIControlItemArray & arBountyControls = tBountyList.GetChildList();

    for ( uint32 i = 0; i < arBountyControls.size(); i++ )
    {
        UIWindow & tCompletedWnd = UI_GetWindowFromID( UIIDCreateFromParent( arBountyControls[i].id, ".Completed" ) );
        tCompletedWnd.SetActive( FALSE );
    }
}
















static void sClearBountyListUI()
{



    sClearBountyListConsoleUI();
}







static void sAddBountyListItemConsole( SNO snoBounty, BOOL bCompleted )
{


    if ( bCompleted )
    {


        UIStackPanel & tBountyList = UI_GetStackPanelFromID( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY_LIST );
        const UIControlItemArray & arBountyControls = tBountyList.GetChildList();

        for ( uint32 i = 0; i < arBountyControls.size(); i++ )
        {
            UIWindow & tCompletedWnd = UI_GetWindowFromID( UIIDCreateFromParent( arBountyControls[i].id, ".Completed" ) );
            if ( !tCompletedWnd.IsItselfActive() )
            {


                tCompletedWnd.SetActive( TRUE );
                break;
            }
        }
    }
}






















































static void sAddBountyListItem( SNO snoBounty, BOOL bCompleted )
{



    sAddBountyListItemConsole( snoBounty, bCompleted );
}







static void sPopulateBountyListUI( Player * ptPlayerViewing, Act eAct )
{

    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    uint32 nCount = tWaypoints.size();
    for ( uint32 i = 0; i < nCount; i++ )
    {

        const CachedWaypoint & tWaypoint = tWaypoints[i];
        if ( tWaypoint.eSourceAct != eAct )
            continue;




        if ( !GameIsAdventureMode() && !PlayerHasActivatedWaypoint( *ptPlayerViewing, i ) )
            continue;




        SNO snoBounty = QuestGetActiveBountyForLevelArea( tWaypoint.tInfo.snoLevelArea, FALSE );
        if ( snoBounty == SNO_NONE )
            continue;




        ScopedQuestDefinitionLockR pdefBounty( snoBounty );
        if ( pdefBounty && pdefBounty->eQuestType == QUEST_TYPE_HORADRIC_QUEST )
            continue;




        QuestInstance * ptQuestInstance = QuestsGetQuestInstance( snoBounty, ACDID_INVALID );
        if ( ptQuestInstance )
            sAddBountyListItem( snoBounty, QuestIsComplete( *ptQuestInstance ) );
    }
}
void UIWaypointMap::SetupBountyOverlay( Player * ptPlayerViewing, Act eAct )
{


    UIWindow & tBountyRewardWindow = UI_GetWindowFromID( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY );
    tBountyRewardWindow.SetActive( GameIsOpenWorld() );

    if ( GameIsOpenWorld() )
    {




        sClearBountyListUI();
        sPopulateBountyListUI( ptPlayerViewing, eAct );

        UIID idBountyRewardWindowActText;
        idBountyRewardWindowActText.SetFromParent( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY, "ActHeader.ActText" );
        UI_GetTextFromID( idBountyRewardWindowActText ).SetText( DisplayActName( eAct ).str(), FALSE );


        UpdateBountyOverlay( ptPlayerViewing, eAct, FALSE );
    }
}


void UIWaypointMap::UpdateBountyRewards(
    Player * ptPlayer,
    Act eAct,
    BOOL bHighlightBountyRewards)
{
    UIID idRewardBlinker;
    idRewardBlinker.SetFromParent( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY, "Callout" );
    UIID idRewardText;
    idRewardText.SetFromParent( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY, "RewardsText" );
    UIText & tRewardText = UI_GetTextFromID( idRewardText );
    tRewardText.SetActive( TRUE );
    tRewardText.SetWordWrap( TRUE );

    UIID idRewardBag;
    idRewardBag.SetFromParent( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY, "Rewards.BagReward" );
    UIItemButton & tItemButtonRewardBag = UI_GetItemButtonFromID( idRewardBag );




    UIID idRewardCurrency;
    idRewardCurrency.SetFromParent( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY, "Rewards.CurrencyReward" );
    UIItemButton & tItemButtonRewardCurrency = UI_GetItemButtonFromID( idRewardCurrency );



    UIID idBonus;
    idBonus.SetFromParent( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY, "Rewards.Bonus" );
    UIWindow & tBonusWindow = UI_GetWindowFromID( idBonus );







    int32 nActIndex = ActIndexFromActEnum( eAct );
    SNO snoRewardQuest = SNOFileLookupGUID( QUEST_SNO, CRefString().Format( "x1_AdventureMode_BountyTurnin_A%d", nActIndex + 1 ) );











    // Inference: DWARF reward-local scope survives, but the original has no guard on this block.
    {
        uint64 nXPGranted;
        int32 nGoldGranted;
        SNO snoRecipe;
        SNO snoTreasureClass;
        QuestGetFirstStepReward( *ptPlayer, snoRewardQuest, nXPGranted, nGoldGranted, snoRecipe, snoTreasureClass );

        CRefString sGoldXPReward;
        sGoldXPReward += UIText_ValueToGameCurrency( nGoldGranted, CURRENCY_GOLD );




        sGoldXPReward += "\n";



        // Inference: original variadic tail argument is a zero pointer; formatting uses normal/default locale.
        sGoldXPReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "RewardXP" ),
            UnicodeText::FormatNumber( nXPGranted, TRUE, FNS_NORMAL, XLOCALE_INVALID ).str(), NULL );

        tRewardText.SetText( sGoldXPReward, FALSE );

        tBonusWindow.SetActive( FALSE );
        tItemButtonRewardCurrency.SetActive( TRUE );













    }

    UI_GetBlinkerFromID( idRewardBlinker ).SetActive( bHighlightBountyRewards );
}




void UIWaypointMap::UpdateBountyOverlay( Player * ptPlayerViewing, Act eAct, BOOL bHighlightBountyRewards )
{



    sClearBountyListUI();
    sPopulateBountyListUI( ptPlayerViewing, eAct );

    UpdateBountyRewards( ptPlayerViewing, eAct, bHighlightBountyRewards );
}







SNO UIWaypointMap::GetQuestWaypointLevelArea( const Player * ptPlayer )
{
    SNO snoQuestWaypointLevelArea = SNO_NONE;

    if ( GameIsStoryMode() )
    {
        int32 nLevelAreaCount = 0;
        SNO snoWaypointLevelAreas[2];
        UIWaypoints::GetCurrentQuestLevelAreas( nLevelAreaCount, snoWaypointLevelAreas );


        for ( int32 i = 0; i < nLevelAreaCount; i++ )
        {
            int32 nWaypoint = ActGetWaypointByLevelArea( snoWaypointLevelAreas[i], FALSE );
            if ( nWaypoint != -1 &&
                 PlayerHasActivatedWaypoint( *ptPlayer, nWaypoint ) )
            {
                snoQuestWaypointLevelArea = snoWaypointLevelAreas[i];
                break;
            }
        }
    }


    return snoQuestWaypointLevelArea;
}




void UIWaypointMap::UpdateQuestWaypointCallout( const UIID & idWaypoint, SNO snoQuestWaypoint )
{
    SNO snoLevelArea = (SNO)(intptr_t)UI_GetControlFromID( idWaypoint ).GetUserData();

    UIID idCallout = UIIDCreateFromParent( idWaypoint, ".LayoutRoot.Callout" );
    UI_GetBlinkerFromID( idCallout ).SetActive( GameIsStoryMode() && snoLevelArea == snoQuestWaypoint );
}






static void sActivateWaypoint( const Player * ptPlayer, const ACDID idACDWaypoint, const int32 nWaypointIndex )
{


    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_WAYPOINTS_OPERATE ), cgtPSODefaults );



    TryWaypointMessage tMessage;
    tMessage.annWaypoint = ANNFromACD( idACDWaypoint );
    tMessage.nWaypoint = nWaypointIndex;
    MessageSendToServer( CMSG_TRY_WAYPOINT, &tMessage, sizeof( tMessage ), ptPlayer );


    CGameLoadingPartRequired( LSC_PLAYER_PRE_WARPED );
}





void UIWaypointMap::ActivateWaypoint( Player * ptPlayer, const int32 nWaypointIndex )
{


    RActor * ptRActorWaypoint = CPlayerGetLastUsedWaypointRActor();
    if ( ptRActorWaypoint )
    {
        sActivateWaypoint( ptPlayer, ptRActorWaypoint->idACD, nWaypointIndex );
    }
    else
    {
        DWordDataMessage tMessage;
        tMessage.dwData = nWaypointIndex;
        MessageSendToServer( CMSG_TELEPORT_TO_WAYPOINT, &tMessage, sizeof( tMessage ), ptPlayer );
    }
}





// Inference: select the female permutation only when its validated sound bank is present.
static int32 sChooseTutorialSample(
    SNO snoSound,
    BOOL bPlayerIsFemale)
{
    ScopedSoundDefinitionLockR pdefSound( snoSound );





    return ( bPlayerIsFemale &&
             pdefSound->nPermutationCount >= 2 &&
             pdefSound->ptPermutations[1].snoValidatedSoundBank != SNO_NONE );
}




static void sShowTutorialText(
    SNO snoStringList,
    BOOL bPlayerIsFemale)
{

    ScopedStringListDefinitionLockR pdefStringList( snoStringList );
    if ( pdefStringList && pdefStringList->dwStringCount )
    {


        // Inference: female entry is index one only when at least two strings exist.
        int32 nIndex = bPlayerIsFemale && pdefStringList->dwStringCount > 1;
        StringTableEntry & tEntry = pdefStringList->arStrings[nIndex];
        CRefString usText( tEntry.szText );

        UIWaypointMap::ShowTutorialText( usText );
    }
}




void UIWaypointMap::TutorialAdvance(
    Player * ptPlayer,
    UIWaypointMap::TutorialState & tTutorialState)
{
    tTutorialState.eMapTutorialState = (MapTutorialState)( tTutorialState.eMapTutorialState + 1 );
    tTutorialState.dwTickNextTutorialAdvance = (uint32)-1;
    // Inference: the original exits after incrementing past the last tutorial state.
    if ( tTutorialState.eMapTutorialState >= NUM_MAP_TUTORIAL_STATES )
    {
        return;
    }
    // Inference: player flag bit one is PLAYERFLAG_FEMALE; a null player selects male text/audio.
    BOOL bPlayerIsFemale = ptPlayer && (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_FEMALE));
    // Inference: failed audio skips to DONE through recursive advancement from CONCLUSION in every case.
    PlaySoundOptions tOptions;
    // Inference: jump-table entries map directly to MapTutorialState values zero through eight.
    switch ( tTutorialState.eMapTutorialState )
    {
        case MAP_TUTORIAL_STATE_INTRO:
        {
            SetupControlsForWorldView();
            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_01 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_01 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }
        case MAP_TUTORIAL_STATE_WORLD_VIEW:
        {
            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_02 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_02 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }
        case MAP_TUTORIAL_STATE_WORLD_VIEW_HIGHLIGHT_ACTS:
        {

            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_03 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_03 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }
        case MAP_TUTORIAL_STATE_ACT_VIEW:
        {
            Act eCurrentAct = ptPlayer ? GetWaypointActForPlayer( ptPlayer ) : ACT_INVALID;
            SetupControlsForAct( eCurrentAct );
            UIWindow & tBountyRewardWindow = UI_GetWindowFromID( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY );
            tBountyRewardWindow.SetActive( FALSE );

            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_04 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_04 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }

        case MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTIES:
        {
            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_05 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_05 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }
        case MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTIES_INDICATOR:
        {
            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_06 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_06 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }

        case MAP_TUTORIAL_STATE_ACT_VIEW_HIGHLIGHT_BOUNTY_REWARDS:
        {
            UIWindow & tBountyRewardWindow = UI_GetWindowFromID( UIWND_WAYPOINTMAP_OVERLAY_BOUNTY );
            tBountyRewardWindow.SetActive( TRUE );

            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_07 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_07 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }
        case MAP_TUTORIAL_STATE_CONCLUSION:
        {
            SNO snoSound = GlobalSNOGet( GLOBALSNO_SOUND_OPEN_WORLD_TUTORIAL_08 );
            tOptions.nPermutationIndex = sChooseTutorialSample( snoSound, bPlayerIsFemale );
            tTutorialState.idTutorialSound = SoundManager::PlaySound( snoSound, tOptions );
            if ( tTutorialState.idTutorialSound == SOUNDID_INVALID )
            {
                tTutorialState.eMapTutorialState = MAP_TUTORIAL_STATE_CONCLUSION;
                TutorialAdvance( ptPlayer, tTutorialState );
                break;
            }

            SNO snoStringList = GlobalSNOGet( GLOBALSNO_STRINGLIST_OPEN_WORLD_TUTORIAL_08 );
            sShowTutorialText( snoStringList, bPlayerIsFemale );
            break;
        }
        case MAP_TUTORIAL_STATE_DONE:
        {
            tTutorialState.bOpenForTutorial = FALSE;

            SimpleMessage tMessage;
            MessageSendToServer( CMSG_FINISHED_OPEN_WORLD_TUTORIAL, &tMessage, sizeof(tMessage), ptPlayer );


            ShowTutorialText( "" );
            Console::Screens::GetActionManager().SetActionsDirtyFlag( TRUE );
            break;
        }







    }
}



BOOL UIWaypointMap::TutorialCanBeClosed( Player * ptPlayer )
{

    if ( !IsOpenForTutorial() )
    {
        return TRUE;
    }



    ACDID idACDPlayer = ptPlayer->idPlayerACD;
    ActorCommonData * ptACDPlayer = ACDTryToGet( idACDPlayer );
    if ( ptACDPlayer )
    {

        SNO snoLevelArea = ACDGetLevelArea( *ptACDPlayer );
        if ( snoLevelArea != SNO_NONE && !LevelAreaIsTown( snoLevelArea ) )
        {
            return TRUE;
        }
    }

    return FALSE;
}

#include "../../../Common/Game.h"
#include "../../../Common/NephalemRiftCommon.h"
#include "../UIStoryboard.h"
// DWARF: X/Client/UserInterface/UI.cpp:5793.
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
// DWARF: X/Common/Quests/Quests.cpp:3019.
BOOL QuestsOpenWorldTrackedSNOIsComplete(SNO sno);
#line 449
static void sUpdateRiftIcons(const UIID & idWaypoint)
{
    SNO snoLootRunQuest = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER);
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoLootRunQuest, ACDID_INVALID);
    if (ptQuestInstance)
    {
        BOOL bLootRunOpen = GameGetActiveGameCommonData().tGameSyncedData.bLootRunOpen;

        BOOL bIsTieredRift = NephalemRift::IsTieredRiftActive();
        // Inference: the original conjunctions choose mutually exclusive normal/tiered icons.
        UI_GetWindowFromID(UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Interest.LootRun")).SetActive(bLootRunOpen && !bIsTieredRift);
        UI_GetWindowFromID(UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Interest.TieredRift")).SetActive(bLootRunOpen && bIsTieredRift);

        UIID idLootRunCompleteOverlay = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Interest.LootRun.Complete");
        UIID idGreaterLootRunCompleteOverlay = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Interest.TieredRift.Complete");
        // Inference: original short-circuit branch requires the open rift and dead boss flags.
        if (GameGetActiveGameCommonData().tGameSyncedData.bLootRunOpen &&
            GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead)
        {
            if (bIsTieredRift)
            {
                UI_GetWindowFromID(idGreaterLootRunCompleteOverlay).SetActive(TRUE);
                UI_GetWindowFromID(idLootRunCompleteOverlay).SetActive(FALSE);
            }
            else
            {
                UI_GetWindowFromID(idGreaterLootRunCompleteOverlay).SetActive(FALSE);
                UI_GetWindowFromID(idLootRunCompleteOverlay).SetActive(TRUE);
            }
        }
        else
        {

            UI_GetWindowFromID(idLootRunCompleteOverlay).SetActive(FALSE);
            UI_GetWindowFromID(idGreaterLootRunCompleteOverlay).SetActive(FALSE);
        }
    }
}
#line 881
void UIWaypointMap::UpdateWaypointControls(
    Player * ptPlayerViewing,
    Act eAct,
    BOOL bHighlightBounties)
{
    SNO snoQuestWaypointLevelArea = GetQuestWaypointLevelArea(ptPlayerViewing);

    int32 nActIndex = ActIndexFromActEnum(eAct);
    SNO snoRewardQuest = SNOFileLookupGUID(QUEST_SNO, CRefString().Format("x1_AdventureMode_BountyTurnin_A%d", nActIndex + 1));

    UIWindow & tOverlayWindow = UI_GetWindowFromID(UIWND_WAYPOINTMAP_OVERLAY_POI);
#line 891
// Inference: the original reference names the child-list member; the canonical accessor supplies that reference.
    const UIControlItemArray & arChildren = tOverlayWindow.GetChildList();
    // Inference: original traversal starts at the first child and visits the entire array.
    for (uint32 i = 0; i < arChildren.size(); i++)
    {

        const UIID & idWaypoint = arChildren[i].id;


        UpdateQuestWaypointCallout(idWaypoint, snoQuestWaypointLevelArea);
        // Inference: original truncates GetUserData metadata to the DWARF SNO integer.
        SNO snoLevelArea = (SNO)(intptr_t)UI_GetControlFromID(idWaypoint).GetUserData();
        SNO snoBounty = QuestGetActiveBountyForLevelArea(snoLevelArea, FALSE);
        if (snoBounty != SNO_NONE)
        {
            UIID idBounty;
            ScopedQuestDefinitionLockR pdefBounty(snoBounty);
            if (pdefBounty && pdefBounty->eQuestType == QUEST_TYPE_HORADRIC_QUEST)
            {
                idBounty = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Bounty_Horadric");
            }
            else
            {
                idBounty = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Bounty");
            }

            UI_GetWindowFromID(idBounty).SetActive(TRUE);

            QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoBounty, ACDID_INVALID);
            UI_GetWindowFromID(UIIDCreateFromParent(idBounty, ".Complete")).SetActive(ptQuestInstance && QuestIsComplete(*ptQuestInstance));

            UIID idStoryboard = UIIDCreateFromParent(idWaypoint, ".POICallout");
            UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);

            UIID idTutorialArrow = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Arrow");
            UI_GetWindowFromID(idTutorialArrow).SetActive(tStoryboard.IsPlaying());
            // Inference: highlight requests start the storyboard only while it is stopped.
            if (bHighlightBounties &&
                !tStoryboard.IsPlaying())
            {
                tStoryboard.Play(TRUE, 0.0f);
            }
        }


        if (LevelAreaIsTown(snoLevelArea))
        {
            sUpdateRiftIcons(idWaypoint);

            UIID idReward = UIIDCreateFromParent(idWaypoint, ".LayoutRoot.Interest.Reward");
            QuestInstance * ptRewardQuestInstance = QuestsGetQuestInstance(snoRewardQuest, ACDID_INVALID);
            if (ptRewardQuestInstance && ptRewardQuestInstance->eInstanceState == QUEST_INSTANCE_ACTIVE)
            {
                UI_GetWindowFromID(idReward).SetActive(TRUE);

                UIID idRewardCompleteOverlay;
                idRewardCompleteOverlay.SetFromParent(idWaypoint, "LayoutRoot.Interest.Reward.Complete");
                if (QuestIsComplete(*ptRewardQuestInstance))
                {
                    UI_GetWindowFromID(idRewardCompleteOverlay).SetActive(TRUE);
                }
                else
                {
                    UI_GetWindowFromID(idRewardCompleteOverlay).SetActive(FALSE);
                }
            }
            else
            {
                UI_GetWindowFromID(idReward).SetActive(FALSE);
            }
        }
        // Inference: the two original unrolled paths establish this local array order and bound.
        LPCSTR szBossControls[2] = { "LayoutRoot.Interest.Boss", "LayoutRoot.Interest.KeyWarden" };
        for (int32 b = 0; b < 2; b++)
        {
            UIID idBoss;
            idBoss.SetFromParent(idWaypoint, szBossControls[b]);
            UIWindow & tBoss = UI_GetWindowFromID(idBoss);

            if (tBoss.IsActive())
            {
#line 971
// Inference: the original truncates opaque control metadata to the DWARF SNO integer.
                SNO snoBossEncounter = (SNO)(intptr_t)tBoss.GetUserData();
                if (QuestsOpenWorldTrackedSNOIsComplete(snoBossEncounter))
                {
                    UIID idBossCompleteOverlay;
                    idBossCompleteOverlay.SetFromParent(idBoss, "Complete");
                    UI_GetWindowFromID(idBossCompleteOverlay).SetActive(TRUE);
                }
            }
        }
        // Inference: the original portrait counter begins at zero for each waypoint.
        int32 nPlayersInLevelArea = 0;

        for (const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {




            // Inference: hostile players take the original shared next-player path.
            if (PlayersAreHostileToEachOther(ptPlayer, ptPlayerViewing))
            {
                continue;
            }

            if (PlayerGetLastWaypointLevelArea(ptPlayer) == snoLevelArea)
            {
                UIID idPlayerIndicator;
#line 998
// Inference: the original increments the portrait counter before formatting the one-based label.
                idPlayerIndicator.SetFromParent(idWaypoint, CRefString().Format("LayoutRoot.P%d", ++nPlayersInLevelArea));
                UIButton & tButtonPlayerIndicator = UI_GetButtonFromID(idPlayerIndicator);
                tButtonPlayerIndicator.SetActive(TRUE);
                ImageTextureFrame tTexFrame;
                CPlayerGetPortraitImage(ptPlayer, tTexFrame, FALSE);
                tButtonPlayerIndicator.SetAllTextureFrames(tTexFrame);
#line 1004
// Inference: the original sign-extends the player index into opaque control metadata.
                tButtonPlayerIndicator.SetUserData((void *)(intptr_t)ptPlayer->nPlayerIndex);

            }
        }


        // Inference: the remaining original portrait slots are hidden up to the four-player bound.
        for (int32 j = nPlayersInLevelArea; j < 4; j++)
        {
            UIID idPlayerIndicator;
            idPlayerIndicator.SetFromParent(idWaypoint, CRefString().Format("LayoutRoot.P%d", j + 1));
            UIButton & tButtonPlayerIndicator = UI_GetButtonFromID(idPlayerIndicator);
            tButtonPlayerIndicator.SetActive(FALSE);
        }
    }
}
// Owner dependencies for the recovered setup caller.
#include "../../../Common/Attributes/FastAttribConstants.h"
#include "../../../Common/Definitions/BossEncounterDefinition.h"
#include "../../../Common/Definitions/LevelAreaDefinition.h"
// DWARF: X/Common/Act.cpp:83.
void ActCacheWaypoints(const Act eAct);
#line 75
struct BossEncounterLevelAreaData
{
    SNO snoLevelArea;
    SNO snoBossEncounter;
    BOOL bIsKeyWarden;
};
#line 501
// Recovered from the original inline instance; _ZL linkage and DW_INL_inlined (no inline keyword).
static void sAddWaypointControls(
    Player * ptPlayerViewing,
    Act eAct)
{
#line 505
// Inference: original null-player branch skips the complete setup helper.
    if (!ptPlayerViewing)
    {
        return;
    }


    const int32 MAX_BOSS_ENCOUNTERS = 64;
    BossEncounterLevelAreaData tBossEncounterLevelAreaData[MAX_BOSS_ENCOUNTERS];
#line 513
// Inference: the concrete DWARF location starts this count at zero.
    int32 nCurrentBossEncounterLevelAreaDataCount = 0;
    // Inference: the original initializes every slot to absent boss/area and false key-warden.
#line 515
// Inference: the original initializes all 64 DWARF array entries.
    for (int32 i = 0; i < MAX_BOSS_ENCOUNTERS; i++)
    {
        tBossEncounterLevelAreaData[i].snoLevelArea = SNO_NONE;
        tBossEncounterLevelAreaData[i].snoBossEncounter = SNO_NONE;
        tBossEncounterLevelAreaData[i].bIsKeyWarden = FALSE;
    }

    if (GameIsOpenWorld())
    {
        PooledSNONameList listSNOFiles(GetSnoNameListPool());
        SNOFileEnumerateSNOsForGroup(BOSS_ENCOUNTER_SNO, &listSNOFiles);

        POS pos = listSNOFiles.GetHeadPosition();
        while (pos && nCurrentBossEncounterLevelAreaDataCount < MAX_BOSS_ENCOUNTERS)
        {
            SNOName & snonameFile = listSNOFiles.GetNext(pos);
            ScopedBossEncounterDefinitionLockR pdefBossEncounter(snonameFile.snoHandle);
            if (pdefBossEncounter->snoWaypointLevelArea != SNO_NONE)
            {
                Act eBossEncounterAct = LevelAreaGetAct(pdefBossEncounter->snoWaypointLevelArea);
#line 535
// Inference: unrecognized act codes keep the original -1 result.
                int32 nBossEncounterAct = -1;
#line 536
// Inference: original comparisons select act numbers 1–5, with an explicit default retaining -1.
                switch (eBossEncounterAct)
                {
                case ACT_1:
                    nBossEncounterAct = 1;
                    break;
                case ACT_2:
                    nBossEncounterAct = 2;
                    break;
                case ACT_3:
                    nBossEncounterAct = 3;
                    break;
                case ACT_4:
                    nBossEncounterAct = 4;
                    break;
                case ACT_5:
                    nBossEncounterAct = 5;
                    break;
                default:
                    break;
                }
                // Inference: unmatched boss IDs keep the original false flag.
                BOOL bIsKeyWarden = FALSE;
                SNO snoActKeyWardenBossEncounter = QuestsOpenWorldGetKeyWardenBossEncounter(nBossEncounterAct);
                if (snonameFile.snoHandle == snoActKeyWardenBossEncounter)
                {
                    // Inference: original retains key wardens only at handicap >=4 and monster level >=60 (2.7.7: GameGetMonsterLevel, 0x24fd1c).
                    if (GameGetHandicapLevel() >= HC_TORMENT_1 &&
                        GameGetMonsterLevel() >= HERO_LEVEL_MAX_VANILLA)
                    {
#line 565
// Inference: the surviving key-warden branch sets TRUE; other qualifying bosses keep FALSE.
                        bIsKeyWarden = TRUE;
                    }
#line 567
// Inference: the original shared loop tail skips under-level or low-handicap key wardens.
                    else { continue; }
                }


#line 571
// Inference: boss, key-warden and level-area stores follow the observed three-field update; grouping still differs.
                tBossEncounterLevelAreaData[nCurrentBossEncounterLevelAreaDataCount].snoBossEncounter = snonameFile.snoHandle;
                tBossEncounterLevelAreaData[nCurrentBossEncounterLevelAreaDataCount].bIsKeyWarden = bIsKeyWarden;
                tBossEncounterLevelAreaData[nCurrentBossEncounterLevelAreaDataCount].snoLevelArea = pdefBossEncounter->snoWaypointLevelArea;
                nCurrentBossEncounterLevelAreaDataCount++;
            }
        }





    }



    // Inference: the original SNOName constructor uses this surviving console template string.
    LPCSTR szSnoName = "ConsoleWaypointMap_POI";


    SNOName snonamePOITemplate(UI_SNO, szSnoName);
    UIControlSourceKey tTemplateSrcKey(snonamePOITemplate.snoHandle, "WaypointMap_POI_main");

    if (ActGetCachedWaypointArray().size() == 0)
    {
        ActCacheWaypoints(GameGetCurrentAct());
    }







    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    uint32 nCount = tWaypoints.size();
#line 606
// Inference: the original visible-waypoint count begins at zero.
    int32 nVisibleWaypoints = 0;
#line 607
// Inference: the original visits the cached waypoint count starting from index zero.
    for (uint32 i = 0; i < nCount; i++)
    {
        const CachedWaypoint & tWaypoint = tWaypoints[i];
        // Inference: original rejects other acts before testing waypoint activation.
        if (tWaypoint.eSourceAct != eAct)
        {
            continue;
        }

        if (!GameIsAdventureMode() && !PlayerHasActivatedWaypoint(*ptPlayerViewing, i))
        {
            continue;
        }

        // Inference: the search defaults to no boss and stops at the first matching area.
        BOOL bIsKeyWarden = FALSE;
        SNO snoBossEncounter = SNO_NONE;
        for (int32 j = 0; j < nCurrentBossEncounterLevelAreaDataCount; j++)
        {
            if (tBossEncounterLevelAreaData[j].snoLevelArea == tWaypoint.tInfo.snoLevelArea)
            {
                snoBossEncounter = tBossEncounterLevelAreaData[j].snoBossEncounter;
                bIsKeyWarden = tBossEncounterLevelAreaData[j].bIsKeyWarden;
#line 630
// Inference: the matching area takes the original immediate search exit.
                break;
            }
        }

        SNO snoBounty = QuestGetActiveBountyForLevelArea(tWaypoint.tInfo.snoLevelArea, FALSE);
        // Inference: DWARF UIWindow* receives the original template control result by derived cast.
        UIWindow * ptWaypoint = (UIWindow *)UI_CreateControlFromTemplate(tTemplateSrcKey,
            DynamicControlCreationParams(
            CRefString().Format("entry %d", i), UIWND_WAYPOINTMAP_OVERLAY_POI), NULL);


        ptWaypoint->SetActive(TRUE);
#line 642
// Inference: the original sign-extends the level-area SNO into opaque control metadata.
        ptWaypoint->SetUserData((void *)(intptr_t)tWaypoint.tInfo.snoLevelArea);

        nVisibleWaypoints++;

        UIID idButtonName;
        idButtonName.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Name");
        UIButton & tButtonName = UI_GetButtonFromID(idButtonName);
        tButtonName.SetText(LevelAreaGetLocalizedName(tWaypoint.tInfo.snoLevelArea).str(), FALSE);
#line 650
// Inference: the original zero-extends the unsigned waypoint index into opaque control metadata.
        tButtonName.SetUserData((void *)(uintptr_t)i);
        tButtonName.SetActive(FALSE);

        UIID idButtonTown;
        idButtonTown.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Town");
        UIButton & tButtonTown = UI_GetButtonFromID(idButtonTown);
        tButtonTown.SetText(LevelAreaGetLocalizedName(tWaypoint.tInfo.snoLevelArea).str(), FALSE);
#line 657
// Inference: the original zero-extends the unsigned waypoint index into opaque control metadata.
        tButtonTown.SetUserData((void *)(uintptr_t)i);
        tButtonTown.SetActive(FALSE);

        UIID idButtonPOI;
        idButtonPOI.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.POI");
        UIButton & tButtonPOI = UI_GetButtonFromID(idButtonPOI);
        tButtonPOI.SetText(LevelAreaGetLocalizedName(tWaypoint.tInfo.snoLevelArea).str(), FALSE);
#line 664
// Inference: the original zero-extends the unsigned waypoint index into opaque control metadata.
        tButtonPOI.SetUserData((void *)(uintptr_t)i);
        tButtonPOI.SetActive(FALSE);

        {
            UIID idSelected;
            idSelected.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Selected");
            UIWindow & tWindowSelected = UI_GetWindowFromID(idSelected);


            tWindowSelected.SetActive(FALSE);
        }











        if (LevelAreaIsTown(tWaypoint.tInfo.snoLevelArea))
        {
            UIID idTown;
            idTown.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Town");
            UI_GetWindowFromID(idTown).SetActive(TRUE);

            tButtonTown.SetActive(TRUE);
        }
        else
        {








            if (snoBounty != SNO_NONE)
            {
                tButtonPOI.SetActive(TRUE);
            }
            else
            {
                tButtonName.SetActive(TRUE);
            }
        }

        if (snoBounty != SNO_NONE)
        {
            UIID idBounty;

            ScopedQuestDefinitionLockR pdefBounty(snoBounty);
            if (pdefBounty && pdefBounty->eQuestType == QUEST_TYPE_HORADRIC_QUEST)
            {
                idBounty.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Bounty_Horadric");
            }
            else
            {
                idBounty.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Bounty");
            }

            UI_GetWindowFromID(idBounty).SetActive(TRUE);
#line 729
// Inference: the original sign-extends the bounty SNO into opaque control metadata.
            UI_GetWindowFromID(idBounty).SetUserData((void *)(intptr_t)snoBounty);

            QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoBounty, ACDID_INVALID);
            if (ptQuestInstance && QuestIsComplete(*ptQuestInstance))
            {
                UIID idBountyCompleteOverlay;
                idBountyCompleteOverlay.SetFromParent(idBounty, "Complete");
                UI_GetWindowFromID(idBountyCompleteOverlay).SetActive(TRUE);
            }
        }


        if (LevelAreaIsTown(tWaypoint.tInfo.snoLevelArea))
        {
            sUpdateRiftIcons(ptWaypoint->GetID());

            UIID idReward;
            idReward.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Interest.Reward");
            int32 nActIndex = ActIndexFromActEnum(eAct);
            SNO snoRewardQuest = SNOFileLookupGUID(QUEST_SNO, CRefString().Format("x1_AdventureMode_BountyTurnin_A%d", nActIndex + 1));
            QuestInstance * ptRewardQuestInstance = QuestsGetQuestInstance(snoRewardQuest, ACDID_INVALID);
            if (ptRewardQuestInstance)
            {
                UI_GetWindowFromID(idReward).SetActive(TRUE);

                UIID idRewardCompleteOverlay;
                idRewardCompleteOverlay.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Interest.Reward.Complete");
                if (QuestIsComplete(*ptRewardQuestInstance))
                {
                    UI_GetWindowFromID(idRewardCompleteOverlay).SetActive(TRUE);
                }
                else
                {
                    UI_GetWindowFromID(idRewardCompleteOverlay).SetActive(FALSE);
                }
            }
            else
            {
                UI_GetWindowFromID(idReward).SetActive(FALSE);
            }
        }

        if (snoBossEncounter != SNO_NONE)
        {
            UIID idBoss;
            if (bIsKeyWarden)
            {
                idBoss.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Interest.KeyWarden");
            }
            else
            {
                idBoss.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Interest.Boss");
            }

            UI_GetWindowFromID(idBoss).SetActive(TRUE);
#line 784
// Inference: the original sign-extends the boss SNO into opaque control metadata.
            UI_GetWindowFromID(idBoss).SetUserData((void *)(intptr_t)snoBossEncounter);

            if (QuestsOpenWorldTrackedSNOIsComplete(snoBossEncounter))
            {
                UIID idBossCompleteOverlay;
                idBossCompleteOverlay.SetFromParent(idBoss, "Complete");
                UI_GetWindowFromID(idBossCompleteOverlay).SetActive(TRUE);
            }
        }

#line 794
// Inference: the original anniversary icon needs both Old Tristram and the named game flag.
        if (tWaypoint.tInfo.snoLevelArea == GlobalSNOGet(GLOBALSNO_LEVELAREA_OLD_TRISTRAM) &&


            (GameGetActiveGameCommonData().tGameSyncedData.dwGameSyncedFlags & (1 << GAME_SYNCED_FLAGS_ANNIVERSARY_DUNGEON)))
        {
            UIID idAnniversaryDungeon;
            idAnniversaryDungeon.SetFromParent(ptWaypoint->GetID(), "LayoutRoot.Interest.AnniversaryDungeon");

            UI_GetWindowFromID(idAnniversaryDungeon).SetActive(TRUE);
        }

        // Inference: portrait numbering restarts from zero for every waypoint.
        int32 nPlayersInLevelArea = 0;
        for (const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
        {





#line 814
// Inference: hostile players take the original shared next-player path.
            if (PlayersAreHostileToEachOther(ptPlayer, ptPlayerViewing))
            {
                continue;
            }

            if (PlayerGetLastWaypointLevelArea(ptPlayer) == tWaypoint.tInfo.snoLevelArea)
            {
                UIID idPlayerIndicator;
#line 822
// Inference: the original increments the portrait counter before formatting the one-based label.
                idPlayerIndicator.SetFromParent(ptWaypoint->GetID(), CRefString().Format("LayoutRoot.P%d", ++nPlayersInLevelArea));
                UIButton & tButtonPlayerIndicator = UI_GetButtonFromID(idPlayerIndicator);
                tButtonPlayerIndicator.SetActive(TRUE);
                ImageTextureFrame tTexFrame;
                CPlayerGetPortraitImage(ptPlayer, tTexFrame, FALSE);
                tButtonPlayerIndicator.SetAllTextureFrames(tTexFrame);
#line 828
// Inference: the original sign-extends the player index into opaque control metadata.
                tButtonPlayerIndicator.SetUserData((void *)(intptr_t)ptPlayer->nPlayerIndex);
            }
        }



        Vector2D vecTL;
#line 835
        Vector2D vecDim;
        ptWaypoint->GetRelativePosition(vecTL, vecDim);


        // Inference: original integer coordinates scale from 1200 to 720 before float addition.
        vecTL.x += (tWaypoint.tInfo.pointMapLocation.x * 720) / 1200;
        vecTL.y += (tWaypoint.tInfo.pointMapLocation.y * 720) / 1200;
        ptWaypoint->SetRelativePosition(vecTL, vecDim);
    }

    if (nVisibleWaypoints > 0)
    {
        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_REGIONMAPOPEN), NULL, TRUE);
    }
}



















// NON_MATCHING: 2836/2836 bytes; 7 instructions differ in boss-data store grouping/register scheduling; tried positive/negative eligibility tests and three field-store orders.
void UIWaypointMap::SetupWaypointControls(
    Player * ptPlayerViewing,
    Act eAct)
{
    sClearWaypointControls();
    sAddWaypointControls(ptPlayerViewing, eAct);
}
