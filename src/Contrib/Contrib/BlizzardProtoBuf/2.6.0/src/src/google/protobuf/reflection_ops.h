// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/reflection_ops.h
// Subset: the entry points generated .pb.cc files call.

#pragma once

#include "stubs/common.h"
#include "../../../../../../blz/include/blz/string.h"
#include "../../../../../../blz/include/blz/vector.h"

namespace google { namespace protobuf { class Message; } }

namespace google {
namespace protobuf {
namespace internal {

class ReflectionOps {  // size 0x1
public:
    static void Copy(const google::protobuf::Message & from, google::protobuf::Message * to);
    static void Merge(const google::protobuf::Message & from, google::protobuf::Message * to);
    static void Clear(google::protobuf::Message * message);
    static bool IsInitialized(const google::protobuf::Message & message);
    static void DiscardUnknownFields(google::protobuf::Message * message);
    static void FindInitializationErrors(const google::protobuf::Message & message, const blz::string & prefix, blz::vector<blz::string> * errors);
private:
    ReflectionOps(const google::protobuf::internal::ReflectionOps &);
    void operator=(const google::protobuf::internal::ReflectionOps &);
};

}  // namespace internal
}  // namespace protobuf
}  // namespace google

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(google::protobuf::internal::ReflectionOps) == 0x1, "google::protobuf::internal::ReflectionOps");
