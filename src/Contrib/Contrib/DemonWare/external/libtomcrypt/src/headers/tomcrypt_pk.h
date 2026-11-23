// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/external/libtomcrypt/src/headers/tomcrypt_pk.h

#pragma once

enum {
    PK_PUBLIC = 0,
    PK_PRIVATE = 1,
};

struct Rsa_key {  // size 0x48
    int type;  // 0x00
    void * e;  // 0x08
    void * d;  // 0x10
    void * N;  // 0x18
    void * p;  // 0x20
    void * q;  // 0x28
    void * qP;  // 0x30
    void * dP;  // 0x38
    void * dQ;  // 0x40
};
static_assert(sizeof(Rsa_key) == 0x48, "Rsa_key");

typedef Rsa_key rsa_key;

typedef struct {  // size 0x18
    void * x;  // 0x00
    void * y;  // 0x08
    void * z;  // 0x10
} ecc_point;

typedef struct {  // size 0x28
    int type;  // 0x00
    int idx;  // 0x04
    ecc_point pubkey;  // 0x08
    void * k;  // 0x20
} ecc_key;

enum {
    LTC_ASN1_EOL = 0,
    LTC_ASN1_INTEGER = 1,
    LTC_ASN1_SHORT_INTEGER = 2,
    LTC_ASN1_BIT_STRING = 3,
    LTC_ASN1_OCTET_STRING = 4,
    LTC_ASN1_NULL = 5,
    LTC_ASN1_OBJECT_IDENTIFIER = 6,
    LTC_ASN1_IA5_STRING = 7,
    LTC_ASN1_PRINTABLE_STRING = 8,
    LTC_ASN1_UTCTIME = 9,
    LTC_ASN1_CHOICE = 10,
    LTC_ASN1_SEQUENCE = 11,
    LTC_ASN1_SET = 12,
    LTC_ASN1_SETOF = 13,
};

struct ltc_asn1_list_ {  // size 0x40
    int type;  // 0x00
    void * data;  // 0x08
    long unsigned int size;  // 0x10
    int used;  // 0x18
    ltc_asn1_list_ * prev;  // 0x20
    ltc_asn1_list_ * next;  // 0x28
    ltc_asn1_list_ * child;  // 0x30
    ltc_asn1_list_ * parent;  // 0x38
};
static_assert(sizeof(ltc_asn1_list_) == 0x40, "ltc_asn1_list_");

typedef ltc_asn1_list_ ltc_asn1_list;


// Added by hand: ECC (C linkage, upstream prototypes).
union Prng_state;
extern "C" {
int ecc_make_key(union Prng_state * prng, int wprng, int keysize, ecc_key * key);
void ecc_free(ecc_key * key);
int ecc_export(unsigned char * out, unsigned long * outlen, int type, ecc_key * key);
int ecc_import(const unsigned char * in, unsigned long inlen, ecc_key * key);
int ecc_shared_secret(ecc_key * private_key, ecc_key * public_key, unsigned char * out, unsigned long * outlen);
int rsa_import(const unsigned char * in, unsigned long inlen, rsa_key * key);
void rsa_free(rsa_key * key);
}
