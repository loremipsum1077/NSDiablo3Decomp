// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Definitions/Definitions.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
// ../Game.h is not adopted yet; nothing here needs a complete type from it.
#include "../SNO/SNOFiles.h"
#include "../SNO/SNOGroups.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XMemoryAllocatorInterface.h"
#include "../../Sigma/DataStructures/XTemplate.h"
#include "../../Sigma/DataStructures/XVirtualHeap.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Sigma.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/Unicode.h"
#include <stddef.h>
#include <stdint.h>

struct AIStateDefinition;
struct ActDefinition;
struct ActorDefinition;
struct AdventureDefinition;
struct AmbientSoundDefinition;
struct AnimSetDefinition;
struct AnimTreeDefinition;
struct AnimationDefinition;
struct AppearanceDefinition;
struct BossEncounterDefinition;
class CRefString;
struct ClothDefinition;
struct ConditionDefinition;
struct ConductorDefinition;
struct ControlSchemeDefinition;
struct ConversationDefinition;
struct ConversationListDefinition;
struct DungeonFinderDefinition;
struct EffectGroupDefinition;
struct EncounterDefinition;
struct FileReference;
struct FlagSetDefinition;
struct FontDefinition;
struct GameBalanceDefinition;
struct GlobalDefinition;
struct LevelAreaDefinition;
struct LightDefinition;
struct LoreDefinition;
struct MarkerSetDefinition;
struct MaterialDefinition;
struct MonsterDefinition;
struct MusicDefinition;
struct ObserverDefinition;
struct ParticleDefinition;
struct PhysMeshDefinition;
struct PhysicsDefinition;
struct PowerDefinition;
struct QuestDefinition;
struct QuestRangeDefinition;
struct RecipeDefinition;
struct ReverbDefinition;
struct RopeDefinition;
struct SNOName;
class SceneDefinition;
struct SceneGroupDefinition;
struct ScriptDefinition;
struct ShaderDefinition;
struct ShaderMapDefinition;
struct ShakeDefinition;
struct SkillKitDefinition;
struct SoundBankDefinition;
struct SoundDefinition;
struct StringListDefinition;
struct SurfaceDefinition;
struct TextureDefinition;
struct TimedEventDefinition;
struct TrailDefinition;
struct TreasureClassDefinition;
struct TutorialDefinition;
struct UIDialogDefinition;
struct VibrationDefinition;
struct WeatherDefinition;
struct WorldDefinition;

enum {
    GROUPDEFINITION_CANNOT_LRU_PURGE_BIT = 0,
    GROUPDEFINITION_THREAD_SAFE_BIT = 1,
    GROUPDEFINITION_ALLOWED_ON_SERVER_BIT = 2,
    GROUPDEFINITION_ALLOWED_ON_CLIENT_BIT = 3,
    GROUPDEFINITION_ALLOWED_ON_ASYNC_RENDER_BIT = 4,
    GROUPDEFINITION_CANT_SHARE_BIT = 5,
    GROUPDEFINITION_SERVER_CANT_LOAD_CONTINUOUS_BLOCK_BIT = 6,
    GROUPDEFINITION_ASYNC_FLUSH = 7,
    GROUPDEFINITION_HAS_PENDING_SNOS = 8,
    GROUPDEFINITION_KEEP_TRACK_OF_THREAD_SAFE_UNIQUE_LOCKS = 9,
    GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT = 10,
    GROUPDEFINITION_USE_SECONDARY_GLOBAL_CACHE_LIMIT = 11,
    GROUPDEFINITION_CAN_BE_MOVED = 12,
    GROUPDEFINITION_NO_LOCK_TRACKING = 13,
    GROUPDEFINITION_USE_GLOBAL_CACHE_LIMIT_ON_LOW_MEM_SYSTEM = 14,
    GROUPDEFINITION_IGNORE_DO_NOT_PURGE_BIT_WHEN_MEMORY_IS_CRITICAL = 15,
};

enum GlobalCacheType {
    GLOBALCACHE_MAIN = 0,
    GLOBALCACHE_SECONDARY = 1,
    GLOBALCACHE_COUNT = 2,
};

class GlobalCachePurgeOverride {  // size 0x1
public:
    GlobalCachePurgeOverride(GlobalCacheType);
    ~GlobalCachePurgeOverride();
};
static_assert(sizeof(GlobalCachePurgeOverride) == 0x1, "GlobalCachePurgeOverride");

