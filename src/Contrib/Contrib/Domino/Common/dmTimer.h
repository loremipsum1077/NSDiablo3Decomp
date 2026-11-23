// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/Domino/Common/dmTimer.h

#pragma once

#include "../Include/dmSerialize.h"

class dmTimer {  // size 0x8
public:
    static void Initialize();
    dmTimer();
    void Reset();
    dm_int32 GetCycles() const;
    dm_float32 GetMilliseconds() const;
private:
    static dm_int64 s_frequency;
    static dm_float32 s_period;
    dm_int64 m_baseCount;  // 0x00
};
static_assert(sizeof(dmTimer) == 0x8, "dmTimer");

