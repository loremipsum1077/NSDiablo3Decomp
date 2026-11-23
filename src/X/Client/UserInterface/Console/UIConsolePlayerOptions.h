// Original file: X/Client/UserInterface/Console/UIConsolePlayerOptions.h.
// Subset of the public APIs used by the party display; types follow the DWARF.
#pragma once
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
namespace UIPlayerOptions { namespace Console {
BOOL IsVisible();
void ClosePlayerOptions();
} }

// The player-info hierarchy (UIConsolePlayerOptions.h:24-220 in the original).
namespace Console { namespace Online { struct Friend; } }
namespace Console { namespace Online { struct PartyInfo; } }
namespace Console { namespace Online { struct PartyMember; } }

namespace UIPlayerOptions { namespace Console {
class PlayerInfo {
public:
    PlayerInfo(int32 nLocalUserIndex) : m_nLocalUserIndex(nLocalUserIndex) {}
    virtual ~PlayerInfo() {}
    virtual BOOL CanInviteToGame() const = 0;
    virtual void InviteToGame() = 0;
    virtual BOOL IsInJoinableGame() const = 0;
    virtual BOOL CanJoinGame() const = 0;
    virtual void JoinGame() = 0;
    virtual BOOL CanMute() const = 0;
    virtual void Mute() = 0;
    virtual BOOL CanUnMute() const = 0;
    virtual void UnMute() = 0;
    virtual BOOL CanShowGamerCard() = 0;
    virtual BOOL CanShowHeroProfile() = 0;
    virtual void ShowGamerCard() = 0;
    virtual void DisplayInfo() = 0;
    virtual int32 GetLocalUserIndex() { return m_nLocalUserIndex; }
protected:
    int32 m_nLocalUserIndex;
};

class PartyPlayerInfo : public PlayerInfo {
public:
    PartyPlayerInfo(int32 nLocalUserIndex, ::Console::Online::PartyMember * ptPartyMember);
    virtual BOOL CanInviteToGame() const;
    virtual void InviteToGame();
    virtual BOOL IsInJoinableGame() const;
    virtual BOOL CanJoinGame() const;
    virtual void JoinGame();
    virtual BOOL CanMute() const;
    virtual void Mute();
    virtual BOOL CanUnMute() const;
    virtual void UnMute();
    virtual BOOL CanShowGamerCard();
    virtual BOOL CanShowHeroProfile();
    virtual void ShowGamerCard();
    virtual void DisplayInfo();
private:
    int32 m_nPartyMemberIndex;
    ::Console::Online::PartyMember * m_ptPartyMember;
};

class FriendPlayerInfo : public PlayerInfo {
public:
    FriendPlayerInfo(int32 nLocalUserIndex, ::Console::Online::Friend * ptFriend);
    virtual BOOL CanInviteToGame() const;
    virtual void InviteToGame();
    virtual BOOL IsInJoinableGame() const;
    virtual BOOL CanJoinGame() const;
    virtual void JoinGame();
    virtual BOOL CanMute() const;
    virtual void Mute();
    virtual BOOL CanUnMute() const;
    virtual void UnMute();
    virtual BOOL CanShowGamerCard();
    virtual BOOL CanShowHeroProfile();
    virtual void ShowGamerCard();
    virtual void DisplayInfo();
private:
    ::Console::Online::Friend * m_ptFriend;
};

class LocalCommPlayerInfo : public PlayerInfo {
public:
    LocalCommPlayerInfo(int32 nLocalUserIndex, int32 nRemotePlayerIndex, ::Console::Online::PartyInfo * ptParty);
    virtual BOOL CanInviteToGame() const { return FALSE; }
    virtual void InviteToGame() {}
    virtual BOOL IsInJoinableGame() const { return TRUE; }
    virtual BOOL CanJoinGame() const { return TRUE; }
    virtual void JoinGame();
    virtual BOOL CanMute() const { return FALSE; }
    virtual void Mute() {}
    virtual BOOL CanUnMute() const { return FALSE; }
    virtual void UnMute() {}
    virtual BOOL CanShowGamerCard() { return FALSE; }
    virtual BOOL CanShowHeroProfile() { return FALSE; }
    virtual void ShowGamerCard() {}
    virtual void DisplayInfo();
private:
    int32 m_nRemotePlayerIndex;
    ::Console::Online::PartyInfo * m_ptPartyInfo;
};
} }

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(UIPlayerOptions::Console::PlayerInfo) == 0x10, "UIPlayerOptions::Console::PlayerInfo");
static_assert(sizeof(UIPlayerOptions::Console::PartyPlayerInfo) == 0x18, "UIPlayerOptions::Console::PartyPlayerInfo");
static_assert(sizeof(UIPlayerOptions::Console::FriendPlayerInfo) == 0x18, "UIPlayerOptions::Console::FriendPlayerInfo");
static_assert(sizeof(UIPlayerOptions::Console::LocalCommPlayerInfo) == 0x18, "UIPlayerOptions::Console::LocalCommPlayerInfo");
