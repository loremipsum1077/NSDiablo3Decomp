// Reconstructed from the 2.6.2 DWARF and original instruction evidence.
#include "FastAttribute.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/RefString.h"
#include <string.h>
#include "AttributeCompressor.h"
// Header-level objects present in this translation unit's original startup.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
static const FastAttribValue ATTRIB_VALUE_NONE(-1); // Original line 47.
static const int32 PARAM_NONE = -1;  // Original line 45.
struct AttribParamLimitDefinition {  // size 0x8
    int32 nMinValue;  // 0x00
    int32 nUpperLimit;  // 0x04
};
static const int32 ATTRIB_PARAM_LIMIT_INVALID = -1;  // Original line 69.
static const AttribParamLimitDefinition cgarAttribParamLimits[22] = {  // Original line 70.
    { 0, 7 }, { -1, -1 }, { 0, 7 }, { -1, -1 }, { -1, -1 }, { 0, 13 }, { 1, 71 }, { -1, -1 },
    { -1, -1 }, { 0, 78 }, { 0, 9 }, { -1, -1 }, { 0, 4 }, { 0, 5 }, { -1, -1 }, { -1, -1 },
    { -1, -1 }, { -1, -1 }, { 0, 13 }, { -1, 26 }, { -1, -1 }, { 0, 5 },
};
#include "FastAttributeDefs.inc"
#include "../SNO/SNOFiles.h"
#include "../SNO/SymbolicValue.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
// The original calls libc atoi, rather than an SDK inline strtol wrapper.
extern "C" int atoi(const char *);
#include "../BBEUtil.h"
#include "../BBE.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XPooledMap.h"
#include <stddef.h>
#include "../Game.h"
#include "../GameCommonData.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"

FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam) {
    FastAttribKey tRetval;
    tRetval.nValue = eAttrib | (nParam << 12);
    return tRetval;
}

Attrib KeyGetAttrib(FastAttribKey tKey) { return tKey.nValue & 0xfff; }

static const AttribDef * sGetAttribDef(Attrib eAttrib) { return &cgarAttribDefs[eAttrib]; }

int32 KeyGetParam(FastAttribKey tKey) { return uint32(tKey.nValue) >> 12; }

AttributeOperator KeyGetAggregationOperator(FastAttribKey tKey) { Attrib eAttrib = KeyGetAttrib(tKey); return cgarAttribDefs[eAttrib].eAttributeOperator; }

AttributeDataType KeyGetDataType(FastAttribKey tKey) { Attrib eAttrib = KeyGetAttrib(tKey); return cgarAttribDefs[eAttrib].eAttributeDataType; }

uint8 KeyGetSyncFlags(FastAttribKey tKey) { Attrib eAttrib = KeyGetAttrib(tKey); return cgarAttribDefs[eAttrib].uSyncFlags; }

int32 StringToAttrib(LPCSTR szAttrib) {
    if (*szAttrib) {
        for (int32 i = 0; i < 1468; ++i)
            if (!strcmp(cgarAttribDefs[i].pszName, szAttrib)) return cgarAttribDefs[i].eAttrib;
    }
    return 0;
}

int32 XMLStringToAttrib(LPCSTR szAttrib) { return *szAttrib ? StringToAttrib(szAttrib) : -1; }

CRefString AttribToString(int32 eAttrib) { return CRefString(sGetAttribDef(eAttrib)->pszName); }

CRefString AttribToXMLString(int32 eAttrib) { return eAttrib == -1 ? CRefString("") : AttribToString(eAttrib); }

static Attrib sCompileAttrib_GetAttrib(const CRefString & sIdentifier) {
    LPCSTR szBangPos = strchr(sIdentifier, '#');
    if (!szBangPos) {
        return StringToAttrib(sIdentifier);
    } else {
        CHAR szBuffer[256];
        size_t nLength = szBangPos - (LPCSTR)sIdentifier;
        strncpy(szBuffer, sIdentifier, nLength);
        szBuffer[nLength] = 0;
        return StringToAttrib(szBuffer);
    }
}

int32 AttribStringToParam(Attrib eAttrib, LPCSTR szParam) {
    if (!*szParam) return -1;
    if (uint32(*szParam - '0') <= 9) return atoi(szParam);
    if (uint32(eAttrib) >= 1468) return -1;
    const AttribDef * ptAttribDef = sGetAttribDef(eAttrib);
    switch (ptAttribDef->eParameterType) {
    case APT_ELEMENT: return NameToSymbol(szParam, gDamageTypes);
    case APT_ATTRIBUTE: return StringToAttrib(szParam);
    case APT_SKILL:
    case APT_RUNE_POWER: return SNOFileLookupGUID(SNOGroup(29), szParam);
    case APT_RESOURCE: return NameToSymbol(szParam, gResourceTypes);
    case APT_TREASURECLASS: return SNOFileLookupGUID(SNOGroup(52), szParam);
    case APT_ACTOR: return SNOFileLookupGUID(SNOGroup(1), szParam);
    case APT_CONVERSATION: return SNOFileLookupGUID(SNOGroup(12), szParam);
    case APT_MONSTER_TYPE: return NameToSymbol(szParam, gMonsterTypes);
    case APT_INVENTORY_SLOT_TYPE: return NameToSymbol(szParam, gInventorySlots);
    default: return -1;
    }
}

CRefString AttribParamToString(Attrib eAttrib, int32 nParam) {
    if (nParam == -1) return CRefString("");
    if (uint32(eAttrib) >= 1468) return CRefString("");
    const AttribDef * ptAttribDef = sGetAttribDef(eAttrib);
    switch (ptAttribDef->eParameterType) {
    case APT_ELEMENT: return CRefString(SymbolToName(nParam, gDamageTypes));
    case APT_ATTRIBUTE: return AttribToString(nParam);
    case APT_SKILL:
    case APT_RUNE_POWER: return SNOToString(SNOGroup(29), SNO(nParam), FALSE);
    case APT_RESOURCE: return CRefString(SymbolToName(nParam, gResourceTypes));
    case APT_TREASURECLASS: return SNOToString(SNOGroup(52), SNO(nParam), FALSE);
    case APT_ACTOR: return SNOToString(SNOGroup(1), SNO(nParam), FALSE);
    case APT_CONVERSATION: return SNOToString(SNOGroup(12), SNO(nParam), FALSE);
    case APT_MONSTER_TYPE: return CRefString(SymbolToName(nParam, gMonsterTypes));
    case APT_INVENTORY_SLOT_TYPE: return CRefString(SymbolToName(nParam, gInventorySlots));
    default: return CRefString(CRefString().Format("%d", nParam));
    }
}

static int32 sCompileAttrib_GetAttribParam(Attrib eAttrib, const CRefString & sIdentifier) {
    CHAR szBuffer[256];
    LPCSTR szBangPos = strchr(sIdentifier, '#');
    if (!szBangPos) return -2;
    Blizzard::String::Copy(szBuffer, szBangPos + 1, sizeof(szBuffer));
    if (!strcmp(szBuffer, "NONE")) return -1;
    return AttribStringToParam(eAttrib, szBuffer);
}

static IdentifierResult sIdentifierFromFastAttribIdentifier(const IdentifierFastAttribResult & iar) {
    IdentifierResult ir;
    ir.nIdentifier = iar.eIdentifier;
    ir.nParam[0] = iar.eAttrib;
    ir.nParam[1] = iar.nAttribParam;
    ir.nParam[2] = iar.eSource | (iar.eValue << 16);
    return ir;
}

