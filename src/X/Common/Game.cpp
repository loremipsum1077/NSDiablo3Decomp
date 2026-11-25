// Original file: X/Common/Game.cpp
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "Game.h"
#include "Definitions/Definitions.h"
#include "../Sigma/DataStructures/XBitArray.h"
#include "../Sigma/Stopwatch.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Sigma/Math/Vector.h"
#include "Subsystem.h"
#include "GlobalSNO.h"
#include "DebugChannel/XDebugChannelControl.h"
#include "ThreadGlobals.h"
#include "JobManager/JobManager.h"
#include "../Client/Battlenet/Hub.h"
#include "Networking/XSocket.h"
#include "../Sigma/SigmaProf.h"
#include "../Sigma/GamePerf.h"
#include "../Client/ImageUtils/ImageCompressAPI.h"
#include "Console/IO/ConsoleIO.h"
#include "OnlineService/OnlineService.h"
#include "../Client/Definitions/TextureDefinition.h"
#include "../Client/Definitions/AppearanceDefinition.h"
#include "Definitions/GlobalDefinition.h"
#include "Definitions/ObserverDefinition.h"
#include "Definitions/SceneDefinition.h"
#include "Definitions/WorldDefinition.h"
#include "Definitions/ActorDefinition.h"
#include "Definitions/AnimSetDefinition.h"
#include "Definitions/AnimationDefinition.h"
#include "Definitions/StringListDefinition.h"
#include "Definitions/PowerDefinition.h"
#include "Definitions/MarkerSetDefinition.h"
#include "Definitions/HeroDefinition.h"
#include "Definitions/AdventureDefinition.h"
#include "Definitions/FlagSetDefinition.h"
#include "Definitions/SceneGroupDefinition.h"
#include "Definitions/QuestDefinition.h"
#include "../Server/Definitions/ConductorDefinition.h"
#include "Definitions/SkillKitDefinition.h"
#include "Definitions/MonsterDefinition.h"
#include "Definitions/RecipeDefinition.h"
#include "Definitions/TimedEventDefinition.h"
#include "Definitions/ActDefinition.h"
#include "Definitions/LoreDefinition.h"
#include "Definitions/BossEncounterDefinition.h"
#include "Definitions/AccoladeDefinition.h"
#include "../Client/Definitions/ExplosionDefinition.h"
#include "../Client/Definitions/SurfaceDefinition.h"
#include "../Client/Definitions/FontDefinition.h"
#include "../Client/Definitions/Animation2DDefinition.h"
#include "../Client/Definitions/ShaderDefinition.h"
#include "../Client/Definitions/UIDialogDefinition.h"
#include "../Client/Definitions/SoundBankDefinition.h"
#include "../Client/Definitions/SoundDefinition.h"
#include "../Client/Definitions/RopeDefinition.h"
#include "../Client/Definitions/WeatherDefinition.h"
#include "../Client/Definitions/ShakeDefinition.h"
#include "../Client/Definitions/MaterialDefinition.h"
#include "../Client/Definitions/ControllerSchemeDefinition.h"
#include "Definitions/AnimTreeDefinition.h"
#include "../Client/Definitions/ShaderMapDefinition.h"
#include "Definitions/PhysMeshDefinition.h"
#include "../Server/Definitions/ScriptDefinition.h"
#include "../Server/Definitions/DungeonFinderDefinition.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Definitions/EncounterDefinition.h"
#include "Definitions/ConversationDefinition.h"
#include "Definitions/ConversationListDefinition.h"
#include "Definitions/LevelAreaDefinition.h"
#include "Definitions/QuestRangeDefinition.h"
#include "../Server/Definitions/AIDefinition.h"
#include "Definitions/ConditionDefinition.h"
#include "Definitions/AccountDefinition.h"
#include "Definitions/TreasureClassDefinition.h"
#include "Definitions/TutorialDefinition.h"
#include "../Client/Definitions/AmbientSoundDefinition.h"
#include "../Client/Definitions/PhysicsDefinition.h"
#include "../Client/Definitions/EffectGroupDefinition.h"
#include "../Client/Definitions/ClothDefinition.h"
#include "../Client/Definitions/TrailDefinition.h"
#include "../Client/Definitions/ParticleDefinition.h"
#include "../Client/Definitions/LightDefinition.h"
#include "../Client/Definitions/MusicDefinition.h"
#include "../Client/Definitions/VibrationDefinition.h"
#include "../Client/Definitions/ReverbDefinition.h"
#include "../../NintendoSDK/Include/nn/nn_ApplicationId.h"
#include "../Client/CPlayer.h"
#include "../Client/CPlayerInput.h"
#include "../Client/Physics/DominoClient.h"
#include "../Client/Console/GamerProfile.h"
#include "../Client/Console/Online.h"
#include "../Client/Prefetch.h"
#include "../Client/UserInterface/UI.h"
#include "../Client/UserInterface/Controls/UIConsoleEditControl.h"
#include "../Client/Font/Font.h"
#include "../Client/Network/NetStat.h"
#include "../Client/Gfx/Gfx.h"
#include "../Client/Input/Input.h"
#include "Items.h"
#include "World.h"
#include "Marker/Marker.h"
#include "Shell/CmdLine.h"
#include "Shell/Shell.h"
#include "../Server/SGame.h"
#include "../Server/SGameManager.h"
#include "../Server/SWorld.h"
#include "../Server/Powers/SpellEffect.h"
#include "../Server/LuaScript/LuaHook.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../../BlizzardCore/Include/Blizzard/File.h"
#include "../../NintendoSDK/Include/nn/account/account_Types.h"
#include "Console/ConsoleLicenses.h"
#include "SNO/SNOGroups.h"
#include "SNO/SNOML.h"
#include "../Client/UserInterface/Console/ConsoleScreenManager.h"
#include "SNO/SNOFiles.h"
#include "GameBalance.h"
#include "Attributes/FastAttribute.h"
#include "../Client/CGame.h"
#include "../Sigma/RefString.h"
#include "../Sigma/SigmaTiming.h"
#include "../Sigma/buildver.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../Client/Battlenet/Subsystems/ModalNotificationManager.h"
#include "../Client/UserInterface/Console/ConsoleModalNotificationManager.h"
#include "../Client/Definitions/PreferencesDefinition.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Sigma.h"
#include "../Sigma/UnicodeText.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../NintendoSDK/Include/nn/nn_Result.h"
#include "../../NintendoSDK/Include/nn/nn_BitTypes.h"
#include <stdio.h>
#include <stdlib.h>
#include "Protobuf/OnlineService.pb.h"
#include "../Server/LuaScript/LuaAbortThreadWatcher.h"
#include "../Sigma/MemoryPools.h"
#include "GameTemplate.h"
#include "GameSwig.h"
#include <string.h>
#include "GameCommonData.h"
#include "Protobuf/ChallengeRifts.pb.h"
#include "Protobuf/Leaderboard.pb.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Random.h"
#include "GameConstants.h"
#include "../Sigma/DataStructures/XPtr64.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "ActorCommonData.h"
#include "Player.h"
#include "SceneCommon.h"
#include "../Sigma/StringManip.h"

// ---- PART-LOCAL ADDITIONS (to be moved into headers) ----
// ---- from part 0 ----

// Headers that do not exist in src/ yet: minimal declarations; each comment names the header.
// Subsystem entry points (sgtAppSubsystems, line 2167): X/Server/TagMap.h, X/Server/ReplayTest.h,
// X/Server/SGameStats.h, X/Common/GameDebug.h, X/Client/UserInterface/ProfUI.h, X/Common/GlobalGBIDList.h,
// X/Client/Appearance.h, X/Common/Account.h, X/Common/Hero.h, X/Server/LuaScript.h, X/Server/ScriptEvent.h,
// X/Common/Quests.h, X/Client/ImageUtils/ImageCollections.h, X/Common/Networking/Network.h,
// X/Common/Attributes/FastAttrib.h, X/Common/ItemCrafting.h, X/Client/Cutscenes.h (in table order).
void TagMapInitializeForApp(); void TagMapDisposeForApp(); void ReplayTestInitializeForApp(); void ReplayTestDisposeForApp();
void SGameStatsInitializeForApp(); void SGameStatsDisposeForApp(); void GameDebugInitializeForApp(); void GameDebugDisposeForApp();
void ProfUIInitializeForApp(); void ProfUIDisposeForApp(); void GlobalGBIDListInitializeForApp(); void GlobalGBIDListDisposeForApp();
void AppearanceInitializeForApp(); void AppearanceDisposeForApp(); void AccountInitializeForApp(); void AccountDisposeForApp();
void HeroInitializeForApp(); void HeroDisposeForApp(); void LuaScriptInitializeForApp(); void LuaScriptDisposeForApp();
void ScriptEventInitializeForApp(); void ScriptEventDisposeForApp(); void QuestsInitializeForApp(); void QuestsDisposeForApp();
void ImageCollectionsInitialize(); void ImageCollectionsDispose(); void NetworkInitializeForApp(); void NetworkDisposeForApp();
void FastAttribInitializeForApp(); void FastAttribDisposeForApp(); void ItemCrafting_InitializeForApp(); void ItemCrafting_DisposeForApp();
void CutscenesInitializeForApp(); void CutscenesDisposeForApp();
namespace Console {
namespace DLCManager { void InitializeForApp(); void DisposeForApp(); }  // X/Common/Console/DLCManager.h
namespace UpdateManager { void InitializeForApp(); void DisposeForApp(); }  // X/Client/Console/UpdateManager.h
namespace NFP { void InitializeForApp(); void DisposeForApp(); }  // X/Client/Console/NFP.h
class Achievements { public: static void InitializeForApp(); static void DisposeForApp(); };  // X/Common/Achievements/Console/ConsoleAchievements.h
}  // namespace Console
namespace SNOPreload { void InitializeForApp(); void DisposeForApp(); }  // X/Client/SNOPreload.h
namespace Tutorial { void InitForApp(); void DisposeForApp(); }  // X/Common/Tutorial.h
namespace ChromaEffects { void InitializeForApp(); void DisposeForApp(); }  // X/Client/ChromaEffects.h
// X/Common/DebugChannel/XDebugChannelConsole.h, X/Sigma/DebugChannelInterface/XDebugChannelMemory.h and
// X/Sigma/DebugChannelInterface/XDebugChannelProperty.h.
void XDebugChannelConsoleInit(); void XDebugChannelConsoleDispose(); void XDebugChannelMemoryInit(); void XDebugChannelMemoryUpdate(); void XDebugChannelPropertyInit();
// BlizzardCore/Include/Blizzard/Process.h (Process_NX64.cpp; JobManager.cpp declares Sleep the same way).
namespace Blizzard { namespace Process { bool GetProcessDirectory(char * buf, int bufMax); bool Sleep(Blizzard::Time::Millisecond milliseconds); } }
void ConfigureServerIdleThread(Blizzard::Thread::Thread tThread);  // X/Common/Shell/Main.h (MainNX64.cpp:208)
namespace SoundPlatform { void ReleaseMemory(); }  // X/Client/Sound/SoundPlatform.h (SoundPlatformFModEx.cpp:1815)
void SNetworkStart(); void SNetworkStop();  // X/Server/Network/SNetwork.h (SNetwork.cpp:44, :69)
BOOL ReplayIsPlayingServerCPUTest();  // X/Server/Replay.h
SNO UIStartup_GetInitialWorld();  // X/Client/UserInterface/Interface/UIStartup.h (UIStartup.cpp:242)
namespace Console { namespace Online { uint32 GameGetClientGameConnectionID(); } }  // X/Client/Console/ConsoleGameConnection.h:89
void MemoryUtilsGenerateHeapStatsString(char * szString, size_t nBufferSize);  // X/Sigma/MemoryUtils.h (src/X/Sigma/MemoryUtils.cpp)

// Forward declarations of this file's own static helpers that the tables and earlier functions
// reference before their definitions (lines 5367, 5403, 5412, 3281, 3307, 3327, 3342, 482, 549, 561, 705, 4467, 4533).
static void InitDebugChannel(); static void DisposeDebugChannel(); static void UpdateDebugChannel();
static void sGroupDefinitionsCreateForApp(); static void sGroupDefinitionsDisposeForApp();
static void sGameDefinitionsInitialize(); static void sGameDefinitionsDispose();
static void sPrecacheDefinitions(); static void sApplyUpdates(); static void sAppGlobalsInitialize(); static void sAppGlobalsDispose();
static uint32 sServerIdleProc(void * ptParam); static void sTracePerformance();

// Game.cpp:19 (reference .cpp): the registration table entry type; GAME_DEFINITION is an invented macro name.
struct GroupDefinitionRegistrationRecord {  // size 0x10
    GroupDefinitionCreationFcn pfnCreate;  // 0x00
    GroupDefinition ** pptGroupDef;  // 0x08
};
#define GAME_DEFINITION(Type, ptGroupDef) { GroupDefinition::Create<Type>, (GroupDefinition **)&ptGroupDef }

// ---- from part 1 ----
// (all adopted into src/ headers or carried by part 0)
// ---- from part 2 ----

// NintendoSDK headers not adopted in src/ (prototypes from the public SDK, bodies not in the DWARF):
// nn/account/account_Api.h, nn/fs/fs_SaveData.h, nn/fs/fs_Mount.h, nn/ec/ec_ShopApi.h.
namespace nn {
namespace account { void Initialize(); nn::Result OpenPreselectedUser(UserHandle * pOutHandle); nn::Result GetUserId(Uid * pOut, const UserHandle & handle); void CloseUser(const UserHandle & handle); }
namespace fs { bool IsSaveDataExisting(const nn::account::Uid & user); nn::Result MountSaveData(const char * name, const nn::account::Uid & user); void Unmount(const char * name); }
namespace ec { nn::Result ShowShopAddOnContentList(nn::ApplicationId id, const nn::account::UserHandle & handle); }
}  // namespace nn
// BlizzardCore/Include/Blizzard/File.h declares Exists (it returns the Existence enum, missing/isFile/isDirectory,
// not bool).
// IsMountedROM is a global in File_NX64.cpp:46.
bool IsMountedROM();
// X/Client/Console/DLCManager.h (DLCManager.cpp:385-529), X/Client/Console/Utilities.h (UtilitiesNX64.cpp:253),
// X/Client/Console/GamerProfile.h (GamerProfileNX64.cpp; nn::account::UserHandle is not known to that header yet).
namespace Console {
namespace DLCManager { uint32 GetLicenses(); bool GetLocaleContentLicense(XLocale nLocale, ConsoleLicense & uLicense); bool GetLocaleContentIndex(XLocale nLocale, int64_t & nContentIndex); bool GetLocaleAOCMountPath(XLocale nLocale, CRefString & szMountPath); }
namespace Utilities { XLocale GetSystemLanguage(); }
namespace GamerProfile { nn::account::UserHandle GetUserHandle(int32 nUserIndex); }
}  // namespace Console

