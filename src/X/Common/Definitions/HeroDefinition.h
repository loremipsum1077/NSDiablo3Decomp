// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/HeroDefinition.h
// Types and members sit on their DWARF decl_line. Inline bodies were written from the copies
// inlined into SPlayer.cpp, SWorld.cpp, SGizmo.cpp, ScriptUtil.cpp, ConversationMgr.cpp,
// Player.cpp, CHero.cpp, OnlineActions.cpp and HeroDefinition.cpp (llvm-objdump -l and
// tools/dwarf_fn.py); a member whose body no readable caller shows is declared only.
// The reference header names X/Client/OnlineService/OnlineServiceClientBase.h for `Act`; that
// typedef comes from X/Common/Act.h here, the client header holds unrelated templates.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Act.h"
#include "../CommonTypes.h"
#include "AttachmentDefinition.h"
#include "Definitions.h"
#include "../GameBalance.h"
#include "../HotbarButton.h"
#include "../Protobuf/Hero.pb.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XPtr64.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/SigmaTypes.h"
#include <stddef.h>
#include <stdint.h>

struct FileReference;
struct Player;
class XMemoryAllocatorInterface;














static const int32 MAX_QUEST_STEP_OBJECTIVES = 20;  // PlayerSavedQuest::arObjectiveState[20]
static const int32 MAX_QUEST_STEP_FAILURE_CONDITIONS = 20;  // PlayerSavedQuest::arFailureConditionState[20]
// static const int32 MAX_QUEST_BONUS_REWARDS;  // line 46; no caller read so far gives its value

typedef int32 SkillButton; enum {
    SKILLBUTTON_INVALID = -1,
    FIRST_SKILL_BUTTON = 0,
    SKILL_LMB = 0,
    SKILL_RMB = 1,
    SKILL_HB1 = 2,
    SKILL_HB2 = 3,
    SKILL_HB3 = 4,
    SKILL_HB4 = 5,
    SKILL_HB_LAST = 5,
    NUM_SKILL_BUTTONS = 6,
};




enum {
    PLAYERFLAG_HARDCORE = 0,
    PLAYERFLAG_FEMALE = 1,
    PLAYERFLAG_BOOSTED = 2,
    PLAYERFLAG_DECEASED = 3,
    PLAYERFLAG_TRIED_SALVAGE = 4,
    DEPRECATED_PLAYERFLAG_KILLED_FIRST = 5,
    DEPRECATED_PLAYERFLAG_KILLED_SKELETON_KING = 5,
    DEPRECATED_PLAYERFLAG_KILLED_WRETCHED_QUEEN = 6,
    DEPRECATED_PLAYERFLAG_KILLED_BUTCHER = 7,
    DEPRECATED_PLAYERFLAG_KILLED_BELIAL = 8,
    DEPRECATED_PLAYERFLAG_KILLED_AZMODAN = 9,
    DEPRECATED_PLAYERFLAG_KILLED_DIABLO = 10,
    DEPRECATED_PLAYERFLAG_KILLED_SPIDER_QUEEN = 11,
    DEPRECATED_PLAYERFLAG_KILLED_MAGHDA = 12,
    DEPRECATED_PLAYERFLAG_KILLED_ZOLTAN_KULLE = 13,
    DEPRECATED_PLAYERFLAG_KILLED_GLUTTONY = 14,
    DEPRECATED_PLAYERFLAG_KILLED_SIEGEBREAKER_DEMON = 15,
    DEPRECATED_PLAYERFLAG_KILLED_MISTRESS_OF_PAIN = 16,
    DEPRECATED_PLAYERFLAG_KILLED_DESPAIR = 17,
    DEPRECATED_PLAYERFLAG_KILLED_IZUAL = 18,
    DEPRECATED_PLAYERFLAG_KILLED_UNIQUE_UNBURIED = 19,
    DEPRECATED_PLAYERFLAG_KILLED_LAST = 19,
    PLAYERFLAG_RECEIVED_LOOT_FIND_BOOST = 23,
    PLAYERFLAG_DELETED_DEPRECATED = 24,
    PLAYERFLAG_ENTERED_ACT1 = 25,
    PLAYERFLAG_ENTERED_ACT2 = 26,
    PLAYERFLAG_ENTERED_ACT3 = 27,
    PLAYERFLAG_ENTERED_ACT4 = 28,
    PLAYERFLAG_FORCE_RENAME = 29,
    PLAYERFLAG_ENTERED_ACT5 = 30,
    PLAYERFLAG_FREE_ENCHANT = 31,
    PLAYERFLAG_COUNT = 32,
};












