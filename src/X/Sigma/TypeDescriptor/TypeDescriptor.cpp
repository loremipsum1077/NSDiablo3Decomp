// Original file: X/Sigma/TypeDescriptor/TypeDescriptor.cpp

#include "TypeDescriptor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/Crypto.h"
#include "../DataStructures/XFloat16.h"
#include "../Networking/XEncoder.h"
#include "../Random.h"
#include "TypeDescriptorField.h"
ComplexTypeDescriptor * ComplexTypeDescriptor::pRegistrationHead;  // DWARF: defined in this CU (0x19640e8, .bss); definition line not recorded
int32 XTypeDescriptor::s_nIgnoringFieldCount;
int32 XTypeDescriptor::s_nServerOnlyFieldCount;
const TypeDescriptorField & XTypeDescriptor::GetField(int32 nIndex) const
{
    return arFields[nIndex];
}

LPCSTR XTypeDescriptor::GetName() const
{
    return szClassName;
}

BOOL XTypeDescriptor::VariantsEqual(const TDVariant & tValue1, const TDVariant & tValue2) const
{
    uint32 dwValue1 = 0;
    uint32 dwValue2 = 0;
    FromVariant(&dwValue1, tValue1);
    FromVariant(&dwValue2, tValue2);
    return dwValue1 == dwValue2;
}

void XTypeDescriptor::CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const
{
    char szTempString[256];

    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED))
    {
        int32 nMinVal = *(const int32 *)&field.vMinFieldValue;
        int32 nMaxVal = *(const int32 *)&field.vMaxFieldValue;
        Blizzard::String::Format(szTempString, sizeof(szTempString), "%s(%d,%d)", GetName(), nMinVal, nMaxVal);
    }
    else
    {
        Blizzard::String::Format(szTempString, sizeof(szTempString), "%s", GetName());
    }

    dwHash = Blizzard::Util::CRC32(szTempString, strlen(szTempString), dwHash);
}

