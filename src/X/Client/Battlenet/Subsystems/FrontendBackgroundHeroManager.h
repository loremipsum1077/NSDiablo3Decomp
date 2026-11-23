// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Battlenet/Subsystems/FrontendBackgroundHeroManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/VisualInventory.h"

struct PlayerAccountBanner;

typedef int32 PlayerClass;

enum {
    HERO_SLOT_1 = 0,
    HERO_SLOT_2 = 1,
    HERO_SLOT_3 = 2,
    HERO_SLOT_4 = 3,
    NUM_HERO_SLOTS = 4,
};

typedef int32 BackgroundHeroSlot;

class BackgroundHeroDescription {  // size 0xac
public:
    BackgroundHeroDescription();
    BOOL operator==(const BackgroundHeroDescription & tDesc) const;
    BOOL operator!=(const BackgroundHeroDescription & tDesc) const;
    BackgroundHeroDescription & operator=(const BackgroundHeroDescription & tDesc);
    void SetDemoHero(PlayerClass eClass, BOOL fIsFemale, BOOL fIsHardcore);
    void SetPlayerHero(PlayerClass eClass, BOOL fIsFemale, BOOL fIsHardcore, BOOL fIsDead, const VisualEquipment & tVisualEquipment);
    RActorID m_idRActor;  // 0x00
    RActorID m_idPetRActor;  // 0x04
    BOOL m_fIsFemale;  // 0x08
    BOOL m_fIsHardcore;  // 0x0c
    PlayerClass m_eClass;  // 0x10
    VisualEquipment m_tVisualEquipment;  // 0x14
    BOOL m_fIsDemo;  // 0xa4
    BOOL m_fIsDead;  // 0xa8
};
static_assert(sizeof(BackgroundHeroDescription) == 0xac, "BackgroundHeroDescription");

class FrontendDemoHeroAnimationManager {  // size 0x18
public:
    FrontendDemoHeroAnimationManager();
    void StartAnimation(RActorID idDemoHeroRActor);
    void StopAnimation();
    void Update();
private:
    RActorID m_idAnimatingRActor;  // 0x00
    Blizzard::Time::Timestamp m_tHeroAnimationStartTime;  // 0x08
    float m_heroEmotePercentComplete;  // 0x10
};
static_assert(sizeof(FrontendDemoHeroAnimationManager) == 0x18, "FrontendDemoHeroAnimationManager");

class FrontendBackgroundHeroManager {  // size 0x2d0
public:
    FrontendBackgroundHeroManager();
    RActorID GetHeroRActorId(BackgroundHeroSlot eSlot) const;
    RActorID GetBannerRActorId() const;
    BOOL ChangeBackgroundHero(BackgroundHeroSlot eSlot, const BackgroundHeroDescription & tHeroDescription);
    BOOL ChangeBackgroundHero(BackgroundHeroSlot, SNO, float);
    BOOL ChangeBackgroundBanner(const PlayerAccountBanner & tBannerConfiguration);
    void ClearBackgroundBanner();
    void ClearBackgroundHero(BackgroundHeroSlot eSlot);
    void ClearAllBackgroundHeros() { ClearBackgroundHero(NUM_HERO_SLOTS); }  // line 111
    BOOL IsBackgroundHeroDead(BackgroundHeroSlot eSlot) const;
    void Update();
private:
    BackgroundHeroDescription m_arBackgroundHeros[4];  // 0x00
    AttachmentID m_idBackgroundBanner;  // 0x2b0
    FrontendDemoHeroAnimationManager m_tDemoHeroAnimationManager;  // 0x2b8
};
static_assert(sizeof(FrontendBackgroundHeroManager) == 0x2d0, "FrontendBackgroundHeroManager");


// Functions defined in this file (bodies are not in the DWARF):
//   line    35  BOOL BackgroundHeroDescription::operator!=(const BackgroundHeroDescription & tDesc) const  [inline]
//   line   111  void FrontendBackgroundHeroManager::ClearAllBackgroundHeros()  [inlined into 5 functions]
