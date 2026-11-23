// Checks the class templates in X/Common/Definitions/Definitions.h (ScopedDefinitionLockR,
// ScopedSingletonDefinitionLockR, AutoDefinitionR) through small functions of the binary that
// inline them, and through the one out-of-line instantiation,
// ScopedDefinitionLockR<AppearanceDefinition>::~ScopedDefinitionLockR().
//
// The definition structs here are stand-ins that only place the fields the callers read (offsets
// from the disassembly), and the group definition globals are declared as GroupDefinition *.
// The DefinitionLockR / DefinitionUnlockR / AutoDefinitionLockR specializations are written the
// way each definition's own header has them (line numbers in the comments).
#include "X/Common/Definitions/Definitions.h"

struct ActorCommonData;
struct Target;

extern GroupDefinition * gptGroupDefAppearance;
extern GroupDefinition * gptGroupDefActor;
extern GroupDefinition * gptGroupDefQuest;
extern GroupDefinition * gptGroupDefGlobal;
extern GroupDefinition * gptGroupDefPower;

struct AppearanceDefinition { RequiredDefinitionHeader tHeader; };
struct ActorDefinition { RequiredDefinitionHeader tHeader; char pad[0x68 - 0xc]; SNO snoAnimSet; };
struct QuestDefinition { RequiredDefinitionHeader tHeader; char pad[0x24 - 0xc]; int32 eAct; };
struct GlobalDefinition { RequiredDefinitionHeader tHeader; char pad0[0x2d4 - 0xc]; float fA; char pad1[0x2f0 - 0x2d8]; float fB; };
struct PowerDefinition { RequiredDefinitionHeader tHeader; char pad0[0xc0 - 0xc]; uint32 uFlags; char pad1[0x2e8 - 0xc4]; BOOL bScripted; };

