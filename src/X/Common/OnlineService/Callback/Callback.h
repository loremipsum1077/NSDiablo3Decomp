// Original file: X/Common/OnlineService/Callback/Callback.h
// Subset: the free-function binds with zero or one stored argument and the call thunk that every Callback node
// points at. The DWARF has only instantiations of these templates; the bodies follow the
// sibling 1.0 decomp (generated there from the same DWARF) and the callers in this tree.

#pragma once

#include "../../../Sigma/SigmaTypes.h"
#include "d3boost/detail/call_traits.hpp"
#include "d3boost/type_traits/function_traits.hpp"

// X/Sigma/DataStructures/XBaseMap.h; the subscriber hash of a method bind uses it.
template <typename ARG_KEY> uint32 XHashKey(ARG_KEY key);

namespace OnlineService {

template <typename Sig> class Callback;

typedef void (*bind_destructor_t)(void *);

namespace internal {

// Invented helper name: the original function binds hash the low four address
// bytes with FNV-1a (ShowPartyDisplay, 0x15c7dc onward).
inline uint32 hash_function_address(const void * address) {
    uint32 key = (uint32)(unsigned long)address;
    uint32 hash = 0x811c9dc5u;
    for (unsigned int i = 0; i < 4; ++i) {
        hash = (hash ^ (key & 0xff)) * 0x01000193u;
        key >>= 8;
    }
    return hash;
}


// A free function R Fn(A1) with no leading argument stored; operator() takes A1.
template <typename R, typename A1, R (*Fn)(A1)> struct function_bind_0_1 {
    uint32 HashKey() const { return hash_function_address((const void *)Fn); }
    void * AddressKey() const { return (void *)Fn; }
    R operator()(typename d3boost::call_traits<A1>::param_type a1) const {  // line 293
        return Fn(a1);
    }
};

// A free function R Fn(A1, A2) with nothing stored; operator() takes A1, A2
// (X/Client/UserInterface/Console/ConsoleScreenManager.cpp subscribes these).
template <typename R, typename A1, typename A2, R (*Fn)(A1, A2)> struct function_bind_0_2 {
    uint32 HashKey() const { return hash_function_address((const void *)Fn); }
    void * AddressKey() const { return (void *)Fn; }
    R operator()(typename d3boost::call_traits<A1>::param_type a1,
                 typename d3boost::call_traits<A2>::param_type a2) const {
        return Fn(a1, a2);
    }
};

// Line 102. A free function R Fn(A1) with A1 stored; operator() takes nothing.
template <typename R, typename A1, R (*Fn)(A1)> struct function_bind_1_0 {
    R operator()() const {  // line 107
        return Fn(m_arg1);
    }
    function_bind_1_0(typename d3boost::call_traits<A1>::param_type arg1) : m_arg1(arg1) {}  // line 111
    A1 m_arg1;
};

// Line 1458. Binds the only argument of a free function R Fn(T1).
template <typename R, typename T1>
struct function_binder_1 {
    template <R (*Fn)(T1)>
    static function_bind_1_0<R, T1, Fn> bind(typename d3boost::call_traits<T1>::param_type arg1) {
        return function_bind_1_0<R, T1, Fn>(arg1);  // line 1466
    }
};

// Line 316. A free function R Fn(A1, A2) with A1 stored; operator() takes A2.
template <typename R, typename A1, typename A2, R (*Fn)(A1, A2)> struct function_bind_1_1 {
    R operator()(typename d3boost::call_traits<A2>::param_type param1) const {  // line 322
        return Fn(m_arg1, param1);
    }
    function_bind_1_1(typename d3boost::call_traits<A1>::param_type arg1) : m_arg1(arg1) {}  // line 326
    A1 m_arg1;
};

// Line 573. A free function R Fn(A1, A2, P1, P2) with A1 and A2 stored; operator() takes P1, P2.
template <typename R, typename A1, typename A2, typename P1, typename P2, R (*Fn)(A1, A2, P1, P2)>
struct function_bind_2_2 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2) const {  // line 580
        return Fn(m_arg1, m_arg2, param1, param2);
    }
    function_bind_2_2(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2)  // line 584
        : m_arg1(arg1), m_arg2(arg2) {}
    A1 m_arg1;
    A2 m_arg2;
};

