// X/Common/UserInterface/UIHirelingCommon.cpp
//
// Shared hireling-class helpers used by the client UI: a class id maps to a
// string-list label, and that label resolves to the localized class name.
//
// The label has an article form ("the Templar") and a bare form ("Templar").

#include "UIHirelingCommon.h"

#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../Definitions/StringListDefinition.h"
#include "../GlobalSNO.h"

namespace UIHireling {

CRefString GetNameForHirelingClass(const HirelingClass eClass, const BOOL fWithArticle);
CRefString GetLabelForHirelingClass(const HirelingClass eClass, const BOOL fWithArticle);

// GetNameForHirelingClass looks the localized class name up in the "Followers"
// string list using the label GetLabelForHirelingClass builds.  Class ids are
// the values stored on the follower; anything else is a data bug.
CRefString GetNameForHirelingClass(const HirelingClass eClass, const BOOL fWithArticle)
{
    CRefString sLabel = GetLabelForHirelingClass(eClass, fWithArticle);

    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_PETS), sLabel.str());
}

// The article form reads as "the Templar"; the bare form is just "Templar".
// The string-list key is the label, not the localized text.


CRefString GetLabelForHirelingClass(const HirelingClass eClass, const BOOL fWithArticle)
{
    CRefString sLabel;

    switch (eClass)
    {
    case 1:
        sLabel = fWithArticle ? "HirelingTemplarArticle" : "HirelingTemplar";
        break;

    case 2:
        sLabel = fWithArticle ? "HirelingScoundrelArticle" : "HirelingScoundrel";
        break;

    case 3:
        sLabel = fWithArticle ? "HirelingEnchantressArticle" : "HirelingEnchantress";
        break;

    default:
        DisplayInternalError(NULL, "", 52, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return sLabel;
}

}  // namespace UIHireling
