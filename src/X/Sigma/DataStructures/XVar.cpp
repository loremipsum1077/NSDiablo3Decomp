#include "XVar.h"
#include "DataStructuresExternal.h"
#include <stdlib.h>
#include "../../../Contrib/Contrib/blz/include/blz/external_string.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"















// The external (debug UI) interface. The DWARF only has forward declarations
// of these classes, so the original header is unknown and everything except
// the class names and the vtable slots called from this file is invented.
class ExternalVarScope {
public:
    virtual void Release() = 0;  // slot 0
};
class ExternalVarBase {
public:
    virtual void Release() = 0;  // slot 0
    virtual void Unknown1() = 0;
    virtual void Unknown2() = 0;
};
template <typename T>
class ExternalVar : public ExternalVarBase {
public:
    virtual void Set(T value) = 0;  // slot 3
};
class XVarExternalInterfaceFactory {
public:
    virtual ~XVarExternalInterfaceFactory();  // slots 0, 1 (assumed)
    virtual ExternalVarScope* CreateScope(ExternalVarScope* ptParentScope, const char* pszName) = 0;                // slot 2
    virtual ExternalVarBase* CreateBool(ExternalVarScope* ptScope, const char* pszName, bool bValue) = 0;           // slot 3
    virtual ExternalVarBase* CreateInt32(ExternalVarScope* ptScope, const char* pszName, int32 nValue) = 0;         // slot 4
    virtual ExternalVarBase* CreateUint32(ExternalVarScope* ptScope, const char* pszName, uint32 uValue) = 0;       // slot 5
    virtual ExternalVarBase* CreateInt64(ExternalVarScope* ptScope, const char* pszName, int64 nValue) = 0;         // slot 6
    virtual ExternalVarBase* CreateUint64(ExternalVarScope* ptScope, const char* pszName, uint64 uValue) = 0;       // slot 7
    virtual ExternalVarBase* CreateFloat(ExternalVarScope* ptScope, const char* pszName, float fValue) = 0;         // slot 8
    virtual ExternalVarBase* CreateString(ExternalVarScope* ptScope, const char* pszName, const char* pszValue) = 0;  // slot 9
};

class XVarScopeInternal {
public:
    ~XVarScopeInternal();
    void AddChildScopeInternal(XVarScopeInternal& tChildScope);
    void RemoveChildScopeInternal(XVarScopeInternal& tChildScope);
    void ComposeFullName(blz::external_string&) const;
    void ComposeFullName(blz::string& tFullName) const;
    void AddScope(XVarScope& tScope);
    void RemoveScope(XVarScope& tScope);
    void AddVar(XVarBase& tVar);
    void RemoveVar(XVarBase& tVar);
    XVarScopeInternal* FindChildScope(const char* pszName, size_t length);
    XVarBase* FindVar(const char* pszName, size_t length);
    void CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory, bool bRecurseChildren);
    void DestroyExternalInterface(bool bRecurseChildren);

    ExternalVarScope* GetExternalScope() { return m_ptExternalScope; }

private:
    XVarScopeInternal(XVarScopeInternal* pParentScope, const char* pszName, bool bIsRoot);

    bool m_bIsRootScope;
    char m_szName[64];
    XVarScopeInternal* m_pParentScope;
    blz::list_link m_tParentScopeLink;
    blz::intrusive_list<XVarScopeInternal, &XVarScopeInternal::m_tParentScopeLink> m_tChildScopes;
    blz::intrusive_list<XVarScope, &XVarScope::m_tScopeInternalLink> m_tPublicScopes;
    blz::intrusive_list<XVarBase, &XVarBase::m_tParentScopeVarLink> m_tVars;
    ExternalVarScope* m_ptExternalScope;

    // Not in the DWARF; XVarRegistry constructs these and walks the lists.
    friend class XVarRegistry;
};
















XVarRegistry* s_ptDefaultVarRegistry = NULL;



