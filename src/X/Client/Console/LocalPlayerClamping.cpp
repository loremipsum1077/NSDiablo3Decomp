// Original file: X/Client/Console/LocalPlayerClamping.cpp

#include "LocalPlayerClamping.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Cinematography.h"
#include "../CGame.h"
#include "../CPlayer.h"
#include "../Input/Input.h"
#include "../Movement/CActorMovement.h"
#include "../RActors/RActors.h"
#include "../UserInterface/Console/ConsoleScreenManager.h"
#include "../UserInterface/Interface/UIConversations.h"
#include "../../Common/ActorCommonCollision.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/Game.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Nav/NavCellPath.h"
#include "../../Common/Nav/NavZone.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/GameTemplate.h"
#include "../../Common/Player.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Intersect.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Plane.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Vector.h"
#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include <float.h>

namespace LocalPlayerClamping {

enum eLocation {
    LOC_CENTER = 0,
    LOC_LEFT_BIT = 1,
    LOC_RIGHT_BIT = 2,
    LOC_TOP_BIT = 4,
    LOC_BOTTOM_BIT = 8,
};





struct PlayerState {  // size 0x48
    PlayerState();
    PlayerActivity m_eActivity;  // 0x00

    Vector2D m_tStick;  // 0x04
    WorldPlace m_tWorldPlace;  // 0x0c
    Vector2D m_tScreenPosition;  // 0x1c
    uint32 m_eLocation;  // 0x24
    Vector2D m_damping;  // 0x28
    Vector2D m_translation;  // 0x30
    Blizzard::Time::Millisecond m_tLastActiveTime;  // 0x38
    Blizzard::Time::Millisecond m_tDragStartTime;  // 0x3c
    Blizzard::Time::Millisecond m_tLastCastTime;  // 0x40
    BOOL m_bPlayerProvidingInput;  // 0x44
};
static_assert(sizeof(PlayerState) == 0x48, "PlayerState");






class Logic {  // size 0x3dc
public:
    Logic();
    Vector2D GetLStickPosition(int32 nControllerIndex) const;
    BOOL IsBeingDragged(int32 nControllerIndex) const;
    BOOL IsPlayerMovingOrCasting(int32 nControllerIndex) const;
    PlayerActivity GetPlayerState(int32 nControllerIndex) const;
    void Update(const InputGameState & tInputState);
    BOOL IsPointInsideActiveCamera(const WorldPlace & wpTarget);
    void ClearInactiveFlagForController(int32 nControllerIndex);
    void SetLastCastTime(int32 nControllerIndex, Blizzard::Time::Millisecond nTime);
    BOOL m_bDebugDraw;  // 0x00
    BOOL m_bDisableClamping;  // 0x04
private:
    PlayerState m_playerState[4];  // 0x08
    void ProcessPlayerInput(uint nControllerIndex, const InputPlayerState & tInputPlayerState);
    void UpdatePlayer(int32 nControllerIndex);
    void GenerateOutput(PlayerState & tPlayer);
    void GenerateActions(int32 nControllerIndex, PlayerState & tPlayer);
    BOOL ForceTeleportPlayer(PlayerState & tPlayer);
    void UpdateCameras();
    void UpdateCamera(Camera & tCamera, BOOL bActiveOnly);
    BOOL WorldToScreen(const Vector3D & world, Vector2D & screen, const Camera & tCamera, BOOL bClipToViewport) const;
    Rect2D m_tDampingBorder;  // 0x128
    Camera m_activeCamera;  // 0x138
    uint m_nNumberOfActivePlayers;  // 0x3d8
};
static_assert(sizeof(Logic) == 0x3dc, "Logic");

}  // namespace LocalPlayerClamping

void ConvertUICToPixelsDoNotSnap(const Rect2D & rectUIC, Rect2D & rectPixels);  // X/Client/UserInterface/UIUtils.cpp:335
void ACDCollGetSettings(SNO snoActor, ACDCollSettings & tCollSettings);  // X/Common/ActorCommonCollision.cpp:292
BOOL CinematographyIsOrbiting();  // X/Client/Camera/Cinematography.cpp:362
BOOL CPlayerConsoleTryWarp(Player * ptPlayer, const WorldPlace & tTargetPlace);  // X/Client/CPlayerInput.cpp:4778
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp:45

