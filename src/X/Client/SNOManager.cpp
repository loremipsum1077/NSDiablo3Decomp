// Original file: X/Client/SNOManager.cpp
// Functions sit on their original lines (see the reference .cpp).

#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "SNOManager.h"
#include "Prefetch.h"
#include "CGame.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/SNO/SNOFilesAsync.h"
#include "../Common/SNO/SNOTOC.h"
#include "Gfx/GfxVB.h"
#include "../Common/ThreadGlobals.h"
#include "../Common/Game.h"
#include "../Common/Definitions/Definitions.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/SigmaTiming.h"
#include "../Sigma/ErrorManager/ErrorManager.h"

// Sleep: X/Sigma/SigmaTiming.h:67.







// X/Common/SNO/SNOName.h:118 (inlined into 16 functions).
inline CompressedSNOName::operator SNOName() {
    return SNOName(snoGroup, snoHandle);
}

struct RequiredDefinitionHeader;

























namespace SNOManager {
class Impl {
public:
    Impl();
    ~Impl();
    void RequireSNOs(const SNOManager::SNOCategory category, const SNOManager::SNOContainer & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority);
    void AddRequireSNOs(const SNOManager::SNOCategory, const SNOManager::SNOContainer &, SNOFilesAsyncPriority);
    void RemoveRequireSNOs(const SNOManager::SNOCategory, const SNOManager::SNOContainer &, BOOL);
    BOOL WaitForSNOs(const SNOManager::SNOContainer & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen);
    BOOL AreSNOsLoaded(const SNOManager::SNOContainer & container);
    BOOL AnySNOsPending(const SNOManager::SNOContainer & container);
    void RequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority);
    void AddRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, SNOFilesAsyncPriority ePriority);
    void RemoveRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired);
    BOOL WaitForSNOs(const SNONameArray & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen);
    BOOL AreSNOsLoaded(const SNONameArray & container);
    BOOL AnySNOsPending(const SNONameArray & container);
    void SetHasBlockedThisFrame();
    BOOL HasBlockedThisFrame();
    BOOL HasBlockedLastFrame();
    void ReloadRequired(SNOGroup snoGroup);
    void OnSNOLoaded(const SNOName & snoName, RequiredDefinitionHeader * lpDefinition);
    void OnLoadingScreenCanStart();
    BOOL DisplayLoadingScreen();
    void Update();

private:
    static const int32 MAX_SNOS_IN_PROGRESS = 2048;
    void ChangeReferences(const SNOManager::SNOContainer & newContainer, SNOManager::SNOContainer & oldContainerToUpdate, SNONameArray & tFilesToBeLoaded, SNONameArray & tFilesToBeUnloaded);
    void ChangeReferences(const SNONameArray & newContainer, SNOManager::SNOContainer & oldContainerToUpdate, SNONameArray & tFilesToBeLoaded, SNONameArray & tFilesToBeUnloaded);
    void AddReferences(const SNOManager::SNOContainer &, SNOManager::SNOContainer &, SNONameArray &);
    void AddReferences(const SNONameArray & container, SNOManager::SNOContainer & oldContainerToUpdate, SNONameArray & tFilesToBeLoaded);
    void RemoveReferences(const SNOManager::SNOContainer &, SNOManager::SNOContainer &, SNONameArray &);
    void RemoveReferences(const SNONameArray & container, SNOManager::SNOContainer & oldContainerToUpdate, SNONameArray & tFilesToBeUnloaded);
    typedef XGrowableMap<CompressedSNOName, const CompressedSNOName &, int, int> SNONameMap;
    SNOManager::SNOContainer * m_ptRequiredSNOsPerCategory[NUMBER_OF_SNO_CATEGORIES];  // 0x00
    SNOManager::Impl::SNONameMap m_tRequiredSNOs;  // 0x90
    Blizzard::Lock::RMutex m_tLock;  // 0x110
    BOOL m_bLoadingScreenEnabled;  // 0x130
    BOOL m_bLoadingScreenCanStart;  // 0x134
    BOOL m_bHasBlockedThisFrame;  // 0x138
    BOOL m_bHasBlockedLastFrame;  // 0x13c
};























