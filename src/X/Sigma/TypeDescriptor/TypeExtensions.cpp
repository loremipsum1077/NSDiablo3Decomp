// Original source: X/Sigma/TypeDescriptor/TypeExtensions.cpp.
// All source methods below are ordered by their original DWARF source lines.
#include "X/Sigma/TypeDescriptor/DescribedType.h"
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
#include "BlizzardCore/Include/Blizzard/Util/Crypto.h"
#include <string.h>
#include "X/Sigma/Files/XFilesCommon.h"
#include "X/Sigma/Sigma.h"
#include "X/Sigma/DataStructures/XGrowableArray.h"
#include "X/Sigma/XML/XMLString.h"
#include <cstring>
#include "X/Common/Definitions/AttachmentDefinition.h"
#include "X/Common/Marker/Marker.h"
#include <stdio.h>
#include "X/Common/Definitions/Definitions.h"

// Declarations recovered from X/Common/Marker/Marker.cpp; the matching header
// declarations do not survive in the available reference material.
void MarkerIterateSnoReferences(const Marker & tMarker, SNOIterateFunc pfuncIterate, void * pData);
void MarkerChangeSNOReference(Marker & tMarker, const SNOName & snonameOld, const SNO snoNew);
extern const XTypeDescriptor * DT_MARKER;
extern const XTypeDescriptor * DT_TRIGGEREVENT;
// Defined by the serialization loader; declaration retained from its call evidence.
void * sLoadSerializedTracked(const UnserializeData &, const SerializeData &, uint32, LPCSTR, int32, LPCSTR);

// SERIALIZABLE_DATA_TYPE_VERSION_STRING, VARIANT_TYPE_STRING, and
// OPTIONAL_EXISTS_STRING have no surviving ELF data and are omitted.

// Original TypeExtensions.cpp line 96.
void ComplexTypeDescriptor::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 102, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 142.
void ComplexTypeDescriptor::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 149, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 196.
void ComplexTypeDescriptor::IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const
{
    if (!ContainsSNOSubfield(field)) return;
    if (this == DT_TRIGGEREVENT)
    {
        const TriggerEvent & tTriggerEvent = *(const TriggerEvent *)lpDefinition;
        TriggerEventIterateSnoReferences(tTriggerEvent, pfuncIterate, pData);
        return;
    }
    if (this == DT_MARKER)
    {
        const Marker & tMarker = *(const Marker *)lpDefinition;
        MarkerIterateSnoReferences(tMarker, pfuncIterate, pData);
        return;
    }
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->IterateSNOReferences(lpSubDef, fieldChild, pfuncIterate, pData);
    }
}

// Original TypeExtensions.cpp line 243.

void ComplexTypeDescriptor::IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const
{
    if (lpDefinition == NULL && (dwFlags & (1 << TD_BEING_SCANNED_FOR_FIELDS)))
        return;
    ComplexTypeDescriptor * ptTypeDesc = const_cast<ComplexTypeDescriptor *>(this);
    ptTypeDesc->dwFlags |= 1 << TD_BEING_SCANNED_FOR_FIELDS;
    pfuncIterate(this, &field, lpDefinition, pData);
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        const void * lpSubDef = lpDefinition ? fieldChild.Offset(lpDefinition) : NULL;
        fieldChild.GetTypeDesc()->IterateDefinitionFields(lpSubDef, fieldChild, pfuncIterate, pData);
    }
    ptTypeDesc->dwFlags &= ~(1 << TD_BEING_SCANNED_FOR_FIELDS);
}

// Original TypeExtensions.cpp line 281.
void ComplexTypeDescriptor::ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const
{
    if (!ContainsSNOSubfield(field)) return;
    if (this == DT_TRIGGEREVENT)
    {
        TriggerEvent & tTriggerEvent = *(TriggerEvent *)lpDefinition;
        TriggerEventChangeSNOReference(tTriggerEvent, snonameOld, snoNew);
        return;
    }
    if (this == DT_MARKER)
    {
        Marker & tMarker = *(Marker *)lpDefinition;
        MarkerChangeSNOReference(tMarker, snonameOld, snoNew);
        return;
    }
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->ChangeSNOReference(lpSubDef, fieldChild, snonameOld, snoNew);
    }
}

// Original TypeExtensions.cpp line 328.
void ComplexTypeDescriptor::DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const
{
    if (!ContainsSNOSubfield(field)) return;
    if (this == DT_TRIGGEREVENT)
    {
        const TriggerEvent & tTriggerEvent = *(const TriggerEvent *)lpDefinition;
        fprintf(pDumpData->fileOut, "%s\t%d\n", pDumpData->sLabel.str(), tTriggerEvent.eType);
        return;
    }
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->DebugDump(lpSubDef, fieldChild, pDumpData);
    }
}

// Original TypeExtensions.cpp line 369.
void ComplexTypeDescriptor::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField &) const
{
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        void * lpSubDefSrc = fieldChild.Offset(lpDefinitionSrc);
        void * lpSubDefDst = fieldChild.Offset(lpDefinitionDst);
        fieldChild.GetTypeDesc()->Byteswap(lpSubDefDst, lpSubDefSrc, lpDefinitionSrcBase, fieldChild);
    }
}

// Original TypeExtensions.cpp line 393.
void ComplexTypeDescriptor::ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField &) const
{
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->ByteswapInPlace(lpSubDef, lpDefinitionBase, fieldChild);
    }
}

// Original TypeExtensions.cpp line 415.
void ComplexTypeDescriptor::AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const
{
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        s_nIgnoringFieldCount += (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount += (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
        fieldChild.GetTypeDesc()->AdjustSerializeDataSize(lpSubDef, fieldChild, fWantFullDefinition);
        s_nIgnoringFieldCount -= (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount -= (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
    }
}

// Original TypeExtensions.cpp line 445.
void ComplexTypeDescriptor::ComputeMergedOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    uint32 dwBaseOffset = dwOffset;
    uint32 dwTempOffset = 0;
    do
    {
        dwOffset = dwTempOffset;
        dwTempOffset = dwBaseOffset;
        MergeableSerializeDataInfoList tTypeSerializeDataInfoList;
        ComputeOffsetsAndMerge(lpDefinition, field, dwTempOffset, tTypeSerializeDataInfoList);
    } while (dwTempOffset != dwOffset);
}

// Original TypeExtensions.cpp line 461.
void ComplexTypeDescriptor::ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const
{
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        s_nIgnoringFieldCount += (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount += (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
        fieldChild.GetTypeDesc()->ComputeOffsetsAndMerge(lpSubDef, fieldChild, dwOffset, tTypeSerializedDataList);
        s_nIgnoringFieldCount -= (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount -= (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
    }
}

// Original TypeExtensions.cpp line 499.
void ComplexTypeDescriptor::ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        s_nIgnoringFieldCount += (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount += (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
        fieldChild.GetTypeDesc()->ComputeOffsets(lpSubDef, fieldChild, dwOffset);
        s_nIgnoringFieldCount -= (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount -= (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
    }
}

// Original TypeExtensions.cpp line 535.
void ComplexTypeDescriptor::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        s_nIgnoringFieldCount += (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount += (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
        int32 nOffsetDelta = fieldChild.nDataOffset;
        fieldChild.GetTypeDesc()->SerializeToBinary(lpSubDef, fieldChild, lpDst, dwDstOffset + nOffsetDelta, dwTotalSize);
        s_nIgnoringFieldCount -= (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount -= (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
    }
}

// Original TypeExtensions.cpp line 593.
void ComplexTypeDescriptor::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{
    if (!ContainsAllocationSubfields(field)) return;
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = arFields[i];
        if (fieldChild.IsNull()) break;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        s_nIgnoringFieldCount += (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount += (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
        fieldChild.GetTypeDesc()->SerializeFromBinary(lpSubDef, tUnserializeData, fieldChild);
        s_nIgnoringFieldCount -= (fieldChild.dwFlags >> TDF_IGNORE_ON_SERVER) & 1;
        s_nServerOnlyFieldCount -= (fieldChild.dwFlags >> TDF_SERVER_ONLY) & 1;
    }
}

// Original TypeExtensions.cpp line 638.
void ComplexTypeDescriptor::Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const
{
    if (!ContainsAllocationSubfields(field)) return;
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->Delete(dwDefinitionFlags, lpSubDef, fieldChild, ptMemoryInterface);
    }
}

// Original TypeExtensions.cpp line 667.
BOOL ComplexTypeDescriptor::Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const
{
    size_t BitsUsed = tEncoder.Used();
    uint32 nValidFieldCount = nFieldCount - 1;
    for (uint32 i = 0; i < nValidFieldCount; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        if (!fieldChild.GetTypeDesc()->Encode(lpSubDef, fieldChild, tEncoder))
        {
            tEncoder.SetUsed(BitsUsed);
            return FALSE;
        }
    }
    return TRUE;
}

// Original TypeExtensions.cpp line 693.
XDecoder::Result ComplexTypeDescriptor::Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const
{
    uint32 nValidFieldCount = nFieldCount - 1;
    for (uint32 i = 0; i < nValidFieldCount; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        XDecoder::Result eResult = fieldChild.GetTypeDesc()->Decode(lpSubDef, fieldChild, tDecoder);
        if (eResult != XDecoder::RESULT_VALID) return eResult;
    }
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 717.
void ComplexTypeDescriptor::Fuzz(void * lpDefinition, const TypeDescriptorField &, RNG & tRNG) const
{
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->Fuzz(lpSubDef, fieldChild, tRNG);
    }
}

// Original TypeExtensions.cpp line 736.
void ComplexTypeDescriptor::CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const
{
    XTypeDescriptor::CreateFieldHash(field, dwHash);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        
        fieldChild.GetTypeDesc()->CreateFieldHash(fieldChild, dwHash);
    }
}

// Original TypeExtensions.cpp line 758.
void VariantTypeDescriptor::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 764, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 807.
void VariantTypeDescriptor::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 814, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 900.
void VariantTypeDescriptor::IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const
{
    if (!ContainsSNOSubfield(field)) return;
    int32 nVariantValue = GetVariantValue(lpDefinition);
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->IterateSNOReferences(lpSubDef, fieldChild, pfuncIterate, pData);
    }
}

// Original TypeExtensions.cpp line 935.
void VariantTypeDescriptor::IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const
{
    pfuncIterate(this, &field, lpDefinition, pData);
    int32 nVariantValue = lpDefinition ? GetVariantValue(lpDefinition) : -1;
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue == -1 || nChildVariantValue == nVariantValue || !lpDefinition)
        {
            const void * lpSubDef = lpDefinition ? fieldChild.Offset(lpDefinition) : NULL;
            fieldChild.GetTypeDesc()->IterateDefinitionFields(lpSubDef, fieldChild, pfuncIterate, pData);
        }
    }

}

// Original TypeExtensions.cpp line 968.
void VariantTypeDescriptor::ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const
{
    if (!ContainsSNOSubfield(field)) return;
    int32 nVariantValue = GetVariantValue(lpDefinition);
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->ChangeSNOReference(lpSubDef, fieldChild, snonameOld, snoNew);
    }
}

// Original TypeExtensions.cpp line 1003.
void VariantTypeDescriptor::DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const
{
    if (!ContainsSNOSubfield(field)) return;
    int32 nVariantValue = GetVariantValue(lpDefinition);
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->DebugDump(lpSubDef, fieldChild, pDumpData);
    }
}

// Original TypeExtensions.cpp line 1037.
void VariantTypeDescriptor::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField &) const
{
    int32 nVariantValue = GetVariantValue(lpDefinitionSrc);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDefSrc = fieldChild.Offset(lpDefinitionSrc);
        void * lpSubDefDst = fieldChild.Offset(lpDefinitionDst);
        fieldChild.GetTypeDesc()->Byteswap(lpSubDefDst, lpSubDefSrc, lpDefinitionSrcBase, fieldChild);
    }
}

// Original TypeExtensions.cpp line 1067.
void VariantTypeDescriptor::ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField &) const
{
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->ByteswapInPlace(lpSubDef, lpDefinitionBase, fieldChild);
    }
}

