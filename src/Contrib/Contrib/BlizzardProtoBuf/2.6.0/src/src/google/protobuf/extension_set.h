// Subset of protobuf 2.6.0 extension_set.h: the ExtensionSet layout (one blz::map, 0x28
// bytes) and the members used so far. The type traits and the accessor templates that
// GOOGLE_PROTOBUF_EXTENSION_ACCESSORS declares are left out, so the macro is empty here.

#pragma once

#include "stubs/common.h"
#include "repeated_field.h"
#include "../../../../../../blz/include/blz/map.h"
#include "../../../../../../blz/include/blz/string.h"
#include "../../../../../../blz/include/blz/vector.h"

namespace google {
namespace protobuf {
  class Descriptor;                                    // descriptor.h
  class FieldDescriptor;                               // descriptor.h
  class DescriptorPool;                                // descriptor.h
  class MessageLite;                                   // message_lite.h
  class Message;                                       // message.h
  class MessageFactory;                                // message.h
  class UnknownFieldSet;                               // unknown_field_set.h
  namespace io {
    class CodedInputStream;                              // coded_stream.h
    class CodedOutputStream;                             // coded_stream.h
  }
  namespace internal {
    class FieldSkipper;                                  // wire_format_lite.h
  }
}

namespace protobuf {
namespace internal {

// Used to store values of type WireFormatLite::FieldType without having to
// #include wire_format_lite.h.  Also, ensures that we use only one byte to
// store these values, which is important to keep the layout of
// ExtensionSet::Extension small.
typedef uint8 FieldType;

class LazyMessageExtension;

// Added for extension_set.cc (protobuf 2.6.1 text).
typedef bool EnumValidityFunc(int number);
typedef bool EnumValidityFuncWithArg(const void* arg, int number);

struct ExtensionInfo {
  inline ExtensionInfo() {}
  inline ExtensionInfo(FieldType type_param, bool isrepeated, bool ispacked)
      : type(type_param), is_repeated(isrepeated), is_packed(ispacked),
        descriptor(NULL) {}

  FieldType type;
  bool is_repeated;
  bool is_packed;

  struct EnumValidityCheck {
    EnumValidityFuncWithArg* func;
    const void* arg;
  };

  union {
    EnumValidityCheck enum_validity_check;
    const MessageLite* message_prototype;
  };

  // The descriptor for this extension, if one exists and is known.  May be
  // NULL.  Must not be NULL if the descriptor for the extension does not
  // live in the same pool as the descriptor for the containing type.
  const FieldDescriptor* descriptor;
};

class ExtensionFinder {
 public:
  virtual ~ExtensionFinder();

  // Find the extension with the given containing type and number.
  virtual bool Find(int number, ExtensionInfo* output) = 0;
};

class GeneratedExtensionFinder : public ExtensionFinder {
 public:
  GeneratedExtensionFinder(const MessageLite* containing_type)
      : containing_type_(containing_type) {}
  virtual ~GeneratedExtensionFinder() {}

  // Returns true and fills in *output if found, otherwise returns false.
  virtual bool Find(int number, ExtensionInfo* output);

 private:
  const MessageLite* containing_type_;
};

// The DWARF nests this class in ExtensionSet; it is completed here, at namespace scope,
// because the forward declaration above (used by Extension) is already at namespace scope.
class LazyMessageExtension {
 public:
  LazyMessageExtension() {}
  virtual ~LazyMessageExtension() {}

  virtual LazyMessageExtension* New() const = 0;
  virtual const MessageLite& GetMessage(
      const MessageLite& prototype) const = 0;
  virtual MessageLite* MutableMessage(const MessageLite& prototype) = 0;
  virtual void SetAllocatedMessage(MessageLite *message) = 0;
  virtual MessageLite* ReleaseMessage(const MessageLite& prototype) = 0;

  virtual bool IsInitialized() const = 0;
  virtual int ByteSize() const = 0;
  virtual int SpaceUsed() const = 0;

  virtual void MergeFrom(const LazyMessageExtension& other) = 0;
  virtual void Clear() = 0;

  virtual bool ReadMessage(const MessageLite& prototype,
                           io::CodedInputStream* input) = 0;
  virtual void WriteMessage(int number,
                            io::CodedOutputStream* output) const = 0;
  virtual uint8* WriteMessageToArray(int number, uint8* target) const = 0;
 private:
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(LazyMessageExtension);
};

class ExtensionSet {
 public:
  ExtensionSet();
  ~ExtensionSet();

