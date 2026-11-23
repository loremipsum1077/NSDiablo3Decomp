#include "XVarCollection.h"

#include "XVar.h"











XVarCollection::XVarCollection()
    : m_tTrackedVars()
    , m_tDirtyVars()
    , m_tDirtyCallbacks()
    , m_uTokenIds(0) {
}





void XVarCollection::Add(XVarBase& tVar, const XVariant& tDefaultValue) {
    m_tTrackedVars.emplace_back(&tVar, tDefaultValue);










    tVar.RegisterDirtyCallback(blz::bind(&XVarCollection::_OnVarDirty, this, &tVar));
}



















































































void XVarCollection::_OnVarDirty(XVarBase* ptVar) {
    m_tDirtyVars.insert(ptVar);
}
