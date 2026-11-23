// Original file: BlizzardCore/Include/Blizzard/Util/Jobs.h
// Written from the DWARF of BlizzardCore/Source/Packages/Util/Package_Util.cpp (cu_00054f3f):
// class layouts, member names, declaration lines, parameters and vtable slots. Every entity is
// on its DWARF line. Only the inline bodies that ~Job (Job.cpp:35) inlines are written; the
// other members are declarations only. The original also declares WorkerPool (line 229),
// JobController and WorkOrder here; nothing in this project uses them, so they are not written.
#pragma once

#include "../Lock.h"

namespace Blizzard {
namespace Jobs {














template <typename T>
class RefPtr {  // size 0x8 (T = Job)

public:
    RefPtr();
    RefPtr(const RefPtr & other);
    RefPtr(T * p);

    ~RefPtr() { release(); }

    RefPtr & operator=(const RefPtr & p);
    RefPtr & operator=(T * p);

    T * get() const;
    operator T *() const;
    T * operator->() const;
    T & operator*() const;

private:





    void assign(T * p);




    // LINKER-REMOVED (inline): DWARF Jobs.h:57, inlined into ~Job at Job.cpp:44 (via ~RefPtr, line 35) as
    void release() { if (m_Ptr) m_Ptr->Release(); }  // a load, a null test and RefCounted::Release only (no store).
    T * m_Ptr;  // 0x00
};














class JobData : public Blizzard::Lock::RefCounted {  // size 0x10

public:
    virtual ~JobData();  // vtable slot 0
};
static_assert(sizeof(JobData) == 0x10, "JobData");






class Job : public Blizzard::Lock::RefCounted {  // size 0x48






    enum { MAX_INPUTS = 4 };







public:
    explicit Job(int numRequiredInputs);
    virtual ~Job();  // vtable slot 0









    virtual bool Execute() = 0;  // vtable slot 2






    virtual int GetClassId() const = 0;  // vtable slot 3








    virtual bool IsReady() const;  // vtable slot 4







    int GetNumInputs() const;





    void SetInputData(int index, JobData * pData);





    bool IsInputSet(int index) const;







    JobData * GetOutputData() const;





    void ConnectTo(RefPtr<Job> job, int inputIndex);






    RefPtr<Job> ForwardOutput();










    bool MonitoredExecute();





    bool DidFail() const;




    bool ShouldRunOnMainThread() const;




    bool IsPropagatingFailure() const;

    bool IsInvalidJob();

protected:
    int m_numRequiredInputs;  // 0x0c
    JobData * m_Inputs[MAX_INPUTS];  // 0x10
    JobData * m_Output;  // 0x30

    RefPtr<Job> m_NextJob;  // 0x38
    int m_NextJobInputIndex;  // 0x40

    bool m_RunOnMainThread;  // 0x44

private:
    bool m_Success;  // 0x45
    bool m_PropagateFailure;  // 0x46
};
static_assert(sizeof(Job) == 0x48, "Job");

}  // namespace Jobs
}  // namespace Blizzard
