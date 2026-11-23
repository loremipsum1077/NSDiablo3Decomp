// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/external/libtomcrypt/src/headers/tomcrypt_hash.h

#pragma once

#include "tomcrypt_macros.h"

struct sha512_state {  // size 0xd0
    ulong64 length;  // 0x00
    ulong64 state[8];  // 0x08
    long unsigned int curlen;  // 0x48
    unsigned char buf[128];  // 0x50
};
static_assert(sizeof(sha512_state) == 0xd0, "sha512_state");

struct sha256_state {  // size 0x90
    ulong64 length;  // 0x00
    ulong32 state[8];  // 0x08
    ulong32 curlen;  // 0x48
    unsigned char buf[64];  // 0x50
};
static_assert(sizeof(sha256_state) == 0x90, "sha256_state");

struct sha1_state {  // size 0x78
    ulong64 length;  // 0x00
    ulong32 state[5];  // 0x08
    ulong32 curlen;  // 0x30
    unsigned char buf[64];  // 0x38
};
static_assert(sizeof(sha1_state) == 0x78, "sha1_state");

struct md5_state {  // size 0x70
    ulong64 length;  // 0x00
    ulong32 state[4];  // 0x08
    ulong32 curlen;  // 0x28
    unsigned char buf[64];  // 0x30
};
static_assert(sizeof(md5_state) == 0x70, "md5_state");

struct tiger_state {  // size 0x68
    ulong64 state[3];  // 0x00
    ulong64 length;  // 0x18
    long unsigned int curlen;  // 0x20
    unsigned char buf[64];  // 0x28
};
static_assert(sizeof(tiger_state) == 0x68, "tiger_state");

union Hash_state {  // size 0xd0
    sha512_state sha512;  // 0x00
    sha256_state sha256;  // 0x00
    sha1_state sha1;  // 0x00
    md5_state md5;  // 0x00
    tiger_state tiger;  // 0x00
    void * data;  // 0x00
};
static_assert(sizeof(Hash_state) == 0xd0, "Hash_state");

typedef Hash_state hash_state;


// Added by hand: hash descriptor registry and one-shot hashing (C linkage, upstream prototypes).
// The descriptor layout is not recorded in the DWARF; only its 0xd0 size is known from the symbols.
struct ltc_hash_descriptor {
    unsigned char opaque[0xd0];
};

extern "C" {
extern const struct ltc_hash_descriptor sha256_desc;
extern const struct ltc_hash_descriptor sha1_desc;
extern const struct ltc_hash_descriptor md5_desc;
extern const struct ltc_hash_descriptor tiger_desc;
int register_hash(const struct ltc_hash_descriptor * hash);
int find_hash(const char * name);
int hash_memory(int hash, const unsigned char * in, unsigned long inlen, unsigned char * out, unsigned long * outlen);
}