#line 333 "X/Sigma/TypeDescriptor/TypeDescriptor.cpp"
void ComplexTypeDescriptor::ScanForSubfields(
    const TypeDescriptorField &) const
{
    if (dwFlags & (1 << TD_SCANNED_FOR_FIELDS))
    {
        return;
    }

#line 371
    // The original caches scan results through a mutable descriptor pointer.
    ComplexTypeDescriptor * ptTypeDesc = const_cast<ComplexTypeDescriptor *>(this);
    ptTypeDesc->dwFlags |= (1 << TD_BEING_SCANNED_FOR_FIELDS);

    BOOL bContainsSNOSubfield = FALSE;
    BOOL bContainsAllocationSubfield = FALSE;
    BOOL bContainsServerOnlySubfield = FALSE;



    BOOL bContains64BitSubfield = FALSE;
    BOOL bContains128BitSubfield = FALSE;

    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull())
        {
            break;
        }

        const XTypeDescriptor * ptTypeDescChild = fieldChild.GetTypeDesc();


        // A self-referential array records its own flags and skips recursive descent.
        if ((ptTypeDescChild == DT_VARIABLEARRAY ||
             ptTypeDescChild == DT_FIXEDARRAY ||
             ptTypeDescChild == DT_CSTRING) &&
            fieldChild.GetAuxTypeDesc() == this)
        {

            if (ptTypeDescChild->ContainsAllocationSubfields(fieldChild))
            {
                bContainsAllocationSubfield = TRUE;
            }

            if (fieldChild.dwFlags & (1 << TDF_SERVER_ONLY))
            {
                bContainsServerOnlySubfield = TRUE;
            }
        }
        else
        {
            // Ordinary fields contribute their recursively scanned properties.

            ptTypeDescChild->ScanForSubfields(fieldChild);

            if (ptTypeDescChild->ContainsAllocationSubfields(fieldChild))
            {
                bContainsAllocationSubfield = TRUE;
            }

            if (ptTypeDescChild->Contains64BitSubfields(fieldChild))
            {
                bContains64BitSubfield = TRUE;
            }

            if (ptTypeDescChild->Contains128BitSubfields(fieldChild))
            {
                bContains128BitSubfield = TRUE;
            }

            if (fieldChild.dwFlags & (1 << TDF_XPTR64))
            {
                bContains64BitSubfield = TRUE;
            }

            if (fieldChild.dwFlags & (1 << TDF_XPTR128))
            {
                bContains128BitSubfield = TRUE;
            }












            if (ptTypeDescChild->ContainsSNOSubfield(fieldChild))
            {
                bContainsSNOSubfield = TRUE;
            }

            if (fieldChild.dwFlags & (1 << TDF_SERVER_ONLY))
            {
                bContainsServerOnlySubfield = TRUE;
            }

        }
    }

    if (bContainsSNOSubfield)
        ptTypeDesc->dwFlags |= (1 << TD_CONTAINS_SNO_SUBFIELD);
    else
        ptTypeDesc->dwFlags &= ~(1 << TD_CONTAINS_SNO_SUBFIELD);
    if (bContainsAllocationSubfield)
        ptTypeDesc->dwFlags |= (1 << TD_CONTAINS_ALLOCATION_SUBFIELD);
    else
        ptTypeDesc->dwFlags &= ~(1 << TD_CONTAINS_ALLOCATION_SUBFIELD);
    if (bContainsServerOnlySubfield)
        ptTypeDesc->dwFlags |= (1 << TD_CONTAINS_SERVER_ONLY_SUBFIELD);
    else
        ptTypeDesc->dwFlags &= ~(1 << TD_CONTAINS_SERVER_ONLY_SUBFIELD);

    if (bContains64BitSubfield)
        ptTypeDesc->dwFlags |= (1 << TD_CONTAINS_64BIT_SUBFIELD);
    else
        ptTypeDesc->dwFlags &= ~(1 << TD_CONTAINS_64BIT_SUBFIELD);
    if (bContains128BitSubfield)
        ptTypeDesc->dwFlags |= (1 << TD_CONTAINS_128BIT_SUBFIELD);
    else
        ptTypeDesc->dwFlags &= ~(1 << TD_CONTAINS_128BIT_SUBFIELD);

#line 478
    ptTypeDesc->dwFlags &= ~(1 << TD_BEING_SCANNED_FOR_FIELDS);
    ptTypeDesc->dwFlags |= (1 << TD_SCANNED_FOR_FIELDS);
}

#line 138 "X/Sigma/TypeDescriptor/TypeDescriptor.cpp"
TDVariant::TDVariant(int32 nValue)
{
    Set(nValue);
}

TDVariant::TDVariant(float flValue)
{
    Set(flValue);
}

TDVariant::TDVariant(LPCSTR sValue)
{
    Set(sValue);
}

TDVariant::TDVariant(LPCSTR sValue, uint32 uValue)
{
    Set(sValue, uValue);
}

TDVariant::TDVariant(LPCSTR sValue, int32 nValue)
{
    Set(sValue, nValue);
}

TDVariant::TDVariant(LPCSTR sValue, float flValue)
{
    Set(sValue, flValue);
}

TDVariant::TDVariant(LPCSTR sValue, uint64 uValue)
{
    Set(sValue, uValue);
}

inline void TDVariant::Set(int32 nValue)
{
    m_value.m_int32 = nValue;
    m_sValue.Format("%d", nValue);
    m_eSource = TDVARIANT_SOURCE_INT32;
}

inline void TDVariant::Set(uint32 uValue)
{
    m_value.m_uint32 = uValue;
    m_sValue.Format("%u", uValue);
    m_eSource = TDVARIANT_SOURCE_UINT32;
}

