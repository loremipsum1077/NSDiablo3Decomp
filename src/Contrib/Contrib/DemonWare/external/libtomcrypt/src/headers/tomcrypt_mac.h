// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/external/libtomcrypt/src/headers/tomcrypt_mac.h

#pragma once

#include "tomcrypt_hash.h"

struct Hmac_state {  // size 0x1b0
    hash_state md;  // 0x00
    int hash;  // 0xd0
    hash_state hashstate;  // 0xd8
    unsigned char * key;  // 0x1a8
};
static_assert(sizeof(Hmac_state) == 0x1b0, "Hmac_state");

typedef Hmac_state hmac_state;


// Added by hand: HMAC entry points (C linkage, upstream prototypes).
extern "C" {
int hmac_init(hmac_state * hmac, int hash, const unsigned char * key, unsigned long keylen);
int hmac_process(hmac_state * hmac, const unsigned char * in, unsigned long inlen);
int hmac_done(hmac_state * hmac, unsigned char * out, unsigned long * outlen);
}
