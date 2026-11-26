// Dark Alchemy console screen in 2.7.7.
#include <float.h>
#include "../../Gfx/GfxVB.h"  // Per-unit vertex normals initialized by the original startup routine.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/GlobalSNO.h"
#include "../../Sound/Sound.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "../../../Common/Items.h"
#include "../../../Common/Networking/GameMessage.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UITextControl.h"

#include "../../Console/ControllerUtils.h"
#include "../../Input/Input.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "ConsoleModalNotificationManager.h"
#include "../UIInit.h"
#include "../Controls/UIStackPanelControl.h"
#include "../../ImageCollection.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"

extern "C" BOOL sub_71004b8b80(const ActorCommonData &, const ActorCommonData &);
extern "C" int32 sub_71004b8400(const ActorCommonData &);
extern "C" void sub_710023e7f0();
extern UIID unk_7101861530, unk_7101861538, unk_7101861540;
extern "C" BOOL sub_71004b7300(const ActorCommonData &);
extern "C" int32 sub_71004b83e0(uint32);
extern "C" SNO sub_71004b8b50(uint32);
extern "C" void sub_71004b8c30(uint32, blz::vector<uint32> &);
extern "C" BOOL sub_710023dcb0(uint32, uint32, UIID &, UIID &, UIID &);
extern "C" CRefString sub_710023e8c0(ActorCommonData &, SNO);
extern "C" CRefString sub_71003f4ca0(ActorCommonData &, SNO, int32);
namespace UISkills { BOOL GetName(SNO, CRefString &); }
UIStackPanel & UI_GetStackPanelFromID(const UIID &);
void ACDInventoryGetItemDrawInfoWithGBID(GBID, ImageTextureFrame &, int32);
GBID CurrencyGetGBID(CurrencyType);
BOOL ItemIsRandomItemPlaceholder(GBID);

extern UIID unk_7101862570, unk_7101862578, unk_7101861520, unk_7101861528;
extern "C" uint32 sub_710023edc0(uint32 index);
extern "C" size_t sub_710023edd0(uint32 node);
extern "C" UIID & sub_710023e8a0(uint32 index);
extern "C" void sub_710023ee30(BOOL notify);
extern "C" BOOL sub_71004b81d0(uint32 index);
extern "C" BOOL sub_71004b81e0(const ActorCommonData &, uint32 index);
extern "C" BOOL sub_71004b8230(const ActorCommonData &, uint32 index);
extern "C" int32 sub_71004b8330(const ActorCommonData &);
extern "C" GameError sub_71004b8530(const ActorCommonData &, uint32 node, const ActorCommonData * item);
extern "C" void sub_710016a990(int32);
namespace Console { namespace Screens { BOOL OpenToScreen(int32, int32, int32, BOOL); } }
BOOL PlayerCanChangeSkillorTrait(const ActorCommonData &, BOOL);
void MessageSendToServer(GameMessageType, void *, uint32, int32);
namespace UIGame { void DisplayError(const GameErrorData &, BOOL, const Player *); }

struct Unk17ad70
{
    Vector2D unk_00;
    uint32 unk_08;
    blz::string unk_10, unk_38, unk_60;
    UIID unk_88, unk_90;
};
static_assert(sizeof(Unk17ad70) == 0x98, "Dark Alchemy node entry");
static blz::vector<Unk17ad70> unk_71017e55b8;

static int32 unk_71017e5534;
static ACDID unk_71017e5538 = ACDID_INVALID;
static ACDID unk_71017e553c = ACDID_INVALID;
static uint32 unk_71017e55d8;
static BOOL unk_71017e55dc;

void GameSoftPause(BOOL bPause);

extern UIID unk_7101862568;  // UIGlobals.cpp: "Root.NormalLayer.DarkAlchemyUI_main"

