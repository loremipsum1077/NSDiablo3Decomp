// Original file: X/Server/Replay.cpp
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/CommonTypes.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/Protobuf/Account.pb.h"
#include "../Common/Protobuf/Hero.pb.h"
#include "../Common/OnlineService/Callback/Callback.h"
#include "../Sigma/Files/XFiles.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Networking/XEncoder.h"
#include "../Sigma/Networking/XDecoder.h"
#include "../Sigma/Random.h"
#include "../Sigma/RefString.h"
#include "../Sigma/SigmaTiming.h"
#include "../Sigma/UnicodeText.h"
#include "SGame.h"
#include "SGameManager.h"
#include "Network/XServerSocket.h"
#include "OnlineService/Legacy/ReplayPlayerResolver.h"
#include "../Common/ActorCommonDataSwig.h"
#include "../Common/Player.h"
#include "../Common/Networking/Network.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "SGameConstants.h"
void FileClose( FileReference & tFileRef ); BOOL FileWrite( FileReference & tFileRef, const void * lpBuf, const uint32 dwOffset, const uint32 dwSize, ErrorCode eErrorCodeIfFails ); BOOL FileOpen( FileReference & tFileRef, uint32 dwAccessFlags ); BOOL FileRead( FileReference & tFileRef, void * lpBuf, const uint32 dwOffset, const uint32 dwSize, const ErrorCode eErrorCodeIfFails );
BOOL FileExists( const FileReference & tFileRef ); BOOL FileGetSize( const FileReference & tFileRef, uint32 & dwSize ); BOOL FileCreate( FileReference & tFileRef ); BOOL FileDelete( FileReference & tFileRef );
namespace Blizzard { namespace Process { bool GetProcessDirectory( char * buf, int bufMax ); }  // BlizzardCore Process.h (not in src); XFilesCore.cpp decls above
}
const int32 DEFAULT_TEST_MAX_GAMES = 900;
const int32 DEFAULT_TEST_SECS_PER_GAME_START = 6;


const bool DEFAULT_REPLAY_ENABLED = false;




static XVarScope s_scopeReplay( "Replay", NULL );
XVarBool s_varEnabled( s_scopeReplay, "Enabled", DEFAULT_REPLAY_ENABLED );

static XVarScope s_scopeTest( s_scopeReplay, "Test" );
XVarUint32 s_varTestMaxGames( s_scopeTest, "MaxGames", DEFAULT_TEST_MAX_GAMES );
XVarUint32 s_varTestStartInterval( s_scopeTest, "StartIntervalSeconds", DEFAULT_TEST_SECS_PER_GAME_START );



const uint32 REPLAY_MAGIC_NUMBER = 0xfacef00d;
const uint32 REPLAY_VERSION = 2;
const uint32 REPLAY_CPU_TEST_RUN_COUNT = 5;

// X/Server/Network/Server.cpp; X/Common/Networking/Network.cpp:518; X/Common/Networking/GameMessage.cpp:837
uint8 * NetworkGetEncodeBuffer(); BOOL EncodeMessage( void * pMessage, XEncoder & tEncoder );
uint8 * NetworkGetDecodeBuffer(); XDecoder::Result DecodeMessage( GameMessageType eMessageType, void * pDecodeToBuffer, uint32 dwBufferSize, XDecoder & tDecoder );
// X/Common/Networking/ByteswapMessage.cpp:74
BOOL GetGameMessageTypeAndSize( GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor, uint32 & dwSize, const char *& pszMessageLabel );
// X/Server/SGame.cpp:5747, :5818, :781
BOOL SGameValidateJoinRequest( ServerSocketID idServerSocket, SGameID idSGame, OnlineService::HeroId uHeroId, uint64 uAuthToken, XLocale eLocale ); BOOL SGameJoinPendingPlayer( const OnlineService::HeroId uHeroId );
// X/Server/SGameManager.cpp:72
extern XVarUint32 s_varStatsUpdateSecondsInterval;
// X/Server/Network/Server.cpp:772, :510
void ServerClientDisconnectGC( GameConnectionID idGameConnection ); void ServerEventReceiveFromReplay( int32 nPlayerIndex, void * pMessage );
struct ReplayTestGlobals
{
    BOOL bActive;
    char szReplayFileName[4096];


    uint8 * ptReplayBuffer;
    uint32 dwReplayBufferSize;


    uint32 dwStartTime;
    uint32 dwTestTime;
    uint32 dwLastStatTime;
    uint32 dwLastUpdate;
    uint32 dwUpdateTimeMin;
    uint32 dwUpdateTimeMax;
    uint32 dwUpdateCount;
    uint32 uCurrentNumGames;
    int32 nUpdatesStalledInARow;
    BOOL bArena;
    uint32 uMaxGameCount;
    uint32 uSpawnDeltaSeconds;
    uint32 uBaseGameCount;
    uint32 uBaseTime;


    int32 nCpuTestRunCount;
    uint32 dwMinGameTime;
    uint32 dwMaxGameTime;
    uint32 dwTotalGameTime;
    uint32 dwGameTicks;
};
static_assert(sizeof(ReplayTestGlobals) == 0x1060, "ReplayTestGlobals");
struct ReplayRandRequestEntry
{
    char szFile[256];
    int nLine;
};

enum ReplayGlobalsFlags
{
    RGF_FILE_OPEN_BIT,
    RGF_IS_REPLAY_GAME_BIT,

    RGF_BIT_COUNT
};

