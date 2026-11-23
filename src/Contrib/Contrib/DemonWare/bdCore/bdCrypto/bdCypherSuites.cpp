#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherSuites.h"

#include "Contrib/Contrib/DemonWare/bdCore/bdCrypto/bdCypherSuites.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
bdCypherAlgorithms bdCypherSuites::getCypherFromSuite(const bdCypherSuite suite)
{
    bdCypherAlgorithms result = BD_MAX_CYPHER_ALGORITHMS;
    switch (suite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        result = BD_CYPHER_AES128_CBC;
        break;
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        result = BD_CYPHER_AES128_GCM;
        break;
    default:
        bdUseVAArgs(suite);
        break;
    }
    return result;
}

bdHashAlgorithms bdCypherSuites::getHashFromSuite(const bdCypherSuite suite)
{
    bdHashAlgorithms result = BD_MAX_HASH_ALGORITHMS;
    switch (suite)
    {
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
    case BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256:
        result = BD_HASH_SHA256;
        break;
    default:
        bdUseVAArgs(suite);
        break;
    }
    return result;
}



