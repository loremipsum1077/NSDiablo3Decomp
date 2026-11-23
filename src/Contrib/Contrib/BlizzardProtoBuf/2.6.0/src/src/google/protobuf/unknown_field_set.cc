#include "unknown_field_set.h"

#include "stubs/common.h"
#include "io/coded_stream.h"
#include "io/zero_copy_stream.h"
#include "io/zero_copy_stream_impl_lite.h"
#include "wire_format.h"
#include "generated_message_util.h"

namespace google {
namespace protobuf {

UnknownFieldSet::UnknownFieldSet()  // line 47
    : fields_(NULL) {}

UnknownFieldSet::~UnknownFieldSet() {  // line 50
  Clear();
  delete fields_;
}

void UnknownFieldSet::ClearFallback() {  // line 55
  GOOGLE_DCHECK(fields_ != NULL);
  for (int i = 0; i < fields_->size(); i++) {
    (*fields_)[i].Delete();
  }
  fields_->clear();
}

void UnknownFieldSet::MergeFrom(const UnknownFieldSet& other) {  // line 71
  for (int i = 0; i < other.field_count(); i++) {
    AddField(other.field(i));
  }
}

int UnknownFieldSet::SpaceUsedExcludingSelf() const {  // line 77
  if (fields_ == NULL) return 0;

  int total_size = sizeof(*fields_) + sizeof(UnknownField) * fields_->size();
  for (int i = 0; i < fields_->size(); i++) {
    const UnknownField& field = (*fields_)[i];
    switch (field.type()) {
      case UnknownField::TYPE_LENGTH_DELIMITED:
        total_size += sizeof(*field.length_delimited_.string_value_) +
                      internal::StringSpaceUsedExcludingSelf(
                          *field.length_delimited_.string_value_);
        break;
      case UnknownField::TYPE_GROUP:
        total_size += field.group_->SpaceUsed();
        break;
      default:
        break;
    }
  }
  return total_size;
}

int UnknownFieldSet::SpaceUsed() const {  // line 99
  return sizeof(*this) + SpaceUsedExcludingSelf();
}

void UnknownFieldSet::AddVarint(int number, uint64 value) {  // line 103
  if (fields_ == NULL) fields_ = new blz::vector<UnknownField>;
  UnknownField field;
  field.number_ = number;
  field.SetType(UnknownField::TYPE_VARINT);
  field.varint_ = value;
  fields_->push_back(field);
}

void UnknownFieldSet::AddFixed32(int number, uint32 value) {  // line 112
  if (fields_ == NULL) fields_ = new blz::vector<UnknownField>;
  UnknownField field;
  field.number_ = number;
  field.SetType(UnknownField::TYPE_FIXED32);
  field.fixed32_ = value;
  fields_->push_back(field);
}

void UnknownFieldSet::AddFixed64(int number, uint64 value) {  // line 121
  if (fields_ == NULL) fields_ = new blz::vector<UnknownField>;
  UnknownField field;
  field.number_ = number;
  field.SetType(UnknownField::TYPE_FIXED64);
  field.fixed64_ = value;
  fields_->push_back(field);
}

blz::string* UnknownFieldSet::AddLengthDelimited(int number) {  // line 130
  if (fields_ == NULL) fields_ = new blz::vector<UnknownField>;
  UnknownField field;
  field.number_ = number;
  field.SetType(UnknownField::TYPE_LENGTH_DELIMITED);
  field.length_delimited_.string_value_ = new blz::string;
  fields_->push_back(field);
  return field.length_delimited_.string_value_;
}

UnknownFieldSet* UnknownFieldSet::AddGroup(int number) {  // line 141
  if (fields_ == NULL) fields_ = new blz::vector<UnknownField>;
  UnknownField field;
  field.number_ = number;
  field.SetType(UnknownField::TYPE_GROUP);
  field.group_ = new UnknownFieldSet;
  fields_->push_back(field);
  return field.group_;
}

void UnknownFieldSet::AddField(const UnknownField& field) {  // line 151
  if (fields_ == NULL) fields_ = new blz::vector<UnknownField>;
  fields_->push_back(field);
  fields_->back().DeepCopy();
}

bool UnknownFieldSet::MergeFromCodedStream(io::CodedInputStream* input) {  // line 190
  UnknownFieldSet other;
  if (internal::WireFormat::SkipMessage(input, &other) &&
      input->ConsumedEntireMessage()) {
    MergeFrom(other);
    return true;
  } else {
    return false;
  }
}

bool UnknownFieldSet::ParseFromCodedStream(io::CodedInputStream* input) {  // line 201
  Clear();
  return MergeFromCodedStream(input);
}

bool UnknownFieldSet::ParseFromZeroCopyStream(io::ZeroCopyInputStream* input) {  // line 206
  io::CodedInputStream coded_input(input);
  return (ParseFromCodedStream(&coded_input) &&
          coded_input.ConsumedEntireMessage());
}

bool UnknownFieldSet::ParseFromArray(const void* data, int size) {  // line 212
  io::ArrayInputStream input(data, size);
  return ParseFromZeroCopyStream(&input);
}

void UnknownField::Delete() {  // line 217
  switch (type()) {
    case UnknownField::TYPE_LENGTH_DELIMITED:
      delete length_delimited_.string_value_;
      break;
    case UnknownField::TYPE_GROUP:
      delete group_;
      break;
    default:
      break;
  }
}

void UnknownField::DeepCopy() {  // line 230
  switch (type()) {
    case UnknownField::TYPE_LENGTH_DELIMITED:
      length_delimited_.string_value_ = new blz::string(
          *length_delimited_.string_value_);
      break;
    case UnknownField::TYPE_GROUP: {
      UnknownFieldSet* group = new UnknownFieldSet;
      group->MergeFrom(*group_);
      group_ = group;
      break;
    }
    default:
      break;
  }
}

void UnknownField::SerializeLengthDelimitedNoTag(  // line 248
    io::CodedOutputStream* output) const {
  GOOGLE_DCHECK_EQ(TYPE_LENGTH_DELIMITED, type());
  const blz::string& data = *length_delimited_.string_value_;
  output->WriteVarint32(data.size());
  output->WriteRawMaybeAliased(data.data(), data.size());
}

// LINKER-REMOVED: upstream protobuf 2.6.0 (unknown_field_set.cc line 256). The original has no copy
// and this unit has no DWARF; written as upstream has it, with blz::string for string.
uint8* UnknownField::SerializeLengthDelimitedNoTagToArray(uint8* target) const {
  GOOGLE_DCHECK_EQ(TYPE_LENGTH_DELIMITED, type());
  const blz::string& data = *length_delimited_.string_value_;
  target = io::CodedOutputStream::WriteVarint32ToArray(data.size(), target);
  target = io::CodedOutputStream::WriteStringToArray(data, target);
  return target;
}

}  // namespace protobuf
}  // namespace google