static SNOManager::Impl s_tImpl;

static const uint32 sdwHashTableSizes[NUMBER_OF_SNO_CATEGORIES] = {
    16384, 1024, 64, 64, 64, 1024, 64, 128, 2048, 64, 8192, 4096, 1024, 2048, 512, 1024, 64, 512,
};

























void ReloadRequired(SNOGroup snoGroup) {
    s_tImpl.ReloadRequired(snoGroup);
}




void RequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority) {
    s_tImpl.RequireSNOs(category, container, fPurgeNoLongerRequired, ePriority);
}







void AddRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, SNOFilesAsyncPriority ePriority) {
    s_tImpl.AddRequireSNOs(category, container, ePriority);
}






void RemoveRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired) {
    s_tImpl.RemoveRequireSNOs(category, container, fPurgeNoLongerRequired);
}






BOOL WaitForSNOs(const SNONameArray & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen) {
    return s_tImpl.WaitForSNOs(container, ptSNONameReason, fIgnoreExpiredLoadingScreen);
}




















void RequireSNOs(const SNOManager::SNOCategory category, const SNOManager::SNOContainer & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority) {
    s_tImpl.RequireSNOs(category, container, fPurgeNoLongerRequired, ePriority);
}

























BOOL WaitForSNOs(const SNOManager::SNOContainer & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen) {
    return s_tImpl.WaitForSNOs(container, ptSNONameReason, fIgnoreExpiredLoadingScreen);
}






BOOL AreSNOsLoaded(const SNOManager::SNOContainer & container) {
    return s_tImpl.AreSNOsLoaded(container);
}




BOOL AnySNOsPending(const SNOManager::SNOContainer & container) {
    return s_tImpl.AnySNOsPending(container);
}




void OnSNOLoaded(const SNOName & snoName, RequiredDefinitionHeader * lpDefinition) {
    s_tImpl.OnSNOLoaded(snoName, lpDefinition);
}





void AddSNOTree(SNOManager::SNOContainer & tContainer, const SNOName & snoName, Prefetch::FilterFunc ptFilterFunc) {
    Prefetch::ChildSNOContainer tLocalContainer;

    Prefetch::GetChildSNOs(snoName, tLocalContainer, ptFilterFunc, TRUE);

    for (uint32 i = 0; i < tLocalContainer.size(); ++i) {
        int32 * pnValue = tContainer.LookupPtr(tLocalContainer[i]);
        if (pnValue) {
            (*pnValue)++;
        } else {
            tContainer.SetAt_Tracked(tLocalContainer[i], 1);
        }
    }
}











void AddSNOsTree(SNOManager::SNOContainer & tContainer, Prefetch::ChildSNOContainer & tSNOsToProcess, Prefetch::FilterFunc ptFilterFunc) {
    Prefetch::ChildSNOContainer tLocalContainer;

    Prefetch::MultiGetChildSNOs(tSNOsToProcess, tLocalContainer, ptFilterFunc);

    for (uint32 i = 0; i < tLocalContainer.size(); ++i) {
        int32 * pnValue = tContainer.LookupPtr(tLocalContainer[i]);
        if (pnValue) {
            (*pnValue)++;
        } else {
            tContainer.SetAt_Tracked(tLocalContainer[i], 1);
        }
    }
}











void OnLoadingScreenCanStart() {
    s_tImpl.OnLoadingScreenCanStart();
}



void Update() {
    s_tImpl.Update();
}



BOOL DisplayLoadingScreen() {
    return s_tImpl.DisplayLoadingScreen();
}









void SetHasBlockedThisFrame() {
    s_tImpl.SetHasBlockedThisFrame();
}



BOOL HasBlockedLastFrame() {
    return s_tImpl.HasBlockedLastFrame();
}














































