// X/Common/TagMaps/MapSerializationUtils.h -- not in the DWARF reference tree (it defines no
// types). The declarations are written from the definitions in MapSerializationUtils.cpp
// (lines 18..434); the mangled names put them in namespace MapSerializationUtils.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "TagNumbers.h"

struct StringFormula;

namespace MapSerializationUtils {
void SerializeInt(int32 nValue, void *& pBlockPtr, int32 & nBlockSize);
void UnserializeInt(void *& pBlockPtr, int32 & nValue);
void SerializeInt(uint32 dwValue, void *& pBlockPtr, int32 & nBlockSize);
void UnserializeInt(void *& pBlockPtr, uint32 & dwValue);
void SerializeFormula(XPtr64<unsigned char> & pData, void *& pBlockPtr, int32 & nBlockSize);
void UnserializeFormula(void *& pBlockPtr, XPtr64<unsigned char> & pData, BOOL bReadOnly);
void SerializeVariant(const TagVariantType eTagVariantType, TagVariant & variantTag, void *& pBlockPtr,
                      int32 & nBlockSize);
void UnserializeVariant(const TagVariantType eTagVariantType, void *& pBlockPtr, TagVariant & variantTag,
                        BOOL bReadOnly);
float VariantToFloat(const TagVariant & variantTag);
StringFormula * VariantToStringFormula(const TagVariant & variantTag);
void PutIntInVariant(TagVariant & variantTag, int32 & nValue);
void FreeVariant(TagVariantType eVariantType, TagVariant & variantTag);
}
