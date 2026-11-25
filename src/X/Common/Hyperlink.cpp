#include "Hyperlink.h"
#include "SNO/SNOName.h"
#include "../Sigma/RefString.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include <stdlib.h>
#include <string.h>

// X/Common/SNO/SNOGroups.cpp:78
LPCSTR SNOGroupGetName(const SNOGroup snoGroup);
#include "../Client/OnlineService/CStore.h"
#include <stdio.h>
#include <strings.h>






#include "OnlineService/Achievements.h"
#include "../Client/Battlenet/Hub.h"
// X/Client/Battlenet/Hub.cpp
Hub & HubGet();












#include "../Client/OnlineService/ChatTypes.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/GlobalDefinition.h"
#include "Protobuf/Items.pb.h"
#include "Items.h"
#include "../../Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/repeated_field.h"
uint32 atoh(LPCSTR sz, BOOL & bSuccess);  // X/Sigma/Hex.cpp:47
uint32 ItemSocketCountFromGenerator(const D3::Items::Generator & tGenerator);  // X/Common/Items.cpp:7820
// X/Client/OnlineService/Chat.h: OnlineService::Chat is declaration-only in the DWARF. sParseChatId calls
// vtable slot 1 (original blr through [vtable, #0x8]) with a ChannelId, returning a ChatId; both slot names invented.
namespace OnlineService {
class Chat {
public:
    virtual void VSlot0() = 0;
    virtual OnlineService::ChatId VSlot1(const OnlineService::ChannelId & tChannelId) = 0;
};
}  // namespace OnlineService
// Hyperlink.cpp:1385, defined below; HyperlinkValidate (line 253) uses it first.
static BOOL sParseGroupId(const char *& pszEntityIdString, GroupId & id);
// X/Client/OnlineService/ClientService.h:227. The DWARF carries ClientService as a declaration
// only; Chat() (line 228) reads 0x18 and Achievements() (line 232) reads 0x38 (original loads).
// Same stand-in as src/X/Client/Font/FormattedText.cpp; member names are not in the DWARF (invented).
namespace OnlineService {
class ClientService {
public:
    OnlineService::Chat & Chat() { return *m_pChat; }
    OnlineService::Achievements & Achievements() { return *m_pAchievements; }
private:
    uint8 _unadopted0[0x18];
    OnlineService::Chat * m_pChat;  // 0x18
    uint8 _unadopted20[0x18];
    OnlineService::Achievements * m_pAchievements;  // 0x38
};
}  // namespace OnlineService

// Defined below on their DWARF lines (762, 1061, 1173, 1417, 1586); HyperlinkValidate (189) calls them first.
static BOOL sPopulateItemGeneratorStructs(const char *& pszItemString, const BOOL fSocketedItem, google::protobuf::RepeatedField<D3::Items::Generator> & generatorList);
static BOOL sParseGameAccount(const char *& pszEntityIdString, OnlineService::GameAccountHandle & tGameAccount);
static BOOL sParseChatId(const char *& pszEntityIdString, OnlineService::ChatId & id);
static BOOL sParseAchievementData(const char *& pszAchievementDataString, HyperlinkAchievementData & tHyperlinkAchievementData);
static BOOL sParseStoreItem(const char *& pszURLString, OnlineService::StoreItem & tItem);

#include "ActorCommon.h"
#include "ActorCommonData.h"
#include "Definitions/StringListDefinition.h"
#include "UserInterface/UITextControl.h"

















// Moved off HyperlinkValidate's DWARF lines (300-316); the table has no DIE of its own.
static const char * const sgszHyperlinkTypeNames[HLT_COUNT] = {
    "",
    "Item",
    "OnlUserHdl",
    "OnlUser",
    "AchievmtHdl",
    "Achievmt",
    NULL,
    NULL,
    "URL",
    "ChtChnlId",
    "ChtChnl",
    "ChtInvt",
    "ClanCht",
    "GroupCht",
    "Prdct",
};





























enum StoreProductURLAttributes
{
    SPUA_CATEGORY,
    SPUA_ATTRIBUTE,
    SPUA_CONTENT_LICENSE,
    SPUA_GBID,
    SPUA_PRODUCT_ID,
    SPUA_PLATINUM_AMOUNT
};
// Inference: original string references identify the six attribute names in enumeration order.
static LPCSTR cguszStoreProductURLAttributeNames[6] = { "C", "A", "CL", "G", "I", "P" };













// The original helper is DW_INL_inlined, not declared inline.
static BOOL sIsHyperlinkTrailer(
    const char *& pszCurrentPos,
    BOOL bAdvancePastTrailer)
{
    if (pszCurrentPos[0] == '|' && pszCurrentPos[1] == 'h')
    {
        if (bAdvancePastTrailer)
            pszCurrentPos += 2;
        return TRUE;
    }
    return FALSE;
}






HyperlinkType HyperlinkValidate(
    const char *& pszString)
{

    const char * pszCurrentPos = pszString;


    HyperlinkType eHLType = HyperlinkParseType(pszCurrentPos, TRUE);
    switch (eHLType)
    {
        case HLT_ITEM:
        {





            google::protobuf::RepeatedField<D3::Items::Generator> generatorList;
            if (!sPopulateItemGeneratorStructs(pszCurrentPos, FALSE, generatorList))
                return HLT_NONE;
            break;


        }
        case HLT_ONLINE_USER:
        {
            OnlineService::GameAccountHandle tGameAccount;
            if (!sParseGameAccount(pszCurrentPos, tGameAccount))
                return HLT_NONE;
            break;
        }












        case HLT_CHAT_CHANNEL:
        {
            OnlineService::ChatId id;
            if (!sParseChatId(pszCurrentPos, id))
                return HLT_NONE;
            break;

        }










        case HLT_GROUP_CHAT:
        {
            GroupId id;
            if (!pszCurrentPos || (*pszCurrentPos != '0' && *pszCurrentPos != '1') || !sParseGroupId(++pszCurrentPos, id))
                return HLT_NONE;
            break;
        }

        case HLT_CLAN_CHAT:
        {
            if (!pszCurrentPos || (*pszCurrentPos != '0' && *pszCurrentPos != '1'))
                return HLT_NONE;


            ++pszCurrentPos;
            break;
        }
        case HLT_ACHIEVEMENT:
        {
            HyperlinkAchievementData tHyperlinkAchievementData;
            if (!sParseAchievementData(pszCurrentPos, tHyperlinkAchievementData))
                return HLT_NONE;
            break;
        }






















        case HLT_STORE_PRODUCT:
        {
            OnlineService::StoreItem tItem;
            if (!sParseStoreItem(pszCurrentPos, tItem))
                return HLT_NONE;
            break;

        }

        default:
            return HLT_NONE;
    }



    // Inference: header trailer follows the parsed payload, then the display text and the closing trailer.
    if (!sIsHyperlinkTrailer(pszCurrentPos, TRUE))
        return HLT_NONE;




    LPCSTR pszBarPos = strchr(pszCurrentPos, '|');
    if (!pszBarPos ||
        !sIsHyperlinkTrailer(pszBarPos, TRUE))
        return HLT_NONE;



    pszCurrentPos = pszBarPos;


    pszString = pszCurrentPos;
    return eHLType;
}



