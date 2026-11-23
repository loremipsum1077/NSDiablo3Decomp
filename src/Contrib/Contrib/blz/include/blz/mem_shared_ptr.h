#pragma once

// blz::shared_ptr. A shared_ptr is {pointer, manager} (0x10); the manager
// (_shared_ptr_manager, vtable + two counts) is allocated with `new` together
// with the object by make_shared (_shared_ptr_emplace, which stores the object
// inline at 0x10). Recorded virtual slots of the manager: 0/1 destructor,
// 2 get_deleter, 3 destroy_managed, 4 destroy_this. Counts start at 1/1.
// The use count reaching zero destroys the object and drops the one weak
// reference the strong owners hold together; the weak count reaching zero
// frees the block. Line numbers are the DWARF's; statements that sit on
// lines it does not name are placed to match the binary.
//
// Not recorded anywhere (declared by the DWARF but never inlined): weak_ptr,
// weak_to_shared, set_weak_this, use_count, unique, reset, operator=(const&).
// They are written the plain way and are not verified.

#include "config.h"
#include "interlocked.h"
#include "type_traits.h"
#include "util_swap.h"

#include <new>

namespace blz {

// line 28
struct _shared_ptr_manager {
    // line 35
    _shared_ptr_manager() : m_use_count(1), m_weak_count(1) {}

    // line 40
    virtual ~_shared_ptr_manager() {}

    // line 43
    void add_shared() {
        interlocked_increment(&m_use_count);
    }

    void add_weak() {
        interlocked_increment(&m_weak_count);
    }

    // Not recorded: take a strong reference unless the count already hit zero.
    bool weak_to_shared() {
        int32_t n = m_use_count;
        while (n != 0) {
            if (__atomic_compare_exchange_n(&m_use_count, &n, n + 1, false, __ATOMIC_SEQ_CST,
                                            __ATOMIC_SEQ_CST))
                return true;
        }
        return false;
    }

    // line 66
    void release_shared() {
        if (interlocked_decrement(&m_use_count) == 0) {
            destroy_managed();
            release_weak();
        }
    }

    // line 75
    void release_weak() {
        if (interlocked_decrement(&m_weak_count) == 0)
            destroy_this();
    }

    virtual void* get_deleter() = 0;
    virtual void destroy_managed() = 0;
    virtual void destroy_this() = 0;

    int32_t m_use_count;
    int32_t m_weak_count;
};
static_assert(sizeof(_shared_ptr_manager) == 0x10, "_shared_ptr_manager");

// line 235
template <typename T>
struct _shared_ptr_emplace : _shared_ptr_manager {
    typedef typename aligned_storage<sizeof(T), alignof(T)>::type storage_type;

    // line 241
    template <typename... Args>
    _shared_ptr_emplace(Args&&... args) {
        new (&m_storage) T(blz::forward<Args>(args)...);
    }

    // line 325
    virtual void* get_deleter() {
        return nullptr;
    }

    // line 331
    virtual void destroy_managed() {
        T* data = reinterpret_cast<T*>(&m_storage);
        data->~T();
    }

    // line 338
    virtual void destroy_this() {
        delete this;
    }

    storage_type m_storage;
};

template <typename T>
class shared_ptr;

// line 675
template <typename T, bool>
class _shared_ptr_base {
public:
    _shared_ptr_base() : m_pointer(nullptr) {}

    // line 681
    _shared_ptr_base(T* pointer) : m_pointer(pointer) {}

    // line 685
    typename add_lvalue_reference<T>::type operator*() const {
        return *m_pointer;
    }

    // line 690
    T* operator->() const {
        return m_pointer;
    }

protected:
    T* m_pointer;
};

// line 726
template <typename T>
class shared_ptr : public _shared_ptr_base<T, false> {
    typedef typename remove_extent<T>::type element_type;

    template <typename U>
    friend class shared_ptr;

    template <typename U, typename... Args>
    friend typename enable_if<true, shared_ptr<U> >::type make_shared(Args&&... args);

public:
    // line 736
    shared_ptr() : _shared_ptr_base<T, false>(), m_manager(nullptr) {}

    // line 810
    shared_ptr(const shared_ptr& r) : _shared_ptr_base<T, false>(r.m_pointer), m_manager(r.m_manager) {
        if (m_manager)
            m_manager->add_shared();
    }

    // line 820
    template <typename U>
    shared_ptr(const shared_ptr<U>& r,
               typename enable_if<is_convertible<U*, T*>::value>::type* = 0)
        : _shared_ptr_base<T, false>(r.m_pointer), m_manager(r.m_manager) {
        if (m_manager)
            m_manager->add_shared();
    }

    // line 829
    shared_ptr(shared_ptr&& r) : _shared_ptr_base<T, false>(r.m_pointer), m_manager(r.m_manager) {
        r.m_pointer = nullptr;
        r.m_manager = nullptr;
    }

    // line 896
    shared_ptr(decltype(nullptr)) : _shared_ptr_base<T, false>(nullptr), m_manager(nullptr) {}

    // line 904
    ~shared_ptr() {
        if (m_manager)
            m_manager->release_shared();
    }

    shared_ptr& operator=(const shared_ptr& r) {
        shared_ptr(r).swap(*this);
        return *this;
    }

    // line 927
    shared_ptr& operator=(shared_ptr&& r) {
        shared_ptr(blz::move(r)).swap(*this);
        return *this;
    }

    // line 951
    void swap(shared_ptr& r) {
        blz::swap(this->m_pointer, r.m_pointer);
        blz::swap(m_manager, r.m_manager);
    }

    void reset() {
        shared_ptr().swap(*this);
    }

    // line 988
    element_type* get() const {
        return this->m_pointer;
    }

    long use_count() const {
        return m_manager ? m_manager->m_use_count : 0;
    }

    bool unique() const {
        return use_count() == 1;
    }

    explicit operator bool() const {
        return this->m_pointer != nullptr;
    }

private:
    _shared_ptr_manager* m_manager;
};

// line 1369
template <typename T, typename... Args>
inline typename enable_if<true, shared_ptr<T> >::type make_shared(Args&&... args) {
    _shared_ptr_emplace<T>* p = new _shared_ptr_emplace<T>(blz::forward<Args>(args)...);
    shared_ptr<T> r;
    r.m_pointer = reinterpret_cast<T*>(&p->m_storage);
    r.m_manager = p;
    return r;
}

// line 1595
template <typename T, typename U>
inline bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b) {
    return a.get() == b.get();
}

}  // namespace blz
