// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/TypeDescriptor/TypeDescriptorField.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../RefString.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"

class CXMLNode;
struct SerializeData;
struct SymbolicEntry;
class XTypeDescriptor;

enum TypeDescriptorFieldFlags {
    TDF_XML_SERIALIZATION = 0,
    TDF_XGROWABLE_ARRAY = 1,
    TDF_GBID_COLUMN = 2,
    TDF_VARIABLE_COUNT = 3,
    TDF_MINMAX_BOUNDED = 4,
    TDF_XPTR64 = 5,
    TDF_IGNORE_ON_SERVER = 6,
    TDF_XFLOAT16 = 7,
    TDF_REQUIRED_SNO = 8,
    TDF_SOFT_LINK = 9,
    TDF_SERVER_ONLY = 10,
    TDF_PC_ONLY = 11,
    TDF_AXE_ONLY = 14,
    TDF_XPTR128 = 15,
    TDF_COMPUTE_ARRAY_COUNT = 16,
    TDF_DISABLE_TAGMAP_OPTIMIZATION = 17,
    TDF_DISABLE_DATA_MERGE = 18,
    TDF_BIT_FLAGS = 19,
    TDF_XBOXONE_ONLY = 20,
    TDF_PS4_ONLY = 21,
    TDF_NX64_ONLY = 22,
};

typedef int32 (*TranslateStringToIntFunc)(LPCSTR);

typedef CRefString (*TranslateIntToStringFunc)(int32);

class TypeDescriptorField {  // size 0xc0
public:
    static const TypeDescriptorField NullField;
    TypeDescriptorField();
    TypeDescriptorField(const XTypeDescriptor *, LPCSTR);
    TypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, uint32);
    void MakeAuxiliary(const TypeDescriptorField & tParentField);
    const int32 GetCount() const { return nCount; } // Original line 74.
    const void * GetDefaultData() const;
    BOOL IsNull() const;
    CRefString GetName() const;
    const XTypeDescriptor * GetTypeDesc() const;
    const XTypeDescriptor * GetAuxTypeDesc() const;
    void * Offset(void * pThis) const;
    const void * Offset(const void * pThis) const;
    SerializeData & GetSerializeDataRaw(void * pThis) const;
    const SerializeData & GetSerializeDataRaw(const void * pThis) const;
    SerializeData & GetSerializeData(void * pThis) const;
    const SerializeData & GetSerializeData(const void *) const;
    int32 GetVariantTypeValue() const;
    void CalculateNetworkBitCounts();
private:
    void _ToDefaults();
public:
    LPCSTR szFieldName;  // 0x00
    const XTypeDescriptor * ptDataType;  // 0x08
    int32 nDataOffset;  // 0x10
    const void * vDefaultFieldValue;  // 0x18
    const void * vMinFieldValue;  // 0x20
    const void * vMaxFieldValue;  // 0x28
    uint32 dwFlags;  // 0x30
    const XTypeDescriptor * ptAuxDataType;  // 0x38
    int32 nSerOffset;  // 0x40
    int32 nCount;  // 0x44
    int32 nUsedElementsOffset;  // 0x48
    int16 nBitCount;  // 0x4c
    int16 nArraySizeBitCount;  // 0x4e
    uint32 dwFilter;  // 0x50
    int32 eTagBase;  // 0x54
    const SymbolicEntry * pSymbolTable;  // 0x58
    int32 nBit;  // 0x60
    TranslateStringToIntFunc pfnStringToInt;  // 0x68
    TranslateIntToStringFunc pfnIntToString;  // 0x70
    int32 nVariantTypeValue;  // 0x78
    CHAR szDynamicFieldName[64];  // 0x7c
};
static_assert(sizeof(TypeDescriptorField) == 0xc0, "TypeDescriptorField");

