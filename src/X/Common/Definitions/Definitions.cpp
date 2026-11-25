// Original file: X/Common/Definitions/Definitions.cpp

#include "Definitions.h"

#include "../../../BlizzardCore/Include/Blizzard/Sys/Atomic.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../SNO/SNOName.h"
#include "../SNO/SNOTOC.h"
#include "../../Sigma/Files/XFiles.h"
#include "../../Sigma/UnicodeText.h"
#include "../../Sigma/RefString.h"
#include "../ThreadGlobals.h"
#include "../../Sigma/Stopwatch.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Sigma/DataStructures/XVirtualHeap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Client/Gfx/GfxVB.h"  // the original CU initializes the GfxVB.h:351-354 normals (it reaches them through Game.h)
// Defined in X/Common/Game.cpp:3357.
GroupDefinition * GameDefinitionGet(const SNOGroup snoGroup);

// Defined in X/Common/SNO/SNOGroups.cpp:72 and :124.
void SNOGroupRegister(SNOGroupInfo & snogroup, const XTypeDescriptor * dt);
const SNOGroupInfo * SNOGroupGetInfo(const SNOGroup snoGroup);

// Defined in X/Common/SNO/SNOFiles.cpp; SNOFiles.h does not declare them yet.
void SNOFileAddEntry(SNOTOC::TOCType eType, const SNOGroup snoGroup, const uint32 uGuid, LPCSTR szFileNameNoExtension);
BOOL SNOFileWriteHeader(FileReference & tRef, SNOElement * ptSNOElement, uint32 dwXMLHash);
CRefString SNOFileGetName(const SNOName & snoname);
XLocale SNOFileGetLocale(const SNOName & snoname);
void SNOFileGetFolderForNewLocalCacheFile(FileReference & tRef, const XLocale eXLocale, const SNOGroup snoGroup);
CRefString SNOFilesGetPlatformDataPath(const SigmaPlatform eSigmaPlatform);
void SNOFixFilename(LPSTR szName);
void SNOFileClose(SNOElement * ptSNOElement);
const char * SNOGetGroupExtension(const SNOGroup snoGroup);
SNOElement * SNOFileLookup(const SNOName & snoname);
void SNOFileRead(SNOElement * ptSNOElement, const uint32 dwOffset, const uint32 dwSize, void * lpBuffer);
void SNOFileWrite(FileReference & tRef, const void * lpBuffer, const uint32 dwOffset, const uint32 dwSize);
BOOL SNOFileDelete(const SNOName & snoname, const BOOL fKeepInFileMap);
BOOL SNOFileExistsOnDisk(const SNOName & snoname, const BOOL fCompileIfNeccessary);
BOOL SNOFileTryGetVersion(const SNOName & snoname, uint32 * dwVersion, uint32 * dwAssetVersion, uint32 * dwXMLHash);
uint64 SNOFileGetFileHash(SNOElement * ptSNOElement, BOOL fWithExtension);
uint32 SNOFileGetDataSize(SNOElement * ptSNOElement, const BOOL fUncompressedSize);
CRefString SNOToString(const SNOGroup snoGroup, const SNO snoHandle, const BOOL fUsedForXMLFields);
CRefString SNONameToString(const SNOGroup snoGroup, const SNO snoHandle, const BOOL fUsedForXMLFields);
uint32 SNOFilesGetTOCCountForGroup(const SNOGroup snoGroup);
SNO SNOFilesGetHighestUsedSNOHandle();

// X/Common/SNO/SNOML.h:6; the rest of that header is not adopted yet.
enum SNOMLResult {
    SNOML_NOFILECREATED = 0,
    SNOML_FILEEXISTS = 1,
    SNOML_FILECREATED = 2,
    SNOML_READONLY = 3,
    SNOML_FAILED = 4,
};

// Defined in X/Common/SNO/SNOML_Shared.cpp and SNOML_X.cpp.
const XTypeDescriptor * SNOMLGetTypeFromSNOGroup(SNOGroup snoGroup);
void SNOMLDelete(const SNOName & snoname);
SNOMLResult SNOMLUpdate(const SNOName &, const void *, LPCSTR);
SNOElement * SNOMLFileOpenForRead(const SNOName & snoname);
XLocale SNOMLFileGetLocale(const SNOName &);
void SNOMLMakeXMLPath(const XLocale eXLocale, SNOGroup snoGroup, LPCSTR pszFileName, CRefString & sPath);
BOOL SNODeleteAdditionalFiles(const char * szPath);
BOOL SNOTransferAdditionalFiles(const char *, const char *, Blizzard::File::ProcessDirCallback);
bool SNOMoveAdditionalFile(const Blizzard::File::ProcessDirParms &);

// Defined in X/Sigma/Files/XFilesCore.cpp; no adopted header declares them yet.
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
void FileReferenceAddDirectory(FileReference & tFileRef, LPCSTR szDirectory);
BOOL FileDirectoryCreate(FileReference & tFileRef, const BOOL fCreateSubDirs);
BOOL FileCreate(FileReference & tFileRef);
BOOL FileDelete(FileReference & tFileRef);
BOOL FileMove(FileReference & tFileRefSrc, FileReference & tFileRefDest);
BOOL FileExists(const FileReference & tFileRef);
BOOL FileOpen(FileReference & tFileRef, uint32 dwAccessFlags);
void FileClose(FileReference & tFileRef);






// External linkage, as in the original (it is reached through the GOT).
extern const CHAR cgszBadDataName[];



























const CHAR cgszBadDataName[] = "Axe Bad Data";
int32 gGroupDefinition_nWritingServerDefinitionRefCount;

// The class statics are defined in the order of their storage in the original.
GlobalPurgeableItem GroupDefinition::spGlobalPurgeArray[2][512];
uint32 GroupDefinition::sdwGlobalPurgeArrayCount[2];



Blizzard::Lock::RMutex GroupDefinition::stGlobalCacheLock[2];
Blizzard::Lock::RMutex GroupDefinition::stGlobalInstanceLock;
TDataArray<DefDataElement, DefDataElementID> * GroupDefinition::sptGlobalInstances;
uint32 GroupDefinition::sdwSNOInArray;
DefDataElementID * GroupDefinition::spptDefinitionArray;
DefDataElementID GroupDefinition::spptDynamicDefinitionArray[10000];

















DefinitionHeapAutoPurgeAlloc stDefinitionHeapAutoPurgeAlloc;


















































uint32 GroupDefinition::sdwLowestUnusedDynamicHandle = (uint32)-10001;
Blizzard::Lock::RMutex GroupDefinition::stDynamicDefinitionLock;
Blizzard::Lock::RMutex GroupDefinition::stDynamicDefinitionHandleLock;

GroupDefinition::MapSNOToDefDataElementID GroupDefinition::stOverflowDefinitions(16, NULL);


__thread GlobalCacheType geGlobalCachePurgeOverride = GLOBALCACHE_COUNT;


static BOOL sgbEnableLoadTrace;
static BOOL sgbEnablePreCaching;
static float sgflLoadTraceMinTime;

// X/Common/SNO/SNOFilesAsync.h and SNOFilesAsync.cpp:896, :1014 and :1843; not adopted yet.
enum AsyncStatus {
    e_pending = 0,
    e_successful = 1,
    e_failed = 2,
    e_canceled = 3,
};
AsyncStatus SNOFilesGetAsyncStatus(SNOElement * ptSNOElement);
void SNOFilesCloseAsyncRecord(SNOElement * ptSNOElement);
BOOL SNOFilesAsyncIsLocking(SNOElement * ptSNOElement);

// Defined in X/Sigma/ErrorManager/ErrorManager.cpp:836.
BOOL ErrorManagerGetSuppressUserInteraction();

// X/Client/SNOManager.h and X/Client/Prefetch.h; neither is adopted yet.
namespace SNOManager {
void OnSNOLoaded(const SNOName & snoName, RequiredDefinitionHeader * lpDefinition);
void SetHasBlockedThisFrame();
}  // namespace SNOManager
namespace Prefetch {
BOOL IsLockingDefinition();
BOOL TakeSNOElement(const SNOName & snoname, SNOElement *& ptSNOElement);
}  // namespace Prefetch

// Defined in X/Client/CGame.cpp:6913.
BOOL CGameAsyncRenderFlush();

// Defined in X/Common/GlobalSNO.cpp:101.
void GlobalSNOListRefresh();

// Defined in BlizzardCore/Source/System/Process/Process_NX64.cpp:25.
namespace Blizzard {
namespace Process {
bool Sleep(Blizzard::Time::Millisecond milliseconds);
}  // namespace Process
}  // namespace Blizzard

// X/Common/Console/IO/ConsoleIO.h:113 and ConsoleIO.cpp:844; that header is not adopted yet.
namespace Console {
namespace IO {
class CPKArchiveReader;
struct FileInfo {  // size 0x18
    uint64 dwHash;  // 0x00
    int32 nSize;  // 0x08
    int32 nLocationCount;  // 0x0c
    int32 nLocationIndex;  // 0x10
    int32 nLocationIndexOverride;  // 0x14
};
CPKArchiveReader * FindArchiveContaining(uint64 dwHash, FileInfo * ptFileInfo);
}  // namespace IO
}  // namespace Console
// X/Client/Prefetch.h; Prefetch::Cancel is defined in X/Client/Prefetch.cpp.
namespace Prefetch {
void Cancel(const SNOName & snoname);
}  // namespace Prefetch
// Defined in X/Common/Shell/CmdLine.cpp:1766. The flags tested are CMDLINE_USE_ASSETSERVER (43)
// and CMDLINE_DO_NOT_CLEAR_LOOSE_FILES (44) from X/Common/Shell/CmdLine.h, which is not adopted.
BOOL CmdLineTestFlag(uint32 dwBitToTest);
// X/Client/CGame.h:145 and :219. Only the members read here are declared; the rest of both
#include "../../Client/CGame.h"










// X/Common/Game.h:543 and Game.cpp:412. AppGlobals is 0x23a8 bytes of mostly unadopted types;
#include "../Game.h"






extern AppGlobals sgtAppGlobals;
// Defined in X/Common/Game.cpp:987, :1045 and :1122.
BOOL _GameServerCodeEnter(const SGameID idSGame);
BOOL GameServerCodeEnter_Track();
SGameID GameServerCodeLeave_Tracked(BOOL bReturnToClient);
// Defined in X/Client/CGame.cpp:6147.
BOOL CGameLoadingScreenIsEnabled();
// Defined at line 6618 below.
BOOL DefinitionCurrentlyPreCaching();
static inline uint32 GetCurrentFrame(BOOL fReadOnly)
{

    return sgtAppGlobals.dwFrameCounter;
}














GlobalCachePurgeOverride::GlobalCachePurgeOverride(GlobalCacheType eType)
{
    geGlobalCachePurgeOverride = eType;
}



GlobalCachePurgeOverride::~GlobalCachePurgeOverride()
{
    geGlobalCachePurgeOverride = GLOBALCACHE_COUNT;
}

















void sDefinitionGetAllocDetails(GlobalCacheType eType, XBaseHeap *& ptHeap, uint32 & dwSize, uint32 & dwAlign)
{





    ptHeap = (eType == GLOBALCACHE_MAIN) ? (XBaseHeap *)gptMainDefinitionsMemoryHeap : gptSecondaryDefinitionsMemoryHeap;
}






