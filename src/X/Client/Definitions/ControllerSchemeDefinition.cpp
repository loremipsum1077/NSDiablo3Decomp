// Original file: X/Client/Definitions/ControllerSchemeDefinition.cpp

#include "ControllerSchemeDefinition.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/Player.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

// Declared here because its header does not carry it yet (InputNX64.cpp:492).
CRefString InputButtonFindActionIconName(InputButton eButton, BOOL fLabelVersion, const Player * ptPlayer);
namespace Controller {
void GetActionButton(const int32 nActionIndex, InputButton & eButton, const Player * ptPlayer); InputButton GetPotionButton(const Player * ptPlayer); CRefString GetPotionButtonIconName(const Player * ptPlayer);
BOOL InputIsPotion(const BufferedInput & tInput); BOOL InputIsTownPortal(const BufferedInput & tInput); BOOL InputIsTargetLock(const BufferedInput & tInput); BOOL InputIsTogglePlayerIndicatorBeam(const BufferedInput & tInput);
CRefString GetActionButtonIconName(const int32 nSkillSlot, const Player * ptPlayer, const BOOL fLabelVersion); int32 GetSlotFromButton(InputButton eInputButton, const Player * ptPlayer); int32 GetSlotFromInput(const BufferedInput & tInput);
InputButton GetButtonFromSlot(int32 nSlot, const Player * ptPlayer); InputButton GetBindingFromSkill(const SNO snoPower, const Player * ptPlayer);
} extern const XTypeDescriptor * DT_CONTROL_SCHEME;

enum {
    CONTROL_SCHEME_XML_VERSION = 2,
    CONTROL_SCHEME_SNO_VERSION = 39,
};

// The InputButton enumerators are not in the DWARF; the tables below hold the binary's values.



static const InputButton cgtDefaultSkillBindings[6] = { 0, 7, 2, 3, 1, 6 };









static const InputButton cgtPotionQuickUseDefault = 5;
static const InputButton cgtTownPortalDefault = 11;
static const InputButton cgtTargetLockDefault = 4;
static const InputButton cgtTogglePlayerIndicatorBeamDefault = 15;


