// X/Common/Networking/GameMessage.h (src's shared header is a subset; the full enum is in GameMessageFull.h)




namespace LocalPlayerClamping {































static Logic * s_ptLogic;

static const float s_flBorder = 30.0f;
static const float s_flBorderExtra = 100.0f;
static const float s_flDampingFactor = 1.0f / s_flBorder;  // DW_AT_const_value 0x3d088889; the expression is assumed

static const Blizzard::Time::Millisecond s_tDeactivationDelay = 5000;
static const Blizzard::Time::Millisecond s_tDraggableDelay = 1500;


static const Rect2D s_tDampingBorder(
    50.0f,
    s_flBorder + s_flBorderExtra,
    50.0f,
    50.0f);

// The DWARF records three file-static Rect2D constants here with no value (they fold into
// Logic::Logic and GenerateActions): s_tDampingBorder (line 154), s_tActiveDraggingBorder (160)
// and s_tInactiveDraggingBorder (166). Only sums survive in the code: the constructor adds
// 50+30 / 130+30 / -50-30 / -50-30 to the viewport edges, GenerateActions adds 130 / 30 (not
// active) or 30 (active) to the border. s_tDampingBorder above is written with those values and
// an assumed sign convention; the two dragging borders are not reproduced and GenerateActions
// uses s_flBorder / s_flBorderExtra directly.






static const float s_flScreenHeightForDistanceDragging = 500.0f;
static const float s_flMaxDistAheadOfCameraForTeleportSq = 10000.0f;
static const float s_flMaxDistAheadOfCameraForDragSq = 2500.0f;
static const float s_flForceDragAggressiveness = 0.25f;




void Init()
{
    
    s_ptLogic = new Logic();
}


void Dispose()
{

    delete s_ptLogic;
    s_ptLogic = NULL;
}

void Update()
{
    InputGameState tState;






    for (uint8 i = 0; i < 4; i++)
    {
        Player * ptPlayer = PlayerGetByControllerIndex(i);
        InputPlayerState & tPlayerState = tState.m_players[i];
        if (ptPlayer)
        {
            const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
            tPlayerState.m_stickDirection = tInputState.tControllerState[i].tCurrentFrame.vecLeftStick;

            tPlayerState.m_eActivity = PA_ACTIVE;

            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            WorldPlace tPlacePlayer = tACDPlayer.GetPlace();

            tPlayerState.m_tPlace = tPlacePlayer;
        }
        else
        {
            tPlayerState.m_eActivity = PA_NOT_CONNECTED;
        }
    }

    s_ptLogic->Update(tState);
}


Vector2D GetLStickPosition(int32 nControllerIndex)
{

    
    return s_ptLogic->GetLStickPosition(nControllerIndex);
}

BOOL IsBeingDragged(int32 nControllerIndex)
{

    
    return s_ptLogic->IsBeingDragged(nControllerIndex);
}


BOOL IsPlayerActive(int32 nControllerIndex)
{

    
    
    PlayerActivity ePlayerActivity = s_ptLogic->GetPlayerState(nControllerIndex);

    return ePlayerActivity == PA_ACTIVE;
}


BOOL IsPlayerInactive(int32 nControllerIndex)
{

    
    
    PlayerActivity ePlayerActivity = s_ptLogic->GetPlayerState(nControllerIndex);

    return ePlayerActivity == PA_INACTIVE;
}


BOOL IsPlayerMovingOrCasting(int32 nControllerIndex)
{

    
    
    
    return s_ptLogic->IsPlayerMovingOrCasting(nControllerIndex);
}



void ClearInactiveFlagForController(int32 nControllerIndex, ActivityReason eReason)
{





    if (eReason == AR_MOVEMENT && IsBeingDragged(nControllerIndex))
    {
        return;
    }
    else if (eReason == AR_ABILITY_USED)
    {
        s_ptLogic->SetLastCastTime(nControllerIndex, Blizzard::Time::Milliseconds());
    }

    s_ptLogic->ClearInactiveFlagForController(nControllerIndex);
}























BOOL IsPointInsideActiveCamera(const WorldPlace & wpTarget)
{

    return s_ptLogic->IsPointInsideActiveCamera(wpTarget);
}




PlayerState::PlayerState()
    : m_eActivity(PA_NOT_CONNECTED)
    , m_eLocation(LOC_CENTER)
    , m_tLastActiveTime(Blizzard::Time::Milliseconds())
    , m_tDragStartTime(0)
    , m_tLastCastTime(0)
    , m_bPlayerProvidingInput(FALSE)
{
}


Vector2D Logic::GetLStickPosition(int32 nControllerIndex) const
{

    
    
    const PlayerState & tPlayerState = m_playerState[nControllerIndex];
    return tPlayerState.m_tStick;
}
BOOL Logic::IsBeingDragged(int32 nControllerIndex) const
{

    
    return m_playerState[nControllerIndex].m_eActivity != PA_NOT_CONNECTED &&
        !m_playerState[nControllerIndex].m_bPlayerProvidingInput && (fabs(m_playerState[nControllerIndex].m_translation.x) > 0.000001f ||
        fabs(m_playerState[nControllerIndex].m_translation.y) > 0.000001f);
}



void Logic::ClearInactiveFlagForController(int32 nControllerIndex)
{

    
    if (m_playerState[nControllerIndex].m_eActivity != PA_NOT_CONNECTED)
    {
        m_playerState[nControllerIndex].m_eActivity = PA_ACTIVE;
        m_playerState[nControllerIndex].m_tLastActiveTime = Blizzard::Time::Milliseconds();
        m_playerState[nControllerIndex].m_tDragStartTime = 0;
    }
}



void Logic::SetLastCastTime(int32 nControllerIndex, Blizzard::Time::Millisecond nTime)
{

    
    m_playerState[nControllerIndex].m_tLastCastTime = nTime;
}



PlayerActivity Logic::GetPlayerState(int32 nControllerIndex) const
{

    
    
    const PlayerState & tPlayerState = m_playerState[nControllerIndex];
    return tPlayerState.m_eActivity;
}

BOOL Logic::IsPlayerMovingOrCasting(int32 nControllerIndex) const
{

    
    
    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
        if (ptRActor && ptRActor->ptCActorMovement)
        {
            if (ptRActor->ptCActorMovement->IsMoving() && (ptRActor->ptCActorMovement->m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
            {
                return TRUE;
            }
        }

        if (ptPlayer->tClientInfo.snoCurrentLoopingPower != SNO_NONE)
        {
            return TRUE;
        }

        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);



        if (ptACD && ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCED_MOVE)))
        {
            return TRUE;
        }
    }



    const PlayerState & tPlayerState = m_playerState[nControllerIndex];
    return tPlayerState.m_bPlayerProvidingInput ||
        (Blizzard::Time::Milliseconds() - tPlayerState.m_tLastCastTime) < 30;
}