// Line 352. A free function R Fn(A1, A2, P1) with A1 and A2 stored; operator() takes P1
// (X/Common/Game.cpp ShowSystemLocaleAOCOffer binds OnCloseMissingSystemLocaleOffer this way).
template <typename R, typename A1, typename A2, typename P1, R (*Fn)(A1, A2, P1)>
struct function_bind_2_1 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1) const {  // line 358
        return Fn(m_arg1, m_arg2, param1);
    }
    function_bind_2_1(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2)  // line 362
        : m_arg1(arg1), m_arg2(arg2) {}
    A1 m_arg1;
    A2 m_arg2;
};

// A free function R Fn(A1, P1, P2) with A1 stored; operator() takes P1, P2
// (X/Client/UserInterface/Console/ConsoleScreenManager.cpp binds its transition-out handlers this way).
template <typename R, typename A1, typename P1, typename P2, R (*Fn)(A1, P1, P2)>
struct function_bind_1_2 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2) const {
        return Fn(m_arg1, param1, param2);
    }
    function_bind_1_2(typename d3boost::call_traits<A1>::param_type arg1) : m_arg1(arg1) {}
    A1 m_arg1;
};

// A free function R Fn(A1, P1, P2, P3) with A1 stored; operator() takes P1, P2, P3
// (X/Client/UserInterface/Console/UIConsoleGameSettings.cpp binds sOnGetChallengeRiftData this way).
template <typename R, typename A1, typename P1, typename P2, typename P3, R (*Fn)(A1, P1, P2, P3)>
struct function_bind_1_3 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2,
                 typename d3boost::call_traits<P3>::param_type param3) const {
        return Fn(m_arg1, param1, param2, param3);
    }
    function_bind_1_3(typename d3boost::call_traits<A1>::param_type arg1) : m_arg1(arg1) {}
    A1 m_arg1;
};

// A free function R Fn(A1, A2, P1, P2, P3) with A1 and A2 stored; operator() takes P1, P2, P3
// (X/Client/UserInterface/Console/UIConsoleLeaderboardInspect.cpp binds sOnGetChallengeRiftData this way).
template <typename R, typename A1, typename A2, typename P1, typename P2, typename P3, R (*Fn)(A1, A2, P1, P2, P3)>
struct function_bind_2_3 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2,
                 typename d3boost::call_traits<P3>::param_type param3) const {
        return Fn(m_arg1, m_arg2, param1, param2, param3);
    }
    function_bind_2_3(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2)
        : m_arg1(arg1), m_arg2(arg2) {}
    A1 m_arg1;
    A2 m_arg2;
};

// A free function R Fn() with nothing stored (UIConsoleMainMenu.cpp subscribes these).
template <typename R, R (*Fn)()> struct function_bind_0_0 {
    uint32 HashKey() const { return hash_function_address((const void *)Fn); }
    void * AddressKey() const { return (void *)Fn; }
    R operator()() const {
        return Fn();
    }
};

// Defined below; a stored const-reference argument is kept by value (OnlineActions.cpp binds a UIID).
template <typename T> struct bind_storage;

// A free function R Fn(A1, A2, A3, P1) with A1..A3 stored; operator() takes P1.
template <typename R, typename A1, typename A2, typename A3, typename P1, R (*Fn)(A1, A2, A3, P1)>
struct function_bind_3_1 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1) const {
        return Fn(m_arg1, m_arg2, m_arg3, param1);
    }
    function_bind_3_1(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2,
                      typename d3boost::call_traits<A3>::param_type arg3)
        : m_arg1(arg1), m_arg2(arg2), m_arg3(arg3) {}
    A1 m_arg1;
    A2 m_arg2;
    typename bind_storage<A3>::type m_arg3;
};

// A free function R Fn(A1, A2) with both stored; operator() takes nothing
// (OnlineActions.cpp hands these to Utilities::ShowNX64Upsell).
template <typename R, typename A1, typename A2, R (*Fn)(A1, A2)>
struct function_bind_2_0 {
    R operator()() const {
        return Fn(m_arg1, m_arg2);
    }
    function_bind_2_0(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2)
        : m_arg1(arg1), m_arg2(arg2) {}
    A1 m_arg1;
    typename bind_storage<A2>::type m_arg2;
};