// Unit globals (2.7.7). 0x71017e55d0 is set to -1 by the unit's static initializer 0x710017b020.
static int32 unk_71017e55d0 = -1;  // 0x71017e55d0: controller index the screen was opened for
static int32 unk_71017e55d4;  // 0x71017e55d4: selected node
static BOOL unk_71017e55e0;  // 0x71017e55e0: keep the node selection when closing
static ACDID unk_710109d4f8 = ACDID_INVALID;  // 0x710109d4f8 (.data)

extern "C" void sub_7100179f60();

// Probably: create a sacrifice cost row in the altar's stack panel.
// NON_MATCHING: The template key, inventory text and string growth use a different register and stack layout.
extern "C" void sub_710017a700(const Unk4b97c0 & cost, int32 index, UIStackPanel & panel)
{
    static blz::string unk_71017e5590("ConsoleDarkAlchemy:DarkAlchemyUI_main.LayoutRoot.TemplateContainer");
    UIControlSourceKey source = UIControlSourceKey::Parse(unk_71017e5590.c_str(), SNO_NONE, NULL, NULL);
    UIControl * control = UI_CreateControlFromTemplate(source,
        DynamicControlCreationParams(CRefString().Format("CostItem%d", index), unk_7101861530), NULL);
    control->SetActive(TRUE);
    UIID id = control->GetID();
    panel.AddChild(id);
    GBID item = cost.unk_28;
    if (!cost.unk_00.empty())
    {
        const Unk4b8080 & first = cost.unk_00[0];
        GBID candidate = INVALID_GBID;
        if (first.unk_00 == 0)
            candidate = CurrencyGetGBID(first.unk_04);
        else if (first.unk_00 == 1)
            candidate = first.unk_08;
        if (item == INVALID_GBID)
            item = candidate;
    }
    UIWindow & icon = UI_GetWindowFromID(UIIDCreateFromParent(id, ".ItemWrapper.ItemIcon"));
    icon.SetActive(TRUE);
    ImageTextureFrame frame;
    ACDInventoryGetItemDrawInfoWithGBID(item, frame, 0);
    icon.SetTextureFrame(frame);
    BOOL sufficient = cost.unk_20 >= cost.unk_18;
    icon.SetShader(GlobalSNOGet(sufficient ? GLOBALSNO_SHADER_SPRITE_TRANSPARENT : GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
    UI_GetWindowFromID(UIIDCreateFromParent(id, ".ItemWrapper.ItemBullet")).SetActive(FALSE);
    UI_GetWindowFromID(UIIDCreateFromParent(id, ".ItemWrapper.ItemUnavailable")).SetActive(!sufficient);
    BOOL random = ItemIsRandomItemPlaceholder(item);
    UI_GetWindowFromID(UIIDCreateFromParent(id, ".ItemWrapper.ItemQuestionMark")).SetActive(random);
    UIText & description = UI_GetTextFromID(UIIDCreateFromParent(id, ".CostDescription"));
    blz::string text(ItemGetDisplayName(item).str());
    text += " x";
    text += blz::to_string(cost.unk_18);
    description.SetText(text.c_str(), FALSE);
}

// Probably: refresh sacrifice requirements and the lines between altar nodes.
// NON_MATCHING: The two UIID insertions inline blz vector growth. The target calls the shared push_back implementation.
extern "C" void sub_7100178050()
{
    uint32 selected = sub_710023edc0(unk_71017e55d4);
    const ActorCommonData & actor = ACDGet(PlayerGetByControllerIndex(unk_71017e55d0)->idPlayerACD);
    BOOL complete = sub_71004b7300(actor);
    UI_GetWindowFromID(unk_7101861540).SetActive(complete);
    BOOL finalSeal = sub_71004b8230(actor, 29);
    UI_GetWindowFromID(unk_7101861530).SetActive(!finalSeal);
    UI_GetTextFromID(unk_7101861538).SetText(StringListFetch(GlobalSNOGet(GLOBALSNO_UNK_912),
        finalSeal ? "NextOffering_FinalSeal" : "ConsoleCostTitle"), FALSE);

    Unk1784d0 costs;
    sub_71004b7340(actor, sub_71004b83e0(selected), costs);
    UIStackPanel & panel = UI_GetStackPanelFromID(unk_7101861530);
    panel.DeleteAllChildren();
    for (uint32 i = 0; i < costs.unk_18.size(); ++i)
        sub_710017a700(costs.unk_18[i], i, panel);
    for (uint32 i = 0; i < costs.unk_00.size(); ++i)
        sub_710017a700(costs.unk_00[i], costs.unk_18.size() + i, panel);

    static blz::vector<UIID> unk_71017e5578;
    unk_71017e5578.clear();
    blz::vector<uint32> prerequisites;
    for (uint32 node = 0; node < 29; ++node)
    {
        uint32 position = sub_710023edd0(node) + 1;
        prerequisites.clear();
        sub_71004b8c30(node, prerequisites);
        for (uint32 other : prerequisites)
        {
            UIID base, lit, enabled;
            if (!sub_710023dcb0(position, sub_710023edd0(other) + 1, base, lit, enabled))
                continue;
            UIWindow & window = UI_GetWindowFromID(lit);
            BOOL both = sub_71004b81e0(actor, node) && sub_71004b81e0(actor, other);
            BOOL available = !both && sub_71004b81e0(actor, other);
            auto setLine = [&](UIWindow & control, const UIID & id, BOOL show) {
                BOOL active = blz::find(unk_71017e5578.begin(), unk_71017e5578.end(), id) != unk_71017e5578.end();
                if (!active && show)
                {
                    unk_71017e5578.push_back(id);
                    active = TRUE;
                }
                control.SetActive(active);
            };
            setLine(window, lit, both);
            setLine(UI_GetWindowFromID(enabled), enabled, available);
        }
    }
}

// Probably: refresh the localized title and tooltip for the selected altar node.
// NON_MATCHING: The tooltip string temporaries and the selected node address use a different register and stack layout.
extern "C" void sub_71001785f0()
{
    const Unk17ad70 & entry = unk_71017e55b8[unk_71017e55d4];
    ActorCommonData * actor = ACDTryToGet(PlayerGetByControllerIndex(unk_71017e55d0)->idPlayerACD);
    CRefString name, description, stats;
    uint32 node = sub_710023edc0(unk_71017e55d4);
    SNO power = sub_71004b8b50(node);
    stats = sub_71003f4ca0(*actor, power, 1);
    BOOL unlocked = sub_71004b81e0(*actor, node);
    static SNO unk_710109d4f4 = INVALID_SNO;
    if (unk_710109d4f4 == INVALID_SNO)
        unk_710109d4f4 = SNOFileLookupGUID(POWER_SNO, "P75_ItemPassive_DarkAlchemySeal_001");
    if (unlocked)
    {
        UISkills::GetName(power, name);
        description = sub_710023e8c0(*actor, power);
    }
    else
    {
        UISkills::GetName(unk_710109d4f4, name);
        description = sub_710023e8c0(*actor, unk_710109d4f4);
    }
    CRefString text(stats);
    text += "\n";
    text += description;
    UI_GetTextFromID(entry.unk_88).SetText(name, FALSE);
    UI_GetTextFromID(entry.unk_90).SetText(text, FALSE);
}

// Probably: confirm the selected item and send the sacrifice to the server.
// NON_MATCHING: The two saved ACD lookups and the success path use a different register layout.
extern "C" void sub_7100178830()
{
    const ActorCommonData * item = ACDTryToGet(unk_71017e5538);
    const ActorCommonData * actor = ACDTryToGet(unk_71017e553c);
    if (!item || !actor)
        return;
    GameError error = sub_71004b8530(*actor, sub_710023edc0(unk_71017e5534), item);
    if (error != GAMEERROR_NONE)
    {
        UIGame::DisplayError(GameErrorData(error), FALSE, NULL);
        return;
    }
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SALVAGE_OPERATE), cgtPSODefaults);
    ACDID itemId = item->id;
    int32 controller = unk_71017e55d0;
    ActivateNodeMessage message;
    message.unk_08 = sub_710023edc0(unk_71017e5534);
    message.unk_0c = ANNFromACD(itemId);
    MessageSendToServer(CMSG_DARK_ALCHEMY_ACTIVATE_NODE, &message, sizeof(message), controller);
}

