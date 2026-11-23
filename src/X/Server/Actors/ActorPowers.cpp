// Original file: X/Server/Actors/ActorPowers.cpp
#include "ActorPowers.h"
#include "../../Common/Powers/SpellEffectCommon.h"
#include "Actors.h"
#include "../Powers/PowerAPI.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Player.h"
#include "../SPlayer.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../Powers/Powers.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Sigma/Memory/SigmaMemory.h"

struct ActorPowers {
    PowerID idPrimaryPower;
    XPooledDataIDList m_listPowers;
    PowerID idPowerThatOwnsMe;
    SpellEffect eSpellEffectToRun;
};
static_assert(sizeof(ActorPowers) == 0x38, "ActorPowers");
struct GlobalPower {
    SNO snoPower;
    BOOL operator==(const GlobalPower & powerSource) const { return snoPower == powerSource.snoPower; }
};
typedef XPooledList<GlobalPower, const GlobalPower &> XPooledGlobalPowerList;
struct ActorPowerGlobals {
    XChainedFixedBlockAllocator<ActorPowers> poolActorPowers;
    XListMemoryPool<GlobalPower> poolGlobalPowers;
    XPooledGlobalPowerList listGlobalPowers;
};
static_assert(sizeof(ActorPowerGlobals) == 0x88, "ActorPowerGlobals");