// Original TypeExtensions.cpp line 1095.
void VariantTypeDescriptor::AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const
{
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->AdjustSerializeDataSize(lpSubDef, fieldChild, fWantFullDefinition);
    }
}

// Original TypeExtensions.cpp line 1124.
void VariantTypeDescriptor::ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const
{
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->ComputeOffsetsAndMerge(lpSubDef, fieldChild, dwOffset, tTypeSerializedDataList);
    }
}

// Original TypeExtensions.cpp line 1154.
void VariantTypeDescriptor::ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->ComputeOffsets(lpSubDef, fieldChild, dwOffset);
    }
}

// Original TypeExtensions.cpp line 1183.
void VariantTypeDescriptor::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        int32 nOffsetDelta = fieldChild.nDataOffset;
        fieldChild.GetTypeDesc()->SerializeToBinary(lpSubDef, fieldChild, lpDst, dwDstOffset + nOffsetDelta, dwTotalSize);
    }
}

// Original TypeExtensions.cpp line 1214.
void VariantTypeDescriptor::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{
    if (!ContainsAllocationSubfields(field)) return;
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = arFields[i];
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->SerializeFromBinary(lpSubDef, tUnserializeData, fieldChild);
    }
}

// Original TypeExtensions.cpp line 1251.
void VariantTypeDescriptor::Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const
{
    if (!ContainsAllocationSubfields(field)) return;
    int32 nVariantValue = GetVariantValue(lpDefinition);
    int32 nFieldIndex = 0;
    while (true)
    {
        const TypeDescriptorField & fieldChild = GetField(nFieldIndex++);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        const void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->Delete(dwDefinitionFlags, lpSubDef, fieldChild, ptMemoryInterface);
    }
}

// Original TypeExtensions.cpp line 1286.
BOOL VariantTypeDescriptor::Encode(void * lpDefinition, const TypeDescriptorField &, XEncoder & tEncoder) const
{
    size_t BitsUsed = tEncoder.Used();
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        if (!fieldChild.GetTypeDesc()->Encode(lpSubDef, fieldChild, tEncoder))
        {
            tEncoder.SetUsed(BitsUsed);
            return FALSE;
        }
    }
    return TRUE;
}

// Original TypeExtensions.cpp line 1320.
XDecoder::Result VariantTypeDescriptor::Decode(void * lpDefinition, const TypeDescriptorField &, XDecoder & tDecoder) const
{
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        XDecoder::Result eResult = fieldChild.GetTypeDesc()->Decode(lpSubDef, fieldChild, tDecoder);
        if (eResult != XDecoder::RESULT_VALID) return eResult;
    }
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 1352.
void VariantTypeDescriptor::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    int32 nVariantValue = GetVariantValue(lpDefinition);
    for (int32 i = 0; ; ++i)
    {
        const TypeDescriptorField & fieldChild = GetField(i);
        if (fieldChild.IsNull()) break;
        int32 nChildVariantValue = fieldChild.GetVariantTypeValue();
        if (nChildVariantValue != -1 && nChildVariantValue != nVariantValue) continue;
        void * lpSubDef = fieldChild.Offset(lpDefinition);
        fieldChild.GetTypeDesc()->Fuzz(lpSubDef, fieldChild, tRNG);
    }
}

// Original TypeExtensions.cpp line 1377.
inline int32 VariantTypeDescriptor::GetVariantValue(const void * lpDefinition) const
{
    int32 nVariantValue = *(const int32 *)((const uint8 *)lpDefinition + nVariantOffset);
    return nVariantValue;
}

// Original TypeExtensions.cpp line 1388.
void TypeDescriptorClass_DT_INT::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::INT32_STR_FORMAT, *(const int *)pData);
}

// Original TypeExtensions.cpp line 1407.
BOOL TypeDescriptorClass_DT_INT::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    const TypeDescriptorField &field = fieldSerializationInfo.GetField();
    if (field.dwFlags & 0x10000) {
        // A serialized-array count: the element count is the data size over the element size.
        int nSizeData = field.GetSerializeDataRaw(pData).dwSize;
        int32 nFullSizeOfElement = field.GetAuxTypeDesc()->SizeOf();
        *(int *)pData = nSizeData / nFullSizeOfElement;
    } else {
        *(int *)pData = atoi(sSerializeString);
    }
    return TRUE;
}

// Original TypeExtensions.cpp line 1440.
TagVariantType TypeDescriptorClass_DT_INT::GetTagVariantType() const
{
    return (int)0;
}

// Original TypeExtensions.cpp line 1451.
void TypeDescriptorClass_DT_INT::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 1465.
void TypeDescriptorClass_DT_INT::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 1477.
void TypeDescriptorClass_DT_INT::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    if (field.dwFlags & 0x10000) {
        DisplayInternalError(0, "", 1499, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    *(int *)(lpDst + dwDstOffset) = *(const int *)lpDefinition;
}

// Original TypeExtensions.cpp line 1510.
void TypeDescriptorClass_DT_INT::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 1522.
BOOL TypeDescriptorClass_DT_INT::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericIntEncode(lpDefinition, field, tEncoder);
}

// Original TypeExtensions.cpp line 1530.
XDecoder::Result TypeDescriptorClass_DT_INT::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericIntDecode(lpDefinition, field, tDecoder);
}

// Original TypeExtensions.cpp line 1538.
void TypeDescriptorClass_DT_INT::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericIntFuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 1558.
void TypeDescriptorClass_DT_DATAID::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::INT32_STR_FORMAT, *(const int *)pData);
}

// Original TypeExtensions.cpp line 1566.
BOOL TypeDescriptorClass_DT_DATAID::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    *(int *)pData = atoi(sSerializeString);
    return TRUE;
}