HyperlinkType HyperlinkParseType(const char *& pszString, BOOL bAdvanceStringPointer)
{
    const char * pszStringStart = pszString;
    if (pszStringStart[0] != '|' || pszStringStart[1] != 'H')
        return HLT_NONE;

    char * pszCurrentPos = (char *)pszStringStart + 2;
    char * pszSeparator = strchr(pszCurrentPos, ':');
    if (!pszSeparator)
        return HLT_NONE;

    HyperlinkType eHLType = HLT_NONE;
    size_t nLen = pszSeparator - pszCurrentPos;
    for (int32 i = 0; i < HLT_COUNT; i++)
    {
        if (sgszHyperlinkTypeNames[i] && strncmp(pszCurrentPos, sgszHyperlinkTypeNames[i], nLen) == 0)
        {
            eHLType = (HyperlinkType)i;
            break;
        }
    }

    if (eHLType == HLT_NONE)
        return HLT_NONE;

    if (bAdvanceStringPointer)
        pszString = pszSeparator + 1;
    return eHLType;
}













BOOL HyperlinkAdvancePastHyperlink(const char *& pszString)
{
    LPCSTR pszEndOfHyperlink = pszString;
    if (HyperlinkParseType(pszEndOfHyperlink, TRUE) == HLT_NONE)
        return FALSE;

    pszEndOfHyperlink = strchr(pszEndOfHyperlink, '|');
    if (!pszEndOfHyperlink || !sIsHyperlinkTrailer(pszEndOfHyperlink, FALSE))
        return FALSE;

    pszEndOfHyperlink = strchr(pszEndOfHyperlink + 2, '|');
    if (!pszEndOfHyperlink || !sIsHyperlinkTrailer(pszEndOfHyperlink, TRUE))
        return FALSE;

    pszString = pszEndOfHyperlink;
    return TRUE;
}









BOOL EscapeHyperlink(char * pszEscapedBuffer, LPCSTR szOriginalStr, size_t nEscapedBufferSize)
{
    size_t nRemainingBufferSize = nEscapedBufferSize;
    char * pszWriteBuffer = pszEscapedBuffer;
    LPCSTR szLastPosition = szOriginalStr;
    size_t nEscapedLength;
    BOOL fWasEscaped = FALSE;
    LPCSTR szEscapedPipe = "\\|";
    LPCSTR szCurrentPipe = strchr(szLastPosition, '|');

    while (szCurrentPipe)
    {
        size_t nLenToCopy = szCurrentPipe - szLastPosition;
        size_t nCopied = Blizzard::String::Copy(pszWriteBuffer, szLastPosition, Math::Min(nLenToCopy + 1, nRemainingBufferSize));
        if (nCopied < nLenToCopy)
            return fWasEscaped;
        nRemainingBufferSize -= nCopied;
        pszWriteBuffer += nCopied;

        nEscapedLength = Blizzard::String::Copy(pszWriteBuffer, szEscapedPipe, Math::Min((size_t)3, nRemainingBufferSize));
        if (nEscapedLength < 2)
            return TRUE;
        pszWriteBuffer += nEscapedLength;
        nRemainingBufferSize -= nEscapedLength;

        szLastPosition = szCurrentPipe + 1;
        fWasEscaped = TRUE;
        szCurrentPipe = strchr(szLastPosition, '|');
    }

    if (szLastPosition)
        Blizzard::String::Copy(pszWriteBuffer, szLastPosition, nRemainingBufferSize);
    return fWasEscaped;
}



































BOOL HyperlinkGetPrettyName(char * pszPrettyName, const char *& pszString, int32 nPrettyNameBufSize, BOOL bRemoveBracketsAndColorCoding)
{
    const char * pszStringStart = pszString;
    if (pszStringStart[0] != '|' || pszStringStart[1] != 'H')
        return FALSE;

    char * pszCurrentPos = (char *)pszStringStart + 2;
    char * pszBarPos = strchr(pszCurrentPos, '|');
    if (!pszBarPos || pszBarPos[1] != 'h')
        return FALSE;

    pszCurrentPos = pszBarPos + 2;
    const char * pszPrettyTextStart = pszCurrentPos;
    if (bRemoveBracketsAndColorCoding)
    {
        const char * pszTest = pszCurrentPos;
        if (*pszTest == '{')
        {
            LPCSTR pszCloseTag = strchr(pszTest, '}');
            if (!pszCloseTag)
                return FALSE;
            pszTest = pszCloseTag + 1;
        }
        if (*pszTest == '[')
            pszTest++;
        pszPrettyTextStart = pszTest;
    }

    char * pszEndBarPos = strchr(pszCurrentPos, '|');
    if (!pszEndBarPos || pszEndBarPos[1] != 'h')
        return FALSE;

    const char * pszPrettyTextEnd = pszEndBarPos;
    if (bRemoveBracketsAndColorCoding)
    {
        LPCSTR pszOpenTag = strchr(pszPrettyTextStart, '{');
        if (pszOpenTag)
        {
            if (!strchr(pszOpenTag, '}'))
                return FALSE;
            pszPrettyTextEnd = pszOpenTag;
        }
        if (pszPrettyTextEnd[-1] == ']')
            pszPrettyTextEnd--;
    }

    Blizzard::String::Copy(pszPrettyName, pszPrettyTextStart, Math::Min((uint32)(pszPrettyTextEnd - pszPrettyTextStart + 1), (uint32)nPrettyNameBufSize));
    pszString += pszEndBarPos + 2 - pszStringStart;
    return TRUE;
}





























