// Template checks for blz::vector: element types with out-of-line special
// members (protobuf messages, FormattedText, XVariant).
#include "Contrib/Contrib/blz/include/blz/vector.h"
#include "vector_types.h"

template blz::vector<UICharacterDetails::Console::BuffData>::~vector();
template void blz::vector<UICharacterDetails::Console::BuffData>::push_back(const UICharacterDetails::Console::BuffData&);
template void blz::vector<PvpPlayerResult>::resize(size_t);
template blz::vector<D3::Leaderboard::Score>& blz::vector<D3::Leaderboard::Score>::operator=(const blz::vector<D3::Leaderboard::Score>&);
template blz::vector<D3::Leaderboard::Score>::insert_raw_ranges blz::vector<D3::Leaderboard::Score>::insert_raw(const D3::Leaderboard::Score*, size_t);
template void blz::vector<D3::Leaderboard::Score>::reserve(size_t);
template void blz::vector<UISeasonStash::Console::SeasonStashItemProvider::StashItemData>::push_back(const UISeasonStash::Console::SeasonStashItemProvider::StashItemData&);
template void blz::vector<FormattedText>::reserve(size_t);
template XVarCollection::TrackedXVar& blz::vector<XVarCollection::TrackedXVar>::emplace_back(XVarBase*&&, const XVariant&);