// Probably: initialize altar glow IDs when the console UI is loaded.
extern "C" void sub_7100178a00()
{
    sub_710023e7f0();
}

// Probably: open the Dark Alchemy screen and select the first locked node.
// NON_MATCHING: Node construction and unit-global addressing leave the body eight bytes larger.
extern "C" void sub_7100178a10(int32 controller)
{
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_OPEN), cgtPSODefaults);
    unk_71017e55d0 = controller;
    const auto & children = UI_GetWindowFromID(unk_7101862570).GetChildList();
    for (uint32 i = 0; i < children.size(); ++i)
    {
        UIWindow & child = UI_GetWindowFromID(children[i].id);
        child.SetUserData(reinterpret_cast<void *>(static_cast<uintptr>(i)));
        child.SetActive(TRUE);
    }

    unk_71017e55b8.clear();
    const auto & nodes = UI_GetWindowFromID(unk_7101862570).GetChildList();
    for (uint32 i = 0; i < nodes.size(); ++i)
    {
        blz::string buttonName(CRefString().Format(".Button_%d", i));
        UIID button = UIIDCreateFromParent(nodes[i].id, buttonName.c_str());
        Rect2D rect = UI_GetButtonFromID(button).GetWindowRect();
        Unk17ad70 entry;
        entry.unk_00.x = rect.left + 0.5f * rect.Width();
        entry.unk_00.y = rect.top + 0.5f * rect.Height();
        entry.unk_08 = reinterpret_cast<uintptr>(UI_GetWindowFromID(nodes[i].id).GetUserData());
        entry.unk_10 = StringListFetch(GlobalSNOGet(GLOBALSNO_UNK_912), CRefString().Format("ConsoleNode_%d_Disabled", entry.unk_08));
        entry.unk_38 = StringListFetch(GlobalSNOGet(GLOBALSNO_UNK_912), CRefString().Format("ConsoleNode_%d_Enabled", entry.unk_08));
        entry.unk_60 = StringListFetch(GlobalSNOGet(GLOBALSNO_UNK_912), CRefString().Format("ConsoleNode_%d_Activated", entry.unk_08));
        entry.unk_88 = UIIDCreateFromParent(unk_7101861520, CRefString().Format(".NodeDescriptionName"));
        entry.unk_90 = unk_7101861528;
        unk_71017e55b8.push_back(entry);
    }

    UI_GetWindowFromID(unk_7101862568).SetActive(TRUE);
    UI_GetWindowFromID(unk_7101862570).SetActive(TRUE);
    UI_GetWindowFromID(unk_7101861520).SetActive(TRUE);
    {
        CRefString title = StringListFetch("UIDarkAlchemy:DarkAlchemyTitle");
        UI_GetTextFromID(unk_7101862578).SetText(title, FALSE);
        if (!unk_71017e55e0)
        {
            unk_71017e55d4 = 0;
            const ActorCommonData * actor = ACDTryToGet(PlayerGetByControllerIndex(unk_71017e55d0)->idPlayerACD);
            if (actor)
            {
                for (uint32 node = 0; node < 30; ++node)
                    if (!sub_71004b81e0(*actor, node))
                    {
                        unk_71017e55d4 = sub_710023edd0(node);
                        break;
                    }
            }
        }
        unk_71017e55d8 = Blizzard::Time::Milliseconds();
        sub_7100179f60();
    }
    unk_71017e55e0 = FALSE;
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    for (uint32 i = 0; i < 30; ++i)
    {
        UIID glow = sub_710023e8a0(i);
        UI_GetWindowFromID(glow).SetActive(FALSE);
    }
    sub_710023ee30(FALSE);
}

