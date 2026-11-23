// Original file: X/Common/OnlineService/Guilds.cpp

#include "Guilds.h"

#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/DataStructures/XVarCollection.h"
#include "../../Sigma/DataStructures/XVariant.h"
#include "OnlineService.h"

// For SNONAME_NONE, cgplaceNull and the GfxVB.h normals, whose initializers the original runs in
// this file's static initializer.
static XVarScope s_scopeGuild("Guild", NULL);  // line 20
XVarUint32 s_varGuildMaxInvitesPerPlayer(s_scopeGuild, "MaxPlayerInvites", GUILD_ACCOUNT_INVITES_DEFAULT);  // line 21
XVarUint32 s_varGuildNumShards(s_scopeGuild, "NumShards", GUILD_NUMBER_SHARDS_DEFAULT);  // line 22

void RegisterGuildSyncedVars()  // line 25
{
    OnlineService::GetSyncronizedXVarCollection().Add(s_varGuildMaxInvitesPerPlayer, XVariant::MakeUint32(GUILD_ACCOUNT_INVITES_DEFAULT));
    OnlineService::GetSyncronizedXVarCollection().Add(s_varGuildNumShards, XVariant::MakeUint32(GUILD_NUMBER_SHARDS_DEFAULT));
}
