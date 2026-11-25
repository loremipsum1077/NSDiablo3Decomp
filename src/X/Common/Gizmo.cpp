// Original file: X/Common/Gizmo.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Gizmo.h"
#include "GizmoSwig.h"
#include "ActorCommonData.h"
#include "Game.h"
#include "GlobalSNO.h"
#include "Player.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "Attributes/FastAttributeStructs.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/StringListDefinition.h"
#include "Definitions/ActDefinition.h"
#include "../Sigma/UnicodeText.h"
#include "SNO/SNOFiles.h"
#include "TagMaps/ActorTags.h"
#include "TagMaps/Tags.h"
#include "InactiveMarker.h"
#include "MarkerSet.h"
#include "Definitions/MarkerSetDefinition.h"
#include "Marker/Marker.h"
#include "Movement/ActorMovement.h"
#include "TagMaps/MarkerTags.h"
#include "../Client/CGame.h"
#include "../Server/Actors/Actors.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/RefString.h"

// Declared here from their definitions elsewhere.
FastAttribKey MakeAttribKey(Attrib eAttrib, int32 nParam);  // X/Common/Attributes/FastAttribute.cpp
ACDID LocalPlayerACDGet();  // X/Client/CPlayer.cpp:1686
BOOL InputIsControllerPresent();  // X/Client/Input/Input.cpp:1137
uint32 GetLoreChestUniqueHash(const ActorCommonData & tACD);  // X/Server/SGizmo.cpp:550
#include "Definitions/QuestRangeDefinition.h"
#include "Definitions/BossEncounterDefinition.h"
#include "Definitions/LevelAreaDefinition.h"
#include "Shell/CmdLine.h"
#include "../Server/SGame.h"
struct LooteeInfo;  // X/Common/Definitions/ConditionDefinition.h
BOOL ConditionEvaluate(SNO snoCondition, const Player * ptPlayer, ACDID idACD, const LooteeInfo * ptLooteeInfo);  // X/Common/Definitions/ConditionDefinition.cpp
BOOL PlayerCanUsePortal(const ActorCommonData & tACD);  // X/Common/Player.cpp
SNO ACDGetLevelArea(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp
int32 ActGetWaypointByLevelArea(SNO snoLevelArea, int32 nAct);  // X/Common/Act.cpp
BOOL ACDIsParticipatingInTieredLootRun(const ActorCommonData & tACD);  // X/Common/ActorCommonData.cpp
namespace NephalemRift {  // X/Common/NephalemRiftCommon.h
BOOL IsTieredRiftActive();
}
extern "C" blz::string sub_710051b1e0(SNO snoWorld);
int32 WorldGetDungeonFinderLevel(SNO snoWorld);  // X/Common/World.cpp
SNO WorldGetLevelAreaSNOFromRActor(RActorID idRActor);  // X/Client/CWorld.cpp
namespace BossEncounterManager {  // X/Server/System/BossEncounter.h
BOOL CanTriggerEncounter(SNO snoBossEncounter);
BOOL IsInProgress();
}
#include "Quests/Quests.h"





QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACD);  // X/Common/Quests/Quests.cpp
BOOL QuestIsComplete(const QuestInstance & tQuestInstance);  // X/Common/Quests/Quests.cpp

// X/Client/RActors/RActors.h:278 and :596. Only the members this file touches are declared.
struct RActor {  // size 0x410
    RActorID id;  // 0x00
    uint8 _unadopted0[0x3a0 - 0x04];  // 0x04 (invented name: idSScene .. )
    InactiveMarkerOwner tMarkerOwner;  // 0x3a0
    uint8 _unadopted1[0x410 - 0x3ac];  // 0x3ac (invented name: .. end)
};
static_assert(sizeof(RActor) == 0x410, "RActor");

inline RActor * RActorTryToGet(const RActorID idRActor) {  // line 596
    if (idRActor == RACTORID_INVALID)
        return NULL;
    CGame & tCGame = CGameGet();
    if (!tCGame.ptRActors)
        return NULL;
    return tCGame.ptRActors->ElementTryToGet(idRActor);
}



















GizmoType ActorCommonData::GetGizmoType() const
{


    return eGizmoType;
}






















BOOL ActorCommonData::IsWreckableObject() const
{

    return GizmoIsWreckable(GetGizmoType());
}


































