// Lambda numbering ($_N) is per translation unit, so this lives apart from
// func_poly.cpp to be the first lambda in its file, as in UIBuffs.cpp.
#include "Contrib/Contrib/blz/include/blz/func_poly.h"

struct Player;

// UIBuffs::BuffShowsActiveOnSkillButtons (UIBuffs.cpp:1656): a lambda
// capturing the player pointer by value and the result flag by reference, wrapped in
// function<void(SortedBuff&)>. Only the layout of the captures is faithful (the
// real lambda tests the buff's power definition); the impl members that do
// not run the lambda must match.
namespace UIBuffs {
struct SortedBuff {
    int snoPower;
    int nBuffIdx;
    const Player* ptPlayer;
    int nDuration;
    int nStackCount;
    int bPlayerCanCancel;
    int fIsDebuff;
    int fIsHidden;
};
void EnumBuffsOnSkillButton(int, blz::function<void(SortedBuff&)>);
bool Matches(int, const SortedBuff&);

bool BuffShowsActiveOnSkillButtons(int snoPower, const Player* ptPlayer) {
    if (!ptPlayer)
        return false;
    bool bIsActive = false;
    EnumBuffsOnSkillButton(snoPower, [ptPlayer, &bIsActive](const SortedBuff& tBuff) {
        if (!bIsActive && tBuff.ptPlayer == ptPlayer && Matches(tBuff.snoPower, tBuff))
            bIsActive = true;
    });
    return bIsActive;
}
}  // namespace UIBuffs