// Probably: UIDarkAlchemy::Console close callback of the screen set (paired with the open callback sub_7100178a10).
// NON_MATCHING: Clang narrows the saved selection flag to a byte. The target tests a word.
extern "C" void sub_71001794e0(int32 nControllerIndex)
{
    UI_GetWindowFromID(unk_7101862568).SetActive(FALSE);
    if (!unk_71017e55e0)
    {
        unk_71017e55d4 = 0;
        unk_710109d4f8 = ACDID_INVALID;
    }
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_MAP_CLOSE), cgtPSODefaults);
    GameSoftPause(FALSE);
}

// "DarkAlchemyUI_OnUpdate" in the UI event table.
// NON_MATCHING: The node-button loop, stick traversal and unit-global loads compile with a different branch and register layout.
extern "C" void DarkAlchemyUI_OnUpdate(const UIID &)
{
    if (!UI_TestID(unk_7101862568) || !UI_GetWindowFromID(unk_7101862568).IsActive())
        return;
    // Both lookups load the same saved ACDID in the 2.7.7 binary.
    const ActorCommonData * first = ACDTryToGet(unk_710109d4f8);
    const ActorCommonData * second = first ? ACDTryToGet(unk_710109d4f8) : NULL;
    if (!second || !sub_71004b8b80(*second, *first))
    {
        sub_71001794e0(unk_71017e55d0);
        return;
    }
    if (!UI_GetWindowFromID(unk_7101862570).IsActive())
        return;
    const auto & children = UI_GetWindowFromID(unk_7101862570).GetChildList();
    for (int32 i = 0; i < children.size(); ++i)
    {
        const UIID & id = children[i].id;
        blz::string arrow(CRefString().Format(".SelectedArrow_%d", i));
        UIID arrowId = UIIDCreateFromParent(id, arrow.c_str());
        BOOL selected = i == unk_71017e55d4;
        UI_GetWindowFromID(arrowId).SetActive(selected);
        blz::string decoration(CRefString().Format(".NodeEmbellishment_%d", i));
        UI_GetWindowFromID(UIIDCreateFromParent(id, decoration.c_str())).SetActive(TRUE);
        blz::string name(CRefString().Format(".Button_%d", i));
        UIID buttonId = UIIDCreateFromParent(id, name.c_str());
        const ActorCommonData & actor = ACDGet(PlayerGetByControllerIndex(unk_71017e55d0)->idPlayerACD);
        UIButton & button = UI_GetButtonFromID(buttonId);
        uint32 node = sub_710023edc0(i);
        if (sub_71004b81e0(actor, node))
        {
            button.SetEnabled(TRUE);
            button.SetButtonState(BS_PUSHED, FALSE, FALSE);
            continue;
        }
        if (!sub_71004b8230(actor, node))
        {
            button.SetEnabled(FALSE);
            continue;
        }
        int32 category = sub_71004b83e0(node);
        if (!category)
            category = sub_71004b8400(actor);
        Unk1784d0 costs;
        if (category)
            sub_71004b7340(actor, category, costs);
        auto sufficient = [](const blz::vector<Unk4b97c0> & group) {
            for (const auto & cost : group)
                if (cost.unk_20 < cost.unk_18)
                    return false;
            return true;
        };
        BOOL enabled = sufficient(costs.unk_00) && sufficient(costs.unk_18);
        button.SetEnabled(enabled);
        if (enabled)
            button.SetButtonState(selected ? BS_MOUSEOVER : BS_NORMAL, FALSE, FALSE);
    }
    sub_7100178050();
    sub_71001785f0();

    static const Vector2D unk_71017e5558(1.0f, 0.0f);
    if (!::Console::ControllerUtils::LStickIsInDeadZone(unk_71017e55d0, FALSE) &&
        Blizzard::Time::Milliseconds() - unk_71017e55d8 >= 300 && unk_71017e55d4 != -1)
    {
        Vector2D stick = InputGetCurrentState(INPUTCONTEXT_DEFAULT).tControllerState[unk_71017e55d0].tCurrentFrame.vecLeftStick;
        stick.y = -stick.y;
        Angle direction = Vector2D::AngleBetween(stick, unk_71017e5558);
        int32 next = -1;
        float nearest = FLT_MAX;
        const Vector2D source = unk_71017e55b8[unk_71017e55d4].unk_00;
        for (uint32 i = 0; i < unk_71017e55b8.size(); ++i)
        {
            if (i == unk_71017e55d4)
                continue;
            const Vector2D & target = unk_71017e55b8[i].unk_00;
            Angle angle = Vector2D::AngleBetween(target - source, unk_71017e5558);
            if (Math::AngularDifference(direction, angle) < 0.7f)
            {
                float distance = Vector2D::DistanceSquared(source, target);
                if (next == -1 || distance < nearest)
                {
                    next = i;
                    nearest = distance;
                }
            }
        }
        if (next != -1)
        {
            unk_71017e55d4 = next;
            unk_71017e55d8 = Blizzard::Time::Milliseconds();
            sub_7100179f60();
        }
    }
    sub_7100179f60();
    auto & manager = ::Console::Screens::GetActionManager();
    if (manager.AreActionsDirty())
    {
        manager.SetActionsDirtyFlag(FALSE);
        manager.ClearAllActions();
        if (unk_71017e55dc)
        {
            blz::string text(StringListFetch("UIDarkAlchemy:ConsoleActivateNodeButtonText").str());
            const ActorCommonData * actor = ACDTryToGet(PlayerGetByControllerIndex(unk_71017e55d0)->idPlayerACD);
            if (sub_71004b8230(*actor, 29))
                text = StringListFetch("UIDarkAlchemy:Unlock").str();
            manager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 1, text.c_str());
        }
        manager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close").str());
        ::Console::Screens::UpdateButtonLegendText();
    }
    sub_710023ee30(TRUE);
}

