#include "CmdLine.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Sigma/Math/MathGeneral.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "../../Sigma/DataStructures/XVar.h"
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../GameSwig.h"
// Declarations src has no header for (DWARF: XVar.cpp:137, PatchCheck.cpp:74, ErrorManager).
XVarRegistry* XVarGetDefaultRegistry();
namespace PatchCheck {
const char* GetLauncherPath();
}
void ErrorManagerSetLogStreamDirectory(LPCSTR pszDirectory);
#line 24 "X/Common/Shell/CmdLine.cpp"
enum ParamType {
    PARAMTYPE_IGNORE_VERSION_CHECK = 0,
    PARAMTYPE_WINDOWED = 1,
    PARAMTYPE_D3D11 = 2,
    PARAMTYPE_LAUNCH = 3,
    PARAMTYPE_LOCALE = 4,
    PARAMTYPE_BATTLENET_ADDRESS = 5,
    PARAMTYPE_BATTLENET_ACCOUNT = 6,
    PARAMTYPE_BATTLENET_PASSWORD = 7,
    PARAMTYPE_AUTO_LOGIN = 8,
    PARAMTYPE_FAKE_INPUT = 9,
    PARAMTYPE_NO_INPUT = 10,
    PARAMTYPE_QUICK_TEST = 11,
    PARAMTYPE_TEST = 12,
    PARAMTYPE_TESTMINFPSSCREENSHOT = 13,
    PARAMTYPE_TESTFILTER = 14,
    PARAMTYPE_TESTLIST = 15,
    PARAMTYPE_TESTENTRYPOINT = 16,
    PARAMTYPE_TESTWAITTIME = 17,
    PARAMTYPE_TESTSAMPLETIME = 18,
    PARAMTYPE_TESTDUMPFREQ = 19,
    PARAMTYPE_TESTPLAYERNUM = 20,
    PARAMTYPE_TESTFORMATJSON = 21,
    PARAMTYPE_NGDP = 22,
    PARAMTYPE_CDP = 23,
    PARAMTYPE_TESTCLASS = 24,
    PARAMTYPE_BLIZZCON = 25,
    PARAMTYPE_BLIZZCON_PVP = 26,
    PARAMTYPE_BLIZZCON_DROP = 27,
    PARAMTYPE_JOINGAMENAME = 28,
    PARAMTYPE_JOINTEAM = 29,
    PARAMTYPE_GAME_TIME_LIMIT = 30,
    PARAMTYPE_AGENT_UID = 31,
    PARAMTYPE_AGENTTYPE = 32,
    PARAMTYPE_3DV = 33,
    PARAMTYPE_HIGH_DPI_CURSOR = 34,
    PARAMTYPE_CURSOR_IGNORE_HIGH_DPI = 35,
    PARAMTYPE_NO_FPS_LIMIT = 36,
    PARAMTYPE_USE_PAGEFLIP = 37,
    PARAMTYPE_DEBUG_D3D = 38,
    PARAMTYPE_OVERRIDE_LOG_FOLDER = 39,
    PARAMTYPE_OVERRIDE_FMOD_EMERGENCY_MEMORY = 40,
    NUMBER_OF_PARAMTYPES = 41,
};
#line 191 "X/Common/Shell/CmdLine.cpp"
struct CmdLineParam
{
    char szString[32];
};