// ---- from part 3 ----
// (all adopted into src/ headers or carried by part 0)
// ---- from part 4 ----
// (all adopted into src/ headers or carried by part 0)
// ---- from part 5 ----
// (all adopted into src/ headers or carried by part 0)
// ---- from part 6 ----
// (all adopted into src/ headers or carried by part 0)
// ---- from part 7 ----
// (all adopted into src/ headers or carried by part 0)
// ---- END PART-LOCAL ADDITIONS ----















static const nn::ApplicationId s_applicationId = { 0x01001B300B9BE000ULL };

// D3_REGISTRY_PATH / D3_REGISTRY_SUBKEY_LOCALE (lines 289-290): the linker dropped both pointers
// and their strings (no symbol, no text in the binary), so their values cannot be recovered.













// const char * D3_REGISTRY_PATH = ...;
// const char * D3_REGISTRY_SUBKEY_LOCALE = ...;

static XVarScope s_scopeGame("Game", NULL);
XVarUint64 s_varPlatinumLimit(s_scopeGame, "PlatinumLimit", 600);
XVarUint32 s_varPlatinumResetTime(s_scopeGame, "PlatinumResetTime", 0);

int32 s_nGameFrameCounter = 0;


const GlobalSNO cgeGlobalSnoDefaultStartWorld = GLOBALSNO_WORLD_UTILITY_SERVER;
// Invented name: the 24-byte .rodata table right after cgeGlobalSnoDefaultStartWorld, the
// (from, to) locale fallback pairs ProcessFallbackLanguage walks; the DWARF dropped the variable.
static const XLocale sgaLocaleFallbacks[3][2] = {
    { XLOCALE_PORTUGUESE_PORTUGAL, XLOCALE_PORTUGUESE_BRAZIL }, { XLOCALE_SPANISH_LATAM, XLOCALE_SPANISH_EU },
    { XLOCALE_SPANISH_EU, XLOCALE_SPANISH_LATAM } };
// 66 entries, one per line (307-372); gptGroupDef* have derived types, hence the cast in GAME_DEFINITION.
static GroupDefinitionRegistrationRecord sgptGameDefinitions[] = {
    GAME_DEFINITION(TextureGroupDef, gptGroupDefTexture),
    GAME_DEFINITION(ShaderMapGroupDef, gptGroupDefShaderMap),
    GAME_DEFINITION(AppearanceGroupDef, gptGroupDefAppearance),
    GAME_DEFINITION(PhysMeshGroupDef, gptGroupDefPhysMesh),
    GAME_DEFINITION(GlobalGroupDef, gptGroupDefGlobal),
    GAME_DEFINITION(ObserverGroupDef, gptGroupDefObserver),
    GAME_DEFINITION(SceneGroupDef, gptGroupDefScene),
    GAME_DEFINITION(WorldGroupDef, gptGroupDefWorld),
    GAME_DEFINITION(ActorGroupDef, gptGroupDefActor),
    GAME_DEFINITION(AnimSetGroupDef, gptGroupDefAnimSet),
    GAME_DEFINITION(AnimationGroupDef, gptGroupDefAnimation),
    GAME_DEFINITION(StringListGroupDef, gptGroupDefStringList),
    GAME_DEFINITION(ScriptGroupDef, gptGroupDefScript),
    GAME_DEFINITION(DungeonFinderGroupDef, gptGroupDefDungeonFinder),
    GAME_DEFINITION(PowerGroupDef, gptGroupDefPower),
    GAME_DEFINITION(MarkerSetGroupDef, gptGroupDefMarkerSet),
    GAME_DEFINITION(HeroGroupDef, gptGroupDefHero),
    GAME_DEFINITION(GameBalanceGroupDef, gptGroupDefGameBalance),
    GAME_DEFINITION(EncounterGroupDef, gptGroupDefEncounter),
    GAME_DEFINITION(AdventureGroupDef, gptGroupDefAdventure),
    GAME_DEFINITION(ConversationGroupDef, gptGroupDefConversation),
    GAME_DEFINITION(ConversationListGroupDef, gptGroupDefConversationList),
    GAME_DEFINITION(FlagSetGroupDef, gptGroupDefFlagSet),
    GAME_DEFINITION(SceneGroupGroupDef, gptGroupDefSceneGroup),
    GAME_DEFINITION(LevelAreaGroupDef, gptGroupDefLevelArea),
    GAME_DEFINITION(QuestGroupDef, gptGroupDefQuest),
    GAME_DEFINITION(QuestRangeGroupDef, gptGroupDefQuestRange),
    GAME_DEFINITION(ConductorGroupDef, gptGroupDefConductor),
    GAME_DEFINITION(SkillKitGroupDef, gptGroupDefSkillKit),
    GAME_DEFINITION(MonsterGroupDef, gptGroupDefMonster),
    GAME_DEFINITION(AIStateGroupDef, gptGroupDefAIState),
    GAME_DEFINITION(AIBehaviorGroupDef, gptGroupDefAIBehavior),
    GAME_DEFINITION(RecipeGroupDef, gptGroupDefRecipe),
    GAME_DEFINITION(TimedEventGroupDef, gptGroupDefTimedEvent),
    GAME_DEFINITION(ActGroupDef, gptGroupDefAct),
    GAME_DEFINITION(LoreGroupDef, gptGroupDefLore),
    GAME_DEFINITION(ConditionGroupDef, gptGroupDefCondition),
    GAME_DEFINITION(AccountGroupDef, gptGroupDefAccount),
    GAME_DEFINITION(TreasureClassGroupDef, gptGroupDefTreasureClass),
    GAME_DEFINITION(BossEncounterGroupDef, gptGroupDefBossEncounter),
    GAME_DEFINITION(TutorialGroupDef, gptGroupDefTutorial),
    GAME_DEFINITION(AccoladeGroupDef, gptGroupDefAccolade),
    GAME_DEFINITION(AmbientSoundGroupDef, gptGroupDefAmbientSound),
    GAME_DEFINITION(PhysicsGroupDef, gptGroupDefPhysics),
    GAME_DEFINITION(ExplosionGroupDef, gptGroupDefExplosion),
    GAME_DEFINITION(SurfaceGroupDef, gptGroupDefSurface),
    GAME_DEFINITION(EffectGroupGroupDef, gptGroupDefEffectGroup),
    GAME_DEFINITION(ClothGroupDef, gptGroupDefCloth),
    GAME_DEFINITION(TrailGroupDef, gptGroupDefTrail),
    GAME_DEFINITION(ParticleGroupDef, gptGroupDefParticle),
    GAME_DEFINITION(FontGroupDef, gptGroupDefFont),
    GAME_DEFINITION(Animation2DGroupDef, gptGroupDefAnimation2D),
    GAME_DEFINITION(ShaderGroupDef, gptGroupDefShader),
    GAME_DEFINITION(UIDialogGroupDef, gptGroupDefUI),
    GAME_DEFINITION(LightGroupDef, gptGroupDefLight),
    GAME_DEFINITION(SoundBankGroupDef, gptGroupDefSoundBank),
    GAME_DEFINITION(SoundGroupDef, gptGroupDefSound),
    GAME_DEFINITION(MusicGroupDef, gptGroupDefMusic),
    GAME_DEFINITION(RopeGroupDef, gptGroupDefRope),
    GAME_DEFINITION(WeatherGroupDef, gptGroupDefWeather),
    GAME_DEFINITION(ShakeGroupDef, gptGroupDefShake),
    GAME_DEFINITION(VibrationGroupDef, gptGroupDefVibration),
    GAME_DEFINITION(MaterialGroupDef, gptGroupDefMaterial),
    GAME_DEFINITION(ReverbGroupDef, gptGroupDefReverb),
    GAME_DEFINITION(ControlSchemeDefinitionGroupDef, gptGroupDefControlScheme),
    GAME_DEFINITION(AnimTreeGroupDef, gptGroupDefAnimTree),
};
#undef GAME_DEFINITION





























static GroupDefinition * sgptGameDefinitionsPerGroup[MAX_SNO_GROUPS];

static const int32 cgnNumGameDefinitions = sizeof(sgptGameDefinitions) / sizeof(sgptGameDefinitions[0]);





AppGlobals sgtAppGlobals;
GroupDefinition ** s_patGroups = NULL;
BOOL sgfAppActivated = FALSE;
static blz::vector<uint32> s_arSafeTicksPerSec;


BOOL sgbRunServerIdleThread = FALSE;
Blizzard::Thread::Thread sgtServerIdleThread = NULL;
Blizzard::Lock::RMutex sgtServerIdleMutex;
nn::account::UserHandle unk_71019079f0;  // 2.7.7: preselected user handle, opened by FileSystemGetLocale and kept open (SNOFiles.cpp uses it)

















































static XDebugChannelControl * s_pDebugChannelControl = NULL;










static void sPrecacheDefinitions()
{
    if (!DefinitionShouldPrecache())
        return;





    DefinitionSetCurrentlyPreCaching(TRUE);



    for (int32 i = 0; i < cgnNumGameDefinitions; i++)
    {


        PooledSNONameList listSNOFiles(GetSnoNameListPool());
        int32 nCount = SNOFileEnumerateSNOsForGroup(s_patGroups[i]->GetGroup(), &listSNOFiles);

        if (!nCount)
            continue;







        s_patGroups[i]->GetPrecacheDependencyCount();  // vtable slot 8, result unused in the shipped build







        POS pos = listSNOFiles.GetHeadPosition();
        while (pos)
        {
            SNOName & snonameFile = listSNOFiles.GetNext(pos);











            const void * lpDefinition = s_patGroups[i]->LockR(snonameFile.snoHandle, FALSE);
            s_patGroups[i]->UnlockR((void *&)lpDefinition);
        }
    }

    DefinitionSetCurrentlyPreCaching(FALSE);


}






static void sApplyUpdates()
{

    SNOFilesApplyAllHotfixInFolder(CRefString(SNOFilesGetConsoleDataPathOverride()) + "\\Update\\CPKs", FALSE);
    SNOFilesApplyAllHotfixInFolder(CRefString(SNOFilesGetConsoleDataPathOverride()) + "\\Update2\\CPKs", FALSE);
}






static void sAppGlobalsInitialize()
{
    for (uint32 i = 1; i < GAME_CLOCK_TICKS_PER_SECOND;
         i++)
    {
        if (GAME_CLOCK_TICKS_PER_SECOND % i == 0)
        {
            s_arSafeTicksPerSec.push_back(i);
        }
    }

    sgtAppGlobals.fAppExitEnabled = TRUE;
    sgtAppGlobals.nGameGfxInitCount = 0;

    sgtAppGlobals.ptCGameActive = NULL;
    sgtAppGlobals.PlayingCGameInit();


    sgtAppGlobals.ptGBIDPool = new GBIDPool;
    sgtAppGlobals.ptGBIDPool->SetBlockCount_Tracked(1024);
    AppDrawFlagReset();
    sgtAppGlobals.eHWClassOverride = HARDWARECLASS_HIGH;
    sgtAppGlobals.nSubSceneOverride = -1;
    sgtAppGlobals.eDebugDisplayMode = DDM_DISABLED;

    sgtAppGlobals.flParticleScalar = 1.0f;

    sgtAppGlobals.dwDebugFlags = 0;
    sgtAppGlobals.dwDebugFlags2 = 0;
    sgtAppGlobals.dwDebugFlags3 = 0;

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {

        sgtAppGlobals.dwDebugFlags |= (1 << APP_GLOBAL_BATTLENET_VALIDATION_DISABLED_BIT);
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_NO_PREFETCH_BIT)) sgtAppGlobals.dwDebugFlags |= (1 << APP_GLOBAL_NO_PREFETCH_BIT); else sgtAppGlobals.dwDebugFlags &= ~(1 << APP_GLOBAL_NO_PREFETCH_BIT);
    sgtAppGlobals.flSpeedHack = 0.0f;
    sgtAppGlobals.dwDisplayAnimLeaf = -1;
    sgtAppGlobals.snoPVPWorldOverride = -1;
    sgtAppGlobals.dwMinFrameMs = 0;
    sgtAppGlobals.dwFrameCounter = 0;

    GameClockInitialize(sgtAppGlobals.tAppClock);
    GameClockInitialize(sgtAppGlobals.tSGameClock);
    sgtAppGlobals.dwTicksSinceAppStart = 0;
    sgtAppGlobals.bInAppIdle = FALSE;
    sgtAppGlobals.bInServerIdle = FALSE;
    sgtAppGlobals.m_dwTicksPerSec = 60;

    sgtAppGlobals.fDoScheduledPrecache = FALSE;
    sgtAppGlobals.fFrameOut = FALSE;
    sgtAppGlobals.uSleepingReasons = 0;
    sgtAppGlobals.bAllowSleeping = CmdLineGetParams()->eTestType != TESTTYPE_WORLD_PERF;
    sgtAppGlobals.tKeyDebugAttrib = MakeAttribKey(-1, 0);

    sgtAppGlobals.tConstraintDebugData.snoActorToTrack = -1;
    sgtAppGlobals.tConstraintDebugData.szConstraintToTrack[0] = 0;


    sgtAppGlobals.tCollisionDebugData.snoActorToTrack = -1;
    sgtAppGlobals.tCollisionDebugData.eMaterialToTrack = (Shape::Material)-1;
    sgtAppGlobals.tCollisionDebugData.nNumSamples = 0;
    sgtAppGlobals.uPerfTraceInterval = 10000;
    sgtAppGlobals.uLastPerfTrace = 0;
    sgtAppGlobals.nTraceCategoryPage = 0;

    sgtAppGlobals.ptGlobalSNOs = new GlobalSNOArray;

    sgtAppGlobals.snoForceBounty = -1;
    sgtAppGlobals.eForcedDefaultAct = (Act)-1;























    if (!GameIsServerOnly())
    {



        UIConsole::InitializeForApp();
    }



    sgtAppGlobals.ptScriptDefsPurged = new SNOArray;
    sgtAppGlobals.ptConversationDefsPurged = new SNOArray;
    sgtAppGlobals.ptPowerDefsPurged = new SNOArray;
    sgtAppGlobals.ptConductorDefsPurged = new SNOArray;
    sgtAppGlobals.tScriptDefPurgeMutex.Create();
    sgtAppGlobals.tConversationDefPurgeMutex.Create();
    sgtAppGlobals.tPowerDefPurgeMutex.Create();
    sgtAppGlobals.tConductorDefPurgeMutex.Create();
    sgtAppGlobals.tGlobalServerSemaphore.Create();
    sgtAppGlobals.tGlobalServerSemaphore.Set();
    sgtAppGlobals.tGlobalDisposeServerMutex.Create();

    sgtAppGlobals.bShowSidekickDebug = FALSE;








    sgtAppGlobals.rgbaGreenscreen = RGBAColor(0, 0, 0);
    sgtAppGlobals.flNormalMapFactor = 1.0f;










    sgtAppGlobals.nLastGameClockTime = 0;
    sgtAppGlobals.nLastGameClockDelta = 0;


}

