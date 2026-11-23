// Hand-written subset; reference: build/dwarf/headers/.../tomcrypt_cipher.h.
// Original file: Contrib/Contrib/DemonWare/external/libtomcrypt/src/headers/tomcrypt_cipher.h

#pragma once

#include "tomcrypt_macros.h"

// Key schedules and chaining-mode states, from the DWARF.
struct rijndael_key {  // size 0x3c8
    ulong32 eK[60];  // 0x00
    ulong32 dK[60];  // 0x1e0
    int Nr;  // 0x3c0
};
static_assert(sizeof(rijndael_key) == 0x3c8, "rijndael_key");

struct des_key {  // size 0x200
    ulong32 ek[32];  // 0x00
    ulong32 dk[32];  // 0x100
};
static_assert(sizeof(des_key) == 0x200, "des_key");

struct des3_key {  // size 0x600
    ulong32 ek[3][32];  // 0x00
    ulong32 dk[3][32];  // 0x300
};
static_assert(sizeof(des3_key) == 0x600, "des3_key");

union Symmetric_key {  // size 0x600
    des_key des;  // 0x00
    des3_key des3;  // 0x00
    rijndael_key rijndael;  // 0x00
    void * data;  // 0x00
};
static_assert(sizeof(Symmetric_key) == 0x600, "Symmetric_key");

typedef Symmetric_key symmetric_key;

typedef struct {  // size 0x710
    int cipher;  // 0x00
    int blocklen;  // 0x04
    int padlen;  // 0x08
    int mode;  // 0x0c
    unsigned char ctr[128];  // 0x10
    unsigned char pad[128];  // 0x90
    symmetric_key key;  // 0x110
} symmetric_CTR;


// symmetric_CBC, from the DWARF of bdAligned<symmetric_CBC, 16> (the union member there).
typedef struct {  // size 0x688
    int cipher;  // 0x00
    int blocklen;  // 0x04
    unsigned char IV[128];  // 0x08
    symmetric_key key;  // 0x88
} symmetric_CBC;
static_assert(sizeof(symmetric_CBC) == 0x688, "symmetric_CBC");

// Added by hand: cipher descriptor registry (C linkage, upstream prototypes).
// Only the leading fields of the upstream descriptor are declared; the rest
// (key lengths, rounds and the function pointers) are not used from C++ here.
struct ltc_cipher_descriptor {
    const char * name;
    unsigned char ID;
};

extern "C" {
extern const struct ltc_cipher_descriptor aes_desc;
extern const struct ltc_cipher_descriptor des3_desc;
int register_cipher(const struct ltc_cipher_descriptor * cipher);
int find_cipher_id(unsigned char ID);
// Added by hand: CBC mode (C linkage, upstream prototypes).
int cbc_start(int cipher, const unsigned char * IV, const unsigned char * key, int keylen, int num_rounds, symmetric_CBC * cbc);
int cbc_encrypt(const unsigned char * pt, unsigned char * ct, unsigned long len, symmetric_CBC * cbc);
int cbc_decrypt(const unsigned char * ct, unsigned char * pt, unsigned long len, symmetric_CBC * cbc);
int cbc_setiv(const unsigned char * IV, unsigned long len, symmetric_CBC * cbc);
}
