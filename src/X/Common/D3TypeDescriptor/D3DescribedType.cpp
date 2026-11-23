// Original file: X/Common/D3TypeDescriptor/D3DescribedType.cpp
// Reconstructed from this binary's DWARF, code, and descriptor data.

#include "X/Sigma/TypeDescriptor/TypeDescriptor.h"
#include "X/Common/SNO/SNOFiles.h"
#include "X/Common/Attributes/FastAttribConstantsSwig.h"
#include "X/Common/GameBalance.h"
#include "X/Sigma/Networking/XEncoder.h"
#include "X/Sigma/Random.h"
#include "X/Common/Definitions/GameBalanceDefinition.h"
#include "X/Common/Attributes/FastAttribute.h"
#include "X/Sigma/XML/XMLString.h"
#include "X/Sigma/Math/MathGeneral.h"
#include <stdlib.h>
#include "X/Sigma/XML/SAXHelper.h"
#include "X/Common/TagMaps/AnimTags.h"
#include "X/Common/Definitions/GlobalDefinition.h"
#include "X/Common/BBE.h"
#include "X/Common/TagMaps/Tags.h"
#include "X/Common/GlobalSNO.h"
#include "X/Common/SNO/SNOName.h"
#include "X/Sigma/Files/XFilesCommon.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "X/Common/TagMaps/TagNumbers.h"
#include <cstring>
#include "BlizzardCore/Include/Blizzard/Util/Crypto.h"

void * sLoadSerializedTracked(const UnserializeData &, const SerializeData &, uint32, LPCSTR, int32, LPCSTR);

class TypeDescriptorClass_DT_SNO : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_SNO(LPCSTR szClassName) : XTypeDescriptor(szClassName) {}
    TypeDescriptorClass_DT_SNO() : XTypeDescriptor("DT_SNO") {}
    virtual ~TypeDescriptorClass_DT_SNO() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField & tField, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const;  // vtable slot 13
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const;  // vtable slot 21
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
protected:
    virtual SNOGroup GetSNOGroupFilter(const TypeDescriptorField & tField, const void * pData) const;  // vtable slot 41
};

class TypeDescriptorClass_DT_SNONAME_HANDLE : public TypeDescriptorClass_DT_SNO {  // size 0x38
public:
    TypeDescriptorClass_DT_SNONAME_HANDLE() : TypeDescriptorClass_DT_SNO("DT_SNONAME_HANDLE") {}
    virtual ~TypeDescriptorClass_DT_SNONAME_HANDLE() {}
protected:
    virtual SNOGroup GetSNOGroupFilter(const TypeDescriptorField & tField, const void * pData) const;  // vtable slot 41
};

class TypeDescriptorClass_DT_SNO_GROUP : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_SNO_GROUP() : XTypeDescriptor("DT_SNO_GROUP") {}
    virtual ~TypeDescriptorClass_DT_SNO_GROUP() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const;  // vtable slot 13
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_TIME : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_TIME() : XTypeDescriptor("DT_TIME") {}
    virtual int32 SizeOf() const;  // vtable slot 3
    static float ConvertTicksToSeconds(int32 nTicks) { return (float)nTicks / 60.0f; }
    static int32 ConvertTicksFromSeconds(float flSeconds) { return Math::FloatToInt(flSeconds * 60.0f); }
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static_assert(sizeof(TypeDescriptorClass_DT_TIME) == 0x38, "TypeDescriptorClass_DT_TIME");

class TypeDescriptorClass_DT_TIME_SHORT : public TypeDescriptorClass_DT_TIME {  // size 0x38
public:
    TypeDescriptorClass_DT_TIME_SHORT() {}
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
};

static_assert(sizeof(TypeDescriptorClass_DT_TIME_SHORT) == 0x38, "TypeDescriptorClass_DT_TIME_SHORT");

class TypeDescriptorClass_DT_FORMULA : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_FORMULA() : XTypeDescriptor("DT_FORMULA") {}
    virtual ~TypeDescriptorClass_DT_FORMULA() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual BOOL FromStringFull(const TypeDescriptorField & tField, const BBETranslationData & bbeTranslationData, void * pData, const CRefString & szData) const;  // vtable slot 11
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField &) const;  // vtable slot 14
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField & field) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const;  // vtable slot 25
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual void Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 32
    virtual BOOL Encode(void *, const TypeDescriptorField &, XEncoder &) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_GBID : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_GBID() : XTypeDescriptor("DT_GBID") {}
    virtual ~TypeDescriptorClass_DT_GBID() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_ATTRIBUTEPARAM : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_ATTRIBUTEPARAM() : XTypeDescriptor("DT_ATTRIBUTEPARAM") {}
    virtual ~TypeDescriptorClass_DT_ATTRIBUTEPARAM() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
    Attrib GetAttrib(const void * pData) const;
};

class TypeDescriptorClass_DT_VELOCITY : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_VELOCITY() : XTypeDescriptor("DT_VELOCITY") {}
    virtual int32 SizeOf() const;  // vtable slot 3
    static float ConvertFeetPerTickToFeetPerSecond(float flFtPerTick) { return flFtPerTick * 60.0f; }
    static float ConvertFeetPerTickFromFeetPerSecond(float flFtPerTick) { return flFtPerTick / 60.0f; }
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static_assert(sizeof(TypeDescriptorClass_DT_VELOCITY) == 0x38, "TypeDescriptorClass_DT_VELOCITY");

class TypeDescriptorClass_DT_ACCEL : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_ACCEL() : XTypeDescriptor("DT_ACCEL") {}
    virtual int32 SizeOf() const;  // vtable slot 3
    static float ConvertFtPerSqTickToFtPerSqSeconds(float flFeetPerSqTick) { return flFeetPerSqTick * 3600.0f; }
    static float ConvertFtPerSqTickFromFtPerSqSeconds(float flFeetPerSqSec) { return flFeetPerSqSec / 3600.0f; }
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static_assert(sizeof(TypeDescriptorClass_DT_ACCEL) == 0x38, "TypeDescriptorClass_DT_ACCEL");

class TypeDescriptorClass_DT_ANGULARVELOCITY : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_ANGULARVELOCITY() : XTypeDescriptor("DT_ANGULARVELOCITY") {}
    virtual int32 SizeOf() const;  // vtable slot 3
    static float ConvertAngleToDegreesPerSecond(Angle aAngle) { return Math::AngleToDeg(aAngle) * 60.0f; }
    static Angle ConvertAngleFromDegreesPerSecond(float flDegPerSec) { return Math::DegToAngle(flDegPerSec / 60.0f); }
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static_assert(sizeof(TypeDescriptorClass_DT_ANGULARVELOCITY) == 0x38, "TypeDescriptorClass_DT_ANGULARVELOCITY");

class TypeDescriptorClass_DT_ANGULARACCEL : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_ANGULARACCEL() : XTypeDescriptor("DT_ANGULARACCEL") {}
    virtual int32 SizeOf() const;  // vtable slot 3
    static float ConvertAngleToDegreesPerSecondsSquared(Angle aAngle) { return Math::AngleToDeg(aAngle) * 3600.0f; }
    static Angle ConvertAngleFromDegreesPerSecondSquared(float flDegPerSec) { return Math::DegToAngle(flDegPerSec / 3600.0f); }
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static_assert(sizeof(TypeDescriptorClass_DT_ANGULARACCEL) == 0x38, "TypeDescriptorClass_DT_ANGULARACCEL");

class TypeDescriptorClass_DT_MASS : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_MASS() : XTypeDescriptor("DT_MASS") {}
    virtual int32 SizeOf() const;  // vtable slot 3
    static float ConvertSlugsToPounds(float flSlugs) { return flSlugs / 0.03105589933693409f; }
    static float ConvertSlugsFromPounds(float flLbs) { return flLbs * 0.03105589933693409f; }
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static_assert(sizeof(TypeDescriptorClass_DT_MASS) == 0x38, "TypeDescriptorClass_DT_MASS");

class TypeDescriptorClass_DT_IMPULSE : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_IMPULSE() : XTypeDescriptor("DT_IMPULSE") {}
    virtual int32 SizeOf() const;  // vtable slot 3
    static float ConvertSlugFtPerTickToLbsPerSec(float flSlugFtPerTick) { return flSlugFtPerTick * 60.0f; }
    static float ConvertSlugFtPerTickFromLbsPerSec(float flLbsPerSec) { return flLbsPerSec / 60.0f; }
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static_assert(sizeof(TypeDescriptorClass_DT_IMPULSE) == 0x38, "TypeDescriptorClass_DT_IMPULSE");

class TypeDescriptorClass_DT_TAGMAP : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_TAGMAP() : XTypeDescriptor("DT_TAGMAP") {}
    virtual ~TypeDescriptorClass_DT_TAGMAP() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const;  // vtable slot 13
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField &) const;  // vtable slot 14
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const;  // vtable slot 21
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const;  // vtable slot 28
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual void Delete(const uint32, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface *) const;  // vtable slot 32
    virtual BOOL Encode(void *, const TypeDescriptorField &, XEncoder &) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_ANIM_TAG : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_ANIM_TAG() : XTypeDescriptor("DT_ANIM_TAG") {}
    virtual ~TypeDescriptorClass_DT_ANIM_TAG() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const;  // vtable slot 13
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField &) const;  // vtable slot 14
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL &) const;  // vtable slot 18
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual BOOL Encode(void *, const TypeDescriptorField &, XEncoder &) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_STRING_FORMULA : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_STRING_FORMULA() : XTypeDescriptor("DT_STRING_FORMULA") {}
    virtual ~TypeDescriptorClass_DT_STRING_FORMULA() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual BOOL VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const;  // vtable slot 10
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsAllocationSubfields(const TypeDescriptorField &) const;  // vtable slot 14
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const;  // vtable slot 25
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const;  // vtable slot 27
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const;  // vtable slot 31
    virtual void Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 32
    virtual BOOL Encode(void *, const TypeDescriptorField &, XEncoder &) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_ACTOR_GROUP : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_ACTOR_GROUP() : XTypeDescriptor("DT_ACTOR_GROUP") {}
    virtual ~TypeDescriptorClass_DT_ACTOR_GROUP() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const;  // vtable slot 13
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_ACTOR_ATTRIBUTE : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_ACTOR_ATTRIBUTE() : XTypeDescriptor("DT_ACTOR_ATTRIBUTE") {}
    virtual ~TypeDescriptorClass_DT_ACTOR_ATTRIBUTE() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const;  // vtable slot 13
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