BOOL ActorCommonData::IsChest() const
{

    GizmoType eType = GetGizmoType();
    return eType == ACTORGIZMO_CHEST || eType == ACTORGIZMO_LORE_CHEST;
}




















BOOL ActorCommonData::IsGizmo() const
{

    GizmoType nGizmoType = GetGizmoType();
    return nGizmoType != ACTORGIZMO_UNDEFINED;
}












































static BOOL sCanOperateBanner(const ActorCommonData & tACDGizmo, const ActorCommonData & tACDOperator)
{





    if (sgtAppGlobals.dwDebugFlags2 & 1)
        return TRUE;




    int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, tACDGizmo.snoActor);


    Player * ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);
    if (ptPlayerDest == NULL)
        return FALSE;





















    if (GameActiveGameCommonDataIsClient())
    {

        if (ptPlayerDest->tClientInfo.ann == ANN_INVALID)
            return FALSE;
    }
    else
    {





        if (ptPlayerDest->idPlayerACD == ACDID_INVALID)
            return FALSE;
    }
    return TRUE;
}

static BOOL sCanOperateLoreChest(const ActorCommonData & tACDGizmo, const ActorCommonData & tACDOperator)
{


    if (GizmoLockedByQuestRange(tACDGizmo))
    {
        return FALSE;
    }

    Player * ptPlayer = PlayerGetByACD(tACDOperator.id);
    if (ptPlayer == NULL)
    {
        return FALSE;
    }

    return tACDGizmo.AttributesGetInt(MakeAttribKey(ATTRIBUTE_CHEST_OPEN, ptPlayer->idGameConnection)) == 0;
}










BOOL ActorCommonData::IsOperatableGizmo(ACDID idACDOperator, BOOL bIgnoreCantStartDisplayedPowers) const
{




    if (!IsGizmo())
    {
        return FALSE;
    }

    if (IsNecroCorpse())
    {
        return FALSE;
    }

    GizmoType nGizmoType = GetGizmoType();

    if (GameActiveGameCommonDataIsClient())
    {
        if (idACDOperator == ACDID_INVALID)
        {
            idACDOperator = LocalPlayerACDGet();
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDOperator);
        if (ptACDPlayer == NULL ||
            !ACDGizmoCanOperate(*this, *ptACDPlayer, bIgnoreCantStartDisplayedPowers))
        {
            return FALSE;
        }

        if (nGizmoType == ACTORGIZMO_HEADSTONE)
        {
            return TRUE;
        }
    }

    switch (nGizmoType)
    {
    case ACTORGIZMO_BREAKABLECHEST:
    case ACTORGIZMO_BREAKABLEDOOR:




        return IsAlive();

    case ACTORGIZMO_CHEST:
        if (GizmoChestIsOpen(*this))
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_HEALING_WELL:
    case ACTORGIZMO_POWERUP:
    case ACTORGIZMO_POOL_OF_REFLECTION:
        if (AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_INACTIVE)
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_SWITCH:
        {



            ScopedActorDefinitionLockR pdefActor(snoActor);

            BOOL fCanOperateMultipleTimes = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_GIZMO_SWITCH_OPERATE_MULTIPLE);
            if (fCanOperateMultipleTimes)
                return TRUE;
            return !GizmoSwitchIsOn(*this);




        }

    case ACTORGIZMO_DOOR:

        if (GizmoDoorIsLocked(*this) ||
            (GizmoDoorIsOpen(*this) && !GizmoPlayerCanCloseDoor(*this)))
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_CHECKPOINT:
    case ACTORGIZMO_PORTAL_DESTINATION:
    case ACTORGIZMO_SPAWNER:
    case ACTORGIZMO_TRIGGER:
    case ACTORGIZMO_DESTROYABLE_OBJECT:
    case ACTORGIZMO_PRESSUREPLATE:
    case ACTORGIZMO_GATE:
    case ACTORGIZMO_DESTROY_ON_PROXIMITY:
    case ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT:
    case ACTORGIZMO_PLACED_LOOT:
    case ACTORGIZMO_SAVEPOINT:
        return FALSE;

    case ACTORGIZMO_SIGN:
    case ACTORGIZMO_ACT_TRANSITION_OBJECT:
    case ACTORGIZMO_BANNER:
    case ACTORGIZMO_LOOTRUN_SWITCH:
    case ACTORGIZMO_WEEKLYCHALLENGE_SWITCH:
        return TRUE;

    case ACTORGIZMO_HEADSTONE:
        {




            ActorCommonData * ptACDOperator = ACDTryToGet(idACDOperator);
            if (ptACDOperator == NULL)
            {
                break;
            }
        }
        return FALSE;

    case ACTORGIZMO_LORE_CHEST:
        {






            ActorCommonData * ptACDOperator = ACDTryToGet(idACDOperator);
            if (ptACDOperator == NULL)
                return TRUE;
            return sCanOperateLoreChest(*this, *ptACDOperator);
        }

    case ACTORGIZMO_OBJECTIVE_FINDER:



        if (GizmoObjectiveFinderIsUsed(*this))
        {
            return FALSE;
        }
        // fall through
    case ACTORGIZMO_MAILBOX:


        if (GameIsSeasonal())
        {
            return FALSE;
        }
        break;
    }


    SNO snoGizmoPower = ACDGizmoGetOperationPower(*this);
    return snoGizmoPower != SNO_NONE;
}