enum ReplayMode
{
    ReplayModeNone,
    ReplayModeRecording,
    ReplayModePlaying,
    ReplayModePlayingServerLoadTest,
    ReplayModePlayingServerCPUTest,
};

struct ReplayGlobals
{
    char szReplayFileName[260];
    FileReference tRef;
    uint32 dwFileOffset;
    uint32 dwFileSize;
    uint32 dwGameHash;
    uint32 dwFlags;
    ReplayMode eReplayMode;
    uint32 dwReplayStartTime;
    Legacy::ReplayPlayerResolver * pResolver;
    uint32 uEntityIdCounter;
};
static_assert(sizeof(ReplayGlobals) == 0x1140, "ReplayGlobals");
struct ReplayFileHeader
{
    uint32 dwMagicNumber;
    uint32 dwVersion;
    GameParams tGameParams;
};

enum ReplayMsgType
{
    REPLAY_MSG_TYPE_INVALID,
    REPLAY_MSG_TYPE_GAME,
    REPLAY_MSG_TYPE_CONNECT,
    REPLAY_MSG_TYPE_DISCONNECT,
    REPLAY_MSG_TYPE_HASH,
    REPLAY_MSG_TYPE_HERO_DATA,
    REPLAY_MSG_TYPE_ACCOUNT_DATA,
};

struct ReplayMessageHeader
{
    uint32 dwGameTime;
    int32 nPlayerIndex;
    ReplayMsgType nType;
    uint32 dwHash;
};
static_assert(sizeof(ReplayMessageHeader) == 0x10, "ReplayMessageHeader");

// Forward declaration (invented: the original defines it at line 961, after its user here).
void sReplay_PlayerDataResolved( OnlineService::ErrorCode nErrorCode );








static void sCreateReplayResolver()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
    Legacy::ReplayPlayerResolver * pResolver = ptReplayGlobals->pResolver;
    if (pResolver == NULL)
    {
        pResolver = new Legacy::ReplayPlayerResolver;
        ptReplayGlobals->pResolver = pResolver;
        OnlineService::HeroId uHeroId;
        uHeroId = ++ptReplayGlobals->uEntityIdCounter;
        pResolver->AddPlayer(uHeroId, OnlineService::GameAccountHandle(uHeroId, OnlineService::PROGRAM_D3, 1), "", TEAM_PLAYER_FIRST, 0);
        pResolver->Begin(OnlineService::internal::function_bind_0_1<void, OnlineService::ErrorCode, &sReplay_PlayerDataResolved>(), 0);
    }
}





static void sReplayMakeFilePath(
    LPCSTR szReplayFileName,
    LPSTR szReplayFilePath)
{
    CHAR szReplayDirectory[4096];


    if (!Blizzard::String::Length(CmdLineGetParams()->sReplayDirectory))
    {
        bool bResult = Blizzard::Process::GetProcessDirectory(szReplayDirectory, sizeof(szReplayDirectory));

    }
    else
    {

        Blizzard::String::Copy(szReplayDirectory, CmdLineGetParams()->sReplayDirectory, sizeof(CmdLineGetParams()->sReplayDirectory));
    }


	Blizzard::String::JoinPath(szReplayFilePath, sizeof(szReplayDirectory), szReplayDirectory, szReplayFileName);

    LPCSTR szReplayExt = ".rep";

	strcat(szReplayFilePath, szReplayExt);
}




static void sReplayCreateFile(
	const GameParams & tParams)
{
	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

	CHAR szReplayFilePath[4096];
	sReplayMakeFilePath(tParams.szReplayFileName, szReplayFilePath);

	FileReferenceInit(ptReplayGlobals->tRef, szReplayFilePath);

	if (FileExists(ptReplayGlobals->tRef))
	{
		FileDelete(ptReplayGlobals->tRef);
	}

	BOOL bCreate = FileCreate(ptReplayGlobals->tRef);


	ReplayFileHeader tHeader;
	tHeader.dwMagicNumber = REPLAY_MAGIC_NUMBER;
	tHeader.dwVersion = REPLAY_VERSION;
	SigmaMemoryCopy(&tHeader.tGameParams, &tParams, sizeof(GameParams));

	BOOL bOpen = FileOpen(ptReplayGlobals->tRef, (1 << ACCESS_WRITE_BIT));


	ptReplayGlobals->dwFlags |= (1 << RGF_FILE_OPEN_BIT);

	BOOL bWrite = FileWrite(ptReplayGlobals->tRef, &tHeader, 0, sizeof(tHeader), ERROR_FILE_WRITE);


	ptReplayGlobals->dwFileOffset = sizeof(tHeader);
	ptReplayGlobals->eReplayMode = ReplayModeRecording;
}




BOOL sReplayDataRead(
    FileReference & tFileRef,
    void * lpBuf,
    const uint32 dwOffset,
    const uint32 dwSize)
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

    if (ptReplayTest->ptReplayBuffer == NULL)
    {

        return FileRead(tFileRef, lpBuf, dwOffset, dwSize, ERROR_FILE_READ);
    }


    if (dwOffset + dwSize > ptReplayTest->dwReplayBufferSize)
    {
        return FALSE;
    }

    SigmaMemoryCopy(lpBuf, ptReplayTest->ptReplayBuffer + dwOffset, dwSize);
    return TRUE;
}