void * sDefinitionTryMemAllocWithPurgeTracked(GlobalCacheType eType, XBaseHeap * ptHeap, uint32 dwSize, uint32 dwAlign, BOOL fClear, BOOL fMemoryIsCritical, const char * szFile, uint32 dwLine)
{








    void * pMem = ptHeap->MemTryAllocTracked(dwSize, dwAlign, fClear, szFile, dwLine);
    while (pMem == NULL)
    {





        GlobalCacheType ePurgeType = eType;
        if (geGlobalCachePurgeOverride != GLOBALCACHE_COUNT)
            ePurgeType = geGlobalCachePurgeOverride;



        if (!GroupDefinition::GlobalCacheManagerPurge(ePurgeType, TRUE, fMemoryIsCritical))
        {

            if (!GroupDefinition::GlobalCacheManagerPurge((ePurgeType == GLOBALCACHE_MAIN) ? GLOBALCACHE_SECONDARY : GLOBALCACHE_MAIN, TRUE, fMemoryIsCritical))
                return NULL;
        }


        pMem = ptHeap->MemTryAllocTracked(dwSize, dwAlign, fClear, szFile, dwLine);
    }









    return pMem;
}



void * sDefinitionTryMemAllocTracked(GlobalCacheType eType, uint32 dwSize, uint32 dwAlign, BOOL fClear, BOOL fMemoryIsCritical, const char * szFile, uint32 dwLine)
{









    XBaseHeap * ptHeap;
    sDefinitionGetAllocDetails(eType, ptHeap, dwSize, dwAlign);
    void * pMem = sDefinitionTryMemAllocWithPurgeTracked(eType, ptHeap, dwSize, dwAlign, fClear, fMemoryIsCritical, szFile, dwLine);
    return pMem;
}








void * DefinitionMemAllocTracked(GlobalCacheType eType, uint32 dwSize, uint32 dwAlign, BOOL fClear, const char * szFile, uint32 dwLine)
{






    void * pMem = sDefinitionTryMemAllocTracked(eType, dwSize, dwAlign, fClear, FALSE, szFile, dwLine);
    if (pMem != NULL)
        return pMem;


    GroupDefinition::GlobalCacheManagerClearPurgeLists(eType);
    return sDefinitionTryMemAllocTracked(eType, dwSize, dwAlign, fClear, TRUE, szFile, dwLine);
}






























void DefinitionMemFree(GlobalCacheType eType, void * pMem)
{
    XBaseHeap * ptHeap = (eType == GLOBALCACHE_MAIN) ? (XBaseHeap *)gptMainDefinitionsMemoryHeap : gptSecondaryDefinitionsMemoryHeap;




    ptHeap->MemFree(pMem);
}
































void * DefinitionHeapAutoPurgeAlloc::MemAllocTracked(size_t size, size_t align, BOOL bClear, const char * szFile, uint32 dwLine)
{





    return DefinitionMemAllocTracked(GLOBALCACHE_MAIN, (uint32)size, (uint32)align, bClear, szFile, dwLine);
}

void DefinitionHeapAutoPurgeAlloc::MemFree(void * pMem)
{


    DefinitionMemFree(GLOBALCACHE_MAIN, pMem);
}






GroupDefinition::GroupDefinition(LPCSTR szName, const SNOGroup snoGroup, const SNOVersions tCurrentVersions, const SNODataType eSNODataType, const uint32 dwInstanceSize, const XTypeDescriptor * ptTypeDesc, uint32 dwSNOGroupFlags, uint32 dwFlags)
{
















    if (dwSNOGroupFlags & (1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_GLOBAL_ON_HIGH_MEM_SYSTEM))
    {

        dwSNOGroupFlags = (dwSNOGroupFlags & ~0x1e) | (1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_GLOBAL);
    }



    if (dwSNOGroupFlags & (1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_PER_ACT_ON_HIGH_MEM_SYSTEM))
    {

        dwSNOGroupFlags = (dwSNOGroupFlags & ~0x1e) | (1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_PER_ACT);
    }



    if (dwSNOGroupFlags & (1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_GLOBAL_ON_HIGH_MEM_SYSTEM))
    {

        dwSNOGroupFlags &= ~0x1e;
        dwSNOGroupFlags |= (1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_GLOBAL);
    }


    if (dwSNOGroupFlags & (1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_PER_ACT_ON_HIGH_MEM_SYSTEM))
    {


        dwSNOGroupFlags &= ~0x1e;
        dwSNOGroupFlags |= (1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_PER_ACT);
    }




























    Blizzard::String::Copy(snoGroupInfo.szName, szName, sizeof(snoGroupInfo.szName));
    snoGroupInfo.snoGroup = snoGroup;
    snoGroupInfo.tVersions = tCurrentVersions;
    snoGroupInfo.dwFlags = dwSNOGroupFlags;
    snoGroupInfo.eSNODataType = eSNODataType;
    snoGroupInfo.pdefGroup = this;





    this->dwMaxCount = 0;
    this->dwInstanceSize = dwInstanceSize;
    this->dwFlags = dwFlags;
    this->ptTypeDesc = ptTypeDesc;

    dwIdenticalLookups = 0;
    dwNumUniqueLocks = 0;

    dwTotalLookups = 0;
    posLockCtr = NULL;
    ptInstances = NULL;
    ptPendingInstances = NULL;
    pTSDef = NULL;
    snoFallback = SNO_NONE;



    if ((snoGroupInfo.dwFlags & 0x1e) != 0)
    {




        if (!CmdLineTestFlag(43) && !(this->dwFlags & (1 << GROUPDEFINITION_ASYNC_FLUSH)) && !CmdLineTestFlag(44))
        {
            this->dwFlags &= ~(1 << GROUPDEFINITION_THREAD_SAFE_BIT);
            this->dwFlags |= (1 << GROUPDEFINITION_NO_LOCK_TRACKING);
        }
    }

}


GroupDefinition::~GroupDefinition()
{
}













































void GroupDefinition::InitializeSNOMap()
{


    SNO snoHighestUsed = SNOFilesGetHighestUsedSNOHandle();
    sdwSNOInArray = snoHighestUsed + 1;
    spptDefinitionArray = new DefDataElementID[snoHighestUsed + 1];
    for (uint32 i = 0; i < sdwSNOInArray; i++)
    {
        spptDefinitionArray[i] = DEFDATAELEMENTID_INVALID;
    }




    memset(spptDynamicDefinitionArray, 0xff, sizeof(spptDynamicDefinitionArray));


    sdwLowestUnusedDynamicHandle = -10001;



    for (int32 nGroup = 0; nGroup < MAX_SNO_GROUPS; nGroup++)
    {


        const SNOGroupInfo * ptInfo = SNOGroupGetInfo(nGroup);
        if (ptInfo == NULL)
            continue;

        SNOGroup snoGroup = nGroup;
        BOOL fGlobal = ptInfo->dwFlags & ((1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_GLOBAL) | (1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_GLOBAL));
        BOOL fPerAct = ptInfo->dwFlags & ((1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_PER_ACT) | (1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_PER_ACT));
        float flCountRatio = 1.0f;
        uint32 dwAdditionnalSlots = 256;




        if (!fGlobal && !fPerAct)
            continue;








        uint32 dwCount = (uint32)(SNOFilesGetTOCCountForGroup(snoGroup) * flCountRatio + dwAdditionnalSlots);




        GroupDefinition * ptGroupDef = GameDefinitionGet(snoGroup);
        if (ptGroupDef->GetMaxCount() < dwCount)
        {
            ptGroupDef->ResizeCache(dwCount);
        }
    }
}
































void GroupDefinition::DisposeSNOMap()
{
    TDataArrayDispose(sptGlobalInstances);

    delete[] spptDefinitionArray;
    spptDefinitionArray = NULL;
    sdwSNOInArray = 0;
}







void GroupDefinition::Init()
{





    dwMaxCount = GetMaxCount();







    CHAR szDefName[256];
    sprintf(szDefName, "%s Def", snoGroupInfo.szName);

    if (dwFlags & (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT))
    {
        if (sptGlobalInstances == NULL)
        {

            sptGlobalInstances = TDataArrayCreateTracked<DefDataElement, DefDataElementID>("Global", NULL, 40000);
        }
        ptInstances = sptGlobalInstances;
    }
    else
    {




        ptInstances = TDataArrayCreateTracked<DefDataElement, DefDataElementID>(szDefName, NULL, dwMaxCount);
    }




    posLockCtr = NULL;


    if (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT))
    {

        pTSDef = new ThreadSafeDef;
        pTSDef->nMapChangeLock = 0;
        pTSDef->tCurrentMapChanger = NULL;
        ptPendingInstances = TDataArrayCreateTracked<PendingDefDataElement, DefDataElementID>(szDefName, NULL, 64);
    }


    SNOGroupRegister(snoGroupInfo, ptTypeDesc);

    dwIdenticalLookups = 0;

    dwTotalLookups = 0;

    dwNumUniqueLocks = 0;

    TypeDescriptorField field(ptTypeDesc, NULL);
    ptTypeDesc->ScanForSubfields(field);
}

void GroupDefinition::ResizeCache(uint32 dwCacheSizeOverride)
{

    BOOL fThreadSafe = dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT);

















    uint32 dwNewMaxCount = dwCacheSizeOverride;
    if (dwNewMaxCount == 0)
        dwNewMaxCount = GetMaxCount();

    if (dwNewMaxCount == dwMaxCount)
        return;

    dwMaxCount = dwNewMaxCount;

    while (ptInstances->GetTotalCount() >= dwNewMaxCount)
    {
        CacheManagerPurge(FALSE);
    }



    if (fThreadSafe)
        BeginMapChange();




    TDataArray<DefDataElement, DefDataElementID> * ptInstancesNew = TDataArrayCreateTracked<DefDataElement, DefDataElementID>(ptInstances->szName, NULL, dwNewMaxCount);
    TDataArrayIterator<DefDataElement> tIterator(0x3ffff);
    DefDataElement * ptNext = ptInstances->Iterate(tIterator);
    while (ptNext != NULL)
    {


        DefDataElementID idDefinition = ptInstancesNew->ElementNew();
        DefDataElement * ptDefData = ptInstancesNew->ElementGet(idDefinition);

        ptDefData->id = idDefinition;
        ptDefData->lpDefinitionBase = ptNext->lpDefinitionBase;








        RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)ptDefData->lpDefinitionBase;
        SetDefinition(ptDefHeader->snoHandle, DEFDATAELEMENTID_INVALID);
        SetDefinition(ptDefHeader->snoHandle, idDefinition);

        ptNext = ptInstances->Iterate(tIterator);
    }

    TDataArrayDispose(ptInstances);
    ptInstances = ptInstancesNew;
    dwMaxCount = dwNewMaxCount;


    if (fThreadSafe)
        EndMapChange();
}







void GroupDefinition::Dispose()
{

    if (ptInstances == NULL)
        return;




    DeleteAll();


    if (dwFlags & (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT))
    {

        ptInstances = NULL;
    }
    else
    {

        TDataArrayDispose(ptInstances);
    }









    if (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT))
    {

        TDataArrayDispose(ptPendingInstances);
        delete pTSDef;
    }


}
















































void GroupDefinition::ReadDefinitionHeader(SNOElement * ptSNOElement, void * lpDefinition)
{


    const XTypeDescriptor * ptType = SNOMLGetTypeFromSNOGroup(GetGroup());
    uint32 dwSize = ptType->SizeOf();



















    SNOFileRead(ptSNOElement, 0, dwSize, lpDefinition);
}





void GroupDefinition::Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const
{






    const XTypeDescriptor * ptType = SNOMLGetTypeFromSNOGroup(GetGroup());
    TypeDescriptorField field(ptType, NULL);

    if (fContiguousBuffer)
    {



        SNOFileRead(ptSNOElement, 0, dwContiguousSize, lpDefinition);





        ReadFromMemoryBuffer(lpDefinition, dwContiguousSize, ptMemoryInterface);
    }
    else
    {

        uint32 dwSize = ptType->SizeOf();








        SNOFileRead(ptSNOElement, 0, dwSize, lpDefinition);





        MergedOffsetMap tMergedOffsetMap(16, NULL);
        UnserializeData tUnserializeData = { ptSNOElement, NULL, 0, ptMemoryInterface, &tMergedOffsetMap };
        ptType->SerializeFromBinary(lpDefinition, tUnserializeData, field);
    }


    *(RequiredDefinitionHeader *)lpDefinition = tHeader;
}



