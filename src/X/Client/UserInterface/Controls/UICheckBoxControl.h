// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UICheckBoxControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;
struct BufferedInput;
class UIText;

typedef int32 ResizeToFitValue;  // UITextControl.h

enum CheckBoxStyle {
    CS_STANDARD_BOX = 0,
    CS_TOGGLE_BUTTON = 1,
};

enum CheckBoxState {
    CS_CHECKED_NORMAL = 0,
    CS_CHECKED_MOUSEOVER = 1,
    CS_CHECKED_DISABLED = 2,
    CS_UNCHECKED_NORMAL = 3,
    CS_UNCHECKED_MOUSEOVER = 4,
    CS_UNCHECKED_DISABLED = 5,
    CS_MAX = 6,
};

class CheckBoxCreationParams : public WindowCreationParams {  // size 0x808
public:
    CheckBoxCreationParams(const UIType, const UIID &, const UIID &);
    CheckBoxCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIMsgFunctionPtr pfnClickPredicate;  // 0x730
    SNO snoClickSound;  // 0x738
    SNO snoMouseoverSound;  // 0x73c
    ImageTextureFrame tTextureFrames[6];  // 0x740
    RGBAColor rgbaTextColors[6];  // 0x770
    RGBAColor rgbaDropShadowColors[6];  // 0x788
    RGBAColor rgbaTextDisabled;  // 0x7a0
    RGBAColor rgbaDisabled;  // 0x7a4
    SNO snoFont;  // 0x7a8
    int32 nFontSize;  // 0x7ac
    CRefString sDecoratedLabel;  // 0x7b0
    CRefString sTooltipLabel;  // 0x7c8
    CheckBoxStyle eStyle;  // 0x7e0
    Rect2D rectTextPaddingUIC;  // 0x7e4
    ResizeToFitValue eResizeToFit;  // 0x7f4
    BOOL fAllCaps;  // 0x7f8
    int32 nLinesBeforeTruncation;  // 0x7fc
    BOOL fAcceptsFocus;  // 0x800
private:
    void sInit();
};
static_assert(sizeof(CheckBoxCreationParams) == 0x808, "CheckBoxCreationParams");

class UICheckBox : public UIWindow {  // size 0x260
public:
    UICheckBox(const CheckBoxCreationParams &, const Vector2D &, const Vector2D &);
    UICheckBox(const CheckBoxCreationParams &, const Rect2D *);
    virtual ~UICheckBox();  // vtable slot 0
    void SetEnabled(const BOOL bEnabled);
    BOOL IsEnabled() const {  // line 102
        return m_bCheckBoxEnabled;
    }
    BOOL IsChecked() const {  // line 109
        return m_eCheckBoxState == CS_CHECKED_NORMAL || m_eCheckBoxState == CS_CHECKED_MOUSEOVER || m_eCheckBoxState == CS_CHECKED_DISABLED;
    }
    void SetCheckBoxState(const BOOL fCheckBoxState, const BOOL fRunOnClick);
    void SetCheckBoxHoverState(const BOOL fHover);
    void SetText(LPCSTR uszNewText);
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput() {  // vtable slot 13; line 154
        return TRUE;
    }
    virtual UIID HitTest(const IVector2D & pt) {  // vtable slot 15; line 161
        if (IsActive() && IsEnabled() && IsInside(pt))
            return GetID();
        return GetUIIDNone();
    }
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting);  // vtable slot 16
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 191
        return eType == UIT_CHECKBOX || UIWindow::IsType(eType);
    }
    virtual BOOL CanAcceptKeyboardFocus() const {  // vtable slot 2; line 204
        return m_bAcceptsFocus && IsActive() && IsEnabled();
    }
    virtual void LostKeyboardFocus();  // vtable slot 4
    virtual void GainedKeyboardFocus();  // vtable slot 5
    virtual void EnteredHitRegion();  // vtable slot 20
    virtual void LeftHitRegion();  // vtable slot 21
private:
    void sInit(const CheckBoxCreationParams & tParams, const Vector2D &, const Vector2D & vecDim);
    BOOL sInput(const BufferedInput & tInput, BOOL & fPassToBaseClass);
    BOOL sOnMouseMove();
    BOOL sToggleState();
    void sSetCheckBoxVisualState(CheckBoxState eCheckBoxState, BOOL fRunOnClick);
public:
    UIMsgFunctionPtr m_pfnClickPredicate;  // 0x1d0
    SNO m_snoClickSound;  // 0x1d8
    SNO m_snoMouseoverSound;  // 0x1dc
    CheckBoxState m_eCheckBoxState;  // 0x1e0
    BOOL m_bCheckBoxEnabled;  // 0x1e4
    BOOL m_bAcceptsFocus;  // 0x1e8
    RGBAColor m_rgbaDisabled;  // 0x1ec
    ImageTextureFrame m_tTextureFrames[6];  // 0x1f0
    RGBAColor m_rgbaTextColors[6];  // 0x220
    RGBAColor m_rgbaDropShadowColors[6];  // 0x238
    UIWindow * m_ptWndBox;  // 0x250
    UIText * m_ptTextLabel;  // 0x258
};
static_assert(sizeof(UICheckBox) == 0x260, "UICheckBox");
