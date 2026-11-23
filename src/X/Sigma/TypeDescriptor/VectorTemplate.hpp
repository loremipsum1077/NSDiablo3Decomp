// Reconstructed from DWARF declarations and original-code comparisons.
// Original file: X/Sigma/TypeDescriptor/VectorTemplate.hpp
//
// The DWARF puts TypeDescriptorClass_DT_VECTOR_RANK2, _RANK3 and _RANK4 and every one of their
// members on the same lines of this file (class at 79, constructor at 90, ...), so the file is a
// template body included once per rank. The includer defines VECTOR_RANK (2, 3 or 4) first;
// VECTOR_RANK_CLASS and the per-component helpers below are invented names.
//
// No include guard: DescribedType.cpp includes it once for each rank.

#include "X/Sigma/TypeDescriptor/TypeDescriptor.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "X/Sigma/Networking/XEncoder.h"
#include "X/Sigma/Random.h"
#include "X/Sigma/XML/SAXHelper.h"
#include <stdio.h>

#ifndef VECTOR_RANK_CAT
#define VECTOR_RANK_CAT2(a, b) a##b
#define VECTOR_RANK_CAT(a, b) VECTOR_RANK_CAT2(a, b)
#endif

#undef VECTOR_RANK_CLASS
#undef VECTOR_RANK_Z
#undef VECTOR_RANK_W
#undef VECTOR_RANK_PZ
#undef VECTOR_RANK_PW

#define VECTOR_RANK_CLASS VECTOR_RANK_CAT(TypeDescriptorClass_DT_VECTOR_RANK, VECTOR_RANK)

// Missing components print as int 0 and scan into NULL.
#if VECTOR_RANK >= 3
#define VECTOR_RANK_Z(v) (v)->z
#define VECTOR_RANK_PZ(v) &(v)->z
#else
#define VECTOR_RANK_Z(v) 0
#define VECTOR_RANK_PZ(v) NULL
#endif
#if VECTOR_RANK >= 4
#define VECTOR_RANK_W(v) (v)->w
#define VECTOR_RANK_PW(v) &(v)->w
#else
#define VECTOR_RANK_W(v) 0
#define VECTOR_RANK_PW(v) NULL
#endif

// Original line 79.
template <typename T>
class VECTOR_RANK_CLASS : public XTypeDescriptor {  // size 0x38
    static const XTypeDescriptor * s_ptDataType;
    static LPCSTR s_szFormatString;
public:
    VECTOR_RANK_CLASS(LPCSTR szDT_NAME, const XTypeDescriptor * ptDataType, LPCSTR szFormatString);
    virtual int32 SizeOf() const;  // vtable slot 3
    static int32 VersionVector(const int32 nCurrentVersion, const CXMLNode & nodeXML, void * pData);
    virtual CRefString ToString(const void * pData) const;  // vtable slot 4
    virtual void FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const;  // vtable slot 5
    virtual void ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const;  // vtable slot 6
    virtual BOOL FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const;  // vtable slot 7
    virtual void LoadFromDefaultData(const TypeDescriptorField &, void * pData) const;  // vtable slot 12
    virtual void SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const;  // vtable slot 17
    virtual void SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const;  // vtable slot 18
    virtual void Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const;  // vtable slot 24
    virtual void ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const;  // vtable slot 25
    virtual void SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const;  // vtable slot 30
    virtual void SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const;  // vtable slot 31
    virtual BOOL Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const;  // vtable slot 33
    virtual XDecoder::Result Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const;  // vtable slot 34
    virtual void Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const;  // vtable slot 35
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const;  // vtable slot 36
};

template <typename T>
const XTypeDescriptor * VECTOR_RANK_CLASS<T>::s_ptDataType;

template <typename T>
LPCSTR VECTOR_RANK_CLASS<T>::s_szFormatString;

