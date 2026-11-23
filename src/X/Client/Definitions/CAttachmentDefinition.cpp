// Original file: X/Client/Definitions/CAttachmentDefinition.cpp

#include "AppearanceDefinition.h"
#include "../../Common/Definitions/AttachmentDefinition.h"

// Defined in X/Sigma/TypeDescriptor/SymbolicValue.cpp:117; no adopted header declares it yet.
uint32 StringHashI(LPCSTR pszName);

int32 HardpointIndexFromName(const AppearanceDefinition * pdefAppearance, HardpointHash szName);
int32 LookIndexFromHash(const AppearanceDefinition * pdefAppearance, uint32 nHash);







// line 19
int32 HardpointIndexFromName(const SNO snoAppearance, HardpointHash szName) {


    ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
    return HardpointIndexFromName(pdefAppearance, szName);
}




// line 30
int32 HardpointIndexFromName(const AppearanceDefinition * pdefAppearance, HardpointHash szName) {


    const Structure & tStructure = pdefAppearance->tStructure;


    HardpointHash szNameToLookFor(DEFAULT_HARDPOINT_NAME);
    if (!(szName == 0u)) {
        szNameToLookFor = szName;
    }

    for (int32 i = 0; i < tStructure.nHardpointCount; i++) {

        if (tStructure.ptHardpoints[i].tHash == szNameToLookFor) {
            return i;
        }
    }



    return -1;
}




// line 56
inline int32 LookIndexFromName(const AppearanceDefinition * pdefAppearance, LPCSTR szLookName) {



    if (!szLookName || !*szLookName) {
        return -1;
    }

    uint32 dwLookHash = StringHashI(szLookName);
    return LookIndexFromHash(pdefAppearance, dwLookHash);
}




// line 72
int32 LookIndexFromHash(const AppearanceDefinition * pdefAppearance, uint32 nHash) {




    for (int32 i = 0; i < pdefAppearance->nNumLooks; i++) {


        if (pdefAppearance->ptAppearanceLooks[i].dwLookHash == nHash) {
            return i;
        }
    }






    return -1;
}




// line 95
int32 LookIndexFromName(const SNO snoAppearance, LPCSTR szLookName) {


    ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
    return LookIndexFromName(pdefAppearance, szLookName);
}




// line 106
int32 LookIndexFromHash(const SNO snoAppearance, uint32 nHash) {


    ScopedAppearanceDefinitionLockR pdefAppearance(snoAppearance);
    return LookIndexFromHash(pdefAppearance, nHash);
}
