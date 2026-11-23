// Original file: X/Client/UserInterface/Interface/UICallouts.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). The file has no header in the DWARF, so its
// types are defined here. Inline helpers are written from their inlined copies.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UIID.h"
#include "../UI.h"
#include "../UIStoryboard.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Console/ConsoleScreenManager.h"
#include "../../Battlenet/Subsystems/FanfareManager.h"
#include "../../CGame.h"
#include "../../CPlayer.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Callouts.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Sigma/Random.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/Math/Vector.h"

// Declared here because their headers are not adopted yet; each comment names the defining file.
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace UIUtils { namespace Console { BOOL IsJapaneseVersion(); } }  // UIConsoleUtils.cpp:1443
UIRActor & UI_GetRActorFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp:5757
float PlayerGetStreakTimerTolerance(const Player & tPlayer);  // X/Common/Player.cpp:5337
void EffectGroupCreateUntracked(const SNO snoEffectGroup, const RActorID idRActorSource, const uint32 dwAttachmentID, const BOOL bIgnoreOwnerAlpha, const BOOL bRemote);  // X/Client/EffectGroup.cpp:1299


static const int32 NUMBER_OF_FLOATING_TEXT = 5;
static const float FLOATING_VERTICAL_VELOCITY_INITIAL = -14.0f;
static const float FLOATING_VERTICAL_VELOCITY_SCALAR = 0.73f;
static const int32 FLOATING_TRANSPARENCY_VELOCITY = 10;

// UIIDs defined in X/Client/UserInterface/UIGlobals.cpp.
extern UIID UIRACTOR_CALLOUT_MASSACRE_TIMER_SPARK, UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_INTRO, UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_OUTRO, UISTORYBOARD_CALLOUT_DESTRUCTION_REPORT_INTRO, UISTORYBOARD_CALLOUT_DESTRUCTION_REPORT_OUTRO, UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_INTRO, UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_OUTRO, UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_INTRO, UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_OUTRO, UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_TIERUP;
extern UIID UISTORYBOARD_CALLOUT_MASSACRE_REPORT_INTRO, UISTORYBOARD_CALLOUT_MASSACRE_REPORT_OUTRO, UITXT_CALLOUT_DESTRUCTION_PROGRESSION_DESTRUCTIONCOUNT, UITXT_CALLOUT_DESTRUCTION_REPORT_DESTRUCTIONCOUNT, UITXT_CALLOUT_DESTRUCTION_REPORT_REWARD, UITXT_CALLOUT_DESTRUCTION_REPORT_TITLE, UITXT_CALLOUT_ENVIRONMENT_KILL_KILLCOUNT, UITXT_CALLOUT_ENVIRONMENT_KILL_REWARD, UITXT_CALLOUT_ENVIRONMENT_KILL_TITLE, UITXT_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT;
extern UIID UITXT_CALLOUT_MASSACRE_PROGRESSION_TITLE, UITXT_CALLOUT_MASSACRE_REPORT_KILLCOUNT, UITXT_CALLOUT_MASSACRE_REPORT_REWARD, UITXT_CALLOUT_MASSACRE_REPORT_TITLE, UIWND_CALLOUT_ARCADE_COMBAT_WRAPPER, UIWND_CALLOUT_DESTRUCTION_PROGRESSION_WRAPPER, UIWND_CALLOUT_DESTRUCTION_REPORT_TITLEJP, UIWND_CALLOUT_DESTRUCTION_REPORT_WRAPPER, UIWND_CALLOUT_ENVIRONMENT_KILL_TITLEJP, UIWND_CALLOUT_ENVIRONMENT_KILL_WRAPPER;
extern UIID UIWND_CALLOUT_MASSACRE_PROGRESSION_BACKGROUND_SPLAT, UIWND_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT_WRAPPER, UIWND_CALLOUT_MASSACRE_PROGRESSION_TITLEJP, UIWND_CALLOUT_MASSACRE_PROGRESSION_WRAPPER, UIWND_CALLOUT_MASSACRE_REPORT_TITLEJP, UIWND_CALLOUT_MASSACRE_REPORT_WRAPPER, UIWND_CALLOUT_MASSACRE_TIMER_BG, UIWND_CONSOLE_HUD;

namespace UICallouts {

void ReportStart(const CalloutEvent & tEvent);
void EnvironmentKillStamp(const CalloutEvent & tEvent);
void RefreshMassacreTimer();
void MassacreStart(const CalloutEvent & tEvent, int32 nLevel);
void MassacreNewTier(int32 nLevel);
void MassacreEffectsForLevel(int32 nLevel);
void FloatText(CalloutEventType eType);
void UpdateBigStamps();
void HideCurrentVisibleBigStamp();
void AnimDoneMassacreProgressionIntro(const UIID &);
void AnimDoneMassacreProgressionOutro(const UIID &);
void AnimDoneMassacreReportIntro(const UIID &);
void AnimDoneMassacreReportOutro(const UIID &);
void AnimDoneDestructionProgressionIntro(const UIID &);
void AnimDoneDestructionProgressionOutro(const UIID &);
void AnimDoneDestructionReportIntro(const UIID &);
void AnimDoneDestructionReportOutro(const UIID &);
void AnimDoneEnvironmentKillsIntro(const UIID &);
void AnimDoneEnvironmentKillsOutro(const UIID &);
inline void InitializeBigStampsStoryboardCallbacks(); inline void DestructionStart(const CalloutEvent & tEvent); inline BOOL CalloutEventIsTickerType(const CalloutEventType eType); inline void GetFontColorsForLevel(int32 nLevel, RGBAColor & rgbaTitleColor, RGBAColor & rgbaDropShadow);
































struct FloatingTextInfo {  // size 0x10
    UIID idText;  // 0x00
    BOOL bFloating;  // 0x08
    float flVelocity;  // 0x0c



    FloatingTextInfo()
    {

        idText = GetUIIDNone();
        bFloating = FALSE;
        flVelocity = 0.0f;
    }
};
static_assert(sizeof(FloatingTextInfo) == 0x10, "FloatingTextInfo");

struct TickerInfo {  // size 0x10
    BOOL bActive;  // 0x00
    uint32 nCount;  // 0x04
    uint32 uExpireTime;  // 0x08
    uint32 uGracePeriodExpireTime;  // 0x0c

