// Original file: X/Common/Definitions/PowerDefinition.cpp

#include "PowerDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "ActorDefinition.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../SNO/SNOName.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

// Defined in X/Common/Powers/PowersStruct.cpp:91; no adopted header declares it yet.
extern const XTypeDescriptor * DT_POWER_DEF;

// Defined in X/Server/Definitions/ScriptDefinition.cpp:871 and :906; no adopted header declares
// them yet.
struct ScriptMessageMap;
SNO ScriptDefinitionCreateScript(uint8 * ptScript, uint32 dwScriptSize, ScriptMessageMap * arMessageMap, int32 nMessageMapCount, SNOName & snonameOwner, LPCSTR szChildFileName);
void ScriptDefinitionDisposeScript(SNO snoScript);

// Defined in X/Common/Game.cpp:3713 and :5488; no adopted header declares them yet.
BOOL ServerIsLocal();
BOOL AppServerThreadIsRunningGame();

// AppGlobals (X/Common/Game.h:940) comes from X/Common/Game.h via PowerDefinition.h.








extern AppGlobals sgtAppGlobals;

enum {
    POWER_XML_VERSION = 44,
    POWER_SNO_COMMON_VERSION = 1706,
    POWER_SNO_PC_VERSION = 1707,
    POWER_SNO_CONSOLE_VERSION = 1721,
    POWER_SNO_AXE_VERSION = 1714,
    POWER_SNO_PS4_VERSION = 1723,
};









static const TypeDescriptorField ScriptFormulaDetails_Fields[5] = {
    CharArrayTypeDescriptorField("", offsetof(ScriptFormulaDetails, szFormulaName), "", 1, sizeof(((ScriptFormulaDetails *)0)->szFormulaName), -1),
    CharArrayTypeDescriptorField("", offsetof(ScriptFormulaDetails, szNote), "", 1, sizeof(((ScriptFormulaDetails *)0)->szNote), -1),
    TypeDescriptorField("", DT_INT, offsetof(ScriptFormulaDetails, uRunes), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ScriptFormulaDetails, nCustomSortIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ScriptFormulaDetails), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ScriptFormulaDetails("ScriptFormulaDetails", 0, ScriptFormulaDetails_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SCRIPT_FORMULA_DETAILS = &TypeDescriptor_ScriptFormulaDetails;



static const TypeDescriptorField PowerDefinition_Fields[8] = {
    TypeDescriptorField("", DT_POWER_DEF, offsetof(PowerDefinition, tPower), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PowerDefinition, bScripted), NULL, (const void *)FALSE, (const void *)TRUE),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PowerDefinition, bUseSecondaryResource), NULL, (const void *)FALSE, (const void *)TRUE),
    TypeDescriptorField("", DT_INT, offsetof(PowerDefinition, nFormulaCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PowerDefinition, serCompiledScript), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(PowerDefinition, pCompiledScript), 0x20,
                                     (int32)offsetof(PowerDefinition, serCompiledScript) - (int32)offsetof(PowerDefinition, pCompiledScript)),
    FilterTypeDescriptorField("", DT_SNO, offsetof(PowerDefinition, snoQuestMetaData), "", QUEST_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PowerDefinition), NULL, 0),
};









ComplexTypeDescriptor TypeDescriptor_PowerDefinition("Power", POWER_XML_VERSION, PowerDefinition_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_POWER_DEFINITION = &TypeDescriptor_PowerDefinition;





PowerGroupDef * gptGroupDefPower;





PowerGroupDef::PowerGroupDef()
    : GroupDefinition("Power", POWER_SNO,
                      SNOVersions(POWER_SNO_PC_VERSION, 1716, POWER_SNO_CONSOLE_VERSION, POWER_SNO_PS4_VERSION, 1722),
                      SNODATATYPE_TITLE, sizeof(PowerDefinition), DT_POWER_DEFINITION, 0x5441, 0x500e) {
}






















uint32 PowerGroupDef::GetMaxCount() {
    return 256;
}









void PowerGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(PowerDefinition));

    PowerDefinition * pdef = (PowerDefinition *)ptDefaultDef;
    pdef->tPower.uFlags = 0;
    for (int32 i = 0; i < 2; i++) {
        ActorCollisionFlags * ptCollFlags = &pdef->tPower.arCollFlags[i];
        ptCollFlags->uStaticOneRequiredFlags = 1;
        ptCollFlags->uDynamicFlagsNoOverlap = 0x63b;
        ptCollFlags->uDynamicMyFlags = 1;
    }
}
































































































































































void PowerGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    PowerDefinition * ptPowerDef = (PowerDefinition *)lpDefinition;
    ptPowerDef->snoScript = SNO_NONE;
    if (ptPowerDef->bScripted && !(ptPowerDef->tHeader.dwFlags & (1 << DH_IS_EXTERNAL_BIT))) {
















        SNOName snoname(POWER_SNO, ptPowerDef->tHeader.snoHandle);




































        ptPowerDef->snoScript = ScriptDefinitionCreateScript(ptPowerDef->pCompiledScript, ptPowerDef->serCompiledScript.dwSize, NULL, 0, snoname, "0000.lua");
    }
}







BOOL PowerGroupDef::CacheManagerCanPurgeElement(const DefDataElement & tDefElement, BOOL fIgnoreDoNotPurgeBit) const {
    return GroupDefinition::CacheManagerCanPurgeElement(tDefElement, fIgnoreDoNotPurgeBit) != FALSE;
}















































































































































void PowerGroupDef::Delete(void * lpDefinition) const {
    PowerDefinition * ptPowerDef = (PowerDefinition *)lpDefinition;
    if (ptPowerDef->bScripted && ptPowerDef->snoScript != SNO_NONE) {
        if (ServerIsLocal() && AppServerThreadIsRunningGame()) {




































            Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tPowerDefPurgeMutex);
            sgtAppGlobals.ptPowerDefsPurged->Add_Tracked(ptPowerDef->snoScript);
        } else {
            ScriptDefinitionDisposeScript(ptPowerDef->snoScript);
        }
        ptPowerDef->snoScript = SNO_NONE;
    }

    GroupDefinition::Delete(lpDefinition);
}


























































































































































































































































































































































































































































BOOL PowerIsContactFrameEnabled(const PowerDefinition * pdefPower, int32 nContactFrame) {
    BOOL bEnabled = FALSE;
    if (nContactFrame >= 0 && nContactFrame < 4)
        bEnabled = (pdefPower->tPower.uFlags >> (nContactFrame + 2)) & 1;
    return bEnabled;
}
