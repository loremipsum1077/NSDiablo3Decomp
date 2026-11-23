// Original file: Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.inl
// Bodies are not in the DWARF. Evidence: the inline trees (addRef calls
// atomic::operator++ from line 42, releaseRef calls operator-- from line 73; the gap between
// each declaration and its statement is original comments/asserts that left no code) and the
// instructions (ldaxr/add/stlxr, ldaxr/subs/stlxr with the result tested against zero).

// line 10
inline bdReferencable::bdReferencable()
    : m_refCount(0)
{
}

// line 19
inline bdInt bdReferencable::addRef()
{
    return ++m_refCount;
}

// line 50
inline bdInt bdReferencable::releaseRef()
{
    return --m_refCount;
}

// Declared in the DWARF but never inlined or emitted: body guessed.
inline bdInt bdReferencable::getRefCount() const
{
    return m_refCount;
}
