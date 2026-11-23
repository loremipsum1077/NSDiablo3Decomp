// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRelayService/bdRelayService.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRelayCredentials;
class bdRemoteTaskManager;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

enum bdRelayServiceConnTypeHint {
    BD_RELAY_SERVICE_CONN_UDP = 1,
    BD_RELAY_SERVICE_CONN_XLSP = 2,
};

enum bdRelayServicePlatform {
    BD_RELAY_SERVICE_PLATFORM_PC = 1,
    BD_RELAY_SERVICE_PLATFORM_PS3 = 2,
    BD_RELAY_SERVICE_PLATFORM_IPHONE = 3,
    BD_RELAY_SERVICE_PLATFORM_ANDROID = 4,
    BD_RELAY_SERVICE_PLATFORM_XENON = 5,
    BD_RELAY_SERVICE_PLATFORM_PS4 = 6,
    BD_RELAY_SERVICE_PLATFORM_XBOXONE = 7,
    BD_RELAY_SERVICE_PLATFORM_WII = 8,
    BD_RELAY_SERVICE_PLATFORM_WIIU = 9,
    BD_RELAY_SERVICE_PLATFORM_WP8 = 10,
    BD_RELAY_SERVICE_PLATFORM_UNIX = 11,
    BD_RELAY_SERVICE_PLATFORM_OSX = 12,
    BD_RELAY_SERVICE_PLATFORM_UNKNOWN = 255,
};

class bdRelayService {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdRelayService);
    bdRelayService(bdRemoteTaskManager *const);
    ~bdRelayService();
    bdRemoteTaskRef getCredentials(bdRelayCredentials *, const bdUInt32, bdRelayServiceConnTypeHint, bdRelayServicePlatform);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
};
static_assert(sizeof(bdRelayService) == 0x8, "bdRelayService");


// Functions defined in this file (bodies are not in the DWARF):
//   line    56  void bdRelayService::operator delete(void * p)  [inlined into 1 functions]
//   line    56  void * bdRelayService::operator new(size_t nbytes)  [inline]
