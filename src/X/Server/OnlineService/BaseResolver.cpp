// Original file: X/Server/OnlineService/BaseResolver.cpp
// SNOName.h + WorldPlace.h first: the static initializer starts with SNONAME_NONE then cgplaceNull.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "BaseResolver.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Sys/Atomic.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Contrib/Contrib/blz/include/blz/intrusive_ptr.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"

namespace OnlineService {

// Lines 18, 19, 21 from __cxx_global_var_init.3/.4/.5; s_resolverCounter's line (20) inferred from its
// .bss address between s_stats and s_scopeResolver.
Blizzard::Lock::RMutex BaseResolver::s_statsMutex;
BaseResolver::Stats BaseResolver::s_stats;
uint32 BaseResolver::s_resolverCounter = 0;
XVarScope BaseResolver::s_scopeResolver("Resolver", NULL);



ResolverStats::ResolverStats(XVarScope & tParentScope, const char * pszName)
    // line 26-27: no rows; ':' placement inferred
    :
      m_scope(tParentScope, pszName),
      m_varCurrent(m_scope, "Current", 0),
      m_varComplete(m_scope, "Complete", 0),
      m_varFailed(m_scope, "Failed", 0),
      m_varRuntimeLast(m_scope, "RuntimeLast", 0),
      m_varRuntimeTrend(m_scope, "RuntimeTrend", 0),
      m_varRuntimeMax(m_scope, "RuntimeMax", 0),
      m_varLocktimeTrend(m_scope, "LocktimeTrend", 0)


// vtable store is attributed to line 39 (the opening brace).
{
}






















BaseResolver::BaseResolver(const char * pszResolverName)
    :
      m_errorCode(ERROR_GLOBAL_SUCCESS),
      m_uResolverId(Blizzard::Lock::Atomic::Increment((volatile int32 *)&s_resolverCounter)),
      m_microsecondsInQueueState(0),
      m_logHeader(),
      m_stats(NULL)
{
    char szLogNameBuffer[64];
    Blizzard::String::Format(szLogNameBuffer, sizeof(szLogNameBuffer), "%s(%llu)", pszResolverName, m_uResolverId);
    blz::string tName(pszResolverName);

    m_logHeader = szLogNameBuffer;
    m_startTime = m_lockTime = Blizzard::Time::Microseconds();



    s_statsMutex.Enter();
    Stats::iterator i = s_stats.find(tName);
    if (i != s_stats.end())
    {
        m_stats = i->second;
    }
    else
    {
        m_stats = new ResolverStats(s_scopeResolver, tName.c_str());
        s_stats.insert(Stats::value_type(tName, m_stats));
    }
    s_statsMutex.Leave();

    m_stats->m_varCurrent.Increment(1);


}



BaseResolver::~BaseResolver()
{
    Blizzard::Time::Microsecond runtime = Blizzard::Time::Microseconds() - m_startTime - m_microsecondsInQueueState;
    Blizzard::Time::Microsecond locktime = m_lockTime - m_startTime;


    m_stats->m_varCurrent.Decrement(1);
    m_stats->m_varComplete.Increment(1);

    if (m_errorCode != ERROR_GLOBAL_SUCCESS)
    {
        m_stats->m_varFailed.Increment(1);
    }

    m_stats->m_varRuntimeLast.Set(runtime, XVarBase::DIRTY_DEFAULT);
    m_stats->m_varRuntimeTrend.Set((uint64)(m_stats->m_varRuntimeTrend.Get() * 0.9f + runtime * 0.1f), XVarBase::DIRTY_DEFAULT);
    m_stats->m_varLocktimeTrend.Set((uint64)(m_stats->m_varLocktimeTrend.Get() * 0.9f + locktime * 0.1f), XVarBase::DIRTY_DEFAULT);
    if (runtime > m_stats->m_varRuntimeMax.Get())
    {
        m_stats->m_varRuntimeMax.Set(runtime, XVarBase::DIRTY_DEFAULT);
    }

}

}  // namespace OnlineService
