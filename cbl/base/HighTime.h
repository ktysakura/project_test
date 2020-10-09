#if !defined(HIGH_TIME_H__20181011)
#define HIGH_TIME_H__20181011

#include <Windows.h>

namespace cbl {

class CHighTime
{
public:
	CHighTime();
	~CHighTime();

	//
	double getTimeUs();
	long long getTimestamp();
	int gettimeofday(struct timeval *tv, struct timezone *tz);

private:
	LARGE_INTEGER m_freq;
};
	
}

#endif
