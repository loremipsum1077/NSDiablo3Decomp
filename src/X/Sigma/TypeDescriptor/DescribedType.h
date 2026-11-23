// Reconstructed from DWARF declarations and original-code comparisons.
// Original file: X/Sigma/TypeDescriptor/DescribedType.h

#pragma once

#include "X/Sigma/Files/XSerializeData.h"
#include "X/Sigma/Math/MathGeneral.h"
#include "X/Sigma/TypeDescriptor/SymbolicValue.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptor.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "X/Sigma/Networking/XEncoder.h"
#include "X/Sigma/Random.h"
#include "X/Sigma/XML/SAXHelper.h"
#include <stdlib.h>
#include "BlizzardCore/Include/Blizzard/String.h"
#include "X/Sigma/DataStructures/XGrowableArray.h"
#include "X/Sigma/XML/XMLString.h"
#include <cstring>


struct BBETranslationData;
class CRefString;
class CXMLNode;
struct RNG;
class SAX;
struct SNOName;
class TypeDescriptorField;
class TypeDescriptorFieldSerializationInfo;
class XEncoder;
class XMemoryAllocatorInterface;

typedef int32 TagVariantType;

extern const XTypeDescriptor * DT_NULL;

class TypeDescriptorClass_DT_INT : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_INT() {}
    // Original DescribedType.h line 41; the default name is what DT_INT and DT_VARIABLEARRAY_COUNT use.
    TypeDescriptorClass_DT_INT(LPCSTR szClassName = "DT_INT") : XTypeDescriptor(szClassName) {}

    // Original DescribedType.h line 45.
    virtual INPUT_TYPE GetInputType() const
    { return INPUT_TYPE_INT; }

    // Original DescribedType.h line 51.
    virtual int32 SizeOf() const
    { return 4; }

    // Original DescribedType.h line 57.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString().Format("%d", *(const int *)pData);
    }

    // Original DescribedType.h line 63.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        *(int *)pData = atoi(szData);
    }

    // Original DescribedType.h line 71.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant(*(const int *)pData);
    }

    // Original DescribedType.h line 77.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        *(int *)pData = tValue.GetInt();
        return 1;
    }

    // Original DescribedType.h line 85.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned int *)pData = (uint32)(uintptr)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 146.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int32 nBitCount = SizeOf() * 8;
        if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
            int32 nDelta = (intptr)field.vMaxFieldValue - (intptr)field.vMinFieldValue;
            nBitCount = nDelta > 0 ? Math::LogBase2(nDelta) + 1 : 0;
        }
        field.nBitCount = nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_INT) == 0x38, "TypeDescriptorClass_DT_INT");

class TypeDescriptorClass_DT_RGBACOLOR : public TypeDescriptorClass_DT_INT {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_RGBACOLOR() {}
    TypeDescriptorClass_DT_RGBACOLOR();
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void LoadFromDefaultData(const TypeDescriptorField &, void * pData) const;  // vtable slot 12
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    static int32 VersionColor(const int32 nCurrentVersion, const CXMLNode & nodeXML, void * pData);
};
static_assert(sizeof(TypeDescriptorClass_DT_RGBACOLOR) == 0x38, "TypeDescriptorClass_DT_RGBACOLOR");

class TypeDescriptorClass_DT_RGBACOLORVALUE : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_RGBACOLORVALUE() {}
    TypeDescriptorClass_DT_RGBACOLORVALUE();
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void LoadFromDefaultData(const TypeDescriptorField &, void * pData) const;  // vtable slot 12
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    static int32 VersionColor(const int32 nCurrentVersion, const CXMLNode & nodeXML, void * pData);
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};
static_assert(sizeof(TypeDescriptorClass_DT_RGBACOLORVALUE) == 0x38, "TypeDescriptorClass_DT_RGBACOLORVALUE");

