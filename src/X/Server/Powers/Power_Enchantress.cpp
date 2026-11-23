// X/Server/Powers/Power_Enchantress.cpp
//
// Byte-matching decompilation from the Diablo III 2.6.2 (Switch, clang 5.0.1)
// debug build.  Inventory (X/Server/Powers/Power_Enchantress.cpp):
//   X/Server/Powers/Power_Enchantress.cpp:31
//     void Enchantress_Disorient(PowerID idPower, void *)  [0x71009693a0, 504 bytes]
//   X/Server/Powers/Power_Enchantress.cpp:67
//     void Enchantress_Charm(PowerID idPower, void *)     [0x71009695a0, 408 bytes]
//
// Both function bodies are attributed to these lines by the listing:
//   Disorient: 34 prologue, 35 PowerTryToGet, 36 check, 41 PowerGetOwner,
//              42 check, 48 DoFirstTimeCosts, 53 listTargets, 54 tTargets,
//              55 ChooseTargetsInit..., 56 DoChooseTargets, 60 payload init,
//              61 DoPayload, 62 scope end.
//   Charm:     70 prologue, 71 PowerTryToGet, 72 check, 78 DoFirstTimeCosts,
//              84 GetACD, 85 ACDTryToGet, 92 buff ctor, 93 bPositiveEffect,
//              94 nAppearanceParameter, 95/96 durations, 97/98 flags,
//              100 PowerGetSNO, 101 rune->attribute, 102 check, 104 set attrib,
//              107 BuffAddToACD, 108 dtor/return.
//
// Notes:
//  * The power-formula tags 0x513C0/0x513D0 and the appearance parameter 28 are
//    written as literals; their original named constants are not in evidence
//    (immediates at 0x969670/0x969674 and 0x969680 in the listing).
//  * PowerHelper_DoFirstTimeCosts is declared in PowerHelper.h (its DWARF owner
//    file); this translation unit includes that header rather than repeating a
//    prototype here.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/Target.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/MemoryPools.h"

#include "PowerAPI.h"
#include "PowerHelper.h"
#include "Powers.h"

// _Z21Enchantress_DisorientiPv  0x71009693a0  504 bytes
void Enchantress_Disorient(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;

    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (!ptACD)
        return;

    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
        return;

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerAPIChooseTargets tTargets;
    PowerHelper_ChooseTargetsInitAroundTargetFromPayload(idPower, tTargets);
    PowerAPI_DoChooseTargets(tTargets, listTargets);

    PowerAPIAttackPayload tPayload;
    PowerHelper_AttackPayloadInit_Standard(idPower, tPayload);
    PowerAPI_DoPayload(tPayload, listTargets, ACDID_INVALID);
}

// _Z17Enchantress_CharmiPv  0x71009695a0  408 bytes
void Enchantress_Charm(PowerID idPower, void *)
{
    Power * ptPower = PowerTryToGet(idPower);
    if (!ptPower)
        return;

    if (!PowerHelper_DoFirstTimeCosts(idPower, TRUE))
        return;

    ACDID idACDTarget = ptPower->tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
        return;

    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bPositiveEffect = FALSE;
    tBuffStartData.nAppearanceParameter = 28;
    tBuffStartData.dwDurationMin = Math::FloatToInt(PowerGetFormulaValue(idPower, 0x000513C0) * 60.0f);
    tBuffStartData.dwDurationDelta = Math::FloatToInt(PowerGetFormulaValue(idPower, 0x000513D0) * 60.0f);
    tBuffStartData.bCharm = TRUE;
    tBuffStartData.bFailsIfCharmImmune = TRUE;

    SNO snoPower = PowerGetSNO(idPower);
    Attrib eAttrib = PowerRuneTypeAndBuffIndexToAttribute(snoPower, -1, tBuffStartData.nBuffIndex);
    if (eAttrib != -1)
    {
        FastAttribSetValueInt(tBuffStartData.idFastAttrib, MakeAttribKey(eAttrib, snoPower), 1);
    }

    PowerAPI_BuffAddToACD(tBuffStartData, *ptACDTarget);
}
