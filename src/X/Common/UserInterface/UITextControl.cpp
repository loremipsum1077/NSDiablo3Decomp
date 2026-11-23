// X/Common/UserInterface/UITextControl.cpp -- the shared (non-client) half of
// the text control: the four `Set_Text_Color` overloads that wrap a string in
// the `{c:rrggbb}` / `{/c}` markup the text renderer reads a colour out of.
//
// The two `RGBAColor` overloads carry the real body; the two `FontSpecifier`
// ones look the colour up in the authored font table and forward.  Both bodies
// keep any trailing newlines *outside* the markup -- the injection point walks
// back over them so `"Foo\n\n"` becomes `"{c:...}Foo{/c}\n\n"` and the line
// breaks survive the colour run.

#include "UITextControl.h"

#include <string.h>

#include "../../Sigma/RGBAColor.h"

// Declarations of the functions this file calls whose header is not adopted yet;
// the DWARF gives their signatures (file noted on each) but not where they were
// declared.
RGBAColor FontGetColorForFontSpecifier(FontSpecifier eFS, BOOL fIgnorePlayerOverrides);  // X/Common/UserInterface/Font.cpp
CRefString FontGetColorString(const RGBAColor & rgbaColor);  // X/Common/UserInterface/Font.cpp

void Set_Text_Color(LPCSTR lpszString, FontSpecifier efcDisplayColor)
{
    Set_Text_Color(lpszString, FontGetColorForFontSpecifier(efcDisplayColor, FALSE));
}

void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor)
{
    Set_Text_Color(usString, FontGetColorForFontSpecifier(efcDisplayColor, FALSE));
}

void Set_Text_Color(LPCSTR lpszString, const RGBAColor & rgbaColor)
{
    int32 nLen = strlen(lpszString);
    if (nLen == 0)
        return;

    int32 nInjectionPt = nLen;
    while (lpszString[nInjectionPt - 1] == '\n')
    {
        if (nInjectionPt < 2)
            return;
        nInjectionPt--;
    }

    CRefString sBuffer = FontGetColorString(rgbaColor);
    sBuffer += CRefString(lpszString, nInjectionPt);
    sBuffer += "{/c}";
    sBuffer += &lpszString[nInjectionPt];
    strcpy((char *)lpszString, sBuffer);
}

void Set_Text_Color(CRefString & usString, const RGBAColor & rgbaColor)
{
    int32 nLen = usString.Length();
    if (nLen == 0)
        return;

    LPCSTR lpszString = usString.str();
    int32 nInjectionPt = nLen;
    while (lpszString[nInjectionPt - 1] == '\n')
    {
        if (nInjectionPt < 2)
            return;
        nInjectionPt--;
    }

    CRefString sBuffer = FontGetColorString(rgbaColor);
    sBuffer += CRefString(lpszString, nInjectionPt);
    sBuffer += "{/c}";
    sBuffer += &lpszString[nInjectionPt];
    usString = sBuffer;
}