static void sAppGlobalsDispose()
{



    if (!GameIsServerOnly())
    {



        UIConsole::DisposeForApp();
    }




    delete sgtAppGlobals.ptGBIDPool; sgtAppGlobals.ptGBIDPool = NULL;
    delete sgtAppGlobals.ptGlobalSNOs; sgtAppGlobals.ptGlobalSNOs = NULL;


    delete sgtAppGlobals.ptConductorDefsPurged; sgtAppGlobals.ptConductorDefsPurged = NULL;
    delete sgtAppGlobals.ptPowerDefsPurged; sgtAppGlobals.ptPowerDefsPurged = NULL;
    delete sgtAppGlobals.ptConversationDefsPurged; sgtAppGlobals.ptConversationDefsPurged = NULL;
    delete sgtAppGlobals.ptScriptDefsPurged; sgtAppGlobals.ptScriptDefsPurged = NULL;
    sgtAppGlobals.tGlobalDisposeServerMutex.Destroy();
    sgtAppGlobals.tGlobalServerSemaphore.Destroy();
    sgtAppGlobals.tConductorDefPurgeMutex.Destroy();
    sgtAppGlobals.tPowerDefPurgeMutex.Destroy();
    sgtAppGlobals.tConversationDefPurgeMutex.Destroy();
    sgtAppGlobals.tScriptDefPurgeMutex.Destroy();
}













void AppOverrideDRLGSeed(int32 nSeed)
{
    sgtAppGlobals.nDRLGSeedOverride = nSeed;
}












int32 AppGetOverriddenDRLGSeed()
{

    return sgtAppGlobals.nDRLGSeedOverride;
}




BOOL AppIsDRLGSeedOverridden()
{

    return AppGetOverriddenDRLGSeed() != 0;
}




// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
void AppClearDRLGSeedOverride()
{

    AppOverrideDRLGSeed(0);
}


// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
void AppRenderLayerHide(int32 nFlag, BOOL bValue)
{

    sgtAppGlobals.tHideRenderLayer.Set(nFlag, bValue);
}
void AppDrawFlagSet(int32 nFlag, BOOL bValue)
{


    sgtAppGlobals.tDrawFlags.Set(nFlag, bValue);
}




// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
BOOL AppRenderLayerToggle(int32 nFlag)
{
    sgtAppGlobals.tHideRenderLayer.Swap(nFlag);
    return sgtAppGlobals.tHideRenderLayer.Get(nFlag);
}
// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
BOOL AppDrawFlagToggle(int32 nFlag)
{

    sgtAppGlobals.tDrawFlags.Swap(nFlag);
    return sgtAppGlobals.tDrawFlags.Get(nFlag);
}


BOOL AppRenderLayerIsHidden(int32 nFlag)
{

    return sgtAppGlobals.tHideRenderLayer.Get(nFlag);
}

BOOL AppDrawFlagIsSet(int32 nFlag)
{

    return sgtAppGlobals.tDrawFlags.Get(nFlag);
}




// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
void AppRenderLayerReset()
{
    sgtAppGlobals.tHideRenderLayer.Zero();
}
void AppDrawFlagReset()
{

    sgtAppGlobals.tDrawFlags.Zero();

    AppDrawFlagSet(APP_DRAW_CLICK_EFFECT_BIT, TRUE);
}














BOOL AppIsBlizzardServer()
{

    return CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZARD_GAME_SERVER);
}




// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
BOOL AppIsBlizzardGameServer()
{

    return CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZARD_GAME_SERVER);
}

// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
BOOL AppIsProductionBnetServer()
{

    return CmdLineGetParams()->tBitArray.Get(CMDLINE_PRODUCTION_BNET_SERVER);
}


BOOL GameCommonDataExists()
{

    if (ThreadGlobals::Get().m_ptActiveGameCommonData == NULL)
        return FALSE;




    return ThreadGlobals::Get().m_ptActiveGameCommonData->ptActiveGCD != NULL;
}








BOOL GameActiveGameCommonDataIsClient()
{

    return ThreadGlobals::Get().m_ptActiveGameCommonData->bClientActive;
}





BOOL GameActiveGameCommonDataIsServer()
{

    return ThreadGlobals::Get().m_ptActiveGameCommonData->bClientActive == FALSE;
}





void GameSetActiveGameCommonData(GameCommonData * ptActiveGCD, BOOL bClientActive)
{

    ThreadGlobals::Get().m_ptActiveGameCommonData->bClientActive = bClientActive;
    ThreadGlobals::Get().m_ptActiveGameCommonData->ptActiveGCD = ptActiveGCD;
}





// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
ScopedLeaveAndReenterServerCode::ScopedLeaveAndReenterServerCode(BOOL bReturnToClient)
{
    m_idSGame = GameServerCodeLeave_Tracked(bReturnToClient);
}



// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
ScopedLeaveAndReenterServerCode::~ScopedLeaveAndReenterServerCode()
{
    _GameServerCodeEnter(m_idSGame);
}


ScopedEnterAndLeaveServerCode::ScopedEnterAndLeaveServerCode()
{
    m_bReturnToClient = (_GameServerCodeEnter() != FALSE);
}




// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
ScopedEnterAndLeaveServerCode::ScopedEnterAndLeaveServerCode(SGameID idSGame)
{
    m_bReturnToClient = (_GameServerCodeEnter(idSGame) != FALSE);
}



ScopedEnterAndLeaveServerCode::~ScopedEnterAndLeaveServerCode()
{
    GameServerCodeLeave_Tracked(m_bReturnToClient);
}





BOOL _GameServerCodeEnter(const SGameID idSGame)
{









    if (idSGame == -1)
        return FALSE;





    if (!GameActiveGameCommonDataIsClient())
    {

        if (SGameManager::GameGet().id != idSGame)
        {
            DisplayInternalError(NULL, "",
                __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }

        return FALSE;
    }





    uint32 uGameCount = SGameManager::GetGameCount();


    if (uGameCount == 0)
    {
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }




    GameSetActiveGameCommonData(NULL, FALSE);




    SGameManager::SetCurrentGame(idSGame);


    SGame & tSGame = SGameManager::GameGet();
    GameSetActiveGameCommonData(&tSGame.tGCD, FALSE);






    return TRUE;
}

BOOL GameServerCodeEnter_Track()
{











    return FALSE;
}


BOOL _GameServerCodeEnter()
{




    if (GameActiveGameCommonDataIsClient())
    {





        if (!GameServerCodeCanEnter())
        {
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }






        return _GameServerCodeEnter(AppServerGetOnlyGame());
    }


    return FALSE;
}




BOOL GameServerCodeCanEnter()
{


    uint32 uGameCount = SGameManager::GetGameCount();
    return uGameCount != 0;
}




















SGameID GameServerCodeLeave_Tracked(BOOL bReturnToClient)
{






    SGameID idSGame = -1;



    if (ThreadGlobals::Get().m_ptActiveGameCommonData == NULL)
        return idSGame;



    BOOL bIsClient = GameActiveGameCommonDataIsClient();

    if (bReturnToClient && !bIsClient)
    {














        if (SGameIsValid())

            idSGame = SGameManager::GameGet().id;




        GameSetActiveGameCommonData(&CGameGet().tGCD, TRUE);
        SGameManager::SetCurrentGame(-1);
    }















    return idSGame;
}



// line 1187: static void sFileSystemDispose()  [inlined into 1 functions]
static void sFileSystemDispose()
{
    // Tick count (TRACE compiled out): keep the stopwatch the DWARF records.
    //
    //
    //
    //
    Stopwatch tStopwatch;

    sGameDefinitionsDispose();



    SNOMLDispose();
    SNOFilesDispose();
    SNOGroupsDispose();

    GroupDefinition::DisposeSNOMap();
}



// line 1210: static void sFileSystemInitialize(CRefString sDataDirectory, const SigmaPlatform eSigmaPlatform, const XLocale eXLocale)  [inlined into 1 functions]
static void sFileSystemInitialize(CRefString sDataDirectory, const SigmaPlatform eSigmaPlatform, const XLocale eXLocale)
{
    //
    //
    //
    //
    //
    //
    SNOGroupsInitialize();






    sGameDefinitionsInitialize();









    SNOFilesInitialize(sDataDirectory, eSigmaPlatform, eXLocale);



    SigmaMemoryDumpOSStats();




    SNOMLInitialize(s_patGroups);



    GroupDefinition::InitializeSNOMap();
}







































// line 1290: BOOL LocaleIsInstalled(const LPCSTR szLocaleString)  [0x71004318b0]
BOOL LocaleIsInstalled(const LPCSTR szLocaleString)
{
    char szMpqName[4096];
    BOOL bMPQInstalled;
    BOOL bLooseFileFolderExists = FALSE;
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    sprintf(szMpqName, "%s\\%s_Common.%s", SNOFilesGetMPQPath().str(), szLocaleString, SNOFilesGetMPQExtension());


    bMPQInstalled = Blizzard::File::Exists(szMpqName);



    if (!bMPQInstalled)
    {
        XLocale nLocale = XLocaleFromString(szLocaleString);
        int64_t nContentIndex;
        ConsoleLicense uContentLicense;


        if (Console::DLCManager::GetLocaleContentIndex(nLocale, nContentIndex) &&
            Console::DLCManager::GetLocaleContentLicense(nLocale, uContentLicense) &&
            (Console::DLCManager::GetLicenses() & (1 << uContentLicense)))
        {
            CRefString szMountPath;
            Console::DLCManager::GetLocaleAOCMountPath(nLocale, szMountPath);

            sprintf(szMpqName, "%s\\%s_Common.%s", szMountPath.str(), szLocaleString, SNOFilesGetMPQExtension());
            bMPQInstalled = Blizzard::File::Exists(szMpqName);
        }
    }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    return bMPQInstalled || bLooseFileFolderExists;
}



// line 1391: static XLocale DetectFirstInstalledLocale()  [inlined into 1 functions]
static XLocale DetectFirstInstalledLocale()
{

    XLocale eLocale = XLOCALE_ENGLISH_US;

    for (int32 i = XLOCALE_LOCALE_START; i < NUM_XLOCALES; i++)
    {
        LPCSTR szLocaleString = XLocaleToString(i);
        if (LocaleIsInstalled(szLocaleString))
        {
            eLocale = i;
            break;
        }
    }

    return eLocale;
}




// line 1413: static void DetectInstalledLocales(blz::vector<int, blz::allocator<int> > & tDetectedLocales, bool bSilent)  [inlined into 2 functions]
// always_inline: with a plain static the clang 5 inliner defers vector::push_back out of this
// helper (shouldBeDeferred), the 17-iteration loop then fully unrolls and the helper no longer
// inlines into its two callers; the original has the loop rolled and push_back inlined.
static inline __attribute__((always_inline)) void DetectInstalledLocales(blz::vector<int, blz::allocator<int> > & tDetectedLocales, bool bSilent)
{
    //
    //
    //
    //
    //
    //
    for (int32 i = XLOCALE_LOCALE_START; i < NUM_XLOCALES; i++)
    {
        LPCSTR szLocaleString = XLocaleToString(i);
        if (LocaleIsInstalled(szLocaleString))
        {
            tDetectedLocales.push_back((XLocale)i);
        }
    }
}
















































// line 1482: void ProcessFallbackLanguage(XLocale & eXLocale, const blz::vector<int, blz::allocator<int> > & tDetectedLocales)  [inlined into 1 functions]
void ProcessFallbackLanguage(XLocale & eXLocale, const blz::vector<int, blz::allocator<int> > & tDetectedLocales)
{
    //
    //
    //
    //
    //
    //
    //
    //
    if (blz::find(tDetectedLocales.begin(), tDetectedLocales.end(), eXLocale) != tDetectedLocales.end())
        return;


    for (int32 i = 0; i < 3; i++)
    {
        if (eXLocale == sgaLocaleFallbacks[i][0] &&
            blz::find(tDetectedLocales.begin(), tDetectedLocales.end(), sgaLocaleFallbacks[i][1]) != tDetectedLocales.end())
        {
            eXLocale = sgaLocaleFallbacks[i][1];
            break;
        }
    }
}

// line 1508: XLocale FileSystemGetLocale()  [0x71004319e0]
// NON_MATCHING: only the inlined vector<int>::push_back grow path differs (original `cmp x8,x26; csel x8,x26,x8,lo`
// reusing the hoisted size+1; ours `csinc x8,x8,x24,hs` plus a `b`/`mov` register shuffle, +8 bytes, 980 vs 972).
// The shape is fixed inside blz/vector.h (reserve(m_capacity+1) gets nuw on the 63-bit field, so GVN-PRE's copy of
// ++m_size is not hoisted with it); every other instruction matches.
// o02: on DWARF lines 1508-1699 (#line); the 8-indent of rows 1621-1669 (cols 20/13) puts everything after the first
// find inside `if (... == end)`; locals on their DWARF lines (tUserId 1631 after Initialize 1629, szMountName 1641).
#line 1508
XLocale FileSystemGetLocale()

{
    const CmdLineParams * ptCmdLineParams = CmdLineGetParams();
    XLocale eXLocale = ptCmdLineParams->eXLocale;












    blz::vector<int, blz::allocator<int> > tDetectedLocales;
    DetectInstalledLocales(tDetectedLocales, false);























































































    if (blz::find(tDetectedLocales.begin(), tDetectedLocales.end(), eXLocale) == tDetectedLocales.end())
    {





        eXLocale = Console::Utilities::GetSystemLanguage();




        if (UsePreselectedUser())
        {

            nn::account::Initialize();

            nn::account::Uid tUserId;

            nn::Result OpenResult = nn::account::OpenPreselectedUser(&unk_71019079f0);


            nn::Result IdResult = nn::account::GetUserId(&tUserId, unk_71019079f0);




            char szMountName[4096];
            Blizzard::String::Format(szMountName, sizeof(szMountName), "save%d", 0);

            if (nn::fs::IsSaveDataExisting(tUserId))
            {
                nn::Result MountResult = nn::fs::MountSaveData(szMountName, tUserId);
                if (MountResult.IsSuccess())
                {
                    char szFilePath[4096];
                    const char * szFilename = "prefs.dat";
                    Blizzard::String::Format(szFilePath, sizeof(szFilePath), "%s:/%s", szMountName, szFilename);

                    Prefs::Load(szFilePath);
                    const GameplayPrefs & GameplayPrefs = Prefs::GetGameplayPrefs();


                    eXLocale = GameplayPrefs.GetSelectedAudioLocale();


                    nn::fs::Unmount(szMountName);
                }
            }


            // 2.7.7: the handle stays open in unk_71019079f0 (no CloseUser).
        }


        if (eXLocale == XLOCALE_INVALID)
        {
            eXLocale = XLOCALE_ENGLISH_EU;
        }

        ProcessFallbackLanguage(eXLocale, tDetectedLocales);

        if (blz::find(tDetectedLocales.begin(), tDetectedLocales.end(), eXLocale) == tDetectedLocales.end())
        {

            if (blz::find(tDetectedLocales.begin(), tDetectedLocales.end(), (XLocale)XLOCALE_ENGLISH_US) != tDetectedLocales.end())
            {
                eXLocale = XLOCALE_ENGLISH_US;
            }
            else
            {
                eXLocale = DetectFirstInstalledLocale();
            }
        }
    }








    return eXLocale;

}
#line 1574
































































































































// line 1702: void OnConfirmMissingSystemLocaleOffer(const int32 nUserIndex, const UIID & tId)  [inlined into 1 functions]
void OnConfirmMissingSystemLocaleOffer(const int32 nUserIndex, const UIID & tId)
{
    nn::account::UserHandle tPrimaryUserHandle = Console::GamerProfile::GetUserHandle(nUserIndex);

    nn::ec::ShowShopAddOnContentList(s_applicationId, tPrimaryUserHandle);
}


// line 1711: void OnCloseMissingSystemLocaleOffer(const int32 nUserIndex, LocaleOfferCallback pfnOnLocaleOfferComplete, const UIID & tId)  [inlined into 1 functions]
void OnCloseMissingSystemLocaleOffer(const int32 nUserIndex, LocaleOfferCallback pfnOnLocaleOfferComplete, const UIID & tId)
{
    if (pfnOnLocaleOfferComplete)
    {
        pfnOnLocaleOfferComplete(nUserIndex);
    }
}



// line 1722 (o02: on DWARF lines 1722-1777 via #line; bPrompt declared after the early returns, DWARF 1742).
// NON_MATCHING: same push_back grow-path residual as FileSystemGetLocale (csel vs csinc, +8 bytes, 1232 vs 1224).
#line 1722
bool ShowSystemLocaleAOCOffer(
    const int32 nUserIndex,
    LocaleOfferCallback pfnOnLocaleOfferComplete)
{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return false;
    }

    if (!UsePreselectedUser())
    {
        return false;
    }







    bool bPrompt = false;
    XLocale eXLocale = Prefs::GetGameplayPrefs().GetSelectedAudioLocale();

    blz::vector<int, blz::allocator<int> > tDetectedLocales;
    DetectInstalledLocales(tDetectedLocales, true);

    BOOL bNotInstalled = blz::find(tDetectedLocales.begin(), tDetectedLocales.end(), eXLocale) == tDetectedLocales.end();
    if (bNotInstalled)
    {



        NotificationCreationParams tParams;
        tParams.nControllerIndex = nUserIndex;
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int, const UIID &>::bind<&OnConfirmMissingSystemLocaleOffer>(nUserIndex);
        tParams.tCallbacks.pfnOnClose = OnlineService::internal::function_binder_3<void, int, LocaleOfferCallback, const UIID &>::bind<&OnCloseMissingSystemLocaleOffer>(nUserIndex, pfnOnLocaleOfferComplete);
        tParams.uszMessage = StringListFetch("ConsoleUI:MissingLocaleContentOffer_Switch");

        Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
        bPrompt = true;
    }
    else if (eXLocale != UnicodeText::GetCurrentLocale())
    {



        NotificationCreationParams tParams;
        tParams.nControllerIndex = nUserIndex;
        tParams.uszMessage = StringListFetch("ConsoleUI:LocaleNeedsReboot_Switch");
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_3<void, int, LocaleOfferCallback, const UIID &>::bind<&OnCloseMissingSystemLocaleOffer>(nUserIndex, pfnOnLocaleOfferComplete);

        Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
        bPrompt = true;
    }
    return bPrompt;
}
#line 1778