    TickerInfo()
    {


        memset(this, 0, sizeof(*this));
    }
};
static_assert(sizeof(TickerInfo) == 0x10, "TickerInfo");

class ShakeComponent {  // size 0x28
    int32 nShakeDuration;  // 0x00
    int32 nShakeStrengthX;  // 0x04
    int32 nShakeStrengthY;  // 0x08
    UIID idWindow;  // 0x10
    Vector2D vOriginalPos;  // 0x18
    Vector2D vOriginalDim;  // 0x20
public:





    ShakeComponent()
    {

        nShakeDuration = 0;
        nShakeStrengthX = 0;
        nShakeStrengthY = 0;
        idWindow = GetUIIDNone();
    }

    void Initialize(const UIID & id)
    {

        idWindow = id;
        UIWindow & tWindow = UI_GetWindowFromID(id);
        tWindow.GetRelativePosition(vOriginalPos, vOriginalDim);
    }

    void Shake(int32 nDuration, int32 nStrengthX, int32 nStrengthY)



    {
        nShakeDuration = nDuration;
        nShakeStrengthX = nStrengthX;
        nShakeStrengthY = nStrengthY;
    }

    void Update()

    {
        UIWindow & tWindow = UI_GetWindowFromID(idWindow);
        if (nShakeDuration > 0)
        {
            nShakeDuration--;

            Vector2D vNewPos = vOriginalPos;
            Vector2D vNewDim = vOriginalDim;
            vNewPos.x += (RandFloat_Internal(CGameGlobalsGet().rngLocal) - 0.5f) * nShakeStrengthX;
            vNewPos.y += (RandFloat_Internal(CGameGlobalsGet().rngLocal) - 0.5f) * nShakeStrengthY;
            tWindow.SetRelativePosition(vNewPos, vNewDim);
        }
        else
        {
            tWindow.SetRelativePosition(vOriginalPos, vOriginalDim);
        }
    }
};
static_assert(sizeof(ShakeComponent) == 0x28, "ShakeComponent");

enum BigStamp {
    BIG_STAMP_NONE = 0,
    BIG_STAMP_MASSACRE_PROGRESSION = 1,
    BIG_STAMP_MASSACRE_REPORT = 2,
    BIG_STAMP_DESTRUCTION_PROGRESSION = 3,
    BIG_STAMP_DESTRUCTION_REPORT = 4,
    BIG_STAMP_ENVIRONMENT_KILL = 5,
};

enum MassacreProgressionState {
    MASSACRE_PROGRESSION_NONE = 0,
    MASSACRE_PROGRESSION_INTRO = 1,
    MASSACRE_PROGRESSION_WAIT = 2,
    MASSACRE_PROGRESSION_OUTRO = 3,
};

enum MassacreReportState {
    MASSACRE_REPORT_NONE = 0,
    MASSACRE_REPORT_INTRO = 1,
    MASSACRE_REPORT_IDLE = 2,
    MASSACRE_REPORT_OUTRO = 3,
};

enum DestructionProgressionState {
    DESTRUCTION_PROGRESSION_NONE = 0,
    DESTRUCTION_PROGRESSION_INTRO = 1,
    DESTRUCTION_PROGRESSION_WAIT = 2,
    DESTRUCTION_PROGRESSION_OUTRO = 3,
};

enum DestructionReportState {
    DESTRUCTION_REPORT_NONE = 0,
    DESTRUCTION_REPORT_INTRO = 1,
    DESTRUCTION_REPORT_IDLE = 2,
    DESTRUCTION_REPORT_OUTRO = 3,
};

enum EnvironmentKillsState {
    ENVIRONMENT_KILLS_NONE = 0,
    ENVIRONMENT_KILLS_INTRO = 1,
    ENVIRONMENT_KILLS_IDLE = 2,
    ENVIRONMENT_KILLS_OUTRO = 3,
};

struct BigStampInfo {  // size 0x24
    BigStamp eCurrentVisibleBigStamp;  // 0x00
    MassacreProgressionState eMassacreProgressionState;  // 0x04
    MassacreReportState eMassacreReportState;  // 0x08
    DestructionProgressionState eDestructionProgressionState;  // 0x0c
    DestructionReportState eDestructionReportState;  // 0x10
    EnvironmentKillsState eEnvironmentKillState;  // 0x14
    uint32 nMassacreReportIdleTick;  // 0x18
    uint32 nDestructionReportIdleTick;  // 0x1c
    uint32 nEnvironmentKillIdleTick;  // 0x20









    BigStampInfo()
    {


        memset(this, 0, sizeof(*this));
    }
};
static_assert(sizeof(BigStampInfo) == 0x24, "BigStampInfo");

class UICalloutsGlobals {  // size 0x1a0
public:
    int32 nCurrentFloatingText;  // 0x00
    FloatingTextInfo artMassacreFloatingTexts[5];  // 0x08
    FloatingTextInfo artDestructionFloatingTexts[5];  // 0x58
    Vector2D vKillCountOriginalPosition;  // 0xa8
    Vector2D vKillCountOriginalDimensions;  // 0xb0
    Vector2D vDestructionCountOriginalPosition;  // 0xb8
    Vector2D vDestructionCountOriginalDimensions;  // 0xc0
    Vector2D vMassacreTimerOriginalPosition;  // 0xc8
    Vector2D vMassacreTimerOriginalDimensions;  // 0xd0
    int32 nCurrentMassacreLevel;  // 0xd8
    TickerInfo tMassacreTicker;  // 0xdc
    TickerInfo tDestructionTicker;  // 0xec
    ShakeComponent tStampShakeComponent;  // 0x100
    ShakeComponent tKillCountTextShakeComponent;  // 0x128
    ShakeComponent tDestructionCountTextShakeComponent;  // 0x150
    BigStampInfo tBigStampInfo;  // 0x178
















    UICalloutsGlobals()

    {
        nCurrentMassacreLevel = 0;
        nCurrentFloatingText = 0;
    }

    UIText & GetFloatingText(int32 nFloatingTextNumber, CalloutEventType eType)

    {

        if (eType == 1)  // massacre
        {

            return UI_GetTextFromID(artMassacreFloatingTexts[nFloatingTextNumber].idText);
        }

        return UI_GetTextFromID(artDestructionFloatingTexts[nFloatingTextNumber].idText);
    }


    void Initialize()

    {
        for (int32 nIndex = 0; nIndex < NUMBER_OF_FLOATING_TEXT; nIndex++)
        {
            CRefString sControlName;
            sControlName.Format(".KillCount_Float%i", nIndex);
            artMassacreFloatingTexts[nIndex].idText = UIIDCreateFromParent(UIWND_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT_WRAPPER, sControlName);
            sControlName.Format(".DestructionCount_Float%i", nIndex);
            artDestructionFloatingTexts[nIndex].idText = UIIDCreateFromParent(UIWND_CALLOUT_DESTRUCTION_PROGRESSION_WRAPPER, sControlName);
        }

        UIText & tKillCountText = UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT);
        tKillCountText.GetRelativePosition(vKillCountOriginalPosition, vKillCountOriginalDimensions);

