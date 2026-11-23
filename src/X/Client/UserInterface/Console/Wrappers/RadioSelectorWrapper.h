// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/Wrappers/RadioSelectorWrapper.h

#pragma once

#include "../../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Controls/UIControlGroup.h"
#include "../../UIID.h"
#include "../../../../Sigma/SigmaTypes.h"

namespace Console {
class RadioSelectorWrapper {  // size 0x38
public:
    typedef BOOL (*SelectionPredicate)(const int32);
    typedef void (*SelectionChanged)(const UIID &, const int32, const BOOL, const BOOL);
public:
    RadioSelectorWrapper();
    void WrapUsingContainerControl(const UIID & idContainer);
    const UIID & GetContainerControlID() const;
    const UIIDArray & GetOptionControlIDs() const;
    void SetCallbacks(Console::RadioSelectorWrapper::SelectionPredicate fnSelectionPredicate, Console::RadioSelectorWrapper::SelectionChanged fnSelectionChanged);
    int32 GetNumberOfOptions() const;
    int32 GetSelectedIndex() const;
    BOOL SetSelectedIndex(const int32 nIndex, const BOOL bTriggeredByInput);
    BOOL SelectNext(const BOOL bTriggeredByInput);
    BOOL SelectPrevious(const BOOL bTriggeredByInput);
    const UIID & GetUIIDAtIndex(const int32 nIndex) const;
private:
    UIID m_idContainer;  // 0x00
    UIIDArray m_aridOptions;  // 0x08
    int32 m_nSelectedIndex;  // 0x20
    Console::RadioSelectorWrapper::SelectionPredicate m_fnSelectionPredicate;  // 0x28
    Console::RadioSelectorWrapper::SelectionChanged m_fnSelectionChanged;  // 0x30
    void Reset();
    void UpdateOptions();
};
static_assert(sizeof(RadioSelectorWrapper) == 0x38, "RadioSelectorWrapper");

}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    19  Console::RadioSelectorWrapper::~RadioSelectorWrapper()  [inlined into 4 functions]
