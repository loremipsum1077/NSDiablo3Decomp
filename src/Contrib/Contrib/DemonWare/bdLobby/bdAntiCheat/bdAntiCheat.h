// Partial declaration from bdAntiCheat.h (DWARF): only what bdLobbyService.cpp uses so far.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdAntiCheat.h
#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdRemoteTaskManager;

class bdAntiCheat {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdAntiCheat);
    enum bdAntiCheatReason {
        BD_REASON_UNKNOWN = 0,
        BD_REASON_REPORT_MISSING = 1,
    };
    bdAntiCheat(bdRemoteTaskManager *const);
    ~bdAntiCheat();
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdAntiCheat) == 0x8, "bdAntiCheat");
