#include "Definitions/SoundDefinition.h"
#include "Definitions/SoundBankDefinition.h"
#include "../Common/Definitions/ConversationDefinition.h"
#include "../Common/Definitions/StringListDefinition.h"
#include "../Common/GameSwig.h"
#include "../Sigma/RefString.h"

// Declarations from ConversationDefinition.cpp's DWARF.
typedef int32 ConversationSpeaker;
struct ConversationDefinition;
typedef ScopedDefinitionLockR<ConversationDefinition> ScopedConversationDefinitionLockR;
template <> inline const ConversationDefinition* DefinitionLockR<ConversationDefinition>(SNO sno)
{
    return (const ConversationDefinition*)gptGroupDefConversation->LockR(sno, FALSE);
}
template <> inline void DefinitionUnlockR<ConversationDefinition>(const ConversationDefinition* pdef)
{
    void* lpDef = (void*)pdef;
    gptGroupDefConversation->UnlockR(lpDef);
}
extern LPCSTR gszConvRootLineLabel;
LPCSTR ConversationGetPlayerClassShortString(PlayerClass);
LPCSTR ConversationGetGenderString(ActorGender);
CRefString ConversationGetSpeakerNameForPlayer(PlayerClass, ActorGender);
CRefString ConversationGetSoundName(SNO, LPCSTR);
SNO ConversationSpeakerToSNO(const ConversationDefinition*, ConversationSpeaker);
uint32 StringHashI(LPCSTR);
uint32 ConversationGetSoundLength(SNO, int32);

#line 172 "X/Client/ConversationUtils.cpp"
StringTableEntry* ConversationChooseStringTableEntry(const StringListDefinition* pdefStringList, const uint32 uLineUID, const ConversationSpeaker eLineSpeaker, const PlayerClass eLineClass, const ActorGender eLineGender, const PlayerClass ePlayerClass, const ActorGender ePlayerGender)
{
    PlayerClass eClass = eLineClass;
    ActorGender eGender = eLineGender;
    if (eLineSpeaker == 0) {
        if (eClass == -1) eClass = ePlayerClass;
        if (eGender == -1) eGender = ePlayerGender;
    }
    else if (eGender == -1 && ePlayerGender == 1)
        eGender = 1;
    CRefString sClass(ConversationGetPlayerClassShortString(eClass));
    CRefString sGender(ConversationGetGenderString(eGender));
    CRefString sStringLabel;
    sStringLabel.Format("%s%d%s%s", gszConvRootLineLabel, uLineUID, sClass.str(), sGender.str());
    StringTableEntry* ptTableEntry = StringListGetEntry(pdefStringList, sStringLabel.str());
    if (ptTableEntry) return ptTableEntry;
    sClass = ConversationGetPlayerClassShortString(eLineClass);
    sGender = ConversationGetGenderString(eLineGender);
    sStringLabel.Format("%s%d%s%s", gszConvRootLineLabel, uLineUID, sClass.str(), sGender.str());
    ptTableEntry = StringListGetEntry(pdefStringList, sStringLabel.str());
    if (ptTableEntry) return ptTableEntry;
    sClass = ConversationGetPlayerClassShortString(ePlayerClass);
    sGender = ConversationGetGenderString(ePlayerGender);
    sStringLabel.Format("%s%d%s", gszConvRootLineLabel, uLineUID, sClass.str());
    ptTableEntry = StringListGetEntry(pdefStringList, sStringLabel.str());
    if (ptTableEntry) return ptTableEntry;
    sStringLabel.Format("%s%d%s", gszConvRootLineLabel, uLineUID, sGender.str());
    ptTableEntry = StringListGetEntry(pdefStringList, sStringLabel.str());
    if (ptTableEntry) return ptTableEntry;
    sStringLabel.Format("%s%d", gszConvRootLineLabel, uLineUID);
    return StringListGetEntry(pdefStringList, sStringLabel.str());
}
#line 314 "X/Client/ConversationUtils.cpp"
int32 SoundNameToIndex(
    const SoundDefinition* ptSoundDef,
    CRefString& sSoundName,
    BOOL bIgnoreEmptySamples)
{

    uint32 dwSoundHash = StringHashI(sSoundName);

    for (int32 i = 0; i < ptSoundDef->nPermutationCount; i++)
    {
        SoundPermutation * ptPerm = &ptSoundDef->ptPermutations[i];


        if (ptPerm->dwSampleHash == dwSoundHash)
        {
            // Inferred: original lines 329-340 left no rows.












            if (ptPerm->snoValidatedSoundBank != -1)
            {




                if (bIgnoreEmptySamples)
                {
                    uint32 nLength = ConversationGetSoundLength(ptSoundDef->tHeader.snoHandle, i);
                    if (nLength <= 11)
                        continue;
                }
                return i;
            }
        }
    }
    return -1;
}

