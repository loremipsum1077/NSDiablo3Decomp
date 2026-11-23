#include "Contrib/Contrib/DemonWare/bdLobby/bdLinkedAccounts/bdLinkedAccountsResult.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdBitBufferTypes.h"
// Blank lines keep the original line numbers


// NON_MATCHING: tail only. The original keeps `if (ok)` and the 16-clamp as branches (tbz; cmp #15; b.ls)
// with a 4-entry phi; ours early-if-converts both into csels (360 vs 368 bytes). Loops match.
static bdBool deserializeCommon(bdByteBufferRef buffer,
                                bdUserAccountID (&linkedAccountsResult)[16],
                                bdUInt32 & numLinkedAccounts)
{
    bdBool ok = true;

    bdUserAccountID dummyAccount;
    bdUInt32 numUserIDs = 0;
    buffer->readArrayStart(BD_BB_UNSIGNED_INTEGER64_TYPE, numUserIDs);
    for (bdUInt32 i = 0; ok && i < numUserIDs; ++i)
    {
        if (i < 16)
        {
            ok = buffer->readUInt64(linkedAccountsResult[i].m_userID);
        }
        else
        {
            ok = buffer->readUInt64(dummyAccount.m_userID);
        }
    }
    buffer->readArrayEnd();

    bdUInt32 numPlatforms = 0;
    buffer->readArrayStart(BD_BB_SIGNED_CHAR8_STRING_TYPE, numPlatforms);
    for (bdUInt32 i = 0; ok && i < numPlatforms; ++i)
    {
        if (i < 16)
        {
            ok = buffer->readString(linkedAccountsResult[i].m_accountType, sizeof(linkedAccountsResult[i].m_accountType));
        }
        else
        {
            ok = buffer->readString(dummyAccount.m_accountType, sizeof(dummyAccount.m_accountType));
        }
    }
    buffer->readArrayEnd();

    if (numUserIDs != numPlatforms)
    {
        bdLogWarn("lobby/linkedaccounts", "Mismatched number of user IDs and platforms");
        ok = false;
    }

    if (ok)
        numLinkedAccounts = (numUserIDs >= 16) ? 16 : numUserIDs;
    else numLinkedAccounts = 0;
    return ok;
}

bdSetLinkedAccountsResult::bdSetLinkedAccountsResult()
    : m_numLinkedAccounts(0)
{
}

bdSetLinkedAccountsResult::~bdSetLinkedAccountsResult()
{
}

bdBool bdSetLinkedAccountsResult::deserialize(bdByteBufferRef buffer)
{
    return deserializeCommon(buffer, m_linkedAccountsResult, m_numLinkedAccounts);
}

bdUInt bdSetLinkedAccountsResult::sizeOf()
{
    return sizeof(*this);
}

bdLinkedAccountsResult::bdLinkedAccountsResult()
    : m_userAccount(0ULL)
    , m_numLinkedAccounts(0)
{
}

bdLinkedAccountsResult::~bdLinkedAccountsResult()
{
}

bdUInt bdLinkedAccountsResult::sizeOf()
{
    return sizeof(*this);
}

bdBool bdLinkedAccountsResult::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = ContextSerialization::readUserID(buffer, m_userAccount);
    ok = ok && deserializeCommon(buffer, m_linkedAccountsResult, m_numLinkedAccounts);
    return ok;
}