enum DefinitionHeaderFlags {
    DH_DO_NOT_PURGE_CACHE_BIT = 0,
    DH_IS_BAD_DATA_BIT = 1,
    DH_IN_ONE_BIG_CONTIGUOUS_BLOCK = 2,
    DH_READ_LOCK_HELD_BIT = 3,
    DH_WRITE_LOCK_HELD_BIT = 4,
    DH_IS_EXTERNAL_BIT = 5,
    DH_WAS_POSTPROCESSED_BIT = 6,
    DH_WAS_PRELOADED_BIT = 7,
    DH_PRECACHED_BIT = 8,
    DH_OCTREE_IS_DYNAMIC_BIT = 9,
    DH_PERSISTENT_RESERVED_0_BIT = 16,
    DH_PERSISTENT_RESERVED_1_BIT = 17,
    DH_PERSISTENT_RESERVED_2_BIT = 18,
    DH_PERSISTENT_RESERVED_3_BIT = 19,
    DH_PERSISTENT_RESERVED_4_BIT = 20,
    DH_PERSISTENT_RESERVED_5_BIT = 21,
    DH_PERSISTENT_RESERVED_6_BIT = 22,
    DH_PERSISTENT_RESERVED_7_BIT = 23,
    DH_PERSISTENT_RESERVED_8_BIT = 24,
    DH_PERSISTENT_RESERVED_9_BIT = 25,
    DH_PERSISTENT_RESERVED_a_BIT = 26,
    DH_PERSISTENT_RESERVED_b_BIT = 27,
    DH_PERSISTENT_RESERVED_c_BIT = 28,
    DH_PERSISTENT_RESERVED_d_BIT = 29,
    DH_PERSISTENT_RESERVED_e_BIT = 30,
    DH_BUILT_BY_CLIENT_BIT = 31,
};

enum DefDataElementID {
    DEFDATAELEMENTID_INVALID = -1,
};

struct DefDataElement {  // size 0x10
    DefDataElementID id;  // 0x00
    uint32 dwLastUsedFrame : 24;  // 0x04
    SNOGroup snoGroup : 8;  // 0x04
    void * lpDefinitionBase;  // 0x08
};
static_assert(sizeof(DefDataElement) == 0x10, "DefDataElement");

struct PendingDefDataElement : public DefDataElement {  // size 0x18
    SNO sno;  // 0x10
};
static_assert(sizeof(PendingDefDataElement) == 0x18, "PendingDefDataElement");

struct GlobalPurgeableItem {  // size 0xc
    SNOGroup snoGroup;  // 0x00
    SNO snoHandle;  // 0x04
    uint32 dwLastUsedFrame;  // 0x08
};
static_assert(sizeof(GlobalPurgeableItem) == 0xc, "GlobalPurgeableItem");

struct RequiredDefinitionHeader {  // size 0xc
    void Initialize(const SNO sno, const BOOL fIsBadData, const BOOL fContiguous, const BOOL fIsExternal, const BOOL fGroupDefinitionCantShare);
    SNO snoHandle;  // 0x00
    volatile int32 nLockCount;  // 0x04
    volatile int32 dwFlags;  // 0x08
};
static_assert(sizeof(RequiredDefinitionHeader) == 0xc, "RequiredDefinitionHeader");

// Defined in X/Sigma/Memory/SigmaMemory.cpp:126 and :128.
extern XVirtualHeap * gptMainDefinitionsMemoryHeap;
extern XBaseHeap * gptSecondaryDefinitionsMemoryHeap;

