#include "ReplayPlayerResolver.h"

namespace Legacy {













ReplayPlayerResolver::ReplayPlayerResolver()
    : OnlineService::PlayerResolver(0, "Legacy::ReplayPlayerResolver")
{
}



ReplayPlayerResolver::~ReplayPlayerResolver()
{
}



void ReplayPlayerResolver::ResolvePlayerData(uint32 uIndex)
{
}





void ReplayPlayerResolver::AccountResolved(const D3::Account::SavedDefinition & tSavedDefinition)

{
    OnlineService::PlayerResolveData & tResolveData = GetResolvedPlayer(0);

    tResolveData.tAccountDefinition = tSavedDefinition;
    tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_ACCOUNT_DEFINITION;
    tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_MAIL;
    CheckAllPlayersResolved();
}



void ReplayPlayerResolver::HeroResolved(const D3::Hero::SavedDefinition & tSavedDefinition)

{
    OnlineService::PlayerResolveData & tResolveData = GetResolvedPlayer(0);

    tResolveData.tHeroDefinition = tSavedDefinition;
    tResolveData.uHeroId = tSavedDefinition.digest().hero_id().id_low();
    tResolveData.uResolveResultFlags |= 1 << OnlineService::RESOLVE_RESULT_HERO_DEFINITION;
    CheckAllPlayersResolved();
}

}
