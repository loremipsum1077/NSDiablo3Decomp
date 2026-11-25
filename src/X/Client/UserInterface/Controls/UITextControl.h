// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UITextControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Font/FormattedText.h"
#include "UIWindowControl.h"
#include "../../../Common/Hyperlink.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/DataStructures/XOptional.h"
#include "../../../../NintendoSDK/Include/nn/oe/oe_Performance-spec.nx.h"

struct BufferedInput;
struct BufferedMouseData;
class FontColorStack;
struct FontPointSizeData;
struct UIControlDataExt;
struct Rect2D;

typedef int32 FontSpecifier;  // X/Common/UserInterface/FontDefinition.h
RGBAColor FontGetColorForFontSpecifier(FontSpecifier eFS, BOOL fIgnorePlayerOverrides);  // X/Common/UserInterface/Font.cpp

extern int32 gnFontBoost;  // UITextControl.cpp:58
extern nn::oe::PerformanceMode gePerformanceMode;  // X/Common/Shell/ShellNX64.cpp:28

enum {
    MAX_UI_TEXT_LEN = 8192,
};

enum {
    UIJH_NONE = 0,
    UIJH_LEFT = 1,
    UIJH_CENTER = 2,
    UIJH_RIGHT = 3,
};

enum {
    UIJV_NONE = 0,
    UIJV_TOP = 1,
    UIJV_MIDDLE = 2,
    UIJV_BOTTOM = 3,
    UIJV_LINE_MIDDLE = 4,
    UIJV_LINE_TOP = 5,
    UIJV_ASCENDER_MIDDLE = 6,
};

typedef int32 UIJustifyVertical;

enum UITextOffset {
    UITEXT_OFFSET_HORIZONTAL = 0,
    UITEXT_OFFSET_VERTICAL = 1,
};

enum UITextControlFlags {
    UITCF_ENABLED_BIT = 0,
    UITCF_WORDWRAP_BIT = 1,
    UITCF_ISPASSWORD_BIT = 2,
    UITCF_USE_BUTTON_PRESS_OFFSET_BIT = 3,
    UITCF_HYPERLINKAWARE_BIT = 4,
    UITCF_SELECTABLE_BIT = 5,
    UITCF_SELECTS_RAW_TEXT_BIT = 6,
    UITCF_USE_DROP_SHADOW_BIT = 7,
    UITCF_ALL_CAPS_BIT = 8,
    UITCF_SHRINK_TO_FIT_BIT = 9,
    UITCF_RESIZE_TO_FIT_HORIZONTAL_BIT = 10,
    UITCF_RESIZE_TO_FIT_VERTICAL_BIT = 11,
    UITCF_DISABLE_SNAP_TO_PIXEL_BIT = 12,
    UITCF_VALIDATE_HYPERLINKS_BIT = 13,
    UITCF_DISABLE_TRUNCATION_TOOLTIP_BIT = 14,
};

enum {
    RESIZETOFIT_DISABLED = 0,
    RESIZETOFIT_ENABLED = 1,
    RESIZETOFIT_HORIZONTAL_ONLY = 2,
    RESIZETOFIT_VERTICAL_ONLY = 3,
};

typedef int32 ResizeToFitValue;

// line 98
struct UITextHyperlinkRecord {  // size 0x20
    CRefString sRawHyperlinkText;  // 0x00
    uint32 uHyperlinkStartChar;  // 0x18
    uint32 uHyperlinkCharacterLength;  // 0x1c
};
static_assert(sizeof(UITextHyperlinkRecord) == 0x20, "UITextHyperlinkRecord");

