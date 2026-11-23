// X/Client/UserInterface/Console/ConsoleUIRActorManager.cpp

#include "ConsoleUIRActorManager.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../../Sigma/SigmaTypes.h"

// X/Client/UserInterface/UI.h
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);

// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIWND_CONSOLE_UIRACTOR_ROOT;
extern UIID UIRACTOR_CONSOLE_CENTERED;
extern UIID UIRACTOR_CONSOLE_RING;
extern UIID UITXT_CONSOLE_PLAYER_LABEL_CENTERED;
extern UIID UITXT_CONSOLE_PLAYER_LABEL_RING;

namespace Console {
namespace UIRActorManager {

struct ControlIds {  // size 0x10
    UIID idRActor;  // 0x00
    UIID idLabel;  // 0x08
};
static_assert(sizeof(ControlIds) == 0x10, "ControlIds");

class ConsoleUIRActorManager {  // size 0x40
public:
    ControlIds m_atControlIds[NUM_RACTOR_CONTROLS];  // 0x00
    RActorControlType m_eCurrentRActorControl;  // 0x30
    RActorCharacterType m_eRActorCharacterType;  // 0x34
    BOOL m_bActive;  // 0x38
    BOOL m_bRActorIsDirty;  // 0x3c

    ConsoleUIRActorManager()
        : m_eCurrentRActorControl(RACTOR_CONTROL_NONE)
        , m_eRActorCharacterType(RACTOR_CHARACTER_NONE)
        , m_bActive(FALSE)
        , m_bRActorIsDirty(FALSE)
    {
    }

    void Initialize();
    UIID GetCurrentRActorId();
    UIID GetCurrentLabelId();
    void SetCurrentRActorControls(RActorControlType);
};
static_assert(sizeof(ConsoleUIRActorManager) == 0x40, "ConsoleUIRActorManager");

void ConsoleUIRActorManager::Initialize()
{
    m_eCurrentRActorControl = RACTOR_CONTROL_NONE;
    m_eRActorCharacterType = RACTOR_CHARACTER_NONE;

    m_atControlIds[RACTOR_CONTROL_CENTERED].idRActor = UIRACTOR_CONSOLE_CENTERED;
    m_atControlIds[RACTOR_CONTROL_CENTERED].idLabel = UITXT_CONSOLE_PLAYER_LABEL_CENTERED;

    m_atControlIds[RACTOR_CONTROL_RING].idRActor = UIRACTOR_CONSOLE_RING;
    m_atControlIds[RACTOR_CONTROL_RING].idLabel = UITXT_CONSOLE_PLAYER_LABEL_RING;

    for (int32 index = RACTOR_CONTROL_RING; index < NUM_RACTOR_CONTROLS; index++)
    {
        UIID idRActor = m_atControlIds[index].idRActor;
        UI_GetRActorFromID(idRActor).SetActive(FALSE);

        UIID idLabel = m_atControlIds[index].idLabel;
        UI_GetTextFromID(idLabel).SetActive(FALSE);
    }
}

inline UIID ConsoleUIRActorManager::GetCurrentRActorId()
{
    return m_atControlIds[m_eCurrentRActorControl].idRActor;
}

inline UIID ConsoleUIRActorManager::GetCurrentLabelId()
{
    return m_atControlIds[m_eCurrentRActorControl].idLabel;
}

ConsoleUIRActorManager * gptUIRActorManager;

void Show()
{
    if (gptUIRActorManager->m_eCurrentRActorControl != RACTOR_CONTROL_NONE)
    {
        UIWindow & tRoot = UI_GetWindowFromID(UIWND_CONSOLE_UIRACTOR_ROOT);
        UIWindow & tRActorControl = UI_GetWindowFromID(gptUIRActorManager->GetCurrentRActorId());
        UIText & tPlayerLabel = UI_GetTextFromID(gptUIRActorManager->GetCurrentLabelId());

        tRoot.SetActive(TRUE);
        tRActorControl.SetActive(TRUE);
        tPlayerLabel.SetActive(TRUE);
    }
}

void Hide()
{
    if (gptUIRActorManager->m_eCurrentRActorControl != RACTOR_CONTROL_NONE)
    {
        UIWindow & tRoot = UI_GetWindowFromID(UIWND_CONSOLE_UIRACTOR_ROOT);
        UIWindow & tRActorControl = UI_GetWindowFromID(gptUIRActorManager->GetCurrentRActorId());
        UIText & tPlayerLabel = UI_GetTextFromID(gptUIRActorManager->GetCurrentLabelId());

        tRoot.SetActive(FALSE);
        tRActorControl.SetActive(FALSE);
        tPlayerLabel.SetActive(FALSE);
    }
}

static void Cleanup()
{
    if (gptUIRActorManager->m_eCurrentRActorControl != RACTOR_CONTROL_NONE)
    {
        UIRActor & tRActorControl = UI_GetRActorFromID(gptUIRActorManager->GetCurrentRActorId());
        tRActorControl.Cleanup();
    }
}

static BOOL IsActive()
{
    return gptUIRActorManager->m_bActive;
}

void SetActive(BOOL bActive)
{
    gptUIRActorManager->m_bActive = bActive;

    if (bActive)
        Show();
    else
        Hide();
}

void ForceRActorRecreate()
{
    gptUIRActorManager->m_bRActorIsDirty = TRUE;
}

void CloseRActor()
{
    ForceRActorRecreate();

    if (IsActive())
    {
        SetActive(FALSE);
    }

    if (gptUIRActorManager->m_eCurrentRActorControl != RACTOR_CONTROL_NONE)
    {
        UIID idRActor = gptUIRActorManager->GetCurrentRActorId();
        UI_GetRActorFromID(idRActor).Cleanup();
        gptUIRActorManager->m_eCurrentRActorControl = RACTOR_CONTROL_NONE;
    }
}

BOOL ShouldCreateRActor(RActorControlType eRActorControl, RActorCharacterType eRActorCharacterType, BOOL bForceRecreate, BOOL bClean)
{
    if (eRActorControl != gptUIRActorManager->m_eCurrentRActorControl)
    {
        Hide();
        gptUIRActorManager->m_eCurrentRActorControl = eRActorControl;
        Show();
        ForceRActorRecreate();
    }

    if (eRActorCharacterType != gptUIRActorManager->m_eRActorCharacterType)
    {
        gptUIRActorManager->m_eRActorCharacterType = eRActorCharacterType;
        ForceRActorRecreate();
    }

    if (bForceRecreate)
    {
        ForceRActorRecreate();
    }

    if (gptUIRActorManager->m_bRActorIsDirty)
    {
        gptUIRActorManager->m_bRActorIsDirty = FALSE;

        if (bClean)
        {
            Cleanup();
        }

        SetActive(TRUE);
        return TRUE;
    }

    return FALSE;
}

void InitializeForGame()
{
    gptUIRActorManager = new ConsoleUIRActorManager();
    gptUIRActorManager->Initialize();
}

void DisposeForGame()
{
    delete gptUIRActorManager;
    gptUIRActorManager = NULL;
}

}  // namespace UIRActorManager
}  // namespace Console
