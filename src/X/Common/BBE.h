// Adopted from the DWARF; callable declarations recovered from BBE.cpp.
// Original file: X/Common/BBE.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "GameBalance.h"
#include "../Sigma/Unicode.h"

struct RNG;
class CRefString;

struct BBETranslationData {  // size 0x8
    GameBalanceType eGBType;  // 0x00
    GBID gbid;  // 0x04
};
static_assert(sizeof(BBETranslationData) == 0x8, "BBETranslationData");

typedef BOOL (*IdentifierEvaluateFunc)(int32, int32, int32, int32, void *, float &);

typedef float (*FunctionEvaluateFunc)(float, float, float, RNG *, void *);

struct TFunctionEvaluateTable {  // size 0x110
    CHAR szFunctionName[256];  // 0x00
    FunctionEvaluateFunc pfnEvalFunc;  // 0x100
    int32 nOperands;  // 0x108
};
static_assert(sizeof(TFunctionEvaluateTable) == 0x110, "TFunctionEvaluateTable");


// Variables defined in this file:
//   line    97  static const int32 MAX_BBE_SIZE

BOOL BBEToString(uint8 * pbBuf, intptr nLen, const TFunctionEvaluateTable * ptFuncEvalTable, int32 nFuncEvalTable, CRefString & sResult);
void BBEByteswap(uint8 * pbBuf, int32 nLen);
int32 BBEFromString(const char * txt, uint8 * pBBE, int32 nMaxLen, const BBETranslationData & bbeTranslate);
extern const TFunctionEvaluateTable BBEFunctions[13];
extern const int32 NUM_BBEFUNCTIONS;
BOOL BBEEvaluate(uint8 * pbBuf, intptr nLen, IdentifierEvaluateFunc pfIdEvalFunc, const TFunctionEvaluateTable * ptFuncEvalTable, int32 nFuncEvalTable, RNG * ptRNG, void * callbackData, float & flResult);  // BBE.cpp:385
