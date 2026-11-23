// Original source: X/Sigma/TypeDescriptor/DescribedType.cpp.
// All source methods below are ordered by their original DWARF source lines.
#include "X/Sigma/TypeDescriptor/DescribedType.h"
#include "X/Sigma/TypeDescriptor/SymbolicValue.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptor.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "X/Sigma/Networking/XEncoder.h"
#include "X/Sigma/Random.h"
#include "X/Sigma/RGBAColor.h"
#include "X/Sigma/Math/Vector.h"
#include "X/Sigma/Math/MathGeneral.h"
#include "X/Sigma/XML/SAXHelper.h"
#include <stdio.h>

// Original line 40.
class TypeDescriptorClass_DT_NULL : public XTypeDescriptor {  // size 0x38
public:
    // Original line 48.
    TypeDescriptorClass_DT_NULL() : XTypeDescriptor("DT_NULL") {}

    // Original line 52.
    virtual int32 SizeOf() const
    {
        DisplayInternalError(NULL, "", 55, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original line 59.
    virtual CRefString ToString(const void * pData) const
    {
        DisplayInternalError(NULL, "", 63, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original line 67.
    virtual void LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
    {
        DisplayInternalError(NULL, "", 73, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original line 76.
    virtual BOOL Encode(void *, const TypeDescriptorField &, XEncoder &) const
    {
        DisplayInternalError(NULL, "", 81, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original line 85.
    virtual XDecoder::Result Decode(void *, const TypeDescriptorField &, XDecoder &) const
    {
        DisplayInternalError(NULL, "", 90, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original line 94.
    virtual void Fuzz(void *, const TypeDescriptorField &, RNG &) const
    {
        DisplayInternalError(NULL, "", 99, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    // Original line 103.
    virtual void CalculateNetworkBitCounts(TypeDescriptorField & field) const
    {
        field.nBitCount = 0;
    }
};

// Original line 113.
BOOL TypeDescriptorClass_DT_ENUM::FromStringFull(const TypeDescriptorField & tField, const BBETranslationData &, void * pData, const CRefString & szData) const
{
    *(int *)pData = NameToSymbol(szData, tField.pSymbolTable);
    return !(szData == "");
}

// Original line 128.
TDVariant TypeDescriptorClass_DT_ENUM::ToVariant(const void * pData) const
{
    return TDVariant(*(const int *)pData);
}

// Original line 135.
BOOL TypeDescriptorClass_DT_ENUM::FromVariant(void * pData, const TDVariant & tValue) const
{
    switch (tValue.GetSource())
    {
    case TDVARIANT_SOURCE_INVALID:
        break;
    case TDVARIANT_SOURCE_STRING:
        DisplayInternalError(NULL, "", 146, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    default:
        *(int *)pData = tValue.GetInt();
        break;
    }
    return TRUE;
}

// Original line 161.
void TypeDescriptorClass_DT_ENUM::CalculateNetworkBitCounts(TypeDescriptorField & field) const
{
    int32 nBitCount = SizeOf() * 8;
    if (field.pSymbolTable)
    {
        field.dwFlags |= (1 << TDF_MINMAX_BOUNDED);
        int32 nMin;
        int32 nMax;
        GetSymbolicMinMax(nMin, nMax, field.pSymbolTable);
        *(int32 *)&field.vMinFieldValue = nMin;
        *(int32 *)&field.vMaxFieldValue = nMax;
        if (nMax == nMin)
        {
            nBitCount = 1;
        }
        else
        {
            int32 nDelta = nMax - nMin;
            nBitCount = Math::LogBase2(nDelta) + 1;
        }
    }
    field.nBitCount = nBitCount;
}

#define VECTOR_RANK 2
#include "X/Sigma/TypeDescriptor/VectorTemplate.hpp"
#undef VECTOR_RANK
#define VECTOR_RANK 3
#include "X/Sigma/TypeDescriptor/VectorTemplate.hpp"
#undef VECTOR_RANK
#define VECTOR_RANK 4
#include "X/Sigma/TypeDescriptor/VectorTemplate.hpp"
#undef VECTOR_RANK

// Original line 205.
class TypeDescriptorClass_DT_IVECTOR2D : public TypeDescriptorClass_DT_VECTOR_RANK2<IVector2D> {  // size 0x38
public:
    // Original line 208.
    TypeDescriptorClass_DT_IVECTOR2D() : TypeDescriptorClass_DT_VECTOR_RANK2<IVector2D>("DT_IVECTOR2D", DT_INT, "%d, %d") {}
};

// Original line 219.
class TypeDescriptorClass_DT_VECTOR2D : public TypeDescriptorClass_DT_VECTOR_RANK2<Vector2D> {  // size 0x38
public:
    // Original line 222.
    TypeDescriptorClass_DT_VECTOR2D() : TypeDescriptorClass_DT_VECTOR_RANK2<Vector2D>("DT_VECTOR2D", DT_FLOAT, "%g, %g") {}
};

// Original line 233.
class TypeDescriptorClass_DT_VECTOR3D : public TypeDescriptorClass_DT_VECTOR_RANK3<Vector3D> {  // size 0x38
public:
    // Original line 236.
    TypeDescriptorClass_DT_VECTOR3D() : TypeDescriptorClass_DT_VECTOR_RANK3<Vector3D>("DT_VECTOR3D", DT_FLOAT, "%g, %g, %g") {}
};

// Original line 247.
class TypeDescriptorClass_DT_VECTOR4D : public TypeDescriptorClass_DT_VECTOR_RANK4<Vector4D> {  // size 0x38
public:
    // Original line 250.
    TypeDescriptorClass_DT_VECTOR4D() : TypeDescriptorClass_DT_VECTOR_RANK4<Vector4D>("DT_VECTOR4D", DT_FLOAT, "%g, %g, %g, %g") {}
};

// Original line 270.
TypeDescriptorClass_DT_RGBACOLOR::TypeDescriptorClass_DT_RGBACOLOR()
    : TypeDescriptorClass_DT_INT("DT_RGBACOLOR")
{
    pfuncDOMVersion = VersionColor;
    nVersion = 1;

    static TypeDescriptorField sarColorFields[4] = {
        TypeDescriptorField("r", DT_BYTE, 0, NULL, 1),
        TypeDescriptorField("g", DT_BYTE, 1, NULL, 1),
        TypeDescriptorField("b", DT_BYTE, 2, NULL, 1),
        TypeDescriptorField("a", DT_BYTE, 3, NULL, 1),
    };

    sarColorFields[0].CalculateNetworkBitCounts();
    sarColorFields[1].CalculateNetworkBitCounts();
    sarColorFields[2].CalculateNetworkBitCounts();
    sarColorFields[3].CalculateNetworkBitCounts();

    arFields = sarColorFields;
    nFieldCount = 4;
}

// Original line 294.
CRefString TypeDescriptorClass_DT_RGBACOLOR::ToString(const void * pData) const
{
    const RGBAColor * color = (const RGBAColor *)pData;
    return CRefString().Format("#%08X", color->ToUIColor());
}

// Original line 301.
void TypeDescriptorClass_DT_RGBACOLOR::FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
{
    RGBAColor * color = (RGBAColor *)pData;
    UIColor uiColor = 0;
    sscanf(szData, "#%08X", &uiColor);
    color->Set(uiColor);
}

// Original line 312.
void TypeDescriptorClass_DT_RGBACOLOR::LoadFromDefaultData(const TypeDescriptorField &, void * pData) const
{
    RGBAColor * color = (RGBAColor *)pData;
    color->Set(0xff, 0xff, 0xff, 0xff);
}

// Original line 320.
void TypeDescriptorClass_DT_RGBACOLOR::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString = ToString(pData);
}

// Original line 328.
BOOL TypeDescriptorClass_DT_RGBACOLOR::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    FromString(fieldSerializationInfo.GetField(), pData, sSerializeString);
    return TRUE;
}

// Original line 337.
void TypeDescriptorClass_DT_RGBACOLOR::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 343, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 377.
void TypeDescriptorClass_DT_RGBACOLOR::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 384, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 417.
int32 TypeDescriptorClass_DT_RGBACOLOR::VersionColor(const int32 nCurrentVersion, const CXMLNode & nodeXML, void * pData)
{
    DisplayInternalError(NULL, "", 443, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 450.
TypeDescriptorClass_DT_RGBACOLORVALUE::TypeDescriptorClass_DT_RGBACOLORVALUE()
    : XTypeDescriptor("DT_RGBACOLORVALUE")
{
    pfuncDOMVersion = VersionColor;
    nVersion = 1;

    static TypeDescriptorField sarColorFields[4] = {
        TypeDescriptorField("r", DT_FLOAT, 0, NULL, 1),
        TypeDescriptorField("g", DT_FLOAT, 4, NULL, 1),
        TypeDescriptorField("b", DT_FLOAT, 8, NULL, 1),
        TypeDescriptorField("a", DT_FLOAT, 12, NULL, 1),
    };

    sarColorFields[0].CalculateNetworkBitCounts();
    sarColorFields[1].CalculateNetworkBitCounts();
    sarColorFields[2].CalculateNetworkBitCounts();
    sarColorFields[3].CalculateNetworkBitCounts();

    arFields = sarColorFields;
    nFieldCount = 4;
}

// Original line 474.
int32 TypeDescriptorClass_DT_RGBACOLORVALUE::SizeOf() const
{
    return sizeof(RGBAColorValue);
}

// Original line 480.
CRefString TypeDescriptorClass_DT_RGBACOLORVALUE::ToString(const void * pData) const
{
    const RGBAColorValue * color = (const RGBAColorValue *)pData;
    return CRefString().Format("sc# %f,%f,%f,%f", color->a, color->r, color->g, color->b);
}

// Original line 487.
void TypeDescriptorClass_DT_RGBACOLORVALUE::FromString(const TypeDescriptorField &, void * pData, const CRefString & szData) const
{
    RGBAColorValue * color = (RGBAColorValue *)pData;
    sscanf(szData, "sc# %f,%f,%f,%f", &color->a, &color->r, &color->g, &color->b);
}

// Original line 496.
void TypeDescriptorClass_DT_RGBACOLORVALUE::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString = ToString(pData);
}

// Original line 504.
BOOL TypeDescriptorClass_DT_RGBACOLORVALUE::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    FromString(fieldSerializationInfo.GetField(), pData, sSerializeString);
    return TRUE;
}

// Original line 513.
void TypeDescriptorClass_DT_RGBACOLORVALUE::LoadFromDefaultData(const TypeDescriptorField &, void * pData) const
{
    RGBAColorValue * color = (RGBAColorValue *)pData;
    color->Set(1.0f, 1.0f, 1.0f, 1.0f);
}

// Original line 521.
void TypeDescriptorClass_DT_RGBACOLORVALUE::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 527, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 561.
void TypeDescriptorClass_DT_RGBACOLORVALUE::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 568, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 601.
int32 TypeDescriptorClass_DT_RGBACOLORVALUE::VersionColor(const int32 nCurrentVersion, const CXMLNode & nodeXML, void * pData)
{
    DisplayInternalError(NULL, "", 627, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original line 631.
void TypeDescriptorClass_DT_RGBACOLORVALUE::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    const RGBAColorValue * colorSrc = (const RGBAColorValue *)lpDefinitionSrc;
    RGBAColorValue * colorDst = (RGBAColorValue *)lpDefinitionDst;
    *(uint32 *)&colorDst->a = *(const uint32 *)&colorSrc->a;
    *(uint32 *)&colorDst->r = *(const uint32 *)&colorSrc->r;
    *(uint32 *)&colorDst->g = *(const uint32 *)&colorSrc->g;
    *(uint32 *)&colorDst->b = *(const uint32 *)&colorSrc->b;
}

// Original line 644.
void TypeDescriptorClass_DT_RGBACOLORVALUE::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original line 656.
void TypeDescriptorClass_DT_RGBACOLORVALUE::SerializeToBinary(void * lpDefinition, const TypeDescriptorField &, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    const RGBAColorValue * colorSrc = (const RGBAColorValue *)lpDefinition;
    RGBAColorValue * colorDst = (RGBAColorValue *)(lpDst + dwDstOffset);
    *(uint32 *)&colorDst->a = *(const uint32 *)&colorSrc->a;
    *(uint32 *)&colorDst->r = *(const uint32 *)&colorSrc->r;
    *(uint32 *)&colorDst->g = *(const uint32 *)&colorSrc->g;
    *(uint32 *)&colorDst->b = *(const uint32 *)&colorSrc->b;
}

// Original line 673.
void TypeDescriptorClass_DT_RGBACOLORVALUE::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original line 685.
BOOL TypeDescriptorClass_DT_RGBACOLORVALUE::Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const
{
    const RGBAColorValue * color = (const RGBAColorValue *)lpDefinition;
    BOOL bResult = TRUE;
    bResult &= GenericFloatEncode((void *)&color->a, GetField(0), tEncoder);
    bResult &= GenericFloatEncode((void *)&color->r, GetField(1), tEncoder);
    bResult &= GenericFloatEncode((void *)&color->g, GetField(2), tEncoder);
    bResult &= GenericFloatEncode((void *)&color->b, GetField(3), tEncoder);
    return bResult;
}

// Original line 701.
XDecoder::Result TypeDescriptorClass_DT_RGBACOLORVALUE::Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const
{
    const RGBAColorValue * color = (const RGBAColorValue *)lpDefinition;
    XDecoder::Result eResult1 = GenericFloatDecode((void *)&color->a, GetField(0), tDecoder);
    XDecoder::Result eResult2 = GenericFloatDecode((void *)&color->r, GetField(1), tDecoder);
    XDecoder::Result eResult3 = GenericFloatDecode((void *)&color->g, GetField(2), tDecoder);
    XDecoder::Result eResult4 = GenericFloatDecode((void *)&color->b, GetField(3), tDecoder);
    if (eResult1 != XDecoder::RESULT_VALID)
        return eResult1;
    if (eResult2 != XDecoder::RESULT_VALID)
        return eResult2;
    if (eResult3 != XDecoder::RESULT_VALID)
        return eResult3;
    return eResult4;
}

// Original line 720.
void TypeDescriptorClass_DT_RGBACOLORVALUE::Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const
{
    const RGBAColorValue * color = (const RGBAColorValue *)lpDefinition;
    GenericFloatFuzz((void *)&color->a, GetField(0), tRNG);
    GenericFloatFuzz((void *)&color->r, GetField(1), tRNG);
    GenericFloatFuzz((void *)&color->g, GetField(2), tRNG);
    GenericFloatFuzz((void *)&color->b, GetField(3), tRNG);
}

// Original line 733.
void TypeDescriptorClass_DT_RGBACOLORVALUE::CalculateNetworkBitCounts(TypeDescriptorField & field) const
{
    field.nBitCount = 0;
}

// Original lines 742-767.
static const TypeDescriptorClass_DT_NULL TypeDescriptor_DT_NULL;
const XTypeDescriptor * DT_NULL = &TypeDescriptor_DT_NULL;
static const TypeDescriptorClass_DT_INT TypeDescriptor_DT_INT;
const XTypeDescriptor * DT_INT = &TypeDescriptor_DT_INT;
static const TypeDescriptorClass_DT_DATAID TypeDescriptor_DT_DATAID;
const XTypeDescriptor * DT_DATAID = &TypeDescriptor_DT_DATAID;
static const TypeDescriptorClass_DT_INT64 TypeDescriptor_DT_INT64;
const XTypeDescriptor * DT_INT64 = &TypeDescriptor_DT_INT64;
static const TypeDescriptorClass_DT_BYTE TypeDescriptor_DT_BYTE;
const XTypeDescriptor * DT_BYTE = &TypeDescriptor_DT_BYTE;
static const TypeDescriptorClass_DT_FLOAT TypeDescriptor_DT_FLOAT;
const XTypeDescriptor * DT_FLOAT = &TypeDescriptor_DT_FLOAT;
static const TypeDescriptorClass_DT_STRING TypeDescriptor_DT_STRING;
const XTypeDescriptor * DT_STRING = &TypeDescriptor_DT_STRING;
static const TypeDescriptorClass_DT_WORD TypeDescriptor_DT_WORD;
const XTypeDescriptor * DT_WORD = &TypeDescriptor_DT_WORD;
static const TypeDescriptorClass_DT_CHARARRAY TypeDescriptor_DT_CHARARRAY;
const XTypeDescriptor * DT_CHARARRAY = &TypeDescriptor_DT_CHARARRAY;
static const TypeDescriptorClass_DT_ANGLE TypeDescriptor_DT_ANGLE;
const XTypeDescriptor * DT_ANGLE = &TypeDescriptor_DT_ANGLE;
static const TypeDescriptorClass_DT_PERCENT TypeDescriptor_DT_PERCENT;
const XTypeDescriptor * DT_PERCENT = &TypeDescriptor_DT_PERCENT;
static const TypeDescriptorClass_DT_HIGHPRECISIONPERCENT TypeDescriptor_DT_HIGHPRECISIONPERCENT;
const XTypeDescriptor * DT_HIGHPRECISIONPERCENT = &TypeDescriptor_DT_HIGHPRECISIONPERCENT;
static const TypeDescriptorClass_DT_ENUM TypeDescriptor_DT_ENUM;
const XTypeDescriptor * DT_ENUM = &TypeDescriptor_DT_ENUM;
static const TypeDescriptorClass_DT_FLAG TypeDescriptor_DT_FLAG;
const XTypeDescriptor * DT_FLAG = &TypeDescriptor_DT_FLAG;
static const TypeDescriptorClass_DT_TRANSLATEABLE TypeDescriptor_DT_TRANSLATEABLE;
const XTypeDescriptor * DT_TRANSLATEABLE = &TypeDescriptor_DT_TRANSLATEABLE;
static const TypeDescriptorClass_DT_FIXEDARRAY TypeDescriptor_DT_FIXEDARRAY;
const XTypeDescriptor * DT_FIXEDARRAY = &TypeDescriptor_DT_FIXEDARRAY;
static const TypeDescriptorClass_DT_VARIABLEARRAY TypeDescriptor_DT_VARIABLEARRAY;
const XTypeDescriptor * DT_VARIABLEARRAY = &TypeDescriptor_DT_VARIABLEARRAY;
static const TypeDescriptorClass_DT_VARIABLEARRAY_COUNT TypeDescriptor_DT_VARIABLEARRAY_COUNT;
const XTypeDescriptor * DT_VARIABLEARRAY_COUNT = &TypeDescriptor_DT_VARIABLEARRAY_COUNT;
static const TypeDescriptorClass_DT_CSTRING TypeDescriptor_DT_CSTRING;
const XTypeDescriptor * DT_CSTRING = &TypeDescriptor_DT_CSTRING;
static const TypeDescriptorClass_DT_OPTIONAL TypeDescriptor_DT_OPTIONAL;
const XTypeDescriptor * DT_OPTIONAL = &TypeDescriptor_DT_OPTIONAL;
static const TypeDescriptorClass_DT_IVECTOR2D TypeDescriptor_DT_IVECTOR2D;
const XTypeDescriptor * DT_IVECTOR2D = &TypeDescriptor_DT_IVECTOR2D;
static const TypeDescriptorClass_DT_VECTOR2D TypeDescriptor_DT_VECTOR2D;
const XTypeDescriptor * DT_VECTOR2D = &TypeDescriptor_DT_VECTOR2D;
static const TypeDescriptorClass_DT_VECTOR3D TypeDescriptor_DT_VECTOR3D;
const XTypeDescriptor * DT_VECTOR3D = &TypeDescriptor_DT_VECTOR3D;
static const TypeDescriptorClass_DT_VECTOR4D TypeDescriptor_DT_VECTOR4D;
const XTypeDescriptor * DT_VECTOR4D = &TypeDescriptor_DT_VECTOR4D;
static const TypeDescriptorClass_DT_RGBACOLOR TypeDescriptor_DT_RGBACOLOR;
const XTypeDescriptor * DT_RGBACOLOR = &TypeDescriptor_DT_RGBACOLOR;
static const TypeDescriptorClass_DT_RGBACOLORVALUE TypeDescriptor_DT_RGBACOLORVALUE;
const XTypeDescriptor * DT_RGBACOLORVALUE = &TypeDescriptor_DT_RGBACOLORVALUE;
