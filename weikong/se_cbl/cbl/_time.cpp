#include "_time.h"

#include <sys/timeb.h>
long long getTimeStamp()
{
#ifdef _WIN32

	struct timeb rawtime;
	ftime(&rawtime);
	return rawtime.time * 1000 + rawtime.millitm;

#else //WIN32

	//struct timeval tv;
	//gettimeofday(&tv, NULL);    //linux�¸ú�����sys/time.hͷ�ļ���  
	//return tv.tv_sec * 1000 + tv.tv_usec / 1000;

#endif//LINUX
}
