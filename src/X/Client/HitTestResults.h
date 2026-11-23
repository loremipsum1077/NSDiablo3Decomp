// Original file: X/Client/HitTestResults.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "RActors/RActorCollision.h"
#include "../Common/CommonTypes.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/Math/Vector.h"

class HitTestResults {  // size 0x60
public:
    HitTestResults();
    ~HitTestResults();
    void AddHitRActor(Vector3D, RActorID, const RActorExactIntersectionResult &);
    int32 GetHitCount() const;
    void Clear();
    void RemoveInvalidMarkers();
protected:
    void EnsureHitListIsSorted() const;
private:
    class Details {  // size 0x44
    public:
        RActorID GetHitRActorID() const;
        WorldDistance GetHitDistance() const;
    private:
        friend class HitTestResults;
        Details();
        Details(Vector3D, RActorID, const RActorExactIntersectionResult &);
        Vector3D m_wpIntersect;  // 0x00
        RActorID m_idRActorHit;  // 0x0c
        RActorExactIntersectionResult m_tExactIntersection;  // 0x10
        static const HitTestResults::Details NOTHING_HIT;
    };
    BOOL m_bListSortOrderDirty;  // 0x00
    XGrowableList<HitTestResults::Details *, HitTestResults::Details *> m_HitTestDetails;  // 0x08
};
static_assert(sizeof(HitTestResults) == 0x60, "HitTestResults");
