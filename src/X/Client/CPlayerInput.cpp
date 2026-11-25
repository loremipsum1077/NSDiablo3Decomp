// X/Client/CPlayerInput.cpp
// Functions sit on their original line numbers.
#include "Console/ControllerUtils.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"


#include "CPlayerInput.h"
#include "CGame.h"
#include "CPlayer.h"
#include "Particle.h"
#include "../Common/ActorCommonData.h"
#include "../Common/ActorInventory.h"
#include "../Common/AnimPlayer.h"
#include "../Common/TagMaps/AnimTagsSwig.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Common/CommonTypes.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Player.h"
#include "../Common/Target.h"
#include "../Common/Movement/ActorMovement.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/Random.h"
#include "RActors/RActors.h"
#include "RWindow.h"
#include "Definitions/PreferencesDefinition.h"
#include "../Common/Powers/PowersCommon.h"
#include "../Common/HotbarButton.h"
#include "../Common/GizmoSwig.h"
// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function.
#include "Movement/CActorMovement.h"
void MessageSendToServer(GameMessageType eType, void * pMessage, uint32 dwMessageSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
void ItemSetIsBeingManipulated(ActorCommonData & tACD, BOOL bManipulated, uint32 dwTimeout);  // X/Common/Items.cpp
BOOL ItemHasLabel(GBID gbidItem, int32 eLabel);  // X/Common/Items.cpp
BOOL ItemIsLegendary(GBID gbidItem);  // X/Common/Items.cpp
BOOL InputShiftKeyIsDown(InputContext eContext);  // X/Client/Input/Input.cpp
AnimReason AnimPlayerGetAnimReason(const ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp
void AnimPlayerPlayIdle(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp
void RActorPlayEffectFromMessage(RActorID idRActor, int32 eEffect, int32 nParam1, int32 nParam2);  // X/Client/RActors/RActorEffects.cpp
void CPlayerReset();  // X/Client/CPlayer.cpp:555
#include "CPlayerUtil.h"
#include "UserInterface/Interface/UIWaypointMapCommon.h"
// UIMap::IsOpen: raw CU 0285081c has only UIMap.cpp:366; no recovered header declaration.
namespace UIMap { BOOL IsOpen(); }
// ItemIsAutoPickup: raw CU 042faaab has only Items.cpp:4183; no recovered header declaration.
BOOL ItemIsAutoPickup(const ActorCommonData & tACDItem);
float PowerGetRange(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower, BOOL bIncludeModifiers);  // X/Common/Powers/PowersCommon.cpp
namespace LocalPlayerClamping { BOOL IsPointInsideActiveCamera(const WorldPlace & tPlace); }  // X/Client/LocalPlayerClamping.cpp
namespace Controller { int32 GetSlotFromButton(int32 eButton, const Player * ptPlayer); }  // X/Client/Input/Controller.cpp
float PowerGetFormulaValue(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower, uint32 dwPowerTag, int32 nParam0, int32 nParam1, uint32 dwFlags);  // X/Common/Powers/PowerFormulas.cpp
namespace Controller {  // X/Client/Input/Controller.cpp
BOOL InputIsTargetLock(const BufferedInput & tInput);
BOOL InputIsTogglePlayerIndicatorBeam(const BufferedInput & tInput);
}
namespace Targeting { namespace Console {  // X/Client/Targeting.cpp
void ResolveTargetForPlayer(const Player * ptPlayer, SNO snoPower, ResolvedClick & tResolvedClick, BOOL b0, BOOL b1, BOOL b2, BOOL b3, BOOL b4);
void TargetSetToGround(const Player * ptPlayer, Target & tTarget, const PowerDefinition * pdefPower);
} }
namespace Console { namespace ControllerUtils {  // X/Client/Console/ControllerUtils.cpp
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bClamped);
Vector2D GetRStickPosition(int32 nControllerIndex);
const Vector2D & GetLStickWorldDirection(int32 nControllerIndex);
const Vector2D & GetRStickWorldDirection(int32 nControllerIndex);
BOOL DidStickCrossThreshold(int32 nControllerIndex, BOOL bRightStick, float flThreshold);
Blizzard::Time::Millisecond GetHeldButtonTime(int32 nControllerIndex, InputButton eButton);
} }
BOOL CPlayerConsoleUseEvade(Player * ptPlayer, const WorldPlace & tTargetPlace, BOOL bQueued);
SNO PlayerGetPowerSNOFromBufferedInput(const Player & tPlayer, const BufferedInput & tInput);  // X/Common/Player.cpp
namespace Controller { BOOL InputIsPotion(const BufferedInput & tInput); }  // X/Client/Input/Controller.cpp
GameError ItemCanBeUsed(ACDID idACDUser, const ActorCommonData & tACDItem, const Target * ptTarget);  // X/Common/Items.cpp
void CACDInventoryRequestUse(ActorCommonData & tACDItem, ACDID idACDTarget, BOOL bFromHotbar, Player * ptPlayer);  // X/Client/CACDInventory.cpp
BOOL CGameInScriptedSequence();  // X/Client/CGame.cpp
namespace UIGame { void DisplayError(const GameErrorData & tError, int32 nParam, const Player * ptPlayer); }  // X/Client/UserInterface/Interface/UIGame.cpp
BOOL CPlayerCanUsePowerOnTarget(const RActor * ptPlayerRActor, const SNO snoPower, const SNO snoOriginalPower, const Target & tActorTarget, const BOOL bIgnoreLOS, const BOOL fCanCachePower, const BOOL fRefreshCacheTime, BOOL & bCancelingExistingPower, const BOOL bRequireRangeModifiers);
BOOL CPlayerUsePowerOnTarget(const RActor * ptPlayerRActor, const SNO snoPower, const SNO snoOriginalPower, const ACDID idACDItemBeingUsed, const Target & tActorTarget, const BOOL bCancelingExistingPower, const SNO snoData0, const int32 nData1, const BOOL bNotifyActivate);
#include "Camera/CameraManager.h"
BOOL PowerExtendTargetToPowerAttackRadius(ACDID idACD, AutoPowerDefinitionR pdefPower, WorldPlace & tPlace);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerPlayerAttackOnlyOncePerClick(AutoPowerDefinitionR pdefPower);  // X/Common/Powers/PowersCommon.cpp
void CPlayerInputClearHeldButton(const BufferedInput & tInput);  // defined below
BOOL PowerUsesTargetingReticleWhenHeld(AutoPowerDefinitionR pdefPower, const Player * ptPlayer);  // X/Common/Powers/PowersCommon.cpp

BOOL PowerFiresOnButtonUp(AutoPowerDefinitionR pdefPower, const Player * ptPlayer);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerUseAtMaxRangeWhenHeld(AutoPowerDefinitionR pdefPower, const Player * ptPlayer);  // X/Common/Powers/PowersCommon.cpp

SNO PowerPlayerFallbackIfNecessary(AutoPowerDefinitionR pdefPower, ACDID idACD, GameError & eError, BOOL bCurrentlyChannelingPower);  // X/Common/Powers/PowersCommon.cpp
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower);  // X/Common/Powers/PowersCommon.cpp
const HotbarButtonData & PlayerGetHotbarBtnAssignment(const Player * ptPlayer, SkillButton eSkillButton);  // X/Common/Player.cpp
void PlayerSetPotionBtnAssignment(Player * ptPlayer, const HotbarButtonData & tButtonData);  // X/Common/Player.cpp
void CinematographyStop();  // X/Client/Camera/Cinematography.cpp
void CinematographyFixedView(RActorID idRActor);  // X/Client/Camera/Cinematography.cpp
void CinematographySetYaw(float flYaw, BOOL bImmediate, float flTime);  // X/Client/Camera/Cinematography.cpp
void CinematographyUpdate(float flDeltaTime, BOOL bForce);  // X/Client/Camera/Cinematography.cpp
namespace Weather { void AddPlayerLights(WeatherRWindowData & tData, RActorID idRActor); }  // X/Client/Weather.cpp
namespace UIGame { void InitializeForPlayer(); }  // X/Client/UserInterface/Interface/UIGame.cpp
namespace UIHUD { namespace Console { void InitializeForPlayer(const Player * ptPlayer); } }  // X/Client/UserInterface/Console/UIHUDConsole.cpp
namespace UISkills { namespace Console { void InitializeForPlayer(int32 nPlayerIndex); } }  // X/Client/UserInterface/Console/UISkillsConsole.cpp
namespace UIParagonBonuses { namespace Console { void InitializeForPlayer(int32 nPlayerIndex); } }  // X/Client/UserInterface/Console/UIParagonBonusesConsole.cpp





// GameMessageType values this file sends (original enumerators).







typedef int32 PickupResponse;  // X/Common/Items.h
enum { PICKUPRESPONSE_INVENTORY_FULL = 3, PICKUPRESPONSE_NOT_YOURS = 4, PICKUPRESPONSE_ITEM_LIMIT = 5 };  // invented names
void CACDAttributeSetValue(ActorCommonData & tACD, FastAttribKey tKey, FastAttribValue tValue, BOOL bForce);  // X/Client/CACDAttributes.cpp






enum {
    MIN_TICKS_PER_HELD_EVENT_WHEN_MOUSE_MOVES = 2,
    MIN_TICKS_PER_HELD_EVENT = 6,
    MIN_MOUSE_DISTANCE_SQUARED = 9,
    HELD_ACTION_REPEAT_INTERVAL = 6,
};









static const UIAction arButtonActions[6] = {
    ACTION_UI_LEFT_MOUSE_BUTTON,
    ACTION_UI_RIGHT_MOUSE_BUTTON,
    ACTION_UI_SKILL_1,
    ACTION_UI_SKILL_2,
    ACTION_UI_SKILL_3,
    ACTION_UI_SKILL_4,
};
static const int32 numButtonActions = sizeof(arButtonActions) / sizeof(arButtonActions[0]);
#include "../Sigma/Math/Intersect.h"
BOOL PlayerCanChangeSkillorTrait(const ActorCommonData & tACDPlayer, BOOL bForHotbarSwapOnly); // X/Common/Player.cpp:4028
void CPlayerUtilResolveClick(const Player * ptPlayer, const SNO snoPowerOnButton, const BOOL fResolvingForHighlight, ResolvedClick & tResolvedClick); // X/Client/CPlayerUtil.cpp:756
void PlayerSendSkillChangeToServer(const SkillButton eSkillButton, const HotbarButtonData & tButtonData, Player * ptPlayer); // definition below at 6894
void CPlayerConsoleTryEvade(Player * ptPlayer); // definition below
void CPlayerConsoleTryGestureEvade(Player * ptPlayer); // definition below
#include "../Common/TagMaps/PowerTags.h"
#include "UserInterface/Interface/UIConversations.h"
static void sPlayerRememberButtonClickForHeldInput(Player *, const BufferedInput &, BOOL);
// Raw cached DWARF records only Input.cpp:1137; no header declaration was found.
BOOL InputIsControllerPresent();
// Raw cached DWARF records only CPlayerUtil.cpp:901; no header declaration was found.
RActorID CPlayerUtilChooseBestAutoTarget(const Player * ptPlayer, SNO snoPower, float flSearchRadius, Angle aSearchAngle, float flCloseRange, const Vector2D * pwpFacing);
static void sPlayerButtonUp(Player *, UIAction);
static BOOL sPlayerHeldActionIsReady(const Player *, UIAction);
static BOOL sPlayerCanProcessHeldActionImmediately(Player *, UIAction);
#include "UserInterface/Interface/UIGame.h"
#include "../Common/NephalemRiftCommon.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/TagMaps/PowerTags.h"
// DWARF: PowersCommon.cpp:2185,5708,1047,1061,1077; UIConsoleHUD.cpp:2907.
BOOL PowerPlayerCanPayChannelCost(const ActorCommonData & tACDPlayer, AutoPowerDefinitionR pdefPower);
void GameErrorFromPowerFailureReasons(GameErrorData & tData, const uint32 dwFailureReason, const ACDID idACD, AutoPowerDefinitionR pdefPower, const BOOL fItemPower);
BOOL PowerIgnoresRangeOnShiftClick(AutoPowerDefinitionR pdefPower);
BOOL PowerClipsTargetToAttackRadius(AutoPowerDefinitionR pdefPower);
BOOL PowerChecksPathAndCliptoPathableAttackRadius(SNO snoPower);
namespace UIHUD { namespace Console { void ReportPowerOnCooldownError(int32 nPlayerIndex, SNO snoPower); } }
static BOOL sPlayerCanInterruptHimselfAllowPowerCanceling(Player * ptPlayer, SNO snoPower);
SNO InputChooseFallbackPower(SNO snoPower, const Player * ptPlayer, GameError * peGameError);
#include "../Common/Definitions/StringListDefinition.h"
namespace Console { namespace UIOnlineActions { BOOL CanLocalPlayerJoin(); void LocalPlayerJoin(int32 nControllerIndex, BOOL bForceChooseHero); } } // OnlineActions.cpp:1496,1413
namespace Console { namespace Online { BOOL PartyIsInParty(int32 nControllerIndex); } } // Online.cpp:3987
#include "CWorld.h"
#include "Camera/Cinematography.h"
#include "Console/LocalPlayerClamping.h"
// Raw cached DWARF records only Cinematography.cpp:525; no header declaration was found.
CinematographyState CinematographyGetState();
// Raw cached DWARF records only RActorAnimation.cpp:1834; no header declaration was found.
uint32 RActorAnimationGetWalkAnimTag(const RActor & tRActor, SNO snoPower, float flSpeedLimit);
// Raw cached DWARF records only UI.cpp:6296; no header declaration was found.
void UI_ResetForPlayerMovement(Player * ptPlayer);
// Raw cached DWARF records only LocalPlayerClamping.cpp:278; no header declaration was found.
namespace LocalPlayerClamping { void ClearInactiveFlagForController(int32 nControllerIndex, ActivityReason eReason); }
void UI_SetCapture(const UIID & idCapture); // X/Client/UserInterface/UI.cpp
#include "RActors/RActorAnimation.h"
// Raw cached DWARF records these APIs only in RActorAnimation.cpp:1962, :1879, :2605.
BOOL RActorAnimationIsAnimTreeEnabled(const RActor & tRActor);
BOOL RActorAnimationHasReachedInterruptFrame(const RActor & tRActor);
BOOL RActorAnimTreeLayerHasReachedInterruptFrame(const RActor & tRActor, AnimLayer eAnimLayer);
// Raw cached DWARF records only AnimPlayer.cpp:2018; no header declaration was found.
uint32 AnimPlayerGetAnimEndTime(ActorCommonData & tACD);
// Genuine same-file declaration: CPlayerInput.cpp:6767.
SNO CPlayerInputGetCurrentAnimPower(Player * ptPlayer);
#include "Selection.h"
#include "UserInterface/UI.h"
#include "UserInterface/Controls/UIItemTagControl.h"
#include "UserInterface/Controls/UIWindowControl.h"
#include "Gfx/Gfx.h"
#include "../Common/Items.h"
// Raw cached DWARF records only CPlayerUtil.cpp:271; no header declaration was found.
void CPlayerUtilGetTarget(const Player * ptPlayer, const RActorID idRActorHit, const IVector2D & ptScreenClick, const SNO snoPower, Target & tTarget, ResolvedClick * ptClick);
// Raw cached DWARF records only CActorInventory.cpp:320; no header declaration was found.
void CACDInventoryRequestUse(ActorCommonData & tACDItem, const Target & tTarget, BOOL bPlaySound, Player * ptPlayer);
// Raw cached DWARF records only Items.cpp:2434 and :2504; no header declarations were found.
SNO ItemGetUsePower(const GBID gbidItem);
BOOL ItemUseIsTargettedInInventory(const ItemUseType eItemUseType);
// DWARF: RActors.cpp:11544,11869; Cinematography.cpp:457; UI.cpp:5366; UIConsoleHUD.cpp:1640.
void RActorHighlight(RActor &, const BOOL); BOOL RActorIsNecroCorpse(const RActor *);
RActorID CinematographyGetFocus(); UIItemTag & UI_GetItemTagFromID(const UIID &);
namespace UIHUD { namespace Console { void UpdatePlayerTargetingIndicator(const Player *, const RActor *, BOOL); } }
static BOOL sPlayerCanInterruptHimself(Player *, BOOL); static BOOL sHandleTargetLock(const RActor *, const TargetLock &, BOOL, BOOL, BOOL &);
void CPlayerInputSetComboLevel(int32, Player *); BOOL CPlayerConsoleUseSkill(Player *, const ResolvedClick &, BOOL, BOOL);
IVector2D InputGetCursorPos(); // DWARF: Input.cpp:1055.
BOOL PowerLocksActorsWhileSweeping(AutoPowerDefinitionR pdefPower); // DWARF: PowersCommon.cpp:795.




struct CPlayerInputGlobals {  // size 0x60
    RActorIDListPooled listHighlightedRActors;  // 0x00
    CursorMgr tCursorMgr;  // 0x28
};

static BOOL sPlayerPowerIsChannelled(const Player * ptPlayer);
static BOOL sPlayerPowerIsClientWalk(const Player * ptPlayer);
static BOOL sAbortChannelledPower(Player * ptPlayer);
static void sSendClientWalkAbortToServer(Player & tPlayer);
BOOL CPlayerInputAbortClientWalkPower(Player * ptPlayer);
static void sPlayerButtonClick(const RActor *, UIAction, SNO, BOOL, BOOL);
static void sPlayerRememberButtonClickForHeldAction(Player *, UIAction, BOOL);
CursorMgr & PlayerGetCursorMgr();  // Raw DWARF has only this file's definition at 6603; no recovered header declaration.


void CPlayerInputInitializeForApp()
{
    CPlayerInputGlobals *& ptCPlayerInputGlobals = CGameGet().ptCPlayerInputGlobals;

    ptCPlayerInputGlobals = (CPlayerInputGlobals *)SigmaMemoryNew(sizeof(CPlayerInputGlobals), 4, NULL, TRUE);
    ptCPlayerInputGlobals->listHighlightedRActors.Initialize(MemoryPoolsGetDATAIDListPool());


    ptCPlayerInputGlobals->tCursorMgr.Init();
}





void CPlayerInputDisposeForApp()
{
    CPlayerInputGlobals *& ptCPlayerInputGlobals = CGameGet().ptCPlayerInputGlobals;

    ptCPlayerInputGlobals->listHighlightedRActors.Dispose();

    SigmaMemoryFree(ptCPlayerInputGlobals, NULL);
    ptCPlayerInputGlobals = NULL;
}





static void sCPlayerInputReset(Player * ptPlayer)
{

    CPlayerInputGlobals * ptCPlayerInputGlobals = CGameGet().ptCPlayerInputGlobals;

    ptCPlayerInputGlobals->tCursorMgr.Init();

    if (ptPlayer)
        ptPlayer->tClientInfo.Reset();



    CPlayerReset();
}





void CPlayerInputInitializeForGame()
{

    sCPlayerInputReset(NULL);
}



void CPlayerInputDisposeForGame()
{
}




void CPlayerInputInitializeForWorld(Player * ptPlayer)
{

    sCPlayerInputReset(ptPlayer);
}



void CPlayerInputDisposeForWorld()
{
}




void sPlayerSetHighlight(
    RActorID idRActor)
{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return;




    CPlayerInputGlobals * ptCPlayerInputGlobals = CGameGet().ptCPlayerInputGlobals;
    ptCPlayerInputGlobals->listHighlightedRActors.AddTail_Tracked(idRActor);
}



static BOOL sIsCursorInMainViewport()
{

    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);


    IRect2D rectViewportPixels;
    CGameGetLetterboxedViewportPixels(rectViewportPixels);

    return rectViewportPixels.ContainsPoint(tInputState.tMouseState.p);
}



static void sUpdateFindNewHighlights(
    const Player * ptPlayer)
{
    if (!InputIsCursorInWindow() && !InputAppHasCapture())
        return;




    if (!sIsCursorInMainViewport())
        return;



    RActorID idLocalPlayerRActor = ptPlayer->idPlayerRActor;
    if (idLocalPlayerRActor == RACTORID_INVALID)
        return;



    const RActor * ptPlayerRActor = RActorTryToGet(idLocalPlayerRActor);
    if (!ptPlayerRActor)
        return;



    UIID idUIElement = UI_GetMousedOverControl();

    RActorID idRActorHit = RACTORID_INVALID;


    if (UI_TestID(idUIElement) &&
        idUIElement != cgUIIDRoot &&
        idUIElement != cgUIIDMainScreen &&

        UI_GetWindowFromID(idUIElement).IsActive())
    {
        if (UI_GetControlFromID(idUIElement).IsType(UIT_ITEMTAG))
        {
            idRActorHit = UI_GetItemTagFromID(idUIElement).GetRActor();
            sPlayerSetHighlight(idRActorHit);
            return;
        }

        if (UI_GetWindowFromID(idUIElement).EatsClicks() != EC_IGNORE)
            return;
    }




    SNO snoHighlightPower = GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT);
    switch (ptPlayer->tClientInfo.nInputMode)
    {
    case 0:
    case 2:
    case 3:
    {
        ResolvedClick tResolvedClick;
        CPlayerUtilResolveClick(ptPlayer, snoHighlightPower, TRUE, tResolvedClick);
        sPlayerSetHighlight(tResolvedClick.idRActor);
        break;
    }
    case 1:
    {



        if (ptPlayer->tClientInfo.tTargetLock.tTarget.eType != TT_ACTOR)
        {
            sPlayerSetHighlight((RActorID)ACDGetOwner(ptPlayer->tClientInfo.tTargetLock.tTarget.idACDTarget));
        }
        else
        {

            ResolvedClick tResolvedClick;
            CPlayerUtilResolveClick(ptPlayer, snoHighlightPower, TRUE, tResolvedClick);
            sPlayerSetHighlight(tResolvedClick.idRActor);
        }
        break;
    }
    default:
    {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    }
    if (PlayerGetPlayerClass(ptPlayer) == PLAYER_CLASS_NECROMANCER)
    {
        blz::vector<RActorID, blz::allocator<RActorID> > listRActorNecroCorpses = SelectionGetTargetedCorpses();
        for (uint32 i = 0; i < listRActorNecroCorpses.size(); ++i)
        {
            RActorID idRActorCorpse = listRActorNecroCorpses[i];
            if (idRActorCorpse != RACTORID_INVALID)
                sPlayerSetHighlight(idRActorCorpse);
        }
    }
}





static BOOL sDoHighlight(
    const RActorID idRActor,
    const Player * ptPlayer)
{
    if (idRActor == RACTORID_INVALID || (sgtAppGlobals.dwDebugFlags & (1 << APP_DRAW_NO_ACTORS_BIT)))
        return FALSE;



    BOOL fHighlightedNewMonster = FALSE;
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {
        RActorHighlight(*ptRActor, TRUE);

        BOOL fShow = FALSE;


        if (PlayerGetLocalPlayerCount() == 1)
        {

            ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
            if (ptACD)
            {
                ActorCommonData * ptACDLocalPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                if (ptACDLocalPlayer &&
                    ACDIsHostile(*ptACDLocalPlayer, *ptACD, FALSE) &&
                    !ptACD->IsWreckableObject() &&
                    !ptACD->IsGizmo() &&

                    ptACD->ConvertsDamageToDamageState())
                    fShow = TRUE;
            }
        }





        if (fShow)
        {
            UIGame::SetMonsterHPBar(ptRActor->idACD);
            fHighlightedNewMonster = TRUE;
        }


        if (!RActorIsNecroCorpse(ptRActor))
        {
            UIGame::SetRActorHighlight(idRActor, 0);
        }
    }
    return fHighlightedNewMonster;
}




static void sDoUnHighlight(
    const RActorID idRActor,
    const BOOL bHideMonsterHPBar,
    const BOOL bHideRActorHighlight)
{
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor)
    {

        RActorHighlight(*ptRActor, FALSE);

        if (!RActorIsNecroCorpse(ptRActor))
        {
            if (bHideMonsterHPBar)
            {
                UIGame::SetMonsterHPBar(ACDID_INVALID);
            }
            if (bHideRActorHighlight)
            {
                UIGame::SetRActorHighlight(RACTORID_INVALID, 0);
            }
        }
    }
}




SNO GetTargetLockSNOForPlayer(const Player * ptPlayer)
{
    if (ptPlayer->nPlayerIndex >= 0 && ptPlayer->nPlayerIndex < 4)
        return GlobalSNOGet((GlobalSNO)(GLOBALSNO_EFFECT_GROUP_TARGET_LOCKED_P1 + ptPlayer->nPlayerIndex));
    return SNO_NONE;
}























