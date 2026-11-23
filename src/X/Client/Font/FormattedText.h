// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Font/FormattedText.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Common/ImageHandle.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/RGBAColor.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include "../Gfx/GfxCommon.h"

class UTBreakIterator;
struct FormattedTextDelta;
class FormattedText;
struct TextRenderProperties;
struct Rect2D;
struct FontDefinition;
struct ShaderDefinition;
class FontChar;
struct FontPointSizeData;

typedef int32 SNO;

typedef int32 ImageCollectionType;

typedef int32 BreakStyle;

enum {
    TEXTDELTA_FONTCOLOR_PUSH = 0,
    TEXTDELTA_FONTCOLOR_POP = 1,
    TEXTDELTA_FONTFACE_PUSH = 2,
    TEXTDELTA_FONTFACE_POP = 3,
    TEXTDELTA_HYPERLINK = 4,
    TEXTDELTA_FONTICON = 5,
    TEXTDELTA_CHARACTER = 6,
    TEXTDELTA_BULLET_INDENT = 7,
    TEXTDELTA_BREAK_HINT = 8,
    TEXTDELTA_HYPHEN_HINT = 9,
};

typedef int32 FormattedTextDeltaType;

enum {
    PARSE_NONE = 0,
    PARSE_SPECIAL_TAGS = 1,
    PARSE_HYPERLINKS = 2,
    PARSE_DO_VALIDATION = 4,
    PARSE_ALL = 3,
    PARSE_DEFAULT = 5,
};

extern const uint32 cguPasswordCharacter;  // FormattedText.cpp:77
extern const uint32 cguInvalidGlyphCharacter;  // FormattedText.cpp:78
extern const uint32 cNBSP;  // FormattedText.cpp:79
extern const uint32 cEllipsisChar;  // FormattedText.cpp:80

// Font.cpp statics named as friends below (defined at Font.cpp:238 and 1037).
static void sFontStringDraw(const TextRenderProperties & tProps, const FormattedText & tFormattedText, IVector2D & vecSPBaseline, IVector2D & vecSubPixelPos, Rect2D * prectClipPixels, const FontDefinition *& pdefFont, const ShaderDefinition * pdefShader, const ShaderDefinition * pdefOutlineShader, const uint8 bAlpha, const RGBAColor * prgbaColorDropShadow, const BOOL fAllowColor, const BOOL bUseDropShadow, const float flZ, const RenderLayer eRenderLayerOverride, BOOL & fFirstChar, const FontChar *& ptCharPrev, BOOL fInRenderDraw, FontPointSizeData * ptPointSize, BOOL fIsPassword);
static BOOL sFontStringGetRenderedSize(const TextRenderProperties & tProps, const FormattedText & tFormattedText, const FontDefinition *& pdefFont, IVector2D & vecSPBaseline, IVector2D & vecSubPixelPos, IVector2D & vecSubPixelSelectionStart, IVector2D & vecSubPixelSelectionEnd, int32 & nSubPixelWidth, int32 & nSubPixelMinY, int32 & nSubPixelMaxY, int32 & nSPFirstLineBearing, BOOL & fFirstChar, BOOL & fOnFirstLine, const FontChar *& ptCharPrev, int32 & nFormattedIndex, const int32 nSelectionBegin, const int32 nSelectionEnd, IVector2D & tSize, IVector2D & vecSelectionBegin, IVector2D & vecSelectionEnd, FontPointSizeData * ptPointSize, BOOL fIsPassword, BOOL fFinalizeSelectionProcess);