// Original TypeExtensions.cpp line 1575.
void TypeDescriptorClass_DT_DATAID::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(0, "", 1581, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 1593.
TagVariantType TypeDescriptorClass_DT_DATAID::GetTagVariantType() const
{
    return (int)0;
}

// Original TypeExtensions.cpp line 1604.
void TypeDescriptorClass_DT_DATAID::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 1618.
void TypeDescriptorClass_DT_DATAID::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 1630.
void TypeDescriptorClass_DT_DATAID::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original TypeExtensions.cpp line 1647.
void TypeDescriptorClass_DT_DATAID::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 1659.
BOOL TypeDescriptorClass_DT_DATAID::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    int32 nValToEncode = *(unsigned int *)lpDefinition;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        if ((int)nValToEncode < nMinVal || (int)nValToEncode > nMaxVal) {
            DisplayInternalError(0, "", 1678, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        nValToEncode -= nMinVal;
    }
    tEncoder.Put(nValToEncode, field.nBitCount);
    return 1;
}

// Original TypeExtensions.cpp line 1689.
XDecoder::Result TypeDescriptorClass_DT_DATAID::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    int32 nDecodedVal;
    XDecoder::Result eResult = tDecoder.Get((uint32 &)nDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        nDecodedVal += nMinVal;
        if (nDecodedVal < nMinVal || nDecodedVal > nMaxVal)
            return XDecoder::RESULT_CORRUPTED;
    }
    *(unsigned int *)lpDefinition = (unsigned int)nDecodedVal;
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 1718.
void TypeDescriptorClass_DT_DATAID::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericIntFuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 1729.
void TypeDescriptorClass_DT_INT64::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::INT64_STR_FORMAT, *(const long long *)pData);
}

// Original TypeExtensions.cpp line 1737.
BOOL TypeDescriptorClass_DT_INT64::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    return (int)1;
}

// Original TypeExtensions.cpp line 1750.
TagVariantType TypeDescriptorClass_DT_INT64::GetTagVariantType() const
{
    return (int)0;
}

// Original TypeExtensions.cpp line 1761.
void TypeDescriptorClass_DT_INT64::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned long long *)lpDefinitionDst = *(const unsigned long long *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 1775.
void TypeDescriptorClass_DT_INT64::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 1787.
void TypeDescriptorClass_DT_INT64::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned long long *)(lpDst + dwDstOffset) = *(const unsigned long long *)lpDefinition;
}

// Original TypeExtensions.cpp line 1804.
void TypeDescriptorClass_DT_INT64::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 1816.
BOOL TypeDescriptorClass_DT_INT64::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    int64 nValToEncode = *(const long *)lpDefinition;
    if (field.dwFlags & 0x10) {
        const long nMinVal = (long)field.vMinFieldValue;
        const long nMaxVal = (long)field.vMaxFieldValue;
        if (nValToEncode < nMinVal || nValToEncode > nMaxVal) {
            DisplayInternalError(0, "", 1835, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        nValToEncode -= nMinVal;
    }
    tEncoder.PutU64(nValToEncode, field.nBitCount);
    return 1;
}

// Original TypeExtensions.cpp line 1846.
XDecoder::Result TypeDescriptorClass_DT_INT64::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    int64 nDecodedVal;
    XDecoder::Result eResult = tDecoder.GetU64(*(unsigned long *)&nDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;
    if (field.dwFlags & 0x10) {
        const long nMinVal = (long)field.vMinFieldValue;
        const long nMaxVal = (long)field.vMaxFieldValue;
        nDecodedVal += nMinVal;
        if (nDecodedVal < nMinVal || nDecodedVal > nMaxVal)
            return XDecoder::RESULT_CORRUPTED;
    }
    *(long *)lpDefinition = nDecodedVal;
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 1875.
void TypeDescriptorClass_DT_INT64::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    if (field.dwFlags & (1 << TDF_MINMAX_BOUNDED)) {
        int64 nMinVal = (intptr)field.vMinFieldValue;
        int64 nMaxVal = (intptr)field.vMaxFieldValue;
        *(uint64 *)lpDefinition = RandRange_Internal(tRNG, nMinVal, nMaxVal);
    } else *(uint64 *)lpDefinition = RandGetDWord(tRNG);
}

// Original TypeExtensions.cpp line 1896.
void TypeDescriptorClass_DT_BYTE::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::INT32_STR_FORMAT, *(const unsigned char *)pData);
}

// Original TypeExtensions.cpp line 1904.
BOOL TypeDescriptorClass_DT_BYTE::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    *(unsigned char *)pData = atoi(sSerializeString);
    return TRUE;
}

// Original TypeExtensions.cpp line 1913.
BOOL TypeDescriptorClass_DT_BYTE::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    int32 nValToEncode = *(unsigned char *)lpDefinition;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        if ((int)nValToEncode < nMinVal || (int)nValToEncode > nMaxVal) {
            DisplayInternalError(0, "", 1932, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        nValToEncode -= nMinVal;
    }
    tEncoder.Put(nValToEncode, field.nBitCount);
    return 1;
}

// Original TypeExtensions.cpp line 1943.
XDecoder::Result TypeDescriptorClass_DT_BYTE::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    int32 nDecodedVal;
    XDecoder::Result eResult = tDecoder.Get((uint32 &)nDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        nDecodedVal += nMinVal;
        if (nDecodedVal < nMinVal || nDecodedVal > nMaxVal)
            return XDecoder::RESULT_CORRUPTED;
    }
    *(unsigned char *)lpDefinition = (unsigned char)nDecodedVal;
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 1972.
void TypeDescriptorClass_DT_BYTE::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    uint32 uValue = 0;
    GenericIntFuzz(&uValue, field, tRNG);
    *(unsigned char *)lpDefinition = (unsigned char)uValue;
}

// Original TypeExtensions.cpp line 1985.
void TypeDescriptorClass_DT_FLOAT::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, *(const float *)pData);
}

// Original TypeExtensions.cpp line 1993.
BOOL TypeDescriptorClass_DT_FLOAT::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    *(float *)pData = (float)atof(sSerializeString);
    return TRUE;
}

// Original TypeExtensions.cpp line 2002.
TagVariantType TypeDescriptorClass_DT_FLOAT::GetTagVariantType() const
{
    return (int)1;
}

// Original TypeExtensions.cpp line 2013.
void TypeDescriptorClass_DT_FLOAT::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 2027.
void TypeDescriptorClass_DT_FLOAT::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2039.
void TypeDescriptorClass_DT_FLOAT::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original TypeExtensions.cpp line 2056.
void TypeDescriptorClass_DT_FLOAT::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2068.
BOOL TypeDescriptorClass_DT_FLOAT::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericFloatEncode(lpDefinition, field, tEncoder);
}

// Original TypeExtensions.cpp line 2076.
XDecoder::Result TypeDescriptorClass_DT_FLOAT::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericFloatDecode(lpDefinition, field, tDecoder);
}

// Original TypeExtensions.cpp line 2084.
void TypeDescriptorClass_DT_FLOAT::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericFloatFuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 2095.
void TypeDescriptorClass_DT_STRING::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString = XMLStringConvertToXMLString(*(const CRefString *)pData);
}

// Original TypeExtensions.cpp line 2103.
BOOL TypeDescriptorClass_DT_STRING::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    *(CRefString *)pData = sSerializeString;
    return TRUE;
}

// Original TypeExtensions.cpp line 2112.
int TypeDescriptorClass_DT_STRING::Encode(void *, const TypeDescriptorField &, XEncoder &) const
{
    DisplayInternalError(0, "", 2117, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 2121.
XDecoder::Result TypeDescriptorClass_DT_STRING::Decode(void *, const TypeDescriptorField &, XDecoder &) const
{
    DisplayInternalError(0, "", 2126, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 2130.
void TypeDescriptorClass_DT_STRING::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    DisplayInternalError(0, "", 2135, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 2141.
void TypeDescriptorClass_DT_WORD::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::INT32_STR_FORMAT, *(const unsigned short *)pData);
}

// Original TypeExtensions.cpp line 2160.
BOOL TypeDescriptorClass_DT_WORD::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    *(unsigned short *)pData = atoi(sSerializeString);
    return TRUE;
}

// Original TypeExtensions.cpp line 2178.
void TypeDescriptorClass_DT_WORD::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned short *)lpDefinitionDst = *(const unsigned short *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 2192.
void TypeDescriptorClass_DT_WORD::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2204.
void TypeDescriptorClass_DT_WORD::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned short *)(lpDst + dwDstOffset) = *(const unsigned short *)lpDefinition;
}

// Original TypeExtensions.cpp line 2221.
void TypeDescriptorClass_DT_WORD::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2233.
BOOL TypeDescriptorClass_DT_WORD::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    int32 nValToEncode = *(unsigned short *)lpDefinition;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        if ((int)nValToEncode < nMinVal || (int)nValToEncode > nMaxVal) {
            DisplayInternalError(0, "", 2252, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        nValToEncode -= nMinVal;
    }
    tEncoder.Put(nValToEncode, field.nBitCount);
    return 1;
}

// Original TypeExtensions.cpp line 2263.
XDecoder::Result TypeDescriptorClass_DT_WORD::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    int32 nDecodedVal;
    XDecoder::Result eResult = tDecoder.Get((uint32 &)nDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        nDecodedVal += nMinVal;
        if (nDecodedVal < nMinVal || nDecodedVal > nMaxVal)
            return XDecoder::RESULT_CORRUPTED;
    }
    *(unsigned short *)lpDefinition = (unsigned short)nDecodedVal;
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 2292.
void TypeDescriptorClass_DT_WORD::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    uint32 uValue;
    GenericIntFuzz(&uValue, field, tRNG);
    *(unsigned short *)lpDefinition = (unsigned short)uValue;
}

// Original TypeExtensions.cpp line 2306.
void TypeDescriptorClass_DT_CHARARRAY::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString = XMLStringConvertToXMLString((const char *)pData);
}

// Original TypeExtensions.cpp line 2314.
BOOL TypeDescriptorClass_DT_CHARARRAY::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    strcpy((char *)pData, sSerializeString.str());
    return TRUE;
}

