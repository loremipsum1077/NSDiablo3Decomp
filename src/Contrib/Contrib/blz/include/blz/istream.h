#pragma once

// blz::basic_istream / basic_iostream. basic_istream virtually derives from
// basic_ios and keeps the last unformatted read count (m_gcount at 0x08;
// 0x28 in all). basic_iostream has an istream and an ostream subobject (0x18
// plus the shared virtual basic_ios, 0x30 in all). Recorded and inlined: the
// istream constructor (38), the istream sentry (56), gcount (231) and the
// iostream constructor (1075: istream(sb) at 1076, ostream(sb) at 1077).
// The formatted and unformatted extractors (operator>>, get, getline, ...)
// are declared by the DWARF but nothing in the binary inlines them and they
// are not written here yet.

#include "config.h"
#include "char_traits.h"
#include "ios.h"
#include "ostream.h"
#include "streambuf.h"
#include "string.h"

namespace blz {

// line 20
template <typename T, typename Traits>
class basic_istream : public virtual basic_ios<T, Traits> {
    basic_istream(const basic_istream&);
    basic_istream& operator=(const basic_istream&);

public:
    typedef T char_type;
    typedef typename Traits::int_type int_type;
    typedef typename Traits::pos_type pos_type;
    typedef typename Traits::off_type off_type;

    // line 38
    explicit basic_istream(basic_streambuf<T, Traits>* sb) : m_gcount(0) {
        this->init(sb);
    }

    virtual ~basic_istream() {}

    // line 56
    class sentry {
    public:
        explicit sentry(basic_istream& is, bool noskipws = false) : m_ok(false) {
            if (is.good()) {
                if (!noskipws) {
                    while (true) {
                        int_type c = is.rdbuf()->sgetc();
                        if (Traits::eq_int_type(c, Traits::eof())) {
                            is.setstate(ios_base::eofbit | ios_base::failbit);
                            return;
                        }
                        if (!(c == ' ' || (c >= '\t' && c <= '\r')))
                            break;
                        is.rdbuf()->sbumpc();
                    }
                }
                m_ok = true;
            } else {
                is.setstate(ios_base::failbit);
            }
        }

        explicit operator bool() const {
            return m_ok;
        }

    private:
        bool m_ok;
    };

    // line 231
    streamsize gcount() const {
        return m_gcount;
    }

protected:
    basic_istream(basic_istream&& x) : m_gcount(x.m_gcount) {
        basic_ios<T, Traits>::move(x);
        x.m_gcount = 0;
    }

    basic_istream& operator=(basic_istream&& x) {
        swap(x);
        return *this;
    }

    void swap(basic_istream& x) {
        basic_ios<T, Traits>::swap(x);
        blz::swap(m_gcount, x.m_gcount);
    }

private:
    streamsize m_gcount;
};

// line 1057
template <typename T, typename Traits>
class basic_iostream : public basic_istream<T, Traits>, public basic_ostream<T, Traits> {
    basic_iostream(const basic_iostream&);
    basic_iostream& operator=(const basic_iostream&);

public:
    // line 1075
    explicit basic_iostream(basic_streambuf<T, Traits>* sb)
        : basic_istream<T, Traits>(sb), basic_ostream<T, Traits>(sb) {}

    virtual ~basic_iostream() {}

protected:
    basic_iostream(basic_iostream&& x)
        : basic_istream<T, Traits>(blz::move(x)), basic_ostream<T, Traits>(blz::move(x)) {}

    basic_iostream& operator=(basic_iostream&& x) {
        swap(x);
        return *this;
    }

    void swap(basic_iostream& x) {
        basic_istream<T, Traits>::swap(x);
    }
};

}  // namespace blz
