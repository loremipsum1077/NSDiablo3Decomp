// Original file: Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdFactory.inl
#pragma once







// line 12
template <typename BASE>
bdCreatorBase<BASE>::~bdCreatorBase()
{
}







// line 24
template <typename DERIVED, typename BASE>
bdCreator<DERIVED, BASE>::~bdCreator()
{
}

template <typename DERIVED, typename BASE>
BASE * bdCreator<DERIVED, BASE>::create() const
{
    return new DERIVED();
}

template <typename DERIVED, typename BASE>
bdUInt bdCreator<DERIVED, BASE>::getSizeOf() const
{
    return sizeof(DERIVED);
}