void Logic::Update(const InputGameState & tInputState)
{

    m_nNumberOfActivePlayers = 0;

    for (uint32 i = 0; i < 4; i++)
    {
        ProcessPlayerInput(i, tInputState.m_players[i]);
    }

    UpdateCameras();

    if (m_bDisableClamping)
        return;
    for (uint32 i = 0; i < 4; i++)
    {
        UpdatePlayer(i);
    }

    if (m_nNumberOfActivePlayers > 0)
    {
        for (uint32 i = 0; i < 4; i++)
        {
            if (m_playerState[i].m_eActivity != PA_NOT_CONNECTED)
            {
                GenerateOutput(m_playerState[i]);
            }
        }
    }
















}


BOOL Logic::IsPointInsideActiveCamera(const WorldPlace & wpTarget)
{

    Vector2D tScreenPos;
    return WorldToScreen(wpTarget.wp, tScreenPos, m_activeCamera, TRUE);
}



static BOOL sShouldForceActive(const ActorCommonData & tACDPlayer)
{

    if (tACDPlayer.IsImmobilized())
    {
        return TRUE;
    }

    if (tACDPlayer.IsAttached())
    {
        return TRUE;
    }

    if (tACDPlayer.IsFeared())
    {
        return TRUE;
    }

    if (tACDPlayer.IsStunned())
    {
        return TRUE;
    }

    if (tACDPlayer.IsBlind())
    {
        return TRUE;
    }

    return FALSE;
}


