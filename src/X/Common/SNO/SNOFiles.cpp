// Original file: X/Common/Sno/SNOFiles.cpp
// Functions sit on their original line numbers.

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/array.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Game.h"
#include "SNOFiles.h"
#include "SNOFilesAsync.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XTemplate.h"
#include "../../Sigma/Files/XFiles.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include "../../Sigma/UnicodeText.h"
#include "../../../BlizzardCore/Include/Blizzard/File.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../NintendoSDK/Include/nn/account/account_Types.h"
#include "../../../NintendoSDK/Include/nn/nn_Result.h"
#include "../../Client/Definitions/PreferencesDefinition.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/LanguageRules.h"
#include "../../Sigma/Sigma.h"
#include "../Shell/CmdLine.h"
#include "SNOGroups.h"
#include <string.h>
#include <ctype.h>
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"



namespace Console { namespace IO { class CPKArchiveReader; struct FileInfo; } }

#include "../FileIO/FileIO.h"















enum {
    MAX_FILENAME_LENGTH = 128,
    CONSOLE_HOTFIX_PRIORITY = 12,
    CONSOLE_CORE_COMMON_PRIORITY = 8,
    CONSOLE_CACHE_COMMON_PRIORITY = 8,
    CONSOLE_COMMON_PRIORITY = 6,
    CONSOLE_ACTIVE_CACHE_ACT_PRIORITY = 4,
    CONSOLE_ACTIVE_ACT_PRIORITY = 2,
    CONSOLE_INACTIVE_CACHE_ACT_PRIORITY = -2,
    CONSOLE_INACTIVE_ACT_PRIORITY = -4,
    CONSOLE_CUTSCENE = -6,
    CONSOLE_LOCALE_DIFF = 1,
};

// NintendoSDK headers not adopted in src/ (prototypes from the public SDK).
namespace nn {
namespace account { void Initialize(); nn::Result OpenPreselectedUser(UserHandle * pOutHandle); nn::Result GetUserId(Uid * pOut, const UserHandle & handle); void CloseUser(const UserHandle & handle); }
namespace fs { bool IsSaveDataExisting(const nn::account::Uid & user); nn::Result MountSaveData(const char * name, const nn::account::Uid & user); void Unmount(const char * name); }
}  // namespace nn
extern nn::account::UserHandle unk_71019079f0;  // Game.cpp .bss (2.7.7): preselected user handle opened by FileSystemGetLocale
// BlizzardCore/Include/Blizzard/File.h and Debug (FileAPI.cpp:934).
namespace Blizzard { namespace Debug { struct ErrorStackRecord; typedef ErrorStackRecord * ErrorStack; } }
namespace Blizzard { namespace File { void SetLastErrorStack(Blizzard::Debug::ErrorStack err); } }
// X/Common/Console/IO/SFileWrapper.h (SFileWrapper.cpp:506,536).
#include "../../../BlizzardCore/Include/Blizzard/Mopaq.h"
bool SFileOpenArchive(const char * szFilename, int priority, unsigned int dwFlags, HSARCHIVE * pOpaqueArchive);
bool SFileSetBasePath(const char * szBasePath);
// X/Client/Console/DLCManager.h, X/Client/Console/Utilities.h.
namespace Console {
namespace DLCManager { bool GetLocaleAOCMountPath(XLocale nLocale, CRefString & szMountPath); }
namespace Utilities { XLocale GetSystemLanguage(); }
}  // namespace Console
// X/Client/LocalizedError.h (LocalizedError.cpp:273).
void ReportLocalizedError_MpqOpenError(XLocale eLocale);
// X/Common/SNO/SNOFilesAsync.cpp:284.
void SNOFilesAsyncInitialize();
void SNOFilesAsyncRead(SNOElement * ptSNOElement, SNOFilesAsyncPriority ePriority);
// X/Common/Hyperlink.h:58.
CRefString HyperlinkCreateFromSNO(const SNOName & snoname, LPCSTR szLinkName);
// X/Common/SNO/SNOGroups.cpp.
LPCSTR SNOGroupGetName(const SNOGroup snoGroup);
int32 SNOGroupGetGroups(SNOGroup * pGroups);
const SNOGroupInfo * SNOGroupGetInfo(const SNOGroup snoGroup);
// Declared in X/Sigma/Files/XFilesCore.h (not adopted yet).
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
LPCSTR FileReferenceGetPath(const FileReference & tFileRef);
void FileSplitPath(LPCSTR szPath, LPSTR pszDrive, LPSTR pszDirectory, LPSTR pszFilename, LPSTR pszExtension);
BOOL FileExists(const FileReference & tFileRef);
void FileFindStart(void *& portableState, const FileReference & tBaseRef, LPCSTR szFilename, const BOOL fFindRecursive, const BOOL fIncludeDirs);
BOOL FileFindNext(void * portableState, FileReference & tResult);
void FileFindClose(void *& portableState);
BOOL FileWrite(FileReference & tFileRef, const void * lpBuf, const uint32 dwOffset, const uint32 dwSize, ErrorCode eErrorCodeIfFails);

int32 SNOFileEnumerate(SNOFileInfo & tCriteria, PooledSNONameList * ptList, const BOOL fIgnoreExtraCriteria);
BOOL SNOFileGetName(const SNOName & snoname, CRefString & sSNOName);
CRefString SNOFileGetName(const SNOName & snoname);
inline CRefString SNONameToString(const SNOName & snoname, const BOOL fUsedForXMLFields);
CRefString SNOFilesGetUserDataPath(LPCSTR szBranchName);
CRefString SNOFilesGetLocalePath(const XLocale eXLocale);
CRefString SNOFilesGetLocalCachePath(const XLocale eXLocale);
AsyncRecordRef SNOFileGetAsyncRecord(SNOElement * ptSNOElement);
BOOL SNOFileLoadTextFile(LPCSTR szPath, CHAR *& lpBuffer, uint32 & dwTextLength, CHAR *& lpTextStart);
BOOL SNOPackChunkLoad(SNOPackChunk eSNOPackChunk);
SNO SNOFileLookupGUID(const SNOGroup snoGroup, LPCSTR szPath);
void SNOFileAddEntry(SNOTOC::TOCType eType, const SNOGroup snoGroup, const uint32 uGuid, LPCSTR szFileNameNoExtension);
inline CRefString SNOFilesGetTitleDataPath();
inline void SNOFileGetFilePath(const SNOElement * ptSNOElement, char * szFilePath, int32 nFilePathMaxSize);
static CRefString sSNOEntryGetName(const SNOEntry & tEntry);
static void sFileTableInitialize();
static void sLoadUserDataSNOs();
static HSARCHIVE sOpenSNOPack(LPCSTR szPath, int32 nPriority);
static void sDisplaySNOFileError(ErrorCode eErrorCode, const FileReference & tFileRef, LPCSTR szMessage, LPCSTR szFile, int32 nLine);
static void sOpenSNOPacks();
static uint64 sGetHashFrom8BytesString(const uint8 * pHash);
static void sReportMpqOpenError(const CRefString & sPath);



















































static const char * scszSNOPackNames[23] = {
    "CoreData", "ClientData", "Texture", "Sound", "ServerData", "Common", "CoreCommon", "ClientCommon",
    "ServerCommon", "Act1", "Act2", "Act3", "Act4", "Act5", "PVP", "CacheCommon", "CacheAct1", "CacheAct2",
    "CacheAct3", "CacheAct4", "CacheAct5", "CachePVP", "Hotfixes",
};


























static Console::IO::CPKArchiveReader * sptGlobalArchives[NUM_SNOPACKTYPES];
static Console::IO::CPKArchiveReader * sptLocaleArchives[NUM_SNOPACKTYPES];
static Blizzard::Lock::RMutex stArchiveLock;










static const uint32 SNOHEADER_SIGNATURE = 0xDEADBEEF;

static const char * cgszSNOExtensions[70] = {
    "", ".acr", ".adv", ".aib", ".ais", ".ams", ".ani", ".an2", ".ans", ".app",
    ".hro", ".clt", ".cnv", ".cnl", ".efg", ".enc", "", ".xpl", ".flg", ".fnt",
    ".gam", ".glo", ".lvl", ".lit", ".mrk", ".mon", ".obs", ".prt", ".phy", ".pow",
    "", ".qst", ".rop", ".scn", ".scg", ".scr", ".shm", ".shd", ".shk", ".skl",
    ".snd", ".sbk", ".stl", ".srf", ".tex", ".trl", ".ui", ".wth", ".wrl", ".rcp",
    "", ".cnd", ".trs", ".acc", ".con", ".tme", ".act", ".mat", ".qsr", ".lor",
    ".rev", ".phm", ".mus", ".tut", ".bos", ".ctr", ".aco", ".ant", ".vib", ".dfn",
};

































































static BOOL cgsfSNOGroupNeedsName[70] = {
    FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    TRUE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, TRUE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE,
    FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE,
    FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, TRUE, FALSE, TRUE, TRUE,
    FALSE, FALSE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
};






































































static BOOL sSNOGroupNeedsName(SNOGroup snoGroup)
{




    return cgsfSNOGroupNeedsName[snoGroup];
}


































LPCSTR cgszSnoNoneName = "[sno none]";
LPCSTR cgszInvalidSnoName = "[sno unknown]";

LPCSTR cgszUserDataPathName = "Diablo III";



