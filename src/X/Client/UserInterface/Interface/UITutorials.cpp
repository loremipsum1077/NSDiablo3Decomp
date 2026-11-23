// Recovered from Diablo III 2.6.2 DWARF and original instruction/line evidence.
#include "UITutorials.h"
#include "../Controls/UITutorialControl.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Client/CGame.h"
// Inference: original initializer constructs the four GfxVB.h:351-354 normal colors.
#include "../../../Client/Gfx/GfxVB.h"
// DWARF InputNX64.h:74 and InputNX64.cpp:1203; RActors.cpp:6546.
enum NpadType { TYPE_NONE=0, TYPE_HANDHELD=1, TYPE_FULLKEY=2, TYPE_JOYDUAL=3, TYPE_JOYLEFT=4, TYPE_JOYRIGHT=5, TYPE_INVALID=6 }; int32_t InputGetControllerType(int32_t nControllerIndex); RActorID RActorFindByANN(const ACDNetworkName idANN);
// DWARF UI.cpp:5658 and UIGlobals.cpp:1370.
UITutorial & UI_GetTutorialFromID(const UIID & id); void UI_DeleteControl(UIID & idControl, const BOOL fAssertIfDeletingRoot); template <typename T> void UI_DeleteControl(T *& ptControl);  // DWARF UI.cpp:2866 and UI.h:550 (src UI.h lacks both; template defined at end of file)
extern UIID UIDLG_TUTORIAL;
// DWARF UITutorials.cpp:41; definition is recovered with its constructor.
extern UITutorialManager gtTutorialMgr;

// DWARF CPlayer.cpp:1612, Player.cpp:1956, Tutorials.cpp:108.
Player * LocalPlayerGet();
Player * PlayerGetByPlayerIndex(int32 nPlayerIndex);
namespace Tutorial { void Shown(Player & tPlayer, const SNO snoTutorial); }
#include "../../../Common/Definitions/TutorialDefinition.h"
#include "../../../Client/Definitions/PreferencesDefinition.h"
#include "../../../Common/GameTesting.h"
#include "../../../Common/Player.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Client/Sound/Sound.h"
#include "../../../Client/Battlenet/Subsystems/FanfareManager.h"
// DWARF TutorialDefinition.h:19; named bit positions required by these live callers.
enum { TUTORIAL_IS_FREESTANDING_BIT=0, TUTORIAL_RECURS_UNTIL_LEARNED=3, TUTORIAL_CANNOT_BE_DISABLED=4 };
// DWARF GameTemplate.cpp, CWorld.cpp, GameTesting.cpp: function declarations.
BOOL GameTypeIsPvP(); BOOL CWorldIsRetro(); namespace GameTesting { TestType GetTestType(); }
// DWARF ConsoleScreenManager.cpp and Tutorials.cpp:181.
namespace Console { namespace Screens { FanfareManager & GetFanfareManager(); } } namespace Tutorial { BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial); }
// DWARF UIGlobals.cpp:1371.
extern UIID UIWND_FLOATING_TUTORIAL_ANCHOR;
#include "../../../Common/Protobuf/Account.pb.h"
#include "../../../Common/Protobuf/GameMessage.pb.h"
#include "../UI.h"
#include "../../../Client/Console/GamerProfile.h"


UITutorialManager gtTutorialMgr;


UITutorialManager::UITutorialManager()

{
    m_bInitialized = FALSE;
    m_ptTutorial = NULL;
    m_snoQueuedTutorial = SNO_NONE;
    m_idQueuedParent = GetUIIDNone();
    m_dwTickToDisplay = (uint32)-1; m_nPlayerIndex = -1;
    m_dwQueuedDelay = 0;
}



void UITutorialManager::InitializeForGame()

{
    m_ptTutorial = &UI_GetTutorialFromID(UIDLG_TUTORIAL);
    m_bInitialized = TRUE;
}


void UITutorialManager::DisposeForGame()

{
    if (m_bInitialized)
    {
        UI_DeleteControl(m_ptTutorial);

        m_bInitialized = FALSE;
    }
}

// LINKER-REMOVED: concrete DIEs (low_pc 0) at UITutorials.cpp:77 and :83; bodies from their inline trees (RemoveAll at 80, Lookup at 87, RemoveAt at 89).
void UITutorialManager::ClearTrackedTutorials()

