// Original file: X/Common/OnlineService/SubscriptionEvent.h
#pragma once
#include "Contrib/Contrib/blz/include/blz/set.h"
#include "Callback/Callback_Impl.h"
namespace OnlineService {
template <typename... Args> class SubscriptionEvent {
public:
    typedef CallbackHash<void(Args...)> Subscriber;
    typedef blz::set<Subscriber, CallbackHashLessThan<Subscriber> > SubscribersSet;
    typedef void (*OnSubscriptionChanged)(bool, const Subscriber &);
    SubscriptionEvent(OnSubscriptionChanged pfnCb = NULL) : m_pfnOnSubscriptionChanged(pfnCb) {} // line 33
    void Subscribe(const Subscriber & cb) { // line 48
        size_t oldSize = m_mapSubscribers.size();
        m_mapSubscribers.insert(cb);
        if (m_mapSubscribers.size() != oldSize)
            FireSubscriptionChanged(m_mapSubscribers.size() > oldSize, cb);
    }
    void Unsubscribe(const Subscriber & cb) { // line 61
        size_t oldSize = m_mapSubscribers.size();
        m_mapSubscribers.erase(cb);
        if (m_mapSubscribers.size() != oldSize)
            FireSubscriptionChanged(m_mapSubscribers.size() > oldSize, cb);
    }
    void NotifySubscribers(Args... args) const { // line 74
        if (m_mapSubscribers.empty()) return;
        SubscribersSet mapCopy(m_mapSubscribers);
        for (typename SubscribersSet::const_iterator itr = mapCopy.begin(); itr != mapCopy.end(); ++itr) {
            const Subscriber & cb = *itr;
            if (m_mapSubscribers.find(cb) != m_mapSubscribers.end()) cb(args...);
        }
    }
private:
    SubscriptionEvent(const SubscriptionEvent &);
    SubscriptionEvent & operator=(const SubscriptionEvent &);
    void FireSubscriptionChanged(bool added, const Subscriber & cb) const { // line 106
        if (m_pfnOnSubscriptionChanged) m_pfnOnSubscriptionChanged(added, cb);
    }
    SubscribersSet m_mapSubscribers;
    OnSubscriptionChanged m_pfnOnSubscriptionChanged;
};
} // namespace OnlineService
