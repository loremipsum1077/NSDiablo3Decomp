// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdAuthCommon.h

#pragma once

#include "bdLobbyServicesCommon.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

enum bdAuthMsgTypes {
    BD_AUTH_CREATE_ACCOUNT_REQUEST = 0,
    BD_AUTH_CREATE_ACCOUNT_REPLY = 1,
    BD_AUTH_CHANGE_USER_KEY_REQUEST = 2,
    BD_AUTH_CHANGE_USER_KEY_REPLY = 3,
    BD_AUTH_RESET_ACCOUNT_REQUEST = 4,
    BD_AUTH_RESET_ACCOUNT_REPLY = 5,
    BD_AUTH_DELETE_ACCOUNT_REQUEST = 6,
    BD_AUTH_DELETE_ACCOUNT_REPLY = 7,
    BD_AUTH_MIGRATE_ACCOUNTS_REQUEST = 8,
    BD_AUTH_MIGRATE_ACCOUNTS_REPLY = 9,
    BD_AUTH_ACCOUNT_FOR_MMP_REQUEST = 10,
    BD_AUTH_ACCOUNT_FOR_MMP_REPLY = 11,
    BD_AUTH_HOST_FOR_MMP_REQUEST = 12,
    BD_AUTH_HOST_FOR_MMP_REPLY = 13,
    BD_AUTH_ACCOUNT_FOR_HOST_REQUEST = 14,
    BD_AUTH_ACCOUNT_FOR_HOST_REPLY = 15,
    BD_AUTH_ANONYMOUS_FOR_MMP_REQUEST = 16,
    BD_AUTH_ANONYMOUS_FOR_MMP_REPLY = 17,
    BD_AUTH_PS3_FOR_MMP_REQUEST = 18,
    BD_AUTH_PS3_FOR_MMP_REPLY = 19,
    BD_AUTH_GET_USERNAMES_BY_LICENSE_REQUEST = 20,
    BD_AUTH_GET_USERNAMES_BY_LICENSE_REPLY = 21,
    BD_AUTH_WII_FOR_MMP_REQUEST = 22,
    BD_AUTH_WII_FOR_MMP_REPLY = 23,
    BD_AUTH_FOR_DEDICATED_SERVER_REQUEST = 24,
    BD_AUTH_FOR_DEDICATED_SERVER_REPLY = 25,
    BD_AUTH_FOR_DEDICATED_SERVER_REQUEST_RSA = 26,
    BD_AUTH_FOR_DEDICATED_SERVER_REPLY_RSA = 27,
    BD_AUTH_STEAM_FOR_MMP_REQUEST = 28,
    BD_AUTH_STEAM_FOR_MMP_REPLY = 29,
    BD_AUTH_3DS_FOR_MMP_REQUEST = 30,
    BD_AUTH_3DS_FOR_MMP_REPLY = 31,
    BD_AUTH_CODO_FOR_MMP_REQUEST = 32,
    BD_AUTH_CODO_FOR_MMP_REPLY = 33,
    BD_AUTH_ABACCOUNTS_FOR_MMP_REQUEST = 34,
    BD_AUTH_ABACCOUNTS_FOR_MMP_REPLY = 35,
    BD_AUTH_WIIU_FOR_MMP_REQUEST = 36,
    BD_AUTH_WIIU_FOR_MMP_REPLY = 37,
    BD_AUTH_WIIU_FOR_MMP_REQUEST2 = 38,
    BD_AUTH_WIIU_FOR_MMP_REPLY2 = 39,
    BD_AUTH_WIIU_SECONDARY_FOR_MMP_REQUEST = 40,
    BD_AUTH_WIIU_SECONDARY_FOR_MMP_REPLY = 41,
    BD_AUTH_XBOX360_FOR_MMP_REQUEST = 42,
    BD_AUTH_XBOX360_FOR_MMP_REPLY = 43,
    BD_AUTH_XBOXONE_FOR_MMP_REQUEST = 44,
    BD_AUTH_XBOXONE_FOR_MMP_REPLY = 45,
    BD_AUTH_PS4_FOR_MMP_REQUEST = 46,
    BD_AUTH_PS4_FOR_MMP_REPLY = 47,
    BD_AUTH_WINPHONE8_FOR_MMP_REQUEST = 48,
    BD_AUTH_WINPHONE8_FOR_MMP_REPLY = 49,
    BD_AUTH_WIN8METRO_FOR_MMP_REQUEST = 50,
    BD_AUTH_WIN8METRO_FOR_MMP_REPLY = 51,
    BD_AUTH_THUNDERHEAD_FOR_MMP_REQUEST = 52,
    BD_AUTH_THUNDERHEAD_FOR_MMP_REPLY = 53,
    BD_AUTH_CODO_FOR_LOGINQUEUE_REQUEST = 56,
    BD_AUTH_CODO_FOR_LOGINQUEUE_REPLY = 57,
    BD_AUTH_APPLEGC_FOR_MMP_REQUEST = 58,
    BD_AUTH_APPLEGC_FOR_MMP_REPLY = 59,
    BD_AUTH_UNO_FOR_MMP_REQUEST = 60,
    BD_AUTH_UNO_FOR_MMP_REPLY = 61,
    BD_AUTH_PS4_FOR_LOGINQUEUE_REQUEST = 62,
    BD_AUTH_PS4_FOR_LOGINQUEUE_REPLY = 63,
    BD_AUTH_XBOXONE_FOR_LOGINQUEUE_REQUEST = 64,
    BD_AUTH_XBOXONE_FOR_LOGINQUEUE_REPLY = 65,
    BD_AUTH_STEAM_FOR_LOGINQUEUE_REQUEST = 66,
    BD_AUTH_STEAM_FOR_LOGINQUEUE_REPLY = 67,
    BD_AUTH_ACCOUNT_FOR_LOGINQUEUE_REQUEST = 68,
    BD_AUTH_ACCOUNT_FOR_LOGINQUEUE_REPLY = 69,
    BD_AUTH_SINGLE_IDENTITY_FOR_MMP_REQUEST = 70,
    BD_AUTH_SINGLE_IDENTITY_FOR_MMP_REPLY = 71,
    BD_AUTH_ATVI_DEDI_SERVER_FOR_MMP_REQUEST = 72,
    BD_AUTH_ATVI_DEDI_SERVER_FOR_MMP_REPLY = 73,
    BD_AUTH_UWP_FOR_MMP_REQUEST = 74,
    BD_AUTH_UWP_FOR_MMP_REPLY = 75,
    BD_AUTH_UWP_FOR_LOGINQUEUE_REQUEST = 76,
    BD_AUTH_UWP_FOR_LOGINQUEUE_REPLY = 77,
    BD_AUTH_SWITCH_FOR_MMP_REQUEST = 78,
    BD_AUTH_SWITCH_FOR_MMP_REPLY = 79,
    BD_AUTH_SWITCH_FOR_LOGINQUEUE_REQUEST = 80,
    BD_AUTH_SWITCH_FOR_LOGINQUEUE_REPLY = 81,
    BD_AUTH_DEVICE_FOR_MMP_REQUEST = 82,
    BD_AUTH_DEVICE_FOR_MMP_REPLY = 83,
    BD_AUTH_BATTLENET_FOR_MMP_REQUEST = 84,
    BD_AUTH_BATTLENET_FOR_MMP_REPLY = 85,
    BD_AUTH_BATTLENET_FOR_LOGINQUEUE_REQUEST = 86,
    BD_AUTH_BATTLENET_FOR_LOGINQUEUE_REPLY = 87,
    BD_AUTH_INVALID_REQUEST = 65535,
};

