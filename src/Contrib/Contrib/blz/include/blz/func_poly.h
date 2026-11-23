#pragma once

// blz::function: a type-erased callable. Layout from the DWARF: m_storage
// (4 pointers, 0x00) holds the callable in place when it fits, otherwise it
// lives in a block from blzext::alloc; m_impl (0x20) points at either (null
// when empty). Everything is told apart by m_impl == m_storage.
//
// impl_base's virtual slots (recorded): 0/1 ~impl_base, 2 call, 3 destroy,
// 4 allocate, 5 clone_to, 6 move_to, 7 get. Line numbers are the DWARF's
// where it records one; bodies of lines it does not show (the size test in
// _set, the enable_if on the converting constructor) are reconstructed from
// the binary.

#include "config.h"
#include "func_bind.h"
#include "func_invoke.h"
#include "mem_alloc.h"
#include "type_traits.h"

#include <new>

namespace blz {

template <typename>
class function;

// line 51
template <typename R, typename... Args>
class function<R(Args...)> {
    class impl_base {
    public:
        // line 57
        virtual ~impl_base() {}
        virtual R call(Args&&...) = 0;
        virtual void destroy(bool) = 0;
        virtual impl_base* allocate() = 0;
        virtual void clone_to(impl_base*) = 0;
        virtual void move_to(impl_base*) = 0;
        virtual const void* get() const = 0;
    };

public:
    // line 76
    function() : m_impl(nullptr) {}

    function(decltype(nullptr)) : m_impl(nullptr) {}

    // line 95
    function(const function& f) {
        _set(f);
    }

    // line 101
    function(function&& f) {
        _set(blz::move(f));
    }

    // line 109
    template <typename F, typename = typename enable_if<!is_same<typename decay<F>::type, function>::value>::type>
    function(F f) {
        _set(blz::move(f));
    }

    // line 115
    ~function() {
        clear();
    }

    // line 121
    function& operator=(const function& f) {
        if (this != &f) {
            clear();
            _set(f);
        }
        return *this;
    }

    // line 155
    function& operator=(decltype(nullptr)) {
        clear();
        return *this;
    }

    // line 231
    explicit operator bool() const {
        return m_impl != nullptr;
    }

    // line 245
    R operator()(Args... args) const {
        return m_impl->call(blz::forward<Args>(args)...);
    }

    // line 290
    void clear() {
        if (m_impl) {
            m_impl->destroy(m_impl != reinterpret_cast<impl_base*>(m_storage));
            m_impl = nullptr;
        }
    }

    bool empty() const {
        return m_impl == nullptr;
    }

private:
    // line 307
    void _set(const function& f) {
        if (f.m_impl) {
            if (f.m_impl == reinterpret_cast<const impl_base*>(f.m_storage)) {
                m_impl = reinterpret_cast<impl_base*>(m_storage);
            } else {
                m_impl = f.m_impl->allocate();
            }
            f.m_impl->clone_to(m_impl);
        } else {
            m_impl = nullptr;
        }
    }

    // line 325
    void _set(function&& f) {
        if (f.m_impl) {
            if (f.m_impl == reinterpret_cast<const impl_base*>(f.m_storage)) {
                m_impl = reinterpret_cast<impl_base*>(m_storage);
                f.m_impl->move_to(m_impl);
                f.m_impl->~impl_base();
            } else {
                m_impl = f.m_impl;
            }
            f.m_impl = nullptr;
        } else {
            m_impl = nullptr;
        }
    }

    template <typename F>
    class impl;

    // line 350
    template <typename F>
    void _set(F f) {
        typedef impl<F> impl_type;
        if (sizeof(impl_type) <= sizeof(m_storage)) {
            m_impl = reinterpret_cast<impl_base*>(m_storage);
        } else {
            m_impl = static_cast<impl_base*>(blzext::alloc(sizeof(impl_type)));
        }
        new (m_impl) impl_type(blz::move(f));
    }

    // line 371
    template <typename F>
    class impl : public impl_base {
    public:
        // line 377
        impl(const F& f) : m_func(f) {}

        // line 378
        impl(F&& f) : m_func(blz::move(f)) {}

        // line 380
        virtual R call(Args&&... args) {
            return blz::invoke(m_func, blz::forward<Args>(args)...);
        }

        // line 385
        virtual void destroy(bool delete_this) {
            this->~impl();
            if (delete_this)
                blzext::free(this);
        }

        // line 392
        virtual impl_base* allocate() {
            return static_cast<impl*>(blzext::alloc(sizeof(impl)));
        }

        // line 397
        virtual void clone_to(impl_base* x) {
            new (x) impl(m_func);
        }

        // line 402
        virtual void move_to(impl_base* x) {
            new (x) impl(blz::move(m_func));
        }

        // line 407
        virtual const void* get() const {
            return &m_func;
        }

    private:
        impl(impl&);
        impl& operator=(impl&);

        F m_func;
    };

    void* m_storage[4];
    impl_base* m_impl;
};

}  // namespace blz
