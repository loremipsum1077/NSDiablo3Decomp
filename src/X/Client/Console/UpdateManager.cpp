// Original file: X/Client/Console/UpdateManager.cpp
// Not written: ROOT_PATH (line 49), which has no storage in the image.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Contrib/Contrib/blz/include/blz/embedded_string.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../Common/Game.h"
#include "../../Common/Protobuf/Console.pb.h"
#include "../../Sigma/UnicodeText.h"
#include "Online.h"
// Declared here because its header is not adopted yet.
BOOL SNOFilesApplyHotfixFromMemory(LPCSTR szHotfixName, const uint8 * pHotfixData, int32 nHotfixSize, BOOL fValidateCRC, LPCSTR szHotfixLocaleName, const uint8 * pHotfixLocaleData, int32 nHotfixLocaleSize);  // X/Common/SNO/SNOFiles.cpp

namespace Console {
namespace UpdateManager {

void InitializeForApp(); void DisposeForApp(); void WriteUpdatesToDisk(); void DeleteUpdatesFromDisk(); int32 GetCurrentUpdateVersion(XLocale & eLocale);
void OnNewUpdateDownloaded(int32 nUpdateVersion, const char * szTOC, blz::string szUpdate, const char * szLocaleTOC, blz::string szLocaleUpdate); void RemoveUpdates(); void ApplyUpdates();

enum UpdateType {
    UPDATE_GLOBAL = 0,
    UPDATE_LOCALE = 1,
    NUM_UPDATE_TYPES = 2,
};












const char * CONFIG_FILENAME = "%s\\Config";
const char * UPDATE_CPK_FILENAME = "%s\\Update.cpk";
const char * UPDATE_LOCALE_CPK_FILENAME = "%s\\Locale.cpk";



struct UpdateFileData {  // size 0xc8

    UpdateFileData() : m_eLocale((XLocale)0)
    {
    }


    void Clear()
    {

        blz::string().swap(m_szData);
        m_eLocale = (XLocale)0;
        m_szTOC.clear();
    }
    blz::string m_szData;  // 0x00
    XLocale m_eLocale;  // 0x28
    blz::embedded_string<128> m_szTOC;  // 0x30
};


struct UpdateCache {  // size 0x198

    UpdateCache() : m_nVersion(0), m_bUpdateApplied(FALSE)
    {
    }


    void Clear()
    {

        m_nVersion = 0;
        for (int32 i = 0; i < NUM_UPDATE_TYPES; i++)
        {
            m_atUpdates[i].Clear();
        }
    }
    int32 m_nVersion;  // 0x00
    BOOL m_bUpdateApplied;  // 0x04
    UpdateFileData m_atUpdates[NUM_UPDATE_TYPES];  // 0x08
};




UpdateCache s_tUpdateCache;
Blizzard::Lock::RMutex s_tUpdateCacheMutex;

char s_szRootPath[4096];






void InitializeForApp()

{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);
}














void DisposeForApp()

{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);

    for (int32 i = 0; i < NUM_UPDATE_TYPES; i++)
    {
        s_tUpdateCache.m_atUpdates[i].Clear();
    }
}

