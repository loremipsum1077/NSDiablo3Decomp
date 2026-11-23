// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/ActorCommonData.h
// Subset: ActorLocation, the leading members of ActorCommonData, ACDGlobals' ACD array and the
// ACD lookups. ActorCommonData's members from 0xb8 on are not adopted yet; an invented
// placeholder array keeps the class its original size, which the ACD array's indexing needs.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "CommonTypes.h"
#include "GameBalance.h"
#include "GameCommonData.h"
#include "SNO/SNOFiles.h"
#include "SNO/SNOName.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/Math/Cylinder.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/SigmaTypes.h"
#include "TeamCommon.h"
#include "../Sigma/RefString.h"
#include "ActorCommonCollision.h"
#include "Items.h"
class XSphereNode;
class ActorMovement;
struct Player;
struct ResolvedPortalDestination;  // X/Common/Definitions/ActDefinition.h
struct FastAttribKey;
struct FastAttribGroup;
struct ACDPack;

typedef int32 WeaponClass;

typedef int32 AttribGroupID;

typedef int32 HirelingClass;

typedef int32 ActorType;  // line 77

typedef int32 GameConnectionID;  // X/Server/PlayerList.h

enum {
    ACDPACKID_INVALID = -1,
};

typedef int32 ACDPackID;

struct ActorInventory;
struct VisualInventory;
class PlayerList;
struct ActorDefinition;
struct ActorSavedAttributes;
struct StackSerializer;
struct Power;
struct GameBalanceDefinition;
struct EliteModifierData;
struct Quaternion;

typedef int32 RuneType;  // X/Common/Definitions/AttachmentDefinition.h
typedef int32 ParentType;  // X/Client/AttachmentParent.h
typedef int32 AttributeTimerType;  // line 69
typedef int32 AffixMessageFlag;  // line 75
typedef int32 PickupResponse;  // line 85

enum {
    WALKTYPE_WALK = 0,
    WALKTYPE_RUN = 1,
    WALKTYPE_SPRINT = 2,
    WALKTYPE_STRAFE = 3,
    WALKTYPE_OTHER = 4,
    WALKTYPE_COUNT = 5,
};

typedef int32 WalkType;

enum {
    GMR_UNKNOWN = 0,
    GMR_SOLD = 1,
    GMR_KILL = 2,
    GMR_QUEST = 3,
    GMR_SCRIPT = 4,
    GMR_CRAFTING = 5,
    GMR_BUY_STASH = 6,
    GMR_PURCHASE = 7,
    GMR_CHEAT_REPAIR_ALL = 8,
    GMR_CHEAT = 9,
    GMR_TRADE = 10,
    GMR_TRADE_FAILED = 11,
    GMR_PICKUP = 12,
    GMR_RESET_PARAGON_POINTS = 13,
    GMR_MAIL = 14,
    GMR_ACHIEVEMENT = 15,
    GMR_DAILY_FIRST = 16,
    GMR_STORE_PURCHASE = 17,
    GMR_EMPOWERED_RIFT = 18,
    GMR_CURRENCY_CONVERSION = 19,
    GMR_CUBE_RECIPE = 20,
    GMR_COUNT = 21,
};

typedef int32 GoldModifiedReason;

enum EliteClass {
    ELITECLASS_INVALID = -1,
    ELITECLASS_CHAMPION = 0,
    ELITECLASS_RARE = 1,
    ELITECLASS_RARE_MINION = 2,
    ELITECLASS_TOUGH = 3,
    ELITECLASS_PLAYER = 4,
};
struct RNG;
struct AnimPlayer;
struct AvengerData;

enum {
    AL_NONE = -1,
    AL_WORLD = 0,
    AL_INVENTORY = 1,
    AL_COUNT = 2,
};

typedef int32 ActorLocationType;

typedef int32 GizmoType;

typedef int32 InventorySlotType;

typedef blz::vector<ACDID, blz::allocator<ACDID> > ACDIDArray;  // line 241
typedef XPooledDataIDList BuffIDListPooled;  // ActorCommonData.h:239
typedef XPooledDataIDList ACDIDListPooled;  // line 235
typedef XPooledDataIDList ComplexEffectIDListPooled;  // line 244

struct InventoryLocation {  // size 0x10
    ACDID idACDContainer;  // 0x00
    InventorySlotType eSlot;  // 0x04
    IVector2D pt;  // 0x08
};
static_assert(sizeof(InventoryLocation) == 0x10, "InventoryLocation");

typedef int32 PickupAction;

struct PickupResult {  // size 0x14
    PickupAction ePickupAction;  // 0x00
    InventoryLocation tNewLocation;  // 0x04
};
static_assert(sizeof(PickupResult) == 0x14, "PickupResult");

struct WorldLocation {  // size 0x58
    PRTransform transform;  // 0x00
    float flScale;  // 0x1c
    Sphere wsBounds;  // 0x20
    AxialCylinder wcylBounds;  // 0x30
    XSphereNode * ptSphereNode;  // 0x48
    SWorldID idSWorld;  // 0x50
    SSceneID idSScene;  // 0x54
};
static_assert(sizeof(WorldLocation) == 0x58, "WorldLocation");

struct ActorLocation {  // size 0x70
    ActorLocationType eActorLocation;  // 0x00
    WorldLocation tWorldLocation;  // 0x08
    InventoryLocation tInvLocation;  // 0x60
};
static_assert(sizeof(ActorLocation) == 0x70, "ActorLocation");

// X/Common/ActorCommonDataSwig.h
enum {
    MR_NONE = -1,
    MR_NORMAL = 0,
    MR_CHAMPION = 1,
    MR_RARE = 2,
    MR_RARE_MINION = 3,
    MR_UNIQUE = 4,
    MR_HIRELING = 5,
    MR_BOSS = 7,
    MR_NUM_RARITIES = 8,
};

