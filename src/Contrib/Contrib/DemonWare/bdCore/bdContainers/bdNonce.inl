// Hand-written from the DWARF.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdNonce.inl

#pragma once

#include "bdSingleton.h"
#include "../bdCrypto/bdCryptoUtils.h"
#include "../bdUtilities/bdTrulyRandom.h"

template <unsigned int size>
bdNonce<size>::bdNonce()
    : m_initialised(false)
{
}

template <unsigned int size>
bdNonce<size>::~bdNonce()
{
    bdCryptoUtils::zeroBuffer(m_nonce, size);
}

template <unsigned int size>
void bdNonce<size>::ensureCreated()
{
    if (!m_initialised)
    {
        bdSingleton<bdTrulyRandomImpl>::getInstance()->getRandomUByte8(m_nonce, size);
        m_initialised = true;
    }
}
