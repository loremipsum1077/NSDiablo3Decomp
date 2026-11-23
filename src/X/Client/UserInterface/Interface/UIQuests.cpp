#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
// blz/tuple.h for blz::ignore, which sits at the front of the merged statics block.
#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of this file's own FadeRamp.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../UIStoryboard.h"
#include "../UIUtils.h"
#include "../Controls/UIControlControl.h"
#include "../Controls/UIHotbarButtonControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UIStackPanelControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../../CGame.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GameCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/HotbarButton.h"
#include "../../../Common/Player.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include "../../../Sigma/DataStructures/XPooledList.h"
#include "../../../Sigma/DataStructures/XTemplate.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/UnicodeText.h"

#include "../../../Common/GameSwig.h"

#include <string.h>

struct UIControlSourceKey;
class UIStackPanel;
class ObjHUD;
struct ObjectiveHUDEntry;
class StepText;
struct QuestInstance;
typedef int32 AttribGroupID;  // X/Common/Powers/PowersCommon.h

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
struct PlaySoundOptions;
typedef uint32 SoundID;
// X/Client/Sound/Sound.cpp
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIProgressBar & UI_GetProgressBarFromID(const UIID & id);
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);
void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot);
BOOL UI_TestID(const UIID & id);
BOOL UI_IsResetting();
void UI_Print(LPCSTR szText, BOOL bChat);
int32 UI_GetXMLInt32Constant(LPCSTR szConstantPath, int32 nDefault);
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_NEWEVENT;
extern UIID UISTORYBOARD_OBJHUD_NEW_EVENT;
extern UIID UITEXT_NEWEVENT_TITLE;
extern UIID UITEXT_NEWEVENT_NAME;
extern UIID UITEXT_NEWEVENT_STEP;
extern UIID UIWND_OBJHUD;
extern UIID UISTACK_OBJHUD_MAINQUEST_REGION;
extern UIID UISTACK_OBJHUD_SIDEQUEST_REGION;
extern UIID UICONTROL_DUNGEON_FINDER_PROGRESSBAR;
extern UIID UIPROGRESS_CHALLENGE_RIFT_PROGRESSBAR;
// X/Client/UserInterface/Interface/UIGame.cpp
namespace UIGame {
void SetGameText(const CRefString & sText);
}  // namespace UIGame
// X/Client/UserInterface/Interface/UIQuestLore.cpp
namespace UIQuestLore {
void MarkMainQuestAsDirty();
void MainQuestRefresh();
}  // namespace UIQuestLore
// X/Client/UserInterface/Interface/UIQuestComplete.cpp
#include "UIQuestComplete.h"



extern UIQuestComplete theQuestCompleteUI;
// X/Client/UserInterface/Interface/UIMap.cpp
namespace UIMap {
BOOL IsOpen();
}  // namespace UIMap
// X/Client/UserInterface/Interface/UISetDungeon.cpp
namespace UISetDungeon {
BOOL IsSetDungeonOwner(const Player * ptPlayer);
}  // namespace UISetDungeon
// X/Client/UserInterface/UITutorials.cpp
#include "UITutorials.h"



extern UITutorialManager gtTutorialMgr;
// X/Client/UserInterface/UIToolTips.cpp
typedef BOOL (*UIToolTipTagResolver)(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR, float &, float *, CRefString &, SNO, int32, void *, void *, FontSpecifier &);
CRefString UIToolTips_ProcessTip(const ActorCommonData * ptACD, AttribGroupID idFriendly, AttribGroupID idEnemy, SNO snoPower, int32 nLevel, LPCSTR szText, BOOL bLocalize, UIToolTipTagResolver pfnResolver, void * ptData0, void * ptData1);
// X/Client/CTimedEvents.cpp
int32 CTimedEventGetRemainingTime(SNO snoTimedEvent);
// X/Client/CPlayer.cpp
const Player * LocalPlayerGet();
ACDID LocalPlayerACDGet();
RActorID LocalPlayerRActorGet();
// X/Client/World/CWorld.cpp
SNO WorldGetLevelAreaSNOFromRActor(RActorID idRActor);
// X/Client/App.cpp
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();
// X/Common/Game.cpp
BOOL GameIsOpenWorld();
BOOL GameTypeIsPvP();
OpenWorldMode GameGetOpenWorldMode();
SNO GameGetOpenWorldModeParam();
// X/Common/Player.cpp
BOOL PlayerIsLocal(const Player * ptPlayer);
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
// X/Common/ActorCommonData.cpp
BOOL ACDIsParticipatingInSetDungeon(const ActorCommonData & tACD);
// X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel);
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
BOOL StringListFetch(SNO snoStringList, LPCSTR szLabel, CRefString & sResult);
CRefString LocalizedSprintf(LPCSTR szFormat, ...);
// X/Common/Definitions/LevelAreaDefinition.cpp
CRefString LevelAreaGetLocalizedName(SNO snoLevelArea);
// X/Common/Definitions/MonsterDefinition.cpp
BOOL MonsterGetBaseName(SNO snoMonster, CRefString & sResult);
// X/Common/Font/FormattedText.cpp
void Set_Text_Color(CRefString & sText, int32 nColor);
// X/Common/Definitions/QuestDefinition.cpp
QuestType QuestGetQuestType(SNO snoQuest);
BOOL QuestIsAnySetDungeonType(SNO snoQuest);
BOOL QuestIsAnyBountyType(SNO snoQuest);
BOOL QuestStepIsCompletionStep(SNO snoQuest, int32 nStepUID);
BOOL QuestStepIsReachableByStep(const QuestDefinition * pdefQuest, int32 nStepUID, int32 nFromStepUID);
BOOL QuestHasAdditionalLevelArea(SNO snoQuest, SNO snoLevelArea);
// QuestStepUIDToIndex is declared by QuestDefinition.h.
CRefString QuestGetObjectiveDescriptionLabel(const QuestStepObjective & tObjective, PlayerClass ePlayerClass);
CRefString QuestGetDynamicObjectiveDescription(const QuestStepObjective & tObjective, int32 nState);
// X/Common/Quests/Quests.cpp
CRefString QuestsGetUIName(SNO snoQuest);
BOOL QuestStepGetSplashText(SNO snoQuest, int32 nStepUID, CRefString & sResult);
BOOL QuestStepGetMeterText(SNO snoQuest, int32 nStepUID, CRefString & sResult);
QuestInstance * QuestsGetQuestInstance(SNO snoQuest, ACDID idACD);
void QuestGetBonusAward(const Player * ptPlayer, SNO snoQuest, const QuestStepBonusObjectiveSet & tBonusSet, int32 & nXP, int32 & nGold, BOOL bForDisplay);
#include "../../../Common/Quests/Quests.h"




















namespace UIQuests {
void InitializeForGame();
void DisposeForGame();
BOOL CanShowXPReward(const uint64 nXPGranted);
SNO GetMainQuest();
void Update();
void ResetUI();
void MakeCurrentObjectivesReappear();
void OnChangeAct();
void QueueUpdate(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID, const BOOL bDisplayButton, const BOOL bFailed, const int32 nPreviousStepUID, const int32 nObjectiveIndex, const int32 nValue, const BOOL bPassed);
void OnHUDCheckboxClick(const UIID & id);
void EnableNewInformationButton(const BOOL bEnable);
CRefString GetCurrentObjectiveText(QuestInstance * ptQuestInst);
}  // namespace UIQuests

static StepText * sFindStepText(SNO snoQuest, int32 nStepUID, POS * pPosLast);
static const FadeRamp scgtAlertFadeRamp(0, 180, 240);
static LPCSTR sctQuestColorOrbIDs[3] = { ".red", ".green", ".blue" };
enum {
    ACTIVE_STEP_DISPLAY_TIME = 900,
    ACTIVE_STEP_FADE_TIME = 60,
};

enum StepUpdateState {
    STEP_IS_NEW = 0,
    STEP_IS_UPDATE = 1,
    STEP_ALREADY_SEEN = 2,
};
struct NewEventParams {  // size 0x10
    NewEventParams()
    {
        dwTimestamp = 0xffffffff; eState = FADESTATE_HIDDEN;
        snoQuest = SNO_NONE; nStepUID = -1;
    }
    uint32 dwTimestamp;  // 0x00
    FadeState eState;  // 0x04
    SNO snoQuest;  // 0x08
    int32 nStepUID;  // 0x0c
};

struct NewQuestParams {  // size 0x8
    NewQuestParams() : snoQuest(SNO_NONE), nStepUID(-1) {}
    SNO snoQuest;  // 0x00
    int32 nStepUID;  // 0x04
};

enum QuestUIUpdateType {
    UpdateTypeNewQuest = 0,
    UpdateTypeStepAssigned = 1,
    UpdateTypeStepRollback = 2,
    UpdateTypeCompleted = 3,
    UpdateTypeRemoved = 4,
    UpdateTypeFailure = 5,
    UpdateTypeCounter = 6,
    UpdateTypeDynamicQuestObjective = 7,
};

struct QueuedQuestUpdate {  // size 0x24
    QuestUIUpdateType eType;  // 0x00
    SNO snoQuest;  // 0x04
    SNO snoLevelArea;  // 0x08
    int32 nCurrentStepUID;  // 0x0c
    BOOL bDisplayButton;  // 0x10
    BOOL bNewStep;  // 0x14
    int32 nObjectiveIndex;  // 0x18
    int32 nValue;  // 0x1c
    BOOL bPassed;  // 0x20
};
class ObjectiveEntry {  // size 0x88
public:
    ObjectiveEntry()
    {
        m_gbidItem = GBID_NONE; m_fTriggerAlert = FALSE;
        m_snoTimedEvent = SNO_NONE; m_nCurrentCount = -1; m_nDesiredCount = -1; m_eState = OBJECTIVESTATE_INCOMPLETE;
        m_uAlertStartTime = 0xffffffff;
        m_bAlertAlpha = 0;
        m_eUpdateState = STEP_IS_NEW;
        m_fHiddenUntilComplete = FALSE; m_fHidden = FALSE;
        m_dwCompletionTime = 0xffffffff; m_bTriggerCompletion = FALSE;
        m_idWrapper = GetUIIDNone();
        m_idAlertGlow = GetUIIDNone();
        m_idTextAlert = GetUIIDNone();
        m_fDarken = FALSE;
        m_bStepFadeAlpha = 255;
    }
    void DisposeControlsForHUD();
    void SetupControlsForHUD(const UIID & idObjStack, const CRefString & sStepLabel);
    BOOL UpdateControlsForHUD(const uint8 bAlpha, const uint32 dwTicksInState, const uint8 bConsoleAlertAlpha, const BOOL bHideCheckbox);
    void UpdateAlertAlpha();
    void MarkAsCompleted(const uint32 dwTimeCompleted, const BOOL bHUDChanged);
    void MarkAsFailed(const BOOL bHUDChanged);
    uint32 GetCompletionTime()
    {
        return m_dwCompletionTime;
    }
    CRefString m_sDescription;  // 0x00
    CRefString m_sLabel;  // 0x18
    SNO m_snoTimedEvent;  // 0x30
    int32 m_nCurrentCount;  // 0x34
    int32 m_nDesiredCount;  // 0x38
    QuestObjectiveState m_eState;  // 0x3c
    BOOL m_fHiddenUntilComplete;  // 0x40
    BOOL m_fHidden;  // 0x44
    BOOL m_fDarken;  // 0x48
    GBID m_gbidItem;  // 0x4c
    BOOL m_fTriggerAlert;  // 0x50
    StepUpdateState m_eUpdateState;  // 0x54
    uint8 m_bStepFadeAlpha;  // 0x58
private:
    void sSetupItemIcon(const UIID &);
    CRefString sGetText();
    uint32 m_uAlertStartTime;  // 0x5c
    uint8 m_bAlertAlpha;  // 0x60
    uint32 m_dwCompletionTime;  // 0x64
    BOOL m_bTriggerCompletion;  // 0x68
    UIID m_idWrapper;  // 0x70
    UIID m_idAlertGlow;  // 0x78
    UIID m_idTextAlert;  // 0x80
};
static_assert(sizeof(ObjectiveEntry) == 0x88, "ObjectiveEntry");

