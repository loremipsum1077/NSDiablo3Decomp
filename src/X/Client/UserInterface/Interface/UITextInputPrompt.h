// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UITextInputPrompt.h

#pragma once

#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

class CRefString;
class Player;

namespace UITextInputPrompt {

typedef void (*DialogCallback)(BOOL, CRefString, Player *);

// The dialog result: whether the player confirmed, the text they typed, and the
// player who opened the keyboard (found again by controller index).
void Show(LPCTSTR szTitle, LPCSTR szMessage, LPCSTR szDefaultText, DialogCallback pfnCallback,
          BOOL bCloseOnMovement, uint32 dwMaxAllowedCharacters, int32 nControllerIndex);
void Close();

}  // namespace UITextInputPrompt
