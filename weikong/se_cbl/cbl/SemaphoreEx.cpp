#include "SemaphoreEx.h"
#include <time.h>

namespace cbl{

/* construct
**/
CSemaphoreEx::CSemaphoreEx(){
	
#ifdef _WIN32
	m_hSem = NULL;
#else //_WIN32
	m_bInited = false;
#endif //_WIN32	
}

/* construct
**/
CSemaphoreEx::CSemaphoreEx(int nMaxCount){
	
#ifdef _WIN32
	m_hSem = NULL;
#else //_WIN32
	m_bInited = false;
#endif //_WIN32
	
	this->create(nMaxCount);
}

/* destruct
**/
CSemaphoreEx::~CSemaphoreEx(){
	
	this->close();
}

/* create
**/
int CSemaphoreEx::create(int nMaxCount){
	
	/* close old */
	this->close();
	/* create new */
#ifdef _WIN32
	if(NULL == (m_hSem = CreateSemaphoreA(NULL, 0, max(nMaxCount, 1), NULL))){
		return -1;
	}
#else //_WIN32
	if(0 != sem_init(&m_sem, 0, 0)){
		return -1;
	}
	m_nMaxCount = max(nMaxCount, 1);
	m_bInited = true;
#endif //_WIN32

	return 0;
}

/* post
**/
int CSemaphoreEx::post(){
	
#ifdef _WIN32
	if(NULL == m_hSem){
		return -1;		
	}
	return (ReleaseSemaphore(m_hSem, 1, NULL) ? 0 : -2);
#else //_WIN32
	
	int nCount;
	
	if(!m_bInited){
		return -1;
	}
	
	if(0 != sem_getvalue(&m_sem, &nCount)){
		return -2;
	}
	
	if(nCount >= m_nMaxCount){
		return 0;
	}
	
	return ((0 == sem_post(&m_sem)) ? 0 : -2);
#endif //_WIN32
}

/* wait
**/
int CSemaphoreEx::wait(){
	
#ifdef _WIN32
	if(NULL == m_hSem){
		return -1;		
	}
	
	return ((WAIT_OBJECT_0 == WaitForSingleObject(m_hSem, INFINITE)) ? 0 : -2);
#else //_WIN32
	if(!m_bInited){
		return -1;
	}
	
	return ((0 == sem_wait(&m_sem)) ? 0 : -2);
#endif //_WIN32
	return 0;
}

/* wait
**/
int CSemaphoreEx::wait(int nTimeout){

#ifdef _WIN32
	if (NULL == m_hSem){
		return -1;
	}

	return ((WAIT_OBJECT_0 == WaitForSingleObject(m_hSem, nTimeout)) ? 0 : -2);
#else //_WIN32

	struct timespec ts;

	/* check whether inited */
	if (!m_bInited){
		return -1;
	}

	/* get time */
	if (clock_gettime(CLOCK_REALTIME, &ts) < 0){
		return -2;
	}

	/* get abs timeout */
	ts.tv_sec += nTimeout / 1000;
	ts.tv_nsec += (nTimeout % 1000) * 1000000;
	if(ts.tv_nsec > 1000000000){
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}

	return ((0 == sem_timedwait(&m_sem, &ts)) ? 0 : -3);
#endif //_WIN32
}

/* close
**/
int CSemaphoreEx::close(){
	
#ifdef _WIN32
	if(m_hSem){
		CloseHandle(m_hSem);
		m_hSem = NULL;
	}
#else //_WIN32
	if(m_bInited){
		sem_destroy(&m_sem);
		m_bInited = false;
	}
#endif //_WIN32

	return 0;
}

/* is NULL
**/
bool CSemaphoreEx::isNull(){
	
#ifdef _WIN32
	return (NULL == m_hSem);
#else //_WIN32
	return (!m_bInited);
#endif //_WIN32
}
	
} //namespace cbl
