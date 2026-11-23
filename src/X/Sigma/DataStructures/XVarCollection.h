#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/func_poly.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_set.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "XVariant.h"

class XVarBase;









class XVarCollection {
    typedef blz::function<void(const blz::vector<XVarBase*, blz::allocator<XVarBase*> >&)> DirtyXVarsCb;

public:
    XVarCollection();
    void Add(XVarBase& tVar, const XVariant& tDefaultValue);
    void Remove(const XVarBase&);
    uint64 RegisterDirtyCallback(DirtyXVarsCb&&);
    void UnregisterDirtyCallback(uint64);
    void GenerateCmdLineString(blz::string&, bool);
    void Process();

















    struct TrackedXVar {
        TrackedXVar(XVarBase* ptVarLocal, const XVariant& tVariantLocal)
            : ptVar(ptVarLocal), tDefaultValue(tVariantLocal) {}

        XVarBase* ptVar;
        XVariant tDefaultValue;
    };

private:
    void _OnVarDirty(XVarBase* ptVar);
    bool _IsVarDefaultValue(TrackedXVar&);

    blz::vector<TrackedXVar, blz::allocator<TrackedXVar> > m_tTrackedVars;
    blz::unordered_set<XVarBase*, blz::hash<XVarBase*>, blz::equal_to<XVarBase*>, blz::allocator<XVarBase*> > m_tDirtyVars;
    blz::unordered_map<unsigned long, DirtyXVarsCb, blz::hash<unsigned long>, blz::equal_to<unsigned long>,
                       blz::allocator<blz::pair<const unsigned long, DirtyXVarsCb> > >
        m_tDirtyCallbacks;
    uint32 m_uTokenIds;
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XVarCollection) == 0x60, "XVarCollection");
