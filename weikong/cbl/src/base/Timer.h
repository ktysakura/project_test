#if !defined(TIMER_H__20190419)
#define TIMER_H__20190419

#include <stdio.h>
#include <stdlib.h>
#include "SemaphoreEx.h"
#include "SafeVariable.h"

namespace cbl {

//定时器的回调函数
typedef int (*PFONTIMER)(int nId, void *parameter);

class CTimer {
public:
	CTimer();
	virtual ~CTimer();
	
	int create(int nId, int nInterval, PFONTIMER pfOnTimer, void *parameter);
	int kill();
	bool isRunning();

private:
	
	friend DWORD WINAPI timerProc(void *parameter);
	
private:
	CSemaphoreEx m_semTimer;
	CSemaphoreEx m_semThread;
	CSafeVariable<bool> m_bRunning;
	
	int m_nId;	
	int m_nInterval;
	PFONTIMER m_pfOnTimer;
	void *m_parameter;
};
	
} 

#endif
