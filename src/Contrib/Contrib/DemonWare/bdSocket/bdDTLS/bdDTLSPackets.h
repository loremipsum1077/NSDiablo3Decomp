// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdDTLS/bdDTLSPackets.h

#pragma once

#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

typedef bdUByte8 bdDTLSVersion;

// Defined in bdDTLSPackets.cpp:20. Not const: the DWARF type is plain bdDTLSVersion and the
// original keeps it in writable data.
extern bdDTLSVersion BD_DTLS_VERSION;

enum bdDTLSPacketTypes {
    BD_DTLS_INIT = 1,
    BD_DTLS_INIT_ACK = 2,
    BD_DTLS_COOKIE_ECHO = 3,
    BD_DTLS_COOKIE_ACK = 4,
    BD_DTLS_ERROR = 5,
    BD_DTLS_DATA = 6,
};
