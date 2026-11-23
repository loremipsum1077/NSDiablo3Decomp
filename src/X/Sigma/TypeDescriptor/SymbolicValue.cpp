// Original file: X/Sigma/TypeDescriptor/SymbolicValue.cpp

#include "SymbolicValue.h"

#include <strings.h>

#include "../Math/MathGeneral.h"
#include "../RefString.h"
#include "../Unicode.h"

// The file's two example symbol tables (original lines 21 and 28):
//   const SymbolicEntry gBoolNames[3]
//   const SymbolicEntry gTernaryNames[4]
// Neither survives in the binary (the linker dropped them); left out rather
// than guessed.

BOOL SymbolIsValid(int32 nSymbol, const SymbolicEntry * pTable)
{
    if (pTable == NULL)
        return FALSE;

    for (int32 i = 0; pTable[i].sName != NULL; ++i) {
        if (pTable[i].nValue == nSymbol)
            return TRUE;
    }

    return FALSE;
}

const char * SymbolToName(int32 nSymbol, const SymbolicEntry * pTable)
{
    if (pTable == NULL)
        return "[bad symbol table]";

    if (pTable->sName == NULL)
        return "[symbol not found]";

    for (int32 i = 0; pTable[i].sName != NULL; ++i) {
        if (pTable[i].nValue == nSymbol)
            return pTable[i].sName;
    }

    return "[symbol not found]";
}

int32 NameToSymbol(const char * sName, const SymbolicEntry * pTable)
{
    if (pTable != NULL) {
        for (int32 i = 0; pTable[i].sName != NULL; ++i) {
            if (strcasecmp(pTable[i].sName, sName) == 0)
                return pTable[i].nValue;
        }
    }

    return -1;
}

uint32 StringHash(LPCSTR pszName)
{
    return XHashKey<LPCSTR>(pszName);
}

uint32 StringHashI(LPCSTR pszName)
{
    uint32 nHash = 0;

    while (*pszName != '\0') {
        char cChar = *pszName++;

        if (cChar >= 'A' && cChar <= 'Z')
            cChar += 'a' - 'A';

        nHash = nHash * 33 + cChar;
    }

    return nHash;
}

BOOL GetSymbolicMinMax(int32 & nMin, int32 & nMax, const SymbolicEntry * pTable)
{
    if (pTable == NULL)
        return FALSE;

    nMin = 0x7fffffff;
    nMax = (int32)0x80000000;

    for (int32 i = 0; pTable[i].sName != NULL; ++i) {
        nMin = Math::Min(nMin, pTable[i].nValue);
        nMax = Math::Max(nMax, pTable[i].nValue);
    }

    return nMin <= nMax;
}
