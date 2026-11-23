// Original file: X/Client/ImageCollection.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
// Not written (removed by the linker): ImageCollectionGetPrefixForType (277), ImageCollectionDump (297),
// ImageCollectionGetHeadPosition (856), ImageCollectionGetNext (867), ImageCollectionGetImageInfo (885),
// ImageCollectionGetImageName (997), ImageCollectionFilterMatch (1035), ImageCollectionIterateImages (1065).
// ImageCollectionsReset (840) was also removed by the linker but is written: the DWARF records its
// parameter, the local at line 847 and the inlined sImageCollectionCreate at line 850; its two calls
// are reconstructed (a second caller is what keeps sImageCollectionDispose and sImageCollectionLoad
// out of line, as they are in the original).
#include "ImageCollection.h"
#include "Gfx/GfxCommon.h"
#include "../Common/FileIO/FileIO.h"
#include "../Common/ImageHandle.h"
#include "../Common/Definitions/StringListDefinition.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SymbolicValue.h"
#include "../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RefString.h"
#include "../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../Sigma/Unicode.h"
#include "../Sigma/UnicodeText.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"

struct ImageCollectionTable {  // size 0x18
    ImageCollectionType eType;  // 0x00
    LPCSTR pszTexturePrefixString;  // 0x08
    ImageCollection * ptImageCollection;  // 0x10
};

struct ImageCollectionItem {  // size 0xc
    ImageHandle hImageHandle;  // 0x00
    SNO snoHandle;  // 0x04
    int32 nFrameIndex;  // 0x08
};

typedef XChainedFixedBlockAllocator<ImageInfo> ImageInfoAllocator;

// The first word of a "Misc\<prefix>_table.dat" file (0xBBCC0005 in both readers); the name is invented.
static const uint32 IMAGE_COLLECTION_FILE_MAGIC = 0xBBCC0005;






























static ImageCollectionTable sgtImageCollectionTable[4] =
{
    { IMAGE_COLLECTION_ITEM, "2DInventory", NULL },
    { IMAGE_COLLECTION_MAP, "2DMap", NULL },
    { IMAGE_COLLECTION_UI, "2DUI", NULL },
    { IMAGE_COLLECTION_INVALID, NULL, NULL },
};



static const uint32 sadwHashTableSizes[3] =
{
    2048,
    1024,
    4096,
};






static ImageInfoAllocator * sgptImageInfoAllocator;
static BOOL sgfLoaded[3];

static void sImageCollectionLoad(ImageCollection & tCollection);










ImageHandle StringToImageHandle(LPCSTR pszName)
{

    return XHashKey<LPCSTR>(pszName);
}



static ImageCollection * sImageCollectionCreate(
    ImageCollectionType eCollectionType,
    LPCSTR pszTexturePrefixString)
{


    ImageCollection * ptCollection = new ImageCollection;


    ptCollection->mapImageMap.SetHashTableSize_Tracked(sadwHashTableSizes[eCollectionType]);


    ptCollection->eType = eCollectionType;
    Blizzard::String::Copy(ptCollection->szTexturePrefixString, pszTexturePrefixString, sizeof(ptCollection->szTexturePrefixString));

    return ptCollection;
}



























































































































































ImageCollection & ImageCollectionGet(const ImageCollectionType eCollection)
{

    ImageCollectionTable & tTableEntry = sgtImageCollectionTable[eCollection];
    return *tTableEntry.ptImageCollection;
}






































static BOOL sReadFile(
    const char * szFileName,
    unsigned char *& pData,
    uint32 & nDataSize)
{
    pData = NULL;
    nDataSize = 0;

    {
        FileIO::File tFile(XLOCALE_GLOBAL);
        if (tFile.Open(szFileName))
        {
            nDataSize = tFile.GetFileSize();
            pData = (unsigned char *)SigmaMemoryNew(nDataSize, 4, NULL, FALSE);

            if (!tFile.ReadFile(pData, nDataSize, NULL))
            {
                SigmaMemoryFree(pData, NULL); pData = NULL;
            }
        }
    }

    return pData != NULL && nDataSize != 0;
}