struct SNOFileHeader {  // size 0x10
    uint32 dwSignature;  // 0x00
    uint32 dwVersion;  // 0x04
    uint32 dwAssetVersion;  // 0x08
    uint32 dwXMLHash;  // 0x0c
};
static_assert(sizeof(SNOFileHeader) == 0x10, "SNOFileHeader");
static const TypeDescriptorField SNOFileHeader_Fields[5] = { TypeDescriptorField("", DT_INT, offsetof(SNOFileHeader, dwSignature), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(SNOFileHeader, dwVersion), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SNOFileHeader, dwAssetVersion), NULL, 1), TypeDescriptorField("", DT_INT, offsetof(SNOFileHeader, dwXMLHash), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(SNOFileHeader), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_SNOFileHeader("SNOFileHeader", 0, SNOFileHeader_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SNOFILEHEADER = &TypeDescriptor_SNOFileHeader;

struct SNOFileReference {  // size 0x1018
    SNOFileReference() { eLocation = LOOSE; FileReferenceInit(tRef, ""); }  // SNOFiles.cpp:473-476; body from the copy inlined into sSNOFileLoadLazyData
    enum Location {
        PACK = 0,
        LOOSE = 1,
    };
    SNOFileReference::Location eLocation;  // 0x00
    FileReference tRef;  // 0x08
};
static_assert(sizeof(SNOFileReference) == 0x1018, "SNOFileReference");

struct SNOLazyData {  // size 0x1048
    void Initialize(const SNOFileReference & _tSNOFileRef, const XLocale _eXLocale, const SNOFileType _eSNOFileType) { pDataBuffer = NULL; tSNOFileRef = _tSNOFileRef; eXLocale = _eXLocale; eSNOFileType = _eSNOFileType; }  // SNOFiles.cpp:491-499
    SNOFileHeader tSNOFileHeader;  // 0x00
    SNOFileReference tSNOFileRef;  // 0x10
    SNOFileType eSNOFileType;  // 0x1028
    XLocale eXLocale;  // 0x102c
    FileIO::File m_tFile{XLOCALE_GLOBAL};  // 0x1030; the implicit constructor passes 1 (how the original spelled the initializer is not recorded)
    void * pDataBuffer;  // 0x1040
};
static_assert(sizeof(SNOLazyData) == 0x1048, "SNOLazyData");

struct SNOElementDetail {  // size 0x10
    SNOLazyData * ptLazyData;  // 0x00
    AsyncRecordRef tAsyncRecord;  // 0x08
};
static_assert(sizeof(SNOElementDetail) == 0x10, "SNOElementDetail");

typedef XGrowableMap<int, int, SNOElement *, SNOElement *> MapSNOToSNOElementPtr;

typedef XGrowableMap<SNOElement *, SNOElement *, SNOElementDetail, SNOElementDetail> MapSNOElementToSNOElementDetail;

struct SNOFilenameHash {  // size 0x8
    int operator==(const SNOFilenameHash & rhs);
    SNOFilenameHash() {}
    SNOFilenameHash(uint8 * pNewHash);
    uint8 * pHash;  // 0x00
};
static_assert(sizeof(SNOFilenameHash) == 0x8, "SNOFilenameHash");

typedef XGrowableMap<SNOFilenameHash, const SNOFilenameHash &, int, int> MapStringToSNOHash;

struct SNOFilename {  // size 0x8
    int operator==(const SNOFilename & rhs);
    SNOFilename() {}
    SNOFilename(LPCSTR szNewName);
    LPCSTR szName;  // 0x00
};
static_assert(sizeof(SNOFilename) == 0x8, "SNOFilename");

struct SNOPackTypeAndPriority {  // size 0xc
    SNOPackType eType;  // 0x00
    int32 nPriority;  // 0x04
    BOOL fLoadLocale;  // 0x08
};
static_assert(sizeof(SNOPackTypeAndPriority) == 0xc, "SNOPackTypeAndPriority");

typedef XGrowableMap<SNOFilename, const SNOFilename &, int, int> MapStringToSNO;

struct SNOElementIterator {  // size 0x18
    POS pos;  // 0x00
    BOOL fInDynElements;  // 0x08
    SNOElement * ptElement;  // 0x10
    SNOElementIterator() : pos(NULL), fInDynElements(FALSE), ptElement(NULL) {}  // body from the copy inlined into SNOFileEnumerate
    SNOElement * operator->();
};
static_assert(sizeof(SNOElementIterator) == 0x18, "SNOElementIterator");

class SNOElementList {  // size 0x70
public:
    SNOElementList();
    ~SNOElementList();
    SNOElement * ElementNew();
    void ElementDelete(SNOElement * ptData);
    void DeleteAll();
    uint32 GetTotalCount() const;
    void SetGlobalCount(uint32 dwGlobalCount);
    SNOElement * Iterate(SNOElementIterator & tIterator);
private:
    SNOElement * atList;  // 0x00
    uint32 dwCount;  // 0x08
    uint32 dwMinFreeSlot;  // 0x0c
    uint32 dwMaxUsedSlot;  // 0x10
    uint32 dwMaxGlobalCount;  // 0x14
    XGrowableList<SNOElement, SNOElement &> tList;  // 0x18
};
static_assert(sizeof(SNOElementList) == 0x70, "SNOElementList");


inline int SNOFilenameHash::operator==(const SNOFilenameHash & rhs)
{


    for (int32 i = 0; i < 8; i++) { if (pHash[i] != rhs.pHash[i]) return FALSE; }
    return TRUE;
}










inline SNOFilenameHash::SNOFilenameHash(uint8 * pNewHash)
{
    pHash = pNewHash;
}






template <>
inline uint32 XHashKey<const SNOFilenameHash &>(const SNOFilenameHash & tName)
{

    return XHashKey<uint32>((uint32)sGetHashFrom8BytesString(tName.pHash));
}






inline int SNOFilename::operator==(const SNOFilename & rhs) { return strcasecmp(szName, rhs.szName) == 0; }






inline SNOFilename::SNOFilename(LPCSTR szNewName)
{
    szName = szNewName;
}






template <>
inline uint32 XHashKey<const SNOFilename &>(const SNOFilename & tName)
{
    CHAR szLowercase[MAX_FILENAME_LENGTH];

    Blizzard::String::Copy(szLowercase, tName.szName, sizeof(szLowercase));
    Blizzard::String::ToLower(szLowercase);
    return XHashKey<const char *>(szLowercase);
}


































































inline SNOElementList::SNOElementList()
{

    atList = NULL; dwCount = 0; dwMinFreeSlot = 0; dwMaxUsedSlot = 0; dwMaxGlobalCount = 0;
}




SNOElementList::~SNOElementList()
{
    delete [] atList;
}

void SNOElementList::ElementDelete(SNOElement * ptData)
{

    for (uint32 i = 0; i <= dwMaxUsedSlot; i++)
    {
        if (&atList[i] == ptData)
        {
            atList[i].tEntry.uGuid = SNOEntry::INVALID_GUID;
            if (i < dwMinFreeSlot)
            {
                dwMinFreeSlot = i;
            }
            break;
        }
    }
}

SNOElement * SNOElementList::ElementNew()
{


    if (dwCount < dwMaxGlobalCount)
    {
        SNOElement & tElement = atList[dwMinFreeSlot++];
        SigmaMemoryClear(&tElement, sizeof(SNOElement));
        dwCount++;

        // Keep dwMinFreeSlot on the first free slot.
        if (dwMaxUsedSlot < dwMinFreeSlot)
        {
            dwMaxUsedSlot = dwMinFreeSlot - 1;
        }
        else
        {

            for (uint32 i = dwMinFreeSlot; i <= dwMaxUsedSlot + 1; i++)
            {
                if (atList[i].tEntry.uGuid == SNOEntry::INVALID_GUID)
                {
                    dwMinFreeSlot = i;
                    break;
                }
            }
        }
        return &tElement;
    }
    else
    {
        SNOElement tElement;
        SigmaMemoryClear(&tElement, sizeof(SNOElement));
        SNOElement * ptElement = (SNOElement *)tList.AddTail_Tracked(tElement);
        return ptElement;
    }

}

void SNOElementList::DeleteAll()

{
    dwCount = 0;
    dwMaxUsedSlot = 0;
    tList.RemoveAll();
    delete [] atList;
    atList = NULL;
    dwMinFreeSlot = 0;

    dwMaxUsedSlot = 0;
    dwMaxGlobalCount = 0;
}
inline uint32 SNOElementList::GetTotalCount() const
{
    return dwCount;
}


void SNOElementList::SetGlobalCount(uint32 dwGlobalCount)
{


    dwMaxGlobalCount = dwGlobalCount + 100;
    atList = new SNOElement[dwMaxGlobalCount];
}
// LINKER-REMOVED: DWARF size 0xcc, ours 0xd8 (unchanged by this layout); address-0 rows put the pos/fInDynElements stores at 766:27/767:38 (12-space indent, inside the ptElement test) and pos = NULL at 797.
SNOElement * SNOElementList::Iterate(SNOElementIterator & tIterator)
{
    if (tIterator.pos == NULL)
    {
        if (tIterator.ptElement == NULL)
        {
            tIterator.pos = (POS)1;
            tIterator.fInDynElements = FALSE;
        }
        else
        {
            return NULL;
        }
    }

    tIterator.ptElement = &atList[(uintptr)tIterator.pos - 1];


    if (!tIterator.fInDynElements)
    {
        BOOL fFound = FALSE;
        if ((uintptr)tIterator.pos - 1 < dwMaxUsedSlot)
        {
            for (uint32 i = (uint32)(uintptr)tIterator.pos; i <= dwMaxUsedSlot; i++)
            {
                if (atList[i].tEntry.uGuid != SNOEntry::INVALID_GUID)
                {
                    tIterator.pos = (POS)(uintptr)(i + 1);
                    fFound = TRUE;
                    break;
                }
            }
        }

        if (!fFound)
        {

            tIterator.pos = NULL;
            if (tList.Count() > 0)
            {
                tIterator.pos = tList.GetHeadPosition();
                tIterator.fInDynElements = TRUE;
            }
        }
    }
    else
    {
        tIterator.ptElement = &tList.GetNext(tIterator.pos);
    }
    return tIterator.ptElement;
}

















































typedef blz::vector<unsigned long, blz::allocator<unsigned long> > ArchiveList;

struct SNOOverFlowNameBuffer {  // size 0x8010
    enum {
        BUFFER_SIZE = 32768,
    };
    char szNameBuffer[32768];  // 0x00
    uint32 dwCurrentOffset;  // 0x8000
    SNOOverFlowNameBuffer * ptNextBuffer;  // 0x8008
};
static_assert(sizeof(SNOOverFlowNameBuffer) == 0x8010, "SNOOverFlowNameBuffer");

class SNOOverflowNames {  // size 0x10
public:
    SNOOverflowNames();
    ~SNOOverflowNames();
    char * AllocateNameBuffer(uint32 dwSize);
    BOOL Contains(const char * szNameBuffer);
    void Clear();
private:
    SNOOverFlowNameBuffer * m_ptHead;  // 0x00
    SNOOverFlowNameBuffer * m_ptTail;  // 0x08
};
static_assert(sizeof(SNOOverflowNames) == 0x10, "SNOOverflowNames");

struct SNOTOCData {  // size 0x598
    SNOElementList * ptSNOElements;  // 0x00
    uint32 dwTOCCountPerGroup[70];  // 0x08
    uint32 dwSNOEntryNameBufferSizes[70];  // 0x120
    CHAR * szSNOEntryNameBuffers[70];  // 0x238
    uint32 dwSNOEntryNameOffsets[70];  // 0x468
    SNOOverflowNames tOverflowNames;  // 0x580
    BOOL fLoaded;  // 0x590
};
static_assert(sizeof(SNOTOCData) == 0x598, "SNOTOCData");

struct SNOGlobals {  // size 0x7dc0
    uint32 dwFullTOCCountPerGroup[70];  // 0x00
    SNOTOCData tTOCData[3];  // 0x118
    SNOElement ** pptSNOToSNOElement;  // 0x11e0
    uint32 dwSNOElementArraySize;  // 0x11e8
    MapStringToSNOHash tStringToSNOMapHash[70];  // 0x11f0
    MapSNOToSNOElementPtr tSNOToSNOElementMap[70];  // 0x34f0
    MapSNOElementToSNOElementDetail tSNOElementToSNOElementDetail;  // 0x57f0
    Blizzard::Lock::RMutex tSNOElementDetailMutex;  // 0x5870
    MapStringToSNO tStringToSNOMap[70];  // 0x5890
    CHAR szDataPath[255];  // 0x7b90
    CHAR szBranch[256];  // 0x7c8f
    uint32 dwSNOCoreDataHash;  // 0x7d90
    ArchiveList tArchiveList;  // 0x7d98
    Act eCurrentAct;  // 0x7db0
    Act ePendingAct;  // 0x7db4
    BOOL fAnyFileToClose;  // 0x7db8
};
static_assert(sizeof(SNOGlobals) == 0x7dc0, "SNOGlobals");








































































static SNOGlobals * sgptSNOGlobals;

























































































































































































static void sSet8BytesStringFromHash(uint64 dwHash, uint8 * pszHash)
{




    pszHash[0] = (uint8)(dwHash >> 56); pszHash[1] = (uint8)(dwHash >> 48); pszHash[2] = (uint8)(dwHash >> 40); pszHash[3] = (uint8)(dwHash >> 32); pszHash[4] = (uint8)(dwHash >> 24); pszHash[5] = (uint8)(dwHash >> 16); pszHash[6] = (uint8)(dwHash >> 8); pszHash[7] = (uint8)dwHash;
}
inline SNOName SNOEntry::ToSNOName() const { return SNOName(snoGroup, uGuid); }  // X/Common/SNO/SNOFiles.h:254; same body as the copy in SNOFilesAsync.cpp




static uint64 sGetHashFrom8BytesString(
    const uint8 * pHash)
{
    uint64 dwHash = 0;
    for (int32 i = 0; i < 8; ++i)
    {
        dwHash <<= 8;
        dwHash |= pHash[i];
    }
    return dwHash;
}
#include "../Console/IO/ConsoleIO_Archive.h"
#include "../../Sigma/StringManip.h"
const char * SNOGetGroupExtension(const SNOGroup snoGroup);
inline LPCSTR SNOEntry::GetName() const { return (LPCSTR)pNameOrHash; }  // X/Common/SNO/SNOFiles.h:266 (inlined into 3 functions; not in the adopted header yet)

static uint64 sSNOFileToHash(
    SNOGroup snoGroup,
    const char * szSNOFile,
    BOOL fWithExtension)
{
    CHAR szFilePath[4096];
    Blizzard::String::Copy(szFilePath, SNOGroupGetName(snoGroup), sizeof(szFilePath));
    strcat_safe(szFilePath, sizeof(szFilePath), "\\");
    strcat_safe(szFilePath, sizeof(szFilePath), szSNOFile);
    if (fWithExtension)
    {
        strcat_safe(szFilePath, sizeof(szFilePath), SNOGetGroupExtension(snoGroup));
    }

    return Console::IO::CPKArchive::GetFileHash(szFilePath);
}






void SNOEntry::SetName(SNOTOC::TOCType eType, const CHAR * szFileNameNoExtension)


{
    SNOTOCData & tTOCData = sgptSNOGlobals->tTOCData[eType];

    if (sSNOGroupNeedsName(snoGroup))
    {
        uint32 dwLen = strlen(szFileNameNoExtension) + 1;

        if (tTOCData.dwSNOEntryNameOffsets[snoGroup] + dwLen > tTOCData.dwSNOEntryNameBufferSizes[snoGroup])
        {





            pNameOrHash = tTOCData.tOverflowNames.AllocateNameBuffer(dwLen);
        }
        else
        {

            pNameOrHash = tTOCData.szSNOEntryNameBuffers[snoGroup] + tTOCData.dwSNOEntryNameOffsets[snoGroup];
            tTOCData.dwSNOEntryNameOffsets[snoGroup] += dwLen;
        }
        strcpy((char *)pNameOrHash, szFileNameNoExtension);
    }
    else
    {

        uint32 dwLen = 8;

        uint64 dwHash = sSNOFileToHash(snoGroup, szFileNameNoExtension, FALSE);

        if (tTOCData.dwSNOEntryNameOffsets[snoGroup] + dwLen > tTOCData.dwSNOEntryNameBufferSizes[snoGroup])
        {





            pNameOrHash = tTOCData.tOverflowNames.AllocateNameBuffer(dwLen);
        }
        else
        {

            pNameOrHash = tTOCData.szSNOEntryNameBuffers[snoGroup] + tTOCData.dwSNOEntryNameOffsets[snoGroup];
            tTOCData.dwSNOEntryNameOffsets[snoGroup] += dwLen;
        }
        sSet8BytesStringFromHash(dwHash, (uint8 *)pNameOrHash);
    }

}




static CRefString sSNOEntryGetName(const SNOEntry & tEntry)
{

    if (sSNOGroupNeedsName(tEntry.snoGroup))
    {  // GetName() leaves no inlined-subroutine DIE here (its one load is hoisted), but only this form keeps tEntry a pointer parameter as the original has it
        return tEntry.GetName();
    }























    else
    {
        uint64 dwHash = sGetHashFrom8BytesString((const uint8 *)tEntry.pNameOrHash);
        return CRefString().Format("0x%.8x%.8x", (uint32)(dwHash >> 32), (uint32)dwHash);
    }




}


bool SFileGetArchiveName(HSARCHIVE OpaqueArchive, char * pNameBuffer, unsigned int dwNameBufferSize);  // X/Common/Console/IO/SFileWrapper.cpp:58
namespace Blizzard { namespace String { char * FindReverse(char * string, char search, size_t stringLength); } }  // BlizzardCore String.cpp:703











static uint64 sSNOEntryGetHash(
    const SNOEntry & tEntry)
{







    return sGetHashFrom8BytesString((const uint8 *)tEntry.pNameOrHash);
}

extern int32 unk_71010aba28;  // Console IO (ConsoleIO.cpp): SNO group being opened, -1 otherwise






bool sSNOEntryOpenFile(SNOElement * ptSNOElement, FileIO::File * ptFile, uint64 dwHash, Console::IO::CPKArchiveReader * ptArchive, Console::IO::FileInfo * ptFileInfo)








{
    bool bFileOpened;
    if (ptArchive != NULL)
    {
        return ptFile->OpenConsoleFile((HSARCHIVE)ptArchive, dwHash, ptFileInfo);
    }

    if (!sSNOGroupNeedsName(ptSNOElement->tEntry.snoGroup))
    {
        uint64 dwMyHash;

        if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
        {

            dwMyHash = Console::IO::CPKArchive::GetSubFileHash(sSNOEntryGetHash(ptSNOElement->tEntry), SNOGetGroupExtension(ptSNOElement->tEntry.snoGroup));
        }
        else
        {

            dwMyHash = Console::IO::CPKArchive::GetSubFileHash(sSNOEntryGetHash(ptSNOElement->tEntry), CRefString().Format("\\%s", ((SNOElementChild *)ptSNOElement)->szChildFile));
        }
        unk_71010aba28 = ptSNOElement->tEntry.snoGroup;
        bFileOpened = ptFile->OpenConsoleFile(0, dwMyHash, NULL);
        unk_71010aba28 = -1;
    }
    else
    {


        if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
        {
            bFileOpened = ptFile->OpenSNO(ptSNOElement->tEntry.ToSNOName());
        }
        else
        {

            bFileOpened = ptFile->OpenChildSNO(ptSNOElement->tEntry.ToSNOName(), ((SNOElementChild *)ptSNOElement)->szChildFile);
        }
    }







    return bFileOpened;
}
// X/Common/Console/IO/SFileWrapper.cpp
Blizzard::Mopaq::FileExistsResult SFileFileExists(HSARCHIVE OpaqueArchive, uint64 dwHash, HSARCHIVE * pExistsInArchive);
Blizzard::Mopaq::FileExistsResult SFileFileExists(HSARCHIVE OpaqueArchive, const char * szFilename, unsigned int, HSARCHIVE * pExistsInArchive);

BOOL sSNOEntryExists(
    SNOElement * ptSNOElement,
    HSARCHIVE * phArchiveExistsIn)
{

    if (!sSNOGroupNeedsName(ptSNOElement->tEntry.snoGroup))
    {
        uint64 dwHash;

        if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
        {

            dwHash = Console::IO::CPKArchive::GetSubFileHash(sSNOEntryGetHash(ptSNOElement->tEntry), SNOGetGroupExtension(ptSNOElement->tEntry.snoGroup));
        }
        else
        {

            dwHash = Console::IO::CPKArchive::GetSubFileHash(sSNOEntryGetHash(ptSNOElement->tEntry), CRefString().Format("\\%s", ((SNOElementChild *)ptSNOElement)->szChildFile));
        }

        unk_71010aba28 = ptSNOElement->tEntry.snoGroup;
        BOOL fFileExists = SFileFileExists(0, dwHash, phArchiveExistsIn);
        unk_71010aba28 = -1;
        return fFileExists;
    }













    char filename[4096];
    if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
    {
        Blizzard::String::Format(filename, sizeof(filename), "%s\\%s%s",
            SNOGroupGetName(ptSNOElement->tEntry.snoGroup),
            ptSNOElement->tEntry.GetName(),
            SNOGetGroupExtension(ptSNOElement->tEntry.snoGroup));
    }
    else
    {
        Blizzard::String::Format(filename, sizeof(filename), "%s\\%s\\%s", SNOGroupGetName(ptSNOElement->tEntry.snoGroup), ptSNOElement->tEntry.GetName(), ((SNOElementChild *)ptSNOElement)->szChildFile);
    }




    BOOL fFileExists = SFileFileExists(0, filename, 0, phArchiveExistsIn);
    return fFileExists;













}

XLocale sGetArchiveLocale(
    HSARCHIVE hArchive)
{

    CHAR szMPQPath[4096];
    if (SFileGetArchiveName(hArchive, szMPQPath, sizeof(szMPQPath)))
    {
        const CHAR * szMPQName = Blizzard::String::FindReverse(szMPQPath, '\\', sizeof(szMPQPath));

        szMPQName = szMPQName ? szMPQName + 1 : szMPQPath;










        for (XLocale eXLocale = XLOCALE_LOCALE_START; eXLocale < NUM_XLOCALES; eXLocale++)
        {
            LPCSTR lpszLocaleName = XLocaleToString(eXLocale);


            if (strlen(lpszLocaleName) <= strlen(szMPQName) &&
                strncmp(szMPQName, lpszLocaleName, strlen(lpszLocaleName)) == 0)
            {
                return eXLocale;
            }
        }
    }


    return XLOCALE_GLOBAL;
}















inline SNOFileInfo::SNOFileInfo()
{
    snoname.snoGroup = SNOGROUP_NONE;
    snoname.snoHandle = SNO_NONE;
    dwVersion = ASSET_VERSION_NONE;
    dwAssetVersion = ASSET_VERSION_NONE;
    eSNOFileType = SNOFILETYPE_INVALID;
    eXLocale = XLOCALE_INVALID;
    pszPath = NULL;
    dwXMLHash = 0;
    fReadOnly = FALSE;
}










static char s_szConsoleDataPathOverride[256] = "data:";












const char * SNOFilesGetConsoleDataPathOverride()
{
    return s_szConsoleDataPathOverride;
}







static SNOElement * sGetSNOElement(SNOName snoname)
{
    uint32 dwIndex = snoname.snoHandle;




    if (dwIndex >= sgptSNOGlobals->dwSNOElementArraySize)
    {
        if (snoname.snoGroup == SNOGROUP_NONE || snoname.snoGroup >= MAX_SNO_GROUPS || snoname.snoHandle == SNO_NONE)
        {
            return NULL;
        }







        SNOElement * ptSNOElement;
        if (!sgptSNOGlobals->tSNOToSNOElementMap[snoname.snoGroup].Lookup(snoname.snoHandle, ptSNOElement))
            return NULL;
        return (ptSNOElement->tEntry.snoGroup == snoname.snoGroup) ? ptSNOElement : NULL;
    }

    return sgptSNOGlobals->pptSNOToSNOElement[dwIndex];
}



BOOL SetSNOElement(
    SNOName snoname,
    SNOElement * ptSNOElement)
{
    uint32 dwIndex = snoname.snoHandle;

    if (dwIndex >= sgptSNOGlobals->dwSNOElementArraySize)
    {
        MapSNOToSNOElementPtr & tElementMap = sgptSNOGlobals->tSNOToSNOElementMap[snoname.snoGroup];
        SNOElement * ptSNOElementLookup;
        BOOL bExists = tElementMap.Lookup(dwIndex, ptSNOElementLookup);


        if (ptSNOElement == NULL)
        {
            if (!bExists)
            {
                return FALSE;
            }

            tElementMap.RemoveAt(dwIndex);
            return TRUE;
        }


        if (bExists)
        {
            return FALSE;
        }

        tElementMap.SetAt_Tracked(dwIndex, ptSNOElement);
        return TRUE;
    }

    if (dwIndex != SNOEntry::INVALID_GUID && sgptSNOGlobals->pptSNOToSNOElement[dwIndex] && ptSNOElement)
    {
        return FALSE;
    }

    sgptSNOGlobals->pptSNOToSNOElement[dwIndex] = ptSNOElement;
    return TRUE;
}




































































































































































































































blz::array<int, 13> g_tNX64TextLocales = { { XLOCALE_GERMAN, XLOCALE_ENGLISH_US, XLOCALE_SPANISH_EU, XLOCALE_SPANISH_LATAM, XLOCALE_FRENCH, XLOCALE_ITALIAN, XLOCALE_POLISH, XLOCALE_PORTUGUESE_BRAZIL, XLOCALE_RUSSIAN, XLOCALE_JAPANESE, XLOCALE_UNK_19, XLOCALE_CHINESE_TAIWAN, XLOCALE_KOREAN } };











blz::array<int, 14> g_tNX64AudioLocales = { { XLOCALE_GERMAN, XLOCALE_ENGLISH_US, XLOCALE_ENGLISH_EU, XLOCALE_SPANISH_EU, XLOCALE_SPANISH_LATAM, XLOCALE_FRENCH, XLOCALE_ITALIAN, XLOCALE_POLISH, XLOCALE_PORTUGUESE_BRAZIL, XLOCALE_RUSSIAN, XLOCALE_JAPANESE, XLOCALE_UNK_19, XLOCALE_CHINESE_TAIWAN, XLOCALE_KOREAN } };













void SNOFilesInitialize(LPCSTR szDataPath, const SigmaPlatform eSigmaPlatform, const XLocale eXLocale)
{



    BOOL fAssetClientStarted = FALSE;










    sgptSNOGlobals = new (SigmaMemoryNew(sizeof(SNOGlobals), 4, NULL, TRUE)) SNOGlobals();

    SigmaMemoryClear(sgptSNOGlobals->dwFullTOCCountPerGroup, sizeof(sgptSNOGlobals->dwFullTOCCountPerGroup));
    for (int32 i = 0; i < SNOTOC::TT_COUNT; i++)
    {
        sgptSNOGlobals->tTOCData[i].ptSNOElements = new SNOElementList;
        SigmaMemoryClear(sgptSNOGlobals->tTOCData[i].dwTOCCountPerGroup, sizeof(sgptSNOGlobals->tTOCData[i].dwTOCCountPerGroup));


        SigmaMemoryClear(sgptSNOGlobals->tTOCData[i].dwSNOEntryNameBufferSizes, sizeof(sgptSNOGlobals->tTOCData[i].dwSNOEntryNameBufferSizes));
        SigmaMemoryClear(sgptSNOGlobals->tTOCData[i].szSNOEntryNameBuffers, sizeof(sgptSNOGlobals->tTOCData[i].szSNOEntryNameBuffers));
        SigmaMemoryClear(sgptSNOGlobals->tTOCData[i].dwSNOEntryNameOffsets, sizeof(sgptSNOGlobals->tTOCData[i].dwSNOEntryNameOffsets));
    }



    Blizzard::String::Copy(sgptSNOGlobals->szDataPath, szDataPath, sizeof(sgptSNOGlobals->szDataPath));
    UnicodeText::SetLocale(eXLocale);

    {
        XLocale eDefaultLocale = XLOCALE_ENGLISH_US;
        XLocale eDesiredLocale = Console::Utilities::GetSystemLanguage();






        if (UsePreselectedUser())
        {
            nn::account::Uid tUserId;

            char szMountName[4096];
            nn::account::Initialize();






            nn::Result IdResult = nn::account::GetUserId(&tUserId, unk_71019079f0);





            Blizzard::String::Format(szMountName, sizeof(szMountName), "save%d", 0);

            if (nn::fs::IsSaveDataExisting(tUserId))
            {
                nn::Result MountResult = nn::fs::MountSaveData(szMountName, tUserId);
                if (MountResult.IsSuccess())
                {
                    char szFilePath[4096];
                    const char * szFilename = "prefs.dat";
                    Blizzard::String::Format(szFilePath, sizeof(szFilePath), "%s:/%s", szMountName, szFilename);

                    Prefs::Load(szFilePath);
                    const GameplayPrefs & GameplayPrefs = Prefs::GetGameplayPrefs();


                    eDesiredLocale = GameplayPrefs.GetSelectedTextLocale();

                    nn::fs::Unmount(szMountName);
                }
            }



        }








        BOOL fFound = FALSE;
        for (int32 i = 0; i < (int32)g_tNX64TextLocales.size(); i++)
            if (eDesiredLocale == g_tNX64TextLocales[i]) { fFound = TRUE; break; }
        if (!fFound)
            eDesiredLocale = eDefaultLocale;


        UnicodeText::SetStringDisplayLocale(eDesiredLocale);
    }

    SigmaPlatformSet(eSigmaPlatform);


    sgptSNOGlobals->tSNOElementToSNOElementDetail.SetHashTableSize_Tracked(65536);
    sgptSNOGlobals->tSNOElementToSNOElementDetail.SetBlockCount_Tracked(65536);
    sgptSNOGlobals->tSNOElementToSNOElementDetail.SetGrow(XALLOC_GROW_LINEAR, 16384);





    sgptSNOGlobals->tSNOElementToSNOElementDetail.SetShrinkable(TRUE);







    sgptSNOGlobals->eCurrentAct = sgptSNOGlobals->ePendingAct = ACT_INVALID;



    sgptSNOGlobals->fAnyFileToClose = FALSE;


















    sFileTableInitialize();

    SNOFilesAsyncInitialize();

    sLoadUserDataSNOs();









































































































    SNOPackChunkLoad(SNOPACKCHUNK_ACT1);
    SNOPackChunkLoad(SNOPACKCHUNK_ACT2);
    SNOPackChunkLoad(SNOPACKCHUNK_ACT3);
    SNOPackChunkLoad(SNOPACKCHUNK_ACT4);
    SNOPackChunkLoad(SNOPACKCHUNK_ACT5);

}





















































































































































































































































































void SNOFilesAsyncDispose();  // X/Common/Sno/SNOFilesAsync.cpp
void SNOFilesCloseAll(); void SNOFilesMapCheckConsistency();
static void sFileTableDispose();
void SNOFilesDispose()
{








    SNOFilesMapCheckConsistency();

    SNOFilesAsyncDispose();

    sFileTableDispose();


















    delete [] sgptSNOGlobals->pptSNOToSNOElement;
    sgptSNOGlobals->pptSNOToSNOElement = NULL;

    for (int32 i = 0; i < SNOTOC::TT_COUNT; i++)
    {

        delete sgptSNOGlobals->tTOCData[i].ptSNOElements;
        sgptSNOGlobals->tTOCData[i].ptSNOElements = NULL;

        for (int32 j = 0; j < MAX_SNO_GROUPS; j++)
        {
            delete [] sgptSNOGlobals->tTOCData[i].szSNOEntryNameBuffers[j];
        }
    }


    delete sgptSNOGlobals;
    sgptSNOGlobals = NULL;
    SNOGroupsDispose();
}













const char * SNOGetGroupExtension(
    const SNOGroup snoGroup)
{

    return cgszSNOExtensions[snoGroup];
}















































































































































































































































































































































































































































































































































































































































#include "../Console/IO/ConsoleIO.h"
#include "../../Client/Prefetch.h"
#include "../../../BlizzardCore/Include/Blizzard/Error.h"
// X/Common/SNO/SNOFilesAsync.cpp:938, X/Sigma/Files/XFilesCore.cpp and BlizzardCore FileAPI.cpp
AsyncStatus SNOFilesBlockUntilCompleted(SNOElement * ptSNOElement);
BOOL FileOpen(FileReference & tFileRef, uint32 dwAccessFlags);
void FileClose(FileReference & tFileRef);
int32 FileReferenceGetRefCount(const FileReference & tFileRef);
BOOL FileRead(FileReference & tFileRef, void * lpBuf, const uint32 dwOffset, const uint32 dwSize, const ErrorCode eErrorCodeIfFails);
namespace Blizzard { namespace File { void SetLastErrorStack(Blizzard::Debug::ErrorStackRecord * stack); int GetLastError(); } }
BOOL FileGetSize(const FileReference & tFileRef, uint32 & dwSize);
LPCSTR FileReferenceGetPath(const FileReference & tFileRef);
Blizzard::Mopaq::ErrorCode SFileGetLastError();  // X/Common/Console/IO/SFileWrapper.cpp
static BOOL sSNOFileOpenForRead(SNOElement * ptSNOElement, uint64 dwHash, Console::IO::CPKArchiveReader * ptArchive, Console::IO::FileInfo * ptFileInfo);
static void sSNOFileClose(SNOElement * ptSNOElement);
static BOOL sReadLocalSNOFileHeader(SNOFileHeader & tSNOFileHeader, FileReference & tRef);
static BOOL sSNOHeaderFromFileHeader(const SNOFileHeader & tSNOHeaderOnDisk, SNOFileHeader & tSNOFileHeader);
static void sDisplaySNOFileReadError(LPCSTR szFile, int32 nLine, SNOLazyData * ptLazyData, LPCSTR szMessage, uint64 uReadOffset, uint64 uReadSize, uint64 uReadBytes, BOOL bRaiseError);
static void sDisplaySNOFileError(ErrorCode eErrorCode, const SNOFileReference & tSNOFileRef, LPCSTR szMessage, LPCSTR szFile, int32 nLine);
static void sDisplayFileReadError(LPCSTR szFile, int32 nLine, FileReference & tFileRef, LPCSTR szMessage, uint64 uReadOffset, uint64 uReadSize);
static void sSNOFileAssignLazyData(SNOElement * ptSNOElement, SNOLazyData * ptLazyData);
SNOLazyData * SNOFileGetLazyData(SNOElement * ptSNOElement);
void SNOFilesCloseAsyncRecord(SNOElement * ptSNOElement);  // X/Common/SNO/SNOFilesAsync.cpp:1014
XLocale sGetArchiveLocale(HSARCHIVE hArchive);
void SNOFileGetFilenameForLocalCacheFile(FileReference & tRef, const XLocale eXLocale, const SNOName & snoname, BOOL fConsoleUsesHandle);

BOOL FileDelete(FileReference & tFileRef);  // X/Sigma/Files/XFilesCore.cpp
BOOL SNODeleteAdditionalFiles(const char * szPath);  // X/Common/SNO/SNOML_X.cpp
static void sSNOFileMapDelete(SNOElement * ptSNOElement, BOOL fDeleteFromElements);







































static void sFileTableInitialize()
{
    {
        CHAR szOriginalPath[4096];
        FileReference tRef;
        BOOL fFoundDataPath;



















        Blizzard::String::Copy(szOriginalPath, sgptSNOGlobals->szDataPath, sizeof(szOriginalPath));













































        FileReferenceInit(tRef, SNOFilesGetTitleDataPath());
        fFoundDataPath = FileExists(tRef);
        Blizzard::File::SetWorkingDirectory(szOriginalPath);

        if (!fFoundDataPath)
        {
            DisplayInternalError(NULL, "",

                                 __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }

    {
        CHAR szWorkingDirectory[4096];
        if (Blizzard::File::GetWorkingDirectory(szWorkingDirectory, sizeof(szWorkingDirectory)))
        {


            SFileSetBasePath(szWorkingDirectory);
        }
    }











    Blizzard::String::Copy(sgptSNOGlobals->szBranch, "2_7_7", sizeof(sgptSNOGlobals->szBranch));





































    sOpenSNOPacks();











    SigmaMemoryDumpOSStats();

    {



























        CRefString sDeclensionFile = CRefString().Format("%s\\declension.txt", SNOFilesGetLocalePath(XLOCALE_RUSSIAN).str());

        uint32 dwFileSize;
        CHAR * pBuffer, * pTextStart;
        if (SNOFileLoadTextFile(sDeclensionFile.str(), pBuffer, dwFileSize, pTextStart))
        {
            LanguageRules::InitializeDeclensions(pTextStart);
            SigmaMemoryFree(pBuffer, NULL);
            pBuffer = NULL;
        }
    }
}













// NON_MATCHING: 4/137; same code, but fInDynElements and ptElement of the iterator are zeroed in the opposite order (header phi order, as in SNOFileEnumerate)
void SNOFilesCloseAll()

{
    for (int32 i = 0; i < SNOTOC::TT_COUNT; i++)
    {
        SNOElementList * ptSNOElements = sgptSNOGlobals->tTOCData[i].ptSNOElements;
        SNOElementIterator tIt;
        while (ptSNOElements->Iterate(tIt))
        {
            SNOElement * ptSNOElement = tIt.ptElement;
            SNOLazyData * ptLazyData = SNOFileGetLazyData(ptSNOElement);
            if (ptLazyData)
            {
                if (ptLazyData->tSNOFileRef.eLocation == SNOFileReference::LOOSE)
                {
                    FileReference & tRef = ptLazyData->tSNOFileRef.tRef;

                    int32 nRefCount = FileReferenceGetRefCount(tRef);
                    if (nRefCount)
                    {


                        while (FileReferenceGetRefCount(tRef))
                        {
                            FileClose(tRef);
                        }
                    }
                }

                sSNOFileAssignLazyData(ptSNOElement, NULL);
            }
        }
    }
}
bool SFileCloseArchive(HSARCHIVE OpaqueArchive);  // X/Common/Console/IO/SFileWrapper.cpp:519
bool SFileGetArchiveFullName(HSARCHIVE OpaqueArchive, char * pNameBuffer, unsigned int dwNameBufferSize);  // SFileWrapper.cpp:527
static void sSNOFileMapDispose();

static void sFileTableDispose()
{

    sSNOFileMapDispose();

    LanguageRules::DisposeDeclensions();







    for (uint32 i = 0; i < sgptSNOGlobals->tArchiveList.size(); i++)
    {
        HSARCHIVE archive = sgptSNOGlobals->tArchiveList[i];

        char fileName[512];
        fileName[0] = 0;
        SFileGetArchiveFullName(archive, fileName, sizeof(fileName) - 1);








        SFileCloseArchive(archive);
    }



    sgptSNOGlobals->tArchiveList.clear();











    SNOFilesCloseAll();


    // The line table puts all three calls on line 3490 (no loop variable in the DWARF).
    sgptSNOGlobals->tTOCData[0].ptSNOElements->DeleteAll(); sgptSNOGlobals->tTOCData[1].ptSNOElements->DeleteAll(); sgptSNOGlobals->tTOCData[2].ptSNOElements->DeleteAll();
}




























// X/Common/SNO/SNOML_X.cpp
uint32 SNOMLGetAssetVersion(const SNOName & snoname, BOOL fForceUpdate);
// Body written from its callers; the reference header has no body (TypeDescriptor.h:490).
inline void XTypeDescriptor::Byteswap(void * lpDefinitionDst, void * lpDefinitionSrc) const { Byteswap(lpDefinitionDst, lpDefinitionSrc, lpDefinitionSrc, TypeDescriptorField(this, NULL)); }
static void sSNOFileFillInHeader(SNOFileHeader & tSNOFileHeader, const SNOName & snoname)
{


    const SNOGroupInfo * ptSNOGroupInfo = SNOGroupGetInfo(snoname.snoGroup);


    uint32 dwXMLAssetVersion = SNOMLGetAssetVersion(snoname, TRUE);














    tSNOFileHeader.dwSignature = SNOHEADER_SIGNATURE;
    tSNOFileHeader.dwVersion = ptSNOGroupInfo->tVersions.GetVersion(geSigmaPlatform);
    tSNOFileHeader.dwAssetVersion = dwXMLAssetVersion;
}









































































static void sConvertRefToChild(
    FileReference & tRef,
    const char * szChildFile)
{
    uint32 fileNameLen = Blizzard::String::FindExtensionWithLeadingDot(tRef.szPath) - tRef.szPath;
    CRefString strFolder(tRef.szPath, fileNameLen);

    Blizzard::String::Format(tRef.szPath, sizeof(tRef.szPath), "%s\\%s", strFolder.str(), szChildFile);
}




static BOOL sResolveFileRef(
    const SNOElement * ptSNOElement,
    BOOL fCompileIfNecessary,
    SNOFileReference & tSNOFileRef,
    XLocale & eLocale,
    SNOFileType & eSNOFileType)
{





    const SNOEntry & tEntry = ptSNOElement->tEntry;

    SNOName snoname(tEntry.snoGroup, tEntry.uGuid);

    if (tEntry.snoGroup == STRINGLIST_SNO)
    {
        eLocale = UnicodeText::GetStringDisplayLocale();
    }
    else
    {
        eLocale = UnicodeText::GetCurrentLocale();
    }

    const SNOGroupInfo * ptSNOGroupInfo = SNOGroupGetInfo(tEntry.snoGroup);


    FileReference tRef;
    switch (ptSNOGroupInfo->eSNODataType)
    {
    case SNODATATYPE_USERDATA:




        SNOFileGetFilenameForLocalCacheFile(tRef, XLOCALE_GLOBAL, snoname, FALSE);

        if (ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE)
        {
            sConvertRefToChild(tRef, ((const SNOElementChild *)ptSNOElement)->szChildFile);
        }

        if (FileExists(tRef))
        {
            tSNOFileRef.tRef = tRef;
            tSNOFileRef.eLocation = SNOFileReference::LOOSE;
            eLocale = XLOCALE_GLOBAL;
            eSNOFileType = SNOFILETYPE_LOCAL;
            return TRUE;
        }


        SNOFileGetFilenameForLocalCacheFile(tRef, XLOCALE_GLOBAL, snoname, TRUE);

        if (ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE)
        {
            sConvertRefToChild(tRef, ((const SNOElementChild *)ptSNOElement)->szChildFile);
        }

        if (FileExists(tRef))
        {
            tSNOFileRef.tRef = tRef;
            tSNOFileRef.eLocation = SNOFileReference::LOOSE;
            eLocale = XLOCALE_GLOBAL;
            eSNOFileType = SNOFILETYPE_LOCAL;
            return TRUE;
        }
        break;

    case SNODATATYPE_TITLE:






























































































































































































        tSNOFileRef.eLocation = SNOFileReference::LOOSE;






















        {
            HSARCHIVE hArchive = 0;
            if (sSNOEntryExists((SNOElement *)ptSNOElement, &hArchive))
            {
                eLocale = sGetArchiveLocale(hArchive);
                tSNOFileRef.eLocation = SNOFileReference::PACK;
                eSNOFileType = SNOFILETYPE_SNOPACK;
                return TRUE;
            }
        }
        break;
    }





    return FALSE;
}




static BOOL sLoadFileHeader(
    SNOElement * ptSNOElement,

    uint64 dwHash,
    Console::IO::CPKArchiveReader * ptArchive,
    Console::IO::FileInfo * ptFileInfo)


{



    if (ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE)
    {
        return TRUE;
    }


















    SNOLazyData * ptLazyData = SNOFileGetLazyData(ptSNOElement);
    if (ptLazyData == NULL)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    bool bFileAlreadyOpen = ptLazyData->m_tFile.IsAlreadyOpen();


    if (!bFileAlreadyOpen && !sSNOFileOpenForRead(ptSNOElement, dwHash, ptArchive, ptFileInfo))
    {



        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    if (ptLazyData->eSNOFileType == SNOFILETYPE_SNOPACK)
    {




        SNOFileHeader tSNOHeaderOnDisk;
        uint64 dwBytesToRead = sizeof(tSNOHeaderOnDisk);
        uint64 dwBytesRead = 0;
        if (!ptLazyData->m_tFile.ReadFile(&tSNOHeaderOnDisk, dwBytesToRead, &dwBytesRead) || dwBytesRead < dwBytesToRead)
        {
            sDisplaySNOFileReadError(NULL, 0, ptLazyData, "", ptLazyData->m_tFile.GetSNOFileStartingLocation(), dwBytesToRead, dwBytesRead, TRUE);
            goto error;
        }




        if (!sSNOHeaderFromFileHeader(tSNOHeaderOnDisk, ptLazyData->tSNOFileHeader))
        {
            goto error;
        }
    }
    else
    {




        BOOL bSuccess = sReadLocalSNOFileHeader(ptLazyData->tSNOFileHeader, ptLazyData->tSNOFileRef.tRef);
        if (!bSuccess)
        {
            goto error;
        }
    }


    if (!bFileAlreadyOpen)
    {
        sSNOFileClose(ptSNOElement);
    }
    return TRUE;

error:  // label name invented

    if (!bFileAlreadyOpen)
    {
        sSNOFileClose(ptSNOElement);
    }
    return FALSE;
}




static SNOLazyData * sSNOFileLoadLazyData(
    SNOElement * ptSNOElement,
    BOOL fCompileIfNecessary,

    Console::IO::CPKArchiveReader * ptArchive,
    Console::IO::FileInfo * ptFileInfo)


{

    int32 nThreadIndex = ThreadGlobals::Get().m_nThreadIndex;
    if (nThreadIndex != THREADINDEX_ASYNCLOAD)
    {
        if ((ptSNOElement->dwFlags & SNOENTRY_FLAG_ASYNCHRONOUS) && (ptSNOElement->dwFlags & SNOENTRY_FLAG_READING))
        {

            if (SNOFilesBlockUntilCompleted(ptSNOElement) == e_canceled)
            {
                Blizzard::Lock::Atomic::And(~(SNOENTRY_FLAG_ASYNCHRONOUS | SNOENTRY_FLAG_READING), &ptSNOElement->dwFlags);

                Prefetch::WasCancelled(ptSNOElement->tEntry.ToSNOName());









                SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, fCompileIfNecessary, ptArchive, ptFileInfo);





















                BOOL bResult = sSNOFileOpenForRead(ptSNOElement, ptFileInfo ? ptFileInfo->dwHash : 0, ptArchive, ptFileInfo);
            }
            else
            {





                Blizzard::Lock::Atomic::And(~SNOENTRY_FLAG_READING, &ptSNOElement->dwFlags);
            }
        }
    }











    SNOLazyData * ptLazyData = SNOFileGetLazyData(ptSNOElement);
    if (ptLazyData)
    {
        return ptLazyData;
    }

























    {
        SNOFileReference tSNOFileRef;
        XLocale eXLocale;
        SNOFileType eSNOFileType;





        BOOL fValid = TRUE;


        if (ptArchive)
        {

            tSNOFileRef.eLocation = SNOFileReference::PACK;
            eSNOFileType = SNOFILETYPE_SNOPACK;
            eXLocale = UnicodeText::GetCurrentLocale();
        }
        else
        {
            fValid = sResolveFileRef(ptSNOElement, fCompileIfNecessary, tSNOFileRef, eXLocale, eSNOFileType);
        }







        if (!fValid)
        {
            return NULL;
        }


        ptLazyData = SNOFileGetLazyData(ptSNOElement); if (!ptLazyData)
        {
            ptLazyData = new (SigmaMemoryNew(sizeof(SNOLazyData), 4, NULL, TRUE)) SNOLazyData();
            sSNOFileAssignLazyData(ptSNOElement, ptLazyData);
            ptLazyData->Initialize(tSNOFileRef, eXLocale, eSNOFileType);

            if (!ptArchive && !sLoadFileHeader(ptSNOElement, ptFileInfo ? ptFileInfo->dwHash : 0, ptArchive, ptFileInfo))
            {





                sSNOFileAssignLazyData(ptSNOElement, NULL);
                return NULL;
            }
        }
    }

































































































    return SNOFileGetLazyData(ptSNOElement);
}
















SNOLazyData * SNOFileGetLazyData(SNOElement * ptSNOElement);
static void sSNOHeaderToFileHeader(const SNOFileHeader & tSNOFileHeader, SNOFileHeader & tSNOHeaderOnDisk);

BOOL SNOFileWriteHeader(FileReference & tRef, SNOElement * ptSNOElement, uint32 dwXMLHash)
{



    SNOFileHeader tSNOFileHeaderLocal;

    SNOLazyData * ptLazyData = SNOFileGetLazyData(ptSNOElement);
    SNOFileHeader * ptSNOFileHeader = ptLazyData ? &ptLazyData->tSNOFileHeader : &tSNOFileHeaderLocal;




    SNOName snoname(ptSNOElement->tEntry.snoGroup, ptSNOElement->tEntry.uGuid);
    sSNOFileFillInHeader(*ptSNOFileHeader, snoname);

    ptSNOFileHeader->dwXMLHash = dwXMLHash;


    SNOFileHeader tSNOHeaderOnDisk;
    sSNOHeaderToFileHeader(*ptSNOFileHeader, tSNOHeaderOnDisk);
    if (!FileWrite(tRef, &tSNOHeaderOnDisk, 0, sizeof(SNOFileHeader), ERROR_FILE_WRITE))
    {
        sDisplaySNOFileError(ERROR_FILE_WRITE, tRef, "Could not write SNO header", __FILE__, __LINE__);
        return FALSE;
    }

    return TRUE;
}













































void SNOFileStartAsynchronousRead(SNOElement * ptSNOElement, SNOFilesAsyncPriority ePriority)
{





    SNOFilesAsyncRead(ptSNOElement, ePriority);

    if (SNOFileGetAsyncRecord(ptSNOElement) != NULL)
    {



        Blizzard::Lock::Atomic::Or(SNOENTRY_FLAG_ASYNCHRONOUS | SNOENTRY_FLAG_READING, &ptSNOElement->dwFlags);
    }
}



SNOFileStatResults SNOFileAsyncGenerateStats(
    SNOElement * ptSNOElement,

    uint64 dwHash,
    void * ptArchive,
    void * ptFileInfo,

    SNOFileAsyncStats & tStats)
{
    BOOL bPrevTrace = SigmaMemorySetTraceAllocations(FALSE);


    SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, (Console::IO::CPKArchiveReader *)ptArchive, (Console::IO::FileInfo *)ptFileInfo);






























    BOOL bResult = sSNOFileOpenForRead(ptSNOElement, dwHash, (Console::IO::CPKArchiveReader *)ptArchive, (Console::IO::FileInfo *)ptFileInfo);


    if (ptArchive)
    {
        bResult = sLoadFileHeader(ptSNOElement, dwHash, (Console::IO::CPKArchiveReader *)ptArchive, (Console::IO::FileInfo *)ptFileInfo);
    }







    switch (ptLazyData->eSNOFileType)
    {




    case SNOFILETYPE_LOCAL:
        {
            uint32 dwOffset = 0;
            uint32 dwFileSize;
            BOOL fSuccess = FileGetSize(ptLazyData->tSNOFileRef.tRef, dwFileSize);


            uint32 dwSize = dwFileSize;
            if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
            {
                dwOffset = sizeof(SNOFileHeader);
                dwSize -= sizeof(SNOFileHeader);
            }



















            ptLazyData->pDataBuffer = SigmaMemoryNew(dwSize, 4, NULL, FALSE);


            tStats.diskFile = &ptLazyData->tSNOFileRef.tRef;
            tStats.buffer = ptLazyData->pDataBuffer;
            tStats.offset = dwOffset;
            tStats.amount = dwSize;
            tStats.m_ptFile = NULL;
        }
        break;






    case SNOFILETYPE_SNOPACK:
        {
            int64 nSize = ptLazyData->m_tFile.GetFileSize();
            if (nSize != -1)
            {
                uint32 dwOffset = 0;
                if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
                {
                    dwOffset = sizeof(SNOFileHeader);
                    nSize -= sizeof(SNOFileHeader);
                }



















                ptLazyData->pDataBuffer = SigmaMemoryNew(nSize, 4, NULL, FALSE);


                tStats.diskFile = NULL;
                tStats.buffer = ptLazyData->pDataBuffer;
                tStats.offset = dwOffset;
                tStats.amount = (uint32)nSize;
                tStats.m_ptFile = &ptLazyData->m_tFile;
            }
        }
        break;

    default:

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    SigmaMemorySetTraceAllocations(bPrevTrace);
    return SNOFILESTATRESULT_SUCCESS;
}





static BOOL sSNOFileOpenForRead(
    SNOElement * ptSNOElement,

    uint64 dwHash,
    Console::IO::CPKArchiveReader * ptArchive,
    Console::IO::FileInfo * ptFileInfo)


{
    if (ptSNOElement == NULL)
    {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    SNOLazyData * ptLazyData = SNOFileGetLazyData(ptSNOElement);


    Blizzard::File::SetLastErrorStack(NULL);






    bool bSuccess = false;
    switch (ptLazyData->eSNOFileType)
    {
    case SNOFILETYPE_SNOPACK:


        bSuccess = sSNOEntryOpenFile(ptSNOElement, &ptLazyData->m_tFile, dwHash, ptArchive, ptFileInfo);
        break;

    case SNOFILETYPE_LOCAL:





        bSuccess = FileOpen(ptLazyData->tSNOFileRef.tRef, 1);
        if (!bSuccess)
        {
            if (!FileExists(ptLazyData->tSNOFileRef.tRef))
            {
                LPCSTR szFilePath = FileReferenceGetPath(ptLazyData->tSNOFileRef.tRef);



                CHAR szWorkingDirectory[4096];
                Blizzard::File::GetWorkingDirectory(szWorkingDirectory, sizeof(szWorkingDirectory));
            }
        }
        break;

    default:








        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }


    if (!bSuccess)
    {




        sDisplaySNOFileError(ERROR_FILE_OPEN, ptLazyData->tSNOFileRef, "Could not open SNO file", NULL, 0);
        return FALSE;
    }
    return TRUE;




}



























































static void sSNOFileCancelAsynchronous(SNOElement * ptSNOElement)
{

    if (ptSNOElement->dwFlags & SNOENTRY_FLAG_ASYNCHRONOUS)
    {



        if (SNOFileGetAsyncRecord(ptSNOElement))
        {
            SNOFilesCloseAsyncRecord(ptSNOElement);
        }


        SNOLazyData * ptLazyData = SNOFileGetLazyData(ptSNOElement);
        if (ptLazyData)
        {




            SigmaMemoryFree(ptLazyData->pDataBuffer, NULL); ptLazyData->pDataBuffer = NULL;
        }


        Blizzard::Lock::Atomic::And(~(SNOENTRY_FLAG_ASYNCHRONOUS | SNOENTRY_FLAG_READING), &ptSNOElement->dwFlags);
    }
}



SNOElement * SNOFileLookup(const SNOName & snoname)
{
    return sGetSNOElement(snoname);
}























BOOL SNOFileTestSNOElement(const SNOName & snoname)
{
    return sGetSNOElement(snoname) != NULL;
}













uint32 SNOFileGetFileTableCount()
{
    uint32 dwTotalCount = 0;
    for (int32 i = 0; i < SNOTOC::TT_COUNT; i++)
    {
        dwTotalCount += sgptSNOGlobals->tTOCData[i].ptSNOElements->GetTotalCount();
    }
    return dwTotalCount;
}





























































































































BOOL SNOFileOpenForRead(
    SNOElement * ptSNOElement)
{
    SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);
    if (!ptLazyData)
    {
        return FALSE;
    }

    return sSNOFileOpenForRead(ptSNOElement, 0, NULL, NULL);
}




SNOElement * SNOFileOpenForRead(
    const SNOName & snoname)
{
    SNOElement * ptSNOElement = SNOFileLookup(snoname);

    if (ptSNOElement)
    {
        if (SNOFileOpenForRead(ptSNOElement))
        {
            return ptSNOElement;
        }
    }
    else




    {
        LPCSTR szGroupName = SNOGroupGetName(snoname.snoGroup);
    }


    return NULL;
}




static void sSNOFileClose(
    SNOElement * ptSNOElement)
{
    SNOLazyData * ptLazyData = SNOFileGetLazyData(ptSNOElement);


    switch (ptLazyData->eSNOFileType)
    {
    case SNOFILETYPE_LOCAL:


        if (FileReferenceGetRefCount(ptLazyData->tSNOFileRef.tRef))
            FileClose(ptLazyData->tSNOFileRef.tRef);
        break;

    case SNOFILETYPE_SNOPACK:


        ptLazyData->m_tFile.Close();
        break;
    }
}









void SNOFileClose(
    SNOElement * ptSNOElement)
{
    sSNOFileCancelAsynchronous(ptSNOElement);

    sSNOFileClose(ptSNOElement);


    sSNOFileAssignLazyData(ptSNOElement, NULL);
}
























BOOL SNOFileDelete(
    const SNOName & snoname,
    const BOOL fKeepInFileMap)
{









    SNOElement * ptSNOElement = SNOFileLookup(snoname);
    if (!ptSNOElement)
    {
        return FALSE;
    }

    SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);
    if (ptLazyData)
    {







        if (ptLazyData->eSNOFileType == SNOFILETYPE_LOCAL)
        {


            FileDelete(ptLazyData->tSNOFileRef.tRef);

            SNODeleteAdditionalFiles(ptLazyData->tSNOFileRef.tRef.szPath);
        }

        SNOFileClose(ptSNOElement);
    }

    if (!fKeepInFileMap)
    {
        sSNOFileMapDelete(ptSNOElement, TRUE);
    }
    else
    {

        sSNOFileAssignLazyData(ptSNOElement, NULL);
    }

    return TRUE;
}





















void SNOFileRead(
    SNOElement * ptSNOElement,
    const uint32 dwOffset,
    const uint32 dwSize,
    void * lpBuffer)
{
    if (dwSize == 0)
    {
        return;
    }






    SNOLazyData * ptLazyData;
    if (ptSNOElement->dwFlags & SNOENTRY_FLAG_ASYNCHRONOUS)
    {
        ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);



        if (ptSNOElement->dwFlags & SNOENTRY_FLAG_ASYNCHRONOUS)
        {
            if (SNOFileGetAsyncRecord(ptSNOElement))
            {
                SNOFilesCloseAsyncRecord(ptSNOElement);

                Blizzard::Lock::Atomic::And(~SNOENTRY_FLAG_READING, &ptSNOElement->dwFlags);
            }


            if (ptLazyData->pDataBuffer)
            {
                SigmaMemoryCopy(lpBuffer, (uint8 *)ptLazyData->pDataBuffer + dwOffset, dwSize);
                return;
            }
        }
    }
    else
    {
        ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);
    }




    uint32 dwAdditionalOffset = (ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE) ? 0 : sizeof(SNOFileHeader);

    switch (ptLazyData->eSNOFileType)
    {
    case SNOFILETYPE_LOCAL:







        if (!FileRead(ptLazyData->tSNOFileRef.tRef, lpBuffer, dwAdditionalOffset + dwOffset, dwSize, ERROR_FILE_READ))
        {
            sDisplaySNOFileReadError(NULL, 0, ptLazyData, "", dwAdditionalOffset + dwOffset, dwSize, 0, TRUE);
        }
        break;

    case SNOFILETYPE_SNOPACK:








        {
            uint64 dwActual = 0;


            ptLazyData->m_tFile.SetFileCursor(dwAdditionalOffset + dwOffset, FileIO::FROM_BEGINNING);
            if (!ptLazyData->m_tFile.ReadFile(lpBuffer, dwSize, &dwActual) || dwActual < dwSize)
            {
                sDisplaySNOFileReadError(NULL, 0, ptLazyData, "", ptLazyData->m_tFile.GetSNOFileStartingLocation(), dwSize, dwActual, TRUE);
            }
        }
        break;

    default:

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




void SNOFileWrite(FileReference & tRef, const void * lpBuffer, const uint32 dwOffset, const uint32 dwSize)
{




    FileWrite(tRef, lpBuffer, dwOffset + sizeof(SNOFileHeader), dwSize, ERROR_FILE_WRITE);
}







BOOL SNOFileExistsOnDisk(const SNOName & snoname, const BOOL fCompileIfNeccessary)
{
    SNOElement * ptSNOElement;


    ptSNOElement = SNOFileLookup(snoname);















    return ptSNOElement != NULL;
}














SNO SNOFileLookupGUID(
    const SNOGroup snoGroup,
    LPCSTR szPath)
{
    if (snoGroup == SNOGROUP_NONE || snoGroup == 0 || !sgptSNOGlobals)
    {


        return SNO_NONE;
    }



    BOOL bResult = FALSE;
    SNO snoLookup = SNO_NONE;


    if (szPath[0] == '\0')
    {
        return SNO_NONE;
    }










    if (!sSNOGroupNeedsName(snoGroup))
    {
        uint64 dwHash = sSNOFileToHash(snoGroup, szPath, FALSE);

        uint8 pHash[8];
        sSet8BytesStringFromHash(dwHash, pHash);
        SNOFilenameHash tName(pHash);
        MapStringToSNOHash & tStringMap = sgptSNOGlobals->tStringToSNOMapHash[snoGroup];
        bResult = tStringMap.Lookup(tName, snoLookup);
    }
    else
    {



        SNOFilename tName(szPath);
        MapStringToSNO & tStringMap = sgptSNOGlobals->tStringToSNOMap[snoGroup];
        bResult = tStringMap.Lookup(tName, snoLookup);
    }

    if (bResult)
    {
        return snoLookup;
    }

















    return (strcmp(szPath, cgszInvalidSnoName) == 0) ? INVALID_SNO : SNO_NONE;
}



uint32 sScoreStringMatch(LPCSTR szPattern, LPCSTR szString)
{
    if (szPattern == NULL || szString == NULL || szPattern[0] == '\0' || szString[0] == '\0')
    {
        return 0xff;
    }
    uint32 nPatternLen = strlen(szPattern);
    uint32 nStringLen = strlen(szString);
    uint32 nMatchingChars = 0;
    while (tolower(*szPattern) == tolower(*szString))
    {
        nMatchingChars++;
        szPattern++;
        szString++;
        if (*szPattern == '\0')
        {
            break;
        }
        if (*szString == '\0')
        {
            return 0xff;
        }
    }
    if (nMatchingChars < nPatternLen)
    {
        return 0xff;
    }
    return nStringLen - nMatchingChars;
}














SNO SNOFileFindBestMatch(const SNOGroup snoGroup, LPCSTR szPath)
{


    if (snoGroup == SNOGROUP_NONE || snoGroup == 0 || !sgptSNOGlobals)
    {


        return SNO_NONE;
    }



    if (!sSNOGroupNeedsName(snoGroup))
    {
        return SNOFileLookupGUID(snoGroup, szPath);
    }

    if (szPath[0] == '\0')
    {
        return SNO_NONE;
    }

    uint32 nBestScore = 255;
    SNO bestMatch = SNO_NONE;
    MapStringToSNO & tStringMap = sgptSNOGlobals->tStringToSNOMap[snoGroup];

    POS pos = tStringMap.GetStartAssoc();


    while (pos)
    {
        SNO value;
        uint32 nScore;
        SNOFilename key;
        tStringMap.GetNextAssoc(pos, key, value);

        nScore = sScoreStringMatch(szPath, key.szName);
        if (nScore < nBestScore)
        {
            bestMatch = value;
            nBestScore = nScore;
            if (nScore == 0)
            {
                return bestMatch;
            }
        }
    }






    if (bestMatch != SNO_NONE)
    {
        return bestMatch;
    }




    return strcmp(szPath, cgszInvalidSnoName) == 0 ? INVALID_SNO : SNO_NONE;

}














































































































BOOL SNOFileExists(const SNOName & snoname)
{

    if (snoname.snoGroup == SNOGROUP_NONE || snoname.snoHandle == SNO_NONE)
        return FALSE;




    SNOElement * ptSNOElement = SNOFileLookup(snoname);





    return ptSNOElement != NULL;
}






































BOOL SNOFileExists(const SNOGroup snoGroup, const SNO sno)
{

    SNOName snoname(snoGroup, sno);

    return SNOFileExists(snoname);
}























































BOOL SNOFileTryGetVersion(
    const SNOName & snoname,
    uint32 * dwVersion,
    uint32 * dwAssetVersion,
    uint32 * dwXMLHash)
{
    BOOL bGotVersion = FALSE;
    SNOElement * ptSNOElement = SNOFileLookup(snoname);

    *dwVersion = 0;

    if (dwAssetVersion)
        *dwAssetVersion = 0;

    if (dwXMLHash)
        *dwXMLHash = 0;

    if (ptSNOElement)
    {
        SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);

        if (ptLazyData)
        {
            *dwVersion = ptLazyData->tSNOFileHeader.dwVersion;

            if (dwAssetVersion)
                *dwAssetVersion = ptLazyData->tSNOFileHeader.dwAssetVersion;

            if (dwXMLHash)
                *dwXMLHash = ptLazyData->tSNOFileHeader.dwXMLHash;

            bGotVersion = TRUE;
        }
    }
    return bGotVersion;
}



















































































































































static BOOL sSNOFileCheckCriteria(
    const SNOFileInfo & tCriteria,
    SNOElement * ptSNOElement,
    const BOOL fTraceCriteriaFailures)
{
    SNOName snonameSrc = ptSNOElement->tEntry.ToSNOName();






















    if (tCriteria.eSNOFileType == SNOFILETYPE_INVALID &&



        tCriteria.eXLocale == XLOCALE_INVALID &&

        tCriteria.dwVersion == ASSET_VERSION_NONE)
    {
        return TRUE;
    }

    SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);
    if (!ptLazyData)
    {
        return FALSE;
    }





    if (tCriteria.eSNOFileType != SNOFILETYPE_INVALID &&
        ptLazyData->eSNOFileType != tCriteria.eSNOFileType)
    {
        return FALSE;
    }




















    if (tCriteria.dwVersion != ASSET_VERSION_NONE)
    {
        if (ptLazyData->tSNOFileHeader.dwVersion != tCriteria.dwVersion)
        {
            return FALSE;
        }
    }
    return TRUE;
}




























































































// NON_MATCHING: 14/380; same code, but the three iterator fields (pos, fInDynElements, ptElement) are zeroed/merged in the opposite order at each loop copy (header phi order)
int32 SNOFileEnumerate(
    SNOFileInfo & tCriteria,
    PooledSNONameList * ptList,
    const BOOL fIgnoreExtraCriteria)
{
    int32 nResultCount = 0;

    if (tCriteria.snoname.snoGroup != SNOGROUP_NONE &&
        tCriteria.snoname.snoHandle != SNO_NONE)
    {
        if (tCriteria.snoname.snoGroup < 0 || tCriteria.snoname.snoGroup >= MAX_SNO_GROUPS)
        {
            return 0;
        }

        SNOElement * ptSNOElement = SNOFileLookup(tCriteria.snoname);
        if (ptSNOElement)
        {



            if (fIgnoreExtraCriteria || sSNOFileCheckCriteria(tCriteria, ptSNOElement, FALSE))
            {
                if (ptList)
                {
                    SNOName snoname(ptSNOElement->tEntry.snoGroup, ptSNOElement->tEntry.uGuid);
                    ptList->AddTail_Tracked(snoname);
                }
                nResultCount++;
            }
        }
    }
    else
    {



        for (int32 i = 0; i < SNOTOC::TT_COUNT; i++)
        {
            SNOElementList * ptSNOElements = sgptSNOGlobals->tTOCData[i].ptSNOElements;
            SNOElementIterator tIt;
            while (ptSNOElements->Iterate(tIt))
            {
                SNOElement * ptSNOElement = tIt.ptElement;

                if (ptSNOElement->tEntry.snoGroup != tCriteria.snoname.snoGroup && tCriteria.snoname.snoGroup != SNOGROUP_NONE) continue;
                if (ptSNOElement->tEntry.uGuid != tCriteria.snoname.snoHandle && tCriteria.snoname.snoHandle != SNO_NONE) continue;




                if (fIgnoreExtraCriteria || sSNOFileCheckCriteria(tCriteria, ptSNOElement, FALSE))
                {
                    if (ptList)
                    {
                        SNOName snoname(ptSNOElement->tEntry.snoGroup, ptSNOElement->tEntry.uGuid);
                        ptList->AddTail_Tracked(snoname);
                    }
                    nResultCount++;
                }
            }
        }
    }
    return nResultCount;
}




int32 SNOFileEnumerateSNOsForGroup(const SNOGroup snoGroup, PooledSNONameList * ptList)
{
    SNOFileInfo tCriteria;
    tCriteria.snoname.Set(snoGroup, SNO_NONE);
    return SNOFileEnumerate(tCriteria, ptList, TRUE);
}







void SNOFileGetFolderForNewLocalCacheFile(FileReference & tRef, const XLocale eXLocale, const SNOGroup snoGroup)
{





    const SNOGroupInfo * ptSNOGroupInfo = SNOGroupGetInfo(snoGroup);
    switch (ptSNOGroupInfo->eSNODataType)
    {
    case SNODATATYPE_USERDATA:
        {
            CRefString sPath;
            FileReferenceInit(tRef, sPath.Format("%s\\%s", SNOFilesGetUserDataPath(sgptSNOGlobals->szBranch).str(), ptSNOGroupInfo->szName));
        }
        break;
    case SNODATATYPE_TITLE:
        {
            CRefString sPath;
            FileReferenceInit(tRef, sPath.Format("%s\\%s", SNOFilesGetLocalCachePath(eXLocale).str(), ptSNOGroupInfo->szName));
        }
        break;
    default:






        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}


void SNOFixFilename(LPSTR szName);

void SNOFileGetFilenameForLocalCacheFile(
    FileReference & tRef,
    const XLocale eXLocale,
    const SNOName & snoname,
    BOOL fConsoleUsesHandle)
{

    const SNOGroupInfo * ptSNOGroupInfo = SNOGroupGetInfo(snoname.snoGroup);



    BOOL bIsConsole = TRUE;




    CHAR szFixedName[256];
    if (bIsConsole && fConsoleUsesHandle)
    {

        sprintf(szFixedName, "%d", snoname.snoHandle);
    }
    else
    {

        Blizzard::String::Copy(szFixedName, SNOFileGetName(snoname).str(), sizeof(szFixedName));
        SNOFixFilename(szFixedName);
    }

    switch (ptSNOGroupInfo->eSNODataType)
    {
    case SNODATATYPE_USERDATA:
        {
            FileReferenceInit(tRef, CRefString().Format(
                "%s\\%s\\%s%s",
                SNOFilesGetUserDataPath(sgptSNOGlobals->szBranch).str(),
                ptSNOGroupInfo->szName,
                szFixedName,
                SNOGetGroupExtension(snoname.snoGroup)));
        }
        break;


    case SNODATATYPE_TITLE:
        {
            FileReferenceInit(tRef, CRefString().Format(
                "%s\\%s\\%s%s",
                SNOFilesGetLocalCachePath(eXLocale).str(),
                ptSNOGroupInfo->szName,
                szFixedName,
                SNOGetGroupExtension(snoname.snoGroup)));
        }
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}















// NON_MATCHING: 440 vs 424; same blocks, calls and line table; the final GetSubFileHash (6257) is tail-called here (IR marks it `tail`, CodeGenPrepare duplicates the ret), the original calls it and shares `mov x19,x0` with the GetFileHash path before one epilogue. Not caused by else/early-return form or child-test polarity (tried).
uint64 SNOFileGetFileHash(
    SNOElement * ptSNOElement,
    BOOL fWithExtension)
{
    if (sSNOGroupNeedsName(ptSNOElement->tEntry.snoGroup))
    {
        if (ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE)
        {
            return Console::IO::CPKArchive::GetSubFileHash(sSNOFileToHash(ptSNOElement->tEntry.snoGroup, ptSNOElement->tEntry.GetName(), FALSE), CRefString().Format("\\%s", ((SNOElementChild *)ptSNOElement)->szChildFile));
        }
        else
        {
            return sSNOFileToHash(ptSNOElement->tEntry.snoGroup, ptSNOElement->tEntry.GetName(), fWithExtension);
        }
    }
    else
    {
        if (ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE)
        {
            return Console::IO::CPKArchive::GetSubFileHash(sSNOEntryGetHash(ptSNOElement->tEntry), CRefString().Format("\\%s", ((SNOElementChild *)ptSNOElement)->szChildFile));
        }
        else
        {
            if (fWithExtension)
            {
                return Console::IO::CPKArchive::GetSubFileHash(sSNOEntryGetHash(ptSNOElement->tEntry), SNOGetGroupExtension(ptSNOElement->tEntry.snoGroup));
            }
            else
            {
                return sSNOEntryGetHash(ptSNOElement->tEntry);
            }
        }
    }
}





void SNOFileGetFilePath(const SNOName & snoname, char * szFilePath, int32 nFilePathMaxSize)
{



    SNOElement * ptSNOElement = sGetSNOElement(snoname);
    if (ptSNOElement == NULL)
    {
        szFilePath[0] = '\0';
        return;
    }

    SNOFileGetFilePath(ptSNOElement, szFilePath, nFilePathMaxSize);
}




inline void SNOFileGetFilePath(const SNOElement * ptSNOElement, char * szFilePath, int32 nFilePathMaxSize)
{










    Blizzard::String::Format(szFilePath, nFilePathMaxSize, "%s\\%s%s",
        SNOGroupGetName(ptSNOElement->tEntry.snoGroup),
        sSNOEntryGetName(ptSNOElement->tEntry).str(),
        SNOGetGroupExtension(ptSNOElement->tEntry.snoGroup));
}


































































































































































































































































































































































































































































































































































































































































































































































































































































































































LPCSTR SNOPackTypeToString(SNOPackType eSNOPackDestination)
{
    return scszSNOPackNames[eSNOPackDestination];
}































































CRefString SNOToString(const SNOGroup snoGroup, const SNO snoHandle, const BOOL fUsedForXMLFields)
{
    SNOElement * ptSNOElement = NULL;




    if (snoHandle == SNO_NONE)
    {
        // An empty string is written to the XML for a missing SNO.
        return CRefString(fUsedForXMLFields ? "" : cgszSnoNoneName);
    }

    SNOName snoName(snoGroup, snoHandle);

    if (snoGroup != SNOGROUP_NONE && snoGroup != SNOGROUP_CODE)
    {
        ptSNOElement = SNOFileLookup(snoName);
    }
    else
    {
        // Find the group the handle belongs to.
        for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
        {
            snoName.snoGroup = i;
            ptSNOElement = SNOFileLookup(snoName);
            if (ptSNOElement != NULL)
                break;
        }
    }



    if (ptSNOElement == NULL)
    {
        return CRefString(fUsedForXMLFields ? "" : cgszInvalidSnoName);
    }

    return SNOFileGetName(snoName);
}




CRefString SNOToString(const SNOName & snoname, const BOOL fUsedForXMLFields)
{
    return SNOToString(snoname.snoGroup, snoname.snoHandle, fUsedForXMLFields);
}






CRefString SNONameToString(const SNOGroup snoGroup, const SNO snoHandle, const BOOL fUsedForXMLFields)
{
    return SNONameToString(SNOName(snoGroup, snoHandle), fUsedForXMLFields);
}







inline CRefString SNONameToString(const SNOName & snoname, const BOOL fUsedForXMLFields)
{


    CRefString sGroupName = SNOGroupToString(snoname.snoGroup, fUsedForXMLFields);
    CRefString sName = SNOToString(snoname, fUsedForXMLFields);
    CRefString sResult;

    sResult.Format("%s:%s", sGroupName.str(), sName.str());
    return sResult;
}




// The name of this macro is invented; each test of the original sits on its own line.
#define STRING_TO_SNOGROUP_TEST(snoGroup) if (strcasecmp(szStr, #snoGroup) == 0) return snoGroup;
SNOGroup StringToSNOGroup(LPCSTR szStr)

{
    STRING_TO_SNOGROUP_TEST(ACCOUNT_SNO)
    STRING_TO_SNOGROUP_TEST(ACTOR_SNO)
    STRING_TO_SNOGROUP_TEST(ADVENTURE_SNO)
    STRING_TO_SNOGROUP_TEST(AIBEHAVIOR_SNO)
    STRING_TO_SNOGROUP_TEST(AISTATE_SNO)
    STRING_TO_SNOGROUP_TEST(AMBIENTSOUND_SNO)
    STRING_TO_SNOGROUP_TEST(ANIMATION_SNO)
    STRING_TO_SNOGROUP_TEST(ANIMATION2D_SNO)
    STRING_TO_SNOGROUP_TEST(ANIMSET_SNO)
    STRING_TO_SNOGROUP_TEST(APPEARANCE_SNO)
    STRING_TO_SNOGROUP_TEST(ACT_SNO)
    STRING_TO_SNOGROUP_TEST(BOSS_ENCOUNTER_SNO)
    STRING_TO_SNOGROUP_TEST(CLOTH_SNO)
    STRING_TO_SNOGROUP_TEST(CONDITION_SNO)
    STRING_TO_SNOGROUP_TEST(CONDUCTOR_SNO)
    STRING_TO_SNOGROUP_TEST(CONVERSATION_SNO)
    STRING_TO_SNOGROUP_TEST(CONVERSATIONLIST_SNO)
    STRING_TO_SNOGROUP_TEST(EFFECTGROUP_SNO)
    STRING_TO_SNOGROUP_TEST(ENCOUNTER_SNO)
    STRING_TO_SNOGROUP_TEST(EXPLOSION_SNO)
    STRING_TO_SNOGROUP_TEST(FLAGSET_SNO)
    STRING_TO_SNOGROUP_TEST(FONT_SNO)
    STRING_TO_SNOGROUP_TEST(GAMEBALANCE_SNO)
    STRING_TO_SNOGROUP_TEST(GLOBAL_SNO)
    STRING_TO_SNOGROUP_TEST(HERO_SNO)
    STRING_TO_SNOGROUP_TEST(LEVELAREA_SNO)
    STRING_TO_SNOGROUP_TEST(LIGHT_SNO)
    STRING_TO_SNOGROUP_TEST(LORE_SNO)
    STRING_TO_SNOGROUP_TEST(MARKERSET_SNO)
    STRING_TO_SNOGROUP_TEST(MATERIAL_SNO)
    STRING_TO_SNOGROUP_TEST(MONSTER_SNO)
    STRING_TO_SNOGROUP_TEST(MUSIC_SNO)
    STRING_TO_SNOGROUP_TEST(OBSERVER_SNO)
    STRING_TO_SNOGROUP_TEST(PARTICLE_SNO)
    STRING_TO_SNOGROUP_TEST(PHYSICS_SNO)
    STRING_TO_SNOGROUP_TEST(PHYSMESH_SNO)
    STRING_TO_SNOGROUP_TEST(POWER_SNO)
    STRING_TO_SNOGROUP_TEST(QUEST_SNO)
    STRING_TO_SNOGROUP_TEST(QUESTRANGE_SNO)
    STRING_TO_SNOGROUP_TEST(RECIPE_SNO)
    STRING_TO_SNOGROUP_TEST(REVERB_SNO)
    STRING_TO_SNOGROUP_TEST(ROPE_SNO)
    STRING_TO_SNOGROUP_TEST(SCENE_SNO)
    STRING_TO_SNOGROUP_TEST(SCENEGROUP_SNO)
    STRING_TO_SNOGROUP_TEST(SCRIPT_SNO)
    STRING_TO_SNOGROUP_TEST(SHADERMAP_SNO)
    STRING_TO_SNOGROUP_TEST(SHADER_SNO)
    STRING_TO_SNOGROUP_TEST(SHAKE_SNO)
    STRING_TO_SNOGROUP_TEST(SKILLKIT_SNO)
    STRING_TO_SNOGROUP_TEST(SOUND_SNO)
    STRING_TO_SNOGROUP_TEST(SOUNDBANK_SNO)
    STRING_TO_SNOGROUP_TEST(STRINGLIST_SNO)
    STRING_TO_SNOGROUP_TEST(SURFACE_SNO)
    STRING_TO_SNOGROUP_TEST(TEXTURE_SNO)
    STRING_TO_SNOGROUP_TEST(TIMEDEVENT_SNO)
    STRING_TO_SNOGROUP_TEST(TRAIL_SNO)
    STRING_TO_SNOGROUP_TEST(TREASURECLASS_SNO)
    STRING_TO_SNOGROUP_TEST(TUTORIAL_SNO)
    STRING_TO_SNOGROUP_TEST(UI_SNO)
    STRING_TO_SNOGROUP_TEST(WEATHER_SNO)
    STRING_TO_SNOGROUP_TEST(WORLD_SNO)
    STRING_TO_SNOGROUP_TEST(CONTROL_SCHEME_SNO)
    STRING_TO_SNOGROUP_TEST(ACCOLADE_SNO)
    STRING_TO_SNOGROUP_TEST(ANIMTREE_SNO)
    STRING_TO_SNOGROUP_TEST(VIBRATION_SNO)
    STRING_TO_SNOGROUP_TEST(DUNGEONFINDER_SNO)


    return SNOGROUP_NONE;
}





// The name of this macro is invented; each case of the original sits on its own line.
#define SNOGROUP_TO_STRING_CASE(snoGroup) case snoGroup: return CRefString(#snoGroup);
CRefString SNOGroupToString(const SNOGroup snoGroup, const BOOL fUsedForXMLFields)
{


    switch (snoGroup)
    {
    SNOGROUP_TO_STRING_CASE(SNOGROUP_CODE)

    SNOGROUP_TO_STRING_CASE(ACCOUNT_SNO)
    SNOGROUP_TO_STRING_CASE(ACTOR_SNO)
    SNOGROUP_TO_STRING_CASE(ADVENTURE_SNO)
    SNOGROUP_TO_STRING_CASE(AIBEHAVIOR_SNO)
    SNOGROUP_TO_STRING_CASE(AISTATE_SNO)
    SNOGROUP_TO_STRING_CASE(AMBIENTSOUND_SNO)
    SNOGROUP_TO_STRING_CASE(ANIMATION_SNO)
    SNOGROUP_TO_STRING_CASE(ANIMATION2D_SNO)
    SNOGROUP_TO_STRING_CASE(ANIMSET_SNO)
    SNOGROUP_TO_STRING_CASE(APPEARANCE_SNO)
    SNOGROUP_TO_STRING_CASE(ACT_SNO)
    SNOGROUP_TO_STRING_CASE(BOSS_ENCOUNTER_SNO)
    SNOGROUP_TO_STRING_CASE(CLOTH_SNO)
    SNOGROUP_TO_STRING_CASE(CONDITION_SNO)
    SNOGROUP_TO_STRING_CASE(CONDUCTOR_SNO)
    SNOGROUP_TO_STRING_CASE(CONVERSATION_SNO)
    SNOGROUP_TO_STRING_CASE(CONVERSATIONLIST_SNO)
    SNOGROUP_TO_STRING_CASE(EFFECTGROUP_SNO)
    SNOGROUP_TO_STRING_CASE(ENCOUNTER_SNO)
    SNOGROUP_TO_STRING_CASE(EXPLOSION_SNO)
    SNOGROUP_TO_STRING_CASE(FLAGSET_SNO)
    SNOGROUP_TO_STRING_CASE(FONT_SNO)
    SNOGROUP_TO_STRING_CASE(GAMEBALANCE_SNO)
    SNOGROUP_TO_STRING_CASE(GLOBAL_SNO)
    SNOGROUP_TO_STRING_CASE(HERO_SNO)
    SNOGROUP_TO_STRING_CASE(LEVELAREA_SNO)
    SNOGROUP_TO_STRING_CASE(LIGHT_SNO)
    SNOGROUP_TO_STRING_CASE(LORE_SNO)
    SNOGROUP_TO_STRING_CASE(MARKERSET_SNO)
    SNOGROUP_TO_STRING_CASE(MATERIAL_SNO)
    SNOGROUP_TO_STRING_CASE(MONSTER_SNO)
    SNOGROUP_TO_STRING_CASE(MUSIC_SNO)
    SNOGROUP_TO_STRING_CASE(OBSERVER_SNO)
    SNOGROUP_TO_STRING_CASE(PARTICLE_SNO)
    SNOGROUP_TO_STRING_CASE(PHYSICS_SNO)
    SNOGROUP_TO_STRING_CASE(PHYSMESH_SNO)
    SNOGROUP_TO_STRING_CASE(POWER_SNO)
    SNOGROUP_TO_STRING_CASE(QUEST_SNO)
    SNOGROUP_TO_STRING_CASE(QUESTRANGE_SNO)
    SNOGROUP_TO_STRING_CASE(RECIPE_SNO)
    SNOGROUP_TO_STRING_CASE(REVERB_SNO)
    SNOGROUP_TO_STRING_CASE(ROPE_SNO)
    SNOGROUP_TO_STRING_CASE(SCENE_SNO)
    SNOGROUP_TO_STRING_CASE(SCENEGROUP_SNO)
    SNOGROUP_TO_STRING_CASE(SCRIPT_SNO)
    SNOGROUP_TO_STRING_CASE(SHADERMAP_SNO)
    SNOGROUP_TO_STRING_CASE(SHADER_SNO)
    SNOGROUP_TO_STRING_CASE(SHAKE_SNO)
    SNOGROUP_TO_STRING_CASE(SKILLKIT_SNO)
    SNOGROUP_TO_STRING_CASE(SOUND_SNO)
    SNOGROUP_TO_STRING_CASE(SOUNDBANK_SNO)
    SNOGROUP_TO_STRING_CASE(STRINGLIST_SNO)
    SNOGROUP_TO_STRING_CASE(SURFACE_SNO)
    SNOGROUP_TO_STRING_CASE(TEXTURE_SNO)
    SNOGROUP_TO_STRING_CASE(TIMEDEVENT_SNO)
    SNOGROUP_TO_STRING_CASE(TRAIL_SNO)
    SNOGROUP_TO_STRING_CASE(TREASURECLASS_SNO)
    SNOGROUP_TO_STRING_CASE(TUTORIAL_SNO)
    SNOGROUP_TO_STRING_CASE(UI_SNO)
    SNOGROUP_TO_STRING_CASE(WEATHER_SNO)
    SNOGROUP_TO_STRING_CASE(WORLD_SNO)
    SNOGROUP_TO_STRING_CASE(CONTROL_SCHEME_SNO)
    SNOGROUP_TO_STRING_CASE(ACCOLADE_SNO)
    SNOGROUP_TO_STRING_CASE(ANIMTREE_SNO)
    SNOGROUP_TO_STRING_CASE(VIBRATION_SNO)
    SNOGROUP_TO_STRING_CASE(DUNGEONFINDER_SNO)
    }

    return CRefString("SNOGROUP_NONE");
}




void * SNOFileLoadSerializedTracked(SNOElement * ptSNOElement, const SerializeData & ser, uint32 dwAlign, LPCSTR szFile, int32 nLine, LPCSTR szDebugString, XMemoryAllocatorInterface * ptMemoryInterface)
{
    void * ptDest = NULL;
    if (ser.dwSize != 0 && ser.dwSize != (uint32)-1)
    {
        ptDest = SigmaMemoryNew(ser.dwSize, dwAlign, ptMemoryInterface, FALSE);
















        SNOFileRead(ptSNOElement, ser, ptDest);
    }
    return ptDest;
}





inline CRefString SNOFilesGetTitleDataPath()
{
    return CRefString(sgptSNOGlobals->szDataPath);
}






inline CRefString SNOFilesGetLocaleSubPath(const XLocale eXLocale)
{
    if (eXLocale == XLOCALE_GLOBAL)
    {
        return CRefString(XLocaleToString(eXLocale));
    }
    CRefString sPath;
    return CRefString(sPath.Format("Locale\\%s", XLocaleToString(eXLocale)));
}







CRefString SNOFilesGetLocalePath(const XLocale eXLocale)
{
    CRefString sPath;
    return CRefString(sPath.Format("Data_D3\\%s", SNOFilesGetLocaleSubPath(eXLocale).str()));
}









CRefString SNOFilesGetUserDataPath(LPCSTR szBranchName)
{
    CHAR szInitialDirectory[256];
    CRefString sFolderName(cgszUserDataPathName);



    LPCSTR szInstanceName = CmdLineGetParams()->szInstanceName;
    if (strlen(szInstanceName) != 0)
    {
        CRefString sTemp;
        sFolderName += sTemp.Format(" (%s)", szInstanceName);
    }


























































    Blizzard::File::GetWorkingDirectory(szInitialDirectory, 255);
    return CRefString(szInitialDirectory);
}





















































BOOL SNOFileLoadTextFile(LPCSTR szPath, CHAR *& lpBuffer, uint32 & dwTextLength, CHAR *& lpTextStart)
{
    const uint8 szUTF8[3] = { 0xEF, 0xBB, 0xBF };
    const uint8 szUTF16LE[2] = { 0xFF, 0xFE };
    const uint8 szUTF16BE[2] = { 0xFE, 0xFF };
    const uint8 szUTF32LE[4] = { 0xFF, 0xFE, 0x00, 0x00 };
    const uint8 szUTF32BE[4] = { 0x00, 0x00, 0xFE, 0xFF };

    lpBuffer = NULL;
    lpTextStart = NULL;

    FileIO::File tFile(XLOCALE_GLOBAL);
    if (!tFile.Open(szPath))
    {
        return FALSE;
    }

    dwTextLength = (uint32)tFile.GetFileSize();
    lpBuffer = (CHAR *)SigmaMemoryNew(dwTextLength + 1, 4, NULL, FALSE);

    if (!tFile.ReadFile(lpBuffer, dwTextLength, NULL))
    {
        SigmaMemoryFree(lpBuffer, NULL); lpBuffer = NULL;
        return FALSE;
    }

    lpBuffer[dwTextLength] = '\0';







    lpTextStart = lpBuffer;
    if (dwTextLength >= 3 && memcmp(lpTextStart, szUTF8, 3) == 0)
    {
        lpTextStart += 3;
        dwTextLength -= 3;
    }
    else if (dwTextLength >= 4 && (memcmp(lpTextStart, szUTF32BE, 4) == 0 ||




                                   memcmp(lpTextStart, szUTF32LE, 4) == 0)) { lpTextStart += 4; dwTextLength -= 4; }




    else if (dwTextLength >= 2 && (memcmp(lpTextStart, szUTF16BE, 2) == 0 ||




                                   memcmp(lpTextStart, szUTF16LE, 2) == 0)) { lpTextStart += 2; dwTextLength -= 2; }





    return TRUE;
}











































































































































































static void sReportMpqOpenError(const CRefString & sPath)
{





    ReportLocalizedError_MpqOpenError(UnicodeText::GetCurrentLocale());

    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);

}










static void sOpenSNOPacks()
{
    static const SNOPackTypeAndPriority atSNOPacks[] = {
        { SNOPACKTYPE_CONSOLE_COMMON, CONSOLE_COMMON_PRIORITY, TRUE },
        { SNOPACKTYPE_CONSOLE_CORE_COMMON, CONSOLE_CORE_COMMON_PRIORITY, TRUE },
        { SNOPACKTYPE_CONSOLE_CLIENT_COMMON, CONSOLE_INACTIVE_ACT_PRIORITY, TRUE },
        { SNOPACKTYPE_CONSOLE_SERVER_COMMON, CONSOLE_INACTIVE_ACT_PRIORITY, TRUE },
    };
    sgptSNOGlobals->dwSNOCoreDataHash = 0;




























    XLocale nLocale = UnicodeText::GetCurrentLocale();
    CRefString szMountPath;
    Console::DLCManager::GetLocaleAOCMountPath(nLocale, szMountPath);
    CRefString sLocaleSubPath = CRefString().Format("%s\\%s", szMountPath.str(), XLocaleToString(nLocale));

    for (int32 i = 0; i < (int32)(sizeof(atSNOPacks) / sizeof(atSNOPacks[0])); i++)
    {
        int32 nPriority = atSNOPacks[i].nPriority;
        SNOPackType eSNOPackType = atSNOPacks[i].eType;




        CRefString sPath = CRefString().Format("%s\\%s.%s", SNOFilesGetMPQPath().str(), scszSNOPackNames[eSNOPackType], SNOFilesGetMPQExtension());

        HSARCHIVE hArchive = sOpenSNOPack(sPath, nPriority);
        HSARCHIVE hLocaleArchive = 0;


        if (hArchive == 0)
        {


            sReportMpqOpenError(sPath);
        }
















        if (atSNOPacks[i].fLoadLocale)
        {
            CRefString sLocalPack = CRefString().Format("%s_%s.%s", sLocaleSubPath.str(), scszSNOPackNames[eSNOPackType], SNOFilesGetMPQExtension());
            hLocaleArchive = sOpenSNOPack(sLocalPack, nPriority + CONSOLE_LOCALE_DIFF);
        }
        sptGlobalArchives[eSNOPackType] = (Console::IO::CPKArchiveReader *)hArchive;
        sptLocaleArchives[eSNOPackType] = (Console::IO::CPKArchiveReader *)hLocaleArchive;





    }


























    sOpenSNOPack(CRefString().Format("%s_%sCutscene.%s", sLocaleSubPath.str(), "Common", SNOFilesGetMPQExtension()), CONSOLE_CUTSCENE);



}




static SNOTOC::TOCType sGetTOCTypeFromSNOElement(
    SNOElement * ptSNOElement)
{


    const char * szName = ptSNOElement->tEntry.GetName();
    SNOGroup snoGroup = ptSNOElement->tEntry.snoGroup;
    for (int32 i = 0; i < SNOTOC::TT_COUNT; i++)
    {
        SNOTOCData & tTOCData = sgptSNOGlobals->tTOCData[i];
        if ((szName >= tTOCData.szSNOEntryNameBuffers[snoGroup] &&
             szName < tTOCData.szSNOEntryNameBuffers[snoGroup] + tTOCData.dwSNOEntryNameBufferSizes[snoGroup]) ||




            tTOCData.tOverflowNames.Contains(szName))
        {
            return (SNOTOC::TOCType)i;
        }
    }
    return SNOTOC::TT_COUNT;
}

























































































void sChangeActSNOPacksPriority(SNOPackType ePackType, int32 nArchivePriority)


{
    if (ePackType != SNOPACKTYPE_INVALID && sptGlobalArchives[ePackType])
    {
        Console::IO::ChangeArchivePriority(sptGlobalArchives[ePackType], nArchivePriority);
    }

    if (ePackType != SNOPACKTYPE_INVALID && sptLocaleArchives[ePackType])
    {
        Console::IO::ChangeArchivePriority(sptLocaleArchives[ePackType], nArchivePriority);
    }
}




void sActToSNOPackType(Act eAct, SNOPackType & eNormalPack, SNOPackType & eCachePack)



{
    switch (eAct)
    {
    case ACT_1: eNormalPack = SNOPACKTYPE_CONSOLE_ACT1; eCachePack = SNOPACKTYPE_CONSOLE_CACHE_ACT1; break;
    case ACT_2: eNormalPack = SNOPACKTYPE_CONSOLE_ACT2; eCachePack = SNOPACKTYPE_CONSOLE_CACHE_ACT2; break;
    case ACT_3: eNormalPack = SNOPACKTYPE_CONSOLE_ACT3; eCachePack = SNOPACKTYPE_CONSOLE_CACHE_ACT3; break;
    case ACT_4: eNormalPack = SNOPACKTYPE_CONSOLE_ACT4; eCachePack = SNOPACKTYPE_CONSOLE_CACHE_ACT4; break;
    case ACT_5: eNormalPack = SNOPACKTYPE_CONSOLE_ACT5; eCachePack = SNOPACKTYPE_CONSOLE_CACHE_ACT5; break;
    default: eNormalPack = SNOPACKTYPE_INVALID; eCachePack = SNOPACKTYPE_INVALID; break;
    }
}















































































































































































































































void SNOFilesUpdate()
{
}






















BOOL SNOPackChunkLoadAct(Act eAct);
void CGameClientUpdateLock(); void CGameClientUpdateUnlock(); BOOL CGameAsyncRenderFlush();  // X/Client/CGame.h
namespace SNOPreload { void SetAct(Act eAct); }  // X/Client/SNOPreload.h

void SNOFilesSetCurrentAct(Act eAct)

{
    if (!sgtAppGlobals.fInitialized)
    {
        return;
    }


    sgptSNOGlobals->ePendingAct = eAct;


    if (!SNOPackChunkLoadAct(eAct))
    {
        return;
    }


    if (sgptSNOGlobals->eCurrentAct != eAct)
    {
        BOOL fIsServer = GameActiveGameCommonDataIsServer();
        if (fIsServer)
        {
            CGameClientUpdateLock();
        }
        CGameAsyncRenderFlush();













        // Line 8537: `static const Act seActs[5]` (DWARF); its values are not in the image and
        // nothing that survives reads them, so it is left out. The loops below run 5 times.


        if (eAct == ACT_OPEN_WORLD)
        {

            Blizzard::Lock::ScopedMutex lock(stArchiveLock);




            for (int32 i = 0; i < 5; i++)
            {
                SNOPackType eNormal;
                SNOPackType eCache;
                sActToSNOPackType(sgptSNOGlobals->eCurrentAct, eNormal, eCache);



                sChangeActSNOPacksPriority(eNormal, 2);




                sChangeActSNOPacksPriority(eNormal, 4);
            }


            sgptSNOGlobals->eCurrentAct = ACT_OPEN_WORLD;
        }
        else
        {



            if (sgptSNOGlobals->eCurrentAct == ACT_OPEN_WORLD)
            {
                Blizzard::Lock::ScopedMutex lock(stArchiveLock);


                for (int32 i = 0; i < 5; i++)
                {
                    SNOPackType eNormal;
                    SNOPackType eCache;
                    sActToSNOPackType(sgptSNOGlobals->eCurrentAct, eNormal, eCache);



                    sChangeActSNOPacksPriority(eNormal, -4);




                    sChangeActSNOPacksPriority(eNormal, -2);
                }

            }


            SNOPackType eNormal;
            SNOPackType eCache;
            sActToSNOPackType(sgptSNOGlobals->eCurrentAct, eNormal, eCache);

            {
                Blizzard::Lock::ScopedMutex lock(stArchiveLock);











                sChangeActSNOPacksPriority(eNormal, -4);











                sChangeActSNOPacksPriority(eCache, -2);



                sgptSNOGlobals->eCurrentAct = eAct;

                sActToSNOPackType(eAct, eNormal, eCache);





                sChangeActSNOPacksPriority(eNormal, 2);








                sChangeActSNOPacksPriority(eCache, 4);


            }
        }

        SNOPreload::SetAct(eAct);




        if (fIsServer)
        {
            CGameClientUpdateUnlock();
        }
    }
}










































#include "../../Sigma/BlzExt/BlzVectorHelper.h"
void FileFindStart(void *& portableState, const FileReference & tBaseRef, LPCSTR szFilename, const BOOL fFindRecursive, const BOOL fIncludeDirs);  // X/Sigma/Files/XFilesCore.cpp:1509
BOOL FileFindNext(void * portableState, FileReference & tResult);  // X/Sigma/Files/XFilesCore.cpp:1532


#include "../SNO/SNOTOC.h"
#include "../../Client/Prefetch.h"
#include "../../Client/ImageCollection.h"
#include "../Console/IO/ConsoleIO.h"
#include "../../Client/SNOPreload.h"
#include "../Definitions/StringListDefinition.h"
#include "../Definitions/Definitions.h"
// X/Common/GameBalance (not declared in an adopted header yet).
void GameBalanceApplyHotfix(SNO sno);
extern SNOTOC gtTOC;
BOOL ImageCollectionApplyHotfix(const ImageCollectionType eCollection, const char * szHotfixName);
void ItemsReinitialize();




static void sApplyHotfix(LPCSTR szHotfixName)
{
    gtTOC.ApplyHotfix(szHotfixName);

    Prefetch::ApplyHotfix(szHotfixName);


    ImageCollectionApplyHotfix(0, CRefString().Format("Misc\\Hotfixes\\%s.itemic", szHotfixName));


    ImageCollectionApplyHotfix(1, CRefString().Format("Misc\\Hotfixes\\%s.mapic", szHotfixName));
    ImageCollectionApplyHotfix(2, CRefString().Format("Misc\\Hotfixes\\%s.uiic", szHotfixName));

    SNOPreload::ApplyHotfix(szHotfixName);


    StringListApplyHotfix(szHotfixName);
}






BOOL SNOFilesApplyHotfixFromMemory(LPCSTR szHotfixName, const uint8 * pHotfixData, int32 nHotfixSize, BOOL fValidateCRC, LPCSTR szHotfixLocaleName, const uint8 * pHotfixLocaleData, int32 nHotfixLocaleSize)
{







    Console::IO::CPKArchiveReader * ptArchive = NULL;
    if (Console::IO::OpenArchiveFromMemory(szHotfixName, pHotfixData, nHotfixSize, 12, fValidateCRC, &ptArchive, Console::IO::TASKPRIORITY_HIGHEST) != Console::IO::IORESULT_SUCCESS)
    {
        return FALSE;
    }


    HSARCHIVE hArchive = (HSARCHIVE)ptArchive;
    sgptSNOGlobals->tArchiveList.push_back(hArchive);

    if (pHotfixLocaleData)
    {


        if (Console::IO::OpenArchiveFromMemory(szHotfixLocaleName, pHotfixLocaleData, nHotfixLocaleSize, 12, fValidateCRC, &ptArchive, Console::IO::TASKPRIORITY_HIGHEST) != Console::IO::IORESULT_SUCCESS)
        {
            return FALSE;
        }

        hArchive = (HSARCHIVE)ptArchive;
        sgptSNOGlobals->tArchiveList.push_back(hArchive);
    }

    sApplyHotfix(szHotfixName);

    if (sgtAppGlobals.ptItemAppGlobals)
    {
        ItemsReinitialize();
    }

    return TRUE;
}



BOOL SNOFilesApplyHotfix(LPCSTR szHotfixName, BOOL fValidateCRC, LPCSTR szHotfixPath, LPCSTR szHotfixLocalePath)
{




    Console::IO::CPKArchiveReader * ptArchive = NULL;
    if (Console::IO::OpenArchive(szHotfixPath, 12, TRUE, fValidateCRC, &ptArchive, Console::IO::TASKPRIORITY_HIGHEST) != Console::IO::IORESULT_SUCCESS)
    {
        return FALSE;
    }


    HSARCHIVE hArchive = (HSARCHIVE)ptArchive;
    sgptSNOGlobals->tArchiveList.push_back(hArchive);

    if (szHotfixLocalePath)
    {
        ptArchive = NULL;
        if (Console::IO::OpenArchive(szHotfixLocalePath, 12, TRUE, fValidateCRC, &ptArchive, Console::IO::TASKPRIORITY_HIGHEST) != Console::IO::IORESULT_SUCCESS)
        {
            return FALSE;
        }


        hArchive = (HSARCHIVE)ptArchive;
        sgptSNOGlobals->tArchiveList.push_back(hArchive);
    }

    sApplyHotfix(szHotfixName);

    if (sgtAppGlobals.ptItemAppGlobals)
    {
        ItemsReinitialize();
    }

    return TRUE;
}
void SNOFileAddHotfixEntry(const SNOGroup snoGroup, const uint32 uGuid, LPCSTR szFileNameNoExtension)
{



    SNOFileAddEntry(SNOTOC::TT_HOTFIX, snoGroup, uGuid, szFileNameNoExtension);
    GroupDefinition * ptGroupDef = GameDefinitionGet(snoGroup);

    ptGroupDef->Purge(uGuid, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);



    const SNOGroupInfo * ptInfo = SNOGroupGetInfo(snoGroup);


    if (SNOPreload::IsInitializedForApp() && ptInfo &&
        ((ptInfo->dwFlags & ((1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_PER_ACT) | (1 << SNOGROUP_ALWAYS_LOADED_FOR_APP_GLOBAL))) ||
         (SNOPreload::IsInitializedForGame() && (ptInfo->dwFlags & ((1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_PER_ACT) | (1 << SNOGROUP_ALWAYS_LOADED_FOR_GAME_GLOBAL))))))
    {
        const void * pdefHotfix = ptGroupDef->LockR(uGuid, FALSE);
        ptGroupDef->UnlockR((void *&)pdefHotfix);
    }



    if (snoGroup == GAMEBALANCE_SNO && sgtAppGlobals.ptGBAppGlobals)
    {

        GameBalanceApplyHotfix(uGuid);
    }

}
// 0x710074fff0 compares the two CRefString elements in the hotfix list.
extern "C" int32 sub_710074fff0(const void *, const void *);

static int32 sHotfixSort(const void * ptA, const void * ptB)
{
    return sub_710074fff0(ptA, ptB);
}






void SNOFilesApplyAllHotfixInFolder(
    const char * szPath,
    BOOL fValidateCRC)
{



    FileReference tRef, tResult;
    void * ptFindFileState;
    FileReferenceInit(tRef, szPath);
    FileFindStart(ptFindFileState, tRef, "*.cpk", FALSE, FALSE);

    blz::vector<CRefString> tHotfixes;

    while (FileFindNext(ptFindFileState, tResult))
    {
        CHAR szFileName[4096];
        FileSplitPath(FileReferenceGetPath(tResult), NULL, NULL, szFileName, NULL);


        if (szFileName[4] == '_')
        {
            CHAR szLocale[5];
            strncpy(szLocale, szFileName, 4);
            szLocale[4] = '\0';
            if (XLocaleFromString(szLocale) != NUM_XLOCALES)
            {
                continue;
            }
        }

        tHotfixes.push_back(CRefString(szFileName));
    }

    blzext::vector_sort(tHotfixes, sHotfixSort);



    for (uint32 i = 0; i < tHotfixes.size(); i++)
    {
        const char * szFileName = tHotfixes[i].str();
        CHAR szFilePath[4096];
        sprintf(szFilePath, "%s\\%s.cpk", szPath, szFileName);

        CHAR szLocaleFileName[4096];
        sprintf(szLocaleFileName, "%s_%s", XLocaleToString(UnicodeText::GetCurrentLocale()), szFileName);
        FileReference tRefLocale;
        FileReferenceInit(tRefLocale, CRefString().Format("%s\\%s.cpk", szPath, szLocaleFileName));

        BOOL fLocaleFileExists = FileExists(tRefLocale);


        SNOFilesApplyHotfix(szFileName, fValidateCRC, szFilePath, fLocaleFileExists ? tRefLocale.szPath : NULL);
    }
}





static HSARCHIVE sOpenSNOPack(LPCSTR szPath, int32 nPriority)
{


    FileReference tRef;
    FileReferenceInit(tRef, szPath);


    if (!FileExists(tRef))
    {
        return 0;
    }











    CRefString sTimer;
    sTimer.Format("sOpenSNOPack %s", szPath);






















    uint32 dwFlags = 0x9c08;
    LPCSTR szFilePath = FileReferenceGetPath(tRef);
    Blizzard::File::SetLastErrorStack(NULL);
    HSARCHIVE hArchive;
    if (!SFileOpenArchive(szFilePath, nPriority, dwFlags, &hArchive))
    {
        sDisplaySNOFileError(ERROR_FILE_OPEN, tRef, "Could not open SNO pack", __FILE__, __LINE__);
        return 0;
    }









    sgptSNOGlobals->tArchiveList.push_back(hArchive);







    return hArchive;
}




static BOOL sReadLocalSNOFileHeader(
    SNOFileHeader & tSNOFileHeader,
    FileReference & tRef)
{

    LPCSTR szFilePath = FileReferenceGetPath(tRef);

    uint32 dwFileSize;
    if (!FileGetSize(tRef, dwFileSize))
    {
        return FALSE;
    }


    if (dwFileSize < sizeof(SNOFileHeader))
    {
        return FALSE;
    }


    SNOFileHeader tSNOHeaderOnDisk;
    if (!FileRead(tRef, &tSNOHeaderOnDisk, 0, sizeof(SNOFileHeader), ERROR_FILE_READ))
    {
        sDisplayFileReadError(NULL, 0, tRef, "", 0, 0);
        return FALSE;
    }
    return sSNOHeaderFromFileHeader(tSNOHeaderOnDisk, tSNOFileHeader);
}



























































































void SNOFixFilename(LPSTR szName)
{
    size_t nLen = strlen(szName);
    size_t nIndex;
    while ((nIndex = strcspn(szName, ",:./\\*?\"<>|\t\n\r$")) != nLen)
    {
        SigmaMemoryMove(szName + nIndex, szName + nIndex + 1, nLen - nIndex);
        nLen--;
        szName[nLen] = '\0';
    }
}









static void sDisplaySNOFileError(ErrorCode eErrorCode, LPCSTR szMessage, LPCSTR szFile, int32 nLine);
static void sDisplaySNOFileError(
    ErrorCode eErrorCode,
    const SNOFileReference & tSNOFileRef,
    LPCSTR szMessage,
    LPCSTR szFile,
    int32 nLine)
{
    if (tSNOFileRef.eLocation == SNOFileReference::PACK)
    {
        sDisplaySNOFileError(eErrorCode, szMessage, szFile, nLine);
    }
    else
    {

        sDisplaySNOFileError(eErrorCode, tSNOFileRef.tRef, szMessage, szFile, nLine);
    }
}



static void sDisplaySNOFileError(ErrorCode eErrorCode, const FileReference & tFileRef, LPCSTR szMessage, LPCSTR szFile, int32 nLine)
{









    DisplayErrorMessage(eErrorCode, "");
}






static void sDisplaySNOFileError(
    ErrorCode eErrorCode,
    LPCSTR szMessage,
    LPCSTR szFile,
    int32 nLine)
{



    DisplayErrorMessage(eErrorCode, CRefString().Format("%s", szMessage));
}

























XLocale SNOFileGetLocale(
    const SNOName & snoname)
{
    SNOElement * ptSNOElement = SNOFileLookup(snoname);

    if (!ptSNOElement)
    {
        return XLOCALE_INVALID;
    }

    SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);

    return ptLazyData->eXLocale;
}





















inline LPCSTR SNOFilesGetBranch()
{
    return sgptSNOGlobals->szBranch;
}
















































BOOL SNOFileGetName(const SNOName & snoname, CRefString & sSNOName)
{


    SNOElement * ptSNOElement = SNOFileLookup(snoname);

    if (ptSNOElement != NULL)
    {












        sSNOName = sSNOEntryGetName(ptSNOElement->tEntry);
        return TRUE;
    }
    return FALSE;
}
































CRefString SNOFileGetName(const SNOName & snoname)
{

    CRefString sSNOName;
    if (SNOFileGetName(snoname, sSNOName))
    {
        return sSNOName;
    }
    else
    {

        return CRefString("[bad sno]");
    }








}


































void SNOFilesMapCheckConsistency()
{
    for (int32 nGroupIndex = 0; nGroupIndex < MAX_SNO_GROUPS; nGroupIndex++)
    {
        MapStringToSNO & tStringMap = sgptSNOGlobals->tStringToSNOMap[nGroupIndex];

        POS pos = tStringMap.GetStartAssoc();
        while (pos)
        {
            SNOFilename key;
            SNO value;
            tStringMap.GetNextAssoc(pos, key, value);
            if (!tStringMap.LookupPtr(key))
            {
                static int s_nErrorCounter = 0;
                s_nErrorCounter++;
            }
        }
    }









    for (int32 nGroupIndex = 0; nGroupIndex < MAX_SNO_GROUPS; nGroupIndex++)
    {
        MapStringToSNOHash & tStringMap = sgptSNOGlobals->tStringToSNOMapHash[nGroupIndex];

        POS pos = tStringMap.GetStartAssoc();
        while (pos)
        {
            SNOFilenameHash key;
            SNO value;
            tStringMap.GetNextAssoc(pos, key, value);
            if (!tStringMap.LookupPtr(key))
            {
                static int s_nErrorCounter = 0;
                s_nErrorCounter++;
            }
        }
    }
}









static void sSNOFileMapDispose()
{



    for (int32 nGroupIndex = 0; nGroupIndex < MAX_SNO_GROUPS; nGroupIndex++)
    {
        MapStringToSNO & tStringMap = sgptSNOGlobals->tStringToSNOMap[nGroupIndex];
        tStringMap.RemoveAll();

        MapSNOToSNOElementPtr & tElementMap = sgptSNOGlobals->tSNOToSNOElementMap[nGroupIndex];
        tElementMap.RemoveAll();
    }


    for (int32 nGroupIndex = 0; nGroupIndex < MAX_SNO_GROUPS; nGroupIndex++)
    {
        MapStringToSNOHash & tStringMap = sgptSNOGlobals->tStringToSNOMapHash[nGroupIndex];
        tStringMap.RemoveAll();
    }
}





static BOOL sSNOFileMapAdd(SNOElement * ptSNOElement)
{
    SNOEntry & tEntry = ptSNOElement->tEntry;

    if (!SetSNOElement(SNOName(tEntry.snoGroup, tEntry.uGuid), ptSNOElement))
    {
        return FALSE;
    }



    if (sSNOGroupNeedsName(tEntry.snoGroup))
    {
        SNOFilename tName((LPCSTR)tEntry.pNameOrHash);
        sgptSNOGlobals->tStringToSNOMap[tEntry.snoGroup].SetAt_Tracked(tName, tEntry.uGuid);
    }
    else
    {

        SNOFilenameHash tName((uint8 *)tEntry.pNameOrHash);
        sgptSNOGlobals->tStringToSNOMapHash[tEntry.snoGroup].SetAt_Tracked(tName, tEntry.uGuid);
    }

    return TRUE;
}




void SNOFilesPrepareFromTOC(SNOTOC::TOCType eType,
                            uint32 * dwCountPerGroup,
                            uint32 * dwNameSizePerGroup,
                            FullTOCDetail * ptFullTOCDetail)

{


    SNOTOCData & tTOCData = sgptSNOGlobals->tTOCData[eType];

    for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
    {
        tTOCData.dwTOCCountPerGroup[i] = dwCountPerGroup[i];
    }


    uint32 dwTotalCount = 0;
    for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
    {
        dwTotalCount += dwCountPerGroup[i];
    }

    tTOCData.ptSNOElements->SetGlobalCount(dwTotalCount);



    for (int32 i = 0; i < MAX_SNO_GROUPS; i++)
    {

        if (sSNOGroupNeedsName(i))
        {
            tTOCData.dwSNOEntryNameBufferSizes[i] += dwNameSizePerGroup[i];
        }
        else
        {

            tTOCData.dwSNOEntryNameBufferSizes[i] += dwCountPerGroup[i] * sizeof(uint64);
        }


        tTOCData.szSNOEntryNameBuffers[i] = new CHAR[tTOCData.dwSNOEntryNameBufferSizes[i]];
    }




    if (ptFullTOCDetail && !sgptSNOGlobals->pptSNOToSNOElement)
    {


        for (int32 nGroupIndex = 0; nGroupIndex < MAX_SNO_GROUPS; nGroupIndex++)
        {
            uint32 dwItemCount = ptFullTOCDetail->dwFullCountPerGroup[nGroupIndex];
            sgptSNOGlobals->dwFullTOCCountPerGroup[nGroupIndex] = dwItemCount;

            uint32 dwAvgItemCount = dwItemCount / 3;

            if (sSNOGroupNeedsName(nGroupIndex))
            {
                MapStringToSNO & tStringMap = sgptSNOGlobals->tStringToSNOMap[nGroupIndex];
                tStringMap.SetHashTableSize_Tracked(dwAvgItemCount);
                tStringMap.SetBlockCount_Tracked(dwItemCount);
                tStringMap.SetGrow(XALLOC_GROW_LINEAR, 128);
                tStringMap.SetShrinkable(TRUE);
            }
            else
            {
                MapStringToSNOHash & tStringMap = sgptSNOGlobals->tStringToSNOMapHash[nGroupIndex];
                tStringMap.SetHashTableSize_Tracked(dwAvgItemCount);
                tStringMap.SetBlockCount_Tracked(dwItemCount);
                tStringMap.SetGrow(XALLOC_GROW_LINEAR, 128);
                tStringMap.SetShrinkable(TRUE);
            }
        }

        sgptSNOGlobals->dwSNOElementArraySize = dwTotalCount = ptFullTOCDetail->dwHighestSNOHandle + 1;  // dwTotalCount reused: the size is not re-read after operator new[] and the DWARF has no other local

        sgptSNOGlobals->pptSNOToSNOElement = new SNOElement *[dwTotalCount];
        SigmaMemoryClear(sgptSNOGlobals->pptSNOToSNOElement, dwTotalCount * sizeof(SNOElement *));
    }










}




uint32 SNOFilesGetTOCCountForGroup(const SNOGroup snoGroup)
{
    return sgptSNOGlobals->dwFullTOCCountPerGroup[snoGroup];
}




SNO SNOFilesGetHighestUsedSNOHandle()
{
    return sgptSNOGlobals->dwSNOElementArraySize - 1;
}























void SNOFileAddEntry(SNOTOC::TOCType eType,
                     const SNOGroup snoGroup,
                     const uint32 uGuid,
                     LPCSTR szFileNameNoExtension)
{



    const SNOGroupInfo * ptSNOGroupInfo = SNOGroupGetInfo(snoGroup);
    if (!ptSNOGroupInfo)
    {
        return;
    }



    SNOName snoname(snoGroup, uGuid);
    SNOElement * ptOldSNOElement = sGetSNOElement(snoname);
    if (ptOldSNOElement)
    {


        if (eType == SNOTOC::TT_HOTFIX)
        {
            if (SNOFileLookupGUID(snoGroup, szFileNameNoExtension) != uGuid)
            {

                if (sSNOGroupNeedsName(snoGroup))
                {
                    SNOFilename tName((LPCSTR)ptOldSNOElement->tEntry.pNameOrHash);
                    sgptSNOGlobals->tStringToSNOMap[snoGroup].SetAt_Tracked(tName, uGuid);
                }
                else
                {
                    SNOFilenameHash tName((uint8 *)ptOldSNOElement->tEntry.pNameOrHash);
                    sgptSNOGlobals->tStringToSNOMapHash[snoGroup].SetAt_Tracked(tName, uGuid);
                }
            }
        }

        return;
    }

    SNOElement * ptSNOElement = sgptSNOGlobals->tTOCData[eType].ptSNOElements->ElementNew();

    SNOEntry & tEntry = ptSNOElement->tEntry;

    tEntry.snoGroup = snoGroup;
    tEntry.uGuid = uGuid;


    tEntry.SetName(eType, szFileNameNoExtension);

    ptSNOElement->dwFlags = 0;












    BOOL fSuccess = sSNOFileMapAdd(ptSNOElement);

}




static uint32 sGetLocalGUID()
{





    static uint32 uLocalGUID;
    uint32 uGuidReturn = uLocalGUID++;
    return uGuidReturn;
}




inline SNO SNOFileAddLocalEntry(const SNOGroup snoGroup, LPCSTR szName)
{



    SNO snoHandle = SNOFileLookupGUID(snoGroup, szName);
    if (snoHandle == SNO_NONE)
    {





        uint32 uGuidReturn = sGetLocalGUID();
        SNOFileAddEntry(SNOTOC::TT_CORE, snoGroup, uGuidReturn, szName);
        snoHandle = uGuidReturn;
    }
    return snoHandle;
}

static void sSNOFileMapDelete(
    SNOElement * ptSNOElement,
    BOOL fDeleteFromElements)
{
    SNOEntry & tEntry = ptSNOElement->tEntry;


    if (sSNOGroupNeedsName(tEntry.snoGroup))
    {
        MapStringToSNO & tStringMap = sgptSNOGlobals->tStringToSNOMap[tEntry.snoGroup];
        SNOFilename tName(tEntry.GetName());

        SNO snoLookup;
        if (tStringMap.Lookup(tName, snoLookup))
        {
            tStringMap.RemoveAt(tName);
        }
    }
    else
    {

        MapStringToSNOHash & tStringMap = sgptSNOGlobals->tStringToSNOMapHash[tEntry.snoGroup];
        SNOFilenameHash tName((uint8 *)tEntry.pNameOrHash);

        SNO snoLookup;
        if (tStringMap.Lookup(tName, snoLookup))
        {
            tStringMap.RemoveAt(tName);
        }
    }


    SetSNOElement(SNOName(tEntry.snoGroup, tEntry.uGuid), NULL);








    if (fDeleteFromElements)
    {
        SNOTOC::TOCType eType = sGetTOCTypeFromSNOElement(ptSNOElement);
        sgptSNOGlobals->tTOCData[eType].ptSNOElements->ElementDelete(ptSNOElement);
    }
}




uint32 SNOGetCoreDataHash()
{
    return sgptSNOGlobals->dwSNOCoreDataHash;
}





CRefString SNOFilesGetPlatformDataPath(const SigmaPlatform eSigmaPlatform)
{
    CRefString sPath;
    return CRefString(sPath.Format("%s", SNOFilesGetTitleDataPath().str()));
}








CRefString SNOFilesGetLocalCachePath(const XLocale eXLocale)
{
    CRefString sPlatformDataPath = SNOFilesGetPlatformDataPath(geSigmaPlatform);
    LPCSTR szLocale = XLocaleToString(eXLocale);
    if (eXLocale == XLOCALE_GLOBAL)
    {
        CRefString sPath;
        return CRefString(sPath.Format("%s\\%s", sPlatformDataPath.str(), szLocale));
    }
    else
    {
        CRefString sPath;
        return CRefString(sPath.Format("%s\\Locale\\%s", sPlatformDataPath.str(), szLocale));
    }
}













CRefString SNOFilesGetMPQPath()
{
    CRefString sPath;
    return CRefString(sPath.Format("%s\\CPKs", s_szConsoleDataPathOverride));
}



































CRefString SNOFilesGetMiscPath()
{
    CRefString sPath;
    return CRefString(sPath.Format("%s\\Misc", SNOFilesGetPlatformDataPath(geSigmaPlatform).str()));
}








































































































uint32 SNOFileGetDataSize(
    SNOElement * ptSNOElement,
    const BOOL fUncompressedSize)
{
    SNOLazyData * ptLazyData = sSNOFileLoadLazyData(ptSNOElement, FALSE, NULL, NULL);


    uint32 uSize = 0;
    switch (ptLazyData->eSNOFileType)
    {
    case SNOFILETYPE_LOCAL:

        if (!FileGetSize(ptLazyData->tSNOFileRef.tRef, uSize))
        {
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }


        if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
        {
            uSize -= sizeof(SNOFileHeader);
        }
        break;

    case SNOFILETYPE_SNOPACK:


        {
            FileIO::File tScopedFile(XLOCALE_GLOBAL);

            if (ptLazyData->m_tFile.IsAlreadyOpen())
            {
                tScopedFile.Open(ptLazyData->m_tFile);
            }
            else
            {
                if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
                {
                    tScopedFile.OpenSNO(ptSNOElement->tEntry.ToSNOName());
                }
                else
                {
                    tScopedFile.OpenChildSNO(ptSNOElement->tEntry.ToSNOName(), ((SNOElementChild *)ptSNOElement)->szChildFile);
                }
            }

            if (fUncompressedSize)
            {
                uSize = tScopedFile.GetFileSize();
                if (uSize == (uint32)-1)
                {
                    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                }



                if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))
                {
                    uSize -= sizeof(SNOFileHeader);
                }
            }
            else
            {

                uSize = tScopedFile.GetCompressedFileSize();
                if (uSize == (uint32)-1)
                {
                    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                }

            }
            if (!(ptSNOElement->dwFlags & SNOENTRY_FLAG_CHILD_FILE))  // Both size-query paths read dwFlags here (0x6967a8).
            {
            }



        }
        break;
    }
    return uSize;
}