class TypeDescriptorClass_DT_DATAID : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_DATAID() {}
    TypeDescriptorClass_DT_DATAID() : XTypeDescriptor("DT_DATAID") {}

    // Original DescribedType.h line 317.
    virtual INPUT_TYPE GetInputType() const
    { return INPUT_TYPE_INT; }

    // Original DescribedType.h line 323.
    virtual int32 SizeOf() const
    { return 4; }

    // Original DescribedType.h line 329.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString().Format("0x%08X", *(const int *)pData);
    }

    // Original DescribedType.h line 335.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        *(int *)pData = atoi(szData);
    }

    // Original DescribedType.h line 343.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant(*(const int *)pData);
    }

    // Original DescribedType.h line 349.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        *(int *)pData = tValue.GetInt();
        return 1;
    }

    // Original DescribedType.h line 357.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned int *)pData = (uint32)(uintptr)pDefaultData;
    }
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 423.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int32 nBitCount = SizeOf() * 8;
        if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
            int32 nDelta = (intptr)field.vMaxFieldValue - (intptr)field.vMinFieldValue;
            nBitCount = nDelta > 0 ? Math::LogBase2(nDelta) + 1 : 0;
        }
        field.nBitCount = nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_DATAID) == 0x38, "TypeDescriptorClass_DT_DATAID");

class TypeDescriptorClass_DT_INT64 : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_INT64() {}
    TypeDescriptorClass_DT_INT64() : XTypeDescriptor("DT_INT64") {}

    // Original DescribedType.h line 454.
    virtual INPUT_TYPE GetInputType() const
    { return INPUT_TYPE_INT; }

    // Original DescribedType.h line 460.
    virtual int32 SizeOf() const
    { return 8; }

    // Original DescribedType.h line 466.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString().Format("%lld", *(const long long *)pData);
    }

    // Original DescribedType.h line 472.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        *(long long *)pData = strtoll(szData, NULL, 0);
    }

    // Original DescribedType.h line 480.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant(*(const int *)pData);
    }

    // Original DescribedType.h line 486.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        *(long long *)pData = tValue.GetInt();
        return 1;
    }

    // Original DescribedType.h line 494.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned long long *)pData = (uint64)(uintptr)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 555.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int32 nBitCount = SizeOf() * 8;
        if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
            int64 nDelta = (intptr)field.vMaxFieldValue - (intptr)field.vMinFieldValue;
            nBitCount = nDelta > 0 ? Math::LogBase2(nDelta) + 1 : 0;
        }
        field.nBitCount = nBitCount;
    }

    // Original DescribedType.h line 574.
    virtual BOOL Contains64BitSubfields(const TypeDescriptorField &) const
    {
        return (int)1;
    }

    // Original DescribedType.h line 580.
    virtual uint32 GetSub64BitPlatformMask(const TypeDescriptorField & field) const
    {
        return field.dwFlags & 0x704800;
    }

    // Original DescribedType.h line 586.
    virtual BOOL Contains128BitSubfields(const TypeDescriptorField &) const
    {
        return (int)0;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_INT64) == 0x38, "TypeDescriptorClass_DT_INT64");

class TypeDescriptorClass_DT_BYTE : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_BYTE() {}
    TypeDescriptorClass_DT_BYTE() : XTypeDescriptor("DT_BYTE") {}

    // Original DescribedType.h line 604.
    virtual INPUT_TYPE GetInputType() const
    { return INPUT_TYPE_INT; }

    // Original DescribedType.h line 610.
    virtual int32 SizeOf() const
    { return 1; }

    // Original DescribedType.h line 616.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString().Format("%d", *(const unsigned char *)pData);
    }

    // Original DescribedType.h line 622.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        *(unsigned char *)pData = (unsigned char)atoi(szData);
    }

    // Original DescribedType.h line 630.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant((int)*(const unsigned char *)pData);
    }

    // Original DescribedType.h line 636.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        *(unsigned char *)pData = tValue.GetInt();
        return 1;
    }

    // Original DescribedType.h line 644.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned char *)pData = (unsigned char)(unsigned long long)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7

    // Original DescribedType.h line 662.
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
    {
        *(unsigned char *)lpDefinitionDst = *(const unsigned char *)lpDefinitionSrc;
    }

    // Original DescribedType.h line 671.
    virtual void ByteswapInPlace(void *, void *, const TypeDescriptorField &) const
    {
    }

    // Original DescribedType.h line 676.
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
    {
        *(unsigned char *)(lpDst + dwDstOffset) = *(const unsigned char *)lpDefinition;
    }

    // Original DescribedType.h line 688.
    virtual void SerializeFromBinary(void *, const UnserializeData &, const TypeDescriptorField &) const
    {
    }
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 708.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int32 nBitCount = SizeOf() * 8;
        if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
            int32 nDelta = (intptr)field.vMaxFieldValue - (intptr)field.vMinFieldValue;
            nBitCount = nDelta > 0 ? Math::LogBase2(nDelta) + 1 : 0;
        }
        field.nBitCount = nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_BYTE) == 0x38, "TypeDescriptorClass_DT_BYTE");