typedef int32 MonsterRarity;

typedef int32 ResourceType;  // X/Common/ActorCommonDataSwig.h

typedef int32 DyeType;  // line 72

typedef int32 CurrencyType;  // X/Common/Items.h

enum {
    ACDMODIFIED_ATTRIBUTES = 0,
    ACDMODIFIED_ONE_SHOTS = 1,
    ACDMODIFIED_VISUAL_INVENTORY = 2,
    ACDMODIFIED_AFFIX = 3,
    ACDMODIFIED_GB_HANDLE = 4,
    ACDMODIFIED_RARE_MONSTER_NAME = 5,
    ACDMODIFIED_RARE_ITEM_NAME = 6,
    ACDMODIFIED_PORTAL_SPECIFIER = 7,
    ACDMODIFIED_INVENTORY_POSITION = 8,
    ACDMODIFIED_INVENTORY_ACTOR_SNO = 9,
    ACDMODIFIED_MOVEMENT = 10,
    ACDMODIFIED_WORLD_POSITION = 11,
    ACDMODIFIED_INVENTORY_CREATED = 12,
    ACDMODIFIED_WARPED = 13,
    ACDMODIFIED_COMPLEX_EFFECTS = 14,
    ACDMODIFIED_COLL_FLAGS = 15,
    ACDMODIFIED_GROUP = 16,
    ACDMODIFIED_NUM_TYPES = 17,
};

typedef int32 ACDModifiedType;

enum {  // the actor flag bits, from the DWARF at ActorCommonData.h:197-216
    ACD_USES_SHEAR_BIT = 0,
    ACD_SEND_HERO_STATE_BIT = 1,
    ACD_IS_FROM_DISTRIBUTION_BIT = 2,
    ACD_EDIT_MODE_ONLY_BIT = 3,
    ACD_QUEUE_INVENTORY_EQUIPMENT_OR_SKILL_CHANGED_BIT = 4,
    ACD_QUEUE_EQUIPPED_ITEMS_CHANGED_BIT = 5,
    ACD_QUEUE_SKILL_CHANGED_BIT = 6,
    ACD_QUEST_OBJECTIVE = 7,
    ACD_FASTMOVER_BIT = 8,
    ACD_SEND_HERO_STATE_SAVEDDATA_BIT = 9,
    ACD_SEND_HERO_STATE_PLAYERFLAGS_BIT = 10,
    ACD_SEND_HERO_STATE_WAYPOINTS_BIT = 11,
    ACD_SEND_HERO_STATE_LASTSAVEPOINT_BIT = 12,
    ACD_SEND_HERO_STATE_CURRENCY_BIT = 13,
    ACD_SEND_HERO_STATE_LAST_WAYPOINT_LEVELAREA_BIT = 14,
    ACD_KNOWN_WITH_SCENE = 15,
    ACD_IS_BEING_PICKED_UP = 16,
    ACD_SUPPRESS_PASSIVE_APPLICATION = 17,
};

struct KnownMessage {  // size 0x2c
    uint32 dwDestinationsPlayerIndexMask;  // 0x00
    uint8 byteBuffer[40];  // 0x04
};
static_assert(sizeof(KnownMessage) == 0x2c, "KnownMessage");

typedef XPooledList<KnownMessage, KnownMessage> TKnownMessageList;

struct FriendGiftData {  // original ActorCommonData.h:265, size 0x20
    FriendGiftData() { idRecipient = 0; }  // line 267
    uint64 idRecipient;
    CRefString sNameRecipient;
};
static_assert(sizeof(FriendGiftData) == 0x20, "FriendGiftData");