static XVarRegistry* sGetDefaultRegistry() {
    if (s_ptDefaultVarRegistry == NULL) {


        s_ptDefaultVarRegistry = new XVarRegistry();
    }
    return s_ptDefaultVarRegistry;
}







bool XVarInitialize() {


    sGetDefaultRegistry();
    return true;
}



void XVarDispose() {
    delete s_ptDefaultVarRegistry;
    s_ptDefaultVarRegistry = NULL;
}




XVarRegistry* XVarGetDefaultRegistry() {
    return s_ptDefaultVarRegistry;
}





XVarScope::XVarScope(const char* pszName, XVarRegistry* ptRegistry)
    : m_pScopeInternal(NULL) {
    if (ptRegistry == NULL) {

        ptRegistry = sGetDefaultRegistry();
    }
    m_ptRegistry = ptRegistry;
    m_ptRegistry->_AddScope(*this, NULL, pszName);
}


XVarScope::XVarScope(XVarScope& tParentScope, const char* pszName)
    : m_pScopeInternal(NULL)
    , m_ptRegistry(tParentScope.m_ptRegistry) {
    m_ptRegistry->_AddScope(*this, &tParentScope, pszName);
}






XVarScope::~XVarScope() {

    if (m_ptRegistry != NULL) {

        m_ptRegistry->_RemoveScope(*this);
    }
}














// Stripped by the linker (DWARF low_pc 0); written from its DWARF and checked only against the recorded size.
void XVarScope::GetFullName(blz::string& tFullName) const {

    if (m_pScopeInternal != NULL) {

        m_pScopeInternal->ComposeFullName(tFullName);
    }
}



// Stripped by the linker (DWARF low_pc 0); written from its DWARF and checked only against the recorded size.
ExternalVarScope* XVarScope::GetExternalScope() {

    return m_pScopeInternal != NULL ? m_pScopeInternal->GetExternalScope() : NULL;
}



// Inlined into the seven derived constructors; needs blz::list_link / blz::function / blz::atomic constructors.
XVarBase::XVarBase()
    : m_pParentScope(NULL)
    , m_ptExternalInterface(NULL)
    , m_uDirtyFlags(0)
    , m_ptRegistry(NULL) {
}


// The deleting destructor (the class is abstract, so it is one trap instruction) differs by toolchain: 0xe7ffdefe in the original, brk #1 here.

XVarBase::~XVarBase() {

    if (m_ptRegistry != NULL) {

        m_ptRegistry->_RemoveVar(*this);
    }
}



void XVarBase::DestroyExternalInterface() {

    if (m_ptExternalInterface != NULL) {

        ExternalVarBase* ptExternalInterface = m_ptExternalInterface;
        m_ptExternalInterface = NULL;
        ptExternalInterface->Release();
    }
}



void XVarBase::_DerivedConstructionInitInternal(XVarScope& tParentScope, const char* pszName) {




    XVarRegistry* ptRegistry = tParentScope.GetRegistry();
    if (ptRegistry != NULL) {
        m_ptRegistry = ptRegistry;
        m_ptRegistry->_AddVar(*this, tParentScope, pszName);
    }
}














// Inlined into every Set/Increment/Decrement; needs blz::atomic::fetch_or, blz::lock_guard, blz::intrusive_list::push_back.
void XVarBase::MarkDirty(uint32 uDirtyFlags) {
    if (m_ptRegistry == NULL) {
        return;
    }










    uint32 uPrevDirtyFlags = m_uDirtyFlags.fetch_or(uDirtyFlags);
    if (uPrevDirtyFlags == 0) {

        m_ptRegistry->_AddToDirtyList(*this);
    }
}

















blz::string XVarBase::GetFullName() const {

    blz::string retString;
    if (m_pParentScope != NULL) {

        m_pParentScope->ComposeFullName(retString);

        retString.push_back('.');
    }
    retString.append(m_szName);
    return retString;
}




void XVarBase::RegisterDirtyCallback(const blz::function<void()>& callback) {


    m_dirtyCallback = callback;
}