// NON_MATCHING: the target duplicates the Find call on the successful pretty-name path. The rebuild shares it with the malformed-link path.
void HyperlinkRemoveFormattingFromString(
    char * pszModifiedString,
    const char * pszString,
    int32 nModifiedBufSize)
{
    const char * pszBegin = pszString;
    const char * pszHyperLinkBegin = Blizzard::String::Find(pszBegin, "|H");

    if (!pszHyperLinkBegin)
    {
        Blizzard::String::Copy(pszModifiedString, pszString, nModifiedBufSize);
    }
    else
    {
        pszModifiedString[0] = 0;
        size_t nLen = nModifiedBufSize;
        while (pszHyperLinkBegin)
        {

            char szTemp[1024];

            Blizzard::String::Copy(szTemp, pszString, Math::Min((uint32)(pszHyperLinkBegin - pszString) + 1, (uint32)sizeof(szTemp)));
            Blizzard::String::Append(pszModifiedString, szTemp, nLen);
            pszBegin = pszHyperLinkBegin;


            if (HyperlinkGetPrettyName(szTemp, pszBegin, sizeof(szTemp), TRUE))
            {
                Blizzard::String::Append(pszModifiedString, szTemp, nLen);
                pszHyperLinkBegin = pszString = pszBegin;
            }
            else
            {
                pszString = pszBegin;
                pszHyperLinkBegin = pszString + 2;
            }
            pszHyperLinkBegin = Blizzard::String::Find(pszHyperLinkBegin, "|H");
        }
        Blizzard::String::Append(pszModifiedString, pszString, nLen);
    }
}



static BOOL sParseItemGBHandle(
    char *& pszHyperlinkString,
    D3::GameBalance::Handle & hGB)
{

    char *pszSeparator = strchr(pszHyperlinkString, ':');
    if (!pszSeparator)
    {
        return FALSE;
    }



    char *pszComma = strchr(pszHyperlinkString, ',');
    if (!pszComma || pszSeparator < pszComma)
    {
        return FALSE;
    }

    BOOL bATOHSuccess = FALSE;
    GameBalanceType eGBType = (GameBalanceType)atoh(pszHyperlinkString, bATOHSuccess);
    if (!bATOHSuccess || eGBType >= 52) // NUM_GBTYPES (GameBalanceDefinition.h:34 enum; not in the src subset)
    {
        return FALSE;
    }

    GBID gbid = atoh(pszComma+1, bATOHSuccess);
    if (!bATOHSuccess ||
        !GBIDExists(gbid, eGBType))
    {
        return FALSE;
    }


    hGB.set_game_balance_type(eGBType);
    hGB.set_gbid(gbid);

    SNO snoActor = GameBalanceGetActorSNO(GBHandle(hGB));
    if (!SNOFileExists(ACTOR_SNO, snoActor))
    {
        return FALSE;
    }


    pszHyperlinkString = pszSeparator+1;
    return TRUE;
}



static BOOL sParseOptionalNumber(
    char *& pszHyperlinkString,
    int32 & nNumber)
{

    char *pszSeparator = strchr(pszHyperlinkString, ':');
    if (!pszSeparator)
    {
        return FALSE;
    }

    if (pszHyperlinkString == pszSeparator)
    {
        nNumber = -1;
    }
    else
    {
        BOOL bATOHSuccess = FALSE;
        nNumber = atoh(pszHyperlinkString, bATOHSuccess);
        if (!bATOHSuccess)
        {
            return FALSE;
        }
    }
    pszHyperlinkString = pszSeparator+1;
    return TRUE;
}



static BOOL sParseNumber(
    char *& pszHyperlinkString,
    int32 & nNumber)
{

    char *pszSeparator = strchr(pszHyperlinkString, ':');
    if (!pszSeparator)
    {
        return FALSE;
    }

    BOOL bATOHSuccess = FALSE;
    nNumber = atoh(pszHyperlinkString, bATOHSuccess);
    if (!bATOHSuccess)
    {
        return FALSE;
    }
    pszHyperlinkString = pszSeparator+1;
    return TRUE;
}






static int sParseVariableCommaSeparatedInts(
    char *& pszIntString,
    google::protobuf::RepeatedField<int> & tIntList)
{

    char *pszSeparator = strchr(pszIntString, ':');
    if (!pszSeparator)
    {
        return -1;
    }

    char *pszCurrentPos = pszIntString;
    while (pszCurrentPos < pszSeparator)
    {
        BOOL bATOHSuccess = FALSE;
        int32 nVal = atoh(pszCurrentPos, bATOHSuccess);
        if (!bATOHSuccess)
        {
            return -1;
        }

        tIntList.Add(nVal);
        char *pszComma = strchr(pszCurrentPos, ',');
        if (!pszComma)
        {
            break;
        }
        pszCurrentPos = pszComma+1;
    }
    pszIntString = pszSeparator+1;

    return tIntList.size();
}



static BOOL sParseItemBaseAffixes(
    char *& pszBaseAffixes,
    google::protobuf::RepeatedField<int> & tBaseAffixes)
{
    int32 nNumAffixesParsed = sParseVariableCommaSeparatedInts(pszBaseAffixes, tBaseAffixes);
    if (nNumAffixesParsed == -1)
    {
        return FALSE;
    }


    for (int32 i = 0; i < tBaseAffixes.size(); i++)
    {
        if (tBaseAffixes.Get(i) != GBID_NONE && !GBIDExists(tBaseAffixes.Get(i), (GameBalanceType)8)) // GB_AFFIXES
        {
            return FALSE;
        }
    }

    return TRUE;
}



