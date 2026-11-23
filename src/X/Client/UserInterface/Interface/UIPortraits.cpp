// Original file: X/Client/UserInterface/Interface/UIPortraits.cpp
//
// The three portrait-art accessors. `UIPortraits` is a namespace: no type or
// variable record for it exists in the DWARF and none of the bodies takes a
// `this` pointer. Each one reads a single `ImageHandle` out of the portrait's
// `ItemDefinition`, resolves it against image collection 2, and falls back to
// the shared null frame when the GBID is unset or the record is missing.

#include "../../Gfx/GfxCommon.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"

// Defined in X/Client/ImageCollection.cpp; src/ has no ImageCollection.h yet.
typedef int32 ImageCollectionType;
BOOL ImageCollectionGetImageInfo(const ImageCollectionType eCollection, const ImageHandle hImage,
                                 ImageTextureFrame & tImageTexFrame);

namespace UIPortraits {

ImageTextureFrame GetPortraitBackgroundImage(GBID gbidPortrait)
{
    if (gbidPortrait == GBID_NONE || gbidPortrait == INVALID_GBID)
    {
        return cgtImageTextureFrameNull;
    }

    ScopedItemDefinitionLockR pdefItem(gbidPortrait);

    if (pdefItem != NULL)
    {
        ImageTextureFrame tImageTextureFrame;
        ImageCollectionGetImageInfo(2, pdefItem->hPortraitBkgrnd, tImageTextureFrame);
        return tImageTextureFrame;
    }

    return cgtImageTextureFrameNull;
}

ImageTextureFrame GetPortraitBannerImage(GBID gbidPortrait)
{
    if (gbidPortrait == GBID_NONE || gbidPortrait == INVALID_GBID)
    {
        return cgtImageTextureFrameNull;
    }

    ScopedItemDefinitionLockR pdefItem(gbidPortrait);

    if (pdefItem != NULL)
    {
        ImageTextureFrame tImageTextureFrame;
        ImageCollectionGetImageInfo(2, pdefItem->hPortraitBanner, tImageTextureFrame);
        return tImageTextureFrame;
    }

    return cgtImageTextureFrameNull;
}

ImageTextureFrame GetPortraitFrameImage(GBID gbidPortrait)
{
    if (gbidPortrait == GBID_NONE || gbidPortrait == INVALID_GBID)
    {
        return cgtImageTextureFrameNull;
    }

    ScopedItemDefinitionLockR pdefItem(gbidPortrait);

    if (pdefItem != NULL)
    {
        ImageTextureFrame tImageTextureFrame;
        ImageCollectionGetImageInfo(2, pdefItem->hPortraitFrame, tImageTextureFrame);
        return tImageTextureFrame;
    }

    return cgtImageTextureFrameNull;
}

}  // namespace UIPortraits
