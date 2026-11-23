// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/UI/Controls/ControlVisibilityManager.h

#pragma once

#include "../../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Sigma/SigmaTypes.h"
#include "../../../UserInterface/UIID.h"

class ControlVisibilityManager {  // size 0x28
    // vtable pointer
public:
    ControlVisibilityManager();
    virtual ~ControlVisibilityManager() {}  // vtable slot 0, line 19
    int32 AddControl(const UIID & id);
    virtual void SetActive(int32 nControlIndex, bool bForceRefresh);  // vtable slot 2
    void RemoveAll();
    int32 GetSelectedIndex() const;
    BOOL GetSelectedControl(UIID & idControl);
private:
    blz::vector<UIID, blz::allocator<UIID> > m_aridMangedControls;  // 0x08
    int32 m_nActiveControl;  // 0x20
};
static_assert(sizeof(ControlVisibilityManager) == 0x28, "ControlVisibilityManager");
