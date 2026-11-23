// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/socket/netdb.h

#pragma once

struct hostent {  // size 0x20
    char * h_name;  // 0x00
    char ** h_aliases;  // 0x08
    int h_addrtype;  // 0x10
    int h_length;  // 0x14
    char ** h_addr_list;  // 0x18
};
static_assert(sizeof(hostent) == 0x20, "hostent");


// Added by hand: BSD layout, offsets from bdPlatformSocket::getHostByName.
struct sockaddr;
struct addrinfo {  // size 0x30
    int ai_flags;  // 0x00
    int ai_family;  // 0x04
    int ai_socktype;  // 0x08
    int ai_protocol;  // 0x0c
    unsigned int ai_addrlen;  // 0x10
    char * ai_canonname;  // 0x18
    sockaddr * ai_addr;  // 0x20
    addrinfo * ai_next;  // 0x28
};
static_assert(sizeof(addrinfo) == 0x30, "addrinfo");
