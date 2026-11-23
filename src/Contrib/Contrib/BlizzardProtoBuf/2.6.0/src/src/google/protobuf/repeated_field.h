// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/repeated_field.h
// Subset: RepeatedField<Element> and RepeatedPtrField<Element> (written by hand after
// protobuf 2.6.0; the DWARF only shows instantiations). The RepeatedPtrFieldBase members
// that GOOGLE_CHECK sit on their original lines.
#line 7 "c:/package_cache/Blizzard Protocol Buffers/Hydra--2.6.0K/noarch/include\\google/protobuf/repeated_field.h"
#pragma once

#include <string.h>

#include "stubs/common.h"
#include "../../../../../../blz/include/blz/algo_minmax.h"
#include "../../../../../../blz/include/blz/algo_seq.h"

namespace google {
namespace protobuf {

namespace internal {

// line 71
static const int kMinRepeatedFieldAllocationSize = 4;

// line 222
template <typename Element, bool HasTrivialCopy = blz::is_pod<Element>::value>
struct ElementCopier {
    void operator()(Element* to, const Element* from, int array_size);
};

}  // namespace internal

// line 99
template <typename Element>
class RepeatedField {
public:
    RepeatedField();
    ~RepeatedField();

    bool empty() const;
    int size() const;

    const Element& Get(int index) const;
    Element* Mutable(int index);
    void Set(int index, const Element& value);
    void Add(const Element& value);
    Element* Add();
    void Resize(int new_size, const Element& value);
    void Truncate(int new_size);  void ExtractSubrange(int start, int num, Element* elements);  // original line 698
    void RemoveLast();
    void Clear();
    void MergeFrom(const RepeatedField& other);
    void Reserve(int new_size);
    void Swap(RepeatedField* other);
    void AddAlreadyReserved(const Element& value);
    int Capacity() const;  int SpaceUsedExcludingSelf() const;  void SwapElements(int index1, int index2);
    Element* mutable_data();
    const Element* data() const;

    typedef Element* iterator;
    typedef const Element* const_iterator;
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;

private:
    static const int kInitialSize = 0;

    Element* elements_;
    int current_size_;
    int total_size_;

