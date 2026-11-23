// Adopted from the DWARF reference; hand-maintained.
// Original file: X/Client/UserInterface/Console/UIConsoleUtils.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"

typedef int32 PlayerClass;

typedef int32 CrafterType;

namespace Console {
namespace Online {
typedef int32 PartyOnlineState;

}  // namespace Online
}  // namespace Console
namespace OnlineService {
typedef int32 GameTagId;

}  // namespace OnlineService
struct UIID;
class CRefString;
namespace UIUtils {
namespace Console {
void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta);
void ResetScrollBox(const UIID & idScrollBox);
CRefString DisplayPartyOnlineState(::Console::Online::PartyOnlineState eState, BOOL bEnabled, float flScale);  // UIConsoleUtils.cpp:326
enum {
    CONTROLLER_NONE = -1,
};

enum {
    VOICE_NONE = -1,
    VOICE_TALKING = 0,
    VOICE_CONNECTED = 1,
    VOICE_MUTED = 2,
};

typedef int32 VoiceChatState;

class PartyVoiceChatStateListener {  // size 0x14
public:
    PartyVoiceChatStateListener();
    ~PartyVoiceChatStateListener();
    BOOL IsEnabled() const;
    UIUtils::Console::VoiceChatState GetVoiceChatState(int32 nPartyIndex) const;
    void Enable(BOOL bEnabled);
private:
    void OnEventPartyMemberSpoke(int32 nPartyIndex);
    BOOL m_bEnabled;  // 0x00
    uint32 m_arLastTalkTime[4];  // 0x04
};
static_assert(sizeof(PartyVoiceChatStateListener) == 0x14, "PartyVoiceChatStateListener");

}  // namespace Console
}  // namespace UIUtils

// Functions defined in this file (bodies are not in the DWARF):
//   line    65  BOOL UIUtils::Console::PartyVoiceChatStateListener::IsEnabled() const  [inlined into 1 functions]
