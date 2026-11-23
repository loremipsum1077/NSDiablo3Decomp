// X/Server/System/SVoteKick.h -- the DWARF has no header for SVoteKick.cpp (it declares no
// types), so these declarations are reconstructed from the function list in the reference
// X/Server/System/SVoteKick.cpp.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Timers.h"

struct Player;
struct VoteKickMessage;

namespace VoteKick {
namespace Server {

void OnVoteKickTimeOut(TimerID, DataID, void *);
void DoKick();
void TryVoteKick(VoteKickMessage * ptMessage);
BOOL CanVoteKick(const Player * ptInitiatingPlayer, const Player * ptTarget);
void KickRequested(const Player & tPlayerAsking, const Player * ptTarget);
void Cancel();
void VoteNo(Player * ptPlayerVoting);
void OnVoteKickDoKick(TimerID, DataID, void *);
void VoteYes(Player * ptPlayerVoting);
void OnRemovedPlayer(const Player & tPlayer);
void StateDestroyForGame();
void StateCreateForGame();

}  // namespace Server
}  // namespace VoteKick