class TypeDescriptorClass_DT_FLOAT : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_FLOAT() {}
    TypeDescriptorClass_DT_FLOAT() : XTypeDescriptor("DT_FLOAT") {}

    // Original DescribedType.h line 740.
    virtual INPUT_TYPE GetInputType() const
    { return INPUT_TYPE_FLOAT; }

    // Original DescribedType.h line 746.
    virtual int32 SizeOf() const
    { return 4; }

    // Original DescribedType.h line 752.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString().Format("%g", *(const float *)pData);
    }

    // Original DescribedType.h line 758.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        *(float *)pData = (float)atof(szData);
    }

    // Original DescribedType.h line 766.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant(*(const float *)pData);
    }

    // Original DescribedType.h line 772.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        *(float *)pData = tValue.GetFloat();
        return 1;
    }

    // Original DescribedType.h line 780.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned int *)pData = (uint32)(uintptr)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 841.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int nBitCount = SizeOf() * 8;
        if (field.dwFlags & 0x80)
            nBitCount = 16;
        field.nBitCount = (short)nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_FLOAT) == 0x38, "TypeDescriptorClass_DT_FLOAT");

class TypeDescriptorClass_DT_STRING : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_STRING() {}
    TypeDescriptorClass_DT_STRING() : XTypeDescriptor("DT_STRING") {}

    // Original DescribedType.h line 864.
    virtual int32 SizeOf() const
    { return sizeof(CRefString); }

    // Original DescribedType.h line 870.
    virtual CRefString ToString(const void * pData) const
    {
        return *(const CRefString *)pData;
    }

    // Original DescribedType.h line 876.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        *(CRefString *)pData = szData;
    }

    // Original DescribedType.h line 884.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant((LPCSTR)*(const CRefString *)pData);
    }

    // Original DescribedType.h line 890.
    virtual int FromVariant(void * pData, const TDVariant & tValue) const
    {
        *(CRefString *)pData = tValue.GetStr();
        return 1;
    }

    // Original DescribedType.h line 898.
    virtual int VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const
    {
        return strcmp(tValue1.GetStr(), tValue2.GetStr()) == 0;
    }

    // Original DescribedType.h line 905.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        DisplayInternalError(0, "", 911, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7

    // Original DescribedType.h line 924.
    virtual void Byteswap(void *, void *, void *, const TypeDescriptorField &) const
    {
    }

    // Original DescribedType.h line 930.
    virtual void ByteswapInPlace(void *, void *, const TypeDescriptorField &) const
    {
    }

    // Original DescribedType.h line 935.
    virtual void SerializeToBinary(void *, const TypeDescriptorField &, uint8 *, const uint32, const uint32) const
    {
        DisplayInternalError(0, "", 942, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 945.
    virtual void SerializeFromBinary(void *, const UnserializeData &, const TypeDescriptorField &) const
    {
    }
    virtual BOOL Encode(void *, const TypeDescriptorField &, XEncoder &) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 965.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        field.nBitCount = 0;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_STRING) == 0x38, "TypeDescriptorClass_DT_STRING");

