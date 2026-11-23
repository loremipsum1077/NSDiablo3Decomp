// Original file: X/Client/UserInterface/Interface/UICredits.cpp
// Not written: ClickFaster/ClickSlower/ClickPause (688-716) and sUpdateButtonsForScrollSpeed (666), all linker-removed.
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
#include "../../CGame.h"
#include "../../Input/Input.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/RefString.h"
#include "../Console/ConsoleScreenManager.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIStackPanelControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../UIStoryboard.h"
#include <string.h>
// Declared here until their headers carry them (signatures from the DWARF / mangled names).
extern UIID UIWND_CREDITS, UIWND_CREDITS_X1, UIWND_CREDITS_2017, UIWND_CREDITS_ETERNAL;  // UIGlobals.cpp
extern UIID UIWND_CONSOLE_CREDITS_SOUND, UISTORYBOARD_CONSOLE_CREDITS_SOUND_FADE_IN, UISTORYBOARD_CONSOLE_CREDITS_SOUND_FADE_OUT;
UIStoryboard & UI_GetStoryboardFromID( const UIID & id );  // UI.cpp
UIStackPanel & UI_GetStackPanelFromID( const UIID & id );  // UI.cpp:1292
void UI_DeleteControl( UIID & idControl, const BOOL fAssertIfDeletingRoot );  // UI.cpp (_Z16UI_DeleteControlR4UIIDi)
void UI_SetPrimaryUIController( int32 nControllerIndex );  // UI.cpp (_Z25UI_SetPrimaryUIControlleri)
void CWorldDisableAreaMusic( const BOOL fDisabled );  // X/Client/CWorld.cpp:1374
void GameStartMenu( Act eLoadingScreenAct );  // X/Common/Game.cpp:2672
namespace Console {
namespace GamerProfile { int32 GetPrimaryProfileUserIndex(); }
namespace Screens { BOOL OpenToScreen( const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition ); }  // ConsoleScreenManager.cpp:1104
}
namespace SoundManager { void SuppressSounds( const uint32 dwSuppressedTypes ); void StopMusic(); void PlayMusic( const SNO snoMusic, const int32 nFirstIndexToPlay, const BOOL fDisableShuffle ); }  // Sound.cpp:1001, 2732
namespace UICredits
{

namespace
{


static uint32 dwBaseTick = (uint32)-1;
static float fBaseOffset;
static uint32 dwLineNumber;




static int32 nLastTriggerPos;


static const uint32 DELAY_AT_END = 60;


static const uint32 NUM_SCROLL_SPEEDS = 4;
static float arScrollSpeeds[NUM_SCROLL_SPEEDS] = { 0.0f, 1.65f, 5.0f, 10.0f };
static int32 nScrollSpeed = 1;



static BOOL bSkipped;
static const Blizzard::Time::Millisecond CONSOLE_SOUND_CREDITS_DURATION = 3000;
static Blizzard::Time::Millisecond msConsoleSoundCreditsStartTime;
static BOOL bStartedSoundCreditsDisplay;

static SNO snoCurCreditsStringlist = SNO_NONE;
static SNO snoCurCredits = SNO_NONE;
static UIID idCurCreditsWindow = GetUIIDNone();





static int32 nDebugNameCount;
}  // namespace

// Forward declarations (the file's own functions, defined below on their DWARF lines).
void sReset();
void sSetupCreditsList();
void sRemoveObsoleteControls();
void sUpdateCreditsList();
void Show();
void Hide();
void Pause();
void sSetScrollSpeed( int32 nNewSpeed );
namespace Console { void Skip( const UIID &, const int32 ); void Pause( const UIID &, const int32 ); void Show( const int32 nControllerIndex ); void ShowConsoleCreditsSound(); void OnConsoleCreditsSoundShowFinished( const UIID & ); void OnConsoleCreditsSoundUpdate( const UIID & ); void OnConsoleCreditsSoundHideFinished( const UIID & ); void OnConsoleCreditsComplete(); }


const CHAR * szTitleTag = "<Title>"; int32 nTitleTagLen = (int32)strlen( szTitleTag );
const CHAR * szHeading1Tag = "<Heading1>"; int32 nHeading1TagLen = (int32)strlen( szHeading1Tag );
const CHAR * szHeading2Tag = "<Heading2>"; int32 nHeading2TagLen = (int32)strlen( szHeading2Tag );
const CHAR * szHeading3Tag = "<Heading3>"; int32 nHeading3TagLen = (int32)strlen( szHeading3Tag );
const CHAR * szleftTag = "<left>"; int32 nleftTagLen = (int32)strlen( szleftTag );
const CHAR * szcol1Tag = "<col1>"; int32 ncol1TagLen = (int32)strlen( szcol1Tag );
const CHAR * szcol2Tag = "<col2>"; int32 ncol2TagLen = (int32)strlen( szcol2Tag );
const CHAR * sznameListTag = "<nameList>"; int32 nnameListTagLen = (int32)strlen( sznameListTag );
// Inference: statically initialised to 2 in the image (.data, no sub_I store); "<>" is the literal line 289 compares with.
int32 nTextLength = (int32)strlen( "<>" );











void InitializeForApp()
{


    idCurCreditsWindow = GetUIIDNone();
    snoCurCreditsStringlist = SNO_NONE;
    snoCurCredits = SNO_NONE;
}


void sReset()
{
    UIID idCreditNames = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body.credits_names" );

    UIWindow & tCreditsNames = UI_GetWindowFromID( idCreditNames );
    const UIControlItemArray & tUIIDArray = tCreditsNames.GetChildList();
    for ( int32 i = (int32)tUIIDArray.size() - 1; i >= 0; i-- )
    {
        UIID idChild = tUIIDArray[i].id;
        UI_DeleteControl( idChild, TRUE );
    }
    // Inference: dwLineNumber = 0 comes from the original's 8-byte zero store over fBaseOffset/dwLineNumber.
    dwBaseTick = CGameGetRealTicks();
    fBaseOffset = 0.0f;
    dwLineNumber = 0;
    nScrollSpeed = 1;
}




void sSetupCreditsList()
{

    if ( snoCurCreditsStringlist == SNO_NONE )
    {

        snoCurCreditsStringlist = GlobalSNOGet( GLOBALSNO_STRINGLIST_CREDITS_2017_CONSOLE );
    }




    if ( snoCurCredits == SNO_NONE )
    {
        snoCurCredits = GlobalSNOGet( GLOBALSNO_UI_CREDITS_2017 );
    }

    ScopedStringListDefinitionLockR pdefStringList( snoCurCreditsStringlist );
    StringTableEntry * pStrings = pdefStringList->arStrings;
    uint32 dwStringCount = pdefStringList->dwStringCount;


    UIID idCreditsTitle = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body.credits_title" );

    UIText & tCreditsTitle = UI_GetTextFromID( idCreditsTitle );
    CRefString usTitle( dwStringCount ? pStrings[0].szText : "" );
    tCreditsTitle.SetText( usTitle.str() + nTitleTagLen, FALSE );


    dwLineNumber = 1;

}


void sRemoveObsoleteControls()
{

    Rect2D rectMain;
    UI_GetWindowFromID( idCurCreditsWindow ).GetWindowRect( rectMain );




    UIID idCreditsNames = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body.credits_names" );
    UIWindow & tCreditsNames = UI_GetWindowFromID( idCreditsNames );
    const UIControlItemArray & tUIIDArray = tCreditsNames.GetChildList();
    for ( int32 i = 0; i < (int32)tUIIDArray.size() - 1; i++ )
    {
        UIID idChild = tUIIDArray[i].id;
        UIWindow * ptChild = &UI_GetWindowFromID( idChild );

        Rect2D rectChild;
        ptChild->GetWindowRect( rectChild );
        if ( rectChild.bottom < rectMain.top )
        {
            UI_DeleteControl( idChild, TRUE );
        }
        else
        {
            break;
        }
    }
}

// NON_MATCHING: 2196 bytes both; only the <col1> branch differs: the original computes the Copy length (nLen + 1) before the alloca's stack adjustment, ours after (12 insns reordered). Tried nLen with +1 folded in, alloca( nLen + sizeof( char ) ) (shares the add, 16 bytes shorter).
void sUpdateCreditsList()
{
    // Inference: the default is assigned first; the original selects it with a csel on line 224.
    LPCSTR szEndPath = "credits_dialog_main";
    if ( snoCurCredits == GlobalSNOGet( GLOBALSNO_UI_CREDITS_X1 ) )
    {
        szEndPath = "credits_dialog_main_X1";
    }
    else if ( snoCurCredits == GlobalSNOGet( GLOBALSNO_UI_CREDITS_2017 ) )
    {
        szEndPath = "credits_dialog_main_2017";
    }
    else if ( snoCurCredits == GlobalSNOGet( GLOBALSNO_UI_CREDITS_ETERNAL ) )
    {
        szEndPath = "credits_dialog_main_eternal";
    }

    UIControlSourceKey tHeaderOneTemplateSrcKey( snoCurCredits, CRefString().Format( "%s.header_one_text", szEndPath ) );
    UIControlSourceKey tHeaderTwoTemplateSrcKey( snoCurCredits, CRefString().Format( "%s.header_two_text", szEndPath ) );
    UIControlSourceKey tHeaderThreeTemplateSrcKey( snoCurCredits, CRefString().Format( "%s.header_three_text", szEndPath ) );
    UIControlSourceKey tNameTemplateSrcKey( snoCurCredits, CRefString().Format( "%s.name_text", szEndPath ) );
    UIControlSourceKey tVoiceCreditTemplateSrcKey( snoCurCredits, CRefString().Format( "%s.voice_credit", szEndPath ) );
    UIControlSourceKey tDisclaimerTemplateSrcKey( snoCurCredits, CRefString().Format( "%s.disclaimer_text", szEndPath ) );
    UIControlSourceKey tWordWrapTemplateSrcKey( snoCurCredits, CRefString().Format( "%s.name_text_wordWrap", szEndPath ) );

    ScopedStringListDefinitionLockR pdefStringList( snoCurCreditsStringlist );
    StringTableEntry * pStrings = pdefStringList->arStrings;
    uint32 dwStringCount = pdefStringList->dwStringCount;


    sRemoveObsoleteControls();


    UIID idCreditsNames = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body.credits_names" );
    UIWindow & tNames = UI_GetWindowFromID( idCreditsNames );
    for ( ; dwLineNumber < dwStringCount; dwLineNumber++ )
    {












        UIID idLastEntry = UIIDCreateFromParent( tNames.GetID(), CRefString().Format( ".%d", dwLineNumber - 1 ) );
        if ( UI_TestID( idLastEntry ) )
        {
            Rect2D rectMain;
            UI_GetWindowFromID( idCurCreditsWindow ).GetWindowRect( rectMain );

            UIWindow & tLastEntry = UI_GetWindowFromID( idLastEntry );
            Rect2D rectEntry;
            tLastEntry.GetWindowRect( rectEntry );
            if ( rectEntry.top > rectMain.bottom )
            {
                break;
            }
        }



        Vector2D vecTLNames, vecDimNames;
        tNames.GetRelativePosition( vecTLNames, vecDimNames );

        Vector2D vecOrigin( 0.0f, vecDimNames.y );
        const int32 nHeightLineBreak = 36;  // Inference: 36 and 6 are the two float constants the blocks below add; which name holds which is a guess.
        const int32 nHeightSmallSpace = 6;

        CRefString usLabel( CRefString().Format( "%u", dwLineNumber ) );
        CRefString usText( pStrings[dwLineNumber].szText );


        if ( strncmp( "<>", usText.str(), nTextLength ) == 0 )
        {
            vecOrigin.y += nHeightSmallSpace;
            UIControl * ptControl = UI_CreateControlFromTemplate( tNameTemplateSrcKey, DynamicControlCreationParams( usLabel.str(), idCreditsNames ), &vecOrigin );
            UIText * ptText = &UI_GetTextFromID( ptControl->GetID() );
            ptText->SetText( usText.str() + nTextLength, FALSE );
            ptText->SetSnapToPixelCenter( FALSE );
        }
        else if ( strncmp( sznameListTag, usText.str(), nnameListTagLen ) == 0 )
        {
            vecOrigin.y += nHeightSmallSpace;
            UIControl * ptControl = UI_CreateControlFromTemplate( tWordWrapTemplateSrcKey, DynamicControlCreationParams( usLabel.str(), idCreditsNames ), &vecOrigin );
            UIText * ptText = &UI_GetTextFromID( ptControl->GetID() );
            ptText->SetText( usText.str() + nnameListTagLen, FALSE );
            ptText->SetSnapToPixelCenter( FALSE );
        }
        else if ( strncmp( szHeading3Tag, usText.str(), nHeading3TagLen ) == 0 )
        {
            vecOrigin.y += nHeightLineBreak;
            UIControl * ptControl = UI_CreateControlFromTemplate( tHeaderThreeTemplateSrcKey, DynamicControlCreationParams( usLabel.str(), idCreditsNames ), &vecOrigin );
            UIText * ptText = &UI_GetTextFromID( ptControl->GetID() );
            ptText->SetText( usText.str() + nHeading3TagLen, FALSE );
            ptText->SetSnapToPixelCenter( FALSE );
        }
        else if ( strncmp( szHeading2Tag, usText.str(), nHeading2TagLen ) == 0 )
        {
            vecOrigin.y += nHeightLineBreak;
            UIControl * ptControl = UI_CreateControlFromTemplate( tHeaderTwoTemplateSrcKey, DynamicControlCreationParams( usLabel.str(), idCreditsNames ), &vecOrigin );
            UIText * ptText = &UI_GetTextFromID( ptControl->GetID() );
            ptText->SetText( usText.str() + nHeading2TagLen, FALSE );
            ptText->SetSnapToPixelCenter( FALSE );
        }
        else if ( strncmp( szHeading1Tag, usText.str(), nHeading1TagLen ) == 0 )
        {
            vecOrigin.y += nHeightLineBreak;
            UIControl * ptControl = UI_CreateControlFromTemplate( tHeaderOneTemplateSrcKey, DynamicControlCreationParams( usLabel.str(), idCreditsNames ), &vecOrigin );
            UIText * ptText = &UI_GetTextFromID( ptControl->GetID() );
            ptText->SetText( usText.str() + nHeading1TagLen, FALSE );
            ptText->SetSnapToPixelCenter( FALSE );
        }
        else if ( strncmp( szleftTag, usText.str(), nleftTagLen ) == 0 )
        {
            vecOrigin.y += nHeightLineBreak;
            UIControl * ptControl = UI_CreateControlFromTemplate( tDisclaimerTemplateSrcKey, DynamicControlCreationParams( usLabel.str(), idCreditsNames ), &vecOrigin );
            UIText * ptText = &UI_GetTextFromID( ptControl->GetID() );
            ptText->SetText( usText.str() + nleftTagLen, FALSE );
            ptText->SetSnapToPixelCenter( FALSE );
        }
        else if ( strncmp( szcol1Tag, usText.str(), ncol1TagLen ) == 0 )
        {
            const char * pChar = Blizzard::String::Find( usText.str(), szcol2Tag );
            if ( pChar )
            {
                const char * ptStartLeft = usText.str() + ncol1TagLen;
                int32 nLen = (int32)( pChar - ptStartLeft );
                char * szTemp = (char *)alloca( nLen + 2 );
                Blizzard::String::Copy( szTemp, ptStartLeft, nLen + 1 );



                UIControl * ptControl = UI_CreateControlFromTemplate( tVoiceCreditTemplateSrcKey, DynamicControlCreationParams( usLabel.str(), idCreditsNames ), NULL );
                UIStackPanel * ptRow = &UI_GetStackPanelFromID( ptControl->GetID() );

                Vector2D vecTL, vecDim;
                ptRow->GetRelativePosition( vecTL, vecDim );
                vecTL.y = vecOrigin.y + nHeightSmallSpace;
                ptRow->SetRelativePosition( vecTL, vecDim );

                UIID idTextLeft = UIIDCreateFromParent( ptRow->GetID(), ".left" );
                UIText * ptTextLeft = &UI_GetTextFromID( idTextLeft );

                UIID idTextRight = UIIDCreateFromParent( ptRow->GetID(), ".right" );
                UIText * ptTextRight = &UI_GetTextFromID( idTextRight );

                ptTextLeft->SetText( szTemp, FALSE );
                ptTextLeft->SetSnapToPixelCenter( FALSE );

                ptTextRight->SetText( pChar + ncol2TagLen, FALSE );
                ptTextRight->SetSnapToPixelCenter( FALSE );
            }
        }
    }
}





























































void Show()
{

    if ( idCurCreditsWindow == GetUIIDNone() )
    {
        idCurCreditsWindow = UIWND_CREDITS_2017;
    }

    if ( UI_TestID( idCurCreditsWindow ) )
    {



        if ( GameIsStartUp() )
        {




            ::Console::Screens::CloseScreenSet( FALSE, TRUE );
        }






        // Inference: no named sound-type mask was found for ~2 (cf. CGame.cpp's ~(uint32)6).
        uint32 dwSuppressFlag = ~(uint32)2;
        SoundManager::SuppressSounds( dwSuppressFlag );
        SoundManager::PlayMusic( GlobalSNOGet( GLOBALSNO_MUSIC_CREDITS ), 0, FALSE );
        CWorldDisableAreaMusic( TRUE );

        sSetupCreditsList();
        UIID idCreditsBody = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body" );
        UI_GetWindowFromID( idCreditsBody ).SetActive( FALSE );
        UI_GetWindowFromID( idCurCreditsWindow ).SetActive( TRUE );

        UIID idCreditsFadeIn = UIIDCreateFromParent( idCurCreditsWindow, ".OnLoad" );
        UI_GetStoryboardFromID( idCreditsFadeIn ).Play( FALSE, 0.0f );


        fBaseOffset = 0.0f;
        dwBaseTick = CGameGetRealTicks();
        nScrollSpeed = 1;
    }
}


BOOL IsActive()
{
    if ( idCurCreditsWindow == GetUIIDNone() )
    {
        return FALSE;
    }
    return UI_GetWindowFromID( idCurCreditsWindow ).IsActive();
}






void OnHideFinished( const UIID & )
{







    UI_GetWindowFromID( idCurCreditsWindow ).SetActive( FALSE );



    UIID idCreditNames = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body.credits_names" );
    UIWindow & tCreditsNames = UI_GetWindowFromID( idCreditNames );
    const UIControlItemArray & tUIIDArray = tCreditsNames.GetChildList();
    for ( int32 i = (int32)tUIIDArray.size() - 1; i >= 0; i-- )
    {
        UIID idChild = tUIIDArray[i].id;
        UI_DeleteControl( idChild, TRUE );
    }


    if ( !bSkipped )
    {

        Console::ShowConsoleCreditsSound();
    }
    else
    {


        Console::OnConsoleCreditsComplete();
    }


















}


void Hide()
{

    if ( UI_TestID( idCurCreditsWindow ) && UI_GetWindowFromID( idCurCreditsWindow ).IsActive() )
    {







        UIID idCreditsFadeOut = UIIDCreateFromParent( idCurCreditsWindow, ".OnUnload" );
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID( idCreditsFadeOut );
        if ( !tStoryboard.IsPlaying() )
        {
            tStoryboard.Play( FALSE, 0.0f );
        }
    }
}









void VanillaCredits()
{

    if ( UI_TestID( idCurCreditsWindow ) )
    {
        UI_GetWindowFromID( idCurCreditsWindow ).SetActive( FALSE );
    }
    idCurCreditsWindow = UIWND_CREDITS;
    UI_GetWindowFromID( idCurCreditsWindow ).SetActive( TRUE );

    sReset();

    snoCurCredits = GlobalSNOGet( GLOBALSNO_UI_CREDITS );
    snoCurCreditsStringlist = GlobalSNOGet( GLOBALSNO_STRINGLIST_CREDITS );

    Show();

}


void ExpansionCredits()
{

    if ( UI_TestID( idCurCreditsWindow ) )
    {
        UI_GetWindowFromID( idCurCreditsWindow ).SetActive( FALSE );
    }
    idCurCreditsWindow = UIWND_CREDITS_X1;
    UI_GetWindowFromID( idCurCreditsWindow ).SetActive( TRUE );

    sReset();

    snoCurCredits = GlobalSNOGet( GLOBALSNO_UI_CREDITS_X1 );



    snoCurCreditsStringlist = GlobalSNOGet( GLOBALSNO_STRINGLIST_CREDITS_X1_CONSOLE );


    Show();

}


void Credits2017()
{

    if ( UI_TestID( idCurCreditsWindow ) )
    {
        UI_GetWindowFromID( idCurCreditsWindow ).SetActive( FALSE );
    }
    idCurCreditsWindow = UIWND_CREDITS_2017;
    UI_GetWindowFromID( idCurCreditsWindow ).SetActive( TRUE );

    sReset();

    snoCurCredits = GlobalSNOGet( GLOBALSNO_UI_CREDITS_2017 );



    snoCurCreditsStringlist = GlobalSNOGet( GLOBALSNO_STRINGLIST_CREDITS_2017_CONSOLE );


    Show();

}


void CreditsEternalCollection()
{
    if ( UI_TestID( idCurCreditsWindow ) )
    {
        UI_GetWindowFromID( idCurCreditsWindow ).SetActive( FALSE );
    }
    idCurCreditsWindow = UIWND_CREDITS_ETERNAL;
    UI_GetWindowFromID( idCurCreditsWindow ).SetActive( TRUE );

    sReset();

    snoCurCredits = GlobalSNOGet( GLOBALSNO_UI_CREDITS_ETERNAL );
    snoCurCreditsStringlist = GlobalSNOGet( GLOBALSNO_STRINGLIST_CREDITS_ETERNAL_CONSOLE );

    Show();

}





















































void OnUpdate( const UIID & )
{

    UIWindow & tParent = UI_GetWindowFromID( idCurCreditsWindow );
    if ( !tParent.IsActive() )
    {
        return;
    }



    const InputState & tInputState = InputGetCurrentState( INPUTCONTEXT_DEFAULT );
    int32 nTriggerPos = tInputState.tControllerState[UI_GetPrimaryUIController()].tCurrentFrame.nRightTrigger;
    int32 nSpeed = nScrollSpeed;
    if ( nScrollSpeed == 0 )
    {


        if ( nTriggerPos > 0 && nLastTriggerPos == 0 )
        {
            nSpeed = 1;
        }
    }


    else if ( nTriggerPos == 0 )
    {
        nSpeed = 1;
    }
    else
    {


        // Inference: 127 is a bare literal in the original (cmp #0x7f); no named trigger threshold found.
        nSpeed = ( nTriggerPos < 127 ) ? 2 : 3;
    }

    nLastTriggerPos = nTriggerPos;
    sSetScrollSpeed( nSpeed );


    UIID idCreditsBody = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body" );
    UIWindow & tBody = UI_GetWindowFromID( idCreditsBody );
    tBody.SetActive( TRUE );

    Vector2D vecTLParent, vecDimParent, vecTLBody, vecDimBody;
    tParent.GetRelativePosition( vecTLParent, vecDimParent );
    tBody.GetRelativePosition( vecTLBody, vecDimBody );

    uint32 dwTime = CGameGetRealTicks();
    vecTLBody.Set( vecTLBody.x, vecDimParent.y - fBaseOffset - ( dwTime - dwBaseTick ) * arScrollSpeeds[nScrollSpeed] );
    tBody.SetRelativePosition( vecTLBody, vecDimBody );


    sUpdateCreditsList();


    if ( vecTLBody.y + vecDimBody.y < vecTLParent.y - arScrollSpeeds[nScrollSpeed] * DELAY_AT_END )
    {
        Hide();
    }
}


void sSetScrollSpeed( int32 nNewSpeed )
{

    if ( nNewSpeed == nScrollSpeed )
    {
        return;
    }

    nScrollSpeed = nNewSpeed;

    UIWindow & tParent = UI_GetWindowFromID( idCurCreditsWindow );
    UIID idCreditsBody = UIIDCreateFromParent( idCurCreditsWindow, ".credits_body" );
    UIWindow & tBody = UI_GetWindowFromID( idCreditsBody );

    Vector2D vecTLParent, vecTLBody, vecDimParent, vecDimBody;
    tParent.GetRelativePosition( vecTLParent, vecDimParent );
    tBody.GetRelativePosition( vecTLBody, vecDimBody );

    fBaseOffset = vecDimParent.y - vecTLBody.y;
    dwBaseTick = CGameGetRealTicks();
}


void Pause()
{

    sSetScrollSpeed( nScrollSpeed == 0 ? 1 : 0 );
}



void Faster()
{
    if ( nScrollSpeed + 1 < NUM_SCROLL_SPEEDS )
    {

        sSetScrollSpeed( nScrollSpeed + 1 );
    }
}


void Slower()
{
    if ( nScrollSpeed > 1 )
    {
        sSetScrollSpeed( nScrollSpeed - 1 );
    }
}



void Console::Skip( const UIID &, const int32 )
{
    if ( !bSkipped )
    {
        bSkipped = TRUE;
        Hide();
    }
}



void Console::Pause( const UIID &, const int32 )
{
    UICredits::Pause();
}











void Console::Show( const int32 nControllerIndex )
{

    nLastTriggerPos = 0;
    bSkipped = FALSE;
    bStartedSoundCreditsDisplay = FALSE;
    UICredits::Show();
    UI_SetPrimaryUIController( nControllerIndex );
}





void Console::ShowConsoleCreditsSound()
{
    if ( UI_TestID( UIWND_CREDITS ) )
    {
        UI_GetWindowFromID( UIWND_CREDITS ).SetActive( FALSE );
    }
    if ( UI_TestID( UIWND_CONSOLE_CREDITS_SOUND ) )
    {
        UI_GetWindowFromID( UIWND_CONSOLE_CREDITS_SOUND ).SetActive( TRUE );
    }
    UI_GetStoryboardFromID( UISTORYBOARD_CONSOLE_CREDITS_SOUND_FADE_IN ).Play( FALSE, 0.0f );
}


void Console::OnConsoleCreditsSoundShowFinished( const UIID & )
{
    msConsoleSoundCreditsStartTime = Blizzard::Time::Milliseconds();
    bStartedSoundCreditsDisplay = TRUE;
}


void Console::OnConsoleCreditsSoundUpdate( const UIID & )
{
    UIWindow & tWindow = UI_GetWindowFromID( UIWND_CONSOLE_CREDITS_SOUND );


    if ( tWindow.IsActive() && bStartedSoundCreditsDisplay )
    {
        const Blizzard::Time::Millisecond msNow = Blizzard::Time::Milliseconds();
        if ( Blizzard::Time::GetTimeElapsed( msConsoleSoundCreditsStartTime, msNow ) >= CONSOLE_SOUND_CREDITS_DURATION )
        {
            UI_GetStoryboardFromID( UISTORYBOARD_CONSOLE_CREDITS_SOUND_FADE_OUT ).Play( FALSE, 0.0f );
            bStartedSoundCreditsDisplay = FALSE;
        }
    }
}


void Console::OnConsoleCreditsSoundHideFinished( const UIID & )
{
    if ( GameIsStartUp() )
    {
        UI_GetWindowFromID( UIWND_CONSOLE_CREDITS_SOUND ).SetActive( FALSE );
    }
    OnConsoleCreditsComplete();
}




void Console::OnConsoleCreditsComplete()
{
    CWorldDisableAreaMusic( FALSE );
    SoundManager::StopMusic();
    if ( GameIsStartUp() )
    {
        SoundManager::SuppressSounds( 0 );
        if ( ::Console::GamerProfile::GetPrimaryProfileUserIndex() != -1 )
        {
            ::Console::Screens::OpenToScreen( (::Console::Screens::ScreenSets)20, (::Console::Screens::ScreenID)1, ::Console::GamerProfile::GetPrimaryProfileUserIndex(), FALSE );
        }
        else
        {
            ::Console::Screens::OpenToScreen( (::Console::Screens::ScreenSets)20, (::Console::Screens::ScreenID)0, -1, FALSE );
        }
    }
    else
    {
        GameStartMenu( ACT_5 );
    }
}

}  // namespace UICredits
