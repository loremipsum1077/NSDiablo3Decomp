// blz::set / blz::map inline helpers (insert, in-place node construction)
// checked through protobuf's descriptor.cc, with stand-in classes that have
// the field offsets of the original.
#include "Contrib/Contrib/blz/include/blz/map.h"
#include "Contrib/Contrib/blz/include/blz/set.h"
#include "Contrib/Contrib/blz/include/blz/vector.h"

namespace google { namespace protobuf {

class Descriptor;
class FieldDescriptor;

class FileDescriptor {
public:
    int public_dependency_count() const { return public_dependency_count_; }
    const FileDescriptor* public_dependency(int index) const {
        return dependencies_[public_dependencies_[index]];
    }

    char pad0[0x20];
    const FileDescriptor** dependencies_;
    int public_dependency_count_;
    const int* public_dependencies_;
};

class FieldDescriptor {
public:
    const Descriptor* containing_type() const { return containing_type_; }
    int number() const { return number_; }

    char pad0[0x28];
    int number_;
    char pad1[0x14];
    const Descriptor* containing_type_;
};

class DescriptorBuilder {
public:
    void RecordPublicDependencies(const FileDescriptor* file);

    char pad[0x70];
    blz::set<const FileDescriptor*> dependencies_;
};

void DescriptorBuilder::RecordPublicDependencies(const FileDescriptor* file) {
    if (file == NULL || !dependencies_.insert(file).second)
        return;
    for (int i = 0; file != NULL && i < file->public_dependency_count(); i++) {
        RecordPublicDependencies(file->public_dependency(i));
    }
}

namespace {
typedef blz::pair<const Descriptor*, int> DescriptorIntPair;
}

// from protobuf/stubs/map-util.h
template <class Collection>
bool InsertIfNotPresent(Collection* const collection, const typename Collection::value_type& vt) {
    return collection->insert(vt).second;
}

template <class Collection>
bool InsertIfNotPresent(Collection* const collection,
                        const typename Collection::value_type::first_type& key,
                        const typename Collection::value_type::second_type& value) {
    return InsertIfNotPresent(collection, typename Collection::value_type(key, value));
}

class DescriptorPool {
public:
    class Tables {
    public:
        bool AddExtension(const FieldDescriptor* field);

        char pad[0x118];
        blz::map<DescriptorIntPair, const FieldDescriptor*> extensions_;
        char pad2[0x188 - 0x118 - 0x28];
        blz::vector<DescriptorIntPair> extensions_after_checkpoint_;
    };
};

bool DescriptorPool::Tables::AddExtension(const FieldDescriptor* field) {
    DescriptorIntPair key(field->containing_type(), field->number());
    if (InsertIfNotPresent(&extensions_, key, field)) {
        extensions_after_checkpoint_.push_back(key);
        return true;
    } else {
        return false;
    }
}

} }

// text_format.cc
namespace google { namespace protobuf { namespace TextFormat {

struct ParseLocation {
    int line;
    int column;
};

class ParseInfoTree {
public:
    void RecordLocation(const FieldDescriptor* field, ParseLocation location);

private:
    typedef blz::map<const FieldDescriptor*, blz::vector<ParseLocation> > LocationMap;
    LocationMap locations_;
};

void ParseInfoTree::RecordLocation(const FieldDescriptor* field, ParseLocation location) {
    locations_[field].push_back(location);
}

} } }
