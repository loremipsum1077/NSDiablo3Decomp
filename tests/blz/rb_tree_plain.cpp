// Template checks for blz::rb_tree with the key/value types of the binary that
// need only a layout stand-in (the types are declared here, with the sizes the
// DWARF gives them).
#include "Contrib/Contrib/blz/include/blz/rb_tree.h"
#include "Contrib/Contrib/blz/include/blz/set.h"
#include "Contrib/Contrib/blz/include/blz/map.h"

typedef unsigned long uint64;
typedef unsigned int uint32;
enum LeaderboardScope { LeaderboardScope_Dummy };
struct LeaderboardKeyAndPartition {  // size 0x10
    uint64 m_id;
    LeaderboardScope m_scope;
    uint32 m_partition;
};

namespace Console { namespace DLCManager {
struct AOCMountInfo {  // size 0x10
    unsigned long nCacheSize;
    void* pMountCacheBuffer;
};
} }

namespace D3 { namespace Leaderboard {
class Score {  // size 0x50, has a virtual destructor
public:
    virtual ~Score();
    char pad[0x48];
};
} }

namespace google { namespace protobuf {
class Descriptor;
class FieldDescriptor;
class FileDescriptor;
namespace internal {
class ExtensionSet {
public:
    struct Extension {  // size 0x18
        void* a;
        int b;
        int c;
        void* d;
    };
};
}
} }

// scalar keys
template class blz::rb_tree<blz::set_traits<int>, blz::less<int>, blz::allocator<int> >;
template class blz::rb_tree<blz::set_traits<unsigned int>, blz::less<unsigned int>, blz::allocator<unsigned int> >;
template class blz::rb_tree<blz::set_traits<unsigned long>, blz::less<unsigned long>, blz::allocator<unsigned long> >;
template class blz::rb_tree<blz::rb_map_traits<int, int>, blz::less<int>, blz::allocator<blz::pair<const int, int> > >;
template class blz::rb_tree<blz::rb_map_traits<unsigned int, unsigned long>, blz::less<unsigned int>, blz::allocator<blz::pair<const unsigned int, unsigned long> > >;
template class blz::rb_tree<blz::rb_map_traits<unsigned int, LeaderboardKeyAndPartition>, blz::less<unsigned int>, blz::allocator<blz::pair<const unsigned int, LeaderboardKeyAndPartition> > >;
template class blz::rb_tree<blz::rb_map_traits<long, Console::DLCManager::AOCMountInfo>, blz::less<long>, blz::allocator<blz::pair<const long, Console::DLCManager::AOCMountInfo> > >;
template class blz::rb_tree<blz::rb_map_traits<int, D3::Leaderboard::Score>, blz::less<int>, blz::allocator<blz::pair<const int, D3::Leaderboard::Score> > >;

// pointer keys (protobuf)
template class blz::rb_tree<blz::set_traits<const google::protobuf::FileDescriptor*>, blz::less<const google::protobuf::FileDescriptor*>, blz::allocator<const google::protobuf::FileDescriptor*> >;
template class blz::rb_tree<blz::set_traits<const google::protobuf::Descriptor*>, blz::less<const google::protobuf::Descriptor*>, blz::allocator<const google::protobuf::Descriptor*> >;
template class blz::rb_tree<blz::rb_map_traits<blz::pair<const google::protobuf::Descriptor*, int>, const google::protobuf::FieldDescriptor*>, blz::less<blz::pair<const google::protobuf::Descriptor*, int> >, blz::allocator<blz::pair<const blz::pair<const google::protobuf::Descriptor*, int>, const google::protobuf::FieldDescriptor*> > >;
template class blz::rb_tree<blz::rb_map_traits<int, google::protobuf::internal::ExtensionSet::Extension>, blz::less<int>, blz::allocator<blz::pair<const int, google::protobuf::internal::ExtensionSet::Extension> > >;
