// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIBossElements.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"

struct Player;
struct UIID;
struct BossEncounterMessage;
struct EncounterInviteStateMessage;

namespace UIBossEncounter {
class UIBossWindowBase {  // size 0x10
public:
    // Both windows are file-scope statics with no static initialiser, so the constructor is
    // inline and constant; the binary keeps m_nControllerIndex's -1 in .data.
    UIBossWindowBase() : m_dwStartTime(0), m_dwStopTime(0), m_nControllerIndex(-1), m_dwOpenTime(0) {}
    void OnOpen(Player * ptPlayer);
    void SetTimer(uint32 dwSecondsRemaining);
    BOOL PlayerHasControl(const int32 nControllerIndex) const;
    BOOL DelayHasElapsed() const;
protected:
    uint32 m_dwStartTime;  // 0x00
    uint32 m_dwStopTime;  // 0x04
private:
    int32 m_nControllerIndex;  // 0x08
    uint32 m_dwOpenTime;  // 0x0c
};
static_assert(sizeof(UIBossWindowBase) == 0x10, "UIBossWindowBase");

// The functions UIBossElements.cpp defines for other files. The reference header does not list
// them; they are declared here in the .cpp's line order.
void Enter_Close();
void Enter_Update(const UIID &);
void Enter_Cancel(const UIID &);
void Enter_Accept(const UIID &);
void Enter_Open(Player * ptPlayer, const BossEncounterMessage * ptMsg);
void Enter_OnButton1(const UIID & id, const int32 nControllerIndex);
void Enter_OnButton2(const UIID & id, const int32 nControllerIndex);
void JoinParty_Update(const UIID &);
void JoinParty_Cancel(const UIID &);
void JoinParty_Accept(const UIID &);
void JoinParty_Open(Player * ptPlayer, const BossEncounterMessage * ptMsg);
void JoinParty_OnButton1(const UIID & id, const int32 nControllerIndex);
void JoinParty_OnButton2(const UIID & id, const int32 nControllerIndex);
BOOL IsActive();
void InviteStatusUpdate(const EncounterInviteStateMessage * pState);
void Started();
void Canceled(BOOL bShowError);
void VoteContinue(const UIID &);
void VoteCancel(const UIID &);
void OneDeclined(Player * ptPlayer, int32 nDeclinedPlayerIndex);

}  // namespace UIBossEncounter
