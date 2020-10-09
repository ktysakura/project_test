#include "Timer.h"

namespace cbl {

typedef DWORD (WINAPI *PFUNC_THREAD_PROC)(void *lpParameter);

int createDetachThread(PFUNC_THREAD_PROC pfunc, void *lpParameter)
{
	HANDLE handle = NULL;

	if (NULL == pfunc) {
		return -1;
	}

	if (NULL == (handle = CreateThread(NULL, 0, pfunc, lpParameter, 0, NULL))) {
		SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
		return -2;
	}

	CloseHandle(handle);
	return 0;
}

DWORD WINAPI timerProc(void *lpParameter)
{
	CTimer *pParent = NULL;
	
	if (NULL == (pParent = (CTimer *)lpParameter)) {
		return 0;
	}
	
	pParent->m_semThread.post();
	
	while (true) {
		if (0 == pParent->m_semTimer.wait(pParent->m_nInterval)) {
			break;
		}

		if (pParent->m_pfOnTimer) {
			pParent->m_pfOnTimer(pParent->m_nId, pParent->m_parameter);
		}
	}	

	pParent->m_bRunning.set(false);
	
	return 0;
}


CTimer::CTimer()
{
	m_bRunning.set(false);
	m_semTimer.create(1);
	m_semThread.create(1);
}

CTimer::~CTimer()
{
	this->kill();
}

int CTimer::create(int nId, int nInterval, PFONTIMER pfOnTimer, void *parameter)
{
	if ((nId < 0) || (nInterval <= 0) || (NULL == pfOnTimer)) {
		return -1;
	}
	
	this->kill();
	
	m_nId = nId;
	m_nInterval = nInterval;
	m_pfOnTimer = pfOnTimer;
	m_parameter = parameter;
	
	if (createDetachThread(timerProc, this) < 0) {
		return -2;
	}
	
	if (m_semThread.wait(100) < 0) {
		return -3;
	}
	
	m_bRunning.set(true);
	return 0;
}

int CTimer::kill()
{
	if (m_bRunning.get()) {
		m_semTimer.post();
	}
		
	return 0;
}

bool CTimer::isRunning()
{
	return m_bRunning.get();
}

}
