// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/external/libtomcrypt/src/headers/tomcrypt.h

#pragma once

enum {
    CRYPT_OK = 0,
    CRYPT_ERROR = 1,
    CRYPT_NOP = 2,
    CRYPT_INVALID_KEYSIZE = 3,
    CRYPT_INVALID_ROUNDS = 4,
    CRYPT_FAIL_TESTVECTOR = 5,
    CRYPT_BUFFER_OVERFLOW = 6,
    CRYPT_INVALID_PACKET = 7,
    CRYPT_INVALID_PRNGSIZE = 8,
    CRYPT_ERROR_READPRNG = 9,
    CRYPT_INVALID_CIPHER = 10,
    CRYPT_INVALID_HASH = 11,
    CRYPT_INVALID_PRNG = 12,
    CRYPT_MEM = 13,
    CRYPT_PK_TYPE_MISMATCH = 14,
    CRYPT_PK_NOT_PRIVATE = 15,
    CRYPT_INVALID_ARG = 16,
    CRYPT_FILE_NOTFOUND = 17,
    CRYPT_PK_INVALID_TYPE = 18,
    CRYPT_PK_INVALID_SYSTEM = 19,
    CRYPT_PK_DUP = 20,
    CRYPT_PK_NOT_FOUND = 21,
    CRYPT_PK_INVALID_SIZE = 22,
    CRYPT_INVALID_PRIME_SIZE = 23,
};


// Added by hand: C linkage, upstream prototype.
extern "C" const char * error_to_string(int err);
