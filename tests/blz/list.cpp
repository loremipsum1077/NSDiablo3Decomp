// Template checks for blz/list.h.
#include "Contrib/Contrib/blz/include/blz/list.h"

struct SceneCommonData;
struct RActorID { unsigned a; };

// Stand-in for bdReference (DemonWare). The refcount lives at +8 of the
// referenced object, and the object is released (virtual deleting destructor, vtable slot 1).
class bdReferencable {
public:
    virtual ~bdReferencable();
    int m_refCount;
};
namespace Console { namespace Online {
class Party : public bdReferencable { public: int pad; };
} }
class bdCommonAddr : public bdReferencable { public: int pad; };
template <typename T> class bdReference {
public:
    bdReference() : m_ptr(0) {}
    ~bdReference() {
        if (m_ptr) {
            if (__atomic_sub_fetch(&m_ptr->m_refCount, 1, __ATOMIC_SEQ_CST) == 0) {
                delete m_ptr;
                m_ptr = 0;
            }
        }
    }
    bool operator==(const bdReference& o) const { return m_ptr == o.m_ptr; }
    bool operator<(const bdReference& o) const { return m_ptr < o.m_ptr; }
protected:
    T* m_ptr;
};

namespace Console { namespace Online {
// PartyInfo is 0x618 bytes. Its destructor releases the address reference at
// 0x488 and calls the destructors of bdSecurityID (0x490) and bdSecurityKey
// (0x498), which are out of line (stand-ins here).
struct bdSecurityID { char c[8]; ~bdSecurityID(); };
struct bdSecurityKey { char c[16]; ~bdSecurityKey(); };
struct PartyInfo {
    char m_pad0[0x488];
    bdReference<bdCommonAddr> m_ptAddress;
    bdSecurityID m_tSecID;
    bdSecurityKey m_tSecKey;
    char m_pad1[0x618 - 0x4a8];
};
} }

template class blz::list<unsigned int>;
template class blz::list<bdReference<Console::Online::Party> >;

typedef int (*SceneCompare)(const void*, const void*);
template void blz::list<SceneCommonData*>::sort<SceneCompare>(SceneCompare);
template void blz::list<SceneCommonData*>::merge<SceneCompare>(blz::list<SceneCommonData*>&, SceneCompare);

typedef bool (*PartyCompare)(const Console::Online::PartyInfo&, const Console::Online::PartyInfo&);
template void blz::list<Console::Online::PartyInfo>::sort<PartyCompare>(PartyCompare);
