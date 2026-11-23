// Original file: X/Client/CGameMode.cpp
#include "./CGame.h"
#include "./CPlayer.h"
#include "./CPlayerInput.h"
#include "./Input/Input.h"
#include "./RActors/RActors.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Definitions/MarkerSetDefinition.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/Marker/Marker.h"
#include "../Common/Player.h"
#include "../Common/TagMaps/MarkerTags.h"
#include "../Common/TagMaps/Tags.h"
#include "../Sigma/ErrorManager/ErrorManager.h"

class RWindow;
class DisplayList;
BOOL InputRegisterListener(const InputMsg eInputMsg, const InputListenerFunc pfunc, InputContext eContext, int32 nIndex);  // X/Client/Input/Input.cpp:217
void InputUnregisterListener(const InputMsg eInputMsg, const InputListenerFunc pfunc, InputContext eContext);  // X/Client/Input/Input.cpp:260
void CinematographyStop();  // X/Client/Camera/Cinematography.cpp:395
void CinematographyResetCameras();  // X/Client/Camera/Cinematography.cpp:404
void CinematographyFixedView(RActorID idTargetRActor);  // X/Client/Camera/Cinematography.cpp:439
void CinematographyPlayerWarped();  // X/Client/Camera/Cinematography.cpp:720
void GfxFogForceDisabled(const BOOL fDisabled);  // X/Client/Gfx/Gfx.cpp:2466
void RActorsCreateForWorld();  // X/Client/RActors/RActors.cpp:6704
void PlayersSetMainPlayerRActor(Player * ptPlayer, RActorID idRActor);  // X/Client/CPlayerInput.cpp:6776