void FastAttribGetIdentifierAttributeFromParams(IdentifierFastAttribResult & tResult,
        int32 nIdentifier, int32 nParam0, int32 nParam1, int32 nParam2) {
    tResult.eIdentifier = nIdentifier;
    tResult.eAttrib = nParam0;
    tResult.nAttribParam = nParam1;
    tResult.eValue = AttributeValue(nParam2 >> 16);
    tResult.eSource = AttributeSource(nParam2 & 0xffff);
}

void FastAttributeGetAttributeAndParamFromString(const char * szString, Attrib & eAttrib, int32 & nParam) {
    eAttrib = -1;
    nParam = 0;
    eAttrib = sCompileAttrib_GetAttrib(szString);
    nParam = sCompileAttrib_GetAttribParam(eAttrib, szString);
}

// NON_MATCHING: correct 712-byte size; source/value default assignments produce different branch layout.
BOOL FastAttributeBBECompileAttrib(const TokenizedIdentifier & tTokenizeIdentifier, IdentifierResult * ptIdentifierResult) {
    IdentifierFastAttribResult tAttribResult;
    tAttribResult.eIdentifier = 0;
    switch (tTokenizeIdentifier.nStringCount) {
    case 1:
        tAttribResult.eValue = ATTRIBVALUE_TOTAL;
        tAttribResult.eSource = ATTRIBSOURCE_DEFAULT_ACTOR;
        tAttribResult.eAttrib = sCompileAttrib_GetAttrib(tTokenizeIdentifier.sString[0]);
        tAttribResult.nAttribParam = sCompileAttrib_GetAttribParam(tAttribResult.eAttrib, tTokenizeIdentifier.sString[0]);
        break;
    case 2:
        if (NameToSymbol(tTokenizeIdentifier.sString[1], gAttributeValues) != ATTRIBVALUE_NONE) {
            tAttribResult.eSource = ATTRIBSOURCE_DEFAULT_ACTOR;
            tAttribResult.eAttrib = sCompileAttrib_GetAttrib(tTokenizeIdentifier.sString[0]);
            tAttribResult.nAttribParam = sCompileAttrib_GetAttribParam(tAttribResult.eAttrib, tTokenizeIdentifier.sString[0]);
            tAttribResult.eValue = AttributeValue(NameToSymbol(tTokenizeIdentifier.sString[1], gAttributeValues));
        } else {
            tAttribResult.eSource = AttributeSource(NameToSymbol(tTokenizeIdentifier.sString[0], gAttributeSources));
            tAttribResult.eAttrib = sCompileAttrib_GetAttrib(tTokenizeIdentifier.sString[1]);
            tAttribResult.nAttribParam = sCompileAttrib_GetAttribParam(tAttribResult.eAttrib, tTokenizeIdentifier.sString[1]);
            tAttribResult.eValue = ATTRIBVALUE_TOTAL;
        }
        break;
    case 3:
        tAttribResult.eSource = AttributeSource(NameToSymbol(tTokenizeIdentifier.sString[0], gAttributeSources));
        tAttribResult.eAttrib = sCompileAttrib_GetAttrib(tTokenizeIdentifier.sString[1]);
        tAttribResult.nAttribParam = sCompileAttrib_GetAttribParam(tAttribResult.eAttrib, tTokenizeIdentifier.sString[1]);
        tAttribResult.eValue = AttributeValue(NameToSymbol(tTokenizeIdentifier.sString[2], gAttributeValues));
        break;
    default: return FALSE;
    }
    if (tAttribResult.eSource == ATTRIBSOURCE_NONE) return FALSE;
    *ptIdentifierResult = sIdentifierFromFastAttribIdentifier(tAttribResult);
    return TRUE;
}

int32 & AttribValueToInt(FastAttribValue & tValue) { return tValue.nValue; }

float & AttribValueToFloat(FastAttribValue & tValue) { return tValue.flValue; }