// Original TypeExtensions.cpp line 2323.
BOOL TypeDescriptorClass_DT_CHARARRAY::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    size_t nLength = strlen((const char *)lpDefinition);
    size_t nBitsRequired = Align8(field.nBitCount) + nLength * 8;
    if (tEncoder.Used() + nBitsRequired > tEncoder.Size()) return FALSE;
    tEncoder.Put(nLength, field.nBitCount);
    tEncoder.PutAsciiStringBytesAligned8((const uint8 *)lpDefinition, nLength);
    return TRUE;
}

// Original TypeExtensions.cpp line 2349.
XDecoder::Result TypeDescriptorClass_DT_CHARARRAY::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    uint32 dwLength;
    XDecoder::Result eResult = tDecoder.Get(dwLength, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID) return eResult;
    if ((int32)dwLength >= field.GetCount()) return XDecoder::RESULT_CORRUPTED;
    return tDecoder.GetAsciiStringBytesAligned8((uint8 *)lpDefinition, dwLength);
}

// Original TypeExtensions.cpp line 2377.
void TypeDescriptorClass_DT_CHARARRAY::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    uint32 dwLength = RandRange_Internal(tRNG, 0, field.GetCount());
    for (uint32 i = 0; i < dwLength; ++i) ((char *)lpDefinition)[i] = RandRange_Internal(tRNG, 0, 255);
}

// Original TypeExtensions.cpp line 2397.
void TypeDescriptorClass_DT_ANGLE::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, *(const float *)pData * 57.2957763671875f);
}

// Original TypeExtensions.cpp line 2405.
BOOL TypeDescriptorClass_DT_ANGLE::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    *(float *)pData = (float)atof(sSerializeString) * 0.01745329238474369f;
    return TRUE;
}

// Original TypeExtensions.cpp line 2414.
TagVariantType TypeDescriptorClass_DT_ANGLE::GetTagVariantType() const
{
    return (int)1;
}

// Original TypeExtensions.cpp line 2425.
void TypeDescriptorClass_DT_ANGLE::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 2439.
void TypeDescriptorClass_DT_ANGLE::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2451.
void TypeDescriptorClass_DT_ANGLE::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original TypeExtensions.cpp line 2468.
void TypeDescriptorClass_DT_ANGLE::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2480.
BOOL TypeDescriptorClass_DT_ANGLE::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericFloatEncode(lpDefinition, field, tEncoder);
}

// Original TypeExtensions.cpp line 2488.
XDecoder::Result TypeDescriptorClass_DT_ANGLE::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericFloatDecode(lpDefinition, field, tDecoder);
}

// Original TypeExtensions.cpp line 2496.
void TypeDescriptorClass_DT_ANGLE::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericFloatFuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 2508.
void TypeDescriptorClass_DT_PERCENT::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::INT32_STR_FORMAT, *(const unsigned char *)pData);
}

// Original TypeExtensions.cpp line 2516.
BOOL TypeDescriptorClass_DT_PERCENT::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    *(unsigned char *)pData = atoi(sSerializeString);
    return TRUE;
}

// Original TypeExtensions.cpp line 2525.
BOOL TypeDescriptorClass_DT_PERCENT::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    tEncoder.Put(*(unsigned char *)lpDefinition, field.nBitCount);
    return 1;
}

// Original TypeExtensions.cpp line 2542.
XDecoder::Result TypeDescriptorClass_DT_PERCENT::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return tDecoder.Get(*(unsigned int *)lpDefinition, field.nBitCount);
}

// Original TypeExtensions.cpp line 2550.
void TypeDescriptorClass_DT_PERCENT::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericIntFuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 2562.
void TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString.Format(SAX::FLOAT_STR_FORMAT, *(const float *)pData);
}

// Original TypeExtensions.cpp line 2570.
BOOL TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo &, const CRefString & sSerializeString, void * pData) const
{
    *(float *)pData = (float)atof(sSerializeString);
    return TRUE;
}

// Original TypeExtensions.cpp line 2579.
void TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 2593.
void TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2605.
void TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original TypeExtensions.cpp line 2622.
void TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2634.
BOOL TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    return GenericFloatEncode(lpDefinition, field, tEncoder);
}

// Original TypeExtensions.cpp line 2642.
XDecoder::Result TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    return GenericFloatDecode(lpDefinition, field, tDecoder);
}

// Original TypeExtensions.cpp line 2650.
void TypeDescriptorClass_DT_HIGHPRECISIONPERCENT::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericFloatFuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 2661.
void TypeDescriptorClass_DT_ENUM::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const
{
    sSerializeString = SymbolToName(*(const int *)pData, fieldSerializationInfo.GetField().pSymbolTable);
}

// Original TypeExtensions.cpp line 2670.
BOOL TypeDescriptorClass_DT_ENUM::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    const TypeDescriptorField &field = fieldSerializationInfo.GetField();
    *(int *)pData = NameToSymbol(sSerializeString, field.pSymbolTable);
    return TRUE;
}

// Original TypeExtensions.cpp line 2680.
void TypeDescriptorClass_DT_ENUM::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 2694.
void TypeDescriptorClass_DT_ENUM::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2706.
TagVariantType TypeDescriptorClass_DT_ENUM::GetTagVariantType() const
{
    return (int)0;
}

// Original TypeExtensions.cpp line 2717.
void TypeDescriptorClass_DT_ENUM::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original TypeExtensions.cpp line 2734.
void TypeDescriptorClass_DT_ENUM::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2746.
BOOL TypeDescriptorClass_DT_ENUM::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    if (tEncoder.Used() + field.nBitCount > tEncoder.Size())
        return 0;
    int32 nValToEncode = *(unsigned int *)lpDefinition;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        if ((int)nValToEncode < nMinVal || (int)nValToEncode > nMaxVal) {
            DisplayInternalError(0, "", 2765, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        nValToEncode -= nMinVal;
    }
    tEncoder.Put(nValToEncode, field.nBitCount);
    return 1;
}

// Original TypeExtensions.cpp line 2776.
XDecoder::Result TypeDescriptorClass_DT_ENUM::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    int32 nDecodedVal;
    XDecoder::Result eResult = tDecoder.Get((uint32 &)nDecodedVal, field.nBitCount);
    if (eResult != XDecoder::RESULT_VALID)
        return eResult;
    if (field.dwFlags & 0x10) {
        int nMinVal = (int)(long)field.vMinFieldValue;
        int nMaxVal = (int)(long)field.vMaxFieldValue;
        nDecodedVal += nMinVal;
        if (nDecodedVal < nMinVal || nDecodedVal > nMaxVal)
            return XDecoder::RESULT_CORRUPTED;
    }
    *(unsigned int *)lpDefinition = (unsigned int)nDecodedVal;
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 2805.
void TypeDescriptorClass_DT_ENUM::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    GenericIntFuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 2817.
void TypeDescriptorClass_DT_FLAG::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    LPCSTR szString = "0";
    if (*(const uint32 *)pData & (1 << field.nBit)) szString = "1";
    sSerializeString = szString;
}

// Original TypeExtensions.cpp line 2826.
BOOL TypeDescriptorClass_DT_FLAG::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    if (atoi(sSerializeString)) *(uint32 *)pData |= 1 << field.nBit;
    else *(uint32 *)pData &= ~(1 << field.nBit);
    return TRUE;
}

// Original TypeExtensions.cpp line 2836.
void TypeDescriptorClass_DT_FLAG::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField & field) const
{
    if (field.nBit == 0)
        *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 2854.
void TypeDescriptorClass_DT_FLAG::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField & field) const
{
}

// Original TypeExtensions.cpp line 2870.
void TypeDescriptorClass_DT_FLAG::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    if (field.nBit == 0)
        *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original TypeExtensions.cpp line 2891.
void TypeDescriptorClass_DT_FLAG::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField & field) const
{
}

// Original TypeExtensions.cpp line 2907.
BOOL TypeDescriptorClass_DT_FLAG::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    if (tEncoder.Used() == tEncoder.Size()) return FALSE;
    BOOL bValToEncode = (*(uint32 *)lpDefinition >> field.nBit) & 1;
    tEncoder.PutBool(bValToEncode);
    return TRUE;
}

// Original TypeExtensions.cpp line 2924.
XDecoder::Result TypeDescriptorClass_DT_FLAG::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    BOOL bVal;
    XDecoder::Result eResult = tDecoder.GetBool(bVal);
    if (eResult != XDecoder::RESULT_VALID) return eResult;
    {
        if (bVal) *(uint32 *)lpDefinition |= 1 << field.nBit;
        else *(uint32 *)lpDefinition &= ~(1 << field.nBit);
    }
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 2940.
void TypeDescriptorClass_DT_FLAG::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    *(unsigned int *)lpDefinition = RandGetDWord(tRNG) & 1;
}

// Original TypeExtensions.cpp line 2951.
void TypeDescriptorClass_DT_TRANSLATEABLE::ToXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData, CRefString & sSerializeString) const
{
    const TypeDescriptorField & field = fieldSerializationInfo.GetField();
    sSerializeString = field.pfnIntToString(*(const int32 *)pData);
}

