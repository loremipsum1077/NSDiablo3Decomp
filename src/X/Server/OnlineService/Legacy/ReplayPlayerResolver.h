// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/OnlineService/Legacy/ReplayPlayerResolver.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../PlayerResolver.h"

namespace D3 {
namespace Account { class SavedDefinition;
}
}
namespace D3 {
namespace Hero { class SavedDefinition;
}
}

namespace Legacy {
class ReplayPlayerResolver : public OnlineService::PlayerResolver {  // size 0xcb8
public:
    ReplayPlayerResolver();
    virtual ~ReplayPlayerResolver();  // vtable slot 0
    void HeroResolved(const D3::Hero::SavedDefinition & tSavedDefinition);
    void AccountResolved(const D3::Account::SavedDefinition & tSavedDefinition);
private:
    virtual void ResolvePlayerData(uint32 uIndex);  // vtable slot 2
};
static_assert(sizeof(ReplayPlayerResolver) == 0xcb8, "ReplayPlayerResolver");

}  // namespace Legacy
