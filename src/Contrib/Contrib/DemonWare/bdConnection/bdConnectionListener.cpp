#include "bdConnectionListener.h"
#include "bdConnection.h"
bdConnectionListener::bdConnectionListener() {}
bdConnectionListener::~bdConnectionListener() {}
void bdConnectionListener::onConnect(bdConnectionRef) {}
void bdConnectionListener::onConnectFailed(bdConnectionRef) {}
void bdConnectionListener::onDisconnect(bdConnectionRef) {}
void bdConnectionListener::onReconnect(bdConnectionRef connection) { onDisconnect(connection); onConnect(connection); }
