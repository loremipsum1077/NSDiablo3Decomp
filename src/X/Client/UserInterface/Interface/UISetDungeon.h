// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UISetDungeon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"

struct Player;
struct UIID;
struct SetDungeonDialogMessage;
struct SetDungeonResultsMessage;
struct EncounterInviteStateMessage;

namespace UISetDungeon {
class UISetEncounterWindowBase {  // size 0x10
public:
    UISetEncounterWindowBase();
    void OnOpen(Player *);
    void SetTimer(uint32 dwSecondsRemaining);
    void Tick();
    BOOL PlayerHasControl(const int32 nControllerIndex) const;
    BOOL DelayHasElapsed() const;
protected:
    uint32 m_dwStartTime;  // 0x00
    uint32 m_dwStopTime;  // 0x04
private:
    int32 m_nControllerIndex;  // 0x08
    uint32 m_dwOpenTime;  // 0x0c
};
static_assert(sizeof(UISetEncounterWindowBase) == 0x10, "UISetEncounterWindowBase");

// The functions UISetDungeon.cpp defines for other files. The reference header does not list
// them; they are declared here in the .cpp's line order. StringLabelHandle is spelled uint32
// (X/Common/Definitions/StringListDefinition.h:19) to keep this header light.
void Show(uint32 hLabelDescription, uint32 hLabelTitle, Player * ptPlayer);
void ShowCannotStartMessage(Player * ptPlayer);
void ShowResults(const SetDungeonResultsMessage * ptResultsMsg);
void ResultsClose(const UIID &);
void Complete_OnButton1(const UIID & id, const int32 nControllerIndex);
void InviteStatusUpdate(const EncounterInviteStateMessage * pState);
void Started();
void Canceled(BOOL);
void JoinParty_Open(Player * ptPlayer, const SetDungeonDialogMessage * ptMsg);
void JoinParty_Update(const UIID &);
void JoinParty_Cancel(const UIID &);
void JoinParty_Accept(const UIID &);
void OneDeclined(Player * ptPlayer, int32 nDeclinedPlayerIndex);
void VoteContinue(const UIID &);
void VoteCancel(const UIID &);
void JoinParty_OnButton1(const UIID & id, const int32 nControllerIndex);
void JoinParty_OnButton2(const UIID & id, const int32 nControllerIndex);
BOOL IsActive();
BOOL IsSetDungeonOwner(const Player * ptPlayer);

}  // namespace UISetDungeon
