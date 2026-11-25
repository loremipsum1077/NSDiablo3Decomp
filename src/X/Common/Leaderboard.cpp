#include "BlizzardCore/Include/Blizzard/Time.h"
#include "X/Common/SNO/SNOName.h"
#include "X/Common/WorldPlace.h"
#include "X/Client/Gfx/GfxVB.h"
#include "X/Common/Leaderboard.h"
#include "X/Common/GlobalSNO.h"

typedef int32 GBID;  // X/Common/GameBalance.h
Blizzard::Time::Timestamp LeaderboardGetConquestTime(uint64 score);


int64 LeaderboardMakeRiftDemonwareRating(uint64 score) {
    return static_cast<int64>(score >> 1);
}

uint32 LeaderboardGetAchievementPoints(uint64 score) { return static_cast<uint32>(score >> 32); }

Blizzard::Time::Timestamp LeaderboardGetLastAchievementTime(uint64 score) { return Blizzard::Time::FromUnixTime(~static_cast<uint32>(score)); }

uint64 LeaderboardMakeConquestScore(Blizzard::Time::Timestamp ts) { return ~static_cast<uint64>(ts); }

Blizzard::Time::Timestamp LeaderboardGetConquestTime(uint64 score) { return ~score; }

extern "C" bool sub_7100517f80(LeaderboardScope eScope)
{
    return (static_cast<uint32>(eScope) & ~1u) == 8;
}

bool LeaderboardIsRift(uint64 leaderboardId)
{
    return (leaderboardId != LB_RIFT_PLACEHOLDER_9 && leaderboardId >= LB_RIFT_FIRST && leaderboardId <= LB_RIFT_LAST)
        || (leaderboardId >= 41 && leaderboardId <= 82);
}

bool LeaderboardIsChallengeRift(uint64_t leaderboardId)
{
    return leaderboardId - LB_CHALLENGE_RIFT_FIRST < 4;
}

// Lines 79-82.
static const uint64 LEVEL_SHIFT = 56;
static const uint64 SCORE_SHIFT = 32;
static const uint64 LEVEL_MAX = 0xff;
static const uint64 SCORE_MAX = 0xffffff;

// Line 84.
// On DWARF lines 84-99 (x13); locals and columns from the DWARF/line table (98: the two `|` and `<<` columns).
#line 84
uint64 LeaderboardMakeRiftScore(uint32 level, int32 millisecondsElapsed, uint32 unixTime)
{
    uint64 wideLevel = level;
    uint64 wideTimeToComplete = millisecondsElapsed;
    uint64 wideInvertedUnixTime = (uint64)(~unixTime);
    // Inferred: 90-91 leave no row (the clamp is the select at 98:61); DWARF gives wideTimeToComplete the constant SCORE_MAX.
    if (wideTimeToComplete > SCORE_MAX)
        wideTimeToComplete = SCORE_MAX;

    wideTimeToComplete = SCORE_MAX - wideTimeToComplete;

    if (wideLevel > LEVEL_MAX)
        wideLevel = LEVEL_MAX;

    return (wideLevel << LEVEL_SHIFT) | (wideTimeToComplete << SCORE_SHIFT) | wideInvertedUnixTime;
}
#line 47

// Line 113.
uint32 LeaderboardGetRiftScoreLevel(uint64 score)
{
    return (uint32)(score >> LEVEL_SHIFT);
}

// Line 118.
Blizzard::Time::Timestamp LeaderboardGetRiftScoreTimeToComplete(uint64 score)
{
    return (SCORE_MAX - ((score >> SCORE_SHIFT) & SCORE_MAX)) * 1000000;
}

// Line 129.
uint64 LeaderboardMakeAchievementPointScore(uint32 achievementPoints, uint32 unixTime)
{
    return ((uint64)achievementPoints << 32) | (uint32)~unixTime;
}

// Line 136.
int64 LeaderboardMakeAchievementDemonwareRating(uint64 score)
{
    return score;
}

