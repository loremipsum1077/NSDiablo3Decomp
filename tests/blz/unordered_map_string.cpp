// Template checks for blz/chained_hash_table.h with blz::basic_string keys or
// values (unordered_map, protobuf's hash_set). Key and value layouts are
// copied from the DWARF.
//
// Known differences, not in chained_hash_table.h: the string-keyed lookups
// (ResolverStatsMap insert/_insert_raw, StringSet insert/_insert_raw) are 20
// bytes short because the binary's char_traits<char>::compare (and move/copy
// in the string move constructor) skip memcmp/memmove/memcpy when n == 0
// (`cbz x2` before the call, char_traits.h:186); the basic_string move
// constructor differs for the same reason.
#include "Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "Contrib/Contrib/blz/include/blz/chained_hash_set.h"
#include "Contrib/Contrib/blz/include/blz/string.h"
#include "Contrib/Contrib/blz/include/blz/intrusive_ptr.h"
#include "Contrib/Contrib/blz/include/blz/ref_count_base.h"

typedef unsigned int uint32;
typedef unsigned long uint64;

// ---- string -> intrusive_ptr<ResolverStats> (X/Server/OnlineService/BaseResolver.h) ----

namespace OnlineService {
class ResolverStats : public blz::atomic_ref_count_base<ResolverStats> {  // size 0x538
public:
    char m_pad[0x538 - 0x10];
};
}  // namespace OnlineService

typedef blz::unordered_map<blz::string, blz::intrusive_ptr<OnlineService::ResolverStats> > ResolverStatsMap;

template ResolverStatsMap::chained_hash_table::~chained_hash_table();
template void ResolverStatsMap::chained_hash_table::rehash(size_t);
template blz::pair<ResolverStatsMap::iterator, bool> ResolverStatsMap::chained_hash_table::insert(ResolverStatsMap::value_type&&);

// _insert_raw is protected; reach it through a derived class.
struct ResolverStatsMapAccess : ResolverStatsMap {
    typedef blz::pair<iterator, bool> (ResolverStatsMap::chained_hash_table::*InsertRaw)(const blz::string&, node_type*);
    static InsertRaw insert_raw() { return &ResolverStatsMapAccess::_insert_raw; }
};
template blz::pair<ResolverStatsMap::iterator, bool> ResolverStatsMap::chained_hash_table::_insert_raw(
    const blz::string&, blz::chained_hash_node<ResolverStatsMap::value_type>*);

// ---- unsigned long -> ActorInventoryMailRecord (X/Common/ItemMailCommon.h) ----

namespace D3 {
namespace Items {
class MailAttachments {  // protobuf message; destructor out of line
public:
    MailAttachments();
    MailAttachments(const MailAttachments&);
    virtual ~MailAttachments();
    char m_pad[0x18];
};
}  // namespace Items
}  // namespace D3

struct ActorInventoryMailRecord {  // size 0xa0
    uint32 tAccountTo;
    uint32 tAccountFrom;
    uint64 tMailId;
    blz::string szTitle;
    blz::string szBody;
    int eStatus;
    uint32 sendTime;
    long tExpireTime;
    uint32 uPartition;
    uint32 uSeasonCreated;
    uint32 uFlags;
    bool bDirty;
    D3::Items::MailAttachments attachments;
};

typedef blz::unordered_map<unsigned long, ActorInventoryMailRecord> MailRecordMap;

template ActorInventoryMailRecord& MailRecordMap::operator[](const unsigned long&);
template void MailRecordMap::chained_hash_table::rehash(size_t);
template blz::pair<MailRecordMap::iterator, bool> MailRecordMap::chained_hash_table::insert(
    blz::pair<unsigned long, ActorInventoryMailRecord>&&);
template MailRecordMap::iterator MailRecordMap::chained_hash_table::erase(MailRecordMap::const_iterator);
template size_t MailRecordMap::chained_hash_table::erase(const unsigned long&);
template void MailRecordMap::chained_hash_table::_deallocate();

// ---- protobuf's hash_set<string> (stubs/hash.h) ----

namespace google {
namespace protobuf {
template <typename Key>
struct hash;

// line 159
template <>
struct hash<const char*> {
    size_t operator()(const char* str) const {
        size_t result = 0;
        for (; *str != '\0'; str++)
            result = 5 * result + static_cast<unsigned char>(*str);
        return result;
    }
};

// line 190
template <>
struct hash<blz::string> {
    size_t operator()(const blz::string& key) const { return hash<const char*>()(key.c_str()); }
};
}  // namespace protobuf
}  // namespace google

typedef blz::chained_hash_set<blz::string, google::protobuf::hash<blz::string> > StringSet;

template blz::pair<StringSet::iterator, bool> StringSet::chained_hash_table::insert(const blz::string&);
template blz::pair<StringSet::iterator, bool> StringSet::chained_hash_table::_insert_raw(
    const blz::string&, blz::chained_hash_node<blz::string>*);
template void StringSet::chained_hash_table::rehash(size_t);