static BOOL sReplayOpenAndLoadHeader(
    const char * pszReplayName,
    ReplayFileHeader & tHeader,
    FileReference * ptRef,
    uint32 * pdwFileSize,
    uint32 * pdwFileOffset)
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

    FileReference tLocalReference;
	FileReference & tRef = ptRef ? *ptRef : tLocalReference;

    uint32 dwFileSize;

    if (ptReplayTest->ptReplayBuffer == NULL)
    {
        CHAR szReplayFilePath[4096];
        sReplayMakeFilePath(pszReplayName, szReplayFilePath);

        FileReferenceInit(tRef, szReplayFilePath);

        if (!FileExists(tRef))
        {
            return FALSE;
        }


        BOOL bSize = FileGetSize(tRef, dwFileSize);
        if (!bSize)
        {
            FileClose(tRef);
            return FALSE;
        }


        BOOL bOpen = FileOpen(tRef, (1 << ACCESS_READ_BIT));

    }
    else
    {
        dwFileSize = ptReplayTest->dwReplayBufferSize;
    }

    BOOL bRead = sReplayDataRead(tRef, &tHeader, 0, sizeof(tHeader));
	if (!bRead)
	{
		FileClose(tRef);
		return FALSE;
	}


	if (tHeader.dwMagicNumber != REPLAY_MAGIC_NUMBER)
	{
		FileClose(tRef);
		return FALSE;
	}


	if (tHeader.dwVersion != REPLAY_VERSION)
	{
		FileClose(tRef);
		return FALSE;
	}



	if (pdwFileSize)
	{
		*pdwFileSize = dwFileSize;
	}

	if (pdwFileOffset)
	{
		*pdwFileOffset = sizeof(tHeader);
	}

	if (!ptRef && !ptReplayTest->ptReplayBuffer)
	{

		FileClose(tRef);
	}

	return TRUE;
}




static BOOL sReplayOpenForPlaying(
	uint32 dwSeed,
	LPCSTR szReplayFileName)
{
	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

	if (ptReplayTest->bArena == FALSE)
	{




		ReplayFileHeader tHeader;

		if (!sReplayOpenAndLoadHeader(szReplayFileName,
			tHeader,
			&ptReplayGlobals->tRef,
			&ptReplayGlobals->dwFileSize,
			&ptReplayGlobals->dwFileOffset))
		{
			return FALSE;
		}


        if (ptReplayTest->ptReplayBuffer == NULL)
        {

        ptReplayGlobals->dwFlags |= (1 << RGF_FILE_OPEN_BIT);
        }

		SGameGlobals & tSGameGlobals = SGameGlobalsGet();

		uint32 dwOldSeed = RandGetSeed(tSGameGlobals.rng);



	RandSetSeed_Internal(tSGameGlobals.rng, tHeader.tGameParams.dwSeed);
	}
	return TRUE;
}





void ReplayInitializeForGame(
	const GameParams & tParams)
{
    if (!s_varEnabled.Get())
    {
        return;
    }

	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals *& ptReplayGlobals = tGCD.ptReplayGlobals;


	uint32 dwFlagMask = (1 << GAMECREATEPARAM_RECORD_SERVER_REPLAY) | (1 << GAMECREATEPARAM_PLAY_SERVER_REPLAY) |
		(1 << GAMECREATEPARAM_PLAY_MULTI_SERVER_REPLAY) | (1 << GAMECREATEPARAM_REPLAY_SERVER_CPU_TEST);


	if (!(tParams.dwCreationFlags & dwFlagMask))
	{
		return;
	}


	ptReplayGlobals = new (SigmaMemoryNew(sizeof(ReplayGlobals), 4, AppGetCurrentMemoryInterface(), TRUE)) ReplayGlobals;

	if ((tParams.dwCreationFlags & (1 << GAMECREATEPARAM_RECORD_SERVER_REPLAY)) &&
			!GameActiveGameCommonDataIsClient())
	{
		sReplayCreateFile(tParams);
	}

	if (tParams.dwCreationFlags & (1 << GAMECREATEPARAM_PLAY_SERVER_REPLAY))
	{
		if (!GameActiveGameCommonDataIsClient())
		{
			BOOL bSuccess = sReplayOpenForPlaying(tParams.dwSeed, tParams.szReplayFileName);
			if (!bSuccess)
			{
				return;
			}
		}


		ptReplayGlobals->eReplayMode = ReplayModePlaying;
		ptReplayGlobals->dwFlags |= (1 << RGF_IS_REPLAY_GAME_BIT);
	}

	if (!GameActiveGameCommonDataIsClient())
	{
		if (tParams.dwCreationFlags & (1 << GAMECREATEPARAM_PLAY_MULTI_SERVER_REPLAY))
		{
			BOOL bSuccess = sReplayOpenForPlaying(tParams.dwSeed, tParams.szReplayFileName);

			if (!bSuccess)
			{
				return;
			}

			ptReplayGlobals->eReplayMode = ReplayModePlayingServerLoadTest;
			ptReplayGlobals->dwFlags |= (1 << RGF_IS_REPLAY_GAME_BIT);
		}

		if (tParams.dwCreationFlags & (1 << GAMECREATEPARAM_REPLAY_SERVER_CPU_TEST))
		{
			BOOL bSuccess = sReplayOpenForPlaying(tParams.dwSeed, tParams.szReplayFileName);

			if (!bSuccess)
			{
				return;
			}

			ptReplayGlobals->eReplayMode = ReplayModePlayingServerCPUTest;
			ptReplayGlobals->dwFlags |= (1 << RGF_IS_REPLAY_GAME_BIT);
		}
	}

	ptReplayGlobals->dwReplayStartTime = GetMachineTicks();
}




