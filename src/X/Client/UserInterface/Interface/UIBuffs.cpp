#include "UIBuffs.h"
#include "UIMinimap.h"
#include "../../../Common/Definitions/SkillKitDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/GameTemplate.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../CGame.h"

#include "../Controls/UIWindowControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../UI.h"
#include "../../../Common/Player.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/Powers/PowersCommon.h"
#include "../../../Common/TagMaps/PowerTags.h"
#include "../../../Common/TagMaps/Tags.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/UserInterface/UIToolTips.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../Battlenet/UI/Util/UIUtil.h"
#include "../../../../Contrib/Contrib/blz/include/blz/func_poly.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"

extern XMemoryAllocatorInterface * AppGetCurrentMemoryInterface(); // X/Common/Game.cpp

// X/Client/UserInterface/Console/UIConsoleHUD.cpp; owner header not adopted.
namespace UIHUD { namespace Console {
void ClearAllBuffs();
void GetBuffDisplayInfoForPlayer(const Player *, UIID &, UIID &, UIID &);
void OnRemoveBuff(SNO, int32, const Player *);
} }
// X/Common/ActorCommonData.cpp
ACDNetworkName ANNFromACD(ACDID idACD);
// X/Common/Powers/PowersCommon.h
void PowerGetBuffAttributes(int32, Attrib &, Attrib &, Attrib &);
BOOL PowerBuffIsHarmful(AutoPowerDefinitionR, int32);
BOOL PowerBuffIsDisplayed(AutoPowerDefinitionR, int32);
BOOL PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR, int32);
// X/Common/Powers/PowersCommon.cpp:805, :1035
BOOL PowerBuffGetMergeTooltipIndex(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);
BOOL PowerBuffIconDisplayDuration(AutoPowerDefinitionR pdefPower, int32 nBuffIndex);
// X/Common/Player.h, X/Common/Game.h
BOOL PlayerIsLocal(const Player *);
BOOL GameIsParamFlagSet(GameParamFlags);
// X/Client/UserInterface/Console/UIConsoleEventQueue.cpp
namespace UIEventQueue { namespace Console { void ReportBuff(const Player *, const CRefString &, const ImageHandle &, BOOL); } }
// X/Client/CPlayer.cpp
ACDID LocalPlayerACDGet();
const Player * LocalPlayerGet();
// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
namespace UIBuffs {
// Defined below (DWARF UIBuffs.cpp:2364); no header declaration in src.
void EnumBuffsOnSkillButton(SNO snoPower, blz::function<void(SortedBuff &)> fnCallback);
static UIBuffGlobals * sgptUIBuffGlobals;
static void sDeleteBuff(SortedBuff & tBuff);

UIBuffGlobals * GetBuffGlobals() { return sgptUIBuffGlobals; }

void InitializeForGame() {
    sgptUIBuffGlobals = (UIBuffGlobals *)SigmaMemoryNew(sizeof(UIBuffGlobals), 4, NULL, TRUE);
    sgptUIBuffGlobals->tSortedBuffPool.Initialize();
    sgptUIBuffGlobals->tSortedBuffPool.SetMemoryInterface(AppGetCurrentMemoryInterface());
    sgptUIBuffGlobals->listBeneficialBuffs.Initialize(&sgptUIBuffGlobals->tSortedBuffPool);
    sgptUIBuffGlobals->listHarmfulBuffs.Initialize(&sgptUIBuffGlobals->tSortedBuffPool);
    sgptUIBuffGlobals->listPartyBuffs.Initialize(&sgptUIBuffGlobals->tSortedBuffPool);
    sgptUIBuffGlobals->listMergeTooltipBuffs.Initialize(&sgptUIBuffGlobals->tSortedBuffPool);
    sgptUIBuffGlobals->listSkillButtonBuffs.Initialize(&sgptUIBuffGlobals->tSortedBuffPool);
    sgptUIBuffGlobals->idBuffHolderPlaceholderBuff = GetUIIDNone();
}

void DisposeForGame() {
    if (!sgptUIBuffGlobals) return;
    sgptUIBuffGlobals->idBuffHolderPlaceholderBuff = GetUIIDNone();
    sgptUIBuffGlobals->listHarmfulBuffs.Dispose();
    sgptUIBuffGlobals->listBeneficialBuffs.Dispose();
    sgptUIBuffGlobals->listPartyBuffs.Dispose();
    sgptUIBuffGlobals->listMergeTooltipBuffs.Dispose();
    sgptUIBuffGlobals->listSkillButtonBuffs.Dispose();
    sgptUIBuffGlobals->tSortedBuffPool.Dispose();
    UIHUD::Console::ClearAllBuffs();
    SigmaMemoryFree(sgptUIBuffGlobals, NULL);
    sgptUIBuffGlobals = NULL;
}

static void sGetBuffParams(const BOOL fIsDebuff, const Player * ptPlayer, UIID & idWindowBase, Vector2D & vecIcon) {
    UIID idBuffRegion, idBuffPositive, idBuffNegative;
    UIHUD::Console::GetBuffDisplayInfoForPlayer(ptPlayer, idBuffRegion, idBuffPositive, idBuffNegative);
    idWindowBase = fIsDebuff ? idBuffNegative : idBuffPositive;
}

BOOL sCanPlayerCast(SNO snoPower) {
    ActorCommonData * ptACDPlayer = ACDTryToGet(LocalPlayerACDGet());
    if (!ptACDPlayer) return FALSE;
    SNO snoSkillkit = ptACDPlayer->GetSkillKit();
    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillkit);
    int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    for (int32 i = 0; i < nCount; ++i) {
        const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        if (tEntry.snoPower == snoPower) return TRUE;
    }
    int32 nTraitCount = pdefSkillKit->serTraitEntries.dwSize / sizeof(TraitEntry);
    for (int32 i = 0; i < nTraitCount; ++i) {
        const TraitEntry & tEntry = pdefSkillKit->arTraitEntries[i];
        if (tEntry.snoPower == snoPower) return TRUE;
    }
    return FALSE;
}
// LINKER-REMOVED: DWARF size 0x88; address-0 rows 218:14/26/30/9 = `if (tACD.IsDueling() && !PlayerIsLocal(ptPlayer))` (IsDueling call col 14, && col 26, PlayerIsLocal col 30), returns at 224.
BOOL sShouldShowBuffOnPlayer(const Player * ptPlayer) {
    if (!ptPlayer) return FALSE;
    const ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);
    if (tACD.IsDueling() && !PlayerIsLocal(ptPlayer)) return FALSE;
    return TRUE;
}
ImageHandle SortedBuff::GetImage() const {
    int32 nTag;
    switch (nBuffIdx) {
    case 0: nTag = TAG_POWER_BUFF_0_ICON; break;
    case 1: nTag = TAG_POWER_BUFF_1_ICON; break;
    case 2: nTag = TAG_POWER_BUFF_2_ICON; break;
    case 3: nTag = TAG_POWER_BUFF_3_ICON; break;
    case 4: nTag = TAG_POWER_BUFF_4_ICON; break;
    case 5: nTag = TAG_POWER_BUFF_5_ICON; break;
    case 6: nTag = TAG_POWER_BUFF_6_ICON; break;
    case 7: nTag = TAG_POWER_BUFF_7_ICON; break;
    case 8: nTag = TAG_POWER_BUFF_8_ICON; break;
    case 9: nTag = TAG_POWER_BUFF_9_ICON; break;
    case 10: nTag = TAG_POWER_BUFF_10_ICON; break;
    case 11: nTag = TAG_POWER_BUFF_11_ICON; break;
    case 12: nTag = TAG_POWER_BUFF_12_ICON; break;
    case 13: nTag = TAG_POWER_BUFF_13_ICON; break;
    case 14: nTag = TAG_POWER_BUFF_14_ICON; break;
    case 15: nTag = TAG_POWER_BUFF_15_ICON; break;
    case 16: nTag = TAG_POWER_BUFF_16_ICON; break;
    case 17: nTag = TAG_POWER_BUFF_17_ICON; break;
    case 18: nTag = TAG_POWER_BUFF_18_ICON; break;
    case 19: nTag = TAG_POWER_BUFF_19_ICON; break;
    case 20: nTag = TAG_POWER_BUFF_20_ICON; break;
    case 21: nTag = TAG_POWER_BUFF_21_ICON; break;
    case 22: nTag = TAG_POWER_BUFF_22_ICON; break;
    case 23: nTag = TAG_POWER_BUFF_23_ICON; break;
    case 24: nTag = TAG_POWER_BUFF_24_ICON; break;
    case 25: nTag = TAG_POWER_BUFF_25_ICON; break;
    case 26: nTag = TAG_POWER_BUFF_26_ICON; break;
    case 27: nTag = TAG_POWER_BUFF_27_ICON; break;
    case 28: nTag = TAG_POWER_BUFF_28_ICON; break;
    case 29: nTag = TAG_POWER_BUFF_29_ICON; break;
    case 30: nTag = TAG_POWER_BUFF_30_ICON; break;
    case 31: nTag = TAG_POWER_BUFF_31_ICON; break;
    default: nTag = -1; break;
    }
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    ImageHandle hImage = 0;
    if (nTag != -1) hImage = TagMapGetInt(pdefPower->tPower.hGeneralTagMap, nTag);
    if (!hImage) hImage = TagMapGetInt(pdefPower->tPower.hGeneralTagMap, TAG_POWER_ICON_NORMAL);
    return hImage;
}

