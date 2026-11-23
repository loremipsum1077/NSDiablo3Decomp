// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SNO/SNOFiles.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
// ../Game.h is not adopted yet (it is large and pulls in most of the game). Nothing in this
// header needs a complete type from it; PackBuilderFileEntry is forward-declared below.
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XGrowableArray.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/Sigma.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

namespace FileIO { class File; }
struct FileReference;
struct PackBuilderFileEntry;

enum {
    SNOFilesAsyncPriority_Low = 0,
    SNOFilesAsyncPriority_Medium = 1,
    SNOFilesAsyncPriority_High = 2,
    SNOFilesAsyncPriority_Count = 3,
};

enum {
    SNOFILETYPE_INVALID = -1,
    SNOFILETYPE_LOCAL = 0,
    SNOFILETYPE_SNOPACK = 1,
    NUM_SNOFILETYPES = 2,
};

typedef int32 SNOFileType;

enum {
    SNOPACKTYPE_INVALID = -1,
    SNOPACKTYPE_COREDATA = 0,
    SNOPACKTYPE_CLIENTDATA = 1,
    SNOPACKTYPE_TEXTURE = 2,
    SNOPACKTYPE_SOUND = 3,
    SNOPACKTYPE_SERVERDATA = 4,
    SNOPACKTYPE_CONSOLE_SPECIFIC_BEGIN = 5,
    SNOPACKTYPE_CONSOLE_SPECIFIC_BEGIN_NON_CACHE = 5,
    SNOPACKTYPE_CONSOLE_COMMON = 5,
    SNOPACKTYPE_CONSOLE_CORE_COMMON = 6,
    SNOPACKTYPE_CONSOLE_CLIENT_COMMON = 7,
    SNOPACKTYPE_CONSOLE_SERVER_COMMON = 8,
    SNOPACKTYPE_CONSOLE_ACT1 = 9,
    SNOPACKTYPE_CONSOLE_ACT2 = 10,
    SNOPACKTYPE_CONSOLE_ACT3 = 11,
    SNOPACKTYPE_CONSOLE_ACT4 = 12,
    SNOPACKTYPE_CONSOLE_ACT5 = 13,
    SNOPACKTYPE_CONSOLE_PVP = 14,
    SNOPACKTYPE_CONSOLE_SPECIFIC_END_NON_CACHE = 14,
    SNOPACKTYPE_CONSOLE_SPECIFIC_CACHE_BEGIN = 15,
    SNOPACKTYPE_CONSOLE_CACHE_COMMON = 15,
    SNOPACKTYPE_CONSOLE_CACHE_ACT1 = 16,
    SNOPACKTYPE_CONSOLE_CACHE_ACT2 = 17,
    SNOPACKTYPE_CONSOLE_CACHE_ACT3 = 18,
    SNOPACKTYPE_CONSOLE_CACHE_ACT4 = 19,
    SNOPACKTYPE_CONSOLE_CACHE_ACT5 = 20,
    SNOPACKTYPE_CONSOLE_CACHE_PVP = 21,
    SNOPACKTYPE_CONSOLE_SPECIFIC_CACHE_END = 21,
    SNOPACKTYPE_CONSOLE_SPECIFIC_END = 21,
    SNOPACKTYPE_CONSOLE_HOTFIXES = 22,
    NUM_SNOPACKTYPES = 23,
};

typedef int32 SNOPackType;

enum SNOPackChunk {
    SNOPACKCHUNK_INITIAL_PAYLOAD = 0,
    SNOPACKCHUNK_ACT1 = 1,
    SNOPACKCHUNK_ACT2 = 2,
    SNOPACKCHUNK_ACT3 = 3,
    SNOPACKCHUNK_ACT4 = 4,
    SNOPACKCHUNK_ACT5 = 5,
    SNOPACKCHUNK_COUNT = 6,
};

enum SNOPackChunkState {
    SNOPACKCHUNKSTATE_INVALID = -1,
    SNOPACKCHUNKSTATE_NOT_READY = 0,
    SNOPACKCHUNKSTATE_READY = 1,
    SNOPACKCHUNKSTATE_LOADED = 2,
    SNOPACKCHUNKSTATE_COUNT = 3,
};

