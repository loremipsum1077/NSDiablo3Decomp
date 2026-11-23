// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIPieChartControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"
#include "../../../Sigma/DataStructures/XArray.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/RefString.h"

struct UIControlDataExt;
struct Rect2D;
struct IVector2D;

struct UIPieChartSample {  // size 0x20, line 16
    CRefString sLabel;  // 0x00
    RGBAColor rgbaPieColor;  // 0x18
    float flSampleValue;  // 0x1c
};
static_assert(sizeof(UIPieChartSample) == 0x20, "UIPieChartSample");

class UIPieChartControl : public UIWindow {  // size 0x3e8
public:
    UIPieChartControl(const WindowCreationParams &, const Vector2D &, const Vector2D &);
    UIPieChartControl(const WindowCreationParams &, const Rect2D *);
    virtual ~UIPieChartControl();  // vtable slot 0
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17, line 40
        return eType == UIT_PIECHART || UIWindow::IsType(eType);
    }
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL &);  // vtable slot 16
private:
    void sInit();
    XArray<UIPieChartSample, const UIPieChartSample &, 16> m_artSamples;  // 0x1d0
public:
    float m_flSampleTotal;  // 0x3e0
    int32 m_nTooltipSampleIndex;  // 0x3e4
};
static_assert(sizeof(UIPieChartControl) == 0x3e8, "UIPieChartControl");