// A free function R Fn(A1, A2, A3) with all three stored; operator() takes nothing.
template <typename R, typename A1, typename A2, typename A3, R (*Fn)(A1, A2, A3)>
struct function_bind_3_0 {
    R operator()() const {
        return Fn(m_arg1, m_arg2, m_arg3);
    }
    function_bind_3_0(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2,
                      typename d3boost::call_traits<A3>::param_type arg3)
        : m_arg1(arg1), m_arg2(arg2), m_arg3(arg3) {}
    A1 m_arg1;
    typename bind_storage<A2>::type m_arg2;  // DWARF get_bound_type<A2>::type: by value (OnlineActions.cpp binds a PartyConnectionInfo)
    typename bind_storage<A3>::type m_arg3;
};

// A free function R Fn(A1, A2, A3, P1, P2) with A1..A3 stored; operator() takes P1, P2.
template <typename R, typename A1, typename A2, typename A3, typename P1, typename P2, R (*Fn)(A1, A2, A3, P1, P2)>
struct function_bind_3_2 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2) const {
        return Fn(m_arg1, m_arg2, m_arg3, param1, param2);
    }
    function_bind_3_2(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2,
                      typename d3boost::call_traits<A3>::param_type arg3)
        : m_arg1(arg1), m_arg2(arg2), m_arg3(arg3) {}
    A1 m_arg1;
    A2 m_arg2;
    A3 m_arg3;
};

// A free function R Fn(A1, A2, A3, A4, P1) with A1..A4 stored; operator() takes P1.
template <typename R, typename A1, typename A2, typename A3, typename A4, typename P1, R (*Fn)(A1, A2, A3, A4, P1)>
struct function_bind_4_1 {
    R operator()(typename d3boost::call_traits<P1>::param_type param1) const {
        return Fn(m_arg1, m_arg2, m_arg3, m_arg4, param1);
    }
    function_bind_4_1(typename d3boost::call_traits<A1>::param_type arg1,
                      typename d3boost::call_traits<A2>::param_type arg2,
                      typename d3boost::call_traits<A3>::param_type arg3,
                      typename d3boost::call_traits<A4>::param_type arg4)
        : m_arg1(arg1), m_arg2(arg2), m_arg3(arg3), m_arg4(arg4) {}
    A1 m_arg1;
    A2 m_arg2;
    A3 m_arg3;
    A4 m_arg4;
};

// Line ~1525. Binds the leading argument of a free function R Fn(T1, T2).
template <typename R, typename T1, typename T2>
struct function_binder_2 {
    template <R (*Fn)(T1, T2)>
    static function_bind_1_1<R, T1, T2, Fn> bind(typename d3boost::call_traits<T1>::param_type arg1) {
        return function_bind_1_1<R, T1, T2, Fn>(arg1);
    }
    // Binds both arguments.
    template <R (*Fn)(T1, T2)>
    static function_bind_2_0<R, T1, T2, Fn> bind(typename d3boost::call_traits<T1>::param_type arg1,
                                                 typename d3boost::call_traits<T2>::param_type arg2) {
        return function_bind_2_0<R, T1, T2, Fn>(arg1, arg2);
    }
};

// Line ~1535. Binds the two leading arguments of a free function R Fn(T1, T2, T3).
template <typename R, typename T1, typename T2, typename T3>
struct function_binder_3 {
    template <R (*Fn)(T1, T2, T3)>
    static function_bind_2_1<R, T1, T2, T3, Fn> bind(
        typename d3boost::call_traits<T1>::param_type arg1,
        typename d3boost::call_traits<T2>::param_type arg2) {  // line 1538
        return function_bind_2_1<R, T1, T2, T3, Fn>(arg1, arg2);  // line 1540
    }
    // Binds all three arguments.
    template <R (*Fn)(T1, T2, T3)>
    static function_bind_3_0<R, T1, T2, T3, Fn> bind(
        typename d3boost::call_traits<T1>::param_type arg1,
        typename d3boost::call_traits<T2>::param_type arg2,
        typename d3boost::call_traits<T3>::param_type arg3) {
        return function_bind_3_0<R, T1, T2, T3, Fn>(arg1, arg2, arg3);
    }    // Line 1543. Binds the two leading arguments, for arguments whose types differ from the parameters'.
    template <R (*Fn)(T1, T2, T3), typename A1, typename A2>
    static function_bind_2_1<R, T1, T2, T3, Fn> bind(const A1 & arg1, const A2 & arg2) {
        return function_bind_2_1<R, T1, T2, T3, Fn>(arg1, arg2);  // line 1545
    }
};

