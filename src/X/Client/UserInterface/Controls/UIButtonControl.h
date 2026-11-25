// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIButtonControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UITextControl.h"

struct UIControlDataExt;
struct Rect2D;
struct BufferedInput;

typedef int32 UIAttributeType;

enum ButtonStyle {
    BUTTONSTYLE_NORMAL = 0,
    BUTTONSTYLE_STRETCH_WITH_CAPS = 1,
};

enum ButtonIconAlignment {
    BUTTON_ICON_LEFT = 0,
    BUTTON_ICON_RIGHT = 1,
};

class ButtonCreationParams : public TextCreationParams {  // size 0x890
public:
    ButtonCreationParams(const UIType, const UIID &, const UIID &);
    ButtonCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    ImageTextureFrame tTextureFrame[3];  // 0x7d8
    ImageTextureFrame tIconTextureFrame[3];  // 0x7f0
    BOOL fHasDisabledTextureFrame;  // 0x808
    ImageTextureFrame tTextureFrameDisabled;  // 0x80c
    ImageTextureFrame tIconTextureFrameDisabled;  // 0x814
    RGBAColor rgbaTextHighlight;  // 0x81c
    RGBAColor rgbaTextHighlightDropShadow;  // 0x820
    RGBAColor rgbaTextPushed;  // 0x824
    RGBAColor rgbaTextPushedDropShadow;  // 0x828
    RGBAColor rgbaDisabled;  // 0x82c
    UIFunctionPtr pfnOnHitRegionEnter;  // 0x830
    UIFunctionPtr pfnOnHitRegionExit;  // 0x838
    UIFunctionPtr pfnOnLeftMouseUp;  // 0x840
    UIFunctionPtr pfnOnLeftMouseDown;  // 0x848
    UIMsgFunctionPtr pfnClickPredicate;  // 0x850
    int32 nRepeatRate;  // 0x858
    BOOL fDoActionOnMouseDown;  // 0x85c
    BOOL fIsDragTarget;  // 0x860
    BOOL fDelayRepeat;  // 0x864
    ButtonStyle eButtonStyle;  // 0x868
    int32 nUIAction;  // 0x86c
    int32 nUIActionAlt;  // 0x870
    SNO snoMouseoverSound;  // 0x874
    SNO snoClickSound;  // 0x878
    SNO snoRightClickSound;  // 0x87c
    Vector2D vecIconDim;  // 0x880
    ButtonIconAlignment eButtonIconAlignment;  // 0x888
    int32 nIconTextPaddingX;  // 0x88c
private:
    void sInit();
};
static_assert(sizeof(ButtonCreationParams) == 0x890, "ButtonCreationParams");

enum ButtonState {
    BS_NORMAL = 0,
    BS_MOUSEOVER = 1,
    BS_PUSHED = 2,
    BS_MAX = 3,
};

class UIButton : public UIText {  // size 0x4c8
public:
    UIButton(const ButtonCreationParams &, const Vector2D &, const Vector2D &);
    UIButton(const ButtonCreationParams &, const Rect2D *);
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput();  // vtable slot 13
    virtual UIID HitTest(const IVector2D & pt);  // vtable slot 15
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting);  // vtable slot 16
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    virtual void EnteredHitRegion();  // vtable slot 20
    virtual void LeftHitRegion();  // vtable slot 21
    virtual void GetTextRegionRectUIC(Rect2D & rectTextExtentsUIC) const;  // vtable slot 36
protected:
    virtual Vector2D CalculateTextSize() const;  // vtable slot 38