class bdAuthUtility {  // size 0x1
public:
    static bdUInt64 getLicenseID(const bdNChar8 *);
    static void getLicenseKey(const bdNChar8 *, bdByte8 *);
    static void getUserKey(const bdNChar8 *, bdByte8 *);
    static bdOnlineUserID getUserID(const bdNChar8 *);
    static bdUInt64 createDWIDForWiiU(bdUInt64);
};
static_assert(sizeof(bdAuthUtility) == 0x1, "bdAuthUtility");

class bdAuthCreateAccountPlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt64 m_licenseID;  // 0x00
};
static_assert(sizeof(bdAuthCreateAccountPlainText) == 0x8, "bdAuthCreateAccountPlainText");

class bdAuthCreateAccountCypherText {  // size 0x5c
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt32 m_magicNumber;  // 0x00
    bdNChar8 m_username[64];  // 0x04
    bdByte8 m_userKey[24];  // 0x44
};
static_assert(sizeof(bdAuthCreateAccountCypherText) == 0x5c, "bdAuthCreateAccountCypherText");

class bdAuthChangeUserKeyPlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdOnlineUserID m_userID;  // 0x00
};
static_assert(sizeof(bdAuthChangeUserKeyPlainText) == 0x8, "bdAuthChangeUserKeyPlainText");

