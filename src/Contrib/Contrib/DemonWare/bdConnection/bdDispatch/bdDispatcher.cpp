#include "bdDispatcher.h"
#include "bdDispatchInterceptor.h"
#include "../bdConnection.h"















bdDispatcher::bdDispatcher() {
}


void bdDispatcher::process(bdConnectionRef connection) {

    bdMessageRef message;

    while (connection->getMessageToDispatch(message)) {

        bdReceivedMessage rmessage(message, connection);
        bdBool accepted = false;
        for (bdUInt i = 0; !accepted && i < m_interceptors.getSize(); ++i) {
            accepted = m_interceptors[i]->accept(rmessage);
        }
    }

    while (connection->getOutOfOrderMessageToDispatch(message)) {

        bdReceivedMessage rmessage(message, connection);
        bdBool accepted = false;
        for (bdUInt i = 0; !accepted && i < m_interceptors.getSize(); ++i) {
            accepted = m_interceptors[i]->acceptOutOfOrder(rmessage);
        }
    }
}








void bdDispatcher::registerInterceptor(bdDispatchInterceptor *const interceptor) {
    m_interceptors.pushBack(interceptor);
}










void bdDispatcher::unregisterInterceptor(bdDispatchInterceptor *const interceptor) {
    m_interceptors.removeAllKeepOrder(interceptor);
}









void bdDispatcher::reset() {
    m_interceptors.clear();
}