static void sSNOHeaderToFileHeader(const SNOFileHeader & tSNOFileHeader, SNOFileHeader & tSNOHeaderOnDisk)
{



    DT_SNOFILEHEADER->Byteswap(&tSNOHeaderOnDisk, (void *)&tSNOFileHeader);
}



static BOOL sSNOHeaderFromFileHeader(
    const SNOFileHeader & tSNOHeaderOnDisk,
    SNOFileHeader & tSNOFileHeader)
{
    uint32 dwSig = tSNOHeaderOnDisk.dwSignature;

    if (dwSig != SNOHEADER_SIGNATURE)
    {
        return FALSE;
    }

    DT_SNOFILEHEADER->Byteswap(&tSNOFileHeader, (void *)&tSNOHeaderOnDisk);
    return TRUE;
}





static void sDisplayFileReadError(
    LPCSTR szFile,
    int32 nLine,
    FileReference & tFileRef,
    LPCSTR szMessage,
    uint64 uReadOffset,
    uint64 uReadSize)
{
    LPCSTR szFilePath = FileReferenceGetPath(tFileRef);
    int32 nLastError = Blizzard::File::GetLastError();
    int32 nLastMopaqError = SFileGetLastError();






    if (FileExists(tFileRef))
    {
        uint32 uFileSize;

        FileGetSize(tFileRef, uFileSize);
    }
}






















