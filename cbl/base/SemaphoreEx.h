#if !defined(SEMAPHORE_EX_H20181106)
#define SEMAPHORE_EX_H20181106

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "windows.h"

using namespace std;

namespace cbl {

class CSemaphoreEx {

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
	HANDLE m_hSem;
};
	
} 

#endif 