BOOL GizmoPlayerCanCloseDoor(const ActorCommonData & tACD)
{

    return ActorDefinitionGetTagMapValueInt(TAG_ACTOR_DOOR_PLAYER_CAN_CLOSE, tACD.snoActor);
}




BOOL ACDGizmoCanOperate(const ActorCommonData & tACDGizmo, const ActorCommonData & tACDOperator, BOOL bIgnoreCantStartDisplayedPowers)
{
    if (GizmoIsDisabled(tACDGizmo))
    {
        return FALSE;
    }

    if (!tACDGizmo.IsEnabled())
    {
        return FALSE;
    }

    if (GizmoHasContentRestrictions(tACDGizmo) &&
        tACDOperator.AttributesGetInt(ATTRIBUTE_IS_CONTENT_RESTRICTED_ACTOR))
    {
        return FALSE;
    }

    Player * ptPlayer = PlayerGetByACD(tACDOperator.id);
    ScopedActorDefinitionLockR pdefActor(tACDGizmo.GetActorSNO());
    SNO snoCondition = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_CONDITION);
    if (snoCondition != SNO_NONE)
    {
        if (!ConditionEvaluate(snoCondition, ptPlayer, tACDOperator.id, NULL))
        {
            return FALSE;
        }
    }

    if (!ACDIsFriendly(tACDGizmo, tACDOperator, FALSE))
    {
        BOOL bDisableNonFriendlyOperation = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_DISABLE_NONFRIENDLY_OPERATION, tACDGizmo.snoActor);
        if (bDisableNonFriendlyOperation)
        {
            return FALSE;
        }
    }

    if (!bIgnoreCantStartDisplayedPowers &&
        tACDOperator.AttributesGetInt(ATTRIBUTE_CANT_START_DISPLAYED_POWERS) &&
        !tACDOperator.AttributesGetInt(ATTRIBUTE_GIZMOS_IGNORE_CANT_START_DISPLAYED_POWERS))
    {
        if (!tACDOperator.AttributesGetInt(ATTRIBUTE_GHOSTED))
        {
            return FALSE;
        }
        GizmoType nGizmoType = tACDGizmo.GetGizmoType();
        if (nGizmoType != ACTORGIZMO_WAYPOINT && nGizmoType != ACTORGIZMO_PORTAL)
        {
            return FALSE;
        }
    }

    GizmoType nGizmoType = tACDGizmo.GetGizmoType();

    switch (nGizmoType)
    {
    case ACTORGIZMO_TOWN_PORTAL:
        if (tACDGizmo.ptResolvedDest == NULL)
        {
            return FALSE;
        }
        if (!PlayerCanUsePortal(tACDOperator))
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_HEARTH_PORTAL:
        if (!PlayerCanUsePortal(tACDOperator))
        {
            return FALSE;
        }
        if (GameActiveGameCommonDataIsClient())
        {
            return CGameGlobalsGet().snoLevelAreaHearthPortal != SNO_NONE;
        }
        if (ptPlayer == NULL || !ptPlayer->tHearthPortalInfo.IsValid())
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_PORTAL:
    case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
    case ACTORGIZMO_DUNGEON_PORTAL:
        if (!PlayerCanUsePortal(tACDOperator))
        {
            return FALSE;
        }
        if (tACDGizmo.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_INACTIVE)
        {
            return FALSE;
        }
        if (tACDGizmo.ptResolvedDest == NULL)
        {
            return FALSE;
        }
        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&
            LevelAreaIsTown(tACDGizmo.ptResolvedDest->snoDestLevelArea))
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_RETURN_PORTAL:
        if (!PlayerCanUsePortal(tACDOperator) || tACDGizmo.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_INACTIVE)
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_HEALING_WELL:
        if (tACDGizmo.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_INACTIVE)
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_SAVEPOINT_PORTAL:
        if (!PlayerCanUsePortal(tACDOperator))
        {
            return FALSE;
        }
        if (GameActiveGameCommonDataIsClient())
        {
            return CGameGlobalsGet().snoLevelAreaReturnPoint != SNO_NONE;
        }
        else
        {
            return SGameGlobalsGet().tReturnPoint.IsValid();
        }

    case ACTORGIZMO_BOSS_PORTAL:
        {
            SNO snoEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, tACDGizmo.snoActor);
            if (GameActiveGameCommonDataIsServer())
            {
                if (ACDIsParticipatingInTieredLootRun(tACDOperator))
                {
                    PlayerDisplayErrorMessage(tACDOperator.id, GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT);
                    return FALSE;
                }
                if (BossEncounterManager::CanTriggerEncounter(snoEncounter))
                {
                    break;
                }
                if (ptPlayer == NULL || (ptPlayer->dwBossEncounterFlags & 0x11))
                {
                    return FALSE;
                }
                if (BossEncounterManager::IsInProgress())
                {
                    PlayerDisplayErrorMessage(tACDOperator.id, GAMEERROR_CANNOT_ENTER_ACTIVE_BOSS_ENCOUNTER);
                }
                else
                {
                    PlayerDisplayErrorMessage(tACDOperator.id, GAMEERROR_CANNOT_ENTER_QUEUED_BOSS_ENCOUNTER);
                }
                return FALSE;
            }
            else
            {
                QuestRangeResult eResult;
                ScopedBossEncounterDefinitionLockR pdefBossEncounter(snoEncounter);
                if (!QuestRangeEvaluate(pdefBossEncounter->snoQuestRange, &eResult) && eResult == QRR_NOT_REACHED_YET)
                {
                    return pdefBossEncounter->bWorkAsPortalBeforeQuestRange;
                }
                return TRUE;
            }
        }

    case ACTORGIZMO_WAYPOINT:
        {
            SNO snoLevelArea = ACDGetLevelArea(tACDGizmo);
            int32 nWaypointNumber = ActGetWaypointByLevelArea(snoLevelArea, 0);
            Player * ptPlayerLoc = PlayerGetByACD(tACDOperator.id);
            if (ptPlayerLoc != NULL && !PlayerHasActivatedWaypoint(*ptPlayerLoc, nWaypointNumber))
            {
                return FALSE;
            }
        }
        break;

    case ACTORGIZMO_MAILBOX:
    case ACTORGIZMO_MULTICLICK:
        return TRUE;

    case ACTORGIZMO_HEADSTONE:
        return FALSE;

    case ACTORGIZMO_BANNER:
        if (!sCanOperateBanner(tACDGizmo, tACDOperator))
        {
            return FALSE;
        }
        break;

    case ACTORGIZMO_LORE_CHEST:
        return sCanOperateLoreChest(tACDGizmo, tACDOperator);

    case ACTORGIZMO_LOOTRUN_SWITCH:
        if (GameActiveGameCommonDataIsServer())
        {
            SNO snoQuest = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER);
            QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
            if (ptQuestInstance != NULL)
            {
                return ptQuestInstance->nCurrentStepUID == -1;
            }
        }
        break;

    case ACTORGIZMO_WEEKLYCHALLENGE_SWITCH:
        if (GameActiveGameCommonDataIsServer())
        {
            SNO snoQuest = GlobalSNOGet(GLOBALSNO_QUEST_WEEKLY_CHALLENGE);
            QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
            if (ptQuestInstance != NULL && ptQuestInstance->nCurrentStepUID != -1)
            {
                return QuestIsComplete(*ptQuestInstance) != FALSE;
            }
        }
        break;

    }

    return TRUE;
}

























































































