#if !defined (AUTO_LOCK_H__20180820)
#define AUTO_LOCK_H__20180820

#include "MutexLock.h"

namespace cbl {

class CAutoLock
{
public:
	CAutoLock(CMutexLock &mutex);
	virtual ~CAutoLock();

private:
	CMutexLock *m_pmutex;
};
	
}

#endif
