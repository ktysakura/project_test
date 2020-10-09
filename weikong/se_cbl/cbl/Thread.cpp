#include "Thread.h"
#include <process.h>

namespace cbl{
static DWORD current_thread_data_tls_index = TLS_OUT_OF_INDEXES;

static void create_tls()
{
	if (current_thread_data_tls_index != TLS_OUT_OF_INDEXES)
		return;
	static CMutexLock mutex;
	CAutoLock locker(&mutex);
	current_thread_data_tls_index = TlsAlloc();
}

static void free_tls()
{
	if (current_thread_data_tls_index != TLS_OUT_OF_INDEXES) {
		TlsFree(current_thread_data_tls_index);
		current_thread_data_tls_index = TLS_OUT_OF_INDEXES;
	}
}

unsigned int __stdcall CThread::on_thread_proc(void *arg)
{
	CThread *thr = reinterpret_cast<CThread *>(arg);
	int res = 0;
	res = thr->started();
	if (res < 0) {
		return res;
	}
	thr->run();
	on_thread_finish(arg);
	return 0;
}
CThread::CThread()
	:m_running(false), m_isInFinish(false), m_finished(false),
	m_exited(false), m_returnCode(-1), m_waiters(0), m_stackSize(0), 
	m_waitThread(false), m_priority(CThread::InheritPriority)
{


}
CThread::~CThread()
{
	CAutoLock locker(&m_mutex);

	if (m_isInFinish) {
		locker.unlock();
		join();
		locker.relock();
	}

	if (m_running && !m_finished) {
		printf("CThread: Destroyed while thread is still running");
	}
}

void CThread::start(Priority priority)
{
	CAutoLock locker(&m_mutex);

	if (m_isInFinish) {
		locker.unlock();
		join();
		locker.relock();
	}

	if (m_running) {
		return;
	}

	m_running = true;
	m_finished = false;
	m_terminated = false;
	m_exited = false;
	m_returnCode = 0;
	
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hEvent) {
		m_running = false;
		m_finished = true;
		return;
	}

	m_hThread = (HANDLE)_beginthreadex(NULL, m_stackSize, on_thread_proc, this, CREATE_SUSPENDED, &m_id);
	if (!m_hThread) {
		CloseHandle(m_hEvent);
		m_running = false;
		m_finished = true;
		return;
	}

	int prio;
	switch (m_priority) {
	case IdlePriority:
		prio = THREAD_PRIORITY_IDLE;
		break;
	case LowestPriority:
		prio = THREAD_PRIORITY_LOWEST;
		break;
	case LowPriority:
		prio = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case NormalPriority:
		prio = THREAD_PRIORITY_NORMAL;
		break;
	case HighPriority:
		prio = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case HighestPriority:
		prio = THREAD_PRIORITY_HIGHEST;
		break;
	case TimeCriticalPriority:
		prio = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	case InheritPriority:
	default:
		prio = GetThreadPriority(GetCurrentThread());
		break;
	}

	if (!SetThreadPriority(m_hThread, prio)) {
		printf("thread::start: Failed to set thread priority\n");
	}

	if (!ResumeThread(m_hThread)) {
		printf("thread::start: Failed to resume new thread\n");
	}
}

bool CThread::join(unsigned long time)
{
	CAutoLock locker(&m_mutex);

	if (m_id == GetCurrentThreadId()) {
		printf("Thread::join: Thread tried to wait on itself");
		return false;
	}

	if (m_finished || !m_running) {
		return true;
	}

	++m_waiters;
	locker.unlock();

	bool ret = false;
	switch (WaitForSingleObject(m_hThread, time)) {
	case WAIT_OBJECT_0:
		ret = true;
		break;
	case WAIT_FAILED:
		printf("Thread::join: thread wait failure\n");
		break;
	case WAIT_TIMEOUT:
		printf("Thread::join: thread wait timeout\n");
		break;
	case WAIT_ABANDONED:
	
	default:
		break;
	}

	locker.relock();
	--m_waiters;

	if (ret && !m_finished) {
		// thread was terminated by someone else
		m_terminated = true;
		on_thread_finish(this, false);
	}

	if (m_finished && !m_waiters) {
		CloseHandle(m_hThread);
		m_hThread = 0;
	}

	return ret;
}

bool CThread::wait(unsigned long time)
{
	CAutoLock locker(&m_mutex);
	if (m_id != GetCurrentThreadId()) {
		printf("Thread::wait: Thread tried to wait on others' threads");
		return false;
	}

	if (!m_running || m_isInFinish) {
		return false;
	}
	
	if (!m_waitThread) {
		return true;
	}
	locker.unlock();

	bool ret = false;
	switch (WaitForSingleObject(m_hEvent, time)) {
	case WAIT_OBJECT_0:
		ret = true;
		break;
	case WAIT_FAILED:
		printf("Thread::wait: Thread wait failure");
		break;
	case WAIT_ABANDONED:
	case WAIT_TIMEOUT:
	default:
		break;
	}
	locker.relock();
	
	return ret;
}

void CThread::setWait()
{
	CAutoLock locker(&m_mutex);
	if (!m_running || m_isInFinish) {
		return;
	}
	m_waitThread = true;
}

bool CThread::notify()
{
	CAutoLock locker(&m_mutex);
	
	if (GetCurrentThreadId() == m_id) {
		return false;
	}

	if (!m_running || m_isInFinish || false == m_waitThread) {
		return false;
	}
	m_waitThread = false;
	return SetEvent(m_hEvent) == TRUE;
}

void CThread::on_thread_finish(void *arg, bool lockAnyway)
{
	CThread *thr = reinterpret_cast<CThread *>(arg);
	CAutoLock locker(lockAnyway ? &thr->m_mutex : 0);

	thr->m_isInFinish = true;
	thr->m_priority = CThread::InheritPriority;
	locker.unlock();
	thr->finished();
	locker.relock();
	thr->m_terminated = false;
	thr->m_running = false;
	thr->m_finished = true;
	thr->m_isInFinish = false;
	CloseHandle(thr->m_hEvent);
	thr->m_hEvent = 0;
	if (!thr->m_waiters) {
		CloseHandle(thr->m_hThread);
		thr->m_hThread = 0;
	}
}

bool CThread::isFinished() const
{
	CAutoLock locker(&m_mutex);
	return m_finished || m_isInFinish;
}

bool CThread::isRunning() const
{
	CAutoLock locker(&m_mutex);
	return m_running && !m_isInFinish;
}

bool CThread::isWaiting() const
{
	CAutoLock locker(&m_mutex);

	if (!m_running || m_isInFinish) {
		return false;
	}

	return m_waitThread;
}

void CThread::exit(int retcode)
{

}

}