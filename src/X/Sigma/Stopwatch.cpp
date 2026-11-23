#include "Stopwatch.h"

#include "SigmaTiming.h"

int64 Stopwatch::s_liFrequency;

void Stopwatch::InitializeForApp() {
    s_liFrequency = 1000000000;
}

void Stopwatch::DisposeForApp() {}

void Stopwatch::Reset() {
    m_liStartTime = SigmaTimingHighResTimer();
}

float Stopwatch::ToSeconds(int64 liTime) {
    int64 liFreq = s_liFrequency;
    uint32 dwFreq = (uint32)liFreq;
    if (dwFreq == 0)
        return 0.0f;

    uint32 dwElapsed = (uint32)liTime & 0xFFFF;
    float flFreq = (float)dwFreq;
    float flTime = (float)dwElapsed / flFreq;
    float flFreqHigh = (float)(dwFreq >> 16);
    uint32 dwElapsedHigh = (uint32)(liTime >> 16);
    if (flFreqHigh != 0.0f)
        flTime += (float)dwElapsedHigh / flFreqHigh;
    return flTime;
}

float Stopwatch::Check() {
    int64 liLastCheckedTime = SigmaTimingHighResTimer();
    int64 liStop = liLastCheckedTime;
    int64 liStart = m_liStartTime;
    uint32 dwReduceMag = 0;
    uint32 dwHigh32 = (uint32)(liStop >> 32) - (uint32)(liStart >> 32);
    while (dwHigh32 != 0) {
        dwHigh32 >>= 1;
        dwReduceMag++;
    }
    int64 liFreq = s_liFrequency;
    if (dwReduceMag != 0) {
        liStart >>= dwReduceMag;
        liStop >>= dwReduceMag;
        liFreq >>= dwReduceMag;
    }
    uint32 dwFreq = (uint32)liFreq;
    if (dwFreq == 0)
        return 0.0f;

    uint32 dwStop = (uint32)liStop;
    uint32 dwStart = (uint32)liStart;
    float flFreq = (float)dwFreq;
    return (float)(dwStop - dwStart) / flFreq;
}

int64 Stopwatch::CheckLong() {
    int64 liLastCheckedTime = SigmaTimingHighResTimer();
    int64 liElapsed = liLastCheckedTime - m_liStartTime;
    return liElapsed;
}
