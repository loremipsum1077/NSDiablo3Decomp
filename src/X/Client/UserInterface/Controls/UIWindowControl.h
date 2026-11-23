// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIWindowControl.h

#pragma once

#include <string.h>

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../AnimationCommon.h"
#include "../../Gfx/GfxCommon.h"
#include "UIControlControl.h"
#include "UIControlGroup.h"
#include "../Interface/UIFunctions.h"
#include "../UIInit.h"
#include "../UIMessage.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

struct BufferedInput;
struct TiledWindowPieces;
struct TrackPieces;
struct UIControlDataExt;
struct UIID;
struct VertTiledWindowPieces;

typedef int32 UIAttributeType;

typedef int32 InputButton;

typedef int32 UIJustifyHorizontal;

enum TiledStyle {
    TILEDSTYLE_NONE = -1,
    TILEDSTYLE_DEFAULT = 0,
    TILEDSTYLE_EDITBOX = 1,
    TILEDSTYLE_COMBOBOX_BUTTON = 2,
    TILEDSTYLE_COMBOBOX_LISTBOX = 3,
    TILEDSTYLE_SCROLLBAR_TRACK_VERTICAL = 4,
    TILEDSTYLE_TEXT_TOOLTIP = 5,
    TILEDSTYLE_SKILL_TOOLTIP = 6,
    TILEDSTYLE_ARROW_VERTICAL = 7,
    TILEDSTYLE_ITEM_TOOLTIP_BODY = 8,
    TILEDSTYLE_CONFIRMATION_DLG = 9,
    TILEDSTYLE_ITEM_TOOLTIP_COST = 10,
    TILEDSTYLE_FOLLOWERSWAP_DLG = 11,
    TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION = 12,
    TILEDSTYLE_CHATBALLOON = 14,
    TILEDSTYLE_TRAIT_TOOLTIP = 15,
    TILEDSTYLE_CONVERSATION_DLG = 16,
    TILEDSTYLE_TUTORIAL_TL = 17,
    TILEDSTYLE_TUTORIAL_TR = 18,
    TILEDSTYLE_TUTORIAL_BL = 19,
    TILEDSTYLE_TUTORIAL_BR = 20,
    TILEDSTYLE_TUTORIAL_NO_ARROW = 21,
    TILEDSTYLE_BOSS_DLG = 22,
    TILEDSTYLE_TOOLTIP_TITLE = 23,
    TILEDSTYLE_TOOLTIP_TITLE_ANCIENT = 24,
    TILEDSTYLE_ITEM_TOOLTIP_BODY_ANCIENT = 25,
    TILEDSTYLE_ITEM_TOOLTIP_COST_ANCIENT = 26,
    TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION_ANCIENT = 27,
    TILEDSTYLE_TOOLTIP_TITLE_PRIMAL_ANCIENT = 28,
    TILEDSTYLE_ITEM_TOOLTIP_BODY_PRIMAL_ANCIENT = 29,
    TILEDSTYLE_ITEM_TOOLTIP_COST_PRIMAL_ANCIENT = 30,
    TILEDSTYLE_ITEM_TOOLTIP_INSTRUCTION_PRIMAL_ANCIENT = 31,
    TILEDSTYLE_COUNT = 32,
};

enum EatsClicksValue {
    EC_IGNORE = 0,
    EC_EAT = 1,
    EC_EAT_MASKED = 3,
};

enum UIHorizontalAnchorPoint {
    UI_ANCHOR_CENTER = 0,
    UI_ANCHOR_LEFT = 1,
    UI_ANCHOR_RIGHT = 2,
    UI_ANCHOR_STRETCH_LEFT_AND_RIGHT = 3,
};

enum UIVerticalAnchorPoint {
    UI_VERT_ANCHOR_CENTER = 0,
    UI_VERT_ANCHOR_TOP = 1,
    UI_VERT_ANCHOR_BOTTOM = 2,
};

enum UICenterOnParent {
    CP_DISABLED = 0,
    CP_X = 1,
    CP_Y = 2,
    CP_XY = 3,
    CP_X_RIGHT = 4,
};

