// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/TypeDescriptor/SymbolicValue.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"

struct SymbolicEntry {  // size 0x10
    int32 nValue;  // 0x00
    const char * sName;  // 0x08
};
static_assert(sizeof(SymbolicEntry) == 0x10, "SymbolicEntry");


const char * SymbolToName(int32 nSymbol, const SymbolicEntry * pTable);
int32 NameToSymbol(const char * sName, const SymbolicEntry * pTable);
BOOL GetSymbolicMinMax(int32 & nMin, int32 & nMax, const SymbolicEntry * pTable);