// "%g" switches to exponent notation for small and large values; those are
// reprinted as plain digits and the trailing zeroes trimmed off.
static void sTrimFloat(char * szBuffer, int32 nLength)
{
    char * pEnd = szBuffer + nLength - 1;
    while (pEnd >= szBuffer)
    {
        if (*pEnd != '0')
        {
            if (*pEnd == '.')
                *pEnd = '\0';
            break;
        }
        *pEnd-- = '\0';
    }
}

void TDVariant::Set(float flValue)
{
    m_value.m_float = flValue;
    m_sValue.Format("%g", flValue);
    if (strchr(m_sValue.str(), 'e'))
    {
        char buffer[512];
        int32 nLength = sprintf(buffer, "%.32f", m_value.m_float);
        sTrimFloat(buffer, nLength);
        m_sValue = buffer;
    }
    m_eSource = TDVARIANT_SOURCE_FLOAT;
}

inline void TDVariant::Set(LPCSTR sValue)
{
    m_sValue = sValue;
    m_value.m_float = (float)atof(sValue);
    m_eSource = TDVARIANT_SOURCE_STRING;
}

inline void TDVariant::Set(LPCSTR sValue, int32 nValue)
{
    m_sValue = sValue;
    m_value.m_int32 = nValue;
    m_eSource = TDVARIANT_SOURCE_INT32;
}

inline void TDVariant::Set(LPCSTR sValue, uint32 uValue)
{
    m_sValue = sValue;
    m_value.m_uint32 = uValue;
    m_eSource = TDVARIANT_SOURCE_UINT32;
}

inline void TDVariant::Set(LPCSTR sValue, int64 nValue)
{
    m_sValue = sValue;
    m_value.m_int64 = nValue;
    m_eSource = TDVARIANT_SOURCE_INT64;
}

inline void TDVariant::Set(LPCSTR sValue, uint64 uValue)
{
    m_sValue = sValue;
    m_value.m_uint64 = uValue;
    m_eSource = TDVARIANT_SOURCE_UINT64;
}

inline void TDVariant::Set(LPCSTR sValue, float flValue)
{
    m_sValue = sValue;
    m_value.m_float = flValue;
    m_eSource = TDVARIANT_SOURCE_FLOAT;
}

uint32 TDVariant::GetUInt() const
{
    if (m_eSource == TDVARIANT_SOURCE_INT32 || m_eSource == TDVARIANT_SOURCE_UINT32)
        return m_value.m_uint32;
    if (m_eSource == TDVARIANT_SOURCE_INT64 || m_eSource == TDVARIANT_SOURCE_UINT64)
        return (uint32)m_value.m_uint64;
    return (uint32)m_value.m_float;
}

int32 TDVariant::GetInt() const
{
    if (m_eSource == TDVARIANT_SOURCE_INT32 || m_eSource == TDVARIANT_SOURCE_UINT32)
        return m_value.m_int32;
    if (m_eSource == TDVARIANT_SOURCE_INT64 || m_eSource == TDVARIANT_SOURCE_UINT64)
        return (int32)m_value.m_int64;
    return (int32)m_value.m_float;
}

float TDVariant::GetFloat() const
{
    switch (m_eSource)
    {
    case TDVARIANT_SOURCE_INT32:
        return (float)m_value.m_int32;
    case TDVARIANT_SOURCE_UINT32:
        return (float)m_value.m_uint32;
    case TDVARIANT_SOURCE_INT64:
        return (float)m_value.m_int64;
    case TDVARIANT_SOURCE_UINT64:
        return (float)m_value.m_uint64;
    default:
        return m_value.m_float;
    }
}

