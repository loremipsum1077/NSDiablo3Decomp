// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/external/libtomcrypt/src/headers/tomcrypt_prng.h

#pragma once

#include "tomcrypt_cipher.h"
#include "tomcrypt_hash.h"
#include "tomcrypt_macros.h"

struct yarrow_prng {  // size 0x798
    int cipher;  // 0x00
    int hash;  // 0x04
    unsigned char pool[128];  // 0x08
    symmetric_CTR ctr;  // 0x88
};
static_assert(sizeof(yarrow_prng) == 0x798, "yarrow_prng");

struct fortuna_prng {  // size 0x2050
    hash_state pool[32];  // 0x00
    symmetric_key skey;  // 0x1a00
    unsigned char K[32];  // 0x2000
    unsigned char IV[16];  // 0x2020
    long unsigned int pool_idx;  // 0x2030
    long unsigned int pool0_len;  // 0x2038
    long unsigned int wd;  // 0x2040
    ulong64 reset_cnt;  // 0x2048
};
static_assert(sizeof(fortuna_prng) == 0x2050, "fortuna_prng");

union Prng_state {  // size 0x2050
    yarrow_prng yarrow;  // 0x00
    fortuna_prng fortuna;  // 0x00
};
static_assert(sizeof(Prng_state) == 0x2050, "Prng_state");

typedef Prng_state prng_state;


// Added by hand: PRNG registry and yarrow (C linkage, upstream prototypes). Only the
// leading field of the upstream descriptor is declared.
struct ltc_prng_descriptor {
    const char * name;
};

extern "C" {
extern const struct ltc_prng_descriptor yarrow_desc;
int register_prng(const struct ltc_prng_descriptor * prng);
int find_prng(const char * name);
int yarrow_start(prng_state * prng);
int yarrow_add_entropy(const unsigned char * in, unsigned long inlen, prng_state * prng);
int yarrow_ready(prng_state * prng);
}
