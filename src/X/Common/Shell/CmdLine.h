// Adopted subset of X/Common/Shell/CmdLine.h; CmdLineParams has every DWARF member at its DWARF offset.
#pragma once
#include "../../Sigma/DataStructures/XBitArray.h"
#include "../../Sigma/UnicodeText.h"
#include "../../Sigma/Sigma.h"

// CmdLine.h:13
enum {
    CMDLINE_NO_COMPILE_BIT = 0,
    CMDLINE_LOAD_ALL_BIT = 1,
    CMDLINE_BLIZZARD_GAME_SERVER = 2,
    CMDLINE_DISABLE_KNOWN_TRACE = 3,
    CMDLINE_BUILD_TOC = 4,
    CMDLINE_BUILD_ASSET_CLASSIFICATION_LISTS = 5,
    CMDLINE_BUILD_SNOGRAPH_AND_PREFETCH = 6,
    CMDLINE_BUILDMOPAQ = 7,
    CMDLINE_LOCALE = 8,
    CMDLINE_COMPILE_GLOBAL_SNOS = 9,
    CMDLINE_COMPILE_LOCALE_SNOS = 10,
    CMDLINE_COMPUTE_CONVERSATION_TIMES = 11,
    CMDLINE_GENERATE_METADATA = 12,
    CMDLINE_FORCE_WINDOWED_MODE_BIT = 13,
    CMDLINE_D3D11_MODE_BIT = 14,
    CMDLINE_NULL_RENDER_BIT = 15,
    CMDLINE_NO_SOUND_BIT = 16,
    CMDLINE_NO_EVICT_BIT = 17,
    CMDLINE_ALLOWCONNECT = 18,
    CMDLINE_DRLG_SEED_BIT = 19,
    CMDLINE_NO_PREFETCH_BIT = 20,
    CMDLINE_PUBLIC_BNET_SERVER_BIT = 21,
    CMDLINE_P4_CLIENT_NAME_BIT = 22,
    CMDLINE_P4_PORT_BIT = 23,
    CMDLINE_P4_USERNAME_BIT = 24,
    CMDLINE_NET_FILE_DEBUGGING_BIT = 25,
    CMDLINE_DUMP_STRINGS_TO_DATABASE_BIT = 26,
    CMDLINE_ASSETSTATS = 27,
    CMDLINE_QA_BIT = 28,
    CMDLINE_HW_LOW_BIT = 29,
    CMDLINE_HW_MEDIUM_BIT = 30,
    CMDLINE_HW_HIGH_BIT = 31,
    CMDLINE_3DV_BIT = 32,
    CMDLINE_HIGH_DPI_CURSOR_BIT = 33,
    CMDLINE_CURSOR_IGNORE_HIGH_DPI_BIT = 34,
    CMDLINE_NO_FPS_LIMIT_BIT = 35,
    CMDLINE_USE_PAGEFLIP_BIT = 36,
    CMDLINE_DEBUG_D3D_BIT = 37,
    CMDLINE_BLIZZCON_BIT = 38,
    CMDLINE_BLIZZCON_DROP_BIT = 39,
    CMDLINE_JOINGAMENAME_BIT = 40,
    CMDLINE_BLIZZCON_MULTIPLAYER_ENABLED_BIT = 41,
    CMDLINE_BLIZZCON_PVP_BIT = 42,
    CMDLINE_USE_ASSETSERVER = 43,
    CMDLINE_DO_NOT_CLEAR_LOOSE_FILES = 44,
    CMDLINE_ENABLE_MEMORY_DEBUG_CHANNEL = 45,
    CMDLINE_DISABLE_CHANGING_WORKING_DIRECTORY = 46,
    CMDLINE_LAUNCH = 47,
    CMDLINE_TEST_FORMAT_JSON_BIT = 48,
    CMDLINE_PRODUCTION_BNET_SERVER = 49,
    CMDLINE_BETA_BNET_SERVER = 50,
    CMDLINE_BUILDSERVER = 51,
    CMDLINE_ENUS_ONLY = 52,
    CMDLINE_TIMESTAMP_LOGFILENAME_BIT = 53,
    CMDLINE_AUTO_LOGIN_BIT = 54,
    CMDLINE_FAKE_INPUT_BIT = 55,
    CMDLINE_NO_INPUT_BIT = 56,
    CMDLINE_QUICK_TEST_BIT = 57,
    CMDLINE_ENABLE_VERSION_CHECK_BIT = 58,
    CMDLINE_NGDP = 59,
    CMDLINE_CDP = 60,
    CMDLINE_BIT_COUNT = 61,
};

