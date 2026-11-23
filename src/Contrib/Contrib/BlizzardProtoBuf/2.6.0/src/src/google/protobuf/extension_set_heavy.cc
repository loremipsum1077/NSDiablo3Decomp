// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#line 31 "C:/Projects/D3/Main/Trunk/Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/extension_set_heavy.cc"
// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.
//
// Contains methods defined in extension_set.h which cannot be part of the
// lite library because they use descriptors or reflection.

#include "descriptor.h"
#include "descriptor.pb.h"
#include "extension_set.h"
#include "message.h"
#include "repeated_field.h"
#include "wire_format.h"
#include "wire_format_lite_inl.h"

namespace google {

namespace protobuf {
namespace internal {

// A FieldSkipper used to store unknown MessageSet fields into UnknownFieldSet.
class MessageSetFieldSkipper
    : public UnknownFieldSetFieldSkipper {
 public:
  explicit MessageSetFieldSkipper(UnknownFieldSet* unknown_fields)
      : UnknownFieldSetFieldSkipper(unknown_fields) {}
  virtual ~MessageSetFieldSkipper() {}

  virtual bool SkipMessageSetField(io::CodedInputStream* input,
                                   int field_number);
};
bool MessageSetFieldSkipper::SkipMessageSetField(
    io::CodedInputStream* input, int field_number) {
  uint32 length;
  if (!input->ReadVarint32(&length)) return false;
  if (unknown_fields_ == NULL) {
    return input->Skip(length);
  } else {
    return input->ReadString(
        unknown_fields_->AddLengthDelimited(field_number), length);
  }
}


// Implementation of ExtensionFinder which finds extensions in a given
// DescriptorPool, using the given MessageFactory to construct sub-objects.
// This class is implemented in extension_set_heavy.cc.
class DescriptorPoolExtensionFinder : public ExtensionFinder {
 public:
  DescriptorPoolExtensionFinder(const DescriptorPool* pool,
                                MessageFactory* factory,
                                const Descriptor* containing_type)
      : pool_(pool), factory_(factory), containing_type_(containing_type) {}
  virtual ~DescriptorPoolExtensionFinder() {}

  virtual bool Find(int number, ExtensionInfo* output);

