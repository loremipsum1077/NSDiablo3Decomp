#pragma once

// blz::queue: adapter over a container (a blz::deque by default) that holds
// it as the protected member `c` (size 0x20 for a deque). The destructor is
// the implicit one (line 20 is the class declaration). Line numbers in
// comments are the original ones from the DWARF; the bodies are the
// one-line forwards to the container the standard adapter has (queue's
// inlined copies in the binary show exactly one container call each).

#include "config.h"
#include "type_traits.h"
#include "deque.h"

namespace blz {

// line 20
template <typename T, typename Container = deque<T> >
class queue {
public:
    typedef typename Container::value_type value_type;
    typedef typename Container::reference reference;
    typedef typename Container::const_reference const_reference;
    typedef typename Container::size_type size_type;

protected:
    Container c;  // 0x00

public:
    // line 36
    queue() : c() {}

    // unverified
    explicit queue(const Container& cont) : c(cont) {}

    // unverified
    explicit queue(Container&& cont) : c(blz::move(cont)) {}

    // unverified
    queue(const queue& q) : c(q.c) {}

    // unverified
    queue(queue&& q) : c(blz::move(q.c)) {}

    // unverified
    queue& operator=(const queue& q) {
        c = q.c;
        return *this;
    }

    // unverified
    queue& operator=(queue&& q) {
        c = blz::move(q.c);
        return *this;
    }

    // line 119
    bool empty() const {
        return c.empty();
    }

    size_type size() const {
        return c.size();
    }

    // line 131
    reference front() {
        return c.front();
    }

    const_reference front() const {
        return c.front();
    }

    reference back() {
        return c.back();
    }

    const_reference back() const {
        return c.back();
    }

    // line 155
    void push(const value_type& x) {
        c.push_back(x);
    }

    reference push() {
        return c.push_back();
    }

    value_type* push_raw() {
        return c.push_back_raw();
    }

    // line 177
    void push(value_type&& x) {
        c.push_back(blz::move(x));
    }

    // line 192
    void pop() {
        c.pop_front();
    }

    // line 198
    void swap(queue& q) {
        c.swap(q.c);
    }
};

}  // namespace blz
