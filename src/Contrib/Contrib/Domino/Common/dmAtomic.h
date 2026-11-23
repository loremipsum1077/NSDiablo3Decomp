// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Common/dmAtomic.h

#pragma once

#include "../Include/dmSerialize.h"

class dmAtomic_s32 {  // size 0x4
public:
    dmAtomic_s32();
    void StoreRelease(dm_int32 value);
    void StoreRelaxed(dm_int32);
    dm_int32 LoadAcquire() const;
    dm_int32 LoadRelaxed() const;
    dm_int32 Increment();
    dm_int32 Decrement();
    dm_int32 OrEquals(dm_int32);
    dm_int32 AndEquals(dm_int32);
    bool CompareAndSwapRelaxed(dm_int32, dm_int32);
private:
    dm_int32 m_value;  // 0x00
};
static_assert(sizeof(dmAtomic_s32) == 0x4, "dmAtomic_s32");

class dmAtomic_float32 {  // size 0x4
public:
    dmAtomic_float32();
    dmAtomic_float32(dm_float32);
    void operator=(dm_float32 value);
    operator float() const;
    void operator+=(dm_float32 a);
private:
    dm_float32 m_value;  // 0x00
};
static_assert(sizeof(dmAtomic_float32) == 0x4, "dmAtomic_float32");

