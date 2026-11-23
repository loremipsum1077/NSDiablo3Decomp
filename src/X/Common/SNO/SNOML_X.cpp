// Original file: X/Common/SNO/SNOML_X.cpp

#include <string.h>

#include "SNOML.h"
#include "SNOMLInternal.h"
#include "SNOFiles.h"
#include "SNOGroups.h"
#include "SNOTOC.h"
#include "../Definitions/Definitions.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/UnicodeText.h"
#include "../../../BlizzardCore/Include/Blizzard/File.h"

// Defined in X/Common/SNO/SNOML_Shared.cpp:25; SNOMLInternal.h carries no prototype.
extern SNOMLGlobals * sgpSNOMLGlobals;

// Defined in X/Common/SNO/SNOTOC.cpp:25; the adopted header carries no prototype.
extern SNOTOC gtTOC;

// Declared by X/Client/Definitions/PreferencesDefinition.h:499; that header is adopted but pulls in
// X/Client headers (GfxCommon, Input, Sound*Definition), so only this one prototype is repeated here
// rather than including it from an X/Common translation unit.
namespace Prefs { void InitializeForApp(); }

// Declared by X/Common/GlobalSNO.h in the original; the adopted header carries no prototype.
void GlobalSNOListRefresh();

// Declared by X/Common/SNO/SNOFiles.h in the original; the adopted header does not carry them yet.
SNOElement * SNOFileOpenForRead(const SNOName & snoname);
BOOL SNOFileDelete(const SNOName & snoname, const BOOL fKeepInFileMap);

void SNOMLInternal_Initialize()  // line 24
{
    gtTOC.ReadIntoFileTable();
    Prefs::InitializeForApp();
    GlobalSNOListRefresh();
}

void SNOMLInternal_Dispose()  // line 44
{
}

SNOMLResult SNOMLUpdate(const SNOName &, const void *, LPCSTR)  // line 67
{
    return SNOML_NOFILECREATED;
}

void SNOMLDelete(const SNOName & snoname)  // line 89
{
    SNOFileDelete(snoname, FALSE);
}

BOOL SNOTransferAdditionalFiles(const char *, const char *,
                                Blizzard::File::ProcessDirCallback)  // line 136
{
    return TRUE;
}

bool SNOMoveAdditionalFile(const Blizzard::File::ProcessDirParms &)  // line 146
{
    return true;
}

BOOL SNODeleteAdditionalFiles(const char * szPath)  // line 179
{
    return TRUE;
}

SNOGroup SNOMLGetSNOGroupTypeFromClassName(LPCSTR szClassName)  // line 298
{
    for (int32 i = 0; ; i++)
    {
        GroupDefinition * pGroupDef = sgpSNOMLGlobals->ptGameDefinitions[i];
        if (!pGroupDef)
            return SNOGROUP_NONE;

        if (strcmp(szClassName, pGroupDef->GetName()) == 0)  // line 310
            return pGroupDef->GetGroup();  // line 312
    }
}

SNOElement * SNOMLFileOpenForRead(const SNOName & snoname)  // line 344
{
    return SNOFileOpenForRead(snoname);
}

uint32 SNOMLGetAssetVersion(const SNOName & snoname, BOOL fForceUpdate)  // line 363
{
    return ASSET_VERSION_NONE;
}

CRefString GetBlameError(const SNOName &)  // line 385
{
    return CRefString("");
}

XLocale SNOMLFileGetLocale(const SNOName &)  // line 452
{
    return XLOCALE_INVALID;
}
