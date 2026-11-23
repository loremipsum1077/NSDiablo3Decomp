// Original file: Contrib/Contrib/DemonWare/bdUtility/bdJOSE/bdJOSE.cpp
// The two name tables the image keeps; bdJWTRegisteredClaims (line 22) and the JWE tables
// (lines 77, 88) were dropped by the linker and are not written.

#include "bdJOSE.h"

































const bdNChar8 * bdJOSE::bdJWTRegisteredHeaders[] = {
    "alg",
    "enc",
    "zip",
    "jku",
    "jwk",
    "kid",
    "x5u",
    "x5c",
    "x5t",
    "x5t#S256",
    "typ",
    "cty",
    "crit",
};


const bdNChar8 * bdJOSE::bdJWSSigAlgorithms[] = {
    "HS256",
    "HS384",
    "HS512",
    "RS256",
    "RS384",
    "RS512",
    "ES256",
    "ES384",
    "ES512",
    "PS256",
    "PS384",
    "PS512",
    "none",
};
