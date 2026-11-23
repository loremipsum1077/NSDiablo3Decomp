#include "UIDrawHookControl.h"

#include <strings.h>

#include "../../Definitions/UIDialogDefinition.h"
#include "../UIAttributes.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
namespace UIHireling { void Draw(const uint8 bLayerAlpha, float & flZSlice); }  // X/Client/UserInterface/Interface/UIHireling.h
namespace UIGame { void Draw(const uint8 bLayerAlpha, float & flZSlice); }  // X/Client/UserInterface/Interface/UIGame.h
namespace UIStash { void Draw(const uint8 bLayerAlpha, float & flZSlice); }  // X/Client/UserInterface/Interface/UIStash.h
namespace Console { void TouchPadDebugDraw(const uint8 bLayerAlpha, float & flZSlice); }  // X/Client/Debug/ControllerDebugView.h
namespace UITransmute { void DrawGrid(const uint8 bLayerAlpha, float & flZSlice); }  // X/Client/UserInterface/Console/UIConsoleTransmute.h

typedef void (*UIDrawFunction)(const uint8, float &);

struct UIDrawFunctionTable {  // size 0x10
    LPCSTR szFunction;  // 0x00
    UIDrawFunction pfnFunction;  // 0x08
};
static_assert(sizeof(UIDrawFunctionTable) == 0x10, "UIDrawFunctionTable");

static const UIDrawFunctionTable sgtUIDrawFunctions[6] =
{
    { "UIHireling_Draw", UIHireling::Draw },
    { "UIGameBase_Draw", UIGame::Draw },
    { "UIStash_Draw", UIStash::Draw },
    { "UIConsole_TouchPadDebugDraw", Console::TouchPadDebugDraw },
    { "UIConsoleTransmute_DrawGrid", UITransmute::DrawGrid },
    { NULL, NULL },
};

static int32 sLookupUIDrawFunction(LPCSTR szFunction)
{
    for (int32 i = 0; i < 6; ++i)
    {
        if (sgtUIDrawFunctions[i].szFunction == NULL)
            break;
        if (strcasecmp(szFunction, sgtUIDrawFunctions[i].szFunction) == 0)
            return i;
    }
    return -1;
}

DrawHookCreationParams::DrawHookCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : ControlCreationParams(eType, id, idParent)
{
    m_nDrawHook = -1;
}

void DrawHookCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    ControlCreationParams::Load(snoUI, pControlData);
    m_nDrawHook = sLookupUIDrawFunction(pControlData->GetAttributeValue(ATT_DRAWHOOK));
}

UIDrawHook::UIDrawHook(const DrawHookCreationParams & tParams)
    : UIControl(tParams)
{
    m_nDrawHook = tParams.m_nDrawHook;
}

void UIDrawHook::Draw(const uint8 bLayerAlpha, float & flZSlice)
{
    if (m_nDrawHook != -1)
        sgtUIDrawFunctions[m_nDrawHook].pfnFunction(bLayerAlpha, flZSlice);
}
