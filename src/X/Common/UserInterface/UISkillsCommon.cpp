// X/Common/UserInterface/UISkillsCommon.cpp -- Diablo III 2.6.2 (Switch, clang 5.0.1).
//
// The skill helpers the PC and console screens share:
//   * UISkills::GetName         -- DWARF X/Common/UserInterface/UISkillsCommon.cpp:21
//                                  (96 B @ 0x71005c8240, quality U)
//   * UISkills::ResolvePowerTag -- DWARF X/Common/UserInterface/UISkillsCommon.cpp:32
//                                  (768 B @ 0x71005c82a0, quality U)
//
// Evidence: build/dwarf/headers/X/Common/UserInterface/UISkillsCommon.cpp, the
// line-attributed instruction listings of both functions, the Ghidra decomp of
// the same range and the callee list. The two functions are written from their
// listings; their bytes have not been checked by tools/try_file.py.
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Sigma/RefString.h"
#include "../ActorCommonData.h"
#include "../Definitions/Definitions.h"
#include "../Definitions/PowerDefinition.h"
#include "../Definitions/StringListDefinition.h"
#include "../GameTemplate.h"
#include "../GlobalSNO.h"
#include "../Powers/PowersCommon.h"
#include "../TagMaps/Tags.h"
#include "../../Sigma/DataStructures/XPtr64.h"

// X/Common/Items.h:40 owns FontSpecifier; the items header is not adopted yet,
// so the reference parameter the DWARF names `FontSpecifier &` is spelled here
// with the type its mangled suffix (`Ri`) implies.
typedef int32 FontSpecifier;

namespace UISkills {

// UISkillsCommon.cpp:21
// Returns the localized power name from the POWERS string list.
BOOL GetName(
    SNO snoPower,
    CRefString & sSkillName)
{
    CRefString sSkillLabel = PowerGetNameLabel(snoPower);
    return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_POWERS), sSkillLabel, sSkillName);
}

// UISkillsCommon.cpp:32 -- `szTag` names a power formula tag; the bare name
// (everything before an optional `#<contact frame>`) is looked up in the
// power's general tag map, and when that misses, in the contact tag map the
// `#` index picks. A tag the power does not own is skipped, but in a PvP game
// the matching PvP tag map counts as well.
BOOL ResolvePowerTag(
    const ActorCommonData * ptACD,
    AttribGroupID idFastAttrib,
    AttribGroupID,
    LPCSTR szTag,
    float & flResult,
    float *,
    CRefString &,
    SNO snoPower,
    int32 nLevel,
    void *,
    void *,
    FontSpecifier &)
{
    ScopedPowerDefinitionLockR pdefPower(snoPower);

    // The name proper is everything before the '#', if there is one.
    char szTagName[1024];
    uint32 dwContactFrame = 0;
    LPCSTR szBangPos = strchr(szTag, '#');
    if (szBangPos == NULL)
    {
        Blizzard::String::Copy(szTagName, szTag, sizeof(szTagName));
    }
    else
    {
        size_t nLength = szBangPos - szTag;
        strncpy(szTagName, szTag, nLength);
        szTagName[nLength] = '\0';
        dwContactFrame = atoi(szBangPos + 1);
    }

    CTagMap * pTagMap = pdefPower->tPower.hGeneralTagMap;
    CTagMap * pPVPTagMap = pdefPower->tPower.hPVPGeneralTagMap;
    BOOL bMatch = FALSE;
    BOOL bIsPVP = GameTypeIsPvP();

    POS pos = TagMapGetStartPosition(pTagMap);
    while (pos != NULL)
    {
        uint32 dwTag = TagMapGetNextTag(pTagMap, pos);
        if (!TagMapTagExists(pTagMap, dwTag) &&
            // These maps hold more tags than the power really uses, so most
            // iterations end here; only a tag the power owns takes part in
            // the name comparison. A tag that lives in the PvP general tag
            // map counts as well, but only in a PvP game.
            //
            // (kept on one condition so the two existence tests land on
            // their original source lines)
            !(bIsPVP && TagMapTagExists(pPVPTagMap, dwTag)))
        {
            continue;
        }

        // Tag ids are not unique, so the one the caller named is found by name.
        const AxeTagInfo * pTagInfo = TagMapGetTagInfo(pTagMap, dwTag);
        if (strcasecmp(szTagName, pTagInfo->szName) == 0)
        {
            // The first id whose name matches ends the search.
            flResult = PowerGetFormulaValue(ptACD, pdefPower, dwTag, nLevel, idFastAttrib, dwContactFrame);
            bMatch = TRUE;
            break;
        }
    }
    if (!bMatch)
    {
        pTagMap = pdefPower->tPower.hContactTagMap[dwContactFrame];
        pPVPTagMap = pdefPower->tPower.hPVPContactTagMap[dwContactFrame];

        pos = TagMapGetStartPosition(pTagMap);
        while (pos != NULL)
        {
            uint32 dwTag = TagMapGetNextTag(pTagMap, pos);
            if (!TagMapTagExists(pTagMap, dwTag) &&
                // The rune's contact tag map is walked exactly like the
                // general one: a tag the general maps did not resolve may
                // still be listed here, in the map the '#' frame index
                // picked out, and the name comparison is the same. A tag in
                // the PvP contact map counts in a PvP game too.
                //
                // (see the note on the first loop's condition)
                !(bIsPVP && TagMapTagExists(pPVPTagMap, dwTag)))
            {
                continue;
            }

            // The first id whose name matches ends the search.
            const AxeTagInfo * pTagInfo = TagMapGetTagInfo(pTagMap, dwTag);
            if (strcasecmp(szTagName, pTagInfo->szName) == 0)
            {
                // Identical to the first loop's match action.
                flResult = PowerGetFormulaValue(ptACD, pdefPower, dwTag, nLevel, idFastAttrib, dwContactFrame);
                bMatch = TRUE;
                break;
            }
        }
    }

    return bMatch;
}

}  // namespace UISkills