// Original line 90.
template <typename T>
VECTOR_RANK_CLASS<T>::VECTOR_RANK_CLASS(LPCSTR szDT_NAME, const XTypeDescriptor * ptDataType, LPCSTR szFormatString)
    : XTypeDescriptor(szDT_NAME)
{
    s_ptDataType = ptDataType;
    s_szFormatString = szFormatString;
    pfuncDOMVersion = VersionVector;
    nVersion = 1;

    static TypeDescriptorField sarVectorFields[VECTOR_RANK] = {
        TypeDescriptorField("x", s_ptDataType, 0, NULL, 1),
        TypeDescriptorField("y", s_ptDataType, 4, NULL, 1),
#if VECTOR_RANK >= 3
        TypeDescriptorField("z", s_ptDataType, 8, NULL, 1),
#endif
#if VECTOR_RANK >= 4
        TypeDescriptorField("w", s_ptDataType, 12, NULL, 1),
#endif
    };

    sarVectorFields[0].CalculateNetworkBitCounts();
    sarVectorFields[1].CalculateNetworkBitCounts();
#if VECTOR_RANK >= 3
    sarVectorFields[2].CalculateNetworkBitCounts();
#endif
#if VECTOR_RANK >= 4
    sarVectorFields[3].CalculateNetworkBitCounts();
#endif

    arFields = sarVectorFields;
    nFieldCount = VECTOR_RANK;
}

// Original line 116.
template <typename T>
int32 VECTOR_RANK_CLASS<T>::SizeOf() const
{
    return sizeof(T);
}

// Original line 122.
template <typename T>
int32 VECTOR_RANK_CLASS<T>::VersionVector(const int32 nCurrentVersion, const CXMLNode & nodeXML, void * pData)
{
    DisplayInternalError(NULL, "", 148, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 152.
template <typename T>
CRefString VECTOR_RANK_CLASS<T>::ToString(const void * pData) const
{
    const T * vec = (const T *)pData;
    return CRefString().Format(s_szFormatString,
        vec->x,
        vec->y,
        VECTOR_RANK_Z(vec),
        VECTOR_RANK_W(vec));
}

// Original line 163.
template <typename T>
void VECTOR_RANK_CLASS<T>::FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
{
    T * vec = (T *)pData;
    sscanf(szData, s_szFormatString,
        &vec->x,
        &vec->y,
        VECTOR_RANK_PZ(vec),
        VECTOR_RANK_PW(vec));
}

// Original line 176.
template <typename T>
void VECTOR_RANK_CLASS<T>::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    const T * vec = (const T *)pData;
    sSerializeString.Format(s_szFormatString,
        vec->x,
        vec->y,
        VECTOR_RANK_Z(vec),
        VECTOR_RANK_W(vec));
}

// Original line 189.
template <typename T>
BOOL VECTOR_RANK_CLASS<T>::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    T * vec = (T *)pData;
    return sscanf(sSerializeString, s_szFormatString,
        &vec->x,
        &vec->y,
        VECTOR_RANK_PZ(vec),
        VECTOR_RANK_PW(vec)) == VECTOR_RANK;
}

// Original line 202.
template <typename T>
void VECTOR_RANK_CLASS<T>::LoadFromDefaultData(const TypeDescriptorField &, void * pData) const
{
    T * vec = (T *)pData;
    vec->x = 0;
    vec->y = 0;
#if VECTOR_RANK >= 3
    vec->z = 0;
#endif
#if VECTOR_RANK >= 4
    vec->w = 0;
#endif
}

// Original line 213.
template <typename T>
void VECTOR_RANK_CLASS<T>::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    if (!(field.dwFlags & (1 << TDF_XML_SERIALIZATION)))
        return;

    CRefString sSerializationString;
    ToXMLSerializedString(fieldSerializationInfo, pData, sSerializationString);

    LPCSTR szAttributes = fieldSerializationInfo.GetXMLAttributes();
    const BOOL bVersioned = nVersion != 0;

    CRefString sAttributes;
    if (bVersioned)
    {
        if (szAttributes)
            sAttributes.Format("%s %s=\"%d\"", szAttributes, "VectorVersion", nVersion);
        else
            sAttributes.Format("%s=\"%d\"", "VectorVersion", nVersion);
    }
    else
    {
        sAttributes = szAttributes ? szAttributes : "";
    }

    sax.WriteStringElement(field.GetName(), sSerializationString.str(), sAttributes.Length() ? sAttributes.str() : NULL);
}

// Original line 253.
template <typename T>
void VECTOR_RANK_CLASS<T>::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 260, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 293.
template <typename T>
void VECTOR_RANK_CLASS<T>::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    for (uint32 i = 0; i < VECTOR_RANK; ++i)
        ((uint32 *)lpDefinitionDst)[i] = ((const uint32 *)lpDefinitionSrc)[i];
}

// Original line 306.
template <typename T>
void VECTOR_RANK_CLASS<T>::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 318.
template <typename T>
void VECTOR_RANK_CLASS<T>::SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    for (uint32 i = 0; i < VECTOR_RANK; ++i)
        ((uint32 *)(lpDst + dwDstOffset))[i] = ((const uint32 *)lpDefinition)[i];
}