// Line 157.
int64 LeaderboardMakeConquestDemonwareRating(uint64 score)
{
    return (uint32)~Blizzard::Time::ToUnixTime(LeaderboardGetConquestTime(score));
}

// Line 741.
bool LeaderboardClassGBIDToId(GBID gbid, LeaderboardId & into)
{
    if (gbid == GlobalGBIDGet(GLOBALGBID_PLAYER_DEMONHUNTER))
        into = LB_RIFT_SOLO_DEMONHUNTER;
    else if (gbid == GlobalGBIDGet(GLOBALGBID_PLAYER_BARBARIAN))
        into = LB_RIFT_SOLO_BARBARIAN;
    else if (gbid == GlobalGBIDGet(GLOBALGBID_PLAYER_WIZARD))
        into = LB_RIFT_SOLO_WIZARD;
    else if (gbid == GlobalGBIDGet(GLOBALGBID_PLAYER_WITCHDOCTOR))
        into = LB_RIFT_SOLO_WITCHDOCTOR;
    else if (gbid == GlobalGBIDGet(GLOBALGBID_PLAYER_MONK))
        into = LB_RIFT_SOLO_MONK;
    else if (gbid == GlobalGBIDGet(GLOBALGBID_PLAYER_NECROMANCER))
        into = LB_RIFT_SOLO_NECROMANCER;
    else if (gbid == GlobalGBIDGet(GLOBALGBID_PLAYER_CRUSADER))
        into = LB_RIFT_SOLO_CRUSADER;
    else
        return false;
    return true;
}