class TypeDescriptorClass_DT_WORD : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_WORD() {}
    TypeDescriptorClass_DT_WORD() : XTypeDescriptor("DT_WORD") {}

    // Original DescribedType.h line 984.
    virtual int32 SizeOf() const
    { return 2; }

    // Original DescribedType.h line 990.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString().Format("%d", *(const unsigned short *)pData);
    }

    // Original DescribedType.h line 996.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        *(unsigned short *)pData = (unsigned short)atoi(szData);
    }

    // Original DescribedType.h line 1004.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant((int)*(const unsigned short *)pData);
    }

    // Original DescribedType.h line 1010.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        *(unsigned short *)pData = tValue.GetUInt();
        return 1;
    }

    // Original DescribedType.h line 1018.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned short *)pData = (unsigned short)(unsigned long long)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 1076.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int32 nBitCount = SizeOf() * 8;
        if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
            int32 nDelta = (intptr)field.vMaxFieldValue - (intptr)field.vMinFieldValue;
            nBitCount = nDelta > 0 ? Math::LogBase2(nDelta) + 1 : 0;
        }
        field.nBitCount = nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_WORD) == 0x38, "TypeDescriptorClass_DT_WORD");

class TypeDescriptorClass_DT_CHARARRAY : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_CHARARRAY() {}
    TypeDescriptorClass_DT_CHARARRAY() : XTypeDescriptor("DT_CHARARRAY") {}

    // Original DescribedType.h line 1108.
    virtual int32 SizeOf() const
    {
        DisplayInternalError(0, "", 1111, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 1115.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString((const char *)pData);
    }

    // Original DescribedType.h line 1121.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        strcpy((char *)pData, szData);
    }

    // Original DescribedType.h line 1129.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant((LPCSTR)pData);
    }

    // Original DescribedType.h line 1135.
    virtual int FromVariant(void * pData, const TDVariant & tValue) const
    {
        strcpy((char *)pData, tValue.GetStr());
        return 1;
    }

    // Original DescribedType.h line 1143.
    virtual int VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const
    {
        return strcmp(tValue1.GetStr(), tValue2.GetStr()) == 0;
    }

    // Original DescribedType.h line 1150.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        if (tField.vDefaultFieldValue)
            strcpy((char *)pData, (LPCSTR)tField.vDefaultFieldValue);
        else
            *(char *)pData = 0;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7

    // Original DescribedType.h line 1175.
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField & field) const
    {
        Blizzard::String::Copy((char *)lpDefinitionDst, (const char *)lpDefinitionSrc, field.nCount);
    }

    // Original DescribedType.h line 1184.
    virtual void ByteswapInPlace(void *, void *, const TypeDescriptorField &) const
    {
    }

    // Original DescribedType.h line 1189.
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
    {
        strcpy((char *)(lpDst + dwDstOffset), (const char *)lpDefinition);
    }

    // Original DescribedType.h line 1202.
    virtual void SerializeFromBinary(void *, const UnserializeData &, const TypeDescriptorField &) const
    {
    }
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 1222.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int32 nBitCount = 0;
        if (field.GetCount() > 0) nBitCount = Math::LogBase2(field.GetCount()) + 1;
        field.nBitCount = nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_CHARARRAY) == 0x38, "TypeDescriptorClass_DT_CHARARRAY");

class TypeDescriptorClass_DT_ANGLE : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_ANGLE() {}
    TypeDescriptorClass_DT_ANGLE() : XTypeDescriptor("DT_ANGLE") {}

    // Original DescribedType.h line 1272.
    virtual int32 SizeOf() const
    { return 4; }

    // Original DescribedType.h line 1278.
    virtual CRefString ToString(const void * pData) const
    {
        const float flAngleInDegrees = *(const float *)pData * 57.2957763671875f;
        return CRefString().Format("%.2f\xc2\xb0", flAngleInDegrees);
    }

    // Original DescribedType.h line 1286.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        float flAngleInDegrees = (float)atof(szData);
        Angle aAngle = flAngleInDegrees * 0.01745329238474369f;
        *(Angle *)pData = aAngle;
    }

    // Original DescribedType.h line 1296.
    virtual TDVariant ToVariant(const void * pData) const
    {
        const float flAngleInDegrees = *(const float *)pData * 57.2957763671875f;
        CRefString sValue;
        sValue.Format("%.2f\xc2\xb0", flAngleInDegrees);
        return TDVariant(sValue, flAngleInDegrees);
    }

    // Original DescribedType.h line 1306.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        float flAngleInDegrees = tValue.GetFloat();
        Angle aAngle = flAngleInDegrees * 0.01745329238474369f;
        *(Angle *)pData = aAngle;
        return TRUE;
    }

    // Original DescribedType.h line 1316.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned int *)pData = (uint32)(uintptr)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 1377.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int nBitCount = SizeOf() * 8;
        if (field.dwFlags & 0x80)
            nBitCount = 16;
        field.nBitCount = (short)nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_ANGLE) == 0x38, "TypeDescriptorClass_DT_ANGLE");