class TypeDescriptorClass_DT_STARTLOC_NAME : public XTypeDescriptor {  // size 0x38
public:
    TypeDescriptorClass_DT_STARTLOC_NAME() : XTypeDescriptor("DT_STARTLOC_NAME") {}
    virtual ~TypeDescriptorClass_DT_STARTLOC_NAME() {}
    virtual int32 SizeOf() const;  // vtable slot 3
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual TDVariant ToVariant(const void * pData) const;  // vtable slot 8
    virtual BOOL FromVariant(void * pData, const TDVariant & tValue) const;  // vtable slot 9
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const;  // vtable slot 12
    virtual BOOL ContainsSNOSubfield(const TypeDescriptorField &) const;  // vtable slot 13
    virtual TagVariantType GetTagVariantType() const;  // vtable slot 23
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const;  // vtable slot 19
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

static const TypeDescriptorClass_DT_SNO TypeDescriptor_DT_SNO;
const XTypeDescriptor * DT_SNO = &TypeDescriptor_DT_SNO;
static const TypeDescriptorClass_DT_SNONAME_HANDLE TypeDescriptor_DT_SNONAME_HANDLE;
const XTypeDescriptor * DT_SNONAME_HANDLE = &TypeDescriptor_DT_SNONAME_HANDLE;
static const TypeDescriptorClass_DT_SNO_GROUP TypeDescriptor_DT_SNO_GROUP;
const XTypeDescriptor * DT_SNO_GROUP = &TypeDescriptor_DT_SNO_GROUP;
static TypeDescriptorClass_DT_TIME TypeDescriptor_DT_TIME;
const XTypeDescriptor * DT_TIME = &TypeDescriptor_DT_TIME;
static TypeDescriptorClass_DT_TIME_SHORT TypeDescriptor_DT_TIME_SHORT;
const XTypeDescriptor * DT_TIME_SHORT = &TypeDescriptor_DT_TIME_SHORT;
static const TypeDescriptorClass_DT_FORMULA TypeDescriptor_DT_FORMULA;
const XTypeDescriptor * DT_FORMULA = &TypeDescriptor_DT_FORMULA;
static const TypeDescriptorClass_DT_GBID TypeDescriptor_DT_GBID;
const XTypeDescriptor * DT_GBID = &TypeDescriptor_DT_GBID;
static const TypeDescriptorClass_DT_ATTRIBUTEPARAM TypeDescriptor_DT_ATTRIBUTEPARAM;
const XTypeDescriptor * DT_ATTRIBUTEPARAM = &TypeDescriptor_DT_ATTRIBUTEPARAM;
static TypeDescriptorClass_DT_VELOCITY TypeDescriptor_DT_VELOCITY;
const XTypeDescriptor * DT_VELOCITY = &TypeDescriptor_DT_VELOCITY;
static TypeDescriptorClass_DT_ACCEL TypeDescriptor_DT_ACCEL;
const XTypeDescriptor * DT_ACCEL = &TypeDescriptor_DT_ACCEL;
static TypeDescriptorClass_DT_ANGULARVELOCITY TypeDescriptor_DT_ANGULARVELOCITY;
const XTypeDescriptor * DT_ANGULARVELOCITY = &TypeDescriptor_DT_ANGULARVELOCITY;
static TypeDescriptorClass_DT_ANGULARACCEL TypeDescriptor_DT_ANGULARACCEL;
const XTypeDescriptor * DT_ANGULARACCEL = &TypeDescriptor_DT_ANGULARACCEL;
static TypeDescriptorClass_DT_MASS TypeDescriptor_DT_MASS;
const XTypeDescriptor * DT_MASS = &TypeDescriptor_DT_MASS;
static TypeDescriptorClass_DT_IMPULSE TypeDescriptor_DT_IMPULSE;
const XTypeDescriptor * DT_IMPULSE = &TypeDescriptor_DT_IMPULSE;
static const TypeDescriptorClass_DT_TAGMAP TypeDescriptor_DT_TAGMAP;
const XTypeDescriptor * DT_TAGMAP = &TypeDescriptor_DT_TAGMAP;
static const TypeDescriptorClass_DT_ANIM_TAG TypeDescriptor_DT_ANIM_TAG;
const XTypeDescriptor * DT_ANIM_TAG = &TypeDescriptor_DT_ANIM_TAG;
static const TypeDescriptorClass_DT_STRING_FORMULA TypeDescriptor_DT_STRING_FORMULA;
const XTypeDescriptor * DT_STRING_FORMULA = &TypeDescriptor_DT_STRING_FORMULA;
static const TypeDescriptorClass_DT_ACTOR_GROUP TypeDescriptor_DT_ACTOR_GROUP;
const XTypeDescriptor * DT_ACTOR_GROUP = &TypeDescriptor_DT_ACTOR_GROUP;
static const TypeDescriptorClass_DT_ACTOR_ATTRIBUTE TypeDescriptor_DT_ACTOR_ATTRIBUTE;
const XTypeDescriptor * DT_ACTOR_ATTRIBUTE = &TypeDescriptor_DT_ACTOR_ATTRIBUTE;
static const TypeDescriptorClass_DT_STARTLOC_NAME TypeDescriptor_DT_STARTLOC_NAME;
const XTypeDescriptor * DT_STARTLOC_NAME = &TypeDescriptor_DT_STARTLOC_NAME;

// Original line 55.
int TypeDescriptorClass_DT_SNO::SizeOf() const
{
    return (int)4;
}

// Original line 64.
CRefString TypeDescriptorClass_DT_SNO::ToString(const void *pData) const
{
    return SNOToString(-1, *(const int *)pData, 0);
}

// Original line 71.
void TypeDescriptorClass_DT_SNO::FromString(const TypeDescriptorField &tField, void *pData, const CRefString &szData) const {
    *(SNO *)pData = SNOFileLookupGUID(GetSNOGroupFilter(tField, pData), szData);
}

// Original line 79.
void TypeDescriptorClass_DT_SNO::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const void *pData, CRefString &sSerializeString) const {
    if (*(const SNO *)pData == SNO_NONE) sSerializeString = "";
    else if (*(const SNO *)pData == INVALID_SNO) sSerializeString = cgszInvalidSnoName;
    else {
        SNOGroup snoGroup = GetSNOGroupFilter(fieldSerializationInfo.GetField(),pData);
        sSerializeString = XMLStringConvertToXMLString(SNOToString(snoGroup,*(const SNO *)pData,1));
    }
}

// Original line 102.
BOOL TypeDescriptorClass_DT_SNO::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CRefString &sSerializeString, void *pData) const {
    if (!sSerializeString.Length()) { *(SNO *)pData = SNO_NONE; return 1; }
    const TypeDescriptorField &field = fieldSerializationInfo.GetField();
    SNOGroup snoGroup = GetSNOGroupFilter(field,pData);
    SNO sno = SNOFileLookupGUID(snoGroup,sSerializeString.str());
    if (snoGroup != SNOGROUP_NONE && (sno == SNO_NONE || sno == INVALID_SNO) && sSerializeString.Length()) {
        *(SNO *)pData = INVALID_SNO; return 0;
    }
    *(SNO *)pData = sno; return 1;
}

// Original line 130.
TDVariant TypeDescriptorClass_DT_SNO::ToVariant(const void *pData) const
{
    const int snoHandle = *(const int *)pData;
    return TDVariant(SNOToString(-1, snoHandle, 0), snoHandle);
}

// Original line 138.
int TypeDescriptorClass_DT_SNO::FromVariant(void *pData, const TDVariant &tValue) const
{
    if (tValue.GetSource() == TDVARIANT_SOURCE_INVALID)
        return 1;
    if (tValue.GetSource() == TDVARIANT_SOURCE_STRING) {
        DisplayInternalError(0, "", 148, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
    }
    *(int *)pData = tValue.GetInt();
    return 1;
}

// Original line 162.
void TypeDescriptorClass_DT_SNO::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const {
    const void *pDefaultData = tField.vDefaultFieldValue;
    *(SNO *)pData = pDefaultData ? SNOFileLookupGUID(GetSNOGroupFilter(tField,pData), (LPCSTR)pDefaultData) : SNO_NONE;
}

// Original line 177.
int TypeDescriptorClass_DT_SNO::ContainsSNOSubfield(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 183.
int TypeDescriptorClass_DT_SNO::GetTagVariantType() const
{
    return (int)2;
}

// Original line 189.
void TypeDescriptorClass_DT_SNO::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 199.
void TypeDescriptorClass_DT_SNO::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 207.
void TypeDescriptorClass_DT_SNO::IterateSNOReferences(const void *lpDefinition, const TypeDescriptorField &field, SNOIterateFunc pfuncIterate, void *pData) const {
    SNO sno = *(const SNO *)lpDefinition;
    SNOGroup snoGroup = GetSNOGroupFilter(field,lpDefinition);
    if (snoGroup != SNOGROUP_NONE) {
        uint32 uFlags = 0;
        if (!(field.dwFlags & (1 << TDF_REQUIRED_SNO))) uFlags |= (1 << SI_CAN_BE_NONE);
        if (field.dwFlags & ((1 << TDF_SOFT_LINK) | (1 << TDF_AXE_ONLY))) uFlags |= (1 << SI_SOFT_LINK);
        if (field.dwFlags & (1 << TDF_PC_ONLY)) uFlags |= (1 << SI_PC_ONLY);
        BOOL bConsoleOnly = (field.dwFlags & ((1 << TDF_XBOXONE_ONLY) | (1 << TDF_PS4_ONLY) | (1 << TDF_NX64_ONLY))) != 0;
        if (bConsoleOnly) uFlags |= (1 << SI_CONSOLE_ONLY);
        SNOName snoName(snoGroup,sno);
        pfuncIterate(snoName,uFlags,pData);
    }
}

// Original line 239.
void TypeDescriptorClass_DT_SNO::ChangeSNOReference(const void *lpDefinition, const TypeDescriptorField &field, const SNOName &snonameOld, const SNO snoNew) const {
    SNOGroup snoGroup = GetSNOGroupFilter(field,lpDefinition);
    SNO *psno = (SNO *)lpDefinition;
    if (snoGroup == snonameOld.snoGroup && *psno == snonameOld.snoHandle) *psno = snoNew;
}

// Original line 255.
void TypeDescriptorClass_DT_SNO::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 268.
void TypeDescriptorClass_DT_SNO::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 276.
int TypeDescriptorClass_DT_SNO::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition, field.nBitCount);
    return 1;
}

// Original line 290.
XDecoder::Result TypeDescriptorClass_DT_SNO::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 298.
void TypeDescriptorClass_DT_SNO::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 306.
void TypeDescriptorClass_DT_SNO::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 313.
int TypeDescriptorClass_DT_SNO::GetSNOGroupFilter(const TypeDescriptorField &tField, const void *pData) const
{
    return tField.dwFilter;
}

// Original line 332.
int TypeDescriptorClass_DT_SNONAME_HANDLE::GetSNOGroupFilter(const TypeDescriptorField &tField, const void *pData) const
{
    return ((const int *)pData)[-1];
}

// Original line 351.
int TypeDescriptorClass_DT_SNO_GROUP::SizeOf() const
{
    return (int)4;
}

// Original line 360.
CRefString TypeDescriptorClass_DT_SNO_GROUP::ToString(const void *pData) const
{
    return SNOGroupToString(*(const int *)pData, 0);
}

// Original line 366.
void TypeDescriptorClass_DT_SNO_GROUP::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(int *)pData = StringToSNOGroup(szData);
}

// Original line 374.
void TypeDescriptorClass_DT_SNO_GROUP::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const {
    if (*(const SNOGroup *)pData == SNOGROUP_NONE) sSerializeString = "";
    else sSerializeString = XMLStringConvertToXMLString(SNOGroupToString(*(const SNOGroup *)pData,1));
}

// Original line 389.
BOOL TypeDescriptorClass_DT_SNO_GROUP::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CRefString &sSerializeString, void *pData) const
{
    *(int *)pData = StringToSNOGroup(sSerializeString);
    return TRUE;
}

// Original line 398.
TDVariant TypeDescriptorClass_DT_SNO_GROUP::ToVariant(const void *pData) const
{
    const int snoGroup = *(const int *)pData;
    return TDVariant(SNOGroupToString(-1, snoGroup), snoGroup);
}

