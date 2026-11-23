// Original file: X/Common/Definitions/FlagSetDefinition.cpp

#include "FlagSetDefinition.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"

#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <strings.h>

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

enum {
    FLAGSET_SNO_VERSION = 19,
};

// _GLOBAL__sub_I_FlagSetDefinition.cpp also runs the header statics of the includes above (cgplaceNull,
// the four RGBAColor normals of X/Client/Gfx/GfxVB.h:351-354).
// The field names are "" in the binary.
//
//
static const TypeDescriptorField FlagLookup_Fields[4] = {  // line 23
    TypeDescriptorField("", DT_INT, offsetof(FlagLookup, nIndex), NULL, 1),
    CharArrayTypeDescriptorField("", offsetof(FlagLookup, szLabel), "", 1, FLAGSET_MAX_LABEL, -1),
    CharArrayTypeDescriptorField("", offsetof(FlagLookup, szDescription), "", 1, FLAGSET_MAX_DESCRIPTION, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FlagLookup), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FlagLookup("FlagLookup", 0, FlagLookup_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 27
const XTypeDescriptor * DT_FLAGLOOKUP = &TypeDescriptor_FlagLookup;

static const TypeDescriptorField FlagSetDefinition_Fields[3] = {  // line 29
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(FlagSetDefinition, serFlagLookups), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_FLAGLOOKUP, offsetof(FlagSetDefinition, arFlagLookups), 0x21, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FlagSetDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FlagSetDefinition("FlagSet", 0, FlagSetDefinition_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 31
const XTypeDescriptor * DT_FLAGSETDEFINITION = &TypeDescriptor_FlagSetDefinition;

FlagSetGroupDef * gptGroupDefFlagSet;

FlagSetGroupDef::FlagSetGroupDef()
    : GroupDefinition("FlagSet", FLAGSET_SNO, SNOVersions(FLAGSET_SNO_VERSION), SNODATATYPE_TITLE, sizeof(FlagSetDefinition),
                      DT_FLAGSETDEFINITION, 0x5005, 14) {
}

uint32 FlagSetGroupDef::GetMaxCount() {
    return 8;
}

BOOL FlagSetGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 102, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Not in the binary as a function (inlined into FlagSetLookup only), so static or inline.
static int32 FlagSetFind(const FlagLookup * arFlagLookups, int32 nArraySize, LPCSTR szFlagLabel) {
    int32 nFirst = 0;
    int32 nIndex = -1;
    int32 nLast = nArraySize - 1;

    while (nFirst <= nLast) {
        int32 nMid = (nFirst + nLast) >> 1;

        int32 nCmp = strcasecmp(szFlagLabel, arFlagLookups[nMid].szLabel);

        if (nCmp > 0) {
            nFirst = nMid + 1;
        } else if (nCmp < 0) {
            nLast = nMid - 1;
        } else {
            nIndex = arFlagLookups[nMid].nIndex;
            break;
        }
    }

    return nIndex;
}

int32 FlagSetLookup(SNO snoFlagSet, LPCSTR szFlagLabel) {
    int32 nIndex = -1;

    const FlagSetDefinition * ptDef = (const FlagSetDefinition *)gptGroupDefFlagSet->LockRFast(snoFlagSet);

    if (ptDef) {
        nIndex = FlagSetFind(ptDef->arFlagLookups, ptDef->serFlagLookups.dwSize / sizeof(FlagLookup), szFlagLabel);
    }

    return nIndex;
}

BOOL FlagSetGetFlag(const uint8 * arFlags, const int32 nBitArraySize, const int32 nFlagIndex) {
    if (nFlagIndex != -1) {
        int32 nByteIndex = nFlagIndex >> 3;
        int32 nBitIndex = nFlagIndex & 7;

        return (arFlags[nByteIndex] >> nBitIndex) & 1;
    }

    return FALSE;
}

void FlagSetSetFlag(uint8 * arFlags, const int32 nBitArraySize, const int32 nFlagIndex, const BOOL bValue) {
    if (nFlagIndex != -1) {
        int32 nByteIndex = nFlagIndex >> 3;
        int32 nBitIndex = nFlagIndex & 7;

        if (bValue)
            arFlags[nByteIndex] |= (1 << nBitIndex);
        else
            arFlags[nByteIndex] &= ~(1 << nBitIndex);
    }
}