class TypeDescriptorClass_DT_PERCENT : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_PERCENT() {}
    TypeDescriptorClass_DT_PERCENT() : XTypeDescriptor("DT_PERCENT") {}

    // Original DescribedType.h line 1399.
    virtual int32 SizeOf() const
    { return 1; }

    // Original DescribedType.h line 1405.
    virtual CRefString ToString(const void * pData) const
    {
        uint8 bPercent = *(const uint8 *)pData;
        float flPercent = ((float)bPercent / 255.0f) * 100.0f;
        return CRefString().Format("%.2f%%", flPercent);
    }

    // Original DescribedType.h line 1413.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        float flPercent = (float)atof(szData);
        uint8 bPercent = (int)(flPercent * 2.549999952316284f);
        *(uint8 *)pData = bPercent;
    }

    // Original DescribedType.h line 1423.
    virtual TDVariant ToVariant(const void * pData) const
    {
        uint8 bPercent = *(const uint8 *)pData;
        float flPercent = ((float)bPercent / 255.0f) * 100.0f;
        CRefString sValue;
        sValue.Format("%.2f%%", flPercent);
        return TDVariant(sValue, flPercent);
    }

    // Original DescribedType.h line 1433.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        float flPercent = tValue.GetFloat();
        uint8 bPercent = (int)(flPercent * 2.549999952316284f);
        *(uint8 *)pData = bPercent;
        return TRUE;
    }

    // Original DescribedType.h line 1443.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned char *)pData = (unsigned char)(unsigned long long)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7

    // Original DescribedType.h line 1461.
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
    {
        *(unsigned char *)lpDefinitionDst = *(const unsigned char *)lpDefinitionSrc;
    }

    // Original DescribedType.h line 1470.
    virtual void ByteswapInPlace(void *, void *, const TypeDescriptorField &) const
    {
    }

    // Original DescribedType.h line 1475.
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
    {
        *(unsigned char *)(lpDst + dwDstOffset) = *(const unsigned char *)lpDefinition;
    }

    // Original DescribedType.h line 1487.
    virtual void SerializeFromBinary(void *, const UnserializeData &, const TypeDescriptorField &) const
    {
    }
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 1507.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int nBitCount = SizeOf() * 8;
        field.nBitCount = (short)nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_PERCENT) == 0x38, "TypeDescriptorClass_DT_PERCENT");

class TypeDescriptorClass_DT_HIGHPRECISIONPERCENT : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_HIGHPRECISIONPERCENT() {}
    TypeDescriptorClass_DT_HIGHPRECISIONPERCENT() : XTypeDescriptor("DT_HIGHPRECISIONPERCENT") {}

    // Original DescribedType.h line 1526.
    virtual int32 SizeOf() const
    { return 4; }

    // Original DescribedType.h line 1532.
    virtual CRefString ToString(const void * pData) const
    {
        const float flPercent = *(const float *)pData * 100.0f;
        return CRefString().Format("%.4f%%", flPercent);
    }

    // Original DescribedType.h line 1540.
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
    {
        float flPercent = (float)atof(szData);
        *(float *)pData = flPercent / 100.0f;
    }

    // Original DescribedType.h line 1550.
    virtual TDVariant ToVariant(const void * pData) const
    {
        const float flPercent = *(const float *)pData * 100.0f;
        CRefString sValue;
        sValue.Format("%.4f%%", flPercent);
        return TDVariant(sValue, flPercent);
    }

    // Original DescribedType.h line 1560.
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const
    {
        float flPercent = tValue.GetFloat();
        *(float *)pData = flPercent / 100.0f;
        return TRUE;
    }

    // Original DescribedType.h line 1570.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(float *)pData = (float)(unsigned long long)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 1626.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int nBitCount = SizeOf() * 8;
        field.nBitCount = (short)nBitCount;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_HIGHPRECISIONPERCENT) == 0x38, "TypeDescriptorClass_DT_HIGHPRECISIONPERCENT");

