// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: X/Client/Camera/HardpointViewController.h

#pragma once

#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/AttachmentDefinition.h"

class HardpointViewController {  // size 0x2c
public:
    HardpointViewController();
    ~HardpointViewController() {}  // empty and inline: ~Cinematography calls no destructor for it
    void Attach(const RActorID idRActor, const Hardpoint & tHardpoint);
    void Update();
private:
    RActorID m_idRActor;  // 0x00
    Hardpoint m_tHardpoint;  // 0x04
};
static_assert(sizeof(HardpointViewController) == 0x2c, "HardpointViewController");

