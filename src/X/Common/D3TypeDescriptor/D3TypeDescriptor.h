// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/D3TypeDescriptor/D3TypeDescriptor.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Sigma/Unicode.h"

class XTypeDescriptor;

class FilterTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    FilterTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, uint32, const uint32, int32);
};
static_assert(sizeof(FilterTypeDescriptorField) == 0xc0, "FilterTypeDescriptorField");

class FilteredFixedArrayTypeDescriptorField : public FixedArrayTypeDescriptorField {  // size 0xc0
public:
    FilteredFixedArrayTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, const void *, const uint32, int32, uint32, int32);
};
static_assert(sizeof(FilteredFixedArrayTypeDescriptorField) == 0xc0, "FilteredFixedArrayTypeDescriptorField");

class FilteredVariableArrayTypeDescriptorField : public VariableArrayTypeDescriptorField {  // size 0xc0
public:
    FilteredVariableArrayTypeDescriptorField(LPCSTR, const XTypeDescriptor *, int32, uint32, int32, uint32);
};
static_assert(sizeof(FilteredVariableArrayTypeDescriptorField) == 0xc0, "FilteredVariableArrayTypeDescriptorField");

class TagMapTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    TagMapTypeDescriptorField(LPCSTR, int32, uint32, int32, int32);
};
static_assert(sizeof(TagMapTypeDescriptorField) == 0xc0, "TagMapTypeDescriptorField");

class FormulaTypeDescriptorField : public TypeDescriptorField {  // size 0xc0
public:
    FormulaTypeDescriptorField(LPCSTR, int32, int32);
    FormulaTypeDescriptorField(LPCSTR, int32, int32, uint32);
};
static_assert(sizeof(FormulaTypeDescriptorField) == 0xc0, "FormulaTypeDescriptorField");

