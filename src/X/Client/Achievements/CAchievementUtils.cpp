#include "../../Common/Achievements/AchievementDefines.h"
#include "../../Sigma/RefString.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/SNO/SymbolicValue.h"
#include "../../Common/Definitions/StringListDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Definitions/LevelAreaDefinition.h"
#include "../../Common/Items.h"
#include "../../Common/Player.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/UserInterface/UIInventory.h"
// DWARF ConversationDefinition.cpp:1068,1080; Quests.cpp:931.
void ConversationGetTitleLabel(SNO snoConv, CRefString & sLabel, SNO & snoStringList);
SNO ConversationGetStringListSNO(SNO snoConv);
CRefString QuestsGetUIName(SNO snoQuest);
#include "../../Common/UserInterface/UIHirelingCommon.h" // DWARF return declaration at UIHirelingCommon.cpp:22.
namespace UIHireling {
CRefString GetNameForHirelingClass(const HirelingClass eClass, const BOOL fWithArticle);
}
// DWARF SymbolicValue.cpp:5061.
extern const SymbolicEntry gDifficulty[7];
namespace Achievement {
CRefString sDisplayStringWithMarkupFromOperand(OperandCategoryID eID, uint64 operand, const CRefString & sStringLabel);
CRefString DisplayStringFromOperand(OperandCategoryID eID, uint64 operand, CRefString & sStringLabel);




// DWARF AchievementDefines.h:492, const Event[135]; initializer values decoded from original relocated table.
#line 492 "X/Common/Achievements/AchievementDefines.h"
static const Event gEvents[135] = {
#include "../../Common/generated-game-data/achievement_events.inc"
};
#line 31 "X/Client/Achievements/CAchievementUtils.cpp"
const Event * EventGetByID(EventID eID)
{
    // Inference: original line-36 scan has no bound test; only comparison/backedge. DWARF out-of-line copy is discarded (PC 0, size 36).
    for (int32 i = 0; ; ++i)
    {
        if (gEvents[i].eID == eID)
            return &gEvents[i];
    }
}





// OperandCategoryID is a DWARF int32 typedef; numeric case values follow original dispatch tables.
BOOL TryGetStringListLabelFromOperand(uint64 operand, OperandCategoryID eID, CRefString & sLabel)



{
    sLabel = "";
    switch (eID)
    {
    case 102:
    {
        SNO snoMonster = ActorGetMonsterSNO((SNO)operand);
        if (snoMonster != SNO_NONE)
        {
            sLabel = SNOToString(MONSTER_SNO, snoMonster, FALSE);
        }
        break;
    }
    case 106:
    {

        SNO snoStringList;
        ConversationGetTitleLabel((SNO)operand, sLabel, snoStringList);
        break;
    }

    case 101:
        sLabel = PowerGetNameLabel((SNO)operand);
        break;

    case 108:
        sLabel = SNOToString(ACTOR_SNO, (SNO)operand, FALSE);
        break;

    case 203:
        sLabel = UIInventory::ItemGetLabelForSlot((InventorySlotType)operand, FALSE);
        break;

    case 207:
        sLabel = SymbolToName((int32)operand, gResourceTypes);
        break;

    case 209:
        sLabel = SymbolToName((int32)operand, gCrafterTypes);
        break;

    case 213:
        sLabel = SymbolToName((int32)operand, gDifficulty);
        break;

    case 222:
        sLabel = SymbolToName((int32)operand, gGemTypeNames);
        break;
    }

    return sLabel.Length() != 0;
}



CRefString DisplayStringFromEvent(EventID eID, uint64 operand, CRefString & sStringLabel)



{

    const Event * ptEvent = EventGetByID(eID);
    if (ptEvent)
    {
        return DisplayStringFromOperand(ptEvent->eOperand, operand, sStringLabel);
    }

    return CRefString("");
}


CRefString sDisplayStringWithMarkupFromOperand(OperandCategoryID eID, uint64 operand, const CRefString & sStringLabel)



{
    switch (eID)
    {
    case 101:
    {
        CRefString sDisplayString;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_POWERS), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }



    case 102:
    {
        CRefString sDisplayString;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_MONSTERS), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }




    case 104:
        return QuestsGetUIName((SNO)operand);


    case 1003:
    {
        SNO snoQuest = (SNO)operand >> 8;
        return QuestsGetUIName(snoQuest);
    }

    case 108:
    {
        CRefString sDisplayString;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GIZMOS), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }




    case 109:
        return LevelAreaGetLocalizedName((SNO)operand);


    case 301:
    case 303:
    case 304:
    case 308:
        return ItemGetDisplayName((GBID)operand);



    case 309:
    {
        CRefString sSetGBIDName = GBIDToString((GBID)operand);
        CRefString sSetName;
        return StringListFetch(sSetName.Format("X1_DevilsHand:SetName%s", sSetGBIDName.str()));
    }

    case 203:
    {
        CRefString sDisplayString;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }




    case 204:
        return ItemGetItemQualityDescription((ItemQualityLevel)operand, 0);



    case 207:
    case 213:
    {
        CRefString sDisplayString;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UILAYOUT), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }


    case 208:
        return PlayerGetClassUIName((PlayerClass)operand, TRUE);


    case 209:
    {
        CRefString sDisplayString;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_VENDOR), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }




    case 210:
        return UIHireling::GetNameForHirelingClass((HirelingClass)operand, FALSE);












    case 222:
    {
        CRefString sDisplayString;
        if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }



    case 106:
    {
        CRefString sDisplayString;
        if (StringListFetch(ConversationGetStringListSNO((SNO)operand), sStringLabel, sDisplayString))
            return sDisplayString;
        break;
    }


    }

    return CRefString("");
}


CRefString DisplayStringFromOperand(OperandCategoryID eID, uint64 operand, CRefString & sStringLabel)
// DWARF out-of-line copy discarded at PC 0, size 92; original helper is not declared inline.



{

    if (sStringLabel.Length() == 0)
    {
        TryGetStringListLabelFromOperand(operand, eID, sStringLabel);
    }

    CRefString sDisplayString = sDisplayStringWithMarkupFromOperand(eID, operand, sStringLabel);
    return sDisplayString;
}
}

