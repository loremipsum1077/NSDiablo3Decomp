// X/Server/Ai/ChampionBrains.cpp
// Byte-matching decompilation of Diablo III 2.6.2 (Switch, clang 5.0.1).
// Original file: X/Server/Ai/ChampionBrains.cpp (src/ spells the directory X/Server/AI).
//
// Evidence used:
//   build/dwarf/headers/X/Server/Ai/ChampionBrains.cpp - inventory: sResetPowerTime line 39
//     [0x71007a2c00, 264 bytes], sResetCloneTime line 56 [inlined only], sResetGhostlyTime
//     line 67 [inlined only], static const int32 CLONE_TIMEOUT_TICKS line 91,
//     ChampionBrainActionUpdate line 92 [0x71007a2d10, 668 bytes].
//   build/ghidra/decomp/X/Server/Ai/ChampionBrains.cpp - only used to cross-check the shapes.
//   The line-attributed listings: F0 columns are lines in this file, F1..F8 lines inside the
//     inlined helpers (SigmaThreadLocalStorage.h, Game.h, ActorCommonData.h,
//     ChainedDataArrayTemplated.h, XPtr64.h, FastAttributeStructs.h, MathGeneral.inl, Random.h).
//
// Evidence notes:
//   * the ghost else-arm sResetGhostlyTime(idACD) call was tail-merged in the original.  DWARF
//     records a single inline site (line 144, inside the bGhosted block) and the listing keeps
//     one inlined copy (0x71007a2f18) reached both from "next tick == 0" (cbz 0x7a2ea0) and from
//     the fired path.  The clone block keeps two inline sites (114 and 120) because its copies
//     were not merged.
//   * attribute 576 (0xFFFFF240, "sub x1, x22, #0x1") is spelled
//     ATTRIBUTE_CHAMPION_GHOSTLY_SAVED_DODGE here; only the spelling is in question.
//   * EPSILON / TICKS_PER_SECOND / GLOBALSNO_POWERS_CHAMPION_CLONE / CLONE_TIMEOUT_TICKS are
//     the named constants for the immediates 1e-06 (s_intTypeLookup+0x60), 60.0f
//     (s_intTypeLookup+0x78), 1337 and 0x78; EPSILON must be 1e-06f for the bytes to hold.
//   * BrainAPIDoSpecialPower is declared by its DWARF owning header,
//     X/Server/AI/BrainComponents.h (X/Server/AI/BrainComponents.cpp:3685, 0x710078d700).
//   * sResetCloneTime / sResetGhostlyTime are `inline`: the DWARF gives them external-linkage
//     names (_Z15sResetCloneTime5ACDID / _Z17sResetGhostlyTime5ACDID) but neither is emitted
//     as a standalone symbol, so both bodies must have been inlined and no out-of-line copy
//     emitted.
//   * the DWARF source lines (39/56/67/91/92) are cited per definition; the exact line
//     placement is approximate and does not affect the bytes.

#include "../Actors/Actors.h"
#include "./ActorAI.h"
#include "./BrainComponents.h"
#include "../SGame.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Attributes/FastAttributeStructs.h"
#include "../../Common/GameConstants.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Target.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Random.h"

// X/Server/Ai/ChampionBrains.cpp:39  (0x71007a2c00, 264 bytes)
// dwNextTeleportTime = dwCurrentTime + RandRange_Internal(rng, nTimeMin, nTimeDelta).
void sResetPowerTime(ACDID idACD, Attrib attribTimeMin, Attrib attribTimeMax, Attrib attribTimer)
{
    ActorCommonData & tACD = ACDGet(idACD);                             // F0:45

    // F0:46 / F0:47: AttributesGetFloat(tACD, key) * 60.0f then Math::FloatToInt (F7:377).
    int32 nTimeMin = Math::FloatToInt(tACD.AttributesGetFloat(FastAttribKey(attribTimeMin)) * TICKS_PER_SECOND);
    int32 nTimeDelta = Math::FloatToInt(tACD.AttributesGetFloat(FastAttribKey(attribTimeMax)) * TICKS_PER_SECOND);

    uint32 dwCurrentTime = GameGetServerTime();                         // F0:49
    // F0:50 passes SGameGlobalsGet()+0x1c8 (SGameGlobals::rng) to RandGetDWord; the
    // "add w9, w20, w21" at F8:189 is RandRange_Internal's minimum add, one draw.
    uint32 dwNextTeleportTime = dwCurrentTime + RandRange_Internal(SGameGlobalsGet().rng, nTimeMin, nTimeDelta);

    tACD.AttributesSetInt(FastAttribKey(attribTimer), dwNextTeleportTime);  // F0:51 (tail call)
}