// Linker-dropped (DWARF concrete DIE at low_pc 0, no bytes to compare): written from its DWARF locals,
// inline tree and call lines, mirroring WriteUpdatesToDisk. Its two m_szTOC assignments are what keep
// embedded_string<128>::operator=(const char *) out of line in OnNewUpdateDownloaded, as in the original.
void LoadUpdatesFromDisk()
{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);

    // inferred: same root-path guard as WriteUpdatesToDisk / DeleteUpdatesFromDisk
    if (s_szRootPath[0] == '\0')
    {
        return;
    }

    char szPath[4096];
    Blizzard::File::Stream tStream;
    D3::Console::BalanceUpdateConfig tConfig;

    Blizzard::String::Format(szPath, sizeof(szPath), CONFIG_FILENAME, s_szRootPath);
    // inferred: read-mode flags 0x1001 as in GamerProfileNX64.cpp:2317
    if (!Blizzard::File::Open(szPath, 0x1001, tStream))
    {
        return;
    }

    // inferred: lexical block holding abBuffer (161) / nSizeRead (162)
    {
        char abBuffer[1024];
        int32 nSizeRead = sizeof(abBuffer);
        BOOL bRead = Blizzard::File::Read(tStream, abBuffer, 0, &nSizeRead, 0) && tConfig.ParseFromArray(abBuffer, nSizeRead);
        Blizzard::File::Close(tStream);
        if (!bRead)
        {
            return;
        }
    }

    if (!tConfig.has_title_version())
    {
        return;
    }





    // inferred: 265 is the title version WriteUpdatesToDisk stores
    if (tConfig.title_version() != 265)
    {

        return;
    }


    s_tUpdateCache.m_nVersion = tConfig.version();
    UpdateFileData & tUpdate = s_tUpdateCache.m_atUpdates[UPDATE_GLOBAL];

    tUpdate.m_szTOC = tConfig.toc().c_str();

    BOOL bSuccess = FALSE;
    Blizzard::String::Format(szPath, sizeof(szPath), UPDATE_CPK_FILENAME, s_szRootPath);
    if (Blizzard::File::Open(szPath, 0x1001, tStream))
    {


        int64 uSize = Blizzard::File::GetFileInfo(tStream)->size;


        bSuccess = Blizzard::File::Read(tStream, tUpdate.m_szData.modify_data(uSize), (int32)uSize);
        Blizzard::File::Close(tStream);
    }



    // inferred: the DWARF shows no has_locale() here (only locale() at 214), so the block is guarded by bSuccess alone
    if (bSuccess)
    {

        UpdateFileData & tUpdateLocale = s_tUpdateCache.m_atUpdates[UPDATE_LOCALE];

        tUpdateLocale.m_eLocale = XLocaleFromString(tConfig.locale().c_str());
        tUpdateLocale.m_szTOC = tConfig.locale_toc().c_str();

        Blizzard::String::Format(szPath, sizeof(szPath), UPDATE_LOCALE_CPK_FILENAME, s_szRootPath);
        bSuccess = Blizzard::File::Open(szPath, 0x1001, tStream);
        if (bSuccess)
        {
            int64 uSize = Blizzard::File::GetFileInfo(tStream)->size;


            bSuccess = Blizzard::File::Read(tStream, tUpdateLocale.m_szData.modify_data(uSize), (int32)uSize);
            Blizzard::File::Close(tStream);
        }
    }



    if (!bSuccess)
    {

        s_tUpdateCache.Clear();
    }




}



// NON_MATCHING: 4 bytes short. On the locale write path the original keeps Write's result in a second register and masks it after Close (mov w19, w0 ... and w20, w19, #1); ours masks it before Close.
void WriteUpdatesToDisk()

{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);

    if (s_szRootPath[0] == '\0')
    {
        return;
    }


    D3::Console::BalanceUpdateConfig tConfig;

    UpdateFileData & tUpdate = s_tUpdateCache.m_atUpdates[UPDATE_GLOBAL];
    UpdateFileData & tUpdateLocale = s_tUpdateCache.m_atUpdates[UPDATE_LOCALE];

    tConfig.set_title_version(277);
    tConfig.set_version(s_tUpdateCache.m_nVersion);
    tConfig.set_toc(tUpdate.m_szTOC);
    if (!tUpdateLocale.m_szData.empty())
    {
        tConfig.set_locale(XLocaleToString(tUpdateLocale.m_eLocale));
        tConfig.set_locale_toc(tUpdateLocale.m_szTOC);
    }

    Blizzard::File::Stream tStream;
    char szPath[4096];

    Blizzard::File::CreateDirectory(s_szRootPath, false);

    Blizzard::String::Format(szPath, sizeof(szPath), CONFIG_FILENAME, s_szRootPath);
    BOOL bOk = Blizzard::File::Open(szPath, 0x402, tStream);
    if (bOk)
    {
        char abBuffer[1024];
        int32 nConfigSize = tConfig.ByteSize();


        bOk = nConfigSize <= sizeof(abBuffer) && tConfig.SerializeToArray(abBuffer, nConfigSize);
        bOk = bOk && Blizzard::File::Write(tStream, abBuffer, nConfigSize);
        Blizzard::File::Close(tStream);
    }






    Blizzard::String::Format(szPath, sizeof(szPath), UPDATE_CPK_FILENAME, s_szRootPath);
    bOk = bOk && Blizzard::File::Open(szPath, 0x402, tStream);
    if (bOk)
    {
        bOk &= Blizzard::File::Write(tStream, tUpdate.m_szData.data(), (int32)tUpdate.m_szData.size());
        Blizzard::File::Close(tStream);
    }






    if (bOk)
    {
        if (!tUpdateLocale.m_szData.empty())
        {
            Blizzard::String::Format(szPath, sizeof(szPath), UPDATE_LOCALE_CPK_FILENAME, s_szRootPath, XLocaleToString(tUpdateLocale.m_eLocale));
            if (Blizzard::File::Open(szPath, 0x402, tStream))
            {

                bOk &= Blizzard::File::Write(tStream, tUpdateLocale.m_szData.data(), (int32)tUpdateLocale.m_szData.size());
                Blizzard::File::Close(tStream);
            }
            else
            {
                bOk = FALSE;
            }
        }
        else
        {


            Blizzard::String::Format(szPath, sizeof(szPath), UPDATE_LOCALE_CPK_FILENAME, s_szRootPath);
            Blizzard::File::Delete(szPath);
        }
    }


    if (!bOk)
    {
        DeleteUpdatesFromDisk();
    }
}








