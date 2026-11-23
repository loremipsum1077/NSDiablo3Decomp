// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIGame.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 SkillButton;

typedef int32 ResourceType;

typedef int32 CrafterType;

typedef int32 HirelingClass;


#include "../../../Common/CommonTypes.h"
#include "../UIID.h"
#include "../../../Common/SNO/SNOFiles.h"

typedef int32 GBID;  // X/Common/GameBalance.h
class CRefString;
typedef uint32 StringLabelHandle;
struct CalloutEvent;
struct Player;
struct HotbarButtonData;
struct RActor;
struct RGBAColor;

// Only what has been needed so far.
namespace UIGame {
BOOL IsShowingNotification();
void UpdateCalloutDisplay(CalloutEvent * ptEvent, const uint8 bOpacity);
int32 HUD_ClickPredicate(const UIID &);
void BugOpen_OnClick(const UIID &);
void InitializeFor_Server();
void DisposeFor_Server();
void SetMonsterHPBar(const ACDID idACD);
void SetBossHPBar(const ACDID idACD);
ACDID GetBossHPBarACD();
void SetRActorHighlight(RActorID idRActor, int32 nControllerIndex);
RActorID GetRActorHighlight(int32 nControllerIndex);
void SetRActorAlternateHighlight(RActorID idRActor, int32 nControllerIndex);
RActorID GetRActorAlternateHighlight(int32 nControllerIndex);
void ClearRActorAlternateHighlightPrompt(int32 nControllerIndex);
void SetSpecialAreaText(LPCSTR uszText, LPCSTR uszTextSubtitle);
void ChallengeRift_SetTargetTimeNotification();
void DisplayErrorString(const CRefString & sError);
void Panel_ToolTip(const UIID & id);
BOOL MenuIsActive();
BOOL ModalIsActive();
void AddHUDUntrackedEffectGroup(const SNO snoEffectGroup);
void ToggledMonsterHPBars();
void ToggledPlayerHPBars();
void ToggledItemsOnGround();
void SetBreathMeter(const float flUnitSuffocation);
void HideLoopingAnimMeter(const int32 nControllerIndex);
void StartTrialTimer(const uint32 dwDelay);
UIID GetPlayerHUDID(const int32 nHUDIndex);
void ShowSavePointButton();
void ShowTransmogRewardNotification(ACDID idACDPlayer, GBID gbidTransmog);
void ShowNewCrafterNotification(Player * ptPlayer, CrafterType eCrafterType);
void ShowNewHirelingNotification(Player * ptPlayer, HirelingClass eHirelingClass);
void InvOpen_OnClick(const UIID &);
void SetGameText(const CRefString & usMsg);
void ConfirmSkipEvent();
void DisplayItemRequiredError(const GBID gbidItem, const StringLabelHandle hLabel);
void ShowAltLevelUpButton(ACDID idACDPlayer);
void OnPlayerLeavingGame(const Player * ptPlayerLeaving);
void InitializeForPlayer();
void UpdateRActorTooltips();
void UpdateRActorHighlights();
void AddTagToACD(ACDID idACD, LPCSTR szUIControlName, LPCSTR szText, const SNO snoFont, const int32 nFontSize, const RGBAColor rgbaText, const RGBAColor rgbaDropShadow, const uint32 dwDuration, const float flYOffset);
void ApplyFreeCastEffects(Player * ptPlayer, const int32 nSkillButtonIndex);
void UpdateACDTags();
void SetLoopingAnimMeter(const RActor & tRActor, const float flUnitLoopingAnim);
void ApplyActiveBuffButtonEffect(const Player * ptPlayer, const SkillButton eSkill, const HotbarButtonData & tButtonData);
}  // namespace UIGame