GBID GizmoGetItemToUse(SNO snoActor)
{

    GizmoType eGizmoType = ActorGetGizmoType(snoActor);  // the original reads the type and never uses it

    return GBID_NONE;
}





BOOL GizmoIsWreckable(const GizmoType eGizmoType)
{

    switch (eGizmoType)
    {
    case ACTORGIZMO_BREAKABLECHEST:
    case ACTORGIZMO_DESTROYABLE_OBJECT:
    case ACTORGIZMO_BREAKABLEDOOR:
    case ACTORGIZMO_REFORMING_DESTROYABLE_OBJECT:
        return TRUE;
    }
    return FALSE;
}




BOOL GizmoIsDisabled(const ActorCommonData & tACD)
{

    int32 nDisabled = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_DISABLED_BY_SCRIPT);
    return nDisabled != 0;
}





BOOL GizmoChestIsOpen(const ActorCommonData & tACD)
{

    int32 nChestOpen = tACD.AttributesGetInt(ATTRIBUTE_CHEST_OPEN);

    return nChestOpen == 1;
}












BOOL GizmoSwitchIsOn(const ActorCommonData & tACD)
{

    return tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_INACTIVE;
}











BOOL GizmoDoorIsOpen(const ActorCommonData & tACD)
{

    int32 nDoorOpen = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);

    return nDoorOpen == GIZMOSTATE_DOOR_OPENED;
}











