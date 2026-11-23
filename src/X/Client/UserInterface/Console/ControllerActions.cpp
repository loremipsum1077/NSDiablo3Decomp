// Original file: X/Client/UserInterface/Console/ControllerActions.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "ControllerActions.h"
#include "ConsoleScreenManager.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../../Sigma/UnicodeText.h"
// Declared here because its header is not adopted yet (X/Common/SNO/SymbolicValue.cpp:1292).
extern const SymbolicEntry gControllerInputIcon[];





namespace Console {



ControllerActionManager::ControllerActionManager()
    : m_bActionsDirtyFlag(FALSE)
{
}




void ControllerActionManager::SetAction(const ControllerInput eInput, const ControllerActionId idAction, const LPCSTR szText)
{



    m_tActions[eInput].id = idAction;

    const Blizzard::Unicode::GetUTF8FromCodepoint tNonBreakingSpace(0xa0);
    m_tActions[eInput].sText = StringListEntryReplace(szText, " ", tNonBreakingSpace, 999);
}





void ControllerActionManager::SetDelayedAction(const ControllerInput eInput, const ControllerActionId idAction, const LPCSTR szText, uint32 nDelay)
{




    ControllerAction & tAction = m_tActions[eInput]; tAction.id = idAction;


    const Blizzard::Unicode::GetUTF8FromCodepoint tNonBreakingSpace(0xa0);
    tAction.sTextNoDelay = StringListEntryReplace(szText, " ", tNonBreakingSpace, 999);
    tAction.sText = tAction.sTextNoDelay;
    tAction.nDelayUntil = Blizzard::Time::Milliseconds() + nDelay * 1000;
}




void ControllerActionManager::ClearAction(const ControllerInput eInput)
{

    m_tActions[eInput].id = ACTION_NONE;
    m_tActions[eInput].sText = "";
}




void ControllerActionManager::ClearAllActions()
{
    for (int32 i = 0; i < CONSOLE_INPUT_COUNT; i++)
    {
        ClearAction(i);
    }
}





ControllerActionId ControllerActionManager::GetActionForInput(const ControllerInput eInput) const
{
    if (m_bActionsDirtyFlag)
        return ACTION_NONE;

    return m_tActions[eInput].id;
}








CRefString ControllerActionManager::ToString(const int32 nSpaceBetweenLabels) const
{

    CRefString sResult;



    CHAR szSpaces[10];
    SigmaMemoryFill(szSpaces, ' ', sizeof(szSpaces));
    szSpaces[sizeof(szSpaces) - 1] = '\0';
    if (nSpaceBetweenLabels < (int32)sizeof(szSpaces))
    {
        szSpaces[nSpaceBetweenLabels] = '\0';
    }

    for (int32 i = 0; i < CONSOLE_INPUT_COUNT; i++)
    {
        if (m_tActions[i].id != ACTION_NONE)
        {





            if (sResult.Length() != 0)
            {
                sResult += szSpaces;
            }
            sResult += InputToString(i);

            const ControllerAction & tAction = m_tActions[i];
            int32 j;
            for (j = i + 1; j < CONSOLE_INPUT_COUNT; j++)
            {
                if (m_tActions[j].id == ACTION_NONE || m_tActions[j].sText != tAction.sText)
                {
                    break;
                }


                sResult += InputToString(j);
            }
            i = j - 1;


            const Blizzard::Unicode::GetUTF8FromCodepoint tNonBreakingSpace(0xa0);
            sResult += tNonBreakingSpace;

            CRefString sText = tAction.sText;

            if (tAction.nDelayUntil > 0)
            {
                Set_Text_Color(sText, crgbaDisabled);
            }

            sResult += sText;
        }
    }

    return sResult;
}



int32 ControllerActionManager::GetActiveActionCount() const
{
    int32 nCount = 0;




    for (int32 i = 0; i < CONSOLE_INPUT_COUNT; i++) { if (m_tActions[i].id != ACTION_NONE) nCount++; }

    return nCount;
}








CRefString ControllerActionManager::InputToString(const ControllerInput eInput)
{

    return SymbolToName(eInput, gControllerInputIcon);
}




BOOL ControllerActionManager::AreActionsDirty() const
{

    return m_bActionsDirtyFlag;
}




void ControllerActionManager::SetActionsDirtyFlag(const BOOL bIsDirty)
{
    m_bActionsDirtyFlag = bIsDirty;
}


void ControllerActionManager::OnUpdate()
{

    const Blizzard::Time::Millisecond nNow = Blizzard::Time::Milliseconds();

    for (int32 i = 0; i < CONSOLE_INPUT_COUNT; i++)
    {
        ControllerAction & tAction = m_tActions[i];
        if (tAction.nDelayUntil > 0)
        {
            if ((int64)tAction.nDelayUntil > (int64)nNow)
            {
                CRefString sSeconds = UnicodeText::FormatNumber((int32)(tAction.nDelayUntil - nNow) / 1000 + 1, TRUE, FNS_NORMAL, 0);
                tAction.sText.Format("%s (%s)", tAction.sTextNoDelay.str(), sSeconds.str());
                Screens::UpdateButtonLegendText();
            }
            else
            {
                tAction.nDelayUntil = 0;
                tAction.sText.Format("%s", tAction.sTextNoDelay.str());
                Screens::UpdateButtonLegendText();
            }
        }
    }
}
BOOL ControllerActionManager::IsActionDelayed(const ControllerInput eInput) const
{

    return m_tActions[eInput].nDelayUntil > 0;
}

}  // namespace Console
