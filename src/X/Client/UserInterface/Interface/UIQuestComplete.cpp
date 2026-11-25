#include "UIQuestComplete.h"

#include "../UI.h"
#include "../UIID.h"
#include "../UIStoryboard.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Console/ConsoleScreenManager.h"
#include "../../CGame.h"
#include "../../Sound/Sound.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
#include "../../../Common/Items.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIItemButtonControl.h"
#include "../../../Common/Quests/Quests.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../CPlayer.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../ImageCollection.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Common/UserInterface/UIInventory.h"
#include "UIToolTipManager.h"
#include "../Controls/UIToolTipControl.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
extern UIID UIDLG_QUESTREWARD;  // 0x71017d5610
extern UIID UIDLG_BOUNTYREWARD;  // 0x71017d8308
extern UIID UIDLG_TIEREDRIFTREWARD;  // 0x71017d8310
extern UIID UIDLG_TIEREDRIFTREWARD_BLOODSHARD_INCREASE;  // 0x71017d55f8
extern UIID UIDLG_TIEREDRIFTSCORECARD;  // 0x71017d8318
extern UIID UITXT_QUESTREWARD_XP_GOLD, UIWND_QUESTREWARD_ITEM_WRAPPER, UIBTN_QUESTREWARD_ITEM, UITXT_QUESTREWARD_ITEM; extern UIID UITXT_QUESTREWARD_TITLE, UIWND_QUESTREWARD_ICON, UITXT_QUESTREWARD_KEYBINDING, UITXT_BOUINTYREWARD_KEYBINDING;  // 0x71017d5618..0x71017d5650
UIItemButton & UI_GetItemButtonFromID(const UIID & id);  // UI.cpp
UIStoryboard & UI_GetStoryboardFromID(const UIID & id); void UI_Print(LPCSTR szText, BOOL bChat); namespace Console { namespace GamerProfile { int32 GetPrimaryProfileUserIndex(); } }  // UI.cpp, GamerProfile.cpp
namespace Console { namespace Screens {
BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition);  // ConsoleScreenManager.cpp:1104
} }
Player * PlayerGetByControllerIndexAlways(int32 nControllerIndex); BOOL PlayerIsLocal(const Player * ptPlayer);  // CPlayer.cpp
namespace UIQuests { BOOL CanShowXPReward(const uint64 nXPGranted); }  // UIQuests.cpp
void QuestGetIcon(const Act eAct, const SNO snoQuest, const BOOL fCompleted, ImageTextureFrame & tFrame);  // QuestDefinition.cpp
namespace UIInventory { void GetItemTooltipParameters(ItemTooltipParameterBlock & tParams, const ActorCommonData & tACDItem, const ItemTooltipPlayerData & tTooltipPlayerData, const BOOL bConsoleInventory, const BOOL bShowAsRepaired); void InitItemTooltip(ItemTooltipData & tData, const ACDID idACD, const GBID gbid, const ItemTooltipParameterBlock & tParams, const BOOL bCalculateComparisonData, ActorCommonData * ptACDPlayer, const BOOL bDisableSocketText); }  // UIInventory.cpp
namespace UIEventQueue { namespace Console { void ReportItemPickup(const Player * ptPlayer, const ActorCommonData & tACDItem); void DisplayMessageWithIconForPlayer(const CRefString & sMessage, const Player * ptPlayer, const ImageTextureFrame & tImage, CRefString * psMessageLine2, BOOL bUseLargeIcon, BOOL bCritical); } }  // UIEventQueue.cpp
void ACDDelete(ACDID idACD, BOOL bSendToClients); void EffectGroupCreateUntracked(SNO snoEffectGroup, RActorID idRActor, uint32 dwFlags, int32 n1, int32 n2);  // ActorCommonData.cpp, EffectGroup.cpp
enum {
    QUEST_REWARD_DURATION = 36000,
};
UIQuestComplete theQuestCompleteUI;
UIBountyReward theBountyRewardUI;
UITieredRiftReward theTieredRiftRewardUI;
UITieredRiftScorecard theTieredRiftScorecardUI;









BOOL UIRewards::IsAnyVisible()
{

    return theQuestCompleteUI.IsVisible() || theBountyRewardUI.IsVisible() || theTieredRiftRewardUI.IsVisible();
}




void UIRewards::CloseAll()
{

    UIQuestComplete::Close( GetUIIDNone() );
    UIBountyReward::Close( GetUIIDNone() );
    UITieredRiftReward::Close( GetUIIDNone() );
}




