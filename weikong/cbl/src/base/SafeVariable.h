#if !defined(SAFE_VARIABLE_H__20180820)
#define SAFE_VARIABLE_H__20180820

#include "MutexLock.h"

namespace cbl {

template <typename T> class CSafeVariable
{
public:
	int set(const T& val);
	const T get();

	CSafeVariable();
	CSafeVariable(const T &val);
	virtual ~CSafeVariable();

private:
	T m_val;
	CMutexLock m_mutex;
};

template <typename T> CSafeVariable<T>::CSafeVariable()
{
}

template <typename T> CSafeVariable<T>::CSafeVariable(const T &val)
{
	this->set(val);
}

template <typename T> CSafeVariable<T>::~CSafeVariable()
{
}

template <typename T> int CSafeVariable<T>::set(const T &val)
{
	m_mutex.lock();
	m_val = val;
	m_mutex.unlock();
	
	return 0;
}

template <typename T> const T CSafeVariable<T>::get()
{
	m_mutex.lock();
	T val = m_val;
	m_mutex.unlock();

	return val;
}
	
} 

#endif