        UIText & tDestructionCountText = UI_GetTextFromID(UITXT_CALLOUT_DESTRUCTION_PROGRESSION_DESTRUCTIONCOUNT);
        tDestructionCountText.GetRelativePosition(vDestructionCountOriginalPosition, vDestructionCountOriginalDimensions);

        UIWindow & tMassacreTimer = UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_TIMER_BG);
        tMassacreTimer.GetRelativePosition(vMassacreTimerOriginalPosition, vMassacreTimerOriginalDimensions);

        tStampShakeComponent.Initialize(UIWND_CALLOUT_MASSACRE_PROGRESSION_WRAPPER);
        tKillCountTextShakeComponent.Initialize(UITXT_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT);
        tDestructionCountTextShakeComponent.Initialize(UITXT_CALLOUT_DESTRUCTION_PROGRESSION_DESTRUCTIONCOUNT);

        InitializeBigStampsStoryboardCallbacks();


        if (UIUtils::Console::IsJapaneseVersion())
        {

            UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_REPORT_TITLEJP).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_CALLOUT_ENVIRONMENT_KILL_TITLEJP).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_TITLEJP).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_REPORT_TITLEJP).SetActive(TRUE);

            UI_GetWindowFromID(UITXT_CALLOUT_DESTRUCTION_REPORT_TITLE).SetActive(FALSE);
            UI_GetWindowFromID(UITXT_CALLOUT_ENVIRONMENT_KILL_TITLE).SetActive(FALSE);
            UI_GetWindowFromID(UITXT_CALLOUT_MASSACRE_PROGRESSION_TITLE).SetActive(FALSE);
            UI_GetWindowFromID(UITXT_CALLOUT_MASSACRE_REPORT_TITLE).SetActive(FALSE);
        }

    }
};
static_assert(sizeof(UICalloutsGlobals) == 0x1a0, "UICalloutsGlobals");
UICalloutsGlobals * sgptUICalloutsGlobals = NULL;

inline BOOL ShouldIgnoreCalloutEvent(const CalloutEventType eType);
inline BOOL IsReportType(const CalloutEventType eType);
void NewCalloutEvent(const CalloutEvent & tEvent)

{
    if (ShouldIgnoreCalloutEvent(tEvent.eType))
    {
        return;
    }

    if (IsReportType(tEvent.eType))
    {
        ReportStart(tEvent);
    }
    else if (tEvent.eType == 4)  // environment kill
    {
        EnvironmentKillStamp(tEvent);
    }
}

inline CRefString GetMassacreLabel(int32 nCount, BOOL bReport)


{
    Callouts::MassacreDef tMassacre = Callouts::GetMassacreDef(nCount);
    CRefString sLabel;
    if (bReport)
    {
        sLabel.Format("MassacreReport_Level%i", tMassacre.nLevel + 1);
    }
    else
    {
        sLabel.Format("MassacreActive_Level%i", tMassacre.nLevel + 1);
    }
    return sLabel;
}

void IncreasedCount(CalloutEvent * ptEvent)

{

    if (!ptEvent || ::Console::Screens::GetFanfareManager().IsActive())
    {
        return;
    }








    BOOL bFirstTime = FALSE;

    if (ptEvent->eType == 1)  // massacre
    {
        if (!sgptUICalloutsGlobals->tMassacreTicker.bActive)
        {
            bFirstTime = TRUE;
        }
        sgptUICalloutsGlobals->tMassacreTicker.bActive = TRUE;
        sgptUICalloutsGlobals->tKillCountTextShakeComponent.Shake(6, 0, 5);
    }
    else if (ptEvent->eType == 2)  // destruction
    {
        if (!sgptUICalloutsGlobals->tDestructionTicker.bActive)
        {
            bFirstTime = TRUE;
        }
        sgptUICalloutsGlobals->tDestructionTicker.bActive = TRUE;
        sgptUICalloutsGlobals->tDestructionCountTextShakeComponent.Shake(6, 0, 5);
    }

    SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS);
    SNO snoConsoleCalloutsStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CALLOUTS);

    if (ptEvent->eType == 1)  // massacre
    {
        int32 nNewMassacreLevel = Callouts::GetMassacreDef(ptEvent->uCount).nLevel;
        if (bFirstTime)
        {






            uint32 uNow = GameGetServerTime();
            const GlobalDefinition * pdefGlobals = DefinitionLockR<GlobalDefinition>();
            float flGracePeriod = pdefGlobals->flMassacreStartingTimer;
            sgptUICalloutsGlobals->tMassacreTicker.uGracePeriodExpireTime = uNow + (uint32)(flGracePeriod * 60.0f);


            MassacreStart(*ptEvent, nNewMassacreLevel);
            sgptUICalloutsGlobals->nCurrentMassacreLevel = 0;
        }
        else
            RefreshMassacreTimer();
        if (nNewMassacreLevel > sgptUICalloutsGlobals->nCurrentMassacreLevel)
        {
            MassacreNewTier(nNewMassacreLevel);
        }
        sgptUICalloutsGlobals->nCurrentMassacreLevel = nNewMassacreLevel;
    }
    else if (bFirstTime && ptEvent->eType == 2)  // destruction
    {
        DestructionStart(*ptEvent);
    }



    if (!bFirstTime)
    {

        FloatText(ptEvent->eType);
    }

    if (ptEvent->eType == 1)  // massacre
    {
        CRefString sMassacreTitle = StringListFetch(snoConsoleCalloutsStringList, GetMassacreLabel(ptEvent->uCount, FALSE));
        UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_PROGRESSION_TITLE).SetText(sMassacreTitle, FALSE);

        CRefString sKills = UnicodeText::FormatNumber((int32)ptEvent->uCount, TRUE, FNS_NORMAL, 0);
        CRefString sKillCount = LocalizedSprintf(StringListFetch(snoStringList, "ConsoleMassacreTicker"), sKills.str(), NULL);
        UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT).SetText(sKillCount, FALSE);
    }
    else if (ptEvent->eType == 2)  // destruction
    {
        CRefString sVal = UnicodeText::FormatNumber((int32)ptEvent->uCount, TRUE, FNS_NORMAL, 0);
        CRefString sDestructionCount = LocalizedSprintf(StringListFetch(snoStringList, "ConsoleDestructionTicker"), sVal.str(), NULL);
        UI_GetTextFromID(UITXT_CALLOUT_DESTRUCTION_PROGRESSION_DESTRUCTIONCOUNT).SetText(sDestructionCount, FALSE);
    }
}