// Original TypeExtensions.cpp line 2960.
BOOL TypeDescriptorClass_DT_TRANSLATEABLE::FromXMLSerializedString(const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CRefString & sSerializeString, void * pData) const
{
    TranslateStringToIntFunc pfnStringToInt = fieldSerializationInfo.GetField().pfnStringToInt;
    *(int *)pData = pfnStringToInt(sSerializeString);
    return TRUE;
}

// Original TypeExtensions.cpp line 2970.
void TypeDescriptorClass_DT_TRANSLATEABLE::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void *, const TypeDescriptorField &) const
{
    *(unsigned int *)lpDefinitionDst = *(const unsigned int *)lpDefinitionSrc;
}

// Original TypeExtensions.cpp line 2984.
void TypeDescriptorClass_DT_TRANSLATEABLE::ByteswapInPlace(void * lpDefinition, void *, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 2996.
void TypeDescriptorClass_DT_TRANSLATEABLE::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    *(unsigned int *)(lpDst + dwDstOffset) = *(const unsigned int *)lpDefinition;
}

// Original TypeExtensions.cpp line 3013.
void TypeDescriptorClass_DT_TRANSLATEABLE::SerializeFromBinary(void * lpDefinition, const UnserializeData &, const TypeDescriptorField &) const
{
}

// Original TypeExtensions.cpp line 3028.
void TypeDescriptorClass_DT_FIXEDARRAY::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 3034, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 3058.
void TypeDescriptorClass_DT_FIXEDARRAY::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 3065, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 3106.
void TypeDescriptorClass_DT_FIXEDARRAY::IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const
{
    if (!ContainsSNOSubfield(field)) return;
    int32 nCount = field.GetCount();
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->IterateSNOReferences(lpDefChild, auxField, pfuncIterate, pData);
    }
}

// Original TypeExtensions.cpp line 3136.
void TypeDescriptorClass_DT_FIXEDARRAY::IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const
{
    pfuncIterate(this, &field, lpDefinition, pData);
    int32 nCount = lpDefinition ? field.GetCount() : 1;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = lpDefinition ? (uint8 *)lpDefinition + (int64)i * nSize : NULL;
        ptAuxTypeDesc->IterateDefinitionFields(lpDefChild, auxField, pfuncIterate, pData);
    }
}

// Original TypeExtensions.cpp line 3164.
void TypeDescriptorClass_DT_FIXEDARRAY::ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const
{
    if (!ContainsSNOSubfield(field)) return;
    int32 nCount = field.GetCount();
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->ChangeSNOReference(lpDefChild, auxField, snonameOld, snoNew);
    }
}

// Original TypeExtensions.cpp line 3194.
void TypeDescriptorClass_DT_FIXEDARRAY::DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const
{
    if (!ContainsSNOSubfield(field)) return;
    int32 nCount = field.GetCount();
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->DebugDump(lpDefChild, auxField, pDumpData);
    }
}

// Original TypeExtensions.cpp line 3223.
void TypeDescriptorClass_DT_FIXEDARRAY::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 size = ptAuxTypeDesc->SizeOf();
    while (count--) {
        ptAuxTypeDesc->Byteswap(lpDefinitionDst, lpDefinitionSrc, lpDefinitionSrcBase, field);
        lpDefinitionDst = (uint8 *)lpDefinitionDst + size;
        lpDefinitionSrc = (uint8 *)lpDefinitionSrc + size;
    }
}

// Original TypeExtensions.cpp line 3244.
void TypeDescriptorClass_DT_FIXEDARRAY::ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 size = ptAuxTypeDesc->SizeOf();
    while (count--) {
        ptAuxTypeDesc->ByteswapInPlace(lpDefinition, lpDefinitionBase, field);
        lpDefinition = (uint8 *)lpDefinition + size;
    }
}

// Original TypeExtensions.cpp line 3263.
void TypeDescriptorClass_DT_FIXEDARRAY::AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < count; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->AdjustSerializeDataSize(lpdefChild, fieldChild, fWantFullDefinition);
    }
}

// Original TypeExtensions.cpp line 3284.
void TypeDescriptorClass_DT_FIXEDARRAY::ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < count; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->ComputeOffsetsAndMerge(lpdefChild, fieldChild, dwOffset, tTypeSerializedDataList);
    }
}

// Original TypeExtensions.cpp line 3306.
void TypeDescriptorClass_DT_FIXEDARRAY::ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < count; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->ComputeOffsets(lpdefChild, fieldChild, dwOffset);
    }
}

// Original TypeExtensions.cpp line 3327.
void TypeDescriptorClass_DT_FIXEDARRAY::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 nFullSize = ptAuxTypeDesc->SizeOf();
    int32 nPlatformSize = nFullSize;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < count; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nFullSize;
        ptAuxTypeDesc->SerializeToBinary(lpdefChild, fieldChild, lpDst, dwDstOffset + i * nPlatformSize, dwTotalSize);
    }
}

// Original TypeExtensions.cpp line 3356.
void TypeDescriptorClass_DT_FIXEDARRAY::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);
    if (!ptAuxTypeDesc->ContainsAllocationSubfields(field)) return;
    int32 nSize = ptAuxTypeDesc->SizeOf();
    int32 nCount = field.GetCount();
    for (int32 i = 0; i < nCount; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->SerializeFromBinary(lpdefChild, tUnserializeData, fieldAux);
    }
}

// Original TypeExtensions.cpp line 3386.
void TypeDescriptorClass_DT_FIXEDARRAY::Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const
{
    if (!ContainsAllocationSubfields(field)) return;
    int32 nCount = field.GetCount();
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->Delete(dwDefinitionFlags, lpDefChild, auxField, ptMemoryInterface);
    }
}

// Original TypeExtensions.cpp line 3416.
BOOL TypeDescriptorClass_DT_FIXEDARRAY::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    size_t BitsUsed = tEncoder.Used();
    if (field.dwFlags & (1 << TDF_VARIABLE_COUNT)) {
        if (tEncoder.Size() - BitsUsed < (size_t)field.nArraySizeBitCount) { tEncoder.SetUsed(BitsUsed); return FALSE; }
        int32 nUsedElements = *(int32 *)((uint8 *)lpDefinition + field.nUsedElementsOffset);
        tEncoder.Put(nUsedElements, field.nArraySizeBitCount);
        count = nUsedElements;
    }
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < count; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        if (!ptAuxTypeDesc->Encode(lpdefChild, field, tEncoder)) { tEncoder.SetUsed(BitsUsed); return FALSE; }
    }
    return TRUE;
}

// Original TypeExtensions.cpp line 3462.
XDecoder::Result TypeDescriptorClass_DT_FIXEDARRAY::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    if (field.dwFlags & (1 << TDF_VARIABLE_COUNT)) {
        uint32 dwCount;
        XDecoder::Result eResult = tDecoder.Get(dwCount, field.nArraySizeBitCount);
        if (eResult != XDecoder::RESULT_VALID) return eResult;
        if ((int32)dwCount > count) return XDecoder::RESULT_CORRUPTED;
        int32 *pnUsedElements = (int32 *)((uint8 *)lpDefinition + field.nUsedElementsOffset);
        *pnUsedElements = count = dwCount;
    }
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < count; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        XDecoder::Result eResult = ptAuxTypeDesc->Decode(lpdefChild, field, tDecoder);
        if (eResult != XDecoder::RESULT_VALID) return eResult;
    }
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 3511.
void TypeDescriptorClass_DT_FIXEDARRAY::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 count = field.GetCount();
    int32 nSize = ptAuxTypeDesc->SizeOf();
    if (field.dwFlags & (1 << TDF_VARIABLE_COUNT)) {
        count = Rand_Internal(tRNG, count);
        int32 *pnUsedElements = (int32 *)((uint8 *)lpDefinition + field.nUsedElementsOffset);
        *pnUsedElements = count;
    }
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < count; ++i) {
        void *lpdefChild = (uint8 *)lpDefinition + (int64)i * nSize;
        ptAuxTypeDesc->Fuzz(lpdefChild, field, tRNG);
    }
}

// Original TypeExtensions.cpp line 3539.
void TypeDescriptorClass_DT_FIXEDARRAY::CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const
{
    char szTempString[256];
    Blizzard::String::Format(szTempString, sizeof(szTempString), "%s[%d]", GetName(), field.GetCount());
    dwHash = Blizzard::Util::CRC32(szTempString, strlen(szTempString), dwHash);
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    ptAuxTypeDesc->CreateFieldHash(field, dwHash);
}

// Original TypeExtensions.cpp line 3558.
void TypeDescriptorClass_DT_VARIABLEARRAY::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 3564, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 3613.
void TypeDescriptorClass_DT_VARIABLEARRAY::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 3620, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 3694.
inline void TypeDescriptorClass_DT_VARIABLEARRAY::sGetArrayCountAndPtr(const void * lpDefinition, const TypeDescriptorField & field, const XTypeDescriptor * ptAuxTypeDesc, int32 nAuxTypeSize, int32 & nCount, void *& pArray)
{
    const SerializeData &ser = field.GetSerializeDataRaw(lpDefinition);
    if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY)) {
        const XGrowableArray<int, int> *ar = (const XGrowableArray<int, int> *)lpDefinition;
        nCount = ar->GetArraySize();
        pArray = (void *)ar->GetPointer();
    } else {
        if (field.dwFlags & ((1 << TDF_XPTR64) | (1 << TDF_XPTR128))) {
            const XPtr64<uint8> &rpArray64 = *(const XPtr64<uint8> *)lpDefinition;
            pArray = rpArray64;
        } else {
            pArray = *(void **)lpDefinition;
        }
        nCount = pArray ? ser.dwSize / nAuxTypeSize : 0;
    }
}

