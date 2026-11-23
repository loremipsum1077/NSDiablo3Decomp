#pragma once

#include "../Types.h"

namespace Blizzard {
namespace Util {
class BitField {  // size 0x20
public:
    void Set(int index);
    void Clear(int index);
    void ClearAll();
    bool IsSet(int index) const;
    bool IsAllSet(int) const;
    int64 GetCountOfBitsSet() const;
    typedef void (*SharedDelete)(void*);
    BitField(int);
    BitField(const void*, int);
    BitField(void*, int, void*, SharedDelete);
    ~BitField();
    const void* Ptr(int, int, int64&, int&) const;
    int GetLength() const { return length; }
    void* GetRawBuffer() const { return bits; }
    int GetNumBits() const;
    static SharedDelete defaultDeleteFunction;

private:
    uint8* bits;  // 0x00
    int numbits;  // 0x08
    int length;  // 0x0c
    void* sharedHandle;  // 0x10
    SharedDelete deleteFunc;  // 0x18
};
static_assert(sizeof(BitField) == 0x20, "BitField");

}  // namespace Util
}  // namespace Blizzard