void FloatText(CalloutEventType eType)

{

    if (::Console::Screens::GetFanfareManager().IsActive() || !CalloutEventIsTickerType(eType))
    {
        return;
    }










    sgptUICalloutsGlobals->nCurrentFloatingText = (sgptUICalloutsGlobals->nCurrentFloatingText + 1) % NUMBER_OF_FLOATING_TEXT;
    UIText & tFloatingText = sgptUICalloutsGlobals->GetFloatingText(sgptUICalloutsGlobals->nCurrentFloatingText, eType);
    FloatingTextInfo * ptFloatingTextInfo = NULL;
    CRefString sPreviousNumber;
    if (eType == 1)  // massacre
    {

        ptFloatingTextInfo = &sgptUICalloutsGlobals->artMassacreFloatingTexts[sgptUICalloutsGlobals->nCurrentFloatingText];
        Vector2D vNewPosition = sgptUICalloutsGlobals->vKillCountOriginalPosition;
        vNewPosition.y += sgptUICalloutsGlobals->vKillCountOriginalDimensions.y * -0.5f;
        tFloatingText.SetRelativePosition(vNewPosition, sgptUICalloutsGlobals->vKillCountOriginalDimensions);
        sPreviousNumber = UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT).GetText().GetText();
    }
    else
    {

        ptFloatingTextInfo = &sgptUICalloutsGlobals->artDestructionFloatingTexts[sgptUICalloutsGlobals->nCurrentFloatingText];
        Vector2D vNewPosition = sgptUICalloutsGlobals->vDestructionCountOriginalPosition;
        vNewPosition.y += sgptUICalloutsGlobals->vDestructionCountOriginalDimensions.y * -0.5f;
        tFloatingText.SetRelativePosition(vNewPosition, sgptUICalloutsGlobals->vDestructionCountOriginalDimensions);
        sPreviousNumber = UI_GetTextFromID(UITXT_CALLOUT_DESTRUCTION_PROGRESSION_DESTRUCTIONCOUNT).GetText().GetText();
    }
    ptFloatingTextInfo->bFloating = TRUE;
    ptFloatingTextInfo->flVelocity = FLOATING_VERTICAL_VELOCITY_INITIAL;
    tFloatingText.SetTransparency(128);
    tFloatingText.SetActive(TRUE);
    tFloatingText.SetText(sPreviousNumber, FALSE);
}

void UpdateFloatingText(CalloutEventType eType)

{


    FloatingTextInfo * ptFloatingTextInfoArray = NULL;
    if (eType == 1)  // massacre
    {
        ptFloatingTextInfoArray = sgptUICalloutsGlobals->artMassacreFloatingTexts;
    }
    else if (eType == 2)  // destruction
    {
        ptFloatingTextInfoArray = sgptUICalloutsGlobals->artDestructionFloatingTexts;
    }
    else
    {
        return;
    }

    for (int32 nIndex = 0; nIndex < NUMBER_OF_FLOATING_TEXT; nIndex++)
    {
        FloatingTextInfo & tFloatingTextInfo = ptFloatingTextInfoArray[nIndex];
        if (tFloatingTextInfo.bFloating)
        {
            UIText & tText = sgptUICalloutsGlobals->GetFloatingText(nIndex, eType);


            Vector2D vPosition, vDimensions;
            tText.GetRelativePosition(vPosition, vDimensions);
            tFloatingTextInfo.flVelocity *= FLOATING_VERTICAL_VELOCITY_SCALAR;
            vPosition.y += tFloatingTextInfo.flVelocity;
            tText.SetRelativePosition(vPosition, vDimensions);


            int32 nTransparency = tText.GetTransparency();
            if (nTransparency < FLOATING_TRANSPARENCY_VELOCITY)
            {
                nTransparency = 0;
                tFloatingTextInfo.bFloating = FALSE;
                tText.SetActive(FALSE);
            }
            else
                nTransparency -= FLOATING_TRANSPARENCY_VELOCITY;
            tText.SetTransparency(nTransparency);
        }
    }
}



void Update()

{

    BOOL bShow = !::Console::Screens::GetFanfareManager().IsActive() && UI_GetWindowFromID(UIWND_CONSOLE_HUD).IsActive();




    UIWindow & tArcadeCombatWrapper = UI_GetWindowFromID(UIWND_CALLOUT_ARCADE_COMBAT_WRAPPER);
    tArcadeCombatWrapper.SetActive(bShow);

    UpdateFloatingText(1);  // massacre
    UpdateFloatingText(2);  // destruction



    if (sgptUICalloutsGlobals->tBigStampInfo.eMassacreProgressionState == MASSACRE_PROGRESSION_WAIT)
    {
        sgptUICalloutsGlobals->tStampShakeComponent.Update();
    }
    sgptUICalloutsGlobals->tKillCountTextShakeComponent.Update();
    sgptUICalloutsGlobals->tDestructionCountTextShakeComponent.Update();



    uint32 uNow = CGameGetPredictedServerTime();
    TickerInfo & tMassacreTicker = sgptUICalloutsGlobals->tMassacreTicker;

    UIWindow & tMassacreTimerBG = UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_TIMER_BG);
    UIRActor & tSpark = UI_GetRActorFromID(UIRACTOR_CALLOUT_MASSACRE_TIMER_SPARK);
    Vector2D vSparkPosition, vSparkDimension;
    tSpark.GetRelativePosition(vSparkPosition, vSparkDimension);

    tMassacreTimerBG.SetActive(tMassacreTicker.bActive);
    tSpark.SetActive(tMassacreTicker.bActive && tMassacreTicker.uGracePeriodExpireTime == 0 && uNow < tMassacreTicker.uExpireTime);

    if (tMassacreTicker.bActive)
    {

        if (tMassacreTicker.uGracePeriodExpireTime != 0)
        {

            if (uNow > tMassacreTicker.uGracePeriodExpireTime)
            {
                uint32 uTimeAfterGracePeriod = tMassacreTicker.uGracePeriodExpireTime;
                Player * ptPlayer = LocalPlayerGet();
                float flStreakTimerTolerance = PlayerGetStreakTimerTolerance(*ptPlayer);
                tMassacreTicker.uExpireTime = uTimeAfterGracePeriod + (uint32)(flStreakTimerTolerance * 60.0f);
                tMassacreTicker.uGracePeriodExpireTime = 0;
            }
            else
            {


                tMassacreTimerBG.SetRelativeClipRect(sgptUICalloutsGlobals->vMassacreTimerOriginalPosition, sgptUICalloutsGlobals->vMassacreTimerOriginalDimensions);
            }
        }

        else if (uNow < tMassacreTicker.uExpireTime)
        {

            Player * ptPlayer = LocalPlayerGet();
            float flStreakTimerTolerance = PlayerGetStreakTimerTolerance(*ptPlayer);
            float flExpireMinusNow = (float)(tMassacreTicker.uExpireTime - uNow);
            float flTotalExpireTime = flStreakTimerTolerance * 60.0f;
            float flTimeLeftFraction = flExpireMinusNow / flTotalExpireTime;


            Vector2D vMassacreTimerNewDimensions = sgptUICalloutsGlobals->vMassacreTimerOriginalDimensions;
            vMassacreTimerNewDimensions.x *= flTimeLeftFraction;
            tMassacreTimerBG.SetRelativeClipRect(sgptUICalloutsGlobals->vMassacreTimerOriginalPosition, vMassacreTimerNewDimensions);


            vSparkPosition.x = sgptUICalloutsGlobals->vMassacreTimerOriginalPosition.x + vMassacreTimerNewDimensions.x + vSparkDimension.x * -0.5;
            tSpark.SetRelativePosition(vSparkPosition, vSparkDimension);
        }
    }

    UpdateBigStamps();
}
void RefreshMassacreTimer()