inline int32 SortedBuff::GetCanCancelTag() const {
    switch (nBuffIdx) {
    case 0: return TAG_POWER_BUFF_0_PLAYER_CAN_CANCEL;
    case 1: return TAG_POWER_BUFF_1_PLAYER_CAN_CANCEL;
    case 2: return TAG_POWER_BUFF_2_PLAYER_CAN_CANCEL;
    case 3: return TAG_POWER_BUFF_3_PLAYER_CAN_CANCEL;
    case 4: return TAG_POWER_BUFF_4_PLAYER_CAN_CANCEL;
    case 5: return TAG_POWER_BUFF_5_PLAYER_CAN_CANCEL;
    case 6: return TAG_POWER_BUFF_6_PLAYER_CAN_CANCEL;
    case 7: return TAG_POWER_BUFF_7_PLAYER_CAN_CANCEL;
    case 8: return TAG_POWER_BUFF_8_PLAYER_CAN_CANCEL;
    case 9: return TAG_POWER_BUFF_9_PLAYER_CAN_CANCEL;
    case 10: return TAG_POWER_BUFF_10_PLAYER_CAN_CANCEL;
    case 11: return TAG_POWER_BUFF_11_PLAYER_CAN_CANCEL;
    case 12: return TAG_POWER_BUFF_12_PLAYER_CAN_CANCEL;
    case 13: return TAG_POWER_BUFF_13_PLAYER_CAN_CANCEL;
    case 14: return TAG_POWER_BUFF_14_PLAYER_CAN_CANCEL;
    case 15: return TAG_POWER_BUFF_15_PLAYER_CAN_CANCEL;
    case 16: return TAG_POWER_BUFF_16_PLAYER_CAN_CANCEL;
    case 17: return TAG_POWER_BUFF_17_PLAYER_CAN_CANCEL;
    case 18: return TAG_POWER_BUFF_18_PLAYER_CAN_CANCEL;
    case 19: return TAG_POWER_BUFF_19_PLAYER_CAN_CANCEL;
    case 20: return TAG_POWER_BUFF_20_PLAYER_CAN_CANCEL;
    case 21: return TAG_POWER_BUFF_21_PLAYER_CAN_CANCEL;
    case 22: return TAG_POWER_BUFF_22_PLAYER_CAN_CANCEL;
    case 23: return TAG_POWER_BUFF_23_PLAYER_CAN_CANCEL;
    case 24: return TAG_POWER_BUFF_24_PLAYER_CAN_CANCEL;
    case 25: return TAG_POWER_BUFF_25_PLAYER_CAN_CANCEL;
    case 26: return TAG_POWER_BUFF_26_PLAYER_CAN_CANCEL;
    case 27: return TAG_POWER_BUFF_27_PLAYER_CAN_CANCEL;
    case 28: return TAG_POWER_BUFF_28_PLAYER_CAN_CANCEL;
    case 29: return TAG_POWER_BUFF_29_PLAYER_CAN_CANCEL;
    case 30: return TAG_POWER_BUFF_30_PLAYER_CAN_CANCEL;
    case 31: return TAG_POWER_BUFF_31_PLAYER_CAN_CANCEL;
    default: return -1;
    }
}

