#pragma warning(disable:4996)
#if !defined (LOG_FILE_H__20160312)
#define LOG_FILE_H__20160312

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "MutexLock.h"
using namespace std;
namespace cbl{
/* define error type
 */
typedef enum{
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_MAX,
}LOG_LEVEL_T;

class CLogFile
{
public:
	static CLogFile *getInstance();
	static int delInstance();
	
	int setLogDir(const string& sDir);
	int log(LOG_LEVEL_T level, const char *format, ...);
	int logi(const char *format, ...);
	int logw(const char *format, ...);
	int loge(const char *format, ...);

private:
	CLogFile();
	virtual ~CLogFile();
	int log(LOG_LEVEL_T level, const char *format, va_list& vl);

private:
	static CLogFile *_instance;
	string m_sDir;
	CMutexLock m_mutex;
};
}
#endif //LOG_FILE_H__20160312
