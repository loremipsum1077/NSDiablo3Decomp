#include "AttributeCompressor.h"
#include "FastAttribute.h"
#include "../../Sigma/Networking/XEncoder.h"
#include "../../Sigma/DataStructures/XFloat16.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"

// Header-defined constants initialize before the message descriptors.
AttributeCompressor * AttributeCompressor::m_ptCreatedCompressors;
static const TypeDescriptorField NetAttributeKeyValue_Fields[] = {
    OptionalBoundedTypeDescriptorField("", DT_INT, 0, 0, 0, (const void *)0xfffff),
    BoundedTypeDescriptorField("", DT_INT, 8, 0, 0, (const void *)0x5cd),
    TypeDescriptorField(0, DT_NULL, 16, 0, 0),
};
ComplexTypeDescriptor TypeDescriptor_NetAttributeKeyValue("NetAttributeKeyValue", 0, NetAttributeKeyValue_Fields, 3, 0, 0, 0, 0, 0, 0);
const XTypeDescriptor * DT_NET_ATTRIBUTE_KEY_VALUE = &TypeDescriptor_NetAttributeKeyValue;
static const TypeDescriptorField AttributeSetValueMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, 0, 0, 1),
    TypeDescriptorField("", DT_INT, 8, 0, 1),
    TypeDescriptorField("", DT_NET_ATTRIBUTE_KEY_VALUE, 12, 0, 1),
    TypeDescriptorField(0, DT_NULL, 28, 0, 0),
};
ComplexTypeDescriptor TypeDescriptor_AttributeSetValueMessage("AttributeSetValueMessage", 0, AttributeSetValueMessage_Fields, 4, 0, 0, 0, 0, 0, 0);
const XTypeDescriptor * DT_ATTRIBUTE_SET_VALUE_MESSAGE = &TypeDescriptor_AttributeSetValueMessage;
static const TypeDescriptorField AttributesSetValuesMessage_Fields[] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, 0, 0, 1),
    TypeDescriptorField("", DT_INT, 8, 0, 1),
    FixedArrayTypeDescriptorField("atKeyVals", DT_NET_ATTRIBUTE_KEY_VALUE, 16, 0, 8, 15, -4),
    TypeDescriptorField(0, DT_NULL, 0x100, 0, 0),
};
ComplexTypeDescriptor TypeDescriptor_AttributesSetValuesMessage("AttributesSetValuesMessage", 0, AttributesSetValuesMessage_Fields, 4, 0, 0, 0, 0, 0, 0);
const XTypeDescriptor * DT_ATTRIBUTES_SET_VALUES_MESSAGE = &TypeDescriptor_AttributesSetValuesMessage;


















BOOL AttributeCompressor::Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) {
    const AttributeCompressor * ptCompressor = KeyGetCompressor(FastAttribKey(tKeyValue.eKeyAttrib));
    return ptCompressor->_Encode(tEncoder, tKeyValue);
}










XDecoder::Result AttributeCompressor::Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) {
    if (tKeyValue.eKeyAttrib >= 0 && tKeyValue.eKeyAttrib < 1485) {
        const AttributeCompressor * ptCompressor = KeyGetCompressor(FastAttribKey(tKeyValue.eKeyAttrib));
        if (ptCompressor) return ptCompressor->_Decode(tDecoder, tKeyValue);
    }
    return XDecoder::RESULT_CORRUPTED;
}
















void AttributeCompressor::Dispose() {
    AttributeCompressor * ptCurrentCompressor = m_ptCreatedCompressors;
    while (ptCurrentCompressor) {
        AttributeCompressor * ptNextCompressor = ptCurrentCompressor->m_ptNextCompressor;
        ::delete ptCurrentCompressor;
        ptCurrentCompressor = ptNextCompressor;
    }
}







inline const AttributeCompressor * AttributeCompressor::_FindNext(const AttributeCompressorType eType, const AttributeCompressor * ptStartCompressor) {
    const AttributeCompressor * ptCurrentCompressor = ptStartCompressor ? ptStartCompressor->m_ptNextCompressor : m_ptCreatedCompressors;
    while (ptCurrentCompressor) {
        if (ptCurrentCompressor->m_eType == eType) return ptCurrentCompressor;
        ptCurrentCompressor = ptCurrentCompressor->m_ptNextCompressor;
    }
    return 0;
}














const AttributeCompressor * AttributeCompressorUncompressed::Register() {
    const AttributeCompressor * ptCurrentCompressor = _FindNext(ACT_UNCOMPRESSED, 0);
    if (!ptCurrentCompressor) ptCurrentCompressor = new AttributeCompressorUncompressed();
    return ptCurrentCompressor;
}














BOOL AttributeCompressorUncompressed::_Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const {
    if (tEncoder.Used() + m_dwBits > tEncoder.Size()) return FALSE;
    tEncoder.Put(tKeyValue.nValue, m_dwBits);
    return TRUE;
}












XDecoder::Result AttributeCompressorUncompressed::_Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const {
    XDecoder::Result eResult = tDecoder.Get(reinterpret_cast<uint32 &>(tKeyValue.nValue), m_dwBits);
    return eResult;
}