typedef XGrowableList<ObjectiveEntry, const ObjectiveEntry &> ObjectiveEntryList;

enum ObjTextState {
    OBJTEXT_DELAYED = 0,
    OBJTEXT_ACTIVE_FADING_IN = 1,
    OBJTEXT_ACTIVE = 2,
    OBJTEXT_COMPLETE_FADE_OUT = 3,
    OBJTEXT_FAILED_FADE_OUT = 4,
    OBJTEXT_COMPLETE_NO_FADE_OUT = 5,
    OBJTEXT_FAILED_NO_FADE_OUT = 6,
};
struct StepObjectiveData {  // size 0x10
    int32 m_nState;  // 0x00
    BOOL m_fPassed;  // 0x04
    BOOL m_fChanged;  // 0x08
    uint32 m_dwChangedTime;  // 0x0c
    void Init()
    {
        m_nState = 0;
        m_fPassed = FALSE;
        m_fChanged = TRUE;
        m_dwChangedTime = 0xffffffff;
    }
};
class StepText {  // size 0x170
public:
    void Initialize(const int32 nNewStepUID, const BOOL fIsUpdate);
    void SwitchState(ObjTextState eNewState);
    void MakeCurrentObjectivesReappear();
    BOOL ProcessState(SNO snoQuest);
    void DisposeControlsForHUD();
    void Destroy();
    void Update(const UIID & idObjStack, const ObjectiveHUDEntry & tObjHUDEntry, const BOOL bShow);
    void MarkAsDirty()
    {
        m_bDirty = TRUE;
    }
    ObjectiveEntryList * GetObjectiveUIList() const
    {
        return m_ptObjectiveUIList;
    }
    int32 GetStepUID() const
    {
        return m_nStepUID;
    }
    SNO GetPowerGranted() const
    {
        return m_snoPowerGranted;
    }
    BOOL GetShowMeter() const
    {
        return m_bShowMeter;
    }
    ObjTextState GetTextState() const
    {
        return m_eState;
    }
    void FlagAsComplete(const BOOL bRollback, const BOOL bNoFade);
    StepObjectiveData & GetStepObjective(const uint32 nIndex)
    {
        return m_tObjData[nIndex];
    }
    void SetIsUpdate(const BOOL fVal)
    {
        m_fIsUpdate = fVal;
    }
    BOOL GetIsUpdate() const
    {
        return m_fIsUpdate;
    }
    uint8 GetAlpha() const
    {
        return m_bAlpha;
    }
private:
    BOOL m_bDirty;  // 0x00
    int32 m_nStepUID;  // 0x04
    BOOL m_fIsUpdate;  // 0x08
    uint32 m_uStateStartTime;  // 0x0c
    ObjTextState m_eState;  // 0x10
    uint8 m_bAlpha;  // 0x14
    uint8 m_bAlertAlpha;  // 0x15
    BOOL m_fShowAlert;  // 0x18
    StepObjectiveData m_tObjData[20];  // 0x1c
    ObjectiveEntryList * m_ptObjectiveUIList;  // 0x160
    SNO m_snoPowerGranted;  // 0x168
    BOOL m_bShowMeter;  // 0x16c
};
static_assert(sizeof(StepText) == 0x170, "StepText");

typedef XPooledList<StepText, StepText &> TStepTextList;

typedef XListMemoryPool<StepText> TStepTextListPool;
struct ObjectiveHUDEntry {  // size 0x40
    ObjectiveHUDEntry()
    {
        m_ptParent = NULL;
        m_snoQuest = SNO_NONE; m_snoLevelArea = SNO_NONE;
        m_dwLastInAreaTime = 0xffffffff; m_bShow = TRUE;
    }
    ObjectiveHUDEntry(const ObjHUD *);
    void SetupQuestHUDStack(UIControlSourceKey & tQuestStackTemplateSrcKey, CRefString sName, const UIID & idParent);
    void DisposeUI();
    void Destroy();
    BOOL Update(UIStackPanel * ptQuestStack, const SNO snoPlayerLevelArea);
    const ObjHUD * m_ptParent;  // 0x00
    SNO m_snoQuest;  // 0x08
    SNO m_snoLevelArea;  // 0x0c
    TStepTextList tStepTextList;  // 0x10
    uint32 m_dwLastInAreaTime;  // 0x38
    BOOL m_bShow;  // 0x3c
};
static_assert(sizeof(ObjectiveHUDEntry) == 0x40, "ObjectiveHUDEntry");

typedef XGrowableList<ObjectiveHUDEntry, const ObjectiveHUDEntry &> HUDEntryList;




class ObjHUD {  // size 0x68
public:

    ObjHUD() {}






    void Initialize()
    {

        m_posMainQuest = NULL;
        m_fRebuildHUD = TRUE;
    }




    void Destroy()
    {

        MarkHUDToBeRebuilt();

        while (!m_tList.IsEmpty())
        {
            ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetHead();
            tObjHUDEntry.Destroy();
            m_tList.RemoveHead();
        }
        m_posMainQuest = NULL;
    }
    void ResetUI();
    void MakeCurrentObjectivesReappear();
    ObjectiveHUDEntry * GetMainQuest();
    ObjectiveHUDEntry * FindObjHUDEntry(SNO snoQuest);
    BOOL NeedsUpdate() const;
    void SetupHUDControls();
    void Update();
    void AddEntry(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID, const BOOL fIsUpdate);
    void DestroyEntry(SNO snoQuest);
    void FlagAllStepsComplete(const SNO snoQuest, BOOL bRollback);























    void MarkHUDToBeRebuilt()
    {

        m_fRebuildHUD = TRUE;
    }
    BOOL IsEmpty();
private:
    BOOL m_fRebuildHUD;  // 0x00
    POS m_posMainQuest;  // 0x08
    HUDEntryList m_tList;  // 0x10
};
static_assert(sizeof(ObjHUD) == 0x68, "ObjHUD");






struct UIQuestGlobals {  // size 0x168
    NewEventParams tNewEventParams;  // 0x00
    NewQuestParams tNewQuestParams;  // 0x10
    BOOL fHasNewQuest;  // 0x18
    XGrowableList<QueuedQuestUpdate, const QueuedQuestUpdate &> tQueuedQuestUpdateList;  // 0x20
    ObjHUD tObjHUD;  // 0x78
    TStepTextListPool poolStepTextList;  // 0xe0
    SNOList listTrackedSideQuests;  // 0x110
};
static_assert(sizeof(UIQuestGlobals) == 0x168, "UIQuestGlobals");




static UIQuestGlobals * sgptUIQuestGlobals;
static BOOL sgbWasUIResetting;




























void UIQuests::InitializeForGame()
{
    const BOOL bWasResetting = sgbWasUIResetting; sgbWasUIResetting = FALSE;
    if (bWasResetting)
        return;

    const int32 INITIAL_EVENT_STEP_TEXT_COUNT = 16;





    sgptUIQuestGlobals = new (SigmaMemoryNew(sizeof(UIQuestGlobals), 4, NULL, TRUE)) UIQuestGlobals();

    sgptUIQuestGlobals->tObjHUD.Initialize();

    sgptUIQuestGlobals->poolStepTextList.SetMemoryInterface(AppGetCurrentMemoryInterface());

    sgptUIQuestGlobals->poolStepTextList.SetBlockCount_Tracked(INITIAL_EVENT_STEP_TEXT_COUNT);
}



void UIQuests::DisposeForGame()
{

    if (!sgptUIQuestGlobals)
        return;



    if (UI_IsResetting())
    {
        sgbWasUIResetting = TRUE;
        return;
    }


    sgptUIQuestGlobals->tObjHUD.Destroy();

    delete sgptUIQuestGlobals; sgptUIQuestGlobals = NULL;
}




BOOL UIQuests::CanShowXPReward(
    const uint64 nXPGranted)
{
    if (nXPGranted != 0)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
        if (!ptACDPlayer)
        {
            return FALSE;
        }

        if (ptACDPlayer->CanGainMoreExperience())
        {
            return TRUE;
        }
    }

    return FALSE;
}




SNO UIQuests::GetMainQuest()
{

    ObjectiveHUDEntry * pEntry = sgptUIQuestGlobals->tObjHUD.GetMainQuest();
    if (pEntry)

        return pEntry->m_snoQuest;



    return SNO_NONE;
}



static void sUpdateNewQuestDisplay()
{

    if (sgptUIQuestGlobals->fHasNewQuest)
    {

        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_QUEST_ASSIGNED), cgtPSODefaults);








        sgptUIQuestGlobals->fHasNewQuest = FALSE;
    }
}




static void sUpdateNewEventDisplay()
{

    NewEventParams & tNewEventParams = sgptUIQuestGlobals->tNewEventParams;

    float flElapsedTime = (float)(CGameGetRealTicks() - tNewEventParams.dwTimestamp) * (1.0f / 60.0f);
    float flDuration = UI_GetStoryboardFromID(UISTORYBOARD_OBJHUD_NEW_EVENT).GetDuration();
    if (flElapsedTime > flDuration)

        UI_GetWindowFromID(UIDLG_NEWEVENT).SetActive(FALSE);
}