// line 1779: bool UsePreselectedUser()  [0x7100431eb0, inlined into 2 functions]
bool UsePreselectedUser()
{
    // On the Switch the preselected user is only usable when the game runs from the mounted ROM.
    //
    //
    //
    return IsMountedROM();
}























// CROSS-PART: owned by part 2. Linker-stripped (low_pc 0 in the CU dump; 0x190 bytes, no bytes to
// verify). Its ComplexTypeDescriptorList local (line 1823, destroyed at 1827) is the TU's second
// XChainedFixedBlockAllocator<T>::Dispose caller: with it ChainedFixedBlockAllocator::Dispose keeps
// FixedBlockAllocator::Dispose inlined (cost 430) and stays out of line in sAppGlobalsDispose, as
// in the original. The body between the two lines is a guess.
void ValidateTypeDescriptors()
{









    ComplexTypeDescriptorList tList;
    for (ComplexTypeDescriptor * ptDescriptor = ComplexTypeDescriptor::GetRegistrationHead(); ptDescriptor != NULL; ptDescriptor = ptDescriptor->GetNextRegisteredDescriptor())
        tList.AddTail_Tracked(ptDescriptor);
    ComplexTypeDescriptor::ValidateDescriptors(tList);
}
// line 1832: void FileSystemInitializeForApp()  [0x7100432390]
void FileSystemInitializeForApp()
{
    const CmdLineParams * ptCmdLineParams = CmdLineGetParams();
    SigmaPlatform eSigmaPlatform = SIGMAPLATFORM_NX64;

    CRefString sDataDirectory = CRefString().Format("Data_D3");

    sDataDirectory = SNOFilesGetConsoleDataPathOverride();

    XLocale eXLocale = FileSystemGetLocale();
    if (eXLocale == XLOCALE_INVALID)
    {
        exit(0);
    }

    LPCSTR sDataDirectoryOverride = ptCmdLineParams->sDataDirectory;
    if (*sDataDirectoryOverride)
    {
        sDataDirectory = sDataDirectoryOverride;
    }

    sFileSystemInitialize(sDataDirectory, eSigmaPlatform, eXLocale);
}



































// line 1891: void FileSystemDisposeForApp()  [0x7100432490]
void FileSystemDisposeForApp()
{
    sFileSystemDispose();
}










































// line 1938: XMemoryAllocatorInterface * AppGetCurrentMemoryInterface()  [0x7100432500]
XMemoryAllocatorInterface * AppGetCurrentMemoryInterface()
{



    if (ThreadGlobals::Get().m_ptActiveGameCommonData == NULL ||
        GameActiveGameCommonDataIsClient())
        return NULL;



    {
        SGame & tSGame = SGameManager::GameGet();
        return tSGame.ptMemoryInterface;
    }
}





// line 1960: static RefStringMemPool * sGameUpcallForRefStringPool()  [0x71004326e0]
static RefStringMemPool * sGameUpcallForRefStringPool()
{




    if (ThreadGlobals::Get().m_ptActiveGameCommonData == NULL ||
        ThreadGlobals::Get().m_ptSGameCurrent == NULL ||
        GameActiveGameCommonDataIsClient())
        return NULL;





    {
        SGame & tSGame = SGameManager::GameGet();
        return tSGame.ptRefStringMemPool;
    }
}



// line 1984: static void * sGetGameDATAIDMemoryPool()  [0x71004325b0]
static void * sGetGameDATAIDMemoryPool()
{




    if (ThreadGlobals::Get().m_ptActiveGameCommonData == NULL ||
        GameActiveGameCommonDataIsClient())
        return NULL;






    {
        SGame & tSGame = SGameManager::GameGet();
        return tSGame.ptDATAIDMemPool;
    }
}



// line 2008: static void * sGetGameSphereNodeMemoryPool()  [0x7100432630]
static void * sGetGameSphereNodeMemoryPool()
{




    if (ThreadGlobals::Get().m_ptActiveGameCommonData == NULL ||
        GameActiveGameCommonDataIsClient())
        return NULL;






    {
        SGame & tSGame = SGameManager::GameGet();
        return tSGame.ptSphereNodeMemPool;
    }
}


// line 2031: void MemoryPoolsInitializeForApp()  [0x7100432580]
void MemoryPoolsInitializeForApp()
{
    MemoryPoolsInitialize();
    MemoryPoolsSetMemoryPoolUpCalls(sGetGameDATAIDMemoryPool, sGetGameSphereNodeMemoryPool);
}



// line 2040: void MemoryPoolsDisposeForApp()  [0x71004326b0]
void MemoryPoolsDisposeForApp()
{

    MemoryPoolsDispose();
}



// line 2049: void GameStringInitializeForApp()  [0x71004326c0]
void GameStringInitializeForApp()
{

    RefStringInitializeForApp();
    RefStringSetUpcallForMemoryPool(sGameUpcallForRefStringPool);
}





// line 2061: void GameStringDisposeForApp()  [0x7100432780]
void GameStringDisposeForApp()
{

    RefStringDisposeForApp();
}




































































































// 58 entries, one per line (lines 2167-2224). Every sSubSystemName in the binary is the same
// empty string (0x7100d6a4a3); the two NULL disposers are NULL in the original table.
static SubsystemSetup sgtAppSubsystems[] = {
    { "", WinSockInit, WinSockDispose },
    { "", InitDebugChannel, DisposeDebugChannel },
    { "", MemoryPoolsInitializeForApp, MemoryPoolsDisposeForApp },
    { "", Stopwatch::InitializeForApp, Stopwatch::DisposeForApp },
    { "", google::protobuf::StartupProtobufLibrary, google::protobuf::ShutdownProtobufLibrary },
    { "", sAppGlobalsInitialize, sAppGlobalsDispose },
    { "", GameStringInitializeForApp, GameStringDisposeForApp },
    { "", ThreadGlobals::InitializeForApp, ThreadGlobals::DisposeForApp },
    { "", SigmaProfInitializeForApp, SigmaProfDisposeForApp },
    { "", SGameManager::InitializeForApp, SGameManager::DisposeForApp },
    { "", TagMapInitializeForApp, TagMapDisposeForApp },
    { "", ReplayTestInitializeForApp, ReplayTestDisposeForApp },
    { "", SGameStatsInitializeForApp, SGameStatsDisposeForApp },
    { "", GameDebugInitializeForApp, GameDebugDisposeForApp },
    { "", ProfUIInitializeForApp, ProfUIDisposeForApp },
    { "", GamePerfInitialize, GamePerfDispose },
    { "", GlobalGBIDListInitializeForApp, GlobalGBIDListDisposeForApp },
    { "", DominoWorld::InitializeForApp, DominoWorld::DisposeForApp },
    { "", AppearanceInitializeForApp, AppearanceDisposeForApp },
    { "", sGroupDefinitionsCreateForApp, sGroupDefinitionsDisposeForApp },
    { "", Console::IO::InitializeForApp, Console::IO::DisposeForApp },
    { "", Console::DLCManager::InitializeForApp, Console::DLCManager::DisposeForApp },
    { "", FileSystemInitializeForApp, FileSystemDisposeForApp },
    { "", OnlineService::InitializeForApp, OnlineService::DisposeForApp },
    { "", AccountInitializeForApp, AccountDisposeForApp },
    { "", HeroInitializeForApp, HeroDisposeForApp },
    { "", GameGlobalsInit, GameGlobalsDispose },
    { "", sPrecacheDefinitions, NULL },
    { "", Prefetch::InitForApp, Prefetch::DisposeForApp },
    { "", LuaScriptInitializeForApp, LuaScriptDisposeForApp },
    { "", ScriptEventInitializeForApp, ScriptEventDisposeForApp },
    { "", SGameInitializeApp, SGameDisposeApp },
    { "", QuestsInitializeForApp, QuestsDisposeForApp },
    { "", ImageCompressAPIInitialize, ImageCompressAPIDispose },
    { "", ImageCollectionsInitialize, ImageCollectionsDispose },
    { "", sApplyUpdates, NULL },
    { "", SNOPreload::InitializeForApp, SNOPreload::DisposeForApp },
    { "", Console::Achievements::InitializeForApp, Console::Achievements::DisposeForApp },
    { "", GamelessGameGlobalsInit, GamelessGameGlobalsDispose },
    { "", UIInitializeForApp, UIDisposeForApp },
    { "", Hub::InitializeForApp, Hub::DisposeForApp },
    { "", FontInitializeForApp, FontDisposeForApp },
    { "", NetworkInitializeForApp, NetworkDisposeForApp },
    { "", CPlayerInitializeForApp, CPlayerDisposeForApp },
    { "", CPlayerInputInitializeForApp, CPlayerInputDisposeForApp },
    { "", FastAttribInitializeForApp, FastAttribDisposeForApp },
    { "", GameBalanceInitializeForApp, GameBalanceDisposeForApp },
    { "", ItemsInitializeForApp, ItemsDisposeForApp },
    { "", ItemCrafting_InitializeForApp, ItemCrafting_DisposeForApp },
    { "", Tutorial::InitForApp, Tutorial::DisposeForApp },
    { "", JobManager::InitializeForApp, JobManager::DisposeForApp },
    { "", ChromaEffects::InitializeForApp, ChromaEffects::DisposeForApp },
    { "", Console::GamerProfile::InitializeForApp, Console::GamerProfile::DisposeForApp },
    { "", Console::UpdateManager::InitializeForApp, Console::UpdateManager::DisposeForApp },
    { "", Console::Online::InitializeForApp, Console::Online::DisposeForApp },
    { "", CutscenesInitializeForApp, CutscenesDisposeForApp },
    { "", GlobalDefinitionInitializeForApp, GlobalDefinitionDisposeForApp },
    { "", Console::NFP::InitializeForApp, Console::NFP::DisposeForApp },
};






























































































