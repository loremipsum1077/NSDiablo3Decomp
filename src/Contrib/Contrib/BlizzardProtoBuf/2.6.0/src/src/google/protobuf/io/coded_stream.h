// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/io/coded_stream.h

#pragma once

#include "../../../../../../../blz/include/blz/string.h"
#include "../../../../../../../blz/include/blz/util_pair.h"
#include <string.h>

#include "../stubs/common.h"

namespace google { namespace protobuf { class DescriptorPool; } }
namespace google { namespace protobuf { class MessageFactory; } }
namespace google { namespace protobuf { namespace io { class CodedInputStream; } } }
namespace google { namespace protobuf { namespace io { class CodedOutputStream; } } }
namespace google { namespace protobuf { namespace io { class ZeroCopyInputStream; } } }
namespace google { namespace protobuf { namespace io { class ZeroCopyOutputStream; } } }

namespace google {
namespace protobuf {
namespace io {
class CodedInputStream {  // size 0x50
public:
    explicit CodedInputStream(google::protobuf::io::ZeroCopyInputStream *);
    explicit CodedInputStream(const google::protobuf::uint8 *, int);
    ~CodedInputStream();
    bool IsFlat() const;
    bool Skip(int count);
    bool GetDirectBufferPointer(const void **, int *);
    void GetDirectBufferPointerInline(const void ** data, int * size);
    bool ReadRaw(void * buffer, int size);
    bool ReadString(blz::string * buffer, int size);
    bool InternalReadStringInline(blz::string * buffer, int size) GOOGLE_ATTRIBUTE_ALWAYS_INLINE;
    bool ReadLittleEndian32(google::protobuf::uint32 * value);
    bool ReadLittleEndian64(google::protobuf::uint64 * value);
    static const google::protobuf::uint8 * ReadLittleEndian32FromArray(const google::protobuf::uint8 * buffer, google::protobuf::uint32 * value);
    static const google::protobuf::uint8 * ReadLittleEndian64FromArray(const google::protobuf::uint8 * buffer, google::protobuf::uint64 * value);
    bool ReadVarint32(google::protobuf::uint32 * value);
    bool ReadVarint64(google::protobuf::uint64 * value);
    google::protobuf::uint32 ReadTag();
    blz::pair<unsigned int, bool> ReadTagWithCutoff(google::protobuf::uint32 cutoff);
    bool ExpectTag(google::protobuf::uint32 expected);
    static const google::protobuf::uint8 * ExpectTagFromArray(const google::protobuf::uint8 * buffer, google::protobuf::uint32 expected);
    bool ExpectAtEnd();
    bool LastTagWas(google::protobuf::uint32 expected);
    bool ConsumedEntireMessage();
    typedef int Limit;
    google::protobuf::io::CodedInputStream::Limit PushLimit(int byte_limit);
    void PopLimit(google::protobuf::io::CodedInputStream::Limit limit);
    int BytesUntilLimit() const;
    int CurrentPosition() const;
    void SetTotalBytesLimit(int, int);
    int BytesUntilTotalBytesLimit() const;
    void SetRecursionLimit(int);
    bool IncrementRecursionDepth();
    void DecrementRecursionDepth();
    void SetExtensionRegistry(const google::protobuf::DescriptorPool *, google::protobuf::MessageFactory *);
    const google::protobuf::DescriptorPool * GetExtensionPool();
    google::protobuf::MessageFactory * GetExtensionFactory();
private:
    CodedInputStream(const google::protobuf::io::CodedInputStream &);
    void operator=(const google::protobuf::io::CodedInputStream &);
    google::protobuf::io::ZeroCopyInputStream * input_;  // 0x00
    const google::protobuf::uint8 * buffer_;  // 0x08
    const google::protobuf::uint8 * buffer_end_;  // 0x10
    int total_bytes_read_;  // 0x18
    int overflow_bytes_;  // 0x1c
    google::protobuf::uint32 last_tag_;  // 0x20
    bool legitimate_message_end_;  // 0x24
    bool aliasing_enabled_;  // 0x25
    google::protobuf::io::CodedInputStream::Limit current_limit_;  // 0x28
    int buffer_size_after_limit_;  // 0x2c
    int total_bytes_limit_;  // 0x30
    int total_bytes_warning_threshold_;  // 0x34
    int recursion_depth_;  // 0x38
    int recursion_limit_;  // 0x3c
    const google::protobuf::DescriptorPool * extension_pool_;  // 0x40
    google::protobuf::MessageFactory * extension_factory_;  // 0x48
    void Advance(int amount);
    void BackUpInputToCurrentPosition();
    void RecomputeBufferLimits();
    void PrintTotalBytesLimitError();
    bool Refresh();
    bool ReadVarint32Fallback(google::protobuf::uint32 * value);
    bool ReadVarint64Fallback(google::protobuf::uint64 * value);
    bool ReadVarint32Slow(google::protobuf::uint32 * value);
    bool ReadVarint64Slow(google::protobuf::uint64 * value);
    bool ReadLittleEndian32Fallback(google::protobuf::uint32 * value);
    bool ReadLittleEndian64Fallback(google::protobuf::uint64 * value);
    google::protobuf::uint32 ReadTagFallback();
    google::protobuf::uint32 ReadTagSlow();
    bool ReadStringFallback(blz::string * buffer, int size);
    int BufferSize() const;
    static const int kDefaultTotalBytesLimit = 67108864;
    static const int kDefaultTotalBytesWarningThreshold = 33554432;
    static int default_recursion_limit_;
};
static_assert(sizeof(CodedInputStream) == 0x50, "CodedInputStream");

class CodedOutputStream {  // size 0x20
public:
    explicit CodedOutputStream(google::protobuf::io::ZeroCopyOutputStream *);
    ~CodedOutputStream();
    bool Skip(int);
    bool GetDirectBufferPointer(void **, int *);
    google::protobuf::uint8 * GetDirectBufferForNBytesAndAdvance(int size);
    void WriteRaw(const void * data, int size);
    void WriteRawMaybeAliased(const void * data, int size);
    static google::protobuf::uint8 * WriteRawToArray(const void * data, int size, google::protobuf::uint8 * target);
    void WriteString(const blz::string & str);
    static google::protobuf::uint8 * WriteStringToArray(const blz::string & str, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteStringWithSizeToArray(const blz::string & str, google::protobuf::uint8 * target);
    void EnableAliasing(bool);
    void WriteLittleEndian32(google::protobuf::uint32 value);
    static google::protobuf::uint8 * WriteLittleEndian32ToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target);
    void WriteLittleEndian64(google::protobuf::uint64 value);
    static google::protobuf::uint8 * WriteLittleEndian64ToArray(google::protobuf::uint64 value, google::protobuf::uint8 * target);
    void WriteVarint32(google::protobuf::uint32 value);
    static google::protobuf::uint8 * WriteVarint32ToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target);
    void WriteVarint64(google::protobuf::uint64 value);
    static google::protobuf::uint8 * WriteVarint64ToArray(google::protobuf::uint64 value, google::protobuf::uint8 * target);
    void WriteVarint32SignExtended(google::protobuf::int32 value);
    static google::protobuf::uint8 * WriteVarint32SignExtendedToArray(google::protobuf::int32 value, google::protobuf::uint8 * target);
    void WriteTag(google::protobuf::uint32 value);
    static google::protobuf::uint8 * WriteTagToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target);
    static int VarintSize32(google::protobuf::uint32 value);
    static int VarintSize64(google::protobuf::uint64 value);
    static int VarintSize32SignExtended(google::protobuf::int32 value);
    int ByteCount() const;
    bool HadError() const;
private:
    CodedOutputStream(const google::protobuf::io::CodedOutputStream &);
    void operator=(const google::protobuf::io::CodedOutputStream &);
    google::protobuf::io::ZeroCopyOutputStream * output_;  // 0x00
    google::protobuf::uint8 * buffer_;  // 0x08
    int buffer_size_;  // 0x10
    int total_bytes_;  // 0x14
    bool had_error_;  // 0x18
    bool aliasing_enabled_;  // 0x19
    void Advance(int amount);
    bool Refresh();
    void WriteAliasedRaw(const void * data, int size);
    static google::protobuf::uint8 * WriteVarint32FallbackToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteVarint32FallbackToArrayInline(google::protobuf::uint32 value, google::protobuf::uint8 * target);
    static google::protobuf::uint8 * WriteVarint64ToArrayInline(google::protobuf::uint64 value, google::protobuf::uint8 * target);
    static int VarintSize32Fallback(google::protobuf::uint32 value);
};
static_assert(sizeof(CodedOutputStream) == 0x20, "CodedOutputStream");

// inline methods ====================================================
// The vast majority of varints are only one byte.  These inline
// methods optimize for that case.

inline bool CodedInputStream::ReadVarint32(uint32* value) {
  if (GOOGLE_PREDICT_TRUE(buffer_ < buffer_end_) && *buffer_ < 0x80) {
    *value = *buffer_;
    Advance(1);
    return true;
  } else {
    return ReadVarint32Fallback(value);
  }
}

inline bool CodedInputStream::ReadVarint64(uint64* value) {
  if (GOOGLE_PREDICT_TRUE(buffer_ < buffer_end_) && *buffer_ < 0x80) {
    *value = *buffer_;
    Advance(1);
    return true;
  } else {
    return ReadVarint64Fallback(value);
  }
}

// static
inline const uint8* CodedInputStream::ReadLittleEndian32FromArray(
    const uint8* buffer,
    uint32* value) {
  memcpy(value, buffer, sizeof(*value));
  return buffer + sizeof(*value);
}

// static
inline const uint8* CodedInputStream::ReadLittleEndian64FromArray(
    const uint8* buffer,
    uint64* value) {
  memcpy(value, buffer, sizeof(*value));
  return buffer + sizeof(*value);
}

inline bool CodedInputStream::ReadLittleEndian32(uint32* value) {
  if (GOOGLE_PREDICT_TRUE(BufferSize() >= static_cast<int>(sizeof(*value)))) {
    memcpy(value, buffer_, sizeof(*value));
    Advance(sizeof(*value));
    return true;
  } else {
    return ReadLittleEndian32Fallback(value);
  }
}

inline bool CodedInputStream::ReadLittleEndian64(uint64* value) {
  if (GOOGLE_PREDICT_TRUE(BufferSize() >= static_cast<int>(sizeof(*value)))) {
    memcpy(value, buffer_, sizeof(*value));
    Advance(sizeof(*value));
    return true;
  } else {
    return ReadLittleEndian64Fallback(value);
  }
}

inline uint32 CodedInputStream::ReadTag() {
  if (GOOGLE_PREDICT_TRUE(buffer_ < buffer_end_) && buffer_[0] < 0x80) {
    last_tag_ = buffer_[0];
    Advance(1);
    return last_tag_;
  } else {
    last_tag_ = ReadTagFallback();
    return last_tag_;
  }
}

inline blz::pair<uint32, bool> CodedInputStream::ReadTagWithCutoff(
    uint32 cutoff) {
  if (GOOGLE_PREDICT_TRUE(buffer_ < buffer_end_)) {
    if (static_cast<int8>(buffer_[0]) > 0) {
      const uint32 kMax1ByteVarint = 0x7f;
      uint32 tag = last_tag_ = buffer_[0];
      Advance(1);
      return blz::make_pair(tag, cutoff >= kMax1ByteVarint || tag <= cutoff);
    }
    if (cutoff >= 0x80 &&
        GOOGLE_PREDICT_TRUE(buffer_ + 1 < buffer_end_) &&
        GOOGLE_PREDICT_TRUE((buffer_[0] & ~buffer_[1]) >= 0x80)) {
      const uint32 kMax2ByteVarint = (0x7f << 7) + 0x7f;
      uint32 tag = last_tag_ = (1u << 7) * buffer_[1] + (buffer_[0] - 0x80);
      Advance(2);
      bool at_or_below_cutoff = cutoff >= kMax2ByteVarint || tag <= cutoff;
      return blz::make_pair(tag, at_or_below_cutoff);
    }
  }
  // Slow path
  last_tag_ = ReadTagFallback();
  return blz::make_pair(last_tag_, static_cast<uint32>(last_tag_ - 1) < cutoff);
}

inline bool CodedInputStream::LastTagWas(uint32 expected) {
  return last_tag_ == expected;
}

inline bool CodedInputStream::ConsumedEntireMessage() {
  return legitimate_message_end_;
}

inline bool CodedInputStream::ExpectTag(uint32 expected) {
  if (expected < (1 << 7)) {
    if (GOOGLE_PREDICT_TRUE(buffer_ < buffer_end_) && buffer_[0] == expected) {
      Advance(1);
      return true;
    } else {
      return false;
    }
  } else if (expected < (1 << 14)) {
    if (GOOGLE_PREDICT_TRUE(BufferSize() >= 2) &&
        buffer_[0] == static_cast<uint8>(expected | 0x80) &&
        buffer_[1] == static_cast<uint8>(expected >> 7)) {
      Advance(2);
      return true;
    } else {
      return false;
    }
  } else {
    // Don't bother optimizing for larger values.
    return false;
  }
}

inline const uint8* CodedInputStream::ExpectTagFromArray(
    const uint8* buffer, uint32 expected) {
  if (expected < (1 << 7)) {
    if (buffer[0] == expected) {
      return buffer + 1;
    }
  } else if (expected < (1 << 14)) {
    if (buffer[0] == static_cast<uint8>(expected | 0x80) &&
        buffer[1] == static_cast<uint8>(expected >> 7)) {
      return buffer + 2;
    }
  }
  return NULL;
}

inline void CodedInputStream::GetDirectBufferPointerInline(const void** data,
                                                           int* size) {
  *data = buffer_;
  *size = buffer_end_ - buffer_;
}

inline bool CodedInputStream::ExpectAtEnd() {
  if (buffer_ == buffer_end_ &&
      ((buffer_size_after_limit_ != 0) ||
       (total_bytes_read_ == current_limit_))) {
    last_tag_ = 0;
    legitimate_message_end_ = true;
    return true;
  } else {
    return false;
  }
}

inline int CodedInputStream::CurrentPosition() const {
  return total_bytes_read_ - (BufferSize() + buffer_size_after_limit_);
}

inline uint8* CodedOutputStream::GetDirectBufferForNBytesAndAdvance(int size) {
  if (buffer_size_ < size) {
    return NULL;
  } else {
    uint8* result = buffer_;
    Advance(size);
    return result;
  }
}

inline uint8* CodedOutputStream::WriteVarint32ToArray(uint32 value,
                                                        uint8* target) {
  if (value < 0x80) {
    *target = value;
    return target + 1;
  } else {
    return WriteVarint32FallbackToArray(value, target);
  }
}

inline void CodedOutputStream::WriteVarint32SignExtended(int32 value) {
  if (value < 0) {
    WriteVarint64(static_cast<uint64>(value));
  } else {
    WriteVarint32(static_cast<uint32>(value));
  }
}

inline uint8* CodedOutputStream::WriteVarint32SignExtendedToArray(
    int32 value, uint8* target) {
  if (value < 0) {
    return WriteVarint64ToArray(static_cast<uint64>(value), target);
  } else {
    return WriteVarint32ToArray(static_cast<uint32>(value), target);
  }
}

inline uint8* CodedOutputStream::WriteLittleEndian32ToArray(uint32 value,
                                                            uint8* target) {
  memcpy(target, &value, sizeof(value));
  return target + sizeof(value);
}

inline uint8* CodedOutputStream::WriteLittleEndian64ToArray(uint64 value,
                                                            uint8* target) {
  memcpy(target, &value, sizeof(value));
  return target + sizeof(value);
}

inline void CodedOutputStream::WriteTag(uint32 value) {
  WriteVarint32(value);
}

inline uint8* CodedOutputStream::WriteTagToArray(
    uint32 value, uint8* target) {
  if (value < (1 << 7)) {
    target[0] = value;
    return target + 1;
  } else if (value < (1 << 14)) {
    target[0] = static_cast<uint8>(value | 0x80);
    target[1] = static_cast<uint8>(value >> 7);
    return target + 2;
  } else {
    return WriteVarint32FallbackToArray(value, target);
  }
}

inline int CodedOutputStream::VarintSize32(uint32 value) {
  if (value < (1 << 7)) {
    return 1;
  } else  {
    return VarintSize32Fallback(value);
  }
}

inline int CodedOutputStream::VarintSize32SignExtended(int32 value) {
  if (value < 0) {
    return 10;
  } else {
    return VarintSize32(static_cast<uint32>(value));
  }
}

inline void CodedOutputStream::WriteString(const blz::string& str) {
  WriteRaw(str.data(), static_cast<int>(str.size()));
}

inline void CodedOutputStream::WriteRawMaybeAliased(
    const void* data, int size) {
  if (aliasing_enabled_) {
    WriteAliasedRaw(data, size);
  } else {
    WriteRaw(data, size);
  }
}

inline uint8* CodedOutputStream::WriteStringToArray(
    const blz::string& str, uint8* target) {
  return WriteRawToArray(str.data(), static_cast<int>(str.size()), target);
}

inline int CodedOutputStream::ByteCount() const {
  return total_bytes_ - buffer_size_;
}

inline void CodedInputStream::Advance(int amount) {
  buffer_ += amount;
}

inline void CodedOutputStream::Advance(int amount) {
  buffer_ += amount;
  buffer_size_ -= amount;
}

inline bool CodedInputStream::IncrementRecursionDepth() {
  ++recursion_depth_;
  return recursion_depth_ <= recursion_limit_;
}

inline void CodedInputStream::DecrementRecursionDepth() {
  if (recursion_depth_ > 0) --recursion_depth_;
}

inline const DescriptorPool* CodedInputStream::GetExtensionPool() {
  return extension_pool_;
}

inline MessageFactory* CodedInputStream::GetExtensionFactory() {
  return extension_factory_;
}

inline int CodedInputStream::BufferSize() const {
  return buffer_end_ - buffer_;
}

inline CodedInputStream::CodedInputStream(ZeroCopyInputStream* input)
  : input_(input),
    buffer_(NULL),
    buffer_end_(NULL),
    total_bytes_read_(0),
    overflow_bytes_(0),
    last_tag_(0),
    legitimate_message_end_(false),
    aliasing_enabled_(false),
    current_limit_(kint32max),
    buffer_size_after_limit_(0),
    total_bytes_limit_(kDefaultTotalBytesLimit),
    total_bytes_warning_threshold_(kDefaultTotalBytesWarningThreshold),
    recursion_depth_(0),
    recursion_limit_(default_recursion_limit_),
    extension_pool_(NULL),
    extension_factory_(NULL) {
  // Eagerly Refresh() so buffer space is immediately available.
  Refresh();
}

inline CodedInputStream::CodedInputStream(const uint8* buffer, int size)
  : input_(NULL),
    buffer_(buffer),
    buffer_end_(buffer + size),
    total_bytes_read_(size),
    overflow_bytes_(0),
    last_tag_(0),
    legitimate_message_end_(false),
    aliasing_enabled_(false),
    current_limit_(size),
    buffer_size_after_limit_(0),
    total_bytes_limit_(kDefaultTotalBytesLimit),
    total_bytes_warning_threshold_(kDefaultTotalBytesWarningThreshold),
    recursion_depth_(0),
    recursion_limit_(default_recursion_limit_),
    extension_pool_(NULL),
    extension_factory_(NULL) {
}

inline bool CodedOutputStream::HadError() const {
  return had_error_;
}

}  // namespace io
}  // namespace protobuf
}  // namespace google

