#include "dmAtomic.h"

static inline dm_int32 dmAtomicIncrement(volatile dm_int32* pDestination)
{
    return __sync_add_and_fetch(pDestination, 1);
}

static inline dm_int32 dmAtomicDecrement(volatile dm_int32* pDestination)
{
    return __sync_sub_and_fetch(pDestination, 1);
}

static inline dm_int32 dmAtomicOr(volatile dm_int32* pDestination, dm_int32 value)
{
    return __sync_fetch_and_or(pDestination, value);
}

static inline dm_int32 dmAtomicAnd(volatile dm_int32* pDestination, dm_int32 value)
{
    return __sync_fetch_and_and(pDestination, value);
}

static inline bool dmAtomicCompareAndSwap(volatile dm_int32* pDestination, dm_int32 expectedOldValue, dm_int32 newValue)
{
    return __sync_bool_compare_and_swap(pDestination, expectedOldValue, newValue);
}

static inline void dmReadBarrier()
{
    __sync_synchronize();
}

static inline void dmWriteBarrier()
{
    __sync_synchronize();
}

void dmAtomic_s32::StoreRelease(dm_int32 value)
{
    m_value = value;
    dmWriteBarrier();
}

dm_int32 dmAtomic_s32::LoadAcquire() const
{
    dmReadBarrier();
    return m_value;
}

void dmAtomic_float32::operator=(dm_float32 value)
{
    m_value = value;
    dmWriteBarrier();
}

dmAtomic_float32::operator float() const
{
    dmReadBarrier();
    return m_value;
}

void dmAtomic_float32::operator+=(dm_float32 a)
{
    for (;;)
    {
        dmReadBarrier();
        dm_float32 oldValue = m_value;
        dm_float32 newValue = oldValue + a;
        if (dmAtomicCompareAndSwap((volatile dm_int32*)&m_value, *(dm_int32*)&oldValue, *(dm_int32*)&newValue))
        {
            break;
        }
    }
}