XVarBool::XVarBool(XVarScope& tParentScope, const char* pszName, bool bInitialValue)



    : XVarBase()
    , m_val(bInitialValue) {

    _DerivedConstructionInitInternal(tParentScope, pszName);
}



bool XVarBool::Parse(const char* pszValue, uint32 uDirtyFlagsIfNecessary) {
    bool bValue;
    if (Blizzard::String::EqualI(pszValue, "true")) {
        bValue = true;
    } else if (Blizzard::String::EqualI(pszValue, "false")) {
        bValue = false;
    } else {
        bValue = strtoul(pszValue, NULL, 10) != 0;
    }










    Set(bValue, uDirtyFlagsIfNecessary);
    return true;
}


blz::string XVarBool::ToString() {

    return Get() ? "true" : "false";
}



bool XVarBool::Set(bool bNewVal, uint32 uDirtyFlagsIfNecessary) {



    bool bOldVal = m_val.exchange(bNewVal);
    if (uDirtyFlagsIfNecessary != 0 && bOldVal != bNewVal) {

        MarkDirty(uDirtyFlagsIfNecessary);
    }
    return true;
}



bool XVarBool::Get() const {

    return m_val;
}



void XVarBool::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory) {

    if (ptExternalInterfaceFactory != NULL) {



        m_ptExternalInterface = ptExternalInterfaceFactory->CreateBool(m_pParentScope->GetExternalScope(), m_szName, m_val);
    }
}



void XVarBool::PushToExternalInterface() {

    static_cast<ExternalVar<bool>*>(m_ptExternalInterface)->Set(Get());
}




XVarInt32::XVarInt32(XVarScope& tParentScope, const char* pszName, int32 nInitialValue)



    : XVarBase()
    , m_val(nInitialValue) {

    _DerivedConstructionInitInternal(tParentScope, pszName);
}



bool XVarInt32::Parse(const char* pszValue, uint32 uDirtyFlagsIfNecessary) {



    Set(atoi(pszValue), uDirtyFlagsIfNecessary);
    return true;
}


blz::string XVarInt32::ToString() {

    return blz::to_string(Get());
}



bool XVarInt32::Set(int32 nNewVal, uint32 uDirtyFlagsIfNecessary) {



    int32 nOldVal = m_val.exchange(nNewVal);
    if (uDirtyFlagsIfNecessary != 0 && nOldVal != nNewVal) {

        MarkDirty(uDirtyFlagsIfNecessary);
    }
    return true;
}



int32 XVarInt32::Get() const {

    return m_val;
}























void XVarInt32::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory) {

    if (ptExternalInterfaceFactory != NULL) {



        m_ptExternalInterface = ptExternalInterfaceFactory->CreateInt32(m_pParentScope->GetExternalScope(), m_szName, m_val);
    }
}



void XVarInt32::PushToExternalInterface() {

    static_cast<ExternalVar<int32>*>(m_ptExternalInterface)->Set(Get());
}




XVarUint32::XVarUint32(XVarScope& tParentScope, const char* pszName, uint32 uInitialValue)



    : XVarBase()
    , m_val(uInitialValue) {

    _DerivedConstructionInitInternal(tParentScope, pszName);
}



bool XVarUint32::Parse(const char* pszValue, uint32 uDirtyFlagsIfNecessary) {



    Set((uint32)strtoul(pszValue, NULL, 10), uDirtyFlagsIfNecessary);
    return true;
}


blz::string XVarUint32::ToString() {

    return blz::to_string(Get());
}



bool XVarUint32::Set(uint32 uNewVal, uint32 uDirtyFlagsIfNecessary) {



    uint32 uOldVal = m_val.exchange(uNewVal);
    if (uDirtyFlagsIfNecessary != 0 && uOldVal != uNewVal) {

        MarkDirty(uDirtyFlagsIfNecessary);
    }
    return true;
}



uint32 XVarUint32::Get() const {

    return m_val;
}



