// Template checks for algo_sort.h / algo_heap.h / algo_merge.h.
#include "Contrib/Contrib/blz/include/blz/algo_sort.h"

struct RigidBody;

typedef bool (*CmpU32)(const unsigned int&, const unsigned int&);
typedef bool (*CmpU64)(const unsigned long&, const unsigned long&);
typedef bool (*CmpInt)(const int&, const int&);

template void blz::partial_sort<unsigned int*, CmpU32>(unsigned int*, unsigned int*, unsigned int*, CmpU32);
template void blz::internal::introsort_loop<unsigned int*, long, CmpU32>(unsigned int*, unsigned int*, long, CmpU32);
template void blz::partial_sort<unsigned long*, CmpU64>(unsigned long*, unsigned long*, unsigned long*, CmpU64);
template void blz::internal::introsort_loop<unsigned long*, long, CmpU64>(unsigned long*, unsigned long*, long, CmpU64);
template void blz::partial_sort<int*, CmpInt>(int*, int*, int*, CmpInt);
template void blz::internal::introsort_loop<int*, long, CmpInt>(int*, int*, long, CmpInt);
template void blz::partial_sort<RigidBody**, blz::less<void> >(RigidBody**, RigidBody**, RigidBody**, blz::less<void>);
template void blz::internal::introsort_loop<RigidBody**, long, blz::less<void> >(RigidBody**, RigidBody**, long, blz::less<void>);
template void blz::partial_sort<int*, blz::less<void> >(int*, int*, int*, blz::less<void>);
template void blz::internal::introsort_loop<int*, long, blz::less<void> >(int*, int*, long, blz::less<void>);

// callers exercising the inlined ones
void sort_rigid(RigidBody** a, RigidBody** b) { blz::sort(a, b); }
void sort_ints(int* a, int* b) { blz::sort(a, b); }

// blzext::vector_sort (X/Sigma/BlzExt/BlzVectorHelper.h:141); the comparison
// lambda at line 147 is what the introsort_loop / partial_sort / adjust_heap
// instantiations below are named after.
#include "Contrib/Contrib/blz/include/blz/vector.h"

typedef int (*PFN_COMPARE)(const void*, const void*);
namespace blzext {
template <typename T>
void vector_sort(blz::vector<T, blz::allocator<T> >& tVector, PFN_COMPARE pfnCompare) {
    blz::sort(tVector.begin(), tVector.end(),
              [pfnCompare](const T& left, const T& right) { return pfnCompare(&left, &right) < 0; });
}
}  // namespace blzext

struct UIControlData;
enum ACDID { ACDID_INVALID = -1 };

template void blzext::vector_sort<int>(blz::vector<int, blz::allocator<int> >&, PFN_COMPARE);
template void blzext::vector_sort<UIControlData*>(blz::vector<UIControlData*, blz::allocator<UIControlData*> >&, PFN_COMPARE);
template void blzext::vector_sort<ACDID>(blz::vector<ACDID, blz::allocator<ACDID> >&, PFN_COMPARE);

typedef bool (*CmpIntByValue)(int, int);
template void blz::partial_sort<int*, CmpIntByValue>(int*, int*, int*, CmpIntByValue);
template void blz::internal::introsort_loop<int*, long, CmpIntByValue>(int*, int*, long, CmpIntByValue);

// D3::Leaderboard::Score: protobuf message, 0x50 bytes, out-of-line copy
// constructor / destructor, operator= forwarding to CopyFrom.
namespace D3 { namespace Leaderboard {
class Score {
public:
    Score(const Score&);
    ~Score();
    Score& operator=(const Score& from) {
        CopyFrom(from);
        return *this;
    }
    void CopyFrom(const Score&);

private:
    void* m_data[10];
};
} }

