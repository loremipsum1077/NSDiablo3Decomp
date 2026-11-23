#include "wire_format_lite_inl.h"
#line 3 "C:/Projects/D3/Main/Trunk/Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/wire_format_lite.cc"
#include "stubs/common.h"
#include "io/coded_stream_inl.h"
#include "io/zero_copy_stream.h"
#include "io/zero_copy_stream_impl_lite.h"

namespace google {
namespace protobuf {
namespace internal {














































// Each of the four message-set tags (11, 12, 16, 26) is a one-byte varint.
const int WireFormatLite::kMessageSetItemTagsSize = 4;

const WireFormatLite::CppType WireFormatLite::kFieldTypeToCppTypeMap[MAX_FIELD_TYPE + 1] = {
    static_cast<CppType>(0),  // 0 is reserved for errors
    CPPTYPE_DOUBLE,   // TYPE_DOUBLE
    CPPTYPE_FLOAT,    // TYPE_FLOAT
    CPPTYPE_INT64,    // TYPE_INT64
    CPPTYPE_UINT64,   // TYPE_UINT64
    CPPTYPE_INT32,    // TYPE_INT32
    CPPTYPE_UINT64,   // TYPE_FIXED64
    CPPTYPE_UINT32,   // TYPE_FIXED32
    CPPTYPE_BOOL,     // TYPE_BOOL
    CPPTYPE_STRING,   // TYPE_STRING
    CPPTYPE_MESSAGE,  // TYPE_GROUP
    CPPTYPE_MESSAGE,  // TYPE_MESSAGE
    CPPTYPE_STRING,   // TYPE_BYTES
    CPPTYPE_UINT32,   // TYPE_UINT32
    CPPTYPE_ENUM,     // TYPE_ENUM
    CPPTYPE_INT32,    // TYPE_SFIXED32
    CPPTYPE_INT64,    // TYPE_SFIXED64
    CPPTYPE_INT32,    // TYPE_SINT32
    CPPTYPE_INT64,    // TYPE_SINT64
};

const WireFormatLite::WireType WireFormatLite::kWireTypeForFieldType[MAX_FIELD_TYPE + 1] = {
    static_cast<WireFormatLite::WireType>(-1),  // invalid
    WIRETYPE_FIXED64,           // TYPE_DOUBLE
    WIRETYPE_FIXED32,           // TYPE_FLOAT
    WIRETYPE_VARINT,            // TYPE_INT64
    WIRETYPE_VARINT,            // TYPE_UINT64
    WIRETYPE_VARINT,            // TYPE_INT32
    WIRETYPE_FIXED64,           // TYPE_FIXED64
    WIRETYPE_FIXED32,           // TYPE_FIXED32
    WIRETYPE_VARINT,            // TYPE_BOOL
    WIRETYPE_LENGTH_DELIMITED,  // TYPE_STRING
    WIRETYPE_START_GROUP,       // TYPE_GROUP
    WIRETYPE_LENGTH_DELIMITED,  // TYPE_MESSAGE
    WIRETYPE_LENGTH_DELIMITED,  // TYPE_BYTES
    WIRETYPE_VARINT,            // TYPE_UINT32
    WIRETYPE_VARINT,            // TYPE_ENUM
    WIRETYPE_FIXED32,           // TYPE_SFIXED32
    WIRETYPE_FIXED64,           // TYPE_SFIXED64
    WIRETYPE_VARINT,            // TYPE_SINT32
    WIRETYPE_VARINT,            // TYPE_SINT64
};


// LINKER-REMOVED: the four Skip functions below are upstream protobuf 2.6.0 (wire_format_lite.cc).
// The original has none of them (nothing kept the FieldSkipper vtable) and this unit has no DWARF;
// they are written as upstream has them, with blz::string for string.
bool WireFormatLite::SkipField(
    io::CodedInputStream* input, uint32 tag) {
  switch (WireFormatLite::GetTagWireType(tag)) {
    case WireFormatLite::WIRETYPE_VARINT: {
      uint64 value;
      if (!input->ReadVarint64(&value)) return false;
      return true;
    }
    case WireFormatLite::WIRETYPE_FIXED64: {
      uint64 value;
      if (!input->ReadLittleEndian64(&value)) return false;
      return true;
    }
    case WireFormatLite::WIRETYPE_LENGTH_DELIMITED: {
      uint32 length;
      if (!input->ReadVarint32(&length)) return false;
      if (!input->Skip(length)) return false;
      return true;
    }
    case WireFormatLite::WIRETYPE_START_GROUP: {
      if (!input->IncrementRecursionDepth()) return false;
      if (!SkipMessage(input)) return false;
      input->DecrementRecursionDepth();
      // Check that the ending tag matched the starting tag.
      if (!input->LastTagWas(WireFormatLite::MakeTag(
          WireFormatLite::GetTagFieldNumber(tag),
          WireFormatLite::WIRETYPE_END_GROUP))) {
        return false;
      }
      return true;
    }
    case WireFormatLite::WIRETYPE_END_GROUP: {
      return false;
    }
    case WireFormatLite::WIRETYPE_FIXED32: {
      uint32 value;
      if (!input->ReadLittleEndian32(&value)) return false;
      return true;
    }
    default: {
      return false;
    }
  }
}

bool WireFormatLite::SkipField(
    io::CodedInputStream* input, uint32 tag, io::CodedOutputStream* output) {
  switch (WireFormatLite::GetTagWireType(tag)) {
    case WireFormatLite::WIRETYPE_VARINT: {
      uint64 value;
      if (!input->ReadVarint64(&value)) return false;
      output->WriteVarint32(tag);
      output->WriteVarint64(value);
      return true;
    }
    case WireFormatLite::WIRETYPE_FIXED64: {
      uint64 value;
      if (!input->ReadLittleEndian64(&value)) return false;
      output->WriteVarint32(tag);
      output->WriteLittleEndian64(value);
      return true;
    }
    case WireFormatLite::WIRETYPE_LENGTH_DELIMITED: {
      uint32 length;
      if (!input->ReadVarint32(&length)) return false;
      output->WriteVarint32(tag);
      output->WriteVarint32(length);
      blz::string temp;
      if (!input->ReadString(&temp, length)) return false;
      output->WriteString(temp);
      return true;
    }
    case WireFormatLite::WIRETYPE_START_GROUP: {
      output->WriteVarint32(tag);
      if (!input->IncrementRecursionDepth()) return false;
      if (!SkipMessage(input, output)) return false;
      input->DecrementRecursionDepth();
      // Check that the ending tag matched the starting tag.
      if (!input->LastTagWas(WireFormatLite::MakeTag(
          WireFormatLite::GetTagFieldNumber(tag),
          WireFormatLite::WIRETYPE_END_GROUP))) {
        return false;
      }
      return true;
    }
    case WireFormatLite::WIRETYPE_END_GROUP: {
      return false;
    }
    case WireFormatLite::WIRETYPE_FIXED32: {
      uint32 value;
      if (!input->ReadLittleEndian32(&value)) return false;
      output->WriteVarint32(tag);
      output->WriteLittleEndian32(value);
      return true;
    }
    default: {
      return false;
    }
  }
}

bool WireFormatLite::SkipMessage(io::CodedInputStream* input) {
  while (true) {
    uint32 tag = input->ReadTag();
    if (tag == 0) {
      // End of input.  This is a valid place to end, so return true.
      return true;
    }

    WireFormatLite::WireType wire_type = WireFormatLite::GetTagWireType(tag);

    if (wire_type == WireFormatLite::WIRETYPE_END_GROUP) {
      // Must be the end of the message.
      return true;
    }

    if (!SkipField(input, tag)) return false;
  }
}

bool WireFormatLite::SkipMessage(io::CodedInputStream* input,
    io::CodedOutputStream* output) {
  while (true) {
    uint32 tag = input->ReadTag();
    if (tag == 0) {
      // End of input.  This is a valid place to end, so return true.
      return true;
    }

    WireFormatLite::WireType wire_type = WireFormatLite::GetTagWireType(tag);

    if (wire_type == WireFormatLite::WIRETYPE_END_GROUP) {
      output->WriteVarint32(tag);
      // Must be the end of the message.
      return true;
    }

    if (!SkipField(input, tag, output)) return false;
  }
}






















































void WireFormatLite::WriteInt32(int field_number, int32 value,
                                io::CodedOutputStream* output) {  // line 302
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteInt32NoTag(value, output);
}
void WireFormatLite::WriteInt64(int field_number, int64 value,
                                io::CodedOutputStream* output) {  // line 307
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteInt64NoTag(value, output);
}
void WireFormatLite::WriteUInt32(int field_number, uint32 value,
                                 io::CodedOutputStream* output) {  // line 312
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteUInt32NoTag(value, output);
}
void WireFormatLite::WriteUInt64(int field_number, uint64 value,
                                 io::CodedOutputStream* output) {  // line 317
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteUInt64NoTag(value, output);
}
void WireFormatLite::WriteSInt32(int field_number, int32 value,
                                 io::CodedOutputStream* output) {  // line 322
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteSInt32NoTag(value, output);
}
void WireFormatLite::WriteSInt64(int field_number, int64 value,
                                 io::CodedOutputStream* output) {  // line 327
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteSInt64NoTag(value, output);
}
void WireFormatLite::WriteFixed32(int field_number, uint32 value,
                                  io::CodedOutputStream* output) {  // line 332
  WriteTag(field_number, WIRETYPE_FIXED32, output);
  WriteFixed32NoTag(value, output);
}
void WireFormatLite::WriteFixed64(int field_number, uint64 value,
                                  io::CodedOutputStream* output) {  // line 337
  WriteTag(field_number, WIRETYPE_FIXED64, output);
  WriteFixed64NoTag(value, output);
}
void WireFormatLite::WriteSFixed32(int field_number, int32 value,
                                   io::CodedOutputStream* output) {  // line 342
  WriteTag(field_number, WIRETYPE_FIXED32, output);
  WriteSFixed32NoTag(value, output);
}
void WireFormatLite::WriteSFixed64(int field_number, int64 value,
                                   io::CodedOutputStream* output) {  // line 347
  WriteTag(field_number, WIRETYPE_FIXED64, output);
  WriteSFixed64NoTag(value, output);
}
void WireFormatLite::WriteFloat(int field_number, float value,
                                io::CodedOutputStream* output) {  // line 352
  WriteTag(field_number, WIRETYPE_FIXED32, output);
  WriteFloatNoTag(value, output);
}
void WireFormatLite::WriteDouble(int field_number, double value,
                                 io::CodedOutputStream* output) {  // line 357
  WriteTag(field_number, WIRETYPE_FIXED64, output);
  WriteDoubleNoTag(value, output);
}
void WireFormatLite::WriteBool(int field_number, bool value,
                               io::CodedOutputStream* output) {  // line 362
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteBoolNoTag(value, output);
}
void WireFormatLite::WriteEnum(int field_number, int value,
                               io::CodedOutputStream* output) {  // line 367
  WriteTag(field_number, WIRETYPE_VARINT, output);
  WriteEnumNoTag(value, output);
}

void WireFormatLite::WriteString(int field_number, const blz::string& value,
                                 io::CodedOutputStream* output) {  // line 373
  // String is for UTF-8 text only
  WriteTag(field_number, WIRETYPE_LENGTH_DELIMITED, output);
  GOOGLE_CHECK(value.size() <= kint32max);
  output->WriteVarint32(value.size());
  output->WriteString(value);
}
void WireFormatLite::WriteStringMaybeAliased(
    int field_number, const blz::string& value,
    io::CodedOutputStream* output) {  // line 381
  // String is for UTF-8 text only
  WriteTag(field_number, WIRETYPE_LENGTH_DELIMITED, output);
  GOOGLE_CHECK(value.size() <= kint32max);
  output->WriteVarint32(value.size());
  output->WriteRawMaybeAliased(value.data(), value.size());
}
void WireFormatLite::WriteBytes(int field_number, const blz::string& value,
                                io::CodedOutputStream* output) {  // line 390
  WriteTag(field_number, WIRETYPE_LENGTH_DELIMITED, output);
  GOOGLE_CHECK(value.size() <= kint32max);
  output->WriteVarint32(value.size());
  output->WriteString(value);
}
void WireFormatLite::WriteBytesMaybeAliased(
    int field_number, const blz::string& value,
    io::CodedOutputStream* output) {  // line 397
  WriteTag(field_number, WIRETYPE_LENGTH_DELIMITED, output);
  GOOGLE_CHECK(value.size() <= kint32max);
  output->WriteVarint32(value.size());
  output->WriteRawMaybeAliased(value.data(), value.size());
}


void WireFormatLite::WriteGroup(int field_number,
                                const MessageLite& value,
                                io::CodedOutputStream* output) {  // line 407
  WriteTag(field_number, WIRETYPE_START_GROUP, output);
  value.SerializeWithCachedSizes(output);
  WriteTag(field_number, WIRETYPE_END_GROUP, output);
}

void WireFormatLite::WriteMessage(int field_number,
                                  const MessageLite& value,
                                  io::CodedOutputStream* output) {  // line 415
  WriteTag(field_number, WIRETYPE_LENGTH_DELIMITED, output);
  const int size = value.GetCachedSize();
  output->WriteVarint32(size);
  value.SerializeWithCachedSizes(output);
}
















void WireFormatLite::WriteMessageMaybeToArray(int field_number,
                                              const MessageLite& value,
                                              io::CodedOutputStream* output) {  // line 439
  WriteTag(field_number, WIRETYPE_LENGTH_DELIMITED, output);
  const int size = value.GetCachedSize();
  output->WriteVarint32(size);
  uint8* target = output->GetDirectBufferForNBytesAndAdvance(size);
  if (target != NULL) {
    uint8* end = value.SerializeWithCachedSizesToArray(target);
    GOOGLE_DCHECK_EQ(end - target, size);
  } else {
    value.SerializeWithCachedSizes(output);
  }
}

bool WireFormatLite::ReadString(io::CodedInputStream* input,
                                blz::string* value) {  // line 454
  // String is for UTF-8 text only
  uint32 length;
  if (!input->ReadVarint32(&length)) return false;
  if (!input->InternalReadStringInline(value, length)) return false;
  return true;
}
bool WireFormatLite::ReadBytes(io::CodedInputStream* input,
                               blz::string* value) {  // line 462
  uint32 length;
  if (!input->ReadVarint32(&length)) return false;
  return input->InternalReadStringInline(value, length);
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