  static void RegisterExtension(const MessageLite* containing_type,
                                int number, FieldType type,
                                bool is_repeated, bool is_packed);
  static void RegisterEnumExtension(const MessageLite* containing_type,
                                    int number, FieldType type,
                                    bool is_repeated, bool is_packed,
                                    EnumValidityFunc* is_valid);
  static void RegisterMessageExtension(const MessageLite* containing_type,
                                       int number, FieldType type,
                                       bool is_repeated, bool is_packed,
                                       const MessageLite* prototype);
  FieldType ExtensionType(int number) const;
  const void* GetRawRepeatedField(int number, const void* default_value) const;
  bool ParseField(uint32 tag, io::CodedInputStream* input,
                  ExtensionFinder* extension_finder,
                  FieldSkipper* field_skipper);
  bool ParseField(uint32 tag, io::CodedInputStream* input,
                  const MessageLite* containing_type);
  bool ParseField(uint32 tag, io::CodedInputStream* input,
                  const MessageLite* containing_type,
                  io::CodedOutputStream* unknown_fields);

  // Add all fields which are currently present to the given vector.  This
  // is useful to implement Reflection::ListFields().
  void AppendToList(const Descriptor* containing_type,
                    const DescriptorPool* pool,
                    blz::vector<const FieldDescriptor*>* output) const;

  bool Has(int number) const;
  int ExtensionSize(int number) const;   // Size of a repeated extension.
  int NumExtensions() const;  // The number of extensions
  void ClearExtension(int number);

  // singular fields -------------------------------------------------

  int32  GetInt32 (int number, int32  default_value) const;
  int64  GetInt64 (int number, int64  default_value) const;
  uint32 GetUInt32(int number, uint32 default_value) const;
  uint64 GetUInt64(int number, uint64 default_value) const;
  float  GetFloat (int number, float  default_value) const;
  double GetDouble(int number, double default_value) const;
  bool   GetBool  (int number, bool   default_value) const;
  int    GetEnum  (int number, int    default_value) const;
  const string & GetString (int number, const string&  default_value) const;
  const MessageLite& GetMessage(int number,
                                const MessageLite& default_value) const;
  const MessageLite& GetMessage(int number, const Descriptor* message_type,
                                MessageFactory* factory) const;

  void SetInt32 (int number, FieldType type, int32  value, const FieldDescriptor* descriptor);
  void SetInt64 (int number, FieldType type, int64  value, const FieldDescriptor* descriptor);
  void SetUInt32(int number, FieldType type, uint32 value, const FieldDescriptor* descriptor);
  void SetUInt64(int number, FieldType type, uint64 value, const FieldDescriptor* descriptor);
  void SetFloat (int number, FieldType type, float  value, const FieldDescriptor* descriptor);
  void SetDouble(int number, FieldType type, double value, const FieldDescriptor* descriptor);
  void SetBool  (int number, FieldType type, bool   value, const FieldDescriptor* descriptor);
  void SetEnum  (int number, FieldType type, int    value, const FieldDescriptor* descriptor);
  inline void SetString(int number, FieldType type, const string& value,
                        const FieldDescriptor* descriptor) {
    MutableString(number, type, descriptor)->assign(value);
  }
  string * MutableString(int number, FieldType type,
                         const FieldDescriptor* descriptor);
  MessageLite* MutableMessage(int number, FieldType type,
                              const MessageLite& prototype,
                              const FieldDescriptor* descriptor);
  MessageLite* MutableMessage(const FieldDescriptor* decsriptor,
                              MessageFactory* factory);
  void SetAllocatedMessage(int number, FieldType type,
                           const FieldDescriptor* descriptor,
                           MessageLite* message);
  MessageLite* ReleaseMessage(int number, const MessageLite& prototype);
  MessageLite* ReleaseMessage(const FieldDescriptor* descriptor,
                              MessageFactory* factory);

  // repeated fields -------------------------------------------------

  void* MutableRawRepeatedField(int number, FieldType field_type,
                                bool packed, const FieldDescriptor* desc);
  void* MutableRawRepeatedField(int number);

  int32  GetRepeatedInt32 (int number, int index) const;
  int64  GetRepeatedInt64 (int number, int index) const;
  uint32 GetRepeatedUInt32(int number, int index) const;
  uint64 GetRepeatedUInt64(int number, int index) const;
  float  GetRepeatedFloat (int number, int index) const;
  double GetRepeatedDouble(int number, int index) const;
  bool   GetRepeatedBool  (int number, int index) const;
  int    GetRepeatedEnum  (int number, int index) const;
  const string & GetRepeatedString (int number, int index) const;
  const MessageLite& GetRepeatedMessage(int number, int index) const;

