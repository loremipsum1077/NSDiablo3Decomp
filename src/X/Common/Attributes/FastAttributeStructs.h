// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Attributes/FastAttributeStructs.h
// Subset: FastAttribKey, FastAttribValue and their constructors.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "FastAttribConstantsSwig.h"
#include "../../Sigma/SigmaTypes.h"

struct FastAttribKey {  // size 0x4
    FastAttribKey() {}  // inline-only (no out-of-line copy); a bodiless declaration spills callers
    // Line 25; the comparison is on line 27.
    BOOL operator==(const FastAttribKey & tKey) const {
        return nValue == tKey.nValue;
    }
    // Line 30.
    BOOL operator!=(const FastAttribKey & tKey) const {
        return !(*this == tKey);
    }
    // Line 35. A key with no parameter keeps all 20 parameter bits set (UIBossElements.cpp
    // passes 0xfffff29f for attribute 0x29f).
    FastAttribKey(Attrib eAttrib) {
        nValue = eAttrib | 0xfffff000;
    }
    int32 nValue;  // 0x00
};
static_assert(sizeof(FastAttribKey) == 0x4, "FastAttribKey");

// Primary template: X/Sigma/DataStructures/XBaseMap.h.
template <typename ARG_KEY>
inline uint32 XHashKey(ARG_KEY key);

// Line 46; the hash is on line 51. Folds the parameter bits into the attribute index
// (FastAttribGetBaseValueFloat: eor w8, w1, w1, lsr #12 before the 256-bucket mask).
template <>
inline uint32 XHashKey<FastAttribKey>(FastAttribKey tKey) {
    // Signed shift: callers with a variable hash mask (RActors.cpp's maps) keep the asr; with a
    // constant mask clang narrows it to the lsr FastAttribGetBaseValueFloat shows.
    return tKey.nValue ^ (tKey.nValue >> 12);
}

struct FastAttribValue {  // size 0x4
    FastAttribValue() {}  // empty: FastAttribGetBaseValueFloat's local tValue has no constructor call
    FastAttribValue(const FastAttribValue & arg_value);
    FastAttribValue & operator=(const FastAttribValue & arg_value);
    FastAttribValue(int32);
    FastAttribValue(uint32);
    FastAttribValue(long int);
    FastAttribValue(float);
    union {  // size 0x4
        float flValue;  // 0x00
        int32 nValue;  // 0x00
    };  // 0x00
};
static_assert(sizeof(FastAttribValue) == 0x4, "FastAttribValue");
























inline FastAttribValue::FastAttribValue(
    const FastAttribValue & arg_value)
{
    nValue = arg_value.nValue;
}








inline FastAttribValue::FastAttribValue(
    int32 arg_nValue)  // line 75
{
    nValue = arg_nValue;
}

// Original line 68, inlined by FastAttribAggregate.
inline FastAttribValue & FastAttribValue::operator=(const FastAttribValue & arg_value) {
    nValue = arg_value.nValue;
    return *this;
}

// Original FastAttributeStructs.h float constructor.
inline FastAttribValue::FastAttribValue(float arg_flValue) { flValue = arg_flValue; }