BOOL GizmoPowerUpIsInactive(const ActorCommonData & tACD)
{

    return tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_INACTIVE;
}











BOOL GizmoHealingWellIsEmpty(const ActorCommonData & tACD)
{

    return tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_EMPTY;
}











BOOL GizmoObjectiveFinderIsUsed(const ActorCommonData & tACD)
{

    return tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == GIZMOSTATE_INACTIVE;
}











BOOL GizmoLockedByQuestRange(const ActorCommonData & tACD)
{
    const InactiveMarkerOwner * ptMarkerOwner;
    SNO snoQuestRange;


    if (GameActiveGameCommonDataIsClient())
    {
        RActor * ptRActor = RActorTryToGet((RActorID)ACDGetOwner(tACD.id));
        if (ptRActor == NULL)
        {
            return FALSE;
        }
        ptMarkerOwner = &ptRActor->tMarkerOwner;
    }
    else
    {


        Actor * ptActor = ActorGet((ActorID)tACD.idOwner);
        ptMarkerOwner = &ptActor->tMarkerOwner;
    }



    if (ptMarkerOwner->snoMarkerSet == SNO_NONE)
    {
        return FALSE;
    }


    ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptMarkerOwner->snoMarkerSet);
    if (ptMarkerOwner->nMarkerIndex < 0 || ptMarkerOwner->nMarkerIndex >= ptMarkerSetDef->tMarkerSet.GetArraySize())
    {
        return FALSE;
    }



    const Marker & tMarker = ptMarkerSetDef->tMarkerSet[ptMarkerOwner->nMarkerIndex];

    snoQuestRange = TagMapGetSNO(tMarker.hTagMap, TAG_MARKER_UNLOCKED_QUESTRANGE);
    if (snoQuestRange == SNO_NONE)
    {
        return FALSE;
    }



    return !QuestRangeEvaluate(snoQuestRange, NULL);






}




BOOL GizmoDoorIsLocked(const ActorCommonData & tACD)
{

    int32 nDoorLocked = tACD.AttributesGetInt(ATTRIBUTE_DOOR_LOCKED);

    if (nDoorLocked == 1)
    {
        return TRUE;
    }

    BOOL bOperable = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_PLAYER_OPERABLE, tACD.snoActor);
    if (!bOperable)
    {
        return TRUE;
    }

    return GizmoLockedByQuestRange(tACD) != FALSE;
}







// LINKER-REMOVED: DWARF size 0x8 (two instructions, one row at 1055:17 = the AttributesGetInt call): the int is returned as is (tail call), no `!= 0`.
BOOL GizmoHasContentRestrictions(const ActorCommonData & tACD)
{


    return tACD.AttributesGetInt(ATTRIBUTE_IS_CONTENT_RESTRICTED_ACTOR);
}


