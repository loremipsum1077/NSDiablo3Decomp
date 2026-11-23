// Subset of protobuf 2.6.0 generated_message_util.h (Blizzard build: blz::string).
#pragma once

#include "stubs/once.h"
#include "../../../../../../blz/include/blz/string.h"

// Upstream line 60: marks deprecated field accessors; empty in this build.
#undef DEPRECATED_PROTOBUF_FIELD
#define PROTOBUF_DEPRECATED

namespace google {
namespace protobuf {
namespace internal {

// Annotation for the empty string, shared by all generated messages.
extern const blz::string* empty_string_;
extern ProtobufOnceType empty_string_once_init_;
void InitEmptyString();

// Defined in generated_message_reflection.cc.
int StringSpaceUsedExcludingSelf(const blz::string& str);

// line 79
inline const blz::string& GetEmptyStringAlreadyInited() {
    return *empty_string_;
}

// line 83
inline const blz::string& GetEmptyString() {
    ::google::protobuf::GoogleOnceInit(&empty_string_once_init_, &InitEmptyString);
    return GetEmptyStringAlreadyInited();
}

// line 101
template <class Type> bool AllAreInitialized(const Type& t) {
    for (int i = t.size(); --i >= 0; ) {
        if (!t.Get(i).IsInitialized()) return false;
    }
    return true;
}

// Functions defined in this file (bodies are not in the DWARF):
//   line    79  const blz::string & GetEmptyStringAlreadyInited()  [inlined into 95 functions]
//   line    83  const blz::string & GetEmptyString()  [inlined into 42 functions]
//   line   102  AllAreInitialized<RepeatedPtrField<FileDescriptorProto> >  [inlined into 13 functions]

}  // namespace internal
}  // namespace protobuf
}  // namespace google