void UIRewards::ShowTieredRiftReward( const D3::Quests::QuestReward & tReward, const BOOL bFinishedInTime, const int32 nRiftDepth, const int32 nControllerIndex )
{




    ACDID idACDItem;

    CloseAll();

    if ( bFinishedInTime )
    {
        theTieredRiftRewardUI.Show( &tReward, -1 );
    }
    else
    {

        theTieredRiftScorecardUI.Show( &tReward, nRiftDepth );
    }



    if ( tReward.item().gb_handle().gbid() != GBID_NONE )
    {
        idACDItem = CItemGenerate( (ACDNetworkName)0x2e00, tReward.item(), NULL );

        ActorCommonData & tACDItem = ACDGet( idACDItem );

        if ( !ItemHasLabel( tACDItem.hGB.gbid, 0x71 ) )
        {
            Player * ptPlayer = PlayerGetByControllerIndexAlways( nControllerIndex );
            if ( ptPlayer && PlayerIsLocal( ptPlayer ) )
            {

                UIEventQueue::Console::ReportItemPickup( ptPlayer, tACDItem );
            }
        }


        ACDDelete( idACDItem, FALSE );
    }
}























UIQuestComplete::UIQuestComplete()
{


    m_dwStartTime = 0;
}



void UIQuestComplete::ConsoleClose( const UIID & id, const int32 )
{



    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_SELECT_GENERIC ), cgtPSODefaults );
    Close( id );
}



static BOOL sQuestIsBounty( SNO snoQuest )
{

    if ( snoQuest != SNO_NONE &&
         ( QuestIsAnyBountyType( snoQuest ) || Blizzard::String::Find( SNOToString( 0x1f, snoQuest, FALSE ).str(), "BountyTurnin" ) ) )
    {
        return TRUE;
    }
    return FALSE;
}





void UIQuestComplete::OnQuestReward( const D3::Quests::QuestReward & tReward, const BOOL bCompleted, const BOOL bEquipped, const int32 nControllerIndex )
{




    BOOL fIsBounty = sQuestIsBounty( tReward.sno_quest() );
    ACDID idACDItem;

    if ( bCompleted )
    {
        RActorID idPlayerRActor = LocalPlayerRActorGet();
        if ( idPlayerRActor != RACTORID_INVALID )
        {
            EffectGroupCreateUntracked( GlobalSNOGet( GLOBALSNO_EFFECTGROUP_QUEST_COMPLETED ), idPlayerRActor, 0, 0, 0 );
        }
        SoundManager::PlaySound( GlobalSNOGet( fIsBounty ? GLOBALSNO_SOUND_BOUNTY_COMPLETED : GLOBALSNO_SOUND_QUEST_COMPLETED ), cgtPSODefaults );
    }

    UIRewards::CloseAll();

    if ( fIsBounty )
    {

        theBountyRewardUI.ConsoleShow( TRUE, &tReward );
    }
    else
    {



        theQuestCompleteUI.Show( TRUE, &tReward, bEquipped );
    }




    if ( tReward.item().gb_handle().gbid() != GBID_NONE )
    {
        idACDItem = CItemGenerate( (ACDNetworkName)0x2e00, tReward.item(), NULL );

        ActorCommonData & tACDItem = ACDGet( idACDItem );

        if ( !ItemHasLabel( tACDItem.hGB.gbid, 0x71 ) )
        {
            Player * ptPlayer = PlayerGetByControllerIndexAlways( nControllerIndex );
            if ( ptPlayer && PlayerIsLocal( ptPlayer ) )
            {

                UIEventQueue::Console::ReportItemPickup( ptPlayer, tACDItem );
            }
        }



        ACDDelete( idACDItem, FALSE );
    }
}





void UIQuestComplete::OnMultiplayerQuestReward( const int32 nControllerIndex, const D3::Quests::QuestReward & tReward, const BOOL bCompleted, const BOOL bEquipped )
{



    BOOL fStepCompletion = !bCompleted;
    if ( bCompleted )
    {

        RActorID idPlayerRActor = LocalPlayerRActorGet();
        if ( idPlayerRActor != RACTORID_INVALID )
        {
            EffectGroupCreateUntracked( GlobalSNOGet( GLOBALSNO_EFFECTGROUP_QUEST_COMPLETED ), idPlayerRActor, 0, 0, 0 );
        }

        BOOL fIsBounty = sQuestIsBounty( tReward.sno_quest() );
        SoundManager::PlaySound( GlobalSNOGet( fIsBounty ? GLOBALSNO_SOUND_BOUNTY_COMPLETED : GLOBALSNO_SOUND_QUEST_COMPLETED ), cgtPSODefaults );
    }

    theQuestCompleteUI.ShowMultiplayerRewards( nControllerIndex, &tReward );
}





void UIQuestComplete::Close( const UIID & )
{


    theQuestCompleteUI.Show( FALSE, NULL, FALSE );
}


BOOL UIQuestComplete::IsVisible()
{


    return UI_GetWindowFromID( UIDLG_QUESTREWARD ).IsActive();
}