static void sUpdateHighlight(
    const Player * ptPlayer)
{
    CPlayerInputGlobals * ptCPlayerInputGlobals = CGameGet().ptCPlayerInputGlobals;

    if (InputIsControllerPresent())
    {


        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDPlayer)
            return;



        SNO snoPower = ptACDPlayer->AttributeGetBasicAttack();
        if (snoPower == SNO_NONE) snoPower = GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_RANGED_WAND);



        {
            ResolvedClick tResolvedClick;
            Targeting::Console::ResolveTargetForPlayer(ptPlayer, snoPower, tResolvedClick, FALSE, TRUE, FALSE, FALSE, FALSE);
            RActorID idRActorHit = tResolvedClick.idRActor;
            RActorID idOldRActorHit = UIGame::GetRActorHighlight(ptPlayer->nControllerIndex);

            RActor * ptRActorHit = RActorTryToGet(idRActorHit);
            RActor * ptOldRActorHit = RActorTryToGet(idOldRActorHit);
            if (idRActorHit != idOldRActorHit || (ptRActorHit && (ptRActorHit->dwFlags & (1 << RACTOR_HIDE_BIT))))
            {
                if (ptRActorHit && !(ptRActorHit->dwFlags & (1 << RACTOR_HIDE_BIT)))
                {


                    UIGame::SetRActorHighlight(idRActorHit, ptPlayer->nControllerIndex);
                }
                else
                {
                    UIGame::SetRActorHighlight(RACTORID_INVALID, ptPlayer->nControllerIndex);
                }

                if (ptOldRActorHit)
                {
                    RActorHighlight(*ptOldRActorHit, FALSE);

                    UIHUD::Console::UpdatePlayerTargetingIndicator(ptPlayer, ptOldRActorHit, FALSE);
                }
            }
        }




        {
            ResolvedClick tResolvedClick;
            Targeting::Console::ResolveTargetForPlayer(ptPlayer, snoPower, tResolvedClick, FALSE, TRUE, TRUE, FALSE, FALSE);
            RActorID idRActorHit = tResolvedClick.idRActor;
            RActorID idOldRActorHit = UIGame::GetRActorAlternateHighlight(ptPlayer->nControllerIndex);
            if (idRActorHit != idOldRActorHit)
            {


                UIGame::ClearRActorAlternateHighlightPrompt(ptPlayer->nControllerIndex);

                RActor * ptRActorHit = RActorTryToGet(idRActorHit);
                RActor * ptOldRActorHit = RActorTryToGet(idOldRActorHit);

                if (ptRActorHit && idRActorHit != UIGame::GetRActorHighlight(ptPlayer->nControllerIndex))
                {
                    UIGame::SetRActorAlternateHighlight(idRActorHit, ptPlayer->nControllerIndex);
                }
                else
                {
                    UIGame::SetRActorAlternateHighlight(RACTORID_INVALID, ptPlayer->nControllerIndex);
                }

                if (ptOldRActorHit)
                {
                    RActorHighlight(*ptOldRActorHit, FALSE);
                }
            }
        }


        if (ptPlayer->tTargetComponent.GetInAimMode())
        {
            ActorCommonData * ptACDLockedTarget = const_cast<ConsoleTargetComponent &>(ptPlayer->tTargetComponent).ResolveLockedTarget();
            if (ptACDLockedTarget)
            {
                SNO snoEffect = GetTargetLockSNOForPlayer(ptPlayer);
                ptACDLockedTarget->RemoveClientEffect(snoEffect);
            }
        }
        return;
    }



    BOOL fAllowNewHighlights = TRUE;
    if (UIGame::MenuIsActive())
        fAllowNewHighlights = FALSE;
    else
    {


        CursorState eCursorState = PlayerGetCursorMgr().GetCursorState();
        if (eCursorState == CURSORSTATE_MOVING_HOTBAR_BUTTON)
            fAllowNewHighlights = FALSE;
    }



    if (UIMap::IsOpen() || UIWaypointMap::IsOpen())
        fAllowNewHighlights = FALSE;



    if (CinematographyGetFocus() != RACTORID_INVALID)
        fAllowNewHighlights = FALSE;









    RActorIDListPooled listHighlightedRActorsOld(MemoryPoolsGetDATAIDListPool());
    while (!ptCPlayerInputGlobals->listHighlightedRActors.IsEmpty())
    {
        RActorID idRActor = (RActorID)ptCPlayerInputGlobals->listHighlightedRActors.GetHead();
        ptCPlayerInputGlobals->listHighlightedRActors.RemoveHead();

        listHighlightedRActorsOld.AddTail_Tracked(idRActor);
    }

    if (fAllowNewHighlights)
    {
        if (ptPlayer->tClientInfo.nInputMode == INPUT_MODE_ACTOR_TARGET_LOCK &&
            ptPlayer->tClientInfo.tTargetLock.tTarget.IsTargetingACD() &&
            (!sPlayerPowerIsChannelled(ptPlayer) ||
             PowerStickyTargetsActorsWhileSweeping(AutoPowerDefinitionR(ptPlayer->tClientInfo.snoCurrentChannelledPower))))
        {
            sPlayerSetHighlight((RActorID)ACDGetOwner(ptPlayer->tClientInfo.tTargetLock.tTarget.idACDTarget));
        }
        else
        {
            sUpdateFindNewHighlights(ptPlayer);
        }
    }


    POS pos = ptCPlayerInputGlobals->listHighlightedRActors.GetHeadPosition();
    BOOL fHighlightedNewMonster = FALSE;
    RActorID idNewlyHighlighted = RACTORID_INVALID;
    while (pos)
    {
        RActorID idRActor = (RActorID)ptCPlayerInputGlobals->listHighlightedRActors.GetNext(pos);
        if (!listHighlightedRActorsOld.Find(idRActor))
        {
            fHighlightedNewMonster = sDoHighlight(idRActor, ptPlayer);
            idNewlyHighlighted = idRActor;
        }
    }



    while (!listHighlightedRActorsOld.IsEmpty())
    {
        RActorID idRActor = (RActorID)listHighlightedRActorsOld.GetHead(); listHighlightedRActorsOld.RemoveHead();
        if (!ptCPlayerInputGlobals->listHighlightedRActors.Find(idRActor))
        {


            BOOL bHideMonsterHPBar = !fHighlightedNewMonster;
            BOOL bHideRActorHighlight = idNewlyHighlighted == RACTORID_INVALID;
            sDoUnHighlight(idRActor, bHideMonsterHPBar, bHideRActorHighlight);
        }
    }












































    if (GfxStereo3DEnabled())
    {
        SelectionGlobals * ptSelectionGlobals = CGameGet().ptSelectionGlobals;
        WorldDistance wdIntersect = ptSelectionGlobals->flStereoDepth;

        RActorIntersectArray & listRActorsHit = SelectionGetPickList();
        if (!listRActorsHit.empty())
        {
            wdIntersect = listRActorsHit[0].flT;
        }
        else
        {
            IVector2D pointCursor = InputGetCursorPos();
            Vector3D wpIntersect;
            ScreenPickGround(pointCursor, wpIntersect, 0, -1, NULL, NULL, NULL, &wdIntersect);
        }
        ptSelectionGlobals->flStereoDepth = wdIntersect;
    }
}





















BOOL CPlayerInputCancelMode(Player * ptPlayer, BOOL bStopDriverMode)
{


    ACDID idLocalPlayerACD = ptPlayer->idPlayerACD;
    if (idLocalPlayerACD == ACDID_INVALID)
        return FALSE;




    if (sPlayerPowerIsChannelled(ptPlayer))

        sAbortChannelledPower(ptPlayer);


    if (!CPlayerInputAbortClientWalkPower(ptPlayer))
        return FALSE;










    ptPlayer->tClientInfo.tTargetLock.Clear();
    ptPlayer->tClientInfo.nInputMode = bStopDriverMode ? 3 : 0;
    ptPlayer->tClientInfo.snoCurrentPower = SNO_NONE;
    ptPlayer->tClientInfo.bClosing = FALSE;
    ptPlayer->tClientInfo.dwClosingAbortTime = 0;
    ptPlayer->tClientInfo.nUIAction = -1;


    return TRUE;
}

static BOOL sPlayerAnimHasReachedInterruptFrame(
    const ActorCommonData & tACD)
{
    RActorID idRActor = (RActorID)ACDGetOwner(tACD.id);
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (!ptRActor)
        return FALSE;
    if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {
        return RActorAnimationHasReachedInterruptFrame(*ptRActor);
    }
    else
    {
        return RActorAnimTreeLayerHasReachedInterruptFrame(*ptRActor, ANIM_LAYER_ACTION);
    }
}







static BOOL sPlayerCanInterruptHimself(
    Player * ptPlayer,
    BOOL bToWalk)
{
    ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
    AnimReason eAnimReason = AnimPlayerGetAnimReason(tACD);


    switch (eAnimReason)
    {
    case ANIM_REASON_IDLE:
    case ANIM_REASON_WALKING:
    case ANIM_REASON_SPAWN:
    case ANIM_REASON_INTERRUPTABLE_ANIMATION:
    case ANIM_REASON_PREPLAY_CHANNELLED:
    case ANIM_REASON_FLOAT_CONVERSATION:
    case ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH:
        break;

    case ANIM_REASON_POWER:
    case ANIM_REASON_PREPLAY_POWER:
    {



        if (bToWalk)
        {
            SNO snoCurrentAnimPower = CPlayerInputGetCurrentAnimPower(ptPlayer);
            if (snoCurrentAnimPower != SNO_NONE)
            {
                if (PowerIsUninterruptableDuring(&tACD, AutoPowerDefinitionR(snoCurrentAnimPower)))
                    return FALSE;



                if (PowerIsCancellableByWalking(AutoPowerDefinitionR(snoCurrentAnimPower)))
                    return TRUE;
            }



            if (sPlayerAnimHasReachedInterruptFrame(tACD))
            {


                uint32 dwAnimEndTime = AnimPlayerGetAnimEndTime(tACD);
                if (dwAnimEndTime > ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime)
                {
                    ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime = dwAnimEndTime;
                }
                return TRUE;
            }
        }
        return FALSE;
    }
    default: return FALSE;
    }
    ActorMovement * ptMove = tACD.GetActorMovement();
    if (ptMove && ptMove->IsMoving())
    {
        if (ptMove->m_uMoveFlags & (1 << MOVEBIT_FROM_POWER))
            return FALSE;



        if (ptMove->IsInArcingMovement())
            return FALSE;
    }


    return TRUE;

}




static BOOL sPlayerCanInterruptHimselfAllowPowerCanceling(
    Player * ptPlayer,
    SNO snoPower)
{
    ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
    AnimReason eAnimReason = AnimPlayerGetAnimReason(tACD);


    if (eAnimReason != ANIM_REASON_IDLE && eAnimReason != ANIM_REASON_WALKING &&
        eAnimReason != ANIM_REASON_POWER && eAnimReason != ANIM_REASON_SPAWN &&
        eAnimReason != ANIM_REASON_STANCE && eAnimReason != ANIM_REASON_INTERRUPTABLE_ANIMATION &&
        eAnimReason != ANIM_REASON_PREPLAY_POWER && eAnimReason != ANIM_REASON_PREPLAY_CHANNELLED &&
        eAnimReason != ANIM_REASON_FLOAT_CONVERSATION && eAnimReason != ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH)
        return FALSE;









    if (tACD.AttributesGetInt(ATTRIBUTE_STUNNED) > 0 ||
        tACD.AttributesGetInt(ATTRIBUTE_FROZEN) > 0 ||
        tACD.AttributesGetInt(ATTRIBUTE_BLIND) > 0 ||
        tACD.AttributesGetInt(ATTRIBUTE_FORCE_GRIPPED))
    {
        if (!PowerBreaksStun(&tACD, AutoPowerDefinitionR(snoPower)))
            return FALSE;
    }


    else if (tACD.AttributesGetInt(ATTRIBUTE_FEARED) > 0)
    {
        if (!PowerBreaksFear(&tACD, AutoPowerDefinitionR(snoPower)))
            return FALSE;
    }


    else if (eAnimReason == ANIM_REASON_STANCE)
        return FALSE;




    if (ptPlayer->tClientInfo.snoCurrentAnimPower != SNO_NONE)
    {
        switch (eAnimReason)
        {
        case ANIM_REASON_WALKING:
            if (tACD.AttributesGetInt(ATTRIBUTE_FROZEN) > 0)
            {
                return PowerBreaksStun(&tACD, AutoPowerDefinitionR(snoPower));
            }

            if (ptPlayer->tClientInfo.snoCurrentPower == SNO_NONE && PowerGetTemplate(AutoPowerDefinitionR(ptPlayer->tClientInfo.snoCurrentAnimPower), ptPlayer->idPlayerACD) == SPECIALWALK_POWER)
                return FALSE;



            if (!PowerIsUninterruptableDuring(&tACD, AutoPowerDefinitionR(ptPlayer->tClientInfo.snoCurrentAnimPower)))
                return FALSE;
            break;



        case ANIM_REASON_POWER:
        case ANIM_REASON_PREPLAY_POWER:
            if (tACD.AttributesGetInt(ATTRIBUTE_FROZEN) > 0)
            {
                return PowerBreaksStun(&tACD, AutoPowerDefinitionR(snoPower));
            }

            if (PowerIsComboPower(AutoPowerDefinitionR(snoPower)) &&
                CPlayerUtilCanInterruptComboNow())
                return TRUE;



            if (PowerIsCancellableByAnyPower(AutoPowerDefinitionR(ptPlayer->tClientInfo.snoCurrentAnimPower)))
                return TRUE;




            else if (!PowerIsCancellable(AutoPowerDefinitionR(ptPlayer->tClientInfo.snoCurrentAnimPower)))
                return FALSE;



            else if (PowerIsUninterruptableDuring(&tACD, AutoPowerDefinitionR(ptPlayer->tClientInfo.snoCurrentAnimPower)))
                return FALSE;



            else if (ptPlayer->tClientInfo.snoCurrentAnimPower == snoPower)
                return FALSE;



            else if (PowerCancelsOtherPowers(AutoPowerDefinitionR(snoPower)))
            {
                if (PowerCancelsOnInterruptFrame(AutoPowerDefinitionR(snoPower)))
                {
                    return sPlayerAnimHasReachedInterruptFrame(tACD);
                }
                return TRUE;
            }
            return FALSE;

        case ANIM_REASON_PREPLAY_CHANNELLED:
        {



            uint32 dwNow = CGameGetClientTicks();
            if (ptPlayer->tClientInfo.snoCurrentChannelledPower != snoPower && dwNow < ptPlayer->tClientInfo.dwMinChanneledDurationTime)
                return FALSE;
            break;
        }
        }
    }

    else if (eAnimReason == ANIM_REASON_PREPLAY_POWER || eAnimReason == ANIM_REASON_POWER)
    {

        if (tACD.AttributesGetInt(ATTRIBUTE_FROZEN) > 0)
        {
            return PowerBreaksStun(&tACD, AutoPowerDefinitionR(snoPower));
        }

        return PowerCancelsOtherPowers(AutoPowerDefinitionR(snoPower));
    }
    else if (eAnimReason == ANIM_REASON_WALKING && ptPlayer->tClientInfo.snoCurrentClientWalkPower != SNO_NONE)
    {
        uint32 dwNow = CGameGetClientTicks();
        if (ptPlayer->tClientInfo.snoCurrentClientWalkPower != snoPower && dwNow < ptPlayer->tClientInfo.dwMinChanneledDurationTime)
            return FALSE;
    }





    RActorID idRActor = RActorID(ACDGetOwner(tACD.id));
    RActor * ptRActor = RActorTryToGet(idRActor);
    if (ptRActor && ptRActor->ptCActorMovement && ptRActor->ptCActorMovement->IsMoving())
    {
        if (ptRActor->ptCActorMovement->m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT))
            return FALSE;
    }

    return TRUE;



}




static BOOL sWalkToPoint(
    const RActor * ptPlayerRActor,
    const SNO snoPower,
    const SNO snoOriginalPower,
    const Target & tActorTarget,
    WorldDistance wdMaxRangeFromDest,
    float flSpeed,
    const BOOL bUsingClientAuthPower,
    const BOOL fCanCachePower,
    const BOOL fRefreshCacheTime,
    const BOOL fIgnoreRange,
    const uint32 nMinWalkTicks)
{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);
    if (sPlayerPowerIsChannelled(ptPlayer))
    {
        if (!sAbortChannelledPower(ptPlayer))
            return FALSE;
    }



    if (ptPlayer->tClientInfo.snoCurrentClientWalkPower != SNO_NONE && ptPlayer->tClientInfo.snoCurrentClientWalkPower != snoPower)
    {
        if (!CPlayerInputAbortClientWalkPower(ptPlayer))
            return FALSE;
    }




    const WorldDistance wdStutterThreshold = 3.0f;

    WorldPlace placeToUse;
    tActorTarget.GetPlace(placeToUse);


    {
        Vector3D wvActorToPoint = placeToUse.wp - ptPlayerRActor->tLocation.tWorldLocation.transform.wp;
        WorldDistance wdSeparation = wvActorToPoint.Normalize();

        if (wdSeparation < wdStutterThreshold &&
            tActorTarget.eType == TT_POINT &&
            CinematographyGetState() == CS_FIXED_VIEW)
        {
            placeToUse.wp += wvActorToPoint * wdStutterThreshold;
        }
    }
    ptPlayer->tClientInfo.snoCurrentPower = snoPower;




    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!sPlayerCanInterruptHimself(ptPlayer, TRUE))
    {

        if (fCanCachePower && ptACDPlayer && !PowerIsChannelled(*ptACDPlayer, AutoPowerDefinitionR(snoOriginalPower)))
        {
            ptPlayer->tClientInfo.tCachedTargetLock.Set(snoOriginalPower, snoOriginalPower, tActorTarget, ACDID_INVALID, fIgnoreRange);
        }

        if (fRefreshCacheTime)
        {
            ptPlayer->tClientInfo.dwTimeInputWasCached = CGameGetClientTicks();
        }
        return FALSE;
    }








    ptPlayer->tClientInfo.snoCurrentAnimPower = SNO_NONE;
    ptPlayer->tClientInfo.snoCurrentOriginalAnimPower = SNO_NONE;
    ptPlayer->tClientInfo.snoCurrentLoopingPower = SNO_NONE;

    uint32 dwAnimTag = RActorAnimationGetWalkAnimTag(*ptPlayerRActor, snoPower, flSpeed < EPSILON ? 4294967296.0f : flSpeed);


    if (ptACDPlayer && CWorldIsRetro())
    {
        float flRunningSpeed = ptACDPlayer->AttributesGetFloat(FastAttribKey(ATTRIBUTE_RUNNING_RATE_TOTAL));
        flSpeed = Math::Max(flRunningSpeed, flSpeed);
    }







    ActorSteer & tSteer = ptPlayerRActor->ptCActorMovement->GetSteer();
    tSteer.SetLookAheadTicks(1);
    tSteer.SetMaxSteeringDist(0.4f);
    tSteer.SetUnitSteer(1.0f);
    tSteer.Set180SteeringHoldTicks(6);
    tSteer.SetIterations(4);
    ptPlayerRActor->ptCActorMovement->SetIgnoreCollision(FALSE);
    ptPlayerRActor->ptCActorMovement->SetMinDistToWalk(0.5f);


    uint32 uFlags = (1 << MOVEBIT_AUTO_UPDATE_YAW);



    if (bUsingClientAuthPower)
        uFlags |= (1 << MOVEBIT_CLIENT_AUTH_POWER);



    SNO snoPowerPassability = snoPower;
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_MANUAL_WALK))
    {
        snoPowerPassability = GlobalSNOGet(GLOBALSNO_POWERS_WALK);
    }

    ptPlayerRActor->ptCActorMovement->MoveTo(placeToUse, flSpeed, 0.0f, dwAnimTag, uFlags, wdMaxRangeFromDest, ACDID_INVALID, snoPowerPassability, nMinWalkTicks, -1, 100.0f, NULL);












    UI_ResetForPlayerMovement(ptPlayer);


    LocalPlayerClamping::ClearInactiveFlagForController(ptPlayer->nControllerIndex, LocalPlayerClamping::AR_MOVEMENT);
    return TRUE;
}





static BOOL sIsANormalWalkPower(SNO snoPower, ACDID idACD)
{


    if (snoPower != SNO_NONE)
    {
        PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), idACD);
        if (eTemplate == WALK_POWER)
            return TRUE;
    }
    return FALSE;
}
#include "../Common/Definitions/ActDefinition.h"  // ResolvedPortalDestination. The declarations below are from their definitions' DWARF entries (no recovered header declares them).
BOOL PowerAlternatesAnims(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower); uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim);  // PowersCommon.cpp:7986, :8013
BOOL PowerNeverUpdatesFacingStarting(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, ACDID idACDTarget); void PowerFaceTarget(ActorCommonData * ptACD, const Target & tTarget, const uint32 dwAnimationTag, BOOL bSnapToFacing);  // PowersCommon.cpp:4857, :5454
BOOL PowerFacesTargetFacing(AutoPowerDefinitionR pdefPower, ACDID idACDTarget); BOOL PowerCastTargetNecroCorpsesOnly(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower);  // PowersCommon.cpp:1340, :1188
BOOL PowerShouldPlayAnimForPower(ACDID idACD, const Target & tTarget, SNO snoNewPower, SNO snoCurrentPower); SNO PowerGetNormalAttackReplacement(ACDID idACD);  // PowersCommon.cpp:8259, :2206
Angle ACDGetYaw(ACDID idACD); void ACDSetYaw(ACDID idACD, const Angle aYaw); ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD);  // ActorCommonData.cpp:2997, :3027; ActorInventory.cpp:5373
BOOL ItemIsOneHandedWeapon(const ActorCommonData & tACDItem); uint32 AnimPlayerGetCurrentAnimTag(ActorCommonData & tACD); void CBlizzconVictory();  // Items.cpp:3422; AnimPlayer.cpp:1856; CBlizzconTimer.cpp:251
BOOL ShouldDisableMessageFromTarget(
    ACDID idACDTarget,
    Player * ptPlayer)
{



    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && idACDTarget != ACDID_INVALID)
    {


        ActorCommonData & tTarget = ACDGet(idACDTarget);
        if (tTarget.snoActor == 176001 &&
            tTarget.ptResolvedDest &&
            tTarget.ptResolvedDest->snoWorld == 75049)
        {
            CBlizzconVictory();
            return TRUE;
        }
    }
    return FALSE;
}




















