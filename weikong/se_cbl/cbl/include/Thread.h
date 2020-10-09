#ifndef _THREAD_20200103_
#define _THREAD_20200103_
#include "AutoLock.h"
namespace cbl{
#define THREAD_EXIT_TIME 3000
//class CThreadImpl;
class CThread {
public:
	enum Priority {
		IdlePriority,
		LowestPriority,
		LowPriority,
		NormalPriority,
		HighPriority,
		HighestPriority,
		TimeCriticalPriority,
		InheritPriority
	};
public:
	//static int currentThreadId();
	//void setPriority(Priority priority);
	//Priority priority() const;
	CThread();
	virtual ~CThread();
	bool isFinished() const;
	bool isRunning() const;
	bool isWaiting() const;
	void exit(int retcode = 0);
	bool wait(unsigned long time = -1);
	bool notify();
	bool join(unsigned long time = THREAD_EXIT_TIME);
	void start(Priority = InheritPriority);
public:
	void setWait();
protected:
	virtual int started() = 0;
	virtual void run() = 0;
	virtual void finished() = 0;
private:

#ifdef _WIN32
	static unsigned int __stdcall on_thread_proc(void *arg);
	static void on_thread_finish(void *arg, bool lockAnyway = true);
	HANDLE m_hThread;
	HANDLE m_hEvent;
#else

#endif
protected:
	bool m_waitThread;
private:
	mutable CMutexLock m_mutex;
	bool m_running;
	bool m_finished;
	bool m_terminated;
	bool m_isInFinish;             //when in QThreadPrivate::finish
	bool m_exited;
	int m_returnCode;
	int m_waiters;
	unsigned int m_id;
	unsigned int m_stackSize;
	Priority m_priority;
	//CThreadImpl *p;
};
}

#endif
