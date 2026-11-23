// Original file: BlizzardCore/Include/Blizzard/Util/Crypto.h
// Trimmed to the one declaration used so far. Which header declared CRC32 is a guess; the
// prototype and parameter names come from its definition in Util/Crypto/CRC-32-Zlib.cpp.

#pragma once

#include "../Types.h"

namespace Blizzard {
namespace Util {
uint32 CRC32(const void * buffer, uint size, uint32 lastCRC);
// Crypto.h:35. Only the members used so far; k_invalid is defined in the MD5/SHA1 sources.
template <int SIZE>
struct Digest {
    uint8 m_data[SIZE];  // 0x00
    static const Digest<SIZE> k_invalid;
};
static_assert(sizeof(Digest<16>) == 0x10, "Digest<16>");
// Crypto.h:220 and :235. Bodies written from the copies inlined into GamerProfileNX64.cpp. EncXor64 rows (relative to 220): while +2, store +5, key update +6 (with the ldrb: zext(load) fold), decrement +8.
inline void EncXor64(uint64 uKey, uint8 * pData, uint32 uDataSize)
{
    while (uDataSize > 0)
    {
        uint8 uByte = *pData;
        *pData = uByte ^ (uint8)uKey;
        uKey = (uKey >> 8) | ((uKey ^ uByte) << 56);
        pData++;
        uDataSize--;
    }
}

inline void DecXor64(uint64 uKey, uint8 * pData, uint32 uDataSize)
{
    while (uDataSize--)
    {
        uint8 uByte;
        uByte = *pData ^ (uint8)uKey;
        *pData = uByte;
        uKey = (uKey >> 8) | ((uKey ^ uByte) << 56);
        pData++;
    }
}
}  // namespace Util
}  // namespace Blizzard