void UIQuestComplete::ShowItemReward( BOOL bEquipped )
{

    if ( PlayerGetLocalPlayerCount() >= 2 && !ItemHasLabel( m_tQuestReward.item().gb_handle().gbid(), 0x71 ) )
    {



        return;
    }

    if ( m_tQuestReward.item().gb_handle().gbid() == GBID_NONE )
        return;
    if ( ACDTryToGet( ACDFromANN( (ACDNetworkName)0x2e00 ) ) )
        return;
    ACDID idItem = CItemGenerate( (ACDNetworkName)0x2e00, m_tQuestReward.item(), NULL );



    if ( idItem == ACDID_INVALID )
        return;
    ActorCommonData & tACDItem = ACDGet( idItem );

    UIText & tTextXPGold = UI_GetTextFromID( UITXT_QUESTREWARD_XP_GOLD );
    UIWindow & tWndItemWrapper = UI_GetWindowFromID( UIWND_QUESTREWARD_ITEM_WRAPPER );

    tWndItemWrapper.SetActive( TRUE );
    tTextXPGold.SetActive( FALSE );


    UIItemButton & tBtnItem = UI_GetItemButtonFromID( UIBTN_QUESTREWARD_ITEM );
    tBtnItem.SetFromItemGenerator( m_tQuestReward.item() );


    CRefString sItemName = ItemGetDisplayName( tACDItem, TRUE, FALSE, FALSE, TRUE );
    UI_GetTextFromID( UITXT_QUESTREWARD_ITEM ).SetText( sItemName, FALSE );


    ACDDelete( idItem, FALSE );
}


void UIQuestComplete::ShowMultiplayerRewards( const int32 nControllerIndex, const D3::Quests::QuestReward * ptReward )
{


    Player * ptPlayer = PlayerGetByControllerIndexAlways( nControllerIndex );

    if ( ptPlayer && PlayerIsLocal( ptPlayer ) )
    {
        int32 nGold = ptReward->gold_granted();
        uint64 nXP = ptReward->xp_granted() + ptReward->bonus_xp_granted();
        int32 nBloodShards = 0;

        for ( int32 i = 0; i < ptReward->items_size(); i++ )
        {
            if ( ptReward->items( i ).gb_handle().gbid() == GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ) )
            {
                nBloodShards += (int32)ptReward->items( i ).stack_size();
            }
        }

        BOOL fCanShowXPReward = UIQuests::CanShowXPReward( nXP );
        ACDID idACDItem;
        if ( nGold > 0 || fCanShowXPReward )
        {
            QuestType eQuestType = QUEST_TYPE_EVENT;
            SNO snoQuest = ptReward->sno_quest();
            if ( snoQuest != SNO_NONE )
            {
                ScopedQuestDefinitionLockR pdefQuest( snoQuest );
                eQuestType = pdefQuest->eQuestType;
            }

            CRefString sTitle;
            ImageTextureFrame tTexFrameIcon = cgtImageTextureFrameNull;
            switch ( eQuestType )
            {
            case QUEST_TYPE_MAIN_QUEST:
                sTitle = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_H2O_LAYOUT ), "QuestRewardTitle" );
                QuestGetIcon( GameGetCurrentAct(), snoQuest, TRUE, tTexFrameIcon );
                break;
            case QUEST_TYPE_BOUNTY:
                sTitle = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_H2O_LAYOUT ), "BountyRewardTitle" );
                ImageCollectionGetImageInfo( IMAGE_COLLECTION_UI, "BattleNetQuestIcons_Bounty_Generic", tTexFrameIcon );
                break;

            case QUEST_TYPE_HORADRIC_QUEST:
                sTitle = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_H2O_LAYOUT ), "BountyRewardTitle" );
                ImageCollectionGetImageInfo( IMAGE_COLLECTION_UI, "BattleNetQuestIcons_Bounty_Generic", tTexFrameIcon );
                break;

            default:
                sTitle = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_H2O_LAYOUT ), "EventRewardTitle" );
                ImageCollectionGetImageInfo( IMAGE_COLLECTION_UI, "BattleNetQuestIcons_Event_Generic", tTexFrameIcon );
                break;
            }
            CRefString sReward;

            sReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardGold" ),
                                         UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD ).str(), NULL );

            if ( nBloodShards > 0 )
            {
                sReward += " ";
                sReward += UIText_ValueToGameCurrency( nBloodShards, CURRENCY_BLOODSHARDS );
            }

            if ( fCanShowXPReward )
            {
                sReward += " ";
                sReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardXP" ),
                                             UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );
            }

            UIEventQueue::Console::DisplayMessageWithIconForPlayer( sTitle, ptPlayer, tTexFrameIcon, &sReward, FALSE, FALSE );
        }


        if ( ptReward->item().gb_handle().gbid() != GBID_NONE )
        {
            idACDItem = CItemGenerate( (ACDNetworkName)0x2e00, ptReward->item(), NULL );

            ActorCommonData & tACDItem = ACDGet( idACDItem );

            if ( !ItemHasLabel( tACDItem.hGB.gbid, 0x71 ) )
            {
                UIEventQueue::Console::ReportItemPickup( ptPlayer, tACDItem );
            }






            ACDDelete( idACDItem, FALSE );
        }
    }
}





