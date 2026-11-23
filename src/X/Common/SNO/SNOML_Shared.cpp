// Original file: X/Common/SNO/SNOML_Shared.cpp

#include <string.h>

#include "SNOGroups.h"
#include "SNOMLInternal.h"
#include "../Definitions/Definitions.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"

// Declared by X/Common/SNO/SNOGroups.h in the original; the adopted header carries no
// prototypes (the DWARF records only bodies), so they are declared here.
LPCSTR SNOGroupGetName(const SNOGroup snoGroup);
const XTypeDescriptor ** GameXMLDefinitionsGet();

// Defined in X/Common/SNO/SNOML_X.cpp; no adopted header records the prototypes.
void SNOMLInternal_Initialize();
void SNOMLInternal_Dispose();

// Declared by X/Common/SNO/SNOFiles.h in the original (body in SNOFiles.cpp:7572); the adopted
// header does not carry it yet.
CRefString SNOFilesGetLocalePath(const XLocale eXLocale);

SNOMLGlobals * sgpSNOMLGlobals;  // line 17

// Line 22. File-local helper, inlined into SNOMLMakeXMLPath (no out-of-line copy in the
// original: the symbol is not in functions.csv).
static CRefString SNOMLGetXMLPath(const XLocale eXLocale)
{
    CRefString sPath;
    return sPath.Format("%s\\XML", SNOFilesGetLocalePath(eXLocale).str());
}

// Line 31.
void SNOMLMakeXMLPath(const XLocale eXLocale, SNOGroup snoGroup, LPCSTR pszFileName, CRefString & sPath)
{
    LPCSTR pszGroupName = SNOGroupGetName(snoGroup);
    CRefString sXMLPath = SNOMLGetXMLPath(eXLocale);
    sPath.Format("%s\\%s\\%s.xml", sXMLPath.str(), pszGroupName, pszFileName);
}

// Line 61. File-local helper, inlined into SNOMLGetTypeFromSNOGroup (no out-of-line copy in the
// original: the symbol is not in functions.csv).
static const XTypeDescriptor * SNOMLGetDataTypeFromClassName(LPCSTR szClassName)
{
    for (int32 i = 0; sgpSNOMLGlobals->ptXMLGameDefinitions[i]; i++)
    {
        const XTypeDescriptor * ptTypeDesc = sgpSNOMLGlobals->ptXMLGameDefinitions[i];
        if (strcmp(szClassName, ptTypeDesc->GetName()) == 0)
            return ptTypeDesc;
    }
    return DT_NULL;
}

// Line 85.
const XTypeDescriptor * SNOMLGetTypeFromSNOGroup(SNOGroup snoGroup)
{
    for (int32 i = 0; ; i++)
    {
        GroupDefinition * pGroupDef = sgpSNOMLGlobals->ptGameDefinitions[i];
        if (!pGroupDef)
            return DT_NULL;
        if (pGroupDef->GetGroup() == snoGroup)
            return SNOMLGetDataTypeFromClassName(pGroupDef->GetName());
    }
}

// Line 109.
void SNOMLInitialize(GroupDefinition ** ptGameDefinitions)
{
    sgpSNOMLGlobals = (SNOMLGlobals *)SigmaMemoryNew(sizeof(SNOMLGlobals), 4, NULL, 0);
    sgpSNOMLGlobals->ptGameDefinitions = ptGameDefinitions;
    sgpSNOMLGlobals->ptXMLGameDefinitions = GameXMLDefinitionsGet();
    SNOMLInternal_Initialize();
}

// Line 130.
void SNOMLDispose()
{
    SNOMLInternal_Dispose();
    SigmaMemoryFree(sgpSNOMLGlobals, NULL);
    sgpSNOMLGlobals = NULL;
}