// Original line 335.
template <typename T>
void VECTOR_RANK_CLASS<T>::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 347.
template <typename T>
BOOL VECTOR_RANK_CLASS<T>::Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const
{
    const T * vec = (const T *)lpDefinition;
    BOOL bResult = TRUE;
    if (s_ptDataType == DT_FLOAT)
    {
        bResult &= GenericFloatEncode((void *)&vec->x, GetField(0), tEncoder);
        bResult &= GenericFloatEncode((void *)&vec->y, GetField(1), tEncoder);
#if VECTOR_RANK >= 3
        bResult &= GenericFloatEncode((void *)&vec->z, GetField(2), tEncoder);
#endif
#if VECTOR_RANK >= 4
        bResult &= GenericFloatEncode((void *)&vec->w, GetField(3), tEncoder);
#endif
    }
    else
    {
        bResult &= GenericIntEncode((void *)&vec->x, GetField(0), tEncoder);
        bResult &= GenericIntEncode((void *)&vec->y, GetField(1), tEncoder);
#if VECTOR_RANK >= 3
        bResult &= GenericIntEncode((void *)&vec->z, GetField(2), tEncoder);
#endif
#if VECTOR_RANK >= 4
        bResult &= GenericIntEncode((void *)&vec->w, GetField(3), tEncoder);
#endif
    }
    return bResult;
}

// Original line 373.
template <typename T>
XDecoder::Result VECTOR_RANK_CLASS<T>::Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const
{
    const T * vec = (const T *)lpDefinition;
    XDecoder::Result eResult1 = XDecoder::RESULT_VALID;
    XDecoder::Result eResult2 = XDecoder::RESULT_VALID;
    XDecoder::Result eResult3 = XDecoder::RESULT_VALID;
    XDecoder::Result eResult4 = XDecoder::RESULT_VALID;
    if (s_ptDataType == DT_FLOAT)
    {
        eResult1 = GenericFloatDecode((void *)&vec->x, GetField(0), tDecoder);
        eResult2 = GenericFloatDecode((void *)&vec->y, GetField(1), tDecoder);
#if VECTOR_RANK >= 3
        eResult3 = GenericFloatDecode((void *)&vec->z, GetField(2), tDecoder);
#endif
#if VECTOR_RANK >= 4
        eResult4 = GenericFloatDecode((void *)&vec->w, GetField(3), tDecoder);
#endif
    }
    else
    {
        eResult1 = GenericIntDecode((void *)&vec->x, GetField(0), tDecoder);
        eResult2 = GenericIntDecode((void *)&vec->y, GetField(1), tDecoder);
#if VECTOR_RANK >= 3
        eResult3 = GenericIntDecode((void *)&vec->z, GetField(2), tDecoder);
#endif
#if VECTOR_RANK >= 4
        eResult4 = GenericIntDecode((void *)&vec->w, GetField(3), tDecoder);
#endif
    }
    if (eResult1 != XDecoder::RESULT_VALID)
        return eResult1;
    if (eResult2 != XDecoder::RESULT_VALID)
        return eResult2;
    if (eResult3 != XDecoder::RESULT_VALID)
        return eResult3;
    return eResult4;
}

// Original line 402.
template <typename T>
void VECTOR_RANK_CLASS<T>::Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const
{
    T * vec = (T *)lpDefinition;
    if (s_ptDataType == DT_FLOAT)
    {
        GenericFloatFuzz(&vec->x, GetField(0), tRNG);
        GenericFloatFuzz(&vec->y, GetField(1), tRNG);
#if VECTOR_RANK >= 3
        GenericFloatFuzz(&vec->z, GetField(2), tRNG);
#endif
#if VECTOR_RANK >= 4
        GenericFloatFuzz(&vec->w, GetField(3), tRNG);
#endif
    }
    else
    {
        GenericIntFuzz(&vec->x, GetField(0), tRNG);
        GenericIntFuzz(&vec->y, GetField(1), tRNG);
#if VECTOR_RANK >= 3
        GenericIntFuzz(&vec->z, GetField(2), tRNG);
#endif
#if VECTOR_RANK >= 4
        GenericIntFuzz(&vec->w, GetField(3), tRNG);
#endif
    }
}

// Original line 425.
template <typename T>
void VECTOR_RANK_CLASS<T>::CalculateNetworkBitCounts(TypeDescriptorField & field) const
{
    field.nBitCount = 0;
}
