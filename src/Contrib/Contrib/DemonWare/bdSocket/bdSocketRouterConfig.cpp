// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSocketRouterConfig.cpp
#include "bdSocketRouterConfig.h"
#include "bdDTLS/bdDTLSPackets.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"
































bdSocketRouterConfig::bdSocketRouterConfig()
    : m_ignoreConnectionReset(true)
    , m_DTLSAssociationReceiveTimeout(30.0f)
    , m_maxConnectionResets(10)






    , m_cypherSuite(bdCypherSuites::BD_DTLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256)
{
}

void bdSocketRouterConfig::reset()
{
    m_ignoreConnectionReset = true;
    m_DTLSAssociationReceiveTimeout = 30.0f;
    m_maxConnectionResets = 10;
}




bdBool bdSocketRouterConfig::sanityCheckConfig()
{




    if (m_ignoreConnectionReset != true)
    {
        bdLogWarn("bdSocket/bdSocketRouterConfig", "m_ignoreConnectionReset should be %s", "True");
    }


    if (m_DTLSAssociationReceiveTimeout > 50.0f || m_DTLSAssociationReceiveTimeout < 10.0f)
    {

        bdLogWarn("bdSocket/bdSocketRouterConfig", "m_DTLSAssociationReceiveTimeout %f", m_DTLSAssociationReceiveTimeout);
    }


    if (m_maxConnectionResets < 1 || m_maxConnectionResets > 100)
    {

        bdLogWarn("bdSocket/bdSocketRouterConfig", "m_maxConnectionResets %u", m_maxConnectionResets);
    }






    if (m_cypherSuite == bdCypherSuites::BD_DTLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256 &&
        BD_DTLS_VERSION < 3)
    {



        bdLogError("bdSocket/bdSocketRouterConfig", "GCM cypher suite requires a newer DTLS version");
        return false;
    }

    return true;
}