void GroupDefinition::ReadFromMemoryBuffer(void * lpDefinition, uint32 dwDefinitionSize, XMemoryAllocatorInterface * ptMemoryInterface) const
{







    RequiredDefinitionHeader * ptDefinitionHeader = (RequiredDefinitionHeader *)lpDefinition;

    const XTypeDescriptor * ptType = SNOMLGetTypeFromSNOGroup(GetGroup());
    TypeDescriptorField field(ptType, NULL);
    MergedOffsetMap tMergedOffsetMap(16, NULL);
    UnserializeData tUnserializeData = { NULL, lpDefinition, dwDefinitionSize, ptMemoryInterface, &tMergedOffsetMap };
    ptType->SerializeFromBinary(lpDefinition, tUnserializeData, field);


    ptDefinitionHeader->dwFlags |= (1 << DH_IN_ONE_BIG_CONTIGUOUS_BLOCK);
}
















































































































































BOOL GroupDefinition::ValidateSourceFiles(const void *) const
{

    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

BOOL GroupDefinition::ValidateCompiledSourceFiles(const void *) const
{

    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}
















































































void GroupDefinition::Postprocess(void * lpDefinition)
{
}























void GroupDefinition::Delete(void * lpDefinition) const
{



    const XTypeDescriptor * ptType = SNOMLGetTypeFromSNOGroup(GetGroup());


    TypeDescriptorField field(ptType, NULL);



    RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)lpDefinition;


    ptType->Delete(ptDefHeader->dwFlags, lpDefinition, field, &stDefinitionHeapAutoPurgeAlloc);
}







































































































BOOL GroupDefinition::Write(void * lpDefinition, FileReference & tRef) const
{


























    BOOL fClientData = GetSNOPackDestination(lpDefinition);
    if (!fClientData)
    {
        gGroupDefinition_nWritingServerDefinitionRefCount++;
    }

    const XTypeDescriptor * ptType = SNOMLGetTypeFromSNOGroup(GetGroup());
    TypeDescriptorField field(ptType, NULL);



    uint32 dwSize = ptType->SizeOf();









    ptType->AdjustSerializeDataSize(lpDefinition, field, FALSE);
    if (SigmaPlatformIsConsole(SigmaPlatformGet()))
    {
        ptType->ComputeMergedOffsets(lpDefinition, field, dwSize);
    }
    else
    {
        ptType->ComputeOffsets(lpDefinition, field, dwSize);
    }


    uint32 dwFileSize = dwSize;
    void * lpBuffer = SigmaMemoryNew(dwFileSize, 4, NULL, TRUE);
    SigmaMemoryCopy(lpBuffer, lpDefinition, sizeof(RequiredDefinitionHeader));











    ((RequiredDefinitionHeader *)lpBuffer)->dwFlags |= (1 << DH_BUILT_BY_CLIENT_BIT);


    ptType->SerializeToBinary(lpDefinition, field, (uint8 *)lpBuffer, 0, dwFileSize);









    SNOFileWrite(tRef, lpBuffer, 0, dwFileSize);

    SigmaMemoryFree(lpBuffer, NULL);

    if (!fClientData)
    {
        gGroupDefinition_nWritingServerDefinitionRefCount--;
    }

    return TRUE;
}






















































BOOL GroupDefinition::WriteMetaData(const SNOName & snoName)
{




























    return TRUE;
}

























































































































































































void GroupDefinition::GetDefaultData(void * ptDefaultDef) const
{

    SigmaMemoryClear(ptDefaultDef, dwInstanceSize);
}






void * GroupDefinition::DefinitionDataArrayIterate(TDataArrayIterator<void> & tIt, void *& ptDefinition) const
{
    DefDataElement * ptDefData = NULL;


    if (ptInstances != NULL)
    {





        ptDefData = ptInstances->Iterate(tIt.unk_0, (DefDataElement *&)tIt.ptElement);


        if (dwFlags & (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT))
        {
            SNOGroup snoGroup = GetGroup();
            while (ptDefData != NULL && ptDefData->snoGroup != snoGroup)
            {
                ptDefData = ptInstances->Iterate(tIt.unk_0, (DefDataElement *&)tIt.ptElement);
            }
        }
    }



    if (ptDefData != NULL)
    {





        ptDefinition = ptDefData->lpDefinitionBase;
    }
    else
    {
        ptDefinition = NULL;
    }
    return ptDefinition;
}

// NON_MATCHING: same instructions, but the iterator's element pointer and the 0xffff constant
// trade registers (x20/x21).










































void GroupDefinition::DeleteAll()
{

    if (ptInstances == NULL)
        return;






    TDataArrayIterator<void> tIt(0x3ffff);
    RequiredDefinitionHeader * ptDefHeader;
    while (DefinitionDataArrayIterate(tIt, (void *&)ptDefHeader) != NULL)
    {


        if (ptDefHeader->dwFlags & (1 << DH_READ_LOCK_HELD_BIT))
        {



            ptDefHeader->dwFlags &= ~(1 << DH_READ_LOCK_HELD_BIT);
        }
        if (ptDefHeader->dwFlags & (1 << DH_WRITE_LOCK_HELD_BIT))
        {



            ptDefHeader->dwFlags &= ~(1 << DH_WRITE_LOCK_HELD_BIT);
        }

        Purge(ptDefHeader->snoHandle, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
    }
}














































































































void GroupDefinition::SNODelete(const SNO sno)
{


    SNOName snoname(GetGroup(), sno);
    SNOMLDelete(snoname);

    Purge(sno, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
    SNOFileDelete(snoname, FALSE);
}









































































































uint32 sGetEstimatedSize(GroupDefinition * ptGroupDef, const SNOGroup snoGroup, const SNO snoHandle)
{




    SNOName snoname(snoGroup, snoHandle);
    SNOElement * ptSNOElement = SNOFileLookup(snoname);
    if (ptSNOElement == NULL)
        return 0;

    {
        uint64 dwHash = SNOFileGetFileHash(ptSNOElement, TRUE);

        Console::IO::FileInfo info;
        Console::IO::CPKArchiveReader * ptArchive = Console::IO::FindArchiveContaining(dwHash, &info);
        if (ptArchive == NULL)
            return 0;


        return info.nSize + ptGroupDef->GetEstimatedChildFilesSize(snoHandle);
    }
}























BOOL GroupDefinition::LoadChildFile(SNOElementChild * ptSNOElementChild)
{



    return FALSE;
}



uint32 GroupDefinition::GetEstimatedChildFilesSize(const SNO sno) const
{



    return 0;
}



void GroupDefinition::Preload(const SNO sno)
{







    if (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT))
    {
        LockMapChange();
        DefDataElement * ptDefData = sGet<true, false>(sno, FALSE, FALSE, TRUE);
        if (ptDefData != NULL)
        {
            ((RequiredDefinitionHeader *)ptDefData->lpDefinitionBase)->dwFlags |= (1 << DH_WAS_PRELOADED_BIT);
        }
        UnlockMapChange();
    }
    else
    {
        DefDataElement * ptDefData = sGet<false, false>(sno, FALSE, FALSE, TRUE);
        if (ptDefData != NULL)
        {
            ((RequiredDefinitionHeader *)ptDefData->lpDefinitionBase)->dwFlags |= (1 << DH_WAS_PRELOADED_BIT);
        }
    }
}




const void * GroupDefinition::LockR(const SNO sno, const BOOL fLockCanFail)
{














    if (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT))
    {
        LockMapChange();
        DefDataElement * ptDefData = sGet<true, true>(sno, FALSE, fLockCanFail, FALSE);

        if (ptDefData == NULL)
        {
            UnlockMapChange();
            return NULL;
        }



        const void * lpDefinition = LockR(ptDefData->lpDefinitionBase);
        UnlockMapChange();
        return lpDefinition;
    }
    else
    {



        DefDataElement * ptDefData = sGet<false, true>(sno, FALSE, fLockCanFail, FALSE);

        if (ptDefData == NULL)
        {
            return NULL;
        }


        return LockR(ptDefData->lpDefinitionBase);
    }
}






const void * GroupDefinition::LockRFast(const SNO sno)
{
    DefDataElement * ptDefData = NULL;


    if ((uint32)sno < sdwSNOInArray)
    {



        DefDataElementID idDefinition = spptDefinitionArray[(uint32)sno];



        if (idDefinition != DEFDATAELEMENTID_INVALID)
        {
            ptDefData = ptInstances->ElementGet(idDefinition);
        }
    }
    if (ptDefData == NULL)
    {

        ptDefData = HandleError<false>(sno, FALSE, FALSE);
    }


    return ptDefData->lpDefinitionBase;
}
BOOL GroupDefinition::SetRequired(const SNO sno, BOOL fRequired)
{


    if (!Exists(sno, NULL))
        return FALSE;






    if (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT))
    {
        LockMapChange();

        DefDataElement * ptDefData = sGet<true, true>(sno, FALSE, FALSE, FALSE);
        if (ptDefData == NULL)
        {
            UnlockMapChange();
            return FALSE;
        }


        SetRequired(ptDefData->lpDefinitionBase, fRequired);
        UnlockMapChange();
    }
    else
    {
        DefDataElement * ptDefData = sGet<false, true>(sno, FALSE, FALSE, FALSE);
        if (ptDefData == NULL)
        {
            return FALSE;
        }


        SetRequired(ptDefData->lpDefinitionBase, fRequired);
    }
    return TRUE;
}
void GroupDefinition::SetRequired(void * lpDefinition, BOOL fRequired)
{




    RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)lpDefinition;

    if (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT))
    {
        if (fRequired)
            Blizzard::Lock::Atomic::Or(1 << DH_DO_NOT_PURGE_CACHE_BIT, &ptDefHeader->dwFlags);
        else
            Blizzard::Lock::Atomic::And(~(1 << DH_DO_NOT_PURGE_CACHE_BIT), &ptDefHeader->dwFlags);
    }
    else if (fRequired)
    {
        ptDefHeader->dwFlags |= (1 << DH_DO_NOT_PURGE_CACHE_BIT);
    }
    else
    {
        ptDefHeader->dwFlags &= ~(1 << DH_DO_NOT_PURGE_CACHE_BIT);
    }
}

const void * GroupDefinition::LockR(void * lpDefinition)
{




    if (!(dwFlags & (1 << GROUPDEFINITION_NO_LOCK_TRACKING)))
    {
        RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)lpDefinition;








        if (!(dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT)))
        {
            if (ptDefHeader->nLockCount == 0)
            {
                dwNumUniqueLocks++;
            }








            ptDefHeader->nLockCount++;
        }
        else
        {

            if (Blizzard::Lock::Atomic::Increment(&ptDefHeader->nLockCount) == 1)
            {
                if (dwFlags & (1 << GROUPDEFINITION_KEEP_TRACK_OF_THREAD_SAFE_UNIQUE_LOCKS))
                {
                    Blizzard::Lock::Atomic::Increment(&dwNumUniqueLocks);
                }
            }
        }
    }










    return lpDefinition;
}
void GroupDefinition::UnlockR(void *& lpDef)
{









    RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)lpDef;

    if (!(dwFlags & (1 << GROUPDEFINITION_NO_LOCK_TRACKING)))
    {












        if (!(dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT)))
        {
            ptDefHeader->nLockCount--;

            if (ptDefHeader->nLockCount == 0)
            {
                dwNumUniqueLocks--;
            }










        }
        else if (Blizzard::Lock::Atomic::Decrement(&ptDefHeader->nLockCount) == 0)
        {
            if (dwFlags & (1 << GROUPDEFINITION_KEEP_TRACK_OF_THREAD_SAFE_UNIQUE_LOCKS))
            {
                Blizzard::Lock::Atomic::Decrement(&dwNumUniqueLocks);
            }
        }
    }












    lpDef = NULL;
}
void * GroupDefinition::LockW(const SNO sno, const BOOL fLockCanFail)
{









    if (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT))
    {
        LockMapChange();
        DefDataElement * ptDefData = sGet<true, true>(sno, TRUE, fLockCanFail, FALSE);

        void * lpDefinition = LockW(ptDefData->lpDefinitionBase);
        UnlockMapChange();
        return lpDefinition;
    }
    else
    {
        DefDataElement * ptDefData = sGet<false, true>(sno, TRUE, fLockCanFail, FALSE);

        return LockW(ptDefData->lpDefinitionBase);
    }
}





