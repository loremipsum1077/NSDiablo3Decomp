#pragma once

#include "../SigmaTypes.h"













template <typename T>
class XOptional {
public:

    inline XOptional() : m_bExists(FALSE) {}

    inline BOOL Exists() const { return m_bExists; }
    inline void SetExists(BOOL bExists) { m_bExists = bExists; }

    inline const T& Get() const { return m_tValue; }
    inline T& Get() { return m_tValue; }
    inline void Set(const T& tValue) { m_bExists = TRUE; m_tValue = tValue; }

    inline operator const T&() const { return m_tValue; }
    inline operator T&() { return m_tValue; }

    inline const T* operator->() const { return &m_tValue; }
    inline T* operator->() { return &m_tValue; }

    inline T& operator=(const T& tValue) { Set(tValue); return m_tValue; }

protected:
    T m_tValue;
    BOOL m_bExists;
};