// NON_MATCHING: 3024 bytes both; only block placement of the 919/925/929 validation returns and the final hash-compare tail differ (original keeps the shared FALSE tail after line 925). Tried: combined hash condition (worse).
static BOOL sPopulateItemGeneratorStructs(
    const char *& pszItemString,
    const BOOL fSocketedItem,
    google::protobuf::RepeatedField<D3::Items::Generator> & generatorList)
{

    D3::Items::Generator tGenerator;
    ItemGeneratorToDefaults(tGenerator);

    char *pszCurrentPos = (char*)pszItemString;


    if (!sParseItemGBHandle(pszCurrentPos, *tGenerator.mutable_gb_handle()))
    {
        return FALSE;
    }


    int32 nSeed;
    if (!sParseNumber(pszCurrentPos, nSeed))
    {
        return FALSE;
    }


    if (!sParseItemBaseAffixes(pszCurrentPos, *tGenerator.mutable_base_affixes()))
    {
        return FALSE;
    }


    GBID gbidEnchantedAffixOld;
    GBID gbidEnchantedAffixNew;
    int32 nEnchantedAffixSeed;
    if (!sParseOptionalNumber(pszCurrentPos, gbidEnchantedAffixOld) ||
        !sParseOptionalNumber(pszCurrentPos, gbidEnchantedAffixNew) ||
        !sParseOptionalNumber(pszCurrentPos, nEnchantedAffixSeed))
    {
        return FALSE;
    }

    int32 nCubeEnchantedGemRank;
    int32 nCubeEnchantedGemType;
    if (!sParseOptionalNumber(pszCurrentPos, nCubeEnchantedGemRank) ||
        !sParseOptionalNumber(pszCurrentPos, nCubeEnchantedGemType))
    {
        return FALSE;
    }


    GBID gbidTransmog;
    if (!sParseOptionalNumber(pszCurrentPos, gbidTransmog))
    {
        return FALSE;
    }

    int32 nJewelRank;
    if (!sParseOptionalNumber(pszCurrentPos, nJewelRank))
    {
        return FALSE;
    }

    uint32 dwItemQualityLevel;
    if (!sParseNumber(pszCurrentPos, (int32 &)dwItemQualityLevel))
    {
        return FALSE;
    }

    RareItemName tRareItemName;
    if (dwItemQualityLevel >= IQL_RARE_AFFIX_START && dwItemQualityLevel <= IQL_RARE_AFFIX_END)
    {


        if (!sParseNumber(pszCurrentPos, tRareItemName.fItemNameIsPrefix))
        {
            return FALSE;
        }

        if (!sParseNumber(pszCurrentPos, tRareItemName.snoAffixStringList))
        {
            return FALSE;
        }

        if (!sParseNumber(pszCurrentPos, tRareItemName.nAffixStringListIndex))
        {
            return FALSE;
        }

        if (!sParseNumber(pszCurrentPos, tRareItemName.nItemStringListIndex))
        {
            return FALSE;
        }
    }
    else
    {
        tRareItemName.Initialize();
    }


    uint32 dwFlags;
    if (!sParseNumber(pszCurrentPos, (int32 &)dwFlags))
    {
        return FALSE;
    }

    uint32 dwDurability;
    if (!sParseNumber(pszCurrentPos, (int32 &)dwDurability))
    {
        return FALSE;
    }

    uint32 dwMaxDurability;
    if (!sParseNumber(pszCurrentPos, (int32 &)dwMaxDurability))
    {
        return FALSE;
    }

    uint32 dwStackSize;
    if (!sParseNumber(pszCurrentPos, (int32 &)dwStackSize))
    {
        return FALSE;
    }

    uint32 dwSavedDyeType;
    if (!sParseNumber(pszCurrentPos, (int32 &)dwSavedDyeType))
    {
        return FALSE;
    }

    int32 nLegendaryItemLevel;
    if (!sParseNumber(pszCurrentPos, nLegendaryItemLevel))
    {
        return FALSE;
    }

    GBID gbidLegendaryBaseItem;
    if (!sParseOptionalNumber(pszCurrentPos, gbidLegendaryBaseItem))
    {
        return FALSE;
    }

    int32 nNumSocketedItems;
    if (!sParseNumber(pszCurrentPos, nNumSocketedItems))
    {
        return FALSE;
    }

    if (tRareItemName.snoAffixStringList != SNO_NONE)
    {
        if (!SNOFileExists(STRINGLIST_SNO, tRareItemName.snoAffixStringList) || tRareItemName.nAffixStringListIndex < 0)
        {
            return FALSE;
        }




        if (tRareItemName.nItemStringListIndex < 0)
        {
            return FALSE;
        }
    }

    if (dwFlags >> IGF_COUNT)
    {
        return FALSE;
    }
    if (dwItemQualityLevel >= IQL_NUM_ITEM_QUALITY_LEVELS || dwDurability > dwMaxDurability || dwSavedDyeType >= MAX_SAVED_DYE_VALUE || nNumSocketedItems < 0)
    {
        return FALSE;
    }













    const D3::Items::Generator &tDefault = D3::Items::Generator::default_instance();

    tGenerator.set_seed(nSeed);

    if (gbidEnchantedAffixOld != GBID_NONE && gbidEnchantedAffixNew != GBID_NONE)
    {

        if (!GBIDExists(gbidEnchantedAffixOld, (GameBalanceType)8) || // GB_AFFIXES
            !GBIDExists(gbidEnchantedAffixNew, (GameBalanceType)8)) // GB_AFFIXES
        {
            return FALSE;
        }

        tGenerator.set_enchanted_affix_old(gbidEnchantedAffixOld);
        tGenerator.set_enchanted_affix_new(gbidEnchantedAffixNew);
        tGenerator.set_enchanted_affix_seed(nEnchantedAffixSeed);
    }

    if (nCubeEnchantedGemRank >= 0 && nCubeEnchantedGemType != 0)
    {

        tGenerator.set_cube_enchanted_gem_rank(nCubeEnchantedGemRank);
        tGenerator.set_cube_enchanted_gem_type(nCubeEnchantedGemType);
    }

    if (gbidTransmog != GBID_NONE)
    {
        if (!GBIDExists(gbidTransmog, GB_ITEMS))
        {
            return FALSE;
        }

        tGenerator.set_transmog_gbid(gbidTransmog);
    }

    if (ItemIsUpgradeableJewel(tGenerator.gb_handle().gbid()))
    {
        tGenerator.set_jewel_rank(nJewelRank);
    }

    tRareItemName.Export(tGenerator);
    tGenerator.set_flags(tGenerator.flags() | dwFlags);
    tGenerator.set_durability(dwDurability);

    if (dwMaxDurability != tDefault.max_durability())
    {
        tGenerator.set_max_durability(dwMaxDurability);
    }
    tGenerator.set_stack_size(dwStackSize);

    if (dwSavedDyeType != tDefault.dye_type())
    {
        tGenerator.set_dye_type(dwSavedDyeType);
    }
    if (dwItemQualityLevel != tDefault.item_quality_level())
    {
        tGenerator.set_item_quality_level(dwItemQualityLevel);
    }

    if (nLegendaryItemLevel != 0)
    {
        tGenerator.set_legendary_item_level(nLegendaryItemLevel);
    }

    if (gbidLegendaryBaseItem != GBID_NONE)
    {
        if (!GBIDExists(gbidLegendaryBaseItem, GB_ITEMS))
        {
            return FALSE;
        }

        tGenerator.set_legendary_base_item_gbid(gbidLegendaryBaseItem);
    }

    uint32 uSocketCount = ItemSocketCountFromGenerator(tGenerator);
    if (uSocketCount > 3)
    {
        return FALSE;
    }


    generatorList.Add(tGenerator);

    for (int32 i = 0; i < nNumSocketedItems; i++)
    {
        if (!sPopulateItemGeneratorStructs((const char *&)pszCurrentPos, TRUE, generatorList))
        {
            return FALSE;
        }
    }


    if (!fSocketedItem)
    {
        CRefString sStringToHash(pszItemString, (int32)(pszCurrentPos - pszItemString));
        uint32 dwHashCheck = (uint32)XHashKey<LPCSTR>(sStringToHash.str());
        uint32 dwHash;
        if (!sParseNumber(pszCurrentPos, (int32 &)dwHash))
        {
            return FALSE;
        }
        if (dwHash != dwHashCheck)
        {
            return FALSE;
        }
    }


    pszItemString = pszCurrentPos;
    return TRUE;

}



