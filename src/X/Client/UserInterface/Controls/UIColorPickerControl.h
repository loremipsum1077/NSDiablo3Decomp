// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIColorPickerControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class ColorPickerCreationParams : public WindowCreationParams {  // size 0x738
public:
    ColorPickerCreationParams(const UIType, const UIID &, const UIID &);
    ColorPickerCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIFunctionPtr m_pfnOnColorChange;  // 0x730
};
static_assert(sizeof(ColorPickerCreationParams) == 0x738, "ColorPickerCreationParams");

struct BufferedInput;
struct IVector2D;
struct RGBAColor;

class UIColorPicker : public UIWindow {  // size 0x208
public:
    UIColorPicker(const ColorPickerCreationParams &, const Vector2D &, const Vector2D &);
    UIColorPicker(const ColorPickerCreationParams & tParams, const Rect2D * pRect);
    virtual ~UIColorPicker();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 47
        return eType == UIT_COLORPICKER || UIWindow::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    void SetHue(double);
    double GetHue() const;
    void SetFromRGBA(const RGBAColor &);
    RGBAColor GetSelectedColor() const;
    virtual void PostCreate();  // vtable slot 18
private:
    void sInit(const ColorPickerCreationParams &);
    BOOL sInput(const BufferedInput & tInput);
    BOOL SampleColorAtPoint(IVector2D pt);
    void SetSaturationAndValue(double dSaturation, double dValue);
    void CreateSaturationValueTexture();
    void DeleteSaturationValueTexture();
public:
    UIID m_idSelectionCursor;  // 0x1d0
    SNO m_snoSaturationValueTexture;  // 0x1d8
    double m_dSelectedHue;  // 0x1e0
    double m_dSelectedSaturation;  // 0x1e8
    double m_dSelectedValue;  // 0x1f0
    BOOL m_bSVTextureDirty;  // 0x1f8
    UIFunctionPtr m_pfnOnColorChange;  // 0x200
};
static_assert(sizeof(UIColorPicker) == 0x208, "UIColorPicker");