void Logic::ProcessPlayerInput(uint nControllerIndex, const InputPlayerState & tInputPlayerState)
{


    
    PlayerState & tPlayerState = m_playerState[nControllerIndex];

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    ActorCommonData * ptACDPlayer = ptPlayer ? ACDTryToGet(ptPlayer->idPlayerACD) : NULL;

    if (tInputPlayerState.m_eActivity != PA_NOT_CONNECTED && tPlayerState.m_eActivity == PA_NOT_CONNECTED)
    {
        tPlayerState.m_eActivity = PA_ACTIVE;
    }
    else if (tInputPlayerState.m_eActivity == PA_NOT_CONNECTED)
    {
        tPlayerState.m_eActivity = PA_NOT_CONNECTED;
    }

    if (ptACDPlayer && !ptACDPlayer->IsAlive())
    {

        tPlayerState.m_eActivity = PA_INACTIVE;
        tPlayerState.m_bPlayerProvidingInput = FALSE;
    }
    else

    
    if (tPlayerState.m_eActivity != PA_NOT_CONNECTED)
    {
        if (GameIsPaused()
            || GameIsSoftPaused()
            || Console::Screens::IsScreenSetOpen()
            || (ptPlayer && UIConversation::ACDIsInConversation(ptPlayer->idPlayerACD, TRUE, FALSE)))
        {
            if (tPlayerState.m_eActivity == PA_ACTIVE)
            {
                tPlayerState.m_tLastActiveTime = Blizzard::Time::Milliseconds();
            }
            else if (tPlayerState.m_eActivity == PA_DRAGGABLE)
            {
                tPlayerState.m_tLastActiveTime = Blizzard::Time::Milliseconds() - s_tDraggableDelay;
            }
        }
        else
        {
            if (Blizzard::Time::Milliseconds() - tPlayerState.m_tLastActiveTime > s_tDeactivationDelay)
            {
                tPlayerState.m_eActivity = PA_INACTIVE;
            }
            else if (Blizzard::Time::Milliseconds() - tPlayerState.m_tLastActiveTime > s_tDraggableDelay)
            {
                tPlayerState.m_eActivity = PA_DRAGGABLE;
            }
            else
            {
                tPlayerState.m_eActivity = PA_ACTIVE;
            }

            if ((ptACDPlayer && sShouldForceActive(*ptACDPlayer)) || IsPlayerMovingOrCasting(nControllerIndex))
            {
                tPlayerState.m_eActivity = PA_ACTIVE;
                tPlayerState.m_tLastActiveTime = Blizzard::Time::Milliseconds();
            }
        }
    }



    if (tPlayerState.m_eActivity == PA_ACTIVE)
    {
        m_nNumberOfActivePlayers++;
    }

    if (tInputPlayerState.m_eActivity != PA_NOT_CONNECTED)
    {
        tPlayerState.m_tStick = tInputPlayerState.m_stickDirection;
        tPlayerState.m_tWorldPlace = tInputPlayerState.m_tPlace;
    }
}


Logic::Logic()
    : m_nNumberOfActivePlayers(0)
    , m_bDebugDraw(FALSE)
    , m_bDisableClamping(FALSE)
{
    Rect2D tViewportUIC;
    CGameGetSafeViewportUIC(tViewportUIC);
    Rect2D tViewport;
    ConvertUICToPixelsDoNotSnap(tViewportUIC, tViewport);

    m_tDampingBorder.left = tViewport.left + s_tDampingBorder.left + s_flBorder;
    m_tDampingBorder.right = tViewport.right - s_tDampingBorder.right - s_flBorder;
    m_tDampingBorder.top = tViewport.top + s_tDampingBorder.top + s_flBorder;
    m_tDampingBorder.bottom = tViewport.bottom - s_tDampingBorder.bottom - s_flBorder;
}


