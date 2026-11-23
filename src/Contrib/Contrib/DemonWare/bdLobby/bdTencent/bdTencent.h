// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdTencent/bdTencent.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../bdLobbyCommon/bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdSanitizedString;
class bdTencentAASRecord;
class bdTencentClientDecidedRewardRequest;
class bdTencentClientDecidedRewardResponse;
class bdTencentCumulativeReward;
class bdTencentRedeemTeamNameChangeRequest;
class bdVerifyString;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdTencent {  // size 0x50018
public:
    BD_CLASS_ALLOCATOR(bdTencent);
    bdTencent();
    bdTencent(bdRemoteTaskManager *const);
    ~bdTencent();
    enum bdTSSQueryType {
        QUERY_WARGOD_QUALIFICATION = 1,
    };
    bdRemoteTaskRef verifyString(const bdNChar8 *const, const bdUInt, bdVerifyString *);
    bdRemoteTaskRef sanitizeString(const bdNChar8 *const, const bdUInt, bdSanitizedString *);
    bdRemoteTaskRef getAASRecord(bdTencentAASRecord *);
    bdRemoteTaskRef getAASRecordsByUserID(const bdOnlineUserID *, const bdUInt, bdTencentAASRecord *);
    bdRemoteTaskRef registerCodoID(const bdUByte8 *, const bdUByte8 *, bdUInt32, const bdUByte8 *, const bdUInt32, const bdUByte8 *, const bdUInt32, const bdNChar8 *const);
    bdRemoteTaskRef tLog(const bdNChar8 *const, bdUInt);
    bdRemoteTaskRef tLogBatchSend(const bdNChar8 *const, bdUInt);
    bdRemoteTaskRef tlogBatchSendCompressed(const bdUByte8 *const, bdUInt);
    bdRemoteTaskRef antibotSendData(const bdNChar8 *const, const bdUInt);
    bdRemoteTaskRef antibotSendDataImmediately(const bdNChar8 *const, const bdUInt);
    bdRemoteTaskRef doLogoff(const bdUInt32);
    bdRemoteTaskRef getClientDecidedRewards(bdTencentClientDecidedRewardRequest *, bdTencentClientDecidedRewardResponse *);
    bdRemoteTaskRef claimRewards();
    bdRemoteTaskRef claimClientDecidedRewards(const bdNChar8 **, const bdUInt32);
    bdRemoteTaskRef getCumulativeRewards(bdTencentCumulativeReward *, const bdUInt32);
    bdRemoteTaskRef claimCumulativeReward(const bdNChar8 *);
    bdRemoteTaskRef redeemNameChange(const bdUInt32, const bdNChar8 *const, const bdUInt32);
    bdRemoteTaskRef redeemTeamNameChange(bdTencentRedeemTeamNameChangeRequest *);
    bdRemoteTaskRef makeTSSQuery(const bdTencent::bdTSSQueryType, const bdUInt32);
    void setAntibotSendDataInterval(bdUInt32 interval);
private:
    bdRemoteTaskRef sendTLog(const bdNChar8 *const, bdUInt, bdBool);
    bdUInt32 m_antibotSendInterval;  // 0x00
    bdUInt32 m_antibotLastMessageTime;  // 0x04
    bdNChar8 m_antibotBatchBuffer[327680];  // 0x08
    bdUWord m_antibotBatchTotalSize;  // 0x50008
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x50010
};
static_assert(sizeof(bdTencent) == 0x50018, "bdTencent");


// Functions defined in this file (bodies are not in the DWARF):
//   line    64  void bdTencent::operator delete(void * p)  [inlined into 1 functions]
//   line    64  void * bdTencent::operator new(size_t nbytes)  [inlined into 1 functions]