void * GroupDefinition::LockW(void * lpDefinition)
{







    if (!(dwFlags & (1 << GROUPDEFINITION_NO_LOCK_TRACKING)))
    {
        RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)lpDefinition;







        if (!(dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT)))
        {
            if (ptDefHeader->nLockCount == 0)
            {
                dwNumUniqueLocks++;
            }







            ptDefHeader->nLockCount++;
        }
        else
        {

            if (Blizzard::Lock::Atomic::Increment(&ptDefHeader->nLockCount) == 1)
            {
                if (dwFlags & (1 << GROUPDEFINITION_KEEP_TRACK_OF_THREAD_SAFE_UNIQUE_LOCKS))
                {
                    Blizzard::Lock::Atomic::Increment(&dwNumUniqueLocks);
                }
            }
        }
    }
    return lpDefinition;
}













void GroupDefinition::UnlockW(void *& lpDef)
{


    RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)lpDef;

    if (!(dwFlags & (1 << GROUPDEFINITION_NO_LOCK_TRACKING)))
    {










        if (!(dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT)))
        {
            ptDefHeader->nLockCount--;

            if (ptDefHeader->nLockCount == 0)
            {
                dwNumUniqueLocks--;
            }
        }








        else if (Blizzard::Lock::Atomic::Decrement(&ptDefHeader->nLockCount) == 0)
        {
            if (dwFlags & (1 << GROUPDEFINITION_KEEP_TRACK_OF_THREAD_SAFE_UNIQUE_LOCKS))
            {
                Blizzard::Lock::Atomic::Decrement(&dwNumUniqueLocks);
            }
        }
    }













    lpDef = NULL;
}



void GroupDefinition::SNOWrite(const SNO sno, void * lpDefinition)
{





    SNOName snoname(GetGroup(), sno);
    CRefString sFileName = SNOFileGetName(snoname);
    XLocale eXLocale = SNOMLFileGetLocale(snoname);
    if (eXLocale == XLOCALE_INVALID)
    {

        eXLocale = SNOFileGetLocale(snoname);

        SNOWriteSNOFile(eXLocale, sno, sFileName, lpDefinition, 0);
    }
    else
    {

        CRefString sPath;
        SNOMLMakeXMLPath(eXLocale, GetGroup(), sFileName, sPath);

        SNOMLResult eResult = SNOMLUpdate(snoname, lpDefinition, sPath);
        switch (eResult)
        {
        case SNOML_FILECREATED:
        case SNOML_FAILED:
            break;

        case SNOML_READONLY:



            if (!ErrorManagerGetSuppressUserInteraction())
            {
            }
            break;


        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
}


static void sCleanupTempFiles(const char * szPath)
{

    FileReference tFileRef;
    FileReferenceInit(tFileRef, CRefString().Format(szPath));
    SNODeleteAdditionalFiles(tFileRef.szPath);
    if (FileExists(tFileRef))
    {
        FileDelete(tFileRef);
    }
}




BOOL GroupDefinition::SNOWriteSNOFile(const XLocale eXLocale, const SNO sno, LPCSTR szFileNameNoExtension, void * lpDefinition, const uint32 dwXMLHash)
{































    SNOName snoname(GetGroup(), sno);
    SNOFileAddEntry(SNOTOC::TT_CORE, GetGroup(), sno, szFileNameNoExtension);
    SNOElement * ptSNOElement = SNOFileLookup(snoname);





    FileReference tTempRef;






    FileReferenceInit(tTempRef, SNOFilesGetPlatformDataPath(SigmaPlatformGet()));
    if (!FileDirectoryCreate(tTempRef, FALSE))
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    FileReferenceInit(tTempRef, CRefString().Format("%s\\AxeCompile-%d.tmp",
        SNOFilesGetPlatformDataPath(SigmaPlatformGet()).str(), sno));

    sCleanupTempFiles(tTempRef.szPath);
    if (!FileCreate(tTempRef))
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }



    BOOL fOpened = FALSE;
    for (int32 i = 0; i < 3; i++)
    {
        if (FileOpen(tTempRef, 1 << ACCESS_WRITE_BIT))
        {
            fOpened = TRUE;
            break;
        }
        Blizzard::Process::Sleep(1000);
    }
    if (!fOpened)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }

    SNOFileWriteHeader(tTempRef, ptSNOElement, dwXMLHash);
    BOOL fSuccess = Write(lpDefinition, tTempRef);

    FileClose(tTempRef);










    if (fSuccess)
    {


        FileReference tRef;
        SNOFileGetFolderForNewLocalCacheFile(tRef, eXLocale, GetGroup());


        if (!FileDirectoryCreate(tRef, TRUE))
        {
            sCleanupTempFiles(tTempRef.szPath);
            return FALSE;
        }







        CHAR szFileName[256];
        Blizzard::String::Copy(szFileName, szFileNameNoExtension, sizeof(szFileName));
        SNOFixFilename(szFileName);
        FileReferenceAddDirectory(tRef, CRefString().Format("%s%s", szFileName, SNOGetGroupExtension(GetGroup())));


        if (FileExists(tRef))
        {
            if (!FileDelete(tRef))
            {
                sCleanupTempFiles(tTempRef.szPath);
                return FALSE;
            }
        }







        if (!SNODeleteAdditionalFiles(tRef.szPath))
        {
            sCleanupTempFiles(tTempRef.szPath);
            return FALSE;
        }




        if (!FileMove(tTempRef, tRef))
        {
            sCleanupTempFiles(tTempRef.szPath);
            return FALSE;
        }




        if (!SNOTransferAdditionalFiles(tTempRef.szPath, tRef.szPath, SNOMoveAdditionalFile))
        {
            sCleanupTempFiles(tTempRef.szPath);
            return FALSE;
        }
        GlobalSNOListRefresh();
    }
    sCleanupTempFiles(tTempRef.szPath);
    return fSuccess;
}
DefDataElementID GroupDefinition::GetDefinition(
    SNO snoHandle)
{
    uint32 dwIndex = (uint32)snoHandle;
    // dwIndex is the index on 3031 (load col 17), 3040 ([dwIndex + 10001], `+` col 55) and 3051.
    if (dwIndex >= sdwSNOInArray)
    {
        if (dwIndex >= (uint32)-10001)
        {
            if ((snoHandle == SNO_NONE) || (snoHandle == INVALID_SNO))  // orig `(uint32)sno > -3` test at 3035:41 = this `||` (operand order/parens inferred from the column)
                return DEFDATAELEMENTID_INVALID;



            return spptDynamicDefinitionArray[dwIndex + 10001];
        }
        {
            DefDataElementID defID;
            if (stOverflowDefinitions.Lookup(snoHandle, defID))
                return defID;
        }

        return DEFDATAELEMENTID_INVALID;
    }

    return spptDefinitionArray[dwIndex];
}




void GroupDefinition::SetDefinition(SNO snoHandle, DefDataElementID defDataElementID)
{


    if ((uint32)snoHandle >= (uint32)-10001)
    {



        uint32 dwIndex = snoHandle + 10001;
        spptDynamicDefinitionArray[dwIndex] = defDataElementID;
        if (defDataElementID == DEFDATAELEMENTID_INVALID)
        {

            stDynamicDefinitionHandleLock.Enter();
            if ((uint32)snoHandle < sdwLowestUnusedDynamicHandle)
            {
                sdwLowestUnusedDynamicHandle = snoHandle;
            }

            stDynamicDefinitionHandleLock.Leave();
        }
        return;
    }



    if ((uint32)snoHandle >= sdwSNOInArray)
    {

        if (defDataElementID == DEFDATAELEMENTID_INVALID)
        {

            if (stOverflowDefinitions.LookupPtr(snoHandle) != NULL)
            {
                stOverflowDefinitions.RemoveAt(snoHandle);
            }
        }
        else
        {

            stOverflowDefinitions.SetAt_Tracked(snoHandle, defDataElementID);
        }
        return;
    }



    spptDefinitionArray[(uint32)snoHandle] = defDataElementID;
}
SNO GroupDefinition::New(const void * lpDefinitionData, BOOL fContiguous, uint32 dwContinuousSize)
{



    SGameID idSGame = GameServerCodeLeave_Tracked(TRUE);
    BOOL fThreadSafe = (dwFlags >> GROUPDEFINITION_THREAD_SAFE_BIT) & 1;

    stDynamicDefinitionLock.Enter();

    stDynamicDefinitionHandleLock.Enter();






    uint32 uDynamicGUID = sdwLowestUnusedDynamicHandle;
    while (Exists(uDynamicGUID, NULL))
    {
        uDynamicGUID++;
    }
    uDynamicGUID++;

    sdwLowestUnusedDynamicHandle = uDynamicGUID;

    stDynamicDefinitionHandleLock.Leave();






    SNO snoHandle = uDynamicGUID - 1;

    if (fThreadSafe)
        LockMapChange();


    AutoPurgeIfNeeded(dwContinuousSize, TRUE, FALSE, fThreadSafe);

    DefDataElementID idDefinition = ptInstances->ElementNew();
    void * lpDefinition;

    sCreate(snoHandle, snoHandle, fContiguous, dwContinuousSize, idDefinition, lpDefinition, TRUE);

    SigmaMemoryCopy(lpDefinition, lpDefinitionData, dwInstanceSize);


    ((RequiredDefinitionHeader *)lpDefinition)->Initialize(snoHandle, FALSE, fContiguous, FALSE, dwFlags & (1 << GROUPDEFINITION_CANT_SHARE_BIT));






    Postprocess(lpDefinition);
    ((RequiredDefinitionHeader *)lpDefinition)->dwFlags |= (1 << DH_WAS_POSTPROCESSED_BIT);


    ((RequiredDefinitionHeader *)lpDefinition)->dwFlags |= (1 << DH_DO_NOT_PURGE_CACHE_BIT);

    if (fThreadSafe)
    {
        UnlockMapChange();
    }
    stDynamicDefinitionLock.Leave();


    if (!GameServerCodeEnter_Track())
        _GameServerCodeEnter(idSGame);
    return snoHandle;
}
BOOL GroupDefinition::WasDynamicallyCreated(const SNO snoHandle) const
{


    return snoHandle > -10002 && snoHandle < -2;
}

































BOOL GroupDefinition::CacheManagerShouldPurge(uint32 dwEstNewElementSize, BOOL fWillCreateInstance) const
{



    if (DefinitionCurrentlyPreCaching() && !(dwFlags & (1 << GROUPDEFINITION_CANT_SHARE_BIT)))
        return FALSE;




    if (fWillCreateInstance && ptInstances->GetTotalCount() >= ptInstances->dwMaximumCount)
        return TRUE;
    return FALSE;
}










































