class ActorCommonData {  // size 0x340
public:
    WorldPlace GetPlace() const;
    const ActorLocation & GetLocation() const;  // ActorCommonData.cpp:2485
    ActorLocationType GetLocationType() const;
    BOOL IsInWorld() const;
    BOOL IsAlive() const;
    BOOL IsHidden() const;
    BOOL IsMonster() const;
    BOOL IsPlayerTeam() const;  // original ActorCommonData.h:316
    WorldLocation & GetWorldLocation();
    BOOL AttributesAttributeExists(const FastAttribKey attribKey) const;
    int32 AttributesGetInt(FastAttribKey tKey) const;
    SNO AttributesGetPower(const int32 nIndex) const;  // ActorCommonData.h: original declaration; ActorAttributes.cpp
    AttribGroupID AttributesGetAttributeGroup() const;  // ActorAttributes.cpp
    const int32 GetComboLevel() const;
    void AttributesSetInt(FastAttribKey tKey, int32 nValue);
    float AttributesGetFloat(FastAttribKey tKey) const;
    GBHandle GetGBHandle() const;  // original ActorCommonData.h:346
    float AttributesSetFloat(FastAttribKey tKey, float flValue);
    float GetScale() const;
    ResourceType GetPrimaryResourceType() const;
    float AddResource(ResourceType eResourceType, float flAmount, BOOL bApplyOnHitBonuses);  // ActorCommonData.cpp:3651
    float GetHitpointPercent() const;
    float GetHitPointsCurrent() const;  // ActorCommonData.h:415 (reference)
    BOOL SetHitPoints(const float flHitPoints, const BOOL bSendEvent, const BOOL bCheckHealthFrozen);
    BOOL CanGainMoreExperience() const;
    uint64 GetExperienceNext() const;  // ActorCommonData.h:833
    uint64 GetAltExperienceNext() const;  // ActorCommonData.h:837
    uint64 GetRestExperience() const;  // ActorCommonData.h:841
    WorldPlace GetWorldPlaceCenter() const;
    SWorldID GetSWorldID() const;
    SNO GetSkillKit() const;
    DyeType GetDyeType() const;
    BOOL CanAffordCurrencyCost(int64 nCost, CurrencyType eCurrencyType) const;
    int64 GetCurrencyAmount(CurrencyType eCurrencyType) const;
    BOOL IsKnownByPlayer(const Player & tPlayer) const;
    GizmoType GetGizmoType() const;
    BOOL IsItem() const;
    BOOL IsNecroCorpse() const;
    BOOL IsEnabled() const;  // ActorCommonData.cpp
    SNO GetActorSNO() const;  // ActorCommonData.cpp
    BOOL IsNPC() const;
    BOOL IsHelper() const;
    BOOL IsGizmo() const;
    BOOL IsOperatableGizmo(ACDID idACDOperator, BOOL bIgnoreCantStartDisplayedPowers) const;
    BOOL IsDueling() const;  // ActorCommonData.cpp:5622
    BOOL IsWreckableObject() const;
    BOOL IsChest() const;
    WorldDistance GetRadius() const;
    Player * GetPetOwnerPlayer() const;  // ActorCommonData.cpp:5426
    BOOL IsPet() const;  // line 900
    BOOL IsAttached() const;  // ActorCommonData.cpp:5288
    ActorMovement * GetActorMovement();  // ActorCommonData.cpp:987
    const ActorMovement * GetActorMovement() const;
    void MarkHeroStateChanged();  // ActorCommonData.cpp
    void ModifiedMark(const ACDModifiedType eModifiedType);
    Team GetTeam() const;  // ActorCommonData.cpp
    BOOL HasSpawner() const;  // ActorCommonData.cpp
    const uint64 GetAssignedHeroIDLow() const;  // ActorCommonData.cpp
    // ActorAttributes.cpp
    void AttributesUpdateCached();  // ActorAttributes.cpp:31
    void SetAttributeGroup(const AttribGroupID idFastAttributeGroup);  // ActorAttributes.cpp:60
    float AttributesGetBaseFloat(FastAttribKey tKey) const;  // ActorAttributes.cpp:87
    SNO AttributeGetBasicAttack() const;  // ActorAttributes.cpp:267
    void AttributesGetPowers(uint32 dwSkillQueryFlags, SNOPooledList & listSkills,
                             SNO * psnoAttackPower) const;  // ActorAttributes.cpp:280
    void AttributesUnmergeAttributeGroup(AttribGroupID idFastAttribUnmerge);  // ActorAttributes.cpp:427
    void AttributesMergeBaseAttributeGroup(AttribGroupID idFastAttribMerge);  // ActorAttributes.cpp:439
    void AttributesMergeAttributeGroup(AttribGroupID idFastAttribMerge);  // ActorAttributes.cpp:451
    // Declared from the reference header (ActorCommonData.h:259-563).
    const WorldLocation & GetWorldLocation() const;
    ActorLocation & GetLocation();
    void Initialize(const ACDNetworkName & _ann, const DataID _idOwner, const SNO _snoActor, const GBHandle & _hGB, const SNOName & _tSNOName, uint32 _tName, const ActorLocation & _tLocation, const AttribGroupID idFastAttributeGroup, const BOOL fForceInventoryCreate, const BOOL bCollision);
    void InitAttributeSyncMask();
    int32 AttributesGetBaseInt(FastAttribKey) const;
    AttribGroupID AttributesRefreshTimerGroup(AttributeTimerType eTimerType, int32 nDuration);
    void AttributesSavedStateRestore(const ActorSavedAttributes &);
    void AttributesSavedStateStore(ActorSavedAttributes &, StackSerializer &);
    void DebugTraceAttributes();
    void ApplyAllPassives(BOOL bApplyOnlyDualWieldPassives);
    void ApplyPassive(SNO snoPower, BOOL bIgnoreSkillLevel);
    int32 GetRuneUnlockLevel(const SNO snoPower, const RuneType eRuneType) const;
    void SetComboLevel(int32 nComboLevel);
    InventoryLocation & GetInventoryLocation();
    const InventoryLocation & GetInventoryLocation() const;
    const InventoryLocation * TryToGetInventoryLocation() const;
    WorldDistance GetHeight() const;
    Quaternion GetOrientation() const;
    Vector3D GetForward() const;
    Vector2D GetForward2D() const;
    Vector3D GetRight() const;
    InventorySlotType GetLocationSlot() const;
    ACDID GetInventoryContainerOwner() const;
    void ChangeLocation(ActorLocationType nLocation);
    WeaponClass GetWeaponClassForAnim(const ActorDefinition * pdefActor) const;
    void GetSpecificAnimSet(const uint32 dwTag, SNO * psnoAnimSet, uint32 * pdwAnimSetTag) const;
    SNO GetSpecificAnim(const uint32 dwTag) const;
    ActorType GetActorType() const;
    Team GetNaturalTeam() const;
    void SetTeam(Team _eTeam);
    void OverrideTeam(const ACDID);
    void RestoreTeam();
    void GetStartLocation(WorldPlace & tPlace, Quaternion & q);
    BOOL IsInSWorldID(const SWorldID idSWorld) const;
    BOOL IsInInventory() const;
    void ChangeWorld(const WorldPlace & tPlace, const Quaternion & q);
    void SetOwner(const DataID _idOwner);
    BOOL IsAvenger() const;
    EliteClass GetEliteClass() const;
    BOOL IsElite() const;
    void GetEliteModifierData(const GameBalanceDefinition ** ppdefGB, const EliteModifierData ** ppEliteModifierData) const;
    BOOL IsMirrorImage() const;
    BOOL IsMonsterTeam() const;
    BOOL IsLoading() const;
    void SetEnabled(const BOOL bEnable);
    void SetTargetable(const BOOL bTargetable);
    BOOL IsTargetable() const;
    BOOL IsNeverTargetable() const;
    void MarkHeroStateSavedDataChanged();
    void MarkHeroStatePlayerFlagsChanged();
    void MarkHeroStateWaypointsChanged();
    void MarkHeroStateLastSavePointChanged();
    void MarkHeroStateLastWaypointLevelAreaChanged();
    void MarkHeroStateCurrencyChanged();
    void SetGBHandle(const GBHandle _hGB);
    GBID GetVisualGBID() const;
    BOOL HasInventory() const;
    void SetTimeStamp(uint32 dwTime);
    uint32 GetTimeStamp() const;
    void SetHitGroundTime(uint32 dwTime);
    uint32 GetHitGroundTime() const;
    void SetKnownByPlayer(const Player & tPlayer, BOOL bKnown);
    BOOL IsKnownByAnyPlayer(const PlayerList &) const;
    void SetFacing(const WorldPlace & placeDest, BOOL bSnapToFacing);
    void ChangeActor(SNO snoNewActor);
    void ComplexEffectTrack(const DataID idComplexEffect);
    void ComplexEffectUntrack(const DataID idComplexEffect);
    BOOL HasComplexEffects() const;
    void Hide(const BOOL bHidden);
    BOOL IsStealthed() const;
    BOOL IsEnraged() const;
    void SetObserver(const BOOL bObserver);
    void SetInvulnerable(const BOOL bInvulnerable);
    void SetTakesNoDamage(const BOOL bNoDamage);
    void SetCannotBeAddedToAITargetList(const BOOL bUntargetable);
    void SetDoesFakeDamage(const BOOL bFakeDamage);
    void SetCannotDie(const BOOL bCannotDie);
    void SetShear(float);
    BOOL IsInCombat() const;
    BOOL IsInConversation() const;
    BOOL IsBusy() const;
    BOOL IsInKnockback() const;
    void SetCombatStatus(BOOL bInCombat);
    void SetCombatWithPlayerStatus(BOOL bInCombat);
    BOOL HasAttachedACD() const;
    void AttachACD(ActorCommonData & tACDAttachment);
    void DetachACD(ActorCommonData & tACDAttachment);
    ACDID GetAttachedACD() const;
    ACDID GetAttachedToACD() const;
    BOOL ShouldRun() const;
    BOOL CanBeImmobilized() const;
    BOOL IsImmobilized() const;
    BOOL IsFeared() const;
    BOOL IsStunned() const;
    BOOL IsBlind() const;
    BOOL IsPoisoned() const;
    BOOL IsCharmed() const;
    BOOL IsTaunted() const;
    BOOL HasResurrectionPower() const;
    BOOL IsRooted() const;
    ACDID GetRootTarget() const;
    BOOL IsBurrowed() const;
    BOOL IsCCD() const;
    BOOL IsSlowed() const;
    BOOL ModifyCurrencyAmount(const int64 nAmount, const CurrencyType eCurrencyType, const GoldModifiedReason eReason, const BOOL bForceNoSound);
    void SetCurrencyAmount(int64 nAmount, const CurrencyType eCurrencyType, const GoldModifiedReason eReason);
    void SendCurrencyModifiedMessage(const BOOL bPlaySound, const CurrencyType eCurrencyType, const GoldModifiedReason eReason);
    ResourceType GetSecondaryResourceType() const;
    void ResourceAttributeSetInt(Attrib, int32, ResourceType);
    int32 ResourceAttributeGetInt(Attrib, ResourceType) const;
    void ResourceAttributeSetFloat(Attrib eAttrib, float flVal, ResourceType eResourceType);
    float ResourceAttributeGetFloat(Attrib eAttrib, ResourceType eResourceType) const;
    FastAttribKey ResourceAttributeGetKey(Attrib, ResourceType) const;
    int32 GetDisplayedHitPointsCurrent() const;
    float GetHitPointsMaxTotal() const;
    BOOL IsNormalMonster() const;
    BOOL IsUndeadMonster() const;
    BOOL IsLargeMonster() const;
    BOOL DoesMonsterGrantEliteBuff() const;
    void SetSkillKit(const SNO snoSkillKit, BOOL bResetIfSame);
    void GetContactOffset(Vector3D & wpContactOffset, uint32 nContactFrame) const;
    void GetContactOffsetForAnim(const SNO snoAnim, Vector3D & wpContactOffset, const uint32 nContactFrame, const uint32 nPermutation, BOOL bWantWorldPosition) const;
    BOOL SetPrimaryGroup(uint32 uGroupHash, const BOOL fModifyLiveActorCount, const ACDID idACDKiller);
    BOOL SetSecondaryGroup(uint32 uGroupHash, const BOOL fModifyLiveActorCount, const ACDID idACDKiller);
    void InheritGroupsFromACD(const ActorCommonData & tACDParent);
    void RemoveFromGroups(const ACDID idACDKiller);
    void Heal(float flHitPointsToHeal, const BOOL bSendEvent);
    void HealFromResourceSpent(const float flResourceSpent, ResourceType eType);
    void HealAndRestoreAllies();
    void SetHitPointsMax(const float flHitPointsMax);
    BOOL SpendResource(float flAmountToSpend, BOOL bIgnoreMinimumResourceCost, ResourceType eType, Power * ptPower, BOOL bDoHeal);
    void PickupNearby();
    void PickupLine(Vector3D wpStart, Vector3D wpEnd);
    BOOL ResetLifeStats(BOOL bForHealing, BOOL bSendEvent, BOOL bRestoreResource, BOOL bResetComboLevels, BOOL bFromResurrection);
    BOOL RestoreStatus();
    void ResetRestoredPlayerStats();
    BOOL CanAnimate() const;
    BOOL IsObserver() const;
    BOOL IsMerchant() const;
    CurrencyType GetCurrencyType() const;
    BOOL HasVisualInventory() const;
    BOOL IsOperatableNPC(const Player * ptPlayer, BOOL bFromDirectClick) const;
    void GizmoOnDeath(ACDID idACDKiller);
    BOOL IsSpawner() const;
    uint64 GetStackCount() const;
    void ClearStackCount();
    uint64 AddToStackCount(uint64 uAddAmount, uint64 uMax);
    void SubtractFromStackCount(uint64 uSubtractAmount);
    void SetStackCount(uint64 uNewCount);
    void SetExperienceNext(uint64 uValue);
    void SetAltExperienceNext(uint64 uValue);
    void SetRestExperience(uint64 uValue);
    uint64 GetPvPExperienceNext() const;
    void SetPvPExperienceNext(uint64 uValue);
    uint64 PvPGetPvPExperienceGainedThisGame() const;
    void PvPSetPvPExperienceGainedThisGame(uint64 uValue);
    uint64 PvPGetNormalExperienceGainedThisGame() const;
    void PvPSetNormalExperienceGainedThisGame(uint64 uValue);
    uint64 GetExperienceGranted() const;
    void SetExperienceGranted(uint64 uValue);
    void SetItemTradePlayerID(uint64 nPlayerID, int32 nPlayerIndex);
    uint64 GetItemTradePlayerID(int32 nPlayerIndex) const;
    void SetItemSourcePlayerID(uint64 nPlayerID);
    uint64 GetItemSourcePlayerID() const;
    void SetNecroCorpseOwnerPlayerID(uint64 nPlayerID, int32 nPlayerIndex);
    uint64 GetNecroCorpseOwnerPlayerID(int32 nPlayerIndex) const;
    int64 GetSecsSinceLastSpecialItemTime() const;
    void StampLastSpecialItemTime();
    uint64 GetUnlockTime() const;
    void SetUnlockTime(uint64 uNewTime);
    uint64 GetDamageDoneTotalTracked() const;
    void SetDamageDoneTotalTracked(uint64);
    ACDID GetPetCreator() const;
    BOOL CreatedByPlayer() const;
    BOOL IsMyPet(int32 nPlayerIndex) const;
    BOOL CanTargetCorpse(ACDID) const;
    BOOL CanTargetNecroCorpse(ACDID idACDCorpse) const;
    BOOL IsCorpse() const;
    SNO GetMonsterSNO() const;
    BOOL IsModifiedAny() const;
    BOOL IsModifiedSpecific(const ACDModifiedType eModifiedType) const;
    void ModifiedClear();
    void ModifiedClear(const ACDModifiedType eModifiedType);
    void SendSegmentIfModified(const PlayerList & listDestPlayers, const ACDModifiedType eModifiedType, bool bForce);
    void SendServerUpdateMessage(const PlayerList & listDestPlayers);
    void SendAllAttributes(const PlayerList & listDestPlayers);
    void SendVisualInventory(const PlayerList & listDestPlayers);
    void SendAffix(const PlayerList & listDestPlayers, AffixMessageFlag eAffixMessageFlag);
    void SendPortalSpecifier(const PlayerList & listDestPlayers);
    void SendRareMonsterName(const PlayerList & listDestPlayers);
    void SendRareItemName(const PlayerList & listDestPlayers);
    void SendWorldPosition(const PlayerList & listDestPlayers);
    void SendInventoryPosition(const PlayerList & listDestPlayers);
    void SendInventoryActorSNO(const PlayerList & listDestPlayers);
    void SendShear(const PlayerList & listDestPlayers);
    void SendGroupInfo(const PlayerList & listDestPlayers);
    void SendFriendRecipientInfo(const PlayerList & listDestPlayers);
    void SetAssignedHeroID(uint64 idHero);
    void SetAssignedHeroID(const Player * ptPlayer);
    uint64 GetAssignedHeroID() const;
    BOOL HasAssignedHeroID() const;
    BOOL DoesPlayerMatchAssignedHeroID(const Player * ptPlayer) const;
    void ClearAssignedHero();
    void SetAssignedGameConnectionID(GameConnectionID idGameConnection);
    GameConnectionID GetAssignedGameConnectionID() const;
    BOOL GetExpTagPlayer(const Player & tPlayer) const;
    void TagForExp(const ACDID idACDAttacker);
    void SetStoneOfRecallUnlocked(BOOL fUnlocked);
    BOOL AccountIsTradeRestricted() const;
    uint32 GenerateSummonerID();
    const ACDCollSettings & GetCollSettings() const { return tCollSettings; }  // line 996
    void UpdateCollision();
    void UpdateCollision(const WorldLocation & tWorldLoc);
    void EnableCollision(BOOL bEnable);
    void SendCollision(const PlayerList & listDestPlayers);
    float GetWalkingSpeed(uint32 dwAnimTag) const;
    void ChangeDamageState(const int32 nValue);
    int32 IncDamageState();
    BOOL ConvertsDamageToDamageState() const;
    int32 GetDamageStateMax() const;
    void SetBossBar(BOOL bUsingBossBar);
    void AddClientEffect(SNO snoEffect, ParentType eTypeOverride);
    void RemoveClientEffect(SNO snoEffect);
    void RemoveAllClientEffects();
    BOOL HasClientEffects() const;
    SNO GetGizmoCursor(ACDID) const;
    BOOL ShouldUseAltLevels() const;
    void RestorePermanentPowers();
    float GetDPSValue(BOOL bDualWielding) const;
    void GetWeaponDamageAndAttackSpeed(BOOL bDualWielding, float & flMainHandDamage, float & flMainHandSpeed, float & flOffHandDamage, float & flOffHandSpeed) const;
    float GetHealingValue(BOOL bDualWielding, float * pflBaseHealing) const;
    float GetDamageReductionPercent() const;
    float GetEHPValue() const;
    static float GetEHPValue(float flHP, float flDmgReductionPercent);
    BOOL GetHasNFPDevice();
    void SetHasNFPDevice(BOOL bHasNFPDevice);
    ACDID GetNFPPortalOwnerACDID();
    void SetNFPPortalOwnerACDID(ACDID);
    // ActorCommonData.cpp (callers: X/Server/LuaScript/ScriptUtil.cpp)
    ACDID id;  // 0x00
    uint32 tName;  // 0x04
    int32 nRefCount;  // 0x08
    uint32 dwFlags;  // 0x0c
    ACDNetworkName ann;  // 0x10
    DataID idOwner;  // 0x14
    SNO snoActor;  // 0x18
    SNOName tSNOName;  // 0x1c
    uint32 dwKnownBits;  // 0x24
    uint32 dwPlayerSeenBits;  // 0x28
    uint32 dwPlayerLastSeenTime;  // 0x2c
    uint32 dwHitGroundTime;  // 0x30
    uint32 dwLastSeen;  // 0x34
    GBHandle hGB;  // 0x38
    MonsterRarity eMonsterRarity;  // 0x40
    ActorLocation tLocation;  // 0x48
    AttribGroupID idFastAttrib;  // 0xb8
    AttribGroupID idPassiveAttribGroup;  // 0xbc
    AttribGroupID idEnhancementAttribGroup;  // 0xc0
    AttribGroupID idUpscaledAttribGroup;  // 0xc4
    ActorInventory * ptInventory;  // 0xc8
    VisualInventory * ptVisualInventory;  // 0xd0
    uint64 idAssignedHero;  // 0xd8 (OnlineService::HeroId)
    GameConnectionID idAssignedGameConnection;  // 0xe0
    GBIDPooledList listIdentifiedAffixGBIDs;  // 0xe8
    GBIDPooledList listUnidentifiedAffixGBIDs;  // 0x110
    RareItemName tRareItemName;  // 0x138; original declaration at ActorCommonData.h:1188
    GizmoType eGizmoType;  // 0x148
    ActorType eActorType;  // 0x14c
    float flHitPointsCurrent;  // 0x150
    Team eTeamOverride;  // 0x154
    Team eTeam;  // 0x158
    HirelingClass eHirelingClass;  // 0x15c
    bool fNeverTargetable : 1;  // 0x160
    bool fIsUntargetable : 1;  // 0x160
    bool fIsStealthed : 1;  // 0x160
    int32 nPetOwner : 3;  // 0x160
    bool fIsNPC : 1;  // 0x160
    bool fIsDisabled : 1;  // 0x160
    bool fIsAlive : 1;  // 0x161
    bool fIsCharmed : 1;  // 0x161
    bool fIsStunned : 1;  // 0x161
    bool fHasAssignedHeroID : 1;  // 0x161
    bool fHasSpawner : 1;  // 0x161
    FastAttribGroup * ptFastAttribGroup;  // 0x168
    GBID pRareNameGBIDs[2];  // 0x170
    GBID pMonsterAffixGBIDs[8];  // 0x178
    RNG * ptRNG;  // 0x198
    uint32 dwModifiedBits;  // 0x1a0
    TKnownMessageList listACDMessages;  // 0x1a8
    ComplexEffectIDListPooled tListComplexEffects;  // 0x1d0
    BuffIDListPooled listBuffs;  // 0x1f8
    AnimPlayer * ptAnimPlayer;  // 0x220
    ResolvedPortalDestination * ptResolvedDest;  // 0x228
    AvengerData * ptAvengerData;  // 0x230
    FriendGiftData * ptFriendGiftData;  // 0x238
    uint32 dwTimerCount;  // 0x240
    ACDPackID idACDPack;  // 0x244
    float flShear;  // 0x248
    uint8 nLookLinkIndex;  // 0x24c
    uint32 * posGroupPrimary;  // 0x250 (POS)
    uint32 uGroupPrimary;  // 0x258
    uint32 * posGroupSecondary;  // 0x260 (POS)
    uint32 uGroupSecondary;  // 0x268
    uint8 nScriptState;  // 0x26c
    uint8 uAttributeSyncMask;  // 0x26d
    ACDCollSettings tCollSettings;  // 0x270
    XSphereNode * ptCollSphereNode;  // 0x2a8
    XSphereNode * ptCollBlockerSphereNode;  // 0x2b0
    SWorldID idCollSWorld;  // 0x2b8
    uint32 uHeroTagBitfield;  // 0x2bc
    XGrowableMap<int, int, int, int> tClientEffects;  // 0x2c0
private:
    SNO WhichAttackIsBetter(SNO snoAttackPower0, SNO snoAttackPower1) const;  // ActorAttributes.cpp:187
    ResourceType sResolveResourceType(Attrib eAttrib, ResourceType eResourceType) const;
    uint64 sGetAttrib64(Attrib eAttribHi, Attrib eAttribLo, int nParam) const;
    void sSetAttrib64(Attrib eAttribHi, Attrib eAttribLo, int nParam, uint64 uValue);
    PickupResponse AutoPickup(const ActorCommonData & tACDItem);
};
static_assert(sizeof(ActorCommonData) == 0x340, "ActorCommonData");
static_assert(__builtin_offsetof(ActorCommonData, idFastAttrib) == 0xb8, "idFastAttrib");
static_assert(__builtin_offsetof(ActorCommonData, listIdentifiedAffixGBIDs) == 0xe8, "listIdentifiedAffixGBIDs");
static_assert(__builtin_offsetof(ActorCommonData, listUnidentifiedAffixGBIDs) == 0x110, "listUnidentifiedAffixGBIDs");
static_assert(__builtin_offsetof(ActorCommonData, pMonsterAffixGBIDs) == 0x178, "pMonsterAffixGBIDs");
static_assert(__builtin_offsetof(ActorCommonData, uGroupPrimary) == 0x258, "uGroupPrimary");
static_assert(__builtin_offsetof(ActorCommonData, uGroupSecondary) == 0x268, "uGroupSecondary");
static_assert(__builtin_offsetof(ActorCommonData, flHitPointsCurrent) == 0x150, "flHitPointsCurrent");
static_assert(__builtin_offsetof(ActorCommonData, eTeamOverride) == 0x154, "eTeamOverride");
static_assert(__builtin_offsetof(ActorCommonData, eTeam) == 0x158, "eTeam");
static_assert(__builtin_offsetof(ActorCommonData, eHirelingClass) == 0x15c, "eHirelingClass");
static_assert(__builtin_offsetof(ActorCommonData, ptFastAttribGroup) == 0x168, "ptFastAttribGroup");
static_assert(__builtin_offsetof(ActorCommonData, nScriptState) == 0x26c, "nScriptState");
static_assert(__builtin_offsetof(ActorCommonData, uAttributeSyncMask) == 0x26d, "uAttributeSyncMask");

