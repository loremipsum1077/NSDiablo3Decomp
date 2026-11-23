// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherSuites.h

#pragma once

#include "bdCypher.h"
#include "bdHash.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

typedef bdUInt16 bdCypherSuite;

class bdCypherSuites {  // size 0x1
public:
    enum {
        BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 = 48385,
        BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256 = 48386,
    };
public:
    static bdCypherAlgorithms getCypherFromSuite(const bdCypherSuite suite);
    static bdHashAlgorithms getHashFromSuite(const bdCypherSuite suite);
};
static_assert(sizeof(bdCypherSuites) == 0x1, "bdCypherSuites");

