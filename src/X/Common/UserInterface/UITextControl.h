// X/Common/UserInterface/UITextControl.cpp's four free colour helpers -- the
// shared (non-client) half of the text control.  Each `Set_Text_Color` overload
// rewrites a string in place, prefixing it with the `{c:rrggbb}` markup the text
// renderer reads a colour out of and suffixing it with `{/c}`.
#pragma once

#include "FontDefinition.h"

#include "../../Sigma/RefString.h"

struct RGBAColor;

void Set_Text_Color(LPCSTR lpszString, FontSpecifier efcDisplayColor);
void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor);
void Set_Text_Color(LPCSTR lpszString, const RGBAColor & rgbaColor);
void Set_Text_Color(CRefString & usString, const RGBAColor & rgbaColor);
