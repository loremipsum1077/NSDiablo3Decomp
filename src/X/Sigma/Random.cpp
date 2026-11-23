#include "Random.h"

#include "SigmaTiming.h"

inline uint32 RandomGenerateSeed() {
    uint32 dwSeed = GetMachineTicks();
    return dwSeed & 0x7FFFFFFF;
}

void RandInit(RNG& rng) {
    RandSetSeed_Internal(rng, RandomGenerateSeed());
}

void RandSetSeed_Internal(RNG& rng, const uint32 dwNewSeed) {
    rng.dwSeed = dwNewSeed;
    rng.dwCarry = 666;
}

uint32 RandGetSeed(RNG& rng) {
    return rng.dwSeed;
}

uint32 RandGetDWord(RNG& rng) {
    uint64 qwMult = (uint64)rng.dwSeed * 1791398085 + rng.dwCarry;
    rng.dwSeed = (uint32)qwMult;
    rng.dwCarry = (uint32)(qwMult >> 32);
    return rng.dwSeed;
}