// Original line 406.
int TypeDescriptorClass_DT_SNO_GROUP::FromVariant(void *pData, const TDVariant &tValue) const
{
    if (tValue.GetSource() == TDVARIANT_SOURCE_INVALID)
        return 1;
    if (tValue.GetSource() == TDVARIANT_SOURCE_STRING)
        *(int *)pData = StringToSNOGroup(tValue.GetStr());
    else
        *(int *)pData = tValue.GetUInt();
    return 1;
}

// Original line 429.
void TypeDescriptorClass_DT_SNO_GROUP::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const
{
    if (tField.vDefaultFieldValue)
        *(int *)pData = StringToSNOGroup((LPCSTR)tField.vDefaultFieldValue);
    else
        *(int *)pData = -1;
}

// Original line 444.
int TypeDescriptorClass_DT_SNO_GROUP::ContainsSNOSubfield(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 450.
int TypeDescriptorClass_DT_SNO_GROUP::GetTagVariantType() const
{
    return (int)0;
}

// Original line 456.
void TypeDescriptorClass_DT_SNO_GROUP::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 466.
void TypeDescriptorClass_DT_SNO_GROUP::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 474.
void TypeDescriptorClass_DT_SNO_GROUP::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 487.
void TypeDescriptorClass_DT_SNO_GROUP::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 495.
int TypeDescriptorClass_DT_SNO_GROUP::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition, field.nBitCount);
    return 1;
}

// Original line 509.
XDecoder::Result TypeDescriptorClass_DT_SNO_GROUP::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 517.
void TypeDescriptorClass_DT_SNO_GROUP::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 525.
void TypeDescriptorClass_DT_SNO_GROUP::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 544.
int TypeDescriptorClass_DT_TIME::SizeOf() const
{
    return (int)4;
}

// Original line 565.
CRefString TypeDescriptorClass_DT_TIME::ToString(const void *pData) const
{
    return CRefString().Format("%.2f sec", ConvertTicksToSeconds(*(const int *)pData));
}

// Original line 571.
void TypeDescriptorClass_DT_TIME::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(int *)pData = ConvertTicksFromSeconds((float)atof(szData));
}

// Original line 579.
void TypeDescriptorClass_DT_TIME::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, ConvertTicksToSeconds(*(const int *)pData));
}

// Original line 587.
BOOL TypeDescriptorClass_DT_TIME::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CRefString &sSerializeString, void *pData) const
{
    *(int *)pData = ConvertTicksFromSeconds((float)atof(sSerializeString));
    return TRUE;
}

// Original line 596.
TDVariant TypeDescriptorClass_DT_TIME::ToVariant(const void *pData) const
{
    const float flSeconds = ConvertTicksToSeconds(*(const int *)pData);
    return TDVariant(ToString(pData), flSeconds);
}

// Original line 604.
int TypeDescriptorClass_DT_TIME::FromVariant(void *pData, const TDVariant &tValue) const
{
    *(int *)pData = ConvertTicksFromSeconds(tValue.GetFloat());
    return 1;
}

// Original line 612.
void TypeDescriptorClass_DT_TIME::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    const void * pDefaultData = tField.GetDefaultData();
    *(uint32 *)pData = (uint32)(uintptr)pDefaultData;
}

// Original line 620.
void TypeDescriptorClass_DT_TIME::SerializeFromXML(TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CXMLNode &node, void *pData, BOOL &fContainsBadLink) const
{
    DisplayInternalError(0, "", 627, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 647.
int TypeDescriptorClass_DT_TIME::GetTagVariantType() const
{
    return (int)0;
}

// Original line 653.
void TypeDescriptorClass_DT_TIME::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 663.
void TypeDescriptorClass_DT_TIME::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 671.
void TypeDescriptorClass_DT_TIME::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 684.
void TypeDescriptorClass_DT_TIME::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 692.
int TypeDescriptorClass_DT_TIME::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition, field.nBitCount);
    return 1;
}

// Original line 706.
XDecoder::Result TypeDescriptorClass_DT_TIME::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 714.
void TypeDescriptorClass_DT_TIME::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 722.
void TypeDescriptorClass_DT_TIME::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 741.
CRefString TypeDescriptorClass_DT_TIME_SHORT::ToString(const void *pData) const
{
    const float flSeconds = ConvertTicksToSeconds(*(const int *)pData);
    return CRefString().Format("%.1f", flSeconds);
}

// Original line 761.
int32 TypeDescriptorClass_DT_FORMULA::SizeOf() const
{
    DisplayInternalError(0, "", 764, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 768.
BOOL TypeDescriptorClass_DT_FORMULA::FromStringFull(const TypeDescriptorField & tField, const BBETranslationData & bbeTranslationData, void * pData, const CRefString & szData) const
{
    XPtr64<uint8> & rpFormula64 = *(XPtr64<uint8> *)pData;
    SerializeData & serFormula = tField.GetSerializeDataRaw(pData);
    serFormula.dwOffset = 0;
    if (szData == "") {
        serFormula.dwSize = 0;
        rpFormula64 = NULL;
        return FALSE;
    }
    rpFormula64 = (uint8 *)SigmaMemoryNew(1024, 4, NULL, TRUE);
    serFormula.dwSize = BBEFromString(szData, rpFormula64, 1024, bbeTranslationData);
    if (!serFormula.dwSize) {
        serFormula.dwSize = 0;
        rpFormula64 = NULL;
        return FALSE;
    }
    return szData != "";
}

// Original line 810.
void TypeDescriptorClass_DT_FORMULA::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const
{
    *(void **)pData = NULL;
}

// Original line 818.
int TypeDescriptorClass_DT_FORMULA::ContainsAllocationSubfields(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 824.
void TypeDescriptorClass_DT_FORMULA::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    const SerializeData & serFormula = field.GetSerializeDataRaw(pData);
    BBEToString(*(uint8 *const *)pData, serFormula.dwSize, BBEFunctions, NUM_BBEFUNCTIONS, sSerializeString);
}

// Original line 838.
BOOL TypeDescriptorClass_DT_FORMULA::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    BBETranslationData bbeTranslationData = { -1, -1 };
    XPtr64<uint8> & rpFormula64 = *(XPtr64<uint8> *)pData;
    rpFormula64 = (uint8 *)SigmaMemoryNew(1024, 4, NULL, TRUE);
    SerializeData & serFormula = field.GetSerializeDataRaw(pData);
    serFormula.dwOffset = 0;
    serFormula.dwSize = BBEFromString(sSerializeString, rpFormula64, 1024, bbeTranslationData);
    return TRUE;
}

// Original line 860.
void TypeDescriptorClass_DT_FORMULA::Byteswap(void *, void * lpDefinitionSrc, void *, const TypeDescriptorField & field) const
{
    field.GetSerializeData(lpDefinitionSrc);
    DisplayInternalError(NULL, "", 869, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 873.
void TypeDescriptorClass_DT_FORMULA::ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const
{
    SerializeData & ser = field.GetSerializeData(lpDefinition);
    uint8 * pFormula = (uint8 *)lpDefinitionBase + ser.dwOffset;
    BBEByteswap(pFormula, ser.dwSize);
}

// Original line 884.
void TypeDescriptorClass_DT_FORMULA::ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    SerializeData & ser = field.GetSerializeDataRaw(lpDefinition);
    FileSerialize(ser, dwOffset, 4);
}

// Original line 893.
void TypeDescriptorClass_DT_FORMULA::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, uint32, uint32) const
{
    SerializeData & ser = field.GetSerializeData(lpDefinition);
    if (ser.dwSize) {
        uint8 * pFormula = *(XPtr64<uint8> *)lpDefinition;
        SigmaMemoryCopy(lpDst + ser.dwOffset, pFormula, ser.dwSize);
        BBEByteswap(lpDst + ser.dwOffset, ser.dwSize);
    }
}

// Original line 914.
void TypeDescriptorClass_DT_FORMULA::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{
    XPtr64<uint8> & rpFormula64 = *(XPtr64<uint8> *)lpDefinition;
    SerializeData & ser = field.GetSerializeData(lpDefinition);
    if (ser.dwSize) {
        rpFormula64 = (uint8 *)sLoadSerializedTracked(tUnserializeData, ser, 8, "D:/BuildServer/2/work/code/branches/2_6_2/X/Common/D3TypeDescriptor/D3DescribedType.cpp", 929, field.szFieldName);
        BBEByteswap(rpFormula64, ser.dwSize);
    } else rpFormula64 = NULL;
}

// Original line 934.
void TypeDescriptorClass_DT_FORMULA::Delete(uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const
{
    XPtr64<uint8> & rpFormula64 = *(XPtr64<uint8> *)lpDefinition;
    SerializeData ser = field.GetSerializeDataRaw(lpDefinition);
    if (ser.dwSize) {
        if (dwDefinitionFlags & 4) return;
        SigmaMemoryFree(rpFormula64, ptMemoryInterface);
    }
    rpFormula64 = NULL;
}

// Original line 956.
int TypeDescriptorClass_DT_FORMULA::Encode(void *, const TypeDescriptorField &, XEncoder &) const
{
    DisplayInternalError(0, "", 961, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 965.
XDecoder::Result TypeDescriptorClass_DT_FORMULA::Decode(void *, const TypeDescriptorField &, XDecoder &) const
{
    DisplayInternalError(0, "", 970, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 974.
void TypeDescriptorClass_DT_FORMULA::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    DisplayInternalError(0, "", 979, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 982.
void TypeDescriptorClass_DT_FORMULA::CalculateNetworkBitCounts(TypeDescriptorField & field) const
{
    field.nBitCount = 0;
}

// Original line 1095.
int TypeDescriptorClass_DT_GBID::SizeOf() const
{
    return (int)4;
}

// Original line 1104.
CRefString TypeDescriptorClass_DT_GBID::ToString(const void *pData) const
{
    return GBIDToString(*(const int *)pData);
}

// Original line 1111.
void TypeDescriptorClass_DT_GBID::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(int *)pData = StringToGBID(szData);
}

// Original line 1119.
void TypeDescriptorClass_DT_GBID::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const void *pData, CRefString &sSerializeString) const {
    if (*(const GBID *)pData == -1) sSerializeString = "";
    else {
        GameBalanceType eGBType = fieldSerializationInfo.GetField().dwFilter;
        // Filter 47 selects an integer representation in the original; no enum name survives.
        if (eGBType == 47) DT_INT->ToXMLSerializedString(fieldSerializationInfo,pData,sSerializeString);
        else sSerializeString = XMLStringConvertToXMLString(GBIDToString(*(const GBID *)pData,eGBType,1));
    }
}

// Original line 1146.
BOOL TypeDescriptorClass_DT_GBID::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CRefString &sSerializeString, void *pData) const {
    GameBalanceType eGBType = fieldSerializationInfo.GetField().dwFilter;
    // Same original integer-representation filter as ToXMLSerializedString.
    if (eGBType == 47) return DT_INT->FromXMLSerializedString(fieldSerializationInfo,sSerializeString,pData);
    *(GBID *)pData = StringToGBID(sSerializeString); return 1;
}

// Original line 1167.
TDVariant TypeDescriptorClass_DT_GBID::ToVariant(const void *pData) const
{
    const int gbid = *(const int *)pData;
    return TDVariant(GBIDToString(gbid), gbid);
}

// Original line 1175.
int TypeDescriptorClass_DT_GBID::FromVariant(void *pData, const TDVariant &tValue) const
{
    if (tValue.GetSource() == TDVARIANT_SOURCE_INVALID)
        return 1;
    if (tValue.GetSource() == TDVARIANT_SOURCE_STRING)
        *(int *)pData = StringToGBID(tValue.GetStr());
    else
        *(int *)pData = tValue.GetUInt();
    return 1;
}

// Original line 1198.
void TypeDescriptorClass_DT_GBID::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const
{
    if (tField.vDefaultFieldValue)
        *(int *)pData = StringToGBID((LPCSTR)tField.vDefaultFieldValue);
    else
        *(int *)pData = -1;
}

// Original line 1213.
int TypeDescriptorClass_DT_GBID::GetTagVariantType() const
{
    return (int)3;
}

// Original line 1219.
void TypeDescriptorClass_DT_GBID::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 1229.
void TypeDescriptorClass_DT_GBID::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 1237.
void TypeDescriptorClass_DT_GBID::SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 1250.
void TypeDescriptorClass_DT_GBID::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 1258.
int TypeDescriptorClass_DT_GBID::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition);
    return 1;
}

// Original line 1272.
XDecoder::Result TypeDescriptorClass_DT_GBID::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 1280.
void TypeDescriptorClass_DT_GBID::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 1288.
void TypeDescriptorClass_DT_GBID::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 1307.
int TypeDescriptorClass_DT_ATTRIBUTEPARAM::SizeOf() const
{
    return (int)4;
}

// Original line 1316.
CRefString TypeDescriptorClass_DT_ATTRIBUTEPARAM::ToString(const void *pData) const
{
    return AttribParamToString(GetAttrib(pData), *(const int *)pData);
}

// Original line 1322.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(int *)pData = AttribStringToParam(GetAttrib(pData), szData);
}

// Original line 1330.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const {
    sSerializeString = XMLStringConvertToXMLString(ToString(pData));
}