enum {
    PARENT_ANCHOR_LEFT_BIT = 0,
    PARENT_ANCHOR_TOP_BIT = 1,
    PARENT_ANCHOR_RIGHT_BIT = 2,
    PARENT_ANCHOR_BOTTOM_BIT = 3,
};

enum {
    UIWINDOW_DRAGGING_BIT = 0,
    UIWINDOW_IGNORES_CLIPPING_BIT = 1,
    UIWINDOW_CLIPPING_STORED_BIT = 2,
    UIWINDOW_DRAGGABLE_BIT = 3,
    UIWINDOW_DRAG_CONSTRAIN_VERTICAL_BIT = 4,
    UIWINDOW_DRAG_CONSTRAIN_HORIZONTAL_BIT = 5,
    UIWINDOW_CLEAR_HIGHLIGHT_LAYER_BIT = 6,
    UIWINDOW_ROLLED_UP_BIT = 7,
    UIWINDOW_CENTER_IN_STACK_BIT = 8,
    UIWINDOW_RESIZE_TO_ENCLOSE_CHILDREN_BIT = 9,
    UIWINDOW_CONTEXT_MENU_LEFT_CLICK_BIT = 10,
    UIWINDOW_CONTEXT_MENU_TOGGLE_BIT = 11,
    UIWINDOW_INPUT_FOCUS_GROUP_BIT = 12,
    UIWINDOW_DIMENSION_INITIALIZED = 13,
};

// Defined in UIWindowControl.cpp (lines 89-90); UITextControl.cpp reads them.
extern const float UIDC_DIALOG_Z_INCREMENT;
extern const float UIDC_DIALOG_Z_START;
extern const float UIDC_CONTROL_Z_INCREMENT;

struct UIWindowFunctionsPtr {  // size 0x150
    UIFunctionPtr pfnOnClick;  // 0x00
    UIFunctionPtr pfnOnRightClick;  // 0x08
    UIFunctionPtr pfnOnDoubleClick;  // 0x10
    UIFunctionPtr pfnOnLButtonStillDown;  // 0x18
    UIConsoleFunctionPtr pfnConsoleButtonDown[18];  // 0x20
    UIConsoleFunctionPtr pfnConsoleButtonHeld[18];  // 0xb0
    UIConsoleFunctionPtr pfnLStickHandler;  // 0x140
    UIConsoleFunctionPtr pfnRStickHandler;  // 0x148
};
static_assert(sizeof(UIWindowFunctionsPtr) == 0x150, "UIWindowFunctionsPtr");

class WindowCreationParams : public ControlCreationParams {  // size 0x730
public:
    WindowCreationParams(const UIType, const UIID &, const UIID &);
    WindowCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    uint8 bAlpha;  // 0xa8
    SNO snoCustomShader;  // 0xac
    RenderLayer eRenderLayerOverride;  // 0xb0
    SNO snoAnim2D;  // 0xb4
    ImageTextureFrame tImageTexBkgrnd;  // 0xb8
    ImageTextureFrame tImageTexMask;  // 0xc0
    UIHorizontalAnchorPoint eHorizontalAnchor;  // 0xc8
    UIVerticalAnchorPoint eVerticalAnchor;  // 0xcc
    uint32 dwFlags;  // 0xd0
    TiledStyle eTiledStyle;  // 0xd4
    EatsClicksValue eEatsClicks;  // 0xd8
    CHAR szContextMenu[512];  // 0xdc
    CHAR szTargetParentContextMenu[512];  // 0x2dc
    CHAR szTooltipLabel[128];  // 0x4dc
    UIJustifyHorizontal eTooltipTextJustify;  // 0x55c
    UIFunctionPtr pfnOnClick;  // 0x560
    UIFunctionPtr pfnOnRightClick;  // 0x568
    UIFunctionPtr pfnOnDoubleClick;  // 0x570
    UIFunctionPtr pfnOnLButtonStillDown;  // 0x578
    UIFunctionPtr pfnTooltip;  // 0x580
    UIFunctionPtr pfnOnDragStart;  // 0x588
    UIMsgFunctionPtr pfnOnDrag;  // 0x590
    UIFunctionPtr pfnOnContextMenu;  // 0x598
    BOOL fAbsoluteCoordinates;  // 0x5a0
    Vector2D vecTL;  // 0x5a4
    Vector2D vecDim;  // 0x5ac
    Vector2D vecMaxDim;  // 0x5b4
    Vector2D vecMinDim;  // 0x5bc
    Angle aRot;  // 0x5c4
    BOOL bMirrorX;  // 0x5c8
    BOOL bMirrorY;  // 0x5cc
    BOOL bNoSnap;  // 0x5d0
    uint32 dwParentAnchorFlags;  // 0x5d4
    int32 nToolTipWidth;  // 0x5d8
    Vector2D vecTooltipOffset;  // 0x5dc
    AlignPosition eToolTipAlignHorizontal;  // 0x5e4
    AlignPosition eToolTipAlignVertical;  // 0x5e8
    UICenterOnParent eCenterOnParent;  // 0x5ec
    uint32 nInputFocusOrder;  // 0x5f0
    Vector2D vecStandardDefOffset;  // 0x5f4
    UIConsoleFunctionPtr pfnConsoleButtonDown[18];  // 0x600
    UIConsoleFunctionPtr pfnConsoleButtonHeld[18];  // 0x690
    UIConsoleFunctionPtr pfnLStickHandler;  // 0x720
    UIConsoleFunctionPtr pfnRStickHandler;  // 0x728
    BOOL HasAnyFunctionsPtr() const;
private:
    void sInit();
};
static_assert(sizeof(WindowCreationParams) == 0x730, "WindowCreationParams");