    void MoveArray(Element* to, Element* from, int array_size);
    void CopyArray(Element* to, const Element* from, int array_size);
};

// line 575
template <typename Element>
inline RepeatedField<Element>::RepeatedField() : elements_(NULL), current_size_(0), total_size_(kInitialSize) {
}

// line 609
template <typename Element>
RepeatedField<Element>::~RepeatedField() {
    delete[] elements_;
}

template <typename Element>
inline bool RepeatedField<Element>::empty() const {
    return current_size_ == 0;
}

// line 627
template <typename Element>
inline int RepeatedField<Element>::size() const {
    return current_size_;
}

template <typename Element>
inline int RepeatedField<Element>::Capacity() const {
    return total_size_;
}

// line 637
template <typename Element>
inline void RepeatedField<Element>::AddAlreadyReserved(const Element& value) {
    elements_[current_size_++] = value;
}

// line 649
template <typename Element>
inline void RepeatedField<Element>::Resize(int new_size, const Element& value) {
    GOOGLE_DCHECK_GE(new_size, 0);
    if (new_size > size()) {
        Reserve(new_size);
        blz::fill(&elements_[current_size_], &elements_[new_size], value);
    }
    current_size_ = new_size;
}

// line 659
template <typename Element>
inline const Element& RepeatedField<Element>::Get(int index) const {
    return elements_[index];
}

template <typename Element>
inline Element* RepeatedField<Element>::Mutable(int index) {
    return elements_ + index;
}

// line 673
template <typename Element>
inline void RepeatedField<Element>::Set(int index, const Element& value) {
    elements_[index] = value;
}

// line 680
template <typename Element>
inline void RepeatedField<Element>::Add(const Element& value) {
    if (current_size_ == total_size_)
        Reserve(total_size_ + 1);
    elements_[current_size_++] = value;
}

template <typename Element>
inline Element* RepeatedField<Element>::Add() {
    if (current_size_ == total_size_)
        Reserve(total_size_ + 1);
    return &elements_[current_size_++];
}

// line 692
template <typename Element>
inline void RepeatedField<Element>::RemoveLast() {
    --current_size_;
}

// line 719
template <typename Element>
inline void RepeatedField<Element>::Clear() {
    current_size_ = 0;
}

template <typename Element>
inline Element* RepeatedField<Element>::mutable_data() {
    return elements_;
}

template <typename Element>
inline const Element* RepeatedField<Element>::data() const {
    return elements_;
}

template <typename Element>
inline typename RepeatedField<Element>::iterator RepeatedField<Element>::begin() {
    return elements_;
}

// line 780
template <typename Element>
inline typename RepeatedField<Element>::const_iterator RepeatedField<Element>::begin() const {
    return elements_;
}

template <typename Element>
inline typename RepeatedField<Element>::iterator RepeatedField<Element>::end() {
    return elements_ + current_size_;
}

// line 790
template <typename Element>
inline typename RepeatedField<Element>::const_iterator RepeatedField<Element>::end() const {
    return elements_ + current_size_;
}

// line 752
template <typename Element>
void RepeatedField<Element>::Swap(RepeatedField* other) {
    if (this == other) return;
    Element* swap_elements = elements_;
    int swap_current_size = current_size_;
    int swap_total_size = total_size_;

    elements_ = other->elements_;
    current_size_ = other->current_size_;
    total_size_ = other->total_size_;

    other->elements_ = swap_elements;
    other->current_size_ = swap_current_size;
    other->total_size_ = swap_total_size;
}

// line 802
template <typename Element>
void RepeatedField<Element>::Reserve(int new_size) {
    if (total_size_ >= new_size)
        return;

    Element* old_elements = elements_;
    total_size_ = blz::max(internal::kMinRepeatedFieldAllocationSize, blz::max(total_size_ * 2, new_size));
    elements_ = new Element[total_size_];
    if (old_elements != NULL) {
        MoveArray(elements_, old_elements, current_size_);
        delete[] old_elements;
    }
}

// line 816
template <typename Element>
inline void RepeatedField<Element>::Truncate(int new_size) {
    GOOGLE_DCHECK_LE(new_size, current_size_);
    current_size_ = new_size;
}

// line 822
template <typename Element>
inline void RepeatedField<Element>::MoveArray(Element* to, Element* from, int array_size) {
    CopyArray(to, from, array_size);
}

// line 828
template <typename Element>
inline void RepeatedField<Element>::CopyArray(Element* to, const Element* from, int array_size) {
    internal::ElementCopier<Element>()(to, from, array_size);
}

namespace internal {

// line 836
template <typename Element, bool HasTrivialCopy>
void ElementCopier<Element, HasTrivialCopy>::operator()(Element* to, const Element* from, int array_size) {
    blz::copy(from, from + array_size, to);
}
template <typename Element>
struct ElementCopier<Element, true> {
    void operator()(Element* to, const Element* from, int array_size) { memcpy(to, from, array_size * sizeof(Element)); }
};

// line 241
class RepeatedPtrFieldBase {  // size 0x18
protected:
    friend class GeneratedMessageReflection;
    friend class ExtensionSet;

    RepeatedPtrFieldBase();

    template <typename TypeHandler>
    void Destroy();

    bool empty() const;
    int size() const;

    template <typename TypeHandler>
    const typename TypeHandler::Type& Get(int index) const;
    template <typename TypeHandler>
    typename TypeHandler::Type* Mutable(int index);
    template <typename TypeHandler>
    typename TypeHandler::Type* Add();
    template <typename TypeHandler>
    void RemoveLast();
    template <typename TypeHandler>
    void Clear();
    template <typename TypeHandler>
    void MergeFrom(const RepeatedPtrFieldBase& other);
    template <typename TypeHandler>
    void CopyFrom(const RepeatedPtrFieldBase& other);

    void Reserve(int new_size);  template <typename TypeHandler> int SpaceUsedExcludingSelf() const;  template <typename TypeHandler> typename TypeHandler::Type* AddFromCleared();  template <typename TypeHandler> void AddAllocated(typename TypeHandler::Type* value);  template <typename TypeHandler> typename TypeHandler::Type* ReleaseLast();
    const void* const* raw_data() const { return elements_; }  // original line 941