// Declared here from its definition in ActorCommonData.cpp.
BOOL ACDIsFriendly(const ActorCommonData & tACD1, const ActorCommonData & tACD2,
                   BOOL bCharmedAlliesAreEnemies);  // ActorCommonData.cpp:1906

// Original ActorCommonData.h:1331/1398 and ActorCommonData.cpp:2029/2290/4475.
enum ACDIterateResult {
    ACD_ITERATE_STOP = 0,
    ACD_ITERATE_CONTINUE = 1,
};
typedef ACDIterateResult (*ACDIterateRadiusFunc)(const WorldDistance, const ACDID, const ACDID, void *);
BOOL ACDIsHostile(const ActorCommonData & tACD1, const ActorCommonData & tACD2,
                  BOOL bCharmedAlliesAreEnemies);
WorldDistance ACDGetDistanceSquared(const ActorCommonData & tACDFrom, const ActorCommonData & tACDTo,
                                   const BOOL bWantAs2D);
void ACDIterateRadius(const WorldDistance & wdRadius, const ACDID idACDSource,
                      ACDIterateRadiusFunc pfnIterateRadius, void * pData);

BOOL ACDIsHireling(const ActorCommonData & tACD);  // ActorCommonData.cpp:8191
BOOL ACDIsProjectile(ACDID idACD);  // ActorCommonData.cpp
HirelingClass ACDGetHirelingClass(const ActorCommonData & tACD);  // ActorCommonData.cpp:8250
BOOL ACDIsParticipatingInSetDungeon(const ActorCommonData & tACD);  // ActorCommonData.cpp:9323
BOOL ACDIsParticipatingInTieredLootRun(const ActorCommonData & tACD);  // ActorCommonData.cpp:9315
SNO ACDGetLevelArea(const ActorCommonData & tACD);  // ActorCommonData.cpp:4421