static void sStartNewEvent(NewEventParams & tNewEventParams)
{

    UI_GetWindowFromID(UIDLG_NEWEVENT).SetActive(TRUE);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_QUEST_ASSIGNED), cgtPSODefaults);

    QuestType eQuestType = QuestGetQuestType(tNewEventParams.snoQuest);

    SNO snoStringList = QuestGetStringList(tNewEventParams.snoQuest);
    CRefString sName = StringListFetch(snoStringList, gszQuestNameLabel);

    if (tNewEventParams.snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER) &&

        GameGetActiveGameCommonData().tGameSyncedData.nOpenLootRunLevel >= 0)

        sName = QuestsGetUIName(GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER_TIERED));



    CRefString sStep;
    QuestStepGetSplashText(tNewEventParams.snoQuest, tNewEventParams.nStepUID, sStep);

    if (eQuestType == QUEST_TYPE_BOUNTY)
    {
        CRefString sMsg = LocalizedSprintf(StringListFetch("General:QuestUpdateNewBountyChatMessage"), sName.str(), sStep.str(), NULL);
        Set_Text_Color(sMsg, FONTCOLOR_GOLD);
        UI_Print(sMsg, TRUE);
    }
    else if (eQuestType == QUEST_TYPE_HORADRIC_QUEST)
    {
        CRefString sMsg = LocalizedSprintf(StringListFetch("General:QuestUpdateNewBountyChatMessage"), sName.str(), sStep.str(), NULL);
        Set_Text_Color(sMsg, FONTCOLOR_GOLD);
        UI_Print(sMsg, TRUE);
    }
    else
    {



        UI_GetStoryboardFromID(UISTORYBOARD_OBJHUD_NEW_EVENT).Play(FALSE, 0.0f);
        UIText & tTextTitle = UI_GetTextFromID(UITEXT_NEWEVENT_TITLE);
        if (eQuestType == QUEST_TYPE_CHALLENGE)

            tTextTitle.SetText(StringListFetch("General:QuestUpdateNewChallenge"), FALSE);
        else


            tTextTitle.SetText(StringListFetch("General:QuestUpdateNewEvent"), FALSE);


        UI_GetTextFromID(UITEXT_NEWEVENT_NAME).SetText(sName, FALSE);
        UI_GetTextFromID(UITEXT_NEWEVENT_STEP).SetText(sStep, FALSE);
    }
}


static void sOnNewQuest(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID)
{



    ScopedQuestDefinitionLockR pdefQuest(snoQuest);

    switch (pdefQuest->eQuestType)
    {
    case QUEST_TYPE_EVENT:
    case QUEST_TYPE_CHALLENGE:
    case QUEST_TYPE_BOUNTY:
    case QUEST_TYPE_HORADRIC_QUEST:
    case QUEST_TYPE_SET_DUNGEON:
    case QUEST_TYPE_SET_DUNGEON_BONUS:
    case QUEST_TYPE_SET_DUNGEON_MASTERY:
    case QUEST_TYPE_SET_DUNGEON_TRACKER:
        {
            NewEventParams & tNewEventParams = sgptUIQuestGlobals->tNewEventParams;
            tNewEventParams.dwTimestamp = CGameGetRealTicks();
            tNewEventParams.snoQuest = snoQuest;
            tNewEventParams.nStepUID = nStepUID;

            sStartNewEvent(tNewEventParams);
        }
        break;
    case QUEST_TYPE_MAIN_QUEST:
    case QUEST_TYPE_SIDE_QUEST:
        {

            NewQuestParams & tNewQuestParams = sgptUIQuestGlobals->tNewQuestParams;
            tNewQuestParams.snoQuest = snoQuest;
            tNewQuestParams.nStepUID = nStepUID;

            sgptUIQuestGlobals->fHasNewQuest = TRUE;
        }
        break;
    case QUEST_TYPE_PAGE_OF_FATE:
        {

            NewQuestParams & tNewQuestParams = sgptUIQuestGlobals->tNewQuestParams;
            tNewQuestParams.snoQuest = snoQuest;
            tNewQuestParams.nStepUID = nStepUID;
        }
        break;
    default:











        XASSERT(FALSE);
    }

    sgptUIQuestGlobals->tObjHUD.AddEntry(snoQuest, snoLevelArea, nStepUID, FALSE);
}



static void sOnStepAssigned(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID)
{



    ScopedQuestDefinitionLockR pdefQuest(snoQuest);



    sgptUIQuestGlobals->tObjHUD.FlagAllStepsComplete(snoQuest, FALSE);
    sgptUIQuestGlobals->tObjHUD.AddEntry(snoQuest, snoLevelArea, nStepUID, TRUE);
}



static void sOnStepRollback(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID)
{



    ScopedQuestDefinitionLockR pdefQuest(snoQuest);



    sgptUIQuestGlobals->tObjHUD.FlagAllStepsComplete(snoQuest, TRUE);
    sgptUIQuestGlobals->tObjHUD.AddEntry(snoQuest, snoLevelArea, nStepUID, TRUE);
}



static void sOnQuestFailed(const SNO snoQuest, const int32)
{


    CRefString sText = StringListFetch("General:QuestFailed");

    sText += QuestsGetUIName(snoQuest);
    UIGame::SetGameText(sText);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_QUEST_FAILED), cgtPSODefaults);

    sgptUIQuestGlobals->tObjHUD.DestroyEntry(snoQuest);
}



static void sOnDynamicQuestObjectiveUpdate(SNO snoQuest, int32 nStepUID)
{


    if (!sgptUIQuestGlobals)
        return;



    StepText * ptStepText = sFindStepText(snoQuest, nStepUID, NULL);
    if (ptStepText)
    {
        ptStepText->MarkAsDirty();
        ptStepText->SetIsUpdate(TRUE);
        ptStepText->MakeCurrentObjectivesReappear();

        OpenWorldMode eOpenWorldMode = GameGetOpenWorldMode();

        if (eOpenWorldMode == OPENWORLD_MODE_BOUNTY)


            gtTutorialMgr.ShowTutorial(UIWND_OBJHUD, GlobalSNOGet(GLOBALSNO_TUTORIAL_OPEN_WORLD_QUEST_OBJECTIVE), NULL, TRUE);
    }
}








static void sUpdateQuestCounter(SNO snoQuest, int32 nStepUID, int32 nObjectiveIndex, int32 nValue, BOOL bPassed)
{





    if (!sgptUIQuestGlobals || nObjectiveIndex < 0)
        return;








    StepText * ptStepText = sFindStepText(snoQuest, nStepUID, NULL);
    if (ptStepText)
    {





        StepObjectiveData & tObjData = ptStepText->GetStepObjective(nObjectiveIndex);
        if ((!tObjData.m_fPassed || !bPassed) && tObjData.m_nState != nValue)
        {
            tObjData.m_nState = nValue;
            tObjData.m_fPassed = bPassed;
            tObjData.m_fChanged = TRUE;
            tObjData.m_dwChangedTime = CGameGetClientTicks();
            ptStepText->MarkAsDirty();
            ptStepText->SetIsUpdate(TRUE);
            ptStepText->MakeCurrentObjectivesReappear();
            UIQuestLore::MarkMainQuestAsDirty();
        }
    }
}




static void sProcessQueuedQuestUpdates()
{

    POS pos = sgptUIQuestGlobals->tQueuedQuestUpdateList.GetHeadPosition();
    while (pos)
    {

        POS posCur = pos;
        const QueuedQuestUpdate & tUpdate = sgptUIQuestGlobals->tQueuedQuestUpdateList.GetNext(pos);
        if (tUpdate.bNewStep || tUpdate.eType == UpdateTypeFailure)
        {
            if (tUpdate.eType == UpdateTypeNewQuest)

                sOnNewQuest(tUpdate.snoQuest, tUpdate.snoLevelArea, tUpdate.nCurrentStepUID);

            else if (tUpdate.bDisplayButton && tUpdate.eType == UpdateTypeStepAssigned)

                sOnStepAssigned(tUpdate.snoQuest, tUpdate.snoLevelArea, tUpdate.nCurrentStepUID);

            else if (tUpdate.bDisplayButton && tUpdate.eType == UpdateTypeStepRollback)

                sOnStepRollback(tUpdate.snoQuest, tUpdate.snoLevelArea, tUpdate.nCurrentStepUID);

            else if (tUpdate.eType == UpdateTypeFailure && tUpdate.nCurrentStepUID != -1)

                sOnQuestFailed(tUpdate.snoQuest, tUpdate.nCurrentStepUID);
            else if (tUpdate.eType == UpdateTypeCompleted)
            {




                sgptUIQuestGlobals->tObjHUD.DestroyEntry(tUpdate.snoQuest);


                ObjectiveHUDEntry * pEntry = sgptUIQuestGlobals->tObjHUD.GetMainQuest();
                if (!pEntry)


                    sgptUIQuestGlobals->fHasNewQuest = FALSE;
            }
            else if (tUpdate.eType == UpdateTypeRemoved)


                sgptUIQuestGlobals->tObjHUD.DestroyEntry(tUpdate.snoQuest);
        }


        if (tUpdate.eType == UpdateTypeDynamicQuestObjective)

            sOnDynamicQuestObjectiveUpdate(tUpdate.snoQuest, tUpdate.nCurrentStepUID);




        sUpdateQuestCounter(tUpdate.snoQuest, tUpdate.nCurrentStepUID, tUpdate.nObjectiveIndex, tUpdate.nValue, tUpdate.bPassed);

        sgptUIQuestGlobals->tQueuedQuestUpdateList.RemoveAt(posCur);
    }
}




void UIQuests::Update()
{
    const Player * ptPlayer = LocalPlayerGet();

    if (!ptPlayer)
        return;



    if (GameTypeIsPvP())
    {
        UI_GetWindowFromID(UIWND_OBJHUD).SetActive(FALSE);
        return;
    }

    if (sgptUIQuestGlobals->tQueuedQuestUpdateList.Count() != 0 ||
        sgptUIQuestGlobals->tObjHUD.NeedsUpdate())

        UIQuestLore::MarkMainQuestAsDirty();






    sProcessQueuedQuestUpdates();

    UIQuestLore::MainQuestRefresh();

    sUpdateNewQuestDisplay();
    theQuestCompleteUI.Update();
    sUpdateNewEventDisplay();
    sgptUIQuestGlobals->tObjHUD.Update();


















}



void UIQuests::ResetUI()
{





    if (!sgptUIQuestGlobals)
        return;




    sgptUIQuestGlobals->tObjHUD.ResetUI();
}


void UIQuests::MakeCurrentObjectivesReappear()
{



    sgptUIQuestGlobals->tObjHUD.MakeCurrentObjectivesReappear();

}



void UIQuests::OnChangeAct()
{

    if (!sgptUIQuestGlobals)
        return;



    sgptUIQuestGlobals->tObjHUD.Destroy();



    UIQuestLore::MainQuestRefresh();
}



void UIQuests::EnableNewInformationButton(const BOOL bEnable)
{
}























void UIQuests::QueueUpdate(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID, const BOOL bDisplayButton, const BOOL bFailed, const int32 nPreviousStepUID, const int32 nObjectiveIndex, const int32 nValue, const BOOL bPassed)
{









    if (!sgptUIQuestGlobals)
        return;

    ScopedQuestDefinitionLockR pdefQuest(snoQuest);






    const BOOL bNewStep = nPreviousStepUID != nStepUID;





    const BOOL bCompletionStep = QuestStepIsCompletionStep(snoQuest, nStepUID);
    const QuestUIUpdateType eStepUpdate = (bNewStep && QuestStepIsReachableByStep(pdefQuest, nStepUID, nPreviousStepUID)) ? UpdateTypeStepRollback : UpdateTypeStepAssigned;

    QuestUIUpdateType eType;
    if (bFailed)
        eType = UpdateTypeFailure;

    else if (bCompletionStep)
        eType = UpdateTypeCompleted;


    else if (nStepUID == -1)
        eType = UpdateTypeRemoved;


    else if (nPreviousStepUID == -1)
        eType = UpdateTypeNewQuest;


    else if (GameIsOpenWorld() && GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC) == snoQuest)
        eType = UpdateTypeDynamicQuestObjective;
    else




        eType = nObjectiveIndex >= 0 ? UpdateTypeCounter : eStepUpdate;

    QueuedQuestUpdate tUpdate;
    tUpdate.eType = eType;
    tUpdate.snoQuest = snoQuest;
    tUpdate.snoLevelArea = snoLevelArea;
    tUpdate.nCurrentStepUID = nStepUID;
    tUpdate.bDisplayButton = bDisplayButton;
    tUpdate.bNewStep = bNewStep;
    tUpdate.nObjectiveIndex = nObjectiveIndex;
    tUpdate.nValue = nValue;
    tUpdate.bPassed = bPassed;
    sgptUIQuestGlobals->tQueuedQuestUpdateList.AddTail_Tracked(tUpdate);
}


