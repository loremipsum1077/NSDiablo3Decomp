// Original file: X/Client/Console/DLCManager.cpp
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../NintendoSDK/Include/nn/os/os_SystemEvent.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "GamerProfileInternal.h"
#include "../../Common/Console/ConsoleLicenses.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/UnicodeText.h"
#include "../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../Contrib/Contrib/blz/include/blz/atomic.h"
#include "../../../Contrib/Contrib/blz/include/blz/format.h"
#include "../../../Contrib/Contrib/blz/include/blz/map.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

// NintendoSDK declarations whose headers are not in src yet (nn/aoc/aoc_Types.h, nn/aoc, nn/fs); signatures from the import names.
namespace nn { namespace aoc { typedef int AddOnContentIndex; int CountAddOnContent(); int ListAddOnContent(AddOnContentIndex * outIndices, int offset, int count); void GetAddOnContentListChangedEvent(nn::os::SystemEvent * outEvent); } namespace fs { void Unmount(const char * name); nn::Result QueryMountAddOnContentCacheSize(size_t * pOutSize, nn::aoc::AddOnContentIndex targetIndex); nn::Result MountAddOnContent(const char * name, nn::aoc::AddOnContentIndex targetIndex, void * pCacheBuffer, size_t cacheBufferSize); } }

namespace Console {
namespace DLCManager {

void RefreshAOC();
void AddContent(const char * szContentName);
bool GetLicenseContentIndex(ConsoleLicense uLicense, int64_t & nContentIndex);
bool GetLocaleContentLicense(XLocale nLocale, ConsoleLicense & uLicense);
CRefString GetAOCMountName(int64_t nContentIndex);

struct DLCPackageToLicense {  // size 0x18
    ConsoleLicense uLicense;  // 0x00
    const char * szDLCPackageID;  // 0x08
    nn::aoc::AddOnContentIndex nContentIndex;  // 0x10
};
static_assert(sizeof(DLCPackageToLicense) == 0x18, "DLCPackageToLicense");













static const DLCPackageToLicense g_DLCPackageToLicense[16] =
{
    { LICENSE_LOC_AUDIO_ENGLISH_EU, "loc.audio.english_eu", 1 },
    { LICENSE_LOC_AUDIO_ENGLISH_SOUTHEAST_ASIA, "loc.audio.english_southeast_asia", 2 },
    { LICENSE_LOC_AUDIO_SPANISH_EU, "loc.audio.spanish_eu", 3 },
    { LICENSE_LOC_AUDIO_SPANISH_LATAM, "loc.audio.spanish_latam", 4 },
    { LICENSE_LOC_AUDIO_FRENCH, "loc.audio.french", 5 },
    { LICENSE_LOC_AUDIO_ITALIAN, "loc.audio.italian", 6 },
    { LICENSE_LOC_AUDIO_GERMAN, "loc.audio.german", 7 },
    { LICENSE_LOC_AUDIO_KOREAN, "loc.audio.korean", 8 },
    { LICENSE_LOC_AUDIO_PORTUGUESE_BRAZIL, "loc.audio.portuguese_brazil", 9 },
    { LICENSE_LOC_AUDIO_RUSSIAN, "loc.audio.russian", 10 },
    { LICENSE_LOC_AUDIO_CHINESE_CHINA, "loc.audio.chinese_china", 11 },
    { LICENSE_LOC_AUDIO_CHINESE_TAIWAN, "loc.audio.chinese_taiwan", 12 },
    { LICENSE_LOC_AUDIO_TURKISH, "loc.audio.turkish", 13 },
    { LICENSE_LOC_AUDIO_POLISH, "loc.audio.polish", 14 },
    { LICENSE_LOC_AUDIO_PORTUGUESE_PORTUGAL, "loc.audio.portuguese_portugal", 15 },
    { LICENSE_LOC_AUDIO_JAPANESE, "loc.audio.japanese", 16 },
};

struct LocaleContentInfo {  // size 0x8
    XLocale nLocale;  // 0x00
    ConsoleLicense uLicense;  // 0x04
};
static_assert(sizeof(LocaleContentInfo) == 0x8, "LocaleContentInfo");











static const LocaleContentInfo s_catLocaleInfoTable[16] =
{
    { 3, LICENSE_LOC_AUDIO_ENGLISH_EU },
    { 4, LICENSE_LOC_AUDIO_ENGLISH_SOUTHEAST_ASIA },
    { 5, LICENSE_LOC_AUDIO_SPANISH_EU },
    { 6, LICENSE_LOC_AUDIO_SPANISH_LATAM },
    { 7, LICENSE_LOC_AUDIO_FRENCH },
    { 8, LICENSE_LOC_AUDIO_ITALIAN },
    { 9, LICENSE_LOC_AUDIO_GERMAN },
    { 10, LICENSE_LOC_AUDIO_KOREAN },
    { 11, LICENSE_LOC_AUDIO_PORTUGUESE_BRAZIL },
    { 12, LICENSE_LOC_AUDIO_RUSSIAN },
    { 13, LICENSE_LOC_AUDIO_CHINESE_CHINA },
    { 14, LICENSE_LOC_AUDIO_CHINESE_TAIWAN },
    { 15, LICENSE_LOC_AUDIO_TURKISH },
    { 16, LICENSE_LOC_AUDIO_POLISH },
    { 17, LICENSE_LOC_AUDIO_PORTUGUESE_PORTUGAL },
    { 18, LICENSE_LOC_AUDIO_JAPANESE },
};

struct AOCMountInfo {  // size 0x10
    size_t nCacheSize;  // 0x00
    void * pMountCacheBuffer;  // 0x08
};
static_assert(sizeof(AOCMountInfo) == 0x10, "AOCMountInfo");

class SearchForDLCTask : public Console::GamerProfile::Internal::SaveLoadTask {  // size 0x18
public:





