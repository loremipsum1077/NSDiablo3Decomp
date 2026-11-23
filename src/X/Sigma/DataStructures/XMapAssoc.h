#pragma once

#include "../SigmaTypes.h"












template <typename Key, typename VALUE>
class XMapAssoc {
public:
    XMapAssoc() { m_pNext = NULL; }

    XMapAssoc<Key, VALUE>* m_pNext;
    Key m_Key;
    VALUE m_Value;
};
