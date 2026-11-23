// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/BlzExt/BlzVectorHelper.h
// Subset: vector_get_at_grow, vector_append_element, vector_erase_at and vector_find. The bodies are not in the DWARF; they
// are written from the copy inlined into X/Client/CGame.cpp (AsyncRender::RegisterDelayedCallback).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../Contrib/Contrib/blz/include/blz/iter_op.h"
#include "../../../Contrib/Contrib/blz/include/blz/iter_insert.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"

namespace blzext {
// line 20. Body written from the out-of-line copy in X/Client/Font/FormattedText.cpp: the last
// element moves into the hole, then the vector shrinks by one.
template <typename T>
void vector_erase_reorder(blz::vector<T, blz::allocator<T> > & tVector, const size_t nIndex) {
    if (nIndex < tVector.size() - 1) {
        tVector[nIndex] = tVector[tVector.size() - 1];
    }
    tVector.resize(tVector.size() - 1);
}

template <typename T>
inline T & vector_get_at_grow(blz::vector<T, blz::allocator<T> > & tVectorA, uint32 nIndex) {
    if (tVectorA.size() <= nIndex) {
        tVectorA.resize(nIndex + 1);
    }
    return tVectorA[nIndex];
}

// line 67. Body written from the copies inlined into X/Server/AI/ActorAI.cpp (sAllocQueuedAction,
// sAllocQueuedCallout): lines 72, 74 and 76.
template <typename T>
inline void vector_set_at_grow(blz::vector<T, blz::allocator<T> > & tVectorA, uint32 nIndex, const T & tValue) {
    if (tVectorA.size() <= nIndex) {
        tVectorA.resize(nIndex + 1);
    }
    tVectorA[nIndex] = tValue;
}

template <typename T>
inline T & vector_append_element(blz::vector<T, blz::allocator<T> > & tVector) {
    return vector_get_at_grow(tVector, tVector.size());
}

// line 43. Body written from the copy inlined into UIConsoleListBox::RemoveMultipleItems.
template <typename T>
inline void vector_erase_at(blz::vector<T, blz::allocator<T> > & tVector, uint32 nIndex) {
    tVector.erase(tVector.begin() + nIndex);
}

// line 81. Body written from the out-of-line copy in X/Client/UserInterface/Console/UIConsoleAchievements.cpp.
template <typename T>
void vector_append(blz::vector<T, blz::allocator<T> > & tVectorA, const blz::vector<T, blz::allocator<T> > & tVectorB) {
    tVectorA.reserve(tVectorA.size() + tVectorB.size());
    blz::copy(tVectorB.begin(), tVectorB.end(), blz::back_inserter(tVectorA));
}

// line 99. Body written from the copy inlined into FormattedText::InsertDelta.
template <typename T>
inline void vector_insert_at(blz::vector<T, blz::allocator<T> > & tVector, const size_t nIndex, const T & tValue) {
    tVector.insert(tVector.begin() + nIndex, tValue);
}

// line 109. Body written from the copies inlined into UIControlGroup::RemoveChild and AddActiveChild.
template <typename T>
inline int32 vector_find(const blz::vector<T, blz::allocator<T> > & tVector, const T & tValue) {
    typename blz::vector<T, blz::allocator<T> >::const_iterator iter = blz::find(tVector.begin(), tVector.end(), tValue);
    if (iter != tVector.end()) {
        return (int32)blz::distance(tVector.begin(), iter);
    }
    return -1;
}
// line 141. Body written from the copies inlined into UISkills::Console::GetTraitList and
// GetSkillList: the comparator is a qsort-style callback, wrapped in a lambda (line 147).
// Not declared `inline`: with the hint blz::insertion_sort is inlined here instead of one level
// later, which changes its guard in SNOFilesApplyAllHotfixInFolder (X/Common/SNO/SNOFiles.cpp).
template <typename T>
void vector_sort(blz::vector<T, blz::allocator<T> > & tVector, int (*pfnCompare)(const void *, const void *)) {
    blz::sort(tVector.begin(), tVector.end(),
              [pfnCompare](const T & tLHS, const T & tRHS) { return pfnCompare(&tLHS, &tRHS) < 0; });
}
}  // namespace blzext