void Logic::GenerateOutput(PlayerState & tPlayer)
{

    tPlayer.m_bPlayerProvidingInput = fabs(tPlayer.m_tStick.x) > 0.2f || fabs(tPlayer.m_tStick.y) > 0.2f;

    if (tPlayer.m_eActivity == PA_ACTIVE)
    {
        if (tPlayer.m_eLocation & LOC_LEFT_BIT)
        {
            if (tPlayer.m_tStick.x < 0.0f)
            {
                tPlayer.m_tStick.x *= tPlayer.m_damping.x;
            }
        }
        else if (tPlayer.m_eLocation & LOC_RIGHT_BIT)
        {
            if (tPlayer.m_tStick.x > 0.0f)
            {
                tPlayer.m_tStick.x *= tPlayer.m_damping.x;
            }
        }




        if (tPlayer.m_eLocation & LOC_TOP_BIT)
        {
            if (tPlayer.m_tStick.y > 0.0f)
            {
                tPlayer.m_tStick.y *= tPlayer.m_damping.y;
            }
        }
        else if (tPlayer.m_eLocation & LOC_BOTTOM_BIT)
        {
            if (tPlayer.m_tStick.y < 0.0f)
            {
                tPlayer.m_tStick.y *= tPlayer.m_damping.y;
            }
        }
    }




    tPlayer.m_tStick.x += tPlayer.m_translation.x;
    tPlayer.m_tStick.y += tPlayer.m_translation.y;
}



BOOL ValidateLocationForPlayer(const Player * ptPlayer, const WorldPlace & tTargetPlace)
{

    
    BOOL bValid = FALSE;

    RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
    if (ptRActor)
    {

        PathPoint2DList listPathPoints;
        listPathPoints.SetPool(NavCellPathGetPointPool());

        ACDCollParams tACDCollParams;

        ACDCollSettings tCollSettings; ACDCollGetSettings(ptRActor->snoActor, tCollSettings);
        tACDCollParams.uObjectNoOverlapFlags = tCollSettings.uDynamicFlagsNoOverlap;

        PathResult eResult = ptRActor->ptCActorMovement->FindPath(tTargetPlace, listPathPoints, 1, 0, &tACDCollParams);

        if (eResult == PATH_RESULT_FOUND)
        {
            bValid = TRUE;
        }
        else if (eResult == PATH_RESULT_NOT_NECESSARY)
        {

            uint32 uStaticFlagsAtDest = NavCellGetStaticFlags(tTargetPlace);
            bValid = (uStaticFlagsAtDest & 1) != 0;
        }
    }

    return bValid;
}




BOOL Logic::ForceTeleportPlayer(PlayerState & tPlayer)
{

    if (tPlayer.m_eActivity == PA_INVALID_LOCATION)
    {
        return TRUE;
    }

    Vector2D tScreenPos;
    if (!WorldToScreen(tPlayer.m_tWorldPlace.wp, tScreenPos, m_activeCamera, TRUE))
    {
        return TRUE;
    }

    if (tScreenPos.y < s_flScreenHeightForDistanceDragging &&
        Vector3D::Distance2DSquared(m_activeCamera.cmState.wvLookAt, tPlayer.m_tWorldPlace.wp) > s_flMaxDistAheadOfCameraForTeleportSq)
    {
        return TRUE;
    }

    return FALSE;
}




