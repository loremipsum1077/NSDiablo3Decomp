// 0x71006739b0 sits in an unnamed 2.7.7 unit between BBEUtil.cpp and VisualInventory.cpp (0x71006739a0 is the
// destructor of the set below, 0x71006740c0 the unit's static initializer). Its caller is
// UIWhatsNewSeason::Console (the season cosmetic rewards list).
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CosmeticItems.h"
#include "../../Contrib/Contrib/blz/include/blz/set.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"

static blz::set<uint32> s_setReplaceableCosmetics;  // 0x710191cb00

static const uint32 COSMETIC_SLOT_EMPTY = (uint32)-2;

static inline uint32 sPickCosmetic(uint32 uCurrent, uint32 uGBID)
{
    if (uCurrent == COSMETIC_SLOT_EMPTY)
        return uGBID;
    return s_setReplaceableCosmetics.find(uGBID) == s_setReplaceableCosmetics.end() ? uGBID : uCurrent;
}

// NON_MATCHING: same control flow and calls (1816 vs 1800 bytes); stack slots/registers differ (the target reuses the
// insert temporary slot for a cosmetic slot and keeps the inserted GBID in w20 instead of reloading it).
// Probably: picks one season cosmetic reward per cosmetic type (frame, pennant, pet, wings, in that output order).
// Per type, the first reward is kept unless a later reward that is not in the built-in list of 13 GBIDs
// (filled on the first call) replaces it; the list is then rebuilt from the picks.
extern "C" void sub_71006739b0(blz::vector<uint32> & arRewards)
{
    if (s_setReplaceableCosmetics.empty())
    {
        s_setReplaceableCosmetics.insert(0x81a32d2b);
        s_setReplaceableCosmetics.insert(0x712f188c);
        s_setReplaceableCosmetics.insert(0x3892d700);
        s_setReplaceableCosmetics.insert(0x2f0c000a);
        s_setReplaceableCosmetics.insert(0xadb46964);
        s_setReplaceableCosmetics.insert(0x208e759b);
        s_setReplaceableCosmetics.insert(0x9b7d818c);
        s_setReplaceableCosmetics.insert(0x1bd32c54);
        s_setReplaceableCosmetics.insert(0x22edf5db);
        s_setReplaceableCosmetics.insert(0x7eee78d2);
        s_setReplaceableCosmetics.insert(0x1582b328);
        s_setReplaceableCosmetics.insert(0xda3884e3);
        s_setReplaceableCosmetics.insert(0x1869e294);
    }

    uint32 uWings = COSMETIC_SLOT_EMPTY;
    uint32 uPet = COSMETIC_SLOT_EMPTY;
    uint32 uPennant = COSMETIC_SLOT_EMPTY;
    uint32 uFrame = COSMETIC_SLOT_EMPTY;

    for (blz::vector<uint32>::iterator it = arRewards.begin(), itEnd = arRewards.end(); it != itEnd; ++it)
    {
        switch (CosmeticItems_GetCosmeticItemTypeFromGBID(*it))
        {
            case COSMETIC_ITEM_WINGS: uWings = sPickCosmetic(uWings, *it); break;
            case COSMETIC_ITEM_PENNANT: uPennant = sPickCosmetic(uPennant, *it); break;
            case COSMETIC_ITEM_PET: uPet = sPickCosmetic(uPet, *it); break;
            case COSMETIC_ITEM_FRAME: uFrame = sPickCosmetic(uFrame, *it); break;
            default: break;
        }
    }

    arRewards.clear();
    if (uFrame != COSMETIC_SLOT_EMPTY)
        arRewards.push_back(uFrame);
    if (uPennant != COSMETIC_SLOT_EMPTY)
        arRewards.push_back(uPennant);
    if (uPet != COSMETIC_SLOT_EMPTY)
        arRewards.push_back(uPet);
    if (uWings != COSMETIC_SLOT_EMPTY)
        arRewards.push_back(uWings);
}