Impl::Impl() : m_tRequiredSNOs(1024, NULL) {
    m_bLoadingScreenEnabled = FALSE;
    m_bLoadingScreenCanStart = FALSE;
    m_bHasBlockedThisFrame = FALSE;
    m_bHasBlockedLastFrame = FALSE;
    m_tRequiredSNOs.SetHashTableSize_Tracked(16384);
    m_tRequiredSNOs.SetGrow(XALLOC_GROW_LINEAR, 512);
    m_tRequiredSNOs.SetShrinkable(TRUE);
    for (uint32 i = 0; i < NUMBER_OF_SNO_CATEGORIES; ++i) {
        m_ptRequiredSNOsPerCategory[i] = new SNOManager::SNOContainer(sdwHashTableSizes[i] / 2, NULL);
        m_ptRequiredSNOsPerCategory[i]->SetHashTableSize_Tracked(sdwHashTableSizes[i]);
        m_ptRequiredSNOsPerCategory[i]->SetGrow(XALLOC_GROW_LINEAR, sdwHashTableSizes[i] / 4);
        m_ptRequiredSNOsPerCategory[i]->SetShrinkable(TRUE);
    }
}





Impl::~Impl() {
    for (uint32 i = 0; i < NUMBER_OF_SNO_CATEGORIES; ++i) {
        delete m_ptRequiredSNOsPerCategory[i];
    }
}




void Impl::ReloadRequired(SNOGroup snoGroup) {
    SNONameArray tNewRequirement;

    {
        Blizzard::Lock::ScopedRMutex mutex(m_tLock);


        POS pos = m_tRequiredSNOs.GetStartAssoc();
        while (pos) {
            CompressedSNOName snoName;

            m_tRequiredSNOs.GetNextAssocKey(pos, snoName);

            if (snoName.snoGroup == snoGroup) {
                tNewRequirement.push_back(snoName);
            }
        }
    }

    Prefetch::BeginAdd();
    for (uint32 i = 0; i < tNewRequirement.size(); ++i) {
        const SNOName & snoName = tNewRequirement[i];
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
        if (ptGroupDef && !ptGroupDef->SetRequired(snoName.snoHandle, TRUE)) {
            Prefetch::AddToQueue(snoName, 2);
        }
    }
    Prefetch::EndAdd();
}














BOOL Impl::WaitForSNOs(const SNOManager::SNOContainer & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen) {
    BOOL fMainThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_MAIN;
    Blizzard::Time::Millisecond dwCurMS = Blizzard::Time::Milliseconds();
    if (fMainThread) {
        Prefetch::BeginActiveWait();
    }

    BOOL fWaited = FALSE;
    BOOL fCancelWait = FALSE;

    POS pos = container.GetStartAssoc();
    while (pos && !fCancelWait) {
        CompressedSNOName snoname;
        container.GetNextAssocKey(pos, snoname);
        while (Prefetch::IsPending(snoname) && !fCancelWait) {
            if (ptSNONameReason) {
                *ptSNONameReason = snoname;
            }
            if (fMainThread) {
                Prefetch::Update(FALSE);
                if (Blizzard::Time::Milliseconds() - dwCurMS > 16) {
                    dwCurMS = Blizzard::Time::Milliseconds();
                    if (CGameLoadingScreenIsEnabled()) {
                        CGameProgressLoadingScreen(TRUE);
                        fCancelWait = CGameLoadingScreenHasExpired() && !fIgnoreExpiredLoadingScreen;
                    }
                }
            }
            fWaited = TRUE;
            m_bHasBlockedThisFrame = fWaited;
            Sleep(1);
        }
    }

    if (fMainThread) {
        Prefetch::EndActiveWait();
    }

    return fWaited;
}

























BOOL Impl::AreSNOsLoaded(const SNOManager::SNOContainer & container) {
    POS pos = container.GetStartAssoc();
    while (pos) {
        CompressedSNOName snoname;
        container.GetNextAssocKey(pos, snoname);
        if (!Prefetch::IsLoaded(snoname)) {
            return FALSE;
        }
    }
    return TRUE;
}







BOOL Impl::AnySNOsPending(const SNOManager::SNOContainer & container) {
    POS pos = container.GetStartAssoc();
    while (pos) {
        CompressedSNOName snoname;
        container.GetNextAssocKey(pos, snoname);
        if (Prefetch::IsPending(snoname)) {
            return TRUE;
        }
    }
    return FALSE;
}







