// Original file: Contrib/Contrib/DemonWare/bdCore/bdReference/bdReference.inl
// Bodies are not in the DWARF. Each function sits at its recorded line; the statements are
// placed from the per-instruction line table of callers that inline them (line numbers in the
// comments) and from the two out-of-line destructors, which match (tests/demonware).

// line 12 (13: initializer)
template <typename T>
inline bdReference<T>::bdReference()
    : m_ptr(NULL)
{
}

// line 19 (20: initializer, 22: test, 24: addRef)
template <typename T>
inline bdReference<T>::bdReference(T* p)
    : m_ptr(p)
{
    if (m_ptr != NULL)
    {
        m_ptr->addRef();
    }
}

// line 29 (30: initializer, 32: test, 34: addRef)
template <typename T>
inline bdReference<T>::bdReference(const bdReference<T>& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr != NULL)
    {
        m_ptr->addRef();
    }
}

// line 40 (42: other.operator->() const is inlined here, 43: test, 45: addRef). The binary
// uses it for downcasts too (bdReference<bdInitChunk> from bdReference<bdChunk>), so the
// pointer is cast rather than converted; the spelling of the cast is a guess.
template <typename T>
template <typename U>
inline bdReference<T>::bdReference(const bdReference<U>& other)
{
    m_ptr = static_cast<T*>(other.operator->());
    if (m_ptr != NULL)
    {
        m_ptr->addRef();
    }
}

// line 50 (52, 54, 56, 57, 60)
template <typename T>
inline bdReference<T>::~bdReference()
{
    if (m_ptr != NULL)
    {
        if (m_ptr->releaseRef() == 0)
        {
            delete m_ptr;
            m_ptr = NULL;
        }
    }
}

// line 63 (65, 67, 69, 72, 73, addRef at 75)
template <typename T>
inline void bdReference<T>::operator=(T* p)
{
    if (m_ptr != NULL)
    {
        if (m_ptr->releaseRef() == 0)
        {
            delete m_ptr;
        }
    }
    m_ptr = p;
    if (m_ptr != NULL)
    {
        m_ptr->addRef();
    }
}

// line 80 (82: address compare, 84, 86, 88, 91, 92, addRef at 94)
template <typename T>
inline bdReference<T>& bdReference<T>::operator=(const bdReference<T>& other)
{
    if (&other != this)
    {
        if (m_ptr != NULL)
        {
            if (m_ptr->releaseRef() == 0)
            {
                delete m_ptr;
            }
        }
        m_ptr = other.m_ptr;
        if (m_ptr != NULL)
        {
            m_ptr->addRef();
        }
    }
    return *this;
}

// line 102 (104: address compare, 106, 108, 110, 116: other.operator->() const, 117, 118,
// addRef at 120). The local `const T* temp` is in the DWARF, inside a block; the casts are
// guesses.
template <typename T>
template <typename U>
inline bdReference<T>& bdReference<T>::operator=(const bdReference<U>& other)
{
    if (static_cast<const void*>(this) != static_cast<const void*>(&other))
    {
        if (m_ptr != NULL)
        {
            if (m_ptr->releaseRef() == 0)
            {
                delete m_ptr;
            }
        }



        const T* temp = other.operator->();
        m_ptr = const_cast<T*>(temp);
        if (m_ptr != NULL)
        {
            m_ptr->addRef();
        }
    }
    return *this;
}

// line 129 (131: other.operator->() const is inlined here)
template <typename T>
template <typename U>
inline bdBool bdReference<T>::operator==(const bdReference<U>& other) const
{
    return m_ptr == other.operator->();
}

// line 136 (138). As operator==.
template <typename T>
template <typename U>
inline bdBool bdReference<T>::operator!=(const bdReference<U>& other) const
{
    return m_ptr != other.operator->();
}

// line 142 (144)
template <typename T>
inline bdBool bdReference<T>::operator!() const
{
    return m_ptr == NULL;
}

// line 148 (150)
template <typename T>
inline bdBool bdReference<T>::isNull() const
{
    return m_ptr == NULL;
}

// line 154 (156)
template <typename T>
inline bdBool bdReference<T>::notNull() const
{
    return m_ptr != NULL;
}

// line 160 (162)
template <typename T>
inline bdReference<T>::operator T*() const
{
    return m_ptr;
}

// line 166 (168)
template <typename T>
inline T* bdReference<T>::operator->()
{
    return m_ptr;
}

// line 172 (174)
template <typename T>
inline T* bdReference<T>::operator->() const
{
    return m_ptr;
}

// line 178 (180)
template <typename T>
inline T& bdReference<T>::operator*()
{
    return *m_ptr;
}

// line 184 (186)
template <typename T>
inline const T& bdReference<T>::operator*() const
{
    return *m_ptr;
}

// line 190. Declared and instantiated but never inlined into any code: body guessed.
template <typename T>
inline bdBool bdReference<T>::operator<(const bdReference<T>& other) const
{
    return m_ptr < other.m_ptr;
}

// Declared (bdReference.h:151) but never instantiated: body guessed.
template <typename T>
inline bdBool bdReference<T>::operator>(const bdReference<T>& other) const
{
    return m_ptr > other.m_ptr;
}

// line 202. Only the abstract (never inlined) record exists: body guessed.
template <typename T>
inline bdReference<T> bdMakeReference()
{
    return bdReference<T>(new T());
}
