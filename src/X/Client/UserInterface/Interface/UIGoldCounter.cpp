#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Math/MathGeneral.h"

// Declared here because X/Client/UserInterface/Interface/UIGoldCounter.h is not in the reference
// tree. It holds only these declarations.
namespace UIGoldCounter {
void InitializeForGame();
void DisposeForGame();
void Update();
void Expired(uint32 nGold);
void NewAmount(uint32 nGold);
}  // namespace UIGoldCounter

struct GoldCounterGlobals {  // size 0x10
    void Initialize() {
        bCounterIsActive = FALSE;
        nGoldCollected = 0;
        nDisplayedGoldCollected = 0;
        nDisplayIncrement = 0;
    }

    BOOL bCounterIsActive;  // 0x00
    uint32 nGoldCollected;  // 0x04
    uint32 nDisplayedGoldCollected;  // 0x08
    uint32 nDisplayIncrement;  // 0x0c
};
static_assert(sizeof(GoldCounterGlobals) == 0x10, "GoldCounterGlobals");

static GoldCounterGlobals * sgptGoldCounterGlobals;

void UIGoldCounter::InitializeForGame()
{
    sgptGoldCounterGlobals = new GoldCounterGlobals;

    sgptGoldCounterGlobals->Initialize();
}

void UIGoldCounter::DisposeForGame()
{
    delete sgptGoldCounterGlobals;
    sgptGoldCounterGlobals = NULL;
}

void UIGoldCounter::Update()
{
    if (sgptGoldCounterGlobals->nGoldCollected > sgptGoldCounterGlobals->nDisplayedGoldCollected)
    {
        sgptGoldCounterGlobals->nDisplayedGoldCollected =
            Math::Min(sgptGoldCounterGlobals->nDisplayedGoldCollected + sgptGoldCounterGlobals->nDisplayIncrement,
                      sgptGoldCounterGlobals->nGoldCollected);
    }
}

void UIGoldCounter::Expired(uint32 nGold)
{
    sgptGoldCounterGlobals->bCounterIsActive = FALSE;
    sgptGoldCounterGlobals->nGoldCollected = 0;
    sgptGoldCounterGlobals->nDisplayedGoldCollected = 0;
}

void UIGoldCounter::NewAmount(uint32 nGold)
{
    if (nGold == 0)
        return;

    const uint32 INCREMENT_TICKS = 30;

    sgptGoldCounterGlobals->bCounterIsActive = TRUE;
    sgptGoldCounterGlobals->nGoldCollected = nGold;

    uint32 nDifference = nGold - sgptGoldCounterGlobals->nDisplayedGoldCollected;
    if (nDifference < INCREMENT_TICKS)
        sgptGoldCounterGlobals->nDisplayIncrement = 1;
    else
        sgptGoldCounterGlobals->nDisplayIncrement = nDifference / INCREMENT_TICKS;
}