// Line 1566. Names the function_bind the call site needs and binds the leading
// arguments of a free function R Fn(T1, T2, T3, T4).
template <typename R, typename T1, typename T2, typename T3, typename T4>
struct function_binder_4 {
    template <R (*Fn)(T1, T2, T3, T4)>
    static function_bind_2_2<R, T1, T2, T3, T4, Fn> bind(
        typename d3boost::call_traits<T1>::param_type arg1,
        typename d3boost::call_traits<T2>::param_type arg2) {  // line 1584
        return function_bind_2_2<R, T1, T2, T3, T4, Fn>(arg1, arg2);  // line 1586
    }
    // Line 1594. Binds the three leading arguments.
    template <R (*Fn)(T1, T2, T3, T4)>
    static function_bind_3_1<R, T1, T2, T3, T4, Fn> bind(
        typename d3boost::call_traits<T1>::param_type arg1,
        typename d3boost::call_traits<T2>::param_type arg2,
        typename d3boost::call_traits<T3>::param_type arg3) {
        return function_bind_3_1<R, T1, T2, T3, T4, Fn>(arg1, arg2, arg3);  // line 1596
    }
};

// Line ~1640. Binds the four leading arguments of a free function R Fn(T1, T2, T3, T4, T5).
template <typename R, typename T1, typename T2, typename T3, typename T4, typename T5>
struct function_binder_5 {
    // Line 1640. Binds the two leading arguments (UILeaderboardInspect::Console::OpenForChallengeRift).
    template <R (*Fn)(T1, T2, T3, T4, T5)>
    static function_bind_2_3<R, T1, T2, T3, T4, T5, Fn> bind(
        typename d3boost::call_traits<T1>::param_type arg1,
        typename d3boost::call_traits<T2>::param_type arg2) {
        return function_bind_2_3<R, T1, T2, T3, T4, T5, Fn>(arg1, arg2);  // line 1642
    }
    template <R (*Fn)(T1, T2, T3, T4, T5)>
    static function_bind_4_1<R, T1, T2, T3, T4, T5, Fn> bind(
        typename d3boost::call_traits<T1>::param_type arg1,
        typename d3boost::call_traits<T2>::param_type arg2,
        typename d3boost::call_traits<T3>::param_type arg3,
        typename d3boost::call_traits<T4>::param_type arg4) {
        return function_bind_4_1<R, T1, T2, T3, T4, T5, Fn>(arg1, arg2, arg3, arg4);  // line 1662
    }
    // Line 1665. The same, for arguments whose types differ from the parameters'.
    template <R (*Fn)(T1, T2, T3, T4, T5), typename A1, typename A2, typename A3, typename A4>
    static function_bind_4_1<R, T1, T2, T3, T4, T5, Fn> bind(const A1 & arg1, const A2 & arg2, const A3 & arg3, const A4 & arg4) {
        return function_bind_4_1<R, T1, T2, T3, T4, T5, Fn>(arg1, arg2, arg3, arg4);  // line 1667
    }
    // Line 1650. Binds the three leading arguments (UIHeroCreate::Console::OnAccept).
    template <R (*Fn)(T1, T2, T3, T4, T5)>
    static function_bind_3_2<R, T1, T2, T3, T4, T5, Fn> bind(
        typename d3boost::call_traits<T1>::param_type arg1,
        typename d3boost::call_traits<T2>::param_type arg2,
        typename d3boost::call_traits<T3>::param_type arg3) {
        return function_bind_3_2<R, T1, T2, T3, T4, T5, Fn>(arg1, arg2, arg3);  // line 1652
    }
};

// Invented helpers for the member-function binds below: the object a bind calls through (held
// as a pointer, or as a Callback by value), and the type a by-reference argument is stored as.
template <typename T> inline T & bind_object(T * p) { return *p; }
template <typename T> inline const T & bind_object(const T & r) { return r; }
template <typename T> struct bind_storage { typedef T type; };
template <typename T> struct bind_storage<const T &> { typedef T type; };

