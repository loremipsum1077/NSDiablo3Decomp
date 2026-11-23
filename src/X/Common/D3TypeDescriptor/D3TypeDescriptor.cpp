// Original file: X/Common/D3TypeDescriptor/D3TypeDescriptor.cpp
//
// The game-side field subclasses. The first three add a `dwFilter` word on top of an
// X/Sigma field class; the last two pick their own built-in descriptor by name.

#include "D3TypeDescriptor.h"

#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"

// The base is built with the caller's flags plus the XML-serialization bit, so a filtered
// field is always serialized.
FilterTypeDescriptorField::FilterTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType, int32 nDataOffset,
                                                     const void * vDefaultFieldValue, uint32 dwFilter, const uint32 dwFlags,
                                                     int32 nVariantTypeValue)
    : TypeDescriptorField(szFieldName, ptDataType, nDataOffset, vDefaultFieldValue, dwFlags | (1 << TDF_XML_SERIALIZATION))
{
    this->dwFilter = dwFilter;
    this->nVariantTypeValue = nVariantTypeValue;
}

FilteredFixedArrayTypeDescriptorField::FilteredFixedArrayTypeDescriptorField(LPCSTR szFieldName, const XTypeDescriptor * ptDataType,
                                                                             int32 nDataOffset, const void * vDefaultFieldValue,
                                                                             const uint32 dwFlags, int32 nCount, uint32 dwFilter,
                                                                             int32 nUsedElementsOffset)
    : FixedArrayTypeDescriptorField(szFieldName, ptDataType, nDataOffset, vDefaultFieldValue,
                                    dwFlags | (1 << TDF_XML_SERIALIZATION), nCount, nUsedElementsOffset)
{
    this->dwFilter = dwFilter;
}

FilteredVariableArrayTypeDescriptorField::FilteredVariableArrayTypeDescriptorField(LPCSTR szFieldName,
                                                                                   const XTypeDescriptor * ptDataType,
                                                                                   int32 nDataOffset, uint32 dwFlags,
                                                                                   int32 nSerOffset, uint32 dwFilter)
    : VariableArrayTypeDescriptorField(szFieldName, ptDataType, nDataOffset, dwFlags, nSerOffset)
{
    this->dwFilter = dwFilter;
}

// The two leaf fields whose type is fixed by the constructor itself: it loads the built-in
// descriptor pointer, so the caller never names a type.
TagMapTypeDescriptorField::TagMapTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, uint32 dwFlags, int32 nSerOffset,
                                                     int32 eTagBase)
    : TypeDescriptorField(szFieldName, DT_TAGMAP, nDataOffset, NULL, dwFlags)
{
    this->nSerOffset = nSerOffset;
    this->eTagBase = eTagBase;
}

FormulaTypeDescriptorField::FormulaTypeDescriptorField(LPCSTR szFieldName, int32 nDataOffset, int32 nSerOffset)
    : TypeDescriptorField(szFieldName, DT_FORMULA, nDataOffset, NULL, 0)
{
    this->nSerOffset = nSerOffset;
}