static const int32 cnNumAppSubsystems = sizeof(sgtAppSubsystems) / sizeof(sgtAppSubsystems[0]);



// line 2327: void GameParams::Export(D3::OnlineService::GameCreateParams & tGameParams, LPCSTR szGameTag) const  [0x7100432790]
void GameParams::Export(D3::OnlineService::GameCreateParams & tGameParams, LPCSTR szGameTag) const
{

    tGameParams.set_game_type(eGameType);
    tGameParams.set_creation_flags(dwCreationFlags);
    tGameParams.set_name(szGameName);

    if (GameTypeIsPvP(eGameType))
    {
        tGameParams.mutable_pvp()->set_sno_world(snoInitialWorld);
    }
    else
    {
        D3::OnlineService::CampaignOrAdventureModeCreateParams & coop = *tGameParams.mutable_campaign_or_adventure_mode();
        coop.set_act(eAct);
        coop.set_sno_quest(snoInitialQuest);
        coop.set_quest_step_id(nInitialQuestStepUID);
        if (bResumeFromSave)
        {
            coop.mutable_resume_from_save_hero_id()->set_id_high(0);
            coop.mutable_resume_from_save_hero_id()->set_id_low(uHeroId);
        }
        else
        {
            coop.clear_resume_from_save_hero_id();
        }


        coop.set_handicap_level(nHandicapLevel);
        if (*szGameTag)
        {
            coop.set_game_tag(szGameTag);
        }
        else
        {
            coop.clear_game_tag();
        }

        coop.set_monster_level(nMonsterLevel);
    }
}



// line 2372: BOOL AppInitialize()  [0x7100432be0]
BOOL AppInitialize()
{

    char szWorkingDir[4096];

    if (!ShellAppCanInitialize())
        return FALSE;




    if (AppIsBlizzardServer())
    {
        ErrorManagerSuppressUserInteraction(TRUE);
    }

    SigmaTimingInitialize(GAME_CLOCK_TICKS_PER_SECOND);







    if (s_varWorkerThreadCount.Get() == 0)
    {






        LuaHook_Create();
    }










    const CmdLineParams * ptCmdLineParams = CmdLineGetParams();
    if (!ptCmdLineParams->tBitArray.Get(CMDLINE_DISABLE_CHANGING_WORKING_DIRECTORY))
    {
        CHAR szProcessPath[4096];
        BOOL fSuccess = Blizzard::Process::GetProcessDirectory(szProcessPath, sizeof(szProcessPath));


        Blizzard::File::SetWorkingDirectory(szProcessPath);
    }


    Blizzard::File::GetWorkingDirectory(szWorkingDir, sizeof(szWorkingDir));








    SubsystemsInitialize(sgtAppSubsystems, cnNumAppSubsystems);

















    if (!SNOFileExists(GLOBAL_SNO, GlobalSNOGet(GLOBALSNO_GLOBALS)))
    {







        int32 nSnoFileCount = SNOFileGetFileTableCount();

        CHAR szWorkingDirectory[4096];

        Blizzard::File::GetWorkingDirectory(szWorkingDirectory, sizeof(szWorkingDirectory));
        return FALSE;
    }











    sgtAppGlobals.fInitialized = TRUE;



    ErrorManagerAddCrashMetadata("Locale", XLocaleToString(UnicodeText::GetCurrentLocale()));














































    sgbRunServerIdleThread = TRUE;
    Blizzard::Thread::Create(sServerIdleProc, NULL, &sgtServerIdleThread, "ServerIdle", 1, 0x10000);
    Blizzard::Thread::SetPriority(sgtServerIdleThread, Blizzard::Thread::PRIORITY_ABOVE_NORMAL);



    ConfigureServerIdleThread(sgtServerIdleThread);
    SigmaProfRegisterThread(sgtServerIdleThread, "ServerIdle", 0x1000);
    Blizzard::Thread::Resume(sgtServerIdleThread);
    return TRUE;
}
// line 2544: void AppDispose()  [0x7100432f10]
void AppDispose()
{


    SigmaProfUnregisterThread(sgtServerIdleThread);
    sgbRunServerIdleThread = FALSE;
    Blizzard::Thread::WaitForever(sgtServerIdleThread);
    Blizzard::Thread::Close(sgtServerIdleThread);

















    if (!sgtAppGlobals.fInitialized)
        return;











    sgtAppGlobals.fInitialized = FALSE;


    SubsystemsDispose(sgtAppSubsystems, cnNumAppSubsystems);


    if (s_varWorkerThreadCount.Get() == 0)
    {

        LuaHook_Destroy();
    }




    SoundPlatform::ReleaseMemory();
}



GameType GameGetGameType()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();

    return tGCD.eGameType;
}

// line 2614: uint32 GameParamsGetGameSeed()  [inlined into 3 functions]


uint32 GameParamsGetGameSeed()
{

    return GetMachineTicks();
}

// line 2623: void GameParamsSetDefault(GameParams & tParams, GameType eGameType)  [inlined into 7 functions]


void GameParamsSetDefault(GameParams & tParams, GameType eGameType)
{

    tParams.bValid = FALSE;

    tParams.snoInitialWorld = tParams.snoInitialQuest = tParams.nInitialQuestStepUID = tParams.snoInitialHero = SNO_NONE;


    tParams.bResumeFromSave = TRUE;

    tParams.dwSeed = 0;
    tParams.eGameType = eGameType;

    tParams.dwCreationFlags = 0;
    tParams.dwInitialUpdateCount = 0;
    tParams.dwMaxGameTime = 0;
    tParams.eMode = GAMEMODE_NONE;
    tParams.nGameParts = GAME_PARTS_NONE;

    tParams.eAct = ACT_1;
    tParams.nMonsterLevel = -1;
    tParams.nHandicapLevel = 0;
    tParams.szServerAddress[0] = 0;

    tParams.idGameConnection = 0;
    tParams.ePreferredAdventureAct = ACT_1;
    tParams.arFreeKeyTieredRiftParticipatingHeroID[0] = 0;
    tParams.arFreeKeyTieredRiftParticipatingHeroID[1] = 0;
    tParams.arFreeKeyTieredRiftParticipatingHeroID[2] = 0;
    tParams.arFreeKeyTieredRiftParticipatingHeroID[3] = 0;
    tParams.szReplayFileName[0] = 0;

    tParams.szGameName[0] = 0;
    tParams.tGameId.SetInvalid();
    tParams.idSGame = (SGameID)-1;
    tParams.uHeroId = OnlineService::INVALID_HERO_ID;
    tParams.uServerAuthToken = 0;

    AppGlobals & tAppGlobals = sgtAppGlobals;
    if (eGameType == GT_NOTHING) tAppGlobals.dwDebugFlags |= (1 << APP_GLOBAL_INSTANT_RESURRECTION_BIT); else tAppGlobals.dwDebugFlags &= ~(1 << APP_GLOBAL_INSTANT_RESURRECTION_BIT);


    tParams.tTemplate = GameTemplate::GetGameTemplate(eGameType);

    tParams.uEntranceGUIDOverride = (uint32)-1;
}

// line 2672: void GameStartMenu(Act eLoadingScreenAct)  [0x7100432fe0]

void GameStartMenu(Act eLoadingScreenAct)
{






    GameParams tParams;
    GameParamsSetDefault(tParams, GT_STARTUP);

    tParams.bValid = TRUE;


    tParams.snoInitialWorld = UIStartup_GetInitialWorld();
    tParams.eMode = GAMEMODE_MENU;
    tParams.nGameParts = GAME_PARTS_CLIENT;
    tParams.dwInitialUpdateCount = INITIAL_UPDATE_COUNT_DEFAULT;
    tParams.dwMaxGameTime = (uint32)-1;
    tParams.dwSeed = GameParamsGetGameSeed();
    tParams.eAct = eLoadingScreenAct;




    strcpy(tParams.szGameName, "GameStartMenu");

    GameQueueGameStart(tParams);
}

// line 2755: void GameStartServer()  [0x71004332f0]




















































void GameStartServer()
{

    GameParams tParams;
    GameParamsSetDefault(tParams, GT_DIABLO3);

    tParams.bValid = TRUE;
    tParams.snoInitialWorld = GlobalSNOGet(cgeGlobalSnoDefaultStartWorld);
    tParams.dwSeed = GameParamsGetGameSeed();
    tParams.eMode = GAMEMODE_PLAYING;
    tParams.nGameParts = GAME_PARTS_BATTLENET_NETWORK;
    tParams.dwInitialUpdateCount = INITIAL_UPDATE_COUNT_DEFAULT;
    tParams.dwMaxGameTime = (uint32)-1;

    strcpy(tParams.szGameName, "GameStartServer");

    GameQueueGameStart(tParams);
}

// line 2778: SNO GetSNOWorldFromGameType(GameType eGameType)  [0x71004332a0, inlined into 1 functions]