static StepText * sFindStepText(SNO snoQuest, int32 nStepUID, POS * pPosLast)
{



    ObjectiveHUDEntry * ptObjHUDEntry = sgptUIQuestGlobals->tObjHUD.FindObjHUDEntry(snoQuest);
    if (ptObjHUDEntry)
    {

        POS pos = ptObjHUDEntry->tStepTextList.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            StepText & tStepText = ptObjHUDEntry->tStepTextList.GetNext(pos);
            if (tStepText.GetStepUID() == nStepUID)
            {
                if (pPosLast)
                    *pPosLast = posLast;
                return &tStepText;
            }
        }
    }
    return NULL;
}






static void sTrackQuest(const SNO snoQuest, const BOOL fTrack)
{



    ScopedQuestDefinitionLockR pdefQuest(snoQuest);
    if (pdefQuest->eQuestType != QUEST_TYPE_MAIN_QUEST)
    {



        POS pos = sgptUIQuestGlobals->listTrackedSideQuests.Find(snoQuest);
        if (fTrack)
        {
            if (!pos)








                sgptUIQuestGlobals->listTrackedSideQuests.AddTail_Tracked(snoQuest);
        }
        else if (pos)
        {
            sgptUIQuestGlobals->listTrackedSideQuests.RemoveAt(pos);
        }
    }
}

void ObjectiveHUDEntry::Destroy()
{
    DisposeUI();
    POS pos = tStepTextList.GetHeadPosition();


    while (pos)
    {
        POS posCur = pos;
        StepText & tStepText = tStepTextList.GetNext(pos);
        tStepText.Destroy();
        tStepTextList.RemoveAt(posCur);
    }
}


void ObjectiveHUDEntry::DisposeUI()
{
    POS pos = tStepTextList.GetHeadPosition();

    while (pos)
    {
        StepText & tStepText = tStepTextList.GetNext(pos);
        tStepText.DisposeControlsForHUD();
    }
}





void ObjectiveEntry::UpdateAlertAlpha()
{




    const uint32 dwNow = CGameGetClientTicks();

    if (m_fTriggerAlert)
    {
        m_uAlertStartTime = dwNow;
        m_fTriggerAlert = FALSE;
    }
    m_bAlertAlpha = 0;
    float flFrac;
    if (scgtAlertFadeRamp.Evaluate(dwNow - m_uAlertStartTime, flFrac) != FADESTATE_HIDDEN)
        m_bAlertAlpha = (uint8)(int32)(flFrac * 255.0f);

}






CRefString ObjectiveEntry::sGetText()
{
    CRefString sText(m_sDescription);

    if (m_eState == OBJECTIVESTATE_FAILED)
    {
        UIText_RemoveFormattingCharacters(sText, 0, TRUE);
        Set_Text_Color(sText, 1);
    }

    if (m_nDesiredCount > 0)
    {
        CRefString sCount(CRefString().Format(" {c_green}(%d/%d){/c}", m_nCurrentCount, m_nDesiredCount));
        sText += sCount;
    }


    if (m_snoTimedEvent != SNO_NONE)
    {
        const int32 nTimeRemaining = CTimedEventGetRemainingTime(m_snoTimedEvent);
        if (nTimeRemaining > 0)
        {
            const uint32 uMinutes = nTimeRemaining / 60;
            sText += "\n";
            sText += UIText_GetTimeString(uMinutes);
        }
    }


    return sText;
}



































void ObjectiveEntry::DisposeControlsForHUD()
{

    UI_DeleteControl(m_idWrapper, TRUE);
    m_idWrapper = GetUIIDNone();
}



void ObjectiveEntry::SetupControlsForHUD(const UIID & idObjStack, const CRefString & sStepLabel)
{

    SNOName snoname(UI_SNO, "ConsoleEventText");
    Vector2D vecOrigin(0.0f, 0.0f);





    UIControlSourceKey tSrcKey(snoname.snoHandle, "eventtext_bkgrnd.objectives_wrapper");
    UIControl * ptControl = UI_CreateControlFromTemplate(tSrcKey, DynamicControlCreationParams(CRefString().Format("%s wrapper", sStepLabel.str()), idObjStack), &vecOrigin);
    UIWindow & tWrapper = UI_GetWindowFromID(ptControl->GetID());
    m_idWrapper = tWrapper.GetID();

    m_idAlertGlow = UIIDCreateFromParent(m_idWrapper, ".alert_glow");
    m_idTextAlert = UIIDCreateFromParent(m_idWrapper, ".alert_text");



    if (m_eState == OBJECTIVESTATE_PASSED)
    {
        if (m_bTriggerCompletion)
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_QUEST_UPDATED), cgtPSODefaults);
    }
    else if (m_eState == OBJECTIVESTATE_FAILED)
    {
        if (m_bTriggerCompletion)
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_QUEST_UPDATED), cgtPSODefaults);
    }
























}



BOOL ObjectiveEntry::UpdateControlsForHUD(const uint8 bAlpha, const uint32 dwTicksInState, const uint8 bConsoleAlertAlpha, const BOOL bHideCheckbox)
{






    if (m_idWrapper == GetUIIDNone())
        return FALSE;



    UIWindow & tWrapper = UI_GetWindowFromID(m_idWrapper);

    if (bAlpha == 0 ||
        (m_fHiddenUntilComplete && m_eState == OBJECTIVESTATE_INCOMPLETE) ||
        m_fHidden)
    {
        tWrapper.SetActive(FALSE);
        return FALSE;
    }
    Vector2D vecTL, vecDim;



    tWrapper.GetRelativePosition(vecTL, vecDim);
    if (vecTL.y + vecDim.y > 900.0f)
    {
        tWrapper.SetActive(FALSE);
        return TRUE;
    }
    tWrapper.SetActive(TRUE);

    UIWindow & tAlertGlow = UI_GetWindowFromID(m_idAlertGlow);
    UIText & tTextAlert = UI_GetTextFromID(m_idTextAlert);








    CRefString sAlert;
    if (bConsoleAlertAlpha != 0)
    {
        if (m_eState == OBJECTIVESTATE_PASSED)


            sAlert = StringListFetch("General:QuestHUDCompleteObjective");
        else if (m_eState == OBJECTIVESTATE_FAILED)

            sAlert = StringListFetch("General:QuestHUDFailedObjective");



        else if (m_eUpdateState == STEP_IS_UPDATE)

            sAlert = StringListFetch("General:QuestHUDUpdateObjective");
        else if (m_eUpdateState == STEP_IS_NEW)


            sAlert = StringListFetch("General:QuestHUDNewObjective");
    }



    if (sAlert.Length() != 0)
    {
        tTextAlert.SetActive(TRUE);
        tTextAlert.SetTransparency(bConsoleAlertAlpha);
        tTextAlert.SetText(sAlert, FALSE);



        tAlertGlow.SetActive(FALSE);
    }
    else
    {















        tAlertGlow.SetActive(FALSE);
        tTextAlert.SetActive(FALSE);
    }

























    const uint8 bTextAlpha = (m_fHidden || m_bStepFadeAlpha == 255) ? bAlpha : m_bStepFadeAlpha;




    UIID idText = UIIDCreateFromParent(m_idWrapper, ".text");
    UIText & tText = UI_GetTextFromID(idText);
    tText.SetText(sGetText(), FALSE);
    tText.SetTransparency(bTextAlpha);
    tText.SetTextColor((FontSpecifier)0);









    return FALSE;
}



void ObjectiveEntry::MarkAsCompleted(const uint32 dwTimeCompleted, const BOOL bHUDChanged)
{


    m_eState = OBJECTIVESTATE_PASSED;

    m_dwCompletionTime = dwTimeCompleted;

    m_bTriggerCompletion = bHUDChanged;
}




void ObjectiveEntry::MarkAsFailed(const BOOL bHUDChanged)
{

    m_eState = OBJECTIVESTATE_FAILED;

    m_bTriggerCompletion = bHUDChanged;
}





void ObjHUD::AddEntry(const SNO snoQuest, const SNO snoLevelArea, const int32 nStepUID, const BOOL fIsUpdate)
{




    MarkHUDToBeRebuilt();
    sTrackQuest(snoQuest, TRUE);

    ObjectiveHUDEntry * ptExistingObjHUDEntry = FindObjHUDEntry(snoQuest);
    if (!ptExistingObjHUDEntry)
    {
        ObjectiveHUDEntry tObjHUDEntry;
        tObjHUDEntry.m_ptParent = this;
        tObjHUDEntry.m_snoQuest = snoQuest;
        tObjHUDEntry.m_snoLevelArea = snoLevelArea;





        POS posEvent = m_tList.AddTail_Tracked(tObjHUDEntry);
        ObjectiveHUDEntry & tAddedObjHUDEntry = m_tList.GetAt(posEvent);
        {
            ScopedQuestDefinitionLockR pdefQuest(snoQuest);
            if (pdefQuest->eQuestType == QUEST_TYPE_MAIN_QUEST)

                m_posMainQuest = posEvent;
        }




        tAddedObjHUDEntry.tStepTextList.SetPool(&sgptUIQuestGlobals->poolStepTextList);
        ptExistingObjHUDEntry = &tAddedObjHUDEntry;
    }
    else if (!fIsUpdate)
    {
        return;
    }
    {
        POS posRemove;
        StepText * ptStepText = sFindStepText(snoQuest, nStepUID, &posRemove);
        if (ptStepText)
        {
            ptStepText->Destroy();
            ptExistingObjHUDEntry->tStepTextList.RemoveAt(posRemove);
        }
        StepText tStepText;
        tStepText.Initialize(nStepUID, fIsUpdate);

        ptExistingObjHUDEntry->tStepTextList.AddTail_Tracked(tStepText);
    }
}




void ObjHUD::ResetUI()
{
    POS pos = m_tList.GetHeadPosition();

    while (pos)
    {
        ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetNext(pos);
        tObjHUDEntry.DisposeUI();
    }
    MarkHUDToBeRebuilt();
}




void ObjHUD::MakeCurrentObjectivesReappear()
{
    POS pos = m_tList.GetHeadPosition();

    while (pos)
    {
        ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetNext(pos);
        POS posStep = tObjHUDEntry.tStepTextList.GetHeadPosition();

        while (posStep)
        {
            StepText & tStepText = tObjHUDEntry.tStepTextList.GetNext(posStep);
            tStepText.MakeCurrentObjectivesReappear();
        }
    }
}





ObjectiveHUDEntry * ObjHUD::GetMainQuest()
{

    if (m_posMainQuest)
    {
        ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetAt(m_posMainQuest);
        return &tObjHUDEntry;
    }
    return NULL;
}




