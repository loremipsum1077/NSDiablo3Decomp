#pragma once

#include "DataStructuresExternal.h"






















































template <typename XBinaryHeapNode, int bNodeIndexChangedEvents>
class XBinaryHeap {
public:


    inline void Initialize(int nSize) {

        m_arHeapNodes = (XBinaryHeapNode*)SigmaMemoryNew(nSize * sizeof(XBinaryHeapNode), 4, NULL, TRUE);
        m_nSize = nSize;
        m_nCount = 0;
    }

    inline void Dispose() {

        XFREE(m_arHeapNodes, NULL);

        m_nSize = 0;
        m_nCount = 0;
    }


    inline int GetNodeIndex(XBinaryHeapNode* ptNode) {

        int nIndex = (int)(ptNode - m_arHeapNodes);

        return nIndex;
    }



    inline XBinaryHeapNode* GetFront() {




        return m_arHeapNodes;
    }


    inline void PopFront() {
        XBinaryHeapNode& tNodeRemove = m_arHeapNodes[0];


        m_nCount--;



        if (m_nCount > 0) {

            XBinaryHeapNode& tNodeReplace = m_arHeapNodes[m_nCount];

            tNodeRemove = tNodeReplace;

            if (bNodeIndexChangedEvents) {

                tNodeRemove.NodeIndexChanged(-1);
                tNodeReplace.NodeIndexChanged(0);
            }

            DownHeap(0);
        } else {


            if (bNodeIndexChangedEvents) {

                tNodeRemove.NodeIndexChanged(-1);
            }
        }
    }

    inline void RemoveAll() {

        m_nCount = 0;
    }


    inline void UpHeap(int nIndex) {



        if (nIndex > 0) {

            int nParent = (nIndex - 1) / 2;

            XBinaryHeapNode& tNode = m_arHeapNodes[nIndex];
            XBinaryHeapNode& tParent = m_arHeapNodes[nParent];

            if (tNode.HeapCompare(tParent)) {

                XBinaryHeapNode tTemp = tParent;
                tParent = tNode;
                tNode = tTemp;

                if (bNodeIndexChangedEvents) {

                    tNode.NodeIndexChanged(nIndex);
                    tParent.NodeIndexChanged(nParent);
                }

                UpHeap(nParent);
            }
        }
    }


    inline void DownHeap(int nIndex) {



        int nChildLeft = 2 * nIndex + 1;
        int nChildRight = nChildLeft + 1;

        XBinaryHeapNode& tNodeParent = m_arHeapNodes[nIndex];
        XBinaryHeapNode* ptNodeSwap = NULL;
        int nIndexSwap = -1;

        if (nChildRight < m_nCount) {

            XBinaryHeapNode& tNodeLeft = m_arHeapNodes[nChildLeft];
            XBinaryHeapNode& tNodeRight = m_arHeapNodes[nChildRight];

            if (tNodeLeft.HeapCompare(tNodeRight)) {

                if (tNodeLeft.HeapCompare(tNodeParent)) {

                    nIndexSwap = nChildLeft;
                    ptNodeSwap = &tNodeLeft;
                }

            } else {

                if (tNodeRight.HeapCompare(tNodeParent)) {

                    nIndexSwap = nChildRight;
                    ptNodeSwap = &tNodeRight;
                }
            }

        } else if (nChildLeft < m_nCount) {

            XBinaryHeapNode& tNodeLeft = m_arHeapNodes[nChildLeft];

            if (tNodeLeft.HeapCompare(tNodeParent)) {

                nIndexSwap = nChildLeft;
                ptNodeSwap = &tNodeLeft;
            }
        }


        if (ptNodeSwap != NULL) {

            XBinaryHeapNode tTemp;
            tTemp = *ptNodeSwap;
            *ptNodeSwap = tNodeParent;
            tNodeParent = tTemp;

            if (bNodeIndexChangedEvents) {

                tNodeParent.NodeIndexChanged(nIndex);
                ptNodeSwap->NodeIndexChanged(nIndexSwap);
            }

            DownHeap(nIndexSwap);
        }
    }


    inline void InsertNode(XBinaryHeapNode& tNode) {



        int nIndex = m_nCount;

        XBinaryHeapNode& tNodeInsert = m_arHeapNodes[nIndex];

        m_nCount++;

        tNodeInsert = tNode;

        if (bNodeIndexChangedEvents) {

            tNodeInsert.NodeIndexChanged(nIndex);
        }

        UpHeap(nIndex);
    }



































    inline XBinaryHeapNode* GetNode(int nIndex) {



        return &m_arHeapNodes[nIndex];
    }




    inline int Count() { return m_nCount; }
    inline bool IsEmpty() { return m_nCount == 0; }

    inline bool IsFull() { return m_nCount == m_nSize; }

protected:
    int m_nSize;
    int m_nCount;
    XBinaryHeapNode* m_arHeapNodes;
};
