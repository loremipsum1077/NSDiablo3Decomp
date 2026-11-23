// X/Client/CPlayer.h -- provisional owning header.
//
// The 2.6.2 DWARF attributes CPlayerAppearanceUpdate and CPlayerPetAppearanceUpdate
// to X/Client/CPlayer.cpp (the relocations reached from CActorCommonData are
// _Z23CPlayerAppearanceUpdate5ACDID and _Z26CPlayerPetAppearanceUpdate5ACDID).
// The full class/skeleton for this header has not been adopted in this checkout, so
// only the two entry points that X/Client/CActorCommonData.cpp:100/:105 needs live
// here. No layouts are declared yet.

#pragma once

#include "../Common/Act.h"
#include "../Common/CommonTypes.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/Math/Vector.h"

struct Player;

// CPlayer.h:30, the per-frame snapshot X/Common/Game.cpp keeps for crash reports.
struct DebugGameState {  // size 0x38
    BOOL bPlayerValid;  // 0x00
    BOOL bGameIsStartUp;  // 0x04
    SNO snoScene;  // 0x08
    SNO snoWorld;  // 0x0c
    SNO snoPlayerActor;  // 0x10
    Act eAct;  // 0x14
    int32 dwSeed;  // 0x18
    SNO snoQuest;  // 0x1c
    int32 nQuestStepUID;  // 0x20
    Vector3D tWorldPos;  // 0x24
    int32 nPlayers;  // 0x30
    int32 nLocalPlayers;  // 0x34
};
static_assert(sizeof(DebugGameState) == 0x38, "DebugGameState");

struct Player;

typedef int32 CalloutEventType;  // as in X/Server/Powers/PowerAPI.h

struct CalloutEvent {  // size 0x24
    CalloutEvent();
    CalloutEvent(const CalloutEventType, const uint32);
    CalloutEventType eType;  // 0x00
    uint32 uTime;  // 0x04
    uint32 uCount;  // 0x08
    uint32 uXPRewarded;  // 0x0c
    BOOL bNewRecord;  // 0x10
    ACDID idACDSavedPlayer;  // 0x14
    float flHealthPercent;  // 0x18
    float flXPMultiplier;  // 0x1c
    uint32 uTotalTime;  // 0x20
};
static_assert(sizeof(CalloutEvent) == 0x24, "CalloutEvent");

typedef XGrowableList<CalloutEvent, const CalloutEvent &> CalloutEventList;

// X/Client/CPlayer.cpp
BOOL PlayerIsLocal(const Player * ptPlayer);  // CPlayer.cpp
BOOL PlayerIsLocal(RActorID idRActor);  // CPlayer.cpp:5241
void CPlayerAppearanceUpdate(const ACDID idACD);
void CPlayerPetAppearanceUpdate(const ACDID idACD);
int32 PlayerLocalIndexToPlayerIndex(const int32 nLocalPlayerIndex);  // CPlayer.cpp:819
void CPlayerInitializeForApp();  // CPlayer.cpp:413
void CPlayerDisposeForApp();  // CPlayer.cpp
Player * LocalPlayerGet();  // CPlayer.cpp:1612
void CPlayerFillDebugGameState(DebugGameState & tGameState);  // CPlayer.cpp:3378

RActorID LocalPlayerRActorGet();  // CPlayer.cpp:1668
BOOL CPlayerCanHighlightItem(const Player * ptPlayer);  // CPlayer.cpp:4862

class CRefString;
struct RActor;
typedef int32 Team;
int32 PlayerIndexToLocalPlayerIndex(const int32 nPlayerIndex);  // CPlayer.cpp:791
ACDID LocalPlayersACDGetByIndex(const int32 nLocalPlayerIndex);  // CPlayer.cpp:844
void CPlayerInitializeForGame();  // CPlayer.cpp:459
void CPlayerHandlePong(uint32 uPingSequence);  // CPlayer.cpp:604
uint32 CPlayerGetLatencyMsec();  // CPlayer.cpp:628
void CPlayerRemovePlayer(const Player & tPlayer);  // CPlayer.cpp:1213
CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound);  // CPlayer.cpp:1225
CRefString CPlayerGetIndicatorString(const Player & tPlayer);  // CPlayer.cpp:1247
CRefString CPlayerGetIndicatorString(int32 nPlayerIndex);  // CPlayer.cpp:1256
void PlayersSetLocalPlayer(const int32 nControllerIndex, const int32 nPlayerIndex, const uint32 dwSyncedAnimSeed);  // CPlayer.cpp:1328
void PlayersAddLocalPlayer(const int32 nControllerIndex, const int32 nPlayerIndex, const uint32 dwSyncedAnimSeed);  // CPlayer.cpp:1358
uint32 PlayerGetLocalPlayerCount();  // CPlayer.cpp:1395
BOOL CPlayerIsValid(const Player * ptPlayer);  // CPlayer.cpp:1404
Team PlayerGetLocalTeam();  // CPlayer.cpp:1587
ACDID LocalPlayerACDGet();  // CPlayer.cpp:1686
void CPlayerAddCalloutEvent(const CalloutEvent & tEvent);  // CPlayer.cpp:2431
void CPlayerMainRActorTeleported();  // CPlayer.cpp:2862
BOOL CPlayerCanUsePortal();  // CPlayer.cpp:2879
void CPlayerBeginInspect(int32 nPlayerIndex, Player * ptPlayer);  // CPlayer.cpp:3058
CRefString CPlayerGetScreenshotComment(BOOL bForInspectorField);  // CPlayer.cpp:3432
void CPlayerTakeScreenshot();  // CPlayer.cpp:3532
void CPlayerSetLastUsedWaypointRActor(RActorID idRActorWaypoint);  // CPlayer.cpp:3543
RActor * CPlayerGetLastUsedWaypointRActor();  // CPlayer.cpp:3552
void CPlayerResetShownItemsOnGroundDuration();  // CPlayer.cpp:4870
SNO CPlayerGetLevelArea(const Player & tPlayer);  // CPlayer.cpp:5074
void CPlayerUpdateServerSidePrefs(Player * ptPlayer);  // CPlayer.cpp:5109
void CPlayerSendPrefsToServer(Player * ptPlayer);  // CPlayer.cpp:5127
BOOL PlayerIsLocal(const ACDID idACD);  // CPlayer.cpp:5216
BOOL PlayerIsLocal(int32 nPlayerIndex);  // CPlayer.cpp:5270
Player * LocalPlayerGetPrimary();  // CPlayer.cpp:5293
Player * LocalPlayerGetValidRActor();  // CPlayer.cpp:5306
void CPlayerSetFriendStatus(Player * ptPlayer);  // CPlayer.cpp:5325
void CPlayerSetAvengerGiftRecipient(CRefString sNameRecipient, uint64 idRecipient);  // CPlayer.cpp:5356
CRefString PlayerGetAvengerGiftRecipientName();  // CPlayer.cpp:5376
BOOL PlayersAreAllLocal();  // CPlayer.cpp:5457
void CPlayerLocalPlayerJoin(int32 nControllerIndex);  // CPlayer.cpp:5475
void CPlayerLocalPlayerLeave(Player * ptPlayer);  // CPlayer.cpp:5502
void CPlayerLocalPlayerLeft(int32 nControllerIndex);  // CPlayer.cpp:5521
int32 CPlayerGetStashLookIndex(const Player & tPlayer);  // CPlayer.cpp:5842

struct BannerRenderData;
void CPlayerUpdateBannerAttachments(RActor & tRActor, const BannerRenderData & tBannerRenderData);  // CPlayer.cpp:5009
