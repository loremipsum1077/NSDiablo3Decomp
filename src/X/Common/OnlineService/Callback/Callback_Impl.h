// Original file: X/Common/OnlineService/Callback/Callback_Impl.h
// Subset: OnlineService::Callback<Sig>, the eight-byte handle on a refcounted heap node, with
// the members this tree inlines so far. Included from the end of Callback.h.
//
//     node_t  +0x00 call_type m_call
//             +0x08 bind_destructor_t m_destructor
//             +0x10 int32 m_refCount
//             +0x18 the bound functor, in place

#pragma once

#include <new>
#include <stddef.h>

#include "../../../../BlizzardCore/Include/Blizzard/Sys/Atomic.h"
#include "../../../Sigma/SigmaTypes.h"
#include "Callback.h"

namespace OnlineService {

template <typename Sig> class Callback : public internal::callback_base<d3boost::function_traits<Sig>::arity, Callback<Sig> > {  // size 0x8
public:
    typedef typename internal::callback_fn_type<Sig>::type call_type;

    struct node_t {  // size 0x18
        call_type m_call;  // 0x00
        bind_destructor_t m_destructor;  // 0x08
        int32 m_refCount;  // 0x10
    };

    template <typename F> struct gen_destructor {
        static void Del(void * p) {  // line 96
            static_cast<F *>(p)->~F();
        }
    };

    typedef Sig fn_type;
    friend struct internal::callback_base<d3boost::function_traits<Sig>::arity, Callback<Sig> >;

    Callback(const Callback & cb) : m_impl(cb.m_impl) { AddRef(m_impl); } // line 152
    Callback() : m_impl(NULL) {}  // line 127

    template <typename F> Callback(const F & bind) {  // line 134
        m_impl = InitFunctor(bind);
        m_impl->m_call = &internal::call_callback<Sig, F>;
        m_impl->m_destructor = &gen_destructor<F>::Del;
    }

    // line 144: a plain function; the node holds a pointer to it.
    Callback(fn_type & fn) {
        m_impl = InitFunctor(&fn);
        m_impl->m_call = &internal::call_callback<Sig, fn_type *>;
        m_impl->m_destructor = &gen_destructor<fn_type *>::Del;
    }

    ~Callback() { Release(m_impl); }
    operator bool() const { return m_impl != NULL && m_impl->m_call != NULL; } // line 233
    void Clear() { Release(m_impl); } // line 211

    // Original line 165; assign the shared node, retaining before releasing.
    Callback & operator=(const Callback & cb) {
        if (m_impl != cb.m_impl) {
            AddRef(cb.m_impl);
            Release(m_impl);
            m_impl = cb.m_impl;
        }
        return *this;
    }

    template <typename F> Callback & operator=(const F & f) {  // line 185
        Release(m_impl);
        m_impl = InitFunctor(f);
        m_impl->m_call = &internal::call_callback<Sig, F>;
        m_impl->m_destructor = &gen_destructor<F>::Del;
        return *this;
    }

    // line 195: assign a plain function; the node holds a pointer to it.
    Callback & operator=(fn_type & fn) {
        Release(m_impl);
        m_impl = InitFunctor(&fn);
        m_impl->m_call = &internal::call_callback<Sig, fn_type *>;
        m_impl->m_destructor = &gen_destructor<fn_type *>::Del;
        return *this;
    }

protected:
    void *const bind() const { return m_impl + 1; } // line 249
    call_type call() const { return m_impl->m_call; } // line 255

private:
    template <typename F> static node_t * InitFunctor(const F & f) {  // line 262
        size_t size = sizeof(F);
        node_t * ptr = Alloc(size);
        ::new (ptr + 1) F(f);
        return ptr;
    }

    static node_t * Alloc(size_t a_size) {  // line 274
        node_t * ptr = (node_t *)new char[sizeof(node_t) + a_size];
        ptr->m_call = NULL;
        ptr->m_refCount = 1;
        return ptr;
    }

    static void Free(node_t * a_node) {  // line 284
        a_node->m_destructor(a_node + 1);
        delete[] (char *)a_node;
    }

    static void AddRef(node_t * p) {  // line 295
        if (p != NULL)
            Blizzard::Lock::Atomic::Increment(&p->m_refCount);
    }

    static void Release(node_t *& a_node) {  // line 305
        if (a_node != NULL) {
            if (Blizzard::Lock::Atomic::Decrement(&a_node->m_refCount) == 0) {
                Free(a_node);
            }
            a_node = NULL;
        }
    }

    node_t * m_impl;  // 0x00
};

// Original lines 322, 387 and 403; explicit base cast prevents selecting the
// functor constructor when retaining an existing callback node.
template <typename FT> class CallbackHash : public Callback<FT> {
public:
    CallbackHash() : m_hashKey(0), m_addressKey(NULL) {}
    template <typename F> CallbackHash(const F & bind) : Callback<FT>(bind),
        m_hashKey(bind.HashKey()), m_addressKey(bind.AddressKey()) {}
    // line 395: a plain function, keyed by its address.
    CallbackHash(FT & fn) : Callback<FT>(fn),
        m_hashKey(internal::hash_function_address((const void *)&fn)), m_addressKey((void *)&fn) {}
    CallbackHash(const CallbackHash & cb) : Callback<FT>(static_cast<const Callback<FT> &>(cb)),
        m_hashKey(cb.m_hashKey), m_addressKey(cb.m_addressKey) {}
    uint32 HashKey() const { return m_hashKey; } // line 353
protected:
    uint32 m_hashKey;
    void * m_addressKey;
};

template <typename _Kty> struct CallbackHashLessThan {
    bool operator()(const _Kty & left, const _Kty & right) const { // line 40
        return left.HashKey() < right.HashKey();
    }
};

}  // namespace OnlineService
