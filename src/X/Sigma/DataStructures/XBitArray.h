#pragma once

#include "../SigmaTypes.h"

#include <stddef.h>

#include "../Memory/SigmaMemory.h"






template <size_t TSIZE>
class XBitArray {
public:

    inline XBitArray() { Zero(); }




    inline void Zero() {


        SigmaMemoryClear(elements, sizeof(elements));
    }

    inline void Set(size_t i, BOOL bVal) {






        elements[i >> 3] &= ~(1 << (i & 7));
        elements[i >> 3] |= bVal ? (1 << (i & 7)) : 0;
    }

    // Declared in the DWARF; never instantiated, so there is no body to go by.
    inline void Swap(size_t i);







    inline BOOL Get(size_t i) const {



        return (elements[i >> 3] & (1 << (i & 7))) != 0;
    }

    inline uint8* ByteData() { return elements; }
    inline const uint8* ByteData() const { return elements; }

    // Declared in the DWARF; never instantiated, so there is no body to go by.
    inline BOOL Test() const;



















    inline BOOL Test(size_t i) const {


        return Get(i);
    }

protected:
    uint8 elements[(TSIZE + 7) / 8];
};

// Swap(i): no instantiation survives in the binary (its only users, Game.cpp's AppRenderLayerToggle
// and AppDrawFlagToggle, were linker-stripped), so this body is a guess that cannot be verified.
template <size_t TSIZE>
inline void XBitArray<TSIZE>::Swap(size_t i) {
    elements[i >> 3] ^= (1 << (i & 7));
}
