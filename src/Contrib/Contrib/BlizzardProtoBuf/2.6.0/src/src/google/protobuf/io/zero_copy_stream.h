// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/io/zero_copy_stream.h

#pragma once

#include "../stubs/common.h"

namespace google {
namespace protobuf {
namespace io {
class ZeroCopyInputStream {  // size 0x8
    // vtable pointer
public:
    ZeroCopyInputStream();
    virtual ~ZeroCopyInputStream();  // vtable slot 0
    virtual bool Next(const void **, int *) = 0;  // vtable slot 2
    virtual void BackUp(int) = 0;  // vtable slot 3
    virtual bool Skip(int) = 0;  // vtable slot 4
    virtual google::protobuf::int64 ByteCount() const = 0;  // vtable slot 5
private:
    ZeroCopyInputStream(const google::protobuf::io::ZeroCopyInputStream &);
    void operator=(const google::protobuf::io::ZeroCopyInputStream &);
};
static_assert(sizeof(ZeroCopyInputStream) == 0x8, "ZeroCopyInputStream");

class ZeroCopyOutputStream {  // size 0x8
    // vtable pointer
public:
    ZeroCopyOutputStream();
    virtual ~ZeroCopyOutputStream();  // vtable slot 0
    virtual bool Next(void **, int *) = 0;  // vtable slot 2
    virtual void BackUp(int) = 0;  // vtable slot 3
    virtual google::protobuf::int64 ByteCount() const = 0;  // vtable slot 4
    virtual bool WriteAliasedRaw(const void *, int);  // vtable slot 5
    virtual bool AllowsAliasing() const;  // vtable slot 6
private:
    ZeroCopyOutputStream(const google::protobuf::io::ZeroCopyOutputStream &);
    void operator=(const google::protobuf::io::ZeroCopyOutputStream &);
};
static_assert(sizeof(ZeroCopyOutputStream) == 0x8, "ZeroCopyOutputStream");

inline ZeroCopyInputStream::ZeroCopyInputStream() {}
inline ZeroCopyOutputStream::ZeroCopyOutputStream() {}

// line 237
inline bool ZeroCopyOutputStream::AllowsAliasing() const { return false; }

}  // namespace io
}  // namespace protobuf
}  // namespace google

// Functions defined in this file (bodies are not in the DWARF):
//   line   237  bool google::protobuf::io::ZeroCopyOutputStream::AllowsAliasing() const  [0x7100b0f980]
