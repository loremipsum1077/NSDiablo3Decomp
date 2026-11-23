#pragma once

#include "../SigmaTypes.h"
























template <typename _T>
class XPtr64 {
public:




    XPtr64() { m_uPtr = 0; }


    ~XPtr64() { m_uPtr = 0; }

    XPtr64(_T* ptr) {
        m_uPtr = (uint64)ptr;
    }



    XPtr64<_T>& operator=(_T* rhs) {
        m_uPtr = (uint64)rhs;
        return *this;
    }

    _T** operator&();






    _T& operator*() const {
        return *m_pDebugPtr;
    }


    _T* operator->() const {
        return m_pDebugPtr;
    }


    operator _T*() const {
        return m_pDebugPtr;
    }


    bool operator!() const {
        return m_pDebugPtr == NULL;
    }



    _T* Get() {
        return m_pDebugPtr;
    }



    const _T* Get() const {
        return m_pDebugPtr;
    }

    void Swap(_T*& rhs);









    void Null() {
        m_uPtr = 0;
    }

private:
    union {
        uint64 m_uPtr;
        _T* m_pDebugPtr;
    };
};