enum SNODataType {
    SNODATATYPE_TITLE = 0,
    SNODATATYPE_USERDATA = 1,
    NUM_SNODATATYPES = 2,
};

typedef int32 XLocale;

enum {
    SNO_NONE = -1,
    INVALID_SNO = -2,
};

typedef int32 SNO;

enum {
    SNOGROUP_CODE = -2,
    SNOGROUP_NONE = -1,
    ACTOR_SNO = 1,
    ADVENTURE_SNO = 2,
    AIBEHAVIOR_SNO = 3,
    AISTATE_SNO = 4,
    AMBIENTSOUND_SNO = 5,
    ANIMATION_SNO = 6,
    ANIMATION2D_SNO = 7,
    ANIMSET_SNO = 8,
    APPEARANCE_SNO = 9,
    HERO_SNO = 10,
    CLOTH_SNO = 11,
    CONVERSATION_SNO = 12,
    CONVERSATIONLIST_SNO = 13,
    EFFECTGROUP_SNO = 14,
    ENCOUNTER_SNO = 15,
    EXPLOSION_SNO = 17,
    FLAGSET_SNO = 18,
    FONT_SNO = 19,
    GAMEBALANCE_SNO = 20,
    GLOBAL_SNO = 21,
    LEVELAREA_SNO = 22,
    LIGHT_SNO = 23,
    MARKERSET_SNO = 24,
    MONSTER_SNO = 25,
    OBSERVER_SNO = 26,
    PARTICLE_SNO = 27,
    PHYSICS_SNO = 28,
    POWER_SNO = 29,
    QUEST_SNO = 31,
    ROPE_SNO = 32,
    SCENE_SNO = 33,
    SCENEGROUP_SNO = 34,
    SCRIPT_SNO = 35,
    SHADERMAP_SNO = 36,
    SHADER_SNO = 37,
    SHAKE_SNO = 38,
    SKILLKIT_SNO = 39,
    SOUND_SNO = 40,
    SOUNDBANK_SNO = 41,
    STRINGLIST_SNO = 42,
    SURFACE_SNO = 43,
    TEXTURE_SNO = 44,
    TRAIL_SNO = 45,
    UI_SNO = 46,
    WEATHER_SNO = 47,
    WORLD_SNO = 48,
    RECIPE_SNO = 49,
    CONDITION_SNO = 51,
    TREASURECLASS_SNO = 52,
    ACCOUNT_SNO = 53,
    CONDUCTOR_SNO = 54,
    TIMEDEVENT_SNO = 55,
    ACT_SNO = 56,
    MATERIAL_SNO = 57,
    QUESTRANGE_SNO = 58,
    LORE_SNO = 59,
    REVERB_SNO = 60,
    PHYSMESH_SNO = 61,
    MUSIC_SNO = 62,
    TUTORIAL_SNO = 63,
    BOSS_ENCOUNTER_SNO = 64,
    CONTROL_SCHEME_SNO = 65,
    ACCOLADE_SNO = 66,
    ANIMTREE_SNO = 67,
    VIBRATION_SNO = 68,
    DUNGEONFINDER_SNO = 69,
    MAX_SNO_GROUPS = 70,
};

typedef int32 SNOGroup;

// SNOName.h and SNOTOC.h use SNO and SNOGroup and this header uses SNOName and SNOTOC, so they
// are included here, after the typedefs, rather than at the top.
#include "SNOName.h"
#include "SNOTOC.h"

// The asset-version sentinel (DWARF: SNOFiles.h:216, const_value 4294967295).
static const uint32 ASSET_VERSION_NONE = 0xffffffff;  // line 216

struct SNOEntry {  // size 0x10
    static const uint32 INVALID_GUID = 16777215;
    void SetName(SNOTOC::TOCType eType, const CHAR * szFileNameNoExtension);
    SNOName ToSNOName() const;
    SNOGroup snoGroup : 8;  // 0x00
    uint32 uGuid : 24;  // 0x00
    void * pNameOrHash;  // 0x08
    LPCSTR GetName() const;
};
static_assert(sizeof(SNOEntry) == 0x10, "SNOEntry");