// Original TypeExtensions.cpp line 3737.
void TypeDescriptorClass_DT_VARIABLEARRAY::IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const
{
    if (!ContainsSNOSubfield(field)) return;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 nAuxTypeSize = ptAuxTypeDesc->SizeOf();
    int32 nCount;
    void *pArray;
    sGetArrayCountAndPtr(lpDefinition, field, ptAuxTypeDesc, nAuxTypeSize, nCount, pArray);
    if (!nCount) return;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = (uint8 *)pArray + (int64)i * nAuxTypeSize;
        ptAuxTypeDesc->IterateSNOReferences(lpDefChild, auxField, pfuncIterate, pData);
    }
}

// Original TypeExtensions.cpp line 3778.
void TypeDescriptorClass_DT_VARIABLEARRAY::IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const
{
    pfuncIterate(this, &field, lpDefinition, pData);
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 nAuxTypeSize = ptAuxTypeDesc->SizeOf();
    int32 nCount;
    void *pArray;
    if (!lpDefinition) { nCount = 1; pArray = NULL; }
    else sGetArrayCountAndPtr(lpDefinition, field, ptAuxTypeDesc, nAuxTypeSize, nCount, pArray);
    if (!nCount) return;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = lpDefinition ? (uint8 *)pArray + (int64)i * nAuxTypeSize : NULL;
        ptAuxTypeDesc->IterateDefinitionFields(lpDefChild, auxField, pfuncIterate, pData);
    }
}

// Original TypeExtensions.cpp line 3820.
void TypeDescriptorClass_DT_VARIABLEARRAY::ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const
{
    if (!ContainsSNOSubfield(field)) return;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 nAuxTypeSize = ptAuxTypeDesc->SizeOf();
    int32 nCount;
    void *pArray;
    sGetArrayCountAndPtr(lpDefinition, field, ptAuxTypeDesc, nAuxTypeSize, nCount, pArray);
    if (!nCount) return;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = (uint8 *)pArray + (int64)i * nAuxTypeSize;
        ptAuxTypeDesc->ChangeSNOReference(lpDefChild, auxField, snonameOld, snoNew);
    }
}

// Original TypeExtensions.cpp line 3861.
void TypeDescriptorClass_DT_VARIABLEARRAY::DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const
{
    if (!ContainsSNOSubfield(field)) return;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 nAuxTypeSize = ptAuxTypeDesc->SizeOf();
    int32 nCount;
    void *pArray;
    sGetArrayCountAndPtr(lpDefinition, field, ptAuxTypeDesc, nAuxTypeSize, nCount, pArray);
    if (!nCount) return;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpDefChild = (uint8 *)pArray + (int64)i * nAuxTypeSize;
        ptAuxTypeDesc->DebugDump(lpDefChild, auxField, pDumpData);
    }
}

// Original TypeExtensions.cpp line 3901.
void TypeDescriptorClass_DT_VARIABLEARRAY::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 size = ptAuxTypeDesc->SizeOf();
    SerializeData &ser = field.GetSerializeData(lpDefinitionSrc);
    int32 count = ser.dwSize / size;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    uint8 *pArrayIteratorDst;
    uint8 *pArrayIteratorSrc;
    for (pArrayIteratorSrc = *(uint8 **)lpDefinitionSrc, pArrayIteratorDst = *(uint8 **)lpDefinitionDst; count--; pArrayIteratorSrc += size, pArrayIteratorDst += size) {
        ptAuxTypeDesc->Byteswap(pArrayIteratorDst, pArrayIteratorSrc, lpDefinitionSrcBase, fieldChild);
    }
}

// Original TypeExtensions.cpp line 3945.
void TypeDescriptorClass_DT_VARIABLEARRAY::ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 size = ptAuxTypeDesc->SizeOf();
    SerializeData &ser = field.GetSerializeData(lpDefinition);
    int32 count = ser.dwSize / size;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    uint8 *pArrayIterator = *(uint8 **)lpDefinition;
    while (count--) {
        ptAuxTypeDesc->ByteswapInPlace(pArrayIterator, lpDefinitionBase, fieldChild);
        pArrayIterator += size;
    }
}

// Original TypeExtensions.cpp line 3986.
void TypeDescriptorClass_DT_VARIABLEARRAY::AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const
{
    if (s_nServerOnlyFieldCount > 0 && !SigmaPlatformIsConsole(SigmaPlatformGet())) return;
    SerializeData &ser = field.GetSerializeDataRaw(lpDefinition);
    ser.dwOffset = 0;
    uint8 *pArrayBase = *(uint8 **)lpDefinition;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 nFullSize = ptAuxTypeDesc->SizeOf();
    int nCount;
    if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY)) {
        XGrowableArray<int, int> *ar = (XGrowableArray<int, int> *)lpDefinition;
        nCount = ar->GetArraySize();
        ser.dwSize = nCount * nFullSize;
    } else {
        nCount = ser.dwSize / (nFullSize > 1 ? nFullSize : 1);
    }
    if (!ser.dwSize) return;
    if (ptAuxTypeDesc == DT_NULL) return;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpdefChild = pArrayBase + (int64)i * nFullSize;
        ptAuxTypeDesc->AdjustSerializeDataSize(lpdefChild, fieldChild, fWantFullDefinition);
    }
}

// Original TypeExtensions.cpp line 4068.
void TypeDescriptorClass_DT_VARIABLEARRAY::ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const
{
    if (s_nServerOnlyFieldCount > 0 && !SigmaPlatformIsConsole(SigmaPlatformGet())) return;
    SerializeData &ser = field.GetSerializeDataRaw(lpDefinition);
    ser.dwOffset = 0;
    uint8 *pArrayBase = *(uint8 **)lpDefinition;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 nFullSize = ptAuxTypeDesc->SizeOf();
    int nCount;
    if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY)) {
        XGrowableArray<int, int> *ar = (XGrowableArray<int, int> *)lpDefinition;
        nCount = ar->GetArraySize();
        ser.dwSize = nCount * nFullSize;
    } else {
        nCount = ser.dwSize / nFullSize;
    }
    if (!ser.dwSize) return;
    if (field.dwFlags & ((1 << TDF_DISABLE_DATA_MERGE) | (1 << TDF_XGROWABLE_ARRAY))) {
    uint32 uByteAlignment = (field.dwFlags & (1 << TDF_XPTR128)) ? 16 : (field.dwFlags & (1 << TDF_XPTR64)) ? 8 : 4;
    FileSerialize(ser, dwOffset, uByteAlignment);
    }
    if (ptAuxTypeDesc == DT_NULL) return;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpdefChild = pArrayBase + (int64)i * nFullSize;
        ptAuxTypeDesc->ComputeOffsetsAndMerge(lpdefChild, fieldChild, dwOffset, tTypeSerializedDataList);
    }
}

// Original TypeExtensions.cpp line 4186.
void TypeDescriptorClass_DT_VARIABLEARRAY::ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    if (s_nServerOnlyFieldCount > 0 && !SigmaPlatformIsConsole(SigmaPlatformGet())) return;
    SerializeData &ser = field.GetSerializeDataRaw(lpDefinition);
    ser.dwOffset = 0;
    uint8 *pArrayBase = *(uint8 **)lpDefinition;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    int32 nFullSize = ptAuxTypeDesc->SizeOf();
    int nCount;
    if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY)) {
        XGrowableArray<int, int> *ar = (XGrowableArray<int, int> *)lpDefinition;
        nCount = ar->GetArraySize();
        ser.dwSize = nCount * nFullSize;
    } else {
        nCount = ser.dwSize / nFullSize;
    }
    if (!ser.dwSize) return;
    uint32 uByteAlignment = (field.dwFlags & (1 << TDF_XPTR128)) ? 16 : (field.dwFlags & (1 << TDF_XPTR64)) ? 8 : 4;
    FileSerialize(ser, dwOffset, uByteAlignment);
    if (ptAuxTypeDesc == DT_NULL) return;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpdefChild = pArrayBase + (int64)i * nFullSize;
        ptAuxTypeDesc->ComputeOffsets(lpdefChild, fieldChild, dwOffset);
    }
}

// Original TypeExtensions.cpp line 4264.
void TypeDescriptorClass_DT_VARIABLEARRAY::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    if (s_nServerOnlyFieldCount > 0 && !SigmaPlatformIsConsole(SigmaPlatformGet())) return;
    SerializeData &ser = field.GetSerializeData(lpDefinition);
    if (!ser.dwSize) return;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    if (ptAuxTypeDesc == DT_NULL) return;
    uint8 *pArrayBase = *(uint8 **)lpDefinition;
    int32 nFullSize = ptAuxTypeDesc->SizeOf();
    int32 nPlatformSize = nFullSize;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    int32 nCount = ser.dwSize / nFullSize;
    for (int32 i = 0; i < nCount; ++i) {
        void *lpdefChild = pArrayBase + (int64)i * nFullSize;
        ptAuxTypeDesc->SerializeToBinary(lpdefChild, fieldChild, lpDst, ser.dwOffset + i * nPlatformSize, dwTotalSize);
    }
}