class FormattedText {  // size 0x58
    // UIText and UIText_RemoveFormattingCharacters (UITextControl.cpp) read m_arTextDeltas and
    // m_szOriginalText directly (no accessor is inlined into their functions).
    friend class UIText;
    friend class UIEdit;
    friend uint32 UIText_RemoveFormattingCharacters(LPSTR puszString, BOOL fPreserveHyperlinks, BOOL fValidateHyperlinks);
    // Inference: Font.cpp's sFontStringDraw (Font.cpp:238), sFontStringGetRenderedSize (1037) and
    // sFontStringGetPosWithinLine (1683) bind a const reference to m_arTextDeltas (DWARF local
    // tStringDeltas) with no accessor inlined. The DWARF records the member as private and lists no
    // DW_TAG_friend for any class, so these friend declarations are inferred; the two static ones
    // need the static declarations above the class.
    friend void sFontStringDraw(const TextRenderProperties & tProps, const FormattedText & tFormattedText, IVector2D & vecSPBaseline, IVector2D & vecSubPixelPos, Rect2D * prectClipPixels, const FontDefinition *& pdefFont, const ShaderDefinition * pdefShader, const ShaderDefinition * pdefOutlineShader, const uint8 bAlpha, const RGBAColor * prgbaColorDropShadow, const BOOL fAllowColor, const BOOL bUseDropShadow, const float flZ, const RenderLayer eRenderLayerOverride, BOOL & fFirstChar, const FontChar *& ptCharPrev, BOOL fInRenderDraw, FontPointSizeData * ptPointSize, BOOL fIsPassword);
    friend BOOL sFontStringGetRenderedSize(const TextRenderProperties & tProps, const FormattedText & tFormattedText, const FontDefinition *& pdefFont, IVector2D & vecSPBaseline, IVector2D & vecSubPixelPos, IVector2D & vecSubPixelSelectionStart, IVector2D & vecSubPixelSelectionEnd, int32 & nSubPixelWidth, int32 & nSubPixelMinY, int32 & nSubPixelMaxY, int32 & nSPFirstLineBearing, BOOL & fFirstChar, BOOL & fOnFirstLine, const FontChar *& ptCharPrev, int32 & nFormattedIndex, const int32 nSelectionBegin, const int32 nSelectionEnd, IVector2D & tSize, IVector2D & vecSelectionBegin, IVector2D & vecSelectionEnd, FontPointSizeData * ptPointSize, BOOL fIsPassword, BOOL fFinalizeSelectionProcess);
    friend BOOL sFontStringGetPosWithinLine(const TextRenderProperties & tProps, const FormattedText & tFormattedText, const FontDefinition *& pdefFont, IVector2D & vecSPBaseline, IVector2D & vecSubPixelPos, BOOL & fFirstChar, const FontChar *& ptCharPrev, int32 & nFormattedIndex, const Vector2D & vecMousePixels, FontPointSizeData * ptPointSize);
public:
    FormattedText();
    FormattedText(CRefString sText, const uint32 dwParseFlags);
    FormattedText(LPCSTR szText, const uint32 dwParseFlags, BOOL fDeepCopy);
    FormattedText(LPCSTR szText, uint32 nLength, const uint32 dwParseFlags, BOOL fDeepCopy);
    FormattedText(const FormattedText & tFormattedText);
    FormattedText & operator=(const FormattedText & tFormattedText);
    FormattedText(FormattedText && tFormattedText);
    FormattedText & operator=(FormattedText && tFormattedText);
    ~FormattedText();
    FormattedText(const FormattedText & tFormattedText, uint32 nLength);
    typedef blz::vector<FormattedTextDelta *, blz::allocator<FormattedTextDelta *> > FormattedTextDeltaArray;
    static void ConvertToDeepCopy(FormattedText & tText);
    LPCSTR GetText() const;
    uint32 Length() const;
    uint32 ToFormattedIndex(uint32 nOriginalIndex) const;
    uint32 ToOriginalIndex(uint32 nFormattedIndex) const;
    // line 108
    BOOL IsPassword() const { return m_fPassword; }
    // line 114
    void SetIsPassword(BOOL fIsPassword) { m_fPassword = fIsPassword; }
    uint32 GetNextFormattedChar(uint32 nFormattedIndex) const;
    uint32 GetPrevFormattedChar(uint32 nFormattedIndex) const;
    uint32 GetNextFormattedWord(uint32 nFormattedIndex) const;
    uint32 GetPrevFormattedWord(uint32 nFormattedIndex) const;
    BOOL IndexIsInsideTextDelta(int32 nCodepointIndex, uint32 dwDeltaSearchMask) const;
    BOOL FindNearestLineBreak(UTBreakIterator & tIterator, LPCSTR szStrippedText, int32 & nLineBreakOriginalIndex, const int32 nOriginalIndexMin, const int32 nOriginalIndexStart, int32 & nBreakHintIndexUsed) const;
    int32 ConvertIdxToStrippedIdx(int32 nIndex, uint32 uIgnoreFlags) const;
    int32 ConvertIdxFromStrippedIdx(int32 nIndex, uint32 uIgnoreFlags) const;
    BOOL IsValidOriginalTextPosition(int32 nOriginalTextIndex);
    void ClearDeltas(FormattedTextDeltaType eDeltaType);
    void Truncate(uint32 nIndex, LPCSTR szReplacementString);
    FormattedTextDelta * AddCharacterDelta(uint32 nIndex, uint32 uChar, BOOL fReplaceCharAtIndex);
    FormattedTextDelta * AddBulletIndentDelta(uint32 nIndex, uint32 nSpaces);
    int32 GetDeltaInsertionIndex(FormattedTextDelta * pDelta) const;
    void InsertDelta(FormattedTextDelta * pDelta, const int32 nInsertionPoint);
    // line 189
    void InsertDelta(FormattedTextDelta * pDelta)
    {
        InsertDelta(pDelta, GetDeltaInsertionIndex(pDelta));
    }
    void SplitIntoLines(blz::vector<FormattedText, blz::allocator<FormattedText> > & arFormattedTexts) const;
    // line 201
    operator const char *() const { return m_szOriginalText; }
    // line 206. The 2.6.2 inline stack pins the whole expression to one line (208): the
    // parsed copy when it is non-empty, otherwise the raw authored pointer.
    operator CRefString() const
    {
        return m_sOriginalText.Length() ? m_sOriginalText : CRefString(m_szOriginalText);
    }
    uint32 GetPrevBreak(const uint32 nFormattedIndex, const BreakStyle eBreakStyle) const;
    uint32 GetNextBreak(const uint32 nFormattedIndex, const BreakStyle eBreakStyle) const;
    // line 218
    uint64 GetHash() const { return m_dwHash; }
    // line 223
    BOOL GetContainsIcons() const { return m_fContainsIcons; }
private:
    int32 sAddSegment(blz::vector<FormattedText, blz::allocator<FormattedText> > & arFormattedTexts, LPCSTR uszSplitStart, const uint32 nSplitLength, const int32 nCurrentDeltaIndex, const BOOL fCopyCurrentDelta, const int32 nLastSplitDeltaIndex, const int32 nLastSplitOriginalIndex, const int32 nLastSplitCodepointIndex, const int32 nFormattedLength, const BOOL fDeepCopy) const;
    void Cleanup();
    void ClearDeltas();
    BOOL ContainsBreakHints() const;
    BOOL ContainsIcons() const;
    int32 FindPrecedingBreakHintIndex(int32 nCodepointIndex, BOOL bIgnoreNewLines) const;
    void ComputeHash();
    LPCSTR m_szOriginalText;  // 0x00
    CRefString m_sOriginalText;  // 0x08
    uint32 m_nOriginalLength;  // 0x20
    FormattedText::FormattedTextDeltaArray m_arTextDeltas;  // 0x28
    uint64 m_dwHash;  // 0x40
    BOOL m_fPassword;  // 0x48
    BOOL m_fContainsBreakHints;  // 0x4c
    BOOL m_fContainsIcons;  // 0x50
};
static_assert(sizeof(FormattedText) == 0x58, "FormattedText");