#include "X/Sigma/DataStructures/XVar.h"
#include "X/Sigma/DataStructures/XVarCollection.h"
#include "X/Common/OnlineService/OnlineService.h"
#include "X/Common/Protobuf/Leaderboard.pb.h"
#include "Contrib/Contrib/Zlib/zlib.h"
#line 24
static XVarScope s_scopeLeaderboard("Leaderboard", NULL);
static XVarBool s_varEnableLeaderboards(s_scopeLeaderboard, "EnableLeaderboards", true);
#line 53
void LeaderboardRegisterSyncedVars()
{
    OnlineService::GetSyncronizedXVarCollection().Add(s_varEnableLeaderboards, XVariant::MakeBool(true));
}
// LINKER-REMOVED: DWARF external, inlined into LeaderboardToDemonwareLeaderboardId at 908; out-of-line copy linker-removed.
// Inference: switch form from the original's two compare-and-branch tests at 203:5 (the keyword column).
#line 201
bool LeaderboardIsHardcore(LeaderboardScope scope)
{
    switch (scope)
    {
    case LP_HARDCORE:
    case LP_SEASONAL_HARDCORE:
        return true;
    default:
        return false;
    }
}
#line 786
blz::string LeaderboardCompressMessage(const google::protobuf::Message & msg, int compressionLevel)
{
#line 788
    blz::string serialized = msg.SerializeAsString();
#line 789
    uLong serializedSize = serialized.size();
#line 790
    uLongf destLen = compressBound(serializedSize);
#line 791
    blz::string compressed(destLen, '\0');
    if (compress2((Bytef *)&compressed[0], &destLen, (const Bytef *)serialized.c_str(), serializedSize, compressionLevel) == Z_OK)
    {
#line 795
        D3::Leaderboard::CompressedMetadata zmetadata;
        zmetadata.set_uncompressed_size(serializedSize);
        zmetadata.mutable_compressed_data()->assign(compressed, 0, destLen);
#line 799
        blz::string zmetadataBinary = zmetadata.SerializeAsString();
        if (zmetadataBinary.size() <= serialized.size())
        {
#line 803
            blz::string headerAdded;
            headerAdded.reserve(zmetadataBinary.size() + 1);
            headerAdded.push_back('\0');
            headerAdded.append(zmetadataBinary);
            return headerAdded;
        }
    }
    return serialized;
}
#line 815
bool LeaderboardDecompressMessage(const blz::string & compressed, google::protobuf::Message * msg)
{
    if (compressed.length() == 0 || compressed[0] != '\0')
        return msg->ParseFromString(compressed);
#line 819
    D3::Leaderboard::CompressedMetadata zmetadata;
    if (!zmetadata.ParseFromArray(&compressed[1], compressed.size() - 1))
        return false;
#line 826
    blz::string uncompressedMessage(zmetadata.uncompressed_size(), '\0');
#line 827
    uLongf outputSize = uncompressedMessage.size();
#line 828
    int status = uncompress((Bytef *)&uncompressedMessage[0], &outputSize, (const Bytef *)zmetadata.compressed_data().c_str(), zmetadata.compressed_data().size());
    if (status != Z_OK || outputSize != uncompressedMessage.size())
        return false;
    return msg->ParseFromString(uncompressedMessage);
}
#line 844
struct ConquestToLeaderboardId
{
    uint64 achievementId;
    uint32 dwLeaderboardId;
};
// The original twenty constant pairs at 0xe495b8, in original array order.
#line 848
static const ConquestToLeaderboardId sConquestToLeaderboardMapping[] = {
    { 0x443354f8e0b2ULL, 101 },
    { 0x44335495923eULL, 102 },
    { 0x443354544c0eULL, 103 },
    { 0x443354bc7df1ULL, 104 },
    { 0x443354a980cfULL, 105 },
    { 0x44335430a951ULL, 106 },
    { 0x4433544a52a0ULL, 107 },
    { 0x44335410e989ULL, 108 },
    { 0x443354e344c5ULL, 109 },
    { 0x44335487f23cULL, 110 },
    { 0x4433549f593fULL, 111 },
    { 0x44335461dbe4ULL, 112 },
    { 0x443354a39cffULL, 113 },
    { 0x44335437457dULL, 114 },
    { 0x443354062862ULL, 115 },
    { 0x443354856425ULL, 116 },
    { 0x4433545c3a04ULL, 117 },
    { 0x443354f55802ULL, 118 },
    { 0x4433544fa6eaULL, 119 },
    { 0x4433546b6bd9ULL, 120 },
};
// Inference: the rift range's lower bound (LB_NONE, exclusive) comes from the original's (id - 1) <= 12 test at 916:41;
// the enumerator spelling is a guess. Condition grouping follows the original partition and scope branches.
#line 876
uint32 LeaderboardToDemonwareLeaderboardId(uint64 leaderboardId, LeaderboardScope eScope, uint32 uPartition, bool bMetadata)
{
#line 881
    uint32 dwLeaderboardId = (uint32)leaderboardId;
    if (leaderboardId > LB_LAST_STATIC)
    {
#line 886
        bool bFound = false;
#line 887
        for (const ConquestToLeaderboardId & tMapping : sConquestToLeaderboardMapping)
        {
            if (tMapping.achievementId == leaderboardId)
            {
                dwLeaderboardId = tMapping.dwLeaderboardId;
                bFound = true;
                break;
            }
        }
        if (!bFound)
            return 0;
    }
    if (bMetadata)
        dwLeaderboardId += 500;
#line 908
    if (LeaderboardIsHardcore(eScope))
        dwLeaderboardId += 1000;
#line 914
    uint32 uWrappedPartition = 0;
    if (leaderboardId > LB_LAST_STATIC || leaderboardId == LB_ACHIEVEMENT_POINTS || (leaderboardId > LB_NONE && leaderboardId <= LB_RIFT_LAST))
        uWrappedPartition = uPartition % 2;
    else if (LeaderboardIsChallengeRift(leaderboardId))
        uWrappedPartition = uPartition % 2;
    return dwLeaderboardId + uWrappedPartition * 100000;
}
#line 946
uint64 LeaderboardGetLeaderboardIdForConsoleMetadata(uint64 leaderboardId, const D3::Leaderboard::Score & tScore)
{
    if (leaderboardId == LB_RIFT_TEAM_1)
        LeaderboardClassGBIDToId(tScore.metadata().team_member(0).hero_gbid_class(), leaderboardId);
    return leaderboardId;
}
