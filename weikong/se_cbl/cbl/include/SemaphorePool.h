#if !defined(SEMAPHORE_POOL_H__20190121)
#define SEMAPHORE_POOL_H__20190121

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "SemaphoreEx.h"
#include "MutexLock.h"
using namespace std;

namespace cbl{

class CSemaphorePool{
public:
	CSemaphorePool();
	CSemaphorePool(int nMaxSize);
	virtual ~CSemaphorePool();
	
	int apply(int nId, CSemaphoreEx **ppSemaphore = NULL);
	int wait(int nId,int nTimeout);
	int signal(int nId);
	int clear();
private:	
	int release(int nId);

private:
	int m_nMaxSize;
	map<int, CSemaphoreEx *> m_mapSemaphore;
	CMutexLock m_mutex;
};
	
} //namespace cbl

#endif //SEMAPHORE_POOL_H__20190121