void ReplayDisposeForGame()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals *& ptReplayGlobals = tGCD.ptReplayGlobals;
    if ( ptReplayGlobals != NULL )
    {
        if ( ptReplayGlobals->dwFlags & ( 1 << RGF_FILE_OPEN_BIT ) )
        {
            FileClose( ptReplayGlobals->tRef );
        }
        ::delete ptReplayGlobals->pResolver;
        ptReplayGlobals->pResolver = NULL;
        XFREE( ptReplayGlobals, AppGetCurrentMemoryInterface() );
    }
}







































































#include "../Sigma/Math/MathGeneral.h"

static void sReplayMultiserverPlayRecordedGame()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;




	ReplayFileHeader tHeader;
	if (!sReplayOpenAndLoadHeader(ptReplayTest->szReplayFileName, tHeader, NULL, NULL, NULL))
	{
		return;
	}


	GameParams tParams;
	SigmaMemoryCopy(&tParams, &tHeader.tGameParams, sizeof(tParams));

	tParams.dwCreationFlags &= ~(1 << GAMECREATEPARAM_RECORD_SERVER_REPLAY);
	tParams.dwCreationFlags |= (1 << GAMECREATEPARAM_PLAY_MULTI_SERVER_REPLAY);
	tParams.nGameParts = GAME_PARTS_BATTLENET_NETWORK;
	tParams.idSGame = SGAMEID_INVALID;
	strcpy(tParams.szServerAddress, "127.0.0.1");
	SGameInitializeGame(tParams, NULL);
}




static void sReplayServerCPUTestPlayRecordedGame()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;




	ReplayFileHeader tHeader;
	if (!sReplayOpenAndLoadHeader(ptReplayTest->szReplayFileName, tHeader, NULL, NULL, NULL))
	{
		return;
	}


	GameParams tParams;
	SigmaMemoryCopy(&tParams, &tHeader.tGameParams, sizeof(tParams));

	tParams.dwCreationFlags &= ~(1 << GAMECREATEPARAM_RECORD_SERVER_REPLAY);
	tParams.dwCreationFlags |= (1 << GAMECREATEPARAM_REPLAY_SERVER_CPU_TEST);
	tParams.nGameParts = GAME_PARTS_BATTLENET_NETWORK;
	tParams.idSGame = SGAMEID_INVALID;
	strcpy(tParams.szServerAddress, "127.0.0.1");
	strcpy(tParams.szGameName, "ReplayServerCPUTest");
	SGameInitializeGame(tParams, NULL);
}






static void sReplayServerWriteHeader(
	int32 nPlayerIndex,
	ReplayMsgType nType)
{
	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
	SGameGlobals & tSGameGlobals = SGameGlobalsGet();
	uint32 dwNow = GameGetServerTime();



	ReplayMessageHeader tMessageHeader;
	tMessageHeader.dwGameTime = dwNow;
	tMessageHeader.nPlayerIndex = nPlayerIndex;
	tMessageHeader.nType = nType;
	tMessageHeader.dwHash = tSGameGlobals.rng.dwSeed;

	BOOL bWrite = FileWrite(
		ptReplayGlobals->tRef,
		&tMessageHeader,
		ptReplayGlobals->dwFileOffset,
		sizeof(tMessageHeader),
		ERROR_FILE_WRITE);

	ptReplayGlobals->dwFileOffset += sizeof(tMessageHeader);
}




void ReplayServerReceiveMessage(
	int32 nPlayerIndex,
	void * pMessage)
{
	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

	if (ptReplayGlobals != NULL &&
		ptReplayGlobals->eReplayMode == ReplayModeRecording)
	{



		RequiredMessageHeader * ptGameHeader = (RequiredMessageHeader *)pMessage;

		sReplayServerWriteHeader(nPlayerIndex, REPLAY_MSG_TYPE_GAME);

		XEncoder tEncoder(NetworkGetEncodeBuffer(), 0x8000);
		tEncoder.SetUsed(32);
	EncodeMessage(pMessage, tEncoder);
		uint32 dwBytesUsed = tEncoder.UsedBytes();
		tEncoder.SetUsed(0);
		tEncoder.Put(dwBytesUsed, 32);



		BOOL bWrite = FileWrite(
			ptReplayGlobals->tRef,
			tEncoder.Data(),
			ptReplayGlobals->dwFileOffset,
			dwBytesUsed,
			ERROR_FILE_WRITE);

		ptReplayGlobals->dwFileOffset += dwBytesUsed;

		const XTypeDescriptor * pTypeDescriptor;
		uint32 dwSize;
		const char * pszLabel;
		uint32 dwNow = GameGetServerTime();
	GetGameMessageTypeAndSize(ptGameHeader->eType, pTypeDescriptor, dwSize, pszLabel);
		SGameGlobals & tSGameGlobals = SGameGlobalsGet();
	}
}




















void ReplayServerClientDisconnect( int nPlayerIndex )
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
    if ( ptReplayGlobals != NULL && ptReplayGlobals->eReplayMode == ReplayModeRecording )
    {

        sReplayServerWriteHeader( nPlayerIndex, REPLAY_MSG_TYPE_DISCONNECT );
    }
}
// ReplayStopReplays: DWARF subprogram at 770 (low_pc 0, linker-removed); locals from DWARF, body reconstructed.
static void sReplayTestStop();  // defined at 1564; forward declaration (invented placement)
// LINKER-REMOVED: DWARF 770 (low_pc 0, 0x1c bytes); locals from DWARF, body reconstructed, no surviving copy; keeps sReplayTestStop out of line.
void ReplayStopReplays()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

	if (ptReplayTest != NULL && ptReplayTest->bActive)
	{
		sReplayTestStop();
	}
}