static UIID sCreateBuffControls(const SortedBuff & tBuff, const UIID & idBuffParent,
        const Vector2D & vecTL, const Vector2D & vecDim, BOOL bInBuffHolder, UIFunctionPtr pfnToolTip) {
    // The original console build only constructs the window parameters here;
    // the control-creation path is absent in its machine code.
    UIID id = GetUIIDNone();
    ACDNetworkName ann = ANNFromACD(ACDGet(tBuff.ptPlayer->idPlayerACD).id);
    WindowCreationParams tDlgParams(UIT_WINDOW, CRefString().Format("buff %s:%d:%d:%d dlg", SNOToString(POWER_SNO, tBuff.snoPower, FALSE).str(), tBuff.nBuffIdx, ann, tBuff.fIsDebuff), idBuffParent);
    return id;
}

void SortedBuff::Initialize(const SNO _snoPower, const int32 _nBuffIdx, const BOOL _fIsDebuff,
        const BOOL _fIsHidden, const int32 _nStackCount, const Player * _ptPlayer, const BOOL bCreateControls) {
    snoPower = _snoPower;
    nBuffIdx = _nBuffIdx;
    ptPlayer = _ptPlayer;
    nDuration = -1;
    nStackCount = _nStackCount;
    bPlayerCanCancel = FALSE;
    fIsDebuff = _fIsDebuff;
    fIsHidden = _fIsHidden;
    if (!bCreateControls) return;
    UIID idWindowBase;
    Vector2D vecIcon;
    sGetBuffParams(_fIsDebuff, _ptPlayer, idWindowBase, vecIcon);
    if (PowerBuffMergesTooltip(AutoPowerDefinitionR(_snoPower), _nBuffIdx)) return;
    UIID idBuffParent, idBuffRegion, idBuffPositive, idBuffNegative, idContainer;
    UIHUD::Console::GetBuffDisplayInfoForPlayer(_ptPlayer, idBuffRegion, idBuffPositive, idBuffNegative);
    idBuffParent = idBuffRegion;
    int32 nTagPlayerCanCancel = GetCanCancelTag();
    if (nTagPlayerCanCancel != -1) {
        AutoPowerDefinitionR pdefPower(_snoPower);
        bPlayerCanCancel = PowerGeneralTagGetBool(pdefPower, nTagPlayerCanCancel);
    }
    Vector2D vecTL, vecDim;
    sCreateBuffControls(*this, idBuffParent, vecTL, vecDim, TRUE, NULL);
}

static void sUpdateTimersInList(XPooledSortedBuffList * plistBuffs) {
    POS pos = plistBuffs->GetHeadPosition();
    while (pos) {
        SortedBuff & tBuff = plistBuffs->GetNext(pos);
        if (!tBuff.ptPlayer) continue;
        ActorCommonData * ptACDEffected = ACDTryToGet(tBuff.ptPlayer->idPlayerACD);
        if (!ptACDEffected) continue;
        Attrib eAttribStart, eAttribEnd, eAttribCount;
        PowerGetBuffAttributes(tBuff.nBuffIdx, eAttribStart, eAttribEnd, eAttribCount);
        uint32 dwEndTick = ptACDEffected->AttributesGetInt(MakeAttribKey(eAttribEnd, tBuff.snoPower));
        // Local name invented for the original attribute-existence result.
        BOOL bHasEndTick = ptACDEffected->AttributesAttributeExists(MakeAttribKey(eAttribEnd, tBuff.snoPower));
        uint32 dwNow = CGameGetPredictedServerTime();
        int32 nStackCount = ptACDEffected->AttributesGetInt(MakeAttribKey(eAttribCount, tBuff.snoPower));
        if (nStackCount <= 0 || (bHasEndTick && dwEndTick < dwNow))
            RemoveBuff(tBuff.snoPower, tBuff.nBuffIdx, tBuff.ptPlayer);
    }
}

