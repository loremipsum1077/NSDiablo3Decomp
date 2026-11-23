// X/Common/Attributes/FastAttribute.h -- provisional owning header.
//
// FastAttribGetValue / FastAttribLightSetValue are defined in
// X/Common/Attributes/FastAttribute.cpp (_Z18FastAttribGetValuei13FastAttribKey and
// _Z23FastAttribLightSetValuei13FastAttribKey15FastAttribValue) and are called from
// X/Client/CActorCommonData.cpp:65/:69. The full skeleton has not been adopted; this
// carries the two declarations and the AttribGroupID typedef they need.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "FastAttributeStructs.h"
#include "../../Sigma/Unicode.h"
#include "../../Sigma/DataStructures/XTemplate.h"

typedef int32 AttribGroupID;

// X/Common/Attributes/FastAttribute.cpp
FastAttribValue FastAttribGetValue(const AttribGroupID idAttribGroup, const FastAttribKey tKey);
void FastAttribLightSetValue(const AttribGroupID idAttribGroup, const FastAttribKey tKey, const FastAttribValue tValue);

// Called from X/Server/Powers/Power_Enchantress.cpp (_Z17Enchantress_CharmiPv,
// 0x71009695a0, Power_Enchantress.cpp:104). Manglings:
//   _Z13MakeAttribKeyii                            (int, int)
//   _Z21FastAttribSetValueInti13FastAttribKeyi     (int, FastAttribKey, int)
FastAttribKey MakeAttribKey(const int32 eAttrib, const int32 snoPower);  // FastAttribute.cpp
void FastAttribSetValueInt(const AttribGroupID idAttribGroup, const FastAttribKey tKey, const int32 nValue);  // FastAttribute.cpp
int32 FastAttribGetValueInt(AttribGroupID idAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4441
BOOL FastAttribGroupIsValid(AttribGroupID idAttribGroup);  // FastAttribute.cpp:3126
BOOL FastAttribExists(AttribGroupID idAttribGroup, FastAttribKey tKey);

class CRefString;

typedef int32 AttributeDataType;  // line 82

// X/Common/Attributes/FastAttribute.cpp
AttributeDataType KeyGetDataType(FastAttribKey tKey);  // FastAttribute.cpp:2175
CRefString AttribToString(int32 eAttrib);  // FastAttribute.cpp:2279
void FastAttribSetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey, float flValue);  // FastAttribute.cpp:4306
void FastAttribClearModified(AttribGroupID idFastAttrib);  // FastAttribute.cpp:5327

enum {  // line 19
    FAT_UNKNOWN = -1,
    FAT_SIMPLE = 0,
    FAT_ACTOR = 1,
    FAT_ITEM = 2,
    FAT_POWER = 3,
    FAT_COMPLEX_ACTOR = 4,
    NUM_FAT_TYPES = 5,
};

typedef int32 FastAttribType;  // line 34

AttribGroupID FastAttribGroupCreate(FastAttribType eType, int32 nDefaultEntryIndex);  // FastAttribute.cpp:3016
void FastAttribGroupDelete(AttribGroupID idAttribGroup);  /* FastAttribute.cpp:3074 */  AttribGroupID FastAttribLightCreate(FastAttribType eType);  // FastAttribute.cpp:4571

// FastAttribute.cpp:2377,2323.
CRefString AttribParamToString(Attrib eAttrib, int32 nParam);
int32 AttribStringToParam(Attrib eAttrib, LPCSTR szParam);

// FastAttribute.cpp:2251.
int32 StringToAttrib(LPCSTR szAttrib);

#include "AttributeEnums.h"
#include "FastAttribConstants.h"

class AttributeCompressor;
struct FastAttribGroup;
typedef int32 BBEIdentifier;

