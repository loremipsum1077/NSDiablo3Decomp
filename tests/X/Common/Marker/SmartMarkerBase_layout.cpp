// Layout check for X/Common/Marker/SmartMarkerBase.h against the DWARF (byte_size and
// data_member_location of MarkerBase). Compiled, not compared: it defines no function the
// binary has (the vtable, and with it the in-class virtual bodies, belongs to
// SmartMarkerBase.cpp's key function).
#include "X/Common/Marker/SmartMarkerBase.h"

#include <cstddef>

struct MarkerBaseProbe : MarkerBase {
    static void Check() {
        static_assert(sizeof(MarkerBase) == 0x18, "MarkerBase size");
        static_assert(offsetof(MarkerBaseProbe, m_snoGroup) == 0x08, "m_snoGroup");
        static_assert(offsetof(MarkerBaseProbe, m_eMarkerType) == 0x0c, "m_eMarkerType");
        static_assert(offsetof(MarkerBaseProbe, m_szDisplayString) == 0x10, "m_szDisplayString");
    }
};
