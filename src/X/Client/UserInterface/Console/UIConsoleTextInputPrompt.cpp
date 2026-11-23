// X/Client/UserInterface/Console/UIConsoleTextInputPrompt.cpp -- the console
// text-entry prompt.  `Show` plays the confirmation sound and hands the platform
// software keyboard an `OnlineService::Callback<void (int, CRefString)>` built
// from `sOnKeyboardFinished`, which is why this unit also owns the
// `call_callback<...>` thunk and the four-byte `gen_destructor<...>::Del` that
// the callback node points at.

#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/Callback/Callback.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

#include "../Interface/UITextInputPrompt.h"

// The original reaches these through an include chain this file does not reproduce; they are
// here for the globals they add to the static initializer (SNONAME_NONE, cgplaceNull, blz::ignore,
// the GfxVB normals and the nn::ldn addresses).
#include "../../../Common/WorldPlace.h"
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
// X/Client/Console/Utilities.h:26 and UtilitiesNX64.cpp:43
namespace Console {
namespace Utilities {
typedef OnlineService::Callback<void (int, CRefString)> StringInputCallback;
void ShowKeyboardForUser(int32 nUserIndex, LPCSTR szDefaultText, LPCSTR szDialogTitle,
                         LPCSTR szDialogMessage, uint32 dwMaxTextLength, StringInputCallback cb);
void CloseKeyboard();  // UtilitiesNX64.cpp:91
}  // namespace Utilities
}  // namespace Console

// UIConsoleTextInputPrompt.cpp:20 -- what the keyboard calls back into. It has
// external linkage: the callback thunks carry the bound function in their own
// mangled name, and 2.6.2 spells it `...Z19sOnKeyboardFinished...` (an external
// name). No standalone body survives; it is inlined into the `call_callback`
// thunk below.
void sOnKeyboardFinished(UITextInputPrompt::DialogCallback pfnCallback, int32 nControllerIndex,
                         BOOL bSuccessful, CRefString sInput)
{
    if (pfnCallback != NULL)
    {
        Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        pfnCallback(bSuccessful, sInput, ptPlayer);
    }
}

// UIConsoleTextInputPrompt.cpp:35 -- open the software keyboard for one
// controller. `szMessage` and `bCloseOnMovement` are dead: the platform dialog
// gets the shared empty-string literal for its message and has no movement rule.
void UITextInputPrompt::Show(LPCTSTR szTitle, LPCSTR szMessage, LPCSTR szDefaultText,
                             DialogCallback pfnCallback, BOOL bCloseOnMovement,
                             uint32 dwMaxAllowedCharacters, int32 nControllerIndex)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONFIRMATION_OPEN), cgtPSODefaults);

    Console::Utilities::ShowKeyboardForUser(
        nControllerIndex,
        szDefaultText,
        szTitle,
        "",
        dwMaxAllowedCharacters,
        OnlineService::internal::function_binder_4<void, UITextInputPrompt::DialogCallback, int,
                                                   int, CRefString>::bind<&sOnKeyboardFinished>(
            pfnCallback, nControllerIndex));
}

// UIConsoleTextInputPrompt.cpp:60 -- take the keyboard back down. The platform
// keyboard closes itself, so this is a tail call to the (empty on NX64)
// `Console::Utilities::CloseKeyboard`.
void UITextInputPrompt::Close()
{
    Console::Utilities::CloseKeyboard();
}
