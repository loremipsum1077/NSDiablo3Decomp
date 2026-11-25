#pragma once
#include "DataArraysTemplated.h"

// Separate base and tracker recovered for the 18-bit definition-array family.
struct DefinitionDataArrayBase {
    CHAR szName[MAXIMUM_DATA_ARRAY_NAME_LENGTH];
    uint32 dwMaximumCount;
    uint32 dwElementSize;
    uint32 unk_108;
    uint32 dwTotalCount;
    uint32 unk_110;
    uint32 dwFreeCount;
    BOOL fGrowable;
    uint32 GetTotalCount() const { return dwTotalCount; }
};
typedef XGrowableList<DefinitionDataArrayBase*, DefinitionDataArrayBase*> DefinitionDataArrayList;
extern DefinitionDataArrayList* unk_7101a46c30;
static_assert(sizeof(DefinitionDataArrayBase) == 0x11c, "18-bit array base");
