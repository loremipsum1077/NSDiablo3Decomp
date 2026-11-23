// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/TypeDescriptor/TypeDescriptor.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
// The original includes "../../Common/SNO/SNOFiles.h" here. Only SNOElement is needed (as a pointer
// in UnserializeData), and including it currently costs a dead SNONAME_NONE initialiser in every
// file's _GLOBAL__sub_I (SNOName.h), which the original does not have; see TypeDescriptorField.cpp.
struct SNOElement;
#include "../DataStructures/XGrowableList.h"
#include "../DataStructures/XGrowableMap.h"
#include "../Networking/XDecoder.h"
#include "../RefString.h"
#include "../SigmaTypes.h"
#include "PointerSnapshot.h"
#include "TypeDescriptorField.h"
#include "../ErrorManager/ErrorManager.h"
#include "../Memory/SigmaMemory.h"
#include "../Unicode.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct BBETranslationData;
class CXMLNode;
struct RNG;
class SAX;
struct SNOName;
class TypeDescriptorField;
class TypeDescriptorFieldSerializationInfo;
class XEncoder;
class XMemoryAllocatorInterface;
class TDVariant;
class XTypeDescriptor;
class ComplexTypeDescriptor;

typedef int32 TagVariantType;

typedef int32 SNO;

typedef int32 SNOGroup;

typedef int32 (*DOMVersionFunc)(const int32, const CXMLNode &, void *);

typedef TDVariant (*ToVariantFunc)(const void *);

typedef BOOL (*FromVariantFunc)(void *, const TDVariant &);

typedef BOOL (*VariantsEqualFunc)(const TDVariant &, const TDVariant &);

typedef void (*LoadFromDefaultDataFunc)(const TypeDescriptorField &, void *);

struct MergeableSerializeDataInfo {  // size 0x30
    MergeableSerializeDataInfo(uint32, uint32, const XTypeDescriptor *, const PointerSnapshot::PointerVector &, void *);
    MergeableSerializeDataInfo(const MergeableSerializeDataInfo &);
    MergeableSerializeDataInfo();
    ~MergeableSerializeDataInfo();
    BOOL IsIdentical(uint32, const XTypeDescriptor *, void *, uint32 &);
    uint32 GetOffset() const;
    uint32 GetSize() const;
    void * GetData() const;
    const XTypeDescriptor * GetType() const;
private:
    uint32 m_dwOffset;  // 0x00
    uint32 m_dwSize;  // 0x04
    void * m_lpData;  // 0x08
    const XTypeDescriptor * m_ptType;  // 0x10
    PointerSnapshot::PointerVector m_tPointers;  // 0x18
};
static_assert(sizeof(MergeableSerializeDataInfo) == 0x30, "MergeableSerializeDataInfo");

typedef XGrowableList<MergeableSerializeDataInfo, const MergeableSerializeDataInfo &> MergeableSerializeDataInfoList;

typedef XGrowableMap<unsigned int, unsigned int, unsigned long, unsigned long> MergedOffsetMap;

enum {
    SI_CAN_BE_NONE = 0,
    SI_SOFT_LINK = 1,
    SI_OVERRIDE = 2,
    SI_CONSOLE_ONLY = 3,
    SI_PC_ONLY = 4,
};

typedef void (*SNOIterateFunc)(const SNOName &, const uint32, void *);

typedef void (*FieldIterateFunc)(const XTypeDescriptor *, const TypeDescriptorField *, const void *, void *);

enum TypeDescriptorFlags {
    TD_BEING_SCANNED_FOR_FIELDS = 0,
    TD_SCANNED_FOR_FIELDS = 1,
    TD_CONTAINS_SNO_SUBFIELD = 2,
    TD_CONTAINS_ALLOCATION_SUBFIELD = 3,
    TD_CONTAINS_SERVER_ONLY_SUBFIELD = 4,
    TD_CONTAINS_64BIT_SUBFIELD = 5,
    TD_CONTAINS_128BIT_SUBFIELD = 6,
    TD_IS_COMPLEX_TYPE = 7,
    TD_IGNORE_AXE_TO_PLATFORM_SIZE_VALIDATION = 8,
};

enum INPUT_TYPE {
    INPUT_TYPE_TEXT = 0,
    INPUT_TYPE_INT = 1,
    INPUT_TYPE_FLOAT = 2,
};

