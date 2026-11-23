#include "ControlVisibilityManager.h"
#include "../../../UserInterface/UI.h"
#include "../../../UserInterface/Controls/UIControlControl.h"
#include "../../../../Sigma/Math/MathGeneral.h"

// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);

ControlVisibilityManager::ControlVisibilityManager()
    : m_nActiveControl(-1)
{
}

int32 ControlVisibilityManager::AddControl(const UIID & id)
{
    m_aridMangedControls.push_back(id);
    int32 nControlIndex = (int32)m_aridMangedControls.size() - 1;
    return nControlIndex;
}

void ControlVisibilityManager::SetActive(int32 nControlIndex, bool bForceRefresh)
{
    UIID idActiveControl;

    if (m_aridMangedControls.size() == 0)
        return;

    nControlIndex = Math::Pin(nControlIndex, -1, (int32)m_aridMangedControls.size() - 1);

    if (m_nActiveControl == -1)
    {
        for (uint32 i = 0; i < m_aridMangedControls.size(); i++)
        {
            const UIID idControl = m_aridMangedControls[i];
            if (UI_TestID(idControl))
                UI_GetControlFromID(idControl).SetActive(FALSE);
        }
    }

    if (m_nActiveControl == nControlIndex && !bForceRefresh)
        return;

    if (m_nActiveControl != -1)
    {
        idActiveControl = m_aridMangedControls[m_nActiveControl];
        if (UI_TestID(idActiveControl))
            UI_GetControlFromID(idActiveControl).SetActive(FALSE);
    }

    m_nActiveControl = nControlIndex;
    if (nControlIndex != -1)
    {
        idActiveControl = m_aridMangedControls[nControlIndex];
        if (UI_TestID(idActiveControl))
            UI_GetControlFromID(idActiveControl).SetActive(TRUE);
    }
}

BOOL ControlVisibilityManager::GetSelectedControl(UIID & idControl)
{
    if (m_nActiveControl != -1 && m_nActiveControl < (int32)m_aridMangedControls.size())
    {
        idControl = m_aridMangedControls[m_nActiveControl];
        return TRUE;
    }
    return FALSE;
}