BOOL ReplayIsPlaying()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
    if ( ptReplayGlobals != NULL )
    {
        return ptReplayGlobals->eReplayMode == ReplayModePlaying;
    }
    return FALSE;
}






BOOL ReplayIsPlayingMultiServerGame()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
    if ( ptReplayGlobals != NULL )
    {
        return ptReplayGlobals->eReplayMode == ReplayModePlayingServerLoadTest;
    }
    return FALSE;
}






BOOL ReplayIsPlayingServerCPUTest()
{
    return ( sgtAppGlobals.dwGameCreateParamFlags & 0x40 ) != 0;
}







BOOL ReplayIsUsingRecordedAVFrustum()
{

    SGame & tSGame = SGameManager::GameGet();
    return ( tSGame.tStartParams.dwCreationFlags & 0x4 ) != 0;
}





BOOL ReplayIsReplayGame()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
    if ( ptReplayGlobals != NULL )
    {
        return ( ptReplayGlobals->dwFlags & ( 1 << RGF_IS_REPLAY_GAME_BIT ) ) != 0;
    }
    return FALSE;
}






BOOL ReplayIsRecordingGame()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
    if ( ptReplayGlobals != NULL )
    {
        return ptReplayGlobals->eReplayMode == ReplayModeRecording;
    }
    return FALSE;
}





static uint32 sReadGenericDataChunk(
    void * pBuffer,
    uint32 uBufferSize)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

    uint32 uChunkSize;
    BOOL bChunkSizeRead = sReplayDataRead(
        ptReplayGlobals->tRef,
        &uChunkSize,
        ptReplayGlobals->dwFileOffset,
        sizeof(uChunkSize));



    ptReplayGlobals->dwFileOffset += sizeof(uChunkSize);


    BOOL bChunkRead = sReplayDataRead(
        ptReplayGlobals->tRef,
        pBuffer,
        ptReplayGlobals->dwFileOffset,
        uChunkSize);


    ptReplayGlobals->dwFileOffset += uChunkSize;

    return uChunkSize;
}



static BOOL sWriteGenericDataChunk(
    int32 nPlayerIndex,
    ReplayMsgType eMsgType,
    void * pBuffer,
    uint32 uBufferSize)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

    sReplayServerWriteHeader(nPlayerIndex, eMsgType);


    BOOL bWrite = FileWrite(
        ptReplayGlobals->tRef,
        &uBufferSize,
        ptReplayGlobals->dwFileOffset,
        sizeof(uBufferSize),
        ERROR_FILE_WRITE);
    ptReplayGlobals->dwFileOffset += sizeof(uBufferSize);


    bWrite = FileWrite(
        ptReplayGlobals->tRef,
        pBuffer,
        ptReplayGlobals->dwFileOffset,
        uBufferSize,
        ERROR_FILE_WRITE);
    ptReplayGlobals->dwFileOffset += uBufferSize;

    return bWrite;
}




void sReplay_PlayerDataResolved(
    OnlineService::ErrorCode nErrorCode)
{


    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

    Legacy::ReplayPlayerResolver * pResolver = ptReplayGlobals->pResolver;


    SGameGlobalsGet().dwFlags |= (1 << SGAMEGLOBALS_DISABLED_HERO_SAVE);

    pResolver->AppendPlayersToGame(SGameManager::GetCurrentGame());
    SGameValidateJoinRequest(
        SERVERSOCKETID_INVALID,
        SGameManager::GetCurrentGame(),
        pResolver->GetResolvedPlayer(0).uHeroId,
        pResolver->GetResolvedPlayer(0).uAuthToken,
        XLOCALE_ENGLISH_US);

    SGameJoinPendingPlayer(pResolver->GetResolvedPlayer(0).uHeroId);

    ::delete ptReplayGlobals->pResolver; ptReplayGlobals->pResolver = NULL;
}




