// Written by hand from the DWARF (Contrib/Contrib/Domino/Include/dmJobDispatcher.h).

#pragma once

typedef void dmJobFcn(void * pParam);

// dmJobDispatcher.h:23. The destructor's out-of-line copy is shared with other
// empty functions in the binary.
class dmJobDispatcher {
public:
    virtual ~dmJobDispatcher() {}
    virtual void Submit(dmJobFcn * pFcn, void * pParam) = 0;
    virtual void FinishJobs() = 0;
};

// dmJobDispatcher.h:46: runs every job immediately on the calling thread.
class dmDefaultJobDispatcher : public dmJobDispatcher {
public:
    void Submit(dmJobFcn * pFcn, void * pParam)
    {
        pFcn(pParam);
    }

    void FinishJobs()
    {
    }
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(dmJobDispatcher) == 0x8, "dmJobDispatcher");
static_assert(sizeof(dmDefaultJobDispatcher) == 0x8, "dmDefaultJobDispatcher");