// CmdLine.h:78
enum {
    TESTTYPE_NONE = 0,
    TESTTYPE_WORLDS = 1,
    TESTTYPE_MONSTERS = 2,
    TESTTYPE_SKILLS = 3,
    TESTTYPE_NETWORKING = 4,
    TESTTYPE_WORLD_PERF = 5,
    TESTTYPE_SKILL_PERF = 6,
    TESTTYPE_BADLINKS = 7,
    TESTTYPE_OPEN_WORLD = 8,
    TESTTYPE_WEBSITE_DATA = 9,
};
typedef int32 TestType;  // line 91

// CmdLine.h:93
enum {
    ASSETMODE_NONE = 0,
    ASSETMODE_BETA = 1,
    ASSETMODE_PUBLIC = 2,
    ASSETMODE_HOTFIX = 3,
};
typedef int32 AssetMode;  // line 100
typedef int32 PlayerClass;  // X/Common/GameSwig.h

struct CmdLineParams {  // size 0xb9a0
    CmdLineParams();
    XBitArray<61> tBitArray;  // 0x00
    CHAR sPlayerName[128];  // 0x08
    CHAR sDataDirectory[64];  // 0x88
    CHAR sReplayDirectory[260];  // 0xc8
    CHAR sBuildMopaqName[4096];  // 0x1cc
    CHAR sBuildMopaqDirectory[4096];  // 0x11cc
    CHAR sBuildName[4096];  // 0x21cc
    CHAR sJoinGameName[260];  // 0x31cc
    int32 nJoinTeamIndex;  // 0x32d0
    CHAR sP4ClientName[96];  // 0x32d4
    CHAR sP4Port[96];  // 0x3334
    CHAR sP4UserName[96];  // 0x3394
    CHAR sBattleNetGameAddress[96];  // 0x33f4
    CHAR sBattleNetAccount[128];  // 0x3454
    CHAR sBattleNetPassword[128];  // 0x34d4
    CHAR sServerBindAddress[96];  // 0x3554
    CHAR sServerAdvertAddress[96];  // 0x35b4
    CHAR sServerNetAdapter[96];  // 0x3614
    AssetMode eAssetMode;  // 0x3674
    CHAR sLoadWorld[4096];  // 0x3678
    CHAR sLoadScript[4096];  // 0x4678
    CHAR szInstanceName[4096];  // 0x5678
    int32 nJobIndex;  // 0x6678
    int32 nTotalJobs;  // 0x667c
    int32 nDRLGSeed;  // 0x6680
    TestType eTestType;  // 0x6684
    CHAR szTestFilter[4096];  // 0x6688
    CHAR szTestListPath[4096];  // 0x7688
    CHAR szTestEntryPoint[4096];  // 0x8688
    uint32 dwMinFPSScreenshot;  // 0x9688
    int32 nTestWaitTime;  // 0x968c
    int32 nTestSampleTime;  // 0x9690
    int32 nTestDumpFreq;  // 0x9694
    int32 nTestPlayerNum;  // 0x9698
    PlayerClass eTestClassOverride;  // 0x969c
    CHAR szSeasonsConfigFilename[260];  // 0x96a0
    CHAR szBlacklistConfigFilename[260];  // 0x97a4
    int32 nLockIdentity;  // 0x98a8
    uint32 uBlizzconGameTimeLimit;  // 0x98ac
    int32 uFmodEmergencyMemory;  // 0x98b0
    SigmaPlatform eBuildSigmaPlatform;  // 0x98b4
    XLocale eXLocale;  // 0x98b8
    CHAR szAgentUid[128];  // 0x98bc
    CHAR szAgentType[4096];  // 0x993c
    CHAR szBranchOverride[4096];  // 0xa93c
    CHAR sAssetServerAddress[96];  // 0xb93c
    int32 nAssetServerPort;  // 0xb99c
};
static_assert(sizeof(CmdLineParams) == 0xb9a0, "CmdLineParams");
static_assert(__builtin_offsetof(CmdLineParams, sReplayDirectory) == 0xc8, "CmdLineParams::sReplayDirectory");
static_assert(__builtin_offsetof(CmdLineParams, eTestType) == 0x6684, "CmdLineParams::eTestType");
static_assert(__builtin_offsetof(CmdLineParams, dwMinFPSScreenshot) == 0x9688, "CmdLineParams::dwMinFPSScreenshot");
static_assert(__builtin_offsetof(CmdLineParams, nTestPlayerNum) == 0x9698, "CmdLineParams::nTestPlayerNum");
static_assert(__builtin_offsetof(CmdLineParams, eXLocale) == 0x98b8, "CmdLineParams::eXLocale");
const CmdLineParams * CmdLineGetParams();
