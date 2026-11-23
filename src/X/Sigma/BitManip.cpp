#include "../../BlizzardCore/Include/Blizzard/Util/BitField.h"
#include "SigmaTypes.h"


void SetBit(uint8* lpb, int32 nWhichBit) {
    lpb[nWhichBit >> 3] |= 1 << (nWhichBit & 7);
}

BOOL TestBit(const uint8* lpb, int32 nWhichBit) {
    return (lpb[nWhichBit >> 3] & (1 << (nWhichBit & 7))) != 0;
}

int32 CountSetBits(const uint8* lpb, const int32 nLen) {
    Blizzard::Util::BitField tBitField(lpb, nLen * 8);
    return tBitField.GetCountOfBitsSet();
}