class bdAuthChangeUserKeyCypherText {  // size 0x1c
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt32 m_magicNumber;  // 0x00
    bdByte8 m_userKey[24];  // 0x04
};
static_assert(sizeof(bdAuthChangeUserKeyCypherText) == 0x1c, "bdAuthChangeUserKeyCypherText");

class bdAuthResetAccountPlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdOnlineUserID m_userID;  // 0x00
};
static_assert(sizeof(bdAuthResetAccountPlainText) == 0x8, "bdAuthResetAccountPlainText");

class bdAuthResetAccountCypherText {  // size 0x1c
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt32 m_magicNumber;  // 0x00
    bdByte8 m_userKey[24];  // 0x04
};
static_assert(sizeof(bdAuthResetAccountCypherText) == 0x1c, "bdAuthResetAccountCypherText");

class bdAuthDeleteAccountPlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdOnlineUserID m_userID;  // 0x00
};
static_assert(sizeof(bdAuthDeleteAccountPlainText) == 0x8, "bdAuthDeleteAccountPlainText");

class bdAuthDeleteAccountCypherText {  // size 0x4
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt32 m_magicNumber;  // 0x00
};
static_assert(sizeof(bdAuthDeleteAccountCypherText) == 0x4, "bdAuthDeleteAccountCypherText");

class bdAuthMigrateAccountPlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt64 m_licenseID;  // 0x00
};
static_assert(sizeof(bdAuthMigrateAccountPlainText) == 0x8, "bdAuthMigrateAccountPlainText");

class bdAuthMigrateAccountCypherText {  // size 0x10
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt32 m_magicNumber;  // 0x00
    bdTitleID m_oldTitleID;  // 0x04
    bdUInt64 m_oldLicenseID;  // 0x08
};
static_assert(sizeof(bdAuthMigrateAccountCypherText) == 0x10, "bdAuthMigrateAccountCypherText");

class bdAuthUserToServicePlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdOnlineUserID m_userID;  // 0x00
};
static_assert(sizeof(bdAuthUserToServicePlainText) == 0x8, "bdAuthUserToServicePlainText");

class bdAuthHostToServicePlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt64 m_licenseID;  // 0x00
};
static_assert(sizeof(bdAuthHostToServicePlainText) == 0x8, "bdAuthHostToServicePlainText");

class bdAuthUserToHostPlainText {  // size 0x10
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdOnlineUserID m_userID;  // 0x00
    bdUInt64 m_hostLicenseID;  // 0x08
};
static_assert(sizeof(bdAuthUserToHostPlainText) == 0x10, "bdAuthUserToHostPlainText");

class bdAuthGetUsernamesForLicensePlainText {  // size 0x8
public:
    void serialize(void *, const bdUInt) const;
    void deserialize(const void *, const bdUInt);
    bdUInt64 m_licenseID;  // 0x00
};
static_assert(sizeof(bdAuthGetUsernamesForLicensePlainText) == 0x8, "bdAuthGetUsernamesForLicensePlainText");