CmdLineParams::CmdLineParams()
{









    tBitArray.Set(CMDLINE_DISABLE_KNOWN_TRACE, TRUE);

    tBitArray.Set(CMDLINE_ENABLE_VERSION_CHECK_BIT, TRUE);

    SigmaMemoryClear(sPlayerName, sizeof(sPlayerName));
    SigmaMemoryClear(sDataDirectory, sizeof(sDataDirectory));
    SigmaMemoryClear(sReplayDirectory, sizeof(sReplayDirectory));
    SigmaMemoryClear(sBuildMopaqName, sizeof(sBuildMopaqName));
    SigmaMemoryClear(sBuildMopaqDirectory, sizeof(sBuildMopaqDirectory));
    SigmaMemoryClear(sBattleNetGameAddress, sizeof(sBattleNetGameAddress));
    SigmaMemoryClear(sServerAdvertAddress, sizeof(sServerAdvertAddress));
    SigmaMemoryClear(sServerNetAdapter, sizeof(sServerNetAdapter));
    SigmaMemoryClear(sServerBindAddress, sizeof(sServerBindAddress));
    SigmaMemoryClear(sLoadWorld, sizeof(sLoadWorld));
    SigmaMemoryClear(sLoadScript, sizeof(sLoadScript));
    SigmaMemoryClear(sJoinGameName, sizeof(sJoinGameName));
    SigmaMemoryClear(szInstanceName, sizeof(szInstanceName));
    SigmaMemoryClear(sBattleNetAccount, sizeof(sBattleNetAccount));
    SigmaMemoryClear(sBattleNetPassword, sizeof(sBattleNetPassword));
    SigmaMemoryClear(sBuildName, sizeof(sBuildName));

    nJoinTeamIndex = -1;
    SigmaMemoryClear(sP4ClientName, sizeof(sP4ClientName));
    nJobIndex = -1;
    nTotalJobs = 20;  // literal: merged into line 233's 64-bit store; no named constant found
    nDRLGSeed = 0;
    eTestType = TESTTYPE_NONE;
    SigmaMemoryClear(szTestFilter, sizeof(szTestFilter));
    SigmaMemoryClear(szTestListPath, sizeof(szTestListPath));
    SigmaMemoryClear(szTestEntryPoint, sizeof(szTestEntryPoint));
    dwMinFPSScreenshot = 0;
    nTestWaitTime = 0;
    nTestSampleTime = 0;
    nTestDumpFreq = 0;
    nTestPlayerNum = 1;
    eTestClassOverride = PLAYER_CLASS_NONE;
    nLockIdentity = 0;
    tBitArray.Set(CMDLINE_TEST_FORMAT_JSON_BIT, FALSE);


    SigmaMemoryClear(szSeasonsConfigFilename, sizeof(szSeasonsConfigFilename));
    SigmaMemoryClear(szBlacklistConfigFilename, sizeof(szBlacklistConfigFilename));




    eBuildSigmaPlatform = SIGMAPLATFORM_AXE;
    eXLocale = XLOCALE_INVALID;



    uBlizzconGameTimeLimit = 0;
    uFmodEmergencyMemory = 0;
    eAssetMode = ASSETMODE_NONE;


    SigmaMemoryClear(szAgentUid, sizeof(szAgentUid));
    SigmaMemoryClear(szAgentType, sizeof(szAgentType));











    SigmaMemoryClear(szBranchOverride, sizeof(szBranchOverride));
    SigmaMemoryClear(sAssetServerAddress, sizeof(sAssetServerAddress));
    nAssetServerPort = 0;
}

blz::string gszFixedCmdLine("");
blz::string gszReopenCmdLine("");
CmdLineParams gtCmdLineParams;

static const CmdLineParam ctParams[NUMBER_OF_PARAMTYPES] = {
    {"-ignoreversioncheck"},
    {"-w"},
    {"-d3d11"},
    {"-launch"},
    {"-locale"},
    {"-battlenetaddress"},
    {"-battlenetaccount"},
    {"-battlenetpassword"},
    {"-autologin"},
    {"-fakeinput"},
    {"-noinput"},
    {"-quicktest"},
    {"-test"},
    {"-perftestminfpsscreenshot"},
    {"-perftestfilter"},
    {"-perftestlist"},
    {"-perftestentrypoint"},
    {"-perftestwaittime"},
    {"-perftestsampletime"},
    {"-perftestdumpfreq"},
    {"-perftestplayernum"},
    {"-perftestformat"},
    {"-ngdp"},
    {"-cdp"},
    {"-perftestclass"},
    {"-blizzcon"},
    {"-blizzconpvp"},
    {"-blizzcondrop"},
    {"-joingamename"},
    {"-jointeam"},
    {"-gametimelimit"},
    {"-uid"},
    {"-agenttype"},
    {"-3dv"},
    {"-highdpicursor"},
    {"-cursorignorehighdpi"},
    {"-nofpslimit"},
    {"-usepageflip"},
    {"-debugd3d"},
    {"-overridelogfolder"},
    {"-overridefmodemergencymem"},
};
// Forward declaration (inferred: C++ needs it; the definition is at line 637, the call at 484).
static void sHandleCmdLineParam(const ParamType ceParamType, const char* szCmdLine);
#line 447 "X/Common/Shell/CmdLine.cpp"
static XVarScope s_scopeVersion("Version", 0);
XVarString s_varLocale(s_scopeVersion, "Locale", "");
// Blizzard/String.h declaration, body already recovered in System_String.cpp.
namespace Blizzard {
namespace String {
void StripWhitespace(char* buff);
}
}







