// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSingleton.inl
#pragma once







template<class T> T *bdSingleton<T>::getInstance()
{

    if (!m_instance) {

        m_instance = new T();
        if (m_instance) {

            if (!bdSingleton<bdSingletonRegistryImpl>::getInstance()->add(destroyInstance)) {


                delete m_instance;
                m_instance = 0; __builtin_debugtrap();
            }
        } else __builtin_debugtrap();
    }







    return m_instance;
}












template<class T> void bdSingleton<T>::destroyInstance()
{
    if (m_instance) {

        delete m_instance;
        m_instance = 0;
    }
}
