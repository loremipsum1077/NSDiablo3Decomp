// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SNO/SNOTOC.h

// Same arrangement as SNOName.h: SNOFiles.h (which declares SNO and SNOGroup, and whose
// SNOEntry uses SNOTOC::TOCType) includes this header part-way through, and the include below
// is outside the guard so that including SNOTOC.h first also works.
#include "SNOFiles.h"

#ifndef X_COMMON_SNO_SNOTOC_H
#define X_COMMON_SNO_SNOTOC_H

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "SNOName.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

class CRefString;

typedef blz::vector<SNOName, blz::allocator<SNOName> > SNONameArray;

typedef XPooledList<SNOName, SNOName> PooledSNONameList;

struct FullTOCDetail {  // size 0x10
    uint32 * dwFullCountPerGroup;  // 0x00
    uint32 dwHighestSNOHandle;  // 0x08
};
static_assert(sizeof(FullTOCDetail) == 0x10, "FullTOCDetail");

class SNOTOC {  // size 0x84
public:  // TOCType is used by SNOEntry::SetName
    enum TOCType {
        TT_CORE = 0,
        TT_SERVER = 1,
        TT_HOTFIX = 2,
        TT_COUNT = 3,
    };
    typedef void (*PerEntryCallback)(SNOTOC::TOCType, SNOGroup, SNO, CHAR *, void *);
    typedef void (*PerTOCCallback)(SNOTOC::TOCType, uint32 *, uint32 *, FullTOCDetail *, void *);
public:
    // SNOTOC.h:50. The body is not in the DWARF; gtTOC's static initializer shows the copy and the flag.
    SNOTOC(LPCSTR szFilename, const BOOL fFullAssetList) {
        Blizzard::String::Copy(m_szFilename, szFilename, sizeof(m_szFilename));
        m_fFullAssetList = fFullAssetList;
    }
    BOOL ReadIntoFileTable();
    BOOL Read(SNONameArray &);
    BOOL ApplyHotfix(const char * szHotfixName);
    LPCSTR GetPartialFilename();
    CRefString GetLooseFilename(SNOTOC::TOCType eType) const;
    CRefString GetMPQFilename(SNOTOC::TOCType eType) const;
    BOOL ReadAndDump(const CRefString &);
    BOOL ReadAndDumpSingle(const CRefString &, SNOTOC::TOCType);
private:
    BOOL ReadSingle(SNONameArray & listUsedSNOs, SNOTOC::TOCType eType);
    BOOL Read(SNOTOC::PerTOCCallback ptPerTOCCallback, SNOTOC::PerEntryCallback ptPerEntryCallback, void * pUserData);
    BOOL ReadSingle(SNOTOC::TOCType eType, SNOTOC::PerTOCCallback ptPerTOCCallback, SNOTOC::PerEntryCallback ptPerEntryCallback, void * pUserData);
    BOOL ReadTOC(SNOTOC::TOCType eType, const CRefString & sMPQFilename, const CRefString & sFilename, SNOTOC::PerTOCCallback ptPerTOCCallback, SNOTOC::PerEntryCallback ptPerEntryCallback, void * pUserData);
    CHAR m_szFilename[128];  // 0x00
    BOOL m_fFullAssetList;  // 0x80
};
static_assert(sizeof(SNOTOC) == 0x84, "SNOTOC");

#endif  // X_COMMON_SNO_SNOTOC_H


// Functions defined in this file (bodies are not in the DWARF):
//   line    50  SNOTOC::SNOTOC(LPCSTR szFilename, const BOOL fFullAssetList)  [inline]