static void sCleanUpBuffIconsForSkillButtonsInList(XPooledSortedBuffList & listBuffs) {
    POS pos = listBuffs.GetHeadPosition();
    while (pos) {
        POS posOld = pos;
        SortedBuff & tBuff = listBuffs.GetNext(pos);
        if (PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR(tBuff.snoPower), tBuff.nBuffIdx) &&
            BuffIsAssociatedWithSkillButton(tBuff.snoPower, LocalPlayerGet())) {
            sDeleteBuff(tBuff);
            listBuffs.RemoveAt(posOld);
        }
    }
}

void CleanUpBuffIconsForSkillButtons() { sCleanUpBuffIconsForSkillButtonsInList(sgptUIBuffGlobals->listBeneficialBuffs); }

static CRefString sGetBuffStringListLabel(SNO snoPower, int32 nBuffIndex) {
    CRefString sPower = SNOToString(POWER_SNO, snoPower, FALSE);
    CRefString sIndex;
    sIndex.Format("_%d", nBuffIndex);
    return sPower + sIndex;
}

// DWARF UIBuffs.cpp:1241; inlined into GetBuffTooltipText.
static CRefString sGetBuffStringListLabel(const SortedBuff & tBuff) {
    return sGetBuffStringListLabel(tBuff.snoPower, tBuff.nBuffIdx);
}

CRefString GetBuffName(SNO snoPower, int32 nBuffIndex) {
    SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_BUFFS);
    CRefString sStringListName = sGetBuffStringListLabel(snoPower, nBuffIndex);
    CRefString sName = StringListFetch(snoStringList, sStringListName);
    CRefString sNameAfterLanguageRules(LanguageRules::ApplyLanguageRules(sName.str()));
    if (PowerBuffIsHarmful(AutoPowerDefinitionR(snoPower), nBuffIndex))
        Set_Text_Color(sNameAfterLanguageRules, FONTCOLOR_RED);
    else
        Set_Text_Color(sNameAfterLanguageRules, FONTCOLOR_BLUE);
    return sNameAfterLanguageRules;
}

BOOL sProcessBuffTag(const ActorCommonData * ptACD, AttribGroupID, AttribGroupID, LPCSTR, float &, float *,
        CRefString & sResult, SNO, int32, void * pData, void *, FontSpecifier &) {
    SortedBuff * ptBuff = (SortedBuff *)pData;
    if (ptBuff && ptBuff->snoPower == GlobalSNOGet(GLOBALSNO_POWERS_CONSUMABLE_POTION_BUFFS)) {
        // Initial value -1 (folded local, DWARF line 1294); ATTRIBUTE_NONE is the enumerator for -1.
        Attrib eAttrib = ATTRIBUTE_NONE;
        switch (ptBuff->nBuffIdx) {
        case 1: eAttrib = ATTRIBUTE_XP_POTION_BUFF_EXPIRATION; break;
        case 2: eAttrib = ATTRIBUTE_GOLD_FIND_POTION_BUFF_EXPIRATION; break;
        case 3: eAttrib = ATTRIBUTE_BLOODSHARD_POTION_BUFF_EXPIRATION; break;
        default: break;
        }
        // Folded away in the original (eAttrib is a known constant on every path).
        if (eAttrib != ATTRIBUTE_NONE) {
            uint32 nNow = Blizzard::Time::ToUnixTime(Blizzard::Time::GetTimestamp());
            uint32 nExpiration = ptACD->AttributesGetInt(eAttrib);
            if (nExpiration <= nNow) {
                // The original builds a CRefString temporary from "0" and copy-assigns it.
                sResult = CRefString("0");
            } else {
                ElapsedTimeDisplayOptions tElapsedTimeDisplayOptions;
                tElapsedTimeDisplayOptions.fRoundDown = TRUE;
                sResult = DisplayElapsedTime(Blizzard::Time::GetTimestamp(), Blizzard::Time::FromUnixTime(nExpiration), tElapsedTimeDisplayOptions);
            }
            return TRUE;
        }
    }
    return FALSE;
}