void FastAttribAggregate(FastAttribValue & tRunningTotal, FastAttribValue tValue,
        AttributeDataType eDataType, AttributeOperator eOperator, const FastAttribKey & tKey) {
    switch (eOperator) {
    case ATTRIBUTEOP_NONE: tRunningTotal = tValue; break;
    case ATTRIBUTEOP_ACDID:
        if (tRunningTotal.nValue != -1) break;
        // Unset actor IDs fall through to the set operation.
    case ATTRIBUTEOP_SET:
        if (tValue.nValue != -1) tRunningTotal = tValue;
        break;
    case ATTRIBUTEOP_NOAGG: break;
    case ATTRIBUTEOP_MAX:
        switch (eDataType) {
        case ATTRIBUTEDT_FLOAT: tRunningTotal.flValue = Math::Max(tRunningTotal.flValue, tValue.flValue); break;
        case ATTRIBUTEDT_INT: tRunningTotal.nValue = Math::Max(tRunningTotal.nValue, tValue.nValue); break;
        default: DisplayInternalError(0, "", 3368, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    case ATTRIBUTEOP_MIN:
        switch (eDataType) {
        case ATTRIBUTEDT_FLOAT: tRunningTotal.flValue = Math::Min(tRunningTotal.flValue, tValue.flValue); break;
        case ATTRIBUTEDT_INT: tRunningTotal.nValue = Math::Min(tRunningTotal.nValue, tValue.nValue); break;
        default: DisplayInternalError(0, "", 3381, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    case ATTRIBUTEOP_PLUS:
        switch (eDataType) {
        case ATTRIBUTEDT_FLOAT: tRunningTotal.flValue += tValue.flValue; break;
        case ATTRIBUTEDT_INT: tRunningTotal.nValue += tValue.nValue; break;
        default: DisplayInternalError(0, "", 3394, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    case ATTRIBUTEOP_MULTIPLY:
        switch (eDataType) {
        case ATTRIBUTEDT_FLOAT: tRunningTotal.flValue *= tValue.flValue; break;
        case ATTRIBUTEDT_INT: tRunningTotal.nValue *= tValue.nValue; break;
        default: DisplayInternalError(0, "", 3407, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    case ATTRIBUTEOP_MULTIPLY_INVERSE:
        switch (eDataType) {
        case ATTRIBUTEDT_FLOAT: tRunningTotal.flValue = 1.0f - ((1.0f - tRunningTotal.flValue) * (1.0f - tValue.flValue)); break;
        case ATTRIBUTEDT_INT: tRunningTotal.nValue = 1 - ((1 - tRunningTotal.nValue) * (1 - tValue.nValue)); break;
        default: DisplayInternalError(0, "", 3421, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        break;
    default: DisplayInternalError(0, "", 3436, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}


BOOL FastAttribValueWentAboveZero(FastAttribKey tKey, FastAttribValue tLastValue, FastAttribValue tValue) {
    switch (KeyGetDataType(tKey)) {
    case ATTRIBUTEDT_FLOAT: {
        float flLastValue = tLastValue.flValue;
        float flValue = tValue.flValue;
        if (flLastValue < 0.000001f && flValue > 0.000001f) return TRUE;
        break;
    }
    case ATTRIBUTEDT_INT: {
        int32 nLastValue = tLastValue.nValue;
        int32 nValue = tValue.nValue;
        if (nLastValue > 0 || nValue <= 0) break;  // original 0x71005f45e8: cmp #1, b.lt (lines 5098/5099 test the exits)
        return TRUE;
    }
    default: DisplayInternalError(0, "", 5108, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return FALSE;
}

AttributeParameterType KeyGetParamType(FastAttribKey tKey) { Attrib eAttrib = KeyGetAttrib(tKey); return cgarAttribDefs[eAttrib].eParameterType; }

const AttributeCompressor * KeyGetCompressor(FastAttribKey tKey) { Attrib eAttrib = KeyGetAttrib(tKey); return cgarAttribDefs[eAttrib].ptCompressor; }

typedef XPooledList<FastAttribGroup *, FastAttribGroup *> FastAttribGroupListPooled;
typedef XPooledMap<FastAttribKey, FastAttribKey, FastAttribValue, FastAttribValue, 512> FastAttribMapPooledVeryLarge;
typedef XPooledMap<FastAttribKey, FastAttribKey, FastAttribValue, FastAttribValue, 256> FastAttribMapPooledLarge;
typedef XPooledMap<FastAttribKey, FastAttribKey, FastAttribValue, FastAttribValue, 32> FastAttribMapPooled;

struct FastAttribListLarge {  // size 0x828
    FastAttribMapPooledLarge mapAttribEntries;  // 0x00
protected:
    FastAttribListLarge & operator=(const FastAttribListLarge &);
};
static_assert(sizeof(FastAttribListLarge) == 0x828, "FastAttribListLarge");

struct FastAttribListVeryLarge {  // size 0x8
    FastAttribMapPooledVeryLarge * pmapAttribEntries;  // 0x00
protected:
    FastAttribListVeryLarge & operator=(const FastAttribListVeryLarge &);
};
static_assert(sizeof(FastAttribListVeryLarge) == 0x8, "FastAttribListVeryLarge");

struct FastAttribGroup {  // size 0x12e8
    AttribGroupID id;  // 0x00
    uint32 bLight : 1;  // 0x04
    uint32 bDirty : 1;  // 0x04
    uint32 bUseVeryLarge : 1;  // 0x04
    FastAttribType eFastAttribType : 4;  // 0x04
    int32 nDefaultEntryIndex;  // 0x08
    FastAttribListVeryLarge tVeryLargeCache;  // 0x10
    FastAttribListLarge tCache;  // 0x18
    FastAttribListLarge tBase;  // 0x840
    AttribGroupID idParentAttribGroup;  // 0x1068
    FastAttribGroupListPooled listChildrenAttribGroups;  // 0x1070
    FastAttribMapPooled mapDirtyKeys;  // 0x1098
    FastAttribMapPooled mapModifiedKeys;  // 0x11c0
};
static_assert(sizeof(FastAttribGroup) == 0x12e8, "FastAttribGroup");
static_assert(offsetof(FastAttribGroup, tBase) == 0x840, "FastAttribGroup");

// line 3443
static BOOL sFastAttribListGetValue(FastAttribType, const FastAttribListLarge & tList, FastAttribKey tKey, FastAttribValue & tRetval)
{
    return tList.mapAttribEntries.Lookup(tKey, tRetval);
}

// line 3941
static void sFastAttribGroupGetDefault(const FastAttribGroup & tGroup, FastAttribKey tKey, FastAttribValue & tRetval)
{
    tRetval = sGetAttribDef(KeyGetAttrib(tKey))->tDefaultValue;
}

// line 4003
static void sFastAttribGroupGetBaseValue(FastAttribGroup & tGroup, FastAttribKey tKey, FastAttribValue & tValue)
{
    BOOL bRetval = sFastAttribListGetValue(tGroup.eFastAttribType, tGroup.tBase, tKey, tValue);
    if (!bRetval)
        sFastAttribGroupGetDefault(tGroup, tKey, tValue);
}

// line 4521
float FastAttribGetBaseValueFloat(FastAttribGroup * ptAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    FastAttribValue tValue;
    sFastAttribGroupGetBaseValue(tGroup, tKey, tValue);
    return tValue.flValue;
}

struct FastAttribGameGlobals {  // size 0xa0
    XMapMemoryPool<FastAttribKey, FastAttribValue> tFastAttribMapPool;  // 0x00
    XListMemoryPool<FastAttribKey> tFastAttribKeyPool;  // 0x30
    XListMemoryPool<FastAttribGroup *> tFastAttribGroupPtrPool;  // 0x60
    TChainedDataArray<FastAttribGroup, int> * pFastAttribGroups;  // 0x90
    POS posAttributeSlowRecalcCounter;  // 0x98
};
static_assert(sizeof(FastAttribGameGlobals) == 0xa0, "FastAttribGameGlobals");

struct BBEFastAttribDependencyCheckData {
    Attrib eAttrib;  // 0x00 (name invented)
    XPooledList<int, int> * plistIDependOn;  // 0x08
};

// line 2580
BOOL BBEFastAttribDependenciesCallback(int32 nIdentifier, int32 nParam0, int32 nParam1, int32 nParam2, void * pData, float & flResult)
{
    const BBEFastAttribDependencyCheckData & tCallbackData = *(BBEFastAttribDependencyCheckData *)pData;
    if (nIdentifier == 0)
    {
        IdentifierFastAttribResult tResult;
        FastAttribGetIdentifierAttributeFromParams(tResult, nIdentifier, nParam0, nParam1, nParam2);
        Attrib eAttrib = tResult.eAttrib;
        if (!tCallbackData.plistIDependOn->Find(eAttrib))
            tCallbackData.plistIDependOn->AddTail_Tracked(eAttrib);
    }
    flResult = 1.0f;
    return TRUE;
}

// line 2801
void FastAttribInitializeForGame()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    FastAttribGameGlobals *& pGlobals = GameGetActiveGameCommonData().ptFastAttribGameGlobals;
    void * pMem = SigmaMemoryNew(sizeof(FastAttribGameGlobals), 4, ptMemoryInterface, FALSE);
    pGlobals = new (pMem) FastAttribGameGlobals;
    pGlobals->pFastAttribGroups = TChainedDataArrayCreateTracked<FastAttribGroup, AttribGroupID>("FastAttribGroups", ptMemoryInterface, 1024);
    pGlobals->tFastAttribMapPool.SetMemoryInterface(ptMemoryInterface);
    pGlobals->tFastAttribMapPool.SetBlockCount_Tracked(32768);
    pGlobals->tFastAttribMapPool.SetGrow(XALLOC_GROW_LINEAR, 4096);
    pGlobals->tFastAttribMapPool.SetShrinkable(TRUE);

    pGlobals->tFastAttribKeyPool.SetMemoryInterface(ptMemoryInterface);
    pGlobals->tFastAttribKeyPool.SetBlockCount_Tracked(128);
    pGlobals->tFastAttribKeyPool.SetGrow(XALLOC_GROW_LINEAR, 16);
    pGlobals->tFastAttribKeyPool.SetShrinkable(TRUE);

    pGlobals->tFastAttribGroupPtrPool.SetMemoryInterface(ptMemoryInterface);
    pGlobals->tFastAttribGroupPtrPool.SetBlockCount_Tracked(1024);
    pGlobals->tFastAttribGroupPtrPool.SetGrow(XALLOC_GROW_LINEAR, 128);
    pGlobals->tFastAttribGroupPtrPool.SetShrinkable(TRUE);
}

// line 2837
void FastAttribDisposeForGame()
{
    FastAttribGameGlobals *& pGlobals = GameGetActiveGameCommonData().ptFastAttribGameGlobals;
    TDataArrayIterator<FastAttribGroup> tIt;
    while (pGlobals->pFastAttribGroups->Iterate(tIt))
    {
        FastAttribGroupDelete(tIt.ptElement->id);
    }
    TChainedDataArrayDispose(pGlobals->pFastAttribGroups);
    pGlobals->~FastAttribGameGlobals();
    SigmaMemoryFree(pGlobals, AppGetCurrentMemoryInterface());
}

// line 2883
static FastAttribGroup & sFastAttribGroupGet(AttribGroupID idAttribGroup)
{
    FastAttribGameGlobals * pGlobals = GameGetActiveGameCommonData().ptFastAttribGameGlobals;
    FastAttribGroup * pAttribGroup = pGlobals->pFastAttribGroups->ElementGet(idAttribGroup);
    return *pAttribGroup;
}

// line 3117
FastAttribGroup * FastAttribGroupGet(AttribGroupID idAttribGroup)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    return &tGroup;
}

// line 3126
BOOL FastAttribGroupIsValid(AttribGroupID idAttribGroup)
{
    if (idAttribGroup != ATTRIBGROUPID_INVALID) {
        FastAttribGameGlobals * pGlobals = GameGetActiveGameCommonData().ptFastAttribGameGlobals;
        FastAttribGroup * pAttribGroup = pGlobals->pFastAttribGroups->ElementTryToGet(idAttribGroup);
        if (pAttribGroup)
            return TRUE;
    }
    return FALSE;
}

// line 4552
FastAttribValue FastAttribGetBaseValue(AttribGroupID idAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    FastAttribValue tValue;
    sFastAttribGroupGetBaseValue(tGroup, tKey, tValue);
    return tValue;
}

typedef XPooledList<int, int> AttribList;

struct FastAttribConstantData {  // size 0x70
    AttribList listDependsOnMe;  // 0x00
    AttribList listIDependOn;  // 0x28
    uint8 * ptBBEFormulaItem;  // 0x50
    int32 nFormulaItemSize;  // 0x58
    uint8 * ptBBEFormulaCharacter;  // 0x60
    int32 nFormulaCharacterSize;  // 0x68
};
static_assert(sizeof(FastAttribConstantData) == 0x70, "FastAttribConstantData");

struct FastAttribAppGlobals {  // size 0x10
    FastAttribConstantData * arAttribConstantData;  // 0x00
    XListMemoryPool<int> * ptAttribConstantDataPool;  // 0x08
};
static_assert(sizeof(FastAttribAppGlobals) == 0x10, "FastAttribAppGlobals");

typedef XPooledList<FastAttribKey, FastAttribKey> FastAttribKeyListPooled;
typedef XPooledList<uint32, uint32> XPooledDWORDList;  // belongs in XPooledList.h

static void sRecalcAttrib(FastAttribGroup & tGroup, FastAttribKey tKey, BOOL fRemoveFromDirty);
static FastAttribValue sFastAttribGroupGetValue(FastAttribGroup & tGroup, FastAttribKey tKey);
static BOOL sFastAttribGroupGetCacheValue(const FastAttribGroup & tGroup, FastAttribKey tKey, FastAttribValue & tValue);
inline FastAttribValue FastAttribGetValue(FastAttribGroup * ptAttribGroup, FastAttribKey tKey);

// line 3065
inline FastAttribAppGlobals * FastAttribAppGlobalsGet()
{
    return sgtAppGlobals.ptFastAttribGlobals;
}

// line 3210
static void sRecalcDirtyAttribs(FastAttribGroup & tGroup)
{
    if (!tGroup.bDirty)
        return;

    FastAttribGameGlobals * pGlobals = GameGetActiveGameCommonData().ptFastAttribGameGlobals;
    FastAttribKeyListPooled listAttribs(&pGlobals->tFastAttribKeyPool);
    POS pos = tGroup.mapDirtyKeys.GetStartAssoc();
    while (pos) {
        FastAttribKey tKey;
        FastAttribValue tTemp;
        tGroup.mapDirtyKeys.GetNextAssoc(pos, tKey, tTemp);
        listAttribs.AddTail_Tracked(tKey);
    }

    while (listAttribs.Count()) {
        FastAttribKey key = listAttribs.GetHead();
        FastAttribValue tTemp;
        if (tGroup.mapDirtyKeys.Lookup(key, tTemp)) {
            sRecalcAttrib(tGroup, key, FALSE);
            tGroup.mapDirtyKeys.RemoveAt(key);
        }
        listAttribs.RemoveHead();
    }

    tGroup.bDirty = tGroup.mapDirtyKeys.Count() > 0;
}

// line 3250
void FastAttribGroupApplyDirty(AttribGroupID idAttribGroup)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    sRecalcDirtyAttribs(tGroup);
}

// line 3260
FastAttribIterator::FastAttribIterator(AttribGroupID idAttribGroup, BOOL bWantsCleanAttributes)
{
    ptGroup = &sFastAttribGroupGet(idAttribGroup);
    if (bWantsCleanAttributes && ptGroup->bDirty)
        sRecalcDirtyAttribs(*ptGroup);
    posCurrentSlowMapPosition = ptGroup->bUseVeryLarge ? ptGroup->tVeryLargeCache.pmapAttribEntries->GetStartAssoc() : ptGroup->tCache.mapAttribEntries.GetStartAssoc();
}

// line 3293
BOOL FastAttribIterate(FastAttribIterator & tIt)
{
    if (!tIt.posCurrentSlowMapPosition)
        return FALSE;
    if (tIt.ptGroup->bUseVeryLarge)
        tIt.ptGroup->tVeryLargeCache.pmapAttribEntries->GetNextAssoc(tIt.posCurrentSlowMapPosition, tIt.tEntryBuffer.tKey, tIt.tEntryBuffer.tValue);
    else
        tIt.ptGroup->tCache.mapAttribEntries.GetNextAssoc(tIt.posCurrentSlowMapPosition, tIt.tEntryBuffer.tKey, tIt.tEntryBuffer.tValue);
    return TRUE;
}

// line 3317
void FastAttribGetAllWithAttrib(AttribGroupID idFastAttrib, Attrib nAttrib, XPooledDWORDList & listParams)
{
    FastAttribIterator tIt(idFastAttrib, TRUE);
    while (FastAttribIterate(tIt)) {
        if (KeyGetAttrib(tIt.tEntryBuffer.tKey) == nAttrib) {
            int32 nParam = KeyGetParam(tIt.tEntryBuffer.tKey);
            listParams.AddTail_Tracked(nParam);
        }
    }
}

// line 3455
static BOOL sFastAttribListGetValue(FastAttribType, const FastAttribListVeryLarge & tList, FastAttribKey tKey, FastAttribValue & tRetval)
{
    return tList.pmapAttribEntries->Lookup(tKey, tRetval);
}

// line 3507
static void sFastAttribGroupAggregate(FastAttribGroup & tGroup, FastAttribKey tKey, FastAttribValue & tRunningTotal)
{
    const AttribDef * ptAttribDef = sGetAttribDef(KeyGetAttrib(tKey));
    AttributeOperator eOperator = ptAttribDef->eAttributeOperator;
    AttributeDataType eDataType = ptAttribDef->eAttributeDataType;
    sFastAttribGroupGetBaseValue(tGroup, tKey, tRunningTotal);
    POS pos = tGroup.listChildrenAttribGroups.GetHeadPosition();
    while (pos) {
        FastAttribGroup * ptChildGroup = tGroup.listChildrenAttribGroups.GetNext(pos);
        FastAttribValue tValue = sFastAttribGroupGetValue(*ptChildGroup, tKey);
        FastAttribAggregate(tRunningTotal, tValue, eDataType, eOperator, tKey);
    }
}

// line 3674
static void sFastAttribListSetValue(FastAttribType, FastAttribListLarge & tList, FastAttribKey tKey, FastAttribValue tValue)
{
    tList.mapAttribEntries.SetAt_Tracked(tKey, tValue);
}

// line 3690
static void sFastAttribListSetValue(FastAttribType, FastAttribListVeryLarge & tList, FastAttribKey tKey, FastAttribValue tValue)
{
    tList.pmapAttribEntries->SetAt_Tracked(tKey, tValue);
}

// line 3722
static void sFastAttribListDeleteAttrib(FastAttribType, FastAttribListLarge & tList, FastAttribKey tKey)
{
    FastAttribValue tValue;
    BOOL bRetval = tList.mapAttribEntries.Lookup(tKey, tValue);
    if (bRetval)
        tList.mapAttribEntries.RemoveAt(tKey);
}

// line 3739
static void sFastAttribListDeleteAttrib(FastAttribType, FastAttribListVeryLarge & tList, FastAttribKey tKey)
{
    FastAttribValue tValue;
    BOOL bRetval = tList.pmapAttribEntries->Lookup(tKey, tValue);
    if (bRetval)
        tList.pmapAttribEntries->RemoveAt(tKey);
}

// line 3765
static void sRecalcAttrib(FastAttribGroup & tGroup, FastAttribKey tKey, BOOL fRemoveFromDirty)
{
    FastAttribValue tPrev;
    BOOL bPrev = sFastAttribGroupGetCacheValue(tGroup, tKey, tPrev);
    const FastAttribConstantData & tData = FastAttribAppGlobalsGet()->arAttribConstantData[KeyGetAttrib(tKey)];
    uint8 * ptBBEFormulaToUse = NULL;
    int32 nSize = 0;
    switch (tGroup.eFastAttribType) {
    case 0:
        break;
    case 1:
    case 4:
        ptBBEFormulaToUse = tData.ptBBEFormulaCharacter;
        nSize = tData.nFormulaCharacterSize;
        break;
    case 2:
        ptBBEFormulaToUse = tData.ptBBEFormulaItem;
        nSize = tData.nFormulaItemSize;
        break;
    default: DisplayInternalError(0, "", 3793, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    FastAttribValue tTotal;
    if (ptBBEFormulaToUse) {
        BBECallbackData tCallback;
        InitBBECallbackData(&tCallback);
        tCallback.idFastAttrib = tGroup.id;
        tCallback.nAttribParam = KeyGetParam(tKey);
        float flRetval;
        BOOL bSuccess = BBEEvaluate(ptBBEFormulaToUse, nSize, BBEIdentifierEvaluate, BBEFunctions, NUM_BBEFUNCTIONS, NULL, &tCallback, flRetval);
        BOOL bIsFinite = isfinite(flRetval);
        if (KeyGetAttrib(tKey) == ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_SUBTOTAL && !bIsFinite) {
            float ftHitpointsRegenPerSecond = FastAttribGetValueFloat(&tGroup, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND);
            float ftHitpointsRegenPerSecondBonus = FastAttribGetValueFloat(&tGroup, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_BONUS);
            float ftHitpointsRegenBonusPercent = FastAttribGetValueFloat(&tGroup, ATTRIBUTE_HITPOINTS_REGEN_BONUS_PERCENT);
        }
        switch (KeyGetDataType(tKey)) {
        case ATTRIBUTEDT_FLOAT: tTotal.flValue = flRetval; break;
        case ATTRIBUTEDT_INT: tTotal.nValue = Math::FloatToInt(flRetval); break;
        default: DisplayInternalError(0, "", 3880, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    } else {
        sFastAttribGroupAggregate(tGroup, tKey, tTotal);
    }

    FastAttribValue tDefault;
    sFastAttribGroupGetDefault(tGroup, tKey, tDefault);
    if (bPrev ? tTotal.nValue != tPrev.nValue : tDefault.nValue != tTotal.nValue)
        tGroup.mapModifiedKeys.SetAt_Tracked(tKey, ATTRIB_VALUE_NONE);

    if (tTotal.nValue == tDefault.nValue && ptBBEFormulaToUse == NULL) {
        if (tGroup.bUseVeryLarge)
            sFastAttribListDeleteAttrib(tGroup.eFastAttribType, tGroup.tVeryLargeCache, tKey);
        else
            sFastAttribListDeleteAttrib(tGroup.eFastAttribType, tGroup.tCache, tKey);
    } else {
        if (tGroup.bUseVeryLarge)
            sFastAttribListSetValue(tGroup.eFastAttribType, tGroup.tVeryLargeCache, tKey, tTotal);
        else
            sFastAttribListSetValue(tGroup.eFastAttribType, tGroup.tCache, tKey, tTotal);
    }

    if (fRemoveFromDirty) {
        FastAttribValue tTemp;
        if (tGroup.mapDirtyKeys.Lookup(tKey, tTemp))
            tGroup.mapDirtyKeys.RemoveAt(tKey);
        tGroup.bDirty = tGroup.mapDirtyKeys.Count() > 0;
    }
}

// line 3958
static BOOL sFastAttribGroupGetCacheValue(const FastAttribGroup & tGroup, FastAttribKey tKey, FastAttribValue & tValue)
{
    if (tGroup.bUseVeryLarge)
        return sFastAttribListGetValue(tGroup.eFastAttribType, tGroup.tVeryLargeCache, tKey, tValue);
    return sFastAttribListGetValue(tGroup.eFastAttribType, tGroup.tCache, tKey, tValue);
}

// line 3976
static FastAttribValue sFastAttribGroupGetValue(FastAttribGroup & tGroup, FastAttribKey tKey)
{
    Attrib eAttrib = KeyGetAttrib(tKey);
    const FastAttribValue * ptDefaultValue = &sGetAttribDef(eAttrib)->tDefaultValue;
    if (tGroup.bDirty) {
        FastAttribValue tTemp;
        if (tGroup.mapDirtyKeys.Lookup(tKey, tTemp))
            sRecalcAttrib(tGroup, tKey, TRUE);
    }
    FastAttribValue tValue;
    if (!sFastAttribGroupGetCacheValue(tGroup, tKey, tValue))
        tValue = *ptDefaultValue;
    return tValue;
}

// line 4441
int32 FastAttribGetValueInt(AttribGroupID idAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    return FastAttribGetValueInt(&tGroup, tKey);
}

// line 4452
float FastAttribGetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    return FastAttribGetValueFloat(&tGroup, tKey);
}

// line 4463
FastAttribValue FastAttribGetValue(AttribGroupID idAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    return FastAttribGetValue(&tGroup, tKey);
}

// line 4473
int32 FastAttribGetValueInt(FastAttribGroup * ptAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    FastAttribValue tValue = sFastAttribGroupGetValue(tGroup, tKey);
    int32 nValue = tValue.nValue;
    return nValue;
}

// line 4488
float FastAttribGetValueFloat(FastAttribGroup * ptAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    FastAttribValue tValue = sFastAttribGroupGetValue(tGroup, tKey);
    float flValue = tValue.flValue;
    return flValue;
}

// line 4539
inline FastAttribValue FastAttribGetValue(FastAttribGroup * ptAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    return sFastAttribGroupGetValue(tGroup, tKey);
}

struct FastAttribDependancyIterator {  // size 0x18
    FastAttribDependancyIterator(FastAttribKey);
    FastAttribKey tKey;  // 0x00
    POS posCurrentDependancy;  // 0x08
    FastAttribKey tKeyDependancy;  // 0x10
};
static_assert(sizeof(FastAttribDependancyIterator) == 0x18, "FastAttribDependancyIterator");

// line 3156
inline FastAttribDependancyIterator::FastAttribDependancyIterator(FastAttribKey arg_tKey)
{
    tKey = arg_tKey;
    posCurrentDependancy = FastAttribAppGlobalsGet()->arAttribConstantData[KeyGetAttrib(tKey)].listDependsOnMe.GetHeadPosition();
}

// line 3164
inline BOOL sFastAttribDependancyIterate(FastAttribDependancyIterator & tIt)
{
    if (!tIt.posCurrentDependancy)
        return FALSE;
    Attrib eAttribDependancy = FastAttribAppGlobalsGet()->arAttribConstantData[KeyGetAttrib(tIt.tKey)].listDependsOnMe.GetNext(tIt.posCurrentDependancy);
    if (sGetAttribDef(KeyGetAttrib(tIt.tKey))->eParameterType == sGetAttribDef(eAttribDependancy)->eParameterType)
        tIt.tKeyDependancy = MakeAttribKey(eAttribDependancy, KeyGetParam(tIt.tKey));
    else
        tIt.tKeyDependancy = MakeAttribKey(eAttribDependancy, PARAM_NONE);
    return TRUE;
}
// Branch order and the loop local i follow the DWARF (iterator block 4121 before the limit loop 4134-4136).
// line 4090
static void sUpdateDirtyKeysList(FastAttribGroup & tGroup, FastAttribKey tKey)
{
    if (FastAttribGroupIsValid(tGroup.idParentAttribGroup)) {
        FastAttribGroup & tParentGroup = sFastAttribGroupGet(tGroup.idParentAttribGroup);
        sUpdateDirtyKeysList(tParentGroup, tKey);
    }

    AttributeParameterType eKeyType = sGetAttribDef(KeyGetAttrib(tKey))->eParameterType;
    FastAttribValue tTemp;
    FastAttribDependancyIterator tItDependancy(tKey);
    while (sFastAttribDependancyIterate(tItDependancy)) {
        AttributeParameterType eDependancyType = sGetAttribDef(KeyGetAttrib(tItDependancy.tKeyDependancy))->eParameterType;
        if (eDependancyType == eKeyType || eDependancyType == -1) {
            tGroup.mapDirtyKeys.SetAt_Tracked(tItDependancy.tKeyDependancy, ATTRIB_VALUE_NONE);
        } else if (cgarAttribParamLimits[eDependancyType].nMinValue == ATTRIB_PARAM_LIMIT_INVALID) {
            FastAttribIterator tIt(tGroup.id, FALSE);
            while (FastAttribIterate(tIt)) {
                if (KeyGetAttrib(tItDependancy.tKeyDependancy) == KeyGetAttrib(tIt.tEntryBuffer.tKey))
                    tGroup.mapDirtyKeys.SetAt_Tracked(tIt.tEntryBuffer.tKey, ATTRIB_VALUE_NONE);
            }
        } else {
            int32 nMinValue = cgarAttribParamLimits[eDependancyType].nMinValue;
            int32 nUpperLimit = cgarAttribParamLimits[eDependancyType].nUpperLimit;
            for (int32 i = nMinValue; i < nUpperLimit; i++) {
                tGroup.mapDirtyKeys.SetAt_Tracked(MakeAttribKey(KeyGetAttrib(tItDependancy.tKeyDependancy), i), ATTRIB_VALUE_NONE);
            }
        }
    }

    tGroup.bDirty = tGroup.mapDirtyKeys.Count() > 0;
}

// line 4148
static void sFastAttribGroupSetBaseValue(FastAttribGroup & tGroup, FastAttribKey tKey, FastAttribValue tValue)
{
    sFastAttribListSetValue(tGroup.eFastAttribType, tGroup.tBase, tKey, tValue);
    sUpdateDirtyKeysList(tGroup, tKey);
}

// line 4328
void FastAttribSetValueInt(FastAttribGroup * ptAttribGroup, FastAttribKey tKey, int32 nValue)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    FastAttribValue tValue;
    tValue.nValue = nValue;
    sFastAttribGroupSetBaseValue(tGroup, tKey, tValue);
}

// line 4345
void FastAttribSetValueFloat(FastAttribGroup * ptAttribGroup, FastAttribKey tKey, float flValue)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    FastAttribValue tValue;
    tValue.flValue = flValue;
    sFastAttribGroupSetBaseValue(tGroup, tKey, tValue);
}

// line 4368
inline void FastAttribSetValue(FastAttribGroup * ptAttribGroup, FastAttribKey tKey, FastAttribValue tValue)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    sFastAttribGroupSetBaseValue(tGroup, tKey, tValue);
}

// line 4294
void FastAttribSetValueInt(AttribGroupID idAttribGroup, FastAttribKey tKey, int32 nValue)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    FastAttribSetValueInt(&tGroup, tKey, nValue);
}

// line 4306
void FastAttribSetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey, float flValue)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    FastAttribSetValueFloat(&tGroup, tKey, flValue);
}

// line 4317
void FastAttribSetValue(AttribGroupID idAttribGroup, FastAttribKey tKey, FastAttribValue tValue)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    FastAttribSetValue(&tGroup, tKey, tValue);
}

// line 4384
void FastAttribSetValueToDefault(FastAttribGroup * ptAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    FastAttribValue tDefaultValue;
    sFastAttribGroupGetDefault(tGroup, tKey, tDefaultValue);
    sFastAttribGroupSetBaseValue(tGroup, tKey, tDefaultValue);
}

// line 4401
void FastAttribSetValueToDefault(AttribGroupID idAttribGroup, FastAttribKey tKey)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    FastAttribValue tDefaultValue;
    sFastAttribGroupGetDefault(tGroup, tKey, tDefaultValue);
    sFastAttribGroupSetBaseValue(tGroup, tKey, tDefaultValue);
}

// line 4175
void FastAttribMerge(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild)
{
    FastAttribGroup & tGroupParent = sFastAttribGroupGet(idAttribGroupParent);
    FastAttribGroup & tGroupChild = sFastAttribGroupGet(idAttribGroupChild);
    tGroupParent.listChildrenAttribGroups.AddTail_Tracked(&tGroupChild);
    tGroupChild.idParentAttribGroup = idAttribGroupParent;
    sRecalcDirtyAttribs(tGroupChild);
    FastAttribIterator tIt(idAttribGroupChild, TRUE);
    while (FastAttribIterate(tIt))
        sUpdateDirtyKeysList(tGroupParent, tIt.tEntryBuffer.tKey);
}

// line 4275
BOOL FastAttribHasChild(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild)
{
    FastAttribGroup & tGroupParent = sFastAttribGroupGet(idAttribGroupParent);
    FastAttribGroup & tGroupChild = sFastAttribGroupGet(idAttribGroupChild);
    POS pos = tGroupParent.listChildrenAttribGroups.Find(&tGroupChild);
    return pos != NULL;
}

// line 4571
AttribGroupID FastAttribLightCreate(FastAttribType eType)
{
    AttribGroupID idAttribGroup = FastAttribGroupCreate(eType, -1);
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    tGroup.bLight = TRUE;
    return idAttribGroup;
}

// line 4585
void FastAttribLightSetValue(AttribGroupID idAttribGroup, FastAttribKey tKey, FastAttribValue tValue)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroup);
    if (tGroup.bUseVeryLarge)
        sFastAttribListSetValue(tGroup.eFastAttribType, tGroup.tVeryLargeCache, tKey, tValue);
    else
        sFastAttribListSetValue(tGroup.eFastAttribType, tGroup.tCache, tKey, tValue);
}

// line 4615
void FastAttribLightCopy(AttribGroupID idAttribGroupSource, AttribGroupID idAttribGroupDest)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idAttribGroupDest);
    FastAttribGroup & tGroupSource = sFastAttribGroupGet(idAttribGroupSource);
    tGroup.nDefaultEntryIndex = tGroupSource.nDefaultEntryIndex;
    FastAttribIterator tIt(idAttribGroupSource, TRUE);
    while (FastAttribIterate(tIt))
        FastAttribLightSetValue(idAttribGroupDest, tIt.tEntryBuffer.tKey, tIt.tEntryBuffer.tValue);
}

static const int32 cgnAttribDefs = sizeof(cgarAttribDefs) / sizeof(cgarAttribDefs[0]);  // Original line 1979.

// line 2615
static inline void sCompileFormula(LPCSTR szFormula, Attrib eAttrib, uint8 *& ptBBEFormula, int32 & nFormulaSize, AttribList & listIDependOn)
{
    uint8 tBuffer[1024];
    BBETranslationData tTranslation;
    tTranslation.eGBType = -1;
    tTranslation.gbid = -1;
    int32 nRetval = BBEFromString(szFormula, tBuffer, sizeof(tBuffer), tTranslation);
    if (nRetval > 0)
    {
        ptBBEFormula = (uint8 *)SigmaMemoryNew(nRetval, 4, NULL, FALSE);
        nFormulaSize = nRetval;
        SigmaMemoryCopy(ptBBEFormula, tBuffer, nRetval);
        BBEFastAttribDependencyCheckData tCheckData;
        tCheckData.eAttrib = eAttrib;
        tCheckData.plistIDependOn = &listIDependOn;
        float flResult;
        BBEEvaluate(ptBBEFormula, nRetval, BBEFastAttribDependenciesCallback, BBEFunctions, NUM_BBEFUNCTIONS, NULL, &tCheckData, flResult);
    }
    else
    {
        ptBBEFormula = NULL;
        nFormulaSize = 0;
    }
}

// line 2669
void FastAttribInitializeForApp()
{
    int32 nNumChanged;
    FastAttribAppGlobals *& pGlobals = sgtAppGlobals.ptFastAttribGlobals;
    pGlobals = new FastAttribAppGlobals;
    pGlobals->ptAttribConstantDataPool = new XListMemoryPool<int>();
    pGlobals->ptAttribConstantDataPool->SetBlockCount_Tracked(1024);
    pGlobals->arAttribConstantData = new FastAttribConstantData[cgnAttribDefs];

    for (int32 i = 0; i < cgnAttribDefs; i++)
    {
        const AttribDef * pAttribDef = &cgarAttribDefs[i];
        pGlobals->arAttribConstantData[i].listIDependOn.SetPool(pGlobals->ptAttribConstantDataPool);
        pGlobals->arAttribConstantData[i].listDependsOnMe.SetPool(pGlobals->ptAttribConstantDataPool);
        sCompileFormula(pAttribDef->pszFormulaItem, pAttribDef->eAttrib, pGlobals->arAttribConstantData[i].ptBBEFormulaItem, pGlobals->arAttribConstantData[i].nFormulaItemSize, pGlobals->arAttribConstantData[i].listIDependOn);
        sCompileFormula(pAttribDef->pszFormulaCharacter, pAttribDef->eAttrib, pGlobals->arAttribConstantData[i].ptBBEFormulaCharacter, pGlobals->arAttribConstantData[i].nFormulaCharacterSize, pGlobals->arAttribConstantData[i].listIDependOn);
        pGlobals->arAttribConstantData[i].listIDependOn.AddTail_Tracked(pAttribDef->eAttrib);
    }

    do
    {
        nNumChanged = 0;
        for (int32 i = 0; i < cgnAttribDefs; i++)
        {
            POS pos = pGlobals->arAttribConstantData[i].listIDependOn.GetHeadPosition();
            while (pos)
            {
                Attrib eIDependOn = pGlobals->arAttribConstantData[i].listIDependOn.GetNext(pos);
                POS posOther = pGlobals->arAttribConstantData[eIDependOn].listIDependOn.GetHeadPosition();
                while (posOther)
                {
                    Attrib eOther = pGlobals->arAttribConstantData[eIDependOn].listIDependOn.GetNext(posOther);
                    if (!pGlobals->arAttribConstantData[i].listIDependOn.Find(eOther))
                    {
                        nNumChanged++;
                        pGlobals->arAttribConstantData[i].listIDependOn.AddTail_Tracked(eOther);
                    }
                }
            }
        }
    } while (nNumChanged > 0);

    for (int32 i = 0; i < cgnAttribDefs; i++)
    {
        POS pos = pGlobals->arAttribConstantData[i].listIDependOn.GetHeadPosition();
        while (pos)
        {
            Attrib eIDependOn = pGlobals->arAttribConstantData[i].listIDependOn.GetNext(pos);
            pGlobals->arAttribConstantData[eIDependOn].listDependsOnMe.AddTail_Tracked(i);
        }
    }
}

// line 2778
void FastAttribDisposeForApp()
{
    FastAttribAppGlobals *& pGlobals = sgtAppGlobals.ptFastAttribGlobals;
    for (int32 i = 0; i < cgnAttribDefs; i++)
    {
        SigmaMemoryFree(pGlobals->arAttribConstantData[i].ptBBEFormulaItem, NULL);
        pGlobals->arAttribConstantData[i].ptBBEFormulaItem = NULL;
        SigmaMemoryFree(pGlobals->arAttribConstantData[i].ptBBEFormulaCharacter, NULL);
        pGlobals->arAttribConstantData[i].ptBBEFormulaCharacter = NULL;
    }
    delete [] pGlobals->arAttribConstantData;
    delete pGlobals->ptAttribConstantDataPool;
    delete pGlobals;
    AttributeCompressor::Dispose();
}

// line 5212
BOOL FastAttribExists(FastAttribGroup * ptAttribGroup, const FastAttribKey tKey)
{
    FastAttribGroup & tGroup = *ptAttribGroup;
    Attrib eAttrib = KeyGetAttrib(tKey);
    if (eAttrib >= cgnAttribDefs)
        return FALSE;
    FastAttribValue tValue = sFastAttribGroupGetValue(tGroup, tKey);
    FastAttribValue tDefaultValue;
    sFastAttribGroupGetDefault(tGroup, tKey, tDefaultValue);
    AttributeDataType eDataType = KeyGetDataType(tKey);
    switch (eDataType) {
    case ATTRIBUTEDT_FLOAT:
        return !Math::FloatEqual(tValue.flValue, tDefaultValue.flValue);
    case ATTRIBUTEDT_INT:
        return tValue.nValue != tDefaultValue.nValue;
    }
    return FALSE;
}

// line 5195
BOOL FastAttribExists(AttribGroupID idFastAttrib, const FastAttribKey tKey)
{
    Attrib eAttrib = KeyGetAttrib(tKey);
    if (eAttrib >= cgnAttribDefs)
        return FALSE;
    FastAttribGroup & tGroup = sFastAttribGroupGet(idFastAttrib);
    return FastAttribExists(&tGroup, tKey);
}

// line 5142
CRefString FastAttribToString(const FastAttribKey tKey)
{
    CRefString sResult;
    Attrib eAttrib = KeyGetAttrib(tKey);
    const AttribDef * ptAttribDef = sGetAttribDef(eAttrib);
    switch (ptAttribDef->eParameterType) {
    case 0:
        sResult.Format("%s#%s", ptAttribDef->pszName, SymbolToName(KeyGetParam(tKey), gDamageTypes));
        break;
    case 10:
        sResult.Format("%s#%s", ptAttribDef->pszName, SymbolToName(KeyGetParam(tKey), gResourceTypes));
        break;
    case 18:
        sResult.Format("%s#%s", ptAttribDef->pszName, SymbolToName(KeyGetParam(tKey), gMonsterTypes));
        break;
    case 19:
        sResult.Format("%s#%s", ptAttribDef->pszName, SymbolToName(KeyGetParam(tKey), gInventorySlots));
        break;
    case 4:
        sResult.Format("%s#%s", ptAttribDef->pszName, SNOToString(0x1d, KeyGetParam(tKey), FALSE).str());
        break;
    default:
        sResult = ptAttribDef->pszName;
        break;
    }
    return sResult;
}

// line 5327
void FastAttribClearModified(AttribGroupID idFastAttrib)
{
    FastAttribGroup & tGroup = sFastAttribGroupGet(idFastAttrib);
    tGroup.mapModifiedKeys.RemoveAll();
}

// line 5338
FastAttribModifiedIterator::FastAttribModifiedIterator(AttribGroupID idAttribGroup)
{
    ptGroup = &sFastAttribGroupGet(idAttribGroup);
    sRecalcDirtyAttribs(*ptGroup);
    pos = ptGroup->mapModifiedKeys.GetStartAssoc();
}

// line 5351
BOOL FastAttribModifiedIterate(FastAttribModifiedIterator & tIt)
{
    if (!tIt.pos)
        return FALSE;
    FastAttribValue tTemp;
    tIt.ptGroup->mapModifiedKeys.GetNextAssoc(tIt.pos, tIt.tEntryBuffer.tKey, tTemp);
    tIt.tEntryBuffer.tValue = sFastAttribGroupGetValue(*tIt.ptGroup, tIt.tEntryBuffer.tKey);
    return TRUE;
}

// line 4200
void FastAttribMergePermanent(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild)
{
    FastAttribGroup & tGroupParent = sFastAttribGroupGet(idAttribGroupParent);
    FastAttribGroup & tGroupChild = sFastAttribGroupGet(idAttribGroupChild);
    sRecalcDirtyAttribs(tGroupChild);
    FastAttribIterator tIt(idAttribGroupChild, TRUE);
    while (FastAttribIterate(tIt)) {
        AttributeDataType eDataType = KeyGetDataType(tIt.tEntryBuffer.tKey);
        AttributeOperator eOperator = KeyGetAggregationOperator(tIt.tEntryBuffer.tKey);
        FastAttribValue tRunningTotal;
        sFastAttribGroupGetBaseValue(tGroupParent, tIt.tEntryBuffer.tKey, tRunningTotal);
        FastAttribAggregate(tRunningTotal, tIt.tEntryBuffer.tValue, eDataType, eOperator, tIt.tEntryBuffer.tKey);
        sFastAttribGroupSetBaseValue(tGroupParent, tIt.tEntryBuffer.tKey, tRunningTotal);
        sUpdateDirtyKeysList(tGroupParent, tIt.tEntryBuffer.tKey);
    }
}

// line 4230
void FastAttribUnmerge(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild)
{
    if (!FastAttribGroupIsValid(idAttribGroupParent))
        return;
    FastAttribGroup & tGroupParent = sFastAttribGroupGet(idAttribGroupParent);
    FastAttribGroup & tGroupChild = sFastAttribGroupGet(idAttribGroupChild);
    POS pos = tGroupParent.listChildrenAttribGroups.Find(&tGroupChild);
    if (!pos)
        return;
    tGroupParent.listChildrenAttribGroups.RemoveAt(pos);
    tGroupChild.idParentAttribGroup = ATTRIBGROUPID_INVALID;
    sRecalcDirtyAttribs(tGroupChild);
    FastAttribIterator tIt(idAttribGroupChild, TRUE);
    while (FastAttribIterate(tIt))
        sUpdateDirtyKeysList(tGroupParent, tIt.tEntryBuffer.tKey);
}

static const int32 UNSPECIFIED = -2;  // Original line 2424.

// line 4025
float FastAttributeEvaluateBBEIdentifier(int32 nIdentifier, int32 nParam0, int32 nParam1, int32 nParam2, const BBECallbackData & tCallbackData)
{
    IdentifierFastAttribResult tResult;
    FastAttribGetIdentifierAttributeFromParams(tResult, nIdentifier, nParam0, nParam1, nParam2);
    if (tResult.nAttribParam == UNSPECIFIED) {
        AttributeParameterType eParamType = sGetAttribDef(tResult.eAttrib)->eParameterType;
        if ((eParamType == 11 || eParamType == 4) && tCallbackData.snoPower != -1)
            tResult.nAttribParam = tCallbackData.snoPower;
        else
            tResult.nAttribParam = tCallbackData.nAttribParam;
    }
    FastAttribKey tKey = MakeAttribKey(tResult.eAttrib, tResult.nAttribParam);
    FastAttribGroup & tGroup = sFastAttribGroupGet(tCallbackData.idFastAttrib);
    FastAttribValue tTotal;
    switch (tResult.eValue) {
    case ATTRIBVALUE_AGG:
        sFastAttribGroupAggregate(tGroup, tKey, tTotal);
        break;
    case ATTRIBVALUE_BASE:
        sFastAttribGroupGetBaseValue(tGroup, tKey, tTotal);
        break;
    case ATTRIBVALUE_TOTAL:
        tTotal = sFastAttribGroupGetValue(tGroup, tKey);
        break;
    default: DisplayInternalError(0, "", 4067, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    float flRetval;
    switch (KeyGetDataType(tKey)) {
    case ATTRIBUTEDT_FLOAT:
        flRetval = tTotal.flValue;
        break;
    case ATTRIBUTEDT_INT:
        flRetval = (float)tTotal.nValue;
        break;
    default: DisplayInternalError(0, "", 4080, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return flRetval;
}

// line 3016
AttribGroupID FastAttribGroupCreate(FastAttribType eType, int32 nDefaultEntryIndex)
{
    FastAttribGameGlobals * pGlobals = GameGetActiveGameCommonData().ptFastAttribGameGlobals;

    AttribGroupID idFastAttribGroup = pGlobals->pFastAttribGroups->ElementNew();
    FastAttribGroup & tFastAttribGroup = sFastAttribGroupGet(idFastAttribGroup);
    tFastAttribGroup.bLight = FALSE;
    tFastAttribGroup.bDirty = FALSE;
    tFastAttribGroup.eFastAttribType = eType;
    tFastAttribGroup.nDefaultEntryIndex = nDefaultEntryIndex;
    tFastAttribGroup.idParentAttribGroup = ATTRIBGROUPID_INVALID;
    tFastAttribGroup.tCache.mapAttribEntries.SetPool(&pGlobals->tFastAttribMapPool);
    tFastAttribGroup.tBase.mapAttribEntries.SetPool(&pGlobals->tFastAttribMapPool);
    tFastAttribGroup.listChildrenAttribGroups.SetPool(&pGlobals->tFastAttribGroupPtrPool);
    tFastAttribGroup.mapDirtyKeys.SetPool(&pGlobals->tFastAttribMapPool);
    tFastAttribGroup.mapModifiedKeys.SetPool(&pGlobals->tFastAttribMapPool);
    if (eType == 4) {
        tFastAttribGroup.tVeryLargeCache.pmapAttribEntries = new FastAttribMapPooledVeryLarge;
        tFastAttribGroup.tVeryLargeCache.pmapAttribEntries->SetPool(&pGlobals->tFastAttribMapPool);
        tFastAttribGroup.bUseVeryLarge = TRUE;
    } else {
        tFastAttribGroup.bUseVeryLarge = FALSE;
    }
    return idFastAttribGroup;
}
#line 3072
// NON_MATCHING: 696 vs 692 bytes; inlined ElementDeleteByID free-list loop and register allocation differ.

void FastAttribGroupDelete(
    AttribGroupID idAttribGroup)
{
    FastAttribGameGlobals * pGlobals = GameGetActiveGameCommonData().ptFastAttribGameGlobals;



















    FastAttribGroup & tFastAttribGroup = sFastAttribGroupGet(idAttribGroup);
// Cols 26/71 of line 3099 are idParentAttribGroup and the `&&` after the INVALID test.
    if (tFastAttribGroup.idParentAttribGroup != ATTRIBGROUPID_INVALID &&
        FastAttribGroupIsValid(tFastAttribGroup.idParentAttribGroup))
    {
        FastAttribUnmerge(tFastAttribGroup.idParentAttribGroup, idAttribGroup);
    }

    if (tFastAttribGroup.bUseVeryLarge)
    {
        delete tFastAttribGroup.tVeryLargeCache.pmapAttribEntries;
    }

    pGlobals->posAttributeSlowRecalcCounter = NULL;

    pGlobals->pFastAttribGroups->ElementDeleteByID(idAttribGroup);
}
#line 1221
