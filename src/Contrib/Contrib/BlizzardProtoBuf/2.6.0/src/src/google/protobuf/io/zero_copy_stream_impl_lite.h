// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/io/zero_copy_stream_impl_lite.h
// Subset: the array and string streams (the Copying*Adaptor classes need stubs/common.h's
// scoped_array, not written yet).

#pragma once

#include "zero_copy_stream.h"
#include "../stubs/common.h"
#include "../../../../../../../blz/include/blz/string.h"
#include "../stubs/stl_util.h"
// #include <google/protobuf/stubs/common.h>  // protobuf

namespace google {
namespace protobuf {
namespace io {
class ArrayInputStream : public google::protobuf::io::ZeroCopyInputStream {  // size 0x20
public:
    ArrayInputStream(const void * data, int size, int block_size = -1);
    virtual ~ArrayInputStream();  // vtable slot 0
    virtual bool Next(const void ** data, int * size);  // vtable slot 2
    virtual void BackUp(int count);  // vtable slot 3
    virtual bool Skip(int count);  // vtable slot 4
    virtual google::protobuf::int64 ByteCount() const;  // vtable slot 5
private:
    const google::protobuf::uint8 *const data_;  // 0x08
    const int size_;  // 0x10
    const int block_size_;  // 0x14
    int position_;  // 0x18
    int last_returned_size_;  // 0x1c
    ArrayInputStream(const google::protobuf::io::ArrayInputStream &);
    void operator=(const google::protobuf::io::ArrayInputStream &);
};
static_assert(sizeof(ArrayInputStream) == 0x20, "ArrayInputStream");

class ArrayOutputStream : public google::protobuf::io::ZeroCopyOutputStream {  // size 0x20
public:
    ArrayOutputStream(void * data, int size, int block_size = -1);
    virtual ~ArrayOutputStream();  // vtable slot 0
    virtual bool Next(void ** data, int * size);  // vtable slot 2
    virtual void BackUp(int count);  // vtable slot 3
    virtual google::protobuf::int64 ByteCount() const;  // vtable slot 4
private:
    google::protobuf::uint8 *const data_;  // 0x08
    const int size_;  // 0x10
    const int block_size_;  // 0x14
    int position_;  // 0x18
    int last_returned_size_;  // 0x1c
    ArrayOutputStream(const google::protobuf::io::ArrayOutputStream &);
    void operator=(const google::protobuf::io::ArrayOutputStream &);
};
static_assert(sizeof(ArrayOutputStream) == 0x20, "ArrayOutputStream");

class StringOutputStream : public google::protobuf::io::ZeroCopyOutputStream {  // size 0x10
public:
    explicit StringOutputStream(blz::string *);
    virtual ~StringOutputStream();  // vtable slot 0
    virtual bool Next(void ** data, int * size);  // vtable slot 2
    virtual void BackUp(int count);  // vtable slot 3
    virtual google::protobuf::int64 ByteCount() const;  // vtable slot 4
private:
    static const int kMinimumSize = 16;
    blz::string * target_;  // 0x08
    StringOutputStream(const google::protobuf::io::StringOutputStream &);
    void operator=(const google::protobuf::io::StringOutputStream &);
};
static_assert(sizeof(StringOutputStream) == 0x10, "StringOutputStream");

// line 340
// Return a pointer to mutable characters underlying the given string.  The
// return value is valid until the next time the string is resized.  We
// trust the caller to treat the return value as an array of length s->size().
inline char* mutable_string_data(blz::string* s) {
  return string_as_array(s);
}

}  // namespace io
}  // namespace protobuf
}  // namespace google

// Functions defined in this file (bodies are not in the DWARF):
//   line   340  char * google::protobuf::io::mutable_string_data(blz::string * s)  [inlined into 5 functions]