void Impl::RequireSNOs(const SNOManager::SNOCategory category, const SNOManager::SNOContainer & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority) {
    SNONameArray tNewFilesToBeLoaded;
    SNONameArray tFilesToBeUnloaded;

    {
        Blizzard::Lock::ScopedRMutex mutex(m_tLock);
        ChangeReferences(container, *m_ptRequiredSNOsPerCategory[category], tNewFilesToBeLoaded, tFilesToBeUnloaded);
    }

    for (uint32 i = 0; i < tFilesToBeUnloaded.size(); ++i) {
        const SNOName & snoName = tFilesToBeUnloaded[i];
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
        if (ptGroupDef) {
            if (ptGroupDef->SetRequired(snoName.snoHandle, FALSE) && fPurgeNoLongerRequired) {
                ptGroupDef->Purge(snoName.snoHandle, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
            }
        }
    }

    if (tNewFilesToBeLoaded.size()) {
        Prefetch::BeginAdd();
        for (uint32 i = 0; i < tNewFilesToBeLoaded.size(); ++i) {
            const SNOName & snoName = tNewFilesToBeLoaded[i];
            GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
            if (ptGroupDef && !ptGroupDef->SetRequired(snoName.snoHandle, TRUE)) {
                Prefetch::AddToQueue(snoName, ePriority);
            }
        }
        Prefetch::EndAdd();
    }
}





























































































































BOOL Impl::WaitForSNOs(const SNONameArray & container, SNOName * ptSNONameReason, BOOL fIgnoreExpiredLoadingScreen) {
    BOOL fMainThread = ThreadGlobals::Get().m_nThreadIndex == THREADINDEX_MAIN;
    Blizzard::Time::Millisecond dwCurMS = Blizzard::Time::Milliseconds();
    if (fMainThread) {
        Prefetch::BeginActiveWait();
    }

    BOOL fWaited = FALSE;
    BOOL fCancelWait = FALSE;

    uint32 nCount = container.size();
    for (uint32 i = 0; i < nCount && !fCancelWait; ++i) {
        CompressedSNOName snoname(container[i]);
        while (Prefetch::IsPending(snoname) && !fCancelWait) {
            if (ptSNONameReason) {
                *ptSNONameReason = snoname;
            }
            if (fMainThread) {
                Prefetch::Update(FALSE);
                if (Blizzard::Time::Milliseconds() - dwCurMS > 16) {
                    dwCurMS = Blizzard::Time::Milliseconds();
                    if (CGameLoadingScreenIsEnabled()) {
                        CGameProgressLoadingScreen(TRUE);
                        fCancelWait = CGameLoadingScreenHasExpired() && !fIgnoreExpiredLoadingScreen;
                    }
                }
            }
            fWaited = TRUE;
            Sleep(1);
            m_bHasBlockedThisFrame = fWaited;
        }
    }

    if (fMainThread) {
        Prefetch::EndActiveWait();
    }

    return fWaited;
}





















































void Impl::RequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired, SNOFilesAsyncPriority ePriority) {
    SNONameArray tNewFilesToBeLoaded;
    SNONameArray tFilesToBeUnloaded;

    {
        Blizzard::Lock::ScopedRMutex mutex(m_tLock);
        ChangeReferences(container, *m_ptRequiredSNOsPerCategory[category], tNewFilesToBeLoaded, tFilesToBeUnloaded);
    }

    for (uint32 i = 0; i < tFilesToBeUnloaded.size(); ++i) {
        const SNOName & snoName = tFilesToBeUnloaded[i];
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
        if (ptGroupDef) {
            if (ptGroupDef->SetRequired(snoName.snoHandle, FALSE) && fPurgeNoLongerRequired) {
                ptGroupDef->Purge(snoName.snoHandle, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
            }
        }
    }

    if (tNewFilesToBeLoaded.size()) {
        Prefetch::BeginAdd();
        for (uint32 i = 0; i < tNewFilesToBeLoaded.size(); ++i) {
            const SNOName & snoName = tNewFilesToBeLoaded[i];
            GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
            if (ptGroupDef && !ptGroupDef->SetRequired(snoName.snoHandle, TRUE)) {
                Prefetch::AddToQueue(snoName, ePriority);
            }
        }
        Prefetch::EndAdd();
    }
}






































void Impl::AddRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, SNOFilesAsyncPriority ePriority) {
    SNONameArray tNewFilesToBeLoaded;

    {
        Blizzard::Lock::ScopedRMutex mutex(m_tLock);
        AddReferences(container, *m_ptRequiredSNOsPerCategory[category], tNewFilesToBeLoaded);
    }

    if (tNewFilesToBeLoaded.size()) {
        Prefetch::BeginAdd();
        for (uint32 i = 0; i < tNewFilesToBeLoaded.size(); ++i) {
            const SNOName & snoName = tNewFilesToBeLoaded[i];
            GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
            if (ptGroupDef && !ptGroupDef->SetRequired(snoName.snoHandle, TRUE)) {
                Prefetch::AddToQueue(snoName, ePriority);
            }
        }
        Prefetch::EndAdd();
    }
}
