// Original line 1338.
BOOL TypeDescriptorClass_DT_ATTRIBUTEPARAM::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CRefString &sSerializeString, void *pData) const {
    FromString(fieldSerializationInfo.GetField(),pData,CRefString(XMLStringConvertFromXMLString(sSerializeString))); return 1;
}

// Original line 1347.
TDVariant TypeDescriptorClass_DT_ATTRIBUTEPARAM::ToVariant(const void *pData) const
{
    return TDVariant(ToString(pData), *(const int *)pData);
}

// Original line 1353.
int TypeDescriptorClass_DT_ATTRIBUTEPARAM::FromVariant(void *pData, const TDVariant &tValue) const
{
    if (tValue.GetSource() == TDVARIANT_SOURCE_STRING)
        *(int *)pData = AttribStringToParam(GetAttrib(pData), tValue.GetStr());
    else
        *(int *)pData = tValue.GetInt();
    return 1;
}

// Original line 1371.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    *(unsigned int *)pData = (unsigned int)(unsigned long long)tField.vDefaultFieldValue;
}

// Original line 1379.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 1389.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 1397.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 1410.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 1418.
int TypeDescriptorClass_DT_ATTRIBUTEPARAM::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition);
    return 1;
}

// Original line 1432.
XDecoder::Result TypeDescriptorClass_DT_ATTRIBUTEPARAM::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 1440.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 1448.
void TypeDescriptorClass_DT_ATTRIBUTEPARAM::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 1454.
inline Attrib TypeDescriptorClass_DT_ATTRIBUTEPARAM::GetAttrib(const void *pData) const { return ((const Attrib *)pData)[-1]; }

// Original line 1473.
int TypeDescriptorClass_DT_VELOCITY::SizeOf() const
{
    return (int)4;
}

// Original line 1494.
CRefString TypeDescriptorClass_DT_VELOCITY::ToString(const void *pData) const
{
    return CRefString().Format("%.2f ft/s", ConvertFeetPerTickToFeetPerSecond(*(const float *)pData));
}

// Original line 1500.
void TypeDescriptorClass_DT_VELOCITY::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(float *)pData = ConvertFeetPerTickFromFeetPerSecond((float)atof(szData));
}

// Original line 1508.
void TypeDescriptorClass_DT_VELOCITY::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, ConvertFeetPerTickToFeetPerSecond(*(const float *)pData));
}

// Original line 1521.
BOOL TypeDescriptorClass_DT_VELOCITY::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(float *)pData = ConvertFeetPerTickFromFeetPerSecond((float)atof(sSerializeString));
    return TRUE;
}

// Original line 1534.
TDVariant TypeDescriptorClass_DT_VELOCITY::ToVariant(const void *pData) const
{
    const float flValue = ConvertFeetPerTickToFeetPerSecond(*(const float *)pData);
    return TDVariant(ToString(pData), flValue);
}

// Original line 1542.
int TypeDescriptorClass_DT_VELOCITY::FromVariant(void *pData, const TDVariant &tValue) const
{
    *(float *)pData = ConvertFeetPerTickFromFeetPerSecond(tValue.GetFloat());
    return 1;
}

// Original line 1553.
void TypeDescriptorClass_DT_VELOCITY::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    const void * pDefaultData = tField.GetDefaultData();
    *(uint32 *)pData = (uint32)(uintptr)pDefaultData;
}

// Original line 1561.
int TypeDescriptorClass_DT_VELOCITY::GetTagVariantType() const
{
    return (int)1;
}

// Original line 1567.
void TypeDescriptorClass_DT_VELOCITY::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 1577.
void TypeDescriptorClass_DT_VELOCITY::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 1585.
void TypeDescriptorClass_DT_VELOCITY::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 1598.
void TypeDescriptorClass_DT_VELOCITY::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 1606.
int TypeDescriptorClass_DT_VELOCITY::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericFloatEncode(lpDefinition, field, tEncoder);
}

// Original line 1614.
XDecoder::Result TypeDescriptorClass_DT_VELOCITY::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericFloatDecode(lpDefinition, field, tDecoder);
}

// Original line 1622.
void TypeDescriptorClass_DT_VELOCITY::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericFloatFuzz(lpDefinition, field, tRNG);
}

// Original line 1630.
void TypeDescriptorClass_DT_VELOCITY::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    if (field.dwFlags & (1 << TDF_XFLOAT16))
        nBitCount = 16;
    field.nBitCount = (short)nBitCount;
}

// Original line 1653.
int TypeDescriptorClass_DT_ACCEL::SizeOf() const
{
    return (int)4;
}

// Original line 1676.
CRefString TypeDescriptorClass_DT_ACCEL::ToString(const void *pData) const
{
    return CRefString().Format("%.2f ft/s^2", ConvertFtPerSqTickToFtPerSqSeconds(*(const float *)pData));
}

// Original line 1682.
void TypeDescriptorClass_DT_ACCEL::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(float *)pData = ConvertFtPerSqTickFromFtPerSqSeconds((float)atof(szData));
}

// Original line 1690.
void TypeDescriptorClass_DT_ACCEL::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, ConvertFtPerSqTickToFtPerSqSeconds(*(const float *)pData));
}

// Original line 1698.
BOOL TypeDescriptorClass_DT_ACCEL::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(float *)pData = ConvertFtPerSqTickFromFtPerSqSeconds((float)atof(sSerializeString));
    return TRUE;
}

// Original line 1707.
TDVariant TypeDescriptorClass_DT_ACCEL::ToVariant(const void *pData) const
{
    const float flValue = ConvertFtPerSqTickToFtPerSqSeconds(*(const float *)pData);
    return TDVariant(ToString(pData), flValue);
}

// Original line 1715.
int TypeDescriptorClass_DT_ACCEL::FromVariant(void *pData, const TDVariant &tValue) const
{
    *(float *)pData = ConvertFtPerSqTickFromFtPerSqSeconds(tValue.GetFloat());
    return 1;
}

// Original line 1726.
void TypeDescriptorClass_DT_ACCEL::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    const void * pDefaultData = tField.GetDefaultData();
    *(uint32 *)pData = (uint32)(uintptr)pDefaultData;
}

// Original line 1734.
int TypeDescriptorClass_DT_ACCEL::GetTagVariantType() const
{
    return (int)1;
}

// Original line 1740.
void TypeDescriptorClass_DT_ACCEL::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 1750.
void TypeDescriptorClass_DT_ACCEL::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 1758.
void TypeDescriptorClass_DT_ACCEL::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 1771.
void TypeDescriptorClass_DT_ACCEL::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 1779.
int TypeDescriptorClass_DT_ACCEL::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericFloatEncode(lpDefinition, field, tEncoder);
}

// Original line 1787.
XDecoder::Result TypeDescriptorClass_DT_ACCEL::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericFloatDecode(lpDefinition, field, tDecoder);
}

// Original line 1795.
void TypeDescriptorClass_DT_ACCEL::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericFloatFuzz(lpDefinition, field, tRNG);
}

// Original line 1803.
void TypeDescriptorClass_DT_ACCEL::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    if (field.dwFlags & (1 << TDF_XFLOAT16))
        nBitCount = 16;
    field.nBitCount = (short)nBitCount;
}

// Original line 1826.
int TypeDescriptorClass_DT_ANGULARVELOCITY::SizeOf() const
{
    return (int)4;
}

// Original line 1847.
CRefString TypeDescriptorClass_DT_ANGULARVELOCITY::ToString(const void *pData) const
{
    const float flDegPerSec = ConvertAngleToDegreesPerSecond(*(const float *)pData);
    return CRefString().Format("%.2f\xc2\xb0/s", flDegPerSec);
}

// Original line 1854.
void TypeDescriptorClass_DT_ANGULARVELOCITY::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(float *)pData = ConvertAngleFromDegreesPerSecond((float)atof(szData));
}

// Original line 1862.
void TypeDescriptorClass_DT_ANGULARVELOCITY::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    float flDegPerSec = ConvertAngleToDegreesPerSecond(*(const float *)pData);
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, flDegPerSec);
}

// Original line 1871.
BOOL TypeDescriptorClass_DT_ANGULARVELOCITY::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(float *)pData = ConvertAngleFromDegreesPerSecond((float)atof(sSerializeString));
    return TRUE;
}

// Original line 1880.
TDVariant TypeDescriptorClass_DT_ANGULARVELOCITY::ToVariant(const void *pData) const
{
    const float flDegPerSec = ConvertAngleToDegreesPerSecond(*(const float *)pData);
    return TDVariant(ToString(pData), flDegPerSec);
}

// Original line 1888.
int TypeDescriptorClass_DT_ANGULARVELOCITY::FromVariant(void *pData, const TDVariant &tValue) const
{
    *(float *)pData = ConvertAngleFromDegreesPerSecond(tValue.GetFloat());
    return 1;
}

// Original line 1899.
void TypeDescriptorClass_DT_ANGULARVELOCITY::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    const void * pDefaultData = tField.GetDefaultData();
    *(uint32 *)pData = (uint32)(uintptr)pDefaultData;
}

// Original line 1907.
int TypeDescriptorClass_DT_ANGULARVELOCITY::GetTagVariantType() const
{
    return (int)1;
}

// Original line 1913.
void TypeDescriptorClass_DT_ANGULARVELOCITY::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 1923.
void TypeDescriptorClass_DT_ANGULARVELOCITY::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 1931.
void TypeDescriptorClass_DT_ANGULARVELOCITY::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 1944.
void TypeDescriptorClass_DT_ANGULARVELOCITY::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 1952.
int TypeDescriptorClass_DT_ANGULARVELOCITY::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
        const float fMin = *(const float *)&field.vMinFieldValue;
        const float fValue = *(const float *)lpDefinition;
        const float fMax = *(const float *)&field.vMaxFieldValue;
        const bool bInRange = fValue >= fMin & fValue <= fMax;
        if (!bInRange) {
            DisplayInternalError(0, "", 1968, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        tEncoder.Put(*(const unsigned int *)&fValue);
    } else {
        tEncoder.Put(*(const unsigned int *)lpDefinition);
    }
    return 1;
}

