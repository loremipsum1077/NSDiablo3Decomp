// Adopted from the DWARF skeleton (build/dwarf/headers/X/Client/ImageCollection.h);
// hand-maintained from here on.
// Original file: X/Client/ImageCollection.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Font/FormattedText.h"
#include "Gfx/GfxCommon.h"
#include "../Common/ImageHandle.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/Unicode.h"

struct ImageInfo {  // size 0xc
    ImageHandle hImage;  // 0x00
    ImageTextureFrame tImageTexFrame;  // 0x04
};
static_assert(sizeof(ImageInfo) == 0xc, "ImageInfo");

enum {
    IMAGE_COLLECTION_INVALID = -1,
    IMAGE_COLLECTION_ITEM = 0,
    IMAGE_COLLECTION_MAP = 1,
    IMAGE_COLLECTION_UI = 2,
    IMAGE_COLLECTION_NUM_TYPES = 3,
};

typedef int32 ImageCollectionType;

typedef XGrowableMap<unsigned int, const unsigned int, ImageInfo *, ImageInfo *> ImageMap;

struct ImageCollection {  // size 0xa8
    ImageCollectionType eType;  // 0x00
    CHAR szTexturePrefixString[32];  // 0x04
    ImageMap mapImageMap;  // 0x28
};
static_assert(sizeof(ImageCollection) == 0xa8, "ImageCollection");

typedef void (*ImageCollectionIterateCallback)(const ImageCollection &, const ImageInfo &, void *);

// X/Client/ImageCollection.cpp:949
//   _Z27ImageCollectionGetImageInfoPKcR17ImageTextureFrame
BOOL ImageCollectionGetImageInfo( LPCSTR szDecoratedLabel,
                                  ImageTextureFrame & tImageTexFrame );
// X/Client/ImageCollection.cpp:979
//   _Z27ImageCollectionGetImageInfoiPKcR17ImageTextureFrame
BOOL ImageCollectionGetImageInfo( const ImageCollectionType eCollection,
                                  const CHAR * szImageName,
                                  ImageTextureFrame & tImageTexFrame );

// Functions defined in this file (bodies are not in the DWARF):
//   line    26  ImageInfo::ImageInfo()  [inlined into 1 functions]
//   line    54  ImageCollection::ImageCollection()  [inlined into 1 functions]
//   line    54  ImageCollection::~ImageCollection()  [inlined into 1 functions]

BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);  // ImageCollection.cpp

// ImageCollection.cpp: original image-handle overload.
BOOL ImageCollectionGetImageInfo(const ImageCollectionType eCollection, const ImageHandle hImage, ImageTextureFrame & tImageTexFrame);