static BOOL sPlayerPlayAnimAndSendMessage(
    const RActor * ptPlayerRActor,
    const SNO snoPower,
    const SNO snoOriginalPower,
    const ACDID idACDItemBeingUsed,
    const Target & tActorTarget,
    BOOL bCancelingExistingPower,
    PowerTemplate eTemplate,
    const SNO snoData0,
    const int32 nData1)
{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);
    ACDID idACD = ptPlayerRActor->idACD;
    SNO snoPowerToUse = snoPower;
    ActorCommonData & tACD = ACDGet(idACD);
    SNO snoReplacementPower = PowerGetNormalAttackReplacement(tACD.id);


    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT) && snoReplacementPower != SNO_NONE)
    {
        snoPowerToUse = snoReplacementPower;

        FastAttribSetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_NORMAL_ATTACK_REPLACEMENT_POWER_SNO), SNO_NONE);
    }

    BOOL bUsesMainHandOnly = PowerUsesMainHandOnly(AutoPowerDefinitionR(snoPower), 0);
    int32 nComboLevelToUse = 0;
    uint32 dwNow = CGameGetClientTicks();
    ScopedGlobalDefinitionLockR pdefGlobal;
    if (ptPlayer->tClientInfo.dwTimeLastComboAttack + Math::FloatToInt(pdefGlobal->flComboClientTimeout * 60.0f) < dwNow)
    {
        CPlayerInputSetComboLevel(0, ptPlayer);
    }

    if (PowerIsComboPower(AutoPowerDefinitionR(snoPowerToUse)))
    {
        nComboLevelToUse = tACD.GetComboLevel();
        bUsesMainHandOnly = PowerUsesMainHandOnly(AutoPowerDefinitionR(snoPower), nComboLevelToUse);
    }


    else if (!PowerRefreshesComboLevel(AutoPowerDefinitionR(snoPowerToUse)) && PowerIsOffensive(AutoPowerDefinitionR(snoPowerToUse)))
    {
        tACD.SetComboLevel(0);
    }



    if (ACDIsDualWielding(tACD, TRUE))
    {
        if (bUsesMainHandOnly)
        {
            FastAttribSetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), DUALWIELD_ATTACK_MAIN);

            ACDID idACDPrimaryWeapon = ACDInventoryGetPrimaryWeapon(tACD.id);
            ACDID idACDSecondaryWeapon = ACDInventoryGetSecondaryWeaponOrShield(tACD.id);

            ActorCommonData * ptACDPrimaryWeapon = ACDTryToGet(idACDPrimaryWeapon);
            ActorCommonData * ptACDSecondaryWeapon = ACDTryToGet(idACDSecondaryWeapon);

            if (!ptACDPrimaryWeapon && ptACDSecondaryWeapon && ItemIsOneHandedWeapon(*ptACDSecondaryWeapon))
            {
                tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), DUALWIELD_ATTACK_OFFHAND);
            }
        }
        else if (PowerAffectedByDualWield(AutoPowerDefinitionR(snoPowerToUse)))
        {

            DualWieldAttackType eAttackType = (DualWieldAttackType)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND_NEXT));
            DualWieldAttackType eNextAttackType =
                (eAttackType == DUALWIELD_ATTACK_MAIN) ? DUALWIELD_ATTACK_OFFHAND : DUALWIELD_ATTACK_MAIN;






            FastAttribSetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND), eAttackType);
            FastAttribSetValueInt(tACD.idFastAttrib, FastAttribKey(ATTRIBUTE_DUALWIELD_HAND_NEXT), eNextAttackType);
        }
    }


    BOOL bPrePlayAnim = PowerPreplaysAnimation(AutoPowerDefinitionR(snoPowerToUse));
    bool bAnimAlreadyPlaying = false;
    BOOL bUseAlternateAnim = FALSE;

    if (PowerAlternatesAnims(tACD, AutoPowerDefinitionR(snoPowerToUse)))
    {
        uint32 lastAnimTypePlayed = 0;
        ptPlayer->tClientInfo.tCachedSnoStates.Lookup(snoPowerToUse, lastAnimTypePlayed);

        if (lastAnimTypePlayed == 0)
        {
            bUseAlternateAnim = TRUE;
            ptPlayer->tClientInfo.tCachedSnoStates.SetAt_Tracked(snoPowerToUse, 1);
        }
        else
        {
            ptPlayer->tClientInfo.tCachedSnoStates.SetAt_Tracked(snoPowerToUse, 0);
        }
    }

    uint32 dwAnimationTag = PowerDetermineAnimation(AutoPowerDefinitionR(snoPowerToUse), ptPlayerRActor->idACD, tActorTarget, bUseAlternateAnim);

    if (bPrePlayAnim &&
        PowerIsChannelled(tACD, AutoPowerDefinitionR(snoPowerToUse)) &&
        snoPowerToUse == ptPlayer->tClientInfo.snoCurrentChannelledPower &&
        AnimPlayerGetCurrentAnimTag(tACD) == dwAnimationTag)
    {
        bAnimAlreadyPlaying = true;
    }



    if (bCancelingExistingPower)
    {
        bPrePlayAnim = FALSE;
    }

    if (!PowerShouldPlayAnimForPower(idACD, tActorTarget, snoPowerToUse, ptPlayer->tClientInfo.snoCurrentPower))
    {
        bAnimAlreadyPlaying = true;
        bPrePlayAnim = TRUE;
    }


    if (!PowerIsChannelled(tACD, AutoPowerDefinitionR(snoPower)) || PowerClientControlsFacing(AutoPowerDefinitionR(snoPower)))
    {
        BOOL bFaceTarget = !PowerNeverUpdatesFacingStarting(AutoPowerDefinitionR(snoPowerToUse), tACD.id, tActorTarget.idACDTarget);
        if (bFaceTarget)
        {
            BOOL bSnapToFacing = PowerSnapsToFacing(AutoPowerDefinitionR(snoPowerToUse));
            PowerFaceTarget(&tACD, tActorTarget, dwAnimationTag, bSnapToFacing);

            if (PowerClientControlsFacing(AutoPowerDefinitionR(snoPower)))
            {
                ptPlayerRActor->ptCActorMovement->SendFacingToServer(bSnapToFacing);
            }
        }
    }

    BOOL bFaceTargetFacing = PowerFacesTargetFacing(AutoPowerDefinitionR(snoPowerToUse), tActorTarget.idACDTarget);
    if (bFaceTargetFacing)
    {
        ACDSetYaw(ptPlayerRActor->idACD, ACDGetYaw(tActorTarget.idACDTarget));
    }

    float flSpeedScalar = PowerGetAnimationSpeedForPower(tACD.id, AutoPowerDefinitionR(snoPowerToUse), -1);
    uint32 dwStartTime = CGameGetPredictedServerTime();

    BOOL bDisableInput = !bPrePlayAnim;


    if (dwAnimationTag == (uint32)-1)
    {
        bPrePlayAnim = FALSE;
    }


    ptPlayer->tClientInfo.snoCurrentAnimPower = snoPowerToUse;
    ptPlayer->tClientInfo.snoCurrentOriginalAnimPower = snoOriginalPower;

    if (tActorTarget.eType == TT_ACTOR)
    {
        ptPlayer->tClientInfo.idACDCurrentTarget = tActorTarget.idACDTarget;
    }

    TargetMessage tMessage;

    if (bPrePlayAnim)
    {
        AnimPreplayData tPreplayData;
        tPreplayData.dwSyncedSeed = RandGetSeed(ptPlayer->tClientInfo.rngSyncedAnim);
        tPreplayData.dwServerTimeAnimStarted = dwStartTime;

        tPreplayData.dwAnimPrePlayDataFlags = (1 << 0 /* PREPLAY_ANIM_FLAGS_PREPLAYED_ANIM */) | (bUseAlternateAnim ? (1 << 1 /* PREPLAY_ANIM_FLAGS_USE_ALT */) : 0);
        tMessage.tAnimPreplayData.Set(tPreplayData);

        uint32 dwLength = (uint32)-1;
        AnimReason eAnimReason = ANIM_REASON_PREPLAY_POWER;
        if (PowerHasInfiniteAnimationTiming(tACD, AutoPowerDefinitionR(snoPowerToUse)))
        {
            dwLength = ANIM_PLAY_FOREVER;
            if (PowerIsChannelled(tACD, AutoPowerDefinitionR(snoPowerToUse)))
            {
                eAnimReason = ANIM_REASON_PREPLAY_CHANNELLED;
                ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime = dwNow + 10;

                if (!bAnimAlreadyPlaying)
                {
                    ptPlayer->tClientInfo.dwMinChanneledDurationTime = dwNow + 10;
                }
            }
            else if (eTemplate == LOOPING_ANIMATION_POWER || eTemplate == GIZMO_POWER)
            {
                BOOL bNoInterruptionByLookswitch = FALSE;
                if (snoPowerToUse != SNO_NONE)
                {
                    ScopedPowerDefinitionLockR pdefPower(snoPowerToUse);
                    bNoInterruptionByLookswitch = PowerGeneralTagGetBool(AutoPowerDefinitionR(pdefPower), TAG_POWER_LOOKSWITCH_DOESNT_INTERRUPT);
                }

                eAnimReason = bNoInterruptionByLookswitch ? ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH : ANIM_REASON_INTERRUPTABLE_ANIMATION;
            }
        }








        if (!bAnimAlreadyPlaying)
        {
            AnimationTiming tAnimationTiming;
            AnimPlayerGetAnimationTiming(tACD, dwAnimationTag, dwLength, flSpeedScalar, ptPlayer->tClientInfo.rngSyncedAnim, tAnimationTiming);
            AnimPlayerPlayAnimationsWithTiming(tACD, &tAnimationTiming, 1, FALSE, eAnimReason, FALSE, FALSE);

            if (PowerIsComboPower(AutoPowerDefinitionR(snoPowerToUse)) || PowerRefreshesComboLevel(AutoPowerDefinitionR(snoPowerToUse)))
            {
                ptPlayer->tClientInfo.dwTimeLastComboAttack = dwNow + tAnimationTiming.dwInterruptTime;
            }
        }
    }




    else if (bDisableInput)
    {
        ptPlayer->tClientInfo.dwTimeLastNonPreplayedAnimatingPower = CGameGetClientTicks();
    }






    if (PowerCanMoveForwardBeforeContactFrame(AutoPowerDefinitionR(snoPower)) &&
        !ptPlayer->tTargetComponent.GetInAimMode())
    {
        float flContollerSquared = Console::ControllerUtils::GetLStickPosition(ptPlayer->nControllerIndex, TRUE).MagnitudeSquared();
        float flMinRange = pdefGlobal->flControllerMoveFacingZone;

        if (flContollerSquared >= flMinRange * flMinRange)
        {
            tMessage.dwFlags = (1 << 0 /* TARGET_MESSAGE_FLAGS_MOVE_TOWARDS_TARGET */);
        }
    }


    if (PowerIsChannelled(tACD, AutoPowerDefinitionR(snoPowerToUse)))
    {
        ptPlayer->tClientInfo.snoCurrentChannelledPower = snoPowerToUse;
    }


    Target tTargetToUse(tActorTarget);



    if (PowerContactTagGetBool(AutoPowerDefinitionR(snoPowerToUse), TAG_POWER_IS_LOBBED, 0))
    {
        WorldPlace tPlace;
        tTargetToUse.GetPlace(tPlace);
        tTargetToUse.InitializeFromPlace(tPlace);
    }




    if (eTemplate == LOOPING_ANIMATION_POWER)
    {
        LoopingAnimationPowerMessage tLoopingAnimMessage;
        tLoopingAnimMessage.snoPower = snoPowerToUse;
        tLoopingAnimMessage.snoData0 = snoData0;
        tLoopingAnimMessage.nData1 = nData1;
        ptPlayer->tClientInfo.snoCurrentLoopingPower = snoPowerToUse;
        MessageSendToServer(CMSG_LOOPING_ANIMATION_POWER, &tLoopingAnimMessage, sizeof(tLoopingAnimMessage), ptPlayer);
    }
    else
    {
        if (eTemplate == GIZMO_POWER && dwAnimationTag != (uint32)-1)
        {
            ptPlayer->tClientInfo.snoCurrentLoopingPower = snoPowerToUse;
        }

        tMessage.eType = tTargetToUse.eType;
        tMessage.annTarget = ANNFromACD(tTargetToUse.idACDTarget);
        tTargetToUse.GetPlace(tMessage.tPlace);
        tMessage.snoPower = snoPowerToUse;
        tMessage.annItemBeingUsed = ANNFromACD(idACDItemBeingUsed);
        tMessage.nComboLevel = nComboLevelToUse;

        if (tTargetToUse.IsNone() && PowerRequiresTarget(tACD, AutoPowerDefinitionR(snoPowerToUse)))
        {
        }

        {
            BOOL bSendMessage = !ShouldDisableMessageFromTarget(tActorTarget.idACDTarget, ptPlayer);

















            if (bSendMessage)
            {
                CPlayerUtilSpawnPowerClickEffect(snoPowerToUse, tTargetToUse);
                MessageSendToServer(CMSG_TARGET, &tMessage, sizeof(tMessage), ptPlayer);
            }
        }
    }



    if (PowerIsComboPower(AutoPowerDefinitionR(snoPowerToUse)))
    {
        int32 nNewComboLevel = (nComboLevelToUse + 1) % 3;
        tACD.SetComboLevel(nNewComboLevel);
    }

    if (PowerIsTranslate(AutoPowerDefinitionR(snoPowerToUse)))
    {
        UI_ResetForPlayerMovement(ptPlayer);
    }

    return TRUE;
}



BOOL CPlayerCanUsePowerOnTarget(
    const RActor * ptPlayerRActor,
    const SNO snoPower,
    const SNO snoOriginalPower,
    const Target & tActorTarget,
    const BOOL bIgnoreLOS,
    const BOOL fCanCachePower,
    const BOOL fRefreshCacheTime,
    BOOL & bCancelingExistingPower,
    const BOOL bRequireRangeModifiers)
{
    if (snoPower == SNO_NONE)
        return FALSE;



    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);

    if (PowerIsGizmo(AutoPowerDefinitionR(snoPower)))
    {

        ACDID idACDTarget = tActorTarget.GetACD();
        if (!ACDTryToGet(idACDTarget))
            return FALSE;
    }



    ActorCommonData & tACDPlayer = ACDGet(ptPlayerRActor->idACD);
    uint32 dwNow = CGameGetClientTicks();

    if (ptPlayer->tClientInfo.snoCurrentChannelledPower != SNO_NONE && ptPlayer->tClientInfo.snoCurrentChannelledPower == snoPower)
    {

        if (!PowerPlayerCanPayChannelCost(tACDPlayer, AutoPowerDefinitionR(snoPower)))
            return FALSE;
    }
    else
    {



        AnimReason eAnimReason = AnimPlayerGetAnimReason(tACDPlayer);
        if (eAnimReason == ANIM_REASON_INTERRUPTABLE_ANIMATION || eAnimReason == ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH)
        {

            if (PowerGetTemplate(AutoPowerDefinitionR(snoPower), tACDPlayer.id) == LOOPING_ANIMATION_POWER)
            {
                if (!PowerGeneralTagGetBool(AutoPowerDefinitionR(snoPower), TAG_POWER_LOOPING_IS_SELF_INTERRUPTING) &&
                    ptPlayer->tClientInfo.snoCurrentLoopingPower == snoPower)
                {
                    return FALSE;
                }
            }





            if (PowerIsGizmo(AutoPowerDefinitionR(snoPower)) &&
                ptPlayer->tClientInfo.snoCurrentLoopingPower == snoPower)
                return FALSE;
        }


        if (PowerGetTemplate(AutoPowerDefinitionR(snoPower), tACDPlayer.id) != SECONDARY_ANIMATION_POWER)

        {
            const uint32 dwNonPreplayedAnimWait = 15;
            if (ptPlayer->tClientInfo.dwTimeLastNonPreplayedAnimatingPower != 0 &&
                ptPlayer->tClientInfo.dwTimeLastNonPreplayedAnimatingPower + dwNonPreplayedAnimWait > dwNow)
                return FALSE;
        }
    }



    if (!tACDPlayer.IsAlive())
        return FALSE;





    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL) &&
        !tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoPower)))
        return FALSE;





    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_USE_STONE_OF_RECALL) && CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        return FALSE;












    if (PowerGetTemplate(AutoPowerDefinitionR(snoPower), tACDPlayer.id) != SECONDARY_ANIMATION_POWER)
    {
        BOOL bReadyNow = sPlayerCanInterruptHimselfAllowPowerCanceling(ptPlayer, snoPower);



        if (!sIsANormalWalkPower(snoPower, tACDPlayer.id) &&
            !PowerIsChannelled(tACDPlayer, AutoPowerDefinitionR(snoPower)) &&
            !PowerShouldUseClientWalking(tACDPlayer, AutoPowerDefinitionR(snoPower)) &&
            !PowerCancelsOtherPowers(AutoPowerDefinitionR(snoPower)) &&
            CGameGetClientTicks() < ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime)
        {
            bReadyNow = FALSE;
        }

        if (PowerCancelsOtherPowers(AutoPowerDefinitionR(snoPower)) &&
            PowerIsNoRecastUntilLastAnimFrame(AutoPowerDefinitionR(snoPower)) &&
            CGameGetClientTicks() < ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime)
        {
            bReadyNow = FALSE;
        }

        SNO snoCachedPower = ptPlayer->tClientInfo.tCachedTargetLock.snoPower;
        BOOL bChannel = PowerIsChannelled(tACDPlayer, AutoPowerDefinitionR(snoOriginalPower)) || PowerShouldUseClientWalking(tACDPlayer, AutoPowerDefinitionR(snoOriginalPower));



        if (!bReadyNow || (!bChannel && snoCachedPower != SNO_NONE && snoCachedPower != snoOriginalPower))
        {



            if (fCanCachePower && !bChannel)
            {



                BOOL bCache = TRUE;


                if (snoCachedPower != SNO_NONE && snoCachedPower != snoOriginalPower)
                {


                    if (ptPlayer->tClientInfo.snoCurrentOriginalAnimPower == snoCachedPower)
                        bCache = TRUE;
                    // Inferred cache-selection nesting from branches at 0x717b0 and 0x719c8; local names/lines are DWARF.

                    else if (ptPlayer->tClientInfo.snoCurrentOriginalAnimPower == snoOriginalPower)
                        bCache = FALSE;
                    else
                    {



                        BOOL bCancels = PowerCancelsOtherPowers(AutoPowerDefinitionR(snoOriginalPower));
                        BOOL bCachedCancels = PowerCancelsOtherPowers(AutoPowerDefinitionR(snoCachedPower));

                        if (bCancels && !bCachedCancels)
                            bCache = TRUE;


                        else if (!bCancels && bCachedCancels)
                            bCache = FALSE;
                        else
                        {



                            int32 nCooldown = PowerGetCastingCooldown(AutoPowerDefinitionR(snoOriginalPower), tACDPlayer);
                            int32 nCachedCooldown = PowerGetCastingCooldown(AutoPowerDefinitionR(snoCachedPower), tACDPlayer);
                            bCache = nCooldown > 0 && nCachedCooldown == 0;
                        }
                    }
                }














                if (bCache)
                {
                    ptPlayer->tClientInfo.tCachedTargetLock.Set(snoOriginalPower, snoOriginalPower, tActorTarget, ACDID_INVALID, bIgnoreLOS);
                }
            }

            if (fRefreshCacheTime)
            {

                ptPlayer->tClientInfo.dwTimeInputWasCached = dwNow;
            }
            return FALSE;
        }






        ptPlayer->tClientInfo.dwTimeInputWasCached = 0;
        ptPlayer->tClientInfo.tCachedTargetLock.Clear();
    }

    bCancelingExistingPower = FALSE;

    {

        AnimReason eAnimReason = AnimPlayerGetAnimReason(tACDPlayer);
        if (eAnimReason == ANIM_REASON_PREPLAY_POWER || eAnimReason == ANIM_REASON_POWER)
        {
            if (ptPlayer->tClientInfo.snoCurrentAnimPower == snoPower)
            {

                if (tActorTarget.eType == TT_ACTOR &&
                    tActorTarget.idACDTarget == ptPlayer->tClientInfo.idACDCurrentTarget)
                    return FALSE;
            }



            bCancelingExistingPower = TRUE;
        }
    }

    uint32 dwPowerFailureReasons = PowerACDCanStartPowerFailureReasons(tACDPlayer, AutoPowerDefinitionR(snoPower), &tActorTarget, FALSE, FALSE);

    if (!bRequireRangeModifiers && (dwPowerFailureReasons & (1u << PS_NOT_CLOSE_ENOUGH_BIT)) &&
        (PowerIgnoresRangeOnShiftClick(AutoPowerDefinitionR(snoPower)) || PowerClipsTargetToAttackRadius(AutoPowerDefinitionR(snoPower)) || PowerChecksPathAndCliptoPathableAttackRadius(snoPower)))
    {
        dwPowerFailureReasons &= ~(1u << PS_NOT_CLOSE_ENOUGH_BIT);
    }

    BOOL bIsChannelling = sPlayerPowerIsChannelled(ptPlayer) && ptPlayer->tClientInfo.snoCurrentChannelledPower == snoPower;
    if (!bIsChannelling && !PowerActorHasResourceMinCostToStart(tACDPlayer, AutoPowerDefinitionR(snoPower)))
    {
        dwPowerFailureReasons |= 1u << PS_NEED_RESOURCE_BIT;
    }

    BOOL bIsChannellingOrClientWalking = bIsChannelling || (sPlayerPowerIsClientWalk(ptPlayer) && ptPlayer->tClientInfo.snoCurrentClientWalkPower == snoPower);
    if (bIsChannellingOrClientWalking)
    {
        dwPowerFailureReasons &= ~(1u << PS_NEED_COOLDOWN_BIT);
    }

    ScopedPowerDefinitionLockR pdefPower(snoPower);
    if (PowerRequiresActorTarget(tACDPlayer, pdefPower))
    {
        if (tActorTarget.idACDTarget == ACDID_INVALID || !tActorTarget.IsTargetingACD() || ACDGet(tActorTarget.idACDTarget).IsItem())
        {
            dwPowerFailureReasons |= 1u << PS_NEED_TARGET;
        }
    }

    GameErrorData tErrorData;
    GameErrorFromPowerFailureReasons(tErrorData, dwPowerFailureReasons, tACDPlayer.id, AutoPowerDefinitionR(snoPower), FALSE);
    if (tErrorData.eError != GAMEERROR_NONE)
    {
        UIGame::DisplayError(tErrorData, 0, ptPlayer);

        if (tErrorData.eError == GAMEERROR_POWER_IN_COOLDOWN)
        {
            UIHUD::Console::ReportPowerOnCooldownError(ptPlayer->nPlayerIndex, snoOriginalPower);
        }
        return FALSE;
    }


    if (dwPowerFailureReasons & ((1u << PS_NEED_EMOTE_COOLDOWN_BIT) | (1u << PS_FROZEN) | (1u << PS_TARGET_IS_DEAD) | (1u << PS_CASTER_CANT_START_DISPLAYED_POWERS)))
        return FALSE;


















    if (dwPowerFailureReasons & (1u << PS_FEARED))
    {
        if (!PowerCanUseWhenFeared(&tACDPlayer, AutoPowerDefinitionR(snoPower)))
            return FALSE;
    }



    ActorMovement * ptMove = tACDPlayer.GetActorMovement();

    if (PowerIsPrimary(AutoPowerDefinitionR(snoPower), tACDPlayer.id) && ptMove->IsInArcingMovement())
        return FALSE;



    if (!bIgnoreLOS && (dwPowerFailureReasons & (1u << PS_NO_LINE_OF_SIGHT)))
    {
        WorldPlace placeTarget;
        tActorTarget.GetPlace(placeTarget);

        ptMove->FaceTarget(placeTarget.wp, FALSE);

        if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO))
        {
            ActorCommonData * ptACDTarget = ACDTryToGet(tActorTarget.idACDTarget);
            if (ptACDTarget)
            {
                GizmoType nGizmoType = ptACDTarget->GetGizmoType();
            }
        }
        // Inference at line 2014: retain the DWARF local/call; no surviving use of its result is visible.




        UIGame::DisplayError(GameErrorData(GAMEERROR_POWER_NEEDS_LINE_OF_SIGHT), 0, ptPlayer);
        return FALSE;
    }

    if (dwPowerFailureReasons & (1u << PS_POWER_DISABLED))
        return FALSE;





    if (snoPower != snoOriginalPower)
    {
        GameError eGameError;
        InputChooseFallbackPower(snoOriginalPower, ptPlayer, &eGameError);
        if (eGameError != GAMEERROR_NONE)
        {
            GameErrorData tErrorDataLoc(eGameError);
            UIGame::DisplayError(tErrorDataLoc, 0, ptPlayer);

            if (tErrorDataLoc.eError == GAMEERROR_POWER_IN_COOLDOWN)
            {
                UIHUD::Console::ReportPowerOnCooldownError(ptPlayer->nPlayerIndex, snoOriginalPower);
            }
        }
    }
    return TRUE;












}



static void sPrePlayWalkAndSendMessage(
    const RActor & tPlayerRActor,
    ActorCommonData & tACD,
    const Target & tActorTarget,
    SNO snoPower)
{
    Player * ptPlayer = PlayerGetByACD(tPlayerRActor.idACD);
    uint32 dwNow = CGameGetClientTicks();
    ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime = dwNow + 20;



    if (ptPlayer->tClientInfo.snoCurrentClientWalkPower != snoPower)
    {
        ptPlayer->tClientInfo.dwMinChanneledDurationTime = dwNow + 20;
    }
    ptPlayer->tClientInfo.snoCurrentClientWalkPower = snoPower;
    ptPlayer->tClientInfo.snoCurrentAnimPower = SNO_NONE;












    TargetMessage tMessage;
    tMessage.eType = tActorTarget.eType;
    tMessage.annTarget = ANNFromACD(tActorTarget.idACDTarget);
    tActorTarget.GetPlace(tMessage.tPlace);
    tMessage.snoPower = snoPower;
    tMessage.annItemBeingUsed = ANN_INVALID;
    tMessage.nComboLevel = 0;





    MessageSendToServer(CMSG_TARGET, &tMessage, sizeof(tMessage), ptPlayer);
}



BOOL CPlayerUsePowerOnTarget(
    const RActor * ptPlayerRActor,
    const SNO snoPower,
    const SNO snoOriginalPower,
    const ACDID idACDItemBeingUsed,
    const Target & tActorTarget,
    const BOOL bCancelingExistingPower,
    const SNO snoData0,
    const int32 nData1,

    const BOOL bNotifyActivate)


