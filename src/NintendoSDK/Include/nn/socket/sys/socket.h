// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/socket/sys/socket.h

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint8_t sa_family_t;

// Not in the DWARF (a macro in the SDK); the value is the BSD one.
#define AF_INET 2

typedef unsigned int socklen_t;

// The SDK's bits/alltypes.h defines these; the vendored tree does not carry it.
typedef long ssize_t;

struct sockaddr {  // size 0x10
    unsigned char sa_len;  // 0x00
    sa_family_t sa_family;  // 0x01
    char sa_data[14];  // 0x02
};
static_assert(sizeof(sockaddr) == 0x10, "sockaddr");

