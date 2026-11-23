// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UITreeControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIButtonControl.h"
#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class TreeCreationParams : public WindowCreationParams {  // size 0x730
public:
    TreeCreationParams(const UIType, const UIID &, const UIID &);
    TreeCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
private:
    void sInit() {}  // body not recorded; nothing of it remains in the constructor
};
static_assert(sizeof(TreeCreationParams) == 0x730, "TreeCreationParams");

class TreeFolderCreationParams : public ButtonCreationParams {  // size 0x8a8
public:
    TreeFolderCreationParams(const UIType, const UIID &, const UIID &);
    TreeFolderCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIControlSourceKey tTemplateKey;  // 0x890
    float flFolderIndentUIC;  // 0x8a0
    float flFolderSpacingAfterCheckbox;  // 0x8a4
private:
    void sInit();
};
static_assert(sizeof(TreeFolderCreationParams) == 0x8a8, "TreeFolderCreationParams");

class UITree : public UIWindow {  // size 0x1e0
public:
    UITree(const TreeCreationParams &, const Vector2D &, const Vector2D &);
    UITree(const TreeCreationParams &, const Rect2D *);
    virtual ~UITree();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 152
        return eType == UIT_TREE || UIWindow::IsType(eType);
    }
    virtual void RecalculateLayout(int32 nStartingChildIndex);  // vtable slot 24
    void AdjustScrollbar();
private:
    void sInit(const TreeCreationParams & tParams);
    int32 m_nFolder;  // 0x1cc
    UIID m_idStackPanel;  // 0x1d0
    UIID m_idScrollbar;  // 0x1d8
};
static_assert(sizeof(UITree) == 0x1e0, "UITree");

class UITreeFolder : public UIWindow {  // size 0x1e8
public:
    UITreeFolder(const TreeFolderCreationParams &, const Vector2D &, const Vector2D &);
    UITreeFolder(const TreeFolderCreationParams &, const Rect2D *);
    virtual ~UITreeFolder();  // vtable slot 0
    virtual void PostCreate();  // vtable slot 18
    virtual void RecalculateLayout(int32);  // vtable slot 24
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 76
        return eType == UIT_TREEFOLDER || UIWindow::IsType(eType);
    }
    static void OnCheckboxClick(const UIID & id);
    const UIID & GetCheckbox() const { return m_idCheckbox; }  // line 95
    const UIID & GetStackPanel() const { return m_idStackPanel; }  // line 101
private:
    void sInit(const TreeFolderCreationParams & tParams);
    float m_flFolderIndentUIC;  // 0x1cc
    float m_flFolderSpacingAfterCheckbox;  // 0x1d0
    UIID m_idCheckbox;  // 0x1d8
    UIID m_idStackPanel;  // 0x1e0
};
static_assert(sizeof(UITreeFolder) == 0x1e8, "UITreeFolder");
