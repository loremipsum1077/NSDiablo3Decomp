#include "generated_message_util.h"

#include "stubs/once.h"

namespace google {
namespace protobuf {
namespace internal {

const blz::string* empty_string_;  // line 52
ProtobufOnceType empty_string_once_init_;  // line 53

void DeleteEmptyString() {  // line 55
  delete empty_string_;
}

void InitEmptyString() {  // line 59
  empty_string_ = new blz::string;
  OnShutdown(&DeleteEmptyString);
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