{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);

    BOOL bOneAttackPerClick = PowerPlayerAttackOnlyOncePerClick(AutoPowerDefinitionR(snoPower));

    if (ptPlayer->tClientInfo.snoCurrentLoopingPower != SNO_NONE)
    {

        DWordDataMessage tMessage;
        tMessage.dwData = ptPlayer->tClientInfo.snoCurrentLoopingPower;
        MessageSendToServer(CMSG_LOOPING_POWER_ABORTED, &tMessage, sizeof(tMessage), ptPlayer);

        ptPlayer->tClientInfo.snoCurrentLoopingPower = SNO_NONE;
    }

    ActorCommonData * ptACD = ACDTryToGet(ptPlayerRActor->idACD);
    if (!ptACD)
        return FALSE;



    PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(snoPower), ptPlayerRActor->idACD);

    if (ptPlayer->tClientInfo.snoCurrentChannelledPower != SNO_NONE &&
        ptPlayer->tClientInfo.snoCurrentChannelledPower != snoPower && eTemplate != SECONDARY_ANIMATION_POWER)
    {


        if (!sAbortChannelledPower(ptPlayer))
            return FALSE;
    }





    if (ptPlayer->tClientInfo.snoCurrentClientWalkPower != SNO_NONE &&
        ptPlayer->tClientInfo.snoCurrentClientWalkPower != snoPower && eTemplate != SECONDARY_ANIMATION_POWER)
    {


        if (!CPlayerInputAbortClientWalkPower(ptPlayer))
            return FALSE;
    }



    BOOL bClearCache = TRUE;

    if (PowerShouldUseClientWalking(*ptACD, AutoPowerDefinitionR(snoPower)))
    {
        sPrePlayWalkAndSendMessage(*ptPlayerRActor, *ptACD, tActorTarget, snoPower);
    }
    else if (eTemplate != PROC_POWER && eTemplate != SECONDARY_ANIMATION_POWER)
    {




        if (!(PowerCanMoveForwardBeforeContactFrame(AutoPowerDefinitionR(snoPower)) &&
              RActorAnimationIsAnimTreeEnabled(*ptPlayerRActor)))
        {

            ActorCommonData * ptACDTarget = (eTemplate == GIZMO_POWER) ? ACDTryToGet(tActorTarget.GetACD()) : NULL;
            if (!ptACDTarget || !ptACDTarget->IsItem())
            {

                if (ptPlayerRActor->ptCActorMovement)
                {
                    ptPlayerRActor->ptCActorMovement->StopNow();
                }
            }
        }

        sPlayerPlayAnimAndSendMessage(ptPlayerRActor, snoPower, snoOriginalPower, idACDItemBeingUsed, tActorTarget, bCancelingExistingPower, eTemplate, snoData0, nData1);
    }
    else if (eTemplate == SECONDARY_ANIMATION_POWER)
    {
        SecondaryAnimationPowerMessage tMessage;
        AnimReason eAnimReason = AnimPlayerGetAnimReason(*ptACD);

        if ((RActorAnimationIsAnimTreeEnabled(*ptPlayerRActor) &&
             (eAnimReason == ANIM_REASON_IDLE || eAnimReason == ANIM_REASON_WALKING || eAnimReason == ANIM_REASON_INTERRUPTABLE_ANIMATION || eAnimReason == ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH)) ||



            (!ptPlayerRActor->ptCActorMovement->IsMoving() &&
             (eAnimReason == ANIM_REASON_IDLE || eAnimReason == ANIM_REASON_INTERRUPTABLE_ANIMATION || eAnimReason == ANIM_REASON_INTERRUPTABLE_ANIMATION_NO_LOOKSWITCH)))
        {


            uint32 dwStartTime = CGameGetPredictedServerTime();
            BOOL bUseAlternateAnim = FALSE;

            if (PowerAlternatesAnims(*ptACD, AutoPowerDefinitionR(snoPower)))
            {
                uint32 lastAnimTypePlayed = 0;
                ptPlayer->tClientInfo.tCachedSnoStates.Lookup(snoPower, lastAnimTypePlayed);

                if (lastAnimTypePlayed == 0)
                {
                    bUseAlternateAnim = TRUE;
                    ptPlayer->tClientInfo.tCachedSnoStates.SetAt_Tracked(snoPower, 1);
                }
                else
                {
                    ptPlayer->tClientInfo.tCachedSnoStates.SetAt_Tracked(snoPower, 0);
                }
            }
            uint32 dwAnimationTag = PowerDetermineAnimation(AutoPowerDefinitionR(snoPower), ptPlayerRActor->idACD, cgTargetNone, bUseAlternateAnim);
            if (dwAnimationTag != (uint32)-1)
            {
                AnimationTiming tAnimationTiming;
                float flSpeedScalar = PowerGetAnimationSpeedForPower(ptACD->id, AutoPowerDefinitionR(snoPower), -1);
                AnimPlayerGetAnimationTiming(*ptACD, dwAnimationTag, (uint32)-1, flSpeedScalar, ptPlayer->tClientInfo.rngSyncedAnim, tAnimationTiming);
                AnimPlayerPlayAnimationsWithTiming(*ptACD, &tAnimationTiming, 1, FALSE, ANIM_REASON_INTERRUPTABLE_ANIMATION, FALSE, FALSE);
                AnimPreplayData tPreplayData;
                tPreplayData.dwSyncedSeed = RandGetSeed(ptPlayer->tClientInfo.rngSyncedAnim);
                tPreplayData.dwServerTimeAnimStarted = dwStartTime;

                tPreplayData.dwAnimPrePlayDataFlags = (1 << 0 /* PREPLAY_ANIM_FLAGS_PREPLAYED_ANIM */) | (bUseAlternateAnim ? (1 << 1 /* PREPLAY_ANIM_FLAGS_USE_ALT */) : 0);
                tMessage.tAnimPreplayData.Set(tPreplayData);
            }
        }

        tMessage.snoPower = snoPower;
        if (PowerRequiresActorTarget(*ptACD, AutoPowerDefinitionR(snoPower)) || PowerRequiresTarget(*ptACD, AutoPowerDefinitionR(snoPower)) || PowerCastTargetNecroCorpsesOnly(*ptACD, AutoPowerDefinitionR(snoPower)))
        {
            tMessage.annTarget = ANNFromACD(tActorTarget.GetACD());
        }
        else
        {
            tMessage.annTarget = ANN_INVALID;
        }
        MessageSendToServer(CMSG_SECONDARY_ANIMATION_POWER, &tMessage, sizeof(tMessage), ptPlayer);
        bClearCache = FALSE;
    }
    else
    {

        if (PowerRequiresTarget(*ptACD, AutoPowerDefinitionR(snoPower)) ||
            PowerRequiresActorTarget(*ptACD, AutoPowerDefinitionR(snoPower)))
        {
            TargetMessage tMessage;
            tMessage.eType = tActorTarget.eType;
            tMessage.annTarget = ANNFromACD(tActorTarget.idACDTarget);
            tActorTarget.GetPlace(tMessage.tPlace);
            tMessage.snoPower = snoPower;
            tMessage.annItemBeingUsed = ANN_INVALID;
            tMessage.nComboLevel = 0;






            CPlayerUtilSpawnPowerClickEffect(snoPower, tActorTarget);
            MessageSendToServer(CMSG_TARGET, &tMessage, sizeof(tMessage), ptPlayer);
        }
        else
        {
            DWordDataMessage tMessage;
            tMessage.dwData = snoPower;
            MessageSendToServer(CMSG_EXECUTE_NON_PRIMARY_POWER, &tMessage, sizeof(tMessage), ptPlayer);
            bClearCache = FALSE;
        }
    }


    if (bOneAttackPerClick)
    {
        ptPlayer->tClientInfo.snoCurrentPower = SNO_NONE;
        CPlayerInputCancelMode(PlayerGetByACD(ptACD->id), TRUE);
    }
    else
    {
        ptPlayer->tClientInfo.snoCurrentPower = snoPower;
    }

    if (!sPlayerPowerIsChannelled(ptPlayer))
    {




        PlayerGetCursorMgr().ResetCursorState(FALSE);




        if (bClearCache)
        {
            ptPlayer->tClientInfo.dwTimeInputWasCached = 0;
            ptPlayer->tClientInfo.tCachedTargetLock.Clear();
        }
    }

    if (bNotifyActivate)
    {
        LocalPlayerClamping::ClearInactiveFlagForController(ptPlayer->nControllerIndex, LocalPlayerClamping::AR_ABILITY_USED);
    }

    return TRUE;

}



static BOOL sPlayerPowerIsChannelled(const Player * ptPlayer)
{


    if (!ptPlayer)
        return FALSE;
    return ptPlayer->tClientInfo.snoCurrentChannelledPower != SNO_NONE;
}





static BOOL sPlayerPowerIsClientWalk(const Player * ptPlayer)
{


    if (!ptPlayer)
        return FALSE;
    return ptPlayer->tClientInfo.snoCurrentClientWalkPower != SNO_NONE;
}





static BOOL sCanAbortChannelledPower(
    const Player * ptPlayer)
{
    if (ptPlayer &&
        ptPlayer->tClientInfo.snoCurrentChannelledPower != SNO_NONE)
    {
        uint32 dwNow = CGameGetClientTicks();
        return ptPlayer->tClientInfo.dwMinChanneledDurationTime <= dwNow;
    }

    return TRUE;
}






static BOOL sPlayerCanWalk(
    const RActor & tPlayerRActor)
{
    ActorCommonData & tACDPlayer = ACDGet(tPlayerRActor.idACD);

    if (tACDPlayer.IsImmobilized())
        return FALSE;



    if (tACDPlayer.IsAttached())
        return FALSE;



    if (tACDPlayer.IsFeared())
        return FALSE;



    if (tACDPlayer.IsStunned())
        return FALSE;



    if (tACDPlayer.IsBlind())
        return FALSE;



    if (!tACDPlayer.IsAlive())
        return FALSE;



    if (tPlayerRActor.ptCActorMovement)
    {
        if (tPlayerRActor.ptCActorMovement->IsMoving() &&
            (tPlayerRActor.ptCActorMovement->m_uMoveFlags & (1 << MOVEBIT_REMOTE_MOVEMENT)))
            return FALSE;
    }




    if (CGameInScriptedSequence())
        return FALSE;




    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.tGameSyncedData.bPVPGameOver)
        return FALSE;










    Player * ptPlayer = PlayerGetByACD(tPlayerRActor.idACD);

    const uint32 dwNonPreplayedAnimWait = 15;
    if (ptPlayer->tClientInfo.dwTimeLastNonPreplayedAnimatingPower + dwNonPreplayedAnimWait > CGameGetClientTicks())
        return FALSE;



    return sCanAbortChannelledPower(ptPlayer);
}









static void sPlayerUpdateClosing(
    const RActor * ptPlayerRActor)
{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);



    if (ptPlayer->tClientInfo.tTargetLock.snoPower == SNO_NONE)
        return;





    BOOL bToCloseForPower =
        !sIsANormalWalkPower(ptPlayer->tClientInfo.tTargetLock.snoPower, ptPlayerRActor->idACD);




    BOOL bInControl = sPlayerCanInterruptHimself(ptPlayer, bToCloseForPower);
    if (!bInControl)
    {


        ptPlayer->tClientInfo.bClosing = FALSE;
        return;
    }

    if (!ptPlayerRActor->ptCActorMovement->IsMoving())
        return;





    if (ptPlayer->tClientInfo.tTargetLock.tTarget.IsInvalidForSource(ACDTryToGet(ptPlayerRActor->idACD)))
    {
        CPlayerInputCancelMode(ptPlayer, FALSE);
        return;
    }

    BOOL fAttacked;
    sHandleTargetLock(ptPlayerRActor, ptPlayer->tClientInfo.tTargetLock, FALSE, FALSE, fAttacked);
    if (fAttacked && ptPlayer->tClientInfo.nInputMode != 1)
    {




        if (ptPlayer->tClientInfo.nUIAction == -1 ||
            ptPlayer->tClientInfo.tTargetLock.tTarget.idACDTarget == ACDID_INVALID)
        {
            CPlayerInputCancelMode(ptPlayer, ptPlayer->tClientInfo.nInputMode == 3);
        }
    }
}


// NON_MATCHING: 7580 vs 7512 bytes; highlight/cached-input branch scheduling differs, UIID globals load through GOT and UIItemTag::GetRActor stays out of line; sDoHighlight's dwDebugFlags bit-20 test is a word load in the original, ours narrows it to ldrb (tried >>&1 and !=0 forms).
void PlayerInputUpdate(
    Player * ptPlayer)
{


    GameCommonData & tGCD = CGameGetGameCommonData();
    if (tGCD.GameModeGet() != GAMEMODE_PLAYING ||
        GameIsStartUp())
        return;



    CPlayerInputGlobals * ptCPlayerInputGlobals = CGameGet().ptCPlayerInputGlobals;

    RActorID idLocalPlayerRActor = ptPlayer->idPlayerRActor;
    const RActor * ptPlayerRActor = RActorTryToGet(idLocalPlayerRActor);
    if (!ptPlayerRActor)
        return;



    AppGlobals & tAppGlobals = sgtAppGlobals;


    if (tAppGlobals.GetTicksPerSec())
    {


        CGame & tClientGame = CGameGet();
        uint32 dwCurrTime = tClientGame.dwClientTicks;
        if (ptPlayer->tClientInfo.bQueuedHighlightForceUpdate ||
            (dwCurrTime & 3) == ptPlayer->nPlayerIndex)
        {


            sUpdateHighlight(ptPlayer);
            ptPlayer->tClientInfo.bQueuedHighlightForceUpdate = FALSE;
        }
    }
    else
    {



        while (!ptCPlayerInputGlobals->listHighlightedRActors.IsEmpty())
        {
            RActorID idRActor = (RActorID)ptCPlayerInputGlobals->listHighlightedRActors.GetHead();
            ptCPlayerInputGlobals->listHighlightedRActors.RemoveHead();

            sDoUnHighlight(idRActor, TRUE, TRUE);
        }
    }

    if (ptPlayer->tClientInfo.bClosing)
    {
        sPlayerUpdateClosing(ptPlayerRActor);
    }


    uint32 dwNow = CGameGetClientTicks();
    if (ptPlayer->tClientInfo.snoCurrentChannelledPower != SNO_NONE &&
        dwNow >= ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime)
    {

        sAbortChannelledPower(ptPlayer);
    }
    if (ptPlayer->tClientInfo.snoCurrentClientWalkPower != SNO_NONE &&
        dwNow >= ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime)
    {

        CPlayerInputAbortClientWalkPower(ptPlayer);
    }

    ScopedGlobalDefinitionLockR pdefGlobal;
    if (dwNow > ptPlayer->tClientInfo.dwTimeLastComboAttack + Math::FloatToInt(pdefGlobal->flComboClientTimeout * 60.0f))
    {
        CPlayerInputSetComboLevel(0, ptPlayer);
    }

    if (ptPlayer->tClientInfo.tCachedTargetLock.snoPower != SNO_NONE)
    {
        BOOL bClearCache = FALSE;
        BOOL bPowerCancelsOthers = PowerCancelsOtherPowers(AutoPowerDefinitionR(ptPlayer->tClientInfo.tCachedTargetLock.snoPower)) ||
            sIsANormalWalkPower(ptPlayer->tClientInfo.tCachedTargetLock.snoPower, ptPlayer->idPlayerACD);

        uint32 dwCacheTresholdTicks;
        if (bPowerCancelsOthers)
            dwCacheTresholdTicks = Math::FloatToInt(pdefGlobal->flPlayerPowerCacheTimeoutInterrupt * 60.0f);
        else
            dwCacheTresholdTicks = Math::FloatToInt(pdefGlobal->flPlayerPowerCacheTimeoutNormal * 60.0f);




        uint32 dwTicksSinceInputWasCached = dwNow - ptPlayer->tClientInfo.dwTimeInputWasCached;

        if (dwTicksSinceInputWasCached >= dwCacheTresholdTicks ||
            ptPlayer->tClientInfo.tCachedTargetLock.tTarget.IsInvalidForSource(ACDTryToGet(ptPlayerRActor->idACD)))
            bClearCache = TRUE;
        else {
            TargetLock tTargetLock = ptPlayer->tClientInfo.tCachedTargetLock;

            GameError eGameError = GAMEERROR_NONE;
            BOOL fAttacked = FALSE;


            if (tTargetLock.snoOriginalPower == GlobalSNOGet(GLOBALSNO_CONSOLE_EVADE))
            {
                WorldPlace tTargetPlace;
                tTargetLock.tTarget.GetPlace(tTargetPlace);
                fAttacked = CPlayerConsoleUseEvade(ptPlayer, tTargetPlace, TRUE);
            }
            else if (PowerIsGizmo(AutoPowerDefinitionR(tTargetLock.snoOriginalPower)))
            {

                ResolvedClick tResolvedClick;
                tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = tTargetLock.snoOriginalPower;
                tResolvedClick.tTarget = tTargetLock.tTarget;
                fAttacked = CPlayerConsoleUseSkill(ptPlayer, tResolvedClick, FALSE, FALSE);
            }
            else
            {
                ResolvedClick tResolvedClick;
                eGameError = CPlayerUtilResolveConsoleButton(tTargetLock.snoOriginalPower, tResolvedClick, ptPlayer, FALSE, TRUE, FALSE);
                fAttacked = tResolvedClick.snoPower != SNO_NONE && CPlayerConsoleUseSkill(ptPlayer, tResolvedClick, FALSE, FALSE);
            }










            if (fAttacked)
            {
                bClearCache = TRUE;
            }

            else if (bPowerCancelsOthers &&
                     eGameError != GAMEERROR_NONE &&
                     sPlayerCanInterruptHimselfAllowPowerCanceling(ptPlayer, tTargetLock.snoOriginalPower))
            {




                GameErrorData tErrorData(eGameError);
                UIGame::DisplayError(tErrorData, 0, ptPlayer);
                bClearCache = TRUE;
            }
        }




        if (bClearCache)
        {
            ptPlayer->tClientInfo.dwTimeInputWasCached = 0;
            ptPlayer->tClientInfo.tCachedTargetLock.Clear();
        }
    }

    if (ptPlayer->tClientInfo.bClosing && !ptPlayerRActor->ptCActorMovement->IsMoving())
    {
        if (ptPlayer->tClientInfo.dwClosingAbortTime)
        {
            const uint32 dwClosingAbortTimeThreshold = 30;
            if (CGameGetClientTicks() - ptPlayer->tClientInfo.dwClosingAbortTime > dwClosingAbortTimeThreshold)
            {
                if (ptPlayer->tClientInfo.nUIAction == -1 ||
                    ptPlayer->tClientInfo.tTargetLock.tTarget.idACDTarget == ACDID_INVALID)
                {
                    CPlayerInputCancelMode(ptPlayer, FALSE);
                }
            }
        }
        else
        {

            ptPlayer->tClientInfo.dwClosingAbortTime = CGameGetClientTicks();
        }
    }





}



SNO InputChooseFallbackPower(SNO snoPower, const Player * ptPlayer, GameError * peGameError)
{


    if (snoPower == SNO_NONE)
        return SNO_NONE;





    BOOL bCurrentlyChannelingPower = FALSE;
    if (ptPlayer->tClientInfo.snoCurrentChannelledPower == snoPower ||
        ptPlayer->tClientInfo.snoCurrentClientWalkPower == snoPower)
        bCurrentlyChannelingPower = TRUE;






    GameError eError;
    SNO snoPowerToUse = PowerPlayerFallbackIfNecessary(AutoPowerDefinitionR(snoPower), ptPlayer->idPlayerACD, eError, bCurrentlyChannelingPower);
    if (peGameError)
    {
        *peGameError = eError;
    }

    return snoPowerToUse;
}
static BOOL sPlayerShouldIgnoreClick(
    SNO snoPower,
    RActorID idRActorHit,
    const Player * ptPlayer)
{
    if (!ptPlayer)
        return TRUE;



    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO) && idRActorHit != RACTORID_INVALID)
    {
        const RActor * ptRActorHit = RActorTryToGet(idRActorHit);
        if (ptRActorHit)
        {
            ActorCommonData * ptACD = ACDTryToGet(ptRActorHit->idACD);
            if (ptACD)
            {
                GizmoType nGizmoType = ptACD->GetGizmoType();
                switch (nGizmoType)
                {
                case ACTORGIZMO_PORTAL: case ACTORGIZMO_TOWN_PORTAL:
                case ACTORGIZMO_PAGE_OF_FATE_PORTAL: case ACTORGIZMO_BOSS_PORTAL:
                case ACTORGIZMO_DUNGEON_PORTAL: case ACTORGIZMO_RETURN_PORTAL:


                    return CPlayerCanUsePortal();
                }
            }
        }
    }

    return FALSE;
}






static BOOL sHandleTargetLock(
    const RActor * ptPlayerRActor,
    const TargetLock & tTargetLock,
    const BOOL fCanCachePower,
    const BOOL fRefreshCacheTime,
    BOOL & fAttacked)
{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);
    fAttacked = FALSE;

    SNO snoPowerWalk = GlobalSNOGet(GLOBALSNO_POWERS_WALK);
    PowerTemplate eTemplate = PowerGetTemplate(AutoPowerDefinitionR(tTargetLock.snoPower), ptPlayerRActor->idACD);
    if (eTemplate == WALK_POWER)
    {
        if (!sPlayerCanWalk(*ptPlayerRActor))
            return FALSE;


        if (fCanCachePower)
        {
            if (ptPlayer->tClientInfo.eCurrentMessage == INPUTMSG_LEFTMOUSEDOWN || ptPlayer->tClientInfo.eCurrentMessage == INPUTMSG_LEFTMOUSEDOUBLECLICK)
            {
                CPlayerUtilSpawnClickEffect(tTargetLock.tTarget);
            }
            else
            if (ptPlayer->tClientInfo.nUIAction == ACTION_UI_WALK && tTargetLock.bIgnoreRange)
            {
                CPlayerUtilSpawnClickEffect(tTargetLock.tTarget);
            }
        }


        sWalkToPoint(ptPlayerRActor, tTargetLock.snoPower, tTargetLock.snoOriginalPower, tTargetLock.tTarget, 0.0f, 0.0f, FALSE, fCanCachePower, fRefreshCacheTime, tTargetLock.bIgnoreRange, 0);
        return TRUE;
    }






    BOOL bHoldPositionAndAttack = CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, FALSE, FALSE) || tTargetLock.bIgnoreRange;


    if (tTargetLock.snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO) ||
        tTargetLock.snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_NPC))
        bHoldPositionAndAttack = FALSE;



    if (!bHoldPositionAndAttack &&
        CPlayerUtilNeedsToGetCloser(ptPlayerRActor, tTargetLock.snoPower, tTargetLock.tTarget, 0.0f, 0.0f) &&
        PowerWalksCloserIfOutOfRange(AutoPowerDefinitionR(tTargetLock.snoPower)))
    {
        if (!sPlayerCanWalk(*ptPlayerRActor))
            return FALSE;


        if (fCanCachePower)
        {
            if (!ptPlayer->tClientInfo.bClosing)
            {
                if (ptPlayer->tClientInfo.eCurrentMessage == INPUTMSG_LEFTMOUSEDOWN || ptPlayer->tClientInfo.eCurrentMessage == INPUTMSG_LEFTMOUSEDOUBLECLICK)
                {

                    CPlayerUtilSpawnClickEffect(tTargetLock.tTarget);
                }
            }
        }

        WorldDistance wdMaxRangeFromDest = CPlayerUtilGetRangeFromDest(ptPlayerRActor->idACD, tTargetLock.snoPower, tTargetLock.tTarget);
        sWalkToPoint(ptPlayerRActor, snoPowerWalk, tTargetLock.snoOriginalPower, tTargetLock.tTarget, wdMaxRangeFromDest, 0.0f, FALSE, fCanCachePower, fRefreshCacheTime, tTargetLock.bIgnoreRange, 0);

        ptPlayer->tClientInfo.bClosing = TRUE;
        ptPlayer->tClientInfo.dwClosingAbortTime = 0;
        return TRUE;
    }



    BOOL bCancelingExistingPower = FALSE;
    BOOL fCanUse = CPlayerCanUsePowerOnTarget(ptPlayerRActor, tTargetLock.snoPower, tTargetLock.snoOriginalPower, tTargetLock.tTarget, bHoldPositionAndAttack, fCanCachePower, fRefreshCacheTime, bCancelingExistingPower, FALSE);
    if (fCanUse)
    {
        CPlayerUsePowerOnTarget(ptPlayerRActor, tTargetLock.snoPower, tTargetLock.snoOriginalPower, tTargetLock.idACDItemToUse, tTargetLock.tTarget, bCancelingExistingPower, SNO_NONE, -1, TRUE);
        fAttacked = TRUE;
        return TRUE;
    }


    return FALSE;
}