// Functions defined in this file (bodies are not in the DWARF):
//   line   749  bool google::protobuf::io::CodedOutputStream::HadError() const  [inlined into 2 functions]
//   line   793  bool google::protobuf::io::CodedInputStream::ReadVarint32(google::protobuf::uint32 * value)  [inlined into 28 functions]
//   line   803  bool google::protobuf::io::CodedInputStream::ReadVarint64(google::protobuf::uint64 * value)  [inlined into 13 functions]
//   line   814  const google::protobuf::uint8 * google::protobuf::io::CodedInputStream::ReadLittleEndian32FromArray(const google::protobuf::uint8 * buffer, google::protobuf::uint32 * value)  [inlined into 1 functions]
//   line   829  const google::protobuf::uint8 * google::protobuf::io::CodedInputStream::ReadLittleEndian64FromArray(const google::protobuf::uint8 * buffer, google::protobuf::uint64 * value)  [inlined into 1 functions]
//   line   850  bool google::protobuf::io::CodedInputStream::ReadLittleEndian32(google::protobuf::uint32 * value)  [inlined into 3 functions]
//   line   864  bool google::protobuf::io::CodedInputStream::ReadLittleEndian64(google::protobuf::uint64 * value)  [inlined into 4 functions]
//   line   878  google::protobuf::uint32 google::protobuf::io::CodedInputStream::ReadTag()  [inlined into 5 functions]
//   line   889  blz::pair<unsigned int, bool> google::protobuf::io::CodedInputStream::ReadTagWithCutoff(google::protobuf::uint32 cutoff)  [inlined into 21 functions]
//   line   928  bool google::protobuf::io::CodedInputStream::LastTagWas(google::protobuf::uint32 expected)  [inlined into 3 functions]
//   line   932  bool google::protobuf::io::CodedInputStream::ConsumedEntireMessage()  [inlined into 26 functions]
//   line   936  bool google::protobuf::io::CodedInputStream::ExpectTag(google::protobuf::uint32 expected)  [inlined into 21 functions]
//   line   980  bool google::protobuf::io::CodedInputStream::ExpectAtEnd()  [inlined into 21 functions]
//   line   995  int google::protobuf::io::CodedInputStream::CurrentPosition() const  [inlined into 4 functions]
//   line   999  google::protobuf::uint8 * google::protobuf::io::CodedOutputStream::GetDirectBufferForNBytesAndAdvance(int size)  [inlined into 1 functions]
//   line  1009  google::protobuf::uint8 * google::protobuf::io::CodedOutputStream::WriteVarint32ToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target)  [inlined into 22 functions]
//   line  1019  void google::protobuf::io::CodedOutputStream::WriteVarint32SignExtended(google::protobuf::int32 value)  [inlined into 5 functions]
//   line  1027  google::protobuf::uint8 * google::protobuf::io::CodedOutputStream::WriteVarint32SignExtendedToArray(google::protobuf::int32 value, google::protobuf::uint8 * target)  [inlined into 8 functions]
//   line  1036  google::protobuf::uint8 * google::protobuf::io::CodedOutputStream::WriteLittleEndian32ToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target)  [inlined into 3 functions]
//   line  1049  google::protobuf::uint8 * google::protobuf::io::CodedOutputStream::WriteLittleEndian64ToArray(google::protobuf::uint64 value, google::protobuf::uint8 * target)  [inlined into 4 functions]
//   line  1069  void google::protobuf::io::CodedOutputStream::WriteTag(google::protobuf::uint32 value)  [inlined into 24 functions]
//   line  1073  google::protobuf::uint8 * google::protobuf::io::CodedOutputStream::WriteTagToArray(google::protobuf::uint32 value, google::protobuf::uint8 * target)  [inlined into 23 functions]
//   line  1087  int google::protobuf::io::CodedOutputStream::VarintSize32(google::protobuf::uint32 value)  [inlined into 26 functions]
//   line  1095  int google::protobuf::io::CodedOutputStream::VarintSize32SignExtended(google::protobuf::int32 value)  [inlined into 10 functions]
//   line  1103  void google::protobuf::io::CodedOutputStream::WriteString(const blz::string & str)  [inlined into 3 functions]
//   line  1107  void google::protobuf::io::CodedOutputStream::WriteRawMaybeAliased(const void * data, int size)  [inlined into 4 functions]
//   line  1116  google::protobuf::uint8 * google::protobuf::io::CodedOutputStream::WriteStringToArray(const blz::string & str, google::protobuf::uint8 * target)  [inlined into 1 functions]
//   line  1121  int google::protobuf::io::CodedOutputStream::ByteCount() const  [inlined into 1 functions]
//   line  1125  void google::protobuf::io::CodedInputStream::Advance(int amount)  [inlined into 41 functions]
//   line  1129  void google::protobuf::io::CodedOutputStream::Advance(int amount)  [inlined into 7 functions]
//   line  1138  bool google::protobuf::io::CodedInputStream::IncrementRecursionDepth()  [inlined into 21 functions]
//   line  1143  void google::protobuf::io::CodedInputStream::DecrementRecursionDepth()  [inlined into 21 functions]
//   line  1153  const google::protobuf::DescriptorPool * google::protobuf::io::CodedInputStream::GetExtensionPool()  [inlined into 2 functions]
//   line  1157  google::protobuf::MessageFactory * google::protobuf::io::CodedInputStream::GetExtensionFactory()  [inlined into 3 functions]
//   line  1161  int google::protobuf::io::CodedInputStream::BufferSize() const  [inlined into 27 functions]
//   line  1165  google::protobuf::io::CodedInputStream::CodedInputStream(google::protobuf::io::ZeroCopyInputStream * input)  [inlined into 2 functions]
//   line  1186  google::protobuf::io::CodedInputStream::CodedInputStream(const google::protobuf::uint8 * buffer, int size)  [inlined into 4 functions]