ACDID ACDFromANN(const ACDNetworkName ann);  // ActorCommonData.cpp:2119
void ACDDelete(const ACDID idACD, BOOL bWorldDeleted);  // ActorCommonData.cpp:996

typedef TDataArrayIterator<ActorCommonData> ACDIterator;  // ActorCommonData.h:647
ActorCommonData * ActorCommonDataIterate(ACDIterator & tIter);  // ActorCommonData.cpp

typedef XPooledList<ACDID, ACDID> ACDIDGroupListPooled;  // ActorCommonData.h:699
typedef XListMemoryPool<ACDID> ACDIDMemoryPool;  // ActorCommonData.h:701

struct ACDGroup {  // size 0x10
    uint32 nCount;  // 0x00
    ACDIDGroupListPooled * ptList;  // 0x08
};
static_assert(sizeof(ACDGroup) == 0x10, "ACDGroup");

typedef XGrowableMap<unsigned int, unsigned int, ACDGroup, ACDGroup &> ACDGroupMap;

struct ACDGroups {  // size 0xe0
    ACDGroupMap tMapACDGroups;  // 0x00
    ACDIDMemoryPool poolACDGroups;  // 0x80
    ChainedFixedBlockAllocator tAllocLists;  // 0xb0
};
static_assert(sizeof(ACDGroups) == 0xe0, "ACDGroups");

