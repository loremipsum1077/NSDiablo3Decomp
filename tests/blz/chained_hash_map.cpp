// Template checks for blz/chained_hash_map.h (protobuf's hash_map) and
// blz/chained_hash_table.h. The protobuf hashers and the key/value types are
// test stand-ins copied from the DWARF (stubs/hash.h, descriptor.cc).
#include "Contrib/Contrib/blz/include/blz/chained_hash_map.h"

#include <string.h>

namespace blz {
template <typename C> struct char_traits;
template <typename C, typename T, typename A> class basic_string;
}  // namespace blz

namespace google {
namespace protobuf {
class Descriptor;
class EnumDescriptor;
class FieldDescriptor;
class EnumValueDescriptor;
class FileDescriptor;
class Message;
class DynamicMessage {
public:
    struct TypeInfo;
};

// stubs/hash.h line 150
template <typename Key>
struct hash;

template <typename Key>
struct hash<Key*> {
    size_t operator()(const Key* key) const { return reinterpret_cast<size_t>(key); }
};

// stubs/hash.h line 159
template <>
struct hash<const char*> {
    size_t operator()(const char* str) const {
        size_t result = 0;
        for (; *str != '\0'; str++)
            result = 5 * result + static_cast<unsigned char>(*str);
        return result;
    }
};

// stubs/hash.h line 224
struct streq {
    bool operator()(const char* a, const char* b) const { return strcmp(a, b) == 0; }
};

namespace {
typedef blz::pair<const void*, const char*> PointerStringPair;

struct PointerStringPairEqual {
    bool operator()(const PointerStringPair& a, const PointerStringPair& b) const {
        return a.first == b.first && strcmp(a.second, b.second) == 0;
    }
};

template <typename PairType>
struct PointerIntegerPairHash {
    size_t operator()(const PairType& p) const {
        return reinterpret_cast<intptr_t>(p.first) * ((1 << 16) - 1) + p.second;
    }
};

typedef blz::pair<const Descriptor*, int> DescriptorIntPair;
typedef blz::pair<const EnumDescriptor*, int> EnumIntPair;

struct PointerStringPairHash {
    size_t operator()(const PointerStringPair& p) const {
        hash<const char*> cstring_hash;
        return reinterpret_cast<intptr_t>(p.first) * ((1 << 16) - 1) + cstring_hash(p.second);
    }
};

struct Symbol {  // size 0x10
    int type;
    const void* descriptor;
};
}  // namespace
}  // namespace protobuf
}  // namespace google

using namespace google::protobuf;

typedef blz::chained_hash_map<const char*, Symbol, hash<const char*>, streq> SymbolsByNameMap;
typedef blz::chained_hash_map<PointerStringPair, Symbol, PointerStringPairHash, PointerStringPairEqual>
    SymbolsByParentMap;
typedef blz::chained_hash_map<const char*, const FileDescriptor*, hash<const char*>, streq> FilesByNameMap;
typedef blz::chained_hash_map<PointerStringPair, const FieldDescriptor*, PointerStringPairHash,
                              PointerStringPairEqual>
    FieldsByNameMap;
typedef blz::chained_hash_map<DescriptorIntPair, const FieldDescriptor*, PointerIntegerPairHash<DescriptorIntPair> >
    FieldsByNumberMap;
typedef blz::chained_hash_map<EnumIntPair, const EnumValueDescriptor*, PointerIntegerPairHash<EnumIntPair> >
    EnumValuesByNumberMap;
typedef blz::chained_hash_map<const Descriptor*, const DynamicMessage::TypeInfo*, hash<const Descriptor*> >
    PrototypeMap;
typedef void (*RegistrationFunc)(const blz::basic_string<char, blz::char_traits<char>, blz::allocator<char> >&);
typedef blz::chained_hash_map<const char*, RegistrationFunc, hash<const char*>, streq> FileMap;
typedef blz::chained_hash_map<const Descriptor*, const Message*, hash<const Descriptor*> > TypeMap;

template void FilesByNameMap::chained_hash_table::rehash(size_t);

// Instantiations over types in protobuf's anonymous namespace have internal
// linkage; keep their addresses so that they are emitted.
typedef void (SymbolsByNameMap::*SymbolsByNameRehash)(size_t);
typedef void (SymbolsByParentMap::*SymbolsByParentRehash)(size_t);
typedef void (FieldsByNameMap::*FieldsByNameRehash)(size_t);
typedef void (FieldsByNumberMap::*FieldsByNumberRehash)(size_t);
typedef void (EnumValuesByNumberMap::*EnumValuesByNumberRehash)(size_t);
__attribute__((used)) static SymbolsByNameRehash g_keep1 = &SymbolsByNameMap::rehash;
__attribute__((used)) static SymbolsByParentRehash g_keep2 = &SymbolsByParentMap::rehash;
__attribute__((used)) static FieldsByNameRehash g_keep3 = &FieldsByNameMap::rehash;
__attribute__((used)) static FieldsByNumberRehash g_keep4 = &FieldsByNumberMap::rehash;
__attribute__((used)) static EnumValuesByNumberRehash g_keep5 = &EnumValuesByNumberMap::rehash;
template void PrototypeMap::chained_hash_table::rehash(size_t);
template void FileMap::chained_hash_table::rehash(size_t);
template void TypeMap::chained_hash_table::rehash(size_t);
template blz::pair<TypeMap::iterator, bool> TypeMap::chained_hash_table::insert(const TypeMap::value_type&);