// AppearanceDefinition.h:933, 934
template <> inline const AppearanceDefinition * DefinitionLockR<AppearanceDefinition>(SNO sno) {
    return (const AppearanceDefinition *)gptGroupDefAppearance->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<AppearanceDefinition>(const AppearanceDefinition * pdef) {
    void * p = (void *)pdef;
    gptGroupDefAppearance->UnlockR(p);
}

// ActorDefinition.h:281, 282
template <> inline const ActorDefinition * DefinitionLockR<ActorDefinition>(SNO sno) {
    return (const ActorDefinition *)gptGroupDefActor->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ActorDefinition>(const ActorDefinition * pdef) {
    void * p = (void *)pdef;
    gptGroupDefActor->UnlockR(p);
}

// QuestDefinition.h:453 (LockRFast; the unlock is empty and leaves no trace in callers)
template <> inline const QuestDefinition * DefinitionLockR<QuestDefinition>(SNO sno) {
    return (const QuestDefinition *)gptGroupDefQuest->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<QuestDefinition>(const QuestDefinition *) {}

// GlobalDefinition.h:790
SNO GlobalSNOGet(int32);
template <> inline const GlobalDefinition * DefinitionLockR<GlobalDefinition>() {
    return (const GlobalDefinition *)gptGroupDefGlobal->LockRFast(GlobalSNOGet(0));
}

// PowerDefinition.h:161, 162, 170, 171
template <> inline const PowerDefinition * DefinitionLockR<PowerDefinition>(SNO sno) {
    return (const PowerDefinition *)gptGroupDefPower->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<PowerDefinition>(const PowerDefinition * _pdef) {
    void * p = (void *)_pdef;
    gptGroupDefPower->UnlockR(p);
}
template <> inline const PowerDefinition * AutoDefinitionR<PowerDefinition>::AutoDefinitionLockR(SNO sno) {
    return (const PowerDefinition *)gptGroupDefPower->LockR(sno, FALSE);
}
template <> inline void AutoDefinitionR<PowerDefinition>::AutoDefinitionUnlockR(const PowerDefinition * _pdef) {
    void * p = (void *)_pdef;
    gptGroupDefPower->UnlockR(p);
}

typedef ScopedDefinitionLockR<ActorDefinition> ScopedActorDefinitionLockR;
typedef ScopedDefinitionLockR<QuestDefinition> ScopedQuestDefinitionLockR;
typedef ScopedDefinitionLockR<PowerDefinition> ScopedPowerDefinitionLockR;
typedef ScopedSingletonDefinitionLockR<GlobalDefinition> ScopedGlobalDefinitionLockR;
typedef AutoDefinitionR<PowerDefinition> AutoPowerDefinitionR;

// The out-of-line destructor, 0x71000d2860 (emitted in X/Client/Scene.cpp).
template class ScopedDefinitionLockR<AppearanceDefinition>;

// X/Common/Definitions/ActorDefinition.cpp:1078 -- ScopedDefinitionLockR constructor and destructor
SNO ActorGetAnimSetSNO(const SNO snoActor) {
    ScopedActorDefinitionLockR ptActor(snoActor);

    return ptActor->snoAnimSet;
}

// X/Common/Definitions/QuestDefinition.cpp:1977 -- ScopedDefinitionLockR over LockRFast
int32 QuestGetAct(SNO snoQuest) {
    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    return pdefQuest->eAct;
}

// X/Client/Console/ControllerUtils.cpp:160 -- ScopedSingletonDefinitionLockR
namespace Console { namespace ControllerUtils {
BOOL RStickIsInDeadZone(int32 nControllerIndex, float, float);
BOOL RStickIsInDeadZone(int32 nControllerIndex) {
    ScopedGlobalDefinitionLockR pdefGlobal;
    return RStickIsInDeadZone(nControllerIndex, pdefGlobal->fA, pdefGlobal->fB);
}
} }

int32 PowerGeneralTagGetInt(AutoPowerDefinitionR pdefPower, uint32 dwTag);
int32 PowerGetDamageType(AutoPowerDefinitionR pdefPower, int32 eRuneType);
uint32 PowerFailureReasons(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL, BOOL);

// X/Server/Powers/Powers.cpp:5896 -- AutoDefinitionR(SNO) and ~AutoDefinitionR
int32 PowerGetBrainAction(SNO snoPower) {
    int32 eAction = -1;

    if (snoPower != SNO_NONE)
        eAction = PowerGeneralTagGetInt(snoPower, 0x50400);

    return eAction;
}

// X/Server/Powers/PowersSwig.cpp:550 -- AutoDefinitionR(ScopedDefinitionLockR &)
int32 PowerGetDamageType(SNO snoPower, int32 eRuneType) {
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    if (!pdefPower)
        return -1;

    return PowerGetDamageType(pdefPower, eRuneType);
}

// X/Common/Powers/PowersCommon.cpp:5671 -- AutoDefinitionR::GetSNO
BOOL PowerCanMoveForwardBeforeContactFrame(AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return FALSE;

    return (pdefPower->uFlags >> 7) & 1;
}

// X/Common/Powers/PowersCommon.cpp -- AutoDefinitionR::operator->
BOOL PowerIsScripted(AutoPowerDefinitionR pdefPower) {
    return pdefPower->bScripted;
}

// X/Server/AI/ActorAI.cpp:1116 -- AutoDefinitionR copy constructor (never locks) and destructor
BOOL ACDCanAttackTarget(const ActorCommonData & tACD, const Target & tTarget, AutoPowerDefinitionR pdefPower, uint32 * pdwFailureReasons) {
    uint32 dwFailureReasons;
    if (pdwFailureReasons == NULL)
        dwFailureReasons = PowerFailureReasons(tACD, pdefPower, &tTarget, TRUE, FALSE);
    else
        dwFailureReasons = *pdwFailureReasons;

    if (dwFailureReasons & 0x400)
        return FALSE;
    return !(dwFailureReasons & 0x200);
}