typedef XListMemoryPool<KnownMessage> TKnownMessageListPool;

struct ACDGlobals {  // size 0x180
    TChainedDataArray<ActorCommonData, ACDID> * ptACDs;  // 0x00
    ACDID * patANNToACDLookup;  // 0x08
    TChainedDataArray<ACDPack, int> * ptACDPacks;  // 0x10
    XPooledDataIDList listDirtyACDs;  // 0x18
    TKnownMessageListPool poolQueuedACDMessageLists;  // 0x40
    GBIDPool poolGBIDs;  // 0x70
    ACDGroups tACDGroups;  // 0xa0
};
static_assert(sizeof(ACDGlobals) == 0x180, "ACDGlobals");

// The body is not in the DWARF; written from the copy inlined into X/Server/Powers/Power_Monk.cpp
// (sMonk_AddSpiritOnHit).
inline BOOL ActorCommonData::IsPet() const {  // line 900
    return GetPetOwnerPlayer() != NULL;
}

inline ActorCommonData * ACDTryToGet(const ACDID idACD) {  // line 1445
    if (idACD == ACDID_INVALID) {
        return NULL;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptACDGlobals == NULL) {
        return NULL;
    }

    return tGCD.ptACDGlobals->ptACDs->ElementTryToGet(idACD);
}