void UIQuestComplete::Show( const BOOL fVisible, const D3::Quests::QuestReward * ptReward, BOOL bEquipped )
{


    static BOOL sbItemToDisplay = FALSE;
    static BOOL sbEquipped = FALSE;



    if ( !fVisible && sbItemToDisplay )
    {

        ShowItemReward( sbEquipped ); sbItemToDisplay = FALSE;
        return;
    }







    UI_GetWindowFromID( UIDLG_QUESTREWARD ).SetActive( fVisible );


    UI_GetTextFromID( UITXT_BOUINTYREWARD_KEYBINDING ).SetControllerIndexOverride( Console::GamerProfile::GetPrimaryProfileUserIndex() );
    UI_GetTextFromID( UITXT_QUESTREWARD_KEYBINDING ).SetControllerIndexOverride( Console::GamerProfile::GetPrimaryProfileUserIndex() );


    if ( fVisible )
    {
        m_tQuestReward = *ptReward;


        SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_QUEST_REWARD ), cgtPSODefaults );

        m_dwStartTime = CGameGetClientTicks();

        uint64 nXP = m_tQuestReward.xp_granted() + m_tQuestReward.bonus_xp_granted();
        int32 nGold = m_tQuestReward.gold_granted();
        GBID gbidItemReward = m_tQuestReward.item().gb_handle().gbid();











        SNO snoQuest = m_tQuestReward.sno_quest();
        QuestType eQuestType = QUEST_TYPE_EVENT;
        if ( snoQuest != SNO_NONE )
        {
            ScopedQuestDefinitionLockR pdefQuest( snoQuest );
            eQuestType = pdefQuest->eQuestType;
        }


        CRefString sTitle;
        ImageTextureFrame tTexFrameIcon = cgtImageTextureFrameNull;

        switch ( eQuestType )
        {
        case QUEST_TYPE_MAIN_QUEST:
            sTitle = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_H2O_LAYOUT ), "QuestRewardTitle" );
            QuestGetIcon( GameGetCurrentAct(), snoQuest, TRUE, tTexFrameIcon );
            break;
        case QUEST_TYPE_CHALLENGE:
            sTitle = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_H2O_LAYOUT ), "ChallengeRewardTitle" );
            ImageCollectionGetImageInfo( IMAGE_COLLECTION_UI, "BattleNetQuestIcons_Trial_Generic", tTexFrameIcon );
            break;

        default:
            sTitle = StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_H2O_LAYOUT ), "EventRewardTitle" );
            ImageCollectionGetImageInfo( IMAGE_COLLECTION_UI, "BattleNetQuestIcons_Event_Generic", tTexFrameIcon );
            break;
        }
        UI_GetTextFromID( UITXT_QUESTREWARD_TITLE ).SetText( sTitle, FALSE );
        UI_GetWindowFromID( UIWND_QUESTREWARD_ICON ).SetTextureFrame( tTexFrameIcon );


        UIText & tTextXPGold = UI_GetTextFromID( UITXT_QUESTREWARD_XP_GOLD );
        UIWindow & tWndItemWrapper = UI_GetWindowFromID( UIWND_QUESTREWARD_ITEM_WRAPPER );

        BOOL fCanShowXPReward = UIQuests::CanShowXPReward( nXP );

        if ( ItemHasLabel( gbidItemReward, 0x71 ) && nGold == 0 && !fCanShowXPReward )
        {
            ShowItemReward( bEquipped );
        }
        else if ( PlayerGetLocalPlayerCount() == 1 )
        {

            if ( nGold > 0 || fCanShowXPReward )
            {
                CRefString sReward;

                if ( nGold > 0 )
                {
                    sReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardGold" ),
                                                 UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD ).str(), NULL );
                    sReward += " ";
                }
                if ( fCanShowXPReward )
                {
                    sReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardXP" ),
                                                 UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );
                }
                UI_GetTextFromID( UITXT_QUESTREWARD_XP_GOLD ).SetText( sReward, FALSE );
                tTextXPGold.SetActive( TRUE );


                tWndItemWrapper.SetActive( FALSE );


                if ( gbidItemReward != GBID_NONE )
                    sbItemToDisplay = TRUE;
            }


            else if ( gbidItemReward != GBID_NONE )
            {

                ShowItemReward( bEquipped ); sbItemToDisplay = FALSE;
            }
            else
            {

                UI_GetWindowFromID( UIDLG_QUESTREWARD ).SetActive( FALSE );
            }
        }
        else
        {

            tWndItemWrapper.SetActive( FALSE );
            tTextXPGold.SetActive( FALSE );
        }





























































































































    }
}




void UIQuestComplete::HandleItemTooltip( const UIID & id )
{
    ACDID idACDItem;
    const D3::Quests::QuestReward & tReward = theQuestCompleteUI.m_tQuestReward;

    if ( id == UIBTN_QUESTREWARD_ITEM )
    {
        if ( tReward.item().gb_handle().gbid() != GBID_NONE )
        {
            idACDItem = CItemGenerate( (ACDNetworkName)0x2e00, tReward.item(), NULL );














            if ( idACDItem != ACDID_INVALID )
            {
                ActorCommonData & tACDItem = ACDGet( idACDItem );
                ActorCommonData & tACDPlayer = ACDGet( LocalPlayerACDGet() );


                ItemTooltipParameterBlock tParams;
                UIInventory::GetItemTooltipParameters( tParams, tACDItem, ItemTooltipPlayerData( &tACDPlayer, NULL ), FALSE, FALSE );


                ItemTooltipData tTipData;
                UIInventory::InitItemTooltip( tTipData, ACDID_INVALID, tACDItem.hGB.gbid, tParams, FALSE, NULL, FALSE );
                tTipData.uszInstruction = NULL;
                gtTooltipMgr.SetItemToolTip( id, tTipData, TRUE, TRUE, NULL, FALSE );


                ACDDelete( idACDItem, FALSE );
            }
        }
    }
}


