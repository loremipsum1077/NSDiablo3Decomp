// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Hireling.h
// Subset: the hireling data types and the functions Hireling.cpp defines. The messages
// (HirelingInfoUpdateMessage, HirelingSwapMessage) are not adopted yet.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "Definitions/HeroDefinition.h"

class ActorCommonData;
struct SkillKitDefinition;

enum {
    HIRELING_TRAIN_OK = 0,
    HIRELING_TRAIN_ALREADY_PURCHASED = 1,
    HIRELING_TRAIN_CONFLICTING_SKILL_PURCHASED = 2,
    HIRELING_TRAIN_LEVEL_REQUIREMENT_NOT_MET = 3,
    HIRELING_TRAIN_INVALID_SKILL = 4,
    HIRELING_TRAIN_COUNT = 5,
};

typedef int32 HirelingTrainReason;

struct HirelingItemStore {  // size 0x18
    ACDID idACDItems[6];  // 0x00
};
static_assert(sizeof(HirelingItemStore) == 0x18, "HirelingItemStore");

struct HirelingData {  // size 0x38
    HirelingInfo tInfo;  // 0x00
    HirelingItemStore tItems;  // 0x20
};
static_assert(sizeof(HirelingData) == 0x38, "HirelingData");

struct HirelingStable {  // size 0xe8
    HirelingData tHireling[4];  // 0x00
    HirelingClass eActiveHireling;  // 0xe0
    HirelingClass eTypeToCreate;  // 0xe4
};
static_assert(sizeof(HirelingStable) == 0xe8, "HirelingStable");

void HirelingDataInit(HirelingData & tData);
void HirelingDataDispose(HirelingData & tData);
void HirelingSavedDataSetDefaults(HirelingSavedData & tSavedData);
void HirelingStableInit(HirelingStable & tStable);
void HirelingStableDispose(HirelingStable & tStable);
GBID HirelingGetGBID(HirelingClass eClass);
HirelingClass HirelingGetClass(const ActorCommonData & tACDHireling);
int32 HirelingGetUnlockLevelBase(const SkillKitDefinition * pdefSkillKit);
BOOL HirelingTestValid(ACDID idACDPlayer, HirelingClass eClass);
void HirelingSetTypeToCreate(ACDID idACDPlayer, HirelingClass eClass);
HirelingClass HirelingGetTypeToCreate(ACDID idACDPlayer);
void HirelingSetTypeAvailable(ACDID idACDPlayer, HirelingClass eClass, BOOL fAvailable);
BOOL HirelingTestTypeAvailable(ACDID idACDPlayer, HirelingClass eClass);
HirelingTrainReason HirelingCanTrainSkillReason(const ActorCommonData & tACDHireling, const SNO snoPower);
BOOL HirelingHasUnspentSkillPoints(const ActorCommonData & tACDHireling);
BOOL HirelingDoesLevelGrantNewSkillRow(const ActorCommonData & tACDHireling, int32 nLevel);
SNO HirelingGetPower(const ActorCommonData & tACDHireling, const int32 nRow, const int32 nColumn);
BOOL HirelingCanRetrain(ActorCommonData & tACDHireling, ActorCommonData & tACDPlayer);
BOOL HirelingPowerGetRowColumn(const ActorCommonData & tACDHireling, SNO snoPower, int32 & nRow, int32 & nColumn);

struct HirelingInfoUpdateMessage {  // size 0x48
    RequiredMessageHeader tHeader;  // 0x00
    int32 eClass;  // 0x08
    BOOL fDead;  // 0x0c
    GBID gbidName;  // 0x10
    int32 eValidClass;  // 0x14
    int32 arnPowerKeyParam[5];  // 0x18
    ACDNetworkName annItems[6];  // 0x2c
    int32 nPlayerIndex;  // 0x44
};
static_assert(sizeof(HirelingInfoUpdateMessage) == 0x48, "HirelingInfoUpdateMessage");
