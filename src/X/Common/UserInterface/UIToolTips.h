// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/UserInterface/UIToolTips.h
// Subset: the tag-resolution callback type and UIToolTips_ProcessTip.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Attributes/FastAttribute.h"
#include "../Items.h"
#include "../SNO/SNOFiles.h"
#include "FontDefinition.h"

class ActorCommonData;
class CRefString;

enum {
    TTVC_NO_COLORING = 0,
    TTVC_NORMAL_COLORING = 1,
    TTVC_ENCHANT_RANGE_COLORING = 2,
};

typedef BOOL (*TagResolutionCallback)(const ActorCommonData *, AttribGroupID, AttribGroupID, LPCSTR, float &, float *, CRefString &, SNO, int32, void *, void *, FontSpecifier &);

// X/Common/UserInterface/UIToolTips.cpp:203
CRefString UIToolTips_ProcessTip(const ActorCommonData * ptACDItem, const AttribGroupID idFastAttribMin, const AttribGroupID idFastAttribMax, const SNO snoPower, const int32 nLevel, LPCSTR szTip, const ToolTipValueColoring eColoringType, TagResolutionCallback pTagCallback, void * pTagData0, void * pTagData1);