SNO ACDGizmoGetOperationPower(const ActorCommonData & tACD)
{

    GizmoType eGizmoType = tACD.GetGizmoType();

    if (GizmoIsWreckable(eGizmoType))
    {
        return SNO_NONE;
    }

    if (tACD.IsNecroCorpse())
    {
        return SNO_NONE;
    }

    switch (eGizmoType)
    {
    case ACTORGIZMO_UNDEFINED:
        if (!tACD.IsNPC())
            return SNO_NONE;
        if (tACD.IsHelper())
        {
            return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_HELPER_ATTACH);
        }

        return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_NPC);
    case ACTORGIZMO_DOOR: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_CHEST: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_PORTAL: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_WAYPOINT: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_ITEM_INTERNAL: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_CHECKPOINT: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_SIGN: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_HEALING_WELL: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_POWERUP: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_TOWN_PORTAL: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_HEARTH_PORTAL: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    case ACTORGIZMO_HEADSTONE: return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    }

    return GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
}





uint32 ACDGizmoGetAnimation(const ActorCommonData & tACD)
{
    uint32 dwAnim = (uint32)-1;

    GizmoType nGizmoType = tACD.GetGizmoType();

    if (nGizmoType != ACTORGIZMO_ITEM_INTERNAL)
    {





        ScopedActorDefinitionLockR pdefActor(tACD.snoActor);
        dwAnim = TagMapGetDWORD(pdefActor->hTagMap, TAG_ACTOR_OPERATOR_ANIM);
    }

    return dwAnim;
}



GizmoType ActorGetGizmoType(SNO snoActor)
{
    GizmoType eGizmoType = ACTORGIZMO_UNDEFINED;


    if (snoActor != SNO_NONE)
    {
        eGizmoType = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_TYPE, snoActor);
    }


    return eGizmoType;
}



BOOL GizmoSupportsProximityDetection(const GizmoType eGizmoType)
{

    switch (eGizmoType)
    {
    case ACTORGIZMO_WAYPOINT:
    case ACTORGIZMO_CHECKPOINT:
    case ACTORGIZMO_SPAWNER:
    case ACTORGIZMO_TRIGGER:
    case ACTORGIZMO_PRESSUREPLATE:
    case ACTORGIZMO_DESTROY_ON_PROXIMITY:
    case ACTORGIZMO_SAVEPOINT:
    case ACTORGIZMO_DUNGEON_PORTAL:
    case ACTORGIZMO_RETURN_PORTAL:
        return TRUE;
    }

    return FALSE;
}



BOOL GizmoIsNonMoving(GizmoType eGizmoType)
{

    return eGizmoType != ACTORGIZMO_UNDEFINED;
}




BOOL GizmoGetUINameFromLabel(SNO snoActor, CRefString & usText)
{


    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS),

                           SNOToString(ACTOR_SNO, snoActor, FALSE),
                           usText);
}