static void sImageCollectionAddItem(
    ImageCollection & tCollection,
    const ImageCollectionItem & item,
    BOOL fHotfix)
{
    CRefString sErrors;

    BOOL fExists = FALSE;
    ImageInfo * ptImageInfo = NULL;
    fExists = tCollection.mapImageMap.Lookup(item.hImageHandle, ptImageInfo);
    if (fExists)
    {
        if (!fHotfix)
        {




            CRefString sMessage;
            sMessage.Format("Frame '0x%.8x' in (%s) already exists in (%s)\n",
                item.hImageHandle,
                SNOToString(TEXTURE_SNO, item.snoHandle, FALSE).str(),
                SNOToString(TEXTURE_SNO, ptImageInfo->tImageTexFrame.snoTexture, FALSE).str());
            sErrors += sMessage;
        }
        else
        {

            ptImageInfo->hImage = item.hImageHandle;
            ptImageInfo->tImageTexFrame.nFrame = item.nFrameIndex;
            ptImageInfo->tImageTexFrame.snoTexture = item.snoHandle;
            tCollection.mapImageMap.SetAt_Tracked(item.hImageHandle, ptImageInfo);
        }
    }
    else
    {

        ptImageInfo = sgptImageInfoAllocator->Alloc_Tracked();
        ptImageInfo->hImage = item.hImageHandle;
        ptImageInfo->tImageTexFrame.nFrame = item.nFrameIndex;
        ptImageInfo->tImageTexFrame.snoTexture = item.snoHandle;
        tCollection.mapImageMap.SetAt_Tracked(item.hImageHandle, ptImageInfo);
    }



}





static void sImageCollectionLoad(ImageCollection & tCollection)

{
    CHAR szCollectionMPQFilename[4096];
    Blizzard::String::Format(szCollectionMPQFilename, sizeof(szCollectionMPQFilename), "Misc\\%s_table.dat", tCollection.szTexturePrefixString);

    unsigned char * pData;
    uint32 nDataSize;
    if (!sReadFile(szCollectionMPQFilename, pData, nDataSize))
    {
        return;
    }


    if (*(uint32 *)pData == IMAGE_COLLECTION_FILE_MAGIC)
    {





        const ImageCollectionItem * pItems = (const ImageCollectionItem *)(pData + sizeof(uint32));
        int32 nItemCount = (nDataSize - sizeof(uint32)) / sizeof(ImageCollectionItem);
        for (int32 i = 0; i < nItemCount; i++)
        {
            sImageCollectionAddItem(tCollection, pItems[i], FALSE);
        }
    }
    SigmaMemoryFree(pData, NULL); pData = NULL;
}





























































































































































































































































static void sLoadImageMapIfNecessary(ImageCollectionType eType)
{



    if (sgfLoaded[eType])
    {
        return;
    }









    sgfLoaded[eType] = TRUE;












    ImageCollectionTable & tTable = sgtImageCollectionTable[eType];
    sImageCollectionLoad(*tTable.ptImageCollection);
}










void ImageCollectionsInitialize()

{







    sgptImageInfoAllocator = new ImageInfoAllocator();

    sgptImageInfoAllocator->SetBlockCount_Tracked(1024);









    for (int32 i = 0; sgtImageCollectionTable[i].eType != IMAGE_COLLECTION_INVALID; i++)
    {






        ImageCollectionTable & tTable = sgtImageCollectionTable[i];











        tTable.ptImageCollection = sImageCollectionCreate(tTable.eType, tTable.pszTexturePrefixString);
    }

    for (int32 i = 0; i < IMAGE_COLLECTION_NUM_TYPES; i++)
    {
        sLoadImageMapIfNecessary(i);
    }










}



static void sImageCollectionDispose(ImageCollection *& ptImageCollection)

