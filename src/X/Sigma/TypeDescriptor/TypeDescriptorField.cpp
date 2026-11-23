// Original file: X/Sigma/TypeDescriptor/TypeDescriptorField.cpp






#include "TypeDescriptorField.h"

#include <ctype.h>
#include <string.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "TypeDescriptor.h"
#include "../Math/Matrix.h"  // DT_MATRIX4x4

const TypeDescriptorField TypeDescriptorField::NullField;

TypeDescriptorField::TypeDescriptorField()
{
    _ToDefaults();
}

TypeDescriptorField::TypeDescriptorField(const XTypeDescriptor * ptTypeDesc, LPCSTR szName)
{
    _ToDefaults();

    if (szName)
        Blizzard::String::Copy(szDynamicFieldName, szName, sizeof(szDynamicFieldName));

    ptDataType = ptTypeDesc;
    nUsedElementsOffset = -1;
    dwFlags = (1 << TDF_XML_SERIALIZATION);
    nSerOffset = -1;
}

void TypeDescriptorField::MakeAuxiliary(const TypeDescriptorField & tParentField)
{
    _ToDefaults();

    ptDataType = tParentField.GetAuxTypeDesc();
    dwFilter = tParentField.dwFilter;
    dwFlags = (1 << TDF_XML_SERIALIZATION);
    // Set-or-clear (not just set): the original selects between the two results with a csel.
    if (tParentField.dwFlags & (1 << TDF_REQUIRED_SNO))
        dwFlags |= (1 << TDF_REQUIRED_SNO);
    else
        dwFlags &= ~(1 << TDF_REQUIRED_SNO);
    if (tParentField.dwFlags & (1 << TDF_SOFT_LINK))
        dwFlags |= (1 << TDF_SOFT_LINK);
    else
        dwFlags &= ~(1 << TDF_SOFT_LINK);

    nDataOffset = ptDataType->SizeOf();
    if (tParentField.vDefaultFieldValue)
        vDefaultFieldValue = (const uint8 *)tParentField.vDefaultFieldValue + nDataOffset;

    nUsedElementsOffset = -1;
    nSerOffset = -1;
}

TypeDescriptorField::TypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType, int32 nDataOffset,
                                         const void * vDefaultFieldValue, uint32 dwFlags)
{
    _ToDefaults();

    this->szFieldName = szFieldName;
    this->ptDataType = ptDataType;
    this->nDataOffset = nDataOffset;
    this->vDefaultFieldValue = vDefaultFieldValue;
    this->dwFlags = dwFlags;
}

// Member order except nBitCount/dwFilter before nUsedElementsOffset; the store merging in every
// constructor that inlines this depends on that order.
inline void TypeDescriptorField::_ToDefaults()
{
    szFieldName = NULL;
    szDynamicFieldName[0] = '\0';
    ptDataType = DT_NULL;
    nDataOffset = 0;
    vDefaultFieldValue = NULL;
    vMinFieldValue = NULL;
    vMaxFieldValue = NULL;
    dwFlags = 0;
    ptAuxDataType = DT_NULL;
    nSerOffset = 0;
    nCount = -1;
    nBitCount = 0;
    dwFilter = 0xFFFFFFFF;
    nUsedElementsOffset = 0;
    nArraySizeBitCount = 0;
    eTagBase = 0;
    pSymbolTable = NULL;
    nBit = 0;
    pfnStringToInt = NULL;
    pfnIntToString = NULL;
    nVariantTypeValue = -1;
}

SerializeData & TypeDescriptorField::GetSerializeDataRaw(void * pThis) const
{
    SerializeData & ser = *(SerializeData *)((uint8 *)pThis + nSerOffset);
    return ser;
}

const SerializeData & TypeDescriptorField::GetSerializeDataRaw(const void * pThis) const
{
    const SerializeData & ser = *(const SerializeData *)((const uint8 *)pThis + nSerOffset);
    return ser;
}

SerializeData & TypeDescriptorField::GetSerializeData(void * pThis) const
{
    SerializeData & ser = *(SerializeData *)((uint8 *)pThis + nSerOffset);
    return ser;
}

int32 TypeDescriptorField::GetVariantTypeValue() const
{
    return nVariantTypeValue;
}

void TypeDescriptorField::CalculateNetworkBitCounts()
{
    ptDataType->CalculateNetworkBitCounts(*this);
}

