// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTeams/bdTeams.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdStatsInfo;
class bdTeamApplication;
class bdTeamApplicationWithTeamID;
class bdTeamAutoJoinInfo;
class bdTeamBannedMember;
class bdTeamFailedHandleMembership;
class bdTeamFailedProposal;
class bdTeamInfo;
class bdTeamInfoAndCounts;
class bdTeamInfoAndType;
class bdTeamInfoAndTypeAndCounts;
class bdTeamInfoV2;
class bdTeamInfoWithAutoJoin;
class bdTeamMember;
class bdTeamMemberAndPrivileges;
class bdTeamMemberProfile;
class bdTeamMemberWithPrivileges;
class bdTeamProfile;
class bdTeamProfileSearchParams;
class bdTeamProposal;
class bdTeamSearchFilter;
class bdTeamSearchOrdering;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdTeams {  // size 0x20
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdTeams);
    enum BD_TEAM_APPLICATION_TYPES {
        BD_TEAM_PENDING_APPLICATION = 0,
        BD_TEAM_REJECTED_APPLICATION = 1,
        BD_TEAM_WITHDRAWN_APPLICATION = 2,
    };
    virtual ~bdTeams();  // vtable slot 0
    bdRemoteTaskRef createTeam(const bdNChar8 *const, bdTeamInfo *const, const bdUserID);
    bdRemoteTaskRef createTeamV2(const bdNChar8 *const, bdTeamInfoV2 *const, const bdUserID, const bdBool);
    bdRemoteTaskRef createTeamWithProfiles(const bdNChar8 *const, bdTeamProfile *, bdTeamProfile *, bdTeamInfo *const, const bdUserID);
    bdRemoteTaskRef createTeamWithProfilesV2(const bdNChar8 *const, bdTeamProfile *, bdTeamProfile *, bdTeamInfoV2 *const, const bdUserID, const bdBool);
    bdRemoteTaskRef createTeamWithProfilesAndTeamType(const bdNChar8 *const, bdTeamProfile *, bdTeamProfile *, bdTeamInfoAndType *const, const bdUserID, const bdUByte8 *const);
    bdRemoteTaskRef createTeamWithProfilesAndTeamTypeV2(const bdNChar8 *const, bdTeamProfile *, bdTeamProfile *, bdTeamInfoV2 *const, const bdUserID, const bdUByte8 *const, const bdBool);
    bdRemoteTaskRef getTeamInfo(const bdNChar8 *const, bdTeamInfo *const, const bdUserID);
    bdRemoteTaskRef getTeamInfoV2(const bdNChar8 *const, bdTeamInfoV2 *const, const bdUserID);
    bdRemoteTaskRef getTeamInfoWithTeamType(const bdNChar8 *const, bdTeamInfoAndType *const, const bdUserID);
    bdRemoteTaskRef getTeamInfoWithAutoJoin(const bdNChar8 *const, bdTeamInfoWithAutoJoin *const, const bdUserID);
    bdRemoteTaskRef updateTeamName(const bdTeamID, const bdNChar8 *const, const bdUserID);
    bdRemoteTaskRef updateTeamType(const bdTeamID, const bdUByte8, const bdUserID);
    bdRemoteTaskRef promoteMember(const bdTeamID, const bdUserID);
    bdRemoteTaskRef promoteMember(const bdTeamID, const bdUserID, const bdUInt32);
    bdRemoteTaskRef promoteMemberToOwner(const bdTeamID, const bdUserID);
    bdRemoteTaskRef promoteMemberToOwnerV2(const bdTeamID, const bdUserID, const bdBool, const bdUInt32);
    bdRemoteTaskRef demoteMember(const bdTeamID, const bdUserID);
    bdRemoteTaskRef kickMember(const bdTeamID, const bdUserID);
    bdRemoteTaskRef leaveTeam(const bdTeamID, const bdUserID);
    bdRemoteTaskRef proposeMembership(const bdTeamID, const bdUserID, const void *const, const bdUInt);
    bdRemoteTaskRef proposeMultipleMemberships(bdTeamFailedProposal *, const bdTeamID, const bdUInt32, const bdUserID *, const void *const, const bdUInt);
    bdRemoteTaskRef withdrawProposal(const bdUserID, const bdTeamID);
    bdRemoteTaskRef rejectMembership(const bdTeamID, const bdUserID);
    bdRemoteTaskRef rejectMemberships(const bdTeamID *, bdTeamFailedHandleMembership *, const bdUInt, const bdUserID);
    bdRemoteTaskRef acceptMembership(const bdTeamID, const bdUserID);
    bdRemoteTaskRef acceptMemberships(const bdTeamID *, bdTeamFailedHandleMembership *, const bdUInt32, const bdUserID);
    bdRemoteTaskRef getOutgoingProposals(bdTeamProposal *, const bdUInt, const bdUserID, const bdUInt, const bdOnlineUserID *, const bdUInt);
    bdRemoteTaskRef getIncomingProposals(bdTeamProposal *, const bdUInt, const bdUserID);
    bdRemoteTaskRef getIncomingProposalsV2(bdTeamProposal *, const bdUInt, const bdUserID, const bdUInt);
    bdRemoteTaskRef getPublicProfiles(const bdTeamID *, bdTeamProfile *, const bdUInt);
    bdRemoteTaskRef getPublicProfilesV2(const bdTeamID *, bdTeamProfile *, const bdUInt);
    bdRemoteTaskRef getPrivateProfile(const bdTeamID, bdTeamProfile *const);
    bdRemoteTaskRef setPublicProfile(const bdTeamID, bdTeamProfile *);
    bdRemoteTaskRef setPrivateProfile(const bdTeamID, bdTeamProfile *);
    bdRemoteTaskRef getPublicMemberProfiles(const bdTeamID, bdTeamMemberProfile *, const bdUInt);
    bdRemoteTaskRef getPrivateMemberProfiles(const bdTeamID, bdTeamMemberProfile *, const bdUInt);
    bdRemoteTaskRef setPublicMemberProfile(const bdTeamID, bdTeamMemberProfile *);
    bdRemoteTaskRef setPrivateMemberProfile(const bdTeamID, bdTeamMemberProfile *);
    bdRemoteTaskRef getMemberships(bdTeamInfo *, const bdUInt, const bdUInt32, const bdUserID);
    bdRemoteTaskRef getMembershipsV2(bdTeamInfoV2 *, const bdUInt, const bdUInt32, const bdUserID);
    bdRemoteTaskRef getMembershipsWithCounts(bdTeamInfoAndCounts *, const bdUInt, const bdUInt32, const bdUserID);
    bdRemoteTaskRef getMembershipsWithCountsAndTeamType(bdTeamInfoAndTypeAndCounts *, const bdUInt, const bdUInt32, const bdUserID);
    bdRemoteTaskRef getMembershipsUser(const bdUserID, bdTeamInfo *, const bdUInt, const bdUInt32);
    bdRemoteTaskRef getMembers(const bdTeamID, bdTeamMember *, const bdUInt, const bdUInt32);
    bdRemoteTaskRef getMembersV2(const bdTeamID, bdTeamMember *, const bdUInt, const bdUInt32, const bdBool);
    bdRemoteTaskRef getMembersAndPrivileges(const bdTeamID, bdTeamMemberAndPrivileges *, const bdUInt, const bdUInt32);
    bdRemoteTaskRef getMembersWithPrivilegesByRole(const bdTeamID, const bdUByte8, bdTeamMemberWithPrivileges *, const bdUInt, const bdUInt32, const bdBool);
    bdRemoteTaskRef getMembersByOnlineStatus(const bdTeamID, const bdBool, bdTeamMember *, const bdUInt, const bdUInt32);
    bdRemoteTaskRef setOnline(const bdUserID);
    bdRemoteTaskRef sendInstantMessage(const bdUserID, const void *const, const bdUInt);
    bdRemoteTaskRef setRichPresence(const bdTeamID, const void *const, const bdUInt);
    bdRemoteTaskRef sendInstantMessageToTeam(const bdTeamID, const void *const, const bdUInt);
    bdRemoteTaskRef searchPublicTeamProfiles(const bdUInt32, const bdUInt32, const bdTeamProfileSearchParams *const, bdTeamProfile *);
    bdRemoteTaskRef addApplication(const bdTeamID, const bdTeamID, const bdUserID, const void *const, const bdUInt);
    bdRemoteTaskRef getApplicationsByTeam(const bdTeamID, bdTeamApplication *, const bdUInt, const bdUInt32, const bdBool, const bdBool, const bdBool);
    bdRemoteTaskRef getApplicationsByTeamPaginated(const bdTeamID *, const bdUInt, bdTeamApplicationWithTeamID *, const bdUInt, const bdUInt32, const bdBool, const bdBool, const bdBool);
    bdRemoteTaskRef acceptApplication(const bdTeamID, const bdUserID);
    bdRemoteTaskRef autoJoinTeam(const bdTeamID, const bdUserID);
    bdRemoteTaskRef rejectApplication(const bdTeamID, const bdUserID);
    bdRemoteTaskRef blockApplication(const bdTeamID, const bdUserID, const bdUInt32);
    bdRemoteTaskRef unblockApplication(const bdTeamID, const bdUserID);
    bdRemoteTaskRef banMember(const bdTeamID, const bdUserID, const void *const, const bdUInt);
    bdRemoteTaskRef unbanMember(const bdTeamID, const bdUserID);
    bdRemoteTaskRef getBannedMembers(const bdTeamID, bdTeamBannedMember *, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef searchTeamsV2(bdTeamInfoV2 *, const bdUserID, const bdUInt, const bdUInt, const bdUInt, const bdUInt64 *, const bdTeamSearchFilter *, const bdTeamSearchFilter *, const bdUInt32, const bdUInt32, const bdNChar8 *const, const bdNChar8 *const, const bdBool);
    bdRemoteTaskRef searchTeamsV3(bdTeamInfoV2 *, const bdUserID, const bdUInt64 *, const bdUInt, const bdTeamSearchFilter *, const bdUInt, const bdTeamSearchFilter *, const bdUInt, const bdTeamSearchOrdering *, const bdUInt, const bdUInt32, const bdUInt32, const bdBool);
    bdRemoteTaskRef setTeamAutoJoin(const bdTeamID, bdBool);
    bdRemoteTaskRef getTeamAutoJoin(const bdTeamID, bdTeamAutoJoinInfo *const);
    bdRemoteTaskRef setTeamLeaderboard(const bdTeamID, const bdLeaderBoardID);
    bdRemoteTaskRef deleteTeamLeaderboard(const bdTeamID);
    bdRemoteTaskRef readTeamLeaderboard(const bdTeamID, const bdUInt64, bdStatsInfo *, const bdUInt32);
    bdRemoteTaskRef getMostPopularTeams(bdTeamInfoWithAutoJoin *, const bdUInt32, const bdUInt32);
    bdRemoteTaskRef getTeamMembersFromUserList(const bdTeamID, bdUserID *, const bdUInt32, bdTeamMember *);
protected:
    bdTeams(bdRemoteTaskManager *const, const bdNChar8 *);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x08
    bdNChar8 m_context[16];  // 0x10
};
static_assert(sizeof(bdTeams) == 0x20, "bdTeams");


// Functions defined in this file (bodies are not in the DWARF):
//   line   159  void bdTeams::operator delete(void * p)  [inline]
//   line   159  void * bdTeams::operator new(size_t nbytes)  [inline]
