#pragma once

#include "SigmaTypes.h"

class Stopwatch {
public:
    Stopwatch() { Reset(); }
    static void InitializeForApp();
    static void DisposeForApp();
    static float ToSeconds(int64 liTime);
    void Reset();
    float Check();
    int64 CheckLong();

private:
    int64 m_liStartTime;
    static int64 s_liFrequency;
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(Stopwatch) == 0x8, "Stopwatch");