struct UnserializeData {  // size 0x28
    SNOElement * ptSNOElement;  // 0x00
    void * lpDefinitionBase;  // 0x08
    uint32 dwDefinitionSize;  // 0x10
    XMemoryAllocatorInterface * pMemoryInterface;  // 0x18
    MergedOffsetMap * ptMergedOffsetMap;  // 0x20
};
static_assert(sizeof(UnserializeData) == 0x28, "UnserializeData");

struct DebugDumpData {  // size 0x20
    CRefString sLabel;  // 0x00
    FILE * fileOut;  // 0x18
};
static_assert(sizeof(DebugDumpData) == 0x20, "DebugDumpData");

enum TDVariantSource {
    TDVARIANT_SOURCE_INVALID = 0,
    TDVARIANT_SOURCE_INT32 = 1,
    TDVARIANT_SOURCE_UINT32 = 2,
    TDVARIANT_SOURCE_INT64 = 3,
    TDVARIANT_SOURCE_UINT64 = 4,
    TDVARIANT_SOURCE_FLOAT = 5,
    TDVARIANT_SOURCE_STRING = 6,
};

class TDVariant {  // size 0x28
public:
    TDVariant();
    explicit TDVariant(int32);
    explicit TDVariant(uint32);
    explicit TDVariant(int64);
    explicit TDVariant(uint64);
    explicit TDVariant(float);
    explicit TDVariant(LPCSTR);
    explicit TDVariant(LPCSTR, float);
    explicit TDVariant(LPCSTR, uint32);
    explicit TDVariant(LPCSTR, int32);
    explicit TDVariant(LPCSTR, uint64);
    explicit TDVariant(LPCSTR, int64);
    ~TDVariant();
    uint32 GetUInt() const;
    uint64 GetUInt64() const;
    float GetFloat() const;
    int32 GetInt() const;
    int64 GetInt64() const;
    uintptr GetUIntPtr() const;
    intptr GetIntPtr() const;
    LPCSTR GetStr() const;
    CRefString GetRefStr() const;
    void Set(int32 nValue);
    void Set(uint32 uValue);
    void Set(int64);
    void Set(uint64);
    void Set(float flValue);
    void Set(LPCSTR sValue);
    void Set(LPCSTR sValue, uint32 uValue);
    void Set(LPCSTR sValue, int32 nValue);
    void Set(LPCSTR sValue, uint64 uValue);
    void Set(LPCSTR sValue, int64 nValue);
    void Set(LPCSTR sValue, float flValue);
    BOOL IsEqual(const TDVariant &, const XTypeDescriptor *) const;
    TDVariantSource GetSource() const { return m_eSource; } // Original line 229.
private:
    BOOL operator==(const TDVariant &);
    BOOL operator!=(const TDVariant &);
protected:
    union {  // size 0x8
        int32 m_int32;  // 0x00
        uint32 m_uint32;  // 0x00
        int64 m_int64;  // 0x00
        uint64 m_uint64;  // 0x00
        float m_float;  // 0x00
    } m_value;  // 0x00
    CRefString m_sValue;  // 0x08
    TDVariantSource m_eSource;  // 0x20
};
static_assert(sizeof(TDVariant) == 0x28, "TDVariant");