static void sHandleResolvedClick(
    const RActor * ptPlayerRActor,
    ResolvedClick & tResolvedClick,
    const BOOL fCanCachePower,
    const BOOL bFromHotkey,
    const BOOL bPrimaryActionKey)
{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);
    IVector2D pointMouseToUse = InputGetCurrentState(INPUTCONTEXT_DEFAULT).tMouseState.p;

    if (!sIsCursorInMainViewport())
    {
        IRect2D rectViewportPixels;
        CGameGetLetterboxedViewportPixels(rectViewportPixels);

        pointMouseToUse.x = Math::Pin<int, int>(pointMouseToUse.x, 0, rectViewportPixels.Width());
        pointMouseToUse.y = Math::Pin<int, int>(pointMouseToUse.y, 0, rectViewportPixels.Height());
    }

    if (tResolvedClick.idACDItemToUse != ACDID_INVALID)
    {

        CursorMgr & tCursorMgr = PlayerGetCursorMgr();

        ActorCommonData & tACDItemToUse = ACDGet(tResolvedClick.idACDItemToUse);


        ItemUseType eItemUseType = ItemGetUseType(tACDItemToUse);
        if (ItemUseIsTargettedInInventory(eItemUseType))
        {
            tCursorMgr.ResetCursorState(FALSE);
            ptPlayer->tClientInfo.nUIAction = ACTION_INVALID;
            ptPlayer->tClientInfo.nInputMode = INPUT_MODE_NORMAL;
            return;
        }

        SNO snoUsePower = ItemGetUsePower(tACDItemToUse.hGB.gbid);
        if (PowerIsPrimary(AutoPowerDefinitionR(snoUsePower), tACDItemToUse.id))
        {
            tResolvedClick.snoPower = snoUsePower;
        }
        else
        {
            Target tTarget;
            CPlayerUtilGetTarget(ptPlayer, tResolvedClick.idRActor, pointMouseToUse, SNO_NONE, tTarget, &tResolvedClick);
            CACDInventoryRequestUse(tACDItemToUse, tTarget, tResolvedClick.bPlayItemSound, NULL);
            tCursorMgr.ResetCursorState(FALSE);
            return;
        }
    }

    if (tResolvedClick.snoPower == SNO_NONE || sPlayerShouldIgnoreClick(tResolvedClick.snoPower, tResolvedClick.idRActor, ptPlayer))
    {
        if (!bFromHotkey)
        {
            ptPlayer->tClientInfo.nUIAction = ACTION_INVALID;
            ptPlayer->tClientInfo.nInputMode = INPUT_MODE_NORMAL;
        }
        return;
    }


    Target tTarget;
    ACDID idACDPlayer = ptPlayerRActor->idACD;
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    if (bFromHotkey && !PowerRequiresTarget(tACDPlayer, AutoPowerDefinitionR(tResolvedClick.snoPower)))
    {


        tTarget.InitializeFromPlace(tACDPlayer.GetPlace());

        tTarget = tResolvedClick.tTarget;
    }
    else
    {

        CPlayerUtilGetTarget(ptPlayer, tResolvedClick.idRActor, pointMouseToUse, tResolvedClick.snoPower, tTarget, &tResolvedClick);
    }


    TargetLock tTargetLock;
    tTargetLock.Set(tResolvedClick.snoOriginalPower, tResolvedClick.snoPower, tTarget, tResolvedClick.idACDItemToUse, FALSE);



    if (PowerExtendsTargetToAttackRadius(AutoPowerDefinitionR(tTargetLock.snoPower)))
    {
        WorldPlace placeTarget;
        tTargetLock.tTarget.GetPlace(placeTarget);

        if (PowerExtendTargetToPowerAttackRadius(ptPlayerRActor->idACD, AutoPowerDefinitionR(tTargetLock.snoPower), placeTarget))
        {
            tTargetLock.tTarget.InitializeFromPlace(placeTarget);
        }
    }

    if (PowerIsPrimary(AutoPowerDefinitionR(tResolvedClick.snoOriginalPower), ptPlayerRActor->idACD))
    {
        ptPlayer->tClientInfo.tTargetLock = tTargetLock;
    }






    if (ptPlayer->tClientInfo.nUIAction != ACTION_UI_LEFT_MOUSE_BUTTON && tTarget.eType == TT_POINT)
    {

        if (PowerCanSteer(AutoPowerDefinitionR(tResolvedClick.snoPower)) && !CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, bPrimaryActionKey, tTarget.idACDTarget != ACDID_INVALID))
        {

            SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_WALK);
            if (!sPlayerCanWalk(*ptPlayerRActor))
                return;

            if (ptPlayer->tClientInfo.snoCurrentPower != snoPower && ptPlayer->tClientInfo.eCurrentMessage != INPUTMSG_KEYSTILLDOWN)
            {
                CPlayerUtilSpawnClickEffect(tTarget);
            }
            sWalkToPoint(ptPlayerRActor, snoPower, tTargetLock.snoOriginalPower, tTarget, 0.0f, 0.0f, FALSE, fCanCachePower, fCanCachePower, tTargetLock.bIgnoreRange, 0);
            return;
        }
    }



    if (tTargetLock.tTarget.IsInvalidForSource(ACDTryToGet(ptPlayerRActor->idACD)))
    {
        if (!bFromHotkey)
        {
            CPlayerInputCancelMode(ptPlayer, FALSE);
        }
        return;
    }

    BOOL fAttacked;
    sHandleTargetLock(ptPlayerRActor, tTargetLock, fCanCachePower, fCanCachePower, fAttacked);
}





BOOL CPlayerInputClickToPickupRActor(
    const RActor * ptPlayerRActor,
    const RActorID idRActorHit)
{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);
    const RActor * ptRActor = RActorTryToGet(idRActorHit);
    if (!ptRActor)
        return FALSE;



    ActorCommonData * ptACD = ACDTryToGet(ptRActor->idACD);
    if (!ptACD)
        return FALSE;



    GizmoType nGizmoType = ptACD->GetGizmoType();
    if (nGizmoType != ACTORGIZMO_ITEM_INTERNAL)
        return FALSE;





    if (ItemIsAutoPickup(*ptACD))
        return FALSE;




    if (PlayerGetCursorMgr().GetCursorState() != CURSORSTATE_NORMAL)
        return FALSE;




    if (!CPlayerInputCancelMode(ptPlayer, FALSE))
        return FALSE;



    ptPlayer->tClientInfo.nUIAction = ACTION_UI_LEFT_MOUSE_BUTTON;

    ResolvedClick tResolvedClick;
    tResolvedClick.idRActor = idRActorHit;
    tResolvedClick.snoOriginalPower = tResolvedClick.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO);
    sHandleResolvedClick(ptPlayerRActor, tResolvedClick, FALSE, FALSE, FALSE);

    return TRUE;
}




void CPlayerInputClickWithPower(
    const RActor * ptPlayerRActor,
    UIAction eAction,
    SNO snoPower,
    BOOL bFromHotkey)
{
    if (UIMap::IsOpen() || UIWaypointMap::IsOpen())
        return;



    if (eAction == ACTION_UI_WALK && sPlayerPowerIsChannelled(PlayerGetByACD(ptPlayerRActor->idACD)))
        return;






    sPlayerButtonClick(ptPlayerRActor, eAction, snoPower, bFromHotkey, FALSE);
}



static void sPlayerLeftClick(
    const RActor * ptPlayerRActor,
    Player * ptPlayer,
    BOOL bHeldButton)
{
    if (!CPlayerInputCancelMode(ptPlayer, FALSE))
        return;






    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    CursorState eCursorState = tCursorMgr.GetCursorState();
    if (eCursorState == CURSORSTATE_QUEUED_SKILL)
    {
        SNO snoQueuedPower = tCursorMgr.GetQueuedSkill();



        ptPlayer->tClientInfo.snoLastCursorQueuedPower = snoQueuedPower;
        CPlayerInputClickWithPower(ptPlayerRActor, ACTION_UI_LEFT_MOUSE_BUTTON, snoQueuedPower, FALSE);
        tCursorMgr.ResetCursorState(FALSE);
        return;
    }
    if (eCursorState == CURSORSTATE_MOVING_HOTBAR_BUTTON)
    {


        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer && PlayerCanChangeSkillorTrait(*ptACDPlayer, FALSE))
        {
            HotbarButtonData tButtonToPlace;
            SkillButton eSkillButtonToMove;
            tCursorMgr.GetHotbarButtonToMove(tButtonToPlace, eSkillButtonToMove);

            if (PowerIsCoolDownDone(*ptACDPlayer, tButtonToPlace.m_snoPower))
            {

                HotbarButtonData tEmptyButtonData;
                tEmptyButtonData.Empty();
                PlayerSendSkillChangeToServer(eSkillButtonToMove, tEmptyButtonData, ptPlayer);
            }
        }


        tCursorMgr.ResetCursorState(FALSE);
        ptPlayer->tClientInfo.snoLastCursorQueuedPower = SNO_NONE;
        return;
    }

    ptPlayer->tClientInfo.snoLastCursorQueuedPower = SNO_NONE;

    ptPlayer->tClientInfo.nUIAction = ACTION_UI_LEFT_MOUSE_BUTTON;
    ptPlayer->tClientInfo.dwGameTickOfLastAction[ACTION_UI_LEFT_MOUSE_BUTTON] = CGameGetClientTicks();
    sPlayerRememberButtonClickForHeldAction(ptPlayer, ACTION_UI_LEFT_MOUSE_BUTTON, TRUE);

    const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, 0);
    ResolvedClick tResolvedClick;
    CPlayerUtilResolveClick(ptPlayer, tButtonData.m_snoPower, FALSE, tResolvedClick);

    sHandleResolvedClick(ptPlayerRActor, tResolvedClick, !bHeldButton, FALSE, FALSE);
}






static BOOL sUpdateCachedStickInfo(
    const RActor * ptPlayerRActor,
    Player * ptPlayer,
    BOOL bRightStick,
    Vector2D & pScreen)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;



    WorldPlace wpPlayer = ptACDPlayer->GetPlace();

    const float flClickDistance = 50.0f;
    if (!WorldPointToScreenPoint(wpPlayer.wp, pScreen, -1))
        return FALSE;



    Vector2D vStick = bRightStick ? Console::ControllerUtils::GetRStickPosition(ptPlayer->nControllerIndex) : Console::ControllerUtils::GetLStickPosition(ptPlayer->nControllerIndex, TRUE);
    BOOL bInDeadZone = bRightStick ? Console::ControllerUtils::RStickIsInDeadZone(ptPlayer->nControllerIndex) : Console::ControllerUtils::LStickIsInDeadZone(ptPlayer->nControllerIndex, TRUE);

    if (!bInDeadZone)
    {
        pScreen.x += (int32)(vStick.x * flClickDistance);
        pScreen.y -= (int32)(vStick.y * flClickDistance);

        if (bRightStick)
        {
            Console::ControllerUtils::SetRStickScreenOffset(ptPlayer->nControllerIndex, (int32)pScreen.x, (int32)pScreen.y);
        }
        else
        {
            Console::ControllerUtils::SetLStickScreenOffset(ptPlayer->nControllerIndex, (int32)pScreen.x, (int32)pScreen.y);
        }

        LineSeg3D tLineSeg;

        ScreenPointToWorldLineSeg(bRightStick ? Console::ControllerUtils::GetRStickScreenOffset(ptPlayer->nControllerIndex) : Console::ControllerUtils::GetLStickScreenOffset(ptPlayer->nControllerIndex), tLineSeg, -1);

        Plane tPlayerPlane;
        tPlayerPlane.Set(wpPlayer.wp, cgwvZAxis);

        Vector3D tIntersection;

        if (Intersect::LineSeg3DAndPlane(tLineSeg, tPlayerPlane, tIntersection))
        {
            if (bRightStick)
            {
                Console::ControllerUtils::SetRStickWorldDirection(ptPlayer->nControllerIndex, tIntersection.x - wpPlayer.wp.x, tIntersection.y - wpPlayer.wp.y);
            }
            else
            {
                Console::ControllerUtils::SetLStickWorldDirection(ptPlayer->nControllerIndex, tIntersection.x - wpPlayer.wp.x, tIntersection.y - wpPlayer.wp.y);
            }
        }
        return TRUE;
    }
    else
    {

        Angle aYaw = ptPlayerRActor->ptCActorMovement->GetDesiredYaw();
        Vector2D vDir(Math::Cos(aYaw), Math::Sin(aYaw));

        if (bRightStick)
        {
            Console::ControllerUtils::SetRStickWorldDirection(ptPlayer->nControllerIndex, vDir.x, vDir.y);
        }
        else
        {
            Console::ControllerUtils::SetLStickWorldDirection(ptPlayer->nControllerIndex, vDir.x, vDir.y);
        }
    }


    return FALSE;

}


float CPlayerGetMoveSpeedForStickInput(ActorCommonData & tACDPlayer, Player & tPlayer, SNO snoPower, float flMinClientWalkSpeed)
{




    if (!PlayerIsLocal(&tPlayer))
        return 0.0f;




    ScopedGlobalDefinitionLockR pdefGlobal;
    float flFacingRange = pdefGlobal->flControllerMoveFacingZone;
    float flMinSpeed = pdefGlobal->flControllerMinMoveSpeed;
    float flMaxRange = pdefGlobal->flControllerMaxMoveThreshold;


    float flController = Console::ControllerUtils::GetLStickPosition(tPlayer.nControllerIndex, TRUE).Magnitude();
    flController -= flFacingRange;
    if (flMinClientWalkSpeed <= 0.0f && flController < 0.0f)
        return 0.0f;




    flController /= (flMaxRange - flFacingRange);
    flController = Math::Pin(flController, 0.0f, 1.0f);
    flMinSpeed += (1.0f - flMinSpeed) * flController;

    float flSpeed = tACDPlayer.GetWalkingSpeed(TAG_ACTOR_ANIMATION_RUN);
    if (snoPower != SNO_NONE)
    {

        float flSpeedMultiplier = Math::Pin(flMinClientWalkSpeed, 0.0f, 1.0f);
        flMinSpeed = Math::Max(flMinSpeed, flSpeedMultiplier);

        flSpeedMultiplier = PowerGetFormulaValue(&tACDPlayer, AutoPowerDefinitionR(snoPower), 0x510b0, -1, -1, 0);
        flSpeed *= flSpeedMultiplier;
    }


    return flMinSpeed * flSpeed;
}




static void sPlayerRightStick(
    const RActor * ptPlayerRActor,
    Player * ptPlayer)
{
    Vector2D vScreen;
    if (sUpdateCachedStickInfo(ptPlayerRActor, ptPlayer, TRUE, vScreen))
        CPlayerConsoleTryEvade(ptPlayer);
}






static void sPlayerGestureFlick(
    const RActor * ptPlayerRActor,
    Player * ptPlayer)
{
    CPlayerConsoleTryGestureEvade(ptPlayer);
}


static void sPlayerLeftStick(
    const RActor * ptPlayerRActor,
    Player * ptPlayer)
{

    BOOL bChanneling = sPlayerPowerIsChannelled(ptPlayer);
    BOOL bClientWalking = sPlayerPowerIsClientWalk(ptPlayer);
    if (!bChanneling && !bClientWalking && ptPlayer->tClientInfo.snoCurrentPower == SNO_NONE)
    {

        if (!CPlayerInputCancelMode(ptPlayer, FALSE))
            return;
    }



    BOOL bInControl = sPlayerCanInterruptHimself(ptPlayer, TRUE) && sPlayerCanWalk(*ptPlayerRActor);

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    WorldPlace wpPlayer = tACDPlayer.GetPlace();

    Vector2D pScreen;
    if (!sUpdateCachedStickInfo(ptPlayerRActor, ptPlayer, FALSE, pScreen) && !bClientWalking)
    {
        if (bInControl)
        {
            ptPlayerRActor->ptCActorMovement->StopNow();




            if (ptPlayer->tTargetComponent.GetLockedTargetType() == ConsoleTargetComponent::TARGETLOCK_SOFT)
            {
                WorldPlace wpPlaceToFace = wpPlayer;
                const Vector2D & vDirection = ptPlayer->tTargetComponent.GetLockedStickDirection();
                Vector3D vOffset(vDirection.x, vDirection.y, 0.0f);
                wpPlaceToFace.wp += vOffset;
                tACDPlayer.SetFacing(wpPlaceToFace, TRUE);
                ptPlayerRActor->ptCActorMovement->SendFacingToServer(TRUE);
            }


            else if (!ptPlayer->tTargetComponent.GetInAimMode())
            {

                const ActorCommonData * ptACDLockedTarget = const_cast<ConsoleTargetComponent &>(ptPlayer->tTargetComponent).ResolveLockedTarget();
                if (ptACDLockedTarget)
                {
                    tACDPlayer.SetFacing(ptACDLockedTarget->GetPlace(), TRUE);
                    ptPlayerRActor->ptCActorMovement->SendFacingToServer(TRUE);
                }
            }
        }
        return;
    }

    ScopedGlobalDefinitionLockR pdefGlobal;
    if (bInControl)
    {



        switch (ptPlayer->tTargetComponent.GetLockedTargetType())
        {
        case ConsoleTargetComponent::TARGETLOCK_SOFT:
            ptPlayer->tTargetComponent.ClearLockedTarget();
            break;
        case ConsoleTargetComponent::TARGETLOCK_PERSISTENT:
        {

            float flStickDelta = Vector2D::DotProduct(ptPlayer->tTargetComponent.GetLockedStickDirection(), ptPlayer->tTargetComponent.GetTargetingDirection());
            Angle aRequiredAngle = Math::DegToAngle(pdefGlobal->flControllerPersistenceStickAngle) * 0.5f;
            float flCosRequiredTheta = Math::Cos(aRequiredAngle);
            if (flStickDelta < flCosRequiredTheta)
                ptPlayer->tTargetComponent.ClearLockedTarget();
            break;
        }
        default: break;
        }
    }
    if (bChanneling)
        return;







    float flFacingRange = pdefGlobal->flControllerMoveFacingZone;
    BOOL bDontMove = Console::ControllerUtils::GetLStickPosition(ptPlayer->nControllerIndex, TRUE).MagnitudeSquared() < flFacingRange * flFacingRange;
    if (!bInControl || (!bClientWalking && (bDontMove || CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, FALSE, FALSE))))
    {











        SNO snoPowerToUse = sPlayerCanWalk(*ptPlayerRActor) ? GlobalSNOGet(GLOBALSNO_POWERS_WALK) : SNO_NONE;
        if (ptPlayer->tClientInfo.snoCurrentAnimPower != SNO_NONE)
            snoPowerToUse = ptPlayer->tClientInfo.snoCurrentAnimPower;
        if (snoPowerToUse == SNO_NONE || !sPlayerCanInterruptHimselfAllowPowerCanceling(ptPlayer, snoPowerToUse))
            return;



        WorldPlace wpPlaceToFace = wpPlayer;
        if (ptPlayer->tTargetComponent.GetInReticleMode())
        {

            Vector2D vReticle;
            ptPlayer->tTargetComponent.GetTargetingReticle(vReticle);
            Vector3D vOffset(vReticle.x, vReticle.y, 0.0f);
            wpPlaceToFace.wp += vOffset;
        }
        else
        {
            Vector3D vMoveOffset(Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).x, Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).y, 0.0f);
            wpPlaceToFace.wp += vMoveOffset;
        }
        tACDPlayer.SetFacing(wpPlaceToFace, FALSE);
        ptPlayerRActor->ptCActorMovement->SendFacingToServer(FALSE);
        ptPlayerRActor->ptCActorMovement->StopNow();
        return;
    }
    else
    {




        SNO snoPower = bClientWalking ? ptPlayer->tClientInfo.snoCurrentClientWalkPower : GlobalSNOGet(GLOBALSNO_POWERS_WALK);

        Target tTarget;
        float flSpeed = 0.0f;
        float flMinClientWalkSpeed = 0.0f;
        if (bClientWalking)
        {


            flMinClientWalkSpeed = PowerGetFormulaValue(&tACDPlayer, AutoPowerDefinitionR(snoPower), TAG_POWER_CONSOLE_MIN_CLIENT_WALK_SPEED, -1, -1, 0);
        }
        if (bClientWalking && flMinClientWalkSpeed <= 0.0f && bDontMove)
        {
            tTarget.InitializeFromPlace(wpPlayer);
        }
        else
        {










            WorldPlace tMovePlace = wpPlayer;
            Vector3D vMoveOffset(Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).x, Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).y, tACDPlayer.GetForward().z);

            vMoveOffset.Normalize();
            tMovePlace.wp += vMoveOffset;
            tTarget.InitializeFromPlace(tMovePlace);
            if (tTarget.IsInvalid())
                return;
            flSpeed = CPlayerGetMoveSpeedForStickInput(tACDPlayer, *ptPlayer, snoPower, flMinClientWalkSpeed);
        }




        // Inference (rows 3505/3507/3511): two sWalkToPoint calls; the client-walk one re-reads snoCurrentClientWalkPower
        // (original reloads +0xd418 after CPlayerGetMoveSpeedForStickInput) and passes TRUE/20, the other passes snoPower/FALSE/0.
        if (!bClientWalking)
        {
            sWalkToPoint(ptPlayerRActor, snoPower, snoPower, tTarget, 0.0f, flSpeed, FALSE, FALSE, FALSE, FALSE, 0);
        }
        else
        {
            sWalkToPoint(ptPlayerRActor, ptPlayer->tClientInfo.snoCurrentClientWalkPower, ptPlayer->tClientInfo.snoCurrentClientWalkPower, tTarget, 0.0f, flSpeed, TRUE, FALSE, FALSE, FALSE, 20);
        }
    }
}


static void sPlayerButtonClick(
    const RActor * ptPlayerRActor,
    UIAction eAction,
    SNO snoPower,
    BOOL bFromHotkey,
    BOOL bHeldButton)
{
    Player * ptPlayer = PlayerGetByACD(ptPlayerRActor->idACD);
    BOOL bSecondary = FALSE;

    if (PowerGetTemplate(AutoPowerDefinitionR(snoPower), ptPlayerRActor->idACD) == SECONDARY_ANIMATION_POWER)
        bSecondary = TRUE;




    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    if (tCursorMgr.GetCursorState() != CURSORSTATE_NORMAL &&
        eAction != ACTION_UI_LEFT_MOUSE_BUTTON)
    {
        tCursorMgr.ResetCursorState(FALSE);
        return;
    }


    if (ptPlayer->tClientInfo.snoCurrentChannelledPower == snoPower ||
        ptPlayer->tClientInfo.snoCurrentClientWalkPower == snoPower)
        return;



    if (PowerIsPrimary(AutoPowerDefinitionR(snoPower), ptPlayerRActor->idACD) && !bSecondary)
    {



        if (!CPlayerInputCancelMode(ptPlayer, FALSE))
            return;


        ptPlayer->tClientInfo.nUIAction = eAction;
    }
    else
    {

        if (PowerGetTemplate(AutoPowerDefinitionR(snoPower), ptPlayerRActor->idACD) != WALK_POWER)
        {
            const int32 SECONDARY_ANIMATION_POWER_MIN_INTERVAL = 30;
            if (CGameGetClientTicks() - ptPlayer->tClientInfo.dwGameTickOfLastActionSecondaryPowerCast[eAction] < SECONDARY_ANIMATION_POWER_MIN_INTERVAL)
                return;
        }
    }



    if (eAction != ACTION_INVALID)
    {


        ptPlayer->tClientInfo.dwGameTickOfLastAction[eAction] = CGameGetClientTicks();
        ptPlayer->tClientInfo.dwGameTickOfLastActionSecondaryPowerCast[eAction] = CGameGetClientTicks();
        sPlayerRememberButtonClickForHeldAction(ptPlayer, eAction, TRUE);
    }

    ResolvedClick tResolvedClick;
    tResolvedClick.snoOriginalPower = snoPower;

    if (snoPower != SNO_NONE)
    {
        tResolvedClick.snoPower = InputChooseFallbackPower(snoPower, ptPlayer, NULL);
        tResolvedClick.idRActor = CPlayerUtilPickRActorForPower(ptPlayer, tResolvedClick.snoPower);
    }































    sHandleResolvedClick(ptPlayerRActor, tResolvedClick, !bHeldButton, bFromHotkey, FALSE);
}





