// Original file: X/Client/UserInterface/Console/UIConsoleWhatsNewNX64.cpp
//
// The Switch-only "what's new" splash.  `UIWhatsNewNX64::Console` is a
// namespace, not a class: one namespace-scope `UIID` plus five free functions
// (one of them the file's static initialiser).  The 2.6.2 build counts the
// splash version in whole numbers (`PreferencesDefinition::nWhatsNewSeenNX64`
// is an `int`), so this is not a verbatim copy of the `2.6f` PC splash next
// door.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/DataStructures/XBitArray.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "ControllerActions.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../Gfx/GfxVB.h"

// Not used directly: the original reaches these through an include chain, and
// their header-level statics (blz::ignore, SNONAME_NONE, cgplaceNull, the
// GfxVB.h normals) are part of this file's _GLOBAL__sub_I.
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"

// Declared here because their headers are not adopted yet. Each comment names
// the file that defines the function or object.
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
int32 UI_GetPrimaryUIController();
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIWND_CONSOLE_WHATS_NEW_NX64;
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
void UpdateButtonLegendText();
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride);
void CloseModalDialog(const UIID & idDialog);
}  // namespace Screens
}  // namespace Console
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
typedef uint32 SoundID;
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/Console/GamerProfile.cpp
namespace Console {
namespace GamerProfile {
void SavePrefs();
}  // namespace GamerProfile
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleStartScreen.cpp
namespace UIStartScreen {
namespace Console {
void OnWhatsNewCompleteNX64(int32 nControllerIndex);
}  // namespace Console
}  // namespace UIStartScreen

namespace UIWhatsNewNX64 {
namespace Console {

// :29 -- the file's one global, external linkage and dynamically initialised
// (`UIID`'s copy constructor is `UIID.h:31`, i.e. one `SetInternal(o.nIDHash,
// NULL)`).  Only `_GLOBAL__sub_I_UIConsoleWhatsNewNX64.cpp` mentions it.
UIID idPreviousWindow = GetUIIDNone();

// :31 -- the splash version the Switch build counts in whole numbers.
static const int32 LATEST_WHATS_NEW_VERSION_NX64 = 1;

// :85 -- declared here because `OnButton2` (:67) is emitted before it and is
// its only caller; it is `static`, which is why no build carries an
// out-of-line symbol for it.
static void Close();

// :50 -- open the splash for one controller. Unlike the PC/console splash
// there are no pages to walk: the screen is one window with a single "close"
// legend.
void OpenForController( int nControllerIndex )
{
    ::Console::Screens::ShowModalDialog( nControllerIndex, UIWND_CONSOLE_WHATS_NEW_NX64, TRUE, GetUIIDNone() );

    ::Console::Screens::GetActionManager().ClearAllActions();
    ::Console::Screens::GetActionManager().SetAction( 7, 0, StringListFetch( "Console:Close" ).str() );

    ::Console::Screens::UpdateButtonLegendText();
}

// :61 -- is the splash on screen right now.
BOOL IsVisible()
{
    return UI_GetWindowFromID( UIWND_CONSOLE_WHATS_NEW_NX64 ).IsActive();
}

// :67 -- the B/circle handler: close, unless the action is still delayed.
void OnButton2( const UIID & id, const int32 nControllerIndex )
{
    if ( UI_GetPrimaryUIController() == nControllerIndex )
    {
        if ( !::Console::Screens::GetActionManager().IsActionDelayed( 7 ) )
        {
            SoundManager::PlaySound( GlobalSNOGet( GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE ), cgtPSODefaults );
            Close();
        }
    }
}

// :85 -- remember that the player has seen this splash and hand the start
// screen back its flow.
static void Close()
{
    if ( IsVisible() )
    {
        ::Console::Screens::CloseModalDialog( UIWND_CONSOLE_WHATS_NEW_NX64 );

        BOOL bSave = Prefs::GetWhatsNewSeenNX64() != LATEST_WHATS_NEW_VERSION_NX64;
        if ( bSave )
        {
            Prefs::GetWhatsNewSeenNX64() = LATEST_WHATS_NEW_VERSION_NX64;

            ::Console::GamerProfile::SavePrefs();
        }

        UIStartScreen::Console::OnWhatsNewCompleteNX64( UI_GetPrimaryUIController() );
    }
}

// :111 -- has the player already seen this splash.
BOOL ShouldShow()
{
    if ( CmdLineGetParams()->tBitArray.Get( CMDLINE_BLIZZCON_BIT ) )
    {
        return FALSE;
    }

    return Prefs::GetWhatsNewSeenNX64() < LATEST_WHATS_NEW_VERSION_NX64;
}

}  // namespace Console
}  // namespace UIWhatsNewNX64