SNO GroupDefinition::CacheManagerFindElementToPurge() const
{
    SNO snoOldest = SNO_NONE;
    uint32 dwOldestFrame = 0xffffff;
    BOOL fFirst = TRUE;
    DefDataElement * ptDefData;
    uint32 dwInvCurrentFrame = 0xffffff - GetCurrentFrame(TRUE);







    TDataArrayIterator<DefDataElement> tIterator(0x3ffff);
    while ((ptDefData = ptInstances->Iterate(tIterator)) != NULL)
    {

        if (CacheManagerCanPurgeElement(*ptDefData, FALSE))
        {

            uint32 dwFrame = (ptDefData->dwLastUsedFrame + dwInvCurrentFrame) & 0xffffff;
            if (fFirst || dwFrame < dwOldestFrame)
            {

                RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)ptDefData->lpDefinitionBase;



                snoOldest = ptDefHeader->snoHandle;
                dwOldestFrame = dwFrame;
                fFirst = FALSE;
            }
        }
    }

    return snoOldest;
}
BOOL GroupDefinition::CacheManagerCanPurgeElement(const DefDataElement & tDefElement, BOOL fMemoryIsCritical) const
{




    RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)tDefElement.lpDefinitionBase;


    if (ptDefHeader == NULL || ptDefHeader->nLockCount != 0)
        return FALSE;




    if (!(ptDefHeader->dwFlags & (1 << DH_DO_NOT_PURGE_CACHE_BIT)))
        return TRUE;


    if (!fMemoryIsCritical)
        return FALSE;


    if (WasDynamicallyCreated(ptDefHeader->snoHandle))
        return FALSE;




    return (dwFlags & (1 << GROUPDEFINITION_IGNORE_DO_NOT_PURGE_BIT_WHEN_MEMORY_IS_CRITICAL)) != 0;
}








BOOL GroupDefinition::GlobalSNOCanBePurged(SNO sno, uint32 dwLastUsedFrame, BOOL fMemoryIsCritical)
{

    BOOL fPending = FALSE;
    if (!Exists(sno, &fPending))
        return FALSE;





    if (fPending)
        return FALSE;







    DefDataElement * ptDefData;
    BOOL fThreadSafe = dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT);
    if (fThreadSafe)
    {
        LockMapChange();
        ptDefData = sGet<true, false>(sno, FALSE, FALSE, FALSE);
        UnlockMapChange();
    }
    else
    {

        ptDefData = sGet<false, false>(sno, FALSE, FALSE, FALSE);
    }


    return ptDefData->dwLastUsedFrame == dwLastUsedFrame && CacheManagerCanPurgeElement(*ptDefData, fMemoryIsCritical);
}
void GroupDefinition::GlobalCacheManagerClearPurgeLists(GlobalCacheType eGlobalCacheType)
{


    Blizzard::Lock::ScopedMutex tLock(stGlobalCacheLock[eGlobalCacheType]);

    sdwGlobalPurgeArrayCount[eGlobalCacheType] = 0;

}



static uint32 sdwInvCurrentUsageFrame;
static int32 sSortPurgeableItems(const void * pLeft, const void * pRight)
{
    const GlobalPurgeableItem * ptLeft = (const GlobalPurgeableItem *)pLeft;
    const GlobalPurgeableItem * ptRight = (const GlobalPurgeableItem *)pRight;




    return ((ptLeft->dwLastUsedFrame + sdwInvCurrentUsageFrame) & 0xffffff) - ((ptRight->dwLastUsedFrame + sdwInvCurrentUsageFrame) & 0xffffff);
}
// NON_MATCHING: 1368 vs 1364 bytes. Loop, calls and stores now match; only the reload of the
// spilled eGlobalCacheType differs (original reloads it once at the start of each frame-delta
// branch, ours once per path into the Math::Min tail: one extra ldr). Register allocation only.
void GroupDefinition::GlobalCacheManagerGeneratePurgeList(
    GlobalCacheType eGlobalCacheType,
    BOOL fMemoryIsCritical)
{
    GlobalPurgeableItem tTempItems[1024];
    uint32 dwTempItemCount = 0;
    uint32 dwHighestLastUsedFrame = 0;
    uint32 dwSortCount;


    uint32 dwBit = (eGlobalCacheType == GLOBALCACHE_MAIN) ? (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT) : (1 << GROUPDEFINITION_USE_SECONDARY_GLOBAL_CACHE_LIMIT);

    BOOL pfValidGroup[MAX_SNO_GROUPS];


    for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
    {
        GroupDefinition * ptGroupDef = GameDefinitionGet(i);
        if (ptGroupDef != NULL





            && (ptGroupDef->dwFlags & dwBit))
        {
            pfValidGroup[i] = TRUE;
            ptGroupDef->LockMapChange();
        }
        else
        {
            pfValidGroup[i] = FALSE;
        }
    }

    uint32 dwCurrentFrame = GetCurrentFrame(TRUE);
    uint32 dwInvCurrentUsageFrame = 0xffffff - dwCurrentFrame;


    sdwInvCurrentUsageFrame = dwInvCurrentUsageFrame & 0xffffff;

    // The original has the Iterate call once, at 3537 (DWARF call_line); this decl-init copy is
    TDataArrayIterator<DefDataElement> tIterator(0x3ffff);
    DefDataElement * ptDefData = sptGlobalInstances->Iterate(tIterator);
    // tail-merged into it. No continue: the loop step sits at the bottom of the body.
    while (ptDefData != NULL)
    {
        if (pfValidGroup[ptDefData->snoGroup])
        {
            GroupDefinition * ptGroupDef = GameDefinitionGet(ptDefData->snoGroup);
            if (ptGroupDef->CacheManagerCanPurgeElement(*ptDefData, fMemoryIsCritical))
            {
                uint32 dwLastUsedFrame = ptDefData->dwLastUsedFrame;
                if (dwTempItemCount >= 512)
                {
                    if (((dwLastUsedFrame + dwInvCurrentUsageFrame) & 0xffffff) < dwHighestLastUsedFrame)
                    {

                        GlobalPurgeableItem & item = tTempItems[dwTempItemCount++];
                        item.snoGroup = ptDefData->snoGroup;
                        item.snoHandle = ((RequiredDefinitionHeader*)ptDefData->lpDefinitionBase)->snoHandle;
                        item.dwLastUsedFrame = dwLastUsedFrame;


                        if (dwTempItemCount == 1024)
                        {
                            qsort(tTempItems, 1024, sizeof(GlobalPurgeableItem), sSortPurgeableItems);

                            dwHighestLastUsedFrame = (tTempItems[511].dwLastUsedFrame + dwInvCurrentUsageFrame) & 0xffffff;
                            dwTempItemCount = 512;
                        }
                    }
                }
                else
                {

                    GlobalPurgeableItem & item = tTempItems[dwTempItemCount++];
                    item.snoGroup = ptDefData->snoGroup;
                    item.snoHandle = ((RequiredDefinitionHeader*)ptDefData->lpDefinitionBase)->snoHandle;
                    item.dwLastUsedFrame = dwLastUsedFrame;
                    // No Math::Max in the DWARF: an if at 3529 (column 25 is the condition start)
                    if (((dwLastUsedFrame + dwInvCurrentUsageFrame) & 0xffffff) > dwHighestLastUsedFrame)
                        dwHighestLastUsedFrame = (dwLastUsedFrame + dwInvCurrentUsageFrame) & 0xffffff;
                }
            }
        }



        ptDefData = sptGlobalInstances->Iterate(tIterator);
    }

    for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
    {
        GroupDefinition * ptGroupDef = GameDefinitionGet(i);
        if (ptGroupDef == NULL)
            continue;



        if (ptGroupDef->dwFlags & dwBit)
        {
            ptGroupDef->UnlockMapChange();
        }
    }

    // GetCurrentFrame is inlined once (3555); the 3557 call folds into it
    if (GetCurrentFrame(TRUE) != dwCurrentFrame)
    {
        uint32 dwDelta = GetCurrentFrame(TRUE) - dwCurrentFrame;


        for (uint32 i = 0; i < dwTempItemCount; i++)
        {
            tTempItems[i].dwLastUsedFrame = (tTempItems[i].dwLastUsedFrame - dwDelta) & 0xffffff;
        }

        qsort(tTempItems, dwTempItemCount, sizeof(GlobalPurgeableItem), sSortPurgeableItems);


        for (uint32 i = 0; i < dwTempItemCount; i++)
        {
            tTempItems[i].dwLastUsedFrame = (tTempItems[i].dwLastUsedFrame + dwDelta) & 0xffffff;
        }
    }
    else
    {
        qsort(tTempItems, dwTempItemCount, sizeof(GlobalPurgeableItem), sSortPurgeableItems);
    }



    dwSortCount = Math::Min<uint32>(dwTempItemCount, 512);
    for (uint32 i = 0; i < dwSortCount; i++)
    {
        spGlobalPurgeArray[eGlobalCacheType][i] = tTempItems[dwSortCount - 1 - i];
    }

    sdwGlobalPurgeArrayCount[eGlobalCacheType] = dwSortCount;




}




SNOName GroupDefinition::GlobalCacheManagerFindElementToPurge(GlobalCacheType eGlobalCacheType, BOOL fMemoryIsCritical)
{


    Blizzard::Lock::ScopedMutex lock(stGlobalCacheLock[eGlobalCacheType]);




    for (;;)
    {
        if (sdwGlobalPurgeArrayCount[eGlobalCacheType] == 0)
        {

            GlobalCacheManagerGeneratePurgeList(eGlobalCacheType, fMemoryIsCritical);

            if (sdwGlobalPurgeArrayCount[eGlobalCacheType] == 0)
                return SNOName(SNOGROUP_NONE, SNO_NONE);
        }





        const GlobalPurgeableItem & oldestItem = spGlobalPurgeArray[eGlobalCacheType][sdwGlobalPurgeArrayCount[eGlobalCacheType] - 1];
        SNOName snoOldest(oldestItem.snoGroup, oldestItem.snoHandle);
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoOldest.snoGroup);

        sdwGlobalPurgeArrayCount[eGlobalCacheType]--;


        if (ptGroupDef->GlobalSNOCanBePurged(snoOldest.snoHandle, oldestItem.dwLastUsedFrame, fMemoryIsCritical))
        {

            return snoOldest;
        }
    }
}
inline BOOL GroupDefinition::GlobalCacheManagerPurge(GlobalCacheType eGlobalCacheType, const BOOL fLockCanFail, const BOOL fMemoryIsCritical)
{




    SNOName snoToPurge = GlobalCacheManagerFindElementToPurge(eGlobalCacheType, fMemoryIsCritical);

    if (snoToPurge.snoHandle == SNO_NONE)
    {


        if (!fLockCanFail)
        {
            char szDumpLocalBuffer[1024];
            uint32 dwDumpLocalBufferLen = 0;



            for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
            {
                GroupDefinition * ptGroupDef = GameDefinitionGet(i);
                if (ptGroupDef == NULL)
                    continue;



                if (!(ptGroupDef->dwFlags & (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT)))
                    continue;

                RequiredDefinitionHeader * ptDefHeader;
                TDataArrayIterator<void> tIt(0x3ffff);
                ptGroupDef->LockMapChange();



                while (ptGroupDef->DefinitionDataArrayIterate(tIt, (void *&)ptDefHeader) != NULL)
                {
                    CRefString sName;

                    sName = SNONameToString(i, ptDefHeader->snoHandle, FALSE);

                    dwDumpLocalBufferLen += sprintf(szDumpLocalBuffer + dwDumpLocalBufferLen, "%s (%d)\t%d\t%d\t%d\n",
                        sName.str(),
                        ptDefHeader->snoHandle,
                        ptDefHeader->nLockCount,
                        ptDefHeader->dwFlags & (1 << DH_DO_NOT_PURGE_CACHE_BIT),
                        sGetEstimatedSize(ptGroupDef, i, ptDefHeader->snoHandle));


                    if (dwDumpLocalBufferLen >= 725)
                    {
                        szDumpLocalBuffer[dwDumpLocalBufferLen] = 0;

                        dwDumpLocalBufferLen = 0;
                    }
                }

                ptGroupDef->UnlockMapChange();
            }


            if (dwDumpLocalBufferLen != 0)
            {
                szDumpLocalBuffer[dwDumpLocalBufferLen] = 0;
            }


            gptMainDefinitionsMemoryHeap->Dump();

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        return FALSE;
    }


    GroupDefinition * ptGroupDef = GameDefinitionGet(snoToPurge.snoGroup);
    return ptGroupDef->Purge(snoToPurge.snoHandle, FALSE, TRUE, fMemoryIsCritical, eGlobalCacheType);
}





BOOL GroupDefinition::CacheManagerPurge(const BOOL fLockCanFail)
{



    uint32 dwGroupFlags = dwFlags;
    if (dwGroupFlags & (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT))
    {

        return GlobalCacheManagerPurge(GLOBALCACHE_MAIN, fLockCanFail, FALSE);
    }

    if (dwGroupFlags & (1 << GROUPDEFINITION_CANNOT_LRU_PURGE_BIT))
    {
        if (fLockCanFail)
            return FALSE;
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }






    if ((dwGroupFlags & (1 << GROUPDEFINITION_ASYNC_FLUSH)) && ThreadGlobals::Get().m_nThreadIndex == 0)
    {

        CGameAsyncRenderFlush();
    }



    bool fThreadSafe = (dwGroupFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT)) != 0;
    if (fThreadSafe)
        BeginMapChange();


    SNO snoHandle = CacheManagerFindElementToPurge();

    if (snoHandle == SNO_NONE)
    {


        if (!fLockCanFail)
        {
            char szDumpLocalBuffer[1024];
            uint32 dwDumpLocalBufferLen = 0;










            RequiredDefinitionHeader * ptDefHeader;
            TDataArrayIterator<void> tIt(0x3ffff);
            while (DefinitionDataArrayIterate(tIt, (void *&)ptDefHeader) != NULL)
            {
                CRefString sName;

                sName = SNONameToString(GetGroup(), ptDefHeader->snoHandle, FALSE);

                dwDumpLocalBufferLen += sprintf(szDumpLocalBuffer + dwDumpLocalBufferLen, "%s (%d)\t%d\t%d\t%d\n",
                    sName.str(),
                    ptDefHeader->snoHandle,
                    ptDefHeader->nLockCount,
                    ptDefHeader->dwFlags & (1 << DH_DO_NOT_PURGE_CACHE_BIT),
                    sGetEstimatedSize(this, GetGroup(), ptDefHeader->snoHandle));


                if (dwDumpLocalBufferLen >= 725)
                {
                    szDumpLocalBuffer[dwDumpLocalBufferLen] = 0;

                    dwDumpLocalBufferLen = 0;
                }
            }

            if (dwDumpLocalBufferLen != 0)
            {
                szDumpLocalBuffer[dwDumpLocalBufferLen] = 0;
            }


            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
            ExitProgram(ERROR_INTERNAL_FATAL);
        }
        if (fThreadSafe)
        {
            EndMapChange();
        }
        return FALSE;
    }









    Purge(snoHandle, TRUE, TRUE, FALSE, GLOBALCACHE_MAIN);

    if (fThreadSafe)
    {
        EndMapChange();
    }



    return TRUE;
}



















































































































