OptionalTypeDescriptorField::OptionalTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType, int32 nDataOffset,
                                                         const void * vDefaultFieldValue, uint32 dwFlags)
    : TypeDescriptorField(szFieldName, DT_OPTIONAL, nDataOffset, vDefaultFieldValue, dwFlags)
{
    ptAuxDataType = ptDataType;
}

VariantTypeDescriptorField::VariantTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType, int32 nDataOffset,
                                                       const void * vDefaultFieldValue, int32 nVariantTypeValue)
    : TypeDescriptorField(szFieldName, ptDataType, nDataOffset, vDefaultFieldValue, (1 << TDF_XML_SERIALIZATION))
{
    this->nVariantTypeValue = nVariantTypeValue;
}

BoundedTypeDescriptorField::BoundedTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType, int32 nDataOffset,
                                                       const void * vDefaultFieldValue, const void * vMinFieldValue,
                                                       const void * vMaxFieldValue)
    : TypeDescriptorField(szFieldName, ptDataType, nDataOffset, vDefaultFieldValue,
                          (1 << TDF_XML_SERIALIZATION) | (1 << TDF_MINMAX_BOUNDED))
{
    this->vMinFieldValue = vMinFieldValue;
    this->vMaxFieldValue = vMaxFieldValue;
}

OptionalBoundedTypeDescriptorField::OptionalBoundedTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType,
                                                                       int32 nDataOffset, const void * vDefaultFieldValue,
                                                                       const void * vMinFieldValue, const void * vMaxFieldValue)
    : BoundedTypeDescriptorField(szFieldName, DT_OPTIONAL, nDataOffset, vDefaultFieldValue, vMinFieldValue, vMaxFieldValue)
{
    ptAuxDataType = ptDataType;
}

FixedArrayTypeDescriptorField::FixedArrayTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType, int32 nDataOffset,
                                                             const void * vDefaultFieldValue, uint32 dwFlags, int32 nCount,
                                                             int32 nUsedElementsOffset)
    : TypeDescriptorField(szFieldName, DT_FIXEDARRAY, nDataOffset, vDefaultFieldValue, dwFlags)
{
    ptAuxDataType = ptDataType;
    this->nCount = nCount;
    this->nUsedElementsOffset = nUsedElementsOffset;
}

VariableArrayTypeDescriptorField::VariableArrayTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType,
                                                                   int32 nDataOffset, uint32 dwFlags, int32 nSerOffset)
    : TypeDescriptorField(szFieldName, DT_VARIABLEARRAY, nDataOffset, NULL, dwFlags)
{
    ptAuxDataType = ptDataType;
    this->nSerOffset = nSerOffset;
}

VariableArrayCountTypeDescriptorField::VariableArrayCountTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, uint32 dwFlags,
                                                                             const XTypeDescriptor * ptArrayType, int32 nSerOffset)
    : TypeDescriptorField(szFieldName, DT_VARIABLEARRAY_COUNT, nDataOffset, NULL, dwFlags)
{
    ptAuxDataType = ptArrayType;
    this->nSerOffset = nSerOffset;
}

CStringTypeDescriptorField::CStringTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, uint32 dwFlags, int32 nSerOffset)
    : TypeDescriptorField(szFieldName, DT_CSTRING, nDataOffset, NULL, dwFlags)
{
    ptAuxDataType = DT_BYTE;
    this->nSerOffset = nSerOffset;
}

CharArrayTypeDescriptorField::CharArrayTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, const void * vDefaultFieldValue,
                                                           uint32 dwFlags, int32 nCount, int32 nVariantTypeValue)
    : TypeDescriptorField(szFieldName, DT_CHARARRAY, nDataOffset, vDefaultFieldValue, dwFlags)
{
    this->nCount = nCount;
    this->nVariantTypeValue = nVariantTypeValue;
}

EnumTypeDescriptorField::EnumTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, const void * vDefaultFieldValue,
                                                 const SymbolicEntry * pSymbolTable)
    : TypeDescriptorField(szFieldName, DT_ENUM, nDataOffset, vDefaultFieldValue, (1 << TDF_XML_SERIALIZATION))
{
    this->pSymbolTable = pSymbolTable;
}

FlagTypeDescriptorField::FlagTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, const void * vDefaultFieldValue, int32 nBit)
    : TypeDescriptorField(szFieldName, DT_FLAG, nDataOffset, vDefaultFieldValue, (1 << TDF_XML_SERIALIZATION))
{
    this->nBit = nBit;
}

