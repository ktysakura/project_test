#include "MutexLock.h"

namespace cbl{

CMutexLock::CMutexLock()
{
	InitializeCriticalSectionAndSpinCount(&m_critical, 4000);
}

CMutexLock::~CMutexLock()
{
	DeleteCriticalSection(&m_critical);
}

int CMutexLock::lock()
{
	EnterCriticalSection(&m_critical);
	return 0;
}

int CMutexLock::unlock()
{
	LeaveCriticalSection(&m_critical);
	return 0;
}
	
} 