enum {
    BOSSKILLFLAG_SKELETON_KING = 0,
    BOSSKILLFLAG_WRETCHED_QUEEN = 1,
    BOSSKILLFLAG_BUTCHER = 2,
    BOSSKILLFLAG_BELIAL = 3,
    BOSSKILLFLAG_AZMODAN = 4,
    BOSSKILLFLAG_DIABLO = 5,
    BOSSKILLFLAG_SPIDER_QUEEN = 6,
    BOSSKILLFLAG_MAGHDA = 7,
    BOSSKILLFLAG_ZOLTAN_KULLE = 8,
    BOSSKILLFLAG_GLUTTONY = 9,
    BOSSKILLFLAG_SIEGEBREAKER_DEMON = 10,
    BOSSKILLFLAG_MISTRESS_OF_PAIN = 11,
    BOSSKILLFLAG_DESPAIR = 12,
    BOSSKILLFLAG_IZUAL = 13,
    BOSSKILLFLAG_UNIQUE_UNBURIED = 14,
    BOSSKILLFLAG_MALTHAEL = 15,
    BOSSKILLFLAG_GREED = 16,
    BOSSKILLFLAG_ADRIA = 17,
    BOSSKILLFLAG_URZAEL = 18,
    BOSSKILLFLAG_UNIQUE_DEATH_MAIDEN = 19,
    BOSSKILLFLAG_ANNIVERSARY_DIABLO = 20,
};


typedef int32 HirelingClass;
typedef int32 ItemQualityLevel;

typedef int32 PrimaryDamageAttribute; enum {
    PRIMARY_DAMAGE_ATTRIBUTE_NONE = -1,
    PRIMARY_DAMAGE_ATTRIBUTE_STRENGTH = 0,
    PRIMARY_DAMAGE_ATTRIBUTE_DEXTERITY = 1,
    PRIMARY_DAMAGE_ATTRIBUTE_INTELLIGENCE = 2,
    PRIMARY_DAMAGE_ATTRIBUTE_COUNT = 3,
};




enum {
    NUM_SKILL_SLOTS = 6,
    NUM_TRAIT_SLOTS = 4,
    NUM_GAME_FLAGS = 1024,
    SIZE_GAME_FLAGS = 128,
    MAX_NUM_SAVED_QUESTS = 64,
    NUM_QUEST_HISTORY_ENTRIES = 200,
    NUM_QUEST_REWARD_ENTRIES = 100,
    MAX_LEARNED_LORES = 512,
    NUM_SAVED_CONVERSATIONS = 560,
    MAX_SAVED_CONVERSATIONS_BYTE_ARRAY_SIZE = 70,
};




int32 QuestGetNumObjectivesToSave(SNO snoQuest, int32 nStepUID);  // X/Common/Quests/Quests.cpp
int32 QuestGetNumFailureConditionsToSave(SNO snoQuest, int32 nStepUID);  // X/Common/Quests/Quests.cpp

struct PlayerSavedQuest {  // size 0xa8

    void Export(D3::Hero::SavedQuest & tQuest) const  // out of line in CHero.cpp, 0x710001fd70

    {

        tQuest.Clear();


        tQuest.set_sno_quest(snoQuest);
        tQuest.set_current_step_uid(nCurrentStepUID);


        int32 nNumObjectives = QuestGetNumObjectivesToSave(snoQuest, nCurrentStepUID);
        int32 nNumFailureConditions = QuestGetNumFailureConditionsToSave(snoQuest, nCurrentStepUID);


        for (int i = 0; i < nNumObjectives && i < sizeof(arObjectiveState) / sizeof(arObjectiveState[0]); i++)
        {
            tQuest.add_objective_state(arObjectiveState[i]);
        }
        for (int i = 0; i < nNumFailureConditions; i++)
        {
            tQuest.add_failure_condition_state(arFailureConditionState[i]);
        }
    }