  void SetRepeatedInt32 (int number, int index, int32  value);
  void SetRepeatedInt64 (int number, int index, int64  value);
  void SetRepeatedUInt32(int number, int index, uint32 value);
  void SetRepeatedUInt64(int number, int index, uint64 value);
  void SetRepeatedFloat (int number, int index, float  value);
  void SetRepeatedDouble(int number, int index, double value);
  void SetRepeatedBool  (int number, int index, bool   value);
  void SetRepeatedEnum  (int number, int index, int    value);
  inline void SetRepeatedString(int number, int index, const string& value) {
    MutableRepeatedString(number, index)->assign(value);
  }
  string * MutableRepeatedString(int number, int index);
  MessageLite* MutableRepeatedMessage(int number, int index);

  void AddInt32 (int number, FieldType type, bool packed, int32  value, const FieldDescriptor* descriptor);
  void AddInt64 (int number, FieldType type, bool packed, int64  value, const FieldDescriptor* descriptor);
  void AddUInt32(int number, FieldType type, bool packed, uint32 value, const FieldDescriptor* descriptor);
  void AddUInt64(int number, FieldType type, bool packed, uint64 value, const FieldDescriptor* descriptor);
  void AddFloat (int number, FieldType type, bool packed, float  value, const FieldDescriptor* descriptor);
  void AddDouble(int number, FieldType type, bool packed, double value, const FieldDescriptor* descriptor);
  void AddBool  (int number, FieldType type, bool packed, bool   value, const FieldDescriptor* descriptor);
  void AddEnum  (int number, FieldType type, bool packed, int    value, const FieldDescriptor* descriptor);
  inline void AddString(int number, FieldType type, const string& value,
                        const FieldDescriptor* descriptor) {
    AddString(number, type, descriptor)->assign(value);
  }
  string * AddString(int number, FieldType type,
                     const FieldDescriptor* descriptor);
  MessageLite* AddMessage(int number, FieldType type,
                          const MessageLite& prototype,
                          const FieldDescriptor* descriptor);
  MessageLite* AddMessage(const FieldDescriptor* descriptor,
                          MessageFactory* factory);

  void RemoveLast(int number);
  MessageLite* ReleaseLast(int number);
  void SwapElements(int number, int index1, int index2);
  void SwapExtension(ExtensionSet* other, int number);

  void Clear();
  void MergeFrom(const ExtensionSet& other);
  void Swap(ExtensionSet* other);
  bool IsInitialized() const;

  // Parses a single extension from the input, using the containing message's
  // descriptor pool and unknown field set (extension_set_heavy.cc).
  bool ParseField(uint32 tag, io::CodedInputStream* input,
                  const Message* containing_type,
                  UnknownFieldSet* unknown_fields);

  // Serializes all extensions with numbers in [start_field_number,
  // end_field_number).
  void SerializeWithCachedSizes(int start_field_number,
                                int end_field_number,
                                io::CodedOutputStream* output) const;
  uint8* SerializeWithCachedSizesToArray(int start_field_number,
                                         int end_field_number,
                                         uint8* target) const;

  int ByteSize() const;
  int SpaceUsedExcludingSelf() const;

 private:
  struct Extension {
    // The order of these fields packs Extension into 24 bytes when using 8
    // byte alignment. Consider this when adding or removing fields here.
    union {
      int32                 int32_value;
      int64                 int64_value;
      uint32                uint32_value;
      uint64                uint64_value;
      float                 float_value;
      double                double_value;
      bool                  bool_value;
      int                   enum_value;
      string*               string_value;
      MessageLite*          message_value;
      LazyMessageExtension* lazymessage_value;

      RepeatedField   <int32      >* repeated_int32_value;
      RepeatedField   <int64      >* repeated_int64_value;
      RepeatedField   <uint32     >* repeated_uint32_value;
      RepeatedField   <uint64     >* repeated_uint64_value;
      RepeatedField   <float      >* repeated_float_value;
      RepeatedField   <double     >* repeated_double_value;
      RepeatedField   <bool       >* repeated_bool_value;
      RepeatedField   <int        >* repeated_enum_value;
      RepeatedPtrField<string     >* repeated_string_value;
      RepeatedPtrField<MessageLite>* repeated_message_value;
    };

    FieldType type;
    bool is_repeated;

    // For singular types, indicates if the extension is "cleared".  This
    // happens when an extension is set and then later cleared by the caller.
    // We want to keep the Extension object around for reuse, so instead of
    // removing it from the map, we just set is_cleared = true.  This has no
    // meaning for repeated types; for those, the size of the RepeatedField
    // simply becomes zero when cleared.
    bool is_cleared : 4;

