#if !defined(SEMAPHORE_EX_H20181106)
#define SEMAPHORE_EX_H20181106

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
using namespace std;

#ifdef _WIN32
#include <windows.h>
#else //_WIN32
#include <semaphore.h>
#endif //_WIN32

namespace cbl{

class CSemaphoreEx{

public:
	CSemaphoreEx();
	CSemaphoreEx(int nMaxCount);
	virtual ~CSemaphoreEx();
	
	int create(int nMaxCount);
	int post();
	int wait();
	int wait(int nTimeout);
	int close();
	bool isNull();
	
private:
	
#ifdef _WIN32
	HANDLE m_hSem;
#else //_WIN32
	int m_nMaxCount;
	sem_t m_sem;
	bool m_bInited;
#endif //_WIN32
};
	
} //namespace cbl

#endif //SEMAPHORE_EX_H20181106