void CmdLineParse(
    const char* szCmdLine)
{
    gszFixedCmdLine = szCmdLine;




    gszFixedCmdLine.replace_all("\x96\x97", "-");  // the bytes of the original's literal (0x7100de0512)


    if (XVarRegistry* ptRegistry = XVarGetDefaultRegistry())
    {
        ptRegistry->RegisterCmdLine(szCmdLine);
    }

    for (int32 i = 0; i < NUMBER_OF_PARAMTYPES; ++i)
    {
        LPCSTR szParam = Blizzard::String::Find(gszFixedCmdLine.c_str(), ctParams[i].szString);

        if (szParam)
        {
            sHandleCmdLineParam((ParamType)i, szParam);
        }
    }
#line 500 "X/Common/Shell/CmdLine.cpp"
    if (gtCmdLineParams.eTestType != TESTTYPE_WORLD_PERF && gtCmdLineParams.eTestType != TESTTYPE_BADLINKS)
    {
        gtCmdLineParams.tBitArray.Set(CMDLINE_AUTO_LOGIN_BIT, FALSE);


        SigmaMemoryClear(gtCmdLineParams.sBattleNetAccount, sizeof(gtCmdLineParams.sBattleNetAccount));
        SigmaMemoryClear(gtCmdLineParams.sBattleNetPassword, sizeof(gtCmdLineParams.sBattleNetPassword));
    }


    if (gtCmdLineParams.eTestType == TESTTYPE_WORLD_PERF)
    {

        gtCmdLineParams.nDRLGSeed = 0xDEADBEEF;
        gtCmdLineParams.tBitArray.Set(CMDLINE_DRLG_SEED_BIT, TRUE);
    }


    XLocale eLocale = XLocaleFromString(s_varLocale.Get().c_str());
    if (eLocale != NUM_XLOCALES)
    {
        gtCmdLineParams.eXLocale = eLocale;
    }





    gszReopenCmdLine = blz::make_stringf("\"%s\"", PatchCheck::GetLauncherPath());
#line 544 "X/Common/Shell/CmdLine.cpp"
}




const CmdLineParams* CmdLineGetParams()
{
    return &gtCmdLineParams;
}
#line 568 "X/Common/Shell/CmdLine.cpp"
const char* GetCmdLine()
{
    return gszFixedCmdLine.c_str();
}