    void Import(const D3::Hero::SavedQuest & tQuest)
    {
        // Inference: loop tests copied from Export (185), size test first and unsigned sizeof bound; matches PlayerGetSavedQuest, but columns 201:24/201:89/205:104 do not fit exactly.
        snoQuest = tQuest.sno_quest();
        nCurrentStepUID = tQuest.current_step_uid();

        for (int i = 0; i < tQuest.objective_state_size() && i < sizeof(arObjectiveState) / sizeof(arObjectiveState[0]); i++)
        {
            arObjectiveState[i] = tQuest.objective_state(i);
        }
        for (int i = 0; i < tQuest.failure_condition_state_size() && i < sizeof(arFailureConditionState) / sizeof(arFailureConditionState[0]); i++)
        {
            arFailureConditionState[i] = tQuest.failure_condition_state(i);
        }
    }

    SNO snoQuest;  // 0x00
    int32 nCurrentStepUID;  // 0x04
    int32 arObjectiveState[20];  // 0x08
    int32 arFailureConditionState[20];  // 0x58
};
static_assert(sizeof(PlayerSavedQuest) == 0xa8, "PlayerSavedQuest");

struct PlayerSavedQuestUpdateMessage {  // size 0x2a1c

    RequiredMessageHeader tHeader;  // 0x00

    Act eLastPlayedAct;  // 0x08
    SNO snoLastPlayedQuest;  // 0x0c
    int32 nLastPlayedStepUID;  // 0x10
    int32 nHandicapLevel;  // 0x14

    PlayerSavedQuest tSavedQuest[64];  // 0x18
    uint32 dwNumQuests;  // 0x2a18
};
static_assert(sizeof(PlayerSavedQuestUpdateMessage) == 0x2a1c, "PlayerSavedQuestUpdateMessage");


struct PlayerQuestHistoryEntry {  // size 0x8

    void Export(D3::Hero::QuestHistoryEntry & tEntry) const
    {

        tEntry.set_sno_quest(snoQuest);

        if (nStepUID != -3)  // -3 is the proto default of highest_played_quest_step
        {
            tEntry.set_highest_played_quest_step(nStepUID);
        }
    }

    void Import(const D3::Hero::QuestHistoryEntry & tEntry)
    {

        snoQuest = tEntry.sno_quest();
        nStepUID = tEntry.highest_played_quest_step();
    }

    BOOL operator==(const PlayerQuestHistoryEntry &);





    SNO snoQuest;  // 0x00
    int32 nStepUID;  // 0x04
};
static_assert(sizeof(PlayerQuestHistoryEntry) == 0x8, "PlayerQuestHistoryEntry");

struct PlayerQuestHistoryUpdateMessage {  // size 0x64c

    RequiredMessageHeader tHeader;  // 0x00
    PlayerQuestHistoryEntry tQuestHistory[200];  // 0x08
    uint32 dwNumEntries;  // 0x648
};
static_assert(sizeof(PlayerQuestHistoryUpdateMessage) == 0x64c, "PlayerQuestHistoryUpdateMessage");












struct PlayerQuestRewardHistoryEntry {  // size 0xc

    void Export(D3::Hero::QuestRewardHistoryEntry & tEntry) const
    {

        tEntry.set_sno_quest(snoQuest);
        tEntry.set_step_uid(nStepUID);
        tEntry.set_difficulty_flags(uDifficultyFlags);
    }

    void Import(const D3::Hero::QuestRewardHistoryEntry & tEntry)
    {

        snoQuest = tEntry.sno_quest();
        nStepUID = tEntry.step_uid();
        if (tEntry.has_difficulty_flags())
        {
            uDifficultyFlags = (uint8)tEntry.difficulty_flags();
        }
        else
        {
            uDifficultyFlags = 0;
        }
    }