class TextCreationParams : public WindowCreationParams {  // size 0x7d8
public:
    TextCreationParams(const UIType, const UIID &, const UIID &);
    TextCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIJustifyHorizontal eJustifyHorizontal;  // 0x730
    UIJustifyVertical eJustifyVertical;  // 0x734
    float flAdvanceScalar;  // 0x738
    BOOL fAllCaps;  // 0x73c
    RGBAColor rgbaText;  // 0x740
    RGBAColor rgbaTextDisabled;  // 0x744
    RGBAColor rgbaDropShadow;  // 0x748
    RGBAColor rgbaDropShadowDisabled;  // 0x74c
    SNO snoFont;  // 0x750
    int32 nFontSize;  // 0x754
    int32 nFontSizeFallback;  // 0x758
    BOOL fWordWrap;  // 0x75c
    BOOL fShrinkToFit;  // 0x760
    ResizeToFitValue eResizeToFit;  // 0x764
    BOOL fHyperlinkAware;  // 0x768
    BOOL fValidateHyperlinks;  // 0x76c
    BOOL fIsPassword;  // 0x770
    BOOL fSelectable;  // 0x774
    BOOL fUseDropShadow;  // 0x778
    int32 nLinesBeforeTruncation;  // 0x77c
    Vector2D vecTextButtonPressOffsetUIC;  // 0x780
    CRefString sDecoratedLabel;  // 0x788
    Vector2D vecTextInsetUIC;  // 0x7a0
    Rect2D rectTextPaddingUIC;  // 0x7a8
    CRefString sPlaceholderTextLabel;  // 0x7b8
    RGBAColor rgbaPlaceholderText;  // 0x7d0
    RGBAColor rgbaPlaceholderTextDropShadow;  // 0x7d4
private:
    void sInit();
};
static_assert(sizeof(TextCreationParams) == 0x7d8, "TextCreationParams");

