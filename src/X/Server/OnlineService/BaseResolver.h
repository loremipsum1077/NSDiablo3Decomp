// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/OnlineService/BaseResolver.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/intrusive_ptr.h"
#include "../../../Contrib/Contrib/blz/include/blz/ref_count_base.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../Common/OnlineService/OnlineServiceErrorCodes.h"
#include "../../Sigma/DataStructures/XVar.h"

namespace OnlineService {
class ResolverStats : public blz::atomic_ref_count_base<OnlineService::ResolverStats> {  // size 0x538
public:
    typedef blz::intrusive_ptr<OnlineService::ResolverStats> Ptr;
    XVarScope m_scope;  // 0x10
    XVarUint32 m_varCurrent;  // 0x30
    XVarUint64 m_varComplete;  // 0xe8
    XVarUint64 m_varFailed;  // 0x1a0
    XVarUint64 m_varRuntimeLast;  // 0x258
    XVarUint64 m_varRuntimeTrend;  // 0x310
    XVarUint64 m_varRuntimeMax;  // 0x3c8
    XVarUint64 m_varLocktimeTrend;  // 0x480
    ResolverStats(XVarScope &, const char *);
};
static_assert(sizeof(ResolverStats) == 0x538, "ResolverStats");

class BaseResolver {  // size 0x60
    // vtable pointer
public:
    static void Dispose();
    void Complete(OnlineService::ErrorCode);
    void Complete();
    uint32 GetCurrent();
    void AddQueueTimeInMicroseconds(Blizzard::Time::Microsecond);
    const char * GetLogHeader() const;
    const blz::string & GetLogHeaderStdString() const;
protected:
    BaseResolver(const char *);
    virtual ~BaseResolver();  // vtable slot 0
    OnlineService::ErrorCode m_errorCode;  // 0x08
    Blizzard::Time::Microsecond m_startTime;  // 0x10
    Blizzard::Time::Microsecond m_lockTime;  // 0x18
    OnlineService::ResolverStats::Ptr m_stats;  // 0x20
private:
    uint64 m_uResolverId;  // 0x28
    Blizzard::Time::Microsecond m_microsecondsInQueueState;  // 0x30
    blz::string m_logHeader;  // 0x38
    static uint32 s_resolverCounter;
    static Blizzard::Lock::RMutex s_statsMutex;
    typedef blz::unordered_map<blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> >, blz::intrusive_ptr<OnlineService::ResolverStats>, blz::hash<blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > >, blz::equal_to<blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > >, blz::allocator<blz::pair<const blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> >, blz::intrusive_ptr<OnlineService::ResolverStats> > > > Stats;
    static OnlineService::BaseResolver::Stats s_stats;
    static XVarScope s_scopeResolver;
};
static_assert(sizeof(BaseResolver) == 0x60, "BaseResolver");

}  // namespace OnlineService

// Functions defined in this file (bodies are not in the DWARF):
//   line    27  OnlineService::ResolverStats::~ResolverStats()  [0x7100968700 (+1 more), inlined into 1 functions]
//   line    77  const blz::string & OnlineService::BaseResolver::GetLogHeaderStdString() const  [inlined into 1 functions]