static BOOL sParseGameAccount(
    const char *& pszEntityIdString,
    OnlineService::GameAccountHandle & tGameAccount)
{

    if (!pszEntityIdString)
        return FALSE;




    LPCSTR pszSeparator = strchr(pszEntityIdString, '-');
    if (!pszSeparator)
        return FALSE;


    CHAR buffer[128];
    strncpy(buffer, pszEntityIdString, pszSeparator - pszEntityIdString);
    buffer[pszSeparator - pszEntityIdString] = 0;

    tGameAccount.m_id = static_cast<OnlineService::GameAccountId>(strtoull(buffer, NULL, 16));
    pszEntityIdString = pszSeparator + 1;

    pszSeparator = strchr(pszEntityIdString, '-');
    if (!pszSeparator)
        return FALSE;


    strncpy(buffer, pszEntityIdString, pszSeparator - pszEntityIdString);
    buffer[pszSeparator - pszEntityIdString] = 0;
    // Inference: the original assigns XFourCC(uint32) (copy operator= inlined at 1091); src XFourCC.h lacks that inline ctor, operator=(uint32) stores the same value.
    tGameAccount.m_program = static_cast<uint32>(strtoull(buffer, NULL, 16));
    pszEntityIdString = pszSeparator + 1;
    LPCSTR pszHeaderClose = strchr(pszEntityIdString, '|');
    if (!pszHeaderClose ||
        !sIsHyperlinkTrailer(pszHeaderClose, FALSE))
        return FALSE;


    strncpy(buffer, pszEntityIdString, pszHeaderClose - pszEntityIdString);
    buffer[pszHeaderClose - pszEntityIdString] = 0;

    tGameAccount.m_region = static_cast<uint32>(strtoull(buffer, NULL, 16));
    pszEntityIdString = pszHeaderClose;




    return !tGameAccount.IsInvalid();

}





























































static BOOL sParseChatId(
    const char *& pszEntityIdString,
    OnlineService::ChatId & id)
{

    if (!pszEntityIdString)
        return FALSE;





    LPCSTR pszHeaderClose = strchr(pszEntityIdString, '|');
    if (!pszHeaderClose ||
        !sIsHyperlinkTrailer(pszHeaderClose, FALSE))
        return FALSE;



    CHAR buffer[128];
    LPCSTR pszSeparator = strchr(pszEntityIdString, '-');
    if (!pszSeparator || pszSeparator > pszHeaderClose)
    {

        // Inference: a bare internal channel name (no '-') up to the trailer.
        if (pszEntityIdString == pszHeaderClose)
            return FALSE;





        size_t lenInternalName = pszHeaderClose - pszEntityIdString;
        if (lenInternalName + 1 > sizeof(buffer))
            return FALSE;



        Blizzard::String::Copy(buffer, pszEntityIdString, lenInternalName + 1);
        id = OnlineService::ChatId(buffer);
        return TRUE;
    }
    else
    {
        OnlineService::ChannelId tChannelId;
        strncpy(buffer, pszEntityIdString, pszSeparator - pszEntityIdString);
        buffer[pszSeparator - pszEntityIdString] = 0;

        tChannelId.m_id = static_cast<uint32>(strtoull(buffer, NULL, 16));
        pszEntityIdString = pszSeparator + 1;

        pszSeparator = strchr(pszEntityIdString, '-');
        strncpy(buffer, pszEntityIdString, pszSeparator - pszEntityIdString);
        buffer[pszSeparator - pszEntityIdString] = 0;

        tChannelId.m_type = static_cast<uint32>(strtoull(buffer, NULL, 16));
        pszEntityIdString = pszSeparator + 1;

        pszSeparator = strchr(pszEntityIdString, '-');
        strncpy(buffer, pszEntityIdString, pszSeparator - pszEntityIdString);
        buffer[pszSeparator - pszEntityIdString] = 0;

        tChannelId.m_label = static_cast<uint32>(strtoull(buffer, NULL, 16));
        pszEntityIdString = pszSeparator + 1;

        pszHeaderClose = strchr(pszEntityIdString, '|');
        if (!pszHeaderClose ||
            !sIsHyperlinkTrailer(pszHeaderClose, FALSE))
            return FALSE;


        strncpy(buffer, pszEntityIdString, pszHeaderClose - pszEntityIdString);
        buffer[pszHeaderClose - pszEntityIdString] = 0;

        tChannelId.m_epoch = static_cast<uint32>(strtoull(buffer, NULL, 16));
        pszEntityIdString = pszHeaderClose;



        id = HubGet().ClientServiceGet().Chat().VSlot1(tChannelId);







        return !id.IsInvalid();
    }

}

























































































































static BOOL sParseGroupId(
    const char *& pszEntityIdString,
    GroupId & id)
{

    if (!pszEntityIdString)
        return FALSE;




    LPCSTR pszHeaderClose = strchr(pszEntityIdString, '|');
    if (!pszHeaderClose ||
        !sIsHyperlinkTrailer(pszHeaderClose, FALSE))
        return FALSE;


    CHAR buffer[128];
    strncpy(buffer, pszEntityIdString, pszHeaderClose - pszEntityIdString);
    buffer[pszHeaderClose - pszEntityIdString] = 0;
    id = strtoull(buffer, NULL, 16);
    pszEntityIdString = pszHeaderClose;

    return id != 0;
}







