// Original file: BlizzardCore/Source/Packages/Util/Package_Util.cpp
// The original is a unity file; the DWARF decl_file of its functions names Hash/Hash.cpp,
// Crypto/CRC-32-Zlib.cpp and BitField.cpp. Their bodies are written here in that order, each
// with a comment giving its original file and line. Functions the linker stripped are not written.
#include "../../../Include/Blizzard/Memory.h"
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Types.h"
#include "../../../Include/Blizzard/Util/BitField.h"
#include "../../../Include/Blizzard/Util/Crypto.h"
#include "../../../Include/Blizzard/Util/Hash.h"
#include "../../../Include/Blizzard/Util/Jobs.h"
#include "../../../../Contrib/Contrib/Zlib/zlib.h"

namespace Blizzard {
namespace Util {
template <> const Digest<16> Digest<16>::k_invalid = { { 0 } };
// ---- Hash/Hash.cpp ----

namespace DefaultHashValues {
static const uint32 FNV_32_PRIME = 0x01000193;  // line 40
static const uint32 FNV_32_INIT = 0x811c9dc5;  // line 41
static const uint64 FNV_64_PRIME = 0x00000100000001b3ULL;  // line 110
static const uint64 FNV_64_INIT = 0xcbf29ce484222325ULL;  // line 111
}  // namespace DefaultHashValues

// line 45
uint32 Hash32More(const void * data, uint size, uint32 previousHash)
{
    uint32 hash = previousHash;
    const uint8 * buffer = (const uint8 *)data;
    while (size--)
    {
        hash ^= *buffer++;
        hash *= DefaultHashValues::FNV_32_PRIME;
    }
    return hash;
}

// line 58
uint32 Hash32(const void * data, uint size)
{
    return Hash32More(data, size, DefaultHashValues::FNV_32_INIT);
}

// line 64
uint32 Hash32(const char * string)
{
    return Hash32(string, String::Length(string));
}

// line 115
uint64 Hash64More(const void * data, uint size, uint64 previousHash)
{
    uint64 hash = previousHash;
    const uint8 * buffer = (const uint8 *)data;
    while (size--)
    {
        hash ^= *buffer++;
        hash *= DefaultHashValues::FNV_64_PRIME;
    }
    return hash;
}

// line 128
uint64 Hash64(const void * data, uint size)
{
    return Hash64More(data, size, DefaultHashValues::FNV_64_INIT);
}

// line 134
uint64 Hash64(const char * string)
{
    return Hash64(string, String::Length(string));
}

// line 140
uint64 Hash64(char * string)
{
    return Hash64(string, String::Length(string));
}

// ---- Crypto/CRC-32-Zlib.cpp ----

// line 35
uint32 CRC32(const void * buffer, uint size, uint32 lastCRC)
{
    return crc32(lastCRC, (const Bytef *)buffer, size);
}

// ---- BitField.cpp ----

// line 31. The well-known table of bit counts, built with the usual B2/B4/B6 macros.
static const uint8 s_BitsSetTable256[256] = {
#define B2(n) n, n + 1, n + 1, n + 2
#define B4(n) B2(n), B2(n + 1), B2(n + 1), B2(n + 2)
#define B6(n) B4(n), B4(n + 1), B4(n + 1), B4(n + 2)
    B6(0), B6(1), B6(1), B6(2)
#undef B6
#undef B4
#undef B2
};

BitField::SharedDelete BitField::defaultDeleteFunction = Memory::Free;

// line 42
BitField::BitField(int numBits)
    : numbits(numBits),
      length((numBits + 7) >> 3)
{
    bits = Memory::Allocate(length, Memory::ZEROFILL);
    sharedHandle = bits;
    deleteFunc = defaultDeleteFunction;
}

// line 50
BitField::BitField(const void * buffer, int numBits)
    : numbits(numBits),
      length((numBits + 7) >> 3)
{
    bits = Memory::Allocate(length);
    sharedHandle = bits;
    deleteFunc = defaultDeleteFunction;

    String::MemCopy(bits, buffer, length);
}

// line 68
BitField::~BitField()
{
    deleteFunc(sharedHandle);
}

// line 73
void BitField::Set(int index)
{
    int byteIndex = index >> 3;
    uint8 mask = (uint8)(1 << (index & 7));
    bits[byteIndex] |= mask;
}

// line 86
void BitField::Clear(int index)
{
    int byteIndex = index >> 3;
    uint8 mask = (uint8)(1 << (index & 7));
    bits[byteIndex] &= ~mask;
}

// line 105
bool BitField::IsSet(int index) const
{
    return (bits[index >> 3] & (1 << (index & 7))) != 0;
}

// line 150
int64 BitField::GetCountOfBitsSet() const
{
    int64 count = 0;
    uint8 * byteptr = bits;
    uint8 * end = bits + length;
    while (byteptr < end)
    {
        count += s_BitsSetTable256[*byteptr++];
    }
    return count;
}

}  // namespace Util

// ---- Jobs/Job.cpp ----

namespace Jobs {

// LINKER-REMOVED: Job.cpp:35, DWARF size 0x144 (low_pc 0). Kept because it is what makes this unit
// emit the Blizzard::Lock::RefCounted vtable and so RefCounted's D2 (0x710000ae80) and D0
// (0x710000aea0): its inline tree ends with ~RefCounted at line 44. Body from the inline tree and
// line rows: a block with local `i` (line 37) whose RefCounted::Release calls (line 40) appear four
// times, each after a null test at line 39 column 7; a null test at line 42 column 6 and Release at
// line 43; then the m_NextJob RefPtr destructor and ~RefCounted at line 44. Inferred: the loop
// bound MAX_INPUTS (four unrolled copies), and the spelling of the null tests.
Job::~Job()
{
    for (int i = 0; i < MAX_INPUTS; i++)
    {
        if (m_Inputs[i])
            m_Inputs[i]->Release();
    }
    if (m_Output)
        m_Output->Release();
}

// LINKER-REMOVED: Job.cpp:47, DWARF size 0x40 (low_pc 0). Its DWARF has a block-scoped local
// `inputIndex` (line 50) and no inline calls, so the members are read directly. Inferred: the loop
// bound m_numRequiredInputs and the null test on m_Inputs.
bool Job::IsReady() const
{
    for (int inputIndex = 0; inputIndex < m_numRequiredInputs; inputIndex++)
    {
        if (!m_Inputs[inputIndex])
            return false;
    }
    return true;
}

}  // namespace Jobs
}  // namespace Blizzard