{
    m_mapRecurringTutorials.RemoveAll();
}

void UITutorialManager::ClearTrackedTutorial(
    const SNO snoTutorial)
{
    RecurringTutorialData tDataIgnored;
    if (m_mapRecurringTutorials.Lookup(snoTutorial, tDataIgnored))
    {
        m_mapRecurringTutorials.RemoveAt(snoTutorial);
    }
}


BOOL UITutorialManager::sWasPlayed(
    const SNO snoTutorial,
    const Player * ptPlayer)
{
    if (!ptPlayer)
    {
        // Inference: all veto guards here share the original TRUE return at line132.
        return TRUE;
    }
    if (GameTypeIsPvP())
    {
        return TRUE;
    }

    if (CWorldIsRetro())
    {
        return TRUE;
    }

    if (GameTesting::GetTestType() != TESTTYPE_NONE)
    {
        return TRUE;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return TRUE;
    }


    ScopedTutorialDefinitionLockR pdefTutorial(snoTutorial);
    if (!(pdefTutorial->dwFlags & (1 << TUTORIAL_CANNOT_BE_DISABLED)) &&
        !Prefs::GetGameplayPrefs().GetShowTutorials())
    {
        return TRUE;
    }

    return Tutorial::PlayerHasSeen(*ptPlayer, snoTutorial);
}


void UITutorialManager::sMarkAsPlayed(
    const SNO snoTutorial,
    Player * ptPlayer)
{
    // Inference: null uses LocalPlayerGet (live line141); nonnull uses its argument (account overload line357).
    if (!ptPlayer)
        ptPlayer = LocalPlayerGet();

    Tutorial::Shown(*ptPlayer, snoTutorial);
}



BOOL UITutorialManager::sHandleShowingTutorial(
    const Player * ptPlayer,
    const SNO snoTutorial,
    const UIID & idParent,
    const Rect2D & rectParent,
    const BOOL fIgnoreIfAlreadyBeingTracked,
    BOOL & fPlaySound)
{


    fPlaySound = FALSE;

    ScopedTutorialDefinitionLockR pdefTutorial(snoTutorial);

    // Inference: original line163 combines the freestanding/fanfare guards and shares FALSE return.
    if ((pdefTutorial->dwFlags & (1 << TUTORIAL_IS_FREESTANDING_BIT)) && Console::Screens::GetFanfareManager().IsActive())
    {
        return FALSE;
    }


    if (pdefTutorial->dwFlags & (1 << TUTORIAL_RECURS_UNTIL_LEARNED))
    { // Inference: line173 combines the lookup result and ignore-tracked guards below.
        RecurringTutorialData tData;
        BOOL fAlreadyShownOnce = m_mapRecurringTutorials.Lookup(snoTutorial, tData);
        if (fAlreadyShownOnce && fIgnoreIfAlreadyBeingTracked)
        {
            return FALSE;
        }

        // Inference: original selects -1 when the optional player is absent.
        tData.nPlayerIndex = ptPlayer ? ptPlayer->nPlayerIndex : -1;
        tData.idParent = idParent;
        tData.rectParent = rectParent;
        tData.dwRecurTime = CGameGetClientTicks() + pdefTutorial->dwRecurTime;
        tData.nOccurrencesLeftUntilLearned = pdefTutorial->nOccurrencesUntilLearned;
        m_mapRecurringTutorials.SetAt_Tracked(snoTutorial, tData);

        fPlaySound = !fAlreadyShownOnce;
    }
    else
    {
        fPlaySound = TRUE;
        sMarkAsPlayed(snoTutorial, NULL);
    }

    return TRUE;
}