uint32 XVarUint32::Increment(uint32 uAmount) {


    uint32 result = (m_val += uAmount);
    MarkDirty(DIRTY_DEFAULT);
    return result;
}



uint32 XVarUint32::Decrement(uint32 uAmount) {


    uint32 result = (m_val -= uAmount);
    MarkDirty(DIRTY_DEFAULT);
    return result;
}



void XVarUint32::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory) {

    if (ptExternalInterfaceFactory != NULL) {



        m_ptExternalInterface = ptExternalInterfaceFactory->CreateUint32(m_pParentScope->GetExternalScope(), m_szName, m_val);
    }
}



void XVarUint32::PushToExternalInterface() {

    static_cast<ExternalVar<uint32>*>(m_ptExternalInterface)->Set(Get());
}





XVarInt64::XVarInt64(XVarScope& tParentScope, const char* pszName, int64 nInitialValue)



    : XVarBase()
    , m_val(nInitialValue) {

    _DerivedConstructionInitInternal(tParentScope, pszName);
}



bool XVarInt64::Parse(const char* pszValue, uint32 uDirtyFlagsIfNecessary) {



    Set(strtoll(pszValue, NULL, 10), uDirtyFlagsIfNecessary);
    return true;
}


blz::string XVarInt64::ToString() {

    return blz::to_string(Get());
}



bool XVarInt64::Set(int64 nNewVal, uint32 uDirtyFlagsIfNecessary) {



    int64 nOldVal = m_val.exchange(nNewVal);
    if (uDirtyFlagsIfNecessary != 0 && nOldVal != nNewVal) {

        MarkDirty(uDirtyFlagsIfNecessary);
    }
    return true;
}



int64 XVarInt64::Get() const {

    return m_val;
}























void XVarInt64::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory) {

    if (ptExternalInterfaceFactory != NULL) {



        m_ptExternalInterface = ptExternalInterfaceFactory->CreateInt64(m_pParentScope->GetExternalScope(), m_szName, m_val);
    }
}



void XVarInt64::PushToExternalInterface() {

    static_cast<ExternalVar<int64>*>(m_ptExternalInterface)->Set(Get());
}




XVarUint64::XVarUint64(XVarScope& tParentScope, const char* pszName, uint64 uInitialValue)



    : XVarBase()
    , m_val(uInitialValue) {

    _DerivedConstructionInitInternal(tParentScope, pszName);
}



bool XVarUint64::Parse(const char* pszValue, uint32 uDirtyFlagsIfNecessary) {



    Set(strtoull(pszValue, NULL, 10), uDirtyFlagsIfNecessary);
    return true;
}


blz::string XVarUint64::ToString() {

    return blz::to_string(Get());
}



bool XVarUint64::Set(uint64 uNewVal, uint32 uDirtyFlagsIfNecessary) {



    uint64 uOldVal = m_val.exchange(uNewVal);
    if (uDirtyFlagsIfNecessary != 0 && uOldVal != uNewVal) {

        MarkDirty(uDirtyFlagsIfNecessary);
    }
    return true;
}



uint64 XVarUint64::Get() const {

    return m_val;
}



uint64 XVarUint64::Increment(uint64 uAmount) {


    uint64 result = (m_val += uAmount);
    MarkDirty(DIRTY_DEFAULT);
    return result;
}













void XVarUint64::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory) {

    if (ptExternalInterfaceFactory != NULL) {



        m_ptExternalInterface = ptExternalInterfaceFactory->CreateUint64(m_pParentScope->GetExternalScope(), m_szName, m_val);
    }
}



void XVarUint64::PushToExternalInterface() {

    static_cast<ExternalVar<uint64>*>(m_ptExternalInterface)->Set(Get());
}




XVarFloat::XVarFloat(XVarScope& tParentScope, const char* pszName, float fInitialValue)



    : XVarBase()
    , m_val(fInitialValue) {

    _DerivedConstructionInitInternal(tParentScope, pszName);
}