void Logic::GenerateActions(int32 nControllerIndex, PlayerState & tPlayer)
{

    tPlayer.m_eLocation = LOC_CENTER;

    tPlayer.m_damping.x = 1.0f; tPlayer.m_damping.y = 1.0f;
    tPlayer.m_translation.x = 0.0f; tPlayer.m_translation.y = 0.0f;





    if (CinematographyIsOrbiting() || CGameInScriptedSequence())
    {
        return;
    }

    if (tPlayer.m_eActivity == PA_INACTIVE || tPlayer.m_eActivity == PA_DRAGGABLE)
    {
        Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
        if (ptPlayer)
        {
            ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);



            if (ptACD && ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCED_MOVE)))
            {
                tPlayer.m_eActivity = PA_ACTIVE;
            }
        }
    }




    Rect2D tMoveBorder = m_tDampingBorder;

    if (tPlayer.m_eActivity == PA_INACTIVE || tPlayer.m_eActivity == PA_DRAGGABLE)
    {
        tMoveBorder.left += s_flBorder + s_flBorderExtra;
        tMoveBorder.right -= s_flBorder + s_flBorderExtra;
        tMoveBorder.top += s_flBorder;
        tMoveBorder.bottom -= s_flBorder;

        if (tPlayer.m_tScreenPosition.y < tMoveBorder.top)
        {
            tPlayer.m_translation.y = -Math::Max((tMoveBorder.top - tPlayer.m_tScreenPosition.y) * s_flDampingFactor, 0.0f);
        }

        else
        if (tPlayer.m_tScreenPosition.y > tMoveBorder.bottom)
        {
            tPlayer.m_translation.y = Math::Max((tPlayer.m_tScreenPosition.y - tMoveBorder.bottom) * s_flDampingFactor, 0.0f);
        }
    }
    else
    {

        Rect2D tSlowBorder = tMoveBorder;

        tMoveBorder.left += s_flBorder;
        tMoveBorder.right -= s_flBorder;



        if (tPlayer.m_tScreenPosition.x < tSlowBorder.left)
        {
            tPlayer.m_eLocation |= LOC_LEFT_BIT;

            tPlayer.m_damping.x = Math::Max(1.0f - (tSlowBorder.left - tPlayer.m_tScreenPosition.x) * s_flDampingFactor, 0.0f);
        }


        else
        if (tPlayer.m_tScreenPosition.x > tSlowBorder.right)
        {
            tPlayer.m_damping.x = Math::Max(1.0f - (tPlayer.m_tScreenPosition.x - tSlowBorder.right) * s_flDampingFactor, 0.0f);
            tPlayer.m_eLocation |= LOC_RIGHT_BIT;
        }



        if (tPlayer.m_tScreenPosition.y < tSlowBorder.top)
        {
            tPlayer.m_damping.y = Math::Max(1.0f - (tSlowBorder.top - tPlayer.m_tScreenPosition.y) * s_flDampingFactor, 0.0f);
            tPlayer.m_eLocation |= LOC_TOP_BIT;
        }


        else
        if (tPlayer.m_tScreenPosition.y > tSlowBorder.bottom)
        {
            tPlayer.m_damping.y = Math::Max(1.0f - (tPlayer.m_tScreenPosition.y - tSlowBorder.bottom) * s_flDampingFactor, 0.0f);
            tPlayer.m_eLocation |= LOC_BOTTOM_BIT;
        }
    }



    if (tPlayer.m_tScreenPosition.x < tMoveBorder.left)
    {
        tPlayer.m_translation.x = Math::Max((tMoveBorder.left - tPlayer.m_tScreenPosition.x) * s_flDampingFactor, 0.0f);
    }

    else
    if (tPlayer.m_tScreenPosition.x > tMoveBorder.right)
    {
        tPlayer.m_translation.x = -Math::Max((tPlayer.m_tScreenPosition.x - tMoveBorder.right) * s_flDampingFactor, 0.0f);
    }



    if (tPlayer.m_tScreenPosition.y < s_flScreenHeightForDistanceDragging)
    {
        float flDistFromCentreSq = Vector3D::Distance2DSquared(m_activeCamera.cmState.wvLookAt, tPlayer.m_tWorldPlace.wp);
        if (flDistFromCentreSq > s_flMaxDistAheadOfCameraForDragSq)
        {
            tPlayer.m_translation.y = -Math::Max((flDistFromCentreSq - s_flMaxDistAheadOfCameraForDragSq) * s_flForceDragAggressiveness * s_flDampingFactor, 0.0f);
        }
    }


    if (tPlayer.m_eActivity == PA_INACTIVE || tPlayer.m_eActivity == PA_DRAGGABLE)
    {

        if (tPlayer.m_translation.x != 0.0f || tPlayer.m_translation.y != 0.0f)
        {
            if (tPlayer.m_tDragStartTime == 0)
            {
                tPlayer.m_tDragStartTime = Blizzard::Time::Milliseconds();
            }
        }
        else
        {
            tPlayer.m_tDragStartTime = 0;
        }
    }

    if ((tPlayer.m_eActivity == PA_ACTIVE && tPlayer.m_bPlayerProvidingInput)
        || PlayerGetLocalPlayerCount() == 1)
    {
        ClearInactiveFlagForController(nControllerIndex);
    }



    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {




            if (GameTypeIsPvP() || ptACDPlayer->IsDueling())
            {
                ClearInactiveFlagForController(nControllerIndex);
            }

            else
            if (ForceTeleportPlayer(tPlayer))
            {

                WorldPlace tTargetPlace = tPlayer.m_tWorldPlace;
                tTargetPlace.wp = m_activeCamera.cmState.wvLookAt;

                BOOL bValidLocation = ValidateLocationForPlayer(ptPlayer, tTargetPlace);

                if (!bValidLocation)
                {
                    BOOL bFoundBackupLocation = FALSE;
                    WorldPlace tBackupPlace;

                    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
                    {
                        if (ptPlayer->nPlayerIndex != ptOtherPlayer->nPlayerIndex
                            && PlayerIsLocal(ptOtherPlayer)
                            && IsPlayerActive(ptOtherPlayer->nControllerIndex))
                        {
                            bValidLocation = ValidateLocationForPlayer(ptOtherPlayer, tTargetPlace);

                            if (!bValidLocation && !bFoundBackupLocation)
                            {
                                const ActorCommonData * ptACD = ACDTryToGet(ptOtherPlayer->idPlayerACD);
                                if (ptACD)
                                {
                                    tBackupPlace = ptACD->GetPlace();
                                    bFoundBackupLocation = ValidateLocationForPlayer(ptOtherPlayer, tBackupPlace);
                                }
                            }
                        }

                        if (bValidLocation)
                        {
                            break;
                        }
                    }

                    if (!bValidLocation && bFoundBackupLocation)
                    {
                        tTargetPlace = tBackupPlace;
                        bValidLocation = TRUE;
                    }
                }

                if (bValidLocation && CPlayerConsoleTryWarp(ptPlayer, tTargetPlace))
                {

                    tPlayer.m_tDragStartTime = 0;
                    tPlayer.m_eActivity = PA_INACTIVE;
                }

                else
                if (!ptACDPlayer->IsAlive())
                {
                    tPlayer.m_tDragStartTime = 0;

                    int32 nOtherIndex = -1;

                    for (Player * ptOtherPlayer = PlayerGetFirstInGame(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextInGame(ptOtherPlayer))
                    {
                        if (ptPlayer->nPlayerIndex != ptOtherPlayer->nPlayerIndex
                            && PlayerIsLocal(ptOtherPlayer))
                        {
                            const ActorCommonData * ptACD = ACDTryToGet(ptOtherPlayer->idPlayerACD);
                            if (ptACD && ptACD->IsAlive())
                            {
                                nOtherIndex = ptOtherPlayer->nPlayerIndex;
                                break;
                            }
                        }
                    }


                    if (nOtherIndex != -1)
                    {
                        PlayerIndexMessage tMessage;
                        tMessage.nPlayerIndex = nOtherIndex;

                        MessageSendToServer(CMSG_WARP_DEAD_PLAYER, &tMessage, sizeof(tMessage), ptPlayer);
                    }
                }
            }
        }
    }
}





