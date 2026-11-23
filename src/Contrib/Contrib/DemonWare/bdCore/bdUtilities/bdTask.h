// Adopted from original DWARF declarations.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdTask.h

#pragma once

#include "../bdReference/bdReferencable.h"
#include <stddef.h>
#include <stdint.h>

#include "../bdReference/bdReference.h"
class bdTask;
typedef bdReference<bdTask> bdTaskRef;

class bdTask : public bdReferencable {  // size 0x10
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    enum bdStatus {
        BD_EMPTY = 0,
        BD_PENDING = 1,
        BD_DONE = 2,
        BD_FAILED = 3,
        BD_TIMED_OUT = 4,
        BD_CANCELLED = 5,
        BD_MAX_STATUS = 6,
    };
    virtual bdTask::bdStatus getStatus() = 0;  // vtable slot 2
    static bdTaskRef bdDone;
    static bdTaskRef bdFailed;
};
static_assert(sizeof(bdTask) == 0x10, "bdTask");


// Functions defined in this file (bodies are not in the DWARF):
//   line    17  bdTask::bdTask()  [inlined into 1 functions]