 private:
  const DescriptorPool* pool_;
  MessageFactory* factory_;
  const Descriptor* containing_type_;
};

void ExtensionSet::AppendToList(const Descriptor* containing_type,
                                const DescriptorPool* pool,
                                blz::vector<const FieldDescriptor*>* output) const {
  for (blz::map<int, Extension>::const_iterator iter = extensions_.begin();
       iter != extensions_.end(); ++iter) {
    bool has = false;
    if (iter->second.is_repeated) {
      has = iter->second.GetSize() > 0;
    } else {
      has = !iter->second.is_cleared;
    }

    if (has) {
      // TODO(kenton): Looking up each field by number is somewhat unfortunate.
      //   Is there a better way?  The problem is that descriptors are lazily-
      //   initialized, so they might not even be constructed until
      //   AppendToList() is called.

      if (iter->second.descriptor == NULL) {
        output->push_back(pool->FindExtensionByNumber(
            containing_type, iter->first));
      } else {
        output->push_back(iter->second.descriptor);
      }
    }
  }
}

inline FieldDescriptor::Type real_type(FieldType type) {
  GOOGLE_DCHECK(type > 0 && type <= FieldDescriptor::MAX_TYPE);
  return static_cast<FieldDescriptor::Type>(type);
}

inline FieldDescriptor::CppType cpp_type(FieldType type) {
  return FieldDescriptor::TypeToCppType(
      static_cast<FieldDescriptor::Type>(type));
}

inline WireFormatLite::FieldType field_type(FieldType type) {
  GOOGLE_DCHECK(type > 0 && type <= WireFormatLite::MAX_FIELD_TYPE);
  return static_cast<WireFormatLite::FieldType>(type);
}

#define GOOGLE_DCHECK_TYPE(EXTENSION, LABEL, CPPTYPE)                            \
  GOOGLE_DCHECK_EQ((EXTENSION).is_repeated ? FieldDescriptor::LABEL_REPEATED     \
                                  : FieldDescriptor::LABEL_OPTIONAL,      \
            FieldDescriptor::LABEL_##LABEL);                              \
  GOOGLE_DCHECK_EQ(cpp_type((EXTENSION).type), FieldDescriptor::CPPTYPE_##CPPTYPE)

const MessageLite& ExtensionSet::GetMessage(int number,
                                            const Descriptor* message_type,
                                            MessageFactory* factory) const {
  blz::map<int, Extension>::const_iterator iter = extensions_.find(number);
  if (iter == extensions_.end() || iter->second.is_cleared) {
    // Not present.  Return the default value.
    return *factory->GetPrototype(message_type);
  } else {
    GOOGLE_DCHECK_TYPE(iter->second, OPTIONAL, MESSAGE);
    if (iter->second.is_lazy) {
      return iter->second.lazymessage_value->GetMessage(
          *factory->GetPrototype(message_type));
    } else {
      return *iter->second.message_value;
    }
  }
}

MessageLite* ExtensionSet::MutableMessage(const FieldDescriptor* descriptor,
                                          MessageFactory* factory) {
  Extension* extension;
  if (MaybeNewExtension(descriptor->number(), descriptor, &extension)) {
    extension->type = descriptor->type();
    GOOGLE_DCHECK_EQ(cpp_type(extension->type), FieldDescriptor::CPPTYPE_MESSAGE);
    extension->is_repeated = false;
    extension->is_packed = false;
    const MessageLite* prototype =
        factory->GetPrototype(descriptor->message_type());
    extension->is_lazy = false;
    extension->message_value = prototype->New();
    extension->is_cleared = false;
    return extension->message_value;
  } else {
    GOOGLE_DCHECK_TYPE(*extension, OPTIONAL, MESSAGE);
    extension->is_cleared = false;
    if (extension->is_lazy) {
      return extension->lazymessage_value->MutableMessage(
          *factory->GetPrototype(descriptor->message_type()));
    } else {
      return extension->message_value;
    }
  }
}

MessageLite* ExtensionSet::ReleaseMessage(const FieldDescriptor* descriptor,
                                          MessageFactory* factory) {
  blz::map<int, Extension>::iterator iter = extensions_.find(descriptor->number());
  if (iter == extensions_.end()) {
    // Not present.  Return NULL.
    return NULL;
  } else {
    GOOGLE_DCHECK_TYPE(iter->second, OPTIONAL, MESSAGE);
    MessageLite* ret = NULL;
    if (iter->second.is_lazy) {
      ret = iter->second.lazymessage_value->ReleaseMessage(
          *factory->GetPrototype(descriptor->message_type()));
      delete iter->second.lazymessage_value;
    } else {
      ret = iter->second.message_value;
    }
    extensions_.erase(descriptor->number());
    return ret;
  }
}

MessageLite* ExtensionSet::AddMessage(const FieldDescriptor* descriptor,
                                      MessageFactory* factory) {
  Extension* extension;
  if (MaybeNewExtension(descriptor->number(), descriptor, &extension)) {
    extension->type = descriptor->type();
    GOOGLE_DCHECK_EQ(cpp_type(extension->type), FieldDescriptor::CPPTYPE_MESSAGE);
    extension->is_repeated = true;
    extension->repeated_message_value =
      new RepeatedPtrField<MessageLite>();
  } else {
    GOOGLE_DCHECK_TYPE(*extension, REPEATED, MESSAGE);
  }

  // RepeatedPtrField<Message> does not know how to Add() since it cannot
  // allocate an abstract object, so we have to be tricky.
  MessageLite* result = extension->repeated_message_value
      ->AddFromCleared<GenericTypeHandler<MessageLite> >();
  if (result == NULL) {
    const MessageLite* prototype;
    if (extension->repeated_message_value->size() == 0) {
      prototype = factory->GetPrototype(descriptor->message_type());
      GOOGLE_CHECK(prototype != NULL);
    } else {
      prototype = &extension->repeated_message_value->Get(0);
    }
    result = prototype->New();
    extension->repeated_message_value->AddAllocated(result);
  }
  return result;
}

static bool ValidateEnumUsingDescriptor(const void* arg, int number) {
  return reinterpret_cast<const EnumDescriptor*>(arg)
      ->FindValueByNumber(number) != NULL;
}

bool DescriptorPoolExtensionFinder::Find(int number, ExtensionInfo* output) {
  const FieldDescriptor* extension =
      pool_->FindExtensionByNumber(containing_type_, number);
  if (extension == NULL) {
    return false;
  } else {
    output->type = extension->type();
    output->is_repeated = extension->is_repeated();
    output->is_packed = extension->options().packed();
    output->descriptor = extension;
    if (extension->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
      output->message_prototype =
          factory_->GetPrototype(extension->message_type());
      GOOGLE_CHECK(output->message_prototype != NULL)
          << "Extension factory's GetPrototype() returned NULL for extension: "
          << extension->full_name();
    } else if (extension->cpp_type() == FieldDescriptor::CPPTYPE_ENUM) {
      output->enum_validity_check.func = ValidateEnumUsingDescriptor;
      output->enum_validity_check.arg = extension->enum_type();
    }

    return true;
  }
}

bool ExtensionSet::ParseField(uint32 tag, io::CodedInputStream* input,
                              const Message* containing_type,
                              UnknownFieldSet* unknown_fields) {
  UnknownFieldSetFieldSkipper skipper(unknown_fields);
  if (input->GetExtensionPool() == NULL) {
    GeneratedExtensionFinder finder(containing_type);
    return ParseField(tag, input, &finder, &skipper);
  } else {
    DescriptorPoolExtensionFinder finder(input->GetExtensionPool(),
                                         input->GetExtensionFactory(),
                                         containing_type->GetDescriptor());
    return ParseField(tag, input, &finder, &skipper);
  }
}

// ExtensionSet::ParseMessageSet(input, containing_type, unknown_fields) is not written yet.
#line 299
int ExtensionSet::SpaceUsedExcludingSelf() const {
  int total_size =
      extensions_.size() * sizeof(blz::map<int, Extension>::value_type);
  for (blz::map<int, Extension>::const_iterator iter = extensions_.begin(),
       end = extensions_.end();
       iter != end;
       ++iter) {
    total_size += iter->second.SpaceUsedExcludingSelf();
  }
  return total_size;
}

inline int ExtensionSet::RepeatedMessage_SpaceUsedExcludingSelf(
    RepeatedPtrFieldBase* field) {
  return field->SpaceUsedExcludingSelf<GenericTypeHandler<Message> >();
}

int ExtensionSet::Extension::SpaceUsedExcludingSelf() const {
  int total_size = 0;
  if (is_repeated) {
    switch (cpp_type(type)) {
#define HANDLE_TYPE(UPPERCASE, LOWERCASE)                          \
      case FieldDescriptor::CPPTYPE_##UPPERCASE:                   \
        total_size += sizeof(*repeated_##LOWERCASE##_value) +      \
            repeated_##LOWERCASE##_value->SpaceUsedExcludingSelf();\
        break

      HANDLE_TYPE(  INT32,   int32);
      HANDLE_TYPE(  INT64,   int64);
      HANDLE_TYPE( UINT32,  uint32);
      HANDLE_TYPE( UINT64,  uint64);
      HANDLE_TYPE(  FLOAT,   float);
      HANDLE_TYPE( DOUBLE,  double);
      HANDLE_TYPE(   BOOL,    bool);
      HANDLE_TYPE(   ENUM,    enum);
      HANDLE_TYPE( STRING,  string);
#undef HANDLE_TYPE

      case FieldDescriptor::CPPTYPE_MESSAGE:
        // repeated_message_value is actually a RepeatedPtrField<MessageLite>,
        // but MessageLite has no SpaceUsed(), so we must directly call
        // RepeatedPtrFieldBase::SpaceUsedExcludingSelf() with a different type
        // handler.
        total_size += sizeof(*repeated_message_value) +
            RepeatedMessage_SpaceUsedExcludingSelf(repeated_message_value);
        break;
    }
  } else {
    switch (cpp_type(type)) {
      case FieldDescriptor::CPPTYPE_STRING:
        total_size += sizeof(*string_value) +
                      StringSpaceUsedExcludingSelf(*string_value);
        break;
      case FieldDescriptor::CPPTYPE_MESSAGE:
        if (is_lazy) {
          total_size += lazymessage_value->SpaceUsed();
        } else {
          total_size += down_cast<Message*>(message_value)->SpaceUsed();
        }
        break;
      default:
        // No extra storage costs for primitive types.
        break;
    }
  }
  return total_size;
}

// The Serialize*ToArray methods are only needed in the heavy library, as
// the lite library only generates SerializeWithCachedSizes.
uint8* ExtensionSet::SerializeWithCachedSizesToArray(
    int start_field_number, int end_field_number,
    uint8* target) const {
  blz::map<int, Extension>::const_iterator iter;
  for (iter = extensions_.lower_bound(start_field_number);
       iter != extensions_.end() && iter->first < end_field_number;
       ++iter) {
    target = iter->second.SerializeFieldWithCachedSizesToArray(iter->first,
                                                               target);
  }
  return target;
}

// ExtensionSet::SerializeMessageSetWithCachedSizesToArray() (MessageSet support) is not written yet.
#line 392
uint8* ExtensionSet::Extension::SerializeFieldWithCachedSizesToArray(
    int number, uint8* target) const {
  if (is_repeated) {
    if (is_packed) {
      if (cached_size == 0) return target;

      target = WireFormatLite::WriteTagToArray(number,
          WireFormatLite::WIRETYPE_LENGTH_DELIMITED, target);
      target = WireFormatLite::WriteInt32NoTagToArray(cached_size, target);

      switch (real_type(type)) {
#define HANDLE_TYPE(UPPERCASE, CAMELCASE, LOWERCASE)                        \
        case FieldDescriptor::TYPE_##UPPERCASE:                             \
          for (int i = 0; i < repeated_##LOWERCASE##_value->size(); i++) {  \
            target = WireFormatLite::Write##CAMELCASE##NoTagToArray(        \
              repeated_##LOWERCASE##_value->Get(i), target);                \
          }                                                                 \
          break

        HANDLE_TYPE(   INT32,    Int32,   int32);
        HANDLE_TYPE(   INT64,    Int64,   int64);
        HANDLE_TYPE(  UINT32,   UInt32,  uint32);
        HANDLE_TYPE(  UINT64,   UInt64,  uint64);
        HANDLE_TYPE(  SINT32,   SInt32,   int32);
        HANDLE_TYPE(  SINT64,   SInt64,   int64);
        HANDLE_TYPE( FIXED32,  Fixed32,  uint32);
        HANDLE_TYPE( FIXED64,  Fixed64,  uint64);
        HANDLE_TYPE(SFIXED32, SFixed32,   int32);
        HANDLE_TYPE(SFIXED64, SFixed64,   int64);
        HANDLE_TYPE(   FLOAT,    Float,   float);
        HANDLE_TYPE(  DOUBLE,   Double,  double);
        HANDLE_TYPE(    BOOL,     Bool,    bool);
        HANDLE_TYPE(    ENUM,     Enum,    enum);
#undef HANDLE_TYPE

        case WireFormatLite::TYPE_STRING:
        case WireFormatLite::TYPE_BYTES:
        case WireFormatLite::TYPE_GROUP:
        case WireFormatLite::TYPE_MESSAGE:
          GOOGLE_LOG(FATAL) << "Non-primitive types can't be packed.";
          break;
      }
    } else {
      switch (real_type(type)) {
#define HANDLE_TYPE(UPPERCASE, CAMELCASE, LOWERCASE)                        \
        case FieldDescriptor::TYPE_##UPPERCASE:                             \
          for (int i = 0; i < repeated_##LOWERCASE##_value->size(); i++) {  \
            target = WireFormatLite::Write##CAMELCASE##ToArray(number,      \
              repeated_##LOWERCASE##_value->Get(i), target);                \
          }                                                                 \
          break

        HANDLE_TYPE(   INT32,    Int32,   int32);
        HANDLE_TYPE(   INT64,    Int64,   int64);
        HANDLE_TYPE(  UINT32,   UInt32,  uint32);
        HANDLE_TYPE(  UINT64,   UInt64,  uint64);
        HANDLE_TYPE(  SINT32,   SInt32,   int32);
        HANDLE_TYPE(  SINT64,   SInt64,   int64);
        HANDLE_TYPE( FIXED32,  Fixed32,  uint32);
        HANDLE_TYPE( FIXED64,  Fixed64,  uint64);
        HANDLE_TYPE(SFIXED32, SFixed32,   int32);
        HANDLE_TYPE(SFIXED64, SFixed64,   int64);
        HANDLE_TYPE(   FLOAT,    Float,   float);
        HANDLE_TYPE(  DOUBLE,   Double,  double);
        HANDLE_TYPE(    BOOL,     Bool,    bool);
        HANDLE_TYPE(  STRING,   String,  string);
        HANDLE_TYPE(   BYTES,    Bytes,  string);
        HANDLE_TYPE(    ENUM,     Enum,    enum);
        HANDLE_TYPE(   GROUP,    Group, message);
        HANDLE_TYPE( MESSAGE,  Message, message);
#undef HANDLE_TYPE
      }
    }
  } else if (!is_cleared) {
    switch (real_type(type)) {
#define HANDLE_TYPE(UPPERCASE, CAMELCASE, VALUE)                 \
      case FieldDescriptor::TYPE_##UPPERCASE:                    \
        target = WireFormatLite::Write##CAMELCASE##ToArray(      \
            number, VALUE, target); \
        break

      HANDLE_TYPE(   INT32,    Int32,    int32_value);
      HANDLE_TYPE(   INT64,    Int64,    int64_value);
      HANDLE_TYPE(  UINT32,   UInt32,   uint32_value);
      HANDLE_TYPE(  UINT64,   UInt64,   uint64_value);
      HANDLE_TYPE(  SINT32,   SInt32,    int32_value);
      HANDLE_TYPE(  SINT64,   SInt64,    int64_value);
      HANDLE_TYPE( FIXED32,  Fixed32,   uint32_value);
      HANDLE_TYPE( FIXED64,  Fixed64,   uint64_value);
      HANDLE_TYPE(SFIXED32, SFixed32,    int32_value);
      HANDLE_TYPE(SFIXED64, SFixed64,    int64_value);
      HANDLE_TYPE(   FLOAT,    Float,    float_value);
      HANDLE_TYPE(  DOUBLE,   Double,   double_value);
      HANDLE_TYPE(    BOOL,     Bool,     bool_value);
      HANDLE_TYPE(  STRING,   String,  *string_value);
      HANDLE_TYPE(   BYTES,    Bytes,  *string_value);
      HANDLE_TYPE(    ENUM,     Enum,     enum_value);
      HANDLE_TYPE(   GROUP,    Group, *message_value);
#undef HANDLE_TYPE
      case FieldDescriptor::TYPE_MESSAGE:
        if (is_lazy) {
          target = lazymessage_value->WriteMessageToArray(number, target);
        } else {
          target = WireFormatLite::WriteMessageToArray(
              number, *message_value, target);
        }
        break;
    }
  }
  return target;
}

}  // namespace internal
}  // namespace protobuf
}  // namespace google