public:
    virtual ~UIButton() {}  // vtable slot 0; line 113
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 301
        return eType == UIT_BUTTON || UIText::IsType(eType);
    }
    void SetTextureFrameDisabled(const ImageTextureFrame & tTextureFrame);
    void SetAllTextureFrames(const ImageTextureFrame & tTextureFrame);
    void SetTextureFrame(const ButtonState eState, const ImageTextureFrame & tTextureFrame);
    void SetTextureFrame(const ButtonState eState, LPCSTR szDecoratedLabel);
    virtual void SetButtonState(ButtonState eButtonState, const BOOL bForceUpdate, const BOOL bSuppressSounds);  // vtable slot 39
    ButtonState GetButtonState();
    void SetTextColor(const RGBAColor & rgbaColor);
    virtual void SetEnabled(const BOOL bEnabled);  // vtable slot 35
    void SetIconTextureFrame(const ButtonState eState, const ImageTextureFrame & tTextureFrame);
    int32 GetUIAction() const {  // line 194
        return m_nUIAction;
    }
    int32 GetUIActionAlt() const {  // line 204
        return m_nUIActionAlt;
    }
    void SetUIAction(int32 nAction, int32 nAltAction);
    void SetClickSound(const SNO snoSound) {  // line 270
        m_snoClickSound = snoSound;
    }
protected:
    void sRunOnClickFunction(UIAttributeType eAttribute, const BOOL fRepeated);
    void sDraw(const uint8 bLayerAlpha, float & flZSlice, BOOL fTextButtonOffset);
private:
    void sInit(const ButtonCreationParams & tParams);
    BOOL sInput(const BufferedInput & tInput, BOOL & fPassToBaseClass);
    void sCustomDraw(const uint8 bLayerAlpha, float & flZSlice);
    void sDrawStretchWithCaps(const Rect2D & rectWindowUIC, const Rect2D & rectClippingUIC, uint8, const RGBAColor & rgbaColor, float flZ);
public:
    ImageTextureFrame m_tTextureFrame[3];  // 0x3e8
    ImageTextureFrame m_tIconTextureFrame[3];  // 0x400
    BOOL m_fHasDisabledTextureFrame;  // 0x418
    ImageTextureFrame m_tTextureFrameDisabled;  // 0x41c
    ImageTextureFrame m_tIconTextureFrameDisabled;  // 0x424
    RGBAColor m_rgbaTextOriginal;  // 0x42c
    RGBAColor m_rgbaTextOriginalDropShadow;  // 0x430
    RGBAColor m_rgbaTextHighlight;  // 0x434
    RGBAColor m_rgbaTextHighlightDropShadow;  // 0x438
    RGBAColor m_rgbaTextPushed;  // 0x43c
    RGBAColor m_rgbaTextPushedDropShadow;  // 0x440
    RGBAColor m_rgbaDisabled;  // 0x444
    ButtonIconAlignment m_eButtonIconAlignment;  // 0x448
    int32 m_nIconTextPaddingX;  // 0x44c
    BOOL m_fDoActionOnMouseDown;  // 0x450
    BOOL m_fIsDragTarget;  // 0x454
    BOOL m_fIsDragSource;  // 0x458
    int32 m_nUIAction;  // 0x45c
    int32 m_nUIActionAlt;  // 0x460
    ButtonState m_eButtonState;  // 0x464
    UIFunctionPtr m_pfnOnHitRegionEnter;  // 0x468
    UIFunctionPtr m_pfnOnHitRegionExit;  // 0x470
    UIFunctionPtr m_pfnOnLeftMouseUp;  // 0x478
    UIFunctionPtr m_pfnOnLeftMouseDown;  // 0x480
    UIMsgFunctionPtr m_pfnClickPredicate;  // 0x488
    int32 m_nRepeatRate;  // 0x490
    int32 m_nRepeatTicks;  // 0x494
    BOOL m_fDelayRepeat;  // 0x498
    BOOL m_fDrawIcon;  // 0x49c
    BOOL m_fAllowClickedTextOffset;  // 0x4a0
    ButtonStyle m_eButtonStyle;  // 0x4a4
    uint32 m_dwSeed;  // 0x4a8
    uint32 m_dwButtonDownOverrideTime;  // 0x4ac
    SNO m_snoMouseoverSound;  // 0x4b0
    SNO m_snoClickSound;  // 0x4b4
    SNO m_snoRightClickSound;  // 0x4b8
    UIID m_idIcon;  // 0x4c0
};
static_assert(sizeof(UIButton) == 0x4c8, "UIButton");
