// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSingleton.h
// Declarations sit on their DWARF decl_line; every instance of the template is an ordinary
// implicit instantiation of the bodies in bdSingleton.inl (no explicit specialization exists:
// each CU that uses a singleton carries its own getInstance/destroyInstance DIE at bdSingleton.inl:10/47).
#pragma once
#include "bdFastArray.h"
#include "../bdMemory/bdMemory-std.h"



























template <class T> class bdSingleton {
public:


    BD_CLASS_ALLOCATOR(bdSingleton);


    static T *getInstance();







    void replaceInstance(const T *const);




    static void destroyInstance();
protected:





    bdSingleton();




    static T *m_instance;
};





























class bdSingletonRegistryImpl {
public:


    BD_CLASS_ALLOCATOR(bdSingletonRegistryImpl);



    typedef void (*bdSingletonDestroyFunction)();



    // Reconstructed from the random singleton instantiation.
    bdBool add(const bdSingletonDestroyFunction destroyFunction)
    {
        if (m_cleaningUp)
            return false;

        m_destroyFunctions.pushBack(destroyFunction);
        return true;
    }












    // Reconstructed from its copy inlined into bdCore::quit.
    void cleanUp()
    {
        bdSingletonDestroyFunction * const first = m_destroyFunctions.begin();
        bdSingletonDestroyFunction * it = m_destroyFunctions.end();
        m_cleaningUp = true;

        while (it != first)
        {
            --it;
            (*it)();
        }
    }
protected:
    template <class T> friend class bdSingleton;










    bdSingletonRegistryImpl() : m_destroyFunctions(0U), m_cleaningUp(false)

    {
    }


    virtual ~bdSingletonRegistryImpl()
    {
    }




    bdFastArray<bdSingletonDestroyFunction> m_destroyFunctions;


    bdBool m_cleaningUp;
};
static_assert(sizeof(bdSingletonRegistryImpl) == 0x20, "bdSingletonRegistryImpl");

#include "bdSingleton.inl"