SNO GetSNOWorldFromGameType(GameType eGameType)
{

    SNO snoWorld = GlobalSNOGet(GameTemplate::GetGameTemplate(eGameType).eStartWorld);

    if (snoWorld != SNO_NONE)
    {
        return snoWorld;
    }


    DisplayInternalError(NULL, "", 2801, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
}

// line 2836: void GameStartConsoleClient(uint32 idGameConnection, BOOL bIsHost, GameType eGameType, const uint32 dwGameCreateParamFlags, BOOL bResumeFromSave, Act eAct, Act ePreferredAdventureAct, SNO snoQuest, int32 nStepUID, int32 nHandicap, uint32 uEntranceGUIDOverride, int32 nFreeKeyLevel, uint64 * pArrayTieredRiftHeroForFreeKey)  [0x71004335a0]
// NON_MATCHING: 852 vs 848 bytes, register allocation only. The original keeps bResumeFromSave in
// w24 and spills eAct and dwGameCreateParamFlags at the GT_DIABLO3 branch; ours spills the
// bResumeFromSave and eAct phis and moves dwGameCreateParamFlags w25->w24 in both arms. Every
// statement order and if/else form tried (120 if-branch permutations, params before the if,
// != form, assigning through the parameters) gives identical IR.






































void GameStartConsoleClient(
    uint32 idGameConnection,
    BOOL bIsHost,
    GameType eGameType,
    const uint32 dwGameCreateParamFlags,
    BOOL bResumeFromSave,
    Act eAct,
    Act ePreferredAdventureAct,
    SNO snoQuest,
    int32 nStepUID,
    int32 nHandicap,
    uint32 uEntranceGUIDOverride,
    int32 nFreeKeyLevel,
    uint64 * pArrayTieredRiftHeroForFreeKey)
{

    // inferred spacing: rows/inline records anchor tParams 2858, SetDefault 2859, the test 2862, GlobalSNOGet 2864,
    // GetSNOWorldFromGameType 2869, GameParamsGetGameSeed 2874, strcpy 2879, nGameParts 2884, GameQueueGameStart 2900;
    // the unrowed stores between them are packed to fit.



    GameParams tParams;
    GameParamsSetDefault(tParams, eGameType);

    tParams.bValid = TRUE;
    if (eGameType == GT_DIABLO3)
    {
        tParams.snoInitialWorld = GlobalSNOGet(cgeGlobalSnoDefaultStartWorld);
        tParams.snoInitialQuest = snoQuest; tParams.nInitialQuestStepUID = nStepUID;
        tParams.eAct = eAct; tParams.bResumeFromSave = bResumeFromSave;
    }
    else
        tParams.snoInitialWorld = GetSNOWorldFromGameType(eGameType);

    tParams.eMode = GAMEMODE_PLAYING;
    tParams.dwCreationFlags = dwGameCreateParamFlags;

    tParams.dwSeed = GameParamsGetGameSeed();
    tParams.dwInitialUpdateCount = INITIAL_UPDATE_COUNT_DEFAULT;
    tParams.dwMaxGameTime = (uint32)-1;
    tParams.nHandicapLevel = nHandicap;

    strcpy(tParams.szServerAddress, "127.0.0.1");
    tParams.idGameConnection = idGameConnection;
    tParams.ePreferredAdventureAct = ePreferredAdventureAct;
    tParams.uEntranceGUIDOverride = uEntranceGUIDOverride;

    tParams.nGameParts = bIsHost ? GAME_PARTS_CLIENT_SERVER_NETWORK : GAME_PARTS_CLIENT_NETWORK;















    GameQueueGameStart(tParams);
}



// line 2906: void GamePlayWorld(SNO snoWorld, uint32 dwCreationFlags, int32 nHandicap, BOOL bResumeFromSave)  [0x71004338f0]
void GamePlayWorld(SNO snoWorld, uint32 dwCreationFlags, int32 nHandicap, BOOL bResumeFromSave)
{




    GameParams tParams;
    GameParamsSetDefault(tParams, GT_NOTHING);

    tParams.bValid = TRUE;
    tParams.snoInitialWorld = snoWorld;
    tParams.dwSeed = (uint32)-1;
    tParams.eMode = GAMEMODE_PLAYING;
    tParams.dwCreationFlags = dwCreationFlags;
    tParams.dwInitialUpdateCount = INITIAL_UPDATE_COUNT_DEFAULT;
    tParams.dwMaxGameTime = (uint32)-1;
    tParams.nHandicapLevel = nHandicap;
    tParams.bResumeFromSave = bResumeFromSave;

    strcpy(tParams.szServerAddress, "127.0.0.1");
    tParams.idGameConnection = Console::Online::GameGetClientGameConnectionID();
    tParams.nGameParts = Console::Online::PartyIsHost() ? GAME_PARTS_CLIENT_SERVER_NETWORK : GAME_PARTS_CLIENT_NETWORK;



    strcpy(tParams.szGameName, "GamePlayWorld");

    GameQueueGameStart(tParams);
}

// line 2940: void GameQueueGameStart(GameParams & tParams)  [inlined into 5 functions]



void GameQueueGameStart(
    GameParams & tParams)
{
    BOOL fInClientMode = GameActiveGameCommonDataIsClient();
    SGameID idSGame = -1;
    if (!fInClientMode)
    {
        idSGame = GameServerCodeLeave_Tracked(TRUE);
    }

    CGame & tClientData = CGameGet();
    tClientData.tQueuedGameStartParams = tParams;

    if (!fInClientMode)
    {
        _GameServerCodeEnter(idSGame);
    }
}




BOOL GameIsAGameStartQueued()
{

    AppGlobals & tAppGlobals = sgtAppGlobals;


    if (tAppGlobals.ptCGameActive == NULL) {
        return FALSE;
    }



    GameParams & tParams = tAppGlobals.ptCGameActive->tQueuedGameStartParams;

    return tParams.bValid != FALSE;
}









void GameCheckForQueuedGameStart()
{






    CGame & tClientData = CGameGet();

    GameParams & tParams = tClientData.tQueuedGameStartParams;

    if (!tParams.bValid) {
        return;
    }







    Blizzard::Lock::ScopedMutex tLock(sgtServerIdleMutex);





    if (tParams.snoInitialWorld == -1 && tParams.nGameParts != GAME_PARTS_BATTLENET_NETWORK) {







        GameDisposeGames();
        GamelessGameGlobalsInit();
        ShellInvalidateWindow();
    } else {



        GameStartGame(tParams);
    }















    tParams.bValid = FALSE;
}
void GameStop()
{

    GameParams tParams;
    GameParamsSetDefault(tParams, -1);
    tParams.snoInitialWorld = -1;
    tParams.nGameParts = GAME_PARTS_NONE;

    // inferred: the original GameStop copy stores bValid = 1, and GameQueueGameStart itself stores nothing (its removed copy is 0x88).
    tParams.bValid = TRUE;
    GameQueueGameStart(tParams);
}




void GamelessGameGlobalsInit()
{

    GameParams tParams;

    GameParamsSetDefault(tParams, -1);

    tParams.bValid = FALSE;
    tParams.snoInitialWorld = -1;
    tParams.dwSeed = (uint32)-1;
    tParams.dwCreationFlags = 0;
    tParams.dwInitialUpdateCount = 0;
    tParams.dwMaxGameTime = (uint32)-1;
    tParams.eMode = GAMEMODE_NONE;
    tParams.nGameParts = GAME_PARTS_NONE;
    strcpy(tParams.szGameName, "GamelessGameGlobals");


    CGameGlobalsInit(tParams);
}





void GamelessGameGlobalsDispose()
{

}




// CPU boost toggle (UtilitiesNX64.cpp, 0x71000b3c50).
extern "C" void sub_71000b3c50(bool bBoost);










void GameStartGame(const GameParams & tParams)
{

    ShellEnableSaveOnQuit(FALSE);



    GameDisposeGames();

    AppGlobals & tAppGlobals = sgtAppGlobals;
    tAppGlobals.tStartTimeStopwatch.Reset();
    tAppGlobals.nGameParts = tParams.nGameParts;
    tAppGlobals.dwGameCreateParamFlags = tParams.dwCreationFlags;
    if (tParams.eMode == GAMEMODE_PLAYING) {



        tAppGlobals.m_dwTicksPerSec = 60;
    }

    if (tParams.nGameParts == GAME_PARTS_BATTLENET_NETWORK && !AppIsBlizzardServer()) {



        ErrorManagerSuppressUserInteraction(TRUE);
    }

    if (tParams.snoInitialWorld == GlobalSNOGet(GLOBALSNO_WORLD_MARKERSET_TOOL)) {

        PRTransform transformMarkerToScene(cgtPRTransformIdentity);
        transformMarkerToScene.wp = Vector3D(100.0f, 100.0f, 0.0f);
        SetMarkerToSceneTransform(transformMarkerToScene);
    }
    else {

        SetMarkerToSceneTransform(cgtPRTransformIdentity);
    }

    if (tParams.eMode != GAMEMODE_PLAYING) {

        if (!(tParams.eMode >= GAMEMODE_EDITUNITLAYOUT && tParams.eMode <= GAMEMODE_MENU)) {


















            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
    else {



        switch (tParams.nGameParts) {
        case GAME_PARTS_CLIENT:
        case GAME_PARTS_CLIENT_SERVER:
        case GAME_PARTS_CLIENT_NETWORK:
        case GAME_PARTS_CLIENT_SERVER_NETWORK:
        case GAME_PARTS_BATTLENET_NETWORK:
        case GAME_PARTS_HIDDEN_SERVER:
            break;
















        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            break;
        }
    }

    if (tParams.nGameParts == GAME_PARTS_CLIENT_SERVER_NETWORK) {

        SNetworkStart();
    }















    SWorldsSuspendPreload();



    if (tParams.nGameParts == GAME_PARTS_CLIENT_SERVER || tParams.nGameParts == GAME_PARTS_CLIENT_SERVER_NETWORK ||
        tParams.nGameParts == GAME_PARTS_HIDDEN_SERVER) {


        GameParams tParamsModifiable = tParams;
        SGameID idSGame = SGameInitializeGame(tParamsModifiable, NULL);




        if (tParams.eGameType == 6) {

            Console::Online::GameParams tPartyGameParams;
            Console::Online::PartyGetGameParams(tPartyGameParams);
            SWorldLoadRiftShapshot(idSGame,
                                   tPartyGameParams.m_tWeeklyChallengeConfig.challenge_number(),
                                   tPartyGameParams.m_tWeeklyChallengeData.rift_snapshot(),
                                   tPartyGameParams.m_tWeeklyChallengeConfig.challenge_end_unix_time_console(),
                                   false,
                                   tPartyGameParams.m_tWeeklyChallengeConfig.has_challenge_nephalem_orb_multiplier() ? tPartyGameParams.m_tWeeklyChallengeConfig.challenge_nephalem_orb_multiplier() : 1.0f);
        }
    }





    CGameGlobalsInit(tParams);

    if (tParams.eGameType != 0 &&
        tParams.nGameParts != GAME_PARTS_HIDDEN_SERVER &&
        tParams.eMode == GAMEMODE_PLAYING) {

        CGameLoadingScreenEnable(LSC_PLAYER_ENTER_KNOWN);
    }

    if (tParams.nGameParts == GAME_PARTS_CLIENT || tParams.nGameParts == GAME_PARTS_CLIENT_SERVER ||
        tParams.nGameParts == GAME_PARTS_CLIENT_NETWORK || tParams.nGameParts == GAME_PARTS_CLIENT_SERVER_NETWORK) {



        CGameInitializeGame(tParams);
    }



    SWorldsResumePreload();






    float fTime = tAppGlobals.tStartTimeStopwatch.Check();


    sub_71000b3c50(FALSE);
    ShellEnableSaveOnQuit(TRUE);

}




static void sGroupDefinitionsCreateForApp()
{


    s_patGroups = (GroupDefinition **)SigmaMemoryNew((cgnNumGameDefinitions + 1) * sizeof(GroupDefinition *), 4, NULL, FALSE);
    SigmaMemoryClear(sgptGameDefinitionsPerGroup, sizeof(sgptGameDefinitionsPerGroup));

    for (int32 i = 0; i < cgnNumGameDefinitions; i++) {


        *sgptGameDefinitions[i].pptGroupDef = sgptGameDefinitions[i].pfnCreate();

        s_patGroups[i] = *sgptGameDefinitions[i].pptGroupDef;
        sgptGameDefinitionsPerGroup[s_patGroups[i]->GetGroup()] = s_patGroups[i];
    }



    s_patGroups[cgnNumGameDefinitions] = NULL;

}





static void sGroupDefinitionsDisposeForApp()
{

    sGameDefinitionsDispose();

    for (int32 i = cgnNumGameDefinitions - 1; i >= 0; i--) {

        if (*sgptGameDefinitions[i].pptGroupDef) {

            ::delete *sgptGameDefinitions[i].pptGroupDef;
            *sgptGameDefinitions[i].pptGroupDef = NULL;
        }
    }

    SigmaMemoryFree(s_patGroups, NULL);
    s_patGroups = NULL;
}



static void sGameDefinitionsInitialize()
{




    for (int32 i = 0; i < cgnNumGameDefinitions; i++) {


        (*sgptGameDefinitions[i].pptGroupDef)->Init();
    }
}



static void sGameDefinitionsDispose()
{

    for (int32 i = cgnNumGameDefinitions - 1; i >= 0; i--) {

        if (*sgptGameDefinitions[i].pptGroupDef) {

            (*sgptGameDefinitions[i].pptGroupDef)->Dispose();
        }
    }
}




GroupDefinition * GameDefinitionGet(const SNOGroup snoGroup)
{

    if (snoGroup >= 0 && snoGroup < MAX_SNO_GROUPS) {

        return sgptGameDefinitionsPerGroup[snoGroup];
    }



    return NULL;
}


void GameDisposeGames()
{



    SetMarkerToSceneTransform(cgtPRTransformIdentity);

    GameParts nGameParts = GameGetParts();
    AppGlobals & tAppGlobals = sgtAppGlobals;






    if (nGameParts == GAME_PARTS_BATTLENET_NETWORK) {
        if (!AppIsBlizzardServer()) {


            ErrorManagerSuppressUserInteraction(FALSE);
        }
    }



    if (nGameParts == GAME_PARTS_CLIENT_SERVER_NETWORK || nGameParts == GAME_PARTS_BATTLENET_NETWORK) {


        SNetworkStop();
    }


    if (SGameManager::Exists()) {




        Blizzard::Lock::ScopedMutex tLock(sgtServerIdleMutex);

        SGameManager::DisposeGames();
    }













    if (CGameIsValid()) {

        CGameDisposeGame();
    }




    tAppGlobals.nGameParts = GAME_PARTS_NONE;
    tAppGlobals.dwGameCreateParamFlags = 0;
}




BOOL GameLoopRunning()
{

    if (SigmaCurrentlyHandlingError())
    {
        return FALSE;
    }

    CGameGlobals * ptCGameGlobals = CGameGlobalsTryToGet();
    if (ptCGameGlobals == NULL)
    {
        return FALSE;
    }

    return !(ptCGameGlobals->dwFlags & (1 << CGAMEGLOBALS_DISABLE_DRAWING_BIT));
}











BOOL AppIsDrawingWorld()
{

    if (!GameLoopRunning())
    {
        return FALSE;
    }

    if (!GameActiveGameCommonDataIsClient())
    {
        return FALSE;
    }

    return CGameGetCurrentWorld() != SNO_NONE;
}











GameParts GameGetParts()
{

    return sgtAppGlobals.nGameParts;
}




DifficultyLevel GameGetDifficultyLevel()
{
    const GameCommonData & tGCD = GameGetActiveGameCommonData();

    return tGCD.eDifficultyLevel;
}



DifficultyLevel GameGetDifficultyParamsLevel(BOOL bIsFromTieredRift)
{
    int32 nHandicapLevel;
    if (bIsFromTieredRift)
    {
        nHandicapLevel = DL_INFERNO;
    }
    else
    {
        nHandicapLevel = GameGetHandicapLevel();
    }
    if (nHandicapLevel >= DL_NORMAL && nHandicapLevel < DL_INFERNO)
    {
        return nHandicapLevel;
    }



    return DL_INFERNO;
}
int32 GameGetHandicapLevel()
{

    const GameCommonData & tGCD = GameGetActiveGameCommonData();
    return tGCD.nHandicapLevel;
}



HandicapLevel * GameGetInfoForHandicapLevel(int32 nHandicapLevel)
{
    int32 nRowOffset = nHandicapLevel - MINIMUM_HANDICAP_LEVEL_FOR_TABLE;








    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_HANDICAP_LEVELS));
    if (ptGameBalDef)
    {
        int32 nMaxHandicapLevel = ptGameBalDef->tHandicapLevelTable.serHandicapLevels.dwSize / sizeof(HandicapLevel);
        if (nMaxHandicapLevel > 0)
        {
            int32 nTableIndex = Math::Min(nRowOffset, nMaxHandicapLevel - 1);
            return &ptGameBalDef->tHandicapLevelTable.ptHandicapLevels[nTableIndex];
        }
    }


    return NULL;
}


float GameGetHandicapDamageScalar()
{
    float flScalar = 1.0f;

    int32 nHandicapLevel = GameGetHandicapLevel();
    if (nHandicapLevel >= 0)
    {
        HandicapLevel * ptLevel = GameGetInfoForHandicapLevel(nHandicapLevel);
        if (ptLevel != NULL)
        {
            flScalar = ptLevel->tHandicapValues.flDamageScalar;
        }
    }

    return flScalar;
}




void GameGetPlayerHandicapValues(int32 nHandicapLevel, float & flMagicFindFromHandicap, float & flGoldFindFromHandicap, float & flXPBonusFromHandicap)
{




    flMagicFindFromHandicap = 0.0f;
    flGoldFindFromHandicap = 0.0f;
    flXPBonusFromHandicap = 0.0f;

    if (nHandicapLevel >= 0)
    {
        HandicapLevel * ptLevel = GameGetInfoForHandicapLevel(nHandicapLevel);
        if (ptLevel != NULL)
        {
            flMagicFindFromHandicap = ptLevel->tHandicapValues.flMagicFind;
            flGoldFindFromHandicap = ptLevel->tHandicapValues.flGoldFind;
            flXPBonusFromHandicap = ptLevel->tHandicapValues.flXPBonus;
        }
    }
}



int32 GameGetMonsterLevel()
{

    if (GameIsWeeklyChallenge())
    {
        return 70;
    }

    int32 nLevel = GameGetActiveGameCommonData().tGameSyncedData.nMonsterLevel;


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return Math::Max(nLevel, 60);
    }



    return Math::Max(nLevel, 1);
}



