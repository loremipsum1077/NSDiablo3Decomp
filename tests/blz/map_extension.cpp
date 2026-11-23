// blz::map / rb_tree inline helpers, checked through protobuf's ExtensionSet
// (extension_set.cc / extension_set_heavy.cc), which keeps a
// blz::map<int, Extension>. The class is a stand-in with the layout of the
// original (size 0x28: the map).
#include "Contrib/Contrib/blz/include/blz/map.h"
#include "Contrib/Contrib/blz/include/blz/set.h"

namespace google { namespace protobuf {
class FieldDescriptor;
namespace io { class CodedOutputStream; }
namespace internal {

class ExtensionSet {
public:
    struct Extension {  // size 0x18; the fields are protobuf's value union, type,
                        // flags and cached_size, folded into words
        unsigned long value;
        unsigned long flags;
        const FieldDescriptor* descriptor;

        Extension() : value(0), flags(0), descriptor(0) {}
        void Clear();
        void Free();
        int GetSize() const;
        int SpaceUsedExcludingSelf() const;
        void SerializeFieldWithCachedSizes(int number, io::CodedOutputStream* output) const;
    };

    ExtensionSet();
    void Clear();
    void ClearExtension(int number);
    int ExtensionSize(int number) const;
    void Swap(ExtensionSet* x);
    bool MaybeNewExtension(int number, const FieldDescriptor* descriptor, Extension** result);
    int SpaceUsedExcludingSelf() const;
    void SerializeWithCachedSizes(int start_field_number, int end_field_number,
                                  io::CodedOutputStream* output) const;

private:
    typedef blz::map<int, Extension> ExtensionMap;
    ExtensionMap extensions_;
};

ExtensionSet::ExtensionSet() {}

void ExtensionSet::ClearExtension(int number) {
    ExtensionMap::iterator iter = extensions_.find(number);
    if (iter == extensions_.end())
        return;
    iter->second.Clear();
}

int ExtensionSet::ExtensionSize(int number) const {
    ExtensionMap::const_iterator iter = extensions_.find(number);
    if (iter == extensions_.end())
        return false;
    return iter->second.GetSize();
}

void ExtensionSet::Clear() {
    for (ExtensionMap::iterator iter = extensions_.begin(); iter != extensions_.end(); ++iter)
        iter->second.Clear();
}

void ExtensionSet::Swap(ExtensionSet* x) {
    extensions_.swap(x->extensions_);
}

bool ExtensionSet::MaybeNewExtension(int number, const FieldDescriptor* descriptor, Extension** result) {
    blz::pair<ExtensionMap::iterator, bool> insert_result =
        extensions_.insert(blz::make_pair(number, Extension()));
    *result = &insert_result.first->second;
    (*result)->descriptor = descriptor;
    return insert_result.second;
}

int ExtensionSet::SpaceUsedExcludingSelf() const {
    int total_size = extensions_.size() * sizeof(ExtensionMap::value_type);
    for (ExtensionMap::const_iterator iter = extensions_.begin(), end = extensions_.end(); iter != end; ++iter)
        total_size += iter->second.SpaceUsedExcludingSelf();
    return total_size;
}

void ExtensionSet::SerializeWithCachedSizes(int start_field_number, int end_field_number,
                                            io::CodedOutputStream* output) const {
    ExtensionMap::const_iterator iter;
    for (iter = extensions_.lower_bound(start_field_number);
         iter != extensions_.end() && iter->first < end_field_number; ++iter) {
        iter->second.SerializeFieldWithCachedSizes(iter->first, output);
    }
}

}  // namespace internal
} }
