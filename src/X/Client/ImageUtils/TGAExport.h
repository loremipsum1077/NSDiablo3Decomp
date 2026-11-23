// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/ImageUtils/TGAExport.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

enum {
    TGAIMAGETYPE_NO_IMAGE_DATA = 0,
    TGAIMAGETYPE_UNCOMPRESSED_COLORMAPPED = 1,
    TGAIMAGETYPE_UNCOMPRESSED_RGB = 2,
    TGAIMAGETYPE_UNCOMPRESSED_BW = 3,
    TGAIMAGETYPE_RLE_COLORMAPPED = 9,
    TGAIMAGETYPE_RLE_RGB = 10,
    TGAIMAGETYPE_RLE_BW = 11,
};

#pragma pack(push, 1)
struct TGAHeader {  // size 0x12
    uint8 bIDLength;  // 0x00
    uint8 bColorMapType;  // 0x01
    uint8 bImageType;  // 0x02
    uint16 wColorMapStartIndex;  // 0x03
    uint16 wColorMapEntries;  // 0x05
    uint8 bColorMapEntrySize;  // 0x07
    uint16 wXOrigin;  // 0x08
    uint16 wYOrigin;  // 0x0a
    uint16 wWidth;  // 0x0c
    uint16 wHeight;  // 0x0e
    uint8 bPixelDepth;  // 0x10
    union {  // size 0x1
        uint8 bImageDescriptor;  // 0x00
        struct {  // size 0x1
            uint8 bAlphaChannelBits : 4;  // 0x00
            uint8 bLeftRightOrder : 1;  // 0x00
            uint8 bTopBottomOrder : 1;  // 0x00
            uint8 bReserved : 2;  // 0x00
        } Desc;  // 0x00
    };  // 0x11
};
#pragma pack(pop)
static_assert(sizeof(TGAHeader) == 0x12, "TGAHeader");
