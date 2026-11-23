// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIControlGroup.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "UIControlControl.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../UIMessage.h"
#include "../../../Sigma/DataStructures/XDataStructuresShared.h"
#include "../../../Sigma/SigmaTypes.h"

struct IVector2D;

struct UIControlItem {  // size 0x18
    UIID id;  // 0x00
    UIControl * ptControl;  // 0x08
    UIControl * ptActiveControl;  // 0x10
    UIControlItem(const UIID & id) : id(id), ptControl(NULL), ptActiveControl(NULL) {}  // line 22
    UIControlItem() : id(0), ptControl(NULL), ptActiveControl(NULL) {}  // line 23
    BOOL operator==(const UIControlItem & item) const {  // line 24
        return id == item.id;
    }
};
static_assert(sizeof(UIControlItem) == 0x18, "UIControlItem");

typedef blz::vector<UIID, blz::allocator<UIID> > UIIDArray;

typedef blz::vector<UIControlItem, blz::allocator<UIControlItem> > UIControlItemArray;

class UIControlGroup : public UIControl {  // size 0x88
public:
    UIControlGroup(const ControlCreationParams &);
    virtual ~UIControlGroup();  // vtable slot 0
    BOOL AddChild(const UIID & idControl);
    BOOL RemoveChild(const UIID & idControl);
    BOOL AddActiveChild(UIControl * ptChild);
    BOOL RemoveActiveChild(UIControl * ptChild);
    void RemoveAllChildren();
    BOOL InsertChild(const UIID & idControl, const int32 nIndex);
    virtual BOOL UpdateCachedActiveState();  // vtable slot 11
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual UIID HitTest(const IVector2D & pt);  // vtable slot 15
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting);  // vtable slot 16
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 89
        return eType == UIT_CONTROLGROUP || UIControl::IsType(eType);
    }
    virtual void PostCreate();  // vtable slot 18
    virtual const UIID & GetInitialKeyboardFocus() const;  // vtable slot 3
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual void RecalculateLayout(int32 nStartingChildIndex) {}  // vtable slot 24; line 113
    virtual void RecalculateLayoutForElement(const UIID & idStartingChildControl);  // vtable slot 25; inline, line 126
    UIControl * GetFirstActiveChild() const { return m_ptFirstActiveChild; }  // line 136
    UIControl * GetLastActiveChild() const;
    const UIControlItemArray & GetChildList() const { return m_listChildren; }  // line 149
    virtual BOOL ContainsUIElement(const UIID & idRecipient) const;  // vtable slot 23
    virtual void OnFinishAddingXmlChildren() {}  // vtable slot 26; line 161
    void SortChildren(PFN_COMPARE);
protected:
    virtual void OnChildAdded(const UIID & idChild) {}  // vtable slot 27; line 169
    virtual BOOL ForwardMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 28
    UIID HitTestChildren(const IVector2D & pt) const;
    UIID HitTestChildrenForTooltips(const IVector2D & pt, BOOL & bContinueTesting) const;
private:
    // UIWindow walks m_listChildren directly (no accessor is inlined into its functions).
    friend class UIWindow;
    UIControlItemArray m_listChildren;  // 0x60
    UIControl * m_ptFirstActiveChild;  // 0x78
    UIControl * m_ptLastActiveChild;  // 0x80
    void UpdateActiveLinkedList();
};
static_assert(sizeof(UIControlGroup) == 0x88, "UIControlGroup");

#include "../../../Sigma/BlzExt/BlzVectorHelper.h"

// line 126
inline void UIControlGroup::RecalculateLayoutForElement(const UIID & idStartingChildControl) {
    int32 i = blzext::vector_find(m_listChildren, UIControlItem(idStartingChildControl));
    if (i >= 0) {
        RecalculateLayout(i);
    }
}