// The body is not in the DWARF; written from the copy inlined into X/Client/CGame.cpp
// (sValidatePlayerOnGoodGround).
inline ActorCommonData & ACDGet(const ACDID idACD) {  // line 1461
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return *tGCD.ptACDGlobals->ptACDs->ElementGet(idACD);
}

inline DataID ACDGetOwner(const ACDID idACD) {  // line 1492
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL) {
        return DATAID_INVALID;
    }
    return ptACD->idOwner;
}


// Functions defined in this file (bodies are not in the DWARF):
//   line  1445  ActorCommonData * ACDTryToGet(const ACDID idACD)  [0x71005c3ba0, inlined into 2500 functions]
//   line  1461  ActorCommonData & ACDGet(const ACDID idACD)  [inlined into 998 functions]
//   line  1492  DataID ACDGetOwner(const ACDID idACD)  [inlined into 393 functions]

// ActorCommonData.cpp (X/Common/Game.cpp GameGetCustomCrashDescription).
void ACDGetNavMeshHandle(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle);
void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace);  // ActorCommonData.cpp:4668
BOOL ACDAffectedByHealthGlobes(const ActorCommonData & tACD);  // ActorCommonData.cpp:7167

ACDID ACDGetControllingActor(ACDID idACDPet);  // ActorCommonData.cpp:1690
ACDNetworkName ANNFromACD(const ACDID idACD);  // ActorCommonData.cpp
ACDID ACDGetPetOwner(ACDID idACDPet);  // ActorCommonData.cpp:1658