enum SNOElementFlags {
    SNOENTRY_FLAG_ASYNCHRONOUS = 1,
    SNOENTRY_FLAG_READING = 2,
    SNOENTRY_FLAG_BROKEN_SNO = 4,
    SNOENTRY_FLAG_BROKEN_XML = 8,
    SNOENTRY_FLAG_LOADED = 16,
    SNOENTRY_FLAG_LOADING = 32,
    SNOENTRY_FLAG_NOT_IN_CPK = 64,
    SNOENTRY_FLAG_CHILD_FILE = 128,
    SNOENTRY_FLAG_NO_ASSET_SERVER = 256,
};

struct SNOElement {  // size 0x18
    SNOEntry tEntry;  // 0x00
    volatile int32 dwFlags;  // 0x10
    void Dump();
};
static_assert(sizeof(SNOElement) == 0x18, "SNOElement");

struct SNOElementChild : public SNOElement {  // size 0x38
    char szChildFile[16];  // 0x18
    volatile int32 nLockCount;  // 0x28
    void * pDataBuffer;  // 0x30
};
static_assert(sizeof(SNOElementChild) == 0x38, "SNOElementChild");

struct SNOFileMetadata {  // size 0x4
    void Init();
    uint8 eAct;  // 0x00
    uint8 bUnused[3];  // 0x01
};
static_assert(sizeof(SNOFileMetadata) == 0x4, "SNOFileMetadata");

class SNOFileInfo {  // size 0x28
public:
    SNOFileInfo();
    SNOName snoname;  // 0x00
    uint32 dwVersion;  // 0x08
    uint32 dwAssetVersion;  // 0x0c
    SNOFileType eSNOFileType;  // 0x10
    XLocale eXLocale;  // 0x14
    LPCSTR pszPath;  // 0x18
    uint32 dwXMLHash;  // 0x20
    BOOL fReadOnly;  // 0x24
};
static_assert(sizeof(SNOFileInfo) == 0x28, "SNOFileInfo");

struct SNOFileAsyncStats {  // size 0x20
    FileIO::File * m_ptFile;  // 0x00
    FileReference * diskFile;  // 0x08
    void * buffer;  // 0x10
    uint32 offset;  // 0x18
    uint32 amount;  // 0x1c
};
static_assert(sizeof(SNOFileAsyncStats) == 0x20, "SNOFileAsyncStats");

typedef XPooledDataIDList SNOPooledList;

typedef XGrowableList<SNOName, SNOName> SNONameList;

typedef XGrowableList<int, int> SNOList;

typedef XGrowableArray<int, int> SNOArray;

enum SNOFileStatResults {
    SNOFILESTATRESULT_FILE_TOO_BIG_FOR_PREFETCH = -3,
    SNOFILESTATRESULT_FILE_NOT_FOUND = -2,
    SNOFILESTATRESULT_OUT_OF_MEMORY = -1,
    SNOFILESTATRESULT_SUCCESS = 0,
};

typedef blz::vector<PackBuilderFileEntry *, blz::allocator<PackBuilderFileEntry *> > PackBuilderFileArray;

enum CutsceneCPKType {
    CUTSCENE_CPK_TYPE_INVALID = -1,
    CUTSCENE_CPK_TYPE_FIRST_CHUNK = 0,
    CUTSCENE_CPK_TYPE_COMMON = 0,
    CUTSCENE_CPK_TYPE_ACT1 = 1,
    CUTSCENE_CPK_TYPE_ACT2 = 2,
    CUTSCENE_CPK_TYPE_ACT3 = 3,
    CUTSCENE_CPK_TYPE_ACT4 = 4,
    CUTSCENE_CPK_TYPE_ACT5 = 5,
    CUTSCENE_CPK_TYPE_LAST_CHUNK = 5,
    CUTSCENE_CPK_TYPE_ALL = 6,
};