BOOL GizmoGetUIName(const ActorCommonData & tACD, CRefString & usText)
{

    BOOL bFound = FALSE;

    BOOL bUseStringListName = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_USE_STRINGLIST_NAME, tACD.snoActor);

    if (!bUseStringListName)
    {
        GizmoType eGizmoType = tACD.GetGizmoType();

        switch (eGizmoType)
        {

        case ACTORGIZMO_HEADSTONE:
            {
                ACDID idACDVictim = ACDFromANN((ACDNetworkName)tACD.AttributesGetInt(ATTRIBUTE_HEADSTONE_PLAYER_ANN));
                ActorCommonData * ptACDVictim = ACDTryToGet(idACDVictim);
                if (ptACDVictim == NULL)
                {
                    return FALSE;
                }

                if (ptACDVictim->IsPet())
                {
                    usText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "HeadstoneHireling");
                    bFound = TRUE;
                }
            }
            break;

        case ACTORGIZMO_WAYPOINT:
            {
                RActor * ptRActor = RActorTryToGet((RActorID)ACDGetOwner(tACD.id));
                if (ptRActor)
                {
                    SNO snoLevelArea = WorldGetLevelAreaSNOFromRActor(ptRActor->id);
                    usText = LevelAreaGetLocalizedName(snoLevelArea);

                    usText += "\n";
                    usText += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "Waypoint");
                    bFound = TRUE;
                }
            }
            break;

        case ACTORGIZMO_SECRET_PORTAL:
            {
                int32 nGizmoState = tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE);
                if (nGizmoState == 0 || nGizmoState == 1)
                {

                    usText = StringListFetch("General:Passageway");
                    bFound = TRUE;
                }
            }
            break;

        case ACTORGIZMO_HEARTH_PORTAL:
            if (GameActiveGameCommonDataIsClient())
            {
                SNO snoLevelArea = CGameGlobalsGet().snoLevelAreaHearthPortal;

                if (!CGameGlobalsGet().bHearthPortalIsTieredRift)
                {
                    if (snoLevelArea != SNO_NONE)
                    {
                        CRefString sName = LevelAreaGetLocalizedName(snoLevelArea);
                        if (sName.Length())
                        {
                            usText = sName;
                            bFound = TRUE;
                        }
                    }
                }
                else
                {

                    if (NephalemRift::IsTieredRiftActive())
                    {
                        int32 nActiveRiftLevel = LevelAreaGetDungeonFinderLevel(snoLevelArea);
                        if (nActiveRiftLevel > 0)
                        {
                            usText = sub_710051b1e0(CGameGlobalsGet().unk_100).c_str();
                            bFound = TRUE;
                        }
                    }
                }
            }
            break;

        case ACTORGIZMO_PORTAL:
        case ACTORGIZMO_TOWN_PORTAL:
        case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
        case ACTORGIZMO_DUNGEON_PORTAL:
            {
                ResolvedPortalDestination * ptSpecifier = tACD.ptResolvedDest;
                if (ptSpecifier == NULL)
                {
                    return FALSE;
                }

                CRefString sName = LevelAreaGetLocalizedName(ptSpecifier->snoDestLevelArea);
                if (sName.Length())
                {
                    usText = sName;
                    bFound = TRUE;
                }

                if (NephalemRift::IsTieredRiftActive())
                {
                    int32 nActiveRiftLevel = WorldGetDungeonFinderLevel(ptSpecifier->snoWorld);
                    if (nActiveRiftLevel > 0)
                    {
                        usText = sub_710051b1e0(ptSpecifier->snoWorld).c_str();
                    }
                }
            }
            break;

        case ACTORGIZMO_RETURN_PORTAL:
            {
                CRefString sName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "ReturnPortal");
                if (sName.Length())
                {
                    usText = sName;
                    bFound = TRUE;
                }
            }
            break;

        case ACTORGIZMO_BOSS_PORTAL:
            {
                ResolvedPortalDestination * ptSpecifier = tACD.ptResolvedDest;
                if (ptSpecifier == NULL)
                {
                    return FALSE;
                }

                CRefString sName = LevelAreaGetLocalizedName(ptSpecifier->snoDestLevelArea);
                if (sName.Length())
                {
                    usText = sName;
                    bFound = TRUE;
                }

                SNO snoEncounter = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_GIZMO_BOSS_ENCOUNTER_SNO, tACD.snoActor);
                ScopedBossEncounterDefinitionLockR pdefEncounter(snoEncounter);
                if (QuestRangeEvaluate(pdefEncounter->snoQuestRange, NULL) && tACD.AttributesGetInt(ATTRIBUTE_GIZMO_STATE) == 0)
                {
                    usText = StringListFetch("H2OLayout:CantUsePortalBossEncounterInProgress", bFound);
                }
            }
            break;

        case ACTORGIZMO_SAVEPOINT_PORTAL:
            if (GameActiveGameCommonDataIsClient())
            {
                SNO snoLevelArea = CGameGlobalsGet().snoLevelAreaReturnPoint;
                if (snoLevelArea != SNO_NONE)
                {
                    CRefString sName = LevelAreaGetLocalizedName(snoLevelArea);
                    if (sName.Length())
                    {
                        usText = sName;
                        bFound = TRUE;
                    }
                }
            }
            break;

        case ACTORGIZMO_UNDEFINED:
            if (tACD.IsHelper() && tACD.GetPetOwnerPlayer() == NULL)
            {
                usText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), "Helper");
                bFound = TRUE;
            }
            break;
        }
    }

    if (!bFound)
    {
        return GizmoGetUINameFromLabel(tACD.snoActor, usText);
    }

    return bFound;
}
