// Original TypeExtensions.cpp line 4326.
// NON_MATCHING: same size; the initial serialized-size and pointer-flag loads are scheduled in the opposite order.
#line 4326 "D:/BuildServer/2/work/code/branches/2_6_2/X/Sigma/TypeDescriptor/TypeExtensions.cpp"
void TypeDescriptorClass_DT_VARIABLEARRAY::SerializeFromBinary(
	void * lpDefinition,
	const UnserializeData & tUnserializeData,
	const TypeDescriptorField & field) const
{




	if (!ContainsAllocationSubfields(field))
		return;





	SerializeData & ser = field.GetSerializeData(lpDefinition);


	void *& pArray = *(void **)lpDefinition;
	XPtr64<uint8> & rpArray64 = *(XPtr64<uint8> *)lpDefinition;











	if (s_nServerOnlyFieldCount > 0 && !SigmaPlatformIsConsole(geSigmaPlatform))
		return;



	const XTypeDescriptor * ptAuxTypeDesc = field.GetAuxTypeDesc();
	if (!ser.dwSize)
	{


		if (field.dwFlags & ((1 << TDF_XPTR64) | (1 << TDF_XPTR128)))
			rpArray64 = NULL;
		else
			pArray = NULL;





		if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY))
		{

			XGrowableArray<int, int> & ar = *(XGrowableArray<int, int> *)lpDefinition;
			ar.InitializeFromSerializeBlock(NULL, ser.dwSize, 1, NULL);
		}

		return;
	}


	if (field.dwFlags & (1 << TDF_XPTR128))

		pArray = sLoadSerializedTracked(tUnserializeData, ser, 16, "D:/BuildServer/2/work/code/branches/2_6_2/X/Sigma/TypeDescriptor/TypeExtensions.cpp", 4390, field.szFieldName);

	else if (field.dwFlags & (1 << TDF_XPTR64))

		pArray = sLoadSerializedTracked(tUnserializeData, ser, 8, "D:/BuildServer/2/work/code/branches/2_6_2/X/Sigma/TypeDescriptor/TypeExtensions.cpp", 4394, field.szFieldName);

	else

		pArray = sLoadSerializedTracked(tUnserializeData, ser, 4, "D:/BuildServer/2/work/code/branches/2_6_2/X/Sigma/TypeDescriptor/TypeExtensions.cpp", 4398, field.szFieldName);


	TypeDescriptorField fieldAux(ptAuxTypeDesc, NULL);




















	uint8 * pArrayBase = (field.dwFlags & (1 << TDF_XPTR64)) ? (uint8 *)rpArray64 : (uint8 *)pArray;

	if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY))
	{

		uint32 dwAuxTypeSize = ptAuxTypeDesc->SizeOf();
		XGrowableArray<int, int> & ar = *(XGrowableArray<int, int> *)lpDefinition;
		ar.InitializeFromSerializeBlock(pArrayBase, ser.dwSize, dwAuxTypeSize, tUnserializeData.pMemoryInterface);
	}


	if (!(field.dwFlags & (1 << TDF_DISABLE_DATA_MERGE)) && tUnserializeData.ptMergedOffsetMap)
	{
		uintptr dwDummy;
		if (tUnserializeData.ptMergedOffsetMap->Lookup(ser.dwOffset, dwDummy))
			return;

		tUnserializeData.ptMergedOffsetMap->SetAt_Tracked(ser.dwOffset, 0);
	}


	if (ptAuxTypeDesc != DT_NULL)
	{
		uint32 dwAuxTypeSize = ptAuxTypeDesc->SizeOf();
		int32 nCount = ser.dwSize / dwAuxTypeSize;


		if (
			ptAuxTypeDesc->ContainsAllocationSubfields(fieldAux))
		{




			for (int32 i = 0; i < nCount; ++i)
			{
				void * lpdefChild = pArrayBase + i * dwAuxTypeSize;
				ptAuxTypeDesc->SerializeFromBinary(lpdefChild, tUnserializeData, fieldAux);
			}
		}
	}
}
#line 2020 "D:/BuildServer/2/work/code/branches/2_6_2/X/Sigma/TypeDescriptor/TypeExtensions.cpp"

// Original TypeExtensions.cpp line 4465.
void TypeDescriptorClass_DT_VARIABLEARRAY::Delete(const uint32 dwDefinitionFlags, const void * lpDefinition, const TypeDescriptorField & field, XMemoryAllocatorInterface * ptMemoryInterface) const
{
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor * ptAuxTypeDesc = auxField.GetTypeDesc();
    if (ptAuxTypeDesc->ContainsAllocationSubfields(auxField)) {
        int32 nAuxTypeSize = ptAuxTypeDesc->SizeOf();
        int32 nCount;
        void * pArray;
        sGetArrayCountAndPtr(lpDefinition, field, ptAuxTypeDesc, nAuxTypeSize, nCount, pArray);
        if (nCount > 0 && pArray) {
            for (int32 i = 0; i < nCount; ++i) {
                void * lpDefChild = (uint8 *)pArray + i * nAuxTypeSize;
                ptAuxTypeDesc->Delete(dwDefinitionFlags, lpDefChild, auxField, ptMemoryInterface);
            }
        }
    }
    if (!(dwDefinitionFlags & (1 << DH_IN_ONE_BIG_CONTIGUOUS_BLOCK))) {
        if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY)) {
            XGrowableArray<int, int> & growableArray = *(XGrowableArray<int, int> *)lpDefinition;
            growableArray.SafeDispose();
        } else {
            if (field.dwFlags & ((1 << TDF_XPTR64) | (1 << TDF_XPTR128))) {
                XPtr64<uint8> & pArrayToDelete = *(XPtr64<uint8> *)lpDefinition;
                SigmaMemoryFree(pArrayToDelete, ptMemoryInterface);
                pArrayToDelete = NULL;
            } else {
                void *& pArrayToDelete = *(void **)lpDefinition;
                SigmaMemoryFree(pArrayToDelete, ptMemoryInterface);
                pArrayToDelete = NULL;
            }
        }
    }
}

// Original TypeExtensions.cpp line 4535.
BOOL TypeDescriptorClass_DT_VARIABLEARRAY::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    SerializeData &ser = field.GetSerializeDataRaw(lpDefinition);
    uint8 *pArrayBase = *(uint8 **)lpDefinition;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    if (field.dwFlags & (1 << TDF_XGROWABLE_ARRAY)) {
        XGrowableArray<int, int> *ar = (XGrowableArray<int, int> *)lpDefinition;
        int32 nElements = ar->GetArraySize();
        ser.dwSize = nElements * ptAuxTypeDesc->SizeOf();
    }
    size_t BitsUsed = tEncoder.Used();
    if (BitsUsed + 8 > tEncoder.Size()) return FALSE;
    int32 nSize = ptAuxTypeDesc->SizeOf();
    int32 nCount = ser.dwSize / nSize;
    tEncoder.Put(nCount, 8);
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (int32 i = 0; i < nCount; ++i) {
        void *lpdefChild = pArrayBase + (int64)i * nSize;
        if (!ptAuxTypeDesc->Encode(lpdefChild, field, tEncoder)) { tEncoder.SetUsed(BitsUsed); return FALSE; }
    }
    return TRUE;
}

// Original TypeExtensions.cpp line 4580.
XDecoder::Result TypeDescriptorClass_DT_VARIABLEARRAY::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    uint8 *pArrayBase = *(uint8 **)lpDefinition;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    uint32 dwCount;
    XDecoder::Result eResult = tDecoder.Get(dwCount, 8);
    if (eResult != XDecoder::RESULT_VALID) return eResult;
    SerializeData &ser = field.GetSerializeDataRaw(lpDefinition);
    int32 nSize = ptAuxTypeDesc->SizeOf();
    ser.dwSize = dwCount * nSize;
    TypeDescriptorField fieldChild(ptAuxTypeDesc, NULL);
    for (uint32 i = 0; i < dwCount; ++i) {
        void *lpdefChild = pArrayBase + i * nSize;
        XDecoder::Result eResult = ptAuxTypeDesc->Decode(lpdefChild, field, tDecoder);
        if (eResult != XDecoder::RESULT_VALID) return eResult;
    }
    return XDecoder::RESULT_VALID;
}

// Original TypeExtensions.cpp line 4682.
void TypeDescriptorClass_DT_VARIABLEARRAY::LoadFromDefaultData(const TypeDescriptorField & tField, void * pData) const
{
    uint32 &dwTotalSize = tField.GetSerializeDataRaw(pData).dwSize;
    uint32 &dwOffset = tField.GetSerializeDataRaw(pData).dwOffset;
    dwOffset = 0;
    dwTotalSize = 0;
    if (tField.dwFlags & (1 << TDF_XPTR64)) {
        XPtr64<uint8> &rpArray64 = *(XPtr64<uint8> *)pData;
        rpArray64 = NULL;
    } else {
        void *&pArray = *(void **)pData;
        pArray = NULL;
    }
    if (tField.dwFlags & (1 << TDF_XGROWABLE_ARRAY)) {
        XGrowableArray<int, int> *ar = (XGrowableArray<int, int> *)pData;
        ar->InitializeFromSerializeBlock(NULL, 0, 1, NULL);
    }
}