CRefString GetBuffTooltipText(const SortedBuff & tBuff) {
    if (!tBuff.ptPlayer) return "";
    ActorCommonData * ptACD = ACDTryToGet(tBuff.ptPlayer->idPlayerACD);
    if (!ptACD) return "";
    CRefString sNameAfterLanguageRules = GetBuffName(tBuff.snoPower, tBuff.nBuffIdx);
    if (PowerBuffMergesTooltip(AutoPowerDefinitionR(tBuff.snoPower), tBuff.nBuffIdx))
        sNameAfterLanguageRules = "";
    CRefString sStringListName = sGetBuffStringListLabel(tBuff);
    sStringListName += "_desc";
    SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_BUFFS);
    // Five rune types (A..E); the original unrolls the loop five times. Literal bound inferred.
    for (int32 i = 0; i < 5; ++i) {
        Attrib eRuneAttrib = PowerRuneTypeAndBuffIndexToAttribute(tBuff.snoPower, i, tBuff.nBuffIdx);
        if (eRuneAttrib != ATTRIBUTE_NONE && ptACD->AttributesGetInt(MakeAttribKey(eRuneAttrib, tBuff.snoPower))) {
            CRefString sRuneDescriptionName = sStringListName;
            sRuneDescriptionName += "_rune_";
            sRuneDescriptionName += CRefString().Format("%c", 'A' + i);
            CRefString sResult;
            if (StringListFetch(snoStringList, sRuneDescriptionName, sResult) && sResult.Length()) {
                sStringListName = sRuneDescriptionName;
                break;
            }
        }
    }
    CRefString strDesc = StringListFetch(snoStringList, sStringListName);
    Attrib eAttribStart;
    Attrib eAttribEnd;
    Attrib eAttribCount;
    PowerGetBuffAttributes(tBuff.nBuffIdx, eAttribStart, eAttribEnd, eAttribCount);
    int32 nCount = ptACD->AttributesGetInt(MakeAttribKey(eAttribCount, tBuff.snoPower));
    strDesc = UIToolTips_ProcessTip(ptACD, ptACD->AttributesGetAttributeGroup(), ATTRIBGROUPID_INVALID, SNO_NONE, nCount, strDesc, TTVC_NORMAL_COLORING, sProcessBuffTag, (void *)&tBuff, NULL);
    POS pos = sgptUIBuffGlobals->listMergeTooltipBuffs.GetHeadPosition();
    while (pos) {
        const SortedBuff & tMergedBuff = sgptUIBuffGlobals->listMergeTooltipBuffs.GetNext(pos);
        if (tMergedBuff.snoPower == tBuff.snoPower) {
            if (PowerBuffGetMergeTooltipIndex(AutoPowerDefinitionR(tMergedBuff.snoPower), tMergedBuff.nBuffIdx) == tBuff.nBuffIdx &&
                tMergedBuff.ptPlayer == tBuff.ptPlayer) {
                if (strDesc.Length())
                    strDesc += "\n";
                strDesc += GetBuffTooltipText(tMergedBuff);
            }
        }
    }
    CRefString usDescAfterLanguageRules(LanguageRules::ApplyLanguageRules(strDesc.str()));
    if (sNameAfterLanguageRules.Length())
        sNameAfterLanguageRules += "\n";
    sNameAfterLanguageRules += usDescAfterLanguageRules;
    if (PowerBuffIconDisplayDuration(AutoPowerDefinitionR(tBuff.snoPower), tBuff.nBuffIdx) &&
        ptACD->AttributesAttributeExists(MakeAttribKey(eAttribEnd, tBuff.snoPower)) &&
        !PowerBuffMergesTooltip(AutoPowerDefinitionR(tBuff.snoPower), tBuff.nBuffIdx)) {
        uint32 dwEndTick = ptACD->AttributesGetInt(MakeAttribKey(eAttribEnd, tBuff.snoPower));
        uint32 dwNow = CGameGetPredictedServerTime();
        uint32 dwTimeLeft = 0;
        if (dwEndTick > dwNow)
            dwTimeLeft = dwEndTick - dwNow;
        CRefString strDurationString;
        int32 nTime = 0;
        // 3600 and 60 game ticks: one minute and one second (magic-number divides in the original).
        if (dwTimeLeft >= 3600) {
            strDurationString = StringListFetch(snoStringList, "Duration_Minutes");
            nTime = dwTimeLeft / 3600 + 1;
        } else {
            strDurationString = StringListFetch(snoStringList, "Duration_Seconds");
            if (dwTimeLeft > 0)
                nTime = dwTimeLeft / 60 + 1;
        }
        sNameAfterLanguageRules += "\n";
        CRefString strDuration = LocalizedSprintf(StringListFetch(snoStringList, "DurationFormat"),
            UnicodeText::FormatNumber(nTime, TRUE, FNS_NORMAL, (XLocale)0).str(), strDurationString.str(), NULL);
        sNameAfterLanguageRules += strDuration;
    }
    return sNameAfterLanguageRules;
}

static void sDeleteBuff(SortedBuff & tBuff) {
    UIHUD::Console::OnRemoveBuff(tBuff.snoPower, tBuff.nBuffIdx, tBuff.ptPlayer);
    tBuff.snoPower = SNO_NONE;
    tBuff.nBuffIdx = -1;
    tBuff.ptPlayer = NULL;
    tBuff.nStackCount = -1;
    tBuff.bPlayerCanCancel = FALSE;
}

void Update() { sUpdateTimersInList(&sgptUIBuffGlobals->listSkillButtonBuffs); }

static void sRemoveBuff(SNO snoPower, int32 nBuffIndex, XPooledSortedBuffList * plistBuffs, const Player * ptPlayer) {
    POS pos = plistBuffs->GetHeadPosition();
    while (pos) {
        POS posOld = pos;
        SortedBuff & tBuff = plistBuffs->GetNext(pos);
        if (tBuff.snoPower == snoPower && tBuff.ptPlayer == ptPlayer && tBuff.nBuffIdx == nBuffIndex) {
            sDeleteBuff(tBuff);
            plistBuffs->RemoveAt(posOld);
            break;
        }
    }
}

static SortedBuff * sGetBuffInList(XPooledSortedBuffList * plistBuffs, SNO snoPower, int32 nBuffIndex, const Player * ptPlayer, const BOOL fIgnoreIndex) {
    for (POS pos = plistBuffs->GetHeadPosition(); pos;) {
        SortedBuff & tBuff = plistBuffs->GetNext(pos);
        if (tBuff.snoPower == snoPower && tBuff.ptPlayer == ptPlayer && (fIgnoreIndex || tBuff.nBuffIdx == nBuffIndex))
            return &tBuff;
    }
    return NULL;
}

