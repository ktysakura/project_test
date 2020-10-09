#include "HighTime.h"

namespace cbl {

CHighTime::CHighTime()
{
	QueryPerformanceFrequency(&m_freq);
}

CHighTime::~CHighTime()
{
}

double CHighTime::getTimeUs()
{
	LARGE_INTEGER time;

	QueryPerformanceCounter(&time);
	return double(time.QuadPart) * 1000000 / m_freq.QuadPart;
}

long long CHighTime::getTimestamp()
{
	SYSTEMTIME systemtime;
	FILETIME filetime;
	ULARGE_INTEGER ullValue;

	::GetSystemTime(&systemtime);
	::SystemTimeToFileTime(&systemtime, &filetime);
	ullValue.LowPart = filetime.dwLowDateTime;
	ullValue.HighPart = filetime.dwHighDateTime;

	return (ullValue.QuadPart - 116444736000000000) / 10000000;	
}

int CHighTime::gettimeofday(struct timeval *tv, struct timezone *tz)
{
	long long llTimestamp;

	if (NULL == tv) {
		return -1;
	}

	llTimestamp = getTimestamp();
	tv->tv_sec = long(llTimestamp / 1000000);
	tv->tv_usec = long(llTimestamp % 1000000);

	return 0;
}
	
}
