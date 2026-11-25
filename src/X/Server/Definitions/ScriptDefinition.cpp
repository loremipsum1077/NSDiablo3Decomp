// Original file: X/Server/Definitions/ScriptDefinition.cpp

#include "ScriptDefinition.h"

// For SNONAME_NONE and cgplaceNull, whose initializers the original runs in this file's static
// initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "ConductorDefinition.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/Definitions/ConversationDefinition.h"
#include "../../Common/Definitions/ConversationListDefinition.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/Hyperlink.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include <stddef.h>

// Declared here because no adopted header carries them yet.
extern const XTypeDescriptor * DT_SNO; extern const XTypeDescriptor * DT_SCRIPT_EVENT_VARIANT_PARAM_LW;  // D3DescribedType.cpp, ScriptEventParams.cpp:120
extern const SymbolicEntry gScriptEventTypes[64];  // SymbolicValue.cpp:3909
void LuaScriptFreeStateWithSNO(SNO snoScript);  // LuaScript.cpp
SNO ScriptDefinitionCreateScript(uint8 * ptScript, uint32 dwScriptSize, ScriptMessageMap * arMessageMap, int32 nMessageMapCount, SNOName & snonameOwner, LPCSTR szChildFileName); void ScriptDefinitionDisposeScript(SNO snoScript);

enum {
    SCRIPT_SNO_COMMON_VERSION = 85,
    SCRIPT_XML_VERSION = 12,
    SCRIPT_SNO_PC_VERSION = 85,
    SCRIPT_SNO_CONSOLE_VERSION = 93,
    SCRIPT_SNO_AXE_VERSION = 88,
    SCRIPT_SNO_PS4_VERSION = 95,
    SCRIPT_MESSAGE_MAP_XML_VERSION = 1,
};


























static const TypeDescriptorField ScriptEventVariantParamString_Console_Fields[3] = { TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ScriptEventVariantParamString_Console, serString), NULL, 0x700000), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ScriptEventVariantParamString_Console, szString), 0x700020, (int32)offsetof(ScriptEventVariantParamString_Console, serString) - (int32)offsetof(ScriptEventVariantParamString_Console, szString)), TypeDescriptorField(NULL, DT_NULL, sizeof(ScriptEventVariantParamString_Console), NULL, 0) };

ComplexTypeDescriptor TypeDescriptor_ScriptEventVariantParamString_Console("ScriptEventVariantParamString_Console", 0, ScriptEventVariantParamString_Console_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SCRIPT_EVENT_VARIANT_PARAM_STRING_CONSOLE = &TypeDescriptor_ScriptEventVariantParamString_Console;


static const TypeDescriptorField ScriptMessageMap_Fields[9] = {
    EnumTypeDescriptorField("", offsetof(ScriptMessageMap, eEventType), NULL, gScriptEventTypes), TypeDescriptorField("", DT_INT, offsetof(ScriptMessageMap, dwUniqueID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ScriptMessageMap, dwListenerFlags), NULL, 0x80001), TypeDescriptorField("", DT_INT, offsetof(ScriptMessageMap, nEventCount), (const void *)1, 1),
    FixedArrayTypeDescriptorField("", DT_SCRIPT_EVENT_VARIANT_PARAM_LW, offsetof(ScriptMessageMap, arMessageFilters), NULL, 0x700000, 4, 0),
    FixedArrayTypeDescriptorField("", DT_SCRIPT_EVENT_VARIANT_PARAM_STRING_CONSOLE, offsetof(ScriptMessageMap, arMessageFilters_Strings), NULL, 0x700000, 4, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ScriptMessageMap, serHandlerFunction_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ScriptMessageMap, szHandlerFunction), 0x700020, (int32)offsetof(ScriptMessageMap, serHandlerFunction_Console) - (int32)offsetof(ScriptMessageMap, szHandlerFunction)), TypeDescriptorField(NULL, DT_NULL, sizeof(ScriptMessageMap), NULL, 0),
};


