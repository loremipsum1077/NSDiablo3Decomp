// 2.7.7: Kanai's powers panel, an unnamed 0x30-byte class (0x710019aec0..0x710019bb60) shared with the
// skills, leaderboard-inspect and equipment-manager skill screens. One definition for all includers.
#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UIID.h"
#include "../../../Sigma/Math/Vector.h"

struct Player;

// One of the four 0xd8-byte children, one per Kanai's cube slot (weapon, armor, jewelry, universal).
// Built by sub_710019aec0. unk_34..unk_80 are the five controls' layout positions read at build time;
// unk_84..unk_d0 are the compact layout used when the fourth slot is unlocked.
struct Unk19aec0 {  // size 0xd8
    int32 unk_00;  // 0x00 slot index
    UIID unk_08;  // 0x08 slot window
    UIID unk_10;  // 0x10 item visual
    UIID unk_18;  // 0x18 scroll box
    UIID unk_20;  // 0x20 power name text
    UIID unk_28;  // 0x28 power details text
    bool unk_30;  // 0x30 hidden unless the fourth slot is unlocked
    Vector2D unk_34;  // 0x34
    Vector2D unk_3c;  // 0x3c
    Vector2D unk_44;  // 0x44
    Vector2D unk_4c;  // 0x4c
    Vector2D unk_54;  // 0x54
    Vector2D unk_5c;  // 0x5c
    Vector2D unk_64;  // 0x64
    Vector2D unk_6c;  // 0x6c
    Vector2D unk_74;  // 0x74
    Vector2D unk_7c;  // 0x7c
    Vector2D unk_84;  // 0x84
    Vector2D unk_8c;  // 0x8c
    Vector2D unk_94;  // 0x94
    Vector2D unk_9c;  // 0x9c
    Vector2D unk_a4;  // 0xa4
    Vector2D unk_ac;  // 0xac
    Vector2D unk_b4;  // 0xb4
    Vector2D unk_bc;  // 0xbc
    Vector2D unk_c4;  // 0xc4
    Vector2D unk_cc;  // 0xcc
};
static_assert(sizeof(Unk19aec0) == 0xd8, "Unk19aec0");

struct Unk19b630 {  // size 0x30
    ~Unk19b630()
    {
        delete unk_00[3];
        delete unk_00[2];
        delete unk_00[1];
        delete unk_00[0];
    }
    Unk19aec0 * unk_00[4];  // 0x00
    UIID unk_20;  // 0x20 parent window
    UIID unk_28;  // 0x28 2x2 background
};
static_assert(sizeof(Unk19b630) == 0x30, "Unk19b630");

extern "C" void sub_710019aec0(Unk19aec0 * pThis, int32 nSlot, const UIID & idWindow, const UIID & idVisual,
                               const UIID & idScrollBox, const UIID & idName, const UIID & idDetails, float flX,
                               float flY, bool bFourthSlotOnly);
extern "C" void sub_710019b0b0(Unk19aec0 * pThis, int32 gbidPower, BOOL bFourthSlotUnlocked, int32 nClass, BOOL bIsMale);
extern "C" void sub_710019b630(Unk19b630 * pThis, const UIID & idParent);
extern "C" BOOL sub_710019b960(Unk19b630 * pThis);  // any child scroll box can scroll
extern "C" void sub_710019b9d0(Unk19b630 * pThis);  // reset child scroll positions
extern "C" void sub_710019ba50(Unk19b630 * pThis, float flDelta);  // scroll child scroll boxes
extern "C" void sub_710019bab0(Unk19b630 * pThis, const Player * ptPlayer);  // refresh from player
