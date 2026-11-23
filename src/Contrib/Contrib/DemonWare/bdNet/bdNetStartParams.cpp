// Original file: Contrib/Contrib/DemonWare/bdNet/bdNetStartParams.cpp
#include "bdNetStartParams.h"
#include "../bdCore/bdContainers/bdArray.inl"
#include "../bdCore/bdString/bdString.h"









bdNetStartParams::bdNetStartParams()
    :




      m_onlineGame(false),
      m_gamePort(3074),
      m_socket(NULL),
      m_overrideNATType(BD_NAT_OPEN),



      m_useAnyIP(true),
      m_threadStackSize(0x10000)
{
    m_natTravHosts.pushBack("stun.us.demonware.net");
    m_natTravHosts.pushBack("stun.eu.demonware.net");
    m_natTravHosts.pushBack("stun.jp.demonware.net");
    m_natTravHosts.pushBack("stun.au.demonware.net");
    m_natTravPort = 3074;
}
