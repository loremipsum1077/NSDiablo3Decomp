#pragma once

// blz::atomic. Layout: the value itself (`x`, public in _atomic_base). The
// bodies are the compiler's __atomic builtins (the type-generic forms, as float is atomic too) at the order passed in (the
// binary shows ldarb / stlrb / ldaxr+stlxr loops for seq_cst). Recorded inline
// helpers sit two lines below their declaration (95 -> 97, 125 -> 127,
// 214 -> 216, 234 -> 236, 254 -> 256 ...): each is a single statement.
// is_lock_free / load / store / compare_exchange_* / fetch_* other than
// fetch_or are declared by the DWARF but never inlined anywhere, so they are
// written the plain way.

#include "config.h"
#include "internal/atomic_types.h"
#include "type_traits.h"

namespace blz {

// line 66
template <typename T, bool>
struct _atomic_base {
    T x;

    _atomic_base() {}

    static const bool is_always_lock_free = true;

    bool is_lock_free() const {
        return true;
    }

    // line 95
    operator T() const {
        T r;
        __atomic_load(&x, &r, __ATOMIC_SEQ_CST);
        return r;
    }

    void store(T d, memory_order m = memory_order_seq_cst) {
        __atomic_store(&x, &d, m);
    }

    T load(memory_order m = memory_order_seq_cst) const {
        T r;
        __atomic_load(&x, &r, m);
        return r;
    }

    // line 125
    T exchange(T d, memory_order m = memory_order_seq_cst) {
        T r;
        __atomic_exchange(&x, &d, &r, m);
        return r;
    }

    bool compare_exchange_weak(T& expected, T d, memory_order s, memory_order f) {
        return __atomic_compare_exchange(&x, &expected, &d, true, s, f);
    }

    bool compare_exchange_strong(T& expected, T d, memory_order s, memory_order f) {
        return __atomic_compare_exchange(&x, &expected, &d, false, s, f);
    }

    bool compare_exchange_weak(T& expected, T d, memory_order m = memory_order_seq_cst) {
        return __atomic_compare_exchange(&x, &expected, &d, true, m, m);
    }

    bool compare_exchange_strong(T& expected, T d, memory_order m = memory_order_seq_cst) {
        return __atomic_compare_exchange(&x, &expected, &d, false, m, m);
    }
};

// line 174
template <typename T>
struct _atomic_base<T, true> : _atomic_base<T, false> {
    T fetch_add(T v, memory_order m = memory_order_seq_cst) {
        return __atomic_fetch_add(&this->x, v, m);
    }

    T fetch_sub(T v, memory_order m = memory_order_seq_cst) {
        return __atomic_fetch_sub(&this->x, v, m);
    }

    T fetch_and(T v, memory_order m = memory_order_seq_cst) {
        return __atomic_fetch_and(&this->x, v, m);
    }

    // line 214
    T fetch_or(T v, memory_order m = memory_order_seq_cst) {
        return __atomic_fetch_or(&this->x, v, m);
    }

    T fetch_xor(T v, memory_order m = memory_order_seq_cst) {
        return __atomic_fetch_xor(&this->x, v, m);
    }

    // line 234
    T operator++(int) {
        return __atomic_fetch_add(&this->x, 1, __ATOMIC_SEQ_CST);
    }

    T operator--(int) {
        return __atomic_fetch_sub(&this->x, 1, __ATOMIC_SEQ_CST);
    }

    // line 254
    T operator++() {
        return __atomic_add_fetch(&this->x, 1, __ATOMIC_SEQ_CST);
    }

    // line 264
    T operator--() {
        return __atomic_sub_fetch(&this->x, 1, __ATOMIC_SEQ_CST);
    }

    // line 274
    T operator+=(T v) {
        return __atomic_add_fetch(&this->x, v, __ATOMIC_SEQ_CST);
    }

    // line 284
    T operator-=(T v) {
        return __atomic_sub_fetch(&this->x, v, __ATOMIC_SEQ_CST);
    }

    T operator&=(T v) {
        return __atomic_and_fetch(&this->x, v, __ATOMIC_SEQ_CST);
    }

    // line 304
    T operator|=(T v) {
        return __atomic_or_fetch(&this->x, v, __ATOMIC_SEQ_CST);
    }

    T operator^=(T v) {
        return __atomic_xor_fetch(&this->x, v, __ATOMIC_SEQ_CST);
    }
};

// line 323
template <typename T>
struct atomic : _atomic_base<T, is_integral<T>::value && !is_same<T, bool>::value> {
    atomic() {}

    // line 334
    atomic(T d) {
        this->x = d;
    }

private:
    atomic(const atomic&);
    atomic& operator=(const atomic&);

public:
    // line 349
    T operator=(T d) {
        __atomic_store(&this->x, &d, __ATOMIC_SEQ_CST);
        return d;
    }
};

typedef atomic<bool> atomic_bool;
typedef atomic<int> atomic_int32_t;
typedef atomic<unsigned int> atomic_uint32_t;

}  // namespace blz