// Probably: refresh the selected node's activate action.
// NON_MATCHING: The selected node and eligibility flag use a different register and branch layout.
extern "C" void sub_7100179f60()
{
    const ActorCommonData * actor = ACDTryToGet(PlayerGetByControllerIndex(unk_71017e55d0)->idPlayerACD);
    uint32 node = sub_710023edc0(unk_71017e55d4);
    GameError error = sub_71004b8530(*actor, node, NULL);
    BOOL available = sub_71004b8230(*actor, node) & (error == GAMEERROR_NONE || error == GAMEERROR_UNK_52);
    available &= !sub_71004b81e0(*actor, node);
    if (unk_71017e55dc != available)
    {
        unk_71017e55dc = available;
        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    }
}

// Probably: UIDarkAlchemy::Console can-close test: TRUE unless the Dark Alchemy window exists and is active.
extern "C" BOOL sub_710017a0b0()
{
    BOOL bCanClose = TRUE;
    if (UI_TestID(unk_7101862568))
        bCanClose = !UI_GetWindowFromID(unk_7101862568).IsActive();
    return bCanClose;
}

// Probably: validate the selected inventory item and ask for confirmation.
// NON_MATCHING: The confirmation functor and parameter initialization compile four bytes larger.
extern "C" void sub_710017a100(const ActorCommonData * item, const ActorCommonData & actor)
{
    if (!item)
        return;
    unk_71017e5538 = item->id;
    unk_71017e553c = actor.id;
    GameError error = sub_71004b8530(actor, sub_710023edc0(unk_71017e5534), item);
    if (error != GAMEERROR_NONE)
    {
        UIGame::DisplayError(GameErrorData(error), FALSE, NULL);
        return;
    }
    NotificationCreationParams params;
    params.uszMessage = StringListFetch("UIDarkAlchemy:Warning");
    params.tCallbacks.pfnOnClickOkButton = [](const UIID &) { sub_7100178830(); };
    params.nControllerIndex = unk_71017e55d0;
    ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(params);
}