static float sGetMovementRadius(const ActorCommonData & tACD)
{



    const ActorMovement * ptActorMovement = tACD.GetActorMovement();
    float flRadius;
    if (ptActorMovement != NULL)
    {
        WorldPlace tPlace;

        ptActorMovement->GetPlaceAndRadius(tPlace, flRadius, FALSE);
    }
    else
    {
        flRadius = tACD.GetRadius();
    }

    return flRadius;
}






void GizmoGetOperationRadius(const ActorCommonData & tACDGizmo, const ActorCommonData * ptACDOperator, float & flOperationRadius)
{



    GizmoType nGizmoType = tACDGizmo.GetGizmoType();

    if (nGizmoType == ACTORGIZMO_ITEM_INTERNAL)
    {
        flOperationRadius = 9.0f;
        return;
    }

    float flOverrideRadius = tACDGizmo.AttributesGetFloat(ATTRIBUTE_GIZMO_OPERATION_RADIUS_OVERRIDE);
    if (flOverrideRadius > 0.0f)
    {
        flOperationRadius = flOverrideRadius;
        return;
    }

    ScopedActorDefinitionLockR pdefActor(tACDGizmo.snoActor);
    flOperationRadius = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_OPERATION_RADIUS);
    BOOL bWalkToCenter = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_WALK_TO_CENTER);


    if (!bWalkToCenter)
    {
        float flGizmoRadius;
        if (ptACDOperator != NULL)
        {
            float flOperatorRadius = sGetMovementRadius(*ptACDOperator);
            flOperationRadius += flOperatorRadius;
        }
        flGizmoRadius = sGetMovementRadius(tACDGizmo);
        flOperationRadius += flGizmoRadius;


        flOperationRadius += 1.25f;
    }
    else
    {
        flOperationRadius = Math::Max(flOperationRadius, 1.25f);
    }




    if (InputIsControllerPresent())
    {
        flOperationRadius += 5.0f;
    }

}




BOOL GizmoShouldFaceToOperate(const ActorCommonData & tACDGizmo)
{

    GizmoType nGizmoType = tACDGizmo.GetGizmoType();

    switch (nGizmoType)
    {
    case ACTORGIZMO_PORTAL:
    case ACTORGIZMO_WAYPOINT:
    case ACTORGIZMO_ITEM_INTERNAL:
    case ACTORGIZMO_CHECKPOINT:
    case ACTORGIZMO_TOWN_PORTAL:
    case ACTORGIZMO_HEARTH_PORTAL:
    case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
    case ACTORGIZMO_BOSS_PORTAL:
    case ACTORGIZMO_SAVEPOINT:
    case ACTORGIZMO_SAVEPOINT_PORTAL:
    case ACTORGIZMO_DUNGEON_PORTAL:
    case ACTORGIZMO_RETURN_PORTAL:
        return FALSE;
    }



    return TRUE;
}


void LoreGizmoRestoreOpenState(ActorCommonData & tACD)
{


    uint32 nGizmoHash = GetLoreChestUniqueHash(tACD);
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        POS pos = ptPlayer->tLoresUsed.GetHeadPosition();
        while (pos != NULL)
        {
            uint32 nHash = ptPlayer->tLoresUsed.GetNext(pos);
            if (nHash == nGizmoHash)
            {



                FastAttribKey tKey = MakeAttribKey(ATTRIBUTE_CHEST_OPEN, ptPlayer->idGameConnection);
                tACD.AttributesSetInt(tKey, 1);
                break;
            }
        }
    }



}


BOOL GizmoIsPortal(GizmoType eGizmoType)
{

    switch (eGizmoType)
    {
    case ACTORGIZMO_PORTAL: case ACTORGIZMO_TOWN_PORTAL: case ACTORGIZMO_PAGE_OF_FATE_PORTAL:
    case ACTORGIZMO_BOSS_PORTAL: case ACTORGIZMO_DUNGEON_PORTAL: case ACTORGIZMO_RETURN_PORTAL:
        return TRUE;
    }
    return eGizmoType == ACTORGIZMO_SAVEPOINT_PORTAL;
}


BOOL GizmoUsesStaticMinimapPing(GizmoType eGizmoType)
{

    return eGizmoType == ACTORGIZMO_POWERUP;
}
