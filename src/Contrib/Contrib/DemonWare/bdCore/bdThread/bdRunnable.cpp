#include "Contrib/Contrib/DemonWare/bdCore/bdThread/bdRunnable.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
// Blank lines keep the original line numbers








bdRunnable::bdRunnable()
    : m_stop(false)
{
}

bdRunnable::~bdRunnable()
{
}

void bdRunnable::stop()
{
    m_stop = true;
}

void bdRunnable::start()
{
    m_stop = false;
}
