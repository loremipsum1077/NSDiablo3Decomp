// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/UIConsoleRiftEntry.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIConsoleInventoryCategoryView.h"

class UIConsoleListBox;
struct UIControlSourceKey;
struct UIID;

namespace UIConsoleRiftEntry {
class RiftSelectionDataSource : public UIInventory::Console::CategoryViewDataSource {  // size 0x28
public:
    RiftSelectionDataSource(UIInventory::Console::CategoryItemUIUpdateCallback);
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
};
static_assert(sizeof(RiftSelectionDataSource) == 0x28, "RiftSelectionDataSource");

class GreaterRiftLevelSelectionDataSource : public UIInventory::Console::CategoryViewDataSource {  // size 0x28
public:
    GreaterRiftLevelSelectionDataSource(UIInventory::Console::CategoryItemUIUpdateCallback);
    virtual int32 GetNumItems() const;  // vtable slot 2
    virtual UIControlSourceKey GetSourceKeyForItem(const int32 nIndex) const;  // vtable slot 3
    virtual void InitializeItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 4
    virtual void UpdateItemUI(const UIConsoleListBox & tListBox, const UIID & idItemUI, const int32 nIndex);  // vtable slot 5
};
static_assert(sizeof(GreaterRiftLevelSelectionDataSource) == 0x28, "GreaterRiftLevelSelectionDataSource");

}  // namespace UIConsoleRiftEntry

// Functions defined in this file (bodies are not in the DWARF):
//   line    23  UIConsoleRiftEntry::RiftSelectionDataSource::~RiftSelectionDataSource()  [0x710016d3a0]
//   line    49  UIConsoleRiftEntry::GreaterRiftLevelSelectionDataSource::~GreaterRiftLevelSelectionDataSource()  [0x710016d420]