// Line 2189. A const member function R (C::*F)(A1) const with the object pointer and A1
// stored; operator() takes nothing (SubscriptionEvent<A1>::NotifySubscribers, queued by
// Console::Online::QueueNotification).
template <typename R, typename C, typename CP, typename A1, R (C::*F)(A1) const, typename = void>
struct method_bind_1_0_const {
    R operator()() const {
        // bind_object: the object may also be a Callback held by value (a callback bound to its
        // result, MailService.cpp), and A1 a const reference stored as its value.
        return (bind_object(m_obj).*F)(m_arg1);
    }
    method_bind_1_0_const(typename d3boost::call_traits<CP>::param_type obj,
                          typename d3boost::call_traits<A1>::param_type arg1)  // line 2199
        : m_obj(obj), m_arg1(arg1) {}
    CP m_obj;
    typename bind_storage<A1>::type m_arg1;
};

// A const member function R (C::*F)() const with the object pointer stored.
template <typename R, typename C, typename CP, R (C::*F)() const, typename = void>
struct method_bind_0_0_const {
    R operator()() const {
        // bind_object: the object may also be a Callback held by value (LobbyService.cpp
        // queues a Callback<void()> this way).
        return (bind_object(m_obj).*F)();
    }
    method_bind_0_0_const(typename d3boost::call_traits<CP>::param_type obj) : m_obj(obj) {}
    CP m_obj;
};

// Binds the object of a const member function that takes no arguments.
template <typename R, typename C>
struct method_binder_0 {
    template <R (C::*F)() const, typename CP>
    static method_bind_0_0_const<R, C, CP, F> bind(const CP & obj) {
        return method_bind_0_0_const<R, C, CP, F>(obj);
    }
};

// Line 7674. Binds the object and the only argument of a const member function.
template <typename R, typename C, typename A1>
struct method_binder_1 {
    template <R (C::*F)(A1) const, typename CP>
    static method_bind_1_0_const<R, C, CP, A1, F> bind(const CP & obj, typename d3boost::call_traits<A1>::param_type arg1) {
        return method_bind_1_0_const<R, C, CP, A1, F>(obj, arg1);  // line 7676
    }
};

// Deduces the binder from a free function's own type; never ODR-used (see the
// OnlineServiceBindFunction macro), so it has no mangled symbol.
template <typename R, typename T1, typename T2, typename T3, typename T4>
function_binder_4<R, T1, T2, T3, T4> get_binder(R (*)(T1, T2, T3, T4));

// The type of Callback<Sig>::node_t::m_call.
template <typename Sig> struct callback_fn_type;
// Original line 10269; the no-argument Callback<void()> used by GamerProfile::Save.
template <typename R> struct callback_fn_type<R()> {
    typedef R (*type)(void *);
};
template <typename R, typename A1> struct callback_fn_type<R(A1)> {
    typedef R (*type)(void *, typename d3boost::call_traits<A1>::param_type);
};
template <typename R, typename A1, typename A2> struct callback_fn_type<R(A1, A2)> {
    typedef R (*type)(void *, typename d3boost::call_traits<A1>::param_type,
                      typename d3boost::call_traits<A2>::param_type);
};

template <typename R, typename A1, typename A2, typename A3>
struct callback_fn_type<R(A1, A2, A3)> {
    typedef R (*type)(void *, typename d3boost::call_traits<A1>::param_type,
                      typename d3boost::call_traits<A2>::param_type,
                      typename d3boost::call_traits<A3>::param_type);
};

template <typename R, typename A1, typename A2, typename A3, typename A4>
struct callback_fn_type<R(A1, A2, A3, A4)> {
    typedef R (*type)(void *, typename d3boost::call_traits<A1>::param_type,
                      typename d3boost::call_traits<A2>::param_type,
                      typename d3boost::call_traits<A3>::param_type,
                      typename d3boost::call_traits<A4>::param_type);
};

