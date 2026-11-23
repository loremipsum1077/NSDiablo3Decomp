#include "PlayerAccount.h"
#include "Definitions/GlobalDefinition.h"
#include "Protobuf/Account.pb.h"

// Original source line layout retained for the inlined banner helpers.





















PlayerAccountBanner::PlayerAccountBanner()
{
    Import(D3::Account::BannerConfiguration::default_instance());
}

PlayerAccountBanner::PlayerAccountBanner(const D3::Account::BannerConfiguration & tBanner)

{
    Import(tBanner);
}



static uint32 sGetHashForBannerIndex(int32 nIndex, XPtr64<BannerTexturePair> arBannerTextures)
{


    const BannerTexturePair & tBannerTexture = arBannerTextures[nIndex];

    return tBannerTexture.snoTexture;
}






static uint32 sGetHashForBannerIndex(int32 nIndex, XPtr64<BannerColorSet> arBannerColors)
{


    const BannerColorSet & tBannerColor = arBannerColors[nIndex];
    return BannerColorSet::XHash(tBannerColor);
}



static uint32 sGetHashForBannerIndex(int32 nIndex, XPtr64<EpicBannerDescription> arEpicBannerDescriptions)
{
    uint32 uHash;

    if (nIndex == -1)
    {
        uHash = 0;
    }
    else
    {
        uHash = EpicBannerDescription::XHash(arEpicBannerDescriptions[nIndex]);
    }
    return uHash;
}


void PlayerAccountBanner::Export(D3::Account::BannerConfiguration & tBanner) const

{
    ScopedGlobalDefinitionLockR tGlobals;
    uint32 uHash;
    uHash = sGetHashForBannerIndex(uBannerIndex, tGlobals->tBannerParams.arTexBackgrounds);
    tBanner.set_banner_shape(uHash);
    uHash = sGetHashForBannerIndex(nSigilMain, tGlobals->tBannerParams.arTexMainSigils);
    tBanner.set_sigil_main(uHash);
    uHash = sGetHashForBannerIndex(nSigilAccent, tGlobals->tBannerParams.arTexSigilAccents);
    tBanner.set_sigil_accent(uHash);
    uHash = sGetHashForBannerIndex(nPatternColorIndex, tGlobals->tBannerParams.arColorSets);
    tBanner.set_pattern_color(uHash);
    uHash = sGetHashForBannerIndex(nBackgroundColorIndex, tGlobals->tBannerParams.arColorSets);
    tBanner.set_background_color(uHash);
    uHash = sGetHashForBannerIndex(nSigilColorIndex, tGlobals->tBannerParams.arColorSets);
    tBanner.set_sigil_color(uHash);
    uHash = sGetHashForBannerIndex(nPattern, tGlobals->tBannerParams.arTexPatterns);
    tBanner.set_pattern(uHash);
    uHash = sGetHashForBannerIndex(nEpicBanner, tGlobals->tBannerParams.arEpicBannerDescriptions);
    tBanner.set_epic_banner(uHash);
    if (tGlobals->tBannerParams.serSigilPlacements.ElementCount(sizeof(BannerSigilPlacement)))
    {
        const BannerSigilPlacement & tSigilPlacement = tGlobals->tBannerParams.arSigilPlacements[nPlacementIndex];
        uHash = XHashKey<LPCSTR>(tSigilPlacement.szName);
    }
    else
    {
        uHash = 0;
    }
    tBanner.set_sigil_placement(uHash);

    tBanner.set_use_sigil_variant(bUseSigilVariant != FALSE);
}



static int32 sGetIndexForBannerHash(uint32 uHash, const BannerParamsMaps::BannerParamsIndexMap & tMap)
{

    uint32 nIndex;

    if (!tMap.Lookup(uHash, nIndex))
        nIndex = 0;

    return nIndex;
}


void PlayerAccountBanner::Import(const D3::Account::BannerConfiguration & tBanner)

{
    ScopedGlobalDefinitionLockR tGlobals;

    uBannerIndex = sGetIndexForBannerHash(tBanner.banner_shape(), tGlobals->tBannerParams.ptParamsMaps->mapTexBackgrounds);
    nSigilMain = sGetIndexForBannerHash(tBanner.sigil_main(), tGlobals->tBannerParams.ptParamsMaps->mapTexMainSigils);
    nSigilAccent = sGetIndexForBannerHash(tBanner.sigil_accent(), tGlobals->tBannerParams.ptParamsMaps->mapTexSigilAccents);
    nPatternColorIndex = sGetIndexForBannerHash(tBanner.pattern_color(), tGlobals->tBannerParams.ptParamsMaps->mapColorSets);
    nBackgroundColorIndex = sGetIndexForBannerHash(tBanner.background_color(), tGlobals->tBannerParams.ptParamsMaps->mapColorSets);
    nSigilColorIndex = sGetIndexForBannerHash(tBanner.sigil_color(), tGlobals->tBannerParams.ptParamsMaps->mapColorSets);
    nPlacementIndex = sGetIndexForBannerHash(tBanner.sigil_placement(), tGlobals->tBannerParams.ptParamsMaps->mapSigilPlacements);
    nPattern = sGetIndexForBannerHash(tBanner.pattern(), tGlobals->tBannerParams.ptParamsMaps->mapTexPatterns);
    uint32 nBannerIndex;
    uint32 uHash = tBanner.epic_banner();
    if (tGlobals->tBannerParams.ptParamsMaps->mapEpicBannerDescriptions.Lookup(uHash, nBannerIndex))
    {
        nEpicBanner = nBannerIndex;
    }
    else
    {
        nEpicBanner = -1;
    }

    bUseSigilVariant = tBanner.use_sigil_variant();
}











BOOL PlayerAccountBanner::operator==(const PlayerAccountBanner & tRHS) const
{

    return uBannerIndex == tRHS.uBannerIndex &&
           nSigilMain == tRHS.nSigilMain &&
           nSigilAccent == tRHS.nSigilAccent &&
           nPatternColorIndex == tRHS.nPatternColorIndex &&
           nBackgroundColorIndex == tRHS.nBackgroundColorIndex &&
           nSigilColorIndex == tRHS.nSigilColorIndex &&
           nPlacementIndex == tRHS.nPlacementIndex &&
           nPattern == tRHS.nPattern &&
           nEpicBanner == tRHS.nEpicBanner &&
           bUseSigilVariant == tRHS.bUseSigilVariant;
}




PlayerAccount::PlayerAccount()
    : tHandle()
    , uBnetAccountId(0)
    , uDigestFlags(0)
    , uPartitionFlags(0)
    , tBanner()
{
    usName[0] = '\0';
}
