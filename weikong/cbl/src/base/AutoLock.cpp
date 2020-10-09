#include "AutoLock.h"

namespace cbl {

CAutoLock::CAutoLock(CMutexLock &mutex) : m_pmutex(&mutex)
{
	m_pmutex->lock();
}

CAutoLock::~CAutoLock()
{
	m_pmutex->unlock();
}
	
}
