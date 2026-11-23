// Original file: X/Common/SNO/SNOGroups.cpp

#include "SNOGroups.h"

#include <stdlib.h>
#include <strings.h>
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"

struct SNOGroupGlobals {  // size 0x15f0
    int32 nGroupCount;  // 0x00
    SNOGroupInfo tGroupList[70];  // 0x08
    int32 nDTCount;  // 0x13b8
    const XTypeDescriptor * arBaseDataTypes[70];  // 0x13c0
};
static_assert(sizeof(SNOGroupGlobals) == 0x15f0, "SNOGroupGlobals");

static SNOGroupGlobals * sgptSNOGroupGlobals;  // line 35

static int32 SNOGroupComparator(const void * a, const void * b)
{
    const SNOGroupInfo * groupA = (const SNOGroupInfo *)a;
    const SNOGroupInfo * groupB = (const SNOGroupInfo *)b;

    return groupA->snoGroup - groupB->snoGroup;
}

// The DWARF does not say which header declared the SNOGroup* functions (SNOGroups.h has none in
// the reference); declared here so SNOGroupRegister can call SNOGroupGetInfo (line 124).
const SNOGroupInfo * SNOGroupGetInfo(const SNOGroup snoGroup);

void SNOGroupsInitialize()
{
    sgptSNOGroupGlobals = (SNOGroupGlobals *)SigmaMemoryNew(sizeof(SNOGroupGlobals), 4, NULL, TRUE);
}

void SNOGroupsDispose()
{
    SigmaMemoryFree(sgptSNOGroupGlobals, NULL);
    sgptSNOGroupGlobals = NULL;
}

void SNOGroupRegister(SNOGroupInfo & snogroup, const XTypeDescriptor * dt)
{
    if (SNOGroupGetInfo(snogroup.snoGroup))
        return;

    SigmaMemoryCopy(&sgptSNOGroupGlobals->tGroupList[sgptSNOGroupGlobals->nGroupCount], &snogroup, sizeof(SNOGroupInfo));
    sgptSNOGroupGlobals->nGroupCount++;

    qsort(sgptSNOGroupGlobals->tGroupList, sgptSNOGroupGlobals->nGroupCount, sizeof(SNOGroupInfo), SNOGroupComparator);

    if (dt != DT_NULL)
    {
        sgptSNOGroupGlobals->arBaseDataTypes[sgptSNOGroupGlobals->nDTCount] = dt;
        sgptSNOGroupGlobals->nDTCount++;
    }
}

int32 SNOGroupGetGroups(SNOGroup * pGroups)
{
    for (int32 i = 0; i < sgptSNOGroupGlobals->nGroupCount; i++)
        pGroups[i] = sgptSNOGroupGlobals->tGroupList[i].snoGroup;

    return sgptSNOGroupGlobals->nGroupCount;
}

const SNOGroupInfo * SNOGroupGetInfo(const SNOGroup snoGroup)
{
    SNOGroupInfo tKey;
    tKey.snoGroup = snoGroup;

    return (const SNOGroupInfo *)bsearch(&tKey, sgptSNOGroupGlobals->tGroupList,
                                         sgptSNOGroupGlobals->nGroupCount, sizeof(SNOGroupInfo),
                                         SNOGroupComparator);
}

LPCSTR SNOGroupGetName(const SNOGroup snoGroup)
{
    const SNOGroupInfo * ptInfo = SNOGroupGetInfo(snoGroup);
    if (ptInfo)
        return ptInfo->szName;

    return "SNOGROUP_NONE";
}

SNOGroup SNOGroupGetByName(LPCSTR szName)
{
    for (int32 i = 0; i < sgptSNOGroupGlobals->nGroupCount; i++)
    {
        if (!strcasecmp(sgptSNOGroupGlobals->tGroupList[i].szName, szName))
            return sgptSNOGroupGlobals->tGroupList[i].snoGroup;
    }

    return SNOGROUP_NONE;
}

const XTypeDescriptor ** GameXMLDefinitionsGet()
{
    return sgptSNOGroupGlobals->arBaseDataTypes;
}