static BOOL sReadReplayMessage(
	ReplayMsgType eExpectedType)
{
	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

	if (ptReplayTest->bArena)
	{
		return FALSE;
	}


















	if (ptReplayGlobals->eReplayMode == ReplayModeNone)
	{
		return FALSE;
	}

	if (ptReplayGlobals->dwFileSize == ptReplayGlobals->dwFileOffset)
	{




		switch (ptReplayGlobals->eReplayMode)
		{
		case ReplayModePlaying:
			ptReplayGlobals->eReplayMode = ReplayModeNone;
			return FALSE;
		case ReplayModePlayingServerCPUTest:
			{

				uint32 dwElapsed = GetMachineTicks() - ptReplayGlobals->dwReplayStartTime;












				SGameGlobals & tSGameGlobals = SGameGlobalsGet();
				tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);

				ptReplayGlobals->eReplayMode = ReplayModeNone;

				ptReplayTest->nCpuTestRunCount++;
				ptReplayTest->dwTotalGameTime += dwElapsed;

				if (dwElapsed < ptReplayTest->dwMinGameTime)
				{
					ptReplayTest->dwMinGameTime = dwElapsed;
				}
				if (dwElapsed > ptReplayTest->dwMaxGameTime)
				{
					ptReplayTest->dwMaxGameTime = dwElapsed;
				}

				ptReplayTest->dwGameTicks = GameGetServerTime();
				return FALSE;
			}

		case ReplayModePlayingServerLoadTest:
			{
				SGameGlobals & tSGameGlobals = SGameGlobalsGet();
				tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);
				return FALSE;
			}
		}
	}


	ReplayMessageHeader tMessageHeader;
	ReplayRandRequestEntry tRandEntry = {};

	BOOL bRead1 = sReplayDataRead(
		ptReplayGlobals->tRef,
		&tMessageHeader,
		ptReplayGlobals->dwFileOffset,
		sizeof(tMessageHeader));


	uint32 dwNow = GameGetServerTime();

	if (tMessageHeader.dwGameTime > dwNow)
	{
		return FALSE;
	}
	// Operand order from columns: && at 1101:47 and 1101:95 (nType test second, dwGameTime test last)
	if (eExpectedType == REPLAY_MSG_TYPE_INVALID && tMessageHeader.nType == REPLAY_MSG_TYPE_HASH && tMessageHeader.dwGameTime == dwNow)
	{
		return FALSE;
	}



	ptReplayGlobals->dwFileOffset += sizeof(tMessageHeader);

	if (tMessageHeader.nType == REPLAY_MSG_TYPE_HASH)
	{

		sReplayDataRead(
			ptReplayGlobals->tRef,
			&tRandEntry,
			ptReplayGlobals->dwFileOffset,
			sizeof(tRandEntry));

		ptReplayGlobals->dwFileOffset += sizeof(tRandEntry);
	}

	if (eExpectedType == REPLAY_MSG_TYPE_HASH && tMessageHeader.nType != eExpectedType)
	{
		ptReplayGlobals->eReplayMode = ReplayModeNone;
		return FALSE;
	}









	SGameGlobals & tSGameGlobals = SGameGlobalsGet();
	if (tMessageHeader.dwHash != tSGameGlobals.rng.dwSeed)
	{
		switch (ptReplayGlobals->eReplayMode)
		{
		case ReplayModePlaying:
			ptReplayGlobals->eReplayMode = ReplayModeNone;
			return FALSE;

		case ReplayModePlayingServerLoadTest:
			tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);
			return FALSE;
		}
	}










































	switch (tMessageHeader.nType)
	{
	case REPLAY_MSG_TYPE_CONNECT:
		return TRUE;






















	case REPLAY_MSG_TYPE_DISCONNECT:
		{
		if (ptReplayGlobals->eReplayMode == ReplayModePlayingServerLoadTest || ptReplayGlobals->eReplayMode == ReplayModePlayingServerCPUTest)
		{




			GameConnectionID idGameConnection = PlayerGetByPlayerIndex(tMessageHeader.nPlayerIndex)->idGameConnection;
			if (idGameConnection != GAMECONNECTIONID_INVALID)
			{
				ServerClientDisconnectGC(idGameConnection);
			}
		}
		}
		return TRUE;
	case REPLAY_MSG_TYPE_HASH:
		return TRUE;
	case REPLAY_MSG_TYPE_HERO_DATA:
		{
			uint8 auHeroDefinitionBuffer[65536];
			uint32 uChunkSize = sReadGenericDataChunk(auHeroDefinitionBuffer, sizeof(auHeroDefinitionBuffer));



			sCreateReplayResolver();
			Legacy::ReplayPlayerResolver * pResolver = ptReplayGlobals->pResolver;

			D3::Hero::SavedDefinition tSavedDefinition;
			tSavedDefinition.ParsePartialFromArray(auHeroDefinitionBuffer, uChunkSize);


			pResolver->HeroResolved(tSavedDefinition);
		}
		return TRUE;

	case REPLAY_MSG_TYPE_ACCOUNT_DATA:
		{
			uint8 auAccountDefinitionBuffer[65536];
			uint32 uChunkSize = sReadGenericDataChunk(auAccountDefinitionBuffer, sizeof(auAccountDefinitionBuffer));



			sCreateReplayResolver();
			Legacy::ReplayPlayerResolver * pResolver = ptReplayGlobals->pResolver;

			D3::Account::SavedDefinition tSavedDefinition;
			tSavedDefinition.ParsePartialFromArray(auAccountDefinitionBuffer, uChunkSize);


			pResolver->AccountResolved(tSavedDefinition);
		}
		return TRUE;
	}







	uint32 dwEncodedMessageSize;
	XDecoder tDecoder(&dwEncodedMessageSize, sizeof(dwEncodedMessageSize));

	BOOL bRead2 = sReplayDataRead(
		ptReplayGlobals->tRef,
		&dwEncodedMessageSize,
		ptReplayGlobals->dwFileOffset,
		sizeof(dwEncodedMessageSize));


	uint32 dwMessageSize;
	XDecoder::Result eResult = tDecoder.Get(dwMessageSize, 32);


	// C cast below: XDecoder::Data() is const (DWARF _ZNK8XDecoder4DataEv), sReplayDataRead takes void *.

	tDecoder.SetData(NetworkGetDecodeBuffer(), 0x8000);
	BOOL bRead3 = sReplayDataRead(
		ptReplayGlobals->tRef,
		(void *)tDecoder.Data(),
		ptReplayGlobals->dwFileOffset,
		dwMessageSize);


	ptReplayGlobals->dwFileOffset += dwMessageSize;


	tDecoder.SetUsed(32);
	RequiredMessageHeader tHeader;
	TypeDescriptorField field(DT_REQUIRED_MESSAGE_HEADER, NULL);
	eResult = DT_REQUIRED_MESSAGE_HEADER->Decode(&tHeader, field, tDecoder);



	uint8 byteMessage[32768];
	tDecoder.SetUsed(32);
	eResult = DecodeMessage(tHeader.eType, byteMessage, sizeof(byteMessage), tDecoder);


	ServerEventReceiveFromReplay(tMessageHeader.nPlayerIndex, byteMessage);

	return TRUE;
}




