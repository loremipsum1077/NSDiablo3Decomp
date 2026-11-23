#pragma once

// blz::basic_ostream: formatted output onto a basic_streambuf. It virtually
// derives from basic_ios (so an object is vptr + the ios subobject, 0x20 for
// the plain stream) and every call into the buffer goes through the vbase
// offset stored before the vtable. Recorded: the constructor (34, init at 36),
// sentry (51, good() at 53), write (237, body 239-248: a sentry, then sputn,
// setstate(badbit) on a short write or on a failed sentry), operator<<(int)
// 119, (unsigned) 127, (unsigned long) 143 (each formats into a local
// buf[24] with to_string and calls write) and the char / const char*
// overloads (347.., 387: write(s, length)). The other numeric overloads are
// declared by the DWARF but have no inlined copies; written the same way.

#include "config.h"
#include "char_traits.h"
#include "format.h"
#include "ios.h"
#include "streambuf.h"

namespace blz {

// line 16
template <typename T, typename Traits>
class basic_ostream : public virtual basic_ios<T, Traits> {
    basic_ostream(const basic_ostream&);
    basic_ostream& operator=(const basic_ostream&);

public:
    typedef T char_type;
    typedef typename Traits::pos_type pos_type;
    typedef typename Traits::off_type off_type;

    // line 34
    explicit basic_ostream(basic_streambuf<T, Traits>* sb) {
        this->init(sb);
    }

    virtual ~basic_ostream() {}

    // line 51
    class sentry {
    public:
        explicit sentry(basic_ostream& os) : m_ok(os.good()) {}

        explicit operator bool() const {
            return m_ok;
        }

    private:
        bool m_ok;
    };

    // line 75
    basic_ostream& operator<<(basic_ostream& (*pf)(basic_ostream&)) {
        return pf(*this);
    }

    // line 81
    basic_ostream& operator<<(basic_ios<T, Traits>& (*pf)(basic_ios<T, Traits>&)) {
        pf(*this);
        return *this;
    }

    basic_ostream& operator<<(ios_base& (*pf)(ios_base&)) {
        pf(*this);
        return *this;
    }

    // line 95
    basic_ostream& operator<<(bool n) {
        return *this << static_cast<int>(n);
    }

    // line 103
    basic_ostream& operator<<(short n) {
        return *this << static_cast<int>(n);
    }

    // line 111
    basic_ostream& operator<<(unsigned short n) {
        return *this << static_cast<unsigned int>(n);
    }

    // line 119
    basic_ostream& operator<<(int n) {
        char_type buf[24];
        int len = to_string(buf, sizeof(buf), int_fmt(0), n);
        return write(buf, len);
    }

    // line 127
    basic_ostream& operator<<(unsigned int n) {
        char_type buf[24];
        int len = to_string(buf, sizeof(buf), int_fmt(0), n);
        return write(buf, len);
    }

    // line 135
    basic_ostream& operator<<(long n) {
        char_type buf[24];
        int len = to_string(buf, sizeof(buf), int_fmt(0), n);
        return write(buf, len);
    }

    // line 143
    basic_ostream& operator<<(unsigned long n) {
        char_type buf[24];
        int len = to_string(buf, sizeof(buf), int_fmt(0), n);
        return write(buf, len);
    }

    basic_ostream& operator<<(long long n) {
        return *this << static_cast<long>(n);
    }

    // line 159
    basic_ostream& operator<<(unsigned long long n) {
        return *this << static_cast<unsigned long>(n);
    }

    // line 167
    basic_ostream& operator<<(float f) {
        char_type buf[32];
        int len = to_string(buf, sizeof(buf), float_fmt(), f);
        return write(buf, len);
    }

    // line 175
    basic_ostream& operator<<(double f) {
        return *this << static_cast<float>(f);
    }

    basic_ostream& operator<<(const void* p);

    basic_ostream& operator<<(basic_streambuf<T, Traits>* sb);

    // line 222
    basic_ostream& put(char_type c) {
        sentry sen(*this);
        if (sen) {
            if (Traits::eq_int_type(this->rdbuf()->sputc(c), Traits::eof()))
                this->setstate(ios_base::badbit);
        } else {
            this->setstate(ios_base::badbit);
        }
        return *this;
    }

    // line 237
    basic_ostream& write(const char_type* s, streamsize n) {
        if (n > 0) {
            sentry sen(*this);
            if (sen) {
                if (this->rdbuf()->sputn(s, n) != n)
                    this->setstate(ios_base::badbit);
            } else {
                this->setstate(ios_base::badbit);
            }
        }
        return *this;
    }

    basic_ostream& flush() {
        if (this->rdbuf() && this->rdbuf()->pubsync() == -1)
            this->setstate(ios_base::badbit);
        return *this;
    }

    pos_type tellp() {
        if (this->fail())
            return pos_type(off_type(-1));
        return this->rdbuf()->pubseekoff(0, ios_base::cur, ios_base::out);
    }

    basic_ostream& seekp(pos_type pos) {
        if (!this->fail())
            this->rdbuf()->pubseekpos(pos, ios_base::out);
        return *this;
    }

    basic_ostream& seekp(off_type off, ios_base::seekdir dir) {
        if (!this->fail())
            this->rdbuf()->pubseekoff(off, dir, ios_base::out);
        return *this;
    }

protected:
    basic_ostream(basic_ostream&& x) {
        basic_ios<T, Traits>::move(x);
    }

    basic_ostream& operator=(basic_ostream&& x) {
        swap(x);
        return *this;
    }

    void swap(basic_ostream& x) {
        basic_ios<T, Traits>::swap(x);
    }
};

// line 347
template <typename Traits>
inline basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>& os, char c) {
    os.write(&c, 1);
    return os;
}

// line 355
template <typename Traits>
inline basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>& os, signed char c) {
    os.write(reinterpret_cast<const char*>(&c), 1);
    return os;
}

// line 362
template <typename Traits>
inline basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>& os, unsigned char c) {
    os.write(reinterpret_cast<const char*>(&c), 1);
    return os;
}

// line 387
template <typename Traits>
inline basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>& os, const char* s) {
    return os.write(s, Traits::length(s));
}

// line 397
template <typename Traits>
inline basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>& os, const signed char* s) {
    return os << reinterpret_cast<const char*>(s);
}

// line 404
template <typename Traits>
inline basic_ostream<char, Traits>& operator<<(basic_ostream<char, Traits>& os, const unsigned char* s) {
    return os << reinterpret_cast<const char*>(s);
}

}  // namespace blz