static BOOL sParseAchievementData(
    const char *& pszAchievementDataString,
    HyperlinkAchievementData & tHyperlinkAchievementData)
{

    if (!pszAchievementDataString)
        return FALSE;





    LPCSTR pszSeparator = strchr(pszAchievementDataString, '-');
    if (!pszSeparator)
        return FALSE;


    CHAR buffer[128];
    strncpy(buffer, pszAchievementDataString, pszSeparator - pszAchievementDataString);
    buffer[pszSeparator - pszAchievementDataString] = 0;
    tHyperlinkAchievementData.tAchievementId = strtoull(buffer, NULL, 16);


    pszAchievementDataString = pszSeparator + 1;
    pszSeparator = strchr(pszAchievementDataString, '-');
    if (!pszSeparator)
        return FALSE;


    strncpy(buffer, pszAchievementDataString, pszSeparator - pszAchievementDataString);
    buffer[pszSeparator - pszAchievementDataString] = 0;
    tHyperlinkAchievementData.tGameAccount.m_id = static_cast<OnlineService::GameAccountId>(strtoull(buffer, NULL, 16));


    pszAchievementDataString = pszSeparator + 1;
    pszSeparator = strchr(pszAchievementDataString, '-');
    if (!pszSeparator)
        return FALSE;


    strncpy(buffer, pszAchievementDataString, pszSeparator - pszAchievementDataString);
    buffer[pszSeparator - pszAchievementDataString] = 0;
    tHyperlinkAchievementData.tGameAccount.m_program = XFourCC(buffer);


    pszAchievementDataString = pszSeparator + 1;
    LPCSTR pszHeaderClose = strchr(pszAchievementDataString, '|');
    if (!pszHeaderClose ||
        !sIsHyperlinkTrailer(pszHeaderClose, FALSE))
        return FALSE;


    strncpy(buffer, pszAchievementDataString, pszHeaderClose - pszAchievementDataString);
    buffer[pszHeaderClose - pszAchievementDataString] = 0;
    tHyperlinkAchievementData.tGameAccount.m_region = static_cast<uint32>(strtoull(buffer, NULL, 16));

    pszAchievementDataString = pszHeaderClose;

    const OnlineService::AchievementDefinition* ptAchievementDef = HubGet().ClientServiceGet().Achievements().GetAchievementDefinition(
        tHyperlinkAchievementData.tAchievementId);
    return (tHyperlinkAchievementData.tAchievementId != OnlineService::INVALID_ACHIEVEMENT_ID && ptAchievementDef != NULL &&

        !tHyperlinkAchievementData.tGameAccount.IsInvalid());




}










































































static inline BOOL sParseURL(const char *& pszURL, CRefString & sURL)
{
    if (!pszURL)
        return FALSE;

    LPCSTR pszHeaderClose = strchr(pszURL, '|');
    if (!pszHeaderClose || !sIsHyperlinkTrailer(pszHeaderClose, FALSE))
        return FALSE;

    sURL = CRefString(pszURL, (int32)(pszHeaderClose - pszURL));
    return TRUE;
}














// NON_MATCHING: 1100 vs 1196 bytes; now emitted standalone (HyperlinkValidate is the second caller). The original unrolls the 6-name search with constant strncasecmp lengths and an attribute state switch; this loop shape does not.
static BOOL sParseStoreItem(
    const char *& pszURLString,
    OnlineService::StoreItem & tItem)
{
    // Inference: original null guard and false exit precede all string accesses.
    if (!pszURLString)
        return FALSE;



    LPCSTR pszHeaderClose = strchr(pszURLString, '|');
    if (!pszHeaderClose ||
        !sIsHyperlinkTrailer(pszHeaderClose, FALSE))
        return FALSE;


    // Inference: scan starts at the input and advances over recognized values or one unmatched byte.
    const char * pszLinkAttributes = pszURLString;
    while (true)
    {
        LPCSTR szSeparator = strchr(pszLinkAttributes, ':');
        if (!szSeparator || pszLinkAttributes >= pszHeaderClose)
            break;



        // Inference: false flag/zero index and name-length guard reconstruct the original unrolled search.
        bool bMatchFound = false;
        for (int32 i = 0; i < 6; ++i)
        {
            if (strlen(cguszStoreProductURLAttributeNames[i]) == static_cast<size_t>(szSeparator - pszLinkAttributes) &&
                strncasecmp(pszLinkAttributes, cguszStoreProductURLAttributeNames[i], szSeparator - pszLinkAttributes) == 0)
            {
                bMatchFound = true;
                pszLinkAttributes = szSeparator + 1;
                StoreProductURLAttributes eAttribute = static_cast<StoreProductURLAttributes>(i);
                switch (eAttribute)
                {
                    case SPUA_CATEGORY:
                    {
                        int32 nBytesRead = 0;
                        sscanf(pszLinkAttributes, "%d%n", &tItem.m_eCategory, &nBytesRead);
                        pszLinkAttributes += nBytesRead;
                        break;
                    }
                    case SPUA_ATTRIBUTE:
                    { // Inference: first attribute has no comma; later entries consume their comma.
                        bool bFirstParsed = false;
                        do
                        {
                            int32 nBytesRead = 0;
                            OnlineService::StoreProductAttributes eProductAttribute = OnlineService::STORE_ATTRIBUTE_INVALID;
                            if (!bFirstParsed)
                            {
                                sscanf(pszLinkAttributes, "%d%n", &eProductAttribute, &nBytesRead);
                                bFirstParsed = true;
                            }
                            else
                                sscanf(pszLinkAttributes, ",%d%n", &eProductAttribute, &nBytesRead);

                            pszLinkAttributes += nBytesRead;
                            if (eProductAttribute != OnlineService::STORE_ATTRIBUTE_INVALID)
                            {
                                tItem.m_arAttributes.push_back(eProductAttribute);
                            }
                        }
                        while (*pszLinkAttributes == ',');
                        break;
                    }
                    case SPUA_CONTENT_LICENSE:
                    { // Inference: original stores use the license type and invalid sentinel before parsing.
                        int32 nBytesRead = 0;
                        OnlineService::StoreItemEntitlement tEntitlement;
                        tEntitlement.m_eType = OnlineService::STORE_ENTITLEMENT_LICENSE;
                        tEntitlement.m_data.m_contentLicense = OnlineService::CONTENT_LICENSE_INVALID;
                        sscanf(pszLinkAttributes, "%d%n", &tEntitlement.m_data.m_contentLicense, &nBytesRead);
                        if (tEntitlement.m_data.m_contentLicense != OnlineService::CONTENT_LICENSE_INVALID)
                        {
                            tItem.m_arEntitlements.push_back(tEntitlement);
                        }
                        pszLinkAttributes += nBytesRead;
                        break;
                    }
                    case SPUA_GBID:
                    { // Inference: original stores use the item type and invalid GBID before parsing.
                        int32 nBytesRead = 0;
                        OnlineService::StoreItemEntitlement tEntitlement;
                        tEntitlement.m_eType = OnlineService::STORE_ENTITLEMENT_ITEM;
                        tEntitlement.m_data.m_gbidItem = GBID_NONE;
                        sscanf(pszLinkAttributes, "%d%n", &tEntitlement.m_data.m_gbidItem, &nBytesRead);
                        if (tEntitlement.m_data.m_gbidItem != GBID_NONE)
                        {
                            tItem.m_arEntitlements.push_back(tEntitlement);
                        }
                        pszLinkAttributes += nBytesRead;
                        break;
                    }
                    case SPUA_PRODUCT_ID:
                    { // Inference: product ID output starts at zero before sscanf.
                        int32 nBytesRead = 0;
                        uint32 productId = 0;
                        sscanf(pszLinkAttributes, "%u%n", &productId, &nBytesRead);
                        tItem.m_productId = productId;
                        pszLinkAttributes += nBytesRead;
                        break;
                    }
                    case SPUA_PLATINUM_AMOUNT:
                    { // Inference: nonzero platinum amount is appended after parsing.
                        int32 nBytesRead = 0;
                        OnlineService::StoreItemEntitlement tEntitlement;
                        tEntitlement.m_eType = OnlineService::STORE_ENTITLEMENT_PLATINUM;

                        sscanf(pszLinkAttributes, "%llu%n", &tEntitlement.m_data.m_platinumAmount, &nBytesRead);
                        if (tEntitlement.m_data.m_platinumAmount != 0)
                        {
                            tItem.m_arEntitlements.push_back(tEntitlement);
                        }
                        pszLinkAttributes += nBytesRead;
                        break;
                    }
                }
                break;
            }
        }




        // Inference: unmatched attribute bytes advance individually, as in the original shared tail.
        if (!bMatchFound)
        {
            ++pszLinkAttributes;
        }
    }
    if (pszLinkAttributes == pszHeaderClose)
    {
        pszURLString = pszLinkAttributes;
        return TRUE;
    }

    return FALSE;
}







































































































































































































































































































































