static void sDisplaySNOFileReadError(
    LPCSTR szFile,
    int32 nLine,
    SNOLazyData * ptLazyData,
    LPCSTR szMessage,
    uint64 uReadOffset,
    uint64 uReadSize,
    uint64 uReadBytes,
    BOOL bRaiseError)
{
    if (ptLazyData)
    {
        FileReference tFileRef = ptLazyData->tSNOFileRef.tRef;

        if (ptLazyData->tSNOFileRef.eLocation == SNOFileReference::PACK)
        {
            ptLazyData->m_tFile.GetFileName(tFileRef.szPath, sizeof(tFileRef.szPath));
        }

        sDisplayFileReadError(szFile, nLine, tFileRef, szMessage, uReadOffset, uReadSize);

        if (ptLazyData->tSNOFileRef.eLocation == SNOFileReference::PACK)
        {
            ptLazyData->m_tFile.GetFileName(tFileRef.szPath, sizeof(tFileRef.szPath));
            if (FileIO::GetLastErrorCode() != 0)
            {
                ptLazyData->m_tFile.TraceFile();
            }
        }






        if (bRaiseError)
        {
            sDisplaySNOFileError(ERROR_FILE_READ, ptLazyData->tSNOFileRef, szMessage, szFile, nLine);
        }
    }




}