void Logic::UpdateCameras()
{
    UpdateCamera(m_activeCamera, TRUE);
}



BOOL Logic::WorldToScreen(const Vector3D & world, Vector2D & screen, const Camera & tCamera, BOOL bClipToViewport) const
{

    
    
    Vector3D wpScreen3D;

    BOOL bResult = CameraWorldToScreenPoint(tCamera, world, wpScreen3D);
    if (!bResult)
        return FALSE;
    if (bClipToViewport)
    {
        IRect2D rectViewportPixels;
        CGameGetLetterboxedViewportPixels(rectViewportPixels);
        if (wpScreen3D.x < rectViewportPixels.left ||
            wpScreen3D.x >= rectViewportPixels.right ||
            wpScreen3D.y < rectViewportPixels.top ||
            wpScreen3D.y >= rectViewportPixels.bottom)
        {
            return FALSE;
        }
    }
    screen.Set(wpScreen3D.x, wpScreen3D.y);
    return bResult;
}




void Logic::UpdateCamera(Camera & tCamera, BOOL bActiveOnly)
{

    
    Vector3D tWorldPos(0.0f, 0.0f, 0.0f);

    uint32 nPlayerCount = 0;

    Rect2D tRect(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);

    float flZ = 0.0f;

    for (uint i = 0; i < 4; i++)
    {
        PlayerState tPlayerState = m_playerState[i];

        if ((bActiveOnly && tPlayerState.m_eActivity != PA_ACTIVE) ||
            tPlayerState.m_eActivity == PA_NOT_CONNECTED)
        {
            continue;
        }

        tWorldPos = tPlayerState.m_tWorldPlace.wp;

        Vector2D screenPos;

        if (WorldPointToScreenPoint(tWorldPos, screenPos, -1))
        {



            if (tRect.left > screenPos.x)
            {
                tRect.left = screenPos.x;
            }

            if (tRect.right < screenPos.x)
            {
                tRect.right = screenPos.x;
            }

            if (tRect.top > screenPos.y)
            {
                tRect.top = screenPos.y;
            }

            if (tRect.bottom < screenPos.y)
            {
                tRect.bottom = screenPos.y;
            }

            flZ += tPlayerState.m_tWorldPlace.wp.z;
            nPlayerCount++;
        }
    }
    tCamera = CameraManagerGetCamera(-1);

    if (nPlayerCount > 0)
    {
        if (nPlayerCount > 1)
        {
            float flPlayerCount = (float)nPlayerCount;

            flZ /= flPlayerCount;

            LineSeg3D wline;
            IVector2D iVec((int32)tRect.Center().x, (int32)tRect.Center().y);
            ScreenPointToWorldLineSeg(iVec, wline, -1);

            Plane p;
            p.Set(Vector3D(0.0f, 0.0f, flZ), cgwvZAxis);

            Intersect::LineSeg3DAndPlane(wline, p, tWorldPos);
        }

        CameraState & tCameraState = tCamera.cmState;

        Vector3D wvCameraOffsetVector = tCameraState.wvCameraOffsetVector;
        Vector3D wvLookAtOffsetVector = tCameraState.wvLookAtOffsetVector;

        Vector3D wvDirectionVector = wvCameraOffsetVector;
        wvDirectionVector.Normalize();

        Vector3D wpLookFrom = tWorldPos + wvCameraOffsetVector + wvDirectionVector;
        Vector3D wpLookAt = tWorldPos + wvLookAtOffsetVector;

        tCameraState.transform.Set(wpLookFrom, wpLookAt, cgwvZAxis);


        tCameraState.wvLookAt = wpLookAt;
        tCameraState.wvLookFrom = wpLookFrom;


        CameraRebuild(tCamera);
    }
}


void Logic::UpdatePlayer(int32 nControllerIndex)
{

    
    
    PlayerState & tPlayerState = m_playerState[nControllerIndex];

    if (tPlayerState.m_eActivity != PA_NOT_CONNECTED)
    {
        BOOL bRes = WorldToScreen(tPlayerState.m_tWorldPlace.wp, tPlayerState.m_tScreenPosition, m_activeCamera, FALSE);

        if (!bRes)
        {
            tPlayerState.m_eActivity = PA_INVALID_LOCATION;
        }
        else if (tPlayerState.m_eActivity == PA_INVALID_LOCATION)
        {
            tPlayerState.m_eActivity = PA_INACTIVE;
        }

        GenerateActions(nControllerIndex, tPlayerState);
    }
}

}  // namespace LocalPlayerClamping