DefDataElement * GroupDefinition::sCreateAndLoad(const SNO sno, const BOOL fWritable, const BOOL fLockCanFail, const BOOL fForPreload)
{







    Stopwatch tStopwatch;



    SGameID idSGame = GameServerCodeLeave_Tracked(TRUE);

    void * lpDefinition = NULL;
    DefDataElementID idDefinition = DEFDATAELEMENTID_INVALID;

    BOOL fThreadSafe = (dwFlags >> GROUPDEFINITION_THREAD_SAFE_BIT) & 1;
    sLoad(sno, FALSE, fWritable, fLockCanFail, lpDefinition, idDefinition, fThreadSafe, fForPreload);
    if (lpDefinition == NULL)
    {

        Purge(sno, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);

        if (!GameServerCodeEnter_Track())
            _GameServerCodeEnter(idSGame);
        return NULL;
    }


    DefDataElement * ptDefData = fThreadSafe ? ptPendingInstances->ElementGet(idDefinition) : ptInstances->ElementGet(idDefinition);


    SNOManager::OnSNOLoaded(SNOName(GetGroup(), sno), (RequiredDefinitionHeader *)ptDefData->lpDefinitionBase);


    if (!GameServerCodeEnter_Track())
        _GameServerCodeEnter(idSGame);

    if (sgbEnableLoadTrace)
    {

        float flTimeMillis = tStopwatch.Check() * 1000.0f;
        if (flTimeMillis >= sgflLoadTraceMinTime)
        {


            LPCSTR szName;
            CRefString sSnoName = SNOToString(GetGroup(), sno, FALSE);
        }
    }

    return ptDefData;
}
void GroupDefinition::sCreate(const SNO sno, const SNO snoHandleToReallyLoad, const BOOL fContiguous, const uint32 dwContiguousSize, DefDataElementID idDefinition, void *& lpDefinition, const BOOL fForceThreadUnsafe)
{







    BOOL fThreadSafe = !fForceThreadUnsafe && (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT));
    DefDataElement * ptDefData = fThreadSafe ? ptPendingInstances->ElementGet(idDefinition) : ptInstances->ElementGet(idDefinition);
    ptDefData->id = idDefinition;

    ptDefData->dwLastUsedFrame = GetCurrentFrame(FALSE) - 2;


    ptDefData->snoGroup = GetGroup();























    uint32 dwSizeToUse = dwInstanceSize;

    if (fContiguous)
    {
        dwSizeToUse = dwContiguousSize;
    }



    const XTypeDescriptor * ptType = SNOMLGetTypeFromSNOGroup(GetGroup());
    TypeDescriptorField field(ptType, NULL);
    uint32 dwAlign = ptType->Contains128BitSubfields(field) ? 16 : (ptType->Contains64BitSubfields(field) ? 8 : 4);
    XMemoryAllocatorInterface * ptMemoryInterface = &stDefinitionHeapAutoPurgeAlloc;
    ptDefData->lpDefinitionBase = MemAlloc(dwSizeToUse, dwAlign, ptMemoryInterface);
    SigmaMemoryClear(ptDefData->lpDefinitionBase, dwSizeToUse);








    lpDefinition = ptDefData->lpDefinitionBase;

    if (!fThreadSafe)
    {
        SetDefinition(sno, idDefinition);
    }
}


void RequiredDefinitionHeader::Initialize(const SNO sno, const BOOL fIsBadData, const BOOL fContiguous, const BOOL fIsExternal, const BOOL fGroupDefinitionCantShare)
{






    snoHandle = sno;

    nLockCount = 0;
    dwFlags = 0;
    if (fIsBadData) dwFlags |= (1 << DH_IS_BAD_DATA_BIT); else dwFlags &= ~(1 << DH_IS_BAD_DATA_BIT);
    if (fContiguous) dwFlags |= (1 << DH_IN_ONE_BIG_CONTIGUOUS_BLOCK); else dwFlags &= ~(1 << DH_IN_ONE_BIG_CONTIGUOUS_BLOCK);
    if (fIsExternal) dwFlags |= (1 << DH_IS_EXTERNAL_BIT); else dwFlags &= ~(1 << DH_IS_EXTERNAL_BIT);

    if (!fGroupDefinitionCantShare && DefinitionCurrentlyPreCaching())
    {
        dwFlags |= (1 << DH_PRECACHED_BIT);
        dwFlags |= (1 << DH_DO_NOT_PURGE_CACHE_BIT);
    }
}



SNO GroupDefinition::GetFallbackDataSNO() const
{


    if (snoFallback == SNO_NONE)
        snoFallback = SNOFileLookupGUID(GetGroup(), cgszBadDataName);


    return snoFallback;
}
uint32 GroupDefinition::GetContiguousReadSize(SNOElement * ptSNOElement)
{

    return SNOFileGetDataSize(ptSNOElement, TRUE);
}





static inline void sEvaluateSNOFileVersion(BOOL fOpenFile, SNOName & snoname, const SNOGroupInfo & snoGroupInfo, SNOElement *& ptSNOElement, BOOL & fUseFallbackData)
{










    ptSNOElement = SNOMLFileOpenForRead(snoname);
    if (ptSNOElement != NULL)
    {







        uint32 dwVersion;
        BOOL bGotVersion = SNOFileTryGetVersion(snoname, &dwVersion, NULL, NULL);
        if (bGotVersion)
        {






            if (dwVersion == snoGroupInfo.tVersions.GetVersion(SIGMAPLATFORM_UNKNOWN))
                return;

            if (dwVersion > snoGroupInfo.tVersions.GetVersion(SIGMAPLATFORM_UNKNOWN))
            {
                CRefString sSNOName = SNOToString(snoname.snoGroup, snoname.snoHandle, FALSE);
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
                ExitProgram(ERROR_INTERNAL_FATAL);
            }





            if (dwVersion >= snoGroupInfo.tVersions.GetVersion(SIGMAPLATFORM_UNKNOWN))
                return;


            SNOFileClose(ptSNOElement);
        }
    }

    fUseFallbackData = TRUE;
}

























