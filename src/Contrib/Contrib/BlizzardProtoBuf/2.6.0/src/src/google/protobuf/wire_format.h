// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/wire_format.h
// Subset: WireFormat's UnknownFieldSet entry points and the UTF-8 verify hooks generated
// code calls (the other descriptor-based members are not written yet).

#pragma once

#include "stubs/common.h"
#include "wire_format_lite.h"
#include "descriptor.h"

// Do UTF-8 validation on string type in Debug build only
#ifndef NDEBUG
#define GOOGLE_PROTOBUF_UTF8_VALIDATION_ENABLED
#endif

namespace google { namespace protobuf { class UnknownFieldSet; } }
namespace google { namespace protobuf { class Message; } }
namespace google { namespace protobuf { namespace io { class CodedInputStream; } } }
namespace google { namespace protobuf { namespace io { class CodedOutputStream; } } }
namespace google { namespace protobuf { class FieldDescriptor; } }

namespace google {
namespace protobuf {
namespace internal {
class WireFormat {  // size 0x1
public:
    static bool ParseAndMergePartial(google::protobuf::io::CodedInputStream * input, google::protobuf::Message * message);
    static void SerializeWithCachedSizes(const google::protobuf::Message & message, int size, google::protobuf::io::CodedOutputStream * output);
    static int ByteSize(const google::protobuf::Message & message);
    static bool SkipField(google::protobuf::io::CodedInputStream * input, google::protobuf::uint32 tag, google::protobuf::UnknownFieldSet * unknown_fields);
    static bool SkipMessage(google::protobuf::io::CodedInputStream * input, google::protobuf::UnknownFieldSet * unknown_fields);
    static void SerializeUnknownFields(const google::protobuf::UnknownFieldSet & unknown_fields, google::protobuf::io::CodedOutputStream * output);
    static google::protobuf::uint8 * SerializeUnknownFieldsToArray(const google::protobuf::UnknownFieldSet & unknown_fields, google::protobuf::uint8 * target);
    static void SerializeUnknownMessageSetItems(const google::protobuf::UnknownFieldSet & unknown_fields, google::protobuf::io::CodedOutputStream * output);
    static google::protobuf::uint8 * SerializeUnknownMessageSetItemsToArray(const google::protobuf::UnknownFieldSet &, google::protobuf::uint8 *);
    static int ComputeUnknownFieldsSize(const google::protobuf::UnknownFieldSet & unknown_fields);
    static int ComputeUnknownMessageSetItemsSize(const google::protobuf::UnknownFieldSet & unknown_fields);
    static google::protobuf::internal::WireFormatLite::WireType WireTypeForFieldType(google::protobuf::FieldDescriptor::Type type);
    static int TagSize(int field_number, google::protobuf::FieldDescriptor::Type type);
    static bool ParseAndMergeField(google::protobuf::uint32 tag, const google::protobuf::FieldDescriptor * field, google::protobuf::Message * message, google::protobuf::io::CodedInputStream * input);
    static void SerializeFieldWithCachedSizes(const google::protobuf::FieldDescriptor * field, const google::protobuf::Message & message, google::protobuf::io::CodedOutputStream * output);
    static int FieldByteSize(const google::protobuf::FieldDescriptor * field, const google::protobuf::Message & message);
    static bool ParseAndMergeMessageSetItem(google::protobuf::io::CodedInputStream * input, google::protobuf::Message * message);
    static void SerializeMessageSetItemWithCachedSizes(const google::protobuf::FieldDescriptor * field, const google::protobuf::Message & message, google::protobuf::io::CodedOutputStream * output);
    static int MessageSetItemByteSize(const google::protobuf::FieldDescriptor * field, const google::protobuf::Message & message);
    static int FieldDataOnlyByteSize(const google::protobuf::FieldDescriptor * field, const google::protobuf::Message & message);
    enum Operation {
        PARSE = 0,
        SERIALIZE = 1,
    };
    static void VerifyUTF8String(const char * data, int size, google::protobuf::internal::WireFormat::Operation op);
    static void VerifyUTF8StringNamedField(const char * data, int size, google::protobuf::internal::WireFormat::Operation op, const char * field_name);
private:
    static void VerifyUTF8StringFallback(const char * data, int size, google::protobuf::internal::WireFormat::Operation op, const char * field_name);
    static bool SkipMessageSetField(google::protobuf::io::CodedInputStream * input, google::protobuf::uint32 field_number, google::protobuf::UnknownFieldSet * unknown_fields);
    static bool ParseAndMergeMessageSetField(google::protobuf::uint32 field_number, const google::protobuf::FieldDescriptor * field, google::protobuf::Message * message, google::protobuf::io::CodedInputStream * input);
    WireFormat(const google::protobuf::internal::WireFormat &);
    void operator=(const google::protobuf::internal::WireFormat &);
};
static_assert(sizeof(WireFormat) == 0x1, "WireFormat");

// Subclass of FieldSkipper which saves skipped fields to an UnknownFieldSet.
class UnknownFieldSetFieldSkipper : public FieldSkipper {
 public:
  UnknownFieldSetFieldSkipper(UnknownFieldSet* unknown_fields)
      : unknown_fields_(unknown_fields) {}
  virtual ~UnknownFieldSetFieldSkipper() {}

  // implements FieldSkipper -----------------------------------------
  virtual bool SkipField(io::CodedInputStream* input, uint32 tag);
  virtual bool SkipMessage(io::CodedInputStream* input);
  virtual void SkipUnknownEnum(int field_number, int value);

 protected:
  UnknownFieldSet* unknown_fields_;
};
static_assert(sizeof(UnknownFieldSetFieldSkipper) == 0x10, "UnknownFieldSetFieldSkipper");

inline WireFormatLite::WireType WireFormat::WireTypeForFieldType(
    FieldDescriptor::Type type) {
  // Some compilers don't like enum -> enum casts, so we implicit_cast to
  // int first.
  return WireFormatLite::WireTypeForFieldType(
      static_cast<WireFormatLite::FieldType>(
        implicit_cast<int>(type)));
}

inline int WireFormat::TagSize(int field_number, FieldDescriptor::Type type) {
  // Some compilers don't like enum -> enum casts, so we implicit_cast to
  // int first.
  return WireFormatLite::TagSize(field_number,
      static_cast<WireFormatLite::FieldType>(
        implicit_cast<int>(type)));
}

inline void WireFormat::VerifyUTF8String(const char* data, int size,
    WireFormat::Operation op) {
#ifdef GOOGLE_PROTOBUF_UTF8_VALIDATION_ENABLED
  WireFormat::VerifyUTF8StringFallback(data, size, op, NULL);
#else
  // Avoid the compiler warning about unsued variables.
  (void)data; (void)size; (void)op;
#endif
}

inline void WireFormat::VerifyUTF8StringNamedField(
    const char* data, int size, WireFormat::Operation op,
    const char* field_name) {
#ifdef GOOGLE_PROTOBUF_UTF8_VALIDATION_ENABLED
  WireFormat::VerifyUTF8StringFallback(data, size, op, field_name);
#endif
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
