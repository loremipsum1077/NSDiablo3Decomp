// Template checks for blz::rb_tree holding OnlineService::CallbackHash
// values (the SubscriptionEvent subscriber sets). The OnlineService types are
// minimal stand-ins with the layout and the refcount code of
// X/Common/OnlineService/Callback/Callback_Impl.h (Callback is a pointer to a
// 0x18 byte node {call, destructor, refcount}; CallbackHash adds a hash key
// and an address key, size 0x18).
#include "Contrib/Contrib/blz/include/blz/rb_tree.h"
#include "Contrib/Contrib/blz/include/blz/set.h"
#include "Contrib/Contrib/blz/include/blz/string.h"

typedef unsigned int uint32;

void* CallbackNodeAlloc(unsigned long);
void CallbackNodeFree(void*);

namespace OnlineService {

template <typename FT>
class Callback;

template <typename R, typename... A>
class Callback<R(A...)> {
public:
    struct node_t {
        void* m_call;
        void (*m_destructor)(void*);
        int m_refCount;
    };

    Callback() : m_impl(0) {}

    Callback(const Callback& cb) {
        m_impl = cb.m_impl;
        AddRef(m_impl);
    }

    ~Callback() { Release(m_impl); }

protected:
    static void Free(node_t* a_node) {
        a_node->m_destructor(a_node + 1);
        if (a_node)
            CallbackNodeFree(a_node);
    }

    static void AddRef(node_t* p) {
        if (p)
            __atomic_add_fetch(&p->m_refCount, 1, __ATOMIC_SEQ_CST);
    }

    static void Release(node_t*& p) {
        if (p) {
            if (__atomic_sub_fetch(&p->m_refCount, 1, __ATOMIC_SEQ_CST) == 0)
                Free(p);
            p = 0;
        }
    }

    node_t* m_impl;
};

template <typename FT>
class CallbackHash : public Callback<FT> {
public:
    CallbackHash() : m_hashKey(0), m_addressKey(0) {}

    CallbackHash(const CallbackHash& cb) : Callback<FT>(cb) {
        m_hashKey = cb.m_hashKey;
        m_addressKey = cb.m_addressKey;
    }

    ~CallbackHash() {}

    uint32 HashKey() const { return m_hashKey; }

protected:
    uint32 m_hashKey;
    void* m_addressKey;
};

template <typename _Kty>
struct CallbackHashLessThan {
    bool operator()(const _Kty& left, const _Kty& right) const {
        return left.HashKey() < right.HashKey();
    }
};

struct AchievementsInterface;
struct GameAccountHandle { unsigned long a; };

}  // namespace OnlineService

namespace Console { namespace Online {
enum MigrationEvent { MigrationEvent_0 };
enum PartySwitchOnlineResult { PartySwitchOnlineResult_0 };
enum PSNSignInStatus { PSNSignInStatus_0 };
enum PhysicalNetworkStatus { PhysicalNetworkStatus_0 };
enum LobbyConnectionEventType { LobbyConnectionEventType_0 };
} }

typedef blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> > BlzString;

#define CB_TREE(SIG) \
    template class blz::rb_tree<blz::set_traits<OnlineService::CallbackHash<SIG> >, \
                                OnlineService::CallbackHashLessThan<OnlineService::CallbackHash<SIG> >, \
                                blz::allocator<OnlineService::CallbackHash<SIG> > >;

CB_TREE(void())
CB_TREE(void(int))
CB_TREE(void(int, int))
CB_TREE(void(Console::Online::MigrationEvent))
CB_TREE(void(Console::Online::PartySwitchOnlineResult))
CB_TREE(void(int, Console::Online::PSNSignInStatus))
CB_TREE(void(Console::Online::PhysicalNetworkStatus))
CB_TREE(void(int, Console::Online::LobbyConnectionEventType))
CB_TREE(void(BlzString))
CB_TREE(void(OnlineService::AchievementsInterface*, OnlineService::GameAccountHandle, unsigned long))
CB_TREE(void(OnlineService::AchievementsInterface*, OnlineService::GameAccountHandle))