static const ButtonBinding cgtDefaultButtons[98] = {
    { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 },
    { -1 }, { -1 }, { -1 }, { -1 }, { 13 }, { -1 }, { 9 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 },
    { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { 3 }, { 2 }, { 1 }, { 6 }, { 4 }, { -1 }, { -1 }, { -1 },
    { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 },
    { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { 8 }, { -1 }, { -1 }, { 10 },
    { -1 }, { -1 }, { -1 }, { 8 }, { 9 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { 12 }, { 13 }, { 0 },
    { 7 }, { 4 }, { 5 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { -1 }, { 1 },
};

















































































































static CRefString sButtonToIconName(InputButton eButton, BOOL fLabelVersion, const Player * ptPlayer)
{

    CRefString result = InputButtonFindActionIconName(eButton, fLabelVersion, ptPlayer);
    if (result.Length() != 0)
    {
        return result;
    }


    switch (eButton)
    {
    case 0:
        return fLabelVersion ? "{icon:button1_label}" : "{icon:button1}";
    case 1:
        return fLabelVersion ? "{icon:button2_label}" : "{icon:button2}";
    case 2:
        return fLabelVersion ? "{icon:button3_label}" : "{icon:button3}";
    case 3:
        return fLabelVersion ? "{icon:button4_label}" : "{icon:button4}";
    case 4:
        return fLabelVersion ? "{icon:l1_label}" : "{icon:l1}";
    case 5:
        return fLabelVersion ? "{icon:r1_label}" : "{icon:r1}";
    case 6:
        return fLabelVersion ? "{icon:l2_label}" : "{icon:l2}";
    case 7:
        return fLabelVersion ? "{icon:r2_label}" : "{icon:r2}";
    case 8:
        return fLabelVersion ? "{icon:up_label}" : "{icon:up}";
    case 9:
        return fLabelVersion ? "{icon:down_label}" : "{icon:down}";
    case 10:
        return fLabelVersion ? "{icon:left_label}" : "{icon:left}";
    case 11:
        return fLabelVersion ? "{icon:right_label}" : "{icon:right}";
    case 12:
        return fLabelVersion ? "{icon:back_label}" : "{icon:back}";
    case 13:
        return fLabelVersion ? "{icon:start_label}" : "{icon:start}";
    case 14:
        return fLabelVersion ? "{icon:l3_label}" : "{icon:l3}";
    case 15:
        return fLabelVersion ? "{icon:r3_label}" : "{icon:r3}";
    }

    return "";
}



void Controller::GetActionButton(const int32 nActionIndex,
                                 InputButton & eButton,
                                 const Player * ptPlayer)
{




    if (ptPlayer && ptPlayer->snoControllerBinding != SNO_NONE)
    {
        const ControlSchemeDefinition * pdefControlScheme = DefinitionLockR<ControlSchemeDefinition>(ptPlayer->snoControllerBinding);
        eButton = pdefControlScheme->arBindings[nActionIndex].eButton;
    }
    else
    {
        eButton = cgtDefaultButtons[nActionIndex].eButton;
    }
}


InputButton Controller::GetPotionButton(const Player * ptPlayer)
{

    InputButton eBinding = cgtPotionQuickUseDefault;
    if (ptPlayer && ptPlayer->snoControllerBinding != SNO_NONE)
    {
        const ControlSchemeDefinition * pdefBinding = DefinitionLockR<ControlSchemeDefinition>(ptPlayer->snoControllerBinding);
        eBinding = pdefBinding->PotionQuickUse;
    }
    return eBinding;
}







CRefString Controller::GetPotionButtonIconName(const Player * ptPlayer)
{

    CRefString result;

    InputButton eButton = GetPotionButton(ptPlayer);
    if (eButton != -1)
    {
        result = sButtonToIconName(eButton, TRUE, ptPlayer);
    }


    return result;
}


BOOL Controller::InputIsPotion(const BufferedInput & tInput)
{
    if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONSTILLDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONREPEAT)
    {





        InputButton eBinding = cgtPotionQuickUseDefault;
        Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);

        if (ptControllingPlayer && ptControllingPlayer->snoControllerBinding != SNO_NONE)
        {
            const ControlSchemeDefinition * pdefBinding = DefinitionLockR<ControlSchemeDefinition>(ptControllingPlayer->snoControllerBinding);
            eBinding = pdefBinding->PotionQuickUse;
        }





        return tInput.tBufControllerData.eInputButton == eBinding;
    }
    return FALSE; }

BOOL Controller::InputIsTownPortal(const BufferedInput & tInput)
{
    if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONSTILLDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONREPEAT)
    {





        InputButton eBinding = cgtTownPortalDefault;
        Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
        if (ptControllingPlayer && ptControllingPlayer->snoControllerBinding != SNO_NONE)
        {
            const ControlSchemeDefinition * pdefBinding = DefinitionLockR<ControlSchemeDefinition>(ptControllingPlayer->snoControllerBinding);
            eBinding = pdefBinding->TownPortalBinding;
        }





        return tInput.tBufControllerData.eInputButton == eBinding;
    }
    return FALSE; }

BOOL Controller::InputIsTargetLock(const BufferedInput & tInput)
{
    if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONSTILLDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONREPEAT || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONUP)
    {







        InputButton eBinding = cgtTargetLockDefault;
        Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
        if (ptControllingPlayer && ptControllingPlayer->snoControllerBinding != SNO_NONE)
        {
            const ControlSchemeDefinition * pdefBinding = DefinitionLockR<ControlSchemeDefinition>(ptControllingPlayer->snoControllerBinding);
            eBinding = pdefBinding->TargetLockBinding;
        }





        return tInput.tBufControllerData.eInputButton == eBinding;
    }
    return FALSE; }

BOOL Controller::InputIsTogglePlayerIndicatorBeam(const BufferedInput & tInput)
{
    if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONSTILLDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONREPEAT || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONUP)
    {







        InputButton eBinding = cgtTogglePlayerIndicatorBeamDefault;
        Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
        if (ptControllingPlayer && ptControllingPlayer->snoControllerBinding != SNO_NONE)
        {
            const ControlSchemeDefinition * pdefBinding = DefinitionLockR<ControlSchemeDefinition>(ptControllingPlayer->snoControllerBinding);
            eBinding = pdefBinding->TogglePlayerIndicatorBeam;
        }





        return tInput.tBufControllerData.eInputButton == eBinding;
    }
    return FALSE; }

BOOL Controller::ActionButtonMatch(const int32 nActionIndex,
                                   const BufferedInput & tInput)
{
    InputButton eButton = -1;
    if (tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONSTILLDOWN || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONREPEAT || tInput.eInputMsg == INPUTMSG_CONTROLLERBUTTONUP)
    {

        eButton = tInput.tBufControllerData.eInputButton;
    }

    if (eButton != -1)
    {




        InputButton eBindedButton;
        Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
        GetActionButton(nActionIndex, eBindedButton, ptControllingPlayer);

        return eBindedButton != -1 &&
               eBindedButton == eButton;
    } return FALSE; }

CRefString Controller::GetActionButtonIconName(const int32 nSkillSlot,
                                               const Player * ptPlayer,
                                               const BOOL fLabelVersion)
{
    InputButton eButton = GetButtonFromSlot(nSkillSlot, ptPlayer);
    return sButtonToIconName(eButton, fLabelVersion, ptPlayer);
}