// The destructor (line 183) is implicit. The inline members are at lines 187, 207, 216 and 223 of
// the original; their only copies are the ones emitted with the vtable in Definitions.cpp
// (0x710061a1e0 - 0x710061a270).
class DefinitionHeapAutoPurgeAlloc : public XMemoryAllocatorInterface {  // size 0x8
public:
    virtual void * MemAlloc(size_t size, size_t align, BOOL bClear) {  // vtable slot 2
        return MemAllocTracked(size, align, bClear, NULL, 0);
    }
    virtual void * MemAllocTracked(size_t size, size_t align, BOOL bClear, const char * szFile, uint32 dwLine);  // vtable slot 3
    virtual void MemFree(void * pMem);  // vtable slot 4
    virtual void * MemResize(void * pMem, size_t size, size_t align) {  // vtable slot 5
        DisplayInternalError(NULL, "", 212, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
    virtual BOOL ContainsMem(void * pMem) {  // vtable slot 7
        return gptMainDefinitionsMemoryHeap->ContainsMem(pMem);
    }
    virtual void PrintUsage() {  // vtable slot 8
        gptMainDefinitionsMemoryHeap->PrintUsage();
    }
};
static_assert(sizeof(DefinitionHeapAutoPurgeAlloc) == 0x8, "DefinitionHeapAutoPurgeAlloc");

extern DefinitionHeapAutoPurgeAlloc stDefinitionHeapAutoPurgeAlloc;  // Definitions.cpp:148

struct ThreadSafeDef {  // size 0x70
    Blizzard::Lock::RMutex tMapChangeLock;  // 0x00
    Blizzard::Lock::RMutex tPendingLock;  // 0x20
    Blizzard::Lock::RMutex tPendingListLock;  // 0x40
    volatile int32 nMapChangeLock;  // 0x60
    Blizzard::Thread::Thread tCurrentMapChanger;  // 0x68
};
static_assert(sizeof(ThreadSafeDef) == 0x70, "ThreadSafeDef");

class GroupDefinition {  // size 0xa0
    // vtable pointer
public:
    GroupDefinition(LPCSTR, const SNOGroup, const SNOVersions, const SNODataType, const uint32, const XTypeDescriptor *, uint32, uint32);
    virtual ~GroupDefinition();  // vtable slot 0
    static GroupDefinition * Get(const SNOGroup);
    template <typename T> static GroupDefinition * Create() {  // line 300
        return new T;
    }
    virtual void * MemAlloc(size_t, uint32) const = 0;  // vtable slot 2
    virtual void * MemAlloc(size_t, uint32, XMemoryAllocatorInterface *) const = 0;  // vtable slot 3
    virtual void MemFree(void *&) const = 0;  // vtable slot 4
    virtual void MemFree(void *&, XMemoryAllocatorInterface *) const = 0;  // vtable slot 5
    void Init();
    SNOPackType GetSNOPackDestination(const SNO sno);
    virtual BOOL SNOPackDestinationIsVariable() const { return FALSE; }  // vtable slot 6, line 326
    virtual SNOPackType GetSNOPackDestination(const void *) const = 0;  // vtable slot 7
    virtual uint32 GetPrecacheDependencyCount() { return 0; }  // vtable slot 8, line 335
    virtual void ResizeCache(uint32 dwCacheSizeOverride);  // vtable slot 9
    virtual void Dispose();  // vtable slot 10
    virtual uint32 GetMaxCount() = 0;  // vtable slot 11
    virtual uint32 GetEstCacheSizeLimit() { return 0; }  // vtable slot 12, line 354
    virtual BOOL CanReferenceOtherSNOs() const = 0;  // vtable slot 13
    virtual void Read(const RequiredDefinitionHeader & tHeader, void *& lpDefinition, SNOElement * ptSNOElement, BOOL fContiguousBuffer, uint32 dwContiguousSize, XMemoryAllocatorInterface * ptMemoryInterface) const;  // vtable slot 14
    BOOL ReadServerOnlyChildFile(const SNOElement *, unsigned char *&, uint32 &, const XTypeDescriptor *) const;
    static void FlushAllAxeBadData();
    void ReadFromMemoryBuffer(void * lpDefinition, uint32 dwDefinitionSize, XMemoryAllocatorInterface * ptMemoryInterface) const;
    virtual BOOL ValidateSourceFiles(const void *) const;  // vtable slot 15
    virtual BOOL ValidateCompiledSourceFiles(const void *) const;  // vtable slot 16
    virtual void Postprocess(void * lpDefinition);  // vtable slot 17
    virtual void Delete(void * lpDefinition) const;  // vtable slot 18
    virtual BOOL Write(void * lpDefinition, FileReference & tRef) const;  // vtable slot 19
    uint32 ComputeWriteSize(void *) const;
    BOOL WriteServerOnlyData(const XTypeDescriptor *, void *, FileReference &) const;
    void WriteServerOnlyChildFile(const FileReference &, const void *, const uint32) const;
    void ReadDefinitionHeader(SNOElement * ptSNOElement, void * lpDefinition);
    virtual BOOL WriteMetaData(const SNOName & snoName);  // vtable slot 20
    virtual void GetDefaultData(void * ptDefaultDef) const;  // vtable slot 21
    void DeleteAll();
    BOOL SetRequired(const SNO sno, BOOL fRequired);
    void SetRequired(void * lpDefinition, BOOL fRequired);
    BOOL TestLock(const SNO);
    virtual BOOL LoadChildFile(SNOElementChild * ptSNOElementChild);  // vtable slot 22
    virtual uint32 GetEstimatedChildFilesSize(const SNO sno) const;  // vtable slot 23
    void Preload(const SNO sno);
    const void * LockR(const SNO sno, const BOOL fLockCanFail);
    const void * LockRFast(const SNO sno);
    const void * LockR(void * lpDefinition);
    void UnlockR(void *& lpDef);
    void * LockW(const SNO sno, const BOOL fLockCanFail);
    void * LockW(void * lpDefinition);
    void UnlockW(void *& lpDef);
    SNO NewSNOFile(const XLocale, LPCSTR, void *, const SNO);
    void SNOWrite(const SNO sno, void * lpDefinition);
    void SNODelete(const SNO sno);
    BOOL SNOWriteSNOFile(const XLocale eXLocale, const SNO sno, LPCSTR szFileNameNoExtension, void * lpDefinition, const uint32 dwXMLHash);
    SNO New(const void * lpDefinitionData, BOOL fContiguous, uint32 dwContinuousSize);
    void * LoadExternalDef(const SNO);
    virtual void WriteExternalDef(const SNO sno, void * lpDefinition);  // vtable slot 24
    void DeleteExternalDef(void *&);
    void GetSNOName(const void * lpDefinition, SNOName & snoname);
    BOOL DeleteIfVersionIsWrong(const SNO sno);
    BOOL Purge(const SNO sno, BOOL fAlreadyProtected, BOOL fFromAutoPurge, BOOL fMemoryIsCritical, GlobalCacheType eType);
    virtual void PurgeDefinitionAndSNO(const SNO sno);  // vtable slot 25
    BOOL Exists(const SNO sno, BOOL * pfPending) const;
    LPCSTR GetName() const { return snoGroupInfo.szName; }  // line 610
    SNOGroup GetGroup() const { return snoGroupInfo.snoGroup; }  // line 616
    SNOName GetSNOName(SNO) const;
    // Inline at line 628; the definition below delegates to SNOVersions::GetVersion
    // and is verified through HeroDigest::Import.
    uint32 GetCurrentVersion(SigmaPlatform ePlatform) const;
    uint32 GetLastFrameUsed(const SNO) const;
    static void UpdateDefrag(BOOL, BOOL);
    static void InvalidateDefrag(GlobalCacheType, void *);
    static void InitializeSNOMap();
    static void DisposeSNOMap();
    void VersionAll(const BOOL);
    SNO GetFallbackDataSNO() const;
    BOOL WasDynamicallyCreated(const SNO snoHandle) const;
    virtual void IterateSnoReferences(const void * lpDefinition, SNOIterateFunc pfuncIterate, void * pData);  // vtable slot 26
    void * DefinitionDataArrayIterate(TDataArrayIterator<void> & tIt, void *& ptDefinition) const;
    virtual uint32 GetContiguousReadSize(SNOElement * ptSNOElement);  // vtable slot 27
    BOOL CacheManagerPurge(const BOOL fLockCanFail);
    static BOOL GlobalCacheManagerPurge(GlobalCacheType eGlobalCacheType, const BOOL fLockCanFail, const BOOL fMemoryIsCritical);
    static void GlobalCacheManagerClearPurgeLists(GlobalCacheType eGlobalCacheType);
    void CacheManagerGetDebugPurgeList(BOOL, BOOL, CRefString &);
    uint32 CacheManagerGetElementCount() const;
    virtual BOOL AllowMultipleCheckout(SNO snoHandle) { return FALSE; }  // vtable slot 28, line 729
    BOOL IsThreadSafe() const;
    BOOL NoLockTracking() const { return dwFlags & ( 1 << GROUPDEFINITION_NO_LOCK_TRACKING ); }  // line 742; body from the copy inlined into DisplayList::SubmitRenderableHeader (raw bit 13 of dwFlags tested)
    uint32 Flags() const;
    uint32 GetNumUniqueLocks() const;
    const XTypeDescriptor * GetTypeDescriptor() const;
    static float GetGlobalCacheStreamed();
    uint32 dwIdenticalLookups;  // 0x08
    uint32 dwTotalLookups;  // 0x0c
    POS posLockCtr;  // 0x10
protected:
    BOOL CacheManagerShouldPurge(uint32 dwEstNewElementSize, BOOL fWillCreateInstance) const;
    SNO CacheManagerFindElementToPurge() const;
    BOOL AutoPurgeIfNeeded(uint32 dwEstNewElementSize, BOOL fWillCreateInstance, BOOL fLockCanFail, BOOL fNeedsUnlock);
    static BOOL GlobalCacheManagerShouldPurge(GlobalCacheType, uint32);
    static SNOName GlobalCacheManagerFindElementToPurge(GlobalCacheType eGlobalCacheType, BOOL fMemoryIsCritical);
    static void GlobalCacheManagerGeneratePurgeList(GlobalCacheType eGlobalCacheType, BOOL fMemoryIsCritical);
    BOOL GlobalSNOCanBePurged(SNO sno, uint32 dwLastUsedFrame, BOOL fMemoryIsCritical);
    virtual BOOL CacheManagerCanPurgeElement(const DefDataElement & tDefElement, BOOL fMemoryIsCritical) const;  // vtable slot 29
    BOOL ValidateSourceFilesEx(const void *, const BOOL) const;
    BOOL ValidateCompiledSourceFilesEx(const void *, const BOOL) const;
    static Blizzard::Lock::RMutex stDynamicDefinitionLock;
    static Blizzard::Lock::RMutex stDynamicDefinitionHandleLock;
    static DefDataElementID spptDynamicDefinitionArray[10000];
    static uint32 sdwLowestUnusedDynamicHandle;
    TDataArray<DefDataElement, DefDataElementID> * ptInstances;  // 0x18
    TDataArray<PendingDefDataElement, DefDataElementID> * ptPendingInstances;  // 0x20
    static Blizzard::Lock::RMutex stGlobalInstanceLock;
    static TDataArray<DefDataElement, DefDataElementID> * sptGlobalInstances;
private:
    // Instantiated as <false, false>, <true, false>, <true, true> and <false, true>. The template
    // parameter names are not in the DWARF.
    template <bool bThreadSafe, bool bUpdateLastUsedFrame>
    DefDataElement * sGet(const SNO sno, const BOOL fWritable, const BOOL fLockCanFail, const BOOL fForPreload);
    DefDataElement * sCreateAndLoad(const SNO sno, const BOOL fWritable, const BOOL fLockCanFail, const BOOL fForPreload);
    void sLoad(const SNO sno, const BOOL fIsExternal, const BOOL fWritable, const BOOL fLockCanFail, void *& lpDefinition, DefDataElementID & idDefinition, const BOOL fFromPending, const BOOL fForPreload);
    void sCreate(const SNO sno, const SNO snoHandleToReallyLoad, const BOOL fContiguous, const uint32 dwContiguousSize, DefDataElementID idDefinition, void *& lpDefinition, const BOOL fForceThreadUnsafe);
    void sReportBadDataToUser(const SNO sno);
    template <bool bThreadSafe> DefDataElement * HandleError(SNO sno, BOOL fWritable, BOOL fLockCanFail);
    XMemoryAllocatorInterface * sGetMemoryInterfaceToUse() const;
    void LockMapChange() const;
    void UnlockMapChange() const;
    void BeginMapChange() const;
    void EndMapChange() const;
    void LockPendingSNOList() const;
    void UnlockPendingSNOList() const;
    void LockPendingSNOs() const;
    void UnlockPendingSNOs() const;
    void MergePendingSNOs();
    static DefDataElementID GetDefinition(SNO snoHandle);
    static void SetDefinition(SNO snoHandle, DefDataElementID defDataElementID);
    static void sStartDefrag();
    static void sStopDefrag();
    static uint sDefragProc(void *);
    uint32 dwFlags;  // 0x28
    SNOGroupInfo snoGroupInfo;  // 0x30
    uint32 dwMaxCount;  // 0x78
    uint32 dwInstanceSize;  // 0x7c
    static uint32 sdwGlobalPurgeArrayCount[2];
    static GlobalPurgeableItem spGlobalPurgeArray[2][512];
    static Blizzard::Lock::RMutex stGlobalCacheLock[2];
    const XTypeDescriptor * ptTypeDesc;  // 0x80
    volatile int32 dwNumUniqueLocks;  // 0x88
    static uint32 sdwSNOInArray;
    static DefDataElementID * spptDefinitionArray;
    typedef XGrowableMap<int, int, DefDataElementID, const DefDataElementID &> MapSNOToDefDataElementID;
    static GroupDefinition::MapSNOToDefDataElementID stOverflowDefinitions;
    ThreadSafeDef * pTSDef;  // 0x90
    mutable SNO snoFallback;  // 0x98 (written by GetFallbackDataSNO() const)
    template <bool bThreadSafe>
    DefDataElement * sGetForLoad(const SNO sno, const BOOL fWritable, const BOOL fLockCanFail, const BOOL fForPreload);
};
static_assert(sizeof(GroupDefinition) == 0xa0, "GroupDefinition");

typedef GroupDefinition *(*GroupDefinitionCreationFcn)();

// Every GroupDefinition subclass defines its two MemAlloc and two MemFree overrides on a single
// source line (PowerDefinition.h:85, FlagSetDefinition.h:44, ...), and all 66 of them compile to
// the same code, so they come from a macro. Its name is not in the DWARF; this one is invented.
// The parameter names are the DWARF's.
#define GROUPDEFINITION_MEMORY_FUNCTIONS()                                                                   \
    virtual void * MemAlloc(size_t nSize, uint32 dwAlign) const {                                            \
        return SigmaMemoryNew(nSize, dwAlign, &stDefinitionHeapAutoPurgeAlloc, FALSE);                       \
    }                                                                                                        \
    virtual void * MemAlloc(size_t nSize, uint32 dwAlign, XMemoryAllocatorInterface * ptMemoryInterface) const { \
        if (ptMemoryInterface)                                                                               \
            return SigmaMemoryNew(nSize, dwAlign, ptMemoryInterface, FALSE);                                 \
        return MemAlloc(nSize, dwAlign);                                                                     \
    }                                                                                                        \
    virtual void MemFree(void *& ptr) const {                                                                \
        SigmaMemoryFree(ptr, &stDefinitionHeapAutoPurgeAlloc);                                               \
        ptr = NULL;                                                                                          \
    }                                                                                                        \
    virtual void MemFree(void *& ptr, XMemoryAllocatorInterface * ptMemoryInterface) const {                 \
        if (ptMemoryInterface) {                                                                             \
            SigmaMemoryFree(ptr, ptMemoryInterface);                                                         \
            ptr = NULL;                                                                                      \
        } else {                                                                                             \
            MemFree(ptr);                                                                                    \
        }                                                                                                    \
    }

// The lock helpers below are declared here and explicitly specialized for each definition type
// in that type's own header (DefinitionLockR<PowerDefinition> is at PowerDefinition.h:161, for
// example). The DWARF records only the specializations; these primary declarations follow from
// their mangled names (_Z15DefinitionLockRI15PowerDefinitionEPKT_i,
// _Z15DefinitionLockRI16GlobalDefinitionEPKT_v, _Z17DefinitionUnlockRI15PowerDefinitionEvPKT_,
// _Z15DefinitionLockWI17TextureDefinitionEPT_i, _Z17DefinitionUnlockWI17TextureDefinitionEvPT_).
template <typename DEFINITION> const DEFINITION * DefinitionLockR(SNO sno);
template <typename DEFINITION> const DEFINITION * DefinitionLockR();
template <typename DEFINITION> void DefinitionUnlockR(const DEFINITION * pdef);
template <typename DEFINITION> DEFINITION * DefinitionLockW(SNO sno);
template <typename DEFINITION> void DefinitionUnlockW(DEFINITION * pdef);

// Class templates. The DWARF has only instantiations (member names, sizes and the line of each
// inline member); the bodies are written from the code inlined into callers.
// Checked by tests/X/Common/Definitions/definition_locks.cpp.

// line 987; 59 instantiations
template <typename DEFINITION> class ScopedDefinitionLockR {  // size 0x8
public:
    ScopedDefinitionLockR(SNO sno) : pdef(DefinitionLockR<DEFINITION>(sno)) {}  // line 990
    ~ScopedDefinitionLockR() { DefinitionUnlockR<DEFINITION>(pdef); }  // line 991
    operator const DEFINITION *() { return pdef; }
    const DEFINITION * operator->() { return pdef; }
protected:
    const DEFINITION * pdef;  // 0x00
private:
    ScopedDefinitionLockR(const ScopedDefinitionLockR<DEFINITION> &);
    ScopedDefinitionLockR<DEFINITION> & operator=(const ScopedDefinitionLockR<DEFINITION> &);
};

// line 1005; 1 instantiation (GlobalDefinition)
template <typename DEFINITION> class ScopedSingletonDefinitionLockR {  // size 0x8
public:
    ScopedSingletonDefinitionLockR() : pdef(DefinitionLockR<DEFINITION>()) {}  // line 1008
    // Body guessed: no caller has anything inlined from this destructor, and GlobalDefinition.h
    // has no DefinitionUnlockR<GlobalDefinition>, so it generates no code.
    ~ScopedSingletonDefinitionLockR() {}
    operator const DEFINITION *() { return pdef; }
    const DEFINITION * operator->() { return pdef; }
protected:
    const DEFINITION * pdef;  // 0x00
private:
    ScopedSingletonDefinitionLockR(const ScopedSingletonDefinitionLockR<DEFINITION> &);
    ScopedSingletonDefinitionLockR<DEFINITION> & operator=(const ScopedSingletonDefinitionLockR<DEFINITION> &);
};

// line 1023; 1 instantiation (TextureDefinition)
template <typename DEFINITION> class ScopedDefinitionLockW {  // size 0x8
public:
    ScopedDefinitionLockW(SNO sno) : pdef(DefinitionLockW<DEFINITION>(sno)) {}  // line 1026
    ~ScopedDefinitionLockW() { DefinitionUnlockW<DEFINITION>(pdef); }  // line 1027
    operator DEFINITION *() { return pdef; }
    DEFINITION * operator->() { return pdef; }
protected:
    DEFINITION * pdef;  // 0x00
private:
    ScopedDefinitionLockW(const ScopedDefinitionLockW<DEFINITION> &);
    ScopedDefinitionLockW<DEFINITION> & operator=(const ScopedDefinitionLockW<DEFINITION> &);
};

// line 1059; 1 instantiation (PowerDefinition)
template <typename DEFINITION> struct AutoDefinitionR {  // size 0x10
private:
    // Explicitly specialized in the definition's header (PowerDefinition.h:170 and 171).
    const DEFINITION * AutoDefinitionLockR(SNO sno);
    void AutoDefinitionUnlockR(const DEFINITION * _pdef);
public:
    AutoDefinitionR(const DEFINITION * pdef_) : fLocked(FALSE), pdef(pdef_) {}  // line 1061
    AutoDefinitionR(ScopedDefinitionLockR<DEFINITION> & pdef_) : fLocked(FALSE), pdef(pdef_) {}  // line 1063
    AutoDefinitionR(SNO sno) : fLocked(TRUE) { pdef = sno != SNO_NONE ? AutoDefinitionLockR(sno) : NULL; }  // line 1065
    AutoDefinitionR(const AutoDefinitionR<DEFINITION> & pdef_) : fLocked(FALSE), pdef(pdef_) {}  // line 1067
    ~AutoDefinitionR() { if (fLocked && pdef) AutoDefinitionUnlockR(pdef); }  // line 1069
    SNO GetSNO() const { return pdef ? pdef->tHeader.snoHandle : SNO_NONE; }  // line 1071
    const DEFINITION * operator->() const { return pdef; }  // line 1073
    operator const DEFINITION *() const { return pdef; }  // line 1075
private:
    BOOL fLocked;  // 0x00
    const DEFINITION * pdef;  // 0x08
};

// Functions defined in this file (bodies are not in the DWARF):
//   line   183  DefinitionHeapAutoPurgeAlloc::~DefinitionHeapAutoPurgeAlloc()  [0x710061a1e0]
//   line   187  void * DefinitionHeapAutoPurgeAlloc::MemAlloc(size_t size, size_t align, BOOL bClear)  [0x710061a1f0]
//   line   207  void * DefinitionHeapAutoPurgeAlloc::MemResize(void * pMem, size_t size, size_t align)  [0x710061a210]
//   line   216  BOOL DefinitionHeapAutoPurgeAlloc::ContainsMem(void * pMem)  [0x710061a250]
//   line   223  void DefinitionHeapAutoPurgeAlloc::PrintUsage()  [0x710061a270]
//   line   260  ThreadSafeDef::ThreadSafeDef()  [inlined into 1 functions]
//   line   260  ThreadSafeDef::~ThreadSafeDef()  [inlined into 1 functions]
//   line   300  GroupDefinition * GroupDefinition::Create<TextureGroupDef>()  [0x7100435c00 (+65 more)]
//   line   309  SNOPackType GroupDefinition::GetSNOPackDestination(const SNO sno)  [inline]
//   line   326  BOOL GroupDefinition::SNOPackDestinationIsVariable() const  [0x71001a8290]
//   line   335  uint32 GroupDefinition::GetPrecacheDependencyCount()  [0x71001a82b0]
//   line   354  uint32 GroupDefinition::GetEstCacheSizeLimit()  [0x71001ab670]
//   line   610  LPCSTR GroupDefinition::GetName() const  [inlined into 2 functions]
//   line   616  SNOGroup GroupDefinition::GetGroup() const  [inlined into 18 functions]
//   line   628  uint32 GroupDefinition::GetCurrentVersion(SigmaPlatform ePlatform) const  [inlined into 12 functions]
//   line   729  BOOL GroupDefinition::AllowMultipleCheckout(SNO snoHandle)  [0x71001a82d0]
//   line   742  BOOL GroupDefinition::NoLockTracking() const  [inlined into 1 functions]
//   line   990  ScopedDefinitionLockR<MarkerSetDefinition>::ScopedDefinitionLockR(SNO sno)  [inlined into 1124 functions]
//   line   991  ScopedDefinitionLockR<MarkerSetDefinition>::~ScopedDefinitionLockR()  [0x71000d2860, inlined into 787 functions]
//   line  1008  ScopedSingletonDefinitionLockR<GlobalDefinition>::ScopedSingletonDefinitionLockR()  [inlined into 182 functions]
//   line  1026  ScopedDefinitionLockW<TextureDefinition>::ScopedDefinitionLockW(SNO sno)  [inlined into 1 functions]
//   line  1027  ScopedDefinitionLockW<TextureDefinition>::~ScopedDefinitionLockW()  [inlined into 1 functions]
//   line  1061  AutoDefinitionR<PowerDefinition>::AutoDefinitionR(const PowerDefinition * pdef_)  [inlined into 10 functions]
//   line  1063  AutoDefinitionR<PowerDefinition>::AutoDefinitionR(ScopedDefinitionLockR<PowerDefinition> & pdef_)  [inlined into 47 functions]
//   line  1065  AutoDefinitionR<PowerDefinition>::AutoDefinitionR(SNO sno)  [inlined into 292 functions]
//   line  1067  AutoDefinitionR<PowerDefinition>::AutoDefinitionR(const AutoDefinitionR<PowerDefinition> & pdef_)  [inlined into 137 functions]
//   line  1069  AutoDefinitionR<PowerDefinition>::~AutoDefinitionR()  [inlined into 329 functions]
//   line  1071  SNO AutoDefinitionR<PowerDefinition>::GetSNO() const  [inlined into 34 functions]
//   line  1073  const PowerDefinition * AutoDefinitionR<PowerDefinition>::operator->() const  [inlined into 23 functions]
//   line  1075  AutoDefinitionR<PowerDefinition>::operator const PowerDefinition *() const  [inlined into 135 functions]

// Line 628; proven through HeroDigest::Import's platform-version lookup.
inline uint32 GroupDefinition::GetCurrentVersion(SigmaPlatform ePlatform) const {
    return snoGroupInfo.tVersions.GetVersion(ePlatform);
}

// Defined in X/Common/Definitions/Definitions.cpp (X/Common/Game.cpp sPrecacheDefinitions).
BOOL DefinitionShouldPrecache();
void DefinitionSetCurrentlyPreCaching(BOOL bPreCaching);