BOOL SNOCheckSNOReference(SNOGroup snoGroupSrc, SNO snoHandleSrc, SNOGroup snoGroupDst, SNO snoHandleDst, LPCSTR szExtraMsg)
{
    BOOL bRefOK = FALSE;







    if (snoHandleDst != SNO_NONE)
    {
        SNOName snonameSrc(snoGroupSrc, snoHandleSrc);
        bRefOK = SNOFileExists(snoGroupDst, snoHandleDst);
        if (!bRefOK)
        {
            CRefString sSource = HyperlinkCreateFromSNO(snonameSrc, NULL);
            LPCSTR szGroupDst = SNOGroupGetName(snoGroupDst);
        }
    }
    return bRefOK;
}








































static void sLoadUserDataSNOs()
{


    CRefString sFolderPath = SNOFilesGetUserDataPath(SNOFilesGetBranch());
    SNOGroup snoGroups[MAX_SNO_GROUPS];
    int32 nGroupCount = SNOGroupGetGroups(snoGroups);
    for (int32 i = 0; i < nGroupCount; i++)
    {
        const SNOGroupInfo * ptSNOGroupInfo = SNOGroupGetInfo(snoGroups[i]);

        CRefString sSubPath = CRefString().Format("%s\\%s", sFolderPath.str(), ptSNOGroupInfo->szName);

        FileReference tRef;
        FileReference tResult;

        FileReferenceInit(tRef, sSubPath);
        void * ptFindFileState;  FileFindStart(ptFindFileState, tRef, "*.*", TRUE, FALSE);

        while (FileFindNext(ptFindFileState, tResult))
        {
            CHAR szFileName[4096];
            FileSplitPath(FileReferenceGetPath(tResult), NULL, NULL, szFileName, NULL);
            SNOFileAddLocalEntry(snoGroups[i], szFileName);
        }


        FileFindClose(ptFindFileState);
    }
}














































































