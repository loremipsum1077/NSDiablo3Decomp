// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/io/coded_stream_inl.h

#pragma once

#include "coded_stream.h"
#include "../stubs/stl_util.h"

namespace google {
namespace protobuf {
namespace io {

inline bool CodedInputStream::InternalReadStringInline(blz::string* buffer,
                                                       int size) {
  if (size < 0) return false;  // security: size is often user-supplied

  if (BufferSize() >= size) {
    STLStringResizeUninitialized(buffer, size);
    // When buffer is empty, string_as_array(buffer) will return NULL but memcpy
    // requires non-NULL pointers even when size is 0. Hench this check.
    if (size > 0) {
      memcpy(string_as_array(buffer), buffer_, size);
      Advance(size);
    }
    return true;
  }

  return ReadStringFallback(buffer, size);
}

}  // namespace io
}  // namespace protobuf
}  // namespace google
