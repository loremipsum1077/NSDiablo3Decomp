// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSingleton.cpp
// Only the registry's instance pointer; the file's functions are header inlines.

#include "bdSingleton.h"








template <> bdSingletonRegistryImpl * bdSingleton<bdSingletonRegistryImpl>::m_instance = NULL;  // line 11