static_assert(__builtin_offsetof(ActorCommonData, ptFriendGiftData) == 0x238, "ptFriendGiftData");

static_assert(__builtin_offsetof(ActorCommonData, listBuffs) == 0x1f8, "ActorCommonData::listBuffs");

// ActorCommonData.cpp:4689.
SSceneID ACDGetCurrentSSceneID(const ActorCommonData & tACD);

struct ACDPack {  // size 0x38, line 1338
    const ACDIDListPooled * GetMemberList();
    void RemoveMember(ActorCommonData & tACDMember);
    void AddMember(ActorCommonData & tACDMember);
    ACDID GetLeader();
    void AddDeactivated() { ++nDeactivatedCount; }
    void RemoveDeactivated();
    int32 GetTotalACDCount(BOOL bIncludeDyingACDs);
    ACDPackID id;  // 0x00
    ACDIDListPooled listACDs;  // 0x08
    int32 nDeactivatedCount;  // 0x30
    BOOL bBroadcastSent;  // 0x34
};
static_assert(sizeof(ACDPack) == 0x38, "ACDPack");

// Items.cpp. Declared for X/Common/ActorCommonData.cpp.
int64 ItemPlayerGetCurrencyCount(const ActorCommonData & tACD, CurrencyType eCurrencyType);
BOOL ItemPlayerModifyCurrency(ActorCommonData & tACD, CurrencyType eCurrency, int64 nCount);  // Items.cpp:7951

// ActorCommonData.cpp:2071,2109.
void ActorLocationInit(ActorLocation & tLocation, const WorldPlace & tPlace, const Quaternion & q, const float flScale);
void ActorLocationSetScale(ActorLocation & tLocation, const float flScale);
BOOL ItemPlayerSetCurrency(ActorCommonData & tACD, CurrencyType eCurrency, int64 nAmount);  // Items.cpp:8004

struct ACDCollFlagsMessage {  // size 0x10
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    uint32 uCollFlags;  // 0x0c
};
static_assert(sizeof(ACDCollFlagsMessage) == 0x10, "ACDCollFlagsMessage");

struct CurrencyModifiedMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    BOOL bPlaySound;  // 0x08
    CurrencyType eCurrencyType;  // 0x0c
    GoldModifiedReason eReason;  // 0x10
};
static_assert(sizeof(CurrencyModifiedMessage) == 0x14, "CurrencyModifiedMessage");

struct RareMonsterNamesMessage {  // size 0x34
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    GBID pgbidRareNames[2];  // 0x0c
    GBID pgbidMonsterAffixes[8];  // 0x14
};
static_assert(sizeof(RareMonsterNamesMessage) == 0x34, "RareMonsterNamesMessage");

typedef int32 LevelAreaID;  // X/Common/CommonTypes.h
LevelAreaID ACDGetLevelAreaID(const ActorCommonData & tACD);  // ActorCommonData.cpp
BOOL ACDIsInTown(const ActorCommonData & tACD);  // ActorCommonData.cpp:4432
int32 ACDCountNearbySummonedUnits(const ActorCommonData & tACD, const float & flRadius);  // ActorCommonData.cpp
void ACDGetNearbySummonedUnits(const ActorCommonData & tACDSummoner, const WorldDistance & wdRadius, XPooledDataIDList & list);  // ActorCommonData.cpp:4847
ACDID ACDGetSummoner(ACDID idACD);  // ActorCommonData.cpp
ACDPack * ACDPackTryToGet(ACDPackID idACDPack);  // ActorCommonData.cpp
BOOL ACDIsInLootRun(const ActorCommonData & tACD);  // ActorCommonData.cpp
BOOL ACDIsInTieredLootRun(const ActorCommonData & tACD);  // ActorCommonData.cpp

BOOL ACDIsHostile(Team eTeam1, Team eTeam2);

// ActorCommonData.cpp:6350; declaration placed in the owning API header.
ACDIDArray ACDGetNecroCorpsesForPlayerAroundPosition(const Player * ptPlayer, Vector3D wpPos, SNO snoPower);

static_assert(__builtin_offsetof(ActorCommonData, tRareItemName) == 0x138, "ActorCommonData::tRareItemName");

#line 1303
struct RareItemNameMessage {

    RequiredMessageHeader tHeader;
    ACDNetworkName ann;
    RareItemName tRareItemName;
};
static_assert(sizeof(RareItemNameMessage) == 0x1c, "RareItemNameMessage");
static_assert(__builtin_offsetof(RareItemNameMessage, tHeader) == 0x0, "RareItemNameMessage::tHeader");
static_assert(__builtin_offsetof(RareItemNameMessage, ann) == 0x8, "RareItemNameMessage::ann");
static_assert(__builtin_offsetof(RareItemNameMessage, tRareItemName) == 0xc, "RareItemNameMessage::tRareItemName");
