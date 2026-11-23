// The nn::socket entry points main.elf imports (llvm-nm --dynamic --undefined-only).
// Declarations only -- the SDK is not vendored and its functions carry no DWARF.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "sys/socket.h"
#include "netinet/in.h"

struct hostent;
struct addrinfo;

// Added by hand (values from the call sites in bdPlatformStreamSocket-switch.cpp; Linux numbering).
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOL_SOCKET 0xffff
#define SO_BROADCAST 0x0020
#define F_GETFL 3
#define F_SETFL 4
#define O_NONBLOCK 0x800
#define POLLOUT 0x0004
#define POLLERR 0x0008
#define EINPROGRESS 115
#define EBADF 9
#define EAGAIN 11
#define EINVAL 22
#define EMFILE 24
#define ENOTSOCK 88
#define EOPNOTSUPP 95
#define EAFNOSUPPORT 97
#define EADDRINUSE 98
#define ECONNABORTED 103
#define ECONNRESET 104
#define ENOTCONN 107

typedef unsigned long nfds_t;

struct pollfd {  // size 0x8
    int fd;  // 0x00
    short events;  // 0x04
    short revents;  // 0x06
};

namespace nn {
namespace socket {

hostent* GetHostByName(const char* pName);
int InetPton(int af, const char* pSrc, void* pDst);
const char* InetNtop(int af, const void* pSrc, char* pDst, unsigned int size);
char* InetNtoa(in_addr in);
uint16_t InetHtons(uint16_t hostValue);
uint32_t InetHtonl(uint32_t hostValue);
uint16_t InetNtohs(uint16_t networkValue);
uint32_t InetNtohl(uint32_t networkValue);

int Close(int socket);
int Socket(int domain, int type, int protocol);
int Fcntl(int socket, int command, ...);
int Connect(int socket, const sockaddr* pAddress, socklen_t addressLength);
ssize_t Send(int socket, const void* buffer, size_t bufferLength, int flags);
ssize_t Recv(int socket, void* buffer, size_t bufferLength, int flags);
int Poll(pollfd* pFds, nfds_t numFds, int timeout);
int Bind(int socket, const sockaddr* pAddress, socklen_t addressLength);
int SetSockOpt(int socket, int level, int optionName, const void* pOptionValue, socklen_t optionLength);
ssize_t RecvFrom(int socket, void* buffer, size_t bufferLength, int flags, sockaddr* pOutAddress,
                 socklen_t* pOutAddressLength);
int GetAddrInfo(const char* pNodeName, const char* pServName, const addrinfo* pHints, addrinfo** pResult);
void FreeAddrInfo(addrinfo* pResult);
int GetLastErrno();
ssize_t SendTo(int socket, const void* buffer, size_t bufferLength, int flags, const sockaddr* pAddress,
               socklen_t addressLength);

}  // namespace socket
}  // namespace nn

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(pollfd) == 0x8, "pollfd");
static_assert(offsetof(pollfd, fd) == 0x0, "pollfd");
static_assert(offsetof(pollfd, events) == 0x4, "pollfd");
static_assert(offsetof(pollfd, revents) == 0x6, "pollfd");
