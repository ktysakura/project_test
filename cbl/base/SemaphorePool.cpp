#include "SemaphorePool.h"

namespace cbl {

#define DEFAULT_SEMAPHORE_POOL_SIZE 64

CSemaphorePool::CSemaphorePool()
{
	m_nMaxSize = DEFAULT_SEMAPHORE_POOL_SIZE;
}

CSemaphorePool::CSemaphorePool(int nMaxSize)
{
	if (nMaxSize <= 0) {
		nMaxSize = DEFAULT_SEMAPHORE_POOL_SIZE;
	}
	m_nMaxSize = nMaxSize;
}

CSemaphorePool::~CSemaphorePool()
{
}

int CSemaphorePool::apply(int nId, CSemaphoreEx **ppSemaphore)
{
	
	CSemaphoreEx *pSemaphore = NULL;	
	pair<map<int, CSemaphoreEx *>::iterator, bool> retInsert;
	CAutoLock lock(m_mutex);

	if (nId < 0) {
		return -1;
	}

	if ((int)m_mapSemaphore.size() >= m_nMaxSize) {
		return -2;
	}

	if (NULL == (pSemaphore = new CSemaphoreEx())) {
		return -3;
	}

	if (pSemaphore->create(1) < 0) {
		delete pSemaphore;
		return -4;
	}

	retInsert = m_mapSemaphore.insert(make_pair(nId, pSemaphore));
	if (!retInsert.second) {
		delete pSemaphore;
		return -5;
	}

	if(ppSemaphore){
		*ppSemaphore = pSemaphore;
	}

	return 0;
}

int CSemaphorePool::wait(int nId, int nTimeout)
{
	int nRtn,nRetCode;
	CSemaphoreEx *pSemaphore = NULL;
	map<int, CSemaphoreEx *>::iterator mit;

	{
		CAutoLock lock(m_mutex);

		if (m_mapSemaphore.end() != (mit = m_mapSemaphore.find(nId))) {
			pSemaphore = mit->second;
		}
	}
	
	if (NULL == pSemaphore) {
		if ((nRtn = this->apply(nId, &pSemaphore)) < 0) {
			return -1;
		}		
	}

	nRtn = pSemaphore->wait(nTimeout);

	if ((nRetCode = this->release(nId)) < 0) {
		fprintf(stderr, "relase semaphore error %d\n", nRetCode);
	}

	return ((0 == nRtn) ? 0 : (nRtn - 0x10));
}

int CSemaphorePool::signal(int nId)
{
	map<int, CSemaphoreEx *>::iterator mit;

	if (nId < 0) {
		return -1;
	}

	{
		CAutoLock lock(m_mutex);

		if (m_mapSemaphore.end() == (mit = m_mapSemaphore.find(nId))) {
			return -2;
		}

		if (mit->second->post() < 0) {
			return -3;
		}
	}

	return 0;
}

int CSemaphorePool::release(int nId)
{
	map<int, CSemaphoreEx *>::iterator mit;

	{
		CAutoLock lock(m_mutex);

		if (m_mapSemaphore.end() == (mit = m_mapSemaphore.find(nId))) {
			return -1;
		}
		if (mit->second){
			delete mit->second;
		}
		m_mapSemaphore.erase(mit);
	}

	return 0;
}
	
} 