static void sHoldPositionAndAttackTowardsCursor(
    const RActor * ptPlayerRActor,
    const BufferedInput & tInput,
    SNO snoPower,
    SNO snoOriginalPower)
{
    if (snoPower == SNO_NONE)
        return;



    const IVector2D & pointMouse = tInput.tBufMouseData.p;
    Target tTarget;

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);




    if (PowerLocksActorsWhileSweeping(AutoPowerDefinitionR(snoPower)))
    {
        RActorID idRActorTarget = CPlayerUtilPickRActorForPower(ptControllingPlayer, snoPower);
        CPlayerUtilGetTarget(ptControllingPlayer, idRActorTarget, pointMouse, snoPower, tTarget, NULL);

        if (PowerStickyTargetsActorsWhileSweeping(AutoPowerDefinitionR(snoPower)))
        {
            if (tTarget.IsTargetingACD())
            {
                ptControllingPlayer->tClientInfo.nInputMode = 1;
                ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower = snoPower;
                ptControllingPlayer->tClientInfo.tTargetLock.snoPower = snoPower;
                ptControllingPlayer->tClientInfo.tTargetLock.tTarget = tTarget;
            }
        }
    }
    else
    {
        CPlayerUtilGetTarget(ptControllingPlayer, RACTORID_INVALID, pointMouse, snoPower, tTarget, NULL);
    }

    if (ptControllingPlayer->tClientInfo.nInputMode == 1 &&
        ptControllingPlayer->tClientInfo.tTargetLock.tTarget.IsTargetingACD())
    {
        ActorCommonData * ptACDTargetLock = ACDTryToGet(ptControllingPlayer->tClientInfo.tTargetLock.tTarget.GetACD());
        if (!ptACDTargetLock || !ptACDTargetLock->IsAlive())
        {
            ptControllingPlayer->tClientInfo.nInputMode = 0;
            ptControllingPlayer->tClientInfo.tTargetLock.tTarget.Clear();
        }
    }

    BOOL bCancelingExistingPower = FALSE;
    BOOL bRequireRangeModifiers = FALSE;
    SNO snoCurrentChanelledPower = ptControllingPlayer->tClientInfo.snoCurrentChannelledPower;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptControllingPlayer->idPlayerACD);
    if (!Prefs::CheckAction(ACTION_UI_HOLD_POSITION, FALSE) && ptACDPlayer &&
        PowerIsChannelled(*ptACDPlayer, AutoPowerDefinitionR(snoPower)) &&
        (snoCurrentChanelledPower == SNO_NONE || snoCurrentChanelledPower != snoPower))
        bRequireRangeModifiers = TRUE;


    BOOL fCanUse = CPlayerCanUsePowerOnTarget(ptPlayerRActor, snoPower, snoOriginalPower, tTarget, TRUE, FALSE, TRUE, bCancelingExistingPower, bRequireRangeModifiers);
    if (fCanUse && !sIsANormalWalkPower(snoPower, ptPlayerRActor->idACD))
    {
        CPlayerUsePowerOnTarget(ptPlayerRActor, snoPower, snoOriginalPower, ACDID_INVALID, tTarget, bCancelingExistingPower, SNO_NONE, -1, TRUE);
    }
    else if (sPlayerPowerIsChannelled(ptControllingPlayer))
    {
        sAbortChannelledPower(ptControllingPlayer);
    }
}





static void sHandleMoveLock(
    const RActor * ptPlayerRActor,
    const BufferedInput & tInput)
{
    const IVector2D & pointMouse = tInput.tBufMouseData.p;

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);




    BOOL bHoldPositionAndAttack = CPlayerUtilPlayerSpecifiedDontMove(ptControllingPlayer, FALSE, FALSE);
    if (bHoldPositionAndAttack)
    {

        const Player * ptPlayer = ptControllingPlayer;
        const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, 0);
        SNO snoPower = InputChooseFallbackPower(tButtonData.m_snoPower, ptPlayer, NULL);
        sHoldPositionAndAttackTowardsCursor(ptPlayerRActor, tInput, snoPower, tButtonData.m_snoPower);
    }
    else
    {


        SNO snoPower = GlobalSNOGet(GLOBALSNO_POWERS_WALK);

        Target tTarget;
        CPlayerUtilGetTarget(ptControllingPlayer, RACTORID_INVALID, pointMouse, snoPower, tTarget, NULL);
        if (sPlayerCanWalk(*ptPlayerRActor))
            sWalkToPoint(ptPlayerRActor, snoPower, snoPower, tTarget, 0.0f, 0.0f, FALSE, FALSE, FALSE, FALSE, 0);
    }
}







static void sHandleActorTargetLock(
    const RActor * ptPlayerRActor,
    const BufferedInput & tInput)
{

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);





    ptControllingPlayer->tClientInfo.dwTimeInputWasCached = CGameGetClientTicks();


    BOOL bInControl = sPlayerCanInterruptHimself(PlayerGetByACD(ptPlayerRActor->idACD), FALSE);
    if (!bInControl)
        return;



    ActorCommonData * ptACDPlayer = ACDTryToGet(ptControllingPlayer->idPlayerACD);
    if (ptACDPlayer && PowerIsChannelled(*ptACDPlayer, AutoPowerDefinitionR(ptControllingPlayer->tClientInfo.tTargetLock.snoPower)))
    {
        sHoldPositionAndAttackTowardsCursor(ptPlayerRActor, tInput, ptControllingPlayer->tClientInfo.tTargetLock.snoPower, ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower);
        return;
    }

    if (ptControllingPlayer->tClientInfo.tTargetLock.tTarget.IsInvalidForSource(ACDTryToGet(ptPlayerRActor->idACD)))
    {
        BOOL bHoldPositionAndAttack = CPlayerUtilPlayerSpecifiedDontMove(ptControllingPlayer, FALSE, FALSE);
        if (bHoldPositionAndAttack)
        {

            sHoldPositionAndAttackTowardsCursor(ptPlayerRActor, tInput, ptControllingPlayer->tClientInfo.tTargetLock.snoPower, ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower);
            return;
        }
    }
    else
    {



        BOOL fAttacked;
        if (sHandleTargetLock(ptPlayerRActor, ptControllingPlayer->tClientInfo.tTargetLock, FALSE, TRUE, fAttacked))
            return;
    }


    CPlayerInputCancelMode(ptControllingPlayer, FALSE);



}










































static void sLeftHold(
    const RActor * ptPlayerRActor,
    const BufferedInput & tInput,
    SNO snoPower)
{

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
    if (ptControllingPlayer->tClientInfo.nUIAction != ACTION_UI_LEFT_MOUSE_BUTTON)
        return;
    if (ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO))
        return;
    if (!CPlayerUtilCanInterruptComboNow())
    {











        if (!sIsANormalWalkPower(ptControllingPlayer->tClientInfo.snoCurrentPower, ptControllingPlayer->idPlayerACD))
        {
            uint32 dwNow = CGameGetClientTicks();
            int32 nTimeDiff = dwNow - ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[ACTION_UI_LEFT_MOUSE_BUTTON];
            int32 nTimeDiffHeld = dwNow - ptControllingPlayer->tClientInfo.dwGameTickOfLastHeldAction[ACTION_UI_LEFT_MOUSE_BUTTON];

            if (!PowerIsPrimary(AutoPowerDefinitionR(snoPower), ptControllingPlayer->idACDActiveVendor) && nTimeDiff < 15)
                return;
            if ((snoPower == ptControllingPlayer->tClientInfo.snoCurrentChannelledPower || snoPower == ptControllingPlayer->tClientInfo.snoCurrentClientWalkPower) && nTimeDiffHeld < HELD_ACTION_REPEAT_INTERVAL)
                return;
            ptControllingPlayer->tClientInfo.dwGameTickOfLastHeldAction[ACTION_UI_LEFT_MOUSE_BUTTON] = dwNow;
        }
    }









    ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[ACTION_UI_LEFT_MOUSE_BUTTON] = CGameGetClientTicks();
    ptControllingPlayer->tClientInfo.dwGameTickOfLastActionSecondaryPowerCast[ACTION_UI_LEFT_MOUSE_BUTTON] = CGameGetClientTicks();

    ptControllingPlayer->tClientInfo.tTargetLock.snoPower = InputChooseFallbackPower(ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower, ptControllingPlayer, NULL);
    if (ptControllingPlayer->tClientInfo.tTargetLock.snoPower == SNO_NONE)
        return;




    ActorCommonData * ptACDPlayer = ACDTryToGet(ptControllingPlayer->idPlayerACD);
    if (ptACDPlayer && PowerIsChannelled(*ptACDPlayer, AutoPowerDefinitionR(ptControllingPlayer->tClientInfo.tTargetLock.snoPower)))
    {
        if (ptControllingPlayer->tClientInfo.bWantToCancelChanneledPower && sAbortChannelledPower(ptControllingPlayer))
            sPlayerLeftClick(ptPlayerRActor, ptControllingPlayer, TRUE);
        else
            sHoldPositionAndAttackTowardsCursor(ptPlayerRActor, tInput, ptControllingPlayer->tClientInfo.tTargetLock.snoPower, ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower);
        return;
    }
    if (ptControllingPlayer->tClientInfo.nInputMode == INPUT_MODE_NORMAL)
    {
        ptControllingPlayer->tClientInfo.nInputMode = ptControllingPlayer->tClientInfo.tTargetLock.tTarget.idACDTarget != ACDID_INVALID ? INPUT_MODE_ACTOR_TARGET_LOCK : INPUT_MODE_MOVE_LOCK;
    }









    switch (ptControllingPlayer->tClientInfo.nInputMode)
    {
    case INPUT_MODE_MOVE_LOCK:
        sHandleMoveLock(ptPlayerRActor, tInput);
        break;
    case INPUT_MODE_ACTOR_TARGET_LOCK:
        sHandleActorTargetLock(ptPlayerRActor, tInput);
        break;
    }
}














static void sHandleAutoAttack(
    const RActor * ptPlayerRActor,
    const BufferedInput & tInput,
    const SNO snoPower,
    const SNO snoOriginalPower)
{

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);




    BOOL bHoldPositionAndAttack = CPlayerUtilPlayerSpecifiedDontMove(ptControllingPlayer, FALSE, FALSE);
    if (bHoldPositionAndAttack && !sPlayerShouldIgnoreClick(snoPower, RACTORID_INVALID, ptControllingPlayer))
    {
        sHoldPositionAndAttackTowardsCursor(ptPlayerRActor, tInput, snoPower, snoOriginalPower);
        return;
    }

    const IVector2D & pointMouse = tInput.tBufMouseData.p;

    const float flSearchRadius = 9.0f;
    const Angle aSearchAngle = QUARTER_CIRCLE;

    const Vector2D * pFacing = NULL;


    if (InputIsControllerPresent())
    {
        pFacing = &Console::ControllerUtils::GetLStickWorldDirection(ptControllingPlayer->nControllerIndex);
    }


    RActorID idRActor = CPlayerUtilChooseBestAutoTarget(ptControllingPlayer, snoPower, flSearchRadius, aSearchAngle, 0.0f, pFacing);

    if (idRActor != RACTORID_INVALID)
    {

        Target tTarget;
        CPlayerUtilGetTarget(ptControllingPlayer, idRActor, pointMouse, snoPower, tTarget, NULL);

        BOOL bCancelingExistingPower = FALSE;
        BOOL fCanUse = CPlayerCanUsePowerOnTarget(ptPlayerRActor, snoPower, snoOriginalPower, tTarget, TRUE, FALSE, TRUE, bCancelingExistingPower, FALSE);
        if (fCanUse)
        {
            CPlayerUsePowerOnTarget(ptPlayerRActor, snoPower, snoOriginalPower, ACDID_INVALID, tTarget, bCancelingExistingPower, SNO_NONE, -1, TRUE);
        }
    }
    else
    {

        SNO snoPowerLoc = GlobalSNOGet(GLOBALSNO_POWERS_WALK);

        Target tTarget;
        CPlayerUtilGetTarget(ptControllingPlayer, RACTORID_INVALID, pointMouse, snoPowerLoc, tTarget, NULL);

        if (sPlayerCanWalk(*ptPlayerRActor))
        {
            sWalkToPoint(ptPlayerRActor, snoPowerLoc, snoOriginalPower, tTarget, 0.0f, 0.0f, FALSE, FALSE, TRUE, FALSE, 0);
        }
    }
}


static void sButtonHold(
    const RActor * ptPlayerRActor,
    const BufferedInput & tInput,
    const UIAction eAction,
    SNO snoPower)
{

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);





    if (ptControllingPlayer->tClientInfo.nUIAction != eAction)
        return;



    UIAction eActionToUse;
    ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower = snoPower;
    ptControllingPlayer->tClientInfo.tTargetLock.snoPower = snoPower = InputChooseFallbackPower(snoPower, ptControllingPlayer, NULL);




























    if (!CPlayerUtilCanInterruptComboNow())
    {
        if (!sIsANormalWalkPower(ptControllingPlayer->tClientInfo.tTargetLock.snoPower, ptPlayerRActor->idACD))
        {
            uint32 dwNow = CGameGetClientTicks();
            int32 nTimeDiff = dwNow - ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[eAction];
            int32 nTimeDiffHeld = dwNow - ptControllingPlayer->tClientInfo.dwGameTickOfLastHeldAction[eAction];
            if (!PowerIsPrimary(AutoPowerDefinitionR(snoPower), ptControllingPlayer->idPlayerACD) && nTimeDiff < 15)
                return;


            if ((snoPower == ptControllingPlayer->tClientInfo.snoCurrentChannelledPower ||
                 snoPower == ptControllingPlayer->tClientInfo.snoCurrentClientWalkPower) && nTimeDiffHeld < HELD_ACTION_REPEAT_INTERVAL)
                return;





            ptControllingPlayer->tClientInfo.dwGameTickOfLastHeldAction[eAction] = dwNow;
        }
        else
        {
            if ((int32)(CGameGetClientTicks() - ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[eAction]) < 15)
                return;
        }
    }






    switch (ptControllingPlayer->tClientInfo.nInputMode)
    {
    case INPUT_MODE_NORMAL:
        if (ptControllingPlayer->tClientInfo.tTargetLock.tTarget.idACDTarget != ACDID_INVALID)
        {
            ptControllingPlayer->tClientInfo.nInputMode = INPUT_MODE_ACTOR_TARGET_LOCK;
        }
        else break;

    case INPUT_MODE_ACTOR_TARGET_LOCK:

        sHandleActorTargetLock(ptPlayerRActor, tInput);
        return;
    }

    if (PowerCanSteer(AutoPowerDefinitionR(ptControllingPlayer->tClientInfo.tTargetLock.snoPower)))
    {

        ptControllingPlayer->tClientInfo.bClosing = FALSE;
        sHandleAutoAttack(ptPlayerRActor, tInput, ptControllingPlayer->tClientInfo.tTargetLock.snoPower, ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower);
    }
    else
    {
        sHoldPositionAndAttackTowardsCursor(ptPlayerRActor, tInput, ptControllingPlayer->tClientInfo.tTargetLock.snoPower, ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower);
    }

}



static BOOL sAbortChannelledPower(Player * ptPlayer)
{
    if (ptPlayer)
    {
        if (ptPlayer->tClientInfo.snoCurrentChannelledPower == SNO_NONE)
            return TRUE;

        ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);

        uint32 dwNow = CGameGetClientTicks();
        if (ptPlayer->tClientInfo.dwMinChanneledDurationTime > dwNow)
        {
            ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime = ptPlayer->tClientInfo.dwMinChanneledDurationTime;
            ptPlayer->tClientInfo.bWantToCancelChanneledPower = TRUE;
            return FALSE;
        }

        {
            DWordDataMessage tMessage;
            tMessage.dwData = ptPlayer->tClientInfo.snoCurrentChannelledPower;
            MessageSendToServer(CMSG_CHANNELLED_POWER_ABORTED, &tMessage, sizeof(tMessage), ptPlayer);

            if (tACD.IsAlive())
            {
                AnimReason eAnimReason = AnimPlayerGetAnimReason(tACD);
                if (eAnimReason != ANIM_REASON_STANCE && eAnimReason != ANIM_REASON_DEATH)
                    AnimPlayerPlayIdle(tACD);
            }
        }

        ptPlayer->tClientInfo.snoCurrentChannelledPower = SNO_NONE;
        ptPlayer->tClientInfo.dwInterruptedAnimCompleteTime = 0;
        ptPlayer->tClientInfo.bWantToCancelChanneledPower = FALSE;
    }

    return TRUE;
}










static void sSendClientWalkAbortToServer(Player & tPlayer)
{

    if (tPlayer.tClientInfo.snoCurrentClientWalkPower != SNO_NONE)
    {

        DWordDataMessage tMessage;
        tMessage.dwData = tPlayer.tClientInfo.snoCurrentClientWalkPower;
        MessageSendToServer(CMSG_CLIENT_WALK_POWER_ABORTED, &tMessage, sizeof(tMessage), &tPlayer);

        tPlayer.tClientInfo.snoCurrentClientWalkPower = SNO_NONE;
        tPlayer.tClientInfo.dwInterruptedAnimCompleteTime = 0;
        tPlayer.tClientInfo.nUIAction = -1;
        tPlayer.tClientInfo.nInputMode = 0;


        RActorID idLocalPlayerRActor = tPlayer.idPlayerRActor;
        const RActor * ptPlayerRActor = RActorTryToGet(idLocalPlayerRActor);
        if (ptPlayerRActor)
        {

            if (ptPlayerRActor->ptCActorMovement &&
                ptPlayerRActor->ptCActorMovement->IsMoving())
            {
                ptPlayerRActor->ptCActorMovement->StopNow();
            }
            else

            if (ptPlayerRActor->idACD != ACDID_INVALID)
            {
                ActorCommonData & tACD = ACDGet(ptPlayerRActor->idACD);
                if (tACD.IsAlive())

                    AnimPlayerPlayIdle(tACD);
            }


        }
    }
}
// LINKER-REMOVED: DWARF size 0x64, rows at address 0 (4206:18 then 4207:31 load, extra return block); inferred `ptPlayer &&` null test.
BOOL CPlayerInputAbortClientWalkPower(
    Player * ptPlayer)
{
    if (ptPlayer &&
        ptPlayer->tClientInfo.snoCurrentClientWalkPower != SNO_NONE)
    {
        if (ptPlayer->tClientInfo.dwMinChanneledDurationTime > CGameGetClientTicks())
            return FALSE;




        sSendClientWalkAbortToServer(*ptPlayer);
    }


    return TRUE;
}




static void sPlayerLeftMouseUp(
    Player * ptPlayer)
{
    sPlayerButtonUp(ptPlayer, ACTION_UI_LEFT_MOUSE_BUTTON);
}

static void sPlayerButtonUp(
    Player * ptPlayer,
    UIAction eUIAction)
{
    ptPlayer->tClientInfo.dwGameTickOfLastAction[eUIAction] = 0;
    sPlayerRememberButtonClickForHeldAction(ptPlayer, eUIAction, FALSE);
    if (ptPlayer->tClientInfo.nUIAction == eUIAction)
    {
        if (ptPlayer->tClientInfo.bClosing)
        {
            ptPlayer->tClientInfo.nUIAction = ACTION_INVALID;
            ptPlayer->tClientInfo.nInputMode = INPUT_MODE_NORMAL;
        }
        else
        {


            CPlayerInputCancelMode(ptPlayer, FALSE);
        }
    }
}



BOOL UIActionIsSkill(UIAction eAction)
{
    return eAction == ACTION_UI_LEFT_MOUSE_BUTTON || eAction == ACTION_UI_RIGHT_MOUSE_BUTTON ||
           eAction == ACTION_UI_SKILL_1 || eAction == ACTION_UI_SKILL_2 ||
           eAction == ACTION_UI_SKILL_3 || eAction == ACTION_UI_SKILL_4 || eAction == ACTION_UI_WALK;
}









































BOOL CPlayerConsoleUseSkill(Player * ptPlayer, const ResolvedClick & tClick, BOOL fCanCache, BOOL fRefreshCache)
{





    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);


    TargetLock tTargetLock;
    tTargetLock.Set(tClick.snoOriginalPower, tClick.snoPower, tClick.tTarget, tClick.idACDItemToUse, FALSE);



    if (PowerExtendsTargetToAttackRadius(AutoPowerDefinitionR(tClick.snoPower)))
    {

        WorldPlace placeTarget;
        tTargetLock.tTarget.GetPlace(placeTarget);

        if (PowerExtendTargetToPowerAttackRadius(ptPlayer->idPlayerACD, AutoPowerDefinitionR(tTargetLock.snoPower), placeTarget))
        {
            tTargetLock.tTarget.InitializeFromPlace(placeTarget);
        }
    }



    BOOL bPrimaryPower = PowerIsPrimary(AutoPowerDefinitionR(tClick.snoOriginalPower), ptPlayer->idPlayerACD);
    if (bPrimaryPower)
    {
        ptPlayer->tClientInfo.tTargetLock = tTargetLock;
    }


    if (tTargetLock.tTarget.IsInvalidForSource(&tACDPlayer))
        return FALSE;




    if (sIsANormalWalkPower(tTargetLock.snoPower, tACDPlayer.id))
        return FALSE;




    BOOL bHoldPositionAndAttack = CPlayerUtilPlayerSpecifiedDontMove(ptPlayer, FALSE, FALSE) || tTargetLock.bIgnoreRange;
    const RActor * ptRActor = RActorGet(ptPlayer->idPlayerRActor);

    BOOL bCancelingExistingPower = FALSE;
    BOOL fCanUse = CPlayerCanUsePowerOnTarget(ptRActor, tTargetLock.snoPower, tTargetLock.snoOriginalPower, tTargetLock.tTarget, bHoldPositionAndAttack, fCanCache, fRefreshCache, bCancelingExistingPower, FALSE);
    if (!fCanUse)
        return FALSE;

    fCanUse = CPlayerUsePowerOnTarget(ptRActor, tTargetLock.snoPower, tTargetLock.snoOriginalPower, tTargetLock.idACDItemToUse, tTargetLock.tTarget, bCancelingExistingPower, SNO_NONE, -1, TRUE);

    ptPlayer->tClientInfo.snoCurrentPower = tTargetLock.snoPower;
    ptPlayer->tClientInfo.snoLastUsedPower = tTargetLock.snoOriginalPower;


    if (!PowerPlayerAttackOnlyOncePerClick(AutoPowerDefinitionR(tTargetLock.snoPower)) && bPrimaryPower)
    {
        if (PowerStickyTargetsActorsWhileSweeping(AutoPowerDefinitionR(tTargetLock.snoPower)))
        {

            ptPlayer->tTargetComponent.SetStickyTarget(tClick.tTarget.GetACD());
        }
        else
        {

            if (ptPlayer->tTargetComponent.GetLockedTargetType() != 1 &&
                ptPlayer->tTargetComponent.GetLockedTargetType() != 2)
            {

                const ActorCommonData * ptACDPersistentTarget = ACDTryToGet(tClick.tTarget.GetACD());
                if (ptACDPersistentTarget && ptACDPersistentTarget->GetActorType() == ACTORTYPE_MONSTER)
                {

                    ptPlayer->tTargetComponent.SetPersistentTarget(tClick.tTarget.GetACD(), ptPlayer->tTargetComponent.GetTargetingDirection());
                }
            }
        }
    }

    return fCanUse;
}