inline ActorPowers * sActorPowersGet(ActorID idActor) {
    Actor * ptActor = ActorGet(idActor);
    return ptActor->ptActorPowers;
}
inline ActorPowers * sActorPowersTryToGet(ActorID idActor) {
    Actor * ptActor = ActorTryToGet(idActor);
    return ptActor ? ptActor->ptActorPowers : NULL;
}
void ActorPowersInitializeForGame() {
    SGame & tSGame = SGameManager::GameGet();
    ActorPowerGlobals *& ptActorPowerGlobals = tSGame.ptActorPowerGlobals;
    ptActorPowerGlobals = (ActorPowerGlobals *)SigmaMemoryNew(sizeof(ActorPowerGlobals), 4, tSGame.ptMemoryInterface, TRUE);
    ptActorPowerGlobals->poolActorPowers.Initialize();
    ptActorPowerGlobals->poolActorPowers.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorPowerGlobals->poolActorPowers.SetBlockCount_Tracked(1000);
    ptActorPowerGlobals->poolGlobalPowers.Initialize();
    ptActorPowerGlobals->poolGlobalPowers.SetMemoryInterface(tSGame.ptMemoryInterface);
    ptActorPowerGlobals->poolGlobalPowers.SetBlockCount_Tracked(256);
    ptActorPowerGlobals->listGlobalPowers.Initialize(&ptActorPowerGlobals->poolGlobalPowers);
}
void ActorPowersDisposeForGame() {
    SGame & tSGame = SGameManager::GameGet();
    ActorPowerGlobals *& ptActorPowerGlobals = tSGame.ptActorPowerGlobals;
    ptActorPowerGlobals->listGlobalPowers.Dispose();
    ptActorPowerGlobals->poolGlobalPowers.Dispose();
    ptActorPowerGlobals->poolActorPowers.Dispose();
    SigmaMemoryFree(ptActorPowerGlobals, tSGame.ptMemoryInterface);
    ptActorPowerGlobals = NULL;
}
void ActorPowersInitialize(ActorID idActor) {
    ActorPowerGlobals * ptActorPowerGlobals = SGameManager::GameGet().ptActorPowerGlobals;
    Actor * ptActor = ActorGet(idActor);
    ptActor->ptActorPowers = ptActorPowerGlobals->poolActorPowers.Alloc_Tracked();
    ptActor->ptActorPowers->idPrimaryPower = POWERID_INVALID;
    ptActor->ptActorPowers->m_listPowers.SetPool(MemoryPoolsGetDATAIDListPool());
    ptActor->ptActorPowers->idPowerThatOwnsMe = POWERID_INVALID;
    ptActor->ptActorPowers->eSpellEffectToRun = SPELL_NONE;
}
void ActorPowersDispose(Actor * ptActor) {
    ActorPowerGlobals * ptActorPowerGlobals = SGameManager::GameGet().ptActorPowerGlobals;
    if (ptActorPowerGlobals && ptActor->ptActorPowers) {
        if (ptActor->ptActorPowers->idPrimaryPower != POWERID_INVALID)
            PowerInterrupt(ptActor->ptActorPowers->idPrimaryPower, FALSE);
        ptActor->ptActorPowers->idPrimaryPower = POWERID_INVALID;
        PowerID idPowerThatOwnsMe = ptActor->ptActorPowers->idPowerThatOwnsMe;
        if (idPowerThatOwnsMe != POWERID_INVALID) {
            ActorPowersSetOwnerPower(ptActor->id, POWERID_INVALID);
            PowerCheckForDone(idPowerThatOwnsMe);
        }
        ptActorPowerGlobals->poolActorPowers.Free(ptActor->ptActorPowers);
        ptActor->ptActorPowers = NULL;
    }
}
void ActorPowersSetPrimary(ActorID idActor, PowerID idPower) {
    ActorPowers * ptActorPowers = sActorPowersGet(idActor);
    ptActorPowers->idPrimaryPower = idPower;
    Power * ptPower = PowerGet(idPower);
}
PowerID ActorPowersGetPrimary(ActorID idActor) { return sActorPowersGet(idActor)->idPrimaryPower; }
PowerID ActorPowersGetPrimary(Actor * ptActor) { return ptActor->ptActorPowers->idPrimaryPower; }
void ActorPowersRemovePrimary(ActorID idActor, PowerID idPower) {
    ActorPowers * ptActorPowers = sActorPowersTryToGet(idActor);
    if (ptActorPowers && ptActorPowers->idPrimaryPower == idPower)
        ptActorPowers->idPrimaryPower = POWERID_INVALID;
}
BOOL ActorPowersPrimaryIsNotInterruptable(ActorID idActor) {
    ActorPowers * ptActorPowers = sActorPowersGet(idActor);
    if (ptActorPowers->idPrimaryPower != POWERID_INVALID)
        return !PowerIsInterruptable(ptActorPowers->idPrimaryPower);
    return FALSE;
}
BOOL ActorPowersPrimaryIsInterruptableByWalking(ActorID idActor) {
    ActorPowers * ptActorPowers = sActorPowersGet(idActor);
    Power * ptPower = PowerTryToGet(ptActorPowers->idPrimaryPower);
    if (ptPower) return PowerIsCancellableByWalking(AutoPowerDefinitionR(ptPower->snoPower));
    return TRUE;
}
SNO ActorPowersGetPrimaryPower(ActorID idActor) {
    ActorPowers * ptActorPowers = sActorPowersTryToGet(idActor);
    if (ptActorPowers) {
        Power * ptCurrentPower = PowerTryToGet(ptActorPowers->idPrimaryPower);
        if (ptCurrentPower) return ptCurrentPower->snoPower;
    }
    return SNO_NONE;
}
void ActorPowersAdd(ActorID idActor, PowerID idPower) {
    ActorPowers * ptActorPowers = sActorPowersTryToGet(idActor);
    if (ptActorPowers) ptActorPowers->m_listPowers.AddTail_Tracked(idPower);
}
void ActorPowersRemove(ActorID idActor, PowerID idPower) {
    ActorPowers * ptActorPowers = sActorPowersTryToGet(idActor);
    if (!ptActorPowers) return;
    if (ptActorPowers->idPrimaryPower == idPower) ptActorPowers->idPrimaryPower = POWERID_INVALID;
    POS pos = ptActorPowers->m_listPowers.Find(idPower);
    if (pos) ptActorPowers->m_listPowers.RemoveAt(pos);
}
void ActorPowersDetachPrimary(ActorID idActor) {
    ActorPowers * ptActorPowers = sActorPowersGet(idActor);
    if (ptActorPowers->idPrimaryPower != POWERID_INVALID)
        PowerDetachFromActor(ptActorPowers->idPrimaryPower, FALSE);
}
void ActorPowersForceStopPrimary(ActorID idActor, BOOL bIgnoreContactFrame) {
    ActorPowers * ptActorPowers = sActorPowersGet(idActor);
    if (ptActorPowers->idPrimaryPower != POWERID_INVALID)
        PowerForceStop(ptActorPowers->idPrimaryPower, bIgnoreContactFrame);
}
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame) {
    ActorPowers * ptActorPowers = sActorPowersGet(idActor);
    if (ptActorPowers->idPrimaryPower != POWERID_INVALID)
        PowerInterrupt(ptActorPowers->idPrimaryPower, bIgnoreContactFrame);
}
void ActorPowersSetOwnerPower(ActorID idActor, PowerID idPower) {
    ACDID idACDActor = ACDFindByActor(idActor);
    ActorPowers * ptActorPowers = sActorPowersGet(idActor);
    if (ptActorPowers->idPowerThatOwnsMe != POWERID_INVALID) {
        Power * ptPower = PowerGet(ptActorPowers->idPowerThatOwnsMe);
        ptPower->tListSummonedActors.RemoveAt(ptPower->tListSummonedActors.Find(idACDActor));
    }
    ptActorPowers->idPowerThatOwnsMe = idPower;
    if (idPower != POWERID_INVALID) {
        Power * ptPower = PowerGet(idPower);
        ptPower->tListSummonedActors.AddTail_Tracked(idACDActor);
    }
}
PowerID ActorPowersGetOwnerPower(ActorID idActor) { return sActorPowersGet(idActor)->idPowerThatOwnsMe; }
void ActorPowersSetSpellEffect(ActorID idActor, SpellEffect eSpellEffectToRun) { sActorPowersGet(idActor)->eSpellEffectToRun = eSpellEffectToRun; }
SpellEffect ActorPowersGetSpellEffect(ActorID idActor) { return sActorPowersGet(idActor)->eSpellEffectToRun; }

