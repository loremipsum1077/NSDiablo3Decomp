// X/Client/UserInterface/Interface/UIHero.cpp -- the shared "who killed this hero" text builder.
// `--gc-sections` left exactly one body of this TU in the build, so the file is `GetKillerName`
// and nothing else.
#include "UIHero.h"

#include "../../../Common/Definitions/ActorDefinition.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Gizmo.h"
#include "../../../Common/GizmoSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Monsters.h"
#include "../../../Common/Player.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/ActorCommonData.cpp:8165
ActorType ActorGetType(const SNOName & tSNOName);
// X/Client/CPlayer.cpp:1686
ACDID LocalPlayerACDGet();
// X/Client/CPlayer.cpp:1225
CRefString CPlayerGetServiceName(const Player & tPlayer, BOOL fDisplayPound);
// X/Common/GameTemplate.cpp:255
BOOL GameTypeIsPvP();
// X/Common/UserInterface/UISkillsCommon.cpp:21
namespace UISkills {
BOOL GetName(SNO snoPower, CRefString & sSkillName);
}  // namespace UISkills

namespace UIHero {

// UIHero.cpp:1087.  `strKiller` is the killer's display name and `strPower` the thing it killed
// with; both are cleared first, so a caller that gets `KILLED_BY_MYSTERY` back also gets two
// empty strings.
KilledByType GetKillerName(int32 nKillerPlayerIndex, SNO snoActor, SNO snoPowerDmgSource,
                           MonsterRarity eMonsterRarity, const GBID * arKillerRareNameGBIDs,
                           CRefString & strKiller, CRefString & strPower)
{
    strKiller = "";
    strPower = "";

    KilledByType eType = KILLED_BY_MYSTERY;
    SNOName tSNOName(ACTOR_SNO, snoActor);
    ActorType eActorType = ACTORTYPE_INVALID;
    if (snoActor != SNO_NONE)
    {
        eActorType = ActorGetType(tSNOName);
    }

    // `PlayerGlobals::tPlayerArray` is four records long, so a killer player index outside
    // [0, 4) is "not a player".
    Player * ptKillerPlayer = NULL;
    BOOL bDueling = FALSE;
    if (nKillerPlayerIndex >= 0 && nKillerPlayerIndex < 4)
    {
        ptKillerPlayer = PlayerGetByPlayerIndex(nKillerPlayerIndex);

        ActorCommonData * ptACDLocalPlayer = ACDTryToGet(LocalPlayerACDGet());
        bDueling = ptACDLocalPlayer != NULL && ptACDLocalPlayer->IsDueling();
    }

    // A player only counts as the killer in PvP or in a duel; outside those the actor SNO is
    // what names the death.
    if (ptKillerPlayer != NULL && (GameTypeIsPvP() || bDueling))
    {
        strKiller = CPlayerGetServiceName(*ptKillerPlayer, FALSE);

        if (snoActor == SNO_NONE)
        {
            if (snoPowerDmgSource != SNO_NONE)
            {
                UISkills::GetName(snoPowerDmgSource, strPower);
            }
            return KILLED_BY_PLAYER;
        }

        // The blow came from something the player summoned.
        strPower = MonsterGetName(tSNOName, arKillerRareNameGBIDs, MR_NONE, TRUE);
        return KILLED_BY_PLAYER_PET;
    }

    if (eActorType == ACTORTYPE_MONSTER)
    {
        // The generic proxy actor is where deaths with no real killer are reported; it has no
        // name worth showing.
        if (GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY) != snoActor)
        {
            strKiller = MonsterGetName(tSNOName, arKillerRareNameGBIDs, eMonsterRarity, TRUE);

            switch (eMonsterRarity)
            {
            case MR_RARE:
            case MR_UNIQUE:
            case MR_BOSS:
                eType = KILLED_BY_RARE;
                break;
            default:
                eType = KILLED_BY_NORMAL;
                break;
            }
        }
    }
    else if (eActorType == ACTORTYPE_GIZMO)
    {
        if (ActorGetGizmoType(snoActor) == ACTORGIZMO_UNDEFINED)
        {
            return KILLED_BY_MYSTERY;
        }

        GizmoGetUINameFromLabel(snoActor, strKiller);

        switch (eMonsterRarity)
        {
        case MR_RARE:
        case MR_UNIQUE:
        case MR_BOSS:
            eType = KILLED_BY_RARE;
            break;
        default:
            eType = KILLED_BY_NORMAL;
            break;
        }
    }

    return eType;
}

}  // namespace UIHero
