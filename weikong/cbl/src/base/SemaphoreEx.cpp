#include "SemaphoreEx.h"
#include <time.h>

namespace cbl {

CSemaphoreEx::CSemaphoreEx()
{
	m_hSem = NULL;
}

CSemaphoreEx::CSemaphoreEx(int nMaxCount)
{
	m_hSem = NULL;
	this->create(nMaxCount);
}

CSemaphoreEx::~CSemaphoreEx()
{
	this->close();
}

int CSemaphoreEx::create(int nMaxCount)
{
	this->close();
	
	if (NULL == (m_hSem = CreateSemaphore(NULL, 0, max(nMaxCount, 1), NULL))) {
		return -1;
	}

	return 0;
}

int CSemaphoreEx::post()
{
	if (NULL == m_hSem) {
		return -1;		
	}
	return (ReleaseSemaphore(m_hSem, 1, NULL) ? 0 : -2);
}

int CSemaphoreEx::wait()
{
	if (NULL == m_hSem) {
		return -1;		
	}
	
	return ((WAIT_OBJECT_0 == WaitForSingleObject(m_hSem, INFINITE)) ? 0 : -2);
}

int CSemaphoreEx::wait(int nTimeout)
{
	if (NULL == m_hSem) {
		return -1;
	}

	return ((WAIT_OBJECT_0 == WaitForSingleObject(m_hSem, nTimeout)) ? 0 : -2);
}

int CSemaphoreEx::close()
{
	if (m_hSem) {
		CloseHandle(m_hSem);
		m_hSem = NULL;
	}

	return 0;
}

bool CSemaphoreEx::isNull()
{
	return (NULL == m_hSem);
}
	
}