// 2.7.7: GameGetInitialMonsterLevel was removed with GameSyncedData::nInitialMonsterLevel.










BOOL GameUsesMonsterDensityModifiers()
{
    const int32 MIN_MONSTER_LEVEL = 24;

    if (GameGetMonsterLevel() < MIN_MONSTER_LEVEL)
        return FALSE;
    return TRUE;
}









BOOL GameIsStartUp()
{
    const GameParams * ptParams;






    if (GameActiveGameCommonDataIsClient())
    {
        ptParams = &CGameGet().tGameStartParams;
    }
    else
    {

        ptParams = &SGameManager::GameGet().tStartParams;
    }



    return ptParams->eGameType == GT_STARTUP;
}









BOOL GameIsServerOnly()
{

    GameParts nGameParts = GameGetParts();

    return nGameParts == GAME_PARTS_BATTLENET_NETWORK || nGameParts == GAME_PARTS_HIDDEN_SERVER;
}












BOOL ServerIsLocal()
{




    switch (GameGetParts())
    {
    case GAME_PARTS_CLIENT_SERVER:
    case GAME_PARTS_CLIENT_SERVER_NETWORK:
    case GAME_PARTS_BATTLENET_NETWORK:
    case GAME_PARTS_HIDDEN_SERVER:
        return TRUE;
    }

    return FALSE;
}




BOOL GameIsNetworked()
{

    GameParts nGameParts = GameGetParts();

    return nGameParts == GAME_PARTS_CLIENT_NETWORK || nGameParts == GAME_PARTS_CLIENT_SERVER_NETWORK || nGameParts == GAME_PARTS_BATTLENET_NETWORK;
}













BOOL GameIsBattleNetServer()
{



    return GameGetParts() == GAME_PARTS_BATTLENET_NETWORK;
}










uint32 GameClockGetTime()
{

    return SigmaGetGameClockTicks();
}




void GameClockInitialize(GameClock & tGameClock)
{

    tGameClock.dwLastGameClockTime = GameClockGetTime();

    tGameClock.dwClockTime = tGameClock.dwUsedClockTime = tGameClock.dwUsedGameTime = tGameClock.dwLastTicksPerSec = 0;
}






void GameClockAdvanceStart(GameClock & tGameClock, const uint32 dwGameClockTicks, uint32 & dwClockTicksToUse, uint32 & dwGameTicksToUse, const uint32 dwTicksPerSecond)
{




    uint32 dwUnscaledClockTicksToUse;


    int32 nTicksSinceLastUpdate = dwGameClockTicks - tGameClock.dwLastGameClockTime;
    nTicksSinceLastUpdate = Math::Pin(nTicksSinceLastUpdate, 0, 10000);

    tGameClock.dwLastGameClockTime = dwGameClockTicks;

    tGameClock.dwClockTime += nTicksSinceLastUpdate;

    if (tGameClock.dwLastTicksPerSec != dwTicksPerSecond)
    {

        dwGameTicksToUse = dwTicksPerSecond / TICKS_PER_SECOND;
        dwClockTicksToUse = dwTicksPerSecond * GAME_CLOCK_TICKS_PER_SECOND / TICKS_PER_SECOND / TICKS_PER_SECOND;
    }
    else
    {

        uint32 dwAdjustedClockTime = tGameClock.dwClockTime - tGameClock.dwUsedClockTime;

        dwClockTicksToUse = dwAdjustedClockTime * dwTicksPerSecond / TICKS_PER_SECOND;


        if (ReplayIsPlayingServerCPUTest())
        {
            const int32 nMinClockTimeToUse = GAME_CLOCK_TICKS_PER_SECOND;
            if (dwClockTicksToUse < nMinClockTimeToUse)
            {

                dwClockTicksToUse = nMinClockTimeToUse;
            }
        }



        if (dwClockTicksToUse > 0)
        {

            dwUnscaledClockTicksToUse = tGameClock.dwUsedClockTime + dwClockTicksToUse * TICKS_PER_SECOND / dwTicksPerSecond;
            dwGameTicksToUse = (dwUnscaledClockTicksToUse - tGameClock.dwUsedGameTime) * dwTicksPerSecond / GAME_CLOCK_TICKS_PER_SECOND;
        }
        else
        {
            dwGameTicksToUse = 0;
        }
    }
}



void GameClockAdvanceComplete(GameClock & tGameClock, const BOOL bResetGameClockTime, const uint32 dwUsedClockTicks, const uint32 dwUsedGameTicks, const uint32 dwTicksPerSecond)
{





    if (ReplayIsPlayingServerCPUTest())
    {
        tGameClock.dwLastTicksPerSec = dwTicksPerSecond;
        tGameClock.dwUsedClockTime = tGameClock.dwClockTime;
        tGameClock.dwUsedGameTime = tGameClock.dwClockTime;
        return;
    }

    if (bResetGameClockTime || tGameClock.dwLastTicksPerSec != dwTicksPerSecond)
    {
        tGameClock.dwLastTicksPerSec = dwTicksPerSecond;
        tGameClock.dwUsedClockTime = tGameClock.dwClockTime;
        tGameClock.dwUsedGameTime = tGameClock.dwClockTime;
        return;
    }

    if (dwTicksPerSecond != 0)
    {
        uint32 dwUnscaledUsedClockTicks = dwUsedClockTicks * TICKS_PER_SECOND / dwTicksPerSecond;
        tGameClock.dwUsedClockTime += dwUnscaledUsedClockTicks;




        uint32 dwActualUnscaledUsedGameTicks = (tGameClock.dwUsedClockTime - tGameClock.dwUsedGameTime) * dwTicksPerSecond;
        uint32 dwUnscaledUsedGameTicks = dwActualUnscaledUsedGameTicks / GAME_CLOCK_TICKS_PER_SECOND * GAME_CLOCK_TICKS_PER_SECOND / dwTicksPerSecond;
        tGameClock.dwUsedGameTime += dwUnscaledUsedGameTicks;
    }
}






BOOL GameClockDebugLag(uint32 dwGameClockTicks, GameClock & tGameClock)
{


    if (tGameClock.bDebugLagEnable)
    {




        if (dwGameClockTicks >= tGameClock.dwDebugLagStartGameClockTime)
        {




            if (dwGameClockTicks < tGameClock.dwDebugLagEndGameClockTime)
            {
                return TRUE;
            }




            tGameClock.dwLastGameClockTime = tGameClock.dwDebugLagEndGameClockTime;

            tGameClock.bDebugLagEnable = FALSE;
        }
    }
    return FALSE;
}
static void sUpdateAppClock()
{







    uint32 dwGameClockTime = GameClockGetTime();



    uint32 dwClockTicksToUse;
    uint32 dwGameTicksToUse;
    GameClockAdvanceStart(sgtAppGlobals.tAppClock, dwGameClockTime, dwClockTicksToUse, dwGameTicksToUse, TICKS_PER_SECOND);
    GameClockAdvanceComplete(sgtAppGlobals.tAppClock, FALSE, dwClockTicksToUse, dwGameTicksToUse, TICKS_PER_SECOND);

    sgtAppGlobals.dwTicksSinceAppStart += dwGameTicksToUse;
}




uint32 AppGetTicks()
{

    return sgtAppGlobals.dwTicksSinceAppStart;
}

































static Blizzard::Time::Millisecond s_tLastAppIdle = 0;

static void sLimitFrameRate()
{
    uint32 dwMinFrameMs = sgtAppGlobals.dwMinFrameMs;














    if (dwMinFrameMs == 0) {
        return;
    }




    Blizzard::Time::Millisecond tCurrentTime = Blizzard::Time::Milliseconds();
    Blizzard::Time::Millisecond tElapsed = tCurrentTime - s_tLastAppIdle;
    if (tElapsed < dwMinFrameMs) {

        Blizzard::Process::Sleep(dwMinFrameMs - tElapsed);
    }
    s_tLastAppIdle = Blizzard::Time::Milliseconds();
}










































DebugGameState s_tStateAtFrameStart;
DebugGameState * s_ptStateAtFrameStart = &s_tStateAtFrameStart;









char szCustomDesription[8192];

char * GameGetCustomCrashDescription()
{
    char szField[48] = "";

    szCustomDesription[0] = '\0';

    // Invented names: the original stringised each DebugGameState member into szField and
    // appended it; this is the shortest way to write that.
#define CRASH_DESC_FIELD(fmt, member) \
    Blizzard::String::Format(szField, sizeof(szField), "%s: " fmt "\n", #member, s_ptStateAtFrameStart->member); \
    strcat_safe(szCustomDesription, sizeof(szCustomDesription), szField)

    char szHeapStats[1024];
    MemoryUtilsGenerateHeapStatsString(szHeapStats, sizeof(szHeapStats));
    strcat_safe(szCustomDesription, sizeof(szCustomDesription), szHeapStats);


    strcat_safe(szCustomDesription, sizeof(szCustomDesription), "State at frame start:\n------\n");









    CRASH_DESC_FIELD("%d", bPlayerValid);

    if (s_ptStateAtFrameStart->bPlayerValid) {

        CRASH_DESC_FIELD("%d", snoPlayerActor);

        CRASH_DESC_FIELD("%d", eAct);
        CRASH_DESC_FIELD("%d", snoWorld);
        CRASH_DESC_FIELD("%d", dwSeed);

        if (s_ptStateAtFrameStart->snoScene != (SNO)0xcdcdcdcd) {

            CRASH_DESC_FIELD("%d", snoScene);
        }

        CRASH_DESC_FIELD("%.2f", tWorldPos.x);
        CRASH_DESC_FIELD("%.2f", tWorldPos.y);
        CRASH_DESC_FIELD("%.2f", tWorldPos.z);
    }

    if (s_ptStateAtFrameStart->nQuestStepUID != (int32)0xcdcdcdcd &&
        s_ptStateAtFrameStart->snoQuest != 0 &&
        !s_ptStateAtFrameStart->bGameIsStartUp) {

        CRASH_DESC_FIELD("%d", snoQuest);
        CRASH_DESC_FIELD("%d", nQuestStepUID);
    }

    CRASH_DESC_FIELD("%d", bGameIsStartUp);
    CRASH_DESC_FIELD("%d", nPlayers);
    CRASH_DESC_FIELD("%d", nLocalPlayers);

    const char * pszCrashDescription = GfxGPUCrashDescription();
    strcat_safe(szCustomDesription, sizeof(szCustomDesription), pszCrashDescription);
#undef CRASH_DESC_FIELD






    return szCustomDesription;
}






static uint64 sSmoothGameClockTime(const uint64 dwGameClockTime)
{







    if (sgtAppGlobals.nLastGameClockTime == 0 || sgtAppGlobals.nLastGameClockDelta == 0) {


        sgtAppGlobals.nLastGameClockTime = dwGameClockTime;
        sgtAppGlobals.nLastGameClockDelta = 50;
        return sgtAppGlobals.nLastGameClockTime;
    }


    if (dwGameClockTime <= sgtAppGlobals.nLastGameClockTime) {
        return sgtAppGlobals.nLastGameClockTime;
    }



    const uint64 dwGameClockDelta = dwGameClockTime - sgtAppGlobals.nLastGameClockTime;


    if (dwGameClockDelta > 150) {
        sgtAppGlobals.nLastGameClockTime = dwGameClockTime;
        return sgtAppGlobals.nLastGameClockTime;
    }




    sgtAppGlobals.nLastGameClockDelta = (uint64)((float)dwGameClockDelta * 0.2f + (float)sgtAppGlobals.nLastGameClockDelta * 0.8f);
    sgtAppGlobals.nLastGameClockTime += sgtAppGlobals.nLastGameClockDelta;
    return sgtAppGlobals.nLastGameClockTime;
}
// Line 4417: the call is at column 12 in the line table (two tabs + "fDraw &= "), and the original tests
// bit 0 of the result (tbnz w19,#0), so the statement is `fDraw &= ClientIdle(...)` on the TRUE-initialised
// fDraw. ClientIdle returns BOOL in its definition DWARF (CGame.cpp:5757).
void AppIdle()
{






















    CPlayerFillDebugGameState(s_tStateAtFrameStart);












    sgtAppGlobals.dwFrameCounter++;
    s_nGameFrameCounter = sgtAppGlobals.dwFrameCounter;

    sgtAppGlobals.mainTimer.Stop();
    sgtAppGlobals.mainTimerHistory.Append(sgtAppGlobals.mainTimer.Get());
    sgtAppGlobals.mainTimer.Reset();
    sgtAppGlobals.mainTimer.Start();

    UpdateDebugChannel();

    sLimitFrameRate();

    sUpdateAppClock();


    Console::GamerProfile::Process();
    Console::Online::Idle();
    Console::IO::Update();
    SNOFilesUpdate();






    if (sgtAppGlobals.bInAppIdle ||
        !GameLoopRunning() ||
        SigmaCurrentlyHandlingError()) {


        if (InputIsCursorInWindow() || InputAppHasCapture()) {

            InputCursorSet(CURSOR_EDITSELECT);
        }
        return;
    }









    sgtAppGlobals.bInAppIdle = TRUE;










    GameCheckForQueuedGameStart();

    uint32 dwGameClockTime = GameClockGetTime();


    dwGameClockTime = (uint32)sSmoothGameClockTime(dwGameClockTime);

    BOOL fDraw = TRUE;
















































































































    if (sgtAppGlobals.PlayingCGameGet() != NULL) {

        if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_TRACE_HESITATION_BIT)) {

            TraceHesitationStart();
        }






        fDraw &= ClientIdle(dwGameClockTime);


        if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_TRACE_HESITATION_BIT)) {

            TraceHesitationEnd();
        }
    }













    sgtAppGlobals.bInAppIdle = FALSE;







    if (fDraw || GameIsServerOnly()) {

        SigmaProfNewFrame(NULL);


        JobManager::Get()->NewFrame();
    }



    if (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_TRACE_PERF)) {

        sTracePerformance();
    }



}



