// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/stubs/substitute.h

#pragma once

#include <string.h>

#include "../../../../../../../blz/include/blz/string.h"
#include "common.h"
#include "strutil.h"

namespace google {
namespace protobuf {
namespace strings {

namespace internal {  // Implementation details.

class SubstituteArg {  // size 0x30
 public:
  inline SubstituteArg(const char* value)
    : text_(value), size_(strlen(text_)) {}
  inline SubstituteArg(const blz::string& value)
    : text_(value.data()), size_(value.size()) {}

  // Indicates that no argument was given.
  inline explicit SubstituteArg()
    : text_(NULL), size_(-1) {}

  // Primitives
  inline SubstituteArg(char value)
    : text_(scratch_), size_(1) { scratch_[0] = value; }
  inline SubstituteArg(short value)
    : text_(FastInt32ToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(unsigned short value)
    : text_(FastUInt32ToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(int value)
    : text_(FastInt32ToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(unsigned int value)
    : text_(FastUInt32ToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(long value)
    : text_(FastLongToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(unsigned long value)
    : text_(FastULongToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(long long value)
    : text_(FastInt64ToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(unsigned long long value)
    : text_(FastUInt64ToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(float value)
    : text_(FloatToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(double value)
    : text_(DoubleToBuffer(value, scratch_)), size_(strlen(text_)) {}
  inline SubstituteArg(bool value)
    : text_(value ? "true" : "false"), size_(strlen(text_)) {}

  inline const char* data() const { return text_; }
  inline int size() const { return size_; }

 private:
  const char* text_;
  int size_;
  char scratch_[kFastToBufferSize];
};
static_assert(sizeof(SubstituteArg) == 0x30, "SubstituteArg");

}  // namespace internal

blz::string Substitute(
  const char* format,
  const internal::SubstituteArg& arg0 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg1 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg2 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg3 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg4 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg5 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg6 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg7 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg8 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg9 = internal::SubstituteArg());

void SubstituteAndAppend(
  blz::string* output, const char* format,
  const internal::SubstituteArg& arg0 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg1 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg2 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg3 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg4 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg5 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg6 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg7 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg8 = internal::SubstituteArg(),
  const internal::SubstituteArg& arg9 = internal::SubstituteArg());

}  // namespace strings
}  // namespace protobuf
}  // namespace google
