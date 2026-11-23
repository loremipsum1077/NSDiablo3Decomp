// Written by hand from the DWARF (the class template only shows as instantiations there).
// Original file: X/Client/UserInterface/UIAnimation.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XGrowableList.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"

// The enumerators are not in the DWARF: 0 and 1 smooth-step, 2 is linear, anything else
// steps to the second value at the end of the segment.
typedef int32 InterpolationMethod;









template <typename T>
class UIAnimation {
public:

    UIAnimation() {}

    virtual ~UIAnimation()
    {
        ClearNodes();
    }

    BOOL Evaluate(float flT, T & out) const
    {
        const Node * tHeadNode;
        const Node * tTailNode;

        if (m_tNodes.IsEmpty())
        {
            return FALSE;
        }


        tHeadNode = m_tNodes.GetHead();
        if (flT < tHeadNode->m_flT)
        {
            out = tHeadNode->m_nodeValue;
            return TRUE;
        }

        tTailNode = m_tNodes.GetTail();
        if (flT > tTailNode->m_flT)
        {
            out = tTailNode->m_nodeValue;
            return TRUE;
        }

        POS pos = m_tNodes.GetHeadPosition();
        do
        {
            POS currentPos = pos;
            const Node * currentNode = m_tNodes.GetNext(pos);

            if (flT < currentNode->m_flT)
            {
                m_tNodes.GetPrev(currentPos);
                const Node * prevNode = m_tNodes.GetPrev(currentPos);


                float flNormalizedT = (flT - prevNode->m_flT) / (currentNode->m_flT - prevNode->m_flT);
                out = Interpolate(prevNode->m_nodeValue, currentNode->m_nodeValue, flNormalizedT, currentNode->m_eInterpMethod);
                return TRUE;
            }

        } while (pos);



        out = tTailNode->m_nodeValue;
        return TRUE;
    }


    BOOL AddNode(float flT, T nodeValue, InterpolationMethod eInterpMethod)
    {
        if (flT < -1e-06f || flT > 1.0 + 1e-06f)
        {
            return FALSE;
        }

        Node * tNewNode = new Node;
        tNewNode->m_flT = flT;
        tNewNode->m_nodeValue = nodeValue;
        tNewNode->m_eInterpMethod = eInterpMethod;


        if (m_tNodes.IsEmpty() || flT > m_tNodes.GetTail()->m_flT)
        {
            m_tNodes.AddTail_Tracked(tNewNode);
            return TRUE;
        }

        POS pos = m_tNodes.GetHeadPosition();
        while (pos)
        {
            POS currentPos = pos;
            Node * currentNode = m_tNodes.GetNext(pos);


            if (NodeIsMatch(flT, currentNode->m_flT))
            {
                delete tNewNode;
                return FALSE;
            }

            if (flT < currentNode->m_flT)
            {
                m_tNodes.InsertBefore_Tracked(currentPos, tNewNode);
                return TRUE;
            }
        }

        m_tNodes.AddTail_Tracked(tNewNode);
        return TRUE;
    }

    BOOL RemoveNode(float);



















    void ClearNodes()
    {
        POS pos = m_tNodes.GetHeadPosition();
        while (pos)
        {
            Node * currentNode = m_tNodes.GetNext(pos);
            delete currentNode;
        }

        m_tNodes.RemoveAll();
    }

    int32 GetNumNodes() const;

protected:







    T Interpolate(T valueA, T valueB, float flT, InterpolationMethod eInterpMethod) const
    {
        switch (eInterpMethod)
        {
            case 2:
                return Math::Lerp(valueA, valueB, flT);

            case 0: case 1:
                return Math::SmoothStep(valueA, valueB, flT);

            default:
                return NodeIsMatch(flT, 1.0f) ? valueB : valueA;
        }
    }

    struct Node {  // size 0x10 for Vector2D
        float m_flT;  // 0x00
        InterpolationMethod m_eInterpMethod;  // 0x04
        T m_nodeValue;  // 0x08
    };
    typedef XGrowableList<Node *, Node *> AnimationNodeList;
    AnimationNodeList m_tNodes;  // 0x08

private:
    static BOOL NodeIsMatch(float flT, float flNodeT)
    {

        return flT == flNodeT || Math::Abs(flNodeT - flT) < 1e-06f;
    }
};
