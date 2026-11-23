// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/OnlineService/Guilds.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

typedef uint64 GuildId;

typedef uint32 GuildRankId;

enum GuildMemberPermission {
    GMP_FIRST_PERMISSION = 0,
    GMP_GUILD_CHAT_LISTEN = 0,
    GMP_GUILD_CHAT_SPEAK = 1,
    GMP_OFFICER_CHAT_LISTEN = 2,
    GMP_OFFICER_CHAT_SPEAK = 3,
    GMP_PROMOTE = 4,
    GMP_DEMOTE = 5,
    GMP_INVITE_MEMBER = 6,
    GMP_REMOVE_MEMBER = 7,
    GMP_SET_MOTD = 8,
    GMP_SET_NEWS = 9,
    GMP_EDIT_NOTE_OTHER = 10,
    GMP_EDIT_NOTE_SELF = 11,
    GMP_SEE_NOTES = 12,
    GMP_MODIFY_RANKS = 13,
    GMP_MOVE_RANK = 14,
    GMP_SET_DESCRIPTION = 15,
    GMP_GET_INVITES = 16,
    GMP_CANCEL_INVITE = 17,
    GMP_SET_SEARCH_SETTINGS = 18,
    GMP_SET_PRIVACY = 19,
    GMP_COUNT_PERMISSIONS = 20,
    GMP_OFFICER_PERMISSION_MODIFY_MASK = 1032154,
    GMP_OFFICER_DEFAULT_PERMISSIONS = 212943,
    GMP_MEMBER_PERMISSION_MODIFY_MASK = 4930,
    GMP_MEMBER_DEFAULT_PERMISSIONS = 3,
};

enum GuildFlags {
    GUILDFLAG_INVITE_REQUIRED = 0,
    GUILDFLAG_DISPLAYS_IN_SEARCH = 1,
    GUILDFLAG_ALLOW_JOIN_REQUEST = 2,
    GUILDFLAG_COUNT = 3,
    GUILDFLAG_FIRST = 0,
};

typedef GuildId GroupId;

static const uint32 GUILD_ACCOUNT_INVITES_DEFAULT = 5;  // line 52
static const uint32 GUILD_NUMBER_SHARDS_DEFAULT = 10;  // line 53


// Variables defined in this file:
//   line    29  static const GuildId INVALID_GUILD_ID
//   line    35  static const int GUILD_MANAGER_RANK
//   line    36  static const int GUILD_OFFICER_RANK
//   line    39  static const uint32 GUILD_NAME_MIN_UTF8_CHARS
//   line    40  static const uint32 GUILD_NAME_MAX_UTF8_CHARS
//   line    42  static const uint32 GUILD_TAG_MIN_UTF8_CHARS
//   line    43  static const uint32 GUILD_TAG_MAX_UTF8_CHARS
//   line    52  static const uint32 GUILD_ACCOUNT_INVITES_DEFAULT
//   line    53  static const uint32 GUILD_NUMBER_SHARDS_DEFAULT
//   line   205  static const GroupId INVALID_GROUP_ID