    SNO snoQuest;  // 0x00
    int32 nStepUID;  // 0x04
    uint8 uDifficultyFlags;  // 0x08
};
static_assert(sizeof(PlayerQuestRewardHistoryEntry) == 0xc, "PlayerQuestRewardHistoryEntry");





struct HirelingInfo {  // size 0x20

    void Export(D3::Hireling::Info & tInfo) const  // out of line in HeroDefinition.cpp, 0x7100636a90
    {

        tInfo.Clear();


        tInfo.set_hireling_class(eClass);
        tInfo.set_gbid_name(gbidName);
        tInfo.set_dead(fDead != FALSE);

        for (int32 i = 0; i < 5; i++)
        {
            tInfo.add_power_key_params(arnPowerKeyParam[i]);
        }
    }

    void Import(const D3::Hireling::Info & tInfo)
    {

        eClass = tInfo.hireling_class();
        gbidName = tInfo.gbid_name();
        fDead = tInfo.dead();

        for (int i = 0; i < 5 && i < tInfo.power_key_params_size(); i++)
        {
            arnPowerKeyParam[i] = tInfo.power_key_params(i);
        }
    }

    HirelingClass eClass;  // 0x00
    GBID gbidName;  // 0x04
    BOOL fDead;  // 0x08
    int32 arnPowerKeyParam[5];  // 0x0c
};
static_assert(sizeof(HirelingInfo) == 0x20, "HirelingInfo");
struct HirelingSavedData {  // size 0x88


    void Export(D3::Hireling::SavedData & tSavedData) const
    {

        tSavedData.Clear();

        for (int32 i = 0; i < 4; i++)
        {
            tHireling[i].Export(*tSavedData.add_hirelings());
        }
        tSavedData.set_active_hireling(eActiveHireling);
        tSavedData.set_available_hirelings_bitfield(dwAvailableHirelings);
    }


    void Import(const D3::Hireling::SavedData & tSavedData)  // out of line in HeroDefinition.cpp, 0x7100636360
    {

        for (int32 i = 0; i < 4 && i < tSavedData.hirelings_size(); i++)
        {
            tHireling[i].Import(tSavedData.hirelings(i));
        }
        eActiveHireling = (HirelingClass)tSavedData.active_hireling();
        dwAvailableHirelings = tSavedData.available_hirelings_bitfield();
    }

    HirelingInfo tHireling[4];  // 0x00
    HirelingClass eActiveHireling;  // 0x80
    uint32 dwAvailableHirelings;  // 0x84
};
static_assert(sizeof(HirelingSavedData) == 0x88, "HirelingSavedData");

struct LearnedLore {  // size 0x804

    void Initialize();




    int32 Find(const SNO snoLore) const
    {

        for (int32 i = 0; i < m_nLearnedCount; i++)
        {
            if (m_snoLoreLearned[i] == snoLore)
            {
                return i;
            }
        }

        return -1;
    }

    void Add(const SNO snoLore)
    {


        m_snoLoreLearned[m_nLearnedCount] = snoLore;
        m_nLearnedCount++;
    }

    void Export(D3::GameBalance::BitPackedGbidArray & tLearnedLore) const
    {

        PackAssetsIntoBitPackedGbidArray(tLearnedLore,
                                         4,  // ASSET_LIST_LORE_SNOS (Definitions/GlobalDefinition.h, not included here)

                                         (int32 *)m_snoLoreLearned,
                                         m_nLearnedCount,
                                         512);
    }

    void Import(const D3::GameBalance::BitPackedGbidArray & tLearnedLore)
    {


        m_nLearnedCount = UnpackAssetsFromBitPackedGbidArray(tLearnedLore,
                                                             4,  // ASSET_LIST_LORE_SNOS
                                                             m_snoLoreLearned,
                                                             512);
    }