// Original TypeExtensions.cpp line 4718.
void TypeDescriptorClass_DT_CSTRING::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(0, "", 4724, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 4757.
void TypeDescriptorClass_DT_CSTRING::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL &) const
{
    DisplayInternalError(0, "", 4764, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 4807.
void TypeDescriptorClass_DT_OPTIONAL::SerializeToXML(SAX & sax, const TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const void * pData) const
{
    DisplayInternalError(NULL, "", 4813, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 4838.
void TypeDescriptorClass_DT_OPTIONAL::SerializeFromXML(TypeDescriptorFieldSerializationInfo & fieldSerializationInfo, const CXMLNode & node, void * pData, BOOL & fContainsBadLink) const
{
    DisplayInternalError(NULL, "", 4845, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

// Original TypeExtensions.cpp line 4864.
void TypeDescriptorClass_DT_OPTIONAL::IterateSNOReferences(const void * lpDefinition, const TypeDescriptorField & field, SNOIterateFunc pfuncIterate, void * pData) const
{
    if (!ContainsSNOSubfield(field)) return;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    void *lpDefChild = (void *)lpDefinition;
    ptAuxTypeDesc->IterateSNOReferences(lpDefChild, auxField, pfuncIterate, pData);
}

// Original TypeExtensions.cpp line 4886.
void TypeDescriptorClass_DT_OPTIONAL::IterateDefinitionFields(const void * lpDefinition, const TypeDescriptorField & field, FieldIterateFunc pfuncIterate, void * pData) const
{
    pfuncIterate(this, &field, lpDefinition, pData);
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    void *lpDefChild = (void *)lpDefinition;
    ptAuxTypeDesc->IterateDefinitionFields(lpDefChild, auxField, pfuncIterate, pData);
}

// Original TypeExtensions.cpp line 4906.
void TypeDescriptorClass_DT_OPTIONAL::ChangeSNOReference(const void * lpDefinition, const TypeDescriptorField & field, const SNOName & snonameOld, const SNO snoNew) const
{
    if (!ContainsSNOSubfield(field)) return;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    void *lpDefChild = (void *)lpDefinition;
    ptAuxTypeDesc->ChangeSNOReference(lpDefChild, auxField, snonameOld, snoNew);
}

// Original TypeExtensions.cpp line 4928.
void TypeDescriptorClass_DT_OPTIONAL::DebugDump(const void * lpDefinition, const TypeDescriptorField & field, DebugDumpData * pDumpData) const
{
    if (!ContainsSNOSubfield(field)) return;
    TypeDescriptorField auxField;
    auxField.MakeAuxiliary(field);
    const XTypeDescriptor *ptAuxTypeDesc = auxField.GetTypeDesc();
    void *lpDefChild = (void *)lpDefinition;
    ptAuxTypeDesc->DebugDump(lpDefChild, auxField, pDumpData);
}

// Original TypeExtensions.cpp line 4949.
void TypeDescriptorClass_DT_OPTIONAL::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc, void * lpDefinitionSrcBase, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    BOOL *pbExistsDst = (BOOL *)((uint8 *)lpDefinitionDst + ptAuxTypeDesc->SizeOf());
    BOOL *pbExistsSrc = (BOOL *)((uint8 *)lpDefinitionSrc + ptAuxTypeDesc->SizeOf());
    *pbExistsDst = *pbExistsSrc;
    ptAuxTypeDesc->Byteswap(lpDefinitionDst, lpDefinitionSrc, lpDefinitionSrcBase, field);
}

// Original TypeExtensions.cpp line 4971.
void TypeDescriptorClass_DT_OPTIONAL::ByteswapInPlace(void * lpDefinition, void * lpDefinitionBase, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    BOOL *pbExists = (BOOL *)((uint8 *)lpDefinition + ptAuxTypeDesc->SizeOf());
    ptAuxTypeDesc->ByteswapInPlace(lpDefinition, lpDefinitionBase, field);
}

// Original TypeExtensions.cpp line 4989.
void TypeDescriptorClass_DT_OPTIONAL::AdjustSerializeDataSize(void * lpDefinition, const TypeDescriptorField & field, const BOOL fWantFullDefinition) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    void *lpDefAux = lpDefinition;
    ptAuxTypeDesc->AdjustSerializeDataSize(lpDefAux, field, fWantFullDefinition);
}

// Original TypeExtensions.cpp line 5003.
void TypeDescriptorClass_DT_OPTIONAL::ComputeOffsetsAndMerge(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset, MergeableSerializeDataInfoList & tTypeSerializedDataList) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    void *lpDefAux = lpDefinition;
    ptAuxTypeDesc->ComputeOffsetsAndMerge(lpDefAux, field, dwOffset, tTypeSerializedDataList);
}

// Original TypeExtensions.cpp line 5018.
void TypeDescriptorClass_DT_OPTIONAL::ComputeOffsets(void * lpDefinition, const TypeDescriptorField & field, uint32 & dwOffset) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    void *lpDefAux = lpDefinition;
    ptAuxTypeDesc->ComputeOffsets(lpDefAux, field, dwOffset);
}

// Original TypeExtensions.cpp line 5032.
void TypeDescriptorClass_DT_OPTIONAL::SerializeToBinary(void * lpDefinition, const TypeDescriptorField & field, uint8 * lpDst, const uint32 dwDstOffset, const uint32 dwTotalSize) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    BOOL *pDst = (BOOL *)(lpDst + ptAuxTypeDesc->SizeOf());
    BOOL *pSrc = (BOOL *)((uint8 *)lpDefinition + ptAuxTypeDesc->SizeOf());
    *pDst = *pSrc;
    ptAuxTypeDesc->SerializeToBinary(lpDefinition, field, lpDst, dwDstOffset + sizeof(BOOL), dwTotalSize);
}

// Original TypeExtensions.cpp line 5054.
void TypeDescriptorClass_DT_OPTIONAL::SerializeFromBinary(void * lpDefinition, const UnserializeData & tUnserializeData, const TypeDescriptorField & field) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    BOOL *pbExists = (BOOL *)((uint8 *)lpDefinition + ptAuxTypeDesc->SizeOf());
    ptAuxTypeDesc->SerializeFromBinary(lpDefinition, tUnserializeData, field);
}

// Original TypeExtensions.cpp line 5072.
BOOL TypeDescriptorClass_DT_OPTIONAL::Encode(void * lpDefinition, const TypeDescriptorField & field, XEncoder & tEncoder) const
{
    size_t BitsUsed = tEncoder.Used();
    if (BitsUsed == tEncoder.Size()) return FALSE;
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    BOOL *pbExists = (BOOL *)((uint8 *)lpDefinition + ptAuxTypeDesc->SizeOf());
    tEncoder.PutBool(*pbExists);
    if (*pbExists && !ptAuxTypeDesc->Encode(lpDefinition, field, tEncoder)) { tEncoder.SetUsed(BitsUsed); return FALSE; }
    return TRUE;
}

// Original TypeExtensions.cpp line 5108.
XDecoder::Result TypeDescriptorClass_DT_OPTIONAL::Decode(void * lpDefinition, const TypeDescriptorField & field, XDecoder & tDecoder) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    BOOL *pbExists = (BOOL *)((uint8 *)lpDefinition + ptAuxTypeDesc->SizeOf());
    XDecoder::Result eResult = tDecoder.GetBool(*pbExists);
    if (*pbExists && eResult == XDecoder::RESULT_VALID) return ptAuxTypeDesc->Decode(lpDefinition, field, tDecoder);
    return eResult;
}

// Original TypeExtensions.cpp line 5128.
void TypeDescriptorClass_DT_OPTIONAL::Fuzz(void * lpDefinition, const TypeDescriptorField & field, RNG & tRNG) const
{
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    BOOL bExists = Rand_Internal(tRNG, 1);
    BOOL *pbExists = (BOOL *)((uint8 *)lpDefinition + ptAuxTypeDesc->SizeOf());
    *pbExists = bExists;
    if (bExists) ptAuxTypeDesc->Fuzz(lpDefinition, field, tRNG);
}

// Original TypeExtensions.cpp line 5147.
void TypeDescriptorClass_DT_OPTIONAL::CreateFieldHash(const TypeDescriptorField & field, uint32 & dwHash) const
{
    XTypeDescriptor::CreateFieldHash(field, dwHash);
    const XTypeDescriptor *ptAuxTypeDesc = field.GetAuxTypeDesc();
    ptAuxTypeDesc->CreateFieldHash(field, dwHash);
}

// Original TypeExtensions.cpp line 5161.
uint32 ComplexTypeDescriptor::GetSub64BitPlatformMask(const TypeDescriptorField & field) const
{
    uint32 dwSub64BitPlatformMask = 0;
    for (int32 i = 0; !arFields[i].IsNull(); ++i)
    {
        const TypeDescriptorField & fieldChild = arFields[i];
        if (fieldChild.dwFlags & ((1 << TDF_XPTR64) | (1 << TDF_XPTR128)))
        {
            uint32 dwPlatformMask = fieldChild.dwFlags & 0x704800;
            if (!dwPlatformMask) return 0x704800;
            dwSub64BitPlatformMask |= dwPlatformMask;
        }
        dwSub64BitPlatformMask |= fieldChild.GetTypeDesc()->GetSub64BitPlatformMask(fieldChild);
    }
    return dwSub64BitPlatformMask;
}