BOOL CPlayerConsoleProcessHeldButton(const BufferedInput & tInput)
{


    if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONREPEAT && tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONSTILLDOWN)
        return FALSE;





    Player * ptPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;




    if (ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_CANT_START_DISPLAYED_POWERS)))
    {
        ptPlayer->tTargetComponent.ResetTargetRange();
        ptPlayer->tTargetComponent.SetInReticleMode(FALSE, SNO_NONE);
        return FALSE;
    }


    SNO snoPower = PlayerGetPowerSNOFromBufferedInput(*ptPlayer, tInput);
    if (snoPower == SNO_NONE)
        return FALSE;




    if (!PowerIsCoolDownDone(*ptACDPlayer, snoPower))
        return FALSE;




    ScopedPowerDefinitionLockR pdefPower(snoPower);
    if (PowerUsesTargetingReticleWhenHeld(AutoPowerDefinitionR(pdefPower), ptPlayer))
    {

        if (!ptPlayer->tTargetComponent.GetInReticleMode())
        {



            ptPlayer->tTargetComponent.ResetTargetingReticle(ptPlayer, snoPower, TRUE);
            ptPlayer->tTargetComponent.SetInReticleMode(TRUE, snoPower);
        }
        else
        {


            ScopedGlobalDefinitionLockR pdefGlobals;
            Blizzard::Time::Millisecond heldTime = Console::ControllerUtils::GetHeldButtonTime(ptPlayer->nControllerIndex, tInput.tBufControllerData.eInputButton);

            if (heldTime > (Blizzard::Time::Millisecond)pdefGlobals->holdTime)
            {


                if (!ptPlayer->tTargetComponent.GetIsHoldingReticlePowerDown())
                {



                    ptPlayer->tTargetComponent.SetIsHoldingReticlePowerDown(TRUE);
                }








                ptPlayer->tTargetComponent.AdjustTargetingReticle(1.0f);
            }
        }


        ptPlayer->tTargetComponent.UpdateReticleDistanceRatio(snoPower);
        ResolvedClick tClick;
        Targeting::Console::ResolveTargetForPlayer(ptPlayer, snoPower, tClick, TRUE, TRUE, FALSE, TRUE, FALSE);
        CPlayerUtilSpawnClickEffect(tClick.tTarget);
        return TRUE;
    }


    else if (PowerUseAtMaxRangeWhenHeld(AutoPowerDefinitionR(pdefPower), ptPlayer))
    {

        ScopedGlobalDefinitionLockR pdefGlobals;
        Blizzard::Time::Millisecond heldTime = Console::ControllerUtils::GetHeldButtonTime(ptPlayer->nControllerIndex, tInput.tBufControllerData.eInputButton);

        if (heldTime > (Blizzard::Time::Millisecond)pdefGlobals->holdTime)
        {


            ResolvedClick tResolvedClick;
            tResolvedClick.snoOriginalPower = snoPower;
            tResolvedClick.snoPower = snoPower;
            Targeting::Console::TargetSetToGround(ptPlayer, tResolvedClick.tTarget, pdefPower);

            if (CPlayerConsoleUseSkill(ptPlayer, tResolvedClick, FALSE, FALSE))
            {

                CPlayerInputClearHeldButton(tInput);
            }
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CPlayerConsoleTryUseSkill(const BufferedInput tInput)
{

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
    const ActorCommonData * ptACD = ACDTryToGet(ptControllingPlayer->idPlayerACD);
    if (!ptACD)
        return FALSE;




    SNO snoPower = PlayerGetPowerSNOFromBufferedInput(*ptControllingPlayer, tInput);
    if (snoPower == SNO_NONE)
        return FALSE;





    BOOL bCheckForHostiles = tInput.tBufControllerData.eInputButton == 0;
    BOOL bCheckForOperatableGizmos = TRUE;
    switch (tInput.eInputMsg)
    {
    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
        bCheckForHostiles = FALSE;

    default:
        {
            ScopedPowerDefinitionLockR pdefPower(snoPower);
            if (PowerFiresOnButtonUp(AutoPowerDefinitionR(pdefPower), ptControllingPlayer) ||
                PowerUseAtMaxRangeWhenHeld(AutoPowerDefinitionR(pdefPower), ptControllingPlayer))
            {
                if (!bCheckForHostiles)
                    return FALSE;
                bCheckForOperatableGizmos = FALSE;
            }



        }
        break;

    case INPUTMSG_CONTROLLERBUTTONUP:
        {
            ScopedPowerDefinitionLockR pdefPower(snoPower);
            if (!PowerFiresOnButtonUp(AutoPowerDefinitionR(pdefPower), ptControllingPlayer) &&
                !PowerUseAtMaxRangeWhenHeld(AutoPowerDefinitionR(pdefPower), ptControllingPlayer))
                return FALSE;

            bCheckForHostiles = FALSE;



        }
        break;
    }

    BOOL bButtonHeld = tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONSTILLDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONREPEAT;

    ResolvedClick tResolvedClick;
    CPlayerUtilResolveConsoleButton(snoPower, tResolvedClick, ptControllingPlayer, bButtonHeld, bCheckForOperatableGizmos, bCheckForHostiles);
    if (tResolvedClick.snoPower == SNO_NONE)
        return FALSE;

    if (!CPlayerConsoleUseSkill(ptControllingPlayer, tResolvedClick, !bButtonHeld, TRUE))
        return FALSE;







    if (PowerPlayerAttackOnlyOncePerClick(AutoPowerDefinitionR(tResolvedClick.snoPower)))
    {

        CPlayerInputClearHeldButton(tInput);
    }
    else
    {





        UIAction eButtonAction = Prefs::GetUIActionFromInput(tInput);
        ptControllingPlayer->tClientInfo.dwGameTickOfLastActionSecondaryPowerCast[eButtonAction] = CGameGetClientTicks();
    }
    return TRUE;
}

BOOL CPlayerConsoleTryUsePotion(const BufferedInput & tInput)
{

    if (!Controller::InputIsPotion(tInput))
        return FALSE;


    if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONDOWN)
        return FALSE;


    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);

    const HotbarButtonData & tPotionData = PlayerGetPotionBtnAssignment(ptControllingPlayer);
    GBID gbidItem = tPotionData.m_gbidItem;
    if (gbidItem == -1)
    {
        ACDID idItem = ACDFromANN(tPotionData.m_annItem);
        if (idItem != ACDID_INVALID)
        {

            gbidItem = ACDGet(idItem).hGB.gbid;
        }
    }


    ACDID idItem = ACDID_INVALID;
    int32 nPotionCount = ActorInventoryCountItemsOfTypeOnPlayer(ptControllingPlayer->idPlayerACD, gbidItem, &idItem);

    if (nPotionCount > 0)
    {


        ActorCommonData * ptACD = ACDTryToGet(idItem);
        if (ptACD)
        {

            GameError eError = ItemCanBeUsed(ptControllingPlayer->idPlayerACD, *ptACD, NULL);
            switch (eError)
            {
            case GAMEERROR_ITEMUSE_UNKNOWN: return FALSE;
            case GAMEERROR_NONE:
                CACDInventoryRequestUse(*ptACD, ACDID_INVALID, TRUE, ptControllingPlayer);
                return TRUE;
            default:
                if (!CGameInScriptedSequence())
                    UIGame::DisplayError(GameErrorData(eError), 0, ptControllingPlayer);
            }
        }
    }

    return FALSE;
}

void CPlayerConsoleTryEvade(Player * ptPlayer)
{


    ScopedGlobalDefinitionLockR pdefGlobal;
    float flFacingRange = pdefGlobal->flControllerEvadeZone;

    if (Console::ControllerUtils::GetRStickPosition(ptPlayer->nControllerIndex).MagnitudeSquared() < flFacingRange * flFacingRange)
        return;





    if (!Console::ControllerUtils::DidStickCrossThreshold(ptPlayer->nControllerIndex, TRUE, flFacingRange))
        return;




    RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
    if (ptRActor)
    {
        const ActorCommonData & tACD = ACDGet(ptRActor->idACD);


        Vector3D vEvadeOffset(Console::ControllerUtils::GetRStickWorldDirection(ptPlayer->nControllerIndex).x, Console::ControllerUtils::GetRStickWorldDirection(ptPlayer->nControllerIndex).y, tACD.GetForward().z);
        WorldPlace tTargetPlace = tACD.GetPlace();
        tTargetPlace.wp += vEvadeOffset * 100.0f;

        CPlayerConsoleUseEvade(ptPlayer, tTargetPlace, FALSE);
    }

}

void CPlayerConsoleTryGestureEvade(Player * ptPlayer)
{



    RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
    if (ptRActor)
    {
        const ActorCommonData & tACD = ACDGet(ptRActor->idACD);


        Vector3D vEvadeOffset(Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).x, Console::ControllerUtils::GetLStickWorldDirection(ptPlayer->nControllerIndex).y, tACD.GetForward().z);

        if (Console::ControllerUtils::GetLStickPosition(ptPlayer->nControllerIndex, FALSE) == Vector2D(0.0f, 0.0f))
        {


            vEvadeOffset.x = -vEvadeOffset.x;
            vEvadeOffset.y = -vEvadeOffset.y;
        }


        WorldPlace tTargetPlace = tACD.GetPlace();
        tTargetPlace.wp += vEvadeOffset * 100.0f;

        CPlayerConsoleUseEvade(ptPlayer, tTargetPlace, FALSE);
    }

}

BOOL CPlayerConsoleUseEvade(Player * ptPlayer, const WorldPlace & tTargetPlace, BOOL bQueued)
{


    RActor * ptRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
    if (!ptRActor)
        return FALSE;


    const ActorCommonData & tACD = ACDGet(ptRActor->idACD);


    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_SKILL_OVERRIDE_ACTIVE)))
    {

        UIGame::DisplayError(GameErrorData(GAMEERROR_POWER_UNUSABLE_GENERIC), 0, ptPlayer);
        return TRUE;
    }


    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PREVENT_EVADE_DURING)))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_POWER_UNUSABLE_GENERIC), 0, ptPlayer);
        return TRUE;
    }


    SNO snoPowerToTry = GlobalSNOGet(GLOBALSNO_CONSOLE_EVADE);


    WorldPlace tEvadePlace = tACD.GetPlace();


    Vector3D vEvadeOffset = tTargetPlace.wp - tEvadePlace.wp;
    vEvadeOffset.Normalize();
    vEvadeOffset *= PowerGetRange(&tACD, AutoPowerDefinitionR(snoPowerToTry), TRUE);
    tEvadePlace.wp += vEvadeOffset;


    if (!LocalPlayerClamping::IsPointInsideActiveCamera(tEvadePlace))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_POWER_UNUSABLE_GENERIC), 0, ptPlayer);
        return TRUE;
    }

    Target tTarget;
    tTarget.InitializeFromPlace(tTargetPlace);

    BOOL bCancelingExistingPower = FALSE;
    BOOL fCanUse = CPlayerCanUsePowerOnTarget(ptRActor, snoPowerToTry, snoPowerToTry, tTarget, FALSE, !bQueued, !bQueued, bCancelingExistingPower, FALSE);
    if (!fCanUse)
        return FALSE;

    {

        SNO snoButtonPower = ptPlayer->tHeroStateData.tSavedData.tActiveSkills[Controller::GetSlotFromButton(0, ptPlayer)].snoSkill;
        ResolvedClick tResolvedClick;
        Targeting::Console::ResolveTargetForPlayer(ptPlayer, snoButtonPower, tResolvedClick, FALSE, TRUE, FALSE, FALSE, FALSE);

        fCanUse = CPlayerUsePowerOnTarget(ptRActor, snoPowerToTry, snoPowerToTry, ACDID_INVALID, tTarget, bCancelingExistingPower, SNO_NONE, -1, TRUE);
        if (fCanUse)
        {

            if (!ptPlayer->tTargetComponent.ResolveLockedTarget())
            {

                const ActorCommonData * ptACDLockedTarget = ACDTryToGet(tResolvedClick.tTarget.GetACD());
                if (ptACDLockedTarget && ptACDLockedTarget->GetActorType() == ACTORTYPE_MONSTER)
                {

                    ptPlayer->tTargetComponent.SetPersistentTarget(tResolvedClick.tTarget.GetACD(), ptPlayer->tTargetComponent.GetTargetingDirection());
                }
                else
                {
                    ptPlayer->tTargetComponent.SetSoftLockDirection(ptPlayer->tTargetComponent.GetTargetingDirection());
                }
            }
        }
    }

    return fCanUse;
}

BOOL CPlayerConsoleTryWarp(Player * ptPlayer, const WorldPlace & tTargetPlace)
{


    const RActor * ptPlayerRActor = RActorTryToGet(ptPlayer->idPlayerRActor);
    if (!ptPlayerRActor)
        return FALSE;


    SNO snoPower = GlobalSNOGet(GLOBALSNO_CONSOLE_WARP);

    Target tTarget;
    tTarget.InitializeFromPlace(tTargetPlace);


    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    uint32 dwPowerFailureReasons = PowerACDCanStartPowerFailureReasons(tACDPlayer, AutoPowerDefinitionR(snoPower), NULL, FALSE, FALSE);
    if (dwPowerFailureReasons != 0)
        return FALSE;


    BOOL bCancellingExistingPower = FALSE;
    BOOL fCanUse = CPlayerCanUsePowerOnTarget(ptPlayerRActor, snoPower, snoPower, tTarget, FALSE, FALSE, FALSE, bCancellingExistingPower, FALSE);
    if (!fCanUse)
        return FALSE;

    return CPlayerUsePowerOnTarget(ptPlayerRActor, snoPower, snoPower, ACDID_INVALID, tTarget, FALSE, SNO_NONE, -1, FALSE);


}

BOOL CPlayerConsoleTargetLock(const BufferedInput & tInput)
{


    if (!Controller::InputIsTargetLock(tInput))
        return FALSE;



    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
    ActorCommonData * ptACDLockedTarget = ptControllingPlayer->tTargetComponent.ResolveLockedTarget();

    if (ptACDLockedTarget && ptControllingPlayer->tTargetComponent.GetLockedTargetType() == ConsoleTargetComponent::TARGETLOCK_HARD)
    {
        if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONUP)
            ptControllingPlayer->tTargetComponent.ClearLockedTarget();

    }
    else if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONUP)
    {


        ActorCommonData * ptACDPlayer = ACDTryToGet(ptControllingPlayer->idPlayerACD);
        if (!ptACDPlayer)
            return FALSE;


        SNO snoPower = ptACDPlayer->AttributeGetBasicAttack();
        if (snoPower == SNO_NONE)
            snoPower = GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_RANGED_WAND);

        ResolvedClick tResolvedClick;

        Targeting::Console::ResolveTargetForPlayer(ptControllingPlayer, snoPower, tResolvedClick, FALSE, TRUE, FALSE, FALSE, TRUE);


        ACDID idACDLastTarget = ptControllingPlayer->tTargetComponent.GetLastTarget();
        ActorCommonData * ptACDLastTarget = ACDTryToGet(idACDLastTarget);
        if (!ptACDLastTarget)
            return FALSE;


        if (!ptACDLastTarget->IsTargetable())
            return FALSE;


        ptControllingPlayer->tTargetComponent.SetLockedTarget(idACDLastTarget);

    }


    return TRUE;
}

BOOL CPlayerTogglePlayerIndicatorBeam(const BufferedInput & tInput)
{


    if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONUP || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN)
    {
        Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
        if (ptControllingPlayer)
        {

            RActor * ptRActor = RActorTryToGet(ptControllingPlayer->idPlayerRActor);
            if (ptRActor)
            {
                ptRActor->bShowLocatorBeam = tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN;
                return TRUE;
            }
        }
    }


    return FALSE;
}
bool InputIsSingleJoycon(int32_t nControllerIndex);  // InputNX64.cpp
BOOL CPlayerConsoleTryUseUtilitySkill(const BufferedInput & tInput)
{


    if (Controller::InputIsPotion(tInput))
        return CPlayerConsoleTryUsePotion(tInput);



    if (Controller::InputIsTargetLock(tInput) && !InputIsSingleJoycon(tInput.tBufControllerData.nControllerIndex))
        return CPlayerConsoleTargetLock(tInput);



    if (Controller::InputIsTogglePlayerIndicatorBeam(tInput))
        return CPlayerTogglePlayerIndicatorBeam(tInput);



    return FALSE;
}














































// NON_MATCHING: 3464 vs 3456 bytes; the original tests eInputMsg before bInReticleMode for the nUIAction store (5073) and at 5098, ours hoists the bInReticleMode test and moves the store out of line; tried continue-form and both && orders in the second-newest loop.
BOOL CPlayerHandleConsoleInput(
    const RActor * ptPlayerRActor,
    BufferedInput & tInput)
{
    if (GameIsPaused() || GameIsSoftPaused() || CGameInScriptedSequence())
        return FALSE;



    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);

    ScopedGlobalDefinitionLockR pdefGlobal;
    switch (tInput.eInputMsg)
    {
    case INPUTMSG_LEFTSTICK:
        sPlayerLeftStick(ptPlayerRActor, ptControllingPlayer);
        break;

    case INPUTMSG_RIGHTSTICK:
        sPlayerRightStick(ptPlayerRActor, ptControllingPlayer);
        break;










    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
    {
        if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONDOWN &&
            !(ptControllingPlayer->tClientInfo.dwDownEventBits & (1 << tInput.tBufControllerData.eInputButton)))
            return FALSE;


















        sPlayerRememberButtonClickForHeldInput(ptControllingPlayer, tInput, TRUE);
        if (tInput.tBufControllerData.eInputButton == 1 && UIConversation::IsCancelButtonOnCooldown())
            return FALSE;
        if (tInput.tBufControllerData.eInputButton == 2 && UIConversation::IsSkipAllButtonOnCooldown())
            return FALSE;













        UIAction eButtonAction = Prefs::GetUIActionFromInput(tInput);
        if (eButtonAction != ACTION_INVALID)
        {
            uint32 dwNow = CGameGetClientTicks();
            int32 nTimeDiffHeld = dwNow - ptControllingPlayer->tClientInfo.dwGameTickOfLastHeldAction[eButtonAction];
            int32 nTimeDiffSecondary = dwNow - ptControllingPlayer->tClientInfo.dwGameTickOfLastActionSecondaryPowerCast[eButtonAction];

            BOOL bInReticleMode = ptControllingPlayer->tTargetComponent.GetInReticleMode();

            SNO snoPower = PlayerGetPowerSNOFromBufferedInput(*ptControllingPlayer, tInput);
            if (snoPower != SNO_NONE &&
                PowerIsPrimary(AutoPowerDefinitionR(snoPower), ptControllingPlayer->idPlayerACD) &&
                PowerGetTemplate(AutoPowerDefinitionR(snoPower), ptControllingPlayer->idPlayerACD) != SECONDARY_ANIMATION_POWER)
            {
                if (!ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[eButtonAction])
                    ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[eButtonAction] = dwNow;
                if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONSTILLDOWN && !bInReticleMode && tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONREPEAT)
                    ptControllingPlayer->tClientInfo.nUIAction = eButtonAction;














                uint32 dwLatestButtonDownTick = 0;
                UIAction eNewestAction = ACTION_INVALID;

                for (int32 i = 0; i < numButtonActions; i++)
                {
                    uint32 dwButtonDownTick = ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[arButtonActions[i]];
                    if (dwButtonDownTick > dwLatestButtonDownTick)
                    {
                        dwLatestButtonDownTick = dwButtonDownTick;
                        eNewestAction = arButtonActions[i];
                    }
                }

                UIAction eSetPrimaryAction = ptControllingPlayer->tClientInfo.nUIAction;
                if (!bInReticleMode && eNewestAction != ACTION_INVALID && eNewestAction != eButtonAction && eButtonAction != eSetPrimaryAction && eSetPrimaryAction != ACTION_INVALID)
                    return FALSE;











                if (bInReticleMode &&
                    ptControllingPlayer->tTargetComponent.GetReticlePower() != snoPower)
                    return FALSE;




                BOOL bCheckForOperatableGizmos = tInput.tBufControllerData.eInputButton == 0 && tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN;
                if (!bCheckForOperatableGizmos)
                {
                    ActorCommonData & tACDPlayer = ACDGet(ptControllingPlayer->idPlayerACD);

                    GameError eError;
                    SNO snoFallbackPower = InputChooseFallbackPower(snoPower, ptControllingPlayer, &eError);
                    uint32 dwFailureReasons = PowerACDCanStartPowerFailureReasons(tACDPlayer, AutoPowerDefinitionR(snoPower), NULL, FALSE, FALSE);
                    if (snoFallbackPower != snoPower || dwFailureReasons)
                    {
                        dwLatestButtonDownTick = 0;



                        UIAction eSecondNewestAction = ACTION_INVALID;

                        for (int32 i = 0; i < numButtonActions; i++)
                        {
                            uint32 dwButtonDownTick = ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[arButtonActions[i]];
                            if (dwButtonDownTick > dwLatestButtonDownTick && arButtonActions[i] != eButtonAction)
                            {
                                dwLatestButtonDownTick = dwButtonDownTick;
                                eSecondNewestAction = arButtonActions[i];
                            }
                        }
                        if (eSecondNewestAction != ACTION_INVALID)
                        {
                            ptControllingPlayer->tClientInfo.nUIAction = eSecondNewestAction;
                            return FALSE;
                        }
                    }
                }
                if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_INVALID || ptControllingPlayer->tClientInfo.nUIAction != eButtonAction)
                {
                    ptControllingPlayer->tClientInfo.nUIAction = eButtonAction;
                }
            }
            else if (nTimeDiffSecondary < 30)
                return FALSE;














            if (!bInReticleMode ||
                ptControllingPlayer->tTargetComponent.GetReticlePower() == snoPower)
                CPlayerConsoleProcessHeldButton(tInput);
            if ((snoPower == ptControllingPlayer->tClientInfo.snoCurrentChannelledPower || snoPower == ptControllingPlayer->tClientInfo.snoCurrentClientWalkPower) && nTimeDiffHeld < 6)
                return FALSE;
            ptControllingPlayer->tClientInfo.dwGameTickOfLastHeldAction[eButtonAction] = dwNow;
        }
        if (CPlayerConsoleTryUseSkill(tInput))
            return TRUE;
        if (CPlayerConsoleTryUseUtilitySkill(tInput))
            return TRUE;
        break;
    }














    case INPUTMSG_CONTROLLERBUTTONUP:
    {
        if (ptControllingPlayer->tClientInfo.dwDownEventBits & (1 << tInput.tBufControllerData.eInputButton))
        {

            BOOL bHandled = CPlayerConsoleTryUseSkill(tInput);
            CPlayerInputClearHeldButton(tInput);
            if (bHandled)
                return TRUE;
        }
        if (CPlayerConsoleTryUseUtilitySkill(tInput))
            return TRUE;
        break;
    }

















    case INPUTMSG_GESTUREFLICK:
        sPlayerGestureFlick(ptPlayerRActor, ptControllingPlayer);
        return TRUE;
    default: break;
    }
    return FALSE;
}




