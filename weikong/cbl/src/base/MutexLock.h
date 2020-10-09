#if !defined (MUTEX_LOCK_H__20180820)
#define MUTEX_LOCK_H__20180820

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

namespace cbl {

class CMutexLock
{
public:
	int lock();
	int unlock();
	
	CMutexLock();
	virtual ~CMutexLock();

private:
	CRITICAL_SECTION m_critical;
};
	
} 

#endif 