ObjectiveHUDEntry * ObjHUD::FindObjHUDEntry(SNO snoQuest)
{
    POS pos = m_tList.GetHeadPosition();

    while (pos)
    {
        ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetNext(pos);
        if (tObjHUDEntry.m_snoQuest == snoQuest)
            return &tObjHUDEntry;
    }
    return NULL;
}








void UIQuests::OnHUDCheckboxClick(const UIID & id)
{







}




BOOL ObjHUD::NeedsUpdate() const
{
    POS pos = m_tList.GetHeadPosition();



    while (pos)
    {
        const ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetNext(pos);
        POS posStep = tObjHUDEntry.tStepTextList.GetHeadPosition();

        while (posStep)
        {
            const StepText & tStepText = tObjHUDEntry.tStepTextList.GetNext(posStep);
            POS posEntry = tStepText.GetObjectiveUIList()->GetHeadPosition();


            while (posEntry)
            {
                ObjectiveEntry & tEntry = tStepText.GetObjectiveUIList()->GetNext(posEntry);
                if (tEntry.m_snoTimedEvent != SNO_NONE)
                    return TRUE;
            }
        }
    }
    return FALSE;
}






static float sGetQuestMeter(SNO snoQuest)
{
    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    float flPercent = -1.0f;

    if (ptQuestInstance)
    {
        if (ptQuestInstance->idACDMeter != ACDID_INVALID)
        {
            ActorCommonData * ptACD = ACDTryToGet(ptQuestInstance->idACDMeter);
            if (ptACD)
                return ptACD->GetHitpointPercent();
        }
        flPercent = ptQuestInstance->flMeter;
    }
    return flPercent;
}
static uint32 sGetQuestFlags(SNO snoQuest)
{
    uint32 dwFlags = 0;

    QuestInstance * ptQuestInstance = QuestsGetQuestInstance(snoQuest, ACDID_INVALID);
    if (ptQuestInstance)

        dwFlags = ptQuestInstance->dwFlags;
    return dwFlags;
}

static BOOL sShouldShowDynamicObjectives();




BOOL ObjectiveHUDEntry::Update(
    UIStackPanel * ptQuestStack,
    const SNO snoPlayerLevelArea)
{
    if (m_snoLevelArea != SNO_NONE)
    {
        BOOL bInLevelArea = FALSE;
        if (m_snoLevelArea == snoPlayerLevelArea)
        {
            bInLevelArea = TRUE;
        }
        else if (m_snoQuest != SNO_NONE && snoPlayerLevelArea != SNO_NONE)
        {

            bInLevelArea = QuestHasAdditionalLevelArea(m_snoQuest, snoPlayerLevelArea);
        }


        if (!bInLevelArea)
        {
            ptQuestStack->SetActive(FALSE);
            return FALSE;
        }
        else
        {
            ptQuestStack->SetActive(TRUE);
        }
    }

    if (GameIsOpenWorld() && m_snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC))
    {
        if (!sShouldShowDynamicObjectives())
        {
            ptQuestStack->SetActive(FALSE);
            return FALSE;
        }
    }

    ptQuestStack->SetActive(TRUE);

    UIID idTitle = UIIDCreateFromParent(ptQuestStack->GetID(), ".wrapper.title_text");
    UIText & tText = UI_GetTextFromID(idTitle);
    CRefString strTitle = QuestsGetUIName(m_snoQuest);

    if (m_snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER) &&

        GameGetActiveGameCommonData().tGameSyncedData.nOpenLootRunLevel >= 0)

        strTitle = QuestsGetUIName(GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER_TIERED));





    if (m_snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC))
    {
        OpenWorldMode eOpenWorldMode = GameGetOpenWorldMode();

        switch (eOpenWorldMode)
        {
        case OPENWORLD_MODE_BOUNTY:
            {




                SNO snoLevelArea = GameGetOpenWorldModeParam();




                if (snoLevelArea == SNO_NONE)
                    strTitle = StringListFetch("General:QuestTitleBountyNoLevelArea");
                else



                    strTitle = LocalizedSprintf(StringListFetch("General:QuestTitleBounty"), LevelAreaGetLocalizedName(snoLevelArea).str(), NULL);
            }
            break;
        case OPENWORLD_MODE_IDLE:
            strTitle = StringListFetch("General:QuestTitleAdventureMode");
            break;
        case OPENWORLD_MODE_DUNGEONFINDER:
            strTitle = StringListFetch("General:QuestTitleLootRun");
            break;
        }
    }
    tText.SetText(strTitle.str(), FALSE);


    QuestType eQuestType = QuestGetQuestType(m_snoQuest);
    if (eQuestType == QUEST_TYPE_PAGE_OF_FATE || QuestIsAnySetDungeonType(m_snoQuest))

        tText.SetActive(FALSE);


    UIID idObjStack = UIIDCreateFromParent(ptQuestStack->GetID(), ".objective_stack");



    BOOL bShowMeter = FALSE;
    SNO snoPowerGranted = SNO_NONE;
    CRefString strMeter;
    uint8 bMaxStepAlpha = 0;
    float flMeter = -1.0f;
    POS posObj = tStepTextList.GetHeadPosition();
    while (posObj)
    {
        POS posLast = posObj;
        StepText & tStepText = tStepTextList.GetNext(posObj);
        if (tStepText.ProcessState(m_snoQuest))
        {
            tStepText.Update(idObjStack, *this, m_bShow);
            snoPowerGranted = tStepText.GetPowerGranted();
            bShowMeter = tStepText.GetShowMeter();
            if (bShowMeter)
            {
                QuestStepGetMeterText(m_snoQuest, tStepText.GetStepUID(), strMeter);
                flMeter = sGetQuestMeter(m_snoQuest);
            }
            bMaxStepAlpha = Math::Max(bMaxStepAlpha, tStepText.GetAlpha());
        }
        else
        {
            tStepText.Destroy();
            tStepTextList.RemoveAt(posLast);
        }
    }


    tText.SetTransparency(bMaxStepAlpha);


    UIID idPowerIcon = UIIDCreateFromParent(ptQuestStack->GetID(), ".wrapper.power_icon");
    UIHotbarButton & tButton = UI_GetHotbarButtonFromID(idPowerIcon);
    if (snoPowerGranted != SNO_NONE)
    {
        tButton.SetActive(TRUE);

        if (snoPowerGranted != tButton.m_tData.m_snoPower)
        {
            HotbarButtonData tHotbarButtonData;
            tHotbarButtonData.SetSkill(snoPowerGranted, -1);
            tButton.UpdateSkillButtonState(tHotbarButtonData, FALSE, FALSE, FALSE, FALSE, FALSE);
        }
    }
    else
    {
        tButton.SetActive(FALSE);
    }


    UIID idMeterWrapper = UIIDCreateFromParent(ptQuestStack->GetID(), ".meter_wrapper");
    UIWindow & tWndMeterWrapper = UI_GetWindowFromID(idMeterWrapper);
    if (flMeter >= 0.0f && bShowMeter)
    {
        tWndMeterWrapper.SetActive(TRUE);
        UIID idMeterProgressBar = UIIDCreateFromParent(tWndMeterWrapper.GetID(), ".meter");
        UIProgressBar & tMeter = UI_GetProgressBarFromID(idMeterProgressBar);
        tMeter.SetExtentsAndPosition(0.0f, 1.0f, flMeter, -1.0f);
        UIID idMeterText = UIIDCreateFromParent(tWndMeterWrapper.GetID(), ".meter_title");
        UIText & tMeterText = UI_GetTextFromID(idMeterText);
        tMeterText.SetText(strMeter.str(), FALSE);
    }
    else
    {
        tWndMeterWrapper.SetActive(FALSE);
    }


    UIID idColorOrbsWrapper = UIIDCreateFromParent(ptQuestStack->GetID(), ".color_orbs_wrapper");
    UIWindow & tWndColorOrbsWrapper = UI_GetWindowFromID(idColorOrbsWrapper);
    uint32 dwQuestFlags = sGetQuestFlags(m_snoQuest);
    uint32 dwColorOrbsFlags = dwQuestFlags & 7;


    if (dwColorOrbsFlags)
    {
        tWndColorOrbsWrapper.SetActive(TRUE);


        for (int32 i = 0; i < 3; i++)
        {
            const BOOL bColorActive = (dwColorOrbsFlags >> i) & 1;
            UIID idColorOrb = UIIDCreateFromParent(idColorOrbsWrapper, sctQuestColorOrbIDs[i]);
            UI_GetWindowFromID(idColorOrb).SetActive(bColorActive);
        }
    }
    else
    {
        tWndColorOrbsWrapper.SetActive(FALSE);
    }
    return TRUE;
}



void ObjectiveHUDEntry::SetupQuestHUDStack(UIControlSourceKey & tQuestStackTemplateSrcKey, CRefString sName, const UIID & idParent)
{



    Vector2D vecOrigin(0.0f, 0.0f);
    UI_CreateControlFromTemplate(tQuestStackTemplateSrcKey, DynamicControlCreationParams(sName, idParent), &vecOrigin);



    POS pos = tStepTextList.GetHeadPosition();
    while (pos)
    {
        StepText & tStepText = tStepTextList.GetNext(pos);
        tStepText.MarkAsDirty();
    }
}




void ObjHUD::SetupHUDControls()
{
    if (m_fRebuildHUD)
    {

        m_fRebuildHUD = FALSE;


        SNOName snoname(UI_SNO, "ConsoleEventText");
        const UIID & idMainQuestRegion = UISTACK_OBJHUD_MAINQUEST_REGION;
        const UIID & idSideQuestRegion = UISTACK_OBJHUD_SIDEQUEST_REGION;

        UIControlSourceKey tQuestStackTemplateSrcKey(snoname.snoHandle, "eventtext_bkgrnd.quest_stack");


        UI_GetStackPanelFromID(idMainQuestRegion).DeleteAllChildren();
        UI_GetStackPanelFromID(idSideQuestRegion).DeleteAllChildren();


        ObjectiveHUDEntry * pEntry = GetMainQuest();
        if (pEntry)

            pEntry->SetupQuestHUDStack(tQuestStackTemplateSrcKey, "quest_stack_main", idMainQuestRegion);


        POS pos = sgptUIQuestGlobals->listTrackedSideQuests.GetHeadPosition();
        while (pos)
        {
            SNO snoQuest = sgptUIQuestGlobals->listTrackedSideQuests.GetNext(pos);
            ObjectiveHUDEntry * pEntryScoped = FindObjHUDEntry(snoQuest);
            if (pEntryScoped)
            {
                CRefString sName = CRefString().Format("quest_stack_side %d", snoQuest);
                UIID idRegionToUse(idSideQuestRegion);

                if (QuestIsAnyBountyType(snoQuest) || QuestIsAnySetDungeonType(snoQuest))
                {
                    idRegionToUse = idMainQuestRegion;
                    sName = CRefString().Format("quest_stack_main_bounty %d", snoQuest);
                }

                pEntryScoped->SetupQuestHUDStack(tQuestStackTemplateSrcKey, sName, idRegionToUse);
            }
        }
    }
}




