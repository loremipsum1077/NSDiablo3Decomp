// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/wire_format_lite.h

#pragma once

#include "../../../../../../blz/include/blz/string.h"
#include "stubs/common.h"
#include "io/coded_stream.h"

namespace google { namespace protobuf { class MessageLite; } }
namespace google { namespace protobuf { template <typename Element> class RepeatedField; } }
namespace google { namespace protobuf { namespace io { class CodedInputStream; } } }
namespace google { namespace protobuf { namespace io { class CodedOutputStream; } } }

namespace google {
namespace protobuf {
namespace internal {
class WireFormatLite {  // size 0x1
public:
    enum WireType {
        WIRETYPE_VARINT = 0,
        WIRETYPE_FIXED64 = 1,
        WIRETYPE_LENGTH_DELIMITED = 2,
        WIRETYPE_START_GROUP = 3,
        WIRETYPE_END_GROUP = 4,
        WIRETYPE_FIXED32 = 5,
    };
    enum FieldType {
        TYPE_DOUBLE = 1,
        TYPE_FLOAT = 2,
        TYPE_INT64 = 3,
        TYPE_UINT64 = 4,
        TYPE_INT32 = 5,
        TYPE_FIXED64 = 6,
        TYPE_FIXED32 = 7,
        TYPE_BOOL = 8,
        TYPE_STRING = 9,
        TYPE_GROUP = 10,
        TYPE_MESSAGE = 11,
        TYPE_BYTES = 12,
        TYPE_UINT32 = 13,
        TYPE_ENUM = 14,
        TYPE_SFIXED32 = 15,
        TYPE_SFIXED64 = 16,
        TYPE_SINT32 = 17,
        TYPE_SINT64 = 18,
        MAX_FIELD_TYPE = 18,
    };
    enum CppType {
        CPPTYPE_INT32 = 1,
        CPPTYPE_INT64 = 2,
        CPPTYPE_UINT32 = 3,
        CPPTYPE_UINT64 = 4,
        CPPTYPE_DOUBLE = 5,
        CPPTYPE_FLOAT = 6,
        CPPTYPE_BOOL = 7,
        CPPTYPE_ENUM = 8,
        CPPTYPE_STRING = 9,
        CPPTYPE_MESSAGE = 10,
        MAX_CPPTYPE = 10,
    };
    static google::protobuf::internal::WireFormatLite::CppType FieldTypeToCppType(google::protobuf::internal::WireFormatLite::FieldType type);
    static google::protobuf::internal::WireFormatLite::WireType WireTypeForFieldType(google::protobuf::internal::WireFormatLite::FieldType type);
    static const int kTagTypeBits = 3;
    static const google::protobuf::uint32 kTagTypeMask = 7;
    static google::protobuf::uint32 MakeTag(int field_number, google::protobuf::internal::WireFormatLite::WireType type);
    static google::protobuf::internal::WireFormatLite::WireType GetTagWireType(google::protobuf::uint32 tag);
    static int GetTagFieldNumber(google::protobuf::uint32 tag);
    static int TagSize(int field_number, google::protobuf::internal::WireFormatLite::FieldType type);
    static bool SkipField(google::protobuf::io::CodedInputStream *, google::protobuf::uint32);
    static bool SkipField(google::protobuf::io::CodedInputStream *, google::protobuf::uint32, google::protobuf::io::CodedOutputStream *);
    static bool SkipMessage(google::protobuf::io::CodedInputStream * input);
    static bool SkipMessage(google::protobuf::io::CodedInputStream * input, google::protobuf::io::CodedOutputStream * output);
    static const int kMessageSetItemNumber = 1;
    static const int kMessageSetTypeIdNumber = 2;
    static const int kMessageSetMessageNumber = 3;
    static const int kMessageSetItemStartTag = 11;
    static const int kMessageSetItemEndTag = 12;
    static const int kMessageSetTypeIdTag = 16;
    static const int kMessageSetMessageTag = 26;
    static const int kMessageSetItemTagsSize;
    static google::protobuf::uint32 EncodeFloat(float value);
    static float DecodeFloat(google::protobuf::uint32);
    static google::protobuf::uint64 EncodeDouble(double value);
    static double DecodeDouble(google::protobuf::uint64);
    static google::protobuf::uint32 ZigZagEncode32(google::protobuf::int32 n);
    static google::protobuf::int32 ZigZagDecode32(google::protobuf::uint32 n);
    static google::protobuf::uint64 ZigZagEncode64(google::protobuf::int64 n);
    static google::protobuf::int64 ZigZagDecode64(google::protobuf::uint64 n);
    static bool ReadPackedEnumNoInline(google::protobuf::io::CodedInputStream *, bool (*)(int), google::protobuf::RepeatedField<int> *);
    template <typename CType, enum FieldType DeclaredType>
    static inline bool ReadPrimitive(google::protobuf::io::CodedInputStream* input, CType* value) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
    template <typename CType, enum FieldType DeclaredType>
    static inline bool ReadRepeatedPrimitive(int tag_size, google::protobuf::uint32 tag, google::protobuf::io::CodedInputStream* input, google::protobuf::RepeatedField<CType>* value) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
    template <typename CType, enum FieldType DeclaredType>
    static bool ReadRepeatedPrimitiveNoInline(int tag_size, google::protobuf::uint32 tag, google::protobuf::io::CodedInputStream* input, google::protobuf::RepeatedField<CType>* value);
    template <typename CType, enum FieldType DeclaredType>
    static inline const google::protobuf::uint8* ReadPrimitiveFromArray(const google::protobuf::uint8* buffer, CType* value) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
    template <typename CType, enum FieldType DeclaredType>
    static inline bool ReadPackedPrimitive(google::protobuf::io::CodedInputStream* input, google::protobuf::RepeatedField<CType>* value) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
    template <typename CType, enum FieldType DeclaredType>
    static bool ReadPackedPrimitiveNoInline(google::protobuf::io::CodedInputStream* input, google::protobuf::RepeatedField<CType>* value);
    template <typename MessageType>
    static inline bool ReadMessageNoVirtual(google::protobuf::io::CodedInputStream* input, MessageType* value);
    static bool ReadString(google::protobuf::io::CodedInputStream * input, blz::string * value);
    static bool ReadBytes(google::protobuf::io::CodedInputStream * input, blz::string * value);
    static bool ReadGroup(int field_number, google::protobuf::io::CodedInputStream * input, google::protobuf::MessageLite * value);
    static bool ReadMessage(google::protobuf::io::CodedInputStream * input, google::protobuf::MessageLite * value);
    static void WriteTag(int field_number, google::protobuf::internal::WireFormatLite::WireType type, google::protobuf::io::CodedOutputStream * output);
    static void WriteInt32NoTag(google::protobuf::int32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteInt64NoTag(google::protobuf::int64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteUInt32NoTag(google::protobuf::uint32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteUInt64NoTag(google::protobuf::uint64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSInt32NoTag(google::protobuf::int32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSInt64NoTag(google::protobuf::int64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteFixed32NoTag(google::protobuf::uint32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteFixed64NoTag(google::protobuf::uint64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSFixed32NoTag(google::protobuf::int32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSFixed64NoTag(google::protobuf::int64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteFloatNoTag(float value, google::protobuf::io::CodedOutputStream * output);
    static void WriteDoubleNoTag(double value, google::protobuf::io::CodedOutputStream * output);
    static void WriteBoolNoTag(bool value, google::protobuf::io::CodedOutputStream * output);
    static void WriteEnumNoTag(int value, google::protobuf::io::CodedOutputStream * output);
    static void WriteInt32(int field_number, google::protobuf::int32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteInt64(int field_number, google::protobuf::int64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteUInt32(int field_number, google::protobuf::uint32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteUInt64(int field_number, google::protobuf::uint64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSInt32(int field_number, google::protobuf::int32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSInt64(int field_number, google::protobuf::int64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteFixed32(int field_number, google::protobuf::uint32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteFixed64(int field_number, google::protobuf::uint64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSFixed32(int field_number, google::protobuf::int32 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteSFixed64(int field_number, google::protobuf::int64 value, google::protobuf::io::CodedOutputStream * output);
    static void WriteFloat(int field_number, float value, google::protobuf::io::CodedOutputStream * output);
    static void WriteDouble(int field_number, double value, google::protobuf::io::CodedOutputStream * output);
    static void WriteBool(int field_number, bool value, google::protobuf::io::CodedOutputStream * output);
    static void WriteEnum(int field_number, int value, google::protobuf::io::CodedOutputStream * output);
    static void WriteString(int field_number, const blz::string & value, google::protobuf::io::CodedOutputStream * output);
    static void WriteBytes(int field_number, const blz::string & value, google::protobuf::io::CodedOutputStream * output);
    static void WriteStringMaybeAliased(int field_number, const blz::string & value, google::protobuf::io::CodedOutputStream * output);
    static void WriteBytesMaybeAliased(int field_number, const blz::string & value, google::protobuf::io::CodedOutputStream * output);
    static void WriteGroup(int field_number, const google::protobuf::MessageLite & value, google::protobuf::io::CodedOutputStream * output);
    static void WriteMessage(int field_number, const google::protobuf::MessageLite & value, google::protobuf::io::CodedOutputStream * output);
    static void WriteGroupMaybeToArray(int, const google::protobuf::MessageLite &, google::protobuf::io::CodedOutputStream *);
    static void WriteMessageMaybeToArray(int field_number, const google::protobuf::MessageLite & value, google::protobuf::io::CodedOutputStream * output);
    static google::protobuf::uint8 * WriteTagToArray(int field_number, google::protobuf::internal::WireFormatLite::WireType type, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteInt32NoTagToArray(google::protobuf::int32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteInt64NoTagToArray(google::protobuf::int64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteUInt32NoTagToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteUInt64NoTagToArray(google::protobuf::uint64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSInt32NoTagToArray(google::protobuf::int32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSInt64NoTagToArray(google::protobuf::int64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteFixed32NoTagToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteFixed64NoTagToArray(google::protobuf::uint64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSFixed32NoTagToArray(google::protobuf::int32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSFixed64NoTagToArray(google::protobuf::int64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteFloatNoTagToArray(float value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteDoubleNoTagToArray(double value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteBoolNoTagToArray(bool value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteEnumNoTagToArray(int value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteInt32ToArray(int field_number, google::protobuf::int32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteInt64ToArray(int field_number, google::protobuf::int64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteUInt32ToArray(int field_number, google::protobuf::uint32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteUInt64ToArray(int field_number, google::protobuf::uint64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSInt32ToArray(int field_number, google::protobuf::int32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSInt64ToArray(int field_number, google::protobuf::int64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteFixed32ToArray(int field_number, google::protobuf::uint32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteFixed64ToArray(int field_number, google::protobuf::uint64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSFixed32ToArray(int field_number, google::protobuf::int32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteSFixed64ToArray(int field_number, google::protobuf::int64 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteFloatToArray(int field_number, float value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteDoubleToArray(int field_number, double value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteBoolToArray(int field_number, bool value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteEnumToArray(int field_number, int value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteStringToArray(int field_number, const blz::string & value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteBytesToArray(int field_number, const blz::string & value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteGroupToArray(int field_number, const google::protobuf::MessageLite & value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteMessageToArray(int field_number, const google::protobuf::MessageLite & value, google::protobuf::uint8 * target);
    template <typename MessageType>
    static inline google::protobuf::uint8* WriteMessageNoVirtualToArray(int field_number, const MessageType& value, google::protobuf::uint8* target) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
    static int Int32Size(google::protobuf::int32 value);
    static int Int64Size(google::protobuf::int64 value);
    static int UInt32Size(google::protobuf::uint32 value);
    static int UInt64Size(google::protobuf::uint64 value);
    static int SInt32Size(google::protobuf::int32 value);
    static int SInt64Size(google::protobuf::int64 value);
    static int EnumSize(int value);
    static const int kFixed32Size = 4;
    static const int kFixed64Size = 8;
    static const int kSFixed32Size = 4;
    static const int kSFixed64Size = 8;
    static const int kFloatSize = 4;
    static const int kDoubleSize = 8;
    static const int kBoolSize = 1;
    static int StringSize(const blz::string & value);
    static int BytesSize(const blz::string & value);
    static int GroupSize(const google::protobuf::MessageLite & value);
    static int MessageSize(const google::protobuf::MessageLite & value);
    static int LengthDelimitedSize(int length);
    template <typename MessageType>
    static inline int MessageSizeNoVirtual(const MessageType& value);
private:
    template <typename CType, enum FieldType DeclaredType>
    static inline bool ReadRepeatedFixedSizePrimitive(int tag_size, google::protobuf::uint32 tag, google::protobuf::io::CodedInputStream* input, google::protobuf::RepeatedField<CType>* value) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
    template <typename CType, enum FieldType DeclaredType>
    static inline bool ReadPackedFixedSizePrimitive(google::protobuf::io::CodedInputStream* input, google::protobuf::RepeatedField<CType>* value) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
public:
private:
    static const google::protobuf::internal::WireFormatLite::CppType kFieldTypeToCppTypeMap[];
    static const google::protobuf::internal::WireFormatLite::WireType kWireTypeForFieldType[];
    WireFormatLite(const google::protobuf::internal::WireFormatLite &);
    void operator=(const google::protobuf::internal::WireFormatLite &);
public:
};
static_assert(sizeof(WireFormatLite) == 0x1, "WireFormatLite");

class FieldSkipper {  // size 0x8
    // vtable pointer
public:
    FieldSkipper() {}
    virtual ~FieldSkipper() {}  // vtable slot 0
    virtual bool SkipField(google::protobuf::io::CodedInputStream *, google::protobuf::uint32);  // vtable slot 2
    virtual bool SkipMessage(google::protobuf::io::CodedInputStream *);  // vtable slot 3
    virtual void SkipUnknownEnum(int, int);  // vtable slot 4
};
static_assert(sizeof(FieldSkipper) == 0x8, "FieldSkipper");

class CodedOutputStreamFieldSkipper : public google::protobuf::internal::FieldSkipper {  // size 0x10
public:
    explicit CodedOutputStreamFieldSkipper(google::protobuf::io::CodedOutputStream *);
    virtual ~CodedOutputStreamFieldSkipper();  // vtable slot 0
    virtual bool SkipField(google::protobuf::io::CodedInputStream *, google::protobuf::uint32);  // vtable slot 2
    virtual bool SkipMessage(google::protobuf::io::CodedInputStream *);  // vtable slot 3
    virtual void SkipUnknownEnum(int, int);  // vtable slot 4
protected:
    google::protobuf::io::CodedOutputStream * unknown_fields_;  // 0x08
};
static_assert(sizeof(CodedOutputStreamFieldSkipper) == 0x10, "CodedOutputStreamFieldSkipper");

// This macro does the same thing as WireFormatLite::MakeTag(), but the
// result is usable as a compile-time constant, which makes it usable
// as a switch case or a template input.  WireFormatLite::MakeTag() is more
// type-safe, though, so prefer it if possible.
#define GOOGLE_PROTOBUF_WIRE_FORMAT_MAKE_TAG(FIELD_NUMBER, TYPE)                  \
  static_cast<uint32>(                                                   \
    ((FIELD_NUMBER) << ::google::protobuf::internal::WireFormatLite::kTagTypeBits) \
      | (TYPE))

// inline methods ====================================================

inline WireFormatLite::CppType
WireFormatLite::FieldTypeToCppType(FieldType type) {  // line 563
  return kFieldTypeToCppTypeMap[type];
}

inline uint32 WireFormatLite::MakeTag(int field_number, WireType type) {  // line 567
  return GOOGLE_PROTOBUF_WIRE_FORMAT_MAKE_TAG(field_number, type);
}

inline WireFormatLite::WireType WireFormatLite::GetTagWireType(uint32 tag) {  // line 571
  return static_cast<WireType>(tag & kTagTypeMask);
}

inline int WireFormatLite::GetTagFieldNumber(uint32 tag) {  // line 575
  return static_cast<int>(tag >> kTagTypeBits);
}

inline int WireFormatLite::TagSize(int field_number,
                                   WireFormatLite::FieldType type) {  // line 579
  int result = io::CodedOutputStream::VarintSize32(
    field_number << kTagTypeBits);
  if (type == TYPE_GROUP) {
    // Groups have both a start and an end tag.
    return result * 2;
  } else {
    return result;
  }
}

inline uint32 WireFormatLite::EncodeFloat(float value) {  // line 591
  union {float f; uint32 i;};
  f = value;
  return i;
}

inline float WireFormatLite::DecodeFloat(uint32 value) {
  union {float f; uint32 i;};
  i = value;
  return f;
}

inline uint64 WireFormatLite::EncodeDouble(double value) {  // line 603
  union {double f; uint64 i;};
  f = value;
  return i;
}

inline double WireFormatLite::DecodeDouble(uint64 value) {
  union {double f; uint64 i;};
  i = value;
  return f;
}

inline uint32 WireFormatLite::ZigZagEncode32(int32 n) {  // line 639
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 31);
}

inline int32 WireFormatLite::ZigZagDecode32(uint32 n) {  // line 644
  return (n >> 1) ^ -static_cast<int32>(n & 1);
}

inline uint64 WireFormatLite::ZigZagEncode64(int64 n) {  // line 648
  // Note:  the right-shift must be arithmetic
  return (n << 1) ^ (n >> 63);
}

inline int64 WireFormatLite::ZigZagDecode64(uint64 n) {  // line 653
  return (n >> 1) ^ -static_cast<int64>(n & 1);
}

// In the original this is defined in the class body (line 139).
inline WireFormatLite::WireType
WireFormatLite::WireTypeForFieldType(FieldType type) {
  return kWireTypeForFieldType[type];
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
