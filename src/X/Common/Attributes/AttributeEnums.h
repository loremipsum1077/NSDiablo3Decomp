// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Attributes/AttributeEnums.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    ATTRIBUTEDT_UNKNOWN = -1,
    ATTRIBUTEDT_FLOAT = 0,
    ATTRIBUTEDT_INT = 1,
};

typedef int32 AttributeDataType;

enum {
    ATTRIBUTEOP_NONE = -1,
    ATTRIBUTEOP_PLUS = 0,
    ATTRIBUTEOP_MAX = 1,
    ATTRIBUTEOP_APPEND = 2,
    ATTRIBUTEOP_SET = 3,
    ATTRIBUTEOP_NOAGG = 4,
    ATTRIBUTEOP_MULTIPLY = 5,
    ATTRIBUTEOP_MIN = 6,
    ATTRIBUTEOP_MULTIPLY_INVERSE = 7,
    ATTRIBUTEOP_ACDID = 8,
};

typedef int32 AttributeOperator;

typedef int32 AttributeParameterType;