// line 4467: static uint32 sServerIdleProc(void * ptParam)  [0x7100432d60]
static uint32 sServerIdleProc(void * ptParam)
{


    ThreadGlobals::InitializeForThread();
    SigmaProfInitializeForThread();


    LuaDeadlockGameThreadData tLuaDeadlockGameThreadData;
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData = &tLuaDeadlockGameThreadData;

    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_SERVERIDLE;

    while (sgbRunServerIdleThread)
    {
        Blizzard::Time::Millisecond dwSleepTime = 200;
        if (SGameManager::GetGameCount() == 0)
        {
            Blizzard::Process::Sleep(dwSleepTime);
            continue;
        }

        {
            Blizzard::Time::Millisecond tCurrentTime;
            const Blizzard::Time::Millisecond tExpectedMS = 16;
            Blizzard::Time::Millisecond tElapsed;
            Blizzard::Time::Millisecond tLastAppIdle;
            Blizzard::Lock::ScopedRMutex tLock(sgtServerIdleMutex);

            tLastAppIdle = Blizzard::Time::Milliseconds();
            if (SGameManager::GetGameCount() > 0)
            {
                ScopedEnterAndLeaveServerCode enterAndLeaveServer;

                uint32 dwGameClockTime = GameClockGetTime();
                AppServerIdle(dwGameClockTime);
                SigmaProfNewFrame(NULL);
            }


            tCurrentTime = Blizzard::Time::Milliseconds();
            tElapsed = tCurrentTime - tLastAppIdle;



            if (tElapsed < tExpectedMS)
                dwSleepTime = tExpectedMS - tElapsed;
            else

                dwSleepTime = 0;
        }

        if (dwSleepTime > 0)
            Blizzard::Process::Sleep(dwSleepTime);
    }

    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData = NULL;

    SigmaProfDisposeForThread();
    ThreadGlobals::DisposeForThread();

    return 0;
}

static void sTracePerformance()
{
    uint32 uTime = GetMachineTicks();
    uint32 uElapsed = uTime - sgtAppGlobals.uLastPerfTrace;

    if (uElapsed < sgtAppGlobals.uPerfTraceInterval) {
        return;
    }

    if (CGameIsValid()) {

        Player * ptPlayer = LocalPlayerGet();

        if (ptPlayer != NULL) {

            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

            if (ptACDPlayer != NULL) {

                ActorCommonData & tACDPlayer = *ptACDPlayer;
                CRefString szScene;

                NavMeshHandle tNavMeshHandle;
                ACDGetNavMeshHandle(tACDPlayer, tNavMeshHandle);
                SceneCommonData * ptSCD = SceneCommonDataTryToGet(tNavMeshHandle.idSScene);

                if (ptSCD != NULL) {

                    szScene = SNOToString(SCENE_SNO, ptSCD->snoScene, FALSE);
                }

                CRefString szWorldPosition;
                if (tACDPlayer.tLocation.eActorLocation == AL_WORLD) {

                    WorldLocation & loc = tACDPlayer.GetWorldLocation();
                    SNO snoWorld = WorldCommonGetWorldSNO(loc.idSWorld);

                    szWorldPosition.Format("(%f,%f,%f) %s",
                        loc.transform.wp.x,
                        loc.transform.wp.y,
                        loc.transform.wp.z,
                        SNOToString(WORLD_SNO, snoWorld, FALSE).str());
                }









            }
        }
    }










    sgtAppGlobals.uLastPerfTrace = uTime;
}
void sCGameInit(CGame * ptCGame)
{

    GameClockInitialize(ptCGame->tRealClock);
    ptCGame->dwRealTicks = 0;

    ptCGame->tQueuedGameStartParams.bValid = FALSE;
    ptCGame->tQueuedGameStartParams.snoInitialWorld = -1;
    ptCGame->tQueuedGameStartParams.nGameParts = GAME_PARTS_NONE;


    ptCGame->ptCGameGlobals = NULL;
    ptCGame->tGCD.ptMarkerGlobals = NULL; ptCGame->tGCD.ptInactiveMarkerGlobals = NULL; ptCGame->tGCD.ptPlayerGlobals = NULL;


    ptCGame->tGCD.ptACDGlobals = NULL;
    ptCGame->ptRActorGlobals = NULL;
    ptCGame->ptCPlayerGlobals = NULL;
    ptCGame->ptCActorMovementGlobals = NULL;


    ptCGame->tObjectUpdateGlobalLock.Create();



    ptCGame->tClientUpdateLock.Create();
    ptCGame->tClientUpdateLock.Set();


    GameSetActiveGameCommonData(&ptCGame->tGCD, TRUE);


    ptCGame->ptNetStats = (NetStat *)SigmaMemoryNew(sizeof(NetStat), 4, NULL, TRUE);

    NetStatInit(*ptCGame->ptNetStats);
}




void sCGameDispose(CGame * ptCGame)
{





    if (ptCGame->ptNetStats) {

        NetStatDispose(*ptCGame->ptNetStats);
        SigmaMemoryFree(ptCGame->ptNetStats, NULL); ptCGame->ptNetStats = NULL;
    }




    ptCGame->tClientUpdateLock.Destroy();



    ptCGame->tObjectUpdateGlobalLock.Destroy();
}



void GameGlobalsInit()
{




    sgtAppGlobals.ptCGamePlaying = (CGame *)SigmaMemoryNew(sizeof(CGame), 4, NULL, TRUE);

    sCGameInit(sgtAppGlobals.ptCGamePlaying);

    sgtAppGlobals.ptCGameActive = sgtAppGlobals.ptCGamePlaying;








    SpellEffect_InitializeForApp();
}





void GameGlobalsDispose()
{

    sCGameDispose(sgtAppGlobals.ptCGamePlaying);
    SigmaMemoryFree(sgtAppGlobals.ptCGamePlaying, NULL); sgtAppGlobals.ptCGamePlaying = NULL;
}









BOOL GameModeIsAnEditMode(const GameMode eMode)
{

    return eMode == GAMEMODE_EDITUNITLAYOUT || eMode == GAMEMODE_EDITSCENE;
}



BOOL GameModeIsAPlayingMode(const GameMode eMode)
{

    return eMode == GAMEMODE_PLAYING || eMode == GAMEMODE_MENU;
}

































BOOL AppExitIsEnabled()  // CROSS-PART: owned by part 5
{
    return sgtAppGlobals.fAppExitEnabled;
}










































int32 GameRandRangeInt(const int32 nMin, const int32 nMax)
{






    if (GameActiveGameCommonDataIsClient()) {

        return ClientRandRangeInt(nMin, nMax);
    }




    return ServerRandRangeInt(nMin, nMax);
}







float GameRandRangeFloat(const float flMin, const float flMax)
{






    if (GameActiveGameCommonDataIsClient()) {

        return ClientRandRangeFloat(flMin, flMax);
    }




    return ServerRandRangeFloat(flMin, flMax);
}

































































































































































































































































BOOL GameIsParamFlagSet(GameParamFlags eFlag)
{
    const GameCommonData & tGCD = GameGetActiveGameCommonData();

    return (tGCD.tTemplate.uFlags >> eFlag) & 1;
}



float GameGetMaxExpDist()
{
    const GameCommonData & tGCD = GameGetActiveGameCommonData();

    return tGCD.tTemplate.flMaxExperienceDist;
}



float GameGetMinimapZoomDist()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return 1.0f / (tGCD.tTemplate.flMinimapFeetPerPixel * 0.6f);
}












uint32 GameGetCurrentSeason()
{

    return OnlineService::GetSeasonNum();
}



BOOL GameIsSeasonal()
{








    return (GameGetActiveGameCommonData().tGameSyncedData.dwGameSyncedFlags >> GAME_SYNCED_FLAGS_SEASONAL) & 1;
}



BOOL GameIsUtility()
{


    return (SGameManager::GameGet().tStartParams.dwCreationFlags >> GAMECREATEPARAM_UTILITY) & 1;
}
























uint64 GameGetDailyPlatinumLimit()
{

    return OnlineService::IsFreeToPlay() ? s_varPlatinumLimit.Get() : 0;
}




uint32 GameGetDailyPlatinumResetTime()
{


    return s_varPlatinumResetTime.Get();
}



AnniversaryEventStatus GameGetAnniversaryEventStatus()
{

    struct AnniversaryTimestamp
    {
        AnniversaryEventStatus eStatus;
        int32 nDayOfYear;

        struct Compare
        {
            bool operator()(const AnniversaryTimestamp & a, const AnniversaryTimestamp & b) const
            {
                return a.nDayOfYear < b.nDayOfYear;
            }
        };
    };






    Blizzard::Time::Timestamp tCurTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::TimeRec tTimeRecord;
    Blizzard::Time::BreakTime(tCurTime, tTimeRecord);

    ScopedGlobalDefinitionLockR pdefGlobals;
    int32 nPrecursorStart = Blizzard::Time::GetDayOfTheYear(pdefGlobals->nAnniversaryPrecursorStartDay, pdefGlobals->nAnniversaryPrecursorStartMonth, tTimeRecord.year);
    int32 nEventStart = Blizzard::Time::GetDayOfTheYear(pdefGlobals->nAnniversaryEventStartDay, pdefGlobals->nAnniversaryEventStartMonth, tTimeRecord.year);
    int32 nEventEnd = Blizzard::Time::GetDayOfTheYear(pdefGlobals->nAnniversaryEventLastDay, pdefGlobals->nAnniversaryEventLastMonth, tTimeRecord.year);

    AnniversaryTimestamp arDays[] = { { ANNIVERSARY_PRECURSOR_START, nPrecursorStart },
                                      { ANNIVERSARY_EVENT_START, nEventStart },
                                      { CURRENT_DAY, tTimeRecord.dayOfYear },
                                      { ANNIVERSARY_EVENT_END, nEventEnd } };


    blz::stable_sort(arDays, arDays + 4, AnniversaryTimestamp::Compare());
    AnniversaryTimestamp * iter = blz::find_if(arDays, arDays + 4, [](const AnniversaryTimestamp & tDay) { return tDay.eStatus == CURRENT_DAY; });



    if (iter == arDays)
    {

        iter = arDays + 4;
    }

    return (iter - 1)->eStatus;
}
















































































































static void InitDebugChannel()
{



















    s_pDebugChannelControl = new XDebugChannelControl(Blizzard::String::QuickFormat<1024>("%s (%s)", "Diablo III NSwitch", BuildVersionCodeData()));
    s_pDebugChannelControl->StartListening(9391);
    XDebugChannelConsoleInit();

    const CmdLineParams * ptCmdLineParams = CmdLineGetParams();
    if (ptCmdLineParams->tBitArray.Get(CMDLINE_ENABLE_MEMORY_DEBUG_CHANNEL))
    {
        XDebugChannelMemoryInit();
    }

    XDebugChannelPropertyInit();
}



static void DisposeDebugChannel()
{
    XDebugChannelConsoleDispose();
    delete s_pDebugChannelControl; s_pDebugChannelControl = NULL;
}




static void UpdateDebugChannel()
{
    s_pDebugChannelControl->Update();
    XDebugChannelMemoryUpdate();
}




// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
void AppGlobals::SetTicksPerSec(uint32 dwTicksPerSec)
{

    m_dwTicksPerSec = AppGetSafeTicksPerSec(dwTicksPerSec);
}


uint32 AppGlobals::GetTicksPerSec() const
{
    return m_dwTicksPerSec;
}




uint32 AppGetSafeTicksPerSec(uint32 dwDesiredTicksPerSec)
{
    // Only copy is inlined into the linker-stripped AppGlobals::SetTicksPerSec, so there are no
    // bytes to verify. DWARF locals: dwLastSafeTicksPerSec, it, dwSafeTicksPerSec (loop at 5447).
    uint32 dwLastSafeTicksPerSec = GAME_CLOCK_TICKS_PER_SECOND;





    for (blz::vector<uint32>::iterator it = s_arSafeTicksPerSec.begin(); it != s_arSafeTicksPerSec.end(); ++it)
    {
        uint32 dwSafeTicksPerSec = *it;
        if (dwSafeTicksPerSec > dwDesiredTicksPerSec)
            break;
        dwLastSafeTicksPerSec = dwSafeTicksPerSec;
    }
    return dwLastSafeTicksPerSec;
}
















// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
BOOL AppBlockServerThread(Blizzard::Time::Millisecond waitMs)
{


    if (waitMs == 0)
    {
        sgtAppGlobals.tGlobalServerSemaphore.WaitForever();
        return TRUE;
    }

    return sgtAppGlobals.tGlobalServerSemaphore.Wait(waitMs) != NULL;
}

BOOL AppServerThreadIsRunningGame()
{
    BOOL bRunning = FALSE;
    Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tGlobalDisposeServerMutex);
    if (SGameManager::GetGameCount() != 0)
    {




        SGameIterator tIt;
        if (SGameManager::GameIterate(tIt))
        {

            SGame & tSGame = *tIt.ptElement;
            bRunning = !((tSGame.ptSGameGlobals->dwFlags >> 1) & 1);
        }
    }

    return bRunning;
}


// Linker-stripped in the shipped binary (low_pc 0 in the CU dump, so absent from the reference
// list and with no bytes to verify); written from its DWARF locals and inlined calls.
void AppUnblockServerThread()
{

    sgtAppGlobals.tGlobalServerSemaphore.Set();
}