ComplexTypeDescriptor TypeDescriptor_ScriptMessageMap("ScriptMessageMap", SCRIPT_MESSAGE_MAP_XML_VERSION, ScriptMessageMap_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SCRIPT_MESSAGE_MAP = &TypeDescriptor_ScriptMessageMap;

static const TypeDescriptorField ScriptDefinition_Fields[9] = {
    TypeDescriptorField("", DT_INT, offsetof(ScriptDefinition, dwMachineTime), NULL, 1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ScriptDefinition, serCompiledData), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ScriptDefinition, pCompiledData), 0x20, (int32)offsetof(ScriptDefinition, serCompiledData) - (int32)offsetof(ScriptDefinition, pCompiledData)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ScriptDefinition, serMessageMap), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SCRIPT_MESSAGE_MAP, offsetof(ScriptDefinition, arScriptMessageMap), 0x21, (int32)offsetof(ScriptDefinition, serMessageMap) - (int32)offsetof(ScriptDefinition, arScriptMessageMap)),
    VariableArrayCountTypeDescriptorField("", offsetof(ScriptDefinition, nMessageMapCount), 0x10000, DT_SCRIPT_MESSAGE_MAP, (int32)offsetof(ScriptDefinition, serMessageMap) - (int32)offsetof(ScriptDefinition, nMessageMapCount)),
    TypeDescriptorField("", DT_INT, offsetof(ScriptDefinition, dwScriptUID), NULL, 1), FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptDefinition, snoQuestMetaData), "", QUEST_SNO, 0, -1), TypeDescriptorField(NULL, DT_NULL, sizeof(ScriptDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ScriptDefinition("Script", SCRIPT_XML_VERSION, ScriptDefinition_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_SCRIPT = &TypeDescriptor_ScriptDefinition;





ScriptGroupDef * gptGroupDefScript;































































ScriptGroupDef::ScriptGroupDef()
    : GroupDefinition("Script", SCRIPT_SNO,
                      SNOVersions(SCRIPT_SNO_PC_VERSION,
                                  SCRIPT_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  SCRIPT_SNO_CONSOLE_VERSION,
                                  SCRIPT_SNO_PS4_VERSION,
                                  SCRIPT_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(ScriptDefinition), DT_SCRIPT, 0x8641, 0x500e)
{
}

















uint32 ScriptGroupDef::GetMaxCount() {
    return 9000;
}















































































































































BOOL ScriptGroupDef::Write(void * lpDefinition,
                           FileReference & tRef) const
{








    XASSERT(FALSE);
    return FALSE;
}






void ScriptGroupDef::Delete(void * lpDefinition) const
{
    ScriptDefinition * pdef = (ScriptDefinition *)lpDefinition;









    SNO snoScript = pdef->tHeader.snoHandle;
    if (!gptGroupDefScript->WasDynamicallyCreated(snoScript))
    {
        if (ServerIsLocal() && AppServerThreadIsRunningGame())
        {
            Blizzard::Lock::ScopedMutex tLock(sgtAppGlobals.tScriptDefPurgeMutex);
            sgtAppGlobals.ptScriptDefsPurged->Add_Tracked(snoScript);
        }
        else
        {
            ScriptDefinitionDisposeScript(snoScript);
        }
    }


    GroupDefinition::Delete(lpDefinition);
}




void ScriptGroupDef::Postprocess(void * lpDefinition)
{
    GroupDefinition::Postprocess(lpDefinition);

    ScriptDefinition * pdef = (ScriptDefinition *)lpDefinition;


    for (int32 i = 0; i < pdef->nMessageMapCount; i++)
    {
        ScriptMessageMap & tMessageMap = pdef->arScriptMessageMap[i];
        tMessageMap.PostProcess();








        for (int32 nFilter = 0; nFilter < 4; nFilter++)
        {
            if (tMessageMap.arMessageFilters[nFilter].eParamType == (ScriptEventVariantParamType)-1)
            {



                tMessageMap.arMessageFilters[nFilter].eParamType = SCRIPT_EVENT_VARIANT_PARAM_NONE;
            }
            else if ((uint32)tMessageMap.arMessageFilters[nFilter].eParamType >= SCRIPT_EVENT_NUM_VARIANT_PARAM_TYPES)
            {

                CRefString sScript = HyperlinkCreateFromSNO(SNOName(SCRIPT_SNO, pdef->tHeader.snoHandle), NULL);


                tMessageMap.arMessageFilters[nFilter].eParamType = SCRIPT_EVENT_VARIANT_PARAM_NONE;
            }
        }
    }




    pdef->snoNameDynamic.Set(SNOGROUP_NONE, SNO_NONE);
}




uint32 ScriptGroupDef::GetPrecacheDependencyCount()
{

    uint32 dwRetCount = 0;

    dwRetCount += SNOFileEnumerateSNOsForGroup(gptGroupDefConversation->GetGroup(), NULL);
    dwRetCount += SNOFileEnumerateSNOsForGroup(gptGroupDefConversationList->GetGroup(), NULL);
    dwRetCount += SNOFileEnumerateSNOsForGroup(gptGroupDefConductor->GetGroup(), NULL);
    dwRetCount += SNOFileEnumerateSNOsForGroup(gptGroupDefPower->GetGroup(), NULL);
    return dwRetCount;
}












































































































































































































































































































































































































































SNO ScriptDefinitionCreateScript(uint8 * ptScript,
                                 uint32 dwScriptSize,
                                 ScriptMessageMap * arMessageMap,
                                 int32 nMessageMapCount,
                                 SNOName & snonameOwner,
                                 LPCSTR szChildFileName)
{

    ScriptDefinition tDef;
    SigmaMemoryClear(&tDef, sizeof(ScriptDefinition));

    tDef.pCompiledData = ptScript;
    tDef.serCompiledData.dwSize = dwScriptSize;
    tDef.arScriptMessageMap = arMessageMap;
    tDef.nMessageMapCount = nMessageMapCount;






    SNO snoScript = gptGroupDefScript->New(&tDef, TRUE, sizeof(ScriptDefinition));

    {
        ScopedScriptDefinitionLockR pdef(snoScript);
        ((ScriptDefinition *)(const ScriptDefinition *)pdef)->snoNameDynamic = snonameOwner;
    }


    return snoScript;
}




void ScriptDefinitionDisposeScript(SNO snoScript)
{

    if (!sgtAppGlobals.ptLuaScriptAppGlobals)
    {
        return;
    }





    if (GameCommonDataExists() && SGameManager::GetGameCount() > 0)
    {
        SGameID idSGameCurrent = SGameManager::GetCurrentGame();
        if (idSGameCurrent != -1)
            GameServerCodeLeave_Tracked(TRUE);


        SGameIterator tIterator;
        while (SGameManager::GameIterate(tIterator))
        {
            SGame * ptSGame = tIterator.ptElement;
            BOOL bReturnToClient = _GameServerCodeEnter(ptSGame->id);

            LuaScriptFreeStateWithSNO(snoScript);

            GameServerCodeLeave_Tracked(bReturnToClient);
        }

        if (idSGameCurrent != -1)
            _GameServerCodeEnter(idSGameCurrent);
    }





    gptGroupDefScript->Purge(snoScript, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
}

void ScriptMessageMap::PostProcess()
{
    for (int32 i = 0; i < 4; ++i)
    {
        switch (arMessageFilters[i].eParamType)
        {
        case SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH:
        case SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME:
        case SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP:
        case SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME:
            arMessageFilters[i].szString = arMessageFilters_Strings[i].szString;
            break;
        default:
            break;
        }
    }
}
