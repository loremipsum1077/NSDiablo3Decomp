#pragma once

#include "../SigmaTypes.h"












template <typename TYPE>
class XBaseListNode {
public:
    XBaseListNode() {
        m_pPrev = NULL;
        m_pNext = NULL;
    }

    TYPE m_tData;
    XBaseListNode<TYPE>* m_pPrev;
    XBaseListNode<TYPE>* m_pNext;
};