// Original callback_base uses CRTP to expose the call operator. The bind and
// call accessors are evidenced by SubscriptionEvent::NotifySubscribers.
// Invented extraction helper: avoids referring to the incomplete CRTP type.
template <typename C> struct callback_traits;
template <typename Sig> struct callback_traits<Callback<Sig> > : d3boost::function_traits<Sig> {};
template <int Arity, typename C> struct callback_base;
template <typename C> struct callback_base<0, C> {
    typedef typename callback_traits<C>::result_type result_type;
    result_type operator()() const {
        void *const bind = static_cast<const C *>(this)->bind();
        return static_cast<const C *>(this)->call()(bind);
    }
    // Original line 10318; what a bound callback (method_bind_0_0_const) calls.
    result_type execute() const {
        return static_cast<const C *>(this)->call()(static_cast<const C *>(this)->bind());
    }
};
template <typename C> struct callback_base<1, C> {
    typedef typename callback_traits<C>::result_type result_type;
    typedef typename callback_traits<C>::arg1_type arg1_type;
    result_type operator()(typename d3boost::call_traits<arg1_type>::param_type param1) const {
        void *const bind = static_cast<const C *>(this)->bind();
        return static_cast<const C *>(this)->call()(bind, param1);
    }
    // Original line 10327; what a bound callback (method_bind_1_0_const) calls.
    result_type execute(typename d3boost::call_traits<arg1_type>::param_type param1) const {
        return static_cast<const C *>(this)->call()(static_cast<const C *>(this)->bind(), param1);
    }
};
// Arity 2/3 layouts are required by achievement subscriptions. Invocation
// remains declared until a concrete original caller is implemented.
template <typename C> struct callback_base<2, C> {
    typedef typename callback_traits<C>::result_type result_type;
    typedef typename callback_traits<C>::arg1_type arg1_type;
    typedef typename callback_traits<C>::arg2_type arg2_type;
    result_type operator()(typename d3boost::call_traits<arg1_type>::param_type param1,
                          typename d3boost::call_traits<arg2_type>::param_type param2) const {
        return execute(param1, param2);
    }
    // Original line 10336.
    result_type execute(typename d3boost::call_traits<arg1_type>::param_type param1,
                        typename d3boost::call_traits<arg2_type>::param_type param2) const {
        return static_cast<const C *>(this)->call()(static_cast<const C *>(this)->bind(), param1, param2);
    }
};

template <typename C> struct callback_base<3, C> {
    typedef typename callback_traits<C>::result_type result_type;
    typedef typename callback_traits<C>::arg1_type arg1_type;
    typedef typename callback_traits<C>::arg2_type arg2_type;
    typedef typename callback_traits<C>::arg3_type arg3_type;
    result_type operator()(typename d3boost::call_traits<arg1_type>::param_type param1,
                          typename d3boost::call_traits<arg2_type>::param_type param2,
                          typename d3boost::call_traits<arg3_type>::param_type param3) const { void *const bind = static_cast<const C *>(this)->bind(); return static_cast<const C *>(this)->call()(bind, param1, param2, param3); }
};

// LobbyService::LookupUserAccountsCallback (X/Client/Console/LobbyService.cpp) is called this way.
template <typename C> struct callback_base<4, C> {
    typedef typename callback_traits<C>::result_type result_type;
    typedef typename callback_traits<C>::arg1_type arg1_type;
    typedef typename callback_traits<C>::arg2_type arg2_type;
    typedef typename callback_traits<C>::arg3_type arg3_type;
    typedef typename callback_traits<C>::arg4_type arg4_type;
    result_type operator()(typename d3boost::call_traits<arg1_type>::param_type param1,
                          typename d3boost::call_traits<arg2_type>::param_type param2,
                          typename d3boost::call_traits<arg3_type>::param_type param3,
                          typename d3boost::call_traits<arg4_type>::param_type param4) const {
        void *const bind = static_cast<const C *>(this)->bind();
        return static_cast<const C *>(this)->call()(bind, param1, param2, param3, param4);
    }
};

// Original line 10222, no-argument invocation thunk.
template <typename Sig, typename F>
typename d3boost::function_traits<Sig>::result_type call_callback(void * bind) {
    return (*static_cast<const F *>(bind))();
}

// Line 10229. What every node's m_call points at: the bound functor, called in place.
template <typename Sig, typename F>
typename d3boost::function_traits<Sig>::result_type call_callback(
    void * bind, typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg1_type>::param_type param1) {
    return (*static_cast<const F *>(bind))(param1);
}

// Line 10236. The same thunk for a two-argument Sig: the bound functor takes both parameters.
template <typename Sig, typename F>
typename d3boost::function_traits<Sig>::result_type call_callback(
    void * bind,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg1_type>::param_type param1,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg2_type>::param_type param2) {
    return (*static_cast<const F *>(bind))(param1, param2);
}