class UIText : public UIWindow {  // size 0x3e8
    // Invented: UIAutoScrollingText::SetHorizontalScrolling stores one justification member
    // directly; the DWARF does not record how it reaches these protected members.
    friend class UIAutoScrollingText;
public:
    UIText(const TextCreationParams &, const Vector2D &, const Vector2D &);
    UIText(const TextCreationParams &, const Rect2D *);
    virtual ~UIText();  // vtable slot 0
    const FormattedText & GetText() const;
    virtual void SetText(LPCSTR szNewText, const BOOL fDisallowUpcalls);  // vtable slot 34
    void ApplyFormatting();
    BOOL IsEnabled() const { return (m_uTCFlags >> UITCF_ENABLED_BIT) & 1; }  // line 191
    BOOL IsPassword() const;
    void SetIsPassword(BOOL);
    BOOL IsTruncated() const { return m_fTextIsTruncated; }  // line 211
    BOOL IsTruncationTooltipEnabled() const;
    void SetTruncationTooltipEnabled(BOOL);
    void SetSnapToPixelCenter(const BOOL bVal);
    virtual void SetEnabled(const BOOL bEnabled) {  // vtable slot 35; line 234
        m_uTCFlags = !bEnabled ? (m_uTCFlags & ~(1 << UITCF_ENABLED_BIT)) : (m_uTCFlags | (1 << UITCF_ENABLED_BIT));
    }
    void SetTextColor(const RGBAColor & rgbaColor) { m_rgbaText = rgbaColor; }  // line 241
    void SetTextColor(FontSpecifier eFontSpecifier) { m_rgbaText = FontGetColorForFontSpecifier(eFontSpecifier, FALSE); }  // line 248
    RGBAColor GetTextColor() const { return m_rgbaText; }  // line 255
    void SetDisabledColor(const RGBAColor &);
    SNO GetFont() const { return m_snoFont; }  // line 269
    int32 GetFontSize() const {  // line 276; the body is written from its inlined copies (lines 282-285)
        if (gePerformanceMode == nn::oe::PerformanceMode_Normal || unk_3bc)  // 2.7.7: unk_3bc also applies the boost
            return m_nFontSize + m_nFontBoostSize + gnFontBoost;
        return m_nFontSize + gnFontBoost;
    }
    float GetAdvanceScalar() const { return m_flAdvanceScalar; }  // line 297
    void SetFont(const SNO snoFont) { m_snoFont = snoFont; }  // line 304
    void SetFontSize(const int32 nFontSize) { m_nFontSize = nFontSize; }  // line 311
    void SetFontBoostSize(const int32 nFontBoostSize) {  // line 319; 2.7.7: every inlined copy also clears unk_3bc
        m_nFontBoostSize = nFontBoostSize;
        unk_3bc = 0;
    }
    void SetWordWrap(const BOOL bWrap);
    void SetValidatingHyperlinks(BOOL fFlag);
    void SetHyperlinkAware(BOOL);
    UIJustifyHorizontal GetHorizontalJustification();
    UIJustifyVertical GetVerticalJustification();
    void SetJustification(const UIJustifyHorizontal eTextJustifyHorizontal, const UIJustifyVertical eTextJustifyVertical) {  // line 359
        m_eTextJustify = eTextJustifyHorizontal;
        m_eTextJustifyVertical = eTextJustifyVertical;
    }
    int32 GetLineCount() const;
    int32 GetNumVisibleLines() const;
    RGBAColor GetTextDropShadowColor() const { return m_rgbaDropShadow; }  // line 376
    void SetDropShadowColor(const RGBAColor & rgbaColor) { m_rgbaDropShadow = rgbaColor; }  // line 383
    int32 GetTopLine() const { return m_nTopLine; }  // line 395
    void SetTopLine(const int32 nLine);
    void SetScrollOffsetUIC(const UITextOffset eOffset, const float flOffsetAmount);
    const FormattedText & GetLine(const int32 nLine) const;
    uint32 GetLineLength(const int32) const;
    uint32 GetHyperlinksInLine(const int32, blz::vector<UITextHyperlinkRecord, blz::allocator<UITextHyperlinkRecord> > &) const;
    virtual void GetTextRegionRectUIC(Rect2D & rectTextExtentsUIC) const;  // vtable slot 36
    void GetTextRenderedSizeUIC(Vector2D & vecTextSizeUIC, BOOL fGetTruncatedTextSize) const;
    void GetActualTextPosition(Vector2D &, Vector2D &) const;
    void ResizeToFitText();
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual LPCSTR GetTooltipText();  // vtable slot 33
    virtual UIID HitTest(const IVector2D & pt) {  // vtable slot 15; line 454
        if (!IsActive())
            return GetUIIDNone();
        XOptional<int> fIsInside;
        if (IsSelectable())
        {
            fIsInside = IsInside(pt);
            if (fIsInside)
                return GetID();
        }
        if (IsHyperlinkAware())
        {
            if (!fIsInside.Exists())
                fIsInside = IsInside(pt);
            if (fIsInside && sIsInHyperlink(pt, NULL, NULL))
                return GetID();
        }
        return GetUIIDNone();
    }
    virtual BOOL CanAcceptKeyboardFocus() const {  // vtable slot 2; line 489
        if (IsActive() && IsSelectable())
            return TRUE;
        return FALSE;
    }
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 502
        return eType == UIT_TEXT || UIWindow::IsType(eType);
    }
    virtual void SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 30
    BOOL IsSelectable() const { return (m_uTCFlags >> UITCF_SELECTABLE_BIT) & 1; }  // line 521
    void SetSelectable(const BOOL fVal) {  // line 528
        m_uTCFlags = !fVal ? (m_uTCFlags & ~(1 << UITCF_SELECTABLE_BIT)) : (m_uTCFlags | (1 << UITCF_SELECTABLE_BIT));
    }
    void ClearSelection();
    void SetSelection(const int32 nSelectionBegin, const int32 nSelectionEnd);
    void SelectAll();
    BOOL HasSelection() const;
    void GetSelection(int32 & nSelectionBegin, int32 & nSelectionEnd) const;
    void GetSelectedText(LPSTR lpuszRetrievedText) const;
    void SetTextInset(const Vector2D & vecTextInsetUIC) { m_vecTextInsetUIC = vecTextInsetUIC; }  // line 566
    void SetTextPadding(const Rect2D &);
    Rect2D GetTextPadding() const { return m_rectTextPaddingUIC; }  // line 582
    void SetResizeToFit(const ResizeToFitValue eVal) {  // line 589
        m_uTCFlags = !(eVal == RESIZETOFIT_ENABLED || eVal == RESIZETOFIT_HORIZONTAL_ONLY) ? (m_uTCFlags & ~(1 << UITCF_RESIZE_TO_FIT_HORIZONTAL_BIT)) : (m_uTCFlags | (1 << UITCF_RESIZE_TO_FIT_HORIZONTAL_BIT));
        m_uTCFlags = !(eVal == RESIZETOFIT_ENABLED || eVal == RESIZETOFIT_VERTICAL_ONLY) ? (m_uTCFlags & ~(1 << UITCF_RESIZE_TO_FIT_VERTICAL_BIT)) : (m_uTCFlags | (1 << UITCF_RESIZE_TO_FIT_VERTICAL_BIT));
    }
    ResizeToFitValue GetResizeToFit() const;
    void SetLinesBeforeTruncation(const int32);
    void SetColorStackForLine(FontColorStack & tColorStack, const RGBAColor & rgbaTextCurrent, const FormattedText & tText);
    void CopySelectionToClipboard();
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    void SetControllerIndexOverride(int32 nControllerIndex) { m_nControllerIndexOverride = nControllerIndex; }  // line 637
    void SetWordWrapScalar(float fWrapLineScalar) { m_wrapLineScalar = fWrapLineScalar; }  // line 643
    static const int32 DefaultSwitchTextBoost = 4;
    virtual void SetText_Internal(LPCSTR, const BOOL) {}  // vtable slot 37; line 653
    virtual Vector2D CalculateTextSize() const;  // vtable slot 38
    void ShrinkToFit();
    void SetSelectsRawText(const BOOL fSelectsRawText) {  // line 668
        m_uTCFlags = !fSelectsRawText ? (m_uTCFlags & ~(1 << UITCF_SELECTS_RAW_TEXT_BIT)) : (m_uTCFlags | (1 << UITCF_SELECTS_RAW_TEXT_BIT));
    }
    float GetScrollOffsetUIC(const UITextOffset eOffset) const;
    void SetUseDropShadow(const BOOL bUseDropShadow) {  // line 679
        m_uTCFlags = !bUseDropShadow ? (m_uTCFlags & ~(1 << UITCF_USE_DROP_SHADOW_BIT)) : (m_uTCFlags | (1 << UITCF_USE_DROP_SHADOW_BIT));
    }
    BOOL IsUsingDropShadow() const { return (m_uTCFlags >> UITCF_USE_DROP_SHADOW_BIT) & 1; }  // line 686
    void EnableTextButtonPressOffset(const BOOL fVal) {  // line 693
        m_uTCFlags = !fVal ? (m_uTCFlags & ~(1 << UITCF_USE_BUTTON_PRESS_OFFSET_BIT)) : (m_uTCFlags | (1 << UITCF_USE_BUTTON_PRESS_OFFSET_BIT));
    }
    BOOL IsHyperlinkAware() const { return (m_uTCFlags >> UITCF_HYPERLINKAWARE_BIT) & 1; }  // line 700
    BOOL IsValidatingHyperlinks() const { return IsHyperlinkAware() && ((m_uTCFlags >> UITCF_VALIDATE_HYPERLINKS_BIT) & 1); }  // line 708
    BOOL IsUsingWordWrap() const;
    BOOL ResizeToFitTextEnabled() const;
    uint32 GetTextParseFlags() const;
    int32 ScreenPtToCursorPos(const Vector2D & vecMouseUIC);
    void DrawText(const uint8 bLayerAlpha, float & flZSlice);
    void DrawSelection(const uint8 bLayerAlpha, float & flZSlice, const Rect2D & rectDrawClippingUIC, const Rect2D & rectTextExtentsUIC, Rect2D * pRectSelectionUIC);
    int32 IsInHyperlink(const int32 nPosition, const BOOL bStartInclusive, const BOOL bEndInclusive) const;
    int32 TextPosToOriginalTextPos(const int32 nPos) const;
    int32 OriginalTextPosToTextPos(const int32 nPos) const;
    void ConvertFormattedSelectionToOriginalTextSelection(int32 & nConvertedSelectionBegin, int32 & nConvertedSelectionEnd) const;
    BOOL HandleLeftMouseDown(const BufferedInput & tInput);
    FormattedText m_tText;  // 0x1d0
    blz::vector<FormattedText, blz::allocator<FormattedText> > m_artTextLines;  // 0x228
    FormattedText m_tTruncatedText;  // 0x240
    FormattedText m_tTruncatedFinalLine;  // 0x298
    int32 m_nSelectionBegin;  // 0x2f0
    int32 m_nSelectionEnd;  // 0x2f4
    BOOL m_bDragging;  // 0x2f8
    int32 m_nDragStartCursorPos;  // 0x2fc
    UIJustifyHorizontal m_eTextJustify;  // 0x300
    UIJustifyVertical m_eTextJustifyVertical;  // 0x304
    Vector2D m_vecTextButtonPressOffsetUIC;  // 0x308
    Vector2D m_vecTextInsetUIC;  // 0x310
    FormattedText m_tPlaceholderText;  // 0x318
    RGBAColor m_rgbaPlaceholderText;  // 0x370
    RGBAColor m_rgbaPlaceholderTextDropShadow;  // 0x374
    float sJustifyHorizontal(const Rect2D & rectTextExtentsUIC, const float flLineWidthUIC);
    float sJustifyVertical(FontPointSizeData * ptPointSize, const Rect2D & rectTextExtentsUIC, const FormattedText & tText);
    void sInit(const TextCreationParams & tParams);
    RGBAColor m_rgbaText;  // 0x378
    RGBAColor m_rgbaDropShadow;  // 0x37c
    RGBAColor m_rgbaTextDisabled;  // 0x380
    RGBAColor m_rgbaDropShadowDisabled;  // 0x384
    int32 m_nTopLine;  // 0x388
    float m_flTextScrollVerticalUIC;  // 0x38c
    float m_flTextScrollHorizontalUIC;  // 0x390
    CHAR * m_uszText;  // 0x398
    int32 m_nTextAllocLen;  // 0x3a0
    BOOL m_fTextIsTruncated;  // 0x3a4
    SNO m_snoFont;  // 0x3a8
    int32 m_nFontSizeOriginal;  // 0x3ac
    int32 m_nFontSize;  // 0x3b0
    int32 m_nFontSizeFallback;  // 0x3b4
    BOOL m_nFontBoostSize;  // 0x3b8
    uint8 unk_3bc;  // 0x3bc (2.7.7: sInit clears it with strb after m_nFontBoostSize)
    float m_flAdvanceScalar;  // 0x3c0
    float m_wrapLineScalar;  // 0x3c4
    uint32 m_uTCFlags;  // 0x3c8
    int32 m_nLinesBeforeTruncation;  // 0x3cc
    Rect2D m_rectTextPaddingUIC;  // 0x3d0
    int32 m_nControllerIndexOverride;  // 0x3e0
    int32 m_nControllerType;  // 0x3e4
    void sApplyFormattingForCurrentDimensions();
    void sTruncateIfNecessary();
    void sGetLineOfText(LPCSTR uszLineStart, const int32 nBulletIndentCount, int32 & nBytes, float & flCurrentLineWidthUIC);
    BOOL sGetHyperlink(const BufferedMouseData & tBufMouseData, UITextHyperlinkRecord & tRecord, HyperlinkType & eHyperlinkType);
    BOOL sIsInHyperlink(const IVector2D & vecMousePosition, UITextHyperlinkRecord * ptRecord, HyperlinkType * peHyperlinkType);
    BOOL sNeedsToRefreshIcons();
    void sRefreshIcons();
};
static_assert(sizeof(UIText) == 0x3e8, "UIText");

// Defined in UITextControl.cpp.
CRefString UIText_GetTimeString(const uint32 uSeconds);
uint32 UIText_RemoveFormattingCharacters(LPSTR puszString, BOOL fPreserveHyperlinks, BOOL fValidateHyperlinks);
void UIText_RemoveFormattingCharacters(CRefString & usString, BOOL fPreserveHyperlinks, BOOL fValidateHyperlinks);

// UITextControl.h:331. The body is not in the DWARF; written from the copies inlined into
// ConsoleModalNotificationManager::CreateModalNotification (clears bit 13 for FALSE), on the
// pattern of SetEnabled.
inline void UIText::SetValidatingHyperlinks(BOOL fFlag) {
    m_uTCFlags = !fFlag ? (m_uTCFlags & ~(1 << UITCF_VALIDATE_HYPERLINKS_BIT)) : (m_uTCFlags | (1 << UITCF_VALIDATE_HYPERLINKS_BIT));
}