{
    Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer)
    {
        uint32 uNow = CGameGetPredictedServerTime();

        if (sgptUICalloutsGlobals->tMassacreTicker.uGracePeriodExpireTime == 0)
        {
            float flStreakTimerTolerance = PlayerGetStreakTimerTolerance(*ptPlayer);
            sgptUICalloutsGlobals->tMassacreTicker.uExpireTime = uNow + (uint32)(flStreakTimerTolerance * 60.0f);
        }
    }
}































inline CRefString GetFormattedTime(uint32 nSeconds)


{
    const uint32 SECONDS_PER_MINUTE = 60;
    const uint32 MINUTES_PER_HOUR = 60;

    uint32 nHours = nSeconds / (SECONDS_PER_MINUTE * MINUTES_PER_HOUR);
    nSeconds -= nHours * SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
    uint32 nMinutes = nSeconds / SECONDS_PER_MINUTE;
    nSeconds -= nMinutes * SECONDS_PER_MINUTE;

    CRefString sFormattedTime;
    if (nHours > 0)
    {
        sFormattedTime.Format("%d:%02d:%02d", nHours, nMinutes, nSeconds);
    }
    else
    {
        sFormattedTime.Format("%d:%02d", nMinutes, nSeconds);
    }
    return sFormattedTime;
}
BOOL IsTickerType(const KillCounterType eType)


{
    return eType == KILLCOUNTER_MONSTER || eType == KILLCOUNTER_OBJECT_DESTRUCTION;
}





inline BOOL CalloutEventIsTickerType(const CalloutEventType eType)


{
    return eType == 1 || eType == 2;  // massacre, destruction
}

















inline BOOL IsReportType(const CalloutEventType eType)


{
    return eType == 1 || eType == 2;  // massacre, destruction
}





inline BOOL ShouldIgnoreCalloutEvent(const CalloutEventType eType)

{
    switch (eType)
    {
    case 3: case 5: case 6: case 7: case 8: case 9: return TRUE;
    }
    return FALSE;
}






void InitializeForGame()

{
    sgptUICalloutsGlobals = new UICalloutsGlobals();
    sgptUICalloutsGlobals->Initialize();
}

void DisposeForGame()

{
    delete sgptUICalloutsGlobals; sgptUICalloutsGlobals = NULL;
}










































void ReportStart(const CalloutEvent & tEvent)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tEvent.eType == 1)  // massacre
    {
        sgptUICalloutsGlobals->tMassacreTicker.bActive = FALSE;

        UIStoryboard & tMassacreProgressionIntro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_INTRO);
        UIStoryboard & tMassacreProgressionOutro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_OUTRO);
        UIStoryboard & tMassacreProgressionTierUp = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_TIERUP);
        tMassacreProgressionIntro.Reset();
        tMassacreProgressionOutro.Reset();
        tMassacreProgressionTierUp.Reset();

        if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_MASSACRE_PROGRESSION)
        {
            tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_OUTRO;
            tMassacreProgressionOutro.Play(FALSE, 0.0f);
        }
        else
        {

            HideCurrentVisibleBigStamp();
            UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_REPORT_WRAPPER).SetActive(TRUE);
            UIStoryboard & tMassacreReportIntro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_REPORT_INTRO);
            tMassacreReportIntro.Reset();
            tMassacreReportIntro.Play(FALSE, 0.0f);
            tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_NONE;
            tBigStampInfo.eMassacreReportState = MASSACRE_REPORT_INTRO;
            tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_MASSACRE_REPORT;
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_REPORT_INTRO), cgtPSODefaults);
        }



        SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS);
        SNO snoConsoleCalloutsStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CALLOUTS);

        CRefString sTitle, sKillCount, sReward;
        CRefString sKills = UnicodeText::FormatNumber((int32)tEvent.uCount, TRUE, FNS_NORMAL, 0);
        CRefString sExp = UnicodeText::FormatNumber(tEvent.flXPMultiplier, TRUE, FNS_NORMAL, 0, 2, 0);

        sTitle = StringListFetch(snoConsoleCalloutsStringList, GetMassacreLabel(tEvent.uCount, TRUE));
        sKillCount = LocalizedSprintf(StringListFetch(snoConsoleCalloutsStringList, "MassacreReport_KillCount"), sKills.str(), NULL);
        sReward = LocalizedSprintf(StringListFetch(snoStringList, "ExperienceBonus"), sExp.str(), NULL);

        UIText & tReportTitle = UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_REPORT_TITLE);
        tReportTitle.SetText(sTitle, FALSE);
        UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_REPORT_KILLCOUNT).SetText(sKillCount, FALSE);
        UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_REPORT_REWARD).SetText(sReward, FALSE);


        if (UIUtils::Console::IsJapaneseVersion())
        {
            int32 nLevel = Callouts::GetMassacreDef(tEvent.uCount).nLevel;
            CRefString sReportTexture = CRefString().Format("UI:Console_TextFX_MassacreReport%dJP", nLevel + 1);
            UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_REPORT_TITLEJP).SetTextureFrame(sReportTexture);
        }



        int32 nMassacreLevel = Callouts::GetMassacreDef(tEvent.uCount).nLevel;
        RGBAColor rgbaTitleColor, rgbaDropShadow;
        GetFontColorsForLevel(nMassacreLevel, rgbaTitleColor, rgbaDropShadow);
        tReportTitle.SetTextColor(rgbaTitleColor);
        tReportTitle.SetDropShadowColor(rgbaDropShadow);


        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_MASSACRE_REPORT), cgtPSODefaults);
    }



    else if (tEvent.eType == 2)  // destruction
    {
        sgptUICalloutsGlobals->tDestructionTicker.bActive = FALSE;

        UIStoryboard & tDestructionProgressionIntro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_INTRO);
        UIStoryboard & tDestructionProgressionOutro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_OUTRO);
        tDestructionProgressionIntro.Reset();
        tDestructionProgressionOutro.Reset();

        if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_DESTRUCTION_PROGRESSION)
        {
            tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_OUTRO;
            tDestructionProgressionOutro.Play(FALSE, 0.0f);

            SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_CALLOUTS);

            CRefString sObjectsDestroyed = UnicodeText::FormatNumber((int32)tEvent.uCount, TRUE, FNS_NORMAL, 0);
            CRefString sFormattedObjectsDestroyed = LocalizedSprintf(StringListFetch(snoStringList, "ConsoleDestructionTicker"), sObjectsDestroyed.str(), NULL);
            CRefString sReward = LocalizedSprintf(StringListFetch("X1_ConsoleCallouts:DestructionReport_Reward"), GetFormattedTime(tEvent.uTotalTime).str(), NULL);

            UI_GetTextFromID(UITXT_CALLOUT_DESTRUCTION_REPORT_DESTRUCTIONCOUNT).SetText(sFormattedObjectsDestroyed, FALSE);
            UI_GetTextFromID(UITXT_CALLOUT_DESTRUCTION_REPORT_REWARD).SetText(sReward, FALSE);

            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_DESTRUCTION_REPORT), cgtPSODefaults);
        }
        else
        {

            tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_NONE;
            UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_PROGRESSION_WRAPPER).SetActive(FALSE);
        }
    }
}

