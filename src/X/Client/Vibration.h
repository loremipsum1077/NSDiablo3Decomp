// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Vibration.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "AttachmentParent.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/Math/Transform.h"

enum {
    VIBRATIONID_INVALID = -1,
};

typedef int32 VibrationID;

namespace Vibration {
struct VibrationInfo {  // size 0x74
    VibrationID id;  // 0x00
    SNO snoVibration;  // 0x04
    float flTime;  // 0x08
    float flOutroTime;  // 0x0c
    int32 nPlayerIndex;  // 0x10
    int32 nControllerIndex;  // 0x14
    BOOL bOutro;  // 0x18
    BOOL bDeleted;  // 0x1c
    float flOutroCrossFade;  // 0x20
    float flMainVibrationInterp;  // 0x24
    float flOutroVibrationInterp;  // 0x28
    PRTransform transform;  // 0x2c
    ParentData tParentData;  // 0x48
    void Update();
};
static_assert(sizeof(VibrationInfo) == 0x74, "VibrationInfo");

void InitializeForGame();
void DisposeForGame();
void DeleteAll();
void DeleteAllForController(int32 nControllerIndex);
VibrationInfo * Get(VibrationID id);
VibrationInfo * TryToGet(VibrationID id);
BOOL Exists(VibrationID idVibration);
void SetParent(const VibrationID idVibration, const ParentData & tParentData);
VibrationID Add(SNO snoVibration, const PRTransform & transform, int32 nPlayerIndex);
void UpdatePropertiesFromParent(VibrationInfo * ptVibration);
void SetPosition(VibrationID idVibration, const Vector3D & wp);
void Delete(VibrationID idVibration);
void PlayOutro(VibrationID idVibration);
void Update();
void FlagForDelete(VibrationID idVibration);
}  // namespace Vibration