TranslatableTypeDescriptorField::TranslatableTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, const void * vDefaultFieldValue,
                                                                 TranslateStringToIntFunc pfnStringToInt,
                                                                 TranslateIntToStringFunc pfnIntToString)
    : TypeDescriptorField(szFieldName, DT_TRANSLATEABLE, nDataOffset, vDefaultFieldValue, (1 << TDF_XML_SERIALIZATION))
{
    this->pfnStringToInt = pfnStringToInt;
    this->pfnIntToString = pfnIntToString;
}

TypeDescriptorFieldSerializationInfo::TypeDescriptorFieldSerializationInfo(const TypeDescriptorField & tTypeDescriptorField,
                                                                           const void * ptData)
    : m_szXMLAttributes(NULL)
    , m_sFullFieldPath("")
    , m_tField(tTypeDescriptorField)
    , m_ptParent(NULL)
    , m_pXMLAttributeGenerator(NULL)
    , m_pXMLAttributeParser(NULL)
    , m_ptData(ptData)
{
}

TypeDescriptorFieldSerializationInfo::TypeDescriptorFieldSerializationInfo(const TypeDescriptorField & tTypeDescriptorField,
                                                                           const TypeDescriptorFieldSerializationInfo & tParentInfo,
                                                                           const CRefString & sFieldAccessorPath, const void * ptData)
    : m_szXMLAttributes(NULL)
    , m_tField(tTypeDescriptorField)
    , m_ptParent(&tParentInfo)
    , m_ptData(ptData)
{
    if (tParentInfo.m_sFullFieldPath.Length() == 0)
    {
        m_sFullFieldPath = sFieldAccessorPath;
    }
    else
    {
        const XTypeDescriptor * ptParentTypeDescriptor = tParentInfo.GetField().GetTypeDesc();
        BOOL bParentIsGrowableArray = (ptParentTypeDescriptor == DT_VARIABLEARRAY || ptParentTypeDescriptor == DT_CSTRING) &&
                                      (tParentInfo.GetField().dwFlags & (1 << TDF_XGROWABLE_ARRAY));

        if ((isalnum(sFieldAccessorPath[0]) && !bParentIsGrowableArray && ptParentTypeDescriptor != DT_FIXEDARRAY &&
             ptParentTypeDescriptor != DT_VARIABLEARRAY) ||
            ptParentTypeDescriptor == DT_MATRIX4x4)
            m_sFullFieldPath.Format("%s.%s", tParentInfo.m_sFullFieldPath.str(), sFieldAccessorPath.str());
        else
            m_sFullFieldPath.Format("%s%s", tParentInfo.m_sFullFieldPath.str(), sFieldAccessorPath.str());
    }

    UseXMLAttributeGenerator(tParentInfo.m_pXMLAttributeGenerator);
    UseXMLAttributeParser(tParentInfo.m_pXMLAttributeParser);
}

TypeDescriptorFieldSerializationInfo::~TypeDescriptorFieldSerializationInfo()
{
    if (m_szXMLAttributes)
        delete[] m_szXMLAttributes;
    m_szXMLAttributes = NULL;
}

inline void TypeDescriptorFieldSerializationInfo::UseXMLAttributeGenerator(
    TypeDescriptorFieldSerializationInfo::XMLAttributeGenerator * pXMLAttributeGenerator)
{
    m_pXMLAttributeGenerator = pXMLAttributeGenerator;
    if (m_szXMLAttributes)
        delete[] m_szXMLAttributes;
    m_szXMLAttributes = NULL;
}

inline void TypeDescriptorFieldSerializationInfo::UseXMLAttributeParser(
    TypeDescriptorFieldSerializationInfo::XMLAttributeParser * pXMLAttributeGenerator)
{
    m_pXMLAttributeParser = pXMLAttributeGenerator;
}

LPCSTR TypeDescriptorFieldSerializationInfo::GetXMLAttributes() const
{
    if (m_pXMLAttributeGenerator && !m_szXMLAttributes)
    {
        CRefString sAttributes = m_pXMLAttributeGenerator->GenerateXMLAttributes(*this);
        if (sAttributes.Length())
        {
            m_szXMLAttributes = new CHAR[sAttributes.Length() + 1];
            strcpy(m_szXMLAttributes, sAttributes.str());
        }
    }
    return m_szXMLAttributes;
}