void sTraceBinary(const uint8 * pData, size_t nSize)
{
    char szByte[16];
    char szLine[1024];
    const uint32 nBytesPerLine = 16;


    szLine[0] = '\0';
    for (size_t nByte = 0; nByte < nSize; nByte++)
    {
        if (szLine[0] == '\0')
        {

            Blizzard::String::Format(szByte, sizeof(szByte), "%02X: ", nByte);
            Blizzard::String::Append(szLine, szByte, sizeof(szLine));
        }

        Blizzard::String::Format(szByte, sizeof(szByte), "%02X ", pData[nByte]);
        Blizzard::String::Append(szLine, szByte, sizeof(szLine));

        switch (nByte % nBytesPerLine)
        {
        case 7:
            Blizzard::String::Append(szLine, " ", sizeof(szLine));
            break;
        case 15:
            Blizzard::String::Append(szLine, "\n", sizeof(szLine));
            szLine[0] = '\0';
            break;
        }
    }
    if (szLine[0] != '\0')
    {
        Blizzard::String::Append(szLine, "\n", sizeof(szLine));
    }
}


































































































const char * SNOFilesGetMPQExtension()
{
    return "cpk";
}




















































































































AsyncRecordRef SNOFileGetAsyncRecord(SNOElement * ptSNOElement)
{
    Blizzard::Lock::ScopedMutex scopedMutex(sgptSNOGlobals->tSNOElementDetailMutex);
    MapSNOElementToSNOElementDetail & tSNOElementToSNOElementDetail = sgptSNOGlobals->tSNOElementToSNOElementDetail;
    SNOElementDetail tDetail;
    if (tSNOElementToSNOElementDetail.Lookup(ptSNOElement, tDetail))
    {
        return tDetail.tAsyncRecord;
    }
    return NULL;
}