typedef bool (*CmpScore)(const D3::Leaderboard::Score&, const D3::Leaderboard::Score&);
template void blz::sort<D3::Leaderboard::Score*, CmpScore>(D3::Leaderboard::Score*, D3::Leaderboard::Score*, CmpScore);
template void blz::internal::introsort_loop<D3::Leaderboard::Score*, long, CmpScore>(D3::Leaderboard::Score*, D3::Leaderboard::Score*, long, CmpScore);
template void blz::partial_sort<D3::Leaderboard::Score*, CmpScore>(D3::Leaderboard::Score*, D3::Leaderboard::Score*, D3::Leaderboard::Score*, CmpScore);
template void blz::internal::adjust_heap<D3::Leaderboard::Score*, long, D3::Leaderboard::Score, CmpScore>(D3::Leaderboard::Score*, long, long, D3::Leaderboard::Score&&, CmpScore);

// UICharacterDetails::Console::DisplayBuffs (UIConsoleCharacterDetails.cpp:1266):
// a stable_sort of buff pointers with a name comparison; the lambda is what
// merge_sort_buffer / merge are named after. Stand-in types, only the shape
// the comparison needs.
#include <strings.h>

struct Player;
namespace UIBuffs { struct SortedBuff { int m_snoPower; int m_nId; }; }
struct CRefString {
    void* m_p[3];
    ~CRefString();
    operator const char*() const;
};
CRefString GetBuffName(int, int);

namespace UICharacterDetails { namespace Console {
void DisplayBuffs(const Player*) {
    blz::vector<const UIBuffs::SortedBuff*, blz::allocator<const UIBuffs::SortedBuff*> > vecBeneficialBuffs;
    blz::stable_sort(vecBeneficialBuffs.begin(), vecBeneficialBuffs.end(),
                     [](const UIBuffs::SortedBuff*& a, const UIBuffs::SortedBuff*& b) -> int {
                         if (a->m_snoPower == b->m_snoPower)
                             return a->m_nId - b->m_nId;
                         CRefString sBuff1 = GetBuffName(a->m_snoPower, a->m_nId);
                         CRefString sBuff2 = GetBuffName(b->m_snoPower, b->m_nId);
                         return strcasecmp(sBuff1, sBuff2);
                     });
}
} }

// Element types of the other vector_sort / sort instantiations: plain data,
// layouts from the DWARF (sizes 0x10, 0xc, 0x38, 0x98).
namespace UITransmogrify {
struct DisplayedTransmog { unsigned int gbidTransmog; int snoActor; int bUnlocked; int bCanUnlock; };
}
struct DamageShieldPriorityEntry { int idBuff; int nPriority; int nTicksLeft; };
namespace UISkills { namespace Console {
struct RuneEntry { int a; int b; };
struct SkillListEntry { RuneEntry tRunes[5]; int nIndex; int nLevel; int snoPower; int eCategory; };
} }
namespace Console { namespace Online {
struct Friend { alignas(8) char m_data[0x98]; };
} }

template void blzext::vector_sort<UITransmogrify::DisplayedTransmog>(blz::vector<UITransmogrify::DisplayedTransmog, blz::allocator<UITransmogrify::DisplayedTransmog> >&, PFN_COMPARE);
template void blzext::vector_sort<DamageShieldPriorityEntry>(blz::vector<DamageShieldPriorityEntry, blz::allocator<DamageShieldPriorityEntry> >&, PFN_COMPARE);
template void blzext::vector_sort<UISkills::Console::SkillListEntry>(blz::vector<UISkills::Console::SkillListEntry, blz::allocator<UISkills::Console::SkillListEntry> >&, PFN_COMPARE);

typedef bool (*CmpFriend)(const Console::Online::Friend&, const Console::Online::Friend&);
template void blz::internal::introsort_loop<Console::Online::Friend*, long, CmpFriend>(Console::Online::Friend*, Console::Online::Friend*, long, CmpFriend);
template void blz::partial_sort<Console::Online::Friend*, CmpFriend>(Console::Online::Friend*, Console::Online::Friend*, Console::Online::Friend*, CmpFriend);
