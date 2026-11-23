// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Attributes/AttributeCompressor.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "FastAttribConstantsSwig.h"
#include "../CommonTypes.h"
#include "../../Sigma/DataStructures/XOptional.h"
#include "../../Sigma/Networking/XDecoder.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "FastAttributeStructs.h"

class XEncoder;

enum AttributeCompressorType {
    ACT_UNCOMPRESSED = 0,
    ACT_BOUNDED_INT = 1,
    ACT_LINEAR_FLOAT = 2,
    ACT_FLOAT16 = 3,
    ACT_FLOAT_VARIABLE = 4,
    ACT_COUNT = 5,
};

struct NetAttributeKeyValue {  // size 0x10
    XOptional<int> nKeyParam;  // 0x00
    Attrib eKeyAttrib;  // 0x08
    int32 nValue;  // 0x0c
};
static_assert(sizeof(NetAttributeKeyValue) == 0x10, "NetAttributeKeyValue");

struct AttributeSetValueMessage {  // size 0x1c
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    NetAttributeKeyValue tKeyVal;  // 0x0c
};
static_assert(sizeof(AttributeSetValueMessage) == 0x1c, "AttributeSetValueMessage");

struct AttributesSetValuesMessage {  // size 0x100
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    int32 nAttributeCount;  // 0x0c
    NetAttributeKeyValue atKeyVals[15];  // 0x10
};
static_assert(sizeof(AttributesSetValuesMessage) == 0x100, "AttributesSetValuesMessage");

class AttributeCompressor {  // size 0x18
    // vtable pointer
public:
    AttributeCompressor(AttributeCompressorType eType) : m_eType(eType), m_ptNextCompressor(m_ptCreatedCompressors) { m_ptCreatedCompressors = this; }
    virtual ~AttributeCompressor() {}  // vtable slot 0
    AttributeCompressorType Type() const;
    static BOOL Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue);
    static XDecoder::Result Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue);
    static void Dispose();
protected:
    static const AttributeCompressor * _FindNext(const AttributeCompressorType eType, const AttributeCompressor * ptStartCompressor);
    virtual BOOL _Encode(XEncoder &, NetAttributeKeyValue &) const = 0;  // vtable slot 2
    virtual XDecoder::Result _Decode(XDecoder &, NetAttributeKeyValue &) const = 0;  // vtable slot 3
private:
    AttributeCompressor();
    AttributeCompressorType m_eType;  // 0x08
    static AttributeCompressor * m_ptCreatedCompressors;
    AttributeCompressor * m_ptNextCompressor;  // 0x10
};
static_assert(sizeof(AttributeCompressor) == 0x18, "AttributeCompressor");

class AttributeCompressorUncompressed : public AttributeCompressor {  // size 0x20
public:
    AttributeCompressorUncompressed() : AttributeCompressor(ACT_UNCOMPRESSED), m_dwBits(32) {}
    static const AttributeCompressor * Register();
private:
    virtual BOOL _Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 2
    virtual XDecoder::Result _Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 3
    uint32 m_dwBits;  // 0x18
};
static_assert(sizeof(AttributeCompressorUncompressed) == 0x20, "AttributeCompressorUncompressed");

class AttributeCompressorBoundedInt : public AttributeCompressor {  // size 0x28
public:
    AttributeCompressorBoundedInt(int32 nMin, int32 nMax) : AttributeCompressor(ACT_BOUNDED_INT), m_nMin(nMin), m_nMax(nMax), m_dwBits(Math::LogBase2(nMax - nMin) + 1) {}
    static const AttributeCompressor * Register(int32 nMin, int32 nMax);
private:
    AttributeCompressorBoundedInt();
    virtual BOOL _Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 2
    virtual XDecoder::Result _Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 3
    int32 m_nMin;  // 0x18
    int32 m_nMax;  // 0x1c
    uint32 m_dwBits;  // 0x20
};
static_assert(sizeof(AttributeCompressorBoundedInt) == 0x28, "AttributeCompressorBoundedInt");

class AttributeCompressorLinearFloat : public AttributeCompressor {  // size 0x30
public:
    AttributeCompressorLinearFloat(float, float, uint32);
    static const AttributeCompressor * Register(float, float, uint32);
private:
    AttributeCompressorLinearFloat();
    virtual BOOL _Encode(XEncoder &, NetAttributeKeyValue &) const;  // vtable slot 2
    virtual XDecoder::Result _Decode(XDecoder &, NetAttributeKeyValue &) const;  // vtable slot 3
    float m_fMin;  // 0x18
    float m_fMax;  // 0x1c
    uint32 m_dwBits;  // 0x20
    float m_fMultiplier;  // 0x24
    float m_fOOMultiplier;  // 0x28
};
static_assert(sizeof(AttributeCompressorLinearFloat) == 0x30, "AttributeCompressorLinearFloat");

class AttributeCompressorFloat16 : public AttributeCompressor {  // size 0x20
public:
    AttributeCompressorFloat16() : AttributeCompressor(ACT_FLOAT16), m_dwBits(16) {}
    static const AttributeCompressor * Register();
private:
    virtual BOOL _Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 2
    virtual XDecoder::Result _Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 3
    uint32 m_dwBits;  // 0x18
};
static_assert(sizeof(AttributeCompressorFloat16) == 0x20, "AttributeCompressorFloat16");

class AttributeCompressorFloatVariable : public AttributeCompressor {  // size 0x18
public:
    AttributeCompressorFloatVariable() : AttributeCompressor(ACT_FLOAT_VARIABLE) {}
    static const AttributeCompressor * Register();
private:
    virtual BOOL _Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 2
    virtual XDecoder::Result _Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const;  // vtable slot 3
};
static_assert(sizeof(AttributeCompressorFloatVariable) == 0x18, "AttributeCompressorFloatVariable");



void AttributeCompressorFillNetAttributeKeyValue(NetAttributeKeyValue & tKeyVal, FastAttribKey tKey, FastAttribValue tValue);
FastAttribKey AttributeCompresserAttribKeyFromNetAttributeKeyValue(const NetAttributeKeyValue & tKeyVal);

class XTypeDescriptor;
extern const XTypeDescriptor * DT_NET_ATTRIBUTE_KEY_VALUE;
extern const XTypeDescriptor * DT_ATTRIBUTE_SET_VALUE_MESSAGE;
extern const XTypeDescriptor * DT_ATTRIBUTES_SET_VALUES_MESSAGE;
