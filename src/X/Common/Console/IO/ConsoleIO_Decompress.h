// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Console/IO/ConsoleIO_Decompress.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/Zlib/zlib.h"

struct EncryptedAndCompSize {  // size 0x2
    uint16 dwCompSize : 15;  // 0x00
    uint16 fEncrypted : 1;  // 0x00
};
static_assert(sizeof(EncryptedAndCompSize) == 0x2, "EncryptedAndCompSize");

struct ZLibPacketHeader {  // size 0x6
    uint16 dwDecompSizeLow;  // 0x00
    uint16 dwDecompSizeHigh;  // 0x02
    union {  // size 0x2
        EncryptedAndCompSize tEncryptedAndCompSize;  // 0x00
        uint16 dwEncryptedAndCompSize;  // 0x00
    };  // 0x04
    void SetDecompSize(uint32);
    uint32 GetDecompSize() const;
};
static_assert(sizeof(ZLibPacketHeader) == 0x6, "ZLibPacketHeader");

// Defined in ConsoleIO_Decompress.cpp:23 and :116; where the original declares them is not in the DWARF.
BOOL SkipDecompressData(z_stream * ptStream, int32 nSize, uint8 * pVoidBuffer, int32 nVoidBufferSize, BOOL fByteSwap, uint32 & dwCurrentPacketRemainingDecompSize, BOOL & fInsidePacketHeader);
BOOL DecompressData(z_stream * ptStream, int32 nSize, uint8 * pBuffer, BOOL fByteSwap, uint32 & dwCurrentPacketRemainingDecompSize, BOOL & fInsidePacketHeader);


// Functions defined in this file (bodies are not in the DWARF):
//   line    38  uint32 ZLibPacketHeader::GetDecompSize() const  [inlined into 2 functions]