const AttributeCompressor * AttributeCompressorBoundedInt::Register(int32 nMin, int32 nMax) {
    const AttributeCompressor * ptCurrentCompressor = _FindNext(ACT_BOUNDED_INT, 0);
    while (ptCurrentCompressor) {
        const AttributeCompressorBoundedInt * ptBoundedIntCompressor = static_cast<const AttributeCompressorBoundedInt *>(ptCurrentCompressor);
        if (ptBoundedIntCompressor->m_nMin == nMin && ptBoundedIntCompressor->m_nMax == nMax) break;
        ptCurrentCompressor = _FindNext(ACT_BOUNDED_INT, ptCurrentCompressor);
    }
    return ptCurrentCompressor ? ptCurrentCompressor : new AttributeCompressorBoundedInt(nMin, nMax);
}























BOOL AttributeCompressorBoundedInt::_Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const {
    if (tEncoder.Used() + m_dwBits > tEncoder.Size()) return FALSE;
    int32 nValToEncode;
    if (m_nMin > tKeyValue.nValue || tKeyValue.nValue > m_nMax) {









        DisplayInternalError(0, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    nValToEncode = tKeyValue.nValue - m_nMin;
    tEncoder.Put(nValToEncode, m_dwBits);
    return TRUE;
}






XDecoder::Result AttributeCompressorBoundedInt::_Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const {
    int32 nDecodedVal;
    XDecoder::Result eResult = tDecoder.Get(reinterpret_cast<uint32 &>(nDecodedVal), m_dwBits);
    if (eResult != XDecoder::RESULT_VALID) return eResult;
    nDecodedVal += m_nMin;
    if (nDecodedVal > m_nMax || nDecodedVal < m_nMin) return XDecoder::RESULT_CORRUPTED;
    tKeyValue.nValue = nDecodedVal;
    return eResult;
}
























































































































const AttributeCompressor * AttributeCompressorFloat16::Register() {
    const AttributeCompressor * ptCurrentCompressor = _FindNext(ACT_FLOAT16, 0);
    if (!ptCurrentCompressor) ptCurrentCompressor = new AttributeCompressorFloat16();
    return ptCurrentCompressor;
}














BOOL AttributeCompressorFloat16::_Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const {
    if (tEncoder.Used() + m_dwBits > tEncoder.Size()) return FALSE;
    float fVal = reinterpret_cast<float &>(tKeyValue.nValue);
    XFloat16 f16Val(fVal);
    tEncoder.Put(f16Val.m_uData, m_dwBits);
    return TRUE;
}











































XDecoder::Result AttributeCompressorFloat16::_Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const {
    uint32 dwDecodedVal;
    XDecoder::Result eResult = tDecoder.Get(dwDecodedVal, m_dwBits);
    if (eResult != XDecoder::RESULT_VALID) return eResult;
    XFloat16 f16Val(static_cast<uint16>(dwDecodedVal));
    reinterpret_cast<float &>(tKeyValue.nValue) = f16Val;
    return eResult;
}















const AttributeCompressor * AttributeCompressorFloatVariable::Register() {
    const AttributeCompressor * ptCurrentCompressor = _FindNext(ACT_FLOAT_VARIABLE, 0);
    if (!ptCurrentCompressor) ptCurrentCompressor = new AttributeCompressorFloatVariable();
    return ptCurrentCompressor;
}














BOOL AttributeCompressorFloatVariable::_Encode(XEncoder & tEncoder, NetAttributeKeyValue & tKeyValue) const {
    float fVal = reinterpret_cast<float &>(tKeyValue.nValue);
    uint32 uBits = 16;
    if (fVal >= XFloat16::GetAbsoluteMaximumValue() || fVal <= -XFloat16::GetAbsoluteMaximumValue()) uBits = 32;
    if (tEncoder.Used() + (uBits + 1) > tEncoder.Size()) return FALSE;
    tEncoder.PutBool(uBits == 16);
    if (uBits == 16) {
        XFloat16 f16Val(fVal);
        tEncoder.Put(f16Val.m_uData, 16);
    } else tEncoder.Put(tKeyValue.nValue, uBits);
    return TRUE;
}

























XDecoder::Result AttributeCompressorFloatVariable::_Decode(XDecoder & tDecoder, NetAttributeKeyValue & tKeyValue) const {
    BOOL bCompressed;
    XDecoder::Result eResult = tDecoder.GetBool(bCompressed);
    if (eResult != XDecoder::RESULT_VALID) return eResult;
    uint32 dwDecodedVal;
    uint32 uBits = bCompressed ? 16 : 32;
    eResult = tDecoder.Get(dwDecodedVal, uBits);
    if (eResult != XDecoder::RESULT_VALID) return eResult;
    if (bCompressed) {
        XFloat16 f16Val(static_cast<uint16>(dwDecodedVal));
        reinterpret_cast<float &>(tKeyValue.nValue) = f16Val;
    } else tKeyValue.nValue = dwDecodedVal;
    return eResult;
}
























void AttributeCompressorFillNetAttributeKeyValue(NetAttributeKeyValue & tKeyVal, FastAttribKey tKey, FastAttribValue tValue) {
    if (KeyGetParamType(tKey) != -1) tKeyVal.nKeyParam.Set(KeyGetParam(tKey));
    else tKeyVal.nKeyParam.SetExists(FALSE);
    tKeyVal.eKeyAttrib = KeyGetAttrib(tKey);
    tKeyVal.nValue = tValue.nValue;
}















FastAttribKey AttributeCompresserAttribKeyFromNetAttributeKeyValue(const NetAttributeKeyValue & tKeyVal) {
    int32 nHighParam = tKeyVal.nKeyParam.Exists() ? tKeyVal.nKeyParam.Get() : -1;
    return MakeAttribKey(tKeyVal.eKeyAttrib, nHighParam);
}