    void Swap(RepeatedPtrFieldBase* other);
    // Original line 964; inlined by AttributeSerializer_RemoveInt.
    void SwapElements(int index1, int index2) {
        blz::swap(elements_[index1], elements_[index2]);
    }  void CloseGap(int start, int num);  // original line 288

private:
    static const int kInitialSize = 0;

    void** elements_;
    int current_size_;
    int allocated_size_;
    int total_size_;

    template <typename TypeHandler>
    static inline typename TypeHandler::Type* cast(void* element) {
        return reinterpret_cast<typename TypeHandler::Type*>(element);
    }
    template <typename TypeHandler>
    static inline const typename TypeHandler::Type* cast(const void* element) {
        return reinterpret_cast<const typename TypeHandler::Type*>(element);
    }

    RepeatedPtrFieldBase(const RepeatedPtrFieldBase&);
    void operator=(const RepeatedPtrFieldBase&);
};

// line 355
template <typename GenericType>
class GenericTypeHandler {
public:
    typedef GenericType Type;
    static GenericType* New() { return new GenericType; }
    static void Delete(GenericType* value) { delete value; }
    static void Clear(GenericType* value) { value->Clear(); }
    static int SpaceUsed(const GenericType& value) { return value.SpaceUsed(); }  static void Merge(const GenericType& from, GenericType* to) {
        to->MergeFrom(from);
    }
};

}  // namespace internal

namespace internal { template <typename Element> class RepeatedPtrIterator; }
// line 426
template <typename Element>
class RepeatedPtrField : public internal::RepeatedPtrFieldBase {
public:
    RepeatedPtrField();
    ~RepeatedPtrField();  RepeatedPtrField& operator=(const RepeatedPtrField& other);  // original line 434 (public; DWARF RepeatedPtrField<HeroSettingsData_114_115>::operator=)

    bool empty() const;
    int size() const;

    const Element& Get(int index) const;
    Element* Mutable(int index);
    Element* Add();
    void RemoveLast();
    void Clear();  int SpaceUsedExcludingSelf() const;  void AddAllocated(Element* value);  Element* ReleaseLast();  void DeleteSubrange(int start, int num);  void ExtractSubrange(int start, int num, Element** elements);
    void MergeFrom(const RepeatedPtrField& other);
    void CopyFrom(const RepeatedPtrField& other);
    typedef internal::RepeatedPtrIterator<const Element> const_iterator;
    const_iterator begin() const;
    const_iterator end() const;  typedef internal::RepeatedPtrIterator<Element> iterator; iterator begin(); iterator end();  // lines 1425/1435

    void Swap(RepeatedPtrField* other);
    // Original line 1188.
    void SwapElements(int index1, int index2) {
        RepeatedPtrFieldBase::SwapElements(index1, index2);
    }

protected:
    class TypeHandler;

private:
    RepeatedPtrField(const RepeatedPtrField&);

};

// Blank lines keep the GOOGLE_CHECKs below on their original lines (725 and 923).


namespace internal {

// line 402
class StringTypeHandlerBase {
public:
    typedef blz::string Type;
    static blz::string* New();  // defined in repeated_field.cc
    static void Delete(blz::string* value);
    static inline void Clear(blz::string* value) { value->clear(); }
    static inline void Merge(const blz::string& from, blz::string* to) { *to = from; }
};

int StringSpaceUsedExcludingSelf(const blz::string& str);  class StringTypeHandler : public StringTypeHandlerBase { public: static int SpaceUsed(const blz::string& value) { return sizeof(value) + StringSpaceUsedExcludingSelf(value); } };

}  // namespace internal

// line 532
template <>
class RepeatedPtrField<blz::string>::TypeHandler
    : public internal::StringTypeHandler {
};


namespace internal {

// line 1260. Only what the const iteration in CosmeticItemData::Import (CosmeticItems.cpp)
// uses: operator* (1282), operator-> (1283), operator++ (1286), operator!= (1293).
template <typename Element>
class RepeatedPtrIterator {
public:
    typedef RepeatedPtrIterator<Element> iterator;
    typedef Element& reference;
    typedef Element* pointer;

    RepeatedPtrIterator() : it_(NULL) {}
    explicit RepeatedPtrIterator(const void* const* it) : it_(it) {}

    reference operator*() const { return *reinterpret_cast<Element*>(const_cast<void*>(*it_)); }
    pointer operator->() const { return &(operator*()); }

