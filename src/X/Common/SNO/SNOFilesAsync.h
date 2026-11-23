// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SNO/SNOFilesAsync.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XTemplate.h"

typedef int32 SNOFilesAsyncPriority;

typedef int32 SNOFilesAsyncPriority;

enum AsyncStatus {
    e_pending = 0,
    e_successful = 1,
    e_failed = 2,
    e_canceled = 3,
};

typedef POS AsyncRecordRef;