// X/Server/Ai/ChampionBrains.cpp:56  (inlined into ChampionBrainActionUpdate at lines 114 & 120)
inline void sResetCloneTime(ACDID idACD)
{
    sResetPowerTime(idACD, ATTRIBUTE_CHAMPION_CLONE_TIME_MIN_IN_SECONDS,
                    ATTRIBUTE_CHAMPION_CLONE_TIME_DELTA_IN_SECONDS,
                    ATTRIBUTE_CHAMPION_CLONE_NEXT_TICK);
}

// X/Server/Ai/ChampionBrains.cpp:67  (inlined into ChampionBrainActionUpdate at line 144;
// body: F0:71 get, F0:74 / F0:81 the two sResetPowerTime calls).
inline void sResetGhostlyTime(ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);                             // F0:70
    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_CHAMPION_GHOSTLY)) != 0) {  // F0:71
        sResetPowerTime(idACD, ATTRIBUTE_CHAMPION_GHOSTLY_INACTIVE_TIME_MIN_IN_SECONDS,
                        ATTRIBUTE_CHAMPION_GHOSTLY_INACTIVE_TIME_DELTA_IN_SECONDS,
                        ATTRIBUTE_CHAMPION_GHOSTLY_NEXT_TICK);                      // F0:74
    } else {
        sResetPowerTime(idACD, ATTRIBUTE_CHAMPION_GHOSTLY_ACTIVE_TIME_MIN_IN_SECONDS,
                        ATTRIBUTE_CHAMPION_GHOSTLY_ACTIVE_TIME_DELTA_IN_SECONDS,
                        ATTRIBUTE_CHAMPION_GHOSTLY_NEXT_TICK);                      // F0:81
    }
}

// X/Server/Ai/ChampionBrains.cpp:91
static const int32 CLONE_TIMEOUT_TICKS = 120;

// X/Server/Ai/ChampionBrains.cpp:92  (0x71007a2d10, 668 bytes)
void ChampionBrainActionUpdate(ActorID idActor)
{
    Actor * ptActor = ActorGet(idActor);                                // F0:95
    ACDID idACD = ptActor->idACD;                                       // F0:97
    ActorCommonData & tACD = ACDGet(idACD);                             // F0:98
    ActorAI & tAI = ActorGetAI(idActor);                                // F0:99

    if (!tACD.IsHidden() && tACD.IsAlive()) {                           // F0:101
        uint32 dwCurrentTime = GameGetServerTime();                     // F0:103

        if (tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_CHAMPION_CLONE_TIME_MIN_IN_SECONDS)) > EPSILON) {  // F0:106
            uint32 dwNextCloneTime = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_CHAMPION_CLONE_NEXT_TICK));  // F0:108
            if (dwNextCloneTime != 0) {                                 // F0:109
                if (dwCurrentTime >= dwNextCloneTime) {                 // F0:111
                    BrainAPIDoSpecialPower(idActor, GlobalSNOGet(GLOBALSNO_POWERS_CHAMPION_CLONE), cgTargetNone,
                                           CLONE_TIMEOUT_TICKS);        // F0:113
                    sResetCloneTime(idACD);                             // F0:120 (tail call after `return`)
                    return;
                }
            } else {
                sResetCloneTime(idACD);                                 // F0:114
            }
        }

        if (tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_CHAMPION_GHOSTLY_INACTIVE_TIME_MIN_IN_SECONDS)) > EPSILON) {  // F0:125
            uint32 dwNextGhostTime = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_CHAMPION_GHOSTLY_NEXT_TICK));  // F0:127
            if (dwNextGhostTime != 0) {                                 // F0:128
                if (dwCurrentTime >= dwNextGhostTime) {                 // F0:130
                    // F0:132: `cmp w0, #0` + `cset w24, eq` in the original, so the stored
                    // boolean is the `== 0` (negated) form and the branch tests the raw int.
                    BOOL bGhosted = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_CHAMPION_GHOSTLY)) == 0;  // F0:132
                    if (!bGhosted) {                                    // F0:133
                        tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS),
                                                tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_CHAMPION_GHOSTLY_SAVED_DODGE)));  // F0:135
                    } else {
                        tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_CHAMPION_GHOSTLY_SAVED_DODGE),
                                                tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS)));  // F0:139
                        tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS), 1.0f);  // F0:140
                    }
                    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_CHAMPION_GHOSTLY), bGhosted);  // F0:143
                    sResetGhostlyTime(idACD);                           // F0:144 (inlined body at F0:71..81)
                }
            } else {
                sResetGhostlyTime(idACD);                               // HYPOTHESIS, tail-merged with F0:144
            }
        }
    }

    if (tAI.pOriginalActionUpdateFunc)                                  // F0:155
        tAI.pOriginalActionUpdateFunc(idActor);                         // F0:157
}