//@ 74
void CGameModeDispose()
{

    GameCommonData & tGCD = CGameGetGameCommonData();
    switch (tGCD.GameModeGet())
    {
    case GAMEMODE_NONE:
    case GAMEMODE_PLAYING:
    case GAMEMODE_HOLODECK:
    case GAMEMODE_MENU:
        break;
    default:
        DisplayInternalError(NULL, "", 111, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

//@ 115
    tGCD.GameModeSet(GAMEMODE_NONE);
}

//@ 121
void CGameModeRenderUpdate(RWindow & tRWindow)
{



    GameCommonData & tGCD = CGameGetGameCommonData();
    switch (tGCD.GameModeGet())
    {
    case GAMEMODE_NONE:
    case GAMEMODE_PLAYING:
    case GAMEMODE_HOLODECK:
    case GAMEMODE_MENU:
        break;
    default:
        DisplayInternalError(NULL, "", 157, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}

//@ 172
void CGameModeDraw(DisplayList & dispList, const RenderLayer eRenderLayer)
{
}

//@ 190
void CGameModeResetCameraToFollowPlayer()
{

    RActorID idPlayerRActor = RACTORID_INVALID;
    GameCommonData & tGCD = CGameGetGameCommonData();


    if (tGCD.GameModeGet() == GAMEMODE_PLAYING)
    {
        idPlayerRActor = LocalPlayerRActorGet();
    }


    if (idPlayerRActor == RACTORID_INVALID)
    {
        CGame & tCGame = CGameGet();
        TDataArrayIterator<RActor> tIt;
        while (RActor * ptRActor = tCGame.ptRActors->Iterate(tIt))
        {


            if (ptRActor->idMarker != -1)
            {
                const MarkerInstance & tMarkerInstance = MarkerInstanceGet(tGCD, ptRActor->idMarker);
                ScopedMarkerSetDefinitionLockR pdefMarkerSet(tMarkerInstance.snoMarkerSet);
                const Marker & tMarker = pdefMarkerSet->tMarkerSet[tMarkerInstance.nMarkerIndex];
                uint32 uStartingLocationGUID = TagMapGetDWORD(tMarker.hTagMap, TAG_MARKER_PORTAL_DEST_ENTRANCE_NUMBER);

                if (uStartingLocationGUID != (uint32)-1)
                {
                    idPlayerRActor = ptRActor->id;
                    break;
                }
            }
        }
    }

//@ 228
    if (idPlayerRActor == RACTORID_INVALID)
    {
        return;
    }


    CinematographyFixedView(idPlayerRActor);
    CinematographyResetCameras();

    if (GameModeIsAnEditMode(CGameGetGameCommonData().GameModeGet()))
    {

        CinematographyStop();
    }


    CinematographyPlayerWarped();
}

//@ 292
static BOOL sCommonInputHandler(const BufferedInput & tInput)
{

    GameCommonData & tGCD = CGameGetGameCommonData();
    switch (tGCD.GameModeGet())
    {
    default:
        break;
    }
    return FALSE;
}

//@ 440
static BOOL sGameModeActions(const BufferedInput & tInput)
{

    if (GameIsStartUp())
    {
        return FALSE;
    }

    if (sCommonInputHandler(tInput))
    {
        return TRUE;
    }


//@ 455
    GameCommonData & tGCD = CGameGetGameCommonData();
    switch (tGCD.GameModeGet())
    {
    case GAMEMODE_NONE:
        break;
    case GAMEMODE_PLAYING:
    case GAMEMODE_MENU:
        return PlayingInput(tInput);
    default:
        DisplayInternalError(NULL, "", 478, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return FALSE;
}

//@ 487
void CGameModeInitializeForGame()
{


    InputRegisterListener(INPUTMSG_KEYDOWNONCE, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_KEYDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_KEYSTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_KEYUP, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_LEFTMOUSEDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_LEFTMOUSESTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_LEFTMOUSEDOUBLECLICK, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_LEFTMOUSEUP, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_MOUSEMOVE, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_RIGHTMOUSEDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_RIGHTMOUSEDOUBLECLICK, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_RIGHTMOUSEUP, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_MIDDLEMOUSEDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_MIDDLEMOUSEUP, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_MOUSEWHEEL, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_LEFTSTICK, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_RIGHTSTICK, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_LEFTTRIGGER, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_RIGHTTRIGGER, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONSTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONREPEAT, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_CONTROLLERBUTTONUP, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
    InputRegisterListener(INPUTMSG_GESTUREFLICK, sGameModeActions, INPUTCONTEXT_DEFAULT, -1);
}

//@ 523
void CGameModeDisposeForGame()
{


    InputUnregisterListener(INPUTMSG_KEYDOWNONCE, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_KEYDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_KEYSTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_KEYUP, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSEDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSESTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSEDOUBLECLICK, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTMOUSEUP, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MOUSEMOVE, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSEDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSESTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSEDOUBLECLICK, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTMOUSEUP, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MIDDLEMOUSEDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MIDDLEMOUSEUP, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_MOUSEWHEEL, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTSTICK, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTSTICK, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_LEFTTRIGGER, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_RIGHTTRIGGER, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONSTILLDOWN, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONREPEAT, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_CONTROLLERBUTTONUP, sGameModeActions, INPUTCONTEXT_DEFAULT);
    InputUnregisterListener(INPUTMSG_GESTUREFLICK, sGameModeActions, INPUTCONTEXT_DEFAULT);
}

//@ 559
static void sCGameModePlayGameInitialize()
{


    GfxFogForceDisabled(FALSE);
    RActorsCreateForWorld();

    Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer)
    {

        PlayersSetMainPlayerRActor(ptPlayer, (RActorID)ACDGetOwner(ptPlayer->idPlayerACD));
    }
}

//@ 687
void CGameModeInitialize(const GameMode eMode)
{


    GameSetDrawFlagsToDefaults();

    switch (eMode)
    {
    case GAMEMODE_NONE:
        GfxFogForceDisabled(TRUE);
        break;
    case GAMEMODE_PLAYING:
        sCGameModePlayGameInitialize();
        break;
    case GAMEMODE_HOLODECK:
    case GAMEMODE_MENU:
        break;
    default:
        DisplayInternalError(NULL, "", 730, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}