inline void InitializeBigStampsStoryboardCallbacks()

{
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_INTRO).SetOnEndCallback(AnimDoneMassacreProgressionIntro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_OUTRO).SetOnEndCallback(AnimDoneMassacreProgressionOutro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_REPORT_INTRO).SetOnEndCallback(AnimDoneMassacreReportIntro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_REPORT_OUTRO).SetOnEndCallback(AnimDoneMassacreReportOutro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_INTRO).SetOnEndCallback(AnimDoneDestructionProgressionIntro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_OUTRO).SetOnEndCallback(AnimDoneDestructionProgressionOutro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_REPORT_INTRO).SetOnEndCallback(AnimDoneDestructionReportIntro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_REPORT_OUTRO).SetOnEndCallback(AnimDoneDestructionReportOutro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_INTRO).SetOnEndCallback(AnimDoneEnvironmentKillsIntro);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_OUTRO).SetOnEndCallback(AnimDoneEnvironmentKillsOutro);
}

void UpdateBigStamps()

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    uint32 uNow = GameGetServerTime();


    if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_MASSACRE_REPORT &&
        tBigStampInfo.eMassacreReportState == MASSACRE_REPORT_IDLE)
    {
        if (uNow > tBigStampInfo.nMassacreReportIdleTick)
        {
            tBigStampInfo.nMassacreReportIdleTick = 0;
            tBigStampInfo.eMassacreReportState = MASSACRE_REPORT_OUTRO;
            UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_REPORT_OUTRO).Play(FALSE, 0.0f);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_REPORT_OUTRO), cgtPSODefaults);
        }
    }

    else if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_DESTRUCTION_REPORT &&
        tBigStampInfo.eDestructionReportState == DESTRUCTION_REPORT_IDLE)
    {
        if (uNow > tBigStampInfo.nDestructionReportIdleTick)
        {
            tBigStampInfo.nDestructionReportIdleTick = 0;
            tBigStampInfo.eDestructionReportState = DESTRUCTION_REPORT_OUTRO;
            UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_REPORT_OUTRO).Play(FALSE, 0.0f);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_REPORT_OUTRO), cgtPSODefaults);
        }
    }

    else if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_ENVIRONMENT_KILL &&
        tBigStampInfo.eEnvironmentKillState == ENVIRONMENT_KILLS_IDLE)
    {
        if (uNow > tBigStampInfo.nEnvironmentKillIdleTick)
        {
            tBigStampInfo.nEnvironmentKillIdleTick = 0;
            tBigStampInfo.eEnvironmentKillState = ENVIRONMENT_KILLS_OUTRO;
            UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_OUTRO).Play(FALSE, 0.0f);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_REPORT_OUTRO), cgtPSODefaults);
        }
    }


    if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_NONE)
    {

        if (tBigStampInfo.eMassacreProgressionState != MASSACRE_PROGRESSION_NONE)
        {
            tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_MASSACRE_PROGRESSION;

            UIStoryboard & tMassacreProgressionIntro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_INTRO);
            UIStoryboard & tMassacreProgressionTierUp = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_TIERUP);
            tMassacreProgressionIntro.Reset();
            tMassacreProgressionIntro.Play(FALSE, 0.0f);
            tMassacreProgressionTierUp.Reset();
            tMassacreProgressionTierUp.Play(FALSE, 0.0f);
            UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_WRAPPER).SetActive(TRUE);
            UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT_WRAPPER).SetActive(TRUE);
        }
        else if (tBigStampInfo.eDestructionProgressionState != DESTRUCTION_PROGRESSION_NONE)
        {
            tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_DESTRUCTION_PROGRESSION;

            UIStoryboard & tDestructionProgressionIntro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_INTRO);
            tDestructionProgressionIntro.Reset();
            tDestructionProgressionIntro.Play(FALSE, 0.0f);
            UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_PROGRESSION_WRAPPER).SetActive(TRUE);
        }
    }
}

void MassacreStart(const CalloutEvent & tEvent, int32 nLevel)


