// Original file: X/Client/CGizmo.cpp
#include "./CGame.h"
#include "./CActTransitions.h"
#include "./CPlayer.h"
#include "./Attachments.h"
#include "./Battlenet/Subsystems/ModalNotificationManager.h"
#include "./RActors/RActors.h"
#include "./RActors/RActorAttachments.h"
#include "./Sound/Sound.h"
#include "./UserInterface/Console/ConsoleModalNotificationManager.h"
#include "./UserInterface/Console/ConsoleScreenManager.h"
#include "./UserInterface/UIID.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/AttachmentDefinition.h"
#include "../Common/Definitions/LevelAreaDefinition.h"
#include "../Common/Definitions/StringListDefinition.h"
#include "../Common/GameCommonData.h"
#include "../Common/Gizmo.h"
#include "../Common/GizmoSwig.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/TagMaps/ActorTags.h"
#include "../Sigma/RefString.h"
#include "../Sigma/UnicodeText.h"
#include "./Gfx/GfxVB.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"

SSceneID RActorGetSSceneID(const RActorID idRActor);  // X/Client/RActors/RActors.cpp:11585
int32 RActorGetCurrentWaypoint(const RActorID idRActor);  // X/Client/RActors/RActors.cpp:11636
int32 ActIndexFromActEnum(const Act eAct);  // X/Common/GameCommonData.cpp:395
Act ActEnumFromActIndex(const int32 nIndex);  // X/Common/GameCommonData.cpp:380
BOOL SNOPackChunkLoadAct(Act eAct);  // X/Common/SNO/SNOFiles.cpp:11554
float SNOPackChunkGetActLoadPercentComplete(Act eAct);  // X/Common/SNO/SNOFiles.cpp:11409
void SNOPackChunkRequestLoad(Act eAct);  // X/Common/SNO/SNOFiles.cpp:11435
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
CRefString PlayerGetHeroDisplayName(const Player & tPlayer, BOOL fDisplayPound);  // X/Common/Player.cpp:663
namespace Console { namespace UIOnlineActions { BOOL IsLocalJoinInProgress(); } }  // X/Client/UserInterface/Console/OnlineActions.cpp:1487
namespace UIWaypointMap { void Open(Player * ptPlayer, RActorID idWaypoint, int32 nWaypoint, BOOL bTutorial); }

//@ 38
static BOOL sGizmoWaypointIsActivated(RActorID idRActor)
{

    const Player * ptPlayer = LocalPlayerGet();
    SSceneID idSScene = RActorGetSSceneID(idRActor);

    if (idSScene == -1)
    {
        return FALSE;
    }


    int32 nWaypointNumber = RActorGetCurrentWaypoint(idRActor);

    if (nWaypointNumber == -1)
    {
        return FALSE;
    }


    return PlayerHasActivatedWaypoint(*ptPlayer, nWaypointNumber);
}

//@ 64
void GizmoActivateWaypointVisual(RActorID idRActor)
{
    if (idRActor == RACTORID_INVALID)
    {
        return;
    }

    BOOL bActivated = sGizmoWaypointIsActivated(idRActor);
    if (!bActivated)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_WAYPOINTS_DISCOVER), cgtPSODefaults);


        RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_DAMAGE_STATE_1, 0.0f, (Shape::Material)0), MSGPASS_TO_ACTORCHAIN);
    }
}

//@ 85
static ACDID sgidCachedACDGizmo = ACDID_INVALID;
static int32 sgnPlayerIndex = -1;

//@ 88
void sOperateGizmo(const UIID &)
{

    ActorCommonData * ptACDGizmo = ACDTryToGet(sgidCachedACDGizmo);
    if (ptACDGizmo)
    {
        Player * ptPlayer = PlayerGetByPlayerIndex(sgnPlayerIndex);


        if (ptACDGizmo->GetGizmoType() == ACTORGIZMO_ACT_TRANSITION_OBJECT)
        {
            Act eNextAct = ActEnumFromActIndex(ActIndexFromActEnum(GameGetCurrentAct()) + 1);
            if (eNextAct != -1 && !SNOPackChunkLoadAct(eNextAct))
            {
                NotificationCreationParams tParams;
                tParams.uszMessage = LocalizedSprintf(
                    StringListFetch("ConsoleErrors:CannotProceedGenericContentNotReady").str(),
                    UnicodeText::FormatNumber(SNOPackChunkGetActLoadPercentComplete(eNextAct) * 100.0f, TRUE, FNS_NORMAL, 2, 2, (XLocale)0).str(),
                    NULL);


                SNOPackChunkRequestLoad(eNextAct);

                ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
                return;
            }
        }


        ANNDataMessage tMessage;
        tMessage.ann = ANNFromACD(ptACDGizmo->id);
        MessageSendToServer(0x9a, &tMessage, sizeof(tMessage), ptPlayer);
    }
}