void ActorPowersCancelForDeath(ActorID idActor) {
    ActorPowers * ptActorPowers = sActorPowersTryToGet(idActor);
    if (!ptActorPowers) return;
    POS pos = ptActorPowers->m_listPowers.GetHeadPosition();
    while (pos) {
        PowerID idPower = ptActorPowers->m_listPowers.GetNext(pos);
        SNO snoPower = PowerGetSNO(idPower);
        PowerTemplate nTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), ACDFindByActor(idActor));
        if (nTemplate == ONCREATE_POWER) PowerInterrupt(idPower, FALSE);
    }
}
void ActorPowersGrantGlobalPower(SNO snoPower, int32 nPlayerIndex) {
    SGame & tSGame = SGameManager::GameGet();
    if (snoPower == SNO_NONE) return;
    GlobalPower tGlobalPower = { snoPower };
    XPooledGlobalPowerList & listGlobalPowers = tSGame.ptActorPowerGlobals->listGlobalPowers;
    POS pos = listGlobalPowers.GetHeadPosition();
    while (pos) {
        const GlobalPower & tGlobalPowerCurr = listGlobalPowers.GetNext(pos);
        if (tGlobalPowerCurr == tGlobalPower) return;
    }
    listGlobalPowers.AddTail_Tracked(tGlobalPower);
    if (nPlayerIndex < 0) {
        for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer)) {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer) ptACDPlayer->AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), TRUE);
        }
    } else {
        Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer) ptACDPlayer->AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower), TRUE);
    }
}
void ActorPowersRemoveGlobalPower(SNO snoPower, int32 nPlayerIndex) {
    SGame & tSGame = SGameManager::GameGet();
    if (snoPower == SNO_NONE) return;
    GlobalPower tGlobalPower = { snoPower };
    XPooledGlobalPowerList & listGlobalPowers = tSGame.ptActorPowerGlobals->listGlobalPowers;
    POS posLast = NULL;
    POS pos = listGlobalPowers.GetHeadPosition();
    while (pos) {
        posLast = pos;
        const GlobalPower & tGlobalPowerCurr = listGlobalPowers.GetNext(pos);
        if (tGlobalPowerCurr == tGlobalPower) {
            listGlobalPowers.RemoveAt(posLast);
            if (nPlayerIndex < 0) {
                for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
                    SPlayerRemoveSkill(*ptPlayer, snoPower, -1);
            } else {
                Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
                SPlayerRemoveSkill(*ptPlayer, snoPower, -1);
            }
            return;
        }
    }
}

void ActorPowersGrantAllGlobalPowers(ActorCommonData & tACD) {
    SGame & tSGame = SGameManager::GameGet();
    XPooledGlobalPowerList & listGlobalPowers = tSGame.ptActorPowerGlobals->listGlobalPowers;
    POS pos = listGlobalPowers.GetHeadPosition();
    while (pos) {
        GlobalPower & tGlobalPower = listGlobalPowers.GetNext(pos);
        tACD.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, tGlobalPower.snoPower), TRUE);
    }
}

