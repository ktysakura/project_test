#if !defined (MUTEX_LOCK_H__20180820)
#define MUTEX_LOCK_H__20180820

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
	#include <windows.h>
#else //_WIN32
	#include <unistd.h>
	#include <pthread.h>
#endif //_WIN32
	
namespace cbl{

class CMutexLock
{
public:
	int lock();
	int unlock();
	
	CMutexLock();
	virtual ~CMutexLock();

private:
#ifdef _WIN32
	CRITICAL_SECTION m_critical;
#else //_WIN32
	pthread_mutex_t	m_mutex;
#endif //_WIN32

};
	
} //namespace cbl

#endif //MUTEX_LOCK_H__20180820