void DeleteUpdatesFromDisk()

{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);

    if (s_szRootPath[0] == '\0')
    {
        return;
    }


    char szPath[4096];
    Blizzard::String::Format(szPath, sizeof(szPath), CONFIG_FILENAME, s_szRootPath);
    Blizzard::File::Delete(szPath);

    Blizzard::String::Format(szPath, sizeof(szPath), UPDATE_CPK_FILENAME, s_szRootPath);
    Blizzard::File::Delete(szPath);

    Blizzard::String::Format(szPath, sizeof(szPath), UPDATE_LOCALE_CPK_FILENAME, s_szRootPath);
    Blizzard::File::Delete(szPath);
}















int32 GetCurrentUpdateVersion(XLocale & eLocale)

{
    eLocale = s_tUpdateCache.m_atUpdates[UPDATE_LOCALE].m_eLocale;
    return s_tUpdateCache.m_nVersion;
}




void OnNewUpdateDownloaded(int32 nUpdateVersion, const char * szTOC, blz::string szUpdate, const char * szLocaleTOC, blz::string szLocaleUpdate)





{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);

    s_tUpdateCache.m_nVersion = nUpdateVersion;

    UpdateFileData & tUpdate = s_tUpdateCache.m_atUpdates[UPDATE_GLOBAL];
    tUpdate.m_szTOC = szTOC;
    tUpdate.m_szData = blz::move(szUpdate);

    UpdateFileData & tUpdateLocale = s_tUpdateCache.m_atUpdates[UPDATE_LOCALE];
    tUpdateLocale.m_szTOC = szLocaleTOC;
    tUpdateLocale.m_szData = blz::move(szLocaleUpdate);
    tUpdateLocale.m_eLocale = UnicodeText::GetCurrentLocale();

    Online::QueueNetworkEventForMainThread(&WriteUpdatesToDisk);
}




void RemoveUpdates()

{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);

    s_tUpdateCache.Clear();

    Online::QueueNetworkEventForMainThread(&DeleteUpdatesFromDisk);
}




void ApplyUpdates()

{
    Blizzard::Lock::ScopedRMutex tLock(s_tUpdateCacheMutex);

    if (s_tUpdateCache.m_bUpdateApplied
        || s_tUpdateCache.m_nVersion == 0
        || s_tUpdateCache.m_atUpdates[UPDATE_GLOBAL].m_szData.empty())
    {
        return;
    }

    UpdateFileData & tUpdate = s_tUpdateCache.m_atUpdates[UPDATE_GLOBAL];
    UpdateFileData & tUpdateLocale = s_tUpdateCache.m_atUpdates[UPDATE_LOCALE];


    uint8 * szData = NULL;
    uint8 * szLocaleData = NULL;


    szData = (uint8 *)SigmaMemoryNew(tUpdate.m_szData.size(), 4, NULL, 0);
    SigmaMemoryCopy(szData, tUpdate.m_szData.data(), tUpdate.m_szData.size());

    if (!tUpdateLocale.m_szData.empty())
    {
        szLocaleData = (uint8 *)SigmaMemoryNew(tUpdateLocale.m_szData.size(), 4, NULL, 0);
        SigmaMemoryCopy(szLocaleData, tUpdateLocale.m_szData.data(), tUpdateLocale.m_szData.size());
    }

    SNOFilesApplyHotfixFromMemory(
        tUpdate.m_szTOC.c_str(),
        szData, (int32)tUpdate.m_szData.size(),
        TRUE,
        tUpdateLocale.m_szTOC.c_str(),
        szLocaleData,
        (int32)tUpdateLocale.m_szData.size());


    blz::string().swap(tUpdate.m_szData);
    blz::string().swap(tUpdateLocale.m_szData);

    s_tUpdateCache.m_bUpdateApplied = TRUE;
}

}  // namespace UpdateManager
}  // namespace Console