void ObjHUD::Update()
{
    RActorID idPlayerRActor;
    ObjectiveHUDEntry * pEntry;
    SNO snoLevelArea;
    BOOL fSideQuestWasVisible;
    POS pos;
    if (m_tList.Count() == 0)
    {
        UI_GetWindowFromID(UIWND_OBJHUD).SetActive(FALSE);
        return;
    }
















    if (GameIsOpenWorld())
    {
        BOOL bFoundVisible = FALSE;
        RActorID idPlayerRActor = LocalPlayerRActorGet();
        SNO snoLevelArea = WorldGetLevelAreaSNOFromRActor(idPlayerRActor);
        POS pos = m_tList.GetHeadPosition();
        while (pos)
        {
            ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetNext(pos);

            if (tObjHUDEntry.m_snoQuest != GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC))
            {
                BOOL bInLevelArea = tObjHUDEntry.m_snoLevelArea == SNO_NONE ||
                                    tObjHUDEntry.m_snoLevelArea == snoLevelArea ||
                                    (snoLevelArea != SNO_NONE && tObjHUDEntry.m_snoQuest != SNO_NONE &&
                                     QuestHasAdditionalLevelArea(tObjHUDEntry.m_snoQuest, snoLevelArea));
                if (bInLevelArea)
                {
                    bFoundVisible = TRUE;
                    break;
                }
            }
        }
    }












































    UI_GetWindowFromID(UIWND_OBJHUD).SetActive(TRUE);
    {

        ACDID idACDPlayer = LocalPlayerACDGet();
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        BOOL bInSetDungeon = ptACDPlayer ? ACDIsParticipatingInSetDungeon(*ptACDPlayer) : FALSE;

        UIWindow & tWnd = UI_GetWindowFromID(UIWND_OBJHUD);
        Vector2D tl, wh;
        tWnd.GetRelativePosition(tl, wh);
        float fYOffset;
        if (PlayerGetNumPlayers() >= 3)
        {
            if (bInSetDungeon)
                fYOffset = (float)UI_GetXMLInt32Constant("ConsoleEventText:MultiplayerSetDungeonTopOffset", 230);
            else
                fYOffset = (float)UI_GetXMLInt32Constant("ConsoleEventText:MultiplayerTopOffset", 300);
        }
        else
        {
            fYOffset = (float)UI_GetXMLInt32Constant("ConsoleEventText:SingleplayerTopOffset", 200);
        }

        if (tl.y != fYOffset)
            tWnd.SetRelativePosition(tl, wh);
    }












    SetupHUDControls();


    idPlayerRActor = LocalPlayerRActorGet();
    snoLevelArea = WorldGetLevelAreaSNOFromRActor(idPlayerRActor);

    pEntry = GetMainQuest();
    if (pEntry)
    {
        UIID idQuestStack = UIIDCreateFromParent(UISTACK_OBJHUD_MAINQUEST_REGION, ".quest_stack_main");
        if (UI_TestID(idQuestStack))
        {
            UIStackPanel & tQuestStack = UI_GetStackPanelFromID(idQuestStack);
            pEntry->Update(&tQuestStack, snoLevelArea);
        }
    }







    fSideQuestWasVisible = FALSE;

    pos = sgptUIQuestGlobals->listTrackedSideQuests.GetHeadPosition();
    while (pos)
    {
        SNO snoQuest = sgptUIQuestGlobals->listTrackedSideQuests.GetNext(pos);
        ObjectiveHUDEntry * pEntryScoped = FindObjHUDEntry(snoQuest);
        if (pEntryScoped)
        {

            UIID idQuestStack = UIIDCreateFromParent(UISTACK_OBJHUD_SIDEQUEST_REGION, CRefString().Format(".quest_stack_side %d", snoQuest));
            if (QuestIsAnyBountyType(snoQuest) || QuestIsAnySetDungeonType(snoQuest))

                idQuestStack = UIIDCreateFromParent(UISTACK_OBJHUD_MAINQUEST_REGION, CRefString().Format(".quest_stack_main_bounty %d", snoQuest));

            if (UI_TestID(idQuestStack))
            {
                UIStackPanel & tQuestStack = UI_GetStackPanelFromID(idQuestStack);
                BOOL bVisible = pEntryScoped->Update(&tQuestStack, snoLevelArea);

                if (!QuestIsAnyBountyType(snoQuest) || QuestIsAnySetDungeonType(snoQuest))

                    fSideQuestWasVisible |= bVisible;
            }
        }
    }



























    if (!pEntry && !fSideQuestWasVisible && !GameIsOpenWorld())

        UI_GetWindowFromID(UIWND_OBJHUD).SetActive(FALSE);
}







void ObjHUD::DestroyEntry(SNO snoQuest)
{

    MarkHUDToBeRebuilt();
    sTrackQuest(snoQuest, FALSE);

    POS pos = m_tList.GetHeadPosition();
    while (pos)
    {
        POS posEntry = pos;
        ObjectiveHUDEntry & tObjHUDEntry = m_tList.GetNext(pos);
        if (tObjHUDEntry.m_snoQuest == snoQuest)
        {
            tObjHUDEntry.Destroy();
            m_tList.RemoveAt(posEntry);
            if (m_posMainQuest == posEntry)

                m_posMainQuest = NULL;
            break;
        }
    }
}





void ObjHUD::FlagAllStepsComplete(const SNO snoQuest, BOOL bRollback)
{


    ScopedQuestDefinitionLockR pdefQuest(snoQuest);


    ObjectiveHUDEntry * ptObjHUDEntry = FindObjHUDEntry(snoQuest);
    if (ptObjHUDEntry)
    {
        ObjectiveHUDEntry & tObjHUDEntry = *ptObjHUDEntry;

        POS posStep = tObjHUDEntry.tStepTextList.GetHeadPosition();
        while (posStep)
        {
            StepText & tStepText = tObjHUDEntry.tStepTextList.GetNext(posStep);

            tStepText.FlagAsComplete(bRollback, QuestIsAnySetDungeonType(snoQuest));
        }
    }
}







BOOL StepText::ProcessState(SNO snoQuest)
{



    const uint32 dwNow = CGameGetClientTicks();
    const uint32 uTicksInState = dwNow - m_uStateStartTime;

    switch (m_eState)
    {
    case OBJTEXT_DELAYED:
        if (uTicksInState >= ACTIVE_STEP_FADE_TIME)
            SwitchState(OBJTEXT_ACTIVE_FADING_IN);
        break;
    case OBJTEXT_ACTIVE_FADING_IN:
        if (uTicksInState >= ACTIVE_STEP_FADE_TIME)
        {
            m_bAlpha = 255;
            SwitchState(OBJTEXT_ACTIVE);
        }
        else
        {
            m_bAlpha = (uint8)(int32)((float)uTicksInState / (float)ACTIVE_STEP_FADE_TIME * 255.0f);
        }
        break;
    case OBJTEXT_ACTIVE:
        {












            const BOOL bAlwaysShow =
                (GameIsOpenWorld() &&
                 (UI_GetProgressBarFromID(UICONTROL_DUNGEON_FINDER_PROGRESSBAR).IsActive() ||
                  QuestIsAnySetDungeonType(snoQuest) ||
                  Prefs::GetGameplayPrefs().GetAlwaysShowAdventureObjectives())) ||
                UI_GetProgressBarFromID(UIPROGRESS_CHALLENGE_RIFT_PROGRESSBAR).IsActive() ||
                (!GameIsOpenWorld() && Prefs::GetGameplayPrefs().GetAlwaysShowCampaignObjectives());




            if (m_snoPowerGranted != SNO_NONE || m_bShowMeter || (UIMap::IsOpen() | bAlwaysShow))
            {




                m_bAlpha = 255;

                if (m_fShowAlert)
                {
                    if (uTicksInState < ACTIVE_STEP_DISPLAY_TIME)
                        m_bAlertAlpha = 255;
                    else if (uTicksInState >= ACTIVE_STEP_DISPLAY_TIME + ACTIVE_STEP_FADE_TIME)
                    {
                        m_bAlertAlpha = 0;
                        m_fShowAlert = FALSE;
                    }
                    else
                        m_bAlertAlpha = (uint8)(int32)((float)(ACTIVE_STEP_DISPLAY_TIME + ACTIVE_STEP_FADE_TIME - uTicksInState) / (float)ACTIVE_STEP_FADE_TIME * 255.0f);
                }
                else
                {
                    m_bAlertAlpha = 0;
                }
            }
            else if (uTicksInState < ACTIVE_STEP_DISPLAY_TIME)
            {
                m_bAlpha = 255;
            }
            else if (uTicksInState >= ACTIVE_STEP_DISPLAY_TIME + ACTIVE_STEP_FADE_TIME)
            {
                m_bAlpha = 0;
                POS pos = m_ptObjectiveUIList->GetHeadPosition();
                while (pos)
                {
                    ObjectiveEntry & tEntry = m_ptObjectiveUIList->GetNext(pos);
                    tEntry.m_eUpdateState = STEP_ALREADY_SEEN;
                }
            }
            else
            {
                m_bAlpha = (uint8)(int32)((float)(ACTIVE_STEP_DISPLAY_TIME + ACTIVE_STEP_FADE_TIME - uTicksInState) / (float)ACTIVE_STEP_FADE_TIME * 255.0f);
            }
        }
        break;
    case OBJTEXT_COMPLETE_FADE_OUT:
    case OBJTEXT_FAILED_FADE_OUT:
        {
            float flFrac;














            const FadeState eFade = scgtAlertFadeRamp.Evaluate(uTicksInState, flFrac);
            m_bAlpha = (uint8)(int32)(flFrac * 255.0f);
            if (eFade == FADESTATE_HIDDEN)
                return FALSE;
        }
        break;
    case OBJTEXT_COMPLETE_NO_FADE_OUT:
    case OBJTEXT_FAILED_NO_FADE_OUT:




        m_bAlpha = 0;
        break;
    }






    return TRUE;
}


void StepText::Initialize(const int32 nNewStepUID, const BOOL fIsUpdate)
{
    m_bDirty = TRUE;
    m_nStepUID = nNewStepUID;
    m_fIsUpdate = fIsUpdate;


    SwitchState(OBJTEXT_DELAYED);
    m_bAlpha = 0;
    m_bAlertAlpha = 255;
    m_fShowAlert = TRUE;
    for (int32 i = 0; i < 20; i++)
        m_tObjData[i].Init();

    m_ptObjectiveUIList = new ObjectiveEntryList;
    m_snoPowerGranted = SNO_NONE;
    m_bShowMeter = FALSE;
}





void StepText::DisposeControlsForHUD()
{

    if (!m_ptObjectiveUIList)
        return;



    POS pos = m_ptObjectiveUIList->GetHeadPosition();
    while (pos)
    {
        ObjectiveEntry & tEntry = m_ptObjectiveUIList->GetNext(pos);
        tEntry.DisposeControlsForHUD();
    }

    MarkAsDirty();
}



void StepText::Destroy()
{

    DisposeControlsForHUD();
    m_ptObjectiveUIList->RemoveAll();
    delete m_ptObjectiveUIList; m_ptObjectiveUIList = NULL;
}



void StepText::SwitchState(ObjTextState eNewState)
{













    m_uStateStartTime = CGameGetClientTicks();
    m_eState = eNewState;
}