void UIQuestComplete::Update()
{

    UIWindow & tDlg = UI_GetWindowFromID( UIDLG_QUESTREWARD );
    if ( tDlg.IsActive() )
    {

        uint32 dwElapsedTime = CGameGetClientTicks() - m_dwStartTime;
        if ( dwElapsedTime > QUEST_REWARD_DURATION )
        {

            Show( FALSE, NULL, FALSE );
        }
    }
}


void UIBountyReward::Close( const UIID & id )
{





















    theBountyRewardUI.ConsoleShow( FALSE, NULL );
}



void UIBountyReward::ConsoleClose( const UIID & id, const int32 )
{



    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_SELECT_GENERIC ), cgtPSODefaults );
    Close( id );
}
typedef blz::vector<D3::Items::Generator> RewardVector;

static void sUpdateRewardItemBtn( const UIID & idItemBtn, const D3::Items::Generator * tItem )
{



    if ( UI_TestID( idItemBtn ) )
    {



        UIItemButton & tBtnItem = UI_GetItemButtonFromID( idItemBtn );

        if ( tItem )
        {
            tBtnItem.SetFromItemGenerator( *tItem );
            tBtnItem.SetActive( TRUE );



            tBtnItem.SetDrawStackCount( TRUE );
        }
        else
        {




            tBtnItem.SetActive( FALSE );
        }
    }
}





void UIBountyReward::ConsoleShow( const BOOL fVisible, const D3::Quests::QuestReward * ptReward )
{


    if ( !fVisible || ptReward == NULL )
    {
        UI_GetWindowFromID( UIDLG_BOUNTYREWARD ).SetActive( FALSE );
        return;
    }

    m_tQuestReward = *ptReward;

    UI_GetWindowFromID( UIDLG_BOUNTYREWARD ).SetActive( TRUE );

    UIID idStoryboard = UIIDCreateFromParent( UIDLG_BOUNTYREWARD, ".OnLoad" );
    UI_GetStoryboardFromID( idStoryboard ).Play( 0, 0.0f );

    UIID idBase = UIIDCreateFromParent( UIDLG_BOUNTYREWARD, ".LayoutRoot.BountyWrapper" );
    UIID idOther = UIIDCreateFromParent( UIDLG_BOUNTYREWARD, ".LayoutRoot.BonusBountyWrapper" );

    BOOL bIsBonus = FALSE;
    for ( int32 i = 0; i < m_tQuestReward.items_size(); i++ )
    {
        GBID gbidRewardItem = m_tQuestReward.items( i ).gb_handle().gbid();


        if ( gbidRewardItem == GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ) )
        {
            continue;
        }

        GBHandle hgbItem( GB_ITEMS, gbidRewardItem );
        SNO snoActor = GameBalanceGetActorSNO( hgbItem );

        if ( snoActor == GlobalSNOGet( GLOBALSNO_HORADRIC_CACHE_BONUS ) )
        {
            UIID idTemp = idBase;
            idBase = idOther;
            idOther = idTemp;
            bIsBonus = TRUE;
            break;
        }
    }


    UI_GetWindowFromID( idBase ).SetActive( TRUE );
    UI_GetWindowFromID( idOther ).SetActive( FALSE );

    UIID idRewardsText = UIIDCreateFromParent( idBase, ".RewardsText" );

    CRefString sGoldXPReward;
    int32 nGold = ptReward->gold_granted();
    uint64 nXP = ptReward->xp_granted() + ptReward->bonus_xp_granted();
    int32 nBloodShards = 0;

    for ( int32 i = 0; i < m_tQuestReward.items_size(); i++ )
    {
        if ( m_tQuestReward.items( i ).gb_handle().gbid() == GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ) )
        {
            nBloodShards += (int32)m_tQuestReward.items( i ).stack_size();
        }
    }

    if ( nGold > 0 )
    {
        sGoldXPReward += UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD );


        CRefString sGoldLog = LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardLogGold" ),
                                                UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD ).str(), NULL );

        UI_Print( sGoldLog, TRUE );
    }

    if ( nXP > 0 )
    {
        if ( sGoldXPReward.Length() )
        {
            sGoldXPReward += "    ";
        }

        sGoldXPReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "BountiesRewardXP" ),
                                           UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );

        CRefString sXPLog = LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardLogXP" ),
                                              UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );

        UI_Print( sXPLog, TRUE );
    }

    if ( nBloodShards > 0 )
    {
        if ( sGoldXPReward.Length() )
        {
            sGoldXPReward += "    ";
        }

        sGoldXPReward += UIText_ValueToGameCurrency( nBloodShards, CURRENCY_BLOODSHARDS );
    }

    UI_GetTextFromID( idRewardsText ).SetText( sGoldXPReward, FALSE );

    UIID idItemBG = UIIDCreateFromParent( idBase, ".Rewards.Items" );
    UIWindow & tItemBG = UI_GetWindowFromID( idItemBG );

    if ( m_tQuestReward.item().gb_handle().gbid() != GBID_NONE )
    {
        tItemBG.SetActive( TRUE );

        const UIControlItemArray & tChildList = UI_GetWindowFromID( idItemBG ).GetChildList();
        if ( tChildList.size() > 1 )
        {
            sUpdateRewardItemBtn( tChildList[0].id, &m_tQuestReward.item() );


            for ( size_t i = 1; i < tChildList.size(); i++ )
            {
                sUpdateRewardItemBtn( tChildList[i].id, NULL );
            }
        }
    }
    else if ( m_tQuestReward.items_size() > 0 )
    {
        tItemBG.SetActive( TRUE );
        RewardVector vecRewards;



        for ( int32 i = 0; i < m_tQuestReward.items_size(); i++ )
        {
            bool bFound = false;
            if ( m_tQuestReward.items( i ).gb_handle().gbid() == GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ) )
            {
                continue;
            }


            for ( RewardVector::iterator iter = vecRewards.begin(); iter != vecRewards.end(); ++iter )
            {

                if ( iter->gb_handle().gbid() == m_tQuestReward.items( i ).gb_handle().gbid() )
                {

                    iter->set_stack_size( iter->stack_size() + m_tQuestReward.items( i ).stack_size() );
                    bFound = true;
                }
            }



            if ( !bFound )
            {

                vecRewards.push_back( m_tQuestReward.items( i ) );
            }
        }


        UIID idRewardItem = UIIDCreateFromParent( idBase, ".Rewards.Items.ItemReward" );
        sUpdateRewardItemBtn( idRewardItem, &vecRewards[0] );
        if ( bIsBonus )
        {
            UIID idBonusText = UIIDCreateFromParent( idBase, ".RewardDTextBonus" );
            UI_GetTextFromID( idBonusText ).SetText( StringListFetch( "ItemDescriptions:BonusAct1Bag" ), FALSE );
        }
    }
    else
    {
        tItemBG.SetActive( FALSE );
    }
}























































































































