{

    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    MassacreEffectsForLevel(nLevel);

    if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_MASSACRE_REPORT ||
        tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_ENVIRONMENT_KILL ||
        tBigStampInfo.eMassacreProgressionState == MASSACRE_PROGRESSION_OUTRO)
    {
        tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_WAIT;
        return;
    }

    HideCurrentVisibleBigStamp();
    tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_MASSACRE_PROGRESSION;
    tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_INTRO;
    UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_WRAPPER).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT_WRAPPER).SetActive(TRUE);
    UIStoryboard & tMassacreProgressionIntro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_INTRO);
    UIStoryboard & tMassacreProgressionTierUp = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_TIERUP);
    tMassacreProgressionIntro.Reset();
    tMassacreProgressionIntro.Play(FALSE, 0.0f);
    tMassacreProgressionTierUp.Reset();
    tMassacreProgressionTierUp.Play(FALSE, 0.0f);
}


inline void GetFontColorsForLevel(int32 nLevel, RGBAColor & rgbaTitleColor, RGBAColor & rgbaDropShadow)

{
    rgbaTitleColor = RGBAColor(255, 234, 0, 255);
    rgbaDropShadow = RGBAColor(56, 0, 0, 191);
    switch (nLevel)
    {
    case 0:
    case 1:
        rgbaTitleColor = RGBAColor(255, 248, 193, 255);
        rgbaDropShadow = RGBAColor(0, 0, 0, 191);
        break;
    case 2:
    case 3:
        rgbaTitleColor = RGBAColor(255, 234, 0, 255);
        rgbaDropShadow = RGBAColor(56, 0, 0, 191);
        break;
    case 4:
    case 5:
        rgbaTitleColor = RGBAColor(255, 120, 0, 255);
        rgbaDropShadow = RGBAColor(56, 0, 0, 191);
        break;
    case 6:
    case 7:
        rgbaTitleColor = RGBAColor(255, 90, 0, 255);
        rgbaDropShadow = RGBAColor(0, 0, 0, 191);
        break;
    case 8:
    case 9:
        rgbaTitleColor = RGBAColor(255, 42, 0, 255);
        rgbaDropShadow = RGBAColor(0, 0, 0, 191);
        break;
    }
}

void MassacreEffectsForLevel(int32 nLevel)

{

    CRefString sMassacreBG = CRefString().Format("UI:Console_TextFX_BigStamp_Massacre%i", nLevel + 1);
    UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_BACKGROUND_SPLAT).SetTextureFrame(sMassacreBG.str());


    if (UIUtils::Console::IsJapaneseVersion())
    {
        CRefString sProgressionTexture = CRefString().Format("UI:Console_TextFX_Massacre%dJP", nLevel + 1);
        UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_TITLEJP).SetTextureFrame(sProgressionTexture);
    }


    SNO snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_1);
    switch (nLevel)
    {
    case 0: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_1); break;
    case 1: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_2); break;
    case 2: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_3); break;
    case 3: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_4); break;
    case 4: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_5); break;
    case 5: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_6); break;
    case 6: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_7); break;
    case 7: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_8); break;
    case 8: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_9); break;
    case 9: snoMassacreSound = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_MASSACRE_LEVEL_10); break;
    }




















    RGBAColor rgbaTitleColor, rgbaDropShadow;
    GetFontColorsForLevel(nLevel, rgbaTitleColor, rgbaDropShadow);
    UIText & tProgressionTitle = UI_GetTextFromID(UITXT_CALLOUT_MASSACRE_PROGRESSION_TITLE);
    tProgressionTitle.SetTextColor(rgbaTitleColor);
    tProgressionTitle.SetDropShadowColor(rgbaDropShadow);

    Player * ptPlayer = LocalPlayerGet();
    if (ptPlayer)
    {
        EffectGroupCreateUntracked(snoMassacreSound, ptPlayer->idPlayerRActor, 0, FALSE, FALSE);
    }
}

void MassacreNewTier(int32 nLevel)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_MASSACRE_PROGRESSION)
    {
        tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_INTRO;
        UIStoryboard & tMassacreProgressionIntro = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_INTRO);
        UIStoryboard & tMassacreProgressionTierUp = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_PROGRESSION_TIERUP);
        tMassacreProgressionIntro.Reset();
        tMassacreProgressionIntro.Play(FALSE, 0.0f);
        tMassacreProgressionTierUp.Reset();
        tMassacreProgressionTierUp.Play(FALSE, 0.0f);
    }
    MassacreEffectsForLevel(nLevel);
}

inline void DestructionStart(const CalloutEvent & tEvent)


{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eCurrentVisibleBigStamp != BIG_STAMP_NONE)
    {

        tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_WAIT;
        return;
    }

    tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_INTRO;
    tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_DESTRUCTION_PROGRESSION;
    UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_PROGRESSION_WRAPPER).SetActive(TRUE);
    UIStoryboard & tDestructionProgression = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_PROGRESSION_INTRO);
    tDestructionProgression.Reset();
    tDestructionProgression.Play(FALSE, 0.0f);
}


void HideCurrentVisibleBigStamp()

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    switch (tBigStampInfo.eCurrentVisibleBigStamp)
    {
    case BIG_STAMP_MASSACRE_PROGRESSION:

        tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_WAIT;
        UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_WRAPPER).SetActive(FALSE);
        UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT_WRAPPER).SetActive(FALSE);
        break;
    case BIG_STAMP_DESTRUCTION_PROGRESSION:
        if (tBigStampInfo.eDestructionProgressionState == DESTRUCTION_PROGRESSION_OUTRO)
        {
            tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_NONE;
        }
        else
        {
            tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_WAIT;
        }


        UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_PROGRESSION_WRAPPER).SetActive(FALSE);
        break;

    case BIG_STAMP_DESTRUCTION_REPORT:
        tBigStampInfo.eDestructionReportState = DESTRUCTION_REPORT_NONE;
        UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_REPORT_WRAPPER).SetActive(FALSE);
        break;

    case BIG_STAMP_ENVIRONMENT_KILL:
        tBigStampInfo.eEnvironmentKillState = ENVIRONMENT_KILLS_NONE;
        UI_GetWindowFromID(UIWND_CALLOUT_ENVIRONMENT_KILL_WRAPPER).SetActive(FALSE);
        break;
    default:
        break;
    }
}


