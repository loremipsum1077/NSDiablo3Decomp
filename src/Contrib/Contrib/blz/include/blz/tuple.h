#pragma once

// blz::tuple: recursive, tuple<Head, Tail...> privately derives from
// tuple<Tail...> and stores its own element last, in the public member
// m_val (so tuple<Player*, int> has the int at 0 and m_val at 8). Members and
// line order from the DWARF; the forwarding constructor has a defaulted third
// template parameter that resolves to void (tuple<Player*, const int&, void>).
// get<I>, tuple_size, tuple_element, make_tuple and tie are not recorded and
// are written the usual way.

#include "config.h"
#include "type_traits.h"
#include "util_swap.h"

namespace blz {

// line 22
struct ignore_t {
    ignore_t() {}

    template <typename T>
    const ignore_t& operator=(const T&) const {
        return *this;
    }
};

// line 30
static const ignore_t ignore;

template <typename... Types> class tuple;
template <size_t I> struct _tuple_get;

// line 57
template <>
class tuple<> {
public:
    void swap(tuple&) {}
    bool equals(const tuple&) const { return true; }
    bool less_than(const tuple&) const { return false; }
};

// line 70
template <typename Head, typename... Tail>
class tuple<Head, Tail...> : private tuple<Tail...> {
    typedef tuple<Tail...> base_type;

    template <size_t I> friend struct _tuple_get;
    template <typename... Types> friend class tuple;

public:
    // line 78
    tuple() : base_type(), m_val() {}

    // line 83
    explicit tuple(const Head& x, const Tail&... args) : base_type(args...), m_val(x) {}

    // line 91
    template <typename H, typename... T,
              typename = typename enable_if<sizeof...(T) == sizeof...(Tail) &&
                                            !is_same<typename decay<H>::type, tuple>::value>::type>
    explicit tuple(H&& x, T&&... args)
        : base_type(blz::forward<T>(args)...), m_val(blz::forward<H>(x)) {}

    // line 98
    tuple(const tuple& t) = default;

    // line 108
    tuple(tuple&& t) = default;

    template <typename UHead, typename... UTail,
              typename = typename enable_if<sizeof...(UTail) == sizeof...(Tail)>::type>
    tuple(const tuple<UHead, UTail...>& t)
        : base_type(static_cast<const tuple<UTail...>&>(t)), m_val(t.m_val) {}

    template <typename UHead, typename... UTail,
              typename = typename enable_if<sizeof...(UTail) == sizeof...(Tail)>::type>
    tuple(tuple<UHead, UTail...>&& t)
        : base_type(blz::move(static_cast<tuple<UTail...>&>(t))), m_val(blz::forward<UHead>(t.m_val)) {}

    // line 154
    tuple& operator=(const tuple& t) {
        m_val = t.m_val;
        static_cast<base_type&>(*this) = static_cast<const base_type&>(t);
        return *this;
    }

    // line 162
    tuple& operator=(tuple&& t) {
        m_val = blz::forward<Head>(t.m_val);
        static_cast<base_type&>(*this) = blz::move(static_cast<base_type&>(t));
        return *this;
    }

    template <typename UHead, typename... UTail>
    tuple& operator=(const tuple<UHead, UTail...>& t) {
        m_val = t.m_val;
        static_cast<base_type&>(*this) = static_cast<const tuple<UTail...>&>(t);
        return *this;
    }

    template <typename UHead, typename... UTail>
    tuple& operator=(tuple<UHead, UTail...>&& t) {
        m_val = blz::forward<UHead>(t.m_val);
        static_cast<base_type&>(*this) = blz::move(static_cast<tuple<UTail...>&>(t));
        return *this;
    }

    // line 207
    void swap(tuple& t) {
        blz::swap(m_val, t.m_val);
        base_type::swap(static_cast<base_type&>(t));
    }

    bool equals(const tuple& t) const {
        return m_val == t.m_val && base_type::equals(static_cast<const base_type&>(t));
    }

    bool less_than(const tuple& t) const {
        if (m_val < t.m_val)
            return true;
        if (t.m_val < m_val)
            return false;
        return base_type::less_than(static_cast<const base_type&>(t));
    }

    // line 233
    Head m_val;
};

template <typename T> struct tuple_size;
template <typename... Types>
struct tuple_size<tuple<Types...> > : integral_constant<size_t, sizeof...(Types)> {};

template <size_t I, typename T> struct tuple_element;
template <typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...> > {
    typedef Head type;
};
template <size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...> > : tuple_element<I - 1, tuple<Tail...> > {};

template <size_t I>
struct _tuple_get {
    template <typename Head, typename... Tail>
    static typename tuple_element<I, tuple<Head, Tail...> >::type& get(tuple<Head, Tail...>& t) {
        return _tuple_get<I - 1>::get(static_cast<tuple<Tail...>&>(t));
    }
    template <typename Head, typename... Tail>
    static const typename tuple_element<I, tuple<Head, Tail...> >::type& get(const tuple<Head, Tail...>& t) {
        return _tuple_get<I - 1>::get(static_cast<const tuple<Tail...>&>(t));
    }
};

template <>
struct _tuple_get<0> {
    template <typename Head, typename... Tail>
    static Head& get(tuple<Head, Tail...>& t) {
        return t.m_val;
    }
    template <typename Head, typename... Tail>
    static const Head& get(const tuple<Head, Tail...>& t) {
        return t.m_val;
    }
};

template <size_t I, typename... Types>
inline typename tuple_element<I, tuple<Types...> >::type& get(tuple<Types...>& t) {
    return _tuple_get<I>::get(t);
}

template <size_t I, typename... Types>
inline const typename tuple_element<I, tuple<Types...> >::type& get(const tuple<Types...>& t) {
    return _tuple_get<I>::get(t);
}

template <size_t I, typename... Types>
inline typename tuple_element<I, tuple<Types...> >::type&& get(tuple<Types...>&& t) {
    return blz::forward<typename tuple_element<I, tuple<Types...> >::type>(_tuple_get<I>::get(t));
}

template <typename... Types>
inline bool operator==(const tuple<Types...>& x, const tuple<Types...>& y) {
    return x.equals(y);
}

template <typename... Types>
inline bool operator!=(const tuple<Types...>& x, const tuple<Types...>& y) {
    return !x.equals(y);
}

template <typename... Types>
inline bool operator<(const tuple<Types...>& x, const tuple<Types...>& y) {
    return x.less_than(y);
}

template <typename... Types>
inline void swap(tuple<Types...>& x, tuple<Types...>& y) {
    x.swap(y);
}

template <typename... Types>
inline tuple<typename decay<Types>::type...> make_tuple(Types&&... args) {
    return tuple<typename decay<Types>::type...>(blz::forward<Types>(args)...);
}

template <typename... Types>
inline tuple<Types&...> tie(Types&... args) {
    return tuple<Types&...>(args...);
}

template <typename... Types>
inline tuple<Types&&...> forward_as_tuple(Types&&... args) {
    return tuple<Types&&...>(blz::forward<Types>(args)...);
}

}  // namespace blz

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(blz::ignore_t) == 0x1, "blz::ignore_t");