void Impl::RemoveRequireSNOs(const SNOManager::SNOCategory category, const SNONameArray & container, BOOL fPurgeNoLongerRequired) {
    SNONameArray tFilesToBeUnloaded;

    {
        Blizzard::Lock::ScopedRMutex mutex(m_tLock);
        RemoveReferences(container, *m_ptRequiredSNOsPerCategory[category], tFilesToBeUnloaded);
    }

    for (uint32 i = 0; i < tFilesToBeUnloaded.size(); ++i) {
        const SNOName & snoName = tFilesToBeUnloaded[i];
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
        if (ptGroupDef) {
            if (ptGroupDef->SetRequired(snoName.snoHandle, FALSE) && fPurgeNoLongerRequired) {
                ptGroupDef->Purge(snoName.snoHandle, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
            }
        }
    }
}

























void Impl::OnSNOLoaded(const SNOName & snoName, RequiredDefinitionHeader * lpDefinition) {
    Blizzard::Lock::ScopedRMutex mutex(m_tLock);

    if (m_tRequiredSNOs.LookupPtr(CompressedSNOName(snoName))) {



        GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
        if (ptGroupDef) {

            ptGroupDef->SetRequired(lpDefinition, TRUE);
        }
    }
}









void Impl::OnLoadingScreenCanStart() {
    m_bLoadingScreenCanStart = TRUE;
    Update();
}

BOOL Impl::HasBlockedThisFrame() {
    return m_bHasBlockedThisFrame;
}





void Impl::SetHasBlockedThisFrame() {
    m_bHasBlockedThisFrame = TRUE;
}



BOOL Impl::HasBlockedLastFrame() {
    return m_bHasBlockedLastFrame;
}



void Impl::Update() {
    if (m_bLoadingScreenCanStart) {
        if (Prefetch::HasPriorityPending(TRUE)) {
            m_bLoadingScreenEnabled = TRUE;
        }
        m_bLoadingScreenCanStart = FALSE;
    }
    m_bHasBlockedLastFrame = m_bHasBlockedThisFrame;
    m_bHasBlockedThisFrame = FALSE;
}








BOOL Impl::DisplayLoadingScreen() {
    if (m_bLoadingScreenEnabled) {
        m_bLoadingScreenEnabled = Prefetch::HasPriorityPending(TRUE);
        if (m_bLoadingScreenEnabled) {
            return TRUE;
        }
    }
    return m_bLoadingScreenCanStart ? TRUE : FALSE;
}






































































































void Impl::ChangeReferences(const SNONameArray & newContainer, SNOManager::SNOContainer & oldContainerToUpdate, SNONameArray & tFilesToBeLoaded, SNONameArray & tFilesToBeUnloaded) {




    SNOManager::SNOContainer newContainerMap;
    newContainerMap.SetHashTableSize_Tracked(1024);


    uint32 nCount = newContainer.size();
    for (uint32 i = 0; i < nCount; ++i) {

        CompressedSNOName snoName(newContainer[i]);
        newContainerMap.SetAt_Tracked(snoName, 1);
        int32 * pnRefCount = oldContainerToUpdate.LookupPtr(snoName);
        if (!pnRefCount) {




            oldContainerToUpdate.SetAt_Tracked(snoName, 0);

            pnRefCount = m_tRequiredSNOs.LookupPtr(snoName);
            if (!pnRefCount) {



                tFilesToBeLoaded.push_back(snoName);

                m_tRequiredSNOs.SetAt_Tracked(snoName, 1);
            } else {

                (*pnRefCount)++;
            }
        }
    }

    POS pos = oldContainerToUpdate.GetStartAssoc();
    while (pos) {
        CompressedSNOName snoName;

        oldContainerToUpdate.GetNextAssocKey(pos, snoName);
        int32 * pnRefCount = newContainerMap.LookupPtr(snoName);
        if (!pnRefCount) {




            oldContainerToUpdate.RemoveAt(snoName);

            pnRefCount = m_tRequiredSNOs.LookupPtr(snoName);



            if (!pnRefCount) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }

            (*pnRefCount)--;
            if (*pnRefCount == 0) {

                tFilesToBeUnloaded.push_back(snoName);
                m_tRequiredSNOs.RemoveAt(snoName);
            }
        }
    }
}

