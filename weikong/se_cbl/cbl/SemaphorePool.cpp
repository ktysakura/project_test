#include "SemaphorePool.h"
#include "AutoLock.h"
namespace cbl{

/* define macros
**/
#define DEFAULT_SEMAPHORE_POOL_SIZE 64

/* construct
**/
CSemaphorePool::CSemaphorePool(){
	m_nMaxSize = DEFAULT_SEMAPHORE_POOL_SIZE;
	//this->clear();
}

/* construct
**/
CSemaphorePool::CSemaphorePool(int nMaxSize){
	
	if (nMaxSize <= 0){
		nMaxSize = DEFAULT_SEMAPHORE_POOL_SIZE;
	}
	this->clear();
	m_nMaxSize = nMaxSize;
}

/* destruct
**/
CSemaphorePool::~CSemaphorePool(){
}

/* apply
**/
int CSemaphorePool::apply(int nId, CSemaphoreEx **ppSemaphore){
	
	CSemaphoreEx *pSemaphore = NULL;	
	pair<map<int, CSemaphoreEx *>::iterator, bool> retInsert;
	CAutoLock locker(&m_mutex);

	/* check params */
	if (nId < 0){
		return -1;
	}

	/* check max size */
	if ((int)m_mapSemaphore.size() >= m_nMaxSize){
		printf("%d > %d\n", (int)m_mapSemaphore.size(), m_nMaxSize);
		return -2;
	}

	/* new semaphore */
	if (NULL == (pSemaphore = new CSemaphoreEx())){
		return -3;
	}

	/* create semaphore */
	if (pSemaphore->create(1) < 0){
		delete pSemaphore;
		return -4;
	}

	/* insert semaphore */
	retInsert = m_mapSemaphore.insert(make_pair(nId, pSemaphore));
	if (!retInsert.second){

		delete pSemaphore;
		return -5;
	}

	/* assign variable */
	if(ppSemaphore){
		*ppSemaphore = pSemaphore;
	}

	return 0;
}
/* wait
**/
int CSemaphorePool::wait(int nId, int nTimeout){

	int nRtn,nRetCode;
	CSemaphoreEx *pSemaphore = NULL;
	map<int, CSemaphoreEx *>::iterator mit;

	/* find semaphore */
	{
		CAutoLock locker(&m_mutex);

		if (m_mapSemaphore.end() != (mit = m_mapSemaphore.find(nId))){
			pSemaphore = mit->second;
		}
	}
	
	/* apply semaphore if need */
	if(NULL == pSemaphore){
		if ((nRtn = this->apply(nId, &pSemaphore)) < 0){
			printf("CSemaphorePool apply error %d\n", nRtn);
			return -1;
		}		
	}

	/* timed wait */
	nRtn = pSemaphore->wait(nTimeout);

	/* release semaphore */
	if ((nRetCode = this->release(nId)) < 0){
		fprintf(stderr, "relase semaphore error %d\n", nRetCode);
	}

	return ((0 == nRtn) ? 0 : (nRtn - 0x10));
}

/* signal
**/
int CSemaphorePool::signal(int nId){

	map<int, CSemaphoreEx *>::iterator mit;

	/* check params */
	if (nId < 0){
		return -1;
	}

	/* signal semaphore */
	{
		CAutoLock locker(&m_mutex);

		if (m_mapSemaphore.end() == (mit = m_mapSemaphore.find(nId))){
			return -2;
		}
		if (mit->second->post() < 0){
			return -3;
		}
	}

	return 0;
}
int CSemaphorePool::clear()
{

	for (map<int, CSemaphoreEx *>::iterator mit = m_mapSemaphore.begin(); mit != m_mapSemaphore.end();)
	{
		if (mit->second){
			delete mit->second;
		}
		mit = m_mapSemaphore.erase(mit);
	}

	return 0;
}
/* release
**/
int CSemaphorePool::release(int nId){

	map<int, CSemaphoreEx *>::iterator mit;

	{
		CAutoLock locker(&m_mutex);

		if (m_mapSemaphore.end() == (mit = m_mapSemaphore.find(nId))){
			return -1;
		}
		if (mit->second){
			delete mit->second;
		}
		m_mapSemaphore.erase(mit);
	}

	return 0;
}
	
} //namespace cbl
