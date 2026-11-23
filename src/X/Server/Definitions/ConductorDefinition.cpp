// Original file: X/Server/Definitions/ConductorDefinition.cpp

#include "ConductorDefinition.h"

// For SNONAME_NONE and cgplaceNull, whose initializers the original runs in this file's static
// initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "ScriptDefinition.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/Game.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include <stddef.h>
#include <stdio.h>

// Declared here because their header does not carry them yet (ScriptDefinition.cpp:83, 871, 906).
extern const XTypeDescriptor * DT_SCRIPT_MESSAGE_MAP;
SNO ScriptDefinitionCreateScript(uint8 * ptScript, uint32 dwScriptSize, ScriptMessageMap * arMessageMap, int32 nMessageMapCount, SNOName & snonameOwner, LPCSTR szChildFileName);
void ScriptDefinitionDisposeScript(SNO snoScript);

enum {
    CONDUCTOR_SNO_COMMON_VERSION = 48,
    CONDUCTOR_XML_VERSION = 7,
    CONDUCTOR_SNO_PC_VERSION = 48,
    CONDUCTOR_SNO_CONSOLE_VERSION = 57,
    CONDUCTOR_SNO_AXE_VERSION = 52,
    CONDUCTOR_SNO_PS4_VERSION = 59,
};








const char * gszSequenceStartFunctionName = "Phase_%d_Sequence_%d_Start";
const char * gszSequenceInterruptedFunctionName = "Phase_%d_Sequence_%d_Interrupted";
const char * gszPhaseInitFunctionName = "Phase_%d_Init";















































































