struct FormattedTextDelta {  // size 0x1c
    // line 274
    BOOL IsBreakHint() const { return eType == TEXTDELTA_BREAK_HINT || eType == TEXTDELTA_HYPHEN_HINT; }
    FormattedTextDeltaType eType;  // 0x00
    uint32 nOriginalTextIndex;  // 0x04
    uint32 nFormattedTextIndex;  // 0x08
    uint32 nOriginalBytesReplaced;  // 0x0c
    uint32 nFormattedTextLength;  // 0x10
    uint32 nCodepointIndex;  // 0x14
    uint32 nCodepointsReplaced;  // 0x18
};
static_assert(sizeof(FormattedTextDelta) == 0x1c, "FormattedTextDelta");

struct FormattedCharacterDelta : public FormattedTextDelta {  // size 0x20
    uint32 uChar;  // 0x1c
};
static_assert(sizeof(FormattedCharacterDelta) == 0x20, "FormattedCharacterDelta");

// line 294
struct FormattedHyperlinkDelta : public FormattedTextDelta {  // size 0x78
    FormattedText tFormattedText;  // 0x20
};
static_assert(sizeof(FormattedHyperlinkDelta) == 0x78, "FormattedHyperlinkDelta");

// line 299
struct FormattedColorPushDelta : public FormattedTextDelta {  // size 0x20
    RGBAColor rgbaColor;  // 0x1c
};
static_assert(sizeof(FormattedColorPushDelta) == 0x20, "FormattedColorPushDelta");

struct FormattedFontFacePushDelta : public FormattedTextDelta {  // size 0x20
    SNO snoFont;  // 0x1c
};
static_assert(sizeof(FormattedFontFacePushDelta) == 0x20, "FormattedFontFacePushDelta");

struct FormattedIconDelta : public FormattedTextDelta {  // size 0x28
    ImageHandle hImage;  // 0x1c
    ImageCollectionType eCollection;  // 0x20
    float flScale;  // 0x24
};
static_assert(sizeof(FormattedIconDelta) == 0x28, "FormattedIconDelta");

struct FormattedBulletIndentDelta : public FormattedTextDelta {  // size 0x20
    int32 nSpaces;  // 0x1c
};
static_assert(sizeof(FormattedBulletIndentDelta) == 0x20, "FormattedBulletIndentDelta");
