// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTeamShowcase/bdTeamShowcase.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdFileID;
class bdRemoteTaskManager;
class bdTeamMember;
class bdTeamShowcaseGetShowcasedFilesResponse;
class bdTeamShowcaseGetTeamIDsAndTagsResponse;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdTeamShowcase {  // size 0x20
    // vtable pointer
public:
    BD_CLASS_ALLOCATOR(bdTeamShowcase);
    virtual ~bdTeamShowcase();  // vtable slot 0
    bdRemoteTaskRef shareUserFileToTeamsShowcase(const bdFileID *);
    bdRemoteTaskRef featureFileInTeamsShowcase(const bdTeamID, const bdFileID *);
    bdRemoteTaskRef unfeatureFileFromTeamsShowcase(const bdTeamID, const bdFileID *);
    bdRemoteTaskRef favoriteFileInTeamsShowcase(const bdTeamID, const bdFileID *);
    bdRemoteTaskRef unfavoriteFileFromTeamsShowcase(const bdTeamID, const bdFileID *);
    bdRemoteTaskRef removeFileFromTeamsShowcase(const bdFileID *);
    bdRemoteTaskRef addTeamShowcaseBlockForUser(const bdTeamID, const bdUserID);
    bdRemoteTaskRef removeTeamShowcaseBlockForUser(const bdTeamID, const bdUserID);
    bdRemoteTaskRef getTeamShowcaseBlocksForTeam(const bdTeamID, bdTeamMember *, const bdUInt);
    bdRemoteTaskRef removeFileFromIndividualTeamShowcase(const bdFileID *const, const bdTeamID);
    bdRemoteTaskRef getShowcasedFilesByTeamIDAndFileTypeTag(const bdTeamID, const bdUInt16, bdTeamShowcaseGetShowcasedFilesResponse *);
    bdRemoteTaskRef getTeamIDsAndFileTypeTagsByFileID(const bdFileID *, bdTeamShowcaseGetTeamIDsAndTagsResponse *);
protected:
    bdTeamShowcase(bdRemoteTaskManager *const, const bdNChar8 *);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x08
    bdNChar8 m_context[16];  // 0x10
};
static_assert(sizeof(bdTeamShowcase) == 0x20, "bdTeamShowcase");


// Functions defined in this file (bodies are not in the DWARF):
//   line    44  void bdTeamShowcase::operator delete(void * p)  [inline]
//   line    44  void * bdTeamShowcase::operator new(size_t nbytes)  [inline]
