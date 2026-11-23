#include "bdDispatchInterceptor.h"









bdDispatchInterceptor::bdDispatchInterceptor() {
}

bdDispatchInterceptor::~bdDispatchInterceptor() {
}

bdBool bdDispatchInterceptor::acceptOutOfOrder(const bdReceivedMessage &) {
    return false;
}