BOOL GroupDefinition::AutoPurgeIfNeeded(uint32 dwEstNewElementSize, BOOL fWillCreateInstance, BOOL fLockCanFail, BOOL fNeedsUnlock)
{





    int32 nIterationCount = 1000;
    BOOL fShouldPurge = CacheManagerShouldPurge(dwEstNewElementSize, fWillCreateInstance);
    while (fShouldPurge)
    {
        BOOL fSuccess;


        if (fNeedsUnlock)
            UnlockMapChange();




        fSuccess = CacheManagerPurge(fLockCanFail);




        if (fNeedsUnlock)
            LockMapChange();


        if (!fSuccess)
            return FALSE;





        fShouldPurge = CacheManagerShouldPurge(dwEstNewElementSize, fWillCreateInstance);
        if (--nIterationCount == 0)
            break;
    }
    return TRUE;
}
// NON_MATCHING: 2412 vs 2400 bytes. Locals now declared in DWARF order; the remaining difference is block placement of
// the fallback / not-thread-3 paths that converge on the 4510 version read (ours tail-duplicates the fUseFallbackData set).
void GroupDefinition::sLoad(const SNO sno, const BOOL fIsExternal, const BOOL fWritable, const BOOL fLockCanFail, void *& lpDefinition, DefDataElementID & idDefinition, const BOOL fFromPending, const BOOL fForPreload)
{







    BOOL fWritableValueToUse = fWritable;



















    BOOL fThreadSafe = dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT);
    BOOL bPrevTrace = SigmaMemorySetTraceAllocations(FALSE);


    SNOElement * ptSNOElement = NULL;
    BOOL fUseFallbackData = FALSE;

    SNOName snoname(GetGroup(), sno);


































    BOOL fNeedToOpen = TRUE;

    BOOL fFirstPendingOnStack = !(dwFlags & (1 << GROUPDEFINITION_HAS_PENDING_SNOS));


    BOOL fAlreadyExists = idDefinition != DEFDATAELEMENTID_INVALID;
    if (!fAlreadyExists)
    {


        if (!AutoPurgeIfNeeded(0, !fFromPending, fLockCanFail, fThreadSafe && !fFromPending))
        {
            lpDefinition = NULL;
            return;
        }



        if (fFromPending)
        {
            PendingDefDataElement * ptDefData;
            LockPendingSNOList();

            idDefinition = ptPendingInstances->ElementNew();
            ptDefData = ptPendingInstances->ElementGet(idDefinition);
            ptDefData->sno = sno;

            dwFlags |= (1 << GROUPDEFINITION_HAS_PENDING_SNOS);

            UnlockPendingSNOList();
        }
        else
        {




            idDefinition = ptInstances->ElementNew();
        }
    }




    if (Prefetch::TakeSNOElement(snoname, ptSNOElement))
    {


        if (ThreadGlobals::Get().m_nThreadIndex == 3)
        {


            if (SNOFilesAsyncIsLocking(ptSNOElement) || SNOFilesGetAsyncStatus(ptSNOElement) == e_pending)
            {

                SNOFilesCloseAsyncRecord(ptSNOElement);

                Blizzard::Lock::Atomic::And(~(SNOENTRY_FLAG_ASYNCHRONOUS | SNOENTRY_FLAG_READING), &ptSNOElement->dwFlags);
            }
            else
            {
                fNeedToOpen = FALSE;
            }
        }
        else
        {
            fNeedToOpen = FALSE;
        }
    }

    if (fNeedToOpen)
    {






















        sEvaluateSNOFileVersion(TRUE, snoname, snoGroupInfo, ptSNOElement, fUseFallbackData);
    }






    if (fUseFallbackData)
    {
        if (fForPreload)
        {

            lpDefinition = NULL;


            if (!fAlreadyExists)
            {

                if (fFromPending)
                {

                    LockPendingSNOList();
                    ptPendingInstances->ElementDeleteByID(idDefinition);
                    UnlockPendingSNOList();
                }
                else
                {


                    ptInstances->ElementDeleteByID(idDefinition);
                }

                idDefinition = DEFDATAELEMENTID_INVALID;
            }


            if (fFirstPendingOnStack)
            {

                dwFlags &= ~(1 << GROUPDEFINITION_HAS_PENDING_SNOS);
            }
            return;
        }





        sReportBadDataToUser(sno);


        snoname.Set(GetGroup(), GetFallbackDataSNO());



        if (!Prefetch::TakeSNOElement(snoname, ptSNOElement))
        {





            ptSNOElement = SNOMLFileOpenForRead(snoname);
        }
    }


















    uint32 dwVersion;
    BOOL fSuccess = SNOFileTryGetVersion(snoname, &dwVersion, NULL, NULL);
























    if (dwVersion != snoGroupInfo.tVersions.GetVersion(SIGMAPLATFORM_UNKNOWN) && fForPreload)
    {


        lpDefinition = NULL;


        if (!fAlreadyExists)
        {

            if (fFromPending)
            {

                LockPendingSNOList();
                ptPendingInstances->ElementDeleteByID(idDefinition);
                UnlockPendingSNOList();
            }
            else
            {


                ptInstances->ElementDeleteByID(idDefinition);
            }

            idDefinition = DEFDATAELEMENTID_INVALID;
        }


        if (fFirstPendingOnStack)
        {

            dwFlags &= ~(1 << GROUPDEFINITION_HAS_PENDING_SNOS);
        }
        SNOFileClose(ptSNOElement);
        return;
    }








    uint32 dwContiguousSize = 0;
    BOOL fContiguous = FALSE;
    if (!fAlreadyExists)
    {

        if (!fWritableValueToUse)
        {

            dwContiguousSize = GetContiguousReadSize(ptSNOElement);
            fContiguous = TRUE;
        }































        if (!AutoPurgeIfNeeded(dwContiguousSize, FALSE, fLockCanFail, fThreadSafe && !fFromPending))
        {

            lpDefinition = NULL;




            if (fFromPending)
            {

                LockPendingSNOList();
                ptPendingInstances->ElementDeleteByID(idDefinition);
                UnlockPendingSNOList();
            }
            else
            {


                ptInstances->ElementDeleteByID(idDefinition);
            }

            idDefinition = DEFDATAELEMENTID_INVALID;


            if (fFirstPendingOnStack)
            {
                dwFlags &= ~(1 << GROUPDEFINITION_HAS_PENDING_SNOS);
            }
            SNOFileClose(ptSNOElement);
            return;
        }










        sCreate(sno, snoname.snoHandle, fContiguous, dwContiguousSize, idDefinition, lpDefinition, FALSE);
    }
    else
    {



        lpDefinition = MemAlloc(dwInstanceSize, 4);
        SigmaMemoryClear(lpDefinition, dwInstanceSize);
    }




    RequiredDefinitionHeader tHeader;
    tHeader.Initialize(sno, fUseFallbackData, fContiguous, fIsExternal, dwFlags & (1 << GROUPDEFINITION_CANT_SHARE_BIT));
    Read(tHeader, lpDefinition, ptSNOElement, fContiguous, dwContiguousSize, &stDefinitionHeapAutoPurgeAlloc);
    SNOFileClose(ptSNOElement);
    Postprocess(lpDefinition);
    ((RequiredDefinitionHeader *)lpDefinition)->dwFlags |= (1 << DH_WAS_POSTPROCESSED_BIT);
    SigmaMemorySetTraceAllocations(bPrevTrace);
}
void GroupDefinition::LockMapChange() const
{






    while (Blizzard::Lock::Atomic::Increment(&pTSDef->nMapChangeLock) < 0)
    {

        Blizzard::Lock::Atomic::Decrement(&pTSDef->nMapChangeLock);


        pTSDef->tMapChangeLock.Enter();
        BOOL fIsChanger = (Blizzard::Thread::CurrentThread() == pTSDef->tCurrentMapChanger);
        pTSDef->tMapChangeLock.Leave();
        if (fIsChanger)
        {
            Blizzard::Lock::Atomic::Increment(&pTSDef->nMapChangeLock);
            break;
        }
    }
}



void GroupDefinition::UnlockMapChange() const
{




    Blizzard::Lock::Atomic::Decrement(&pTSDef->nMapChangeLock);
}










void GroupDefinition::BeginMapChange() const
{




    while (!Blizzard::Lock::Atomic::CompareAndSwap(0, (int32)0x80000000, &pTSDef->nMapChangeLock))
    {
    }


    pTSDef->tMapChangeLock.Enter();
    pTSDef->tCurrentMapChanger = Blizzard::Thread::CurrentThread();
}


void GroupDefinition::EndMapChange() const
{




    while (!Blizzard::Lock::Atomic::CompareAndSwap((int32)0x80000000, 0, &pTSDef->nMapChangeLock))
    {
    }


    pTSDef->tCurrentMapChanger = NULL;
    pTSDef->tMapChangeLock.Leave();
}


void GroupDefinition::LockPendingSNOs() const
{




    pTSDef->tPendingLock.Enter();
}



void GroupDefinition::UnlockPendingSNOs() const
{



    pTSDef->tPendingLock.Leave();
}



void GroupDefinition::LockPendingSNOList() const
{




    pTSDef->tPendingListLock.Enter();
}


void GroupDefinition::UnlockPendingSNOList() const
{



    pTSDef->tPendingListLock.Leave();
}


template <bool bThreadSafe>
DefDataElement * GroupDefinition::HandleError(SNO sno, BOOL fWritable, BOOL fLockCanFail)
{



    SNO snoBadData = GetFallbackDataSNO();
    if (snoBadData == sno)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }











    sReportBadDataToUser(sno);

    DefDataElement * ptDefDataElement = sGet<bThreadSafe, false>(snoBadData, fWritable, fLockCanFail, FALSE);
    RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)ptDefDataElement->lpDefinitionBase;
    ptDefHeader->dwFlags |= (1 << DH_IS_BAD_DATA_BIT);
    return ptDefDataElement;
}
// NON_MATCHING: sGetForLoad<false> has the same instructions with this and the result pointer in
// swapped registers (x22/x23); sGetForLoad<true> is 8 bytes short, its second GetDefinition (after
// MergePendingSNOs) indexes the SNO map with a folded address instead of a separate shift.
template <bool bThreadSafe>
#line 4856
DefDataElement * GroupDefinition::sGetForLoad(
    const SNO sno,
    const BOOL fWritable,
    const BOOL fLockCanFail,
    const BOOL fForPreload)
{
    SNOName snoName(GetGroup(), sno);
    SNOElement * ptSNOElement = SNOFileLookup(snoName);
    if (ptSNOElement == NULL

































        || ptSNOElement->tEntry.snoGroup != snoName.snoGroup)
    {
        if (fForPreload)
            return NULL;
        return HandleError<bThreadSafe>(sno, fWritable, fLockCanFail);
    }










    Blizzard::Lock::Atomic::Or(SNOENTRY_FLAG_LOADING, &ptSNOElement->dwFlags);


    DefDataElement * ptDefData;

    if (!bThreadSafe)
    {

        ptDefData = sCreateAndLoad(sno, FALSE, fLockCanFail, fForPreload);
    }
    else
    {



        UnlockMapChange();



        if ((dwFlags & (1 << GROUPDEFINITION_ASYNC_FLUSH)) && ThreadGlobals::Get().m_nThreadIndex == 0)
        {
            CGameAsyncRenderFlush();
        }


        LockPendingSNOs();

        LockMapChange();

        DefDataElementID dataID = GetDefinition(sno);
        if (dataID != DEFDATAELEMENTID_INVALID)
        {
            ptDefData = ptInstances->ElementGet(dataID);


            UnlockPendingSNOs();
        }
        else
        {
            UnlockMapChange();

            BOOL fFirstPendingOnStack = !(dwFlags & (1 << GROUPDEFINITION_HAS_PENDING_SNOS));


            ptDefData = sCreateAndLoad(sno, FALSE, fLockCanFail, fForPreload);




            if (ptDefData != NULL && ptDefData->lpDefinitionBase != NULL)
            {

                RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)ptDefData->lpDefinitionBase;
                Blizzard::Lock::Atomic::Increment(&ptDefHeader->nLockCount);


                if (fFirstPendingOnStack)
                {
                    MergePendingSNOs();



                    LockMapChange();

                    dataID = GetDefinition(sno);

                    ptDefData = ptInstances->ElementGet(dataID);

                    UnlockMapChange();
                }

                LockMapChange();

                Blizzard::Lock::Atomic::Decrement(&ptDefHeader->nLockCount);

                UnlockPendingSNOs();
            }
            else
            {
                LockMapChange();
                UnlockPendingSNOs();
            }
        }
    }



    if (ptDefData == NULL)
    {
        Blizzard::Lock::Atomic::And(~(SNOENTRY_FLAG_LOADED | SNOENTRY_FLAG_LOADING), &ptSNOElement->dwFlags);
    }
    else
    {
        Blizzard::Lock::Atomic::Or(SNOENTRY_FLAG_LOADED, &ptSNOElement->dwFlags);
        Blizzard::Lock::Atomic::And(~SNOENTRY_FLAG_LOADING, &ptSNOElement->dwFlags);

        if (ThreadGlobals::Get().m_nThreadIndex == 0 && !Prefetch::IsLockingDefinition())
        {
            SNOManager::SetHasBlockedThisFrame();
        }
    }


    // Inferred: a second NULL test (jump-threaded away); 5020 sits at 8 spaces, on the failure path only.
    if (ptDefData == NULL)
    {
        if (!fForPreload && !fLockCanFail)
        {
            ptDefData = HandleError<bThreadSafe>(sno, fWritable, fLockCanFail);
        }
    }


    return ptDefData;
}
#line 5033
template <bool bThreadSafe, bool bUpdateLastUsedFrame>
DefDataElement * GroupDefinition::sGet(const SNO sno, const BOOL fWritable, const BOOL fLockCanFail, const BOOL fForPreload)
{
    BOOL bLoaded = FALSE;
    DefDataElement * ptDefData = NULL;











    DefDataElementID dataID = GetDefinition(sno);
    if (dataID != DEFDATAELEMENTID_INVALID)
    {










        ptDefData = ptInstances->ElementGet(dataID);
    }
    else if (sno < 0)
    {

        return fForPreload ? NULL : HandleError<bThreadSafe>(sno, fWritable, fLockCanFail);
    }
    else if (bThreadSafe)
    {
        UnlockMapChange();

        LockPendingSNOs();




        TDataArrayIterator<PendingDefDataElement> tIt;
        PendingDefDataElement * ptPending;
        while ((ptPending = ptPendingInstances->Iterate(tIt)) != NULL)
        {
            if (ptPending->sno == sno)
                break;
        }
        ptDefData = ptPending;
        UnlockPendingSNOs();
        LockMapChange();
    }







    if (ptDefData != NULL)
    {
        if (bUpdateLastUsedFrame)
        {







            ptDefData->dwLastUsedFrame = GetCurrentFrame(FALSE);
        }
        return ptDefData;
    }

























































    ptDefData = sGetForLoad<bThreadSafe>(sno, fWritable, fLockCanFail, fForPreload);

    if (bUpdateLastUsedFrame && ptDefData != NULL)
    {
        ptDefData->dwLastUsedFrame = GetCurrentFrame(FALSE);
    }



    return ptDefData;
}