BOOL UIBountyReward::IsVisible()
{


    return UI_GetWindowFromID( UIDLG_BOUNTYREWARD ).IsActive();
}









































// X/Client/Console/LobbyService.cpp:661 (same declaration as src/X/Client/Console/SaveMigration.cpp:85)
namespace Console {
namespace Online {
BOOL IsSeasonsInitialized();
}
}



void UITieredRiftReward::Close( const UIID & id )
{



    theTieredRiftRewardUI.Hide();
}



void UITieredRiftReward::ConsoleClose( const UIID & id, const int32 )
{



    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_SELECT_GENERIC ), cgtPSODefaults );
    Close( id );
}




void UITieredRiftReward::ConsoleOpenLeaderboards( const UIID & id, const int32 nControllerIndex )
{



    Close( id );

    Console::Screens::OpenToScreen( Console::Screens::LEADERBOARD_SCREEN_SET, 0, nControllerIndex, FALSE );
}

// Declared here because their headers are not adopted yet; each comment names the defining file.
extern UIID UIWND_TIEREDRIFTREWARD_LEADERBOARD_PROMPT, UIWND_TIEREDRIFTREWARD_NORMAL_PROMPT;  // UIGlobals.cpp:15

// NON_MATCHING: 565 vs 564 insns; regalloc only: original spills nGold (ldrsw+str [sp,#0x10]) and keeps nNewBloodShardCap in w20, ours spills nNewBloodShardCap; calls, constants and control flow identical
void UITieredRiftReward::Show(
    const D3::Quests::QuestReward * ptReward,
    const int32 nNewBloodShardCap)
{
    if (ptReward == NULL)
    {
        Hide();
        return;
    }

    UIID idRewardScreen = UIDLG_TIEREDRIFTREWARD;

    if (nNewBloodShardCap > 0)
    {
        idRewardScreen = UIDLG_TIEREDRIFTREWARD_BLOODSHARD_INCREASE;
    }

    if ( Console::Online::IsSeasonsInitialized() && OnlineService::GetSeasonState() == OnlineService::SEASON_STATE_ACTIVE )
    {
        UI_GetWindowFromID( UIWND_TIEREDRIFTREWARD_LEADERBOARD_PROMPT ).SetActive( TRUE );
        UI_GetWindowFromID( UIWND_TIEREDRIFTREWARD_NORMAL_PROMPT ).SetActive( FALSE );
    }
    else
    {
        UI_GetWindowFromID( UIWND_TIEREDRIFTREWARD_LEADERBOARD_PROMPT ).SetActive( FALSE );
        UI_GetWindowFromID( UIWND_TIEREDRIFTREWARD_NORMAL_PROMPT ).SetActive( TRUE );
    }






    m_tQuestReward = *ptReward;

    UI_GetWindowFromID( idRewardScreen ).SetActive( TRUE );

    UIID idStoryboard;
    idStoryboard.SetFromParent( idRewardScreen, "OnLoad" );
    UI_GetStoryboardFromID( idStoryboard ).Play( 0, 0.0f );

    UIID idRewardsText;
    idRewardsText.SetFromParent( idRewardScreen, "LayoutRoot.RewardsText" );

    CRefString sGoldXPReward;
    int32 nGold = ptReward->gold_granted();
    uint64 nXP = ptReward->xp_granted() + ptReward->bonus_xp_granted();
    int32 nBloodShards = 0;
    int32 nIndexNonCurrencyItemReward = -1;

    for ( int32 i = 0; i < m_tQuestReward.items_size(); i++ )
    {
        if (m_tQuestReward.items( i ).gb_handle().gbid() == GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ))
        {
            nBloodShards += (int32)m_tQuestReward.items( i ).stack_size();
        }
        else if (nIndexNonCurrencyItemReward == -1)
        {
            nIndexNonCurrencyItemReward = i;
        }
    }

    if (m_tQuestReward.item().gb_handle().gbid() == GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ))
    {
        nBloodShards += (int32)m_tQuestReward.item().stack_size();
    }

    if (nNewBloodShardCap > 0)
    {
        sGoldXPReward += UnicodeText::FormatNumber( nNewBloodShardCap, TRUE, FNS_NORMAL, 0 );
        sGoldXPReward += " {icon:BloodshardUpgradeIcon}";
    }

    if (nGold > 0)
    {
        if (sGoldXPReward.Length())
        {
            sGoldXPReward += "    ";
        }

        sGoldXPReward += UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD );


        CRefString sGoldLog = LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardLogGold" ),
                                                UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD ).str(), NULL );

        UI_Print( sGoldLog, TRUE );
    }

    if (nXP > 0)
    {
        if (sGoldXPReward.Length())
        {
            sGoldXPReward += "    ";
        }

        sGoldXPReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "BountiesRewardXP" ),
                                           UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );

        CRefString sXPLog = LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardLogXP" ),
                                              UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );

        UI_Print( sXPLog, TRUE );
    }

    if (nBloodShards > 0)
    {
        if (sGoldXPReward.Length())
        {
            sGoldXPReward += "    ";
        }

        sGoldXPReward += UIText_ValueToGameCurrency( nBloodShards, CURRENCY_BLOODSHARDS );
    }

    UI_GetTextFromID( idRewardsText ).SetText( sGoldXPReward, FALSE );



    UIID idItemBG = UIIDCreateFromParent( idRewardScreen, ".LayoutRoot.Rewards.Item" );
    UIWindow & tItemBG = UI_GetWindowFromID( idItemBG );

    UIID idItemReward = UIIDCreateFromParent( idItemBG, ".ItemReward" );
    UIItemButton & tBtnItem = UI_GetItemButtonFromID( idItemReward );





    CRefString sItemName;
    const D3::Items::Generator * ptGenerator = NULL;
    if (m_tQuestReward.item().gb_handle().gbid() != GBID_NONE &&
        m_tQuestReward.item().gb_handle().gbid() != GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ))
    {
        ptGenerator = &m_tQuestReward.item();

        tItemBG.SetActive( TRUE );

        tBtnItem.SetFromItemGenerator( m_tQuestReward.item() );
        tBtnItem.SetActive( TRUE );
        tBtnItem.SetDrawStackCount( FALSE );
    }
    else if (nIndexNonCurrencyItemReward != -1 && m_tQuestReward.items_size() > 0)
    {
        ptGenerator = &m_tQuestReward.items( nIndexNonCurrencyItemReward );

        tItemBG.SetActive( TRUE );

        tBtnItem.SetFromItemGenerator( m_tQuestReward.items( nIndexNonCurrencyItemReward ) );
        tBtnItem.SetActive( TRUE );
        tBtnItem.SetDrawStackCount( TRUE );

        sItemName = ItemGetDisplayName( m_tQuestReward.items( nIndexNonCurrencyItemReward ).gb_handle().gbid() );



        const D3::Items::Generator & tItemReward = m_tQuestReward.items( nIndexNonCurrencyItemReward );
        GBID gbidItemReward = tItemReward.gb_handle().gbid();
        uint64 uStackSize = tItemReward.stack_size();
        for ( int32 i = 0; i < m_tQuestReward.items_size(); i++ )
        {
            if (i != nIndexNonCurrencyItemReward)
            {
                const D3::Items::Generator & tItem = m_tQuestReward.items( i );
                GBID gbidItem = tItem.gb_handle().gbid();
                if (gbidItem != GBID_NONE && gbidItem == gbidItemReward)
                {
                    uStackSize += tItem.stack_size();
                }
            }
        }
        if (uStackSize > tItemReward.stack_size())
        {
            tBtnItem.SetStackCount( uStackSize );
        }
    }
    else
    {
        tItemBG.SetActive( FALSE );
        tBtnItem.SetActive( FALSE );
    }





    if (ptGenerator != NULL)
    {
        ACDID idACDItem = CItemGenerate( (ACDNetworkName)0x2e00, *ptGenerator, NULL );
        if (idACDItem != ACDID_INVALID)
        {
            ActorCommonData & tACDItem = ACDGet( idACDItem );
            sItemName = ItemGetDisplayName( tACDItem, TRUE, FALSE, FALSE, TRUE );
            ACDDelete( idACDItem, FALSE );
        }
    }
    UIID idItemName = UIIDCreateFromParent( idRewardScreen, ".LayoutRoot.ItemText" );
    UI_GetTextFromID( idItemName ).SetText( sItemName, FALSE );




}



