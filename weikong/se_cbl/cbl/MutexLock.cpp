/*Mutex.cpp: create automatically by makeclass at 2013-07-12*/
#include "MutexLock.h"

namespace cbl{

CMutexLock::CMutexLock()
{
#ifdef _WIN32
	InitializeCriticalSectionAndSpinCount(&m_critical, 4000);
#else //_WIN32
	pthread_mutex_init (&m_mutex, 0);
#endif //_WIN32
}

CMutexLock::~CMutexLock()
{
#ifdef _WIN32
	DeleteCriticalSection(&m_critical);
#else //_WIN32
	pthread_mutex_destroy (&m_mutex);
#endif //_WIN32
}

int CMutexLock::lock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_critical);
	return 0;
#else //_WIN32
	return pthread_mutex_lock (&m_mutex);
#endif //_WIN32
}

int CMutexLock::unlock()
{
#ifdef _WIN32
	LeaveCriticalSection(&m_critical);
	return 0;
#else //_WIN32
	return pthread_mutex_unlock (&m_mutex);
#endif //_WIN32
}
	
} //namespace cbl