// "DarkAlchemyUI_OnButton1" in the UI event table.
// NON_MATCHING: The saved selection flag is narrowed to a byte and the two selection indices use different data addressing.
extern "C" void DarkAlchemyUI_OnButton1(const UIID &, int32 controller)
{
    if (!UI_TestID(unk_7101862568) || !UI_GetWindowFromID(unk_7101862568).IsActive())
        return;
    const ActorCommonData * actor = ACDTryToGet(PlayerGetByControllerIndex(controller)->idPlayerACD);
    if (!actor)
        return;
    if (!PlayerCanChangeSkillorTrait(*actor, FALSE))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_LOOTRUN_TIERED_CANNOT_DO_THAT), FALSE, NULL);
        return;
    }
    if (!sub_71004b81d0(unk_71017e55d4))
        return;
    uint32 node = sub_710023edc0(unk_71017e55d4);
    if (!sub_71004b8230(*actor, node))
        return;
    GameError error = sub_71004b8530(*actor, node, NULL);
    if (error == GAMEERROR_INSUFFICIENT_FUNDS)
        return;
    unk_71017e5534 = unk_71017e55d4;
    if (error == GAMEERROR_NONE)
    {
        ActivateNodeMessage msg;
        msg.unk_08 = sub_710023edc0(unk_71017e55d4);
        msg.unk_0c = ANNFromACD(ACDID_INVALID);
        MessageSendToServer(CMSG_DARK_ALCHEMY_ACTIVATE_NODE, &msg, sizeof(msg), controller);
        return;
    }
    if (sub_71004b8330(*actor) == 3)
        sub_710016a990(8);
    if (sub_71004b8330(*actor) == 6)
        sub_710016a990(12);
    if (sub_71004b8330(*actor) == 9)
        sub_710016a990(2);
    if (sub_71004b8330(*actor) == 17)
        sub_710016a990(3);
    if (sub_71004b8330(*actor) == 19)
        sub_710016a990(2);
    if (sub_71004b8330(*actor) == 22)
        sub_710016a990(26);
    unk_71017e55e0 = TRUE;
    ::Console::Screens::OpenToScreen(32, FALSE, controller, FALSE);
}

