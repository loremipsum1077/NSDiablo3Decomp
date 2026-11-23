// Original file: BlizzardCore/Source/System/Using/System_Using.cpp
// The original is a unity file whose one source is Using.cpp; the functions are in its line order.
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Types.h"
#include "../../../Include/Blizzard/Using.h"

namespace System_Using {
enum {
    USING_MAX = 32,
    DEPENDS_MAX = 128,
    STRINGPOOL_SIZE = 1024,
};

struct Hash {  // size 0x204
    enum {
        HASH_SIZE = 64,
    };
    struct Slot {  // size 0x8
        uint32 m_keyhash;  // 0x00
        short m_data;  // 0x04
    };
    bool Next(uint & slot, short & data);
    bool Find(const char * key, short & data);
    bool Insert(const char * key, const short & data);
    static uint32 Hash32(const char * string);

    uint ComputeSlot(const uint32 keyHash) { return keyHash & (HASH_SIZE - 1); }
    bool FindSlot(const char * key, uint & slot);
    Slot m_slots[HASH_SIZE];  // 0x00
    uint m_used;  // 0x200
};

struct Depends {  // size 0x10
    short id;  // 0x00
    Depends * next;  // 0x08
};

struct UsingRec {  // size 0x28
    char * name;  // 0x00
    short id;  // 0x08
    short refCount;  // 0x0a
    void (*shutdownProc)();  // 0x10
    short dependCount;  // 0x18
    Depends * depends;  // 0x20
};

struct GlobalUsing : public Blizzard::Using::ScopedUse {  // size 0x8
    GlobalUsing();
};

// Using.cpp:84
inline uint32 Hash::Hash32(const char * string)
{
    uint32 hash = 0x1000193;
    for (; *string != '\0'; string++)
    {
        hash = (hash ^ static_cast<uint8>(*string)) * 0x1000193;
    }
    if (hash == 0)
    {
        hash = 1;
    }
    return hash;
}

// Using.cpp:96
inline bool Hash::Next(uint & slot, short & data)
{
    while (slot < HASH_SIZE)
    {
        // Using.cpp:99 cols 17/29/33 and 100 col 42: the slot++ is inside the subscript.
        if (m_slots[slot++].m_keyhash)
        {
            data = m_slots[slot - 1].m_data;
            return true;
        }
    }
    return false;
}

// Using.cpp:107
inline bool Hash::Find(const char * key, short & data)
{
    uint slot;
    if (FindSlot(key, slot))
    {
        data = m_slots[slot].m_data;
        return true;
    }
    return false;
}

// Using.cpp:120
inline bool Hash::Insert(const char * key, const short & data)
{
    uint slot;
    if (FindSlot(key, slot))
    {
        return false;
    }
    m_used++;
    m_slots[slot].m_keyhash = Hash32(key);
    m_slots[slot].m_data = data;
    return true;
}

// Using.cpp:135
inline bool Hash::FindSlot(const char * key, uint & slot)
{
    uint32 keyHash = Hash32(key);
    slot = ComputeSlot(keyHash);
    while (m_slots[slot].m_keyhash != 0)
    {
        if (m_slots[slot].m_keyhash == keyHash)
        {
            return true;
        }
        slot = ComputeSlot(slot + 1);
    }
    return false;
}

// Using.cpp:157-188
Hash s_Using;
char s_UsingNames[STRINGPOOL_SIZE];
char * s_UsingNamesTail;
short s_NextUsingID;
UsingRec use[USING_MAX];
Depends s_depends[DEPENDS_MAX];
int s_dependsCount;

UsingRec * Intern(const char * name);
void ShutdownAny();
}  // namespace System_Using

namespace Blizzard {
namespace Using {
// Using.cpp:276
void Register(const char * usingName, void (*shutdownProc)())
{
    System_Using::Intern(usingName)->shutdownProc = shutdownProc;
}
}  // namespace Using
}  // namespace Blizzard

namespace System_Using {
// Using.cpp:293
void RegisterNull()
{
}
}  // namespace System_Using

namespace Blizzard {
namespace Using {
// Using.cpp:301
void StartUsing(const char * usingName)
{
    System_Using::Intern(usingName)->refCount++;
}

// Using.cpp:324
// NON_MATCHING: 276 vs 260 bytes, 30 differing lines (was 105). Shapes follow the line table: Next
// slot++ in the subscript (99 col 29); slot/id in the outer loop block (DWARF 417/418); continue
// tests at 424/428; for over depends (440). Left: block placement of Next's bounds test (original
// has one cmp #0x3f; b.hi reached by both continues, ours duplicates it); do/while forms tried.
void StopUsing(const char * usingName)
{
    System_Using::UsingRec * pd = System_Using::Intern(usingName);
    pd->refCount -= 1;
    System_Using::ShutdownAny();
}

// Using.cpp:346
void DependsOn(const char * me, const char * dependsOn)
{
    System_Using::UsingRec * uSelf = System_Using::Intern(me);
    System_Using::UsingRec * uDepends = System_Using::Intern(dependsOn);
    System_Using::Depends * depends;
    System_Using::Depends ** ppNext = &uSelf->depends;
    for (depends = *ppNext; depends != NULL; depends = *ppNext)
    {
        if (depends->id == uDepends->id)
        {
            return;
        }
        ppNext = &depends->next;
    }
    uSelf->dependCount++;
    uDepends->refCount++;
    *ppNext = &System_Using::s_depends[System_Using::s_dependsCount++];
    (*ppNext)->id = uDepends->id;
    (*ppNext)->next = NULL;
}
}  // namespace Using
}  // namespace Blizzard

namespace System_Using {
// Using.cpp:398
GlobalUsing::GlobalUsing() : ScopedUse("Blizzard::Using::GlobalUse")
{
    Blizzard::Using::DependsOn("Blizzard::Using::GlobalUse", "Blizzard::File");
    Blizzard::Using::DependsOn("Blizzard::Using::GlobalUse", "Blizzard::Memory");
}

// Using.cpp:404
GlobalUsing s_globalusing;

// Using.cpp:412
inline void ShutdownAny()
{
    bool done = false;
    while (!done)
    {
        uint slot = 0;
        short id;
        done = true;
        while (s_Using.Next(slot, id))
        {
            if (use[id].shutdownProc == NULL)
            {
                continue;
            }
            if (use[id].refCount > 0)
            {
                continue;
            }
            void (*func)() = use[id].shutdownProc;
            use[id].shutdownProc = NULL;
            func();
            for (Depends * depends = use[id].depends; depends != NULL; depends = depends->next)
            {
                use[depends->id].refCount -= 1;
            }
            use[id].dependCount = 0;
            use[id].depends = NULL;
            done = false;
        }
    }
}

// Using.cpp:496
UsingRec * Intern(const char * name)
{
    short ID;
    if (!s_Using.Find(name, ID))
    {
        ID = s_NextUsingID++;
        if (s_UsingNamesTail == NULL)
        {
            s_UsingNamesTail = s_UsingNames;
        }
        UsingRec * info = &use[ID];
        info->name = s_UsingNamesTail;
        info->id = ID;
        info->refCount = 0;
        info->shutdownProc = RegisterNull;
        info->dependCount = 0;
        info->depends = NULL;
        s_UsingNamesTail +=
            Blizzard::String::Copy(s_UsingNamesTail, name, s_UsingNames + STRINGPOOL_SIZE - s_UsingNamesTail) + 1;
        s_Using.Insert(name, ID);
    }
    return &use[ID];
}
}  // namespace System_Using