static int32 sSortBuffInstancesByStartTick(const void * pLHS, const void * pRHS) {
    const BuffID * ptLHSBuffID = (const BuffID *)pLHS;
    uint32 uLeftStartTick = 0;
    PowerAPIBuffInstance * ptBuffLeft = PowerAPI_BuffTryToGet(*ptLHSBuffID);
    if (ptBuffLeft) uLeftStartTick = ptBuffLeft->dwStartTick;
    uint32 uRightStartTick = 0;
    PowerAPIBuffInstance * ptBuffRight = PowerAPI_BuffTryToGet(*(const BuffID *)pRHS);
    if (ptBuffRight) uRightStartTick = ptBuffRight->dwStartTick;
    return uLeftStartTick < uRightStartTick ? -1 : 1;
}
void ActorPowersApplyBuffInstanceCap(ActorID idActor, SNO snoPower, int32 nBuffIndex, int32 nCap) {
    ActorPowers * ptActorPowers = sActorPowersTryToGet(idActor);
    if (!ptActorPowers) return;
    XPooledDataIDList listBuffsFound(MemoryPoolsGetDATAIDListPool());
    POS pos = ptActorPowers->m_listPowers.GetHeadPosition();
    while (pos) {
        PowerID idPower = ptActorPowers->m_listPowers.GetNext(pos);
        Power * ptPower = PowerTryToGet(idPower);
        if (!ptPower || ptPower->snoPower != snoPower) continue;
        POS posBuffs = ptPower->listBuffInstances.GetHeadPosition();
        while (posBuffs) {
            BuffID idBuff = ptPower->listBuffInstances.GetNext(posBuffs);
            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && ptBuff->nBuffIndex == nBuffIndex && !ptBuff->bDeleted)
                listBuffsFound.AddTail_Tracked(idBuff);
        }
    }
    if (listBuffsFound.Count() > nCap) {
        listBuffsFound.Sort(sSortBuffInstancesByStartTick);
        while (listBuffsFound.Count() > nCap) {
            BuffID idTail = listBuffsFound.GetTail();
            PowerAPI_BuffFlagForDeletion(idTail, FALSE, TRUE, BUFF_REMOVAL_REASON_DEFAULT, TRUE);
            listBuffsFound.RemoveTail();
        }
    }
}

void ActorPowersRemoveBuffsForRespec(ActorCommonData & tACD, const SNO snoPower) {
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(tACD.id));
    if (!ptActor) return;
    ActorPowers * ptActorPowers = sActorPowersTryToGet(ptActor->id);
    if (!ptActorPowers) return;
    POS pos = ptActorPowers->m_listPowers.GetHeadPosition();
    while (pos) {
        PowerID idPower = ptActorPowers->m_listPowers.GetNext(pos);
        Power * ptPower = PowerTryToGet(idPower);
        SNO snoSynergyPower = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_SYNERGY_POWER);
        if (ptPower && (ptPower->snoPower == snoPower || ptPower->snoPower == snoSynergyPower)) {
            POS posBuffs = ptPower->listBuffInstances.GetHeadPosition();
            while (posBuffs) {
                BuffID idBuff = ptPower->listBuffInstances.GetNext(posBuffs);
                PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                if (ptBuff && ptBuff->bRemoveOnRespec)
                    PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_RESPECCED, TRUE);
            }
        }
    }
}

void ActorPowersRemoveBuffsForDueling(ActorCommonData & tACD, const SNO snoPower) {
    Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(tACD.id));
    if (!ptActor) return;
    ActorPowers * ptActorPowers = sActorPowersTryToGet(ptActor->id);
    if (!ptActorPowers) return;
    POS pos = ptActorPowers->m_listPowers.GetHeadPosition();
    while (pos) {
        PowerID idPower = ptActorPowers->m_listPowers.GetNext(pos);
        Power * ptPower = PowerTryToGet(idPower);
        SNO snoSynergyPower = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_SYNERGY_POWER);
        if (ptPower && (ptPower->snoPower == snoPower || ptPower->snoPower == snoSynergyPower)) {
            POS posBuffs = ptPower->listBuffInstances.GetHeadPosition();
            while (posBuffs) {
                BuffID idBuff = ptPower->listBuffInstances.GetNext(posBuffs);
                PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
                if (ptBuff && ptBuff->bRemoveOnPassiveApplication)
                    PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, BUFF_REMOVAL_REASON_DUELING, TRUE);
            }
        }
    }
}
