// Original file: X/Client/Map/Minimap.cpp
//
// The client's minimap helpers. In this build the only function the linker kept in this
// translation unit is ResolveIconForRActor, which turns an RActor's marker set into the
// image-collection entry the UI draws.

#include "../Gfx/GfxCommon.h"
#include "../RActors/RActors.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Player.h"
#include "../../Sigma/RefString.h"

// Defined in X/Client/CPlayer.cpp:1612; no adopted header declares it yet.
Player * LocalPlayerGet();

// Defined in X/Common/Map/MinimapCommon.cpp:28; no adopted header declares it yet.
namespace Minimap {
void ResolveIconForActor(const ActorCommonData & tACD, const SNO snoActor, const MinimapMarkerSetInfo & tMarkerOwner,
                         const int32 nPlayerIndex, SNO & snoRet, CRefString & szRet);
}

// Defined in X/Client/ImageCollection.cpp:979; src/ has no ImageCollection.h yet.
typedef int32 ImageCollectionType;
enum {
    IMAGE_COLLECTION_MAP = 1,
    IMAGE_COLLECTION_UI = 2,
};
BOOL ImageCollectionGetImageInfo(const ImageCollectionType eCollection, const CHAR * szImageName,
                                 ImageTextureFrame & tImageTexFrame);

namespace Minimap {

ImageTextureFrame ResolveIconForRActor(const RActor * ptRActor)
{
    const ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (!ptACD)
    {
        return cgtImageTextureFrameNull;
    }

    CRefString sImageHandle;
    SNO snoRet;

    ResolveIconForActor(*ptACD, ptACD->snoActor, ptRActor->tMinimapInfo, LocalPlayerGet()->nPlayerIndex, snoRet,
                        sImageHandle);

    ImageTextureFrame tImageTexFrame;
    if (snoRet == SNO_NONE)
    {
        if (!ImageCollectionGetImageInfo(IMAGE_COLLECTION_MAP, sImageHandle, tImageTexFrame))
        {
            ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, sImageHandle, tImageTexFrame);
        }
    }
    else
    {
        tImageTexFrame.snoTexture = snoRet;
    }

    return tImageTexFrame;
}

}  // namespace Minimap