class UIWindow : public UIControlGroup {  // size 0x1d0
public:
    UIWindow(const WindowCreationParams &, const Vector2D &, const Vector2D &);
    UIWindow(const WindowCreationParams &, const Rect2D *);
    virtual ~UIWindow();  // vtable slot 0
    void SetWindowColor(const RGBAColor & rgbaColor) {  // line 254
        m_rgbaWindowColor = rgbaColor;
        SetTransparency(rgbaColor.a);
    }
    RGBAColor GetWindowColor() const { return m_rgbaWindowColor; }  // line 262
    void SetWindowRotation(const Angle aRot) {  // line 268
        m_aRot = Math::NormalizeAngle(aRot);
    }
    void SetWindowMirroring(BOOL bMirrorX, BOOL bMirrorY) {  // line 274
        m_bMirrorX = bMirrorX;
        m_bMirrorY = bMirrorY;
    }
    void SetNoSnap(BOOL bNoSnap) { m_bNoSnap = bNoSnap; }  // line 282
    void GetWindowRect(Rect2D & rect) const {  // line 289
        rect = m_rect;
    }
    const Rect2D & GetWindowRect() const { return m_rect; }  // line 295
    void GetUnshiftedWindowRect(Rect2D & rect) const {  // line 301
        rect = m_rectUnshifted;
    }
    const Rect2D & GetUnshiftedWindowRect() const;
    void GetUnshiftedClippingRect(Rect2D & rect) const {  // line 313
        rect = m_rectClippingUnshifted;
    }
    void GetClippingRect(Rect2D & rect) const {  // line 320
        rect = m_rectClipping;
    }
    const Rect2D & GetCascadedClippingRect();
    BOOL IsInside(const IVector2D & pt);
    EatsClicksValue EatsClicks() const { return m_eEatsClicks; }  // line 337
    void SetTransparency(const uint8 bAlpha) {  // line 344
        m_rgbaWindowColor.a = bAlpha;
    }
    uint8 GetTransparency() { return m_rgbaWindowColor.a; }  // line 351
    void SetTextureFrame(LPCSTR szDecoratedLabel);
    void SetTextureFrame(const ImageTextureFrame & tImage) { m_tImageTexBkgrnd = tImage; }  // line 362
    const ImageTextureFrame & GetTextureFrame() const;
    void GetTextureFrameSize(Vector2D & vecSize) const;
    void SetMaskTextureFrame(LPCSTR);
    void SetMaskTextureFrame(const ImageTextureFrame & tImage);
    const ImageTextureFrame & GetMaskTextureFrame() const;
    void SetShader(const SNO snoShader) { m_snoCustomShader = snoShader; }  // line 398
    SNO GetShader() const { return m_snoCustomShader; }  // line 405
    void SetBackgroundAnimation(SNO snoBackgroundAnimation2D);
    virtual void SetWindowRect(const Rect2D & rectUIC);  // vtable slot 29
    virtual void SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 30
    void GetRelativePosition(Vector2D & vecTL, Vector2D & vecDim) const {  // line 423
        vecTL = m_vecTL;
        vecDim = m_vecDim;
    }
    void SetMinDimension(Vector2D);
    void SetMaxDimension(Vector2D);
    void GetMaxDimension(Vector2D & vecMaxDim) { vecMaxDim = m_vecMaxDim; }  // line 443
    void GetOriginalDimension(Vector2D & vecOrigDim) { vecOrigDim = m_vecOriginalDim; }  // line 449
    UIHorizontalAnchorPoint GetHorizontalAnchor() const { return m_eHorizontalAnchor; }  // line 456
    void SetHorizontalAnchor(const UIHorizontalAnchorPoint);
    UIVerticalAnchorPoint GetVerticalAnchor() const { return m_eVerticalAnchor; }  // line 467
    void SetVerticalAnchor(const UIVerticalAnchorPoint);
    void RecomputeCascadedAnchor();
    void CenterOverACD(const ACDID idACD);
    BOOL IsAnchored() const {  // line 485
        const uint32 cdwUnanchored = (1 << PARENT_ANCHOR_LEFT_BIT) | (1 << PARENT_ANCHOR_TOP_BIT);
        return m_dwParentAnchorFlags != 0 && m_dwParentAnchorFlags != cdwUnanchored;
    }
    uint32 GetAnchorEdges() const;
    LPCSTR GetTooltipTextLabel() const { return m_szTooltipLabel ? m_szTooltipLabel : ""; }  // line 506
    void SetTooltipTextLabel(LPCSTR szTooltipText) {  // line 517
        if (m_szTooltipLabel != NULL) {
            delete[] m_szTooltipLabel;
        }
        uint32 dwLen = strlen(szTooltipText);
        if (dwLen != 0) {
            m_szTooltipLabel = new CHAR[dwLen + 1];
            strcpy(m_szTooltipLabel, szTooltipText);
        } else {
            m_szTooltipLabel = NULL;
        }
    }
    int32 GetTextTooltipWidth() const { return m_nToolTipWidth; }  // line 539
    void SetTextTooltipWidth(int32);
    void GetTooltipAlignment(AlignPosition & eTipHorizontal, AlignPosition & eTipVertical) const {  // line 556
        eTipHorizontal = m_eToolTipAlignHorizontal;
        eTipVertical = m_eToolTipAlignVertical;
    }
    void SetTooltipAlignment(const AlignPosition &, const AlignPosition &);
    Vector2D GetTooltipOffset() const { return m_vecTooltipOffset; }  // line 575
    void SetTooltipOffset(Vector2D);
    BOOL IsClipped();
    virtual void RecalculateLayout(int32 nStartingChildIndex);  // vtable slot 24
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput();  // vtable slot 13
    virtual UIID HitTest(const IVector2D & pt);  // vtable slot 15
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting);  // vtable slot 16
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 633
        return eType == UIT_WINDOW || UIControlGroup::IsType(eType);
    }
    virtual void Move(const Vector2D & vecDelta);  // vtable slot 9
    Vector2D GetAnchorShiftUIC() const;
    virtual void UpdateAbsolutePositions();  // vtable slot 22
    void MoveTo(const Vector2D & vecTL);
    void MoveInsideViewport();
    void SetClipRect(const Rect2D & rectClipUIC);
    virtual void SetRelativeClipRect(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 31
    void SetInitialEditText();
    void ResizeToFitChildren();
    void Scroll(const Vector2D & vecScroll);
    BOOL HandleContextMenu(const BufferedInput & tInput);
    BOOL IsContextMenuToggled();
    BOOL IsContextMenuCalledOnLeftClick();
    void SetContextMenuId(LPCSTR);
    void SetContextMenuTargetParent(LPCSTR);
    virtual void OnScrollbarPositionChanged(const UIID & idScrollbar) {}  // vtable slot 32; line 734
    void SetOnClickFunction(const UIFunctionPtr pfnOnClick) {  // line 741
        AllocateFunctionsPtrIfNeeded();
        m_pFunctionsPtr->pfnOnClick = pfnOnClick;
    }
    void SetOnDoubleClickFunction(const UIFunctionPtr);
    void SetOnRightClickFunction(const UIFunctionPtr);
    void SetOnButtonFunction(InputButton, const UIConsoleFunctionPtr);
    BOOL IsRolledUp() const;
    BOOL IsCenteredInStack() const;
    BOOL IsInputFocusGroup() const;
    void SetIsInputFocusGroup(BOOL);
    uint32 GetInputFocusOrder() const;
    void SetInputFocusOrder(uint32);
    void SetTiledStyle(const TiledStyle eTiledStyle) { m_eTiledStyle = eTiledStyle; }  // line 827
    void SetTextureFrameScale(Vector2D);
    void SetTextureFrameOffset(Vector2D);
    void SetCenterOnParent(UICenterOnParent);
    void SetStereoDepth(float flStereoDepth) { m_flStereoDepth = flStereoDepth; }  // line 851
    float GetStereoDepth() const { return m_flStereoDepth; }  // line 857
    void SetEatsClicks(EatsClicksValue eEatsClicks) { m_eEatsClicks = eEatsClicks; }  // line 864
protected:
    // DWARF places m_tImageTexBkgrnd (UIWindow+0xa0) in the protected section,
    // yet UIConsoleScrollIndicator::PositionAndSizeThumb/GetThumbSize read it
    // directly on a UIWindow & (no accessor call in the original bytes), so the
    // original UIWindow must have granted it friendship; DWARF does not encode
    // friend declarations. Additive, codegen-neutral (verified: consumers byte-
    // identical with and without it).
    friend class UIConsoleScrollIndicator;
    BOOL IgnoresClipping();
    void SetIgnoresClipping(const BOOL bIgnoresClipping) {  // line 910
        m_dwFlags = !bIgnoresClipping ? (m_dwFlags & ~(1 << UIWINDOW_IGNORES_CLIPPING_BIT)) : (m_dwFlags | (1 << UIWINDOW_IGNORES_CLIPPING_BIT));
    }
    BOOL ClickIsEaten(const IVector2D & p) const;
    virtual LPCSTR GetTooltipText();  // vtable slot 33
    void AllocateFunctionsPtrIfNeeded() {  // line 927
        if (m_pFunctionsPtr == NULL) {
            m_pFunctionsPtr = (UIWindowFunctionsPtr *)SigmaMemoryNew(sizeof(UIWindowFunctionsPtr), 4, NULL, TRUE);
        }
    }
    void DrawChildren(const uint8 bAlphaCascaded, float & flZSlice);
    void sRelativeCoordsFromCreationParams(Vector2D & vecTL, Vector2D & vecDim, const Rect2D * pRect, const WindowCreationParams & tParams) const;
    Rect2D m_rect;  // 0x88
    RGBAColor m_rgbaWindowColor;  // 0x98
    TiledStyle m_eTiledStyle;  // 0x9c
    ImageTextureFrame m_tImageTexBkgrnd;  // 0xa0
    RenderLayer m_eRenderLayerOverride;  // 0xa8
    uint32 m_dwFlags;  // 0xac
    uint32 m_dwParentAnchorFlags;  // 0xb0
    SNO m_snoBackgroundAnimation;  // 0xb4
    AnimState m_tBackgroundAnimState;  // 0xb8
    Rect2D m_rectUnshifted;  // 0xd0
    Vector2D m_vecTL;  // 0xe0
    Vector2D m_vecDim;  // 0xe8
    Vector2D m_vecMaxDim;  // 0xf0
    Vector2D m_vecMinDim;  // 0xf8
    Vector2D m_vecOriginalDim;  // 0x100
    Angle m_aRot;  // 0x108
    BOOL m_bMirrorX;  // 0x10c
    BOOL m_bMirrorY;  // 0x110
    BOOL m_bNoSnap;  // 0x114
    UIWindowFunctionsPtr * m_pFunctionsPtr;  // 0x118
    Vector2D m_vecStandardDefOffset;  // 0x120
    Vector2D m_vecCascadedStandardDefOffset;  // 0x128
    LPSTR m_szTooltipLabel;  // 0x130
    UIJustifyHorizontal m_eTooltipTextJustify;  // 0x138
    int32 m_nToolTipWidth;  // 0x13c
    Vector2D m_vecTooltipOffset;  // 0x140
    AlignPosition m_eToolTipAlignHorizontal;  // 0x148
    AlignPosition m_eToolTipAlignVertical;  // 0x14c
    UICenterOnParent m_eCenterOnParent;  // 0x150
private:
    ImageTextureFrame m_tImageTexMask;  // 0x154
    SNO m_snoCustomShader;  // 0x15c
    Rect2D m_rectClipping;  // 0x160
    Rect2D m_rectClippingUnshifted;  // 0x170
    Vector2D m_vecClippingTL;  // 0x180
    Vector2D m_vecClippingDim;  // 0x188
    Vector2D m_vecTexOffset;  // 0x190
    Vector2D m_vecTexScale;  // 0x198
    Rect2D m_rectStoredClipping;  // 0x1a0
    EatsClicksValue m_eEatsClicks;  // 0x1b0
    UIHorizontalAnchorPoint m_eHorizontalAnchor;  // 0x1b4
    UIHorizontalAnchorPoint m_eCascadedHorizontalAnchor;  // 0x1b8
    UIVerticalAnchorPoint m_eVerticalAnchor;  // 0x1bc
    UIVerticalAnchorPoint m_eCascadedVerticalAnchor;  // 0x1c0
    float m_flStereoDepth;  // 0x1c4
    uint32 m_nInputFocusOrder;  // 0x1c8
    void sInit(const WindowCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim);
    void sDrawSelf(const uint8 bAlphaCascaded, float & flZSlice);
    void sDrawTrack(const TrackPieces & tPieces, Rect2D & rectWindowUIC, float flZ, uint8 bAlphaVal) const;
    void sDrawVertTiledWindow(const VertTiledWindowPieces & tPieces, Rect2D & rectWindowUIC, float flZ, uint8 bAlphaVal) const;
    void sDrawTiledWindowDefault(const TiledWindowPieces & tPieces, const Rect2D & rectWindowPixels, float flZ, const uint8 bAlphaVal, const BOOL fInsetBackground) const;
    void sDrawTiledWindow(Rect2D & rectWindowPixels, const Rect2D & rectClippingPixels, float flZ, uint8 bAlphaVal) const;
    BOOL sInput(const BufferedInput & tInput);
    BOOL sOnMouseDown(const BufferedInput &);
    void sUpdate();
    void sComputeCascadedAnchor();
    void sComputeCascadedSDOffset();
    void sRelativeToAbsoluteRect(const Vector2D & vecTL, const Vector2D & vecDim, const Vector2D & vecParentTL, Rect2D & rectUIC) const;
    void sRelativeFromAbsoluteRect(Vector2D & vecTL, Vector2D & vecDim, const Rect2D & rectUIC) const;
};
static_assert(sizeof(UIWindow) == 0x1d0, "UIWindow");

// Body inferred: inlined into Console::Screens::InitializeForGame, where its only instruction
// folds into the field loads (so the DWARF has no inlined-subroutine record for it).
inline const Rect2D & UIWindow::GetUnshiftedWindowRect() const {
    return m_rectUnshifted;
}

inline BOOL UIWindow::IsCenteredInStack() const {  // line 786
    return (m_dwFlags & (1 << UIWINDOW_CENTER_IN_STACK_BIT)) != 0;
}

// Body inferred from the copies inlined into UI.cpp (sGetFocusableControls, sIncrementFocus).
inline BOOL UIWindow::IsInputFocusGroup() const {
    return (m_dwFlags & (1 << UIWINDOW_INPUT_FOCUS_GROUP_BIT)) != 0;
}

inline uint32 UIWindow::GetInputFocusOrder() const {  // line 813
    return m_nInputFocusOrder;
}



inline const ImageTextureFrame & UIWindow::GetTextureFrame() const {  // no DWARF line: an accessor with no code of its own
    return m_tImageTexBkgrnd;
}