    virtual ~SearchForDLCTask() {}


    SearchForDLCTask(Console::GamerProfile::Internal::SaveLoadTaskCallback pfnCallback)

        : m_pfnCallback(pfnCallback)
    {
    }
    virtual void Process();  // vtable slot 2
    Console::GamerProfile::Internal::SaveLoadTaskCallback m_pfnCallback;  // 0x10
};
static_assert(sizeof(SearchForDLCTask) == 0x18, "SearchForDLCTask");












static const int64_t MAX_MOUNT_PATH = 16;
static const int64_t s_nMaxListupCount = 256;
nn::aoc::AddOnContentIndex s_atAocListupBuffer[s_nMaxListupCount];
blz::map<long, AOCMountInfo> s_tAocMountCacheBuffers;


blz::atomic_uint32_t s_uLicenses(0);







void InitializeForApp()
{

























    RefreshAOC();
}





void DisposeForApp()
{
}

void SearchForContent();










void Process()

{









    nn::os::SystemEvent tAddOnContentListChangedEvent;
    nn::aoc::GetAddOnContentListChangedEvent(&tAddOnContentListChangedEvent);
    if (tAddOnContentListChangedEvent.TryWait())
    {
        SearchForContent();
    }
}





void SearchForDLCTask::Process()
{












































































































    RefreshAOC();



    if (m_pfnCallback)
    {
        Console::GamerProfile::Internal::QueueCallback(m_pfnCallback);
    }
}




void SearchForContent()
{

    SearchForDLCTask * ptTask = new SearchForDLCTask(Console::GamerProfile::Internal::SaveLoadTaskCallback());
    ptTask->Queue();
}




void AddContent(const char * szContentName)

{
    for (const DLCPackageToLicense & tPackageEntry : g_DLCPackageToLicense)
    {
        if (strcasecmp(szContentName, tPackageEntry.szDLCPackageID) == 0)
        {

            s_uLicenses |= (1 << tPackageEntry.uLicense);
            break;
        }
    }
}




uint32 GetLicenses()
{

    uint32 uLicenses = s_uLicenses;



    uLicenses |= (1 << LICENSE_NECROMANCER);










    return uLicenses;
}



void RefreshAOC()

{
    CRefString szMountName;


    for (auto iter = s_tAocMountCacheBuffers.begin(); iter != s_tAocMountCacheBuffers.end(); ++iter)
    {
        int64_t nContentIndex = iter->first;
        AOCMountInfo & tMountInfo = s_tAocMountCacheBuffers[nContentIndex];


        szMountName = GetAOCMountName(nContentIndex);


        nn::fs::Unmount(szMountName.str());
        free(tMountInfo.pMountCacheBuffer);
    }
    s_tAocMountCacheBuffers.clear();


    int nAocCount = nn::aoc::CountAddOnContent();



    int nListupCount = nn::aoc::ListAddOnContent(s_atAocListupBuffer, 0, s_nMaxListupCount);







    if (nListupCount > 0) for (int i = 0; i < nListupCount; i++)
    {

        nn::aoc::AddOnContentIndex nContentIndex = s_atAocListupBuffer[i];


        for (const DLCPackageToLicense & tPackageEntry : g_DLCPackageToLicense)
        {
            if (tPackageEntry.nContentIndex == nContentIndex)
            {
                AddContent(tPackageEntry.szDLCPackageID);
                break;
            }
        }


        szMountName = GetAOCMountName(nContentIndex);


        size_t nCacheSize = 0;
        nn::fs::QueryMountAddOnContentCacheSize(&nCacheSize, nContentIndex);

        void * const pMountCacheBuffer = malloc(nCacheSize);

        s_tAocMountCacheBuffers[nContentIndex] = { nCacheSize, pMountCacheBuffer };

        nn::Result tResult = nn::fs::MountAddOnContent(
            szMountName.str(), nContentIndex, pMountCacheBuffer, nCacheSize);
    }


}

bool GetLicenseContentIndex(ConsoleLicense uLicense, int64_t & nContentIndex)
{
    bool bFound = false;


    for (const DLCPackageToLicense & tPackageEntry : g_DLCPackageToLicense)
    {
        if (tPackageEntry.uLicense == uLicense)
        {

            nContentIndex = tPackageEntry.nContentIndex;
            bFound = true;
            break;
        }
    }
    return bFound;
}
bool GetLocaleContentLicense(XLocale nLocale, ConsoleLicense & uLicense)
{
    bool bFound = false;


    for (const LocaleContentInfo & tLocaleInfo : s_catLocaleInfoTable)
    {
        if (tLocaleInfo.nLocale == nLocale)
        {

            uLicense = tLocaleInfo.uLicense;
            bFound = true;
            break;
        }
    }
    return bFound;
}
bool GetLocaleContentIndex(XLocale nLocale, int64_t & nContentIndex)
{
    bool bFound = false;
    ConsoleLicense uLicense;


    if (GetLocaleContentLicense(nLocale, uLicense))
    {
        bFound = GetLicenseContentIndex(uLicense, nContentIndex);
    }
    return bFound;
}

CRefString GetAOCMountName(int64_t nContentIndex)
{
    char szPathString[MAX_MOUNT_PATH];

    blz::snprintf(szPathString, MAX_MOUNT_PATH, "aoc_%d", nContentIndex);
    return CRefString(szPathString, MAX_MOUNT_PATH);
}

bool GetLocaleAOCMountPath(XLocale nLocale, CRefString & szMountPath)


{
    char szPotentialDataRootLocation[4096];
    bool bFound = false;


    sprintf(szPotentialDataRootLocation, "%s\\%s_Common.%s", SNOFilesGetMPQPath().str(), XLocaleToString(nLocale), SNOFilesGetMPQExtension());
    BOOL bInstalledToDataRoot = Blizzard::File::Exists(szPotentialDataRootLocation);
    int64_t nContentIndex;
    if (bInstalledToDataRoot)
    {
        bFound = true;
        szMountPath = SNOFilesGetMPQPath();
    }
    else if (GetLocaleContentIndex(nLocale, nContentIndex))
    {
        char szMpqName[4096];
        bFound = true;

        sprintf(szMpqName, "%s:", GetAOCMountName(nContentIndex).str());
        szMountPath = szMpqName;
    }
    else
    {
        szMountPath = "";
    }
    return bFound;
}

}  // namespace DLCManager
}  // namespace Console
