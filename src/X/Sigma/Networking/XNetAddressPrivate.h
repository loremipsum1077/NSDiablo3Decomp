// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/Networking/XNetAddressPrivate.h

#pragma once

#include "../../../NintendoSDK/Include/nn/socket/netinet/in.h"

typedef sockaddr_in SOCKADDR_IN;

struct XNetAddressInternal {  // size 0x10
    SOCKADDR_IN m_sockAddr;  // 0x00
};
static_assert(sizeof(XNetAddressInternal) == 0x10, "XNetAddressInternal");

