// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/TagMaps/Tags.h
// CTagMap layout and the APIs used by descriptor serialization.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "TagNumbers.h"
#include "../GameBalance.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/DataStructures/XTemplate.h"
#include "../../Sigma/Files/XSerializeData.h"

#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XPooledMap.h"
typedef XPooledMap<unsigned int, unsigned int, TagVariant, TagVariant, 16> TTagMap;

enum {
    TAGMAP_FLAG_INITIALIZED = 0,
    TAGMAP_FLAG_READONLY = 1,
};

class CTagMap {  // size 0xb8
public:
    uint32 m_dwFlags : 2;  // 0x00
    uint32 m_dwRefCount : 25;  // 0x00
    uint32 dwTagBase : 5;  // 0x00
    inline void SetTagBase(TagBases eTagBase) { dwTagBase = eTagBase >> 16; }  // Original line 30
    inline TagBases GetTagBase() const { return (TagBases)(dwTagBase << 16); }  // Original line 36
    TTagMap m_TagMap;  // 0x08
    uint32 m_dwCRC;  // 0xb0
};
static_assert(sizeof(CTagMap) == 0xb8, "CTagMap");



class CTagMapRef {  // size 0x8
public:
    inline CTagMapRef() {
        m_ptTagMap = NULL;
    }
    inline CTagMapRef(CTagMap * ptTagMap) {  // line 85
        m_ptTagMap = ptTagMap;
    }
    inline CTagMap * GetTagMap() const {  // line 89
        return m_ptTagMap;
    }
    BOOL operator==(const CTagMapRef & tagMapRef) const;
private:
    CTagMap * m_ptTagMap;  // 0x00
};
static_assert(sizeof(CTagMapRef) == 0x8, "CTagMapRef");

typedef XGrowableMap<CTagMapRef, const CTagMapRef &, CTagMapRef, const CTagMapRef &> TGlobalTagMapRefMap;

typedef XMapMemoryPool<unsigned int, TagVariant> TTagMemoryPool;

struct TagMapPools {  // size 0xe0
    TTagMemoryPool tTagMemoryPool;  // 0x00
    XChainedFixedBlockAllocator<CTagMap> poolTagMaps;  // 0x30
    TGlobalTagMapRefMap tGlobalTagMapRef;  // 0x60
};
static_assert(sizeof(TagMapPools) == 0xe0, "TagMapPools");

typedef XGrowableMap<unsigned int, unsigned int, const AxeTagInfo *, const AxeTagInfo *> TagInfoMap;

struct TagMapAppGlobals {  // size 0xa60
    TagInfoMap tTagInfoMaps[19];  // 0x00
    TagMapPools tAppPools;  // 0x980
};
static_assert(sizeof(TagMapAppGlobals) == 0xa60, "TagMapAppGlobals");

struct StringFormula {  // size 0x20
    StringFormula();
    void Copy(const StringFormula & tSrcStringFormula);
    void Set(LPCSTR szFormula);
    void Compile();
    void Dispose();
    void DisposeFormula();
    XPtr64<char> szFormulaString;  // 0x00
    XPtr64<unsigned char> pbFormula;  // 0x08
    SerializeData serFormulaString;  // 0x10
    SerializeData serFormula;  // 0x18
};
static_assert(sizeof(StringFormula) == 0x20, "StringFormula");

struct StringFormulaDefault {  // size 0x48
    StringFormulaDefault(LPCSTR);
    StringFormula tFormula;  // 0x00
    uint8 arFormula[32];  // 0x20
    BOOL bCompiled;  // 0x40
    void Compile();
};
static_assert(sizeof(StringFormulaDefault) == 0x48, "StringFormulaDefault");

// Declared here from their definitions in Tags.cpp (:607, :1040, :1084, :1107 and :1252).
BOOL TagMapTagExists(const CTagMap * pTagMap, uint32 dwTag);
int32 TagMapGetInt(const CTagMap * pTagMap, uint32 dwTag);
uint32 TagMapGetDWORD(const CTagMap * pTagMap, uint32 dwTag);
float TagMapGetFloat(const CTagMap * pTagMap, uint32 dwTag);
SNO TagMapGetSNO(const CTagMap * pTagMap, uint32 dwTag);

