// Template checks for blz::vector with DemonWare element types.
#include "Contrib/Contrib/blz/include/blz/vector.h"

#include <atomic>
#include <stddef.h>

typedef int bdInt;
typedef unsigned int bdUInt;
typedef unsigned int bdUInt32;
typedef long bdInt64;
typedef unsigned long bdUInt64;
typedef bool bdBool;
typedef char bdNChar8;
class bdByteBuffer;
class bdByteBufferRef;

class bdReferencable {  // size 0x10
public:
    bdReferencable() : m_refCount(0) {}
    bdReferencable(const bdReferencable&);
    virtual ~bdReferencable();
    const bdReferencable& operator=(const bdReferencable&);
    void addRef() { ++m_refCount; }
    int releaseRef() { return --m_refCount; }
    std::atomic<int> m_refCount;
};

template <typename T>
class bdReference {  // size 0x8
public:
    static void* operator new(size_t, void* p) { return p; }
    static void operator delete(void*, void*) {}
    bdReference(const bdReference& other) : m_ptr(other.m_ptr) {
        if (m_ptr)
            m_ptr->addRef();
    }
    ~bdReference() {
        if (m_ptr && m_ptr->releaseRef() == 0) {
            delete m_ptr;
            m_ptr = 0;
        }
    }
    T* m_ptr;
};

namespace Console { namespace Online {
class Mail : public bdReferencable {};
class Peer : public bdReferencable {};
} }  // namespace Console::Online

class bdTaskResult : public virtual bdReferencable {  // size 0x18
public:
    bdTaskResult();
    virtual ~bdTaskResult();
    virtual bdBool deserialize(bdByteBufferRef) = 0;
    virtual bdUInt sizeOf() = 0;
protected:
    bdTaskResult(const bdTaskResult&);
};

class bdSerializable : public virtual bdReferencable {  // size 0x18
public:
    bdSerializable();
    virtual ~bdSerializable();
    virtual bdBool serialize(bdByteBuffer&) const = 0;
    virtual bdInt serializedSizeOf() const;
protected:
    bdSerializable(const bdSerializable&);
};

class bdCounterValue : public bdTaskResult, public bdSerializable {  // size 0x30
public:
    bdCounterValue();
    virtual bdBool serialize(bdByteBuffer& buffer) const;
    virtual bdBool deserialize(bdByteBufferRef buffer);
    virtual bdUInt sizeOf();
    bdUInt32 m_counterID;
    bdInt64 m_counterValue;
};

typedef bdUInt64 bdLeaderBoardID;
typedef bdUInt64 bdEntityID;
namespace bdStats { enum bdWriteType { BD_WRITE_0 }; }

class bdStatsInfo : public bdTaskResult, public bdSerializable {  // size 0x90
public:
    bdStatsInfo();
    virtual ~bdStatsInfo();
    virtual bdBool serialize(bdByteBuffer& buffer) const;
    virtual bdBool deserialize(bdByteBufferRef buffer);
    virtual bdUInt sizeOf();
    bdLeaderBoardID m_leaderboardID;
    bdEntityID m_entityID;
    bdStats::bdWriteType m_writeType;
    bdInt64 m_rating;
    bdUInt64 m_rank;
    bdNChar8 m_entityName[65];
    bdUInt32 m_secondsSinceUpdate;
};

class bdStatsInfoEx : public bdTaskResult {  // size 0x30
public:
    bdStatsInfoEx();
    virtual ~bdStatsInfoEx();
    virtual bdBool deserialize(bdByteBufferRef buffer);
    virtual bdUInt sizeOf();
    unsigned int m_leaderboardID;
    bdUInt32 m_error;
    bdEntityID m_entityID;
    bdStatsInfo* m_statsInfo;
};

namespace google { namespace protobuf { class Message { public: virtual ~Message(); }; } }
namespace D3 { namespace Leaderboard {
class Metadata : public google::protobuf::Message {  // size 0x60
public:
    Metadata();
    Metadata(const Metadata&);
    virtual ~Metadata();
    Metadata& operator=(const Metadata& from) {
        CopyFrom(from);
        return *this;
    }
    void CopyFrom(const Metadata& from);
    char _fields_[0x58];
};
} }  // namespace D3::Leaderboard

enum LeaderboardScope { LEADERBOARD_SCOPE_0 };
struct LeaderboardKeyAndPartition {  // size 0x10
    LeaderboardKeyAndPartition() : m_id(0), m_scope(LEADERBOARD_SCOPE_0), m_partition(0) {}
    unsigned long m_id;
    LeaderboardScope m_scope;
    unsigned int m_partition;
};

namespace Console { namespace Online {
class LeaderboardStatsInfo : public bdStatsInfo {  // size 0x110
public:
    LeaderboardStatsInfo() : m_score(0), m_timestamp(0) {}
    virtual bdBool serialize(bdByteBuffer& buffer) const;
    virtual bdBool deserialize(bdByteBufferRef buffer);
    virtual bdUInt sizeOf();
    bdUInt64 m_score;
    bdInt64 m_timestamp;
    D3::Leaderboard::Metadata m_metadata;
    LeaderboardKeyAndPartition m_lbKey;
};
} }  // namespace Console::Online

template void blz::vector<bdCounterValue>::_assign_rv(blz::vector<bdCounterValue>&&);
template void blz::vector<bdStatsInfoEx>::resize(size_t);
template void blz::vector<bdReference<Console::Online::Mail> >::push_back(bdReference<Console::Online::Mail>&&);
template void blz::vector<bdReference<Console::Online::Peer> >::push_back(const bdReference<Console::Online::Peer>&);

// LeaderboardStatsInfo (X/Client/Console/LobbyService.cpp): the original TU
// has all of these out of line, so push_back() and _insert call _transfer and
// insert_raw instead of inlining them.
// The key function makes this TU emit LeaderboardStatsInfo's vtables and VTT,
// as LobbyService.cpp does; the inlined copy constructor then stores the
// construction vtables directly instead of loading them from the VTT.
bdBool Console::Online::LeaderboardStatsInfo::serialize(bdByteBuffer&) const { return false; }

typedef Console::Online::LeaderboardStatsInfo LSI;
template LSI& blz::vector<LSI>::push_back();
template void blz::vector<LSI>::_assign_rv(blz::vector<LSI>&&);
template void blz::vector<LSI>::resize(size_t);
template LSI* blz::vector<LSI>::_insert<const LSI*>(const LSI*, const LSI*, const LSI*, blz::forward_iterator_tag);
template blz::vector<LSI>::insert_raw_ranges blz::vector<LSI>::insert_raw(const LSI*, size_t);
template void blz::vector<LSI>::_transfer(LSI*);