// Probably: "DarkAlchemyUI_OnButton2": closes the screen set when the button-2 action is close (action 1).
extern "C" void sub_710017a510(const UIID & id, const int32 nControllerIndex)
{
    if (unk_71017e55d0 != nControllerIndex)
        return;
    if (::Console::Screens::GetActionManager().GetActionForInput(::Console::CONTROLLER_INPUT_BUTTON_2) != 1)
        return;
    if (UI_TestID(unk_7101862568) && UI_GetWindowFromID(unk_7101862568).IsActive())
        ::Console::Screens::CloseScreenSet(TRUE, FALSE);
}

// Probably: restore the altar after the inventory selection screen closes.
// NON_MATCHING: Clang tests the narrowed selection flag and addresses the controller separately.
extern "C" void sub_710017a580(uint32)
{
    if (unk_71017e55e0)
    {
        unk_71017e55e0 = FALSE;
        ::Console::Screens::OpenToScreen(31, FALSE, unk_71017e55d0, FALSE);
    }
}

// Probably: open the altar for the interacting player's controller.
extern "C" void sub_710017a5b0(ACDID sourceId, ACDID playerId)
{
    const ActorCommonData * actor = ACDTryToGet(playerId);
    if (!actor)
        return;
    const ActorCommonData * source = ACDTryToGet(sourceId);
    if (!source || !sub_71004b8b80(*source, *actor))
        return;
    const Player * player = PlayerGetByACD(playerId);
    if (player)
    {
        unk_710109d4f8 = playerId;
        ::Console::Screens::OpenToScreen(31, FALSE, player->nControllerIndex, FALSE);
    }
}
