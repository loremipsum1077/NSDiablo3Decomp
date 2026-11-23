// Checks of inlined blz::basic_string members through small game functions
// that inline them (protobuf code, rewritten here from the protobuf 2.6
// sources with blz::string as protobuf's string type). Only the string
// members are under test.
#include <string.h>

#include "Contrib/Contrib/blz/include/blz/string.h"

namespace google {
namespace protobuf {

typedef blz::string string;

namespace internal {

enum LogLevel { LOGLEVEL_INFO, LOGLEVEL_WARNING, LOGLEVEL_ERROR, LOGLEVEL_FATAL };

class LogMessage {
public:
    LogMessage& operator<<(const string& value);

private:
    LogLevel level_;
    const char* filename_;
    int line_;
    string message_;
};

// common.cc:144 (operator+=(const string&))
LogMessage& LogMessage::operator<<(const string& value)
{
    message_ += value;
    return *this;
}

}  // namespace internal

namespace io {

class Tokenizer {
public:
    static bool IsIdentifier(const string& text);
};

namespace {

#define CHARACTER_CLASS(NAME, EXPRESSION)      \
    class NAME {                               \
    public:                                    \
        static inline bool InClass(char c) {   \
            return EXPRESSION;                 \
        }                                      \
    }

CHARACTER_CLASS(Letter, ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_'));
CHARACTER_CLASS(Alphanumeric, ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') ||
                                  (c == '_'));

template <typename CharacterClass>
bool AllInClass(const string& s)
{
    for (int i = 0; i < s.size(); i++) {
        if (!CharacterClass::InClass(s[i]))
            return false;
    }
    return true;
}

}  // namespace

// tokenizer.cc:1114 (size, at, substr -> basic_string(s, n), operator[], ~basic_string)
bool Tokenizer::IsIdentifier(const string& text)
{
    if (text.size() == 0)
        return false;
    if (!Letter::InClass(text.at(0)))
        return false;
    if (!AllInClass<Alphanumeric>(text.substr(1)))
        return false;
    return true;
}

}  // namespace io

namespace io {

class CodedInputStream {
public:
    bool ReadString(string* buffer, int size);

private:
    int BufferSize() const;
    void Advance(int amount);
    bool InternalReadStringInline(string* buffer, int size);
    bool ReadStringFallback(string* buffer, int size);

    void* unknown_;  // 0x00, not used here
    const unsigned char* buffer_;  // 0x08
    const unsigned char* buffer_end_;  // 0x10
};

inline int CodedInputStream::BufferSize() const
{
    return buffer_end_ - buffer_;
}

inline void CodedInputStream::Advance(int amount)
{
    buffer_ += amount;
}

}  // namespace io

inline void STLStringResizeUninitialized(string* s, size_t new_size)
{
    s->resize(new_size);
}

inline char* string_as_array(string* str)
{
    return &*str->begin();
}

namespace io {

inline char* mutable_string_data(string* s)
{
    return string_as_array(s);
}

inline bool CodedInputStream::InternalReadStringInline(string* buffer, int size)
{
    if (size < 0)
        return false;

    if (BufferSize() >= size) {
        STLStringResizeUninitialized(buffer, size);
        if (size > 0) {
            memcpy(mutable_string_data(buffer), buffer_, size);
            Advance(size);
        }
        return true;
    }

    return ReadStringFallback(buffer, size);
}

// coded_stream.cc:249 (resize -> erase / append(n, c), begin)
bool CodedInputStream::ReadString(string* buffer, int size)
{
    if (size < 0)
        return false;
    return InternalReadStringInline(buffer, size);
}

}  // namespace io
static const int kFastToBufferSize = 32;
char* FastUInt32ToBufferLeft(unsigned int i, char* buffer);

// strutil.cc:1048 (basic_string<char*>(first, last) -> assign -> _assign -> modify_data(n))
string SimpleItoa(unsigned int i)
{
    char buffer[kFastToBufferSize];
    return string(buffer, FastUInt32ToBufferLeft(i, buffer));
}

// strutil.cc:599 (data, substr(pos, n), operator=(&&), ~basic_string)
bool safe_parse_sign(string* text, bool* negative_ptr)
{
    const char* start = text->data();
    const char* end = start + text->size();

    while (start < end && (start[0] == ' ')) {
        ++start;
    }
    while (start < end && (end[-1] == ' ')) {
        --end;
    }
    if (start >= end) {
        return false;
    }

    *negative_ptr = (start[0] == '-');
    if (*negative_ptr || start[0] == '+') {
        ++start;
        if (start >= end) {
            return false;
        }
    }
    *text = text->substr(start - text->data(), end - start);
    return true;
}

}  // namespace protobuf
}  // namespace google

// strutil.cc has other users of _assign_rv, so there it is not inlined into
// safe_parse_sign; an explicit instantiation reproduces that here.
template void blz::string::_assign_rv(blz::string&&);