UITutorial * UITutorialManager::ShowTutorial(
    const UIID & idDesiredParent,
    const SNO snoTutorial,
    const Player * ptPlayer,
    const BOOL fIgnoreIfAlreadyBeingTracked)
{
    if (!ptPlayer)
    {
        ptPlayer = LocalPlayerGet();
    }

    if (sWasPlayed(snoTutorial, ptPlayer))
    {
        return NULL;
    }

    UIID idParent = idDesiredParent;
    { // Inference: the definition lock and freestanding flag share this scope before line229.
        ScopedTutorialDefinitionLockR pdefTutorial(snoTutorial);
        BOOL bFreestanding = (pdefTutorial->dwFlags & (1 << TUTORIAL_IS_FREESTANDING_BIT)) != 0;
        if (idParent == GetUIIDNone())
        { // Inference: missing parent for a nonfreestanding tutorial shares the NULL exit.
            if (!bFreestanding)
                return NULL;

            idParent = UIWND_FLOATING_TUTORIAL_ANCHOR;
        }
    }


    BOOL fPlaySound = FALSE;
    if (!sHandleShowingTutorial(ptPlayer, snoTutorial, idParent, cgrectNull, fIgnoreIfAlreadyBeingTracked, fPlaySound))
    {
        return NULL;
    }

    UITutorial & tTutorial = *m_ptTutorial;
    tTutorial.SetTutorial(idParent, snoTutorial, ptPlayer);
    // Inference: original line238 combines the play-sound flag and live-owner guard.
    if (fPlaySound && !tTutorial.OwnerIsMissing())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TUTORIAL_PLAYED), cgtPSODefaults);
    }

    return &tTutorial;
}


UITutorial * UITutorialManager::ShowTutorial(
    const Rect2D & parentRect,
    const SNO snoTutorial,
    const Player * ptPlayer,
    const BOOL fIgnoreIfAlreadyBeingTracked)
{
    if (!ptPlayer)
    {
        ptPlayer = LocalPlayerGet();
    }

    if (sWasPlayed(snoTutorial, ptPlayer))
    {
        return NULL;
    }

    BOOL fPlaySound = FALSE;
    if (!sHandleShowingTutorial(ptPlayer, snoTutorial, GetUIIDNone(), parentRect, fIgnoreIfAlreadyBeingTracked, fPlaySound))
    {
        return NULL;
    }

    UITutorial & tTutorial = *m_ptTutorial;
    tTutorial.SetTutorial(parentRect, RACTORID_INVALID, snoTutorial, ptPlayer);
    // Inference: original line272 combines the play-sound flag and live-owner guard.
    if (fPlaySound && !tTutorial.OwnerIsMissing())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TUTORIAL_PLAYED), cgtPSODefaults);
    }

    return &tTutorial;
}


UITutorial * UITutorialManager::ShowTutorial(
    const RActorID idRActor,
    const SNO snoTutorial,
    const Player * ptPlayer,
    const BOOL fIgnoreIfAlreadyBeingTracked)
{
    if (!ptPlayer)
    {
        ptPlayer = LocalPlayerGet();
    }

    if (sWasPlayed(snoTutorial, ptPlayer))
    {
        return NULL;
    }

    BOOL fPlaySound = FALSE;
    if (!sHandleShowingTutorial(ptPlayer, snoTutorial, GetUIIDNone(), cgrectNull, fIgnoreIfAlreadyBeingTracked, fPlaySound))
    {
        return NULL;
    }

    UITutorial & tTutorial = *m_ptTutorial;
    tTutorial.SetTutorial(cgrectNull, idRActor, snoTutorial, ptPlayer);
    // Inference: original line306 combines the play-sound flag and live-owner guard.
    if (fPlaySound && !tTutorial.OwnerIsMissing())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TUTORIAL_PLAYED), cgtPSODefaults);
    }

    return &tTutorial;
}



UITutorial * UITutorialManager::ShowTutorial(
    const UIID & idDesiredParent,
    const SNO snoTutorial,
    D3::Account::SavedDefinition & tAccountDefinition)
{

    if (!GameIsStartUp())
    {
        // Inference: the startup guard returns NULL at the shared original epilogue.
        return NULL;
    }

    Player tPlayer;

    if (tAccountDefinition.seen_tutorials().size())
    {

        SigmaMemoryCopy(tPlayer.tSeenTutorials.GetRawBuffer(), tAccountDefinition.seen_tutorials().c_str(), tAccountDefinition.seen_tutorials().size());
    }


    if (sWasPlayed(snoTutorial, &tPlayer))
    {
        return NULL;
    }

    UIID idParent = idDesiredParent;
    { // Inference: the definition lock and freestanding flag share this scope before line357.
        ScopedTutorialDefinitionLockR pdefTutorial(snoTutorial);
        BOOL bFreestanding = (pdefTutorial->dwFlags & (1 << TUTORIAL_IS_FREESTANDING_BIT)) != 0;
        if (idParent == GetUIIDNone())
        { // Inference: missing parent for a nonfreestanding tutorial shares the NULL exit.
            if (!bFreestanding)
                return NULL;

            idParent = UIWND_FLOATING_TUTORIAL_ANCHOR;
        }
    }



    sMarkAsPlayed(snoTutorial, &tPlayer);


    tAccountDefinition.set_seen_tutorials(tPlayer.tSeenTutorials.GetRawBuffer(), tPlayer.tSeenTutorials.GetLength());

    UITutorial & tTutorial = *m_ptTutorial;
    tTutorial.SetTutorial(idParent, snoTutorial, NULL);

    if (!tTutorial.OwnerIsMissing())
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_TUTORIAL_PLAYED), cgtPSODefaults);
    }

    return &tTutorial;
}



