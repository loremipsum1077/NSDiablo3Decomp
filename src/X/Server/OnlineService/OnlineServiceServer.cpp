// Original file: X/Server/OnlineService/OnlineServiceServer.cpp
// Only the functions the shipped binary keeps are written; every other line is blank so the
// written ones sit on their DWARF lines (cu 0x56aea09).
#include "OnlineServiceServer.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Contrib/Contrib/blz/include/blz/iosfwd.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../Common/OnlineService/OnlineServiceErrorCodes.h"
#include <ctype.h>
#include <stdlib.h>









namespace OnlineServiceServer {
















































































































































































































































































































































































































































































































































































































































































































































































































































































































BOOL StringToTimeRec(const blz::string & originalTimeString, blz::string & errMsg, Blizzard::Time::TimeRec & tr)
{

    const blz::string formatString = "YYYY-MM-DDTHH:mm";
    const blz::string newFormatString = "YYYY-MM-DDTHH:mm:SS.%%%Z";
    blz::stringstream errStream;
    if(originalTimeString.length() != formatString.length() && originalTimeString.length() != newFormatString.length())
    {
        errMsg = "Incorrect length";
        return FALSE;
    }

    blz::string timeString = originalTimeString;

    if(timeString.length() == formatString.length())
    {
        timeString += ":00.000Z";
    }

    blz::string dateTime = "YYYYMMDDHHmmSS000";
    uint32 j = 0;
    for(uint32 i=0; i<timeString.length(); i++)
    {
        if(isdigit(timeString[i]))
        {
            if(j >= dateTime.length())
            {
                errMsg = "Too many digits";
                return FALSE;
            }
            dateTime[j] = timeString[i];
            j++;
        }
    }
    if(j < dateTime.length())
    {
        errMsg = "Too few digits";
        return FALSE;
    }

    tr.year = atoi(dateTime.substr(0, 4).c_str());
    tr.month = atoi(dateTime.substr(4, 2).c_str());
    tr.day = atoi(dateTime.substr(6, 2).c_str());
    tr.hour = atoi(dateTime.substr(8, 2).c_str());
    tr.minute = atoi(dateTime.substr(10, 2).c_str());
    tr.second = atoi(dateTime.substr(12, 2).c_str());
    tr.nanosecond = 0;

    if(tr.month > 12)
    {
        errMsg = "Months only valid up to 12";
        return FALSE;
    }

    if(tr.day > 31)
    {
        errMsg = "There are no months with more than 31 days";
        return FALSE;
    }

    return TRUE;
}




License::License()

    : id(0), expireTime(0), transactionId(0), consumed(false), has_consumed(false), revoked(false)
    // instanceId's store is attributed to line 974 (line table 0x71006d58a0); lines 970-973 hold no code.



    , instanceId(0)
{

}





























































































































void _AddLicense(
    blz::unordered_map<OnlineService::ContentLicenseId, OnlineService::ContentLicense> & contentMap,
    OnlineService::ContentLicenseId licenseId,
    uint32 dwLicensePropertyFlags,
    Blizzard::Time::Timestamp expireTime,
    bool bStackable)
{
    OnlineService::ContentLicense & tLicense = contentMap[licenseId];

    // Column 29 on line 1115 puts this store one indent level deeper than line 1117; any
    // enclosing condition folded away (every caller passes expireTime 0), so it is written plainly.

    tLicense.expireTime = expireTime;

    tLicense.licenseId = licenseId;
    if (dwLicensePropertyFlags != 0 && (tLicense.quantity == 0 || tLicense.propertyFlags != 0))
    {

        tLicense.propertyFlags |= dwLicensePropertyFlags;
    }
    else if (dwLicensePropertyFlags == 0)
    {
        // Line 1123 has no row: jump threading folds this test. A plain `else` placed the loads differently (try_file 400/488 vs 408/496 bytes).
        tLicense.propertyFlags = 0;
    }

    tLicense.quantity = bStackable ? (tLicense.quantity+1) : 1;


}

void _AddRoSLicenseBundle(
    blz::unordered_map<OnlineService::ContentLicenseId, OnlineService::ContentLicense> & contentMap,
    uint32 dwLicensePropertyFlags,
    bool & bAddedBundle)
{
    if (!bAddedBundle && !OnlineService::IsFreeToPlay())
    {
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_LEVEL_70, dwLicensePropertyFlags, 0, false);
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_CLASS_CRUSADER, dwLicensePropertyFlags, 0, false);
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_ADVMODE_ACTV, dwLicensePropertyFlags, 0, false);
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_CHARACTER_SLOTS_PLUS_2, dwLicensePropertyFlags, 0, true);
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_CAN_PURCHASE_STASH_TABS_PLUS_2, dwLicensePropertyFlags, 0, true);
    }
    bAddedBundle = true;
}



void _AddClassicLicenseBundle(
    blz::unordered_map<OnlineService::ContentLicenseId, OnlineService::ContentLicense> & contentMap,
    uint32 dwLicensePropertyFlags,
    bool & bAddedBundle)
{
    if (!bAddedBundle && !OnlineService::IsFreeToPlay())
    {
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_LEVEL_60, dwLicensePropertyFlags, 0, false);
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_ACT1_ACT4, dwLicensePropertyFlags, 0, false);
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_CHARACTER_SLOTS_PLUS_6, dwLicensePropertyFlags, 0, true);
        _AddLicense(contentMap, OnlineService::CONTENT_LICENSE_STASH_TABS_PLUS_3, dwLicensePropertyFlags, 0, true);
    }
    bAddedBundle = true;
}



OnlineService::ErrorCode ComposeContentLicenses(
    const OnlineServiceServer::LicenseArray & tLicenses,
    OnlineService::ContentLicenseArray & tContentLicenses)
{
    blz::unordered_map<OnlineService::ContentLicenseId, OnlineService::ContentLicense> contentMap;

    bool bAddedClassicLicenseBundle = false;
    bool bAddedRoSLicenseBundle = false;



    _AddRoSLicenseBundle(contentMap, 0, bAddedRoSLicenseBundle);
    _AddClassicLicenseBundle(contentMap, 0, bAddedClassicLicenseBundle);
    // Lines 1182-1324 leave no code and no line rows in the shipped binary (tLicenses is unused there).














































































































































    tContentLicenses.clear();
    for (auto it = contentMap.begin(); it != contentMap.end(); ++it)
    {
        tContentLicenses.push_back(it->second);
    }
    return OnlineService::ERROR_GLOBAL_SUCCESS;
}






























































































Blizzard::Time::Timestamp GetCurrentTime()
{

    return Blizzard::Time::GetTimestamp();
}





Blizzard::Time::UnixTime64 GetCurrentUnixTime()
{

    return Blizzard::Time::ToUnixTime64(Blizzard::Time::GetTimestamp());
}



















}  // namespace OnlineServiceServer
