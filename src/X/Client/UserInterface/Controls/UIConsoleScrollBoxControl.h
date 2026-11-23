// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIConsoleScrollBoxControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"
#include "../Console/ConsoleInputUtilities.h"

struct UIControlDataExt;
struct Rect2D;
struct BufferedInput;

class ConsoleScrollBoxCreationParams : public WindowCreationParams {  // size 0x758; line 18
public:
    ConsoleScrollBoxCreationParams(const UIType, const UIID &, const UIID &);
    ConsoleScrollBoxCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    BOOL bRequiresControllerFocus;  // 0x730
    float flScrollIndicatorPaddingTop;  // 0x734
    float flScrollIndicatorPaddingLeft;  // 0x738
    float flScrollIndicatorPaddingRight;  // 0x73c
    float flScrollIndicatorPaddingBottom;  // 0x740
    ImageTextureFrame tImageTexDimmerTop;  // 0x744
    ImageTextureFrame tImageTexDimmerBottom;  // 0x74c
private:
    void sInit();
};
static_assert(sizeof(ConsoleScrollBoxCreationParams) == 0x758, "ConsoleScrollBoxCreationParams");

class UIConsoleScrollBox : public UIWindow {  // size 0x230
public:
    UIConsoleScrollBox(const ConsoleScrollBoxCreationParams &, const Vector2D &, const Vector2D &);
    UIConsoleScrollBox(const ConsoleScrollBoxCreationParams &, const Rect2D *);
    virtual ~UIConsoleScrollBox();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 69
        return eType == UIT_CONSOLESCROLLBOX || UIWindow::IsType(eType);
    }
    float GetScrollIndicatorPaddingTop() const;
    float GetScrollIndicatorPaddingRight() const;
    float GetScrollIndicatorPaddingBottom() const;
    void SetScrollIndicatorPadding(const float, const float, const float);
    virtual void SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 30
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual void PostCreate();  // vtable slot 18
    virtual BOOL HandlesInput() { return TRUE; }  // vtable slot 13; line 113
    float GetScrollLimit() const;
    float GetScrollPos() const { return m_flPos; }  // line 122
    void SetScrollPos(const float flPos);
    void Scroll(const float flDelta);
    void SetExternallyControlled(BOOL bExternallyControlled) { m_bExternallyControlled = bExternallyControlled; }  // line 135
private:
    UIID m_idScrollIndicator;  // 0x1d0
    UIID m_idScrollIndicatorTop;  // 0x1d8
    UIID m_idScrollIndicatorBottom;  // 0x1e0
    BOOL m_bExternallyControlled;  // 0x1e8
    BOOL m_bRequiresControllerFocus;  // 0x1ec
    float m_flScrollIndicatorPaddingTop;  // 0x1f0
    float m_flScrollIndicatorPaddingLeft;  // 0x1f4
    float m_flScrollIndicatorPaddingRight;  // 0x1f8
    float m_flScrollIndicatorPaddingBottom;  // 0x1fc
    float m_flPos;  // 0x200
    float m_nMouseWheelScaleFactor;  // 0x204
    Console::AnalogStickInputFilter m_tStickFilter;  // 0x208
    void sInit(const ConsoleScrollBoxCreationParams & tParams, const Vector2D &, const Vector2D & vecDim);
    BOOL HandleInput(const BufferedInput & tInput);
    void UpdateScrollState();
    void CalculateChildBounds(Rect2D & rectBounds) const;
    void PositionChildControls();
};
static_assert(sizeof(UIConsoleScrollBox) == 0x230, "UIConsoleScrollBox");