static const TypeDescriptorField ConductorSequence_Fields[5] = {
    TypeDescriptorField("", DT_INT, offsetof(ConductorSequence, nMessageMapCount), NULL, 1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConductorSequence, serMessageMap), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SCRIPT_MESSAGE_MAP, offsetof(ConductorSequence, arScriptMessageMap), 0x21, (int32)offsetof(ConductorSequence, serMessageMap) - (int32)offsetof(ConductorSequence, arScriptMessageMap)),
    TypeDescriptorField("", DT_INT, offsetof(ConductorSequence, bRepeatable), NULL, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(ConductorSequence), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConductorSequence("ConductorSequence", 0, ConductorSequence_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CONDUCTOR_SEQUENCE = &TypeDescriptor_ConductorSequence;

static const TypeDescriptorField ConductorPhase_Fields[6] = {
    TypeDescriptorField("", DT_INT, offsetof(ConductorPhase, dwPhaseHash), NULL, 0x700000), TypeDescriptorField("", DT_INT, offsetof(ConductorPhase, dwPhaseHashPadding), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(ConductorPhase, nNumSequences), NULL, 1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConductorPhase, serSequences), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CONDUCTOR_SEQUENCE, offsetof(ConductorPhase, arSequences), 0x21, (int32)offsetof(ConductorPhase, serSequences) - (int32)offsetof(ConductorPhase, arSequences)), TypeDescriptorField(NULL, DT_NULL, sizeof(ConductorPhase), NULL, 0),
};

ComplexTypeDescriptor TypeDescriptor_ConductorPhase("ConductorPhase", 0, ConductorPhase_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CONDUCTOR_PHASE = &TypeDescriptor_ConductorPhase;

static const TypeDescriptorField ConductorDefinition_Fields[8] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConductorDefinition, serPhases), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_CONDUCTOR_PHASE, offsetof(ConductorDefinition, arPhases), 0x21, (int32)offsetof(ConductorDefinition, serPhases) - (int32)offsetof(ConductorDefinition, arPhases)),
    TypeDescriptorField("", DT_INT, offsetof(ConductorDefinition, nNumPhases), NULL, 1), BoundedTypeDescriptorField("", DT_INT, offsetof(ConductorDefinition, bRunOnlyOnce), NULL, (const void *)0, (const void *)1),
    TypeDescriptorField("", DT_INT, offsetof(ConductorDefinition, dwScriptMachineTime), NULL, 1), TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConductorDefinition, serCompiledScript), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ConductorDefinition, pCompiledScript), 0x20, (int32)offsetof(ConductorDefinition, serCompiledScript) - (int32)offsetof(ConductorDefinition, pCompiledScript)), TypeDescriptorField(NULL, DT_NULL, sizeof(ConductorDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConductorDefinition("Conductor", CONDUCTOR_XML_VERSION, ConductorDefinition_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CONDUCTOR = &TypeDescriptor_ConductorDefinition;






ConductorGroupDef * gptGroupDefConductor;





ConductorGroupDef::ConductorGroupDef()
    : GroupDefinition("Conductor", CONDUCTOR_SNO,
                      SNOVersions(CONDUCTOR_SNO_PC_VERSION,
                                  CONDUCTOR_SNO_AXE_VERSION + AXE_BASE_SNO_VERSION,
                                  CONDUCTOR_SNO_CONSOLE_VERSION,
                                  CONDUCTOR_SNO_PS4_VERSION,
                                  CONDUCTOR_SNO_CONSOLE_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(ConductorDefinition), DT_CONDUCTOR, 0x8241, 0x500e)
{
}
















uint32 ConductorGroupDef::GetMaxCount() {
    return 128;
}





void ConductorGroupDef::GetDefaultData(void * ptDefaultDef) const {
    ConductorDefinition * pdef = (ConductorDefinition *)ptDefaultDef;
    SigmaMemoryClear(ptDefaultDef, sizeof(ConductorDefinition));
    pdef->arPhases = NULL;
    pdef->serPhases.dwSize = 0;
}
































































BOOL ConductorGroupDef::Write(void * lpDefinition,
                              FileReference & tRef) const
{



    XASSERT(FALSE);
    return FALSE;
}






void ConductorGroupDef::Postprocess(void * lpDefinition)
{

    GroupDefinition::Postprocess(lpDefinition);

    ConductorDefinition * pdef = (ConductorDefinition *)lpDefinition;

    pdef->snoScript = SNO_NONE;

    for (uint32 nPhase = 0; nPhase < pdef->nNumPhases; nPhase++)
    {
        ConductorPhase & tPhase = pdef->arPhases[nPhase];
        for (uint32 nSequence = 0; nSequence < tPhase.nNumSequences; nSequence++)
        {
            ConductorSequence & tSequence = tPhase.arSequences[nSequence];
            ScriptMessageMap * ptMessageMap = tSequence.arScriptMessageMap;
            ptMessageMap->PostProcess();
        }
    }

    if (!(pdef->tHeader.dwFlags & (1 << DH_IS_EXTERNAL_BIT)))
    {
        SNOName snoname(CONDUCTOR_SNO, pdef->tHeader.snoHandle);










        pdef->snoScript = ScriptDefinitionCreateScript(pdef->pCompiledScript, pdef->serCompiledScript.dwSize, NULL, 0, snoname, "0000.lua");
    }
}







void ConductorGroupDef::Delete(void * lpDefinition) const
{
    ConductorDefinition * pdef = (ConductorDefinition *)lpDefinition;






    if (ServerIsLocal() && AppServerThreadIsRunningGame())
    {
        Blizzard::Lock::ScopedMutex tLock(sgtAppGlobals.tConductorDefPurgeMutex);
        sgtAppGlobals.ptConductorDefsPurged->Add_Tracked(pdef->snoScript);
    }
    else
    {
        if (pdef->snoScript != SNO_NONE)
        {
            ScriptDefinitionDisposeScript(pdef->snoScript);
            pdef->snoScript = SNO_NONE;
        }
    }






















































    GroupDefinition::Delete(lpDefinition);
}







































































































































CRefString ConductorGetPhaseInitFunctionName(int32 nPhaseIndex)
{

    return CRefString().Format(gszPhaseInitFunctionName, nPhaseIndex);
}



CRefString ConductorGetSequenceStartFunctionName(int32 nPhaseIndex,
                                                 int32 nSequenceIndex)
{
    return CRefString().Format(gszSequenceStartFunctionName, nPhaseIndex, nSequenceIndex);
}




CRefString ConductorGetSequenceInterruptedFunctionName(int32 nPhaseIndex,
                                                       int32 nSequenceIndex)
{
    return CRefString().Format(gszSequenceInterruptedFunctionName, nPhaseIndex, nSequenceIndex);
}




BOOL ConductorGetPhaseFromInitFunctionName(const char * szFunction,
                                           int32 & nPhaseIndex)
{
    int32 nScanned = sscanf(szFunction, gszPhaseInitFunctionName, &nPhaseIndex);
    return nScanned == 1;
}




BOOL ConductorGetPhaseAndSequenceFromStartFunctionName(const char * szFunction,
                                                       int32 & nPhaseIndex,
                                                       int32 & nSequenceIndex)
{
    int32 nScanned = sscanf(szFunction, gszSequenceStartFunctionName, &nPhaseIndex, &nSequenceIndex);
    return nScanned == 2;
}































CRefString ConductorGetPhaseLabel(SNO snoConductor,
                                  uint32 nPhaseIndex)
{



    return CRefString().Format("#%d", nPhaseIndex);
}










CRefString ConductorGetSequenceLabel(SNO snoConductor,
                                     uint32 nPhaseIndex,
                                     uint32 nSequenceIndex)
{
    return CRefString().Format("#%d.%d", nPhaseIndex, nSequenceIndex);
}