SNOLazyData * SNOFileGetLazyData(
    SNOElement * ptSNOElement)
{
    Blizzard::Lock::ScopedMutex scopedMutex(sgptSNOGlobals->tSNOElementDetailMutex);

    MapSNOElementToSNOElementDetail & tSNOElementToSNOElementDetail = sgptSNOGlobals->tSNOElementToSNOElementDetail;
    SNOElementDetail tDetail;
    if (tSNOElementToSNOElementDetail.Lookup(ptSNOElement, tDetail))
    {
        return tDetail.ptLazyData;
    }

    return NULL;
}




void SNOFileAssignAsyncRecord(SNOElement * ptSNOElement, AsyncRecordRef pos)
{
    BOOL fWasPresent;
    Blizzard::Lock::ScopedMutex scopedMutex(sgptSNOGlobals->tSNOElementDetailMutex);
    MapSNOElementToSNOElementDetail & tSNOElementToSNOElementDetail = sgptSNOGlobals->tSNOElementToSNOElementDetail;

    SNOElementDetail tDetail;
    fWasPresent = tSNOElementToSNOElementDetail.Lookup(ptSNOElement, tDetail);
    if (!fWasPresent)
    {
        tDetail.ptLazyData = NULL;
    }
    tDetail.tAsyncRecord = pos;








    if (pos)
    {
        tSNOElementToSNOElementDetail.SetAt_Tracked(ptSNOElement, tDetail);
    }
    else if (fWasPresent)
    {
        if (tDetail.ptLazyData == NULL)
        {
            tSNOElementToSNOElementDetail.RemoveAt(ptSNOElement);
        }
        else
        {
            tSNOElementToSNOElementDetail.SetAt_Tracked(ptSNOElement, tDetail);
        }
    }
}