static SortedBuff * sBuffExists(SNO snoPower, int32 nBuffIndex, const Player * ptPlayer, const BOOL fIgnoreIndex) {
    SortedBuff * ptBuff;
    if ((ptBuff = sGetBuffInList(&sgptUIBuffGlobals->listBeneficialBuffs, snoPower, nBuffIndex, ptPlayer, fIgnoreIndex))) return ptBuff;
    if ((ptBuff = sGetBuffInList(&sgptUIBuffGlobals->listHarmfulBuffs, snoPower, nBuffIndex, ptPlayer, fIgnoreIndex))) return ptBuff;
    if ((ptBuff = sGetBuffInList(&sgptUIBuffGlobals->listMergeTooltipBuffs, snoPower, nBuffIndex, ptPlayer, fIgnoreIndex))) return ptBuff;
    if ((ptBuff = sGetBuffInList(&sgptUIBuffGlobals->listMinimapBuffs, snoPower, nBuffIndex, ptPlayer, fIgnoreIndex))) return ptBuff;
    if ((ptBuff = sGetBuffInList(&sgptUIBuffGlobals->listPartyBuffs, snoPower, nBuffIndex, ptPlayer, fIgnoreIndex))) return ptBuff;
    if ((ptBuff = sGetBuffInList(&sgptUIBuffGlobals->listSkillButtonBuffs, snoPower, nBuffIndex, ptPlayer, fIgnoreIndex))) return ptBuff;
    return NULL;
}

#line 1795
static bool sRefreshBuff(
    XPooledSortedBuffList * plistBuffs,
    SNO snoPower,
    int32 nBuffIndex,
    int32 nStackCount,
    const Player * ptPlayer)
{
    SortedBuff * ptBuff = sGetBuffInList(plistBuffs, snoPower, nBuffIndex, ptPlayer, FALSE);
    if (ptBuff)
    {
        ptBuff->nStackCount = nStackCount;

        int32 nTagPlayerCanCancel = ptBuff->GetCanCancelTag();
        if (nTagPlayerCanCancel != -1)
        {
            ptBuff->bPlayerCanCancel = PowerGeneralTagGetBool(AutoPowerDefinitionR(snoPower), nTagPlayerCanCancel);
        }
        return true;
    }
    return false;
}

// NON_MATCHING: 1356 vs 1360 bytes; the create-controls boolean lowers to
// bic instead of the original eor/and pair. List, ownership and event flow agree.
#line 1821
void AddBuff(
    SNO snoPower,
    int32 nBuffIndex,
    int32 nStackCount,
    const Player * ptPlayer)
{
    XPooledSortedBuffList * plistBuffs;

    BOOL bBuffHasSkillButton = BuffIsAssociatedWithSkillButton(snoPower, ptPlayer);
    BOOL bBuffShowsOnSkillButton = PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR(snoPower), nBuffIndex) || PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR(snoPower), nBuffIndex);

    BOOL bHarmful = PowerBuffIsHarmful(AutoPowerDefinitionR(snoPower), nBuffIndex);
    BOOL bMergesTooltips = PowerBuffMergesTooltip(AutoPowerDefinitionR(snoPower), nBuffIndex);





    BOOL bHiddenInStack = FALSE;
    BOOL bShowInMinimapArea = FALSE; // DWARF local, constant FALSE, no use survives

    BOOL bShowInBeneficialBuffArea = !bHiddenInStack && !(bBuffHasSkillButton && bBuffShowsOnSkillButton);





    if (bMergesTooltips)
    {
        plistBuffs = &sgptUIBuffGlobals->listMergeTooltipBuffs;
    }
    else if (bBuffHasSkillButton && bBuffShowsOnSkillButton)
    {
        plistBuffs = &sgptUIBuffGlobals->listSkillButtonBuffs;
    }








    else if (bHarmful)
    {
		if (!PlayerIsLocal(ptPlayer))
        {
            return;
        }
        plistBuffs = &sgptUIBuffGlobals->listHarmfulBuffs;
    }


    else if (!PlayerIsLocal(ptPlayer) && sCanPlayerCast(snoPower) && sShouldShowBuffOnPlayer(ptPlayer))
    {
        plistBuffs = &sgptUIBuffGlobals->listPartyBuffs;
    } else {
        if (GameIsParamFlagSet(GPF_ARENA_SCOREBOARD))
        {
            return;
        }
        else
        {
            plistBuffs = &sgptUIBuffGlobals->listBeneficialBuffs;
        }
    }





    if (sRefreshBuff(plistBuffs, snoPower, nBuffIndex, nStackCount, ptPlayer))
    {
        return;
    }


    SortedBuff tBuff;
    tBuff.Initialize(snoPower, nBuffIndex, bHarmful, bHiddenInStack, nStackCount, ptPlayer, !bMergesTooltips && bShowInBeneficialBuffArea);


    if (nStackCount == 1 && PlayerIsLocal(ptPlayer))
    {

        if (!sBuffExists(snoPower, nBuffIndex, ptPlayer, FALSE) && !PowerBuffMergesTooltip(AutoPowerDefinitionR(snoPower), nBuffIndex))
        {
            CRefString sBuffName = GetBuffName(tBuff.snoPower, tBuff.nBuffIdx);
            ImageHandle hImage = tBuff.GetImage();
            UIEventQueue::Console::ReportBuff(ptPlayer, sBuffName, hImage, bHarmful);
        }
    }


































    plistBuffs->AddTail_Tracked(tBuff);
}
#line 492
static XPooledSortedBuffList * sGetListContainingBuff(SNO snoPower, int32 nBuffIndex, const Player * ptPlayer) {
    // The original repeats this beneficial-list search (including after party).
    if (sGetBuffInList(&sgptUIBuffGlobals->listBeneficialBuffs, snoPower, nBuffIndex, ptPlayer, FALSE)) return &sgptUIBuffGlobals->listBeneficialBuffs;
    if (sGetBuffInList(&sgptUIBuffGlobals->listMergeTooltipBuffs, snoPower, nBuffIndex, ptPlayer, FALSE)) return &sgptUIBuffGlobals->listMergeTooltipBuffs;
    if (sGetBuffInList(&sgptUIBuffGlobals->listHarmfulBuffs, snoPower, nBuffIndex, ptPlayer, FALSE)) return &sgptUIBuffGlobals->listHarmfulBuffs;
    if (sGetBuffInList(&sgptUIBuffGlobals->listPartyBuffs, snoPower, nBuffIndex, ptPlayer, FALSE)) return &sgptUIBuffGlobals->listPartyBuffs;
    // The original repeats this beneficial-list search (including after party).
    if (sGetBuffInList(&sgptUIBuffGlobals->listBeneficialBuffs, snoPower, nBuffIndex, ptPlayer, FALSE)) return &sgptUIBuffGlobals->listBeneficialBuffs;
    if (sGetBuffInList(&sgptUIBuffGlobals->listSkillButtonBuffs, snoPower, nBuffIndex, ptPlayer, FALSE)) return &sgptUIBuffGlobals->listSkillButtonBuffs;
    return NULL;
}