BOOL UITutorialManager::CloseTutorialIfOpen(
    const SNO snoTutorial)
{

    PerformedDesiredTutorialBehavior(snoTutorial);
    // Inference: the null and SNO branches at original line 381 form this combined guard.
    if (m_ptTutorial && m_ptTutorial->GetTutorial() == snoTutorial)
    {
        m_ptTutorial->SetActive(FALSE);
        // Inference: the original line-383 constant one is the successful return.
        return TRUE;
    }
    // Inference: failed guards return the original zero result.
    return FALSE;
}
void UITutorialManager::sHandleMissingParents()

{
    UITutorial & tTutorial = *m_ptTutorial;


    if (tTutorial.OwnerIsMissing())
    {
        tTutorial.SetActive(FALSE);
    }
}


void UITutorialManager::QueueTutorial(
    const UIID & idParent,
    const SNO snoTutorial,
    uint32 dwDelayInTicks,
    int32 nPlayerIndex)
{
    m_snoQueuedTutorial = snoTutorial;
    m_dwQueuedDelay = dwDelayInTicks;
    m_idQueuedParent = idParent;
    m_dwTickToDisplay = (uint32)-1;
    m_nPlayerIndex = nPlayerIndex;
}


BOOL UITutorialManager::IsAnyShowing(
    uint32 dwIgnoreMask) const
{ // Inference: line 420 combines the pointer guard and cached-active test below.
    if (m_ptTutorial && m_ptTutorial->IsActive())
    {
        if (m_ptTutorial->IsFreestanding())
        {
            return !(dwIgnoreMask & IGNORE_FREESTANDING_TUTORIALS);
        }
        else if (m_ptTutorial->IsChatBalloon())
        {
            return !(dwIgnoreMask & IGNORE_CHATBALLOON_TUTORIALS);
        }
        else
        {
            return !(dwIgnoreMask & IGNORE_ANCHORED_TUTORIALS);
        }
    }
    return FALSE;
}


void UITutorialManager::PerformedDesiredTutorialBehavior(
    const SNO snoTutorial)
{
    RecurringTutorialData tData;
    if (m_mapRecurringTutorials.Lookup(snoTutorial, tData))
    {
        --tData.nOccurrencesLeftUntilLearned;
        if (tData.nOccurrencesLeftUntilLearned > 0)
        {

            m_mapRecurringTutorials.SetAt_Tracked(snoTutorial, tData);
        }
        else
        {
            // Inference: this call inlines the original line-141 LocalPlayerGet path.
            sMarkAsPlayed(snoTutorial, NULL);
            m_mapRecurringTutorials.RemoveAt(snoTutorial);
        }
    }

}



void UITutorialManager::sUpdateRecurringTutorials()

{
    uint32 dwCurTime = CGameGetClientTicks();
    POS pos = m_mapRecurringTutorials.GetStartAssoc();
    while (pos)
    {
        SNO snoTutorial;
        RecurringTutorialData tData;
        m_mapRecurringTutorials.GetNextAssoc(pos, snoTutorial, tData);

        if (dwCurTime > tData.dwRecurTime)
        {

            if (tData.idParent != GetUIIDNone())
            {
                ShowTutorial(tData.idParent, snoTutorial, PlayerGetByPlayerIndex(tData.nPlayerIndex), FALSE);
            }
            else
            {
                ShowTutorial(tData.rectParent, snoTutorial, PlayerGetByPlayerIndex(tData.nPlayerIndex), FALSE);
            }
        }
    }
}



void UITutorialManager::Update()

