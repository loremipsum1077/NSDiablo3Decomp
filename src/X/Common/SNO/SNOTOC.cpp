// Original file: X/Common/SNO/SNOTOC.cpp
#include "SNOTOC.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../FileIO/FileIO.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/UnicodeText.h"

struct SNODiskEntry {  // size 0xc
    SNOGroup snoGroup;  // 0x00
    uint32 uGuid;  // 0x04
    uint32 dwNameOffset;  // 0x08
};
static_assert(sizeof(SNODiskEntry) == 0xc, "SNODiskEntry");

extern const XTypeDescriptor * DT_SNO_GROUP;

CRefString SNOFilesGetMiscPath();
void SNOFilesPrepareFromTOC(SNOTOC::TOCType eType, uint32 * dwCountPerGroup, uint32 * dwNameSizePerGroup, FullTOCDetail * ptFullTOCDetail);
void SNOFileAddEntry(SNOTOC::TOCType eType, const SNOGroup snoGroup, const uint32 uGuid, LPCSTR szFileNameNoExtension);
void SNOFileAddHotfixEntry(const SNOGroup snoGroup, const uint32 uGuid, LPCSTR szFileNameNoExtension);

//@ 25
SNOTOC gtTOC("TOC", FALSE);

//@ 40
static const TypeDescriptorField SNODiskEntry_Fields[4] = {
    TypeDescriptorField("", DT_SNO_GROUP, offsetof(SNODiskEntry, snoGroup), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SNODiskEntry, uGuid), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(SNODiskEntry, dwNameOffset), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(SNODiskEntry), NULL, 0),
};
//@ 44
ComplexTypeDescriptor TypeDescriptor_SNODiskEntry("SNODiskEntry", 0, SNODiskEntry_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SNODISKENTRY = &TypeDescriptor_SNODiskEntry;

// Only the TT_CORE branch of the two filename functions survives (both are inlined into Read with a
// constant type); the names for the other TOC types are not in the image.
//@ 59
CRefString SNOTOC::GetMPQFilename(SNOTOC::TOCType eType) const
{
    switch (eType)
    {
    case TT_CORE:
//@ 78
        return CRefString().Format("Core%s.dat", m_szFilename);
    default:
        break;
    }
    return CRefString();
}

//@ 85
CRefString SNOTOC::GetLooseFilename(SNOTOC::TOCType eType) const
{
    switch (eType)
    {
    case TT_CORE:
//@ 104
        return CRefString().Format("%s\\Core%s.dat", SNOFilesGetMiscPath().str(), m_szFilename);
    default:
        break;
    }
    return CRefString();
}

//@ 111
BOOL SNOTOC::Read(SNOTOC::PerTOCCallback ptPerTOCCallback, SNOTOC::PerEntryCallback ptPerEntryCallback, void * pUserData)
{
//@ 122
    BOOL fResult = ReadTOC(TT_CORE, GetMPQFilename(TT_CORE), GetLooseFilename(TT_CORE), ptPerTOCCallback, ptPerEntryCallback, pUserData);
//@ 137
    return fResult;
}

//@ 290
static void sAppendEntryToHotfix(SNOTOC::TOCType eType, SNOGroup snoGroup, SNO snoHandle, CHAR * szName, void * pUserData)
{
//@ 297
    SNOFileAddHotfixEntry(snoGroup, snoHandle, szName);
}

//@ 303
BOOL SNOTOC::ApplyHotfix(const char * szHotfixName)
{
//@ 306
    CRefString sFilename;
    sFilename.Format("Misc\\Hotfixes\\%s.toc", szHotfixName);
    return ReadTOC(TT_HOTFIX, sFilename, sFilename, NULL, sAppendEntryToHotfix, NULL) != FALSE;
}

//@ 320
static void sAppendEntryToSNOFiles(SNOTOC::TOCType eType, SNOGroup snoGroup, SNO snoHandle, CHAR * szName, void *)
{
//@ 327
    SNOFileAddEntry(eType, snoGroup, snoHandle, szName);
}

//@ 333
static void sPrepareSNOFilesFromTOC(SNOTOC::TOCType eType, uint32 * dwCountPerGroup, uint32 * dwNameSizePerGroup, FullTOCDetail * ptFullTOCDetail, void *)
{
//@ 340
    SNOFilesPrepareFromTOC(eType, dwCountPerGroup, dwNameSizePerGroup, ptFullTOCDetail);
}

//@ 346
BOOL SNOTOC::ReadIntoFileTable()
{
//@ 349
    return Read(sPrepareSNOFilesFromTOC, sAppendEntryToSNOFiles, NULL) != FALSE;
}

#define NUM_TOC_GROUPS 70

//@ 965
BOOL SNOTOC::ReadTOC(SNOTOC::TOCType eType, const CRefString & sMPQFilename, const CRefString & sFilename,
                     SNOTOC::PerTOCCallback ptPerTOCCallback, SNOTOC::PerEntryCallback ptPerEntryCallback, void * pUserData)
{
//@ 974
    FileIO::File tFile(XLOCALE_GLOBAL);

    if (!tFile.Open(sFilename, sMPQFilename))
    {
        return FALSE;
    }
//@ 983
    uint32 uSize = (uint32)tFile.GetFileSize();
    uint64 uBytesRead;
    uint32 uHeaderSize = NUM_TOC_GROUPS * sizeof(uint32) * 3 + sizeof(uint32);

    uint8 * pbFileHeader = (uint8 *)SigmaMemoryNew(uHeaderSize, 4, NULL, FALSE);

    uBytesRead = 0;

    if (!tFile.ReadFile(pbFileHeader, uHeaderSize, &uBytesRead) || uBytesRead != uHeaderSize)
    {
        SigmaMemoryFree(pbFileHeader, NULL);
        return FALSE;
    }
//@ 1001
    uint32 * dwCountPerGroup = (uint32 *)pbFileHeader;
    uint32 * dwBaseOffsetPerGroup = (uint32 *)(pbFileHeader + NUM_TOC_GROUPS * sizeof(uint32));
    FullTOCDetail tFullTOCDetail;
    tFullTOCDetail.dwFullCountPerGroup = (uint32 *)(pbFileHeader + NUM_TOC_GROUPS * sizeof(uint32) * 2);
    tFullTOCDetail.dwHighestSNOHandle = *(uint32 *)(pbFileHeader + NUM_TOC_GROUPS * sizeof(uint32) * 3);
//@ 1018
    uint32 dwTotalDataSize = uSize - uHeaderSize;
//@ 1027
    if (ptPerTOCCallback)
    {
        uint32 dwNameSizePerGroup[NUM_TOC_GROUPS];
        for (int32 i = 0; i < NUM_TOC_GROUPS; i++)
        {
            uint32 dwGroupSize = ((i == NUM_TOC_GROUPS - 1) ? dwTotalDataSize : dwBaseOffsetPerGroup[i + 1])
                                 - dwBaseOffsetPerGroup[i];
            uint32 dwSNODiskEntrySize = dwCountPerGroup[i] * sizeof(SNODiskEntry);
            dwNameSizePerGroup[i] = dwGroupSize - dwSNODiskEntrySize;
        }
//@ 1037
        ptPerTOCCallback(eType, dwCountPerGroup, dwNameSizePerGroup, &tFullTOCDetail, pUserData);
    }

    if (ptPerEntryCallback)
    {
//@ 1042
        for (int32 i = 0; i < NUM_TOC_GROUPS; i++)
        {
            uint32 uDataSize = ((i + 1 < NUM_TOC_GROUPS) ? dwBaseOffsetPerGroup[i + 1] : dwTotalDataSize) - dwBaseOffsetPerGroup[i];
            uint8 * pbFileData = (uint8 *)SigmaMemoryNew(uDataSize, 4, NULL, FALSE);

            if (!tFile.ReadFile(pbFileData, uDataSize, &uBytesRead) || uBytesRead != uDataSize)
            {
                SigmaMemoryFree(pbFileData, NULL);
                SigmaMemoryFree(pbFileHeader, NULL);
                return FALSE;
            }
//@ 1055
            uint32 dwGroupCount = dwCountPerGroup[i];
            uint32 dwGroupNameOffset = dwGroupCount * sizeof(SNODiskEntry);
            char * szNames = (char *)(pbFileData + dwGroupNameOffset);
            SNODiskEntry * ptDiskEntries = (SNODiskEntry *)pbFileData;
            for (uint32 j = 0; j < dwGroupCount; j++)
            {
                SNODiskEntry & tEntry = ptDiskEntries[j];
//@ 1062
                DT_SNODISKENTRY->ByteswapInPlace(&tEntry);
                ptPerEntryCallback(eType, tEntry.snoGroup, tEntry.uGuid, szNames + tEntry.dwNameOffset, pUserData);
            }
//@ 1066
            SigmaMemoryFree(pbFileData, NULL);
        }
    }
//@ 1070
    SigmaMemoryFree(pbFileHeader, NULL);
    return TRUE;
}
