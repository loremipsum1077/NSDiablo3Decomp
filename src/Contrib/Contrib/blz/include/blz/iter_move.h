#pragma once

// move_iterator. Members and line order from the DWARF; the comparison
// operators are templated on two iterator types (operator!=<I1, I2>) and
// operator- returns decltype(x.base() - y.base()) (from the mangled names).

#include "config.h"
#include "iter_types.h"
#include "type_traits.h"

namespace blz {

// line 16
template <typename Iter>
class move_iterator {
public:
    typedef Iter iterator_type;
    typedef typename iterator_traits<Iter>::iterator_category iterator_category;
    typedef typename iterator_traits<Iter>::value_type value_type;
    typedef typename iterator_traits<Iter>::difference_type difference_type;
    typedef typename iterator_traits<Iter>::reference _base_reference;
    typedef typename conditional<is_reference<_base_reference>::value,
                                 typename remove_reference<_base_reference>::type&&,
                                 _base_reference>::type reference;
    typedef Iter pointer;

    move_iterator() : current() {}

    explicit move_iterator(Iter i) : current(i) {}

    template <typename U>
    move_iterator(const move_iterator<U>& u) : current(u.base()) {}

    template <typename U>
    move_iterator& operator=(const move_iterator<U>& u) {
        current = u.base();
        return *this;
    }

    // line 50
    iterator_type base() const {
        return current;
    }

    // line 55
    reference operator*() const {
        return static_cast<reference>(*current);
    }

    // line 66
    pointer operator->() const {
        return current;
    }

    // line 71
    move_iterator& operator++() {
        ++current;
        return *this;
    }

    move_iterator operator++(int) {
        move_iterator tmp(*this);
        ++current;
        return tmp;
    }

    move_iterator& operator--() {
        --current;
        return *this;
    }

    move_iterator operator--(int) {
        move_iterator tmp(*this);
        --current;
        return tmp;
    }

    move_iterator operator+(difference_type n) const {
        return move_iterator(current + n);
    }

    move_iterator& operator+=(difference_type n) {
        current += n;
        return *this;
    }

    move_iterator operator-(difference_type n) const {
        return move_iterator(current - n);
    }

    move_iterator& operator-=(difference_type n) {
        current -= n;
        return *this;
    }

    // line 119
    reference operator[](difference_type n) const {
        return static_cast<reference>(current[n]);
    }

private:
    Iter current;
};

template <typename I1, typename I2>
inline bool operator==(const move_iterator<I1>& x, const move_iterator<I2>& y) {
    return x.base() == y.base();
}

// line 141
template <typename I1, typename I2>
inline bool operator!=(const move_iterator<I1>& x, const move_iterator<I2>& y) {
    return x.base() != y.base();
}

template <typename I1, typename I2>
inline bool operator<(const move_iterator<I1>& x, const move_iterator<I2>& y) {
    return x.base() < y.base();
}

template <typename I1, typename I2>
inline bool operator<=(const move_iterator<I1>& x, const move_iterator<I2>& y) {
    return x.base() <= y.base();
}

template <typename I1, typename I2>
inline bool operator>(const move_iterator<I1>& x, const move_iterator<I2>& y) {
    return x.base() > y.base();
}

template <typename I1, typename I2>
inline bool operator>=(const move_iterator<I1>& x, const move_iterator<I2>& y) {
    return x.base() >= y.base();
}

// line 172
template <typename I1, typename I2>
inline auto operator-(const move_iterator<I1>& x, const move_iterator<I2>& y)
    -> decltype(x.base() - y.base()) {
    return x.base() - y.base();
}

template <typename Iter>
inline move_iterator<Iter> operator+(typename move_iterator<Iter>::difference_type n,
                                     const move_iterator<Iter>& x) {
    return move_iterator<Iter>(x.base() + n);
}

template <typename Iter>
inline move_iterator<Iter> make_move_iterator(Iter i) {
    return move_iterator<Iter>(i);
}

}  // namespace blz