#line 365 "X/Client/ConversationUtils.cpp"
BOOL ConversationChooseSoundForLine(SNO& snoSound, int32& nPermIndex, const SNO snoConversation, const uint32 uLineUID, const ConversationSpeaker eLineSpeaker, const PlayerClass eLineClass, const ActorGender eLineGender, const PlayerClass ePlayerClass, const ActorGender ePlayerGender, const BOOL fReportErrors)
{
    CRefString sSpeaker;
    if (eLineSpeaker == 0)
        sSpeaker = ConversationGetSpeakerNameForPlayer(ePlayerClass, ePlayerGender);
    else {
        ScopedConversationDefinitionLockR pdefConversation(snoConversation);
        SNO snoSpeaker = ConversationSpeakerToSNO(pdefConversation, eLineSpeaker);
        sSpeaker = SNOToString(1, snoSpeaker, FALSE);
    }
    CRefString sSoundName = ConversationGetSoundName(snoConversation, sSpeaker.str());
    snoSound = SNOFileLookupGUID(40, sSoundName.str());
    if (snoSound == -1) return FALSE;
    CRefString sSampleName;
    ScopedSoundDefinitionLockR pdefSound(snoSound);
    PlayerClass eClass = eLineClass;
    ActorGender eGender = eLineGender;
    if (eLineSpeaker == 0) {
        if (eClass == -1) eClass = ePlayerClass;
        if (eGender == -1) eGender = ePlayerGender;
        sSpeaker = "";
    }
    else if (eGender == -1 && ePlayerGender == 1)
        eGender = 1;
    CRefString sClass(ConversationGetPlayerClassShortString(eClass));
    CRefString sGender(ConversationGetGenderString(eGender));
    sSampleName.Format("%03d%s%s%s", uLineUID, sSpeaker.str(), sClass.str(), sGender.str());
    nPermIndex = SoundNameToIndex(pdefSound, sSampleName, TRUE);
    if (nPermIndex != -1) return TRUE;
    sClass = ConversationGetPlayerClassShortString(eLineClass);
    sGender = ConversationGetGenderString(eLineGender);
    sSampleName.Format("%03d%s%s%s", uLineUID, sSpeaker.str(), sClass.str(), sGender.str());
    nPermIndex = SoundNameToIndex(pdefSound, sSampleName, TRUE);
    if (nPermIndex != -1) return TRUE;
    sClass = ConversationGetPlayerClassShortString(ePlayerClass);
    sGender = ConversationGetGenderString(ePlayerGender);
    sSampleName.Format("%03d%s%s", uLineUID, sSpeaker.str(), sClass.str());
    nPermIndex = SoundNameToIndex(pdefSound, sSampleName, TRUE);
    if (nPermIndex != -1) return TRUE;
    sSampleName.Format("%03d%s%s", uLineUID, sSpeaker.str(), sGender.str());
    nPermIndex = SoundNameToIndex(pdefSound, sSampleName, TRUE);
    if (nPermIndex != -1) return TRUE;
    sSampleName.Format("%03d%s", uLineUID, sSpeaker.str());
    nPermIndex = SoundNameToIndex(pdefSound, sSampleName, TRUE);
    // Inferred: if/return form; the original branches into the shared return-TRUE block (510:2) instead of a cset.
    if (nPermIndex != -1) return TRUE;
    return FALSE;
}
#line 578 "X/Client/ConversationUtils.cpp"
uint32 ConversationGetSoundLength(const SNO snoSound, const int32 nPermIndex)
{
    uint32 dwLength = 0;
    ScopedSoundDefinitionLockR ptSoundDef(snoSound);
    if (nPermIndex >= 0 && nPermIndex < ptSoundDef->nPermutationCount) {
        const SoundPermutation* ptSoundPerm = &ptSoundDef->ptPermutations[nPermIndex];
        ScopedSoundBankDefinitionLockR ptSoundBankDef(ptSoundPerm->snoValidatedSoundBank);
        int32 nSampleIndex = SoundBankSampleIndexFromHash(ptSoundPerm->snoValidatedSoundBank, ptSoundPerm->dwSampleHash);
        if (nSampleIndex != -1) {
            const SoundSample* ptSample = &ptSoundBankDef->ptSoundSamples[nSampleIndex];
            dwLength = ptSample->ComputeDuration();
        }
    }
    return dwLength;
}