static void sSNOFileAssignLazyData(
    SNOElement * ptSNOElement,
    SNOLazyData * ptLazyData)
{
    Blizzard::Lock::ScopedMutex scopedMutex(sgptSNOGlobals->tSNOElementDetailMutex);

    MapSNOElementToSNOElementDetail & tSNOElementToSNOElementDetail = sgptSNOGlobals->tSNOElementToSNOElementDetail;
    SNOElementDetail tDetail;
    BOOL fWasPresent = tSNOElementToSNOElementDetail.Lookup(ptSNOElement, tDetail);
    if (!fWasPresent)
    {
        tDetail.ptLazyData = NULL;
        tDetail.tAsyncRecord = NULL;
    }

    if (tDetail.ptLazyData)
    {
        delete tDetail.ptLazyData;
    }
    tDetail.ptLazyData = ptLazyData;

    if (ptLazyData)
    {
        tSNOElementToSNOElementDetail.SetAt_Tracked(ptSNOElement, tDetail);
    }
    else if (fWasPresent)


    {
        if (tDetail.tAsyncRecord == NULL)
        {
            tSNOElementToSNOElementDetail.RemoveAt(ptSNOElement);
        }
        else
        {
            tSNOElementToSNOElementDetail.SetAt_Tracked(ptSNOElement, tDetail);
        }
    }

}











































































































































































































































































































































































const char * GetCutsceneChunkPrefix(CutsceneCPKType eCutsceneCPKType)
{






    const char * aszCutscenePrefix[] = { "Common", "Act1", "Act2", "Act3", "Act4", "Act5" };








    return aszCutscenePrefix[eCutsceneCPKType];
}






static SNOPackChunkState saeSNOPackChunkStates[SNOPACKCHUNK_COUNT] = { SNOPACKCHUNKSTATE_LOADED, SNOPACKCHUNKSTATE_INVALID, SNOPACKCHUNKSTATE_INVALID, SNOPACKCHUNKSTATE_INVALID, SNOPACKCHUNKSTATE_INVALID, SNOPACKCHUNKSTATE_INVALID };









static const SNOPackType aeSNOPackChunks[SNOPACKCHUNK_COUNT] = { SNOPACKTYPE_INVALID, SNOPACKTYPE_CONSOLE_ACT1, SNOPACKTYPE_CONSOLE_ACT2, SNOPACKTYPE_CONSOLE_ACT3, SNOPACKTYPE_CONSOLE_ACT4, SNOPACKTYPE_CONSOLE_ACT5 };








static const CutsceneCPKType aeCutsceneTypeChunks[SNOPACKCHUNK_COUNT] = { CUTSCENE_CPK_TYPE_INVALID, CUTSCENE_CPK_TYPE_ACT1, CUTSCENE_CPK_TYPE_ACT2, CUTSCENE_CPK_TYPE_ACT3, CUTSCENE_CPK_TYPE_ACT4, CUTSCENE_CPK_TYPE_ACT5 };





























SNOPackChunkState SNOPackChunkGetState(SNOPackChunk eSNOPackChunk, BOOL fForceUpdate)
{


    SNOPackChunkState eSNOPackChunkState = saeSNOPackChunkStates[eSNOPackChunk];





    if (eSNOPackChunkState != SNOPACKCHUNKSTATE_NOT_READY &&




        eSNOPackChunkState != SNOPACKCHUNKSTATE_INVALID)
    { return eSNOPackChunkState; }

































    saeSNOPackChunkStates[eSNOPackChunk] = SNOPACKCHUNKSTATE_READY;
    return SNOPACKCHUNKSTATE_READY;

}






























































BOOL SNOPackChunkLoad(
    SNOPackChunk eSNOPackChunk)
{
    SNOPackChunkState eSNOPackChunkState = SNOPackChunkGetState(eSNOPackChunk, TRUE);
    if (eSNOPackChunkState != SNOPACKCHUNKSTATE_READY)
    {
        return eSNOPackChunkState == SNOPACKCHUNKSTATE_LOADED;
    }










    SNOPackType eSNOPackType = aeSNOPackChunks[eSNOPackChunk];



    XLocale nLocale = UnicodeText::GetCurrentLocale();
    CRefString szMountPath;
    Console::DLCManager::GetLocaleAOCMountPath(nLocale, szMountPath);
    CRefString sLocaleSubPath = CRefString().Format("%s\\%s", szMountPath.str(), XLocaleToString(nLocale));




















    if (eSNOPackChunk != SNOPACKCHUNK_INITIAL_PAYLOAD)
    {
        HSARCHIVE hArchive = sOpenSNOPack(CRefString().Format("%s\\%s.%s", SNOFilesGetMPQPath().str(), SNOPackTypeToString(eSNOPackType), SNOFilesGetMPQExtension()), CONSOLE_INACTIVE_ACT_PRIORITY);
        sptGlobalArchives[eSNOPackType] = (Console::IO::CPKArchiveReader *)hArchive;


        HSARCHIVE hLocaleArchive = sOpenSNOPack(CRefString().Format("%s_%s.%s", sLocaleSubPath.str(), SNOPackTypeToString(eSNOPackType), SNOFilesGetMPQExtension()), CONSOLE_INACTIVE_ACT_PRIORITY + CONSOLE_LOCALE_DIFF);
        sptLocaleArchives[eSNOPackType] = (Console::IO::CPKArchiveReader *)hLocaleArchive;



        CutsceneCPKType eCutsceneCPKType = aeCutsceneTypeChunks[eSNOPackChunk];


        sOpenSNOPack(CRefString().Format("%s_%sCutscene.%s", sLocaleSubPath.str(), GetCutsceneChunkPrefix(eCutsceneCPKType), SNOFilesGetMPQExtension()), CONSOLE_CUTSCENE);
    }

    saeSNOPackChunkStates[eSNOPackChunk] = SNOPACKCHUNKSTATE_LOADED;


    if (sgptSNOGlobals->ePendingAct != sgptSNOGlobals->eCurrentAct)
    {
        SNOFilesSetCurrentAct(sgptSNOGlobals->ePendingAct);
    }

    return TRUE;
}




float SNOPackChunkGetActLoadPercentComplete(Act eAct)
{
    return 1.0f;
}






















void SNOPackChunkRequestLoad(Act eAct)
{
}




















































































































BOOL SNOPackChunkLoadAct(Act eAct)
{
    switch (eAct)
    {
    case ACT_1: if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT1)) return FALSE; break;
    case ACT_2: if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT2)) return FALSE; break;
    case ACT_3: if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT3)) return FALSE; break;
    case ACT_4: if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT4)) return FALSE; break;
    case ACT_5: if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT5)) return FALSE; break;
    default:
        if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT1)) return FALSE;
        if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT2)) return FALSE;
        if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT3)) return FALSE;
        if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT4)) return FALSE;
        if (!SNOPackChunkLoad(SNOPACKCHUNK_ACT5)) return FALSE;
        break;
    }
    return TRUE;
}















void SNOPackSetInstallationSpeed(SNOPackInstallationSpeed eSpeed)
{
}




































































inline SNOOverflowNames::SNOOverflowNames()
{
    m_ptHead = NULL;
    m_ptTail = NULL;
}





SNOOverflowNames::~SNOOverflowNames()
{
    Clear();
}




char * SNOOverflowNames::AllocateNameBuffer(uint32 dwSize)

{
    if (!m_ptTail || m_ptTail->dwCurrentOffset + dwSize > SNOOverFlowNameBuffer::BUFFER_SIZE)
    {
        SNOOverFlowNameBuffer * ptTail = m_ptTail;
        m_ptTail = new SNOOverFlowNameBuffer;
        m_ptTail->dwCurrentOffset = dwSize;
        m_ptTail->ptNextBuffer = NULL;
        if (ptTail)
        {
            ptTail->ptNextBuffer = m_ptTail;
        }
        if (!m_ptHead)
        {
            m_ptHead = m_ptTail;
        }
        return m_ptTail->szNameBuffer;
    }

    uint32 dwCurrentOffset = m_ptTail->dwCurrentOffset;
    m_ptTail->dwCurrentOffset += dwSize;
    return m_ptTail->szNameBuffer + dwCurrentOffset;
}




void SNOOverflowNames::Clear()
{
    while (m_ptHead)
    {
        SNOOverFlowNameBuffer * ptNext = m_ptHead->ptNextBuffer;
        delete m_ptHead;
        m_ptHead = ptNext;
    }
}




BOOL SNOOverflowNames::Contains(const char * szNameBuffer)
{
    SNOOverFlowNameBuffer * ptCurrent = m_ptHead;

    while (ptCurrent)
    {
        if (szNameBuffer >= ptCurrent->szNameBuffer && szNameBuffer < ptCurrent->szNameBuffer + SNOOverFlowNameBuffer::BUFFER_SIZE)
        {
            return TRUE;
        }
        ptCurrent = ptCurrent->ptNextBuffer;
    }
    return FALSE;
}