// Inline virtual defaults reconstructed from the copies emitted by D3DescribedType.cpp.
// Error locations retain the original TypeDescriptor.h line numbers.
class XTypeDescriptor {  // size 0x38
    // vtable pointer
protected:
    static int32 s_nIgnoringFieldCount;
    static int32 s_nServerOnlyFieldCount;
public:
    LPCSTR szClassName;  // 0x08
    int32 nVersion;  // 0x10
    const TypeDescriptorField * arFields;  // 0x18
    uint32 nFieldCount;  // 0x20
    DOMVersionFunc pfuncDOMVersion;  // 0x28
    uint32 dwFlags;  // 0x30
    XTypeDescriptor(LPCSTR szClassName);
    // Inline and empty in the original (line 280; the emitted D2 is a bare ret). It must stay
    // empty here: static ComplexTypeDescriptor objects then need no __cxa_atexit registration.
    virtual ~XTypeDescriptor() {}  // vtable slot 0
    const TypeDescriptorField & GetField(int32 nIndex) const;
    const TypeDescriptorField & GetFieldByName(LPCSTR) const;
    const TypeDescriptorField & GetFieldByNameRecursive(LPCSTR, int32 &) const;
    const TypeDescriptorField & GetFirstFieldByType(const XTypeDescriptor *) const;
    const TypeDescriptorField & GetFirstFieldByTypeRecursive(const XTypeDescriptor *, int32 &) const;
    LPCSTR GetName() const;
    virtual INPUT_TYPE GetInputType() const { return INPUT_TYPE_TEXT; }  // vtable slot 2
    virtual int32 SizeOf() const = 0;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const { return CRefString().Format("<%s>", GetName()); }  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void *, const CRefString &) const { DisplayInternalError(NULL, "", 323, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void *, CRefString &) const { DisplayInternalError(NULL, "", 332, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString &, void *) const { DisplayInternalError(NULL, "", 341, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 7
    virtual TDVariant ToVariant(const void *) const { DisplayInternalError(NULL, "", 347, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 8
    virtual BOOL FromVariant(void *, const TDVariant &) const { DisplayInternalError(NULL, "", 355, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 9
    virtual BOOL VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const;  // vtable slot 10
    virtual BOOL FromStringFull(const TypeDescriptorField & tField, const BBETranslationData &, void * pData, const CRefString & szData) const { FromString(tField, pData, szData); return !(szData == ""); }  // vtable slot 11
    // Original TypeDescriptor.h:378, defined in the class (its DWARF definition has no decl_line of
    // its own). Shape from its abstract DIE: nSize 382 and pDefaultData 383 at function scope, i (386)
    // in the block of the !pDefaultData branch, field (389) in the loop body; SigmaMemoryCopy at 396
    // comes last. Inlined into ComplexTypeDescriptor::LoadFromDefaultData (call line 748); the
    // TypeDescriptor.cpp CU records a linker-removed copy (low_pc 0, 0x9c bytes; ours is 0x9c too).
    // Loop form: `while (!GetField(i++).IsNull())` + GetField(i - 1) differs in 4 instructions once
    // inlined into the override; this form gives the original's loop (h16, Nov 22).
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const  // vtable slot 12
    {
        int32 nSize = SizeOf();
        const void * pDefaultData = tField.GetDefaultData();
        if (!pDefaultData)
        {
            int32 i = 0;
            // Original column 11 at line 387 puts the GetField call right after "while (": no '!'.
            while (GetField(i).IsNull() == FALSE)
            {
                const TypeDescriptorField & field = GetField(i);
                field.GetTypeDesc()->LoadFromDefaultData(field, field.Offset(pData));
                // Inferred: no line row of its own (nor at the original's 391); the loop needs it.
                i++;
            }
        }
        else
        {
            SigmaMemoryCopy(pData, pDefaultData, nSize);
        }
    }
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const { return FALSE; }  // vtable slot 13
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField &) const { return FALSE; }  // vtable slot 14
    virtual BOOL ContainsServerOnlySubfields(const TypeDescriptorField &) const { return FALSE; }  // vtable slot 15
    virtual void ScanForSubfields(const TypeDescriptorField &) const {  }  // vtable slot 16
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void IterateSNOReferences(const void *, const TypeDescriptorField &, SNOIterateFunc, void *) const {  }  // vtable slot 19
    virtual void IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const { pfuncIterate(this, &field, lpDefinition, pData); }  // vtable slot 20
    virtual void ChangeSNOReference(const void *, const TypeDescriptorField &, const SNOName &, const SNO) const {  }  // vtable slot 21
    virtual void DebugDump(const void *, const TypeDescriptorField &, DebugDumpData *) const {  }  // vtable slot 22
    virtual TagVariantType GetTagVariantType() const { return -1; }  // vtable slot 23
    virtual void Byteswap(void *, void *, void *, const TypeDescriptorField &) const { DisplayInternalError(NULL, "", 479, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 24
    virtual void ByteswapInPlace(void *, void *, const TypeDescriptorField &) const { DisplayInternalError(NULL, "", 487, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 25
    void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc) const;
    void ByteswapInPlace(void * lpDefinition) const;
    virtual void AdjustSerializeDataSize(void *, const TypeDescriptorField &, const BOOL) const {  }  // vtable slot 26
    virtual void ComputeOffsets(void *, const TypeDescriptorField &, uint32 &) const {  }  // vtable slot 27
    virtual void ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList &) const { ComputeOffsets(lpDefinition, field, dwOffset); }  // vtable slot 28
    virtual void ComputeMergedOffsets(void *, const TypeDescriptorField &, uint32 &) const {  }  // vtable slot 29
    virtual void SerializeToBinary(void *, const TypeDescriptorField &, uint8 *, const uint32, const uint32) const { DisplayInternalError(NULL, "", 546, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 30 (line 539)
    virtual void SerializeFromBinary(void *, const UnserializeData &, const TypeDescriptorField &) const { DisplayInternalError(NULL, "", 554, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }  // vtable slot 31 (line 549)
    virtual void Delete(const uint32, const void *, const TypeDescriptorField &, XMemoryAllocatorInterface *) const {  }  // vtable slot 32
    virtual BOOL Encode(void *, const TypeDescriptorField &, XEncoder &) const = 0;  // vtable slot 33
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const = 0;  // vtable slot 34
    virtual void Fuzz(void *, const TypeDescriptorField &, RNG &) const = 0;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField &) const = 0;  // vtable slot 36
    virtual void CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const;  // vtable slot 37
    virtual BOOL Contains64BitSubfields(const TypeDescriptorField &) const { return FALSE; }  // vtable slot 38
    virtual uint32 GetSub64BitPlatformMask(const TypeDescriptorField &) const { return 0; }  // vtable slot 39
    virtual BOOL Contains128BitSubfields(const TypeDescriptorField &) const { return FALSE; }  // vtable slot 40
};
static_assert(sizeof(XTypeDescriptor) == 0x38, "XTypeDescriptor");

typedef XGrowableList<const ComplexTypeDescriptor *, const ComplexTypeDescriptor *> ComplexTypeDescriptorList;

class ComplexTypeDescriptor : public XTypeDescriptor {  // size 0x60
public:
    ToVariantFunc pfuncToVariant;  // 0x38
    FromVariantFunc pfuncFromVariant;  // 0x40
    VariantsEqualFunc pfuncVariantsEqual;  // 0x48
    LoadFromDefaultDataFunc pfuncLoadFromDefaultData;  // 0x50
    ComplexTypeDescriptor(LPCSTR szClassName, int32 nVersion, const TypeDescriptorField * arFields, uint32 nFieldCount, DOMVersionFunc pfuncDOMVersion, uint32 dwFlags, ToVariantFunc pfuncToVariant, FromVariantFunc pfuncFromVariant, VariantsEqualFunc pfuncVariantsEqual, LoadFromDefaultDataFunc pfuncLoadFromDefaultData);
    // Original lines 729-814 define these in the class body. Being inline at the class definition
    // is what makes ScanForSubfields the key function, so the vtable (and the D0) is emitted by
    // TypeDescriptor.cpp. Bodies are reconstructed from the out-of-line copies at 0x7100605790..
    // (TypeExtensions.cpp); the error lines are the original's.
    // The original keeps TypeExtensions.cpp's copies of these inline members (its CU holds their
    // addresses 0x7100605790..; TypeDescriptor.cpp's SizeOf/LoadFromDefaultData have low_pc 0 and
    // inline GetField): toolchain/weak-providers.json names TypeExtensions.cpp.obj for them.
    virtual int32 SizeOf() const  // vtable slot 3
    {
        return GetField(nFieldCount - 1).nDataOffset;
    }

    // Original lines 737-750: the callback, else the base implementation (inlined at 748).
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const  // vtable slot 12
    {
        if (pfuncLoadFromDefaultData)
        {
            pfuncLoadFromDefaultData(tField, pData);
        }
        else
        {
            XTypeDescriptor::LoadFromDefaultData(tField, pData);
        }
    }

    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField & field) const  // vtable slot 13
    {
        if (!(dwFlags & (1 << TD_SCANNED_FOR_FIELDS)))
            ScanForSubfields(field);
        return (dwFlags >> TD_CONTAINS_SNO_SUBFIELD) & 1;
    }

    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField & field) const  // vtable slot 14
    {
        if (!(dwFlags & (1 << TD_SCANNED_FOR_FIELDS)))
            ScanForSubfields(field);
        return (dwFlags >> TD_CONTAINS_ALLOCATION_SUBFIELD) & 1;
    }

    BOOL ShouldIgnoreAxeToPlatformSizeValidation() const;

    virtual BOOL ContainsServerOnlySubfields(const TypeDescriptorField & field) const  // vtable slot 15
    {
        if (!(dwFlags & (1 << TD_SCANNED_FOR_FIELDS)))
            ScanForSubfields(field);
        return (dwFlags >> TD_CONTAINS_SERVER_ONLY_SUBFIELD) & 1;
    }

    virtual TDVariant ToVariant(const void * pData) const  // vtable slot 8
    {
        if (pfuncToVariant)
            return pfuncToVariant(pData);
        DisplayInternalError(NULL, "", 798, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const  // vtable slot 9
    {
        if (pfuncFromVariant)
            return pfuncFromVariant(pData, tValue);
        DisplayInternalError(NULL, "", 810, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    virtual BOOL VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const  // vtable slot 10
    {
        if (pfuncVariantsEqual)
            return pfuncVariantsEqual(tValue1, tValue2);
        DisplayInternalError(NULL, "", 822, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    virtual void ScanForSubfields(const TypeDescriptorField &) const;  // vtable slot 16
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const;  // vtable slot 20
    virtual void ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const;  // vtable slot 21
    virtual void DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const;  // vtable slot 22
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const;  // vtable slot 26
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const;  // vtable slot 28
    virtual void ComputeMergedOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 29
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual void Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 32
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const { field.nBitCount = 0; }  // vtable slot 36
    virtual void CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const;  // vtable slot 37
    static ComplexTypeDescriptor * GetRegistrationHead();
    ComplexTypeDescriptor * GetNextRegisteredDescriptor();
    static void ValidateDescriptors(const ComplexTypeDescriptorList &);
    static void GenerateValidationFile(const ComplexTypeDescriptorList &);
    static CRefString GetValidationLooseFilename();
    static CRefString GetValidationMPQFilename();
    virtual BOOL Contains64BitSubfields(const TypeDescriptorField & field) const
    {
        if (!(dwFlags & (1 << TD_SCANNED_FOR_FIELDS)))
            ScanForSubfields(field);
        return (dwFlags >> TD_CONTAINS_64BIT_SUBFIELD) & 1;
    }  // vtable slot 38
    virtual uint32 GetSub64BitPlatformMask(const TypeDescriptorField & field) const;  // vtable slot 39
    virtual BOOL Contains128BitSubfields(const TypeDescriptorField & field) const
    {
        if (!(dwFlags & (1 << TD_SCANNED_FOR_FIELDS)))
            ScanForSubfields(field);
        return (dwFlags >> TD_CONTAINS_128BIT_SUBFIELD) & 1;
    }  // vtable slot 40
private:
    static ComplexTypeDescriptor * pRegistrationHead;
    ComplexTypeDescriptor * pNextRegisteredDescriptor;  // 0x58
};
static_assert(sizeof(ComplexTypeDescriptor) == 0x60, "ComplexTypeDescriptor");

class VariantTypeDescriptor : public ComplexTypeDescriptor {  // size 0x68
public:
    VariantTypeDescriptor(LPCSTR, int32, const TypeDescriptorField *, uint32, DOMVersionFunc, uint32, ToVariantFunc, FromVariantFunc, VariantsEqualFunc, int32);
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const;  // vtable slot 20
    virtual void ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const;  // vtable slot 21
    virtual void DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const;  // vtable slot 22
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const;  // vtable slot 26
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const;  // vtable slot 28
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual void Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 32
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    int32 GetVariantValue(const void * lpDefinition) const;
protected:
    int32 nVariantOffset;  // 0x60
};
static_assert(sizeof(VariantTypeDescriptor) == 0x68, "VariantTypeDescriptor");


// Original line 211. Body reconstructed from its inlined copies (e.g. sQuatFromVariant in
// X/Sigma/Math/Quaternion.cpp): a call to CRefString::str on m_sValue.
inline LPCSTR TDVariant::GetStr() const
{
    return m_sValue.str();
}

// The original defines these inline further down this header (lines 269 and 701). Bodies are
// reconstructed from the static initialisers that inline them (e.g. _GLOBAL__sub_I_Plane.cpp,
// _GLOBAL__sub_I_DescribedType.cpp).
inline XTypeDescriptor::XTypeDescriptor(LPCSTR szClassName)
    : szClassName(szClassName)
    , nVersion(0)
    , arFields(NULL)
    , nFieldCount(0)
    , pfuncDOMVersion(NULL)
    , dwFlags(0)
{
}

inline ComplexTypeDescriptor::ComplexTypeDescriptor(LPCSTR szClassName, int32 nVersion, const TypeDescriptorField * arFields, uint32 nFieldCount,
                                                    DOMVersionFunc pfuncDOMVersion, uint32 dwFlags, ToVariantFunc pfuncToVariant,
                                                    FromVariantFunc pfuncFromVariant, VariantsEqualFunc pfuncVariantsEqual,
                                                    LoadFromDefaultDataFunc pfuncLoadFromDefaultData)
    : XTypeDescriptor(szClassName)
{
    this->nVersion = nVersion;
    this->arFields = arFields;
    this->nFieldCount = nFieldCount;
    this->pfuncDOMVersion = pfuncDOMVersion;
    // Every registered complex type in the binary has exactly this bit set (0x80).
    this->dwFlags = dwFlags | (1 << TD_IS_COMPLEX_TYPE);
    this->pfuncToVariant = pfuncToVariant;
    this->pfuncFromVariant = pfuncFromVariant;
    this->pfuncVariantsEqual = pfuncVariantsEqual;
    this->pfuncLoadFromDefaultData = pfuncLoadFromDefaultData;
    pNextRegisteredDescriptor = pRegistrationHead;
    pRegistrationHead = this;
}

// Declared in the DWARF without bodies or an out-of-line copy; these bodies are written for
// NetworkInitializeForApp (X/Common/Networking/Network.cpp:226-240), which walks the registration list.
inline ComplexTypeDescriptor * ComplexTypeDescriptor::GetRegistrationHead() { return pRegistrationHead; }
inline ComplexTypeDescriptor * ComplexTypeDescriptor::GetNextRegisteredDescriptor() { return pNextRegisteredDescriptor; }

inline VariantTypeDescriptor::VariantTypeDescriptor(LPCSTR szClassName, int32 nVersion, const TypeDescriptorField * arFields, uint32 nFieldCount,
                                                    DOMVersionFunc pfuncDOMVersion, uint32 dwFlags, ToVariantFunc pfuncToVariant,
                                                    FromVariantFunc pfuncFromVariant, VariantsEqualFunc pfuncVariantsEqual, int32 nVariantOffset)
    : ComplexTypeDescriptor(szClassName, nVersion, arFields, nFieldCount, pfuncDOMVersion, dwFlags, pfuncToVariant, pfuncFromVariant,
                            pfuncVariantsEqual, NULL)
{
    this->nVariantOffset = nVariantOffset;
}

// The built-in descriptors, defined in DescribedType.cpp (lines 742-767). Where the original
// declares them is not in the DWARF; they are declared here so every user of a field table sees them.
extern const XTypeDescriptor * DT_NULL;
// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp:321, not in DescribedType.cpp;
// declared here alongside the other built-ins (the sibling 1.0 build reaches it through
// X/Sigma/TypeDescriptor/DescribedType.h, which does not exist under src/ here).
extern const XTypeDescriptor * DT_SNO;
// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp:729, likewise declared here
// (the sibling 1.0 build declares it in its TypeDescriptor.h).
extern const XTypeDescriptor * DT_TIME;
// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp:2414, likewise declared here
// (the sibling 1.0 build declares it in X/Sigma/TypeDescriptor/DescribedType.h).
extern const XTypeDescriptor * DT_MASS;
// Defined in X/Sigma/Files/XFilesCommon.cpp:20, likewise declared here
// (the sibling 1.0 build declares it in its TypeDescriptor.h).
extern const XTypeDescriptor * DT_SERIALIZEDATA;
extern const XTypeDescriptor * DT_INT;
extern const XTypeDescriptor * DT_DATAID;
extern const XTypeDescriptor * DT_INT64;
extern const XTypeDescriptor * DT_BYTE;
extern const XTypeDescriptor * DT_FLOAT;
extern const XTypeDescriptor * DT_STRING;
extern const XTypeDescriptor * DT_WORD;
extern const XTypeDescriptor * DT_CHARARRAY;
extern const XTypeDescriptor * DT_ANGLE;
extern const XTypeDescriptor * DT_PERCENT;
extern const XTypeDescriptor * DT_HIGHPRECISIONPERCENT;
extern const XTypeDescriptor * DT_ENUM;
extern const XTypeDescriptor * DT_FLAG;
extern const XTypeDescriptor * DT_TRANSLATEABLE;
extern const XTypeDescriptor * DT_FIXEDARRAY;
extern const XTypeDescriptor * DT_VARIABLEARRAY;
extern const XTypeDescriptor * DT_VARIABLEARRAY_COUNT;
extern const XTypeDescriptor * DT_CSTRING;
extern const XTypeDescriptor * DT_OPTIONAL;
extern const XTypeDescriptor * DT_IVECTOR2D;
extern const XTypeDescriptor * DT_VECTOR2D;
extern const XTypeDescriptor * DT_VECTOR3D;
extern const XTypeDescriptor * DT_VECTOR4D;
extern const XTypeDescriptor * DT_RGBACOLOR;
extern const XTypeDescriptor * DT_RGBACOLORVALUE;
// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp (lines 1083 and 3275); declared here so
// the D3 field constructors that build a tag-map and a formula field by name can see them.
extern const XTypeDescriptor * DT_FORMULA;
extern const XTypeDescriptor * DT_TAGMAP;

// Functions defined in this file (bodies are not in the DWARF):
//   line   211  LPCSTR TDVariant::GetStr() const  [inlined into 15 functions]
//   line   229  TDVariantSource TDVariant::GetSource() const  [inlined into 9 functions]
//   line   269  XTypeDescriptor::XTypeDescriptor(LPCSTR szClassName)  [inlined into 6 functions]
//   line   280  XTypeDescriptor::~XTypeDescriptor()  [0x71000c4330]
//   line   302  INPUT_TYPE XTypeDescriptor::GetInputType() const  [0x71005fb430]
//   line   311  CRefString XTypeDescriptor::ToString(const void * pData) const  [0x71005fc1d0]
//   line   318  void XTypeDescriptor::FromString(const TypeDescriptorField &, void *, const CRefString &) const  [0x71005fc230]
//   line   326  void XTypeDescriptor::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void *, CRefString &) const  [0x71005fe240]
//   line   335  BOOL XTypeDescriptor::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString &, void *) const  [0x71005fe270]
//   line   344  TDVariant XTypeDescriptor::ToVariant(const void *) const  [0x71005fc330]
//   line   351  BOOL XTypeDescriptor::FromVariant(void *, const TDVariant &) const  [0x71005fc360]
//   line   363  BOOL XTypeDescriptor::FromStringFull(const TypeDescriptorField & tField, const BBETranslationData &, void * pData, const CRefString & szData) const  [0x71005fb6d0]
//   line   378  void XTypeDescriptor::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const  [inlined into 1 functions]
//   line   400  BOOL XTypeDescriptor::ContainsSNOSubfield(const TypeDescriptorField &) const  [0x71005fbfd0]
//   line   406  BOOL XTypeDescriptor::ContainsAllocationSubfields(const TypeDescriptorField &) const  [0x71005fb780]
//   line   412  BOOL XTypeDescriptor::ContainsServerOnlySubfields(const TypeDescriptorField &) const  [0x71005fb790]
//   line   418  void XTypeDescriptor::ScanForSubfields(const TypeDescriptorField &) const  [0x71005fb7a0]
//   line   435  void XTypeDescriptor::IterateSNOReferences(const void *, const TypeDescriptorField &, SNOIterateFunc, void *) const  [0x71005fbca0]
//   line   443  void XTypeDescriptor::IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const  [0x71005fb860]
//   line   452  void XTypeDescriptor::ChangeSNOReference(const void *, const TypeDescriptorField &, const SNOName &, const SNO) const  [0x71005fbcb0]
//   line   460  void XTypeDescriptor::DebugDump(const void *, const TypeDescriptorField &, DebugDumpData *) const  [0x71005fb8e0]
//   line   467  TagVariantType XTypeDescriptor::GetTagVariantType() const  [0x71005fc470]
//   line   473  void XTypeDescriptor::Byteswap(void *, void *, void *, const TypeDescriptorField &) const  [0x71005fe6f0]
//   line   482  void XTypeDescriptor::ByteswapInPlace(void *, void *, const TypeDescriptorField &) const  [0x71005fe720]
//   line   490  void XTypeDescriptor::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc) const  [inlined into 2 functions]
//   line   497  void XTypeDescriptor::ByteswapInPlace(void * lpDefinition) const  [inlined into 2 functions]
//   line   515  void XTypeDescriptor::AdjustSerializeDataSize(void *, const TypeDescriptorField &, const BOOL) const  [0x71005fb920]
//   line   520  void XTypeDescriptor::ComputeOffsets(void *, const TypeDescriptorField &, uint32 &) const  [0x71005fb930]
//   line   525  void XTypeDescriptor::ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList &) const  [0x71005fb940]
//   line   534  void XTypeDescriptor::ComputeMergedOffsets(void *, const TypeDescriptorField &, uint32 &) const  [0x71005fb950]
//   line   539  void XTypeDescriptor::SerializeToBinary(void *, const TypeDescriptorField &, uint8 *, const uint32, const uint32) const  [0x7100981370]
//   line   549  void XTypeDescriptor::SerializeFromBinary(void *, const UnserializeData &, const TypeDescriptorField &) const  [0x71009813a0]
//   line   557  void XTypeDescriptor::Delete(const uint32, const void *, const TypeDescriptorField &, XMemoryAllocatorInterface *) const  [0x71005fb980]
//   line   588  BOOL XTypeDescriptor::Contains64BitSubfields(const TypeDescriptorField &) const  [0x71005fba40]
//   line   594  uint32 XTypeDescriptor::GetSub64BitPlatformMask(const TypeDescriptorField &) const  [0x71005fba50]
//   line   600  BOOL XTypeDescriptor::Contains128BitSubfields(const TypeDescriptorField &) const  [0x71005fba60]
//   line   691  ComplexTypeDescriptor::~ComplexTypeDescriptor()  [0x7100984740]
//   line   701  ComplexTypeDescriptor::ComplexTypeDescriptor(LPCSTR szClassName, int32 nVersion, const TypeDescriptorField * arFields, uint32 nFieldCount, DOMVersionFunc pfuncDOMVersion, uint32 dwFlags, ToVariantFunc pfuncToVariant, FromVariantFunc pfuncFromVariant, VariantsEqualFunc pfuncVariantsEqual, LoadFromDefaultDataFunc pfuncLoadFromDefaultData)  [inline]
//   line   729  int32 ComplexTypeDescriptor::SizeOf() const  [0x7100605790]
//   line   737  void ComplexTypeDescriptor::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const  [0x7100605870]
//   line   752  BOOL ComplexTypeDescriptor::ContainsSNOSubfield(const TypeDescriptorField & field) const  [0x7100605950]
//   line   763  BOOL ComplexTypeDescriptor::ContainsAllocationSubfields(const TypeDescriptorField & field) const  [0x7100605990]
//   line   780  BOOL ComplexTypeDescriptor::ContainsServerOnlySubfields(const TypeDescriptorField & field) const  [0x71006059d0]
//   line   791  TDVariant ComplexTypeDescriptor::ToVariant(const void * pData) const  [0x71006057b0]
//   line   802  BOOL ComplexTypeDescriptor::FromVariant(void * pData, const TDVariant & tValue) const  [0x71006057f0]
//   line   814  BOOL ComplexTypeDescriptor::VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const  [0x7100605830]
//   line   930  void ComplexTypeDescriptor::CalculateNetworkBitCounts(TypeDescriptorField & field) const  [0x7100605a10]
//   line   968  BOOL ComplexTypeDescriptor::Contains64BitSubfields(const TypeDescriptorField & field) const  [0x7100605a20]
//   line   982  BOOL ComplexTypeDescriptor::Contains128BitSubfields(const TypeDescriptorField & field) const  [0x7100605a60]
//   line  1048  VariantTypeDescriptor::~VariantTypeDescriptor()  [0x7100605780]
//   line  1052  VariantTypeDescriptor::VariantTypeDescriptor(LPCSTR szClassName, int32 nVersion, const TypeDescriptorField * arFields, uint32 nFieldCount, DOMVersionFunc pfuncDOMVersion, uint32 dwFlags, ToVariantFunc pfuncToVariant, FromVariantFunc pfuncFromVariant, VariantsEqualFunc pfuncVariantsEqual, int32 nVariantOffset)  [inline]

// Shared scalar serialization operations, defined in TypeDescriptor.cpp.
BOOL GenericFloatEncode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder);
XDecoder::Result GenericFloatDecode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder);
void GenericFloatFuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG);

BOOL GenericIntEncode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder);
XDecoder::Result GenericIntDecode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder);
void GenericIntFuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG);

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp, alongside DT_SNO above.
extern const XTypeDescriptor * DT_GBID;  // D3DescribedType.cpp:1295
extern const XTypeDescriptor * DT_VELOCITY;  // D3DescribedType.cpp:1641
extern const XTypeDescriptor * DT_ACCEL;  // D3DescribedType.cpp:1814
extern const XTypeDescriptor * DT_ANGULARVELOCITY;  // D3DescribedType.cpp:2029
extern const XTypeDescriptor * DT_ANGULARACCEL;  // D3DescribedType.cpp:2242
extern const XTypeDescriptor * DT_STRING_FORMULA;  // D3DescribedType.cpp:4142
extern const XTypeDescriptor * DT_ACTOR_GROUP;  // D3DescribedType.cpp:4346
extern const XTypeDescriptor * DT_ACTOR_ATTRIBUTE;  // D3DescribedType.cpp:4564
extern const XTypeDescriptor * DT_STARTLOC_NAME;  // D3DescribedType.cpp:4759
extern const XTypeDescriptor * DT_ANIM_TAG;  // D3DescribedType.cpp:529

// TypeDescriptor.h:497 (inlined into SNOTOC::ReadTOC at line 500).
inline void XTypeDescriptor::ByteswapInPlace(void * lpDefinition) const
{
    ByteswapInPlace(lpDefinition, lpDefinition, TypeDescriptorField(this, NULL));
}