bool XVarFloat::Parse(const char* pszValue, uint32 uDirtyFlagsIfNecessary) {



    Set((float)atof(pszValue), uDirtyFlagsIfNecessary);
    return true;
}


blz::string XVarFloat::ToString() {

    return blz::to_string(Get());
}



bool XVarFloat::Set(float fNewVal, uint32 uDirtyFlagsIfNecessary) {



    float fOldVal = m_val.exchange(fNewVal);
    if (uDirtyFlagsIfNecessary != 0 && fOldVal != fNewVal) {

        MarkDirty(uDirtyFlagsIfNecessary);
    }
    return true;
}



float XVarFloat::Get() const {

    return m_val;
}



void XVarFloat::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory) {

    if (ptExternalInterfaceFactory != NULL) {



        m_ptExternalInterface = ptExternalInterfaceFactory->CreateFloat(m_pParentScope->GetExternalScope(), m_szName, m_val);
    }
}



void XVarFloat::PushToExternalInterface() {

    static_cast<ExternalVar<float>*>(m_ptExternalInterface)->Set(Get());
}




XVarString::XVarString(XVarScope& tParentScope, const char* pszName, const char* pszInitialValue)



    : XVarBase() {

    if (pszInitialValue != NULL) {

        m_val = pszInitialValue;
    }

    _DerivedConstructionInitInternal(tParentScope, pszName);
}



bool XVarString::Parse(const char* pszValue, uint32 uDirtyFlagsIfNecessary) {
    Set(pszValue, uDirtyFlagsIfNecessary);
    return true;
}





blz::string XVarString::ToString() {
    return Get();
}




bool XVarString::Set(const char* pszNewVal, uint32 uDirtyFlagsIfNecessary) {



    {
        blz::lock_guard<blz::recursive_mutex> tScope(m_mutex);
        m_val = pszNewVal;
    }

    if (uDirtyFlagsIfNecessary != 0) {

        MarkDirty(uDirtyFlagsIfNecessary);
    }
    return true;
}




blz::string XVarString::Get() const {

    blz::lock_guard<blz::recursive_mutex> tScope(m_mutex);
    return m_val;
}



void XVarString::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory) {

    if (ptExternalInterfaceFactory != NULL) {



        m_ptExternalInterface = ptExternalInterfaceFactory->CreateString(m_pParentScope->GetExternalScope(), m_szName, m_val.c_str());
    }
}



void XVarString::PushToExternalInterface() {

    static_cast<ExternalVar<const char*>*>(m_ptExternalInterface)->Set(Get().c_str());
}



// Inlined into XVarInitialize and XVarScope::XVarScope; needs blz::intrusive_list / blz::vector constructors.
XVarRegistry::XVarRegistry()
    : m_pRootScopeInternal(NULL)
    , m_ptExternalInterfaceFactory(NULL) {


    m_pRootScopeInternal = new XVarScopeInternal(NULL, "__root__", true);
}



XVarRegistry::~XVarRegistry() {
    delete m_pRootScopeInternal;
    m_pRootScopeInternal = NULL;
}



void XVarRegistry::_AddScope(XVarScope& tScope, XVarScope* ptParentScope, const char* pszName) {





    blz::lock_guard<blz::recursive_mutex> scopedMutex(m_tLock);

    XVarScopeInternal* pParentInternal = ptParentScope != NULL ? ptParentScope->m_pScopeInternal : m_pRootScopeInternal;
    XVarScopeInternal* pScopeInternal = NULL;

    blz::intrusive_list_iterator<XVarScopeInternal, &XVarScopeInternal::m_tParentScopeLink> it = blz::find_if(pParentInternal->m_tChildScopes.begin(), pParentInternal->m_tChildScopes.end(),
        [pszName](const XVarScopeInternal& tChildScope) { return Blizzard::String::Equal(pszName, tChildScope.m_szName); });


    if (it != pParentInternal->m_tChildScopes.end()) {

        pScopeInternal = &*it;
    }

    if (pScopeInternal == NULL) {

        pScopeInternal = new XVarScopeInternal(pParentInternal, pszName, false);
        if (m_ptExternalInterfaceFactory != NULL) {


            pScopeInternal->CreateExternalInterface(m_ptExternalInterfaceFactory, false);
        }
    }

    pScopeInternal->AddScope(tScope);
}