// The same thunk for a three-argument Sig.
template <typename Sig, typename F>
typename d3boost::function_traits<Sig>::result_type call_callback(
    void * bind,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg1_type>::param_type param1,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg2_type>::param_type param2,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg3_type>::param_type param3) {
    return (*static_cast<const F *>(bind))(param1, param2, param3);
}

// Line ~1995. A member function R (C::*Fn)() bound to its object; operator() takes nothing.
// The hash and address keys identify the subscriber in a SubscriptionEvent.
template <typename R, typename C, typename P, R (C::*Fn)(), typename E = void>
struct method_bind_0_0 {
    method_bind_0_0(typename d3boost::call_traits<P>::param_type obj) : m_obj(obj) {}
    R operator()() const {  // line 2002
        return (bind_object(m_obj).*Fn)();
    }
    uint32 HashKey() const {  // line 2006
        // The original instantiates XHashKey<P>; XBaseMap.h's body casts the key straight to
        // uint32, which a pointer refuses, so the pointer goes in as its integer value.
        return XHashKey((size_t)m_obj) ^ hash_function_address(AddressKey());
    }
    static void * AddressKey() {  // line 2013
        return (void *)&AddressKey;
    }
    P m_obj;
};

// Line ~2335. As method_bind_1_0_const, with two bound arguments.
template <typename R, typename C, typename P, typename A1, typename A2, R (C::*Fn)(A1, A2) const, typename E = void>
struct method_bind_2_0_const {
    method_bind_2_0_const(typename d3boost::call_traits<P>::param_type obj,
                          typename d3boost::call_traits<A1>::param_type arg1,
                          typename d3boost::call_traits<A2>::param_type arg2)
        : m_obj(obj), m_arg1(arg1), m_arg2(arg2) {}
    R operator()() const {  // line 2345
        return (bind_object(m_obj).*Fn)(m_arg1, m_arg2);
    }
    P m_obj;
    typename bind_storage<A1>::type m_arg1;
    typename bind_storage<A2>::type m_arg2;
};

// A member function R (C::*Fn)(A1, P1, P2) bound to its object and first argument.
template <typename R, typename C, typename P, typename A1, typename P1, typename P2, R (C::*Fn)(A1, P1, P2), typename E = void>
struct method_bind_1_2 {
    method_bind_1_2(typename d3boost::call_traits<P>::param_type obj,
                    typename d3boost::call_traits<A1>::param_type arg1)
        : m_obj(obj), m_arg1(arg1) {}
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2) const {
        return (bind_object(m_obj).*Fn)(m_arg1, param1, param2);
    }
    P m_obj;
    A1 m_arg1;
};

// Line 4066. A member function R (C::*Fn)(A1, A2, P1, P2) bound to its object and first two
// arguments (LobbyService.cpp: OnUpdateFragmentRetrieved).
template <typename R, typename C, typename P, typename A1, typename A2, typename P1, typename P2, R (C::*Fn)(A1, A2, P1, P2), typename E = void>
struct method_bind_2_2 {
    method_bind_2_2(typename d3boost::call_traits<P>::param_type obj,
                    typename d3boost::call_traits<A1>::param_type arg1,
                    typename d3boost::call_traits<A2>::param_type arg2)
        : m_obj(obj), m_arg1(arg1), m_arg2(arg2) {}
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2) const {  // line 4074
        return (bind_object(m_obj).*Fn)(m_arg1, m_arg2, param1, param2);
    }
    P m_obj;
    A1 m_arg1;
    A2 m_arg2;
};

// A member function R (C::*Fn)(P1, P2) bound to its object only; operator() takes both
// parameters (LobbyService.cpp: onLinkedAccountStatusChange -> OnLinkedAccountsRetrieved).
template <typename R, typename C, typename P, typename P1, typename P2, R (C::*Fn)(P1, P2), typename E = void>
struct method_bind_0_2 {
    method_bind_0_2(typename d3boost::call_traits<P>::param_type obj) : m_obj(obj) {}
    R operator()(typename d3boost::call_traits<P1>::param_type param1,
                 typename d3boost::call_traits<P2>::param_type param2) const {
        return (bind_object(m_obj).*Fn)(param1, param2);
    }
    P m_obj;
};

