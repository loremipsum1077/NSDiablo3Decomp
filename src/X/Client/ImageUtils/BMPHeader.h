// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/ImageUtils/BMPHeader.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

// The DWARF records the member offsets but not the packing; the 0xe/0x28 sizes
// require the headers to be byte- or word-packed.
#pragma pack(push, 1)
struct tagBITMAPFILEHEADER {  // size 0xe
    uint16 bfType;  // 0x00
    uint32 bfSize;  // 0x02
    uint16 bfReserved1;  // 0x06
    uint16 bfReserved2;  // 0x08
    uint32 bfOffBits;  // 0x0a
};
static_assert(sizeof(tagBITMAPFILEHEADER) == 0xe, "tagBITMAPFILEHEADER");

typedef tagBITMAPFILEHEADER BITMAPFILEHEADER;

struct tagBITMAPINFOHEADER {  // size 0x28
    uint32 biSize;  // 0x00
    uint32 biWidth;  // 0x04
    uint32 biHeight;  // 0x08
    uint16 biPlanes;  // 0x0c
    uint16 biBitCount;  // 0x0e
    uint32 biCompression;  // 0x10
    uint32 biSizeImage;  // 0x14
    uint32 biXPelsPerMeter;  // 0x18
    uint32 biYPelsPerMeter;  // 0x1c
    uint32 biClrUsed;  // 0x20
    uint32 biClrImportant;  // 0x24
};
static_assert(sizeof(tagBITMAPINFOHEADER) == 0x28, "tagBITMAPINFOHEADER");

typedef tagBITMAPINFOHEADER BITMAPINFOHEADER;
#pragma pack(pop)