// Declared here from their definitions in Tags.cpp:563 and :586.
const AxeTagInfo * TagMapGetTagInfo(const CTagMap * pTagMap, const uint32 dwTagId);
const AxeTagInfo * TagMapGetTagInfo(const TagBases eTagBase, const uint32 dwTagId);

// Declared here from their definitions in Tags.cpp (:473 and :489). The
// iteration position is the `POS` the DWARF records (typedef uint32 * POS,
// Sigma/DataStructures/XTemplate.h:5): TagMapGetStartPosition returns it and
// TagMapGetNextTag advances it by reference
// (_Z16TagMapGetNextTagPK7CTagMapRPj).
POS TagMapGetStartPosition(const CTagMap * pTagMap);
uint32 TagMapGetNextTag(const CTagMap * pTagMap, POS & pos);

// Original Tags.cpp declarations required by descriptor serialization.
CTagMap * TagMapCreateEmpty(TagBases eTagBase);
CTagMap * TagMapCreate(TagBases eTagBase);
void TagMapRegisterGlobal(CTagMap * pTagMap);
CTagMap * TagMapFindIdenticalGlobal(CTagMap * pTagMap);
void TagMapDispose(CTagMap *& pTagMap);
void TagMapDispose(XPtr64<CTagMap> & pTagMap);
int32 TagMapGetSerialLength(const CTagMap * pTagMap, uint32 dwFlags);
void TagMapSerialize(const CTagMap * pTagMap, void * pBlock, uint32 dwFlags);
void TagMapUnserialize(CTagMap * pTagMap, void * pBlock);
int32 TagMapGetTagCount(const CTagMap * pTagMap);
void TagMapIterateSnoReferences(const CTagMap * pTagMap, SNOIterateFunc pfuncIterate, void * pData);
void TagMapChangeSNOReference(CTagMap * pTagMap, const SNOName & snonameOld, SNO snoNew);
StringFormula * TagMapGetFormula(const CTagMap * pTagMap, uint32 dwTag);
GBID TagMapGetGBID(const CTagMap * pTagMap, uint32 dwTag);
CTagMap * TagMapRef(CTagMap * pTagMap);
BOOL TagMapCanBeRefed(const CTagMap * pTagMap);

const XTypeDescriptor * TagGetDefinitionDataType(AxeTagType eType);
TagVariantType TagGetVariantType(AxeTagType eType);

void TagMapRemoveTag(CTagMap * pTagMap, uint32 dwTag);  // :642

void TagMapRemoveAll(CTagMap * pTagMap);  // :673

void TagMapSetInt(CTagMap * pTagMap, uint32 dwTag, int32 nValue);  // :1066

uint8 * TagMapGetBBEFormula(const CTagMap * pTagMap, uint32 dwTag, uint32 & dwSize);  // :1162

LPCSTR TagMapGetFormulaString(const CTagMap * pTagMap, uint32 dwTag);  // :1189

SNO TagMapGetDefaultSNO(const CTagMap * pTagMap, const uint32 dwTag);  // :1363

GBID TagMapGetDefaultGBID(const CTagMap * pTagMap, const uint32 dwTag);  // :1388

BOOL TagMapCompare(const CTagMap * pTagMap1, const CTagMap * pTagMap2);  // :1612

TagVariantType TagGetVariantType(const CTagMap * pTagMap, const uint32 dwTag);  // Tags.cpp:821
int32 TagMapGetInheritedValueInt(const CTagMap * ptTagMapDef, const CTagMap * ptTagMapMarker,
                                 const uint32 dwActorTag, const uint32 dwMarkerTag);  // :1939
float TagMapGetInheritedValueFloat(const CTagMap * ptTagMapDef, const CTagMap * ptTagMapMarker,
                                   const uint32 dwActorTag, const uint32 dwMarkerTag);  // :1960