void RemoveBuff(SNO snoPower, int32 nBuffIndex, const Player * ptPlayer) {
    // Original notifies once here and again in sDeleteBuff when a record exists.
    UIHUD::Console::OnRemoveBuff(snoPower, nBuffIndex, ptPlayer);
    XPooledSortedBuffList * plistBuffs = sGetListContainingBuff(snoPower, nBuffIndex, ptPlayer);
    if (plistBuffs) sRemoveBuff(snoPower, nBuffIndex, plistBuffs, ptPlayer);
}

void PlayerLeftGame(const Player * ptPlayer) {
    if (!ptPlayer || !sgptUIBuffGlobals) return;
    XPooledSortedBuffList * plistBuffs = &sgptUIBuffGlobals->listPartyBuffs;
    POS pos = plistBuffs->GetHeadPosition();
    while (pos) {
        POS posOld = pos;
        SortedBuff & tBuff = plistBuffs->GetNext(pos);
        if (tBuff.ptPlayer == ptPlayer) {
            sDeleteBuff(tBuff);
            plistBuffs->RemoveAt(posOld);
        }
    }
}

void HandleBuffCountAttributeChange(const FastAttribValue tValue, const SNO snoPower, const int32 nBuffIndex, const ActorCommonData & tACD) {
    const Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (!ptPlayer) return;
    BOOL bIsDisplayed = PowerBuffIsDisplayed(AutoPowerDefinitionR(snoPower), nBuffIndex) || UIMinimap::CanShowBuff(snoPower, nBuffIndex);
    BOOL bIsMergedTooltip = PowerBuffMergesTooltip(AutoPowerDefinitionR(snoPower), nBuffIndex);
    BOOL bAppearsOnSkillButton = BuffIsAssociatedWithSkillButton(snoPower, ptPlayer);
    if (bIsDisplayed || bAppearsOnSkillButton || bIsMergedTooltip) {
    int32 nStackCount = tValue.nValue;
    if (nStackCount <= 0) RemoveBuff(snoPower, nBuffIndex, ptPlayer);
    else {
        SortedBuff * ptBuff = sBuffExists(snoPower, nBuffIndex, ptPlayer, FALSE);
        if (!ptBuff) AddBuff(snoPower, nBuffIndex, nStackCount, ptPlayer);
        else if (nStackCount != ptBuff->nStackCount) ptBuff->nStackCount = nStackCount;
    }
    }
}

void UpdateBuffProgressBar(const UIID & idGreenBar, const UIID & idYellowBar, float flPos) {
    const float POS_SCALE = 1.0f / 0.75f;
    const float MAX_POS = 1.0f;
    const float MIN_POS = 0.25f;
    UIProgressBar & tProgressBar = UI_GetProgressBarFromID(idGreenBar);
    UIProgressBar & tYellowBar = UI_GetProgressBarFromID(idYellowBar);
    tProgressBar.SetExtentsAndPosition(0.0f, 1.0f, flPos, -1.0f);
    tProgressBar.SetTransparency(Math::Pin((flPos - MIN_POS) * POS_SCALE, 0.0f, MAX_POS) * 255.0f);
    tYellowBar.SetExtentsAndPosition(0.0f, 1.0f, flPos, -1.0f);
}
bool BuffShowsActiveOnSkillButtons(SNO snoPower, const Player * ptPlayer) {
    if (!ptPlayer) return false;
    bool bIsActive = false;
    EnumBuffsOnSkillButton(snoPower, [ptPlayer, &bIsActive](SortedBuff & tBuff) {
        if (!bIsActive && ptPlayer == tBuff.ptPlayer &&
            PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR(tBuff.snoPower), tBuff.nBuffIdx))
            bIsActive = true;
    });
    return bIsActive;
}

bool BuffIsAssociatedWithSkillButton(const SNO snoPower, const Player * ptPlayer) {
    if (!PlayerIsLocal(ptPlayer)) return false;
    for (int32 i = 0; i < NUM_SKILL_SLOTS; ++i) {
        SNO snoActiveSkill = ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].snoSkill;
        if (snoActiveSkill != SNO_NONE) {
            if (snoActiveSkill == snoPower) return true;
            SNO snoPassive = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoActiveSkill), TAG_POWER_SYNERGY_POWER);
            if (snoPassive != SNO_NONE && snoPassive == snoPower) return true;
        }
    }
    return false;
}