{
    if (ptImageCollection)
    {

        POS posImageMap = ptImageCollection->mapImageMap.GetStartAssoc();
        while (posImageMap)
        {
            ImageInfo * ptImageInfo;
            ImageHandle hImage;
            ptImageCollection->mapImageMap.GetNextAssoc(posImageMap, hImage, ptImageInfo);



            sgptImageInfoAllocator->Free(ptImageInfo);
        }

        ptImageCollection->mapImageMap.RemoveAll();

        delete ptImageCollection; ptImageCollection = NULL;
    }
}



void ImageCollectionsDispose()

{
    for (int32 i = 0; i < IMAGE_COLLECTION_NUM_TYPES; i++)
    {
        sImageCollectionDispose(sgtImageCollectionTable[i].ptImageCollection);
        sgfLoaded[sgtImageCollectionTable[i].eType] = FALSE;
    }

    delete sgptImageInfoAllocator; sgptImageInfoAllocator = NULL;
}



void ImageCollectionsReset(
    const ImageCollectionType eCollection)
{




    ImageCollectionTable & tTable = sgtImageCollectionTable[eCollection];
    sImageCollectionDispose(tTable.ptImageCollection);

    tTable.ptImageCollection = sImageCollectionCreate(tTable.eType, tTable.pszTexturePrefixString);
    sImageCollectionLoad(*tTable.ptImageCollection);
}


















































BOOL ImageCollectionGetImageInfo(
    const ImageCollectionType eCollection,
    const ImageHandle hImage,
    ImageTextureFrame & tImageTexFrame)
{




    ImageCollection & tImageCollection = ImageCollectionGet(eCollection);


    ImageInfo * ptImageInfo;
    BOOL bResult = tImageCollection.mapImageMap.Lookup(hImage, ptImageInfo);


















    tImageTexFrame = bResult ? ptImageInfo->tImageTexFrame : cgtImageTextureFrameNull;






    return bResult;
}





BOOL ImageCollectionGetImageInfo(
    LPCSTR szDecoratedLabel,
    ImageTextureFrame & tImageTexFrame)
{

    if (!szDecoratedLabel[0])
    {
        return FALSE;
    }

    CHAR szImageCollection[128];
    CHAR szLabel[1024];

    if (!SplitDecoratedLabel(szDecoratedLabel, szImageCollection, szLabel))
    {
        return FALSE;
    }

    ImageCollectionType eCollection = NameToSymbol(szImageCollection, gImageCollections);
    if (eCollection == IMAGE_COLLECTION_INVALID)
    {
        return FALSE;
    }

    return ImageCollectionGetImageInfo(eCollection, szLabel, tImageTexFrame);
}




BOOL ImageCollectionGetImageInfo(
    const ImageCollectionType eCollection,
    const CHAR * szImageName,
    ImageTextureFrame & tImageTexFrame)
{

    if (!szImageName || !szImageName[0])
    {
        return FALSE;
    }

    ImageHandle hImage = StringToImageHandle(szImageName);

    return ImageCollectionGetImageInfo(eCollection, hImage, tImageTexFrame);
}






























































































































BOOL ImageCollectionApplyHotfix(
    const ImageCollectionType eCollection,
    const char * szHotfixName)
{
    unsigned char * pData;
    uint32 nDataSize;
    if (!sReadFile(szHotfixName, pData, nDataSize))
    {
        return FALSE;
    }


    if (*(uint32 *)pData != IMAGE_COLLECTION_FILE_MAGIC)
    {

        SigmaMemoryFree(pData, NULL); pData = NULL;
        return FALSE;
    }

    ImageCollection & tCollection = ImageCollectionGet(eCollection);

    const ImageCollectionItem * pItems = (const ImageCollectionItem *)(pData + sizeof(uint32));
    int32 nItemCount = (nDataSize - sizeof(uint32)) / sizeof(ImageCollectionItem);
    for (int32 i = 0; i < nItemCount; i++)
    {
        sImageCollectionAddItem(tCollection, pItems[i], TRUE);
    }

    SigmaMemoryFree(pData, NULL); pData = NULL;
    return TRUE;
}