void EnvironmentKillStamp(const CalloutEvent & tEvent)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;

    if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_MASSACRE_REPORT ||
        tBigStampInfo.eMassacreReportState == MASSACRE_REPORT_OUTRO)
        return;
    HideCurrentVisibleBigStamp();


    CRefString sKillCount = UnicodeText::FormatNumber((int32)tEvent.uCount, TRUE, FNS_NORMAL, 0);
    sKillCount = LocalizedSprintf(StringListFetch("X1_ConsoleCallouts:EnvironmentKill_KillCount"), sKillCount.str(), NULL);

    SNO snoDestructionBuffPower = GlobalSNOGet(GLOBALSNO_POWERS_ENVIRONMENTKILL_BUFF_RESOURCE_REGEN);
    ScopedPowerDefinitionLockR pdefDestructionBuffPower(snoDestructionBuffPower);
    CRefString sReward;
    sReward = LocalizedSprintf(StringListFetch("X1_ConsoleCallouts:EnvironmentKill_Reward"), GetFormattedTime(tEvent.uTotalTime).str(), NULL);

    UI_GetTextFromID(UITXT_CALLOUT_ENVIRONMENT_KILL_KILLCOUNT).SetText(sKillCount, FALSE);
    UI_GetTextFromID(UITXT_CALLOUT_ENVIRONMENT_KILL_REWARD).SetText(sReward, FALSE);

    tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_ENVIRONMENT_KILL;
    tBigStampInfo.eEnvironmentKillState = ENVIRONMENT_KILLS_INTRO;
    UI_GetWindowFromID(UIWND_CALLOUT_ENVIRONMENT_KILL_WRAPPER).SetActive(TRUE);
    UIStoryboard & tIntroAnim = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_INTRO);
    UIStoryboard & tOutroAnim = UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_INTRO);
    tIntroAnim.Reset();
    tOutroAnim.Reset();
    tIntroAnim.Play(FALSE, 0.0f);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_REPORT_INTRO), cgtPSODefaults);
}






void AnimDoneMassacreProgressionIntro(const UIID &)

{
    if (sgptUICalloutsGlobals->tBigStampInfo.eMassacreProgressionState == MASSACRE_PROGRESSION_INTRO)
    {
        sgptUICalloutsGlobals->tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_WAIT;
    }
}


void AnimDoneMassacreProgressionOutro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_MASSACRE_REPORT;
    if (tBigStampInfo.eMassacreProgressionState != MASSACRE_PROGRESSION_WAIT)
    {
        tBigStampInfo.eMassacreProgressionState = MASSACRE_PROGRESSION_NONE;
    }
    tBigStampInfo.eMassacreReportState = MASSACRE_REPORT_INTRO;
    UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_WRAPPER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_PROGRESSION_KILLCOUNT_WRAPPER).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_REPORT_WRAPPER).SetActive(TRUE);
    UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_MASSACRE_REPORT_INTRO).Play(FALSE, 0.0f);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_REPORT_INTRO), cgtPSODefaults);
}




void AnimDoneMassacreReportIntro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eMassacreReportState == MASSACRE_REPORT_INTRO)
    {
        tBigStampInfo.eMassacreReportState = MASSACRE_REPORT_IDLE;



        uint32 uNow = GameGetServerTime();
        tBigStampInfo.nMassacreReportIdleTick = uNow + 180;
    }
}
void AnimDoneMassacreReportOutro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eMassacreReportState == MASSACRE_REPORT_OUTRO)
    {
        tBigStampInfo.eMassacreReportState = MASSACRE_REPORT_NONE;
        tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_NONE;
        UI_GetWindowFromID(UIWND_CALLOUT_MASSACRE_REPORT_WRAPPER).SetActive(FALSE);
    }
}


void AnimDoneDestructionProgressionIntro(const UIID &)

{
    if (sgptUICalloutsGlobals->tBigStampInfo.eDestructionProgressionState == DESTRUCTION_PROGRESSION_INTRO)
    {
        sgptUICalloutsGlobals->tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_WAIT;
    }
}


void AnimDoneDestructionProgressionOutro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eDestructionProgressionState == DESTRUCTION_PROGRESSION_OUTRO)
    {
        tBigStampInfo.eDestructionProgressionState = DESTRUCTION_PROGRESSION_NONE;
        UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_PROGRESSION_WRAPPER).SetActive(FALSE);
        if (tBigStampInfo.eCurrentVisibleBigStamp == BIG_STAMP_DESTRUCTION_PROGRESSION)
        {
            tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_DESTRUCTION_REPORT;
            tBigStampInfo.eDestructionReportState = DESTRUCTION_REPORT_INTRO;
            UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_REPORT_WRAPPER).SetActive(TRUE);
            UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_DESTRUCTION_REPORT_INTRO).Play(FALSE, 0.0f);
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_REPORT_INTRO), cgtPSODefaults);
        }
    }
}


void AnimDoneDestructionReportIntro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eDestructionReportState == DESTRUCTION_REPORT_INTRO)
    {
        tBigStampInfo.eDestructionReportState = DESTRUCTION_REPORT_IDLE;



        uint32 uNow = GameGetServerTime();
        tBigStampInfo.nDestructionReportIdleTick = uNow + 120;
    }
}
void AnimDoneDestructionReportOutro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eDestructionReportState == DESTRUCTION_REPORT_OUTRO)
    {
        tBigStampInfo.eDestructionReportState = DESTRUCTION_REPORT_NONE;
        tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_NONE;
        UI_GetWindowFromID(UIWND_CALLOUT_DESTRUCTION_REPORT_WRAPPER).SetActive(FALSE);
    }
}


void AnimDoneEnvironmentKillsIntro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eEnvironmentKillState == ENVIRONMENT_KILLS_INTRO)
    {
        tBigStampInfo.eEnvironmentKillState = ENVIRONMENT_KILLS_IDLE;
        UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_INTRO).Reset();
        UI_GetStoryboardFromID(UISTORYBOARD_CALLOUT_ENVIRONMENT_KILL_OUTRO).Reset();



        uint32 uNow = GameGetServerTime();
        tBigStampInfo.nEnvironmentKillIdleTick = uNow + 120;
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CALLOUT_CONSOLE_PULVERIZED), cgtPSODefaults);
    }
}

void AnimDoneEnvironmentKillsOutro(const UIID &)

{
    BigStampInfo & tBigStampInfo = sgptUICalloutsGlobals->tBigStampInfo;
    if (tBigStampInfo.eEnvironmentKillState == ENVIRONMENT_KILLS_OUTRO)
    {
        tBigStampInfo.eEnvironmentKillState = ENVIRONMENT_KILLS_NONE;
        tBigStampInfo.eCurrentVisibleBigStamp = BIG_STAMP_NONE;
        UI_GetWindowFromID(UIWND_CALLOUT_ENVIRONMENT_KILL_WRAPPER).SetActive(FALSE);
    }
}

}  // namespace UICallouts