// Inlined into XVarScope::~XVarScope; needs blz::lock_guard, blz::intrusive_list::erase.
void XVarRegistry::_RemoveScope(XVarScope& tScope) {


    blz::lock_guard<blz::recursive_mutex> scopedMutex(m_tLock);

    if (tScope.m_pScopeInternal != NULL) {

        tScope.m_pScopeInternal->RemoveScope(tScope);
    }
}



void XVarRegistry::_AddVar(XVarBase& tVar, XVarScope& tScope, const char* pszName) {







    blz::lock_guard<blz::recursive_mutex> scopedMutex(m_tLock);

    // The result is unused in the binary; presumably a duplicate-name assert that is compiled out.
    for (blz::intrusive_list_iterator<XVarBase, &XVarBase::m_tParentScopeVarLink> it = tScope.m_pScopeInternal->m_tVars.begin(); it != tScope.m_pScopeInternal->m_tVars.end(); ++it) {

        Blizzard::String::Compare(pszName, it->Name());
    }

    Blizzard::String::Copy(tVar.m_szName, pszName, sizeof(tVar.m_szName));



    tScope.m_pScopeInternal->AddVar(tVar);

    _ApplyRegisteredCommandLines(tVar);

    if (m_ptExternalInterfaceFactory != NULL) {

        tVar.CreateExternalInterface(m_ptExternalInterfaceFactory);
    }
}





void XVarRegistry::_RemoveVar(XVarBase& tVar) {



    blz::lock_guard<blz::recursive_mutex> scopedMutex(m_tLock);

    tVar.DestroyExternalInterface();

    if (tVar.m_pParentScope != NULL) {

        tVar.m_pParentScope->RemoveVar(tVar);
    }


    if (tVar.m_uDirtyFlags != 0) {

        blz::lock_guard<blz::mutex> tLock(m_tDirtyListMutex);
        if (tVar.m_tDirtyLink.is_linked()) {

            m_tDirtyList.erase(tVar);
        }
        tVar.m_uDirtyFlags = 0;
    }
}



void XVarRegistry::_ApplyRegisteredCommandLines(XVarBase& tVar) {



    if (m_tRegisteredCmdLines.empty()) {
        return;
    }

    blz::string fullName = tVar.GetFullName();



    for (blz::string* it = m_tRegisteredCmdLines.begin(); it != m_tRegisteredCmdLines.end(); ++it) {

        const char* pszVarName = it->c_str();


        while ((pszVarName = Blizzard::String::Find(pszVarName, fullName.c_str())) != NULL) {



            if ((pszVarName == it->c_str() || pszVarName[-1] == ' ') && pszVarName[fullName.length()] == '=') {

                tVar.Parse(pszVarName + fullName.length() + 1, 0);
            }



            pszVarName += fullName.length();
        }
    }
}




// NON_MATCHING: the original calls blz::vector<blz::string>::emplace_back<const char*&> out of line (204 vs 72 bytes). -Rpass-missed=inline shows clang defers inlining basic_string::assign(const T*, size_type) into _assign_rv ("increases the cost of inlining _assign_rv in other contexts"), so the still-small move constructor inlines into _uninitialized_move, vector::_transfer grows to cost 480 (> 250) and stays out of line, and emplace_back inlines here. In the original CU the move constructor C2EOS5_ is emitted out of line with _assign_rv and assign inlined (DWARF cu_075414e9), and no second caller of either exists, so the cost difference is in the blz string.h/vector.h bodies; fixing it needs a shared-header change with an includer check.
void XVarRegistry::RegisterCmdLine(
    const char* pszCmdLine)
{
    blz::lock_guard<blz::recursive_mutex> scopedMutex(m_tLock);
    m_tRegisteredCmdLines.emplace_back(pszCmdLine);

    ParseCmdLine(pszCmdLine, XVarBase::DIRTY_EXTERNAL_INTERFACE);
}



