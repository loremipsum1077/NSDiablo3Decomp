// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/ErrorManager/ErrorManagerInspectorData.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../UnicodeText.h"

enum {
    EMBEM_NORMAL = 0,
    EMBEM_SUMMARY_NO_CANCEL = 1,
    EMBEM_SUMMARY = 2,
    EMBEM_AUTO = 3,
};

typedef int32 ErrorManagerBlizzardErrorMode;

enum ErrorManagerJiraSeverity {
    EMJS_HIGH = 0,
    EMJS_MAJOR = 1,
    EMJS_MINOR = 2,
    EMJS_TRIVIAL = 3,
    EMJS_NONE = 4,
    EMJS_COUNT = 5,
};

enum ErrorManagerBlizzardErrorIssueType {
    EMBEIT_EXCEPTION = 0,
    EMBEIT_BUG = 1,
    EMBEIT_COUNT = 2,
};

struct ErrorManagerBlizzardErrorMetaData {  // size 0x50
    blz::string metadata_key;  // 0x00
    blz::string metadata_value;  // 0x28
};
static_assert(sizeof(ErrorManagerBlizzardErrorMetaData) == 0x50, "ErrorManagerBlizzardErrorMetaData");

struct ErrorManagerBlizzardErrorData {  // size 0x14d48
    ErrorManagerBlizzardErrorData();
    void Init();
    const char* GetIssueTypeString();
    void SetLocaleString(const XLocale);
    void AddMetadata(const char* pszKey, const char* pszValue);
    void SetBranchString(const char*);
    ErrorManagerBlizzardErrorMode eMode;  // 0x00
    ErrorManagerBlizzardErrorIssueType eIssueType;  // 0x04
    char szApplicationDirectory[4096];  // 0x08
    char szSummary[1024];  // 0x1008
    char szAssertion[8192];  // 0x1408
    char szModules[16384];  // 0x3408
    char szStackDigest[8192];  // 0x7408
    char szLocale[128];  // 0x9408
    char szComments[1024];  // 0x9488
    char aszAttachments[8][4096];  // 0x9888
    uint32 dwNumAttachments;  // 0x11888
    char szUserAssignment[128];  // 0x1188c
    char szEmailAddress[1024];  // 0x1190c
    char szReopenCmdLine[4096];  // 0x11d0c
    char szRepairCmdLine[4096];  // 0x12d0c
    ErrorManagerJiraSeverity eSeverity;  // 0x13d0c
    char szReportDirectory[4096];  // 0x13d10
    blz::vector<ErrorManagerBlizzardErrorMetaData, blz::allocator<ErrorManagerBlizzardErrorMetaData> > aMetadata;  // 0x14d10
    char szBranch[32];  // 0x14d28
};
static_assert(sizeof(ErrorManagerBlizzardErrorData) == 0x14d48, "ErrorManagerBlizzardErrorData");
