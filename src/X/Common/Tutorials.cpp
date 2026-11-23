#include <string.h>
#include "Player.h"
#include "Game.h"
#include "ActorCommonData.h"
#include "SNO/SNOName.h"
#include "SNO/SNOFiles.h"
#include "Networking/GameMessage.h"
#include "Protobuf/GameMessage.pb.h"
#include "../Server/PlayerList.h"
#include "../Sigma/DataStructures/XGrowableMap.h"

void MessageSendToServer(GameMessageType, void*, uint32, const Player*);
BOOL PlayerIsLocal(const Player*);

// BitField.h:49; original inlined caller loads the numbits member at offset8.
inline int Blizzard::Util::BitField::GetNumBits() const  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    return numbits;
}
namespace Tutorial {
typedef XGrowableMap<int, const int&, int, int> SNOIndexMap;
struct TutorialGlobals {
    SNOIndexMap* m_pMap;
    void Init();
    void Dispose();
};
TutorialGlobals sgtTutorialGlobals;
#line 34 "X/Common/Tutorials.cpp"
int32 SortByGUID(const void* pLHS, const void* pRHS)
{
    const uint32* pLHSSno = static_cast<const uint32*>(pLHS);
    const uint32* pRHSSno = static_cast<const uint32*>(pRHS);
    if (*pLHSSno < *pRHSSno) return 1;
    if (*pLHSSno > *pRHSSno) return -1;
    return 0;
}
#line 54 "X/Common/Tutorials.cpp"
void TutorialGlobals::Init()
{
    m_pMap = new SNOIndexMap;
    PooledSNONameList listSNOFiles(GetSnoNameListPool());
    SNOFileEnumerateSNOsForGroup(63, &listSNOFiles);
    listSNOFiles.Sort(SortByGUID);
    int32 nIndex = 0;
    POS pos = listSNOFiles.GetHeadPosition();
    while (pos) {
        SNOName snoName = listSNOFiles.GetNext(pos);
        m_pMap->SetAt_Tracked(snoName.snoHandle, nIndex++);
    }
}
#line 72 "X/Common/Tutorials.cpp"
inline void TutorialGlobals::Dispose()  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    delete m_pMap;
    m_pMap = 0;
}
#line 81 "X/Common/Tutorials.cpp"
void InitForApp()
{
    sgtTutorialGlobals.Init();
}
#line 88 "X/Common/Tutorials.cpp"
void DisposeForApp()
{
    sgtTutorialGlobals.Dispose();
}
#line 95 "X/Common/Tutorials.cpp"
int32 TutorialSNOToBitIndex(const SNO snoTutorial)
{
    int32 nIndex = -1;
    if (SNOFileExists(63, snoTutorial))
        sgtTutorialGlobals.m_pMap->Lookup(snoTutorial, nIndex);
    return nIndex;
}
#line 108 "X/Common/Tutorials.cpp"
void Shown(Player& tPlayer, const SNO snoTutorial)
{
    if (!SNOFileExists(63, snoTutorial)) return;
    int32 nIndex = TutorialSNOToBitIndex(snoTutorial);
    if (!tPlayer.tSeenTutorials.IsSet(nIndex) && GameActiveGameCommonDataIsClient() && !GameIsStartUp()) {
        SNODataMessage tMessage;
        tMessage.snoData = snoTutorial;
        MessageSendToServer(0x17e, &tMessage, sizeof(tMessage), &tPlayer);
    }
    if (GameIsStartUp()) {
        if (nIndex >= 0 && nIndex < tPlayer.tSeenTutorials.GetNumBits())
            tPlayer.tSeenTutorials.Set(nIndex);
    }
    else {
        for (Player* ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer)) {
            if (GameActiveGameCommonDataIsClient() ? PlayerIsLocal(ptPlayer) : PlayersAreLocalToEachOther(*ptPlayer, tPlayer)) {
                if (nIndex >= 0 && nIndex < ptPlayer->tSeenTutorials.GetNumBits())
                    ptPlayer->tSeenTutorials.Set(nIndex);
            }
        }
    }
}
#line 181 "X/Common/Tutorials.cpp"
BOOL PlayerHasSeen(const Player& tPlayer, const SNO snoTutorial)
{
    int32 nIndex = TutorialSNOToBitIndex(snoTutorial);
    if (nIndex > 0 && nIndex < tPlayer.tSeenTutorials.GetNumBits())
        return tPlayer.tSeenTutorials.IsSet(nIndex);
    return FALSE;
}
#line 195 "X/Common/Tutorials.cpp"
void Reset(Player& tPlayer, const SNO snoTutorial)
{
    int32 nIndex = TutorialSNOToBitIndex(snoTutorial);
    if (nIndex > 0 && nIndex < tPlayer.tSeenTutorials.GetNumBits())
        tPlayer.tSeenTutorials.Clear(nIndex);
}
#line 215 "X/Common/Tutorials.cpp"
BOOL SendTutorialToClient(SNO snoTutorial, Player* ptPlayer, const char* szUIAnchorName, const ACDID idACDTarget)
{
    if (snoTutorial == -1 || !ptPlayer) return FALSE;
    if (PlayerHasSeen(*ptPlayer, snoTutorial)) return FALSE;
    D3::GameMessage::TutorialMessage tMessage;
    tMessage.set_tutorial_sno(snoTutorial);
    if (szUIAnchorName && szUIAnchorName[0])
        tMessage.set_ui_anchor_name(szUIAnchorName, strlen(szUIAnchorName));
    if (idACDTarget != ACDID_INVALID)
        tMessage.set_actor_id(ANNFromACD(idACDTarget));
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, *ptPlayer);
    MessageSendToClient(listDestPlayers, 0x23, tMessage);
    Shown(*ptPlayer, snoTutorial);
    return TRUE;
}
}