int32 Controller::GetSlotFromButton(InputButton eInputButton,
                                    const Player * ptPlayer)
{

    if (ptPlayer && ptPlayer->snoControllerBinding != SNO_NONE)
    {
        const ControlSchemeDefinition * pdefControlScheme = DefinitionLockR<ControlSchemeDefinition>(ptPlayer->snoControllerBinding);
        for (int32 i = 0; i < 6; i++)
        {
            if (pdefControlScheme->SkillSlotBindings[i] == eInputButton)
            {
                return i;
            }
        }
    }
    else
    {

        for (int32 i = 0; i < 6; i++)
        {
            if (cgtDefaultSkillBindings[i] == eInputButton)
            {
                return i;
            }
        }
    }

    return -1;
}


int32 Controller::GetSlotFromInput(const BufferedInput & tInput)
{
    Player * ptControllingPlayer = PlayerGetByControllerIndex(tInput.tBufControllerData.nControllerIndex);
    return GetSlotFromButton(tInput.tBufControllerData.eInputButton, ptControllingPlayer);
}



InputButton Controller::GetButtonFromSlot(int32 nSlot,
                                          const Player * ptPlayer)
{

    if (nSlot < 0 || nSlot >= 6)
    {
        return -1;
    }

    if (ptPlayer && ptPlayer->snoControllerBinding != SNO_NONE)
    {
        const ControlSchemeDefinition * pdefControlScheme = DefinitionLockR<ControlSchemeDefinition>(ptPlayer->snoControllerBinding);
        return pdefControlScheme->SkillSlotBindings[nSlot];
    }


    return cgtDefaultSkillBindings[nSlot];
}



InputButton Controller::GetBindingFromSkill(const SNO snoPower,
                                            const Player * ptPlayer)
{
    if (ptPlayer)
    {




        for (int32 i = 0; i < 6; i++)
        {
            if (ptPlayer->tHeroStateData.tSavedData.tActiveSkills[i].snoSkill == snoPower)
            {
                return GetButtonFromSlot(i, ptPlayer);
            }
        }
    }
    return -1;
}



ControlSchemeDefinitionGroupDef::ControlSchemeDefinitionGroupDef()
    : GroupDefinition("ControlScheme", CONTROL_SCHEME_SNO,
                      SNOVersions(CONTROL_SCHEME_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(ControlSchemeDefinition), DT_CONTROL_SCHEME, 0x2005, 8)
{
}









uint32 ControlSchemeDefinitionGroupDef::GetMaxCount()
{
    return 16;
}






void ControlSchemeDefinitionGroupDef::GetDefaultData(void * ptDefaultDef) const
{
{
    SigmaMemoryClear(ptDefaultDef, sizeof(ControlSchemeDefinition));
    ControlSchemeDefinition * pdef = (ControlSchemeDefinition *)ptDefaultDef;

    SigmaMemoryCopy(pdef->arBindings, cgtDefaultButtons, sizeof(cgtDefaultButtons));
    SigmaMemoryCopy(pdef->SkillSlotBindings, cgtDefaultSkillBindings, sizeof(cgtDefaultSkillBindings));
    pdef->PotionQuickUse = cgtPotionQuickUseDefault;
    pdef->TownPortalBinding = cgtTownPortalDefault;
    pdef->TargetLockBinding = cgtTargetLockDefault;
    pdef->TogglePlayerIndicatorBeam = cgtTogglePlayerIndicatorBeamDefault;
} }

ControlSchemeDefinitionGroupDef * gptGroupDefControlScheme;























static const TypeDescriptorField ButtonBinding_Fields[2] = { TypeDescriptorField("", DT_INT, offsetof(ButtonBinding, eButton), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(ButtonBinding), NULL, 0) };

ComplexTypeDescriptor TypeDescriptor_ButtonBinding("ButtonBinding", 0, ButtonBinding_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_BUTTON_BINDING = &TypeDescriptor_ButtonBinding;


static const TypeDescriptorField ControlSchemeDefinition_Fields[7] = {
    FixedArrayTypeDescriptorField("", DT_BUTTON_BINDING, offsetof(ControlSchemeDefinition, arBindings), NULL, 1, 98, 0), FixedArrayTypeDescriptorField("", DT_INT, offsetof(ControlSchemeDefinition, SkillSlotBindings), NULL, 1, 6, 0),
    TypeDescriptorField("", DT_INT, offsetof(ControlSchemeDefinition, PotionQuickUse), (const void *)8, 1), TypeDescriptorField("", DT_INT, offsetof(ControlSchemeDefinition, TownPortalBinding), (const void *)9, 1),
    TypeDescriptorField("", DT_INT, offsetof(ControlSchemeDefinition, TargetLockBinding), (const void *)14, 1), TypeDescriptorField("", DT_INT, offsetof(ControlSchemeDefinition, TogglePlayerIndicatorBeam), (const void *)15, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(ControlSchemeDefinition), NULL, 0),
};


ComplexTypeDescriptor TypeDescriptor_ControlSchemeDefinition("ControlScheme", CONTROL_SCHEME_XML_VERSION, ControlSchemeDefinition_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CONTROL_SCHEME = &TypeDescriptor_ControlSchemeDefinition;