BOOL GenericFloatEncode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder)
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return FALSE;

    float fVal = *(float *)lpDefinition;
    uint32 dwValToTransmit;

    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED))
    {
        // Non-short-circuit `&`: the original loads both bounds up front and tests them with fccmp.
        if (!((fVal >= *(const float *)&field.vMinFieldValue) & (fVal <= *(const float *)&field.vMaxFieldValue)))
        {
            DisplayInternalError(NULL, "", 874, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }

    if (field.dwFlags & (1 << TDF_XFLOAT16))
    {
        XFloat16 f16Val(fVal);
        dwValToTransmit = f16Val.m_uData;
    }
    else
    {
        dwValToTransmit = *(uint32 *)&fVal;
    }

    tEncoder.Put(dwValToTransmit);
    return TRUE;
}

BOOL GenericIntEncode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder)
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return FALSE;

    int32 nValToEncode = *(int32 *)lpDefinition;

    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED))
    {
        int32 nMaxVal = *(const int32 *)&field.vMaxFieldValue;
        int32 nMinVal = *(const int32 *)&field.vMinFieldValue;
        if (nValToEncode < nMinVal || nValToEncode > nMaxVal)
        {
            DisplayInternalError(NULL, "", 917, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        nValToEncode -= nMinVal;
    }

    tEncoder.Put(nValToEncode, field.nBitCount);
    return TRUE;
}

#line 931 "X/Sigma/TypeDescriptor/TypeDescriptor.cpp"
XDecoder::Result GenericFloatDecode(
    void * lpDefinition,
    const TypeDescriptorField & field,
    XDecoder & tDecoder)
{
    uint32 dwDecodedVal;
    XDecoder::Result eResult = tDecoder.Get(dwDecodedVal, field.nBitCount);

    if (eResult != XDecoder::RESULT_VALID)
    {
        return eResult;
    }

    if (field.dwFlags & (1 << TDF_XFLOAT16))
    {

        XFloat16 f16Val((uint16)dwDecodedVal);
        *(float *)&dwDecodedVal = f16Val;
    }



    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED))
    {
        float fMinVal = *(const float *)&field.vMinFieldValue;
        float fMaxVal = *(const float *)&field.vMaxFieldValue;
        if (!((*(float *)&dwDecodedVal <= fMaxVal) & (*(float *)&dwDecodedVal >= fMinVal))) // Same inclusive bitwise range check as GenericFloatEncode.
        {
            return XDecoder::RESULT_CORRUPTED;
        }
    }
    // The original stores the decoded payload as a 32-bit word.
    *(uint32 *)lpDefinition = dwDecodedVal;
    return XDecoder::RESULT_VALID;
}

#line 381 "X/Sigma/TypeDescriptor/TypeDescriptor.cpp"
XDecoder::Result GenericIntDecode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder)
{
    int32 nDecodedVal;
    XDecoder::Result eResult = tDecoder.Get(*(uint32 *)&nDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;

    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED))
    {
        int32 nMinVal = *(const int32 *)&field.vMinFieldValue;
        int32 nMaxVal = *(const int32 *)&field.vMaxFieldValue;
        nDecodedVal += nMinVal;
        if (nDecodedVal < nMinVal || nDecodedVal > nMaxVal)
            return XDecoder::RESULT_CORRUPTED;
    }

    *(int32 *)lpDefinition = nDecodedVal;
    return XDecoder::RESULT_VALID;
}

void GenericFloatFuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG)
{
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED))
    {
        float fMinVal = *(const float *)&field.vMinFieldValue;
        float fMaxVal = *(const float *)&field.vMaxFieldValue;
        *(float *)lpDefinition = fMinVal * RandFloat_Internal(tRNG) * (fMaxVal - fMinVal);
    }
    else
    {
        *(float *)lpDefinition = RandFloat_Internal(tRNG) * 65536.0f;
    }
}

void GenericIntFuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG)
{
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED))
    {
        int32 nMinVal = *(const int32 *)&field.vMinFieldValue;
        int32 nMaxVal = *(const int32 *)&field.vMaxFieldValue;
        *(int32 *)lpDefinition = RandRange_Internal(tRNG, nMinVal, nMaxVal);
    }
    else
    {
        *(uint32 *)lpDefinition = RandGetDWord(tRNG);
    }
}

void XTypeDescriptor::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 1049, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

void XTypeDescriptor::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 1077, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

MergeableSerializeDataInfo::~MergeableSerializeDataInfo()
{
}