void XVarRegistry::ParseCmdLine(
    const char* pszCmdLine,
    uint32 uDirtyFlagsIfNecessary) {



    blz::embedded_string<4096> tWorkingString;

    const char* pszCurPos = pszCmdLine;
    const char* pszSpace;
    do {
        pszSpace = Blizzard::String::Find(pszCurPos, ' ');
        {
            const char* pszEqual = Blizzard::String::Find(pszCurPos, '=');
            if (pszEqual != NULL && (pszSpace == NULL || pszEqual < pszSpace)) {

                if (pszSpace != NULL) {


                    tWorkingString.assign(pszCurPos, pszSpace - pszCurPos);
                } else {



                    tWorkingString.assign(pszCurPos);
                }

                size_t uWorkingEqualPos = pszEqual - pszCurPos;
                tWorkingString[uWorkingEqualPos] = '\0';

                XVarBase* ptBaseVar = FindVar(tWorkingString.c_str());
                if (ptBaseVar != NULL && tWorkingString.size() > uWorkingEqualPos + 1) {

                    ptBaseVar->Parse(&tWorkingString[uWorkingEqualPos + 1], uDirtyFlagsIfNecessary);
                }
            }
        }
        pszCurPos = pszSpace + 1;
    } while (pszSpace != NULL);
}



XVarBase* XVarRegistry::FindVar(
    const char* pszName) {


    blz::lock_guard<blz::recursive_mutex> scopedMutex(m_tLock);

    XVarScopeInternal* pScope = m_pRootScopeInternal;
    const char* pszPrev = pszName;
    const char* pszNext;
    while (pScope != NULL && (pszNext = Blizzard::String::Find(pszPrev, '.')) != NULL) {







        pScope = pScope->FindChildScope(pszPrev, pszNext - pszPrev);
        pszPrev = pszNext + 1;
    }

    return pScope != NULL ? pScope->FindVar(pszPrev, Blizzard::String::Length(pszPrev)) : NULL;
}



































































// Inlined into its three callers; needs blz::list_link / blz::intrusive_list constructors.
XVarScopeInternal::XVarScopeInternal(XVarScopeInternal* pParentScope, const char* pszName, bool bIsRoot)




    : m_bIsRootScope(bIsRoot)
    , m_pParentScope(NULL)
    , m_tParentScopeLink()
    , m_tChildScopes()
    , m_tPublicScopes()
    , m_tVars()
    , m_ptExternalScope(NULL) {

    Blizzard::String::Copy(m_szName, pszName, sizeof(m_szName));

    if (pParentScope != NULL) {

        pParentScope->AddChildScopeInternal(*this);
    }
}



XVarScopeInternal::~XVarScopeInternal() {


    while (!m_tChildScopes.empty()) {



        delete &m_tChildScopes.front();
    }


    while (!m_tVars.empty()) {

        RemoveVar(m_tVars.front());
    }


    while (!m_tPublicScopes.empty()) {

        RemoveScope(m_tPublicScopes.front());
    }


    DestroyExternalInterface(false);

    if (m_pParentScope != NULL) {


        m_pParentScope->RemoveChildScopeInternal(*this);
    }
}


// Inlined; needs blz::intrusive_list::push_back.
void XVarScopeInternal::AddChildScopeInternal(XVarScopeInternal& tChildScope) {
    tChildScope.m_pParentScope = this;
    m_tChildScopes.push_back(tChildScope);
}





// Stripped by the linker (DWARF low_pc 0); written from its DWARF and checked only against the recorded size.
void XVarScopeInternal::ComposeFullName(
    blz::external_string& tFullName) const {

    if (m_pParentScope != NULL && !m_pParentScope->m_bIsRootScope) {

        m_pParentScope->ComposeFullName(tFullName);
        tFullName.push_back('.');
    }

    tFullName.append(m_szName);
}



