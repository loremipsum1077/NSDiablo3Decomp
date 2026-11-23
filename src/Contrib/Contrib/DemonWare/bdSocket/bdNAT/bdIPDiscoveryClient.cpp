#include "bdIPDiscoveryClient.h"
#include "bdPacket/bdIPDiscoveryPacket.h"
#include "bdPacket/bdIPDiscoveryPacketReply.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
bdIPDiscoveryClient::bdIPDiscoveryClient():m_socket(0),m_status(BD_IP_DISC_UNINITIALIZED),m_retries(0) {}
bdIPDiscoveryClient::~bdIPDiscoveryClient() {m_status=BD_IP_DISC_UNINITIALIZED;m_socket=0;}
bdBool bdIPDiscoveryClient::init(bdDefaultSocket *socket,const bdAddr &server,bdIPDiscoveryConfig config) {
 bdBool result=false;
 if(m_status!=BD_IP_DISC_UNINITIALIZED) {bdUseVAArgs();return false;}
 if(socket) {
 m_socket=socket;m_serverAddr=server;m_retries=0;
 config.sanityCheckConfig();m_config=config;m_status=BD_IP_DISC_RUNNING;
 bdBool sendIPDiscPacketResult=sendIPDiscoveryPacket();result=sendIPDiscPacketResult;
 } else bdUseVAArgs();
 return result;
}
bdBool bdIPDiscoveryClient::sendIPDiscoveryPacket() {
 bdBool result=false;
 if(m_status==BD_IP_DISC_RUNNING) {
 m_timer.start();
 bdIPDiscoveryPacket packet(m_config.m_version);
 const bdUInt bufferSize=1288;
 bdUByte8 buffer[bufferSize]={};
 bdUInt len=0;
 const bdBool serializeResult=packet.serialize(buffer,bufferSize,0,len);
 if(serializeResult) {
 const bdInt sendResult=m_socket->sendTo(m_serverAddr,buffer,len);
 result=sendResult>0 && static_cast<bdUInt>(sendResult)==len;
 }
 if(!result) {bdUseVAArgs();m_status=BD_IP_DISC_ERROR;}
 }
 return result;
}
const bdAddr &bdIPDiscoveryClient::getPublicAddress() const {
 if(m_status!=BD_IP_DISC_SUCCESS) bdUseVAArgs();return m_publicAddr;
}
void bdIPDiscoveryClient::pump(bdAddr fromAddr,bdUByte8 *data,const bdInt dataSize) {
 if(m_status==BD_IP_DISC_RUNNING && dataSize>0) {
 bdIPDiscoveryPacketReply packet;bdUInt tmpUInt;
 if(fromAddr==m_serverAddr && packet.deserialize(data,dataSize,0,tmpUInt)) {
 m_publicAddr=packet.getAddr();
 if(packet.getProtocolVersion()>2 && m_publicAddr!=packet.getProtectedAddr()) {
 m_publicAddr=packet.getProtectedAddr();
 bdNChar8 publicAddrBuffer[100]={};bdNChar8 protectedAddrBuffer[100]={};
 packet.getAddr().toString(publicAddrBuffer,100);packet.getProtectedAddr().toString(protectedAddrBuffer,100);
 bdUseVAArgs(publicAddrBuffer,protectedAddrBuffer);
 }
 m_status=BD_IP_DISC_SUCCESS;
 bdNChar8 addrBuffer[100]={};m_publicAddr.toString(addrBuffer,100);bdUseVAArgs(addrBuffer);
 }
 }
 if(m_status==BD_IP_DISC_RUNNING) {
 if(m_timer.getElapsedTimeInSeconds()>m_config.m_requestTimeout) {
 if(m_retries++ < m_config.m_retries) {bdUseVAArgs(m_retries,m_config.m_retries);sendIPDiscoveryPacket();return;}
 }
 if(m_retries>=m_config.m_retries) {bdUseVAArgs(m_config.m_retries);m_status=BD_IP_DISC_FAIL;}
 }
}
void bdIPDiscoveryClient::quit() {
 m_socket=0;m_serverAddr=bdAddr();m_publicAddr=bdAddr();m_timer.reset();m_status=BD_IP_DISC_UNINITIALIZED;m_retries=0;
}
bdIPDiscoveryClient::bdIPDiscoveryClientStatus bdIPDiscoveryClient::getStatus() const{return m_status;}