    // For singular message types, indicates whether lazy parsing is enabled
    // for this extension. This field is only valid when type == TYPE_MESSAGE
    // and !is_repeated because we only support lazy parsing for singular
    // message types currently. If is_lazy = true, the extension is stored in
    // lazymessage_value. Otherwise, the extension will be message_value.
    bool is_lazy : 4;

    // For repeated types, this indicates if the [packed=true] option is set.
    bool is_packed;

    // For packed fields, the size of the packed data is recorded here when
    // ByteSize() is called then used during serialization.
    // TODO(kenton):  Use atomic<int> when C++ supports it.
    mutable int cached_size;

    // The descriptor for this extension, if one exists and is known.  May be
    // NULL.  Must not be NULL if the descriptor for the extension does not
    // live in the same pool as the descriptor for the containing type.
    const FieldDescriptor* descriptor;

    // Some helper methods for operations on a single Extension.
    void SerializeFieldWithCachedSizes(
        int number,
        io::CodedOutputStream* output) const;
    uint8* SerializeFieldWithCachedSizesToArray(
        int number,
        uint8* target) const;
    int ByteSize(int number) const;
    void Clear();
    int GetSize() const;
    void Free();
    int SpaceUsedExcludingSelf() const;
  };

  bool FindExtensionInfoFromTag(uint32 tag, ExtensionFinder* extension_finder,
                                int* field_number, ExtensionInfo* extension,
                                bool* was_packed_on_wire);
  bool FindExtensionInfoFromFieldNumber(int wire_type, int field_number,
                                        ExtensionFinder* extension_finder,
                                        ExtensionInfo* extension,
                                        bool* was_packed_on_wire);
  bool ParseFieldWithExtensionInfo(int field_number,
                                   bool was_packed_on_wire,
                                   const ExtensionInfo& extension,
                                   io::CodedInputStream* input,
                                   FieldSkipper* field_skipper);
  bool MaybeNewExtension(int number, const FieldDescriptor* descriptor,
                         Extension** result);

  // Returns the space used by a RepeatedPtrField<Message> held as a
  // RepeatedPtrField<MessageLite> (extension_set_heavy.cc).
  static inline int RepeatedMessage_SpaceUsedExcludingSelf(
      RepeatedPtrFieldBase* field);

  // The Extension struct is small enough to be passed by value, so we use it
  // directly as the value type in the map rather than use pointers.  We use
  // a map rather than hash_map here because we expect most ExtensionSets will
  // only contain a small number of extensions whereas hash_map is optimized
  // for 100 elements or more.  Also, we want AppendToList() to order fields
  // by field number.
  blz::map<int, Extension> extensions_;

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ExtensionSet);
};
static_assert(sizeof(ExtensionSet) == 0x28, "ExtensionSet");

// Added for extension_set.cc (protobuf 2.6.1): default repeated fields.
void InitializeDefaultRepeatedFields();
void DeleteDefaultRepeatedFields();

class RepeatedPrimitiveGenericTypeTraits {
 private:
  friend void InitializeDefaultRepeatedFields();
  friend void DeleteDefaultRepeatedFields();
  static const RepeatedField<int32>* default_repeated_field_int32_;
  static const RepeatedField<int64>* default_repeated_field_int64_;
  static const RepeatedField<uint32>* default_repeated_field_uint32_;
  static const RepeatedField<uint64>* default_repeated_field_uint64_;
  static const RepeatedField<double>* default_repeated_field_double_;
  static const RepeatedField<float>* default_repeated_field_float_;
  static const RepeatedField<bool>* default_repeated_field_bool_;
};

class RepeatedStringTypeTraits {
 public:
  typedef RepeatedPtrField<string> RepeatedFieldType;
 private:
  friend void InitializeDefaultRepeatedFields();
  friend void DeleteDefaultRepeatedFields();
  static const RepeatedFieldType* default_repeated_field_;
};

class RepeatedMessageGenericTypeTraits {
 public:
  typedef RepeatedPtrField< ::google::protobuf::MessageLite*> RepeatedFieldType;
 private:
  friend void InitializeDefaultRepeatedFields();
  friend void DeleteDefaultRepeatedFields();
  static const RepeatedFieldType* default_repeated_field_;
};

// The accessor templates are not in this subset.
#define GOOGLE_PROTOBUF_EXTENSION_ACCESSORS(CLASSNAME)

}  // namespace internal
}  // namespace protobuf
}  // namespace google