    iterator& operator++() { ++it_; return *this; }

    bool operator!=(const iterator& x) const { return it_ != x.it_; }

private:
    const void* const* it_;
};

}  // namespace internal

// line 1430
template <typename Element>
inline typename RepeatedPtrField<Element>::const_iterator RepeatedPtrField<Element>::begin() const {
    return const_iterator(RepeatedPtrFieldBase::raw_data());
}

// line 1440
template <typename Element>
inline typename RepeatedPtrField<Element>::const_iterator RepeatedPtrField<Element>::end() const {
    return const_iterator(RepeatedPtrFieldBase::raw_data() + size());
}

template <typename Element> inline typename RepeatedPtrField<Element>::iterator RepeatedPtrField<Element>::begin() { return iterator(RepeatedPtrFieldBase::raw_data()); }  template <typename Element> inline typename RepeatedPtrField<Element>::iterator RepeatedPtrField<Element>::end() { return iterator(RepeatedPtrFieldBase::raw_data() + size()); }  // lines 1425/1435, non-const pair































































































































































































































































































// line 724
template <typename Element>
inline void RepeatedField<Element>::MergeFrom(const RepeatedField& other) {
    GOOGLE_CHECK_NE(&other, this);
    if (other.current_size_ != 0) {
        Reserve(current_size_ + other.current_size_);
        CopyArray(elements_ + current_size_, other.elements_, other.current_size_);
        current_size_ += other.current_size_;
    }
}

























































































































namespace internal {

inline RepeatedPtrFieldBase::RepeatedPtrFieldBase()
  : elements_(NULL),
    current_size_(0),
    allocated_size_(0),
    total_size_(kInitialSize) {
}

template <typename TypeHandler>
void RepeatedPtrFieldBase::Destroy() {
  for (int i = 0; i < allocated_size_; i++) {
    TypeHandler::Delete(cast<TypeHandler>(elements_[i]));
  }
  delete [] elements_;
}

inline bool RepeatedPtrFieldBase::empty() const {
  return current_size_ == 0;
}

inline int RepeatedPtrFieldBase::size() const {
  return current_size_;
}

template <typename TypeHandler>
inline const typename TypeHandler::Type&
RepeatedPtrFieldBase::Get(int index) const {
  GOOGLE_DCHECK_GE(index, 0);
  GOOGLE_DCHECK_LT(index, size());
  return *cast<TypeHandler>(elements_[index]);
}


template <typename TypeHandler>
inline typename TypeHandler::Type*
RepeatedPtrFieldBase::Mutable(int index) {
  GOOGLE_DCHECK_GE(index, 0);
  GOOGLE_DCHECK_LT(index, size());
  return cast<TypeHandler>(elements_[index]);
}

template <typename TypeHandler>
inline typename TypeHandler::Type* RepeatedPtrFieldBase::Add() {
  if (current_size_ < allocated_size_) {
    return cast<TypeHandler>(elements_[current_size_++]);
  }
  if (allocated_size_ == total_size_) Reserve(total_size_ + 1);
  typename TypeHandler::Type* result = TypeHandler::New();
  ++allocated_size_;
  elements_[current_size_++] = result;
  return result;
}

template <typename TypeHandler>
inline void RepeatedPtrFieldBase::RemoveLast() {
  GOOGLE_DCHECK_GT(current_size_, 0);
  TypeHandler::Clear(cast<TypeHandler>(elements_[--current_size_]));
}

template <typename TypeHandler>
void RepeatedPtrFieldBase::Clear() {
  for (int i = 0; i < current_size_; i++) {
    TypeHandler::Clear(cast<TypeHandler>(elements_[i]));
  }
  current_size_ = 0;
}

template <typename TypeHandler>
inline void RepeatedPtrFieldBase::MergeFrom(const RepeatedPtrFieldBase& other) {
  GOOGLE_CHECK_NE(&other, this);
  Reserve(current_size_ + other.current_size_);
  for (int i = 0; i < other.current_size_; i++) {
    TypeHandler::Merge(other.template Get<TypeHandler>(i), Add<TypeHandler>());
  }
}

template <typename TypeHandler>
inline void RepeatedPtrFieldBase::CopyFrom(const RepeatedPtrFieldBase& other) {
  if (&other == this) return;
  RepeatedPtrFieldBase::Clear<TypeHandler>();
  RepeatedPtrFieldBase::MergeFrom<TypeHandler>(other);
}

}  // namespace internal

template <typename Element>
class RepeatedPtrField<Element>::TypeHandler
    : public internal::GenericTypeHandler<Element> {
};

template <typename Element>
inline RepeatedPtrField<Element>::RepeatedPtrField() {}

template <typename Element>
RepeatedPtrField<Element>::~RepeatedPtrField() {
  Destroy<TypeHandler>();
}

template <typename Element>
inline bool RepeatedPtrField<Element>::empty() const {
  return RepeatedPtrFieldBase::empty();
}

template <typename Element>
inline int RepeatedPtrField<Element>::size() const {
  return RepeatedPtrFieldBase::size();
}

template <typename Element>
inline const Element& RepeatedPtrField<Element>::Get(int index) const {
  return RepeatedPtrFieldBase::Get<TypeHandler>(index);
}

template <typename Element>
inline Element* RepeatedPtrField<Element>::Mutable(int index) {
  return RepeatedPtrFieldBase::Mutable<TypeHandler>(index);
}

template <typename Element>
inline Element* RepeatedPtrField<Element>::Add() {
  return RepeatedPtrFieldBase::Add<TypeHandler>();
}

template <typename Element>
inline void RepeatedPtrField<Element>::RemoveLast() {
  RepeatedPtrFieldBase::RemoveLast<TypeHandler>();
}

template <typename Element>
inline void RepeatedPtrField<Element>::Clear() {
  RepeatedPtrFieldBase::Clear<TypeHandler>();
}

template <typename Element>
inline void RepeatedPtrField<Element>::MergeFrom(
    const RepeatedPtrField& other) {
  RepeatedPtrFieldBase::MergeFrom<TypeHandler>(other);
}

template <typename Element>
inline void RepeatedPtrField<Element>::CopyFrom(
    const RepeatedPtrField& other) {
  RepeatedPtrFieldBase::CopyFrom<TypeHandler>(other);
}

template <typename Element>
void RepeatedPtrField<Element>::Swap(RepeatedPtrField* other) {
  RepeatedPtrFieldBase::Swap(other);
}
template <typename Element> inline RepeatedPtrField<Element>& RepeatedPtrField<Element>::operator=(const RepeatedPtrField& other) { if (this != &other) CopyFrom(other); return *this; }  // original lines 1090-1094: compare at 1092:7, CopyFrom at 1093
// Added for generated_message_reflection.cc / extension_set.cc (protobuf 2.6.0 text).
template <typename Element>
inline int RepeatedField<Element>::SpaceUsedExcludingSelf() const {
  return (elements_ != NULL) ? total_size_ * sizeof(elements_[0]) : 0;
}

template <typename Element>
void RepeatedField<Element>::SwapElements(int index1, int index2) {
  blz::swap(elements_[index1], elements_[index2]);
}

namespace internal {

template <typename TypeHandler>
int RepeatedPtrFieldBase::SpaceUsedExcludingSelf() const {
  int allocated_bytes =
      (elements_ != NULL) ? total_size_ * sizeof(elements_[0]) : 0;
  for (int i = 0; i < allocated_size_; ++i) {
    allocated_bytes += TypeHandler::SpaceUsed(*cast<TypeHandler>(elements_[i]));
  }
  return allocated_bytes;
}

template <typename TypeHandler>
inline typename TypeHandler::Type* RepeatedPtrFieldBase::AddFromCleared() {
  if (current_size_ < allocated_size_) {
    return cast<TypeHandler>(elements_[current_size_++]);
  } else {
    return NULL;
  }
}

template <typename TypeHandler>
void RepeatedPtrFieldBase::AddAllocated(
    typename TypeHandler::Type* value) {
  // Make room for the new pointer.
  if (current_size_ == total_size_) {
    // The array is completely full with no cleared objects, so grow it.
    Reserve(total_size_ + 1);
    ++allocated_size_;
  } else if (allocated_size_ == total_size_) {
    // There is no more space in the pointer array because it contains some
    // cleared objects awaiting reuse.  We don't want to grow the array in this
    // case because otherwise a loop calling AddAllocated() followed by Clear()
    // would leak memory.
    TypeHandler::Delete(cast<TypeHandler>(elements_[current_size_]));
  } else if (current_size_ < allocated_size_) {
    // We have some cleared objects.  We don't care about their order, so we
    // can just move the first one to the end to make space.
    elements_[allocated_size_] = elements_[current_size_];
    ++allocated_size_;
  } else {
    // There are no cleared objects.
    ++allocated_size_;
  }

  elements_[current_size_++] = value;
}

template <typename TypeHandler>
inline typename TypeHandler::Type* RepeatedPtrFieldBase::ReleaseLast() {
  GOOGLE_DCHECK_GT(current_size_, 0);
  typename TypeHandler::Type* result =
      cast<TypeHandler>(elements_[--current_size_]);
  --allocated_size_;
  if (current_size_ < allocated_size_) {
    // There are cleared elements on the end; replace the removed element
    // with the last allocated element.
    elements_[current_size_] = elements_[allocated_size_];
  }
  return result;
}

}  // namespace internal

template <typename Element>
inline int RepeatedPtrField<Element>::SpaceUsedExcludingSelf() const {
  return RepeatedPtrFieldBase::SpaceUsedExcludingSelf<TypeHandler>();
}

template <typename Element>
inline void RepeatedPtrField<Element>::AddAllocated(Element* value) {
  RepeatedPtrFieldBase::AddAllocated<TypeHandler>(value);
}

template <typename Element>
inline Element* RepeatedPtrField<Element>::ReleaseLast() {
  return RepeatedPtrFieldBase::ReleaseLast<TypeHandler>();
}

// Original lines 288-294 (CloseGap), 1131-1136 (DeleteSubrange), 1139-1162 (ExtractSubrange).
namespace internal {
inline void RepeatedPtrFieldBase::CloseGap(int start, int num) {
  // Close up a gap of "num" elements starting at offset "start".
  for (int i = start + num; i < allocated_size_; ++i)
    elements_[i - num] = elements_[i];
  current_size_ -= num;
  allocated_size_ -= num;
}
}  // namespace internal

template <typename Element>
inline void RepeatedPtrField<Element>::DeleteSubrange(int start, int num) {
  for (int i = 0; i < num; ++i)
    delete RepeatedPtrFieldBase::Mutable<TypeHandler>(start + i);
  ExtractSubrange(start, num, NULL);
}

template <typename Element>
inline void RepeatedPtrField<Element>::ExtractSubrange(
    int start, int num, Element** elements) {
  if (num > 0) {
    // Save the values of the removed elements if requested.
    if (elements != NULL) {
      for (int i = 0; i < num; ++i)
        elements[i] = RepeatedPtrFieldBase::Mutable<TypeHandler>(i + start);
    }
    CloseGap(start, num);
  }
}

// Original lines 698-716 (protobuf 2.6.0 RepeatedField<Element>::ExtractSubrange; DWARF: size at 712,
// Get/Set at 713, Truncate at 714 inlined into LobbyService.cpp:4219). Kept here so no existing line moves.
template <typename Element>
inline void RepeatedField<Element>::ExtractSubrange(int start, int num, Element* elements) {
    GOOGLE_DCHECK_GE(start, 0);
    GOOGLE_DCHECK_GE(num, 0);
    GOOGLE_DCHECK_LE(start + num, this->size());

    if (elements != NULL) {
        for (int i = 0; i < num; ++i)
            elements[i] = this->Get(i + start);
    }

    if (num > 0) {
        for (int i = start + num; i < this->size(); ++i)
            this->Set(i - num, this->Get(i));
        this->Truncate(this->size() - num);
    }
}

}  // namespace protobuf
}  // namespace google

// Layout checks generated from the DWARF by tools/layout_asserts.py.
// Not checked: RepeatedField (template); GenericTypeHandler (template); RepeatedPtrField (template); ElementCopier (template)
static_assert(sizeof(google::protobuf::internal::RepeatedPtrFieldBase) == 0x18, "google::protobuf::internal::RepeatedPtrFieldBase");
static_assert(sizeof(google::protobuf::internal::StringTypeHandlerBase) == 0x1, "google::protobuf::internal::StringTypeHandlerBase");
static_assert(sizeof(google::protobuf::internal::StringTypeHandler) == 0x1, "google::protobuf::internal::StringTypeHandler");
