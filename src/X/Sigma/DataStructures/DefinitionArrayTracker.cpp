#include "DefinitionArrayTracker.h"

// Separate 2.7.7 definition-array tracker; existing unnamed identities retained.
DefinitionDataArrayList* unk_7101a46c30;
extern "C" void sub_7100a244b0() {
    unk_7101a46c30 = new DefinitionDataArrayList;
    unk_7101a46c30->SetBlockCount_Tracked(1024);
}
extern "C" void sub_7100a24520() {
    delete unk_7101a46c30;
    unk_7101a46c30 = NULL;
}