class TypeDescriptorClass_DT_ENUM : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_ENUM() {}
    TypeDescriptorClass_DT_ENUM() : XTypeDescriptor("DT_ENUM") {}

    // Original DescribedType.h line 1645.
    virtual int32 SizeOf() const
    {
        return (int)4;
    }
    virtual BOOL FromStringFull(const TypeDescriptorField & tField, const BBETranslationData &, void * pData, const CRefString & szData) const;  // vtable slot 11
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9

    // Original DescribedType.h line 1664.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        *(unsigned int *)pData = (uint32)(uintptr)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};
static_assert(sizeof(TypeDescriptorClass_DT_ENUM) == 0x38, "TypeDescriptorClass_DT_ENUM");

class TypeDescriptorClass_DT_FLAG : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_FLAG() {}
    TypeDescriptorClass_DT_FLAG() : XTypeDescriptor("DT_FLAG") {}

    // Original DescribedType.h line 1740.
    virtual int32 SizeOf() const
    {
        DisplayInternalError(0, "", 1743, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 1747.
    virtual BOOL FromStringFull(const TypeDescriptorField & tField, const BBETranslationData &, void * pData, const CRefString & szData) const
    {
        if (atoi(szData)) *(uint32 *)pData |= 1 << tField.nBit;
        else *(uint32 *)pData &= ~(1 << tField.nBit);
        return !(szData == "");
    }

    // Original DescribedType.h line 1762.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.vDefaultFieldValue;
        if ((int)(long)pDefaultData)
            *(unsigned int *)pData |= 1 << tField.nBit;
        else
            *(unsigned int *)pData &= ~(1 << tField.nBit);
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField & field) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField & field) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 1820.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        field.nBitCount = 1;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_FLAG) == 0x38, "TypeDescriptorClass_DT_FLAG");

class TypeDescriptorClass_DT_TRANSLATEABLE : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_TRANSLATEABLE() {}
    TypeDescriptorClass_DT_TRANSLATEABLE() : XTypeDescriptor("DT_TRANSLATEABLE") {}

    // Original DescribedType.h line 1868.
    virtual int32 SizeOf() const
    { return sizeof(int32); }

    // Original DescribedType.h line 1874.
    virtual BOOL FromStringFull(const TypeDescriptorField & tField, const BBETranslationData &, void * pData, const CRefString & szData) const
    {
        *(int32 *)pData = tField.pfnStringToInt(szData);
        return !(szData == "");
    }

    // Original DescribedType.h line 1889.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const void * pDefaultData = tField.GetDefaultData();
        *(int32 *)pData = (intptr)pDefaultData;
    }
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31

    // Original DescribedType.h line 1932.
    virtual int Encode(void *, const TypeDescriptorField &, XEncoder &) const
    {
        DisplayInternalError(0, "", 1937, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 1941.
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const
    {
        DisplayInternalError(0, "", 1946, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 1950.
    virtual void Fuzz(void *, const TypeDescriptorField &, RNG &) const
    {
        DisplayInternalError(0, "", 1955, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 1958.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        field.nBitCount = 0;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_TRANSLATEABLE) == 0x38, "TypeDescriptorClass_DT_TRANSLATEABLE");

class TypeDescriptorClass_DT_FIXEDARRAY : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_FIXEDARRAY() {}
    TypeDescriptorClass_DT_FIXEDARRAY() : XTypeDescriptor("DT_FIXEDARRAY") {}

    // Original DescribedType.h line 1976.
    virtual int32 SizeOf() const
    {
        DisplayInternalError(NULL, "", 1979, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 1983.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString("<array>");
    }

    // Original DescribedType.h line 1990.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        const XTypeDescriptor *ptAuxTypeDesc = tField.GetAuxTypeDesc();
        const void *pDefaultData = tField.GetDefaultData();
        int32 nSizeOfElement = ptAuxTypeDesc->SizeOf();
        int32 nCount = tField.GetCount();
        uint8 *pbData = (uint8 *)pData;
        if (pDefaultData) {
            for (int32 i = 0; i < nCount; ++i) { SigmaMemoryCopy(pbData, pDefaultData, nSizeOfElement); pbData += nSizeOfElement; }
        } else {
            TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);
            for (int32 i = 0; i < nCount; ++i) { ptAuxTypeDesc->LoadFromDefaultData(fieldAux, pbData); pbData += nSizeOfElement; }
        }
    }

    // Original DescribedType.h line 2019.
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->ContainsSNOSubfield(fieldAux);
    }

    // Original DescribedType.h line 2026.
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->ContainsAllocationSubfields(fieldAux);
    }

    // Original DescribedType.h line 2033.
    virtual void ScanForSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        field.GetAuxTypeDesc()->ScanForSubfields(fieldAux);
    }
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const;  // vtable slot 20
    virtual void ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const;  // vtable slot 21
    virtual void DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const;  // vtable slot 22
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField & field) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const;  // vtable slot 25
    virtual void AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const;  // vtable slot 26
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const;  // vtable slot 28
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual void Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 32
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 2136.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        int32 nBitCount = 0;
        if (field.dwFlags & (1 << TDF_VARIABLE_COUNT)) nBitCount = Math::LogBase2(field.GetCount()) + 1;
        field.nArraySizeBitCount = nBitCount;
        const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
        ptAuxTypeDesc->CalculateNetworkBitCounts(field);
    }
    virtual void CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const;  // vtable slot 37

    // Original DescribedType.h line 2157.
    virtual BOOL Contains64BitSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->Contains64BitSubfields(fieldAux);
    }

    // Original DescribedType.h line 2164.
    virtual uint32 GetSub64BitPlatformMask(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->GetSub64BitPlatformMask(fieldAux);
    }

    // Original DescribedType.h line 2171.
    virtual BOOL Contains128BitSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->Contains128BitSubfields(fieldAux);
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_FIXEDARRAY) == 0x38, "TypeDescriptorClass_DT_FIXEDARRAY");