struct FastAttribEntry {
    FastAttribKey tKey;
    FastAttribValue tValue;
};
static_assert(sizeof(FastAttribEntry) == 0x8, "FastAttribEntry");
struct FastAttribIterator {
    FastAttribIterator(FastAttribGroup *, BOOL);
    FastAttribIterator(AttribGroupID, BOOL);
    FastAttribGroup * ptGroup;
    POS posCurrentSlowMapPosition;
    FastAttribEntry tEntryBuffer;
};
static_assert(sizeof(FastAttribIterator) == 0x18, "FastAttribIterator");
struct FastAttribModifiedIterator {  // size 0x18
    FastAttribModifiedIterator(AttribGroupID);
    FastAttribGroup * ptGroup;  // 0x00
    POS pos;  // 0x08
    FastAttribEntry tEntryBuffer;  // 0x10
};
static_assert(sizeof(FastAttribModifiedIterator) == 0x18, "FastAttribModifiedIterator");
struct IdentifierFastAttribResult {
    BBEIdentifier eIdentifier;
    Attrib eAttrib;
    int32 nAttribParam;
    AttributeValue eValue;
    AttributeSource eSource;
};
static_assert(sizeof(IdentifierFastAttribResult) == 0x14, "IdentifierFastAttribResult");

Attrib KeyGetAttrib(FastAttribKey tKey);
int32 KeyGetParam(FastAttribKey tKey);
AttributeOperator KeyGetAggregationOperator(FastAttribKey tKey);
AttributeParameterType KeyGetParamType(FastAttribKey tKey);
const AttributeCompressor * KeyGetCompressor(FastAttribKey tKey);
uint8 KeyGetSyncFlags(FastAttribKey tKey);
BOOL FastAttribIterate(FastAttribIterator & tIt);
FastAttribValue FastAttribGetBaseValue(AttribGroupID idAttribGroup, FastAttribKey tKey);
void FastAttribSetValue(AttribGroupID idAttribGroup, FastAttribKey tKey, FastAttribValue tValue);
int32 XMLStringToAttrib(LPCSTR szAttrib);
CRefString AttribToXMLString(int32 eAttrib);
int32 & AttribValueToInt(FastAttribValue & tValue);
float & AttribValueToFloat(FastAttribValue & tValue);
void FastAttribAggregate(FastAttribValue & tRunningTotal, FastAttribValue tValue,
    AttributeDataType eDataType, AttributeOperator eOperator, const FastAttribKey & tKey);
BOOL FastAttribValueWentAboveZero(FastAttribKey tKey, FastAttribValue tLastValue, FastAttribValue tValue);
void FastAttribGetIdentifierAttributeFromParams(IdentifierFastAttribResult & tResult,
    int32 nIdentifier, int32 nParam0, int32 nParam1, int32 nParam2);

enum {  // line 35
    ATTRIBGROUPID_INVALID = -1,
};

// X/Common/Attributes/FastAttribute.cpp (called from X/Common/ActorAttributes.cpp).
FastAttribGroup * FastAttribGroupGet(AttribGroupID idAttribGroup);  // FastAttribute.cpp:3117
void FastAttribMerge(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild);  // FastAttribute.cpp:4175
void FastAttribMergePermanent(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild);  // FastAttribute.cpp:4200
void FastAttribUnmerge(AttribGroupID idAttribGroupParent, AttribGroupID idAttribGroupChild);  // FastAttribute.cpp:4230
void FastAttribSetValueInt(FastAttribGroup * ptAttribGroup, FastAttribKey tKey, int32 nValue);  // FastAttribute.cpp:4328
void FastAttribSetValueFloat(FastAttribGroup * ptAttribGroup, FastAttribKey tKey, float flValue);  // FastAttribute.cpp:4345
int32 FastAttribGetValueInt(FastAttribGroup * ptAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4473
int32 FastAttribGetValueInt(AttribGroupID idAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4441
float FastAttribGetValueFloat(AttribGroupID idAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4452
float FastAttribGetValueFloat(FastAttribGroup * ptAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4488
float FastAttribGetBaseValueFloat(FastAttribGroup * ptAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4521
BOOL FastAttribExists(FastAttribGroup * ptAttribGroup, const FastAttribKey tKey);  // FastAttribute.cpp:5212

int32 FastAttribGetValueInt(AttribGroupID idAttribGroup, FastAttribKey tKey);  // FastAttribute.cpp:4441

// Declaration placement inferred from the owning API; DWARF definition: FastAttribute.cpp:5351.
BOOL FastAttribModifiedIterate(FastAttribModifiedIterator & tIt);

// Owning-header placement inferred; full raw DWARF search records only FastAttribute.cpp:5142.
CRefString FastAttribToString(const FastAttribKey tKey);