void GroupDefinition::sReportBadDataToUser(const SNO sno)
{






    CRefString sSnoString = SNOToString(GetGroup(), sno, FALSE);
    CRefString sErrorString;
    sErrorString.Format("Tried to use bad %s SNO '%s'!\n", GetName(), sSnoString.str());
}



















































































































































































void GroupDefinition::WriteExternalDef(const SNO sno, void * lpDefinition)
{


    Purge(sno, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
    SNOWrite(sno, lpDefinition);
}





















void GroupDefinition::GetSNOName(const void * lpDefinition, SNOName & snoname)
{


    SNO sno = ((const RequiredDefinitionHeader *)lpDefinition)->snoHandle;
    if (((const RequiredDefinitionHeader *)lpDefinition)->dwFlags & (1 << DH_IS_BAD_DATA_BIT))
    {

        sno = GetFallbackDataSNO();
    }
    snoname.Set(GetGroup(), sno);
}


































BOOL GroupDefinition::DeleteIfVersionIsWrong(const SNO sno)
{

    BOOL fDelete = TRUE;
    SNOName snoname(GetGroup(), sno);



    if (SNOFileExistsOnDisk(snoname, TRUE))
    {











        uint32 dwVersion;
        if (SNOFileTryGetVersion(snoname, &dwVersion, NULL, NULL) && dwVersion == snoGroupInfo.tVersions.GetVersion(SIGMAPLATFORM_UNKNOWN))
        {
            fDelete = FALSE;
        }
    }




    if (fDelete)
    {
        SNOMLDelete(snoname);
        SNOFileDelete(snoname, FALSE);
    }


    return fDelete;
}


BOOL GroupDefinition::Purge(const SNO sno, BOOL fAlreadyProtected, BOOL fFromAutoPurge, BOOL fMemoryIsCritical, GlobalCacheType eType)
{











    if (sno >= 0)
    {

        SNOName snoName(GetGroup(), sno);
        SNOElement * ptSNOElement = SNOFileLookup(snoName);
        if (ptSNOElement == NULL)
            return TRUE;







        if (ptSNOElement->tEntry.snoGroup != snoName.snoGroup)
            return TRUE;
    }






    BOOL fThreadSafe = (dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT)) && !fAlreadyProtected;

    if (fThreadSafe)
    {



        if ((dwFlags & (1 << GROUPDEFINITION_ASYNC_FLUSH)) && ThreadGlobals::Get().m_nThreadIndex == 0)
        {

            CGameAsyncRenderFlush();
        }



        BeginMapChange();
    }


    DefDataElementID dataID = GetDefinition(sno);

    if (dataID == DEFDATAELEMENTID_INVALID)
    {

        if (fThreadSafe)
        {
            EndMapChange();
        }
        return TRUE;
    }














    DefDataElement * ptDefData = ptInstances->ElementGet(dataID);
    void * lpDefinition = ptDefData->lpDefinitionBase;
    RequiredDefinitionHeader * ptDefHeader = (RequiredDefinitionHeader *)lpDefinition;
    if (ptDefHeader->nLockCount > 0)
    {
        if (fThreadSafe)
        {
            EndMapChange();
        }
        return TRUE;
    }
    if (fFromAutoPurge)
    {

        uint32 dwCurrentFrame = GetCurrentFrame(TRUE);

        CGameGlobals * ptCGameGlobals = sgtAppGlobals.ptCGameActive ? sgtAppGlobals.ptCGameActive->ptCGameGlobals : NULL;


        if (ptCGameGlobals != NULL && !CGameLoadingScreenIsEnabled() && ptCGameGlobals->dwLoadingScreenDoneFrame < dwCurrentFrame - 1)
        {







            BOOL fRecentlyUsed = ptDefData->dwLastUsedFrame == dwCurrentFrame || ptDefData->dwLastUsedFrame == dwCurrentFrame - 1;
            if (fRecentlyUsed || (((RequiredDefinitionHeader *)ptDefData->lpDefinitionBase)->dwFlags & (1 << DH_PRECACHED_BIT)))
            {
            }























































            if (fRecentlyUsed && !fMemoryIsCritical)
            {



                if (fThreadSafe)
                {
                    EndMapChange();
                }


                GlobalCacheManagerClearPurgeLists(eType);
                return FALSE;
            }
        }
    }







    SGameID idSGame = GameServerCodeLeave_Tracked(TRUE);


    SetDefinition(sno, DEFDATAELEMENTID_INVALID);


    SNOName snoname(GetGroup(), sno);








    Prefetch::Cancel(snoname);


    if (!WasDynamicallyCreated(sno))
    {
        SNOElement * ptSNOElement = SNOFileLookup(snoname);
        if (ptSNOElement != NULL)
        {

            Blizzard::Lock::Atomic::And(~SNOENTRY_FLAG_LOADED, &ptSNOElement->dwFlags);
        }
    }



















    if (dwFlags & (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT))
    {
        Blizzard::Lock::ScopedMutex lock(stGlobalInstanceLock);

        ptInstances->ElementDelete(ptDefData);
    }
    else
    {

        ptInstances->ElementDelete(ptDefData);
    }


    if (fThreadSafe)
    {
        EndMapChange();
    }


    Delete(lpDefinition);




    XMemoryAllocatorInterface * ptMemoryInterface = &stDefinitionHeapAutoPurgeAlloc;
    MemFree(lpDefinition, ptMemoryInterface);
    BOOL fSuccess = GameServerCodeEnter_Track();
    if (!fSuccess)
        _GameServerCodeEnter(idSGame);
    return TRUE;
}
void GroupDefinition::PurgeDefinitionAndSNO(const SNO sno)
{

    SNOName snoname(GetGroup(), sno);

    Prefetch::Cancel(snoname);

    SNOFileDelete(snoname, TRUE);
    Purge(sno, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
}



























void GroupDefinition::MergePendingSNOs()
{





    while (ptPendingInstances->GetTotalCount() > 0)
    {
        PendingDefDataElement * ptPendingDefData;
        BOOL fUseGlobalCache;
        TDataArrayIterator<PendingDefDataElement> tIterator;

        for (uint32 i = 0; i < ptPendingInstances->GetTotalCount(); i++)
        {

            AutoPurgeIfNeeded(0, TRUE, FALSE, FALSE);
        }


        BeginMapChange();


        LockPendingSNOList();


        fUseGlobalCache = dwFlags & (1 << GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT);
        if (fUseGlobalCache)
            stGlobalInstanceLock.Enter();







        while ((ptPendingDefData = ptPendingInstances->Iterate(tIterator)) != NULL)
        {


            SNO sno = ptPendingDefData->sno;


            DefDataElementID idDefinition = ptInstances->ElementNew();
            if (idDefinition != DEFDATAELEMENTID_INVALID)
            {

                DefDataElement * ptDefData = ptInstances->ElementGet(idDefinition);
                ptDefData->id = idDefinition;
                ptDefData->dwLastUsedFrame = ptPendingDefData->dwLastUsedFrame;

                ptDefData->snoGroup = GetGroup();





                ptDefData->lpDefinitionBase = ptPendingDefData->lpDefinitionBase;

                SetDefinition(sno, idDefinition);

                ptPendingInstances->ElementDelete(ptPendingDefData);
            }
        }

        if (fUseGlobalCache)
            stGlobalInstanceLock.Leave();
        UnlockPendingSNOList();
        EndMapChange();
    }
    dwFlags &= ~(1 << GROUPDEFINITION_HAS_PENDING_SNOS);
}
// NON_MATCHING: 1236 vs 1228 bytes. In the original the unlocked GetDefinition loads use a
// register-offset load; here two of them compute the address with a separate add.
BOOL GroupDefinition::Exists(
    const SNO sno,
    BOOL * pfPending) const
{
    if (pfPending != NULL)
    {
        *pfPending = FALSE;
    }

    BOOL fThreadSafe = dwFlags & (1 << GROUPDEFINITION_THREAD_SAFE_BIT);
    if (fThreadSafe)
    {
        LockMapChange();

        BOOL fExists = GetDefinition(sno) != DEFDATAELEMENTID_INVALID;

        UnlockMapChange();

        if (!fExists)
        {
            LockPendingSNOList();

            TDataArrayIterator<PendingDefDataElement> tIterator;
            PendingDefDataElement * ptPendingDefData = ptPendingInstances->Iterate(tIterator);
            while (ptPendingDefData != NULL)
            {
                if (ptPendingDefData->sno == sno)
                {
                    fExists = TRUE;
                    if (pfPending != NULL)
                    {
                        *pfPending = TRUE;
                    }
                    break;
                }
                // The only surviving Iterate record is this one (DWARF call_line 5923); the 5910 copy is tail-merged into it
                ptPendingDefData = ptPendingInstances->Iterate(tIterator);
            }
            UnlockPendingSNOList();

            if (!fExists)
            {
                LockMapChange();


                fExists = GetDefinition(sno) != DEFDATAELEMENTID_INVALID;

                UnlockMapChange();
            }
        }

        return fExists;
    }


    return (GetDefinition(sno) != DEFDATAELEMENTID_INVALID);

}










































































































































































































































































































































void DefinitionIterateSnoReferencesFromFile(const SNOName & snoname, SNOIterateFunc pfuncIterate, void * pData)
{

    GroupDefinition * ptGroupDef = GameDefinitionGet(snoname.snoGroup);
    if (ptGroupDef->CanReferenceOtherSNOs())
    {
        void * lpDefinition = (void *)ptGroupDef->LockR(snoname.snoHandle, FALSE);
        ptGroupDef->IterateSnoReferences(lpDefinition, pfuncIterate, pData);
        ptGroupDef->UnlockR(lpDefinition);
    }
}



























































































void GroupDefinition::IterateSnoReferences(const void * lpDefinition, SNOIterateFunc pfuncIterate, void * pData)
{


    const XTypeDescriptor * ptType = SNOMLGetTypeFromSNOGroup(GetGroup());
    TypeDescriptorField field(ptType, NULL);
    ptType->IterateSNOReferences(lpDefinition, field, pfuncIterate, pData);
}




































































































































void DefinitionStatInitializeForGame()
{
}























void DefinitionStatDisposeForGame()
{
}












































void DefinitionEnableLoadTrace(BOOL bEnable)
{

    sgbEnableLoadTrace = bEnable;
}

BOOL DefinitionIsLoadTraceEnabled()
{
    return sgbEnableLoadTrace;
}


BOOL DefinitionShouldPrecache()
{





    return FALSE;
}

void DefinitionSetCurrentlyPreCaching(BOOL bEnable)
{

    sgbEnablePreCaching = bEnable;
}

BOOL DefinitionCurrentlyPreCaching()
{
    return sgbEnablePreCaching;
}


void DefinitionSetLoadTraceMinTime(float flTimeMillis)
{

    sgflLoadTraceMinTime = flTimeMillis;
}




float GroupDefinition::GetGlobalCacheStreamed()
{








    return 0.0f;
}




void DefinitionPurgeNonRequired()
{


    for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
    {

        GroupDefinition * ptGroupDef = GameDefinitionGet(i);
        if (ptGroupDef == NULL)
            continue;

        RequiredDefinitionHeader * ptHeader;
        TDataArrayIterator<void> tIt(0x3ffff);


        while (ptGroupDef->DefinitionDataArrayIterate(tIt, (void *&)ptHeader) != NULL)
        {

            if (!ptGroupDef->WasDynamicallyCreated(ptHeader->snoHandle) && ptHeader->nLockCount == 0 && !(ptHeader->dwFlags & (1 << DH_DO_NOT_PURGE_CACHE_BIT)))
            {

                ptGroupDef->Purge(ptHeader->snoHandle, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
            }
        }
    }
}
