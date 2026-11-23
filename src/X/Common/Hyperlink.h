// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Hyperlink.h
// Subset: the types and functions X/Client/Font/FormattedText.cpp uses. The original also
// includes OnlineService/Achievements.h, which is not adopted yet.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "OnlineService/OnlineService.h"

class CRefString;
struct SNOName;

typedef int32 FontSpecifier;

enum HyperlinkType {
    HLT_NONE = 0,
    HLT_ITEM = 1,
    HLT_ONLINE_USER = 2,
    HLT_ONLINE_USER_LEGACY = 3,
    HLT_ACHIEVEMENT = 4,
    HLT_ACHIEVEMENT_LEGACY = 5,
    HLT_SNO = 6,
    HLT_SNOSCRIPT = 7,
    HLT_URL = 8,
    HLT_CHAT_CHANNEL = 9,
    HLT_CHAT_CHANNEL_LEGACY = 10,
    HLT_CHAT_INVITE = 11,
    HLT_CLAN_CHAT = 12,
    HLT_GROUP_CHAT = 13,
    HLT_STORE_PRODUCT = 14,
    HLT_COUNT = 15,
};

namespace OnlineService {
typedef uint64 AchievementId;

typedef uint64 InvitationId;

}  // namespace OnlineService
typedef uint64 GroupId;

// line 64: the implicit constructor.
struct HyperlinkAchievementData {  // size 0x18
    OnlineService::AchievementId tAchievementId;  // 0x00
    OnlineService::GameAccountHandle tGameAccount;  // 0x08
};
static_assert(sizeof(HyperlinkAchievementData) == 0x18, "HyperlinkAchievementData");

// X/Common/Hyperlink.cpp
HyperlinkType HyperlinkValidate(const char *& pszString);
HyperlinkType HyperlinkParseType(const char *& pszString, BOOL bAdvanceStringPointer);
BOOL HyperlinkAdvancePastHyperlink(const char *& pszString);
BOOL HyperlinkGetPrettyName(char * pszPrettyName, const char *& pszString, int32 nPrettyNameBufSize, BOOL bRemoveBracketsAndColorCoding);
BOOL CHyperlinkGetAchievementData(LPCSTR szHyperLink, HyperlinkAchievementData & tHyperlinkAchievementData);
BOOL CHyperlinkGetItemName(LPCSTR szHyperLink, CRefString & sItemName, BOOL fIncludeBrackets);
// X/Common/Hyperlink.cpp:2735 [0x7100455a30].
CRefString HyperlinkCreateFromSNO(const SNOName & snoname, LPCSTR szLinkName);