    int32 m_nLearnedCount;  // 0x00
    SNO m_snoLoreLearned[512];  // 0x04
};
static_assert(sizeof(LearnedLore) == 0x804, "LearnedLore");
int32 AssetListArrayToBitfield(AssetListType eAssetList, const int32 * ptAssetIDArray, int32 nAssets, uint8 * ptByteArray, int32 nByteArrayLength, int32 * pnLeadingNullBytes);  int32 AssetListIndexGet(AssetListType eAssetList, int32 nAssetID);  int32 AssetListGetAssetAtIndex(AssetListType eAssetList, int32 nIndex);  void SigmaMemoryClear(void* lp, size_t size);  int32 AssetListBitfieldToArray(AssetListType eAssetList, const uint8 * ptByteArray, int32 nByteArrayLength, int32 * ptAssetIDArray, int32 nMaxAssets, int32 nLeadingNullBytes);  void SigmaMemoryCopy(void* lpDest, const void* lpSrc, size_t size);
struct SavedConversations {  // size 0x90c

    void Initialize()
    {
        m_nSavedConvCount = 0;
    }





    BOOL IsUnlocked(const SNO snoConv) const
    {

        for (int32 i = 0; i < m_nSavedConvCount; i++)
        {
            if (m_snoSavedConv[i] == snoConv)
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL HasPlayed(const SNO snoConv) const
    {

        for (int32 i = 0; i < m_nSavedConvCount; i++)
        {
            if (m_snoSavedConv[i] == snoConv)
            {
                int32 nByteIndex = i >> 3;
                int32 nBitIndex = i & 7;
                return (m_arPlayedConv[nByteIndex] & (1 << nBitIndex)) != 0;
            }
        }
        return FALSE;
    }


    BOOL HasPlayed(const int32 index, SNO & snoConv) const
    {

        if (index < 0 || index >= NUM_SAVED_CONVERSATIONS)
        {
            return FALSE;
        }

        snoConv = m_snoSavedConv[index];

        const int32 nBitIndex = index & 7;
        return (m_arPlayedConv[index >> 3] & (1 << nBitIndex)) != 0;
    }



    int32 GetNumConversations() const
    {
        return m_nSavedConvCount;
    }


    void Save(const SNO snoConv, const BOOL bPlayed)
    {
        int32 index = -1;
        BOOL bNewEntry = FALSE;

        for (int32 i = 0; i < m_nSavedConvCount; i++)
        {
            if (m_snoSavedConv[i] == snoConv)
            {
                index = i;
                break;
            }
        }

        if (index == -1)
        {

            if (m_nSavedConvCount >= NUM_SAVED_CONVERSATIONS)
            {
                return;
            }

            index = m_nSavedConvCount;
            m_snoSavedConv[m_nSavedConvCount++] = snoConv;
            bNewEntry = TRUE;
        }

        const int32 nByte = index >> 3;
        const int32 nBit = index & 7;

        if (bPlayed)
        {
            m_arPlayedConv[nByte] |= (uint8)(1 << nBit);
        }
        else if (bNewEntry)
        {
            m_arPlayedConv[nByte] &= (uint8)~(1 << nBit);
        }
    }
    void Export(D3::Hero::SavedConversations & tConversations) const  // out of line in CHero.cpp

    {
        tConversations.Clear();




        uint8 arSavedConv[70];
        int32 nLeadingNullBytes = 0;

        int32 nNumBytes = AssetListArrayToBitfield(1, m_snoSavedConv, m_nSavedConvCount, arSavedConv, sizeof(arSavedConv), &nLeadingNullBytes);






        BOOL bUseBitfield = FALSE;


        if (nNumBytes != 0 && nNumBytes < m_nSavedConvCount * sizeof(int32))
        {
            bUseBitfield = TRUE;
            tConversations.set_sno_saved_conversations_bitfield(arSavedConv, nNumBytes);

            if (nLeadingNullBytes > 0)
                tConversations.set_bitfield_leading_null_bytes(nLeadingNullBytes);
        }








        for (int32 i = 0; i < m_nSavedConvCount; i++)
        {
            int32 nListIndex = AssetListIndexGet(1, m_snoSavedConv[i]);
            if (bUseBitfield && nListIndex != -1)
                continue;
            tConversations.add_sno_saved_conversations(m_snoSavedConv[i]);
        }



        if (bUseBitfield)
        {


            uint8 arPlayedConvSwizzled[70];

            SigmaMemoryClear(arPlayedConvSwizzled, sizeof(arPlayedConvSwizzled));

            int32 nCount = 0;

            for (int32 nByteIndex = 0; nByteIndex < nNumBytes; nByteIndex++)
            {
                uint8 uByte = arSavedConv[nByteIndex];
                for (int32 nBitIndex = 0; nBitIndex < 8; nBitIndex++)
                {
                    if (uByte & (1 << nBitIndex))
                    {
                        int32 nIndex = nByteIndex * 8 + nBitIndex;

                        SNO snoConv = AssetListGetAssetAtIndex(1, nIndex);


                        if (HasPlayed(snoConv))
                        {

                            int32 nBitIndex2 = nCount & 7;
                            int32 nByteIndex2 = nCount >> 3;  arPlayedConvSwizzled[nByteIndex2] |= (1 << nBitIndex2);
                        }

                        nCount++;
                    }
                }
            }


            for (int32 i = 0; i < tConversations.sno_saved_conversations_size(); i++)
            {
                SNO snoConv = tConversations.sno_saved_conversations(i);
                if (HasPlayed(snoConv))
                {

                    int32 nBitIndex = nCount & 7;
                    int32 nByteIndex = nCount >> 3;  arPlayedConvSwizzled[nByteIndex] |= (1 << nBitIndex);
                }
                nCount++;
            }




            int32 nNumBytes2 = (m_nSavedConvCount + 7) >> 3;
            tConversations.set_played_conversations_bitfield(arPlayedConvSwizzled, nNumBytes2);
        }
        else
        {


            int32 nNumBytes = (m_nSavedConvCount + 7) >> 3;
            tConversations.set_played_conversations_bitfield(m_arPlayedConv, nNumBytes);
        }
    }

    void Import(  // out of line in SaveMigration.cpp, 0x71000641f0
        const D3::Hero::SavedConversations & tConversations)
    {

        const blz::string & strData = tConversations.sno_saved_conversations_bitfield();
        m_nSavedConvCount = 0;
        if (strData.length() > 0)
        {
            uint8 arSavedConv[70];

            int32 nBytes = Math::Min((int32)strData.length(), (int32)sizeof(arSavedConv));
            SigmaMemoryCopy(arSavedConv, strData.data(), nBytes);

            if (nBytes != 0)
            {
                int32 nLeadingNullBytes = tConversations.has_bitfield_leading_null_bytes() ? tConversations.bitfield_leading_null_bytes() : 0;
                m_nSavedConvCount = AssetListBitfieldToArray(1, arSavedConv, nBytes,


                            m_snoSavedConv, NUM_SAVED_CONVERSATIONS, nLeadingNullBytes);
            }
        }

        // inferred: original compares unsigned (b.hs) and the i++ column (127) implies a 24-character bound;
        // a sizeof-based array count is the closest spelling found (no _countof-style macro exists in src).

        for (int32 i = 0; i < tConversations.sno_saved_conversations_size() && m_nSavedConvCount < sizeof(m_snoSavedConv) / sizeof(SNO); i++)
        {
            SNO snoSavedConversation = tConversations.sno_saved_conversations(i);
            if (SNOFileExists(CONVERSATION_SNO, snoSavedConversation))
            {
                m_snoSavedConv[m_nSavedConvCount] = snoSavedConversation;
                m_nSavedConvCount++;
            }
        }



        const blz::string & strPlayedData = tConversations.played_conversations_bitfield();


        if (strPlayedData.length() > 0)
        {
            SigmaMemoryCopy(m_arPlayedConv, strPlayedData.data(), Math::Min(strPlayedData.length(), sizeof(m_arPlayedConv)));
        }
    }

    int32 m_nSavedConvCount;  // 0x00
    uint8 m_arPlayedConv[70];  // 0x04
    SNO m_snoSavedConv[560];  // 0x4c
};
static_assert(sizeof(SavedConversations) == 0x90c, "SavedConversations");


struct PlayerSavedConversationsUpdateMessage {  // size 0x914

    RequiredMessageHeader tHeader;  // 0x00

    SavedConversations tSavedConversations;  // 0x08
};
static_assert(sizeof(PlayerSavedConversationsUpdateMessage) == 0x914, "PlayerSavedConversationsUpdateMessage");




struct SavePointData {  // size 0x10

    SNO snoWorld;  // 0x00
    int32 nNum;  // 0x04
    BOOL bCreatesPortal;  // 0x08


    ACDID idACDReturnPortal;  // 0x0c

    void Reset()
    {
        snoWorld = SNO_NONE;
        nNum = -1;
        bCreatesPortal = FALSE;
        idACDReturnPortal = ACDID_INVALID;
    }


    BOOL IsValid() const
    {

        return HasValidSavePoint() || HasValidPortalDestination();
    }

    BOOL HasValidSavePoint() const
    {

        return snoWorld != SNO_NONE && nNum != -1;
    }

    BOOL HasValidPortalDestination() const
    {

        return snoWorld != SNO_NONE && idACDReturnPortal != ACDID_INVALID;
    }

    void Export(D3::Hero::SavePointData_Proto & tSavePoint) const
    {

        tSavePoint.Clear();
        tSavePoint.set_sno_world(snoWorld);
        tSavePoint.set_savepoint_number(nNum);
        tSavePoint.set_creates_portal(bCreatesPortal);
    }

    void Import(const D3::Hero::SavePointData_Proto & tSavePoint)
    {

        snoWorld = tSavePoint.sno_world();
        nNum = tSavePoint.savepoint_number();
        bCreatesPortal = tSavePoint.creates_portal();


        idACDReturnPortal = ACDID_INVALID;
    }
};
static_assert(sizeof(SavePointData) == 0x10, "SavePointData");

struct SavePointDataMessage {  // size 0x1c

    RequiredMessageHeader tHeader;  // 0x00
    SavePointData tData;  // 0x08
    int32 nPlayerIndex;  // 0x18
};
static_assert(sizeof(SavePointDataMessage) == 0x1c, "SavePointDataMessage");


typedef int32 TimerID;

struct ActiveSkillSavedData {  // size 0x10

    void Export(D3::Hero::SkillWithRune & tSavedData) const;


    void Import(const D3::Hero::SkillWithRune & tSavedData);


    SNO snoSkill;  // 0x00
    RuneType eRuneType;  // 0x04

    uint32 dwLastLifeOnHitTick;  // 0x08
    TimerID idChargesTimer;  // 0x0c
};
static_assert(sizeof(ActiveSkillSavedData) == 0x10, "ActiveSkillSavedData");

struct PlayerSavedData {  // size 0x12b0; the ctor sits on this line (HotbarButtonData() is its only effect)
    PlayerSavedData() {}
    void SetDefaults();

    void Export(const Player & tPlayer, const GBID gbidHero, D3::Hero::SavedData & tSavedData, BOOL bConsoleClient) const;





    void Import(const D3::Hero::SavedData & tSavedData);


    HotbarButtonData tHotbarBtnAssignments[6];  // 0x00
    HotbarButtonData tPotionBtn;  // 0x60
    uint8 bSkillSlotEverAssigned;  // 0x70

    uint32 uSecondsPlayed;  // 0x74

    uint32 m_ActivatedWaypoints;  // 0x78

    HirelingSavedData tHirelingSavedData;  // 0x7c

    uint32 uTimeLastLevel;  // 0x104

    LearnedLore tLearnedLore;  // 0x108


    SavedConversations tSavedConversations;  // 0x90c

    ActiveSkillSavedData tActiveSkills[6];  // 0x1218
    SNO snoTraits[4];  // 0x1278
    GBID gbidLegendaryPowers[4];  // 0x1288

    SavePointData tLastSavePoint;  // 0x1298
    uint32 uEventFlags;  // 0x12a8
    uint32 uSkillKitVersion;  // 0x12ac
};
static_assert(sizeof(PlayerSavedData) == 0x12b0, "PlayerSavedData");

struct PlayerSavedDataMessage {  // size 0x12bc; the ctor sits on this line
    PlayerSavedDataMessage() {}
    RequiredMessageHeader tHeader;  // 0x00
    PlayerSavedData tSavedData;  // 0x08
    int32 nPlayerIndex;  // 0x12b8
};
static_assert(sizeof(PlayerSavedDataMessage) == 0x12bc, "PlayerSavedDataMessage");

struct PlayerSkillsMessage {  // size 0x8c

    RequiredMessageHeader tHeader;  // 0x00
    ActiveSkillSavedData tActiveSkills[6];  // 0x08
    SNO snoTraits[4];  // 0x68
    GBID gbidLegendaryPowers[4];  // 0x78
    int32 nPlayerIndex;  // 0x88
};
static_assert(sizeof(PlayerSkillsMessage) == 0x8c, "PlayerSkillsMessage");

struct LastWaypointLevelAreaDataMessage {  // size 0x10

    RequiredMessageHeader tHeader;  // 0x00
    SNO snoLevelArea;  // 0x08
    int32 nPlayerIndex;  // 0x0c
};
static_assert(sizeof(LastWaypointLevelAreaDataMessage) == 0x10, "LastWaypointLevelAreaDataMessage");











struct AssignSkillMessage {  // size 0x18

    RequiredMessageHeader tHeader;  // 0x00
    SNO snoPower;  // 0x08
    int32 nRuneType;  // 0x0c
    int32 nSlot;  // 0x10
    int32 nPlayerIndex;  // 0x14
};
static_assert(sizeof(AssignSkillMessage) == 0x18, "AssignSkillMessage");


struct AssignLegendaryPowerMessage {  // size 0x14

    RequiredMessageHeader tHeader;  // 0x00
    GBID gbidLegendaryItem;  // 0x08
    int32 nSlot;  // 0x0c
    int32 nPlayerIndex;  // 0x10
};
static_assert(sizeof(AssignLegendaryPowerMessage) == 0x14, "AssignLegendaryPowerMessage");


struct UnassignSkillMessage {  // size 0xc

    RequiredMessageHeader tHeader;  // 0x00
    int32 nSlot;  // 0x08
};
static_assert(sizeof(UnassignSkillMessage) == 0xc, "UnassignSkillMessage");


struct SwapSkillsMessage {  // size 0x10

    RequiredMessageHeader tHeader;  // 0x00
    int32 nSlot0;  // 0x08
    int32 nSlot1;  // 0x0c
};
static_assert(sizeof(SwapSkillsMessage) == 0x10, "SwapSkillsMessage");


struct AssignTraitsMessage {  // size 0x18

    RequiredMessageHeader tHeader;  // 0x00
    SNO snoPower[4];  // 0x08
};
static_assert(sizeof(AssignTraitsMessage) == 0x18, "AssignTraitsMessage");


struct SpendParagonPointsMessage {  // size 0x10

    RequiredMessageHeader tHeader;  // 0x00
    GBID gbidParagonBonus;  // 0x08
    int32 nPoints;  // 0x0c
};
static_assert(sizeof(SpendParagonPointsMessage) == 0x10, "SpendParagonPointsMessage");


struct HeroDefinition {  // size 0x20

    RequiredDefinitionHeader tHeader;  // 0x00

    SerializeData serProtoData;  // 0x0c
    XPtr64<unsigned char> pProtoData;  // 0x18
};
static_assert(sizeof(HeroDefinition) == 0x20, "HeroDefinition");


class HeroGroupDef : public GroupDefinition {  // size 0xa0
public:

    HeroGroupDef();

    GROUPDEFINITION_MEMORY_FUNCTIONS()  // line 951; the macro name is invented, see Definitions.h

    virtual uint32 GetMaxCount();  // vtable slot 11
    virtual BOOL CanReferenceOtherSNOs() const { return FALSE; }  // vtable slot 13

    virtual SNOPackType GetSNOPackDestination(const void *) const  // vtable slot 7
    {

        return SNOPACKTYPE_INVALID;  // line 959
    }

    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21


    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
};
static_assert(sizeof(HeroGroupDef) == 0xa0, "HeroGroupDef");

extern HeroGroupDef * gptGroupDefHero; // HeroDefinition.cpp:247
extern const uint32 HERO_DEFINITION_VERSION_1_0_5; // HeroDefinition.cpp:43
extern const uint32 HERO_DEFINITION_VERSION_2_0_0; // HeroDefinition.cpp:44