class TypeDescriptorFieldSerializationInfo {  // size 0x48
public:
    // Not in the DWARF beyond their names. GetXMLAttributes calls the generator through vtable
    // slot 2 (after the two destructor entries); the method name is invented.
    class XMLAttributeGenerator {
    public:
        virtual ~XMLAttributeGenerator();
        virtual CRefString GenerateXMLAttributes(const TypeDescriptorFieldSerializationInfo & tInfo) = 0;
    };
    class XMLAttributeParser;
private:
    TypeDescriptorFieldSerializationInfo(const TypeDescriptorFieldSerializationInfo &);
    TypeDescriptorFieldSerializationInfo & operator=(const TypeDescriptorFieldSerializationInfo &);
public:
    TypeDescriptorFieldSerializationInfo(const TypeDescriptorField &, const void *);
    TypeDescriptorFieldSerializationInfo(const TypeDescriptorField &, const TypeDescriptorFieldSerializationInfo &, const CRefString &, const void *);
    ~TypeDescriptorFieldSerializationInfo();
    void UseXMLAttributeGenerator(TypeDescriptorFieldSerializationInfo::XMLAttributeGenerator * pXMLAttributeGenerator);
    void UseXMLAttributeParser(TypeDescriptorFieldSerializationInfo::XMLAttributeParser * pXMLAttributeGenerator);  // sic: the DWARF names the parameter pXMLAttributeGenerator
    const CRefString & GetFullFieldPath() const;
    LPCSTR GetXMLAttributes() const;
    BOOL ParseXMLAttributes(const CXMLNode &);
    const TypeDescriptorField & GetRootField() const;
    const TypeDescriptorField & GetField() const;
    const TypeDescriptorFieldSerializationInfo * GetParent() const;
    const TypeDescriptorField & GetParentField() const;
    const BOOL HasParent() const;
    const void * GetData() const;
protected:
    mutable CHAR * m_szXMLAttributes;  // 0x00 (written by the const GetXMLAttributes)
    CRefString m_sFullFieldPath;  // 0x08
    const TypeDescriptorField & m_tField;  // 0x20
    const TypeDescriptorFieldSerializationInfo * m_ptParent;  // 0x28
    TypeDescriptorFieldSerializationInfo::XMLAttributeGenerator * m_pXMLAttributeGenerator;  // 0x30
    TypeDescriptorFieldSerializationInfo::XMLAttributeParser * m_pXMLAttributeParser;  // 0x38
    const void * m_ptData;  // 0x40
};
static_assert(sizeof(TypeDescriptorFieldSerializationInfo) == 0x48, "TypeDescriptorFieldSerializationInfo");

class OptionalTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    OptionalTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, uint32);
};
static_assert(sizeof(OptionalTypeDescriptorField) == 0xc0, "OptionalTypeDescriptorField");

class VariantTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    VariantTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, int32);
};
static_assert(sizeof(VariantTypeDescriptorField) == 0xc0, "VariantTypeDescriptorField");

class BoundedTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    BoundedTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, const void *, const void *);
    BoundedTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, const void *, const void *, uint32);
};
static_assert(sizeof(BoundedTypeDescriptorField) == 0xc0, "BoundedTypeDescriptorField");

class OptionalBoundedTypeDescriptorField : public BoundedTypeDescriptorField {  // size 0xc0
public:
    OptionalBoundedTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, const void *, const void *);
};
static_assert(sizeof(OptionalBoundedTypeDescriptorField) == 0xc0, "OptionalBoundedTypeDescriptorField");

class FixedArrayTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    FixedArrayTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, uint32, int32, int32);
};
static_assert(sizeof(FixedArrayTypeDescriptorField) == 0xc0, "FixedArrayTypeDescriptorField");

class VariableArrayTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    VariableArrayTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, uint32, int32);
};
static_assert(sizeof(VariableArrayTypeDescriptorField) == 0xc0, "VariableArrayTypeDescriptorField");

class VariableArrayCountTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    VariableArrayCountTypeDescriptorField(LPCSTR, int32, uint32, const XTypeDescriptor *, int32);
};
static_assert(sizeof(VariableArrayCountTypeDescriptorField) == 0xc0, "VariableArrayCountTypeDescriptorField");

class CStringTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    CStringTypeDescriptorField(LPCSTR, int32, uint32, int32);
};
static_assert(sizeof(CStringTypeDescriptorField) == 0xc0, "CStringTypeDescriptorField");

class CharArrayTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    CharArrayTypeDescriptorField(LPCSTR, int32, const void *, uint32, int32, int32);
};
static_assert(sizeof(CharArrayTypeDescriptorField) == 0xc0, "CharArrayTypeDescriptorField");

class EnumTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    EnumTypeDescriptorField(LPCSTR, int32, const void *, const SymbolicEntry *);
};
static_assert(sizeof(EnumTypeDescriptorField) == 0xc0, "EnumTypeDescriptorField");

class OptionalEnumTypeDescriptorField : public EnumTypeDescriptorField {  // size 0xc0
public:
    OptionalEnumTypeDescriptorField(LPCSTR, int32, const void *, const SymbolicEntry *);
};
static_assert(sizeof(OptionalEnumTypeDescriptorField) == 0xc0, "OptionalEnumTypeDescriptorField");

class FlagTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    FlagTypeDescriptorField(LPCSTR, int32, const void *, int32);
};
static_assert(sizeof(FlagTypeDescriptorField) == 0xc0, "FlagTypeDescriptorField");

class TranslatableTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    TranslatableTypeDescriptorField(LPCSTR, int32, const void *, TranslateStringToIntFunc, TranslateIntToStringFunc);
    TranslatableTypeDescriptorField(LPCSTR, int32, const void *, TranslateStringToIntFunc, TranslateIntToStringFunc, uint32);
};
static_assert(sizeof(TranslatableTypeDescriptorField) == 0xc0, "TranslatableTypeDescriptorField");


// Inline helpers (original lines 80, 105, 111, 117, 124, 264). Only the ones whose bodies are
// certain are written; GetCount and GetName are declared above but not defined yet.
inline const void * TypeDescriptorField::GetDefaultData() const
{
    return vDefaultFieldValue;
}

// Original line 86. Field tables end with a null (unnamed) entry; written from
// ComplexTypeDescriptor::ScanForSubfields, which stops on a null szFieldName.
inline BOOL TypeDescriptorField::IsNull() const
{
    return szFieldName == NULL;
}

// Original line 92; reconstructed from TAGMAP::SerializeToXML.
inline CRefString TypeDescriptorField::GetName() const
{
    return (IsNull() || !*szFieldName) ? szDynamicFieldName : szFieldName;
}

inline const XTypeDescriptor * TypeDescriptorField::GetTypeDesc() const
{
    return ptDataType;
}

inline const XTypeDescriptor * TypeDescriptorField::GetAuxTypeDesc() const
{
    return ptAuxDataType;
}

inline void * TypeDescriptorField::Offset(void * pThis) const
{
    return (uint8 *)pThis + nDataOffset;
}

inline const void * TypeDescriptorField::Offset(const void * pThis) const
{
    return (const uint8 *)pThis + nDataOffset;
}

inline const TypeDescriptorField & TypeDescriptorFieldSerializationInfo::GetField() const
{
    return m_tField;
}

// Functions defined in this file (bodies are not in the DWARF):
//   line    74  const int32 TypeDescriptorField::GetCount() const  [inlined into 21 functions]
//   line    80  const void * TypeDescriptorField::GetDefaultData() const  [inlined into 32 functions]
//   line    86  BOOL TypeDescriptorField::IsNull() const  [inlined into 39 functions]
//   line    92  CRefString TypeDescriptorField::GetName() const  [inlined into 6 functions]
//   line   105  const XTypeDescriptor * TypeDescriptorField::GetTypeDesc() const  [inlined into 45 functions]
//   line   111  const XTypeDescriptor * TypeDescriptorField::GetAuxTypeDesc() const  [inlined into 59 functions]
//   line   117  void * TypeDescriptorField::Offset(void * pThis) const  [inlined into 21 functions]
//   line   124  const void * TypeDescriptorField::Offset(const void * pThis) const  [inlined into 10 functions]
//   line   264  const TypeDescriptorField & TypeDescriptorFieldSerializationInfo::GetField() const  [inlined into 23 functions]
