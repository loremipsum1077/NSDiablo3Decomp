#pragma once

#include "../Math/Sphere.h"
#include "XPooledList.h"
#include "XTemplate.h"

class CRefString;
class XSphereNode;

typedef CRefString (*SphereNameFunc)(const XSphereNode&);

class XSphereNode {
public:
    XSphereNode();
    void Init(const Sphere& ws);
    void GetSphere(Sphere& ws) const;
    void* GetUserData() const;
    void SetUserData(void* pUserData);

protected:
    friend class XSphereArray;

    Sphere m_sphere;
    void* m_pUserData;
    XSphereNode* m_pParent;
    XSphereNode* m_listChildren[2];
};

typedef XPooledList<const XSphereNode*, const XSphereNode*> XSphereNodeList;

inline XSphereNode::XSphereNode() { m_pUserData = NULL; m_pParent = NULL; m_listChildren[0] = NULL; m_listChildren[1] = NULL; }


inline void XSphereNode::Init(const Sphere& ws) {
    m_sphere = ws;
}




















































































inline void XSphereNode::GetSphere(Sphere& ws) const {
    ws = m_sphere;
}



inline void* XSphereNode::GetUserData() const {
    return m_pUserData;
}



inline void XSphereNode::SetUserData(void* pUserData) {
    m_pUserData = pUserData;
}

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(XSphereNode) == 0x30, "XSphereNode");
