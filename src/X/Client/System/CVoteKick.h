// X/Client/System/CVoteKick.h -- the DWARF has no header for CVoteKick.cpp (it declares no
// types), so these declarations are reconstructed from the function list in the reference
// X/Client/System/CVoteKick.cpp.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"

struct VoteKickMessage;

namespace VoteKick {
namespace Client {

void DoVoteKick(const VoteKickMessage * ptMessage);
void StartKick(uint32 nPlayerIndex);
void Passed(uint32 nPlayerIndex);
void Failed(uint32 nPlayerIndex);

}  // namespace Client
}  // namespace VoteKick
