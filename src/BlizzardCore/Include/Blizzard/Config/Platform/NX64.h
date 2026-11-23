// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: BlizzardCore/Include/Blizzard/Config/Platform/NX64.h

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../../../../../NintendoSDK/Include/nn/socket/netdb.h"
#include "../../../../../NintendoSDK/Include/nn/socket/netinet/in.h"
#include "../../../../../NintendoSDK/Include/nn/socket/socket_Api.h"

// The BSD socket spellings, forwarded onto the SDK (only the ones written so far).
inline int closesocket(int s) { return nn::socket::Close(s); }

inline hostent* gethostbyname(const char* pName) { return nn::socket::GetHostByName(pName); }

inline uint16_t htons(uint16_t hostValue) { return nn::socket::InetHtons(hostValue); }

inline uint32_t htonl(uint32_t hostValue) { return nn::socket::InetHtonl(hostValue); }

inline uint32_t ntohl(uint32_t networkValue) { return nn::socket::InetNtohl(networkValue); }

inline uint16_t ntohs(uint16_t networkValue) { return nn::socket::InetNtohs(networkValue); }

inline uint32_t inet_addr(const char* pAddressIn) {
    in_addr tAddress;
    if (nn::socket::InetPton(AF_INET, pAddressIn, &tAddress) == 0) {
        return INADDR_NONE;
    }
    return tAddress.s_addr;
}

inline ssize_t sendto(int socket, const void* buffer, size_t bufferLength, int flags, const sockaddr* pAddress,
                      socklen_t addressLength) {
    return nn::socket::SendTo(socket, buffer, bufferLength, flags, pAddress, addressLength);
}


// Functions defined in this file (bodies are not in the DWARF):
//   line   126  int socket(int domain, int type, int protocol)  [inline]
//   line   130  int closesocket(int s)  [inlined into 1 functions]
//   line   134  int setsockopt(int socket, int level, int optionName, const void * pOptionValue, socklen_t optionLength)  [inline]
//   line   142  hostent * gethostbyname(const char * pName)  [inlined into 1 functions]
//   line   146  uint16_t htons(uint16_t hostValue)  [inlined into 6 functions]
//   line   150  uint32_t htonl(uint32_t hostValue)  [inlined into 6 functions]
//   line   154  uint32_t ntohl(uint32_t networkValue)  [inlined into 4 functions]
//   line   158  uint16_t ntohs(uint16_t networkValue)  [inlined into 1 functions]
//   line   162  ssize_t recvfrom(int socket, void * outBuffer, size_t outBufferLength, int flags, sockaddr * pOutAddress, socklen_t * pOutAddressLength)  [inline]
//   line   166  int connect(int socket, const sockaddr * pAddress, socklen_t addressLength)  [inline]
//   line   170  int bind(int socket, const sockaddr * pAddress, socklen_t addressLength)  [inline]
//   line   174  int listen(int socket, int backlog)  [inline]
//   line   178  uint32_t inet_addr(const char * pAddressIn)  [inlined into 1 functions]
//   line   187  int getsockname(int socket, sockaddr * pOutAddress, socklen_t * pOutAddressLength)  [inline]
//   line   191  int getpeername(int socket, sockaddr * pOutAddress, socklen_t * pOutAddressLength)  [inline]
//   line   195  int shutdown(int socket, int how)  [inline]
//   line   199  int accept(int socket, sockaddr * pOutAddress, socklen_t * pOutAddressLength)  [inline]
//   line   203  ssize_t send(int socket, const void * buffer, size_t bufferLength, int flags)  [inline]
//   line   207  ssize_t sendto(int socket, const void * buffer, size_t bufferLength, int flags, const sockaddr * pAddress, socklen_t addressLength)  [inlined into 1 functions]
//   line   211  ssize_t recv(int socket, void * outBuffer, size_t outBufferLength, int flags)  [inline]
//   line   215  int fcntl(int socket, int arg0, int arg1)  [inline]
//   line   219  int select(int numberOfDescriptors, fd_set * pReadDescriptors, fd_set * pWriteDescriptors, fd_set * pExceptDescriptors, timeval * pTimeout)  [inline]