//@ 126
extern "C" blz::string sub_710051b420();

void GizmoConfirmOperation(ACDID idACDGizmo, Player * ptPlayer)


{
    ActorCommonData * ptACD = ACDTryToGet(idACDGizmo);
    if (ptPlayer == NULL || ptACD == NULL)
    {
        return;
    }







//@ 141
    sgidCachedACDGizmo = idACDGizmo;
    sgnPlayerIndex = ptPlayer->nPlayerIndex;

    CRefString sMsg;
    GizmoType eGizmoType = ptACD->GetGizmoType();

    switch (eGizmoType)
    {
    case ACTORGIZMO_PORTAL:
        sMsg = StringListFetch("Gizmos:TimedDungeonPortalConfirm");
        break;
    case ACTORGIZMO_DUNGEON_PORTAL:
    case ACTORGIZMO_RETURN_PORTAL:
        sMsg = StringListFetch("Gizmos:DungeonPortalConfirm");
        break;
//@ 157
    case ACTORGIZMO_ACT_TRANSITION_OBJECT:
        {
            if (CActTransitionInProgress())
            {
                return;
            }
        }
        sMsg = StringListFetch("Gizmos:ActSwitchConfirm");
        break;

    case ACTORGIZMO_SAVEPOINT_PORTAL:
        sMsg = StringListFetch("Gizmos:SavePortalConfirm");
        break;

    case ACTORGIZMO_BANNER:
        {
            int32 nPlayerDest = ActorDefinitionGetTagMapValueInt(TAG_ACTOR_GIZMO_BANNER_PLAYER_INDEX, ptACD->snoActor);
            Player * ptPlayerDest = PlayerGetByPlayerIndex(nPlayerDest);
            if (ptPlayerDest == NULL)
            {
                return;
            }


            CRefString sPlayerName = PlayerGetHeroDisplayName(*ptPlayerDest, FALSE);

            CRefString sLocationName;
            SNO snoLevelAreaDest = CPlayerGetLevelArea(*ptPlayerDest);
            if (snoLevelAreaDest != SNO_NONE)
            {
                int32 nDungeonFinderLevel = LevelAreaGetDungeonFinderLevel(snoLevelAreaDest);
                if (nDungeonFinderLevel > 0)
                {
                    CRefString sVal = UnicodeText::FormatNumber(nDungeonFinderLevel, TRUE, FNS_NORMAL, (XLocale)0);
                    if (ACDIsInTieredLootRun(*ptACD))
                    {
                        sLocationName = LocalizedSprintf(sub_710051b420().c_str(), sVal.str(), NULL);
                    }
                    else
                    {
                        sLocationName = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_LEVEL_AREAS), "NephalemRift").str(), sVal.str(), NULL);
                    }
                }

                if (sLocationName.Length() == 0)
                {
                    sLocationName = LevelAreaGetLocalizedName(snoLevelAreaDest);
                }
            }

            sMsg = LocalizedSprintf(StringListFetch("ConsoleUI:BannerPortalWarning"), sPlayerName.str(), sLocationName.str(), NULL);
        }
        break;


//@ 212
    case ACTORGIZMO_LOOTRUN_SWITCH:
        sMsg = StringListFetch("Gizmos:LootRunSwitchConfirm");
        break;
    }




    if (!::Console::UIOnlineActions::IsLocalJoinInProgress())
    {




//@ 226
        NotificationCreationParams tParams;
        tParams.uszMessage = sMsg;
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sOperateGizmo>();
        tParams.nControllerIndex = ptPlayer->nControllerIndex;

        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
}

//@ 266
void GizmoOperateWaypoint(RActorID idRActor, const int32 nControllerIndex)


{


    if (idRActor == RACTORID_INVALID)
    {
        return;
    }


    if (sGizmoWaypointIsActivated(idRActor))
    {




//@ 284
        UIWaypointMap::Open(PlayerGetByControllerIndex(nControllerIndex), idRActor, -1, FALSE);
    }
}

//@ 293
void GizmoWaypointRActorInitialize(RActorID idRActor)
{


    if (sGizmoWaypointIsActivated(idRActor))
    {
        RActorSendMsgKey(idRActor, KeyedMessage(MSGKEY_RESERVED_RECREATE_DAMAGE_STATE_1, 0.0f, (Shape::Material)0), MSGPASS_TO_ACTORCHAIN);
    }
}