// Original line 1976.
XDecoder::Result TypeDescriptorClass_DT_ANGULARVELOCITY::Decode(void *lpDefinition, const TypeDescriptorField &field, XDecoder &tDecoder) const
{
    uint32 dwDecodedVal;
    XDecoder::Result eResult = tDecoder.Get(dwDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
        const float fMin = *(const float *)&field.vMinFieldValue;
        const float fMax = *(const float *)&field.vMaxFieldValue;
        const float fValue = *(const float *)&dwDecodedVal;
        if (!(fValue <= fMax & fValue >= fMin))
            return XDecoder::RESULT_CORRUPTED;
    }
    *(unsigned int *)lpDefinition = dwDecodedVal;
    return XDecoder::RESULT_VALID;
}

// Original line 2005.
void TypeDescriptorClass_DT_ANGULARVELOCITY::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
        Angle fMinVal = *(const Angle *)&field.vMinFieldValue;
        Angle fMaxVal = *(const Angle *)&field.vMaxFieldValue;
        *(Angle *)lpDefinition = fMinVal + RandFloat_Internal(tRNG) * (fMaxVal - fMinVal);
    } else {
        *(Angle *)lpDefinition = RandAngle(tRNG);
    }
}

// Original line 2022.
void TypeDescriptorClass_DT_ANGULARVELOCITY::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 2041.
int TypeDescriptorClass_DT_ANGULARACCEL::SizeOf() const
{
    return (int)4;
}

// Original line 2062.
CRefString TypeDescriptorClass_DT_ANGULARACCEL::ToString(const void *pData) const
{
    return CRefString().Format("%.2f ft/s^2", ConvertAngleToDegreesPerSecondsSquared(*(const float *)pData));
}

// Original line 2068.
void TypeDescriptorClass_DT_ANGULARACCEL::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(float *)pData = ConvertAngleFromDegreesPerSecondSquared((float)atof(szData));
}

// Original line 2076.
void TypeDescriptorClass_DT_ANGULARACCEL::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, ConvertAngleToDegreesPerSecondsSquared(*(const float *)pData));
}

// Original line 2084.
BOOL TypeDescriptorClass_DT_ANGULARACCEL::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(float *)pData = ConvertAngleFromDegreesPerSecondSquared((float)atof(sSerializeString));
    return TRUE;
}

// Original line 2093.
TDVariant TypeDescriptorClass_DT_ANGULARACCEL::ToVariant(const void *pData) const
{
    const float flValue = ConvertAngleToDegreesPerSecondsSquared(*(const float *)pData);
    return TDVariant(ToString(pData), flValue);
}

// Original line 2101.
int TypeDescriptorClass_DT_ANGULARACCEL::FromVariant(void *pData, const TDVariant &tValue) const
{
    *(float *)pData = ConvertAngleFromDegreesPerSecondSquared(tValue.GetFloat());
    return 1;
}

// Original line 2112.
void TypeDescriptorClass_DT_ANGULARACCEL::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    const void * pDefaultData = tField.GetDefaultData();
    *(uint32 *)pData = (uint32)(uintptr)pDefaultData;
}

// Original line 2120.
int TypeDescriptorClass_DT_ANGULARACCEL::GetTagVariantType() const
{
    return (int)1;
}

// Original line 2126.
void TypeDescriptorClass_DT_ANGULARACCEL::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 2136.
void TypeDescriptorClass_DT_ANGULARACCEL::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 2144.
void TypeDescriptorClass_DT_ANGULARACCEL::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 2157.
void TypeDescriptorClass_DT_ANGULARACCEL::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 2165.
int TypeDescriptorClass_DT_ANGULARACCEL::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
        const float fMin = *(const float *)&field.vMinFieldValue;
        const float fValue = *(const float *)lpDefinition;
        const float fMax = *(const float *)&field.vMaxFieldValue;
        const bool bInRange = fValue >= fMin & fValue <= fMax;
        if (!bInRange) {
            DisplayInternalError(0, "", 2181, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        tEncoder.Put(*(const unsigned int *)&fValue);
    } else {
        tEncoder.Put(*(const unsigned int *)lpDefinition);
    }
    return 1;
}

// Original line 2189.
XDecoder::Result TypeDescriptorClass_DT_ANGULARACCEL::Decode(void *lpDefinition, const TypeDescriptorField &field, XDecoder &tDecoder) const
{
    uint32 dwDecodedVal;
    XDecoder::Result eResult = tDecoder.Get(dwDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
        const float fMin = *(const float *)&field.vMinFieldValue;
        const float fMax = *(const float *)&field.vMaxFieldValue;
        const float fValue = *(const float *)&dwDecodedVal;
        if (!(fValue <= fMax & fValue >= fMin))
            return XDecoder::RESULT_CORRUPTED;
    }
    *(unsigned int *)lpDefinition = dwDecodedVal;
    return XDecoder::RESULT_VALID;
}

// Original line 2218.
void TypeDescriptorClass_DT_ANGULARACCEL::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
        Angle fMinVal = *(const Angle *)&field.vMinFieldValue;
        Angle fMaxVal = *(const Angle *)&field.vMaxFieldValue;
        *(Angle *)lpDefinition = fMinVal + RandFloat_Internal(tRNG) * (fMaxVal - fMinVal);
    } else {
        *(Angle *)lpDefinition = RandAngle(tRNG);
    }
}

// Original line 2235.
void TypeDescriptorClass_DT_ANGULARACCEL::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 2254.
int TypeDescriptorClass_DT_MASS::SizeOf() const
{
    return (int)4;
}

// Original line 2275.
CRefString TypeDescriptorClass_DT_MASS::ToString(const void *pData) const
{
    return CRefString().Format("%.2f lb", ConvertSlugsToPounds(*(const float *)pData));
}

// Original line 2281.
void TypeDescriptorClass_DT_MASS::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(float *)pData = ConvertSlugsFromPounds((float)atof(szData));
}

// Original line 2290.
void TypeDescriptorClass_DT_MASS::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, ConvertSlugsToPounds(*(const float *)pData));
}

// Original line 2298.
BOOL TypeDescriptorClass_DT_MASS::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(float *)pData = ConvertSlugsFromPounds((float)atof(sSerializeString));
    return TRUE;
}

// Original line 2307.
TDVariant TypeDescriptorClass_DT_MASS::ToVariant(const void *pData) const
{
    const float flPounds = ConvertSlugsToPounds(*(const float *)pData);
    return TDVariant(ToString(pData), flPounds);
}

// Original line 2315.
int TypeDescriptorClass_DT_MASS::FromVariant(void *pData, const TDVariant &tValue) const
{
    *(float *)pData = ConvertSlugsFromPounds(tValue.GetFloat());
    return 1;
}

// Original line 2326.
void TypeDescriptorClass_DT_MASS::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    const void * pDefaultData = tField.GetDefaultData();
    *(uint32 *)pData = (uint32)(uintptr)pDefaultData;
}

// Original line 2334.
int TypeDescriptorClass_DT_MASS::GetTagVariantType() const
{
    return (int)1;
}

// Original line 2340.
void TypeDescriptorClass_DT_MASS::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 2350.
void TypeDescriptorClass_DT_MASS::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 2358.
void TypeDescriptorClass_DT_MASS::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 2371.
void TypeDescriptorClass_DT_MASS::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 2379.
int TypeDescriptorClass_DT_MASS::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericFloatEncode(lpDefinition, field, tEncoder);
}

// Original line 2387.
XDecoder::Result TypeDescriptorClass_DT_MASS::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericFloatDecode(lpDefinition, field, tDecoder);
}

// Original line 2395.
void TypeDescriptorClass_DT_MASS::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericFloatFuzz(lpDefinition, field, tRNG);
}

// Original line 2403.
void TypeDescriptorClass_DT_MASS::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    if (field.dwFlags & (1 << TDF_XFLOAT16))
        nBitCount = 16;
    field.nBitCount = (short)nBitCount;
}

// Original line 2432.
int TypeDescriptorClass_DT_IMPULSE::SizeOf() const
{
    return (int)4;
}

// Original line 2455.
CRefString TypeDescriptorClass_DT_IMPULSE::ToString(const void *pData) const
{
    return CRefString().Format("%.2f lb-sec", ConvertSlugFtPerTickToLbsPerSec(*(const float *)pData));
}

// Original line 2461.
void TypeDescriptorClass_DT_IMPULSE::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(float *)pData = ConvertSlugFtPerTickFromLbsPerSec((float)atof(szData));
}

// Original line 2469.
void TypeDescriptorClass_DT_IMPULSE::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, ConvertSlugFtPerTickToLbsPerSec(*(const float *)pData));
}

// Original line 2477.
BOOL TypeDescriptorClass_DT_IMPULSE::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(float *)pData = ConvertSlugFtPerTickFromLbsPerSec((float)atof(sSerializeString));
    return TRUE;
}

// Original line 2486.
TDVariant TypeDescriptorClass_DT_IMPULSE::ToVariant(const void *pData) const
{
    const float flPoundSec = ConvertSlugFtPerTickToLbsPerSec(*(const float *)pData);
    return TDVariant(ToString(pData), flPoundSec);
}

// Original line 2494.
int TypeDescriptorClass_DT_IMPULSE::FromVariant(void *pData, const TDVariant &tValue) const
{
    *(float *)pData = ConvertSlugFtPerTickFromLbsPerSec(tValue.GetFloat());
    return 1;
}

// Original line 2505.
void TypeDescriptorClass_DT_IMPULSE::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    const void * pDefaultData = tField.GetDefaultData();
    *(uint32 *)pData = (uint32)(uintptr)pDefaultData;
}

// Original line 2513.
int TypeDescriptorClass_DT_IMPULSE::GetTagVariantType() const
{
    return (int)1;
}

// Original line 2519.
void TypeDescriptorClass_DT_IMPULSE::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 2529.
void TypeDescriptorClass_DT_IMPULSE::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 2537.
void TypeDescriptorClass_DT_IMPULSE::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 2550.
void TypeDescriptorClass_DT_IMPULSE::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 2558.
int TypeDescriptorClass_DT_IMPULSE::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericFloatEncode(lpDefinition, field, tEncoder);
}

// Original line 2566.
XDecoder::Result TypeDescriptorClass_DT_IMPULSE::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericFloatDecode(lpDefinition, field, tDecoder);
}

// Original line 2574.
void TypeDescriptorClass_DT_IMPULSE::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericFloatFuzz(lpDefinition, field, tRNG);
}

// Original line 2582.
void TypeDescriptorClass_DT_IMPULSE::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    if (field.dwFlags & (1 << TDF_XFLOAT16))
        nBitCount = 16;
    field.nBitCount = (short)nBitCount;
}