class TypeDescriptorClass_DT_VARIABLEARRAY : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_VARIABLEARRAY() {}
    TypeDescriptorClass_DT_VARIABLEARRAY() : XTypeDescriptor("DT_VARIABLEARRAY") {}
    TypeDescriptorClass_DT_VARIABLEARRAY(LPCSTR szClassName) : XTypeDescriptor(szClassName) {}

    // Original DescribedType.h line 2236.
    virtual int32 SizeOf() const
    {
        DisplayInternalError(NULL, "", 2239, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 2243.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString("<var_array>");
    }
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12

    // Original DescribedType.h line 2254.
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField & field) const
    {
        const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
        TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);
        return ptAuxTypeDesc->ContainsSNOSubfield(fieldAux);
    }

    // Original DescribedType.h line 2262.
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField &) const
    {
        return TRUE;
    }

    // Original DescribedType.h line 2268.
    virtual void ScanForSubfields(const TypeDescriptorField & field) const
    {
        const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
        TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);
        ptAuxTypeDesc->ScanForSubfields(fieldAux);
    }
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const;  // vtable slot 20
    virtual void ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const;  // vtable slot 21
    virtual void DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const;  // vtable slot 22
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField & field) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const;  // vtable slot 25
    virtual void AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const;  // vtable slot 26
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const;  // vtable slot 28
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual void Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 32
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34

    // Original DescribedType.h line 2367.
    virtual void Fuzz(void *, const TypeDescriptorField &, RNG &) const
    {
        DisplayInternalError(NULL, "", 2373, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original DescribedType.h line 2376.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        field.nBitCount = 0;
    }

    // Original DescribedType.h line 2382.
    virtual BOOL Contains64BitSubfields(const TypeDescriptorField & field) const
    {
        const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
        TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);
        return ptAuxTypeDesc->Contains64BitSubfields(fieldAux);
    }

    // Original DescribedType.h line 2390.
    virtual uint32 GetSub64BitPlatformMask(const TypeDescriptorField & field) const
    {
        const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
        TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);
        return ptAuxTypeDesc->GetSub64BitPlatformMask(fieldAux);
    }

    // Original DescribedType.h line 2398.
    virtual BOOL Contains128BitSubfields(const TypeDescriptorField & field) const
    {
        const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
        TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);
        return ptAuxTypeDesc->Contains128BitSubfields(fieldAux);
    }
