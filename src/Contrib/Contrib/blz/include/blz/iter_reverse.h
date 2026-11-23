#pragma once

// reverse_iterator. Members and line order from the DWARF; operator* keeps a
// local `tmp` (line 52) and operator-> calls operator* (line 60).

#include "config.h"
#include "iter_types.h"

namespace blz {

// line 16
template <typename Iter>
class reverse_iterator {
public:
    typedef Iter iterator_type;
    typedef typename iterator_traits<Iter>::iterator_category iterator_category;
    typedef typename iterator_traits<Iter>::value_type value_type;
    typedef typename iterator_traits<Iter>::difference_type difference_type;
    typedef typename iterator_traits<Iter>::pointer pointer;
    typedef typename iterator_traits<Iter>::reference reference;

    // line 27
    reverse_iterator() : current() {}

    // line 31
    explicit reverse_iterator(Iter x) : current(x) {}

    template <typename U>
    reverse_iterator(const reverse_iterator<U>& u) : current(u.base()) {}

    template <typename U>
    reverse_iterator& operator=(const reverse_iterator<U>& u) {
        current = u.base();
        return *this;
    }

    // line 47
    Iter base() const {
        return current;
    }

    // line 52
    reference operator*() const {
        Iter tmp = current;
        return *--tmp;
    }

    // line 58
    pointer operator->() const {
        return &(operator*());
    }

    reverse_iterator& operator++() {
        --current;
        return *this;
    }

    reverse_iterator operator++(int) {
        reverse_iterator tmp(*this);
        --current;
        return tmp;
    }

    reverse_iterator& operator--() {
        ++current;
        return *this;
    }

    reverse_iterator operator--(int) {
        reverse_iterator tmp(*this);
        ++current;
        return tmp;
    }

    reverse_iterator operator+(difference_type n) const {
        return reverse_iterator(current - n);
    }

    reverse_iterator& operator+=(difference_type n) {
        current -= n;
        return *this;
    }

    reverse_iterator operator-(difference_type n) const {
        return reverse_iterator(current + n);
    }

    reverse_iterator& operator-=(difference_type n) {
        current += n;
        return *this;
    }

    // line 111
    reference operator[](difference_type n) const {
        return *(*this + n);
    }

protected:
    Iter current;
};

template <typename I1, typename I2>
inline bool operator==(const reverse_iterator<I1>& x, const reverse_iterator<I2>& y) {
    return x.base() == y.base();
}

// line 129
template <typename I1, typename I2>
inline bool operator!=(const reverse_iterator<I1>& x, const reverse_iterator<I2>& y) {
    return x.base() != y.base();
}

template <typename I1, typename I2>
inline bool operator<(const reverse_iterator<I1>& x, const reverse_iterator<I2>& y) {
    return x.base() > y.base();
}

template <typename I1, typename I2>
inline bool operator<=(const reverse_iterator<I1>& x, const reverse_iterator<I2>& y) {
    return x.base() >= y.base();
}

template <typename I1, typename I2>
inline bool operator>(const reverse_iterator<I1>& x, const reverse_iterator<I2>& y) {
    return x.base() < y.base();
}

template <typename I1, typename I2>
inline bool operator>=(const reverse_iterator<I1>& x, const reverse_iterator<I2>& y) {
    return x.base() <= y.base();
}

template <typename I1, typename I2>
inline auto operator-(const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
    -> decltype(y.base() - x.base()) {
    return y.base() - x.base();
}

template <typename Iter>
inline reverse_iterator<Iter> operator+(typename reverse_iterator<Iter>::difference_type n,
                                        const reverse_iterator<Iter>& x) {
    return reverse_iterator<Iter>(x.base() - n);
}

}  // namespace blz