void UITieredRiftReward::Hide()
{


    UI_GetWindowFromID( UIDLG_TIEREDRIFTREWARD ).SetActive( FALSE );

    UI_GetWindowFromID( UIDLG_TIEREDRIFTREWARD_BLOODSHARD_INCREASE ).SetActive( FALSE );
}



BOOL UITieredRiftReward::IsVisible()
{


    return UI_GetWindowFromID( UIDLG_TIEREDRIFTREWARD ).IsActive() ||

           UI_GetWindowFromID( UIDLG_TIEREDRIFTREWARD_BLOODSHARD_INCREASE ).IsActive();
}





















































void UITieredRiftScorecard::Close( const UIID & id )
{



    theTieredRiftScorecardUI.Hide();
}



void UITieredRiftScorecard::ConsoleClose( const UIID & id, const int32 )
{



    SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_SELECT_GENERIC ), cgtPSODefaults );
    Close( id );
}


void UITieredRiftScorecard::Show(
    const D3::Quests::QuestReward * ptReward,
    const int32 nRiftDepth)
{
    if ( ptReward == NULL )
    {
        Hide();
        return;
    }

    m_tQuestReward = *ptReward;

    UI_GetWindowFromID( UIDLG_TIEREDRIFTSCORECARD ).SetActive( TRUE );
    UI_GetStoryboardFromID( UIIDCreateFromParent( UIDLG_TIEREDRIFTSCORECARD, ".OnLoad" ) ).Play( 0, 0.0f );
    UI_GetTextFromID( UIIDCreateFromParent( UIDLG_TIEREDRIFTSCORECARD, ".LayoutRoot.DepthNumber" ) ).SetText( UnicodeText::FormatNumber( nRiftDepth, TRUE, FNS_NORMAL, 0 ), FALSE );

    int32 nGold = ptReward->gold_granted();
    uint64 nXP = ptReward->xp_granted() + ptReward->bonus_xp_granted();
    int32 nBloodShards = 0;

    for ( int32 i = 0; i < m_tQuestReward.items_size(); i++ )
    {
        if ( m_tQuestReward.items( i ).gb_handle().gbid() == GlobalGBIDGet( GLOBALGBID_CURRENCY_BLOODSHARDS ) )
        {
            nBloodShards += (int32)m_tQuestReward.items( i ).stack_size();
        }
    }

    CRefString sXPReward;
    if ( nXP > 0 )
    {
        sXPReward += LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "BountiesRewardXP" ),
                                       UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );

        CRefString sXPLog = LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardLogXP" ),
                                              UnicodeText::FormatNumber( nXP, TRUE, FNS_NORMAL, 0 ).str(), NULL );

        UI_Print( sXPLog, TRUE );
    }
    UI_GetTextFromID( UIIDCreateFromParent( UIDLG_TIEREDRIFTSCORECARD, ".LayoutRoot.XPReward" ) ).SetText( sXPReward, FALSE );

    CRefString sGoldReward;
    if ( nGold > 0 )
    {
        sGoldReward += UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD );
        CRefString sGoldLog = LocalizedSprintf( StringListFetch( GlobalSNOGet( GLOBALSNO_STRINGLIST_QUESTLOG ), "QuestRewardLogGold" ),
                                                UIText_ValueToGameCurrency( nGold, CURRENCY_GOLD ).str(), NULL );
        UI_Print( sGoldLog, TRUE );
    }

    if ( nBloodShards > 0 )
    {
        if ( sGoldReward.Length() )
        {
            sGoldReward += "    ";
        }

        sGoldReward += UIText_ValueToGameCurrency( nBloodShards, CURRENCY_BLOODSHARDS );
    }

    UI_GetTextFromID( UIIDCreateFromParent( UIDLG_TIEREDRIFTSCORECARD, ".LayoutRoot.GoldReward" ) ).SetText( sGoldReward, FALSE );
}



void UITieredRiftScorecard::Hide()
{


    UI_GetWindowFromID( UIDLG_TIEREDRIFTSCORECARD ).SetActive( FALSE );
}