void XVarScopeInternal::ComposeFullName(blz::string& tFullName) const {

    if (m_pParentScope != NULL && !m_pParentScope->m_bIsRootScope) {

        m_pParentScope->ComposeFullName(tFullName);

        tFullName.push_back('.');
    }

    tFullName.append(m_szName);
}


// Inlined; needs blz::intrusive_list::erase.
void XVarScopeInternal::RemoveChildScopeInternal(XVarScopeInternal& tChildScope) {



    m_tChildScopes.erase(tChildScope);
    tChildScope.m_pParentScope = NULL;
}



// Inlined; needs blz::intrusive_list::push_back.
void XVarScopeInternal::AddScope(XVarScope& tScope) {



    tScope.m_pScopeInternal = this;


    m_tPublicScopes.push_back(tScope);
}


// Inlined; needs blz::intrusive_list::erase.
void XVarScopeInternal::RemoveScope(XVarScope& tScope) {




    m_tPublicScopes.erase(tScope);
    tScope.m_pScopeInternal = NULL;
    tScope.m_ptRegistry = NULL;
}


// Inlined; needs blz::intrusive_list::push_back.
void XVarScopeInternal::AddVar(XVarBase& tVar) {



    tVar.m_pParentScope = this;


    m_tVars.push_back(tVar);
}


// Inlined; needs blz::intrusive_list::erase.
void XVarScopeInternal::RemoveVar(XVarBase& tVar) {




    m_tVars.erase(tVar);
    tVar.m_pParentScope = NULL;
    tVar.m_ptRegistry = NULL;
}


// Inlined; needs blz::intrusive_list, blz::find_if.
XVarScopeInternal* XVarScopeInternal::FindChildScope(const char* pszName, size_t length) {



    blz::intrusive_list_iterator<XVarScopeInternal, &XVarScopeInternal::m_tParentScopeLink> it = blz::find_if(m_tChildScopes.begin(), m_tChildScopes.end(),
        [pszName, length](const XVarScopeInternal& tChildScope) {
            return Blizzard::String::Equal(pszName, tChildScope.m_szName, length) && Blizzard::String::Length(tChildScope.m_szName) == length;
        });


    return it != m_tChildScopes.end() ? &*it : NULL;
}


// Inlined; needs blz::intrusive_list, blz::find_if.
XVarBase* XVarScopeInternal::FindVar(const char* pszName, size_t length) {



    blz::intrusive_list_iterator<XVarBase, &XVarBase::m_tParentScopeVarLink> it = blz::find_if(m_tVars.begin(), m_tVars.end(),
        [pszName, length](const XVarBase& tVar) {
            return Blizzard::String::Equal(pszName, tVar.Name(), length) && Blizzard::String::Length(tVar.Name()) == length;
        });


    return it != m_tVars.end() ? &*it : NULL;
}


// Inlined, always with bRecurseChildren == false, so what the flag does cannot be recovered from the binary.
void XVarScopeInternal::CreateExternalInterface(XVarExternalInterfaceFactory* ptExternalInterfaceFactory, bool bRecurseChildren) {
    (void)bRecurseChildren;

    if (ptExternalInterfaceFactory != NULL && !m_bIsRootScope) {







        m_ptExternalScope = ptExternalInterfaceFactory->CreateScope(m_pParentScope != NULL ? m_pParentScope->GetExternalScope() : NULL, m_szName);
    }
}



















// Inlined, always with bRecurseChildren == false, so what the flag does cannot be recovered from the binary.
void XVarScopeInternal::DestroyExternalInterface(bool bRecurseChildren) {
    (void)bRecurseChildren;

    if (m_ptExternalScope != NULL) {
        m_ptExternalScope->Release();
        m_ptExternalScope = NULL;
    }
}
