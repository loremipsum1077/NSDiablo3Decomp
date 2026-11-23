// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdSocketRouterAdHoc-switch.h

#pragma once

#include "../bdCore/bdSocket/bdCommonAddr.h"
#include "../bdCore/bdSocket/bdDefaultSocket.h"
#include "../bdCore/bdSocket/bdSecurityID.h"
#include "../bdCore/bdSocket/bdSecurityKey.h"
#include "../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "bdAddrHandle.h"
#include "bdSocketRouter.h"
#include "../../../../NintendoSDK/Include/nn/ldn/ldn_Types.h"
#include <stddef.h>
#include <stdint.h>

class bdSocketRouterConfig;

class bdSocketRouterAdHoc : public bdSocketRouter {  // size 0x9a8
public:
    BD_CLASS_ALLOCATOR(bdSocketRouterAdHoc);
    bdSocketRouterAdHoc();
    virtual ~bdSocketRouterAdHoc();  // vtable slot 0
    virtual bdBool connect(bdAddrHandleRef & addrHandle);  // vtable slot 7
    virtual bdBool init(bdDefaultSocket * socket, const bdCommonAddrRef localCommonAddr, const bdSocketRouterConfig & config);  // vtable slot 4
    virtual void pump();  // vtable slot 5
    virtual bdBool quit();  // vtable slot 6
    bdBool scan(nn::ldn::NetworkInfo * networkInfoBuffer, const bdUInt networkInfoBufferSize, bdInt & numNetworkInfoFound, const nn::ldn::ScanFilter filter, const bdUInt channel);
    bdBool createNetwork(const nn::ldn::NetworkConfig networkConfig, const nn::ldn::SecurityConfig securityConfig, const nn::ldn::UserConfig userConfig, const bdNChar8 *const customData, size_t customDataSize, bdSecurityID secId, bdSecurityKey secKey);
    bdBool connectNetwork(nn::ldn::NetworkInfo & networkInfo, const nn::ldn::SecurityConfig securityConfig, const nn::ldn::UserConfig userConfig, bdAddrHandleRef & addrHandle, bdUInt communicationVersion, bool registerSecurityKey, bool hideErrors);
    bdBool updateAdvertiseData(const bdNChar8 *const customData, size_t customDataSize, bdSecurityID secId, bdSecurityKey secKey);
    bdBool leaveNetwork();
    struct bdAdvertiseData {  // size 0x180
        bdSecurityID m_secID;  // 0x00
        bdSecurityKey m_secKey;  // 0x08
        bdNChar8 m_customData[360];  // 0x18
    };
protected:
    void processLdnState();
    void processNetworkNodeUpdates();
    bdBool updateLocalCommonAddr(const bdUInt ipAddress);
    nn::ldn::NetworkInfo m_networkInfo;  // 0x318
    nn::ldn::NodeInfo m_nodesInfo[8];  // 0x798
    bdUInt32 m_nodeID;  // 0x998
    bdSecurityID m_secID;  // 0x99c
    bdBool m_isHost;  // 0x9a4
};
static_assert(sizeof(bdSocketRouterAdHoc) == 0x9a8, "bdSocketRouterAdHoc");


// Functions defined in this file (bodies are not in the DWARF):
//   line    30  void bdSocketRouterAdHoc::operator delete(void * p)  [inlined into 1 functions]
//   line    30  void * bdSocketRouterAdHoc::operator new(size_t nbytes)  [inlined into 1 functions]
//   line   127  bdSocketRouterAdHoc::bdAdvertiseData::~bdAdvertiseData()  [inlined into 2 functions]
