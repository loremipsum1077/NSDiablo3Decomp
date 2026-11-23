// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/socket/netinet/in.h

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../sys/socket.h"

enum {
    IPPROTO_IP = 0,
    IPPROTO_ICMP = 1,
    IPPROTO_TCP = 6,
    IPPROTO_UDP = 17,
};

enum {
    INADDR_ANY = 0,
    INADDR_BROADCAST = -1,
};

typedef uint32_t in_addr_t;

typedef uint16_t in_port_t;

struct in_addr {  // size 0x4
    in_addr_t s_addr;  // 0x00
};
static_assert(sizeof(in_addr) == 0x4, "in_addr");

struct sockaddr_in {  // size 0x10
    uint8_t sin_len;  // 0x00
    sa_family_t sin_family;  // 0x01
    in_port_t sin_port;  // 0x02
    in_addr sin_addr;  // 0x04
    char sin_zero[8];  // 0x08
};
static_assert(sizeof(sockaddr_in) == 0x10, "sockaddr_in");

enum {
    INADDR_LOOPBACK = 2130706433,
    INADDR_NONE = -1,
    INADDR_UNSPEC_GROUP = -536870912,
    INADDR_ALLHOSTS_GROUP = -536870911,
    INADDR_ALLRTRS_GROUP = -536870910,
    INADDR_ALLRPTS_GROUP = -536870890,
    INADDR_CARP_GROUP = -536870894,
    INADDR_PFSYNC_GROUP = -536870672,
    INADDR_ALLMDNS_GROUP = -536870661,
    INADDR_MAX_LOCAL_GROUP = -536870657,
};