template <typename R, typename C, typename A1, typename A2>
struct method_binder_2 {
    // DWARF Callback.h:7884, the only method_binder_2::bind instantiation in the binary:
    // bind<&Fn, P, T1, T2>(const P &, const T1 &, const T2 &); the arguments convert to A1/A2 inside bind.
    template <R (C::*Fn)(A1, A2) const, typename P, typename T1, typename T2>
    static method_bind_2_0_const<R, C, P, A1, A2, Fn> bind(const P & obj, const T1 & arg1, const T2 & arg2) {
        return method_bind_2_0_const<R, C, P, A1, A2, Fn>(obj, arg1, arg2);
    }
};

// DWARF Callback.h:7904. All three instantiations in the binary (ScreenManager::FinishGoToScreenCB,
// MailServiceInternal::OnMailHeadersReceived, CachingAchievementsInterface::OnSnapshotCallback) are
// bind<&Fn>(C *, call_traits<T1>::param_type) returning method_bind_1_2<R, C, C *, T1, T2, T3, Fn>.
template <typename R, typename C, typename T1, typename T2, typename T3>
struct method_binder_3 {
    template <R (C::*F)(T1, T2, T3)>
    static method_bind_1_2<R, C, C *, T1, T2, T3, F> bind(C * obj, typename d3boost::call_traits<T1>::param_type arg1) {
        return method_bind_1_2<R, C, C *, T1, T2, T3, F>(obj, arg1);
    }
};

// The same thunk for a four-argument Sig (Console::Online::PresenceManagerInternal's
// LookupUserAccounts callback, PresenceManager.cpp).
template <typename Sig, typename F>
typename d3boost::function_traits<Sig>::result_type call_callback(
    void * bind,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg1_type>::param_type param1,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg2_type>::param_type param2,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg3_type>::param_type param3,
    typename d3boost::call_traits<typename d3boost::function_traits<Sig>::arg4_type>::param_type param4) {
    return (*static_cast<const F *>(bind))(param1, param2, param3, param4);
}

// A member function R (C::*Fn)(A1) bound to its object; operator() takes A1
// (PresenceManagerInternal::OnConnect / OnDisconnect subscribe these).
template <typename R, typename C, typename P, typename A1, R (C::*Fn)(A1), typename E = void>
struct method_bind_0_1 {
    method_bind_0_1(typename d3boost::call_traits<P>::param_type obj) : m_obj(obj) {}
    R operator()(typename d3boost::call_traits<A1>::param_type a1) const {
        return (bind_object(m_obj).*Fn)(a1);
    }
    uint32 HashKey() const {
        return XHashKey((size_t)m_obj) ^ hash_function_address(AddressKey());
    }
    static void * AddressKey() {
        return (void *)&AddressKey;
    }
    P m_obj;
};

// A member function R (C::*Fn)(A1, A2, A3, A4) bound to its object; operator() takes all four.
template <typename R, typename C, typename P, typename A1, typename A2, typename A3, typename A4,
          R (C::*Fn)(A1, A2, A3, A4), typename E = void>
struct method_bind_0_4 {
    method_bind_0_4(typename d3boost::call_traits<P>::param_type obj) : m_obj(obj) {}
    R operator()(typename d3boost::call_traits<A1>::param_type a1,
                 typename d3boost::call_traits<A2>::param_type a2,
                 typename d3boost::call_traits<A3>::param_type a3,
                 typename d3boost::call_traits<A4>::param_type a4) const {
        return (bind_object(m_obj).*Fn)(a1, a2, a3, a4);
    }
    P m_obj;
};

// Invented binder names for the two binds above.
template <typename R, typename C, typename A1>
struct method_binder_0_1 {
    template <R (C::*Fn)(A1), typename P>
    static method_bind_0_1<R, C, P, A1, Fn> bind(const P & obj) {
        return method_bind_0_1<R, C, P, A1, Fn>(obj);
    }
};
template <typename R, typename C, typename A1, typename A2, typename A3, typename A4>
struct method_binder_0_4 {
    template <R (C::*Fn)(A1, A2, A3, A4), typename P>
    static method_bind_0_4<R, C, P, A1, A2, A3, A4, Fn> bind(const P & obj) {
        return method_bind_0_4<R, C, P, A1, A2, A3, A4, Fn>(obj);
    }
};

}  // namespace internal
}  // namespace OnlineService

#include "Callback_Impl.h"