enum {
    SNOPackInstallationSpeed_Suspended = 0,
    SNOPackInstallationSpeed_Default = 1,
    SNOPackInstallationSpeed_FullSpeed = 2,
    SNOPackInstallationSpeed_Count = 3,
};

typedef int32 SNOPackInstallationSpeed;

// Defined in SNOFiles.cpp:5270; the DWARF does not record where it was declared.
SNO SNOFileLookupGUID(const SNOGroup snoGroup, LPCSTR szPath);

// Defined in SNOFiles.cpp (lines 5620, 7271 and 7315); the DWARF does not record where they were
// declared.
BOOL SNOFileExists(const SNOGroup snoGroup, const SNO sno);
BOOL SNOFileExists(const SNOName & snoname);  // SNOFiles.cpp:5565
CRefString SNOToString(const SNOGroup snoGroup, const SNO snoHandle, const BOOL fUsedForXMLFields = FALSE);  // default: GameTesting.cpp:1546/1547 columns of .str() (44/52) leave no room for a third argument
CRefString SNOToString(const SNOName & snoname, const BOOL fUsedForXMLFields);

// Defined in SNOFiles.cpp; the DWARF does not record where it was declared.
void SNOFileRead(SNOElement * ptSNOElement, const uint32 dwOffset, const uint32 dwSize, void * lpBuffer);

// line 466. The body is not in the DWARF; reads the block a SerializeData describes.
inline void SNOFileRead(SNOElement * ptSNOElement, const SerializeData & ser, void * lpBuffer) {
    SNOFileRead(ptSNOElement, ser.dwOffset, ser.dwSize, lpBuffer);
}


// Functions defined in this file (bodies are not in the DWARF):
//   line   254  SNOName SNOEntry::ToSNOName() const  [inlined into 5 functions]
//   line   266  LPCSTR SNOEntry::GetName() const  [inlined into 3 functions]
//   line   466  void SNOFileRead(SNOElement * ptSNOElement, const SerializeData & ser, void * lpBuffer)  [inlined into 4 functions]

// Variables defined in this file:
//   line   210  static const int32 MAXIMUM_CHILD_FILE_NAME_LENGTH
//   line   216  static const uint32 ASSET_VERSION_NONE
//   line   222  static const uint32 DYNAMIC_GUID_START
//   line   223  static const uint32 DYNAMIC_GUID_END

// Declared here from their definitions in X/Common/Sno/SNOFiles.cpp (X/Common/Game.cpp calls them).
void SNOFilesInitialize(LPCSTR szDataPath, const SigmaPlatform eSigmaPlatform, const XLocale eXLocale);  // SNOFiles.cpp:1574
void SNOFilesDispose();  // SNOFiles.cpp:1912
void SNOFilesUpdate();  // SNOFiles.cpp
CRefString SNOFilesGetMPQPath();  // SNOFiles.cpp:2455
const char * SNOFilesGetMPQExtension();  // SNOFiles.cpp:9880
const char * SNOFilesGetConsoleDataPathOverride();  // SNOFiles.cpp:10543
void SNOFilesApplyAllHotfixInFolder(LPCSTR szPath, BOOL fValidateCRC);  // SNOFiles.cpp
uint32 SNOFileGetFileTableCount();  // SNOFiles.cpp:4806
int32 SNOFileEnumerateSNOsForGroup(SNOGroup snoGroup, PooledSNONameList * plistSNOFiles);  // SNOFiles.cpp

// SNOFiles.cpp:426,7353,7431.
extern LPCSTR cgszInvalidSnoName;
SNOGroup StringToSNOGroup(LPCSTR szStr);
CRefString SNOGroupToString(const SNOGroup snoGroup, const BOOL fUsedForXMLFields);

// SNOFiles.cpp:7515.
void * SNOFileLoadSerializedTracked(SNOElement * ptSNOElement, const SerializeData & ser, uint32 dwAlign, LPCSTR szFile, int32 nLine, LPCSTR szDebugString, XMemoryAllocatorInterface * ptMemoryInterface);
