#include "X/Sigma/ErrorManager/ErrorManagerInspectorData.h"


ErrorManagerBlizzardErrorData::ErrorManagerBlizzardErrorData() {
    Init();
}

void ErrorManagerBlizzardErrorData::Init() {
    SigmaMemoryClear(szApplicationDirectory, sizeof(szApplicationDirectory));
    SigmaMemoryClear(szSummary, sizeof(szSummary));
    SigmaMemoryClear(szAssertion, sizeof(szAssertion));
    SigmaMemoryClear(szModules, sizeof(szModules));
    SigmaMemoryClear(szStackDigest, sizeof(szStackDigest));
    SigmaMemoryClear(szLocale, sizeof(szLocale));
    SigmaMemoryClear(szComments, sizeof(szComments));
    SigmaMemoryClear(szReopenCmdLine, sizeof(szReopenCmdLine));
    SigmaMemoryClear(szRepairCmdLine, sizeof(szRepairCmdLine));
    SigmaMemoryClear(aszAttachments, sizeof(aszAttachments));
    dwNumAttachments = 0;
    SigmaMemoryClear(szUserAssignment, sizeof(szUserAssignment));
    SigmaMemoryClear(szEmailAddress, sizeof(szEmailAddress));
    SigmaMemoryClear(szReportDirectory, sizeof(szReportDirectory));
    eSeverity = EMJS_HIGH;
    eMode = EMBEM_NORMAL;
    eIssueType = EMBEIT_EXCEPTION;
    SigmaMemoryClear(szBranch, sizeof(szBranch));
}






































































void ErrorManagerBlizzardErrorData::AddMetadata(const char * pszKey, const char * pszValue)


{
    blz::vector<ErrorManagerBlizzardErrorMetaData, blz::allocator<ErrorManagerBlizzardErrorMetaData> >::iterator it;

    for (it = aMetadata.begin(); it != aMetadata.end(); ++it) {

        if (strcmp(it->metadata_key.c_str(), pszKey) == 0) {

            it->metadata_value = pszValue;
            return;
        }
    }


    ErrorManagerBlizzardErrorMetaData tMetaData;
    tMetaData.metadata_key = pszKey;
    tMetaData.metadata_value = pszValue;
    aMetadata.push_back(tMetaData);
}
