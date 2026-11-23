// Compile check for blz/intrusive_list.h. The binary has no out-of-line
// intrusive_list function (everything is inlined into XVar.cpp, which is the
// real check: XVarScopeInternal::~XVarScopeInternal is byte-identical), so
// this only instantiates every member to catch template errors.
#include "Contrib/Contrib/blz/include/blz/intrusive_list.h"

struct XVarScopeInternal {
    int m_pad[20];
    blz::list_link m_tParentScopeLink;
    bool operator<(const XVarScopeInternal&) const { return false; }
    bool operator==(const XVarScopeInternal&) const { return true; }
};

template class blz::intrusive_list<XVarScopeInternal, &XVarScopeInternal::m_tParentScopeLink>;
template class blz::intrusive_list_iterator<XVarScopeInternal, &XVarScopeInternal::m_tParentScopeLink>;
template class blz::intrusive_list_const_iterator<XVarScopeInternal, &XVarScopeInternal::m_tParentScopeLink>;
