#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"
// Blank lines keep the original line numbers

namespace nn { namespace crypto {
void GenerateCryptographicallyRandomBytes(void*, size_t);
} }







void bdGetRandomUChar8(bdUByte8* in, const bdUInt32 length)
{

    nn::crypto::GenerateCryptographicallyRandomBytes(in, length);
}