// NON_MATCHING: 4436 vs 4436 bytes, 20 insns differ; the original folds every action->SkillButton comparison at 5355 (default -1 included) into one jump table, ours lowers the innermost ternary (RMB ? SKILL_RMB : SKILLBUTTON_INVALID) to a select; tried both chain orders and a skill-range arm.
BOOL PlayingInput(
    const BufferedInput & tInput)
{

    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
    if (!ptControllingPlayer)
    {
        if (tInput.eInputMsg != INPUTMSG_CONTROLLERBUTTONDOWN)
            return FALSE;
        if (Console::UIOnlineActions::CanLocalPlayerJoin())
        {
            if (Console::Online::PartyIsInParty(tInput.tBufControllerData.nControllerIndex))
                return FALSE;
            Console::UIOnlineActions::LocalPlayerJoin(tInput.tBufControllerData.nControllerIndex, TRUE);
            return TRUE;
        }
        else
        {
            BOOL bSentErrorMessage = FALSE;
            ACDID idPrimaryPlayer = LocalPlayerACDGet();
            if (idPrimaryPlayer != ACDID_INVALID)
            {
                const ActorCommonData & tACDPrimary = ACDGet(idPrimaryPlayer);
                if (tACDPrimary.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER))
                {
                    UIGame::DisplayErrorString(StringListFetch("Errors:CannotDoDuringBossEncounter"));
                    bSentErrorMessage = TRUE;
                }
                else if (tACDPrimary.AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON))
                {
                    UIGame::DisplayErrorString(StringListFetch("ConsoleErrors:SetDungeonNoLocalPlayerJoin"));
                    bSentErrorMessage = TRUE;
                }
            }
            if (!bSentErrorMessage && !CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && NephalemRift::IsTieredRiftActive())
            {
                if (GameIsWeeklyChallenge())
                    UIGame::DisplayErrorString(StringListFetch("ConsoleErrors:ChallengeRiftsNoLocalPlayerJoin"));
                else
                    UIGame::DisplayErrorString(StringListFetch("ConsoleErrors:TieredRiftsNoLocalPlayerJoin"));
            }
        }
        return FALSE;
    }
















    RActorID idLocalPlayerRActor = ptControllingPlayer->idPlayerRActor;
    const RActor * ptPlayerRActor = RActorTryToGet(idLocalPlayerRActor);
    if (!ptPlayerRActor)
        return FALSE;



    const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
    ptControllingPlayer->tClientInfo.eCurrentMessage = tInput.eInputMsg;

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayerRActor->idACD);
    if (!ptACDPlayer ||
        !ptACDPlayer->IsAlive() ||
        CGameGlobalsGet().bPVPPaused)
    {
        CPlayerInputCancelMode(ptControllingPlayer, FALSE);
        return FALSE;
    }


    UIGlobals *& ptUIGlobals = CGameGet().ptUIGlobals;


    switch (tInput.eInputMsg)
    {
    case INPUTMSG_KEYSTILLDOWN:
    {
        if (UIGame::MenuIsActive() || UIMap::IsOpen() || UIWaypointMap::IsOpen())
            return TRUE;







        UIAction eAction = Prefs::GetUIActionFromInput(tInput);

        if (UIActionIsSkill(eAction) && sPlayerHeldActionIsReady(ptControllingPlayer, eAction))
        {
            const Player * ptPlayer = ptControllingPlayer;

            SNO snoPower;
            if (eAction == ACTION_UI_WALK)
            {
                if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_UI_WALK || ptControllingPlayer->tClientInfo.nUIAction == ACTION_INVALID)
                {
                    sPlayerButtonClick(ptPlayerRActor, ACTION_UI_WALK, GlobalSNOGet(GLOBALSNO_POWERS_MANUAL_WALK), FALSE, TRUE);
                }
                return TRUE;
            }
            else
            {
                const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, eAction == ACTION_UI_SKILL_1 ? SKILL_HB1 : eAction == ACTION_UI_SKILL_2 ? SKILL_HB2 : eAction == ACTION_UI_SKILL_3 ? SKILL_HB3 : eAction == ACTION_UI_SKILL_4 ? SKILL_HB4 : eAction == ACTION_UI_LEFT_MOUSE_BUTTON ? SKILL_LMB : eAction == ACTION_UI_RIGHT_MOUSE_BUTTON ? SKILL_RMB : SKILLBUTTON_INVALID);
                snoPower = tButtonData.m_snoPower;
            }

            if (snoPower == SNO_NONE)
                break;
            if ((ptControllingPlayer->tClientInfo.nUIAction != ACTION_INVALID && !Prefs::KeyIsModifier(tInput.tBufKeyData.eKey)) ||

                CGameGet().ptCPlayerInputGlobals->tCursorMgr.GetCursorState() != CURSORSTATE_NORMAL)
            {
                if (ptControllingPlayer->tClientInfo.nUIAction == eAction)
                    sButtonHold(ptPlayerRActor, tInput, eAction, snoPower);
            }
            else
                sPlayerButtonClick(ptPlayerRActor, eAction, snoPower, FALSE, TRUE);
            return TRUE;
        }
        break;
    }
    case INPUTMSG_KEYUP:
    {





        UIAction eAction = Prefs::GetUIActionFromInput(tInput);
        if (UIActionIsSkill(eAction) && ptControllingPlayer->tClientInfo.nUIAction == eAction)
        {
            sPlayerButtonUp(ptControllingPlayer, eAction);
            return TRUE;
        }
        if (ptControllingPlayer->tClientInfo.nUIAction != ACTION_INVALID &&
            !Prefs::CheckAction(ptControllingPlayer->tClientInfo.nUIAction, TRUE))
        {
            sPlayerButtonUp(ptControllingPlayer, ptControllingPlayer->tClientInfo.nUIAction);
        }
        break;
    }



    case INPUTMSG_LEFTSTICK: case INPUTMSG_RIGHTSTICK:
    case INPUTMSG_LEFTTRIGGER: case INPUTMSG_RIGHTTRIGGER:
    case INPUTMSG_CONTROLLERBUTTONDOWN: case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT: case INPUTMSG_CONTROLLERBUTTONUP:
    case INPUTMSG_GESTUREFLICK:







        return CPlayerHandleConsoleInput(ptPlayerRActor, const_cast<BufferedInput &>(tInput));



    case INPUTMSG_LEFTMOUSEDOWN:
    case INPUTMSG_LEFTMOUSEDOUBLECLICK:
    {
        BOOL fIgnoreClick = FALSE;








        if (CGameInScriptedSequence() || UIGame::ModalIsActive() || UIMap::IsOpen() || UIWaypointMap::IsOpen())
            return FALSE;
        if (!fIgnoreClick)
        {




            sPlayerLeftClick(ptPlayerRActor, ptControllingPlayer, FALSE);
            UI_SetCapture(cgUIIDMainScreen);
        }
        break;
    }































    case INPUTMSG_RIGHTMOUSEDOWN:
    case INPUTMSG_RIGHTMOUSEDOUBLECLICK:
    {

        if (!CGameInScriptedSequence() && !UIGame::ModalIsActive() && !UIMap::IsOpen() && !UIWaypointMap::IsOpen())
        {



            UI_SetCapture(cgUIIDMainScreen);

            const Player * ptPlayer = ptControllingPlayer;
            const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, 1);
            sPlayerButtonClick(ptPlayerRActor, ACTION_UI_RIGHT_MOUSE_BUTTON, tButtonData.m_snoPower, FALSE, FALSE);
        }
        break;
    }
    case INPUTMSG_LEFTMOUSESTILLDOWN:
    case INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY: {
        if (ptControllingPlayer->tClientInfo.nInputMode == INPUT_MODE_CANCELED)
            return FALSE;




        if (UIMap::IsOpen() || UIWaypointMap::IsOpen())
            return FALSE;



        if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_INVALID &&
            CGameGet().ptCPlayerInputGlobals->tCursorMgr.GetCursorState() == CURSORSTATE_NORMAL &&
            (tInput.eInputMsg == INPUTMSG_LEFTMOUSESTILLDOWN ||
             sPlayerCanProcessHeldActionImmediately(ptControllingPlayer, ACTION_UI_LEFT_MOUSE_BUTTON)))
            sPlayerLeftClick(ptPlayerRActor, ptControllingPlayer, TRUE);
        else if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_UI_LEFT_MOUSE_BUTTON)
        {


            const Player * ptPlayer = ptControllingPlayer;
            const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, 0);

            SNO snoPowerToUse = tButtonData.m_snoPower;
            if (ptControllingPlayer->tClientInfo.snoLastCursorQueuedPower != SNO_NONE)
                snoPowerToUse = ptControllingPlayer->tClientInfo.snoLastCursorQueuedPower;



            sLeftHold(ptPlayerRActor, tInput, snoPowerToUse);
        }
        break;
    }
    case INPUTMSG_RIGHTMOUSESTILLDOWN:
    case INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY:


        if (UIMap::IsOpen() || UIWaypointMap::IsOpen())
            return FALSE;



        if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_INVALID &&
            CGameGet().ptCPlayerInputGlobals->tCursorMgr.GetCursorState() == CURSORSTATE_NORMAL &&
            (tInput.eInputMsg == INPUTMSG_RIGHTMOUSESTILLDOWN ||
             sPlayerCanProcessHeldActionImmediately(ptControllingPlayer, ACTION_UI_RIGHT_MOUSE_BUTTON)))
        {
            const Player * ptPlayer = ptControllingPlayer;
            const HotbarButtonData & tButtonData = PlayerGetHotbarBtnAssignment(ptPlayer, 1);
            sPlayerButtonClick(ptPlayerRActor, ACTION_UI_RIGHT_MOUSE_BUTTON, tButtonData.m_snoPower, FALSE, TRUE);
            UI_SetCapture(cgUIIDMainScreen);
        }
        else if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_UI_RIGHT_MOUSE_BUTTON)
        {
            sButtonHold(ptPlayerRActor, tInput, ACTION_UI_RIGHT_MOUSE_BUTTON, ptControllingPlayer->tClientInfo.tTargetLock.snoOriginalPower);
        }
        break;
    case INPUTMSG_LEFTMOUSEUP:
        if (ptControllingPlayer->tClientInfo.nUIAction != ACTION_INVALID)
        {
            if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_UI_LEFT_MOUSE_BUTTON)
            {
                sPlayerLeftMouseUp(ptControllingPlayer);
            }
            else if (!Prefs::CheckAction(ptControllingPlayer->tClientInfo.nUIAction, TRUE))
            {


                sPlayerButtonUp(ptControllingPlayer, ptControllingPlayer->tClientInfo.nUIAction);
            }
        }
        UI_SetCapture(GetUIIDNone());
        break;
    case INPUTMSG_RIGHTMOUSEUP:
        if (ptControllingPlayer->tClientInfo.nUIAction != ACTION_INVALID)
        {
            if (ptControllingPlayer->tClientInfo.nUIAction == ACTION_UI_RIGHT_MOUSE_BUTTON)
            {
                sPlayerButtonUp(ptControllingPlayer, ACTION_UI_RIGHT_MOUSE_BUTTON);
            }
            else if (!Prefs::CheckAction(ptControllingPlayer->tClientInfo.nUIAction, TRUE))
            {
                sPlayerButtonUp(ptControllingPlayer, ptControllingPlayer->tClientInfo.nUIAction);
            }
        }
        UI_SetCapture(GetUIIDNone());
        break;



    case INPUTMSG_MOUSEMOVE:

        if (!(tInputState.tMouseState.dwFlags & (1 << MOUSESTATE_LEFT_BUTTON_DOWN_BIT)) &&
            ptControllingPlayer->tClientInfo.nUIAction == ACTION_UI_LEFT_MOUSE_BUTTON)
        {

            sPlayerLeftMouseUp(ptControllingPlayer);
        }
        if (!(tInputState.tMouseState.dwFlags & (1 << MOUSESTATE_RIGHT_BUTTON_DOWN_BIT)) &&
            ptControllingPlayer->tClientInfo.nUIAction == ACTION_UI_RIGHT_MOUSE_BUTTON)
        {

            sPlayerButtonUp(ptControllingPlayer, ACTION_UI_RIGHT_MOUSE_BUTTON);
        }
        break;
    default: break;
    }
    return FALSE;
}















































































































































































































































































































































void CursorMgr::Init()
{





    sSetCursorState(CURSORSTATE_NORMAL);
    m_bHotbarOverridden = FALSE;
}



void CursorMgr::sSetCursorState(const CursorState eState)
{




    m_tButtonToMove.m_snoPower = -1; m_tButtonToMove.m_eRuneType = -1; m_tButtonToMove.m_gbidItem = -1; m_tButtonToMove.m_annItem = ANN_INVALID; m_eSkillSlotToMove = -1; m_snoQueuedPower = SNO_NONE; m_idACDItem = ACDID_INVALID;


    m_idACDNPC = ACDID_INVALID; m_gbidEnchantment = -1;

    m_eCursorState = eState;
    m_dwAmount = 0;
}



CursorState CursorMgr::GetCursorState() const
{


    return m_eCursorState;
}



void CursorMgr::ResetCursorState(const BOOL fClearManipulationTimeout)
{

    if (fClearManipulationTimeout)
    {
        ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
        if (ptACDItem)

            ItemSetIsBeingManipulated(*ptACDItem, FALSE, 120);
    }





    if (GetCursorState() == CURSORSTATE_USE_ITEM &&
        InputShiftKeyIsDown(INPUTCONTEXT_DEFAULT))
        return;



    sSetCursorState(CURSORSTATE_NORMAL);
}








































































































































































































































// LINKER-REMOVED: DWARF size 0x68, rows at address 0: Empty() (HotbarButton.h:35), store -1 at 6232:18, two ifs at 6234:9 and 6240:9 (&& at 6240:51, gbid at 6241:25) each copying at 6236/6243; state per if inferred from enumerator length at 6240.
void CursorMgr::GetHotbarButtonToMove(
    HotbarButtonData & tButtonToMove,
    SkillButton & eSkillButton) const
{
    tButtonToMove.Empty();
    eSkillButton = SKILLBUTTON_INVALID;

    if (GetCursorState() == CURSORSTATE_MOVING_HOTBAR_BUTTON)
    {
        tButtonToMove = m_tButtonToMove;
        eSkillButton = m_eSkillSlotToMove;
    }

    if (GetCursorState() == CURSORSTATE_MOVE_ITEM &&
        m_tButtonToMove.m_gbidItem != GBID_NONE)
    {
        tButtonToMove = m_tButtonToMove;
        eSkillButton = m_eSkillSlotToMove;
    }
}






























SNO CursorMgr::GetQueuedSkill() const
{


    return m_snoQueuedPower;
}



void CursorMgr::SetItemToMove(const ActorCommonData & tACDItemToMove)
{

    if (GetCursorState() != CURSORSTATE_NORMAL)
        return;

    sSetCursorState(CURSORSTATE_MOVE_ITEM);







    m_idACDItem = tACDItemToMove.id;
    ItemSetIsBeingManipulated(const_cast<ActorCommonData &>(tACDItemToMove), TRUE, 120);

    if (ItemHasLabel(tACDItemToMove.hGB.gbid, 40))
    {
        if (ItemIsLegendary(tACDItemToMove.hGB.gbid))
        {
            m_tButtonToMove.SetItemANN(ANNFromACD(tACDItemToMove.id));
        }
        else
        {
            m_tButtonToMove.SetItemGBID(tACDItemToMove.hGB.gbid);
        }
    }



}

ActorCommonData * CursorMgr::GetItemToMove()
{

    if (GetCursorState() != CURSORSTATE_MOVE_ITEM)
        return NULL;





    ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
    if (ptACDItem)
        return ptACDItem;




    ResetCursorState(FALSE);
    return NULL;
}























ActorCommonData * CursorMgr::GetItemToBuy()
{

    if (GetCursorState() != CURSORSTATE_BUY_ITEM)
        return NULL;





    ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
    if (ptACDItem)
        return ptACDItem;




    ResetCursorState(FALSE);
    return NULL;
}



































































ActorCommonData * CursorMgr::GetStackToSplit()
{

    if (GetCursorState() != CURSORSTATE_SPLIT_STACK)
        return NULL;

    ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
    if (ptACDItem)
        return ptACDItem;

    ResetCursorState(FALSE);
    return NULL;
}









uint32 CursorMgr::GetStackSplitAmount() const
{


    return m_dwAmount;
}




void CursorMgr::SalvageItem()
{

    sSetCursorState(CURSORSTATE_SALVAGE_ITEM);
}
































































void CursorMgr::SetHotBarOverridden(const BOOL bOverridden)
{

    m_bHotbarOverridden = bOverridden;
}



BOOL CursorMgr::GetHotBarOverridden()
{

    return m_bHotbarOverridden;
}




void CursorMgr::HandleSkillRemoval(const SNO snoPower)
{

    if (GetCursorState() == CURSORSTATE_QUEUED_SKILL &&
        GetQueuedSkill() == snoPower)
    {
        ResetCursorState(FALSE);
    }
}




ActorCommonData * CursorMgr::GetItemToUse()
{

    if (GetCursorState() != CURSORSTATE_USE_ITEM)
        return NULL;

    ActorCommonData * ptACDItem = ACDTryToGet(m_idACDItem);
    if (ptACDItem)
        return ptACDItem;

    ResetCursorState(FALSE);
    return NULL;
}










CursorMgr & PlayerGetCursorMgr()
{


    return CGameGet().ptCPlayerInputGlobals->tCursorMgr;
}




BOOL CPlayerInputIsSkillDisplayedAsUnusable(const Player * ptPlayer, const SNO snoPower)
{


    if (snoPower == SNO_NONE)
        return FALSE;





    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    if (!tACDPlayer.IsAlive())
        return FALSE;




    uint32 dwFailureReasons = PowerACDCanStartPowerFailureReasons(tACDPlayer, AutoPowerDefinitionR(snoPower), NULL, FALSE, FALSE);

    BOOL bIsChannelling = sPlayerPowerIsChannelled(ptPlayer) &&
        ptPlayer->tClientInfo.snoCurrentChannelledPower == snoPower;

    BOOL bIsClientWalking = sPlayerPowerIsClientWalk(ptPlayer) &&
        ptPlayer->tClientInfo.snoCurrentClientWalkPower == snoPower;

    if (!bIsChannelling && !bIsClientWalking &&

        !PowerActorHasResourceMinCostToStart(tACDPlayer, AutoPowerDefinitionR(snoPower)))
    {

        dwFailureReasons |= 0x80;
    }




    return (dwFailureReasons & 0x3c77f1e0) != 0;
}









































void CPlayerInputCancelACDTarget(const ACDID idACD, Player * ptPlayer)
{


    if (ptPlayer->tClientInfo.tTargetLock.tTarget.eType == TT_ACTOR &&
        ptPlayer->tClientInfo.tTargetLock.tTarget.idACDTarget == idACD)
    {



        CPlayerInputCancelMode(ptPlayer, FALSE);

    }

}


void CPlayerInputPickupFailed(BOOL bIsCurrency, const PickupResponse ePickupResponse, Player * ptPlayer)
{



    uint32 dwNow = CGameGetClientTicks();
    if (dwNow > ptPlayer->tClientInfo.dwNextTimeCanSayImFull)
    {
        RActorID idRActor = ptPlayer->idPlayerRActor;
        if (idRActor != RACTORID_INVALID)
        {


            int32 eEffect;
            switch (ePickupResponse)
            {
                case PICKUPRESPONSE_ITEM_LIMIT: eEffect = bIsCurrency ? 19 : 18; break;
                case PICKUPRESPONSE_INVENTORY_FULL: eEffect = 21; break;
                case PICKUPRESPONSE_NOT_YOURS: eEffect = 20; break;
                default: eEffect = 17; break;
            }












            RActorPlayEffectFromMessage(idRActor, eEffect, 0, 0);
        }
        ptPlayer->tClientInfo.dwNextTimeCanSayImFull = dwNow + 180;
    }

}

void CPlayerInputPlayedNonPreplayedAnimPower(Player * ptPlayer)
{

    ptPlayer->tClientInfo.dwTimeLastNonPreplayedAnimatingPower = 0;
    ptPlayer->tClientInfo.snoCurrentLoopingPower = SNO_NONE;
}



void CPlayerInputResetLoopingPower(Player * ptPlayer)
{

    ptPlayer->tClientInfo.snoCurrentLoopingPower = SNO_NONE;
}



SNO CPlayerInputGetCurrentAnimPower(Player * ptPlayer)
{

    return ptPlayer->tClientInfo.snoCurrentAnimPower;
}




void PlayersSetMainPlayerRActor(Player * ptPlayer, RActorID idRActor)
{



    ptPlayer->idPlayerRActor = idRActor;

    sCPlayerInputReset(ptPlayer);

    CGameGlobals & tCGameGlobals = CGameGlobalsGet();

    if (PlayerGetLocalTeam() == ptPlayer->eTeam)
    {
        if (tCGameGlobals.GetCameraMode() != CAMERAMODE_FREE_CAMERA)
        {



            if (!GameIsStartUp())
            {

                CinematographyFixedView(idRActor);
                if (GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1) &&
                    ptPlayer->eTeam == 3)
                {

                    CinematographySetYaw(PI, TRUE, 0.0f);
                }


                CinematographyUpdate(1.0f / 60.0f, TRUE);
            }
        }
        else
        {


            CinematographyStop();
        }
    }


    if (idRActor != RACTORID_INVALID)
    {

        if (GameIsStartUp())
        {

            RActor * ptRActor = RActorGet(idRActor);
            ptRActor->dwFlags |= 4;
        }
        else
        {


            CPlayerAppearanceUpdate(ptPlayer->idPlayerACD);

            Weather::AddPlayerLights(GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData(), idRActor);
            UIGame::InitializeForPlayer();


            UIHUD::Console::InitializeForPlayer(ptPlayer);
            UISkills::Console::InitializeForPlayer(ptPlayer->nPlayerIndex);
            UIParagonBonuses::Console::InitializeForPlayer(ptPlayer->nPlayerIndex);
        }
    }
    else
    {


        Player * ptValidPlayerForLights = LocalPlayerGetValidRActor();
        if (ptValidPlayerForLights)
        {

            Weather::AddPlayerLights(GetRWindowMgr()->GetCurrentRWindow().GetWeatherRWindowData(), ptValidPlayerForLights->idPlayerRActor);
        }
    }


}

void CPlayerInputSetAnimSeed(Player * ptPlayer, const uint32 dwSyncedAnimSeed)
{

    RandSetSeed_Internal(ptPlayer->tClientInfo.rngSyncedAnim, dwSyncedAnimSeed);
}




void CPlayerInputForceHighlightUpdate(Player * ptPlayer)
{

    ptPlayer->tClientInfo.bQueuedHighlightForceUpdate = TRUE;
}























void PlayerSendSkillChangeToServer(const SkillButton eSkillButton, const HotbarButtonData & tButtonData, Player * ptPlayer)
{



    if (!ptPlayer)
        return;




    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACD)
        return;





    const HotbarButtonData & tButtonDataCurrent = PlayerGetHotbarBtnAssignment(ptPlayer, eSkillButton);
    if (tButtonDataCurrent == tButtonData)
        return;




    if (tButtonData.m_snoPower == SNO_NONE)
    {

        UnassignSkillMessage tMessage;
        tMessage.nSlot = eSkillButton;
        MessageSendToServer(CMSG_UNASSIGN_SKILL, &tMessage, sizeof(tMessage), ptPlayer);
    }
    else
    {



        if (!PowerIsCoolDownDone(*ptACD, tButtonData.m_snoPower))
            return;





        AssignSkillMessage tMessage;
        tMessage.snoPower = tButtonData.m_snoPower;
        tMessage.nRuneType = tButtonData.m_eRuneType;
        tMessage.nSlot = eSkillButton;

        MessageSendToServer(CMSG_TRY_ASSIGN_SKILL_TO_SLOT, &tMessage, sizeof(tMessage), ptPlayer);
    }


}

















void PlayerSendPotionSlotChangeToServer(const HotbarButtonData & tButtonData, Player * ptPlayer)
{


    if (!ptPlayer)
        return;



    if (ptPlayer->idPlayerACD == ACDID_INVALID)
        return;



    const HotbarButtonData & tButtonDataCurrent = PlayerGetPotionBtnAssignment(ptPlayer);
    if (tButtonDataCurrent == tButtonData)
        return;

    ChangeUsableItemMessage tMessage;

    if (tButtonData.m_annItem != ANN_INVALID)
    {

        ActorCommonData * ptACDItem = ACDTryToGet(ACDFromANN(tButtonData.m_annItem));
        if (!ptACDItem ||
            !ptACDItem->IsInInventory() ||
            ptACDItem->GetInventoryLocation().idACDContainer != ptPlayer->idPlayerACD)
        {


            return;
        }
    }

    tMessage.annItem = tButtonData.m_annItem;
    tMessage.gbidItem = tButtonData.m_gbidItem;
    MessageSendToServer(CMSG_PLAYER_CHANGE_POTION_BUTTON, &tMessage, sizeof(tMessage), ptPlayer);

    PlayerSetPotionBtnAssignment(ptPlayer, tButtonData);
}

void CPlayerInputOtherListenerReceivedAction(Player * ptPlayer, UIAction eAction, uint32 nDelay)
{








    ptPlayer->tClientInfo.dwGameTickOfLastActionOtherListener[eAction] = CGameGetClientTicks() + nDelay;
}



int32 CPlayerInputGetComboLevel(Player * ptPlayer)
{
    if (ptPlayer)
        return ptPlayer->tClientInfo.nComboLevel;
    return -1;
}


void CPlayerInputSetComboLevel(int32 nComboLevel, Player * ptPlayer)
{


    if (!ptPlayer)
        return;




    ptPlayer->tClientInfo.nComboLevel = nComboLevel;

    ActorCommonData * ptACDLocalPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDLocalPlayer)

        CACDAttributeSetValue(*ptACDLocalPlayer, FastAttribKey(ATTRIBUTE_COMBO_LEVEL), FastAttribValue(nComboLevel), FALSE);

}


















































static BOOL sPlayerHeldActionIsReady(
    const Player * ptPlayer,
    UIAction eAction)
{
    return ptPlayer->tClientInfo.dwGameTickOfLastActionOtherListener[eAction] < CGameGetClientTicks();
}








static BOOL sPlayerCanProcessHeldActionImmediately(
    Player * ptPlayer,
    UIAction eAction)
{
    for (int32 i = 0; i < numButtonActions; i++)
    {
        if (arButtonActions[i] == eAction)
            return (ptPlayer->tClientInfo.dwDownEventBits & (1 << i)) != 0;
    }
    return FALSE;
}






static void sPlayerRememberButtonClickForHeldAction(Player * ptPlayer, UIAction eAction, BOOL bSet)
{






    for (int32 i = 0; i < numButtonActions; i++)
    {
        if (arButtonActions[i] == eAction)
        {
            if (bSet) ptPlayer->tClientInfo.dwDownEventBits |= (1 << i); else ptPlayer->tClientInfo.dwDownEventBits &= ~(1 << i);
            break;
        }
    }
}












static void sPlayerRememberButtonClickForHeldInput(Player * ptControllingPlayer, const BufferedInput & tInput, BOOL bSet)
{



    if (bSet) ptControllingPlayer->tClientInfo.dwDownEventBits |= (1 << tInput.tBufControllerData.eInputButton); else ptControllingPlayer->tClientInfo.dwDownEventBits &= ~(1 << tInput.tBufControllerData.eInputButton);

}

// Lines follow the original line table (scope line 7170). The 64-bit store at +0xc728 in the original also zeroes
void CPlayerInputClearHeldButton(const BufferedInput & tInput)
// bIsHoldingReticlePowerDown; line 7185 for that statement is inferred (see build/try/hdrB/NOTES.txt).
{
    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
    if (ptControllingPlayer->tClientInfo.dwDownEventBits & (1 << tInput.tBufControllerData.eInputButton))
    {
        sPlayerRememberButtonClickForHeldInput(ptControllingPlayer, tInput, FALSE);


        SNO snoPower = PlayerGetPowerSNOFromBufferedInput(*ptControllingPlayer, tInput);
        if (snoPower != SNO_NONE)
        {

            if (snoPower == ptControllingPlayer->tTargetComponent.GetReticlePower())
            {
                ptControllingPlayer->tTargetComponent.ResetTargetRange();
                ptControllingPlayer->tTargetComponent.SetInReticleMode(FALSE, SNO_NONE);
                ptControllingPlayer->tTargetComponent.SetIsHoldingReticlePowerDown(FALSE);
            }


            UIAction eAction = Prefs::GetUIActionFromInput(tInput);
            ptControllingPlayer->tClientInfo.dwGameTickOfLastAction[eAction] = 0;
            ptControllingPlayer->tClientInfo.dwGameTickOfLastHeldAction[eAction] = 0;



            if (ptControllingPlayer->tClientInfo.nUIAction == eAction)
            {
                CPlayerInputCancelMode(ptControllingPlayer, FALSE);
            }
        }
    }
}






void CPlayerInputClearCachedPower(Player * ptPlayer)
{

    ptPlayer->tClientInfo.dwTimeInputWasCached = 0;

    ptPlayer->tClientInfo.tCachedTargetLock.Clear();
}


void CPlayerInputClearClientWalkPower(Player * ptPlayer)
{

    sSendClientWalkAbortToServer(*ptPlayer);
}