void StepText::MakeCurrentObjectivesReappear()
{



    if (m_eState == OBJTEXT_ACTIVE)
    {
        m_uStateStartTime = CGameGetClientTicks();
        m_fShowAlert = FALSE;
    }
}




static BOOL sResolveObjectiveTag(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR szTag, float & flResult, float *, CRefString & strResult, SNO, int32 nLevel, void * ptData0, void * ptData1, FontSpecifier & eFontColor)
{
















    StepText * ptStepText = (StepText *)ptData0;
    BOOL bMatch = FALSE;
    const StepObjectiveData & tObjData = ptStepText->GetStepObjective(nLevel);
    QuestStepObjective * ptObjective = (QuestStepObjective *)ptData1;
    if (!strcasecmp(szTag, "Done"))
    {
        flResult = (float)tObjData.m_nState;
        eFontColor = FONTCOLOR_GREEN;
        bMatch = TRUE;
    }
    else if (!strcasecmp(szTag, "Needed"))
    {
        flResult = (float)ptObjective->nParam2;
        eFontColor = FONTCOLOR_GREEN;
        bMatch = TRUE;
    }
    else if (!strcasecmp(szTag, "Left"))
    {
        float flDone = (float)tObjData.m_nState;
        float flNeeded = (float)ptObjective->nParam2;
        flResult = flNeeded - flDone;
        eFontColor = FONTCOLOR_GREEN;
        bMatch = TRUE;
    }
    else if (!strcasecmp(szTag, "Name") && ptObjective->snoname1.snoGroup == ACTOR_SNO)
    {
        ScopedActorDefinitionLockR pdefActor(ptObjective->snoname1.snoHandle);
        if (pdefActor->eType == ACTORTYPE_MONSTER)
        {
            if (pdefActor->snoMonster != SNO_NONE && MonsterGetBaseName(pdefActor->snoMonster, strResult))
            {


                eFontColor = FONTCOLOR_GREEN;
                bMatch = TRUE;
            }
        }
        else if (pdefActor->eType == ACTORTYPE_GIZMO)
        {
            strResult = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), SNOToString(ACTOR_SNO, ptObjective->snoname1.snoHandle, FALSE).str());
            eFontColor = FONTCOLOR_GREEN;
            bMatch = TRUE;
        }




    }




    return bMatch;
}






static BOOL sIsDungeonOwnerLocal(const Player * ptCurrentPlayer)
{


    for (const Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (PlayerIsLocal(ptPlayer) && UISetDungeon::IsSetDungeonOwner(ptPlayer))
            return TRUE;
    }



    return FALSE;
}







static BOOL sGetObjectiveDescription(SNO snoQuest, StepText & tStepText, QuestStepObjective & tObjective, const SNO snoStringList, const int32 nObjectiveIndex, ObjectiveEntry & tEntry, BOOL bInverse, BOOL bUpdatingHUD, const Player * ptPlayer, BOOL bBonusObjective, QuestStepBonusObjectiveSet * ptBonusObjectiveSet)
{


    QuestType nQuestType = QuestGetQuestType(snoQuest);

    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);

    CRefString strLabel = QuestGetObjectiveDescriptionLabel(tObjective, ePlayerClass);

    CRefString strRawString;
    BOOL bHasString = StringListFetch(snoStringList, strLabel.str(), strRawString) && strRawString.Length() != 0;


    if (!bHasString)
    {

        if (!QuestIsAnySetDungeonType(snoQuest) || nQuestType != QUEST_TYPE_SET_DUNGEON_MASTERY ||
            tEntry.m_eState != OBJECTIVESTATE_INCOMPLETE)


            return FALSE;
    }






    StepObjectiveData & tObjData = tStepText.GetStepObjective(nObjectiveIndex);


    tEntry.m_sDescription = UIToolTips_ProcessTip(NULL, -1, -1, -1, nObjectiveIndex, strRawString, TRUE, sResolveObjectiveTag, &tStepText, &tObjective);


    if (bBonusObjective && nQuestType == QUEST_TYPE_CHALLENGE && ptBonusObjectiveSet)
    {

        int32 nBonusXPGranted = 0;
        int32 nBonusGoldGranted = 0;
        QuestGetBonusAward(ptPlayer, snoQuest, *ptBonusObjectiveSet, nBonusXPGranted, nBonusGoldGranted, TRUE);


        tEntry.m_sDescription += LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_QUESTLOG), "RewardXP"), UnicodeText::FormatNumber(nBonusXPGranted * tObjData.m_nState, TRUE, FNS_NORMAL, (XLocale)0).str(), NULL);



        tEntry.m_sDescription += CRefString().Format("\n%s {icon:gold}", UnicodeText::FormatNumber(nBonusGoldGranted * tObjData.m_nState, TRUE, FNS_NORMAL, (XLocale)0).str());
    }




    else if (nQuestType == QUEST_TYPE_SET_DUNGEON)
    {
        tEntry.m_sDescription = CRefString().Format("{icon:bullet,1.5} %s", tEntry.m_sDescription.str());
    }
    else if (nQuestType == QUEST_TYPE_SET_DUNGEON_BONUS)
    {
        BOOL bDungeonOwnerIsLocal = sIsDungeonOwnerLocal(ptPlayer);


        if (bDungeonOwnerIsLocal)
        {
            tEntry.m_sDescription = CRefString().Format("{icon:Marker_Set,1.5} %s", tEntry.m_sDescription.str());
            tEntry.m_fDarken = FALSE;
        }
        else
        {
            tEntry.m_sDescription = CRefString().Format("{icon:SetDungeon_RedSetIcon,1.5} %s", tEntry.m_sDescription.str());
            tEntry.m_fDarken = TRUE;
        }
    }
    else if (nQuestType == QUEST_TYPE_SET_DUNGEON_MASTERY)
    {
        BOOL bDungeonOwnerIsLocal = sIsDungeonOwnerLocal(ptPlayer);
        tEntry.m_sDescription = CRefString().Format("{icon:bullet2,1.5} %s", tEntry.m_sDescription.str());
        tEntry.m_fDarken = !bDungeonOwnerIsLocal;
    }

    tEntry.m_fHiddenUntilComplete = tObjective.bHiddenUntilComplete;
    switch (tObjective.eObjectiveType)
    {
    case OBJECTIVE_POSSESS_ITEM:
        tEntry.m_gbidItem = tObjective.gbidParam1;
        tEntry.m_nCurrentCount = tObjData.m_nState;
        tEntry.m_nDesiredCount = tObjective.nParam1;
        break;
    case OBJECTIVE_TIMED_EVENT_EXPIRED:
        tEntry.m_snoTimedEvent = tObjective.snoname1.snoHandle;
        break;


    case OBJECTIVE_KILL_ALL_MONSTERS_IN_WORLD:
        if (tObjData.m_nState > 0)
        {
            CRefString sVal = UnicodeText::FormatNumber(tObjData.m_nState, TRUE, FNS_NORMAL, (XLocale)0);
            CRefString sMsg = LocalizedSprintf(StringListFetch("General:QuestObjectiveMonstersRemaining"), sVal.str(), NULL);



            tEntry.m_sDescription += CRefString().Format("\n%s", sMsg.str());
        }
        break;
    }



    if (tEntry.m_gbidItem == GBID_NONE)
        tEntry.m_gbidItem = tObjective.gbidItemToShow;
    if (tObjData.m_fPassed)
    {



        if (bInverse)
            tEntry.MarkAsFailed(tObjData.m_fChanged != FALSE);
        else



            tEntry.MarkAsCompleted(tObjData.m_dwChangedTime, tObjData.m_fChanged != FALSE);
    }

    else if (bInverse && tStepText.GetTextState() == OBJTEXT_COMPLETE_FADE_OUT)
    {
        tEntry.MarkAsCompleted(tObjData.m_dwChangedTime, tObjData.m_fChanged != FALSE);
    }
    else
    {
        tEntry.m_eState = OBJECTIVESTATE_INCOMPLETE;
    }

    if (tObjData.m_fChanged)
    {
        tEntry.m_eUpdateState = tStepText.GetIsUpdate() ? STEP_IS_UPDATE : STEP_IS_NEW;
        tEntry.m_fTriggerAlert = TRUE;
        tObjData.m_fChanged = FALSE;
    }
    else
    {
        tEntry.m_eUpdateState = STEP_ALREADY_SEEN;
    }

    return TRUE;
}