#include "../Client/UserInterface/Controls/UIToolTipControl.h"
#include "UserInterface/UIInventory.h"
#include "../Client/UserInterface/UIID.h"
void ItemSetBoundToACD(ActorCommonData & tACD, const ACDID idACDOwner);  // X/Common/Items.cpp:5565
void ACDAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, const BOOL bMarkItemAsDirty, InventorySlotType eCameFrom);  // X/Common/ActorInventory.cpp:3969
namespace UIInventory { void GetItemTooltipParameters(ItemTooltipParameterBlock & tParams, const ActorCommonData & tACDItem, const ItemTooltipPlayerData & tTooltipPlayerData, const BOOL bConsoleInventory, const BOOL bShowAsRepaired); }  // X/Client/UserInterface/Interface/UIInventory.cpp:5260
namespace UIInventory { void InitItemTooltip(ItemTooltipData & tData, const ACDID idACD, const GBID gbid, const ItemTooltipParameterBlock & tParams, const BOOL bCalculateComparisonData, ActorCommonData * ptACDPlayer, const BOOL bDisableSocketText); }  // X/Client/UserInterface/Interface/UIInventory.cpp:5705

BOOL CHyperlinkPopulateItemTooltip(
    const char * pszRawHyperlinkString,
    ActorCommonData * ptACDPlayer,
    UIToolTip & tTip)
{
    const char * pszStringStart = pszRawHyperlinkString;
    char * pszCurrentPos = (char *)pszStringStart;


    HyperlinkType eHLType = HyperlinkParseType((const char *&)pszCurrentPos, TRUE);
    if (eHLType != HLT_ITEM)
        return FALSE;





    google::protobuf::RepeatedField<D3::Items::Generator> generatorList;
    if (!sPopulateItemGeneratorStructs((const char *&)pszCurrentPos, FALSE, generatorList))
        return FALSE;





    // Inference: network name 8192 is MAX_ACTORS, the only CU enumerator of that value (as in CHyperlinkGetItemName).
    ACDID idACDItem = CItemGenerate((ACDNetworkName)MAX_ACTORS, generatorList.Get(0), NULL);
    if (idACDItem == ACDID_INVALID)
        return FALSE;


    ActorCommonData & tACDItem = ACDGet(idACDItem);


    if (generatorList.Get(0).flags() & (1 << IGF_BOUND_BIT))
        // Inference: the original passes the item's network name (0x2000) as the owner.
        ItemSetBoundToACD(tACDItem, (ACDID)MAX_ACTORS);



    for (int32 i = 1; i < generatorList.size(); i++)
    {
        ACDID idSocketedItem = CItemGenerate((ACDNetworkName)(MAX_ACTORS + i), generatorList.Get(i), NULL);
        if (idSocketedItem == ACDID_INVALID)
            continue;








        ActorCommonData & tACDToSocket = ACDGet(idSocketedItem);
        InventoryLocation tLocation;
        tLocation.eSlot = SLOT_ITEM_SOCKETS;
        tLocation.idACDContainer = idACDItem;
        tLocation.pt.Set(0, i - 1);
        ACDAddToInventory(tACDToSocket, tLocation, FALSE, SLOT_NONE);
        ItemAddSocketedItemAttributes(tACDToSocket, tACDItem);

        FastAttribMerge(tACDItem.idFastAttrib, tACDToSocket.idFastAttrib);


        if (generatorList.Get(i).flags() & (1 << IGF_BOUND_BIT))
            // Inference: the owner passed is the socketed item's network name, as at line 2131.
            ItemSetBoundToACD(tACDToSocket, (ACDID)(MAX_ACTORS + i));
    }

    if (generatorList.size() > 1)
    {

        FastAttribSetValueInt(tACDItem.idFastAttrib, ATTRIBUTE_SOCKETS_FILLED, generatorList.size() - 1);
    }


    ItemTooltipParameterBlock tParams;
    UIInventory::GetItemTooltipParameters(tParams, tACDItem, ItemTooltipPlayerData(ptACDPlayer, NULL), FALSE, FALSE);

    tParams.dwTooltipFlags |= 1 << TOOLTIP_CREATE_CLOSE_BUTTON;

    ItemTooltipData tData;
    UIInventory::InitItemTooltip(tData, idACDItem, tACDItem.hGB.gbid, tParams, TRUE, ptACDPlayer, FALSE);

    tTip.SetItemTooltipData(GetUIIDNone(), tData, TRUE);


    for (int32 i = generatorList.size(); i > 0; i--)
    {
        ACDID idACDItemToDelete = ACDFromANN((ACDNetworkName)(MAX_ACTORS + i - 1));
        ACDDelete(idACDItemToDelete, FALSE);
    }
    return TRUE;

}

























































