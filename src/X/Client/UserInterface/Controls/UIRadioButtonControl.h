// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIRadioButtonControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIButtonControl.h"

struct UIControlDataExt;
struct Rect2D;

class RadioButtonCreationParams : public ButtonCreationParams {  // size 0x8d8; line 18
public:
    RadioButtonCreationParams(const UIType, const UIID &, const UIID &);
    RadioButtonCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    int32 nValue;  // 0x890
    BOOL bDefault;  // 0x894
    UIMsgFunctionPtr pfnClickPredicate;  // 0x898
    ImageTextureFrame tSelectedTextureFrames[3];  // 0x8a0
    ImageTextureFrame tUnselectedTextureFrames[3];  // 0x8b8
    UIID idGroup;  // 0x8d0
private:
    void sInit();
};
static_assert(sizeof(RadioButtonCreationParams) == 0x8d8, "RadioButtonCreationParams");

class UIRadioButton : public UIButton {  // size 0x520
public:
    UIRadioButton(const RadioButtonCreationParams &, const Vector2D &, const Vector2D &);
    UIRadioButton(const RadioButtonCreationParams &, const Rect2D *);
    virtual ~UIRadioButton() {}  // vtable slot 0; line 67
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 73
        return eType == UIT_RADIOBUTTON || UIButton::IsType(eType);
    }
    virtual void PostCreate();  // vtable slot 18
    virtual void SetButtonState(ButtonState eButtonState, const BOOL bForceUpdate, const BOOL bSuppressSounds);  // vtable slot 39
    UIID GetRadioButtonGroup() const { return m_idRadioButtonGroup; }  // line 96
    int32 GetValue() const;
    BOOL IsSelected() const;
    void SetAllTextureFrames(const ImageTextureFrame &);
    void SetTextureFrames(const ImageTextureFrame & tTextureFrameNormal, const ImageTextureFrame & tTextureFrameMouseover, const ImageTextureFrame & tTextureFramePushed);
    void SetTextureFrames(const ImageTextureFrame &, const ImageTextureFrame &, const ImageTextureFrame &, const ImageTextureFrame &, const ImageTextureFrame &, const ImageTextureFrame &, const ImageTextureFrame &);
    void SetValue(const int32 nVal) { m_nValue = nVal; }  // line 129
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
private:
    friend class UIRadioButtonGroup;
    void sInit(const RadioButtonCreationParams & tParams);
    void sUpdateButtonTextureFrames();
    void SetSelected(const BOOL bSelected);
    int32 m_nValue;  // 0x4c8
    BOOL m_bSelected;  // 0x4cc
    BOOL m_bDefault;  // 0x4d0
    UIMsgFunctionPtr m_pfnClickPredicate;  // 0x4d8
    ImageTextureFrame m_tSelectedTextureFrames[3];  // 0x4e0
    ImageTextureFrame m_tUnselectedTextureFrames[3];  // 0x4f8
    ImageTextureFrame m_tMouseOverIconTextureFrame;  // 0x510
    UIID m_idRadioButtonGroup;  // 0x518
};
static_assert(sizeof(UIRadioButton) == 0x520, "UIRadioButton");
