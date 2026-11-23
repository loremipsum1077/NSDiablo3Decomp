#pragma once

// blz iostreams, base layer. ios_base is a vtable pointer (virtual destructor
// only); basic_ios adds the stream buffer pointer and the state bits (0x08 and
// 0x10, 0x18 in all) and is a VIRTUAL base of basic_istream / basic_ostream,
// so every access to it goes through the vbase offset stored before the
// vtable. Line numbers are the DWARF's: setstate 224 (body 226), good 230
// (232), init 261. The state members are the ones the binary shows used:
// m_rdstate |= state in setstate, == 0 in good, init sets the state from the
// buffer pointer. The rest (clear, eof, fail, bad, rdbuf(sb), move, swap) are
// declared by the DWARF but never inlined anywhere; written the usual way.

#include "config.h"
#include "char_traits.h"
#include "util_swap.h"

#include <wchar.h>

namespace blz {

template <typename T, typename Traits>
class basic_streambuf;

class ios_base {
    ios_base(const ios_base&);
    ios_base& operator=(const ios_base&);

public:
    typedef unsigned int iostate;
    static const iostate goodbit = 0;
    static const iostate badbit = 1;
    static const iostate eofbit = 2;
    static const iostate failbit = 4;

    typedef unsigned int openmode;
    static const openmode app = 1;
    static const openmode ate = 2;
    static const openmode binary = 4;
    static const openmode in = 8;
    static const openmode out = 16;
    static const openmode trunc = 32;

    typedef unsigned int seekdir;
    static const seekdir beg = 0;
    static const seekdir cur = 1;
    static const seekdir end = 2;

    virtual ~ios_base() {}

protected:
    ios_base() {}
};
static_assert(sizeof(ios_base) == 0x8, "ios_base");

// line 81
template <typename State>
class fpos {
public:
    fpos() {}
    // The state is left uninitialised when only an offset is given (the binary
    // returns pos_type(off_type(-1)) with whatever was in the register).
    fpos(int off) : m_offset(off) {}
    fpos(streamoff off) : m_offset(off) {}

    State state() const { return m_state; }
    void state(State s) { m_state = s; }

    operator long long() const { return m_offset; }

    bool operator==(const fpos& x) const { return m_offset == x.m_offset; }
    bool operator!=(const fpos& x) const { return m_offset != x.m_offset; }

    fpos operator+(streamoff off) const { return fpos(m_offset + off); }
    fpos& operator+=(streamoff off) { m_offset += off; return *this; }
    fpos operator-(streamoff off) const { return fpos(m_offset - off); }
    fpos& operator-=(streamoff off) { m_offset -= off; return *this; }

private:
    streamoff m_offset;
    State m_state;
};

}  // namespace blz

// streambuf.h needs ios_base (seekdir, openmode), so it is included here,
// after ios_base and before basic_ios (the original has the two include each
// other the same way).
#include "streambuf.h"

namespace blz {

template <typename T, typename Traits>
class basic_ios : public ios_base {
    basic_ios(const basic_ios&);
    basic_ios& operator=(const basic_ios&);

public:
    explicit basic_ios(basic_streambuf<T, Traits>* sb) {
        init(sb);
    }

    virtual ~basic_ios() {}

    basic_streambuf<T, Traits>* rdbuf() const {
        return m_rdbuf;
    }

    basic_streambuf<T, Traits>* rdbuf(basic_streambuf<T, Traits>* sb) {
        basic_streambuf<T, Traits>* old = m_rdbuf;
        m_rdbuf = sb;
        clear(goodbit);
        return old;
    }

    explicit operator bool() const {
        return !fail();
    }

    bool operator!() const {
        return fail();
    }

    iostate rdstate() const {
        return m_rdstate;
    }

    // line 216
    void clear(iostate state) {
        m_rdstate = m_rdbuf ? state : (state | badbit);
    }

    // line 224
    void setstate(iostate state) {
        m_rdstate |= state;
    }

    // line 230
    bool good() const {
        return m_rdstate == 0;
    }

    // line 236
    bool eof() const {
        return (m_rdstate & eofbit) != 0;
    }

    // line 242
    bool fail() const {
        return (m_rdstate & (failbit | badbit)) != 0;
    }

    bool bad() const {
        return (m_rdstate & badbit) != 0;
    }

protected:
    basic_ios() {}

    // line 261
    void init(basic_streambuf<T, Traits>* sb) {
        m_rdbuf = sb;
        m_rdstate = sb ? goodbit : badbit;
    }

    void move(basic_ios& x) {
        m_rdbuf = nullptr;
        m_rdstate = x.m_rdstate;
    }

    void swap(basic_ios& x) {
        blz::swap(m_rdstate, x.m_rdstate);
    }

    void set_rdbuf(basic_streambuf<T, Traits>* sb) {
        m_rdbuf = sb;
    }

    basic_streambuf<T, Traits>* m_rdbuf;
    iostate m_rdstate;
};

}  // namespace blz
