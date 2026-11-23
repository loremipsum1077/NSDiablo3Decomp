// Original file: X/Client/HitTestResults.cpp
// Include order from the original line table (SNOName.h, WorldPlace.h, GfxVB.h, tuple.h, then
// HitTestResults.h); it fixes the order of the header statics in _GLOBAL__sub_I_HitTestResults.cpp.
#include "../Common/SNO/SNOName.h"
#include "../Common/WorldPlace.h"
#include "Gfx/GfxVB.h"
#include "../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "HitTestResults.h"
#include <float.h>



//@ 16
const HitTestResults::Details HitTestResults::Details::NOTHING_HIT;

//@ 19
HitTestResults::HitTestResults()
    : m_bListSortOrderDirty(FALSE)
{
}

//@ 26
HitTestResults::Details::Details()
    // The original loads both sources through the GOT: the copy at 0x10e5698 (-> 0xe5dadc, cgwvNull,
    // just before cgwvOne in Vector.cpp's data) and the triangle at 0x10e5578 (-> 0xe5daa4, cgwpNull).
    : m_wpIntersect(cgwvNull)
    , m_idRActorHit(RACTORID_INVALID)
{
    m_tExactIntersection.wdT = FLT_MAX;
    m_tExactIntersection.wtri.Set(cgwpNull, cgwpNull, cgwpNull);
}

//@ 66
HitTestResults::~HitTestResults()
{
    Clear();
}

//@ 90
void HitTestResults::Clear()
{
    while (!m_HitTestDetails.IsEmpty())
    {
        Details * pDetails = m_HitTestDetails.GetHead();
        m_HitTestDetails.RemoveHead();
        delete pDetails;
    }
}
