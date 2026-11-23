// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Partition.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

enum D3AccountPartition {
    ACCOUNT_PARTITION_ALL = -1,
    ACCOUNT_PARTITION_NORMAL = 0,
    ACCOUNT_PARTITION_HARDCORE = 1,
    ACCOUNT_PARTITION_SEASON_NORMAL = 2,
    ACCOUNT_PARTITION_SEASON_HARDCORE = 3,
    ACCOUNT_PARTITION_COUNT = 4,
    ACCOUNT_PARTITION_FIRST = 0,
};

// line 64. Body not in the DWARF; reconstructed from its inlined copies (PresenceManager.cpp UpdateUser).
inline D3AccountPartition GetPartition(BOOL isHardcore, BOOL isSeasonal)
{
    if (isSeasonal)
    {
        return isHardcore ? ACCOUNT_PARTITION_SEASON_HARDCORE : ACCOUNT_PARTITION_SEASON_NORMAL;
    }
    return isHardcore ? ACCOUNT_PARTITION_HARDCORE : ACCOUNT_PARTITION_NORMAL;
}

// Functions defined in this file (bodies are not in the DWARF):
//   line    24  bool IsPartitionNormal(D3AccountPartition p)  [inlined into 1 functions]
//   line    29  bool IsPartitionHardcore(D3AccountPartition p)  [inlined into 2 functions]
//   line    59  bool IsPartitionSeasonal(D3AccountPartition p)  [inlined into 3 functions]
//   line    64  D3AccountPartition GetPartition(BOOL isHardcore, BOOL isSeasonal)  [inlined into 10 functions]

// line 24. Body not in the DWARF; moved from its owner-local copy in AccountMigration.cpp
// (Migrate_113_114 inlines it from Partition.h:24).
inline bool IsPartitionNormal(D3AccountPartition p)
{
    return p == ACCOUNT_PARTITION_NORMAL || p == ACCOUNT_PARTITION_SEASON_NORMAL;
}

// line 29. Body not in the DWARF; reconstructed from its inlined copy in ConsolePlayerResolver.cpp
// LoadFromParty (original computes (p | 2) == 3).
inline bool IsPartitionHardcore(D3AccountPartition p)
{
    return p == ACCOUNT_PARTITION_HARDCORE || p == ACCOUNT_PARTITION_SEASON_HARDCORE;
}

// line 59. Body not in the DWARF; reconstructed from its inlined copy in ConsolePlayerResolver.cpp
// LoadFromParty (original computes (p | 1) == 3).
inline bool IsPartitionSeasonal(D3AccountPartition p)
{
    return p == ACCOUNT_PARTITION_SEASON_NORMAL || p == ACCOUNT_PARTITION_SEASON_HARDCORE;
}