void ReplaySendServerMessages()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

    if (ptReplayGlobals != NULL &&
        ReplayIsReplayGame() &&
        !(SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN)))
    {

        while (TRUE)
        {


            BOOL bContinue = sReadReplayMessage(REPLAY_MSG_TYPE_INVALID);
            if (!bContinue)
            {
                break;
            }
        }
    }
}

// LINKER-REMOVED: ReplayCheckHash: DWARF subprogram at 1353 (low_pc 0, 0x12c bytes). Params, locals, the inlined
// sReplayServerWriteHeader at 1361 and all decl lines are from DWARF; the statements between them and
// the playback-side sReadReplayMessage( REPLAY_MSG_TYPE_HASH ) call are reconstructed (no bytes survive).
void ReplayCheckHash(
	const char * szFile, const int32 nLine)
{
	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;
	if (ptReplayGlobals != NULL && ReplayIsRecordingGame())
	{
		sReplayServerWriteHeader(-1, REPLAY_MSG_TYPE_HASH);

		ReplayRandRequestEntry tRandEntry = {};
		Blizzard::String::Copy(tRandEntry.szFile, szFile, sizeof(tRandEntry.szFile));
		tRandEntry.nLine = nLine;
		BOOL bWrite = FileWrite(
			ptReplayGlobals->tRef,
			&tRandEntry,
			ptReplayGlobals->dwFileOffset,
			sizeof(tRandEntry),
			ERROR_FILE_WRITE);
		ptReplayGlobals->dwFileOffset += sizeof(tRandEntry);

		SGameGlobals & tSGameGlobals = SGameGlobalsGet();
		uint32 dwNow = GameGetServerTime();
	}
	else if (ptReplayGlobals != NULL && ReplayIsReplayGame())
	{
		sReadReplayMessage(REPLAY_MSG_TYPE_HASH);
	}
}






void ReplayTestInitializeForApp()

{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ReplayTestGlobals *& ptReplayTest = tAppGlobals.ptReplayTestGlobals;
	ptReplayTest = (ReplayTestGlobals *)SigmaMemoryNew(sizeof(ReplayTestGlobals), 4, NULL, TRUE);


    ptReplayTest->uMaxGameCount = s_varTestMaxGames.Get();
    ptReplayTest->uSpawnDeltaSeconds = s_varTestStartInterval.Get();
}




void ReplayTestDisposeForApp()
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ReplayTestGlobals *& ptReplayTest = tAppGlobals.ptReplayTestGlobals;
    XFREE( ptReplayTest->ptReplayBuffer, NULL );
    XFREE( ptReplayTest, NULL );
}





static BOOL sReplayTestDetectFailure(
	uint32 dwGameUpdateCount)

{
	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;


	return ptReplayTest->nUpdatesStalledInARow >= 5;
}
































static void sReplayTestStartNewGames()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;


	uint32 uNewMaxGameCount = s_varTestMaxGames.Get();
	uint32 uNewStartInterval = s_varTestStartInterval.Get();

	if (uNewMaxGameCount != ptReplayTest->uMaxGameCount ||
		uNewStartInterval != ptReplayTest->uSpawnDeltaSeconds)
	{
		ptReplayTest->uMaxGameCount = uNewMaxGameCount;
		ptReplayTest->uSpawnDeltaSeconds = uNewStartInterval;


		ptReplayTest->uBaseTime = ptReplayTest->dwTestTime;
		ptReplayTest->uBaseGameCount = SGameManager::GetGameCount();
	}

	if (ptReplayTest->nUpdatesStalledInARow > 0)
	{
		return;
	}



	uint32 uDesiredDeltaGameCount = (ptReplayTest->dwTestTime - ptReplayTest->uBaseTime) / (uNewStartInterval * 60);
	uint32 uTotalDesiredNumGames = ptReplayTest->uBaseGameCount + uDesiredDeltaGameCount + 1;

	if (uTotalDesiredNumGames > uNewMaxGameCount)
	{
		uTotalDesiredNumGames = uNewMaxGameCount;
	}

	ptReplayTest->uCurrentNumGames = SGameManager::GetGameCount();

	int32 nGamesToStart = uTotalDesiredNumGames - ptReplayTest->uCurrentNumGames;


	if (nGamesToStart > 0)
	{

		sReplayMultiserverPlayRecordedGame();
	}
}




