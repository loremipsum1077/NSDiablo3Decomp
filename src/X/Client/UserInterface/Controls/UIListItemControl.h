// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIListItemControl.h

#pragma once

#include "../../AnimationCommon.h"
#include "UITextControl.h"

struct UIControlDataExt;
struct Rect2D;
struct BufferedInput;
struct TextRenderProperties;
struct UIControlSourceKey;

class ListItemCreationParams : public TextCreationParams {  // size 0x7f0; line 21
public:
    ListItemCreationParams(const UIType, const UIID &, const UIID &);
    ListItemCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    ImageTextureFrame m_tTexUnselected;  // 0x7d8
    ImageTextureFrame m_tTexSelected;  // 0x7e0
    ImageTextureFrame m_tTexHover;  // 0x7e8
private:
    void sInit();
};
static_assert(sizeof(ListItemCreationParams) == 0x7f0, "ListItemCreationParams");

class UIListItem : public UIText {  // size 0x4a0
public:
    UIListItem(const ListItemCreationParams &, const Vector2D &, const Vector2D &);
    UIListItem(const ListItemCreationParams &, const Rect2D *);
    virtual ~UIListItem();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 63
        return eType == UIT_LISTITEM || UIText::IsType(eType);
    }
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual void SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim);  // vtable slot 30
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual UIID HitTest(const IVector2D & pt) {  // vtable slot 15; line 91
        if (!IsActive())
            return GetUIIDNone();
        UIID idHit = HitTestChildren(pt);
        if (idHit == GetUIIDNone())
        {
            if (IsInside(pt))
                return GetID();
            return GetUIIDNone();
        }
        return idHit;
    }
    virtual void EnteredHitRegion();  // vtable slot 20
    virtual void LeftHitRegion();  // vtable slot 21
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    void SetOwnerListBox(const UIID & idOwnerListBox);
    const UIID & GetOwnerListBox() const;
    void SetMouseoverSound(const SNO snoSound);
    enum {
        UNSELECTED = 0,
        SELECTED = 1,
    };
    typedef int32 SelectedState;
    enum {
        NORMAL = 0,
        HOVER = 1,
    };
    typedef int32 HoverState;
    LPCSTR GetItemText() const;
    void SetItemText(LPCSTR szNewText);
    void * GetItemData() const;
    void SetItemData(void * pNewData);
    void SetItemIndex(int32 nItemIndex);
    int32 GetItemIndex() const;
    static float GetBasicItemHeight(const TextRenderProperties & tProps);
    void SetBasicItemFont(const SNO snoFont);
    void SetBasicItemFontSize(const int32 nFontSize);
    void SetBackgroundAnimations(SNO snoAnimationSelectedState, SNO snoAnimationHoveredState);
    void ResetBackgroundAnimations();
    void EnterSelectedState(UIListItem::SelectedState eNewState);
    void EnterHoverState(UIListItem::HoverState eNewState);
    BOOL IsSelected() const { return m_eSelectedState == SELECTED; }  // line 208
    BOOL IsHovered() const { return m_eHoverState == HOVER; }  // line 209
    BOOL IsBasicItem() const;
    typedef LPCSTR (*GetTextFunction)(const UIListItem &);
    typedef void (*SetTextFunction)(UIListItem &, LPCSTR);
    typedef void (*OnSetDataCallback)(UIListItem &, void *);
    typedef void (*EnterSelectedStateFunction)(UIListItem &, UIListItem::SelectedState);
    typedef void (*EnterHoverStateFunction)(UIListItem &, UIListItem::HoverState);
    struct CustomFunctions {  // size 0x28
        UIListItem::OnSetDataCallback pfnOnSetDataCallback;  // 0x00
        UIListItem::SetTextFunction pfnSetItemText;  // 0x08
        UIListItem::GetTextFunction pfnGetItemText;  // 0x10
        UIListItem::EnterSelectedStateFunction pfnEnterSelectedState;  // 0x18
        UIListItem::EnterHoverStateFunction pfnEnterHoverState;  // 0x20
    };
    void SetCustomFunctions(UIListItem::OnSetDataCallback pfnOnSetDataCallback, UIListItem::SetTextFunction pfnSetItemText, UIListItem::GetTextFunction pfnGetItemText, UIListItem::EnterSelectedStateFunction pfnEnterSelectedState, UIListItem::EnterHoverStateFunction pfnEnterHoverState);
    struct CreateParams {  // size 0x38
        CreateParams();
        UIID m_idOwnerListBox;  // 0x00
        int32 m_nNewItemIndex;  // 0x08
        SNO m_snoHighlightAnimation;  // 0x0c
        SNO m_snoHoverAnimation;  // 0x10
        LPCSTR m_szText;  // 0x18
        void * m_pData;  // 0x20
        SNO m_snoFont;  // 0x28
        int32 m_nFontSize;  // 0x2c
        SNO m_snoMouseoverSound;  // 0x30
    };
    static UIListItem & CreateBasicItem(LPCSTR, const UIID &, float, UIListItem::CreateParams *);
    static UIListItem & CreateFromTemplate(UIControlSourceKey &, LPCSTR, const UIID &, UIListItem::CreateParams *, UIListItem::CustomFunctions *);
protected:
    BOOL m_fIsBasicItem;  // 0x3e4
    UIID m_idOwnerListBox;  // 0x3e8
    int32 m_nItemIndex;  // 0x3f0
    void * m_pItemData;  // 0x3f8
    UIListItem::SelectedState m_eSelectedState;  // 0x400
    UIListItem::HoverState m_eHoverState;  // 0x404
    SNO m_snoMouseoverSound;  // 0x408
    SNO m_snoAnimationUnselectedState;  // 0x40c
    AnimState m_tAnimStateUnselectedState;  // 0x410
    ImageTextureFrame m_tTexUnselected;  // 0x428
    SNO m_snoAnimationSelectedState;  // 0x430
    AnimState m_tAnimStateSelectedState;  // 0x434
    ImageTextureFrame m_tTexSelected;  // 0x44c
    SNO m_snoAnimationHoveredState;  // 0x454
    AnimState m_tAnimStateHoveredState;  // 0x458
    ImageTextureFrame m_tTexHover;  // 0x470
    UIListItem::OnSetDataCallback m_pfnCustomOnSetDataCallback;  // 0x478
    UIListItem::SetTextFunction m_pfnCustomSetItemText;  // 0x480
    UIListItem::GetTextFunction m_pfnCustomGetItemText;  // 0x488
    UIListItem::EnterSelectedStateFunction m_pfnCustomEnterSelectedState;  // 0x490
    UIListItem::EnterHoverStateFunction m_pfnCustomEnterHoverState;  // 0x498
private:
    void sInit(const ListItemCreationParams & tParams, const Vector2D &, const Vector2D &);
    BOOL sInput(const BufferedInput &);
};
static_assert(sizeof(UIListItem) == 0x4a0, "UIListItem");