// Original line 2609.
int32 TypeDescriptorClass_DT_TAGMAP::SizeOf() const
{
    DisplayInternalError(0, "", 2612, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 2616.
CRefString TypeDescriptorClass_DT_TAGMAP::ToString(const void *pData) const
{
    return CRefString("<tag_map>");
}

// Original line 2622.
TDVariant TypeDescriptorClass_DT_TAGMAP::ToVariant(const void *pData) const
{
    return TDVariant("<tag_map>", *(const uint64 *)pData);
}

// Original line 2641.
void TypeDescriptorClass_DT_TAGMAP::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    if (tField.GetDefaultData()) {
        DisplayInternalError(NULL, "", 2660, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    if (tField.dwFlags & (1 << TDF_XPTR64))
        *(XPtr64<CTagMap> *)pData = TagMapCreate((TagBases)tField.eTagBase);
    else
        *(CTagMap **)pData = TagMapCreate((TagBases)tField.eTagBase);
}

// Original line 2664.
int TypeDescriptorClass_DT_TAGMAP::ContainsSNOSubfield(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 2670.
int TypeDescriptorClass_DT_TAGMAP::ContainsAllocationSubfields(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 2676.
void TypeDescriptorClass_DT_TAGMAP::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    if (!(field.dwFlags & (1 << TDF_XML_SERIALIZATION))) return;
    sax.WriteBeginElement(field.GetName(), NULL);
    const CTagMap * hTagMap;
    if (field.dwFlags & (1 << TDF_XPTR64)) {
        const XPtr64<CTagMap> & rhTagMap64 = *(const XPtr64<CTagMap> *)pData;
        hTagMap = rhTagMap64;
    } else {
        const CTagMap *& hTagMap32 = *(const CTagMap **)pData;
        hTagMap = hTagMap32;
    }
    if (!hTagMap) { sax.WriteEndElement(field.GetName()); return; }
    POS pos = TagMapGetStartPosition(hTagMap);
    while (pos) {
        uint32 dwTag = TagMapGetNextTag(hTagMap, pos);
        const AxeTagInfo * ptTagInfo = TagMapGetTagInfo(hTagMap, dwTag);
        if (!ptTagInfo) continue;
        CRefString sFieldName;
        sFieldName.Format("E");
        sax.WriteBeginElement(sFieldName, NULL);
        if (ptTagInfo->szXMLName[0]) sax.WriteStringElement("sTag", ptTagInfo->szXMLName, NULL);
        else sax.WriteIntElement("dwTag", ptTagInfo->dwTagId, NULL);
        const XTypeDescriptor * ptTypeDesc = TagGetDefinitionDataType(ptTagInfo->eType);
        TypeDescriptorField fieldChild(ptTypeDesc, "vValue");
        TypeDescriptorFieldSerializationInfo childFieldSerializationInfo(fieldChild, fieldSerializationInfo, CRefString().Format(":%s", ptTagInfo->szXMLName), pData);
        switch (TagGetVariantType(ptTagInfo->eType)) {
        case TAGVARIANTTYPE_INT:
        case TAGVARIANTTYPE_ACTOR_ATTRIBUTE: {
            int32 nValue = TagMapGetInt(hTagMap, dwTag);
            ptTypeDesc->SerializeToXML(sax, childFieldSerializationInfo, &nValue); break;
        }
        case TAGVARIANTTYPE_FLOAT: {
            float flValue = TagMapGetFloat(hTagMap, dwTag);
            ptTypeDesc->SerializeToXML(sax, childFieldSerializationInfo, &flValue); break;
        }
        case TAGVARIANTTYPE_SNO: {
            fieldChild.dwFilter = AxeTagTypeToSNOGroup(ptTagInfo->eType);
            SNO sno = TagMapGetSNO(hTagMap, dwTag);
            ptTypeDesc->SerializeToXML(sax, childFieldSerializationInfo, &sno); break;
        }
        case TAGVARIANTTYPE_GBID: {
            fieldChild.dwFilter = AxeTagTypeToGBType(ptTagInfo->eType);
            GBID gbid = TagMapGetGBID(hTagMap, dwTag);
            ptTypeDesc->SerializeToXML(sax, childFieldSerializationInfo, &gbid); break;
        }
        case TAGVARIANTTYPE_FORMULA: {
            StringFormula * ptStringFormula = TagMapGetFormula(hTagMap, dwTag);
            ptTypeDesc->SerializeToXML(sax, childFieldSerializationInfo, ptStringFormula); break;
        }
        case TAGVARIANTTYPE_ACTOR_GROUP:
        case TAGVARIANTTYPE_STARTLOC_NAME: {
            uint32 dwValue = TagMapGetDWORD(hTagMap, dwTag);
            ptTypeDesc->SerializeToXML(sax, childFieldSerializationInfo, &dwValue); break;
        }
        default: DisplayInternalError(NULL, "", 2795, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
        sax.WriteEndElement(sFieldName);
    }
    sax.WriteEndElement(field.GetName());
}

// Original line 2804.
void TypeDescriptorClass_DT_TAGMAP::SerializeFromXML(TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CXMLNode &node, void *pData, BOOL &fContainsBadLink) const
{
    DisplayInternalError(0, "", 2811, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 2912.
void TypeDescriptorClass_DT_TAGMAP::IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField &, SNOIterateFunc pfuncIterate, void * pData) const
{
    CTagMap * hTagMap = *(const XPtr64<CTagMap> *)lpDefinition;
    if (hTagMap) TagMapIterateSnoReferences(hTagMap, pfuncIterate, pData);
}

// Original line 2928.
void TypeDescriptorClass_DT_TAGMAP::ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField &, const SNOName & snonameOld, SNO snoNew) const
{
    CTagMap * hTagMap = *(const XPtr64<CTagMap> *)lpDefinition;
    if (hTagMap) TagMapChangeSNOReference(hTagMap, snonameOld, snoNew);
}

// Original line 2944.
void TypeDescriptorClass_DT_TAGMAP::ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    SerializeData & ser = field.GetSerializeDataRaw(lpDefinition);
    CTagMap * hTagMap = *(XPtr64<CTagMap> *)lpDefinition;
    ser.dwOffset = ser.dwSize = 0;
    if (hTagMap) {
        ser.dwSize = TagMapGetSerialLength(hTagMap, field.dwFlags);
        FileSerialize(ser, dwOffset, 4);
    }
}

// Original line 2966.
void TypeDescriptorClass_DT_TAGMAP::ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList &) const
{
    SerializeData & ser = field.GetSerializeDataRaw(lpDefinition);
    CTagMap * hTagMap = *(XPtr64<CTagMap> *)lpDefinition;
    ser.dwOffset = ser.dwSize = 0;
    if (hTagMap) {
        ser.dwSize = TagMapGetSerialLength(hTagMap, field.dwFlags);
        if (field.dwFlags & (1 << TDF_DISABLE_DATA_MERGE))
            FileSerialize(ser, dwOffset, 4);
    }
}

// Original line 3030.
void TypeDescriptorClass_DT_TAGMAP::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, uint32, uint32) const
{
    CTagMap *& hTagMap32 = *(CTagMap **)lpDefinition;
    XPtr64<CTagMap> & rhTagMap64 = *(XPtr64<CTagMap> *)lpDefinition;
    SerializeData & ser = field.GetSerializeData(lpDefinition);
    CTagMap * hTagMap = (field.dwFlags & (1 << TDF_XPTR64)) ? (CTagMap *)rhTagMap64 : hTagMap32;
    if (hTagMap) TagMapSerialize(hTagMap, lpDst + ser.dwOffset, field.dwFlags);
}

// Original line 3062.
// NON_MATCHING: same size; branch-folding and register allocation retain wider flag loads and one extra saved register.
void TypeDescriptorClass_DT_TAGMAP::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{

    SerializeData & ser = field.GetSerializeData(lpDefinition);
    if (!(field.dwFlags & (1 << TDF_DISABLE_DATA_MERGE)) && tUnserializeData.ptMergedOffsetMap) {
        uintptr dwTagMap;
        if (tUnserializeData.ptMergedOffsetMap->Lookup(ser.dwOffset | (field.eTagBase << 11), dwTagMap)) {
            *(CTagMap **)lpDefinition = TagMapRef((CTagMap *)dwTagMap);
            return;
        }
    }
    void * pTagMapSerialized = sLoadSerializedTracked(tUnserializeData, ser, (field.dwFlags & (1 << TDF_XPTR64)) ? 8 : 4,
        "D:/BuildServer/2/work/code/branches/2_6_2/X/Common/D3TypeDescriptor/D3DescribedType.cpp", 3090, field.szFieldName);
    CTagMap * hTagMap = NULL;
    if (field.dwFlags & (1 << TDF_XPTR64)) {
            *(XPtr64<CTagMap> *)lpDefinition = TagMapCreate((TagBases)field.eTagBase);
            hTagMap = *(XPtr64<CTagMap> *)lpDefinition;
        } else {
            *(CTagMap **)lpDefinition = TagMapCreate((TagBases)field.eTagBase);
            hTagMap = *(CTagMap **)lpDefinition;
        }
    if (!tUnserializeData.ptSNOElement) hTagMap->m_dwFlags |= 1 << TAGMAP_FLAG_READONLY;
    if (ser.dwSize) TagMapUnserialize(hTagMap, pTagMapSerialized);
    if (!TagMapGetTagCount(hTagMap)) {
        TagMapDispose(hTagMap);
        if (field.dwFlags & (1 << TDF_XPTR64)) {
            *(XPtr64<CTagMap> *)lpDefinition = TagMapCreateEmpty((TagBases)field.eTagBase);
            hTagMap = *(XPtr64<CTagMap> *)lpDefinition;
        } else {
            *(CTagMap **)lpDefinition = TagMapCreateEmpty((TagBases)field.eTagBase);
            hTagMap = *(CTagMap **)lpDefinition;
        }
    }
    if (!(field.dwFlags & (1 << TDF_DISABLE_DATA_MERGE)) && tUnserializeData.ptMergedOffsetMap) {
        // The original tests the encoded empty-map pointer tag before dereferencing.
        if (((uintptr)hTagMap & 0xffee000000000000ULL) != 0xffee000000000000ULL && TagMapCanBeRefed(hTagMap)) {
            uint32 dwCRC = Blizzard::Util::CRC32(pTagMapSerialized, ser.dwSize, 0);
            hTagMap->m_dwCRC = dwCRC;
            CTagMap * ptIdentical = TagMapFindIdenticalGlobal(hTagMap);
            if (ptIdentical) {
                TagMapDispose(hTagMap);
                if (field.dwFlags & (1 << TDF_XPTR64)) {
            *(XPtr64<CTagMap> *)lpDefinition = TagMapRef(ptIdentical);
            hTagMap = *(XPtr64<CTagMap> *)lpDefinition;
        } else {
            *(CTagMap **)lpDefinition = TagMapRef(ptIdentical);
            hTagMap = *(CTagMap **)lpDefinition;
        }
            } else TagMapRegisterGlobal(hTagMap);
        }
        tUnserializeData.ptMergedOffsetMap->SetAt_Tracked(ser.dwOffset | (field.eTagBase << 11), (uintptr)hTagMap);
    }
    if (tUnserializeData.ptSNOElement) SigmaMemoryFree(pTagMapSerialized, tUnserializeData.pMemoryInterface);
}

// Original line 3181.
void TypeDescriptorClass_DT_TAGMAP::Delete(uint32, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface *) const
{
    if (field.dwFlags & (1 << TDF_XPTR64)) {
        XPtr64<CTagMap> & rpTagMap64 = *(XPtr64<CTagMap> *)lpDefinition;
        if (rpTagMap64) TagMapDispose(rpTagMap64);
    } else {
        CTagMap ** hTagMap32 = (CTagMap **)lpDefinition;
        if (*hTagMap32) TagMapDispose(*hTagMap32);
    }
}

// Original line 3205.
int TypeDescriptorClass_DT_TAGMAP::Encode(void *, const TypeDescriptorField &, XEncoder &) const
{
    DisplayInternalError(0, "", 3210, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3214.
XDecoder::Result TypeDescriptorClass_DT_TAGMAP::Decode(void *, const TypeDescriptorField &, XDecoder &) const
{
    DisplayInternalError(0, "", 3219, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3223.
void TypeDescriptorClass_DT_TAGMAP::Fuzz(void *lpDefinition, const TypeDescriptorField &field, RNG &tRNG) const
{
    DisplayInternalError(0, "", 3228, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3230.
void TypeDescriptorClass_DT_TAGMAP::CalculateNetworkBitCounts(TypeDescriptorField & field) const
{
    field.nBitCount = 0;
}

// Original line 3587.
int TypeDescriptorClass_DT_ANIM_TAG::SizeOf() const
{
    return (int)4;
}

// Original line 3596.
CRefString TypeDescriptorClass_DT_ANIM_TAG::ToString(const void *pData) const
{
    return CRefString(AnimTagGetName(*(const unsigned int *)pData));
}

// Original line 3603.
void TypeDescriptorClass_DT_ANIM_TAG::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString = AnimTagGetName(*(const unsigned int *)pData);
}

// Original line 3612.
BOOL TypeDescriptorClass_DT_ANIM_TAG::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(unsigned int *)pData = AnimTagGetID(sSerializeString);
    return TRUE;
}

// Original line 3621.
TDVariant TypeDescriptorClass_DT_ANIM_TAG::ToVariant(const void *pData) const
{
    return TDVariant(ToString(pData), *(const unsigned int *)pData);
}

// Original line 3630.
void TypeDescriptorClass_DT_ANIM_TAG::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const
{
    if (tField.vDefaultFieldValue)
        *(int *)pData = *(const int *)tField.vDefaultFieldValue;
    else
        *(int *)pData = -1;
}

// Original line 3648.
int TypeDescriptorClass_DT_ANIM_TAG::ContainsSNOSubfield(const TypeDescriptorField &) const
{
    return (int)0;
}

// Original line 3654.
int TypeDescriptorClass_DT_ANIM_TAG::ContainsAllocationSubfields(const TypeDescriptorField &) const
{
    return (int)0;
}

// Original line 3660.
void TypeDescriptorClass_DT_ANIM_TAG::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    if (field.dwFlags & (1 << TDF_XML_SERIALIZATION)) {
        sax.WriteBeginElement(field.GetName(), NULL);
        CRefString sSerializationString;
        ToXMLSerializedString(fieldSerializationInfo, pData, sSerializationString);
        sax.WriteStringElement("szAnimTag", sSerializationString, fieldSerializationInfo.GetXMLAttributes());
        sax.WriteEndElement(field.GetName());
    }
}

// Original line 3685.
void TypeDescriptorClass_DT_ANIM_TAG::SerializeFromXML(TypeDescriptorFieldSerializationInfo &fieldSerializationInfo, const CXMLNode &node, void *pData, BOOL &) const
{
    DisplayInternalError(0, "", 3692, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3699.
void TypeDescriptorClass_DT_ANIM_TAG::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 3712.
void TypeDescriptorClass_DT_ANIM_TAG::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{
}

// Original line 3722.
int TypeDescriptorClass_DT_ANIM_TAG::Encode(void *, const TypeDescriptorField &, XEncoder &) const
{
    DisplayInternalError(0, "", 3727, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3731.
XDecoder::Result TypeDescriptorClass_DT_ANIM_TAG::Decode(void *, const TypeDescriptorField &, XDecoder &) const
{
    DisplayInternalError(0, "", 3736, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3740.
void TypeDescriptorClass_DT_ANIM_TAG::Fuzz(void *lpDefinition, const TypeDescriptorField &field, RNG &tRNG) const
{
    DisplayInternalError(0, "", 3745, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3748.
void TypeDescriptorClass_DT_ANIM_TAG::CalculateNetworkBitCounts(TypeDescriptorField & field) const
{
    field.nBitCount = 0;
}

// Original line 3767.
int32 TypeDescriptorClass_DT_STRING_FORMULA::SizeOf() const
{
    DisplayInternalError(0, "", 3770, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3774.
int TypeDescriptorClass_DT_STRING_FORMULA::VariantsEqual(const TDVariant &tValue1, const TDVariant &tValue2) const
{
    return strcmp(tValue1.GetStr(), tValue2.GetStr()) == 0;
}

// Original line 3787.
CRefString TypeDescriptorClass_DT_STRING_FORMULA::ToString(const void * pData) const
{
    const StringFormula * ptStringFormula = (const StringFormula *)pData;
    CRefString sString("");
    if (ptStringFormula->szFormulaString) sString = ptStringFormula->szFormulaString;
    return sString;
}

// Original line 3802.
void TypeDescriptorClass_DT_STRING_FORMULA::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString = XMLStringConvertToXMLString(ToString(pData));
}

// Original line 3810.
BOOL TypeDescriptorClass_DT_STRING_FORMULA::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    StringFormula * ptStringFormula = (StringFormula *)pData;
    uint32 dwStringSize = sSerializeString.Length() + 1;
    ptStringFormula->szFormulaString = (char *)SigmaMemoryNew(dwStringSize, 4, NULL, TRUE);
    ptStringFormula->pbFormula = (uint8 *)SigmaMemoryNew(1024, 4, NULL, TRUE);
    strcpy(ptStringFormula->szFormulaString, sSerializeString.str());
    BBETranslationData bbeTranslationData = { -1, -1 };
    uint32 dwFormulaSize = BBEFromString(sSerializeString.str(), ptStringFormula->pbFormula, 1024, bbeTranslationData);
    ptStringFormula->serFormulaString.dwSize = dwStringSize;
    ptStringFormula->serFormula.dwSize = dwFormulaSize;
    return TRUE;
}

// Original line 3838.
TDVariant TypeDescriptorClass_DT_STRING_FORMULA::ToVariant(const void *pData) const
{
    return TDVariant((LPCSTR)ToString(pData));
}

// Original line 3844.
BOOL TypeDescriptorClass_DT_STRING_FORMULA::FromVariant(void * pData, const TDVariant & tValue) const
{
    StringFormula * ptStringFormula = (StringFormula *)pData;
    if (tValue.GetSource() == TDVARIANT_SOURCE_INVALID) return TRUE;
    if (tValue.GetSource() != TDVARIANT_SOURCE_STRING) {
        DisplayInternalError(NULL, "", 3863, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    ptStringFormula->Set(tValue.GetStr());
    ptStringFormula->Compile();
    return TRUE;
}

// Original line 3872.
void TypeDescriptorClass_DT_STRING_FORMULA::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    StringFormula * ptStringFormula = (StringFormula *)pData;
    const void * pDefaultData = tField.GetDefaultData();
    if (pDefaultData) ptStringFormula->Copy(*(const StringFormula *)pDefaultData);
    else ptStringFormula->Set("1");
}

// Original line 3889.
int TypeDescriptorClass_DT_STRING_FORMULA::ContainsAllocationSubfields(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 3895.
void TypeDescriptorClass_DT_STRING_FORMULA::Byteswap(void *lpDefinitionDst, void *lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    DisplayInternalError(0, "", 3904, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 3908.
void TypeDescriptorClass_DT_STRING_FORMULA::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
    StringFormula * ptStringFormula = (StringFormula *)lpDefinition;
    BBEByteswap(ptStringFormula->pbFormula, ptStringFormula->serFormula.dwSize);
}

// Original line 3919.
TagVariantType TypeDescriptorClass_DT_STRING_FORMULA::GetTagVariantType() const
{
    return (TagVariantType)4;
}

// Original line 3925.
void TypeDescriptorClass_DT_STRING_FORMULA::ComputeOffsets(void * lpDefinition, const TypeDescriptorField &, uint32 & dwOffset) const
{
    StringFormula * ptStringFormula = (StringFormula *)lpDefinition;
    SerializeData & ser = ptStringFormula->serFormula;
    SerializeData & serString = ptStringFormula->serFormulaString;
    FileSerialize(serString, dwOffset, 4);
    FileSerialize(ser, dwOffset, 4);
}

// Original line 3939.
void TypeDescriptorClass_DT_STRING_FORMULA::SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, uint32, uint32) const
{
    StringFormula * ptStringFormula = (StringFormula *)lpDefinition;
    LPCSTR pFormulaStr = ptStringFormula->szFormulaString;
    uint8 * pFormula = ptStringFormula->pbFormula;
    SerializeData & ser = ptStringFormula->serFormula;
    SerializeData & serString = ptStringFormula->serFormulaString;
    SigmaMemoryCopy(lpDst + serString.dwOffset, pFormulaStr, serString.dwSize);
    SigmaMemoryCopy(lpDst + ser.dwOffset, pFormula, ser.dwSize);
    BBEByteswap(lpDst + ser.dwOffset, ser.dwSize);
}

// Original line 3964.
void TypeDescriptorClass_DT_STRING_FORMULA::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{
    StringFormula * ptStringFormula = (StringFormula *)lpDefinition;
    SerializeData & ser = ptStringFormula->serFormula;
    SerializeData & serString = ptStringFormula->serFormulaString;
    DT_SERIALIZEDATA->ByteswapInPlace(&ser, &ser, TypeDescriptorField(DT_SERIALIZEDATA, NULL));
    DT_SERIALIZEDATA->ByteswapInPlace(&serString, &serString, TypeDescriptorField(DT_SERIALIZEDATA, NULL));
    if (ser.dwSize) {
        ptStringFormula->pbFormula = (uint8 *)sLoadSerializedTracked(tUnserializeData, ser, 4, "D:/BuildServer/2/work/code/branches/2_6_2/X/Common/D3TypeDescriptor/D3DescribedType.cpp", 3983, field.szFieldName);
        BBEByteswap(ptStringFormula->pbFormula, ser.dwSize);
    } else ptStringFormula->pbFormula = NULL;
    if (serString.dwSize)
        ptStringFormula->szFormulaString = (char *)sLoadSerializedTracked(tUnserializeData, serString, 4, "D:/BuildServer/2/work/code/branches/2_6_2/X/Common/D3TypeDescriptor/D3DescribedType.cpp", 3994, field.szFieldName);
    else ptStringFormula->szFormulaString = NULL;
}

// Original line 3998.
void TypeDescriptorClass_DT_STRING_FORMULA::Delete(uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField &, XMemoryAllocatorInterface * ptMemoryInterface) const
{
    if (!(dwDefinitionFlags & 4)) {
        StringFormula * ptStringFormula = (StringFormula *)lpDefinition;
        SigmaMemoryFree(ptStringFormula->szFormulaString, ptMemoryInterface);
        ptStringFormula->szFormulaString = NULL;
        SigmaMemoryFree(ptStringFormula->pbFormula, ptMemoryInterface);
        ptStringFormula->pbFormula = NULL;
    }
}

// Original line 4015.
int TypeDescriptorClass_DT_STRING_FORMULA::Encode(void *, const TypeDescriptorField &, XEncoder &) const
{
    DisplayInternalError(0, "", 4020, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 4024.
XDecoder::Result TypeDescriptorClass_DT_STRING_FORMULA::Decode(void *, const TypeDescriptorField &, XDecoder &) const
{
    DisplayInternalError(0, "", 4029, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 4033.
void TypeDescriptorClass_DT_STRING_FORMULA::Fuzz(void *lpDefinition, const TypeDescriptorField &field, RNG &tRNG) const
{
    DisplayInternalError(0, "", 4038, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 4041.
void TypeDescriptorClass_DT_STRING_FORMULA::CalculateNetworkBitCounts(TypeDescriptorField & field) const
{
    field.nBitCount = 0;
}

// Original line 4154.
int TypeDescriptorClass_DT_ACTOR_GROUP::SizeOf() const
{
    return (int)4;
}

// Original line 4163.
CRefString TypeDescriptorClass_DT_ACTOR_GROUP::ToString(const void *pData) const
{
    return CRefString(ActorGroupHashToName(*(const unsigned int *)pData));
}

// Original line 4169.
void TypeDescriptorClass_DT_ACTOR_GROUP::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(unsigned int *)pData = ActorGroupNameToHash(szData);
}

// Original line 4177.
void TypeDescriptorClass_DT_ACTOR_GROUP::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    if (*(const uint32 *)pData == (uint32)-1) sSerializeString = "";
    else sSerializeString = XMLStringConvertToXMLString(ActorGroupHashToName(*(const uint32 *)pData));
}

// Original line 4192.
BOOL TypeDescriptorClass_DT_ACTOR_GROUP::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    uint32 uHash = ActorGroupNameToHash(sSerializeString);
    *(uint32 *)pData = uHash;
    BOOL fContainsBadLink = uHash != (uint32)-1 && ActorGroupHashToIndex(uHash) == -1;
    return !fContainsBadLink;
}

// Original line 4204.
TDVariant TypeDescriptorClass_DT_ACTOR_GROUP::ToVariant(const void *pData) const
{
    const unsigned int uHash = *(const unsigned int *)pData;
    return TDVariant(CRefString(ActorGroupHashToName(uHash)), uHash);
}

// Original line 4212.
int TypeDescriptorClass_DT_ACTOR_GROUP::FromVariant(void *pData, const TDVariant &tValue) const
{
    if (tValue.GetSource() == TDVARIANT_SOURCE_INVALID)
        return 1;
    if (tValue.GetSource() == TDVARIANT_SOURCE_STRING)
        *(unsigned int *)pData = ActorGroupNameToHash(tValue.GetStr());
    else
        *(unsigned int *)pData = tValue.GetUInt();
    return 1;
}

// Original line 4233.
void TypeDescriptorClass_DT_ACTOR_GROUP::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const
{
    if (tField.vDefaultFieldValue)
        *(unsigned int *)pData = ActorGroupNameToHash((LPCSTR)tField.vDefaultFieldValue);
    else
        *(unsigned int *)pData = -1;
}

// Original line 4248.
int TypeDescriptorClass_DT_ACTOR_GROUP::ContainsSNOSubfield(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 4254.
TagVariantType TypeDescriptorClass_DT_ACTOR_GROUP::GetTagVariantType() const
{
    return (TagVariantType)5;
}

// Original line 4260.
void TypeDescriptorClass_DT_ACTOR_GROUP::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 4270.
void TypeDescriptorClass_DT_ACTOR_GROUP::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 4278.
void TypeDescriptorClass_DT_ACTOR_GROUP::IterateSNOReferences(const void *, const TypeDescriptorField &, SNOIterateFunc pfuncIterate, void * pData) const
{
    SNOName snoName(GLOBAL_SNO, GlobalSNOGet(GLOBALSNO_GLOBALS));
    pfuncIterate(snoName, 0, pData);
}

// Original line 4288.
void TypeDescriptorClass_DT_ACTOR_GROUP::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 4301.
void TypeDescriptorClass_DT_ACTOR_GROUP::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 4309.
int TypeDescriptorClass_DT_ACTOR_GROUP::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition, field.nBitCount);
    return 1;
}

// Original line 4323.
XDecoder::Result TypeDescriptorClass_DT_ACTOR_GROUP::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 4331.
void TypeDescriptorClass_DT_ACTOR_GROUP::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 4339.
void TypeDescriptorClass_DT_ACTOR_GROUP::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 4358.
int TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::SizeOf() const
{
    return (int)4;
}

// Original line 4367.
CRefString TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::ToString(const void * pData) const
{
    CRefString sString("- NONE -");
    Attrib eAttrib = *(const Attrib *)pData;
    if (eAttrib > 0 && eAttrib < NUM_ATTRIBUTES) sString = AttribToString(eAttrib);
    return sString;
}

// Original line 4379.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(int *)pData = StringToAttrib(szData);
}

// Original line 4387.

void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    if (*(const int32 *)pData != -1 && *(const int32 *)pData != 0)  // original: test at line 4392, the "" store at 4399 is the else
        sSerializeString = XMLStringConvertToXMLString(ToString(pData));
    else sSerializeString = "";
}

// Original line 4403.
BOOL TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    int32 eAttrib = sSerializeString.Length() == 0 ? 0 : StringToAttrib(sSerializeString);
    *(int32 *)pData = eAttrib;
    BOOL fContainsBadLink = (sSerializeString.Length() != 0) & (eAttrib == 0 || eAttrib == -1);
    return !fContainsBadLink;
}

// Original line 4420.
TDVariant TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::ToVariant(const void *pData) const
{
    const int eAttrib = *(const int *)pData;
    return TDVariant(ToString(pData), eAttrib);
}

// Original line 4428.
int TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::FromVariant(void *pData, const TDVariant &tValue) const
{
    if (tValue.GetSource() == TDVARIANT_SOURCE_INVALID)
        return 1;
    if (tValue.GetSource() == TDVARIANT_SOURCE_STRING)
        *(int *)pData = StringToAttrib(tValue.GetStr());
    else
        *(int *)pData = tValue.GetUInt();
    return 1;
}

// Original line 4449.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const
{
    if (tField.vDefaultFieldValue)
        *(int *)pData = StringToAttrib((LPCSTR)tField.vDefaultFieldValue);
    else
        *(int *)pData = -1;
}

// Original line 4464.
int TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::ContainsSNOSubfield(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 4470.
TagVariantType TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::GetTagVariantType() const
{
    return (TagVariantType)6;
}

// Original line 4476.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 4486.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 4494.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::IterateSNOReferences(const void *, const TypeDescriptorField &, SNOIterateFunc, void *) const {}

// Original line 4506.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 4519.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 4527.
int TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition, field.nBitCount);
    return 1;
}

// Original line 4541.
XDecoder::Result TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 4549.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 4557.
void TypeDescriptorClass_DT_ACTOR_ATTRIBUTE::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 4576.
int TypeDescriptorClass_DT_STARTLOC_NAME::SizeOf() const
{
    return (int)4;
}

// Original line 4585.
CRefString TypeDescriptorClass_DT_STARTLOC_NAME::ToString(const void *pData) const
{
    return CRefString(StartingLocationGUIDToName(*(const unsigned int *)pData));
}

// Original line 4591.
void TypeDescriptorClass_DT_STARTLOC_NAME::FromString(const TypeDescriptorField &, void *pData, const CRefString &szData) const
{
    *(unsigned int *)pData = StartingLocationNameToGUID(szData);
}

// Original line 4599.
void TypeDescriptorClass_DT_STARTLOC_NAME::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void *pData, CRefString &sSerializeString) const
{
    sSerializeString.Format(SAX::INT32_STR_FORMAT, *(const int *)pData);
}

// Original line 4607.
BOOL TypeDescriptorClass_DT_STARTLOC_NAME::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString &sSerializeString, void *pData) const
{
    *(int *)pData = atoi(sSerializeString.str());
    return TRUE;
}

// Original line 4616.
TDVariant TypeDescriptorClass_DT_STARTLOC_NAME::ToVariant(const void *pData) const
{
    const unsigned int uGUID = *(const unsigned int *)pData;
    return TDVariant(CRefString(StartingLocationGUIDToName(uGUID)), uGUID);
}

// Original line 4624.
int TypeDescriptorClass_DT_STARTLOC_NAME::FromVariant(void *pData, const TDVariant &tValue) const
{
    // Inferred: a switch (row 4628:9) whose two GetUInt calls and three stores were merged (the merged
    // call and store carry no line of their own); only an INVALID first case reproduces the original.
    switch (tValue.GetSource())
    {
    case TDVARIANT_SOURCE_INVALID:
        *(uint32 *)pData = tValue.GetUInt();
        break;
    case TDVARIANT_SOURCE_STRING:
        *(uint32 *)pData = StartingLocationNameToGUID(tValue.GetStr());
        break;
    default:
        *(uint32 *)pData = tValue.GetUInt();
        break;
    }
    return 1;
}

// Original line 4646.
void TypeDescriptorClass_DT_STARTLOC_NAME::LoadFromDefaultData(const TypeDescriptorField &tField, void *pData) const
{
    if (tField.vDefaultFieldValue)
        *(unsigned int *)pData = StartingLocationNameToGUID((LPCSTR)tField.vDefaultFieldValue);
    else
        *(unsigned int *)pData = -1;
}

// Original line 4661.
int TypeDescriptorClass_DT_STARTLOC_NAME::ContainsSNOSubfield(const TypeDescriptorField &) const
{
    return (int)1;
}

// Original line 4667.
TagVariantType TypeDescriptorClass_DT_STARTLOC_NAME::GetTagVariantType() const
{
    return (TagVariantType)7;
}

// Original line 4673.
void TypeDescriptorClass_DT_STARTLOC_NAME::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original line 4683.
void TypeDescriptorClass_DT_STARTLOC_NAME::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 4691.
void TypeDescriptorClass_DT_STARTLOC_NAME::IterateSNOReferences(const void *, const TypeDescriptorField &, SNOIterateFunc pfuncIterate, void * pData) const
{
    SNOName snoName(GLOBAL_SNO, GlobalSNOGet(GLOBALSNO_GLOBALS));
    pfuncIterate(snoName, 0, pData);
}

// Original line 4701.
void TypeDescriptorClass_DT_STARTLOC_NAME::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, unsigned char * lpDst, const unsigned int dwDstOffset, const unsigned int dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original line 4714.
void TypeDescriptorClass_DT_STARTLOC_NAME::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 4722.
int TypeDescriptorClass_DT_STARTLOC_NAME::Encode(void *lpDefinition, const TypeDescriptorField &field, XEncoder &tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned int *)lpDefinition, field.nBitCount);
    return 1;
}

// Original line 4736.
XDecoder::Result TypeDescriptorClass_DT_STARTLOC_NAME::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original line 4744.
void TypeDescriptorClass_DT_STARTLOC_NAME::Fuzz(void *lpDefinition, const TypeDescriptorField &, RNG &tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG);
}

// Original line 4752.
void TypeDescriptorClass_DT_STARTLOC_NAME::CalculateNetworkBitCounts(TypeDescriptorField &field) const
{
    int nBitCount = SizeOf() * 8;
    field.nBitCount = (short)nBitCount;
}

// Original line 4764.
void * sLoadSerializedTracked(const UnserializeData & tUnserializeData, const SerializeData & ser, uint32 dwAlign, LPCSTR szFile, int32 nLine, LPCSTR szDebugString)
{
    if (!tUnserializeData.ptSNOElement)
        return (uint8 *)tUnserializeData.lpDefinitionBase + ser.dwOffset;
    if (!tUnserializeData.lpDefinitionBase)
        return SNOFileLoadSerializedTracked(tUnserializeData.ptSNOElement, ser, dwAlign, szFile, nLine, szDebugString, tUnserializeData.pMemoryInterface);
    if (ser.dwSize == 0 || ser.dwSize == (uint32)-1)
        return NULL;
    void * pBuffer = SigmaMemoryNew(ser.dwSize, dwAlign, tUnserializeData.pMemoryInterface, FALSE);
    void * ptSource = (uint8 *)tUnserializeData.lpDefinitionBase + ser.dwOffset;
    SigmaMemoryCopy(pBuffer, ptSource, ser.dwSize);
    return pBuffer;
}