private:
    static void sGetArrayCountAndPtr(const void * lpDefinition, const TypeDescriptorField & field, const XTypeDescriptor * ptAuxTypeDesc, int32 nAuxTypeSize, int32 & nCount, void *& pArray);
};
static_assert(sizeof(TypeDescriptorClass_DT_VARIABLEARRAY) == 0x38, "TypeDescriptorClass_DT_VARIABLEARRAY");

class TypeDescriptorClass_DT_VARIABLEARRAY_COUNT : public TypeDescriptorClass_DT_INT {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_VARIABLEARRAY_COUNT() {}
    TypeDescriptorClass_DT_VARIABLEARRAY_COUNT() {}

    // Original DescribedType.h line 2470.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        CRefString emptyString;
        TypeDescriptorFieldSerializationInfo fieldSerializationInfo(tField, pData);
        FromXMLSerializedString(fieldSerializationInfo, emptyString, pData);
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_VARIABLEARRAY_COUNT) == 0x38, "TypeDescriptorClass_DT_VARIABLEARRAY_COUNT");

class TypeDescriptorClass_DT_CSTRING : public TypeDescriptorClass_DT_VARIABLEARRAY {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_CSTRING() {}
    TypeDescriptorClass_DT_CSTRING() : TypeDescriptorClass_DT_VARIABLEARRAY("DT_CSTRING") {}
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL &) const;  // vtable slot 18

    // Original DescribedType.h line 2507.
    virtual TDVariant ToVariant(const void * pData) const
    {
        return TDVariant((LPCSTR)pData);
    }

    // Original DescribedType.h line 2513.
    virtual int VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const
    {
        return strcmp(tValue1.GetStr(), tValue2.GetStr()) == 0;
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_CSTRING) == 0x38, "TypeDescriptorClass_DT_CSTRING");

class TypeDescriptorClass_DT_OPTIONAL : public XTypeDescriptor {  // size 0x38
public:
    virtual ~TypeDescriptorClass_DT_OPTIONAL() {}
    TypeDescriptorClass_DT_OPTIONAL() : XTypeDescriptor("DT_OPTIONAL") {}

    // Original DescribedType.h line 2533.
    virtual int32 SizeOf() const
    {
        return 4;
    }

    // Original DescribedType.h line 2539.
    virtual CRefString ToString(const void * pData) const
    {
        return CRefString("<optional>");
    }

    // Original DescribedType.h line 2546.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        TypeDescriptorField fieldAux(tField.GetAuxTypeDesc(), NULL);
        BOOL *pbExists = (BOOL *)((uint8 *)pData + tField.GetAuxTypeDesc()->SizeOf());
        *pbExists = FALSE;
    }

    // Original DescribedType.h line 2556.
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->ContainsSNOSubfield(fieldAux);
    }

    // Original DescribedType.h line 2563.
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->ContainsAllocationSubfields(fieldAux);
    }

    // Original DescribedType.h line 2570.
    virtual void ScanForSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        field.GetAuxTypeDesc()->ScanForSubfields(fieldAux);
    }
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const;  // vtable slot 20
    virtual void ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const;  // vtable slot 21
    virtual void DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const;  // vtable slot 22
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField & field) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const;  // vtable slot 25
    virtual void AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const;  // vtable slot 26
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const;  // vtable slot 28
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35

    // Original DescribedType.h line 2667.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
        ptAuxTypeDesc->CalculateNetworkBitCounts(field);
    }
    virtual void CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const;  // vtable slot 37

    // Original DescribedType.h line 2679.
    virtual BOOL Contains64BitSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->Contains64BitSubfields(fieldAux);
    }

    // Original DescribedType.h line 2686.
    virtual uint32 GetSub64BitPlatformMask(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->GetSub64BitPlatformMask(fieldAux);
    }

    // Original DescribedType.h line 2693.
    virtual BOOL Contains128BitSubfields(const TypeDescriptorField & field) const
    {
        TypeDescriptorField fieldAux(field.GetAuxTypeDesc(), NULL);
        return field.GetAuxTypeDesc()->Contains128BitSubfields(fieldAux);
    }
};
static_assert(sizeof(TypeDescriptorClass_DT_OPTIONAL) == 0x38, "TypeDescriptorClass_DT_OPTIONAL");


