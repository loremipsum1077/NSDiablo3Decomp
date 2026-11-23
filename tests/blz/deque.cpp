// Template checks for blz/deque.h.
#include "Contrib/Contrib/blz/include/blz/deque.h"
#include "Contrib/Contrib/blz/include/blz/func_poly.h"

// Stand-in for bdReference (DemonWare): the refcount lives at +8 of the
// referenced object; copying adds a reference.
class bdReferencable {
public:
    virtual ~bdReferencable();
    int m_refCount;
};
namespace Console { namespace Online {
class MailInternal : public bdReferencable { public: int pad; };
} }
template <typename T> class bdReference {
public:
    bdReference() : m_ptr(0) {}
    bdReference(const bdReference& o) : m_ptr(o.m_ptr) {
        if (m_ptr)
            __atomic_add_fetch(&m_ptr->m_refCount, 1, __ATOMIC_SEQ_CST);
    }
    ~bdReference() {
        if (m_ptr) {
            if (__atomic_sub_fetch(&m_ptr->m_refCount, 1, __ATOMIC_SEQ_CST) == 0) {
                delete m_ptr;
                m_ptr = 0;
            }
        }
    }
protected:
    T* m_ptr;
};

typedef bdReference<Console::Online::MailInternal> MailRef;
typedef blz::function<void()> VoidFunction;

template void blz::deque<MailRef>::push_back(const MailRef&);
template void blz::deque<VoidFunction>::push_back(VoidFunction&&);