{ // Inference: line-494 initialization guard uses an early return for the shared epilogue.
    if (!m_bInitialized)
    {
        return;
    }

    sUpdateRecurringTutorials();
    sHandleMissingParents();


    UITutorial & tTutorial = *m_ptTutorial;
    if (tTutorial.IsActive())
    {
        tTutorial.Update();
        m_dwTickToDisplay = (uint32)-1;
    }
    // Inference: original line 510 combines the inactive and queued-SNO guards.
    if (!tTutorial.IsActive() && m_snoQueuedTutorial != SNO_NONE)
    {
        if (m_dwTickToDisplay == (uint32)-1)
        {
            m_dwTickToDisplay = CGameGetClientTicks() + m_dwQueuedDelay;
        }

        if (CGameGetClientTicks() > m_dwTickToDisplay)
        {
            Player * ptPlayer = PlayerGetByPlayerIndex(m_nPlayerIndex);
            ShowTutorial(m_idQueuedParent, m_snoQueuedTutorial, ptPlayer, TRUE);

            m_snoQueuedTutorial = SNO_NONE;
            m_idQueuedParent = GetUIIDNone();
            m_dwTickToDisplay = (uint32)-1; m_nPlayerIndex = -1;
            m_dwQueuedDelay = 0;
        }
    }

}


void UITutorialManager::Close(
    const UIID &)
{
    if (gtTutorialMgr.m_ptTutorial)
    {
        gtTutorialMgr.m_ptTutorial->SetActive(FALSE);
    }
}



void UITutorialManager::OnButtonClose(
    const UIID & id,
    const int32 nControllerIndex)
{
    if (gtTutorialMgr.m_ptTutorial)
    {
        SNO snoTutorial = gtTutorialMgr.m_ptTutorial->GetTutorial();
        if (snoTutorial != SNO_NONE)
        {
            gtTutorialMgr.CloseTutorialIfOpen(snoTutorial);
        }
    }
}



void UITutorialManager::ShowTutorialFromMessage(
    const D3::GameMessage::TutorialMessage & tMessage)
{
    UIID idParent = GetUIIDNone();
    RActorID idRActorTarget = RACTORID_INVALID;
    SNO snoTutorial = tMessage.tutorial_sno();
    { // Inference: the tutorial lock scope spans the original flag and target-selection paths.
        ScopedTutorialDefinitionLockR pdefTutorial(snoTutorial);




        SNO snoEvadeTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_EVADE);
        if (snoTutorial == snoEvadeTutorial)
        {




            int32_t nType = InputGetControllerType(Console::GamerProfile::GetPrimaryProfileUserIndex());
            if (nType == TYPE_JOYLEFT || nType == TYPE_JOYRIGHT)
            {

                snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_EVADE_JOYCON_NX64);
            }
        }


        if (pdefTutorial->dwFlags & (1 << TUTORIAL_IS_FREESTANDING_BIT))
        {
            ShowTutorial(idParent, snoTutorial, NULL, TRUE);
        }
        else
        {
            if (tMessage.has_ui_anchor_name())
            {
                idParent = UIIDCreate(tMessage.ui_anchor_name().c_str());
            }
            if (tMessage.has_actor_id())
            { // Inference: actor_id carries the ACDNetworkName value consumed by RActorFindByANN.
                idRActorTarget = RActorFindByANN((ACDNetworkName)tMessage.actor_id());
            }
            // Inference: original line602 combines valid-ID and cached-active guards.
            if (UI_TestID(idParent) && UI_GetControlFromID(idParent).IsActive())
            {
                ShowTutorial(idParent, snoTutorial, NULL, TRUE);
            }
            else if (idRActorTarget != RACTORID_INVALID)
            {
                ShowTutorial(idRActorTarget, snoTutorial, NULL, TRUE);
            }
        }
    }


    Tutorial::Shown(*LocalPlayerGet(), snoTutorial);

}

// DWARF X/Client/UserInterface/UI.h:550-559 (_Z16UI_DeleteControlI10UITutorialEvRPT_, inlined only; not declared inline).
// src UI.h lacks this template; CPlayer.cpp, ModalNotificationManager.cpp and others carry local copies.
template <typename T>
void UI_DeleteControl(T *& ptControl)
{
    if (ptControl)
    {
        UIID id(ptControl->GetID());
        UI_DeleteControl(id, TRUE);
        ptControl = NULL;
    }
}
