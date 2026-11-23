// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UIInit.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Sigma/Unicode.h"

struct UIID;
class UIControl;
class UIWindow;
struct UIControlSourceKey;
struct Vector2D;

typedef int32 UIType;

struct DynamicControlCreationParams {  // size 0x10
    DynamicControlCreationParams(LPCSTR szNewControlName, const UIID & idDlgParent)  // line 29
        : m_szNewControlName(szNewControlName), m_idDlgParent(idDlgParent) {}
    LPCSTR m_szNewControlName;  // 0x00
    const UIID & m_idDlgParent;  // 0x08
};
static_assert(sizeof(DynamicControlCreationParams) == 0x10, "DynamicControlCreationParams");

void UIInit_CreateRoot();
UIControl * UI_CreateControlFromTemplate(const UIControlSourceKey & tSrcKey, const DynamicControlCreationParams & tCreationParams, const Vector2D * pvecTLNewControl);
UIWindow * UI_CreateDlg(const UIID & idDlgParent, const SNO snoUI);
