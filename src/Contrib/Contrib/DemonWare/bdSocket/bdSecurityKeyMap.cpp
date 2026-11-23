#include "bdSecurityKeyMap.h"
#include "bdSecurityKeyMapListener.h"
#include "../bdCore/bdSocket/bdSecurityInfo.h"
#include "../bdCore/bdUtilities/bdTrulyRandom.h"
#include "../bdCore/bdContainers/bdSingleton.h"
#include "../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdSecurityKeyMap::bdSecurityKeyMap() : m_map(4, 0.75f) { m_listener = 0; }

bdBool bdSecurityKeyMap::initKey(bdSecurityID& id, bdSecurityKey& key)
{
    bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(id.ab, sizeof(id.ab));
    bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(key.ab, sizeof(key.ab));
    bdNChar8 tmp[36];
    bdSecurityInfo::toString(id, tmp, sizeof(tmp));
    bdUseVAArgs(tmp);
    bdSecurityInfo::toString(key, tmp, sizeof(tmp));
    bdUseVAArgs(tmp);
    return true;
}

bdBool bdSecurityKeyMap::registerKey(const bdSecurityID& id, const bdSecurityKey& key)
{
    const bdBool added = m_map.put(id, key);
    bdNChar8 tmp[36];
    bdSecurityInfo::toString(id, tmp, sizeof(tmp));
    bdUseVAArgs(tmp);
    bdSecurityInfo::toString(key, tmp, sizeof(tmp));
    bdUseVAArgs(tmp);
    if (!added) bdUseVAArgs();
    return added;
}

bdBool bdSecurityKeyMap::unregisterKey(const bdSecurityID& id)
{
    if (m_listener && m_map.containsKey(id)) m_listener->onSecurityKeyRemove(id);
    const bdBool removed = m_map.remove(id);
    bdNChar8 tmp[36];
    bdSecurityInfo::toString(id, tmp, sizeof(tmp));
    bdUseVAArgs(tmp);
    if (!removed) bdUseVAArgs();
    return removed;
}

bdBool bdSecurityKeyMap::get(const bdSecurityID& id, bdSecurityKey& key)
{
    const bdBool found = m_map.get(id, key);
    bdNChar8 tmp[36];
    bdSecurityInfo::toString(id, tmp, sizeof(tmp));
    bdUseVAArgs(tmp);
    if (found) {
        bdSecurityInfo::toString(key, tmp, sizeof(tmp));
        bdUseVAArgs(tmp);
    } else bdUseVAArgs();
    return found;
}

bdBool bdSecurityKeyMap::contains(const bdSecurityID& id) const
{
    const bdBool found = m_map.containsKey(id);
    bdNChar8 securityIDString[36];
    bdSecurityInfo::toString(id, securityIDString, sizeof(securityIDString));
    bdUseVAArgs(securityIDString);
    return found;
}

void bdSecurityKeyMap::clear()
{
    if (m_listener) {
        for (bdHashMap<bdSecurityID, bdSecurityKey, bdHashingClass>::Iterator it = m_map.getIterator(); it; m_map.next(it)) {
            m_listener->onSecurityKeyRemove(m_map.getKey(it));
        }
    }
    m_map.clear();
}

void bdSecurityKeyMap::registerListener(bdSecurityKeyMapListener* listener) { m_listener = listener; }
void bdSecurityKeyMap::unregisterListener() { m_listener = 0; }
