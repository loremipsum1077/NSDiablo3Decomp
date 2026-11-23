// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIConsoleScrollIndicatorControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIScrollbarControl.h"
#include "UIWindowControl.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

struct Rect2D;
struct UIControlDataExt;
struct Vector2D;

class ConsoleScrollIndicatorCreationParams : public WindowCreationParams {  // size 0x738
public:
    ConsoleScrollIndicatorCreationParams(const UIType, const UIID &, const UIID &);
    ConsoleScrollIndicatorCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    int32 nMax;  // 0x730
    ScrollbarVisibility eScrollbarVisibility;  // 0x734
private:
    void sInit();
};
static_assert(sizeof(ConsoleScrollIndicatorCreationParams) == 0x738, "ConsoleScrollIndicatorCreationParams");

class UIConsoleScrollIndicator : public UIWindow {  // size 0x1e8
public:
    UIConsoleScrollIndicator(const ConsoleScrollIndicatorCreationParams &, const Vector2D &, const Vector2D &);
    UIConsoleScrollIndicator(const ConsoleScrollIndicatorCreationParams &, const Rect2D *);
    virtual ~UIConsoleScrollIndicator();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const;  // vtable slot 17
    void SetExtentsAndPosition(int32 nMax, int32 nPosition);
    void GetThumbSize(Vector2D & vecDim) const;
    virtual void SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 30
    void SetScrollbarVisibility(ScrollbarVisibility eNewVisibility) { m_eScrollbarVisibility = eNewVisibility; } // DWARF line 88
private:
    UIID m_idThumb;  // 0x1d0
    int32 m_nMax;  // 0x1d8
    int32 m_nPosition;  // 0x1dc
    ScrollbarVisibility m_eScrollbarVisibility;  // 0x1e0
    void sInit(const ConsoleScrollIndicatorCreationParams & tParams, const Vector2D &, const Vector2D &);
    void PositionAndSizeThumb();
};
static_assert(sizeof(UIConsoleScrollIndicator) == 0x1e8, "UIConsoleScrollIndicator");

// DWARF records this body in this header at line 60.
inline BOOL UIConsoleScrollIndicator::IsType(const UIType eType) const {  // line 60
    return eType == UIT_CONSOLESCROLLINDICATOR || UIWindow::IsType(eType);
}

// Functions defined in this file (bodies are not in the DWARF):
//   line    18  ConsoleScrollIndicatorCreationParams::~ConsoleScrollIndicatorCreationParams()  [0x710029e460]
//   line    60  BOOL UIConsoleScrollIndicator::IsType(const UIType eType) const  [0x710029e4a0]
//   line    88  void UIConsoleScrollIndicator::SetScrollbarVisibility(ScrollbarVisibility eNewVisibility)  [inlined into 1 functions]