// LINKER-REMOVED: DWARF 2248 (low_pc 0, 0x64 bytes); written as a mirror of CHyperlinkGetAchievementData; it is the second caller that keeps its static parser out of line.
BOOL CHyperlinkGetChatChannel(
    LPCSTR szHyperLink,
    OnlineService::ChatId & id)
{

    const char * pszCurrentPos = szHyperLink;

    // Inference: linker-dropped (DWARF low_pc 0, 100 bytes); written as the CHyperlinkGetAchievementData mirror.
    HyperlinkType eHLType = HyperlinkParseType(pszCurrentPos, TRUE);
    if (eHLType == HLT_NONE)
        return FALSE;

    if (eHLType == HLT_CHAT_CHANNEL)
    {



        LPCSTR pszEntityId = pszCurrentPos;
        if (sParseChatId(pszEntityId, id))
            return TRUE;
    }

    return FALSE;
}

















































































































































CRefString CHyperlinkCreateFromGameAccountHandle(const OnlineService::GameAccountHandle & tGameAccount, CRefString sDisplayName)
{
    CRefString sResult;
    return sResult.Format("|H%s:%x-%x-%x|h%s|h", "OnlUserHdl", tGameAccount.m_id, tGameAccount.m_program.GetValue(), tGameAccount.m_region, sDisplayName.str());
}




// LINKER-REMOVED: DWARF 2427 (low_pc 0, 0x64 bytes); written as a mirror of CHyperlinkGetAchievementData; it is the second caller that keeps its static parser out of line.
BOOL CHyperlinkGetGameAccountHandle(
    LPCSTR szHyperLink,
    OnlineService::GameAccountHandle & tGameAccount)
{

    const char * pszCurrentPos = szHyperLink;

    // Inference: linker-dropped (DWARF low_pc 0, 100 bytes); written as the CHyperlinkGetAchievementData mirror.
    HyperlinkType eHLType = HyperlinkParseType(pszCurrentPos, TRUE);
    if (eHLType == HLT_NONE)
        return FALSE;

    if (eHLType == HLT_ONLINE_USER)
    {



        LPCSTR pszEntityId = pszCurrentPos;
        if (sParseGameAccount(pszEntityId, tGameAccount))
            return TRUE;
    }

    return FALSE;
}














































BOOL CHyperlinkGetAchievementData(
    LPCSTR szHyperLink,
    HyperlinkAchievementData & tHyperlinkAchievementData)
{

    const char * pszCurrentPos = szHyperLink;


    HyperlinkType eHLType = HyperlinkParseType(pszCurrentPos, TRUE);
    if (eHLType == HLT_NONE)
        return FALSE;

    if (eHLType == HLT_ACHIEVEMENT)
    {


        LPCSTR pszAchievementData = pszCurrentPos;
        if (sParseAchievementData(pszAchievementData, tHyperlinkAchievementData))
            return TRUE;
    }

    return FALSE;












}



BOOL CHyperlinkGetItemName(
    LPCSTR szHyperLink,
    CRefString & sItemName,
    BOOL fIncludeBrackets)
{

    const char * pszCurrentPos = szHyperLink;


    HyperlinkType eHLType = HyperlinkParseType(pszCurrentPos, TRUE);
    if (eHLType == HLT_ITEM)
    {
        google::protobuf::RepeatedField<D3::Items::Generator> generatorList;
        if (!sPopulateItemGeneratorStructs(pszCurrentPos, FALSE, generatorList))
            return FALSE;





        // Inference: network name 8192 is MAX_ACTORS, the only CU enumerator of that value.
        ACDID idACDItem = CItemGenerate((ACDNetworkName)MAX_ACTORS, generatorList.Get(0), NULL);
        if (idACDItem == ACDID_INVALID)
            return FALSE;


            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (fIncludeBrackets)
            {
                RGBAColor rgbaItemColor = ItemGetDisplayNameColor(tACDItem);
                AncientRank eAncientRank = ANCIENT_RANK_NONE;
                if (!ItemIsUnidentified(tACDItem))
                {
                    eAncientRank = ItemGetAncientRank(tACDItem);
                }

                CRefString sFormat;
                switch (eAncientRank)
                {
                    case ANCIENT_RANK_NONE:
                        sFormat = StringListFetch("Bnet_Chat:HyperlinkFormatItem");
                        break;
                    case ANCIENT_RANK_ANCIENT:
                        sFormat = StringListFetch("Bnet_Chat:HyperlinkFormatAncientItem");
                        break;
                    case ANCIENT_RANK_PRIMAL_ANCIENT:
                        sFormat = StringListFetch("Bnet_Chat:HyperlinkFormatPrimalAncientItem");
                        break;
                }



                sItemName = LocalizedSprintf(sFormat.str(), ItemGetDisplayName(tACDItem, FALSE, FALSE, FALSE, TRUE).str(), NULL);
                Set_Text_Color(sItemName, rgbaItemColor);
            }
            else
            {
                sItemName = ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE);
            }
            ACDDelete(idACDItem, FALSE);
            return TRUE;
    }

    return FALSE;
}













BOOL CHyperlinkGetURL(LPCSTR szHyperLink, CRefString & sURL)
{
    const char * pszCurrentPos = szHyperLink;
    HyperlinkType eHLType = HyperlinkParseType(pszCurrentPos, TRUE);
    if (eHLType == HLT_URL)
    {
        LPCSTR pszURL = pszCurrentPos;
        if (sParseURL(pszURL, sURL))
            return TRUE;
    }
    return FALSE;
}













































































BOOL CHyperlinkGetStoreItem(
    LPCSTR szHyperLink,
    OnlineService::StoreItem & tItem)
{

    const char * pszCurrentPos = szHyperLink;

    // Inference: original type checks and parser success branch return normalized BOOL.
    HyperlinkType eHLType = HyperlinkParseType(pszCurrentPos, TRUE);
    if (eHLType == HLT_NONE)
        return FALSE;

    if (eHLType == HLT_STORE_PRODUCT)
    {



        LPCSTR pszURL = pszCurrentPos;
        if (sParseStoreItem(pszURL, tItem))
            return TRUE;
    }

    return FALSE;


}







CRefString HyperlinkCreateFromSNO(const SNOName & snoname, LPCSTR szLinkName)
{
    CRefString sEncodedString;
    CRefString sLinkName;
    if (!szLinkName)
    {
        CRefString sGroup;
        sGroup = snoname.snoGroup == SNOGROUP_NONE ? "<Unknown>" : SNOGroupGetName(snoname.snoGroup);
        sLinkName.Format("%s '%d'", sGroup.str(), snoname.snoHandle);
        szLinkName = sLinkName.str();
    }
    sEncodedString = szLinkName;
    return sEncodedString;
}