static void sReplayTestDisplayStats()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

	int32 nTimeSinceUpdate = ptReplayTest->dwTestTime - ptReplayTest->dwLastUpdate;
	ptReplayTest->dwLastUpdate = ptReplayTest->dwTestTime;

	ptReplayTest->dwUpdateTimeMin = Math::Min<uint32>(nTimeSinceUpdate, ptReplayTest->dwUpdateTimeMin);
	ptReplayTest->dwUpdateTimeMax = Math::Max<uint32>(nTimeSinceUpdate, ptReplayTest->dwUpdateTimeMax);
	ptReplayTest->dwUpdateCount++;

	int32 nTimeSinceStats = ptReplayTest->dwTestTime - ptReplayTest->dwLastStatTime;

	if (nTimeSinceStats < 60)
	{
		return;
	}

	ptReplayTest->dwLastStatTime = ptReplayTest->dwTestTime;

	int32 nCurGames = SGameManager::GetGameCount();

	int32 nAveTicksPerUpdate = nTimeSinceStats / ptReplayTest->dwUpdateCount;

	ptReplayTest->nUpdatesStalledInARow = (ptReplayTest->dwUpdateCount < 12 || SGameManager::GetAverageWorkerThreadHz() < 15.0f) ? ptReplayTest->nUpdatesStalledInARow + 1 : 0;











	CRefString sMessage;
	sMessage.Format("REPLAYTEST: %d Games, Stall Count %d, Update Count %d, Avg Worker Thread Hz %d",
		nCurGames,
		ptReplayTest->nUpdatesStalledInARow,
		ptReplayTest->dwUpdateCount,
		SGameManager::GetAverageWorkerThreadHz());




	ptReplayTest->dwUpdateCount = 0;
	ptReplayTest->dwUpdateTimeMin = 10000;
	ptReplayTest->dwUpdateTimeMax = 0;

}




static void sReplayTestStop()

{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;





    SigmaMemoryTotals tMemTotals;
    SigmaMemoryGetTotals(tMemTotals);








    SGameIterator tIt;
    while (SGameManager::GameIterate(tIt))
    {

        SGame & tSGame = *tIt.ptElement;
        if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PLAY_MULTI_SERVER_REPLAY))
        {





            SGameManager::SignalKillGame(tSGame.id);
        }
    }





    s_varStatsUpdateSecondsInterval.Set(0, XVarBase::DIRTY_DEFAULT);
    GameStop();


    ptReplayTest->bActive = FALSE;
}




static void sReplayCPUTestStop()

{
	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

	uint32 dwAvgGameTime = ptReplayTest->dwTotalGameTime / REPLAY_CPU_TEST_RUN_COUNT;
	uint32 dwRealGameTimeMs = ptReplayTest->dwGameTicks * 1000 / 60;
	float flGamesPerCpu = (float)dwRealGameTimeMs / (float)dwAvgGameTime;
















	GameStop();
}





void ReplayTestUpdate(
	uint32 dwGameUpdateCount)
{
    if (!s_varEnabled.Get())
	{
		return;
	}

	AppGlobals & tAppGlobals = sgtAppGlobals;
	ReplayTestGlobals * ptReplayTest = tAppGlobals.ptReplayTestGlobals;

	if ((tAppGlobals.dwGameCreateParamFlags & (1 << GAMECREATEPARAM_REPLAY_SERVER_CPU_TEST)) &&
		SGameManager::GetGameCount() == 0)
	{
		if (ptReplayTest->nCpuTestRunCount == REPLAY_CPU_TEST_RUN_COUNT)
		{
			sReplayCPUTestStop();
		}
		else
		{
			sReplayServerCPUTestPlayRecordedGame();
		}
	}

	if (!ptReplayTest->bActive)
	{
		return;
	}

	uint32 dwNow = AppGetTicks();
	ptReplayTest->dwTestTime = dwNow - ptReplayTest->dwStartTime;


	BOOL bFailure = sReplayTestDetectFailure(dwGameUpdateCount);

	if (bFailure)
	{
		sReplayTestStop();
		return;
	}

	sReplayTestDisplayStats();

	sReplayTestStartNewGames();
}









































































































































































static void sSaveBattleNetAccountDef(
    const D3::Account::SavedDefinition & tSavedDefinition)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

    if ( ptReplayGlobals == NULL || ( ptReplayGlobals->eReplayMode != ReplayModeRecording ) )
    {
        return;
    }


    uint8 auAccountDefinitionBuffer[65536];
    int32 nByteCount = tSavedDefinition.ByteSize();

    BOOL bProtoCompiled = tSavedDefinition.SerializePartialToArray(auAccountDefinitionBuffer, sizeof(auAccountDefinitionBuffer));


    if (!bProtoCompiled)
    {
        return;
    }

    sWriteGenericDataChunk(-1, REPLAY_MSG_TYPE_ACCOUNT_DATA, auAccountDefinitionBuffer, nByteCount);
}








static void sSaveBattleNetHero(
    const D3::Hero::SavedDefinition & tSavedDefinition)
{
	GameCommonData & tGCD = GameGetActiveGameCommonData();
	ReplayGlobals * ptReplayGlobals = tGCD.ptReplayGlobals;

	if ( ptReplayGlobals == NULL || ( ptReplayGlobals->eReplayMode != ReplayModeRecording ) )
	{
		return;
	}


    uint8 auHeroDefinitionBuffer[65536];
    int32 nByteCount = tSavedDefinition.ByteSize();

    BOOL bProtoCompiled = tSavedDefinition.SerializePartialToArray(auHeroDefinitionBuffer, sizeof(auHeroDefinitionBuffer));


    if (!bProtoCompiled)
    {
        return;
    }

    sWriteGenericDataChunk(-1, REPLAY_MSG_TYPE_HERO_DATA, auHeroDefinitionBuffer, nByteCount);
}







BOOL ReplayPlayerJoin(
    const OnlineService::PlayerResolveData & tResolvedPlayer)
{
    if (tResolvedPlayer.uJoinFlags & (1 << OnlineService::PLAYER_JOIN_REPLAY_JOIN_RECORDED))
        return FALSE;
    sSaveBattleNetAccountDef(tResolvedPlayer.tAccountDefinition);
    sSaveBattleNetHero(tResolvedPlayer.tHeroDefinition);



    return TRUE;
}