BOOL CmdLineGetStringParam(const char* szParam, char* szString, uint32 dwLen)
{
    char szCopy[4096];
    Blizzard::String::Copy(szCopy, szParam, sizeof(szCopy));
    Blizzard::String::StripWhitespace(szCopy);
    if (szCopy[0] == '\'' || szCopy[0] == '"') {
        char* end = strchr(szCopy + 1, szCopy[0]);
        if (end) {
            size_t dwMinLen = Math::Min(static_cast<size_t>(dwLen), static_cast<size_t>(end - szCopy));
            Blizzard::String::Copy(szString, szCopy + 1, dwMinLen);
            return TRUE;
        }
    }
    const char cszTokens[] = " ";
    char* szTokStr0 = strtok(szCopy, cszTokens);
    if (!szTokStr0) return FALSE;
    uint32 dwMinLen = Math::Min(dwLen, static_cast<uint32>(sizeof(szCopy)));
    Blizzard::String::Copy(szString, szTokStr0, dwMinLen);
    return TRUE;
}
#line 637 "X/Common/Shell/CmdLine.cpp"
static void sHandleCmdLineParam(
    const ParamType ceParamType,
    const char* szCmdLine)
{
    const CmdLineParam* ptParam = &ctParams[ceParamType];

    switch (ceParamType)
    {

    case PARAMTYPE_IGNORE_VERSION_CHECK:
        // inferred: the binary clears bit 58 (CMDLINE_ENABLE_VERSION_CHECK_BIT)
        gtCmdLineParams.tBitArray.Set(CMDLINE_ENABLE_VERSION_CHECK_BIT, FALSE);
        break;


    case PARAMTYPE_WINDOWED:

        gtCmdLineParams.tBitArray.Set(CMDLINE_FORCE_WINDOWED_MODE_BIT, TRUE);
        break;

    case PARAMTYPE_D3D11:

        gtCmdLineParams.tBitArray.Set(CMDLINE_D3D11_MODE_BIT, TRUE);
        break;









    case PARAMTYPE_LAUNCH:

        gtCmdLineParams.tBitArray.Set(CMDLINE_LAUNCH, TRUE);
        break;



    case PARAMTYPE_LOCALE:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;

            char szLocale[SIGMA_MAX_PATH];
            CmdLineGetStringParam(szParam, szLocale, sizeof(szLocale));
            XLocale eXLocale = XLocaleFromString(szLocale);
            gtCmdLineParams.eXLocale = eXLocale;
        }
        break;


    case PARAMTYPE_BATTLENET_ADDRESS:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            CmdLineGetStringParam(szParam, gtCmdLineParams.sBattleNetGameAddress, sizeof(gtCmdLineParams.sBattleNetGameAddress));
        }
        break;


    case PARAMTYPE_BATTLENET_ACCOUNT:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            CmdLineGetStringParam(szParam, gtCmdLineParams.sBattleNetAccount, sizeof(gtCmdLineParams.sBattleNetAccount));
        }
        break;


    case PARAMTYPE_BATTLENET_PASSWORD:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            CmdLineGetStringParam(szParam, gtCmdLineParams.sBattleNetPassword, sizeof(gtCmdLineParams.sBattleNetPassword));
        }
        break;

    case PARAMTYPE_AUTO_LOGIN:

        gtCmdLineParams.tBitArray.Set(CMDLINE_AUTO_LOGIN_BIT, TRUE);
        break;


    case PARAMTYPE_FAKE_INPUT:

        gtCmdLineParams.tBitArray.Set(CMDLINE_FAKE_INPUT_BIT, TRUE);
        break;


    case PARAMTYPE_NO_INPUT:

        gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
        break;


    case PARAMTYPE_QUICK_TEST:

        gtCmdLineParams.tBitArray.Set(CMDLINE_QUICK_TEST_BIT, TRUE);
        break;



    case PARAMTYPE_TEST:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;

            char szSubParam[SIGMA_MAX_PATH];
            CmdLineGetStringParam(szParam, szSubParam, sizeof(szSubParam));
            if (strcasecmp(szSubParam, "worlds") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
                gtCmdLineParams.eTestType = TESTTYPE_WORLDS;
            }
            else if (strcasecmp(szSubParam, "openworld") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
                gtCmdLineParams.eTestType = TESTTYPE_OPEN_WORLD;
            }
            else if (strcasecmp(szSubParam, "monsters") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
                gtCmdLineParams.eTestType = TESTTYPE_MONSTERS;
            }
            else if (strcasecmp(szSubParam, "skills") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
                gtCmdLineParams.eTestType = TESTTYPE_SKILLS;
            }
            else if (strcasecmp(szSubParam, "networking") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
                gtCmdLineParams.eTestType = TESTTYPE_NETWORKING;
            }
            else if (strcasecmp(szSubParam, "perf") == 0)
            {
                gtCmdLineParams.eTestType = TESTTYPE_WORLD_PERF;
            }
            else if (strcasecmp(szSubParam, "skillperf") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
                gtCmdLineParams.eTestType = TESTTYPE_SKILL_PERF;
            }
            else if (strcasecmp(szSubParam, "badlinks") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_NO_INPUT_BIT, TRUE);
                gtCmdLineParams.eTestType = TESTTYPE_BADLINKS;
            }
        }
        break;










    case PARAMTYPE_TESTMINFPSSCREENSHOT:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.dwMinFPSScreenshot = strtol(szParam, NULL, 10);
        }
        break;

    case PARAMTYPE_TESTFILTER:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;
            CmdLineGetStringParam(szParam, gtCmdLineParams.szTestFilter, SIGMA_MAX_PATH);
        }
        break;

    case PARAMTYPE_TESTLIST:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;
            CmdLineGetStringParam(szParam, gtCmdLineParams.szTestListPath, SIGMA_MAX_PATH);
        }
        break;

    case PARAMTYPE_TESTENTRYPOINT:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;
            CmdLineGetStringParam(szParam, gtCmdLineParams.szTestEntryPoint, SIGMA_MAX_PATH);
        }
        break;

    case PARAMTYPE_TESTWAITTIME:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.nTestWaitTime = strtol(szParam, NULL, 10);
        }
        break;

    case PARAMTYPE_TESTSAMPLETIME:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.nTestSampleTime = strtol(szParam, NULL, 10);
        }
        break;

    case PARAMTYPE_TESTDUMPFREQ:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.nTestDumpFreq = strtol(szParam, NULL, 10);
        }
        break;

    case PARAMTYPE_TESTPLAYERNUM:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.nTestPlayerNum = strtol(szParam, NULL, 10);
        }
        break;

    case PARAMTYPE_TESTFORMATJSON:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;
            char szSubParam[SIGMA_MAX_PATH];
            CmdLineGetStringParam(szParam, szSubParam, sizeof(szSubParam));
            if (strcasecmp(szSubParam, "json") == 0)
            {
                gtCmdLineParams.tBitArray.Set(CMDLINE_TEST_FORMAT_JSON_BIT, TRUE);
            }
        }
        break;


    case PARAMTYPE_NGDP:

        gtCmdLineParams.tBitArray.Set(CMDLINE_NGDP, TRUE);
        break;



    case PARAMTYPE_CDP:

        gtCmdLineParams.tBitArray.Set(CMDLINE_CDP, TRUE);
        break;



    case PARAMTYPE_TESTCLASS:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.eTestClassOverride = (PlayerClass)strtol(szParam, NULL, 10);
        }
        break;



    case PARAMTYPE_BLIZZCON:

        gtCmdLineParams.tBitArray.Set(CMDLINE_BLIZZCON_BIT, TRUE);
        break;



    case PARAMTYPE_BLIZZCON_PVP:

        gtCmdLineParams.tBitArray.Set(CMDLINE_BLIZZCON_BIT, TRUE);
        gtCmdLineParams.tBitArray.Set(CMDLINE_BLIZZCON_PVP_BIT, TRUE);
        break;



    case PARAMTYPE_BLIZZCON_DROP:

        gtCmdLineParams.tBitArray.Set(CMDLINE_BLIZZCON_DROP_BIT, TRUE);
        break;



    case PARAMTYPE_JOINGAMENAME:
        {
            const char* szName = szCmdLine + strlen(ptParam->szString);
            CmdLineGetStringParam(szName, gtCmdLineParams.sJoinGameName, sizeof(gtCmdLineParams.sJoinGameName));
        }
        break;


    case PARAMTYPE_JOINTEAM:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.nJoinTeamIndex = strtol(szParam, NULL, 10);
        }
        break;


    case PARAMTYPE_GAME_TIME_LIMIT:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.uBlizzconGameTimeLimit = strtol(szParam, NULL, 10);
        }
        break;


    case PARAMTYPE_3DV:

        gtCmdLineParams.tBitArray.Set(CMDLINE_3DV_BIT, TRUE);
        break;


    case PARAMTYPE_AGENT_UID:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;
            CmdLineGetStringParam(szParam, gtCmdLineParams.szAgentUid, sizeof(gtCmdLineParams.szAgentUid));
        }
        break;


    case PARAMTYPE_AGENTTYPE:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            CmdLineGetStringParam(szParam, gtCmdLineParams.szAgentType, sizeof(gtCmdLineParams.szAgentType));
        }
        break;


    case PARAMTYPE_HIGH_DPI_CURSOR:

        gtCmdLineParams.tBitArray.Set(CMDLINE_HIGH_DPI_CURSOR_BIT, TRUE);
        break;



    case PARAMTYPE_CURSOR_IGNORE_HIGH_DPI:

        gtCmdLineParams.tBitArray.Set(CMDLINE_CURSOR_IGNORE_HIGH_DPI_BIT, TRUE);
        break;



    case PARAMTYPE_NO_FPS_LIMIT:

        gtCmdLineParams.tBitArray.Set(CMDLINE_NO_FPS_LIMIT_BIT, TRUE);
        break;



    case PARAMTYPE_USE_PAGEFLIP:

        gtCmdLineParams.tBitArray.Set(CMDLINE_USE_PAGEFLIP_BIT, TRUE);
        break;



    case PARAMTYPE_DEBUG_D3D:

        gtCmdLineParams.tBitArray.Set(CMDLINE_DEBUG_D3D_BIT, TRUE);
        break;




    case PARAMTYPE_OVERRIDE_LOG_FOLDER:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString) + 1;

            char szFolderName[4096];
            CmdLineGetStringParam(szParam, szFolderName, sizeof(szFolderName));


            ErrorManagerSetLogStreamDirectory(szFolderName);
        }
        break;



    case PARAMTYPE_OVERRIDE_FMOD_EMERGENCY_MEMORY:
        {
            const char* szParam = szCmdLine + strlen(ptParam->szString);
            gtCmdLineParams.uFmodEmergencyMemory = strtol(szParam, NULL, 10) * KILO;  // inferred: KILO (Sigma.h) for the shift by 10
        }
        break;
    }
}
#line 1766 "X/Common/Shell/CmdLine.cpp"
BOOL CmdLineTestFlag(uint32 dwBitToTest)
{
    return gtCmdLineParams.tBitArray.Test(dwBitToTest);
}