// Lookups without a local + early `continue` (DWARF: pnRefCount 1243/1270 in the loop-body block; push_back/++ at 12 spaces).
void Impl::ChangeReferences(
    const SNOManager::SNOContainer & newContainer,
    SNOManager::SNOContainer & oldContainerToUpdate,
    SNONameArray & tFilesToBeLoaded,
    SNONameArray & tFilesToBeUnloaded)
{

    POS pos = newContainer.GetStartAssoc();
    CompressedSNOName snoName;
    while (pos)
    {
        newContainer.GetNextAssocKey(pos, snoName);
        if (oldContainerToUpdate.LookupPtr(snoName))
        {
            continue;
        }


        oldContainerToUpdate.SetAt_Tracked(snoName, 0);

        int32 * pnRefCount = m_tRequiredSNOs.LookupPtr(snoName);
        if (!pnRefCount)
        {

            tFilesToBeLoaded.push_back(snoName);

            m_tRequiredSNOs.SetAt_Tracked(snoName, 1);
        }
        else
        {
            (*pnRefCount)++;
        }
    }


    pos = oldContainerToUpdate.GetStartAssoc();
    while (pos)
    {
        oldContainerToUpdate.GetNextAssocKey(pos, snoName);
        if (newContainer.LookupPtr(snoName))
        {
            continue;
        }


        oldContainerToUpdate.RemoveAt(snoName);

        int32 * pnRefCount = m_tRequiredSNOs.LookupPtr(snoName);


        if (!pnRefCount) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }


        (*pnRefCount)--;
        if (*pnRefCount == 0)
        {
            tFilesToBeUnloaded.push_back(snoName);
            m_tRequiredSNOs.RemoveAt(snoName);
        }
    }
}
































void Impl::AddReferences(const SNONameArray & container, SNOManager::SNOContainer & oldContainerToUpdate, SNONameArray & tFilesToBeLoaded) {




    uint32 nCount = container.size();
    for (uint32 i = 0; i < nCount; ++i) {

        CompressedSNOName snoName(container[i]);


        oldContainerToUpdate.SetAt_Tracked(snoName, 0);

        int32 * pnRefCount = m_tRequiredSNOs.LookupPtr(snoName);
        if (!pnRefCount) {



            tFilesToBeLoaded.push_back(snoName);

            m_tRequiredSNOs.SetAt_Tracked(snoName, 1);
        } else {

            (*pnRefCount)++;
        }
    }
}
































void Impl::RemoveReferences(const SNONameArray & container, SNOManager::SNOContainer & oldContainerToUpdate, SNONameArray & tFilesToBeUnloaded) {




    uint32 nCount = container.size();
    for (uint32 i = 0; i < nCount; ++i) {

        CompressedSNOName snoName(container[i]);


        oldContainerToUpdate.RemoveAt(snoName);

        int32 * pnRefCount = m_tRequiredSNOs.LookupPtr(snoName);


        if (!pnRefCount) { DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL); }


        (*pnRefCount)--;
        if (*pnRefCount == 0) {

            tFilesToBeUnloaded.push_back(snoName);
            m_tRequiredSNOs.RemoveAt(snoName);
        }
    }
}








}  // namespace SNOManager
