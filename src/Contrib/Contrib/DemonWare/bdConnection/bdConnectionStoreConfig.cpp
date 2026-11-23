#include "bdConnectionStoreConfig.h"















bdConnectionStoreConfig::bdConnectionStoreConfig() : m_maxReceiveIterations(100), m_maxShutdownDuration(5.0f), m_maxFlushIterations(5), m_acceptOutOfOrder(false) {
}

void bdConnectionStoreConfig::reset() {
    m_maxReceiveIterations = 100;
    m_maxShutdownDuration = 5.0f;
    m_maxFlushIterations = 5;
    m_acceptOutOfOrder = false;
}