// LINKER-REMOVED: DWARF UIBuffs.cpp:1489; the original's out-of-line copy (0x60 bytes) was removed by the linker.
// Its unused local `POS pos` (line 1492) is left out.
const SortedBuff * sGetBuffByID(const UIID & id) {
    UIID idFirstBuffIcon = UIIDCreateFromParent(sgptUIBuffGlobals->idBuffHolderPlaceholderBuff, ".icon");
    if (idFirstBuffIcon == id) return &sgptUIBuffGlobals->listHiddenBuffs.GetHead();
    return NULL;
}

void OnClickTryToCancelBuff(const UIID & id) {
    if (!UI_TestID(id)) return;
    const SortedBuff * ptBuff = sGetBuffByID(id);
    if (ptBuff && ptBuff->bPlayerCanCancel) {
        RequestBuffCancelMessage tBuffCancelMsg;
        tBuffCancelMsg.nBuffIndex = ptBuff->nBuffIdx;
        tBuffCancelMsg.snoPower = ptBuff->snoPower;
        MessageSendToServer(CMSG_REQUEST_BUFF_CANCEL, &tBuffCancelMsg, sizeof(tBuffCancelMsg), LocalPlayerGet());
    }
}

void HandleBuffEndTickAttributeChange(const FastAttribValue tValue, const Attrib eAttribCount, const SNO snoPower, const int32 nBuffIndex, const ActorCommonData & tACD) {
    const Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (!ptPlayer) return;
    BOOL bIsDisplayed = PowerBuffIsDisplayed(AutoPowerDefinitionR(snoPower), nBuffIndex) || UIMinimap::CanShowBuff(snoPower, nBuffIndex);
    BOOL bAppearsOnSkillButton = BuffIsAssociatedWithSkillButton(snoPower, ptPlayer);
    BOOL bShowsProgressOnSkillButton = PlayerIsLocal(ptPlayer) && PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR(snoPower), nBuffIndex);
    if (!bIsDisplayed && !bAppearsOnSkillButton && !bShowsProgressOnSkillButton) return;
    int32 nStackCount = tACD.AttributesGetInt(MakeAttribKey(eAttribCount, snoPower));
    if (nStackCount > 0) {
        uint32 dwNow = CGameGetPredictedServerTime();
        // Unsigned compare (b.lo) of the end tick against now; the cast is inferred.
        if ((uint32)tValue.nValue >= dwNow) {
            SortedBuff * ptBuff = sBuffExists(snoPower, nBuffIndex, ptPlayer, FALSE);
            if (!ptBuff) AddBuff(snoPower, nBuffIndex, nStackCount, ptPlayer);
        }
    }
}

// LINKER-REMOVED: DWARF UIBuffs.cpp:2301. The original's out-of-line copy (0x1c8 bytes) was removed by the linker;
// it is kept so the lambdas are numbered as in the original ($_1 here, $_2 in GetStackCountForSkillButton).
// Lines 2314-2336 are inferred: DWARF shows size() at 2314, operator[] at 2320, a block with
// ptLongestBuff (2325) and a range-for over ptBuff (2326) with no calls in its body.
SortedBuff * FindBuffWithDurationOnSkillButton(SNO snoPower, const Player * ptPlayer) {
    blz::vector<SortedBuff *> vecBuffsFound;
    EnumBuffsOnSkillButton(snoPower, [ptPlayer, &vecBuffsFound](SortedBuff & tBuff) {
        if (tBuff.ptPlayer == ptPlayer &&
            PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR(tBuff.snoPower), tBuff.nBuffIdx))
            vecBuffsFound.push_back(&tBuff);
    });
    if (vecBuffsFound.size() == 0) return NULL;
    if (vecBuffsFound.size() == 1) return vecBuffsFound[0];
    else {
        SortedBuff * ptLongestBuff = NULL;
        for (SortedBuff * ptBuff : vecBuffsFound) {
            if (!ptLongestBuff || ptBuff->nDuration > ptLongestBuff->nDuration) ptLongestBuff = ptBuff;
        }
        return ptLongestBuff;
    }
}

int32 GetStackCountForSkillButton(SNO snoPower, const Player * ptPlayer) {
    if (!ptPlayer) return 0;
    int32 nHighestStackCount = 0;
    EnumBuffsOnSkillButton(snoPower, [ptPlayer, &nHighestStackCount](SortedBuff & tBuff) {
        if (tBuff.ptPlayer == ptPlayer && tBuff.nStackCount > nHighestStackCount)
            nHighestStackCount = tBuff.nStackCount;
    });
    return nHighestStackCount;
}

void EnumBuffsOnSkillButton(SNO snoPower, blz::function<void(SortedBuff &)> fnCallback) {
    SNO snoSynergyPower = PowerGeneralTagGetSNO(AutoPowerDefinitionR(snoPower), TAG_POWER_SYNERGY_POWER);
    POS pos = sgptUIBuffGlobals->listSkillButtonBuffs.GetHeadPosition();
    while (pos) {
        SortedBuff & tBuff = sgptUIBuffGlobals->listSkillButtonBuffs.GetNext(pos);
        if (tBuff.snoPower == snoPower || tBuff.snoPower == snoSynergyPower)
            fnCallback(tBuff);
    }
}
} // namespace UIBuffs