void StepText::Update(
    const UIID & idObjStack,
    const ObjectiveHUDEntry & tObjHUDEntry,
    const BOOL bShow)
{
    if (!bShow)
    {
        return;
    }

    if (m_bDirty || m_ptObjectiveUIList->IsEmpty())
    {
        DisposeControlsForHUD();

        m_ptObjectiveUIList->RemoveAll();


        const Player * ptPlayer = LocalPlayerGet();

        SNO snoCachedTimedEvent = SNO_NONE;
        if (GameIsOpenWorld() && tObjHUDEntry.m_snoQuest == GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC))
        {
            GameCommonData & tGCD = GameGetActiveGameCommonData();
            for (int32 i = 0; i < 10; i++)
            {
                QuestStepObjective & tObjective = tGCD.ptQuestGlobals->tDynamicQuestObjectives[i];
                if (tObjective.eObjectiveType != OBJECTIVE_INVALID)
                {
                    ObjectiveEntry tEntry;
                    tEntry.m_sDescription = QuestGetDynamicObjectiveDescription(tObjective, -1);
                    tEntry.m_sLabel.Format("dynamic objective: %d", tObjective.nUID);

                    tEntry.m_fHidden = tGCD.ptQuestGlobals->arDynamicQuestObjectiveHidden[i];

                    const int32 DYNAMIC_QUEST_ASSIGNED_STEP = 2;

                    StepText * ptStepText = sFindStepText(GlobalSNOGet(GLOBALSNO_QUEST_DYNAMIC), DYNAMIC_QUEST_ASSIGNED_STEP, NULL);
                    if (ptStepText)
                    {
                        StepObjectiveData & tObjData = ptStepText->GetStepObjective(tObjective.nUID);

                        tEntry.m_sDescription = QuestGetDynamicObjectiveDescription(tObjective, tObjData.m_nState);

                        if (tObjData.m_fPassed)
                        {
                            tEntry.MarkAsCompleted(tObjData.m_dwChangedTime, tObjData.m_fChanged);
                            tEntry.m_eUpdateState = STEP_IS_UPDATE;
                        }

                        if (tObjData.m_fChanged)
                        {
                            if (tObjData.m_fPassed)
                            {
                                tEntry.m_eUpdateState = STEP_IS_UPDATE;
                                tEntry.m_fTriggerAlert = TRUE;
                            }
                            else if (tObjData.m_nState == 0)
                            {
                                tEntry.m_eUpdateState = STEP_IS_NEW;
                                tEntry.m_fTriggerAlert = TRUE;
                            }
                            else
                            {
                                tEntry.m_eUpdateState = STEP_ALREADY_SEEN;
                            }
                        }

                        switch (tObjective.eObjectiveType)
                        {
                        case OBJECTIVE_KILL_MONSTER_FROM_FAMILY:
                        case OBJECTIVE_KILL_ANY_MONSTER_IN_LEVEL_AREA:
                            tEntry.m_nCurrentCount = tObjData.m_nState;
                            tEntry.m_nDesiredCount = tObjective.nParam2;
                        case OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA:
                            if (tObjective.eObjectiveType == OBJECTIVE_KILL_ELITE_PACK_IN_LEVEL_AREA && tObjective.nParam2 >= 2)
                            {
                                tEntry.m_nCurrentCount = tObjData.m_nState;
                                tEntry.m_nDesiredCount = tObjective.nParam2;
                            }
                            break;


                        case OBJECTIVE_HAD_CONVERSATION:
                            gtTutorialMgr.ShowTutorial(UIWND_OBJHUD, GlobalSNOGet(GLOBALSNO_TUTORIAL_OPEN_WORLD_QUEST_COMPLETE), NULL, TRUE);
                            break;
                        }
                        if (tObjData.m_fChanged)
                            tObjData.m_fChanged = FALSE;
                    }

                    m_ptObjectiveUIList->AddTail_Tracked(tEntry);
                }
            }
        }


        else
        {
            ScopedQuestDefinitionLockR pdefQuest(tObjHUDEntry.m_snoQuest);
            SNO snoStringList = QuestGetStringList(tObjHUDEntry.m_snoQuest);
            int32 nStepIndex = QuestStepUIDToIndex(pdefQuest, m_nStepUID);
            if (nStepIndex != -1)
            {
                QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];
                m_snoPowerGranted = tStep.snoPowerGranted;
                m_bShowMeter = (tStep.dwFlags >> QSF_SHOW_METER) & 1;

                int32 nNumObjectiveSets = tStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
                int32 nObjectiveIndex = 0;
                for (int32 i = 0; i < nNumObjectiveSets; i++)
                {
                    QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[i];
                    int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
                    for (int32 j = 0; j < nNumObjectives; j++, nObjectiveIndex++)
                    {
                        QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[j];


                        if (tObjective.eObjectiveType == OBJECTIVE_TIMED_EVENT_EXPIRED)

                            snoCachedTimedEvent = tObjective.snoname1.snoHandle;


                        ObjectiveEntry tEntry;
                        if (sGetObjectiveDescription(tObjHUDEntry.m_snoQuest, *this, tObjective, snoStringList, nObjectiveIndex, tEntry, FALSE, TRUE, ptPlayer, FALSE, NULL))
                        {
                            CRefString sStepLabel;
                            tEntry.m_sLabel.Format("%d %d %d", tObjHUDEntry.m_snoQuest, m_nStepUID, m_ptObjectiveUIList->Count());
                            m_ptObjectiveUIList->AddTail_Tracked(tEntry);
                        }

                    }

                }
                int32 nNumBonusObjectiveSets = tStep.serStepBonusObjectiveSets.dwSize / sizeof(QuestStepBonusObjectiveSet);
                for (int32 i = 0; i < nNumBonusObjectiveSets; i++)
                {
                    QuestStepBonusObjectiveSet & tObjectiveSet = tStep.arStepBonusObjectiveSets[i];
                    int32 nNumObjectives = tObjectiveSet.serStepBonusObjectives.dwSize / sizeof(QuestStepObjective);
                    for (int32 j = 0; j < nNumObjectives; j++, nObjectiveIndex++)
                    {
                        QuestStepObjective & tObjective = tObjectiveSet.arStepBonusObjectives[j];

                        ObjectiveEntry tEntry;
                        if (sGetObjectiveDescription(tObjHUDEntry.m_snoQuest, *this, tObjective, snoStringList, nObjectiveIndex, tEntry, tObjectiveSet.bInverse, TRUE, ptPlayer, TRUE, &tObjectiveSet))
                        {
                            if (!(tStep.dwFlags & (1 << QSF_HIDE_BONUS_TEXT)))

                                tEntry.m_sDescription = CRefString().Format("%s %s", StringListFetch("General:QuestBonus").str(), tEntry.m_sDescription.str());


                            CRefString sStepLabel;
                            tEntry.m_sLabel.Format("%d %d %d", tObjHUDEntry.m_snoQuest, m_nStepUID, m_ptObjectiveUIList->Count());
                            m_ptObjectiveUIList->AddTail_Tracked(tEntry);
                        }

                    }

                }
            }
        }
        if (m_ptObjectiveUIList->Count() == 0)
        {

            ObjectiveEntry tEntry;
            QuestStepGetSplashText(tObjHUDEntry.m_snoQuest, m_nStepUID, tEntry.m_sDescription);


            switch (m_eState) {
            case OBJTEXT_COMPLETE_FADE_OUT:
                tEntry.MarkAsCompleted(CGameGetClientTicks(), TRUE);  // The completed and failed paths share the trigger-alert store at 0x301b20.
                tEntry.m_fTriggerAlert = TRUE;
                break;
            case OBJTEXT_FAILED_FADE_OUT:

                tEntry.MarkAsFailed(TRUE);
                tEntry.m_fTriggerAlert = TRUE;
                break;

            default:
                tEntry.m_eState = OBJECTIVESTATE_INCOMPLETE;
                if (m_eState == OBJTEXT_DELAYED)

                    tEntry.m_fTriggerAlert = TRUE;
            }


            if (snoCachedTimedEvent != SNO_NONE)

                tEntry.m_snoTimedEvent = snoCachedTimedEvent;


            CRefString sStepLabel;
            tEntry.m_sLabel.Format("%d %d %d", tObjHUDEntry.m_snoQuest, m_nStepUID, m_ptObjectiveUIList->Count());
            m_ptObjectiveUIList->AddTail_Tracked(tEntry);
        }

        POS pos = m_ptObjectiveUIList->GetHeadPosition();
        while (pos)
        {
            ObjectiveEntry & tEntry = m_ptObjectiveUIList->GetNext(pos);
            tEntry.SetupControlsForHUD(idObjStack, tEntry.m_sLabel);
        }

        m_bDirty = FALSE;
    }



    const uint32 CONSOLE_TIME_BEFORE_COMPLETE_FADE = 300;
    const uint32 CONSOLE_TIME_FADE = 60;

    POS eventPos = m_ptObjectiveUIList->GetHeadPosition();
    while (eventPos)
    {
        ObjectiveEntry & tEntry = m_ptObjectiveUIList->GetNext(eventPos);
        if (tEntry.m_eState == OBJECTIVESTATE_PASSED && !tEntry.m_fHidden)
        {
            m_fShowAlert = TRUE;
            uint32 uNow = CGameGetClientTicks();

            uint32 nTimeSinceComplete = uNow - tEntry.GetCompletionTime();
            if (nTimeSinceComplete > CONSOLE_TIME_BEFORE_COMPLETE_FADE)
            {
                if (nTimeSinceComplete > CONSOLE_TIME_BEFORE_COMPLETE_FADE + CONSOLE_TIME_FADE)
                {
                    tEntry.m_fHidden = TRUE;
                    tEntry.m_bStepFadeAlpha = 0;
                }
                else
                {
                    tEntry.m_bStepFadeAlpha = 255 - ((nTimeSinceComplete - CONSOLE_TIME_BEFORE_COMPLETE_FADE) * 255 / CONSOLE_TIME_FADE);
                }
            }
        }
    }






    uint32 uTicksInState = CGameGetClientTicks() - m_uStateStartTime;

    POS pos = m_ptObjectiveUIList->GetHeadPosition();
    while (pos)
    {
        ObjectiveEntry & tEntry = m_ptObjectiveUIList->GetNext(pos);
        if (m_bAlpha)
        {
            tEntry.UpdateAlertAlpha();
        }

        uint8 bAlphaToUse = m_bAlpha;
        bAlphaToUse >>= (tEntry.m_fDarken ? 1 : 0);




        BOOL fExtendedPastBounds = tEntry.UpdateControlsForHUD(bAlphaToUse, uTicksInState, m_bAlertAlpha, FALSE);
        if (fExtendedPastBounds)
        {
            break;
        }
    }




}



void StepText::FlagAsComplete(const BOOL bRollback, const BOOL bNoFade)
{


    if (m_eState != OBJTEXT_COMPLETE_FADE_OUT && m_eState != OBJTEXT_FAILED_FADE_OUT)
    {

        MarkAsDirty();
        ObjTextState eState = bRollback ? (bNoFade ? OBJTEXT_FAILED_NO_FADE_OUT : OBJTEXT_FAILED_FADE_OUT)
                                        : (bNoFade ? OBJTEXT_COMPLETE_NO_FADE_OUT : OBJTEXT_COMPLETE_FADE_OUT);
        SwitchState(eState);
    }
}










CRefString UIQuests::GetCurrentObjectiveText(QuestInstance * ptQuestInst)
{
    CRefString sObjective;


    ScopedQuestDefinitionLockR pdefQuest(ptQuestInst->snoQuest);
    SNO snoStringList = QuestGetStringList(ptQuestInst->snoQuest);
    int32 nStepIndex = QuestStepUIDToIndex(pdefQuest, ptQuestInst->nCurrentStepUID);

    if (nStepIndex != -1)
    {

        QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];
        StepText * ptStepText = sFindStepText(ptQuestInst->snoQuest, ptQuestInst->nCurrentStepUID, NULL);

        if (ptStepText)
        {
            int32 nNumObjectiveSets = tStep.serStepObjectiveSets.dwSize / sizeof(QuestStepObjectiveSet);
            for (int32 i = 0; i < nNumObjectiveSets; i++)
            {
                QuestStepObjectiveSet & tObjectiveSet = tStep.arStepObjectiveSets[i];
                int32 nNumObjectives = tObjectiveSet.serStepObjectives.dwSize / sizeof(QuestStepObjective);
                for (int32 j = 0; j < nNumObjectives; j++)
                {
                    QuestStepObjective & tObjective = tObjectiveSet.arStepObjectives[j];
                    if (!tObjective.bHiddenUntilComplete)
                    {
                        CRefString strLabel = QuestGetObjectiveDescriptionLabel(tObjective, -1);
                        CRefString strRawString;
                        BOOL bHasString = StringListFetch(snoStringList, strLabel.str(), strRawString) && strRawString.Length() != 0;
                        if (bHasString)
                        {
                            CRefString sDescription = UIToolTips_ProcessTip(NULL, -1, -1, -1, j, strRawString, TRUE, sResolveObjectiveTag, ptStepText, &tObjective);
                            sObjective += sDescription + CRefString("\n");
                        }
                    }
                }
            }
        }
    }



    if (sObjective.Length() == 0)

        QuestStepGetSplashText(ptQuestInst->snoQuest, ptQuestInst->nCurrentStepUID, sObjective);


    return sObjective;
}











static BOOL sShouldShowDynamicObjectives()

{
    OpenWorldMode eOpenWorldMode = GameGetOpenWorldMode();
    if (eOpenWorldMode != OPENWORLD_MODE_BOUNTY)
    {
        return FALSE;
    }


    BOOL bHasObjectives = FALSE;
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    for (int32 i = 0; i < 10; i++)
    {
        QuestStepObjective & tObjective = tGCD.ptQuestGlobals->tDynamicQuestObjectives[i];
        if (tObjective.eObjectiveType != OBJECTIVE_INVALID)
        {
            bHasObjectives = TRUE;
            break;
        }
    }

    return bHasObjectives;
}
