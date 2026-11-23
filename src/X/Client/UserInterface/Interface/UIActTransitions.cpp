#include "../Controls/UITextControl.h"
#include "../UIID.h"
#include "../../CGame.h"
#include "../../CActTransitions.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Client/UserInterface/UI.cpp
BOOL UI_TestID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
// X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIWND_ACTTRANSITIONS;
extern UIID UITEXT_ACTTRANSITIONS_TITLE;
extern UIID UITEXT_ACTTRANSITIONS_TIME;

void UIActTransitions_Update()
{
    if (UI_TestID(UIWND_ACTTRANSITIONS) &&
        UI_TestID(UITEXT_ACTTRANSITIONS_TITLE) &&
        UI_TestID(UITEXT_ACTTRANSITIONS_TIME))
    {
        UIText & tTextTitle = UI_GetTextFromID(UITEXT_ACTTRANSITIONS_TITLE);
        UIText & tTextTime = UI_GetTextFromID(UITEXT_ACTTRANSITIONS_TIME);

        uint32 dwNow = CGameGetPredictedServerTime();
        uint32 dwActTransitionTime = CGameGet().ptCActTransitionGlobals->dwActTransitionTime;

        if (dwActTransitionTime > dwNow && dwActTransitionTime != 0xFFFFFFFF)
        {
            CRefString sTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "ActTransitionTitle");
            tTextTitle.SetText(sTitle, FALSE);

            int32 nRemainingSeconds = (dwActTransitionTime - dwNow) / 60;
            tTextTime.SetText(UIText_GetTimeString(nRemainingSeconds), FALSE);
        }
    }
}
